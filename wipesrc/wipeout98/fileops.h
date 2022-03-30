#include <stdint.h>

typedef struct _TIMlist
{
	void *memBase;
	uintptr_t	memOffset[400];
} TIMlist;

extern TIMlist *LoadCompressedTextureSequence( char *texName );
