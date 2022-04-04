#ifndef AJS_H
#define AJS_H

#include <stdint.h>
#include "libgpu.h"

void	SetPolyGT3(POLY_GT3 *p);
void	InitAJSVSync();
void setWinLose();
void	AddPrim(P_TAG *ordt, int32_t depth, P_TAG *p);
void SetSemiTrans(P_TAG *p, int32_t abe);
void SetShadeTex(P_TAG *p, int32_t tge);
void	LoadPilotIntoVram(char *filename);
void LoadClut4(RECT	*rect, uint16_t *clutptr, CLUT *cluts);
int32_t		LoadImage4(RECT	*recp, uint32_t *p, CLUT *cluts);
void LoadClut8(RECT	*rect, uint16_t *clutptr, CLUT *cluts);
int32_t		LoadImage8(RECT	*recp, uint32_t *p, CLUT *cluts);

#endif
