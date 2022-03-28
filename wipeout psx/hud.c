#include "standard.h"
 
#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "wtl.h"
#include "combat.h"
#include "menus.h"
#include "hud.h"
#include "hiscores.h"
#include "fileops.h"
#include "droid.h"
#include "camera.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"


#ifdef PC_DEMO
#define	END_LAP 1
#else
#define END_LAP 3
#endif

extern short FrameState,screenres;
extern float upres;
 
TexChar	CharSet16[40] ;
TexChar	CharSet12[40] ;
TexChar	CharSet8[40] ;
extern char globalPilot[];

short speedo_x[]={179,259,371,499};
short speedo_y[]={195,255,337,435};

ushort Text16[] = 	{25, 24, 17, 24, 24, 17, 25, 18, 7, 17, 
						117, 117, 128, 117, 124, 124, 126, 124, 118, 117, 
						217, 217, 229,	224, 217, 218, 224, 210, 218, 217, 218, 
						318, 318, 318, 322, 325, 307, 307} ;

ushort Text12[] = 	{19, 19, 14, 19, 19, 13, 19, 14, 6, 13, 14, 14,
				122, 114, 119, 118, 120, 119, 115, 114, 113, 113, 122,
				219, 213, 214, 219, 208, 215, 213, 214, 215, 214, 215, 218, 219, 205, 205} ;

ushort Text8[] =	{13, 13, 10, 13, 13, 9, 13, 10, 4, 9, 10, 10, 16, 10, 13, 13, 14,
 		113, 110, 109, 109, 109, 116, 114, 109, 110, 113, 106, 111, 110, 110, 111, 110, 110, 112, 114, 104, 104} ;

POLY_FT4		symbols[2] ;

extern 	HiScores	scoreTable[] ;
extern	ChampionShip	winTable[] ;
extern	int	inattract;
extern long	theFrameRate;
 
void InitText()
{

	TIMlist  		*timPtr;
	int				i;
	
	/**********************************************************/
	/*** 			Initialise In Game Character Set				  ***/
	/**********************************************************/

   DRTEXT_16 = TextureTableCount;
	timPtr = LoadCompressedTextureSequence( "wipeout\\textures\\drfonts.cmp" );
   LoadCmpFiles( timPtr ); 
   DRTEXT_12 = DRTEXT_16 + 1 ;
   DRTEXT_8 = DRTEXT_16 + 2 ;
	DRTEXT_RED = DRTEXT_16 + 4 ;

	SetDrawMode((DR_MODE*)&dmode,0,0,FONT_TPAGE,0);
	DrawPrim((u_long *)&dmode);
	for(i = 0; i < 600; i++)
	{
//		SetPolyFT4(&(display_text[i]));	// PCwipeout
		SetSprt(&(display_text[i]));			

//		setTPage(&(display_text[i]),0,0,576, 256); // PCwipeout
		display_text[i].code=0x64;
		display_text[i].r0=230; // PCwipeout
		display_text[i].g0=0;
		display_text[i].b0=BLACK_NOT_DRAWN;
	}																		      
}	

void InitSpeedo (Speedo *speedo)
{

	int i, j;
	Texture *tex ;
	Texture *tex_ptr ;
	
	tex_ptr = LoadTexture ( "wipeout\\textures\\speedo.tim", 1 ) ;

	for(j = 0; j < 2; j++)
	{
		SetSprt(&(speedo->facia[j]));			
		speedo->facia[j].r0=230; // PCwipeout
		speedo->facia[j].g0=0; // PCwipeout
		speedo->facia[j].b0=BLACK_NOT_DRAWN; // PCwipeout
		setXY0(&(speedo->facia[j]),speedo_x[screenres],speedo_y[screenres]);
		setUV0(&(speedo->facia[j]),tex_ptr->u0, tex_ptr->v0);
		setWH(&(speedo->facia[j]),SPEEDO_WIDTH,SPEEDO_HEIGHT);
	   	speedo->facia[j].clut = tex_ptr->cba;

		SetSprt(&(speedo->transbar[j]));			
		speedo->transbar[j].r0=230; // PCwipeout
		speedo->transbar[j].b0=RED_TRANSL; // PCwipeout
		speedo->transbar[j].g0=0; // PCwipeout
		setXY0(&(speedo->transbar[j]),speedo_x[screenres],speedo_y[screenres]);
#if USE_ATI
		setUV0(&(speedo->transbar[j]),tex_ptr->u0+128, tex_ptr->v0-30);
#else
		setUV0(&(speedo->transbar[j]),tex_ptr->u0+128, tex_ptr->v0);
#endif
		setWH(&(speedo->transbar[j]),SPEEDO_WIDTH,SPEEDO_HEIGHT);
   		speedo->transbar[j].clut = tex_ptr->cba;

		SetSprt(&(speedo->solidbar[j]));			
		speedo->solidbar[j].r0=230; // PCwipeout
		speedo->solidbar[j].g0=0; // PCwipeout
		speedo->solidbar[j].b0=BLACK_NOT_DRAWN; // PCwipeout
		setXY0(&(speedo->solidbar[j]),speedo_x[screenres],speedo_y[screenres]);
		setUV0(&(speedo->solidbar[j]),tex_ptr->u0+128, tex_ptr->v0);
		setWH(&(speedo->solidbar[j]),SPEEDO_WIDTH,SPEEDO_HEIGHT);
   	speedo->solidbar[j].clut = tex_ptr->cba;

		SetDrawMode(&(speedo->fred[j]), 1, 1, FONT_TPAGE , 0); //PCWipeout
	}

	speedo->barInfo[0].Left.topx = 3 ;
	speedo->barInfo[0].Right.topx =	10 ;

	speedo->barInfo[1].Left.topx = 11 ;
	speedo->barInfo[1].Right.topx =	17 ;

	speedo->barInfo[2].Left.topx = 18 ;
	speedo->barInfo[2].Right.topx =	24 ;

	speedo->barInfo[3].Left.topx = 25 ;
	speedo->barInfo[3].Right.topx =	31 ;

	speedo->barInfo[4].Left.topx = 32 ;
	speedo->barInfo[4].Right.topx =	38 ;

	speedo->barInfo[5].Left.topx = 39 ;
	speedo->barInfo[5].Right.topx =	46 ;

	speedo->barInfo[6].Left.topx = 47 ;
	speedo->barInfo[6].Right.topx =	57 ;

	speedo->barInfo[7].Left.topx = 57 ;
	speedo->barInfo[7].Right.topx =	65 ;

	speedo->barInfo[8].Left.topx = 66 ;
	speedo->barInfo[8].Right.topx =	76 ;

	speedo->barInfo[9].Left.topx = 77 ;
	speedo->barInfo[9].Right.topx =	89 ;

	speedo->barInfo[10].Left.topx = 90 ;
	speedo->barInfo[10].Right.topx =	104 ;

	speedo->barInfo[11].Left.topx = 105 ;
	speedo->barInfo[11].Right.topx =	124 ;
}

void fill_speedo (Speedo *speedo, int speed, int thrust)
{
	int i ;
	int target;
	int width, barIntervalXTop, barIntervalXBot;
	int	andy=0;
	char	tex_x,tex_y;

	target = speed/2000;		
	if(target > NUM_BARS) target = NUM_BARS;

// THE FOLLOWING DOES ALL THE LOVELY SPEEDO STUFF																		 
		andy = (speed/2000);		
		if(andy > NUM_BARS) andy = NUM_BARS;
		if (andy==NUM_BARS)
		{
			setWH(&(speedo->solidbar[CurrentScreen]),SPEEDO_WIDTH,SPEEDO_HEIGHT);
		}
		else
		{
			target = speed%2000;	
			width = 	(speedo->barInfo[andy].Right.topx) - (speedo->barInfo[andy].Left.topx);
			barIntervalXTop = (width*target)/2000;
			setWH(&(speedo->solidbar[CurrentScreen]),speedo->barInfo[andy].Left.topx+barIntervalXTop,SPEEDO_HEIGHT);
		}
		AddPrim(OT[CurrentScreen], (SPRT *)&(speedo->solidbar[CurrentScreen]));

		andy = (thrust/70);		
		if(andy > NUM_BARS) andy = NUM_BARS;
		if (andy==NUM_BARS)
		{
			setWH(&(speedo->transbar[CurrentScreen]),SPEEDO_WIDTH,SPEEDO_HEIGHT);
		}
		else
		{
			target = thrust%70;		
			width = 	(speedo->barInfo[andy].Right.topx) - (speedo->barInfo[andy].Left.topx);
			barIntervalXTop = (width*target)/70;
			setWH(&(speedo->transbar[CurrentScreen]),speedo->barInfo[andy].Left.topx+barIntervalXTop	,SPEEDO_HEIGHT);
		}

		AddPrim(OT[CurrentScreen], (SPRT *)&(speedo->transbar[CurrentScreen]));
// BIT SHORTER THAN STU'S
}  	



