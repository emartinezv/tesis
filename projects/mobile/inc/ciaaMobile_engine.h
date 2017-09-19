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

#ifndef _CIAAMOBILE_ENGINE_H_
#define _CIAAMOBILE_ENGINE_H_

/** \addtogroup uart Bare-metal uart example
 ** @{ */

/*==================[inclusions]=============================================*/

#include "lpc_types.h"
#include "string.h"
#include "ciaaUART_T.h"
#include "ciaaMobile_parser.h"
#include "ciaaMobile_commands.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

typedef enum _GSMstates
{
   WAITING = 0,  /* waiting for a command */
   CMD_SENT = 1, /* cmd sent through serial port */
   CMD_ACK = 2,  /* cmd echo confirmed */
} GSMstate;

typedef enum _cmdState
{
   cmdWait = 0,   /* waiting for command */
   cmdProc = 1,   /* command being processed */
   cmdClosed = 2, /* command closed succesfully */
   cmdError = 3,  /* command closed due to error */
} cmdState;

typedef enum { AUTOBAUD, BASIC_STD, BASIC_AMP, EXT_TEST, EXT_WRITE, EXT_READ,
               EXT_EXEC, SMS } ATcmdType;

typedef struct _ATcmd
{
   uint8_t * cmd;
   uint8_t * par;
   ATcmdType type;
} ATcmd;

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** @brief processToken function
* @return
*/
cmdState processToken(void);

/** @brief sendATcmd function
* @return
*/
void sendATcmd (const uint8_t * cmd, const uint8_t * par, const ATcmdType);

/** @brief getCmdResp function
* @return
*/
uint8_t * getCmdResp (uint8_t index);

/** @brief getNoCmdResp function
* @return
*/
uint8_t getNoCmdResp (void);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef _MAIN_H_ */
