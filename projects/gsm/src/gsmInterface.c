/* Copyright 2019, Ezequiel Martinez Vazquez
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

//#define DEBUG_INTERFACE
#ifdef DEBUG_INTERFACE
   #define debug(msg) gsmTermUartSend(msg, strlen(msg))
#else
   #define debug(msg)
#endif

/*==================[global data]============================================*/

static uint8_t const * const exitCmds [] = {"OK", "CLOSED"};

static gsmInterface_t * SysTickInterface;

/*==================[internal functions declaration]=========================*/

/*---------------------------------------------------------------------------*/
/*              General GSM library operation formula functions              */
/*---------------------------------------------------------------------------*/

/** @brief Formula to start up the GSM interface
 *
 * @param interface : Pointer to interface
 *
 * @return Returns true if initialization successful
 */

static bool gsmInitInterface (gsmInterface_t * interface);

/** @brief Formula to start up the GSM library
 */

static void gsmStartUpF (gsmInterface_t * interface);

/** @brief Formula to get signal quality values (RSSI and BER)
 */

static void gsmGetSigQualF (gsmInterface_t * interface);

/** @brief Formula to check status of GSM and GPRS connection
 */

static void gsmCheckConnF (gsmInterface_t * interface);

/*---------------------------------------------------------------------------*/
/*                          SMS formula functions                            */
/*---------------------------------------------------------------------------*/

/** @brief Formula to send an SMS
 */

static void gsmSmsSendF (gsmInterface_t * interface);

/** @brief Formula to read a single SMS
 */

static void gsmSmsReadF (gsmInterface_t * interface);

/** @brief Formula to read several received SMSs into a list
 */

static void gsmSmsListF (gsmInterface_t * interface);

/** @brief Formula to delete a single SMS from memory
 */

static void gsmSmsDelF (gsmInterface_t * interface);

/*---------------------------------------------------------------------------*/
/*                         GPRS formula functions                            */
/*---------------------------------------------------------------------------*/

/** @brief Formula to start the GPRS connection
 */

static void gsmGprsStartF (gsmInterface_t * interface);

/** @brief Formula to open a TCP or UDP port
 */

static void gsmGprsOpenPortF (gsmInterface_t * interface);

/** @brief Formula to close a TCP or UDP port
 */

static void gsmGprsClosePortF (gsmInterface_t * interface);

/*---------------------------------------------------------------------------*/
/*                         GNSS formula functions                            */
/*---------------------------------------------------------------------------*/

/** @brief Formula to turn GNSS power on or off
 */

static void gsmGnssPwrF (gsmInterface_t * interface);

/** @brief Formula to get GNSS data
 */

static void gsmGnssGetDataF (gsmInterface_t * interface);

/*---------------------------------------------------------------------------*/
/*                     Formula-processing subroutines                        */
/*---------------------------------------------------------------------------*/

/** @brief Initialize formula state variables and error structure
*
* @param interface : Pointer to interface
*
*/

void gsmFrmInit (gsmInterface_t * interface);

/** @brief Subroutine to send a cmd and check for the correct echo
*
* @param interface : Pointer to interface
* @param cmd       : Pointer to cmd string
* @param nextState : Next procState to go to if echo is correct
*
* @return
*/

void gsmFrmSendCmdCheckEcho (gsmInterface_t * interface,
                             uint8_t const * const cmd,
                             procStatus_e nextState);

/** @brief Subroutine to process all responses and check for a final result:
 *         either a closing response, error response or timeout. The frm states
 *         are adjusted accordingly
*
* @param interface    : Pointer to interface
* @param closingState : Next procstate to go to if final response was closing
* @param closingWrap  : Go to WRAP formula state after closing response?
* @param errorState   : Next procstate to go to if final response was error
* @param errorWrap    : Go to WRAP formula state after closing response?
*
* @return
*/

void gsmFrmProcRspsGetFinal (gsmInterface_t * interface,
                             procStatus_e closingState, Bool closingWrap,
                             procStatus_e errorState, Bool errorWrap);

/** @brief If a GSM error has been detected, it recalls the last response and
 *         extracts the cmd and par portions of the error message into the
 *         errorOut variable
*
* @param interface : Pointer to interface
*
*/

void gsmFrmCopyGsmError (gsmInterface_t * interface);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*---------------------------------------------------------------------------*/
/*                  General GSM library operation functions                  */
/*---------------------------------------------------------------------------*/

static bool gsmInitInterface(gsmInterface_t * interface)
{
   bool ok = FALSE;

   SysTickInterface = interface;

   ok = gsmInitEngine(&(interface->engine)); /* Initializes the GSM engine */

   interface->frmState = IDLE;
   interface->procState = NOCMD;

   interface->procCnt = DELAY_PROC;

   interface->urcMode = MANUAL_MODE;
   interface->urcCback = NULL;

   interface->dataCback = NULL;
   interface->exitCmdList = &exitCmds;

   interface->frm = NULL;
   interface->frmInput = NULL;
   interface->frmOutput = NULL;
   interface->frmCback = NULL;
   interface->errorOut.errorFrm = OK;

   return ok;
}


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

