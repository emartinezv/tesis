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

/** @brief This module handles the user interface and formulas.
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

#define UNIT_TESTING

/** @brief Number of commands which return the modem to data mode. See
           gsmCheckDataMode function for more details */
#define CMD_MODE_NO 2

/** @brief Max size of the commands which return the modem to data mode. See
           gsmCheckDataMode function for more details */
#define CMD_MODE_SIZE 6

/*==================[global data]============================================*/

/*==================[internal functions declaration]=========================*/

#ifdef UNIT_TESTING

/* Local itoa function version for compilation in unit testing under strict
 * gcc. Taken from https://github.com/wsq003/itoa_for_linux/blob/master/itoa.c
 * under GNU Lesser General Public License v3.0
 *
 * Modified code to include a dummy input variable even if it is not used (no
 * functional difference).
 *
 * */

int itoa(int val, char* buf, int dummy)
{
    const unsigned int radix = 10;

    char* p;
    unsigned int a;        //every digit
    int len;
    char* b;            //start of the digit char
    char temp;
    unsigned int u;

    p = buf;

    if (val < 0)
    {
        *p++ = '-';
        val = 0 - val;
    }
    u = (unsigned int)val;

    b = p;

    do
    {
        a = u % radix;
        u /= radix;

        *p++ = a + '0';

    } while (u > 0);

    len = (int)(p - buf);

    *p-- = 0;

    //swap
    do
    {
        temp = *p;
        *p = *b;
        *b = temp;
        --p;
        ++b;

    } while (b < p);

    return len;
}

#endif

/*---------------------------------------------------------------------------*/
/*              General GSM library operation formula functions              */
/*---------------------------------------------------------------------------*/

/** @brief Formula to start up the GSM library */

static void gsmStartUpF (gsmInterface_t * interface);

/** @brief Formula to get signal quality values (RSSI and BER) */

static void gsmGetSigQualF (gsmInterface_t * interface);

/** @brief Formula to check status of GSM and GPRS connection */

static void gsmCheckConnF (gsmInterface_t * interface);

/*---------------------------------------------------------------------------*/
/*                          SMS formula functions                            */
/*---------------------------------------------------------------------------*/

/** @brief Formula to send an SMS */

static void gsmSmsSendF (gsmInterface_t * interface);

/** @brief Formula to read a single SMS */

static void gsmSmsReadF (gsmInterface_t * interface);

/** @brief Formula to read several received SMSs into a list */

static void gsmSmsListF (gsmInterface_t * interface);

/** @brief Formula to delete a single SMS from memory */

static void gsmSmsDelF (gsmInterface_t * interface);

/*---------------------------------------------------------------------------*/
/*                         GPRS formula functions                            */
/*---------------------------------------------------------------------------*/

/** @brief Formula to start the GPRS connection */

static void gsmGprsStartF (gsmInterface_t * interface);

/** @brief Formula to open a TCP or UDP port */

static void gsmGprsOpenPortF (gsmInterface_t * interface);

/** @brief Formula to close a TCP or UDP port */

static void gsmGprsClosePortF (gsmInterface_t * interface);

/*---------------------------------------------------------------------------*/
/*                         GNSS formula functions                            */
/*---------------------------------------------------------------------------*/

/** @brief Formula to turn GNSS power on or off */

static void gsmGnssPwrF (gsmInterface_t * interface);

/** @brief Formula to get GNSS data */

static void gsmGnssGetDataF (gsmInterface_t * interface);

/*---------------------------------------------------------------------------*/
/*                     Formula-processing subroutines                        */
/*---------------------------------------------------------------------------*/

/** @brief Initialize formula state variables and error structure
*
* @param interface : Pointer to interface
*
*/

static void gsmFrmInit (gsmInterface_t * interface);

/** @brief Subroutine to send a cmd and check for the correct echo
*
* @param interface : Pointer to interface
* @param cmd       : Pointer to cmd string
* @param nextState : Next procState to go to if echo is correct
*
* @return
*/

static void gsmFrmSendCmdCheckEcho (gsmInterface_t * interface,
                             uint8_t const * const cmd,
                             procStatus_t nextState);

