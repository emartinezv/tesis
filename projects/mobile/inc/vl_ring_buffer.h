/* Copyright 2016, Ezequiel Martinez Vazquez
 * All rights reserved.
 *
 *
 */

#ifndef _VL_RING_BUFFER_H_
#define _VL_RING_BUFFER_H_

/** \addtogroup vl_ring_buffer
 ** @{ */

/*==================[inclusions]=============================================*/

#include "ring_buffer.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/

/*==================[typedef]================================================*/

typedef struct {
   RINGBUFF_T * rb;
   int        vlcount;
} VLRINGBUFF_T;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/**
 * @brief   Initialize variable-length ring buffer
 * @param   vlrb     : Pointer to variable-length ring buffer to initialize
 * @param   RingBuff : Pointer to ring buffer to initialize
 * @param   buffer   : Pointer to buffer to associate with RingBuff
 * @param   itemSize : Size of each buffer item size
 * @param   count    : Size of ring buffer
 * @note Memory pointed by @a buffer must have correct alignment of
 *          @a itemSize, and @a count must be a power of 2 and must at
 *          least be 2 or greater.
 * @return  Nothing
 */
int VLRingBuffer_Init(VLRINGBUFF_T * vlrb, RINGBUFF_T *RingBuff, void *buffer, int itemSize, int count);

/** @brief VLRB insert function
 * @param vlrb pointer to variable-length ring buffer structure
 * @param data pointer to first element of the item array
 * @param num  size of the array to be inserted
 */
int VLRingBuffer_Insert(VLRINGBUFF_T * vlrb, const void * data, uint16_t num);

/** @brief VLRB pop function
 * @param vlrb pointer to variable-length ring buffer structure
 * @param data pointer to memory where popped data will be stored
 * @param cap  capacity of the memory space pointed to by data
 */
int VLRingBuffer_Pop(VLRINGBUFF_T * vlrb, void * data, uint16_t cap);

/**
 * @brief   Resets the VL ring buffer to empty
 * @param   RingBuff : Pointer to VL ring buffer
 * @return  Nothing
 */
void VLRingBuffer_Flush(VLRINGBUFF_T * vlrb);

/**
 * @brief   Determines if the VL ring buffer is empty
 * @param   vlrb : Pointer to VL ring buffer
 * @return  1 if the ring buffer is empty, otherwise 0
 */
int VLRingBuffer_IsEmpty(VLRINGBUFF_T * vlrb);

/**
 * @brief   Determines if the VL ring buffer is full
 * @param   vlrb : Pointer to VL ring buffer
 * @return  1 if the ring buffer is full, otherwise 0
 *
 *          Note that every VL entry needs 2 bytes for sizing,
 *          so if the VL's buffer has 2 or less bytes left it
 *          will report as full.
 */
int VLRingBuffer_IsFull(VLRINGBUFF_T * vlrb);

/**
 * @brief   Return size of free space in the VL ring buffer
 * @param   vlrb : Pointer to VL ring buffer
 * @return  Size of free space in the ring buffer
 */
int VLRingBuffer_GetFree(VLRINGBUFF_T * vlrb);

/**
 * @brief   Return number of VL items in the VL ring buffer
 * @param   vlrb : Pointer to VL ring buffer
 * @return  Number of VL items in the VL ring buffer
 */
int VLRingBuffer_GetCount(VLRINGBUFF_T * vlrb);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif
