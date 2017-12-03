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

/** @brief This is a simple UART example
 */

/** \addtogroup uart Bare-metal example
 ** @{ */

/*==================[inclusions]=============================================*/

#include "ciaaMobile_interface.h"

/*==================[macros and definitions]=================================*/

/*==================[global data]============================================*/

/*==================[internal data declaration]==============================*/

static frmStatus frmState = IDLE;
static void (*frm) (void);
static void * frmInput;
static void * frmOutput;
static void * (*frmCback) (void *);

/*==================[internal functions declaration]=========================*/

static void ciaaMobile_sendSMS_f (void);

static void ciaaMobile_listRecSMS_f (void);

static void ciaaMobile_startUp_f (void)

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void ciaaMobile_sendSMS_f (void)
{
   static uint8_t runState = 0;

   static uint8_t dest[20];
   static uint8_t text[150];

   switch(frmState) {

      case INIT:

         /*sprintf(dest, "\"%s\"", ((SMS_send *)msg)->dest);   VER SI SE PUEDE INCORPORAR MAS ADELANTE */
         dest[0] = '"';
         dest[1] = '\0';
         strncat(dest, ((SMS_send *)frmInput)->dest,strlen(((SMS_send *)frmInput)->dest));
         strncat(dest, "\"", 1);

         strncpy(text, ((SMS_send *)frmInput)->text, strlen(((SMS_send *)frmInput)->text));
         text[strlen(((SMS_send *)frmInput)->text)] = '\0';

         frmState = PROC;

         dbgPrint("Destino: ");
         dbgPrint(dest);
         dbgPrint("\r\nMensaje: ");
         dbgPrint(text);
         dbgPrint("\r\n");

         break;

      case PROC:

         switch(runState){

            case 0:

               sendATcmd("CMGS",dest,EXT_WRITE);
               runState = 1;
               break;

            case 1:

               if(cmdClosed == processToken()){runState = 2;}
               break;

            case 2:

               sendATcmd("SMS_BODY",text,SMS);
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

               sendATcmd("CMGL","\"REC UNREAD\"",EXT_WRITE);
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
         uint8_t respNo; /* MISSING: a check to see if the list can
                            accomodate all reponses */
         uint8_t * resp;
         SMS_rec * target = (SMS_rec *)frmOutput;

         respNo = getNoCmdResp();

         if(1 == respNo){dbgPrint("No hay SMSs\r\n");}

         else {

            for(i = 0; i < (respNo-1)/2; i++){

               resp = getCmdResp(2*i);
               strncpy((target+i)->meta, resp, 149);
               (target+i)->meta[149] = '\0';

               resp = getCmdResp((2*i)+1);
               strncpy((target+i)->text, resp, 149);
               (target+i)->text[149] = '\0';
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

               sendATcmd(0,0,AUTOBAUD);
               runState = 1;
               break;

            case 1:

               if(cmdClosed == processToken()){runState = 2;}
               break;

            case 2:

               sendATcmd("CMGF","1",EXT_WRITE);
               runState = 3;
               break;

            case 3:

               if(cmdClosed == processToken()){runState = 4;}
               break;

            case 4:

               sendATcmd("CSCS","\"GSM\"",EXT_WRITE);
               runState = 5;
               break;

            case 5:

               if(cmdClosed == processToken()){runState = 6;}
               break;

            case 6:

               sendATcmd("CNMI","0,0",EXT_WRITE);
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

void ciaaMobile_sendSMS (void * msg, void * (*cback) (void *))
{
   frm = ciaaMobile_sendSMS_f;
   frmInput = msg;
   frmCback = cback;
   frmState = INIT;

   return;
}

void ciaaMobile_listRecSMS (void * list, void * (*cback) (void *))
{
   frm = ciaaMobile_listRecSMS_f;
   frmOutput = list;
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
