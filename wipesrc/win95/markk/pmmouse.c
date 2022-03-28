/* This is the protected mode mouse handler routine. */
#include	"c:\wc10\h\dos.h"
#include	"c:\wc10\h\string.h"
#include	"c:\wc10\h\graph.h"
#include	"..\markk\input.h"

#pragma off (check_stack)
static void __loadds __far	pmmouse(short mask, short state, short col, short row,
							signed short mickey_h, signed short mickey_v)
{
//	static char			newstack[_MOUSE_STACK];
//	static void __far	*oldstack;

	short					temp_mask;
	short					temp_state;
	short					temp_col;
	short					temp_row;
	signed short			temp_mickey_h;
	signed short			temp_mickey_v;
	char					temp;


	/* Setup Stack. */
//	oldstack = GetStack();
//	SetStack(newstack + _MOUSE_STACK);

	/* Copy Imported parameters to safe space. */
	temp_mask = mask;
	temp_state = state;
	temp_col = col;
	temp_row = row;
	temp_mickey_h = mickey_h;
	temp_mickey_v = mickey_v;

	/* Clear Mouse space. */
	memset(&etc_Mouse1, 0, sizeof(MOUSE));
	memset(&etc_Mouse2, 0, sizeof(MOUSE));
	etc_Mouse1Register = 0;
	etc_Mouse2Register = 0;

	/* Transform mouse information into an incoming data buffer. */
	if(etc_Mouse1Switch == _DEVICE_ON)
	{
		etc_Mouse1.mX = temp_col;
		etc_Mouse1.mY = temp_row;
//		if(temp_mickey_h < 0) etc_Mouse1.mDir |= 1 << etc_Mouse1Config.MouseLeft;
//		if(temp_mickey_h > 0) etc_Mouse1.mDir |= 1 << etc_Mouse1Config.MouseRight;
//		if(temp_mickey_v < 0) etc_Mouse1.mDir |= 1 << etc_Mouse1Config.MouseDown;
//		if(temp_mickey_v > 0) etc_Mouse1.mDir |= 1 << etc_Mouse1Config.MouseUp;
		etc_Mouse1.mDir = 0;
		etc_Mouse1.mButton1 = (temp_state & 0x01);
		etc_Mouse1.mButton2 = (temp_state & 0x04) >> 2;
		etc_Mouse1.mButton3 = (temp_state & 0x02) >> 1;

		etc_Mouse1Register |= etc_Mouse1.mDir;
		etc_Mouse1Register |= etc_Mouse1.mButton1 << etc_Mouse1Config.MouseLeftButton;
		etc_Mouse1Register |= etc_Mouse1.mButton2 << etc_Mouse1Config.MouseMiddleButton;
		etc_Mouse1Register |= etc_Mouse1.mButton3 << etc_Mouse1Config.MouseRightButton;

		/* Transmit Data to data buffer. */
		etc_TransferBuffer1[0] = OK;
		etc_TransferBuffer1[1] = SONY_PAD;
//		etc_TransferBuffer1[2] = (char) ~(etc_Mouse1Register >> 8);

		temp = ~etc_TransferBuffer1[3];					// This lot preserves the
		temp &= 0x9F;										// brake bits, which get
		temp |= (char) (etc_Mouse1Register & 0xFF);	// lost somehow otherwise
		etc_TransferBuffer1[3] = ~temp;
	}
	if(etc_Mouse2Switch == _DEVICE_ON)
	{
		etc_Mouse2.mX = temp_col;
		etc_Mouse2.mY = temp_row;
		if(temp_mickey_h < 0) etc_Mouse2.mDir |= 1 << etc_Mouse2Config.MouseLeft;
		if(temp_mickey_h > 0) etc_Mouse2.mDir |= 1 << etc_Mouse2Config.MouseRight;
		if(temp_mickey_v < 0) etc_Mouse2.mDir |= 1 << etc_Mouse2Config.MouseDown;
		if(temp_mickey_v > 0) etc_Mouse2.mDir |= 1 << etc_Mouse2Config.MouseUp;
		etc_Mouse2.mButton1 = (temp_state & 0x01);
		etc_Mouse2.mButton2 = (temp_state & 0x04) >> 2;
		etc_Mouse2.mButton3 = (temp_state & 0x02) >> 1;

		etc_Mouse2Register |= etc_Mouse2.mDir;
		etc_Mouse2Register |= etc_Mouse2.mButton1 << etc_Mouse2Config.MouseLeftButton;
		etc_Mouse2Register |= etc_Mouse2.mButton2 << etc_Mouse2Config.MouseMiddleButton;
		etc_Mouse2Register |= etc_Mouse2.mButton3 << etc_Mouse2Config.MouseRightButton;

		/* Transmit Data to data buffer. */
		etc_TransferBuffer1[0] = OK;
		etc_TransferBuffer1[1] = SONY_PAD;
		etc_TransferBuffer1[2] = (char) ~(etc_Mouse1Register >> 8);
		etc_TransferBuffer1[3] = (char) ~(etc_Mouse1Register & 0x00FF);
	}

	/* Restore Stack. */
//	SetStack(oldstack);
}
#pragma on (check_stack)

void	InstallProtectedModeMouseHandler(void)
{
	struct videoconfig	vc;

	/* Set video mode. */
//	_setvideomode(_MAXRESMODE);

	/* Reset Mouse Driver. */
	etc_regs.w.ax = 0;
	int386(_MOUSE_INT, &etc_regs, &etc_regs);
	if(etc_regs.w.ax == 0xFFFF)
	{
		_getvideoconfig(&vc);

	 	etc_regs.w.ax = 0x000C;
		etc_regs.w.cx = 0x007F; /* Call when anything happens. */
		etc_regs.x.edx = FP_OFF(&pmmouse);
		etc_sregs.es = FP_SEG(&pmmouse);
		int386x(_MOUSE_INT, &etc_regs, &etc_regs, &etc_sregs);
		etc_MouseHandler = -1;

		etc_regs.w.ax = 0x0007;			// X range
		etc_regs.w.cx = 0;
		etc_regs.w.dx = 319;
		int386(_MOUSE_INT, &etc_regs, &etc_regs);

		etc_regs.w.ax = 0x0008;			// Y range
		etc_regs.w.cx = 0;
		etc_regs.w.dx = 199;
		int386(_MOUSE_INT, &etc_regs, &etc_regs);

		etc_regs.w.ax = 0x000F;			// Sensitivity
		etc_regs.w.cx = 32;				// Default 8
		etc_regs.w.dx = 16;				// Default 16
		int386(_MOUSE_INT, &etc_regs, &etc_regs);
	}
	else
		etc_MouseHandler = 0;
}
