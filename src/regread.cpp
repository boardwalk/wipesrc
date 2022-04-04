#include <stdint.h>

extern char Resolution, controller;
extern int32_t DrawDistance;
extern int16_t ft;
extern char Sky;
extern uint16_t NewOriginalKeyCodes[], OriginalKeyCodes[];

int32_t ShellGetPrefs() {
  Resolution = 3;
  ft = 0;
  Sky = 1;
  DrawDistance = 65535;
  controller = 1;
  return 0;
}
