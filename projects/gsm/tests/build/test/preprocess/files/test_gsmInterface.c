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

static _Bool frmCbackFlag = 0;







static uint8_t const * const exitCmds [] = {"OK", "CLOSED"};











void urcCbackTest (uint8_t const * const cmd, uint8_t const * const par)

{

   UnityAssertEqualString((const char*)(("CMTI")), (const char*)((cmd)), (((void *)0)), (UNITY_UINT)(51));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((par)), (((void *)0)), (UNITY_UINT)(52));



   return;

}



void dataCbackTest (gsmInterface_t * interface)

{

   dataCbackFlag = 1;



   return;

}



void * frmCbackTest (errorUser_t error, void * output)

{

   frmCbackFlag = 1;

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







   gsmInitEngine_CMockExpectAndReturn(111, &(interface.engine), TRUE);







   result = gsmInitInterface(&interface);







   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(119)));};



   if ((IDLE == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(121)));};

   if ((NOCMD == interface.procState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(122)));};



   if ((10 == interface.procCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(124)));};

   if ((0 == interface.auxCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(125)));};



   if ((MANUAL_MODE == interface.urcMode)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(127)));};

   if ((((void *)0) == interface.urcCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(128)));};



   if ((((void *)0) == interface.dataCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(130)));};

   UnityAssertEqualString((const char*)(("OK")), (const char*)((interface.exitCmdList[0])), (((void *)0)), (UNITY_UINT)(131));

   UnityAssertEqualString((const char*)(("CLOSED")), (const char*)((interface.exitCmdList[1])), (((void *)0)), (UNITY_UINT)(132));



   if ((((void *)0) == interface.frm)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(134)));};

   if ((((void *)0) == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(135)));};

   if ((((void *)0) == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(136)));};

   if ((((void *)0) == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(137)));};

   if ((OK == interface.errorOut.errorFrm)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(138)));};



}

void test_gsmStartUp(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;













   gsmStartUp(&interface, cbackTest);



   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(164)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(165)));};



}

void test_gsmExitDataMode(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;







   gsmExitDataMode(&interface, cbackTest);



   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(188)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(189)));};



}

void test_gsmSysTickHandler(void)

{





   gsmInterface_t interface;







   gsmInitEngine_CMockExpectAndReturn(209, &(interface.engine), TRUE);

   gsmDecToutCnt_CMockExpect(210, &(interface.engine));

   gsmDecToutCnt_CMockExpect(211, &(interface.engine));







   gsmInitInterface(&interface);









   interface.procCnt = 100;

   interface.auxCnt = 100;



   gsmSysTickHandler();



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((99)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(225), UNITY_DISPLAY_STYLE_UINT32);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((99)), (UNITY_INT)(UNITY_UINT32)((interface.auxCnt)), (((void *)0)), (UNITY_UINT)(226), UNITY_DISPLAY_STYLE_UINT32);



   interface.procCnt = 0;

   interface.auxCnt = 0;







   gsmSysTickHandler();



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((0)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(235), UNITY_DISPLAY_STYLE_UINT32);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((0)), (UNITY_INT)(UNITY_UINT32)((interface.auxCnt)), (((void *)0)), (UNITY_UINT)(236), UNITY_DISPLAY_STYLE_UINT32);



}

void test_gsmProcess(void)

