#define		NumNames		5
#define		nameLetters	3
#define		HiScoreTracks	14

typedef struct
{

	char	 		names[NumNames][nameLetters+1] ; 	/* 20 */
	int			lapTimes[NumNames] ;						/* 20 */
	signed char	lapTimesPilot[NumNames] ; 				/* 5  */
	short			position[NumNames] ;						/* 10 */
	int			lapRecord ;									/*  4 */
	signed char	lapRecordPilot ;							/*  1 */
}HiScores ;														/* 64 */


typedef struct
{
	char 			nameEntered[9] ;
	char			currChar ;
	short			charVal ;
	short			numChars ;
	char			dispChar[2] ;
	int			toggleDisplay ;		
	char			userEntry ;
	char			checkVal ;
	char			enterHs ;
	int			count ;
	int 			padDelay ;
	int			bestLap  ;
	char			highScore  ;
	char			displayHiScore ;
	short			charX ;
	char			confirm ;
	char			lConfirm ;
}HiScoreData ;



void HighScore (ConfigData *) ;
void AddEntry2Table (ConfigData *, int , char []) ;
char CheckLapRecord (ConfigData *, ShipData *, int)	;
void DisplayHiScore (char)	;
void InitHiScoreData (HiScoreData *) ;	
int  CheckHiScore (ConfigData *, HiScoreData *)	;
void NewHighScore (ConfigData *, HiScoreData *)	;
char ChampLapRecord 	(ConfigData *, ShipData *)	;
void NewHighScoreDraw (ConfigData *gameData, HiScoreData *hiScoreData);
