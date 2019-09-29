/* Copyright 2016, Ezequiel Martinez Vazquez
 *
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

/*==================[inclusions]=============================================*/

#include "main.h"

/*==================[macros and definitions]=================================*/

/*==================[global data]============================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 * @return none
 */
static void initHardware(void);

/** @brief delay function
 * @param t desired milliseconds to wait
 */
static void pausems(uint32_t t);

void * cb (errorUser_t, void *);

void * cbEmpty (errorUser_t, void *);

void * cbgsmgprs (errorUser_t, void *);

void * cbprint (errorUser_t, void *);

/*==================[internal data definition]===============================*/

/* TIMING COUNTERS */

/** @brief used for delay counter */
static uint32_t pausemsCount = DELAY_INIT;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*---------------------------------------------------------------------------*/
/*                Hardware initialization and system functions               */
/*---------------------------------------------------------------------------*/

static void initHardware(void)
{
   Board_Init();
   SystemCoreClockUpdate();
   SysTick_Config(SystemCoreClock / 1000);
}

static void pausems(uint32_t t)
{
   pausemsCount = t;
   while (pausemsCount != 0) {
      __WFI();
   }
}

/*---------------------------------------------------------------------------*/
/*                            Auxiliary functions                            */
/*---------------------------------------------------------------------------*/

static bool errorCheck (errorUser_t * error)
{
   if(OK != error->errorFrm){

      switch(error->errorFrm){

         case ERR_INIT:

            dbgPrint("\r\nINIT Error\r\n");

            break;

         case ERR_PROC:

            dbgPrint("\r\nPROC Error\r\n");

            break;

         case ERR_GSM:

            dbgPrint("\r\nGPRS modem Error: ");
            dbgPrint(error->errorCmd.cmd);
            dbgPrint("(");
            dbgPrint(error->errorCmd.par);
            dbgPrint(")\r\n");

            break;

         case ERR_WRAP:

            dbgPrint("\r\nWRAP Error\r\n");

            break;

         default:

            dbgPrint("\r\nEUnknown Error\r\n");

            break;

      }

   return true;

   }

   return false;
}

/*---------------------------------------------------------------------------*/
/*                             Callback functions                            */
/*---------------------------------------------------------------------------*/

void * cbEmpty (errorUser_t error_in, void * input)
{
   dbgPrint("\r\ncbEmpty\r\n");

   errorCheck(&error_in);

   dbgPrint("\r\nFunción ");
   dbgPrint((char *)input);
   dbgPrint(" ejecutada\r\n");

   return 0;
}

void * cbLed (errorUser_t error_in, void * input)
{
   dbgPrint("\r\ncbLed\r\n");

   if(false == errorCheck(&error_in)){

      dbgPrint("Updating LEDs...\r\n");

      uint8_t i = 0;
      smsRec_t * target = (smsRec_t *)input;

      for(i = 0; (target+i)->meta[0] != '\0'; i++){

         if(0 != strstr((target+i)->text,"greenledon")){
            Board_LED_Set(LED_VERDE,1);
            dbgPrint("Turning on LED...\r\n");
         }
         if(0 != strstr((target+i)->text,"greenledoff")){
            Board_LED_Set(LED_VERDE,0);
            dbgPrint("Turning off LED...\r\n");
         }

      }

   }

   return;
}

void * cbSmsSend (errorUser_t error_in, void * input)
{
   dbgPrint("\r\ncbSmsSend\r\n");

   if(false == errorCheck(&error_in)){

      uint8_t auxStr[4]; /* used to print out sent SMS confirmation number */

      dbgPrint("\r\nSMS sent. MR value ");
      itoa(((smsConf_t *)input)->mr, auxStr, 10);
      dbgPrint(auxStr);
      dbgPrint("\r\n");

   }

   return;
}

