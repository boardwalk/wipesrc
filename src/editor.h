#ifndef EDITOR_H
#define EDITOR_H

#include <stdint.h>
#include "libgpu.h"

/* the tools */
#define fDynamics 0
#define fTrans 1
#define fBug 2
#define fSky 3
#define fLand 4
#define fSave 5

#define fDumpVram 6

/* the popouts attached to 0 */
#define fCamera 7
#define fTexSelect 8
#define fDpa 9
#define fRadius 10
#define fTrakView 11

/* used in the texture viewer */
#define fEmpty 12

#define numTools 7
#define maxPopouts 5

typedef struct Tools {
  int16_t type; /* -1 for popout otherwise toggle type */
  int16_t onOff; /* whether icon is toggled on or off for toggle type */

  int16_t popOut[maxPopouts]; /* the popout indices */
  int16_t currentPopout; /* the curent selected popout */
  int16_t poppedOut; /* are we popped out or not */
  POLY_GT4 popIcons[2][maxPopouts]; /* the popouts */
  int16_t numPopouts; /* the number of icons that pop out */

  int16_t leftPos; /* position of popped out highlighter ( 0 right ) */
  POLY_G4 poppedHighlight[2]; /* the popped out highlight */

} Tools;

typedef struct Editor {
  int16_t status; /* editor on 1 / off 0 */

  int16_t iconTextures; /* index into texture table */

  POLY_GT4 icons[2][numTools]; /* the actual icons */
  POLY_G4 selected[2][numTools]; /* the always selected blue overlays */

  int16_t downPos; /* position of highlighter ( 0 top )*/
  POLY_G4 highlight[2]; /* the cursor highlight */

  Tools toolBar[numTools];

  int16_t justSaved; /* timer for the just saved message */
  int16_t justDumped; /* timer for the just dumped vram message */
} Editor;

void InitEditor(Editor* editor);
void EditorUpdate(Editor* editor, char level);
int16_t EditorFlyMode(Editor* editor);

#endif
