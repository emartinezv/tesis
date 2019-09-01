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







   gsmInitTokenizer_CMockIgnoreAndReturn(334, 1);

   VLRingBuffer_Init_CMockIgnoreAndReturn(335, 1);







   result = gsmInitEngine(&engine);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(341)));};

}

void test_gsmProcessTkn(void)

{





   gsmEngine_t engine;

   fsmEvent_e event;







   engine.fsmState = WAITING;







   gsmTermUartSend_CMockIgnoreAndReturn(367, 0);

   gsmDetectTkns_CMockIgnore();

   VLRingBuffer_IsEmpty_StubWithCallback(VLRingBuffer_IsEmpty_Callback);

   VLRingBuffer_IsFull_CMockIgnoreAndReturn(370, 0);

   VLRingBuffer_Insert_CMockIgnoreAndReturn(371, 0);

   VLRingBuffer_Flush_CMockIgnore();

   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback);

   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback);

   gsmUrcSearch_StubWithCallback(gsmUrcSearch_Callback);

   gsmGetCmdErrRsp_StubWithCallback(gsmGetCmdErrRsp_Callback);

   gsmGetCmdSucRsp_StubWithCallback(gsmGetCmdSucRsp_Callback);











   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(385), UNITY_DISPLAY_STYLE_INT);







   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(391), UNITY_DISPLAY_STYLE_INT);







   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(397), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(408), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(412), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(423), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(434), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(438), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(442), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(453), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(464), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   engine.toutCnt = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TIMEOUT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(477), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(488), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_MSG_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(499), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "CIFSR", strlen("CIFSR")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 19;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(510), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_RSP)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(521), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(525), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(536), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   engine.toutCnt = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TIMEOUT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(549), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(560), UNITY_DISPLAY_STYLE_INT);







   engine.toutCnt = 1000;

   engine.fsmState = CMD_ACK+1;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_FSM_OOR)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(569), UNITY_DISPLAY_STYLE_INT);



}

void test_gsmToutCntZero(void)

{





   gsmEngine_t engine;

   _Bool result;







   engine.toutCnt = 1000;

   result = gsmToutCntZero(&engine);

   if (!(result)) {} else {UnityFail( ((" Expected FALSE Was TRUE")), (UNITY_UINT)((UNITY_UINT)(593)));};



   engine.toutCnt = 0;

   result = gsmToutCntZero(&engine);

   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(597)));};



}

void test_gsmDecToutCnt(void)

{





   gsmEngine_t engine;

   _Bool result;







   engine.toutCnt = 100;



   gsmDecToutCnt(&engine);



   if ((99 == engine.toutCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(623)));};



   int i;



   for(i = 0; i <10; i++){

      gsmDecToutCnt(&engine);

   }



   if ((89 == engine.toutCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(631)));};



   engine.toutCnt = 0;



   gsmDecToutCnt(&engine);



   if ((0 == engine.toutCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(637)));};



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

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(669), UNITY_DISPLAY_STYLE_INT);



   engine.fsmState = WAITING;

   event = gsmSendCmd(&engine, "texto sms");

   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_SENT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(673), UNITY_DISPLAY_STYLE_INT);



   engine.fsmState = WAITING;

   event = gsmSendCmd(&engine, "ATB\r");

   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_UKN)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(677), UNITY_DISPLAY_STYLE_INT);



}
