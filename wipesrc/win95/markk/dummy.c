/* Test dummy terminal, for error frequency. */
#include	<stdio.h>
#include	<conio.h>
#include	<graph.h>
#include	"comio.h"

#define		N		32

long		comerr;
int			key;
char		buffer[256];
int			i = 0;

void	main(void)
{
	_setvideomoderows(_TEXTC80, _MAXTEXTROWS);

	comerr = copen(_COM2, _9600_BAUD, _8BIT_DATA, _NO_PARITY, _1BIT_STOP);
	if(comerr == _COM_CONFLICT_ERROR)  {
		printf("\nError opening com device.\n");
		exit(0);
	}
	
	cread(_COM2, buffer, N);
	debugging();
	printf("\n%s\n", buffer);
	
	cclose(_COM2);
}

