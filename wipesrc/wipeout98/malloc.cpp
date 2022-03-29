/* (C) Psygnosis 1994. By Jason Carl Denton
*/

/* Modified DAlloc() function to return ptr to a 4-byte boundary.
   (C) Psygnosis 1995. By Mark Kelly (it rules)
*/


#include "standard.h"
#include <stdio.h>

#include "tim.h"
#include "object.h"
#include "malloc.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "scene.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "error.h"

short FreeBlockError( FreeBlock* block )
{
/*   if ( ( block < ( FreeBlock* )0x80000000 ) || ( block > ( FreeBlock* )0x80800000 ) )
   {
      return( 1 );
   }

   if ( ( block->flags != BlockFree ) && ( block->flags != BlockAllocated ) )
   {
      return( 1 );
   }
*/
   return( 0 );
}



short BitLength(long size)
{

   short          bitsize;

/* Calculate the bit length of the size */

   bitsize = 0;
   while( size > 1 )
   {
      size = size >> 1;
      bitsize += 1;
   }

   return( bitsize );
}





void InitDynamicMem
(
   DynamicHeap*   heap,
   long           size
)
{
   long        i;
   short       bitsize;
   FreeBlock*  free;

/* Clear the free block pointers */

   for ( i=0; i<32; i++ )
   {
      heap->block[ i ] = NULL;
   }

/* Memory size minus the heap header size and the block header size */

   size = size - sizeof( DynamicHeap ) - sizeof( Block );

/* Actual heap memory starts just after the header */

   free = (FreeBlock*)(heap+1);

/* Calculate the bit length of the size */

   bitsize = BitLength( size );

/* Set up the the first free block */

   heap->block[ bitsize ] = free;

   free->flags    = BlockFree;
   free->size     = size;
   free->prev     = NULL;
   free->next     = NULL;
   free->prevFree = NULL;
   free->nextFree = NULL;

/* Set the maximum and current free memory sizes */

   heap->size = size;
   heap->free = size;

}



/* DJoin

   This procedure joins two consecutive blocks into
   one, setting the sequential links up correctly and
   calculating the size of the joined blocks.

   The two blocks must be adjacent and block two must
   follow block one in the list.

   This procedure does not manipulate any free list links.
*/

void DJoin
(
   FreeBlock*     one,
   FreeBlock*     two
)
{
   if ( FreeBlockError( one ) )
   {
      Error( "Malloc.c:DJoin: Block one is not valid", Warning );
      return;
   }

   if ( FreeBlockError( two ) )
   {
      Error( "Malloc.c:DJoin: Block two is not valid", Warning );
      return;
   }

   if ( one->next != two )
   {
      Error( "Malloc.c:DJoin: Blocks are not consecutive", Warning );
      return;
   }

   one->next = two->next;

   if ( one->next )
   {
      if ( FreeBlockError( one->next ) )
      {
         Error( "Malloc.c:DJoin: Next block is not valid", Warning );
         return;
      }

      one->next->prev = one;
   }

   one->size += two->size + sizeof( Block );
}



/* DSplit

   This procedure splits a block in two, linking the new
   block into the sequential block list and returning the
   address of the new block.

   If the block is not big enought to split, then NULL is
   returned.

   This procedure does not manipulate any free list links.
*/

FreeBlock* DSplit
(
   FreeBlock*     free,
   long           size
)
{
   FreeBlock*     block;
   long           spare;


   if ( FreeBlockError( free ) )
   {
      Error( "Malloc.c:DSplit: Free block is not valid", Warning );
      return( NULL );
   }

/* Calculate size of the split block */

   spare = free->size;
   spare -= size;
   spare -= sizeof( Block );

   spare = free->size - size - sizeof(Block);

   if ( spare >= MinimumBlockSize )
   {
   /* Calculate address of the split block */

      block = (FreeBlock*)( ((char*)free) + sizeof(Block) + size );

   /* Link new block into sequential block list */

      block->prev = free;
      block->next = free->next;

      free->next = block;

      if ( block->next )
      {
         if ( FreeBlockError( block->next ) )
         {
            Error( "Malloc.c:DSplit: Next block is not valid", Warning );
            return( NULL );
         }

         block->next->prev = block;
      }

   /* Set size of split block */

      free->size  = size;
      block->size = spare;

   /* Return address of split block */

      return( block );
   }
   else
   {
      return( NULL );
   }
}


