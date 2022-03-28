#include "c:\watcom\h\stdlib.h"
#include <math.h>

#include "utypes.h"

#define MIN(a,b)		((a) < (b) ? (a) : (b))

#define ECHO 				0
#define DELAY_1			1
#define DELAY_2			2
#define REVERB			3

#define MONO 0
#define STEREO 1

static uint8  wReverbTypeG;
uint8 bDepthG = 1;
uint8 bDelayCoeffG = 1;

/*
 * Reverb parameters
 */
#define COMB1_LOOP_TIME		51
#define COMB2_LOOP_TIME  	27
#define COMB3_LOOP_TIME  	43
#define ALLPASS1_LOOP_TIME 7

/*
 * Comment out these defines to 
 * use the assembler versions of the 
 * echo/delay/reverb routines
 */
#define CLEAR_CODE
//#define ECHO_CODE
//#define DELAY_CODE
//#define REVERB_CODE

/*
 * Exported globals (to sfxrvblo.asm)
 */
int32 *pdwWorkBufG;

int32  *pdwDelayPtrG;   
int32  *pdwDelayPosG;
uint32 dwDelayLenG;

int32  *pdwComb1PtrG;
int32  *pdwComb1PosG;
uint32 dwComb1LenG;

int32  *pdwComb2PtrG;
int32  *pdwComb2PosG;
uint32 dwComb2LenG;

int32  *pdwComb3PtrG;
int32  *pdwComb3PosG;
uint32 dwComb3LenG;


int32  *pdwAllPassPtrG;
int32  *pdwAllPassPosG;
uint32 dwAllPassLenG;


extern int32 *pdwMixBufG;
extern uint32 dwMixBufSamplesG;
extern uint32 dwMixBufFramesG;
extern uint16 wSampleRateG;

void (*DoReverb)(void);
void MonoEcho(void);
void StereoEcho(void);
void MonoDelay(void);
void StereoDelay(void);
void MonoReverb(void);
void StereoReverb(void);

void MonoEchoLo(void);
void StereoEchoLo(void);

