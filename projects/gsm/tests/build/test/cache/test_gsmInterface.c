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

   if (!(interface.cmdSent)) {} else {UnityFail( ((" Expected FALSE Was TRUE")), (UNITY_UINT)((UNITY_UINT)(123)));};



   if ((10 == interface.procCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(125)));};

   if ((0 == interface.auxCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(126)));};



   if ((MANUAL_MODE == interface.urcMode)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(128)));};

   if ((((void *)0) == interface.urcCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(129)));};



   if ((((void *)0) == interface.dataCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(131)));};

   UnityAssertEqualString((const char*)(("OK")), (const char*)((interface.exitCmdList[0])), (((void *)0)), (UNITY_UINT)(132));

   UnityAssertEqualString((const char*)(("CLOSED")), (const char*)((interface.exitCmdList[1])), (((void *)0)), (UNITY_UINT)(133));



   if ((((void *)0) == interface.frm)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(135)));};

   if ((((void *)0) == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(136)));};

   if ((((void *)0) == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(137)));};

   if ((((void *)0) == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(138)));};

   if ((OK == interface.errorOut.errorFrm)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(139)));};



}

void test_gsmStartUp(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;













   gsmStartUp(&interface, cbackTest);



   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(165)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(166)));};



}

void test_gsmExitDataMode(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;







   gsmExitDataMode(&interface, cbackTest);



   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(189)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(190)));};



}

void test_gsmSysTickHandler(void)

{





   gsmInterface_t interface;







   gsmInitEngine_CMockExpectAndReturn(210, &(interface.engine), TRUE);

   gsmDecToutCnt_CMockExpect(211, &(interface.engine));

   gsmDecToutCnt_CMockExpect(212, &(interface.engine));







   gsmInitInterface(&interface);









   interface.procCnt = 100;

   interface.auxCnt = 100;



   gsmSysTickHandler();



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((99)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(226), UNITY_DISPLAY_STYLE_UINT32);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((99)), (UNITY_INT)(UNITY_UINT32)((interface.auxCnt)), (((void *)0)), (UNITY_UINT)(227), UNITY_DISPLAY_STYLE_UINT32);



   interface.procCnt = 0;

   interface.auxCnt = 0;







   gsmSysTickHandler();



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((0)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(236), UNITY_DISPLAY_STYLE_UINT32);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((0)), (UNITY_INT)(UNITY_UINT32)((interface.auxCnt)), (((void *)0)), (UNITY_UINT)(237), UNITY_DISPLAY_STYLE_UINT32);



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



   gsmGetSerialMode_CMockExpectAndReturn(279, &(interface.engine), COMMAND_MODE);

   gsmProcessTkn_CMockExpectAndReturn(280, &(interface.engine), NO_UPDATE);

   gsmGetUrc_CMockExpectAndReturn(281, &(interface.engine), rspTest);



   gsmProcess(&interface);





   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((10)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(286), UNITY_DISPLAY_STYLE_UINT32);







   interface.procCnt = 0;



   gsmGetSerialMode_CMockExpectAndReturn(292, &(interface.engine), DATA_MODE);

   gsmGetSerialMode_CMockExpectAndReturn(293, &(interface.engine), DATA_MODE);



   gsmProcess(&interface);



   if ((dataCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(297)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((10)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(298), UNITY_DISPLAY_STYLE_UINT32);



   return;







   interface.procCnt = 0;

   interface.frmState = INIT;



   gsmProcess(&interface);



   if ((frmFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(309)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((10)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(310), UNITY_DISPLAY_STYLE_UINT32);



   return;



}

void test_gsmIsIdle(void)

{





   gsmInterface_t interface;

   _Bool result;







   interface.frmState = IDLE;



   result = gsmIsIdle(&interface);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(337)));};



   interface.frmState = INIT;



   result = gsmIsIdle(&interface);



   if (!(result)) {} else {UnityFail( ((" Expected FALSE Was TRUE")), (UNITY_UINT)((UNITY_UINT)(343)));};



}

