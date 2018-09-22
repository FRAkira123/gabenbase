#include "offset.h"

cOffset offset;

void cOffset::GetRenderType()
{
	HwDll = (DWORD)GetModuleHandleA( HW_DLL );
	SwDll = (DWORD)GetModuleHandleA( SW_DLL );
	HlMod = (DWORD)GetModuleHandleA( NULL );
}

bool cOffset::GetModuleInfo()
{
	GetRenderType();

	if ( HwDll )
		HwBase = HwDll;
	else if ( SwDll )
		HwBase = SwDll;
	else
		HwBase = HlMod;

	HwSize = GetModuleSize( HwBase );
	HwEnd = HwBase + HwSize - 1;

	HlBase = HlMod;
	HlSize = (DWORD)GetModuleSize( HlBase );
	HlEnd = HlBase + HlSize - 1;

	ClBase = (DWORD)GetModuleHandleA( CLIENT_DLL );

	if ( ClBase )
	{
		ClSize = GetModuleSize( ClBase );
		ClEnd = ClBase + ClSize - 1;
	}
	else
	{
		ClBase = HwBase;
		ClEnd = HwEnd;
		ClSize = HwSize;
	}

	VgBase = (DWORD)GetModuleHandleA( GAMEUI_DLL );

	if ( VgBase )
	{
		VgSize = (DWORD)GetModuleSize( VgBase );
		VgEnd = VgBase + VgSize - 1;
	}

	return ( HwBase && ClBase && VgBase );
}

void cOffset::Error( char* Msg )
{
	MessageBoxA( 0 , Msg , OFF_ERROR , MB_OK | MB_ICONERROR );
	ExitProcess( 0 );
}

DWORD cOffset::FindClientTable()
{
	BYTE ClientOffset[2] = { 0x10, 0x13 };

	DWORD dwClientPattern = FindString( OFF_CLIENT_PATTERN , HwBase , HwEnd , 0 );

	if ( dwClientPattern )
	{
		for ( byte i = 0; i < sizeof( ClientOffset ); i++ )
		{
			DWORD dwClientTablePtr = *(PDWORD)( FindPushString( HwBase , HwEnd , dwClientPattern ) + ClientOffset[i] );

			if ( !FarProc( (DWORD)dwClientTablePtr , HwBase , HwEnd ) &&
				!IsBadReadPtr( (PVOID)dwClientTablePtr , sizeof( cl_clientfunc_t ) ) )
			{
				return dwClientTablePtr;
			}
		}
	}

	return 0;
}

DWORD cOffset::FindEngineTable()
{
	PVOID pEnginePtr = 0;

	BYTE EngineOffset[7] =
	{
		0x22 ,	// OLD PATCH HL/CS
		0x23 ,	// DOD STEAM
		0x1C ,	// NEW PATCH CS
		0x1D ,	// STEAM NEW PATCH
		0x37 ,	// STEAM HL
		0x2D ,	// SVEN COOP
		0x0E ,  // HL Opposing Force
	};

	for ( BYTE bOffset = 0; bOffset < sizeof( EngineOffset ); bOffset++ )
	{
		pEnginePtr = ( cl_enginefunc_t* )*(PDWORD)( (DWORD)g_pClient->Initialize + EngineOffset[bOffset] );

		if ( FarProc( (DWORD)pEnginePtr , HwBase , HwEnd ) && FarProc( (DWORD)pEnginePtr , HlBase , HlEnd ) )
		{
			if ( !FarProc( (DWORD)pEnginePtr , ClBase , ClEnd ) )
				goto found_offset;
		}
	}

	return 0;

found_offset:

	return (DWORD)pEnginePtr;
}

DWORD cOffset::FindStudioTable()
{
	DWORD dwStudioPattern = FindString( OFF_STUDIO_PATTERN , HwBase , HwEnd , 0 );

	if ( dwStudioPattern )
	{
		DWORD dwStudioTablePtr = *(PDWORD)( FindPushString( HwBase , HwEnd , dwStudioPattern ) - 0x14 );

		if ( !FarProc( dwStudioTablePtr , HwBase , HwEnd ) )
			return dwStudioTablePtr;
		else
			goto FindNext;
	}

	return 0;

	FindNext:
	
	DWORD dwStudioTablePtr = *(DWORD*)( (DWORD)g_pClient->HUD_GetStudioModelInterface + 0x30 ); // old patch, dod

	if ( FarProc( (DWORD)dwStudioTablePtr , HwBase , HwEnd ) && FarProc( (DWORD)dwStudioTablePtr , HlBase , HlEnd ) &&
		FarProc( (DWORD)dwStudioTablePtr , ClBase , ClEnd ) )
	{
		dwStudioTablePtr = *(DWORD*)( (DWORD)g_pClient->HUD_GetStudioModelInterface + 0x1A ); // new patch / steam	

		if ( FarProc( (DWORD)dwStudioTablePtr , ClBase , ClEnd ) )
		{
			dwStudioTablePtr = *(DWORD*)( (DWORD)g_pClient->HUD_GetStudioModelInterface + 0x20 ); // sven coop

			if ( FarProc( dwStudioTablePtr , ClBase , ClEnd ) )
				return 0;
		}
	}

	return dwStudioTablePtr;
}