static void gsmStartUpF (gsmInterface_t * interface)
{
   fsmEvent_e result;

   switch(interface->frmState) {

      case INIT:


         gsmInitInterface(interface);      /* Initializes the GSM interface */

         gsmFrmInit(interface);

         break;

      case PROC:

         switch(interface->procState){

            case ATCMD1:

               gsmFrmSendCmdCheckEcho(interface, "AT\r", ATCMD1RESP);

               break;

            case ATCMD1RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD2, FALSE, NOCMD, TRUE);

               break;

            case ATCMD2:

               gsmFrmSendCmdCheckEcho(interface, "AT+CMEE=2\r", ATCMD2RESP);

               break;

            case ATCMD2RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD3, FALSE, NOCMD, TRUE);

               break;

            case ATCMD3:

               gsmFrmSendCmdCheckEcho(interface, "AT+CSCS=\"GSM\"\r",
                                      ATCMD3RESP);

               break;

            case ATCMD3RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD4, FALSE, NOCMD, TRUE);

               break;

            case ATCMD4:

               gsmFrmSendCmdCheckEcho(interface, "AT+CMGF=1\r", ATCMD4RESP);

               break;

            case ATCMD4RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD4RESP, TRUE, NOCMD,
                                      TRUE);

               break;

         }

         break;

      case WRAP:

         if(OK != interface->errorOut.errorFrm){

            if(ERR_GSM == interface->errorOut.errorFrm){
               gsmFrmCopyGsmError(interface);
            }
         }

         interface->frmCback(interface->errorOut, 0);
         interface->frmState = IDLE;

         break;
   }

   return;

}

/*****************************************************************************/

static void gsmGetSigQualF (gsmInterface_t * interface)
{
   fsmEvent_e result;

   switch(interface->frmState) {

      case INIT:

         gsmFrmInit(interface);

         break;

      case PROC:

         switch(interface->procState){

            case ATCMD1:

               gsmFrmSendCmdCheckEcho(interface, "AT+CSQ\r", ATCMD1RESP);

               break;

            case ATCMD1RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD1RESP, TRUE, NOCMD,
                                      TRUE);

               break;

         }

         break;

      case WRAP:

         if(OK == interface->errorOut.errorFrm){

            rsp_t rsp;
            uint8_t auxStr[2+1];  /* auxiliary string for response parsing of
                                   * the RSSI and BER values
                                   * XX\0 --> max string length 3 */
            int8_t auxVal;   /* auxiliary integer to store the RSSI value and
                                make all the math below more readable */

            auxStr[0] = '\0';

            uint8_t i;
            uint8_t commaPos = 0; /* position of the comma in the response */

            rsp = gsmGetCmdRsp(&(interface->engine));  /* get the signal
                                                          quality response */

            /* Search for the comma in the response, which is in the form RSSI,
             * BER */

            for(i = 0; i < (strlen(rsp.par)-1); i++){
               if(',' == rsp.par[i]){commaPos = i; break;}
            }

            if(commaPos != 0){

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

               auxVal = atoi(auxStr); /* convert the RSSI string value into
                                         an integer for the math below */

               if(0 == auxVal) {auxVal = -115;}

               else if(1 == auxVal) {auxVal = -111;}

               else if(auxVal >= 2 && auxVal <= 30){
                  auxVal = -110 + (auxVal -2)*2;
               }

               else if(31 == auxVal) {auxVal = -52;}

               else if(99 == auxVal) {auxVal = 99;}

               ((sigQual_s *)interface->frmOutput)->rssi = auxVal;

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

               ((sigQual_s *)interface->frmOutput)->ber = atoi(auxStr);

               debug(">>>interf<<<   RSSI: ");
               itoa(((sigQual_s *)interface->frmOutput)->rssi, auxStr, 10);
               debug(auxStr);
               debug(", RXQUAL: ");
               itoa(((sigQual_s *)interface->frmOutput)->ber, auxStr, 10);
               debug(auxStr);
               debug(" \r\n");

               }

            else{

               interface->errorOut.errorFrm = ERR_WRAP;

            }

         }

         else if(ERR_GSM == interface->errorOut.errorFrm){
            gsmFrmCopyGsmError(interface);
         }

         interface->frmCback(interface->errorOut, interface->frmOutput);
         interface->frmState = IDLE;

         break;
   }

   return;

}

/*****************************************************************************/

void gsmCheckConnF (gsmInterface_t * interface)
{
   fsmEvent_e result;

   /* We use two different rsp_t structure variables in this case because we
    * need the result of two successive commands. As the engine flushes the
    * rspVlRb VLRB after each command is closed, we would lose the response
    * of the first command. Therefore, we need to save it in a separate
    * variable. All other formula functions use the result of the last sent
    * command and therefore the rspVlRb is not yet flushed.
    */

   static rsp_t rspGsm;
   static rsp_t rspGprs;

   switch(interface->frmState) {

      case INIT:

         gsmFrmInit(interface);

         break;

      case PROC:

         switch(interface->procState){

            case ATCMD1:

               gsmFrmSendCmdCheckEcho(interface, "AT+CREG?\r", ATCMD1RESP);

               break;

            case ATCMD1RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD2, FALSE, NOCMD, TRUE);

               if(ATCMD2 == interface->procState) {
                  rspGsm = gsmGetCmdRsp(&(interface->engine));
                  /* if command was closed without error, store the GSM rsp */
               }

               break;

            case ATCMD2:

               gsmFrmSendCmdCheckEcho(interface, "AT+CGATT?\r", ATCMD2RESP);

               break;

            case ATCMD2RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD2RESP, TRUE, NOCMD, TRUE);

               if(ATCMD2RESP == interface->procState) {
                  rspGprs = gsmGetCmdRsp(&(interface->engine));
                  /* if command was closed without error, store the GPRS rsp */
               }

               break;

         }

         break;

      case WRAP:

         if(OK == interface->errorOut.errorFrm){

            /* The AT+CREG? response is of the form "+CREG: <n>,<stat>" where
             * both <n> and <stat> are single-digit integers and <stat> is the
             * actual GSM network status we care about. Therefore, we take char
             * 3 of the parameter part of the response.
             * */

            ((connStatus_s *)interface->frmOutput)->gsm =
            ('1' == rspGsm.par[3]);

            /* Print out the GSM string */

            debug(">>>interf<<<   GSM String:");
            debug(&rspGsm.par[0]);
            debug(" \r\n");

            /* The AT+CGATT? response is of the form "+CGATT: <state>" where
             * both <state> is a single-digit integer.Therefore, we take char
             * 1 of the parameter part of the response.
             * */

            ((connStatus_s *)interface->frmOutput)->gprs =
            ('1' == rspGprs.par[1]);

            /* Print out the GPRS string */

            debug(">>>interf<<<   GPRS String:");
            debug(&rspGprs.par[0]);
            debug(" \r\n");

         }

         else if(ERR_GSM == interface->errorOut.errorFrm){
            gsmFrmCopyGsmError(interface);
         }

         interface->frmCback(interface->errorOut, interface->frmOutput);
         interface->frmState = IDLE;

         break;
   }

   return;

}

