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

#ifndef _GSM_COMMANDS_H_
#define _GSM_COMMANDS_H_

/** @brief This module handles the AT command catalog and URC catalog, as well
 *         as storage and search functions for both.
 */

/** \addtogroup commands commands
 ** @{ */

/*==================[inclusions]=============================================*/

#include "lpc_types.h"
#include "string.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/** @brief Value returned by gsmCmdSearch when cmd is not found */
#define UNKNOWN_CMD 65535

/** @brief Default timeout for AT commands in ms */

#define TOUT_DEF 100   /* SIMCOM modules do not specify a default timeout
                             period, but looking at other manufacturers the
                             suggested value seems to be around 100 ms */

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** @brief Searches for a cmd and returns its index
 *
 *  @param cmd : Pointer to str with the name of the command to search
 *
 *  @return Index number of the cmd (UNKNOWN_CMD if unknown)
 */

uint16_t gsmCmdSearch(uint8_t const * const cmd);

/** @brief Given a cmd index, returns the string of successful close responses
 *         for the corresponding command
 *
 *  @param idx : Index of the command as reported by gsmCmdSearch
 *
 *  @return String of successful close responses
 */

const uint8_t const * gsmGetCmdSucRsp (uint16_t idx);

/** @brief Given a cmd index, returns the string of error close responses for
 *         the corresponding command
 *
 *  @param idx : Index of the command as reported by gsmCmdSearch
 *
 *  @return String of error close responses
 */

const uint8_t const * gsmGetCmdErrRsp (uint16_t idx);

/** @brief Given a cmd index, returns the timeout in ms for the corresponding
 *         command
 *
 *  @param idx : Index of the command as reported by gsmCmdSearch
 *
 *  @return Timeout in ms
 */

uint32_t gsmGetCmdTimeout (uint16_t idx);

/** @brief Returns true if the urc is recognized
 *
 *  @param urc : Pointer to str with the name of the URC to search
 *
 *  @return True if URC is recognized
 */

bool gsmUrcSearch(uint8_t const * const urc);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef _GSM_COMMANDS_H_ */
