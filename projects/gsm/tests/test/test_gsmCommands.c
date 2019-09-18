/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

//-- unity: unit test framework
#include "unity.h"

//-- module being tested
#include "gsmCommands.h"

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

/* test_gsmCmdSearch
 *
 * Functions tested:
 *
 * - gsmCmdSearch
 *
 * */

void test_gsmCmdSearch(void)
{
   /* Variables */

   uint16_t idx = UNKNOWN_CMD;

   /* Test sequence */

   /* Test AT cmd */

   idx = gsmCmdSearch("AT");

   TEST_ASSERT_EQUAL_INT16(0, idx);

   /* Test CMGF cmd */

   idx = gsmCmdSearch("CMGF");

   TEST_ASSERT_EQUAL_INT16(5, idx);

   /* Test CIPSTART cmd */

   idx = gsmCmdSearch("CIPSTART");

   TEST_ASSERT_EQUAL_INT16(16, idx);

   /* Test nonexistent cmd */

   idx = gsmCmdSearch("abcde");

   TEST_ASSERT_EQUAL_INT16(UNKNOWN_CMD, idx);

   return;

}

/* test_gsmGetCmdSucRsp
 *
 * Functions tested:
 *
 * - gsmGetCmdSucRsp
 *
 * */

void test_gsmGetCmdSucRsp(void)
{
   /* Variables */

   const uint8_t * rsp;

   /* Test sequence */

   /* Test CMGL cmd */

   rsp = gsmGetCmdSucRsp(3);

   TEST_ASSERT_EQUAL_STRING("-OK-", rsp);

   /* Test CIPSTART cmd */

   rsp = gsmGetCmdSucRsp(16);

   TEST_ASSERT_EQUAL_STRING("-CONNECT OK-ALREADY CONNECT-CONNECT FAIL-CONNECT-"
                            ,rsp);

   /* Test CIPCLOSE cmd */

   rsp = gsmGetCmdSucRsp(17);

   TEST_ASSERT_EQUAL_STRING("-CLOSE OK-", rsp);
}

/* test_gsmGetCmdErrRsp
 *
 * Functions tested:
 *
 * - gsmGetCmdErrRsp
 *
 * */

void test_gsmGetCmdErrRsp(void)
{
   /* Variables */

   const uint8_t * rsp;

   /* Test sequence */

   /* Test CMGL cmd */

   rsp = gsmGetCmdErrRsp(3);

   TEST_ASSERT_EQUAL_STRING("-ERROR-CME ERROR-CMS ERROR-", rsp);

   /* Test CNMI cmd */

   rsp = gsmGetCmdErrRsp(11);

   TEST_ASSERT_EQUAL_STRING("-ERROR-", rsp);

}

/* test_gsmGetCmdTimeout
 *
 * Functions tested:
 *
 * - gsmGetCmdTimeout
 *
 * */

void test_gsmGetCmdTimeout(void)
{
   /* Variables */

   uint32_t timeout;

   /* Test sequence */

   /* Test CMEE cmd */

   timeout = gsmGetCmdTimeout(2);

   TEST_ASSERT_EQUAL_UINT32(TOUT_DEF, timeout);

   /* Test CIICR cmd */

   timeout = gsmGetCmdTimeout(15);

   TEST_ASSERT_EQUAL_UINT32(85000, timeout);

   /* Test CMGD cmd */

   timeout = gsmGetCmdTimeout(12);

   TEST_ASSERT_EQUAL_UINT32(25000, timeout);

}

/* test_gsmUrcSearch
 *
 * Functions tested:
 *
 * - gsmUrcSearch
 *
 * */

void test_gsmUrcSearch(void)
{
   /* Variables */

   uint8_t idx = 2;

   /* Test CMTI urc */

   idx = gsmUrcSearch("CMTI");

   TEST_ASSERT_EQUAL_INT8(1, idx);

   /* Test nonexistent urc */

   idx = gsmUrcSearch("abcde");

   TEST_ASSERT_EQUAL_INT8(0, idx);

   return;

}

