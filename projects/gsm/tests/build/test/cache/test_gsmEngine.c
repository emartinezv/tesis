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

   tknTypeParser_e type;

   int size;

} tknTypeSize_s;











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











int gsm232UartRecv_Callback (uint8_t * const buffer, int n, int NumCalls)

{

   UnityAssertEqualNumber((UNITY_INT)((10)), (UNITY_INT)((n)), (((void *)0)), (UNITY_UINT)(123), UNITY_DISPLAY_STYLE_INT);



   return 10;

}



int VLRingBuffer_IsEmpty_Callback (VLRINGBUFF_T * vlrb, int NumCalls)

{

   if(NumCalls > (sizeof(bufEmptyV)/sizeof(int))){

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(131));

   }



   return (bufEmptyV[NumCalls]);

}



int VLRingBuffer_Pop_Callback (VLRINGBUFF_T * vlrb, void * data, uint16_t cap,

                               int NumCalls)

{

   if(NumCalls > (sizeof(tknTypeSizeV)/sizeof(tknTypeSize_s))){

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(141));

   }



   strncpy(data, tknTypeSizeV[NumCalls].tkn,

           strlen(tknTypeSizeV[NumCalls].tkn));



   return (tknTypeSizeV[NumCalls].size);

}



tknTypeParser_e gsmParseTkn_Callback (uint8_t const * const tkn, uint8_t * cmd,

                                      uint8_t * par, uint16_t tknLen,

                                      int NumCalls)

{

   if(NumCalls > (sizeof(tknTypeSizeV)/sizeof(tknTypeSize_s))){

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(155));

   }



   strncpy(cmd, tknTypeSizeV[NumCalls].cmd,

           strlen(tknTypeSizeV[NumCalls].cmd));

   cmd[strlen(tknTypeSizeV[NumCalls].cmd)] = '\0';



   if(strlen(tknTypeSizeV[NumCalls].par) > 0){

      strncpy(par, tknTypeSizeV[NumCalls].par,

              strlen(tknTypeSizeV[NumCalls].par));

      par[strlen(tknTypeSizeV[NumCalls].par)] = '\0';

   }

   else{

      par[0]='\0';

   }



   return (tknTypeSizeV[NumCalls].type);

}



uint8_t gsmUrcSearch_Callback (uint8_t const * const urc, int NumCalls)

{

   if(0 == strcmp(urc,"CMTI")){return 1;}



   return 0;

}



const uint8_t const * gsmGetCmdErrRsp_Callback (uint16_t idx, int Numcalls)

{

   return ("-ERROR-CMS ERROR-");

}



const uint8_t const * gsmGetCmdSucRsp_Callback (uint16_t idx, int Numcalls)

{

   if(0 == idx){

      return ("-OK-");

   }

   else if(19 == idx){

      return ("");

   }

   else{

      UnityFail( (("gsmGetCmdSucRsp_Callback called incorrectly")), (UNITY_UINT)(195));

   }

}



tknTypeParser_e gsmParseTkn_Callback2 (uint8_t const * const tkn,

                                       uint8_t * cmd, uint8_t * par,

                                       uint16_t tknLen, int NumCalls)

{

   if(NumCalls > (sizeof(tknTypeV)/sizeof(tknTypeSize_s))){

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(204));

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

         if(0 != strcmp(buffer,"AT\r")){UnityFail( (("Not sending correct string through UART!")), (UNITY_UINT)(236));}

         else{return 3;}

         break;



      case 1:

         if(0 != strcmp(buffer,"texto sms")){UnityFail( (("Not sending correct string through UART!")), (UNITY_UINT)(241));}

         else{return 9;}

         break;



      case 2:

         if(0 != strcmp(buffer,"\x1A")){UnityFail( (("Not sending correct string through UART!")), (UNITY_UINT)(246));}

         else{return 1;}

         break;



      default:

         UnityFail( (("gsm232UartSend_Callback called to many times")), (UNITY_UINT)(251));

         break;

   }

}



uint32_t gsmGetCmdTimeout_Callback (uint16_t idx, int NumCalls)

{

   switch(NumCalls){



      case 0:



         if(0 != idx){UnityFail( (("Incorrect command index")), (UNITY_UINT)(262));}

         else{return 100;}

         break;



      case 1:

         if(23 != idx){UnityFail( (("Incorrect command index")), (UNITY_UINT)(267));}

         else{return 60000;}

         break;



      default:

         UnityFail( (("gsmGetCmdTimeout_Callback called to many times")), (UNITY_UINT)(272));

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

         UnityFail( (("VLRingBuffer_Pop_Callback2 called to many times")), (UNITY_UINT)(293));

         break;

   }

}



