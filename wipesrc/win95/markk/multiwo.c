//-----------------------------------------------------------------------//
// WIPEOUT PC Multiplayer Generic game routines.
// Copyright(C). Sony Interactive. (05/09/95). Mark Kelly.
//-----------------------------------------------------------------------//

#include			<libsnd.h>
#include			"..\origsrc\standard.h"
//#include			<sys\file.h>
#include			<sys\ioctl.h>
#define				LANGUAGE_C
#include			<asm.h>
#include			<kernel.h>

#include			"..\origsrc\malloc.h"
#include			"..\origsrc\tim.h"
#include			"..\origsrc\object.h"
#include			"..\origsrc\skeleton.h"
#include			"..\origsrc\track.h"
#include			"..\origsrc\ships.h"
#include			"..\origsrc\weapon.h"
#include			"..\origsrc\control.h"
#include			"..\origsrc\droid.h"
#include			"..\origsrc\camera.h"
#include			"..\origsrc\menus.h"
#include			"..\origsrc\handlers.h"
#include			"..\origsrc\dynam.h"
#include			"..\origsrc\sound.h"
#include			"..\origsrc\sparks2.h"
#include			"..\origsrc\global.h"

#define				_SIZE_T_DEFINED_
#include			"c:\wc10\h\stdio.h"
#include			"c:\wc10\h\string.h"
#include			"..\markk\multiwo.h"
#include			"..\markk\serialwo.h"
#include			"..\markk\modemwo.h"
#include			"..\markk\modem.h"
#include			"..\markk\comio.h"
#include			"..\markk\netio.h"
#include			"..\markk\netwo.h"
#include			"..\markk\merror.h"
#include			"..\markk\debug.h"

//-----------------------------------------------------------------------//

long				CommMultiPlaySelect;
long				CommLinkCompleted;
long				CommLinkError;
long				CommFirstRead;
long 				AllPlayersReady;
long				NumMachinesWaiting;
long				StartCount;

//-----------------------------------------------------------------------//

/* Initialize wipeout link controller. */
void				_init_cont(void)
{
	fn_track("init cont\n")
	// reference all structures here
}

//-----------------------------------------------------------------------//

#ifndef USE_ATI
/* Initialize combat cable link. */
void				InitializeCombatLink(void)
{
	fn_track("initializecombatlink\n")

	CommLinkCompleted = 0;

	#ifdef	TEST_SERIAL	
	CommMultiPlaySelect = _HEAD_2_HEAD;		// temp
	#endif
	#ifdef	TEST_NETWORK
	CommMultiPlaySelect = _TCP_NETWORK;		// temp
	#endif
}

#endif //USE_ATI
//-----------------------------------------------------------------------//

