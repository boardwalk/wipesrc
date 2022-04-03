#define DBG	1
#define WIN32_LEAN_AND_MEAN

#define ERR_TIMER	1
#define ERR_NOERROR	0

#include <windows.h>
#include <mmsystem.h>


#define TARGET_RESOLUTION 1 //1mS base freq 33mS timer = ~30Hz
#define TIMER_RESOLUTION 1 //1mS base freq 33mS timer = ~30Hz

#define	PAD_STACK_SIZE	60

int32_t	NeedKeys = 0;
uint16_t	pad_stack[PAD_STACK_SIZE];
int32_t		Curpad = 0;
extern uint16_t pad;

extern int32_t frameRate30;
extern void TimerIntRoutine();
extern void PASCAL OneShotTimer(UINT wTimerID, UINT msg,
								DWORD dwUser, DWORD dw1, DWORD dw2) ;
extern int32_t SetTimerCallback(int32_t *cttimer,  UINT msInterval);

int32_t	ctrlTimer;
TIMECAPS tc;
UINT     wTimerRes;
int32_t	frame_count = 0;
void DestroyTimer(int32_t *cttimer);

void InitialiseWinTimer()
{
	if(timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) {
		/* Error; application can't continue. */
	}

	wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);

	timeBeginPeriod(wTimerRes); //tick the windows timer off at 1mS
	SetTimerCallback(&ctrlTimer, TIMER_RESOLUTION); //Lets have our callback every 30Hz
}

void EndWinTimer ()
{
	DestroyTimer(&ctrlTimer);
	timeEndPeriod(wTimerRes);
}


int32_t SetTimerCallback(int32_t *cttimer,  // global timernumber
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
    int32_t	*cttimer;

	cttimer = (int32_t *)dwUser;
    *cttimer = 0;
	SetTimerCallback(cttimer, TIMER_RESOLUTION); //set it off again
	if (frame_count++ > 33)	//Drive off 1mS timer
	{
		frame_count = 0;
	}

}
void DestroyTimer(int32_t *cttimer)
{
    if (*cttimer) {      /* If timer event is pending */
        timeKillEvent(*cttimer);  /* Cancel the event */
        *cttimer = 0;
    }
}

int32_t HowLong()
{
	return((int32_t) timeGetTime());
}
