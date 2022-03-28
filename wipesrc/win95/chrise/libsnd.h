/************************************************************************
*									*
*	PlayStation -> PC Conversion					*
*									*
*  	Sound Library (libsnd) Dummy Version - Header File 		*
*									*
*	Lovingly coded for you by Captain Bodger and Patchman		*
*									*
*		March 95						*
*									*
************************************************************************/


/************************************************************************
*									*
*	Los Defines							*
*									*
************************************************************************/

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

/************************************************************************
*									*
*	Los Structures							*
*									*
************************************************************************/

/************************************************************************
*									*
*			 Vag & Vab Structure 				*
*									*
************************************************************************/ 
typedef struct VabHdr
 {
    /* VAB Bank Headdings */

	long           form;          /* always 'VABp' */
	long           ver;           /* VAB file version number */
	long           id;            /* VAB id */
	unsigned long  fsize;         /* VAB file size */
	unsigned short reserved0;     /* system reserved */
	unsigned short ps;            /* # of the programs in this bank */
	unsigned short ts;            /* # of the tones in this bank */
	unsigned short vs;            /* # of the vags in this bank */
	unsigned char  mvol;          /* master volume for this bank */
	unsigned char  pan;           /* master panning for this bank */
	unsigned char  attr1;         /* bank attributes1 */
	unsigned char  attr2;         /* bank attributes2 */
	unsigned long  reserved1;     /* system reserved */

 } VabHdr;			/* 32 byte */


typedef struct ProgAtr
 {        
    /* Program Headdings */

	unsigned char tones;          /* # of tones */
	unsigned char mvol;           /* program volume */
	unsigned char prior;          /* program priority */
	unsigned char mode;           /* program mode */
	unsigned char mpan;           /* program pan */
	char          reserved0;      /* system reserved */
	short         attr;           /* program attribute */
	unsigned long reserved1;      /* system reserved */
	unsigned long reserved2;      /* system reserved */

 } ProgAtr;			/* 16 byte */


typedef struct VagAtr
 {      
    /* VAG Tone Headdings */

	unsigned char  prior;         /* tone priority */
	unsigned char  mode;          /* play mode */
	unsigned char  vol;           /* tone volume*/
	unsigned char  pan;           /* tone panning */
	unsigned char  center;        /* center note */
	unsigned char  shift;         /* center note fine tune */
	unsigned char  min;           /* minimam note limit */
	unsigned char  max;           /* maximam note limit */
	unsigned char  vibW;          /* vibrate depth */
	unsigned char  vibT;          /* vibrate duration */
	unsigned char  porW;          /* portamento depth */
	unsigned char  porT;          /* portamento duration */
	unsigned char  pbmin;         /* under pitch bend max */
	unsigned char  pbmax;         /* upper pitch bend max */
	unsigned char  reserved1;     /* system reserved */
	unsigned char  reserved2;     /* system reserved */
	unsigned short adsr1;         /* adsr1 */
	unsigned short adsr2;         /* adsr2 */
	short          prog;          /* parent program*/
	short          vag;           /* vag reference */
	short          reserved[4];   /* system reserved */

} VagAtr;			/* 32 byte */

/************************************************************************
*									*
*			 Volume Structure				*
*									*
************************************************************************/

typedef struct
 {
        unsigned short left;    /* L Channel */
        unsigned short right;   /* R Channel */
 } SndVolume;

/************************************************************************
*									*
*			 CallBack					*
*									*
************************************************************************/

typedef void (*SsMarkCallbackProc)(short, short, short);


/************************************************************************  
*									*
*	Los Function Prototypes						*
*									* 
************************************************************************/

extern short SsVabTransCompleted (short Arse);
extern short SsVabOpenHead (unsigned char* Bollocks, short Arse);
extern short SsVabTransBody (unsigned char* Bollocks, short Arse);

extern void  SsInit (void);
extern void  SsSetTableSize (char* DontKnow, short DontCare, short WhoCares);
extern void  SsSetTickMode (long tick_mode);
extern void  SsStart (void);

extern void  SsSeqCalledTbyT (void);           

extern void  SsSetMVol (short Something, short SomethingElse);              
extern void  SsSetSerialAttr (char Rubbish, char Trash, char Garbage);     
extern void  SsSetSerialVol (char Nothing, short Nowt, short Zip);    


extern short SsUtKeyOn (short A, short B, short C, short D, short E, short F,
			short G);
extern short SsUtKeyOff (short A, short B, short C, short D, short E);
extern short SsUtChangePitch (short A, short B, short C, short D, short, E
                              short F, short G);
extern short SsUtSetVVol (short Something, short SomethingElse, short Arse);
extern void  SsUtReverbOn (void);
extern void  SsUtReverbOff (void);
extern short SsUtSetReverbType (short Arse);
extern void  SsUtSetReverbDepth (short Something, short SomethingElse);

#define SS_ALL_KEY_OFF_MODE 0

#define SS_AUTO_KEY_OFFon  0     /* default */
#define SS_AUTO_KEY_OFFoff 1     /* for backward compatibility */

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */


/* DON'T ADD STUFF AFTER THIS */
