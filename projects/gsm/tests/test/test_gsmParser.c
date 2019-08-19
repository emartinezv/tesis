/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

//-- unity: unit test framework
#include "unity.h"

//-- module being tested
#include "gsmParser.h"
#include "string.h"
#include "mock_gsmComms.h"
#include "mock_gsmTokenizer.h"

/*******************************************************************************
 *    DEFINITIONS
 ******************************************************************************/

#define TKN_SIZ 300

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

void test_gsmParseTkn(void)
{
   tknTypeParser_e tknType;

   uint8_t tkn[2*TKN_SIZ];

   uint8_t cmd[TKN_SIZ];
   uint8_t par[TKN_SIZ];

   /* Testing INVALID token option 1 */

   tknType = AUTOBAUD;

   strncpy(tkn,"AT&\r",strlen("AT&\r"));
   tkn[strlen("AT&\r")] = ECHO;

   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT&\r")+1);

   TEST_ASSERT_EQUAL_INT(0, strlen(cmd));
   TEST_ASSERT_EQUAL_INT(0, strlen(par));
   TEST_ASSERT_EQUAL_INT(INVALID, tknType);

   /* Testing INVALID token option 2 */

   tknType = AUTOBAUD;

   strncpy(tkn,"abcde\r",strlen("abcde\r"));
   tkn[strlen("abcde\r")] = ECHO;

   tknType = gsmParseTkn(tkn, cmd, par, strlen("abcde\r")+1);

   TEST_ASSERT_EQUAL_INT(0, strlen(cmd));
   TEST_ASSERT_EQUAL_INT(0, strlen(par));
   TEST_ASSERT_EQUAL_INT(INVALID, tknType);

   /* Testing AUTOBAUD token */

   tknType = INVALID;

   strncpy(tkn,"AT\r",strlen("AT\r"));
   tkn[strlen("AT\r")] = ECHO;

   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT\r")+1);

   TEST_ASSERT_EQUAL_STRING_LEN("AT", cmd, strlen(cmd));
   TEST_ASSERT_EQUAL_INT(0, strlen(par));
   TEST_ASSERT_EQUAL_INT8(AUTOBAUD, tknType);

   /* Testing BASIC_CMD token no parameters */

   tknType = INVALID;

   strncpy(tkn,"ATX\r",strlen("ATX\r"));
   tkn[strlen("ATX\r")] = ECHO;

   tknType = gsmParseTkn(tkn, cmd, par, strlen("ATX\r")+1);

   TEST_ASSERT_EQUAL_STRING_LEN("X", cmd, strlen(cmd));
   TEST_ASSERT_EQUAL_INT(0, strlen(par));
   TEST_ASSERT_EQUAL_INT8(BASIC_CMD, tknType);

   /* Testing BASIC_CMD token with parameters */

   tknType = INVALID;

   strncpy(tkn,"ATX123\r",strlen("ATX123\r"));
   tkn[strlen("ATX123\r")] = ECHO;

   tknType = gsmParseTkn(tkn, cmd, par, strlen("ATX123\r")+1);

   TEST_ASSERT_EQUAL_STRING_LEN("X", cmd, strlen(cmd));
   TEST_ASSERT_EQUAL_STRING_LEN("123", par, strlen(par));
   TEST_ASSERT_EQUAL_INT8(BASIC_CMD, tknType);

   /* Testing BASIC_CMD_AMP token */

   tknType = INVALID;

   strncpy(tkn,"AT&X123\r",strlen("AT&X123\r"));
   tkn[strlen("AT&X123\r")] = ECHO;

   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT&X123\r")+1);

   TEST_ASSERT_EQUAL_STRING_LEN("X", cmd, strlen(cmd));
   TEST_ASSERT_EQUAL_STRING_LEN("123", par, strlen(par));
   TEST_ASSERT_EQUAL_INT8(BASIC_CMD_AMP, tknType);

   /* Testing EXT_CMD_TEST token */

   tknType = INVALID;

   strncpy(tkn,"AT+X=?\r",strlen("AT+X=?\r"));
   tkn[strlen("AT+X=?\r")] = ECHO;

   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT+X=?\r")+1);

   TEST_ASSERT_EQUAL_STRING_LEN("X", cmd, strlen(cmd));
   TEST_ASSERT_EQUAL_INT(0, strlen(par));
   TEST_ASSERT_EQUAL_INT8(EXT_CMD_TEST, tknType);

   /* Testing EXT_CMD_WRITE token */

   tknType = INVALID;

   strncpy(tkn,"AT+X=123\r",strlen("AT+X=123\r"));
   tkn[strlen("AT+X=123\r")] = ECHO;

   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT+X=123\r")+1);

   TEST_ASSERT_EQUAL_STRING_LEN("X", cmd, strlen(cmd));
   TEST_ASSERT_EQUAL_STRING_LEN("123", par, strlen(par));
   TEST_ASSERT_EQUAL_INT8(EXT_CMD_WRITE, tknType);

   /* Testing EXT_CMD_READ token */

   tknType = INVALID;

   strncpy(tkn,"AT+X?\r",strlen("AT+X?\r"));
   tkn[strlen("AT+X?\r")] = ECHO;

   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT+X?\r")+1);

   TEST_ASSERT_EQUAL_STRING_LEN("X", cmd, strlen(cmd));
   TEST_ASSERT_EQUAL_INT(0, strlen(par));
   TEST_ASSERT_EQUAL_INT8(EXT_CMD_READ, tknType);

   /* Testing EXT_CMD_EXEC token */

   tknType = INVALID;

   strncpy(tkn,"AT+X\r",strlen("AT+X\r"));
   tkn[strlen("AT+X\r")] = ECHO;

   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT+X\r")+1);

   TEST_ASSERT_EQUAL_STRING_LEN("X", cmd, strlen(cmd));
   TEST_ASSERT_EQUAL_INT(0, strlen(par));
   TEST_ASSERT_EQUAL_INT8(EXT_CMD_EXEC, tknType);

   /* Testing SMS_BODY_P token */

   tknType = INVALID;

   strncpy(tkn,"abcde",strlen("abcde"));
   tkn[strlen("abcde")] = SMS_BODY;

   tknType = gsmParseTkn(tkn, cmd, par, strlen("abcde")+1);

   TEST_ASSERT_EQUAL_STRING_LEN("SMS_BODY", cmd, strlen(cmd));
   TEST_ASSERT_EQUAL_STRING_LEN("abcde", par, strlen(par));
   TEST_ASSERT_EQUAL_INT8(SMS_BODY_P, tknType);

   /* Testing BASIC_RSP token */

   tknType = INVALID;

   strncpy(tkn,"\r\nabcde\r\n",strlen("\r\nabcde\r\n"));
   tkn[strlen("\r\nabcde\r\n")] = RSP;

   tknType = gsmParseTkn(tkn, cmd, par, strlen("\r\nabcde\r\n")+1);

   TEST_ASSERT_EQUAL_STRING_LEN("abcde", cmd, strlen(cmd));
   TEST_ASSERT_EQUAL_INT(0, strlen(par));
   TEST_ASSERT_EQUAL_INT8(BASIC_RSP, tknType);

   /* Testing DATA_BLOCK_P token */

   tknType = INVALID;

   strncpy(tkn,"abcde\r\n",strlen("abcde\r\n"));
   tkn[strlen("abcde\r\n")] = DATA_BLOCK;

   tknType = gsmParseTkn(tkn, cmd, par, strlen("abcde\r\n")+1);

   TEST_ASSERT_EQUAL_STRING_LEN("DATA", cmd, strlen(cmd));
   TEST_ASSERT_EQUAL_STRING_LEN("abcde", par, strlen(par));
   TEST_ASSERT_EQUAL_INT8(DATA_BLOCK_P, tknType);

   /* Testing SMS_PROMPT_P token */

   tknType = INVALID;

   strncpy(tkn,"\r\n> ",strlen("\r\n> "));
   tkn[strlen("\r\n> ")] = SMS_PROMPT;

   tknType = gsmParseTkn(tkn, cmd, par, strlen("\r\n> ")+1);

   TEST_ASSERT_EQUAL_STRING_LEN("> ", cmd, strlen(cmd));
   TEST_ASSERT_EQUAL_INT(0, strlen(par));
   TEST_ASSERT_EQUAL_INT8(SMS_PROMPT_P, tknType);

   /* Testing EXT_RSP token without parameters */

   tknType = INVALID;

   strncpy(tkn,"\r\n+X\r\n",strlen("\r\n+X\r\n"));
   tkn[strlen("\r\n+X\r\n")] = RSP;

   tknType = gsmParseTkn(tkn, cmd, par, strlen("\r\n+X\r\n")+1);

   TEST_ASSERT_EQUAL_STRING_LEN("X", cmd, strlen(cmd));
   TEST_ASSERT_EQUAL_INT(0, strlen(par));
   TEST_ASSERT_EQUAL_INT8(EXT_RSP, tknType);

   /* Testing EXT_RSP token with parameters */

   tknType = INVALID;

   strncpy(tkn,"\r\n+X:123\r\n",strlen("\r\n+X:123\r\n"));
   tkn[strlen("\r\n+X:123\r\n")] = RSP;

   tknType = gsmParseTkn(tkn, cmd, par, strlen("\r\n+X:123\r\n")+1);

   TEST_ASSERT_EQUAL_STRING_LEN("X", cmd, strlen(cmd));
   TEST_ASSERT_EQUAL_STRING_LEN("123", par, strlen(par));
   TEST_ASSERT_EQUAL_INT8(EXT_RSP, tknType);

   return;

}




