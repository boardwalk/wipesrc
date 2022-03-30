#define	FADE_LEVEL	32
#define	FFADE_LEVEL	32.0

typedef struct colours_
{
	char	blue;
	char	green;
	char	red;
	char	pad;
} COLOURS;

extern void MakePal332(void);
extern void MakePal2222( void );
extern void MakeFade(char red, char green, char blue);
extern char find_nearest(COLOURS *pal, char r, char g, char b);


extern char fade_buf[256][FADE_LEVEL];

extern char max_pal;


extern int32_t GetPal(char);
extern void SetPal(char,char,char,char);