tknTypeParser_e gsmParseTkn_Callback3 (uint8_t const * const tkn,

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





   gsmEngine_t engine;

   _Bool result;







   gsmInitTokenizer_CMockIgnoreAndReturn(353, 1);

   VLRingBuffer_Init_CMockIgnoreAndReturn(354, 1);







   result = gsmInitEngine(&engine);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(360)));};

}

void test_gsmProcessTkn(void)

{





   gsmEngine_t engine;

   fsmEvent_e event;







   engine.fsmState = WAITING;







   gsmTermUartSend_CMockIgnoreAndReturn(386, 0);

   gsmNoChTokenizer_CMockIgnoreAndReturn(387, 10);

   gsm232UartRecv_StubWithCallback(gsm232UartRecv_Callback);

   gsmDetectTkns_CMockIgnore();

   VLRingBuffer_IsEmpty_StubWithCallback(VLRingBuffer_IsEmpty_Callback);

   VLRingBuffer_IsFull_CMockIgnoreAndReturn(391, 0);

   VLRingBuffer_Insert_CMockIgnoreAndReturn(392, 0);

   VLRingBuffer_Flush_CMockIgnore();

   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback);

   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback);

   gsmUrcSearch_StubWithCallback(gsmUrcSearch_Callback);

   gsmGetCmdErrRsp_StubWithCallback(gsmGetCmdErrRsp_Callback);

   gsmGetCmdSucRsp_StubWithCallback(gsmGetCmdSucRsp_Callback);













   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(407), UNITY_DISPLAY_STYLE_INT);







   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(413), UNITY_DISPLAY_STYLE_INT);







   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(419), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(430), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(434), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(445), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(456), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(460), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(464), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(475), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(486), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   engine.toutCnt = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TIMEOUT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(499), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(510), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_MSG_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(521), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "CIFSR", strlen("CIFSR")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 19;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(532), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_RSP)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(543), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(547), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(558), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   engine.toutCnt = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TIMEOUT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(571), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(582), UNITY_DISPLAY_STYLE_INT);







   engine.toutCnt = 1000;

   engine.fsmState = CMD_ACK+1;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_FSM_OOR)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(591), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmToutCntZero(void)

{





   gsmEngine_t engine;

   _Bool result;







   engine.toutCnt = 1000;

   result = gsmToutCntZero(&engine);

   if (!(result)) {} else {UnityFail( ((" Expected FALSE Was TRUE")), (UNITY_UINT)((UNITY_UINT)(615)));};



   engine.toutCnt = 0;

   result = gsmToutCntZero(&engine);

   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(619)));};



}

void test_gsmDecToutCnt(void)

{





   gsmEngine_t engine;

   _Bool result;







   engine.toutCnt = 100;



   gsmDecToutCnt(&engine);



   if ((99 == engine.toutCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(645)));};



   int i;



   for(i = 0; i <10; i++){

      gsmDecToutCnt(&engine);

   }



   if ((89 == engine.toutCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(653)));};



   engine.toutCnt = 0;



   gsmDecToutCnt(&engine);



   if ((0 == engine.toutCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(659)));};



}

void test_gsmSendCmd(void)

{





   gsmEngine_t engine;

   fsmEvent_e event;







   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback2);

   gsmCmdSearch_StubWithCallback(gsmCmdSearch_Callback);

   gsm232UartSend_StubWithCallback(gsm232UartSend_Callback);

   gsmGetCmdTimeout_StubWithCallback(gsmGetCmdTimeout_Callback);







   engine.fsmState = WAITING;

   event = gsmSendCmd(&engine, "AT\r");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(691), UNITY_DISPLAY_STYLE_INT);



   engine.fsmState = WAITING;

   event = gsmSendCmd(&engine, "texto sms");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(695), UNITY_DISPLAY_STYLE_INT);



   engine.fsmState = WAITING;

   event = gsmSendCmd(&engine, "ATB\r");

   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_UKN)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(699), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmGetCmdRsp(void)

{





   gsmEngine_t engine;

   rsp_t rsp;

   VLRINGBUFF_T * buffer;







   buffer = &(engine.rspVlRb);







   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback2);







   VLRingBuffer_IsEmpty_CMockExpectAndReturn(730, buffer, 0);

   rsp = gsmGetCmdRsp(&engine);

   if ((0 == strcmp(rsp.cmd,"RESP1"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(732)));};

   if ((0 == strcmp(rsp.par,"parametros1"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(733)));};



   VLRingBuffer_IsEmpty_CMockExpectAndReturn(735, buffer, 0);

   rsp = gsmGetCmdRsp(&engine);

   if ((0 == strcmp(rsp.cmd,"RESP2"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(737)));};

   if ((0 == strcmp(rsp.par,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(738)));};



   VLRingBuffer_IsEmpty_CMockExpectAndReturn(740, buffer, 1);

   rsp = gsmGetCmdRsp(&engine);

   if ((0 == strcmp(rsp.cmd,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(742)));};

   if ((0 == strcmp(rsp.par,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(743)));};



}

