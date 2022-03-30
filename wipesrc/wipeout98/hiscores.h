#define		NumNames		5
#define		nameLetters	3
#define		HiScoreTracks	14

typedef struct
{

	char	 		names[NumNames][nameLetters+1] ; 	/* 20 */
	int32_t			lapTimes[NumNames] ;						/* 20 */
	int8_t	lapTimesPilot[NumNames] ; 				/* 5  */
	int16_t			position[NumNames] ;						/* 10 */
	int32_t			lapRecord ;									/*  4 */
	int8_t	lapRecordPilot ;							/*  1 */
}HiScores ;														/* 64 */


typedef struct
{
	char 			nameEntered[9] ;
	char			currChar ;
	int16_t			charVal ;
	int16_t			numChars ;
	char			dispChar[2] ;
	int32_t			toggleDisplay ;
	char			userEntry ;
	char			checkVal ;
	char			enterHs ;
	int32_t			count ;
	int32_t 			padDelay ;
	int32_t			bestLap  ;
	char			highScore  ;
	char			displayHiScore ;
	int16_t			charX ;
	char			confirm ;
	char			lConfirm ;
}HiScoreData ;



void HighScore (ConfigData *) ;
void AddEntry2Table (ConfigData *, int32_t , char []) ;
char CheckLapRecord (ConfigData *, ShipData *, int32_t)	;
void DisplayHiScore (char)	;
void InitHiScoreData (HiScoreData *) ;
int32_t  CheckHiScore (ConfigData *, HiScoreData *)	;
void NewHighScore (ConfigData *, HiScoreData *)	;
char ChampLapRecord 	(ConfigData *, ShipData *)	;
void NewHighScoreDraw (ConfigData *gameData, HiScoreData *hiScoreData);
