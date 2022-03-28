#define			_SCREEN_WIDTH		320
#define			_SCREEN_HEIGHT		200
#define			_FONT_CELLS			256
#define			_ROW_PER_CELL		8
#define			_CELL_WIDTH			8
#define			_CELL_HEIGHT		8

#define			_COLOUR_PIXEL_BYTE	15
#define			_ZERO_PIXEL_BYTE	2

typedef	struct {
	char		RowByte[_ROW_PER_CELL];
} CELL;

extern CELL		DebugFontTable[_FONT_CELLS];

extern void		WriteVGAString(char x, char y, char *Buffer);
extern void		WriteVGACell(char x, char y, char c);

extern void		HeadTest(void);
extern void 	MAKdelay(void);

extern void		OutputCarDataFile(void);
extern void		OpenCarDataFile(void);
extern void		CloseCarDataFile(void);
extern void		OpenBinaryCarFile(void);
extern void		CloseBinaryCarFile(void);
extern void		OutputBinaryCarData(void);
