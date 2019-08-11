/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

//-- unity: unit test framework
#include "unity.h"

//-- module being tested
#include "gsmTokenizer.h"
#include "mock_gsmComms.h" /* VER */
#include "ring_buffer.h" /* VER */
#include "vl_ring_buffer.h" /* VER */

/*******************************************************************************
 *    DEFINITIONS
 ******************************************************************************/

#define TKN_BUF_SIZE 1024

/*******************************************************************************
 *    PRIVATE TYPES
 ******************************************************************************/

/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/


/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/

uint8_t gsm232UartRecv_Callback1 (uint8_t * const buffer, int n, int ncalls)
{
   strncpy(buffer, "AT\rOK", 5);

   uint8_t res = 0;

   res = 4;

   return res;
}

/*******************************************************************************
 *    SETUP, TEARDOWN
 ******************************************************************************/

void setUp(void)
{
   return;
}

void tearDown(void)
{
   return;
}

/*******************************************************************************
 *    TESTS
 ******************************************************************************/

void test_gsmInitTokenizer(void)
{
   int n = 0;

   n = gsmInitTokenizer();

   TEST_ASSERT_EQUAL_INT(1, n);
}

void test_gsmDetectTkns_1(void)
{
   /** Variables for the VL token RB */

   static uint8_t tknRbBuf[TKN_BUF_SIZE];
   static RINGBUFF_T tknRb;
   static VLRINGBUFF_T tknVlRb;

   /** Initializations */

   VLRingBuffer_Init(&tknVlRb, &tknRb, &tknRbBuf, 1, TKN_BUF_SIZE);

   gsmInitTokenizer();

   uint8_t token[10];

   uint8_t res = 0;

   /* Mock definitions */

   gsm232UartRecv_StubWithCallback(gsm232UartRecv_Callback1);

   /* Testing routine */

   gsmDetectTkns(&tknVlRb);

   res = VLRingBuffer_Pop(&tknVlRb, token, 10);

   TEST_ASSERT_EQUAL_STRING_LEN("AT\r", token, 3);
}




