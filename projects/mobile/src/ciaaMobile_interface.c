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

static void * (*frmCback) (void *);

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

   static uint8_t smsCmd[30];
   static uint8_t smsText[150];

   switch(frmState) {

      case INIT:

         smsCmd[0] = '\0';
         smsText[0] = '\0';

         /*sprintf(dest, "\"%s\"", ((SMS_send *)msg)->dest);   VER SI SE PUEDE INCORPORAR MAS ADELANTE */
         strncat(smsCmd, "AT+CMGS=\"", 9);
         strncat(smsCmd, ((SMS_send *)frmInput)->dest,strlen(((SMS_send *)frmInput)->dest));
         strncat(smsCmd, "\"\r", 2);

         strncpy(smsText, ((SMS_send *)frmInput)->text, strlen(((SMS_send *)frmInput)->text));
         smsText[strlen(((SMS_send *)frmInput)->text)] = '\0';

         frmState = PROC;

         //#ifdef DEBUGGSM
         dbgPrint("\r\nsmsCmd: ");
         dbgPrint(smsCmd);
         dbgPrint("\nsmsText: ");
         dbgPrint(smsText);
         dbgPrint("\r\n");
         //#endif

         break;

      case PROC:

         switch(runState){

            case 0:

               sendATcmd(smsCmd);
               runState = 1;
               break;

            case 1:

               if(cmdClosed == processToken()){runState = 2;}
               break;

            case 2:

               sendATcmd(smsText);
               runState = 3;
               break;

            case 3:

               if(cmdClosed == processToken()){runState = 0; frmState = WRAP;}
               break;

         }

         break;

      case WRAP:

         frmCback(0);
         frmState = IDLE;
         break;
   }

   return;

}

static void ciaaMobile_listRecSMS_f (void)
{
   static uint8_t runState = 0;

   switch(frmState) {

      case INIT:

         dbgPrint("Leyendo SMS\r\n");

         frmState = PROC;

         break;

      case PROC:

         switch(runState){

            case 0:

               sendATcmd("AT+CMGL=\"ALL\"\r");
               runState = 1;
               break;

            case 1:

               if(cmdClosed == processToken()){runState = 0; frmState = WRAP;}
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

         dbgPrint("Tamaño del vector: ");
         itoa(target->meta[0], auxtext, 10);
         dbgPrint(auxtext);
         dbgPrint("\r\n");
         dbgPrint("Cantidad de SMS: ");
         itoa((respNo-1)/2, auxtext, 10);
         dbgPrint(auxtext);
         dbgPrint("\r\n");

         // DEBUG ERASE LATER

         if(target->meta[0] < ((respNo-1)/2)){
            dbgPrint("No hay suficiente espacio para los mensajes \r\n");
         }

         else {

            if(1 == respNo){dbgPrint("No hay SMSs\r\n");}

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

         frmCback(frmOutput);

         dbgPrint("Lectura de SMS concluida\r\n");

         frmState = IDLE;
         break;
   }

   return;

}

static void ciaaMobile_delSMS_f (void)
{
   static uint8_t runState = 0;

   static uint8_t aux[5]; /* aux buffer */
   static uint8_t smsDel[20];  /* holds the str of the sms del cmd including paramenters */

   switch(frmState) {

      case INIT:

         aux[0] = '\0';
         smsDel[0] = '\0';

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

               sendATcmd(smsDel);
               runState = 1;
               break;

            case 1:

               if(cmdClosed == processToken()){runState = 0; frmState = WRAP;}
               break;

         }

         break;

      case WRAP:

         frmCback(0);
         frmState = IDLE;
         break;
   }

   return;
}

static void ciaaMobile_startUp_f (void)
{
   static uint8_t runState = 0;

   switch(frmState) {

      case INIT:

         dbgPrint("Inicializando ciaaMobile...\r\n");

         frmState = PROC;

         break;

      case PROC:

         switch(runState){

            case 0:

               sendATcmd("AT\r");
               runState = 1;
               break;

            case 1:

               if(cmdClosed == processToken()){runState = 2;}
               break;

            case 2:

               sendATcmd("AT+CMGF=1\r");
               runState = 3;
               break;

            case 3:

               if(cmdClosed == processToken()){runState = 4;}
               break;

            case 4:

               sendATcmd("AT+CSCS=\"GSM\"\r");
               runState = 5;
               break;

            case 5:

               if(cmdClosed == processToken()){runState = 6;}
               break;

            case 6:

               sendATcmd("AT+CNMI=0,0\r");
               runState = 7;
               break;

            case 7:

               if(cmdClosed == processToken()){runState = 0; frmState = WRAP;}
               break;

         }

         break;

      case WRAP:

         dbgPrint("ciaaMobile inicializado!\r\n");

         frmState = IDLE;
         break;
   }

   return;

}

/*==================[external functions definition]==========================*/

void ciaaMobile_sendSMS (SMS_send * msg, void * (*cback) (void *))
{
   frm = ciaaMobile_sendSMS_f;
   frmInput = msg;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_listRecSMS (SMS_rec * list, uint8_t noMsg, void * (*cback) (void *))
{
   frm = ciaaMobile_listRecSMS_f;
   list->meta[0] = noMsg; /* we hide the noMsg integer inside the list vector */
   frmOutput = list;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_delSMS (SMS_del * msgdel, void * (*cback) (void *))
{
   frm = ciaaMobile_delSMS_f;
   frmInput = msgdel;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_startUp (void)
{
   frm = ciaaMobile_startUp_f;
   frmState = INIT;

   return;
}

/** This function needs to be called periodically to process commands and
 *  their responses.
 */

void ciaaMobile_sysUpdate (void)
{
   if (IDLE == frmState){return;}
   else {frm();}

   return;
}

uint8_t ciaaMobile_isIdle (void)
{
   return(frmState == IDLE);
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
