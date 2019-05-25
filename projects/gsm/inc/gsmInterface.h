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

/** @brief Invocation period of gsmProcess in ms */

#define DELAY_PROC 10

/** @brief Number of commands which return the modem to data mode. See
 *         gsmCheckDataMode function for more details */

#define CMD_MODE_NO 2

/** @brief Max size of the commands which return the modem to data mode. See
 *         gsmCheckDataMode function for more details */

#define CMD_MODE_SIZE 5

/*==================[typedef]================================================*/

/*---------------------------------------------------------------------------*/
/*                Data structures for internal state retention               */
/*---------------------------------------------------------------------------*/

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

/*---------------------------------------------------------------------------*/
/*                     Data structures for error handling                    */
/*---------------------------------------------------------------------------*/

/** @brief Type for formula level error code return */

typedef enum _errorFrm_e {
   OK,         /**< no errors */
   ERR_INIT,   /**< error during initialization */
   ERR_PROC,   /**< error during command processing */
   ERR_GSM,    /**< error from the GSM engine */
   ERR_TOUT,   /**< error due to timeout */
   ERR_FSM,    /**< error due to cmd FSM being out of range */
   ERR_WRAP,   /**< error during wrap process */
   ERR_UNK     /**< error of type unknown */
} errorFrm_e;

/** @brief Type for command level error code return */

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
/*                              Misc. data structures                        */
/*---------------------------------------------------------------------------*/

/** @brief Type for formula callback */

typedef void * (*frmCback_t) (errorUser_s, void *);

/** @brief Modes for URC handling */

typedef enum _urcMode_e
{
   MANUAL_MODE,
   CBACK_MODE
} urcMode_e ;

/** @brief Type for URC callback */

typedef void (*urcCback_t) (uint8_t const * const cmd,
                            uint8_t const * const par);

/** @brief Type for DATA_MODE callback */

typedef void (*dataCback_t) (void);

/*---------------------------------------------------------------------------*/
/*              Data structures for the gsmGetSigQual function               */
/*---------------------------------------------------------------------------*/

/** @brief Type for signal quality */

typedef struct _sigQual_s {
   int8_t  rssi;    /**< RSSI in dBm */
   uint8_t ber;     /**< BER as RXQUAL values in the table in GSM 05.08 [20]
                       subclause 7.2.4 */
} sigQual_s;

/*---------------------------------------------------------------------------*/
/*               Data structures for the gsmCheckConn function               */
/*---------------------------------------------------------------------------*/

/** @brief Struct for determining the status of the GSM and GPRS connections */

typedef struct _connStatus_s {
   bool  gsm;   /**< Is the ME attached to the GSM network? */
   bool  gprs;  /**< Is the ME attached to the GPRS service? */
} connStatus_s;

/*---------------------------------------------------------------------------*/
/*                Data structures for the gsmSmsSend function                */
/*---------------------------------------------------------------------------*/

/** @brief Type for outgoing SMS message */

typedef struct _smsOut_s {
   uint8_t * dest; /**< destination number as a str */
   uint8_t * text; /**< text of the sms message */
} smsOut_s;

/** @brief Type for outgoing SMS confirmation message */

typedef struct _smsConf_s {
   uint8_t mr;     /**< mr code returned */
} smsConf_s;

/*---------------------------------------------------------------------------*/
/*        Data structures for the gsmSmsRead and gsmSmsList functions        */
/*---------------------------------------------------------------------------*/

/** @brief Type for SMS read mode */

typedef enum _smsReadMode_e {
   NORMAL = 0,     /**< change status to read if message unread */
   NOCHANGE = 1,   /**< do not change status of message */
} smsReadMode_e;

/** @brief Type for SMS read parameters */

typedef struct _smsReadPars_s {
   uint8_t idx;        /**< SMS message index in memory */
   smsReadMode_e mode; /**< SMS read mode (change status or leave as is) */
} smsReadPars_s;

