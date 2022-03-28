//-------------------------------------------------------------------

#include 			"..\origsrc\standard.h"
#include 			"..\origsrc\malloc.h"
#include 			"..\origsrc\tim.h"
#include 			"..\origsrc\object.h"
#include 			"..\origsrc\skeleton.h"
#include 			"..\origsrc\track.h"
#include 			"..\origsrc\ships.h"
#include 			"..\origsrc\weapon.h"
#include 			"..\origsrc\control.h"
#include 			"..\origsrc\droid.h"
#include 			"..\origsrc\camera.h"
#include 			"..\origsrc\menus.h"
#include 			"..\origsrc\handlers.h"
#include 			"..\origsrc\dynam.h"
#include 			"..\origsrc\sound.h"
#include 			"..\origsrc\sparks2.h"
#include 			"..\origsrc\global.h"

#define				_SIZE_T_DEFINED_
#include			"c:\watcom\h\dos.h"
#include			"c:\watcom\h\stdio.h"
#include			"c:\watcom\h\stdlib.h"
#include			"c:\watcom\h\string.h"

#include			"..\markk\pccombat.h"
#include			"..\markk\comio.h"
#include			"..\markk\comerr.h"
#include			"..\markk\input.h"
//#include			"..\markk\modem.h"
#include			"..\markk\debug.h"
#include			"..\markk\commwo.h"

//-------------------------------------------------------------------

long				CommMultiPlaySelect;
long				CommNetPlayers;
long				CommSerialConnectMethod;
long				CommSerialMachine;
long				CommSerialDevice;
long				CommSerialBaudRate;
long				CommSerialDataSize;
long				CommSerialParity;
long				CommSerialStopSize;
long				CommModemMachine;
char				CommModemInitString[256];
char				CommModemSetupString[256];
char				CommModemDialupString[256];
char				CommModemAnswerString[256];
char				CommModemHangupString[256];
long				CommFirstRead;
long				CommLinkCompleted;
long				CommLinkError;
char				CommMasterCount;
char				CommMasterCount2;

//-------------------------------------------------------------------

/* Open a link between 2 or more machines. */
void				MAK_OpenLink(void)
{
	long	connect_error = OK;

	fn_track("mak_openlink\n")
	fn_track("WIPEOUT pre-Alpha Serial v1.4a\n")

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
//	CommSerialBaudRate = _57600_BAUD;
	CommSerialBaudRate = _115200_BAUD;
	CommSerialDataSize = _8BIT_DATA;
	CommSerialParity = _NO_PARITY;
	CommSerialStopSize = _1BIT_STOP;
#endif
#ifdef	TEST_NETWORK
	CommNetPlayers = 2;
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
	connect_error = MAK_OpenDevice();
	if(connect_error) {
		CommLinkError = TRUE;
		fn_track("failure opening device\n")
		MAK_ReportConnectFailure(connect_error);
		return;
	}

#ifdef	TEST_MODEM
	// connect modem and telephone lines if required
	if(CommSerialConnectMethod == _DIALUP) {
		connect_error = MAK_ConnectModem();
		if(connect_error) {
			CommLinkError = TRUE;
			fn_track("failure connecting modem\n")
			MAK_ReportConnectFailure(connect_error);
			return;
		}
	}
#endif

	// connect the machines to be linked
	connect_error = MAK_SerialHandshake();
	if(connect_error) {
		CommLinkError = TRUE;
		fn_track("failure handshaking\n")
		MAK_ReportConnectFailure(connect_error);
		return;
	}
	fn_track("openlink completed\n")
	CommLinkCompleted = 1;
}

//-------------------------------------------------------------------

/* Close Link and terminate environment. */
void				MAK_CloseLink(void)
{
	fn_track("mak_closelink\n")

#ifdef	TEST_MODEM
	if(CommSerialConnectMethod == _DIALUP) {
		MAK_Hangup();
		MAK_DisconnectModem();
	}
#endif	
	
	MAK_CloseDevice();
}

//-------------------------------------------------------------------

