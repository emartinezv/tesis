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

/** \addtogroup gsm
 ** @{ */

/*==================[inclusions]=============================================*/

#include "gsmEngine.h"

/*==================[macros and definitions]=================================*/

//#define DEBUG_ENGINE
#ifdef DEBUG_ENGINE
   #define debug(msg) gsmTermUartSend(msg, strlen(msg))
#else
   #define debug(msg)
#endif

//#define DEBUG_NOTIMEOUT // ignore command timeouts

/*==================[global data]============================================*/

/*==================[internal data declaration]==============================*/

/*---------------------------------------------------------------------------*/
/*                      Internal engine state variables                      */
/*---------------------------------------------------------------------------*/

/** @brief Current state of the engine */

static fsmState_e fsmState = WAITING;

/** @brief used for AT command timeout counter */

static uint32_t toutCnt = 0;

/** @brief Command or Data mode for the serial port */

static serialMode_e serialMode = COMMAND_MODE;

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
*  @param cmd       Command part of the token used as input
*  @param par       Parameter part of the token used as input
*  @param index     Index of the AT command in the commands vector (only when
*                   sending cmds)
*
*
*  @return Returns the event triggered by the updateFSM call
*/

static fsmEvent_e gsmUpdateFsm (tknTypeParser_e tknType,
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
 *  received from the GSM module or an AT command the user is trying to send.
 *  The token is already processed by the gsmParseTkn function, so we have its
 *  type in detail. The output of the function is the event triggered by the
 *  processing of the input token. State is internal to the FSM, but the upper
 *  layer (Interface) gets the events to see what just happened. All executions
 *  of the function return a fsmEvent_e type response of some sort.
 */

static fsmEvent_e gsmUpdateFsm (tknTypeParser_e tknType,
                                uint8_t const * const cmd,
                                uint8_t const * const par, uint8_t idx)
{
   static uint8_t currCmd[TKN_CMD_SIZE]; /* command being currently executed */
   static uint8_t currPar[TKN_PAR_SIZE]; /* parameter of the current command */
   static uint8_t idxSave;               /* save index of current command */

   uint8_t rspAux[TKN_CMD_SIZE+TKN_PAR_SIZE+1];  /* auxiliary buffer for storing
                                                  the latest response */

   switch(fsmState){

      /* This is the initial state of the FSM when the engine is initialized or
       * after a command has been closed.
       */

      case WAITING:

         /* If the token type is between AUTOBAUD AND SMS_BODY_P, we are
          * dealing with a command sent by the user */

         if ((AUTOBAUD <= tknType) && (SMS_BODY_P >= tknType)){

            toutCnt = commands[idx].timeout; /* load timeout counter for the
                                                specific command */

            idxSave = idx;                   /* save command index */

            debug(">>>engine<<<   TIMEOUT COUNTER UPDATED\r\n");

            /* Initialize current command buffers cmd and par and progress
             * to CMD_SENT state*/

            strncpy(currCmd,cmd,strlen(cmd));
            currCmd[strlen(cmd)] = '\0';
            strncpy(currPar,par,strlen(par));
            currPar[strlen(par)] = '\0';

            fsmState = CMD_SENT;

            debug(">>>engine<<<   COMMAND SENT: ");
            debug(cmd);
            debug("(");
            debug(par);
            debug(")\r\n");

            return OK_CMD_SENT; /* return 1.1 */

         }

         /* If the token type is BASIC_RSP or EXT_RESP, we may be dealing with
          * an URC. We check the list of recognized URCs and log it if it is
          * in that list. Otherwise, we raise an out-of-order event, as we got
          * a response without any command being currently executed. */

         else if ((BASIC_RSP == tknType) || (EXT_RSP == tknType)){

            /* Search in the recognized URCs list and log the URC if correct */

            if(1 == gsmUrcSearch(cmd)){

               debug(">>>engine<<<   URC detected\r\n");
               gsmRecordUrc (cmd, par);

               return OK_URC; /* return 1.2 */
            }

            else{
               debug(">>>engine<<<   RECEIVED RESPONSE, COMMAND EXPECTED\r\n");

               return ERR_OOO; /* return 1.3 */
            }

         }

         /* Any other sort of token is considered invalid */

         else{
            debug(">>>engine<<<   INVALID TOKEN\r\n");

            return ERR_TKN_INV; /* return 1.4 */

         }

         break;

      /* The user has sent a recognized command and we are waiting for the echo
       * from the modem.
       */

      case CMD_SENT:

         /* We check the echo from the modem and verify that it agrees with the
            command that was sent by comparing both the command and parameter
            strings. The token type must be between AUTOBAUD and SMS_BODY_P. */

         if ((AUTOBAUD <= tknType) && (SMS_BODY_P >= tknType)){

            uint8_t eqCmd = 1;
            uint8_t eqPar = 1;

            /* Compare the cmd and par part of the current command with the
             * received echo */

            eqCmd = strncmp(cmd, currCmd, strlen(currCmd));
            eqPar = strncmp(par, currPar, strlen(currPar));

            /* If the echo is congruent, flush the rspVlRb and progress to
             * CMD_ACK state */

            if ((0 == eqCmd) && (0 == eqPar)){
               VLRingBuffer_Flush(&rspVlRb); /* flush response VLRB since we
                                                have a new command */
               fsmState = CMD_ACK;

               debug(">>>engine<<<   COMMAND ACK\r\n");

               return OK_CMD_ACK; /* return 2.1 */
            }

            /* If the echo is incorrect, report error and go back to WAITING
             * state */

            else{
               fsmState = WAITING;

               debug(">>>engine<<<   COMMAND ECHO ERROR\r\n");

               return ERR_CMD_ECHO; /* return 2.2 */
            }
         }

         /* If the token type is BASIC_RSP or EXT_RESP, we may be dealing with
          * an URC. We check the list of recognized URCs and log it if it is
          * in that list. Otherwise, we raise an out-of-order event, as we got
          * a response without any command being currently executed. */

         else if ((BASIC_RSP == tknType) || (EXT_RSP == tknType)){

            /* Search in the recognized URCs list and log the URC if correct */

            if(1 == gsmUrcSearch(cmd)){
               debug(">>>engine<<<   URC detected\r\n");
               gsmRecordUrc (cmd, par);

               return OK_URC; /* return 2.3 */
            }

            else{
               fsmState = WAITING;

               debug(">>>engine<<<   RECEIVED RESPONSE, COMMAND ECHO EXPECTED\r\n");

               return ERR_CMD_ECHO; /* return 2.4 */
            }

         }

         /* If the token type is TIMEOUT, the maximum response time for this
          * specific command has been exceeded. We report the error and go back
          * to the WAITING state */

         else if (TIMEOUT == tknType){
               fsmState = WAITING;

               debug(">>>engine<<<   AT COMMAND TIMEOUT\r\n");

               return ERR_TIMEOUT; /* return 2.5 */
         }

         /* Any other sort of token is considered invalid */

         else{
            fsmState = WAITING;

            debug(">>>engine<<<   INVALID TOKEN RECEIVED: ");
            debug(cmd);
            debug("(");
            debug(par);
            debug(")\r\n");

            return ERR_TKN_INV; /* return 2.6 */
         }

         break;

      /* This is the third possible state of the FSM. We have correctly
       * received the echo and are now waiting for the responses to the
       * command. The command can be closed by either a successful closing
       * response, an error closing response or timeout. In all cases, we
       * return to the WAITING state.
       */

      case CMD_ACK:

         /* If the token type is BASIC_RSP or EXT_RESP, we may be dealing with
          * an URC. We check the list of recognized URCs and log it if it is
          * in that list. Otherwise, we consider it a response to the command
          * and add it to the rspVlRb. */

         if ((BASIC_RSP <= tknType) && (EXT_RSP >= tknType)){

            /* Search in the recognized URCs list and log the URC if correct */

            if(1 == gsmUrcSearch(cmd)){
               debug(">>>engine<<<   URC detected\r\n");
               gsmRecordUrc(cmd, par);

               return OK_URC; /* return 3.1 */
            }

            else{

               /* Store the response in the rspAux auxiliary variable and
                * insert it in the rspVlRb. */

               rspAux[0] = '\0';

               strncat(rspAux, cmd, strlen(cmd));
               strncat(rspAux, ".", 1);
               strncat(rspAux, par, strlen(par));

               VLRingBuffer_Insert(&rspVlRb, rspAux, (uint16_t) (strlen(rspAux)));

               debug(">>>engine<<<   rspAux: ");
               debug(rspAux);
               debug("\r\n");

               uint8_t auxCmd[22]; /* aux variable to format the cmd so that
                                      it includes the closing response
                                      separator characters */

               auxCmd[0]='-';
               auxCmd[1]='\0';
               strncat(auxCmd, cmd, strlen(cmd));
               strncat(auxCmd, "-", strlen("-"));

               /* Compare current response with the string of valid error    */
               /* end responses for the current command. If a match is       */
               /* detected, close command and report ERR_MSG_CLOSE           */

               if(NULL != strstr(commands[idxSave].errRsp,auxCmd)){

                  debug(">>>engine<<<   COMMAND CLOSED IN ERROR\r\n");

                  fsmState = WAITING;
                  return ERR_MSG_CLOSE; /* return 3.2 */
               }

               /* Compare current response with the string of valid          */
               /* successful end responses for the current command. If a     */
               /* match is detected, close command and report OK_CLOSE.      */

               else if(NULL != strstr(commands[idxSave].sucRsp,auxCmd)){

                  debug(">>>engine<<<   COMMAND CLOSED SUCCESSFULLY\r\n");

                  fsmState = WAITING;
                  return OK_CLOSE; /* return 3.3 */
               }

               /* CORNER CASE: AT+CIFSR does not return an OK after
                * reporting its response. To handle this and other commands
                * with this non-standard behavior, we make the sucRsp string
                * in it's command definition empty and we check for that here.
                * If the sucRsp string is empty, we consider that the single
                * response of the command (already saved in rspVlRb) to also
                * be a closing response.
                */

               else if(0 == strlen(commands[idxSave].sucRsp)){

                  debug(">>>engine<<<   COMMAND CLOSED SUCCESSFULLY\r\n");

                  fsmState = WAITING;
                  return OK_CLOSE; /* return 3.4 */
               }

               /* If the response is not an end response, just report
                * OK_RESP */

               else{

                  return OK_RSP; /* return 3.5 */
               }
            }

         }

         /* If we get a command-type token, we report an out-of-order error and
          * go back to the WAITING state */

         else if ((AUTOBAUD <= tknType) && (SMS_BODY_P >= tknType)){
            fsmState = WAITING;

            debug(">>>engine<<<   RECEIVED COMMAND, RESPONSE EXPECTED\r\n");

            return ERR_OOO; /* return 3.6 */
         }

         /* If the token type is TIMEOUT, the maximum response time for this
          * specific command has been exceeded. We report the error and go back
          * to the WAITING state */

         else if (TIMEOUT == tknType){
               fsmState = WAITING;

               debug(">>>engine<<<   AT COMMAND TIMEOUT\r\n");

               return ERR_TIMEOUT; /* return 3.7 */
         }

         /* Any other sort of token is considered invalid */

         else{

            debug(">>>engine<<<   INVALID TOKEN RECEIVED: ");
            debug(cmd);
            debug("(");
            debug(par);
            debug(")\r\n");

            return ERR_TKN_INV; /* return 3.8 */
         }

         break;

      /* If for some reason we reach the default case the FSM has been
       * corrupted. We report this and go back to the WAITING state. */

      default:

         debug(">>>engine<<<   ERROR: FSM OUT OF RANGE");

         fsmState = WAITING;
         return ERR_FSM_OOR; /* return 3.9 */

         break;
   }

}

/** The gsmRecordUrc function records received URCs in a VLRB */

static uint8_t gsmRecordUrc (uint8_t const * const command,
                    uint8_t const * const parameter)
{
   if(0 == VLRingBuffer_IsFull(&urcVlRb)){

     uint8_t urcAux[TKN_LEN]; /* auxiliary vector */

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

/*==================[external functions definition]==========================*/

/* The gsmInitEngine function initializes the tokenizer module, the serial
 * comms and all three variable-length ring buffers.
 */

void gsmInitEngine(void){

   gsmInitTokenizer();

   gsmSetSerialMode(COMMAND_MODE); /* start serial comms in command mode */

   VLRingBuffer_Init(&tknVlRb, &tknRb, &tknRbBuf, 1, TKN_BUF_SIZE);
   VLRingBuffer_Init(&rspVlRb, &rspRb, &rspRbBuf, 1, RSP_BUF_SIZE);
   VLRingBuffer_Init(&urcVlRb, &urcRb, &urcRbBuf, 1, URC_BUF_SIZE);

   return;

}

/** The processToken function checks if there are unread tokens in the token
 *  ring buffer. If so, it reads the oldest token, parses the token through the
 *  parse function and calls upon the gsmUpdateFsm function with the result. It
 *  then passes along the return value of the gsmUpdateFsm call.
 */

fsmEvent_e gsmProcessTkn(void)
{
   gsmDetectTkns(&tknVlRb);

   tknTypeParser_e received;       /* stores the received token type */
   fsmEvent_e result = NO_UPDATE;  /* stores the resulting event of the
                                      gsmUpdateFsm call */

   uint8_t tkn[TKN_LEN]; /* received token */
   int tknSize;          /* size of read token */
   uint8_t cmd[TKN_CMD_SIZE]; /* AT command or response */
   uint8_t par[TKN_PAR_SIZE]; /* AT command or response argument */

   /* If the tknVlRb is not empty, pop the latest token, parse ir and send it
    * as an argument to the gsmUpdateFsm function to update the FSM. */

   if(0 == VLRingBuffer_IsEmpty(&tknVlRb)){

      tknSize = VLRingBuffer_Pop(&tknVlRb, &tkn, TKN_LEN);/* pop latest tkn */
      received = gsmParseTkn(tkn, cmd, par, tknSize);     /* parse the tkn */
      result = gsmUpdateFsm(received, cmd, par, 0);       /* update FSM */

   }

   #ifndef DEBUG_NOTIMEOUT

   /* If the timeout counter has dropped to 0, call the gsmUpdateFsm function
    * with a TIMEOUT token type. */

   else if((fsmState != WAITING) && (toutCnt == 0)){

      result = gsmUpdateFsm(TIMEOUT, 0, 0, 0);     /* update FSM */

   }

   #endif

   return result;

}

uint8_t gsmToutCntZero(void){

   return(toutCnt == 0);

}

void gsmDecToutCnt(void){

   if(toutCnt > 0){
      toutCnt--;
   }

   return;
}

/** When the serial port is in DATA_MODE, this function prints everything
 *  received through the 232-UART serial port to the Term-UART. This is
 *  merely a test function; the user of the library needs to send
 *  these data to a user-defined buffer of some sort for processing.
 */

void gsmPrintData(void){

   uint8_t c = 0;

   if(0 != gsm232UartRecv(&c, 1)){

      gsmTermUartSend(&c, 1);

   }

   if(0 != gsmTermUartRecv(&c, 1)){

      gsm232UartSend(&c, 1);

   }

   return;
}


/** The gsmSendCmd function first parses cmdStr in the same way as we would do
 *  with a token received from the GSM module. This is needed since we are
 *  going to use the token to update the FSM. We parse cmdStr, make sure that
 *  it is a recognized command and write it in the correct format to the
 *  serial port. Afterwards, gsmUpdateFsm is called.
 */

fsmEvent_e gsmSendCmd (const uint8_t * cmdStr)
{
   tknTypeParser_e sending;   /* classifies the command being sent */
   fsmEvent_e result;         /* result of the updateFSM invocation */
   uint16_t idx;              /* index of the command to be sent in
                                 the commands vector */

   uint8_t aux = 0;            /* auxiliary variable*/
   uint8_t cmdStrCla[TKN_LEN]; /* auxiliary vector for expanded cmd string*/
   cmdStrCla[0] = '\0';
   strncat(cmdStrCla, cmdStr, strlen(cmdStr));

   /* Add ECHO (if tkn finishes with '\r') or SMS_BODY to the end of the
    * expanded cmd string to aid the parser.
    */

   if('\r' == cmdStr[strlen(cmdStr)-1]){
      aux = (uint8_t)ECHO;
   }
   else{
      aux = (uint8_t)SMS_BODY;
   }

   strncat(cmdStrCla, &aux, 1);

   uint8_t cmd[TKN_CMD_SIZE];   /* AT command  */
   uint8_t par[TKN_PAR_SIZE];   /* AT command arguments */

   /* Parse AT cmd and check if it is valid. */

   sending = gsmParseTkn(cmdStrCla, cmd, par, strlen(cmdStrCla));
   idx = gsmCmdSearch(cmd); /* search for command */

   /* If the command is valid, send it through the serial port and then update
    * the FSM by invoking gsmUpdateFsm. */

   if((UNKNOWN_CMD != idx) && ((sending >= AUTOBAUD) &&
      (sending <= SMS_BODY_P))){

      gsm232UartSend(cmdStr, strlen(cmdStr)); /* Send the cmd string through the serial port */

      if(SMS_BODY_P == sending){

         gsm232UartSend("\x1A", strlen("\x1A")); /* If we are sending an SMS_BODY type token we
                                need to add the final Ctrl-Z char */

      }

      debug(">>>engine<<<   SENT COMMAND ");
      debug(cmdStr);
      debug("\r\n");

      result = gsmUpdateFsm(sending,cmd,par,idx); /* update FSM */

   }

   else{

      debug(">>>engine<<<   TRIED TO SEND UKNOWN COMMAND\r\n");

      result = ERR_CMD_UKN;

   }

   return result;

}

rsp_t gsmGetCmdRsp (void)
{
   /* fetches the next command response; returns empty response if there are no more
      responses left */

   uint8_t rspAux[TKN_CMD_SIZE+TKN_PAR_SIZE]; /* aux buffer for the rsp str */
   rsp_t dummy;                               /* aux rsp variable */
   uint16_t rspSiz;                           /* rsp size */

   dummy.cmd[0] = '\0';
   dummy.par[0] = '\0';

   /* If the rspVlRb is not empty, pop a response and separate it into cmd and
    * par strings by looking for the '.' char which acts as a separator.
    */

   if(0 != VLRingBuffer_IsEmpty(&rspVlRb)){
      return dummy;
   }

   rspSiz = VLRingBuffer_Pop(&rspVlRb, (void *) rspAux, TKN_CMD_SIZE+TKN_PAR_SIZE);

   /* Find the dot which separated command from parameter and separate the
   /* raw response into command and parameter strings */

   uint8_t dotPos = 0;
   uint8_t i;

   for (i = 0; i < (TKN_CMD_SIZE+TKN_PAR_SIZE); i++){
      if ('.' == rspAux[i]){
         dotPos = i;
         break;
      }
   }

   /* Copy the cmd and par strings in the respective parts of the dummy rsp
    * variable.
    */

   if(0 != dotPos){

      strncpy(dummy.cmd, rspAux, dotPos);
      dummy.cmd[dotPos] = '\0';
      strncpy(dummy.par, &rspAux[dotPos+1], rspSiz-dotPos-1);
      dummy.par[rspSiz-dotPos-1] = '\0';

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

   uint8_t urcAux[TKN_CMD_SIZE+TKN_PAR_SIZE]; /* aux buffer for the urc str */
   rsp_t dummy;                               /* aux rsp variable */
   uint16_t urcSiz;                           /* urc size */

   dummy.cmd[0] = '\0';
   dummy.par[0] = '\0';

   /* If the urcVlRb is not empty, pop an urc and separate it into cmd and par
    * strings by looking for the '.' char which acts as a separator.
    */

   if(0 == VLRingBuffer_IsEmpty(&urcVlRb)){

      urcSiz = VLRingBuffer_Pop(&urcVlRb, (void *) urcAux, TKN_CMD_SIZE+TKN_PAR_SIZE);

      /* Find the dot which separates command from parameter and separate the
      /* raw urc into command and parameter strings */

      uint8_t dotPos = 0;
      uint8_t i;

      for (i = 0; i < TKN_LEN+20; i++){
         if ('.' == urcAux[i]){
            dotPos = i;
            break;
         }
      }

      /* Copy the cmd and par strings in the respective parts of the dummy rsp
       * variable.
       */

      if(0 != dotPos){

         strncpy(dummy.cmd, urcAux, dotPos);
         dummy.cmd[dotPos] = '\0';
         strncpy(dummy.par, &urcAux[dotPos+1], urcSiz-dotPos-1);
         dummy.par[urcSiz-dotPos-1] = '\0';

      }

   }

   return dummy;
}

serialMode_e gsmGetSerialMode(void)
{
   return serialMode;
}

void gsmSetSerialMode(serialMode_e mode)
{
   serialMode = mode;
   return;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