void AddTime(int time, TEXT_DATA *pos_data, int colour)
						
{
   int i,j;
	int index = 0;
	int digit[20];
	int x, y;
	int mins=0, secs=0, tenths=0;
	Texture *curr_num, *charTex, *redTex ;
	int small_num_size;
	int size, charNum ;
 	short			vramRow=0, vramY=0, vramX=0 ;
	ushort		currLett=0, searchLett=0 ;
 	TexChar		*charSet ;

	x = pos_data->start_pos_x;
	y = pos_data->start_pos_y;

	x*=upres;
	y*=upres;

	size= pos_data->size ;
	small_num_size = size>>1;

//	SetDrawMode((DR_MODE*)&dmode,0,0,FONT_TPAGE,0);
//	DrawPrim((u_long *)&dmode);

	switch (size)
	{
		case 16:	charSet = &(CharSet16[0]) ;	
					charTex = (Texture *) TextureTable [DRTEXT_16] ;
					break ;

		case 12:	charSet = &(CharSet12[0]) ;
					charTex = (Texture *) TextureTable [DRTEXT_12] ;
					break ;

		case  8:	charSet = &(CharSet8[0]) ;
					charTex = (Texture *) TextureTable [DRTEXT_8] ;
					break ;
	}

#if PAL
	tenths = (time / 5) % 10;
	time = time/50;
#else
	tenths = (time / FR6) % 10;
	time = time/FR60;
#endif

	secs = time % 60;
	mins = time / 60;

	digit[0] = mins%10;

	if(mins > 9)
	{
		mins = mins/10;
		digit[1] = mins%10;
	}
	else digit[1] = 0;

   for (j = 1; j >= 0; j--)
	{

		charNum = (digit[j]+26) ;
		currLett = charSet[charNum].width ;


		setXY0(&display_text[textPrim],x,y);	// PCwipeout
		setUV0(&display_text[textPrim],charSet[charNum].vramX, charSet[charNum].vramY);	// PCwipeout
		setWH(&display_text[textPrim],currLett,size);	// PCwipeout

		if (colour == WhiteText)
		{
  			display_text[textPrim].clut = charTex->cba;
				display_text[textPrim].r0 = 230;
				display_text[textPrim].g0 = 0;
				display_text[textPrim].b0 = BLACK_NOT_DRAWN;
		}
		else
		{
			redTex = (Texture *) TextureTable [DRTEXT_RED] ;
				display_text[textPrim].r0 = 230;
				display_text[textPrim].g0 = BRIGHT_RED_TRANSL;
				display_text[textPrim].b0 = 0;
  			display_text[textPrim].clut = redTex->cba;
		}

		AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(display_text[textPrim]) );
		x += currLett;
		textPrim++;
	}	


	currLett = charSet[36].width ;

	
		setXY0(&display_text[textPrim],x,y);	// PCwipeout
		setUV0(&display_text[textPrim],charSet[36].vramX, charSet[36].vramY);	// PCwipeout
		setWH(&display_text[textPrim],currLett,size);	// PCwipeout


	if (colour == WhiteText)
	{
 			display_text[textPrim].clut = charTex->cba;
			display_text[textPrim].r0 = 230;
			display_text[textPrim].g0 = 0;
			display_text[textPrim].b0 = BLACK_NOT_DRAWN;
	}
	else
	{
		redTex = (Texture *) TextureTable [DRTEXT_RED] ;
		display_text[textPrim].r0 = 230;
		display_text[textPrim].g0 = BRIGHT_RED_TRANSL;
		display_text[textPrim].b0 = 0;
   	display_text[textPrim].clut = redTex->cba;
	}

   AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(display_text[textPrim]) );
	

	x += currLett;

	textPrim++;


	digit[0] = secs%10;
	if(secs > 9)
	{
		secs = secs/10;
		digit[1] = secs%10;
	}
	else digit[1] = 0;

   for (j = 1; j >= 0; j--)
	{
		charNum = (digit[j]+26)	;
		currLett = charSet[charNum].width ;


		setXY0(&display_text[textPrim],x,y);	// PCwipeout
		setUV0(&display_text[textPrim],charSet[charNum].vramX, charSet[charNum].vramY);	// PCwipeout
		setWH(&display_text[textPrim],currLett,size);	// PCwipeout
	
		if (colour == WhiteText)
		{
 			display_text[textPrim].clut = charTex->cba;
			display_text[textPrim].r0 = 230;
			display_text[textPrim].g0 = 0;
			display_text[textPrim].b0 = BLACK_NOT_DRAWN;
		}
		else
		{
			redTex = (Texture *) TextureTable [DRTEXT_RED] ;
			display_text[textPrim].r0 = 230;
			display_text[textPrim].g0 = BRIGHT_RED_TRANSL;
			display_text[textPrim].b0 = 0;
   		display_text[textPrim].clut = redTex->cba;
		}

   	AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(display_text[textPrim]) );
	
		x += currLett;
		textPrim++;
	}	
	
	currLett = charSet[37].width ;
	
	setXY0(&display_text[textPrim],x,y);	// PCwipeout
	setUV0(&display_text[textPrim],charSet[37].vramX, charSet[37].vramY);	// PCwipeout
	setWH(&display_text[textPrim],currLett,size);	// PCwipeout

	if (colour == WhiteText)
	{
		display_text[textPrim].clut = charTex->cba;
		display_text[textPrim].r0 = 230;
		display_text[textPrim].g0 = 0;
		display_text[textPrim].b0 = BLACK_NOT_DRAWN;
	}
	else
	{
		redTex = (Texture *) TextureTable [DRTEXT_RED] ;
		display_text[textPrim].r0 = 230;
		display_text[textPrim].g0 = BRIGHT_RED_TRANSL;
		display_text[textPrim].b0 = 0;
		display_text[textPrim].clut = redTex->cba;
	}


   AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(display_text[textPrim]) );
	

	x += currLett;

	textPrim++;

	charSet = &(CharSet8[0]) ;
	charTex = (Texture *) TextureTable [DRTEXT_8] ;

	if (size != 8)
	{
		size = 8 ;
		y += 8 ;	
	}
	
	currLett = charSet[(tenths+26)].width ;

	
	setXY0(&display_text[textPrim],x,y);	// PCwipeout
	setUV0(&display_text[textPrim],charSet[tenths+26].vramX, charSet[tenths+26].vramY);	// PCwipeout
	setWH(&display_text[textPrim],currLett,size);	// PCwipeout

	if (colour == WhiteText)
	{
		display_text[textPrim].clut = charTex->cba;
		display_text[textPrim].r0 = 230;
		display_text[textPrim].g0 = 0;
		display_text[textPrim].b0 = BLACK_NOT_DRAWN;
	}
	else
	{
		redTex = (Texture *) TextureTable [DRTEXT_RED] ;
		display_text[textPrim].r0 = 230;
		display_text[textPrim].g0 = BRIGHT_RED_TRANSL;
		display_text[textPrim].b0 = 0;
		display_text[textPrim].clut = redTex->cba;
	}

   AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(display_text[textPrim]) );

 	x += currLett ;
	textPrim++;

}
  	
void AddText(char* text_ptr, TEXT_DATA *pos_data, int colour)
{
	uchar 	letter=0;
	char 		AValue=0;
	Texture 	*charTex, *redTex ;
	int 		size=0 ;
	int 		x=0, y=0, i=0 ,newx,newy;
	short		vramRow=0, vramY=0, vramX=0 ;
	short 	j=0;
	ushort	 currLett=0, searchLett=0 ;
	TexChar	*charSet ;
	short		charNum ;		

	x = pos_data->start_pos_x;
	y = pos_data->start_pos_y;

	size = pos_data->size ;

	switch (size)
	{
		case 16:	charSet = &(CharSet16[0]) ;	
					charTex = (Texture *) TextureTable [DRTEXT_16] ;
					break ;

		case 12:	charSet = &(CharSet12[0]) ;
					charTex = (Texture *) TextureTable [DRTEXT_12] ;
					break ;

		case  8:	charSet = &(CharSet8[0]) ;
					charTex = (Texture *) TextureTable [DRTEXT_8] ;
					break ;
	}

	while(1)
	{
		letter = *text_ptr;
		text_ptr++;


		if (letter != 32)
		{
			if(letter == 0) return;

			AValue = 'A';
			if(letter>='0' && letter<='9')
				j=letter-'0'+26;
			else
				j = letter - AValue;

			currLett = charSet[j].width ;

			newx = x*upres + ((currLett*upres)-currLett)/2;
			newy = y*upres;

			setXY0(&display_text[textPrim],newx,newy);	// PCwipeout
			setUV0(&display_text[textPrim],charSet[j].vramX, charSet[j].vramY);	// PCwipeout
			setWH(&display_text[textPrim],currLett,size);	// PCwipeout
									
	
			if (colour == WhiteText)
			{
   				display_text[textPrim].clut = charTex->cba;
				display_text[textPrim].r0 = 230;
				display_text[textPrim].g0 = 0;
				display_text[textPrim].b0 = BLACK_NOT_DRAWN;
			}
			else
			{
				redTex = (Texture *) TextureTable [DRTEXT_RED] ;
				display_text[textPrim].r0 = 230;
				display_text[textPrim].g0 = BRIGHT_RED_TRANSL;
				display_text[textPrim].b0 = 0;
   				display_text[textPrim].clut = redTex->cba;
			}

	   		AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(display_text[textPrim]) );

			textPrim++;
			x += currLett ;
		}
		else
		{
		 	x += 8 ;
		}
	}	
}


