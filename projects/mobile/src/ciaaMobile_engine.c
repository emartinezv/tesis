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

#include "ciaaMobile_engine.h"

/*==================[macros and definitions]=================================*/

#define DEBUG_ENGINE // debug mode
#ifdef DEBUG_ENGINE
   #define debug(msg) dbgPrint(msg)
#else
   #define debug(msg)
#endif

/*==================[global data]============================================*/

/*==================[internal data declaration]==============================*/

/** @brief Command or data mode for the serial port */

static serialMode_e serialMode = COMMAND_MODE;

/** @brief Buffer for the auxiliary ring buffer */

static uint8_t tknRbBuffer[AUX_RB_SIZE];

/** @brief Auxiliary ring buffer*/

static RINGBUFF_T tknRb;

/** @brief Token VL ring buffer */

static VLRINGBUFF_T tknVlRb;

/** @brief Current state of the GSM engine */

static GSMstate GSMstatus = WAITING;

/** @brief Stores URC events */

static URCevent URCeventVector [URC_VECTOR_SIZE];

/** @brief Number of URC events stored */

static uint8_t URCevents = 0;

/** @brief Stores the responses to the active command */

static ATresp respVector[RESPVECTOR_SIZE];

/** @brief Number of response tokens in the last command executed */

static int8_t lastResp = -1;

/** @brief used for AT command timeout counter */

uint32_t timeout_count = 0; /* NO ES STATIC, VER SI ES PROBLEMATICO */

/*==================[internal functions declaration]=========================*/

/** @brief Updates the AT command FSM with the latest received token
*
*  @param received  Type of AT token as per the parse function
*  @param command   Main part of the AT token used as input
*  @param parameter Parameter part of the AT token used as input
*  @param index     Index of the AT command (for sending only)
*
*  @return Returns the result of the latest updateFSM invocation
*/

static FSMresult updateFSM (ATToken received,
                    uint8_t const * const command,
                    uint8_t const * const parameter,
                    uint8_t const index);

/** @brief Records a new URC event in the URC event vector
*
*  @param command   Main part of the URC
*  @param parameter Parameter part of the URC
*
*  @return Returns 1 if successful or 0 if vector is full
*/

static uint8_t recordURC (uint8_t const * const command,
                    uint8_t const * const parameter);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/** The updateFSM function updates the internal FSM which preserves the state
 *  of the  current command execution. The latest token received from the GSM
 *  engine (as processed by the parse function) is the input, and the state
 *  of the current command after the function call is the output.
 */

