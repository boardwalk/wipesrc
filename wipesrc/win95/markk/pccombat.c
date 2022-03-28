//-------------------------------------------------------------------

#include 					<libspu.h>
#include 					<libsnd.h>
#include 					<sys\ioctl.h>
#define						LANGUAGE_C						// PCwipeout
#include					<asm.h>							// PCwipeout
#include 					<kernel.h>

#include 					"..\origsrc\standard.h"
#include 					"..\origsrc\malloc.h"
#include 					"..\origsrc\tim.h"
#include 					"..\origsrc\object.h"
#include 					"..\origsrc\skeleton.h"
#include 					"..\origsrc\track.h"
#include 					"..\origsrc\ships.h"
#include 					"..\origsrc\weapon.h"
#include 					"..\origsrc\control.h"
#include 					"..\origsrc\droid.h"
#include 					"..\origsrc\camera.h"
#include 					"..\origsrc\menus.h"
#include 					"..\origsrc\handlers.h"
#include 					"..\origsrc\dynam.h"
#include 					"..\origsrc\sound.h"
#include 					"..\origsrc\sparks2.h"
#include 					"..\origsrc\global.h"

#define						_SIZE_T_DEFINED_
#include					"c:\watcom\h\stdio.h"
#include					"..\markk\comio.h"
#include					"..\markk\comerr.h"
#include					"..\markk\pccombat.h"
#include					"..\markk\commwo.h"
#include					"..\markk\transfer.h"
#include					"..\markk\debug.h"

//-------------------------------------------------------------------

char						first = SINGLE;
long						fr,fw;							/* file descriptor */
static unsigned long		ev1, ev2;						/* read/write event descriptor */
static unsigned long		linkev1, linkev2;				/* read/write event descriptor */
static long					initfr,initfw;					/* file descriptor */
static unsigned long		initev1, initev2;				/* read/write event descriptor */
static unsigned long		errorev;						/* error event descriptor */
static unsigned long		timeev;							/* timeout event descriptor */
static unsigned long		ev_r, ev_e, ev_w;				/* read/error event descriptor */
int							writeFlag = 0;
static						readMFlag = 1;
static						readSFlag = 1;

//-------------------------------------------------------------------

void		_init_cont(void)
{
	fn_track("init cont\n")

	// reference all structures here
	
}

//-------------------------------------------------------------------

void		InitializeCombatLink(void)
{
	fn_track("initializecombatlink\n")

	CommLinkCompleted = 0;
}

//-------------------------------------------------------------------

void		MasterSlaveSIO(MusicPrims *musicPrims, SlideTextures *musicTex, SelectionData *selectData, char loadGames[][9], ConfigData *gameData)
{
	fn_track("masterslavesio\n")
	
	if(!selectData->combatComplete) CentreText("INITIALIZING SERIAL LINK UP", text_data (1,116, 8), RedText);
	if(neg1.head == 8960) DisplayButtons(130, 210, NBACK);
	else DisplayButtons(130, 210, PBACK);
	if(selectData->combatInit > 30 && !selectData->combatComplete) {
		if(!CommLinkCompleted) MAK_OpenLink();
		MAK_SelectMasterSlave();
		selectData->combatComplete = 1;
	}
	selectData->combatInit++;
	if(selectData->combatComplete > 0 && selectData->combatComplete < 32) {
	 	switch (gameType) {
			case MASTER:	CentreText("SERIAL LINK ESTABLISHED", text_data (1,116, 8), RedText);
							CentreText("YOU ARE PLAYER ONE", text_data (1,150, 8), RedText);
		 					break;
			case SLAVE:		CentreText("SERIAL LINK ESTABLISHED", text_data (1,116, 8), RedText);
							CentreText("YOU ARE PLAYER TWO", text_data (1,150, 8), RedText);
							break;
			case SINGLE:	CentreText("NO OTHER PLAYSTATION PRESENT", text_data (1,116, 8), RedText);
							break;
		}
		selectData->combatComplete++;
		CommFirstRead = 1;
		delta_frames_on = 0;
		exit_status_on = 0;
		if(gameType == SLAVE) cycle_count = 0;
		else cycle_count = 1;
	}
}

