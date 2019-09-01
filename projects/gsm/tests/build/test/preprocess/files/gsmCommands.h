#include "lpc_types.h"
typedef struct {

   uint8_t const * const name;

   uint8_t const * const sucRsp;



   uint8_t const * const errRsp;



   uint32_t timeout;

} atCmd_t;







typedef struct {

   uint8_t const * const name;

} urc_t;







extern atCmd_t const commands [];



extern urc_t const urcs [];

uint16_t gsmCmdSearch(uint8_t const * const cmd);

const uint8_t const * gsmGetCmdSucRsp (uint16_t idx);

const uint8_t const * gsmGetCmdErrRsp (uint16_t idx);

uint32_t gsmGetCmdTimeout (uint16_t idx);

uint8_t gsmUrcSearch(uint8_t const * const urc);
