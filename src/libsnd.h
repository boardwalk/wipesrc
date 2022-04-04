#ifndef LIBSND_H
#define LIBSND_H

/*****************************************************************
 *
 * $RCSfile: libsnd.h,v $
 *
 * 	Copyright (C) 1994 by Sony Computer Entertainment Inc.
 *				          All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

#include <sys/types.h>

/*
 * Macro
 */
#define SSPLAY_INFINITY  0
#define SS_NOTICK	 0x1000
#define SS_NOTICK0	 0
#define SS_TICK60	 1
#define SS_TICK240	 2
#define SS_TICK120	 3
#define SS_TICKMODE_MAX  4
#define SSPLAY_PAUSE     0
#define SSPLAY_PLAY      1
#define SS_SOFF          0
#define SS_SON           1
#define SS_MIX           0
#define SS_REV           1
#define SS_SERIAL_A      0
#define SS_SERIAL_B      1
#define SS_MUTE_OFF      0
#define SS_MUTE_ON       1

#define SS_IMEDIATE 	    0
#define SS_IMMEDIATE        0
#define SS_WAIT_COMPLETED   1

#define SS_REV_TYPE_OFF        0
#define SS_REV_TYPE_ROOM       1
#define SS_REV_TYPE_STUDIO_A   2
#define SS_REV_TYPE_STUDIO_B   3
#define SS_REV_TYPE_STUDIO_C   4
#define SS_REV_TYPE_HALL       5
#define SS_REV_TYPE_SPACE      6
#define SS_REV_TYPE_ECHO       7
#define SS_REV_TYPE_DELAY      8
#define SS_REV_TYPE_PIPE       9

#define SS_SEQ_TABSIZ     172

/*
 * Vag & Vab Structure
 */
typedef struct VabHdr {         /* VAB Bank Headdings */

	int32_t           form;          /* always 'VABp' */
	int32_t           ver;           /* VAB file version number */
	int32_t           id;            /* VAB id */
	uint32_t  fsize;         /* VAB file size */
	uint16_t reserved0;     /* system reserved */
	uint16_t ps;            /* # of the programs in this bank */
	uint16_t ts;            /* # of the tones in this bank */
	uint16_t vs;            /* # of the vags in this bank */
	uint8_t  mvol;          /* master volume for this bank */
	uint8_t  pan;           /* master panning for this bank */
	uint8_t  attr1;         /* bank attributes1 */
	uint8_t  attr2;         /* bank attributes2 */
	uint32_t  reserved1;     /* system reserved */

} VabHdr;			/* 32 byte */


typedef struct ProgAtr {        /* Program Headdings */

	uint8_t tones;          /* # of tones */
	uint8_t mvol;           /* program volume */
	uint8_t prior;          /* program priority */
	uint8_t mode;           /* program mode */
	uint8_t mpan;           /* program pan */
	char          reserved0;      /* system reserved */
	int16_t         attr;           /* program attribute */
	uint32_t reserved1;      /* system reserved */
	uint32_t reserved2;      /* system reserved */

} ProgAtr;			/* 16 byte */


typedef struct VagAtr {         /* VAG Tone Headdings */

	uint8_t  prior;         /* tone priority */
	uint8_t  mode;          /* play mode */
	uint8_t  vol;           /* tone volume*/
	uint8_t  pan;           /* tone panning */
	uint8_t  center;        /* center note */
	uint8_t  shift;         /* center note fine tune */
	uint8_t  min;           /* minimam note limit */
	uint8_t  max;           /* maximam note limit */
	uint8_t  vibW;          /* vibrate depth */
	uint8_t  vibT;          /* vibrate duration */
	uint8_t  porW;          /* portamento depth */
	uint8_t  porT;          /* portamento duration */
	uint8_t  pbmin;         /* under pitch bend max */
	uint8_t  pbmax;         /* upper pitch bend max */
	uint8_t  reserved1;     /* system reserved */
	uint8_t  reserved2;     /* system reserved */
	uint16_t adsr1;         /* adsr1 */
	uint16_t adsr2;         /* adsr2 */
	int16_t          prog;          /* parent program*/
	int16_t          vag;           /* vag reference */
	int16_t          reserved[4];   /* system reserved */

} VagAtr;			/* 32 byte */


/*
 * Volume Structure
 */
typedef struct {
        uint16_t left;    /* L Channel */
        uint16_t right;   /* R Channel */
} SndVolume;


/*
 * CallBack
 */
typedef void (*SsMarkCallbackProc)(int16_t, int16_t, int16_t);


/*
 * Prototype
 */
extern int16_t SsVabOpen (uint8_t*, VabHdr*);
extern void  SsVabClose (int16_t);
extern int16_t SsVabTransCompleted (int16_t);
extern int16_t SsVabOpenHead (uint8_t*, int16_t);
extern int16_t SsVabTransBody (uint8_t*, int16_t);
extern int16_t SsVabTransBodyPartly (uint8_t*, uint32_t, int16_t);

extern void  SsInit ();
extern void  SsSetTableSize (char*, int16_t, int16_t);
extern void  SsSetTickMode (int32_t);
extern void  SsStart ();
extern void  SsEnd ();

