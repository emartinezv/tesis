/* Copyright 2016, Ezequiel Martinez Vazquez
 * All rights reserved.
 *
 * This file is part of Workspace.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** @brief This module handles the user interface
 */

/** \addtogroup interface interface
 ** @{ */

/*==================[inclusions]=============================================*/

#include "ciaaMobile_interface.h"

/*==================[macros and definitions]=================================*/

#define DEBUG_INTERF // debug mode
#ifdef DEBUG_INTERF
   #define debug(msg) dbgPrint(msg)
#else
   #define debug(msg)
#endif

/*==================[global data]============================================*/

/*==================[internal data declaration]==============================*/

/** @brief State of the current formula being run
 */

static frmStatus frmState = IDLE;

/** @brief Function pointer to formula function for the current formula
 */

static void (*frm) (void);

/** @brief Pointer to inputs for the current formula being run
 */

static void * frmInput;

/** @brief Pointer to outputs for the current formula being run
 */

static void * frmOutput;

/** @brief Function pointer to callback for the current formula being run
 */

static void * (*frmCback) (error_user, void *);

/** @brief used for sysUpdate counter */

static int32_t sysupd_count = DELAY_SYSUPD;

/*==================[internal functions declaration]=========================*/

/** @brief Formula to send an SMS
 */

static void ciaaMobile_sendSMS_f (void);

/** @brief Formula to list received SMSs
 */

static void ciaaMobile_listRecSMS_f (void);

/** @brief Formula to delete an SMS from memory
 */

static void ciaaMobile_delSMS_f (void);

/** @brief Formula to start up the GSM engine
 */

static void ciaaMobile_startUp_f (void);

/** @brief Formula to start up GPRS connection
 */

static void ciaaMobile_startGPRS_f (void);

/** @brief Formula to open a TCP port
 */

