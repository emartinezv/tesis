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

#ifndef _GSM_PARSER_H_
#define _GSM_PARSER_H_

/** \addtogroup parser parser
 ** @{ */

/*==================[inclusions]=============================================*/

#include "lpc_types.h"
#include "string.h"
#include "gsmComms.h"
#include "gsmTokenizer.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/** @brief Type of AT token as per the parse function; used to feed the cmd FSM
 */

typedef enum {INVALID,        /**< Token is invalid */
              AUTOBAUD,       /**< Token is the autobauding sequence */
              BASIC_CMD,      /**< Token is a basic command */
              BASIC_CMD_AMP,  /**< Token is a basic command with ampersand */
              EXT_CMD_TEST,   /**< Token is a extended test command */
              EXT_CMD_WRITE,  /**< Token is a extended test command */
              EXT_CMD_READ,   /**< Token is a extended test command */
              EXT_CMD_EXEC,   /**< Token is a extended test command */
              SMS_BODY_P,     /**< Token is the body of an SMS */
              BASIC_RSP,      /**< Token is a basic response */
              DATA_BLOCK_P,   /**< Token is a DATA block from a response */
              SMS_PROMPT_P,   /**< Token is the SMS prompt */
              EXT_RSP,        /**< Token is an extended response */
              TIMEOUT,        /**< Dummy value for timeout events */
              }tknTypeParser_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** @brief AT token parser
*
*  @param tkn    : Pointer to AT token (processed by tokenizer)
*  @param cmd    : AT command buffer
*  @param par    : AT parameter buffer
*  @param tknLen : Length of the received token
*
*  @return Type of AT token in an tknTypeParser_t enum, as well as
*          the command and parameter parts in their corresponding buffers
*/

tknTypeParser_t gsmParseTkn(uint8_t const * const tkn, uint8_t * cmd,
                            uint8_t * par, uint16_t tknLen);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef _GSM_PARSER_H_ */
