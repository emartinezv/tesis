/* AUTOGENERATED FILE. DO NOT EDIT. */
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include "unity.h"
#include "cmock.h"
#include "mock_gsmParser.h"

static const char* CMockString_cmd = "cmd";
static const char* CMockString_gsmParseTkn = "gsmParseTkn";
static const char* CMockString_par = "par";
static const char* CMockString_tkn = "tkn";
static const char* CMockString_tknLen = "tknLen";

typedef struct _CMOCK_gsmParseTkn_CALL_INSTANCE
{
  UNITY_LINE_TYPE LineNumber;
  tknTypeParser_e ReturnVal;
  int CallOrder;
  uint8_t const* Expected_tkn;
  uint8_t* Expected_cmd;
  uint8_t* Expected_par;
  uint16_t Expected_tknLen;

} CMOCK_gsmParseTkn_CALL_INSTANCE;

static struct mock_gsmParserInstance
{
  int gsmParseTkn_IgnoreBool;
  tknTypeParser_e gsmParseTkn_FinalReturn;
  CMOCK_gsmParseTkn_CALLBACK gsmParseTkn_CallbackFunctionPointer;
  int gsmParseTkn_CallbackCalls;
  CMOCK_MEM_INDEX_TYPE gsmParseTkn_CallInstance;
} Mock;

extern jmp_buf AbortFrame;
extern int GlobalExpectCount;
extern int GlobalVerifyOrder;

void mock_gsmParser_Verify(void)
{
  UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
  if (Mock.gsmParseTkn_IgnoreBool)
    Mock.gsmParseTkn_CallInstance = CMOCK_GUTS_NONE;
  UNITY_SET_DETAIL(CMockString_gsmParseTkn);
  UNITY_TEST_ASSERT(CMOCK_GUTS_NONE == Mock.gsmParseTkn_CallInstance, cmock_line, CMockStringCalledLess);
  if (Mock.gsmParseTkn_CallbackFunctionPointer != NULL)
    Mock.gsmParseTkn_CallInstance = CMOCK_GUTS_NONE;
}

void mock_gsmParser_Init(void)
{
  mock_gsmParser_Destroy();
}

void mock_gsmParser_Destroy(void)
{
  CMock_Guts_MemFreeAll();
  memset(&Mock, 0, sizeof(Mock));
  Mock.gsmParseTkn_CallbackFunctionPointer = NULL;
  Mock.gsmParseTkn_CallbackCalls = 0;
  GlobalExpectCount = 0;
  GlobalVerifyOrder = 0;
}

tknTypeParser_e gsmParseTkn(uint8_t const* const tkn, uint8_t* cmd, uint8_t* par, uint16_t tknLen)
{
  UNITY_LINE_TYPE cmock_line = TEST_LINE_NUM;
  CMOCK_gsmParseTkn_CALL_INSTANCE* cmock_call_instance;
  UNITY_SET_DETAIL(CMockString_gsmParseTkn);
  cmock_call_instance = (CMOCK_gsmParseTkn_CALL_INSTANCE*)CMock_Guts_GetAddressFor(Mock.gsmParseTkn_CallInstance);
  Mock.gsmParseTkn_CallInstance = CMock_Guts_MemNext(Mock.gsmParseTkn_CallInstance);
  if (Mock.gsmParseTkn_IgnoreBool)
  {
    UNITY_CLR_DETAILS();
    if (cmock_call_instance == NULL)
      return Mock.gsmParseTkn_FinalReturn;
    memcpy(&Mock.gsmParseTkn_FinalReturn, &cmock_call_instance->ReturnVal, sizeof(tknTypeParser_e));
    return cmock_call_instance->ReturnVal;
  }
  if (Mock.gsmParseTkn_CallbackFunctionPointer != NULL)
  {
    return Mock.gsmParseTkn_CallbackFunctionPointer(tkn, cmd, par, tknLen, Mock.gsmParseTkn_CallbackCalls++);
  }
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringCalledMore);
  cmock_line = cmock_call_instance->LineNumber;
  if (cmock_call_instance->CallOrder > ++GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledEarly);
  if (cmock_call_instance->CallOrder < GlobalVerifyOrder)
    UNITY_TEST_FAIL(cmock_line, CMockStringCalledLate);
  {
    UNITY_SET_DETAILS(CMockString_gsmParseTkn,CMockString_tkn);
    if (cmock_call_instance->Expected_tkn == NULL)
      { UNITY_TEST_ASSERT_NULL(tkn, cmock_line, CMockStringExpNULL); }
    else
      { UNITY_TEST_ASSERT_EQUAL_HEX8_ARRAY(cmock_call_instance->Expected_tkn, tkn, 1, cmock_line, CMockStringMismatch); }
  }
  {
    UNITY_SET_DETAILS(CMockString_gsmParseTkn,CMockString_cmd);
    if (cmock_call_instance->Expected_cmd == NULL)
      { UNITY_TEST_ASSERT_NULL(cmd, cmock_line, CMockStringExpNULL); }
    else
      { UNITY_TEST_ASSERT_EQUAL_HEX8_ARRAY(cmock_call_instance->Expected_cmd, cmd, 1, cmock_line, CMockStringMismatch); }
  }
  {
    UNITY_SET_DETAILS(CMockString_gsmParseTkn,CMockString_par);
    if (cmock_call_instance->Expected_par == NULL)
      { UNITY_TEST_ASSERT_NULL(par, cmock_line, CMockStringExpNULL); }
    else
      { UNITY_TEST_ASSERT_EQUAL_HEX8_ARRAY(cmock_call_instance->Expected_par, par, 1, cmock_line, CMockStringMismatch); }
  }
  {
    UNITY_SET_DETAILS(CMockString_gsmParseTkn,CMockString_tknLen);
    UNITY_TEST_ASSERT_EQUAL_HEX16(cmock_call_instance->Expected_tknLen, tknLen, cmock_line, CMockStringMismatch);
  }
  UNITY_CLR_DETAILS();
  return cmock_call_instance->ReturnVal;
}

