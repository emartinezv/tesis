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

   UnityAssertEqualString((const char*)(("CMTI")), (const char*)((cmd)), (((void *)0)), (UNITY_UINT)(47));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((par)), (((void *)0)), (UNITY_UINT)(48));



   return;

}



void dataCbackTest (gsmInterface_t * interface)

{

   dataCbackFlag = 1;



   return;

}



void * frmCbackTest (errorUser_s error, void * output)

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







   gsmInitEngine_CMockExpectAndReturn(107, &(interface.engine), TRUE);







   result = gsmInitInterface(&interface);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(113)));};



   if ((IDLE == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(115)));};

   if ((NOCMD == interface.procState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(116)));};



   if ((10 == interface.procCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(118)));};

   if ((0 == interface.auxCnt)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(119)));};



   if ((MANUAL_MODE == interface.urcMode)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(121)));};

   if ((((void *)0) == interface.urcCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(122)));};



   if ((((void *)0) == interface.dataCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(124)));};

   UnityAssertEqualString((const char*)(("OK")), (const char*)((interface.exitCmdList[0])), (((void *)0)), (UNITY_UINT)(125));

   UnityAssertEqualString((const char*)(("CLOSED")), (const char*)((interface.exitCmdList[1])), (((void *)0)), (UNITY_UINT)(126));



   if ((((void *)0) == interface.frm)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(128)));};

   if ((((void *)0) == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(129)));};

   if ((((void *)0) == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(130)));};

   if ((((void *)0) == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(131)));};

   if ((OK == interface.errorOut.errorFrm)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(132)));};



}

void test_gsmStartUp(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;







   gsmStartUp(&interface, cbackTest);



   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(155)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(156)));};



}

void test_gsmExitDataMode(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;







   gsmExitDataMode(&interface, cbackTest);



   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(179)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(180)));};



}

void test_gsmSysTickHandler(void)

{





   gsmInterface_t interface;







   gsmInitEngine_CMockExpectAndReturn(200, &(interface.engine), TRUE);

   gsmDecToutCnt_CMockExpect(201, &(interface.engine));

   gsmDecToutCnt_CMockExpect(202, &(interface.engine));







   gsmInitInterface(&interface);



   interface.procCnt = 100;

   interface.auxCnt = 100;



   gsmSysTickHandler();



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((99)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(213), UNITY_DISPLAY_STYLE_UINT32);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((99)), (UNITY_INT)(UNITY_UINT32)((interface.auxCnt)), (((void *)0)), (UNITY_UINT)(214), UNITY_DISPLAY_STYLE_UINT32);



   interface.procCnt = 0;

   interface.auxCnt = 0;



   gsmSysTickHandler();



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((0)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(221), UNITY_DISPLAY_STYLE_UINT32);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((0)), (UNITY_INT)(UNITY_UINT32)((interface.auxCnt)), (((void *)0)), (UNITY_UINT)(222), UNITY_DISPLAY_STYLE_UINT32);



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



   gsmGetSerialMode_CMockExpectAndReturn(269, &(interface.engine), COMMAND_MODE);

   gsmProcessTkn_CMockExpectAndReturn(270, &(interface.engine), NO_UPDATE);

   gsmGetUrc_CMockExpectAndReturn(271, &(interface.engine), rspTest);



   gsmProcess(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((10)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(275), UNITY_DISPLAY_STYLE_UINT32);







   interface.procCnt = 0;



   gsmGetSerialMode_CMockExpectAndReturn(281, &(interface.engine), DATA_MODE);

   gsmGetSerialMode_CMockExpectAndReturn(282, &(interface.engine), DATA_MODE);



   gsmProcess(&interface);



   if ((dataCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(286)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((10)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(287), UNITY_DISPLAY_STYLE_UINT32);



   return;







   interface.procCnt = 0;

   interface.frmState = INIT;



   gsmProcess(&interface);



   if ((frmFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(298)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((10)), (UNITY_INT)(UNITY_UINT32)((interface.procCnt)), (((void *)0)), (UNITY_UINT)(299), UNITY_DISPLAY_STYLE_UINT32);



   return;



}

void test_gsmIsIdle(void)

{





   gsmInterface_t interface;

   _Bool result;







   interface.frmState = IDLE;



   result = gsmIsIdle(&interface);



   if ((result)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(326)));};



   interface.frmState = INIT;



   result = gsmIsIdle(&interface);



   if (!(result)) {} else {UnityFail( ((" Expected FALSE Was TRUE")), (UNITY_UINT)((UNITY_UINT)(332)));};



}

void test_gsmGetSigQual(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   sigQual_s sigQualTest;







   gsmGetSigQual(&interface, &sigQualTest, cbackTest);



   if ((&sigQualTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(356)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(357)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(358)));};



}

