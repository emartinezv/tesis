#include "build/temp/_test_gsmEngine.c"
#include "ring_buffer.h"
#include "mock_gsmCommands.h"
#include "mock_vl_ring_buffer.h"
#include "mock_gsmParser.h"
#include "mock_gsmComms.h"
#include "mock_gsmTokenizer.h"
#include "gsmEngine.h"
#include "unity.h"


















typedef struct _tknTypeSize {

   uint8_t const * tkn;

   uint8_t const * cmd;

   uint8_t const * par;

   tknTypeParser_t type;

   int size;

} tknTypeSize_s;















gsmEngine_t engine;



uint16_t tknzerNoCh = 0;



uint8_t tknTest[280+20+1];

uint8_t cmdTest[20+1];

uint8_t parTest[280+1];



uint16_t tknSizeTest;

tknTypeParser_t tknTypeTest = AUTOBAUD;



int vlrb_empty = 0;













static uint8_t auxBuffer[100];



static gsmEngine_t engine2;



static tknTypeSize_s tknTypeSizeV [] = {





   {"\r\n+CMTI:abcde\r\n","CMTI","abcde",EXT_RSP,16},





   {"\r\nOK\r\n","OK","",BASIC_RSP,7},





   {"abcde\r\n","DATA","abcde",DATA_BLOCK_P,8},





   {"AT\r","AT","",AUTOBAUD,4},

   {"\r\nOK\r\n","OK","",BASIC_RSP,7},





   {"ATI\r","I","",BASIC_CMD,5},





   {"AT\r","AT","",AUTOBAUD,4},

   {"\r\n+CMTI:abcde\r\n","CMTI","abcde",EXT_RSP,16},

   {"\r\nOK\r\n","OK","",BASIC_RSP,7},





   {"\r\nOK\r\n","OK","",BASIC_RSP,7},





   {"\r\n+CMTI:abcde\r\n","CMTI","abcde",EXT_RSP,16},





   {"TA\r","","",INVALID,4},





   {"\r\nERROR\r\n","ERROR","",BASIC_RSP,10},





   {"\r\n192.168.0.1\r\n","192.168.0.1","",BASIC_RSP,16},





   {"\r\nRSP\r\n","RSP","",BASIC_RSP,8},

   {"\r\nOK\r\n","OK","",BASIC_RSP,7},





   {"AT\r","AT","",AUTOBAUD,4},





   {"TA\r","","",INVALID,4},





   {"\r\nRSP\r\n","RSP","",BASIC_RSP,8}



};



static tknTypeSize_s tknTypeV [] = {





   {"AT\r","AT","",AUTOBAUD,0},





   {"texto sms","SMS_BODY","texto sms",SMS_BODY_P,0},





   {"ATB\r","B","",BASIC_CMD,0}



};



static int bufEmptyV [] = {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0};













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









uint16_t gsmNoChTokenizer_Callback(int NumCalls)

{

   return tknzerNoCh;

}



int gsm232UartRecv_Callback (uint8_t * const buffer, int n, int NumCalls)

{

   UnityAssertEqualNumber((UNITY_INT)((tknzerNoCh)), (UNITY_INT)((n)), (((void *)0)), (UNITY_UINT)(168), UNITY_DISPLAY_STYLE_INT);



   return tknSizeTest;

}



void gsmDetectTkns_Callback (VLRINGBUFF_T * const tknVlRb, uint16_t nch,

                             uint8_t const * const buffer, int NumCalls)

{

   if ((tknVlRb == &(engine.tknVlRb))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(176)));};

   UnityAssertEqualNumber((UNITY_INT)((tknSizeTest)), (UNITY_INT)((nch)), (((void *)0)), (UNITY_UINT)(177), UNITY_DISPLAY_STYLE_INT);

}



int VLRingBuffer_IsEmpty_Callback (VLRINGBUFF_T * vlrb, int NumCalls)

{

   if ((&(engine.tknVlRb) == vlrb)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(182)));}



   return vlrb_empty;

}



int VLRingBuffer_Pop_Callback (VLRINGBUFF_T * vlrb, void * data, uint16_t cap,

                               int NumCalls)

{

   if ((vlrb == &(engine.tknVlRb))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(190)));};

   strncpy(data, tknTest, strlen(tknTest));



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



      default:



         return ("???");



   }



}



