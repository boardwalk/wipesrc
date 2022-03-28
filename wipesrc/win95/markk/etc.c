/* General Library of LIBETC/API controller routines. */
#include	<stdio.h>
#include	<dos.h>
#include	<process.h>
#include	<mem.h>
#include	"libetc.h"

void	InstallProtectedModeKeyboardHandler(void)
{
	SaveProtectedModeKeyboardHandler();

	etc_regs.x.eax = 0x2509;
	etc_regs.x.edx = FP_OFF((void far *) pmkey);
	etc_sregs.ds = FP_SEG((void far *) pmkey);
	etc_sregs.es = 0;
	int386x(_DOS_INT, &etc_regs, &etc_regs, &etc_sregs);
	etc_KeyboardHandler = -1;
}

void	SaveProtectedModeKeyboardHandler(void)
{
	etc_regs.x.eax = 0x3509;
	etc_sregs.ds = 0;
	etc_sregs.es = 0;
	int386x(_DOS_INT, &etc_regs, &etc_regs, &etc_sregs);
	etc_OriginalInt09Selector = (unsigned short int) etc_sregs.es;
	etc_OriginalInt09Offset = etc_regs.x.ebx;
}

void	RestoreProtectedModeKeyboardHandler(void)
{
	etc_regs.x.eax = 0x2509;
	etc_sregs.ds = etc_OriginalInt09Selector;
	etc_sregs.es = 0;
	etc_regs.x.edx = etc_OriginalInt09Offset;
	int386x(_DOS_INT, &etc_regs, &etc_regs, &etc_sregs);
	etc_KeyboardHandler = 0;
}

void	InstallProtectedModeMouseHandler(void)
{
	/* Reset Mouse Driver. */
	etc_regs.w.ax = 0;
	int386(_MOUSE_INT, &etc_regs, &etc_regs);
	if(etc_regs.w.ax == 0xFFFF)
	{
	 	etc_regs.w.ax = 0x000C;
		etc_regs.w.cx = 0; /* Call when mouse moves. */
		etc_regs.x.edx = FP_OFF(&pmmouse);
		etc_sregs.es = FP_SEG(&pmmouse);
		int386x(_MOUSE_INT, &etc_regs, &etc_regs, &etc_sregs);
		etc_MouseHandler = -1;
	}
	else
		etc_MouseHandler = 0;
}

void	RestoreProtectedModeMouseHandler(void)
{
	/* Reset Mouse Driver. */
	etc_regs.w.ax = 0;
	int386(_MOUSE_INT, &etc_regs, &etc_regs);
	etc_MouseHandler = 0;
}

void	InstallProtectedModeJoystickHandler(void)
{
	/* Check game port exists. */
	etc_regs.h.ah = 0x84;
	etc_regs.w.dx = 1;
	int386(_JOYSTICK_INT, &etc_regs, &etc_regs);
	if(etc_regs.w.cflag == 0)
	{
		etc_JoystickService = ;
		etc_JoystickHandler = -1;

		/* Store Centered Coordinates. */
		etc_Joystick1CentreX = etc_regs.w.ax;
		etc_Joystick1CentreY = etc_regs.w.bx;
		etc_Joystick2CentreX = etc_regs.w.cx;
		etc_Joystick2CentreY = etc_regs.w.dx;
	}
	else
	{
		etc_JoystickService = NULL;
		etc_JoystickHandler = 0;
	}
	
}

void	RestoreProtectedModeJoystickHandler(void)
{
	/* Remove service routine. */
	etc_JoystickService = NULL;
	etc_JoystickHandler = 0;
}

void	ConfigureKeyboard(void)
{
	int	i;

	/* Initialize KeyboardMap(s). */
	for(i=0; i<_MAX_KEYS; i++)
	{
		etc_KeyboardMap[i] = 0;
		//etc_KeyboardMap[i+_MAX_KEYS] = 0xFFFFFFFF;
		etc_ExtendedKeyboardMap[i] = 0;
		//etc_ExtendedKeyboardMap[i+_MAX_KEYS] = 0xFFFFFFFF;
	}

	/* Clear Joypad Configuration Table. */
	for(i=0; i<_MAX_BUTTONS; i++) etc_JoypadConfig[i] = 0;

	AssignKey(Start);

	AssignKey(Left);
	AssignKey(Right);
	AssignKey(Up);
	AssignKey(Down);
}

void	ConfigureMouse1(void)
{
}

void	ConfigureMouse2(void)
{
}

void	ConfigureJoystick1(void)
{
}

void	ConfigureJoystick2(void)
{
}

void	AssignKey(unsigned long int button)
{
	/* Ask user for joypad key config. */
	printf("\nPress %s Key:\n", buttonname[button]);
	etc_ExtKey = GetKey();
	if((etc_ExtKey & 0xFF00) == 0xFF00)
	{
		/* Keypress must have been an extended scan code. */
		etc_JoypadConfig[button] = etc_ExtKey & 0x00FF;
		printf("%d\n", etc_JoypadConfig[button]);
		etc_ExtendedKeyboardMap[etc_JoypadConfig[button]] = 1 << button;
		etc_ExtendedKeyboardMap[etc_JoypadConfig[button] + 0x80] = ~(1 << button);
	}
	else
	{
		/* Keypress was a standard scan code. */
		etc_JoypadConfig[button] = etc_ExtKey & 0x00FF;
		printf("%d\n", etc_JoypadConfig[button]);
		etc_KeyboardMap[etc_JoypadConfig[button]] = 1 << button;
		//etc_KeyboardMap[etc_JoypadConfig[button] + 0x80] = ~(1 << button);
	}
}

unsigned short int	GetKey(void)
{
	long i;
	etc_ExtendedFlag = 0;
	etc_KeyPressed = 0;
	for(i=0; etc_KeyPressed == 0; i++);
	return (etc_Key + (etc_ExtendedFlag << 8));
}

void	SetCursorPosition(unsigned char row, unsigned char col)
{
 	etc_regs.h.ah = 0x02;
	etc_regs.h.bh = 0x00;
	etc_regs.h.dh = row;
	etc_regs.h.dl = col;
	int386(_BIOS_INT, &etc_regs, &etc_regs);
}

void	CalibrateJoysticks(void)
{
 	/* Define X and Y Ranges and Centering. */
}