void test_gsmCheckConn(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   connStatus_s connStateTest;







   gsmCheckConn(&interface, &connStateTest, cbackTest);



   if ((&connStateTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(382)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(383)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(384)));};



}

void test_gsmSetUrcMode(void)

{





   gsmInterface_t interface;







   gsmSetUrcMode(&interface, MANUAL_MODE);



   if ((MANUAL_MODE == interface.urcMode)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(406)));};



   gsmSetUrcMode(&interface, CBACK_MODE);



   if ((CBACK_MODE == interface.urcMode)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(410)));};



}

void test_gsmSetUrcCback(void)

{





   gsmInterface_t interface;







   gsmSetUrcCback(&interface, urcCbackTest);



   if ((urcCbackTest == interface.urcCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(432)));};



}

void test_gsmSetDataCback(void)

{





   gsmInterface_t interface;







   gsmSetDataCback(&interface, dataCbackTest);



   if ((dataCbackTest == interface.dataCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(454)));};



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



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((nchInput)), (UNITY_INT)(UNITY_UINT8 )((nchTest)), (((void *)0)), (UNITY_UINT)(490), UNITY_DISPLAY_STYLE_UINT8);







   nchInput = strlen(testBuf2);



   gsmSetSerialMode_CMockExpectAndReturn(496, &(interface.engine), COMMAND_MODE, 1);



   nchTest = gsmCheckDataMode(&interface, testBuf2, &nchInput);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((15)), (UNITY_INT)(UNITY_UINT8 )((nchTest)), (((void *)0)), (UNITY_UINT)(500), UNITY_DISPLAY_STYLE_UINT8);







   nchInput = strlen(testBuf3);



   gsmSetSerialMode_CMockExpectAndReturn(506, &(interface.engine), COMMAND_MODE, 1);



   nchTest = gsmCheckDataMode(&interface, testBuf3, &nchInput);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((13)), (UNITY_INT)(UNITY_UINT8 )((nchTest)), (((void *)0)), (UNITY_UINT)(510), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmSmsSend(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   smsOut_s msgTest;

   smsConf_s confTest;







   gsmSmsSend(&interface, &msgTest, &confTest, cbackTest);



   if ((&msgTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(535)));};

   if ((&confTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(536)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(537)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(538)));};



}

void test_gsmSmsRead(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   smsRec_s msgTest;

   smsReadPars_s parsTest;







   gsmSmsRead(&interface, &msgTest, &parsTest, cbackTest);



   if ((&parsTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(563)));};

   if ((&msgTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(564)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(565)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(566)));};



}

void test_gsmSmsList(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   smsRec_s listTest;

   smsListPars_s parsTest;







   gsmSmsList(&interface, &listTest, &parsTest, cbackTest);



   if ((&parsTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(591)));};

   if ((&listTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(592)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(593)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(594)));};



}

void test_gsmSmsDel(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   smsDelPars_s msgdelTest;







   gsmSmsDel(&interface, &msgdelTest, cbackTest);



   if ((&msgdelTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(618)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(619)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(620)));};



}

void test_gsmGprsStart(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   apnUserPwd_s apnTest;







   gsmGprsStart(&interface, &apnTest, cbackTest);



   if ((&apnTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(644)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(645)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(646)));};



}

void test_gsmGprsStop(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;







   gsmGprsStop(&interface, cbackTest);



   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(669)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(670)));};



}

void test_gsmGprsOpenPort(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   socket_s portTest;







   gsmGprsOpenPort(&interface, &portTest, cbackTest);



   if ((&portTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(694)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(695)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(696)));};



}

void test_gsmGprsClosePort(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;







   gsmGprsClosePort(&interface, cbackTest);



   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(719)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(720)));};



}

void test_gsmGnssPwr(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   pwrGnss_e cmdTest;







   gsmGnssPwr(&interface, &cmdTest, cbackTest);



   if ((&cmdTest == interface.frmInput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(744)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(745)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(746)));};



}

