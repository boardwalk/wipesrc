#include "\wipesrc\psx26\include\sys\types.h"
#include "\wipesrc\psx26\include\libgte.h"
#include "\wipesrc\psx26\include\libgpu.h"
#include "grabber.h"

#define _SIZE_T_DEFINED_			// PCwipeout
#include <stdio.h>		// PCwipeout

int	save_file(char *name,char *buff,int len)
{
	FILE *fd;	// PCwipeout
	
  fd = fopen(name, "wb");	// PCwipeout	

  if (fwrite(buff, 1, len, fd) != len)	// PCwipeout
  {
     return (0);
  } 
  fclose(fd);	// PCwipeout
  return (len);
}