void test_gsmGetSigQual(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   sigQual_t sigQualTest;







   gsmGetSigQual(&interface, &sigQualTest, cbackTest);



   if ((&sigQualTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(367)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(368)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(369)));};



}

void test_gsmCheckConn(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   connStatus_t connStateTest;







   gsmCheckConn(&interface, &connStateTest, cbackTest);



   if ((&connStateTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(393)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(394)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(395)));};



}

void test_gsmReadUrc(void)

{





   gsmInterface_t interface;

   urc_t urcTest = {"\0","\0"};

   _Bool result;



   rsp_t rspTest1 = {"\0","\0"};

   rsp_t rspTest2 = {"command",""};

   rsp_t rspTest3 = {"command","parameters"};











   gsmGetUrc_CMockExpectAndReturn(423, &(interface.engine), rspTest1);



   result = gsmReadUrc(&interface, &urcTest);



   if (!(result)) {} else {UnityFail( ((" Expected FALSE Was TRUE")), (UNITY_UINT)((UNITY_UINT)(427)));};

   UnityAssertEqualString((const char*)(("\0")), (const char*)((urcTest.cmd)), (((void *)0)), (UNITY_UINT)(428));

   UnityAssertEqualString((const char*)(("\0")), (const char*)((urcTest.par)), (((void *)0)), (UNITY_UINT)(429));







   gsmGetUrc_CMockExpectAndReturn(433, &(interface.engine), rspTest2);



   result = gsmReadUrc(&interface, &urcTest);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(437)));};

   UnityAssertEqualString((const char*)(("command")), (const char*)((urcTest.cmd)), (((void *)0)), (UNITY_UINT)(438));

   UnityAssertEqualString((const char*)(("\0")), (const char*)((urcTest.par)), (((void *)0)), (UNITY_UINT)(439));







   gsmGetUrc_CMockExpectAndReturn(443, &(interface.engine), rspTest3);



   result = gsmReadUrc(&interface, &urcTest);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(447)));};

   UnityAssertEqualString((const char*)(("command")), (const char*)((urcTest.cmd)), (((void *)0)), (UNITY_UINT)(448));

   UnityAssertEqualString((const char*)(("parameters")), (const char*)((urcTest.par)), (((void *)0)), (UNITY_UINT)(449));



}

void test_gsmSetUrcMode(void)

{





   gsmInterface_t interface;







   gsmSetUrcMode(&interface, MANUAL_MODE);



   if ((MANUAL_MODE == interface.urcMode)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(471)));};



   gsmSetUrcMode(&interface, CBACK_MODE);



   if ((CBACK_MODE == interface.urcMode)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(475)));};



}

void test_gsmSetUrcCback(void)

{





   gsmInterface_t interface;







   gsmSetUrcCback(&interface, urcCbackTest);



   if ((urcCbackTest == interface.urcCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(497)));};



}

void test_gsmSetDataCback(void)

{





   gsmInterface_t interface;







   gsmSetDataCback(&interface, dataCbackTest);



   if ((dataCbackTest == interface.dataCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(519)));};



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



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((nchInput)), (UNITY_INT)(UNITY_UINT8 )((nchTest)), (((void *)0)), (UNITY_UINT)(555), UNITY_DISPLAY_STYLE_UINT8);







   nchInput = strlen(testBuf2);



   gsmSetSerialMode_CMockExpectAndReturn(561, &(interface.engine), COMMAND_MODE, 1);



   nchTest = gsmCheckDataMode(&interface, testBuf2, &nchInput);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((15)), (UNITY_INT)(UNITY_UINT8 )((nchTest)), (((void *)0)), (UNITY_UINT)(565), UNITY_DISPLAY_STYLE_UINT8);







   nchInput = strlen(testBuf3);



   gsmSetSerialMode_CMockExpectAndReturn(571, &(interface.engine), COMMAND_MODE, 1);



   nchTest = gsmCheckDataMode(&interface, testBuf3, &nchInput);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((13)), (UNITY_INT)(UNITY_UINT8 )((nchTest)), (((void *)0)), (UNITY_UINT)(575), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmSmsSend(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   smsOut_t msgTest;

   smsConf_t confTest;







   gsmSmsSend(&interface, &msgTest, &confTest, cbackTest);



   if ((&msgTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(600)));};

   if ((&confTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(601)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(602)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(603)));};



}