void test_gsmGnssGetData(void)

{





   gsmInterface_t interface;

   frmCback_t cbackTest;

   dataGnss_s dataGnssTest;







   gsmGnssGetData(&interface, &dataGnssTest, cbackTest);



   if ((&dataGnssTest == interface.frmOutput)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(770)));};

   if ((cbackTest == interface.frmCback)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(771)));};

   if ((INIT == interface.frmState)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(772)));};



}

void test_gsmStartUpF(void)

{





   gsmInterface_t interface;







   interface.frmCback = frmCbackTest;

   frmFlag = 0;











   gsmStartUp(&interface, frmCbackTest);



   interface.frm(&interface);



   if ((OK == interface.errorOut.errorFrm)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(808)));};

   if (('\0' == interface.errorOut.errorCmd.cmd[0])) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(809)));};

   if (('\0' == interface.errorOut.errorCmd.par[0])) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(810)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ATCMD1)), (UNITY_INT)(UNITY_UINT8 )((interface.procState)), (((void *)0)), (UNITY_UINT)(811), UNITY_DISPLAY_STYLE_UINT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((PROC)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(812), UNITY_DISPLAY_STYLE_UINT8);



   gsmSendCmd_CMockExpectAndReturn(814, &(interface.engine), "AT\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(818, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(822, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(826, &(interface.engine), "AT+CMEE=2\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(830, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(834, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 839

   , &(interface.engine), "AT+CSCS=\"GSM\"\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(843, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(847, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 852

   , &(interface.engine), "AT+CMGF=1\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(856, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(860, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(866)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(867), UNITY_DISPLAY_STYLE_UINT8);







   frmCbackFlag = 0;



   gsmStartUp(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(877, &(interface.engine), "AT\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(881, &(interface.engine), ERR_CMD_ECHO);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(887)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(888), UNITY_DISPLAY_STYLE_UINT8);







   rsp_t rspTest;



   strncpy(rspTest.cmd, "ERROR", strlen("ERROR"));

   rspTest.cmd[strlen("ERROR")] = '\0';

   strncpy(rspTest.par, "abcde", strlen("abcde"));

   rspTest.par[strlen("abcde")] = '\0';



   frmCbackFlag = 0;



   gsmStartUp(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(905, &(interface.engine), "AT\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(909, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(913, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   gsmGetCmdRsp_CMockExpectAndReturn(917, &(interface.engine), rspTest);



   interface.frm(&interface);



   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(921));

   UnityAssertEqualString((const char*)(("abcde")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(922));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(923)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(924), UNITY_DISPLAY_STYLE_UINT8);



   return;

}

void test_gsmExitDataModeF(void)

{





   gsmInterface_t interface;







   frmCbackFlag = 0;







   gsmExitDataMode(&interface, frmCbackTest);



   interface.frm(&interface);





   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((INIT)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(955), UNITY_DISPLAY_STYLE_UINT8);



   interface.auxCnt = 0;



   gsm232UartSend_CMockExpectAndReturn(959, "+++", strlen("+++"), 3);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((1000)), (UNITY_INT)(UNITY_UINT32)((interface.auxCnt)), (((void *)0)), (UNITY_UINT)(963), UNITY_DISPLAY_STYLE_UINT32);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((PROC)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(964), UNITY_DISPLAY_STYLE_UINT8);



   interface.frm(&interface);





   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((PROC)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(969), UNITY_DISPLAY_STYLE_UINT8);



   interface.auxCnt = 0;



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((WRAP)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(975), UNITY_DISPLAY_STYLE_UINT8);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(979)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(980), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGetSigQualF(void)

{





   gsmInterface_t interface;

   sigQual_s sigQualTest;









   frmCbackFlag = 0;











   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1016, &(interface.engine), "AT+CSQ\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1020, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1024, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1028, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest1 = {"CSQ"," 2,1"};



   gsmGetCmdRsp_CMockExpectAndReturn(1034, &(interface.engine), rspTest1);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((-110)), (UNITY_INT)(UNITY_INT8 )((sigQualTest.rssi)), (((void *)0)), (UNITY_UINT)(1038), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((1)), (UNITY_INT)(UNITY_UINT8 )((sigQualTest.ber)), (((void *)0)), (UNITY_UINT)(1039), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1040)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1041), UNITY_DISPLAY_STYLE_UINT8);







   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1049, &(interface.engine), "AT+CSQ\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1053, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1057, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1061, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest2 = {"CSQ"," 99,7"};



   gsmGetCmdRsp_CMockExpectAndReturn(1067, &(interface.engine), rspTest2);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((99)), (UNITY_INT)(UNITY_INT8 )((sigQualTest.rssi)), (((void *)0)), (UNITY_UINT)(1071), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((7)), (UNITY_INT)(UNITY_UINT8 )((sigQualTest.ber)), (((void *)0)), (UNITY_UINT)(1072), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1073)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1074), UNITY_DISPLAY_STYLE_UINT8);







   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1082, &(interface.engine), "AT+CSQ\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1086, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1090, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1094, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest3 = {"CSQ"," 31,1"};



   gsmGetCmdRsp_CMockExpectAndReturn(1100, &(interface.engine), rspTest3);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((-52)), (UNITY_INT)(UNITY_INT8 )((sigQualTest.rssi)), (((void *)0)), (UNITY_UINT)(1104), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((1)), (UNITY_INT)(UNITY_UINT8 )((sigQualTest.ber)), (((void *)0)), (UNITY_UINT)(1105), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1106)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1107), UNITY_DISPLAY_STYLE_UINT8);







   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1115, &(interface.engine), "AT+CSQ\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1119, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1123, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1127, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest4 = {"CSQ","abcde"};



   gsmGetCmdRsp_CMockExpectAndReturn(1133, &(interface.engine), rspTest4);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_WRAP)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1137), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((-52)), (UNITY_INT)(UNITY_INT8 )((sigQualTest.rssi)), (((void *)0)), (UNITY_UINT)(1138), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((1)), (UNITY_INT)(UNITY_UINT8 )((sigQualTest.ber)), (((void *)0)), (UNITY_UINT)(1139), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1140)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1141), UNITY_DISPLAY_STYLE_UINT8);







   gsmGetSigQual(&interface, &sigQualTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1149, &(interface.engine), "AT+CSQ\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1153, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1157, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest5 = {"ERROR","params"};



   gsmGetCmdRsp_CMockExpectAndReturn(1163, &(interface.engine), rspTest5);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_GSM)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1167), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1168));

   UnityAssertEqualString((const char*)(("params")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1169));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1170)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1171), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmCheckConnF(void)