static void ciaaMobile_openTCP_f (void);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void ciaaMobile_sendSMS_f (void)
{
   static runStatus runState = NOCMD;
   static error_user error_out;

   /* smsCmd is "AT+CMGS="XXX"\r" where XXX is the phone number. Maximum phone
    * number length under the ITU-T standard E.164 is 15 digits plus the "+"
      symbol if present. */

   static uint8_t smsCmd[9+16+3];

   /* Maximum SMS size according to 3GPP standard TS 23.038 is 160 chars using
    * GSM 7-bit alphabet */

   static uint8_t smsText[160+1];

   FSMresult result;

   switch(frmState) {

      case INIT:

         smsCmd[0] = '\0';
         smsText[0] = '\0';

         error_out.error_formula = OK;
         error_out.error_command.command[0] = '\0';
         error_out.error_command.parameter[0] = '\0';

         /* VERIFICAR SI EL NUMERO TIENE ALGO MAS QUE + Y DIGITOS*/
         /* VERIFICAR SI EL CUERPO TIENE LF, CR O ALGO QUE PUEDA JODER */

         /*sprintf(dest, "\"%s\"", ((SMS_send *)msg)->dest);   VER SI SE PUEDE INCORPORAR MAS ADELANTE */
         strncat(smsCmd, "AT+CMGS=\"", 9);
         strncat(smsCmd, ((SMS_send *)frmInput)->dest,strlen(((SMS_send *)frmInput)->dest));
         strncat(smsCmd, "\"\r", 2);

         strncpy(smsText, ((SMS_send *)frmInput)->text, strlen(((SMS_send *)frmInput)->text));
         smsText[strlen(((SMS_send *)frmInput)->text)] = '\0';

         runState = ATCMD1;
         frmState = PROC;

         debug(">>>interf<<<   COMANDO ENVIO SMS: ");
         debug(smsCmd);
         debug("\r\n>>>interf<<<   TEXTO SMS: ");
         debug(smsText);
         debug("\r\n");

         break;

      case PROC:

         switch(runState){

            case ATCMD1:

               result = sendATcmd(smsCmd);
               if(OK_CMD_SENT == result){runState = ATCMD1RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){runState = ATCMD2;}
                  else if(ERR_MSG_CLOSE == result){{error_out.error_formula = ERR_GSM; frmState = WRAP;};}
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD2:

               result = sendATcmd(smsText);
               if(OK_CMD_SENT == result){runState = ATCMD2RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD2RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){frmState = WRAP;}
                  else if(ERR_MSG_CLOSE == result){{error_out.error_formula = ERR_GSM; frmState = WRAP;};}
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(ERR_GSM == error_out.error_formula){

            ATresp resp;

            resp = getCmdResp(getNoCmdResp()-1);
            strncpy(error_out.error_command.command, resp.cmd, 19);
            error_out.error_command.command[20] = '\0';
            strncpy(error_out.error_command.parameter, resp.param, 149);
            error_out.error_command.parameter[150] = '\0';

         }

         frmCback(error_out, 0);
         frmState = IDLE;
         break;
   }

   return;

}

static void ciaaMobile_listRecSMS_f (void)
{
   static runStatus runState = NOCMD;
   static error_user error_out;

   FSMresult result;

   switch(frmState) {

      case INIT:

         error_out.error_formula = OK;
         error_out.error_command.command[0] = '\0';
         error_out.error_command.parameter[0] = '\0';

         debug(">>>interf<<<   Leyendo SMS...\r\n");

         runState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(runState){

            case ATCMD1:

               result = sendATcmd("AT+CMGL=\"ALL\"\r");
               if(OK_CMD_SENT == result){runState = ATCMD1RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){frmState = WRAP;}
                  else if(ERR_MSG_CLOSE == result){{error_out.error_formula = ERR_GSM; frmState = WRAP;};}
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(OK == error_out.error_formula){

            uint8_t i = 0;
            uint8_t respNo;
            uint8_t auxtext[4]; // ERASE LATER, FOR DEBUG ONLY

            ATresp resp;
            SMS_rec * target = (SMS_rec *)frmOutput;
            respNo = getNoCmdResp();

            // DEBUG ERASE LATER

            debug(">>>interf<<<   TAMAÑO DEL VECTOR: ");
            itoa(target->meta[0], auxtext, 10);
            debug(auxtext);
            debug("\r\n");
            debug(">>>interf<<<   CANTIDAD DE SMS: ");
            itoa((respNo-1)/2, auxtext, 10);
            debug(auxtext);
            debug("\r\n");

            // DEBUG ERASE LATER

            if(target->meta[0] < ((respNo-1)/2)){
               error_out.error_formula = ERR_WRAP;
               debug(">>>interf<<<   No hay suficiente espacio para los mensajes!\r\n");
            }

            else {

               if(1 == respNo){debug(">>>interf<<<   No hay SMSs!\r\n");}

               else{

                  for(i = 0; i < (respNo-1)/2; i++){

                     resp = getCmdResp(2*i);
                     strncpy((target+i)->meta, resp.param, 149);
                     (target+i)->meta[149] = '\0';

                     resp = getCmdResp((2*i)+1);
                     strncpy((target+i)->text, resp.param, 149);
                     (target+i)->text[149] = '\0';
                  }

               }

            }

            (target+i)->meta[0] = '\0';

            frmCback(error_out, frmOutput);

            debug(">>>interf<<<   Lectura de SMS concluida!\r\n");

            frmState = IDLE;

         }

         else{

            ATresp resp;

            resp = getCmdResp(getNoCmdResp()-1);
            strncpy(error_out.error_command.command, resp.cmd, 19);
            error_out.error_command.command[20] = '\0';
            strncpy(error_out.error_command.parameter, resp.param, 149);
            error_out.error_command.parameter[150] = '\0';

            frmCback(error_out, frmOutput);

            debug(">>>interf<<<   Lectura de SMS concluida!\r\n");

            frmState = IDLE;

         }

         break;
   }

   return;

}

static void ciaaMobile_delSMS_f (void)
{
   static runStatus runState = NOCMD;
   static error_user error_out;

   static uint8_t aux[5]; /* aux buffer */
   static uint8_t smsDel[20];  /* holds the str of the sms del cmd including paramenters */

   FSMresult result;

   switch(frmState) {

      case INIT:

         aux[0] = '\0';
         smsDel[0] = '\0';

         error_out.error_formula = OK;
         error_out.error_command.command[0] = '\0';
         error_out.error_command.parameter[0] = '\0';

         strncat(smsDel, "AT+CMGD=", 8);
         itoa(((SMS_del *)frmInput)->index, aux, 10);
         strncat(smsDel, aux, strlen(aux));
         strncat(smsDel, ",", 1);
         itoa(((SMS_del *)frmInput)->mode, aux, 10);
         strncat(smsDel, aux, strlen(aux));
         strncat(smsDel, "\r", 1);

         runState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(runState){

            case ATCMD1:

               result = sendATcmd(smsDel);
               if(OK_CMD_SENT == result){runState = ATCMD1RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){frmState = WRAP;}
                  else if(ERR_MSG_CLOSE == result){{error_out.error_formula = ERR_GSM; frmState = WRAP;};}
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(ERR_GSM == error_out.error_formula){

            ATresp resp;

            resp = getCmdResp(getNoCmdResp()-1);
            strncpy(error_out.error_command.command, resp.cmd, 19);
            error_out.error_command.command[20] = '\0';
            strncpy(error_out.error_command.parameter, resp.param, 149);
            error_out.error_command.parameter[150] = '\0';

         }

         frmCback(error_out, 0);
         frmState = IDLE;
         break;
   }

   return;
}

static void ciaaMobile_startUp_f (void)
{
   static runStatus runState = NOCMD;
   static error_user error_out;

   FSMresult result;

   switch(frmState) {

      case INIT:

         error_out.error_formula = OK;
         error_out.error_command.command[0] = '\0';
         error_out.error_command.parameter[0] = '\0';

         debug(">>>interf<<<   Inicializando ciaaMobile...\r\n");

         runState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(runState){

            case ATCMD1:

               result = sendATcmd("AT\r");
               if(OK_CMD_SENT == result){runState = ATCMD1RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){runState = ATCMD2;}
                  else if(ERR_MSG_CLOSE == result){{error_out.error_formula = ERR_GSM; frmState = WRAP;};}
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD2:

               result = sendATcmd("AT+CMEE=2\r");
               if(OK_CMD_SENT == result){runState = ATCMD2RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD2RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){runState = ATCMD3;}
                  else if(ERR_MSG_CLOSE == result){{error_out.error_formula = ERR_GSM; frmState = WRAP;};}
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD3:

               result = sendATcmd("AT+CSCS=\"GSM\"\r");
               if(OK_CMD_SENT == result){runState = ATCMD3RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD3RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){runState = ATCMD4;}
                  else if(ERR_MSG_CLOSE == result){{error_out.error_formula = ERR_GSM; frmState = WRAP;};}
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD4:

               result = sendATcmd("AT+CMGF=1\r");
               if(OK_CMD_SENT == result){runState = ATCMD4RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD4RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){frmState = WRAP;}
                  else if(ERR_MSG_CLOSE == result){{error_out.error_formula = ERR_GSM; frmState = WRAP;};}
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(ERR_GSM == error_out.error_formula){

            ATresp resp;

            resp = getCmdResp(getNoCmdResp()-1);
            strncpy(error_out.error_command.command, resp.cmd, 19);
            error_out.error_command.command[20] = '\0';
            strncpy(error_out.error_command.parameter, resp.param, 149);
            error_out.error_command.parameter[150] = '\0';

         }

         frmCback(error_out, 0);

         debug(">>>interf<<<   ciaaMobile inicializado!\r\n");

         frmState = IDLE;
         break;
   }

   return;

}

static void ciaaMobile_startGPRS_f (void)
{
   static runStatus runState = NOCMD;
   static error_user error_out;

   static uint8_t APNstring[100];  /* holds the str for the AT+CSTT command */

   FSMresult result;

   switch(frmState) {

      case INIT:

         APNstring[0] = '\0';

         error_out.error_formula = OK;
         error_out.error_command.command[0] = '\0';
         error_out.error_command.parameter[0] = '\0';

         strncat(APNstring, "AT+CSTT=\"", 9);
         strncat(APNstring, ((APN_usr_pwd *)frmInput)->apn, 30);
         strncat(APNstring, "\",\"", 3);
         strncat(APNstring, ((APN_usr_pwd *)frmInput)->user, 30);
         strncat(APNstring, "\",\"", 3);
         strncat(APNstring, ((APN_usr_pwd *)frmInput)->pwd, 30);
         strncat(APNstring, "\"\r", 2);

         debug(APNstring);  // SACAR MAS TARDE
         debug("\r\n");

         runState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(runState){

            case ATCMD1:

               result = sendATcmd("AT+CGATT=1\r");
               if(OK_CMD_SENT == result){runState = ATCMD1RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){runState = ATCMD2;}
                  else if(ERR_MSG_CLOSE == result){{error_out.error_formula = ERR_GSM; frmState = WRAP;};}
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD2:

               result = sendATcmd(APNstring);
               if(OK_CMD_SENT == result){runState = ATCMD2RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD2RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){runState = ATCMD3;}
                  else if(ERR_MSG_CLOSE == result){{error_out.error_formula = ERR_GSM; frmState = WRAP;};}
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD3:

               result = sendATcmd("AT+CIICR\r");
               if(OK_CMD_SENT == result){runState = ATCMD3RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD3RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){frmState = WRAP;}
                  else if(ERR_MSG_CLOSE == result){{error_out.error_formula = ERR_GSM; frmState = WRAP;};}
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(ERR_GSM == error_out.error_formula){

            ATresp resp;

            resp = getCmdResp(getNoCmdResp()-1);
            strncpy(error_out.error_command.command, resp.cmd, 19);
            error_out.error_command.command[20] = '\0';
            strncpy(error_out.error_command.parameter, resp.param, 149);
            error_out.error_command.parameter[150] = '\0';

         }

         frmCback(error_out, 0);
         frmState = IDLE;
         break;
   }

   return;
}

static void ciaaMobile_openTCP_f (void)
{
   static runStatus runState = NOCMD;
   static error_user error_out;

   static uint8_t addr_port_string[100];  /* str for the AT+CIPSTART command */

   FSMresult result;

   switch(frmState) {

      case INIT:

         addr_port_string[0] = '\0';

         error_out.error_formula = OK;
         error_out.error_command.command[0] = '\0';
         error_out.error_command.parameter[0] = '\0';

         strncat(addr_port_string, "AT+CIPSTART=\"TCP\",\"", 19);
         strncat(addr_port_string, ((TCP_addr_port *)frmInput)->address, 100);
         strncat(addr_port_string, "\",\"", 3);
         strncat(addr_port_string, ((TCP_addr_port *)frmInput)->port, 6);
         strncat(addr_port_string, "\"\r", 2);

         debug(addr_port_string);  // SACAR MAS TARDE
         debug("\r\n");

         runState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(runState){

            case ATCMD1:

               result = sendATcmd("AT+CIPSHUT\r");
               if(OK_CMD_SENT == result){runState = ATCMD1RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){runState = ATCMD2;}
                  else if(ERR_MSG_CLOSE == result){{error_out.error_formula = ERR_GSM; frmState = WRAP;};}
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD2:

               result = sendATcmd(addr_port_string);
               if(OK_CMD_SENT == result){runState = ATCMD2RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD2RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){frmState = WRAP;}
                  else if(ERR_MSG_CLOSE == result){{error_out.error_formula = ERR_GSM; frmState = WRAP;};}
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(ERR_GSM == error_out.error_formula){

            ATresp resp;

            resp = getCmdResp(getNoCmdResp()-1);
            strncpy(error_out.error_command.command, resp.cmd, 19);
            error_out.error_command.command[20] = '\0';
            strncpy(error_out.error_command.parameter, resp.param, 149);
            error_out.error_command.parameter[150] = '\0';

         }

         frmCback(error_out, 0);
         frmState = IDLE;
         break;
   }

   return;
}

/*==================[external functions definition]==========================*/

void ciaaMobile_SysTick_Handler (void)
{
   if(timeout_count > 0) timeout_count--;
   if(sysupd_count > 0) sysupd_count--;

   return;
}

void ciaaMobile_sendSMS (SMS_send * msg, void * (*cback) (error_user, void *))
{
   frm = ciaaMobile_sendSMS_f;
   frmInput = msg;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_listRecSMS (SMS_rec * list, uint8_t noMsg, void * (*cback) (error_user, void *))
{
   frm = ciaaMobile_listRecSMS_f;
   list->meta[0] = noMsg; /* we hide the noMsg integer inside the list vector */
   frmOutput = list;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_delSMS (SMS_del * msgdel, void * (*cback) (error_user, void *))
{
   frm = ciaaMobile_delSMS_f;
   frmInput = msgdel;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_startGPRS (APN_usr_pwd * APN, void * (*cback) (error_user, void *))
{
   frm = ciaaMobile_startGPRS_f;
   frmInput = APN;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_openTCP (TCP_addr_port * addr_port, void * (*cback) (error_user, void *))
{
   frm = ciaaMobile_openTCP_f;
   frmInput = addr_port;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_startUp (void * (*cback) (error_user, void *))
{
   frm = ciaaMobile_startUp_f;
   frmCback = cback;
   frmState = INIT;

   return;
}

/** This function needs to be called periodically to process commands and
 *  their responses.
 */

void ciaaMobile_sysUpdate (void)
{
   if(0 == sysupd_count){

      if (IDLE == frmState){
         processToken();
      }
      else {frm();}

      sysupd_count = DELAY_SYSUPD;

      return;

   }

   else{

      return;

   }

}

uint8_t ciaaMobile_isIdle (void)
{
   return(frmState == IDLE);
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
