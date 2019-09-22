#include "build/temp/_test_gsmEngine.c"
#include "ring_buffer.h"
#include "mock_gsmCommands.h"
#include "mock_vl_ring_buffer.h"
#include "mock_gsmParser.h"
#include "mock_gsmComms.h"
#include "mock_gsmTokenizer.h"
#include "gsmEngine.h"
#include "unity.h"


















typedef enum _vlBufType {

   TKN,

   RSPB,

   URC

} vlBufType_t ;















gsmEngine_t engine;



vlBufType_t bufSel;

_Bool vlIsFull = 0;







uint16_t tknzerNoCh = 0;



uint8_t tknTest[280+20+1];

uint8_t cmdTest[20+1];

uint8_t parTest[280+1];



uint16_t tknSizeTest;

tknTypeParser_t tknTypeTest = AUTOBAUD;



int vlrb_empty = 0;







auxBuffer[280+20+1];

void loadProcessTkn(const uint8_t * const tkn,

                    const uint8_t * const cmd,

                    const uint8_t * const par,

                    tknTypeParser_t tknType)

{

   strncpy(tknTest, tkn, strlen(tkn));

   tknTest[strlen(tkn)] = '\0';

   tknzerNoCh = strlen(tknTest);

   strncpy(cmdTest, cmd, strlen(cmd));

   cmdTest[strlen(cmd)] = '\0';

   strncpy(parTest, par, strlen(par));

   parTest[strlen(par)] = '\0';



   tknSizeTest = tknzerNoCh;

   tknTypeTest = tknType;



   return;

}

int VLRingBuffer_IsFull_Callback(VLRINGBUFF_T * vlrb, int Numcalls)

{

   return vlIsFull;

}



uint16_t gsmNoChTokenizer_Callback(int NumCalls)

{

   return tknzerNoCh;

}



int gsm232UartRecv_Callback (uint8_t * const buffer, int n, int NumCalls)

{

   UnityAssertEqualNumber((UNITY_INT)((tknzerNoCh)), (UNITY_INT)((n)), (((void *)0)), (UNITY_UINT)(112), UNITY_DISPLAY_STYLE_INT);



   return tknSizeTest;

}



void gsmDetectTkns_Callback (VLRINGBUFF_T * const tknVlRb, uint16_t nch,

                             uint8_t const * const buffer, int NumCalls)

{

   if ((tknVlRb == &(engine.tknVlRb))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(120)));};

   UnityAssertEqualNumber((UNITY_INT)((tknSizeTest)), (UNITY_INT)((nch)), (((void *)0)), (UNITY_UINT)(121), UNITY_DISPLAY_STYLE_INT);

}



int VLRingBuffer_IsEmpty_Callback (VLRINGBUFF_T * vlrb, int NumCalls)

{

   if ((&(engine.tknVlRb) == vlrb)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(126)));}



   return vlrb_empty;

}



int VLRingBuffer_Pop_Callback (VLRINGBUFF_T * vlrb, void * data, uint16_t cap,

                               int NumCalls)

{





   switch(bufSel){



      case TKN:



         if ((vlrb == &(engine.tknVlRb))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(140)));};



         break;



      case RSPB:



         if ((vlrb == &(engine.rspVlRb))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(146)));};



         break;



      case URC:



         if ((vlrb == &(engine.urcVlRb))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(152)));};



         break;



      default:



         break;



   }



   strncpy(data, tknTest, strlen(tknTest));

   ((char *)data)[strlen(tknTest)]= '\0';



   return (tknSizeTest);

}



tknTypeParser_t gsmParseTkn_Callback (uint8_t const * const tkn, uint8_t * cmd,

                                      uint8_t * par, uint16_t tknLen,

                                      int NumCalls)

