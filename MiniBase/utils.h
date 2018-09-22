#pragma once

#include "main.h"

#define HW_DLL				"hw.dll"
#define SW_DLL				"sw.dll"

#define CLIENT_DLL			"client.dll"
#define GAMEUI_DLL			"GameUI.dll"
#define MP_DLL				"mp.dll"

#define CLIENT_FIND_ERROR		"Client Find Error"
#define ENGINE_FIND_ERROR		"Engine Find Error"
#define STUDIO_FIND_ERROR		"Studio Find Error"

#define ABSOLUTE_ERROR		"Absolute Address Error"
#define OFF_ERROR			"Error Find"

#define OFF_CLIENT_PATTERN	"ScreenFade"

#define OFF_ENGINE_PATTERN	"\xFF\x15\xFF\xFF\xFF\xFF\x68\xFF\xFF\xFF\xFF\x89\x86\xFF\xFF\xFF\xFF\xFF\x15\xFF\xFF\xFF\xFF\x68\xFF\xFF\xFF\xFF\x89\x86\xFF\xFF\xFF\xFF\xFF\x15\xFF\xFF\xFF\xFF\x68\xFF\xFF\xFF\xFF\x89\x86\xFF\xFF\xFF\xFF\xFF\x15\xFF\xFF\xFF\xFF\x68\xFF\xFF\xFF\xFF\x89\x86\xFF\xFF\xFF\xFF\xFF\x15\xFF\xFF\xFF\xFF\x68\xFF\xFF\xFF\xFF\x89\x86\xFF\xFF\xFF\xFF\xFF\x15\xFF\xFF\xFF\xFF\x68\xFF\xFF\xFF\xFF\x89\x86\xFF\xFF\xFF\xFF\xFF\x15\xFF\xFF\xFF\xFF\x68\xFF\xFF\xFF\xFF\x89\x86\xFF\xFF\xFF\xFF\xFF\x15\xFF\xFF\xFF\xFF"
#define OFF_ENGINE_MASK		"xx????x????xx????xx????x????xx????xx????x????xx????xx????x????xx????xx????x????xx????xx????x????xx????xx????x????xx????xx????"

#define OFF_STUDIO_PATTERN	"Couldn't get client .dll studio model rendering interface."

#define OFF_SVC_MSG_PATTERN "-------- Message Load ---------"
#define OFF_SVC_MSG_ERROR_1 "Couldn't find SvcBase pointer #1."
#define OFF_SVC_MSG_ERROR_2 "Couldn't find SvcBase pointer #2."

#define OFF_USERMSG_PATTERN	"UserMsg: Not Present on Client %d"
#define OFF_USERMSG_ERROR_1	"Couldn't find UserMsgBase pointer #1."
#define OFF_USERMSG_ERROR_2	"Couldn't find UserMsgBase pointer #2."

#define OFF_GAME_CONSOLE_P	"GameConsole003"
#define OFF_GAME_CONSOLE_1	"Couldn't find GameConsole pointer #1."
#define OFF_GAME_CONSOLE_2	"Couldn't find GameConsole pointer #2."
#define OFF_GAME_CONSOLE_3	"Couldn't find GameConsole pointer #3."

#define OFF_MSG_READ_CORD	"MSG_ReadCoord"
#define OFF_MSG_STR_READING	"MSG_StartBitReading"
#define OFF_MSG_END_READING	"MSG_EndBitReading"
#define OFF_ENGINE_MSG_BASE	"EngineMsgBase"
#define OFF_SVC_MESSAGES_P	"End of List"

#define OFF_EVENTMSG_BASE	"CL_HookEvent:  Must provide a valid event name"
#define OFF_EVENTMSG_ERROR_1 "Couldn't find EventMsgBase pointer #1."
#define OFF_EVENTMSG_ERROR_2 "Couldn't find EventMsgBase pointer #2."

#define OFF_ERR_GAMEINFO	"Couldn't find GameInfo pointer."

PUserMsg UserMsgByName( char* szMsgName );
PEngineMsg EngineMsgByName( char* szMsgName );
PEventMsg EventMsgByName( char* szMsgName );

pcmd_t CommandByName( char* szName );

pfnUserMsgHook HookUserMsg( char *szMsgName , pfnUserMsgHook pfn );
pfnEngineMsgHook HookEngineMsg( char *szMsgName , pfnEngineMsgHook pfn );
pfnEventMsgHook HookEventMsg( char *szMsgName , pfnEventMsgHook pfn );

char* native_strcpy( char *dest , const char *src );
char* native_strcat( char *dest , const char *src );

int native_strcmp( char *pStr1 , char *pStr2 );
int native_strlen( char *pStr );

void native_memcpy( void * dst , const void * src , size_t count );
void native_memset( void *szBuffer , DWORD dwLen , DWORD dwSym );