void AddNumber(int num, TEXT_DATA *pos_data, int colour)
{
   int i,j;
	int digit[12];
	int x, y , newx ,newy;
	int number_position ;
	Texture *curr_num, *charTex, *redTex ;
	int size, charNum ;
	short		vramRow=0, vramY=0, vramX=0 ;
	ushort	currLett=0, searchLett=0 ;
	TexChar	*charSet ;

	x = pos_data->start_pos_x;
	y = pos_data->start_pos_y;
	size = pos_data->size ;

	switch (size)
	{
		case 16:	charSet = &(CharSet16[0]) ;	
					charTex = (Texture *) TextureTable [DRTEXT_16] ;
					break ;

		case 12:	charSet = &(CharSet12[0]) ;
					charTex = (Texture *) TextureTable [DRTEXT_12] ;
					break ;

		case  8:	charSet = &(CharSet8[0]) ;
					charTex = (Texture *) TextureTable [DRTEXT_8] ;
					break ;
	}

   for (i = 0; i < 12; i++)
   {
      digit[i] = num%10;
      num = num/10;
      if (num == 0)
         break;
   } 

   for (j = i; j >= 0; j--)
	{
		charNum = (digit[j]+26) ;
		currLett = charSet[charNum].width ;

		newx = x*upres + ((currLett*upres)-currLett)/2;
		newy = y*upres;

		setXY0(&display_text[textPrim],newx,newy);	// PCwipeout
		setUV0(&display_text[textPrim],charSet[charNum].vramX, charSet[charNum].vramY);	// PCwipeout
		setWH(&display_text[textPrim],currLett,size);	// PCwipeout

		if (colour == WhiteText)
		{
		
			display_text[textPrim].r0 = 230;
			display_text[textPrim].g0 = 0;
			display_text[textPrim].b0 = BLACK_NOT_DRAWN;
   		display_text[textPrim].clut = charTex->cba;
		}
		else
		{
			redTex = (Texture *) TextureTable [DRTEXT_RED] ;
			display_text[textPrim].r0 = 230;
			display_text[textPrim].g0 = BRIGHT_RED_TRANSL;
			display_text[textPrim].b0 = 0;
   		display_text[textPrim].clut = redTex->cba;
		}

 
      AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(display_text[textPrim]) );
		textPrim++;	
		x += currLett;
	}	

}


void AddDelEnd(char dispChar, TEXT_DATA *pos_data, int colour)
{
	Texture 	*endTex, *delTex, *redTex ;
	int 		size=0 ;
	int 		x=0, y=0 ,newx ,newy;

	x = pos_data->start_pos_x;
	y = pos_data->start_pos_y;

	newx = x * upres;
	newy = y * upres;
	
	size = pos_data->size ;

	endTex = (Texture *) TextureTable [DRTEXT_RED+3] ;
	delTex = (Texture *) TextureTable [DRTEXT_RED+4] ;

	if (dispChar)
	{
		setXY0(&display_text[textPrim],newx,newy);	// PCwipeout
		setUV0(&display_text[textPrim],endTex->u0, endTex->v0);	// PCwipeout
		setWH(&display_text[textPrim],24,size);	// PCwipeout
	}
	else
	{
		setXY0(&display_text[textPrim],newx,newy);	// PCwipeout
		setUV0(&display_text[textPrim],delTex->u0, delTex->v0);	// PCwipeout
		setWH(&display_text[textPrim],24,size);	// PCwipeout
	}	

	if (colour == WhiteText)
 			display_text[textPrim].clut = endTex->cba;
	else
	{
		redTex = (Texture *) TextureTable [DRTEXT_RED] ;
 			display_text[textPrim].clut = redTex->cba;
	}

	AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(display_text[textPrim]) );

	textPrim++;
}

void SetCharTextures (char size)
{
	short currLett ;
	short i, j ;
	short vramY, vramX ;
	short searchLett, vramRow ;
	ushort *charSet ;
	


	switch (size)
	{
		case 8:
				charSet = &(Text8[0]) ;
			  	break ;

		case 12:
				charSet = &(Text12[0]) ;
			  	break ;

		case 16:
				charSet = &(Text16[0]) ;
			  	break ;

	}
 

	for (j=0; j < 40; j++)
	{

		currLett = charSet[j] ;
	
		vramY = 0 ;

		vramRow = (currLett / 100) ;

		vramY += (vramRow * size) ;
		vramX = 0 ;
  

		for (i=0 ; i < j; i++)
		{
			searchLett = charSet[i] ;
			if ((searchLett / 100)  == vramRow)
				vramX += (searchLett % 100)  ;
		}
	

	 
		switch (size)
		{
			case 8:

				CharSet8[j].vramX = vramX ;
				CharSet8[j].vramY = vramY + 100 ;
				CharSet8[j].width = (currLett % 100)  ;
				
			  	break ;

			case 12:

				CharSet12[j].vramX = vramX ;
/*				printf ("vramx = %d pos = %d \n", CharSet12[j].vramX, j) ;*/
				CharSet12[j].vramY = vramY + 64 ;
				CharSet12[j].width = (currLett % 100)  ;
			  	break ;

			case 16:

				CharSet16[j].vramX = vramX ;
				CharSet16[j].vramY = vramY ;
				CharSet16[j].width = (currLett % 100)  ;
			  	break ;

		}
 
	}					

}


TEXT_DATA *text_data( int start_pos_x, int start_pos_y, int size)
{
	static TEXT_DATA text;

	text.start_pos_x = start_pos_x ;
	text.start_pos_y = start_pos_y ;
	text.size = size ;

	return(&text);
}

extern int short startLinePos[ ];
  	

