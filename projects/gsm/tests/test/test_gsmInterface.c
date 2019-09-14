/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

//-- unity: unit test framework
#include "unity.h"

//-- module being tested
#include "gsmInterface.h"

//-- mocks
#include "mock_gsmTokenizer.h"
#include "mock_gsmComms.h"
#include "mock_gsmParser.h"
#include "mock_gsmCommands.h"
#include "mock_gsmEngine.h"
#include "mock_vl_ring_buffer.h"

//-- other includes
#include "ring_buffer.h"
#include "string.h"

/*******************************************************************************
 *    DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 *    PRIVATE TYPES
 ******************************************************************************/

/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/

static bool dataCbackFlag = false;
static bool frmFlag = false;
static bool frmCbackFlag = false;

static uint8_t const * const exitCmds [] = {"OK", "CLOSED"};

/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/

void urcCbackTest (uint8_t const * const cmd, uint8_t const * const par)
{
   TEST_ASSERT_EQUAL_STRING("CMTI",cmd);
   TEST_ASSERT_EQUAL_STRING("abcde",par);

   return;
}

void dataCbackTest (gsmInterface_t * interface)
{
   dataCbackFlag = true;

   return;
}

void * frmCbackTest (errorUser_s error, void * output)
{
   frmCbackFlag = true;
}

