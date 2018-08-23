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

#include "gsmParser.h"

/*==================[macros and definitions]=================================*/

#define DEBUG_PARSER
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

tknTypeParser_e gsmParseTkn(uint8_t const * const tkn, uint8_t * cmd,
                            uint8_t * par, uint16_t tknLen)
{
   int i = 0; /* loop counter */

   uint8_t equalPos = 0; /* position of the '=' char in tkn, if present */
   uint8_t intPos = 0;   /* position of the '?' char in tkn, if present */
   uint8_t colonPos = 0; /* position of the ':' char in tkn, if present */

   /* In all cases we determine which sort of token we have and we copy the
    * appropiate parts of the token into the command and parameter buffers */

   cmd[0] = '\0';
   par[0] = '\0';

   /* The last character of the token has the tokenizer token type encoded. We
    * extract it and make a first classification before trying to determine
    * the parser token type.
    */

   tknTypeTknzer_e tknType;
   tknType = (tknTypeTknzer_e)tkn[tknLen-1];
   tknLen--; /* discard this last character for string search purposes */

   /* In the following code we analize the token and try to divide it into a
    * cmd and par (if present). For all purposes the leading and trailing \r
    * and \n characters are ignored.
    */

   switch(tknType){

      case ECHO: /* token should be the echo of an AT command sent by us;
                    the last character is by definition \r */

         if(('A' == tkn[0]) && ('T' == tkn[1])){

            /* autobauding sync sequence (AT) */

            if('\r' == tkn[2]){
               strncpy(cmd,"AT\0",3);

               debug(">>>parser<<<   AUTOBAUD\r\n");

               return AUTOBAUD;
            }

            /* extended AT command */

            else if('+' == tkn[2]){

               /* Search for '=' and '?' characters and store position to
                * determine type of extended AT command */

               for (i = 3; i < tknLen; i++){
                  if ('=' == tkn[i]){
                     equalPos = i;
                     break;
                  }
               }

               for (i = 3; i < tknLen; i++){
                  if ('?' == tkn[i]){
                     intPos = i;
                     break;
                  }
               }

               /* Determine the type of extended command (TEST, READ, WRITE or
                * EXECUTION) depending on the position of the '=' and '?'
                * characters. Afterwards, copy the correct part of the token
                * corresponding to the command and parameter */

               if (0 != equalPos){

                  /* copy the part between '+' and '=' to cmd */
                  strncpy(cmd,&tkn[3],(equalPos - 3));
                  cmd[equalPos -3] = '\0';

                  /* TEST extended command (AT+...=?) */
                  if((equalPos+1) == intPos){

                     debug(">>>parser<<<   EXTENDED COMMAND TEST: ");
                     debug(cmd);
                     debug("\r\n");

                     return EXT_CMD_TEST;
                  }

                  /* WRITE extended command (AT+...=...) */
                  else{
                     /* copy the part between '=' and the end to par */
                     strncpy(par,&tkn[equalPos+1],tknLen-equalPos-2);
                     par[tknLen-equalPos-2] = '\0';

                     debug(">>>parser<<<   EXTENDED COMMAND WRITE: ");
                     debug(cmd);
                     debug("(");
                     debug(par);
                     debug(")\r\n");

                     return EXT_CMD_WRITE;
                  }

               }

               /* READ extended command (AT+...?) */
               else if((tknLen-2) == intPos){

                  /* copy the part between '+' and '?' to cmd*/
                  strncpy(cmd,&tkn[3],(intPos - 3));
                  cmd[intPos -3] = '\0';

                  debug(">>>parser<<<   EXTENDED COMMAND READ: ");
                  debug(cmd);
                  debug("\r\n");

                  return EXT_CMD_READ;
               }

               /* EXECUTE extended command (AT+...) */
               else{

                  /* copy everything after '+' to cmd */
                  strncpy(cmd,&tkn[3],(tknLen - 4));
                  cmd[tknLen -4] = '\0';

                  debug(">>>parser<<<   EXTENDED COMMAND EXEC: ");
                  debug(cmd);
                  debug("\r\n");

                  return EXT_CMD_EXEC;
               }
            }

            /* basic AT command with ampersand (AT&...) */

            else if('&' == tkn[2]){

               if('\r' != tkn[3]){
                  /* first char after the & is the cmd */
                  cmd[0] = tkn[3];
                  cmd[1] = '\0';

                  debug(">>>parser<<<   AT COMMAND W/&: ");
                  debug(cmd);

                  if('\r' != tkn[4]){
                     /* if present, everything after cmd char is the par */
                     strncpy(par,&tkn[4],tknLen-4);
                     par[tknLen-4] = '\0';

                     debug("(");
                     debug(par);
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

            /* basic AT command (AT...) */

            else {
               /* first char after AT is the cmd */
               cmd[0] = tkn[2];
               cmd[1] = '\0';

               debug(">>>parser<<<   AT COMMAND: ");
               debug(cmd);

               if('\r' != tkn[3]){
                  /* if present, everything after cmd char is the par */
                  strncpy(par,&tkn[3],tknLen-3);
                  par[tknLen-3] = '\0';

                  debug("(");
                  debug(par);
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

      break;

      case RSP:
      case SMS_PROMPT: /* token should be a response by the GSM modem, be it a
                          basic or extended command response or a SMS prompt;
                          the token starts and ends with \r\n */

         /* Extended syntax response */

         if('+' == tkn[2]){

            /* Search for ':' character and store position if present */

            for(i = 3; i < (tknLen-2); i++){
               if(':' == tkn[i]){
                  colonPos = i;
                  break;
               }
            }

            /* If no ':' character is present we have a simple extended sintax
             * response (+...) */

            if(0 == colonPos){
               /* everything after the + is the cmd */
               strncpy(cmd,&tkn[3],tknLen-5);
               cmd[tknLen-5] = '\0';

               debug(">>>parser<<<   EXTENDED RESPONSE: ");
               debug(cmd);
               debug("\r\n");

               return EXT_RSP;
            }

            /* If ':' character is present, what follows is a parameter of
             * the response (+...:...) */

            else{
               /* everything between the + and the : is the cmd */
               strncpy(cmd,&tkn[3],colonPos-3);
               cmd[colonPos-3] = '\0';
               /* everything between the : and the end is the par */
               strncpy(par,&tkn[colonPos+1],tknLen-colonPos-3);
               par[tknLen-colonPos-3] = '\0';

               debug(">>>parser<<<   EXTENDED RESPONSE: ");
               debug(cmd);
               debug("(");
               debug(par);
               debug(")\r\n");

               return EXT_RSP;
            }

         }

         /* SMS Prompt (> ) */

         else if('>' == tkn[2] && ' ' == tkn[3]){
            strncpy(cmd,"> \0",3);

            debug(">>>parser<<<   SMS PROMPT\r\n");

            return SMS_PROMPT_P;
         }

         /* Basic response */

         else{
            /* the whole token is the response */
            strncpy(cmd,&tkn[2],tknLen-4);
            cmd[tknLen-4] = '\0';

            debug(">>>parser<<<   BASIC RESPONSE: ");
            debug(cmd);
            debug("\r\n");

            return BASIC_RSP;
         }

      break;

      case DATA_BLOCK:

         /* we enter DATA as the cmd */
         strncpy(cmd,"DATA\0",5);
         /* the whole token is the parameter */
         strncpy(par,tkn,tknLen-2);
         par[tknLen-2] = '\0';

         debug(">>>parser<<<   DATA BLOCK: ");
         debug(par);
         debug("\r\n");

         return DATA_BLOCK_P;

      break;

      case SMS_BODY:
      default:

         /* we enter SMS_BODY as the cmd */
         strncpy(cmd,"SMS_BODY\0",9);
         /* the whole token is the parameter */
         strncpy(par,&tkn[0],tknLen);
         par[tknLen] = '\0';

         debug(">>>parser<<<   SMS BODY: ");
         debug(par);
         debug("\r\n");

         return SMS_BODY_P;

      break;

   }

}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
