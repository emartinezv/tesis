#include "ring_buffer.h"
typedef struct {

   RINGBUFF_T * rb;

   int vlcount;

} VLRINGBUFF_T;

int VLRingBuffer_Init(VLRINGBUFF_T * vlrb, RINGBUFF_T *RingBuff, void *buffer, int itemSize, int count);













int VLRingBuffer_Insert(VLRINGBUFF_T * vlrb, const void * data, uint16_t num);













int VLRingBuffer_Pop(VLRINGBUFF_T * vlrb, void * data, uint16_t cap);













void VLRingBuffer_Flush(VLRINGBUFF_T * vlrb);













int VLRingBuffer_IsEmpty(VLRINGBUFF_T * vlrb);

int VLRingBuffer_IsFull(VLRINGBUFF_T * vlrb);













int VLRingBuffer_GetFree(VLRINGBUFF_T * vlrb);













int VLRingBuffer_GetCount(VLRINGBUFF_T * vlrb);
