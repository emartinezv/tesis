/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

//-- unity: unit test framework
#include "unity.h"

//-- module being tested
#include "gsmEngine.h"

//-- mocks
#include "mock_gsmTokenizer.h"
#include "mock_gsmComms.h"
#include "mock_gsmParser.h"
#include "mock_vl_ring_buffer.h"

//-- other includes
#include "gsmCommands.h"
#include "ring_buffer.h"
#include "string.h"


/*******************************************************************************
 *    DEFINITIONS
 ******************************************************************************/

#define TKN_SIZ 300

/*******************************************************************************
 *    PRIVATE TYPES
 ******************************************************************************/

typedef struct _tknTypeSize {
   uint8_t const * tkn;
   uint8_t const * cmd;
   uint8_t const * par;
   tknTypeParser_e type;
   int size
} tknTypeSize_s;

/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/

static tknTypeSize_s tknTypeSizeV [] = {

   {"\r\n+CMTI:abcde\r\n","CMTI","abcde",EXT_RSP,16},

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
      "AT\rT","A\r\r","\n+CMTI:abcde\r\n",

      /* AT+CMGS=num echo + sms prompt + sms body + OK */
      "AT+CMGS=num\r\r","\n> ","hola\r\n","OK\r\n"
};

/*******************************************************************************
 *    PRIVATE FUNCTIONS
 ******************************************************************************/

int VLRingBuffer_Pop_Callback (VLRINGBUFF_T * vlrb, void * data, uint16_t cap,
                               int NumCalls)
{
   if(NumCalls > (sizeof(tknTypeSizeV)/sizeof(tknTypeSize_s))){
      TEST_FAIL_MESSAGE("Demasiadas llamadas!");
   }

   strncpy(data, tknTypeSizeV[NumCalls].tkn,
           strlen(tknTypeSizeV[NumCalls].tkn));

   return (strlen(tknTypeSizeV[NumCalls].tkn);
}

tknTypeParser_e gsmParseTkn_Callback (uint8_t const * const tkn, uint8_t * cmd,
                                      uint8_t * par, uint16_t tknLen,
                                      int NumCalls)
{
   if(NumCalls > (sizeof(tknTypeSizeV)/sizeof(tknTypeSize_s))){
      TEST_FAIL_MESSAGE("Demasiadas llamadas!");
   }

   strncpy(data, tknTypeSizeV[NumCalls].tkn,
           strlen(tknTypeSizeV[NumCalls].tkn));

   return (strlen(tknTypeSizeV[NumCalls].tkn);
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

/* test_gsmInitEngine
 *
 * Functions tested:
 *
 * - gsmInitEngine
 *
 * */

void test_gsmInitEngine(void)
{
   /* Variables */

   gsmEngine_t engine;
   bool result;

   /* Mocks */

   gsmInitTokenizer_IgnoreAndReturn(true);
   VLRingBuffer_Init_IgnoreAndReturn(1);

   /* Test sequence */

   result = gsmInitEngine(&engine);

   TEST_ASSERT_TRUE(result);
}

/* test_gsmProcessTkn
 *
 * Functions tested:
 *
 * - gsmProcessTkn
 * - gsmUpdateFsm (static)
 *
 * */

void test_gsmProcessTkn(void)
{
   /* Variables */

   gsmEngine_t engine;
   fsmEvent_e event;

   /* Mocks */

   gsmDetectTkns_Ignore();
   VLRingBuffer_IsEmpty_IgnoreAndReturn(0);
   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback);
   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback)


}