/** @brief Subroutine to process all responses and check for a final result:
 *         either a closing response, error response or timeout. The frm states
 *         are adjusted accordingly
*
* @param interface    : Pointer to interface
* @param closingState : Next procstate to go to if final response was closing
* @param closingWrap  : Go to WRAP after sucessful closing response?
* @param errorState   : Next procstate to go to if final response was error
* @param errorWrap    : Go to WRAP after error closing response?
*
* @return
*/

static void gsmFrmProcRspsGetFinal (gsmInterface_t * interface,
                             procStatus_t closingState, Bool closingWrap,
                             procStatus_t errorState, Bool errorWrap);

/** @brief If a GSM error has been detected, it recalls the last response and
 *         extracts the cmd and par portions of the error message into the
 *         errorOut variable
*
* @param interface : Pointer to interface
*
*/

static void gsmFrmCopyGsmError (gsmInterface_t * interface);

/*==================[internal data definition]===============================*/

/** @brief List of commands that signal an exit from DATA_MODE */
static uint8_t const * const exitCmds [] = {"OK", "CLOSED"};

/** @brief Local file-level interface variable for SysTickHandler use */
static gsmInterface_t * SysTickInterface = NULL;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*---------------------------------------------------------------------------*/
/*                  General structure of formula functions                   */
/*---------------------------------------------------------------------------*/

/* All formula functions end with F (gsmStartUpF, smsSmsSendF, etc.). Their
 * main purpose is to send a number of AT commands and process the responses.
 * After sending each command we check that the modem response is valid and
 * continue to the next command until we are finished. If an error occurs in
 * any part of the process we stop and record the specific error. After we've
 * sent all the needed commands and received the appropiate responses, we
 * invoke a user-determined callback function which usually does something with
 * the responses of the commands we sent. The behaviour is rather linear and is
 * therefore implemented in a multi-level FSM.
 *
 * Formula functions are invoked regularly, with the period determined by the
 * DELAY_PROC constant. Each invocation usually gobbles up a single AT token
 * and updates the engine. Generally speaking, we have two levels:
 * frmStatus at the highest level and procStatus when actually sending
 * the SMS commands and processing the responses.
 *
 * frmStatus is limited to INIT-PROC-WRAP.
 *
 * INIT is the part of the function in which we initialize all the state and
 * error variables with the gsmFrmInit function and declare and initialize any
 * auxiliary variables we need.
 *
 * PROC is the part of the function in which we send the AT commands, check
 * the responses and report errors if any. Each invocation processes a single
 * token; procState changes when a command is either in error or completed.
 * The possible states at ATCMD1, ATCMD1RESP, ATCMD2, ATCMD2RESP,...etc.
 *
 * The general format is simple. Starting with ATCMD1, the first AT command is
 * sent with the gsmFrmSendCmdCheckEcho function; if the echo is correct, then
 * procState advances to ATCMD1RESP. There, the gsmFrmProcRspsGetFinal function
 * keeps reading tokens so that all non-closing responses to the command are
 * received and stored until we get either a closing response (OK_CLOSE) or
 * an error (ERR_MSG_CLOSE). This is repeated until we finish with all
 * commands. All deviations from this general format are commented.
 *
 * WRAP is the part of the function when we wrap up, making any further error
 * processing needed, parsing and formatting the output, calling the
 * user-defined callback function and finally resetting frmState to IDLE.
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
   switch(interface->frmState) {

      case INIT:

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

static void gsmExitDataModeF (gsmInterface_t * interface)
{
   switch(interface->frmState) {

      case INIT:

         /* The "+++" sequence must be preceeded and followed by a 1s no
            transmission period. We initialize the auxCnt to 1000 before
            calling gsmExitDataModeF and let the SysTickHandler routine get
            it down to zero before sending the '+++' sequence. */

         if(interface->auxCnt == 0){
            gsm232UartSend("+++",strlen("+++"));
            interface->auxCnt = 1000;
            interface->frmState = PROC;
         }

         break;

      case PROC:

         /* If auxCnt is zero it means the 1s we need after the '+++' sequence
            has passed */

         if(interface->auxCnt == 0){
            interface->frmState = WRAP;
         }

         break;

      case WRAP:

         interface->frmCback(interface->errorOut,NULL);
         interface->frmState = IDLE;

         break;
   }

   return;
}

