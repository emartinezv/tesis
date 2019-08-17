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



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT16)((0)), (UNITY_INT)(UNITY_INT16)((idx)), (((void *)0)), (UNITY_UINT)(54), UNITY_DISPLAY_STYLE_INT16);







   idx = gsmCmdSearch("CMGF");



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT16)((5)), (UNITY_INT)(UNITY_INT16)((idx)), (((void *)0)), (UNITY_UINT)(60), UNITY_DISPLAY_STYLE_INT16);







   idx = gsmCmdSearch("CIPSTART");



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT16)((16)), (UNITY_INT)(UNITY_INT16)((idx)), (((void *)0)), (UNITY_UINT)(66), UNITY_DISPLAY_STYLE_INT16);







   idx = gsmCmdSearch("abcde");



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT16)((65535)), (UNITY_INT)(UNITY_INT16)((idx)), (((void *)0)), (UNITY_UINT)(72), UNITY_DISPLAY_STYLE_INT16);



   return;



}



void test_gsmUrcSearch(void)

{

   uint8_t idx = 2;







   idx = gsmUrcSearch("CMTI");



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((1)), (UNITY_INT)(UNITY_INT8 )((idx)), (((void *)0)), (UNITY_UINT)(86), UNITY_DISPLAY_STYLE_INT8);







   idx = gsmUrcSearch("abcde");



   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((0)), (UNITY_INT)(UNITY_INT8 )((idx)), (((void *)0)), (UNITY_UINT)(92), UNITY_DISPLAY_STYLE_INT8);



   return;



}