void frmTest (gsmInterface_t * interface)
{
   frmFlag = true;

   return;
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

/* test_gsmInitInterface
 *
 * Functions tested:
 *
 * - gsmInitInterface
 *
 * */

void test_gsmInitInterface(void)
{
   /* Variables */

   gsmInterface_t interface;
   bool result;

   /* Mocks */

   gsmInitEngine_ExpectAndReturn(&(interface.engine), TRUE);

   /* Test sequence */

   result = gsmInitInterface(&interface);

   TEST_ASSERT_TRUE(result);

   TEST_ASSERT_TRUE(IDLE == interface.frmState);
   TEST_ASSERT_TRUE(NOCMD == interface.procState);

   TEST_ASSERT_TRUE(DELAY_PROC == interface.procCnt);
   TEST_ASSERT_TRUE(0 == interface.auxCnt);

   TEST_ASSERT_TRUE(MANUAL_MODE == interface.urcMode);
   TEST_ASSERT_TRUE(NULL == interface.urcCback);

   TEST_ASSERT_TRUE(NULL == interface.dataCback);
   TEST_ASSERT_EQUAL_STRING("OK", interface.exitCmdList[0]);
   TEST_ASSERT_EQUAL_STRING("CLOSED", interface.exitCmdList[1]);

   TEST_ASSERT_TRUE(NULL == interface.frm);
   TEST_ASSERT_TRUE(NULL == interface.frmInput);
   TEST_ASSERT_TRUE(NULL == interface.frmOutput);
   TEST_ASSERT_TRUE(NULL == interface.frmCback);
   TEST_ASSERT_TRUE(OK == interface.errorOut.errorFrm);

}

/* test_gsmStartUp
 *
 * Functions tested:
 *
 * - gsmStartUp
 *
 * */

void test_gsmStartUp(void)
{
   /* Variables */

   gsmInterface_t interface;
   frmCback_t cbackTest;

   /* Test sequence */

   gsmStartUp(&interface, cbackTest);

   TEST_ASSERT_TRUE(cbackTest == interface.frmCback);
   TEST_ASSERT_TRUE(INIT == interface.frmState);

}

/* test_gsmExitDataMode
 *
 * Functions tested:
 *
 * - gsmExitDataMode
 *
 * */

void test_gsmExitDataMode(void)
{
   /* Variables */

   gsmInterface_t interface;
   frmCback_t cbackTest;

   /* Test sequence */

   gsmExitDataMode(&interface, cbackTest);

   TEST_ASSERT_TRUE(cbackTest == interface.frmCback);
   TEST_ASSERT_TRUE(INIT == interface.frmState);

}

/* test_gsmSysTickHandler
 *
 * Functions tested:
 *
 * - gsmSysTickHandler
 *
 * */

void test_gsmSysTickHandler(void)
{
   /* Variables */

   gsmInterface_t interface;

   /* Mocks */

   gsmInitEngine_ExpectAndReturn(&(interface.engine), TRUE);
   gsmDecToutCnt_Expect(&(interface.engine));
   gsmDecToutCnt_Expect(&(interface.engine));

   /* Test sequence */

   gsmInitInterface(&interface);

   interface.procCnt = 100;
   interface.auxCnt = 100;

   gsmSysTickHandler();

   TEST_ASSERT_EQUAL_UINT32(99, interface.procCnt);
   TEST_ASSERT_EQUAL_UINT32(99, interface.auxCnt);

   interface.procCnt = 0;
   interface.auxCnt = 0;

   gsmSysTickHandler();

   TEST_ASSERT_EQUAL_UINT32(0, interface.procCnt);
   TEST_ASSERT_EQUAL_UINT32(0, interface.auxCnt);

}

/* test_gsmProcess
 *
 * Functions tested:
 *
 * - gsmProcess
 *
 * */

void test_gsmProcess(void)
{
   /* Variables */

   gsmInterface_t interface;
   rsp_t rspTest;

   /* Initialization */

   interface.frmState = IDLE;
   interface.engine.serialMode = COMMAND_MODE;
   interface.urcMode = CBACK_MODE;
   interface.urcCback = urcCbackTest;
   interface.dataCback = dataCbackTest;
   interface.frm = frmTest;

   strncpy(rspTest.cmd, "CMTI", strlen("CMTI"));
   rspTest.cmd[strlen("CMTI")] = '\0';
   strncpy(rspTest.par, "abcde", strlen("abcde"));
   rspTest.par[strlen("abcde")] = '\0';

   /* Mocks */

   /* Test sequence */

   /* Branch 2 */

   interface.procCnt = 100;

   gsmProcess(&interface);

   /* Branch 1.1.1.2 */

   interface.procCnt = 0;

   gsmGetSerialMode_ExpectAndReturn(&(interface.engine), COMMAND_MODE);
   gsmProcessTkn_ExpectAndReturn(&(interface.engine), NO_UPDATE);
   gsmGetUrc_ExpectAndReturn(&(interface.engine), rspTest);

   gsmProcess(&interface);

   TEST_ASSERT_EQUAL_UINT32(DELAY_PROC, interface.procCnt);

   /* Branch 1.1.2 */

   interface.procCnt = 0;

   gsmGetSerialMode_ExpectAndReturn(&(interface.engine), DATA_MODE);
   gsmGetSerialMode_ExpectAndReturn(&(interface.engine), DATA_MODE);

   gsmProcess(&interface);

   TEST_ASSERT_TRUE(dataCbackFlag);
   TEST_ASSERT_EQUAL_UINT32(DELAY_PROC, interface.procCnt);

   return;

   /* Branch 1.2 */

   interface.procCnt = 0;
   interface.frmState = INIT;

   gsmProcess(&interface);

   TEST_ASSERT_TRUE(frmFlag);
   TEST_ASSERT_EQUAL_UINT32(DELAY_PROC, interface.procCnt);

   return;

}

/* test_gsmIsIdle
 *
 * Functions tested:
 *
 * - gsmIsIdle
 *
 * */

void test_gsmIsIdle(void)
{
   /* Variables */

   gsmInterface_t interface;
   bool result;

   /* Tests */

   interface.frmState = IDLE;

   result = gsmIsIdle(&interface);

   TEST_ASSERT_TRUE(result);

   interface.frmState = INIT;

   result = gsmIsIdle(&interface);

   TEST_ASSERT_FALSE(result);

}

/* test_gsmGetSigQual
 *
 * Functions tested:
 *
 * - gsmGetSigQual
 *
 * */

void test_gsmGetSigQual(void)
{
   /* Variables */

   gsmInterface_t interface;
   frmCback_t cbackTest;
   sigQual_s sigQualTest;

   /* Test sequence */

   gsmGetSigQual(&interface, &sigQualTest, cbackTest);

   TEST_ASSERT_TRUE(&sigQualTest == interface.frmOutput);
   TEST_ASSERT_TRUE(cbackTest == interface.frmCback);
   TEST_ASSERT_TRUE(INIT == interface.frmState);

}

/* test_gsmCheckConn
 *
 * Functions tested:
 *
 * - gsmCheckConn
 *
 * */

void test_gsmCheckConn(void)
{
   /* Variables */

   gsmInterface_t interface;
   frmCback_t cbackTest;
   connStatus_s connStateTest;

   /* Test sequence */

   gsmCheckConn(&interface, &connStateTest, cbackTest);

   TEST_ASSERT_TRUE(&connStateTest == interface.frmOutput);
   TEST_ASSERT_TRUE(cbackTest == interface.frmCback);
   TEST_ASSERT_TRUE(INIT == interface.frmState);

}

/* test_gsmReadUrc
 *
 * Functions tested:
 *
 * - gsmReadUrc
 *
 * */

void test_gsmReadUrc(void)
{
   /* Variables */

   gsmInterface_t interface;
   urc_s urcTest = {"\0","\0"};
   bool result;

   rsp_t rspTest1 = {"\0","\0"};
   rsp_t rspTest2 = {"command",""};
   rsp_t rspTest3 = {"command","parameters"};

   /* Test sequence */

   /* Test for no URCs in store */

   gsmGetUrc_ExpectAndReturn(&(interface.engine), rspTest1);

   result = gsmReadUrc(&interface, &urcTest);

   TEST_ASSERT_FALSE(result);
   TEST_ASSERT_EQUAL_STRING("\0", urcTest.cmd);
   TEST_ASSERT_EQUAL_STRING("\0", urcTest.par);

   /* Test for URC with cmd and no pars*/

   gsmGetUrc_ExpectAndReturn(&(interface.engine), rspTest2);

   result = gsmReadUrc(&interface, &urcTest);

   TEST_ASSERT_TRUE(result);
   TEST_ASSERT_EQUAL_STRING("command", urcTest.cmd);
   TEST_ASSERT_EQUAL_STRING("\0", urcTest.par);

   /* Test for IRC with cmd and pars*/

   gsmGetUrc_ExpectAndReturn(&(interface.engine), rspTest3);

   result = gsmReadUrc(&interface, &urcTest);

   TEST_ASSERT_TRUE(result);
   TEST_ASSERT_EQUAL_STRING("command", urcTest.cmd);
   TEST_ASSERT_EQUAL_STRING("parameters", urcTest.par);

}

/* test_gsmSetUrcMode
 *
 * Functions tested:
 *
 * - gsmSetUrcMode
 *
 * */

void test_gsmSetUrcMode(void)
{
   /* Variables */

   gsmInterface_t interface;

   /* Test */

   gsmSetUrcMode(&interface, MANUAL_MODE);

   TEST_ASSERT_TRUE(MANUAL_MODE == interface.urcMode);

   gsmSetUrcMode(&interface, CBACK_MODE);

   TEST_ASSERT_TRUE(CBACK_MODE == interface.urcMode);

}

/* test_gsmSetUrcCback
 *
 * Functions tested:
 *
 * - gsmSetUrcCback
 *
 * */

void test_gsmSetUrcCback(void)
{
   /* Variables */

   gsmInterface_t interface;

   /* Test */

   gsmSetUrcCback(&interface, urcCbackTest);

   TEST_ASSERT_TRUE(urcCbackTest == interface.urcCback);

}

/* test_gsmSetDataCback
 *
 * Functions tested:
 *
 * - gsmSetDataCback
 *
 * */

void test_gsmSetDataCback(void)
{
   /* Variables */

   gsmInterface_t interface;

   /* Test */

   gsmSetDataCback(&interface, dataCbackTest);

   TEST_ASSERT_TRUE(dataCbackTest == interface.dataCback);

}

/* test_gsmCheckDataMode
 *
 * Functions tested:
 *
 * - gsmCheckDataMode
 *
 * */

void test_gsmCheckDataMode(void)
{
   /* Variables */

   gsmInterface_t interface;
   uint8_t nchTest;
   uint8_t nchInput;

   uint8_t const * const testBuf1 = "blablabla123123123";
   uint8_t const * const testBuf2 = "blablabla\r\nOK\r\n123123123";
   uint8_t const * const testBuf3 = "bla\r\nCLOSED\r\n123123123";

   /* Initialization */

   interface.exitCmdList = exitCmds;

   /* Test */

   /* No detected exit cmds */

   nchInput = strlen(testBuf1);

   nchTest = gsmCheckDataMode(&interface, testBuf1, &nchInput);

   TEST_ASSERT_EQUAL_UINT8(nchInput, nchTest);

   /* "OK" exit cmd detected */

   nchInput = strlen(testBuf2);

   gsmSetSerialMode_ExpectAndReturn(&(interface.engine), COMMAND_MODE, true);

   nchTest = gsmCheckDataMode(&interface, testBuf2, &nchInput);

   TEST_ASSERT_EQUAL_UINT8(15, nchTest);

   /* "CLOSED" exit cmd detected */

   nchInput = strlen(testBuf3);

   gsmSetSerialMode_ExpectAndReturn(&(interface.engine), COMMAND_MODE, true);

   nchTest = gsmCheckDataMode(&interface, testBuf3, &nchInput);

   TEST_ASSERT_EQUAL_UINT8(13, nchTest);

}

/* test_gsmSmsSend
 *
 * Functions tested:
 *
 * - gsmSmsSend
 *
 * */

void test_gsmSmsSend(void)
{
   /* Variables */

   gsmInterface_t interface;
   frmCback_t cbackTest;
   smsOut_s msgTest;
   smsConf_s confTest;

   /* Test sequence */

   gsmSmsSend(&interface, &msgTest, &confTest, cbackTest);

   TEST_ASSERT_TRUE(&msgTest == interface.frmInput);
   TEST_ASSERT_TRUE(&confTest == interface.frmOutput);
   TEST_ASSERT_TRUE(cbackTest == interface.frmCback);
   TEST_ASSERT_TRUE(INIT == interface.frmState);

}

/* test_gsmSmsRead
 *
 * Functions tested:
 *
 * - gsmSmsRead
 *
 * */

void test_gsmSmsRead(void)
{
   /* Variables */

   gsmInterface_t interface;
   frmCback_t cbackTest;
   smsRec_s msgTest;
   smsReadPars_s parsTest;

   /* Test sequence */

   gsmSmsRead(&interface, &msgTest, &parsTest, cbackTest);

   TEST_ASSERT_TRUE(&parsTest == interface.frmInput);
   TEST_ASSERT_TRUE(&msgTest == interface.frmOutput);
   TEST_ASSERT_TRUE(cbackTest == interface.frmCback);
   TEST_ASSERT_TRUE(INIT == interface.frmState);

}

/* test_gsmSmsList
 *
 * Functions tested:
 *
 * - gsmSmsList
 *
 * */

void test_gsmSmsList(void)
{
   /* Variables */

   gsmInterface_t interface;
   frmCback_t cbackTest;
   smsRec_s listTest;
   smsListPars_s parsTest;

   /* Test sequence */

   gsmSmsList(&interface, &listTest, &parsTest, cbackTest);

   TEST_ASSERT_TRUE(&parsTest == interface.frmInput);
   TEST_ASSERT_TRUE(&listTest == interface.frmOutput);
   TEST_ASSERT_TRUE(cbackTest == interface.frmCback);
   TEST_ASSERT_TRUE(INIT == interface.frmState);

}

/* test_gsmSmsDel
 *
 * Functions tested:
 *
 * - gsmSmsDel
 *
 * */

void test_gsmSmsDel(void)
{
   /* Variables */

   gsmInterface_t interface;
   frmCback_t cbackTest;
   smsDelPars_s msgdelTest;

   /* Test sequence */

   gsmSmsDel(&interface, &msgdelTest, cbackTest);

   TEST_ASSERT_TRUE(&msgdelTest == interface.frmInput);
   TEST_ASSERT_TRUE(cbackTest == interface.frmCback);
   TEST_ASSERT_TRUE(INIT == interface.frmState);

}

/* test_gsmGprsStart
 *
 * Functions tested:
 *
 * - gsmGprsStart
 *
 * */

void test_gsmGprsStart(void)
{
   /* Variables */

   gsmInterface_t interface;
   frmCback_t cbackTest;
   apnUserPwd_s apnTest;

   /* Test sequence */

   gsmGprsStart(&interface, &apnTest, cbackTest);

   TEST_ASSERT_TRUE(&apnTest == interface.frmInput);
   TEST_ASSERT_TRUE(cbackTest == interface.frmCback);
   TEST_ASSERT_TRUE(INIT == interface.frmState);

}

/* test_gsmGprsStop
 *
 * Functions tested:
 *
 * - gsmGprsStop
 *
 * */

void test_gsmGprsStop(void)
{
   /* Variables */

   gsmInterface_t interface;
   frmCback_t cbackTest;

   /* Test sequence */

   gsmGprsStop(&interface, cbackTest);

   TEST_ASSERT_TRUE(cbackTest == interface.frmCback);
   TEST_ASSERT_TRUE(INIT == interface.frmState);

}

/* test_gsmGprsOpenPort
 *
 * Functions tested:
 *
 * - gsmGprsOpenPort
 *
 * */

void test_gsmGprsOpenPort(void)
{
   /* Variables */

   gsmInterface_t interface;
   frmCback_t cbackTest;
   socket_s portTest;

   /* Test sequence */

   gsmGprsOpenPort(&interface, &portTest, cbackTest);

   TEST_ASSERT_TRUE(&portTest == interface.frmInput);
   TEST_ASSERT_TRUE(cbackTest == interface.frmCback);
   TEST_ASSERT_TRUE(INIT == interface.frmState);

}

/* test_gsmGprsClosePort
 *
 * Functions tested:
 *
 * - gsmGprsClosePort
 *
 * */

void test_gsmGprsClosePort(void)
{
   /* Variables */

   gsmInterface_t interface;
   frmCback_t cbackTest;

   /* Test sequence */

   gsmGprsClosePort(&interface, cbackTest);

   TEST_ASSERT_TRUE(cbackTest == interface.frmCback);
   TEST_ASSERT_TRUE(INIT == interface.frmState);

}

/* test_gsmGnssPwr
 *
 * Functions tested:
 *
 * - gsmGprsGnssPwr
 *
 * */

void test_gsmGnssPwr(void)
{
   /* Variables */

   gsmInterface_t interface;
   frmCback_t cbackTest;
   pwrGnss_e cmdTest;

   /* Test sequence */

   gsmGnssPwr(&interface, &cmdTest, cbackTest);

   TEST_ASSERT_TRUE(&cmdTest == interface.frmInput);
   TEST_ASSERT_TRUE(cbackTest == interface.frmCback);
   TEST_ASSERT_TRUE(INIT == interface.frmState);

}

/* test_gsmGnssGetData
 *
 * Functions tested:
 *
 * - gsmGprsGnssGetData
 *
 * */

void test_gsmGnssGetData(void)
{
   /* Variables */

   gsmInterface_t interface;
   frmCback_t cbackTest;
   dataGnss_s dataGnssTest;

   /* Test sequence */

   gsmGnssGetData(&interface, &dataGnssTest, cbackTest);

   TEST_ASSERT_TRUE(&dataGnssTest == interface.frmOutput);
   TEST_ASSERT_TRUE(cbackTest == interface.frmCback);
   TEST_ASSERT_TRUE(INIT == interface.frmState);

}

/* test_gsmStartUpF
 *
 * Functions tested:
 *
 * - gsmStartupF
 * - gsmStartUp
 * - gsmFrmInit
 * - gsmFrmSendCmdCheckEcho
 * - gsmFrmProcRspsGetFinal
 * - gsmFrmCopyGsmError
 *
 * */

void test_gsmStartUpF(void)
{
   /* Variables */

   gsmInterface_t interface;

   /* Initialization */

   interface.frmCback = frmCbackTest;
   frmFlag = false;

   /* Test sequence */

   /* All responses OK */

   gsmStartUp(&interface, frmCbackTest);

   interface.frm(&interface);

   TEST_ASSERT_TRUE(OK == interface.errorOut.errorFrm);
   TEST_ASSERT_TRUE('\0' == interface.errorOut.errorCmd.cmd[0]);
   TEST_ASSERT_TRUE('\0' == interface.errorOut.errorCmd.par[0]);
   TEST_ASSERT_EQUAL_UINT8(ATCMD1, interface.procState);
   TEST_ASSERT_EQUAL_UINT8(PROC, interface.frmState);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),"AT\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),"AT+CMEE=2\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),"AT+CSCS=\"GSM\"\r",
                              OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),"AT+CMGF=1\r",
                              OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   interface.frm(&interface);

   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

   /* AT\r echo incorrect */

   frmCbackFlag = false;

   gsmStartUp(&interface, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),"AT\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), ERR_CMD_ECHO);

   interface.frm(&interface);

   interface.frm(&interface);

   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

   /* ERROR message after AT\r+echo */

   rsp_t rspTest;

   strncpy(rspTest.cmd, "ERROR", strlen("ERROR"));
   rspTest.cmd[strlen("ERROR")] = '\0';
   strncpy(rspTest.par, "abcde", strlen("abcde"));
   rspTest.par[strlen("abcde")] = '\0';

   frmCbackFlag = false;

   gsmStartUp(&interface, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),"AT\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), ERR_MSG_CLOSE);

   interface.frm(&interface);

   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest);

   interface.frm(&interface);

   TEST_ASSERT_EQUAL_STRING("ERROR", interface.errorOut.errorCmd.cmd);
   TEST_ASSERT_EQUAL_STRING("abcde", interface.errorOut.errorCmd.par);
   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

   return;
}