/*---------------------------------------------------------------------------*/
/*                          SMS formula functions                            */
/*---------------------------------------------------------------------------*/

/*****************************************************************************/

static void gsmSmsSendF (gsmInterface_t * interface)
{
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

   switch(interface->frmState) {

      case INIT:

         smsCmd[0] = '\0';
         smsText[0] = '\0';

         /* If the SMS text has '\r' or '\n' characters it could confuse the
          * tokenizer. Therefore, we check that neither of these chars are
          * present.
          */

         if ( (strchr(((smsOut_s *)interface->frmInput)->text, '\n') != NULL)
               &&
              (strchr(((smsOut_s *)interface->frmInput)->text, '\r') != NULL)){

            debug(">>>interf<<<   ERROR: The SMS text contains the \\r and/or "
                                         "\\n characters\r\n");
            interface->errorOut.errorFrm = ERR_INIT;
            interface->frmState = WRAP;

         }

         /* We assemble the AT+CMGS command copying the phone number string */

         strncat(smsCmd, "AT+CMGS=\"", strlen("AT+CMGS=\""));
         strncat(smsCmd, ((smsOut_s *)interface->frmInput)->dest,
                 strlen(((smsOut_s *)interface->frmInput)->dest));
         strncat(smsCmd, "\"\r", strlen("\"\r"));

         /* We assemble the SMS text command */

         strncpy(smsText, ((smsOut_s *)interface->frmInput)->text,
                 strlen(((smsOut_s *)interface->frmInput)->text));
         smsText[strlen(((smsOut_s *)interface->frmInput)->text)] = '\0';

         gsmFrmInit(interface);

         debug(">>>interf<<<   SEND SMS CMD: ");
         debug(smsCmd);
         debug("\r\n>>>interf<<<   SMS TEXT: ");
         debug(smsText);
         debug("\r\n");

         break;

      case PROC:

         switch(interface->procState){

            case ATCMD1:

               gsmFrmSendCmdCheckEcho(interface, smsCmd, ATCMD1RESP);

               break;

            case ATCMD1RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD2, FALSE, NOCMD, TRUE);

               break;

            case ATCMD2:

               gsmFrmSendCmdCheckEcho(interface, smsText, ATCMD2RESP);

               break;

            case ATCMD2RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD2RESP, TRUE, NOCMD,
                                      TRUE);

               break;

         }

         break;

      case WRAP:

         if(OK != interface->errorOut.errorFrm){

            if(ERR_GSM == interface->errorOut.errorFrm){
               gsmFrmCopyGsmError(interface);
            }

         }

         else{

               /* If the SMS is successfully sent we are given a <mr> number
                * which represents the number assigned in the TE to the sent
                * SMS. We store it in the smsConf_s structure.
                */

               rsp_t rsp;

               rsp = gsmGetCmdRsp(&(interface->engine)); /* discard the final OK response */
               rsp = gsmGetCmdRsp(&(interface->engine)); /* get the "+CMGS:<mr>\r\n" response */

               /* Convert the <mr> number string into an integer type  and
                * store it for reference */

               atoi(((smsConf_s *)interface->frmOutput)->mr, rsp.par, 10);

         }

         interface->frmCback(interface->errorOut, interface->frmOutput);
         interface->frmState = IDLE;

         break;
   }

   return;

}

/*****************************************************************************/

static void gsmSmsReadF (gsmInterface_t * interface)
{
   fsmEvent_e result;

   static uint8_t strSmsRead[8+5+1]; /* string for the SMS read command*/
                                     /* AT+CMGR=XXX,Y\0 --> max str len 14 */

   uint8_t auxStr[3+1];              /* aux string for the idx SMS number */
                                     /* XXX\0 --> max str length 4 */

   switch(interface->frmState) {

      case INIT:

         strSmsRead[0] = '\0';

         /* We assemble the AT+CMGR command copying the idx and mode variables
          * into the string after converting them from integers to text
          */

         strncat(strSmsRead, "AT+CMGR=", strlen("AT+CMGR="));
         itoa(((smsReadPars_s *)interface->frmInput)->idx, auxStr, 10);
         strncat(strSmsRead, auxStr, strlen(auxStr));
         strncat(strSmsRead, ",", strlen(","));
         itoa(((smsReadPars_s *)interface->frmInput)->mode, auxStr, 10);
         strncat(strSmsRead, auxStr, strlen(auxStr));
         strncat(strSmsRead, "\r", strlen("\r"));

         debug(">>>interf<<<   strSmsRead: ");
         debug(strSmsRead);
         debug("\r\n");

         gsmFrmInit(interface);

         break;

      case PROC:

         switch(interface->procState){

            case ATCMD1:

               gsmFrmSendCmdCheckEcho(interface, "AT+CSDH=1\r", ATCMD1RESP);

               break;

            case ATCMD1RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD2, FALSE, NOCMD, TRUE);

               break;

            case ATCMD2:

               gsmFrmSendCmdCheckEcho(interface, strSmsRead, ATCMD2RESP);

               break;

            case ATCMD2RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD2RESP, TRUE, NOCMD,
                                      TRUE);

               break;

         }

         break;

      case WRAP:

         if(OK == interface->errorOut.errorFrm){

            rsp_t rsp;

            /* We copy the meta part of the SMS and the actual text into the
             * corresponding parts of the smsRec structure
             */

            rsp = gsmGetCmdRsp(&(interface->engine));
            strncpy(((smsRec_s *)interface->frmOutput)->meta, rsp.par,
                    strlen(rsp.par));
            ((smsRec_s *)interface->frmOutput)->meta[strlen(rsp.par)] = '\0';

            rsp = gsmGetCmdRsp(&(interface->engine));
            strncpy(((smsRec_s *)interface->frmOutput)->text, rsp.par,
                    strlen(rsp.par));
            ((smsRec_s *)interface->frmOutput)->text[strlen(rsp.par)] = '\0';

         }

         else if(ERR_GSM == interface->errorOut.errorFrm){
            gsmFrmCopyGsmError(interface);
         }

         smsListRet_s msgVector; /* Message vector includes the actual array
                                    and the number of messages; this is
                                    needed for further processing of the SMSs
                                    and compatibility with gsmSmsList */

         msgVector.msgs = interface->frmOutput;
         msgVector.noMsgs = 1;

         interface->frmCback(interface->errorOut, &msgVector);
         interface->frmState = IDLE;

         break;
   }

   return;

}