{

   strncpy(cmd, cmdTest, strlen(cmdTest));

   cmd[strlen(cmdTest)] = '\0';



   if(strlen(parTest) > 0){

      strncpy(par, parTest, strlen(parTest));

      par[strlen(parTest)] = '\0';

   }

   else{

      par[0]='\0';

   }



   return (tknTypeTest);

}



_Bool gsmUrcSearch_Callback (uint8_t const * const urc, int NumCalls)

{

   if(0 == strncmp(urc,"CMTI", strlen("CMTI"))){return 1;}



   return 0;

}



const uint8_t const * gsmGetCmdErrRsp_Callback (uint16_t idx, int Numcalls)

{

   switch(idx){



      case 0:



         return("-ERROR-CMS ERROR-");



         break;



      case 19:



         return("-ERROR-CMS ERROR-");



         break;



      default:



         return ("???");



   }



}



const uint8_t const * gsmGetCmdSucRsp_Callback (uint16_t idx, int Numcalls)

{

   switch(idx){



      case 0:



         return("-OK-");



         break;



      case 19:



         return("");



         break;



      default:



         return ("???");



   }



}







uint16_t gsmCmdSearch_Callback(uint8_t const * const cmd, int NumCalls)

{

   if(0 == strcmp(cmd,"AT")){return 0;}

   else if(0 == strcmp(cmd,"SMS_BODY")){return 23;}



   return 65535;

}











int gsm232UartSend_Callback (uint8_t const * const buffer, int n, int NumCalls)

{

   switch(NumCalls){



      case 0:

         if(0 != strcmp(buffer,"AT\r")){

            UnityFail( (("Not sending correct string through UART!")), (UNITY_UINT)(261));

         }

         else{return 3;}

         break;



      case 1:

         if(0 != strcmp(buffer,"texto sms")){

            UnityFail( (("Not sending correct string through UART!")), (UNITY_UINT)(268));

         }

         else{return 9;}

         break;



      case 2:

         if(0 != strcmp(buffer,"\x1A")){

            UnityFail( (("Not sending correct string through UART!")), (UNITY_UINT)(275));

         }

         else{return 1;}

         break;



      default:

         UnityFail( (("gsm232UartSend_Callback called to many times")), (UNITY_UINT)(281));

         break;

   }

}



uint32_t gsmGetCmdTimeout_Callback (uint16_t idx, int NumCalls)

{

   switch(NumCalls){



      case 0:



         if(0 != idx){UnityFail( (("Incorrect command index")), (UNITY_UINT)(292));}

         else{return 100;}

         break;



      case 1:

         if(23 != idx){UnityFail( (("Incorrect command index")), (UNITY_UINT)(297));}

         else{return 60000;}

         break;



      default:

         UnityFail( (("gsmGetCmdTimeout_Callback called to many times")), (UNITY_UINT)(302));

         break;

   }

}



int VLRingBuffer_Insert_Callback (VLRINGBUFF_T * vlrb, const void * data,

                                  uint16_t num, int NumCalls)

{

   strncpy(auxBuffer,data,num);

   auxBuffer[num] = '\0';



   return 1;

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





   _Bool result;







   gsmInitTokenizer_CMockIgnoreAndReturn(350, 1);

   VLRingBuffer_Init_CMockIgnoreAndReturn(351, 1);







   result = gsmInitEngine(&engine);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(357)));};

}

void test_gsmProcessTkn(void)

