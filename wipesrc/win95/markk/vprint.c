/* direct print to video ram. */
#include	"c:\watcom\h\string.h"
#include	"c:\watcom\h\stdlib.h"
#include	"..\markk\comio.h"

void	swrite(char x, char y, char *buf);
void	nwrite(char x, char y, int val);
void	debugging(void);

#define	SCWID 	80
#define	CSHGT	30

void	swrite(char x, char y, char *buf)
{
	int	i;

	char	*vram = (char *) 0xB8000;
	
	vram += y * SCWID * 2;
	vram += x * 2;
	
	for(i=0; i<strlen(buf); i++) *(vram + i * 2) = *(buf + i);
}

void	nwrite(char x, char y, int val)
{
	char	nbuf[40];

	ultoa(val, nbuf, 16);
	swrite(x, y, nbuf);
}	

void	iwrite(char x, char y, int val)
{
	int 	mask = 0xF0000000;
	int		i;
	char	*nbuf = "00000000h\0";
	char	*hextable = "0123456789ABCDEF";
	
	for(i=0; i<7; i++) {
		*(nbuf+i) = *(hextable+((val & mask) >> ((7-i)*4)));
		mask = mask >> 4;
	}
	swrite(x,y,nbuf);
}


void	debugging(void)
{
		#if	0
		swrite(0,_LOOPLN,"Receive Buffer Head Ptr: ");
		swrite(0,_LOOPLN+1,"Receive Buffer Tail Ptr: ");
		iwrite(26,_LOOPLN, (int) com_ReceiveHead);
		iwrite(26,_LOOPLN+1, (int) com_ReceiveTail);
		
		swrite(0,_LOOPLN+3,"Transmit Buffer Head Ptr: ");
		swrite(0,_LOOPLN+4,"Transmit Buffer Tail Ptr: ");
		iwrite(26,_LOOPLN+3, (int) com_TransmitHead);
		iwrite(26,_LOOPLN+4, (int) com_TransmitTail);
		
//		swrite(0,_LOOPLN+6,"Total Interrupts: ");
//		swrite(0,_LOOPLN+7,"Transmit Interrupts: ");
//		swrite(0,_LOOPLN+8,"Receive Interrupts: ");
//		iwrite(26,_LOOPLN+6, (int) com_IntCount);
//		iwrite(26,_LOOPLN+7, (int) com_SendCount);
//		iwrite(26,_LOOPLN+8, (int) com_ReceiveCount);
		#endif
}