{





   gsmInterface_t interface;

   rsp_t rspTest = {"CMTI", "abcde"};







   interface.frmState = IDLE;

   interface.engine.serialMode = COMMAND_MODE;

   interface.urcMode = CBACK_MODE;

   interface.urcCback = urcCbackTest;

   interface.dataCback = dataCbackTest;

   interface.frm = frmTest;















   interface.procCnt = 100;



   gsmProcess(&interface);







   interface.procCnt = 0;



   gsmGetSerialMode_CMockExpectAndReturn(278, &(interface.engine), COMMAND_MODE);

   gsmProcessTkn_CMockExpectAndReturn(279, &(interface.engine), NO_UPDATE);

   gsmGetUrc_CMockExpectAndReturn(280, &(interface.engine), rspTest);



   gsmProcess(&interface);





   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((10)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(285), UNITY_DISPLAY_STYLE_UINT32);







   interface.procCnt = 0;



   gsmGetSerialMode_CMockExpectAndReturn(291, &(interface.engine), DATA_MODE);

   gsmGetSerialMode_CMockExpectAndReturn(292, &(interface.engine), DATA_MODE);



   gsmProcess(&interface);



   if ((dataCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(296)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((10)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(297), UNITY_DISPLAY_STYLE_UINT32);



   return;







   interface.procCnt = 0;

   interface.frmState = INIT;



   gsmProcess(&interface);



   if ((frmFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(308)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((10)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(309), UNITY_DISPLAY_STYLE_UINT32);



   return;



}

void test_gsmIsIdle(void)

{





   gsmInterface_t interface;

   _Bool result;







   interface.frmState = IDLE;



   result = gsmIsIdle(&interface);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(336)));};



   interface.frmState = INIT;



   result = gsmIsIdle(&interface);



   if (!(result)) {} else {UnityFail( ((" Expected FALSE Was TRUE")), (UNITY_UINT)((UNITY_UINT)(342)));};



}

void test_gsmGetSigQual(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   sigQual_t sigQualTest;







   gsmGetSigQual(&interface, &sigQualTest, cbackTest);



   if ((&sigQualTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(366)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(367)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(368)));};



}

void test_gsmCheckConn(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   connStatus_t connStateTest;







   gsmCheckConn(&interface, &connStateTest, cbackTest);



   if ((&connStateTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(392)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(393)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(394)));};



}

void test_gsmReadUrc(void)

{





   gsmInterface_t interface;

   urc_t urcTest = {"\0","\0"};

   _Bool result;



   rsp_t rspTest1 = {"\0","\0"};

   rsp_t rspTest2 = {"command",""};

   rsp_t rspTest3 = {"command","parameters"};











   gsmGetUrc_CMockExpectAndReturn(422, &(interface.engine), rspTest1);



   result = gsmReadUrc(&interface, &urcTest);



   if (!(result)) {} else {UnityFail( ((" Expected FALSE Was TRUE")), (UNITY_UINT)((UNITY_UINT)(426)));};

   UnityAssertEqualString((const char*)(("\0")), (const char*)((urcTest.cmd)), (((void *)0)), (UNITY_UINT)(427));

   UnityAssertEqualString((const char*)(("\0")), (const char*)((urcTest.par)), (((void *)0)), (UNITY_UINT)(428));







   gsmGetUrc_CMockExpectAndReturn(432, &(interface.engine), rspTest2);



   result = gsmReadUrc(&interface, &urcTest);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(436)));};

   UnityAssertEqualString((const char*)(("command")), (const char*)((urcTest.cmd)), (((void *)0)), (UNITY_UINT)(437));

   UnityAssertEqualString((const char*)(("\0")), (const char*)((urcTest.par)), (((void *)0)), (UNITY_UINT)(438));







   gsmGetUrc_CMockExpectAndReturn(442, &(interface.engine), rspTest3);



   result = gsmReadUrc(&interface, &urcTest);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(446)));};

   UnityAssertEqualString((const char*)(("command")), (const char*)((urcTest.cmd)), (((void *)0)), (UNITY_UINT)(447));

   UnityAssertEqualString((const char*)(("parameters")), (const char*)((urcTest.par)), (((void *)0)), (UNITY_UINT)(448));



}

void test_gsmSetUrcMode(void)

{





   gsmInterface_t interface;







   gsmSetUrcMode(&interface, MANUAL_MODE);



   if ((MANUAL_MODE == interface.urcMode)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(470)));};



   gsmSetUrcMode(&interface, CBACK_MODE);



   if ((CBACK_MODE == interface.urcMode)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(474)));};



}

void test_gsmSetUrcCback(void)

{





   gsmInterface_t interface;







   gsmSetUrcCback(&interface, urcCbackTest);



   if ((urcCbackTest == interface.urcCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(496)));};



}

void test_gsmSetDataCback(void)

