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







   interface.procCnt = 0;

   interface.frmState = INIT;

   frmFlag = 0;



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



   uint8_t const * const testBuf1 = "blabla\r\nbla123123123\r\n";

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



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 886

   , &(interface.engine), 1, "AT\r", strlen("AT\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(890, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(894, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 900

   , &(interface.engine), 1, "AT+CMEE=2\r", strlen("AT+CMEE=2\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(904, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(908, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 915

   , &(interface.engine), 1, "AT+CSCS=\"GSM\"\r", strlen("AT+CSCS=\"GSM\"\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(919, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(923, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 929

   , &(interface.engine), 1, "AT+CMGF=1\r", strlen("AT+CMGF=1\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(933, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(937, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(943)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(944), UNITY_DISPLAY_STYLE_UINT8);







   frmCbackFlag = 0;



   gsmStartUp(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 956

   , &(interface.engine), 1, "AT\r", strlen("AT\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(960, &(interface.engine), ERR_CMD_ECHO);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(966)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(967), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspTest = {"ERROR", "abcde"};



   frmCbackFlag = 0;



   gsmStartUp(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 981

   , &(interface.engine), 1, "AT\r", strlen("AT\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(985, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(989, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   gsmGetCmdRsp_CMockExpectAndReturn(993, &(interface.engine), rspTest);



   interface.frm(&interface);



   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(997));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(998));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(999)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1000), UNITY_DISPLAY_STYLE_UINT8);



   return;

}

void test_gsmExitDataModeF(void)

{





   gsmInterface_t interface;







   frmCbackFlag = 0;







   gsmExitDataMode(&interface, frmCbackTest);



   interface.frm(&interface);





   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((INIT)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1031), UNITY_DISPLAY_STYLE_UINT8);



   interface.auxCnt = 0;



   gsm232UartSend_CMockExpectWithArrayAndReturn(

 1036

   , "+++", strlen("+++"), strlen("+++"), 3)

                                                           ;



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((1000)), (UNITY_INT)(UNITY_UINT32)((interface.auxCnt)), (((void *)0)), (UNITY_UINT)(1040), UNITY_DISPLAY_STYLE_UINT32);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((PROC)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1041), UNITY_DISPLAY_STYLE_UINT8);



   interface.frm(&interface);





   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((PROC)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1046), UNITY_DISPLAY_STYLE_UINT8);



   interface.auxCnt = 0;



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((WRAP)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1052), UNITY_DISPLAY_STYLE_UINT8);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1056)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1057), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGetSigQualF(void)

