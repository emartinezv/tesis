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

#define PRINTOUT ;

/*==================[global data]============================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 * @return none
 */
static void initHardware(void);

/** @brief processToken function
* @return
*/
void processToken(void);

/** @brief sendAT function
* @return
*/
void sendAT(void);

/** @brief updateFSM function
* @return
*/
int updateFSM (ATToken received,
               uint8_t const * const command,
               uint8_t const * const parameter);

/** @brief delay function
 * @param t desired milliseconds to wait
 */
static void pausems(uint32_t t);

/*==================[internal data definition]===============================*/

/** @brief used for processToken function scheduling with SysTick */
static uint32_t processToken_count = DELAY_PROTKN;

/** @brief used for sendAT function scheduling with SysTick */
static uint32_t sendAT_count = DELAY_SENDAT;

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

void processToken(void)
{
   processToken_count = DELAY_PROTKN;

   ATToken received; /* classifies the received token*/

   uint8_t token[TKN_LEN]; /* received token */
   uint8_t command[TKN_LEN]; /* AT command or responde */
   uint8_t parameter[TKN_LEN]; /* AT command or response argument */

   if(0 != tokenRead(token)){

      received = parse(token, command, parameter); /* parse the received token */

      #ifdef PRINTOUT
      switch(received){

         case BASIC_CMD:
         case EXT_CMD_WRITE:
         case EXT_CMD_READ:
         case EXT_CMD_TEST:
         case EXT_CMD_EXEC:

            /* printout */

            dbgPrint("\r\nCOMMAND: ");
            dbgPrint(command);
            dbgPrint("(");
            dbgPrint(parameter);
            dbgPrint(")\r\n");

         break;

         case BASIC_RSP:
         case EXT_RSP:

            /* printout */

            dbgPrint("\r\nRESPONSE: ");
            dbgPrint(command);
            dbgPrint("(");
            dbgPrint(parameter);
            dbgPrint(")\r\n");

         break;

      }

      dbgPrint("\r\n");
      #endif

      updateFSM(received, command, parameter);

   }

   return;

}

void sendAT(void)
{
   sendAT_count = DELAY_SENDAT;

   rs232Print("AT\r");
   dbgPrint("AT enviado\r\n");
   updateFSM(SENT,"AT","");

   return;
}

int updateFSM (ATToken received,
               uint8_t const * const command,
               uint8_t const * const parameter)
{
   static GSMstates state = IDLE;
   static uint8_t currCMD[TKN_LEN];
   static uint8_t currPAR[TKN_LEN];

   switch(state){

      case IDLE:

         if (SENT == received){
            strncpy(currCMD,command,strlen(command));
            currCMD[strlen(command)] = '\0';
            strncpy(currPAR,parameter,strlen(parameter));
            currPAR[strlen(parameter)] = '\0';
            state = CMD_SENT;
            dbgPrint("COMMAND SENT");
            return 1;
         }
         else
            return 0;

         break;

      case CMD_SENT:

         if ((received >= BASIC_CMD) && (received <= EXT_CMD_EXEC)){

            uint8_t eqCMD = 1;
            uint8_t eqPAR = 1;
            eqCMD = strncmp(command, currCMD, strlen(currCMD));
            eqPAR = strncmp(parameter, currPAR, strlen(currPAR));

            if ((0 == eqCMD) && (0 == eqPAR)){
               state = CMD_ACK;
               dbgPrint("COMMAND ACK");
               return 1;
            }
            else
               return 2;
         }
         else
            return 2;

         break;

      case CMD_ACK:

         if ((received >= BASIC_RSP) && (received <= EXT_RSP))
            dbgPrint("RESPONSE RECEIVED");
         else
            return 2;

         break;

      default:

         dbgPrint("ERROR: SWITCH OUT OF RANGE");

         break;
   }

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

   while (1){

      if(0 == processToken_count)
         processToken();
      if(0 == sendAT_count)
         sendAT();

   }

   return 0;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
