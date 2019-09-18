/* Copyright 2019, Ezequiel Martinez Vazquez
 * All rights reserved.
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

/** \addtogroup engine engine
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

/* Ignore command timeouts (for debugging) */
//#define DEBUG_NOTIMEOUT

/*==================[global data]============================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/** @brief Updates the engine FSM with a token as input
*
*  @param engine  : Pointer to engine
*  @param tknType : Type of token as per the gsmParseTkn function
*  @param cmd     : Command part of the token used as input
*  @param par     : Parameter part of the token used as input
*  @param index   : Index of the AT command (only when sending cmds, 0
*                    otherwise)
*
*  @return Event triggered by the updateFSM call
*/

static fsmEvent_t gsmUpdateFsm (gsmEngine_t * const engine,
                                tknTypeParser_t tknType,
                                uint8_t const * const cmd,
                                uint8_t const * const par, uint8_t index);

/** @brief Records a new URC in the URC VLRB
*
*  @param cmd : Command part of the URC
*  @param par : Parameter part of the URC
*
*  @return True if successful
*/

static bool gsmRecordUrc (gsmEngine_t * engine , uint8_t const * const cmd,
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
 *  of the function return a fsmEvent_t type response of some sort.
 */

static fsmEvent_t gsmUpdateFsm (gsmEngine_t * const engine,
                                tknTypeParser_t tknType,
                                uint8_t const * const cmd,
                                uint8_t const * const par, uint8_t idx)
{
   uint8_t rspAux[TKN_CMD_SIZE+TKN_PAR_SIZE+1];

   switch(engine->fsmState){

      /* This is the initial state of the FSM when the engine is initialized or
       * after a command has been closed.
       */

      case WAITING:

         /* If the token type is between AUTOBAUD AND SMS_BODY_P, we have a
          * command sent by the user */

         if ((AUTOBAUD <= tknType) && (SMS_BODY_P >= tknType)){

            engine->toutCnt = gsmGetCmdTimeout(idx);

            engine->currIdx = idx;

            debug(">>>engine<<<   TIMEOUT COUNTER UPDATED\r\n");

            /* Initialize current command buffers cmd and par and progress to
             * CMD_SENT state*/

            strncpy(engine->currCmd,cmd,strlen(cmd));
            engine->currCmd[strlen(cmd)] = '\0';
            strncpy(engine->currPar,par,strlen(par));
            engine->currPar[strlen(par)] = '\0';

            engine->fsmState = CMD_SENT;

            debug(">>>engine<<<   COMMAND SENT: ");
            debug(cmd);
            debug("(");
            debug(par);
            debug(")\r\n");

            return OK_CMD_SENT; /* return 1.1 */

         }

         /* If the token type is BASIC_RSP or EXT_RESP, we may have an URC. We
          * check the list of recognized URCs and log it if it is in that list.
          * Otherwise, we raise an out-of-order event, as we got a response
          * without any command being currently executed. */

         else if ((BASIC_RSP == tknType) || (EXT_RSP == tknType)){

            if(true == gsmUrcSearch(cmd)){

               debug(">>>engine<<<   URC detected\r\n");
               gsmRecordUrc (engine, cmd, par);

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
            command that was sent. */

         if ((AUTOBAUD <= tknType) && (SMS_BODY_P >= tknType)){

            uint8_t eqCmd = 1;
            uint8_t eqPar = 1;

            eqCmd = strncmp(cmd, engine->currCmd, strlen(engine->currCmd));
            eqPar = strncmp(par, engine->currPar, strlen(engine->currPar));

            /* If the echo is congruent, flush the rspVlRb and progress to
             * CMD_ACK state */

            if ((0 == eqCmd) && (0 == eqPar)){
               VLRingBuffer_Flush(&(engine->rspVlRb));
               engine->fsmState = CMD_ACK;

               debug(">>>engine<<<   COMMAND ACK\r\n");

               return OK_CMD_ACK; /* return 2.1 */
            }

            /* If the echo is incorrect, report error and go back to WAITING
             * state */

            else{
               engine->fsmState = WAITING;

               debug(">>>engine<<<   COMMAND ECHO ERROR\r\n");

               return ERR_CMD_ECHO; /* return 2.2 */
            }
         }

         /* If the token type is BASIC_RSP or EXT_RESP, we may have an URC. We
          * check the list of recognized URCs and log it if it is in that list.
          * Otherwise, we raise an out-of-order event, as we got a response
          * without any command being currently executed. */

         else if ((BASIC_RSP == tknType) || (EXT_RSP == tknType)){

            if(true == gsmUrcSearch(cmd)){
               debug(">>>engine<<<   URC detected\r\n");
               gsmRecordUrc(engine, cmd, par);

               return OK_URC; /* return 2.3 */
            }

            else{
               engine->fsmState = WAITING;

               debug(">>>engine<<<   RECEIVED RESPONSE, "
                     "COMMAND ECHO EXPECTED\r\n");

               return ERR_CMD_ECHO; /* return 2.4 */
            }

         }

         /* If the token type is TIMEOUT, the maximum response time for this
          * specific command has been exceeded. We report the error and go back
          * to the WAITING state */

         else if (TIMEOUT == tknType){
               engine->fsmState = WAITING;

               debug(">>>engine<<<   AT COMMAND TIMEOUT\r\n");

               return ERR_TIMEOUT; /* return 2.5 */
         }

         /* Any other sort of token is considered invalid */

         else{
            engine->fsmState = WAITING;

            debug(">>>engine<<<   INVALID TOKEN RECEIVED: ");
            debug(cmd);
            debug("(");
            debug(par);
            debug(")\r\n");

            return ERR_TKN_INV; /* return 2.6 */
         }

         break;

      /* Echo was OK and we are now waiting for the responses to the
       * command. The command can be closed by either a successful closing
       * response, an error closing response or timeout. In all cases, we
       * return to the WAITING state.
       */

      case CMD_ACK:

         /* If the token type is BASIC_RSP or EXT_RESP, we may be dealing with
          * an URC. We check the list of recognized URCs and log it if it is
          * in that list. Otherwise, we consider it a response to the command
          * and add it to the response VLRB. */

         if ((BASIC_RSP <= tknType) && (EXT_RSP >= tknType)){

            if(true == gsmUrcSearch(cmd)){
               debug(">>>engine<<<   URC detected\r\n");
               gsmRecordUrc(engine, cmd, par);

               return OK_URC; /* return 3.1 */
            }

            else{

               rspAux[0] = '\0';

               /* We store responses as a single string, with a dot separating
                  the cmd and par parts */

               strncat(rspAux, cmd, strlen(cmd));
               strncat(rspAux, ".", 1);
               strncat(rspAux, par, strlen(par));

               VLRingBuffer_Insert(&(engine->rspVlRb), rspAux,
                                   (uint16_t) (strlen(rspAux)));

               debug(">>>engine<<<   rspAux: ");
               debug(rspAux);
               debug("\r\n");

               uint8_t auxCmd[22];

               /* We alter the cmd in an aux variable so that it includes the
                * closing response separator characters '-'  */

               auxCmd[0]='-';
               auxCmd[1]='\0';
               strncat(auxCmd, cmd, strlen(cmd));
               strncat(auxCmd, "-", strlen("-"));

               /* Compare current response with the string of valid error    */
               /* end responses for the current command. If a match is       */
               /* detected, close command and report ERR_MSG_CLOSE           */

               if(NULL != strstr(gsmGetCmdErrRsp(engine->currIdx),auxCmd)){

                  debug(">>>engine<<<   COMMAND CLOSED IN ERROR\r\n");

                  engine->fsmState = WAITING;
                  return ERR_MSG_CLOSE; /* return 3.2 */
               }

               /* Compare current response with the string of valid          */
               /* successful end responses for the current command. If a     */
               /* match is detected, close command and report OK_CLOSE.      */

               else if(NULL != strstr(gsmGetCmdSucRsp(engine->currIdx),auxCmd)){

                  debug(">>>engine<<<   COMMAND CLOSED SUCCESSFULLY\r\n");

                  engine->fsmState = WAITING;
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

               else if(0 == strlen(gsmGetCmdSucRsp(engine->currIdx))){

                  debug(">>>engine<<<   COMMAND CLOSED SUCCESSFULLY\r\n");

                  engine->fsmState = WAITING;
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
            engine->fsmState = WAITING;

            debug(">>>engine<<<   RECEIVED COMMAND, RESPONSE EXPECTED\r\n");

            return ERR_OOO; /* return 3.6 */
         }

         /* If the token type is TIMEOUT, the maximum response time for this
          * specific command has been exceeded. We report the error and go back
          * to the WAITING state */

         else if (TIMEOUT == tknType){
               engine->fsmState = WAITING;

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

         engine->fsmState = WAITING;
         return ERR_FSM_OOR; /* return 3.9 */

         break;
   }

}

static bool gsmRecordUrc (gsmEngine_t * engine,
                             uint8_t const * const command,
                             uint8_t const * const parameter)
{
   if(0 == VLRingBuffer_IsFull(&(engine->urcVlRb))){

     uint8_t urcAux[TKN_LEN];

     urcAux[0] = '\0';

     /* We store URCs as a single string, with a dot separating the cmd and par
        parts */

     strncat(urcAux, command, strlen(command));
     strncat(urcAux, ".", 1);
     strncat(urcAux, parameter, strlen(parameter));

     VLRingBuffer_Insert(&(engine->urcVlRb), urcAux,
                         (uint16_t) (strlen(urcAux)));

     debug(">>>engine<<<   urcAux: ");
     debug(urcAux);
     debug("\r\n");

     return true;
  }

  else{

     debug(">>>engine<<<   URC VLRB lleno!\r\n");

     return false;
  }

}

/*==================[external functions definition]==========================*/

/* The gsmInitEngine function initializes the tokenizer module, the engine FSM,
 * the serial comms state and all three variable-length ring buffers.
 */

bool gsmInitEngine(gsmEngine_t * const engine){

   bool ok = FALSE;

   ok = gsmInitTokenizer();

   engine->fsmState = WAITING;

   engine->serialMode = COMMAND_MODE;

   ok = ok && VLRingBuffer_Init(&(engine->tknVlRb), &(engine->tknRb),
                                &(engine->tknRbBuf), 1, TKN_BUF_SIZE);
   ok = ok && VLRingBuffer_Init(&(engine->rspVlRb), &(engine->rspRb),
                                &(engine->rspRbBuf), 1, RSP_BUF_SIZE);
   ok = ok && VLRingBuffer_Init(&(engine->urcVlRb), &(engine->urcRb),
                                &(engine->urcRbBuf), 1, URC_BUF_SIZE);

   return ok;

}

/** The processToken function reads the UART, calls the gsmDetectTkns function
 *  and then checks if there are unread tokens in the token ring buffer. If so,
 *  it reads the oldest token, parses the token through the parse function and
 *  calls upon the gsmUpdateFsm function with the result. It then passes along
 *  the return value of the gsmUpdateFsm call.
 */

fsmEvent_t gsmProcessTkn(gsmEngine_t * const engine)
{
   uint16_t nch;
   uint8_t buffer[RD_BUF_SIZ];

   nch = gsm232UartRecv(buffer, gsmNoChTokenizer());

   gsmDetectTkns(&(engine->tknVlRb), nch, buffer);

   tknTypeParser_t received;
   fsmEvent_t result = NO_UPDATE;

   uint8_t tkn[TKN_LEN];
   int tknSize;
   uint8_t cmd[TKN_CMD_SIZE+1];
   uint8_t par[TKN_PAR_SIZE+1];

   if(0 == VLRingBuffer_IsEmpty(&(engine->tknVlRb))){

      tknSize = VLRingBuffer_Pop(&(engine->tknVlRb),
                                 &tkn, TKN_LEN);
      received = gsmParseTkn(tkn, cmd, par, tknSize);
      result = gsmUpdateFsm(engine, received, cmd, par, 0);

   }

   #ifndef DEBUG_NOTIMEOUT

   else if((engine->fsmState != WAITING) && (0 == engine->toutCnt)){

      result = gsmUpdateFsm(engine, TIMEOUT, 0, 0, 0);

   }

   #endif

   return result;

}

bool gsmToutCntZero(gsmEngine_t * const engine){

   return(engine->toutCnt == 0);

}

void gsmDecToutCnt(gsmEngine_t * const engine){

   if(engine->toutCnt > 0){
      engine->toutCnt--;
   }

   return;
}

/** The gsmSendCmd parses cmdStr, makes sure that it is a recognized command
 *  and writes it in the correct format to the serial port. Afterwards,
 *  gsmUpdateFsm is called.
 */

fsmEvent_t gsmSendCmd (gsmEngine_t * const engine,
                       const uint8_t * const cmdStr)
{
   tknTypeParser_t sending;
   fsmEvent_t result;
   uint16_t idx;


   uint8_t aux = 0;
   uint8_t cmdStrCla[TKN_LEN];
   cmdStrCla[0] = '\0';
   strncat(cmdStrCla, cmdStr, strlen(cmdStr));

   /* Add ECHO (if tkn finishes with '\r') or SMS_BODY to the end of the
    * expanded cmd string (needed for the parser) */

   if('\r' == cmdStr[strlen(cmdStr)-1]){
      aux = (uint8_t)ECHO;
   }
   else{
      aux = (uint8_t)SMS_BODY;
   }

   strncat(cmdStrCla, &aux, 1);

   uint8_t cmd[TKN_CMD_SIZE];
   uint8_t par[TKN_PAR_SIZE];

   /* Parse AT cmd and check if it is valid. */

   sending = gsmParseTkn(cmdStrCla, cmd, par, strlen(cmdStrCla));
   idx = gsmCmdSearch(cmd);

   /* If the command is valid, send it through the serial port and then call
    * gsmUpdateFsm. */

   if((UNKNOWN_CMD != idx) && ((sending >= AUTOBAUD) &&
      (sending <= SMS_BODY_P))){

      gsm232UartSend(cmdStr, strlen(cmdStr));

      if(SMS_BODY_P == sending){

         /* If sending an SMS_BODY token we need to add a final Ctrl-Z char */
         gsm232UartSend("\x1A", strlen("\x1A"));


      }

      debug(">>>engine<<<   SENT COMMAND ");
      debug(cmdStr);
      debug("\r\n");

      result = gsmUpdateFsm(engine, sending, cmd, par, idx);

   }

   else{

      debug(">>>engine<<<   TRIED TO SEND UKNOWN COMMAND\r\n");

      result = ERR_CMD_UKN;

   }

   return result;

}

rsp_t gsmGetCmdRsp (gsmEngine_t * const engine)
{
   uint8_t rspAux[TKN_CMD_SIZE+TKN_PAR_SIZE];
   rsp_t dummy;
   uint16_t rspSiz;

   dummy.cmd[0] = '\0';
   dummy.par[0] = '\0';

   /* If the rspVlRb is not empty, pop a response and separate it into cmd and
    * par strings by looking for the '.' char which acts as a separator.
    */

   if(0 != VLRingBuffer_IsEmpty(&(engine->rspVlRb))){
      return dummy;
   }

   rspSiz = VLRingBuffer_Pop(&(engine->rspVlRb), (void *) rspAux,
                             TKN_CMD_SIZE+TKN_PAR_SIZE);

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

uint8_t gsmGetNoCmdRsp (const gsmEngine_t * const engine)
{
   return (VLRingBuffer_GetCount(&(engine->rspVlRb)));
}

rsp_t gsmGetUrc (gsmEngine_t * const engine)
{
   uint8_t urcAux[TKN_CMD_SIZE+TKN_PAR_SIZE];
   rsp_t dummy;
   uint16_t urcSiz;

   dummy.cmd[0] = '\0';
   dummy.par[0] = '\0';

   /* If the urcVlRb is not empty, pop an urc and separate it into cmd and par
    * strings by looking for the '.' char which acts as a separator.
    */

   if(0 == VLRingBuffer_IsEmpty(&(engine->urcVlRb))){

      urcSiz = VLRingBuffer_Pop(&(engine->urcVlRb),
                                (void *) urcAux, TKN_CMD_SIZE+TKN_PAR_SIZE);

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

serialMode_t gsmGetSerialMode(const gsmEngine_t * const engine)
{
   return engine->serialMode;
}

bool gsmSetSerialMode(gsmEngine_t * const engine, serialMode_t mode)
{
   engine->serialMode = mode;
   return (mode == engine->serialMode);
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