{





   gsmInterface_t interface;







   gsmSetDataCback(&interface, dataCbackTest);



   if ((dataCbackTest == interface.dataCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(518)));};



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



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((nchInput)), (UNITY_INT)(UNITY_UINT8 )((nchTest)), (((void *)0)), (UNITY_UINT)(554), UNITY_DISPLAY_STYLE_UINT8);







   nchInput = strlen(testBuf2);



   gsmSetSerialMode_CMockExpectAndReturn(560, &(interface.engine), COMMAND_MODE, 1);



   nchTest = gsmCheckDataMode(&interface, testBuf2, &nchInput);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((15)), (UNITY_INT)(UNITY_UINT8 )((nchTest)), (((void *)0)), (UNITY_UINT)(564), UNITY_DISPLAY_STYLE_UINT8);







   nchInput = strlen(testBuf3);



   gsmSetSerialMode_CMockExpectAndReturn(570, &(interface.engine), COMMAND_MODE, 1);



   nchTest = gsmCheckDataMode(&interface, testBuf3, &nchInput);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((13)), (UNITY_INT)(UNITY_UINT8 )((nchTest)), (((void *)0)), (UNITY_UINT)(574), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmSmsSend(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   smsOut_t msgTest;

   smsConf_t confTest;







   gsmSmsSend(&interface, &msgTest, &confTest, cbackTest);



   if ((&msgTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(599)));};

   if ((&confTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(600)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(601)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(602)));};



}

void test_gsmSmsRead(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   smsRec_t msgTest;

   smsReadPars_t parsTest;







   gsmSmsRead(&interface, &msgTest, &parsTest, cbackTest);



   if ((&parsTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(627)));};

   if ((&msgTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(628)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(629)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(630)));};



}

void test_gsmSmsList(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   smsRec_t listTest;

   smsListPars_t parsTest;







   gsmSmsList(&interface, &listTest, &parsTest, cbackTest);



   if ((&parsTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(655)));};

   if ((&listTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(656)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(657)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(658)));};



}

void test_gsmSmsDel(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   smsDelPars_t msgdelTest;







   gsmSmsDel(&interface, &msgdelTest, cbackTest);



   if ((&msgdelTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(682)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(683)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(684)));};



}

void test_gsmGprsStart(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   apnUserPwd_t apnTest;







   gsmGprsStart(&interface, &apnTest, cbackTest);



   if ((&apnTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(708)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(709)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(710)));};



}

void test_gsmGprsStop(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;







   gsmGprsStop(&interface, cbackTest);



   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(733)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(734)));};



}

void test_gsmGprsOpenPort(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   socket_t portTest;







   gsmGprsOpenPort(&interface, &portTest, cbackTest);



   if ((&portTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(758)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(759)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(760)));};



}

void test_gsmGprsClosePort(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;







   gsmGprsClosePort(&interface, cbackTest);



   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(783)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(784)));};



}

void test_gsmGnssPwr(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   pwrGnss_t cmdTest;







   gsmGnssPwr(&interface, &cmdTest, cbackTest);



   if ((&cmdTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(808)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(809)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(810)));};



}

void test_gsmGnssGetData(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   dataGnss_t dataGnssTest;







   gsmGnssGetData(&interface, &dataGnssTest, cbackTest);



   if ((&dataGnssTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(834)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(835)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(836)));};



}

void test_gsmStartUpF(void)

