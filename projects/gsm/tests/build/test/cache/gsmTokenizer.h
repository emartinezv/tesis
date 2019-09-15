#include "vl_ring_buffer.h"
#include "gsmComms.h"
#include "lpc_types.h"
typedef enum {

   NONE = 0,

   ECHO = 1,

   RSP = 2,

   DATA_BLOCK = 3,

   SMS_PROMPT = 4,

   SMS_BODY = 5,

}

tknTypeTknzer_t;

_Bool gsmInitTokenizer(void);













uint16_t gsmNoChTokenizer(void);

void gsmDetectTkns(VLRINGBUFF_T * tknVlRb, uint16_t nch,

                   uint8_t const * const buffer);