/************************************************************
*
* uint16 InitReverb(uint16 wReverbType,uint8 bChannels,uint16 wDelay)
*
* uint16 wReverbType: Type of effect: DELAY
* uint8 bChannels:    Number of channels: 1 or 2
* uint16 wDelay:			Delay time in milli seconds (echo and delay)
*
**************************************************************
*
* Sets up the reverb work areas 
*
**************************************************************/
uint16 InitReverb(uint16 wReverbType,uint8 bChannels,uint16 wDelay)
{
	uint32 dwBufLen;

	wReverbTypeG = wReverbType;		/* store type in global */

	/*
	 * Setup Buffers
	 */
	switch(wReverbType)
	{
		case ECHO:
		case DELAY_1:
		case DELAY_2:

			if(wReverbType == DELAY_1)
				bDelayCoeffG = 2;		/* slow decay */
			else
				bDelayCoeffG = 1;		/* fast decay */
		 
			/*
			 * Calculate length of buffer
			 */
			dwBufLen = (wSampleRateG*wDelay)/1000;									
			//dwBufLen &= 0xFFFFFFC;	/* make multiple of 4 */

			/*
			 * Allocate working buffers
			 */
			pdwDelayPtrG = (int32*)malloc(dwBufLen*sizeof(int32));
			if(pdwDelayPtrG == NULL)
				return 1;		/* couldn't allocate buffers */
				  
			/* start and length of buffer */
			pdwDelayPosG = pdwDelayPtrG;
			dwDelayLenG = dwBufLen;
			break;
		case REVERB:

			pdwWorkBufG = (int32 *)malloc(sizeof(int32)*dwMixBufFramesG);
			if(pdwWorkBufG == NULL)
			{
				return 1;		/* not enough memory */
			}

			/* 
			 * setup first comb filter 
			 */
			dwBufLen = (wSampleRateG*COMB1_LOOP_TIME)/1000;
			pdwComb1PtrG = (int32*)malloc(dwBufLen*sizeof(int32));
			if(pdwComb1PtrG == NULL)
			{
				return 1;
			}
			pdwComb1PosG = pdwComb1PtrG;
			dwComb1LenG = dwBufLen;

			/*
			 * setup second comb filter 
			 */
			dwBufLen = (wSampleRateG*COMB2_LOOP_TIME)/1000;
			pdwComb2PtrG = (int32*)malloc(dwBufLen*sizeof(int32));
			if(pdwComb2PtrG == NULL)
			{
				return 1;
			}
			pdwComb2PosG = pdwComb2PtrG;
			dwComb2LenG = dwBufLen;

			/*
			 * setup second comb filter 
			 */
			dwBufLen = (wSampleRateG*COMB3_LOOP_TIME)/1000;
			pdwComb3PtrG = (int32*)malloc(dwBufLen*sizeof(int32));
			if(pdwComb3PtrG == NULL)
			{
				return 1;
			}
			pdwComb3PosG = pdwComb3PtrG;
			dwComb3LenG = dwBufLen;

			/*
			 * setup final all pass filter 
			 */
			dwBufLen = (wSampleRateG*ALLPASS1_LOOP_TIME)/1000;
			pdwAllPassPtrG = (int32*)malloc(dwBufLen*sizeof(int32));
			if(pdwAllPassPtrG == NULL)
			{	
				return 1;
			}
			pdwAllPassPosG = pdwAllPassPtrG;
			dwAllPassLenG = dwBufLen;
			break;

		default:
			return 1; 		/* effect unknown or unsupported */
	}

	/*
	 * Clear buffers
	 */																					
	ClearReverbBuffers();	 

	/*
	 * Setup reverb routine
	 */
	wReverbType<<=1;
	wReverbType += (bChannels == 2 ? STEREO : MONO);
	switch(wReverbType)
	{
		case (ECHO<<1) | MONO:
			DoReverb = MonoEcho;
			break;
		case (ECHO<<1) | STEREO:
			DoReverb = StereoEcho;
			break;
		case (DELAY_1<<1) | MONO:
		case (DELAY_2<<1) | MONO:
			DoReverb = MonoDelay;
			break;
		case (DELAY_1<<1) | STEREO:
		case (DELAY_2<<1) | STEREO:
			DoReverb = StereoDelay;
			break;
		case (REVERB<<1) | MONO:
			DoReverb = MonoReverb;
			break;
		case (REVERB<<1) | STEREO:
			DoReverb = StereoReverb;
			break;
	}

	return 0;
}

void UnInitReverb(void)
{

	switch(wReverbTypeG)
	{
		case ECHO:
		case DELAY_1:
		case DELAY_2:
			free(pdwDelayPtrG);
			break;
		case REVERB:
			free(pdwWorkBufG);
			free(pdwComb1PtrG);
			free(pdwComb2PtrG);
			free(pdwComb3PtrG);
			free(pdwAllPassPtrG);
			break;
	}
}


void SetReverbDepth(uint8 bDepth)
{
	bDepthG = 3-((bDepth&0x7f)/32);
}															 

void AddReverb(void)
{
	DoReverb();
}

/************ Non exported functions ***************/
/*
 * Assembler versions of these functions exist in 
 * Sfxrvblo.asm
 */

#ifdef CLEAR_CODE

