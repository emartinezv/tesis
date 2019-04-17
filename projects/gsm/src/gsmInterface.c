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

static frmStatus_e frmState = IDLE;

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

static frmCback_t frmCback;

/** @brief Mode variable for URC handling
 */

static urcMode_e urcMode = MANUAL_MODE;

/** @brief Function pointer to callback for URCs
 */

static urcCback_t urcCback;

/** @brief Function pointer to callback for DATA_MODE
 */

static dataCback_t dataCback;

/** @brief Period counter for GSM processing */

static int32_t procCnt = DELAY_PROC;

/*==================[internal functions declaration]=========================*/

/*---------------------------------------------------------------------------*/
/*                  General GSM library operation functions                  */
/*---------------------------------------------------------------------------*/

/** @brief Formula to start up the GSM library
 */

static void gsmStartUpF (void);

/** @brief Formula to get signal quality values (RSSI and BER)
 */

static void gsmGetSigQualF (void);

/** @brief Formula to check status of GSM and GPRS connection
 */

static void gsmCheckConnF (void);

/*---------------------------------------------------------------------------*/
/*                              SMS functions                                */
/*---------------------------------------------------------------------------*/

/** @brief Formula to send an SMS
 */

static void gsmSmsSendF (void);

/** @brief Formula to read a single SMS
 */

static void gsmSmsReadF (void);

/** @brief Formula to read several received SMSs into a list
 */

static void gsmSmsListF (void);

/** @brief Formula to delete a single SMS from memory
 */

static void gsmSmsDelF (void);

/*---------------------------------------------------------------------------*/
/*                             GPRS functions                                */
/*---------------------------------------------------------------------------*/

/** @brief Formula to start the GPRS connection
 */

static void gsmGprsStartF (void);

/** @brief Formula to open a TCP or UDP port
 */

static void gsmGprsOpenPortF (void);

/** @brief Formula to close a TCP or UDP port
 */

static void gsmGprsClosePortF (void);

/*---------------------------------------------------------------------------*/
/*                             GNSS functions                                */
/*---------------------------------------------------------------------------*/

/** @brief Formula to turn GNSS power on or off
 */

static void gsmGnssPwrF (void);

/** @brief Formula to get GNSS data
 */

static void gsmGnssGetDataF (void);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*---------------------------------------------------------------------------*/
/*                  General structure of formula functions                   */
/*---------------------------------------------------------------------------*/

/* All formula functions end with F (gsmStartUpF, smsSmsSendF, etc.). Their
 * main purporse is to send a number of AT commands. After sending each
 * command we check that the modem response is valid and continue to the next
 * command until we are finished. If an error occurs in any part of the process
 * we stop and record the specific error. After we've sent all the needed
 * commands and received the appropiate responses, we invoke a user-determined
 * callback function which usually does something with the responses of the
 * commands we sent. The behaviour is rather linear and is therefore
 * implemented in a multi-level FSM.
 *
 * Formula functions are invoked regularly, with the period determined by the
 * DELAY_PROC constant. Each invocation usually gobbles up a single AT token
 * and updates the engine. Thus, we need static variables to remember the
 * states of the different FSM levels. Generally speaking, we have two levels:
 * frmStatus at the highest level and procStatus when actually sending
 * the SMS commands and processing the responses.
 *
 * frmStatus is limited to INIT-PROC-WRAP.
 *
 * INIT is the part of the function in which we initialize the error report
 * structure and declare and initialize any auxiliary variables we need. We
 * also initialize procState (the sub-state variable used for the PROC state)
 * and change frmState to PROC at the end so that at the next invoke we start
 * sending the commands.
 *
 * PROC is the part of the function in which we send the AT commands, check
 * the responses and report errors if any. Each invocation processes a single
 * token; procState changes when a command is either in error or completed.
 * The possible states at ATCMD1, ATCMD1RESP, ATCMD2, ATCMD2RESP,...etc.
 *
 * The general format is simple. Starting with ATCMD1, the first AT command is
 * sent with the gsmSendCmd function; if the result indicates that the command
 * has been received correctly by the modem (OK_CMD_SENT) then procState
 * advances to ATCMD1RESP. There, there is a repeated invocation if the
 * gsmProcessTkn function so that all non-closing responses to the command are
 * received and stored until we get either a closing response (OK_CLOSE) or
 * an error (ERR_MSG_CLOSE). If we get a closing response procState advances
 * to ATCMD2 and repeats the process. If we get an error, we go directly to
 * the finishing section by updating frmState to WRAP. All deviations from this
 * general format are commented.
 *
 * WRAP is the part of the function when we wrap up, making any further error
 * processing needed, calling the user-defined callback function and finally
 * resetting frmState to IDLE.
 *
 * Regarding errors, there is a distinction between errors during the INIT or
 * WRAP phase and errors reported by the GSM modem itself as a response to a
 * specific command. This is why the errorUser structure has two members:
 * errorFrm and errorCmd. In the WRAP phase, if there is a GSM modem error,
 * it is copied to the errorCmd part separated into cmd and par as other
 * tokens.
 */

/*---------------------------------------------------------------------------*/
/*              General GSM library operation formula functions              */
/*---------------------------------------------------------------------------*/

