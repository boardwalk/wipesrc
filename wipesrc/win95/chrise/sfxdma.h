/***************************************************
*
* Sfxdma.h
*
****************************************************
*
*	Header file for sfxdma.c
* Contains function prototypes for DMA data transfer
* functions
*
****************************************************
*
* $Log:   P:/ridger/ridger/SFXDMA.H_v  $
 * 
 *    Rev 1.0   13 Jul 1995 14:40:38   DaveS
 * Put Back after we stopped being naughty children
 * 
 *    Rev 1.0   18 Nov 1994 15:25:18   AndrewC
 * Initial revision.
*
***************************************************/

/*
 * #defines : transfer mode
 */

#define SINGLE 		0
#define AUTO			1


void LoDmaReadSet(uint32 dwStart,uint16 wLen, uint16 wChnnl,uint16 wAuto);
void HiDmaReadSet(uint32 dwStart,uint16 wLen, uint16 wChnnl,uint16 wAuto);
void LoDmaWriteSet(uint32 dwStart,uint16 wLen, uint16 wChnnl,uint16 wAuto);
void *DosMemAlloc(uint32 dwSize, uint32 *pdwSelector);
uint16 DosMemFree(uint32 dwSelector);
void *RecursiveAlloc(uint32 dwSize,uint32 *pdwSelector);
