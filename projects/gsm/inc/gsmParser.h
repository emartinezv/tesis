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

#ifndef _GSM_PARSER_H_
#define _GSM_PARSER_H_

/** \addtogroup parser parser
 ** @{ */

/*==================[inclusions]=============================================*/

#include "lpc_types.h"
#include "string.h"
#include "ciaaUART.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/



/*==================[typedef]================================================*/

/** @brief Type of AT token as per the parse function; used to feed the cmd FSM
 */

typedef enum {INVALID,        /**< token is invalid */
              AUTOBAUD,       /**< token is the autobauding sequence */
              BASIC_CMD,      /**< token is a basic command */
              BASIC_CMD_AMP,  /**< token is a basic command with ampersand */
              EXT_CMD_TEST,   /**< token is a extended test command */
              EXT_CMD_WRITE,  /**< token is a extended test command */
              EXT_CMD_READ,   /**< token is a extended test command */
              EXT_CMD_EXEC,   /**< token is a extended test command */
              SMS_BODY,       /**< token is the body of an SMS */
              BASIC_RSP,      /**< token is a basic response */
              DATA,           /**< token is a DATA block from a response */
              SMS_PROMPT,     /**< token is the SMS prompt */
              EXT_RSP,        /**< token is an extended response */
              TIMEOUT,        /**< dummy value for timeout events */
              }ATToken;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** @brief AT token parser
*
*  @param token       Pointer to AT token fetched from the serial port tokenizer
*  @param command     AT command buffer
*  @param parameter   AT parameter buffer
*  @param tknlen      length of the received token
*
*  @return Returns the type of AT token in an ATToken enum, as well as the
*          command and parameter parts in their corresponding buffers
*/

ATToken parse(uint8_t const * const token, uint8_t * command, uint8_t * parameter, int tknlen);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef _GSM_PARSER_H_ */
