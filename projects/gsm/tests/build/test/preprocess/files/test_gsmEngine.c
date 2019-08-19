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





      "AT\rT","A\r\r","\n+CMTI:abcde\r\n"

};











uint8_t gsm232UartRecv_Callback (uint8_t * const buffer, int n, int NumCalls)

{

   uint8_t res = 0;



   if(NumCalls > (sizeof(uartRecvMock)/sizeof(uint8_t *))){

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(89));

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

void test_gsmProcessTkn(void)

{





   gsm232UartRecv_StubWithCallback(gsm232UartRecv_Callback);

   gsm232UartSend_StubWithCallback(gsm232UartSend_Callback);







   gsmInitEngine();



   fsmEvent_e fsmEvent;









   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(149), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(155), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(161), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(167), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(170), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(173), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(179), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(182), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(185), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(191), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(194), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(197), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(200), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(206), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(209), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(215), UNITY_DISPLAY_STYLE_INT);



   int i = 0;



   for(i = 0; i < 100; i++) {gsmDecToutCnt();}

   UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((gsmToutCntZero())), (((void *)0)), (UNITY_UINT)(220), UNITY_DISPLAY_STYLE_INT);

   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_TIMEOUT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(222), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(228), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(231), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_RSP)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(234), UNITY_DISPLAY_STYLE_INT);



   for(i = 0; i < 100; i++) {gsmDecToutCnt();}

   UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((gsmToutCntZero())), (((void *)0)), (UNITY_UINT)(237), UNITY_DISPLAY_STYLE_INT);

   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_TIMEOUT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(239), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(245), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(248), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(254), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(257), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_MSG_CLOSE)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(260), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT+CIFSR\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(266), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(269), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(272), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(278), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(281), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(284), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(287), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(293), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(296), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(299), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(302), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(308), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(311), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(314), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(317), UNITY_DISPLAY_STYLE_INT);



   return;

}
