#include				<sys\types.h>
#include				"..\daves\libgte.h"

#include				"c:\wc10\h\dos.h"
#include				"c:\wc10\h\conio.h"

#ifdef WIPEOUTPC
char					*backup_cache;
#endif
int						processing_interupt = 0;
char					*INT_STACK;
long					stack_addr;
short					stack_select;
long 					old_stack;
short					old_stack_select;
char					imr_state;
long					TimerInstalled = 0;
extern long				frameRate30;
extern unsigned long	FastRam;	// ajy 
extern char				fpstate;

extern void		SET_ES(void);
#pragma aux		SET_ES	= \
						"push	es"	\
						"push	ds" \
						"pop	es" \
						parm [];

extern void		POP_ES(void);
#pragma aux		POP_ES	= \
						"pop	es" \
						parm [];


#if 0
extern	void	PUSH_ALL(void);
#pragma aux		PUSH_ALL = \
						"push eax" \
						"push ebx" \
						"push ecx" \
						"push edx" \
						"push esi" \
						"push edi" \
						"push ebp" \
						"pushfd"   \
						parm [];

extern	void	POP_ALL(void);
#pragma	aux		POP_ALL	= \
						"popfd"	  \
						"pop ebp" \
						"pop edi" \
						"pop esi" \
						"pop edx" \
						"pop ecx" \
						"pop ebx" \
						"pop eax" \
						parm [];

#pragma aux		PUSH_ALL = \
						"pushad" \
						"pushfd" \
						parm [];

#pragma	aux		POP_ALL	 = \
						"popfd"	\
						"popad" \
						parm [];
#else


#pragma aux		PUSH_ALL = 								\
						"pushad" 						\
						"pushfd" 						\
						"fsave	[fpstate]" 				\
						parm [];

#pragma	aux		POP_ALL = 								\
						"frstor	[fpstate]"				\
						"popfd"	  						\
						"popad" 						\
						parm [];
#endif

#pragma aux		STORE_STACK = 							\
						"push	eax" 					\
						"mov	ax,ss" 					\
						"mov	[stack_select], ax" 	\
						"pop	eax" 					\	
						parm [];

#pragma aux		SWAP_STACK = 							\
						"mov	eax,esp" 				\
						"mov	[old_stack],eax"		\
						"mov	ax,ss"					\
						"mov	[old_stack_select], ax" \
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

#pragma aux		RESTORE_STACK =							\
						"cli"							\
						"mov	ax,[old_stack_select]"	\
						"mov	ss,ax"					\
						"nop"							\
						"nop"							\
						"nop"							\
						"nop"							\
						"mov	eax,[old_stack]"		\
						"mov	esp,eax"				\
						"sti"							\
						modify	[eax]					\
						parm [];

extern long				FR30;
extern long				FR60;
extern long				FR50;
extern long				FR6;
extern long				FR10;
extern long				FR40;
extern long				FR50U;
extern long				FR15U;
extern long				FR30U;
extern long				FR7U;
extern long				FR32U;


/*GTE externals*/
extern MATRIX			LCM;			// local colour matrix
extern SVECTOR			LLV[3];			// local lighting vector
extern MATRIX			LLM;			// local lighting matrix
extern GTECVECTOR		BK;				// background colour
extern GTECVECTOR		FC;				// far colour
extern long				FFAR;		// fog far
extern long				FNEAR;		// fog near
extern long				xOffset;	// geometry offset
extern long				yOffset;
extern long				screenZ;	// geometry screen Z
extern long				SCR2;		// geometry screen Z / 2
extern MATRIX 			RTM;			// 1 current rotation matrix
extern GTECVECTOR 		grgb[3];		// 3 current colour values
extern long				gsxy[3];	// 3 current screen coordinates
extern long				gsz[4];		// 4 current screen z corrdinates
extern MATRIX			stack[20];		// 20 rotation matricies for stack
extern long				stackPtr;

/*backup variables*/
MATRIX					LCMbackup;			// local colour matrix
SVECTOR					LLVbackup[3];			// local lighting vector
MATRIX					LLMbackup;			// local lighting matrix
GTECVECTOR				BKbackup;				// background colour
GTECVECTOR				FCbackup;				// far colour
long					FFARbackup;		// fog far
long					FNEARbackup;		// fog near
long					xOffsetbackup;	// geometry offset
long					yOffsetbackup;
long					screenZbackup;	// geometry screen Z
long					SCR2backup;		// geometry screen Z / 2
MATRIX 					RTMbackup;			// 1 current rotation matrix
GTECVECTOR 				grgbbackup[3];		// 3 current colour values
long					gsxybackup[3];	// 3 current screen coordinates
long					gszbackup[4];		// 4 current screen z corrdinates
MATRIX					stackbackup[20];		// 20 rotation matricies for stack
long					stackPtrbackup;

/*loop counter, made global as I don't trust __interupt with the stack*/
int						varbackloop;
void 					(__interrupt __far *prev_int_1c)();

