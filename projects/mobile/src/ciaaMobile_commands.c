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

/** @brief This module handles the internal AT cmd catalog storage and search
 */

/** \addtogroup commands commands
 ** @{ */

/*==================[inclusions]=============================================*/

#include "ciaaMobile_commands.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/** @brief Vector of known AT commands, including their end response options */

ATComm const commands [] = {
      {"AT" , "ERROR,OK" , TOUT_DEF},
      {"I" , "ERROR,OK" , TOUT_DEF},
      {"CMGL" , "ERROR,OK", 20000},
      {"CMGF" , "ERROR,OK", TOUT_DEF},
      {"CMGS" , "> ,ERROR" , 60000},
      {"CSCS" , "ERROR,OK" , TOUT_DEF},
      {"CNMI" , "ERROR,OK" , TOUT_DEF},
      {"CMGD" , "ERROR,OK" , 25000},
      {"SMS_BODY" , "ERROR,OK" , 60000},
      {0 , 0},
};

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

uint16_t commSearch(uint8_t const * const command)
{
   int i = 0;

   for(i = 0; 0 != commands[i].name; i++){
      if(0 == strcmp(command, commands[i].name)){return i;}
   }

   return 65535;
}

/*==================[end of file]============================================*/
