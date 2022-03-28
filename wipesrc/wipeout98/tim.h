/* (C) Psygnosis 1994. By Jason Carl Denton
*/

/* DEFINES FOR DRAWING PRIMITIVE */

#define	POLYGON		0x20
#define	LINE			0x40
#define	SPRITE		0x60

#define	GOURAUD		0x10
#define	QUAD			0x08
#define	TEXTURE		0x04
#define	TRANSPARENT	0x02
#define	NOBRIGHT		0x01

#define	DOUBLE		0x02
#define	NOLIGHT		0x01

#define	CLUT4			0x00
#define	CLUT8			0x01
#define	TRUE15		0x10


typedef struct
{
	short			type;
	short			tsb;
	short			cba;
	short			clutX;
	short			clutY;
	short			clutW;
	short			clutH;
	short			textureX;
	short			textureY;
	short			textureW;
	short			textureH;
	short			u0;
	short			v0;
	short			u1;
	short			v1;
	short			u2;
	short			v2;
	short			u3;
	short			v3;
} Texture;


typedef struct Tim
{
/* Header */

   long     id;
   
/* Flags */

   long     flags;

} Tim;

typedef struct TimClut8
{
/* Header */

   long     id;
   
/* Flags */

   long     flags;

/* Clut */

   long     clutBytes;

   short    clutX;
   short    clutY;
   short    clutW;
   short    clutH;

   short    clut[ 256 ];

/* Tetxure */
   
   long     textureBytes;

   short    textureX;
   short    textureY;
   short    textureW;
   short    textureH;

} TimClut8;

typedef struct TimClut4
{
/* Header */

   long     id;
   
/* Flags */

   long     flags;

/* Clut */

   long     clutBytes;

   short    clutX;
   short    clutY;
   short    clutW;
   short    clutH;

   short    clut[ 16 ];

/* Tetxure */
   
   long     textureBytes;

   short    textureX;
   short    textureY;
   short    textureW;
   short    textureH;

} TimClut4;



#define  Clut4Bit    0
#define  Clut8Bit    1
#define  Clut15Bit   2

#define  NoClut      0
#define  HasClut     1

#define  TPAGE(x,y)  ( ( x >> 6 ) + ( ( y & 0x100 ) >> 4 ) )
#define	CBA(x,y)		( ( y << 6 ) | x )
#define	TSB(c,a,t)	( ( c << 7 ) | ( a << 5 ) | t )


#define  TextureHOffset(x) ( x & 0xffc0 )
#define  TextureVOffset(y) ( y & 0x100 )

#define  ClutType(t)       ( t->flags & 7 )


/* Forward Procedure Declarations */

Texture* LoadTexture( char* name, short translucent );
