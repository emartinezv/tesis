#include "gsmTokenizer.h"
#include "gsmComms.h"
#include "lpc_types.h"
typedef enum {INVALID,

              AUTOBAUD,

              BASIC_CMD,

              BASIC_CMD_AMP,

              EXT_CMD_TEST,

              EXT_CMD_WRITE,

              EXT_CMD_READ,

              EXT_CMD_EXEC,

              SMS_BODY_P,

              BASIC_RSP,

              DATA_BLOCK_P,

              SMS_PROMPT_P,

              EXT_RSP,

              TIMEOUT,

              }tknTypeParser_t;

tknTypeParser_t gsmParseTkn(uint8_t const * const tkn, uint8_t * cmd,

                            uint8_t * par, uint16_t tknLen);