void UpdateRaceHud(ShipData* shipIndex, Speedo *speedo, int choice, ConfigData *gameData)
{	
	int 				i, j;
	int 				position = NO_OF_SHIPS;
	int 				ownSecNo, iSecNo, serialSecNo;
	int				width = 0 ;
	int				tempPosition = 1 ;
	char				trackList[15] = {2, 4, 1, 12, 8, 9, 10, 3, 5, 6, 7, 11, 13, 14, 15} ;
	int				secNo;
	static			lapRecordCount = 0 ;
	int				target ;
	SortTable		sortTable [ NO_OF_SHIPS ] ;
	char				tempShip ;
	char				tempPoints ;


	if (raceType == HEAD_TO_HEAD)
	{
		for(i = 0; i < NO_OF_SHIPS; i++)
		{   
			if ((i == ownShip) || (i == serialShip))
			{
				if( i == ownShip )
				{
					if( (shipIndex[i].prevSecNo == (startLinePos[ trackNo ]+1) ) &&
						 (shipIndex[i].nearSecNo <= startLinePos[ trackNo ])  )
					{
						crossedLine--;
					}
			 	}

				if( (shipIndex[i].prevSecNo == startLinePos[ trackNo ] ) &&
					 ((shipIndex[i].nearSecNo > startLinePos[ trackNo ]) &&
					  (shipIndex[i].nearSecNo < (startLinePos[ trackNo ]+10))) )
				{
					if( i == ownShip)
					{
						if(shipIndex[i].lapNo == crossedLine)
								shipIndex[i].lapNo++;
						
						crossedLine++;
					}
					else
					{
						shipIndex[i].lapNo++;
					}

				}
			}
		}

		for(i = 0; i < NO_OF_SHIPS; i++)
		{  	
			if ((i == ownShip) || (i == serialShip))
			{

				secNo = shipIndex[i].nearSecNo - (startLinePos[ trackNo ] + 1);

				if(i == ownShip)
				{
					if(secNo < 0)
					{
						secNo = secNo + (crossedLine * NoOfMainSections);
					}
					else
					{
						secNo = secNo + ((crossedLine - 1) * NoOfMainSections);
					}
				}
				else
				{
					if(secNo < 0)
					{
						secNo = secNo + (shipIndex[i].lapNo * NoOfMainSections);
					}
					else
					{
						secNo = secNo + ((shipIndex[i].lapNo - 1) * NoOfMainSections);
					}
				}

				shipIndex[i].DPASectDiff = secNo;
			}
		}
	}
	else
	{
		for(i = 0; i < NO_OF_SHIPS; i++)
		{  	
			if( i == ownShip)
			{
				if( (shipIndex[i].prevSecNo == (startLinePos[ trackNo ]+1) ) &&
					 (shipIndex[i].nearSecNo <= startLinePos[ trackNo ])  )
				{
					crossedLine--;
				}
		 	}

			if( (shipIndex[i].prevSecNo == startLinePos[ trackNo ] ) &&
				 ((shipIndex[i].nearSecNo > startLinePos[ trackNo ]) &&
				  (shipIndex[i].nearSecNo < (startLinePos[ trackNo ]+10))) )
			{
				if( i == ownShip)
				{
					if(shipIndex[i].lapNo == crossedLine)
							shipIndex[i].lapNo++;
					
					crossedLine++;
					if(raceType == TIME_TRIAL)
						shipIndex[i].weaponType = TURBO;
				}
				else
				{
					shipIndex[i].lapNo++;
				}

			}
		}
		for(i = 0; i < NO_OF_SHIPS; i++)
		{  	
			secNo = shipIndex[i].nearSecNo - (startLinePos[ trackNo ] + 1);

			if(i == ownShip)
			{
				if(secNo < 0)
				{
					secNo = secNo + (crossedLine * NoOfMainSections);
				}
				else
				{
					secNo = secNo + ((crossedLine - 1) * NoOfMainSections);
				}
			}
			else
			{
				if(secNo < 0)
				{
					secNo = secNo + (shipIndex[i].lapNo * NoOfMainSections);
				}
				else
				{
					secNo = secNo + ((shipIndex[i].lapNo - 1) * NoOfMainSections);
				}
			}

			shipIndex[i].DPASectDiff = secNo;
		}
	}

	if(shipIndex[ownShip].nearSecNo <= startLinePos[trackNo]) ownSecNo = shipIndex[ownShip].nearSecNo + track->sectionCount;
	else ownSecNo = shipIndex[ownShip].nearSecNo;

	if(gameType != SINGLE)
	{
		if(shipIndex[serialShip].nearSecNo <= startLinePos[trackNo]) serialSecNo = shipIndex[serialShip].nearSecNo + track->sectionCount;
		else serialSecNo = shipIndex[serialShip].nearSecNo;
	
	}
	
	position = CalcPosition(shipIndex);

	if ((raceType == TWO_PLAYER_RACE) || (raceType == HEAD_TO_HEAD))
	{
		if(shipIndex[serialShip].lapNo > 0)
		{
			if((choice == -1) && (shipIndex[serialShip].lapNo < END_LAP+1))
			{
				shipIndex[serialShip].lapTimes[ (shipIndex[serialShip].lapNo-1) ] += 2;
			}
		}
	}


	if(shipIndex[ownShip].lapNo > 0)
	{
		if((choice == -1) && (shipIndex[ownShip].lapNo < END_LAP+1))
		{
			shipIndex[ownShip].lapTimes[ (shipIndex[ownShip].lapNo-1) ] += 2;
		}
	}



	if(gameType == SINGLE)
	{
		if((shipIndex[ownShip].lapNo != shipIndex[ownShip].prevLapNo) && 
			(shipIndex[ownShip].lapNo > 1) && (shipIndex[ownShip].lapNo <= END_LAP+1))
		{
			if ((CheckLapRecord (gameData, &(shipIndex[0]), ownShip)))
			{
				gameData->newLapRecord = 1 ;
//				printf ("New lap record\n") ;
			}			
		}
		shipIndex[ownShip].prevLapNo = shipIndex[ownShip].lapNo ;
	}
	else
	{
		if	(	((shipIndex[ownShip].lapNo != shipIndex[ownShip].prevLapNo) && 
				(shipIndex[ownShip].lapNo > 1) && (shipIndex[ownShip].lapNo <= END_LAP+1)) 
			)
		{
			if (gameType != SINGLE)
				gameData->newLapRecord = CheckLapRecord (gameData, &(shipIndex[0]), ownShip) ;
			else
				if (CheckLapRecord (gameData, &(shipIndex[0]), ownShip))
				{
					gameData->newLapRecord = 1 ;
				}
		}
		else if( ((shipIndex[serialShip].lapNo != shipIndex[serialShip].prevLapNo) && 
					(shipIndex[serialShip].lapNo > 1) && (shipIndex[serialShip].lapNo <= END_LAP+1)) 
				)
		{
			if (gameType != SINGLE)
				gameData->newLapRecord = CheckLapRecord (gameData, &(shipIndex[0]), serialShip) ;
			else
				if (CheckLapRecord (gameData, &(shipIndex[0]), serialShip))
				{
					gameData->newLapRecord = 1 ;
				}
		}


		shipIndex[ownShip].prevLapNo = shipIndex[ownShip].lapNo ;
		shipIndex[serialShip].prevLapNo = shipIndex[serialShip].lapNo ;

	}

	if(shipIndex[ownShip].attr & RACING)
	{
#if PAL

		if (gameType == MASTER || gameType == SLAVE)
		{
			AddText ("P", text_data (260, 60, 8), RedText) ;
			AddNumber (2, text_data (274, 60, 8), RedText) ; 	
			AddNumber (winTable[serialShip].racePosition, text_data (290, 60, 8), WhiteText) ; 	
		}

		AddText ("LAP", text_data (15, 4, 8), RedText) ; // PCwipeout Y was 10
		AddNumber (shipIndex[ownShip].lapNo, text_data (10, 16, 16), WhiteText) ; 	// Y was 20

		width = Text16[(shipIndex[ownShip].lapNo+26)] % 100 ;

		AddText ("OF", text_data ((10+width), 24, 8), RedText) ; // PCwipeout Y was 28

		width += 22 ;

		AddNumber (3, text_data ((10+width), 16, 16), WhiteText) ; 	// PCwipeout Y was 20

		if(raceType != TIME_TRIAL)
		{
			AddText ("POSITION", text_data (240, 4, 8), RedText) ; // PCwipeout Y was 10
			AddNumber (winTable[ownShip].racePosition, text_data (270, 16, 16), WhiteText) ; // PCwipeout
  	}
		AddText ("LAP RECORD", text_data (15, 40, 8), RedText) ;


		if (gameData->gameType == TIME_TRIAL)
			AddTime(scoreTable[((gameData->trackNum-1)+HiScoreTracks)].lapRecord,
							 text_data (15, 52, 8), WhiteText);
		else
			AddTime(scoreTable[(gameData->trackNum-1)].lapRecord,
							 text_data (15, 52, 8), WhiteText);

		if((shipIndex[ownShip].attr & DIRECTION) == 0)
		{
			CentreText ("WRONG WAY", text_data (0, 120, 16), RedText) ;
		}	
#else

#endif
	}

	/************ HAVE FINISHED RACE *****************/
	for(i = 0; i < NO_OF_SHIPS; i++)
	{ 
		if( i != ownShip)
		{
			if((shipIndex[i].lapNo > END_LAP) && (shipIndex[i].attr & RACING))
			{
				shipIndex[i].attr &= ~RACING;
				if(i == serialShip)
				{
					serialLapTimes[0] = shipIndex[serialShip].lapTimes[0];
					serialLapTimes[1] = shipIndex[serialShip].lapTimes[1];
					serialLapTimes[2] = shipIndex[serialShip].lapTimes[2];
					serialLapTimes[3] = shipIndex[serialShip].lapTimes[3];
					serialLapTimes[4] = shipIndex[serialShip].lapTimes[4];
				}
			}
		}
	}


	if(shipIndex[ownShip].lapNo > END_LAP /*&& ownSecNo == (startLinePos[ trackNo ]+1))*/ && (shipIndex[ownShip].attr & RACING))
	{
	
		inattract=1;

		gameData->Position = winTable[ownShip].racePosition; 

//		printf ("Position = %d \n", gameData->Position) ;

		gameData->EndOfGame = 1 ;

		if (gameData->gameType == SINGLE_RACE || gameData->gameType == TIME_TRIAL)
		{
			gameData->showPilotCount = 120 ;
		}

		if (raceType == HEAD_TO_HEAD || raceType == TWO_PLAYER_RACE)
		{

		}


		if (gameData->gameType == CHAMPIONSHIP)
		{
			gameData->showPilotCount = 120 ;

			if ((gameData->Position >= 1 && gameData->Position <= 3)
					&& gameData->Lives > 0)
			{		

				/* Find position during race */		
				for (i=0; i < NO_OF_SHIPS; i++)
				{
					switch (winTable[i].racePosition)
					{
						case 1:  winTable[i].points += 9 ;
									winTable[i].racePoints = 9 ;									
									break ;

						case 2:	winTable[i].points += 7 ;
									winTable[i].racePoints = 7 ;									
									break ;

						case 3:	winTable[i].points += 5;
									winTable[i].racePoints = 5 ;									
									break ;

						case 4:	winTable[i].points += 3;
									winTable[i].racePoints = 3 ;									
									break ;

						case 5:	winTable[i].points += 2;
									winTable[i].racePoints = 2 ;									
									break ;

						case 6:	winTable[i].points += 1;
									winTable[i].racePoints = 1 ;									
									break ;

						case 7:	winTable[i].points += 0;
									winTable[i].racePoints = 0 ;									
									break ;

						case 8:	winTable[i].points += 0;
									winTable[i].racePoints = 0 ;									
									break ;

					}

				}

				for (i=0; i < NO_OF_SHIPS; i++)
				{
				 	sortTable[i].shipNum = i ;
					sortTable[i].points = winTable[i].points ;
				}

				
				for (i=0; i < NO_OF_SHIPS; i++)
				{

				for (j=0; j < (NO_OF_SHIPS-1); j++)
				{
			  		if (sortTable[j].points < sortTable[j+1].points)
					{
						tempShip = sortTable[j].shipNum ;
						tempPoints = sortTable[j].points ;

						sortTable[j].shipNum = sortTable[j+1].shipNum ;							
						sortTable[j].points = sortTable[j+1].points ;
 
						sortTable[j+1].shipNum = tempShip ;							
						sortTable[j+1].points = tempPoints ;
  
					}
				}

				}


//				for (i=0; i < NO_OF_SHIPS; i++)
//				{
//				 	printf ("%d  = %d \n",(i+1),sortTable[i].points) ;
//				}

				for (i=0; i < NO_OF_SHIPS; i++)
				{
				 	winTable[(sortTable[i].shipNum)].position	= (i+1) ;
				}

				gameData->showPointCount = 120 ;
				gameData->showChampCount = 120 ;
			}

			gameData->gameOverCount	= 120 ;



			if (gameData->Position > 3)
			{
				gameData->Lives -- ;
				gameData->trackInc = 0 ;
			}
			else
			{
				if (gameData->trackNum == NumTracks)  
				{
				 	if (winTable[ownShip].position == 1)
					{
						gameData->RapierClass = 1 ;
						gameData->class = 1 ;
						if (gameData->showBonusTrack == 0)
						{
							gameData->trackNum += 2 ;
							gameData->trackInc = 2 ;
						}
						else
						{
							gameData->trackNum ++ ;
							gameData->trackInc = 1 ;
						}
						gameData->actTrackNum = trackList[(gameData->trackNum-1)] ;
						gameData->venomComplete = 1 ;
						gameData->gotoMenu = 1 ;
					}
					else
					{
					 	gameData->lostGame = 1 ;
						gameData->trackInc = 0 ;
					}

				}
				else

					if (gameData->trackNum == totalTracks)
					{
					 	if (winTable[ownShip].position == 1)
						{
						 	gameData->gameComplete = 1 ;
							totalTracks = 14 ;
							NumTracks = 7 ;
							gameData->showBonusTrack = 1 ;
						}
						else
						{
						 	gameData->lostGame = 1 ;
							gameData->trackInc = 0 ;
						}
					}


					else
					{
						gameData->trackInc = 1 ;
						gameData->trackNum ++ ;
						gameData->actTrackNum = trackList[(gameData->trackNum-1)] ;
					}
			}
		
		}

		
		shipIndex[ownShip].update = UpdateRemoteShip;
		shipIndex[ownShip].updateCount = 0;
		shipIndex[ownShip].remoteMaxThrust = 3160;
		shipIndex[ownShip].remoteThrustMag = 32;
		shipIndex[ownShip].speed = 3160;
		shipIndex[ownShip].attr &= ~SHIELDED;
		shipIndex[ownShip].attr &= ~VIEW_IN;
		shipIndex[ownShip].attr &= ~RACING;

		CameraUpdate = UpdateCameraAttractMode;

		tempLapTimes[0] = shipIndex[ownShip].lapTimes[0];
		tempLapTimes[1] = shipIndex[ownShip].lapTimes[1];
		tempLapTimes[2] = shipIndex[ownShip].lapTimes[2];
		tempLapTimes[3] = shipIndex[ownShip].lapTimes[3];
		tempLapTimes[4] = shipIndex[ownShip].lapTimes[4];

		if(gameType != SINGLE)
  			attractDelay = 1;
		else
			if (gameData->gameType == CHAMPIONSHIP)
  				attractDelay = 1;
			else	
  				attractDelay = 300;
	}

}


