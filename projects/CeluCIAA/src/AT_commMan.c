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
ATComm commands[MAX_COMM];

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

/** @brief commSearch function
 * @return takes a token type and command, returns position of the command in
 *  the known command/response vector or 0 if no command/response found
 */

uint8_t commSearch(uint8_t const * const command)
{
   int i = 0;

   for(i = 0; i < MAX_COMM; i++){
      if(0 == strcmp(command, commands[i].name)){return i;}
   }

   return 255;
}

/** @brief commInit function
 * @return loads the command structs with name, number of response tokens
 * and valid responses for each token
 */

void commInit (void)
{
   commands[0].name = commAT_name;
   commands[0].respTokens = commAT_tokens;
   commands[0].responses[0] = commAT_response0;

   commands[1].name = commATI_name;
   commands[1].respTokens = commATI_tokens;
   commands[1].responses[0] = commATI_responses0;
   commands[1].responses[1] = commATI_responses1;

   return;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
