/* Copyright 2016, Ezequiel Martinez Vazquez
 * All rights reserved.
 *
 * This file is part of Workspace.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** @brief This is a simple UART example
 */

/** \addtogroup uart Bare-metal example
 ** @{ */

/*==================[inclusions]=============================================*/

#include "rb_tests.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 * @return none
 */
static void initHardware(void);

/** @brief delay function
 * @param t desired milliseconds to wait
 */
static void pausems(uint32_t t);

/**
 * @brief   Initialize variable-length ring buffer
 * @param   vlrb     : Pointer to variable-length ring buffer to initialize
 * @param   RingBuff : Pointer to ring buffer to initialize
 * @param   buffer   : Pointer to buffer to associate with RingBuff
 * @param   itemSize : Size of each buffer item size
 * @param   count    : Size of ring buffer
 * @note Memory pointed by @a buffer must have correct alignment of
 *          @a itemSize, and @a count must be a power of 2 and must at
 *          least be 2 or greater.
 * @return  Nothing
 */

/*==================[internal data definition]===============================*/

/** @brief used for delay counter */
static uint32_t pausems_count;

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
   Board_Init();
   SystemCoreClockUpdate();
   SysTick_Config(SystemCoreClock / 1000);
}

static void pausems(uint32_t t)
{
   pausems_count = t;
   while (pausems_count != 0) {
      __WFI();
   }
}

/*==================[external functions definition]==========================*/

void SysTick_Handler(void)
{
   if(pausems_count > 0) pausems_count--;
}

int main(void)
{
   initHardware();
   ciaaUARTInit();

   RINGBUFF_T rb;
   uint8_t buf[BUF_SIZE];
   VLRINGBUFF_T vlrb;

   VLRingBuffer_Init(&vlrb, &rb, &buf, 1, BUF_SIZE);

   pausems(DELAY_MS);

   while(1){

      dbgPrint("\r\n>>>LISTA DE OPCIONES<<<\r\n\r\n");
      dbgPrint("1) Ingresar caracteres a VL ring buffer \r\n");
      dbgPrint("2) Leer ultimo registro del VL ring buffer \r\n");
      dbgPrint("3) Leer cantidad de items del VL ring buffer \r\n");
      dbgPrint("4) Vaciar VL ring buffer \r\n");

      uint8_t option;

      while(0 == uartRecv(CIAA_UART_USB, (void *) &option, 1)){;}

      switch(option){

         case '1':

            dbgPrint("\r\nIngrese caracteres y oprima enter para finalizar\r\n\r\n");

            uint8_t buffer[BUF_SIZE-1];
            uint8_t character = 'A';
            int i = 0;

            while(('\r' != character) && (i < (BUF_SIZE-2))){

                  if(0 != uartRecv(CIAA_UART_USB, (void *) &character, 1)){
                     buffer[i] = character;
                     buffer[i+1] = '\0';
                     dbgPrint(&buffer[i]);
                     i++;
                  }

            }

            if(0 == VLRingBuffer_Insert(&vlrb, &buffer[0], i)){
               dbgPrint("\r\nNo hay suficiente espacio\r\n");
            }

            dbgPrint("\r\n");

            break;

         case '2':

            ;

            uint8_t buffer2[BUF_SIZE-1];
            int size;

            size = VLRingBuffer_Pop(&vlrb, &buffer2[0], BUF_SIZE-2);

            if(0 != size){

               buffer2[size] = '\0';
               dbgPrint("\r\n");
               dbgPrint(buffer2);
               dbgPrint("\r\n");

            }
            else{
               dbgPrint("\r\nVL ring buffer vacio\r\n");
            }

            break;

         case '3':

            ;

            int count;
            uint8_t countstr[10];

            count = VLRingBuffer_GetCount(&vlrb);

            itoa(count, countstr, 10);

            dbgPrint("\r\nHay ");
            dbgPrint(countstr);
            dbgPrint(" elementos en el VL ring buffer\r\n");

            break;

         case '4':

            VLRingBuffer_Flush(&vlrb);

            dbgPrint("\r\nVL ring buffer vaciado\r\n");

            break;

         default:

            dbgPrint("\r\nOpcion no reconocida\r\n");

            break;

      }

   }

   return 0;

}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