tknTypeParser_t gsmParseTkn_Callback2 (uint8_t const * const tkn,

                                       uint8_t * cmd, uint8_t * par,

                                       uint16_t tknLen, int NumCalls)

{

   if(NumCalls > (sizeof(tknTypeV)/sizeof(tknTypeSize_s))){

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(262));

   }



   strncpy(cmd, tknTypeV[NumCalls].cmd,

           strlen(tknTypeV[NumCalls].cmd));

   cmd[strlen(tknTypeV[NumCalls].cmd)] = '\0';



   if(strlen(tknTypeV[NumCalls].par) > 0){

      strncpy(par, tknTypeV[NumCalls].par,

              strlen(tknTypeV[NumCalls].par));

      par[strlen(tknTypeV[NumCalls].par)] = '\0';

   }

   else{

      par[0]='\0';

   }



   return (tknTypeV[NumCalls].type);

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

         if(0 != strcmp(buffer,"AT\r")){UnityFail( (("Not sending correct string through UART!")), (UNITY_UINT)(294));}

         else{return 3;}

         break;



      case 1:

         if(0 != strcmp(buffer,"texto sms")){UnityFail( (("Not sending correct string through UART!")), (UNITY_UINT)(299));}

         else{return 9;}

         break;



      case 2:

         if(0 != strcmp(buffer,"\x1A")){UnityFail( (("Not sending correct string through UART!")), (UNITY_UINT)(304));}

         else{return 1;}

         break;



      default:

         UnityFail( (("gsm232UartSend_Callback called to many times")), (UNITY_UINT)(309));

         break;

   }

}



uint32_t gsmGetCmdTimeout_Callback (uint16_t idx, int NumCalls)

{

   switch(NumCalls){



      case 0:



         if(0 != idx){UnityFail( (("Incorrect command index")), (UNITY_UINT)(320));}

         else{return 100;}

         break;



      case 1:

         if(23 != idx){UnityFail( (("Incorrect command index")), (UNITY_UINT)(325));}

         else{return 60000;}

         break;



      default:

         UnityFail( (("gsmGetCmdTimeout_Callback called to many times")), (UNITY_UINT)(330));

         break;

   }

}



int VLRingBuffer_Pop_Callback2 (VLRINGBUFF_T * vlrb, void * data, uint16_t cap,

                               int NumCalls)

{

   switch(NumCalls){



      case 0:

         strncpy(data,"RESP1.parametros1",strlen("RESP1.parametros1"));

         return(17);

         break;



      case 1:

         strncpy(data,"RESP2.",strlen("RESP2."));

         return(6);

         break;



      default:

         UnityFail( (("VLRingBuffer_Pop_Callback2 called to many times")), (UNITY_UINT)(351));

         break;

   }

}



tknTypeParser_t gsmParseTkn_Callback3 (uint8_t const * const tkn,

                                       uint8_t * cmd, uint8_t * par,

                                       uint16_t tknLen, int NumCalls)

{

   strncpy(cmd,"CMTI",strlen("CMTI"));

   cmd[strlen("CMTI")]='\0';

   strncpy(par,"abcde",strlen("abcde"));

   par[strlen("abcde")]='\0';

   return(EXT_RSP);

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







   gsmInitTokenizer_CMockIgnoreAndReturn(410, 1);

   VLRingBuffer_Init_CMockIgnoreAndReturn(411, 1);







   result = gsmInitEngine(&engine);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(417)));};

}

void test_gsmProcessTkn(void)

