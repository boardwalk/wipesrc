#include <stdint.h>

bool TimerInstalled = false;
extern int32_t frameRate30;

extern int32_t FR30;
extern int32_t FR60;
extern int32_t FR50;
extern int32_t FR6;
extern int32_t FR10;
extern int32_t FR40;
extern int32_t FR50U;
extern int32_t FR15U;
extern int32_t FR30U;
extern int32_t FR7U;
extern int32_t FR32U;

void InstallTimer() {
  if (!TimerInstalled) {
    FR30 = frameRate30;
    FR60 = frameRate30 * 2;
    FR50 = (frameRate30 * 50) / 30;
    FR6 = (frameRate30 * 6) / 30;
    FR10 = frameRate30 / 3;
    FR40 = (frameRate30 * 40) / 30;

    FR50U = (50 * 30) / frameRate30;
    FR15U = (15 * 30) / frameRate30;
    FR30U = (30 * 30) / frameRate30;
    FR7U = (7 * 30) / frameRate30;
    FR32U = (32 * 30) / frameRate30;

    TimerInstalled = true;
  }
}
