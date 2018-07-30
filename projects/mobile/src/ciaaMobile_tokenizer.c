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

/** @brief This module handles the tokenizer function
 */

/** \addtogroup tokenizer tokenizer
 ** @{ */

/*==================[inclusions]=============================================*/

#include "ciaaMobile_tokenizer.h"

/*==================[macros and definitions]=================================*/

#define DEBUG_TOKENIZER // debug mode
#ifdef DEBUG_TOKENIZER
   #define debug(msg) dbgPrint(msg)
#else
   #define debug(msg)
#endif

/*==================[global data]============================================*/

/*==================[internal data declaration]==============================*/

/** @brief Auxiliary buffer */

static uint8_t auxBuffer[AUX_BUFF_SIZE];

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

void detectTokens(VLRINGBUFF_T * vlrb)
{
   /* State-machine variables */

   tokenType_t token = NONE;

   static uint8_t pCh = 0x00; /* store previous character */
   static uint8_t crLf = 0;   /* initial <CR><LF> sequence detected */
   static uint8_t empty = 1;  /* non-control characters received */
   static uint8_t smsIn = 0;  /* SMS promp token just received */

   /* Buffer processing variables */

   uint8_t n = 0;            /* number of characters read from UART ring buffer */
   static uint8_t i = 0;     /* index counter */
   uint8_t ch = '\0';        /* character being read */

   n = uartRecv(CIAA_UART_232, &auxBuffer[i], 150);

   if(0 != n){
      for(; i < n; i++){

         ch = auxBuffer[i];

         if(!iscntrl(ch)){empty = 0;}
         //RingBuffer_Insert(pRB, &ch);

         if(('\r' == pCh) && ('\n' != ch) && !crLf && !empty) { token = ECHO; smsIn = 0; } /* cmd echo */
         else if(('\r' == pCh) && ('\n' == ch) && crLf) { token = RESP; smsIn = 0; } /* response */
         else if(('\r' == pCh) && ('\n' == ch) && !crLf && !empty && !smsIn) { token = DATAB; smsIn = 0; } /* <data> block */
         else if(('\r' == pCh) && ('\n' == ch) && !crLf && !empty && smsIn) {
            token = SMSBod; smsIn = 0; crLf = 1;
         } /* SMS Body */
         else if(('>' == pCh) && (' ' == ch) && crLf) {
            token = SMSIn; smsIn = 1;
         } /* SMS input query */
         else { token = NONE; }

         if(NONE != token){

            if(SMSBod != token){ crLf = 0; }
            empty = 1;
            uint8_t length = (i+1) - (ECHO == token) - (SMSBod == token)*2;
            VLRingBuffer_Insert(vlrb, &auxBuffer, length); /* insert new token into token VL ring buffer */
            i=0;
         }

         else if(('\r' == pCh) && ('\n' == ch) && !crLf && empty){
            crLf = 1;
         }

         pCh = ch;
      }

   }

   return;

}




/** @} doxygen end group definition */

/*==================[end of file]============================================*/