#include <libspu.h>

struct CdlPos {
	unsigned char minute;
	unsigned char second;
	unsigned char sector;
	unsigned char track;
};
typedef struct CdlPos CdlLOC;

#ifndef USE_ATI


void SpuInit (void)
{
	
}

void SpuStart (void)
{
	
}	

void SpuQuit (void)
{
	
}

long SpuSetMute (long on_off)
{
	return(1L);
}

long SpuGetMute (void)
{
	return(1L);
}


long SpuSetNoiseClock (long n_clock)
{
	return(1L);
}

long SpuGetNoiseClock (void)
{
	return(1L);
}

unsigned long SpuSetNoiseVoice (long on_off, unsigned long voice_bit)
{
	return(1L);
}

unsigned long SpuGetNoiseVoice (void)
{
	return(1L);
}



long SpuSetReverb (long on_off)
{
	return(1L);
}

long SpuGetReverb (void)
{
	return(1L);
}

long SpuSetReverbModeParam (SpuReverbAttr *attr)
{
	return(1L);
}

void SpuGetReverbModeParam (SpuReverbAttr *attr)
{
}

void SpuGetReverbDepth (SpuReverbAttr *attr)
{
}
#endif //USE_ATI

void SpuSetReverbDepth (SpuReverbAttr *attr)
{
}

#ifndef USE_ATI

long SpuReserveReverbWorkArea (long on_off)
{
	return(1L);
}

long SpuIsReverbWorkAreaReserved (long on_off)
{
	return(1L);
}

unsigned long SpuSetReverbOffsetAddr (unsigned long addr)
{
	return(1L);
}

unsigned long SpuGetReverbOffsetAddr (void)
{
	return(1L);
}	

unsigned long SpuSetReverbVoice (long on_off, unsigned long voice_bit)
{
	return(1L);
}

unsigned long SpuGetReverbVoice (void)
{
	return(1L);
}

long SpuClearReverbWorkArea (long mode)
{
	return(1L);
}


unsigned long SpuWrite (unsigned char *addr, unsigned long size)
{
	return(1L);
}

unsigned long SpuWrite0 (unsigned long size)
{
	return(1L);
}
	
unsigned long SpuRead (unsigned char *addr, unsigned long size)
{
	return(1L);
}

long SpuSetTransferMode (long mode)
{
	return(1L);
}

long SpuGetTransferMode (void)
{
	return(1L);
}

unsigned long SpuSetTransferStartAddr (unsigned long addr)
{
	return(1L);
}

unsigned long SpuGetTransferStartAddr (void)
{
	return(1L);
}

unsigned long SpuWritePartly (unsigned char *addr, unsigned long size)
{
	return(1L);
}


long SpuIsTransferCompleted (long flag)
{
	return(1L);
}


long SpuReadDecodeData (SpuDecodeData *d_data, long flag)
{
	return(1L);
}
	

long SpuSetIRQ (long on_off)
{
	return(1L);
}

long SpuGetIRQ (void)
{
	return(1L);
}

unsigned long SpuSetIRQAddr (unsigned long fred)
{
	return(1L);
}

unsigned long SpuGetIRQAddr (void)
{
	return(1L);
}

void SpuSetIRQCallback (SpuIRQCallbackProc fred)
{
}
#endif //USE_ATI


void SpuSetVoiceAttr (SpuVoiceAttr *arg)
{
}

#ifndef USE_ATI

void SpuGetVoiceAttr (SpuVoiceAttr *arg)
{
}

void SpuSetKey (long on_off, unsigned long voice_bit)
{
}

void SpuSetKeyOnWithAttr (SpuVoiceAttr *attr)
{
}

long SpuGetKeyStatus (unsigned long voice_bit)
{
	return(1L);
}

void SpuGetAllKeysStatus (char *status)
{
}


unsigned long SpuSetPitchLFOVoice (long on_off, unsigned long voice_bit)
{
	return(1L);
}

unsigned long SpuGetPitchLFOVoice (void)
{
	return(1L);
}
#endif //USE_ATI


long SpuSetCommonAttr (SpuCommonAttr *attr)
{
	return(1L);
}

#ifndef USE_ATI

void SpuGetCommonAttr (SpuCommonAttr *attr)
{
}


long SpuInitMalloc (long num, char *top)
{
	return(1L);
}

long SpuMalloc (long size)
{
	return(1L);
}

long SpuMallocWithStartAddr (unsigned long addr, long size)
{
	return(1L);
}

void SpuFree (unsigned long addr)
{
}

long	CdIntToPos(long fred, CdlLOC* george)
{
	 return(1L);
}

long CdPosToInt(CdlLOC* george)
{
		return(1L);
}

void	CdSetDebug(int fred)
{
	
}


#endif //USE_ATI
