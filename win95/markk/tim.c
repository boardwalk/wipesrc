#include <sys\types.h>
#include "..\daves\libgte.h"
#include "c:\watcom\h\dos.h"
#include "c:\watcom\h\conio.h"
#include "c:\watcom\h\i86.h"

extern long frameRate30;

int		processing_interupt = 0;
char	*INT_STACK;
long	stack_addr;
short	stack_select;

long 	old_stack;
short	old_stack_select;



#pragma aux		SET_ES=	   \
		"push	es"	\
		"push	ds" \
		"pop	es" \
		parm [];

#pragma aux		POP_ES=	  \
		"pop	es" \
		parm [];

#if 0
#pragma aux		PUSH_ALL=	\
		"push eax" \
		"push ebx" \
		"push ecx" \
		"push edx" \
		"push esi" \
		"push edi" \
		"push ebp" \
		"pushfd"	   \
		parm [];
extern	void PUSH_ALL(void);
extern	void POP_ALL(void);

#pragma aux		PUSH_ALL parm[];
#pragma aux		POP_ALL parm[];

#pragma	aux		POP_ALL=	\
		"popfd"	  \
		"pop ebp" \
		"pop edi" \
		"pop esi" \
		"pop edx" \
		"pop ecx" \
		"pop ebx" \
		"pop eax" \
		parm [];

#pragma aux		PUSH_ALL=	\
		"pushad" \
		"pushfd"	   \
		parm [];

#pragma	aux		POP_ALL=	\
		"popfd"	  \
		"popad" \
		parm [];

#else

extern	char	fpstate;

#pragma aux		PUSH_ALL=	\
		"pushad" \
		"pushfd"	   \
		"fsave	[fpstate]"	\
		parm [];

#pragma	aux		POP_ALL=	\
		"frstor	[fpstate]"	\
		"popfd"	  \
		"popad" \
		parm [];

#endif

#pragma aux		STORE_STACK=	\
		"push	eax"				\
		"mov	ax,ss"				\
		"mov	[stack_select], ax"\
		"pop	eax"				\
		parm [];

#pragma aux		SWAP_STACK=				\
		"mov	eax,esp"				\
		"mov	[old_stack],eax"			\
		"mov	ax,ss"					\
		"mov	[old_stack_select], ax" 	\
		"cli"							\
		"mov	eax,[stack_addr]"		\
		"mov	bx,[stack_select]"		\
		"mov	ss,bx"					\
		"nop"							\
		"nop"							\
		"nop"							\
		"nop"							\
		"mov	esp,[stack_addr]"		\
		"sti"							\
		modify	[eax ebx] 				\
		parm [];

#pragma aux		RESTORE_STACK=			\
		"cli"							\
		"mov	ax,[old_stack_select]"	\
		"mov	ss,ax"					\
		"nop"							\
		"nop"							\
		"nop"							\
		"nop"							\
		"mov	eax,[old_stack]"			\
		"mov	esp,eax"				\
		"sti"							\
		modify	[eax]					\
		parm [];

/*GTE externals*/
extern MATRIX			LCM;			// local colour matrix
extern SVECTOR			LLV[3];			// local lighting vector
extern MATRIX			LLM;			// local lighting matrix
extern GTECVECTOR		BK;				// background colour
extern GTECVECTOR		FC;				// far colour
extern long			FFAR;		// fog far
extern long			FNEAR;		// fog near
extern long			xOffset;	// geometry offset
extern long			yOffset;
extern long			screenZ;	// geometry screen Z
extern long			SCR2;		// geometry screen Z / 2
extern MATRIX 			RTM;			// 1 current rotation matrix
extern GTECVECTOR 		grgb[3];		// 3 current colour values
extern long			gsxy[3];	// 3 current screen coordinates
extern long			gsz[4];		// 4 current screen z corrdinates
extern MATRIX			stack[20];		// 20 rotation matricies for stack
extern long			stackPtr;

/*backup variables*/
MATRIX			LCMbackup;			// local colour matrix
SVECTOR			LLVbackup[3];			// local lighting vector
MATRIX			LLMbackup;			// local lighting matrix
GTECVECTOR		BKbackup;				// background colour
GTECVECTOR		FCbackup;				// far colour
long			FFARbackup;		// fog far
long			FNEARbackup;		// fog near
long			xOffsetbackup;	// geometry offset
long			yOffsetbackup;
long			screenZbackup;	// geometry screen Z
long			SCR2backup;		// geometry screen Z / 2
MATRIX 			RTMbackup;			// 1 current rotation matrix
GTECVECTOR 		grgbbackup[3];		// 3 current colour values
long			gsxybackup[3];	// 3 current screen coordinates
long			gszbackup[4];		// 4 current screen z corrdinates
MATRIX			stackbackup[20];		// 20 rotation matricies for stack
long			stackPtrbackup;