/* test_gsmExitDataModeF
 *
 * Functions tested:
 *
 * - gsmExitDataModeF
 * - gsmExitDataMode
 *
 * */

void test_gsmExitDataModeF(void)
{
   /* Variables */

   gsmInterface_t interface;

   /* Initialization */

   frmCbackFlag = false;

   /* Test sequence */

   gsmExitDataMode(&interface, frmCbackTest);

   interface.frm(&interface); /* Will remain in INIT state until auxCnt goes
                                 down to 0 */

   TEST_ASSERT_EQUAL_UINT8(INIT, interface.frmState);

   interface.auxCnt = 0;

   gsm232UartSend_ExpectAndReturn("+++",strlen("+++"),3);

   interface.frm(&interface); /* Sends +++ and reloads auxCnt */

   TEST_ASSERT_EQUAL_UINT32(1000, interface.auxCnt);
   TEST_ASSERT_EQUAL_UINT8(PROC, interface.frmState);

   interface.frm(&interface); /* Will remain in PROC state until auxCnt goes
                                 down to 0 */

   TEST_ASSERT_EQUAL_UINT8(PROC, interface.frmState);

   interface.auxCnt = 0;

   interface.frm(&interface); /* Goes to WRAP */

   TEST_ASSERT_EQUAL_UINT8(WRAP, interface.frmState);

   interface.frm(&interface);

   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

}

