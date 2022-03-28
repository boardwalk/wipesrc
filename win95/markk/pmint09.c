/* Source File: pmkey.c */
/* This is the INT09 Protected-Mode Keyboard Handler (LIBETC). */
#include	<dos.h>
#include	"libetc.h"

#pragma off (check_stack)
void __interrupt __far	pmint09()
{
	/* Disable Interrupts. */
	_disable();

	/* Request next keyboard code from 8042 Keyboard Controller. */
	while((etc_Status = inp(_STATUS_PORT)) & 0x02 == 0);
	etc_ScanCode = inp(_KEYBOARD_PORT);

	/* Enable Interrupts. */
	_enable();

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
			default		:	ManageScanCode();
								break;
		}
	}

	/* Transmit End-Of-Interrupt to 8259A Interrupt Controller. */
	outp(_INTERRUPT1_PORT, 0x20);
}
#pragma on (check_stack)


#pragma off (check_stack)
void	ManageScanCode(void)
{
	switch (etc_ExtendedMode)
	{	
		case	_XT	:		/* Assume ScanCode is normal make/break. */
		{				
			if(etc_ScanCode < 0x80)
				etc_KeyboardRegister |= etc_KeyboardMap[etc_ScanCode];
			else
			{
				etc_KeyboardRegister &= etc_KeyboardMap[etc_ScanCode];
			
				/* Signal that a key has been depressed. */
				etc_KeyPressed = -1;
				etc_Key = etc_ScanCode - 0x80;
			}
			etc_ExtendedMode = _XT;
		}
		break;


		case	_AT	:		/* Assume ScanCode is AT break code. */
		{
			etc_KeyboardRegister &= etc_KeyboardMap[etc_ScanCode+_MAX_KEYS];

			/* Signal that a key has been depressed. */
			etc_KeyPressed = -1;
			etc_Key = etc_ScanCode;
			etc_ExtendedMode = _XT;
		}
		break;


		case	_MF_II:		/* Assume ScanCode is MF-II extended key code. */
		{
			if(etc_ScanCode < 0x80)
				etc_KeyboardRegister |= etc_ExtendedKeyboardMap[etc_ScanCode];
			else
			{
				etc_KeyboardRegister &= etc_ExtendedKeyboardMap[etc_ScanCode];
		
				/* Signal that a key has been depressed. */
				etc_KeyPressed = -1;
				etc_Key = etc_ScanCode - 0x80;
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
		etc_TransferBuffer1[2] = (char) (etc_KeyboardRegister >> 8);
		etc_TransferBuffer1[3] = (char) (etc_KeyboardRegister & 0x00FF);
	}
}
#pragma on (check_stack)
