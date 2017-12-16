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

void * cb (void *);

void * cbempty (void *);

void * cbprint (void *);

/*==================[internal data definition]===============================*/

/* TIMING COUNTERS */

/** @brief used for processToken function scheduling with SysTick */
static int32_t mblSysUp_count = DELAY_MBLSYSUP;

/** @brief used for delay counter */
static uint32_t pausems_count = DELAY_INIT;

/** @brief used for delay counter */
static uint32_t readsms_count = DELAY_READSMS;

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

void * cbempty (void * input)
{
   return 0;
}

void * cbled (void * input)
{
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

void * cbprint (void * input)
{
   dbgPrint("Imprimiendo SMS...\r\n\r\n");

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

   return;
}

/*==================[external functions definition]==========================*/

void SysTick_Handler(void)
{
   if(mblSysUp_count > 0) mblSysUp_count--;
   if(pausems_count > 0) pausems_count--;
   if(readsms_count > 0) readsms_count--;
}

int main(void)
{
   initHardware();
   ciaaUARTInit();

   SMS_rec list[10];
   SMS_del borrar;

   //SMS_send msg = {"1151751809","Hola mundo de nuevo!"};

   pausems(DELAY_INIT);

   ciaaMobile_startUp();

   while (1){

      if (1 == deleteall){
         deleteall = 0;
         borrar.index = 1;
         borrar.mode = 4;
         if(ciaaMobile_isIdle()){ciaaMobile_delSMS(&borrar, cbempty);}
      }

      if (0 == mblSysUp_count){

         mblSysUp_count = DELAY_MBLSYSUP;
         ciaaMobile_sysUpdate();

      }

      if (0 == readsms_count){

         readsms_count = DELAY_READSMS;
         if(ciaaMobile_isIdle()){ciaaMobile_listRecSMS(list, cbprint);}

      }
   }

   return 0;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