void * cbSmsPrint (errorUser_t error_in, void * input)
{
   dbgPrint("\r\ncbSmsPrint\r\n");

   if(false == errorCheck(&error_in)){

      dbgPrint("\r\nPrinting SMSs...\r\n");

      uint8_t i = 0;
      smsRec_t * target = ((smsListRet_t *)input)->msgs;
      uint8_t noMsg = ((smsListRet_t *)input)->noMsgs;

      uint8_t auxtext[5];

      dbgPrint("\r\nNo. of messages: ");
      itoa(noMsg, auxtext, 10);
      dbgPrint(auxtext);
      dbgPrint("\r\n\r\n");

      for(i = 0; i < noMsg; i++){

         dbgPrint((target+i)->text);
         dbgPrint("\r\n\r\n");

      }

   }

   return;
}

void * cbSmsDel (errorUser_t error_in, void * input)
{
   dbgPrint("\r\ncbSmsDel\r\n");

   if(false == errorCheck(&error_in)){

      switch(((smsDelPars_t *)input)->mode){

         case INDEX:

            ;
            uint8_t auxStr[4]; /* used to print out deleted SMS index */

            dbgPrint("\r\nSMS index no. ");
            itoa(((smsDelPars_t *)input)->idx, auxStr, 10);
            dbgPrint(auxStr);
            dbgPrint(" deleted\r\n");

            break;

         case READ:

            dbgPrint("\r\nAll read SMSs deleted\r\n");

            break;

         case READ_SENT:

            dbgPrint("\r\nAll read SMSs and sent SMSs deleted\r\n");

            break;

         case READ_SENT_UNSENT:

            dbgPrint("\r\nAll read SMSs, sent and unsent SMSs deleted\r\n");

            break;

         case DEL_ALL:

            dbgPrint("\r\nAll SMSs deleted\r\n");

            break;

         default:

            break;

      }

   }

   return;
}

void * cbGprsOpenPort (errorUser_t error_in, void * input)
{
   dbgPrint("\r\ncbGprsOpenPort\r\n");

   if(false == errorCheck(&error_in)){

      dbgPrint("\r\nConnection established through ");

      if(TCP == ((socket_t *)input)->type){
         dbgPrint("TCP ");
      }
      else{
         dbgPrint("UDP ");
      }

      dbgPrint("protocol to IP ");
      dbgPrint(((socket_t *)input)->address);
      dbgPrint(" at port ");

      uint8_t auxStr[6]; /* used to print out port number */

      itoa(((socket_t *)input)->port, auxStr, 10);
      dbgPrint(auxStr);
      dbgPrint("\r\n");

   }

   return;
}

void cbUrc (uint8_t const * const cmd, uint8_t const * const par)
{
   dbgPrint("\r\ncbUrc\r\n");

   dbgPrint("\r\nURC received\r\n");
   dbgPrint("\r\nCMD: ");
   dbgPrint(cmd);
   dbgPrint("\r\n");
   dbgPrint("\r\nPAR: ");
   dbgPrint(par);
   dbgPrint("\r\n");

   return;
}

void cbData (gsmInterface_t * interface)
{
   uint8_t usbReadBuf[21];
   uint8_t serialReadBuf[21];

   uint8_t nRead = 20;

   uint8_t n;
   static uint8_t nCalls = 0;

   nCalls++;

   /* Read USB UART and store it */

   n = uartRecv(CIAA_UART_USB, usbReadBuf, 20);
   usbReadBuf[n]='\0';

   /* Echo USB UART to screen */

   n = uartSend(CIAA_UART_USB, usbReadBuf, n);

   /* If we type an uppercase X, call the gsmExitDataMode function */

   if(strstr(usbReadBuf,"X")){
      gsmExitDataMode(interface, cbEmpty);
   }

   /* Write USB UART data to serial port and read from serial port */

   n = gsm232UartSend (usbReadBuf, n);
   nRead = gsm232UartRecv (serialReadBuf, nRead);

   serialReadBuf[nRead]='\0';

   n = gsmCheckDataMode(interface, &serialReadBuf[0], &nRead);

   /* Write data read from serial port to USB UART */

   uartSend(CIAA_UART_USB, serialReadBuf, n);

   return;
}

