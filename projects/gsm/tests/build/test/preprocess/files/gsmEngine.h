#include "gsmTokenizer.h"
#include "gsmCommands.h"
#include "gsmParser.h"
#include "gsmComms.h"
#include "lpc_types.h"
typedef struct _rsp

{

   uint8_t cmd[20 +1];

   uint8_t par[280 +1];

} rsp_t;







typedef enum _serialMode

{

   COMMAND_MODE,

   DATA_MODE

} serialMode_t ;







typedef enum _fsmState

{

   WAITING = 0,

   CMD_SENT = 1,

   CMD_ACK = 2,

} fsmState_t;







typedef enum _fsmEvent

{

   NO_UPDATE,

   OK_CMD_SENT,

   OK_CMD_ACK,

   OK_RSP,

   OK_URC,

   OK_CLOSE,

   ERR_CMD_UKN,

   ERR_CMD_ECHO,

   ERR_OOO,

   ERR_TKN_INV,

   ERR_MSG_CLOSE,

   ERR_TIMEOUT,

   ERR_FSM_OOR

} fsmEvent_t;







typedef struct _gsmEngine

{





   fsmState_t fsmState;

   serialMode_t serialMode;

   uint8_t currCmd[20];

   uint8_t currPar[280];

   uint8_t currIdx;







   uint32_t toutCnt;







   VLRINGBUFF_T tknVlRb;

   VLRINGBUFF_T rspVlRb;

   VLRINGBUFF_T urcVlRb;







   RINGBUFF_T tknRb;

   RINGBUFF_T rspRb;

   RINGBUFF_T urcRb;







   uint8_t tknRbBuf[1024];

   uint8_t rspRbBuf[1024];

   uint8_t urcRbBuf[256];



} gsmEngine_t;

_Bool gsmInitEngine(gsmEngine_t * const engine);

fsmEvent_t gsmProcessTkn(gsmEngine_t * const engine);

_Bool gsmToutCntZero(gsmEngine_t * const engine);















void gsmDecToutCnt(gsmEngine_t * const engine);

fsmEvent_t gsmSendCmd (gsmEngine_t * const engine,

                       const uint8_t * const cmdStr);

rsp_t gsmGetCmdRsp (gsmEngine_t * const engine);

uint8_t gsmGetNoCmdRsp (const gsmEngine_t * const engine);

rsp_t gsmGetUrc (gsmEngine_t * const engine);

serialMode_t gsmGetSerialMode(const gsmEngine_t * const engine);

_Bool gsmSetSerialMode(gsmEngine_t * const engine, serialMode_t mode);
