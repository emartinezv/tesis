#include "build/temp/_test_gsmInterface.c"
#include "ring_buffer.h"
#include "mock_vl_ring_buffer.h"
#include "mock_gsmEngine.h"
#include "mock_gsmCommands.h"
#include "mock_gsmParser.h"
#include "mock_gsmComms.h"
#include "mock_gsmTokenizer.h"
#include "gsmInterface.h"
#include "unity.h"


















static _Bool dataCbackFlag = 0;

static _Bool frmFlag = 0;



static uint8_t const * const exitCmds [] = {"OK", "CLOSED"};











void urcCbackTest (uint8_t const * const cmd, uint8_t const * const par)

{

   UnityAssertEqualString((const char*)(("CMTI")), (const char*)((cmd)), (((void *)0)), (UNITY_UINT)(46));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((par)), (((void *)0)), (UNITY_UINT)(47));



   return;

}



void dataCbackTest (gsmInterface_t * interface)

{

   dataCbackFlag = 1;



   return;

}



void frmTest (gsmInterface_t * interface)

{

   frmFlag = 1;



   return;

}











void setUp(void)

{

   return;

}



void tearDown(void)

{

   return;

}

void test_gsmInitInterface(void)

{





   gsmInterface_t interface;

   _Bool result;







   gsmInitEngine_CMockExpectAndReturn(101, &(interface.engine), TRUE);







   result = gsmInitInterface(&interface);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(107)));};



   if ((IDLE == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(109)));};

   if ((NOCMD == interface.procState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(110)));};



   if ((10 == interface.procCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(112)));};

   if ((0 == interface.auxCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(113)));};



   if ((MANUAL_MODE == interface.urcMode)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(115)));};

   if ((((void *)0) == interface.urcCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(116)));};



   if ((((void *)0) == interface.dataCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(118)));};

   UnityAssertEqualString((const char*)(("OK")), (const char*)((interface.exitCmdList[0])), (((void *)0)), (UNITY_UINT)(119));

   UnityAssertEqualString((const char*)(("CLOSED")), (const char*)((interface.exitCmdList[1])), (((void *)0)), (UNITY_UINT)(120));



   if ((((void *)0) == interface.frm)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(122)));};

   if ((((void *)0) == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(123)));};

   if ((((void *)0) == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(124)));};

   if ((((void *)0) == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(125)));};

   if ((OK == interface.errorOut.errorFrm)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(126)));};



}

void test_gsmStartUp(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;







   gsmStartUp(&interface, cbackTest);



   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(149)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(150)));};



}

void test_gsmExitDataMode(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;







   gsmExitDataMode(&interface, cbackTest);



   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(173)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(174)));};



}

void test_gsmSysTickHandler(void)

{





   gsmInterface_t interface;







   gsmInitEngine_CMockExpectAndReturn(194, &(interface.engine), TRUE);

   gsmDecToutCnt_CMockExpect(195, &(interface.engine));

   gsmDecToutCnt_CMockExpect(196, &(interface.engine));







   gsmInitInterface(&interface);



   interface.procCnt = 100;

   interface.auxCnt = 100;



   gsmSysTickHandler();



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((99)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(207), UNITY_DISPLAY_STYLE_UINT32);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((99)), (UNITY_INT)(UNITY_UINT32)((interface.auxCnt)), (((void *)0)), (UNITY_UINT)(208), UNITY_DISPLAY_STYLE_UINT32);



   interface.procCnt = 0;

   interface.auxCnt = 0;



   gsmSysTickHandler();



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((0)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(215), UNITY_DISPLAY_STYLE_UINT32);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((0)), (UNITY_INT)(UNITY_UINT32)((interface.auxCnt)), (((void *)0)), (UNITY_UINT)(216), UNITY_DISPLAY_STYLE_UINT32);



}

void test_gsmProcess(void)

{





   gsmInterface_t interface;

   rsp_t rspTest;







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















   interface.procCnt = 100;



   gsmProcess(&interface);







   interface.procCnt = 0;



   gsmGetSerialMode_CMockExpectAndReturn(263, &(interface.engine), COMMAND_MODE);

   gsmProcessTkn_CMockExpectAndReturn(264, &(interface.engine), NO_UPDATE);

   gsmGetUrc_CMockExpectAndReturn(265, &(interface.engine), rspTest);



   gsmProcess(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((10)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(269), UNITY_DISPLAY_STYLE_UINT32);







   interface.procCnt = 0;



   gsmGetSerialMode_CMockExpectAndReturn(275, &(interface.engine), DATA_MODE);

   gsmGetSerialMode_CMockExpectAndReturn(276, &(interface.engine), DATA_MODE);



   gsmProcess(&interface);



   if ((dataCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(280)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((10)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(281), UNITY_DISPLAY_STYLE_UINT32);



   return;







   interface.procCnt = 0;

   interface.frmState = INIT;



   gsmProcess(&interface);



   if ((frmFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(292)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((10)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(293), UNITY_DISPLAY_STYLE_UINT32);



   return;



}

void test_gsmIsIdle(void)

{





   gsmInterface_t interface;

   _Bool result;







   interface.frmState = IDLE;



   result = gsmIsIdle(&interface);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(320)));};



   interface.frmState = INIT;



   result = gsmIsIdle(&interface);



   if (!(result)) {} else {UnityFail( ((" Expected FALSE Was TRUE")), (UNITY_UINT)((UNITY_UINT)(326)));};



}

void test_gsmGetSigQual(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   sigQual_s sigQualTest;







   gsmGetSigQual(&interface, &sigQualTest, cbackTest);



   if ((&sigQualTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(350)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(351)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(352)));};



}

void test_gsmCheckConn(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   connStatus_s connStateTest;







   gsmCheckConn(&interface, &connStateTest, cbackTest);



   if ((&connStateTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(376)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(377)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(378)));};



}

