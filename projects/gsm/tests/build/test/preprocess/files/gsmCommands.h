#include "lpc_types.h"
uint16_t gsmCmdSearch(uint8_t const * const cmd);

const uint8_t const * gsmGetCmdSucRsp (uint16_t idx);

const uint8_t const * gsmGetCmdErrRsp (uint16_t idx);

uint32_t gsmGetCmdTimeout (uint16_t idx);

_Bool gsmUrcSearch(uint8_t const * const urc);
