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

#include "usb_token.h"
#include "board.h"
#include "string.h"
#include "ciaaUART.h"

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

static void tokenize(uint8_t * buffer);

static void parse(uint8_t * token);

static uint8_t commSearch(uint8_t * command);

static void CeluCIAAInit (void);


/*==================[internal data definition]===============================*/

/** @brief used for delay counter */
static uint32_t pausems_count;

/** @UART received messages buffer */
uint8_t UARTBuffer[50];

/** @token buffer */
uint8_t tokenBuffer[25];

/** @vector of extended AT commands */
uint8_t extComm [5][10];

/** @vector of basic AT commands */
uint8_t basicComm [5][10];

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

static void tokenize(uint8_t * buffer)
{
   uint8_t UARTRead; /* UART RX character */
   int i = 0;
   int tk_f = 0; /* flag for string being read */
   int nRead; /* number of bytes read */

   dbgPrint(">>> funcion tokenize <<<\n\r\n\r");

   while (1){

      nRead = uartRecv(CIAA_UART_USB, (void *) &UARTRead, 1);
      if (0 != nRead){

         if('\r' != UARTRead && '\n' != UARTRead) {
            tk_f = 1;
            UARTBuffer[i++] = UARTRead;
            dbgPrint("Caracter leido: ");
            uartSend(CIAA_UART_USB, (void *) &UARTRead, 1);
            dbgPrint("\r\n");
         }
         else if('\r' == UARTRead && 1 == tk_f){
            UARTBuffer[i] = '\0';
            strncpy(tokenBuffer, UARTBuffer, i);
            tokenBuffer[i] = '\0';
            parse(tokenBuffer);
            i = 0;
            tk_f = 0;
         }
      }

   }

}

static void parse(uint8_t * token)
{
   int i = 0; /* loop counter */
   uint8_t equalPos = 0; /* position of the '=' char in the token, if present */
   uint8_t intPos = 0; /* position of the '?' char in the token, if present */
   uint8_t command[20]; /* command string not considering the 'AT' or "AT+' headers */

   for (i = 0; i < 20; i++){
      command[i] = 'F';
   }

   dbgPrint("\r\n>>> ABRE funcion parse <<<\r\n");
   dbgPrint("Token recibido: ");
   dbgPrint(token);

   if(('A' == token[0]) && ('T' == token[1])){
      if('+' == token[2]){

         dbgPrint("\r\nComando extendido");

         /* Search for '=' and '?' characters to determine type of extended AT command */

         for (i = 3; i < strlen(token); i++){
            if ('=' == token[i]){
               equalPos = i;
               dbgPrint("\r\nEncontre el caracter '='");
            }
            else if ('?' == token[i]){
               intPos = i;
               dbgPrint("\r\nEncontre el caracter '?'");
            }
         }

         if (0 != equalPos){
            strncpy(command,token[3],(equalPos - 3));
            //command[equalPos -3] = '\0';
            dbgPrint("\r\nComando: ");
            dbgPrint(command);
            dbgPrint("\r\n");
         }

      }
      else {
         dbgPrint("\r\nComando basico");
      }
   }
   else if(('O' == token[0]) && ('K' == token[1])){
      dbgPrint("\r\nIndicador de comando exitoso");
   }
   else{
      dbgPrint("\r\nComando o respuesta desconocido");
   }

   dbgPrint("\r\n>>> CIERRA funcion parse <<<\r\n\r\n");

   return 0;
}

static void CeluCIAAInit (void)
{
   extComm[0][0] = "CMGF";
   extComm[1][0] = "CSCS";
   extComm[2][0] = "CMGS";

   return 0;
}

/*==================[external functions definition]==========================*/

void SysTick_Handler(void)
{
   if(pausems_count > 0) pausems_count--;
}

int main(void)
{
   int i; /* loop counter */

   initHardware();
   ciaaUARTInit();
   CeluCIAAInit();

   for (i = 0; i < 20; i++){
      UARTBuffer[i] = '\0';
   }

   pausems(DELAY_S);

   tokenize(UARTBuffer);

   return 0;

}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