static FSMresult updateFSM (ATToken received, uint8_t const * const command,
                            uint8_t const * const parameter, uint8_t index)
{
   static uint8_t currCMD[TKN_LEN]; /* command being currently executed */
   static uint8_t currPAR[TKN_LEN]; /* parameter of the current command */
   static uint8_t currTKN;          /* number of response token being
                                       processed */
   static uint8_t idx;              /* index of the command in the list */

   switch(GSMstatus){

      case WAITING: /* initial state */

         idx = index;
         lastResp = -1;

         if ((AUTOBAUD <= received) && (SMS_BODY >= received)){ /* command sent by serial port */

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
               recordURC (command, parameter);
               debug(">>>engine<<<   URC detected\r\n");

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

         if ((AUTOBAUD <= received) && (SMS_BODY >= received)){

            uint8_t eqCMD = 1;
            uint8_t eqPAR = 1;
            eqCMD = strncmp(command, currCMD, strlen(currCMD));
            eqPAR = strncmp(parameter, currPAR, strlen(currPAR));

            if ((0 == eqCMD) && (0 == eqPAR)){
               GSMstatus = CMD_ACK;
               currTKN = 0;

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
               recordURC (command, parameter);
               debug(">>>engine<<<   URC detected\r\n");

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
         ;

         if ((BASIC_RSP <= received) && (EXT_RSP >= received)){

            if(1 == URCSearch(command)){
               recordURC (command, parameter);
               debug(">>>engine<<<   URC detected\r\n");

               return OK_URC;
            }

            else{

               /* store the response in the active command response vector */

               /*if(currTKN >= RESPVECTOR_SIZE){

               }  ERROR SI ME QUEDA GRANDE LA RESPUESTA EN RESPVECTOR...

               else*/

               strncpy(respVector[currTKN].cmd,command,strlen(command));
               respVector[currTKN].cmd[strlen(command)] = '\0';
               strncpy(respVector[currTKN].param,parameter,strlen(parameter));
               respVector[currTKN].param[strlen(parameter)] = '\0';

               debug(">>>engine<<<   RESP: ");
               debug(respVector[currTKN].cmd);
               debug("(");
               debug(respVector[currTKN].param);
               debug(")\r\n");

               lastResp++;
               currTKN++;

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

         else if ((AUTOBAUD <= received) && (SMS_BODY >= received)){
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
   if((URCevents < 0) || (URCevents >= URC_VECTOR_SIZE)){
      return 0;
   }
   else{
      strncpy(URCeventVector[URCevents].command, command, strlen(command));
      URCeventVector[URCevents].command[strlen(command)] = '\0';
      strncpy(URCeventVector[URCevents].parameter, parameter, strlen(parameter));
      URCeventVector[URCevents].parameter[strlen(parameter)] = '\0';
      URCevents ++;

      debug(">>>engine<<<   URC RECORDED: ");
      debug(command);
      debug("(");
      debug(parameter);
      debug(")\r\n");

      return 1;
   }
}

/*==================[external functions definition]==========================*/

void initEngine(void){

   initTokenizer();

   VLRingBuffer_Init(&tknVlRb, &tknRb, &tknRbBuffer, 1, AUX_RB_SIZE);

   return;

}

/** The processToken function checks is there are unread tokens in the token
 *  ring buffer. If so, it reads the oldest token, parses the token through the
 *  parse function and calls upon the updateFSM function with the result. It
 *  returns the result of the updateFSM invocation
 */

FSMresult processToken(void)
{
   detectTokens(&tknVlRb);

   ATToken received; /* classifies the received token*/
   FSMresult currCmd = NO_UPDATE; /* result of the updateFSM invocation */

   uint8_t token[TKN_LEN]; /* received token */
   uint8_t tknSize;        /* size of read token */
   uint8_t command[TKN_LEN]; /* AT command or response */
   uint8_t parameter[TKN_LEN]; /* AT command or response argument */

   if(0 == VLRingBuffer_IsEmpty(&tknVlRb)){

      tknSize = VLRingBuffer_Pop(&tknVlRb, &token, TKN_LEN);
      token[tknSize] = '\0';
      received = parse(token, command, parameter);              /* parse the token */
      currCmd = updateFSM(received, command, parameter, 0);     /* update FSM */

   }
   else if((GSMstatus != WAITING) && (timeout_count == 0)){

      currCmd = updateFSM(TIMEOUT, 0, 0, 0);     /* update FSM */

   }

   return currCmd;

}

/** When the serial port is in DATA_MODE, this function prints everything
 *  received through the 232-UART serial port to the USB-UART. This is
 *  merely a test function; the final version of the library needs to send
 *  these data to a user-defined buffer of some sort for processing.
 */

void printData(void){

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

FSMresult sendATcmd (const uint8_t * cmdstr)
{
   ATToken sending;   /* classifies the command being sent */
   FSMresult result;  /* result of the updateFSM invocation */
   uint16_t idx;     /* index of the command to be sent in the command list */

   uint8_t cmd[TKN_LEN];   /* AT command  */
   uint8_t par[TKN_LEN];   /* AT command arguments */

   sending = parse(cmdstr, cmd, par);
   idx = commSearch(cmd); /* search for command */

   if((65535 != idx) && ((sending >= AUTOBAUD) && (sending <= SMS_BODY))){

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

         case SMS_BODY:

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

ATresp getCmdResp (uint8_t index)
{
   /* fetches the next command response; returns 0 if there are no more
      response left */

   ATresp dummy;

   dummy.cmd[0] = '\0';
   dummy.param[0] = '\0';

   if(index > lastResp | index < 0){return dummy;}
   else {return respVector[index];}
}

uint8_t getNoCmdResp (void)
{
   /* returns number of total command responses for the last command */

   return (lastResp+1);
}

uint8_t readURC (uint8_t * const command,
                    uint8_t * const parameter)
{
   if((URCevents <= 0) || (URCevents > URC_VECTOR_SIZE)){
      return 0;
   }
   else{
      strncpy(command, URCeventVector[URCevents - 1].command, strlen(URCeventVector[URCevents - 1].command));
      command[strlen(URCeventVector[URCevents - 1].command)] = '\0';
      strncpy(parameter, URCeventVector[URCevents - 1].parameter, strlen(URCeventVector[URCevents - 1].parameter));
      parameter[strlen(URCeventVector[URCevents -1].parameter)] = '\0';
      URCevents --;

      debug(">>>engine<<<   URC READ: ");
      debug(command);
      debug("(");
      debug(parameter);
      debug(")\r\n");

      return 1;
   }
}

serialMode_e checkSerialMode(void){

   return serialMode;
}

void changeSerialMode(serialMode_e mode){;

   serialMode = mode;
   return;

}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
