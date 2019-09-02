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











int VLRingBuffer_IsEmpty_Callback (VLRINGBUFF_T * vlrb, int NumCalls)

{

   if(NumCalls > (sizeof(bufEmptyV)/sizeof(int))){

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(120));

   }



   return (bufEmptyV[NumCalls]);

}



int VLRingBuffer_Pop_Callback (VLRINGBUFF_T * vlrb, void * data, uint16_t cap,

                               int NumCalls)

{

   if(NumCalls > (sizeof(tknTypeSizeV)/sizeof(tknTypeSize_s))){

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(130));

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

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(144));

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

      UnityFail( (("gsmGetCmdSucRsp_Callback called incorrectly")), (UNITY_UINT)(184));

   }

}



tknTypeParser_e gsmParseTkn_Callback2 (uint8_t const * const tkn,

                                       uint8_t * cmd, uint8_t * par,

                                       uint16_t tknLen, int NumCalls)

{

   if(NumCalls > (sizeof(tknTypeV)/sizeof(tknTypeSize_s))){

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(193));

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



         if(0 != strcmp(buffer,"AT\r")){

            UnityFail( (("Not sending correct string through UART!")), (UNITY_UINT)(227));

         }

         else{

            return 3;

         }



         break;



      case 1:



         if(0 != strcmp(buffer,"texto sms")){

            UnityFail( (("Not sending correct string through UART!")), (UNITY_UINT)(238));

         }

         else{

            return 9;

         }



         break;



      case 2:



         if(0 != strcmp(buffer,"\x1A")){

            UnityFail( (("Not sending correct string through UART!")), (UNITY_UINT)(249));

         }

         else{

            return 1;

         }



         break;



      default:



         UnityFail( (("gsm232UartSend_Callback called to many times")), (UNITY_UINT)(259));



         break;

   }

}



uint32_t gsmGetCmdTimeout_Callback (uint16_t idx, int NumCalls)

{

   switch(NumCalls){



      case 0:



         if(0 != idx){

            UnityFail( (("Incorrect command index")), (UNITY_UINT)(272));

         }

         else{

            return 100;

         }



         break;



      case 1:



         if(23 != idx){

            UnityFail( (("Incorrect command index")), (UNITY_UINT)(283));

         }

         else{

            return 60000;

         }



         break;



      default:



         UnityFail( (("gsmGetCmdTimeout_Callback called to many times")), (UNITY_UINT)(293));



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



         UnityFail( (("VLRingBuffer_Pop_Callback2 called to many times")), (UNITY_UINT)(320));



         break;

   }

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







   gsmInitTokenizer_CMockIgnoreAndReturn(361, 1);

   VLRingBuffer_Init_CMockIgnoreAndReturn(362, 1);







   result = gsmInitEngine(&engine);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(368)));};

}

void test_gsmProcessTkn(void)

{





   gsmEngine_t engine;

   fsmEvent_e event;







   engine.fsmState = WAITING;







   gsmTermUartSend_CMockIgnoreAndReturn(394, 0);

   gsmDetectTkns_CMockIgnore();

   VLRingBuffer_IsEmpty_StubWithCallback(VLRingBuffer_IsEmpty_Callback);

   VLRingBuffer_IsFull_CMockIgnoreAndReturn(397, 0);

   VLRingBuffer_Insert_CMockIgnoreAndReturn(398, 0);

   VLRingBuffer_Flush_CMockIgnore();

   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback);

   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback);

   gsmUrcSearch_StubWithCallback(gsmUrcSearch_Callback);

   gsmGetCmdErrRsp_StubWithCallback(gsmGetCmdErrRsp_Callback);

   gsmGetCmdSucRsp_StubWithCallback(gsmGetCmdSucRsp_Callback);











   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(412), UNITY_DISPLAY_STYLE_INT);







   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(418), UNITY_DISPLAY_STYLE_INT);







   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(424), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(435), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(439), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(450), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(461), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(465), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(469), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(480), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(491), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   engine.toutCnt = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TIMEOUT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(504), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(515), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_MSG_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(526), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "CIFSR", strlen("CIFSR")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 19;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(537), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_RSP)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(548), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(552), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(563), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   engine.toutCnt = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TIMEOUT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(576), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(587), UNITY_DISPLAY_STYLE_INT);







   engine.toutCnt = 1000;

   engine.fsmState = CMD_ACK+1;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_FSM_OOR)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(596), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmToutCntZero(void)

