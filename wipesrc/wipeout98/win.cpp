#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "wipesrc/psx26/include/libgte.h"
#include "wipesrc/psx26/include/libgpu.h"
#include "input.h"
#include "object.h"
#include "track.h"
#include "ships.h"
#include "pad.h"
#include <stdio.h>
#include <GLFW/glfw3.h>

typedef int32_t BOOL;
#define		TRUE			1
#define		FALSE			0

typedef struct
{
	int32_t			id;
	int32_t 			flags;
	int32_t			bnum;
	int16_t			textureX, textureY;
	int16_t 			textureH, textureW;
} TimData;

int16_t ft=0;
char MipMap = 0;
char *SaveDirBase;
char *DataDirBase;
char *MovieDirBase;
char globalPilot[255]={0};
int16_t FrameState=0;
int32_t menuTexturesIn = 0;
int32_t numoff=0;
int32_t numsmall=0;
int16_t screenres=3;
int16_t trackNum = 0;
float upres=2.0f;

static GLFWwindow* g_window = nullptr;

char *LockPilot()
{
    return NULL;
}

void UnlockPilot()
{}

BOOL JJSJoyConfig()
{ return FALSE; }

void changeScreen()
{
	printf("changeScreen\n");
}

void setRenderBegin()
{}

void setRenderEnd()
{}

void JJS_Sprite(SPRT *sp)
{
	// printf("JJS_Sprite(x0=%d, y0=%d, u0=%u, v0=%u, clut=%u, w=%d, h=%d\n",
	// 	sp->x0, sp->y0, sp->u0, sp->v0, sp->clut, sp->w, sp->h);
}

void JJS_Sprite8(SPRT_8 *sp)
{
	printf("JJS_Sprite8\n");
}

void	    JJSDrawPolyF3(POLY_F3 * p)
{
	// printf("f3 p0=(%d,%d) p1=(%d,%d) p2=(%d,%d)\n",
	// 	p->x0, p->y0,
	// 	p->x1, p->y1,
	// 	p->x2, p->y2);

	glBegin(GL_TRIANGLES);
	// glColor3b(p->r0, p->g0, p->b0);
	glVertex2s(p->x0, p->y0);
	glVertex2s(p->x1, p->y1);
	glVertex2s(p->x2, p->y2);
	glEnd();
}

void	    JJSDrawPolyF4(POLY_F4 * p)
{
	// printf("f4 p0=(%d,%d) p1=(%d,%d) p2=(%d,%d) p3=(%d,%d)\n",
	// 	p->x0, p->y0,
	// 	p->x1, p->y1,
	// 	p->x2, p->y2,
	// 	p->x3, p->y3);

	glBegin(GL_QUADS);
	// glColor3b(p->r0, p->g0, p->b0);
	glVertex2s(p->x0, p->y0);
	glVertex2s(p->x1, p->y1);
	glVertex2s(p->x2, p->y2);
	glVertex2s(p->x3, p->y3);
	glEnd();
}

void	    JJSDrawPolyG3(POLY_G3 * p)
{
	// printf("g3 p0=(%d,%d) p1=(%d,%d) p2=(%d,%d)\n",
	// 	p->x0, p->y0,
	// 	p->x1, p->y1,
	// 	p->x2, p->y2);

	glBegin(GL_TRIANGLES);
	// glColor3b(p->r0, p->g0, p->b0);
	glVertex2s(p->x0, p->y0);
	// glColor3b(p->r1, p->g1, p->b1);
	glVertex2s(p->x1, p->y1);
	// glColor3b(p->r2, p->g2, p->b2);
	glVertex2s(p->x2, p->y2);
	glEnd();
}

