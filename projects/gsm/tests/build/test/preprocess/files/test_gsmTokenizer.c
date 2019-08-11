#include "build/temp/_test_gsmTokenizer.c"
#include "vl_ring_buffer.h"
#include "ring_buffer.h"
#include "mock_gsmComms.h"
#include "gsmTokenizer.h"
#include "unity.h"
















uint8_t gsm232UartRecv_Callback1 (uint8_t * const buffer, int n, int ncalls)

{

   strncpy(buffer, "AT\rOK", 5);



   uint8_t res = 0;



   res = 4;



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



   UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((n)), (((void *)0)), (UNITY_UINT)(68), UNITY_DISPLAY_STYLE_INT);

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



   UnityAssertEqualStringLen((const char*)(("AT\r")), (const char*)((token)), (UNITY_UINT32)((3)), (((void *)0)), (UNITY_UINT)(99));

}
