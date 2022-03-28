#define 		NORMAL_COMBAT_LINK	1
#define			MAX_OPTIONS			15
#define			MAX_MENUS			30
#define 		VRAM				0
#define 		SCREEN				1
#define			MODELSIZE			800	/*1800*/
#define			MAX_CD_TRACKS		11	
#define 		MAX_GAMES			15

#define			NO_ERROR			0
#define			CARD_FULL			101
#define			DUP_NAME			102	
#define			LOAD_ERROR			103
#define			SAVE_ERROR			104
#define			RENAME_ERROR		105
#define			NO_CARD				106
#define			NO_FILES			107
#define			NO_FORMAT			108
#define			FORMAT_ERROR 		109

#define			FORMATTED			0									
#define			UNFORMATTED			1
#define 		NORMPAD_PRESETS 	8
#define 		NEGCON_PRESETS 		5
#define 		NORM                1
#define 		OVERWRITE           2
#define 		RENAME              3
/* BUTTON TYPE */
#define         PSELECT         	0
#define         PSTART          	1
#define         PBACK           	2
#define         PCHANGE         	3
#define         PFORMAT         	4
#define         NSELECT         	5
#define         NSTART          	6
#define         NBACK           	7
#define         NCHANGE         	8
#define         NFORMAT         	9
#define         THRUST          	10
#define         PADFIRE         	11
#define         VIEW            	12
#define         LBRAKE          	13
#define         RBRAKE          	14
#define         ARROWL          	15
#define         ARROWR          	16
#define         DOTT            	17


/******************/
/***** Menu 1 *****/
/******************/
#define			MAIN_MENU			0
#define			SINGLE_GAME			0
#define			MULTI_GAME			1
#define			GAME_OPTIONS		1
#define			QUIT_TO_DOS			2  // PCwipeout

/******************/
/***** Menu 2 *****/
/******************/
#define			TRACK_MENU			1

/******************/
/***** Menu 3 *****/
/******************/
#define			DUAL_GAME_MENU		2
#define			MASTER_PSX			0
#define			SLAVE_PSX			1

/******************/
/***** Menu 4 *****/
/******************/
//#define			AUDIO_MENU			3

/******************/
/***** Menu 5 *****/
/******************/
#define 		OPTION_MENU			4
#define 		LOAD_GAME			0
#define 		SAVE_GAME			1
#define 		CONTROLS			2
#define 		DETAILS				3
//#define 		AUDIO_CONFIG		4
#define 		HIGH_SCORES			4

/******************/
/***** Menu 6 *****/
/******************/
#define			CLASS_MENU			5

/******************/
/***** Menu 7 *****/
/******************/
#define 		TEAM_MENU			6
#define 		AG_SYSTEMS			0
#define 		AURICOM				1
#define 		QIREX				2
#define 		FEISAR				3

/******************/
/***** Menu 8 *****/
/******************/
#define			PILOT_MENU			7

/******************/
/***** Menu 9 *****/
/******************/
#define			RACE_TYPE_MENU 		8
#define			NO_GAME_SET	 		0
#define			CHAMPIONSHIP 		1
#define			TIME_TRIAL 			3
#define			SINGLE_RACE	 		2

/*******************/
/***** Menu 10 *****/
/*******************/
#define			LOAD_MENU 			9

/*******************/
/***** Menu 11 *****/
/*******************/
#define			SAVE_MENU 			10
	
/*******************/
/***** Menu 12 *****/
/*******************/
#define			ENTER_GAME_NAME 	11

/*******************/
/***** Menu 13 *****/
/*******************/
#define			HIGH_SCORES_MENU 	12

/*******************/
/***** Menu 14 *****/
/*******************/
#define			CONFIRM_SAVE 		13

/*******************/
/***** Menu 15 *****/
/*******************/
#define			INIT_TWO_PLAYER 	14

/*******************/
/***** Menu 16 *****/
/*******************/
#define			SERIAL_RACE_TYPE_MENU 15
#define			TWO_PLAYER_RACE		4
#define			HEAD_TO_HEAD	 	5

/*******************/
/***** Menu 17 *****/
/*******************/
#define			START_TWO_PLAYER_MENU 16

/*******************/
/***** Menu 18 *****/
/*******************/
#define			PADCONFIG_MENU		17