void test_gsmSetUrcMode(void)

{





   gsmInterface_t interface;







   gsmSetUrcMode(&interface, MANUAL_MODE);



   if ((MANUAL_MODE == interface.urcMode)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(400)));};



   gsmSetUrcMode(&interface, CBACK_MODE);



   if ((CBACK_MODE == interface.urcMode)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(404)));};



}

void test_gsmSetUrcCback(void)

{





   gsmInterface_t interface;







   gsmSetUrcCback(&interface, urcCbackTest);



   if ((urcCbackTest == interface.urcCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(426)));};



}

void test_gsmSetDataCback(void)

{





   gsmInterface_t interface;







   gsmSetDataCback(&interface, dataCbackTest);



   if ((dataCbackTest == interface.dataCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(448)));};



}

void test_gsmCheckDataMode(void)

{





   gsmInterface_t interface;

   uint8_t nchTest;

   uint8_t nchInput;



   uint8_t const * const testBuf1 = "blablabla123123123";

   uint8_t const * const testBuf2 = "blablabla\r\nOK\r\n123123123";

   uint8_t const * const testBuf3 = "bla\r\nCLOSED\r\n123123123";







   interface.exitCmdList = exitCmds;











   nchInput = strlen(testBuf1);



   nchTest = gsmCheckDataMode(&interface, testBuf1, &nchInput);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((nchInput)), (UNITY_INT)(UNITY_UINT8 )((nchTest)), (((void *)0)), (UNITY_UINT)(484), UNITY_DISPLAY_STYLE_UINT8);







   nchInput = strlen(testBuf2);



   gsmSetSerialMode_CMockExpectAndReturn(490, &(interface.engine), COMMAND_MODE, 1);



   nchTest = gsmCheckDataMode(&interface, testBuf2, &nchInput);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((15)), (UNITY_INT)(UNITY_UINT8 )((nchTest)), (((void *)0)), (UNITY_UINT)(494), UNITY_DISPLAY_STYLE_UINT8);







   nchInput = strlen(testBuf3);



   gsmSetSerialMode_CMockExpectAndReturn(500, &(interface.engine), COMMAND_MODE, 1);



   nchTest = gsmCheckDataMode(&interface, testBuf3, &nchInput);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((13)), (UNITY_INT)(UNITY_UINT8 )((nchTest)), (((void *)0)), (UNITY_UINT)(504), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmSmsSend(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   smsOut_s * msgTest;

   smsConf_s * confTest;







   gsmSmsSend(&interface, &msgTest, &confTest, cbackTest);



   if ((&msgTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(529)));};

   if ((&confTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(530)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(531)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(532)));};



}

void test_gsmSmsRead(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   smsRec_s * msgTest;

   smsReadPars_s * parsTest;







   gsmSmsRead(&interface, &msgTest, &parsTest, cbackTest);



   if ((&parsTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(557)));};

   if ((&msgTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(558)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(559)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(560)));};



}

void test_gsmSmsList(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   smsRec_s listTest;

   smsListPars_s parsTest;







   gsmSmsList(&interface, &listTest, &parsTest, cbackTest);



   if ((&parsTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(585)));};

   if ((&listTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(586)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(587)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(588)));};



}

void test_gsmSmsDel(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   smsDelPars_s msgdelTest;







   gsmSmsDel(&interface, &msgdelTest, cbackTest);



   if ((&msgdelTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(612)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(613)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(614)));};



}

void test_gsmGprsStart(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   apnUserPwd_s apnTest;







   gsmGprsStart(&interface, &apnTest, cbackTest);



   if ((&apnTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(638)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(639)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(640)));};



}

void test_gsmGprsStop(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;







   gsmGprsStop(&interface, cbackTest);



   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(663)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(664)));};



}

void test_gsmGprsOpenPort(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   socket_s portTest;







   gsmGprsOpenPort(&interface, &portTest, cbackTest);



   if ((&portTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(688)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(689)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(690)));};



}

void test_gsmGprsClosePort(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;







   gsmGprsClosePort(&interface, cbackTest);



   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(713)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(714)));};



}

void test_gsmGnssPwr(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   pwrGnss_e cmdTest;







   gsmGnssPwr(&interface, &cmdTest, cbackTest);



   if ((&cmdTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(738)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(739)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(740)));};



}

void test_gsmGnssGetData(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   dataGnss_s dataGnssTest;







   gsmGnssGetData(&interface, &dataGnssTest, cbackTest);



   if ((&dataGnssTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(764)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(765)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(766)));};



}