static void gsmStartUpF (void)
{
   static procStatus_e procState = NOCMD;
   static errorUser_s errorOut;

   fsmEvent_e result;

   switch(frmState) {

      case INIT:

         errorOut.errorFrm = OK;
         errorOut.errorCmd.cmd[0] = '\0';
         errorOut.errorCmd.par[0] = '\0';

         gsmInitEngine(); /* Initializes the GSM engine */

         procState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(procState){

            case ATCMD1:

               result = gsmSendCmd("AT\r");
               if(OK_CMD_SENT == result){procState = ATCMD1RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){procState = ATCMD2;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD2:

               result = gsmSendCmd("AT+CMEE=2\r");
               if(OK_CMD_SENT == result){procState = ATCMD2RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD2RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){procState = ATCMD3;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD3:

               result = gsmSendCmd("AT+CSCS=\"GSM\"\r");
               if(OK_CMD_SENT == result){procState = ATCMD3RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD3RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){procState = ATCMD4;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD4:

               result = gsmSendCmd("AT+CMGF=1\r");
               if(OK_CMD_SENT == result){procState = ATCMD4RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD4RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){frmState = WRAP;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(OK != errorOut.errorFrm){

            if(ERR_GSM == errorOut.errorFrm){

               rsp_t rsp;

               rsp = gsmGetCmdRsp();
               strncpy(errorOut.errorCmd.cmd, rsp.cmd, TKN_CMD_SIZE-1);
               errorOut.errorCmd.cmd[TKN_CMD_SIZE] = '\0';
               strncpy(errorOut.errorCmd.par, rsp.par, (TKN_PAR_SIZE/2)-1);
               errorOut.errorCmd.par[TKN_PAR_SIZE/2] = '\0';

            }
         }

         frmCback(errorOut, 0);
         frmState = IDLE;

         break;
   }

   return;

}

/*****************************************************************************/

static void gsmGetSigQualF (void)
{
   static procStatus_e procState = NOCMD;
   static errorUser_s errorOut;

   fsmEvent_e result;

   switch(frmState) {

      case INIT:

         errorOut.errorFrm = OK;
         errorOut.errorCmd.cmd[0] = '\0';
         errorOut.errorCmd.par[0] = '\0';

         procState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(procState){

            case ATCMD1:

               result = gsmSendCmd("AT+CSQ\r");
               if(OK_CMD_SENT == result){procState = ATCMD1RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){frmState = WRAP;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(OK == errorOut.errorFrm){

            rsp_t rsp;
            uint8_t auxStr[2+1];  /* auxiliary string for response parsing of
                                   * the RSSI and BER values
                                   * XX\0 --> max string length 3 */
            auxStr[0] = '\0';
            uint8_t i;
            uint8_t commaPos = 0; /* position of the comma in the response */

            rsp = gsmGetCmdRsp();  /* get the signal quality response */

            /* Search for the comma in the response, which is in the form RSSI,
             * BER */

            for(i = 0; i < (strlen(rsp.par)-1); i++){
               if(',' == rsp.par[i]){commaPos = i; break;}
            }

            /* Copy the string of RSSI and convert it into a integer. Then, use
             * the table in the SIM808 manual to translate this number into an
             * actual RSSI reading in dBm. The table goes as follows:
             *
             * RSSI = 0          -->   -115 dBm
             * RSSI = 1          -->   -111 dBm
             * 2 <= RSSI <= 30   -->   -110 + (RSSI-2)*2 dBm
             * RSSI = 31         -->    -52 dBm
             * RSSI = 99         -->    Error
             *
             */

            strncpy(auxStr,&rsp.par[0],commaPos);
            auxStr[commaPos] = '\0';

            ((sigQual_s *)frmOutput)->rssi = atoi(auxStr);

            if(0 == ((sigQual_s *)frmOutput)->rssi){
               ((sigQual_s *)frmOutput)->rssi = -115;
            }

            else if(1 == ((sigQual_s *)frmOutput)->rssi){
               ((sigQual_s *)frmOutput)->rssi = -111;
            }

            else if((((sigQual_s *)frmOutput)->rssi) >= 2 &&
                    (((sigQual_s *)frmOutput)->rssi) <= 30){
               ((sigQual_s *)frmOutput)->rssi = -110 +
                                         (((sigQual_s *)frmOutput)->rssi -2)*2;
            }

            else if(31 == ((sigQual_s *)frmOutput)->rssi){
               ((sigQual_s *)frmOutput)->rssi = -52;
            }

            else if(99 == ((sigQual_s *)frmOutput)->rssi){
               ((sigQual_s *)frmOutput)->rssi = 99;
            }

            /* Copy the string of BER and convert it to a number. The module
             * reports RXQUAL values as per table in GSM 05.08 [20] subclause
             * 8.2.4
             *
             * RXQUAL = 0   -->
             * RXQUAL = 1   -->   0.2%    < BER <   0.4%
             * RXQUAL = 2   -->   0.4%    < BER <   0.8%
             * RXQUAL = 3   -->   0.8%    < BER <   1.6%
             * RXQUAL = 4   -->   1.6%    < BER <   3.2%
             * RXQUAL = 5   -->   3.2%    < BER <   6.4%
             * RXQUAL = 6   -->   6.4%    < BER <   12.8%
             * RXQUAL = 7   -->   12.8%   < BER
             *
             * */

            strncpy(auxStr,&rsp.par[commaPos+1],strlen(rsp.par)-commaPos-1);
            auxStr[strlen(rsp.par)-commaPos-1] = '\0';

            ((sigQual_s *)frmOutput)->ber = atoi(auxStr);

            debug(">>>interf<<<   RSSI: ");
            itoa(((sigQual_s *)frmOutput)->rssi, auxStr, 10);
            debug(auxStr);
            debug(", RXQUAL: ");
            itoa(((sigQual_s *)frmOutput)->ber, auxStr, 10);
            debug(auxStr);
            debug(" \r\n");

            frmCback(errorOut, frmOutput);
            frmState = IDLE;

         }

         else if(ERR_GSM == errorOut.errorFrm){

            rsp_t rsp;

            rsp = gsmGetCmdRsp();
            strncpy(errorOut.errorCmd.cmd, rsp.cmd, TKN_CMD_SIZE-1);
            errorOut.errorCmd.cmd[TKN_CMD_SIZE] = '\0';
            strncpy(errorOut.errorCmd.par, rsp.par, (TKN_PAR_SIZE/2)-1);
            errorOut.errorCmd.par[TKN_PAR_SIZE/2] = '\0';

         }

         frmCback(errorOut, frmOutput);
         frmState = IDLE;

         break;
   }

   return;

}

/*****************************************************************************/

void gsmCheckConnF (void)
{
   static procStatus_e procState = NOCMD;
   static errorUser_s errorOut;

   fsmEvent_e result;

   /* We use two different rsp_t structure variables in this case because we
    * need the result of two successive commands. As the engine flushes the
    * rspVlRb VLRB after each command is closed, we would loose the response
    * of the first command. Therefore, we need to save it in a separate
    * variable. All other formula functions use the result of the last sent
    * command and therefore the rspVlRb is not yet flushed.
    */

   static rsp_t rspGsm;
   static rsp_t rspGprs;

   switch(frmState) {

      case INIT:

         errorOut.errorFrm = OK;
         errorOut.errorCmd.cmd[0] = '\0';
         errorOut.errorCmd.par[0] = '\0';

         procState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(procState){

            case ATCMD1:

               result = gsmSendCmd("AT+CREG?\r");
               if(OK_CMD_SENT == result){procState = ATCMD1RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){
                     rspGsm = gsmGetCmdRsp(); /* store the GSM response */
                     procState = ATCMD2;
                  }
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD2:

               result = gsmSendCmd("AT+CGATT?\r");
               if(OK_CMD_SENT == result){procState = ATCMD2RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD2RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){
                     rspGprs = gsmGetCmdRsp(); /* store the GPRS response */
                     frmState = WRAP;
                  }
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(OK == errorOut.errorFrm){

            /* The AT+CREG? response is of the form "+CREG: <n>,<stat>" where
             * both <n> and <stat> are single-digit integers and <stat> is the
             * actual GSM network status we care about. Therefore, we take char
             * 3 of the parameter part of the response.
             * */

            if('1' == rspGsm.par[3]){
               ((connStatus_s *)frmOutput)->gsm = true;
            }
            else{
               ((connStatus_s *)frmOutput)->gsm = false;
            }

            /* Print out the GSM string */

            debug(">>>interf<<<   GSM String:");
            debug(&rspGsm.par[0]);
            debug(" \r\n");

            /* The AT+CGATT? response is of the form "+CGATT: <state>" where
             * both <state> is a single-digit integer.Therefore, we take char
             * 1 of the parameter part of the response.
             * */

            if('1' == rspGprs.par[1]){
               ((connStatus_s *)frmOutput)->gprs = true;
            }
            else{
               ((connStatus_s *)frmOutput)->gprs = false;
            }

            /* Print out the GPRS string */

            debug(">>>interf<<<   GPRS String:");
            debug(&rspGprs.par[0]);
            debug(" \r\n");

         }

         else if(ERR_GSM == errorOut.errorFrm){

            rsp_t rsp;

            rsp = gsmGetCmdRsp();
            strncpy(errorOut.errorCmd.cmd, rsp.cmd, TKN_CMD_SIZE-1);
            errorOut.errorCmd.cmd[TKN_CMD_SIZE] = '\0';
            strncpy(errorOut.errorCmd.par, rsp.par, (TKN_PAR_SIZE/2)-1);
            errorOut.errorCmd.par[TKN_PAR_SIZE/2] = '\0';

         }

         frmCback(errorOut, frmOutput);
         frmState = IDLE;

         break;
   }

   return;

}

/*---------------------------------------------------------------------------*/
/*                          SMS formula functions                            */
/*---------------------------------------------------------------------------*/

/*****************************************************************************/

static void gsmSmsSendF (void)
{
   static procStatus_e procState = NOCMD;
   static errorUser_s errorOut;

   /* Maximum phone number length under the ITU-T standard E.164 is 15 digits
    * plus the "+" symbol if present.
    *
    * AT+CMGS="+(phone number)"\r\0 --> max str length 28 */

   static uint8_t smsCmd[9+1+15+3];

   /* Maximum SMS size according to 3GPP standard TS 23.038 is 160 chars using
    * GSM 7-bit alphabet
    *
    * (smstext)\0 --> max str length 161 */

   static uint8_t smsText[160+1];

   fsmEvent_e result;

   switch(frmState) {

      case INIT:

         smsCmd[0] = '\0';
         smsText[0] = '\0';

         errorOut.errorFrm = OK;
         errorOut.errorCmd.cmd[0] = '\0';
         errorOut.errorCmd.par[0] = '\0';

         /* If the SMS text has '\r' or '\n' characters it could confuse the
          * tokenizer. Therefore, we check that neither of these chars are
          * present.
          */

         if ( (strchr(((smsOut_s *)frmInput)->text, '\n') != NULL) &&
              (strchr(((smsOut_s *)frmInput)->text, '\r') != NULL) ){

            debug(">>>interf<<<   ERROR: The SMS text contains the \\r and/or "
                                         "\\n characters\r\n");
            errorOut.errorFrm = ERR_INIT;
            frmState = WRAP;

         }

         /* We assemble the AT+CMGS command copying the phone number string */

         strncat(smsCmd, "AT+CMGS=\"", strlen("AT+CMGS=\""));
         strncat(smsCmd, ((smsOut_s *)frmInput)->dest,
                 strlen(((smsOut_s *)frmInput)->dest));
         strncat(smsCmd, "\"\r", strlen("\"\r"));

         /* We assemble the SMS text command */

         strncpy(smsText, ((smsOut_s *)frmInput)->text,
                 strlen(((smsOut_s *)frmInput)->text));
         smsText[strlen(((smsOut_s *)frmInput)->text)] = '\0';

         procState = ATCMD1;
         frmState = PROC;

         debug(">>>interf<<<   SEND SMS CMD: ");
         debug(smsCmd);
         debug("\r\n>>>interf<<<   SMS TEXT: ");
         debug(smsText);
         debug("\r\n");

         break;

      case PROC:

         switch(procState){

            case ATCMD1:

               result = gsmSendCmd(smsCmd);
               if(OK_CMD_SENT == result){procState = ATCMD1RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){procState = ATCMD2;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD2:

               result = gsmSendCmd(smsText);
               if(OK_CMD_SENT == result){procState = ATCMD2RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD2RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){frmState = WRAP;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(OK != errorOut.errorFrm){
            if(ERR_GSM == errorOut.errorFrm){

               rsp_t rsp;

               rsp = gsmGetCmdRsp();
               strncpy(errorOut.errorCmd.cmd, rsp.cmd, TKN_CMD_SIZE-1);
               errorOut.errorCmd.cmd[TKN_CMD_SIZE] = '\0';
               strncpy(errorOut.errorCmd.par, rsp.par, (TKN_PAR_SIZE/2)-1);
               errorOut.errorCmd.par[TKN_PAR_SIZE/2] = '\0';

            }
         }
         else{

               /* If the SMS is successfully sent we are given a <mr> number
                * which represents the number assigned in the TE to the sent
                * SMS. We store it in the smsConf_s structure.
                */

               rsp_t rsp;

               rsp = gsmGetCmdRsp(); /* discard the final OK response */
               rsp = gsmGetCmdRsp(); /* get the "+CMGS:<mr>\r\n" response */

               /* Convert the <mr> number string into an integer type  and
                * store it for reference */

               atoi(((smsConf_s *)frmOutput)->mr, rsp.par, 10);

         }

         frmCback(errorOut, frmOutput);
         frmState = IDLE;

         break;
   }

   return;

}

/*****************************************************************************/

static void gsmSmsReadF (void)
{
   static procStatus_e procState = NOCMD;
   static errorUser_s errorOut;

   fsmEvent_e result;

   static uint8_t strSmsRead[8+5+1]; /* string for the SMS read command*/
                                     /* AT+CMGR=XXX,Y\0 --> max str len 14 */

   uint8_t auxStr[3+1];              /* aux string for the idx SMS number */
                                     /* XXX\0 --> max str length 4 */

   switch(frmState) {

      case INIT:

         errorOut.errorFrm = OK;
         errorOut.errorCmd.cmd[0] = '\0';
         errorOut.errorCmd.par[0] = '\0';

         procState = ATCMD1;
         frmState = PROC;

         strSmsRead[0] = '\0';

         /* We assemble the AT+CMGR command copying the idx and mode variables
          * into the string after converting them from integers to text
          */

         strncat(strSmsRead, "AT+CMGR=", strlen("AT+CMGR="));
         itoa(((smsReadPars_s *)frmInput)->idx, auxStr, 10);
         strncat(strSmsRead, auxStr, strlen(auxStr));
         strncat(strSmsRead, ",", strlen(","));
         itoa(((smsReadPars_s *)frmInput)->mode, auxStr, 10);
         strncat(strSmsRead, auxStr, strlen(auxStr));
         strncat(strSmsRead, "\r", strlen("\r"));

         debug(">>>interf<<<   strSmsRead: ");
         debug(strSmsRead);
         debug("\r\n");

         break;

      case PROC:

         switch(procState){

            case ATCMD1:

               result = gsmSendCmd("AT+CSDH=1\r");
               if(OK_CMD_SENT == result){procState = ATCMD1RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){procState = ATCMD2;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD2:

               result = gsmSendCmd(strSmsRead);
               if(OK_CMD_SENT == result){procState = ATCMD2RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD2RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){frmState = WRAP;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(OK == errorOut.errorFrm){

            rsp_t rsp;

            /* We copy the meta part of the SMS and the actual text into the
             * corresponding parts of the smsRec structure
             */

            rsp = gsmGetCmdRsp();
            strncpy(((smsRec_s *)frmOutput)->meta, rsp.par, strlen(rsp.par));
            ((smsRec_s *)frmOutput)->meta[strlen(rsp.par)] = '\0';

            rsp = gsmGetCmdRsp();
            strncpy(((smsRec_s *)frmOutput)->text, rsp.par, strlen(rsp.par));
            ((smsRec_s *)frmOutput)->text[strlen(rsp.par)] = '\0';

         }

         else if(ERR_GSM == errorOut.errorFrm){

            rsp_t rsp;

            rsp = gsmGetCmdRsp();
            strncpy(errorOut.errorCmd.cmd, rsp.cmd, TKN_CMD_SIZE-1);
            errorOut.errorCmd.cmd[TKN_CMD_SIZE] = '\0';
            strncpy(errorOut.errorCmd.par, rsp.par, (TKN_PAR_SIZE/2)-1);
            errorOut.errorCmd.par[TKN_PAR_SIZE/2] = '\0';

         }

         smsListRet_s msgVector; /* Message vector includes the actual array
                                    and the number of messages; this is
                                    needed for further processing of the SMSs
                                    and compatibility with gsmSmsList */

         msgVector.msgs = frmOutput;
         msgVector.noMsgs = 1;

         frmCback(errorOut, &msgVector);
         frmState = IDLE;

         break;
   }

   return;

}

/*****************************************************************************/

static void gsmSmsListF (void)
{
   static procStatus_e procState = NOCMD;
   static errorUser_s errorOut;

   fsmEvent_e result;

   /* String to assemble the SMS list command
    *
    * AT+CMGL="(status)",Y\r\0 --> max string length */

   static uint8_t strSmsList[9+10+3+2];

   /* Auxiliary string. Longest status string is "REC UNREAD" which is 10
    * chars long, so the string needs to be 11 chars long including the \0  */

   static uint8_t auxStr[10+1];

   switch(frmState) {

      case INIT:

         errorOut.errorFrm = OK;
         errorOut.errorCmd.cmd[0] = '\0';
         errorOut.errorCmd.par[0] = '\0';

         procState = ATCMD1;
         frmState = PROC;

         strSmsList[0] = '\0';

         /* We assemble the AT+CMGL command copying the stat and mode variables
          * into the string
          */

         strncat(strSmsList, "AT+CMGL=", strlen("AT+CMGL="));

         switch(((smsListPars_s *)frmInput)->stat){

            case REC_UNREAD:

               strncpy(auxStr,"\"REC UNREAD\"",strlen("\"REC UNREAD\""));

               break;

            case REC_READ:

               strncpy(auxStr,"\"REC READ\"",strlen("\"REC READ\""));

               break;

            case STO_UNSENT:

               strncpy(auxStr,"\"STO UNSENT\"",strlen("\"STO UNSENT\""));

               break;

            case STO_SENT:

               strncpy(auxStr,"\"STO SENT\"",strlen("\"STO SENT\""));

               break;

            case ALL_MSG:

               strncpy(auxStr,"\"ALL\"",strlen("\"ALL\""));

               break;

         }

         strncat(strSmsList, auxStr, strlen(auxStr));
         strncat(strSmsList, ",", strlen(","));
         itoa(((smsListPars_s *)frmInput)->mode, auxStr, 10);
         strncat(strSmsList, auxStr, strlen(auxStr));
         strncat(strSmsList, "\r", strlen("\r"));

         debug(">>>interf<<<   strSmsList: ");
         debug(strSmsList);
         debug("\r\n");

         break;

      case PROC:

         switch(procState){

            case ATCMD1:

               result = gsmSendCmd("AT+CSDH=1\r");
               if(OK_CMD_SENT == result){procState = ATCMD1RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){procState = ATCMD2;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD2:

               result = gsmSendCmd(strSmsList);
               if(OK_CMD_SENT == result){procState = ATCMD2RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD2RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){frmState = WRAP;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         ;
         uint8_t rspNo;
         rspNo = gsmGetNoCmdRsp();

         if(OK == errorOut.errorFrm){

            uint8_t i = 0;

            rsp_t rsp;
            smsRec_s * target = (smsRec_s *)frmOutput;

            /* Check that the list vector is big enough to hold all the SMSs.
             * We need to discard the final OK response and each SMS is split
             * into two responses: the meta data and the actual text. The
             * number of SMSs is therefore the number of responses - 1 divided
             * by two. */

            if(((smsListPars_s *)frmInput)->listSize < ((rspNo-1)/2)){
               errorOut.errorFrm = ERR_WRAP;
               debug(">>>interf<<<   ERROR: Not enough space for available"
                     " SMSs\r\n");
            }

            else {

               /* If rspNo = 1 then we just have the OK response, which means
                * there are no SMSs to read
                */

               if(1 == rspNo){debug(">>>interf<<<   NO SMSs TO READ\r\n");}

               else{

                  /* We cycle through the responses and copy them to the list
                   * vector.
                   */

                  for(i = 0; i < (rspNo-1)/2; i++){

                     rsp = gsmGetCmdRsp();
                     strncpy((target+i)->meta, rsp.par, strlen(rsp.par));
                     (target+i)->meta[strlen(rsp.par)] = '\0';

                     rsp = gsmGetCmdRsp();
                     strncpy((target+i)->text, rsp.par, strlen(rsp.par));
                     (target+i)->text[strlen(rsp.par)] = '\0';

                  }

               }

            }

         }

         else if(ERR_GSM == errorOut.errorFrm){

            rsp_t rsp;

            rsp = gsmGetCmdRsp();
            strncpy(errorOut.errorCmd.cmd, rsp.cmd, TKN_CMD_SIZE-1);
            errorOut.errorCmd.cmd[TKN_CMD_SIZE] = '\0';
            strncpy(errorOut.errorCmd.par, rsp.par, (TKN_PAR_SIZE/2)-1);
            errorOut.errorCmd.par[TKN_PAR_SIZE/2] = '\0';

         }

         smsListRet_s msgVector; /* Message vector includes the actual array
                                    andnthe number of messages */

         msgVector.msgs = frmOutput;
         msgVector.noMsgs = (rspNo-1)/2;

         frmCback(errorOut, &msgVector);
         frmState = IDLE;

         break;
   }

   return;

}

/*****************************************************************************/

static void gsmSmsDelF (void)
{
   static procStatus_e procState = NOCMD;
   static errorUser_s errorOut;

   uint8_t auxStr[4]; /* auxiliary string */
                      /* XXX\0 --> max str length 4 */

   /* String to assemble the SMS delete command
    *
    * AT+CMGD=XXX,Y\r\0 --> max string length 15 */

   static uint8_t smsDel[8+3+1+1+2];

   fsmEvent_e result;

   switch(frmState) {

      case INIT:

         auxStr[0] = '\0';
         smsDel[0] = '\0';

         errorOut.errorFrm = OK;
         errorOut.errorCmd.cmd[0] = '\0';
         errorOut.errorCmd.par[0] = '\0';

         /* We assemble the AT+CMGD command copying the idx and mode variables
          * into the string
          */

         strncat(smsDel, "AT+CMGD=", strlen("AT+CMGD="));
         itoa(((smsDelPars_s *)frmInput)->idx, auxStr, 10);
         strncat(smsDel, auxStr, strlen(auxStr));
         strncat(smsDel, ",", strlen(","));
         itoa(((smsDelPars_s *)frmInput)->mode, auxStr, 10);
         strncat(smsDel, auxStr, strlen(auxStr));
         strncat(smsDel, "\r", strlen("\r"));

         procState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(procState){

            case ATCMD1:

               result = gsmSendCmd(smsDel);
               if(OK_CMD_SENT == result){procState = ATCMD1RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){frmState = WRAP;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(OK != errorOut.errorFrm){
            if(ERR_GSM == errorOut.errorFrm){

               rsp_t rsp;

               rsp = gsmGetCmdRsp();
               strncpy(errorOut.errorCmd.cmd, rsp.cmd, TKN_CMD_SIZE-1);
               errorOut.errorCmd.cmd[TKN_CMD_SIZE] = '\0';
               strncpy(errorOut.errorCmd.par, rsp.par, (TKN_PAR_SIZE/2)-1);
               errorOut.errorCmd.par[TKN_PAR_SIZE/2] = '\0';

            }
         }

         frmCback(errorOut, 0);
         frmState = IDLE;
         break;
   }

   return;
}

/*---------------------------------------------------------------------------*/
/*                         GPRS formula functions                            */
/*---------------------------------------------------------------------------*/

static void gsmGprsStartF (void)
{
   static procStatus_e procState = NOCMD;
   static errorUser_s errorOut;

   /* String to assemble the GPRS start command
    *
    * apn, user and pwd are arbitrarely assumed to be no longer than 30 chars
    * each (a reasonable assumption by all practical accounts)
    *
    * AT+CSTT="(apn)","(user)","(pwd)"\r\0 --> max string length */

   static uint8_t APNstring[9+30+3+30+3+30+3];

   fsmEvent_e result;

   switch(frmState) {

      case INIT:

         APNstring[0] = '\0';

         errorOut.errorFrm = OK;
         errorOut.errorCmd.cmd[0] = '\0';
         errorOut.errorCmd.par[0] = '\0';

         /* We assemble the AT+CSTT command copying the apn, user and pwd
          * variables into the string
          */

         strncat(APNstring, "AT+CSTT=\"", strlen("AT+CSTT=\""));
         strncat(APNstring, ((apnUserPwd_s *)frmInput)->apn,
                             strlen(((apnUserPwd_s *)frmInput)->apn));
         strncat(APNstring, "\",\"", strlen("\",\""));
         strncat(APNstring, ((apnUserPwd_s *)frmInput)->user,
                             strlen(((apnUserPwd_s *)frmInput)->user));
         strncat(APNstring, "\",\"", strlen("\",\""));
         strncat(APNstring, ((apnUserPwd_s *)frmInput)->pwd,
                             strlen(((apnUserPwd_s *)frmInput)->pwd));
         strncat(APNstring, "\"\r", strlen("\"\r"));

         procState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(procState){

            case ATCMD1:

               result = gsmSendCmd("AT+CIPSHUT\r");
               if(OK_CMD_SENT == result){procState = ATCMD1RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){procState = ATCMD2;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD2:

               result = gsmSendCmd("AT+CIPMODE=1\r");
               if(OK_CMD_SENT == result){procState = ATCMD2RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD2RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){procState = ATCMD3;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD3:

               result = gsmSendCmd(APNstring);
               if(OK_CMD_SENT == result){procState = ATCMD3RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD3RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){procState = ATCMD4;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD4:

               result = gsmSendCmd("AT+CIICR\r");
               if(OK_CMD_SENT == result){procState = ATCMD4RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD4RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){procState = ATCMD5;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD5:

               result = gsmSendCmd("AT+CIFSR\r");
               if(OK_CMD_SENT == result){procState = ATCMD5RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD5RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){frmState = WRAP;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(OK != errorOut.errorFrm){
            if(ERR_GSM == errorOut.errorFrm){

               rsp_t rsp;

               rsp = gsmGetCmdRsp();
               strncpy(errorOut.errorCmd.cmd, rsp.cmd, TKN_CMD_SIZE-1);
               errorOut.errorCmd.cmd[TKN_CMD_SIZE] = '\0';
               strncpy(errorOut.errorCmd.par, rsp.par, (TKN_PAR_SIZE/2)-1);
               errorOut.errorCmd.par[TKN_PAR_SIZE/2] = '\0';

            }
         }

         frmCback(errorOut, 0);
         frmState = IDLE;

         break;
   }

   return;
}

/*****************************************************************************/

static void gsmGprsOpenPortF (void)
{
   static procStatus_e procState = NOCMD;
   static errorUser_s errorOut;

   /* String to assemble the TCP/UDP open port command
    *
    * IP/domain is arbitrarely assumed to be no longer than 50 chars
    *
    * AT+CIPSTART="XXX","(IP/domain)",(port)\r\0 --> max string length 78 */

   static uint8_t portStr[13+3+3+50+2+5+2];

   uint8_t auxStr[6]; /* auxiliary string */
                      /* XXXXX\0 --> max str length 6 */

   rsp_t rsp; /* auxiliary variable used to analize the response to CIPSTART
              /* and distinguish the open port attempt results */

   fsmEvent_e result;

   switch(frmState) {

      case INIT:

         portStr[0] = '\0';

         errorOut.errorFrm = OK;
         errorOut.errorCmd.cmd[0] = '\0';
         errorOut.errorCmd.par[0] = '\0';

         /* We assemble the AT+CIPSTART command copying the type, address and
          * port variables into the string
          */

         strncat(portStr, "AT+CIPSTART=\"", strlen("AT+CIPSTART=\""));

         if(TCP == ((port_s *)frmInput)->type){
            strncat(portStr, "TCP\",\"", strlen("TCP\",\""));
         }
         else if(UDP == ((port_s *)frmInput)->type){
            strncat(portStr, "UDP\",\"", strlen("UDP\",\""));
         }

         strncat(portStr, ((port_s *)frmInput)->address,
                 strlen(((port_s *)frmInput)->address));

         itoa(((port_s *)frmInput)->port, auxStr, 10);
         strncat(portStr, "\",\"", strlen("\",\""));
         strncat(portStr, auxStr, strlen(auxStr));
         strncat(portStr, "\"\r", strlen("\"\r"));

         procState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(procState){

            case ATCMD1:

               result = gsmSendCmd("AT+CIPCLOSE=0\r");
               if(OK_CMD_SENT == result){procState = ATCMD1RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){procState = ATCMD2;}
                  else if(ERR_MSG_CLOSE == result){procState = ATCMD2;}
                  /* Operation is not stopped if AT+CIPCLOSE returns an error
                   * code, as there may simply be no open port to close.
                   * The command is sent merely as a precaution. */
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

            case ATCMD2:

               result = gsmSendCmd(portStr);
               if(OK_CMD_SENT == result){procState = ATCMD2RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD2RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){

                  if(OK_CMD_ACK <= result && OK_URC >= result){;}

                  /* CIPSTART has several closing responses, some of which
                   * indicate an error. Therefore, we need to further analyze
                   * the specific response in order to determine any possible
                   * error messages. */

                  else if(OK_CLOSE == result){

                     uint8_t noRsp, i;
                     noRsp = gsmGetNoCmdRsp(); /* store number of responses */

                     /* get the last response */
                     for (i = 0; i < noRsp; i++){rsp = gsmGetCmdRsp();}

                     if (0 == strcmp(rsp.cmd, "CONNECT FAIL")) {
                        errorOut.errorFrm = ERR_PROC; frmState = WRAP;
                     }

                     else {
                        frmState = WRAP;
                     }

                  }
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(OK != errorOut.errorFrm){
            if(ERR_GSM == errorOut.errorFrm){

               rsp_t rsp;

               rsp = gsmGetCmdRsp();
               strncpy(errorOut.errorCmd.cmd, rsp.cmd, TKN_CMD_SIZE-1);
               errorOut.errorCmd.cmd[TKN_CMD_SIZE] = '\0';
               strncpy(errorOut.errorCmd.par, rsp.par, (TKN_PAR_SIZE/2)-1);
               errorOut.errorCmd.par[TKN_PAR_SIZE/2] = '\0';

            }
         }

         else{

            /* If the port is opened correctly we enter DATA_MODE */

            gsmSetSerialMode(DATA_MODE);

         }

         frmCback(errorOut, 0);
         frmState = IDLE;

         break;
   }

   return;
}

/*****************************************************************************/

static void gsmGprsClosePortF (void)
{
   static procStatus_e procState = NOCMD;
   static errorUser_s errorOut;

   fsmEvent_e result;

   switch(frmState) {

      case INIT:

         errorOut.errorFrm = OK;
         errorOut.errorCmd.cmd[0] = '\0';
         errorOut.errorCmd.par[0] = '\0';

         procState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(procState){

            case ATCMD1:

               result = gsmSendCmd("AT+CIPCLOSE=0\r");
               if(OK_CMD_SENT == result){procState = ATCMD1RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){frmState = WRAP;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(OK != errorOut.errorFrm){
            if(ERR_GSM == errorOut.errorFrm){

               rsp_t rsp;

               rsp = gsmGetCmdRsp();
               strncpy(errorOut.errorCmd.cmd, rsp.cmd, TKN_CMD_SIZE-1);
               errorOut.errorCmd.cmd[TKN_CMD_SIZE] = '\0';
               strncpy(errorOut.errorCmd.par, rsp.par, (TKN_PAR_SIZE/2)-1);
               errorOut.errorCmd.par[TKN_PAR_SIZE/2] = '\0';

            }
         }

         /* If the port is closed correctly we return to COMMAND_MODE */

         gsmSetSerialMode(COMMAND_MODE);

         frmCback(errorOut, 0);
         frmState = IDLE;

         break;
   }

   return;
}

/*---------------------------------------------------------------------------*/
/*                         GNSS formula functions                            */
/*---------------------------------------------------------------------------*/

static void gsmGnssPwrF (void)
{
   static procStatus_e procState = NOCMD;
   static errorUser_s errorOut;

   /* String to assemble the GNSS on/off command
    *
    *
    * AT+CGNSPWR=X\r\0 --> max string length 14 */

   static uint8_t cmdStr[14];

   fsmEvent_e result;

   switch(frmState) {

      case INIT:

         errorOut.errorFrm = OK;
         errorOut.errorCmd.cmd[0] = '\0';
         errorOut.errorCmd.par[0] = '\0';

         procState = ATCMD1;
         frmState = PROC;

         /* We assemble the AT+CGNSPWR string depending on the value of the
          * input pwrGnss variable
          */

         if(ON == *((pwrGnss_e *)frmInput)){
            strncpy(cmdStr,"AT+CGNSPWR=1\r", strlen("AT+CGNSPWR=1\r"));
         }
         else if(OFF == *((pwrGnss_e *)frmInput)){
            strncpy(cmdStr,"AT+CGNSPWR=0\r", strlen("AT+CGNSPWR=0\r"));
         }

         break;

      case PROC:

         switch(procState){

            case ATCMD1:

               result = gsmSendCmd(cmdStr);
               if(OK_CMD_SENT == result){procState = ATCMD1RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){frmState = WRAP;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(OK != errorOut.errorFrm){
            if(ERR_GSM == errorOut.errorFrm){

               rsp_t rsp;

               rsp = gsmGetCmdRsp();
               strncpy(errorOut.errorCmd.cmd, rsp.cmd, TKN_CMD_SIZE-1);
               errorOut.errorCmd.cmd[TKN_CMD_SIZE] = '\0';
               strncpy(errorOut.errorCmd.par, rsp.par, (TKN_PAR_SIZE/2)-1);
               errorOut.errorCmd.par[TKN_PAR_SIZE/2] = '\0';

            }
         }

         frmCback(errorOut, 0);
         frmState = IDLE;

         break;
   }

   return;

}

/*****************************************************************************/

static void gsmGnssGetDataF (void)
{
   static procStatus_e procState = NOCMD;
   static errorUser_s errorOut;

   fsmEvent_e result;

   switch(frmState) {

      case INIT:

         errorOut.errorFrm = OK;
         errorOut.errorCmd.cmd[0] = '\0';
         errorOut.errorCmd.par[0] = '\0';

         procState = ATCMD1;
         frmState = PROC;

         break;

      case PROC:

         switch(procState){

            case ATCMD1:

               result = gsmSendCmd("AT+CGNSINF\r");
               if(OK_CMD_SENT == result){procState = ATCMD1RESP;}
               else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               break;

            case ATCMD1RESP:

               result = gsmProcessTkn();
               if(NO_UPDATE != result){
                  if(OK_CMD_ACK <= result && OK_URC >= result){;}
                  else if(OK_CLOSE == result){frmState = WRAP;}
                  else if(ERR_MSG_CLOSE == result){
                     errorOut.errorFrm = ERR_GSM;
                     frmState = WRAP;
                  }
                  else{errorOut.errorFrm = ERR_PROC; frmState = WRAP;}
               }
               break;

         }

         break;

      case WRAP:

         if(OK == errorOut.errorFrm){

            rsp_t rsp;

            rsp = gsmGetCmdRsp(); /* Get the navigation info string */

            /* Copy the navigation info string to the provided output. The
             * constant MAX_CGNSINF_SIZE is defined in gsmEngine.h and is
             * taken from the SIM808 manual as being 94 characters max */

            strncpy(((dataGnss_s *)frmOutput)->data,&rsp.par[0],
                    MAX_CGNSINF_SIZE);
            ((uint8_t *)frmOutput)[MAX_CGNSINF_SIZE] = '\0';

            debug(">>>interf<<<   NavInfo String:");
            debug(((dataGnss_s *)frmOutput)->data);
            debug("\r\n");

         }

         else if(ERR_GSM == errorOut.errorFrm){

            rsp_t rsp;

            rsp = gsmGetCmdRsp();
            strncpy(errorOut.errorCmd.cmd, rsp.cmd, TKN_CMD_SIZE-1);
            errorOut.errorCmd.cmd[TKN_CMD_SIZE] = '\0';
            strncpy(errorOut.errorCmd.par, rsp.par, (TKN_PAR_SIZE/2)-1);
            errorOut.errorCmd.par[TKN_PAR_SIZE/2] = '\0';

         }

         frmCback(errorOut, frmOutput);
         frmState = IDLE;

         break;
   }

   return;

}

/*==================[external functions definition]==========================*/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*                  General GSM library operation functions                  */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* This function needs to be called from the basic ARM SysTick_Handler function
 * to handle gsmProcess calls and AT command timeouts. When called, it
 * decrements both procCnt and toutCnt. It must be mentioned that procCnt is
 * an interface-level variable, while toutCnt is an engine-level variable.
 * This is why a function is used to decrement toutCnt, as it is not in scope
 * in interface.
 */

void gsmSysTickHandler (void)
{
   if(!gsmToutCntZero) gsmDecToutCnt();
   if(procCnt > 0) procCnt--;

   return;
}

/*****************************************************************************/

/** This function needs to be called periodically to process commands and
 *  their responses. It is the heart of the library from the user's
 *  perspective.
 *
 *  In the simplest approach, the user calls the function continuously in a
 *  "round-robin" scheme, but the function actually executes only if the
 *  procCnt timer is down to zero (this is handled by the prior
 *  gsmSysTickHandler function). The specific tasks executed depend on whether
 *  there is a formula being executed, the current URC handling mode and the
 *  serial mode. The comments below detail the behaviour.
 */

void gsmProcess (void)
{
   if(0 == procCnt){

      if (IDLE == frmState){ /* no formula currently being run */

         /* If we are in COMMAND_MODE, we first process a single token. */

         if(COMMAND_MODE == gsmGetSerialMode()){
            gsmProcessTkn();

            /* If the URC handling mode is CBACK_MODE, we check if there any
             * unprocessed URCs; if so, we get them and fire the user-defined
             * URC callback function.
             */

            if(CBACK_MODE == urcMode){

               rsp_t dummy;
               dummy = gsmGetUrc();

               if('\0' != dummy.cmd[0]){
                  urcCback(dummy.cmd, dummy.par);
               }
            }

         }

         /* If we are in DATA_MODE, we call the user-defined data handling
          * callback function.
          */

         else if (DATA_MODE == gsmGetSerialMode()){
            dataCback();
         }

      }

      else {frm();} /* there is a formula being run; execute the formula */

      procCnt = DELAY_PROC; /* reset gsmProcess counter */

      return;

   }

   else{

      return;

   }

}

/*****************************************************************************/

uint8_t gsmIsIdle (void)
{
   return(frmState == IDLE); /* determines if a formula is being run */
}

/*****************************************************************************/

void gsmSetUrcMode (urcMode_e mode)
{
   urcMode = mode;
   return;
}

/*****************************************************************************/

void gsmSetUrcCback (urcCback_t cback)
{
   urcCback = cback;
   return;
}

/*****************************************************************************/

void gsmSetDataCback (dataCback_t cback)
{
   dataCback = cback;
   return;
}

/*****************************************************************************/

/* This interface-level function executes a serial port write of n characters
 * followed by a serial port read of n characters. If used when in DATA_MODE
 * and calls upon two engine-level functions; this is done in order to respect
 * the abstraction layers and keep all serial commmunications with the modem
 * at the level of engine or lower.
 */

void gsmWriteReadDataMode (uint8_t * write, uint8_t * nWrite, uint8_t * read,
                           uint8_t * nRead)
{
   *nWrite = gsmWriteData(write, *nWrite);
   *nRead = gsmReadData(read, *nRead);

   return;
}

/*****************************************************************************/

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*                          Formula-loading functions                        */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* All the following functions follow the same scheme: load a formula function
 * pointer, the appropiate inputs and/or outputs in the frmInput and frmOutput
 * variables, a callback function in frmCback and set the frmState variable to
 * INIT so that the following gsmProcess invocation starts executing the
 * formula.
 *
 * They are the user-facing functions of the library and therefore aim at
 * solving a small and specific request.
 */

/*---------------------------------------------------------------------------*/
/*                        Startup and system functions                       */
/*---------------------------------------------------------------------------*/

/*****************************************************************************/

void gsmStartUp (frmCback_t cback)
{
   frm = gsmStartUpF;
   frmCback = cback;
   frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmGetSigQual (sigQual_s * sigQual, frmCback_t cback)
{
   frm = gsmGetSigQualF;
   frmOutput = sigQual;
   frmCback = cback;
   frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmCheckConn (connStatus_s * status, frmCback_t cback)
{
   frm = gsmCheckConnF;
   frmOutput = status;
   frmCback = cback;
   frmState = INIT;

   return;
}

/*---------------------------------------------------------------------------*/
/*                              SMS functions                                */
/*---------------------------------------------------------------------------*/

void gsmSmsSend (smsOut_s * msg, smsConf_s * conf, frmCback_t cback)
{
   frm = gsmSmsSendF;
   frmInput = msg;
   frmOutput = conf;
   frmCback = cback;
   frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmSmsRead (smsRec_s * msg, smsReadPars_s * pars, frmCback_t cback)
{
   frm = gsmSmsReadF;
   frmInput = pars;
   frmOutput = msg;
   frmCback = cback;
   frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmSmsList (smsRec_s * list, smsListPars_s * pars, frmCback_t cback)
{
   frm = gsmSmsListF;
   frmInput = pars;
   frmOutput = list;
   frmCback = cback;
   frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmSmsDel (smsDelPars_s * msgdel, frmCback_t cback)
{
   frm = gsmSmsDelF;
   frmInput = msgdel;
   frmCback = cback;
   frmState = INIT;

   return;
}

/*---------------------------------------------------------------------------*/
/*                             GPRS functions                                */
/*---------------------------------------------------------------------------*/

void gsmGprsStart (apnUserPwd_s * apn, frmCback_t cback)
{
   frm = gsmGprsStartF;
   frmInput = apn;
   frmCback = cback;
   frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmGprsOpenPort (port_s * port, frmCback_t cback)
{
   frm = gsmGprsOpenPortF;
   frmInput = port;
   frmCback = cback;
   frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmGprsClosePort (frmCback_t cback)
{
   frm = gsmGprsClosePortF;
   frmCback = cback;
   frmState = INIT;

   return;
}

/*---------------------------------------------------------------------------*/
/*                             GNSS functions                                */
/*---------------------------------------------------------------------------*/

void gsmGnssPwr (pwrGnss_e * cmd, frmCback_t cback)
{
   frm = gsmGnssPwrF;
   frmInput = cmd;
   frmCback = cback;
   frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmGnssGetData (dataGnss_s * dataGnss, frmCback_t cback)
{
   frm = gsmGnssGetDataF;
   frmOutput = dataGnss;
   frmCback = cback;
   frmState = INIT;

   return;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