DWORD cOffset::FindUserMsgBase()
{
	BYTE Offset_UserMsgBase[2] =
	{
		0x16 ,	// HL/CS ALL
		0x18	// SVEN COOP
	};

	DWORD dwUserMsgPattern = FindString( OFF_USERMSG_PATTERN , HwBase , HwEnd , 0 );

	if ( !dwUserMsgPattern )
	{
		Error( OFF_USERMSG_ERROR_1 );
		return 0;
	}

	DWORD dwFindAddress = FindPushString( HwBase , HwEnd , dwUserMsgPattern );

	if ( dwFindAddress )
	{
		for ( BYTE bOffset = 0; bOffset < sizeof( Offset_UserMsgBase ); bOffset++ )
		{
			PBYTE pvMovPtr = (PBYTE)( dwFindAddress - Offset_UserMsgBase[bOffset] );

			if ( *pvMovPtr == 0x8B )
			{
				DWORD dwUserMsgBase = *(PDWORD)( (DWORD)pvMovPtr + 2 );

				if ( !FarProc( dwUserMsgBase , HwBase , HwEnd ) )
					return *(PDWORD)dwUserMsgBase;
			}
		}
	}

	Error( OFF_USERMSG_ERROR_2 );
	return 0;
}

DWORD cOffset::FindSVCMessages()
{
	DWORD dwSvcMsgPattern = FindString( OFF_SVC_MSG_PATTERN , HwBase , HwEnd , 0 );

	if ( !dwSvcMsgPattern )
	{
		Error( OFF_SVC_MSG_ERROR_1 );
		return 0;
	}

	DWORD dwFindAddress = *(PDWORD)( FindPushString( HwBase , HwEnd , dwSvcMsgPattern ) + 0x12 );

	if ( FarProc( dwFindAddress , HwBase , HwEnd ) )
	{
		Error( OFF_SVC_MSG_ERROR_2 );
		return 0;
	}

	PEngineMsg pEngineMsgBase = (PEngineMsg)( dwFindAddress - sizeof( DWORD ) );

	BYTE Offset_ReadCoord[5] =
	{
		0x13 ,
		0x15 ,
		0x17 ,
		0x0E ,
		0x0B
	};

	BYTE Offset_SVC_SoundBase[3] =
	{
		0x0E ,
		0x0C ,
		0x16
	};

	if ( pEngineMsgBase )
	{
		MSG_ReadByte = (HL_MSG_ReadByte)Absolute( ( (DWORD)pEngineMsgBase[SVC_CDTRACK].pfn ) + 1 );
		MSG_ReadShort = (HL_MSG_ReadShort)Absolute( ( (DWORD)pEngineMsgBase[SVC_STOPSOUND].pfn ) + 1 );
		MSG_ReadLong = (HL_MSG_ReadLong)Absolute( ( (DWORD)pEngineMsgBase[SVC_VERSION].pfn ) + 1 );
		MSG_ReadFloat = (HL_MSG_ReadFloat)Absolute( ( (DWORD)pEngineMsgBase[SVC_TIMESCALE].pfn ) + 1 );
		MSG_ReadString = (HL_MSG_ReadString)Absolute( ( (DWORD)pEngineMsgBase[SVC_PRINT].pfn ) + 1 );

		DWORD CallMSG_ReadCoord = Absolute( (DWORD)( pEngineMsgBase[SVC_PARTICLE].pfn ) + 1 );
		
		for ( BYTE bOffset = 0; bOffset < sizeof( Offset_ReadCoord ); bOffset++ )
		{
			if ( *(PBYTE)( CallMSG_ReadCoord + Offset_ReadCoord[bOffset] ) == 0xE8 )
			{
				MSG_ReadCoord = (HL_MSG_ReadCoord)Absolute( ( CallMSG_ReadCoord + Offset_ReadCoord[bOffset] + 1 ) );
				goto NextFind1;
			}
		}

		Error( OFF_MSG_READ_CORD );

	NextFind1:

		MSG_ReadCount = *(PINT*)( (INT)(MSG_ReadByte)+1 );
		MSG_CurrentSize = *(PINT*)( (INT)(MSG_ReadByte)+7 );
		MSG_BadRead = *(PINT*)( (INT)(MSG_ReadByte)+20 );

		DWORD SVC_SoundBase = (DWORD)pEngineMsgBase[SVC_SOUND].pfn;
				
		for ( BYTE bOffset = 0; bOffset < sizeof( Offset_SVC_SoundBase ); bOffset++ )
		{
			if ( *(PBYTE)( SVC_SoundBase + Offset_SVC_SoundBase[bOffset] ) == 0xE8 )
			{
				MSG_Buffer = (sizebuf_t *)( *(PDWORD)( SVC_SoundBase + Offset_SVC_SoundBase[bOffset] - 4 ) );
				MSG_StartBitReading = (HL_MSG_StartBitReading)Absolute( SVC_SoundBase + Offset_SVC_SoundBase[bOffset] + 1 );
				MSG_ReadBits = (HL_MSG_ReadBits)Absolute( SVC_SoundBase + Offset_SVC_SoundBase[bOffset] + 8 );
				goto NextFind2;
			}
		}

		Error( OFF_MSG_STR_READING );

	NextFind2:
		
		if ( *(PBYTE)( SVC_SoundBase + 0xD6 ) == 0xE8 )
		{
			MSG_EndBitReading = (HL_MSG_EndBitReading)Absolute( SVC_SoundBase + 0xD7 );
			MSG_ReadBitVec3Coord = (HL_MSG_ReadBitVec3Coord)Absolute( SVC_SoundBase + 0xAF );
		}
		else if ( *(PBYTE)( SVC_SoundBase + 0xE2 ) == 0xE8 )
		{
			MSG_EndBitReading = (HL_MSG_EndBitReading)Absolute( SVC_SoundBase + 0xE3 );
			MSG_ReadBitVec3Coord = (HL_MSG_ReadBitVec3Coord)Absolute( SVC_SoundBase + 0xBE );
		}
		else if ( *(PBYTE)( SVC_SoundBase + 0xDD ) == 0xE8 )
		{
			MSG_EndBitReading = (HL_MSG_EndBitReading)Absolute( SVC_SoundBase + 0xDE );
			MSG_ReadBitVec3Coord = (HL_MSG_ReadBitVec3Coord)Absolute( SVC_SoundBase + 0xB9 );
		}
		else
			Error( OFF_MSG_END_READING );
	}
	else
		Error( OFF_ENGINE_MSG_BASE );

	return (DWORD)pEngineMsgBase;
}

