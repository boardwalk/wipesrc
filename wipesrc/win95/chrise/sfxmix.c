/*
 * Known bugs...
 * Looping in PhaseMix not correct, causes some glitching
 */
#include "c:\watcom\h\stdio.h"
#include "c:\watcom\h\stdlib.h"
#include "utypes.h"
#include <math.h>

/* mix types */
#define MONO 	1				
#define STEREO  2
#define PHASED  3
#define QSOUND  4

/* effect flags */
#define ACTIVE      0x01
#define LOOP        0x02
#define CONTROL     0x04              
#define CHAINED     0x08
#define SURROUND   	0x10
#define QSND_IMAGE 	0x20
#define CALLBACK   	0x80

#define VOLUME_LEVELS 32  /* change in sfx.c also */

/* mix flags */
#define COPY		0x00		
#define MIX			0x01

#define NORMAL		0x00
#define PITCH		0x02

#define FULL		0x00
#define HALF 		0x04

#define NON_DOLBY 	0x00
#define DOLBY		0x08

#define ONE_CHNL	0x10

#define SKIP_MIX 	0xFFFF

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define SIGN(x)  ((x) >= 0 ? (1) : (-1))

/*
 * #ifdef defs
 */
//#define CLEAR_MIX		      
//#define COPY_DMA
//#define MONO_MIX
//#define STEREO_MIX
//#define SURROUND_MIX

/*
 * Local prototypes
 */
void MonoCopyNormal(void);
void MonoCopyNormalHalf(void);
void MonoCopyPitch(void);
void MonoCopyPitchHalf(void);
void MonoMixNormal(void);
void MonoMixNormalHalf(void);
void MonoMixPitch(void);
void MonoMixPitchHalf(void);

void MonoZeroPad(void);

void StereoCopyNormal(void);
void StereoCopyNormalHalf(void);
void StereoCopyPitch(void);
void StereoCopyPitchHalf(void);
void StereoMixNormal(void);
void StereoMixNormalHalf(void);
void StereoMixPitch(void);
void StereoMixPitchHalf(void);

void StereoCopyNormalSurround(void);
void StereoCopyNormalSurroundHalf(void);
void StereoCopyPitchSurround(void);
void StereoCopyPitchSurroundHalf(void);
void StereoMixNormalSurround(void);
void StereoMixNormalSurroundHalf(void);
void StereoMixPitchSurround(void);
void StereoMixPitchSurroundHalf(void);

void StereoZeroPad(void);

static uint16 GenDeCmprs(void);
static uint16 GenScale8(void);
static uint16 GenPanLookup(void);
static uint16 GenPhaseLookup(void);
static uint16 GenQSoundLookup(void);


struct phaseStruct
{
	uint8 bDelay;
	uint8 bVol;
};

struct QSndStruct
{
	uint8 bOrgL;
	uint8 bOrgR;
	uint8 bImgL;
	uint8 bImgR;
};

/*
 * Imported globals from Sfx.c
 */
extern int32 *pdwMixBufG;
extern int16 *ppvBufferG[2];
extern volatile uint16 wBufferFlagG;
extern uint16 wMaxActiveG;
extern uint16 wMaxBitsG;
extern uint32 dwMixBufSamplesG;
extern uint32 dwMixBufFramesG;
extern uint32 dwDmaBlockSizeG;
extern uint16 wSampleRateG;

extern uint8 *pbChatMsgPtrG;				/* pointer to message buffer */
extern uint32 dwChatMsgIndxG;				/* index to message buffer */
extern uint32 dwChatMsgSizeG;				/* length of data in message buffer */
extern uint8 bChatReadyG;

extern uint32 dwDebug; 

struct playListStruct{
	uint16 wPlayId;
	uint16 wFlags;
	uint8 bVolume;
	uint8 bPan;
	uint32 dwSmpIndx;			 /* Mono and left index */
	uint32 dwSmpIndx2;		 /* Second indx (for phased stereo) */
	uint32 dwSmpLen;
	uint8 *pbSmpPtr;
	uint16 wSrate;
	uint16 wIncrement;
	uint16 wIncIndx;
	uint16 wIncIndx2;
	struct playListStruct *pNext;
	struct playListStruct *pPrev;
	struct playListStruct *pChain;
	uint8 *pbImgPtr;		/* QSound image file */
}; 

/*
 * TEMP DEBUG STUFF 
 */
uint32 dwDebug1G;
uint32 dwDebug2G;
uint32 dwDebug3G;
uint32 dwDebug4G;

/*
 * Global variables used in mixing routines 
 */
uint16 wOffsetG;
int32 *pdwDstPtrG;
uint8  *pbSrcPtrG;
uint32 dwLoopCountG;
uint16 wIncIndxG;						  
uint16 wIncrementG;
uint8 bLeftOffG;
uint8 bRightOffG;
		 
int16 *pwDeCmprsG;				/* 16k byte lookup table */
int8 *pbScaleG;						/* 8k byte lookup table */
uint8 *pbPanG;
struct phaseStruct *pPhase22G;
struct phaseStruct *pPhase11G;

static uint8 *pQSndLookupG;

static uint8 pbLeadG[2];			/* Global cos it works !!! */

void (*pMixFunctionTableG[32])(void);	/* Table of pointers to mix routines */

void (*DoNormalMix)(void);
void (*DoPitchMix)(void);
void (*DoNormalSurround)(void);
void (*DoPitchSurround)(void);