void ClearReverbBuffers(void)
{
	uint32 dwLoopCount;
	int32 *pdwBufPtr;

	switch(wReverbTypeG)
	{
		case ECHO:
		case DELAY_1:
		case DELAY_2:
			pdwBufPtr = pdwDelayPtrG;
			dwLoopCount = dwDelayLenG;
			do{
				*pdwBufPtr=0;
				pdwBufPtr++;
				dwLoopCount--;
			}while(dwLoopCount);
			break;
	
		case REVERB:
			pdwBufPtr = pdwComb1PtrG;
			dwLoopCount = dwComb1LenG;
			do{
				*pdwBufPtr=0;
				pdwBufPtr++;
				dwLoopCount--;
			}while(dwLoopCount);

			pdwBufPtr = pdwComb2PtrG;
			dwLoopCount = dwComb2LenG;
			do{
				*pdwBufPtr=0;
				pdwBufPtr++;
				dwLoopCount--;
			}while(dwLoopCount);

			pdwBufPtr = pdwComb3PtrG;
			dwLoopCount = dwComb3LenG;
			do{
				*pdwBufPtr=0;
				pdwBufPtr++;
				dwLoopCount--;
			}while(dwLoopCount);


			pdwBufPtr = pdwAllPassPtrG;
			dwLoopCount = dwAllPassLenG;
			do{
				*pdwBufPtr=0;
				pdwBufPtr++;
				dwLoopCount--;
			}while(dwLoopCount);
			break;
	}
}

#endif

#ifdef ECHO_CODE

void MonoEcho(void)
{
	int32 *pdwMixPtr;
	uint32 dwLoopCount;
	int32 dwOutputSample;
	uint32 dwMixBufNotDone;
	uint32 dwDelayBufNotDone;

	/*
	 * Add delayed sample to each sample in mix buffer
	 */
	pdwMixPtr = pdwMixBufG;
	dwMixBufNotDone = dwMixBufFramesG;
	while(dwMixBufNotDone)
	{
		dwDelayBufNotDone = dwDelayLenG - (pdwDelayPosG-pdwDelayPtrG);
		dwLoopCount = MIN(dwMixBufNotDone,dwDelayBufNotDone);
		dwMixBufNotDone -= dwLoopCount;
		dwDelayBufNotDone -= dwLoopCount;

		/*
		 * get input sample
		 */
		*pdwDelayPosG = *pdwMixPtr;

		pdwDelayPosG++;

		dwLoopCount--;
		if(dwLoopCount>0)
		{
			do{
				/*
				 * output delayed sample
				 */
				dwOutputSample = (*pdwDelayPosG >> bDepthG);
				*pdwMixPtr += dwOutputSample;
				pdwMixPtr+=2;

				/*
				 * get input samples
				 */
				*pdwDelayPosG = *pdwMixPtr;

				pdwDelayPosG++;

				dwLoopCount--;
			}while(dwLoopCount);
		}

		if(dwDelayBufNotDone == 0)
			pdwDelayPosG = pdwDelayPtrG;

		/*
		 * output delayed sample
		 */
		dwOutputSample = (*pdwDelayPosG >> bDepthG);
		*pdwMixPtr += dwOutputSample;
		pdwMixPtr+=2;
	}
}

void StereoEcho(void)
{
	int32 *pdwMixPtr;
	uint32 dwLoopCount;
	int32 dwOutputSample;
	uint32 dwMixBufNotDone;
	uint32 dwDelayBufNotDone;

	/*
	 * Add delayed sample to each sample in mix buffer
	 */
	pdwMixPtr = pdwMixBufG;
	dwMixBufNotDone = dwMixBufFramesG;
	while(dwMixBufNotDone)
	{
		dwDelayBufNotDone = dwDelayLenG - (pdwDelayPosG-pdwDelayPtrG);
		dwLoopCount = MIN(dwMixBufNotDone,dwDelayBufNotDone);
		dwMixBufNotDone -= dwLoopCount;
		dwDelayBufNotDone -= dwLoopCount;

		/*
		 * get input samples
		 */
		*pdwDelayPosG = *pdwMixPtr;
		*pdwDelayPosG += *(pdwMixPtr+1);
		*pdwDelayPosG >>= 1;

		pdwDelayPosG++;

		dwLoopCount--;
		if(dwLoopCount>0)
		{
			do{
				/*
				 * output delayed sample 
				 */
				dwOutputSample = (*pdwDelayPosG >> bDepthG);
				*pdwMixPtr += dwOutputSample;
				pdwMixPtr++;
				*pdwMixPtr -= dwOutputSample;
				pdwMixPtr++;

				/*
				 * get input samples
				 */
				*pdwDelayPosG = *pdwMixPtr;
				*pdwDelayPosG += *(pdwMixPtr+1);
				*pdwDelayPosG >>= 1;

				pdwDelayPosG++;

				dwLoopCount--;
 			}while(dwLoopCount);
		}

		if(dwDelayBufNotDone == 0)
			pdwDelayPosG = pdwDelayPtrG;

		/* 
		 * output delayed sample
		 */
		dwOutputSample = (*pdwDelayPosG>>bDepthG);
		*pdwMixPtr += dwOutputSample;
		pdwMixPtr++;
		*pdwMixPtr -= dwOutputSample;
		pdwMixPtr++;
	}
}

