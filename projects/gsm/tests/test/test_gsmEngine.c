/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

//-- unity: unit test framework
#include "unity.h"

//-- module being tested
#include "gsmEngine.h"

//-- mocks
#include "mock_gsmTokenizer.h"
#include "mock_gsmComms.h"
#include "mock_gsmParser.h"
#include "mock_vl_ring_buffer.h"
#include "mock_gsmCommands.h"

//-- other includes
#include "ring_buffer.h"
#include "string.h"


/*******************************************************************************
 *    DEFINITIONS
 ******************************************************************************/

#define TKN_SIZ 300

/*******************************************************************************
 *    PRIVATE TYPES
 ******************************************************************************/

typedef struct _tknTypeSize {
   uint8_t const * tkn;
   uint8_t const * cmd;
   uint8_t const * par;
   tknTypeParser_e type;
   int size;
} tknTypeSize_s;

/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/

static uint8_t auxBuffer[100];

static gsmEngine_t engine2;

static tknTypeSize_s tknTypeSizeV [] = {

   /* URC */
   {"\r\n+CMTI:abcde\r\n","CMTI","abcde",EXT_RSP,16},

   /* ooo rsp */
   {"\r\nOK\r\n","OK","",BASIC_RSP,7},

   /* ooo data block */
   {"abcde\r\n","DATA","abcde",DATA_BLOCK_P,8},

   /* AT echo + OK */
   {"AT\r","AT","",AUTOBAUD,4},
   {"\r\nOK\r\n","OK","",BASIC_RSP,7},

   /* echo error */
   {"ATI\r","I","",BASIC_CMD,5},

   /* AT echo + URC + OK */
   {"AT\r","AT","",AUTOBAUD,4},
   {"\r\n+CMTI:abcde\r\n","CMTI","abcde",EXT_RSP,16},
   {"\r\nOK\r\n","OK","",BASIC_RSP,7},

   /* ooo rsp */
   {"\r\nOK\r\n","OK","",BASIC_RSP,7},

   /* URC */
   {"\r\n+CMTI:abcde\r\n","CMTI","abcde",EXT_RSP,16},

   /* invalid */
   {"TA\r","","",INVALID,4},

   /* error */
   {"\r\nERROR\r\n","ERROR","",BASIC_RSP,10},

   /* IP response to CIFSR */
   {"\r\n192.168.0.1\r\n","192.168.0.1","",BASIC_RSP,16},

   /* rsp + OK */
   {"\r\nRSP\r\n","RSP","",BASIC_RSP,8},
   {"\r\nOK\r\n","OK","",BASIC_RSP,7},

   /* AT echo */
   {"AT\r","AT","",AUTOBAUD,4},

   /* invalid */
   {"TA\r","","",INVALID,4},

   /* rsp */
   {"\r\nRSP\r\n","RSP","",BASIC_RSP,8}

};

static tknTypeSize_s tknTypeV [] = {

   /* AT command */
   {"AT\r","AT","",AUTOBAUD,0},

   /* SMS text */
   {"texto sms","SMS_BODY","texto sms",SMS_BODY_P,0},

   /* UKN command */
   {"ATB\r","B","",BASIC_CMD,0}

};

static int bufEmptyV [] = {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0};

/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/

int VLRingBuffer_IsEmpty_Callback (VLRINGBUFF_T * vlrb, int NumCalls)
{
   if(NumCalls > (sizeof(bufEmptyV)/sizeof(int))){
      TEST_FAIL_MESSAGE("Demasiadas llamadas!");
   }

   return (bufEmptyV[NumCalls]);
}

int VLRingBuffer_Pop_Callback (VLRINGBUFF_T * vlrb, void * data, uint16_t cap,
                               int NumCalls)
{
   if(NumCalls > (sizeof(tknTypeSizeV)/sizeof(tknTypeSize_s))){
      TEST_FAIL_MESSAGE("Demasiadas llamadas!");
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
      TEST_FAIL_MESSAGE("Demasiadas llamadas!");
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
      TEST_FAIL_MESSAGE("gsmGetCmdSucRsp_Callback called incorrectly");
   }
}

