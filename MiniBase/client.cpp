#include "client.h"
#include "detours.h"
#include <vector>

bool FirstFrame = false;

GameInfo_t BuildInfo;

client_state_t* client_state = nullptr;

void HookUserMessages()
{
	pUserMsgBase = (PUserMsg)offset.FindUserMsgBase();
		
	pResetHUD = HookUserMsg( "ResetHUD" , ResetHUD );
	pSetFOV = HookUserMsg( "SetFOV" , SetFOV );
	pTeamInfo = HookUserMsg( "TeamInfo" , TeamInfo );
	pCurWeapon = HookUserMsg( "CurWeapon" , CurWeapon );
	pDeathMsg = HookUserMsg( "DeathMsg" , DeathMsg );
}

void HookEngineMessages()
{
	pEngineMsgBase = (PEngineMsg)offset.FindSVCMessages();

	pSVC_sound = HookEngineMsg( "svc_sound" , SVC_Sound );
	pSVC_SpawnStaticSound = HookEngineMsg( "svc_spawnstaticsound" , SVC_SpawnStaticSound );
	pSVC_StuffText = HookEngineMsg( "svc_stufftext" , SVC_StuffText );
	pSVC_NewUserMsg = HookEngineMsg( "svc_newusermsg" , SVC_NewUserMsg );
	pSVC_UpdateUserInfo = HookEngineMsg( "svc_updateuserinfo" , SVC_UpdateUserInfo );
	pSVC_SendCvarValue = HookEngineMsg( "svc_sendcvarvalue" , SVC_SendCvarValue );
	pSVC_SendCvarValue2 = HookEngineMsg( "svc_sendcvarvalue2" , SVC_SendCvarValue2 );
	pSVC_Director = HookEngineMsg( "svc_director" , SVC_Director );
}

void HookEventMessages()
{
	pEventMsgBase = (PEventMsg)offset.FindEventMsgBase();

	pEvent_usp = HookEventMsg( "events/usp.sc" , Event_usp );
	pEvent_m4a1 = HookEventMsg( "events/m4a1.sc" , Event_m4a1 );
}

void ConsolePrintColor( BYTE R , BYTE G , BYTE B , char* string )
{
	TColor24 DefaultColor;
	PColor24 Ptr;
	Ptr = Console_TextColor;
	DefaultColor = *Ptr;
	Ptr->R = R;
	Ptr->G = G;
	Ptr->B = B;
	g_Engine.Con_Printf( "%s" , string );
	*Ptr = DefaultColor;
}

void CL_Dump_User_Messages()
{
	PUserMsg Ptr = nullptr;
	Ptr = pUserMsgBase;

	while ( Ptr->next )
	{
		g_Engine.Con_Printf( "[%i] UserMsgName: %s , Ptr: 0x%X\n" , Ptr->size , Ptr->name , Ptr->pfn );
		Ptr = Ptr->next;
	}
}

void CL_Dump_Engine_Messages()
{
	PEngineMsg Ptr = nullptr;
	Ptr = pEngineMsgBase;

	while ( native_strcmp( Ptr->name , OFF_SVC_MESSAGES_P ) )
	{
		g_Engine.Con_Printf( "[%i] EngineMsgName: %s , Ptr: 0x%X\n" , Ptr->number , Ptr->name , Ptr->pfn );
		Ptr++;
	}
}

void CL_Dump_Event_Messages()
{
	PEventMsg Ptr = nullptr;
	Ptr = pEventMsgBase;

	DWORD i = 0;

	while ( Ptr->next )
	{
		g_Engine.Con_Printf( "[%i] EventMsgName: %s , Ptr: 0x%X\n" , i , Ptr->name , Ptr->pfn );
		Ptr = Ptr->next;
		i++;
	}
}

char* ConvertTypeToRenderString( byte type )
{
	char* RenderTypeGL = "opengl";
	char* RenderTypeD3 = "d3d";
	char* RenderTypeSF = "software";

	if ( type == 3 )
		return RenderTypeD3;
	else if ( type == 2 )
		return RenderTypeGL;
	else if ( type == 1 )
		return RenderTypeSF;

	return "unknown";
}

char* CL_SoundFromIndex( int iIndex )
{
	return client_state->sound_precache[iIndex]->name;
}

void CL_CacheSound()
{
	for ( int i = 0; i < HL_SOUND_MAX; i++ )
	{
		if ( client_state->sound_precache[i]->name )
		{
			g_Engine.Con_Printf( "| %s - %i |\n" , client_state->sound_precache[i]->name , i );
		}
	}
}

void CL_CacheModel()
{
	for ( int i = 0; i < HL_MODEL_MAX; i++ )
	{
		if ( client_state->model_precache[i]->name )
		{
			g_Engine.Con_Printf( "| %s - %i |\n" , client_state->model_precache[i]->name , i );
		}
	}
}

