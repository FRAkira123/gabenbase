#pragma once

#include "main.h"

void Event_usp( struct event_args_s *args );
void Event_m4a1( struct event_args_s *args );

extern pfnEventMsgHook pEvent_usp;
extern pfnEventMsgHook pEvent_m4a1;