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
   short       count;
   short       current;
} TileArray;


typedef struct TileLib
{
   TileArray*  library[ TileLibMax ][ 3 ][ 2 ];     
   short       count;
} TileLib;



TileLib* NewTileLib( );
TileArray* NewTileArray( );
void AddTileArrayToLibrary( TileLib* tileLib, TileArray* tileArray1, TileArray* tileArray2, TileArray* tileArray3, short reversed );
void AddTileToLibrary( TileLib* tileLib, short lib, short res, char* prim, short reversed );
short CanTileFitInLibrary( TileLib* tileLib, short lib, short res, short reversed );
void ResetTileLibrary( TileLib* tileLib );
char* GetTileObject( TileLib* tileLib, short index, short res, short reversed );