{





   fsmEvent_t event;







   engine.fsmState = WAITING;

   bufSel = TKN;







   gsmTermUartSend_CMockIgnoreAndReturn(385, 0);

   VLRingBuffer_Insert_CMockIgnoreAndReturn(386, 0);

   VLRingBuffer_Flush_CMockIgnore();







   VLRingBuffer_IsFull_StubWithCallback(VLRingBuffer_IsFull_Callback);

   gsmNoChTokenizer_StubWithCallback(gsmNoChTokenizer_Callback);

   gsm232UartRecv_StubWithCallback(gsm232UartRecv_Callback);

   gsmDetectTkns_StubWithCallback(gsmDetectTkns_Callback);

   VLRingBuffer_IsEmpty_StubWithCallback(VLRingBuffer_IsEmpty_Callback);

   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback);

   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback);

   gsmUrcSearch_StubWithCallback(gsmUrcSearch_Callback);

   gsmGetCmdErrRsp_StubWithCallback(gsmGetCmdErrRsp_Callback);

   gsmGetCmdSucRsp_StubWithCallback(gsmGetCmdSucRsp_Callback);

   loadProcessTkn("\r\n+CMTI:abcde\r\n", "CMTI", "abcde", EXT_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(413), UNITY_DISPLAY_STYLE_INT);







   loadProcessTkn("\r\n+CMTI:abcde\r\n", "CMTI", "abcde", EXT_RSP);



   vlIsFull = 1;

   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(422), UNITY_DISPLAY_STYLE_INT);

   vlIsFull = 0;







   loadProcessTkn("\r\n+OK\r\n", "OK", "", BASIC_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(431), UNITY_DISPLAY_STYLE_INT);







   loadProcessTkn("abcde\r\n", "DATA", "abcde", DATA_BLOCK_P);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(439), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT"));

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("AT\r", "AT", "", AUTOBAUD);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(452), UNITY_DISPLAY_STYLE_INT);



   loadProcessTkn("\r\nOK\r\n", "OK", "", BASIC_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(458), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("ATI\r", "I", "", AUTOBAUD);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(471), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("AT\r", "AT", "", AUTOBAUD);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(484), UNITY_DISPLAY_STYLE_INT);



   loadProcessTkn("\r\n+CMTI:abcde\r\n", "CMTI", "abcde", EXT_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(490), UNITY_DISPLAY_STYLE_INT);



   loadProcessTkn("\r\nOK\r\n", "OK", "", BASIC_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(496), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("\r\nOK\r\n", "OK", "", BASIC_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(509), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("\r\n+CMTI:abcde\r\n", "CMTI", "abcde", EXT_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(522), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   engine.toutCnt = 0;



   loadProcessTkn("\r\n+CMTI:abcde\r\n", "CMTI", "abcde", EXT_RSP);



   vlrb_empty = 1;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TIMEOUT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(539), UNITY_DISPLAY_STYLE_INT);



   vlrb_empty = 0;

   engine.toutCnt = 1;







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("TA\r", "", "", INVALID);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(555), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("\r\nERROR\r\n", "ERROR", "", BASIC_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_MSG_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(568), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "CIFSR", strlen("CIFSR")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 19;



   loadProcessTkn("\r\nE192.168.0.1\r\n", "192.168.0.1", "", BASIC_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(581), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("\r\nresponse\r\n", "response", "", BASIC_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_RSP)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(594), UNITY_DISPLAY_STYLE_INT);



   loadProcessTkn("\r\nOK\r\n", "OK", "", BASIC_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(600), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("\r\nresponse\r\n", "response", "", BASIC_RSP);



   vlIsFull = 1;

   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_RSP)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(614), UNITY_DISPLAY_STYLE_INT);

   vlIsFull = 0;



   loadProcessTkn("\r\nOK\r\n", "OK", "", BASIC_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(621), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("AT\r", "AT", "", AUTOBAUD);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(634), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   engine.toutCnt = 0;



   vlrb_empty = 1;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TIMEOUT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(649), UNITY_DISPLAY_STYLE_INT);



   vlrb_empty = 0;

   engine.toutCnt = 1;







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("TA\r", "", "", INVALID);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(665), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK+1;



   loadProcessTkn("\r\nresponse\r\n", "response", "", BASIC_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_FSM_OOR)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(675), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmToutCntZero(void)

{





   _Bool result;











   engine.toutCnt = 1000;

   result = gsmToutCntZero(&engine);

   if (!(result)) {} else {UnityFail( ((" Expected FALSE Was TRUE")), (UNITY_UINT)((UNITY_UINT)(700)));};







   engine.toutCnt = 0;

   result = gsmToutCntZero(&engine);

   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(706)));};



}

