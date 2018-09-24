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

/** @brief This module handles the internal engine of the library
 */

/** \addtogroup engine engine
 ** @{ */

/*==================[inclusions]=============================================*/

#include "gsmEngine.h"

/*==================[macros and definitions]=================================*/

#define DEBUG_ENGINE
#ifdef DEBUG_ENGINE
   #define debug(msg) dbgPrint(msg)
#else
   #define debug(msg)
#endif

#define DEBUG_NOTIMEOUT // ignore command timeouts

/*==================[global data]============================================*/

/*==================[internal data declaration]==============================*/

/*---------------------------------------------------------------------------*/
/*                      Internal engine state variables                      */
/*---------------------------------------------------------------------------*/

/** @brief Current state of the engine */

static fsmState_e fsmState = WAITING;

/** @brief used for AT command timeout counter */

uint32_t timeout_count = 0; /* NO ES STATIC, VER SI ES PROBLEMATICO */

/** @brief Command or Data mode for the serial port */

static serialMode_e serialMode = COMMAND_MODE;

/** @brief Manual or Callback mode for URC handling */

static urcMode_e urcMode = MANUAL_MODE;

/** @brief Function pointer to callback for URCs */

static void (*urcCback) (uint8_t const * const cmd, uint8_t const * const par) = 0;

/*---------------------------------------------------------------------------*/
/*                   Variables for the token VL ring buffer                  */
/*---------------------------------------------------------------------------*/

/** @brief Buffer for the tknVlRb VLRB */

static uint8_t tknRbBuf[TKN_BUF_SIZE];

/** @brief RB structure for the tknVlRb VLRB */

static RINGBUFF_T tknRb;

/** @brief Token VL ring buffer */

static VLRINGBUFF_T tknVlRb;

/*---------------------------------------------------------------------------*/
/*                 Variables for the response VL ring buffer                 */
/*---------------------------------------------------------------------------*/

/** @brief Buffer for the rspVlRb VLRB */

static uint8_t rspRbBuf[RSP_BUF_SIZE];

/** @brief RB structure for the rspVlRb VLRB */

static RINGBUFF_T rspRb;

/** @brief Response VL ring buffer */

static VLRINGBUFF_T rspVlRb;

/*---------------------------------------------------------------------------*/
/*                   Variables for the URC VL ring buffer                    */
/*---------------------------------------------------------------------------*/

/** @brief Buffer for the urcVlRb VLRB */

static uint8_t urcRbBuf[URC_BUF_SIZE];

/** @brief RB structure for the urcVlRb VLRB */

static RINGBUFF_T urcRb;

/** @brief URC VL ring buffer */

static VLRINGBUFF_T urcVlRb;

/*==================[internal functions declaration]=========================*/

/** @brief Updates the engine FSM with the latest received token
*
*  @param received  Type of token as per the gsmParseTkn function
*  @param cmd       Main part of the token used as input
*  @param par       Parameter part of the token used as input
*  @param index     Index of the AT command (for sending cmds only)
*
*  @return Returns the event triggered by the updateFSM call
*/

static fsmEvent gsmUpdateFsm (tknTypeParser_e received,
                              uint8_t const * const cmd,
                              uint8_t const * const par, uint8_t const index);

/** @brief Records a new URC in the URC vector
*
*  @param cmd   Command part of the URC
*  @param par   Parameter part of the URC
*
*  @return Returns 1 if successful or 0 if VLRB is full
*/

static uint8_t gsmRecordUrc (uint8_t const * const cmd,
                             uint8_t const * const par);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/** The gsmUpdateFsm function updates the engine FSM which preserves the state
 *  of the current command execution. The input is always a token, either one
 *  received from the GSM module or an AT command the user is  trying to send.
 *  The token is already processed by the gsmParseTkn functino, so we have it's
 *  type in detail. The output of the function is the event triggered by the
 *  processing of the input token. State is internal to the FSM, but the user
 *  gets the events to see what just happened.
 */