void * cbGnssOnOff (errorUser_t error_in, void * input)
{
   dbgPrint("\r\ncbGnssOnOff\r\n");

   if(false == errorCheck(&error_in)){

      if(ON == *(pwrGnss_t *)input){

         dbgPrint("\r\nGPS/GNSS on\r\n");

      }

      else{

         dbgPrint("\r\nGPS/GNSS off\r\n");

      }

   }

   return;
}

void * cbGnssData (errorUser_t error_in, void * input)
{
   dbgPrint("\r\ncbGnssData\r\n");

   if(false == errorCheck(&error_in)){

      dbgPrint("\r\nNAVINFO: ");
      dbgPrint(((dataGnss_t *)input)->data);
      dbgPrint("\r\n");

   }

   return;
}

void * cbSigQual (errorUser_t error_in, void * input)
{
   dbgPrint("\r\ncbSigQual\r\n");

   if(false == errorCheck(&error_in)){

      uint8_t auxStr[4]; /* used to print out RSSI and BER */

      dbgPrint("\r\nRSSI: ");
      itoa(((sigQual_t *)input)->rssi, auxStr, 10);
      dbgPrint(auxStr);
      dbgPrint("\r\n");
      dbgPrint("\r\nBER: ");
      itoa(((sigQual_t *)input)->ber, auxStr, 10);
      dbgPrint(auxStr);
      dbgPrint("\r\n");

   }

   return 0;
}

void * cbCheckConn (errorUser_t error_in, void * input)
{
   dbgPrint("\r\ncbCheckConn\r\n");

   if(false == errorCheck(&error_in)){

      if(true == ((connStatus_t *)input)->gsm){
         dbgPrint("\r\nConnected to GSM network\r\n");
      }
      else{
         dbgPrint("\r\nNot connected to GSM network\r\n");
      }

      if(true == ((connStatus_t *)input)->gprs){
         dbgPrint("\r\nConnected to GPRS service\r\n");
      }
      else{
         dbgPrint("\r\nNot connected to GPRS service\r\n");
      }

   }

   return;
}

/*---------------------------------------------------------------------------*/
/*                         Testing console functions                         */
/*---------------------------------------------------------------------------*/

void console_sms (gsmInterface_t * interface)
{
   uint8_t instruction = 0;

   smsOut_t msg = {"1151751809","Hello world!"};
   smsConf_t conf;
   smsRec_t msgList[SMS_READ_SIZ];
   smsDelPars_t msgDel = {1, DEL_ALL};
   smsRec_t recMsg;
   smsReadPars_t parRead = {1, NOCHANGE};
   smsListPars_t parList = {ALL_MSG, NOCHANGE, SMS_READ_SIZ};

   while ('S' != instruction){

      if(gsmIsIdle(interface)){

         dbgPrint("\r\n\r\n>>> SMS CONSOLE <<< \r\n\r\n");

         dbgPrint("1) Send SMS \r\n");
         dbgPrint("2) Read all SMSs \r\n");
         dbgPrint("3) Erase all SMSs \r\n");
         dbgPrint("4) Read oldest SMS \r\n\r\n");

         dbgPrint("S) Return to main console \r\n");

         while(0 == uartRecv(CIAA_UART_USB, &instruction, 1)){
            gsmProcess(interface);
         }

         dbgPrint("\r\n");
         uartSend(CIAA_UART_USB, &instruction, 1);
         dbgPrint("\r\n");

         switch (instruction) {

            case '1':

            gsmSmsSend(interface, &msg, &conf, cbSmsSend);

            break;

            case '2':

            gsmSmsList(interface, &msgList[0], &parList, cbSmsPrint);

            break;

            case '3':

            gsmSmsDel(interface, &msgDel, cbSmsDel);

            break;

            case '4':

            gsmSmsRead(interface, &recMsg, &parRead, cbSmsPrint);

            break;

            case 'S':

            break;

            default:

            dbgPrint("\r\nUnknown instruction \r\n\r\n");
            break;

         }

         while(!gsmIsIdle(interface)){
            gsmProcess(interface);
         }

      }

   }

   return;

}

