/* This function stops controller status reading and removes the
	appropriate handler routine. */
#include	"..\markk\libapi.h"

void	StopPAD(void)
{
	/* Switch Off Handlers if not already done. */
	switch (etc_InputDeviceSelect)
	{
	 	case	_KEYBOARD	:	
		{
			etc_KeyboardSwitch = _DEVICE_OFF;
			RestoreProtectedModeKeyboardHandler();
		}
		break;

		case	_MOUSE1		:
		{
			etc_Mouse1Switch = _DEVICE_OFF;
			RestoreProtectedModeMouseHandler();
		}
		break;

		case	_MOUSE2		:
		{
			etc_Mouse2Switch = _DEVICE_OFF;
			RestoreProtectedModeMouseHandler();
		}
		break;

		case	_JOYSTICK1	:
		{
			etc_Joystick1Switch = _DEVICE_OFF;
			RestoreProtectedModeJoystickHandler();
		}
		break;

		case	_JOYSTICK2	:
		{
			etc_Joystick2Switch = _DEVICE_OFF;
			RestoreProtectedModeJoystickHandler();
//			restore_timer();
		}
		break;

		default				:
		{
			/* unknown controller. */
		}
		break;
	}

	/* Flush the Controller Status Buffers. */
	etc_TransferBuffer1[0] = NG;
	etc_TransferBuffer2[0] = NG;
}
