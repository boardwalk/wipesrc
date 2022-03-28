typedef struct
{
   short             tex16 [ 16 ];
	short					tex4  [ 4 ];
	short					tex1  [ 1 ];
} TexTemplate;



TexTemplate* LoadTtfFile( char *file, short *count, short libraryTextures );
