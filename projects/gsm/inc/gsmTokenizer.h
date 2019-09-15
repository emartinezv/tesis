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

#ifndef _GSM_TOKENIZER_H_
#define _GSM_TOKENIZER_H_

/** @brief This module handles the tokenizer function, turning the raw serial
 *         input from the modem into discrete tokens
 */

/** \addtogroup tokenizer tokenizer
 ** @{ */

/*==================[inclusions]============================================*/

#include "lpc_types.h"
#include "string.h"
#include "gsmComms.h"
#include "vl_ring_buffer.h"

/*==================[cplusplus]=============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]================================================*/

/** @brief Maximum size of tokens */
#define TKN_LEN 300

/** @brief Size in bytes of the UART swap buffer */
#define SWAP_BUF_SIZ TKN_LEN

/** @brief Size in bytes of the current token buffer (must be power of 2) */
#define CURR_TKN_BUF_SIZ 512

/*==================[typedef]===============================================*/

/** @brief Token type enum for the tokenizer functionality */

typedef enum {
   NONE       = 0,   /**< No token detected yet */
   ECHO       = 1,   /**< AT command echo */
   RSP        = 2,   /**< AT command response */
   DATA_BLOCK = 3,   /**< DATA block */
   SMS_PROMPT = 4,   /**< SMS send prompt */
   SMS_BODY   = 5,   /**< SMS send body */
}
tknTypeTknzer_t;

/*==================[external data declaration]=============================*/

/*==================[external functions declaration]========================*/

/** @brief Initializes tokenizer and it's RB
 *
 *  @return True if successful
 */

bool gsmInitTokenizer(void);

/** @brief Determines how many chars can be sent to the tokenizer at this time
 *
 *  @return Number of chars the tokenizer can accommodate right now
 */

uint16_t gsmNoChTokenizer(void);

/** @brief Get characters from input buffer into the current tkn ring buffer,
 *         cycle though it and send detected tokens to the output token VL
 *         ring buffer
 *
 *  @param tknVlRb : Pointer to the output token VL ring buffer
 *  @param nch     : Number of characters in input buffer
 *  @param buffer  : Pointer to input buffer
 *
 */

void gsmDetectTkns(VLRINGBUFF_T * tknVlRb, uint16_t nch,
                   uint8_t const * const buffer);

/*==================[cplusplus]=============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]===========================================*/
#endif
