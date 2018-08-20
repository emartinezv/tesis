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

#ifndef _GSM_COMMANDS_H_
#define _GSM_COMMANDS_H_

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

 /** @brief Default timeout for AT commands in ms */

#define TOUT_DEF 40 // PARA TESTING, USAR 20 ms COMO VALOR ESTANDAR

/*==================[typedef]================================================*/

/** @brief Used for the internal catalog of AT commands; stores the name and
 *         valid end responses */

typedef struct {
   uint8_t const * const name;     /**< pointer to str with command name */
   uint8_t const * const sucResp;  /**< pointer to str with successful end responses */
   uint8_t const * const errResp;  /**< pointer to str with error end responses */
   uint32_t timeout;                /**< command timeout in ms */
} ATComm;

/*==================[external data declaration]==============================*/

extern ATComm const commands [];

extern uint8_t const * const URCs [];

/*==================[external functions declaration]=========================*/

/** @brief Searches for a cmd and returns it's position in the commands vector
 *
 *  @param command Pointer to str with the name of the command to search
 *
 *  @return Position of the cmd in the commands vector (65535 if unknown)
 */

uint16_t commSearch(uint8_t const * const command);

/** @brief Searches for an URC and returns 1 if recognized, 0 if absent
 *
 *  @param  urc Pointer to str with the name of the URC to search
 *
 *  @return 1 if URC recognized, 0 if absent
 */

uint8_t URCSearch(uint8_t const * const urc);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef _GSM_COMMANDS_H_ */