extern void  SsSeqCalledTbyT ();
extern int16_t SsSeqOpen (uint32_t*, int16_t);
extern void  SsSeqPlay (int16_t, char, int16_t);
extern void  SsSeqPause (int16_t);
extern void  SsSeqReplay (int16_t);
extern void  SsSeqStop (int16_t);
extern void  SsSeqSetVol (int16_t, int16_t, int16_t);
extern void  SsSeqSetNext (int16_t, int16_t);
extern void  SsSeqSetCrescendo (int16_t, int16_t, int32_t);
extern void  SsSeqSetDecrescendo (int16_t, int16_t, int32_t);
extern void  SsSeqSetRitardando (int16_t, int32_t, int32_t);
extern void  SsSeqSetAccelerando (int16_t, int32_t, int32_t);
extern void  SsSeqClose (int16_t);

extern int16_t SsSepOpen (uint32_t*, int16_t, int16_t);
extern void  SsSepPlay (int16_t, int16_t, char, int16_t);
extern void  SsSepPause (int16_t, int16_t);
extern void  SsSepReplay (int16_t, int16_t);
extern void  SsSepStop (int16_t, int16_t);
extern void  SsSepSetVol (int16_t, int16_t, int16_t, int16_t);
extern void  SsSepSetCrescendo (int16_t, int16_t, int16_t, int32_t);
extern void  SsSepSetDecrescendo (int16_t, int16_t, int16_t, int32_t);
extern void  SsSepSetRitardando (int16_t, int16_t, int32_t, int32_t);
extern void  SsSepSetAccelerando (int16_t, int16_t, int32_t, int32_t);
extern void  SsSepClose (int16_t);

extern void  SsVoKeyOn (int32_t, int32_t, uint16_t, uint16_t);
extern void  SsVoKeyOff (int32_t, int32_t);
extern void  SsSetMVol (int16_t, int16_t);
extern void  SsGetMVol (SndVolume*);
extern void  SsSetRVol (int16_t, int16_t);
extern void  SsGetRVol (SndVolume*);
extern void  SsSetMute (char);
extern void  SsSetSerialAttr (char, char, char);
extern char  SsGetSerialAttr (char, char);
extern void  SsSetSerialVol (char, int16_t, int16_t);
extern void  SsGetSerialVol (char, SndVolume*);
extern void  SsSetNck (int16_t);
extern int16_t SsGetNck ();
extern void  SsSetNoiseOn (int16_t, int16_t);
extern void  SsSetNoiseOff ();
extern void  SsSetMono ();
extern void  SsSetStereo ();

extern void  SsSetTempo (int16_t, int16_t, int16_t);
extern void  SsSetLoop (int16_t, int16_t, int16_t);
extern int16_t SsIsEos (int16_t, int16_t);
extern void  SsPlayBack (int16_t, int16_t, int16_t);
extern void  SsSetMarkCallback (int16_t, int16_t, SsMarkCallbackProc);
extern char  SsSetReservedVoice (char);

extern int16_t SsUtKeyOn (int16_t, int16_t, int16_t, int16_t, int16_t, int16_t, int16_t);
extern int16_t SsUtKeyOff (int16_t, int16_t, int16_t, int16_t, int16_t);
extern int16_t SsUtPitchBend (int16_t, int16_t, int16_t, int16_t, int16_t);
extern int16_t SsUtChangePitch (int16_t, int16_t, int16_t, int16_t, int16_t,
                              int16_t, int16_t);
extern int16_t SsUtChangeADSR (int16_t, int16_t, int16_t, int16_t,
                             uint16_t, uint16_t);
extern int16_t SsUtSetVabHdr (int16_t, VabHdr*);
extern int16_t SsUtGetVabHdr (int16_t, VabHdr*);
extern int16_t SsUtSetProgAtr (int16_t, int16_t, ProgAtr*);
extern int16_t SsUtGetProgAtr (int16_t, int16_t, ProgAtr*);
extern int16_t SsUtSetVagAtr (int16_t, int16_t, int16_t, VagAtr*);
extern int16_t SsUtGetVagAtr (int16_t, int16_t, int16_t, VagAtr*);
extern int16_t SsUtSetDetVVol (int16_t, int16_t, int16_t);
extern int16_t SsUtGetDetVVol (int16_t, int16_t*, int16_t*);
extern int16_t SsUtSetVVol (int16_t, int16_t, int16_t);
extern int16_t SsUtGetVVol (int16_t, int16_t*, int16_t*);
extern int16_t SsUtAutoVol (int16_t, int16_t, int16_t, int16_t);
extern int16_t SsUtAutoPan (int16_t, int16_t, int16_t, int16_t);
extern void  SsUtReverbOn ();
extern void  SsUtReverbOff ();
extern int16_t SsUtSetReverbType (int16_t);
extern int16_t SsUtGetReverbType ();
extern void  SsUtSetReverbDepth (int16_t, int16_t);
extern void  SsUtSetReverbFeedback (int16_t);
extern void  SsUtSetReverbDelay (int16_t);

extern void  SsUtAllKeyOff (int16_t /* mode */);
#define SS_ALL_KEY_OFF_MODE 0

extern void  SsSetAutoKeyOffMode (int16_t /* mode */);
#define SS_AUTO_KEY_OFFon  0     /* default */
#define SS_AUTO_KEY_OFFoff 1     /* for backward compatibility */

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
/*
 */
#endif /* _LIBSND_H_ */
/* DON'T ADD STUFF AFTER THIS */