/* test_gsmGetSigQualF
 *
 * Functions tested:
 *
 * - gsmGetSigQualF
 * - gsmGetSigQual
 * - gsmFrmInit
 * - gsmFrmSendCmdCheckEcho
 * - gsmFrmProcRspsGetFinal
 *
 * */

void test_gsmGetSigQualF(void)
{
   /* Variables */

   gsmInterface_t interface;
   sigQual_s sigQualTest;


   /* Initialization */

   frmCbackFlag = false;

   /* Test sequence */

   /* Middle RRSI value */

   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),"AT+CSQ\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_RSP);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   rsp_t rspTest1 = {"CSQ"," 2,1"};

   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest1);

   interface.frm(&interface);

   TEST_ASSERT_EQUAL_INT8(-110, sigQualTest.rssi);
   TEST_ASSERT_EQUAL_UINT8(1, sigQualTest.ber);
   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

   /* Lower limit RRSI value */

   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),"AT+CSQ\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_RSP);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   rsp_t rspTest2 = {"CSQ"," 99,7"};

   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest2);

   interface.frm(&interface);

   TEST_ASSERT_EQUAL_INT8(99, sigQualTest.rssi);
   TEST_ASSERT_EQUAL_UINT8(7, sigQualTest.ber);
   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

   /* Upper limit RRSI value */

   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),"AT+CSQ\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_RSP);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   rsp_t rspTest3 = {"CSQ"," 31,1"};

   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest3);

   interface.frm(&interface);

   TEST_ASSERT_EQUAL_INT8(-52, sigQualTest.rssi);
   TEST_ASSERT_EQUAL_UINT8(1, sigQualTest.ber);
   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

   /* Incorrectly formated response (no comma) */

   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),"AT+CSQ\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_RSP);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   rsp_t rspTest4 = {"CSQ","abcde"};

   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest4);

   interface.frm(&interface);

   TEST_ASSERT_EQUAL_INT(ERR_WRAP, interface.errorOut.errorFrm);
   TEST_ASSERT_EQUAL_INT8(-52, sigQualTest.rssi);
   TEST_ASSERT_EQUAL_UINT8(1, sigQualTest.ber);
   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

   /* Error response from modem */

   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),"AT+CSQ\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), ERR_MSG_CLOSE);

   interface.frm(&interface);

   rsp_t rspTest5 = {"ERROR","params"};

   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest5);

   interface.frm(&interface);

   TEST_ASSERT_EQUAL_INT(ERR_GSM, interface.errorOut.errorFrm);
   TEST_ASSERT_EQUAL_STRING("ERROR", interface.errorOut.errorCmd.cmd);
   TEST_ASSERT_EQUAL_STRING("params", interface.errorOut.errorCmd.par);
   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

}

