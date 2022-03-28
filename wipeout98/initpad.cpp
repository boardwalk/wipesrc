/* This function registers the incoming data buffer for each controller. */
#include	"\wipesrc\win95\markk\libapi.h"

long	InitPAD(void *bufA, long lenA, void *bufB, long lenB)
{
	/* Check for available controller. */
	if((etc_InputDeviceSelect & 0x07) == 0) return 0;

	/* Switch Off all input devices. */
	etc_KeyboardSwitch = _DEVICE_OFF;
	etc_Mouse1Switch = _DEVICE_OFF;
	etc_Mouse2Switch = _DEVICE_OFF;
	etc_Joystick1Switch = _DEVICE_OFF;
	etc_Joystick2Switch = _DEVICE_OFF;

	/* Setup the transfer buffer ptrs using above parameters. */
	etc_TransferBuffer1 = (volatile char *)bufA;
	etc_TransferBuffer2 = (volatile char *)bufB;

	/* Initialize Controller Status. */
	etc_TransferBuffer1[0] = NG;
	etc_TransferBuffer2[0] = NG;

	/* Initialized Successfully. */
	return 1;
}
