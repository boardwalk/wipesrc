typedef struct _TIMlist
{
	void *memBase;
	long	memOffset[400];
} TIMlist;

extern TIMlist *LoadCompressedTextureSequence( char *texName );