void test_gsmSmsRead(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   smsRec_t msgTest;

   smsReadPars_t parsTest;







   gsmSmsRead(&interface, &msgTest, &parsTest, cbackTest);



   if ((&parsTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(628)));};

   if ((&msgTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(629)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(630)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(631)));};



}

void test_gsmSmsList(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   smsRec_t listTest;

   smsListPars_t parsTest;







   gsmSmsList(&interface, &listTest, &parsTest, cbackTest);



   if ((&parsTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(656)));};

   if ((&listTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(657)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(658)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(659)));};



}

void test_gsmSmsDel(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   smsDelPars_t msgdelTest;







   gsmSmsDel(&interface, &msgdelTest, cbackTest);



   if ((&msgdelTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(683)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(684)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(685)));};



}

void test_gsmGprsStart(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   apnUserPwd_t apnTest;







   gsmGprsStart(&interface, &apnTest, cbackTest);



   if ((&apnTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(709)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(710)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(711)));};



}

void test_gsmGprsStop(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;







   gsmGprsStop(&interface, cbackTest);



   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(734)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(735)));};



}

void test_gsmGprsOpenPort(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   socket_t portTest;







   gsmGprsOpenPort(&interface, &portTest, cbackTest);



   if ((&portTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(759)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(760)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(761)));};



}

void test_gsmGprsClosePort(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;







   gsmGprsClosePort(&interface, cbackTest);



   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(784)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(785)));};



}

void test_gsmGnssPwr(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   pwrGnss_t cmdTest;







   gsmGnssPwr(&interface, &cmdTest, cbackTest);



   if ((&cmdTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(809)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(810)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(811)));};



}

void test_gsmGnssGetData(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   dataGnss_t dataGnssTest;







   gsmGnssGetData(&interface, &dataGnssTest, cbackTest);



   if ((&dataGnssTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(835)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(836)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(837)));};



}

void test_gsmStartUpF(void)

{





   gsmInterface_t interface;







   interface.frmCback = frmCbackTest;

   frmFlag = 0;











   gsmStartUp(&interface, frmCbackTest);



   interface.frm(&interface);



   if ((OK == interface.errorOut.errorFrm)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(879)));};

   if (('\0' == interface.errorOut.errorCmd.cmd[0])) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(880)));};

   if (('\0' == interface.errorOut.errorCmd.par[0])) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(881)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ATCMD1)), (UNITY_INT)(UNITY_UINT8 )((interface.procState)), (((void *)0)), (UNITY_UINT)(882), UNITY_DISPLAY_STYLE_UINT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((PROC)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(883), UNITY_DISPLAY_STYLE_UINT8);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 887

   , &(interface.engine), 1, "AT\r", strlen("AT\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(891, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(895, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 901

   , &(interface.engine), 1, "AT+CMEE=2\r", strlen("AT+CMEE=2\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(905, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(909, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 916

   , &(interface.engine), 1, "AT+CSCS=\"GSM\"\r", strlen("AT+CSCS=\"GSM\"\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(920, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(924, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 930

   , &(interface.engine), 1, "AT+CMGF=1\r", strlen("AT+CMGF=1\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(934, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(938, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(944)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(945), UNITY_DISPLAY_STYLE_UINT8);







   frmCbackFlag = 0;



   gsmStartUp(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 957

   , &(interface.engine), 1, "AT\r", strlen("AT\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(961, &(interface.engine), ERR_CMD_ECHO);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(967)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(968), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspTest = {"ERROR", "abcde"};



   frmCbackFlag = 0;



   gsmStartUp(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 982

   , &(interface.engine), 1, "AT\r", strlen("AT\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(986, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(990, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   gsmGetCmdRsp_CMockExpectAndReturn(994, &(interface.engine), rspTest);



   interface.frm(&interface);



   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(998));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(999));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1000)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1001), UNITY_DISPLAY_STYLE_UINT8);



   return;

}

void test_gsmExitDataModeF(void)

{





   gsmInterface_t interface;







   frmCbackFlag = 0;







   gsmExitDataMode(&interface, frmCbackTest);



   interface.frm(&interface);





   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((INIT)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1032), UNITY_DISPLAY_STYLE_UINT8);



   interface.auxCnt = 0;



   gsm232UartSend_CMockExpectWithArrayAndReturn(

 1037

   , "+++", strlen("+++"), strlen("+++"), 3)

                                                           ;



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((1000)), (UNITY_INT)(UNITY_UINT32)((interface.auxCnt)), (((void *)0)), (UNITY_UINT)(1041), UNITY_DISPLAY_STYLE_UINT32);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((PROC)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1042), UNITY_DISPLAY_STYLE_UINT8);



   interface.frm(&interface);





   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((PROC)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1047), UNITY_DISPLAY_STYLE_UINT8);



   interface.auxCnt = 0;



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((WRAP)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1053), UNITY_DISPLAY_STYLE_UINT8);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1057)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1058), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGetSigQualF(void)