uint16 MixSetup(uint16 wMixType)
{
	/*
	 * Setup array of pointers to mixing routines
	 */
	switch(wMixType)
	{
		case PHASED:
			pMixFunctionTableG[DOLBY|COPY|NORMAL|FULL] = StereoCopyNormalSurround;
			pMixFunctionTableG[DOLBY|COPY|NORMAL|HALF] = StereoCopyNormalSurroundHalf;
			pMixFunctionTableG[DOLBY|COPY|PITCH|FULL] = StereoCopyPitchSurround;
			pMixFunctionTableG[DOLBY|COPY|PITCH|HALF] = StereoCopyPitchSurroundHalf;
			pMixFunctionTableG[DOLBY|MIX|NORMAL|FULL] = StereoMixNormalSurround;
			pMixFunctionTableG[DOLBY|MIX|NORMAL|HALF] = StereoMixNormalSurroundHalf;
			pMixFunctionTableG[DOLBY|MIX|PITCH|FULL] = StereoMixPitchSurround;
			pMixFunctionTableG[DOLBY|MIX|PITCH|HALF] = StereoMixPitchSurroundHalf;
			/* no break ! */
		case MONO:
			pMixFunctionTableG[COPY|NORMAL|FULL] = MonoCopyNormal;
			pMixFunctionTableG[COPY|NORMAL|HALF] = MonoCopyNormalHalf;
			pMixFunctionTableG[COPY|PITCH|FULL] = MonoCopyPitch;
			pMixFunctionTableG[COPY|PITCH|HALF] = MonoCopyPitchHalf;
			pMixFunctionTableG[MIX|NORMAL|FULL] = MonoMixNormal;
			pMixFunctionTableG[MIX|NORMAL|HALF] = MonoMixNormalHalf;
			pMixFunctionTableG[MIX|PITCH|FULL] = MonoMixPitch;
			pMixFunctionTableG[MIX|PITCH|HALF] = MonoMixPitchHalf;
			break;
		case STEREO:
		case QSOUND:
			pMixFunctionTableG[COPY|NORMAL|FULL] = StereoCopyNormal;
			pMixFunctionTableG[COPY|NORMAL|HALF] = StereoCopyNormalHalf;
			pMixFunctionTableG[COPY|PITCH|FULL] = StereoCopyPitch;
			pMixFunctionTableG[COPY|PITCH|HALF] = StereoCopyPitchHalf;
			pMixFunctionTableG[MIX|NORMAL|FULL] = StereoMixNormal;
			pMixFunctionTableG[MIX|NORMAL|HALF] = StereoMixNormalHalf;
			pMixFunctionTableG[MIX|PITCH|FULL] = StereoMixPitch;
			pMixFunctionTableG[MIX|PITCH|HALF] = StereoMixPitchHalf;
			pMixFunctionTableG[DOLBY|COPY|NORMAL|FULL] = StereoCopyNormalSurround;
			pMixFunctionTableG[DOLBY|COPY|NORMAL|HALF] = StereoCopyNormalSurroundHalf;
			pMixFunctionTableG[DOLBY|COPY|PITCH|FULL] = StereoCopyPitchSurround;
			pMixFunctionTableG[DOLBY|COPY|PITCH|HALF] = StereoCopyPitchSurroundHalf;
			pMixFunctionTableG[DOLBY|MIX|NORMAL|FULL] = StereoMixNormalSurround;
			pMixFunctionTableG[DOLBY|MIX|NORMAL|HALF] = StereoMixNormalSurroundHalf;
			pMixFunctionTableG[DOLBY|MIX|PITCH|FULL] = StereoMixPitchSurround;
			pMixFunctionTableG[DOLBY|MIX|PITCH|HALF] = StereoMixPitchSurroundHalf;
			pMixFunctionTableG[ONE_CHNL|COPY|NORMAL|FULL] = MonoCopyNormal;
			pMixFunctionTableG[ONE_CHNL|COPY|NORMAL|HALF] = MonoCopyNormalHalf;
			pMixFunctionTableG[ONE_CHNL|COPY|PITCH|FULL] = MonoCopyPitch;
			pMixFunctionTableG[ONE_CHNL|COPY|PITCH|HALF] = MonoCopyPitchHalf;
			pMixFunctionTableG[ONE_CHNL|MIX|NORMAL|FULL] = MonoMixNormal;
			pMixFunctionTableG[ONE_CHNL|MIX|NORMAL|HALF] = MonoMixNormalHalf;
			pMixFunctionTableG[ONE_CHNL|MIX|PITCH|FULL] = MonoMixPitch;
			pMixFunctionTableG[ONE_CHNL|MIX|PITCH|HALF] = MonoMixPitchHalf;
			break;			
	}

	/*
	 * Generate Lookup tables 
	 */
	if(wMaxBitsG == 16)
	{
		if(GenDeCmprs())
		{
			return 1;		/* failed to allocate memory */
		}
	}
	else
	{
		if(GenScale8())
		{
			return 1;
		}
	}

	if(wMixType >= STEREO)
	{
		if(GenPanLookup())
			return 1;
	}

	if(wMixType == PHASED)
	{
		if(GenPhaseLookup())
			return 1;
	}

	if(wMixType == QSOUND)
	{
		if(GenQSoundLookup())
			return 1;
	}

	return 0;	
}

                                                                                                                                                                                               
uint16 MonoMix(struct playListStruct **ppItem)
{
	extern uint32 dwMixBufFramesG;
	extern int32 *pdwMixBufG;
	extern int16 *pwDeCmprsG;
	extern uint16 wMaxActiveG;

	struct playListStruct *pEffect;
	uint32 dwLoopLen;
	uint8 *pbSmpPtr;
	uint32 dwSmpIndx;
	uint16 wUpdate;
	uint16 wCount;
	uint16 wVol;
	uint16 wMixType;
	uint8 bComplete;

	wMixType = 0;
	wCount = 0;
	wUpdate = 0;
	pEffect = *ppItem;

	/* First time round, copy samples to buffer */
	wMixType = COPY;

	while(pEffect && (wCount < wMaxActiveG))
	{
		wCount++;
		bComplete = 0;

		pbSmpPtr = pEffect->pbSmpPtr;
		dwSmpIndx = pEffect->dwSmpIndx;

		/*
		 * Calculate decompression offsets from
		 * volume and pan
		 */
		wVol = (uint16)pEffect->bVolume;
		wOffsetG = wVol << 8;

		pdwDstPtrG = pdwMixBufG;         /* start of mix buffer */
		pbSrcPtrG = pbSmpPtr+dwSmpIndx;

		if(pEffect->wIncrement == 256)
		{
			/*
			 * Effect is normal pitch 
			 */
			wMixType |= NORMAL;						

			if(pEffect->wSrate == wSampleRateG)
			{
				/* mix at full sample rate */
	 	 		dwLoopCountG = MIN(((pEffect->dwSmpLen)-dwSmpIndx),dwMixBufFramesG);
			  dwLoopLen = dwLoopCountG;
				wMixType |= FULL;
			}
			else
			{
				/* mix at half sample rate */
	 	 		dwLoopCountG = MIN(((pEffect->dwSmpLen)-dwSmpIndx),dwMixBufFramesG>>1);
				dwLoopLen = dwLoopCountG<<1;
				wMixType |= HALF;
			}
		}
		else
		{
			/*
			 * Effect is shifted pitch 
			 */
			wMixType |= PITCH;

			wIncIndxG = pEffect->wIncIndx;
			wIncrementG = pEffect->wIncrement;
	
			if(pEffect->wSrate == wSampleRateG)
			{
				/* mix at full sample rate */
				dwLoopCountG = MIN((((pEffect->dwSmpLen)-dwSmpIndx)<<8)/wIncrementG,dwMixBufFramesG);
			  dwLoopLen = dwLoopCountG;
				wMixType |= FULL;
			}
			else
			{
				/* mix at half sample rate */
				dwLoopCountG = MIN((((pEffect->dwSmpLen)-dwSmpIndx)<<8)/wIncrementG,dwMixBufFramesG>>1);
				dwLoopLen = dwLoopCountG<<1;
				wMixType |= HALF;
			}
		}

		/*
		 * Call mixing routine
		 */
		if(dwLoopCountG>0)
		{
			pMixFunctionTableG[wMixType]();

			/* increment index to position in sample */
			dwSmpIndx = (uint32)(pbSrcPtrG-pbSmpPtr);
		}

		/*
		 * Loop or zero pad
		 */
		if(dwLoopLen < dwMixBufFramesG)
		{
			bComplete = 1;
			pEffect->wFlags |= CALLBACK; 	/* set callback flag */

			if(pEffect->wFlags & LOOP)
			{
				/*** Loop Sample from start ***/
				
				dwSmpIndx = 0;
				if(pEffect->wIncrement == 256)
				{
					wMixType |= NORMAL;

					/*** Normal Pitch ***/
		     	if(pEffect->wSrate == wSampleRateG)
		     	{
		     		/* mix at full sample rate */
					  dwLoopCountG = dwMixBufFramesG-dwLoopLen;
					  dwLoopLen = dwLoopCountG;
		     		wMixType |= FULL;
		     	}
		     	else
		     	{
		     		/* mix at half sample rate */
					  dwLoopCountG = (dwMixBufFramesG-dwLoopLen)/2;
						dwLoopLen = dwLoopCountG<<1;
		     		wMixType |= HALF;
		     	}
				}
				else
				{
					wMixType |= PITCH;

					/*** Mix effect at different pitch ***/
    			if(pEffect->wSrate == wSampleRateG)
    			{
    				/* mix at full sample rate */
					  dwLoopCountG = dwMixBufFramesG-dwLoopLen;
						dwLoopLen = dwLoopCountG;
    				wMixType |= FULL;
    			}
    			else
    			{
    				/* mix at half sample rate */
					  dwLoopCountG = (dwMixBufFramesG-dwLoopLen)/2;
						dwLoopLen = dwLoopCountG<<1;
    				wMixType |= HALF;
    			}
				}					  

    		/*
    		 * Call mixing routine
    		 */
    		if(dwLoopCountG>0)
				{
					pbSrcPtrG = pbSmpPtr;    /* point to start of sample */
    			pMixFunctionTableG[wMixType]();
				
					/* increment index to position in sample */
					dwSmpIndx = (uint32)(pbSrcPtrG-pbSmpPtr);
				}
			}
			else if(wCount == 1)
			{
				/** Zero pad remainder of buffer **/
				dwLoopCountG = dwMixBufFramesG-dwLoopLen;
				if(dwLoopCountG>0)
				{
					MonoZeroPad();
				}
			}

			if((bComplete == 1) &&
				!(pEffect->wFlags & LOOP))
			{
				/* Effect finished so reset active bit */
				pEffect->wFlags &= ~ACTIVE;
				wUpdate++; 
			}
		}
		pEffect->dwSmpIndx = dwSmpIndx;
		pEffect->wIncIndx = wIncIndxG;
		pEffect = pEffect->pNext;

		/* reset mix type */
		wMixType = MIX;
	}

	*ppItem = pEffect;  
	return wUpdate;
}


uint16 PhaseMix(struct playListStruct **ppItem)
{
	extern uint32 dwMixBufFramesG;
	extern int32 *pdwMixBufG;
	extern int16 *pwDeCmprsG;
	extern uint16 wMaxActiveG;

	struct playListStruct *pEffect;
	uint32 dwLoopLen;
	uint8 *pbSmpPtr;
	uint32 dwSmpIndx;
	uint16 wUpdate;
	uint16 wCount;
	uint16 wVol,wPan;
	uint16 wMixType;
	uint8 bComplete;
	uint8 bSide;

	wCount = 0;
	wUpdate = 0;
	pEffect = *ppItem;

	/* First time round, copy samples to buffer */
	wMixType = COPY;

	while(pEffect && (wCount < wMaxActiveG))
	{
		wCount++;
		bComplete = 0;

		wVol = (uint16)pEffect->bVolume;
		wPan = (uint16)pEffect->bPan;

		/*
		 * Determine lead sample
		 */
		if(wPan <= 15)
		{
			pbLeadG[0] = 0;	 /* left sample leads */
			pbLeadG[1] = 1;
			wPan = 15-wPan;
		}
		else
		{
			pbLeadG[0] = 1;	 /* right sample leads */
			pbLeadG[1] = 0;
			wPan -= 15;
		}

		for(bSide=0;bSide<2;bSide++)
		{
			/*
			 * Mix trailing sample
			 */
			pbSmpPtr = pEffect->pbSmpPtr;
			dwSmpIndx = pEffect->dwSmpIndx;

			if(bSide==0)
			{
				/* Advance leading sample */
				if(pEffect->wSrate == 22050)
					dwSmpIndx += pPhase22G[wPan].bDelay;
				else
					dwSmpIndx += pPhase11G[wPan].bDelay;

				if(dwSmpIndx > pEffect->dwSmpLen)
				{
					dwSmpIndx -= pEffect->dwSmpLen;
					dwDebug++;
				}

				wOffsetG = wVol << 8;
			}						  
			else
			{
				/* Fade trailing sample */
				wOffsetG = *(pbPanG+ ((uint16)pPhase22G[wPan].bVol<<5) + wVol) << 8;
			}
			
			pdwDstPtrG = pdwMixBufG+pbLeadG[bSide];   /* start of mix buffer */
			pbSrcPtrG = pbSmpPtr+dwSmpIndx;   /* current place in sample */

			bLeftOffG = wOffsetG >> 8;

			if(pEffect->wIncrement == 256)
			{
				/*
				 * Mix effect at normal pitch 
				 */
				wMixType |= NORMAL;
			
				if(pEffect->wSrate == wSampleRateG)
				{
					/* mix at full sample rate */
					dwLoopCountG = MIN(((pEffect->dwSmpLen)-dwSmpIndx),dwMixBufFramesG);
					dwLoopLen = dwLoopCountG;
					wMixType |= FULL;
				}
				else
				{
					/* mix at half sample rate */
					dwLoopCountG = MIN(((pEffect->dwSmpLen)-dwSmpIndx),dwMixBufFramesG>>1);
					dwLoopLen = dwLoopCountG<<1;
					wMixType |= HALF;
				}
			}
			else
			{
				/*
				 * Effect is shifted pitch 
				 */
				wMixType |= PITCH;

				wIncIndxG = pEffect->wIncIndx;
				wIncrementG = pEffect->wIncrement;
		
				if(pEffect->wSrate == wSampleRateG)
				{
					/* mix at full sample rate */
					dwLoopCountG = MIN((((pEffect->dwSmpLen)-dwSmpIndx)<<8)/wIncrementG,dwMixBufFramesG);
				  dwLoopLen = dwLoopCountG;
					wMixType |= FULL;
				}
				else
				{
					/* mix at half sample rate */
					dwLoopCountG = MIN((((pEffect->dwSmpLen)-dwSmpIndx)<<8)/wIncrementG,dwMixBufFramesG>>1);
					dwLoopLen = dwLoopCountG<<1;
					wMixType |= HALF;
				}
			}

			/*
			 * Set DOLBY flags 
			 */		
			if(pEffect->wFlags & SURROUND)
			{
				wMixType |= DOLBY;
				bSide++;		/* Do Dolby stuff once only */
			}

			/*
			 * Call mixing routine
			 */
			if(dwLoopCountG>0)
			{
				pMixFunctionTableG[wMixType]();

				/* increment index to position in sample */
				dwSmpIndx = (uint32)(pbSrcPtrG-pbSmpPtr);
			}
																							  
			if(dwLoopLen < dwMixBufFramesG)
			{
				if(bSide==1)
				{
					bComplete = 1;
					pEffect->wFlags |= CALLBACK;	/* set callback flag */
				}

				/*
				 * Either loop or zero pad buffer 
				 */
				if(pEffect->wFlags & LOOP)
				{
					/*** Loop sample from start ***/
					dwSmpIndx=0;

					if(pEffect->wSrate == wSampleRateG)
					{
						/* mix at full sample rate */
						dwLoopCountG = dwMixBufFramesG-dwLoopLen;
					}
					else
					{
						/* mix at half sample rate */
						dwLoopCountG = (dwMixBufFramesG-dwLoopLen)/2;
					}

					/*
					 * Call mixing routine
					 */
					if(dwLoopCountG>0) 
					{
						if(bSide==0)
						{
							if(pEffect->wSrate == 22050)
								dwSmpIndx += pPhase22G[wPan].bDelay;	/* advance sample */
							else
								dwSmpIndx += pPhase11G[wPan].bDelay;	/* advance sample */
						}
						
						pbSrcPtrG = pbSmpPtr+dwSmpIndx;		/* point to start of sample */
						pMixFunctionTableG[wMixType]();

						/* increment index to position in sample */
						dwSmpIndx = (uint32)(pbSrcPtrG-pbSmpPtr);
					}
						 	
				}
				else if(wCount == 1)
				{
					/*** Zero pad remainder of buffer ***/
					dwLoopCountG = dwMixBufFramesG-dwLoopLen;
					if(dwLoopCountG>0)
					{
						MonoZeroPad();
					}
				}
 	 	 	}
		}

		if((bComplete == 1) &&
		 !(pEffect->wFlags & LOOP))
		{
			/* Effect finished so reset active bit */
			pEffect->wFlags &= ~ACTIVE;
			wUpdate++; 
		}

		pEffect->dwSmpIndx = dwSmpIndx;
		pEffect->wIncIndx = wIncIndxG;
		pEffect = pEffect->pNext;
		wMixType = MIX;
	}
	*ppItem = pEffect;  
	return wUpdate;							 
}