int CalcPosition(ShipData *shipIndex)
{
	int 				i, j;
	int 				tempPosition;
	VECTOR			B, C, AB, AC, trackVect;
	int				angleB, angleC, distanceB, distanceC;
	int				position;

	for(i = 0; i < NO_OF_SHIPS; i++)
	{
		tempPosition = 1 ;

		if	( 
				(raceType != HEAD_TO_HEAD) ||
				( (raceType == HEAD_TO_HEAD) && ((i == ownShip) || (i == serialShip)))
			)
		{
		 	for (j = 0; j < NO_OF_SHIPS; j++)
			{
				if	( 
						(raceType != HEAD_TO_HEAD) ||
						( (raceType == HEAD_TO_HEAD) && ((j == ownShip) || (j == serialShip)))
					)
				{

					if(i != j)
					{
						
						if( ( (shipIndex[j].attr & RACING) == 0))
							tempPosition ++ ;

						else if (shipIndex[i].DPASectDiff == shipIndex[j].DPASectDiff)
						{
							B.vx = shipIndex[i].ppivot.vx;
							B.vy = shipIndex[i].ppivot.vy;
							B.vz = shipIndex[i].ppivot.vz;

							C.vx = shipIndex[j].ppivot.vx;
							C.vy = shipIndex[j].ppivot.vy;
							C.vz = shipIndex[j].ppivot.vz;

							AB.vx = B.vx - shipIndex[i].nearTrkSect->centre.vx;
							AB.vy = B.vy - shipIndex[i].nearTrkSect->centre.vy;
							AB.vz = B.vz - shipIndex[i].nearTrkSect->centre.vz;

							AC.vx = C.vx - shipIndex[j].nearTrkSect->centre.vx;
							AC.vy = C.vy - shipIndex[j].nearTrkSect->centre.vy;
							AC.vz = C.vz - shipIndex[j].nearTrkSect->centre.vz;

							trackVect.vx = shipIndex[i].nearTrkSect->nextSection.ptr->centre.vx - shipIndex[i].nearTrkSect->centre.vx ;
							trackVect.vy = shipIndex[i].nearTrkSect->nextSection.ptr->centre.vy - shipIndex[i].nearTrkSect->centre.vy ;
							trackVect.vz = shipIndex[i].nearTrkSect->nextSection.ptr->centre.vz - shipIndex[i].nearTrkSect->centre.vz ;

							angleB = GetAng(AB, trackVect);
							angleC = GetAng(AC, trackVect);
							distanceB = (rcos(angleB>>3) * GetMagnitude(AB))>>12;
							distanceC = (rcos(angleC>>3) * GetMagnitude(AC))>>12;

							if((distanceB == distanceC) && (i > j))
								tempPosition ++ ;

							else if(distanceB < distanceC)
								tempPosition ++ ;
						}
						else if (shipIndex[i].DPASectDiff < shipIndex[j].DPASectDiff)
						{
							tempPosition ++ ;
						}
					}
				}							 	
			}
		}
		if(i == ownShip)
			position = tempPosition	;
	
		if(shipIndex[i].attr & RACING)
			winTable[i].racePosition = tempPosition ;									
	}
	return(position);
}


