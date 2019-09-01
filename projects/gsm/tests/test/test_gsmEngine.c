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
   int size;
} tknTypeSize_s;

/*******************************************************************************
 *    PRIVATE DATA
 ******************************************************************************/

static tknTypeSize_s tknTypeSizeV [] = {

   /* URC */
   {"\r\n+CMTI:abcde\r\n","CMTI","abcde",EXT_RSP,16},

   /* ooo rsp */
   {"\r\nOK\r\n","OK","",BASIC_RSP,7},

   /* ooo data block */
   {"abcde\r\n","DATA","abcde",DATA_BLOCK_P,8},

   /* AT echo + OK */
   {"AT\r","AT","",AUTOBAUD,4},
   {"\r\nOK\r\n","OK","",BASIC_RSP,7}
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

   return (tknTypeSizeV[NumCalls].size);
}

tknTypeParser_e gsmParseTkn_Callback (uint8_t const * const tkn, uint8_t * cmd,
                                      uint8_t * par, uint16_t tknLen,
                                      int NumCalls)
{
   if(NumCalls > (sizeof(tknTypeSizeV)/sizeof(tknTypeSize_s))){
      TEST_FAIL_MESSAGE("Demasiadas llamadas!");
   }

   strncpy(cmd, tknTypeSizeV[NumCalls].cmd,
           strlen(tknTypeSizeV[NumCalls].cmd));
   cmd[strlen(tknTypeSizeV[NumCalls].cmd)] = '\0';

   if(strlen(tknTypeSizeV[NumCalls].par) > 0){
      strncpy(par, tknTypeSizeV[NumCalls].par,
              strlen(tknTypeSizeV[NumCalls].par));
      par[strlen(tknTypeSizeV[NumCalls].par)] = '\0';
   }
   else{
      par[0]='\0';
   }

   return (tknTypeSizeV[NumCalls].type);
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
 *
 * */

void test_gsmProcessTkn(void)
{
   /* Variables */

   gsmEngine_t engine;
   fsmEvent_e event;

   /* Initialization */

   engine.fsmState = WAITING;

   /* Mocks */

   gsmTermUartSend_IgnoreAndReturn(0);
   gsmDetectTkns_Ignore();
   VLRingBuffer_IsEmpty_IgnoreAndReturn(0);
   VLRingBuffer_IsFull_IgnoreAndReturn(0);
   VLRingBuffer_Insert_IgnoreAndReturn(0);
   VLRingBuffer_Flush_Ignore();
   VLRingBuffer_Pop_StubWithCallback(VLRingBuffer_Pop_Callback);
   gsmParseTkn_StubWithCallback(gsmParseTkn_Callback);

   /* Test sequence */

   /* URC */

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_URC, event);

   /* ooo rsp */

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_OOO, event);

   /* ooo data block */

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(ERR_TKN_INV, event);

   /* AT sent + AT echo + OK */

   engine.fsmState = CMD_SENT;
   strncpy(engine.currCmd, "AT", strlen("AT")+1);
   engine.currPar[0]='\0';
   engine.currIdx = 0;

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_CMD_ACK, event);

   event = gsmProcessTkn(&engine);

   TEST_ASSERT_EQUAL_INT(OK_CLOSE, event);
}

