#ifndef PALLETTE_H
#define PALLETTE_H

#define	FADE_LEVEL	32
#define	FFADE_LEVEL	32.0

typedef struct colours_
{
	char	blue;
	char	green;
	char	red;
	char	pad;
} COLOURS;

void MakePal332();
void MakePal2222();
void MakeFade(char red, char green, char blue);
char find_nearest(COLOURS *pal, char r, char g, char b);


extern char fade_buf[256][FADE_LEVEL];

extern char max_pal;


int32_t GetPal(char);
void SetPal(char,char,char,char);

char FindNearestColourIndex(CVECTOR *TestCol);

#endif
