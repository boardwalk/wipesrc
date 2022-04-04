#ifndef HUD_H
#define HUD_H

#include <stdint.h>
#include "hiscores.h"
#include "libgpu.h"
#include "menus.h"
#include "ships.h"
#include "standard.h"
#include "tim.h"

#define NUM_BARS 12
#define SPEEDO_X 179
#if PAL
#define SPEEDO_Y 220 /*200*/
#else
#define SPEEDO_Y 195 /*200*/
#endif
#define SPEEDO_WIDTH 127
#define SPEEDO_HEIGHT 31 /*31*/
#define BAR_X 178
#if PAL
#define BAR_Y 254
#else
#define BAR_Y 229
#endif
#define MAX_TEXT 256
#define MINUS 10
#define DOT 10
#define COLON 11
#define SLASH 12
#define RACE 0
#define MENU 1

typedef struct
{
  int16_t topx;
  int16_t topy;
  int16_t bottomx;
  int16_t bottomy;
} Points;

typedef struct
{
  Points Left, Right;
  char r0; // PCwipeout
  char r1, g1, b1;
  char r2, g2, b2;
} BarPos;

typedef struct
{
  SPRT facia[2];
  SPRT transbar[2];
  SPRT solidbar[2];
  DR_MODE fred[2];
  //	POLY_GT4  	bars[NUM_BARS][2];
  //	DR_MODE		transPrim[NUM_BARS][2];
  //	POLY_G4  	thrustBar[NUM_BARS][2];
  BarPos barInfo[NUM_BARS];
} Speedo;

typedef struct
{
  int32_t start_pos_x;
  int32_t start_pos_y;
  int32_t size;
} TEXT_DATA;

typedef struct
{
  int16_t vramX;
  int16_t vramY;
  int16_t width;
  int16_t pad;
} TexChar;

typedef struct
{
  char pilot[25];
  int16_t x, y;
} PilotData;

void InitText();
void InitSpeedo(Speedo*);
void fill_speedo(Speedo*, int32_t, int32_t);
void AddNumber(int32_t, TEXT_DATA*, int32_t);
void AddTime(int32_t, TEXT_DATA*, int32_t);
void AddText(const char*, TEXT_DATA*, int32_t);
void SpecialText(char*, TEXT_DATA*, int32_t);
void demo_screen(POLY_FT4*, int32_t);
TEXT_DATA* text_data(int32_t, int32_t, int32_t);
void UpdateRaceHud(ShipData*, Speedo*, int32_t, ConfigData*);
void AddText2(char*, TEXT_DATA*, int32_t);
void AddNumber2(int32_t, TEXT_DATA*, int32_t);
void SetCharTextures(char);
void AddDelEnd(char, TEXT_DATA*, int32_t);
void CentreText(const char*, TEXT_DATA*, int32_t);
void DisplayLives(SPRT[][2], ConfigData*); // PCwipeout
//void 			DisplayLives 		(POLY_FT4 [][2], ConfigData *)	;
int32_t CalcPosition(ShipData* shipIndex);
void ScreenFooter(int32_t x, int32_t y, int32_t colour);
void ShowCopyRight(Texture* copyrightSymbol, POLY_FT4* copyPrim, int16_t x, int16_t y);
void InitChampTable();
void DisplayChampTable();
void DisplayRacePoints();
void InitScreenTex(Texture** screenTex, const char* fileName);
void DisplayWinLose(Texture* winLose, POLY_FT4* picPrim, int16_t x, int16_t y, int16_t height, char type);
void DisplayVenomComplete();
void DisplayAllVenomComplete();
void InitTrackTex(Texture** trackTex);
void BackPoly(POLY_F4*, DR_MODE*, int16_t, int16_t, int16_t, int16_t);
void DisplayRaceInfo(ConfigData* gameData, HiScoreData* hiScoreData);
void DrawHud(ShipData* shipIndex, Speedo* speedo, int32_t choice, ConfigData* gameData);

#endif