uint16 QStereoMix(struct playListStruct **ppItem)
{
	extern uint8 *pbPanG;
	extern uint32 dwMixBufFramesG;
	extern int32 *pdwMixBufG;
	extern int16 *pwDeCmprsG;
	extern uint16 wMaxActiveG;

	struct playListStruct *pEffect;
	uint32 dwLoopLen;
	uint32 dwLoopCountStore;
	uint8 *pbSmpPtr;
	uint32 dwSmpIndx;
	uint16 wUpdate;
	uint16 wCount;
	uint16 wVol,wLeftPan,wRightPan;
	uint8 bComplete;
	uint16 wMixType;
	uint8 bLeftOffStore;
	uint8 bRightOffStore;
	uint8 bLeftOffImg;
	uint8 bRightOffImg;
	uint32 dwImgIndx;
	int32 *pdwMixBufStore;

	wCount = 0;
	wUpdate = 0;
	pEffect = *ppItem;

	/* First time round, copy samples to buffer */
	wMixType = COPY;

	while(pEffect && (wCount < wMaxActiveG))
	{
		wCount++;
		bComplete = 0;

		pbSmpPtr = pEffect->pbSmpPtr;
		dwSmpIndx = pEffect->dwSmpIndx;
		dwImgIndx = dwSmpIndx;

		/*
		 * Calculate decompression offsets from
		 * volume and pan
		 */
		wVol = (uint16)pEffect->bVolume;
		wLeftPan = (uint16)pEffect->bPan;
		wRightPan = wLeftPan;

		if(pEffect->wFlags & SURROUND)
		{
			/*
			 * Ignore panning information, place effect 
			 * on surround channel
			 */
			bLeftOffG = *(pbPanG + wVol);
			bRightOffG = *(pbPanG + wVol);
		}
		else if(pEffect->wFlags & QSND_IMAGE)
		{
			/* 
			 * QSound offsets
			 */
			if(wLeftPan<16)
			{
				wLeftPan<<=7;
				wVol<<=2;
				bLeftOffG = *(pQSndLookupG + wLeftPan + wVol);
				bRightOffG = *(pQSndLookupG + wLeftPan + wVol + 1);
				bLeftOffImg = *(pQSndLookupG + wLeftPan + wVol + 2);
				bRightOffImg = *(pQSndLookupG + wLeftPan + wVol + 3);
			}
			else
			{
				wLeftPan -= 15;
				wLeftPan = 15-wLeftPan;
				wLeftPan <<= 7;
				wVol<<=2;
				bLeftOffG = *(pQSndLookupG + wLeftPan + wVol+1);
				bRightOffG = *(pQSndLookupG + wLeftPan + wVol);
				bLeftOffImg = *(pQSndLookupG + wLeftPan + wVol + 3);
				bRightOffImg = *(pQSndLookupG + wLeftPan + wVol + 2);
			}


		}
		else
		{
			/*
			 * Normal Stereo Offsets
			 */
			if(wLeftPan < 15)
			{
				wLeftPan = 0;
				wRightPan = 15-wRightPan;
			}
			else if(wLeftPan > 15)
			{
				wRightPan = 0;
				wLeftPan -= 15;
			}
			else
			{
				wLeftPan = 0;
				wRightPan = 0;
			}

			wLeftPan <<= 5;             /* multiply by 32 */
			wRightPan <<= 5;             

			bLeftOffG = *(pbPanG + wLeftPan + wVol);
			bRightOffG = *(pbPanG + wRightPan + wVol);
		}

/****
		dwDebug1G = (uint32)bLeftOffG;
		dwDebug2G = (uint32)bRightOffG;
		dwDebug3G = (uint32)bLeftOffImg;
		dwDebug4G = (uint32)bRightOffImg;
****/

		bLeftOffStore = bLeftOffG;
		bRightOffStore = bRightOffG;

		pdwDstPtrG = pdwMixBufG;         /* start of mix buffer */
		pbSrcPtrG = pbSmpPtr+dwSmpIndx;

		if(pEffect->wIncrement == 256)
		{
			/*** Mix effect at normal pitch ***/
			wMixType |= NORMAL;

			/*
			 * Calculate loop count
			 */
			if(pEffect->wSrate == wSampleRateG)
			{
				/* mix at full sample rate */
				dwLoopCountG = MIN(((pEffect->dwSmpLen)-dwSmpIndx),dwMixBufFramesG);
				dwLoopCountStore = dwLoopCountG;
				dwLoopLen = dwLoopCountG;
				wMixType |= FULL;
			}
			else
			{
				/* mix at half sample rate */
	 	 		dwLoopCountG = MIN(((pEffect->dwSmpLen)-dwSmpIndx),dwMixBufFramesG>>1);
				dwLoopCountStore = dwLoopCountG;
				dwLoopLen = dwLoopCountG<<1;
				wMixType |= HALF;
			}
		}
		else
		{
			/*
			 * Effect is shifted pitch
			 */
			wMixType |= PITCH;

			wIncIndxG = pEffect->wIncIndx;
			wIncrementG = pEffect->wIncrement;

			if(pEffect->wSrate == wSampleRateG)
			{
				/* mix at full sample rate */
				dwLoopCountG = MIN((((pEffect->dwSmpLen)-dwSmpIndx)<<8)/wIncrementG,dwMixBufFramesG);
				dwLoopCountStore = dwLoopCountG;
			  dwLoopLen = dwLoopCountG;
				wMixType |= FULL;
			}
			else
			{
				/* mix at half sample rate */
				dwLoopCountG = MIN((((pEffect->dwSmpLen)-dwSmpIndx)<<8)/wIncrementG,dwMixBufFramesG>>1);
				dwLoopCountStore = dwLoopCountG;
				dwLoopLen = dwLoopCountG<<1;
				wMixType |= HALF;
			}
		}

		/*
		 * Set DOLBY flags 
		 */		
		if(pEffect->wFlags & SURROUND)
		{
			wMixType |= DOLBY;
		}

		/*
		 * Call mixing routine
		 */
		if(dwLoopCountG>0)
		{
			/*
			 * Mix Source file 
			 */
			if( (pEffect->wFlags & QSND_IMAGE) &&
				 !(pEffect->wFlags & SURROUND))
			{
				/*
				 * Attempt to find the most efficient way of mixing
				 * Basically, if the channel has zero volume don't
				 * mix it unless it's copying (not mixing) zeros in to the buffer
				 */
				if( ((bLeftOffG>0) && (bRightOffG>0))  ||
						(!(wMixType & COPY) && (bLeftOffG>0) && (bRightOffImg==0)) ||
						(!(wMixType & COPY) && (bRightOffG>0) && (bLeftOffImg==0)) ||
						(!(wMixType & COPY) && (bLeftOffG==0) && (bRightOffG==0)) )
				{
					/* play on both channels */
					pMixFunctionTableG[wMixType]();
				}
				else
				{
					if(bLeftOffG>0)
					{
						/* play on left channel only */
						wOffsetG = bLeftOffG << 8;
						pMixFunctionTableG[wMixType | ONE_CHNL]();
					}
					else
					{
						/* play on right channel only */
						wOffsetG = bRightOffG << 8;
						pdwDstPtrG++;
						pMixFunctionTableG[wMixType | ONE_CHNL]();
						pdwDstPtrG--;
					}
				}
			}
			else
			{
				/* normal stereo call */
				pMixFunctionTableG[wMixType]();
			}

			/* increment index to position in sample */
			dwSmpIndx = (uint32)(pbSrcPtrG-pbSmpPtr);

			/** Mix Image file **/
			if( (pEffect->wFlags & QSND_IMAGE) &&
				 !(pEffect->wFlags & SURROUND))
			{
				/* 
				 * Reset variables
				 */
				bLeftOffG = bLeftOffImg;
				bRightOffG = bRightOffImg;

				wIncIndxG = pEffect->wIncIndx;
				wIncrementG = pEffect->wIncrement;

				dwLoopCountG = dwLoopCountStore;
	
				/* call mix function */
				if((bLeftOffG != 0) || (bRightOffG != 0))
				{
					pbSrcPtrG = (pEffect->pbImgPtr)+dwImgIndx;
					if(bLeftOffG == 0)
					{
						/* play image on right channel only */
						wOffsetG = bRightOffG << 8;
						pdwDstPtrG = pdwMixBufG;
						pdwDstPtrG++;
						pMixFunctionTableG[wMixType | ONE_CHNL]();
						pdwDstPtrG--;
					}
					if(bRightOffG == 0)
					{
						/* play image on left channel only */
						wOffsetG = bLeftOffG << 8;
						pdwDstPtrG = pdwMixBufG;
						pMixFunctionTableG[wMixType | ONE_CHNL]();
					}
				}
			}
		}

		/* store current position in mix buffer */
		pdwMixBufStore = pdwDstPtrG;

		if(dwLoopLen < dwMixBufFramesG)
		{
			bComplete = 1;
			pEffect->wFlags |= CALLBACK; 	/* set callback flag */

			if(pEffect->wFlags & LOOP)
			{
				/*** Loop Sample from start ***/
				dwSmpIndx = 0;

				if(pEffect->wSrate == wSampleRateG)
				{
					dwLoopCountG = dwMixBufFramesG-dwLoopLen;
					dwLoopCountStore = dwLoopCountG;
					dwLoopLen = dwLoopCountG;
				}
				else
				{
					dwLoopCountG = (dwMixBufFramesG-dwLoopLen)/2;
					dwLoopCountStore = dwLoopCountG;
					dwLoopLen = dwLoopCountG<<1;
				}

    		/*
    		 * Call mixing routine
    		 */
    		if(dwLoopCountG>0)
				{
					bLeftOffG = bLeftOffStore;
					bRightOffG = bRightOffStore;
					
					pbSrcPtrG = pEffect->pbSmpPtr;    /* point to start of sample */

					/*
					 * Mix Source file 
					 */
					if( (pEffect->wFlags & QSND_IMAGE) &&
						 !(pEffect->wFlags & SURROUND))
					{
						/*
						 * Attempt to find the most efficient way of mixing
						 * Basically, if the channel has zero volume don't
						 * mix it unless it's copying (not mixing) zeros in to the buffer
						 */
						if( ((bLeftOffG>0) && (bRightOffG>0))  ||
								(!(wMixType & COPY) && (bLeftOffG>0) && (bRightOffImg==0)) ||
								(!(wMixType & COPY) && (bRightOffG>0) && (bLeftOffImg==0)) ||
								(!(wMixType & COPY) && (bLeftOffG==0) && (bRightOffG==0)) )
						{
							/* play on both channels */
							pMixFunctionTableG[wMixType]();
						}
						else
						{
							if(bLeftOffG>0)
							{
								/* play on left channel only */
								wOffsetG = bLeftOffG << 8;
								pMixFunctionTableG[wMixType | ONE_CHNL]();
							}
							else
							{
								/* play on right channel only */
								wOffsetG = bRightOffG << 8;
								pdwDstPtrG++;
								pMixFunctionTableG[wMixType | ONE_CHNL]();
								pdwDstPtrG--;
							}
						}
					}
					else
					{
						/* normal stereo call */
						pMixFunctionTableG[wMixType]();
					}

					/* increment index to position in sample */
					dwSmpIndx = (uint32)(pbSrcPtrG-pbSmpPtr);

					if((pEffect->wFlags & QSND_IMAGE) &&
						!(pEffect->wFlags & SURROUND))
					{
						/* 
						 * Reset variables
						 */
						bLeftOffG = bLeftOffImg;		 
						bRightOffG = bRightOffImg;

						wIncIndxG = pEffect->wIncIndx;
						wIncrementG = pEffect->wIncrement;
			
						dwLoopCountG = dwLoopCountStore;

						/** call mix function */
						if((bLeftOffG != 0) || (bRightOffG != 0))
						{
							pbSrcPtrG = pEffect->pbImgPtr;
							if(bLeftOffG == 0)
							{
								/* play image on right channel only */
								wOffsetG = bRightOffG << 8;
								pdwDstPtrG = pdwMixBufStore;
								pdwDstPtrG++;
								pMixFunctionTableG[wMixType | ONE_CHNL| MIX]();
								pdwDstPtrG--;
							}
							if(bRightOffG == 0)
							{
								/* play image on left channel only */
								wOffsetG = bLeftOffG << 8;
								pdwDstPtrG = pdwMixBufStore;
								pMixFunctionTableG[wMixType | ONE_CHNL| MIX]();
							}
						}
					}
				}
			}
			else if(wCount == 1)
			{
				/** Zero pad remainder of buffer **/
				dwLoopCountG = dwMixBufFramesG-dwLoopLen;
				if(dwLoopCountG>0)
				{
					StereoZeroPad();
				}
			}

			if((bComplete == 1) &&
				!(pEffect->wFlags & LOOP))
			{
				/* Effect finished so reset active bit */
				pEffect->wFlags &= ~ACTIVE;
				wUpdate++; 
			}
		}
		pEffect->dwSmpIndx = dwSmpIndx;
		pEffect->wIncIndx = wIncIndxG;
		pEffect = pEffect->pNext;

		/* reset mix type */
		wMixType = MIX;
	}

	*ppItem = pEffect;  
	return wUpdate;
}

