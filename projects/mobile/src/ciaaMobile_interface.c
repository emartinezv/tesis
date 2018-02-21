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

static void * (*frmCback) (error_frm, void *);

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

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void ciaaMobile_sendSMS_f (void)
{
   static uint8_t runState = 0;
   static error_frm frmError = OK;

   static uint8_t smsCmd[30];
   static uint8_t smsText[150];

   FSMresult result;

   switch(frmState) {

      case INIT:

         smsCmd[0] = '\0';
         smsText[0] = '\0';

         runState = 0;
         frmError = OK;

         /*sprintf(dest, "\"%s\"", ((SMS_send *)msg)->dest);   VER SI SE PUEDE INCORPORAR MAS ADELANTE */
         strncat(smsCmd, "AT+CMGS=\"", 9);
         strncat(smsCmd, ((SMS_send *)frmInput)->dest,strlen(((SMS_send *)frmInput)->dest));
         strncat(smsCmd, "\"\r", 2);

         strncpy(smsText, ((SMS_send *)frmInput)->text, strlen(((SMS_send *)frmInput)->text));
         smsText[strlen(((SMS_send *)frmInput)->text)] = '\0';

         frmState = PROC;

         debug(">>>interf<<<   COMANDO ENVIO SMS: ");
         debug(smsCmd);
         debug("\r\n>>>interf<<<   TEXTO SMS: ");
         debug(smsText);
         debug("\r\n");

         break;

      case PROC:

         switch(runState){

            case 0:

               result = sendATcmd(smsCmd);
               if(OK_CMD_SENT == result){runState = 1;}
               else{frmError = ERR_PROC; frmState = WRAP;}
               break;

            case 1:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CLOSE == result){runState = 2;}
                  else if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else{frmError = ERR_PROC; frmState = WRAP;}
               }
               break;

            case 2:

               result = sendATcmd(smsText);
               if(OK_CMD_SENT == result){runState = 3;}
               else{frmError = ERR_PROC; frmState = WRAP;}
               break;

            case 3:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CLOSE == result){frmState = WRAP;}
                  else if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else{frmError = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         frmCback(frmError, 0);
         frmState = IDLE;
         break;
   }

   return;

}

static void ciaaMobile_listRecSMS_f (void)
{
   static uint8_t runState = 0;
   static error_frm frmError = OK;

   FSMresult result;

   switch(frmState) {

      case INIT:

         runState = 0;
         frmError = OK;

         debug(">>>interf<<<   Leyendo SMS...\r\n");

         frmState = PROC;

         break;

      case PROC:

         switch(runState){

            case 0:

               result = sendATcmd("AT+CMGL=\"ALL\"\r");
               if(OK_CMD_SENT == result){runState = 1;}
               else{frmError = ERR_PROC; frmState = WRAP;}
               break;

            case 1:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CLOSE == result){frmState = WRAP;}
                  else if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else{frmError = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         ;
         uint8_t i = 0;
         uint8_t respNo;
         uint8_t auxtext[4]; // ERASE LATER, FOR DEBUG ONLY

         uint8_t * resp;
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
            frmError = ERR_WRAP;
            debug(">>>interf<<<   No hay suficiente espacio para los mensajes!\r\n");
         }

         else {

            if(1 == respNo){debug(">>>interf<<<   No hay SMSs!\r\n");}

            else{

               for(i = 0; i < (respNo-1)/2; i++){

                  resp = getCmdResp(2*i);
                  strncpy((target+i)->meta, resp, 149);
                  (target+i)->meta[149] = '\0';

                  resp = getCmdResp((2*i)+1);
                  strncpy((target+i)->text, resp, 149);
                  (target+i)->text[149] = '\0';
               }

            }

         }

         (target+i)->meta[0] = '\0';

         frmCback(frmError, frmOutput);

         debug(">>>interf<<<   Lectura de SMS concluida!\r\n");

         frmState = IDLE;
         break;
   }

   return;

}

