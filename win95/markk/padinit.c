/* Source File: padinit.c */
/* This function initializes a controller. The type of controller is
	specified by mode. At present, only mode type 0 is supported. */
#include	"c:\watcom\h\dos.h"
#include	"..\markk\libetc.h"

long	PadInit(unsigned long	mode)
{
	/* Check for supported modes. */
	if(mode != 0) return -1;

	/*
	if(device_available != 0)
	{
		Increment the number of connected controllers.
		etc_Controllers++;
	}
	*/

	/* Flush Controller Buffers. */
	etc_TransferBuffer1[0] = NG;
	etc_TransferBuffer2[0] = NG;

	etc_InputDeviceSelect = _KEYBOARD;
	ConfigureKeyboard();

	/* Install Handler if not already done. */
	if(!etc_KeyboardHandler) InstallProtectedModeKeyboardHandler();

	/* Return the number of controllers connected. */
	etc_KeyboardRegister = 0;
	etc_KeyboardSwitch = _DEVICE_ON;

	/* Initialize Controller Buffers. */
	etc_TransferBuffer1[0] = OK;
	etc_TransferBuffer2[0] = OK;

	return 0;
}