/** @brief Type for received SMS */

typedef struct _smsRec_s {
   uint8_t meta[TKN_PAR_SIZE]; /**< metadata */
   uint8_t text[TKN_PAR_SIZE];   /**< text of the SMS message */
} smsRec_s;

/** @brief Type for SMS list messages status */

typedef enum _smsListStat_e {
   REC_UNREAD,  /**< Received unread messages */
   REC_READ,    /**< Received read messages */
   STO_UNSENT,  /**< Stored unsent messages */
   STO_SENT,    /**< Stored sent messages */
   ALL_MSG      /**< All messages */
} smsListStat_e;

/** @brief Type for SMS list parameters */

typedef struct _smsListPars_s {
   smsListStat_e stat; /**< SMS list messages status */
   smsReadMode_e mode; /**< SMS read mode (change status or leave as is) */
   uint8_t listSize;   /**< Size of the SMS list vector */
} smsListPars_s;

/** @brief Type for SMS read/list return */

typedef struct _smsListRet_s {
   smsRec_s * msgs;  /**< Pointer to message array */
   uint8_t noMsgs;   /**< Size of message array */
} smsListRet_s;

/*---------------------------------------------------------------------------*/
/*                Data structures for the gsmSmsDel function                 */
/*---------------------------------------------------------------------------*/

/** @brief Type for SMS deletion mode */

typedef enum _smsDelMode_e {
   INDEX = 0,              /**< del the message indicated by index */
   READ = 1,               /**< del all read messages */
   READ_SENT = 2,          /**< del all read and sent messages */
   READ_SENT_UNSENT = 3,   /**< del all read, sent and unsent messages */
   DEL_ALL = 4             /**< del all messages */
} smsDelMode_e;

/** @brief Type for SMS delete parameters */

typedef struct _smsDelPars_s {
   uint8_t idx;        /**< index of the message in memory */
   smsDelMode_e mode;  /**< deletion mode */
} smsDelPars_s;

/*---------------------------------------------------------------------------*/
/*               Data structures for the gsmGprsStart function               */
/*---------------------------------------------------------------------------*/

/** @brief Type for APN name, user and password */

typedef struct _apnUserPwd_s {
   uint8_t * apn;   /**< APN address */
   uint8_t * user;  /**< User */
   uint8_t * pwd;   /**< Password */
} apnUserPwd_s;

/*---------------------------------------------------------------------------*/
/*              Data structures for the gsmGprsOpenPort function             */
/*---------------------------------------------------------------------------*/

typedef enum _portType_e {
   TCP,         /**< TCP port */
   UDP          /**< UDP port */
} portType_e;

/** @brief Type for address and port */

typedef struct _port_s {
   portType_e type;      /**< TCP or UDP port */
   uint8_t *  address;   /**< address (domain or IP) */
   uint16_t   port;      /**< port number */
} port_s;

/*---------------------------------------------------------------------------*/
/*                Data structures for the gsmGnssPwr function                */
/*---------------------------------------------------------------------------*/

/** @brief Enum for turning GNSS module on or off */

typedef enum _pwrGnss_e {
   ON,    /**< Power GNSS module on */
   OFF    /**< Power GNSS module off */
} pwrGnss_e;

/*---------------------------------------------------------------------------*/
/*              Data structures for the gsmGnssGetData function              */
/*---------------------------------------------------------------------------*/

/** @brief Struct for storing GNSS info */

typedef struct _dataGnss_s {
   uint8_t data[95];   /**< GNSS data string */
} dataGnss_s;

/*==================[external data declaration]==============================*/

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

void gsmStartUp (frmCback_t cback);

/** @brief Handles AT command timeout and gsmProcess function timing
*
* @return
*/

void gsmSysTickHandler (void);

