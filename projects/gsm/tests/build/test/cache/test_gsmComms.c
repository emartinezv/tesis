#include "build/temp/_test_gsmComms.c"
#include "mock_ciaaUART.h"
#include "gsmComms.h"
#include "unity.h"














void setUp(void)

{

   return;

}



void tearDown(void)

{

   return;

}











void test_gsm232UartRecv(void)

{

   uint8_t buffer[10];



   uartRecv_CMockExpectAndReturn(52, CIAA_UART_232, buffer, 10, 8);



   int res = 0;



   res = gsm232UartRecv(buffer, 10);



   UnityAssertEqualNumber((UNITY_INT)((8)), (UNITY_INT)((res)), (((void *)0)), (UNITY_UINT)(58), UNITY_DISPLAY_STYLE_INT);

}



void test_gsm232UartSend(void)

{

   uint8_t buffer[10];



   uartSend_CMockExpectAndReturn(65, CIAA_UART_232, buffer, 10, 8);



   int res = 0;



   res = gsm232UartSend(buffer, 10);



   UnityAssertEqualNumber((UNITY_INT)((8)), (UNITY_INT)((res)), (((void *)0)), (UNITY_UINT)(71), UNITY_DISPLAY_STYLE_INT);

}



void test_gsmTermUartRecv(void)

{

   uint8_t buffer[10];



   uartRecv_CMockExpectAndReturn(78, CIAA_UART_USB, buffer, 10, 8);



   int res = 0;



   res = gsmTermUartRecv(buffer, 10);



   UnityAssertEqualNumber((UNITY_INT)((8)), (UNITY_INT)((res)), (((void *)0)), (UNITY_UINT)(84), UNITY_DISPLAY_STYLE_INT);

}



void test_gsmTermUartSend(void)

{

   uint8_t buffer[10];



   uartSend_CMockExpectAndReturn(91, CIAA_UART_USB, buffer, 10, 8);



   int res = 0;



   res = gsmTermUartSend(buffer, 10);



   UnityAssertEqualNumber((UNITY_INT)((8)), (UNITY_INT)((res)), (((void *)0)), (UNITY_UINT)(97), UNITY_DISPLAY_STYLE_INT);

}