static void ciaaMobile_delSMS_f (void)
{
   static uint8_t runState = 0;
   static error_frm frmError = OK;

   static uint8_t aux[5]; /* aux buffer */
   static uint8_t smsDel[20];  /* holds the str of the sms del cmd including paramenters */

   FSMresult result;

   switch(frmState) {

      case INIT:

         aux[0] = '\0';
         smsDel[0] = '\0';

         runState = 0;
         frmError = OK;

         strncat(smsDel, "AT+CMGD=", 8);
         itoa(((SMS_del *)frmInput)->index, aux, 10);
         strncat(smsDel, aux, strlen(aux));
         strncat(smsDel, ",", 1);
         itoa(((SMS_del *)frmInput)->mode, aux, 10);
         strncat(smsDel, aux, strlen(aux));
         strncat(smsDel, "\r", 1);

         frmState = PROC;

         break;

      case PROC:

         switch(runState){

            case 0:

               result = sendATcmd(smsDel);
               if(OK_CMD_SENT == result){runState = 1;}
               else{frmError = ERR_PROC; frmState = WRAP;}
               break;

            case 1:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CLOSE == result){frmState = WRAP;}
                  else if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else{frmError = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         frmCback(frmError, 0);
         frmState = IDLE;
         break;
   }

   return;
}

static void ciaaMobile_startUp_f (void)
{
   static uint8_t runState = 0;
   static error_frm frmError = OK;

   FSMresult result;

   switch(frmState) {

      case INIT:

         runState = 0;
         frmError = OK;

         debug(">>>interf<<<   Inicializando ciaaMobile...\r\n");

         frmState = PROC;

         break;

      case PROC:

         switch(runState){

            case 0:

               result = sendATcmd("AT\r");
               if(OK_CMD_SENT == result){runState = 1;}
               else{frmError = ERR_PROC; frmState = WRAP;}
               break;

            case 1:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CLOSE == result){runState = 2;}
                  else if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else{frmError = ERR_PROC; frmState = WRAP;}
               }
               break;

            case 2:

               result = sendATcmd("AT+CMGF=1\r");
               if(OK_CMD_SENT == result){runState = 3;}
               else{frmError = ERR_PROC; frmState = WRAP;}
               break;

            case 3:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CLOSE == result){runState = 4;}
                  else if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else{frmError = ERR_PROC; frmState = WRAP;}
               }
               break;

            case 4:

               result = sendATcmd("AT+CSCS=\"GSM\"\r");
               if(OK_CMD_SENT == result){runState = 5;}
               else{frmError = ERR_PROC; frmState = WRAP;}
               break;

            case 5:

               result = processToken();
               if(NO_UPDATE != result){
                  if(OK_CLOSE == result){frmState = WRAP;}
                  else if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else{frmError = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         frmCback(frmError, 0);

         debug(">>>interf<<<   ciaaMobile inicializado!\r\n");

         frmState = IDLE;
         break;
   }

   return;

}

/*==================[external functions definition]==========================*/

void ciaaMobile_sendSMS (SMS_send * msg, void * (*cback) (error_frm, void *))
{
   frm = ciaaMobile_sendSMS_f;
   frmInput = msg;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_listRecSMS (SMS_rec * list, uint8_t noMsg, void * (*cback) (error_frm, void *))
{
   frm = ciaaMobile_listRecSMS_f;
   list->meta[0] = noMsg; /* we hide the noMsg integer inside the list vector */
   frmOutput = list;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_delSMS (SMS_del * msgdel, void * (*cback) (error_frm, void *))
{
   frm = ciaaMobile_delSMS_f;
   frmInput = msgdel;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_startUp (void * (*cback) (error_frm, void *))
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
   if (IDLE == frmState){
      processToken();
      return;
   }
   else {frm();}

   return;
}

uint8_t ciaaMobile_isIdle (void)
{
   return(frmState == IDLE);
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