static FSMresult updateFSM (tknTypeParser_e received, uint8_t const * const command,
                            uint8_t const * const parameter, uint8_t index)
{
   static uint8_t currCMD[TKN_LEN]; /* command being currently executed */
   static uint8_t currPAR[TKN_LEN]; /* parameter of the current command */

   static uint8_t idx;              /* index of the command in the list */

   uint8_t rspAux[TKN_LEN+20];      /* auxiliary buffer for storing the response */

   switch(GSMstatus){

      case WAITING: /* initial state */

         idx = index;

         if ((AUTOBAUD <= received) && (SMS_BODY_P >= received)){ /* command sent by serial port */

            timeout_count = commands[idx].timeout;
            debug(">>>engine<<<   TIMEOUT COUNTER UPDATED\r\n");

            strncpy(currCMD,command,strlen(command));
            currCMD[strlen(command)] = '\0';
            strncpy(currPAR,parameter,strlen(parameter));
            currPAR[strlen(parameter)] = '\0';

            GSMstatus = CMD_SENT;

            debug(">>>engine<<<   COMMAND SENT: ");
            debug(command);
            debug("(");
            debug(parameter);
            debug(")\r\n");

            return OK_CMD_SENT;

         }

         else if ((BASIC_RSP == received) || (EXT_RSP == received)){ /* possible URC */

            if(1 == URCSearch(command)){
               debug(">>>engine<<<   URC detected\r\n");
               recordURC (command, parameter);

               return OK_URC;
            }
            else{
               debug(">>>engine<<<   RECEIVED RESPONSE, COMMAND EXPECTED\r\n");

               return ERR_OOO;
            }

         }

         else if (TIMEOUT == received){
               GSMstatus = WAITING;

               debug(">>>engine<<<   AT COMMAND TIMEOUT\r\n");

               return ERR_TIMEOUT;
         }

         else{
            debug(">>>engine<<<   INVALID TOKEN\r\n");

            return ERR_TKN_INV;

         }

         break;

      case CMD_SENT:

         /* now that the command has been sent, we check the echo from the
            modem and verify that it agrees with the sent command */

         if ((AUTOBAUD <= received) && (SMS_BODY_P >= received)){

            uint8_t eqCMD = 1;
            uint8_t eqPAR = 1;
            eqCMD = strncmp(command, currCMD, strlen(currCMD));
            eqPAR = strncmp(parameter, currPAR, strlen(currPAR));

            if ((0 == eqCMD) && (0 == eqPAR)){
               VLRingBuffer_Flush(&rspVlRb);
               GSMstatus = CMD_ACK;

               debug(">>>engine<<<   COMMAND ACK\r\n");

               return OK_CMD_ACK;
            }
            else{
               GSMstatus = WAITING;

               debug(">>>engine<<<   COMMAND ECHO ERROR\r\n");

               return ERR_CMD_ECHO;
            }
         }

         else if ((BASIC_RSP == received) || (EXT_RSP == received)){ /* possible URC */

            if(1 == URCSearch(command)){
               debug(">>>engine<<<   URC detected\r\n");
               recordURC (command, parameter);

               return OK_URC;
            }
            else{
               GSMstatus = WAITING;

               debug(">>>engine<<<   RECEIVED RESPONSE, COMMAND ECHO EXPECTED\r\n");

               return ERR_CMD_ECHO;
            }

         }

         else if (TIMEOUT == received){
               GSMstatus = WAITING;

               debug(">>>engine<<<   AT COMMAND TIMEOUT\r\n");

               return ERR_TIMEOUT;
         }

         else{
            GSMstatus = WAITING;

            debug(">>>engine<<<   INVALID TOKEN\r\n");

            return ERR_TKN_INV;
         }

         break;

      case CMD_ACK:

         /* process a number of tokens depending on the command, checking for
            end responses each time */

         if ((BASIC_RSP <= received) && (EXT_RSP >= received)){

            if(1 == URCSearch(command)){
               debug(">>>engine<<<   URC detected\r\n");
               recordURC (command, parameter);

               return OK_URC;
            }

            else{

               /* store the response in the active command response vector */

               /*if(currTKN >= RESPVECTOR_SIZE){

               }  ERROR SI ME QUEDA GRANDE LA RESPUESTA EN RESPVECTOR...

               else*/

               rspAux[0] = '\0';

               strncat(rspAux, command, strlen(command));
               strncat(rspAux, ".", 1);
               strncat(rspAux, parameter, strlen(parameter));

               VLRingBuffer_Insert(&rspVlRb, rspAux, (uint16_t) (strlen(rspAux)));

               debug(">>>engine<<<   rspAux: ");
               debug(rspAux);
               debug("\r\n");

               /* Since AT+CIFSR does not return an OK after reporting the IP,
                * we change check the response and change it to OK if it is
                * not ERROR.
                */

               if(0 == strncmp("CIFSR", currCMD, 5)){
                  if(0 == strstr(command,"ERROR")){

                     debug(">>>engine<<<   COMMAND CLOSED SUCCESSFULLY\r\n");

                     GSMstatus = WAITING;
                     return OK_CLOSE;
                  }
               }

               /* Compare current response with the string of valid          */
               /* successful end responses for the current command. If a     */
               /* match is detected, close command and report OK_CLOSE.      */

               if(NULL != strstr(commands[idx].sucResp,command)){

                  debug(">>>engine<<<   COMMAND CLOSED SUCCESSFULLY\r\n");

                  GSMstatus = WAITING;
                  return OK_CLOSE;
               }

               /* Compare current response with the string of valid error    */
               /* end responses for the current command. If a match is       */
               /* detected, close command and report ERR_MSG_CLOSE           */

               else if(NULL != strstr(commands[idx].errResp,command)){

                  debug(">>>engine<<<   COMMAND CLOSED IN ERROR\r\n");

                  GSMstatus = WAITING;
                  return ERR_MSG_CLOSE;
               }

               /* if the response is not an end response, just report OK_RESP */

               else{
                  return OK_RESP;
               }
            }

         }

         else if ((AUTOBAUD <= received) && (SMS_BODY_P >= received)){
            GSMstatus = WAITING;

            debug(">>>engine<<<   RECEIVED COMMAND, RESPONSE EXPECTED\r\n");

            return ERR_OOO;
         }

         else if (TIMEOUT == received){
               GSMstatus = WAITING;

               debug(">>>engine<<<   AT COMMAND TIMEOUT\r\n");

               return ERR_TIMEOUT;
         }

         else{

            debug(">>>engine<<<   INVALID TOKEN RECEIVED: ");
            debug(command);
            debug("(");
            debug(parameter);
            debug(")\r\n");

            return ERR_TKN_INV;
         }

         break;

      default:

         debug(">>>engine<<<   ERROR: SWITCH OUT OF RANGE");

         GSMstatus = WAITING;
         return ERR_FSM_OOR;

         break;
   }

}