void ScreenFooter (int x, int y, int colour)
{
	Texture		*symbolTex, *redTex ;
	int			i ;
	int			offSet = 110 ;

	for (i=0; i < 2; i++)
	{
		symbolTex = (Texture *)TextureTable [DRTEXT_RED + (i+1)] ;	

		SetPolyFT4(&(symbols[i]));			
		symbols[i].r0=230;
		symbols[i].g0=0;
		symbols[i].b0=BLACK_NOT_DRAWN;

		if (i == 0)
		{
			setXY4(&(symbols[i]), x * upres, y * upres,
										  x+20, y,
										  x, y+16,
										  x+20, y+16) ;

			AddText ("SELECT",text_data (x+22,y+4, 8),WhiteText) ;

		}
		else
		{
		 	setXY4(&(symbols[i]), (x+offSet) * upres, y * upres,
										  x+20+offSet, y,
										  x+offSet, y+16,
										  x+20+offSet, y+16) ;

			AddText ("BACK",text_data (x+22+offSet,y+4, 8),WhiteText) ;
		}

	 	setUV4(&(symbols[i]),
				symbolTex->u0, symbolTex->v0, 
				symbolTex->u1, symbolTex->v1, 
				symbolTex->u2, symbolTex->v2+1, 
				symbolTex->u3, symbolTex->v3+1) ;

 		setTPage(&(symbols[i]),0,0,symbolTex->textureX,symbolTex->textureY);

		if (colour == WhiteText)
  			symbols[i].clut = symbolTex->cba;
		else
		{
			redTex = (Texture *) TextureTable [DRTEXT_RED] ;
  			symbols[i].clut = redTex->cba;
		}
 
 		AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(symbols[i]) );

		
	}

}
 	


void CentreText(char* the_text_ptr, TEXT_DATA *pos_data, int colour)
{
	uchar 	letter=0;
	char 		AValue=0, *tmpPtr ;
	Texture 	*charTex, *redTex ;
	int 		size=0 ;
	int 		x=0, y=0, i=0 , newx , newy;
	short		vramRow=0, vramY=0, vramX=0 ;
	short 	j=0;
	ushort	 currLett=0, searchLett=0 ;
	TexChar	*charSet ;
	short		charNum ;	
	short		totSize = 0 ;	

	y = pos_data->start_pos_y;

//	y*=upres;

	size = pos_data->size ;

	switch (size)
	{
		case 16:	charSet = &(CharSet16[0]) ;	
					charTex = (Texture *) TextureTable [DRTEXT_16] ;
					break ;

		case 12:	charSet = &(CharSet12[0]) ;
					charTex = (Texture *) TextureTable [DRTEXT_12] ;
					break ;

		case  8:	charSet = &(CharSet8[0]) ;
					charTex = (Texture *) TextureTable [DRTEXT_8] ;
					break ;
	}



	tmpPtr = the_text_ptr ;

	letter = *tmpPtr ;

	while (letter != 0)
	{
		if(letter>='0' && letter<='9')
			j=letter-'0'+26;
		else
			j = letter - 'A';

		if (letter != 32)
			totSize += charSet[j].width ; 
		else
			totSize += 8 ;

		tmpPtr ++ ;
		letter = *tmpPtr ;
	}

	x = 160 - (totSize>>1) ;

	tmpPtr = the_text_ptr;
	while(1)
	{
		letter = *tmpPtr++;

		if (letter != 32)
		{
			if(letter == 0) return;

			AValue = 'A';

			if(letter>='0' && letter<='9')
				j=letter-'0'+26;
			else
				j = letter - AValue;

			currLett = charSet[j].width ;

			newx = x*upres + ((currLett*upres)-currLett)/2;
			newy = y*upres;

			setXY0(&display_text[textPrim],newx,newy);	// PCwipeout
			setUV0(&display_text[textPrim],charSet[j].vramX, charSet[j].vramY);	// PCwipeout
			setWH(&display_text[textPrim],currLett,size);	// PCwipeout
									
			if (colour == WhiteText)
			{
				display_text[textPrim].r0 = 230;
				display_text[textPrim].g0 = 0;
				display_text[textPrim].b0 = BLACK_NOT_DRAWN;
	   			display_text[textPrim].clut = charTex->cba;
			}
			else
			{
				redTex = (Texture *) TextureTable [DRTEXT_RED] ;
				display_text[textPrim].r0 = 230;
				display_text[textPrim].g0 = BRIGHT_RED_TRANSL;
				display_text[textPrim].b0 = 0;
   				display_text[textPrim].clut = redTex->cba;
			}

   			AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(display_text[textPrim]) );

			textPrim++;
			x += currLett ;
		}
		else
		{
		 	x += 8 ;
		}
	}	
}


void DisplayLives (SPRT lifeIcons[][2], ConfigData *gameData) // PCwipeout
{

	int i ;
 	Texture	*livesTex ;
	short		ySpace = 13 ;

	livesTex = (Texture *) TextureTable [DRTEXT_RED+5] ;
 
	for (i=0; i < gameData->Lives; i++)
	{
		SetSprt(&(lifeIcons[i][CurrentScreen]));			
		lifeIcons[i][CurrentScreen].r0=230;
		lifeIcons[i][CurrentScreen].g0=0;
		lifeIcons[i][CurrentScreen].b0=BLACK_NOT_DRAWN;

#if PAL
		setXY0(&(lifeIcons[i][CurrentScreen]),294-(ySpace*i),215);
		setWH(&(lifeIcons[i][CurrentScreen]),11,9);
#else
		setXY0(&(lifeIcons[i][CurrentScreen]),speedo_x[screenres]+115-(ySpace*i),speedo_y[screenres]-5);
		setWH(&(lifeIcons[i][CurrentScreen]),11,9);
#endif

		setUV0(&(lifeIcons[i][CurrentScreen]),livesTex->u0, livesTex->v0);
	
		lifeIcons[i][CurrentScreen].clut = livesTex->cba ;

  		AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(lifeIcons[i][CurrentScreen]) );

	}


}

void InitChampTable ()
{
	strcpy (winTable[0].pilot,"JOHN DEKKA") ;
	strcpy (winTable[1].pilot,"DANIEL CHANG") ;
	strcpy (winTable[2].pilot,"ARIAL TETSUO") ;
	strcpy (winTable[3].pilot,"ANASTASIA CHEROVOSKI") ;
	strcpy (winTable[4].pilot,"KEL SOLAAR") ;
	strcpy (winTable[5].pilot,"ARIAN TETSUO") ;
	strcpy (winTable[6].pilot,"SOFIA DE LA RENTE") ;
	strcpy (winTable[7].pilot,"PAUL JACKSON") ;
}

void DisplayChampTable ()	  					  
{
	short		i ;
	short		yPos = 80 ;
	short		screenPos = 3 ;
	static int toggle = 0 ;

	CentreText ("CHAMPIONSHIP TABLE",text_data (60, 40, 8),RedText) ;
	AddText ("PILOT NAME",text_data (85, 65, 8),RedText) ;
	AddText ("POINTS",text_data (240, 65, 8),RedText) ;


	for (i=0; i < NO_OF_SHIPS; i++)
	{

		if (i == ownShip)
		{
			if ((toggle % 2) == 0)
			{
				AddText (winTable[i].pilot,text_data (35, yPos+((winTable[i].position)*15), 8),WhiteText) ;
				AddNumber (winTable[i].points,text_data (255, yPos+((winTable[i].position)*15), 8),WhiteText) ;
			}
			else
			{
				AddText (winTable[i].pilot,text_data (35, yPos+((winTable[i].position)*15), 8),RedText) ;
				AddNumber (winTable[i].points,text_data (255, yPos+((winTable[i].position)*15), 8),RedText) ;
			}
		}
		else
		{
			AddText (winTable[i].pilot,text_data (35, yPos+((winTable[i].position)*15), 8),WhiteText) ;
			AddNumber (winTable[i].points,text_data (255, yPos+((winTable[i].position)*15), 8),WhiteText) ;
		}
	}
	toggle ++ ;

}


void DisplayRacePoints ()	  					  
{
	short		i ;
	short		yPos = 80 ;
	static 	int		toggle = 0 ;

	CentreText ("RACE POINTS",text_data (60, 40, 8),RedText) ;
	AddText ("PILOT NAME",text_data (85, 65, 8),RedText) ;
	AddText ("POINTS",text_data (240, 65, 8),RedText) ;


	for (i=0; i < NO_OF_SHIPS; i++)
	{

		if (i == ownShip)
		{
			if ((toggle % 2) == 0)
			{
				AddText (winTable[i].pilot,text_data (35, yPos+((winTable[i].racePosition)*15), 8),WhiteText) ;
				AddNumber (winTable[i].racePoints,text_data (255, yPos+((winTable[i].racePosition)*15), 8),WhiteText) ;
			}
			else
			{
				AddText (winTable[i].pilot,text_data (35, yPos+((winTable[i].racePosition)*15), 8),RedText) ;
				AddNumber (winTable[i].racePoints,text_data (255, yPos+((winTable[i].racePosition)*15), 8),RedText) ;
			}
		}
		else
		{
			AddText (winTable[i].pilot,text_data (35, yPos+((winTable[i].racePosition)*15), 8),WhiteText) ;
			AddNumber (winTable[i].racePoints,text_data (255, yPos+((winTable[i].racePosition)*15), 8),WhiteText) ;
		}
	}

	toggle ++ ;

}