{





   gsmInterface_t interface;

   connStatus_s connStateTest;

   rsp_t rspGsmTest;

   rsp_t rspGprsTest;







   frmCbackFlag = 0;



   strncpy(rspGsmTest.cmd, "CREG", strlen("CREG"));

   rspGsmTest.cmd[strlen("CREG")] = '\0';

   strncpy(rspGsmTest.par, " 0,1", strlen(" 1,0"));

   rspGsmTest.par[strlen(" 0,1")] = '\0';



   strncpy(rspGprsTest.cmd, "CGATT", strlen("CGATT"));

   rspGprsTest.cmd[strlen("CGATT")] = '\0';

   strncpy(rspGprsTest.par, " 1", strlen(" 1"));

   rspGprsTest.par[strlen(" 1")] = '\0';











   gsmCheckConn(&interface, &connStateTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1218, &(interface.engine), "AT+CREG?\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1222, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1226, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1230, &(interface.engine), OK_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(1231, &(interface.engine), rspGsmTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1235, &(interface.engine), "AT+CGATT?\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1239, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1243, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1247, &(interface.engine), OK_CLOSE);

   gsmGetCmdRsp_CMockExpectAndReturn(1248, &(interface.engine), rspGprsTest);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((connStateTest.gprs)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1254)));};

   if ((connStateTest.gsm)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1255)));};

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1256)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1257), UNITY_DISPLAY_STYLE_UINT8);







   gsmCheckConn(&interface, &connStateTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1265, &(interface.engine), "AT+CREG?\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1269, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1273, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest5 = {"ERROR","params"};



   gsmGetCmdRsp_CMockExpectAndReturn(1279, &(interface.engine), rspTest5);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_GSM)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1283), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1284));

   UnityAssertEqualString((const char*)(("params")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1285));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1286)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1287), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmSmsSendF(void)

