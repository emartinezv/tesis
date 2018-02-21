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

/** @brief Current state of the GSM engine */

static GSMstate GSMstatus = WAITING;

/** @brief Stores URC events */

static URCevent URCeventVector [URC_VECTOR_SIZE];

/** @brief Number of URC events stored */

static uint8_t URCevents = 0;

/** @brief Stores the responses to the active command */

static uint8_t respVector[TKN_BUF_SIZE][TKN_LEN];

/** @brief Number of response tokens in the last command executed */

static int8_t lastResp = -1;

/*==================[internal functions declaration]=========================*/

/** @brief Updates the AT command FSM with the latest received token
*
*  @param received  Type of AT token as per the parse function
*  @param command   Main part of the AT token used as input
*  @param parameter Parameter part of the AT token used as input
*
*  @return Returns the result of the latest updateFSM invocation
*/

static FSMresult updateFSM (ATToken received,
                    uint8_t const * const command,
                    uint8_t const * const parameter);

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

static FSMresult updateFSM (ATToken received,
               uint8_t const * const command,
               uint8_t const * const parameter)
{
   static uint8_t currCMD[TKN_LEN]; /* command being currently executed */
   static uint8_t currPAR[TKN_LEN]; /* parameter of the current command */
   static uint8_t currTKN;          /* number of response token being
                                       processed */

   static uint8_t i = 65535;        /* command indexing variable */

   switch(GSMstatus){

      case WAITING: /* initial state */

         lastResp = -1;

         if ((received >= AUTOBAUD) && (received <= SMS_BODY)){ /* command sent by serial port */

            i = commSearch(command); /* search for command */

            if(65535 != i){

               /* if the command is valid, copy command and parameter into
                  currCMD and currPAR; this is the command now being
                  executed by the FSM  */

               strncpy(currCMD,command,strlen(command));
               currCMD[strlen(command)] = '\0';
               strncpy(currPAR,parameter,strlen(parameter));
               currPAR[strlen(parameter)] = '\0';

               //if (0 == strncmp(command, "SMS_BODY", strlen(currCMD))){
               //   GSMstatus = CMD_ACK; /* The Ctrl-Z char that closes SMS body  */
               //                        /* messages is not echoed, so ack is not */
               //                        /* viable                                */
               //}
               //else{
               //   GSMstatus = CMD_SENT; /* ack received, command has been sent */
               //} AHORA SMS_BODY SI TIENE ECO, BORRAR ESTO MAS TARDE

               GSMstatus = CMD_SENT;

               debug(">>>engine<<<   COMMAND SENT: ");
               debug(command);
               debug("(");
               debug(parameter);
               debug(")\r\n");

               return OK_CMD_SENT;

            }

            else

               debug(">>>engine<<<   UNKNOWN COMMAND\r\n");

               return ERR_CMD_UKN;

         }

         else if ((received = BASIC_RSP) || (received = EXT_RSP)){ /* possible URC */

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

         else{
            debug(">>>engine<<<   INVALID TOKEN\r\n");

            return ERR_TKN_INV;

         }
         break;

      case CMD_SENT:

         /* now that the command has been sent, we check the echo from the
            modem and verify that it agrees with the sent command */

         if ((received >= AUTOBAUD) && (received <= SMS_BODY)){

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

         else if ((received = BASIC_RSP) || (received = EXT_RSP)){ /* possible URC */

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

         if ((received >= BASIC_RSP) && (received <= EXT_RSP)){

            if(1 == URCSearch(command)){
               recordURC (command, parameter);
               debug(">>>engine<<<   URC detected\r\n");

               return OK_URC;
            }

            else{

               /* store the response in the active command response vector and
                  check if it is an end response */

               strncpy(respVector[currTKN],command,strlen(command));
               respVector[currTKN][strlen(command)] = '\0';
               strncat(respVector[currTKN],"(",1);
               strncat(respVector[currTKN],parameter,strlen(parameter));
               strncat(respVector[currTKN],")",1);

               debug(">>>engine<<<   RESP: ");
               debug(respVector[currTKN]);
               debug("\r\n");

               lastResp++;
               currTKN++;

               uint8_t * place;

               place = strstr(commands[i].endresp,command);

               if(NULL != place){

                  debug(">>>engine<<<   COMMAND CLOSED\r\n");

                  GSMstatus = WAITING;
                  return OK_CLOSE;
               }
               else{
                  return OK_RESP;
               }
            }

         }

         else if ((received >= AUTOBAUD) && (received <= SMS_BODY)){
            GSMstatus = WAITING;

            debug(">>>engine<<<   RECEIVED COMMAND, RESPONSE EXPECTED\r\n");

            return ERR_OOO;
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

/** The processToken function checks is there are unread tokens in the token
 *  ring buffer. If so, it reads the oldest token, parses the token through the
 *  parse function and calls upon the updateFSM function with the result. It
 *  returns the result of the updateFSM invocation
 */

FSMresult processToken(void)
{
   ATToken received; /* classifies the received token*/
   FSMresult currCmd = NO_UPDATE; /* result of the updateFSM invocation */

   uint8_t token[TKN_LEN]; /* received token */
   uint8_t command[TKN_LEN]; /* AT command or response */
   uint8_t parameter[TKN_LEN]; /* AT command or response argument */

   if(0 != tokenRead(token)){

      received = parse(token, command, parameter); /* parse the token */
      currCmd = updateFSM(received, command, parameter);     /* update FSM */

   }

   return currCmd;

}

/** The sendATcmd function sends an AT command to the GSM engine.
 */

FSMresult sendATcmd (const uint8_t * cmdstr)
{
   ATToken sending; /* classifies the command being sent */
   FSMresult result; /* result of the updateFSM invocation */

   uint8_t cmd[TKN_LEN];   /* AT command  */
   uint8_t par[TKN_LEN]; /* AT command arguments */

   sending = parse(cmdstr, cmd, par);

   switch(sending){ /* modify format depending on the AT command type, send
                    through serial port and call updateFSM function */

      case AUTOBAUD:

         rs232Print("AT\r");

         result = updateFSM(sending,"AT","");

         break;

      case BASIC_CMD:

         rs232Print("AT");
         rs232Print(cmd);
         if(par != 0) {rs232Print(par);}
         rs232Print("\r");

         result = updateFSM(sending,cmd,par);

         break;

      case BASIC_CMD_AMP:

         rs232Print("AT&");
         rs232Print(cmd);
         if(par != 0) {rs232Print(par);}
         rs232Print("\r");

         result = updateFSM(sending,cmd,par);

         break;

      case EXT_CMD_TEST:

         rs232Print("AT+");
         rs232Print(cmd);
         rs232Print("=?\r");

         result = updateFSM(sending,cmd,par);

         break;

      case EXT_CMD_WRITE:

         rs232Print("AT+");
         rs232Print(cmd);
         rs232Print("=");
         rs232Print(par);
         rs232Print("\r");

         result = updateFSM(sending,cmd,par);

         break;

      case EXT_CMD_READ:

         rs232Print("AT+");
         rs232Print(cmd);
         rs232Print("?\r");

         result = updateFSM(sending,cmd,par);

         break;

      case EXT_CMD_EXEC:

         rs232Print("AT+");
         rs232Print(cmd);
         rs232Print("\r");

         result = updateFSM(sending,cmd,par);

         break;

      case SMS_BODY:

         rs232Print(par);
         rs232Print("\x1A");

         result = updateFSM(sending,cmd,par);

         break;

      default:

         debug(">>>engine<<<   Se intento enviar un tipo de comando desconocido\r\n");

         result = ERR_CMD_UKN;

         break;

   }

   return result;
}

uint8_t * getCmdResp (uint8_t index)
{
   /* fetches the next command response; returns 0 if there are no more
      response left */

   if(index > lastResp | index < 0){return 0;}
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

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
