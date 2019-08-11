/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

//-- unity: unit test framework
#include "unity.h"

//-- module being tested
#include "gsmComms.h"
#include "mock_ciaaUART.h"

/*******************************************************************************
 *    DEFINITIONS
 ******************************************************************************/

/*******************************************************************************
 *    PRIVATE TYPES
 ******************************************************************************/

/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/


/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/


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

void test_gsm232UartRecv(void)
{
   uint8_t buffer[10];

   uartRecv_ExpectAndReturn(CIAA_UART_232, buffer, 10, 8);

   int res = 0;

   res = gsm232UartRecv(buffer, 10);

   TEST_ASSERT_EQUAL_INT(8, res);
}

void test_gsm232UartSend(void)
{
   uint8_t buffer[10];

   uartSend_ExpectAndReturn(CIAA_UART_232, buffer, 10, 8);

   int res = 0;

   res = gsm232UartSend(buffer, 10);

   TEST_ASSERT_EQUAL_INT(8, res);
}

void test_gsmTermUartRecv(void)
{
   uint8_t buffer[10];

   uartRecv_ExpectAndReturn(CIAA_UART_USB, buffer, 10, 8);

   int res = 0;

   res = gsmTermUartRecv(buffer, 10);

   TEST_ASSERT_EQUAL_INT(8, res);
}

void test_gsmTermUartSend(void)
{
   uint8_t buffer[10];

   uartSend_ExpectAndReturn(CIAA_UART_USB, buffer, 10, 8);

   int res = 0;

   res = gsmTermUartSend(buffer, 10);

   TEST_ASSERT_EQUAL_INT(8, res);
}