{





   gsmInterface_t interface;







   interface.frmCback = frmCbackTest;

   frmFlag = 0;











   gsmStartUp(&interface, frmCbackTest);



   interface.frm(&interface);



   if ((OK == interface.errorOut.errorFrm)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(878)));};

   if (('\0' == interface.errorOut.errorCmd.cmd[0])) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(879)));};

   if (('\0' == interface.errorOut.errorCmd.par[0])) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(880)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ATCMD1)), (UNITY_INT)(UNITY_UINT8 )((interface.procState)), (((void *)0)), (UNITY_UINT)(881), UNITY_DISPLAY_STYLE_UINT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((PROC)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(882), UNITY_DISPLAY_STYLE_UINT8);



   gsmSendCmd_CMockExpectAndReturn(884, &(interface.engine), "AT\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(888, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(892, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(896, &(interface.engine), "AT+CMEE=2\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(900, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(904, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 909

   , &(interface.engine), "AT+CSCS=\"GSM\"\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(913, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(917, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 922

   , &(interface.engine), "AT+CMGF=1\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(926, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(930, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(936)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(937), UNITY_DISPLAY_STYLE_UINT8);







   frmCbackFlag = 0;



   gsmStartUp(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(947, &(interface.engine), "AT\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(951, &(interface.engine), ERR_CMD_ECHO);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(957)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(958), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspTest;



   strncpy(rspTest.cmd, "ERROR", strlen("ERROR"));

   rspTest.cmd[strlen("ERROR")] = '\0';

   strncpy(rspTest.par, "abcde", strlen("abcde"));

   rspTest.par[strlen("abcde")] = '\0';



   frmCbackFlag = 0;



   gsmStartUp(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(975, &(interface.engine), "AT\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(979, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(983, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   gsmGetCmdRsp_CMockExpectAndReturn(987, &(interface.engine), rspTest);



   interface.frm(&interface);



   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(991));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(992));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(993)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(994), UNITY_DISPLAY_STYLE_UINT8);



   return;

}

void test_gsmExitDataModeF(void)

{





   gsmInterface_t interface;







   frmCbackFlag = 0;







   gsmExitDataMode(&interface, frmCbackTest);



   interface.frm(&interface);





   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((INIT)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1025), UNITY_DISPLAY_STYLE_UINT8);



   interface.auxCnt = 0;



   gsm232UartSend_CMockExpectAndReturn(1029, "+++", strlen("+++"), 3);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((1000)), (UNITY_INT)(UNITY_UINT32)((interface.auxCnt)), (((void *)0)), (UNITY_UINT)(1033), UNITY_DISPLAY_STYLE_UINT32);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((PROC)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1034), UNITY_DISPLAY_STYLE_UINT8);



   interface.frm(&interface);





   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((PROC)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1039), UNITY_DISPLAY_STYLE_UINT8);



   interface.auxCnt = 0;



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((WRAP)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1045), UNITY_DISPLAY_STYLE_UINT8);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1049)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1050), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGetSigQualF(void)

{





   gsmInterface_t interface;

   sigQual_t sigQualTest;







   frmCbackFlag = 0;











   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1085, &(interface.engine), "AT+CSQ\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1089, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1093, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1097, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest1 = {"CSQ"," 2,1"};



   gsmGetCmdRsp_CMockExpectAndReturn(1103, &(interface.engine), rspTest1);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((-110)), (UNITY_INT)(UNITY_INT8 )((sigQualTest.rssi)), (((void *)0)), (UNITY_UINT)(1107), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((1)), (UNITY_INT)(UNITY_UINT8 )((sigQualTest.ber)), (((void *)0)), (UNITY_UINT)(1108), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1109)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1110), UNITY_DISPLAY_STYLE_UINT8);







   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1118, &(interface.engine), "AT+CSQ\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1122, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1126, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1130, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest2 = {"CSQ"," 99,7"};



   gsmGetCmdRsp_CMockExpectAndReturn(1136, &(interface.engine), rspTest2);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((99)), (UNITY_INT)(UNITY_INT8 )((sigQualTest.rssi)), (((void *)0)), (UNITY_UINT)(1140), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((7)), (UNITY_INT)(UNITY_UINT8 )((sigQualTest.ber)), (((void *)0)), (UNITY_UINT)(1141), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1142)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1143), UNITY_DISPLAY_STYLE_UINT8);







   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1151, &(interface.engine), "AT+CSQ\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1155, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1159, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1163, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest3 = {"CSQ"," 31,1"};



   gsmGetCmdRsp_CMockExpectAndReturn(1169, &(interface.engine), rspTest3);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((-52)), (UNITY_INT)(UNITY_INT8 )((sigQualTest.rssi)), (((void *)0)), (UNITY_UINT)(1173), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((1)), (UNITY_INT)(UNITY_UINT8 )((sigQualTest.ber)), (((void *)0)), (UNITY_UINT)(1174), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1175)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1176), UNITY_DISPLAY_STYLE_UINT8);







   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1184, &(interface.engine), "AT+CSQ\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1188, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1192, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1196, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest4 = {"CSQ","abcde"};



   gsmGetCmdRsp_CMockExpectAndReturn(1202, &(interface.engine), rspTest4);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_WRAP)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1206), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((-52)), (UNITY_INT)(UNITY_INT8 )((sigQualTest.rssi)), (((void *)0)), (UNITY_UINT)(1207), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((1)), (UNITY_INT)(UNITY_UINT8 )((sigQualTest.ber)), (((void *)0)), (UNITY_UINT)(1208), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1209)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1210), UNITY_DISPLAY_STYLE_UINT8);







   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1218, &(interface.engine), "AT+CSQ\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1222, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1226, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest5 = {"ERROR","params"};



   gsmGetCmdRsp_CMockExpectAndReturn(1232, &(interface.engine), rspTest5);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_GSM)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1236), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1237));

   UnityAssertEqualString((const char*)(("params")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1238));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1239)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1240), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmCheckConnF(void)

