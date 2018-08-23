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

#include "gsmInterface.h"

/*==================[macros and definitions]=================================*/

#define DEBUG_INTERF
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

/** @brief Formula to open a TCP or UDP port
 */

static void ciaaMobile_openPort_f (void);

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

         if(OK != error_out.error_formula){
            if(ERR_GSM == error_out.error_formula){

               ATresp resp;

               resp = getCmdResp();
               strncpy(error_out.error_command.command, resp.cmd, 19);
               error_out.error_command.command[20] = '\0';
               strncpy(error_out.error_command.parameter, resp.param, 149);
               error_out.error_command.parameter[150] = '\0';

            }
         }

         frmCback(error_out, 0);
         frmState = IDLE;
         break;
   }

   return;

}

static void ciaaMobile_readRecSMS_f (void)
{
   static runStatus runState = NOCMD;
   static error_user error_out;

   FSMresult result;

   static uint8_t strSmsRead[15]; /* string to assemble the SMS read command*/
   uint8_t strAux[5];      /* auxiliary string */

   switch(frmState) {

      case INIT:

         error_out.error_formula = OK;
         error_out.error_command.command[0] = '\0';
         error_out.error_command.parameter[0] = '\0';

         debug(">>>interf<<<   Leyendo SMS...\r\n");

         runState = ATCMD1;
         frmState = PROC;

         strSmsRead[0] = '\0';

         strncat(strSmsRead, "AT+CMGR=", 8);
         itoa(((SMS_rd_params *)frmInput)->index, strAux, 10);
         strncat(strSmsRead, strAux, strlen(strAux));
         strncat(strSmsRead, ",", 1);
         itoa(((SMS_rd_params *)frmInput)->mode, strAux, 10);
         strncat(strSmsRead, strAux, strlen(strAux));
         strncat(strSmsRead, "\r", 1);

         debug(">>>interf<<<   strSmsRead: ");
         debug(strSmsRead);
         debug("\r\n");

         break;

      case PROC:

         switch(runState){

            case ATCMD1:

               result = sendATcmd("AT+CSDH=1\r");
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

               result = sendATcmd(strSmsRead);
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

         if(OK == error_out.error_formula){

            ATresp resp;

            resp = getCmdResp();
            strncpy(((SMS_rec *)frmOutput)->meta, resp.param, strlen(resp.param));
            ((SMS_rec *)frmOutput)->meta[strlen(resp.param)] = '\0';

            resp = getCmdResp();
            strncpy(((SMS_rec *)frmOutput)->text, resp.param, strlen(resp.param));
            ((SMS_rec *)frmOutput)->text[strlen(resp.param)] = '\0';

            SMS_print printout;

            printout.noMsg = 1;
            printout.firstMsg = frmOutput;

            frmCback(error_out, &printout);

            debug(">>>interf<<<   Lectura de SMS concluida!\r\n");

            frmState = IDLE;

         }

         else{

            ATresp resp;

            resp = getCmdResp();
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

               result = sendATcmd("AT+CSDH=1\r");
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

               result = sendATcmd("AT+CMGL=\"ALL\"\r");
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

                     resp = getCmdResp();
                     strncpy((target+i)->meta, resp.param, strlen(resp.param));
                     (target+i)->meta[strlen(resp.param)] = '\0';

                     resp = getCmdResp();
                     strncpy((target+i)->text, resp.param, strlen(resp.param));
                     (target+i)->text[strlen(resp.param)] = '\0';

                  }

               }

            }

            SMS_print printout;

            printout.noMsg = i;
            printout.firstMsg = frmOutput;

            frmCback(error_out, &printout);

            debug(">>>interf<<<   Lectura de SMS concluida!\r\n");

            frmState = IDLE;

         }

         else{

            ATresp resp;

            resp = getCmdResp();
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

   uint8_t aux[5]; /* aux buffer */
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

         if(OK != error_out.error_formula){
            if(ERR_GSM == error_out.error_formula){

               ATresp resp;

               resp = getCmdResp();
               strncpy(error_out.error_command.command, resp.cmd, 19);
               error_out.error_command.command[20] = '\0';
               strncpy(error_out.error_command.parameter, resp.param, 149);
               error_out.error_command.parameter[150] = '\0';

            }
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

         initEngine();

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

         if(OK != error_out.error_formula){
            if(ERR_GSM == error_out.error_formula){

               ATresp resp;

               resp = getCmdResp();
               strncpy(error_out.error_command.command, resp.cmd, 19);
               error_out.error_command.command[20] = '\0';
               strncpy(error_out.error_command.parameter, resp.param, 149);
               error_out.error_command.parameter[150] = '\0';

            }
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

               result = sendATcmd("AT+CIPMODE=1\r");
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

               result = sendATcmd(APNstring);
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

               result = sendATcmd("AT+CIICR\r");
               if(OK_CMD_SENT == result){runState = ATCMD4RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD4RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){runState = ATCMD5;}
                  else if(ERR_MSG_CLOSE == result){{error_out.error_formula = ERR_GSM; frmState = WRAP;};}
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD5:

               result = sendATcmd("AT+CIFSR\r");
               if(OK_CMD_SENT == result){runState = ATCMD5RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD5RESP:

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

         if(OK != error_out.error_formula){
            if(ERR_GSM == error_out.error_formula){

               ATresp resp;

               resp = getCmdResp();
               strncpy(error_out.error_command.command, resp.cmd, 19);
               error_out.error_command.command[20] = '\0';
               strncpy(error_out.error_command.parameter, resp.param, 149);
               error_out.error_command.parameter[150] = '\0';

            }
         }

         frmCback(error_out, 0);
         frmState = IDLE;
         break;
   }

   return;
}

static void ciaaMobile_openPort_f (void)
{
   static runStatus runState = NOCMD;
   static error_user error_out;

   static uint8_t port_string[100];  /* str for the AT+CIPSTART command */

   FSMresult result;

   switch(frmState) {

      case INIT:

         port_string[0] = '\0';

         error_out.error_formula = OK;
         error_out.error_command.command[0] = '\0';
         error_out.error_command.parameter[0] = '\0';

         strncat(port_string, "AT+CIPSTART=\"", 13);

         if(TCP == ((port_s *)frmInput)->type){
            strncat(port_string, "TCP\",\"", 6);
         }
         else if(UDP == ((port_s *)frmInput)->type){
            strncat(port_string, "UDP\",\"", 6);
         }

         strncat(port_string, ((port_s *)frmInput)->address, 100);
         strncat(port_string, "\",\"", 3);
         strncat(port_string, ((port_s *)frmInput)->port, 6);
         strncat(port_string, "\"\r", 2);

         debug(port_string);  // SACAR MAS TARDE
         debug("\r\n");

         runState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(runState){

            case ATCMD1:

               result = sendATcmd("AT+CIPCLOSE=0\r");
               if(OK_CMD_SENT == result){runState = ATCMD1RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){runState = ATCMD2;}
                  else if(ERR_MSG_CLOSE == result){runState = ATCMD2;}
                  /* Operation is not stopped if AT+CIPCLOSE returns an error code,
                   * as there may simply be no open port to close. The command is
                   * sent as a precaution. */
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD2:

               result = sendATcmd(port_string);
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

         if(OK != error_out.error_formula){
            if(ERR_GSM == error_out.error_formula){

               ATresp resp;

               resp = getCmdResp();
               strncpy(error_out.error_command.command, resp.cmd, 19);
               error_out.error_command.command[20] = '\0';
               strncpy(error_out.error_command.parameter, resp.param, 149);
               error_out.error_command.parameter[150] = '\0';

            }
         }

         else{

            changeSerialMode(DATA_MODE);

         }

         frmCback(error_out, 0);
         frmState = IDLE;
         break;
   }

   return;
}

static void ciaaMobile_closePort_f (void)
{
   static runStatus runState = NOCMD;
   static error_user error_out;

   FSMresult result;

   switch(frmState) {

      case INIT:

         error_out.error_formula = OK;
         error_out.error_command.command[0] = '\0';
         error_out.error_command.parameter[0] = '\0';

         runState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(runState){

            case ATCMD1:

               result = sendATcmd("AT+CIPCLOSE=0\r");
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

         if(OK != error_out.error_formula){
            if(ERR_GSM == error_out.error_formula){

               ATresp resp;

               resp = getCmdResp();
               strncpy(error_out.error_command.command, resp.cmd, 19);
               error_out.error_command.command[20] = '\0';
               strncpy(error_out.error_command.parameter, resp.param, 149);
               error_out.error_command.parameter[150] = '\0';

            }
         }

         changeSerialMode(COMMAND_MODE);

         frmCback(error_out, 0);
         frmState = IDLE;
         break;
   }

   return;
}

static void ciaaMobile_getSignalQuality_f (void)
{
   static runStatus runState = NOCMD;
   static error_user error_out;

   FSMresult result;

   switch(frmState) {

      case INIT:

         error_out.error_formula = OK;
         error_out.error_command.command[0] = '\0';
         error_out.error_command.parameter[0] = '\0';

         runState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(runState){

            case ATCMD1:

               result = sendATcmd("AT+CSQ\r");
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

            ATresp resp;
            uint8_t auxStr[5];    /* auxiliary string for response parsing*/
            auxStr[0] = '\0';
            uint8_t i;
            uint8_t commaPos = 0; /* position of the comma in the response */

            resp = getCmdResp();  /* get the signal quality response */

            /* Search for the comma in the response, which is in the form RSSI,BER */

            for(i = 0; i < (strlen(resp.param)-1); i++){
               if(',' == resp.param[i]){commaPos = i; break;}
            }

            /* Copy the string of RSSI and convert it into a integer. Then, use the
             * table in the SIM808 manual to translate this number into an actual
             * RSSI reading in dBm.
             *
             * If the value is not known or detectable by the module, the function
             * will give a reading of positive 99. This should be interpreted by
             * the user as an error result. */

            strncpy(auxStr,&resp.param[0],commaPos);
            auxStr[commaPos] = '\0';

            ((signal_quality_s *)frmOutput)->rssi = atoi(auxStr);

            if(0 == ((signal_quality_s *)frmOutput)->rssi){
               ((signal_quality_s *)frmOutput)->rssi = -115;
            }

            else if(1 == ((signal_quality_s *)frmOutput)->rssi){
               ((signal_quality_s *)frmOutput)->rssi = -111;
            }

            else if((((signal_quality_s *)frmOutput)->rssi) >= 2 &&
                    (((signal_quality_s *)frmOutput)->rssi) <= 30){
               ((signal_quality_s *)frmOutput)->rssi = -110+(((signal_quality_s *)frmOutput)->rssi -2)*2;
            }

            else if(31 == ((signal_quality_s *)frmOutput)->rssi){
               ((signal_quality_s *)frmOutput)->rssi = -52;
            }

            else if(99 == ((signal_quality_s *)frmOutput)->rssi){
               ((signal_quality_s *)frmOutput)->rssi = 99;
            }

            /* Copy the string of BER and convert it to a number. The module reports
             * RXQUAL values as per table in GSM 05.08 [20] subclause 7.2.4 */

            strncpy(auxStr,&resp.param[commaPos+1],strlen(resp.param)-commaPos-1);
            auxStr[strlen(resp.param)-commaPos-1] = '\0';

            ((signal_quality_s *)frmOutput)->ber = atoi(auxStr);

            debug(">>>interf<<<   RSSI: ");
            itoa(((signal_quality_s *)frmOutput)->rssi, auxStr, 10);
            debug(auxStr);
            debug(", RXQUAL: ");
            itoa(((signal_quality_s *)frmOutput)->ber, auxStr, 10);
            debug(auxStr);
            debug(" \r\n");

            frmCback(error_out, frmOutput);

            frmState = IDLE;

         }

         else{

            ATresp resp;

            resp = getCmdResp();
            strncpy(error_out.error_command.command, resp.cmd, 19);
            error_out.error_command.command[20] = '\0';
            strncpy(error_out.error_command.parameter, resp.param, 149);
            error_out.error_command.parameter[150] = '\0';

            frmCback(error_out, frmOutput);

            frmState = IDLE;

         }

         break;
   }

   return;

}

static void ciaaMobile_powerGNSS_f (void)
{
   static runStatus runState = NOCMD;
   static error_user error_out;

   static uint8_t cmdStr[14]; /* string to determine ON or OFF command */

   FSMresult result;

   switch(frmState) {

      case INIT:

         error_out.error_formula = OK;
         error_out.error_command.command[0] = '\0';
         error_out.error_command.parameter[0] = '\0';

         runState = ATCMD1;
         frmState = PROC;

         if(ON == *((power_GNSS_e *)frmInput)){
            strncpy(cmdStr,"AT+CGNSPWR=1\r",14);
            debug(cmdStr);
         }
         else if(OFF == *((power_GNSS_e *)frmInput)){
            strncpy(cmdStr,"AT+CGNSPWR=0\r",14);
            debug(cmdStr);
         }

         break;

      case PROC:

         switch(runState){

            case ATCMD1:

               result = sendATcmd(cmdStr);
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

         if(OK != error_out.error_formula){
            if(ERR_GSM == error_out.error_formula){

               ATresp resp;

               resp = getCmdResp();
               strncpy(error_out.error_command.command, resp.cmd, 19);
               error_out.error_command.command[20] = '\0';
               strncpy(error_out.error_command.parameter, resp.param, 149);
               error_out.error_command.parameter[150] = '\0';

            }
         }

         frmCback(error_out, 0);
         frmState = IDLE;

         break;
   }

   return;

}

static void ciaaMobile_getGNSSNavInfo_f (void)
{
   static runStatus runState = NOCMD;
   static error_user error_out;

   FSMresult result;

   switch(frmState) {

      case INIT:

         error_out.error_formula = OK;
         error_out.error_command.command[0] = '\0';
         error_out.error_command.parameter[0] = '\0';

         runState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(runState){

            case ATCMD1:

               result = sendATcmd("AT+CGNSINF\r");
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

            ATresp resp;

            resp = getCmdResp(); /* Get the navigation info string */

            /* Copy the navigation info string to the provided output */

            strncpy((uint8_t *)frmOutput,&resp.param[0],95);
            ((uint8_t *)frmOutput)[94] = '\0';

            debug(">>>interf<<<   NavInfo String:");
            debug(((uint8_t *)frmOutput));
            debug(" \r\n");

         }

         else{

            ATresp resp;

            resp = getCmdResp();
            strncpy(error_out.error_command.command, resp.cmd, 19);
            error_out.error_command.command[20] = '\0';
            strncpy(error_out.error_command.parameter, resp.param, 149);
            error_out.error_command.parameter[150] = '\0';

         }

         frmCback(error_out, frmOutput);

         frmState = IDLE;

         break;
   }

   return;

}

void ciaaMobile_checkGSMGPRS_f (void)
{
   static runStatus runState = NOCMD;
   static error_user error_out;

   FSMresult result;

   static ATresp respGSM;
   static ATresp respGPRS;

   switch(frmState) {

      case INIT:

         error_out.error_formula = OK;
         error_out.error_command.command[0] = '\0';
         error_out.error_command.parameter[0] = '\0';

         runState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(runState){

            case ATCMD1:

               result = sendATcmd("AT+CREG?\r");
               if(OK_CMD_SENT == result){runState = ATCMD1RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){
                     respGSM = getCmdResp(); /* get the GSM response */
                     runState = ATCMD2;
                  }
                  else if(ERR_MSG_CLOSE == result){{error_out.error_formula = ERR_GSM; frmState = WRAP;};}
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD2:

               result = sendATcmd("AT+CGATT?\r");
               if(OK_CMD_SENT == result){runState = ATCMD2RESP;}
               else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD2RESP:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){
                     respGPRS = getCmdResp(); /* get the GPRS response */
                     frmState = WRAP;
                  }
                  else if(ERR_MSG_CLOSE == result){{error_out.error_formula = ERR_GSM; frmState = WRAP;};}
                  else{error_out.error_formula = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(OK == error_out.error_formula){

            /* Copy the GSM info string to the provided output */

            if('1' == respGSM.param[3]){
               ((statusGSMGPRS_s *)frmOutput)->gsm = true;
            }
            else{
               ((statusGSMGPRS_s *)frmOutput)->gsm = false;
            }

            /* Print out the GSM string */

            debug(">>>interf<<<   GSM String:");
            debug(&respGSM.param[0]);
            debug(" \r\n");

            /* Copy the GPRS info string to the provided output */

            if('1' == respGPRS.param[1]){
               ((statusGSMGPRS_s *)frmOutput)->gprs = true;
            }
            else{
               ((statusGSMGPRS_s *)frmOutput)->gprs = false;
            }

            /* Print out the GPRS string */

            debug(">>>interf<<<   GPRS String:");
            debug(&respGPRS.param[0]);
            debug(" \r\n");

         }

         else{

            ATresp resp;

            resp = getCmdResp();
            strncpy(error_out.error_command.command, resp.cmd, 19);
            error_out.error_command.command[20] = '\0';
            strncpy(error_out.error_command.parameter, resp.param, 149);
            error_out.error_command.parameter[150] = '\0';

         }

         frmCback(error_out, frmOutput);

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

void ciaaMobile_readRecSMS (SMS_rec * msg, SMS_rd_params * params, void * (*cback) (error_user, void *))
{
   frm = ciaaMobile_readRecSMS_f;
   msg->meta[0] = 1; /* we hide the number of messages inside the msg record */
   frmInput = params;
   frmOutput = msg;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_listRecSMS (SMS_rec * list, uint8_t listSz, void * (*cback) (error_user, void *))
{
   frm = ciaaMobile_listRecSMS_f;
   list->meta[0] = listSz;
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

void ciaaMobile_openPort (port_s * port, void * (*cback) (error_user, void *))
{
   frm = ciaaMobile_openPort_f;
   frmInput = port;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_closePort (void * (*cback) (error_user, void *))
{
   frm = ciaaMobile_closePort_f;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_getSignalQuality (signal_quality_s * signal_quality, void * (*cback) (error_user, void *))
{
   frm = ciaaMobile_getSignalQuality_f;
   frmOutput = signal_quality;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_powerGNSS (power_GNSS_e * command, void * (*cback) (error_user, void *))
{
   frm = ciaaMobile_powerGNSS_f;
   frmInput = command;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_getGNSSNavInfo (uint8_t * navInfo, void * (*cback) (error_user, void *))
{
   frm = ciaaMobile_getGNSSNavInfo_f;
   frmOutput = navInfo;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_checkGSMGPRS (statusGSMGPRS_s * status, void * (*cback) (error_user, void *))
{
   frm = ciaaMobile_checkGSMGPRS_f;
   frmOutput = status;
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

void ciaaMobile_setUrcCback (void (*cback) (uint8_t const * const cmd, uint8_t const * const par))
{
   changeUrcMode(CBACK_MODE);
   setUrcCback(cback);
   return;
}

void ciaaMobile_setUrcManual (void)
{
   changeUrcMode(MANUAL_MODE);
   setUrcCback(0);
   return;
}

/** This function needs to be called periodically to process commands and
 *  their responses.
 */

void ciaaMobile_sysUpdate (void)
{
   if(0 == sysupd_count){

      if (IDLE == frmState){

         if(COMMAND_MODE == checkSerialMode()){
            processToken();
         }

         else if (DATA_MODE == checkSerialMode()){
            printData(); // PARA PRUEBA; CAMBIAR DESPUES A ALGO QUE LE PASE EL STREAM AL USUARIO
         }

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