void InitScreenTex (Texture **screenTex, char *fileName)
{
	TIMlist  		*timPtr;
	short				tablePos ;
	char 				modelFile[256];

	strcpy( modelFile, "wipeout\\textures\\" );
	strcat( modelFile, fileName) ;
	strcat( modelFile, ".cmp");

	tablePos = TextureTableCount;

	timPtr = LoadCompressedTextureSequence( modelFile );
	LoadCmpFiles( timPtr ); 

	screenTex[0] = TextureTable [tablePos++] ;
	screenTex[1] = TextureTable [tablePos++] ;

	// AND NOW LOAD IN THE PROPER PICCIES FOR PC WIPEOUT

	strcpy( modelFile, "wipeout\\newgraph\\" );
//	strcat( modelFile, trackdir) ;
	strcat( modelFile, fileName) ;
	LoadPilotIntoVram(modelFile);
	strcpy(globalPilot,modelFile);
}

void DisplayWinLose (Texture *winLose, POLY_FT4 *picPrim, short x, short y, short height, char type)
{
		short width = 128 ;		

		SetPolyFT4(&(picPrim[CurrentScreen]));			
		picPrim[CurrentScreen].r0=230 ; // PCwipeout
		picPrim[CurrentScreen].g0=0 ;
		picPrim[CurrentScreen].b0=BLACK_NOT_DRAWN ;


		setXY4(&(picPrim[CurrentScreen]),x, y, x + width, y, x, y + height, x + width, y + height);
	
		setUV4(&(picPrim[CurrentScreen]),
			winLose->u0, winLose->v0, 
			winLose->u1, winLose->v1, 
			winLose->u2, winLose->v2+1, 
			winLose->u3, winLose->v3+1) ;

		setPAD4(&(picPrim[CurrentScreen]),0x4000);
		
 		setTPage(&(picPrim[CurrentScreen]),1,0,winLose->textureX,winLose->textureY);
	
//		picPrim[CurrentScreen].clut = winLose->cba ; // PCwipeout

 		AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(picPrim[CurrentScreen]) );
}


void BackPoly (POLY_F4* selectBox, DR_MODE* selectTrans, short x, short y, short w, short h)
{

	SetPolyF4(&(selectBox[CurrentScreen]));			
	selectBox[CurrentScreen].r0=230; // PCwipeout
	selectBox[CurrentScreen].g0=0;
	selectBox[CurrentScreen].b0=BLACK3_TRANSL;
	
	setXY4(&(selectBox[CurrentScreen]),x , y,
								  x+w , y,
								  x , y+h,
								  x+w , y+h);

	SetSemiTrans(&(selectBox[CurrentScreen]),	1);

	SetDrawMode(&(selectTrans[CurrentScreen]), 1, 1, FONT_TPAGE, 0);


  	AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(selectBox[CurrentScreen]) );
  	AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(selectTrans[CurrentScreen]) );

}

void DisplayVenomComplete () 							 	
{

	CentreText ("WELL DONE",text_data (60, 40, 8),WhiteText) ;

	CentreText ("VENOM CLASS",text_data (85, 65, 8),WhiteText) ;
	CentreText ("COMPETENCE ACHIEVED",text_data (85, 77, 8),WhiteText) ;
	CentreText ("YOU HAVE NOW QUALIFIED",text_data (240, 89, 8),WhiteText) ;
	CentreText ("FOR THE ULTRA FAST",text_data (240, 101, 8),WhiteText) ;
	CentreText ("RAPIER CLASS",text_data (240, 125, 8),WhiteText) ;
	CentreText ("WE RECOMMEND YOU",text_data (240, 200, 8),WhiteText) ;
	CentreText ("SAVE YOUR CURRENT GAME",text_data (240, 212, 8),WhiteText) ;

}

void DisplayAllVenomComplete () 							 	
{

	CentreText ("AMAZING",text_data (60, 40, 8),RedText) ;

	CentreText ("YOU HAVE COMPLETED THE FULL",text_data (85, 65, 8),WhiteText) ;
	CentreText ("VENOM CLASS CHAMPIONSHIP",text_data (240, 77, 8),WhiteText) ;
	CentreText ("WELL DONE",text_data (240, 89, 8),WhiteText) ;
	CentreText ("YOU ARE A GREAT PILOT",text_data (240, 114, 8),WhiteText) ;
	CentreText ("NOW TAKE ON THE FULL",text_data (240, 126, 8),WhiteText) ;
	CentreText ("RAPIER CLASS CHAMPIONSHIP",text_data (240, 138, 8),WhiteText) ;
	CentreText ("KEEP GOING",text_data (240, 162, 8),WhiteText) ;

}

void InitTrackTex (Texture **trackTex)
{

	TIMlist  		*timPtr;
	short				tablePos ;
#if USE_ATI
	Texture *t;
#endif
	tablePos = TextureTableCount;

	timPtr = LoadCompressedTextureSequence( "wipeout\\textures\\track.cmp" );
	LoadCmpFiles( timPtr ); 

	trackTex[0] = TextureTable [tablePos++] ;
	trackTex[1] = TextureTable [tablePos++] ;
	trackTex[2] = TextureTable [tablePos++] ;
	trackTex[3] = TextureTable [tablePos++] ;
	trackTex[4] = TextureTable [tablePos++] ;
	trackTex[5] = TextureTable [tablePos++] ;
	trackTex[6] = TextureTable [tablePos++] ;
#if USE_ATI
	t=trackTex[0];
	t->tsb = 0xa5;
	t->textureX=320;
	t->textureY=0;
	t->u0=128;
	t->v0=148;
	t->u1=255;
	t->v1=148;
	t->u2=128;
	t->v2=221;
	t->u3=255;
	t->v3=221;
	t=trackTex[1];
	t->tsb = 0xa5;
	t->textureX=320;
	t->textureY=0;
	t->u0=128;
	t->v0=74;
	t->u1=255;
	t->v1=74;
	t->u2=128;
	t->v2=147;
	t->u3=255;
	t->v3=147;
	t=trackTex[2];
	t->tsb = 0xa5;
	t->textureX=320;
	t->textureY=0;
	t->u0=0;
	t->v0=148;
	t->u1=127;
	t->v1=148;
	t->u2=0;
	t->v2=221;
	t->u3=127;
	t->v3=221;
	t=trackTex[3];
	t->tsb = 0xa5;
	t->textureX=320;
	t->textureY=0;
	t->u0=128;
	t->v0=0;
	t->u1=255;
	t->v1=0;
	t->u2=128;
	t->v2=73;
	t->u3=255;
	t->v3=73;
#endif
}

