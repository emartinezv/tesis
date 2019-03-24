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

void * cb (errorUser_s, void *);

void * cbempty (errorUser_s, void *);

void * cbgsmgprs (errorUser_s, void *);

void * cbprint (errorUser_s, void *);

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
/*                             Callback functions                            */
/*---------------------------------------------------------------------------*/

void * cbempty (errorUser_s error_in, void * input)
{
   dbgPrint("Funcion cbempty ejecutada\r\n");

   if(OK != error_in.errorFrm){
      dbgPrint("Error en la ejecucion de formula: ");

      switch(error_in.errorFrm){

         case ERR_INIT:

            dbgPrint("Error en inicializacion\r\n");

            break;

         case ERR_PROC:

            dbgPrint("Error en proceso\r\n");

            break;

         case ERR_GSM:

            dbgPrint("Error del engine GSM --- ");
            dbgPrint(error_in.errorCmd.cmd);
            dbgPrint("(");
            dbgPrint(error_in.errorCmd.par);
            dbgPrint(") ---\r\n");

            break;

         case ERR_WRAP:

            dbgPrint("Error en cierre\r\n");

            break;

         default:

            dbgPrint("Error no reconocido\r\n");

            break;

      }

   }

   return 0;
}

void * cbled (errorUser_s error_in, void * input)
{
   if(OK != error_in.errorFrm){
      dbgPrint("Error en la ejecucion de formula: ");

      switch(error_in.errorFrm){

         case ERR_INIT:

            dbgPrint("Error en inicializacion\r\n");

            break;

         case ERR_PROC:

            dbgPrint("Error en proceso\r\n");

            break;

         case ERR_GSM:

            dbgPrint("Error del engine GSM --- ");
            dbgPrint(error_in.errorCmd.cmd);
            dbgPrint("(");
            dbgPrint(error_in.errorCmd.par);
            dbgPrint(") ---\r\n");

            break;

         case ERR_WRAP:

            dbgPrint("Error en cierre\r\n");

            break;

         default:

            dbgPrint("Error no reconocido\r\n");

            break;

      }

   }

   dbgPrint("Actualizando LEDs...\r\n");

   uint8_t i = 0;
   smsRec_s * target = (smsRec_s *)input;

   for(i = 0; (target+i)->meta[0] != '\0'; i++){

      if(0 != strstr((target+i)->text,"ledverdeon")){
         Board_LED_Set(LED_VERDE,1);
         dbgPrint("Enciendo LED...\r\n");
      }
      if(0 != strstr((target+i)->text,"ledverdeoff")){
         Board_LED_Set(LED_VERDE,0);
         dbgPrint("Apago LED...\r\n");
      }

   }

   return;
}

void * cbprint (errorUser_s error_in, void * input)
{
   if(OK != error_in.errorFrm){
      dbgPrint("Error en la ejecucion de formula: ");

      switch(error_in.errorFrm){

         case ERR_INIT:

            dbgPrint("Error en inicializacion\r\n");

            break;

         case ERR_PROC:

            dbgPrint("Error en proceso\r\n");

            break;

         case ERR_GSM:

            dbgPrint("Error del engine GSM --- ");
            dbgPrint(error_in.errorCmd.cmd);
            dbgPrint("(");
            dbgPrint(error_in.errorCmd.par);
            dbgPrint(") ---\r\n");

            break;

         case ERR_WRAP:

            dbgPrint("Error en cierre\r\n");

            break;

         default:

            dbgPrint("Error no reconocido\r\n");

            break;

      }

   }

   else{

      dbgPrint("Imprimiendo SMS...\r\n\r\n");

      uint8_t i = 0;
      smsRec_s * target = ((smsListRet_s *)input)->msgs;
      uint8_t noMsg = ((smsListRet_s *)input)->noMsgs;

      uint8_t auxtext[5];

      dbgPrint("Nro de mensajes: ");
      itoa(noMsg, auxtext, 10);
      dbgPrint(auxtext);
      dbgPrint("\r\n");

      for(i = 0; i < noMsg; i++){

         dbgPrint((target+i)->text);
         dbgPrint("\r\n");

      }

      dbgPrint("\r\nFin de los mensajes\r\n");

   }

   return;
}

void * cbgsmgprs (errorUser_s error_in, void * input)
{
   if(OK != error_in.errorFrm){
      dbgPrint("Error en la ejecucion de formula: ");

      switch(error_in.errorFrm){

         case ERR_INIT:

            dbgPrint("Error en inicializacion\r\n");

            break;

         case ERR_PROC:

            dbgPrint("Error en proceso\r\n");

            break;

         case ERR_GSM:

            dbgPrint("Error del engine GSM --- ");
            dbgPrint(error_in.errorCmd.cmd);
            dbgPrint("(");
            dbgPrint(error_in.errorCmd.par);
            dbgPrint(") ---\r\n");

            break;

         case ERR_WRAP:

            dbgPrint("Error en cierre\r\n");

            break;

         default:

            dbgPrint("Error no reconocido\r\n");

            break;

      }

   }

   else{

      if(true == ((connStatus_s *)input)->gsm){
         dbgPrint("\r\n Conectado a red GSM\r\n");
      }
      else{
         dbgPrint("\r\n No conectado a red GSM\r\n");
      }

      if(true == ((connStatus_s *)input)->gprs){
         dbgPrint("\r\n Conectado a servicio GPRS\r\n");
      }
      else{
         dbgPrint("\r\n No conectado a servicio GPRS\r\n");
      }

   }

   return;
}