#endif

#ifdef DELAY_CODE

void MonoDelay(void)
{
	int32 *pdwMixPtr;
	uint32 dwLoopCount;
	int32 *pdwOutPtr;
	int32 *pdwEndPtr;
	int32 dwInputSample;
	int32 dwOutputSample;

	/*
	 * Add delayed sample to each sample in mix buffer
	 */
	pdwMixPtr = pdwMixBufG;
	dwLoopCount = dwMixBufFramesG;
	pdwEndPtr = pdwDelayPtrG+dwDelayLenG-1;
	do{
		dwInputSample = *pdwMixPtr;	
 
		/* end of delay line */
  	pdwOutPtr = pdwDelayPosG+1;
  	if(pdwOutPtr > pdwEndPtr)
  		pdwOutPtr = pdwDelayPtrG;

		*pdwDelayPosG = dwInputSample + *pdwOutPtr - (*pdwOutPtr>>bDelayCoeffG);

		/* increment delay point */
		pdwDelayPosG = pdwOutPtr;

		/*
		 * Add delayed sample to buffer
		 */
		//dwOutputSample = (*pdwDelayPosG>>bDepthG);
		dwOutputSample = (*pdwOutPtr>>bDepthG);
		*pdwMixPtr += dwOutputSample;	
		pdwMixPtr+=2;
		
		dwLoopCount--;
	}while(dwLoopCount);	
}		

void StereoDelay(void)
{
	int32 *pdwMixPtr;
	uint32 dwLoopCount;
	int32 *pdwOutPtr;
	int32 *pdwEndPtr;
	int32 dwInputSample;
	int32 dwOutputSample;

	/*
	 * Add delayed sample to each sample in mix buffer
	 */
	pdwMixPtr = pdwMixBufG;
	dwLoopCount = dwMixBufFramesG;
	pdwEndPtr = pdwDelayPtrG+dwDelayLenG-1;
	do{
		dwInputSample = *pdwMixPtr;			/* left */
		dwInputSample += *(pdwMixPtr+1);	/* right */
		dwInputSample >>= 1;						/* average */
 
		/* end of delay line */
  	pdwOutPtr = pdwDelayPosG+1;
  	if(pdwOutPtr > pdwEndPtr)
  		pdwOutPtr = pdwDelayPtrG;

		*pdwDelayPosG = dwInputSample + *pdwOutPtr - (*pdwOutPtr>>bDelayCoeffG);

		/* increment delay point */
		pdwDelayPosG = pdwOutPtr;

		/*
		 * Add delayed sample to buffer
		 */
		//dwOutputSample = (*pdwDelayPosG>>bDepthG);
		dwOutputSample = (*pdwOutPtr>>bDepthG);
		*pdwMixPtr += dwOutputSample;	/* left channel */
		pdwMixPtr++;
		*pdwMixPtr -= dwOutputSample; /* right channel */
		pdwMixPtr++;
		
		dwLoopCount--;
	}while(dwLoopCount);	
}		
	
#endif

#ifdef REVERB_CODE



