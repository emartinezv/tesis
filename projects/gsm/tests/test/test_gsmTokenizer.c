/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

//-- unity: unit test framework
#include "unity.h"

//-- module being tested
#include "gsmTokenizer.h"

//-- other modules
#include "string.h"
#include "vl_ring_buffer.h"
#include "ring_buffer.h"


/*******************************************************************************
 *    DEFINITIONS
 ******************************************************************************/

#define SWAP_BUF_SIZE TKN_LEN /* Should pick this up from gsmTokenizer.h but
                                 for some reason it doesn't */

#define TKN_BUF_SIZE 16 /* Must be larger than the largest token used for
                           testing purposes */

/*******************************************************************************
 *    PRIVATE TYPES
 ******************************************************************************/

/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/

/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/

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

/* test_gsmInitTokenizer
 *
 * Functions tested:
 *
 * - gsmInitTokenizer
 *
 * */

void test_gsmInitTokenizer(void)
{
   int n = 0;

   n = gsmInitTokenizer();

   TEST_ASSERT_EQUAL_INT(1, n);
}

/* test_gsmNoChTokenizer
 *
 * Functions tested:
 *
 * - gsmInitTokenizer
 * - gsmNoChTokenizer
 *
 * Ideally this test would be independent of RB operation, but since we need
 * the actual RB module as an include we cannot mock it at the same time. Test
 * is incomplete in the sense that we would need to change defines in order to
 * test other branches.
 *
 * */

void test_gsmNoChTokenizer(void)
{
   /* Variables */

   uint16_t noCh;
   bool init;

   /* Initializations */

   init = gsmInitTokenizer();

   TEST_ASSERT_TRUE(init);

   /* Test sequence */

   noCh = gsmNoChTokenizer();

   TEST_ASSERT_EQUAL_INT(SWAP_BUF_SIZE, noCh);

}

/* test_gsmDetectTkns
 *
 * Functions tested:
 *
 * - gsmDetectTkns
 *
 * */

void test_gsmDetectTkns(void)
{
   /** Variables for the VL token RB */

   uint8_t tknRbBuf[TKN_BUF_SIZE];
   RINGBUFF_T tknRb;
   VLRINGBUFF_T tknVlRb;

   uint8_t * bufferTest[5] = {"AT\r\r\nOK", "\r\nabcde",
                                     "\r\n", "\r\n> ", "abcde\r\n"};

   /** Initializations */

   VLRingBuffer_Init(&tknVlRb, &tknRb, &tknRbBuf, 1, TKN_BUF_SIZE);

   gsmInitTokenizer();

   uint8_t token[10];

   uint8_t res = 0;

   /* 1st Call: Testing ECHO token */

   gsmDetectTkns(&tknVlRb, 7, bufferTest[0]);

   res = VLRingBuffer_Pop(&tknVlRb, token, 10);

   TEST_ASSERT_EQUAL_STRING_LEN("AT\r", token, strlen("AT\r"));
   TEST_ASSERT_EQUAL_INT8(ECHO, token[strlen("AT\r")]);

   /* 2nd Call: Testing RSP token */

   gsmDetectTkns(&tknVlRb, 7, bufferTest[1]);

   res = VLRingBuffer_Pop(&tknVlRb, token, 10);

   TEST_ASSERT_EQUAL_STRING_LEN("\r\nOK\r\n", token, strlen("\r\nOK\r\n"));
   TEST_ASSERT_EQUAL_INT8(RSP, token[strlen("\r\nOK\r\n")]);

   /* 3rd Call: Testing DATA_BLOCK */

   gsmDetectTkns(&tknVlRb, 2, bufferTest[2]);

   res = VLRingBuffer_Pop(&tknVlRb, token, 10);

   TEST_ASSERT_EQUAL_STRING_LEN("abcde\r\n", token, strlen("abcde\r\n"));
   TEST_ASSERT_EQUAL_INT8(DATA_BLOCK, token[strlen("abcde\r\n")]);

   /* 4th Call: Testing SMS_PROMPT */

   gsmDetectTkns(&tknVlRb, 4, bufferTest[3]);

   res = VLRingBuffer_Pop(&tknVlRb, token, 10);

   TEST_ASSERT_EQUAL_STRING_LEN("\r\n> ", token, strlen("\r\n> "));
   TEST_ASSERT_EQUAL_INT8(SMS_PROMPT, token[strlen("\r\n> ")]);

   /* 5th Call: Testing SMS_BODY */

   gsmDetectTkns(&tknVlRb, 7, bufferTest[4]);

   res = VLRingBuffer_Pop(&tknVlRb, token, 10);

   TEST_ASSERT_EQUAL_STRING_LEN("abcde", token, strlen("abcde"));
   TEST_ASSERT_EQUAL_INT8(SMS_BODY, token[strlen("abcde")]);

}