/*****************************************************************************/

static void gsmGetSigQualF (gsmInterface_t * interface)
{
   fsmEvent_t result;

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
                                     the RSSI and BER values
                                     XX\0 --> max string length 3 */
            auxStr[0] = '\0';

            int8_t auxVal;        /* auxiliary integer to store the RSSI value
                                     and make the math below more readable */

            uint8_t i;
            uint8_t commaPos = 0; /* position of the comma in the response */

            /* get the signal quality response */
            rsp = gsmGetCmdRsp(&(interface->engine));

            /* Search for the ',' char in the response, which is in the form
               RSSI, BER */
            for(i = 0; i < (strlen(rsp.par)-1); i++){
               if(',' == rsp.par[i]){commaPos = i; break;}
            }

            if(commaPos != 0){

               /* Copy the string of RSSI and convert it into a integer. Then,
                * use the table in the SIM808 manual to translate this number
                * into an actual RSSI reading in dBm. The table reads:
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

               ((sigQual_t *)interface->frmOutput)->rssi = auxVal;

               /* Copy the string of BER and convert it to a number. The module
                * reports RXQUAL as per table in GSM 05.08 [20] subclause
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

               ((sigQual_t *)interface->frmOutput)->ber = atoi(auxStr);

               debug(">>>interf<<<   RSSI: ");
               itoa(((sigQual_t *)interface->frmOutput)->rssi, auxStr, 10);
               debug(auxStr);
               debug(", RXQUAL: ");
               itoa(((sigQual_t *)interface->frmOutput)->ber, auxStr, 10);
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
   /* We use two different rsp_t structure variables in this case because we
    * need the result of two successive commands. As the engine flushes the
    * rspVlRb VLRB after each command is closed, we would lose the response
    * of the first command. Therefore, we need to save it in a separate
    * variable. All other formula functions use the result of the last sent
    * command and therefore the rspVlRb is not yet flushed.
    */

   static rsp_t rspGsm;   /* Variable to save the GSM state response */
   static rsp_t rspGprs;  /* Variable to save the GPRS state response */

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

               if(ATCMD2RESP == interface->procState &&
                  WRAP == interface->frmState) {
                  rspGprs = gsmGetCmdRsp(&(interface->engine));
                  /* if command was closed without error, store the GPRS rsp */
               }

               break;

         }

         break;

      case WRAP:

         if(OK == interface->errorOut.errorFrm){

            /* The AT+CREG? response is of the form "+CREG: <n>,<stat>" where
               both <n> and <stat> are single-digit integers and <stat> is the
               actual GSM network status we care about. Therefore, we take char
               3 of the parameter part of the response. */

            ((connStatus_t *)interface->frmOutput)->gsm =
            ('1' == rspGsm.par[3]);

            debug(">>>interf<<<   GSM String:");
            debug(&rspGsm.par[0]);
            debug(" \r\n");

            /* The AT+CGATT? response is of the form "+CGATT: <state>" where
             * <state> is a single-digit integer.Therefore, we take char 1 of
             * the parameter part of the response.
             * */

            ((connStatus_t *)interface->frmOutput)->gprs =
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

   static uint8_t smsCmd[9+1+15+3]; /* SMS send command string */

   /* Maximum SMS size according to 3GPP standard TS 23.038 is 160 chars using
    * GSM 7-bit alphabet
    *
    * (smstext)\0 --> max str length 161 */

   static uint8_t smsText[160+1];  /* SMS text string */

   switch(interface->frmState) {

      case INIT:

         smsCmd[0] = '\0';
         smsText[0] = '\0';

         /* If the SMS text has '\r' or '\n' characters it could confuse the
          * tokenizer. Therefore, we check that neither of these chars are
          * present.
          */

         if ( (strchr(((smsOut_t *)interface->frmInput)->text, '\n') != NULL)
               ||
              (strchr(((smsOut_t *)interface->frmInput)->text, '\r') != NULL)){

            debug(">>>interf<<<   ERROR: The SMS text contains the \\r and/or "
                                         "\\n characters\r\n");
            interface->errorOut.errorFrm = ERR_INIT;
            interface->frmState = WRAP;

            return;

         }

         /* We assemble the AT+CMGS command copying the phone number string */

         strncat(smsCmd, "AT+CMGS=\"", strlen("AT+CMGS=\""));
         strncat(smsCmd, ((smsOut_t *)interface->frmInput)->dest,
                 strlen(((smsOut_t *)interface->frmInput)->dest));
         strncat(smsCmd, "\"\r", strlen("\"\r"));

         /* We assemble the SMS text command */

         strncpy(smsText, ((smsOut_t *)interface->frmInput)->text,
                 strlen(((smsOut_t *)interface->frmInput)->text));
         smsText[strlen(((smsOut_t *)interface->frmInput)->text)] = '\0';

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
                * SMS. We store it in the smsConf_t structure.
                */

               rsp_t rsp;

               /* get the "+CMGS:<mr>\r\n" response */
               rsp = gsmGetCmdRsp(&(interface->engine));

               /* Convert the <mr> number string into an integer type  and
                * store it for reference */

               ((smsConf_t *)interface->frmOutput)->mr = atoi(rsp.par);

               debug(">>>interf<<<   SENT MSG MR: ");
               debug(rsp.par);
               debug("\r\n");

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
   fsmEvent_t result;

   static uint8_t strSmsRead[8+5+1]; /* string for the SMS read command*/
                                     /* AT+CMGR=XXX,Y\0 --> max str len 14 */

   uint8_t auxStr[3+1];              /* aux string for the idx SMS number */
                                     /* XXX\0 --> max str length 4 */

   switch(interface->frmState) {

      case INIT:

         strSmsRead[0] = '\0';

         /* We assemble the AT+CMGR command copying the idx and mode variables
           into the string after converting them from integers to text */

         strncat(strSmsRead, "AT+CMGR=", strlen("AT+CMGR="));
         itoa(((smsReadPars_t *)interface->frmInput)->idx, auxStr, 10);
         strncat(strSmsRead, auxStr, strlen(auxStr));
         strncat(strSmsRead, ",", strlen(","));
         itoa(((smsReadPars_t *)interface->frmInput)->mode, auxStr, 10);
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
               corresponding parts of the smsRec structure */

            rsp = gsmGetCmdRsp(&(interface->engine));
            strncpy(((smsRec_t *)interface->frmOutput)->meta, rsp.par,
                    strlen(rsp.par));
            ((smsRec_t *)interface->frmOutput)->meta[strlen(rsp.par)] = '\0';

            rsp = gsmGetCmdRsp(&(interface->engine));
            strncpy(((smsRec_t *)interface->frmOutput)->text, rsp.par,
                    strlen(rsp.par));
            ((smsRec_t *)interface->frmOutput)->text[strlen(rsp.par)] = '\0';

         }

         else if(ERR_GSM == interface->errorOut.errorFrm){
            gsmFrmCopyGsmError(interface);
         }

         smsListRet_t msgVector; /* Message vector includes the actual array
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
   fsmEvent_t result;

   /* Maximum status string length is 10 ("REC UNREAD" or "STO UNSENT")

      AT+CMGL="(status)",Y\r\0 --> max string length 24 */

   static uint8_t strSmsList[9+10+3+2]; /* SMS list command string */

   static uint8_t auxStr[10+1]; /* Aux string, max length = status = 10 as
                                   per comment above */

   switch(interface->frmState) {

      case INIT:

         strSmsList[0] = '\0';

         /* We assemble the AT+CMGL command copying the stat and mode variables
          * into the string
          */

         strncat(strSmsList, "AT+CMGL=", strlen("AT+CMGL="));

         switch(((smsListPars_t *)interface->frmInput)->stat){

            case REC_UNREAD:

               strncpy(auxStr,"\"REC UNREAD\"",strlen("\"REC UNREAD\""));
               auxStr[strlen("\"REC UNREAD\"")] = '\0';

               break;

            case REC_READ:

               strncpy(auxStr,"\"REC READ\"",strlen("\"REC READ\""));
               auxStr[strlen("\"REC READ\"")] = '\0';

               break;

            case STO_UNSENT:

               strncpy(auxStr,"\"STO UNSENT\"",strlen("\"STO UNSENT\""));
               auxStr[strlen("\"STO UNSENT\"")] = '\0';

               break;

            case STO_SENT:

               strncpy(auxStr,"\"STO SENT\"",strlen("\"STO SENT\""));
               auxStr[strlen("\"STO SENT\"")] = '\0';

               break;

            case ALL_MSG:

               strncpy(auxStr,"\"ALL\"",strlen("\"ALL\""));
               auxStr[strlen("\"ALL\"")] = '\0';

               break;

         }

         strncat(strSmsList, auxStr, strlen(auxStr));
         strncat(strSmsList, ",", strlen(","));
         itoa(((smsListPars_t *)interface->frmInput)->mode, auxStr, 10);
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
         uint8_t rspNo; /* Number of responses to the SMS list command */
         rspNo = gsmGetNoCmdRsp(&(interface->engine));

         if(OK == interface->errorOut.errorFrm){

            uint8_t i = 0;

            rsp_t rsp;
            smsRec_t * target = (smsRec_t *)interface->frmOutput;

            /* Check that the list vector is big enough to hold all the SMSs.
               We need to discard the final OK response and each SMS is split
               into two responses: the meta data and the actual text. The
               number of SMSs is therefore the number of responses - 1 divided
               by two. */

            if(((smsListPars_t *)interface->frmInput)->listSize
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

         smsListRet_t msgVector; /* Message vector includes the actual array
                                    and the number of messages */

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
   uint8_t auxStr[4]; /* Auxiliary string */
                      /* XXX\0 --> max str length 4 */

   static uint8_t smsDel[8+3+1+1+2]; /* SMS delete command string */
                                     /* AT+CMGD=XXX,Y\r\0 --> max length 15 */

   switch(interface->frmState) {

      case INIT:

         auxStr[0] = '\0';
         smsDel[0] = '\0';

         /* We assemble the AT+CMGD command copying the idx and mode variables
          * into the string
          */

         strncat(smsDel, "AT+CMGD=", strlen("AT+CMGD="));
         itoa(((smsDelPars_t *)interface->frmInput)->idx, auxStr, 10);
         strncat(smsDel, auxStr, strlen(auxStr));
         strncat(smsDel, ",", strlen(","));
         itoa(((smsDelPars_t *)interface->frmInput)->mode, auxStr, 10);
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
   /* apn, user and pwd are arbitrarily assumed to be no longer than 30 chars
      each (a reasonable assumption by all practical accounts)

      AT+CSTT="(apn)","(user)","(pwd)"\r\0 --> max string length 111 */

   static uint8_t APNstring[9+30+3+30+3+30+3]; /* Start GPRS command string */

   switch(interface->frmState) {

      case INIT:

         APNstring[0] = '\0';

         /* We assemble the AT+CSTT command copying the apn, user and pwd
          * variables into the string
          */

         strncat(APNstring, "AT+CSTT=\"", strlen("AT+CSTT=\""));
         strncat(APNstring, ((apnUserPwd_t *)interface->frmInput)->apn,
                             strlen(((apnUserPwd_t *)interface->frmInput)->apn));
         strncat(APNstring, "\",\"", strlen("\",\""));
         strncat(APNstring, ((apnUserPwd_t *)interface->frmInput)->user,
                             strlen(((apnUserPwd_t *)interface->frmInput)->user));
         strncat(APNstring, "\",\"", strlen("\",\""));
         strncat(APNstring, ((apnUserPwd_t *)interface->frmInput)->pwd,
                             strlen(((apnUserPwd_t *)interface->frmInput)->pwd));
         strncat(APNstring, "\"\r", strlen("\"\r"));

         gsmFrmInit(interface);

         break;

      case PROC:

         switch(interface->procState){

            case ATCMD1:

               gsmFrmSendCmdCheckEcho(interface, "AT+CIPCLOSE\r", ATCMD1RESP);

               break;

            case ATCMD1RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD2, FALSE, ATCMD2, FALSE);

               break;

            case ATCMD2:

               gsmFrmSendCmdCheckEcho(interface, "AT+CIPSHUT\r", ATCMD2RESP);

               break;

            case ATCMD2RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD3, FALSE, NOCMD, TRUE);

               break;

            case ATCMD3:

               gsmFrmSendCmdCheckEcho(interface, "AT+CIPMODE=1\r", ATCMD3RESP);

               break;

            case ATCMD3RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD4, FALSE, NOCMD, TRUE);

               break;

            case ATCMD4:

               gsmFrmSendCmdCheckEcho(interface, APNstring, ATCMD4RESP);

               break;

            case ATCMD4RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD5, FALSE, NOCMD, TRUE);

               break;

            case ATCMD5:

               gsmFrmSendCmdCheckEcho(interface, "AT+CIICR\r", ATCMD5RESP);

               break;

            case ATCMD5RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD6, FALSE, NOCMD, TRUE);

               break;

            case ATCMD6:

               gsmFrmSendCmdCheckEcho(interface, "AT+CIFSR\r", ATCMD6RESP);

               break;

            case ATCMD6RESP:

               gsmFrmProcRspsGetFinal(interface, ATCMD6RESP, TRUE, NOCMD,
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

static void gsmGprsStopF (gsmInterface_t * interface)
{
   switch(interface->frmState) {

      case INIT:

         gsmFrmInit(interface);

         break;

      case PROC:

         switch(interface->procState){

            case ATCMD1:

               gsmFrmSendCmdCheckEcho(interface, "AT+CIPSHUT\r", ATCMD1RESP);

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

static void gsmGprsOpenPortF (gsmInterface_t * interface)
{
   /* IP/domain is arbitrarely assumed to be no longer than 50 chars

      AT+CIPSTART="XXX","(IP/domain)",(port)\r\0 --> max string length 78 */

   static uint8_t socketStr[13+3+3+50+2+5+2]; /* GPRS open port cmd string*/

   uint8_t auxStr[6]; /* Auxiliary string */
                      /* XXXXX\0 --> max str length 6 */

   rsp_t rsp; /* auxiliary variable used to analize the response to CIPSTART
              /* and distinguish the open port attempt results */

   switch(interface->frmState) {

      case INIT:

         socketStr[0] = '\0';

         /* We assemble the AT+CIPSTART command copying the type, address and
          * port variables into the string
          */

         strncat(socketStr, "AT+CIPSTART=\"", strlen("AT+CIPSTART=\""));

         if(TCP == ((socket_t *)interface->frmInput)->type){
            strncat(socketStr, "TCP\",\"", strlen("TCP\",\""));
         }
         else if(UDP == ((socket_t *)interface->frmInput)->type){
            strncat(socketStr, "UDP\",\"", strlen("UDP\",\""));
         }

         strncat(socketStr, ((socket_t *)interface->frmInput)->address,
                 strlen(((socket_t *)interface->frmInput)->address));

         itoa(((socket_t *)interface->frmInput)->port, auxStr, 10);
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
   fsmEvent_t result;

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

         else{

            /* If the port is closed correctly we return to COMMAND_MODE */
            gsmSetSerialMode(&(interface->engine), COMMAND_MODE);

         }

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
   static uint8_t cmdStr[14]; /* GNSS power on/off command string */
                              /* AT+CGNSPWR=X\r\0 --> max string length 14 */

   switch(interface->frmState) {

      case INIT:

         /* We assemble the AT+CGNSPWR string depending on the value of the
          * input pwrGnss variable
          */

         if(ON == *((pwrGnss_t *)interface->frmInput)){
            strncpy(cmdStr,"AT+CGNSPWR=1\r", strlen("AT+CGNSPWR=1\r"));
         }
         else if(OFF == *((pwrGnss_t *)interface->frmInput)){
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

            strncpy(((dataGnss_t *)interface->frmOutput)->data,&rsp.par[0],
                    MAX_CGNSINF_SIZE);
            ((uint8_t *)interface->frmOutput)[MAX_CGNSINF_SIZE] = '\0';

            debug(">>>interf<<<   NavInfo String:");
            debug(((dataGnss_t *)interface->frmOutput)->data);
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

static void gsmFrmInit (gsmInterface_t * interface)
{
   /* We initialize the error vector and the FSM formula-level and
      command-level state variables. */

   interface->errorOut.errorFrm = OK;
   interface->errorOut.errorCmd.cmd[0] = '\0';
   interface->errorOut.errorCmd.par[0] = '\0';

   interface->procState = ATCMD1;
   interface->frmState = PROC;
   interface->cmdSent = false;

   return;
}

static void gsmFrmSendCmdCheckEcho (gsmInterface_t * interface,
                             uint8_t const * const cmd, procStatus_t nextState)
{
   fsmEvent_t result;

   /* If this is the first call of the function for the current cmd we send
    * the str through the serial port; otherwise we pick up and process the
    * response tokens.
    */

   if(! interface->cmdSent){
      result = gsmSendCmd(&(interface->engine), cmd);
      interface->cmdSent = TRUE;
   }
   else{result = gsmProcessTkn(&(interface->engine));}

   if(NO_UPDATE != result){
      if(OK_CMD_SENT == result){;}

      /* If the cmd was acknowledged we progress to nextState*/
      else if(OK_CMD_ACK == result){
         interface-> procState = nextState;
         interface->cmdSent = FALSE;
      }

      /* If we received an echo error we go to WRAP directly */
      else{
         interface->errorOut.errorFrm = ERR_PROC;
         interface->frmState = WRAP;
         interface->cmdSent = FALSE;
         debug(">>>interf<<< Echo error");
      }
   }

   return;
}

static void gsmFrmProcRspsGetFinal (gsmInterface_t * interface,
                             procStatus_t closingState, Bool closingWrap,
                             procStatus_t errorState, Bool errorWrap)
{
   fsmEvent_t result;

   /* We process the most recent token */
   result = gsmProcessTkn(&(interface->engine));

   if(NO_UPDATE != result){

      /* If we have a non-closing response, we do not take action */

      if(OK_RSP <= result && OK_URC >= result){;}

      /* If we have a successful closing response, we advance to closingState
       * and depending on closingWrap we either stay in PROC or go to WRAP */

      else if(OK_CLOSE == result){
         interface->procState = closingState;
         interface->frmState = (closingWrap == TRUE ? WRAP : PROC );
      }

      /* If we have an error closing response, we advance to errorState
       * and depending on errorWrap we either stay in PROC or go to WRAP */

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

static void gsmFrmCopyGsmError (gsmInterface_t * interface)
{
   rsp_t rsp;

   /* If we had an error closing response, we get the corresponding response
    * and copy it to the errorOur vector */

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

bool gsmInitInterface(gsmInterface_t * interface)
{
   bool ok = FALSE;

   ok = gsmInitEngine(&(interface->engine)); /* Initializes the GSM engine */

   /* We initialize all the elements of the interface */

   interface->frmState = IDLE;
   interface->procState = NOCMD;
   interface->cmdSent = false;

   interface->procCnt = DELAY_PROC;
   interface->auxCnt = 0;

   interface->urcMode = MANUAL_MODE;
   interface->urcCback = NULL;

   interface->dataCback = NULL;
   interface->exitCmdList = exitCmds;

   interface->frm = NULL;
   interface->frmInput = NULL;
   interface->frmOutput = NULL;
   interface->frmCback = NULL;
   interface->errorOut.errorFrm = OK;

   SysTickInterface = interface;

   return ok;
}

/*****************************************************************************/

/* This function needs to be called from the basic ARM SysTick_Handler function
 * to handle gsmProcess calls and AT command timeouts. When called, it
 * decrements both procCnt and toutCnt. It must be mentioned that procCnt is
 * an interface-level variable, while toutCnt is an engine-level variable.
 * This is why a function is used to decrement toutCnt, as it is not in scope
 * in interface.
 */

void gsmSysTickHandler (void)
{
   if(NULL != SysTickInterface){

      /* Decrement timeout counter */
      gsmDecToutCnt(&(SysTickInterface->engine));

      /* Decrement process counter */
      if((SysTickInterface->procCnt) > 0) {
         (SysTickInterface->procCnt)--;
      }

      /* Decrement auxiliary counter */
      if((SysTickInterface->auxCnt) > 0) {
         (SysTickInterface->auxCnt)--;
      }

   }

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
 *  serial mode. The comments below detail the behavior.
 */

void gsmProcess (gsmInterface_t * interface)
{
   /* The function runs only if the process counter is down to zero, which
      means that the periodic time specified for it's execution has passed. */

   if(0 == (interface->procCnt)){

      /* No formula currently running */

      if (IDLE == interface->frmState){

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
            interface->dataCback(interface);
         }

      }

      /* There is a formula running; execute the formula */

      else {
         interface->frm(interface);
      }

      interface->procCnt = DELAY_PROC; /* reset gsmProcess counter */

      return;

   }

   else{

      return;

   }

}

/******************************************************************************/

bool gsmIsIdle (gsmInterface_t * interface)
{
   return(IDLE == interface->frmState);
}

/*****************************************************************************/

bool gsmSetUrcMode (gsmInterface_t * interface, urcMode_t mode)
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

bool gsmSetDataCback (gsmInterface_t * interface, frm_t cback)
{
   interface->dataCback = cback;
   return true;
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
                     debug(">>>interf<<< Exiting DATA MODE!");
                     gsmSetSerialMode(&(interface->engine), COMMAND_MODE);
                     return nChRet;
                  }
               }

            }

            /* If the number of chars is too large for this to be an exit cmd,
             * we reset the char count and crLf flag */

            else{
               cmdChCnt = 0; crLf = FALSE;
            }

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

bool gsmReadUrc (gsmInterface_t * interface, urc_t * urc)
{
   rsp_t temp;

   temp = gsmGetUrc(&(interface->engine));

   if('\0' != temp.cmd[0]){
      strncpy(urc->cmd, temp.cmd, strlen(temp.cmd));
      urc->cmd[strlen(temp.cmd)]='\0';
      strncpy(urc->par, temp.par, strlen(temp.par));
      urc->par[strlen(temp.par)]='\0';
      return true;
   }

   return false;
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

void gsmExitDataMode (gsmInterface_t * interface, frmCback_t cback)
{
   interface->frm = gsmExitDataModeF;
   interface->frmCback = cback;
   interface->frmState = INIT;

   SysTickInterface->auxCnt = 1000; /* initialize auxiliary counter */

   return;
}

/*****************************************************************************/

void gsmGetSigQual (gsmInterface_t * interface, sigQual_t * sigQual,
                    frmCback_t cback)
{
   interface->frm = gsmGetSigQualF;
   interface->frmOutput = sigQual;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmCheckConn (gsmInterface_t * interface, connStatus_t * status,
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

void gsmSmsSend (gsmInterface_t * interface, smsOut_t * msg, smsConf_t * conf,
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

void gsmSmsRead (gsmInterface_t * interface, smsRec_t * msg,
                 smsReadPars_t * pars, frmCback_t cback)
{
   interface->frm = gsmSmsReadF;
   interface->frmInput = pars;
   interface->frmOutput = msg;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmSmsList (gsmInterface_t * interface, smsRec_t * list,
                 smsListPars_t * pars, frmCback_t cback)
{
   interface->frm = gsmSmsListF;
   interface->frmInput = pars;
   interface->frmOutput = list;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmSmsDel (gsmInterface_t * interface, smsDelPars_t * msgdel,
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

void gsmGprsStart (gsmInterface_t * interface, apnUserPwd_t * apn,
                   frmCback_t cback)
{
   interface->frm = gsmGprsStartF;
   interface->frmInput = apn;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmGprsStop (gsmInterface_t * interface, frmCback_t cback)
{
   interface->frm = gsmGprsStopF;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}


/*****************************************************************************/

void gsmGprsOpenPort (gsmInterface_t * interface, socket_t * socket,
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
                 pwrGnss_t * cmd, frmCback_t cback)
{
   interface->frm = gsmGnssPwrF;
   interface->frmInput = cmd;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/*****************************************************************************/

void gsmGnssGetData (gsmInterface_t * interface,
                     dataGnss_t * dataGnss, frmCback_t cback)
{
   interface->frm = gsmGnssGetDataF;
   interface->frmOutput = dataGnss;
   interface->frmCback = cback;
   interface->frmState = INIT;

   return;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
