/* This is the protected mode joystick service polling routine.
	It is called by a configurable service routine from within
	the vertical blank routine. */
#include	"c:\wc10\h\dos.h"
#include	"..\markk\input.h"

//#pragma aux ReadJoystick modify [eax ebx ecx edx esi]
//extern void ReadJoystick(void);

extern short JoyButton1, JoyButton2;

#ifndef USE_ATI

#if 0
#pragma off (check_stack)
void __far __loadds	pmjoy(void)
{
	char					temp;
	extern unsigned short	JoyXVal, JoyYVal;
	extern char				JoyButtons;


	/* Clear Joystick space. */
	memset(&etc_Joy1, 0, sizeof(JOYSTICK));
	memset(&etc_Joy2, 0, sizeof(JOYSTICK));
	etc_Joystick1Register = 0;
	etc_Joystick2Register = 0;

//	ReadJoystick();

	//JoyButtons = ~JoyButtons;		// Returned from ReadJoystick();
	//etc_Joy1.jButton1 = (JoyButtons & 0x10) >> 4;
	//etc_Joy1.jButton2 = (JoyButtons & 0x20) >> 5;

	etc_Joy1.jButton1 = JoyButton1;
	etc_Joy1.jButton2 = JoyButton2;
	
	if(etc_Joystick1Switch == _DEVICE_ON)
	{
		etc_Joystick1Register = (etc_Joy1.jButton1 << etc_Joy1Config.JoyButton1);
		etc_Joystick1Register |= (etc_Joy1.jButton2 << etc_Joy1Config.JoyButton2);
	}
	if(etc_Joystick2Switch == _DEVICE_ON)
	{
		etc_Joystick2Register = (etc_Joy2.jButton1 << etc_Joy2Config.JoyButton1);
		etc_Joystick2Register |= (etc_Joy2.jButton2 << etc_Joy2Config.JoyButton2);
	}

	etc_Joy1.jX = JoyXVal;			// Returned from ReadJoystick();
	etc_Joy1.jY = JoyYVal;

	if(etc_Joystick1Switch == _DEVICE_ON)
	{
		// Calculate Direction.
//		if((etc_Joystick1CentreX + _JOY_ERROR - etc_Joy1.jX) < 0) etc_Joy1.jDir = (1 << etc_Joy1Config.JoyRight);
//		if((etc_Joystick1CentreX - _JOY_ERROR - etc_Joy1.jX) > 0) etc_Joy1.jDir |= (1 << etc_Joy1Config.JoyLeft);
//		if((etc_Joystick1CentreY + _JOY_ERROR - etc_Joy1.jY) < 0) etc_Joy1.jDir |= (1 << etc_Joy1Config.JoyDown);
//		if((etc_Joystick1CentreY - _JOY_ERROR - etc_Joy1.jY) > 0) etc_Joy1.jDir |= (1 << etc_Joy1Config.JoyUp);
		etc_Joy1.jDir = 0;
		etc_Joystick1Register |= etc_Joy1.jDir;
	}
	if(etc_Joystick2Switch == _DEVICE_ON)
	{
		// Calculate Direction.
//		if((etc_Joystick2CentreX + _JOY_ERROR - etc_Joy1.jX) < 0) etc_Joy2.jDir = (1 << etc_Joy2Config.JoyRight);
//		if((etc_Joystick2CentreX - _JOY_ERROR - etc_Joy1.jX) > 0) etc_Joy2.jDir |= (1 << etc_Joy2Config.JoyLeft);
//		if((etc_Joystick2CentreY + _JOY_ERROR - etc_Joy1.jY) < 0) etc_Joy2.jDir |= (1 << etc_Joy2Config.JoyDown);
//		if((etc_Joystick2CentreY - _JOY_ERROR - etc_Joy1.jY) > 0) etc_Joy2.jDir |= (1 << etc_Joy2Config.JoyUp);
		etc_Joy2.jDir = 0;
		etc_Joystick2Register |= etc_Joy2.jDir;
	}

	/* Transfer the new data to data buffers. */
	if(etc_Joystick1Switch == _DEVICE_ON)
	{
		etc_TransferBuffer1[0] = OK;
		etc_TransferBuffer1[1] = SONY_PAD;
//		etc_TransferBuffer1[2] = (char) ~(etc_Joystick1Register >> 8);

		temp = ~etc_TransferBuffer1[3];					// This lot preserves the
		temp &= 0x9F;										// brake bits, which get
		temp |= (char) (etc_Joystick1Register & 0xFF);	// lost somehow otherwise
		etc_TransferBuffer1[3] = ~temp;
	}
	else if(etc_Joystick2Switch == _DEVICE_ON)
	{
		etc_TransferBuffer1[0] = OK;
		etc_TransferBuffer1[1] = SONY_PAD;
		etc_TransferBuffer1[2] = (char) ~(etc_Joystick2Register >> 8);
		etc_TransferBuffer1[3] = (char) ~(etc_Joystick2Register & 0x00FF);
	}
}
#pragma on (check_stack)


