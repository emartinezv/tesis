/* Mock chip.f file with the bare minimum for handling UART stuff */

/*==================[inclusions]============================================*/

#include "ring_buffer.h"

/*==================[typedef]===============================================*/

/* Took the LPC_USART_T typedef from the uart_18xx_43xx.h file and copied it
 * removing all the __IO, __I and __O attributes
 */

typedef struct {              /*!< USARTn Structure       */

   union {
      uint32_t  DLL;        /*!< Divisor Latch LSB. Least significant byte of the baud rate divisor value. The full divisor is used to generate a baud rate from the fractional rate divider (DLAB = 1). */
      uint32_t  THR;        /*!< Transmit Holding Register. The next character to be transmitted is written here (DLAB = 0). */
      uint32_t  RBR;        /*!< Receiver Buffer Register. Contains the next received character to be read (DLAB = 0). */
   };

   union {
      uint32_t IER;         /*!< Interrupt Enable Register. Contains individual interrupt enable bits for the 7 potential UART interrupts (DLAB = 0). */
      uint32_t DLM;         /*!< Divisor Latch MSB. Most significant byte of the baud rate divisor value. The full divisor is used to generate a baud rate from the fractional rate divider (DLAB = 1). */
   };

   union {
      uint32_t FCR;         /*!< FIFO Control Register. Controls UART FIFO usage and modes. */
      uint32_t IIR;         /*!< Interrupt ID Register. Identifies which interrupt(s) are pending. */
   };

   uint32_t LCR;            /*!< Line Control Register. Contains controls for frame formatting and break generation. */
   uint32_t MCR;            /*!< Modem Control Register. Only present on USART ports with full modem support. */
   uint32_t LSR;            /*!< Line Status Register. Contains flags for transmit and receive status, including line errors. */
   uint32_t MSR;            /*!< Modem Status Register. Only present on USART ports with full modem support. */
   uint32_t SCR;            /*!< Scratch Pad Register. Eight-bit temporary storage for software. */
   uint32_t ACR;            /*!< Auto-baud Control Register. Contains controls for the auto-baud feature. */
   uint32_t ICR;            /*!< IrDA control register (not all UARTS) */
   uint32_t FDR;            /*!< Fractional Divider Register. Generates a clock input for the baud rate divider. */
   uint32_t OSR;            /*!< Oversampling Register. Controls the degree of oversampling during each bit time. Only on some UARTS. */
   uint32_t TER1;           /*!< Transmit Enable Register. Turns off USART transmitter for use with software flow control. */
   uint32_t  RESERVED0[3];
   uint32_t HDEN;          /*!< Half-duplex enable Register- only on some UARTs */
   uint32_t RESERVED1[1];
   uint32_t SCICTRL;        /*!< Smart card interface control register- only on some UARTs */

   uint32_t RS485CTRL;      /*!< RS-485/EIA-485 Control. Contains controls to configure various aspects of RS-485/EIA-485 modes. */
   uint32_t RS485ADRMATCH;  /*!< RS-485/EIA-485 address match. Contains the address match value for RS-485/EIA-485 mode. */
   uint32_t RS485DLY;       /*!< RS-485/EIA-485 direction control delay. */

   union {
      uint32_t SYNCCTRL;    /*!< Synchronous mode control register. Only on USARTs. */
      uint32_t FIFOLVL;     /*!< FIFO Level register. Provides the current fill levels of the transmit and receive FIFOs. */
   };

   uint32_t TER2;           /*!< Transmit Enable Register. Only on LPC177X_8X UART4 and LPC18XX/43XX USART0/2/3. */
} LPC_USART_T;
