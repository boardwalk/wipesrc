/* All routines which configure the input devices. */
//#include	"c:\wc10\h\dos.h"
#include	"wipesrc/win95/markk/input.h"
#include <stdio.h>

void	SetKey(uint32_t button, uint16_t code);

void	ConfigureKeyboard(void)
{
	uint32_t	i;

	/* Initialize KeyboardMap(s). */
	for(i=0; i<_MAX_KEYS; i++)
	{
		etc_KeyboardMap[i] = 0;
		//etc_KeyboardMap[i+_MAX_KEYS] = 0xFFFFFFFF;
		etc_ExtendedKeyboardMap[i] = 0;
		//etc_ExtendedKeyboardMap[i+_MAX_KEYS] = 0xFFFFFFFF;
	}

	/* Assign Default Key Set. */
	for(i=0; i<_MAX_BUTTONS; i++)
		SetKey(i, etc_DefaultKeyboardSet[i]);

}

void	ConfigureMouse1(void)
{
	etc_Mouse1Config.MouseUp = etc_DefaultMouseSet[0];
	etc_Mouse1Config.MouseDown = etc_DefaultMouseSet[1];
	etc_Mouse1Config.MouseLeft = etc_DefaultMouseSet[2];
	etc_Mouse1Config.MouseRight = etc_DefaultMouseSet[3];
	etc_Mouse1Config.MouseLeftButton = etc_DefaultMouseSet[4];
	etc_Mouse1Config.MouseMiddleButton = etc_DefaultMouseSet[5];
	etc_Mouse1Config.MouseRightButton = etc_DefaultMouseSet[6];
	etc_Mouse1Config.MouseDummy = etc_DefaultMouseSet[7];
}

void	ConfigureMouse2(void)
{
	etc_Mouse2Config.MouseUp = etc_DefaultMouseSet[0];
	etc_Mouse2Config.MouseDown = etc_DefaultMouseSet[1];
	etc_Mouse2Config.MouseLeft = etc_DefaultMouseSet[2];
	etc_Mouse2Config.MouseRight = etc_DefaultMouseSet[3];
	etc_Mouse2Config.MouseLeftButton = etc_DefaultMouseSet[4];
	etc_Mouse2Config.MouseMiddleButton = etc_DefaultMouseSet[5];
	etc_Mouse2Config.MouseRightButton = etc_DefaultMouseSet[6];
	etc_Mouse2Config.MouseDummy = etc_DefaultMouseSet[7];
}

void	ConfigureJoystick1(void)
{
	etc_Joy1Config.JoyUp = etc_DefaultJoystickSet[0];
	etc_Joy1Config.JoyDown = etc_DefaultJoystickSet[1];
	etc_Joy1Config.JoyLeft = etc_DefaultJoystickSet[2];
	etc_Joy1Config.JoyRight = etc_DefaultJoystickSet[3];
	etc_Joy1Config.JoyButton1 = etc_DefaultJoystickSet[4];
	etc_Joy1Config.JoyButton2 = etc_DefaultJoystickSet[5];
	etc_Joy1Config.JoyButton3 = etc_DefaultJoystickSet[6];
	etc_Joy1Config.JoyButton4 = etc_DefaultJoystickSet[7];
}

void	ConfigureJoystick2(void)
{
	etc_Joy2Config.JoyUp = etc_DefaultJoystickSet[0];
	etc_Joy2Config.JoyDown = etc_DefaultJoystickSet[1];
	etc_Joy2Config.JoyLeft = etc_DefaultJoystickSet[2];
	etc_Joy2Config.JoyRight = etc_DefaultJoystickSet[3];
	etc_Joy2Config.JoyButton1 = etc_DefaultJoystickSet[4];
	etc_Joy2Config.JoyButton2 = etc_DefaultJoystickSet[5];
	etc_Joy2Config.JoyButton3 = etc_DefaultJoystickSet[6];
	etc_Joy2Config.JoyButton4 = etc_DefaultJoystickSet[7];
}

void	AssignKey(uint32_t button)
{
	/* Ask user for joypad key config. */
	printf("\nPress %s Key:\n", buttonname[button]);
	etc_ExtKey = GetKey();
	if((etc_ExtKey & 0xFF00) == 0xFF00)
	{
		/* Keypress must have been an extended scan code. */
		etc_KeyboardConfig[button] = etc_ExtKey & 0x00FF;
		printf("Extended %d\n", etc_KeyboardConfig[button]);
		etc_ExtendedKeyboardMap[etc_KeyboardConfig[button]] = 1 << button;
		//etc_ExtendedKeyboardMap[etc_KeyboardConfig[button] + 0x80] = ~(1 << button);
	}
	else
	{
		/* Keypress was a standard scan code. */
		etc_KeyboardConfig[button] = etc_ExtKey & 0x00FF;
		printf("Standard %d\n", etc_KeyboardConfig[button]);
		etc_KeyboardMap[etc_KeyboardConfig[button]] = 1 << button;
		//etc_KeyboardMap[etc_KeyboardConfig[button] + 0x80] = ~(1 << button);
	}
}



void	SetKey(uint32_t button, uint16_t code)
{
	if((code & 0xFF00) == 0xFF00)
	{
		/* It must be an extended scan code. */
		code &= 0x00FF;
		etc_KeyboardConfig[button] = code;
		etc_ExtendedKeyboardMap[code] |= 1 << button;
		etc_ExtendedKeyboardMap[code + 0x80] &= ~(1 << button);
	}
	else
	{
		/* It must be a standard scan code. */
		code &= 0x00FF;
		etc_KeyboardConfig[button] = code;
		etc_KeyboardMap[code] |= 1 << button;
		//etc_KeyboardMap[code + 0x80] &= ~(1 << button);
	}
}



uint16_t	GetKey(void)
{
	int32_t i;
	etc_ExtendedFlag = 0;
	etc_KeyPressed = 0;
	for(i=0; etc_KeyPressed == 0; i++);
	return (etc_Key + (etc_ExtendedFlag << 8));
}



uint16_t	GetKeyNoBlock(void)
{
	uint16_t	result;


	etc_KeyPressed = 0;
	result = etc_Key + (etc_ExtendedFlag << 8);
	etc_ExtendedFlag = 0;

	return (result);
}