void CL_CacheEvent()
{
	for ( int i = 0; i < HL_EVENT_MAX; i++ )
	{
		if ( client_state->event_precache[i].filename )
		{
			g_Engine.Con_Printf( "| %s - %i |\n" , client_state->event_precache[i].filename , i );
		}
	}
}

void InitHack()
{
	font.InitText( "Verdana" , 12 , 7 );

	ConsolePrintColor( 255 , 64 , 64 , "\n\n\t\t\t[visit: csxr.ru]\n" );

	ConsolePrintColor( 0 , 164 , 255 , "\t\t\t[render: " );
	ConsolePrintColor( 0 , 164 , 255 , ConvertTypeToRenderString( offset.HLType ) );
	ConsolePrintColor( 0 , 164 , 255 , "]\n" );

	ConsolePrintColor( 255 , 200 , 0 , "\t\t\t[Universal goldsrc base by _or_75]\n\n" );

	g_Engine.Con_Printf( "\t\t\tclient: 0x%X\n" , g_pClient );
	g_Engine.Con_Printf( "\t\t\tengine: 0x%X\n" , g_pEngine );
	g_Engine.Con_Printf( "\t\t\tstudio: 0x%X\n\n" , g_pStudio );

	g_Engine.Con_Printf( "\t\t\tuser_msg: 0x%X\n" , pUserMsgBase );
	g_Engine.Con_Printf( "\t\t\tengine_msg: 0x%X\n" , pEngineMsgBase );
	g_Engine.Con_Printf( "\t\t\tevent_msg: 0x%X\n\n" , pEventMsgBase );

	g_Engine.Con_Printf( "\t\t\tGameName: %s\n" , BuildInfo.GameName );
	g_Engine.Con_Printf( "\t\t\tGameVersion: %s\n" , BuildInfo.GameVersion );
	g_Engine.Con_Printf( "\t\t\tProtocol: %d\n" , BuildInfo.Protocol );
	g_Engine.Con_Printf( "\t\t\tBuild: %d\n\n" , BuildInfo.Build );

	g_Engine.Con_Printf( "\t\t\tbasedir: %s\n" , BaseDir );
	
	DWORD dwClientStateMap = *(PDWORD)offset.FindPattern( "\x6A\x27\x50\x68" , "xxxx" , offset.HwBase , offset.HwEnd , 0x4 );

	if ( !offset.FarProc( dwClientStateMap , offset.HwBase , offset.HwEnd ) )
		client_state = (client_state_t*)( dwClientStateMap - 0x1765BC );

	g_Engine.Con_Printf( "\t\t\tclient_state_t: %X\n" , client_state );

	g_Engine.pfnAddCommand( "cl_user_messages" , CL_Dump_User_Messages );
	g_Engine.pfnAddCommand( "cl_engine_messages" , CL_Dump_Engine_Messages );
	g_Engine.pfnAddCommand( "cl_event_messages" , CL_Dump_Event_Messages );

	g_Engine.pfnAddCommand( "cl_cache_sound" , CL_CacheSound );
	g_Engine.pfnAddCommand( "cl_cache_model" , CL_CacheModel );
	g_Engine.pfnAddCommand( "cl_cache_event" , CL_CacheEvent );

	add_log( "[base load ok]" );
}

void HUD_Frame( double time )
{
	if ( !FirstFrame )
	{
		g_Screen.iSize = sizeof( SCREENINFO );

		offset.HLType = g_Studio.IsHardware() + 1;
		
		offset.ConsoleColorInitalize();
		offset.GetGameInfo( &BuildInfo );

		HookUserMessages();
		HookEngineMessages();
		HookEventMessages();

		InitHack();

		FirstFrame = true;
	}
	
	g_Engine.pfnGetScreenInfo( &g_Screen );
	g_Client.HUD_Frame( time );
}

void HUD_Redraw( float time , int intermission )
{
	g_Client.HUD_Redraw( time , intermission );
}

void CL_CreateMove( float frametime , usercmd_s *cmd , int active )
{
	g_Client.CL_CreateMove( frametime , cmd , active );

	static bool bunnyhop = false;

	if ( cmd->buttons & IN_JUMP )
	{
		if ( bunnyhop )
		{
			cmd->buttons &= ~IN_JUMP;
			bunnyhop = false;
		}
		else
		{
			bunnyhop = true;
		}
	}
}

int HUD_AddEntity( int type , cl_entity_s *ent , const char *modelname )
{
	return g_Client.HUD_AddEntity( type , ent , modelname );
}

void HookFunction()
{
	g_pClient->HUD_Frame = HUD_Frame;
	g_pClient->HUD_Redraw = HUD_Redraw;
	g_pClient->CL_CreateMove = CL_CreateMove;
	g_pClient->HUD_AddEntity = HUD_AddEntity;
}