void console_gprs (gsmInterface_t * interface)
{
   uint8_t instruction = 0;

   apnUserPwd_t APN = {"datos.personal.com","datos","datos"};
   socket_t port1 = {TCP, "104.236.225.217",2399};
   socket_t port2 = {UDP, "104.236.225.217",2399};

   while ('S' != instruction){

      if(gsmIsIdle(interface)){

         dbgPrint("\r\n\r\n>>> GPRS CONSOLE <<< \r\n\r\n");

         dbgPrint("1) Turn on GPRS \r\n");
         dbgPrint("2) Open TCP port \r\n");
         dbgPrint("3) Open UDP port \r\n");
         dbgPrint("4) Close UDP or TCP port \r\n");
         dbgPrint("5) Turn off GPRS \r\n\r\n");

         dbgPrint("S) Return to main console \r\n");

         while(0 == uartRecv(CIAA_UART_USB, &instruction, 1)){
            gsmProcess(interface);
         }

         dbgPrint("\r\n");
         uartSend(CIAA_UART_USB, &instruction, 1);
         dbgPrint("\r\n");

         switch (instruction) {

            case '1':

            gsmGprsStart(interface, &APN, cbEmpty);

            break;

            case '2':

            gsmGprsOpenPort(interface, &port1, cbGprsOpenPort);

            break;

            case '3':

            gsmGprsOpenPort(interface, &port2, cbGprsOpenPort);

            break;

            case '4':

            gsmGprsClosePort(interface, cbEmpty);

            break;

            case '5':

            gsmGprsStop(interface, cbEmpty);

            break;

            case 'S':

            break;

            default:

            dbgPrint("\r\nUnknown instruction \r\n\r\n");
            break;

         }

         while(!gsmIsIdle(interface)){
            gsmProcess(interface);
         }

         while(DATA_MODE == gsmGetSerialMode(&interface->engine)){
            gsmProcess(interface);
         }

      }

      else{
         gsmProcess(interface);
      }

   }

   return;
}

void console_gnss (gsmInterface_t * interface)
{
   uint8_t instruction = 0;

   dataGnss_t navInfo;
   pwrGnss_t powerGNSS;

   while ('S' != instruction){

      if(gsmIsIdle(interface)){

         dbgPrint("\r\n\r\n>>> GPS/GNSS CONSOLE <<< \r\n\r\n");

         dbgPrint("1) Turn on GPS/GNSS \r\n");
         dbgPrint("2) Turn off GPS/GNSS \r\n");
         dbgPrint("3) Get GPS/GNSS navigation information \r\n\r\n");

         dbgPrint("S) Return to main console\r\n");

         while(0 == uartRecv(CIAA_UART_USB, &instruction, 1)){
            gsmProcess(interface);
         }

         dbgPrint("\r\n");
         uartSend(CIAA_UART_USB, &instruction, 1);
         dbgPrint("\r\n");

         switch (instruction) {

            case '1':

            powerGNSS = ON;

            gsmGnssPwr(interface, &powerGNSS, cbGnssOnOff);

            break;

            case '2':

            powerGNSS = OFF;

            gsmGnssPwr(interface, &powerGNSS, cbGnssOnOff);

            break;

            case '3':

            gsmGnssGetData(interface, &navInfo, cbGnssData);

            break;

            case 'S':

            break;

            default:

            dbgPrint("\r\nUnknown instruction \r\n\r\n");
            break;

         }

         while(!gsmIsIdle(interface)){
            gsmProcess(interface);
         }

      }

   }

   return;
}

