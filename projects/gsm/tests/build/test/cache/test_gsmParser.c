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

   tknTypeParser_t tknType;



   uint8_t tkn[2*300];



   uint8_t cmd[300];

   uint8_t par[300];

   tknType = AUTOBAUD;



   strncpy(tkn,"AT&\r",strlen("AT&\r"));

   tkn[strlen("AT&\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT&\r")+1);



   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(77), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(78), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)((INVALID)), (UNITY_INT)((tknType)), (((void *)0)), (UNITY_UINT)(79), UNITY_DISPLAY_STYLE_INT);







   tknType = AUTOBAUD;



   strncpy(tkn,"abcde\r",strlen("abcde\r"));

   tkn[strlen("abcde\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("abcde\r")+1);



   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(90), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(91), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)((INVALID)), (UNITY_INT)((tknType)), (((void *)0)), (UNITY_UINT)(92), UNITY_DISPLAY_STYLE_INT);







   tknType = INVALID;



   strncpy(tkn,"AT\r",strlen("AT\r"));

   tkn[strlen("AT\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT\r")+1);



   UnityAssertEqualStringLen((const char*)(("AT")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(103));

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(104), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((AUTOBAUD)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(105), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"ATX\r",strlen("ATX\r"));

   tkn[strlen("ATX\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("ATX\r")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(116));

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(117), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((BASIC_CMD)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(118), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"ATX123\r",strlen("ATX123\r"));

   tkn[strlen("ATX123\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("ATX123\r")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(129));

   UnityAssertEqualStringLen((const char*)(("123")), (const char*)((par)), (UNITY_UINT32)((strlen(par))), (((void *)0)), (UNITY_UINT)(130));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((BASIC_CMD)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(131), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"AT&X123\r",strlen("AT&X123\r"));

   tkn[strlen("AT&X123\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT&X123\r")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(142));

   UnityAssertEqualStringLen((const char*)(("123")), (const char*)((par)), (UNITY_UINT32)((strlen(par))), (((void *)0)), (UNITY_UINT)(143));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((BASIC_CMD_AMP)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(144), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"AT+X=?\r",strlen("AT+X=?\r"));

   tkn[strlen("AT+X=?\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT+X=?\r")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(155));

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(156), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((EXT_CMD_TEST)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(157), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"AT+X=123\r",strlen("AT+X=123\r"));

   tkn[strlen("AT+X=123\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT+X=123\r")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(168));

   UnityAssertEqualStringLen((const char*)(("123")), (const char*)((par)), (UNITY_UINT32)((strlen(par))), (((void *)0)), (UNITY_UINT)(169));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((EXT_CMD_WRITE)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(170), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"AT+X?\r",strlen("AT+X?\r"));

   tkn[strlen("AT+X?\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT+X?\r")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(181));

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(182), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((EXT_CMD_READ)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(183), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"AT+X\r",strlen("AT+X\r"));

   tkn[strlen("AT+X\r")] = ECHO;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("AT+X\r")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(194));

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(195), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((EXT_CMD_EXEC)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(196), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"abcde",strlen("abcde"));

   tkn[strlen("abcde")] = SMS_BODY;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("abcde")+1);



   UnityAssertEqualStringLen((const char*)(("SMS_BODY")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(207));

   UnityAssertEqualStringLen((const char*)(("abcde")), (const char*)((par)), (UNITY_UINT32)((strlen(par))), (((void *)0)), (UNITY_UINT)(208));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((SMS_BODY_P)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(209), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"\r\nabcde\r\n",strlen("\r\nabcde\r\n"));

   tkn[strlen("\r\nabcde\r\n")] = RSP;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("\r\nabcde\r\n")+1);



   UnityAssertEqualStringLen((const char*)(("abcde")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(220));

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(221), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((BASIC_RSP)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(222), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"abcde\r\n",strlen("abcde\r\n"));

   tkn[strlen("abcde\r\n")] = DATA_BLOCK;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("abcde\r\n")+1);



   UnityAssertEqualStringLen((const char*)(("DATA")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(233));

   UnityAssertEqualStringLen((const char*)(("abcde")), (const char*)((par)), (UNITY_UINT32)((strlen(par))), (((void *)0)), (UNITY_UINT)(234));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((DATA_BLOCK_P)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(235), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"\r\n> ",strlen("\r\n> "));

   tkn[strlen("\r\n> ")] = SMS_PROMPT;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("\r\n> ")+1);



   UnityAssertEqualStringLen((const char*)(("> ")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(246));

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(247), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((SMS_PROMPT_P)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(248), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"\r\n+X\r\n",strlen("\r\n+X\r\n"));

   tkn[strlen("\r\n+X\r\n")] = RSP;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("\r\n+X\r\n")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(259));

   UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((strlen(par))), (((void *)0)), (UNITY_UINT)(260), UNITY_DISPLAY_STYLE_INT);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((EXT_RSP)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(261), UNITY_DISPLAY_STYLE_INT8);







   tknType = INVALID;



   strncpy(tkn,"\r\n+X:123\r\n",strlen("\r\n+X:123\r\n"));

   tkn[strlen("\r\n+X:123\r\n")] = RSP;



   tknType = gsmParseTkn(tkn, cmd, par, strlen("\r\n+X:123\r\n")+1);



   UnityAssertEqualStringLen((const char*)(("X")), (const char*)((cmd)), (UNITY_UINT32)((strlen(cmd))), (((void *)0)), (UNITY_UINT)(272));

   UnityAssertEqualStringLen((const char*)(("123")), (const char*)((par)), (UNITY_UINT32)((strlen(par))), (((void *)0)), (UNITY_UINT)(273));

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((EXT_RSP)), (UNITY_INT)(UNITY_INT8 )((tknType)), (((void *)0)), (UNITY_UINT)(274), UNITY_DISPLAY_STYLE_INT8);



   return;



}
