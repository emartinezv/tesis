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

/** @brief sysUpdate period in ms */

#define DELAY_SYSUPD 10;

/*==================[typedef]================================================*/

/** @brief State of the current formula being run */

typedef enum {
   IDLE = 0, /**< no formula currently running*/
   INIT = 1, /**< initializing formula */
   PROC = 2, /**< running commands */
   WRAP = 3 /**< wrapping up formula and callback */
} frmStatus;

/** @brief Number of command in the PROC section being executed */

typedef enum {
   NOCMD,
   ATCMD1,
   ATCMD1RESP,
   ATCMD2,
   ATCMD2RESP,
   ATCMD3,
   ATCMD3RESP,
   ATCMD4,
   ATCMD4RESP,
   ATCMD5,
   ATCMD5RESP,
   ATCMD6,
   ATCMD6RESP,
   ATCMD7,
   ATCMD7RESP,
   ATCMD8,
   ATCMD8RESP,
   ATCMD9,
   ATCMD9RESP,
   ATCMD10,
   ATCMD10RESP
} runStatus;

/** @brief Type for formula error code return */

typedef enum {
   OK,         /**< no errors */
   ERR_INIT,   /**< error during initialization */
   ERR_PROC,   /**< error during command processing */
   ERR_GSM,    /**< error from the GSM engine */
   ERR_WRAP    /**< error during wrap process */
} error_frm;

/** @brief Type for command error code return */

typedef struct {
   uint8_t command[20];     /**< error command */
   uint8_t parameter[150];  /**< error parameter */
} error_cmd;

/** @brief Type for user error reporting*/

typedef struct {
   error_frm error_formula;     /**< formula level error */
   error_cmd error_command;     /**< command level error */
} error_user;

/*---------------------------------------------------------------------------*/
/*            Data structures for the ciaaMobile_sendSMS function            */
/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/
/*           Data structures for the ciaaMobile_listRecSMS function          */
/*---------------------------------------------------------------------------*/

/** @brief Type for SMS read return */

typedef struct {
   uint8_t meta[150]; /**< metadata */
   uint8_t text[150]; /**< text of the SMS message */
} SMS_rec;

/*---------------------------------------------------------------------------*/
/*             Data structures for the ciaaMobile_delSMS function            */
/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/
/*             Data structures for the ciaaMobile_startGPRS function         */
/*---------------------------------------------------------------------------*/

/** @brief Type for APN name, user and password */

typedef struct {
   uint8_t * apn;   /**< APN address */
   uint8_t * user;  /**< User */
   uint8_t * pwd;   /**< Password */
} APN_usr_pwd;

/*---------------------------------------------------------------------------*/
/*            Data structures for the ciaaMobile_openPort function           */
/*---------------------------------------------------------------------------*/

typedef enum {
   TCP,         /**< TCP port */
   UDP          /**< UDP port */
} portType_e;

/** @brief Type for address and port */

typedef struct {
   portType_e type;      /**< TCP or UDP port */
   uint8_t *  address;   /**< TCP address (domain or IP) */
   uint8_t *  port;      /**< TCP port number as a string */
} port_s;

/*---------------------------------------------------------------------------*/
/*       Data structures for the ciaaMobile_getSignalQuality function        */
/*---------------------------------------------------------------------------*/

/** @brief Type for signal quality */

typedef struct {
   int8_t  rssi;    /**< RSSI in dBm */
   uint8_t ber;     /**< BER as RXQUAL values in the table in GSM 05.08 [20]
                       subclause 7.2.4 */
} signal_quality_s;

/*==================[external data declaration]==============================*/

/** @brief used for AT command timeout counter */

extern uint32_t timeout_count;

/*==================[external functions declaration]=========================*/

/** @brief Handles AT command timeout and sysUpdate timing
*
* @return
*/

void ciaaMobile_SysTick_Handler (void);

/** @brief Sends an SMS
*
* @param msg Pointer to SMS message to be sent
* @param cback Function pointer to callback function
*
* @return
*/

void ciaaMobile_sendSMS (SMS_send * msg, void * (*cback) (error_user, void *));

/** @brief Lists received SMSs in a vector
*
* @param list  Pointer to storage vector for SMSs to be read
* @param noMsg Number of messages to read (must be <= the size of list)
* @param cback Function pointer to callback function
*
* @return
*/

void ciaaMobile_listRecSMS (SMS_rec * list, uint8_t noMsg, void * (*cback) (error_user, void *));

/** @brief Deletes a single SMS from memory
*
* @param msgdel Pointer to index and mode for the SMS deletion command
* @param cback  Function pointer to callback function
*
* @return
*/

void ciaaMobile_delSMS (SMS_del * msgdel, void * (*cback) (error_user, void *));

/** @brief Starts up GPRS connection in the modem
*
* @param APN    Pointer to struct containing APN, user and password
* @param cback  Function pointer to callback function
*
* @return
*/

void ciaaMobile_startGPRS (APN_usr_pwd * APN, void * (*cback) (error_user, void *));

/** @brief Opens TCP or UDP port
*
* @param port      Pointer to struct containing the type of port, IP address and port #
* @param cback     Function pointer to callback function
*
* @return
*/

void ciaaMobile_openPort (port_s * port, void * (*cback) (error_user, void *));

/** @brief Closes open TCP or UDP port
*
* @param cback     Function pointer to callback function
*
* @return
*/

void ciaaMobile_closePort (void * (*cback) (error_user, void *));

/** @brief Gets signal quality values (RSSI and BER)
*
* @param signal_quality Pointer to signal quality struct
* @param cback          Function pointer to callback function
*
* @return
*/

void ciaaMobile_getSignalQuality (signal_quality_s * signal_quality, void * (*cback) (error_user, void *));

/** @brief ciaaMobile_startUp function
*
* @param cback  Function pointer to callback function
*
* @return
*/

void ciaaMobile_startUp (void * (*cback) (error_user, void *));

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