void cbUrc (uint8_t const * const cmd, uint8_t const * const par)
{
   dbgPrint("\r\nURC received!\r\n");
   dbgPrint("CMD: ");
   dbgPrint(cmd);
   dbgPrint(" PAR: ");
   dbgPrint(par);
   dbgPrint("\r\n\r\n");

   return;
}

void cbData (void)
{
   uint8_t usbReadBuf[21];
   uint8_t serialReadBuf[21];

   uint8_t nRead = 20;

   uint8_t n;

   /* Read USB UART and store it */

   n = uartRecv(CIAA_UART_USB, usbReadBuf, 20);
   usbReadBuf[n]='\0';

   /* Write USB UART data to serial port and read from serial port */

   gsmWriteReadDataMode (usbReadBuf, &n, serialReadBuf, &nRead);

   serialReadBuf[nRead]='\0';

   /* Write data read from serial port to USB UART */

   n = uartSend(CIAA_UART_USB, serialReadBuf, nRead);

   return;
}

/*---------------------------------------------------------------------------*/
/*                         Testing console functions                         */
/*---------------------------------------------------------------------------*/

void console_sms (void)
{
   uint8_t instruction = 0;

   smsOut_s msg = {"1151751809","Hola mundo!"};
   smsConf_s conf;
   smsRec_s msgList[SMS_READ_SIZ];
   smsDelPars_s msgDel = {1, DEL_ALL};
   smsRec_s recMsg;
   smsReadPars_s parRead = {1, NOCHANGE};
   smsListPars_s parList = {ALL_MSG, NOCHANGE, SMS_READ_SIZ};

   while ('S' != instruction){

      if(gsmIsIdle()){

         dbgPrint("\r\n\r\n>>> CONSOLA SMS <<< \r\n\r\n");

         dbgPrint("1) Mandar SMS \r\n");
         dbgPrint("2) Leer todos los SMSs \r\n");
         dbgPrint("3) Borrar todos los SMSs \r\n");
         dbgPrint("4) Leer el primer SMS \r\n\r\n");

         dbgPrint("S) Salir a la consola principal \r\n");

         while(0 == uartRecv(CIAA_UART_USB, &instruction, 1)){gsmProcess();}

         switch (instruction) {

            case '1':

            gsmSmsSend(&msg, &conf, cbempty);

            break;

            case '2':

            gsmSmsList(&msgList[0], &parList, cbprint);

            break;

            case '3':

            gsmSmsDel(&msgDel, cbempty);

            break;

            case '4':

            gsmSmsRead(&recMsg, &parRead, cbprint);

            break;

            case 'S':

            break;

            default:

            dbgPrint("INSTRUCCION DESCONOCIDA \r\n\r\n");
            break;

         }

         while(!gsmIsIdle()){
            gsmProcess();
         }

      }

   }

   return;

}

void console_gprs (void)
{
   uint8_t instruction = 0;

   apnUserPwd_s APN = {"datos.personal.com","datos","datos"};
   port_s port1 = {TCP, "104.236.225.217",2399};
   port_s port2 = {UDP, "104.236.225.217",2399};

   while ('S' != instruction){

      if(gsmIsIdle()){

         dbgPrint("\r\n\r\n>>> CONSOLA GPRS <<< \r\n\r\n");

         dbgPrint("1) Prender GPRS \r\n");
         dbgPrint("2) Abrir puerto TCP \r\n");
         dbgPrint("3) Abrir puerto UDP \r\n");
         dbgPrint("4) Cerrar puerto TCP o UDP \r\n\r\n");

         dbgPrint("S) Salir a la consola principal \r\n");

         while(0 == uartRecv(CIAA_UART_USB, &instruction, 1)){gsmProcess();}

         switch (instruction) {

            case '1':

            gsmGprsStart(&APN, cbempty);

            break;

            case '2':

            gsmGprsOpenPort(&port1, cbempty);

            break;

            case '3':

            gsmGprsOpenPort(&port2, cbempty);

            break;

            case '4':

            gsmGprsClosePort(cbempty);

            break;

            case 'S':

            break;

            default:

            dbgPrint("INSTRUCCION DESCONOCIDA \r\n\r\n");
            break;

         }

         while(!gsmIsIdle()){
            gsmProcess();
         }

         while(DATA_MODE == gsmGetSerialMode()){

            uint8_t data_char;

            if(0 != uartRecv(CIAA_UART_USB, &data_char, 1)){
               uartSend(CIAA_UART_232, &data_char, 1); /* mando lo que escribo a 232 */
               uartSend(CIAA_UART_USB, &data_char, 1); /* eco */
            }

            if(0 != uartRecv(CIAA_UART_232, &data_char, 1)){
               uartSend(CIAA_UART_USB, &data_char, 1); /* mando lo recibido a terminal */
            }

         }

      }

   }

   return;
}

