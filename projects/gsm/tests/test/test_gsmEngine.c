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

/******************************************************************************
 *    DEFINITIONS
 *****************************************************************************/

/******************************************************************************
 *    PRIVATE TYPES
 *****************************************************************************/

/* Type to determine which buffer will be checked for as input in the
   VLRingBuffer_Pop mock callback function */

typedef enum _vlBufType {
   TKN,
   RSPB,
   URC
} vlBufType_t ;

/******************************************************************************
 *    PRIVATE DATA
 *****************************************************************************/

/* Global engine variable for all tests */

gsmEngine_t engine;

vlBufType_t bufSel;
bool vlIsFull = false;

/* Gloval variables used for test_gsmProcessTkn */

uint16_t tknzerNoCh = 0;

uint8_t tknTest[280+20+1];
uint8_t cmdTest[20+1];
uint8_t parTest[280+1];

uint16_t tknSizeTest;
tknTypeParser_t tknTypeTest = AUTOBAUD;

int vlrb_empty = 0;

/* Global variables used for test_gsmRecordUrc */

auxBuffer[280+20+1];

/******************************************************************************
 *    PRIVATE FUNCTIONS - HELPERS
 *****************************************************************************/

/* Loads tknTest, cmdTest, parTest and tknTypeTest to aid the gsmParseTkn and
   VLRingBuffer_Pop mock callback functions */

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
/******************************************************************************
 *    PRIVATE FUNCTIONS - CALLBACKS
 *****************************************************************************/

/* All callbacks try to check the input variables (unless they are private
   variables from the calling function) and mimic the effect of the original
   function up to the needs of the test. */

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
   TEST_ASSERT_EQUAL_INT(tknzerNoCh, n);

   return tknSizeTest;
}

void gsmDetectTkns_Callback (VLRINGBUFF_T * const tknVlRb, uint16_t nch,
                             uint8_t const * const buffer, int NumCalls)
{
   TEST_ASSERT_TRUE(tknVlRb == &(engine.tknVlRb));
   TEST_ASSERT_EQUAL_INT(tknSizeTest, nch);
}

int VLRingBuffer_IsEmpty_Callback (VLRINGBUFF_T * vlrb, int NumCalls)
{
   TEST_ASSERT_TRUE(&(engine.tknVlRb) == vlrb)

   return vlrb_empty;
}