void test_gsmGetNoCmdRsp (void)

{





   gsmEngine_t engine;

   VLRINGBUFF_T * buffer;

   int noRsp;







   buffer = &(engine.rspVlRb);







   VLRingBuffer_GetCount_CMockExpectAndReturn(770, buffer, 2);

   noRsp = gsmGetNoCmdRsp(&engine);

   UnityAssertEqualNumber((UNITY_INT)((2)), (UNITY_INT)((noRsp)), (((void *)0)), (UNITY_UINT)(772), UNITY_DISPLAY_STYLE_INT);



   VLRingBuffer_GetCount_CMockExpectAndReturn(774, buffer, 10);

   noRsp = gsmGetNoCmdRsp(&engine);

   UnityAssertEqualNumber((UNITY_INT)((10)), (UNITY_INT)((noRsp)), (((void *)0)), (UNITY_UINT)(776), UNITY_DISPLAY_STYLE_INT);



   VLRingBuffer_GetCount_CMockExpectAndReturn(778, buffer, 0);

   noRsp = gsmGetNoCmdRsp(&engine);

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((noRsp)), (((void *)0)), (UNITY_UINT)(780), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmGetUrc(void)

{





   gsmEngine_t engine;

   rsp_t rsp;

   VLRINGBUFF_T * buffer;







   buffer = &(engine.urcVlRb);







   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback2);







   VLRingBuffer_IsEmpty_CMockExpectAndReturn(811, buffer, 0);

   rsp = gsmGetUrc(&engine);

   if ((0 == strcmp(rsp.cmd,"RESP1"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(813)));};

   if ((0 == strcmp(rsp.par,"parametros1"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(814)));};



   VLRingBuffer_IsEmpty_CMockExpectAndReturn(816, buffer, 0);

   rsp = gsmGetUrc(&engine);

   if ((0 == strcmp(rsp.cmd,"RESP2"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(818)));};

   if ((0 == strcmp(rsp.par,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(819)));};



   VLRingBuffer_IsEmpty_CMockExpectAndReturn(821, buffer, 1);

   rsp = gsmGetUrc(&engine);

   if ((0 == strcmp(rsp.cmd,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(823)));};

   if ((0 == strcmp(rsp.par,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(824)));};



}

void test_gsmGetSerialMode(void)

{





   gsmEngine_t engine;

   serialMode_e mode;







   engine.serialMode = COMMAND_MODE;

   mode = gsmGetSerialMode(&engine);

   UnityAssertEqualNumber((UNITY_INT)((COMMAND_MODE)), (UNITY_INT)((mode)), (((void *)0)), (UNITY_UINT)(848), UNITY_DISPLAY_STYLE_INT);



   engine.serialMode = DATA_MODE;

   mode = gsmGetSerialMode(&engine);

   UnityAssertEqualNumber((UNITY_INT)((DATA_MODE)), (UNITY_INT)((mode)), (((void *)0)), (UNITY_UINT)(852), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmSetSerialMode(void)

{





   gsmEngine_t engine;

   serialMode_e mode;







   gsmSetSerialMode(&engine,COMMAND_MODE);

   mode = engine.serialMode;

   UnityAssertEqualNumber((UNITY_INT)((COMMAND_MODE)), (UNITY_INT)((mode)), (((void *)0)), (UNITY_UINT)(876), UNITY_DISPLAY_STYLE_INT);



   gsmSetSerialMode(&engine,DATA_MODE);

   mode = engine.serialMode;

   UnityAssertEqualNumber((UNITY_INT)((DATA_MODE)), (UNITY_INT)((mode)), (((void *)0)), (UNITY_UINT)(880), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmRecordUrc(void)

{





   uint8_t result;







   engine2.fsmState = WAITING;







   gsmTermUartSend_CMockIgnoreAndReturn(904, 0);

   gsmNoChTokenizer_CMockIgnoreAndReturn(905, 10);

   gsm232UartRecv_CMockIgnoreAndReturn(906, 10);

   gsmDetectTkns_CMockIgnore();

   VLRingBuffer_Pop_CMockIgnoreAndReturn(908, 0);

   gsmUrcSearch_CMockIgnoreAndReturn(909, 1);







   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback3);

   VLRingBuffer_Insert_StubWithCallback(VLRingBuffer_Insert_Callback);







   VLRingBuffer_IsEmpty_CMockExpectAndReturn(918, &(engine2.tknVlRb), 0);

   VLRingBuffer_IsFull_CMockExpectAndReturn(919, &(engine2.urcVlRb), 0);

   result = gsmProcessTkn(&engine2);



   UnityAssertEqualString((const char*)(("CMTI.abcde")), (const char*)((auxBuffer)), (((void *)0)), (UNITY_UINT)(922));



}
