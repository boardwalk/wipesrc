#include	<dos.h>
#include	"libetc.h"

void	init_timer(void)
{
	/* Save Original Timer Interrupt. */
	old_timer = _dos_getvect(_TIMER_INT);

	/* Install New Timer Interrupt. */
	_dos_setvect(_TIMER_INT, pmtimer);

	etc_TimerHandler = -1;
}

void	restore_timer(void)
{
	/* Restore Original Timer Interrupt. */
	_dos_setvect(_TIMER_INT, old_timer);
	etc_TimerHandler = 0;
}
