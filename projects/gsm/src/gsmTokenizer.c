/* Copyright 2019, Ezequiel Martinez Vazquez
 * All rights reserved.
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

/** \addtogroup tokenizer tokenizer
 ** @{ */

/*==================[inclusions]=============================================*/

#include "gsmTokenizer.h"

/*==================[macros and definitions]=================================*/

//#define DEBUG_TOKENIZER
#ifdef DEBUG_TOKENIZER
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

bool gsmInitTokenizer(void)
{
   return RingBuffer_Init(&currTknRb, &currTknBuf, 1, CURR_TKN_BUF_SIZ);
}

uint16_t gsmNoChTokenizer(void)
{
   uint16_t tknRbFree;

   tknRbFree = RingBuffer_GetFree(&currTknRb);

   /* The maximum no of chars the tokenizer can handle at the moment is either
      the free space in the tkn rb or the swap buffer size, whichever is
      smaller */
   return ((tknRbFree < SWAP_BUF_SIZ) ? tknRbFree : SWAP_BUF_SIZ);
}

/* gsmDetectTkns takes the raw serial input and turns it into discrete parts we
 * call tokens. This is accomplished by detecting certain character sequences
 * that indicate the beginning or end of certain type of token (AT commands,
 * responses, prompts for SMS text and others). Once we have tokens, then the
 * upper layers of the library can process them according to the rules of the
 * AT protocol. The function does make a very rough classification of each
 * token type, but the upper layers refine this classification.
 *
 * The function accumulates input buffer characters in a ring buffer
 * (currTknRb) and looks for the specific characters mentioned. This takes the
 * form of an implicit FSM, since the condition for token detection sometimes
 * involves both starting and finishing characters. When the function has
 * detected a token, it pops a number of characters from currTknRb and inserts
 * them as a single entry in the output tknVlRb. The token type (at tokenizer
 * level) is added to the end of the token to assist the higher-level parser
 * function.
 *
 */

void gsmDetectTkns(VLRINGBUFF_T * const tknVlRb, uint16_t nCh,
                   uint8_t const * const buffer)
{
   /* State-machine variables and flags */

   tknTypeTknzer_t currTkn = NONE; /* Classification of the current token */

   static uint8_t crLf = 0;    /* Initial <CR><LF> sequence detected */
   static uint8_t empty = 1;   /* Non-control characters received */
   static uint8_t smsIn = 0;   /* SMS promp token just received */

   /* Buffer processing variables */

   uint8_t swapBuf[SWAP_BUF_SIZ];
   uint16_t i = 0;
   uint8_t ch = '\0';               /* character being read */
   static uint8_t pCh = '\0';       /* previous character read */

   /* Cycles through each char in buffer, inserts chars in local RB, detects
    * all tokens and inserts them in the token VLRB */

   for(i = 0; i < nCh; i++){

      ch = buffer[i];

      RingBuffer_Insert(&currTknRb, &ch);

      if(RingBuffer_IsFull(&currTknRb)){
         debug(">>>tknzer<<<   CURRENT TOKEN BUFFER FULL!\r\n");
      }

      if(!iscntrl(ch)){empty = 0;}

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

      /* If a valid token is detected empty the current token RB into
       * the swap buffer, add the current token type as a trailer and insert
       * the whole thing in the token VLRB */

      if(NONE != currTkn){

         if(SMS_BODY != currTkn){ crLf = 0;}
         empty = 1;

         /* Both ECHO and SMS_BODY require characters from the following tkn
            to arrive before they can be confirmed (1 char for ECHO, 2 chars
            for SMS_BODY). We do not retrieve these from the tkn RB as they are
            part of the following tkn. */

         int length = RingBuffer_GetCount(&currTknRb)
                      - (ECHO == currTkn) - (SMS_BODY == currTkn)*2;

         RingBuffer_PopMult(&currTknRb, &swapBuf, length);

         /* Add the token type as a trailer*/
         swapBuf[length]=(uint8_t)currTkn;

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
