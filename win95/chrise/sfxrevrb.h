#ifndef _REVERB_H_INCLUDED_
#define _REVERB_H_INCLUDED_

uint16 InitReverb(uint16 wReverbType,uint8 bChannels,uint16 wDelay);
void ClearReverbBuffers(void);
void SetReverbDepth(uint8 bDepth);
void UnInitReverb(void);
void AddReverb(void);


#endif
