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

#ifndef _AT_COMMMAN_H_
#define _AT_COMMMAN_H_

/** \addtogroup uart Bare-metal uart example
 ** @{ */

/*==================[inclusions]=============================================*/

#include "lpc_types.h"
#include "AT_parser.h"
#include "string.h"
#include "ciaaUART.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

#define MAX_COMM_LEN 15 /* maximum length of a command */
#define MAX_COMM 5 /* maximum ammount of loaded commands */
#define MAX_RESP_LEN 15 /* maximum length of a response */
#define MAX_RESP 5 /* maximum ammount of loaded responses */

/*==================[typedef]================================================*/

/** typedef for callback function pointer */

typedef int (*ATCallback) (const uint8_t const * parameter);

/** struct for AT commands */

typedef struct {
   uint8_t name[MAX_COMM_LEN];
   ATCallback execution;
   ATCallback write;
   ATCallback test;
   ATCallback read;
} ATComm;

/** struct for AT responses */

typedef struct {
   uint8_t name[MAX_RESP_LEN];
   ATCallback response;
} ATResp;

/** struct for URC */

typedef struct {
   uint8_t name[MAX_RESP_LEN];
   ATCallback event;
} ATURC;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** @brief commSearch function
 * @return takes a token type and command, returns position of the command in
 *  the known command/response vector or 0 if no command/response found
 */

uint8_t commSearch(ATToken const type, uint8_t const * const command);

/** @brief commInit function
 * @return loads the command and response vectors with the name and callback
 *  for each command or response
 */

void commInit (void);

/** @brief ccAT function
 * @return callback function for the "AT" command
 */

int cbAT (uint8_t const * const parameter);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef _MAIN_H_ */