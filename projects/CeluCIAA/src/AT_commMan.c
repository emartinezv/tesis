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

#include "AT_commMan.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/** @vector of known AT commands*/
ATComm commands [MAX_COMM];

/** @vector of known AT responses*/
ATResp responses [MAX_RESP];

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

/** @brief commSearch function
 * @return takes a token type and command, returns position of the command in
 *  the known command/response vector or 0 if no command/response found
 */

uint8_t commSearch(ATToken const type, uint8_t const * const command)
{
   int i = 0;

   switch(type){
      case BASIC_CMD:
      case EXT_CMD_WRITE:
      case EXT_CMD_READ:
      case EXT_CMD_TEST:
      case EXT_CMD_EXEC:

         for(i = 0; i < MAX_COMM; i++){
            if(0 == strcmp(command, commands[i].name)){return i;}
         }

         break;

      case EXT_RSP:
      case BASIC_RSP:

         for(i = 0; i < MAX_RESP; i++){
            if(0 == strcmp(command, responses[i].name)){return i;}
         }

         break;

      case INVALID:

         return 0;
   }

   return 0;
}

/** @brief commInit function
 * @return loads the command and response vectors with the name and callback
 *  for each command or response
 */

void commInit (void)
{
   commands[0].name[0] = '\0';
   commands[0].execution = 0;
   commands[0].write = 0;
   commands[0].test = 0;
   commands[0].read = 0;

   responses[0].name[0] = '\0';
   responses[0].response = 0;

   strncpy(commands[1].name, "AT", strlen("AT"));
   commands[1].name[strlen("AT")] = '\0';
   commands[1].execution = &cbAT;
   commands[1].write = 0;
   commands[1].test = 0;
   commands[1].read = 0;

   strncpy(commands[2].name, "CMGF", strlen("CMGF"));
   commands[2].name[strlen("CMGF")] = '\0';
   commands[2].execution = 0;
   commands[2].write = 0;
   commands[2].test = 0;
   commands[2].read = 0;

   strncpy(commands[3].name, "CSCS", strlen("CSCS"));
   commands[3].name[strlen("CSCS")] = '\0';
   commands[3].execution = 0;
   commands[3].write = 0;
   commands[3].test = 0;
   commands[3].read = 0;

   strncpy(commands[4].name, "CMGS", strlen("CMGS"));
   commands[4].name[strlen("CMGS")] = '\0';
   commands[4].execution = 0;
   commands[4].write = 0;
   commands[4].test = 0;
   commands[4].read = 0;

   strncpy(responses[1].name, "OK", strlen("OK"));
   responses[1].name[strlen("OK")] = '\0';
   responses[1].response = 0;

   return;
}

/** @brief ccAT function
 * @return callback function for the "AT" command
 */

int cbAT (uint8_t const * const parameter)
{
   dbgPrint("Funcion callback del comando AT\r\n");
   return 0;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
