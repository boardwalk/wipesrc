#include "pad.h"


#define              CD_FS             0
#define 					COMBAT   	      1
#define              SystemFont        0        /* system font on or off */ // PCwipeout
#define              EditorOn          0        /* editor on or off */
#define              CD_AUDIO          1

#define					INTERACTIVE			0
#define					ATTRACT				1

#define              LoadMessages      0        /* printf for Loading etc */
#define					RedText				0
#define					WhiteText			1

#define              FileRoot     "c:"

#define              TextureTableMax 800

#define  	         SoundHeapSize (1024 * 20 / 4)
// #define              FastRam     (( uint32_t ) 0x1f800000 )

#define              Warning     0
#define              NonFatal    1
#define              Fatal       2

#define					OT_SIZE 8192

#define				FONT_TPAGE	25		//PCWipeout

extern int32_t				copyright_Psygnosis_1995;

extern NegconBuff 	neg1, neg2;
extern TrackData     *track;
extern int32_t				ownShip;
extern int32_t				serialShip;
extern char				gameType;
extern int32_t 				textStart;
extern int32_t 				textPrim;
extern int16_t 			DRTEXT_16;
extern int16_t 			DRTEXT_12;
extern int16_t 			DRTEXT_8;
extern int16_t 			DRTEXT_RED;
//extern POLY_FT4 		display_text[600] ;
extern SPRT 		display_text[600] ;
extern	DR_MODE		dmode;
extern void				(*CameraUpdate)(TrackCamera *, ShipData*, RescueData*);
extern TrackCamera    *cameraPtr;
extern int32_t				NoOfMainSections;
extern DR_MODE			shieldParams[64*3][2];
extern CdlLOC			loc[20];
extern int32_t				ntoc;
extern int32_t				actualCdTrack;
extern uint16_t			pad;
extern uint16_t        opad, xpad;
extern PadInfo			pCon;
extern NegInfo			nCon;
extern char             negSelect ;
extern char             negBack  ;
extern char             negQuit ;

extern int32_t  	   	soundHead[ SoundHeapSize ];
extern SpuCommonAttr c_attr;
extern SpuReverbAttr	r_attr;
extern ChanTable		chanTable[24];
extern VagTable	  	vagTable[42];
extern int16_t  			sampleLevel;

extern int32_t				trackNo ;
extern char				NumTracks ;
extern char             totalTracks ;
extern int32_t				attractDelay;
extern int32_t				slowCraft;
extern int32_t 				raceType;

extern int32_t				tempLapTimes[5];
extern int32_t              serialLapTimes[5];

extern char				trackPath[64];
extern Object*			startBoom[4];
extern int16_t 			startBoomCount;
extern int32_t           crossedLine;
extern int32_t            crossedLineSerial;

extern int16_t			numTex;


extern int32_t 	  		primCount;

extern DynamicHeap*  heap;

extern Texture*      TextureTable[ ];
extern int16_t         TextureTableCount;

extern P_TAG*        OT[ ];

extern P_TAG         ot0[ ];
extern P_TAG         ot1[ ];

extern	int16_t                edDebug;

extern char          errorString[ ];

/* Quick bodge to auto place textures in vram! */
extern int16_t		   textureX;
extern int16_t		   textureY;

extern int16_t		   clutX;
extern int16_t		   clutY;


#define MaxUnique 128
#define MaxObjects 40


extern char*          sharedPrims[ ][ MaxObjects ];
extern int16_t          sharedPrimCount[ ];


extern int32_t           spareMax;

extern int16_t          showFaceFlags;



extern Object         **objectTable;


extern int32_t           buffer1[ ];
extern Line3D         line3d[ ];
extern LineSkel       lineSkel[ ];
extern int32_t           buffer2[ ];


#define     TrackLoTiles      (7*80*1)
#define     TrackMedTiles     (7*4*4)
#define     TrackHiTiles      (7*4*16)

#define     TrackTextures     24

//#if 0	// PCwipeout
#if 1
#define     MeshRamSize       (16384 * 4)
#else
#define     MeshRamSize       (0)
#endif

extern int32_t           MeshRam0[ ];
extern int32_t           MeshRam1[ ];
extern int32_t*          MeshPtr;



extern PolyFT4        *prConfirm;


#define  PrimitiveBufferSize  65536

extern uint8_t          PrimitiveBuffer0[ ];
extern uint8_t          PrimitiveBuffer1[ ];
extern uint8_t*         PrimitiveBuffer[  ];
extern StdPoly        prims;


#define SlowMemSize  1024

extern int32_t           SlowMem[ ];

/* spline stuff */
extern Object              *camSpline[ ];
extern Object              *shipSpline[ ];
extern int16_t         finishedExtro;
extern int16_t         extroLevel;

/* different palettes */

extern	char	main_palette[];
extern	char	shield_palette[];
extern	char	current_palette;
#define	MAIN_PAL	0
#define	SHIELD_PAL	1

/* fade table references */

#define	BLACK_NOT_DRAWN	1
#define	BLACK1_TRANSL	2
#define	BLACK2_TRANSL	3
#define	BLACK3_TRANSL	4
#define	BLACK4_TRANSL	5
#define	BLACK5_TRANSL	6
#define	BLACK6_TRANSL	7
#define	WHITE1_TRANSL	8
#define	WHITE2_TRANSL	9
#define	WHITE3_TRANSL	10
#define	WHITE4_TRANSL	11
#define	WHITE5_TRANSL	12
#define	WHITE6_TRANSL	13
#define	WHITE7_TRANSL	14
#define	WHITE8_TRANSL	15
#define	RED_TRANSL		16
#define	BRIGHT_RED_TRANSL	17
#define	YELLOW_TRANSL	18
#define	YELLOW_WHITE_TRANSL	19
#define	BLUE_TRANSL		20
#define	PURPLE_TRANSL	21
#define	UNUSED1_TRANSL	22
#define	UNUSED2_TRANSL	23
#define	UNUSED3_TRANSL	24
#define	UNUSED4_TRANSL	25

#define	BLUE_SPEEDUP	25

#define	RED_LIGHT_FADE	 16
#define	AMBER_LIGHT_FADE 26
#define	GREEN_LIGHT_FADE 27

#define	CYCLE_START		40
#define	CYCLE_END		80

extern	char	inmenu;
extern	char	Reallyinmenu;
extern	char	*background_buffer;

#define	CdControlB(a,b,c) CdControl(a,b,c)

extern uint32_t	this_frame, last_frame;
extern  char trackdir[];
extern	int32_t	frameRate30;


#define	HIGH_DISTANCE	65535
#define	MEDIUM_DISTANCE	38000
#define	LOW_DISTANCE	18000
extern int32_t	DrawDistance;
extern char	Sky;
extern char	Wide;
extern int32_t	DrawnShieldBefore;
extern int32_t			FR30;
extern int32_t			FR60;
extern int32_t			FR50;
extern int32_t			FR6;
extern int32_t			FR10;
extern int32_t			FR40;
extern int32_t			FR50U;
extern int32_t			FR15U;
extern int32_t			FR30U;
extern int32_t			FR7U;
extern int32_t			FR32U;
extern int32_t			TextureTrack;
extern int32_t			SkyTrackFlags;

extern int32_t				WinPitch;
extern int16_t 			WinWidth;
extern int16_t	 		WinHeight;
extern int16_t			WinHeightX2;