/* test_gsmCheckConnF
 *
 * Functions tested:
 *
 * - gsmCheckConnF
 * - gsmCheckConn
 * - gsmFrmInit
 * - gsmFrmSendCmdCheckEcho
 * - gsmFrmProcRspsGetFinal
 *
 * */

void test_gsmCheckConnF(void)
{
   /* Variables */

   gsmInterface_t interface;
   connStatus_s connStateTest;
   rsp_t rspGsmTest;
   rsp_t rspGprsTest;

   /* Initialization */

   frmCbackFlag = false;

   strncpy(rspGsmTest.cmd, "CREG", strlen("CREG"));
   rspGsmTest.cmd[strlen("CREG")] = '\0';
   strncpy(rspGsmTest.par, " 0,1", strlen(" 1,0"));
   rspGsmTest.par[strlen(" 0,1")] = '\0';

   strncpy(rspGprsTest.cmd, "CGATT", strlen("CGATT"));
   rspGprsTest.cmd[strlen("CGATT")] = '\0';
   strncpy(rspGprsTest.par, " 1", strlen(" 1"));
   rspGprsTest.par[strlen(" 1")] = '\0';

   /* Test sequence */

   /* Normal functioning */

   gsmCheckConn(&interface, &connStateTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),"AT+CREG?\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_RSP);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);
   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspGsmTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),"AT+CGATT?\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_RSP);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);
   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspGprsTest);

   interface.frm(&interface);

   interface.frm(&interface);

   TEST_ASSERT_TRUE(connStateTest.gprs);
   TEST_ASSERT_TRUE(connStateTest.gsm);
   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

   /* Error response from modem */

   gsmCheckConn(&interface, &connStateTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),"AT+CREG?\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), ERR_MSG_CLOSE);

   interface.frm(&interface);

   rsp_t rspTest5 = {"ERROR","params"};

   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest5);

   interface.frm(&interface);

   TEST_ASSERT_EQUAL_INT(ERR_GSM, interface.errorOut.errorFrm);
   TEST_ASSERT_EQUAL_STRING("ERROR", interface.errorOut.errorCmd.cmd);
   TEST_ASSERT_EQUAL_STRING("params", interface.errorOut.errorCmd.par);
   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

}

