/* Source File: test3.c */
#include	<stdio.h>
#include	<conio.h>
#include	<dos.h>
#include	"libetc.h"

typedef unsigned char	u_char;
typedef unsigned short	u_short;
typedef unsigned long	u_long;

typedef struct {
	u_char	status;
	u_char	kind;
	u_char	data[2];
	u_char	ch[4];
} RAW_PAD;

typedef struct {
	u_char	status;
	u_char	kind;
	
	u_short	data;
	u_short	old_data;
	u_short	trig;
	
	short	h;
	short	gas;
	short	brake;
	short	dummy;
	
	u_short	s_down,s_up;
	short	handle;
} PAD_INFO;

RAW_PAD		pad_buf[2];
PAD_INFO		pad_info;

RAW_PAD		*p = &pad_buf[0];
PAD_INFO		*f = &pad_info;

void	main(void)
{
	/* Select Input Device. */
	etc_InputDeviceSelect = _KEYBOARD;

	/* Initialize JoyPad Interface. */
	InitPAD(&pad_buf[0], 16, &pad_buf[1], 16);
	StartPAD();

	/* Test Loop */
	while(!(f->data & (1<<Start)))
	{
		f->data = ((p->data[0] << 8) | (p->data[1]));

		SetCursorPosition(20,1);
		printf("\n Status: \t%4xH\n", p->status);
		printf("\n Kind: \t\t%4xH\n", p->kind);
		printf("\n Data: \t\t%4lxH\n", f->data);
	}

	StopPAD();
}
