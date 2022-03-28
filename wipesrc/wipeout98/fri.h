typedef struct _glos
{
	TrackCamera		camera;
	Skeleton		camPos;
	AutoMeshData	autoMesh;
	ShipData		*shipIndex;
	Shadow			*shadow;
	Object			*shipShapes[(NO_OF_SHIPS * 2) + 1];
	RescueData		rescueDroid;
	char			showMenu;
	char			confirm;
	char			machinePaused;
	char			showGameOver;
	int				choice;
	ulong			toggleDisplay;
	int				cdPause;
	Speedo			speedo;
	short			sky;
	Effect			*effects;
	WeaponData		*weaponIndex;
	WeaponShape     *weaponShapes;			 
	HiScoreData 	hiScoreData	;
	long			attractDelay;
	long			NeedEndText;
	long			NeedWinLose;
	long			NeedBackPoly;
	long			NeedRaceInfo;
	long			NeedDeSelectScreen;
	long			NeedNewHighScores;
	long			NeedRacePoints;
	long			NeedChampTable;
	long			NeedAllVenomComplete;
	long			NeedVenomComplete;
	long			NeedContinueGameConfirm;
	long			NeedEndTextVar;
	long			NeedWinLoseVar;
	long			NeedInGameReset;
	long			NeedInGameConfirm;
	long			NeedInSingleGameConfirm;
	short			startFireText;
	short			startIceText;
	long			xpad;
	long			pad;

} GLOS;

#if 0
extern	GLOS	glo, glo1;
#endif

extern	long	ctrl_init;
extern	long	ok_to_interrupt;
extern	long	ctrl_says_quit;