//-------------------------------------------------------------------

#if	0
void		MenuMaster(ConfigData *gameData, int *currMenu)
{
	volatile char		send[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	volatile char		receive[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };

	fn_track("menumaster\n")

	send[0] = gameData->class;
	send[1] = gameData->gameType;
	send[2] = gameData->team;
	send[3] = gameData->shipNum;
	fn_track("master: sending shipnum to slave: ")
	fn_var(gameData->shipNum)
	send[4] = gameData->trackNum;
	send[5] = gameData->actTrackNum;
//	send[6] = (char) (*currMenu & 0x00FF); 
	send[6] = *currMenu;
	send[7] = gameData->exitMenu;
	send[8] = gameData->gameType;



	WriteSIOData((char *) &send, 12);
	

	ReadSIOData((char *) &receive, 12);

	#if	0
	gameData->class			= receive[0];
	gameData->gameType		= receive[1];
	gameData->team2			= receive[2];
	gameData->ship2Num		= receive[3];
	fn_track("master: receiving shipnum from slave: ")
	fn_var(gameData->ship2Num)
	gameData->trackNum		= receive[4];
	gameData->actTrackNum	= receive[5];
	gameData->oppMenu		= receive[6];
	gameData->exit2Menu		= receive[7];
	gameData->gameType 		= receive[8];
	raceType				= (int) receive[8];
	if(receive[8] & 0x80 != 0) raceType |= 0xFF00;
	#endif
}

//-------------------------------------------------------------------

void		CaptainBirdsEye(ConfigData *gameData, int *currMenu)
{
	volatile char		send[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	volatile char		receive[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };

	fn_track("captainbirdseye\n")

	ReadSIOData((char *) &receive, 12);
	gameData->class			= receive[0];
	gameData->gameType		= receive[1];
	gameData->team2			= receive[2];
	gameData->ship2Num		= receive[3];
	fn_track("slave: received shipnum from master: ")
	fn_var(gameData->ship2Num)
	gameData->trackNum		= receive[4];
	gameData->actTrackNum	= receive[5];
	gameData->oppMenu		= receive[6];
	gameData->exit2Menu		= receive[7];
	gameData->gameType		= receive[8];
	raceType				= (int) receive[8];
	if(receive[8] & 0x80 != 0) raceType |= 0xFF00;

	send[0] = gameData->class;
	send[1] = gameData->gameType;
	send[2] = gameData->team;
	send[3] = gameData->shipNum;
	fn_track("slave: sending shipnum to master: ")
	fn_var(gameData->shipNum)
	send[4] = gameData->trackNum;
	send[5] = gameData->actTrackNum;
//	send[6] = (char) (*currMenu & 0x00FF);
	send[6] = *currMenu;
	send[7] = gameData->exitMenu;
	send[8] = gameData->gameType;
	WriteSIOData((char *) &send, 12);
}
#endif

//-------------------------------------------------------------------

void		StartTwoPlayer(MusicPrims *musicPrims, SlideTextures *musicTex, SelectionData *selectData, char loadGames[][9], ConfigData *gameData)
{
	fn_track("starttwoplayer\n")
	
	fn_count++;
	fn_track("starttwoplayer entered: ")
	fn_var(fn_count)	

	if (gameData->oppMenu != START_TWO_PLAYER_MENU)
	{
		fn_track("StartTwoPlayer: shipNum: ")
		fn_var(gameData->shipNum)
		fn_track("StartTwoPlayer: ship2Num: ")
		fn_var(gameData->ship2Num)
	
		ownShip = gameData->shipNum;
		serialShip = gameData->ship2Num;
		
		fn_track("StartTwoPlayer: ownShip: ")
		fn_var(ownShip)
		fn_track("StartTwoPlayer: serialShip: ")
		fn_var(serialShip)

		if(gameType == MASTER) CentreText("WAITING FOR PLAYER TWO", text_data(1,116, 8), RedText);
		if(gameType == SLAVE) CentreText("WAITING FOR PLAYER ONE", text_data(1,116, 8), RedText);
	}
}
//-------------------------------------------------------------------

void		SetSerialData(ShipData *shipIndex, Object **shipShapes)
{
	int					i, j;
	TrackSection		*section;
	register short		sx, sy, sz;
	register short		cx, cy, cz;

	fn_track("setserialdata\n")
	
 	for(i=0; i<NO_OF_SHIPS; i++) {
	  	sy = rsin(shipIndex[i].hdg);
	 	cy = rcos(shipIndex[i].hdg);
	 	sx = rsin(shipIndex[i].pitch);
	 	cx = rcos(shipIndex[i].pitch);
 		sz = rsin(shipIndex[i].roll);
  		cz = rcos(shipIndex[i].roll);

 		if(gameType == MASTER) {
 			if(i == serialShip) {
				SetSkeletonPosition(shipShapes[shipIndex[i].highResTableIndex]->skeleton, shipIndex[i].ppivot.vx, shipIndex[i].ppivot.vy, shipIndex[i].ppivot.vz);
				SetSkeletonDirectionHPR(shipShapes[shipIndex[i].highResTableIndex]->skeleton, shipIndex[i].hdg, shipIndex[i].pitch, shipIndex[i].roll); 
	  			shipIndex[i].unitVecNose.vx = -frac(sy,cx);
 				shipIndex[i].unitVecNose.vy = -sx;
				shipIndex[i].unitVecNose.vz = frac(cy,cx);
				shipIndex[i].unitVecWing.vx = ((cy * cz) >> 12) + ((((sy * sz) >> 12) * sx) >> 12);
				shipIndex[i].unitVecWing.vy = -(sz * cx) >> 12;
				shipIndex[i].unitVecWing.vz = ((sy * cz) >> 12) - ((((cy * sx) >> 12) * sz) >> 12);
				shipIndex[serialShip].vpivot.vx = (shipIndex[serialShip].ppivot.vx - shipIndex[serialShip].vpivot.vx)<<6;
				shipIndex[serialShip].vpivot.vy = (shipIndex[serialShip].ppivot.vy - shipIndex[serialShip].vpivot.vy)<<6;
				shipIndex[serialShip].vpivot.vz = (shipIndex[serialShip].ppivot.vz - shipIndex[serialShip].vpivot.vz)<<6;
			}
		}
		else if(gameType == SLAVE) {
			if(i != ownShip) {
				SetSkeletonPosition(shipShapes[shipIndex[i].highResTableIndex]->skeleton, shipIndex[i].ppivot.vx, shipIndex[i].ppivot.vy, shipIndex[i].ppivot.vz);
				SetSkeletonDirectionHPR(shipShapes[shipIndex[i].highResTableIndex]->skeleton, shipIndex[i].hdg, shipIndex[i].pitch, shipIndex[i].roll);
	   			shipIndex[i].unitVecNose.vx = -frac(sy,cx);	 
 				shipIndex[i].unitVecNose.vy = -sx;
				shipIndex[i].unitVecNose.vz = frac(cy,cx);
				shipIndex[i].unitVecWing.vx = ((cy * cz) >> 12) + ((((sy * sz) >> 12) * sx) >> 12);
				shipIndex[i].unitVecWing.vy = -(sz * cx) >> 12;
				shipIndex[i].unitVecWing.vz = ((sy * cz) >> 12) - ((((cy * sx) >> 12) * sz) >> 12);
				shipIndex[i].vpivot.vx = (shipIndex[i].ppivot.vx - shipIndex[i].vpivot.vx)<<6;
				shipIndex[i].vpivot.vy = (shipIndex[i].ppivot.vy - shipIndex[i].vpivot.vy)<<6;
				shipIndex[i].vpivot.vz = (shipIndex[i].ppivot.vz - shipIndex[i].vpivot.vz)<<6;
			}							   
		}
	}
	section = track->sections;
	for(i=0; i<track->sectionCount; i++) {
		if(gameType == SLAVE) {
			for(j=0; j<NO_OF_SHIPS; j++) {
				if(j != ownShip) {
					if(shipIndex[j].nearSecNo == section->secNo) {
						shipIndex[j].nearTrkSect = section;
						break;
					}
				}
			}
		}
		if(gameType == MASTER) {
		 	if(shipIndex[serialShip].nearSecNo == section->secNo) {
		 		shipIndex[serialShip].nearTrkSect = section;
		 		break;
		 	}
		}
		section = section->nextSection.ptr;
	}
}

//-------------------------------------------------------------------

void		MenuMaster(ConfigData *gameData, int *currMenu)
{
	volatile char		send[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	volatile char		receive[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };

	fn_track("menumaster\n")

	send[0] = (char) (*currMenu & 0x00FF);
	send[1] = (char) ((*currMenu & 0xFF00) >> 8);
	send[2] = gameData->gameType;
	send[3] = gameData->shipNum;
	send[4] = gameData->team;
	send[5] = gameData->class;
	send[6] = gameData->trackNum;
	send[7] = gameData->actTrackNum;
	send[8] = gameData->exitMenu;
	WriteSIOData((char *) &send, 12);
	fn_track("master: sending shipnum to slave: ")
	fn_var(gameData->shipNum)
	

	ReadSIOData((char *) &receive, 12);
	gameData->oppMenu		= receive[0];
	gameData->gameType		= receive[2];
	gameData->ship2Num		= receive[3];
	gameData->team2			= receive[4];
	gameData->class			= receive[5];
	gameData->trackNum		= receive[6];
	gameData->actTrackNum	= receive[7];
	gameData->exit2Menu		= receive[8];
	raceType = (int) (receive[2] & 0x00FF);
	if((raceType & 0x0080) != 0) raceType |= 0xFF00;

	serialShip = gameData->ship2Num;
	
	fn_track("master: receiving shipnum from slave: ")
	fn_var(gameData->ship2Num)
	fn_track("slave: team: ")
	fn_var(gameData->team)

//	fn_var(gameData->class)
//	fn_var(gameData->trackNum)
//	fn_var(gameData->actTrackNum)
}

//-------------------------------------------------------------------

void		CaptainBirdsEye(ConfigData *gameData, int *currMenu)
{
	volatile char		send[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	volatile char		receive[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };

	fn_track("captainbirdseye\n")

	ReadSIOData((char *) &receive, 12);
	gameData->oppMenu		= receive[0];
	gameData->gameType		= receive[2];
	gameData->ship2Num		= receive[3];//AJY
//	gameData->ship2Num		= 0;
	gameData->team2			= receive[4];
	gameData->class			= receive[5];
	gameData->trackNum		= receive[6];
	gameData->actTrackNum	= receive[7];
	gameData->exit2Menu		= receive[8];
	raceType = (int) (receive[2] & 0x00FF);
	if((raceType & 0x0080) != 0) raceType |= 0xFF00;
	fn_track("slave: received shipnum from master: ")
	fn_var(gameData->ship2Num)

	serialShip = gameData->ship2Num;

	send[0] = (char) (*currMenu & 0x00FF);
	send[1] = (char) ((*currMenu & 0xFF00) >> 8);
	send[2] = gameData->gameType;
	send[3] = gameData->shipNum;	//AJY
//	send[3] = 1;	//AJY test master always 0, slave always 1!
	send[4] = gameData->team;
	send[5] = gameData->class;
	send[6] = gameData->trackNum;
	send[7] = gameData->actTrackNum;
	send[8] = gameData->exitMenu;
	WriteSIOData((char *) &send, 12);

//	gameData->shipNum = 1;
	fn_track("slave: sending shipnum to master: ")
	fn_var(gameData->shipNum)
	fn_track("slave: ownShip: ")
	fn_var(ownShip)
	fn_track("slave: team: ")
	fn_var(gameData->team)
//	fn_var(gameData->class)
//	fn_var(gameData->trackNum)
//	fn_var(gameData->actTrackNum)
}

//-------------------------------------------------------------------