/* Initialise Comm Device and Linkup Environment. */
long				MAK_OpenDevice(void)
{
	long	open_error = OK;

	fn_track("mak_opendevice\n")

	/* Initiate Communication Device. */
	switch (CommMultiPlaySelect) {
		case	_SINGLE_PLAY	:	//CommNumLinkedCars = 1;
									break;
		case	_HEAD_2_HEAD	:	//CommNumLinkedCars = 2;
									open_error = copen(CommSerialDevice, CommSerialBaudRate, CommSerialDataSize, CommSerialParity, CommSerialStopSize);
									if(open_error) return (open_error);
									break;
		case	_TCP_NETWORK	:	//CommNumLinkedCars = CommNetPlayers;
									//nopen();
									break;
		default					:	break;
	}

	delay(_1_FRAME*15);
	
	return (OK);
}

//-------------------------------------------------------------------

/* Terminate Comm Device. */
void				MAK_CloseDevice(void)
{
	fn_track("mak_closedevice\n")

	/* Terminate Communication Device. */
	switch (CommMultiPlaySelect)
	{
		case	_SINGLE_PLAY	:	break;	
		case	_HEAD_2_HEAD	:	cclose(CommSerialDevice);
									break;
		case	_TCP_NETWORK	:	//nclose();
									break;
		default					:	break;
	}
}	

//-------------------------------------------------------------------

long				MAK_SerialHandshake(void)
{
	long	connect_error = OK;
	long	no_connection = 1;
	long	retries = _HANDSHAKE_RETRIES;

	fn_track("mak_serialhandshake\n")

	CommMasterCount = 0;
	CommMasterCount2 = 0;
	
	// first handshake and synchronise both machines (keep a count for master/slave select later)
	while(no_connection) {
		delay(_1_FRAME);
		cwrite(CommSerialDevice, "HANDHAND", _SYNC_PACKET_SIZE*2);
		
		// collect 8 bytes of data if there is any
		if((com_ReceiveTail > com_ReceiveHead) && (com_ReceiveTail - com_ReceiveHead >= 8)) {
			no_connection = MAK_CheckHandshake();
		}
		if((com_ReceiveTail < com_ReceiveHead) && ((_RECEIVE_BUFFER_SIZE - com_ReceiveHead + com_ReceiveTail) >= 8)) {
			no_connection = MAK_CheckHandshake();
		}
		
		// increment master counter
		CommMasterCount++;
		
		// keep retrying until we decide we've not got a connection.
		if(!retries--) return (_ERR_HANDSHAKING);
	}
	
	// now synchronise both machine's circular buffer pointers.
	delay(_1_FRAME);
	MAK_SendSync();
	delay(_1_FRAME);
	connect_error = MAK_SearchSync();
	if(connect_error) return (_ERR_SYNCHRONISING);
	delay(_1_FRAME);
	MAK_SendAckn();
	delay(_1_FRAME);
	connect_error = MAK_SearchAckn();
	if(connect_error) return (_ERR_SYNCHRONISING);
	
	fn_track("serialhandshake completed\n")
	return (OK);
}

//-------------------------------------------------------------------

long				MAK_CheckHandshake(void)
{
	long	SizeToEnd;
	long	WrapSize;
	char	handshake[9];
	char	*hptr = handshake;
	long	i;

	fn_track("mak_checkhandshake\n")

	if((com_ReceiveHead + 8) > _RECEIVE_BUFFER_SIZE) {
		// Wrap Read
		SizeToEnd = _RECEIVE_BUFFER_SIZE - com_ReceiveHead;
		memcpy(handshake, &com_ReceiveBuffer[com_ReceiveHead], SizeToEnd);
		WrapSize = 8 - SizeToEnd;
		memcpy(handshake+SizeToEnd, &com_ReceiveBuffer, WrapSize);
		com_ReceiveHead = WrapSize;
	}
	else {
		memcpy(handshake, &com_ReceiveBuffer[com_ReceiveHead], 8);
		com_ReceiveHead += 8;
	}

//	SerialDataReport(handshake, 8);
	
	// now look for HAND appearing somewhere in this array.
	for(i=0; i<8; i++, hptr++) {
		if(memcmp("HAND", hptr, 4) == 0) return 0;
	}
	return 1;
}

//-------------------------------------------------------------------

