#include <stdio.h>
#include <stdint.h>

void SfxEffectPan(int16_t wPlayId, uint16_t bPan) {
}

void SfxEffectVol(int16_t wPlayId, uint8_t bVolume) {
}

uint16_t SfxGetFxBank(const char* filename) {
  return 0;
}

void SfxReverbOff() {
}

void SfxSetVolume(uint8_t bVolume) {
}

uint16_t SfxInitReverb(uint16_t wReverbType, uint16_t wDelay) {
  return 0;
}

int16_t SfxOpenFxBank(int16_t* pwFirstId) {
  return 0;
}

void SfxEffectPitch(int16_t wPlayId, uint16_t wPitch) {
}

void SfxSetCdVolume(uint8_t bVolume) {
}

uint16_t SfxSetMaxEffects(uint16_t wNum) {
  return 0;
}

void SfxSetReverbDepth(uint8_t bDepth) {
}

void SfxOn() {
}

uint16_t SfxInit(uint16_t wCard, uint16_t wMixType, uint16_t wSampRate) {
  // "NO_SOUND"
  return 1;
}

void SfxPlay(int16_t wSampleId, uint8_t bVolume, uint8_t bLRPan, uint8_t bFBPan,
             uint16_t wPitch, uint8_t bFlags) {
  printf("SfxPlay(wSampleId=%d)\n", wSampleId);
}

int16_t SfxPlayID(int16_t wSampleId, uint8_t bVolume, uint8_t bLRPan, uint8_t bFBPan,
                  uint16_t wPitch, uint8_t bFlags) {
  printf("SfxPlayID(wSampleId=%d)\n", wSampleId);
  return -1;
}

void SfxStop(int16_t wPlayId) {
}

void KillAllFx() {
}

void SfxUnInit(uint8_t bFlag) {
}