/* Wipeout multiplayer menu function. */
void				MasterSlaveSIO(MusicPrims *musicPrims, SlideTextures *musicTex, SelectionData *selectData, char loadGames[][9], ConfigData *gameData)
{
//	fn_track("masterslavesio\n")
	
	if(CommMultiPlaySelect == _HEAD_2_HEAD)
	{
//		fn_track("masterslavesiohead\n")
		
		if(!selectData->combatComplete) CentreText("INITIALIZING SERIAL LINK UP", text_data (1,116, 8), RedText);
		if(neg1.head == 8960) DisplayButtons(130, 210, NBACK);
		else DisplayButtons(130, 210, PBACK);
		if(selectData->combatInit > 30 && !selectData->combatComplete) 
		{
			if(!CommLinkCompleted) 
			{
				MAK_OpenLink();
				if(global_mark_error) return;
				MAK_ResolveSerial();
				if(global_mark_error) return;
				selectData->combatComplete = 1;
			}
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
	}	// endif _HEAD_2_HEAD
	
	if(CommMultiPlaySelect == _TCP_NETWORK)
	{
//		fn_track("masterslavesionet\n")
		
		if(!selectData->combatComplete) CentreText("INITIALIZING NETWORK GAME", text_data (1,116, 8), RedText);
		if(neg1.head == 8960) DisplayButtons(130, 210, NBACK);
		else DisplayButtons(130, 210, PBACK);
		if(selectData->combatInit > 30 && !selectData->combatComplete) {
			if(!CommLinkCompleted) {
				MAK_OpenLink();
				if(global_mark_error) return;
//				TestHardWirePacketSend();
				MAK_HandshakeNetwork();
				if(global_mark_error) return;
				MAK_ResolveNetwork();
				if(global_mark_error) return;
				MAK_SynchroniseNetwork();
				if(global_mark_error) return;
				selectData->combatComplete = 1;
			}
		}
		selectData->combatInit++;
		if(selectData->combatComplete > 0 && selectData->combatComplete < 32) {
//		 	switch (gameType) {
		 	switch (CommMyShip) {
				case MASTER:	CentreText("NETWORK LINK ESTABLISHED", text_data (1,116, 8), RedText);
								CentreText("YOU ARE PLAYER ONE", text_data (1,150, 8), RedText);
			 					break;
				case SLAVE1:	CentreText("NETWORK LINK ESTABLISHED", text_data (1,116, 8), RedText);
								CentreText("YOU ARE PLAYER TWO", text_data (1,150, 8), RedText);
								break;
				case SLAVE2:	CentreText("NETWORK LINK ESTABLISHED", text_data (1,116, 8), RedText);
								CentreText("YOU ARE PLAYER THREE", text_data (1,150, 8), RedText);
								break;
				case SLAVE3:	CentreText("NETWORK LINK ESTABLISHED", text_data (1,116, 8), RedText);
								CentreText("YOU ARE PLAYER FOUR", text_data (1,150, 8), RedText);
								break;
				case SLAVE4:	CentreText("NETWORK LINK ESTABLISHED", text_data (1,116, 8), RedText);
								CentreText("YOU ARE PLAYER FIVE", text_data (1,150, 8), RedText);
								break;
				case SLAVE5:	CentreText("NETWORK LINK ESTABLISHED", text_data (1,116, 8), RedText);
								CentreText("YOU ARE PLAYER SIX", text_data (1,150, 8), RedText);
								break;
				case SLAVE6:	CentreText("NETWORK LINK ESTABLISHED", text_data (1,116, 8), RedText);
								CentreText("YOU ARE PLAYER SEVEN", text_data (1,150, 8), RedText);
								break;
				case SLAVE7:	CentreText("NETWORK LINK ESTABLISHED", text_data (1,116, 8), RedText);
								CentreText("YOU ARE PLAYER EIGHT", text_data (1,150, 8), RedText);
								break;

// use for connection failure
				case NSINGLE:	CentreText("NO OTHER PLAYER PRESENT", text_data (1,116, 8), RedText);
								break;
			}
			selectData->combatComplete++;
			exit_status_on = 0;
		}
	}	// endif _TCP_NETWORK
}

//-----------------------------------------------------------------------//

/* Wipeout menu function. */
void				StartTwoPlayer(MusicPrims *musicPrims, SlideTextures *musicTex, SelectionData *selectData, char loadGames[][9], ConfigData *gameData)
{
	int		i;

	fn_track("starttwoplayer\n")
	
	StartCount++;
	fn_track("start count: ")
	fn_var(StartCount)
	
	fn_count++;
	fn_track("starttwoplayer entered: ")
	fn_var(fn_count)	

	if(CommMultiPlaySelect == _HEAD_2_HEAD)
	{
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
	
	if(CommMultiPlaySelect == _TCP_NETWORK)
	{
		ownShip = gameData->shipNum;
		gameData->exitMenu = 0xFF;

		// see how many machines we're waiting for...
		NumMachinesWaiting = CommNetPlayers;
		for(i=0; i<CommNetPlayers; i++)
		{
			fn_track("exit: ")
			fn_var(NetworkConfigTable.ExitMenu[i]);
			if(NetworkConfigTable.ExitMenu[i] == 0xFF) NumMachinesWaiting--;
		}
		
				// display message depending upon how many players we're waiting for...
		fn_track("waiting for: ")
		fn_var(NumMachinesWaiting)
		switch (NumMachinesWaiting)
		{
			case	7		:	{
									fn_track("WAITING FOR SEVEN PLAYERS\n")
									CentreText("WAITING FOR SEVEN PLAYERS", text_data(1,116, 8), RedText);
									break;
								}
			case	6		:	{
									fn_track("WAITING FOR SIX PLAYERS\n")
									CentreText("WAITING FOR SIX PLAYERS", text_data(1,116, 8), RedText);
									break;
								}
			case	5		:	{	
									fn_track("WAITING FOR FIVE PLAYERS\n")
									CentreText("WAITING FOR FIVE PLAYERS", text_data(1,116, 8), RedText);
									break;
								}
			case	4		:	{
									fn_track("WAITING FOR FOUR PLAYERS\n")
									CentreText("WAITING FOR FOUR PLAYERS", text_data(1,116, 8), RedText);
									break;
								}
			case	3		:	{
									fn_track("WAITING FOR THREE PLAYERS\n")
									CentreText("WAITING FOR THREE PLAYERS", text_data(1,116, 8), RedText);
									break;
								}
			case	2		:	{
									fn_track("WAITING FOR TWO PLAYERS\n")
									CentreText("WAITING FOR TWO PLAYERS", text_data(1,116, 8), RedText);
									break;
								}
			case	1		:	{
									fn_track("WAITING FOR ONE PLAYER\n")
									CentreText("WAITING FOR ONE PLAYER", text_data(1,116, 8), RedText);
									break;
								}
			case	0		:	{
									fn_track("ALL PLAYERS READY\n")
									CentreText("ALL PLAYERS READY", text_data(1,116, 8), RedText);
									AllPlayersReady = 1;	// let's go!
									break;
								}
		}
	
	}
}

//-----------------------------------------------------------------------//

/* Wipeout game function called every frame. */
void				SetSerialData(ShipData *shipIndex, Object **shipShapes)
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

//-----------------------------------------------------------------------//

/* Wipeout MASTER menu function. */
void				MenuMaster(ConfigData *gameData, int *currMenu)
{
	volatile char						send[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	volatile char						receive[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	
	fn_track("menumaster\n")
	
	/* Serial Menu Transfer Code. */
	if(CommMultiPlaySelect == _HEAD_2_HEAD)
	{
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
		//fn_var(gameData->class)
		//fn_var(gameData->trackNum)
		//fn_var(gameData->actTrackNum)
	}

	/* MASTER network menu transfer system. */
	if(CommMultiPlaySelect == _TCP_NETWORK)	MAK_MasterNetworkMenus(gameData);
}

//-----------------------------------------------------------------------//

/* Wipeout SLAVE menu function. */
void				CaptainBirdsEye(ConfigData *gameData, int *currMenu)
{
	volatile char						send[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	volatile char						receive[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };

	fn_track("captainbirdseye\n")
	
	/* Serial Menu Transfer Code. */
	if(CommMultiPlaySelect == _HEAD_2_HEAD)
	{
		ReadSIOData((char *) &receive, 12);
		gameData->oppMenu		= receive[0];
		gameData->gameType		= receive[2];
		gameData->ship2Num		= receive[3];//AJY
//		gameData->ship2Num		= 0;
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
//		send[3] = 1;	//AJY test master always 0, slave always 1!
		send[4] = gameData->team;
		send[5] = gameData->class;
		send[6] = gameData->trackNum;
		send[7] = gameData->actTrackNum;
		send[8] = gameData->exitMenu;
		WriteSIOData((char *) &send, 12);

//		gameData->shipNum = 1;
		fn_track("slave: sending shipnum to master: ")
		fn_var(gameData->shipNum)
		fn_track("slave: ownShip: ")
		fn_var(ownShip)
		fn_track("slave: team: ")
		fn_var(gameData->team)
//		fn_var(gameData->class)
//		fn_var(gameData->trackNum)
//		fn_var(gameData->actTrackNum)
	}
	
	/* SLAVE network menu transfer system .*/
	if(CommMultiPlaySelect == _TCP_NETWORK) MAK_SlaveNetworkMenus(gameData);
}

//-----------------------------------------------------------------------//

/* Open multiplayer environment. */
long				MAK_OpenLink(void)
{
	long	error = OK;

	fn_track("mak openlink\n")
	fn_track("WIPEOUT pre-Alpha Serial v1.6\n")

	// will be setup via option screen when finished
	CommFirstRead = 0;
#ifdef	MAK_LEAD
	CommSerialMachine = _LEAD;
#endif
#ifdef	MAK_FOLLOW
	CommSerialMachine = _FOLLOW;
#endif
#ifdef	TEST_SERIAL		/* test option setup. */
	CommMultiPlaySelect = _HEAD_2_HEAD;
	CommSerialConnectMethod = _DIRECT;
	CommSerialDevice = _COM1;
	CommSerialBaudRate = _57600_BAUD;
//	CommSerialBaudRate = _115200_BAUD;
	CommSerialDataSize = _8BIT_DATA;
	CommSerialParity = _NO_PARITY;
	CommSerialStopSize = _1BIT_STOP;
#endif
#ifdef	TEST_NETWORK
	CommNetSocket = 0x00000000;
	CommMultiPlaySelect = _TCP_NETWORK;
	CommNetPlayers = TEST_NET_PLAYERS;
	CommNetSocket += 0x00004000;
#endif
#ifdef	TEST_MODEM
	CommSerialConnectMethod = _DIALUP;
	if(CommSerialMachine == _LEAD) CommModemMachine = _DIAL;
	if(CommSerialMachine == _FOLLOW) CommModemMachine = _ANSWER;
	if(CommModemMachine == _DIAL) strcpy(CommModemInitString, InitStringA);
	if(CommModemMachine == _ANSWER) strcpy(CommModemInitString, InitStringB);
	strcpy(CommModemSetupString, SetupString1);
	strcpy(CommModemDialupString, DialupStringA);
	strcpy(CommModemAnswerString, AnswerString);
	strcpy(CommModemHangupString, HangupString);
#endif	

	// initialise communication system.
	error = MAK_OpenDevice();
	if(error) {
		CommLinkError = TRUE;
		fn_track("failure opening device\n")
		MAK_ReportConnectFailure(error);
		return(error);
	}

	// connect via serial link
	if(CommMultiPlaySelect == _HEAD_2_HEAD)
	{
		#ifdef	TEST_MODEM
		// connect modem and telephone lines if required
		if(CommSerialConnectMethod == _DIALUP) {
			error = connectmodem();
			if(error) {
				CommLinkError = TRUE;
				fn_track("failure connecting modem\n")
				MAK_ReportConnectFailure(error);
				return(error);
			}
		}
		#endif	//TEST_MODEM

		// serial handshake
		error = MAK_SerialSynchronize();
		if(error) {
			CommLinkError = TRUE;
			fn_track("failure handshaking\n")
			MAK_ReportConnectFailure(error);
			return(error);
		}
	}
	
	// connect via IPX Network
	if(CommMultiPlaySelect == _TCP_NETWORK)
	{
		// initialize IPX system
		error =	MAK_InitializeNetwork();
		if(error) {
			CommLinkError = TRUE;
			fn_track("failure initializing IPX\n")
			MAK_ReportConnectFailure(error);
			return(error);
		}
	}
		
	// completed open link function successfully
	fn_track("openlink completed\n")
	CommLinkCompleted = 1;
	return (OK);
}

//-----------------------------------------------------------------------//

/* Close multiplayer environment. */
long				MAK_CloseLink(void)
{
	long	error = OK;

	fn_track("mak closelink\n")

	if(CommSerialConnectMethod == _DIALUP) {
		hangup();
		disconnectmodem();
	}

	if(CommMultiPlaySelect == _TCP_NETWORK)
	{
		// de-initialize IPX system
		error =	MAK_TerminateNetwork();
		if(error) {
			CommLinkError = TRUE;
			fn_track("failure terminating IPX\n")
			MAK_ReportConnectFailure(error);
		}
	}
		
	MAK_CloseDevice();

	// completed open link function successfully
	return (OK);
}

//-----------------------------------------------------------------------//

/* Open linkup device. */
long				MAK_OpenDevice(void)
{
	long	open_error = OK;

	fn_track("mak opendevice\n")

	/* Initiate Communication Device. */
	switch (CommMultiPlaySelect) {
		case	_SINGLE_PLAY	:	//CommNumLinkedCars = 1;
									break;
		case	_HEAD_2_HEAD	:	open_error = copen(CommSerialDevice, CommSerialBaudRate, CommSerialDataSize, CommSerialParity, CommSerialStopSize);
									if(open_error) return (open_error);
									break;
		case	_TCP_NETWORK	:	open_error = nopen(CommNetSocket);
									break;
		default					:	break;
	}

	delay(_1_FRAME*15);
	
	return (OK);
}

//-----------------------------------------------------------------------//

/* Close linkup device. */
long				MAK_CloseDevice(void)
{
	fn_track("mak closedevice\n")

	/* Terminate Communication Device. */
	switch (CommMultiPlaySelect)
	{
		case	_SINGLE_PLAY	:	break;	
		case	_HEAD_2_HEAD	:	cclose(CommSerialDevice);
									break;
		case	_TCP_NETWORK	:	nclose(CommNetSocket);
									break;
		default					:	break;
	}
	
	return (OK);
}	

//-----------------------------------------------------------------------//