/* DLink

   This procedure links a free block into the appropriate
   free block list, according to its size
*/

void DLink
(
   DynamicHeap*   heap,
   FreeBlock*     free
)
{
   short          bitsize;

   if ( FreeBlockError( free ) )
   {
      Error( "Malloc.c:DLink: Free block is not valid", Warning );
      return;
   }

/* Link the split block into the correct free list */

   bitsize = BitLength( free->size );

   free->prevFree = NULL;
   free->nextFree = heap->block[ bitsize ];

   heap->block[ bitsize ] = free;

   if ( free->nextFree )
   {
      if ( FreeBlockError( free->nextFree ) )
      {
         Error( "Malloc.c:DLink: Next free block is not valid", Warning );
         return;
      }

      free->nextFree->prevFree = free;
   }
}



/* DUnlink

   This procedure unlinks a free block from the
   free block list
*/

void DUnlink
(
   DynamicHeap*   heap,
   FreeBlock*     free
)
{

   short          bitsize;

   if ( FreeBlockError( free ) )
   {
      Error( "Malloc.c:DUnlink: Free block is not valid", Warning );
      return;
   }

/* Unlink from previous free block if there is one */

   if ( free->prevFree )
   {
      if ( FreeBlockError( free->prevFree ) )
      {
         Error( "Malloc.c:DUnlink: Previous block is not valid", Warning );
         return;
      }

      free->prevFree->nextFree = free->nextFree;
   }
   else
   {
   /* Unlink from top level table */

      bitsize = BitLength( free->size );
      heap->block[ bitsize ] = free->nextFree;
   }

/* Unlink from next free block if there is one */

   if ( free->nextFree )
   {
      if ( FreeBlockError( free->nextFree ) )
      {
         Error( "Malloc.c:DUnlink: Next block is not valid", Warning );
         return;
      }

      free->nextFree->prevFree = free->prevFree;
   }
}



void DCoalesce
(
   DynamicHeap*   heap,
   FreeBlock*     free
)
{

   FreeBlock*     prev;
   FreeBlock*     next;

   if ( FreeBlockError( free ) )
   {
      Error( "Malloc.c:DCoalesce: Free block is not valid", Warning );
      return;
   }

   prev = free->prev;
   next = free->next;

   if ( prev && FreeBlockError( prev ) )
   {
      Error( "Malloc.c:DCoalesce: Previous free block is not valid", Warning );
      return;
   }

   if ( next && FreeBlockError( next ) )
   {
      Error( "Malloc.c:DCoalesce: Next free block is not valid", Warning );
      return;
   }

   if ( prev && ( prev->flags == BlockFree ) &&
        next && ( next->flags == BlockFree ) )
   {
      DUnlink( heap, prev );
      DUnlink( heap, free );
      DUnlink( heap, next );

      DJoin( prev, free );
      DJoin( prev, next );

      DLink( heap, prev );

      heap->free += ( sizeof(Block) << 1 );
   }
   else if ( prev && ( prev->flags == BlockFree ) )
   {
      DUnlink( heap, prev );
      DUnlink( heap, free );

      DJoin( prev, free );

      DLink( heap, prev );

      heap->free += sizeof(Block);
   }
   else if ( next && ( next->flags == BlockFree ) )
   {
      DUnlink( heap, free );
      DUnlink( heap, next );

      DJoin( free, next );

      DLink( heap, free );

      heap->free += sizeof(Block);
   }
}

// the new bound macro
#define		return_bound4(x)		return ((char *) ((((uintptr_t) x) + 3) & ~uintptr_t(3)))