/**********************************************
*
* uint16 GenDeCmprs(void)
*
* returns: error flag. 1 = error, not enough memory
*
***********************************************
*
* Generates a sample scaling and decompression 
*	lookup table.
*
***********************************************
*
* Called by: SfxInit()
*
* Depends on:	nothing
*
**********************************************/
uint16 GenDeCmprs(void)
{
	extern int16 *pwDeCmprsG;

	float fLog;
	float fMaxSamp;
	float fInput;
	float fExpo;
	float fInterm;
	uint16 wIndx;
	uint16 wVol;
	uint16 wSamp;
	uint16 pwMaxVols[VOLUME_LEVELS];

	/*
	 * Calculate maxmimum sample value for each 
	 * volume level, based on 1.5dB steps
	 */
#if 0
	float fDB;
	fDB = -1.5*(VOLUME_LEVELS-1); 
	for(wIndx=0;wIndx<VOLUME_LEVELS;wIndx++)
	{
		pwMaxVols[wIndx] = (uint16)(pow(3,(fDB/20.0))*32767.0);

		fDB+=1.5;		/* 1.5dB steps */
	}
#else
	/*
	 * linear volume scale
	 */
	for(wIndx=0;wIndx<VOLUME_LEVELS;wIndx++)
	{
		pwMaxVols[wIndx] = (uint16) (((uint32)wIndx*32767.0)/(VOLUME_LEVELS-1));
	}
#endif

	pwDeCmprsG = (int16 *)malloc(VOLUME_LEVELS*256*sizeof(int16));
	if(pwDeCmprsG == NULL)
	{
		return 1;		/* failed to allocate enough memory */
	}

	fLog = (float)log(88.6);			/* 1+87.6, u-Law decompression ratio */

	wIndx=0;
	for(wVol=0;wVol<VOLUME_LEVELS;wVol++)
	{
		/** max sample for volume range **/
		fMaxSamp = (float)(pwMaxVols[wVol]);

		for(wSamp=0;wSamp<256;wSamp++)
		{
			fInput = ((float)wSamp-128)/128;

			fExpo = (fInput*fLog)/(float)(SIGN(fInput));

			fInterm = (exp(fExpo)-1)/87.6;

			*(pwDeCmprsG+wIndx) = SIGN(fInput)*(int16)(fMaxSamp * fInterm);

			wIndx++;
		}
	}
	
	return 0;
}

/**********************************************
*
* uint16 GenScale8(void)
*
* returns: error flag. 1 = error, not enough memory
*
***********************************************
*
* Generates an 8bit sample scaling lookup table.
*
***********************************************
*
* Called by: SfxInit()
*
* Depends on:	nothing
*
**********************************************/
uint16 GenScale8(void)
{
	extern int16 *pwDeCmprsG;					 
	
	float fMaxSamp;
	float fStep,fSample;
	uint16 wIndx;
	uint16 wVol;
	uint16 wSamp;
	float fDB;
	uint16 pwMaxVols[VOLUME_LEVELS];

	/*
	 * Calculate maxmimum sample value for each 
	 * volume level, based on 1.5dB steps
	 */
	fDB = -1.5*(VOLUME_LEVELS-1); 
	for(wIndx=0;wIndx<VOLUME_LEVELS;wIndx++)
	{
		pwMaxVols[wIndx] = (uint16)(pow(4,(fDB/20.0))*127.0);

		fDB+=1.5;		/* 1.5dB steps */
	}

	pwDeCmprsG = (int16 *)malloc(VOLUME_LEVELS*256*sizeof(int16));
	if(pwDeCmprsG == NULL)
	{
		return 1;		/* failed to allocate enough memory */
	}

	wIndx=0;
	for(wVol=0;wVol<VOLUME_LEVELS;wVol++)
	{
		/** max sample for volume range **/
		fMaxSamp = (float)(pwMaxVols[wVol]);
		fStep = ((2*fMaxSamp)+1)/256;
		fSample = -(fMaxSamp+1);

		for(wSamp=0;wSamp<256;wSamp++)
		{
			*(pwDeCmprsG+wIndx) = (int16)fSample;

			fSample += fStep;

			wIndx++;
		}
	}

	return 0;
}	

/**********************************************
*
* uint16 GenPanLookup(void)
*
* returns: error flag. 1 = error, not enough memory
*
***********************************************
*
* Generates sample panning lookup table.
*	Used to give offsets to the decompression table 
* for a given pan and volume.
*
***********************************************
*
* Called by: SfxInit()
*
* Depends on:	nothing
*
**********************************************/
uint16 GenPanLookup(void)
{
	extern uint8 *pbPanG;

	uint16 wPan,wVol;
	float fOffset;
	float pfSteps[16];
	float fStep;

	/*
	 * Allocate memory for lookup table
	 */
	pbPanG = (uint8*)malloc(VOLUME_LEVELS*16*sizeof(uint8));
	if(pbPanG == NULL)
	{
		return 1;
	}

	/*
	 * Generate panning steps
	 */
	for(wPan=0;wPan<16;wPan++)
	{
		pfSteps[wPan] = (16.0-(float)wPan)/16;
	}

 	/*
   * Generate lookup table
	 */
 	for(wPan=0;wPan<16;wPan++)
 	{
		fStep = pfSteps[wPan];
		fOffset=0;
		for(wVol=0;wVol<VOLUME_LEVELS;wVol++)
		{
			*(pbPanG + (wPan<<5) + wVol) = (uint8)fOffset;
			fOffset += fStep;
		}
	}

	return 0;
}

static uint16 GenPhaseLookup(void)
{
	uint8 pbDelay22[16] = {0,1,2,3,4,5,6,7,9,11,13,15,17,19,22,25};
	uint8 pbDelay11[16] = {0,1,2,3,4,5,6,7,8, 9,10,11,12,13,14,15};
	uint8 pbPan[16] = {0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7};
	uint16 wIndx;

	/*
	 * Allocate memory for phase tables
	 */
	pPhase22G = (struct phaseStruct *)malloc(16*sizeof(struct phaseStruct));	
	pPhase11G = (struct phaseStruct *)malloc(16*sizeof(struct phaseStruct));	
	if((pPhase22G == NULL) || (pPhase11G == NULL))
	{
		return 1;
	}
	
	for(wIndx=0;wIndx<16;wIndx++)
	{
		pPhase22G[wIndx].bDelay = pbDelay22[wIndx];
		pPhase22G[wIndx].bVol = pbPan[wIndx];
	}
	for(wIndx=0;wIndx<16;wIndx++)
	{
		pPhase11G[wIndx].bDelay = pbDelay11[wIndx];
		pPhase11G[wIndx].bVol = pbPan[wIndx];
	}

	return 0;
}


