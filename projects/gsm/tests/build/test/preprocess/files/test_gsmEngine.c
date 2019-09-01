#include "build/temp/_test_gsmEngine.c"
#include "ring_buffer.h"
#include "gsmCommands.h"
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



static int bufEmptyV [] = {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0};











int VLRingBuffer_IsEmpty_Callback (VLRINGBUFF_T * vlrb, int NumCalls)

{

   if(NumCalls > (sizeof(bufEmptyV)/sizeof(int))){

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(107));

   }



   return (bufEmptyV[NumCalls]);

}



int VLRingBuffer_Pop_Callback (VLRINGBUFF_T * vlrb, void * data, uint16_t cap,

                               int NumCalls)

{

   if(NumCalls > (sizeof(tknTypeSizeV)/sizeof(tknTypeSize_s))){

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(117));

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

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(131));

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







   gsmInitTokenizer_CMockIgnoreAndReturn(187, 1);

   VLRingBuffer_Init_CMockIgnoreAndReturn(188, 1);







   result = gsmInitEngine(&engine);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(194)));};

}

void test_gsmProcessTkn(void)

{





   gsmEngine_t engine;

   fsmEvent_e event;







   engine.fsmState = WAITING;







   gsmTermUartSend_CMockIgnoreAndReturn(220, 0);

   gsmDetectTkns_CMockIgnore();

   VLRingBuffer_IsEmpty_StubWithCallback(VLRingBuffer_IsEmpty_Callback);

   VLRingBuffer_IsFull_CMockIgnoreAndReturn(223, 0);

   VLRingBuffer_Insert_CMockIgnoreAndReturn(224, 0);

   VLRingBuffer_Flush_CMockIgnore();

   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback);

   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback);











   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(235), UNITY_DISPLAY_STYLE_INT);







   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(241), UNITY_DISPLAY_STYLE_INT);







   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(247), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(258), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(262), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(273), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(284), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(288), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(292), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_CMD_ECHO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(303), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(314), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   engine.toutCnt = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TIMEOUT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(327), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(338), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_MSG_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(349), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "CIFSR", strlen("CIFSR")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 19;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(360), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_RSP)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(371), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(375), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(386), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   engine.toutCnt = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TIMEOUT)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(399), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_ACK;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(410), UNITY_DISPLAY_STYLE_INT);







   engine.toutCnt = 1000;

   engine.fsmState = CMD_ACK+1;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_FSM_OOR)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(419), UNITY_DISPLAY_STYLE_INT);



}
