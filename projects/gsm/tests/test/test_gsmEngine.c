/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

//-- unity: unit test framework
#include "unity.h"

//-- module being tested
#include "gsmEngine.h"
#include "gsmParser.h"
#include "gsmCommands.h"
#include "gsmTokenizer.h"
#include "string.h"
#include "mock_gsmComms.h"
#include "ring_buffer.h"
#include "vl_ring_buffer.h"

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

static uint8_t const * const uartRecvMock [] = {

      /* urc */
      "\r\n+CMTI:abcde\r\n",

      /* ooo OK response */
      "\r\nOK\r\n",

      /* ooo data block */
      "abcde\r\n",

      /* AT-OK */
      "AT\rA","T\r\r","\nOK\r\n",

      /* AT-cmd echo error*/

      "AT\rA","TI\rA",

      /* AT-urc-OK */
      "T\rA","T\r\r","\n+CMTI:abcde\r\n","\r\nOK\r\n",

      /* AT-ooo rsp*/

      "AT\r\r","\nOK\r\n",

      /* AT-TIMEOUT */
      "AT\rA","T\rA", "",

      /* AT-ooo data block */
      "T\ra","bcde\r\n",

      /* AT-ERROR */
      "AT\rA","T\r\r","\nERROR\r\n",

      /* ATI-info-OK */
      "AT\rA","T\r\r","\ninfo\r\n", "\r\nOK\r\n"

};

/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/

uint8_t gsm232UartRecv_Callback (uint8_t * const buffer, int n, int NumCalls)
{
   uint8_t res = 0;

   if(NumCalls > (sizeof(uartRecvMock)/sizeof(uint8_t *))){
      TEST_FAIL_MESSAGE("Demasiadas llamadas!");
   }

   strncpy(buffer, uartRecvMock[NumCalls], strlen(uartRecvMock[NumCalls]));
   res = strlen(uartRecvMock[NumCalls]);

   return res;
}

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

/* This tests the gsmProcessTkn function as well as the pricate gsmUpdateFsm
 * function. The several sequences included are meant to completely cover the
 * code of gsmUpdateFsm. */

void test_gsmProcessTkn(void)
{
   /* Mock definitions */

   gsm232UartRecv_StubWithCallback(gsm232UartRecv_Callback);

   /* Initializations */

   gsmInitEngine();

   fsmEvent_e fsmEvent;

   /* Test urc sequence */

   fsmEvent = gsmProcessTkn(); /* mock mimics urc */
   TEST_ASSERT_EQUAL_INT(OK_URC, fsmEvent);

   /* Test out-of-order response sequence */

   fsmEvent = gsmProcessTkn(); /* mock mimics ooo OK response */
   TEST_ASSERT_EQUAL_INT(ERR_OOO, fsmEvent);

   /* Test out-of-order data block sequence */

   fsmEvent = gsmProcessTkn(); /* mock mimics ooo data block */
   TEST_ASSERT_EQUAL_INT(ERR_TKN_INV, fsmEvent);

   /* Test AT-OK sequence */

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd echo */
   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics OK response */
   TEST_ASSERT_EQUAL_INT(OK_CLOSE, fsmEvent);

   /* Test AT-cmd echo error sequence */

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics cmd echo error */
   TEST_ASSERT_EQUAL_INT(ERR_CMD_ECHO, fsmEvent);

   /* Test AT-urc-OK sequence */

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd echo */
   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics URC */
   TEST_ASSERT_EQUAL_INT(OK_URC, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics OK response */
   TEST_ASSERT_EQUAL_INT(OK_CLOSE, fsmEvent);

   /* Test AT-ooo rsp sequence */

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics ooo response error */
   TEST_ASSERT_EQUAL_INT(ERR_CMD_ECHO, fsmEvent);

   /* Test AT-TIMEOUT sequence */

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd echo */
   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, fsmEvent);

   int i = 0;

   for(i = 0; i < TOUT_DEF; i++) {gsmDecToutCnt();} /* mimic timeout */
   TEST_ASSERT_EQUAL_INT(1, gsmToutCntZero()); /* check that toutCnt == 0*/
   fsmEvent = gsmProcessTkn(); /* mock mimics no chars received event */
   TEST_ASSERT_EQUAL_INT(ERR_TIMEOUT, fsmEvent);

   /* Test AT-ooo data block sequence */

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics ooo data block error */
   TEST_ASSERT_EQUAL_INT(ERR_TKN_INV, fsmEvent);

   /* Test AT-ERROR sequence */

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd echo */
   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics ERROR response */
   TEST_ASSERT_EQUAL_INT(ERR_MSG_CLOSE, fsmEvent);

   /* Test ATI-info-OK sequence */

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd echo */
   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics info response */
   TEST_ASSERT_EQUAL_INT(OK_RSP, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics OK response */
   TEST_ASSERT_EQUAL_INT(OK_CLOSE, fsmEvent);



   return;
}

