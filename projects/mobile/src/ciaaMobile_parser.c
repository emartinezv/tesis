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

/** @brief This module handles AT token parsing
 */

/** \addtogroup parser parser
 ** @{ */

/*==================[inclusions]=============================================*/

#include "ciaaMobile_parser.h"

/*==================[macros and definitions]=================================*/

#define DEBUG_PARSER // debug mode
#ifdef DEBUG_PARSER
   #define debug(msg) dbgPrint(msg)
#else
   #define debug(msg)
#endif

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/

/** The parse function takes a pointer to token and two buffers (command and
 *  parameter)and returns the token type plus the corresponding parts in
 *  each buffer. These results are later used to feed the command FSM.
 */

ATToken parse(uint8_t const * const token, uint8_t * command, uint8_t * parameter, int tknlen)
{
   int i = 0; /* loop counter */

   uint8_t equalPos = 0; /* position of the '=' char in the token, if present */
   uint8_t intPos = 0;   /* position of the '?' char in the token, if present */
   uint8_t colonPos = 0; /* position of the ':' char in the token, if present */

   /* In all cases we determine which sort of token we have and we copy the
    * appropiate parts of the token into the command and parameter buffers */

   command[0] = '\0';
   parameter[0] = '\0';

   /* determine if the token is a response or an echo */

   if(('\r' == token[0]) && ('\n' == token[1])){ /* token is a response */

      /* Extended syntax response */

      if('+' == token[2]){

         /* Search for ':' character and store position if present */

         for(i = 3; i < (tknlen-2); i++){
            if(':' == token[i]){
               colonPos = i;
               break;
            }
         }

         /* If no ':' character is present we have a simple extended sintax response */

         if(0 == colonPos){
            strncpy(command,&token[3],tknlen-5);
            command[tknlen-5] = '\0';

            debug(">>>parser<<<   EXTENDED RESPONSE: ");
            debug(command);
            debug("\r\n");

            return EXT_RSP;
         }

         /* If ':' character is present, what follows is a parameter of the response */

         else{
            strncpy(command,&token[3],colonPos-3);
            command[colonPos-3] = '\0';
            strncpy(parameter,&token[colonPos+1],tknlen-colonPos-3);
            parameter[tknlen-colonPos-3] = '\0';

            debug(">>>parser<<<   EXTENDED RESPONSE: ");
            debug(command);
            debug("(");
            debug(parameter);
            debug(")\r\n");

            return EXT_RSP;
         }

      }

      /* SMS Prompt */

      else if( '>' == token[2] && ' ' == token[3]){
         strncpy(command,"> \0",3);

         debug(">>>parser<<<   SMS PROMPT\r\n");

         return SMS_PROMPT;
      }

      /* Basic response */

      else{
         strncpy(command,&token[2],tknlen-4);
         command[tknlen-4] = '\0';

         debug(">>>parser<<<   BASIC RESPONSE: ");
         debug(command);
         debug("\r\n");

         return BASIC_RSP;
      }
   }

   else if('\r' == token[tknlen-1]){ /* token is an echo */

      /* determine if the token is an AT command, be it extended or basic */

      if(('A' == token[0]) && ('T' == token[1])){

         /* autobauding sync sequence */

         if('\r' == token[2]){
            strncpy(command,"AT\0",3);

            debug(">>>parser<<<   AUTOBAUD\r\n");

            return AUTOBAUD;
         }

         /* extended AT command */

         else if('+' == token[2]){

            /* Search for '=' and '?' characters and store position to determine type
               of extended AT command */

            for (i = 3; i < tknlen; i++){
               if ('=' == token[i]){
                  equalPos = i;
                  break;
               }
            }

            for (i = 3; i < tknlen; i++){
               if ('?' == token[i]){
                  intPos = i;
                  break;
               }
            }

            /* Determine the type of extended command (TEST, READ, WRITE or EXECUTION)
               depending on the position of the '=' and '?' characters. Afterwards,
               copy the correct part of the token corresponding to the command. */

            if (0 != equalPos){

               strncpy(command,&token[3],(equalPos - 3)); /* copy the part between '+' and '=' */
               command[equalPos -3] = '\0';

               if((equalPos+1) == intPos){

                  debug(">>>parser<<<   EXTENDED COMMAND TEST: ");
                  debug(command);
                  debug("\r\n");

                  return EXT_CMD_TEST;
               }

               else{
                  strncpy(parameter,&token[equalPos+1],tknlen-equalPos-2);
                  parameter[tknlen-equalPos-2] = '\0';

                  debug(">>>parser<<<   EXTENDED COMMAND WRITE: ");
                  debug(command);
                  debug("(");
                  debug(parameter);
                  debug(")\r\n");

                  return EXT_CMD_WRITE;
               }

            }

            else if((tknlen-2) == intPos){
               strncpy(command,&token[3],(intPos - 3)); /* copy the part between '+' and '?' */
               command[intPos -3] = '\0';

               debug(">>>parser<<<   EXTENDED COMMAND READ: ");
               debug(command);
               debug("\r\n");

               return EXT_CMD_READ;
            }

            else{
               strncpy(command,&token[3],(tknlen - 4)); /* copy everything after '+' */
               command[tknlen -4] = '\0';

               debug(">>>parser<<<   EXTENDED COMMAND EXEC: ");
               debug(command);
               debug("\r\n");

               return EXT_CMD_EXEC;
            }
         }

         /* basic AT command with ampersand */

         else if('&' == token[2]){

            if('\r' != token[3]){
               command[0] = token[3];
               command[1] = '\0';

               debug(">>>parser<<<   AT COMMAND W/&: ");
               debug(command);

               if('\r' != token[4]){
                  strncpy(parameter,&token[4],tknlen-4);
                  parameter[tknlen-4] = '\0';

                  debug("(");
                  debug(parameter);
                  debug(")");
               }

               debug("\r\n");

               return BASIC_CMD_AMP;

            }

            else{

               debug(">>>parser<<<   INVALID TOKEN\r\n");

               return INVALID;

            }

         }

         /* basic AT command */

         else {
            command[0] = token[2];
            command[1] = '\0';

            debug(">>>parser<<<   AT COMMAND: ");
            debug(command);

            if('\r' != token[3]){
               strncpy(parameter,&token[3],tknlen-3);
               parameter[tknlen-3] = '\0';

               debug("(");
               debug(parameter);
               debug(")");
            }

            debug("\r\n");

            return BASIC_CMD;

         }

      }

      else{

         debug(">>>parser<<<   INVALID TOKEN\r\n");

         return INVALID;

      }
   }

   else if(('\r' == token[tknlen-2]) && ('\n' == token[tknlen-1])){ /* token is a <data> block */

      strncpy(command,"DATA\0",5);
      strncpy(parameter,token,tknlen-2);
      parameter[tknlen-2] = '\0';

      debug(">>>parser<<<   DATA BLOCK: ");
      debug(parameter);
      debug("\r\n");

      return DATA;
   }

   else{ /* token is SMS Body echo or error */

      strncpy(command,"SMS_BODY\0",9);
      strncpy(parameter,&token[0],tknlen);
      parameter[tknlen] = '\0';

      debug(">>>parser<<<   SMS BODY: ");
      debug(parameter);
      debug("\r\n");

      return SMS_BODY;

   }
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
