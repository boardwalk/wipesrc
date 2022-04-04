#include				<sys/types.h>

#include "datacash.h"
#include "wintime.h"

#ifdef WIPEOUTPC
char					*backup_cache;
#endif
int32_t						processing_interupt = 0;
char					*INT_STACK;
int32_t					stack_addr;
int16_t					stack_select;
int32_t 					old_stack;
int16_t					old_stack_select;
char					imr_state;
int32_t					TimerInstalled = 0;
extern int32_t				frameRate30;
extern uintptr_t	FastRam;	// ajy
extern char				fpstate;



extern int32_t				FR30;
extern int32_t				FR60;
extern int32_t				FR50;
extern int32_t				FR6;
extern int32_t				FR10;
extern int32_t				FR40;
extern int32_t				FR50U;
extern int32_t				FR15U;
extern int32_t				FR30U;
extern int32_t				FR7U;
extern int32_t				FR32U;




extern int32_t 			TimerInstalled;
extern void				UserAYInt();
extern uint32_t	this_frame, last_frame;


void InstallTimer()
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



void RemoveTimer()
{
	EndWinTimer();
}

char	old_mask;
