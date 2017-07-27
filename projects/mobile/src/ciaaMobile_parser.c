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

#include "ciaaMobile_parser.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

/** @brief parse function
 * @return takes a token, returns token type and writes to command and parameter
 *  buffers
 */

ATToken parse(uint8_t const * const token, uint8_t * command, uint8_t * parameter)
{
   int i = 0; /* loop counter */
   uint8_t equalPos = 0; /* position of the '=' char in the token, if present */
   uint8_t intPos = 0; /* position of the '?' char in the token, if present */
   uint8_t colonPos = 0; /* position of the ':' char in the token, if present */

   command[0] = '\0';
   parameter[0] = '\0';

   /* determine if the token is a response or an echo */

   if(('\r' == token[0]) && ('\n' == token[1])){ /* token is a response */

      /* Extended syntax response */

      if('+' == token[2]){

         /* Search for ':' character and store position if present */

         for (i = 3; i < (strlen(token)-2); i++){if (':' == token[i]) {colonPos = i;}}

         /* If no ':' character is present we have a simple extended sintax response */

         if(0 == colonPos){
            strncpy(command,&token[3],strlen(token)-5);
            command[strlen(token)-5] = '\0';
            return EXT_RSP;
         }

         /* If ':' character is present, what follows is a parameter of the response */

         else{
            strncpy(command,&token[3],colonPos-3);
            command[colonPos-3] = '\0';
            strncpy(parameter,&token[colonPos+1],strlen(token)-colonPos-2);
            parameter[strlen(token)-colonPos-2] = '\0';
            return EXT_RSP;
         }

      }

      /* Basic response */

      else{
         strncpy(command,&token[2],strlen(token)-4);
         command[strlen(token)-4] = '\0';
         return BASIC_RSP;
      }
   }

   else{ /* token is an echo */

      /* determine if the token is an AT command, be it extended or basic */

      if(('A' == token[0]) && ('T' == token[1])){

         /* autobauding sync sequence */

         if('\r' == token[2]){
            strncpy(command,"AT\0",3);
            return BASIC_CMD;
         }

         /* extended AT command */

         else if('+' == token[2]){

            /* Search for '=' and '?' characters and store position to determine type
               of extended AT command */

            for (i = 3; i < strlen(token); i++){
               if ('=' == token[i]) {equalPos = i;}
               else if ('?' == token[i]) {intPos = i;}
            }

            /* Determine the type of extended command (TEST, READ, WRITE or EXECUTION)
               depending on the position of the '=' and '?' characters. Afterwards,
               copy the correct part of the token corresponding to the command. */

            if (0 != equalPos){

               strncpy(command,&token[3],(equalPos - 3)); /* copy the part between '+' and '=' */
               command[equalPos -3] = '\0';

               if((equalPos+1) == intPos){return EXT_CMD_TEST;}

               else{
                  strncpy(parameter,&token[equalPos+1],strlen(token)-equalPos-1);
                  return EXT_CMD_WRITE;
               }

            }

            else if((strlen(token)-1) == intPos){
               strncpy(command,&token[3],(intPos - 3)); /* copy the part between '+' and '?' */
               command[intPos -3] = '\0';
               return EXT_CMD_READ;
            }

            else{
               strncpy(command,&token[3],(strlen(token) - 3)); /* copy everything after '+' */
               command[strlen(token) -3] = '\0';
               return EXT_CMD_EXEC;
            }
         }

         /* basic AT command with ampersand */

         else if('&' == token[2]){

            if('\0' != token[3]){
               command[0] = token[3];
               command[1] = '\0';

               if('\0' != token[4]){
                  strncpy(parameter,&token[4],strlen(token)-4);
                  parameter[strlen(token)-4] = '\0';
               }

               return BASIC_CMD;

            }

            else {return INVALID;}

         }

         /* basic AT command */

         else if('\0' != token[2]){
            command[0] = token[2];
            command[1] = '\0';

            if('\0' != token[3]){
               strncpy(parameter,&token[3],strlen(token)-3);
               parameter[strlen(token)-3] = '\0';
            }

            return BASIC_CMD;

         }

         else{return INVALID;}

      }

      else{

         return INVALID;

      }
   }
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