tknTypeParser_e gsmParseTkn_Callback2 (uint8_t const * const tkn,
                                       uint8_t * cmd, uint8_t * par,
                                       uint16_t tknLen, int NumCalls)
{
   if(NumCalls > (sizeof(tknTypeV)/sizeof(tknTypeSize_s))){
      TEST_FAIL_MESSAGE("Demasiadas llamadas!");
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

   return UNKNOWN_CMD;
}

int gsm232UartSend_Callback (uint8_t const * const buffer, int n, int NumCalls)
{
   switch(NumCalls){

      case 0:
         if(0 != strcmp(buffer,"AT\r")){TEST_FAIL_MESSAGE("Not sending correct string through UART!");}
         else{return 3;}
         break;

      case 1:
         if(0 != strcmp(buffer,"texto sms")){TEST_FAIL_MESSAGE("Not sending correct string through UART!");}
         else{return 9;}
         break;

      case 2:
         if(0 != strcmp(buffer,"\x1A")){TEST_FAIL_MESSAGE("Not sending correct string through UART!");}
         else{return 1;}
         break;

      default:
         TEST_FAIL_MESSAGE("gsm232UartSend_Callback called to many times");
         break;
   }
}

uint32_t gsmGetCmdTimeout_Callback (uint16_t idx, int NumCalls)
{
   switch(NumCalls){

      case 0:

         if(0 != idx){TEST_FAIL_MESSAGE("Incorrect command index");}
         else{return TOUT_DEF;}
         break;

      case 1:
         if(23 != idx){TEST_FAIL_MESSAGE("Incorrect command index");}
         else{return 60000;}
         break;

      default:
         TEST_FAIL_MESSAGE("gsmGetCmdTimeout_Callback called to many times");
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
         TEST_FAIL_MESSAGE("VLRingBuffer_Pop_Callback2 called to many times");
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

/*******************************************************************************
 *    SETUP, TEARDOWN
 ******************************************************************************/

void setUp(void)
{
   return;
}

void tearDown(void)
{
   return;
}

/*******************************************************************************
 *    TESTS
 ******************************************************************************/

/* test_gsmInitEngine
 *
 * Functions tested:
 *
 * - gsmInitEngine
 *
 * */

void test_gsmInitEngine(void)
{
   /* Variables */

   gsmEngine_t engine;
   bool result;

   /* Mocks */

   gsmInitTokenizer_IgnoreAndReturn(true);
   VLRingBuffer_Init_IgnoreAndReturn(1);

   /* Test sequence */

   result = gsmInitEngine(&engine);

   TEST_ASSERT_TRUE(result);
}

/* test_gsmProcessTkn
 *
 * Functions tested:
 *
 * - gsmProcessTkn
 * - gsmUpdateFsm (all return points except 1.1)
 *
 *
 * */

void test_gsmProcessTkn(void)
{
   /* Variables */

   gsmEngine_t engine;
   fsmEvent_e event;

   /* Initialization */

   engine.fsmState = WAITING;

   /* Mocks */

   gsmTermUartSend_IgnoreAndReturn(0);
   gsmDetectTkns_Ignore();
   VLRingBuffer_IsEmpty_StubWithCallback(VLRingBuffer_IsEmpty_Callback);
   VLRingBuffer_IsFull_IgnoreAndReturn(0);
   VLRingBuffer_Insert_IgnoreAndReturn(0);
   VLRingBuffer_Flush_Ignore();
   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback);
   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback);
   gsmUrcSearch_StubWithCallback(gsmUrcSearch_Callback);
   gsmGetCmdErrRsp_StubWithCallback(gsmGetCmdErrRsp_Callback);
   gsmGetCmdSucRsp_StubWithCallback(gsmGetCmdSucRsp_Callback);

   /* Test sequence */

   /* URC (1.2) */

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_URC, event);

   /* ooo rsp (1.3) */

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_OOO, event);

   /* ooo data block (1.4) */

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_TKN_INV, event);

   /* AT sent + AT echo + OK (2.1, 3.3) */

   engine.fsmState = CMD_SENT;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, event);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_CLOSE, event);

   /* AT sent + echo error (2.2) */

   engine.fsmState = CMD_SENT;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_CMD_ECHO, event);

   /* AT sent + AT echo + URC + OK (2.1, 3.1, 3.3) */

   engine.fsmState = CMD_SENT;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, event);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_URC, event);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_CLOSE, event);

   /* AT sent + ooo rsp (2.4) */

   engine.fsmState = CMD_SENT;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_CMD_ECHO, event);

   /* AT sent + URC (2.3) */

   engine.fsmState = CMD_SENT;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_URC, event);

   /* AT Sent + timeout (2.5) */

   engine.fsmState = CMD_SENT;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   engine.toutCnt = 0; /* simulate timeout */

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_TIMEOUT, event);

   /* AT sent + invalid (2.6) */

   engine.fsmState = CMD_SENT;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_TKN_INV, event);

   /* AT sent and echoed + error (3.2) */

   engine.fsmState = CMD_ACK;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_MSG_CLOSE, event);

   /* AT+CIFSR sent and echoed + non-standard close (3.4) */

   engine.fsmState = CMD_ACK;
   strncpy(engine.currCmd, "CIFSR", strlen("CIFSR")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 19;

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_CLOSE, event);

   /* AT sent and echoed + rsp + OK (3.5, 3.3)*/

   engine.fsmState = CMD_ACK;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_RSP, event);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_CLOSE, event);

   /* AT sent and echoed + AT echo received (3.6)*/

   engine.fsmState = CMD_ACK;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_OOO, event);

   /* AT sent and echoed + timeout (3.7) */

   engine.fsmState = CMD_ACK;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   engine.toutCnt = 0; /* simulate timeout */

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_TIMEOUT, event);

   /* AT sent and echoed + invalid (3.8) */

   engine.fsmState = CMD_ACK;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_TKN_INV, event);

   /* rsp with FSM out of range (3.9) */

   engine.toutCnt = 1000; /* avoid timeout */
   engine.fsmState = CMD_ACK+1;

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_FSM_OOR, event);

}