{





   gsmEngine_t engine;

   _Bool result;







   engine.toutCnt = 1000;

   result = gsmToutCntZero(&engine);

   if (!(result)) {} else {UnityFail( ((" Expected FALSE Was TRUE")), (UNITY_UINT)((UNITY_UINT)(620)));};



   engine.toutCnt = 0;

   result = gsmToutCntZero(&engine);

   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(624)));};



}

void test_gsmDecToutCnt(void)

{





   gsmEngine_t engine;

   _Bool result;







   engine.toutCnt = 100;



   gsmDecToutCnt(&engine);



   if ((99 == engine.toutCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(650)));};



   int i;



   for(i = 0; i <10; i++){

      gsmDecToutCnt(&engine);

   }



   if ((89 == engine.toutCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(658)));};



   engine.toutCnt = 0;



   gsmDecToutCnt(&engine);



   if ((0 == engine.toutCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(664)));};



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

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(696), UNITY_DISPLAY_STYLE_INT);



   engine.fsmState = WAITING;

   event = gsmSendCmd(&engine, "texto sms");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(700), UNITY_DISPLAY_STYLE_INT);



   engine.fsmState = WAITING;

   event = gsmSendCmd(&engine, "ATB\r");

   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_UKN)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(704), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmGetCmdRsp(void)

{





   gsmEngine_t engine;

   rsp_t rsp;

   VLRINGBUFF_T * buffer;







   buffer = &(engine.rspVlRb);







   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback2);







   VLRingBuffer_IsEmpty_CMockExpectAndReturn(735, buffer, 0);

   rsp = gsmGetCmdRsp(&engine);

   if ((0 == strcmp(rsp.cmd,"RESP1"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(737)));};

   if ((0 == strcmp(rsp.par,"parametros1"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(738)));};



   VLRingBuffer_IsEmpty_CMockExpectAndReturn(740, buffer, 0);

   rsp = gsmGetCmdRsp(&engine);

   if ((0 == strcmp(rsp.cmd,"RESP2"))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(742)));};

   if ((0 == strcmp(rsp.par,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(743)));};



   VLRingBuffer_IsEmpty_CMockExpectAndReturn(745, buffer, 1);

   rsp = gsmGetCmdRsp(&engine);

   if ((0 == strcmp(rsp.cmd,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(747)));};

   if ((0 == strcmp(rsp.par,""))) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(748)));};



}

void test_gsmGetNoCmdRsp (void)

{





   gsmEngine_t engine;

   VLRINGBUFF_T * buffer;

   int noRsp;







   buffer = &(engine.rspVlRb);







   VLRingBuffer_GetCount_CMockExpectAndReturn(775, buffer, 2);

   noRsp = gsmGetNoCmdRsp(&engine);

   UnityAssertEqualNumber((UNITY_INT)((2)), (UNITY_INT)((noRsp)), (((void *)0)), (UNITY_UINT)(777), UNITY_DISPLAY_STYLE_INT);



   VLRingBuffer_GetCount_CMockExpectAndReturn(779, buffer, 10);

   noRsp = gsmGetNoCmdRsp(&engine);

   UnityAssertEqualNumber((UNITY_INT)((10)), (UNITY_INT)((noRsp)), (((void *)0)), (UNITY_UINT)(781), UNITY_DISPLAY_STYLE_INT);



   VLRingBuffer_GetCount_CMockExpectAndReturn(783, buffer, 0);

   noRsp = gsmGetNoCmdRsp(&engine);

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((noRsp)), (((void *)0)), (UNITY_UINT)(785), UNITY_DISPLAY_STYLE_INT);



}