{





   fsmEvent_t event;







   engine.fsmState = WAITING;







   gsmTermUartSend_CMockIgnoreAndReturn(442, 0);

   VLRingBuffer_IsFull_CMockIgnoreAndReturn(443, 0);

   VLRingBuffer_Insert_CMockIgnoreAndReturn(444, 0);

   VLRingBuffer_Flush_CMockIgnore();







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



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(470), UNITY_DISPLAY_STYLE_INT);







   loadProcessTkn("\r\n+OK\r\n", "OK", "", BASIC_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(478), UNITY_DISPLAY_STYLE_INT);







   loadProcessTkn("abcde\r\n", "DATA", "abcde", DATA_BLOCK_P);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(486), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT"));

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("AT\r", "AT", "", AUTOBAUD);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(499), UNITY_DISPLAY_STYLE_INT);



   loadProcessTkn("\r\nOK\r\n", "OK", "", BASIC_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(505), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("ATI\r", "I", "", AUTOBAUD);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(518), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("AT\r", "AT", "", AUTOBAUD);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(531), UNITY_DISPLAY_STYLE_INT);



   loadProcessTkn("\r\n+CMTI:abcde\r\n", "CMTI", "abcde", EXT_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(537), UNITY_DISPLAY_STYLE_INT);



   loadProcessTkn("\r\nOK\r\n", "OK", "", BASIC_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(543), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("\r\nOK\r\n", "OK", "", BASIC_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(556), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("\r\n+CMTI:abcde\r\n", "CMTI", "abcde", EXT_RSP);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(569), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   engine.toutCnt = 0;



   loadProcessTkn("\r\n+CMTI:abcde\r\n", "CMTI", "abcde", EXT_RSP);



   vlrb_empty = 1;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TIMEOUT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(586), UNITY_DISPLAY_STYLE_INT);



   vlrb_empty = 0;







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   loadProcessTkn("TA\r", "", "", INVALID);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(601), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmToutCntZero(void)

{





   gsmEngine_t engine;

   _Bool result;







   engine.toutCnt = 1000;

   result = gsmToutCntZero(&engine);

   if (!(result)) {} else {UnityFail( ((" Expected FALSE Was TRUE")), (UNITY_UINT)((UNITY_UINT)(724)));};



   engine.toutCnt = 0;

   result = gsmToutCntZero(&engine);

   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(728)));};



}

void test_gsmDecToutCnt(void)

{





   gsmEngine_t engine;

   _Bool result;







   engine.toutCnt = 100;



   gsmDecToutCnt(&engine);



   if ((99 == engine.toutCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(754)));};



   int i;



   for(i = 0; i <10; i++){

      gsmDecToutCnt(&engine);

   }



   if ((89 == engine.toutCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(762)));};



   engine.toutCnt = 0;



   gsmDecToutCnt(&engine);



   if ((0 == engine.toutCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(768)));};



}

void test_gsmSendCmd(void)

{





   gsmEngine_t engine;

   fsmEvent_t event;







   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback2);

   gsmCmdSearch_StubWithCallback(gsmCmdSearch_Callback);

   gsm232UartSend_StubWithCallback(gsm232UartSend_Callback);

   gsmGetCmdTimeout_StubWithCallback(gsmGetCmdTimeout_Callback);







   engine.fsmState = WAITING;

   event = gsmSendCmd(&engine, "AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(800), UNITY_DISPLAY_STYLE_INT);



   engine.fsmState = WAITING;

   event = gsmSendCmd(&engine, "texto sms");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(804), UNITY_DISPLAY_STYLE_INT);



   engine.fsmState = WAITING;

   event = gsmSendCmd(&engine, "ATB\r");

   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_UKN)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(808), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmGetCmdRsp(void)

{





   gsmEngine_t engine;

   rsp_t rsp;

   VLRINGBUFF_T * buffer;







   buffer = &(engine.rspVlRb);







   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback2);







   VLRingBuffer_IsEmpty_CMockExpectAndReturn(839, buffer, 0);

   rsp = gsmGetCmdRsp(&engine);

   if ((0 == strcmp(rsp.cmd,"RESP1"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(841)));};

   if ((0 == strcmp(rsp.par,"parametros1"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(842)));};



   VLRingBuffer_IsEmpty_CMockExpectAndReturn(844, buffer, 0);

   rsp = gsmGetCmdRsp(&engine);

   if ((0 == strcmp(rsp.cmd,"RESP2"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(846)));};

   if ((0 == strcmp(rsp.par,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(847)));};



   VLRingBuffer_IsEmpty_CMockExpectAndReturn(849, buffer, 1);

   rsp = gsmGetCmdRsp(&engine);

   if ((0 == strcmp(rsp.cmd,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(851)));};

   if ((0 == strcmp(rsp.par,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(852)));};



}

void test_gsmGetNoCmdRsp (void)

