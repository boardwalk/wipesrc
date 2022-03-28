//-----------------------------------------------------------------------//
// WIPEOUT PC Multiplayer error handling routines header.
// Copyright(C). Sony Interactive. (05/09/95). Mark Kelly.
//-----------------------------------------------------------------------//

// no error
#define					OK							0

// serial errors
#define					_ERR_BAD_SERIAL_PORT		0x10
#define					_ERR_SERIAL_PORT_NOT_FOUND	0x11
#define					_ERR_INSTALLING_SERIAL		0x12
#define					_ERR_HANDSHAKING_SERIAL		0x15
#define					_ERR_SYNCHRONISING_SERIAL	0x16
#define					_ERR_TRANSFERRING_SERIAL	0x17
#define					_ERR_TIMEOUT_SERIAL			0x18
#define					_ERR_RESOLVING_SERIAL		0x19

// modem errors
#define					_ERR_INSTALLING_MODEM		0x20
#define					_ERR_INITIALIZING_MODEM		0x21		// error when no modem init
#define					_ERR_DIALLING_MODEM			0x22		// error during dialup
#define					_ERR_CONNECTING_MODEM		0x23		// error during connection
#define					_ERR_ANSWERING_MODEM		0x24		// error during auto-answer
#define					_ERR_TIMEOUT_MODEM			0x25

// network errors
#define					_ERR_IPX_NOT_PRESENT		0x30
#define					_ERR_IPX_SYNCHRONISING		0x31
#define					_ERR_IPX_SOCKET_FULL		0x32
#define					_ERR_IPX_SOCKET_OPEN		0x33
#define					_ERR_IPX_OPEN_ERROR			0x34
#define					_ERR_IPX_TOO_MANY_EVENTS	0x35
#define					_ERR_NETWARE_NOT_LOADED		0x36
#define					_ERR_IPX_DATA_TOO_BIG		0x37
#define					_ERR_IPX_TIMEOUT_NETWORK	0x38


//-----------------------------------------------------------------------//