static uint8_t recordURC (uint8_t const * const command,
                    uint8_t const * const parameter)
{
   if(MANUAL_MODE == urcMode){

      if(0 == VLRingBuffer_IsFull(&urcVlRb)){

        uint8_t urcAux[TKN_LEN];

        urcAux[0] = '\0';

        strncat(urcAux, command, strlen(command));
        strncat(urcAux, ".", 1);
        strncat(urcAux, parameter, strlen(parameter));

        VLRingBuffer_Insert(&urcVlRb, urcAux, (uint16_t) (strlen(urcAux)));

        debug(">>>engine<<<   urcAux: ");
        debug(urcAux);
        debug("\r\n");

        return 1;
     }

     else{

        debug(">>>engine<<<   URC VLRB lleno!\r\n");

        return 0;
     }
   }

   else if(CBACK_MODE == urcMode){

      urcCback(command, parameter);
      return;

   }

}

/*==================[external functions definition]==========================*/

void gsmInitEngine(void){

   gsmInitTokenizer();

   changeSerialMode(COMMAND_MODE); /* start serial comms in command mode */
   changeUrcMode(MANUAL_MODE);     /* URC handling starts in manual mode */

   VLRingBuffer_Init(&tknVlRb, &tknRb, &tknRbBuf, 1, TKN_BUF_SIZE);
   VLRingBuffer_Init(&rspVlRb, &rspRb, &rspRbBuf, 1, RSP_BUF_SIZE);
   VLRingBuffer_Init(&urcVlRb, &urcRb, &urcRbBuf, 1, URC_BUF_SIZE);

   return;

}