/* test_gsmSmsSendF
 *
 * Functions tested:
 *
 * - gsmSmsSendF
 * - gsmSmsSend
 * - gsmFrmInit
 * - gsmFrmSendCmdCheckEcho
 * - gsmFrmProcRspsGetFinal
 *
 * */

void test_gsmSmsSendF(void)
{
   /* Variables */

   gsmInterface_t interface;
   smsOut_s msgTest1 = {"+5491151751809","Hola mundo!\r\n"};
   smsOut_s msgTest2 = {"+5491151751809","Hola mundo!"};
   smsConf_s confTest;
   rsp_t rspTest1 = {"OK",""};
   rsp_t rspTest2 = {"CMGS"," 5"};

   /* Initialization */

   frmCbackFlag = false;

   /* Test sequence */

   /* Normal functioning */

   gsmSmsSend(&interface, &msgTest2, &confTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),
                              "AT+CMGS=\"+5491151751809\"\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),
                              "Hola mundo!", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_RSP);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest1);
   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest2);

   interface.frm(&interface);

   TEST_ASSERT_EQUAL_UINT8(5, confTest.mr);
   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

   /* SMS text has forbidden characters; goes to WRAP with error */

   gsmSmsSend(&interface, &msgTest1, &confTest, frmCbackTest);

   interface.frm(&interface);

   interface.frm(&interface);

   TEST_ASSERT_EQUAL_INT(ERR_INIT, interface.errorOut.errorFrm);
   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

   /* Error response from modem */

   gsmSmsSend(&interface, &msgTest2, &confTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),
                              "AT+CMGS=\"+5491151751809\"\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), ERR_MSG_CLOSE);

   interface.frm(&interface);

   rsp_t rspTest5 = {"ERROR","params"};

   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest5);

   interface.frm(&interface);

   TEST_ASSERT_EQUAL_INT(ERR_GSM, interface.errorOut.errorFrm);
   TEST_ASSERT_EQUAL_STRING("ERROR", interface.errorOut.errorCmd.cmd);
   TEST_ASSERT_EQUAL_STRING("params", interface.errorOut.errorCmd.par);
   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

}

/* test_gsmSmsReadF
 *
 * Functions tested:
 *
 * - gsmSmsReadF
 * - gsmSmsRead
 * - gsmFrmInit
 * - gsmFrmSendCmdCheckEcho
 * - gsmFrmProcRspsGetFinal
 *
 * */

void test_gsmSmsReadF(void)
{
   /* Variables */

   gsmInterface_t interface;
   smsRec_s msgTest;
   smsReadPars_s parsTest = {1,NOCHANGE};
   rsp_t rspTest1 = {"CMGR","meta"};
   rsp_t rspTest2 = {"SMS_BODY","text"};

   /* Initialization */

   frmCbackFlag = false;

   /* Test sequence */

   /* Normal functioning */

   gsmSmsRead(&interface, &msgTest, &parsTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CSDH=1\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CMGR=1,1\r",
                              OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest1);
   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest2);

   interface.frm(&interface);

   TEST_ASSERT_EQUAL_STRING("meta", msgTest.meta);
   TEST_ASSERT_EQUAL_STRING("text", msgTest.text);
   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

   /* Error response from modem */

   gsmSmsRead(&interface, &msgTest, &parsTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CSDH=1\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), ERR_MSG_CLOSE);

   interface.frm(&interface);

   rsp_t rspTest5 = {"ERROR","params"};

   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest5);

   interface.frm(&interface);

   TEST_ASSERT_EQUAL_INT(ERR_GSM, interface.errorOut.errorFrm);
   TEST_ASSERT_EQUAL_STRING("ERROR", interface.errorOut.errorCmd.cmd);
   TEST_ASSERT_EQUAL_STRING("params", interface.errorOut.errorCmd.par);
   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

}