// Mark updated now returns ptr on a 4-byte boundary
char	*DAlloc(DynamicHeap* heap, long size)
{
   short       bitsize;
   FreeBlock*  free;
   FreeBlock*  block;

	// mark added vars
	char			*unbounded_ptr = NULL;
	unsigned long	unbounded_value = 0;
	unsigned long	bounded_value = 0;

/* Increment size to a 4 byte boundary */

   size = ( (size + 3) & ~3 );

   if ( size < MinimumAllocSize )
   {
      size = MinimumAllocSize;
   }

/* Calculate the bit length of the size */

   bitsize = BitLength( size );

/* Find the first big enough free block */

   free = NULL;
   while ( (!free) && ( bitsize < 31 ) )
   {
      free = heap->block[ bitsize ];

      if ( free )
      {
      /* Search free block list for a big enough block */

         if ( FreeBlockError( free ) )
         {
            Error( "Malloc.c:DAlloc: Heap header corrupted", Warning );
            return( NULL );
         }

         while( free && ( free->size < size ) )
         {
            free = free->nextFree;

            if ( free && FreeBlockError( free ) )
            {
               Error( "Malloc.c:DAlloc: Block header corrupted", Warning );
               return( NULL );
            }
         }
      }

      if ( !free )
      {
         bitsize += 1;
      }
   }

/* If block found then unlink it */

   if ( free )
   {
   /* Is block big enough to split in two ? */

      block = DSplit( free, size );

      if ( block )
      {
      /* Set flags */

         free->flags  = BlockAllocated;
         block->flags = BlockFree;

      /* Unlink from previous free block if there is one */

         if ( free->prevFree )
         {
            free->prevFree->nextFree = free->nextFree;
         }
         else
         {
         /* Unlink from top level table */

            heap->block[ bitsize ] = free->nextFree;
         }

      /* Unlink from next free block if there is one */

         if ( free->nextFree )
         {
            free->nextFree->prevFree = free->prevFree;
         }

      /* Calculate the bit length of the split block size */

         bitsize = BitLength( block->size );

      /* Link the split block into the correct free list */

         block->prevFree = NULL;
         block->nextFree = heap->block[ bitsize ];

         heap->block[ bitsize ] = block;

         if ( block->nextFree )
         {
            block->nextFree->prevFree = block;
         }

      /* Decrement total free memory */

         heap->free -= ( free->size + sizeof(Block) );

      /* Return address of data after block header */

         //return( ((char*)free) + sizeof(Block) );
         return_bound4 (((char *) free) + sizeof(Block));

      }
      else
      {
      /* Set block flags */

         free->flags = BlockAllocated;

      /* Unlink from previous free block if there is one */

         if ( free->prevFree )
         {
            free->prevFree->nextFree = free->nextFree;
         }
         else
         {
         /* Unlink from top level table */

            heap->block[ bitsize ] = free->nextFree;
         }

      /* Unlink from next free block if there is one */

         if ( free->nextFree )
         {
            free->nextFree->prevFree = free->prevFree;
         }

      /* Decrement total free memory */

         heap->free -= free->size;

      /* Return address of data after block header */

      //   return( ((char*)free) + sizeof(Block) );
		 return_bound4 (((char *) free) + sizeof(Block));
      }
   }
   else
   {
      return( NULL );
   }
}



void DFree
(
   DynamicHeap*   heap,
   char*          mem
)
{
   FreeBlock*  block;
   short       bitsize;

   if ( !mem )
   {
   /* Catch Null Pointers */

      return;
   }

/* Get block header */

   block = ( FreeBlock* )( mem - sizeof( Block ) );

   if ( block->flags != BlockAllocated )
   {
   /* Block Header Not Valid! */

      printf( "Malloc.c:DFree(): Block header not valid!\n" );
      return;
   }

/* Calculate the bit length of the block size */

   bitsize = BitLength( block->size );

/* Link the block into the correct free list */

   block->prevFree = NULL;

   block->nextFree = heap->block[ bitsize ];

   if ( ( block->nextFree ) && ( block->nextFree->flags != BlockFree ) )
      printf( "Malloc.c:DFree(): Free block list not valid!" );

   heap->block[ bitsize ] = block;

   if ( block->nextFree )
   {
      block->nextFree->prevFree = block;
   }

/* Increment total free memory */

   heap->free += block->size;

/* Set block flags to indicate that it is free */

   block->flags = BlockFree;

/* Coalesce any adjacent free blocks */

#if 1
   DCoalesce( heap, block );
#endif

}