{





   gsmInterface_t interface;

   smsOut_s msgTest1 = {"+5491151751809","Hola mundo!\r\n"};

   smsOut_s msgTest2 = {"+5491151751809","Hola mundo!"};

   smsConf_s confTest;

   rsp_t rspTest1 = {"OK",""};

   rsp_t rspTest2 = {"CMGS"," 5"};







   frmCbackFlag = 0;











   gsmSmsSend(&interface, &msgTest2, &confTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1327

   , &(interface.engine), "AT+CMGS=\"+5491151751809\"\r", OK_CMD_SENT)

                                                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1331, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1335, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1340

   , &(interface.engine), "Hola mundo!", OK_CMD_SENT)

                                                         ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1344, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1348, &(interface.engine), OK_RSP);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1352, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmGetCmdRsp_CMockExpectAndReturn(1356, &(interface.engine), rspTest1);

   gsmGetCmdRsp_CMockExpectAndReturn(1357, &(interface.engine), rspTest2);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((5)), (UNITY_INT)(UNITY_UINT8 )((confTest.mr)), (((void *)0)), (UNITY_UINT)(1361), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1362)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1363), UNITY_DISPLAY_STYLE_UINT8);







   gsmSmsSend(&interface, &msgTest1, &confTest, frmCbackTest);



   interface.frm(&interface);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_INIT)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1373), UNITY_DISPLAY_STYLE_INT);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1374)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1375), UNITY_DISPLAY_STYLE_UINT8);







   gsmSmsSend(&interface, &msgTest2, &confTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1384

   , &(interface.engine), "AT+CMGS=\"+5491151751809\"\r", OK_CMD_SENT)

                                                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1388, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1392, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest5 = {"ERROR","params"};



   gsmGetCmdRsp_CMockExpectAndReturn(1398, &(interface.engine), rspTest5);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_GSM)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1402), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1403));

   UnityAssertEqualString((const char*)(("params")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1404));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1405)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1406), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmSmsReadF(void)

{





   gsmInterface_t interface;

   smsRec_s msgTest;

   smsReadPars_s parsTest = {1,NOCHANGE};

   rsp_t rspTest1 = {"CMGR","meta"};

   rsp_t rspTest2 = {"SMS_BODY","text"};







   frmCbackFlag = 0;











   gsmSmsRead(&interface, &msgTest, &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1444, &(interface.engine), "AT+CSDH=1\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1448, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1452, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1457

   , &(interface.engine), "AT+CMGR=1,1\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1461, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1465, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmGetCmdRsp_CMockExpectAndReturn(1469, &(interface.engine), rspTest1);

   gsmGetCmdRsp_CMockExpectAndReturn(1470, &(interface.engine), rspTest2);



   interface.frm(&interface);



   UnityAssertEqualString((const char*)(("meta")), (const char*)((msgTest.meta)), (((void *)0)), (UNITY_UINT)(1474));

   UnityAssertEqualString((const char*)(("text")), (const char*)((msgTest.text)), (((void *)0)), (UNITY_UINT)(1475));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1476)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1477), UNITY_DISPLAY_STYLE_UINT8);







   gsmSmsRead(&interface, &msgTest, &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1485, &(interface.engine), "AT+CSDH=1\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1489, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1493, &(interface.engine), ERR_MSG_CLOSE);



   interface.frm(&interface);



   rsp_t rspTest5 = {"ERROR","params"};



   gsmGetCmdRsp_CMockExpectAndReturn(1499, &(interface.engine), rspTest5);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)((ERR_GSM)), (UNITY_INT)((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1503), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualString((const char*)(("ERROR")), (const char*)((interface.errorOut.errorCmd.cmd)), (((void *)0)), (UNITY_UINT)(1504));

   UnityAssertEqualString((const char*)(("params")), (const char*)((interface.errorOut.errorCmd.par)), (((void *)0)), (UNITY_UINT)(1505));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1506)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1507), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmSmsListF(void)

