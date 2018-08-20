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

/* DEBUG DEFINES */

//#define DEBUG_TKNZER
//#define DEBUG_PARSER
//#define DEBUG_ENGINE
//#define DEBUG_INTERF

/*==================[macros and definitions]=================================*/

/*==================[global data]============================================*/

/*==================[internal data declaration]==============================*/

static uint8_t deleteall = 0;

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 * @return none
 */
static void initHardware(void);

/** @brief delay function
 * @param t desired milliseconds to wait
 */
static void pausems(uint32_t t);

void * cb (error_user, void *);

void * cbempty (error_user, void *);

void * cbgsmgprs (error_user, void *);

void * cbprint (error_user, void *);

/*==================[internal data definition]===============================*/

/* TIMING COUNTERS */

/** @brief used for delay counter */
static uint32_t pausems_count = DELAY_INIT;

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
   pausems_count = t;
   while (pausems_count != 0) {
      __WFI();
   }
}

/*---------------------------------------------------------------------------*/
/*                             Callback functions                            */
/*---------------------------------------------------------------------------*/

void * cbempty (error_user error_in, void * input)
{
   dbgPrint("Funcion cbempty ejecutada\r\n");

   if(OK != error_in.error_formula){
      dbgPrint("Error en la ejecucion de formula: ");

      switch(error_in.error_formula){

         case ERR_INIT:

            dbgPrint("Error en inicializacion\r\n");

            break;

         case ERR_PROC:

            dbgPrint("Error en proceso\r\n");

            break;

         case ERR_GSM:

            dbgPrint("Error del engine GSM --- ");
            dbgPrint(error_in.error_command.command);
            dbgPrint("(");
            dbgPrint(error_in.error_command.parameter);
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

void * cbled (error_user error_in, void * input)
{
   if(OK != error_in.error_formula){
      dbgPrint("Error en la ejecucion de formula: ");

      switch(error_in.error_formula){

         case ERR_INIT:

            dbgPrint("Error en inicializacion\r\n");

            break;

         case ERR_PROC:

            dbgPrint("Error en proceso\r\n");

            break;

         case ERR_GSM:

            dbgPrint("Error del engine GSM --- ");
            dbgPrint(error_in.error_command.command);
            dbgPrint("(");
            dbgPrint(error_in.error_command.parameter);
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
   SMS_rec * target = (SMS_rec *)input;

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

void * cbprint (error_user error_in, void * input)
{
   if(OK != error_in.error_formula){
      dbgPrint("Error en la ejecucion de formula: ");

      switch(error_in.error_formula){

         case ERR_INIT:

            dbgPrint("Error en inicializacion\r\n");

            break;

         case ERR_PROC:

            dbgPrint("Error en proceso\r\n");

            break;

         case ERR_GSM:

            dbgPrint("Error del engine GSM --- ");
            dbgPrint(error_in.error_command.command);
            dbgPrint("(");
            dbgPrint(error_in.error_command.parameter);
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

   dbgPrint("Imprimiendo SMS...\r\n\r\n");

   if(OK != error_in.error_formula){
      dbgPrint("Error en la lectura de SMS!\r\n");
      dbgPrint(error_in.error_command.command);
      dbgPrint("(");
      dbgPrint(error_in.error_command.parameter);
      dbgPrint(")\r\n");
   }

   else{

      uint8_t i = 0;
      SMS_rec * target = ((SMS_print *)input)->firstMsg;
      uint8_t noMsg = ((SMS_print *)input)->noMsg;

      for(i = 0; i < noMsg; i++){

         dbgPrint((target+i)->text);
         dbgPrint("\r\n");

         if(0 != strstr((target+i)->text,"borrar")){
            deleteall = 1;
         }

      }

      dbgPrint("\r\nFin de los mensajes\r\n");

   }

   return;
}

void * cbgsmgprs (error_user error_in, void * input)
{
   if(OK != error_in.error_formula){
      dbgPrint("Error en la ejecucion de formula: ");

      switch(error_in.error_formula){

         case ERR_INIT:

            dbgPrint("Error en inicializacion\r\n");

            break;

         case ERR_PROC:

            dbgPrint("Error en proceso\r\n");

            break;

         case ERR_GSM:

            dbgPrint("Error del engine GSM --- ");
            dbgPrint(error_in.error_command.command);
            dbgPrint("(");
            dbgPrint(error_in.error_command.parameter);
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

      if(true == ((statusGSMGPRS_s *)input)->gsm){
         dbgPrint("\r\n Conectado a red GSM\r\n");
      }
      else{
         dbgPrint("\r\n No conectado a red GSM\r\n");
      }

      if(true == ((statusGSMGPRS_s *)input)->gprs){
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
   dbgPrint("PAR: ");
   dbgPrint(par);
   dbgPrint("\r\n\r\n");

   return;
}

/*---------------------------------------------------------------------------*/
/*                         Testing console functions                         */
/*---------------------------------------------------------------------------*/

void console_sms (void)
{
   uint8_t instruction = 0;

   SMS_send msg = {"1151751809","Hola mundo!"};
   SMS_rec msgList[SMS_READ_SIZ];
   SMS_del msgDel = {1, 4};
   SMS_rec recMsg;
   SMS_rd_params params = {1, NOCHANGE};

   while ('S' != instruction){

      if(ciaaMobile_isIdle()){

         dbgPrint("\r\n\r\n>>> CONSOLA SMS <<< \r\n\r\n");

         dbgPrint("1) Mandar SMS \r\n");
         dbgPrint("2) Leer todos los SMSs \r\n");
         dbgPrint("3) Borrar todos los SMSs \r\n");
         dbgPrint("4) Leer el primer SMS \r\n\r\n");

         dbgPrint("S) Salir a la consola principal \r\n");

         while(0 == uartRecv(CIAA_UART_USB, &instruction, 1)){;}

         switch (instruction) {

            case '1':

            ciaaMobile_sendSMS(&msg, cbempty);

            break;

            case '2':

            ciaaMobile_listRecSMS(&msgList[0], SMS_READ_SIZ, cbprint);

            break;

            case '3':

            ciaaMobile_delSMS(&msgDel, cbempty);

            break;

            case '4':

            ciaaMobile_readRecSMS(&recMsg, &params, cbprint);

            break;

            case 'S':

            break;

            default:

            dbgPrint("INSTRUCCION DESCONOCIDA \r\n\r\n");
            break;

         }

         while(!ciaaMobile_isIdle()){
            ciaaMobile_sysUpdate();
         }

      }

      ciaaMobile_sysUpdate();

   }

   return;

}

void console_gprs (void)
{
   uint8_t instruction = 0;

   APN_usr_pwd APN = {"datos.personal.com","datos","datos"};
   port_s port1 = {TCP, "104.236.225.217","2399"};
   port_s port2 = {UDP, "104.236.225.217","2399"};

   while ('S' != instruction){

      if(ciaaMobile_isIdle()){

         dbgPrint("\r\n\r\n>>> CONSOLA GPRS <<< \r\n\r\n");

         dbgPrint("1) Prender GPRS \r\n");
         dbgPrint("2) Abrir puerto TCP \r\n");
         dbgPrint("3) Abrir puerto UDP \r\n");
         dbgPrint("4) Cerrar puerto TCP o UDP \r\n\r\n");

         dbgPrint("S) Salir a la consola principal \r\n");

         while(0 == uartRecv(CIAA_UART_USB, &instruction, 1)){;}

         switch (instruction) {

            case '1':

            ciaaMobile_startGPRS(&APN, cbempty);

            break;

            case '2':

            ciaaMobile_openPort(&port1, cbempty);

            break;

            case '3':

            ciaaMobile_openPort(&port2, cbempty);

            break;

            case '4':

            ciaaMobile_closePort(cbempty);

            break;

            case 'S':

            break;

            default:

            dbgPrint("INSTRUCCION DESCONOCIDA \r\n\r\n");
            break;

         }

         while(!ciaaMobile_isIdle()){
            ciaaMobile_sysUpdate();
         }

         while(DATA_MODE == checkSerialMode()){

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

      ciaaMobile_sysUpdate();

   }

   return;
}

void console_gnss (void)
{
   uint8_t instruction = 0;

   uint8_t navInfo[95];
   power_GNSS_e powerGNSS;

   while ('S' != instruction){

      if(ciaaMobile_isIdle()){

         dbgPrint("\r\n\r\n>>> CONSOLA GNSS <<< \r\n\r\n");

         dbgPrint("1) Prender GNSS \r\n");
         dbgPrint("2) Apagar GNSS \r\n");
         dbgPrint("3) Obtener informacion de navegacion GNSS \r\n\r\n");

         dbgPrint("S) Salir a la consola principal \r\n");

         while(0 == uartRecv(CIAA_UART_USB, &instruction, 1)){;}

         switch (instruction) {

            case '1':

            powerGNSS = ON;

            ciaaMobile_powerGNSS(&powerGNSS, cbempty);

            break;

            case '2':

            powerGNSS = OFF;

            ciaaMobile_powerGNSS(&powerGNSS, cbempty);

            break;

            case '3':

            ciaaMobile_getGNSSNavInfo(navInfo, cbempty);

            break;

            case 'S':

            break;

            default:

            dbgPrint("INSTRUCCION DESCONOCIDA \r\n\r\n");
            break;

         }

         while(!ciaaMobile_isIdle()){
            ciaaMobile_sysUpdate();
         }

      }

      ciaaMobile_sysUpdate();

   }

   return;
}

void console_urc (void)
{
   uint8_t instruction = 0;

   ATresp urc;

   while ('S' != instruction){

      if(ciaaMobile_isIdle()){

         dbgPrint("\r\n\r\n>>> CONSOLA URC <<< \r\n\r\n");

         dbgPrint("1) Leer URC mas reciente\r\n");
         dbgPrint("2) Poner URC handling en modo callback\r\n");
         dbgPrint("3) Poner URC handling en modo manual\r\n\r\n");

         dbgPrint("S) Salir a la consola principal \r\n");

         while(0 == uartRecv(CIAA_UART_USB, &instruction, 1)){;}

         switch (instruction) {

            case '1':

            urc = getURC();

            if(urc.cmd[0] != '\0'){
               dbgPrint("\r\nURC: ");
               dbgPrint(urc.cmd);
               dbgPrint("(");
               dbgPrint(urc.param);
               dbgPrint(")\r\n");
            }
            else{
               dbgPrint("\r\nNo hay URCs pendientes\r\n");
            }

            break;

            case '2':

            ciaaMobile_setUrcCback(cbUrc);

            break;

            case '3':

            ciaaMobile_setUrcManual();

            break;

            case 'S':

            break;

            default:

            dbgPrint("INSTRUCCION DESCONOCIDA \r\n\r\n");
            break;

         }

         while(!ciaaMobile_isIdle()){
            ciaaMobile_sysUpdate();
         }

      }

      ciaaMobile_sysUpdate();

   }

   return;
}

/*==================[external functions definition]==========================*/

void SysTick_Handler(void)
{
   if(pausems_count > 0) pausems_count--;

   ciaaMobile_SysTick_Handler();
}

int main(void)
{
   initHardware();
   ciaaUARTInit();

   uint8_t gprs_flag = 0;
   uint8_t tcp_flag = 0;

   uint8_t instruction;

   signal_quality_s sigqual;
   statusGSMGPRS_s status;

   pausems(DELAY_INIT);

   dbgPrint("\r\n>>> INICIALIZANDO MODEM CELULAR <<< \r\n\r\n");

   ciaaMobile_startUp(cbempty);

   while(!ciaaMobile_isIdle()){
      ciaaMobile_sysUpdate();
   }

   while (1){

      if(ciaaMobile_isIdle()){

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

            ciaaMobile_getSignalQuality (&sigqual, cbempty);

            break;

            case '6':

            ciaaMobile_checkGSMGPRS(&status, cbgsmgprs);

            break;

            default:

            dbgPrint("INSTRUCCION DESCONOCIDA \r\n\r\n");

            break;

         }

         while(!ciaaMobile_isIdle()){
            ciaaMobile_sysUpdate();
         }

      }

      ciaaMobile_sysUpdate();

   }

   return 0;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
