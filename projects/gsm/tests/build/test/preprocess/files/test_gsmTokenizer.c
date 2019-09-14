#include "build/temp/_test_gsmTokenizer.c"
#include "vl_ring_buffer.h"
#include "ring_buffer.h"
#include "mock_gsmComms.h"
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



   UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((n)), (((void *)0)), (UNITY_UINT)(58), UNITY_DISPLAY_STYLE_INT);

}



void test_gsmDetectTkns(void)

{





   static uint8_t tknRbBuf[16];

   static RINGBUFF_T tknRb;

   static VLRINGBUFF_T tknVlRb;



   static uint8_t * bufferTest[5] = {"AT\r\r\nOK", "\r\nabcde",

                                     "\r\n", "\r\n> ", "abcde\r\n"};







   VLRingBuffer_Init(&tknVlRb, &tknRb, &tknRbBuf, 1, 16);



   gsmInitTokenizer();



   uint8_t token[10];



   uint8_t res = 0;







   gsmDetectTkns(&tknVlRb, 7, bufferTest[0]);



   res = VLRingBuffer_Pop(&tknVlRb, token, 10);



   UnityAssertEqualStringLen((const char*)(("AT\r")), (const char*)((token)), (UNITY_UINT32)((strlen("AT\r"))), (((void *)0)), (UNITY_UINT)(88));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((ECHO)), (UNITY_INT)(UNITY_INT8 )((token[strlen("AT\r")])), (((void *)0)), (UNITY_UINT)(89), UNITY_DISPLAY_STYLE_INT8);







   gsmDetectTkns(&tknVlRb, 7, bufferTest[1]);



   res = VLRingBuffer_Pop(&tknVlRb, token, 10);



   UnityAssertEqualStringLen((const char*)(("\r\nOK\r\n")), (const char*)((token)), (UNITY_UINT32)((strlen("\r\nOK\r\n"))), (((void *)0)), (UNITY_UINT)(97));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((RSP)), (UNITY_INT)(UNITY_INT8 )((token[strlen("\r\nOK\r\n")])), (((void *)0)), (UNITY_UINT)(98), UNITY_DISPLAY_STYLE_INT8);







   gsmDetectTkns(&tknVlRb, 2, bufferTest[2]);



   res = VLRingBuffer_Pop(&tknVlRb, token, 10);



   UnityAssertEqualStringLen((const char*)(("abcde\r\n")), (const char*)((token)), (UNITY_UINT32)((strlen("abcde\r\n"))), (((void *)0)), (UNITY_UINT)(106));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((DATA_BLOCK)), (UNITY_INT)(UNITY_INT8 )((token[strlen("abcde\r\n")])), (((void *)0)), (UNITY_UINT)(107), UNITY_DISPLAY_STYLE_INT8);







   gsmDetectTkns(&tknVlRb, 4, bufferTest[3]);



   res = VLRingBuffer_Pop(&tknVlRb, token, 10);



   UnityAssertEqualStringLen((const char*)(("\r\n> ")), (const char*)((token)), (UNITY_UINT32)((strlen("\r\n> "))), (((void *)0)), (UNITY_UINT)(115));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((SMS_PROMPT)), (UNITY_INT)(UNITY_INT8 )((token[strlen("\r\n> ")])), (((void *)0)), (UNITY_UINT)(116), UNITY_DISPLAY_STYLE_INT8);







   gsmDetectTkns(&tknVlRb, 7, bufferTest[4]);



   res = VLRingBuffer_Pop(&tknVlRb, token, 10);



   UnityAssertEqualStringLen((const char*)(("abcde")), (const char*)((token)), (UNITY_UINT32)((strlen("abcde"))), (((void *)0)), (UNITY_UINT)(124));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((SMS_BODY)), (UNITY_INT)(UNITY_INT8 )((token[strlen("abcde")])), (((void *)0)), (UNITY_UINT)(125), UNITY_DISPLAY_STYLE_INT8);



}