/*loop counter, made global as I don't trust __interupt with the stack*/
int varbackloop;


void	(__interrupt __far *prev_int_1c)();

extern long TimerInstalled;
extern void	UserAYInt(void);

void __interrupt __far timer_rtn(union INTPACK	r)
{
PUSH_ALL();
SET_ES();
SWAP_STACK();


if (processing_interupt)
	{
	return;			//We're running too slow
	}
processing_interupt = 1;


allow_IRQ1();
/*backup the state of the GTE control regs*/
LCMbackup = 		LCM;		
LLMbackup = 		LLM;		
BKbackup = 		  	BK;		
FCbackup = 		  	FC;		
FFARbackup = 		FFAR;		
FNEARbackup = 	  	FNEAR;	
xOffsetbackup = 	xOffset;	
yOffsetbackup = 	yOffset;
screenZbackup = 	screenZ;	
SCR2backup = 		SCR2;		
RTMbackup = 		RTM;		
stackPtrbackup = 	stackPtr;
for (varbackloop = 0; varbackloop < 3; varbackloop++)
  {
	LLVbackup[varbackloop] = 	  LLV[varbackloop];	
  }
for (varbackloop = 0; varbackloop < 3;varbackloop++ )
  {
	grgbbackup[varbackloop] = 	  grgb[varbackloop];	
  }
for (varbackloop = 0; varbackloop < 3;varbackloop++ )
  {
	gsxybackup[varbackloop] = 	  gsxy[varbackloop];	
  }
for (varbackloop = 0; varbackloop < 4;varbackloop++ )
  {
	gszbackup[varbackloop] = 	  gsz[varbackloop];	
  }
for (varbackloop = 0; varbackloop < 20;varbackloop++ )
  {
	stackbackup[varbackloop] =   stack[varbackloop];
  }


	// function call here
   	UserAYInt();



/*restore the state of the GTE control reg*/
LCM = 		LCMbackup;		
LLM = 		LLMbackup;		
BK = 		BKbackup;		
FC = 		FCbackup;		
FFAR = 		FFARbackup;		
FNEAR = 	FNEARbackup;	
xOffset = 	xOffsetbackup;	
yOffset = 	yOffsetbackup;
screenZ = 	screenZbackup;	
SCR2 = 		SCR2backup;		
RTM = 		RTMbackup;		
stackPtr = 	stackPtrbackup;
for (varbackloop = 0; varbackloop < 3; varbackloop++)
  {
	LLV[varbackloop] = 	  LLVbackup[varbackloop];	
  }
for (varbackloop = 0; varbackloop < 3;varbackloop++ )
  {
	grgb[varbackloop] = 	  grgbbackup[varbackloop];	
  }
for (varbackloop = 0; varbackloop < 3;varbackloop++ )
  {
	gsxy[varbackloop] = 	  gsxybackup[varbackloop];	
  }
for (varbackloop = 0; varbackloop < 4;varbackloop++ )
  {
	gsz[varbackloop] = 	  gszbackup[varbackloop];	
  }
for (varbackloop = 0; varbackloop < 20;varbackloop++ )
  {
	stack[varbackloop] =   stackbackup[varbackloop];
  }







restore_IRQ0();

processing_interupt = 0;

RESTORE_STACK();	
POP_ES();
POP_ALL();
    _chain_intr(prev_int_1c);	// or you can EOI
	
//	outp(0x20, 0x20);	// EOI
}

void InstallTimer(void)
{
	if (!TimerInstalled)
	{
		INT_STACK = (char *)malloc(100*1024);	//100k stack
		stack_addr = (long)INT_STACK + (100*1024);
		STORE_STACK();

		processing_interupt = 0;

		outp(0x43, 0x36);
		outp(0x40, (1193180 / frameRate30) & 0xFF);
		outp(0x40, (1193180 / frameRate30)/256);

		TimerInstalled = 1;
	    prev_int_1c = _dos_getvect(0x1c);
	    _dos_setvect(0x1c, timer_rtn);
	}
}


void RemoveTimer(void)
{
	if (TimerInstalled)
	{
		TimerInstalled = 0;
  	_dos_setvect(0x1c, prev_int_1c);
	}

}

char	old_mask;

void	allow_IRQ1(void)
{
		old_mask = inp(0x21);	// save old IMR
		outp(0x20, 0x68);		// set set Special Mask Mode
		outp(0x21, 0xFD);		// enable IRQ1
}

void	restore_IRQ0(void)
{
		outp(0x21, old_mask);
		outp(0x20, 0x48);
}
