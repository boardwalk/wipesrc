#include "standard.h"
#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "combat.h"
#include "menus.h"
#include "hiscores.h"
#include "hud.h"
#include "droid.h"
#include "camera.h"
#include "scene.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"

int32_t				copyright_Psygnosis_1995 = 1;

NegconBuff 		neg1, neg2;
int32_t				ownShip;
int32_t				serialShip;
char				gameType;
int32_t 				textStart;
int32_t				textPrim;
int16_t				DRTEXT_16;
int16_t				DRTEXT_12;
int16_t				DRTEXT_8;
int16_t				DRTEXT_RED;
// POLY_FT4 		display_text[600] ;
SPRT		 		display_text[600] ;
void 				(*CameraUpdate)(TrackCamera *, ShipData*, RescueData*);
TrackCamera    *cameraPtr;
int32_t				NoOfMainSections;
DR_MODE			shieldParams[64 * 3][2];
DR_MODE			dmode;	// PCwipeout
CdlLOC			loc[20];
int32_t				ntoc;
int32_t				actualCdTrack;

uint16_t			pad;
uint16_t         opad, xpad;
PadInfo			pCon;
NegInfo			nCon;
char          negSelect = 0;
char          negBack = 0 ;
char          negQuit = 0 ;


/*** Sound Variables required for libSpu ***/
int32_t  	   	soundHead[ SoundHeapSize ];
SpuCommonAttr 	c_attr;
SpuReverbAttr 	r_attr;

ChanTable		chanTable[24];
VagTable			vagTable[42];
int16_t				sampleLevel;

char				trackPath[64];
Object*			startBoom[4];
int16_t 			startBoomCount = 0;
int32_t            crossedLine;
int32_t            crossedLineSerial;

TrackData*     track;
int32_t				trackNo;
int32_t 				raceType;
char				NumTracks = 6 ;
char				totalTracks = 13;
int32_t				tempLapTimes[5];
int32_t				serialLapTimes[5];
int32_t				attractDelay;
int32_t				slowCraft = 0;

int32_t 				primCount;
int16_t				numTex;
DynamicHeap*   heap;

Texture*       TextureTable[ TextureTableMax ];
int16_t          TextureTableCount = 0;

P_TAG*         OT[ 2 ];

P_TAG          ot0[ OT_SIZE ];	//PCWipeout
P_TAG          ot1[ OT_SIZE ];	//PCWipeout

int16_t          edDebug;

char           errorString[ 256 ];


int16_t		      textureX;
int16_t		      textureY;

int16_t		      clutX;
int16_t		      clutY;


char*          sharedPrims[ MaxUnique ][ MaxObjects ];
int16_t          sharedPrimCount[ MaxUnique ];

int32_t           spareMax = 0;

int16_t          showFaceFlags = 0;



Object         **objectTable;



int32_t           MeshRam0[ MeshRamSize ];
int32_t           MeshRam1[ MeshRamSize ];
int32_t*          MeshPtr;


int32_t           buffer1[ 32 ];
Line3D         line3d[ Max3DLines ];
LineSkel       lineSkel[ Max3DLineSkels ];
int32_t           buffer2[ 32 ];


PolyFT4        *prConfirm;




uint8_t          PrimitiveBuffer0[ PrimitiveBufferSize ];
uint8_t          PrimitiveBuffer1[ PrimitiveBufferSize ];
uint8_t*         PrimitiveBuffer[ 2 ];
StdPoly        prims;


int32_t           SlowMem[ SlowMemSize ];
/* spline stuff */
Object             *camSpline[ 20 ];
Object             *shipSpline[ 20 ];
int16_t          finishedExtro;
int16_t          extroLevel;

uint32_t	this_frame = 0, last_frame = 0;
int32_t			frameRate30 = 30;
int32_t			DrawnShieldBefore;

int32_t			FR30;
int32_t			FR60;
int32_t			FR50;
int32_t			FR6;
int32_t			FR10;
int32_t			FR40;
int32_t			FR50U;
int32_t			FR15U;
int32_t			FR30U;
int32_t			FR7U;
int32_t			FR32U;
int32_t			TextureTrack = 1;
int32_t			SkyTrackFlags = 3;

uint16_t	JoyXVal;
uint16_t	JoyYVal;

char			JoyButtons;
int32_t				WinPitch;
int16_t			WinWidth;
int16_t			WinHeight;
int16_t			WinHeightX2;
