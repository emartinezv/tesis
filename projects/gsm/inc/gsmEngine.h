/* Copyright 2019, Ezequiel Martinez Vazquez
 * All rights reserved.
 *
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

#ifndef _GSM_ENGINE_H_
#define _GSM_ENGINE_H_

/** @brief This module handles the internal engine of the library, including
 *         the different states of the modem and the execution of commands.
 */

/** \addtogroup engine engine
 ** @{ */

/*==================[inclusions]=============================================*/

#include "lpc_types.h"
#include "string.h"
#include "gsmComms.h"
#include "gsmParser.h"
#include "gsmCommands.h"
#include "gsmTokenizer.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/** @brief Size in bytes of the tknVlRb buffer */
#define TKN_BUF_SIZE 1024

/** @brief Size in bytes of the rspVlRb buffer */
#define RSP_BUF_SIZE 1024

/** @brief Size in bytes of the urcVlRb buffer */
#define URC_BUF_SIZE 256

/** @brief Max size in chars of the AT+CGNSINF response */
#define MAX_CGNSINF_SIZE 94

/** @brief Size in bytes of the gsmProcessTkn read buffer */
#define RD_BUF_SIZ TKN_LEN

/*==================[typedef]================================================*/

/** @brief AT command response type */

typedef struct _rsp
{
   uint8_t cmd[TKN_CMD_SIZE];
   uint8_t par[TKN_PAR_SIZE];
} rsp_t;

/** @brief Modes for the serial port */

typedef enum _serialMode
{
   COMMAND_MODE,
   DATA_MODE
} serialMode_t ;

/** @brief State of the engine FSM */

typedef enum _fsmState
{
   WAITING = 0,  /**< waiting for a command */
   CMD_SENT = 1, /**< cmd sent through serial port */
   CMD_ACK = 2,  /**< cmd echo confirmed */
} fsmState_t;

/** @brief State of the current command */

typedef enum _fsmEvent
{
   NO_UPDATE,         /**< no change to FSM */
   OK_CMD_SENT,       /**< command sent */
   OK_CMD_ACK,        /**< command acknowledged */
   OK_RSP,            /**< non-closing response received */
   OK_URC,            /**< URC processed */
   OK_CLOSE,          /**< successful closing response received */
   ERR_CMD_UKN,       /**< unknown command sent */
   ERR_CMD_ECHO,      /**< cmd echo erroneous or missing */
   ERR_OOO,           /**< out of order response received */
   ERR_TKN_INV,       /**< token invalid */
   ERR_MSG_CLOSE,     /**< error message from modem received */
   ERR_TIMEOUT,       /**< AT command timeout */
   ERR_FSM_OOR        /**< FSM out of range */
} fsmEvent_t;

/** @brief GSM engine type */

typedef struct _gsmEngine
{
   /* State */

   fsmState_t fsmState;
   serialMode_t serialMode;
   uint8_t currCmd[TKN_CMD_SIZE]; /* command being currently executed */
   uint8_t currPar[TKN_PAR_SIZE]; /* parameter of the current command */
   uint8_t currIdx;               /* save index of current command */

   /* Counters */

   uint32_t toutCnt;

   /* VL ring buffers */

   VLRINGBUFF_T tknVlRb;
   VLRINGBUFF_T rspVlRb;
   VLRINGBUFF_T urcVlRb;

   /* Ring buffers */

   RINGBUFF_T tknRb;
   RINGBUFF_T rspRb;
   RINGBUFF_T urcRb;

   /* Ring buffer storage arrays */

   uint8_t tknRbBuf[TKN_BUF_SIZE];
   uint8_t rspRbBuf[RSP_BUF_SIZE];
   uint8_t urcRbBuf[URC_BUF_SIZE];

} gsmEngine_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** @brief Initializes the engine
*
*  @param engine : Pointer to engine
*
*  @return True if successful
*/

bool gsmInitEngine(gsmEngine_t * const engine);

/** @brief Processes a single token
*
*  @param engine : Pointer to engine
*
*  @return Event triggered by the gsmUpdateFsm call
*/

fsmEvent_t gsmProcessTkn(gsmEngine_t * const engine);

/** @brief Checks if timeout counter has reached zero
*
*  @param engine : Pointer to engine
*
*  @return True if the engine toutCnt == 0
*
*/

bool gsmToutCntZero(gsmEngine_t * const engine);

/** @brief Decrements the timeout counter
*
*  @param engine : Pointer to engine
*
*/

void gsmDecToutCnt(gsmEngine_t * const engine);

/** @brief Sends an AT command to the GSM module
*
*  @param engine : Pointer to engine
*  @param cmdStr : AT command string including parameters
*
*  @return Event triggered by the gsmUpdateFsm call
*/

fsmEvent_t gsmSendCmd (gsmEngine_t * const engine,
                       const uint8_t * const cmdStr);

/** @brief Gets command response from the response VLRB (FIFO)
*
*  @param engine : Pointer to engine
*
*  @return Response ('\0' in both cmd and par if no responses left)
*/

rsp_t gsmGetCmdRsp (gsmEngine_t * const engine);

/** @brief Gets the number of responses generated by the current command
*
*  @param  engine : Pointer to engine
*
*  @return Number of responses generated by the command
*/

uint8_t gsmGetNoCmdRsp (const gsmEngine_t * const engine);

/** @brief Gets URC from the URC VLRB (FIFO)
*
*  @param  engine : Pointer to engine
*
*  @return URC ('\0' in both cmd and par if no URCs left)
*/

rsp_t gsmGetUrc (gsmEngine_t * const engine);

/** @brief Returns the current mode of the serial port
*
*  @param  engine : Pointer to engine
*
*  @return COMMAND_MODE or DATA_MODE
*/

serialMode_t gsmGetSerialMode(const gsmEngine_t * const engine);

/** @brief Changes the mode of the serial port
*
*  @param mode   : Serial port mode
*  @param engine : Pointer to engine
*
*  @return True if successful
*
*/

bool gsmSetSerialMode(gsmEngine_t * const engine, serialMode_t mode);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif
