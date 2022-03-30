typedef struct _TIMlist
{
	void *memBase;
	intptr_t	memOffset[400];
} TIMlist;

extern TIMlist *LoadCompressedTextureSequence( char *texName );
