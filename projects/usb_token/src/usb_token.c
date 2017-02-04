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

static ATToken parse(uint8_t * token);

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
   ATToken received; /* classifies the received token*/

   while (1){

      nRead = uartRecv(CIAA_UART_USB, (void *) &UARTRead, 1);
      if (0 != nRead){

         if('\r' != UARTRead && '\n' != UARTRead) {
            tk_f = 1;
            UARTBuffer[i++] = UARTRead;
            uartSend(CIAA_UART_USB, (void *) &UARTRead, 1);
         }
         else if('\r' == UARTRead && 1 == tk_f){
            UARTBuffer[i] = '\0';
            strncpy(tokenBuffer, UARTBuffer, i); /* this seems to be useless as one could send */
            tokenBuffer[i] = '\0';               /* a pointer to UARTBuffer directly, but in   */
            received = parse(tokenBuffer);       /* the final implementation the UART Read will*/

            switch(received){
               case INVALID:
                  dbgPrint("Invalid");
                  break;
               case COMMAND:
                  dbgPrint("AT Command");
                  break;
               case RESPONSE:
                  dbgPrint("AT Response");
                  break;
            }
            dbgPrint("\r\n\r\n");
            i = 0;                               /* perhaps interrupt the parsing, so the      */
            tk_f = 0;                            /* buffers need to be different               */
         }
      }

   }

}

static ATToken parse(uint8_t * token)
{
   int i = 0; /* loop counter */
   uint8_t equalPos = 0; /* position of the '=' char in the token, if present */
   uint8_t intPos = 0; /* position of the '?' char in the token, if present */
   uint8_t command[20]; /* AT command string not considering the 'AT' or "AT+' headers */
   uint8_t parameter[20]; /* AT command argument string (for WRITE extended commands) */

   dbgPrint("\r\nToken: ");
   dbgPrint(token);

   /* determine if the token is an AT command, be it extended or basic */

   if(('A' == token[0]) && ('T' == token[1])){

      /* extended AT command */

      if('+' == token[2]){

         /* Search for '=' and '?' characters and store position to determine type
            of extended AT command */

         for (i = 3; i < strlen(token); i++){
            if ('=' == token[i]) {equalPos = i;}
            else if ('?' == token[i]) {intPos = i;}
         }

         /* Determine the type of extended command (TEST, READ, WRITE or EXECUTION)
            depending on the position of the '=' and '?' characters. Afterwards,
            copy the correct part of the token corresponding to the command. */

         if (0 != equalPos){

            strncpy(command,&token[3],(equalPos - 3)); /* copy the part between '+' and '=' */
            command[equalPos -3] = '\0';

            if((equalPos+1) == intPos){
            dbgPrint("\r\nExtended command TEST: ");
            dbgPrint(command);
            dbgPrint("\r\n");
            return COMMAND;
            }

            else{
               strncpy(parameter,&token[equalPos+1],strlen(token)-equalPos);
               dbgPrint("\r\nParameter: ");
               dbgPrint(parameter);
               dbgPrint("\r\nExtended command WRITE: ");
               dbgPrint(command);
               dbgPrint("\r\n");
               return COMMAND;
            }

         }
         else if((strlen(token)-1) == intPos){
            strncpy(command,&token[3],(intPos - 3)); /* copy the part between '+' and '?' */
            command[intPos -3] = '\0';
            dbgPrint("\r\nExtended command READ: ");
            dbgPrint(command);
            dbgPrint("\r\n");
            return COMMAND;
         }
         else{
            strncpy(command,&token[3],(strlen(token) - 3)); /* copy everything after '+' */
            command[strlen(token) -3] = '\0';
            dbgPrint("\r\nExtended command EXECUTION: ");
            dbgPrint(command);
            dbgPrint("\r\n");
            return COMMAND;
         }
      }

      /* basic AT command */

      else {

         if('&' == token[2]){

            if('\0' != token[3]){
               command[0] = token[3];
               command[1] = '\0';
               dbgPrint("\r\nBasic command: ");
               dbgPrint(command);

               if('\0' != token[4]){
                  strncpy(parameter,&token[4],strlen(token)-4);
                  parameter[strlen(token)-4] = '\0';
                  dbgPrint("\r\nParameter: ");
                  dbgPrint(parameter);

               }

               dbgPrint("\r\n");
               return COMMAND;
            }

            else {dbgPrint("\r\nUnknown or incomplete AT command\r\n");}
         }
         else{

            if('\0' != token[2]){
               command[0] = token[2];
               command[1] = '\0';
               dbgPrint("\r\nBasic command: ");
               dbgPrint(command);

               if('\0' != token[3]){
                  strncpy(parameter,&token[3],strlen(token)-3);
                  parameter[strlen(token)-3] = '\0';
                  dbgPrint("\r\nParameter: ");
                  dbgPrint(parameter);
               }

               dbgPrint("\r\n");
               return COMMAND;
            }
            else{
               dbgPrint("\r\nUnknown or incomplete AT command\r\n");
               return INVALID;
            }
         }
      }
   }

   /* If not an AT command, determine is the token is a known response */

   /* Basic "OK" response, used in many commands */

   else if(('O' == token[0]) && ('K' == token[1])){
      dbgPrint("\r\nSuccessful command indicator\r\n");
      return RESPONSE;
   }

   /* +<x>:<n> response, where <x> is an AT command and <n> a parameter> */

   //else if('+')

   else{
      dbgPrint("\r\nUnknown response or command");
   }

   dbgPrint("\r\n");

   return INVALID;
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
