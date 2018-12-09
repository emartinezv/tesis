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

/** \addtogroup gsm
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

/** The function takes a pointer to token, two buffers (cmd and par) and the
 *  length of the token. In the AT protocol, most tokens have a command part
 *  and optional parameters which specify variable input. Not all tokens have
 *  parameters, but all of them have a command part.
 *
 *  This function returns the token type plus the command part in cmd and the
 *  parameter part (if present) in par. It turns the "raw" token we get from
 *  the tokenizer into something usable for the Engine layer.
 */

tknTypeParser_e gsmParseTkn(uint8_t const * const tkn, uint8_t * cmd,
                            uint8_t * par, uint16_t tknLen)
{
   int i = 0; /* loop counter */

   /* In all cases we classify the token and then copy the appropiate parts of
    * the token into the command and parameter buffers, depending on the
    * specific token syntax. */

   cmd[0] = '\0';
   par[0] = '\0';

   /* The last character of the token has the tokenizer token type encoded.
    * This is a basic classification which we further refine in this function.
    * We extract this basic token type by taking the last char and feeding
    * a switch statement with it. Each switch option then detects a more
    * specific sub-class of token.
    */

   tknTypeTknzer_e tknType;
   tknType = (tknTypeTknzer_e)tkn[tknLen-1];
   tknLen--; /* discard this last character for string search purposes */

   /* Different sorts of tokens have trailing and/or leading '\r' and/or '\n'
    * characters. For all purposes these are ignored and not copied into the
    * cmd and par buffers.
    *
    * There is extensive use of the strncpy function in the following code. To
    * understand the "from" and "length" arguments used, it is important to
    * know the type of token. We always need to discard any leading '\r' and
    * 'n' characters.
    */

   switch(tknType){

      case ECHO: /* The token should be the echo of an AT command sent by us;
                    the last character is always \r */

         /* All AT commands start with "AT", so we check this first. If we pass
          * this check, we determine what specific sort of AT command we have
          * depending on the following chars. */

         if(('A' == tkn[0]) && ('T' == tkn[1])){

            /* If the token is just "AT\r" then we have the autobauding sync
             * sequence */

            if('\r' == tkn[2]){
               /* Simply copy "AT" to cmd */
               strncpy(cmd,"AT",2);
               cmd[2] = '\0';

               debug(">>>parser<<<   AUTOBAUD\r\n");

               return AUTOBAUD;
            }

            /* If the third character is a '+' we have an extended AT command,
             * written "AT+cmd...\r". The variable part may include the '='
             * and '?' characters. */

            else if('+' == tkn[2]){

               /* Search for '=' and '?' characters and store position to
                * determine type of extended AT command */

               uint8_t equalPos = 0; /* position of the '=' char in tkn */
               uint8_t intPos = 0;   /* position of the '?' char in tkn */

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
                * EXECUTION) depending on the presence and position of the '='
                * and '?' characters. Afterwards, copy the correct part of the
                * token corresponding to the command and parameter */

               /* If token is written "AT+cmd=...\r" it can be a TEST or WRITE
                * extended command */
               if (0 != equalPos){

                  /* Copy the part between '+' and '=' to cmd */
                  strncpy(cmd,&tkn[3],(equalPos - 3));
                  cmd[equalPos -3] = '\0';

                  /* If the token is written "AT+cmd=?\r" we have a TEST
                   * extended command */
                  if((equalPos+1) == intPos){

                     debug(">>>parser<<<   EXTENDED COMMAND TEST: ");
                     debug(cmd);
                     debug("\r\n");

                     return EXT_CMD_TEST;
                  }

                  /* If the token is written "AT+cmd=par\r" we have a WRITE
                   * extended command*/
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

               /* If the token is written is written "AT+cmd?\r" we have a
                * READ extended command */
               else if((tknLen-2) == intPos){

                  /* Copy the part between '+' and '?' to cmd*/
                  strncpy(cmd,&tkn[3],(intPos - 3));
                  cmd[intPos -3] = '\0';

                  debug(">>>parser<<<   EXTENDED COMMAND READ: ");
                  debug(cmd);
                  debug("\r\n");

                  return EXT_CMD_READ;
               }

               /* If the token is written "AT+cmd\r" we have a EXECUTE extended
                * command */
               else{

                  /* Copy everything after '+' to cmd */
                  strncpy(cmd,&tkn[3],(tknLen - 4));
                  cmd[tknLen -4] = '\0';

                  debug(">>>parser<<<   EXTENDED COMMAND EXEC: ");
                  debug(cmd);
                  debug("\r\n");

                  return EXT_CMD_EXEC;
               }
            }

            /* If the third character is a '&' we have a basic AT
             * command with ampersand written "AT&...\r" */

            else if('&' == tkn[2]){

               if('\r' != tkn[3]){
                  /* The first char after the '&' is the cmd */
                  cmd[0] = tkn[3];
                  cmd[1] = '\0';

                  debug(">>>parser<<<   AT COMMAND W/&: ");
                  debug(cmd);

                  if('\r' != tkn[4]){
                     /* If present, everything after the cmd char is the par */
                     strncpy(par,&tkn[4],tknLen-4);
                     par[tknLen-4] = '\0';

                     debug("(");
                     debug(par);
                     debug(")");
                  }

                  debug("\r\n");

                  return BASIC_CMD_AMP;

               }

               /* If the token is just "AT&" there is no command it is invalid
                */

               else{

                  debug(">>>parser<<<   INVALID TOKEN\r\n");

                  return INVALID;

               }

            }

            /* If we have neither '+' nor '&' as the third character, then this
             * is just a basic AT command written "AT...\r" */

            else {
               /* The first char after "AT" is the cmd */
               cmd[0] = tkn[2];
               cmd[1] = '\0';

               debug(">>>parser<<<   AT COMMAND: ");
               debug(cmd);

               if('\r' != tkn[3]){
                  /* If present, everything after the cmd char is the par */
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

         /* If an ECHO token does not start with AT it cannot be the echo of
          * a command and is therefore invalid
          */

         else{

            debug(">>>parser<<<   INVALID TOKEN\r\n");

            return INVALID;

         }

      break;

      case RSP:
      case SMS_PROMPT: /* The token should be a response by the GSM modem,
                          be it a basic or extended command response or a SMS
                          prompt; the token starts with "\r\n" and ends with
                          either "\r\n" (response) or "> " (SMS prompt) */

         /* If '+' is the first char after "\r\n", we have an extended syntax
          * response written "\r\n+cmd...\r\n" */

         if('+' == tkn[2]){

            /* The extended command response may have a ':' character; we look
             * for it and store its position if present */

            uint8_t colonPos = 0; /* position of the ':' char in tkn */

            for(i = 3; i < (tknLen-2); i++){
               if(':' == tkn[i]){
                  colonPos = i;
                  break;
               }
            }

            /* If no ':' character is present we have a simple extended sintax
             * response written "\r\n+cmd\r\n" */

            if(0 == colonPos){
               /* Everything after the + is the cmd */
               strncpy(cmd,&tkn[3],tknLen-5);
               cmd[tknLen-5] = '\0';

               debug(">>>parser<<<   EXTENDED RESPONSE: ");
               debug(cmd);
               debug("\r\n");

               return EXT_RSP;
            }

            /* If ':' character is present, what follows is a parameter of
             * the response written "\r\n+cmd:par\r\n" */

            else{
               /* Everything between the + and the : is the cmd */
               strncpy(cmd,&tkn[3],colonPos-3);
               cmd[colonPos-3] = '\0';
               /* Everything between the : and the end is the par */
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

         /* The SMS prompt is always written "\r\n> " */

         else if('>' == tkn[2] && ' ' == tkn[3]){
            strncpy(cmd,"> ",2);
            cmd[2] = '\0';

            debug(">>>parser<<<   SMS PROMPT\r\n");

            return SMS_PROMPT_P;
         }

         /* If there is no '+' character or a SMS prompt, we have a basic
          * response written "\r\ncmd\r\n" */

         else{
            /* The whole token excepting the "\r\n" parts is the cmd */
            strncpy(cmd,&tkn[2],tknLen-4);
            cmd[tknLen-4] = '\0';

            debug(">>>parser<<<   BASIC RESPONSE: ");
            debug(cmd);
            debug("\r\n");

            return BASIC_RSP;
         }

      break;

      case DATA_BLOCK: /* DATA blocks are usually used in reporting SMS content
                          and other chunks of data. They end in \r\n.
                       */

         /* We enter DATA as the cmd */
         strncpy(cmd,"DATA",4);
         cmd[4]='\0';
         /* The whole rest of the token excepting the trailing "\r\n" is the
          * parameter */
         strncpy(par,tkn,tknLen-2);
         par[tknLen-2] = '\0';

         debug(">>>parser<<<   DATA BLOCK: ");
         debug(par);
         debug("\r\n");

         return DATA_BLOCK_P;

      break;

      case SMS_BODY: /* SMS body is the text entered after the SMS prompt by
                        the user, written "...\0x1A" (\0x1A is the escape
                        sequence for Ctrl-Z). However, when sent the sendATcmd
                        adds \0x1A on it's own and the modem does not echo the
                        \0x1A character, so it will not appear in the token.

                        Since it has no intrinsic structure, we assume that a
                        token is SMS body by default if it fits no other token
                        type. The upper layers will handle this if it is an
                        erroneous assumption.
                     */
      default:

         /* Ee enter SMS_BODY as the cmd */
         strncpy(cmd,"SMS_BODY",8);
         cmd[8]='\0';
         /* The whole token is the parameter */
         strncpy(par,tkn,tknLen);
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
