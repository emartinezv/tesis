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

      /* ooo rsp */
      "\r\nOK\r\n",

      /* ooo data block */
      "abcde\r\n",

      /* AT echo + OK */
      "AT\r\r","\nOK\r\n",

      /* echo error + urc*/
      "ATI\r\r","\n+CMTI:adcde\r\n",

      /* AT echo + urc + OK */
      "AT\r\r","\n+CMTI:abcde\r\n","\r\nOK\r\n",

      /* ooo rsp */
      "\r\nOK\r\n",

      /* timeout */
      "",

      /* AT echo + rsp + timeout */
      "AT\r\r","\nabcde\r\n","",

      /* ooo data block */
      "abcde\r\n",

      /* AT echo + ERROR */
      "AT\r\r","\nERROR\r\n",

      /* CIFSR echo + ip */
      "AT+CIFSR\r\r","\n192.168.0.1\r\n",

      /* urc + AT echo + OK */
      "\r\n+CMTI:abcde\r\n","AT\r\r","\nOK\r\n",

      /* AT echo + ATI echo + urc */
      "AT\rA","TI\r\r","\n+CMTI:abcde\r\n",

      /* AT echo + invalid + urc */
      "AT\rT","A\r\r","\n+CMTI:abcde\r\n"
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

uint8_t gsm232UartSend_Callback (uint8_t * const buffer, int n, int NumCalls)
{
   return n;
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

/* This tests the gsmProcessTkn function as well as the private gsmSendCmd and
 * gsmUpdateFsm functions. The mutiple cmd-rsp sequences included are meant to
 * completely cover the code of gsmUpdateFsm.
 *
 * The gsmUpdateFsm function has a FSM wih 3 states, each of which has several
 * return points. These are labeled in the function code as 1.1, 1.2,...,
 * 2.1, 2.2,... etc. so as to reference what parts of the code we are covering
 * with each test.
 *
 * */

void test_gsmProcessTkn(void)
{
   /* Mock definitions */

   gsm232UartRecv_StubWithCallback(gsm232UartRecv_Callback);
   gsm232UartSend_StubWithCallback(gsm232UartSend_Callback);

   /* Initializations */

   gsmInitEngine();

   fsmEvent_e fsmEvent;

   /* urc received */
   /* tests 1.2 */

   fsmEvent = gsmProcessTkn(); /* mock mimics urc */
   TEST_ASSERT_EQUAL_INT(OK_URC, fsmEvent);

   /* response received */
   /* tests 1.3 */

   fsmEvent = gsmProcessTkn(); /* mock mimics ooo response */
   TEST_ASSERT_EQUAL_INT(ERR_OOO, fsmEvent);

   /* data block received */
   /* tests 1.4 */

   fsmEvent = gsmProcessTkn(); /* mock mimics ooo data block */
   TEST_ASSERT_EQUAL_INT(ERR_TKN_INV, fsmEvent);

   /* AT sent + AT echo received + OK received */
   /* tests 1.1, 2.1, 3.3 */

   fsmEvent = gsmSendCmd("AT\r"); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd echo */
   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics OK response */
   TEST_ASSERT_EQUAL_INT(OK_CLOSE, fsmEvent);

   /* AT sent + echo error + urc */
   /* tests 1.1, 2.2, 1.2 */

   fsmEvent = gsmSendCmd("AT\r"); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics cmd echo error */
   TEST_ASSERT_EQUAL_INT(ERR_CMD_ECHO, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics URC */
   TEST_ASSERT_EQUAL_INT(OK_URC, fsmEvent);

   /* AT sent + AT echo + urc + OK  */
   /* tests 1.1, 2.1, 3.1, 3.3 */

   fsmEvent = gsmSendCmd("AT\r"); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd echo */
   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics URC */
   TEST_ASSERT_EQUAL_INT(OK_URC, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics OK response */
   TEST_ASSERT_EQUAL_INT(OK_CLOSE, fsmEvent);

   /* AT sent + ooo rsp */
   /* tests 1.1, 2.4 */

   fsmEvent = gsmSendCmd("AT\r"); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics ooo response error */
   TEST_ASSERT_EQUAL_INT(ERR_CMD_ECHO, fsmEvent);

   /* AT sent + timeout */
   /* tests 1.1, 2.5 */

   fsmEvent = gsmSendCmd("AT\r"); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   int i = 0;

   for(i = 0; i < TOUT_DEF; i++) {gsmDecToutCnt();} /* mimic timeout */
   TEST_ASSERT_EQUAL_INT(1, gsmToutCntZero()); /* check that toutCnt == 0*/
   fsmEvent = gsmProcessTkn(); /* mock mimics no chars received event */
   TEST_ASSERT_EQUAL_INT(ERR_TIMEOUT, fsmEvent);

   /* AT sent + AT echo + rsp + timeout */
   /* tests 1.1, 2.1, 3.5, 3.7 */

   fsmEvent = gsmSendCmd("AT\r"); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd echo */
   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics OK response */
   TEST_ASSERT_EQUAL_INT(OK_RSP, fsmEvent);

   for(i = 0; i < TOUT_DEF; i++) {gsmDecToutCnt();} /* mimic timeout */
   TEST_ASSERT_EQUAL_INT(1, gsmToutCntZero()); /* check that toutCnt == 0*/
   fsmEvent = gsmProcessTkn(); /* mock mimics no chars received */
   TEST_ASSERT_EQUAL_INT(ERR_TIMEOUT, fsmEvent);

   /* AT sent + ooo data block */
   /* tests 1.1, 2.6 */

   fsmEvent = gsmSendCmd("AT\r"); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics ooo data block */
   TEST_ASSERT_EQUAL_INT(ERR_TKN_INV, fsmEvent);

   /* AT sent + AT echo + ERROR */
   /* tests 1.1, 2.1, 3.2 */

   fsmEvent = gsmSendCmd("AT\r"); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd echo */
   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics ERROR response */
   TEST_ASSERT_EQUAL_INT(ERR_MSG_CLOSE, fsmEvent);

   /* CIFSR sent + CIFSR echo + ip */
   /* tests 1.1, 2.1, 3.4 */

   fsmEvent = gsmSendCmd("AT+CIFSR\r"); /* mock mimics CIFSR cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics CIFSR cmd echo */
   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics IP response */
   TEST_ASSERT_EQUAL_INT(OK_CLOSE, fsmEvent);

   /* AT sent + urc + AT echo + OK */
   /* tests 1.1, 2.3, 2.1, 3.3 */

   fsmEvent = gsmSendCmd("AT\r"); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics URC */
   TEST_ASSERT_EQUAL_INT(OK_URC, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd echo */
   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics OK response */
   TEST_ASSERT_EQUAL_INT(OK_CLOSE, fsmEvent);

   /* AT sent + AT echo + ATI echo + urc */
   /* tests 1.1, 2.1, 3.6, 1.2 */

   fsmEvent = gsmSendCmd("AT\r"); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd echo */
   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics ATI cmd echo error */
   TEST_ASSERT_EQUAL_INT(ERR_OOO, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics urc */
   TEST_ASSERT_EQUAL_INT(OK_URC, fsmEvent);

   /* AT sent + AT echo + invalid + urc */
   /* tests 1.1, 2.1, 3.8, 1.2 */

   fsmEvent = gsmSendCmd("AT\r"); /* mock mimics AT cmd sent */
   TEST_ASSERT_EQUAL_INT(OK_CMD_SENT, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics AT cmd echo */
   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics invalid tkn error */
   TEST_ASSERT_EQUAL_INT(ERR_TKN_INV, fsmEvent);

   fsmEvent = gsmProcessTkn(); /* mock mimics urc */
   TEST_ASSERT_EQUAL_INT(OK_URC, fsmEvent);

   return;
}

