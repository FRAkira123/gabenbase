#include "main.h"

cl_clientfunc_t *g_pClient = nullptr;
cl_enginefunc_t *g_pEngine = nullptr;
engine_studio_api_t *g_pStudio = nullptr;

cl_clientfunc_t g_Client;
cl_enginefunc_t g_Engine;
engine_studio_api_t g_Studio;

//CGameRules* g_pGameRules = nullptr;

PUserMsg pUserMsgBase;
PEngineMsg pEngineMsgBase;
PEventMsg pEventMsgBase;
PColor24 Console_TextColor;

SCREENINFO g_Screen;

BYTE bPreType = 0;

ofstream logfile;
char* BaseDir;

DWORD WINAPI CheatEntry( LPVOID lpThreadParameter );

void add_log( const char *fmt , ... )
{
	if ( !fmt )	return;
	va_list va_alist;
	char logbuf[256] = { 0 };
	va_start( va_alist , fmt );
	vsnprintf( logbuf + native_strlen( logbuf ) , sizeof( logbuf ) - native_strlen( logbuf ) , fmt , va_alist );
	va_end( va_alist );
	logfile << logbuf << endl;
}

DWORD WINAPI ProcessReload( LPVOID lpThreadParameter )
{
	while ( true )
	{
		if ( FirstFrame )
		{
			offset.GetRenderType();

			if ( !offset.GetModuleInfo() )
				FirstFrame = false;
		}
		else
		{
			CreateThread( 0 , 0 , CheatEntry , 0 , 0 , 0 );
		}

		Sleep( 100 );
	}

	return 0;
}

DWORD WINAPI CheatEntry( LPVOID lpThreadParameter )
{
	static HANDLE hProcessReloadThread = 0;

	if ( hProcessReloadThread )
	{
		TerminateThread( hProcessReloadThread , 0 );
		CloseHandle( hProcessReloadThread );
	}

	BYTE counter_find = 0;

start_hook:

	if ( counter_find == 100 )
	{
		if ( !g_pClient )
			offset.Error( CLIENT_FIND_ERROR );

		if ( !g_pEngine )
			offset.Error( ENGINE_FIND_ERROR );

		if ( !g_pStudio )
			offset.Error( STUDIO_FIND_ERROR );
	}

	Sleep( 100 );
	counter_find++;

	if ( !offset.GetModuleInfo() )
	{
		goto start_hook;
	}

	DWORD ClientTable = offset.FindClientTable();

	if ( ClientTable )
	{
		g_pClient = (cl_clientfunc_t*)ClientTable;
		offset.CopyClient();

		if ( (DWORD)g_Client.Initialize )
		{
			DWORD EngineTable = offset.FindEngineTable();

			if ( EngineTable )
			{
				g_pEngine = (cl_enginefunc_t*)EngineTable;
				offset.CopyEngine();

				if ( (DWORD)g_Engine.V_CalcShake )
				{
					DWORD StudioTable = offset.FindStudioTable();

					if ( StudioTable )
					{
						g_pStudio = (engine_studio_api_t*)StudioTable;
						offset.CopyStudio();

						if ( (DWORD)g_Studio.StudioSetupSkin )
						{
							while ( !FirstFrame )
							{
								HookFunction();
								Sleep( 100 );
							}

							bPreType = offset.HLType;

							hProcessReloadThread = CreateThread( 0 , 0 , ProcessReload , 0 , 0 , 0 );
						}
						else
						{
							goto start_hook;
						}
					}
					else
					{
						goto start_hook;
					}
				}
				else
				{
					goto start_hook;
				}
			}
			else
			{
				goto start_hook;
			}
		}
		else
		{
			goto start_hook;
		}
	}
	else
	{
		goto start_hook;
	}

	return 0;
}

BOOL WINAPI DllMain( HINSTANCE hinstDLL , DWORD fdwReason , LPVOID lpReserved )
{
	switch ( fdwReason )
	{
	case DLL_PROCESS_ATTACH:

		DisableThreadLibraryCalls( hinstDLL );

		BaseDir = (char*)HeapAlloc( GetProcessHeap() , HEAP_ZERO_MEMORY , MAX_PATH );
		char* LogFile = new char[256];

		GetModuleFileNameA( hinstDLL , BaseDir , MAX_PATH );

		char* pos = BaseDir + native_strlen( BaseDir );
		while ( pos >= BaseDir && *pos != '\\' ) --pos; pos[1] = 0;

		lstrcpyA( LogFile , BaseDir );
		lstrcatA( LogFile , "\\debug.txt" );

		remove( LogFile );
		logfile.open( LogFile , ios::app );

		CreateThread( 0 , 0 , CheatEntry , 0 , 0 , 0 );

		return TRUE;
	}

	return FALSE;
}