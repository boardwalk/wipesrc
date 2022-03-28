#include		"c:\wc10\h\stdio.h"
#include		"..\markk\debug.h"

char			DebugString[256];
int				fn_count = 0;
FILE			*fn_file;

/* Draw a null-terminated string using my VGA bitmap font. */
void			WriteVGAString(char x, char y, char *Buffer)
{
	char	i = 0;
	while (Buffer[i] != '\0') {
		WriteVGACell(i + x, y, Buffer[i]);
		i++;
	}
}

/* Draw character cell. */
void			WriteVGACell(char x, char y, char c)
{
	int		i,j;
	char	*VGACell = (char *) 0x000A0000;
	char	mask;
	FONT	*rcell;
	
	rcell = &DebugFontTable[c];
	VGACell += (y * _SCREEN_WIDTH) + (x * _CELL_WIDTH);

	for (i=0; i<_CELL_HEIGHT; i++) {
		mask = 0x80;
 		for (j=0; j<_CELL_WIDTH; j++) {
			if((rcell->RowByte[i] & mask) != 0)	*VGACell = _COLOUR_PIXEL_BYTE;
			else *VGACell = _ZERO_PIXEL_BYTE;
			mask >>= 1;
			VGACell++;
		}
		VGACell += _SCREEN_WIDTH - _CELL_WIDTH;
	}	
}
