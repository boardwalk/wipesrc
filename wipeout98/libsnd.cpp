/************************************************************************
*									*
*	PlayStation -> PC Conversion					*
*									*
*  	Sound Library (libsnd) Dummy Version - Da Code	 		*
*									*
*	Lovingly coded for you by Captain Bodger and Patchman		*
*									*
*		March 95						*
*									*
*************************************************************************

/************************************************************************
*									*
*			Les Includes					*
*									*
************************************************************************/

#include <stdio.h>
#include "\wipesrc\psx26\include\libspu.h"
#include "\wipesrc\psx26\include\libsnd.h"

/************************************************************************
*									*
*			Los Functions					*
*									*
************************************************************************/
#ifndef USE_ATI


extern short SsVabTransCompleted (short Arse)
 {
  return(1);
 }

/***********************************************************************/

extern short SsVabOpenHead (unsigned char *Bollocks, short Arse)
 {
  return(0);
 }

/***********************************************************************/

extern short SsVabTransBody (unsigned char *Bollocks, short Arse)
 {
  return(0);
 }

/***********************************************************************/
extern void  SsInit (void)
 {
  return;
 }

/***********************************************************************/

extern void  SsSetTableSize (char *DontKnow, short DontCare, short WhoCares)
 {
  return;
 }

/***********************************************************************/

extern void  SsSetTickMode (long tick_mode)
 {
  return;
 }

/***********************************************************************/

extern void  SsStart (void)
 {
  return;
 }

/***********************************************************************/

extern void  SsSeqCalledTbyT (void)           
 {
  return;
 }

/***********************************************************************/

extern void  SsSetMVol (short Something, short SomethingElse)
 {
  return;
 }

/***********************************************************************/

extern void  SsSetSerialAttr (char Rubbish, char Trash, char Garbage)
 {
  return;
 }     

/***********************************************************************/

extern void  SsSetSerialVol (char Nothing, short Nowt, short Zip)
 {
  return;    
 }

/***********************************************************************/

extern short SsUtKeyOn (short A, short B, short C, short D, short E, short F,
		short G)
 {
  return(0);
 }

/***********************************************************************/

extern short SsUtKeyOff (short A, short B, short C, short D, short E)
 {
  return(0);
 }

/***********************************************************************/

extern short SsUtChangePitch (short A, short B, short C, short D, short E,
                              short F, short G)
 {
  return(0);
 }

/***********************************************************************/

extern short SsUtSetVVol (short Something, short SomethingElse, short Arse)
 {
  return(0);
 }

/***********************************************************************/

extern void  SsUtReverbOn (void)
 {
  return;
 }

/***********************************************************************/

extern void  SsUtReverbOff (void)
 {
  return;
 }

/***********************************************************************/

extern short SsUtSetReverbType (short Arse)
 {
  return(Arse);
 }

/***********************************************************************/

extern void  SsUtSetReverbDepth (short Something, short SomethingElse)
 {
  return;
 }

/***********************************************************************/


#endif //USE_ATI
