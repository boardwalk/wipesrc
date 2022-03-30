/* (C) Psygnosis 1994. By Jason Carl Denton
*/


/* Dynamic Memory Allocation Structures

   These structures are used to keep track of memory
   which can be allocated and freed.
*/

typedef struct Block
{
   int32_t                 flags;      /* Attribute flags */
   int32_t                 size;       /* Size of this block */
   struct Block*        prev;       /* Previous adjacent block */
   struct Block*        next;       /* Next adjacent block */
} Block;

typedef struct FreeBlock
{
   int32_t                 flags;      /* Attribute flags */
   int32_t                 size;       /* Size of this block */
   struct FreeBlock*    prev;       /* Previous adjacent block */
   struct FreeBlock*    next;       /* Next adjacent block */
   struct FreeBlock*    prevFree;   /* Previous free block */
   struct FreeBlock*    nextFree;   /* Next free block */
} FreeBlock;

typedef struct DynamicHeap
{
   int32_t           size;          /* Total size of heap */
   int32_t           free;          /* Total free space left */

   FreeBlock*     block[ 32 ];   /* Array of free block lists */
} DynamicHeap;

#define  MinimumAllocSize  16

#define  MinimumBlockSize  64

#if 0
#define  BlockFree         0x0000
#define  BlockAllocated    0x1248
#endif

#define  BlockFree         0x46524545
#define  BlockAllocated    0x414c4f43


void InitDynamicMem
(
   DynamicHeap*   heap,
   int32_t           size
);

void DJoin
(
   FreeBlock*     one,
   FreeBlock*     two
);

FreeBlock* DSplit
(
   FreeBlock*     free,
   int32_t           size
);

int16_t BitLength
(
   int32_t           size
);

void DLink
(
   DynamicHeap*   heap,
   FreeBlock*     free
);

void DUnlink
(
   DynamicHeap*   heap,
   FreeBlock*     free
);

void DCoalesce
(
   DynamicHeap*   heap,
   FreeBlock*     free
);

char* DAlloc
(
   DynamicHeap*   heap,
   int32_t           size
);

void DFree
(
   DynamicHeap*   heap,
   char*          mem
);





/* Static Memory Allocation */

typedef struct EmptyBlock
{
   struct EmptyBlock*   next;
} EmptyBlock;


typedef struct StaticHeap
{
   DynamicHeap*         heap;
   EmptyBlock*          empty;
   char*                block;
   char*                mem;
   int32_t                 free;
   int32_t                 size;
} StaticHeap;



StaticHeap* InitStaticMem
(
   DynamicHeap*         dh,
   int32_t                 size
);

char* SAlloc( StaticHeap* sh, int32_t size );

void SFreeAll( StaticHeap* sh );

void SClear( StaticHeap* sh );
