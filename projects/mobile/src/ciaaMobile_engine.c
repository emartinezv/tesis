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

//#define DEBUGGSM // debug mode

/*==================[global data]============================================*/

/*==================[internal data declaration]==============================*/

/** @brief Current state of the GSM engine */

static GSMstate GSMstatus = WAITING;

/** @brief Number of response tokens in the last command executed */

static int8_t lastResp = -1;

/*==================[internal functions declaration]=========================*/

/** @brief Updates the AT command FSM with the latest received token
*
*  @param received  Type of AT token as per the parse function
*  @param command   Main part of the AT token used as input
*  @param parameter Parameter part of the AT token used as input
*
*  @return Returns the state of the current command
*/

static cmdState updateFSM (ATToken received,
                    uint8_t const * const command,
                    uint8_t const * const parameter);

/*==================[internal data definition]===============================*/

/** @brief Stores the responses to the active command */

static uint8_t respVector[TKN_BUF_SIZE][TKN_LEN];

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/** The updateFSM function updates the internal FSM which preserves the state
 *  of the  current command execution. The latest token received from the GSM
 *  engine (as processed by the parse function) is the input, and the state
 *  of the current command after the function call is the output.
 */

static cmdState updateFSM (ATToken received,
               uint8_t const * const command,
               uint8_t const * const parameter)
{
   static uint8_t currCMD[TKN_LEN]; /* command being currently executed */
   static uint8_t currPAR[TKN_LEN]; /* parameter of the current command */
   static uint8_t currTKN;          /* number of response token being
                                       processed */

   static uint8_t i = 255;          /* command indexing variable */

   switch(GSMstatus){

      case WAITING: /* initial state */

         lastResp = -1;

         if (SENT == received){ /* command sent by serial port */

            i = commSearch(command); /* search for command */

            if(65535 != i){

               /* if the command is valid, copy command and parameter into
                  currCMD and currPAR; this is the command now being
                  executed by the FSM  */

               strncpy(currCMD,command,strlen(command));
               currCMD[strlen(command)] = '\0';
               strncpy(currPAR,parameter,strlen(parameter));
               currPAR[strlen(parameter)] = '\0';

               if (0 == strncmp(command, "SMS_BODY", strlen(currCMD))){
                  GSMstatus = CMD_ACK; /* The Ctrl-Z char that closes SMS body  */
                                       /* messages is not echoed, so ack is not */
                                       /* viable                                */
               }
               else{
                  GSMstatus = CMD_SENT; /* ack received, command has been sent */
               }

               #ifdef DEBUGGSM
               dbgPrint("COMMAND SENT: ");
               dbgPrint(command);
               dbgPrint("(");
               dbgPrint(parameter);
               dbgPrint(")\r\n");
               #endif

               return cmdProc;

            }

            else

               #ifdef DEBUGGSM
               dbgPrint("UNKNOWN COMMAND\r\n");
               #endif

               return cmdError;

         }
         else

            #ifdef DEBUGGSM
            dbgPrint("RESPONSE OUT OF ORDER\r\n");
            #endif

            return cmdError;

         break;

      case CMD_SENT:

         /* now that the command has been sent, we check the echo from the
            modem and verify that it agrees with the sent command */

         if ((received >= BASIC_CMD) && (received <= EXT_CMD_EXEC)){

            uint8_t eqCMD = 1;
            uint8_t eqPAR = 1;
            eqCMD = strncmp(command, currCMD, strlen(currCMD));
            eqPAR = strncmp(parameter, currPAR, strlen(currPAR));

            if ((0 == eqCMD) && (0 == eqPAR)){
               GSMstatus = CMD_ACK;
               currTKN = 0;

               #ifdef DEBUGGSM
               dbgPrint("COMMAND ACK\r\n");
               #endif

               return cmdProc;
            }
            else{
               GSMstatus = WAITING;

               #ifdef DEBUGGSM
               dbgPrint("COMMAND ECHO ERROR\r\n");
               #endif

               return cmdError;
            }
         }
         else{
            GSMstatus = WAITING;

            #ifdef DEBUGGSM
            dbgPrint("COMMAND ECHO MISSING\r\n");
            #endif

            return cmdError;
         }

         break;

      case CMD_ACK:

         /* process a number of tokens depending on the command, checking for
            end responses each time */

         ;

         if ((received >= BASIC_RSP) && (received <= EXT_RSP)){

            /* store the response in the active command response vector and
               check if it is an end response */

            strncpy(respVector[currTKN],command,strlen(command));
            respVector[currTKN][strlen(command)] = '\0';
            strncat(respVector[currTKN],"(",1);
            strncat(respVector[currTKN],parameter,strlen(parameter));
            strncat(respVector[currTKN],")",1);

            #ifdef DEBUGGSM
            dbgPrint("RESP: ");
            dbgPrint(respVector[currTKN]);
            dbgPrint("\r\n");
            #endif

            lastResp++;
            currTKN++;

            uint8_t * place;

            place = strstr(commands[i].endresp,command);

            if(NULL != place){

               #ifdef DEBUGGSM
               dbgPrint("COMMAND CLOSED\r\n");
               #endif

               GSMstatus = WAITING;
               return cmdClosed;
            }
            else{
               return cmdProc;
            }

         }

         else{

               #ifdef DEBUGGSM
               dbgPrint("INVALID TOKEN RECEIVED: ");
               dbgPrint(command);
               dbgPrint("(");
               dbgPrint(parameter);
               dbgPrint(")\r\n");
               #endif

               return cmdError;
         }

         break;

      default:

         #ifdef DEBUGGSM
         dbgPrint("ERROR: SWITCH OUT OF RANGE");
         #endif

         GSMstatus = WAITING;
         return cmdError;

         break;
   }

}

