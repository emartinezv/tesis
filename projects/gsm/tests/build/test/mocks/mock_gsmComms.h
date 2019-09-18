/* AUTOGENERATED FILE. DO NOT EDIT. */
#ifndef _MOCK_GSMCOMMS_H
#define _MOCK_GSMCOMMS_H

#include "gsmComms.h"

/* Ignore the following warnings, since we are copying code */
#if defined(__GNUC__) && !defined(__ICC) && !defined(__TMS470__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 6 || (__GNUC_MINOR__ == 6 && __GNUC_PATCHLEVEL__ > 0)))
#pragma GCC diagnostic push
#endif
#if !defined(__clang__)
#pragma GCC diagnostic ignored "-Wpragmas"
#endif
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wduplicate-decl-specifier"
#endif

void mock_gsmComms_Init(void);
void mock_gsmComms_Destroy(void);
void mock_gsmComms_Verify(void);




#define gsm232UartRecv_IgnoreAndReturn(cmock_retval) gsm232UartRecv_CMockIgnoreAndReturn(__LINE__, cmock_retval)
void gsm232UartRecv_CMockIgnoreAndReturn(UNITY_LINE_TYPE cmock_line, int cmock_to_return);
#define gsm232UartRecv_ExpectAnyArgsAndReturn(cmock_retval) gsm232UartRecv_CMockExpectAnyArgsAndReturn(__LINE__, cmock_retval)
void gsm232UartRecv_CMockExpectAnyArgsAndReturn(UNITY_LINE_TYPE cmock_line, int cmock_to_return);
#define gsm232UartRecv_ExpectAndReturn(buffer, n, cmock_retval) gsm232UartRecv_CMockExpectAndReturn(__LINE__, buffer, n, cmock_retval)
void gsm232UartRecv_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, uint8_t* const buffer, int n, int cmock_to_return);
typedef int (* CMOCK_gsm232UartRecv_CALLBACK)(uint8_t* const buffer, int n, int cmock_num_calls);
void gsm232UartRecv_StubWithCallback(CMOCK_gsm232UartRecv_CALLBACK Callback);
#define gsm232UartRecv_ExpectWithArrayAndReturn(buffer, buffer_Depth, n, cmock_retval) gsm232UartRecv_CMockExpectWithArrayAndReturn(__LINE__, buffer, buffer_Depth, n, cmock_retval)
void gsm232UartRecv_CMockExpectWithArrayAndReturn(UNITY_LINE_TYPE cmock_line, uint8_t* const buffer, int buffer_Depth, int n, int cmock_to_return);
#define gsm232UartRecv_ReturnThruPtr_buffer(buffer) gsm232UartRecv_CMockReturnMemThruPtr_buffer(__LINE__, buffer, sizeof(*buffer))
#define gsm232UartRecv_ReturnArrayThruPtr_buffer(buffer, cmock_len) gsm232UartRecv_CMockReturnMemThruPtr_buffer(__LINE__, buffer, (int)(cmock_len * (int)sizeof(*buffer)))
#define gsm232UartRecv_ReturnMemThruPtr_buffer(buffer, cmock_size) gsm232UartRecv_CMockReturnMemThruPtr_buffer(__LINE__, buffer, cmock_size)
void gsm232UartRecv_CMockReturnMemThruPtr_buffer(UNITY_LINE_TYPE cmock_line, uint8_t* buffer, int cmock_size);
#define gsm232UartRecv_IgnoreArg_buffer() gsm232UartRecv_CMockIgnoreArg_buffer(__LINE__)
void gsm232UartRecv_CMockIgnoreArg_buffer(UNITY_LINE_TYPE cmock_line);
#define gsm232UartRecv_IgnoreArg_n() gsm232UartRecv_CMockIgnoreArg_n(__LINE__)
void gsm232UartRecv_CMockIgnoreArg_n(UNITY_LINE_TYPE cmock_line);
#define gsm232UartSend_IgnoreAndReturn(cmock_retval) gsm232UartSend_CMockIgnoreAndReturn(__LINE__, cmock_retval)
void gsm232UartSend_CMockIgnoreAndReturn(UNITY_LINE_TYPE cmock_line, int cmock_to_return);
#define gsm232UartSend_ExpectAnyArgsAndReturn(cmock_retval) gsm232UartSend_CMockExpectAnyArgsAndReturn(__LINE__, cmock_retval)
void gsm232UartSend_CMockExpectAnyArgsAndReturn(UNITY_LINE_TYPE cmock_line, int cmock_to_return);
#define gsm232UartSend_ExpectAndReturn(buffer, n, cmock_retval) gsm232UartSend_CMockExpectAndReturn(__LINE__, buffer, n, cmock_retval)
void gsm232UartSend_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, uint8_t const* const buffer, int n, int cmock_to_return);
typedef int (* CMOCK_gsm232UartSend_CALLBACK)(uint8_t const* const buffer, int n, int cmock_num_calls);
void gsm232UartSend_StubWithCallback(CMOCK_gsm232UartSend_CALLBACK Callback);
#define gsm232UartSend_ExpectWithArrayAndReturn(buffer, buffer_Depth, n, cmock_retval) gsm232UartSend_CMockExpectWithArrayAndReturn(__LINE__, buffer, buffer_Depth, n, cmock_retval)
void gsm232UartSend_CMockExpectWithArrayAndReturn(UNITY_LINE_TYPE cmock_line, uint8_t const* const buffer, int buffer_Depth, int n, int cmock_to_return);
#define gsm232UartSend_IgnoreArg_buffer() gsm232UartSend_CMockIgnoreArg_buffer(__LINE__)
void gsm232UartSend_CMockIgnoreArg_buffer(UNITY_LINE_TYPE cmock_line);
#define gsm232UartSend_IgnoreArg_n() gsm232UartSend_CMockIgnoreArg_n(__LINE__)
void gsm232UartSend_CMockIgnoreArg_n(UNITY_LINE_TYPE cmock_line);
#define gsmTermUartRecv_IgnoreAndReturn(cmock_retval) gsmTermUartRecv_CMockIgnoreAndReturn(__LINE__, cmock_retval)
void gsmTermUartRecv_CMockIgnoreAndReturn(UNITY_LINE_TYPE cmock_line, int cmock_to_return);
#define gsmTermUartRecv_ExpectAnyArgsAndReturn(cmock_retval) gsmTermUartRecv_CMockExpectAnyArgsAndReturn(__LINE__, cmock_retval)
void gsmTermUartRecv_CMockExpectAnyArgsAndReturn(UNITY_LINE_TYPE cmock_line, int cmock_to_return);
#define gsmTermUartRecv_ExpectAndReturn(buffer, n, cmock_retval) gsmTermUartRecv_CMockExpectAndReturn(__LINE__, buffer, n, cmock_retval)
void gsmTermUartRecv_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, uint8_t* const buffer, int n, int cmock_to_return);
typedef int (* CMOCK_gsmTermUartRecv_CALLBACK)(uint8_t* const buffer, int n, int cmock_num_calls);
void gsmTermUartRecv_StubWithCallback(CMOCK_gsmTermUartRecv_CALLBACK Callback);
#define gsmTermUartRecv_ExpectWithArrayAndReturn(buffer, buffer_Depth, n, cmock_retval) gsmTermUartRecv_CMockExpectWithArrayAndReturn(__LINE__, buffer, buffer_Depth, n, cmock_retval)
void gsmTermUartRecv_CMockExpectWithArrayAndReturn(UNITY_LINE_TYPE cmock_line, uint8_t* const buffer, int buffer_Depth, int n, int cmock_to_return);
#define gsmTermUartRecv_ReturnThruPtr_buffer(buffer) gsmTermUartRecv_CMockReturnMemThruPtr_buffer(__LINE__, buffer, sizeof(*buffer))
#define gsmTermUartRecv_ReturnArrayThruPtr_buffer(buffer, cmock_len) gsmTermUartRecv_CMockReturnMemThruPtr_buffer(__LINE__, buffer, (int)(cmock_len * (int)sizeof(*buffer)))
#define gsmTermUartRecv_ReturnMemThruPtr_buffer(buffer, cmock_size) gsmTermUartRecv_CMockReturnMemThruPtr_buffer(__LINE__, buffer, cmock_size)
void gsmTermUartRecv_CMockReturnMemThruPtr_buffer(UNITY_LINE_TYPE cmock_line, uint8_t* buffer, int cmock_size);
#define gsmTermUartRecv_IgnoreArg_buffer() gsmTermUartRecv_CMockIgnoreArg_buffer(__LINE__)
void gsmTermUartRecv_CMockIgnoreArg_buffer(UNITY_LINE_TYPE cmock_line);
#define gsmTermUartRecv_IgnoreArg_n() gsmTermUartRecv_CMockIgnoreArg_n(__LINE__)
void gsmTermUartRecv_CMockIgnoreArg_n(UNITY_LINE_TYPE cmock_line);
#define gsmTermUartSend_IgnoreAndReturn(cmock_retval) gsmTermUartSend_CMockIgnoreAndReturn(__LINE__, cmock_retval)
void gsmTermUartSend_CMockIgnoreAndReturn(UNITY_LINE_TYPE cmock_line, int cmock_to_return);
#define gsmTermUartSend_ExpectAnyArgsAndReturn(cmock_retval) gsmTermUartSend_CMockExpectAnyArgsAndReturn(__LINE__, cmock_retval)
void gsmTermUartSend_CMockExpectAnyArgsAndReturn(UNITY_LINE_TYPE cmock_line, int cmock_to_return);
#define gsmTermUartSend_ExpectAndReturn(buffer, n, cmock_retval) gsmTermUartSend_CMockExpectAndReturn(__LINE__, buffer, n, cmock_retval)
void gsmTermUartSend_CMockExpectAndReturn(UNITY_LINE_TYPE cmock_line, uint8_t const* const buffer, int n, int cmock_to_return);
typedef int (* CMOCK_gsmTermUartSend_CALLBACK)(uint8_t const* const buffer, int n, int cmock_num_calls);
void gsmTermUartSend_StubWithCallback(CMOCK_gsmTermUartSend_CALLBACK Callback);
#define gsmTermUartSend_ExpectWithArrayAndReturn(buffer, buffer_Depth, n, cmock_retval) gsmTermUartSend_CMockExpectWithArrayAndReturn(__LINE__, buffer, buffer_Depth, n, cmock_retval)
void gsmTermUartSend_CMockExpectWithArrayAndReturn(UNITY_LINE_TYPE cmock_line, uint8_t const* const buffer, int buffer_Depth, int n, int cmock_to_return);
#define gsmTermUartSend_IgnoreArg_buffer() gsmTermUartSend_CMockIgnoreArg_buffer(__LINE__)
void gsmTermUartSend_CMockIgnoreArg_buffer(UNITY_LINE_TYPE cmock_line);
#define gsmTermUartSend_IgnoreArg_n() gsmTermUartSend_CMockIgnoreArg_n(__LINE__)
void gsmTermUartSend_CMockIgnoreArg_n(UNITY_LINE_TYPE cmock_line);

#if defined(__GNUC__) && !defined(__ICC) && !defined(__TMS470__)
#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 6 || (__GNUC_MINOR__ == 6 && __GNUC_PATCHLEVEL__ > 0)))
#pragma GCC diagnostic pop
#endif
#endif

#endif