{





   gsmInterface_t interface;

   sigQual_t sigQualTest;







   frmCbackFlag = 0;











   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1094

   , &(interface.engine), 1, "AT+CSQ\r", strlen("AT+CSQ\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1098, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1102, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1106, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest1 = {"CSQ"," 2,1"};



   gsmGetCmdRsp_CMockExpectAndReturn(1112, &(interface.engine), rspTest1);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((-110)), (UNITY_INT)(UNITY_INT8 )((sigQualTest.rssi)), (((void *)0)), (UNITY_UINT)(1116), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((1)), (UNITY_INT)(UNITY_UINT8 )((sigQualTest.ber)), (((void *)0)), (UNITY_UINT)(1117), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1118)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1119), UNITY_DISPLAY_STYLE_UINT8);







   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1129

   , &(interface.engine), 1, "AT+CSQ\r", strlen("AT+CSQ\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1133, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1137, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1141, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest2 = {"CSQ"," 99,7"};



   gsmGetCmdRsp_CMockExpectAndReturn(1147, &(interface.engine), rspTest2);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((99)), (UNITY_INT)(UNITY_INT8 )((sigQualTest.rssi)), (((void *)0)), (UNITY_UINT)(1151), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((7)), (UNITY_INT)(UNITY_UINT8 )((sigQualTest.ber)), (((void *)0)), (UNITY_UINT)(1152), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1153)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1154), UNITY_DISPLAY_STYLE_UINT8);







   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1164

   , &(interface.engine), 1, "AT+CSQ\r", strlen("AT+CSQ\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1168, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1172, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1176, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest3 = {"CSQ"," 31,1"};



   gsmGetCmdRsp_CMockExpectAndReturn(1182, &(interface.engine), rspTest3);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((-52)), (UNITY_INT)(UNITY_INT8 )((sigQualTest.rssi)), (((void *)0)), (UNITY_UINT)(1186), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((1)), (UNITY_INT)(UNITY_UINT8 )((sigQualTest.ber)), (((void *)0)), (UNITY_UINT)(1187), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1188)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1189), UNITY_DISPLAY_STYLE_UINT8);







   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1199

   , &(interface.engine), 1, "AT+CSQ\r", strlen("AT+CSQ\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1203, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1207, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1211, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest4 = {"CSQ","abcde"};



   gsmGetCmdRsp_CMockExpectAndReturn(1217, &(interface.engine), rspTest4);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_WRAP)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1221), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((-52)), (UNITY_INT)(UNITY_INT8 )((sigQualTest.rssi)), (((void *)0)), (UNITY_UINT)(1222), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((1)), (UNITY_INT)(UNITY_UINT8 )((sigQualTest.ber)), (((void *)0)), (UNITY_UINT)(1223), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1224)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1225), UNITY_DISPLAY_STYLE_UINT8);







   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1235

   , &(interface.engine), 1, "AT+CSQ\r", strlen("AT+CSQ\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1239, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1243, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest5 = {"ERROR","params"};



   gsmGetCmdRsp_CMockExpectAndReturn(1249, &(interface.engine), rspTest5);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_GSM)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1253), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1254));

   UnityAssertEqualString((const char*)(("params")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1255));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1256)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1257), UNITY_DISPLAY_STYLE_UINT8);



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



 1296

   , &(interface.engine), 1, "AT+CREG?\r", strlen("AT+CREG?\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1300, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1304, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1308, &(interface.engine), OK_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(1309, &(interface.engine), rspGsmTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1315

   , &(interface.engine), 1, "AT+CGATT?\r", strlen("AT+CGATT?\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1319, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1323, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1327, &(interface.engine), OK_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(1328, &(interface.engine), rspGprsTest);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((connStateTest.gprs)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1334)));};

   if ((connStateTest.gsm)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1335)));};

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1336)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1337), UNITY_DISPLAY_STYLE_UINT8);







   gsmCheckConn(&interface, &connStateTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1347

   , &(interface.engine), 1, "AT+CREG?\r", strlen("AT+CREG?\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1351, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1355, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest5 = {"ERROR","params"};



   gsmGetCmdRsp_CMockExpectAndReturn(1361, &(interface.engine), rspTest5);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_GSM)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1365), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1366));

   UnityAssertEqualString((const char*)(("params")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1367));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1368)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1369), UNITY_DISPLAY_STYLE_UINT8);



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





 1410

   , &(interface.engine), 1, "AT+CMGS=\"+5491151751809\"\r", strlen("AT+CMGS=\"+5491151751809\"\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1414, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1418, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(

 1423

   , &(interface.engine), 1, "Hola mundo!", strlen("Hola mundo!"), OK_CMD_SENT)

                                                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1427, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1431, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1435, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmGetCmdRsp_CMockExpectAndReturn(1439, &(interface.engine), rspTest);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((5)), (UNITY_INT)(UNITY_UINT8 )((confTest.mr)), (((void *)0)), (UNITY_UINT)(1443), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1444)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1445), UNITY_DISPLAY_STYLE_UINT8);







   gsmSmsSend(&interface, &msgTest1, &confTest, frmCbackTest);



   interface.frm(&interface);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_INIT)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1455), UNITY_DISPLAY_STYLE_INT);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1456)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1457), UNITY_DISPLAY_STYLE_UINT8);







   gsmSmsSend(&interface, &msgTest2, &confTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1468

   , &(interface.engine), 1, "AT+CMGS=\"+5491151751809\"\r", strlen("AT+CMGS=\"+5491151751809\"\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1472, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1476, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest5 = {"ERROR","params"};



   gsmGetCmdRsp_CMockExpectAndReturn(1482, &(interface.engine), rspTest5);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_GSM)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1486), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1487));

   UnityAssertEqualString((const char*)(("params")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1488));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1489)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1490), UNITY_DISPLAY_STYLE_UINT8);



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



 1530

   , &(interface.engine), 1, "AT+CSDH=1\r", strlen("AT+CSDH=1\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1534, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1538, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1544

   , &(interface.engine), 1, "AT+CMGR=1,1\r", strlen("AT+CMGR=1,1\r"), OK_CMD_SENT)



                                                                            ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1548, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1552, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmGetCmdRsp_CMockExpectAndReturn(1556, &(interface.engine), rspTest1);

   gsmGetCmdRsp_CMockExpectAndReturn(1557, &(interface.engine), rspTest2);



   interface.frm(&interface);



   UnityAssertEqualString((const char*)(("meta")), (const char*)((msgTest.meta)), (((void *)0)), (UNITY_UINT)(1561));

   UnityAssertEqualString((const char*)(("text")), (const char*)((msgTest.text)), (((void *)0)), (UNITY_UINT)(1562));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1563)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1564), UNITY_DISPLAY_STYLE_UINT8);







   gsmSmsRead(&interface, &msgTest, &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(



 1574

   , &(interface.engine), 1, "AT+CSDH=1\r", strlen("AT+CSDH=1\r"), OK_CMD_SENT)



                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1578, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1582, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest5 = {"ERROR","params"};



   gsmGetCmdRsp_CMockExpectAndReturn(1588, &(interface.engine), rspTest5);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_GSM)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1592), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1593));

   UnityAssertEqualString((const char*)(("params")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1594));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1595)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1596), UNITY_DISPLAY_STYLE_UINT8);



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





 1641

   , &(interface.engine), 1, "AT+CSDH=1\r", strlen("AT+CSDH=1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1645, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1649, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1656

   , &(interface.engine), 1, "AT+CMGL=\"REC UNREAD\",1\r", strlen("AT+CMGL=\"REC UNREAD\",1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1660, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);











   gsmProcessTkn_CMockExpectAndReturn(1668, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);









   gsmGetNoCmdRsp_CMockExpectAndReturn(1675, &(interface.engine), 7);

   gsmGetCmdRsp_CMockExpectAndReturn(1676, &(interface.engine), rspTest1);

   gsmGetCmdRsp_CMockExpectAndReturn(1677, &(interface.engine), rspTest2);

   gsmGetCmdRsp_CMockExpectAndReturn(1678, &(interface.engine), rspTest3);

   gsmGetCmdRsp_CMockExpectAndReturn(1679, &(interface.engine), rspTest4);

   gsmGetCmdRsp_CMockExpectAndReturn(1680, &(interface.engine), rspTest5);

   gsmGetCmdRsp_CMockExpectAndReturn(1681, &(interface.engine), rspTest6);



   interface.frm(&interface);



   UnityAssertEqualString((const char*)(("meta1")), (const char*)((listTest[0].meta)), (((void *)0)), (UNITY_UINT)(1685));

   UnityAssertEqualString((const char*)(("text1")), (const char*)((listTest[0].text)), (((void *)0)), (UNITY_UINT)(1686));

   UnityAssertEqualString((const char*)(("meta2")), (const char*)((listTest[1].meta)), (((void *)0)), (UNITY_UINT)(1687));

   UnityAssertEqualString((const char*)(("text2")), (const char*)((listTest[1].text)), (((void *)0)), (UNITY_UINT)(1688));

   UnityAssertEqualString((const char*)(("meta3")), (const char*)((listTest[2].meta)), (((void *)0)), (UNITY_UINT)(1689));

   UnityAssertEqualString((const char*)(("text3")), (const char*)((listTest[2].text)), (((void *)0)), (UNITY_UINT)(1690));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1691)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1692), UNITY_DISPLAY_STYLE_UINT8);







   frmCbackFlag = 0;



   gsmSmsList(&interface, &listTest[0], &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1705

   , &(interface.engine), 1, "AT+CSDH=1\r", strlen("AT+CSDH=1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1709, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1713, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1720

   , &(interface.engine), 1, "AT+CMGL=\"REC UNREAD\",1\r", strlen("AT+CMGL=\"REC UNREAD\",1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1724, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);











   gsmProcessTkn_CMockExpectAndReturn(1732, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);













   gsmGetNoCmdRsp_CMockExpectAndReturn(1741, &(interface.engine), 9);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_WRAP)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1745), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1746)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1747), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspError = {"ERROR","abcde"};



   gsmSmsList(&interface, &listTest[0], &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1760

   , &(interface.engine), 1, "AT+CSDH=1\r", strlen("AT+CSDH=1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1764, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1768, &(interface.engine), ERR_MSG_CLOSE);

   gsmGetNoCmdRsp_CMockExpectAndReturn(1769, &(interface.engine), 1);

   gsmGetCmdRsp_CMockExpectAndReturn(1770, &(interface.engine), rspError);



   interface.frm(&interface);

   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_GSM)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1775), UNITY_DISPLAY_STYLE_UINT8);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1776));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1777));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1778)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1779), UNITY_DISPLAY_STYLE_UINT8);











   smsListPars_t parsTest2 = {REC_READ,NOCHANGE,3};



   gsmSmsList(&interface, &listTest[0], &parsTest2, frmCbackTest);



   interface.frm(&interface);



   interface.procState = ATCMD2;



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1796

   , &(interface.engine), 1, "AT+CMGL=\"REC READ\",1\r", strlen("AT+CMGL=\"REC READ\",1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);







   smsListPars_t parsTest3 = {STO_UNSENT,NOCHANGE,3};



   gsmSmsList(&interface, &listTest[0], &parsTest3, frmCbackTest);



   interface.frm(&interface);



   interface.procState = ATCMD2;



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1813

   , &(interface.engine), 1, "AT+CMGL=\"STO UNSENT\",1\r", strlen("AT+CMGL=\"STO UNSENT\",1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);







   smsListPars_t parsTest4 = {STO_SENT,NOCHANGE,3};



   gsmSmsList(&interface, &listTest[0], &parsTest4, frmCbackTest);



   interface.frm(&interface);



   interface.procState = ATCMD2;



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1830

   , &(interface.engine), 1, "AT+CMGL=\"STO SENT\",1\r", strlen("AT+CMGL=\"STO SENT\",1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);







   smsListPars_t parsTest5 = {ALL_MSG,NOCHANGE,3};



   gsmSmsList(&interface, &listTest[0], &parsTest5, frmCbackTest);



   interface.frm(&interface);



   interface.procState = ATCMD2;



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1847

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

 1888

   , &(interface.engine), 1, "AT+CMGD=1,0\r", strlen("AT+CMGD=1,0\r"), OK_CMD_SENT)

                                                                            ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1892, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1896, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1902)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1903), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspError = {"ERROR","abcde"};



   gsmSmsDel(&interface, &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(

 1914

   , &(interface.engine), 1, "AT+CMGD=1,0\r", strlen("AT+CMGD=1,0\r"), OK_CMD_SENT)

                                                                            ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1918, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1922, &(interface.engine), ERR_MSG_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(1923, &(interface.engine), rspError);



   interface.frm(&interface);

   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_GSM)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1928), UNITY_DISPLAY_STYLE_UINT8);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1929));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1930));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1931)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1932), UNITY_DISPLAY_STYLE_UINT8);



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





 1971

   , &(interface.engine), 1, "AT+CIPCLOSE\r", strlen("AT+CIPCLOSE\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1975, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1979, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 1986

   , &(interface.engine), 1, "AT+CIPSHUT\r", strlen("AT+CIPSHUT\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1990, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1994, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2001

   , &(interface.engine), 1, "AT+CIPMODE=1\r", strlen("AT+CIPMODE=1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2005, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2009, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(









 2018

   , &(interface.engine), 1, "AT+CSTT=\"apn\",\"user\"," "\"password\"\r", strlen("AT+CSTT=\"apn\",\"user\"," "\"password\"\r"), OK_CMD_SENT)









                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2022, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2026, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 2031

   , &(interface.engine), "AT+CIICR\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2035, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2039, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2046

   , &(interface.engine), 1, "AT+CIFSR\r", strlen("AT+CIFSR\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2050, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2054, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2060)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2061), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspError = {"ERROR","abcde"};



   gsmGprsStart(&interface, &apnTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2074

   , &(interface.engine), 1, "AT+CIPCLOSE\r", strlen("AT+CIPCLOSE\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2078, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2082, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2089

   , &(interface.engine), 1, "AT+CIPSHUT\r", strlen("AT+CIPSHUT\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2093, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2097, &(interface.engine), ERR_MSG_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(2098, &(interface.engine), rspError);



   interface.frm(&interface);

   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_GSM)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(2103), UNITY_DISPLAY_STYLE_UINT8);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(2104));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(2105));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2106)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2107), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGprsStopF(void)

