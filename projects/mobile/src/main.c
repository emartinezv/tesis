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

void * cbprint (error_user, void *);

/*==================[internal data definition]===============================*/

/* TIMING COUNTERS */

/** @brief used for delay counter */
static uint32_t pausems_count = DELAY_INIT;

/** @brief used for delay counter */
static uint32_t readsms_count = DELAY_READSMS;

/** @brief used for delay counter */
static uint32_t sendsms_count = DELAY_SENDSMS;

/** @brief used for delay counter */
static uint32_t readURC_count = DELAY_READURC;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

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
      SMS_rec * target = (SMS_rec *)input;

      for(i = 0; (target+i)->meta[0] != '\0'; i++){

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

/*==================[external functions definition]==========================*/

void SysTick_Handler(void)
{
   if(pausems_count > 0) pausems_count--;
   if(readsms_count > 0) readsms_count--;
   if(sendsms_count > 0) sendsms_count--;
   if(readURC_count > 0) readURC_count--;

   ciaaMobile_SysTick_Handler();
}

int main(void)
{
   initHardware();
   ciaaUARTInit();

   uint8_t gprs_flag = 0;
   uint8_t tcp_flag = 0;

   uint8_t instruction_str[2];
   uint8_t instruction;

   uint8_t command [150];
   uint8_t parameter [150];

   SMS_rec list[10];
   SMS_del borrar;

   SMS_send msg = {"1151751809","Hola mundo!"};
   APN_usr_pwd APN = {"datos.personal.com","datos","datos"};
   port_s port1 = {TCP, "104.236.225.217","2399"};
   port_s port2 = {UDP, "104.236.225.217","2399"};
   signal_quality_s sigqual;
   power_GNSS_e powerGNSS;

   pausems(DELAY_INIT);

   dbgPrint("\r\n>>> INICIALIZANDO MODEM CELULAR <<< \r\n\r\n");

   ciaaMobile_startUp(cbempty);

   while(!ciaaMobile_isIdle()){
      ciaaMobile_sysUpdate();
   }

   dbgPrint("\r\n >>> CONSOLA DE TESTEO <<< \r\n");
   dbgPrint("\r\n1) Mandar SMS \r\n");
   dbgPrint("2) Abrir puerto TCP \r\n");
   dbgPrint("3) Abrir puerto UDP \r\n");
   dbgPrint("4) Cerrar puerto TCP o UDP \r\n");
   dbgPrint("5) Ver calidad de señal \r\n");
   dbgPrint("6) Prender GNSS \r\n");
   dbgPrint("7) Apagar GNSS \r\n");

   while (1){

      if(ciaaMobile_isIdle()){
         if(0 != uartRecv(CIAA_UART_USB, instruction_str, 1)){
            instruction_str[1] = '\0';
            instruction = atoi(instruction_str);

            switch (instruction) {

               case 1:

               ciaaMobile_sendSMS(&msg, cbempty);

               while(!ciaaMobile_isIdle()){
                  ciaaMobile_sysUpdate();
               }

               break;

               case 2:

               dbgPrint("EJECUTANDO INSTRUCCION 2... \r\n\r\n");
               break;

               case 3:

               ciaaMobile_startGPRS(&APN, cbempty);

               while(!ciaaMobile_isIdle()){
                  ciaaMobile_sysUpdate();
               }

               ciaaMobile_openPort(&port2, cbempty);

               while(!ciaaMobile_isIdle()){
                  ciaaMobile_sysUpdate();
               }

               while(DATA_MODE == checkSerialMode()){

                  uint8_t data_char;

                  if(0 != uartRecv(CIAA_UART_USB, data_char, 1)){
                     uartSend(CIAA_UART_232, data_char, 1); /* mando a 232 */
                     uartSend(CIAA_UART_USB, data_char, 1); /* eco */
                  }

               }

               break;

               case 4:

               dbgPrint("EJECUTANDO INSTRUCCION 4... \r\n\r\n");
               break;

               case 5:

               ciaaMobile_getSignalQuality(&sigqual, cbempty);

               while(!ciaaMobile_isIdle()){
                  ciaaMobile_sysUpdate();
               }

               break;

               case 6:

               powerGNSS = ON;

               ciaaMobile_powerGNSS(&powerGNSS, cbempty);

               while(!ciaaMobile_isIdle()){
                  ciaaMobile_sysUpdate();
               }

               break;

               case 7:

               powerGNSS = OFF;

               ciaaMobile_powerGNSS(&powerGNSS, cbempty);

               while(!ciaaMobile_isIdle()){
                  ciaaMobile_sysUpdate();
               }

               break;

               default:

               dbgPrint("INSTRUCCION DESCONOCIDA \r\n\r\n");
               break;

            }

            dbgPrint("\r\n >>> CONSOLA DE TESTEO <<< \r\n");
            dbgPrint("\r\n1) Mandar SMS \r\n");
            dbgPrint("2) Abrir puerto TCP \r\n");
            dbgPrint("3) Abrir puerto UDP \r\n");
            dbgPrint("4) Cerrar puerto TCP o UDP \r\n");
            dbgPrint("5) Ver calidad de señal \r\n");
            dbgPrint("6) Prender GNSS \r\n");
            dbgPrint("7) Apagar GNSS \r\n");

         }




      }

      ciaaMobile_sysUpdate();

      /*if (1 == deleteall){
         deleteall = 0;
         borrar.index = 1;
         borrar.mode = 4;
         if(ciaaMobile_isIdle()){ciaaMobile_delSMS(&borrar, cbempty);}
      }

      /*if (0 == sendsms_count){

         sendsms_count = DELAY_SENDSMS;
         if(ciaaMobile_isIdle()){ciaaMobile_sendSMS(&msg, cbempty);}

      }*/

      /*if (0 == readsms_count){

         readsms_count = DELAY_READSMS;
         if(ciaaMobile_isIdle()){ciaaMobile_listRecSMS(list, 10, cbprint);}

      }*/

      /*if (0 == readsms_count){

         if (0 == gprs_flag){

            if(ciaaMobile_isIdle()){
               gprs_flag = 1;
               readsms_count = DELAY_READSMS;
               ciaaMobile_startGPRS(&APN, cbempty);
            }

         }

         else if (0 == tcp_flag){

            if(ciaaMobile_isIdle()){
               tcp_flag = 1;
               readsms_count = DELAY_READSMS;
               ciaaMobile_openPort(&port1, cbempty);
            }

         }

      }

      /*if (0 == readURC_count){

         readURC_count = DELAY_READURC;
         dbgPrint("Leyendo URCs...\r\n");
         if(readURC(command, parameter)){

            dbgPrint("CMD: ");
            dbgPrint(command);
            dbgPrint("\r\nPAR: ");
            dbgPrint(parameter);
            dbgPrint("\r\n");

         }
      }*/
   }

   return 0;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