/* test_gsmToutCntZero
 *
 * Functions tested:
 *
 * - gsmToutCntZero
 *
 *
 * */

void test_gsmToutCntZero(void)
{
   /* Variables */

   gsmEngine_t engine;
   bool result;

   /* Test sequence */

   engine.toutCnt = 1000;
   result = gsmToutCntZero(&engine);
   TEST_ASSERT_FALSE(result);

   engine.toutCnt = 0;
   result = gsmToutCntZero(&engine);
   TEST_ASSERT_TRUE(result);

}

/* test_gsmDecToutCnt
 *
 * Functions tested:
 *
 * - gsmDecToutCnt
 *
 *
 * */

void test_gsmDecToutCnt(void)
{
   /* Variables */

   gsmEngine_t engine;
   bool result;

   /* Test sequence */

   engine.toutCnt = 100;

   gsmDecToutCnt(&engine);

   TEST_ASSERT_TRUE(99 == engine.toutCnt);

   int i;

   for(i = 0; i <10; i++){
      gsmDecToutCnt(&engine);
   }

   TEST_ASSERT_TRUE(89 == engine.toutCnt);

   engine.toutCnt = 0;

   gsmDecToutCnt(&engine);

   TEST_ASSERT_TRUE(0 == engine.toutCnt);

}

/* test_gsmSendCmd
 *
 * Functions tested:
 *
 * - gsmSendCmd
 * - gsmUpdateFsm (return point 1.1)
 *
 *
 * */

void test_gsmSendCmd(void)
{
   /* Variables */

   gsmEngine_t engine;
   fsmEvent_e event;

   /* Mocks */

   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback2);
   gsmCmdSearch_StubWithCallback(gsmCmdSearch_Callback);
   gsm232UartSend_StubWithCallback(gsm232UartSend_Callback);
   gsmGetCmdTimeout_StubWithCallback(gsmGetCmdTimeout_Callback);

   /* Test sequence */

   engine.fsmState = WAITING;
   event = gsmSendCmd(&engine, "AT\r");
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, event);

   engine.fsmState = WAITING;
   event = gsmSendCmd(&engine, "texto sms");
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, event);

   engine.fsmState = WAITING;
   event = gsmSendCmd(&engine, "ATB\r");
   TEST_ASSERT_EQUAL_INT(ERR_CMD_UKN, event);

}

/* test_gsmGetCmdRsp
 *
 * Functions tested:
 *
 * - gsmGetCmdRsp
 *
 *
 * */

void test_gsmGetCmdRsp(void)
{
   /* Variables */

   gsmEngine_t engine;
   rsp_t rsp;
   VLRINGBUFF_T * buffer;

   /* Initializations */

   buffer = &(engine.rspVlRb);

   /* Mocks */

   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback2);

   /* Test sequence */

   VLRingBuffer_IsEmpty_ExpectAndReturn(buffer,0);
   rsp = gsmGetCmdRsp(&engine);
   TEST_ASSERT_TRUE(0 == strcmp(rsp.cmd,"RESP1"));
   TEST_ASSERT_TRUE(0 == strcmp(rsp.par,"parametros1"));

   VLRingBuffer_IsEmpty_ExpectAndReturn(buffer,0);
   rsp = gsmGetCmdRsp(&engine);
   TEST_ASSERT_TRUE(0 == strcmp(rsp.cmd,"RESP2"));
   TEST_ASSERT_TRUE(0 == strcmp(rsp.par,""));

   VLRingBuffer_IsEmpty_ExpectAndReturn(buffer,1);
   rsp = gsmGetCmdRsp(&engine);
   TEST_ASSERT_TRUE(0 == strcmp(rsp.cmd,""));
   TEST_ASSERT_TRUE(0 == strcmp(rsp.par,""));

}

/* test_gsmGetNoCmdRsp
 *
 * Functions tested:
 *
 * - gsmGetNoCmdRsp
 *
 *
 * */

