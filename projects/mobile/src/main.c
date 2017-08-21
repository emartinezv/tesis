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

/** @brief This is a simple UART example
 */

/** \addtogroup uart Bare-metal example
 ** @{ */

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

/*==================[internal data definition]===============================*/

/* TIMING COUNTERS */

/** @brief used for processToken function scheduling with SysTick */
static int32_t processToken_count = DELAY_PROTKN;

/** @brief used for sendAT function scheduling with SysTick */
static int32_t sendAT_count = DELAY_SENDAT;

/** @brief used for delay counter */
static int32_t pausems_count;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
   Board_Init();
   SystemCoreClockUpdate();
   SysTick_Config(SystemCoreClock / 1000);
}

/*==================[external functions definition]==========================*/

void SysTick_Handler(void)
{
   if(pausems_count > 0) pausems_count--;
   if(processToken_count > 0) processToken_count--;
   if(sendAT_count > 0) sendAT_count--;
}

int main(void)
{
   initHardware();
   ciaaUARTInit();

   const ATcmd cmdList [5] = {
         {0,0,AUTOBAUD},
         {"CMGF","1",EXT_WRITE},
         {"CSCS","\"GSM\"",EXT_WRITE},
         {"CMGS","\"1551751809\"",EXT_WRITE},
         {"SMS_BODY","Mensaje de prueba",SMS}
   };

   uint8_t cmdNum = 0;

   while (1){

      if (0 == processToken_count)

         processToken_count = DELAY_PROTKN;
         processToken();

      if (0 == sendAT_count){

         sendAT_count = DELAY_SENDAT;

         if (cmdNum < 5){
            sendATcmd(cmdList[cmdNum].cmd, cmdList[cmdNum].par, cmdList[cmdNum].type);
            cmdNum++;
         }

      }
   }

   return 0;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