{





   gsmInterface_t interface;

   sigQual_t sigQualTest;







   frmCbackFlag = 0;











   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1095

   , &(interface.engine), 1, "AT+CSQ\r", strlen("AT+CSQ\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1099, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1103, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1107, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest1 = {"CSQ"," 2,1"};



   gsmGetCmdRsp_CMockExpectAndReturn(1113, &(interface.engine), rspTest1);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((-110)), (UNITY_INT)(UNITY_INT8 )((sigQualTest.rssi)), (((void *)0)), (UNITY_UINT)(1117), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((1)), (UNITY_INT)(UNITY_UINT8 )((sigQualTest.ber)), (((void *)0)), (UNITY_UINT)(1118), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1119)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1120), UNITY_DISPLAY_STYLE_UINT8);







   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1130

   , &(interface.engine), 1, "AT+CSQ\r", strlen("AT+CSQ\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1134, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1138, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1142, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest2 = {"CSQ"," 99,7"};



   gsmGetCmdRsp_CMockExpectAndReturn(1148, &(interface.engine), rspTest2);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((99)), (UNITY_INT)(UNITY_INT8 )((sigQualTest.rssi)), (((void *)0)), (UNITY_UINT)(1152), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((7)), (UNITY_INT)(UNITY_UINT8 )((sigQualTest.ber)), (((void *)0)), (UNITY_UINT)(1153), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1154)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1155), UNITY_DISPLAY_STYLE_UINT8);







   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1165

   , &(interface.engine), 1, "AT+CSQ\r", strlen("AT+CSQ\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1169, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1173, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1177, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest3 = {"CSQ"," 31,1"};



   gsmGetCmdRsp_CMockExpectAndReturn(1183, &(interface.engine), rspTest3);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((-52)), (UNITY_INT)(UNITY_INT8 )((sigQualTest.rssi)), (((void *)0)), (UNITY_UINT)(1187), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((1)), (UNITY_INT)(UNITY_UINT8 )((sigQualTest.ber)), (((void *)0)), (UNITY_UINT)(1188), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1189)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1190), UNITY_DISPLAY_STYLE_UINT8);







   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1200

   , &(interface.engine), 1, "AT+CSQ\r", strlen("AT+CSQ\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1204, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1208, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1212, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest4 = {"CSQ","abcde"};



   gsmGetCmdRsp_CMockExpectAndReturn(1218, &(interface.engine), rspTest4);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_WRAP)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1222), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((-52)), (UNITY_INT)(UNITY_INT8 )((sigQualTest.rssi)), (((void *)0)), (UNITY_UINT)(1223), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((1)), (UNITY_INT)(UNITY_UINT8 )((sigQualTest.ber)), (((void *)0)), (UNITY_UINT)(1224), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1225)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1226), UNITY_DISPLAY_STYLE_UINT8);







   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1236

   , &(interface.engine), 1, "AT+CSQ\r", strlen("AT+CSQ\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1240, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1244, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest5 = {"ERROR","params"};



   gsmGetCmdRsp_CMockExpectAndReturn(1250, &(interface.engine), rspTest5);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_GSM)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1254), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1255));

   UnityAssertEqualString((const char*)(("params")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1256));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1257)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1258), UNITY_DISPLAY_STYLE_UINT8);



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



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1297

   , &(interface.engine), 1, "AT+CREG?\r", strlen("AT+CREG?\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1301, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1305, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1309, &(interface.engine), OK_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(1310, &(interface.engine), rspGsmTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1316

   , &(interface.engine), 1, "AT+CGATT?\r", strlen("AT+CGATT?\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1320, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1324, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1328, &(interface.engine), OK_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(1329, &(interface.engine), rspGprsTest);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((connStateTest.gprs)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1335)));};

   if ((connStateTest.gsm)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1336)));};

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1337)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1338), UNITY_DISPLAY_STYLE_UINT8);







   gsmCheckConn(&interface, &connStateTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1348

   , &(interface.engine), 1, "AT+CREG?\r", strlen("AT+CREG?\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1352, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1356, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest5 = {"ERROR","params"};



   gsmGetCmdRsp_CMockExpectAndReturn(1362, &(interface.engine), rspTest5);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_GSM)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1366), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1367));

   UnityAssertEqualString((const char*)(("params")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1368));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1369)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1370), UNITY_DISPLAY_STYLE_UINT8);



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



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1411

   , &(interface.engine), 1, "AT+CMGS=\"+5491151751809\"\r", strlen("AT+CMGS=\"+5491151751809\"\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1415, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1419, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(

 1424

   , &(interface.engine), 1, "Hola mundo!", strlen("Hola mundo!"), OK_CMD_SENT)

                                                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1428, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1432, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1436, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmGetCmdRsp_CMockExpectAndReturn(1440, &(interface.engine), rspTest);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((5)), (UNITY_INT)(UNITY_UINT8 )((confTest.mr)), (((void *)0)), (UNITY_UINT)(1444), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1445)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1446), UNITY_DISPLAY_STYLE_UINT8);







   gsmSmsSend(&interface, &msgTest1, &confTest, frmCbackTest);



   interface.frm(&interface);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_INIT)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1456), UNITY_DISPLAY_STYLE_INT);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1457)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1458), UNITY_DISPLAY_STYLE_UINT8);







   gsmSmsSend(&interface, &msgTest2, &confTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1469

   , &(interface.engine), 1, "AT+CMGS=\"+5491151751809\"\r", strlen("AT+CMGS=\"+5491151751809\"\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1473, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1477, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest5 = {"ERROR","params"};



   gsmGetCmdRsp_CMockExpectAndReturn(1483, &(interface.engine), rspTest5);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_GSM)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1487), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1488));

   UnityAssertEqualString((const char*)(("params")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1489));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1490)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1491), UNITY_DISPLAY_STYLE_UINT8);



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



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1531

   , &(interface.engine), 1, "AT+CSDH=1\r", strlen("AT+CSDH=1\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1535, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1539, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1545

   , &(interface.engine), 1, "AT+CMGR=1,1\r", strlen("AT+CMGR=1,1\r"), OK_CMD_SENT)



                                                                            ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1549, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1553, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmGetCmdRsp_CMockExpectAndReturn(1557, &(interface.engine), rspTest1);

   gsmGetCmdRsp_CMockExpectAndReturn(1558, &(interface.engine), rspTest2);



   interface.frm(&interface);



   UnityAssertEqualString((const char*)(("meta")), (const char*)((msgTest.meta)), (((void *)0)), (UNITY_UINT)(1562));

   UnityAssertEqualString((const char*)(("text")), (const char*)((msgTest.text)), (((void *)0)), (UNITY_UINT)(1563));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1564)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1565), UNITY_DISPLAY_STYLE_UINT8);







   gsmSmsRead(&interface, &msgTest, &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1575

   , &(interface.engine), 1, "AT+CSDH=1\r", strlen("AT+CSDH=1\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1579, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1583, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest5 = {"ERROR","params"};



   gsmGetCmdRsp_CMockExpectAndReturn(1589, &(interface.engine), rspTest5);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_GSM)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1593), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1594));

   UnityAssertEqualString((const char*)(("params")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1595));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1596)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1597), UNITY_DISPLAY_STYLE_UINT8);



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



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1642

   , &(interface.engine), 1, "AT+CSDH=1\r", strlen("AT+CSDH=1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1646, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1650, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1657

   , &(interface.engine), 1, "AT+CMGL=\"REC UNREAD\",1\r", strlen("AT+CMGL=\"REC UNREAD\",1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1661, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);











   gsmProcessTkn_CMockExpectAndReturn(1669, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);









   gsmGetNoCmdRsp_CMockExpectAndReturn(1676, &(interface.engine), 7);

   gsmGetCmdRsp_CMockExpectAndReturn(1677, &(interface.engine), rspTest1);

   gsmGetCmdRsp_CMockExpectAndReturn(1678, &(interface.engine), rspTest2);

   gsmGetCmdRsp_CMockExpectAndReturn(1679, &(interface.engine), rspTest3);

   gsmGetCmdRsp_CMockExpectAndReturn(1680, &(interface.engine), rspTest4);

   gsmGetCmdRsp_CMockExpectAndReturn(1681, &(interface.engine), rspTest5);

   gsmGetCmdRsp_CMockExpectAndReturn(1682, &(interface.engine), rspTest6);



   interface.frm(&interface);



   UnityAssertEqualString((const char*)(("meta1")), (const char*)((listTest[0].meta)), (((void *)0)), (UNITY_UINT)(1686));

   UnityAssertEqualString((const char*)(("text1")), (const char*)((listTest[0].text)), (((void *)0)), (UNITY_UINT)(1687));

   UnityAssertEqualString((const char*)(("meta2")), (const char*)((listTest[1].meta)), (((void *)0)), (UNITY_UINT)(1688));

   UnityAssertEqualString((const char*)(("text2")), (const char*)((listTest[1].text)), (((void *)0)), (UNITY_UINT)(1689));

   UnityAssertEqualString((const char*)(("meta3")), (const char*)((listTest[2].meta)), (((void *)0)), (UNITY_UINT)(1690));

   UnityAssertEqualString((const char*)(("text3")), (const char*)((listTest[2].text)), (((void *)0)), (UNITY_UINT)(1691));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1692)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1693), UNITY_DISPLAY_STYLE_UINT8);







   frmCbackFlag = 0;



   gsmSmsList(&interface, &listTest[0], &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1706

   , &(interface.engine), 1, "AT+CSDH=1\r", strlen("AT+CSDH=1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1710, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1714, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1721

   , &(interface.engine), 1, "AT+CMGL=\"REC UNREAD\",1\r", strlen("AT+CMGL=\"REC UNREAD\",1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1725, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);











   gsmProcessTkn_CMockExpectAndReturn(1733, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);













   gsmGetNoCmdRsp_CMockExpectAndReturn(1742, &(interface.engine), 9);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_WRAP)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1746), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1747)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1748), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspError = {"ERROR","abcde"};



   gsmSmsList(&interface, &listTest[0], &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1761

   , &(interface.engine), 1, "AT+CSDH=1\r", strlen("AT+CSDH=1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1765, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1769, &(interface.engine), ERR_MSG_CLOSE);

   gsmGetNoCmdRsp_CMockExpectAndReturn(1770, &(interface.engine), 1);

   gsmGetCmdRsp_CMockExpectAndReturn(1771, &(interface.engine), rspError);



   interface.frm(&interface);

   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_GSM)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1776), UNITY_DISPLAY_STYLE_UINT8);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1777));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1778));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1779)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1780), UNITY_DISPLAY_STYLE_UINT8);











   smsListPars_t parsTest2 = {REC_READ,NOCHANGE,3};



   gsmSmsList(&interface, &listTest[0], &parsTest2, frmCbackTest);



   interface.frm(&interface);



   interface.procState = ATCMD2;



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1797

   , &(interface.engine), 1, "AT+CMGL=\"REC READ\",1\r", strlen("AT+CMGL=\"REC READ\",1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);







   smsListPars_t parsTest3 = {STO_UNSENT,NOCHANGE,3};



   gsmSmsList(&interface, &listTest[0], &parsTest3, frmCbackTest);



   interface.frm(&interface);



   interface.procState = ATCMD2;



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1814

   , &(interface.engine), 1, "AT+CMGL=\"STO UNSENT\",1\r", strlen("AT+CMGL=\"STO UNSENT\",1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);







   smsListPars_t parsTest4 = {STO_SENT,NOCHANGE,3};



   gsmSmsList(&interface, &listTest[0], &parsTest4, frmCbackTest);



   interface.frm(&interface);



   interface.procState = ATCMD2;



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1831

   , &(interface.engine), 1, "AT+CMGL=\"STO SENT\",1\r", strlen("AT+CMGL=\"STO SENT\",1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);







   smsListPars_t parsTest5 = {ALL_MSG,NOCHANGE,3};



   gsmSmsList(&interface, &listTest[0], &parsTest5, frmCbackTest);



   interface.frm(&interface);



   interface.procState = ATCMD2;



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1848

   , &(interface.engine), 1, "AT+CMGL=\"ALL\",1\r", strlen("AT+CMGL=\"ALL\",1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   return;



}

void test_gsmSmsDelF(void)

{





   gsmInterface_t interface;

   smsDelPars_t parsTest = {1,INDEX};

   rsp_t rspTest1 = {"cmd","par"};







   frmCbackFlag = 0;











   gsmSmsDel(&interface, &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(

 1889

   , &(interface.engine), 1, "AT+CMGD=1,0\r", strlen("AT+CMGD=1,0\r"), OK_CMD_SENT)

                                                                            ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1893, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1897, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1903)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1904), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspError = {"ERROR","abcde"};



   gsmSmsDel(&interface, &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(

 1915

   , &(interface.engine), 1, "AT+CMGD=1,0\r", strlen("AT+CMGD=1,0\r"), OK_CMD_SENT)

                                                                            ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1919, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1923, &(interface.engine), ERR_MSG_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(1924, &(interface.engine), rspError);



   interface.frm(&interface);

   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_GSM)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1929), UNITY_DISPLAY_STYLE_UINT8);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1930));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1931));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1932)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1933), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGprsStartF(void)