/** The processToken function checks is there are unread tokens in the token
 *  ring buffer. If so, it reads the oldest token, parses the token through the
 *  parse function and calls upon the updateFSM function with the result. It
 *  returns the result of the updateFSM invocation
 */

fsmEvent gsmProcessTkn(void)
{
   gsmDetectTkns(&tknVlRb);

   tknTypeParser_e received; /* classifies the received token*/
   FSMresult currCmd = NO_UPDATE; /* result of the updateFSM invocation */

   uint8_t token[TKN_LEN]; /* received token */
   int tknSize;        /* size of read token */
   uint8_t command[TKN_LEN]; /* AT command or response */
   uint8_t parameter[TKN_LEN]; /* AT command or response argument */

   if(0 == VLRingBuffer_IsEmpty(&tknVlRb)){

      tknSize = VLRingBuffer_Pop(&tknVlRb, &token, TKN_LEN);
      received = gsmParseTkn(token, command, parameter, tknSize);     /* parse the token */
      currCmd = updateFSM(received, command, parameter, 0);     /* update FSM */

   }

   #ifndef DEBUG_NOTIMEOUT

   else if((GSMstatus != WAITING) && (timeout_count == 0)){

      currCmd = updateFSM(TIMEOUT, 0, 0, 0);     /* update FSM */

   }

   #endif

   return currCmd;

}

/** When the serial port is in DATA_MODE, this function prints everything
 *  received through the 232-UART serial port to the USB-UART. This is
 *  merely a test function; the final version of the library needs to send
 *  these data to a user-defined buffer of some sort for processing.
 */

void gsmPrintData(void){

   uint8_t c = 0;

   if(0 != uartRecv(CIAA_UART_232, &c, 1)){

      uartSend(CIAA_UART_USB, &c, 1);

   }

   if(0 != uartRecv(CIAA_UART_USB, &c, 1)){

      uartSend(CIAA_UART_232, &c, 1);

   }

   return;
}


/** The sendATcmd function sends an AT command to the GSM engine.
 */

fsmEvent gsmSendCmd (const uint8_t * cmdstr)
{
   tknTypeParser_e sending;   /* classifies the command being sent */
   FSMresult result;  /* result of the updateFSM invocation */
   uint16_t idx;     /* index of the command to be sent in the command list */

   uint8_t aux = 0;
   uint8_t cmdStrCla[TKN_LEN];
   cmdStrCla[0] = '\0';
   strncat(cmdStrCla, cmdstr, strlen(cmdstr));

   if('\r' == cmdstr[strlen(cmdstr)-1]){
      aux = (uint8_t)ECHO;
   }
   else{
      aux = (uint8_t)SMS_BODY;
   }

   strncat(cmdStrCla, &aux, 1);

   uint8_t cmd[TKN_CMD_SIZ];   /* AT command  */
   uint8_t par[TKN_PAR_SIZ];   /* AT command arguments */

   sending = gsmParseTkn(cmdStrCla, cmd, par, strlen(cmdStrCla));
   idx = commSearch(cmd); /* search for command */

   if((65535 != idx) && ((sending >= AUTOBAUD) && (sending <= SMS_BODY_P))){

      switch(sending){ /* modify format depending on the AT command type, send
                       through serial port and call updateFSM function */

         case AUTOBAUD:

            rs232Print("AT\r");

            result = updateFSM(sending,"AT","",idx);

            break;

         case BASIC_CMD:

            rs232Print("AT");
            rs232Print(cmd);
            if(par != 0) {rs232Print(par);}
            rs232Print("\r");

            result = updateFSM(sending,cmd,par,idx);

            break;

         case BASIC_CMD_AMP:

            rs232Print("AT&");
            rs232Print(cmd);
            if(par != 0) {rs232Print(par);}
            rs232Print("\r");

            result = updateFSM(sending,cmd,par,idx);

            break;

         case EXT_CMD_TEST:

            rs232Print("AT+");
            rs232Print(cmd);
            rs232Print("=?\r");

            result = updateFSM(sending,cmd,par,idx);

            break;

         case EXT_CMD_WRITE:

            rs232Print("AT+");
            rs232Print(cmd);
            rs232Print("=");
            rs232Print(par);
            rs232Print("\r");

            result = updateFSM(sending,cmd,par,idx);

            break;

         case EXT_CMD_READ:

            rs232Print("AT+");
            rs232Print(cmd);
            rs232Print("?\r");

            result = updateFSM(sending,cmd,par,idx);

            break;

         case EXT_CMD_EXEC:

            rs232Print("AT+");
            rs232Print(cmd);
            rs232Print("\r");

            result = updateFSM(sending,cmd,par,idx);

            break;

         case SMS_BODY_P:

            rs232Print(par);
            rs232Print("\x1A");

            result = updateFSM(sending,cmd,par,idx);

            break;

      }

      return result;

   }

   else{

      debug(">>>engine<<<   TRIED TO SEND UKNOWN COMMAND\r\n");

      result = ERR_CMD_UKN;

      return result;

   }

}

