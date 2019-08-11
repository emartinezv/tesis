#include "ciaaUART.h"
#include "lpc_types.h"






int gsm232UartRecv (uint8_t * const buffer, int n);

int gsm232UartSend (uint8_t const * const buffer, int n);

int gsmTermUartRecv (uint8_t * const buffer, int n);

int gsmTermUartSend (uint8_t const * const buffer, int n);