{





   gsmInterface_t interface;

   connStatus_t connStateTest;

   rsp_t rspGsmTest = {"CREG", " 0,1"};

   rsp_t rspGprsTest = {"CGATT", " 1"};







   frmCbackFlag = 0;











   gsmCheckConn(&interface, &connStateTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1277, &(interface.engine), "AT+CREG?\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1281, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1285, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1289, &(interface.engine), OK_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(1290, &(interface.engine), rspGsmTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1294, &(interface.engine), "AT+CGATT?\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1298, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1302, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1306, &(interface.engine), OK_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(1307, &(interface.engine), rspGprsTest);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((connStateTest.gprs)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1313)));};

   if ((connStateTest.gsm)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1314)));};

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1315)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1316), UNITY_DISPLAY_STYLE_UINT8);







   gsmCheckConn(&interface, &connStateTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1324, &(interface.engine), "AT+CREG?\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1328, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1332, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest5 = {"ERROR","params"};



   gsmGetCmdRsp_CMockExpectAndReturn(1338, &(interface.engine), rspTest5);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_GSM)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1342), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1343));

   UnityAssertEqualString((const char*)(("params")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1344));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1345)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1346), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmSmsSendF(void)

{





   gsmInterface_t interface;

   smsOut_t msgTest1 = {"+5491151751809","Hola mundo!\r\n"};

   smsOut_t msgTest2 = {"+5491151751809","Hola mundo!"};

   smsConf_t confTest;

   rsp_t rspTest = {"CMGS", " 5"};







   frmCbackFlag = 0;











   gsmSmsSend(&interface, &msgTest2, &confTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1385

   , &(interface.engine), "AT+CMGS=\"+5491151751809\"\r", OK_CMD_SENT)

                                                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1389, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1393, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1398

   , &(interface.engine), "Hola mundo!", OK_CMD_SENT)

                                                         ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1402, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1406, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1410, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmGetCmdRsp_CMockExpectAndReturn(1414, &(interface.engine), rspTest);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((5)), (UNITY_INT)(UNITY_UINT8 )((confTest.mr)), (((void *)0)), (UNITY_UINT)(1418), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1419)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1420), UNITY_DISPLAY_STYLE_UINT8);







   gsmSmsSend(&interface, &msgTest1, &confTest, frmCbackTest);



   interface.frm(&interface);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_INIT)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1430), UNITY_DISPLAY_STYLE_INT);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1431)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1432), UNITY_DISPLAY_STYLE_UINT8);







   gsmSmsSend(&interface, &msgTest2, &confTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1441

   , &(interface.engine), "AT+CMGS=\"+5491151751809\"\r", OK_CMD_SENT)

                                                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1445, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1449, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest5 = {"ERROR","params"};



   gsmGetCmdRsp_CMockExpectAndReturn(1455, &(interface.engine), rspTest5);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_GSM)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1459), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1460));

   UnityAssertEqualString((const char*)(("params")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1461));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1462)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1463), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmSmsReadF(void)