void test_gsmDecToutCnt(void)

{





   _Bool result;











   engine.toutCnt = 100;



   gsmDecToutCnt(&engine);



   if ((99 == engine.toutCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(733)));};







   int i;



   for(i = 0; i <10; i++){

      gsmDecToutCnt(&engine);

   }



   if ((89 == engine.toutCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(743)));};







   engine.toutCnt = 0;



   gsmDecToutCnt(&engine);



   if ((0 == engine.toutCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(751)));};



}

void test_gsmSendCmd(void)

{





   fsmEvent_t event;







   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback);

   gsmCmdSearch_StubWithCallback(gsmCmdSearch_Callback);

   gsm232UartSend_StubWithCallback(gsm232UartSend_Callback);

   gsmGetCmdTimeout_StubWithCallback(gsmGetCmdTimeout_Callback);

   engine.fsmState = WAITING;



   loadProcessTkn("AT\r", "AT", "", AUTOBAUD);



   event = gsmSendCmd(&engine, "AT\r");



   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(791), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = WAITING;



   loadProcessTkn("texto sms", "SMS_BODY", "texto sms", SMS_BODY_P);



   event = gsmSendCmd(&engine, "texto sms");



   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(801), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = WAITING;



   loadProcessTkn("ATB\r", "B", "", BASIC_CMD);



   event = gsmSendCmd(&engine, "ATB\r");



   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_UKN)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(811), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmGetCmdRsp(void)

{





   rsp_t rsp;







   bufSel = RSPB;







   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback);

   VLRingBuffer_IsEmpty_CMockExpectAndReturn(845, &(engine.rspVlRb), 0);

   loadProcessTkn("RESP1.parametros1", "RESP1", "parametros1", BASIC_RSP);



   rsp = gsmGetCmdRsp(&engine);



   if ((0 == strcmp(rsp.cmd,"RESP1"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(850)));};

   if ((0 == strcmp(rsp.par,"parametros1"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(851)));};







   VLRingBuffer_IsEmpty_CMockExpectAndReturn(855, &(engine.rspVlRb), 0);

   loadProcessTkn("RESP2.", "RESP2", "", BASIC_RSP);



   rsp = gsmGetCmdRsp(&engine);



   if ((0 == strcmp(rsp.cmd,"RESP2"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(860)));};

   if ((0 == strcmp(rsp.par,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(861)));};







   VLRingBuffer_IsEmpty_CMockExpectAndReturn(865, &(engine.rspVlRb), 1);



   rsp = gsmGetCmdRsp(&engine);



   if ((0 == strcmp(rsp.cmd,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(869)));};

   if ((0 == strcmp(rsp.par,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(870)));};



}

void test_gsmGetNoCmdRsp (void)