/*==================[external functions definition]==========================*/

/** The processToken function checks is there are unread tokens in the token
 *  ring buffer. If so, it reads the oldest token, parses the token through the
 *  parse function and calls upon the updateFSM function with the result. It
 *  returns the state of the current command as reported by the updateFSM
 *  function.
 */

cmdState processToken(void)
{
   ATToken received; /* classifies the received token*/
   cmdState currCmd = cmdWait; /* state of current cmd after token is processed*/

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

void sendATcmd (const uint8_t * cmd, const uint8_t * par, const ATcmdType type)
{
   switch(type){ /* modify format depending on the AT command type, send
                    through serial port and call updateFSM function */

      case AUTOBAUD:

         rs232Print("AT\r");

         #ifdef DEBUGGSM
         dbgPrint("AT enviado\r\n");
         #endif

         updateFSM(SENT,"AT","");

         break;

      case BASIC_STD:

         rs232Print("AT");
         rs232Print(cmd);
         if(par != 0) {rs232Print(par);}
         rs232Print("\r");

         #ifdef DEBUGGSM
         dbgPrint("AT");
         dbgPrint(cmd);
         if(par != 0) {dbgPrint(par);}
         dbgPrint(" enviado\r\n");
         #endif

         updateFSM(SENT,cmd,par);

         break;

      case BASIC_AMP:

         rs232Print("AT&");
         rs232Print(cmd);
         if(par != 0) {rs232Print(par);}
         rs232Print("\r");

         #ifdef DEBUGGSM
         dbgPrint("AT&");
         dbgPrint(cmd);
         if(par != 0) {dbgPrint(par);}
         dbgPrint(" enviado\r\n");
         #endif

         updateFSM(SENT,cmd,par);

         break;

      case EXT_TEST:

         rs232Print("AT+");
         rs232Print(cmd);
         rs232Print("=?\r");

         #ifdef DEBUGGSM
         dbgPrint("AT+");
         dbgPrint(cmd);
         dbgPrint("=? enviado\r\n");
         #endif

         updateFSM(SENT,cmd,par);

         break;

      case EXT_WRITE:

         rs232Print("AT+");
         rs232Print(cmd);
         rs232Print("=");
         rs232Print(par);
         rs232Print("\r");

         #ifdef DEBUGGSM
         dbgPrint("AT+");
         dbgPrint(cmd);
         dbgPrint("=");
         dbgPrint(par);
         dbgPrint(" enviado\r\n");
         #endif

         updateFSM(SENT,cmd,par);

         break;

      case EXT_READ:

         rs232Print("AT+");
         rs232Print(cmd);
         rs232Print("?\r");

         #ifdef DEBUGGSM
         dbgPrint("AT+");
         dbgPrint(cmd);
         dbgPrint("? enviado\r\n");
         #endif

         updateFSM(SENT,cmd,par);

         break;

      case EXT_EXEC:

         rs232Print("AT+");
         rs232Print(cmd);
         rs232Print("\r");

         #ifdef DEBUGGSM
         dbgPrint("AT+");
         dbgPrint(cmd);
         dbgPrint(" enviado\r\n");
         #endif

         updateFSM(SENT,cmd,par);

         break;

      case SMS:

         rs232Print(par);
         rs232Print("\x1A");

         updateFSM(SENT,cmd,par);

         break;

      default:

         dbgPrint("Tipo de comando no reconocido\r\n");
         break;

   }

   return;
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

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
