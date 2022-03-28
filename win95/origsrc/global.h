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
// #define              FastRam     (( ulong ) 0x1f800000 )

#define              Warning     0
#define              NonFatal    1
#define              Fatal       2

#define					OT_SIZE 8192

#define				FONT_TPAGE	25		//PCWipeout

extern int				copyright_Psygnosis_1995;

extern NegconBuff 	neg1, neg2;
extern TrackData     *track;
extern int				ownShip;
extern int				serialShip;
extern char				gameType;
extern int 				textStart;
extern int 				textPrim;
extern short 			DRTEXT_16;	 
extern short 			DRTEXT_12;	 
extern short 			DRTEXT_8;	 
extern short 			DRTEXT_RED;	 
//extern POLY_FT4 		display_text[600] ;
extern SPRT 		display_text[600] ;
extern	DR_MODE		dmode;
extern void				(*CameraUpdate)(TrackCamera *, ShipData*, RescueData*);
extern TrackCamera    *cameraPtr;
extern int				NoOfMainSections;
extern DR_MODE			shieldParams[64*3][2];
extern CdlLOC			loc[20];
extern int				ntoc;
extern int				actualCdTrack;
extern ushort			pad;
extern ushort        opad, xpad;
extern PadInfo			pCon;
extern NegInfo			nCon;
extern char             negSelect ;
extern char             negBack  ;
extern char             negQuit ;

extern long  	   	soundHead[ SoundHeapSize ];
extern SpuCommonAttr c_attr;
extern SpuReverbAttr	r_attr;
extern ChanTable		chanTable[24];
extern VagTable	  	vagTable[42];
extern short  			sampleLevel;

extern int				trackNo ;
extern char				NumTracks ;
extern char             totalTracks ;
extern int				attractDelay;
extern int				slowCraft;
extern int 				raceType;

extern int				tempLapTimes[5];
extern int              serialLapTimes[5];

extern char				trackPath[64];
extern Object*			startBoom[4];
extern short 			startBoomCount;
extern int           crossedLine;
extern int            crossedLineSerial;

extern short			numTex;

													 
extern long 	  		primCount;

extern DynamicHeap*  heap;

extern Texture*      TextureTable[ ];
extern short         TextureTableCount;

extern P_TAG*        OT[ ]; 

extern P_TAG         ot0[ ];
extern P_TAG         ot1[ ];

extern	short                edDebug;

extern char          errorString[ ];

/* Quick bodge to auto place textures in vram! */
extern short		   textureX;
extern short		   textureY;

extern short		   clutX;
extern short		   clutY;


#define MaxUnique 128  
#define MaxObjects 40  


extern char*          sharedPrims[ ][ MaxObjects ];
extern short          sharedPrimCount[ ];


extern long           spareMax;

extern short          showFaceFlags;



extern Object         **objectTable;


extern long           buffer1[ ];
extern Line3D         line3d[ ];
extern LineSkel       lineSkel[ ];
extern long           buffer2[ ];


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

extern long           MeshRam0[ ];
extern long           MeshRam1[ ];
extern long*          MeshPtr;



extern PolyFT4        *prConfirm;


#define  PrimitiveBufferSize  65536

extern uchar          PrimitiveBuffer0[ ];
extern uchar          PrimitiveBuffer1[ ];
extern uchar*         PrimitiveBuffer[  ];
extern StdPoly        prims;


#define SlowMemSize  1024

extern long           SlowMem[ ];

/* spline stuff */
extern Object              *camSpline[ ];
extern Object              *shipSpline[ ];
extern short         finishedExtro;
extern short         extroLevel;

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

extern unsigned int	this_frame, last_frame;
extern  char trackdir[];
extern	long	frameRate30;


#define	HIGH_DISTANCE	65535
#define	MEDIUM_DISTANCE	38000
#define	LOW_DISTANCE	18000
extern long	DrawDistance;
extern char	Sky;
extern char	Wide;
extern long	DrawnShieldBefore;
extern long			FR30;
extern long			FR60;
extern long			FR50;
extern long			FR6;
extern long			FR10;
extern long			FR40;
extern long			FR50U;
extern long			FR15U;
extern long			FR30U;
extern long			FR7U;
extern long			FR32U;
extern long			TextureTrack;
extern long			SkyTrackFlags;

extern int				WinPitch;
extern short 			WinWidth;
extern short	 		WinHeight;
extern short			WinHeightX2;
