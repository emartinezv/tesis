/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

//-- unity: unit test framework
#include "unity.h"

//-- module being tested
#include "gsmTokenizer.h"
#include "string.h"
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

uint8_t gsm232UartRecv_Callback1 (uint8_t * const buffer, int n, int NumCalls)
{
   uint8_t res = 0;

   switch(NumCalls){

      case 0:

         strncpy(buffer, "AT\r\r\nOK", strlen("AT\r\r\nOK"));
         res = strlen("AT\r\r\nOK");

         break;

      case 1:

         strncpy(buffer, "\r\nabcde", strlen("\r\nabcde"));
         res = strlen("\r\nabcde");

         break;

      case 2:

         strncpy(buffer, "\r\n", strlen("\r\n"));
         res = strlen("\r\n");

         break;

      case 3:

         strncpy(buffer, "\r\n> ", strlen("\r\n> "));
         res = strlen("\r\n> ");

         break;

      case 4:

         strncpy(buffer, "abcde\r\n", strlen("abcde\r\n"));
         res = strlen("abcde\r\n");

         break;

      default:

         TEST_FAIL_MESSAGE("Demasiadas llamadas!");

         break;
   }

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

   /* 1st Call: Testing ECHO token */

   gsmDetectTkns(&tknVlRb);

   res = VLRingBuffer_Pop(&tknVlRb, token, 10);

   TEST_ASSERT_EQUAL_STRING_LEN("AT\r", token, strlen("AT\r"));
   TEST_ASSERT_EQUAL_INT8(ECHO, token[strlen("AT\r")]);

   /* 2nd Call: Testing RSP token */

   gsmDetectTkns(&tknVlRb);

   res = VLRingBuffer_Pop(&tknVlRb, token, 10);

   TEST_ASSERT_EQUAL_STRING_LEN("\r\nOK\r\n", token, strlen("\r\nOK\r\n"));
   TEST_ASSERT_EQUAL_INT8(RSP, token[strlen("\r\nOK\r\n")]);

   /* 3rd Call: Testing DATA_BLOCK */

   gsmDetectTkns(&tknVlRb);

   res = VLRingBuffer_Pop(&tknVlRb, token, 10);

   TEST_ASSERT_EQUAL_STRING_LEN("abcde\r\n", token, strlen("abcde\r\n"));
   TEST_ASSERT_EQUAL_INT8(DATA_BLOCK, token[strlen("abcde\r\n")]);

   /* 4th Call: Testing SMS_PROMPT */

   gsmDetectTkns(&tknVlRb);

   res = VLRingBuffer_Pop(&tknVlRb, token, 10);

   TEST_ASSERT_EQUAL_STRING_LEN("\r\n> ", token, strlen("\r\n> "));
   TEST_ASSERT_EQUAL_INT8(SMS_PROMPT, token[strlen("\r\n> ")]);

   /* 5th Call: Testing SMS_BODY */

   gsmDetectTkns(&tknVlRb);

   res = VLRingBuffer_Pop(&tknVlRb, token, 10);

   TEST_ASSERT_EQUAL_STRING_LEN("abcde", token, strlen("abcde"));
   TEST_ASSERT_EQUAL_INT8(SMS_BODY, token[strlen("abcde")]);

}




