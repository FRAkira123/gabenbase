#include "main.h"

pfnEventMsgHook pEvent_usp;
pfnEventMsgHook pEvent_m4a1;

void Event_usp( struct event_args_s *args )
{
	g_Engine.Con_Printf( "Event_usp\n" );
	pEvent_usp( args );
}

void Event_m4a1( struct event_args_s *args )
{
	g_Engine.Con_Printf( "Event_m4a1\n" );
	pEvent_m4a1( args );
}