//-------------------------------------------------------------------
// Header file to store line number info for each debugging var used
// (17/08/95)
//-------------------------------------------------------------------

//#define					_GR_RECEIVE_PTRS_ON
//#define					FN_DEBUG
//#define					_GR_READ_LOCK_ON
//#define					FRAME_CHECKING

//-------------------------------------------------------------------

#define					_RECEIVE_HEAD_PTR	1*8
#define					_RECEIVE_TAIL_PTR	2*8

//-------------------------------------------------------------------

#define					_SCREEN_WIDTH		320
#define					_SCREEN_HEIGHT		200
#define					_FONT_CELLS			256
#define					_ROW_PER_CELL		8
#define					_CELL_WIDTH			8
#define					_CELL_HEIGHT		8

#define					_COLOUR_PIXEL_BYTE	25
#define					_ZERO_PIXEL_BYTE	2

//-------------------------------------------------------------------

#define					dbg_point			WriteVGAString(0, 0, "DEBUG POINT");

#ifdef					FN_DEBUG
#define					fn_track(x)			fn_track1(x)
#define					fn_create			fn_file = fopen("FNTRACK", "wt");
#else
#define					fn_track(x)			;
#define					fn_create			;
#endif
#define					fn_track1(x)		WriteVGAString(0, 20*8, x); \
											fn_file = fopen("FNTRACK", "at"); \
											fprintf(fn_file, x); \
											fclose(fn_file);
#define					fn_var(x)			ultoa(x, DebugString, 16); \
											strcat(DebugString, "\n"); \
											fn_track(DebugString)
#define					fn_var2(x)			ultoa(x, DebugString, 16); \
											strcat(DebugString, " "); \
											fn_track(DebugString)


#define					delta_check(x, v)  	if(((x > 0) && (x > 128)) || ((x < 0) && (x < -128))) { \
												fn_track("\ndelta overflow error: ") \
												fn_track(v) \
												fn_var(x) \
												while(1){}; \
											}
											
//-------------------------------------------------------------------

typedef	struct _cell {
	char				RowByte[_ROW_PER_CELL];
} FONT;

//-------------------------------------------------------------------

extern FONT				DebugFontTable[_FONT_CELLS];
extern char				DebugString[];
extern int				fn_count;
extern FILE				*fn_file;

//-------------------------------------------------------------------

extern void				WriteVGAString(char x, char y, char *Buffer);
extern void				WriteVGACell(char x, char y, char c);

//-------------------------------------------------------------------
