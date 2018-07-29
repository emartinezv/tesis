/* Copyright 2016, Ezequiel Martinez Vazquez
 * All rights reserved.
 */

/** \addtogroup vl_ring_buffer
 ** @{ */

/*==================[inclusions]=============================================*/

#include "vl_ring_buffer.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

int VLRingBuffer_Insert(VLRINGBUFF_T * vlrb, const void * data, uint16_t num)
{
   int free = 0;

   free = RingBuffer_GetFree(vlrb->rb); /* check for free space in the rb */

   if(free >= num +2){
      RingBuffer_InsertMult(vlrb->rb, (void *) &num, 2); /* insert size tag */
      RingBuffer_InsertMult(vlrb->rb, data, num); /* insert data */
      vlrb->vlcount++;
      return 1;
   }

   else{
      return 0;
   }
}

int VLRingBuffer_Pop(VLRINGBUFF_T * vlrb, void * data, uint16_t cap)
{
   if(RingBuffer_IsEmpty(vlrb->rb)){ /* if buffer is empty return 0 */
      return 0;
   }

   uint16_t size = 0;

   RingBuffer_PopMult(vlrb->rb, (void *) &size, 2); /* pop size tag of latest entry */

   if(cap >= size){ /* if size of last entry is less than memory capacity, pop all data */
      RingBuffer_PopMult(vlrb->rb, data, (int) size);
      vlrb->vlcount--;
      return (int) size;
   }

   else{
      RingBuffer_InsertMult(vlrb->rb, (void *) &size, 2); /* push size tag back again */
      return 0; /* return 0 */
   }

}

int VLRingBuffer_Init(VLRINGBUFF_T * vlrb, RINGBUFF_T *RingBuff, void *buffer, int itemSize, int count){

   vlrb->vlcount = 0;
   vlrb->rb = RingBuff;

   return RingBuffer_Init(RingBuff, buffer, itemSize, count);

}

void VLRingBuffer_Flush(VLRINGBUFF_T * vlrb)
{
   RingBuffer_Flush(vlrb->rb);

   vlrb->vlcount = 0;
}

int VLRingBuffer_IsEmpty(VLRINGBUFF_T * vlrb)
{
   return RingBuffer_IsEmpty(vlrb->rb);
}

int VLRingBuffer_IsFull(VLRINGBUFF_T * vlrb)
{
   if(RingBuffer_GetFree(vlrb->rb) <= 2){
      return 1;
   }
   else{
      return 0;
   }
}

int VLRingBuffer_GetFree(VLRINGBUFF_T * vlrb)
{
   return RingBuffer_GetFree(vlrb->rb);
}

int VLRingBuffer_GetCount(VLRINGBUFF_T * vlrb)
{
   return vlrb->vlcount;
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
