/************************************************************
*
* File: sbsfx.h												 
*
*	Header file to sbsfx.c
*
*************************************************************
*
* $Log:   P:/ridger/ridger/SBSFX.H_v  $
 * 
 *    Rev 1.0   13 Jul 1995 14:40:28   DaveS
 * Put Back after we stopped being naughty children
 * 
 *    Rev 1.2   08 Feb 1995 14:14:04   AndrewC
 * Parameters to SBlasterSfxStart() changed
 * 
 *    Rev 1.1   18 Nov 1994 15:22:16   AndrewC
 * functions renamed, some parameters added to SBlasterSfxInit();
*
************************************************************/
uint16 SBlasterSfxInit(uint16 *pwIrq,uint16 *pwChnls,uint16 *pwBits);
void SBlasterSfxStart(uint16 wSampleRate,uint16 wChannels,
											uint32 dwStartAddr,uint16 wBlockSize);
void SBlasterRecStart(uint16 wSampleRate,uint32 dwStartAddr,uint16 wBlockSize);
void SBlasterAckInt(void);
void SBlasterAckRecInt(void);
void SBlasterSfxSetVolume(uint8 bVolume);
void SBlasterSfxUnInit(void);
