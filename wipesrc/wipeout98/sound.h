#ifndef SOUND_H
#define SOUND_H

#define SOUND  1

#define NOT_PAUSED 1
#define PAUSED 0
#define PAUSE_LEAVE 8

/****** CONSTANT VOICE CHANNELS ******/

#define ENG_1  0
#define ENG_2	1
#define ENG_3  2
#define ENG_4	3
#define ENG_5	4
#define BRAKE	5
#define ENG_5a	6

/****** PROGRAME IDENTIFIERS ******/

#define SHIP_2_SHIP		0
#define ELECTROBOLT 	1
#define ENG_AIR_INTAKE	2
#define ENG_RUMBLE 		3
#define ENG_THRUST		4
#define EXPLOSION_1	 	5
#define EXPLOSION_2	 	6
#define SHIP_2_TRACK	7
#define MENU_MOVE 		8
#define MENU_SELECT	 	9
#define MENU_2_MENU 	10
#define MINE_DROP 		11
#define MISS_LAUNCH 	12
#define REMOTE_ENG		13
#define POWER_UP	  	14
#define SHIELDS 		15
#define START_SIREN 	16
#define TRACTOR_BEAM	17
#define TURBULENCE 		18
#define CROWD		 	19
#define SP_MINES	 	20
#define SP_MISSILE 		21
#define SP_ROCKET 		22
#define SP_REVCON 		23
#define SP_SHOCKWAVE	24
#define SP_SPECIAL		25
#define SP_3			26
#define SP_2			27
#define SP_1			28
#define SP_GO			29

#define REMOTE		   	13	 /*	31 */	/* REMOTE_ENG */
#define REMOTE2			13	 /* 31 */	/* REMOTE_ENG */
#define REMOTE3			13	 /*	33 */ 	/* REMOTE_ENG */
#define TURBO_SOUND		12	 /* 34 */ 	/* MISS_LAUNCH */
#define CROWD2			19				/* CROWD */


/****** Max volume settings ******/

#define ENG_AIR_INTAKE_VOL 4096
#define ENG_RUMBLE_VOL 8192
#define ENG_THRUST_VOL 500
#define REMOTE_ENG_VOL 125


#define ENG_NOTE_1 60
#define ENG_NOTE_2 42
#define ENG_NOTE_3 39
#define ENG_NOTE_4 48
#define ENG_NOTE_5 55
#define ENG_NOTE_5a 50

typedef struct  SampRec
 {
  uint8_t		SampVol;
  uint8_t		SampPan;
  uint16_t	SampPitch;
  uint8_t		Loop;
  uint8_t		Playing;
  int16_t		PlayID;
 } SampRec;

typedef struct ChanTable
{
	int32_t 		duration;
	int16_t 		priority;
	int16_t 		vagID;
	SpuVoiceAttr s_attr;
} ChanTable;

typedef struct VagTable
{
   uint32_t addr;
	int16_t channelID;
} VagTable;					  /*	 int32_t ?? */

typedef struct _sfx_info
{
char	Volume;
char	Pan;
int16_t	Pitch;

int16_t	vagNo;
int16_t	Playing;

int16_t	Priority;
int16_t	Duration;

int16_t	PlayID;
int16_t	pad;
}SFX_INFO;

void 		InitSpuSound();
uint32_t 		FindVagInVab (uint32_t, int16_t);
void 		UpdateNotes	(int32_t paused);
void 		KillNote(int32_t note);
void 		KillNotes();
void 		KillFadeNotes();
int16_t 		PlayNote(int16_t, int16_t, int16_t);
int16_t 		PlayDirectionalNote(int16_t vagID, int16_t priority, int16_t duration, VECTOR pos );
void 		SetPitch(int16_t VagID, int16_t pitch);
void 		SetVolume(int16_t vagID, int16_t voll, int16_t volr);
int16_t 		AdjustDirectionalNote(int16_t vagID, int16_t priority, int16_t duration, VECTOR pos );
int32_t 		InitCD( CdlLOC*, int32_t );
int16_t 		AdjustRemoteEngineSound(ShipData *shipIndex, Object** shipShapes, TrackCamera *camera );
void 		AdjustEngineSound(ShipData*, TrackCamera*);
void StopNotes();

#endif
