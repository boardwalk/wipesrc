#ifndef TILELIB_H
#define TILELIB_H

/* Maximum number of faces in a section */
#define        MaxFaces       7

/* Maximum number of sections to draw */
#define        MaxLo          100
#define        MaxMed         4
#define        MaxHi          4

/* no more than this many textures */
#define        TileLibMax     24

/* no more than this many object pointers in a TileArray */
#define        TileArrayMax   ( MaxLo * MaxFaces * 3 )


typedef struct TileArray
{
   char*       primitives[ TileArrayMax ];
   int16_t       count;
   int16_t       current;
} TileArray;


typedef struct TileLib
{
   TileArray*  library[ TileLibMax ][ 3 ][ 2 ];
   int16_t       count;
} TileLib;



TileLib* NewTileLib( );
TileArray* NewTileArray( );
void AddTileArrayToLibrary( TileLib* tileLib, TileArray* tileArray1, TileArray* tileArray2, TileArray* tileArray3, int16_t reversed );
void AddTileToLibrary( TileLib* tileLib, int16_t lib, int16_t res, char* prim, int16_t reversed );
int16_t CanTileFitInLibrary( TileLib* tileLib, int16_t lib, int16_t res, int16_t reversed );
void ResetTileLibrary( TileLib* tileLib );
char* GetTileObject( TileLib* tileLib, int16_t index, int16_t res, int16_t reversed );

#endif
