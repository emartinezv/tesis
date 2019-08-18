#include "build/temp/_test_gsmEngine.c"
#include "vl_ring_buffer.h"
#include "ring_buffer.h"
#include "mock_gsmComms.h"
#include "gsmTokenizer.h"
#include "gsmCommands.h"
#include "gsmParser.h"
#include "gsmEngine.h"
#include "unity.h"


















static uint8_t const * const uartRecvMock [] = {





      "\r\n+CMTI:abcde\r\n",





      "\r\nOK\r\n",





      "abcde\r\n",





      "AT\rA","T\r\r","\nOK\r\n",







      "AT\rA","TI\rA",





      "T\rA","T\r\r","\n+CMTI:abcde\r\n","\r\nOK\r\n",







      "AT\r\r","\nOK\r\n",





      "AT\rA","T\rA", "",





      "T\ra","bcde\r\n",





      "AT\rA","T\r\r","\nERROR\r\n",





      "AT\rA","T\r\r","\ninfo\r\n", "\r\nOK\r\n"



};











uint8_t gsm232UartRecv_Callback (uint8_t * const buffer, int n, int NumCalls)

{

   uint8_t res = 0;



   if(NumCalls > (sizeof(uartRecvMock)/sizeof(uint8_t *))){

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(80));

   }



   strncpy(buffer, uartRecvMock[NumCalls], strlen(uartRecvMock[NumCalls]));

   res = strlen(uartRecvMock[NumCalls]);



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

void test_gsmProcessTkn(void)

{





   gsm232UartRecv_StubWithCallback(gsm232UartRecv_Callback);







   gsmInitEngine();



   fsmEvent_e fsmEvent;







   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(126), UNITY_DISPLAY_STYLE_INT);







   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(131), UNITY_DISPLAY_STYLE_INT);







   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(136), UNITY_DISPLAY_STYLE_INT);







   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(141), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(144), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(147), UNITY_DISPLAY_STYLE_INT);







   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(152), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(155), UNITY_DISPLAY_STYLE_INT);







   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(160), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(163), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(166), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(169), UNITY_DISPLAY_STYLE_INT);







   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(174), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(177), UNITY_DISPLAY_STYLE_INT);







   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(182), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(185), UNITY_DISPLAY_STYLE_INT);



   int i = 0;



   for(i = 0; i < 100; i++) {gsmDecToutCnt();}

   UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((gsmToutCntZero())), (((void *)0)), (UNITY_UINT)(190), UNITY_DISPLAY_STYLE_INT);

   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_TIMEOUT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(192), UNITY_DISPLAY_STYLE_INT);







   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(197), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(200), UNITY_DISPLAY_STYLE_INT);







   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(205), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(208), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_MSG_CLOSE)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(211), UNITY_DISPLAY_STYLE_INT);







   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(216), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(219), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_RSP)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(222), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(225), UNITY_DISPLAY_STYLE_INT);







   return;

}