/*****************************************************************************/

static void gsmSmsListF (gsmInterface_t * interface)
{
   fsmEvent_e result;

   /* String to assemble the SMS list command
    *
    * AT+CMGL="(status)",Y\r\0 --> max string length */

   static uint8_t strSmsList[9+10+3+2];

   /* Auxiliary string. Longest status string is "REC UNREAD" which is 10
    * chars long, so the string needs to be 11 chars long including the \0  */

   static uint8_t auxStr[10+1];

   switch(interface->frmState) {

      case INIT:

         strSmsList[0] = '\0';

         /* We assemble the AT+CMGL command copying the stat and mode variables
          * into the string
          */

         strncat(strSmsList, "AT+CMGL=", strlen("AT+CMGL="));

         switch(((smsListPars_s *)interface->frmInput)->stat){

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
         itoa(((smsListPars_s *)interface->frmInput)->mode, auxStr, 10);
         strncat(strSmsList, auxStr, strlen(auxStr));
         strncat(strSmsList, "\r", strlen("\r"));

         debug(">>>interf<<<   strSmsList: ");
         debug(strSmsList);
         debug("\r\n");

         gsmFrmInit(interface);

         break;

      case PROC:

         switch(interface->procState){

            case ATCMD1:

               gsmFrmSendCmdCheckEcho(interface, "AT+CSDH=1\r", ATCMD1RESP);

               break;

            case ATCMD1RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD2, FALSE, NOCMD, TRUE);

               break;

            case ATCMD2:

               gsmFrmSendCmdCheckEcho(interface, strSmsList, ATCMD2RESP);

               break;

            case ATCMD2RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD2RESP, TRUE, NOCMD,
                                      TRUE);

               break;

         }

         break;

      case WRAP:

         ;
         uint8_t rspNo;
         rspNo = gsmGetNoCmdRsp(&(interface->engine));

         if(OK == interface->errorOut.errorFrm){

            uint8_t i = 0;

            rsp_t rsp;
            smsRec_s * target = (smsRec_s *)interface->frmOutput;

            /* Check that the list vector is big enough to hold all the SMSs.
             * We need to discard the final OK response and each SMS is split
             * into two responses: the meta data and the actual text. The
             * number of SMSs is therefore the number of responses - 1 divided
             * by two. */

            if(((smsListPars_s *)interface->frmInput)->listSize
               < ((rspNo-1)/2)){
               interface->errorOut.errorFrm = ERR_WRAP;
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

                     rsp = gsmGetCmdRsp(&(interface->engine));
                     strncpy((target+i)->meta, rsp.par, strlen(rsp.par));
                     (target+i)->meta[strlen(rsp.par)] = '\0';

                     rsp = gsmGetCmdRsp(&(interface->engine));
                     strncpy((target+i)->text, rsp.par, strlen(rsp.par));
                     (target+i)->text[strlen(rsp.par)] = '\0';

                  }

               }

            }

         }

         else if(ERR_GSM == interface->errorOut.errorFrm){
            gsmFrmCopyGsmError(interface);
         }

         smsListRet_s msgVector; /* Message vector includes the actual array
                                    andnthe number of messages */

         msgVector.msgs = interface->frmOutput;
         msgVector.noMsgs = (rspNo-1)/2;

         interface->frmCback(interface->errorOut, &msgVector);
         interface->frmState = IDLE;

         break;
   }

   return;

}

/*****************************************************************************/

static void gsmSmsDelF (gsmInterface_t * interface)
{
   uint8_t auxStr[4]; /* auxiliary string */
                      /* XXX\0 --> max str length 4 */

   /* String to assemble the SMS delete command
    *
    * AT+CMGD=XXX,Y\r\0 --> max string length 15 */

   static uint8_t smsDel[8+3+1+1+2];

   fsmEvent_e result;

   switch(interface->frmState) {

      case INIT:

         auxStr[0] = '\0';
         smsDel[0] = '\0';

         /* We assemble the AT+CMGD command copying the idx and mode variables
          * into the string
          */

         strncat(smsDel, "AT+CMGD=", strlen("AT+CMGD="));
         itoa(((smsDelPars_s *)interface->frmInput)->idx, auxStr, 10);
         strncat(smsDel, auxStr, strlen(auxStr));
         strncat(smsDel, ",", strlen(","));
         itoa(((smsDelPars_s *)interface->frmInput)->mode, auxStr, 10);
         strncat(smsDel, auxStr, strlen(auxStr));
         strncat(smsDel, "\r", strlen("\r"));

         gsmFrmInit(interface);

         break;

      case PROC:

         switch(interface->procState){

            case ATCMD1:

               gsmFrmSendCmdCheckEcho(interface, smsDel, ATCMD1RESP);

               break;

            case ATCMD1RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD1RESP, TRUE, NOCMD,
                                      TRUE);

               break;

         }

         break;

      case WRAP:

         if(OK != interface->errorOut.errorFrm){

            if(ERR_GSM == interface->errorOut.errorFrm){
               gsmFrmCopyGsmError(interface);
            }

         }

         interface->frmCback(interface->errorOut, 0);
         interface->frmState = IDLE;
         break;
   }

   return;
}