void MonoReverb(void)
{
	int32 dwInputSample;
	int32 dwOutputSample;
	int32 *pdwWorkPtr;
	int32 *pdwMixPtr;
	int32 *pdwOut1Ptr;
	int32 *pdwOut2Ptr;
	int32 *pdwOut3Ptr;
	int32 *pdwEnd1Ptr;
	int32 *pdwEnd2Ptr;
	int32 *pdwEnd3Ptr;
	uint32 dwLoopCount;

	/*
	 * Comb filter stage
	 */
	pdwMixPtr = pdwMixBufG;
	pdwWorkPtr = pdwWorkBufG;
	pdwEnd1Ptr = pdwComb1PtrG+dwComb1LenG-1;
	pdwEnd2Ptr = pdwComb2PtrG+dwComb2LenG-1;
	pdwEnd3Ptr = pdwComb3PtrG+dwComb3LenG-1;
	dwLoopCount = dwMixBufFramesG;
	do{
		dwInputSample = *pdwMixPtr;
		pdwMixPtr+=2;

		/* end of delay line */
		pdwOut1Ptr = pdwComb1PosG+1;
		if(pdwOut1Ptr > pdwEnd1Ptr)
			pdwOut1Ptr = pdwComb1PtrG;

		pdwOut2Ptr = pdwComb2PosG+1;
		if(pdwOut2Ptr > pdwEnd2Ptr)
			pdwOut2Ptr = pdwComb2PtrG;

		pdwOut3Ptr = pdwComb3PosG+1;
		if(pdwOut3Ptr > pdwEnd3Ptr)
			pdwOut3Ptr = pdwComb3PtrG;

		/* feed back sample */
		*pdwComb1PosG = dwInputSample + *pdwOut1Ptr - (*pdwOut1Ptr>>2);
		*pdwComb2PosG = dwInputSample + *pdwOut2Ptr - (*pdwOut2Ptr>>3);
		*pdwComb3PosG = dwInputSample + *pdwOut3Ptr - (*pdwOut3Ptr>>3);

		/* increment delay point */
		pdwComb1PosG = pdwOut1Ptr;
		pdwComb2PosG = pdwOut2Ptr;
		pdwComb3PosG = pdwOut3Ptr;

		/* thru sample */
		*pdwWorkPtr = *pdwOut1Ptr;
		*pdwWorkPtr += *pdwOut2Ptr;
		*pdwWorkPtr += *pdwOut3Ptr;
		pdwWorkPtr++;
			
		dwLoopCount--;
	}while(dwLoopCount);

	/*
	 * Final allpass filter and mix back stage
	 */
	pdwMixPtr = pdwMixBufG;
	pdwWorkPtr = pdwWorkBufG;
	pdwEnd1Ptr = pdwAllPassPtrG+dwAllPassLenG-1;
	dwLoopCount = dwMixBufFramesG;
	do{
		/* Get input sample */
		dwInputSample = *pdwWorkPtr;
		pdwWorkPtr++;

		/* end of delay line */
		pdwOut1Ptr = pdwAllPassPosG+1;
		if(pdwOut1Ptr > pdwEnd1Ptr)
			pdwOut1Ptr = pdwAllPassPtrG;

		/* feed back sample */
		*pdwAllPassPosG = dwInputSample + *pdwOut1Ptr - (*pdwOut1Ptr>>2);

		/* thru sample */
		dwOutputSample = *pdwOut1Ptr - *pdwAllPassPosG + (*pdwAllPassPosG>>2);

		/* increment delay point */
		pdwAllPassPosG = pdwOut1Ptr;

		/*
		 * Mix back into buffer
		 */
		dwOutputSample = (dwOutputSample>>bDepthG);
		*pdwMixPtr += dwOutputSample;
		pdwMixPtr+=2;

		dwLoopCount--;
	}while(dwLoopCount);
}

