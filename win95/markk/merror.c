//-----------------------------------------------------------------------//
// WIPEOUT PC Multiplayer error handling routines.
// Copyright(C). Sony Interactive. (05/09/95). Mark Kelly.
//-----------------------------------------------------------------------//

#include			"c:\wc10\h\stdio.h"
#include			"..\markk\merror.h"
#include			"..\markk\debug.h"

long				global_mark_error;

//-----------------------------------------------------------------------//

/* handshake failure message. */
void				MAK_ReportConnectFailure(long error)
{
	fn_track("mak_reportconnectfailure\n")
	
	global_mark_error=error;

#if COMMENTED_OUT
	if(!error) return;
	switch (error) {
		// serial errors
		case	_ERR_BAD_SERIAL_PORT		:	WriteVGAString(0, 0, "Error(10): Bad serial port.\n");
												break;
		case	_ERR_SERIAL_PORT_NOT_FOUND	:	WriteVGAString(0, 0, "Error(11): Serial port not found.\n");
												break;
		case	_ERR_INSTALLING_SERIAL		:	WriteVGAString(0, 0, "Error(12): Cannot install serial device.\n");
												break;
		case	_ERR_HANDSHAKING_SERIAL		:	WriteVGAString(0, 0, "Error(15): Serial handshake failure.\n");
												break;
		case	_ERR_SYNCHRONISING_SERIAL	:	WriteVGAString(0, 0, "Error(16): Serial synchronize failure.\n");
												break;
		case	_ERR_TRANSFERRING_SERIAL	:	WriteVGAString(0, 0, "Error(17): Serial transfer error.\n");
												break;
		case	_ERR_TIMEOUT_SERIAL			:	WriteVGAString(0, 0, "Error(18): Serial timeout error.\n");
												break;
		case	_ERR_RESOLVING_SERIAL		:	WriteVGAString(0, 0, "Error(19): Serial resolve failure.\n");
												break;
						
		// modem errors
		case	_ERR_INSTALLING_MODEM		:	WriteVGAString(0, 0, "Error(20): Cannot install modem device.\n");
												break;
		case	_ERR_INITIALIZING_MODEM		:	WriteVGAString(0, 0, "Error(21): Cannot initialize modem.\n");
												break;
		case	_ERR_DIALLING_MODEM			:	WriteVGAString(0, 0, "Error(22): Cannot dial modem.\n");
												break;
		case	_ERR_CONNECTING_MODEM		:	WriteVGAString(0, 0, "Error(23): Cannot connect to other modem.\n");
												break;
		case	_ERR_ANSWERING_MODEM		:	WriteVGAString(0, 0, "Error(24): Cannot answer on modem.\n");
												break;
		case	_ERR_TIMEOUT_MODEM			:	WriteVGAString(0, 0, "Error(25): Modem not responding.\n");
												break;
	
		// network errors
		case	_ERR_IPX_NOT_PRESENT		:	WriteVGAString(0, 0, "Error(30): IPX Network software not detected.\n");
												break;
		case	_ERR_IPX_SYNCHRONISING		:	WriteVGAString(0, 0, "Connect Error(31): IPX synchronize failure.\n");
												break;
		case	_ERR_IPX_SOCKET_FULL		:	WriteVGAString(0, 0, "Connect Error(32): IPX socket full.\n");
												break;
		case	_ERR_IPX_SOCKET_OPEN		:	WriteVGAString(0, 0, "Connect Error(33): IPX socket already in use.\n");
												break;
		case	_ERR_IPX_OPEN_ERROR			:	WriteVGAString(0, 0, "Connect Error(34): IPX open socket failure.\n");
												break;
		case	_ERR_IPX_TOO_MANY_EVENTS	:	WriteVGAString(0, 0, "Connect Error(35): IPX too many event blocks.\n");
												break;
		case	_ERR_NETWARE_NOT_LOADED		:	WriteVGAString(0, 0, "Connect Error(36): Netware not loaded.\n");
												break;
		case	_ERR_IPX_DATA_TOO_BIG		:	WriteVGAString(0, 0, "Connect Error(37): Not enough IPX space for data.\n");
												break;
		case	_ERR_IPX_TIMEOUT_NETWORK	:	WriteVGAString(0, 0, "Connect Error(38): IPX Network timeout error.\n");
												break;
														
		// unknown errors
		default								: 	WriteVGAString(0, 0, "Connect Error(??): Unknown error.\n");
												break;
		}
	while(1) {;}; // temp lock on failure
#endif
}

//-----------------------------------------------------------------------//
