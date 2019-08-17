/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

//-- unity: unit test framework
#include "unity.h"

//-- module being tested
#include "gsmCommands.h"
#include "string.h"

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

void test_gsmCmdSearch(void)
{
   uint16_t idx = UNKNOWN_CMD;

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

void test_gsmUrcSearch(void)
{
   uint8_t idx = 2;

   /* Test CMTI urc */

   idx = gsmUrcSearch("CMTI");

   TEST_ASSERT_EQUAL_INT8(1, idx);

   /* Test nonexistent urc */

   idx = gsmUrcSearch("abcde");

   TEST_ASSERT_EQUAL_INT8(0, idx);

   return;

}