/*---------------------------------------------------------------------------*/
/*                         GPRS formula functions                            */
/*---------------------------------------------------------------------------*/

static void gsmGprsStartF (gsmInterface_t * interface)
{
   /* String to assemble the GPRS start command
    *
    * apn, user and pwd are arbitrarely assumed to be no longer than 30 chars
    * each (a reasonable assumption by all practical accounts)
    *
    * AT+CSTT="(apn)","(user)","(pwd)"\r\0 --> max string length */

   static uint8_t APNstring[9+30+3+30+3+30+3];

   fsmEvent_e result;

   switch(interface->frmState) {

      case INIT:

         APNstring[0] = '\0';

         /* We assemble the AT+CSTT command copying the apn, user and pwd
          * variables into the string
          */

         strncat(APNstring, "AT+CSTT=\"", strlen("AT+CSTT=\""));
         strncat(APNstring, ((apnUserPwd_s *)interface->frmInput)->apn,
                             strlen(((apnUserPwd_s *)interface->frmInput)->apn));
         strncat(APNstring, "\",\"", strlen("\",\""));
         strncat(APNstring, ((apnUserPwd_s *)interface->frmInput)->user,
                             strlen(((apnUserPwd_s *)interface->frmInput)->user));
         strncat(APNstring, "\",\"", strlen("\",\""));
         strncat(APNstring, ((apnUserPwd_s *)interface->frmInput)->pwd,
                             strlen(((apnUserPwd_s *)interface->frmInput)->pwd));
         strncat(APNstring, "\"\r", strlen("\"\r"));

         gsmFrmInit(interface);

         break;

      case PROC:

         switch(interface->procState){

            case ATCMD1:

               gsmFrmSendCmdCheckEcho(interface, "AT+CIPSHUT\r", ATCMD1RESP);

               break;

            case ATCMD1RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD2, FALSE, NOCMD, TRUE);

               break;

            case ATCMD2:

               gsmFrmSendCmdCheckEcho(interface, "AT+CIPMODE=1\r", ATCMD2RESP);

               break;

            case ATCMD2RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD3, FALSE, NOCMD, TRUE);

               break;

            case ATCMD3:

               gsmFrmSendCmdCheckEcho(interface, APNstring, ATCMD3RESP);

               break;

            case ATCMD3RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD4, FALSE, NOCMD, TRUE);

               break;

            case ATCMD4:

               gsmFrmSendCmdCheckEcho(interface, "AT+CIICR\r", ATCMD4RESP);

               break;

            case ATCMD4RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD5, FALSE, NOCMD, TRUE);

               break;

            case ATCMD5:

               gsmFrmSendCmdCheckEcho(interface, "AT+CIFSR\r", ATCMD5RESP);

               break;

            case ATCMD5RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD5RESP, TRUE, NOCMD,
                                      TRUE);

               break;

         }

         break;

      case WRAP:

         if(OK != interface->errorOut.errorFrm){

            if(ERR_GSM == interface->errorOut.errorFrm){
               gsmFrmCopyGsmError(interface);
            }

         }

         interface->frmCback(interface->errorOut, 0);
         interface->frmState = IDLE;

         break;
   }

   return;
}

/*****************************************************************************/

static void gsmGprsOpenPortF (gsmInterface_t * interface)
{
   /* String to assemble the TCP/UDP open port command
    *
    * IP/domain is arbitrarely assumed to be no longer than 50 chars
    *
    * AT+CIPSTART="XXX","(IP/domain)",(port)\r\0 --> max string length 78 */

   static uint8_t socketStr[13+3+3+50+2+5+2];

   uint8_t auxStr[6]; /* auxiliary string */
                      /* XXXXX\0 --> max str length 6 */

   rsp_t rsp; /* auxiliary variable used to analize the response to CIPSTART
              /* and distinguish the open port attempt results */

   fsmEvent_e result;

   switch(interface->frmState) {

      case INIT:

         socketStr[0] = '\0';

         /* We assemble the AT+CIPSTART command copying the type, address and
          * port variables into the string
          */

         strncat(socketStr, "AT+CIPSTART=\"", strlen("AT+CIPSTART=\""));

         if(TCP == ((socket_s *)interface->frmInput)->type){
            strncat(socketStr, "TCP\",\"", strlen("TCP\",\""));
         }
         else if(UDP == ((socket_s *)interface->frmInput)->type){
            strncat(socketStr, "UDP\",\"", strlen("UDP\",\""));
         }

         strncat(socketStr, ((socket_s *)interface->frmInput)->address,
                 strlen(((socket_s *)interface->frmInput)->address));

         itoa(((socket_s *)interface->frmInput)->port, auxStr, 10);
         strncat(socketStr, "\",\"", strlen("\",\""));
         strncat(socketStr, auxStr, strlen(auxStr));
         strncat(socketStr, "\"\r", strlen("\"\r"));

         gsmFrmInit(interface);

         break;

      case PROC:

         switch(interface->procState){

            case ATCMD1:

               gsmFrmSendCmdCheckEcho(interface, "AT+CIPCLOSE\r", ATCMD1RESP);

               break;

            case ATCMD1RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD2, FALSE, ATCMD2, FALSE);

               /* Clear the error variable since we do not care about the
                * result of the CIPCLOSE command; it is there as a formality
                */

               interface->errorOut.errorFrm = OK;
               interface->errorOut.errorCmd.cmd[0] = '\0';
               interface->errorOut.errorCmd.par[0] = '\0';

               break;

            case ATCMD2:

               gsmFrmSendCmdCheckEcho(interface, socketStr, ATCMD2RESP);

               break;

            case ATCMD2RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD2RESP, TRUE, NOCMD,
                                      TRUE);

                  /* CIPSTART has several closing responses, some of which
                   * indicate an error. Therefore, we need to further analyze
                   * the specific response in order to determine any possible
                   * error messages. */

                  if(ATCMD2RESP == interface->procState &&
                     WRAP == interface->frmState){

                     uint8_t noRsp, i;
                     noRsp = gsmGetNoCmdRsp(&(interface->engine));

                     /* get the last response */
                     for (i = 0; i < noRsp; i++){
                        rsp = gsmGetCmdRsp(&(interface->engine));
                     }

                     if (0 == strcmp(rsp.cmd, "CONNECT FAIL")) {
                        interface->errorOut.errorFrm = ERR_PROC;
                     }

                  }

               break;

            }

            break;

      case WRAP:

         if(OK != interface->errorOut.errorFrm){

            if(ERR_GSM == interface->errorOut.errorFrm){
               gsmFrmCopyGsmError(interface);
            }

         }

         else{

            /* If the port is opened correctly we enter DATA_MODE */
            gsmSetSerialMode(&(interface->engine), DATA_MODE);

         }

         interface->frmCback(interface->errorOut, 0);
         interface->frmState = IDLE;

         break;
   }

   return;
}