{





   gsmInterface_t interface;

   apnUserPwd_t apnTest = {"apn","user","password"};

   rsp_t rspTest1 = {"cmd","par"};







   frmCbackFlag = 0;











   gsmGprsStart(&interface, &apnTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1972

   , &(interface.engine), 1, "AT+CIPCLOSE\r", strlen("AT+CIPCLOSE\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1976, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1980, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1987

   , &(interface.engine), 1, "AT+CIPSHUT\r", strlen("AT+CIPSHUT\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1991, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1995, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2002

   , &(interface.engine), 1, "AT+CIPMODE=1\r", strlen("AT+CIPMODE=1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2006, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2010, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(









 2019

   , &(interface.engine), 1, "AT+CSTT=\"apn\",\"user\"," "\"password\"\r", strlen("AT+CSTT=\"apn\",\"user\"," "\"password\"\r"), OK_CMD_SENT)









                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2023, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2027, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 2032

   , &(interface.engine), "AT+CIICR\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2036, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2040, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2047

   , &(interface.engine), 1, "AT+CIFSR\r", strlen("AT+CIFSR\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2051, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2055, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2061)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2062), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspError = {"ERROR","abcde"};



   gsmGprsStart(&interface, &apnTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2075

   , &(interface.engine), 1, "AT+CIPCLOSE\r", strlen("AT+CIPCLOSE\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2079, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2083, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2090

   , &(interface.engine), 1, "AT+CIPSHUT\r", strlen("AT+CIPSHUT\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2094, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2098, &(interface.engine), ERR_MSG_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(2099, &(interface.engine), rspError);



   interface.frm(&interface);

   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_GSM)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(2104), UNITY_DISPLAY_STYLE_UINT8);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(2105));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(2106));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2107)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2108), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGprsStopF(void)

{





   gsmInterface_t interface;







   frmCbackFlag = 0;











   gsmGprsStop(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2145

   , &(interface.engine), 1, "AT+CIPSHUT\r", strlen("AT+CIPSHUT\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2149, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2153, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2159)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2160), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspError = {"ERROR","abcde"};



   gsmGprsStop(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2173

   , &(interface.engine), 1, "AT+CIPSHUT\r", strlen("AT+CIPSHUT\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2177, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2181, &(interface.engine), ERR_MSG_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(2182, &(interface.engine), rspError);



   interface.frm(&interface);

   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_GSM)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(2187), UNITY_DISPLAY_STYLE_UINT8);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(2188));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(2189));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2190)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2191), UNITY_DISPLAY_STYLE_UINT8);



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



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2231

   , &(interface.engine), 1, "AT+CIPCLOSE\r", strlen("AT+CIPCLOSE\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2235, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2239, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(









 2248

   , &(interface.engine), 1, "AT+CIPSTART=\"TCP\",\"192.168.0.1" "\",\"80\"\r", strlen("AT+CIPSTART=\"TCP\",\"192.168" ".0.1\",\"80\"\r"), OK_CMD_SENT)









                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2252, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2256, &(interface.engine), OK_CLOSE);

   gsmGetNoCmdRsp_CMockExpectAndReturn(2257, &(interface.engine), 2);

   gsmGetCmdRsp_CMockExpectAndReturn(2258, &(interface.engine), rspTest1);

   gsmGetCmdRsp_CMockExpectAndReturn(2259, &(interface.engine), rspTest2);



   interface.frm(&interface);



   gsmSetSerialMode_CMockExpectAndReturn(2263, &(interface.engine), DATA_MODE, 1);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2267)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2268), UNITY_DISPLAY_STYLE_UINT8);







   socket_t socketTest2 = {UDP, "192.168.0.1", 80};



   gsmGprsOpenPort(&interface, &socketTest2, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2281

   , &(interface.engine), 1, "AT+CIPCLOSE\r", strlen("AT+CIPCLOSE\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2285, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2289, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(









 2298

   , &(interface.engine), 1, "AT+CIPSTART=\"UDP\",\"192.168.0.1" "\",\"80\"\r", strlen("AT+CIPSTART=\"UDP\",\"192.168" ".0.1\",\"80\"\r"), OK_CMD_SENT)









                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2302, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2306, &(interface.engine), OK_CLOSE);

   gsmGetNoCmdRsp_CMockExpectAndReturn(2307, &(interface.engine), 2);

   gsmGetCmdRsp_CMockExpectAndReturn(2308, &(interface.engine), rspTest1);

   gsmGetCmdRsp_CMockExpectAndReturn(2309, &(interface.engine), rspTest2);



   interface.frm(&interface);



   gsmSetSerialMode_CMockExpectAndReturn(2313, &(interface.engine), DATA_MODE, 1);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2317)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2318), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGprsClosePortF(void)

{





   gsmInterface_t interface;







   frmCbackFlag = 0;











   gsmGprsClosePort(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 2353

   , &(interface.engine), "AT+CIPCLOSE=0\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2357, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2361, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSetSerialMode_CMockExpectAndReturn(2365, &(interface.engine), COMMAND_MODE, 1);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2369)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2370), UNITY_DISPLAY_STYLE_UINT8);



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

 2408

   , &(interface.engine), "AT+CGNSPWR=1\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2412, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2416, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2422)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2423), UNITY_DISPLAY_STYLE_UINT8);







   cmdTest = OFF;



   gsmGnssPwr(&interface, &cmdTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 2434

   , &(interface.engine), "AT+CGNSPWR=0\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2438, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2442, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2448)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2449), UNITY_DISPLAY_STYLE_UINT8);



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

 2484

   , &(interface.engine), "AT+CGNSINF\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2488, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2492, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmGetCmdRsp_CMockExpectAndReturn(2496, &(interface.engine), rspTest);



   interface.frm(&interface);



   UnityAssertEqualString((const char*)(("data")), (const char*)((dataGnssTest.data)), (((void *)0)), (UNITY_UINT)(2500));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2501)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2502), UNITY_DISPLAY_STYLE_UINT8);



}
