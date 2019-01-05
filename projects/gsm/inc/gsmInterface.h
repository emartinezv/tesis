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

#ifndef _GSM_INTERFACE_H_
#define _GSM_INTERFACE_H_

/** \addtogroup interface interface
 ** @{ */

/*==================[inclusions]=============================================*/

#include "lpc_types.h"
#include "string.h"
#include "stdio.h"
#include "ciaaUART.h"
#include "gsmEngine.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/** @brief sysUpdate period in ms */

#define DELAY_SYSUPD 10;

/*==================[typedef]================================================*/

/** @brief State of the current formula being run */

typedef enum _frmStatus_e {
   IDLE = 0, /**< no formula currently running*/
   INIT = 1, /**< initializing formula */
   PROC = 2, /**< processing commands */
   WRAP = 3  /**< wrapping up formula and callback */
} frmStatus_e;

/** @brief Number of command or response in the PROC section being processed */

typedef enum _procStatus_e {
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
} procStatus_e;

/** @brief Type for formula error code return */

typedef enum _errorFrm_e {
   OK,         /**< no errors */
   ERR_INIT,   /**< error during initialization */
   ERR_PROC,   /**< error during command processing */
   ERR_GSM,    /**< error from the GSM engine */
   ERR_WRAP    /**< error during wrap process */
} errorFrm_e;

/** @brief Type for command error code return */

typedef struct _errorCmd_s {
   uint8_t cmd[TKN_CMD_SIZE];     /**< error command */
   uint8_t par[TKN_PAR_SIZE];     /**< error parameter */
} errorCmd_s;

/** @brief Type for user error reporting*/

typedef struct _errorUser_s {
   errorFrm_e errorFrm;     /**< formula level error */
   errorCmd_s errorCmd;     /**< command level error */
} errorUser_s;

/*---------------------------------------------------------------------------*/
/*            Data structures for the ciaaMobile_sendSMS function            */
/*---------------------------------------------------------------------------*/

/** @brief Type for outgoing SMS message */

typedef struct _smsOut_s {
   uint8_t * dest; /**< destination number as a str */
   uint8_t * text; /**< text of the sms message */
} smsOut_s;

/** @brief Type for outgoing SMS confirmation message */

typedef struct _smsConf_s {
   uint8_t cmgl;   /**< CMGL code returned */
   uint8_t result; /**< result of the send attempt */
} smsConf_s;

/*---------------------------------------------------------------------------*/
/*  Data structures for the ciaaMobile_listRecSMS and ciaaMobile_readRecSMS  */
/*  functions                                                                */
/*---------------------------------------------------------------------------*/

/** @brief Type for SMS read mode */

typedef enum _smsRdMode_e {
   NORMAL = 0,     /**< change status to read if message unread */
   NOCHANGE = 1,   /**< do not change status of message */
} smsRdMode_e;

/** @brief Type for SMS read parameters */

typedef struct _smsRdPars_s {
   uint8_t idx;        /**< SMS message index in memory */
   smsRdMode_e mode; /**< SMS read mode (change status or leave as is) */
} smsRdPars_s;

/** @brief Type for SMS record */

typedef struct _smsRec_s {
   uint8_t meta[TKN_PAR_SIZE/2]; /**< metadata */
   uint8_t text[TKN_PAR_SIZE];   /**< text of the SMS message */
} smsRec_s;

/** @brief Type for SMS printout */

typedef struct _smsPrint_s {
   uint8_t noMsg;      /**< number of messages */
   smsRec_s * firstMsg; /**< pointer to the first message */
} smsPrint_s;

/*---------------------------------------------------------------------------*/
/*             Data structures for the ciaaMobile_delSMS function            */
/*---------------------------------------------------------------------------*/

/** @brief Type for SMS deletion command */

typedef struct _smsDel_s {
   uint8_t idx;   /**< index of the message in memory */
   uint8_t mode;  /**< deletion mode
                   *   0 del the message indicated by index
                   *   1 del all read messages (ignores index)
                   *   2 del all read and sent messages (ignores index)
                   *   3 del all read, sent and unsent messages (ignores index)
                   *   4 del all messages (ignores index)
                   */
} smsDel_s;

/*---------------------------------------------------------------------------*/
/*             Data structures for the ciaaMobile_startGPRS function         */
/*---------------------------------------------------------------------------*/

/** @brief Type for APN name, user and password */

typedef struct _apnUserPwd_s {
   uint8_t * apn;   /**< APN address */
   uint8_t * user;  /**< User */
   uint8_t * pwd;   /**< Password */
} apnUserPwd_s;

/*---------------------------------------------------------------------------*/
/*            Data structures for the ciaaMobile_openPort function           */
/*---------------------------------------------------------------------------*/

typedef enum _portType_e {
   TCP,         /**< TCP port */
   UDP          /**< UDP port */
} portType_e;

/** @brief Type for address and port */

typedef struct _port_s {
   portType_e type;      /**< TCP or UDP port */
   uint8_t *  address;   /**< TCP address (domain or IP) */
   uint8_t *  port;      /**< TCP port number as a string */
} port_s;

/*---------------------------------------------------------------------------*/
/*       Data structures for the ciaaMobile_getSignalQuality function        */
/*---------------------------------------------------------------------------*/

/** @brief Type for signal quality */

typedef struct _sigQual_s {
   int8_t  rssi;    /**< RSSI in dBm */
   uint8_t ber;     /**< BER as RXQUAL values in the table in GSM 05.08 [20]
                       subclause 7.2.4 */
} sigQual_s;