void console_urc (gsmInterface_t * interface)
{
   uint8_t instruction = 0;

   urc_t urc = {"\0","\0"};

   while ('S' != instruction){

      if(gsmIsIdle(interface)){

         dbgPrint("\r\n\r\n>>> URC CONSOLE <<< \r\n\r\n");

         dbgPrint("1) Read oldest URC\r\n");
         dbgPrint("2) Set URC handling mode to callback\r\n");
         dbgPrint("3) Set URC handling mode to manual\r\n\r\n");

         dbgPrint("S) Return to main console \r\n");

         while(0 == uartRecv(CIAA_UART_USB, &instruction, 1)){
            gsmProcess(interface);
         }

         dbgPrint("\r\n");
         uartSend(CIAA_UART_USB, &instruction, 1);
         dbgPrint("\r\n");

         switch (instruction) {

            case '1':

            if (gsmReadUrc(interface, &urc)){

               dbgPrint("\r\nURC: ");
               dbgPrint(urc.cmd);
               dbgPrint("(");
               dbgPrint(urc.par);
               dbgPrint(")\r\n");

            }

            else{
               dbgPrint("\r\nNo pending URCs\r\n");
            }

            break;

            case '2':

            gsmSetUrcMode(interface, CBACK_MODE);

            break;

            case '3':

            gsmSetUrcMode(interface, MANUAL_MODE);

            break;

            case 'S':

            break;

            default:

            dbgPrint("\r\nUnknown instruction \r\n\r\n");
            break;

         }

         while(!gsmIsIdle(interface)){
            gsmProcess(interface);
         }

      }

   }

   return;
}

/*==================[external functions definition]==========================*/

void SysTick_Handler(void)
{
   if(pausemsCount > 0) pausemsCount--;

   gsmSysTickHandler();
}

int main(void)
{
   initHardware();
   ciaaUARTInit();

   pausems(DELAY_INIT);

   uint8_t instruction;

   sigQual_t sigqual;
   connStatus_t status;

   gsmInterface_t interface;
   gsmInitInterface(&interface); /* Initializes the GSM interface */

   gsmSetUrcCback(&interface, cbUrc);
   gsmSetDataCback(&interface, cbData);

   gsmStartUp(&interface, cbEmpty);

   while(!gsmIsIdle(&interface)){
      gsmProcess(&interface);
   }

   dbgPrint("\r\n>>> INITIALIZING... <<< \r\n\r\n");

   while (1){

      if(gsmIsIdle(&interface)){

         dbgPrint("\r\n>>> MAIN CONSOLE <<< \r\n\r\n");

         dbgPrint("1) SMS CONSOLE \r\n");
         dbgPrint("2) GPRS CONSOLE \r\n");
         dbgPrint("3) GPS/GNSS CONSOLE \r\n");
         dbgPrint("4) URC CONSOLE \r\n\r\n");

         dbgPrint("5) Get signal quality levels\r\n");
         dbgPrint("6) Get GSM & GPRS network status\r\n\r\n");

         while(0 == uartRecv(CIAA_UART_USB, &instruction, 1)){;}

         dbgPrint("\r\n");
         uartSend(CIAA_UART_USB, &instruction, 1);
         dbgPrint("\r\n");

         switch (instruction) {

            case '1':

            console_sms(&interface);

            break;

            case '2':

            console_gprs(&interface);

            break;

            case '3':

            console_gnss(&interface);

            break;

            case '4':

            console_urc(&interface);

            break;

            case '5':

            gsmGetSigQual(&interface, &sigqual, cbSigQual);

            break;

            case '6':

            gsmCheckConn(&interface, &status, cbCheckConn);

            break;

            default:

            dbgPrint("\r\nUnknown instruction \r\n\r\n");
            break;

         }

         while(!gsmIsIdle(&interface)){
            gsmProcess(&interface);
         }

      }

      gsmProcess(&interface);

   }

   return 0;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
