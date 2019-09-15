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

/** \addtogroup comms comms
 ** @{ */

/*==================[inclusions]=============================================*/

#include "gsmComms.h"

/*==================[macros and definitions]=================================*/

#define DEBUG_COMMS
#ifdef DEBUG_COMMS
   #define debug(msg) dbgPrint(msg)
#else
   #define debug(msg)
#endif

/*==================[global data]============================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

int gsm232UartRecv (uint8_t * const buffer, int n)
{
   int recv = 0;

   /* Call HAL 232-UART receive function here so that at most n characters are
      read into buffer. We assume that the function returns the number of
      read characters. */

   recv = uartRecv(CIAA_UART_232, buffer, n);

   return recv;
}

int gsm232UartSend (uint8_t const * const buffer, int n)
{
   int sent = 0;

   /* Call HAL 232-UART send function here so that at most n characters are
      written from buffer. We assume that the function returns the number of
      written characters. */

   sent = uartSend(CIAA_UART_232, buffer, n);

   return sent;
}

int gsmTermUartRecv (uint8_t * const buffer, int n)
{
   int recv = 0;

   /* Call HAL Term-UART receive function here so that at most n characters are
      read into buffer. We assume that the function returns the number of
      read characters. */

   recv = uartRecv(CIAA_UART_USB, buffer, n);

   return recv;
}

int gsmTermUartSend (uint8_t const * const buffer, int n)
{
   int sent = 0;

   /* Call HAL Term-UART send function here so that at most n characters are
      written from buffer. We assume that the function returns the number of
      written characters. */

   sent = uartSend(CIAA_UART_USB, buffer, n);

   return sent;
}


/** @} doxygen end group definition */

/*==================[end of file]============================================*/
