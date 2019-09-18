/* Copyright 2019, Ezequiel Martinez Vazquez
 * All rights reserved.
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

/** \addtogroup commands commands
 ** @{ */

/*==================[inclusions]=============================================*/

#include "gsmCommands.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/** @brief Used for the private vector of AT commands. Stores the name,
 *         successful and error end responses and timeout in ms */

typedef struct _atCmd {
   uint8_t const * const name;     /**< pointer to str with command name */
   uint8_t const * const sucRsp;   /**< pointer to str with successful end
                                        responses */
   uint8_t const * const errRsp;   /**< pointer to str with error end
                                        responses */
   uint32_t timeout;               /**< command timeout in ms */
} atCmd_t;

/** @brief Used for the private vector of URCs. Stores the name of the URC */

typedef struct _urc {
   uint8_t const * const name;     /**< pointer to str with URC name */
} urc_t;

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/** @brief Vector of known AT commands */

static atCmd_t const atCmds [] = {

      {"AT"         , "-OK-" , "-ERROR-CMS ERROR-"              , TOUT_DEF},
      {"I"          , "-OK-" , "-ERROR-CMS ERROR-"              , TOUT_DEF},
      {"CMEE"       , "-OK-" , "-ERROR-CMS ERROR-"              , TOUT_DEF},
      {"CMGL"       , "-OK-" , "-ERROR-CME ERROR-CMS ERROR-"    , 20000},
      {"CMGR"       , "-OK-" , "-ERROR-CME ERROR-CMS ERROR-"    , 5000},
      {"CMGF"       , "-OK-" , "-ERROR-CME ERROR-CMS ERROR-"    , TOUT_DEF},
      {"CMGS"       , "-> -" , "-ERROR-CME ERROR-CMS ERROR-"    , 60000},
      {"CSDH"       , "-OK-" , "-ERROR-CME ERROR-CMS ERROR-"    , TOUT_DEF},
      {"CSCS"       , "-OK-" , "-ERROR-CMS ERROR-"              , TOUT_DEF},
      {"CREG"       , "-OK-" , "-ERROR-CMS ERROR-"              , TOUT_DEF},
      {"CSQ"        , "-OK-" , "-ERROR-CMS ERROR-"              , TOUT_DEF},
      {"CNMI"       , "-OK-" , "-ERROR-"                        , TOUT_DEF},
      {"CMGD"       , "-OK-" , "-ERROR-"                        , 25000},
      {"CGATT"      , "-OK-" , "-ERROR-CME ERROR-"              , 10000},
      {"CSTT"       , "-OK-" , "-ERROR-CME ERROR-"              , TOUT_DEF},
      {"CIICR"      , "-OK-" , "-ERROR-CME ERROR-"              , 85000},
      {"CIPSTART"   , "-CONNECT OK-ALREADY CONNECT-CONNECT FAIL-CONNECT-",
                            /* "CONNECT" is not listed as a closing response
                             * in the SIM808 manual, but actual experiments
                             * indicate that it appears occasionally when
                             * connecting to an UDP port. */
                               "-ERROR-CME ERROR-"              , 160000},
      {"CIPCLOSE"   , "-CLOSE OK-", "-ERROR-CME ERROR-"         , TOUT_DEF},
      {"CIPSHUT"    , "-SHUT OK-" , "-ERROR-CME ERROR-"         , 65000},
      {"CIFSR"      , ""     , "-ERROR-CME ERROR-"              , TOUT_DEF},
      {"CIPMODE"    , "-OK-" , "-ERROR-CME ERROR-"              , TOUT_DEF},
      {"CGNSPWR"    , "-OK-" , "-ERROR-CME ERROR-"              , TOUT_DEF},
      {"CGNSINF"    , "-OK-" , "-ERROR-CME ERROR-"              , TOUT_DEF},
      {"SMS_BODY"   , "-OK-" , "-ERROR-"                        , 60000},
};

/** @brief Vector of known URCs */

static urc_t const urcs [] = {
      {"CMTI"},
};

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

uint16_t gsmCmdSearch(uint8_t const * const cmd)
{
   uint16_t i = 0;

   for(i = 0; i < (sizeof(atCmds)/sizeof(atCmd_t)); i++){
      if(0 == strcmp(cmd, atCmds[i].name)){return i;}
   }

   return UNKNOWN_CMD;
}

const uint8_t const *  gsmGetCmdSucRsp (uint16_t idx)
{
   if(idx < (sizeof(atCmds)/sizeof(atCmd_t))){
      return atCmds[idx].sucRsp;
   }

   return NULL;
}

const uint8_t const * gsmGetCmdErrRsp (uint16_t idx)
{
   if(idx < (sizeof(atCmds)/sizeof(atCmd_t))){
      return atCmds[idx].errRsp;
   }

   return NULL;
}

uint32_t gsmGetCmdTimeout (uint16_t idx)
{
   if(idx < (sizeof(atCmds)/sizeof(atCmd_t))){
      return atCmds[idx].timeout;
   }

   return 0;
}

bool gsmUrcSearch(uint8_t const * const urc)
{
   uint8_t i = 0;

   for(i = 0; i < (sizeof(urcs)/sizeof(urc_t)); i++){
      if(0 == strcmp(urc, urcs[i].name)){return true;}
   }

   return false;
}

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