/*******************/
/***** Menu 19 *****/
/*******************/
#define			FORMAT_CONFIRM		18

/*******************/
/***** Menu 20 *****/
/*******************/
#define			NEGCON_MENU			19

/*******************/
/***** Menu 21 *****/
/*******************/
#define			TWIST_MENU			20

/*******************/
/***** Menu 22 *****/
/*******************/
#define			BEST_TIMES_MENU		21

/*******************/
/***** Menu 23 *****/
/*******************/
#define			TIME_TRIAL_MENU		22

/*******************/
/***** Menu 24 *****/
/*******************/
#define			QUIT_MENU			23

/*******************/
/***** Menu 25 *****/
/*******************/
#define			DETAILS_MENU		24

/*******************/
/***** Menu 26 *****/
/*******************/
#define			CONTROLLER_MENU		25

/*******************/
/***** Menu 27 *****/
/*******************/
#define			CONTROL_EDIT_MENU 	26

/*******************/
/***** Menu 28 *****/
/*******************/
#define			MULTI_MENU			27
#define			DUAL_GAME			0
#define			NETWORK_GAME		1

/*******************/
/***** Menu 29 *****/
/*******************/
#define			NETWORK_MENU		28

/*******************/
/***** Menu 30 *****/
/*******************/
#define			SERIAL_MENU			29

struct SelectionData;
struct ConfigData;

typedef struct
{
	Texture	*slideTex;
	Texture	*sliderTex;

} SlideTextures;

typedef struct
{
	POLY_FT4		pointer; 
	POLY_FT4		slidePrim;
	POLY_FT4		sliderPrim;
	POLY_F4			lines[12];
} MusicPrims;

typedef struct
{
	long			id;
	long 			flags;
	long			bnum;
	short			textureX, textureY;
	short 			textureH, textureW;
} TimData;

typedef struct
{
	short			textX, textY;
	char			optionText[32];
} MenuOptions;

typedef struct	menu
{
	char			menuTitle[32];
	short			titleX, titleY;
	char			numOptions;
	MenuOptions		menuOptions[MAX_OPTIONS];
	signed char		prevMenu;
	Object			**menuModel;
	Object			**menu2Model;
	void			(*displayObject) (struct menu *, int *, int, int *);
	void			(*displayMenu) (MusicPrims *, SlideTextures *, struct SelectionData *, char [][9], struct ConfigData *);
	int				updateCount;
	char			errorFlag;
} Menus;

typedef struct ConfigData
{
	int				defaultMenu;
	int				masterWins;
	int				slaveWins;
    int     		modelHdg;

	short			gameOverCount;
	short			showPilotCount;

	short			showPointCount;
	short			showChampCount;

    short           currSliderPos;
    short   		oldSliderPos;

	short			venomComplete;
    unsigned char	negTwist;
    unsigned char	oldNegTwist;

    signed char     negconPreset;
    signed char     oldNegPreset;
    signed char     normpadPreset;
    signed char     oldPadPreset;

	signed char		cdTrack;
    signed char     oldCdTrack;
	char			pad1;
	char			pad2;

	char 			trackNum;
	char 			actTrackNum;
	char			shipNum;
	char			ship2Num;

	char			Class;
	char			team;
	char			team2;
	char			oppMenu;

	char			exitMenu;
	char			exit2Menu;
	char			gameType;
	char			NumTracks;

	char			Lives;
	char			Position;
	char			EndOfGame;
	char			gameComplete;

	char			continueConfirm;
	char			RapierClass;
	char			CheatMode;
    char    		cardPresent;
	
	char			prevLap;
	char			newLapRecord;
    char    		gotoMenu;
    char    		showGameOver;

    char            showBonusTrack;
    char            trackInc;
    char            lostGame;
	char			pad3;
} ConfigData;

typedef struct SelectionData 
{
//	short			currSliderPos;
	signed char		slotRow;
	signed char		slotColumn;
	signed char		optionNo;
	char 			slotsFilled;
	char			select;
	char			firstTime;
	char			nameEntered[9];
	char 			numCardFiles;
	char			trackNum;
	char			fadeHeight;
	char			confirm;
	signed char		speechMode;
	signed char		hiscoreTrack;
	Menus			*menuSystem;
	ulong			toggle;
	char			combatInit;
	char			combatComplete;
	CdlLOC			loc[20];
	int				cardNum;
	char			card1;
	char			card2;
	char			formatCard;
	char			formatConfirm;
	char			calledMenu;
	char			saveType;
} SelectionData;