{





   gsmInterface_t interface;







   frmCbackFlag = 0;











   gsmGprsStop(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2144

   , &(interface.engine), 1, "AT+CIPSHUT\r", strlen("AT+CIPSHUT\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2148, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2152, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2158)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2159), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspError = {"ERROR","abcde"};



   gsmGprsStop(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2172

   , &(interface.engine), 1, "AT+CIPSHUT\r", strlen("AT+CIPSHUT\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2176, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2180, &(interface.engine), ERR_MSG_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(2181, &(interface.engine), rspError);



   interface.frm(&interface);

   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_GSM)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(2186), UNITY_DISPLAY_STYLE_UINT8);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(2187));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(2188));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2189)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2190), UNITY_DISPLAY_STYLE_UINT8);



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





 2230

   , &(interface.engine), 1, "AT+CIPCLOSE\r", strlen("AT+CIPCLOSE\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2234, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2238, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(









 2247

   , &(interface.engine), 1, "AT+CIPSTART=\"TCP\",\"192.168.0.1" "\",\"80\"\r", strlen("AT+CIPSTART=\"TCP\",\"192.168" ".0.1\",\"80\"\r"), OK_CMD_SENT)









                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2251, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2255, &(interface.engine), OK_CLOSE);

   gsmGetNoCmdRsp_CMockExpectAndReturn(2256, &(interface.engine), 2);

   gsmGetCmdRsp_CMockExpectAndReturn(2257, &(interface.engine), rspTest1);

   gsmGetCmdRsp_CMockExpectAndReturn(2258, &(interface.engine), rspTest2);



   interface.frm(&interface);



   gsmSetSerialMode_CMockExpectAndReturn(2262, &(interface.engine), DATA_MODE, 1);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2266)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2267), UNITY_DISPLAY_STYLE_UINT8);







   socket_t socketTest2 = {UDP, "192.168.0.1", 80};



   gsmGprsOpenPort(&interface, &socketTest2, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2280

   , &(interface.engine), 1, "AT+CIPCLOSE\r", strlen("AT+CIPCLOSE\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2284, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2288, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(









 2297

   , &(interface.engine), 1, "AT+CIPSTART=\"UDP\",\"192.168.0.1" "\",\"80\"\r", strlen("AT+CIPSTART=\"UDP\",\"192.168" ".0.1\",\"80\"\r"), OK_CMD_SENT)









                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2301, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2305, &(interface.engine), OK_CLOSE);

   gsmGetNoCmdRsp_CMockExpectAndReturn(2306, &(interface.engine), 2);

   gsmGetCmdRsp_CMockExpectAndReturn(2307, &(interface.engine), rspTest1);

   gsmGetCmdRsp_CMockExpectAndReturn(2308, &(interface.engine), rspTest2);



   interface.frm(&interface);



   gsmSetSerialMode_CMockExpectAndReturn(2312, &(interface.engine), DATA_MODE, 1);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2316)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2317), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspTest3 = {"CONNECT FAIL",""};



   gsmGprsOpenPort(&interface, &socketTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2330

   , &(interface.engine), 1, "AT+CIPCLOSE\r", strlen("AT+CIPCLOSE\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2334, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2338, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(









 2347

   , &(interface.engine), 1, "AT+CIPSTART=\"TCP\",\"192.168.0.1" "\",\"80\"\r", strlen("AT+CIPSTART=\"TCP\",\"192.168" ".0.1\",\"80\"\r"), OK_CMD_SENT)









                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2351, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2355, &(interface.engine), OK_CLOSE);

   gsmGetNoCmdRsp_CMockExpectAndReturn(2356, &(interface.engine), 2);

   gsmGetCmdRsp_CMockExpectAndReturn(2357, &(interface.engine), rspTest1);

   gsmGetCmdRsp_CMockExpectAndReturn(2358, &(interface.engine), rspTest3);



   interface.frm(&interface);

   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_PROC)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(2363), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2364)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2365), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspError = {"ERROR","abcde"};



   gsmGprsOpenPort(&interface, &socketTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2378

   , &(interface.engine), 1, "AT+CIPCLOSE\r", strlen("AT+CIPCLOSE\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2382, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2386, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(









 2395

   , &(interface.engine), 1, "AT+CIPSTART=\"TCP\",\"192.168.0.1" "\",\"80\"\r", strlen("AT+CIPSTART=\"TCP\",\"192.168" ".0.1\",\"80\"\r"), OK_CMD_SENT)









                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2399, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2403, &(interface.engine), ERR_MSG_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(2404, &(interface.engine), rspError);



   interface.frm(&interface);

   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_GSM)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(2409), UNITY_DISPLAY_STYLE_UINT8);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(2410));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(2411));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2412)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2413), UNITY_DISPLAY_STYLE_UINT8);





}