#else
void __far __loadds	pmjoy(void)
{
	char					temp;
	extern unsigned short	JoyXVal, JoyYVal;
	extern char				JoyButtons;


	/* Clear Joystick space. */
	memset(&etc_Joy1, 0, sizeof(JOYSTICK));
	memset(&etc_Joy2, 0, sizeof(JOYSTICK));
	etc_Joystick1Register = 0;
	etc_Joystick2Register = 0;

	JJSReadJoystick();

//	JoyButtons = ~JoyButtons;		// Returned from ReadJoystick();
//	etc_Joy1.jButton1 = (JoyButtons & 0x10) >> 4;
//	etc_Joy1.jButton2 = (JoyButtons & 0x20) >> 5;

	etc_Joy1.jButton1 = JoyButton1;
	etc_Joy2.jButton2 = JoyButton2;


	if(etc_Joystick1Switch == _DEVICE_ON)
	{
		etc_Joystick1Register = (etc_Joy1.jButton1 << etc_Joy1Config.JoyButton1);
		etc_Joystick1Register |= (etc_Joy1.jButton2 << etc_Joy1Config.JoyButton2);
	}
	if(etc_Joystick2Switch == _DEVICE_ON)
	{
		etc_Joystick2Register = (etc_Joy2.jButton1 << etc_Joy2Config.JoyButton1);
		etc_Joystick2Register |= (etc_Joy2.jButton2 << etc_Joy2Config.JoyButton2);
	}

	etc_Joy1.jX = JoyXVal;			// Returned from ReadJoystick();
	etc_Joy1.jY = JoyYVal;

	if(etc_Joystick1Switch == _DEVICE_ON)
	{
		// Calculate Direction.
//		if((etc_Joystick1CentreX + _JOY_ERROR - etc_Joy1.jX) < 0) etc_Joy1.jDir = (1 << etc_Joy1Config.JoyRight);
//		if((etc_Joystick1CentreX - _JOY_ERROR - etc_Joy1.jX) > 0) etc_Joy1.jDir |= (1 << etc_Joy1Config.JoyLeft);
//		if((etc_Joystick1CentreY + _JOY_ERROR - etc_Joy1.jY) < 0) etc_Joy1.jDir |= (1 << etc_Joy1Config.JoyDown);
//		if((etc_Joystick1CentreY - _JOY_ERROR - etc_Joy1.jY) > 0) etc_Joy1.jDir |= (1 << etc_Joy1Config.JoyUp);
		etc_Joy1.jDir = 0;
		etc_Joystick1Register |= etc_Joy1.jDir;
	}
	if(etc_Joystick2Switch == _DEVICE_ON)
	{
		// Calculate Direction.
//		if((etc_Joystick2CentreX + _JOY_ERROR - etc_Joy1.jX) < 0) etc_Joy2.jDir = (1 << etc_Joy2Config.JoyRight);
//		if((etc_Joystick2CentreX - _JOY_ERROR - etc_Joy1.jX) > 0) etc_Joy2.jDir |= (1 << etc_Joy2Config.JoyLeft);
//		if((etc_Joystick2CentreY + _JOY_ERROR - etc_Joy1.jY) < 0) etc_Joy2.jDir |= (1 << etc_Joy2Config.JoyDown);
//		if((etc_Joystick2CentreY - _JOY_ERROR - etc_Joy1.jY) > 0) etc_Joy2.jDir |= (1 << etc_Joy2Config.JoyUp);
		etc_Joy2.jDir = 0;
		etc_Joystick2Register |= etc_Joy2.jDir;
	}

	/* Transfer the new data to data buffers. */
	if(etc_Joystick1Switch == _DEVICE_ON)
	{
		etc_TransferBuffer1[0] = OK;
		etc_TransferBuffer1[1] = SONY_PAD;
//		etc_TransferBuffer1[2] = (char) ~(etc_Joystick1Register >> 8);

		temp = ~etc_TransferBuffer1[3];					// This lot preserves the
		temp &= 0x9F;										// brake bits, which get
		temp |= (char) (etc_Joystick1Register & 0xFF);	// lost somehow otherwise
		etc_TransferBuffer1[3] = ~temp;
	}
	else if(etc_Joystick2Switch == _DEVICE_ON)
	{
		etc_TransferBuffer1[0] = OK;
		etc_TransferBuffer1[1] = SONY_PAD;
		etc_TransferBuffer1[2] = (char) ~(etc_Joystick2Register >> 8);
		etc_TransferBuffer1[3] = (char) ~(etc_Joystick2Register & 0x00FF);
	}
}
#endif
#endif //USE_ATI
