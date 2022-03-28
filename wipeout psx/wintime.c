#define DBG	1
#define WIN32_LEAN_AND_MEAN

#define ERR_TIMER	1
#define ERR_NOERROR	0

#include <windows.h>
#include <mmsystem.h>


#define TARGET_RESOLUTION 1 //1mS base freq 33mS timer = ~30Hz
#define TIMER_RESOLUTION 1 //1mS base freq 33mS timer = ~30Hz

#define	PAD_STACK_SIZE	60

long	NeedKeys = 0;
unsigned short	pad_stack[PAD_STACK_SIZE];
int		Curpad = 0;
extern unsigned short pad;

extern long frameRate30;
extern void TimerIntRoutine(void);
extern void PASCAL OneShotTimer(UINT wTimerID, UINT msg, 
				    DWORD dwUser, DWORD dw1, DWORD dw2) ;
extern int SetTimerCallback(long *cttimer,  UINT msInterval);
#ifndef USE_ATI
extern void KeyBoardTimerRoutine(void);
#endif //USE_ATI

long	ctrlTimer;
TIMECAPS tc; 
UINT     wTimerRes; 
long	frame_count = 0;
void DestroyTimer(long *cttimer); 

void InitialiseWinTimer(void)
{
	if(timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) { 
	    /* Error; application can't continue. */ 
	} 
	 
	wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax); 
	 
	timeBeginPeriod(wTimerRes); //tick the windows timer off at 1mS
	SetTimerCallback(&ctrlTimer, TIMER_RESOLUTION); //Lets have our callback every 30Hz
}
 
void EndWinTimer (void)
{
DestroyTimer(&ctrlTimer);
timeEndPeriod(wTimerRes); 
}


int SetTimerCallback(long *cttimer,  // global timernumber
    UINT msInterval)                // event interval
{ 
    *cttimer = timeSetEvent(
        msInterval,                    // delay
        wTimerRes,                     // resolution (global variable)
        OneShotTimer,               // callback function
        (DWORD) cttimer,                  		// just need timer ID to kill it
        TIME_ONESHOT );                // single timer event

    if(! *cttimer)
        return ERR_TIMER;
    else
        return ERR_NOERROR;
} 

void PASCAL OneShotTimer(UINT wTimerID, UINT msg, 
    DWORD dwUser, DWORD dw1, DWORD dw2) 
{ 
    long	*cttimer;
 
	cttimer = (long *)dwUser;
    *cttimer = 0;
	SetTimerCallback(cttimer, TIMER_RESOLUTION); //set it off again
	if (frame_count++ > 33)	//Drive off 1mS timer
		{
		frame_count = 0;
//	    KeyBoardTimerRoutine(); /* Handle interrupt-time tasks */ 
		}
 
} 
void DestroyTimer(long *cttimer) 
{ 
    if (*cttimer) {      /* If timer event is pending */ 
        timeKillEvent(*cttimer);  /* Cancel the event */ 
        *cttimer = 0; 
    } 
}

long HowLong(void)
{
	return((long) timeGetTime());
}

#ifndef USE_ATI
void KeyBoardTimerRoutine(void)
{
	NeedKeys = 1;
ProcessMessages();
	pad_stack[Curpad++] = pad;
	if (Curpad >= PAD_STACK_SIZE)
		Curpad = PAD_STACK_SIZE - 1; //Overflow
	NeedKeys = 0;	
}
#endif //USE_ATI