void test_gsmGprsClosePortF(void)

{





   gsmInterface_t interface;







   frmCbackFlag = 0;











   gsmGprsClosePort(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2451

   , &(interface.engine), 1, "AT+CIPCLOSE=0\r", strlen("AT+CIPCLOSE=0\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2455, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2459, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSetSerialMode_CMockExpectAndReturn(2463, &(interface.engine), COMMAND_MODE, 1);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2467)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2468), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspError = {"ERROR","abcde"};



   gsmGprsClosePort(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2481

   , &(interface.engine), 1, "AT+CIPCLOSE=0\r", strlen("AT+CIPCLOSE=0\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2485, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2489, &(interface.engine), ERR_MSG_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(2490, &(interface.engine), rspError);



   interface.frm(&interface);

   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_GSM)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(2495), UNITY_DISPLAY_STYLE_UINT8);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(2496));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(2497));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2498)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2499), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGnssPwrF(void)

{





   gsmInterface_t interface;

   pwrGnss_t cmdTest;







   frmCbackFlag = 0;











   cmdTest = ON;



   gsmGnssPwr(&interface, &cmdTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2539

   , &(interface.engine), 1, "AT+CGNSPWR=1\r", strlen("AT+CGNSPWR=1\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2543, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2547, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2553)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2554), UNITY_DISPLAY_STYLE_UINT8);







   cmdTest = OFF;



   gsmGnssPwr(&interface, &cmdTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2567

   , &(interface.engine), 1, "AT+CGNSPWR=0\r", strlen("AT+CGNSPWR=0\r"), OK_CMD_SENT)





                                                   ;





   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2572, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2576, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2582)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2583), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspError = {"ERROR","abcde"};



   cmdTest = OFF;



   gsmGnssPwr(&interface, &cmdTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2598

   , &(interface.engine), 1, "AT+CGNSPWR=0\r", strlen("AT+CGNSPWR=0\r"), OK_CMD_SENT)





                                                   ;





   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2603, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2607, &(interface.engine), ERR_MSG_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(2608, &(interface.engine), rspError);



   interface.frm(&interface);

   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_GSM)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(2613), UNITY_DISPLAY_STYLE_UINT8);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(2614));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(2615));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2616)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2617), UNITY_DISPLAY_STYLE_UINT8);







   cmdTest = OFF;



   gsmGnssPwr(&interface, &cmdTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2630

   , &(interface.engine), 1, "AT+CGNSPWR=0\r", strlen("AT+CGNSPWR=0\r"), OK_CMD_SENT)





                                                   ;





   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2635, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2639, &(interface.engine), ERR_TIMEOUT);



   interface.frm(&interface);

   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_TOUT)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(2644), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2645)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2646), UNITY_DISPLAY_STYLE_UINT8);







   cmdTest = OFF;



   gsmGnssPwr(&interface, &cmdTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2659

   , &(interface.engine), 1, "AT+CGNSPWR=0\r", strlen("AT+CGNSPWR=0\r"), OK_CMD_SENT)





                                                   ;





   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2664, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2668, &(interface.engine), ERR_FSM_OOR);



   interface.frm(&interface);

   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_FSM)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(2673), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2674)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2675), UNITY_DISPLAY_STYLE_UINT8);







   cmdTest = OFF;



   gsmGnssPwr(&interface, &cmdTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2688

   , &(interface.engine), 1, "AT+CGNSPWR=0\r", strlen("AT+CGNSPWR=0\r"), OK_CMD_SENT)





                                                   ;





   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2693, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2697, &(interface.engine), ERR_FSM_OOR+1);



   interface.frm(&interface);

   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_UNK)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(2702), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2703)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2704), UNITY_DISPLAY_STYLE_UINT8);







}