void	    JJSDrawPolyG4(POLY_G4 * p)
{
	// printf("g4 p0=(%d,%d) p1=(%d,%d) p2=(%d,%d) p3=(%d,%d)\n",
	// 	p->x0, p->y0,
	// 	p->x1, p->y1,
	// 	p->x2, p->y2,
	// 	p->x3, p->y3);

	glBegin(GL_QUADS);
	// glColor3b(p->r0, p->g0, p->b0);
	glVertex2s(p->x0, p->y0);
	// glColor3b(p->r1, p->g1, p->b1);
	glVertex2s(p->x1, p->y1);
	// glColor3b(p->r2, p->g2, p->b2);
	glVertex2s(p->x2, p->y2);
	// glColor3b(p->r3, p->g3, p->b3);
	glVertex2s(p->x3, p->y3);
	glEnd();
}

void	    JJSDrawPolyFT3(POLY_FT3 * p)
{
	// printf("ft3 p0=(%d,%d) p1=(%d,%d) p2=(%d,%d)\n",
	// 	p->x0, p->y0,
	// 	p->x1, p->y1,
	// 	p->x2, p->y2);

	glBegin(GL_TRIANGLES);
	// glColor3b(255, 255, 255);
	glVertex2s(p->x0, p->y0);
	glVertex2s(p->x1, p->y1);
	glVertex2s(p->x2, p->y2);
	glEnd();
}

void	    JJSDrawPolyFT4(POLY_FT4 * p)
{
	// printf("ft4 p0=(%d,%d) p1=(%d,%d) p2=(%d,%d) p3=(%d,%d)\n",
	// 	p->x0, p->y0,
	// 	p->x1, p->y1,
	// 	p->x2, p->y2,
	// 	p->x3, p->y3);

	glBegin(GL_QUADS);
	// glColor3b(255, 255, 255);
	glVertex2s(p->x0, p->y0);
	glVertex2s(p->x1, p->y1);
	glVertex2s(p->x2, p->y2);
	glVertex2s(p->x3, p->y3);
	glEnd();
}

void	    JJSDrawPolyGT3(POLY_GT3 * p)
{
	// printf("gt3 p0=(%d,%d) p1=(%d,%d) p2=(%d,%d), tpage=%d\n",
	// 	p->x0, p->y0,
	// 	p->x1, p->y1,
	// 	p->x2, p->y2,
	// 	p->tpage);

	glBegin(GL_TRIANGLES);
	// glColor3b(255, 255, 255);
	glVertex2s(p->x0, p->y0);
	glVertex2s(p->x1, p->y1);
	glVertex2s(p->x2, p->y2);
	glEnd();
}

void	    JJSDrawPolyGT4(POLY_GT4 * p)
{
	// printf("gt4 p0=(%d,%d) p1=(%d,%d) p2=(%d,%d) p3=(%d,%d)\n",
	// 	p->x0, p->y0,
	// 	p->x1, p->y1,
	// 	p->x2, p->y2,
	// 	p->x3, p->y3);

	glBegin(GL_QUADS);
	// glColor3b(255, 255, 255);
	glVertex2s(p->x0, p->y0);
	glVertex2s(p->x1, p->y1);
	glVertex2s(p->x2, p->y2);
	glVertex2s(p->x3, p->y3);
	glEnd();
}

BOOL SJRNewTexture(const char *filename)
{
	printf("SJRNewTexture(%s)\n", filename);
	return FALSE;
}

void centreMouse()
{}

int32_t JJSLoadSprites ()
{
	printf("JJSLoadSprites()\n");
	return 0;
}

int32_t	     LockBackBuffer()
{ return 0; }

void	    UnlockBackBuffer()
{}

void	    WinClearScreen(char colour)
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void	    BltClearToScreen()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void	    DDSwap()
{
	// printf("DDSwap\n");
	glfwSwapBuffers(g_window);
}

int32_t	     ProcessMessages()
{
	glfwPollEvents();
	return 0;
}

TimData *JJSLoad16BitTexture(const char *filename, char location)
{
	printf("JJSLoad16BitTexture(%s, %d)\n", filename, location);
	return NULL;
}

int16_t randlist[2000];
int16_t *randnum;
int16_t ctr;