static uint16 GenQSoundLookup(void)
{
	uint16 wVolume;
	uint16 wPan;

	struct QFloatStruct{
		float fOrgL;
		float fOrgR;
		float fImgL;
		float fImgR;
	};

	struct QFloatStruct pfQTable[16] = 
	{
		{1.0000, 0.0000, 0.0000, 1.0000},
		{1.0000, 0.0000, 0.0000, 0.8981},
		{1.0000, 0.0000, 0.0000, 0.7466},
		{1.0000, 0.0000, 0.0000, 0.6235},
		{1.0000, 0.0000, 0.0000, 0.4649},
		{1.0000, 0.0000, 0.0000, 0.2614},
		{1.0000, 0.0000, 0.0000, 0.1416},
		{1.0000, 0.0999, 0.0000, 0.0310},
		{0.9500, 0.2000, 0.0000, 0.0109},
		{0.9500, 0.2999, 0.0000, 0.0052},
		{0.9000, 0.4000, 0.0000, 0.0026},
		{0.9000, 0.5000, 0.0000, 0.0013},
		{0.8499, 0.5499, 0.0000, 0.0006},
		{0.8499, 0.5999, 0.0000, 0.0003},
		{0.7999, 0.6500, 0.0000, 0.0000},
		{0.7000, 0.7000, 0.0000, 0.0000}
	};

	/*
	 * Allocate memory for lookup table
	 */
	pQSndLookupG = (uint8 *)malloc(16*VOLUME_LEVELS*4*sizeof(uint8));
	if(pQSndLookupG == NULL)
	{
		return 1;
	}

	/*
	 * Generate table
	 */
	for(wPan=0;wPan<16;wPan++)
	{
		for(wVolume=0;wVolume<VOLUME_LEVELS;wVolume++)
		{						
			*(pQSndLookupG + (wPan<<7) + (wVolume<<2)) 
						= (uint8)(pfQTable[wPan].fOrgL*(float)wVolume);
			*(pQSndLookupG + (wPan<<7) + (wVolume<<2) + 1) 
						= (uint8)(pfQTable[wPan].fOrgR*(float)wVolume);
			*(pQSndLookupG + (wPan<<7) + (wVolume<<2) + 2) 
						= (uint8)(pfQTable[wPan].fImgL*(float)wVolume);
			*(pQSndLookupG + (wPan<<7) + (wVolume<<2) + 3) 
						= (uint8)(pfQTable[wPan].fImgR*(float)wVolume);
		}
	}

	return 0;
}
	



/*********************************************************
 * Assembler versions of following routines in
 * sfxmixlo.asm	
 *********************************************************/

#ifdef CLEAR_MIX

void ClearMixBuf(void)
{
	extern int32 *pdwMixBufG;
	extern uint32 dwMixBufSamplesG;

	int32 *pdwBufPtr;
	uint32 dwLoopCount;

	pdwBufPtr = pdwMixBufG;
	dwLoopCount = dwMixBufSamplesG/4;

	do{
		*pdwBufPtr = 0;
		pdwBufPtr++;
		*pdwBufPtr = 0;
		pdwBufPtr++;
		*pdwBufPtr = 0;
		pdwBufPtr++;
		*pdwBufPtr = 0;
		pdwBufPtr++;
		dwLoopCount--;
	}while(dwLoopCount);
}

#endif 

#ifdef COPY_DMA

void ClearDmaBuf(void)
{
	extern uint16 	wMaxBitsG;
	extern void 	*ppvBufferG[2];
	extern volatile uint16 wBufferFlagG;
	extern uint32 	dwDmaBlockSizeG;

	uint32 			dwIndx;
	uint32 			*pdwBuf;
	uint32	 		dwZero;

	if(wMaxBitsG == 8)
		dwZero = 0x80808080;
	else
		dwZero = 0;

	/* clear 4 bytes at a time */
	pdwBuf = (uint32 *)ppvBufferG[wBufferFlagG];

	for(dwIndx=0; dwIndx<dwDmaBlockSizeG >> 2; dwIndx++)
	{
		*(pdwBuf + dwIndx) = dwZero;
	}

}

/******************************************************
 * The following routines copy the mixed data into the
 * DMA buffer in DOS memory
 *****************************************************/

void Mix2Dma16Mono(void)
{
	extern void *ppvBufferG[2];
	extern int32 *pdwMixBufG;
	extern uint32 dwMixBufSamplesG;
	extern volatile uint16 wBufferFlagG;

	uint32 dwIndx;
	uint32 dwDmaIndx;
	int16 *pwBufPtr;

	pwBufPtr = (int16 *)ppvBufferG[wBufferFlagG];            
	
	for(dwIndx=0,dwDmaIndx=0;dwIndx<dwMixBufSamplesG;dwIndx+=2,dwDmaIndx++)
	{
		*(pwBufPtr + dwDmaIndx) = *(pdwMixBufG + dwIndx);

		if(*(pdwMixBufG + dwIndx) > 32767)
			*(pwBufPtr + dwDmaIndx) = 32767;

		if(*(pdwMixBufG + dwIndx) < -32768)                                                                                                                                      
			*(pwBufPtr + dwDmaIndx) = -32768;
	}                                                                                                         
}                                                        

void Mix2Dma16Stereo(void)
{
	extern void *ppvBufferG[2];
	extern int32 *pdwMixBufG;
	extern volatile uint16 wBufferFlagG;
	extern uint32 dwMixBufFramesG;

	uint32 dwLoopCountG;
	int16 *pwDstPtr;
	int32 *pdwSrcPtr;

	pwDstPtr = (int16 *)ppvBufferG[wBufferFlagG];
	pdwSrcPtr = pdwMixBufG;

	dwLoopCountG = dwMixBufFramesG;
						  
	do{
		/*
		 * Get and clip left sample 
		 */
		*pwDstPtr = (int16)*pdwSrcPtr;

		if((uint32)(*pdwSrcPtr+32768) > 65536)
		{
			if(*pdwSrcPtr>32767)
				*pwDstPtr = 32767;
			else
				*pwDstPtr = -32768;
		}

		pwDstPtr++;
		pdwSrcPtr++;

		/*
		 * Get and clip right sample
		 */
		*pwDstPtr = (int16)*pdwSrcPtr;

		if((uint32)(*pdwSrcPtr+32768)   > 65536)
		{
			if(*pdwSrcPtr>32767)
				*pwDstPtr = 32767;
			else
				*pwDstPtr = -32768;
		}

		pwDstPtr++;
		pdwSrcPtr++;

		dwLoopCountG--;
	}while(dwLoopCountG);
}

void Mix2Dma8Mono(void)
{
	extern void *ppvBufferG[2];
	extern int32 *pdwMixBufG;
	extern uint32 dwMixBufFramesG;
	extern volatile uint16 wBufferFlagG;

	uint32 dwLoopCountG;
	uint8 *pbDstPtr;
	int32 *pdwSrcPtr;
	int32 dwSample;
	
	pbDstPtr = (uint8 *)ppvBufferG[wBufferFlagG];

	dwLoopCountG = dwMixBufFramesG;

	pdwSrcPtr = pdwMixBufG;

	do{
		/*
		 * Get and clip sample
		 */
		dwSample = *pdwSrcPtr;

		if((uint32)(dwSample+128) > 256)
		{
			if(dwSample>127)		 
				dwSample=127;
			else
				dwSample=-128;
		}
		pdwSrcPtr+=2;

		*pbDstPtr = (uint8)(dwSample+128);
		pbDstPtr++;

		dwLoopCountG--;
	}while(dwLoopCountG);
}

void Mix2Dma8Stereo(void)
{
	extern void *ppvBufferG[2];
	extern int32 *pdwMixBufG;
	extern uint32 dwMixBufFramesG;
	extern volatile uint16 wBufferFlagG;

	uint32 dwLoopCountG;
	int32 *pdwSrcPtr;
	uint8 *pbDstPtr;
	int32 dwSample;

	pbDstPtr = (uint8 *)ppvBufferG[wBufferFlagG];

	dwLoopCountG = dwMixBufFramesG;

	pdwSrcPtr = pdwMixBufG;

	do{                             

		/*
		 * Get and clip left sample
		 */
		dwSample = *pdwSrcPtr;
		if((uint32)(dwSample+128) > 256)
		{
			if(dwSample>127)
				dwSample=127;
			else
				dwSample=-128;
		}
		pdwSrcPtr++;

		*pbDstPtr = (uint8)(dwSample+128);
		pbDstPtr++;
	
		/*
		 * Get and clip right sample
		 */
		dwSample = *pdwSrcPtr;
		if((uint32)(dwSample+128) > 256)
		{
			if(dwSample>127)
				dwSample=127;
			else
				dwSample=-128;
		}
		pdwSrcPtr++;

		*pbDstPtr = (uint8)(dwSample+128);
		pbDstPtr++;

		dwLoopCountG--;
	}while(dwLoopCountG);
}    

#endif


/********************************************************
 * Mono and Phased stereo mixing routines 
 *******************************************************/
#ifdef MONO_MIX

void MonoCopyNormal(void)
{

  do{
  	wOffsetG &= 0xFF00;
  	wOffsetG |= *pbSrcPtrG;

  	*pdwDstPtrG = *(pwDeCmprsG+wOffsetG);
  	pdwDstPtrG+=2;  
  	pbSrcPtrG++;

  	dwLoopCountG--;
  }while(dwLoopCountG);
}

void MonoCopyNormalHalf(void)
{
	int32 dwThisSample;
	int32 dwNextSample;
	int32 dwInterpol;

	wOffsetG &= 0xFF00;
	wOffsetG |= *pbSrcPtrG;

	dwThisSample = *(pwDeCmprsG+wOffsetG);
	pbSrcPtrG++;

	dwLoopCountG--;

	do{
		wOffsetG &= 0xFF00;
		wOffsetG |= *pbSrcPtrG;
		dwNextSample = *(pwDeCmprsG+wOffsetG);

		dwInterpol = (dwNextSample-dwThisSample) >> 1;

		*pdwDstPtrG = dwThisSample;
		pdwDstPtrG+=2;
		*pdwDstPtrG = dwThisSample+dwInterpol;
		pdwDstPtrG+=2;

		dwThisSample = dwNextSample;
		pbSrcPtrG++;

		dwLoopCountG--;
	}while(dwLoopCountG);

	*pdwDstPtrG = dwThisSample;
	pdwDstPtrG+=2;
	*pdwDstPtrG = dwThisSample;
	pdwDstPtrG+=2;
}

void MonoCopyPitch(void)
{
	do{
		wOffsetG &= 0xFF00;
		wOffsetG |= *pbSrcPtrG;

		*pdwDstPtrG = *(pwDeCmprsG+wOffsetG);
		pdwDstPtrG+=2;

		/*
		 * Increment sample pointer 
		 */
		wIncIndxG += wIncrementG;
		pbSrcPtrG += (wIncIndxG>>8);
		wIncIndxG &= 0x00FF;

		dwLoopCountG--;
	}while(dwLoopCountG);
}