void console_gnss (void)
{
   uint8_t instruction = 0;

   dataGnss_s navInfo;
   pwrGnss_e powerGNSS;

   while ('S' != instruction){

      if(gsmIsIdle()){

         dbgPrint("\r\n\r\n>>> CONSOLA GNSS <<< \r\n\r\n");

         dbgPrint("1) Prender GNSS \r\n");
         dbgPrint("2) Apagar GNSS \r\n");
         dbgPrint("3) Obtener informacion de navegacion GNSS \r\n\r\n");

         dbgPrint("S) Salir a la consola principal \r\n");

         while(0 == uartRecv(CIAA_UART_USB, &instruction, 1)){gsmProcess();}

         switch (instruction) {

            case '1':

            powerGNSS = ON;

            gsmGnssPwr(&powerGNSS, cbempty);

            break;

            case '2':

            powerGNSS = OFF;

            gsmGnssPwr(&powerGNSS, cbempty);

            break;

            case '3':

            gsmGnssGetData(&navInfo, cbempty);

            break;

            case 'S':

            break;

            default:

            dbgPrint("INSTRUCCION DESCONOCIDA \r\n\r\n");
            break;

         }

         while(!gsmIsIdle()){
            gsmProcess();
         }

      }

   }

   return;
}

void console_urc (void)
{
   uint8_t instruction = 0;

   rsp_t urc;

   while ('S' != instruction){

      if(gsmIsIdle()){

         dbgPrint("\r\n\r\n>>> CONSOLA URC <<< \r\n\r\n");

         dbgPrint("1) Leer URC mas reciente\r\n");
         dbgPrint("2) Poner URC handling en modo callback\r\n");
         dbgPrint("3) Poner URC handling en modo manual\r\n\r\n");

         dbgPrint("S) Salir a la consola principal \r\n");

         while(0 == uartRecv(CIAA_UART_USB, &instruction, 1)){gsmProcess();}

         switch (instruction) {

            case '1':

            urc = gsmGetUrc();

            if(urc.cmd[0] != '\0'){
               dbgPrint("\r\nURC: ");
               dbgPrint(urc.cmd);
               dbgPrint("(");
               dbgPrint(urc.par);
               dbgPrint(")\r\n");
            }
            else{
               dbgPrint("\r\nNo hay URCs pendientes\r\n");
            }

            break;

            case '2':

            gsmSetUrcMode(CBACK_MODE);

            break;

            case '3':

            gsmSetUrcMode(MANUAL_MODE);

            break;

            case 'S':

            break;

            default:

            dbgPrint("INSTRUCCION DESCONOCIDA \r\n\r\n");
            break;

         }

         while(!gsmIsIdle()){
            gsmProcess();
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

   uint8_t instruction;

   sigQual_s sigqual;
   connStatus_s status;

   gsmSetUrcCback(cbUrc);
   gsmSetDataCback(cbData);

   pausems(DELAY_INIT);

   dbgPrint("\r\n>>> INICIALIZANDO MODEM CELULAR <<< \r\n\r\n");

   gsmStartUp(cbempty);

   while(!gsmIsIdle()){
      gsmProcess();
   }

   while (1){

      if(gsmIsIdle()){

         dbgPrint("\r\n>>> CONSOLA PRINCIPAL <<< \r\n\r\n");

         dbgPrint("1) CONSOLA SMS \r\n");
         dbgPrint("2) CONSOLA GPRS \r\n");
         dbgPrint("3) CONSOLA GNSS \r\n");
         dbgPrint("4) CONSOLA URC \r\n\r\n");

         dbgPrint("5) Ver calidad de señal \r\n");
         dbgPrint("6) Estado red GSM y GPRS \r\n\r\n");

         while(0 == uartRecv(CIAA_UART_USB, &instruction, 1)){;}

         switch (instruction) {

            case '1':

            console_sms();

            break;

            case '2':

            console_gprs();

            break;

            case '3':

            console_gnss();

            break;

            case '4':

            console_urc();

            break;

            case '5':

            gsmGetSigQual (&sigqual, cbempty);

            break;

            case '6':

            gsmCheckConn(&status, cbgsmgprs);

            break;

            default:

            dbgPrint("INSTRUCCION DESCONOCIDA \r\n\r\n");

            break;

         }

         while(!gsmIsIdle()){
            gsmProcess();
         }

      }

      gsmProcess();

   }

   return 0;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