{





   gsmInterface_t interface;

   smsRec_t msgTest;

   smsReadPars_t parsTest = {1,NOCHANGE};

   rsp_t rspTest1 = {"CMGR","meta"};

   rsp_t rspTest2 = {"SMS_BODY","text"};







   frmCbackFlag = 0;











   gsmSmsRead(&interface, &msgTest, &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1501, &(interface.engine), "AT+CSDH=1\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1505, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1509, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1514

   , &(interface.engine), "AT+CMGR=1,1\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1518, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1522, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmGetCmdRsp_CMockExpectAndReturn(1526, &(interface.engine), rspTest1);

   gsmGetCmdRsp_CMockExpectAndReturn(1527, &(interface.engine), rspTest2);



   interface.frm(&interface);



   UnityAssertEqualString((const char*)(("meta")), (const char*)((msgTest.meta)), (((void *)0)), (UNITY_UINT)(1531));

   UnityAssertEqualString((const char*)(("text")), (const char*)((msgTest.text)), (((void *)0)), (UNITY_UINT)(1532));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1533)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1534), UNITY_DISPLAY_STYLE_UINT8);







   gsmSmsRead(&interface, &msgTest, &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1542, &(interface.engine), "AT+CSDH=1\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1546, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1550, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest5 = {"ERROR","params"};



   gsmGetCmdRsp_CMockExpectAndReturn(1556, &(interface.engine), rspTest5);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_GSM)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1560), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1561));

   UnityAssertEqualString((const char*)(("params")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1562));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1563)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1564), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmSmsListF(void)

{





   gsmInterface_t interface;

   smsRec_t listTest[3];

   smsListPars_t parsTest = {REC_UNREAD,NOCHANGE,3};

   rsp_t rspTest1 = {"CMGR","meta1"};

   rsp_t rspTest2 = {"SMS_BODY","text1"};

   rsp_t rspTest3 = {"CMGR","meta2"};

   rsp_t rspTest4 = {"SMS_BODY","text2"};

   rsp_t rspTest5 = {"CMGR","meta3"};

   rsp_t rspTest6 = {"SMS_BODY","text3"};







   frmCbackFlag = 0;











   gsmSmsList(&interface, &listTest[0], &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1606, &(interface.engine), "AT+CSDH=1\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1610, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1614, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1619

   , &(interface.engine), "AT+CMGL=\"REC_UNREAD\",1\r", OK_CMD_SENT)

                                                                        ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1623, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);











   gsmProcessTkn_CMockExpectAndReturn(1631, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);









   gsmGetNoCmdRsp_CMockExpectAndReturn(1638, &(interface.engine), 7);

   gsmGetCmdRsp_CMockExpectAndReturn(1639, &(interface.engine), rspTest1);

   gsmGetCmdRsp_CMockExpectAndReturn(1640, &(interface.engine), rspTest2);

   gsmGetCmdRsp_CMockExpectAndReturn(1641, &(interface.engine), rspTest3);

   gsmGetCmdRsp_CMockExpectAndReturn(1642, &(interface.engine), rspTest4);

   gsmGetCmdRsp_CMockExpectAndReturn(1643, &(interface.engine), rspTest5);

   gsmGetCmdRsp_CMockExpectAndReturn(1644, &(interface.engine), rspTest6);



   interface.frm(&interface);



   UnityAssertEqualString((const char*)(("meta1")), (const char*)((listTest[0].meta)), (((void *)0)), (UNITY_UINT)(1648));

   UnityAssertEqualString((const char*)(("text1")), (const char*)((listTest[0].text)), (((void *)0)), (UNITY_UINT)(1649));

   UnityAssertEqualString((const char*)(("meta2")), (const char*)((listTest[1].meta)), (((void *)0)), (UNITY_UINT)(1650));

   UnityAssertEqualString((const char*)(("text2")), (const char*)((listTest[1].text)), (((void *)0)), (UNITY_UINT)(1651));

   UnityAssertEqualString((const char*)(("meta3")), (const char*)((listTest[2].meta)), (((void *)0)), (UNITY_UINT)(1652));

   UnityAssertEqualString((const char*)(("text3")), (const char*)((listTest[2].text)), (((void *)0)), (UNITY_UINT)(1653));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1654)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1655), UNITY_DISPLAY_STYLE_UINT8);







   frmCbackFlag = 0;



   gsmSmsList(&interface, &listTest[0], &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1665, &(interface.engine), "AT+CSDH=1\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1669, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1673, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1678

   , &(interface.engine), "AT+CMGL=\"REC_UNREAD\",1\r", OK_CMD_SENT)

                                                                        ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1682, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);











   gsmProcessTkn_CMockExpectAndReturn(1690, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);













   gsmGetNoCmdRsp_CMockExpectAndReturn(1699, &(interface.engine), 9);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_WRAP)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1703), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1704)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1705), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmSmsDelF(void)

