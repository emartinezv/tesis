#include "build/temp/_test_gsmCommands.c"
#include "gsmCommands.h"
#include "unity.h"














void setUp(void)

{

   return;

}



void tearDown(void)

{

   return;

}











void test_gsmCmdSearch(void)

{





   uint16_t idx = 65535;











   idx = gsmCmdSearch("AT");



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT16)((0)), (UNITY_INT)(UNITY_INT16)((idx)), (((void *)0)), (UNITY_UINT)(58), UNITY_DISPLAY_STYLE_INT16);







   idx = gsmCmdSearch("CMGF");



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT16)((5)), (UNITY_INT)(UNITY_INT16)((idx)), (((void *)0)), (UNITY_UINT)(64), UNITY_DISPLAY_STYLE_INT16);







   idx = gsmCmdSearch("CIPSTART");



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT16)((16)), (UNITY_INT)(UNITY_INT16)((idx)), (((void *)0)), (UNITY_UINT)(70), UNITY_DISPLAY_STYLE_INT16);







   idx = gsmCmdSearch("abcde");



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT16)((65535)), (UNITY_INT)(UNITY_INT16)((idx)), (((void *)0)), (UNITY_UINT)(76), UNITY_DISPLAY_STYLE_INT16);



   return;



}



void test_gsmGetCmdSucRsp(void)

{





   const uint8_t * rsp;











   rsp = gsmGetCmdSucRsp(3);



   UnityAssertEqualString((const char*)(("-OK-")), (const char*)((rsp)), (((void *)0)), (UNITY_UINT)(94));







   rsp = gsmGetCmdSucRsp(16);



   UnityAssertEqualString((const char*)(("-CONNECT OK-ALREADY CONNECT-CONNECT FAIL-CONNECT-")), (const char*)((rsp)), (((void *)0)), (UNITY_UINT)(101))

                                 ;







   rsp = gsmGetCmdSucRsp(17);



   UnityAssertEqualString((const char*)(("-CLOSE OK-")), (const char*)((rsp)), (((void *)0)), (UNITY_UINT)(107));

}



void test_gsmGetCmdErrRsp(void)

{





   const uint8_t * rsp;











   rsp = gsmGetCmdErrRsp(3);



   UnityAssertEqualString((const char*)(("-ERROR-CME ERROR-CMS ERROR-")), (const char*)((rsp)), (((void *)0)), (UNITY_UINT)(122));







   rsp = gsmGetCmdErrRsp(11);



   UnityAssertEqualString((const char*)(("-ERROR-")), (const char*)((rsp)), (((void *)0)), (UNITY_UINT)(128));



}



void test_gsmGetCmdtimeout(void)

{





   uint32_t timeout;











   timeout = gsmGetCmdTimeout(2);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((100)), (UNITY_INT)(UNITY_UINT32)((timeout)), (((void *)0)), (UNITY_UINT)(144), UNITY_DISPLAY_STYLE_UINT32);







   timeout = gsmGetCmdTimeout(15);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((85000)), (UNITY_INT)(UNITY_UINT32)((timeout)), (((void *)0)), (UNITY_UINT)(150), UNITY_DISPLAY_STYLE_UINT32);







   timeout = gsmGetCmdTimeout(12);



   UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((25000)), (UNITY_INT)(UNITY_UINT32)((timeout)), (((void *)0)), (UNITY_UINT)(156), UNITY_DISPLAY_STYLE_UINT32);



}



void test_gsmUrcSearch(void)

{





   uint8_t idx = 2;







   idx = gsmUrcSearch("CMTI");



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((1)), (UNITY_INT)(UNITY_INT8 )((idx)), (((void *)0)), (UNITY_UINT)(170), UNITY_DISPLAY_STYLE_INT8);







   idx = gsmUrcSearch("abcde");



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((0)), (UNITY_INT)(UNITY_INT8 )((idx)), (((void *)0)), (UNITY_UINT)(176), UNITY_DISPLAY_STYLE_INT8);



   return;



}
