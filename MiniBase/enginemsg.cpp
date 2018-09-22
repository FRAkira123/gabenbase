#include "main.h"

int* MSG_ReadCount = nullptr;
int* MSG_CurrentSize = nullptr;
int* MSG_BadRead = nullptr;
int MSG_SavedReadCount = 0;
sizebuf_t* MSG_Buffer = nullptr;

HL_MSG_ReadByte MSG_ReadByte = nullptr;
HL_MSG_ReadShort MSG_ReadShort = nullptr;
HL_MSG_ReadLong MSG_ReadLong = nullptr;
HL_MSG_ReadFloat MSG_ReadFloat = nullptr;
HL_MSG_ReadString MSG_ReadString = nullptr;
HL_MSG_ReadCoord MSG_ReadCoord = nullptr;
HL_MSG_ReadBitVec3Coord MSG_ReadBitVec3Coord = nullptr;
HL_MSG_ReadBits MSG_ReadBits = nullptr;
HL_MSG_StartBitReading MSG_StartBitReading = nullptr;
HL_MSG_EndBitReading MSG_EndBitReading = nullptr;

void MSG_SaveReadCount()
{
	MSG_SavedReadCount = *MSG_ReadCount;
}

void MSG_RestoreReadCount()
{
	*MSG_ReadCount = MSG_SavedReadCount;
}

pfnEngineMsgHook pSVC_sound;
pfnEngineMsgHook pSVC_SpawnStaticSound;
pfnEngineMsgHook pSVC_NewUserMsg;
pfnEngineMsgHook pSVC_UpdateUserInfo;
pfnEngineMsgHook pSVC_StuffText;
pfnEngineMsgHook pSVC_SendCvarValue;
pfnEngineMsgHook pSVC_SendCvarValue2;
pfnEngineMsgHook pSVC_Director;

void SVC_Sound()
{
	MSG_SaveReadCount();

	int field_mask , volume;
	float attenuation;
	int channel , ent , sound_num;
	float origin[3] = { 0 };
	int pitch;

	MSG_StartBitReading( MSG_Buffer );

	field_mask = MSG_ReadBits( 9 );

	if ( field_mask & SND_FL_VOLUME )
		volume = MSG_ReadBits( 8 ) / 255.0;
	else
		volume = DEFAULT_SOUND_PACKET_VOLUME;

	if ( field_mask & SND_FL_ATTENUATION )
		attenuation = MSG_ReadBits( 8 ) / 64.0;

	channel = MSG_ReadBits( 3 );
	ent = MSG_ReadBits( 11 );

	if ( field_mask & SND_FL_LARGE_INDEX )
		sound_num = MSG_ReadBits( 16 );
	else
		sound_num = MSG_ReadBits( 8 );

	MSG_ReadBitVec3Coord( origin );

	if ( field_mask & SND_FL_PITCH )
		pitch = MSG_ReadBits( 8 );

	MSG_EndBitReading( MSG_Buffer );

	add_log( "SVC_Sound: %i , %i [%0.2f , %0.2f %0.2f] [%s]\n" ,
			ent , sound_num , origin[0] , origin[1] , origin[2] , CL_SoundFromIndex( sound_num ) );

	MSG_RestoreReadCount();
	pSVC_sound();
}

void SVC_SpawnStaticSound()
{
	MSG_SaveReadCount();

	float pos[3] = { 0 };

	int soundnum;
	float vol , attenuation;
	int ent , pitch , fFlags;

	MSG_StartBitReading( MSG_Buffer );

	pos[0] = MSG_ReadCoord();
	pos[1] = MSG_ReadCoord();
	pos[2] = MSG_ReadCoord();

	soundnum = MSG_ReadShort();
	vol = MSG_ReadByte() / 255.0;
	attenuation = MSG_ReadByte() / 64.0;
	ent = MSG_ReadShort();
	pitch = MSG_ReadByte();
	fFlags = MSG_ReadByte();

	add_log( "SVC_SpawnStaticSound: %i , %i [%0.2f , %0.2f %0.2f] [%s]\n" ,
			ent , soundnum , pos[0] , pos[1] , pos[2] , CL_SoundFromIndex( soundnum ) );

	MSG_EndBitReading( MSG_Buffer );

	MSG_RestoreReadCount();
	pSVC_SpawnStaticSound();
}

void SVC_NewUserMsg()
{
	MSG_SaveReadCount();

	int Index = MSG_ReadByte();
	int Size = MSG_ReadByte();
	char* Name = MSG_ReadString();

	add_log( "SVC_NEWUSERMSG: %i / %i [%s]" , Index , Size , Name );

	MSG_RestoreReadCount();
	pSVC_NewUserMsg();
}

void SVC_UpdateUserInfo()
{
	MSG_SaveReadCount();

	int ClientIndex = MSG_ReadByte();
	int ClientUserID = MSG_ReadLong();
	char* ClientUserInfo = MSG_ReadString();

	BYTE ClientCDKeyHash[16] = { 0 };

	for ( int i = 0; i < 16; i++ )
	{
		ClientCDKeyHash[i] = MSG_ReadByte();
	}

	add_log( "SVC_UPDATEUSERINFO: %i / %i [%s]" , ClientIndex , ClientUserID , ClientUserInfo );

	MSG_RestoreReadCount();
	pSVC_UpdateUserInfo();
}

void SVC_StuffText()
{
	MSG_SaveReadCount();

	char* command = MSG_ReadString();
	add_log( "SVC_StuffText: %s" , command );

	MSG_RestoreReadCount();
	pSVC_StuffText();
}

void SVC_SendCvarValue()
{
	MSG_SaveReadCount();

	char* cvar = MSG_ReadString();
	add_log( "SVC_SendCvarValue: %s" , cvar );

	MSG_RestoreReadCount();
	pSVC_SendCvarValue();
}

void SVC_SendCvarValue2()
{
	MSG_SaveReadCount();

	int RequestID = MSG_ReadLong();
	char* cvar = MSG_ReadString();

	add_log( "SVC_SendCvarValue2: %i , %s", RequestID , cvar );

	MSG_RestoreReadCount();
	pSVC_SendCvarValue2();
}

void SVC_Director()
{
	MSG_SaveReadCount();

	int SizeCommand = MSG_ReadByte();
	int DirectType = MSG_ReadByte();
	char* DirectCommand = MSG_ReadString();

	add_log( "SVC_Director: %i %i [%s]" , SizeCommand , DirectType , DirectCommand );

	MSG_RestoreReadCount();
	pSVC_Director();
}