{





   int noRsp;











   VLRingBuffer_GetCount_CMockExpectAndReturn(893, &(engine.rspVlRb), 2);



   noRsp = gsmGetNoCmdRsp(&engine);



   UnityAssertEqualNumber((UNITY_INT)((2)), (UNITY_INT)((noRsp)), (((void *)0)), (UNITY_UINT)(897), UNITY_DISPLAY_STYLE_INT);



   VLRingBuffer_GetCount_CMockExpectAndReturn(899, &(engine.rspVlRb), 10);



   noRsp = gsmGetNoCmdRsp(&engine);



   UnityAssertEqualNumber((UNITY_INT)((10)), (UNITY_INT)((noRsp)), (((void *)0)), (UNITY_UINT)(903), UNITY_DISPLAY_STYLE_INT);



   VLRingBuffer_GetCount_CMockExpectAndReturn(905, &(engine.rspVlRb), 0);



   noRsp = gsmGetNoCmdRsp(&engine);



   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((noRsp)), (((void *)0)), (UNITY_UINT)(909), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmGetUrc(void)

{





   rsp_t rsp;







   bufSel = URC;







   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback);

   VLRingBuffer_IsEmpty_CMockExpectAndReturn(943, &(engine.urcVlRb), 0);



   loadProcessTkn("RESP1.parametros1", "RESP1", "parametros1", BASIC_RSP);



   rsp = gsmGetUrc(&engine);



   if ((0 == strcmp(rsp.cmd,"RESP1"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(949)));};

   if ((0 == strcmp(rsp.par,"parametros1"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(950)));};







   VLRingBuffer_IsEmpty_CMockExpectAndReturn(954, &(engine.urcVlRb), 0);



   loadProcessTkn("RESP2.", "RESP2", "", BASIC_RSP);



   rsp = gsmGetUrc(&engine);



   if ((0 == strcmp(rsp.cmd,"RESP2"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(960)));};

   if ((0 == strcmp(rsp.par,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(961)));};







   VLRingBuffer_IsEmpty_CMockExpectAndReturn(965, &(engine.urcVlRb), 1);



   rsp = gsmGetUrc(&engine);



   if ((0 == strcmp(rsp.cmd,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(969)));};

   if ((0 == strcmp(rsp.par,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(970)));};



}

void test_gsmGetSerialMode(void)

{





   serialMode_t mode;







   engine.serialMode = COMMAND_MODE;



   mode = gsmGetSerialMode(&engine);



   UnityAssertEqualNumber((UNITY_INT)((COMMAND_MODE)), (UNITY_INT)((mode)), (((void *)0)), (UNITY_UINT)(995), UNITY_DISPLAY_STYLE_INT);



   engine.serialMode = DATA_MODE;



   mode = gsmGetSerialMode(&engine);



   UnityAssertEqualNumber((UNITY_INT)((DATA_MODE)), (UNITY_INT)((mode)), (((void *)0)), (UNITY_UINT)(1001), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmSetSerialMode(void)

{





   serialMode_t mode;







   gsmSetSerialMode(&engine,COMMAND_MODE);



   mode = engine.serialMode;



   UnityAssertEqualNumber((UNITY_INT)((COMMAND_MODE)), (UNITY_INT)((mode)), (((void *)0)), (UNITY_UINT)(1026), UNITY_DISPLAY_STYLE_INT);



   gsmSetSerialMode(&engine,DATA_MODE);



   mode = engine.serialMode;



   UnityAssertEqualNumber((UNITY_INT)((DATA_MODE)), (UNITY_INT)((mode)), (((void *)0)), (UNITY_UINT)(1032), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmRecordUrc(void)

{





   uint8_t result;







   engine.fsmState = WAITING;

   bufSel = TKN;







   gsmTermUartSend_CMockIgnoreAndReturn(1057, 0);







   gsmNoChTokenizer_StubWithCallback(gsmNoChTokenizer_Callback);

   gsm232UartRecv_StubWithCallback(gsm232UartRecv_Callback);

   gsmDetectTkns_StubWithCallback(gsmDetectTkns_Callback);

   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback);

   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback);

   gsmUrcSearch_StubWithCallback(gsmUrcSearch_Callback);

   VLRingBuffer_Insert_StubWithCallback(VLRingBuffer_Insert_Callback);







   VLRingBuffer_IsEmpty_CMockExpectAndReturn(1071, &(engine.tknVlRb), 0);

   VLRingBuffer_IsFull_CMockExpectAndReturn(1072, &(engine.urcVlRb), 0);



   loadProcessTkn("\r\n+CMTI:abcde\r\n", "CMTI", "abcde", EXT_RSP);



   result = gsmProcessTkn(&engine);



   UnityAssertEqualString((const char*)(("CMTI.abcde")), (const char*)((auxBuffer)), (((void *)0)), (UNITY_UINT)(1078));



}