/* test_gsmSmsListF
 *
 * Functions tested:
 *
 * - gsmSmsListF
 * - gsmSmsList
 * - gsmFrmInit
 * - gsmFrmSendCmdCheckEcho
 * - gsmFrmProcRspsGetFinal
 *
 * */

void test_gsmSmsListF(void)
{
   /* Variables */

   gsmInterface_t interface;
   smsRec_s listTest[3];
   smsListPars_s parsTest = {REC_UNREAD,NOCHANGE,3};
   rsp_t rspTest1 = {"CMGR","meta1"};
   rsp_t rspTest2 = {"SMS_BODY","text1"};
   rsp_t rspTest3 = {"CMGR","meta2"};
   rsp_t rspTest4 = {"SMS_BODY","text2"};
   rsp_t rspTest5 = {"CMGR","meta3"};
   rsp_t rspTest6 = {"SMS_BODY","text3"};

   /* Initialization */

   frmCbackFlag = false;

   /* Test sequence */

   /* Normal functioning */

   gsmSmsList(&interface, &listTest[0], &parsTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CSDH=1\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),
                              "AT+CMGL=\"REC_UNREAD\",1\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   /* The real execution would have n non-closing responses here for the
    * messages in store, but we need not test that part so we simply go to
    * the closing response */

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   /* We asume that there there are exactly three SMSs in store, so we have
    * two responses per SMS plus the OK response for a total of 7*/

   gsmGetNoCmdRsp_ExpectAndReturn(&(interface.engine), 7);
   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest1);
   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest2);
   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest3);
   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest4);
   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest5);
   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest6);

   interface.frm(&interface);

   TEST_ASSERT_EQUAL_STRING("meta1", listTest[0].meta);
   TEST_ASSERT_EQUAL_STRING("text1", listTest[0].text);
   TEST_ASSERT_EQUAL_STRING("meta2", listTest[1].meta);
   TEST_ASSERT_EQUAL_STRING("text2", listTest[1].text);
   TEST_ASSERT_EQUAL_STRING("meta3", listTest[2].meta);
   TEST_ASSERT_EQUAL_STRING("text3", listTest[2].text);
   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

   /* Space in message vector < number of SMSs read */

   frmCbackFlag = false;

   gsmSmsList(&interface, &listTest[0], &parsTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CSDH=1\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),
                              "AT+CMGL=\"REC_UNREAD\",1\r", OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   /* The real execution would have n non-closing responses here for the
    * messages in store, but we need not test that part so we simply go to
    * the closing response */

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   /* We asume that there there are four SMSs in store, so we have
    * two responses per SMS plus the OK response for a total of 9. The list
    * vector only has space for three and thus will raise an error during the
    * WRAP process. */

   gsmGetNoCmdRsp_ExpectAndReturn(&(interface.engine), 9);

   interface.frm(&interface);

   TEST_ASSERT_EQUAL_UINT8(ERR_WRAP, interface.errorOut.errorFrm);
   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

   /* Test just for different options in smsListPars_s */

   smsListPars_s parsTest2 = {REC_READ,NOCHANGE,3};

   gsmSmsList(&interface, &listTest[0], &parsTest2, frmCbackTest);

   interface.frm(&interface);

   interface.procState = ATCMD2;

   //gsmSendCmd_ExpectAndReturn(&(interface.engine),
   //                           "AT+CMGL=\"REC READ\",1\r", OK_CMD_SENT);

   //interface.frm(&interface);

   //return;

}

/* test_gsmSmsDelF
 *
 * Functions tested:
 *
 * - gsmSmsDelF
 * - gsmSmsDel
 * - gsmFrmInit
 * - gsmFrmSendCmdCheckEcho
 * - gsmFrmProcRspsGetFinal
 *
 * */

