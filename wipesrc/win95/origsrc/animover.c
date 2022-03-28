#ifdef PCwipeout

#include <kernel.h>
#include "standard.h"

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ttf.h"
#include "ships.h"
#include "weapon.h"
#include "droid.h"
#include "camera.h"
#include "draw.h"
#include "scene.h"
#include "main.h"
#include "sound.h"
#include "control.h"
#include "combat.h"
#include "menus.h"
#include "hud.h"
#include "iso9660.h"
#include "sparks2.h"
#include "global.h"

extern u_long __bss, __bsslen, _ramsize, _stacksize;

#define H_SIZE 2048             

#define Sectors(x) ((x+H_SIZE-1)/H_SIZE)

struct XF_HDR head;

#define MAX_ARGS           10

unsigned long* rambase   = (unsigned long *)0x80100000;

int LoadProg(char *file)
{
   unsigned long 	*ptr;
   CdlFILE 			fp;
   int				mode = CdlModeSpeed;	
   int 				offset;
	int 				i;
   
  for (i = 0; i < 10; i++) {	
    if (CdSearchFile(&fp, file) == 0) 
      continue;
	}

   if (!CdControl(CdlSetloc, (unsigned char *)&(fp.pos), 0))
     return 0;

   if (!CdRead(Sectors(H_SIZE), (unsigned long *)rambase, mode))
     return 0;
	
   while (CdReadSync(1,0) > 0 );

   memcpy(&head,rambase,sizeof(head));

//   printf("pc = %08x\n",(head.exec).pc0);
//   printf("gp = %08x\n",(head.exec).gp0);

//   printf("t_addr = %08x\n",(head.exec).t_addr);
//   printf("t_size = %08x\n",(head.exec).t_size);

	offset = CdPosToInt(&fp.pos);
	offset++;
	CdIntToPos(offset,&fp.pos);

   /* Clear BSS */ 
   /* I don't think I need to do this, Exec() is supposed to. */
   /* Also, I can't use s_addr because cpe2x doesn't fill it in properly. */

   for ( ptr = (unsigned long *)head.exec.t_addr;
         ptr < (unsigned long *)0x801fbff0; ptr++ )
     *ptr = 0L;

   if ( !CdControl(CdlSetloc, (unsigned char *)&(fp.pos), 0))
     return 0;

   return CdRead(Sectors(head.exec.t_size),
                 (unsigned long *)head.exec.t_addr, mode);

}


void anim(char *filename)
{
//	printf("Loading anim %s\n", filename);

   while(!LoadProg(filename))
   {
   }
   while (CdReadSync(1,0) > 0 );

	close_iso_events();
	InitDynamicMem(heap, _ramsize - _stacksize - ((u_long)heap & 0x7fffffff));	

//	printf("File load finished\n");
	
//   printf("EXEC!\n");

   SetDispMask(0); 
   StopCallback(); 
   ResetGraph(3);
	StopPAD();

   EnterCriticalSection();

	Exec(&(head.exec), 0, 0);

   ExitCriticalSection();
//   printf("LEAVING EXEC!\n");

//	ClearVram();

   InitSystem( );
   CdInit();
	iso_init(heap);
 	InitController();
}

#endif  //PCwipeout