DWORD cOffset::FindEventMsgBase()
{
	DWORD dwEventMsgPattern = FindString( OFF_EVENTMSG_BASE , HwBase , HwEnd , 0 );

	BYTE Offset_EventMsgBase[2] =
	{
		0x07 ,	// HL ALL
		0x08	// SVEN COOP
	};

	if ( !dwEventMsgPattern )
	{
		Error( OFF_EVENTMSG_ERROR_1 );
		return 0;
	}

	DWORD dwFindAddress = FindPushString( HwBase , HwEnd , dwEventMsgPattern );

	if ( dwFindAddress )
	{
		for ( BYTE bOffset = 0; bOffset < sizeof( Offset_EventMsgBase ); bOffset++ )
		{
			DWORD dwEventMsgBase = *(PDWORD)( dwFindAddress - Offset_EventMsgBase[bOffset] );
			
			if ( !FarProc( dwEventMsgBase , HwBase , HwEnd ) )
				return *(PDWORD)dwEventMsgBase;
		}
	}

	Error( OFF_EVENTMSG_ERROR_2 );

	return 0;
}

DWORD cOffset::FindGameConsole()
{
	DWORD dwGameConsolePattern = FindPattern( OFF_GAME_CONSOLE_P , lstrlenA( OFF_GAME_CONSOLE_P ) , VgBase , VgEnd , 0 );

	if ( !dwGameConsolePattern )
	{
		Error( OFF_GAME_CONSOLE_1 );
		return 0;
	}

	DWORD dwFindAddress = *(PDWORD)( FindPushString( VgBase , VgEnd , dwGameConsolePattern ) + 0x21 );

	if ( FarProc( dwFindAddress , VgBase , VgEnd ) )
	{
		Error( OFF_GAME_CONSOLE_2 );
		return 0;
	}

	return dwFindAddress;
}