void CMockExpectParameters_gsmParseTkn(CMOCK_gsmParseTkn_CALL_INSTANCE* cmock_call_instance, uint8_t const* const tkn, uint8_t* cmd, uint8_t* par, uint16_t tknLen)
{
  cmock_call_instance->Expected_tkn = tkn;
  cmock_call_instance->Expected_cmd = cmd;
  cmock_call_instance->Expected_par = par;
  cmock_call_instance->Expected_tknLen = tknLen;
}

void gsmParseTkn_CMockIgnoreAndReturn(UNITY_LINE_TYPE cmock_line, tknTypeParser_e cmock_to_return)
{
  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_gsmParseTkn_CALL_INSTANCE));
  CMOCK_gsmParseTkn_CALL_INSTANCE* cmock_call_instance = (CMOCK_gsmParseTkn_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
  Mock.gsmParseTkn_CallInstance = CMock_Guts_MemChain(Mock.gsmParseTkn_CallInstance, cmock_guts_index);
  Mock.gsmParseTkn_IgnoreBool = (int)0;
  cmock_call_instance->LineNumber = cmock_line;
  cmock_call_instance->ReturnVal = cmock_to_return;
  Mock.gsmParseTkn_IgnoreBool = (int)1;
}

void gsmParseTkn_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, uint8_t const* const tkn, uint8_t* cmd, uint8_t* par, uint16_t tknLen, tknTypeParser_e cmock_to_return)
{
  CMOCK_MEM_INDEX_TYPE cmock_guts_index = CMock_Guts_MemNew(sizeof(CMOCK_gsmParseTkn_CALL_INSTANCE));
  CMOCK_gsmParseTkn_CALL_INSTANCE* cmock_call_instance = (CMOCK_gsmParseTkn_CALL_INSTANCE*)CMock_Guts_GetAddressFor(cmock_guts_index);
  UNITY_TEST_ASSERT_NOT_NULL(cmock_call_instance, cmock_line, CMockStringOutOfMemory);
  memset(cmock_call_instance, 0, sizeof(*cmock_call_instance));
  Mock.gsmParseTkn_CallInstance = CMock_Guts_MemChain(Mock.gsmParseTkn_CallInstance, cmock_guts_index);
  Mock.gsmParseTkn_IgnoreBool = (int)0;
  cmock_call_instance->LineNumber = cmock_line;
  cmock_call_instance->CallOrder = ++GlobalExpectCount;
  CMockExpectParameters_gsmParseTkn(cmock_call_instance, tkn, cmd, par, tknLen);
  memcpy(&cmock_call_instance->ReturnVal, &cmock_to_return, sizeof(tknTypeParser_e));
  UNITY_CLR_DETAILS();
}

void gsmParseTkn_StubWithCallback(CMOCK_gsmParseTkn_CALLBACK Callback)
{
  Mock.gsmParseTkn_IgnoreBool = (int)0;
  Mock.gsmParseTkn_CallbackFunctionPointer = Callback;
}