/* handshake failure message. */
void				MAK_ReportConnectFailure(long error_code)
{
	fn_track("mak_reportconnectfailure\n")

	switch (error_code) {
		case	OK					:	WriteVGAString(0, 0, "Connect Error(00): Serial link OK.\n");
										break;
		case	_ERR_INSTALLING		:	WriteVGAString(0, 0, "Connect Error(10): Cannot install device.\n");
										break;
		case	_ERR_INITIALIZING	:	WriteVGAString(0, 0, "Connect Error(11): Cannot initialize modem.\n");
										break;
		case	_ERR_DIALLING		:	WriteVGAString(0, 0, "Connect Error(12): Cannot dial modem.\n");
										break;
		case	_ERR_CONNECTING		:	WriteVGAString(0, 0, "Connect Error(13): Cannot connect to other modem.\n");
										break;
		case	_ERR_ANSWERING		:	WriteVGAString(0, 0, "Connect Error(14): Cannot answer using modem.\n");
										break;
		case	_ERR_HANDSHAKING	:	WriteVGAString(0, 0, "Connect Error(15): Cannot handshake with other machine.\n");
										break;
		case	_ERR_SYNCHRONISING	:	WriteVGAString(0, 0, "Connect Error(16): Cannot synchronize with other machine.\n");
										break;
		case	_ERR_TRANSFERRING	:	WriteVGAString(0, 0, "Connect Error(17): Cannot Transfer data.\n");
										break;
		case	_ERR_TIMEOUT		:	WriteVGAString(0, 0, "Connect Error(18): Timeout Error.\n");
										break;
		case	_ERR_RESOLVING		:	WriteVGAString(0, 0, "Connect Error(19): Cannot resolve master & slave.\n");
										break;
	}
	while(1) {;}; // temp lock on failure
}

//-------------------------------------------------------------------

/* Write out a SYNC packet. */
void				MAK_SendSync(void)
{
	fn_track("mak_sendsync\n")

	cwrite(CommSerialDevice, "SYNC", _SYNC_PACKET_SIZE);
}

//-------------------------------------------------------------------

/* Write out an ACKN packet. */
void				MAK_SendAckn(void)
{
	fn_track("mak_sendackn\n")

	cwrite(CommSerialDevice, "ACKN", _SYNC_PACKET_SIZE);
}

//-------------------------------------------------------------------

/* Locate SYNC packet in receive buffer. */
long				MAK_SearchSync(void)
{
	long	no_sync = 1;
	char	sync[4] = { 0, 0, 0, 0 };
	
	fn_track("mak_searchsync\n")

	while(no_sync) {
//		delay(_1_FRAME);
		delay(5);
		memset(sync, '\0', 4);
		while(sync[0] != 'S') cread(CommSerialDevice, &sync[0], 1);
		cread(CommSerialDevice, &sync[1], 3);
		if(memcmp("SYNC", &sync[0], 4) == 0) no_sync = 0;
	}
	return (OK);
}

//-------------------------------------------------------------------

/* Locate ACKN packet in receive buffer. */
long				MAK_SearchAckn(void)
{
	long	no_sync = 1;
	char	sync[4] = { 0, 0, 0, 0 };
	
	fn_track("mak_searchackn\n")

	while(no_sync) {
//		delay(_1_FRAME);
		delay(5);
		memset(sync, '\0', 4);
		while(sync[0] != 'A') cread(CommSerialDevice, &sync[0], 1);
		cread(CommSerialDevice, &sync[1], 3);
		if(memcmp("ACKN", &sync[0], 4) == 0) no_sync = 0;
	}	
	
	return (OK);
}

//-------------------------------------------------------------------

/* Select which machine is master and which is slave using rand number comparison. */
void				MAK_SelectMasterSlave(void)
{
	long	connect_error = OK;

	connect_error = MAK_ResolveMasterSlave();
	if(connect_error) {
		CommLinkError = TRUE;
		fn_track("failure handshaking\n")
		MAK_ReportConnectFailure(connect_error);
		return;
	}

	fn_track("resolve completed\n")
}

//-------------------------------------------------------------------

/* Select which machine is master and which is slave using rand number comparison. */
long				MAK_ResolveMasterSlave(void)
{
	long	not_resolved = 1;

	cwrite(CommSerialDevice, &CommMasterCount, 1);
	delay(_1_FRAME);
	cread(CommSerialDevice, &CommMasterCount2, 1);
	
	if(CommMasterCount2 > CommMasterCount) {
		gameType = SLAVE;
		not_resolved = 0;
		fn_track("resolved as slave\n")
	}
	else if(CommMasterCount2 < CommMasterCount) {
		gameType = MASTER;
		not_resolved = 0;
		fn_track("resolved as master\n")
	}
	else return (_ERR_RESOLVING);
	
	return (OK);
}

//-------------------------------------------------------------------
