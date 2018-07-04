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
      {"AT"         , "OK" , "ERROR, +CMS ERROR"              , TOUT_DEF},
      {"I"          , "OK" , "ERROR, +CMS ERROR"              , TOUT_DEF},
      {"CMEE"       , "OK" , "ERROR, +CME ERROR"              , TOUT_DEF},
      {"CMGL"       , "OK" , "ERROR, +CME ERROR, +CMS ERROR"  , 20000},
      {"CMGF"       , "OK" , "ERROR, +CME ERROR, +CMS ERROR"  , TOUT_DEF},
      {"CMGS"       , "> " , "ERROR, +CME ERROR, +CMS ERROR"  , 60000},
      {"CSCS"       , "OK" , "ERROR, +CME ERROR"              , TOUT_DEF},
      {"CSQ"        , "OK" , "ERROR, +CME ERROR"              , TOUT_DEF},
      {"CNMI"       , "OK" , "ERROR"                          , TOUT_DEF},
      {"CMGD"       , "OK" , "ERROR"                          , 25000},
      {"CGATT"      , "OK" , "ERROR, +CME ERROR"              , 10000},
      {"CSTT"       , "OK" , "ERROR, +CME ERROR"              , 1000*TOUT_DEF}, // VER SI MEJORA
      {"CIICR"      , "OK" , "ERROR, +CME ERROR"              , 85000},
      {"CIPSTART"   , "ALREADY CONNECT, CONNECT, STATE: , CONNECT FAIL" // PARCHE HORRIBLE DEBERIA DECIR
                                                                        // CONNECT OK ENTRE LAS OPCIONES
                                                                        // PERO GENERA PROBLEMAS...VER!!!
                    , "ERROR, +CME ERROR"                     , 160000},
      {"CIPCLOSE"   , "CLOSE OK", "ERROR, +CME ERROR"         , 1000 * TOUT_DEF}, // ARBITRARIO
      {"CIPSHUT"    , "SHUT OK" , "ERROR, +CME ERROR"         , 65000},
      {"CIPMODE"    , "OK" , "ERROR, +CME ERROR"              , TOUT_DEF},
      {"SMS_BODY"   , "OK" , "ERROR"                          , 60000},
      {0 , 0 , 0},
};

/** @brief Vector of known Unrequested Result Codes */

uint8_t const * const URCs [] = {
      "CMTI",
      0
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

uint8_t URCSearch(uint8_t const * const urc)
{
   int i = 0;

   for(i = 0; 0 != URCs[i]; i++){
      if(0 == strcmp(urc, URCs[i])){return 1;}
   }

   return 0;
}

/*==================[end of file]============================================*/