void MonoCopyPitchHalf(void)
{
	int32 dwThisSample;
	int32 dwNextSample;
	int32 dwInterpol;

	wOffsetG &= 0xFF00;
	wOffsetG |= *pbSrcPtrG;

	dwThisSample = *(pwDeCmprsG+wOffsetG);
	wIncIndxG += wIncrementG;
	pbSrcPtrG += (wIncIndxG>>8);
	wIncIndxG &= 0x00FF;

	dwLoopCountG--;
	
	do{
		wOffsetG &= 0xFF00;
		wOffsetG |= *pbSrcPtrG;
		dwNextSample = *(pwDeCmprsG+wOffsetG);

		dwInterpol = (dwNextSample-dwThisSample) >> 1;

		*pdwDstPtrG = dwThisSample;
		pdwDstPtrG+=2;
		*pdwDstPtrG = dwThisSample+dwInterpol;
		pdwDstPtrG+=2;

		dwThisSample = dwNextSample;

		/*
		 * Increment sample pointer 
		 */
		wIncIndxG += wIncrementG;
		pbSrcPtrG += (wIncIndxG>>8);
		wIncIndxG &= 0x00FF;

		dwLoopCountG--;
	}while(dwLoopCountG);

	*pdwDstPtrG = dwThisSample;
	pdwDstPtrG += 2;
	*pdwDstPtrG = dwThisSample;
	pdwDstPtrG += 2;

}

void MonoMixNormal(void)
{
	do{
		wOffsetG &= 0xFF00;
		wOffsetG |= *pbSrcPtrG;

		*pdwDstPtrG += *(pwDeCmprsG+wOffsetG);
		pdwDstPtrG+=2;

		pbSrcPtrG++;
		dwLoopCountG--;
	}while(dwLoopCountG);
}

void MonoMixNormalHalf(void)
{
	int32 dwThisSample;
	int32 dwNextSample;
	int32 dwInterpol;

	wOffsetG &= 0xFF00;
	wOffsetG |= *pbSrcPtrG;

	dwThisSample = *(pwDeCmprsG+wOffsetG);
	pbSrcPtrG++;

	dwLoopCountG--;

	do{
		wOffsetG &= 0xFF00;
		wOffsetG |= *pbSrcPtrG;
		dwNextSample = *(pwDeCmprsG+wOffsetG);

		dwInterpol = (dwNextSample-dwThisSample) >> 1;

		*pdwDstPtrG += dwThisSample;
		pdwDstPtrG+=2;
		*pdwDstPtrG += dwThisSample+dwInterpol;
		pdwDstPtrG+=2;

		dwThisSample = dwNextSample;
		pbSrcPtrG++;

		dwLoopCountG--;
	}while(dwLoopCountG);

	*pdwDstPtrG += dwThisSample;
	pdwDstPtrG+=2;
	*pdwDstPtrG += dwThisSample;
	pdwDstPtrG+=2;
}

void MonoMixPitch(void)
{
	do{
		wOffsetG &= 0xFF00;
		wOffsetG |= *pbSrcPtrG;

		*pdwDstPtrG += *(pwDeCmprsG+wOffsetG);
		pdwDstPtrG++;	  

		/*
		 * Increment sample pointer 
		 */
		wIncIndxG += wIncrementG;
		pbSrcPtrG += (wIncIndxG >> 8);
		wIncIndxG &= 0x00FF;

		dwLoopCountG--;
	}while(dwLoopCountG);
}

void MonoMixPitchHalf(void)
{
	int32 dwThisSample;
	int32 dwNextSample;
	int32 dwInterpol;

	wOffsetG &= 0xFF00;
	wOffsetG |= *pbSrcPtrG;

	dwThisSample = *(pwDeCmprsG+wOffsetG);
	wIncIndxG += wIncrementG;
	pbSrcPtrG += (wIncIndxG>>8);
	wIncIndxG &= 0x00FF;

	dwLoopCountG--;
	
	do{
		wOffsetG &= 0xFF00;
		wOffsetG |= *pbSrcPtrG;
		dwNextSample = *(pwDeCmprsG+wOffsetG);

		dwInterpol = (dwNextSample-dwThisSample) >> 1;

		*pdwDstPtrG += dwThisSample;
		pdwDstPtrG+=2;
		*pdwDstPtrG += dwThisSample+dwInterpol;
		pdwDstPtrG+=2;

		dwThisSample = dwNextSample;

		/*
		 * Increment sample pointer 
		 */
		wIncIndxG += wIncrementG;
		pbSrcPtrG += (wIncIndxG>>8);
		wIncIndxG &= 0x00FF;

		dwLoopCountG--;
	}while(dwLoopCountG);

	*pdwDstPtrG = dwThisSample;
	pdwDstPtrG += 2;
	*pdwDstPtrG = dwThisSample;
	pdwDstPtrG += 2;
}


void MonoZeroPad(void)
{
	do{
		*pdwDstPtrG = 0;
		pdwDstPtrG+=2;

		dwLoopCountG--;
	}while(dwLoopCountG);
}

#endif

/*******************************************************
 * Stereo Mixing routines 
 ******************************************************/

#ifdef STEREO_MIX

void StereoCopyNormal(void)
{
	uint16 wLeftOffset;
	uint16 wRightOffset;

	wLeftOffset = (uint16)bLeftOffG << 8;
	wRightOffset = (uint16)bRightOffG << 8;

	do{
		wLeftOffset &= 0xFF00;
		wLeftOffset |= *pbSrcPtrG;

		*pdwDstPtrG = *(pwDeCmprsG+wLeftOffset);
		pdwDstPtrG++;

		wRightOffset &= 0xFF00;
		wRightOffset |= *pbSrcPtrG;

		*pdwDstPtrG = *(pwDeCmprsG+wRightOffset);
		pdwDstPtrG++;

		pbSrcPtrG++;
		dwLoopCountG--;
	}while(dwLoopCountG);
}

void StereoCopyNormalHalf(void)
{	
	int32 dwThisLeftSample;
	int32 dwThisRightSample;
	int32 dwNextLeftSample;
	int32 dwNextRightSample;
	int32 dwLeftInterpol;
	int32 dwRightInterpol;
	uint16 wLeftOffset;
	uint16 wRightOffset;

	wLeftOffset = (uint16)bLeftOffG << 8;
	wRightOffset = (uint16)bRightOffG << 8;

	wLeftOffset &= 0xFF00;
	wLeftOffset |= *pbSrcPtrG;
	dwThisLeftSample = *(pwDeCmprsG+wLeftOffset);

	wRightOffset &= 0xFF00;
	wRightOffset |= *pbSrcPtrG;
	dwThisRightSample = *(pwDeCmprsG+wRightOffset);

	pbSrcPtrG++;

	dwLoopCountG--;
	
	do{
		wLeftOffset &= 0xFF00;
		wLeftOffset |= *pbSrcPtrG;
		dwNextLeftSample = *(pwDeCmprsG+wLeftOffset);

		wRightOffset &= 0xFF00;
		wRightOffset |= *pbSrcPtrG;
		dwNextRightSample = *(pwDeCmprsG+wRightOffset);

		dwLeftInterpol = (dwNextLeftSample-dwThisLeftSample) >> 1;
		dwRightInterpol = (dwNextRightSample-dwThisRightSample) >> 1;

		*pdwDstPtrG = dwThisLeftSample;
		pdwDstPtrG++;
		*pdwDstPtrG = dwThisRightSample;
		pdwDstPtrG++;
		*pdwDstPtrG = dwThisLeftSample+dwLeftInterpol;
		pdwDstPtrG++;
		*pdwDstPtrG = dwThisRightSample+dwRightInterpol;
		pdwDstPtrG++;

		dwThisLeftSample = dwNextLeftSample;
		dwThisRightSample = dwNextLeftSample;

		pbSrcPtrG++;

		dwLoopCountG--;
	}while(dwLoopCountG);

	*pdwDstPtrG = dwThisLeftSample;
	pdwDstPtrG++;
	*pdwDstPtrG = dwThisRightSample;
	pdwDstPtrG++;
	*pdwDstPtrG = dwThisLeftSample;
	pdwDstPtrG++;
	*pdwDstPtrG = dwThisRightSample;
	pdwDstPtrG++;
}

void StereoCopyPitch(void)
{
	uint16 wLeftOffset;
	uint16 wRightOffset;

	wLeftOffset = (uint16)bLeftOffG << 8;
	wRightOffset = (uint16)bRightOffG << 8;

	do{
		wLeftOffset &= 0xFF00;
		wLeftOffset |= *pbSrcPtrG;

		*pdwDstPtrG = *(pwDeCmprsG+wLeftOffset);
		pdwDstPtrG++;
	 
		wRightOffset &= 0xFF00;
		wRightOffset |= *pbSrcPtrG;

		*pdwDstPtrG = *(pwDeCmprsG+wRightOffset);
		pdwDstPtrG++;
							 
		/*
		 * Increment sample pointer 
		 */
		wIncIndxG += wIncrementG;
		pbSrcPtrG += (wIncIndxG >> 8);
		wIncIndxG &= 0x00FF;             

		dwLoopCountG--;
	}while(dwLoopCountG);
}

void StereoCopyPitchHalf(void)
{
	int32 dwThisLeftSample;
	int32 dwThisRightSample;
	int32 dwNextLeftSample;
	int32 dwNextRightSample;
	int32 dwLeftInterpol;
	int32 dwRightInterpol;
	uint16 wLeftOffset;
	uint16 wRightOffset;

	wLeftOffset = (uint16)bLeftOffG << 8;
	wRightOffset = (uint16)bRightOffG << 8;

	wLeftOffset &= 0xFF00;
	wLeftOffset |= *pbSrcPtrG;
	dwThisLeftSample = *(pwDeCmprsG+wLeftOffset);

	wRightOffset &= 0xFF00;
	wRightOffset |= *pbSrcPtrG;
	dwThisRightSample = *(pwDeCmprsG+wRightOffset);

	wIncIndxG += wIncrementG;
	pbSrcPtrG += (wIncIndxG>>8);
	wIncIndxG &= 0x00FF;

	dwLoopCountG--;

	do{
		wLeftOffset &= 0xFF00;
		wLeftOffset |= *pbSrcPtrG;
		dwNextLeftSample = *(pwDeCmprsG+wLeftOffset);

		wRightOffset &= 0xFF00;
		wRightOffset |= *pbSrcPtrG;
		dwNextRightSample = *(pwDeCmprsG+wRightOffset);
		
		dwLeftInterpol = (dwNextLeftSample-dwThisLeftSample) >> 1;
		dwRightInterpol = (dwNextRightSample-dwThisRightSample) >> 1;

		*pdwDstPtrG = dwThisLeftSample;
		pdwDstPtrG++;
		*pdwDstPtrG = dwThisRightSample;
		pdwDstPtrG++;
		*pdwDstPtrG = dwThisLeftSample+dwLeftInterpol;
		pdwDstPtrG++;
		*pdwDstPtrG = dwThisRightSample+dwRightInterpol;
		pdwDstPtrG++;

		dwThisLeftSample = dwNextLeftSample;
		dwThisRightSample = dwNextLeftSample;

		/*
		 * Increment sample pointer 
		 */
		wIncIndxG += wIncrementG;
		pbSrcPtrG += (wIncIndxG >> 8);
		wIncIndxG &= 0x00FF;             
	
		dwLoopCountG--;
	}while(dwLoopCountG);

	*pdwDstPtrG = dwThisLeftSample;
	pdwDstPtrG++;
	*pdwDstPtrG = dwThisRightSample;
	pdwDstPtrG++;
	*pdwDstPtrG = dwThisLeftSample;
	pdwDstPtrG++;
	*pdwDstPtrG = dwThisRightSample;
	pdwDstPtrG++;
}

