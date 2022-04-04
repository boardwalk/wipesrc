/* This is the INT09 Protected-Mode Keyboard Handler (LIBETC). */
#include <stdint.h>

#include	"input.h"

void pmkey(uint8_t ScanCode, uint8_t KeyDown)
{

	etc_ScanCode = ScanCode;

	/* Perform Keyboard Code management. */
	if(etc_KeyboardSwitch == _DEVICE_ON)
	{
		/* Watch out for multi-byte codes. */
		switch (etc_ScanCode)
		{
			case	0xE0	:	etc_ExtendedMode = _MF_II;
								break;
			case	0xE1	:	etc_ExtendedMode = _PAUSE_ID;
								break;
			case	0xF0	:	etc_ExtendedMode = _AT;
								break;
			default			:	ManageScanCode(KeyDown);
								break;
		}
	}
}

void	ManageScanCode(uint8_t KeyDown)
{
	switch (etc_ExtendedMode)
	{
		case	_XT	:		/* Assume ScanCode is normal make/break. */
		{
			if(KeyDown)
				//key down
				etc_KeyboardRegister |= etc_KeyboardMap[etc_ScanCode];
			else
			{
				//key up
				etc_KeyboardRegister &= ~etc_KeyboardMap[etc_ScanCode];

				/* Signal that a key has been depressed. */
				etc_KeyPressed = -1;
//				etc_KeyboardCounter++;
				etc_Key = etc_ScanCode;
			}
			etc_ExtendedMode = _XT;
		}
		break;


		case	_AT	:		/* Assume ScanCode is AT break code. */
		{
			etc_KeyboardRegister &= etc_KeyboardMap[etc_ScanCode+_MAX_KEYS];

			/* Signal that a key has been depressed. */
			etc_KeyPressed = -1;
//			etc_KeyboardCounter++;
			etc_Key = etc_ScanCode;
			etc_ExtendedMode = _XT;
		}
		break;


		case	_MF_II:		/* Assume ScanCode is MF-II extended key code. */
		{
			if(!KeyDown)
				etc_KeyboardRegister |= etc_ExtendedKeyboardMap[etc_ScanCode];
			else
			{
				etc_KeyboardRegister &= etc_ExtendedKeyboardMap[etc_ScanCode];

				/* Signal that a key has been depressed. */
				etc_KeyPressed = -1;
//				etc_KeyboardCounter++;
				etc_Key = etc_ScanCode;
				etc_ExtendedFlag = 0xFF;
			}
			etc_ExtendedMode = _XT;
		}
		break;


		case	_PAUSE_ID:	/* Assume this scancode is PAUSE ID. */
		{
			etc_ExtendedMode = _MF_II;
		}
		break;


		default		:		break;
	}

	/* Send Data out to InitPAD's bufferA, when available. */
	if(etc_ExtendedMode == _XT)
	{
		etc_TransferBuffer1[0] = OK;
		etc_TransferBuffer1[1] = SONY_PAD;
		etc_TransferBuffer1[2] = (uint8_t) ~(etc_KeyboardRegister >> 8);
		etc_TransferBuffer1[3] = (uint8_t) ~(etc_KeyboardRegister & 0x00FF);
	}
}
