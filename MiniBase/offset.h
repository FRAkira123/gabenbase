#pragma once

#include "main.h"

class cOffset
{
public:

	DWORD HwDll , SwDll , HlMod;

	DWORD HwBase , HwSize , HwEnd;
	DWORD ClBase , ClSize , ClEnd;
	DWORD HlBase , HlSize , HlEnd;
	DWORD VgBase , VgSize , VgEnd;

	BYTE HLType;

	void GetRenderType();
	bool GetModuleInfo();

	void Error( char* Msg );

	DWORD FindClientTable();
	DWORD FindEngineTable();
	DWORD FindStudioTable();
	DWORD FindUserMsgBase();
	DWORD FindSVCMessages();
	DWORD FindEventMsgBase();
	DWORD FindGameConsole();

	void ConsoleColorInitalize();
	void GetGameInfo( pGameInfo_s GameInfo );

	void CopyClient();
	void CopyEngine();
	void CopyStudio();

	DWORD Absolute( DWORD Address );

	DWORD FarProc( DWORD Address , DWORD LB , DWORD HB );

	DWORD FindPushString( DWORD dwStart , DWORD dwEnd , DWORD dwAddress );
	DWORD FindString( PCHAR pString , DWORD dwStart , DWORD dwEnd , DWORD dwOffset );

	DWORD FindPattern( PCHAR pPattern , PCHAR pszMask , DWORD dwStart , DWORD dwEnd , DWORD dwOffset );
	DWORD FindPattern( PCHAR pPattern , DWORD dwPtLen , DWORD dwStart , DWORD dwEnd , DWORD dwOffset );
	
	DWORD GetModuleSize( DWORD Address );
};

extern cOffset offset;