#include "chip.h"


typedef enum _ciaaUarts_e

{

 CIAA_UART_485 = 0,

 CIAA_UART_USB = 1,

 CIAA_UART_232 = 2

}ciaaUART_e;

typedef struct _uartData

{

 LPC_USART_T * uart;

 RINGBUFF_T * rrb;

 RINGBUFF_T * trb;

}uartData_t;



void ciaaUARTInit(void);

int uartSend(ciaaUART_e nUART, void * data, int datalen);

int uartRecv(ciaaUART_e nUART, void * data, int datalen);