{





   gsmInterface_t interface;

   smsDelPars_t parsTest = {1,INDEX};

   rsp_t rspTest1 = {"cmd","par"};







   frmCbackFlag = 0;











   gsmSmsDel(&interface, &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1742

   , &(interface.engine), "AT+CMGD=1,0\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1746, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1750, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1756)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1757), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGprsStartF(void)

{





   gsmInterface_t interface;

   apnUserPwd_t apnTest = {"apn","user","password"};

   rsp_t rspTest1 = {"cmd","par"};







   frmCbackFlag = 0;











   gsmGprsStart(&interface, &apnTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1794

   , &(interface.engine), "AT+CIPCLOSE\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1798, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1802, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1807

   , &(interface.engine), "AT+CIPSHUT\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1811, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1815, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1820

   , &(interface.engine), "AT+CIPMODE=1\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1824, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1828, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(



 1834

   , &(interface.engine), "AT+CSTT=\"apn\",\"user\",\"password\"\r", OK_CMD_SENT)



                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1838, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1842, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1847

   , &(interface.engine), "AT+CIICR\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1851, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1855, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1860

   , &(interface.engine), "AT+CIFSR\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1864, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1868, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1874)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1875), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGprsStopF(void)

{





   gsmInterface_t interface;







   frmCbackFlag = 0;











   gsmGprsStop(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1910

   , &(interface.engine), "AT+CIPSHUT\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1914, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1918, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1924)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1925), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGprsOpenPortF(void)

{





   gsmInterface_t interface;

   socket_t socketTest = {TCP, "192.168.0.1", 80};

   rsp_t rspTest1 = {"OK",""};

   rsp_t rspTest2 = {"CONNECT OK",""};







   frmCbackFlag = 0;











   gsmGprsOpenPort(&interface, &socketTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1963

   , &(interface.engine), "AT+CIPCLOSE\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1967, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1971, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(



 1977

   , &(interface.engine), "AT+CIPSTART=\"TCP\",\"192.168.0.1\",\"80\"\r", OK_CMD_SENT)



                                         ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1981, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1985, &(interface.engine), OK_CLOSE);

   gsmGetNoCmdRsp_CMockExpectAndReturn(1986, &(interface.engine), 2);

   gsmGetCmdRsp_CMockExpectAndReturn(1987, &(interface.engine), rspTest1);

   gsmGetCmdRsp_CMockExpectAndReturn(1988, &(interface.engine), rspTest2);



   interface.frm(&interface);



   gsmSetSerialMode_CMockExpectAndReturn(1992, &(interface.engine), DATA_MODE, 1);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1996)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1997), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGprsClosePortF(void)

{





   gsmInterface_t interface;







   frmCbackFlag = 0;











   gsmGprsClosePort(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 2032

   , &(interface.engine), "AT+CIPCLOSE=0\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2036, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2040, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSetSerialMode_CMockExpectAndReturn(2044, &(interface.engine), COMMAND_MODE, 1);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2048)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2049), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGnssPwrF(void)

{





   gsmInterface_t interface;

   pwrGnss_t cmdTest;







   frmCbackFlag = 0;











   cmdTest = ON;



   gsmGnssPwr(&interface, &cmdTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 2087

   , &(interface.engine), "AT+CGNSPWR=1\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2091, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2095, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2101)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2102), UNITY_DISPLAY_STYLE_UINT8);







   cmdTest = OFF;



   gsmGnssPwr(&interface, &cmdTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 2113

   , &(interface.engine), "AT+CGNSPWR=0\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2117, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2121, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2127)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2128), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGnssGetDataF(void)

{





   gsmInterface_t interface;

   dataGnss_t dataGnssTest;

   rsp_t rspTest = {"CGNSINF","data"};







   frmCbackFlag = 0;







   gsmGnssGetData(&interface, &dataGnssTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 2163

   , &(interface.engine), "AT+CGNSINF\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2167, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2171, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmGetCmdRsp_CMockExpectAndReturn(2175, &(interface.engine), rspTest);



   interface.frm(&interface);



   UnityAssertEqualString((const char*)(("data")), (const char*)((dataGnssTest.data)), (((void *)0)), (UNITY_UINT)(2179));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2180)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2181), UNITY_DISPLAY_STYLE_UINT8);



}