void test_gsmGnssGetDataF(void)

{





   gsmInterface_t interface;

   dataGnss_t dataGnssTest;

   rsp_t rspTest = {"CGNSINF","data"};







   frmCbackFlag = 0;







   gsmGnssGetData(&interface, &dataGnssTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2743

   , &(interface.engine), 1, "AT+CGNSINF\r", strlen("AT+CGNSINF\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2747, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2751, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmGetCmdRsp_CMockExpectAndReturn(2755, &(interface.engine), rspTest);



   interface.frm(&interface);



   UnityAssertEqualString((const char*)(("data")), (const char*)((dataGnssTest.data)), (((void *)0)), (UNITY_UINT)(2759));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2760)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2761), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspError = {"ERROR","abcde"};



   gsmGnssGetData(&interface, &dataGnssTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectWithArrayAndReturn(





 2774

   , &(interface.engine), 1, "AT+CGNSINF\r", strlen("AT+CGNSINF\r"), OK_CMD_SENT)





                                                   ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2778, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2782, &(interface.engine), ERR_MSG_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(2783, &(interface.engine), rspError);



   interface.frm(&interface);

   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_GSM)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(2788), UNITY_DISPLAY_STYLE_UINT8);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(2789));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(2790));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2791)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2792), UNITY_DISPLAY_STYLE_UINT8);



}
