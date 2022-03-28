typedef struct TexPlace
{
   short       libraryTextures;              /* index into library texture table */
   short       emptyTexture;                 /* the 'empty' texture */

   POLY_GT4    textures[ 2 ][ 8 ];           /* the actual icons */
   POLY_G4     highlight[ 2 ];               /* the cursor highlight */

   short       numLib;                       /* number of track textures in memory eg 30 */

   short       position;                     /* position of cursor highlight on mini textures */
   short       bank;                         /* the bank of textures we are on */
} TexPlace;



void InitTexPlace( TexPlace *texPlace, TexTemplate* texTemplate );
void NewBank( TexPlace *texPlace, TexTemplate*	texTemplate );
void TexPlaceUpdate( TexPlace *texPlace, TrackSection *section, short face, TexTemplate *texTemplate, short copy );
void TexPlaceControl( TrackCamera *camera, short *face, Object *texHighlight, TrackSection *nextSection );
Object *CreateTexHighlight( );