{





   gsmInterface_t interface;

   smsRec_s listTest[3];

   smsListPars_s parsTest = {REC_UNREAD,NOCHANGE,3};

   rsp_t rspTest1 = {"CMGR","meta1"};

   rsp_t rspTest2 = {"SMS_BODY","text1"};

   rsp_t rspTest3 = {"CMGR","meta2"};

   rsp_t rspTest4 = {"SMS_BODY","text2"};

   rsp_t rspTest5 = {"CMGR","meta3"};

   rsp_t rspTest6 = {"SMS_BODY","text3"};







   frmCbackFlag = 0;











   gsmSmsList(&interface, &listTest[0], &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1549, &(interface.engine), "AT+CSDH=1\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1553, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1557, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1562

   , &(interface.engine), "AT+CMGL=\"REC_UNREAD\",1\r", OK_CMD_SENT)

                                                                        ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1566, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);











   gsmProcessTkn_CMockExpectAndReturn(1574, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);









   gsmGetNoCmdRsp_CMockExpectAndReturn(1581, &(interface.engine), 7);

   gsmGetCmdRsp_CMockExpectAndReturn(1582, &(interface.engine), rspTest1);

   gsmGetCmdRsp_CMockExpectAndReturn(1583, &(interface.engine), rspTest2);

   gsmGetCmdRsp_CMockExpectAndReturn(1584, &(interface.engine), rspTest3);

   gsmGetCmdRsp_CMockExpectAndReturn(1585, &(interface.engine), rspTest4);

   gsmGetCmdRsp_CMockExpectAndReturn(1586, &(interface.engine), rspTest5);

   gsmGetCmdRsp_CMockExpectAndReturn(1587, &(interface.engine), rspTest6);



   interface.frm(&interface);



   UnityAssertEqualString((const char*)(("meta1")), (const char*)((listTest[0].meta)), (((void *)0)), (UNITY_UINT)(1591));

   UnityAssertEqualString((const char*)(("text1")), (const char*)((listTest[0].text)), (((void *)0)), (UNITY_UINT)(1592));

   UnityAssertEqualString((const char*)(("meta2")), (const char*)((listTest[1].meta)), (((void *)0)), (UNITY_UINT)(1593));

   UnityAssertEqualString((const char*)(("text2")), (const char*)((listTest[1].text)), (((void *)0)), (UNITY_UINT)(1594));

   UnityAssertEqualString((const char*)(("meta3")), (const char*)((listTest[2].meta)), (((void *)0)), (UNITY_UINT)(1595));

   UnityAssertEqualString((const char*)(("text3")), (const char*)((listTest[2].text)), (((void *)0)), (UNITY_UINT)(1596));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1597)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1598), UNITY_DISPLAY_STYLE_UINT8);







   frmCbackFlag = 0;



   gsmSmsList(&interface, &listTest[0], &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(1608, &(interface.engine), "AT+CSDH=1\r", OK_CMD_SENT);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1612, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1616, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1621

   , &(interface.engine), "AT+CMGL=\"REC_UNREAD\",1\r", OK_CMD_SENT)

                                                                        ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1625, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);











   gsmProcessTkn_CMockExpectAndReturn(1633, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);













   gsmGetNoCmdRsp_CMockExpectAndReturn(1642, &(interface.engine), 9);



   interface.frm(&interface);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((ERR_WRAP)), (UNITY_INT)(UNITY_UINT8 )((interface.errorOut.errorFrm)), (((void *)0)), (UNITY_UINT)(1646), UNITY_DISPLAY_STYLE_UINT8);

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1647)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1648), UNITY_DISPLAY_STYLE_UINT8);







   smsListPars_s parsTest2 = {REC_READ,NOCHANGE,3};



   gsmSmsList(&interface, &listTest[0], &parsTest2, frmCbackTest);



   interface.frm(&interface);



   interface.procState = ATCMD2;

}

void test_gsmSmsDelF(void)

