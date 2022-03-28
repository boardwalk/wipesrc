/************************************************
*
* file: sfxmix.h
* 
* Header file to sfxmix.c
*
*************************************************
*
* $Log:   P:/ridger/ridger/SFXMIX.H_v  $
 * 
 *    Rev 1.0   13 Jul 1995 14:40:42   DaveS
 * Put Back after we stopped being naughty children
 * 
 *    Rev 1.3   08 Feb 1995 14:08:04   AndrewC
 * StereoMix() prototype added
 * 
 *    Rev 1.2   18 Nov 1994 15:20:02   AndrewC
 * prototypes to new functions added.
*
************************************************/
uint16 MixSetup(uint16 wMixType);
void ClearDmaBuf(void);
void ClearMixBuf(void);
uint16 MonoMix(struct playListStruct **ppItem);
uint16 PhaseMix(struct playListStruct **ppItem);
uint16 QStereoMix(struct playListStruct **ppItem);
void Mix2Dma16Mono(void);
void Mix2Dma16Stereo(void);
void Mix2Dma8Mono(void);
void Mix2Dma8Stereo(void);
void MixChat(void);
