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

#include "AT_engine.h"
#include "AT_parser.h"
#include "AT_commMan.h"
#include "board.h"
#include "string.h"
#include "ciaaUART_T.h"

/*==================[macros and definitions]=================================*/

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

/*==================[internal data definition]===============================*/

/** @brief used for delay counter */
static uint32_t pausems_count;

/*==================[external data definition]===============================*/

/** @vector of known AT commands*/
extern ATComm commands [MAX_COMM];

/** @vector of known AT responses*/
extern ATResp responses [MAX_RESP];



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

/*==================[external functions definition]==========================*/

void SysTick_Handler(void)
{
   if(pausems_count > 0) pausems_count--;
}

void processToken(void)
{
   ATToken received; /* classifies the received token*/

   uint8_t token[TKN_LENGTH]; /* received token */
   uint8_t command[TKN_LENGTH]; /* AT command or responde */
   uint8_t parameter[TKN_LENGTH]; /* AT command or response argument */

   if(0 != tokenRead(token)){

      received = parse(token, command, parameter); /* parse the received token */

      switch(received){

         case BASIC_COMMAND:
         case EXTENDED_COMMAND_WRITE:
         case EXTENDED_COMMAND_READ:
         case EXTENDED_COMMAND_TEST:
         case EXTENDED_COMMAND_EXECUTION:

            /* printout */

            dbgPrint("\r\nCOMMAND: ");
            dbgPrint(command);
            dbgPrint("(");
            dbgPrint(parameter);
            dbgPrint(")\r\n");

         break;

         case BASIC_RESPONSE:
         case EXTENDED_RESPONSE:

            /* printout */

            dbgPrint("\r\nRESPONSE: ");
            dbgPrint(command);
            dbgPrint("(");
            dbgPrint(parameter);
            dbgPrint(")\r\n");

         break;

      }

      dbgPrint("\r\n");
   }

   return;

}


int main(void)
{
   int i; /* loop counter */

   initHardware();
   ciaaUARTInit();
   commInit();

   while (1){



      /* processToken(); */

   }

   return 0;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