void initrandy()
{
	int16_t i;
	time_t t;

	t=time(NULL);

	srand(t);

	for (i = 0; i < 2000; i++)
		randlist[i] = rand() & 0x7FFF;

	randnum = &randlist[0];;
	ctr = 0;
}

int32_t randy()
{
	int16_t num;

	num = *randnum++;

	ctr++;
	if (ctr == 2000)
	{
		randnum = &randlist[0];;
		ctr = 0;
	}

	return num;

}

void	    CopyPal(char *palette, int16_t start, int16_t end)
{}

void	    WinQuit()
{}

void sendList()
{}

void oldmain();

extern int16_t    WinWidth;
extern int16_t    WinHeight;
extern int16_t    WinHeightX2;

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	uint8_t scan_code;
	switch(key) {
		case GLFW_KEY_ESCAPE: scan_code = _ESC_KEY; break;
		case GLFW_KEY_TAB: scan_code = _TAB_KEY; break;
		case GLFW_KEY_P: scan_code = _P_KEY; break;
		case GLFW_KEY_ENTER: scan_code = _ENTER_KEY; break;
		case GLFW_KEY_LEFT_CONTROL: scan_code = _LEFT_CTRL_KEY; break;
		case GLFW_KEY_LEFT_SHIFT: scan_code = _LEFT_SHIFT_KEY; break;
		case GLFW_KEY_UP: scan_code = _UP_KEY; break;
		case GLFW_KEY_LEFT: scan_code = _LEFT_KEY; break;
		case GLFW_KEY_RIGHT: scan_code = _RIGHT_KEY; break;
		case GLFW_KEY_DOWN: scan_code = _DOWN_KEY; break;
		case GLFW_KEY_Z: scan_code = _Z_KEY; break;
		case GLFW_KEY_X: scan_code = _X_KEY; break;
		case GLFW_KEY_F1: scan_code = _F1_KEY; break;
		case GLFW_KEY_F2: scan_code = _F2_KEY; break;
		case GLFW_KEY_F3: scan_code = _F3_KEY; break;
		case GLFW_KEY_F4: scan_code = _F4_KEY; break;
		case GLFW_KEY_F5: scan_code = _F5_KEY; break;
		case GLFW_KEY_F6: scan_code = _F6_KEY; break;
		case GLFW_KEY_F7: scan_code = _F7_KEY; break;
		case GLFW_KEY_F8: scan_code = _F8_KEY; break;
		case GLFW_KEY_F9: scan_code = _F9_KEY; break;
		case GLFW_KEY_F10: scan_code = _F10_KEY; break;
		case GLFW_KEY_F11: scan_code = _F11_KEY; break;
		case GLFW_KEY_F12: scan_code = _F12_KEY; break;
		case GLFW_KEY_LEFT_ALT: scan_code = _LEFT_ALT_KEY; break;
		default: return;
	}

	if (action == GLFW_PRESS) {
		pmkey(scan_code, 1);
	} else if(action == GLFW_RELEASE) {
		pmkey(scan_code, 0);
	}
}

int main(int argc, char** argv)
{
	initrandy();

	WinWidth = 640;
	WinHeight = 480;
	WinHeightX2 = WinHeight * 2;

	SaveDirBase = (char *)malloc(120);
	strcpy(SaveDirBase, ".");

	DataDirBase = (char *)malloc(120);
	strcpy(DataDirBase, ".");

	MovieDirBase = (char *)malloc(120);
	strcpy(MovieDirBase, ".");

	chdir(DataDirBase);

	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize glfw\n");
		return EXIT_FAILURE;
	}

	g_window = glfwCreateWindow(WinWidth, WinHeight, "Wipeout", nullptr, nullptr);
	if (g_window == nullptr) {
		fprintf(stderr, "Failed to create window\n");
		return EXIT_FAILURE;
	}

	glfwSetKeyCallback(g_window, keyCallback);

	glfwMakeContextCurrent(g_window);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 320.0, 240.0, 0.0, -1.0, 1.0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glCullFace(GL_FRONT_AND_BACK);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	oldmain();

	glfwTerminate();
	return 0;
}