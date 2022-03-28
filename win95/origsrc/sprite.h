typedef struct Sprites
{
	VECTOR 			pos;
   POLY_FT4       sprite[ 100 ][ 2 ];
   Skeleton       *skeleton;
} Sprites;



void InitSprites( Sprites *sprites, Skeleton *camPos );
void AddSprites( Sprites *sprites, TrackSection *section );
void UpdateSprites( Sprites *sprites, TrackSection *section, short hdg );
