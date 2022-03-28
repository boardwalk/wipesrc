/* Source File: padread.c */
#include	"..\markk\libetc.h"

long	PadRead(unsigned short id)
{
	/* Select appropriate register from possible PC devices(given id).
		Mat need to poll some devices for latest values. */
	/* return Device's Register value; */
	return (~etc_KeyboardRegister);
}
