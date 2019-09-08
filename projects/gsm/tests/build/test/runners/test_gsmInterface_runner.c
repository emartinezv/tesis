/* AUTOGENERATED FILE. DO NOT EDIT. */

/*=======Test Runner Used To Run Each Test Below=====*/
#define RUN_TEST(TestFunc, TestLineNum) \
{ \
  Unity.CurrentTestName = #TestFunc; \
  Unity.CurrentTestLineNumber = TestLineNum; \
  Unity.NumberOfTests++; \
  CMock_Init(); \
  UNITY_CLR_DETAILS(); \
  if (TEST_PROTECT()) \
  { \
      setUp(); \
      TestFunc(); \
  } \
  if (TEST_PROTECT()) \
  { \
    tearDown(); \
    CMock_Verify(); \
  } \
  CMock_Destroy(); \
  UnityConcludeTest(); \
}

/*=======Automagically Detected Files To Include=====*/
#ifdef __WIN32__
#define UNITY_INCLUDE_SETUP_STUBS
#endif
#include "unity.h"
#include "cmock.h"
#ifndef UNITY_EXCLUDE_SETJMP_H
#include <setjmp.h>
#endif
#include <stdio.h>
#include "mock_gsmTokenizer.h"
#include "mock_gsmComms.h"
#include "mock_gsmParser.h"
#include "mock_gsmCommands.h"
#include "mock_gsmEngine.h"
#include "mock_vl_ring_buffer.h"

int GlobalExpectCount;
int GlobalVerifyOrder;
char* GlobalOrderError;

/*=======External Functions This Runner Calls=====*/
extern void setUp(void);
extern void tearDown(void);
extern void test_gsmInitInterface(void);
extern void test_gsmStartUp(void);
extern void test_gsmExitDataMode(void);
extern void test_gsmSysTickHandler(void);
extern void test_gsmProcess(void);
extern void test_gsmIsIdle(void);
extern void test_gsmGetSigQual(void);
extern void test_gsmCheckConn(void);
extern void test_gsmSetUrcMode(void);
extern void test_gsmSetUrcCback(void);
extern void test_gsmSetDataCback(void);
extern void test_gsmCheckDataMode(void);
extern void test_gsmSmsSend(void);
extern void test_gsmSmsRead(void);
extern void test_gsmSmsList(void);
extern void test_gsmSmsDel(void);
extern void test_gsmGprsStart(void);
extern void test_gsmGprsStop(void);
extern void test_gsmGprsOpenPort(void);
extern void test_gsmGprsClosePort(void);
extern void test_gsmGnssPwr(void);
extern void test_gsmGnssGetData(void);
extern void test_gsmStartUpF(void);
extern void test_gsmExitDataModeF(void);
extern void test_gsmGetSigQualF(void);
extern void test_gsmCheckConnF(void);
extern void test_gsmSmsSendF(void);
extern void test_gsmSmsReadF(void);
extern void test_gsmSmsListF(void);
extern void test_gsmSmsDelF(void);
extern void test_gsmGprsStartF(void);
extern void test_gsmGprsStopF(void);
extern void test_gsmGprsOpenPortF(void);
extern void test_gsmGprsClosePortF(void);
extern void test_gsmGnssPwrF(void);
extern void test_gsmGnssGetDataF(void);


/*=======Mock Management=====*/
static void CMock_Init(void)
{
  GlobalExpectCount = 0;
  GlobalVerifyOrder = 0;
  GlobalOrderError = NULL;
  mock_gsmTokenizer_Init();
  mock_gsmComms_Init();
  mock_gsmParser_Init();
  mock_gsmCommands_Init();
  mock_gsmEngine_Init();
  mock_vl_ring_buffer_Init();
}
static void CMock_Verify(void)
{
  mock_gsmTokenizer_Verify();
  mock_gsmComms_Verify();
  mock_gsmParser_Verify();
  mock_gsmCommands_Verify();
  mock_gsmEngine_Verify();
  mock_vl_ring_buffer_Verify();
}
static void CMock_Destroy(void)
{
  mock_gsmTokenizer_Destroy();
  mock_gsmComms_Destroy();
  mock_gsmParser_Destroy();
  mock_gsmCommands_Destroy();
  mock_gsmEngine_Destroy();
  mock_vl_ring_buffer_Destroy();
}

/*=======Suite Setup=====*/
static void suite_setup(void)
{
#if defined(UNITY_WEAK_ATTRIBUTE) || defined(UNITY_WEAK_PRAGMA)
  suiteSetUp();
#endif
}

/*=======Suite Teardown=====*/
static int suite_teardown(int num_failures)
{
#if defined(UNITY_WEAK_ATTRIBUTE) || defined(UNITY_WEAK_PRAGMA)
  return suiteTearDown(num_failures);
#else
  return num_failures;
#endif
}

/*=======Test Reset Option=====*/
void resetTest(void);
void resetTest(void)
{
  CMock_Verify();
  CMock_Destroy();
  tearDown();
  CMock_Init();
  setUp();
}


/*=======MAIN=====*/
int main(void)
{
  suite_setup();
  UnityBegin("test_gsmInterface.c");
  RUN_TEST(test_gsmInitInterface, 90);
  RUN_TEST(test_gsmStartUp, 136);
  RUN_TEST(test_gsmExitDataMode, 160);
  RUN_TEST(test_gsmSysTickHandler, 184);
  RUN_TEST(test_gsmProcess, 226);
  RUN_TEST(test_gsmIsIdle, 305);
  RUN_TEST(test_gsmGetSigQual, 336);
  RUN_TEST(test_gsmCheckConn, 362);
  RUN_TEST(test_gsmSetUrcMode, 388);
  RUN_TEST(test_gsmSetUrcCback, 414);
  RUN_TEST(test_gsmSetDataCback, 436);
  RUN_TEST(test_gsmCheckDataMode, 458);
  RUN_TEST(test_gsmSmsSend, 514);
  RUN_TEST(test_gsmSmsRead, 542);
  RUN_TEST(test_gsmSmsList, 570);
  RUN_TEST(test_gsmSmsDel, 598);
  RUN_TEST(test_gsmGprsStart, 624);
  RUN_TEST(test_gsmGprsStop, 650);
  RUN_TEST(test_gsmGprsOpenPort, 674);
  RUN_TEST(test_gsmGprsClosePort, 700);
  RUN_TEST(test_gsmGnssPwr, 724);
  RUN_TEST(test_gsmGnssGetData, 750);
  RUN_TEST(test_gsmStartUpF, 776);
  RUN_TEST(test_gsmExitDataModeF, 929);
  RUN_TEST(test_gsmGetSigQualF, 984);
  RUN_TEST(test_gsmCheckConnF, 1047);
  RUN_TEST(test_gsmSmsSendF, 1131);
  RUN_TEST(test_gsmSmsReadF, 1219);
  RUN_TEST(test_gsmSmsListF, 1288);
  RUN_TEST(test_gsmSmsDelF, 1429);
  RUN_TEST(test_gsmGprsStartF, 1481);
  RUN_TEST(test_gsmGprsStopF, 1599);
  RUN_TEST(test_gsmGprsOpenPortF, 1649);
  RUN_TEST(test_gsmGprsClosePortF, 1721);
  RUN_TEST(test_gsmGnssPwrF, 1773);
  RUN_TEST(test_gsmGnssGetDataF, 1852);

  CMock_Guts_MemFreeFinal();
  return suite_teardown(UnityEnd());
}
