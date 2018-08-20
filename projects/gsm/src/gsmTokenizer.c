/* Copyright 2018, Ezequiel Martinez Vazquez
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

/** @brief This module handles the tokenizer function
 */

/** \addtogroup gsm
 ** @{ */

/*==================[inclusions]=============================================*/

#include "gsmTokenizer.h"

/*==================[macros and definitions]=================================*/

#ifdef DEBUG_TKNZER
   #define debug(msg) dbgPrint(msg)
#else
   #define debug(msg)
#endif

/*==================[global data]============================================*/

/*==================[internal data declaration]==============================*/

/** @brief Read characters buffer */

static uint8_t rdChBuf[RD_BUF_SIZ];

/** @brief Read characters ring buffer structure */

static RINGBUFF_T rdChRb;

/** @brief Current token buffer */

static uint8_t currTknBuf[CURR_TKN_BUF_SIZ];

/** @brief Current token ring buffer structure */

static RINGBUFF_T currTknRb;

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

void gsmInitTokenizer(void)
{
   /* Initialize read character ring buffer */
   RingBuffer_Init(&rdChRb, &rdChBuf, 1, RD_BUF_SIZ);

   /* Initialize current token ring buffer */
   RingBuffer_Init(&currTknRb, &currTknBuf, 1, CURR_TKN_BUF_SIZ);

   return;
}

void gsmDetectTkns(VLRINGBUFF_T * tknVlRb)
{
   /* State-machine variables and flags */

   tknType_e currTkn = NONE;     /* Classification of the current token */

   static uint8_t crLf = 0;    /* Initial <CR><LF> sequence detected */
   static uint8_t empty = 1;   /* Non-control characters received */
   static uint8_t smsIn = 0;   /* SMS promp token just received */

   /* Buffer processing variables */

   uint8_t swapBuf[SWAP_BUF_SIZ];   /* swap buffer */
   int n = 0;                       /* no of chars read from UART rb */
   uint8_t ch = '\0';               /* character being read */
   static uint8_t pCh = '\0';       /* previous character read */

   /* Get free space in the read char rb */
   int rdChRbFree = RingBuffer_GetFree(&rdChRb);

   /* Read chars from the UART up to free space in the read char rb or
    * swap buffer size, whichever is smaller */
   n = uartRecv(CIAA_UART_232, &swapBuf,
                (rdChRbFree < SWAP_BUF_SIZ) ? rdChRbFree : SWAP_BUF_SIZ);

   /* Insert swap buffer into the read char rb */
   RingBuffer_InsertMult(&rdChRb, &swapBuf, n);

   if(RingBuffer_IsFull(&rdChRb)){
      debug(">>>tknzer<<<   READ BUFFER FULL!\r\n");
   }

   /* Cycle through each read char, detect all tokens and insert them in the
    * token vlrb */
   while(0 != RingBuffer_Pop(&rdChRb, &ch)){

         RingBuffer_Insert(&currTknRb, &ch); /* insert char in the current
                                                token rb */

         if(RingBuffer_IsFull(&currTknRb)){
            debug(">>>tknzer<<<   CURR TOKEN BUFFER FULL!\r\n");
         }

         if(!iscntrl(ch)){empty = 0;} /* if ch is not a control character then
                                         lower the empty flag */

         /* Read "tokenizer SM.txt" to understand the logic of the tokenizer
          * state machine logic which follows
          */

         if(('\r' == pCh) && ('\n' != ch) && !crLf && !empty){
            currTkn = ECHO; smsIn = 0;
            debug(">>>tknzer<<<   AT command echo\r\n");
         } /* AT command echo */
         else if(('\r' == pCh) && ('\n' == ch) && crLf){
            currTkn = RESP; smsIn = 0;
            debug(">>>tknzer<<<   AT command response\r\n");
         } /* AT command response */
         else if(('\r' == pCh) && ('\n' == ch) && !crLf && !empty && !smsIn){
            currTkn = DATAB; smsIn = 0;
            debug(">>>tknzer<<<   DATA block\r\n");
         } /* DATA block */
         else if(('\r' == pCh) && ('\n' == ch) && !crLf && !empty && smsIn) {
            currTkn = SMSBOD; smsIn = 0; crLf = 1;
            debug(">>>tknzer<<<   SMS send body\r\n");
         } /* SMS send body */
         else if(('>' == pCh) && (' ' == ch) && crLf) {
            currTkn = SMSIN; smsIn = 1;
            debug(">>>tknzer<<<   SMS send prompt\r\n");
         } /* SMS send prompt */
         else { currTkn = NONE; }

         /* if a valid token is detected, empty the current token rb into
          * the swap buffer and insert it in the token vlrb */

         if(NONE != currTkn){

            if(SMSBOD != currTkn){ crLf = 0; }
            empty = 1;
            int length = RingBuffer_GetCount(&currTknRb)
                         - (ECHO == currTkn) - (SMSBOD == currTkn)*2;
            RingBuffer_PopMult(&currTknRb, &swapBuf, length);

            /* insert current token into token VL ring buffer */
            VLRingBuffer_Insert(tknVlRb, &swapBuf, length);
         }

         else if(('\r' == pCh) && ('\n' == ch) && !crLf && empty){
            crLf = 1;
         }

         pCh = ch;
   }

   return;

}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
