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
   smsOut_s * msgTest;
   smsConf_s * confTest;

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
   smsRec_s * msgTest;
   smsReadPars_s * parsTest;

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


