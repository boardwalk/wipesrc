#define NUM_BARS			12
#define SPEEDO_X			179	
#if PAL
#define SPEEDO_Y			220	/*200*/
#else
#define SPEEDO_Y			195	/*200*/
#endif
#define SPEEDO_WIDTH	   127
#define SPEEDO_HEIGHT	31		/*31*/
#define BAR_X				178	
#if PAL
#define BAR_Y				254
#else
#define BAR_Y				229
#endif
#define MAX_TEXT			256
#define MINUS				10
#define DOT					10
#define COLON				11
#define SLASH				12
#define RACE				0
#define MENU				1


typedef struct 
{
	short	topx;
	short topy;
	short bottomx;
	short bottomy;
}Points;

typedef struct
{
	Points Left, Right;
	char r0; // PCwipeout
	char r1, g1, b1;
	char r2, g2, b2;
}BarPos;

typedef struct
{
	SPRT	 	facia[2];
	SPRT		transbar[2];
	SPRT		solidbar[2];
	DR_MODE		fred[2];
//	POLY_GT4  	bars[NUM_BARS][2];
//	DR_MODE		transPrim[NUM_BARS][2];
//	POLY_G4  	thrustBar[NUM_BARS][2];
	BarPos 		barInfo[NUM_BARS];
} Speedo;

typedef struct
{
	int start_pos_x ;
	int start_pos_y ;
	int size ;
} TEXT_DATA ;

typedef struct
{
	short	vramX ;
	short	vramY ;
	short width ;
	short pad;
}TexChar ;

typedef struct 
{
	char	pilot[25] ;
	short	x,y ;
}PilotData ;


void 			InitText();
void			InitSpeedo 			(Speedo*) ;
void 			fill_speedo 		(Speedo*, int, int) ;
void 			AddNumber			(int, 	TEXT_DATA*, int)	;
void 			AddTime				(int, 	TEXT_DATA*, int) ;
void 			AddText				(char* , TEXT_DATA* , int) ;
void 			SpecialText			(char* , TEXT_DATA* , int) ;
void 			demo_screen 		(POLY_FT4 *, int) ;
TEXT_DATA* 	text_data 			(int, int, int) ;
void 			UpdateRaceHud		(ShipData*, Speedo*, int, ConfigData *);
void 			AddText2				(char* , TEXT_DATA* , int) ;
void 			AddNumber2			(int , TEXT_DATA* , int) ;
void 			SetCharTextures 	(char) ;
void 			AddDelEnd			(char, TEXT_DATA *, int) ;
void 			CentreText			(char* , TEXT_DATA *, int ) ;
void 			DisplayLives 		(SPRT [][2], ConfigData *)	; // PCwipeout
//void 			DisplayLives 		(POLY_FT4 [][2], ConfigData *)	;
int 			CalcPosition		(ShipData *shipIndex);
void 			ScreenFooter 		(int x, int y, int colour);
void 			ShowCopyRight 		(Texture *copyrightSymbol, POLY_FT4 *copyPrim, short x, short y);
void 			InitChampTable 	();
void 			DisplayChampTable ();	  					  
void 			DisplayRacePoints ();	  					  
void 			InitScreenTex 		(Texture **screenTex, char *fileName);
void 			DisplayWinLose 	(Texture *winLose, POLY_FT4 *picPrim, short x, short y, short height, char type);
void 			DisplayVenomComplete (); 							 	
void 			DisplayAllVenomComplete (); 							 	
void 			InitTrackTex 		(Texture **trackTex);
void 			BackPoly 			(POLY_F4 *, DR_MODE *, short, short, short, short)	;
void DisplayRaceInfo (ConfigData *gameData, HiScoreData *hiScoreData);
void DrawHud(ShipData* shipIndex, Speedo *speedo, int choice, ConfigData *gameData);