typedef struct ChampionShip
{
	short			points;
	short			racePoints;
	char			pilot[25];
	short			xPos;
	char			position;
	char			racePosition;
} ChampionShip;

typedef struct SortTable
{
    char            shipNum;
    char            points;
} SortTable;


void 		MenuControl(ConfigData *, TimData *, int *, int);
void 		InitMenuSystem(Menus *);
TimData		*Load16BitTexture(char *, char);
void 		ShowMenuBackground(POLY_FT4 *, POLY_FT4 *, TimData *);
void 		CallMenuSelector(Menus *, int *, int *, Object **, ConfigData *, MusicPrims *, SlideTextures *, SelectionData *, char [][9]);
void 		CallMenuActivator(ConfigData *, int *, int *, Menus *, SelectionData *, char [][9]);
void 		LoadModel(Object**, char *);
void 		LoadTexModel(Object** , char *);
void 		LoadNegConTexModel(Object** , char *);
void 		DrawShip(Menus *, int *, int,  int *);
void 		DrawAudio(Menus *, int *, int,  int *);
void 		MusicVSfx(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void 		LoadGame(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void 		SaveGame(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void 		EnterSaveGame(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void 		ShowHiScores(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void 		InitLoadSave(char [][9]);
void 		MenuErrors(int *, Menus *);
void 		InitMusicVSfx(SlideTextures *);
void 		InitSelectData(SelectionData *);
void 		InitGameData(ConfigData *);
void 		FadeScreen(POLY_F4 *, POLY_F4 *, SelectionData *);
void 		ConfirmSaveGame(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void 		DrawPilot(Menus *, int *, int , int *);
void 		DrawTeams(Menus *, int *, int , int *);
void 		DrawClass(Menus *, int *, int , int *);
void 		DrawTeamShips(int , int , int , int *, Object *);
void 		ContinueGameConfirm (char *, short, char *, ulong *); 
void 		SetUpPilots(int, Menus *);
void 		MasterSlaveSIO(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void 		MenuMaster(ConfigData *, int *);
void 		CaptainBirdsEye(ConfigData *, int *);
void 		StartTwoPlayer(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void 		DrawMainMenu(Menus *, int *, int, int *);

void 		DrawNegCon(Menus *, int *, int, int *);
void 		DrawPlayPad(Menus *, int *, int, int *);

void 		MenuNegCon(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void 		MenuPlayPad(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);

void 		DrawOptionMenus(Menus *, int *, int, int *);
void 		DisplayPadMenu(Menus *, int *, int, int *);
void 		DisplayNegConMenu(Menus *, int *, int, int *);
void 		ConfirmFormat(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void 		SetNormPadPresets(ConfigData *);
void 		SetNegconPresets(ConfigData *);
void 		InitChampionship();
void 		CheckSaveType(SelectionData *, char [][9], ConfigData *);
void 		NegConButtonDisplay(Menus *, int *, int, int *);
void 		InitButtons(char *);
void 		DisplayButtons(int, int, int);
void 		DrawLine(int, int, int, int, POLY_F4 *, int);
void 		WaitForMaster(SelectionData *);
void 		WaitForSlave(SelectionData *);
void 		DrawGames(Menus *, int *, int, int *);
void 		DrawHiScoreMenu(Menus *, int *, int, int *);
void 		ShowTimeTrialScores(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void 		QuitYesNo(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void 		GraphicOptions(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void 		ControllerOptions(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void 		ControllerEdit(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void 		NetworkOptions(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void 		SerialOptions(MusicPrims *, SlideTextures *, SelectionData *, char [][9], ConfigData *);
void		SerialNetworkHandling(MusicPrims *musicPrims, SlideTextures *musicTex, SelectionData *selectData, char loadGames[][9], ConfigData *gameData);
void	RestartGameConfirm (char *confirm, short xpad, char *lConfirm, ulong *toggle);
void MostRecentSaveFile(char *filename);
void JJSReadTIM(char *filename , char *location, TimData *tim);