void test_gsmSmsDelF(void)
{
   /* Variables */

   gsmInterface_t interface;
   smsDelPars_s parsTest = {1,INDEX};
   rsp_t rspTest1 = {"cmd","par"};

   /* Initialization */

   frmCbackFlag = false;

   /* Test sequence */

   /* Normal functioning */

   gsmSmsDel(&interface, &parsTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CMGD=1,0\r",
                              OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   interface.frm(&interface);

   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

}

/* test_gsmGprsStartF
 *
 * Functions tested:
 *
 * - gsmGprsStartF
 * - gsmGprsStart
 * - gsmFrmInit
 * - gsmFrmSendCmdCheckEcho
 * - gsmFrmProcRspsGetFinal
 *
 * */

void test_gsmGprsStartF(void)
{
   /* Variables */

   gsmInterface_t interface;
   apnUserPwd_s apnTest = {"apn","user","password"};
   rsp_t rspTest1 = {"cmd","par"};

   /* Initialization */

   frmCbackFlag = false;

   /* Test sequence */

   /* Normal functioning */

   gsmGprsStart(&interface, &apnTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CIPCLOSE\r",
                              OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CIPSHUT\r",
                              OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CIPMODE=1\r",
                              OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),
                              "AT+CSTT=\"apn\",\"user\",\"password\"\r",
                              OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CIICR\r",
                              OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CIFSR\r",
                              OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   interface.frm(&interface);

   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

}

/* test_gsmGprsStopF
 *
 * Functions tested:
 *
 * - gsmGprsStopF
 * - gsmGprsStop
 * - gsmFrmInit
 * - gsmFrmSendCmdCheckEcho
 * - gsmFrmProcRspsGetFinal
 *
 * */

void test_gsmGprsStopF(void)
{
   /* Variables */

   gsmInterface_t interface;

   /* Initialization */

   frmCbackFlag = false;

   /* Test sequence */

   /* Normal functioning */

   gsmGprsStop(&interface, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CIPSHUT\r",
                              OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   interface.frm(&interface);

   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

}

/* test_gsmGprsOpenPortF
 *
 * Functions tested:
 *
 * - gsmGprsOpenPortF
 * - gsmGprsOpenPort
 * - gsmFrmInit
 * - gsmFrmSendCmdCheckEcho
 * - gsmFrmProcRspsGetFinal
 *
 * */

void test_gsmGprsOpenPortF(void)
{
   /* Variables */

   gsmInterface_t interface;
   socket_s socketTest = {TCP, "192.168.0.1", 80};
   rsp_t rspTest1 = {"OK",""};
   rsp_t rspTest2 = {"CONNECT OK",""};

   /* Initialization */

   frmCbackFlag = false;

   /* Test sequence */

   /* Normal functioning */

   gsmGprsOpenPort(&interface, &socketTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CIPCLOSE\r",
                              OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine),
                              "AT+CIPSTART=\"TCP\",\"192.168.0.1\",\"80\"\r",
                             OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);
   gsmGetNoCmdRsp_ExpectAndReturn(&(interface.engine), 2);
   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest1);
   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest2);

   interface.frm(&interface);

   gsmSetSerialMode_ExpectAndReturn(&(interface.engine), DATA_MODE, true);

   interface.frm(&interface);

   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

}

/* test_gsmGprsClosePortF
 *
 * Functions tested:
 *
 * - gsmGprsClosePortF
 * - gsmGprsClosePort
 * - gsmFrmInit
 * - gsmFrmSendCmdCheckEcho
 * - gsmFrmProcRspsGetFinal
 *
 * */

void test_gsmGprsClosePortF(void)
{
   /* Variables */

   gsmInterface_t interface;

   /* Initialization */

   frmCbackFlag = false;

   /* Test sequence */

   /* Normal functioning */

   gsmGprsClosePort(&interface, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CIPCLOSE=0\r",
                              OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmSetSerialMode_ExpectAndReturn(&(interface.engine), COMMAND_MODE, true);

   interface.frm(&interface);

   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

}

/* test_gsmGnssPwrF
 *
 * Functions tested:
 *
 * - gsmGnssPwrF
 * - gsmGnssPwr
 * - gsmFrmInit
 * - gsmFrmSendCmdCheckEcho
 * - gsmFrmProcRspsGetFinal
 *
 * */

void test_gsmGnssPwrF(void)
{
   /* Variables */

   gsmInterface_t interface;
   pwrGnss_e cmdTest;

   /* Initialization */

   frmCbackFlag = false;

   /* Test sequence */

   /* Turn on */

   cmdTest = ON;

   gsmGnssPwr(&interface, &cmdTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CGNSPWR=1\r",
                              OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   interface.frm(&interface);

   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

   /* Turn off */

   cmdTest = OFF;

   gsmGnssPwr(&interface, &cmdTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CGNSPWR=0\r",
                              OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   interface.frm(&interface);

   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

}

/* test_gsmGnssGetDataF
 *
 * Functions tested:
 *
 * - gsmGnssGetDataF
 * - gsmGnssGetData
 * - gsmFrmInit
 * - gsmFrmSendCmdCheckEcho
 * - gsmFrmProcRspsGetFinal
 *
 * */

void test_gsmGnssGetDataF(void)
{
   /* Variables */

   gsmInterface_t interface;
   dataGnss_s dataGnssTest;
   rsp_t rspTest = {"CGNSINF","data"};

   /* Initialization */

   frmCbackFlag = false;

   /* Test sequence */

   gsmGnssGetData(&interface, &dataGnssTest, frmCbackTest);

   interface.frm(&interface);

   gsmSendCmd_ExpectAndReturn(&(interface.engine), "AT+CGNSINF\r",
                              OK_CMD_SENT);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CMD_ACK);

   interface.frm(&interface);

   gsmProcessTkn_ExpectAndReturn(&(interface.engine), OK_CLOSE);

   interface.frm(&interface);

   gsmGetCmdRsp_ExpectAndReturn(&(interface.engine), rspTest);

   interface.frm(&interface);

   TEST_ASSERT_EQUAL_STRING("data", dataGnssTest.data);
   TEST_ASSERT_TRUE(frmCbackFlag);
   TEST_ASSERT_EQUAL_UINT8(IDLE, interface.frmState);

}