/*****************************************************************************/

static void gsmGprsClosePortF (gsmInterface_t * interface)
{
   fsmEvent_e result;

   switch(interface->frmState) {

      case INIT:

         gsmFrmInit(interface);

         break;

      case PROC:

         switch(interface->procState){

            case ATCMD1:

               gsmFrmSendCmdCheckEcho(interface, "AT+CIPCLOSE=0\r",
                                      ATCMD1RESP);

               break;

            case ATCMD1RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD1RESP, TRUE, NOCMD,
                                      TRUE);

               break;

         }

         break;

      case WRAP:

         if(OK != interface->errorOut.errorFrm){

            if(ERR_GSM == interface->errorOut.errorFrm){
               gsmFrmCopyGsmError(interface);
            }

         }

         /* If the port is closed correctly we return to COMMAND_MODE */

         gsmSetSerialMode(&(interface->engine), COMMAND_MODE);

         interface->frmCback(interface->errorOut, 0);
         interface->frmState = IDLE;

         break;
   }

   return;
}

/*---------------------------------------------------------------------------*/
/*                         GNSS formula functions                            */
/*---------------------------------------------------------------------------*/

static void gsmGnssPwrF (gsmInterface_t * interface)
{
   /* String to assemble the GNSS on/off command
    *
    *
    * AT+CGNSPWR=X\r\0 --> max string length 14 */

   static uint8_t cmdStr[14];

   fsmEvent_e result;

   switch(interface->frmState) {

      case INIT:

         /* We assemble the AT+CGNSPWR string depending on the value of the
          * input pwrGnss variable
          */

         if(ON == *((pwrGnss_e *)interface->frmInput)){
            strncpy(cmdStr,"AT+CGNSPWR=1\r", strlen("AT+CGNSPWR=1\r"));
         }
         else if(OFF == *((pwrGnss_e *)interface->frmInput)){
            strncpy(cmdStr,"AT+CGNSPWR=0\r", strlen("AT+CGNSPWR=0\r"));
         }

         gsmFrmInit(interface);

         break;

      case PROC:

         switch(interface->procState){

            case ATCMD1:

               gsmFrmSendCmdCheckEcho(interface, cmdStr, ATCMD1RESP);

               break;

            case ATCMD1RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD1RESP, TRUE, NOCMD,
                                      TRUE);

               break;

         }

         break;

      case WRAP:

         if(OK != interface->errorOut.errorFrm){

            if(ERR_GSM == interface->errorOut.errorFrm){
               gsmFrmCopyGsmError(interface);
            }

         }

         interface->frmCback(interface->errorOut, 0);
         interface->frmState = IDLE;

         break;
   }

   return;

}

/*****************************************************************************/

static void gsmGnssGetDataF (gsmInterface_t * interface)
{
   fsmEvent_e result;

   switch(interface->frmState) {

      case INIT:

         gsmFrmInit(interface);

         break;

      case PROC:

         switch(interface->procState){

            case ATCMD1:

               gsmFrmSendCmdCheckEcho(interface, "AT+CGNSINF\r", ATCMD1RESP);

               break;

            case ATCMD1RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD1RESP, TRUE, NOCMD, TRUE);

               break;

         }

         break;

      case WRAP:

         if(OK == interface->errorOut.errorFrm){

            rsp_t rsp;

            rsp = gsmGetCmdRsp(&(interface->engine)); /* Get the navinfo str */

            /* Copy the navigation info string to the provided output. The
             * constant MAX_CGNSINF_SIZE is defined in gsmEngine.h and is
             * taken from the SIM808 manual as being 94 characters max */

            strncpy(((dataGnss_s *)interface->frmOutput)->data,&rsp.par[0],
                    MAX_CGNSINF_SIZE);
            ((uint8_t *)interface->frmOutput)[MAX_CGNSINF_SIZE] = '\0';

            debug(">>>interf<<<   NavInfo String:");
            debug(((dataGnss_s *)interface->frmOutput)->data);
            debug("\r\n");

         }

         else if(ERR_GSM == interface->errorOut.errorFrm){
            gsmFrmCopyGsmError(interface);
         }

         interface->frmCback(interface->errorOut, interface->frmOutput);
         interface->frmState = IDLE;

         break;
   }

   return;

}

/*****************************************************************************/

/*---------------------------------------------------------------------------*/
/*                     Formula-processing subroutines                        */
/*---------------------------------------------------------------------------*/