{





   gsmInterface_t interface;

   smsDelPars_s parsTest = {1,INDEX};

   rsp_t rspTest1 = {"cmd","par"};







   frmCbackFlag = 0;











   gsmSmsDel(&interface, &parsTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1702

   , &(interface.engine), "AT+CMGD=1,0\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1706, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1710, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1716)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1717), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGprsStartF(void)

{





   gsmInterface_t interface;

   apnUserPwd_s apnTest = {"apn","user","password"};

   rsp_t rspTest1 = {"cmd","par"};







   frmCbackFlag = 0;











   gsmGprsStart(&interface, &apnTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1754

   , &(interface.engine), "AT+CIPCLOSE\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1758, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1762, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1767

   , &(interface.engine), "AT+CIPSHUT\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1771, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1775, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1780

   , &(interface.engine), "AT+CIPMODE=1\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1784, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1788, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(



 1794

   , &(interface.engine), "AT+CSTT=\"apn\",\"user\",\"password\"\r", OK_CMD_SENT)



                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1798, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1802, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1807

   , &(interface.engine), "AT+CIICR\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1811, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1815, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1820

   , &(interface.engine), "AT+CIFSR\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1824, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1828, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1834)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1835), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGprsStopF(void)

{





   gsmInterface_t interface;







   frmCbackFlag = 0;











   gsmGprsStop(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1870

   , &(interface.engine), "AT+CIPSHUT\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1874, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1878, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1884)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1885), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGprsOpenPortF(void)

{





   gsmInterface_t interface;

   socket_s socketTest = {TCP, "192.168.0.1", 80};

   rsp_t rspTest1 = {"OK",""};

   rsp_t rspTest2 = {"CONNECT OK",""};







   frmCbackFlag = 0;











   gsmGprsOpenPort(&interface, &socketTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1923

   , &(interface.engine), "AT+CIPCLOSE\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1927, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1931, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(



 1937

   , &(interface.engine), "AT+CIPSTART=\"TCP\",\"192.168.0.1\",\"80\"\r", OK_CMD_SENT)



                                         ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1941, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1945, &(interface.engine), OK_CLOSE);

   gsmGetNoCmdRsp_CMockExpectAndReturn(1946, &(interface.engine), 2);

   gsmGetCmdRsp_CMockExpectAndReturn(1947, &(interface.engine), rspTest1);

   gsmGetCmdRsp_CMockExpectAndReturn(1948, &(interface.engine), rspTest2);



   interface.frm(&interface);



   gsmSetSerialMode_CMockExpectAndReturn(1952, &(interface.engine), DATA_MODE, 1);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(1956)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(1957), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGprsClosePortF(void)

{





   gsmInterface_t interface;







   frmCbackFlag = 0;











   gsmGprsClosePort(&interface, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 1992

   , &(interface.engine), "AT+CIPCLOSE=0\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(1996, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2000, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmSetSerialMode_CMockExpectAndReturn(2004, &(interface.engine), COMMAND_MODE, 1);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2008)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2009), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGnssPwrF(void)

{





   gsmInterface_t interface;

   pwrGnss_e cmdTest;







   frmCbackFlag = 0;











   cmdTest = ON;



   gsmGnssPwr(&interface, &cmdTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 2047

   , &(interface.engine), "AT+CGNSPWR=1\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2051, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2055, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2061)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2062), UNITY_DISPLAY_STYLE_UINT8);







   cmdTest = OFF;



   gsmGnssPwr(&interface, &cmdTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 2073

   , &(interface.engine), "AT+CGNSPWR=0\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2077, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2081, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   interface.frm(&interface);



   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2087)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2088), UNITY_DISPLAY_STYLE_UINT8);



}

void test_gsmGnssGetDataF(void)

{





   gsmInterface_t interface;

   dataGnss_s dataGnssTest;

   rsp_t rspTest = {"CGNSINF","data"};







   frmCbackFlag = 0;







   gsmGnssGetData(&interface, &dataGnssTest, frmCbackTest);



   interface.frm(&interface);



   gsmSendCmd_CMockExpectAndReturn(

 2123

   , &(interface.engine), "AT+CGNSINF\r", OK_CMD_SENT)

                                          ;



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2127, &(interface.engine), OK_CMD_ACK);



   interface.frm(&interface);



   gsmProcessTkn_CMockExpectAndReturn(2131, &(interface.engine), OK_CLOSE);



   interface.frm(&interface);



   gsmGetCmdRsp_CMockExpectAndReturn(2135, &(interface.engine), rspTest);



   interface.frm(&interface);



   UnityAssertEqualString((const char*)(("data")), (const char*)((dataGnssTest.data)), (((void *)0)), (UNITY_UINT)(2139));

   if ((frmCbackFlag)) {} else {UnityFail( ((" Expected TRUE Was FALSE")), (UNITY_UINT)((UNITY_UINT)(2140)));};

   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((IDLE)), (UNITY_INT)(UNITY_UINT8 )((interface.frmState)), (((void *)0)), (UNITY_UINT)(2141), UNITY_DISPLAY_STYLE_UINT8);



}