/** @brief Processes commands, responded and URCs at the rate of one token
*  per invocation
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

void gsmGetSigQual (sigQual_s * sigQual, frmCback_t cback);

/** @brief Check status of GSM and GPRS connection
*
* @param status   Pointer to GSM & GPRS status struct
* @param cback    Function pointer to callback function
*
* @return
*/

void gsmCheckConn (connStatus_s * status, frmCback_t cback);

/** @brief Sets URC handling either cback mode or manual mode
*
* @param mode   URC handling mode
*
* @return
*/

void gsmSetUrcMode (urcMode_e mode);

/** @brief Sets URC cback function
*
* @param cback  Function pointer to callback function
*
* @return
*/

void gsmSetUrcCback (urcCback_t cback);

/** @brief Sets DATA_MODE cback
*
* @param cback  Function pointer to callback function
*
* @return
*/

void gsmSetDataCback (dataCback_t cback);

/** @brief Writes and reads data from serial port in DATA_MODE
*
* @param write   Pointer to buffer with characters to be written
* @param nwrite  Pointer to integer with number of characters to be written;
*                this number needs to be < RD_BUF_SIZE/2
* @param read    Pointer to buffer to store characters to be read
* @param nread   Pointer to integer with number of characters to be read;
*                this number needs to be < RD_BUF_SIZE/2
*
* @return
*/

void gsmWriteReadDataMode (uint8_t * write, uint8_t * nwrite, uint8_t * read,
                           uint8_t * nread);

/** @brief Detects tokens from the GSM modem which indicate that we have left
 *         data mode
*
* @param buf     Serial port read buffer
* @param nch     Number of characters in the buffer
*
* @return
*/

void gsmCheckDataMode (uint8_t const * const buf , uint8_t * const nch);

/*---------------------------------------------------------------------------*/
/*                              SMS functions                                */
/*---------------------------------------------------------------------------*/

/** @brief Sends an SMS
*
* @param msg   Pointer to SMS message to be sent
* @param conf  Pointer to confirmation of SMS being sent
* @param cback Function pointer to callback function
*
* @return
*/

void gsmSmsSend (smsOut_s * msg, smsConf_s * conf, frmCback_t cback);

/** @brief Reads a single received SMS
*
* @param msg    Pointer to storage variable for SMS to be read
* @param pars   SMS read parameters (index and mode)
* @param cback  Function pointer to callback function
*
* @return
*/

void gsmSmsRead (smsRec_s * msg, smsReadPars_s * pars, frmCback_t cback);

/** @brief Lists received SMSs in a vector
*
* @param list     Pointer to storage vector for SMSs to be read
* @param pars     SMS list parameters (status, mode and number of messages)
* @param cback    Function pointer to callback function
*
* @return
*/

void gsmSmsList (smsRec_s * list, smsListPars_s * pars, frmCback_t cback);

/** @brief Deletes a single SMS from memory
*
* @param msgdel Pointer to SMS delete structure (indicated index and mode)
* @param cback  Function pointer to callback function
*
* @return
*/

void gsmSmsDel (smsDelPars_s * msgdel, frmCback_t cback);

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

void gsmGprsStart (apnUserPwd_s * apn, frmCback_t cback);

/** @brief Opens TCP or UDP port
*
* @param port      Pointer to struct containing the type of port, IP address and port #
* @param cback     Function pointer to callback function
*
* @return
*/

void gsmGprsOpenPort (port_s * port, frmCback_t cback);

/** @brief Closes open TCP or UDP port
*
* @param cback     Function pointer to callback function
*
* @return
*/

void gsmGprsClosePort (frmCback_t cback);

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

void gsmGnssPwr (pwrGnss_e * cmd, frmCback_t cback);

/** @brief Get GNSS data
*
* @param gnssData  GNSS data vector; must be of size 95 at least
* @param cback     Function pointer to callback function
*
* @return
*/

void gsmGnssGetData (dataGnss_s * dataGnss, frmCback_t cback);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef _GSM_INTERFACE_H_ */
