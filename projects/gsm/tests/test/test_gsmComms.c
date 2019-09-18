/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

//-- unity: unit test framework
#include "unity.h"

//-- module being tested
#include "gsmComms.h"

//-- mocks
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

/* test_gsm232UartRecv
 *
 * Functions tested:
 *
 * - gsm232UartRecv
 *
 * */

void test_gsm232UartRecv(void)
{
   /* Variables */

   uint8_t buffer[10];

   /* Test sequence */

   uartRecv_ExpectAndReturn(CIAA_UART_232, buffer, 10, 8);

   int res = 0;

   res = gsm232UartRecv(buffer, 10);

   TEST_ASSERT_EQUAL_INT(8, res);
}

/* test_gsm232UartSend
 *
 * Functions tested:
 *
 * - gsm232UartSend
 *
 * */

void test_gsm232UartSend(void)
{
   /* Variables */

   uint8_t buffer[10];

   /* Test sequence */

   uartSend_ExpectAndReturn(CIAA_UART_232, buffer, 10, 8);

   int res = 0;

   res = gsm232UartSend(buffer, 10);

   TEST_ASSERT_EQUAL_INT(8, res);
}

/* test_gsmTermUartRecv
 *
 * Functions tested:
 *
 * - gsmTermUartRecv
 *
 * */

void test_gsmTermUartRecv(void)
{
   /* Variables */

   uint8_t buffer[10];

   /* Test sequence */

   uartRecv_ExpectAndReturn(CIAA_UART_USB, buffer, 10, 8);

   int res = 0;

   res = gsmTermUartRecv(buffer, 10);

   TEST_ASSERT_EQUAL_INT(8, res);
}

/* test_gsmTermUartSend
 *
 * Functions tested:
 *
 * - gsmTermUartSend
 *
 * */

void test_gsmTermUartSend(void)
{
   /* Variables */

   uint8_t buffer[10];

   /* Test sequence */

   uartSend_ExpectAndReturn(CIAA_UART_USB, buffer, 10, 8);

   int res = 0;

   res = gsmTermUartSend(buffer, 10);

   TEST_ASSERT_EQUAL_INT(8, res);
}