extern long 			TimerInstalled;
extern void				UserAYInt(void);
extern unsigned long	this_frame, last_frame;

#ifndef USE_ATI
//#pragma	off	(check_stack);
void __interrupt __far		timer_rtn(union INTPACK	r)
{
#ifdef TIMER_ON
	PUSH_ALL();
	SET_ES();
	SWAP_STACK();

	set_special_mask_mode();
	disable_timer_mask();

	this_frame++;

	if (processing_interupt)
	{
		RESTORE_STACK();	
		POP_ES();
		POP_ALL();
		enable_timer_mask();
		clear_special_mask_mode();
		return;			//We're running too slow
	}
	//allow_IRQ1();

	TimerIntRoutine();

	enable_timer_mask();
	clear_special_mask_mode();

	RESTORE_STACK();
	POP_ES();
	POP_ALL();

	_chain_intr(prev_int_1c);	// or you can EOI
	//	outp(0x20, 0x20);	// GENERIC EOI
#endif
}
//#pragma	on	(check_stack);
#endif //USE_ATI

#ifndef USE_ATI
void TimerIntRoutine(void)
{
	this_frame++;

	if (processing_interupt)
	{
//		QuitToDos();
		return;			//We're running too slow
	}
	processing_interupt = 1;

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

	#ifdef WIPEOUTPC
	  memcpy(backup_cache,FastRam,1024);
	#endif

	// function call here
   	UserAYInt();

	#ifdef WIPEOUTPC
	  memcpy(FastRam,backup_cache,1024);
	#endif

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

	//restore_IRQ0();

	processing_interupt = 0;
}
#endif //USE_ATI


void InstallTimer(void)
{
//	TimerInstalled = 1;	// mark	

	if (!TimerInstalled)
	{
#ifdef WIPEOUTPC
		FR30 = frameRate30;
		FR60 = frameRate30*2;
		FR50 = (frameRate30*50)/30;
		FR6 = (frameRate30*6)/30;
		FR10 = frameRate30/3;
		FR40 = (frameRate30*40)/30;

		FR50U = (50*30)/frameRate30;
		FR15U = (15*30)/frameRate30;
		FR30U = (30*30)/frameRate30;
		FR7U = (7*30)/frameRate30;
		FR32U = (32*30)/frameRate30;
#endif

#ifdef DOS_TIMER_ON
		INT_STACK = (char *)malloc(100*1024);	//100k stack
		stack_addr = (long)INT_STACK + (100*1024);
		STORE_STACK();

#ifdef WIPEOUTPC

		backup_cache = (char *)make_data_cache();
#endif


		processing_interupt = 0;

//		outp(0x20, 0xc2);

		outp(0x43, 0x36);
		outp(0x40, (1193180 / frameRate30) & 0xFF);
		outp(0x40, (1193180 / frameRate30)/256);

		TimerInstalled = 1;
	    prev_int_1c = _dos_getvect(0x1c);
	    _dos_setvect(0x1c, timer_rtn);
#else
#ifdef WIPEOUTPC

		backup_cache = (char *)make_data_cache();
#endif
	InitialiseWinTimer();
	TimerInstalled = 1;
#endif
	}
}


#ifndef USE_ATI
void InstallSerial(void)
{
#ifdef TIMER_ON
	if (!TimerInstalled)
	{
		INT_STACK = (char *)malloc(100*1024);	//100k stack
		stack_addr = (long)INT_STACK + (100*1024);
		STORE_STACK();

		processing_interupt = 0;

//		outp(0x20, 0xc2);
//		outp(0x43, 0x36);
//		outp(0x40, (1193180 / frameRate30) & 0xFF);
//		outp(0x40, (1193180 / frameRate30)/256);

		TimerInstalled = 1;
	    prev_int_1c = _dos_getvect(0x1c);
	    _dos_setvect(0x1c, timer_rtn);
	}
#endif
}
#endif //USE_ATI



void RemoveTimer(void)
{
#ifdef DOS_TIMER_ON
	if (TimerInstalled)
	{
		TimerInstalled = 0;
#ifdef WIPEOUTPC
		free(backup_cache);
#endif
  	_dos_setvect(0x1c, prev_int_1c);
//		outp(0x20, 0xc7);
	}
#else	//Assume Win'95
	EndWinTimer();
#endif
}

char	old_mask;

#ifdef TIMER_ON
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

void	set_special_mask_mode(void)
{
		outp(0x20, 0x68);		// Activate Special Mask Mode
}

void	disable_timer_mask(void)
{

		imr_state = inp(0x21);
		outp(0x21, imr_state | 0x01);	// disable IRQ0
}

void	clear_special_mask_mode(void)
{
		outp(0x20, 0x48);		// Restore Interrupt Controller to normal mode
}

void	enable_timer_mask(void)
{
		imr_state = inp(0x21);
		outp(0x21, imr_state & 0xFE);	// enable IRQ0
}

#endif
