#ifndef WINCD_H
#define WINCD_H

#include <stdint.h>

struct cdlPos {
  int32_t min;
  int32_t sec;
};

void CDRestart();
void CdSetVol(int32_t newVol);
int32_t numTracks();
void CDPlay(int16_t CurrTrackNo);
int32_t CdGetToc2(int32_t sess, struct CdlPos* table);

#endif