void DisplayRaceInfo (ConfigData *gameData, HiScoreData *hiScoreData)
{
	short i ;
	static int	toggle = 0 ;

	AddText ("RACE STATISTICS",text_data (15, 80, 8),RedText) ;

	for (i=0; i < 3; i++)
	{

		AddText ("LAP",text_data (25, 95+(i*12), 8),RedText) ;
		AddNumber ((i+1), text_data (65, 95+(i*12), 8), RedText) ; 
		AddTime(tempLapTimes[i], text_data (85, 95+(i*12), 8), WhiteText);

	}

	
	AddText ("RACE POSITION",text_data (15, 50, 8),RedText) ;
	AddNumber (gameData->Position, text_data (155, 50, 8), WhiteText) ; 


	if (gameData->gameType == SINGLE_RACE)
	{
		if ((CheckHiScore (gameData, hiScoreData)) && gameData->Position == 1)
		{
			if ((toggle % 30) <= 20)
			{
				AddText ("RACE RECORD",text_data (15, 170, 8),RedText) ;
				AddTime(hiScoreData->bestLap, text_data (25, 182, 8), WhiteText);
			}
		}
		else
		{
				AddText ("RACE TIME",text_data (15, 170, 8),RedText) ;
				AddTime(hiScoreData->bestLap, text_data (25, 182, 8), WhiteText);
		}
	}
	else
		if (gameData->gameType == CHAMPIONSHIP)
		{
			if ((CheckHiScore (gameData, hiScoreData)) && (gameData->Position >= 1 &&
				  gameData->Position <= 3))
			{
				if ((toggle % 30) <= 20)
				{
					AddText ("RACE RECORD",text_data (15, 170, 8),RedText) ;
					AddTime(hiScoreData->bestLap, text_data (25, 182, 8), WhiteText);
				}
			}
			else
			{
					AddText ("RACE TIME",text_data (15, 170, 8),RedText) ;
					AddTime(hiScoreData->bestLap, text_data (25, 182, 8), WhiteText);
			}
		}
		else
		if (gameData->gameType == TIME_TRIAL)
		{
			if (CheckHiScore (gameData, hiScoreData))
			{
				if ((toggle % 30) <= 20)
				{
					AddText ("RACE RECORD",text_data (15, 170, 8),RedText) ;
					AddTime(hiScoreData->bestLap, text_data (25, 182, 8), WhiteText);
				}
			}
			else
			{
					AddText ("RACE TIME",text_data (15, 170, 8),RedText) ;
					AddTime(hiScoreData->bestLap, text_data (25, 182, 8), WhiteText);
			}
		}
		else
		{
			if (CheckHiScore (gameData, hiScoreData))
			{
				if ((toggle % 30) <= 20)
				{
					AddText ("RACE RECORD",text_data (15, 170, 8),RedText) ;
					AddTime(hiScoreData->bestLap, text_data (25, 182, 8), WhiteText);
				}
			}
			else
			{
					AddText ("RACE TIME",text_data (15, 170, 8),RedText) ;
					AddTime(hiScoreData->bestLap, text_data (25, 182, 8), WhiteText);
			}
		}

	if (gameData->gameType == CHAMPIONSHIP)
	{
		if (gameData->newLapRecord == 1)
		{
/*
			printf ("track in showinfo = %d \n",gameData->trackNum ) ;
*/
			if ((toggle % 30) <= 20)
			{
				AddText ("LAP RECORD",text_data (15, 194, 8),RedText) ;
				if (gameData->Position >= 1 && gameData->Position <= 3)
					AddTime(scoreTable[((gameData->trackNum - 1)-gameData->trackInc)].lapRecord, text_data (25, 206, 8), WhiteText);
				else
					AddTime(scoreTable[((gameData->trackNum - 1)-gameData->trackInc)].lapRecord, text_data (25, 206, 8), WhiteText);
			}

//			printf ("Lap record = %d   track num = %d \n",
//						scoreTable[((gameData->trackNum - 1)-gameData->trackInc)].lapRecord,
//						gameData->trackNum) ;

		}
		else
		{
			AddText ("LAP RECORD",text_data (15, 194, 8),RedText) ;
			AddTime(scoreTable[((gameData->trackNum - 1)-gameData->trackInc)].lapRecord, text_data (25, 206, 8), WhiteText);
		}
	}
	else
	if (gameData->gameType == TIME_TRIAL)
	{
		if (gameData->newLapRecord == 1)
		{
/*
			printf ("track in showinfo = %d \n",gameData->trackNum ) ;
*/
			if ((toggle % 30) <= 20)
			{
				AddText ("LAP RECORD",text_data (15, 194, 8),RedText) ;
				AddTime(scoreTable[((gameData->trackNum - 1)+ HiScoreTracks)].lapRecord, text_data (25, 206, 8), WhiteText);
			}

/*
			printf ("Lap record = %d   track num = %d \n",
						scoreTable[(gameData->trackNum - 1)].lapRecord,
						gameData->trackNum) ;
*/
		}
		else
		{
			AddText ("LAP RECORD",text_data (15, 194, 8),RedText) ;
			AddTime(scoreTable[((gameData->trackNum - 1)+HiScoreTracks)].lapRecord, text_data (25, 206, 8), WhiteText);
		}
	}
	else
	{
		if (gameData->newLapRecord == 1)
		{
/*
			printf ("track in showinfo = %d \n",gameData->trackNum ) ;
*/
			if ((toggle % 30) <= 20)
			{
				AddText ("LAP RECORD",text_data (15, 194, 8),RedText) ;
				AddTime(scoreTable[(gameData->trackNum - 1)].lapRecord, text_data (25, 206, 8), WhiteText);
			}

/*
			printf ("Lap record = %d   track num = %d \n",
						scoreTable[(gameData->trackNum - 1)].lapRecord,
						gameData->trackNum) ;
*/
		}
		else
		{
			AddText ("LAP RECORD",text_data (15, 194, 8),RedText) ;
			AddTime(scoreTable[(gameData->trackNum - 1)].lapRecord, text_data (25, 206, 8), WhiteText);
		}

	}
	toggle ++ ;

}


void DrawHud(ShipData* shipIndex, Speedo *speedo, int choice, ConfigData *gameData)
{
int i;
int width;
static char fixof[]={0,2,3,4};

	if(shipIndex[ownShip].lapNo > 0)	//gone past start line
	{
		if(shipIndex[ownShip].attr & RACING)	//in race mode
		{
			if(shipIndex[ownShip].lapNo < END_LAP+1)	//not the last lap
				AddTime(shipIndex[ownShip].lapTimes[shipIndex[ownShip].lapNo-1], text_data (16, 210, 16), WhiteText);
			else
				AddTime(shipIndex[ownShip].lapTimes[2], text_data (16, 210, 16), WhiteText);
		}
	}
	if(shipIndex[ownShip].attr & RACING)
	{
		if(shipIndex[ownShip].lapNo > 1)
		{
			for(i = 1; i < shipIndex[ownShip].lapNo; i++)
			{  	
				if (i < END_LAP)
				{
					AddTime(shipIndex[ownShip].lapTimes[i-1],
							 text_data (16, 205 - (10 * i), 8), RedText);
				}
			}
		}
	}



	if(shipIndex[ownShip].attr & RACING)
	{
		if (gameType == MASTER || gameType == SLAVE)
		{
			AddText ("P", text_data (260, 50, 8), RedText) ;
			AddNumber (2, text_data (274, 50, 8), RedText) ; 	
			AddNumber (winTable[serialShip].racePosition, text_data (290, 50, 8), WhiteText) ; 	
		}


		AddText ("LAP", text_data (15, 8, 8), RedText) ; // PCwipeout
		AddNumber (shipIndex[ownShip].lapNo, text_data (10, 19, 16), WhiteText) ; 	// PCwipeout

		width = Text16[(shipIndex[ownShip].lapNo+26)] % 100 ;

		AddText ("OF", text_data ((10+width), 27-fixof[screenres], 8), RedText) ;

		width += 22 ;

		AddNumber (END_LAP, text_data ((10+width), 19, 16), WhiteText) ; 	


		if(raceType != TIME_TRIAL)
		{
			AddText ("POSITION", text_data (230, 8, 8), RedText) ;
			AddNumber (winTable[ownShip].racePosition, text_data (260, 19, 16), WhiteText) ; 	
		}

#if 1
		if(FrameState)
		{
			AddText ("FRAME", text_data (230, 41, 8), RedText) ; // PCwipeout
			AddText ("RATE", text_data (235, 52, 8), RedText) ; // PCwipeout
			AddNumber ((theFrameRate / 10), text_data (245, 63, 16), WhiteText) ; 	
			AddNumber ((theFrameRate % 10), text_data (275, 63, 16), WhiteText) ; 	
		}	
#endif


		/* pc WIPEOUT TEXT BIT */
//		CentreText("WORK IN PROGRESS",text_data(0,74,8),RedText);

		AddText ("LAP RECORD", text_data (15, 43, 8), RedText) ;
		if (gameData->gameType == TIME_TRIAL)
			AddTime(scoreTable[((gameData->trackNum-1)+HiScoreTracks)].lapRecord,
							 text_data (15, 55, 8), WhiteText);
		else
			AddTime(scoreTable[(gameData->trackNum-1)].lapRecord,
							 text_data (15, 55, 8), WhiteText);
		if((shipIndex[ownShip].attr & DIRECTION) == 0)
		{
			CentreText ("WRONG WAY", text_data (0, 110, 16), RedText) ;
		}	

	}

	if(shipIndex[ownShip].attr & RACING)
	{
		int j;

		for(j=0;j<2;++j)
		{
			setXY0(&(speedo->facia[j]),speedo_x[screenres],speedo_y[screenres]);
			setXY0(&(speedo->transbar[j]),speedo_x[screenres],speedo_y[screenres]);
			setXY0(&(speedo->solidbar[j]),speedo_x[screenres],speedo_y[screenres]);
		}
 		AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(speedo->facia[CurrentScreen]) );

	 	if(CameraUpdate == UpdateCameraAttractInternal)
			fill_speedo(speedo, shipIndex[ownShip].speed*7, shipIndex[ownShip].thrust_mag);
		else
			fill_speedo(speedo, shipIndex[ownShip].speed, shipIndex[ownShip].thrust_mag);
	}
}

