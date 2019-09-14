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

#ifndef _GSM_COMMS_H_
#define _GSM_COMMS_H_

/** @brief This module handles UART communications.
 */

/** \addtogroup comms comms
 ** @{ */

/*==================[inclusions]============================================*/

#include "lpc_types.h"
#include "string.h"

/* Add the corresponding 232 UART and terminal UART HAL .h files here */

#include "ciaaUART.h"

/*==================[cplusplus]=============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]================================================*/

/*==================[typedef]===============================================*/

/*==================[external data declaration]=============================*/

/*==================[external functions declaration]========================*/

/** @brief Get at most n characters from the 232 UART into a local buffer
 *
 *  @param buffer : Pointer to the reception char buffer
 *  @param n      : Number of characters to get
 *
 *  @return Number of chars actually read
 *
 */

int gsm232UartRecv (uint8_t * const buffer, int n);

/** @brief Send at most n characters to the 232 UART from a local buffer
*
*  @param  buffer : Pointer to the reception char buffer
*  @param  n      : Number of characters to send
*
*  @return Number of chars actually written
*
*/

int gsm232UartSend (uint8_t const * const buffer, int n);

/** @brief Get at most n characters from the terminal UART into a local buffer
 *
 *  @param buffer : Pointer to the reception char buffer
 *  @param n      : Number of characters to get
 *
 *  @return Number of chars actually read
 *
 */

int gsmTermUartRecv (uint8_t * const buffer, int n);

/** @brief Send at most n characters to the terminal UART from a local buffer
*
*  @param  buffer : Pointer to the reception char buffer
*  @param  n      : Number of characters to send
*
*  @return Number of chars actually written
*
*/

int gsmTermUartSend (uint8_t const * const buffer, int n);

/*==================[cplusplus]=============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]===========================================*/
#endif
