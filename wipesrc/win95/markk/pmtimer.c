/* Protected Mode temporary timer interrupt to simulate VBL. */
#include	<stdio.h>
#include	<dos.h>
#include	"libetc.h"

#pragma off (stack_check)
void __interrupt __far __loadds	pmtimer(void)
{
	/* Call Joystick Handler if available. */
	if(etc_JoystickService != NULL)
	{	
		_enable();
		(*etc_JoystickService)();	
		_disable();
	}

	/* Chain to original interrupt. */
	_chain_intr(old_timer);
}
#pragma on (stack_check)