/*---------------------------------------------------------------------------*/
/*           Data structures for the ciaaMobile_powerGNSS function           */
/*---------------------------------------------------------------------------*/

/** @brief Enum for turning GNSS module on or off */

typedef enum _pwrGnss_e {
   ON,    /**< Power GNSS module on */
   OFF    /**< Power GNSS module off */
} pwrGnss_e;

/*---------------------------------------------------------------------------*/
/*         Data structures for the ciaaMobile_checkGSMGPRS function          */
/*---------------------------------------------------------------------------*/

/** @brief Struct for determining the status of the GSM and GPRS connections */

typedef struct _connStatus_s {
   bool  gsm;   /**< Is the ME attached to the GSM network? */
   bool  gprs;  /**< Is the ME attached to the GPRS service? */
} connStatus_s;


/*==================[external data declaration]==============================*/

/** @brief used for AT command timeout counter */

extern uint32_t timeout_count;

/*==================[external functions declaration]=========================*/

/*---------------------------------------------------------------------------*/
/*                  General GSM library operation functions                  */
/*---------------------------------------------------------------------------*/

/** @brief Starts up the GSM engine
*
* @param cback  Function pointer to callback function
*
* @return
*/

void gsmStartUp (void * (*cback) (errorUser_s, void *));

/** @brief Handles AT command timeout and sysUpdate timing
*
* @return
*/

void gsmSysTickHandler (void);

/** @brief Processes commands and URCs at the rate of one token per invocation
*
*/

void gsmProcess (void);

/** @brief Indicates if the GSM engine is currently idle
 *
 * @return Returns 1 if no formula is being run, 0 otherwise
*/

uint8_t gsmIsIdle (void);

/** @brief Gets signal quality values (RSSI and BER)
*
* @param signal_quality Pointer to signal quality struct
* @param cback          Function pointer to callback function
*
* @return
*/

void gsmGetSigQual (sigQual_s * sigQual, void * (*cback) (errorUser_s, void *));

/** @brief Check status of GSM and GPRS connection
*
* @param status   Pointer to GSM & GPRS status struct
* @param cback    Function pointer to callback function
*
* @return
*/

void gsmCheckConn (connStatus_s * status, void * (*cback) (errorUser_s, void *));

/** @brief Sets URC handling to cback mode and sets callback function
*
* @param cback  Function pointer to callback function
*
* @return
*/

void gsmSetUrcCbackMode (void (*cback) (uint8_t const * const cmd, uint8_t const * const par));

/** @brief Sets URC handling to manual mode
*
* @return
*/

void gsmSetUrcManualMode (void);

/*---------------------------------------------------------------------------*/
/*                              SMS functions                                */
/*---------------------------------------------------------------------------*/

/** @brief Sends an SMS
*
* @param msg Pointer to SMS message to be sent
* @param cback Function pointer to callback function
*
* @return
*/

void gsmSmsSend (smsOut_s * msg, void * (*cback) (errorUser_s, void *));

/** @brief Read a single received SMS
*
* @param msg    Pointer to storage variable for SMS to be read
* @param pars   SMS read parameters (index and mode)
* @param cback  Function pointer to callback function
*
* @return
*/

void gsmSmsRead (smsRec_s * msg, smsRdPars_s * pars, void * (*cback) (errorUser_s, void *));

/** @brief Lists received SMSs in a vector
*
* @param list     Pointer to storage vector for SMSs to be read
* @param listSize Size of the storage vector
* @param cback    Function pointer to callback function
*
* @return
*/

void gsmSmsList (smsRec_s * list, uint8_t listSize, void * (*cback) (errorUser_s, void *));

/** @brief Deletes a single SMS from memory
*
* @param msgdel Pointer to SMS delete structure (indicated index and mode)
* @param cback  Function pointer to callback function
*
* @return
*/

void gsmSmsDel (smsDel_s * msgdel, void * (*cback) (errorUser_s, void *));

/*---------------------------------------------------------------------------*/
/*                             GPRS functions                                */
/*---------------------------------------------------------------------------*/

/** @brief Starts up GPRS connection in the modem
*
* @param apn    Pointer to struct containing APN, user and password
* @param cback  Function pointer to callback function
*
* @return
*/

void gsmGprsStart (apnUserPwd_s * apn, void * (*cback) (errorUser_s, void *));

/** @brief Opens TCP or UDP port
*
* @param port      Pointer to struct containing the type of port, IP address and port #
* @param cback     Function pointer to callback function
*
* @return
*/

void gsmGprsOpenPort (port_s * port, void * (*cback) (errorUser_s, void *));

/** @brief Closes open TCP or UDP port
*
* @param cback     Function pointer to callback function
*
* @return
*/

void gsmGprsClosePort (void * (*cback) (errorUser_s, void *));

/*---------------------------------------------------------------------------*/
/*                             GNSS functions                                */
/*---------------------------------------------------------------------------*/

/** @brief Turns GNSS on or off
*
* @param cmd      ON or OFF command
* @param cback    Function pointer to callback function
*
* @return
*/

void gsmGnssPwr (pwrGnss_e * cmd, void * (*cback) (errorUser_s, void *));

/** @brief Get GNSS data
*
* @param gnssData  GNSS data vector; must be of size 95 at least
* @param cback     Function pointer to callback function
*
* @return
*/

void gsmGnssGetData (uint8_t * gnssData, void * (*cback) (errorUser_s, void *));

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef _GSM_INTERFACE_H_ */