void StereoReverb(void)
{
	int32 dwInputSample;
	int32 dwOutputSample;
	int32 *pdwWorkPtr;
	int32 *pdwMixPtr;
	int32 *pdwOut1Ptr;
	int32 *pdwOut2Ptr;
	int32 *pdwOut3Ptr;
	int32 *pdwEnd1Ptr;
	int32 *pdwEnd2Ptr;
	int32 *pdwEnd3Ptr;
	uint32 dwLoopCount;

	/*
	 * Comb filter stage
	 */
	pdwMixPtr = pdwMixBufG;
	pdwWorkPtr = pdwWorkBufG;
	pdwEnd1Ptr = pdwComb1PtrG+dwComb1LenG-1;
	pdwEnd2Ptr = pdwComb2PtrG+dwComb2LenG-1;
	pdwEnd3Ptr = pdwComb3PtrG+dwComb3LenG-1;
	dwLoopCount = dwMixBufFramesG;
	do{
		dwInputSample = *pdwMixPtr;
		pdwMixPtr++;
		dwInputSample += *pdwMixPtr;
		pdwMixPtr++;
		dwInputSample >>= 1;

		/* end of delay line */
		pdwOut1Ptr = pdwComb1PosG+1;
		if(pdwOut1Ptr > pdwEnd1Ptr)
			pdwOut1Ptr = pdwComb1PtrG;

		pdwOut2Ptr = pdwComb2PosG+1;
		if(pdwOut2Ptr > pdwEnd2Ptr)
			pdwOut2Ptr = pdwComb2PtrG;

		pdwOut3Ptr = pdwComb3PosG+1;
		if(pdwOut3Ptr > pdwEnd3Ptr)
			pdwOut3Ptr = pdwComb3PtrG;

		/* feed back sample */
		*pdwComb1PosG = dwInputSample + *pdwOut1Ptr - (*pdwOut1Ptr>>2);
		*pdwComb2PosG = dwInputSample + *pdwOut2Ptr - (*pdwOut2Ptr>>3);
		*pdwComb3PosG = dwInputSample + *pdwOut3Ptr - (*pdwOut3Ptr>>3);

		/* increment delay point */
		pdwComb1PosG = pdwOut1Ptr;
		pdwComb2PosG = pdwOut2Ptr;
		pdwComb3PosG = pdwOut3Ptr;

		/* thru sample */
		*pdwWorkPtr = *pdwOut1Ptr;
		*pdwWorkPtr += *pdwOut2Ptr;
		*pdwWorkPtr += *pdwOut3Ptr;
		pdwWorkPtr++;
			
		dwLoopCount--;
	}while(dwLoopCount);

	/*
	 * Final allpass filter and mix back stage
	 */
	pdwMixPtr = pdwMixBufG;
	pdwWorkPtr = pdwWorkBufG;
	pdwEnd1Ptr = pdwAllPassPtrG+dwAllPassLenG-1;
	dwLoopCount = dwMixBufFramesG;
	do{
		/* Get input sample */
		dwInputSample = *pdwWorkPtr;
		pdwWorkPtr++;

		/* end of delay line */
		pdwOut1Ptr = pdwAllPassPosG+1;
		if(pdwOut1Ptr > pdwEnd1Ptr)
			pdwOut1Ptr = pdwAllPassPtrG;

		/* feed back sample */
		*pdwAllPassPosG = dwInputSample + *pdwOut1Ptr - (*pdwOut1Ptr>>2);

		/* thru sample */
		dwOutputSample = *pdwOut1Ptr - *pdwAllPassPosG + (*pdwAllPassPosG>>2);

		/* increment delay point */
		pdwAllPassPosG = pdwOut1Ptr;

		/*
		 * Mix back into buffer
		 */
		dwOutputSample = (dwOutputSample>>bDepthG);
		*pdwMixPtr += dwOutputSample;
		pdwMixPtr++;
		*pdwMixPtr -= dwOutputSample;
		pdwMixPtr++;

		dwLoopCount--;
	}while(dwLoopCount);
}


#endif		
	
	
	