void gsmFrmInit (gsmInterface_t * interface)
{
   interface->errorOut.errorFrm = OK;
   interface->errorOut.errorCmd.cmd[0] = '\0';
   interface->errorOut.errorCmd.par[0] = '\0';

   interface->procState = ATCMD1;
   interface->frmState = PROC;

   return;
}

void gsmFrmSendCmdCheckEcho (gsmInterface_t * interface,
                             uint8_t const * const cmd, procStatus_e nextState)
{
   fsmEvent_e result;
   static Bool firstCall = TRUE;

   /* If this is the first call of the function for the current cmd we send
    * the str through the serial port; otherwise we pick up and process the
    * response tokens.
    */

   if(firstCall){
      result = gsmSendCmd(&(interface->engine), cmd);
      firstCall = FALSE;
   }
   else{result = gsmProcessTkn(&(interface->engine));}

   if(NO_UPDATE != result){
      if(OK_CMD_SENT == result){;}
      else if(OK_CMD_ACK == result){
         interface-> procState = nextState;
         firstCall = TRUE;
      }
      else{
         interface->errorOut.errorFrm = ERR_PROC;
         interface->frmState = WRAP;
         firstCall = TRUE;
         debug(">>>interf<<< Echo error");
      }
   }

   return;
}

void gsmFrmProcRspsGetFinal (gsmInterface_t * interface,
                             procStatus_e closingState, Bool closingWrap,
                             procStatus_e errorState, Bool errorWrap)
{
   fsmEvent_e result;

   result = gsmProcessTkn(&(interface->engine));

   if(NO_UPDATE != result){

      if(OK_RSP <= result && OK_URC >= result){;}
      else if(OK_CLOSE == result){
         interface->procState = closingState;
         interface->frmState = (closingWrap == TRUE ? WRAP : PROC );
      }
      else if(ERR_MSG_CLOSE == result){
         interface->errorOut.errorFrm = ERR_GSM;
         interface->procState = errorState;
         interface->frmState = (errorWrap == TRUE ? WRAP : PROC );
      }
      else if(ERR_TIMEOUT == result){
         interface->errorOut.errorFrm = ERR_TOUT;
         interface->procState = errorState;
         interface->frmState = (errorWrap == TRUE ? WRAP : PROC );
      }
      else if(ERR_FSM_OOR == result){
         interface->errorOut.errorFrm = ERR_FSM;
         interface->procState = errorState;
         interface->frmState = (errorWrap == TRUE ? WRAP : PROC );
      }
      else{
         interface->errorOut.errorFrm = ERR_UNK;
         interface->procState = errorState;
         interface->frmState = (errorWrap == TRUE ? WRAP : PROC );
      }
   }

   return;
}