{





   gsmEngine_t engine;

   VLRINGBUFF_T * buffer;

   int noRsp;







   buffer = &(engine.rspVlRb);







   VLRingBuffer_GetCount_CMockExpectAndReturn(879, buffer, 2);

   noRsp = gsmGetNoCmdRsp(&engine);

   UnityAssertEqualNumber((UNITY_INT)((2)), (UNITY_INT)((noRsp)), (((void *)0)), (UNITY_UINT)(881), UNITY_DISPLAY_STYLE_INT);



   VLRingBuffer_GetCount_CMockExpectAndReturn(883, buffer, 10);

   noRsp = gsmGetNoCmdRsp(&engine);

   UnityAssertEqualNumber((UNITY_INT)((10)), (UNITY_INT)((noRsp)), (((void *)0)), (UNITY_UINT)(885), UNITY_DISPLAY_STYLE_INT);



   VLRingBuffer_GetCount_CMockExpectAndReturn(887, buffer, 0);

   noRsp = gsmGetNoCmdRsp(&engine);

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((noRsp)), (((void *)0)), (UNITY_UINT)(889), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmGetUrc(void)

{





   gsmEngine_t engine;

   rsp_t rsp;

   VLRINGBUFF_T * buffer;







   buffer = &(engine.urcVlRb);







   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback2);







   VLRingBuffer_IsEmpty_CMockExpectAndReturn(920, buffer, 0);

   rsp = gsmGetUrc(&engine);

   if ((0 == strcmp(rsp.cmd,"RESP1"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(922)));};

   if ((0 == strcmp(rsp.par,"parametros1"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(923)));};



   VLRingBuffer_IsEmpty_CMockExpectAndReturn(925, buffer, 0);

   rsp = gsmGetUrc(&engine);

   if ((0 == strcmp(rsp.cmd,"RESP2"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(927)));};

   if ((0 == strcmp(rsp.par,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(928)));};



   VLRingBuffer_IsEmpty_CMockExpectAndReturn(930, buffer, 1);

   rsp = gsmGetUrc(&engine);

   if ((0 == strcmp(rsp.cmd,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(932)));};

   if ((0 == strcmp(rsp.par,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(933)));};



}

void test_gsmGetSerialMode(void)

{





   gsmEngine_t engine;

   serialMode_t mode;







   engine.serialMode = COMMAND_MODE;

   mode = gsmGetSerialMode(&engine);

   UnityAssertEqualNumber((UNITY_INT)((COMMAND_MODE)), (UNITY_INT)((mode)), (((void *)0)), (UNITY_UINT)(957), UNITY_DISPLAY_STYLE_INT);



   engine.serialMode = DATA_MODE;

   mode = gsmGetSerialMode(&engine);

   UnityAssertEqualNumber((UNITY_INT)((DATA_MODE)), (UNITY_INT)((mode)), (((void *)0)), (UNITY_UINT)(961), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmSetSerialMode(void)

{





   gsmEngine_t engine;

   serialMode_t mode;







   gsmSetSerialMode(&engine,COMMAND_MODE);

   mode = engine.serialMode;

   UnityAssertEqualNumber((UNITY_INT)((COMMAND_MODE)), (UNITY_INT)((mode)), (((void *)0)), (UNITY_UINT)(985), UNITY_DISPLAY_STYLE_INT);



   gsmSetSerialMode(&engine,DATA_MODE);

   mode = engine.serialMode;

   UnityAssertEqualNumber((UNITY_INT)((DATA_MODE)), (UNITY_INT)((mode)), (((void *)0)), (UNITY_UINT)(989), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmRecordUrc(void)

{





   uint8_t result;







   engine2.fsmState = WAITING;







   gsmTermUartSend_CMockIgnoreAndReturn(1013, 0);

   gsmNoChTokenizer_CMockIgnoreAndReturn(1014, 10);

   gsm232UartRecv_CMockIgnoreAndReturn(1015, 10);

   gsmDetectTkns_CMockIgnore();

   VLRingBuffer_Pop_CMockIgnoreAndReturn(1017, 0);

   gsmUrcSearch_CMockIgnoreAndReturn(1018, 1);







   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback3);

   VLRingBuffer_Insert_StubWithCallback(VLRingBuffer_Insert_Callback);







   VLRingBuffer_IsEmpty_CMockExpectAndReturn(1027, &(engine2.tknVlRb), 0);

   VLRingBuffer_IsFull_CMockExpectAndReturn(1028, &(engine2.urcVlRb), 0);

   result = gsmProcessTkn(&engine2);



   UnityAssertEqualString((const char*)(("CMTI.abcde")), (const char*)((auxBuffer)), (((void *)0)), (UNITY_UINT)(1031));



}
