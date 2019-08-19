#include "build/temp/_test_gsmParser.c"
#include "mock_gsmTokenizer.h"
#include "mock_gsmComms.h"
#include "gsmParser.h"
#include "unity.h"
















void setUp(void)

{

   return;

}



void tearDown(void)

{

   return;

}











void test_gsmParseTkn(void)

{

   tknTypeParser_e tknType;



   uint8_t tkn[2*300];



   uint8_t cmd[300];

   uint8_t par[300];







   tknType = AUTOBAUD;



   strncpy(tkn,"AT&\r",strlen("AT&\r"));

   tkn[strlen("AT&\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT&\r")+1);



   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(68), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(69), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)((INVALID)), (UNITY_INT)((tknType)), (((void *)0)), (UNITY_UINT)(70), UNITY_DISPLAY_STYLE_INT);







   tknType = AUTOBAUD;



   strncpy(tkn,"abcde\r",strlen("abcde\r"));

   tkn[strlen("abcde\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("abcde\r")+1);



   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(81), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(82), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)((INVALID)), (UNITY_INT)((tknType)), (((void *)0)), (UNITY_UINT)(83), UNITY_DISPLAY_STYLE_INT);







   tknType = INVALID;



   strncpy(tkn,"AT\r",strlen("AT\r"));

   tkn[strlen("AT\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT\r")+1);



   UnityAssertEqualStringLen((const char*)(("AT")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(94));

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(95), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((AUTOBAUD)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(96), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"ATX\r",strlen("ATX\r"));

   tkn[strlen("ATX\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("ATX\r")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(107));

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(108), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((BASIC_CMD)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(109), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"ATX123\r",strlen("ATX123\r"));

   tkn[strlen("ATX123\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("ATX123\r")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(120));

   UnityAssertEqualStringLen((const char*)(("123")), (const char*)((par)), (UNITY_UINT32)((strlen(par))), (((void *)0)), (UNITY_UINT)(121));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((BASIC_CMD)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(122), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"AT&X123\r",strlen("AT&X123\r"));

   tkn[strlen("AT&X123\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT&X123\r")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(133));

   UnityAssertEqualStringLen((const char*)(("123")), (const char*)((par)), (UNITY_UINT32)((strlen(par))), (((void *)0)), (UNITY_UINT)(134));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((BASIC_CMD_AMP)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(135), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"AT+X=?\r",strlen("AT+X=?\r"));

   tkn[strlen("AT+X=?\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT+X=?\r")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(146));

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(147), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((EXT_CMD_TEST)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(148), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"AT+X=123\r",strlen("AT+X=123\r"));

   tkn[strlen("AT+X=123\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT+X=123\r")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(159));

   UnityAssertEqualStringLen((const char*)(("123")), (const char*)((par)), (UNITY_UINT32)((strlen(par))), (((void *)0)), (UNITY_UINT)(160));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((EXT_CMD_WRITE)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(161), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"AT+X?\r",strlen("AT+X?\r"));

   tkn[strlen("AT+X?\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT+X?\r")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(172));

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(173), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((EXT_CMD_READ)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(174), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"AT+X\r",strlen("AT+X\r"));

   tkn[strlen("AT+X\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT+X\r")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(185));

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(186), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((EXT_CMD_EXEC)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(187), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"abcde",strlen("abcde"));

   tkn[strlen("abcde")] = SMS_BODY;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("abcde")+1);



   UnityAssertEqualStringLen((const char*)(("SMS_BODY")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(198));

   UnityAssertEqualStringLen((const char*)(("abcde")), (const char*)((par)), (UNITY_UINT32)((strlen(par))), (((void *)0)), (UNITY_UINT)(199));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((SMS_BODY_P)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(200), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"\r\nabcde\r\n",strlen("\r\nabcde\r\n"));

   tkn[strlen("\r\nabcde\r\n")] = RSP;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("\r\nabcde\r\n")+1);



   UnityAssertEqualStringLen((const char*)(("abcde")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(211));

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(212), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((BASIC_RSP)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(213), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"abcde\r\n",strlen("abcde\r\n"));

   tkn[strlen("abcde\r\n")] = DATA_BLOCK;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("abcde\r\n")+1);



   UnityAssertEqualStringLen((const char*)(("DATA")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(224));

   UnityAssertEqualStringLen((const char*)(("abcde")), (const char*)((par)), (UNITY_UINT32)((strlen(par))), (((void *)0)), (UNITY_UINT)(225));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((DATA_BLOCK_P)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(226), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"\r\n> ",strlen("\r\n> "));

   tkn[strlen("\r\n> ")] = SMS_PROMPT;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("\r\n> ")+1);



   UnityAssertEqualStringLen((const char*)(("> ")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(237));

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(238), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((SMS_PROMPT_P)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(239), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"\r\n+X\r\n",strlen("\r\n+X\r\n"));

   tkn[strlen("\r\n+X\r\n")] = RSP;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("\r\n+X\r\n")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(250));

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(251), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((EXT_RSP)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(252), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"\r\n+X:123\r\n",strlen("\r\n+X:123\r\n"));

   tkn[strlen("\r\n+X:123\r\n")] = RSP;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("\r\n+X:123\r\n")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(263));

   UnityAssertEqualStringLen((const char*)(("123")), (const char*)((par)), (UNITY_UINT32)((strlen(par))), (((void *)0)), (UNITY_UINT)(264));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((EXT_RSP)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(265), UNITY_DISPLAY_STYLE_INT8);



   return;



}
