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

#ifndef _CIAAMOBILE_INTERFACE_H_
#define _CIAAMOBILE_INTERFACE_H_

/** \addtogroup interface interface
 ** @{ */

/*==================[inclusions]=============================================*/

#include "lpc_types.h"
#include "string.h"
#include "stdio.h"
#include "ciaaUART_T.h"
#include "ciaaMobile_engine.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

/** @brief State of the current formula being run */

typedef enum {
   IDLE = 0, /**< no formula currently running*/
   INIT = 1, /**< initializing formula */
   PROC = 2, /**< running commands */
   WRAP = 3, /**< wrapping up formula and callback */
} frmStatus;

/** @brief Type for SMS message */

typedef struct {
   uint8_t * dest; /**< destination number as a str */
   uint8_t * text; /**< text of the sms message */
} SMS_send;

/** @brief Type for SMS send return */

typedef struct {
   uint8_t cmgl;   /**< CMGL code returned */
   uint8_t result; /**< result of the send attempt */
} SMS_send_ret;

/** @brief Type for SMS read return */

typedef struct {
   uint8_t meta[150]; /**< metadata */
   uint8_t text[150]; /**< text of the SMS message */
} SMS_rec;

/** @brief Type for SMS deletion command */

typedef struct {
   uint8_t index; /**< index of the message in memory */
   uint8_t mode;  /**< deletion mode
                   *   0 del the message indicated by index
                   *   1 del all read messages (ignores index)
                   *   2 del all read and sent messages (ignores index)
                   *   3 del all read, sent and unsent messages (ignores index)
                   *   4 del all messages (ignores index)
                   */
} SMS_del;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** @brief Sends an SMS
*
* @param msg Pointer to SMS message to be sent
* @param cback Function pointer to callback function
*
* @return
*/

void ciaaMobile_sendSMS (SMS_send * msg, void * (*cback) (void *));

/** @brief Lists received SMSs in a vector
*
* @param list Pointer to storage vector for SMSs to be read
* @param cback Function pointer to callback function
*
* @return
*/

void ciaaMobile_listRecSMS (SMS_rec * list, void * (*cback) (void *));

/** @brief Deletes a single SMS from memory
*
* @param msgdel Pointer to index and mode for the SMS deletion command
* @param cback  Function pointer to callback function
*
* @return
*/

void ciaaMobile_delSMS (SMS_del * msgdel, void * (*cback) (void *));

/** @brief ciaaMobile_startUp function
* @return
*/

/** @brief Starts up the GSM engine

*/

void ciaaMobile_startUp (void);

/** @brief Indicates if the GSM engine is currently idle
 *
 * @return Returns 1 if no formula is being run, 0 otherwise
*/

uint8_t ciaaMobile_isIdle (void);

/** @brief Updates the GSM engine
*
*/

void ciaaMobile_sysUpdate (void);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef _CIAAMOBILE_INTERFACE_H_ */
