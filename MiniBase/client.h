#pragma once

#include "main.h"

extern bool FirstFrame;
extern GameInfo_s BuildInfo;

char* CL_SoundFromIndex( int iIndex );

void ConsolePrintColor( BYTE R , BYTE G , BYTE B , char* string );

void HookUserMessages();
void HookEngineMessages();
void HookFunction();

#define HL_SOUND_MAX 512
#define HL_MODEL_MAX 512
#define HL_EVENT_MAX 256

typedef struct packet_entities_s
{
	int num_entities;
	unsigned char flags[32];
	entity_state_t *entities;
} packet_entities_t;

typedef struct frame_s
{
	double receivedtime;
	double latency;
	qboolean invalid;
	qboolean choked;
	entity_state_t playerstate[32];
	double time;
	clientdata_t clientdata;
	weapon_data_t weapondata[64];
	packet_entities_t packet_entities;
	uint16 clientbytes;
	uint16 playerinfobytes;
	uint16 packetentitybytes;
	uint16 tentitybytes;
	uint16 soundbytes;
	uint16 eventbytes;
	uint16 usrbytes;
	uint16 voicebytes;
	uint16 msgbytes;
} frame_t;

typedef struct event_s
{
	unsigned short	index;
	const char		*filename;
	int				filesize;
	const char		*pszScript;
} event_t;

typedef struct sfx_s
{
	char name[64];
	cache_user_t cache;
	int servercount;
} sfx_t;

typedef struct consistency_s
{
	char * filename;
	int issound;
	int orig_index;
	int value;
	int check_type;
	float mins[3];
	float maxs[3];
} consistency_t;

typedef struct client_state_s
{
	int max_edicts;
	resource_t resourcesonhand;
	resource_t resourcesneeded;
	resource_t resourcelist[1280];
	int num_resources;
	qboolean need_force_consistency_response;
	char serverinfo[512];
	int servercount;
	int validsequence;
	int parsecount;
	int parsecountmod;
	int stats[32];
	int weapons;
	usercmd_t cmd;
	vec3_t viewangles;
	vec3_t punchangle;
	vec3_t crosshairangle;
	vec3_t simorg;
	vec3_t simvel;
	vec3_t simangles;
	vec_t predicted_origins[64][3];
	vec3_t prediction_error;
	float idealpitch;
	vec3_t viewheight;
	screenfade_t sf;
	qboolean paused;
	int onground;
	int moving;
	int waterlevel;
	int usehull;
	float maxspeed;
	int pushmsec;
	int light_level;
	int intermission;
	double mtime[2];
	double time;
	double oldtime;
	frame_t frames[64];
	cmd_t commands[64];
	local_state_t predicted_frames[64];
	int delta_sequence;
	int playernum;
	event_t event_precache[HL_EVENT_MAX];
	model_t *model_precache[HL_MODEL_MAX];
	int model_precache_count;
	sfx_s *sound_precache[HL_SOUND_MAX];
	consistency_t consistency_list[512];
	int num_consistency;
	int highentity;
	char levelname[40];
	int maxclients;
	int gametype;
	int viewentity;
	model_t *worldmodel;
	efrag_t *free_efrags;
	int num_entities;
	int num_statics;
	cl_entity_t viewent;
	int cdtrack;
	int looptrack;
	CRC32_t serverCRC;
	unsigned char clientdllmd5[16];
	float weaponstarttime;
	int weaponsequence;
	int fPrecaching;
	dlight_t *pLight;
	player_info_t players[32];
	entity_state_t instanced_baseline[64];
	int instanced_baseline_number;
	CRC32_t mapCRC;
	event_state_t events;
	char downloadUrl[128];
} client_state_t;