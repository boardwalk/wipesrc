#include				<sys/types.h>

#include "datacash.h"
#include "wintime.h"

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




extern long 			TimerInstalled;
extern void				UserAYInt(void);
extern unsigned long	this_frame, last_frame;


void InstallTimer(void)
{
//	TimerInstalled = 1;	// mark

	if (!TimerInstalled)
	{
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

		backup_cache = (char *)make_data_cache();

		InitialiseWinTimer();
		TimerInstalled = 1;
	}
}



void RemoveTimer(void)
{
	EndWinTimer();
}

char	old_mask;