void gsmFrmCopyGsmError (gsmInterface_t * interface)
{
   rsp_t rsp;

   rsp = gsmGetCmdRsp(&(interface->engine));
   strncpy(interface->errorOut.errorCmd.cmd, rsp.cmd, TKN_CMD_SIZE-1);
   interface->errorOut.errorCmd.cmd[TKN_CMD_SIZE] = '\0';
   strncpy(interface->errorOut.errorCmd.par, rsp.par, (TKN_PAR_SIZE/2)-1);
   interface->errorOut.errorCmd.par[TKN_PAR_SIZE/2] = '\0';

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
   gsmDecToutCnt(&(SysTickInterface->engine));
   if(SysTickInterface->procCnt > 0) SysTickInterface->procCnt--;

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

void gsmProcess (gsmInterface_t * interface)
{
   if(0 == interface->procCnt){

      if (IDLE == interface->frmState){ /* no formula currently being run */

         /* If we are in COMMAND_MODE, we first process a single token. */

         if(COMMAND_MODE == gsmGetSerialMode(&(interface->engine))){
            gsmProcessTkn(&(interface->engine));

            /* If the URC handling mode is CBACK_MODE, we check if there any
             * unprocessed URCs; if so, we get them and fire the user-defined
             * URC callback function.
             */

            if(CBACK_MODE == interface->urcMode){

               rsp_t dummy;
               dummy = gsmGetUrc(&(interface->engine));

               if('\0' != dummy.cmd[0]){
                  interface->urcCback(dummy.cmd, dummy.par);
               }
            }

         }

         /* If we are in DATA_MODE, we call the user-defined data handling
          * callback function.
          */

         else if (DATA_MODE == gsmGetSerialMode(&(interface->engine))){
            interface->dataCback();
         }

      }

      else {interface->frm(interface);} /* there is a formula being run;
                                           execute the formula */

      interface->procCnt = DELAY_PROC; /* reset gsmProcess counter */

      return;

   }

   else{

      return;

   }

}

/*****************************************************************************/

bool gsmIsIdle (gsmInterface_t * interface)
{
   return(IDLE == interface->frmState);
}

/*****************************************************************************/

bool gsmSetUrcMode (gsmInterface_t * interface, urcMode_e mode)
{
   interface->urcMode = mode;
   return true;
}

/*****************************************************************************/

bool gsmSetUrcCback (gsmInterface_t * interface, urcCback_t cback)
{
   interface->urcCback = cback;
   return true;
}

/*****************************************************************************/

bool gsmSetDataCback (gsmInterface_t * interface, dataCback_t cback)
{
   interface->dataCback = cback;
   return true;
}

/*****************************************************************************/

/* This interface-level function executes a serial port write of n characters
 * followed by a serial port read of n characters. It is used when in DATA_MODE
 * and calls upon two comms-level functions.
 *
 * To keep some coherence with the token-reading functions, the maximum read
 * size is kept at RD_BUF_SIZ/2 (constant in the gsmTokenizer module). If a
 * number larger than this is used, no read or write operation is made.
 */

void gsmWriteReadDataMode (uint8_t * write, uint8_t * nWrite, uint8_t * read,
                           uint8_t * nRead)
{

   if((*nWrite > RD_BUF_SIZ/2) || (*nRead > RD_BUF_SIZ/2)){

      *nWrite = 0;
      *nRead = 0;

   }

   else{

      *nWrite = gsm232UartSend(write, *nWrite);
      *nRead = gsm232UartRecv(read, *nRead);

   }

   return;
}

/*****************************************************************************/

/* This interface-level function checks the read characters in a DATA MODE
 * flow and detects any cmd in the exitCmdList vector, which are the ones used
 * by the modem when in DATA MODE to indicate it has gone back to COMMAND MODE.
 * If it finds any of these commands, it reverts to COMMAND MODE.
 *
 * It should be called periodically by the user in his dataCback function.*/

uint8_t gsmCheckDataMode (gsmInterface_t * interface,
                          uint8_t const * const buf,
                          uint8_t const * const nch){

   uint8_t ch = '\0';               /* character just read */
   static uint8_t pCh = '\0';       /* previous character read */

   static uint8_t cmdChCnt = 0;     /* number of characters in supposed cmd */

   static Bool crLf = FALSE;        /* initial "\r\n" sequence flag */

   uint8_t i = 0;
   uint8_t j = 0;
   uint8_t nChRet = 0;              /* how many characters of the sent buffer
                                       should be printed out; if one of the
                                       exit cmds is detected, everything after
                                       it is ignored */

   /* Start cycling through the characters in the buffer */

   for(i = 0; i < *nch; i++){

      pCh = ch;
      ch = buf[i];
      nChRet++;

      /* If an opening "\r\n" sequence has been received, we might be in
       * presence of a cmd by the modem, so we start counting the number of
       * chars. */

      if(crLf){

         cmdChCnt++;

         /* If a closing "\r\n" sequence is found, we check that the number
          * of chars so far it not bigger than the size of the largest exit
          * cmd. If this is not the case, we test with the vector of exit
          * cmds to see if we need to go to COMMAND MODE.*/

         if( ('\r' == pCh) && ('\n' == ch) ){

            if (cmdChCnt <= (CMD_MODE_SIZE +2)){

               /* Compare to all commands in the exitCmdList vector; if there
                * is a match, go to COMMAND MODE and reset the char count and
                * the crLf flag */

               for(j = 0; j < CMD_MODE_NO; j++){
                  if(0 == strncmp(interface->exitCmdList[j],
                                  &buf[i-(cmdChCnt-1)],
                                  cmdChCnt-2)){
                     cmdChCnt = 0;
                     crLf = 0;
                     gsmSetSerialMode(&(interface->engine), COMMAND_MODE);
                     break;
                  }
               }

            }

            /* If the number of chars is too large for this to be an exit cmd,
             * we reset the char count and crLf flag */

            else{cmdChCnt = 0; crLf = FALSE;}

         }

      }

      else{

         /* Raise the crLf flag if the "\r\n" opening sequence has just been
          * received.
          */

         if( ('\r' == pCh) && ('\n' == ch) ){
            crLf = TRUE;}

      }


   }

   return nChRet; /* Return the number of chars to be printed out */
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

void gsmStartUp (gsmInterface_t * interface, frmCback_t cback)
{
   interface->frm = gsmStartUpF;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmGetSigQual (gsmInterface_t * interface, sigQual_s * sigQual,
                    frmCback_t cback)
{
   interface->frm = gsmGetSigQualF;
   interface->frmOutput = sigQual;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmCheckConn (gsmInterface_t * interface, connStatus_s * status,
                   frmCback_t cback)
{
   interface->frm = gsmCheckConnF;
   interface->frmOutput = status;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/*---------------------------------------------------------------------------*/
/*                              SMS functions                                */
/*---------------------------------------------------------------------------*/

void gsmSmsSend (gsmInterface_t * interface, smsOut_s * msg, smsConf_s * conf,
                 frmCback_t cback)
{
   interface->frm = gsmSmsSendF;
   interface->frmInput = msg;
   interface->frmOutput = conf;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmSmsRead (gsmInterface_t * interface, smsRec_s * msg,
                 smsReadPars_s * pars, frmCback_t cback)
{
   interface->frm = gsmSmsReadF;
   interface->frmInput = pars;
   interface->frmOutput = msg;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmSmsList (gsmInterface_t * interface, smsRec_s * list,
                 smsListPars_s * pars, frmCback_t cback)
{
   interface->frm = gsmSmsListF;
   interface->frmInput = pars;
   interface->frmOutput = list;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmSmsDel (gsmInterface_t * interface, smsDelPars_s * msgdel,
                frmCback_t cback)
{
   interface->frm = gsmSmsDelF;
   interface->frmInput = msgdel;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/*---------------------------------------------------------------------------*/
/*                             GPRS functions                                */
/*---------------------------------------------------------------------------*/

void gsmGprsStart (gsmInterface_t * interface, apnUserPwd_s * apn,
                   frmCback_t cback)
{
   interface->frm = gsmGprsStartF;
   interface->frmInput = apn;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmGprsOpenPort (gsmInterface_t * interface, socket_s * socket,
                      frmCback_t cback)
{
   interface->frm = gsmGprsOpenPortF;
   interface->frmInput = socket;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmGprsClosePort (gsmInterface_t * interface, frmCback_t cback)
{
   interface->frm = gsmGprsClosePortF;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/*---------------------------------------------------------------------------*/
/*                             GNSS functions                                */
/*---------------------------------------------------------------------------*/

void gsmGnssPwr (gsmInterface_t * interface,
                 pwrGnss_e * cmd, frmCback_t cback)
{
   interface->frm = gsmGnssPwrF;
   interface->frmInput = cmd;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmGnssGetData (gsmInterface_t * interface,
                     dataGnss_s * dataGnss, frmCback_t cback)
{
   interface->frm = gsmGnssGetDataF;
   interface->frmOutput = dataGnss;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
