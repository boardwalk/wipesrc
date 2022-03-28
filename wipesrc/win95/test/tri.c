#include <sys\types.h>
#include <libgte.h>
#include <libgpu.h>

#define NUM_TRI	2000
#define TRI_SIZE 10

POLY_F3	TRIS[NUM_TRI];
DVECTOR	SPEED[NUM_TRI];
DVECTOR POS[NUM_TRI];

int	inattract = 0;
int inmenu = 0;
int Reallyinmenu = 0;
short drawenv_tpage = 0;

typedef struct _clip
{
	short	tx, ty;
	short	bx, by;
	short	w, h;
} CLIP;

CLIP curclip;
CLIP curclipS;	// jed

void Init_Triangles(void)
{
	int i;

	curclip.tx = 0;
	curclip.ty = 0;
	curclip.bx = 319;
	curclip.by = 199;
	curclip.w = 320;
	curclip.h = 200;

	curclipS.tx = 0;
	curclipS.ty = 0;
	curclipS.bx = 319;
	curclipS.by = 199;
	curclipS.w = 320;
	curclipS.h = 200;

	for (i = 0; i < NUM_TRI; i++)
	{
	SPEED[i].vx = (rand() % 2) +1;
	SPEED[i].vy = (rand() % 2) +1;

	POS[i].vx = rand() % (320 - TRI_SIZE);
	POS[i].vy = rand() % (200 - TRI_SIZE);

	setPolyF3(&TRIS[i]);
	setXY3(&TRIS[i],
		POS[i].vx,POS[i].vy,
		POS[i].vx+TRI_SIZE,POS[i].vy,
		POS[i].vx,POS[i].vy+TRI_SIZE);
	setRGB0(&TRIS[i],
			rand() % 256,  0 ,0);
	setaddr(&TRIS[i],&TRIS[i+1]);
	}
	termPrim(&TRIS[i-1]);
}

void Render_Triangles(void)
{
	DrawOTag(TRIS);
}

void Move_Triangles(void)
{
	int i;
	short x,y;

	for (i = 0; i < NUM_TRI;i++ )
	{
		x = TRIS[i].x0;
		y = TRIS[i].y0;

		x += SPEED[i].vx;
		y += SPEED[i].vy;

		if (x > (320 - TRI_SIZE))
			x = 0;

		if (y > (200 - TRI_SIZE))
			y = 0;

		setXY3(&TRIS[i],
				x,y,
				x+TRI_SIZE, y,
				x, y + TRI_SIZE);
	}
}