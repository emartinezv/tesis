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

#ifndef _GSM_ENGINE_H_
#define _GSM_ENGINE_H_

/** \addtogroup gsm
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

/** @brief Max size of an AT token command part */
#define TKN_CMD_SIZE 20

/** @brief Max size of an AT token parameter part */

#define TKN_PAR_SIZE 300 /* Max size is usually determined by SMS message
                            length which is topped at 160 chars with the usual
                            GSM-7 coding. However, if using UCS2 coding the max
                            number of chars will be 67, but they come out of
                            the modem coded at a 4-to-1 rate, which means a max
                            of 268 effective chars to store.

/** @brief Max size in chars of the AT+CGNSINF response */
#define MAX_CGNSINF_SIZE 94

/** @brief AT command response type */
typedef struct _rsp_t
{
   uint8_t cmd[TKN_CMD_SIZE];
   uint8_t par[TKN_PAR_SIZE];
} rsp_t;

/** @brief Modes for the serial port */

typedef enum _serialMode_e
{
   COMMAND_MODE,
   DATA_MODE
} serialMode_e ;

/** @brief State of the engine FSM */

typedef enum _fsmState_e
{
   WAITING = 0,  /**< waiting for a command */
   CMD_SENT = 1, /**< cmd sent through serial port */
   CMD_ACK = 2,  /**< cmd echo confirmed */
} fsmState_e;

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
} fsmEvent_e;

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** @brief Initializes the library engine
*
*/

void gsmInitEngine(void);


/** @brief Processes a single token
*
*  @return Returns the event triggered by the gsmUpdateFsm call
*/

fsmEvent_e gsmProcessTkn(void);

/** @brief Checks if timeout counter has reached zero
*
*/

uint8_t gsmToutCntZero(void);

/** @brief Decrements the timeout counter
*
*/

void gsmDecToutCnt(void);

/** @brief Prints data to the USB UART when serial port is in DATA_MODE
*
*/

void gsmPrintData(void);

/** @brief Sends an AT command to the GSM module
*
*  @return Returns the event triggered by the gsmUpdateFsm call
*
*  @param cmdStr AT command string including parameters
*/

fsmEvent_e gsmSendCmd (const uint8_t * cmdStr);

/** @brief Gets the latest command response
*
*  @return Returns the required response (0 if no such response)
*/

rsp_t gsmGetCmdRsp (void);

/** @brief Gets the number of responses generated by the command
*
*  @return Returns the number of responses generated by the command
*/

uint8_t gsmGetNoCmdRsp (void);

/** @brief Reads the oldest URC event in the URC VLRB
*
*  @return Returns the required URC (0 if no such response)
*/

rsp_t gsmGetUrc (void);

/** @brief Returns the actual mode of the serial port
*
*
*  @return Returns COMMAND_MODE or DATA_MODE
*/

serialMode_e gsmGetSerialMode(void);

/** @brief Changes the mode of the serial port
*
*  @param mode   : Serial port mode
*
*/

void gsmSetSerialMode(serialMode_e mode);

/** @brief Writes data to the RS-232 port when in DATA_MODE
*
*  @param write  : Pointer to data being written
*  @param size   : Number of chars to be written
*
*  @return       : Number of chars effectively written
*
*/

uint8_t gsmWriteData(uint8_t const * const write, uint8_t size);

/** @brief Reads data from the RS-232 port when in DATA_MODE
*
*  @param read   : Pointer to data buffer for read data
*  @param size   : Number of chars to be read at max
*
*  @return       : Number of chars effectively read
*
*/

uint8_t gsmReadData(uint8_t * const read, uint8_t size);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif
