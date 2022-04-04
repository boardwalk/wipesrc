#include "standard.h"
#include <stdio.h>
#include <stdlib.h>
#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "combat.h"
#include "menus.h"
#include "hiscores.h"
#include "hud.h"
#include "droid.h"
#include "camera.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"

void Error(const char* string, int16_t errorLevel) {
  printf("Error: %s (error level %d)\n", string, errorLevel);
  exit(errorLevel);

#if PCwipeout
  BlkFill* clear[2];
  int16_t i;

  clear[0] = BlockFill(0, 0, 320, 240, 0x00, 0x00, 0x00);
  clear[1] = BlockFill(0, 240, 320, 240, 0x00, 0x00, 0x00);

  switch (errorLevel) {
  case Warning:
    printf("Warning!!!  ");
    printf("%s\n", string);
    break;

  case NonFatal:
    printf("NonFatal Error!!!  ");
    printf("%s\n", string);
    for (i = 0; i < (60 * 2); i++) {
      ClearOTagR(OT[CurrentScreen], OT_SIZE);

      AddPrim(OT[CurrentScreen], OT_SIZE - 1, (P_TAG*)clear[CurrentScreen]);

      ObjectTable[CurrentScreen] = OT[CurrentScreen] + OT_SIZE - 1;

      Swap();
    }
    break;

  case Fatal:
    printf("Fatal Error!!!  ");
    printf("%s\n", string);
    for (i = 0; i < 2; i++) {
      ClearOTagR(OT[CurrentScreen], OT_SIZE);

      AddPrim(OT[CurrentScreen], OT_SIZE - 1, (P_TAG*)clear[CurrentScreen]);

      ObjectTable[CurrentScreen] = OT[CurrentScreen] + (OT_SIZE - 1);

      Swap();
    }
    exit(0);
    break;
  }
#endif
}
