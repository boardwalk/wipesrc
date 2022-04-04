#ifndef TTF_H
#define TTF_H

#include <stdint.h>

typedef struct
{
   int16_t             tex16 [ 16 ];
	int16_t					tex4  [ 4 ];
	int16_t					tex1  [ 1 ];
} TexTemplate;



TexTemplate* LoadTtfFile( char *file, int16_t *count, int16_t libraryTextures );

#endif
