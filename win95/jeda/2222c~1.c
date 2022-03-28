
extern void SetPal(char,char,char,char);
#pragma aux SetPal=							\
				"mov	bh,dl"					\
				"mov	edx,03c8h"				\
				"out	dx,al"					\
				"inc	dx"						\
				"mov	al,cl"					\
				"out	dx,al"					\
				"mov	al,bh"					\
				"out	dx,al"					\
				"mov	al,bl"					\
				"out	dx,al"					\
				parm	 [al] [cl] [dl] [bl]	\
				modify [ax bx cx dx];

void make2222( void )
{
int	xc,yc,cc;
int	r,g,b,i;	
int	a;
int	tr,tg,tb;

	a=0;

	cc=0;
	for (i=0;i<4;i++)
	{
		for (r=0;r<4;r++)
		{
			for (g=0;g<4;g++)
			{
				for (b=0;b<4;b++)
				{
				tr=r*4;
				tg=g*4;
				tb=b*4;
				if (tr!=0) tr+=i*16;
				if (tg!=0) tg+=i*16;
				if (tb!=0) tb+=i*16;

				if ((tr==0) && (tg==0) && (tb==0)) 
					{
				 	tr+=i*16;
				 	tg+=i*16;
				 	tb+=i*16;
					}
				SetPal(cc++,tr,tg,tb);
				}
			}
		}						  
	}


	
	
}



int GetCol(int r,int g,int b)
{
int tr,tg,tb;
	tr=r;tg=g;tb=b;
	
		r=r>>4&3;
		g=g>>4&3;
		b=b>>4&3;
	
	
		tr=tr+tg+tb;
		tr=tr/42;

//	printf("r %d g %d b %d tr %d\n",r,g,b,tr);

		return (tr<<6|r<<4|g<<2|b);
}
	
