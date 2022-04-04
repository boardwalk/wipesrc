#ifndef FRI_H
#define FRI_H

#include <stdint.h>
#include "droid.h"
#include "effects.h"
#include "hiscores.h"
#include "hud.h"
#include "object.h"
#include "ships.h"
#include "track.h"
#include "weapon.h"

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
	int32_t				choice;
	uint32_t			toggleDisplay;
	int32_t				cdPause;
	Speedo			speedo;
	int16_t			sky;
	Effect			*effects;
	WeaponData		*weaponIndex;
	WeaponShape     *weaponShapes;
	HiScoreData 	hiScoreData	;
	int32_t			attractDelay;
	int32_t			NeedEndText;
	int32_t			NeedWinLose;
	int32_t			NeedBackPoly;
	int32_t			NeedRaceInfo;
	int32_t			NeedDeSelectScreen;
	int32_t			NeedNewHighScores;
	int32_t			NeedRacePoints;
	int32_t			NeedChampTable;
	int32_t			NeedAllVenomComplete;
	int32_t			NeedVenomComplete;
	int32_t			NeedContinueGameConfirm;
	int32_t			NeedEndTextVar;
	int32_t			NeedWinLoseVar;
	int32_t			NeedInGameReset;
	int32_t			NeedInGameConfirm;
	int32_t			NeedInSingleGameConfirm;
	int16_t			startFireText;
	int16_t			startIceText;
	int32_t			xpad;
	int32_t			pad;

} GLOS;

#if 0
extern	GLOS	glo, glo1;
#endif

extern	int32_t	ctrl_init;
extern	int32_t	ok_to_interrupt;
extern	int32_t	ctrl_says_quit;

#endif
