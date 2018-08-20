/* Copyright 2018, Ezequiel Martinez Vazquez
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

/** \addtogroup gsm
 ** @{ */

/*==================[inclusions]============================================*/

#include "lpc_types.h"
#include "string.h"
#include "ciaaUART.h"
#include "vl_ring_buffer.h"

/*==================[cplusplus]=============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]================================================*/

/** @brief Maximum size of tokens */
#define TKN_LEN 300

/** @brief Size in bytes of the UART swap buffer */
#define SWAP_BUF_SIZ (TKN_LEN)

/** @brief Size in bytes of the read character buffer (must be power of 2) */
#define RD_BUF_SIZ 512

/** @brief Size in bytes of the current token buffer (must be power of 2) */
#define CURR_TKN_BUF_SIZ 512

/*==================[typedef]===============================================*/

/** @brief Token type enum for the tokenizer functionality */

typedef enum {
   NONE,    /**< No token detected yet */
   ECHO,    /**< AT command echo */
   RESP,    /**< AT command response */
   DATAB,   /**< DATA block */
   SMSIN,   /**< SMS send prompt */
   SMSBOD   /**< SMS send body */
}
tknType_e;

/*==================[external data declaration]=============================*/

/*==================[external functions declaration]========================*/

/** @brief Initializes tokenizer
*
*/

void gsmInitTokenizer(void);

/** @brief Get characters from the UART ring buffer into a local buffer, cycle
 *         though it and send detected tokens to the token VL ring buffer
*
*  @param  tknVlRb   : Pointer to the token VL ring buffer
*
*/

void gsmDetectTkns(VLRINGBUFF_T * tknVlRb);

/*==================[cplusplus]=============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]===========================================*/
#endif
