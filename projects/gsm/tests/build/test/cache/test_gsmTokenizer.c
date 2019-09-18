#include "build/temp/_test_gsmTokenizer.c"
#include "ring_buffer.h"
#include "vl_ring_buffer.h"
#include "gsmTokenizer.h"
#include "unity.h"




















void setUp(void)

{

   return;

}



void tearDown(void)

{

   return;

}

void test_gsmInitTokenizer(void)

{

   int n = 0;



   n = gsmInitTokenizer();



   UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((n)), (((void *)0)), (UNITY_UINT)(71), UNITY_DISPLAY_STYLE_INT);

}

void test_gsmNoChTokenizer(void)

{





   uint16_t noCh;

   _Bool init;







   init = gsmInitTokenizer();



   if ((init)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(99)));};







   noCh = gsmNoChTokenizer();



   UnityAssertEqualNumber((UNITY_INT)(((20 +280 +1))), (UNITY_INT)((noCh)), (((void *)0)), (UNITY_UINT)(105), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmDetectTkns(void)

{





   uint8_t tknRbBuf[16];

   RINGBUFF_T tknRb;

   VLRINGBUFF_T tknVlRb;



   uint8_t * bufferTest[5] = {"AT\r\r\nOK", "\r\nabcde",

                                     "\r\n", "\r\n> ", "abcde\r\n"};







   VLRingBuffer_Init(&tknVlRb, &tknRb, &tknRbBuf, 1, 16);



   gsmInitTokenizer();



   uint8_t token[10];



   uint8_t res = 0;







   gsmDetectTkns(&tknVlRb, 7, bufferTest[0]);



   res = VLRingBuffer_Pop(&tknVlRb, token, 10);



   UnityAssertEqualStringLen((const char*)(("AT\r")), (const char*)((token)), (UNITY_UINT32)((strlen("AT\r"))), (((void *)0)), (UNITY_UINT)(144));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((ECHO)), (UNITY_INT)(UNITY_INT8 )((token[strlen("AT\r")])), (((void *)0)), (UNITY_UINT)(145), UNITY_DISPLAY_STYLE_INT8);







   gsmDetectTkns(&tknVlRb, 7, bufferTest[1]);



   res = VLRingBuffer_Pop(&tknVlRb, token, 10);



   UnityAssertEqualStringLen((const char*)(("\r\nOK\r\n")), (const char*)((token)), (UNITY_UINT32)((strlen("\r\nOK\r\n"))), (((void *)0)), (UNITY_UINT)(153));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((RSP)), (UNITY_INT)(UNITY_INT8 )((token[strlen("\r\nOK\r\n")])), (((void *)0)), (UNITY_UINT)(154), UNITY_DISPLAY_STYLE_INT8);







   gsmDetectTkns(&tknVlRb, 2, bufferTest[2]);



   res = VLRingBuffer_Pop(&tknVlRb, token, 10);



   UnityAssertEqualStringLen((const char*)(("abcde\r\n")), (const char*)((token)), (UNITY_UINT32)((strlen("abcde\r\n"))), (((void *)0)), (UNITY_UINT)(162));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((DATA_BLOCK)), (UNITY_INT)(UNITY_INT8 )((token[strlen("abcde\r\n")])), (((void *)0)), (UNITY_UINT)(163), UNITY_DISPLAY_STYLE_INT8);







   gsmDetectTkns(&tknVlRb, 4, bufferTest[3]);



   res = VLRingBuffer_Pop(&tknVlRb, token, 10);



   UnityAssertEqualStringLen((const char*)(("\r\n> ")), (const char*)((token)), (UNITY_UINT32)((strlen("\r\n> "))), (((void *)0)), (UNITY_UINT)(171));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((SMS_PROMPT)), (UNITY_INT)(UNITY_INT8 )((token[strlen("\r\n> ")])), (((void *)0)), (UNITY_UINT)(172), UNITY_DISPLAY_STYLE_INT8);







   gsmDetectTkns(&tknVlRb, 7, bufferTest[4]);



   res = VLRingBuffer_Pop(&tknVlRb, token, 10);



   UnityAssertEqualStringLen((const char*)(("abcde")), (const char*)((token)), (UNITY_UINT32)((strlen("abcde"))), (((void *)0)), (UNITY_UINT)(180));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((SMS_BODY)), (UNITY_INT)(UNITY_INT8 )((token[strlen("abcde")])), (((void *)0)), (UNITY_UINT)(181), UNITY_DISPLAY_STYLE_INT8);



}
