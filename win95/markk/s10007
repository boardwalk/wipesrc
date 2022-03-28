/* This function starts controller status reading, which is triggered by a
	vertical blanking interrupt (on PSX). */
#include	"..\markk\libapi.h"

long	StartPAD(void)
{
	/* Flush Controller Buffers. */
	etc_TransferBuffer1[0] = NG;
	etc_TransferBuffer2[0] = NG;

	/* Install New Handlers if not already done. */
	switch (etc_InputDeviceSelect)
	{
	 	case	_KEYBOARD	:	
		{
			ConfigureKeyboard();
			if(!etc_KeyboardHandler) InstallProtectedModeKeyboardHandler();
			etc_KeyboardSwitch = _DEVICE_ON;

			/* Initialize Controller Buffers. */
			etc_TransferBuffer1[0] = NG;
			etc_TransferBuffer2[0] = NG;
		}
		break;

		case	_MOUSE1		:
		{
			ConfigureMouse1();
#ifdef PORT_MOUSE
			if(!etc_MouseHandler) InstallProtectedModeMouseHandler();
#endif
			etc_Mouse1Switch = _DEVICE_ON;

			/* Initialize Controller Buffers. */
			etc_TransferBuffer1[0] = NG;
			etc_TransferBuffer2[0] = NG;
		}
		break;

		case	_MOUSE2		:
		{
			ConfigureMouse2();
#ifdef PORT_MOUSE
			if(!etc_MouseHandler) InstallProtectedModeMouseHandler();
#endif
			etc_Mouse2Switch = _DEVICE_ON;

			/* Initialize Controller Buffers. */
			etc_TransferBuffer1[0] = NG;
			etc_TransferBuffer2[0] = NG;
		}
		break;

		case	_JOYSTICK1	:
		{
//			if(!etc_TimerHandler) init_timer();
			ConfigureJoystick1();
//			if(!etc_JoystickHandler) InstallProtectedModeJoystickHandler();
			etc_Joystick1Switch = _DEVICE_ON;

			/* Initialize Controller Buffers. */
			etc_TransferBuffer1[0] = NG;
			etc_TransferBuffer2[0] = NG;
		}
		break;

		case	_JOYSTICK2	:
		{
//			if(!etc_TimerHandler) init_timer();
			ConfigureJoystick2();
//			if(!etc_JoystickHandler) InstallProtectedModeJoystickHandler();
			etc_Joystick2Switch = _DEVICE_ON;

			/* Initialize Controller Buffers. */
			etc_TransferBuffer1[0] = NG;
			etc_TransferBuffer2[0] = NG;
		}
		break;

		default				:
		{
			/* failure to start a controller. */
			return 0;
		}
		break;
	}

	/* Controller Initialized Successfully. */
	return	1;
}