void cOffset::ConsoleColorInitalize()
{
	DWORD dwGameConsole = FindGameConsole();

	if ( !dwGameConsole )
		Error( OFF_GAME_CONSOLE_3 );

	DWORD Panel = ( *(PDWORD)( dwGameConsole + 8 ) - dwGameConsole );

	Console_TextColor = PColor24( Panel + dwGameConsole + 288 + sizeof( DWORD ) );

	if ( *(PDWORD)( DWORD( Console_TextColor ) + 8 ) != 0 )
	{
		Console_TextColor = PColor24( Panel + dwGameConsole + 288 + ( sizeof( DWORD ) * 2 ) );
	}
}

void cOffset::GetGameInfo( pGameInfo_s GameInfo )
{
	typedef int( *function )( );
	pcmd_t cmd = CommandByName( "version" );
	DWORD Address = (DWORD)cmd->function;

	GameInfo->GameName = *(PCHAR*)( UINT( Address ) + 1 );
	GameInfo->GameVersion = *(PCHAR*)( UINT( Address ) + 6 );
	GameInfo->Protocol = *(PBYTE)( UINT( Address ) + 11 );

	Address = Absolute( UINT( Address ) + 23 );

	if ( FarProc( Address , HwBase , HwEnd ) )
		Error( OFF_ERR_GAMEINFO );

	function GetBuild = (function)Address;
	GameInfo->Build = GetBuild();
}

void cOffset::CopyClient()
{
	native_memcpy( &g_Client , g_pClient , sizeof( cl_clientfunc_t ) );
}

void cOffset::CopyEngine()
{
	native_memcpy( &g_Engine , g_pEngine , sizeof( cl_enginefunc_t ) );
}

void cOffset::CopyStudio()
{
	native_memcpy( &g_Studio , g_pStudio , sizeof( engine_studio_api_t ) );
}

DWORD cOffset::Absolute( DWORD Address )
{
	if ( !Address )
		Error( ABSOLUTE_ERROR );

	return Address + *(PDWORD)Address + 4;
}

DWORD cOffset::FarProc( DWORD Address , DWORD LB , DWORD HB )
{
	return ( ( Address < LB ) || ( Address > HB ) );
}

DWORD cOffset::FindPushString( DWORD dwStart , DWORD dwEnd , DWORD dwAddress )
{
	char szPattern[5] = { 0x68 , 0x00 , 0x00 , 0x00 , 0x00 };
	*(PDWORD)&szPattern[1] = dwAddress;
	return FindPattern( szPattern , sizeof( szPattern ) , dwStart , dwEnd , 0 );
}

DWORD cOffset::FindString( PCHAR pString , DWORD dwStart , DWORD dwEnd , DWORD dwOffset )
{
	bool bFound = false;
	DWORD dwPtLen = lstrlenA( pString );

	for ( DWORD i = dwStart; i < dwEnd - dwPtLen; i++ )
	{
		bFound = true;

		for ( DWORD idx = 0; idx < dwPtLen; idx++ )
		{
			if ( pString[idx] != *(PCHAR)( i + idx ) )
			{
				bFound = false;
				break;
			}
		}

		if ( bFound )
			return i + dwOffset;
	}

	return 0;
}

DWORD cOffset::FindPattern( PCHAR pPattern , PCHAR pszMask , DWORD dwStart , DWORD dwEnd , DWORD dwOffset )
{
	bool bFound = false;
	DWORD dwPtLen = lstrlenA( pszMask );

	for ( DWORD i = dwStart; i < dwEnd - dwPtLen; i++ )
	{
		bFound = true;

		for ( DWORD idx = 0; idx < dwPtLen; idx++ )
		{
			if ( pszMask[idx] == 'x' && pPattern[idx] != *(PCHAR)( i + idx ) )
			{
				bFound = false;
				break;
			}
		}

		if ( bFound )
			return i + dwOffset;
	}

	return 0;
}

DWORD cOffset::FindPattern( PCHAR pPattern , DWORD dwPtLen , DWORD dwStart , DWORD dwEnd , DWORD dwOffset )
{
	bool bFound = false;

	for ( DWORD i = dwStart; i < dwEnd - dwPtLen; i++ )
	{
		bFound = true;

		for ( DWORD idx = 0; idx < dwPtLen; idx++ )
		{
			if ( pPattern[idx] != *(PCHAR)( i + idx ) )
			{
				bFound = false;
				break;
			}
		}

		if ( bFound )
			return i + dwOffset;
	}

	return 0;
}

DWORD cOffset::GetModuleSize( DWORD Address )
{
	return PIMAGE_NT_HEADERS( Address + (DWORD)PIMAGE_DOS_HEADER( Address )->e_lfanew )->OptionalHeader.SizeOfImage;
}