void StereoMixNormal(void)
{
	uint16 wLeftOffset;
	uint16 wRightOffset;

	wLeftOffset = (uint16)bLeftOffG << 8;
	wRightOffset = (uint16)bRightOffG << 8;
	
	do{
		wLeftOffset &= 0xFF00;
		wLeftOffset |= *pbSrcPtrG;

		*pdwDstPtrG += *(pwDeCmprsG+wLeftOffset);
		pdwDstPtrG++;

		wRightOffset &= 0xFF00;
		wRightOffset |= *pbSrcPtrG;

		*pdwDstPtrG += *(pwDeCmprsG+wRightOffset);
		pdwDstPtrG++;

		pbSrcPtrG++;
		dwLoopCountG--;
	}while(dwLoopCountG);
}

void StereoMixNormalHalf(void)
{
	int32 dwThisLeftSample;
	int32 dwThisRightSample;
	int32 dwNextLeftSample;
	int32 dwNextRightSample;
	int32 dwLeftInterpol;
	int32 dwRightInterpol;
	uint16 wLeftOffset;
	uint16 wRightOffset;

	wLeftOffset = (uint16)bLeftOffG << 8;
	wRightOffset = (uint16)bRightOffG << 8;

	wLeftOffset &= 0xFF00;
	wLeftOffset |= *pbSrcPtrG;
	dwThisLeftSample = *(pwDeCmprsG+wLeftOffset);

	wRightOffset &= 0xFF00;
	wRightOffset |= *pbSrcPtrG;
	dwThisRightSample = *(pwDeCmprsG+wRightOffset);

	pbSrcPtrG++;

	dwLoopCountG--;
	
	do{
		wLeftOffset &= 0xFF00;
		wLeftOffset |= *pbSrcPtrG;
		dwNextLeftSample = *(pwDeCmprsG+wLeftOffset);

		wRightOffset &= 0xFF00;
		wRightOffset |= *pbSrcPtrG;
		dwNextRightSample = *(pwDeCmprsG+wRightOffset);

		dwLeftInterpol = (dwNextLeftSample-dwThisLeftSample) >> 1;
		dwRightInterpol = (dwNextRightSample-dwThisRightSample) >> 1;

		*pdwDstPtrG += dwThisLeftSample;
		pdwDstPtrG++;
		*pdwDstPtrG += dwThisRightSample;
		pdwDstPtrG++;
		*pdwDstPtrG += dwThisLeftSample+dwLeftInterpol;
		pdwDstPtrG++;
		*pdwDstPtrG += dwThisRightSample+dwRightInterpol;
		pdwDstPtrG++;

		dwThisLeftSample = dwNextLeftSample;
		dwThisRightSample = dwNextLeftSample;

		pbSrcPtrG++;

		dwLoopCountG--;
	}while(dwLoopCountG);

	*pdwDstPtrG += dwThisLeftSample;
	pdwDstPtrG++;
	*pdwDstPtrG += dwThisRightSample;
	pdwDstPtrG++;
	*pdwDstPtrG += dwThisLeftSample;
	pdwDstPtrG++;
	*pdwDstPtrG += dwThisRightSample;
	pdwDstPtrG++;
}

void StereoMixPitch(void)
{
	uint16 wLeftOffset;
	uint16 wRightOffset;

	wLeftOffset = (uint16)bLeftOffG << 8;
	wRightOffset = (uint16)bRightOffG << 8;
	
	do{
		wLeftOffset &= 0xFF00;
		wLeftOffset |= *pbSrcPtrG;

		*pdwDstPtrG += *(pwDeCmprsG+wLeftOffset);
		pdwDstPtrG++;	  

		wRightOffset &= 0xFF00;
		wRightOffset |= *pbSrcPtrG;

		*pdwDstPtrG += *(pwDeCmprsG+wRightOffset);
		pdwDstPtrG++;

		/*
		 * Increment sample pointer 
		 */
		wIncIndxG += wIncrementG;
		pbSrcPtrG += (wIncIndxG >> 8);
		wIncIndxG &= 0x00FF;

		dwLoopCountG--;
	}while(dwLoopCountG);
}

void StereoMixPitchHalf(void)
{
	int32 dwThisLeftSample;
	int32 dwThisRightSample;
	int32 dwNextLeftSample;
	int32 dwNextRightSample;
	int32 dwLeftInterpol;
	int32 dwRightInterpol;
	uint16 wLeftOffset;
	uint16 wRightOffset;

	wLeftOffset = (uint16)bLeftOffG << 8;
	wRightOffset = (uint16)bRightOffG << 8;

	wLeftOffset &= 0xFF00;
	wLeftOffset |= *pbSrcPtrG;
	dwThisLeftSample = *(pwDeCmprsG+wLeftOffset);

	wRightOffset &= 0xFF00;
	wRightOffset |= *pbSrcPtrG;
	dwThisRightSample = *(pwDeCmprsG+wRightOffset);

	wIncIndxG += wIncrementG;
	pbSrcPtrG += (wIncIndxG>>8);
	wIncIndxG &= 0x00FF;

	dwLoopCountG--;

	do{
		wLeftOffset &= 0xFF00;
		wLeftOffset |= *pbSrcPtrG;
		dwNextLeftSample = *(pwDeCmprsG+wLeftOffset);

		wRightOffset &= 0xFF00;
		wRightOffset |= *pbSrcPtrG;
		dwNextRightSample = *(pwDeCmprsG+wRightOffset);
		
		dwLeftInterpol = (dwNextLeftSample-dwThisLeftSample) >> 1;
		dwRightInterpol = (dwNextRightSample-dwThisRightSample) >> 1;

		*pdwDstPtrG += dwThisLeftSample;
		pdwDstPtrG++;
		*pdwDstPtrG += dwThisRightSample;
		pdwDstPtrG++;
		*pdwDstPtrG += dwThisLeftSample+dwLeftInterpol;
		pdwDstPtrG++;
		*pdwDstPtrG += dwThisRightSample+dwRightInterpol;
		pdwDstPtrG++;

		dwThisLeftSample = dwNextLeftSample;
		dwThisRightSample = dwNextLeftSample;

		/*
		 * Increment sample pointer 
		 */
		wIncIndxG += wIncrementG;
		pbSrcPtrG += (wIncIndxG >> 8);
		wIncIndxG &= 0x00FF;             
	
		dwLoopCountG--;
	}while(dwLoopCountG);

	*pdwDstPtrG += dwThisLeftSample;
	pdwDstPtrG++;
	*pdwDstPtrG += dwThisRightSample;
	pdwDstPtrG++;
	*pdwDstPtrG += dwThisLeftSample;
	pdwDstPtrG++;
	*pdwDstPtrG += dwThisRightSample;
	pdwDstPtrG++;
}
	

void StereoZeroPad(void)
{
	do{
		*pdwDstPtrG = 0;
		pdwDstPtrG++;

		*pdwDstPtrG = 0;
		pdwDstPtrG++;

		dwLoopCountG--;
	}while(dwLoopCountG);
}

#endif

/*****************************************************
* Dolby Surround Sound mixing routines               *
*****************************************************/

#ifdef SURROUND_MIX

void StereoCopyNormalSurround(void)
{
	uint16 wLeftOffset;

	wLeftOffset = (uint16)bLeftOffG << 8;

	do{
		wLeftOffset &= 0xFF00;
		wLeftOffset |= *pbSrcPtrG;

		*pdwDstPtrG = *(pwDeCmprsG+wLeftOffset);
		*(pdwDstPtrG+1) = -(*pdwDstPtrG);	/* invert */
		pdwDstPtrG+=2;

		pbSrcPtrG++;
		dwLoopCountG--;
	}while(dwLoopCountG);
}

void StereoCopyNormalSurroundHalf(void)
{
	int32 dwThisSample;
	int32 dwNextSample;
	int32 dwInterpol;
	uint16 wLeftOffset;

	wLeftOffset = (uint16)bLeftOffG << 8;

	wLeftOffset &= 0xFF00;
	wLeftOffset |= *pbSrcPtrG;

	dwThisSample = *(pwDeCmprsG+wLeftOffset);
	pbSrcPtrG++;

	dwLoopCountG--;

	do{
		wLeftOffset &= 0xFF00;
		wLeftOffset |= *pbSrcPtrG;
		dwNextSample = *(pwDeCmprsG+wLeftOffset);

		dwInterpol = (dwNextSample-dwThisSample) >> 1;

		*pdwDstPtrG = dwThisSample;
		pdwDstPtrG++;
		*pdwDstPtrG = -dwThisSample;		/* invert */
		pdwDstPtrG++;
		*pdwDstPtrG = dwThisSample+dwInterpol;
		pdwDstPtrG++;
		*pdwDstPtrG = -(dwThisSample+dwInterpol); /* invert */
		pdwDstPtrG++;

		dwThisSample = dwNextSample;
		pbSrcPtrG++;

		dwLoopCountG--;
	}while(dwLoopCountG);

	*pdwDstPtrG = dwThisSample;
	pdwDstPtrG++;
	*pdwDstPtrG = -dwThisSample;		/* invert */
	pdwDstPtrG++;
	*pdwDstPtrG = dwThisSample+dwInterpol;
	pdwDstPtrG++;
	*pdwDstPtrG = -(dwThisSample+dwInterpol);	/* invert */
	pdwDstPtrG++;
}

