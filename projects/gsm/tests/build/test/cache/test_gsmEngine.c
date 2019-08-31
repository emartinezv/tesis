#include "build/temp/_test_gsmEngine.c"
#include "ring_buffer.h"
#include "gsmParser.h"
#include "gsmCommands.h"
#include "mock_vl_ring_buffer.h"
#include "mock_gsmComms.h"
#include "mock_gsmTokenizer.h"
#include "gsmEngine.h"
#include "unity.h"






















static uint8_t const * const uartRecvMock [] = {





      "\r\n+CMTI:abcde\r\n",





      "\r\nOK\r\n",





      "abcde\r\n",





      "AT\r\r","\nOK\r\n",





      "ATI\r\r","\n+CMTI:adcde\r\n",





      "AT\r\r","\n+CMTI:abcde\r\n","\r\nOK\r\n",





      "\r\nOK\r\n",





      "",





      "AT\r\r","\nabcde\r\n","",





      "abcde\r\n",





      "AT\r\r","\nERROR\r\n",





      "AT+CIFSR\r\r","\n192.168.0.1\r\n",





      "\r\n+CMTI:abcde\r\n","AT\r\r","\nOK\r\n",





      "AT\rA","TI\r\r","\n+CMTI:abcde\r\n",





      "AT\rT","A\r\r","\n+CMTI:abcde\r\n",





      "AT+CMGS=num\r\r","\n> ","hola\r\n","OK\r\n"

};











uint8_t gsm232UartRecv_Callback (uint8_t * const buffer, int n, int NumCalls)

{

   uint8_t res = 0;



   if(NumCalls > (sizeof(uartRecvMock)/sizeof(uint8_t *))){

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(97));

   }



   strncpy(buffer, uartRecvMock[NumCalls], strlen(uartRecvMock[NumCalls]));

   res = strlen(uartRecvMock[NumCalls]);



   return res;

}



uint8_t gsm232UartSend_Callback (uint8_t * const buffer, int n, int NumCalls)

{

   return n;

}











void setUp(void)

{

   return;

}



void tearDown(void)

{

   return;

}

void test_gsmInitEngine(void)

{

   gsmEngine_t engine;

   _Bool result;







   gsmInitTokenizer_CMockIgnoreAndReturn(140, 1);

   VLRingBuffer_Init_CMockIgnoreAndReturn(141, 1);







   result = gsmInitEngine(&engine);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(147)));};

}
