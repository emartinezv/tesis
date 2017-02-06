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

static ATToken parse(uint8_t const * const token, uint8_t * command, uint8_t * parameter);

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

static ATToken parse(uint8_t const * const token, uint8_t * command, uint8_t * parameter)
{
   int i = 0; /* loop counter */
   uint8_t equalPos = 0; /* position of the '=' char in the token, if present */
   uint8_t intPos = 0; /* position of the '?' char in the token, if present */
   uint8_t colonPos = 0; /* position of the ':' char in the token, if present */

   command[0] = '\0';
   parameter[0] = '\0';

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

            if((equalPos+1) == intPos){return EXTENDED_COMMAND_TEST;}

            else{
               strncpy(parameter,&token[equalPos+1],strlen(token)-equalPos);
               return EXTENDED_COMMAND_WRITE;
            }

         }
         else if((strlen(token)-1) == intPos){
            strncpy(command,&token[3],(intPos - 3)); /* copy the part between '+' and '?' */
            command[intPos -3] = '\0';
            return EXTENDED_COMMAND_READ;
         }
         else{
            strncpy(command,&token[3],(strlen(token) - 3)); /* copy everything after '+' */
            command[strlen(token) -3] = '\0';
            return EXTENDED_COMMAND_EXECUTION;
         }
      }

      /* basic AT command */

      else {

         if('&' == token[2]){

            if('\0' != token[3]){
               command[0] = token[3];
               command[1] = '\0';

               if('\0' != token[4]){
                  strncpy(parameter,&token[4],strlen(token)-4);
                  parameter[strlen(token)-4] = '\0';

               }

               return BASIC_COMMAND;
            }

            else {return INVALID;}
         }
         else{

            if('\0' != token[2]){
               command[0] = token[2];
               command[1] = '\0';

               if('\0' != token[3]){
                  strncpy(parameter,&token[3],strlen(token)-3);
                  parameter[strlen(token)-3] = '\0';
               }

               return BASIC_COMMAND;
            }
            else{return INVALID;}
         }
      }
   }

   /* If not an AT command, determine is the token is a known response */

   /* Extended syntax response */

   if('+' == token[0]){

      /* Search for ':' character and store position if present */

      for (i = 1; i < strlen(token); i++){if (':' == token[i]) {colonPos = i;}}

      /* If no ':' character is present we have a simple extended sintax response */

      if(0 == colonPos){
         strncpy(command,&token[1],strlen(token)-1);
         command[strlen(token)-1] = '\0';
         return EXTENDED_RESPONSE;
      }

      /* If ':' character is present, what follows is a parameter of the response */

      else{
         strncpy(command,&token[1],colonPos-1);
         command[colonPos-1] = '\0';
         strncpy(parameter,&token[colonPos+1],strlen(token)-colonPos);
         command[strlen(token)-colonPos] = '\0';
         return EXTENDED_RESPONSE;
      }

   }

   /* Basic response */

   else{
      strncpy(command,token,strlen(token));
      command[strlen(token)] = '\0';
      return BASIC_RESPONSE;
   }

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

   ATToken received; /* classifies the received token*/

   uint8_t token[TKN_LENGTH]; /* received token */
   uint8_t command[TKN_LENGTH]; /* AT command or responde */
   uint8_t parameter[TKN_LENGTH]; /* AT command or response argument */

   while (1){

      if(0 != tokenRead(token)){

         received = parse(token, command, parameter);

         dbgPrint("\r\n\r\n");

         switch(received){
            case INVALID:
               dbgPrint("INVALID TOKEN\r\n");
               break;
            case BASIC_COMMAND:
               dbgPrint("BASIC COMMAND: ");
               dbgPrint(command);
               dbgPrint("(");
               dbgPrint(parameter);
               dbgPrint(")\r\n");
               break;
            case EXTENDED_COMMAND_WRITE:
               dbgPrint("EXTENDED COMMAND WRITE: ");
               dbgPrint(command);
               dbgPrint("(");
               dbgPrint(parameter);
               dbgPrint(")\r\n");
               break;
            case EXTENDED_COMMAND_READ:
               dbgPrint("EXTENDED COMMAND READ: ");
               dbgPrint(command);
               dbgPrint("\r\n");
               break;
            case EXTENDED_COMMAND_TEST:
               dbgPrint("EXTENDED COMMAND TEST: ");
               dbgPrint(command);
               dbgPrint("\r\n");
               break;
            case EXTENDED_COMMAND_EXECUTION:
               dbgPrint("EXTENDED COMMAND EXECUTION: ");
               dbgPrint(command);
               dbgPrint("\r\n");
               break;
            case BASIC_RESPONSE:
               dbgPrint("BASIC RESPONSE: ");
               dbgPrint(command);
               dbgPrint("(");
               dbgPrint(parameter);
               dbgPrint(")");
               break;
            case EXTENDED_RESPONSE:
               dbgPrint("EXTENDED RESPONSE: ");
               dbgPrint(command);
               dbgPrint("(");
               dbgPrint(parameter);
               dbgPrint(")");
               break;

         dbgPrint("\r\n\r\n");

         }

      }

   }

   return 0;

}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