void StereoMixNormalSurround(void)
{
	int32 dwSample;
	uint16 wLeftOffset ;

	wLeftOffset = (uint16)bLeftOffG << 8;

	do{
		wLeftOffset &= 0xFF00;
		wLeftOffset |= *pbSrcPtrG;

		dwSample = *(pwDeCmprsG+wLeftOffset);
		*pdwDstPtrG += dwSample;
		*(pdwDstPtrG+1) -= dwSample;	/* invert */
		pdwDstPtrG+=2;

		pbSrcPtrG++;
		dwLoopCountG--;
	}while(dwLoopCountG);
}

void StereoMixNormalSurroundHalf(void)
{
	int32 dwThisSample;
	int32 dwNextSample;
	int32 dwInterpol;
	uint16 wLeftOffset ;

	wLeftOffset = (uint16)bLeftOffG << 8;

	wLeftOffset &= 0xFF00;
	wLeftOffset |= *pbSrcPtrG;

	dwThisSample = *(pwDeCmprsG+wLeftOffset);
	pbSrcPtrG++;

	dwLoopCountG--;

	do{
		wLeftOffset &= 0xFF00;
		wLeftOffset |= *pbSrcPtrG;
		dwNextSample = *(pwDeCmprsG+wLeftOffset);

		dwInterpol = (dwNextSample-dwThisSample) >> 1;

		*pdwDstPtrG += dwThisSample;
		pdwDstPtrG++;
		*pdwDstPtrG += -dwThisSample;		/* invert */
		pdwDstPtrG++;
		*pdwDstPtrG += dwThisSample+dwInterpol;
		pdwDstPtrG++;
		*pdwDstPtrG += -(dwThisSample+dwInterpol); /* invert */
		pdwDstPtrG++;

		dwThisSample = dwNextSample;
		pbSrcPtrG++;

		dwLoopCountG--;
	}while(dwLoopCountG);

	*pdwDstPtrG += dwThisSample;
	pdwDstPtrG++;
	*pdwDstPtrG += -dwThisSample;		/* invert */
	pdwDstPtrG++;
	*pdwDstPtrG += dwThisSample+dwInterpol;
	pdwDstPtrG++;
	*pdwDstPtrG += -(dwThisSample+dwInterpol);	/* invert */
	pdwDstPtrG++;
}

void StereoCopyPitchSurround(void)
{
	int32 dwSample;
	uint16 wLeftOffset ;

	wLeftOffset = (uint16)bLeftOffG << 8;

	do{
		wLeftOffset &= 0xFF00;
		wLeftOffset |= *pbSrcPtrG;

		dwSample = *(pwDeCmprsG+wLeftOffset);
		*pdwDstPtrG = dwSample;
		*(pdwDstPtrG+1) = -dwSample		; /* invert */
		pdwDstPtrG+=2;
							 
		/*
		 * Increment sample pointer 
		 */
		wIncIndxG += wIncrementG;
		pbSrcPtrG += (wIncIndxG >> 8);
		wIncIndxG &= 0x00FF;             

		dwLoopCountG--;
	}while(dwLoopCountG);
}

void StereoCopyPitchSurroundHalf(void)
{
	int32 dwThisSample;
	int32 dwNextSample;
	int32 dwInterpol;
	uint16 wLeftOffset ;

	wLeftOffset = (uint16)bLeftOffG << 8;

	wLeftOffset &= 0xFF00;
	wLeftOffset |= *pbSrcPtrG;

	dwThisSample = *(pwDeCmprsG+wLeftOffset);
	/*
	 * Increment sample pointer 
	 */
	wIncIndxG += wIncrementG;
	pbSrcPtrG += (wIncIndxG >> 8);
	wIncIndxG &= 0x00FF;             

	dwLoopCountG--;

	do{
		wLeftOffset &= 0xFF00;
		wLeftOffset |= *pbSrcPtrG;
		dwNextSample = *(pwDeCmprsG+wLeftOffset);

		dwInterpol = (dwNextSample-dwThisSample) >> 1;

		*pdwDstPtrG = dwThisSample;
		pdwDstPtrG++;
		*pdwDstPtrG = -dwThisSample;		/* invert */
		pdwDstPtrG++;
		*pdwDstPtrG = dwThisSample+dwInterpol;
		pdwDstPtrG++;
		*pdwDstPtrG = -(dwThisSample+dwInterpol); /* invert */
		pdwDstPtrG++;

		dwThisSample = dwNextSample;
		/*
		 * Increment sample pointer 
		 */
		wIncIndxG += wIncrementG;
		pbSrcPtrG += (wIncIndxG >> 8);
		wIncIndxG &= 0x00FF;             

		dwLoopCountG--;
	}while(dwLoopCountG);

	*pdwDstPtrG = dwThisSample;
	pdwDstPtrG++;
	*pdwDstPtrG = -dwThisSample;		/* invert */
	pdwDstPtrG++;
	*pdwDstPtrG = dwThisSample+dwInterpol;
	pdwDstPtrG++;
	*pdwDstPtrG = -(dwThisSample+dwInterpol);	/* invert */
	pdwDstPtrG++;
}

void StereoMixPitchSurround(void)
{
	int32 dwSample;
	uint16 wLeftOffset ;

	wLeftOffset = (uint16)bLeftOffG << 8;

	do{
		wLeftOffset &= 0xFF00;
		wLeftOffset |= *pbSrcPtrG;

		dwSample = *(pwDeCmprsG+wLeftOffset);
		*pdwDstPtrG += dwSample;
		*(pdwDstPtrG+1) -= dwSample;		/* invert */
		pdwDstPtrG+=2;
							 
		/*
		 * Increment sample pointer 
		 */
		wIncIndxG += wIncrementG;
		pbSrcPtrG += (wIncIndxG >> 8);
		wIncIndxG &= 0x00FF;             

		dwLoopCountG--;
	}while(dwLoopCountG);
}

void StereoMixPitchSurroundHalf(void)
{
	int32 dwThisSample;
	int32 dwNextSample;
	int32 dwInterpol;
	uint16 wLeftOffset ;

	wLeftOffset = (uint16)bLeftOffG << 8;

	wLeftOffset &= 0xFF00;
	wLeftOffset |= *pbSrcPtrG;

	dwThisSample = *(pwDeCmprsG+wLeftOffset);
	/*
	 * Increment sample pointer 
	 */
	wIncIndxG += wIncrementG;
	pbSrcPtrG += (wIncIndxG >> 8);
	wIncIndxG &= 0x00FF;             

	dwLoopCountG--;

	do{
		wLeftOffset &= 0xFF00;
		wLeftOffset |= *pbSrcPtrG;
		dwNextSample = *(pwDeCmprsG+wLeftOffset);

		dwInterpol = (dwNextSample-dwThisSample) >> 1;

		*pdwDstPtrG += dwThisSample;
		pdwDstPtrG++;
		*pdwDstPtrG += -dwThisSample;		/* invert */
		pdwDstPtrG++;
		*pdwDstPtrG += dwThisSample+dwInterpol;
		pdwDstPtrG++;
		*pdwDstPtrG += -(dwThisSample+dwInterpol); /* invert */
		pdwDstPtrG++;

		dwThisSample = dwNextSample;
		/*
		 * Increment sample pointer 
		 */
		wIncIndxG += wIncrementG;
		pbSrcPtrG += (wIncIndxG >> 8);
		wIncIndxG &= 0x00FF;             

		dwLoopCountG--;
	}while(dwLoopCountG);

	*pdwDstPtrG += dwThisSample;
	pdwDstPtrG++;
	*pdwDstPtrG += -dwThisSample;		/* invert */
	pdwDstPtrG++;
	*pdwDstPtrG += dwThisSample+dwInterpol;
	pdwDstPtrG++;
	*pdwDstPtrG += -(dwThisSample+dwInterpol);	/* invert */
	pdwDstPtrG++;
}

#endif

void MixChat(void)
{
	uint32 dwLoopCount;
	uint8 *pbSrcPtr;
	int32 *pdwDstPtr;
	uint32 dwThisSample;
	uint32 dwNextSample;
	uint32 dwQuaterDelta;

	/*
	 * Mix chat message into soundFX mix buffer
	 */
	pdwDstPtr = pdwMixBufG;
	pbSrcPtr = pbChatMsgPtrG + dwChatMsgIndxG;
	dwLoopCount = MIN(dwChatMsgSizeG,dwMixBufFramesG/4);

	dwChatMsgIndxG += dwLoopCount;			
	dwChatMsgSizeG -= dwLoopCount;

	if(dwChatMsgSizeG == 0)
	{
		bChatReadyG = 0;		/* message complete */
	}

	dwLoopCount--;

	if(dwLoopCount)
	{
		dwThisSample = *pbSrcPtr;
		dwThisSample -= 128;
		dwThisSample <<= 8;		/* convert to 16 bit */
		pbSrcPtr++;

		do{
			/*
			 * Assume stereo 16bit 22kHz
			 */
			dwNextSample = *pbSrcPtr;
			dwNextSample -= 128;
			dwNextSample <<= 8;

			dwQuaterDelta = (dwNextSample-dwThisSample)>>2;

			*pdwDstPtr += dwThisSample;		/* left */
			pdwDstPtr++;
			*pdwDstPtr += dwThisSample;		/* right */
			pdwDstPtr++;

			/*
			 * 1/4 interpolation
			 */
			dwThisSample += dwQuaterDelta;
			*pdwDstPtr += dwThisSample;
			pdwDstPtr++;
			*pdwDstPtr += dwThisSample;
			pdwDstPtr++;

			/*
			 * 1/2 interpolation
			 */
			dwThisSample += dwQuaterDelta;
			*pdwDstPtr += dwThisSample;
			pdwDstPtr++;
			*pdwDstPtr += dwThisSample;
			pdwDstPtr++;

			/*
			 * 3/4 interpolation
			 */
			dwThisSample += dwQuaterDelta;
			*pdwDstPtr += dwThisSample;
			pdwDstPtr++;
			*pdwDstPtr += dwThisSample;
			pdwDstPtr++;
			
			dwThisSample = dwNextSample;
			pbSrcPtr++;
	
			dwLoopCount--;
		}while(dwLoopCount);

		/*
		 * last 4 frames 
		 */
		*pdwDstPtr += dwThisSample;		/* left */
		pdwDstPtr++;
		*pdwDstPtr += dwThisSample;		/* right */
		pdwDstPtr++;
		*pdwDstPtr += dwThisSample;		/* left */
		pdwDstPtr++;
		*pdwDstPtr += dwThisSample;		/* right */
		pdwDstPtr++;
		*pdwDstPtr += dwThisSample;		/* left */
		pdwDstPtr++;
		*pdwDstPtr += dwThisSample;		/* right */
		pdwDstPtr++;
		*pdwDstPtr += dwThisSample;		/* left */
		pdwDstPtr++;
		*pdwDstPtr += dwThisSample;		/* right */
		pdwDstPtr++;
	}	
}	

	