int VLRingBuffer_Pop_Callback (VLRINGBUFF_T * vlrb, void * data, uint16_t cap,
                               int NumCalls)
{
   /* Check input data buffer depending on the global bufSel variable */

   switch(bufSel){

      case TKN:

         TEST_ASSERT_TRUE(vlrb == &(engine.tknVlRb));

         break;

      case RSPB:

         TEST_ASSERT_TRUE(vlrb == &(engine.rspVlRb));

         break;

      case URC:

         TEST_ASSERT_TRUE(vlrb == &(engine.urcVlRb));

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

bool gsmUrcSearch_Callback (uint8_t const * const urc, int NumCalls)
{
   if(0 == strncmp(urc,"CMTI", strlen("CMTI"))){return true;}

   return false;
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

/**/

uint16_t gsmCmdSearch_Callback(uint8_t const * const cmd, int NumCalls)
{
   if(0 == strcmp(cmd,"AT")){return 0;}
   else if(0 == strcmp(cmd,"SMS_BODY")){return 23;}

   return UNKNOWN_CMD;
}

/* Both gsm232UartSend_Callback and gsmGetCmdTimeout are not very flexible
   callbacks, but the way in which they are called forces us to use a more
   static structure */

int gsm232UartSend_Callback (uint8_t const * const buffer, int n, int NumCalls)
{
   switch(NumCalls){

      case 0:
         if(0 != strcmp(buffer,"AT\r")){
            TEST_FAIL_MESSAGE("Not sending correct string through UART!");
         }
         else{return 3;}
         break;

      case 1:
         if(0 != strcmp(buffer,"texto sms")){
            TEST_FAIL_MESSAGE("Not sending correct string through UART!");
         }
         else{return 9;}
         break;

      case 2:
         if(0 != strcmp(buffer,"\x1A")){
            TEST_FAIL_MESSAGE("Not sending correct string through UART!");
         }
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
 * - gsmRecordUrc
 * - gsmRecordRsp
 * - gsmCompCloRsp
 *
 * */

void test_gsmProcessTkn(void)
{
   /* Variables */

   fsmEvent_t event;

   /* Initialization */

   engine.fsmState = WAITING;
   bufSel = TKN;

   /* Mocks */

   gsmTermUartSend_IgnoreAndReturn(0);
   VLRingBuffer_Insert_IgnoreAndReturn(0);
   VLRingBuffer_Flush_Ignore();

   //gsm232UartSend_Ignore();

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

   /* Test sequence */

   /* We created helper function loadProcessTkn to load the corresponding
      constants to feed all the callback mocks easily */

   /* URC (1.2) */

   loadProcessTkn("\r\n+CMTI:abcde\r\n", "CMTI", "abcde", EXT_RSP);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_URC, event);

   /* URC (1.2) with full URC VLRB */

   loadProcessTkn("\r\n+CMTI:abcde\r\n", "CMTI", "abcde", EXT_RSP);

   vlIsFull = true;
   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_URC, event);
   vlIsFull = false;

   /* ooo rsp (1.3) */

   loadProcessTkn("\r\n+OK\r\n", "OK", "", BASIC_RSP);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_OOO, event);

   /* ooo data block (1.4) */

   loadProcessTkn("abcde\r\n", "DATA", "abcde", DATA_BLOCK_P);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_TKN_INV, event);

   /* AT sent + AT echo + OK (2.1, 3.3) */

   engine.fsmState = CMD_SENT;
   strncpy(engine.currCmd, "AT", strlen("AT"));
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   loadProcessTkn("AT\r", "AT", "", AUTOBAUD);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, event);

   loadProcessTkn("\r\nOK\r\n", "OK", "", BASIC_RSP);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_CLOSE, event);

   /* AT sent + echo error (2.2) */

   engine.fsmState = CMD_SENT;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   loadProcessTkn("ATI\r", "I", "", AUTOBAUD);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_CMD_ECHO, event);

   /* AT sent + AT echo + URC + OK (2.1, 3.1, 3.3) */

   engine.fsmState = CMD_SENT;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   loadProcessTkn("AT\r", "AT", "", AUTOBAUD);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, event);

   loadProcessTkn("\r\n+CMTI:abcde\r\n", "CMTI", "abcde", EXT_RSP);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_URC, event);

   loadProcessTkn("\r\nOK\r\n", "OK", "", BASIC_RSP);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_CLOSE, event);

   /* AT sent + ooo rsp (2.4) */

   engine.fsmState = CMD_SENT;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   loadProcessTkn("\r\nOK\r\n", "OK", "", BASIC_RSP);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_CMD_ECHO, event);

   /* AT sent + URC (2.3) */

   engine.fsmState = CMD_SENT;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   loadProcessTkn("\r\n+CMTI:abcde\r\n", "CMTI", "abcde", EXT_RSP);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_URC, event);

   /* AT Sent + timeout (2.5) */

   engine.fsmState = CMD_SENT;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   engine.toutCnt = 0; /* simulate timeout */

   loadProcessTkn("\r\n+CMTI:abcde\r\n", "CMTI", "abcde", EXT_RSP);

   vlrb_empty = 1; /* no tokens read */

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_TIMEOUT, event);

   vlrb_empty = 0;      /* reset to normal state */
   engine.toutCnt = 1;  /* reset to normal state */

   /* AT sent + invalid (2.6) */

   engine.fsmState = CMD_SENT;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   loadProcessTkn("TA\r", "", "", INVALID);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_TKN_INV, event);

   /* AT sent and echoed + error (3.2) */

   engine.fsmState = CMD_ACK;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   loadProcessTkn("\r\nERROR\r\n", "ERROR", "", BASIC_RSP);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_MSG_CLOSE, event);

   /* AT+CIFSR sent and echoed + non-standard close (3.4) */

   engine.fsmState = CMD_ACK;
   strncpy(engine.currCmd, "CIFSR", strlen("CIFSR")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 19;

   loadProcessTkn("\r\nE192.168.0.1\r\n", "192.168.0.1", "", BASIC_RSP);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_CLOSE, event);

   /* AT sent and echoed + rsp + OK (3.5, 3.3)*/

   engine.fsmState = CMD_ACK;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   loadProcessTkn("\r\nresponse\r\n", "response", "", BASIC_RSP);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_RSP, event);

   loadProcessTkn("\r\nOK\r\n", "OK", "", BASIC_RSP);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_CLOSE, event);

   /* AT sent and echoed + rsp + OK (3.5, 3.3) with full rsp VLRB*/

   engine.fsmState = CMD_ACK;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   loadProcessTkn("\r\nresponse\r\n", "response", "", BASIC_RSP);

   vlIsFull = true;
   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_RSP, event);
   vlIsFull = false;

   loadProcessTkn("\r\nOK\r\n", "OK", "", BASIC_RSP);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_CLOSE, event);

   /* AT sent and echoed + AT echo received (3.6)*/

   engine.fsmState = CMD_ACK;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   loadProcessTkn("AT\r", "AT", "", AUTOBAUD);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_OOO, event);

   /* AT sent and echoed + timeout (3.7) */

   engine.fsmState = CMD_ACK;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   engine.toutCnt = 0; /* simulate timeout */

   vlrb_empty = 1; /* no tokens read */

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_TIMEOUT, event);

   vlrb_empty = 0;      /* reset to normal state */
   engine.toutCnt = 1;  /* reset to normal state */

   /* AT sent and echoed + invalid (3.8) */

   engine.fsmState = CMD_ACK;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   loadProcessTkn("TA\r", "", "", INVALID);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_TKN_INV, event);

   /* rsp with FSM out of range (3.9) */

   engine.fsmState = CMD_ACK+1;

   loadProcessTkn("\r\nresponse\r\n", "response", "", BASIC_RSP);

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

   bool result;

   /* Test sequence */

   /* toutCnt not zero */

   engine.toutCnt = 1000;
   result = gsmToutCntZero(&engine);
   TEST_ASSERT_FALSE(result);

   /* toutCnt zero */

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

   bool result;

   /* Test sequence */

   /* Decrement once */

   engine.toutCnt = 100;

   gsmDecToutCnt(&engine);

   TEST_ASSERT_TRUE(99 == engine.toutCnt);

   /* Decrement 10 more times */

   int i;

   for(i = 0; i <10; i++){
      gsmDecToutCnt(&engine);
   }

   TEST_ASSERT_TRUE(89 == engine.toutCnt);

   /* Already 0, should stay there and not cause any errors */

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

   fsmEvent_t event;

   /* Mocks */

   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback);
   gsmCmdSearch_StubWithCallback(gsmCmdSearch_Callback);
   gsm232UartSend_StubWithCallback(gsm232UartSend_Callback);
   gsmGetCmdTimeout_StubWithCallback(gsmGetCmdTimeout_Callback);

   /* Test sequence */

   /* We also use loadProcessTkn here to load the cmd, par and tknType info
      for the gsmParseTkn mock callback */

   /* Send cmd */

   engine.fsmState = WAITING;

   loadProcessTkn("AT\r", "AT", "", AUTOBAUD);

   event = gsmSendCmd(&engine, "AT\r");

   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, event);

   /* Send sms text body */

   engine.fsmState = WAITING;

   loadProcessTkn("texto sms", "SMS_BODY", "texto sms", SMS_BODY_P);

   event = gsmSendCmd(&engine, "texto sms");

   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, event);

   /* Send unrecognized cmd */

   engine.fsmState = WAITING;

   loadProcessTkn("ATB\r", "B", "", BASIC_CMD);

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

   rsp_t rsp;

   /* Initializations */

   bufSel = RSPB;

   /* Mocks */

   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback);

   /* Test sequence */

   /* We also use loadProcessTkn here to load tknTest and tknSizeTest for the
      VLRingBuffer_Pop mock callback */

   /* Get rsp with parameters */

   VLRingBuffer_IsEmpty_ExpectAndReturn(&(engine.rspVlRb),0);
   loadProcessTkn("RESP1.parametros1", "RESP1", "parametros1", BASIC_RSP);

   rsp = gsmGetCmdRsp(&engine);

   TEST_ASSERT_TRUE(0 == strcmp(rsp.cmd,"RESP1"));
   TEST_ASSERT_TRUE(0 == strcmp(rsp.par,"parametros1"));

   /* Get rsp without parameters */

   VLRingBuffer_IsEmpty_ExpectAndReturn(&(engine.rspVlRb),0);
   loadProcessTkn("RESP2.", "RESP2", "", BASIC_RSP);

   rsp = gsmGetCmdRsp(&engine);

   TEST_ASSERT_TRUE(0 == strcmp(rsp.cmd,"RESP2"));
   TEST_ASSERT_TRUE(0 == strcmp(rsp.par,""));

   /* No response in buffer */

   VLRingBuffer_IsEmpty_ExpectAndReturn(&(engine.rspVlRb),1);

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

   int noRsp;

   /* Test sequence */

   /* We test different number of responses, including none */

   VLRingBuffer_GetCount_ExpectAndReturn(&(engine.rspVlRb),2);

   noRsp = gsmGetNoCmdRsp(&engine);

   TEST_ASSERT_EQUAL_INT(2,noRsp);

   VLRingBuffer_GetCount_ExpectAndReturn(&(engine.rspVlRb),10);

   noRsp = gsmGetNoCmdRsp(&engine);

   TEST_ASSERT_EQUAL_INT(10,noRsp);

   VLRingBuffer_GetCount_ExpectAndReturn(&(engine.rspVlRb),0);

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

   rsp_t rsp;

   /* Initializations */

   bufSel = URC;

   /* Mocks */

   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback);

   /* Test sequence */

   /* We also use loadProcessTkn here to load tknTest and tknSizeTest for the
      VLRingBuffer_Pop mock callback */

   /* urc with parameters */

   VLRingBuffer_IsEmpty_ExpectAndReturn(&(engine.urcVlRb),0);

   loadProcessTkn("RESP1.parametros1", "RESP1", "parametros1", BASIC_RSP);

   rsp = gsmGetUrc(&engine);

   TEST_ASSERT_TRUE(0 == strcmp(rsp.cmd,"RESP1"));
   TEST_ASSERT_TRUE(0 == strcmp(rsp.par,"parametros1"));

   /* urc without parameters */

   VLRingBuffer_IsEmpty_ExpectAndReturn(&(engine.urcVlRb),0);

   loadProcessTkn("RESP2.", "RESP2", "", BASIC_RSP);

   rsp = gsmGetUrc(&engine);

   TEST_ASSERT_TRUE(0 == strcmp(rsp.cmd,"RESP2"));
   TEST_ASSERT_TRUE(0 == strcmp(rsp.par,""));

   /* no urcs */

   VLRingBuffer_IsEmpty_ExpectAndReturn(&(engine.urcVlRb),1);

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

   serialMode_t mode;

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

   serialMode_t mode;

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

   engine.fsmState = WAITING;
   bufSel = TKN;

   /* Mocks -Ignore- */

   gsmTermUartSend_IgnoreAndReturn(0);

   /* Mocks -Callback- */

   gsmNoChTokenizer_StubWithCallback(gsmNoChTokenizer_Callback);
   gsm232UartRecv_StubWithCallback(gsm232UartRecv_Callback);
   gsmDetectTkns_StubWithCallback(gsmDetectTkns_Callback);
   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback);
   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback);
   gsmUrcSearch_StubWithCallback(gsmUrcSearch_Callback);
   VLRingBuffer_Insert_StubWithCallback(VLRingBuffer_Insert_Callback);

   /* Test sequence */

   VLRingBuffer_IsEmpty_ExpectAndReturn(&(engine.tknVlRb),0);
   VLRingBuffer_IsFull_ExpectAndReturn(&(engine.urcVlRb),0);

   loadProcessTkn("\r\n+CMTI:abcde\r\n", "CMTI", "abcde", EXT_RSP);

   result = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_STRING("CMTI.abcde",auxBuffer);

}





