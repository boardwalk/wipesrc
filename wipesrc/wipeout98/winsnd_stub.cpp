#include <cstdint>
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;

void SfxEffectPan(int16 wPlayId,uint16 bPan)
{}

void SfxEffectVol(int16 wPlayId,uint8 bVolume)
{}

uint16 SfxGetFxBank(char *filename)
{ return 0; }

void SfxReverbOff(void)
{}

void SfxSetVolume(uint8 bVolume)
{}

uint16 SfxInitReverb(uint16 wReverbType,uint16 wDelay)
{ return 0; }

int16 SfxOpenFxBank(int16 *pwFirstId)
{ return 0; }

void SfxEffectPitch(int16 wPlayId,uint16 wPitch)
{}

void SfxSetCdVolume(uint8 bVolume)
{}

uint16 SfxSetMaxEffects(uint16 wNum)
{ return 0; }

void SfxSetReverbDepth(uint8 bDepth)
{}

void SfxOn(void)
{}

uint16 SfxInit(uint16 wCard,uint16 wMixType,uint16 wSampRate)
{ return 0; }

void SfxPlay(int16 wSampleId,uint8 bVolume,uint8 bLRPan,uint8 bFBPan,
						 uint16 wPitch,uint8 bFlags)
{}

int16 SfxPlayID(int16 wSampleId,uint8 bVolume,uint8 bLRPan,uint8 bFBPan,
						 uint16 wPitch,uint8 bFlags)
{ return 0; }

void SfxStop(int16 wPlayId)
{}

void KillAllFx(void)
{}

void SfxUnInit(uint8 bFlag)
{}
