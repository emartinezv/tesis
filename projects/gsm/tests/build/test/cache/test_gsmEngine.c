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





      "AT\rT","A\r\r","\n+CMTI:abcde\r\n",





      "AT+CMGS=num\r\r","\n> ","hola\r\n","OK\r\n"

};











uint8_t gsm232UartRecv_Callback (uint8_t * const buffer, int n, int NumCalls)

{

   uint8_t res = 0;



   if(NumCalls > (sizeof(uartRecvMock)/sizeof(uint8_t *))){

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(92));

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

void test_gsmFsm(void)

{





   gsm232UartRecv_StubWithCallback(gsm232UartRecv_Callback);

   gsm232UartSend_StubWithCallback(gsm232UartSend_Callback);







   gsmInitEngine();



   fsmEvent_e fsmEvent;









   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(162), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(168), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(174), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(180), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(183), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(186), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(192), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(195), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(198), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(204), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(207), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(210), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(213), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(219), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(222), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(228), UNITY_DISPLAY_STYLE_INT);



   int i = 0;



   for(i = 0; i < 100; i++) {gsmDecToutCnt();}

   UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((gsmToutCntZero())), (((void *)0)), (UNITY_UINT)(233), UNITY_DISPLAY_STYLE_INT);

   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_TIMEOUT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(235), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(241), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(244), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_RSP)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(247), UNITY_DISPLAY_STYLE_INT);



   for(i = 0; i < 100; i++) {gsmDecToutCnt();}

   UnityAssertEqualNumber((UNITY_INT)((1)), (UNITY_INT)((gsmToutCntZero())), (((void *)0)), (UNITY_UINT)(250), UNITY_DISPLAY_STYLE_INT);

   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_TIMEOUT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(252), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(258), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(261), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(267), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(270), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_MSG_CLOSE)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(273), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT+CIFSR\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(279), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(282), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(285), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(291), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(294), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(297), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(300), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(306), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(309), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(312), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(315), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(321), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(324), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(327), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(330), UNITY_DISPLAY_STYLE_INT);









   fsmEvent = gsmSendCmd("AT+CMGS=sum\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(336), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(339), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(342), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmSendCmd("hola");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(345), UNITY_DISPLAY_STYLE_INT);



   fsmEvent = gsmProcessTkn();

   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((fsmEvent)), (((void *)0)), (UNITY_UINT)(348), UNITY_DISPLAY_STYLE_INT);



   return;

}
