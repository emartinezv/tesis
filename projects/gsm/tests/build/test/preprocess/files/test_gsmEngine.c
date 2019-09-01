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

   {"\r\nOK\r\n","OK","",BASIC_RSP,7}

};











int VLRingBuffer_Pop_Callback (VLRINGBUFF_T * vlrb, void * data, uint16_t cap,

                               int NumCalls)

{

   if(NumCalls > (sizeof(tknTypeSizeV)/sizeof(tknTypeSize_s))){

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(69));

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

      UnityFail( (("Demasiadas llamadas!")), (UNITY_UINT)(83));

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







   gsmInitTokenizer_CMockIgnoreAndReturn(139, 1);

   VLRingBuffer_Init_CMockIgnoreAndReturn(140, 1);







   result = gsmInitEngine(&engine);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(146)));};

}

void test_gsmProcessTkn(void)

{





   gsmEngine_t engine;

   fsmEvent_e event;







   engine.fsmState = WAITING;







   gsmTermUartSend_CMockIgnoreAndReturn(172, 0);

   gsmDetectTkns_CMockIgnore();

   VLRingBuffer_IsEmpty_CMockIgnoreAndReturn(174, 0);

   VLRingBuffer_IsFull_CMockIgnoreAndReturn(175, 0);

   VLRingBuffer_Insert_CMockIgnoreAndReturn(176, 0);

   VLRingBuffer_Flush_CMockIgnore();

   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback);

   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback);











   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_URC)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(187), UNITY_DISPLAY_STYLE_INT);







   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_OOO)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(193), UNITY_DISPLAY_STYLE_INT);







   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((ERR_TKN_INV)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(199), UNITY_DISPLAY_STYLE_INT);







   engine.fsmState = CMD_SENT;

   strncpy(engine.currCmd, "AT", strlen("AT")+1);

   engine.currPar[0]='\0';

   engine.currIdx = 0;



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CMD_ACK)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(210), UNITY_DISPLAY_STYLE_INT);



   event = gsmProcessTkn(&engine);



   UnityAssertEqualNumber((UNITY_INT)((OK_CLOSE)), (UNITY_INT)((event)), (((void *)0)), (UNITY_UINT)(214), UNITY_DISPLAY_STYLE_INT);

}
