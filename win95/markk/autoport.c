/* Autodetect device's port address, using BIOS RAM area. */
#include	"..\markk\comio.h"
#include	"..\markk\comerr.h"

short	autoport(long device)
{
	short	*port;

	switch (device)
	{
		case	_COM1	:	port = (short *) _COM1_PORT;
							break;
		case	_COM2	:	port = (short *) _COM2_PORT;
							break;
		case	_COM3	:	port = (short *) _COM3_PORT;
							break;
		case	_COM4	:	port = (short *) _COM4_PORT;
							break;
		default			:	return (-1);
	}

	if(*port) return (*port);
	else return (0);
}
