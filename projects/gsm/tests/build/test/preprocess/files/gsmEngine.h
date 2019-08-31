#include "gsmTokenizer.h"
#include "gsmCommands.h"
#include "gsmParser.h"
#include "gsmComms.h"
#include "lpc_types.h"
typedef struct _rsp_t

{

   uint8_t cmd[20];

   uint8_t par[300];

} rsp_t;







typedef enum _serialMode_e

{

   COMMAND_MODE,

   DATA_MODE

} serialMode_e ;







typedef enum _fsmState_e

{

   WAITING = 0,

   CMD_SENT = 1,

   CMD_ACK = 2,

} fsmState_e;







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

} fsmEvent_e;







typedef struct _gsmEngine_t

{





   fsmState_e fsmState;

   serialMode_e serialMode;







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

_Bool gsmInitEngine(gsmEngine_t * engine);

fsmEvent_e gsmProcessTkn(gsmEngine_t * engine);

_Bool gsmToutCntZero(gsmEngine_t * engine);















void gsmDecToutCnt(gsmEngine_t * engine);











void gsmPrintData(void);

fsmEvent_e gsmSendCmd (gsmEngine_t * engine, const uint8_t * cmdStr);

rsp_t gsmGetCmdRsp (gsmEngine_t * engine);

uint8_t gsmGetNoCmdRsp (gsmEngine_t * engine);

rsp_t gsmGetUrc (gsmEngine_t * engine);

serialMode_e gsmGetSerialMode(gsmEngine_t * engine);

_Bool gsmSetSerialMode(gsmEngine_t * engine, serialMode_e mode);