void test_gsmGetNoCmdRsp (void)
{
   /* Variables */

   gsmEngine_t engine;
   VLRINGBUFF_T * buffer;
   int noRsp;

   /* Initializations */

   buffer = &(engine.rspVlRb);

   /* Test sequence */

   VLRingBuffer_GetCount_ExpectAndReturn(buffer,2);
   noRsp = gsmGetNoCmdRsp(&engine);
   TEST_ASSERT_EQUAL_INT(2,noRsp);

   VLRingBuffer_GetCount_ExpectAndReturn(buffer,10);
   noRsp = gsmGetNoCmdRsp(&engine);
   TEST_ASSERT_EQUAL_INT(10,noRsp);

   VLRingBuffer_GetCount_ExpectAndReturn(buffer,0);
   noRsp = gsmGetNoCmdRsp(&engine);
   TEST_ASSERT_EQUAL_INT(0,noRsp);

}

/* test_gsmGetUrc
 *
 * Functions tested:
 *
 * - gsmGetUrc
 *
 *
 * */

void test_gsmGetUrc(void)
{
   /* Variables */

   gsmEngine_t engine;
   rsp_t rsp;
   VLRINGBUFF_T * buffer;

   /* Initializations */

   buffer = &(engine.urcVlRb);

   /* Mocks */

   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback2);

   /* Test sequence */

   VLRingBuffer_IsEmpty_ExpectAndReturn(buffer,0);
   rsp = gsmGetUrc(&engine);
   TEST_ASSERT_TRUE(0 == strcmp(rsp.cmd,"RESP1"));
   TEST_ASSERT_TRUE(0 == strcmp(rsp.par,"parametros1"));

   VLRingBuffer_IsEmpty_ExpectAndReturn(buffer,0);
   rsp = gsmGetUrc(&engine);
   TEST_ASSERT_TRUE(0 == strcmp(rsp.cmd,"RESP2"));
   TEST_ASSERT_TRUE(0 == strcmp(rsp.par,""));

   VLRingBuffer_IsEmpty_ExpectAndReturn(buffer,1);
   rsp = gsmGetUrc(&engine);
   TEST_ASSERT_TRUE(0 == strcmp(rsp.cmd,""));
   TEST_ASSERT_TRUE(0 == strcmp(rsp.par,""));

}

/* test_gsmGetSerialMode
 *
 * Functions tested:
 *
 * - gsmGetSerialMode
 *
 *
 * */

void test_gsmGetSerialMode(void)
{
   /* Variables */

   gsmEngine_t engine;
   serialMode_e mode;

   /* Test sequence */

   engine.serialMode = COMMAND_MODE;
   mode = gsmGetSerialMode(&engine);
   TEST_ASSERT_EQUAL_INT(COMMAND_MODE,mode);

   engine.serialMode = DATA_MODE;
   mode = gsmGetSerialMode(&engine);
   TEST_ASSERT_EQUAL_INT(DATA_MODE,mode);

}

/* test_gsmSetSerialMode
 *
 * Functions tested:
 *
 * - gsmSetSerialMode
 *
 *
 * */

void test_gsmSetSerialMode(void)
{
   /* Variables */

   gsmEngine_t engine;
   serialMode_e mode;

   /* Test sequence */

   gsmSetSerialMode(&engine,COMMAND_MODE);
   mode = engine.serialMode;
   TEST_ASSERT_EQUAL_INT(COMMAND_MODE,mode);

   gsmSetSerialMode(&engine,DATA_MODE);
   mode = engine.serialMode;
   TEST_ASSERT_EQUAL_INT(DATA_MODE,mode);

}

/* test_gsmRecordUrc
 *
 * Functions tested:
 *
 * - gsmRecordUrc, through the gsmProcessTkn interface
 *
 * */

void test_gsmRecordUrc(void)
{
   /* Variables */

   uint8_t result;

   /* Initialization */

   engine2.fsmState = WAITING;

   /* Mocks -Ignore- */

   gsmTermUartSend_IgnoreAndReturn(0);
   gsmDetectTkns_Ignore();
   VLRingBuffer_Pop_IgnoreAndReturn(0);
   gsmUrcSearch_IgnoreAndReturn(1);

   /* Mocks -Callback- */

   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback3);
   VLRingBuffer_Insert_StubWithCallback(VLRingBuffer_Insert_Callback);

   /* Test sequence */

   VLRingBuffer_IsEmpty_ExpectAndReturn(&(engine2.tknVlRb),0);
   VLRingBuffer_IsFull_ExpectAndReturn(&(engine2.urcVlRb),0);
   result = gsmProcessTkn(&engine2);

   TEST_ASSERT_EQUAL_STRING("CMTI.abcde",auxBuffer);

}





