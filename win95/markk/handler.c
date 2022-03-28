/* General Library of LIBETC/API controller routines. */
#include	"c:\wc10\h\stdio.h"
#include	"c:\wc10\h\dos.h"
#include	"c:\wc10\h\process.h"
#include	"..\markk\input.h"

void	InstallProtectedModeKeyboardHandler(void)
{
#ifdef WINPORT_KEYS
	SaveProtectedModeKeyboardHandler();

	etc_regs.x.eax = 0x2509;
	etc_regs.x.edx = FP_OFF((void far *) pmkey);
	etc_sregs.ds = FP_SEG((void far *) pmkey);
	etc_sregs.es = 0;
	int386x(_DOS_INT, &etc_regs, &etc_regs, &etc_sregs);
	etc_KeyboardHandler = -1;
#endif
}

void	SaveProtectedModeKeyboardHandler(void)
{
#ifdef WINPORT_KEYS
	etc_regs.x.eax = 0x3509;
	etc_sregs.ds = 0;
	etc_sregs.es = 0;
	int386x(_DOS_INT, &etc_regs, &etc_regs, &etc_sregs);
	etc_OriginalInt09Selector = (unsigned short int) etc_sregs.es;
	etc_OriginalInt09Offset = etc_regs.x.ebx;
#endif

}

void	RestoreProtectedModeKeyboardHandler(void)
{
#ifdef WINPORT_KEYS
	etc_regs.x.eax = 0x2509;
	etc_sregs.ds = etc_OriginalInt09Selector;
	etc_sregs.es = 0;
	etc_regs.x.edx = etc_OriginalInt09Offset;
	int386x(_DOS_INT, &etc_regs, &etc_regs, &etc_sregs);
	etc_KeyboardHandler = 0;
#endif

}

void	RestoreProtectedModeMouseHandler(void)
{
#ifdef WINPORT_MOUSE
	/* Restore video mode. */
//	_setvideomode(_DEFAULTMODE);

	/* Reset Mouse Driver. */
	etc_regs.w.ax = 0;
	int386(_MOUSE_INT, &etc_regs, &etc_regs);
	etc_MouseHandler = 0;
#endif

}

void	InstallProtectedModeJoystickHandler(void)
{
#ifdef WINPORT_JOY
	/* Check game port exists. */
	etc_regs.h.ah = 0x84;
	etc_regs.w.dx = 1;
	int386(_JOYSTICK_INT, &etc_regs, &etc_regs);
	if(etc_regs.w.cflag == 0)
	{
		etc_JoystickService = pmjoy;
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
#endif
	
}

void	RestoreProtectedModeJoystickHandler(void)
{
#ifdef WINPORT_JOY
	/* Remove service routine. */
	etc_JoystickService = NULL;
	etc_JoystickHandler = 0;
#endif
}


