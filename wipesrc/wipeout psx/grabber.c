#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include "grabber.h"

#define _SIZE_T_DEFINED_			// PCwipeout
#include <stdio.h>		// PCwipeout

#ifndef USE_ATI


void save_screen(int no)
{
	char	         name[128];
	RECT	         r;
	int	         i,j;
	char	         *buff0,*buff2;
	unsigned int   a,b,c;
	u_short	      *buffw;
	char				buff[(320*240*2) + HEADER];

	r.x=0;
	r.y=0;
	r.w=320;
	r.h=240;
	
	StoreImage(&r,(unsigned long*)(buff+HEADER)); // PCwipeout
	DrawSync (0) ;
	
	buffw=(ushort*)(buff+HEADER);
	for(j=0;j<240;j++)
	{
		for (i=0;i<320;i++)
		{
			a=*buffw;
		 	b=(a>>5)&31;
			c=(a>>10)&31;
			a=a&31;
			*buffw++ = (a<<10)|(b<<5)|c;
	 	}
	}
	
	buff[0]=0x00;
	buff[1]=0x00;
	buff[2]=0x02;
	buff[3]=0x00;
	buff[4]=0x00;
	buff[5]=0x00;
	buff[6]=0x00;
	buff[7]=0x00;
	buff[8]=0x00;
	buff[9]=0x00;
	
	buff[10]=0x00;
	buff[11]=0x00;
	
	buff[12]=0x40;
	buff[13]=0x01;
	buff[14]=0xf0;
	buff[15]=0x00;
	buff[16]=0x0f;
	buff[16]=16;
	buff[17]=0x02|0x20;
	
	sprintf(name,"screen%d.tga",no);
	save_file(name,buff,HEADER+(320*240*2));
}

#endif //USE_ATI
	
	
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


