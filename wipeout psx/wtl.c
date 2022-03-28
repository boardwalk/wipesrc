 
/* (C) Psygnosis 1994. By Jason Carl Denton & Rob Smith
*/


#include "standard.h"

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "wtl.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"



#ifndef USE_ATI

void LoadWtlFiles( char *file )
{
   long        length;
   char*       wtlFile;
   char*       ptr;
   char*       end;

   char        name[ 256 ];
   short       i;

   Texture*    texture;


   length = FileLength( file );
   if ( length <= 0 )
   {
      printf( "Wtl.c:LoadWtlFiles: Wtl file %s not found\n", file );
      return;
   }

   wtlFile = DAlloc( heap, length + 32 );

   if ( !wtlFile )
   {
   /* Memory Allocation Failure! */

      printf( "Wtl.c:LoadWtlFiles(): Failed to allocate memory for file %s\n", file );
      return;
   }

   if ( LoadFile( file, wtlFile ) != length )
   {
   /* File Error! */

		printf( "Wtl.c:LoadWtlFiles(): Failed to load file %s\n", file );
      DFree( heap, wtlFile );
      return;
   }

   ptr = wtlFile;
   end = wtlFile + length;

   while ( ( ptr < end ) && ( TextureTableCount < TextureTableMax ) )
   {
   /* Read in a name ( up to a '\r' ) */

      i = 0;
      while ( ( ptr < end ) && ( *ptr != '\r' ) )   
      {
         name[ i++ ] = *( ptr++ );   
      }

      name[ i ] = 0;

   /* Skip past the '\r' and '\n' */

      ptr += 2;

   /* Load in the file 'name' */

      if ( strlen ( name ) > 3 )
      {
         texture = LoadTexture( name, 1 );
         if ( texture )
         {
  		      TextureTable[ TextureTableCount++ ] = texture;
         }
      }

      if ( TextureTableCount >= TextureTableMax )
      {
         printf( "Wtl.c:LoadWtlFiles(): TextureTable is full\n" );
      }
   }

/* Free up the wtl file */

   DFree( heap, wtlFile );
}
#endif //USE_ATI


