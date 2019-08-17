#include "build/temp/_test_gsmTokenizer.c"
#include "vl_ring_buffer.h"
#include "ring_buffer.h"
#include "mock_gsmComms.h"
#include "gsmTokenizer.h"
#include "unity.h"


















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



         UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(77));



         break;

   }



   return res;

}











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



   UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((n)), (((void *)0)), (UNITY_UINT)(109), UNITY_DISPLAY_STYLE_INT);

}



void test_gsmDetectTkns_1(void)

{





   static uint8_t tknRbBuf[1024];

   static RINGBUFF_T tknRb;

   static VLRINGBUFF_T tknVlRb;







   VLRingBuffer_Init(&tknVlRb, &tknRb, &tknRbBuf, 1, 1024);



   gsmInitTokenizer();



   uint8_t token[10];



   uint8_t res = 0;







   gsm232UartRecv_StubWithCallback(gsm232UartRecv_Callback1);







   gsmDetectTkns(&tknVlRb);



   res = VLRingBuffer_Pop(&tknVlRb, token, 10);



   UnityAssertEqualStringLen((const char*)(("AT\r")), (const char*)((token)), (UNITY_UINT32)((strlen("AT\r"))), (((void *)0)), (UNITY_UINT)(140));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((ECHO)), (UNITY_INT)(UNITY_INT8 )((token[strlen("AT\r")])), (((void *)0)), (UNITY_UINT)(141), UNITY_DISPLAY_STYLE_INT8);







   gsmDetectTkns(&tknVlRb);



   res = VLRingBuffer_Pop(&tknVlRb, token, 10);



   UnityAssertEqualStringLen((const char*)(("\r\nOK\r\n")), (const char*)((token)), (UNITY_UINT32)((strlen("\r\nOK\r\n"))), (((void *)0)), (UNITY_UINT)(149));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((RSP)), (UNITY_INT)(UNITY_INT8 )((token[strlen("\r\nOK\r\n")])), (((void *)0)), (UNITY_UINT)(150), UNITY_DISPLAY_STYLE_INT8);







   gsmDetectTkns(&tknVlRb);



   res = VLRingBuffer_Pop(&tknVlRb, token, 10);



   UnityAssertEqualStringLen((const char*)(("abcde\r\n")), (const char*)((token)), (UNITY_UINT32)((strlen("abcde\r\n"))), (((void *)0)), (UNITY_UINT)(158));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((DATA_BLOCK)), (UNITY_INT)(UNITY_INT8 )((token[strlen("abcde\r\n")])), (((void *)0)), (UNITY_UINT)(159), UNITY_DISPLAY_STYLE_INT8);







   gsmDetectTkns(&tknVlRb);



   res = VLRingBuffer_Pop(&tknVlRb, token, 10);



   UnityAssertEqualStringLen((const char*)(("\r\n> ")), (const char*)((token)), (UNITY_UINT32)((strlen("\r\n> "))), (((void *)0)), (UNITY_UINT)(167));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((SMS_PROMPT)), (UNITY_INT)(UNITY_INT8 )((token[strlen("\r\n> ")])), (((void *)0)), (UNITY_UINT)(168), UNITY_DISPLAY_STYLE_INT8);







   gsmDetectTkns(&tknVlRb);



   res = VLRingBuffer_Pop(&tknVlRb, token, 10);



   UnityAssertEqualStringLen((const char*)(("abcde")), (const char*)((token)), (UNITY_UINT32)((strlen("abcde"))), (((void *)0)), (UNITY_UINT)(176));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((SMS_BODY)), (UNITY_INT)(UNITY_INT8 )((token[strlen("abcde")])), (((void *)0)), (UNITY_UINT)(177), UNITY_DISPLAY_STYLE_INT8);



}
