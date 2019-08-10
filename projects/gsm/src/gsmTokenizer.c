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

#define DEBUG_ENGINE
#ifdef DEBUG_ENGINE
   #define debug(msg) gsmTermUartSend(msg, strlen(msg))
#else
   #define debug(msg)
#endif

/*==================[global data]============================================*/

/*==================[internal data declaration]==============================*/

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
   /* Initialize current token ring buffer */
   RingBuffer_Init(&currTknRb, &currTknBuf, 1, CURR_TKN_BUF_SIZ);

   return;
}

/* gsmDetectTkns takes the raw serial input and turns it into discrete parts we
 * call tokens. This is accomplished by detecting certain character sequences
 * that indicate the beginning or end of certain type of token (AT commands,
 * responses, prompts for SMS text and others). Once we have tokens, then the
 * upper layers of the library can process them according to the rules of the
 * AT protocol. The function does make a very rough classification of each
 * token type, but the upper layers refine this classification.
 *
 * The function accumulates the incoming characters in a ring buffer
 * (currTknRb) and looks for the specific characters mentioned. This takes the
 * form of a FSM of sorts, since the condition for token detection sometimes
 * involves both starting and finishing characters. When the function has
 * detected a token, it pops a number of characters from currTknRb and inserts
 * them as a single entry in tknVlRb, the VLRB sent as by the upper layer as
 * the single input variable with the invocation.
 *
 */

void gsmDetectTkns(VLRINGBUFF_T * tknVlRb)
{
   /* State-machine variables and flags */

   tknTypeTknzer_e currTkn = NONE;     /* Classification of the current token */

   static uint8_t crLf = 0;    /* Initial <CR><LF> sequence detected */
   static uint8_t empty = 1;   /* Non-control characters received */
   static uint8_t smsIn = 0;   /* SMS promp token just received */

   /* Buffer processing variables */

   uint8_t rdBuf[RD_BUF_SIZ];       /* read buffer */
   uint8_t swapBuf[SWAP_BUF_SIZ];   /* swap buffer */
   uint16_t n = 0;                  /* no of chars read from UART rb */
   uint16_t i = 0;                  /* swap buffer indexing variable */
   uint8_t ch = '\0';               /* character being read */
   static uint8_t pCh = '\0';       /* previous character read */

   /* Get free space in the current token rb */
   uint16_t currTknRbFree = RingBuffer_GetFree(&currTknRb);

   /* Read chars from the UART up to free space in the current token rb or
    * swap buffer size, whichever is smaller */
   n = gsm232UartRecv(&rdBuf,
                     (currTknRbFree < RD_BUF_SIZ) ?
                      currTknRbFree : RD_BUF_SIZ);

   if(RingBuffer_IsFull(&currTknRb)){
      debug(">>>tknzer<<<   CURRENT TOKEN RB FULL!\r\n");
   }

   /* Cycle through each char in rdBuf, detect all tokens and insert them in
    * the token vlrb */
   for(i = 0; i < n; i++){

      ch = rdBuf[i];

      RingBuffer_Insert(&currTknRb, &ch); /* insert char in the current token
                                             rb */

      if(RingBuffer_IsFull(&currTknRb)){
         debug(">>>tknzer<<<   CURRENT TOKEN BUFFER FULL!\r\n");
      }

      if(!iscntrl(ch)){empty = 0;} /* if ch is not a control character then
                                      lower the empty flag */

      if(('\r' == pCh) && ('\n' != ch) && !crLf && !empty){
         currTkn = ECHO; smsIn = 0;
         debug(">>>tknzer<<<   AT command echo\r\n");
      } /* AT command echo */
      else if(('\r' == pCh) && ('\n' == ch) && crLf && !empty){
         currTkn = RSP; smsIn = 0;
         debug(">>>tknzer<<<   AT command response\r\n");
      } /* AT command response */
      else if(('\r' == pCh) && ('\n' == ch) && !crLf && !empty && !smsIn){
         currTkn = DATA_BLOCK; smsIn = 0;
         debug(">>>tknzer<<<   DATA block\r\n");
      } /* DATA block */
      else if(('\r' == pCh) && ('\n' == ch) && !crLf && !empty && smsIn) {
         currTkn = SMS_BODY; smsIn = 0; crLf = 1;
         debug(">>>tknzer<<<   SMS send body\r\n");
      } /* SMS send body */
      else if(('>' == pCh) && (' ' == ch) && crLf) {
         currTkn = SMS_PROMPT; smsIn = 1;
         debug(">>>tknzer<<<   SMS send prompt\r\n");
      } /* SMS send prompt */
      else { currTkn = NONE; }

      /* if a valid token is detected, empty the current token rb into
       * the swap buffer and insert it in the token vlrb */

      if(NONE != currTkn){

         if(SMS_BODY != currTkn){ crLf = 0;}
         empty = 1;
         int length = RingBuffer_GetCount(&currTknRb)
                      - (ECHO == currTkn) - (SMS_BODY == currTkn)*2;
         RingBuffer_PopMult(&currTknRb, &swapBuf, length);

         /* Add the token type as a trailer to the current token */
         swapBuf[length]=(uint8_t)currTkn;

         /* insert current token into token VL ring buffer */
         VLRingBuffer_Insert(tknVlRb, &swapBuf, length+1);
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