rsp_t gsmGetCmdRsp (void)
{
   /* fetches the next command response; returns empty response if there are no more
      responses left */

   uint8_t rspAux[TKN_LEN+20];      /* auxiliary buffer for storing the response */
   ATresp dummy;
   uint16_t rspSiz;

   dummy.cmd[0] = '\0';
   dummy.param[0] = '\0';

   if(0 == VLRingBuffer_IsEmpty(&rspVlRb)){

      rspSiz = VLRingBuffer_Pop(&rspVlRb, (void *) rspAux, TKN_LEN+20);

      /* Find the dot which separated command from parameter and parse the */
      /* raw response into command and parameter strings */

      uint8_t dotPos = 0;
      uint8_t i;

      for (i = 0; i < TKN_LEN+20; i++){
         if ('.' == rspAux[i]){
            dotPos = i;
            break;
         }
      }

      if(0 != dotPos){

         strncpy(dummy.cmd, rspAux, dotPos);
         dummy.cmd[dotPos] = '\0';
         strncpy(dummy.param, &rspAux[dotPos+1], rspSiz-dotPos-1);
         dummy.param[rspSiz-dotPos-1] = '\0';

      }

   }

   return dummy;
}

uint8_t gsmGetNoCmdRsp (void)
{
   /* returns number of total command responses for the last command */

   return (VLRingBuffer_GetCount(&rspVlRb));
}

rsp_t gsmGetUrc (void)
{
   /* fetches the next URC; returns empty response if there are no more
      responses left */

   uint8_t urcAux[TKN_LEN+20];      /* auxiliary buffer for storing the urc */
   ATresp dummy;
   uint8_t urcSiz;

   dummy.cmd[0] = '\0';
   dummy.param[0] = '\0';

   if(0 == VLRingBuffer_IsEmpty(&urcVlRb)){

      urcSiz = VLRingBuffer_Pop(&urcVlRb, (void *) urcAux, TKN_LEN+20);

      /* Find the dot which separates command from parameter and parse the */
      /* raw response into command and parameter strings */

      uint8_t dotPos = 0;
      uint8_t i;

      for (i = 0; i < TKN_LEN+20; i++){
         if ('.' == urcAux[i]){
            dotPos = i;
            break;
         }
      }

      if(0 != dotPos){

         strncpy(dummy.cmd, urcAux, dotPos);
         dummy.cmd[dotPos] = '\0';
         strncpy(dummy.param, &urcAux[dotPos+1], urcSiz-dotPos-1);
         dummy.param[urcSiz-dotPos-1] = '\0';

      }

   }

   return dummy;
}

serialMode_e gsmCheckSerialMode(void)
{
   return serialMode;
}

void gsmChangeSerialMode(serialMode_e mode)
{
   serialMode = mode;
   return;
}

urcMode_e gsmCheckUrcMode(void)
{
   return urcMode;
}

void gsmChangeUrcMode(urcMode_e mode)
{
   urcMode = mode;
   return;
}

void gsmSetUrcCback(void (*Cback) (uint8_t const * const cmd, uint8_t const * const par))
{
   urcCback = Cback;
   return;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
