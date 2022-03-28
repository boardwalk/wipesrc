#include <libspu.h> 
#include <libsnd.h>
#include "standard.h"
#include <sys\file.h>
#include <sys\ioctl.h>

#define	LANGUAGE_C	// PCwipeout	
#include <asm.h>		// PCwipeout

#include <kernel.h>

  

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "combat.h"
#include "menus.h"
#include "handlers.h"
#include "dynam.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"


#define WriteSIOData( b, l )			write( fw, b, l )
#define ReadSIOData( b, l )			read( fr, b, l )	  


char 							first = SINGLE ;
long 				fr,fw;		/* file descriptor */
static unsigned long 	ev1, ev2;	/* read/write event descriptor */
static unsigned long 	linkev1, linkev2;	/* read/write event descriptor */

static long 				initfr,initfw;		/* file descriptor */
static unsigned long 	initev1, initev2;	/* read/write event descriptor */

static unsigned long 	errorev ;	/* error event descriptor */
static unsigned long 	timeev ;	/* timeout event descriptor */

static unsigned long 	ev_r, ev_e, ev_w; /* read/error event descriptor */

int	 writeFlag = 0 ;
static readMFlag = 1 ;
static readSFlag = 1 ;


uchar 				transmitBuf[DATA_PACKET_SIZE] ;	/* transmit buffer */
uchar 				receiveBuf[DATA_PACKET_SIZE] ;	/* receive buffer */








 
void SetSerialData(ShipData *shipIndex, Object **shipShapes)
{
	#ifdef	PCwipeout
	int				i, j;
	TrackSection   *section;
   register short    sx, sy, sz;
   register short    cx, cy, cz;


 	for(i= 0; i < NO_OF_SHIPS; i++)
 	{
	  	sy = rsin( shipIndex[i].hdg );
	 	cy = rcos( shipIndex[i].hdg );
	 	sx = rsin( shipIndex[i].pitch );
	 	cx = rcos( shipIndex[i].pitch );
 		sz = rsin( shipIndex[i].roll );
  		cz = rcos( shipIndex[i].roll );

 		if (gameType == MASTER)
 		{
 			if(i == serialShip)
 			{
				SetSkeletonPosition( shipShapes[ shipIndex[i].highResTableIndex ]->skeleton,
					shipIndex[i].ppivot.vx,
	  				shipIndex[i].ppivot.vy,
					shipIndex[i].ppivot.vz);

				SetSkeletonDirectionHPR(shipShapes[ shipIndex[i].highResTableIndex ]->skeleton,
					 shipIndex[i].hdg,
					 shipIndex[i].pitch,
					 shipIndex[i].roll );

   			shipIndex[i].unitVecNose.vx = -frac(sy,cx);	 
 				shipIndex[i].unitVecNose.vy = -sx;
				shipIndex[i].unitVecNose.vz = frac(cy,cx);

				shipIndex[i].unitVecWing.vx = ((cy * cz) >> 12) + ((((sy * sz) >> 12) * sx) >> 12);
				shipIndex[i].unitVecWing.vy = -(sz * cx) >> 12;
				shipIndex[i].unitVecWing.vz = ((sy * cz) >> 12) - ((((cy * sx) >> 12) * sz) >> 12);
#if 1
				shipIndex[serialShip].vpivot.vx = (shipIndex[serialShip].ppivot.vx - shipIndex[serialShip].vpivot.vx)<<6;
				shipIndex[serialShip].vpivot.vy = (shipIndex[serialShip].ppivot.vy - shipIndex[serialShip].vpivot.vy)<<6;
				shipIndex[serialShip].vpivot.vz = (shipIndex[serialShip].ppivot.vz - shipIndex[serialShip].vpivot.vz)<<6;
#endif
			}
		}
		else if (gameType == SLAVE)
		{
			if(i != ownShip)
			{
				SetSkeletonPosition( shipShapes[ shipIndex[i].highResTableIndex ]->skeleton,
					shipIndex[i].ppivot.vx,
	  				shipIndex[i].ppivot.vy,
					shipIndex[i].ppivot.vz);

				SetSkeletonDirectionHPR(shipShapes[ shipIndex[i].highResTableIndex ]->skeleton,
					 shipIndex[i].hdg,
					 shipIndex[i].pitch,
					 shipIndex[i].roll );

   			shipIndex[i].unitVecNose.vx = -frac(sy,cx);	 
 				shipIndex[i].unitVecNose.vy = -sx;
				shipIndex[i].unitVecNose.vz = frac(cy,cx);

				shipIndex[i].unitVecWing.vx = ((cy * cz) >> 12) + ((((sy * sz) >> 12) * sx) >> 12);
				shipIndex[i].unitVecWing.vy = -(sz * cx) >> 12;
				shipIndex[i].unitVecWing.vz = ((sy * cz) >> 12) - ((((cy * sx) >> 12) * sz) >> 12);
#if 1
				shipIndex[i].vpivot.vx = (shipIndex[i].ppivot.vx - shipIndex[i].vpivot.vx)<<6;
				shipIndex[i].vpivot.vy = (shipIndex[i].ppivot.vy - shipIndex[i].vpivot.vy)<<6;
				shipIndex[i].vpivot.vz = (shipIndex[i].ppivot.vz - shipIndex[i].vpivot.vz)<<6;
#endif
			}							   
		}

	}
	section = track->sections;
	for(i =  0; i < track->sectionCount; i++)
	{
		if (gameType == SLAVE)
		{
			for(j  = 0; j < NO_OF_SHIPS; j++)
			{
				if(j != ownShip)
				{
					if(shipIndex[j].nearSecNo == section->secNo)
					{
						shipIndex[j].nearTrkSect = section;
						break;
					}
				}
			}
		}
		if (gameType == MASTER)
		{
		 	if(shipIndex[serialShip].nearSecNo == section->secNo)
		 	{
		 		shipIndex[serialShip].nearTrkSect = section;
		 		break;
		 	}
		}
		section = section->nextSection.ptr;
	}
#endif
}

#if 0

/*****************************/
/****** NEW MASTER CODE ******/
/*****************************/

									   
void NewMaster (combatData *packetData, ShipData *shipInfo)
{
	ulong 	*long_ptr ;
	ushort 	*short_ptr ;
	uchar		send[8], rec[8] ;

	static 	writeFlag = 1 ;
/*	static 	readFlag = 1 ;*/

	char 		packetCount = 0 ;
	char		buffCnt = 0 ;
	char 		i ;
	int		j ;

	if (writeFlag)
	{
		long_ptr = (ulong *) &(transmitBuf[0]) ;

		for (i=0; i < NO_OF_SHIPS; i++)
		{
			if (i != serialShip)
			{
				*long_ptr++ = shipInfo[i].ppivot.vx ;
				*long_ptr++ = shipInfo[i].ppivot.vy ;
				*long_ptr++ = shipInfo[i].ppivot.vz ;
				short_ptr = (ushort *)long_ptr ;
				*short_ptr++ = shipInfo[i].hdg ;
				*short_ptr++ = shipInfo[i].pitch ;
				*short_ptr++ = shipInfo[i].roll ;
				*short_ptr++ = shipInfo[i].haveFired;
				*short_ptr++ = shipInfo[i].nearSecNo ;
				*short_ptr++ =  shipInfo[i].prevSecNo;
				long_ptr = (ulong *)short_ptr ;
			}
		}

		while (packetCount != 21)
		{
			if (_comb_control (3,0,0))
			{
#if 1
				for (i=0; i < 8; i++)
					send[i] = transmitBuf[(buffCnt+i)] ;
	   							 
				write (fw, send, 8) ;
				while (TestEvent(ev_w) != 1) 
				{
				}

/*				for (j=0; j < 30000; j++) ;*/
#else
				write (fw, transmitBuf, 168) ;
#endif
				writeFlag = 0 ;
/*				printf ("Master Wrote data \n") ;*/
		 		packetCount ++ ;
				buffCnt += 8 ;
			}
		}
		
	}
	else
	{
		while (packetCount != 3)
		{
			if (TestEvent(ev_r) == 1)
			{
			 	writeFlag = 1 ;
				readMFlag = 1 ;

#if 1
				for (i=0; i < 8; i++)
					receiveBuf[(buffCnt + i)] = rec[i] ;
#endif
				packetCount ++ ;
				buffCnt += 8 ;
/*				printf ("Master Read data \n") ;*/

			}
			else
			{
				EnterCriticalSection () ;
					if (readMFlag)
					{
#if 1
					 	if (read(fr, rec, 8) == -1)
#else
					 	if (read(fr, receiveBuf, 24) == -1)
#endif
						{
						   _comb_control(2,3,0);
							printf ("Master Read error \n") ;
						}
						readMFlag = 0 ;
					}
				ExitCriticalSection () ;
	
			}
		}

		long_ptr = (ulong *) &(receiveBuf[0]) ;

		shipInfo[serialShip].ppivot.vx = *long_ptr ++ ;
		shipInfo[serialShip].ppivot.vy = *long_ptr ++ ;
		shipInfo[serialShip].ppivot.vz = *long_ptr ++ ;
		short_ptr = (ushort *)long_ptr ;
		shipInfo[serialShip].hdg = *short_ptr ++ ;
		shipInfo[serialShip].pitch = *short_ptr ++ ;
		shipInfo[serialShip].roll = *short_ptr ++ ;
		shipInfo[serialShip].haveFired = *short_ptr ++ ;
		shipInfo[serialShip].nearSecNo = *short_ptr ++;
		shipInfo[serialShip].prevSecNo = *short_ptr ;

	}
}



/****************************/
/****** NEW SLAVE CODE ******/
/****************************/


void NewSlave (combatData *packetData, ShipData *shipInfo)
{
	ulong 	*long_ptr ;
	ushort 	*short_ptr ;
	uchar		send[8], rec[8] ;

	static 	writeFlag = 0 ;
/*	static 	readFlag = 1 ;*/

	char		packetCount = 0 ;
	char		buffCnt = 0 ;
	char 		i ;
	int		j ;

	if (writeFlag)
	{

		long_ptr = (ulong *) &(transmitBuf[0]) ;

		*long_ptr++ = shipInfo[ownShip].ppivot.vx ;
		*long_ptr++ = shipInfo[ownShip].ppivot.vy ;
		*long_ptr++ = shipInfo[ownShip].ppivot.vz ;
		short_ptr = (ushort *)long_ptr ;
		*short_ptr++ = shipInfo[ownShip].hdg ;
		*short_ptr++ = shipInfo[ownShip].pitch ;
		*short_ptr++ = shipInfo[ownShip].roll ;
		*short_ptr++ = shipInfo[ownShip].haveFired ;
		*short_ptr++ = shipInfo[ownShip].nearSecNo ;
		*short_ptr = shipInfo[ownShip].prevSecNo ;

		while (packetCount != 3)
		{
			if (_comb_control (3,0,0))
			{
  
#if 1
				for (i=0; i < 8; i++)
					send[i] = transmitBuf[(buffCnt + i)] ;

				write (fw, send, 8) ;

				while (TestEvent(ev_w) != 1) 
				{
				}

/*				for (j=0; j < 30000; j++) ;*/
#else
				write (fw, transmitBuf, 24) ;
#endif
				writeFlag = 0 ;
/*				printf ("Slave Wrote data \n") ;*/
		 		packetCount ++ ;
				buffCnt += 8 ;
			}
		}	
	}
	else
	{
		while (packetCount != 21)
		{
			if (TestEvent(ev_r) == 1)
			{
			 	writeFlag = 1 ;
			 	readSFlag = 1 ;
#if 1
				for (i=0; i < 8; i++)
					receiveBuf[(buffCnt + i)] = rec[i] ;
#endif
				packetCount ++ ;
				buffCnt += 8 ;
/*				printf ("Slave Read data \n") ;*/

			}
			else
			{
			EnterCriticalSection () ;
  	
				if (readSFlag)
				{
#if 1
				 	if (read(fr, rec, 8) == -1)
#else
				 	if (read(fr, receiveBuf, 168) == -1)
#endif
					{
					   _comb_control(2,3,0);
						printf ("Slave Read error \n") ;
					}
					readSFlag = 0 ;
				}
			ExitCriticalSection () ;
			
			}
		}

		long_ptr = (ulong *) &(receiveBuf[0]) ;

		for (i=0; i < NO_OF_SHIPS; i++)
		{
			if ( i != ownShip )
			{
												 
				shipInfo[i].ppivot.vx = *long_ptr ++ ;
				shipInfo[i].ppivot.vy = *long_ptr ++ ;
				shipInfo[i].ppivot.vz = *long_ptr ++ ;
				short_ptr = (ushort *)long_ptr ;
				shipInfo[i].hdg = *short_ptr ++ ;
				shipInfo[i].pitch = *short_ptr ++ ;
				shipInfo[i].roll = *short_ptr ++ ;
				shipInfo[i].haveFired = *short_ptr ++ ;
				shipInfo[i].nearSecNo = *short_ptr ++;
				shipInfo[i].prevSecNo = *short_ptr ++ ;
				long_ptr = (ulong *)short_ptr ;
			}
		}

	}
	
}

#endif




void NewMaster (combatData *packetData, ShipData *shipInfo)
{
#ifdef	PCwipeout
	ulong 	*long_ptr ;
	ushort 	*short_ptr ;
	short		i ;
	u_long	cts ;
	int		loopTime ;	 

#if 0
	loopTime = GsGetVcount () ;
#endif

	long_ptr = (ulong *) &(transmitBuf[0]) ;

	for (i=0; i < NO_OF_SHIPS; i++)
	{
		if (i != serialShip)
		{
			*long_ptr++ = shipInfo[i].ppivot.vx ;
			*long_ptr++ = shipInfo[i].ppivot.vy ;
			*long_ptr++ = shipInfo[i].ppivot.vz ;
			short_ptr = (ushort *)long_ptr ;
			*short_ptr++ = shipInfo[i].hdg ;
			*short_ptr++ = shipInfo[i].pitch ;
			*short_ptr++ = shipInfo[i].roll ;
			*short_ptr++ = shipInfo[i].haveFired;
			*short_ptr++ = shipInfo[i].nearSecNo ;
			*short_ptr++ =  shipInfo[i].prevSecNo;
			long_ptr = (ulong *)short_ptr ;
		}
	}


	SetRTSDTR( SIOCL_DTRON | SIOCL_RTSON );	/* Signal that data is waiting */
	
	cts = ReadCTS();
	while( cts == 0 )		 							/* wait for OK to send */
	{
/*		pollhost();*/
		cts = ReadCTS();
	}

	cts = 0;
	while( cts < 200 )				 /*100*/
		cts++;

	WriteSIOData( &(transmitBuf[0]), DATA_PACKET_SIZE );

	cts = ReadCTS();
	while( cts == 1 ) 								/* wait for acknowledge */
	{
/*		pollhost();*/
		cts = ReadCTS();
	}

	SetRTSDTR( 0 );									/* remove signals */


	cts = ReadDSR();
	while( cts == 0 )
	{
/*		pollhost();*/
		cts = ReadDSR();
	}

	/* Read Data */
	EnterCriticalSection();

	SetRTSDTR( SIOCL_DTRON | SIOCL_RTSON );		/* Set OK to send */

	ReadSIOData( &(receiveBuf[0]), MASTER_RECEIVE_SIZE );

	SetRTSDTR( 0 );										/* remove OK to send */

	cts = ReadCTS();
	while( cts == 1 ) 									/* wait for acknowledge */
	{
/*		pollhost();*/
		cts = ReadCTS();
	}

	ExitCriticalSection();




	long_ptr = (ulong *) &(receiveBuf[0]) ;

	shipInfo[serialShip].ppivot.vx = *long_ptr ++ ;
	shipInfo[serialShip].ppivot.vy = *long_ptr ++ ;
	shipInfo[serialShip].ppivot.vz = *long_ptr ++ ;
	short_ptr = (ushort *)long_ptr ;
	shipInfo[serialShip].hdg = *short_ptr ++ ;
	shipInfo[serialShip].pitch = *short_ptr ++ ;
	shipInfo[serialShip].roll = *short_ptr ++ ;
	shipInfo[serialShip].haveFired = *short_ptr ++ ;
	shipInfo[serialShip].nearSecNo = *short_ptr ++;
	shipInfo[serialShip].prevSecNo = *short_ptr ;

#if 0
	loopTime = GsGetVcount () - loopTime ;	 
	AddNumber( loopTime, text_data( 10, 160, 8 ), WhiteText ); 	
#endif	
#endif	// PCwipeout
  
}


void NewSlave (combatData *packetData, ShipData *shipInfo)
{
#ifdef	PCwipeout
	ulong 	*long_ptr ;
	ushort 	*short_ptr ;
	short		i ;
	u_long	cts ;
	long 		transferTime, loopTime ;


#if 0
	loopTime = GsGetVcount();
#endif

	/* Read Data */
	cts = ReadDSR();
	while( cts == 0 )
		cts = ReadDSR();

	EnterCriticalSection();

	SetRTSDTR( SIOCL_DTRON | SIOCL_RTSON );		/* Set OK to send */

	ReadSIOData( &(receiveBuf[0]), DATA_PACKET_SIZE );

	SetRTSDTR( 0 );										/* remove OK to send */

	cts = ReadCTS();
	while( cts == 1 ) 									/* wait for acknowledge */
	{
/*		pollhost();*/
		cts = ReadCTS();
	}

	ExitCriticalSection();







	long_ptr = (ulong *) &(receiveBuf[0]) ;

	for (i=0; i < NO_OF_SHIPS; i++)
	{
		if ( i != ownShip )
		{
												 
			shipInfo[i].ppivot.vx = *long_ptr ++ ;
			shipInfo[i].ppivot.vy = *long_ptr ++ ;
			shipInfo[i].ppivot.vz = *long_ptr ++ ;
			short_ptr = (ushort *)long_ptr ;
			shipInfo[i].hdg = *short_ptr ++ ;
			shipInfo[i].pitch = *short_ptr ++ ;
			shipInfo[i].roll = *short_ptr ++ ;
			shipInfo[i].haveFired = *short_ptr ++ ;
			shipInfo[i].nearSecNo = *short_ptr ++;
			shipInfo[i].prevSecNo = *short_ptr ++ ;
			long_ptr = (ulong *)short_ptr ;
		}
	}


	long_ptr = (ulong *) &(transmitBuf[0]) ;

	*long_ptr++ = shipInfo[ownShip].ppivot.vx ;
	*long_ptr++ = shipInfo[ownShip].ppivot.vy ;
	*long_ptr++ = shipInfo[ownShip].ppivot.vz ;
	short_ptr = (ushort *)long_ptr ;
	*short_ptr++ = shipInfo[ownShip].hdg ;
	*short_ptr++ = shipInfo[ownShip].pitch ;
	*short_ptr++ = shipInfo[ownShip].roll ;
	*short_ptr++ = shipInfo[ownShip].haveFired ;
	*short_ptr++ = shipInfo[ownShip].nearSecNo ;
	*short_ptr = shipInfo[ownShip].prevSecNo ;


	SetRTSDTR( SIOCL_DTRON | SIOCL_RTSON );	/* Signal that data is waiting */

	cts = ReadCTS();
	while( cts == 0 )		 							/* wait for OK to send */
	{
/*		pollhost();*/
		cts = ReadCTS();
	}

	cts = 0;
	while( cts < 200 )		/*100*/
		cts++;

	WriteSIOData( &(transmitBuf[0]), SLAVE_TRANSMIT_SIZE );

	cts = ReadCTS();
	while( cts == 1 ) 								/* wait for acknowledge */
	{
/*		pollhost();*/
		cts = ReadCTS();
	}

	SetRTSDTR( 0 );									/* remove signals */

#if 0
	loopTime = GsGetVcount () - loopTime ;	 
	AddNumber( loopTime, text_data( 10, 120, 8 ), WhiteText ); 	
#endif

#endif	// PCwipeout	
}


#if 0
void WaitForMaster( void )
{
#ifdef PCwipeout
	long	ctl = ReadCTSDSR();
	while( ctl != 2 )
		ctl = ReadCTSDSR();
#endif	// PCwipeout
}


void WaitForSlave( void )
{
#ifdef	PCwipeout
	long	ctl = ReadCTSDSR();
	while( ctl != 1 )
		ctl = ReadCTSDSR();
#endif	// PCwipeout
}

#endif

void MasterSlaveSIO (MusicPrims *musicPrims, SlideTextures *musicTex, SelectionData *selectData, char loadGames[][9], ConfigData *gameData)
{
#ifdef	PCwipeout
	long ctl = ReadCTSDSR();


	if (!selectData->combatComplete)
	{

		CentreText ("INITIALIZING SERIAL LINK UP", 
					text_data (1,116, 8), RedText) ;
	}

	if (neg1.head == 8960)
	{
		DisplayButtons ( 130, 210, NBACK) ;
	}
	else
	{
		DisplayButtons ( 130, 210, PBACK) ;
	}
/*
	ScreenFooter (70, 210, WhiteText) ;
*/
	if (selectData->combatInit > 30 && !selectData->combatComplete)
	{
		switch( ctl )
		{
			case 0:
				/* PSX is booting Up.. become master */
				gameType = MASTER;
				SetRTSDTR( SIOCL_RTSON );
				WaitForSlave();
				selectData->combatComplete = 1 ;
				break;

			case 1:
				/* other PSX seems to think it's slave already */
				gameType = MASTER;
				SetRTSDTR( SIOCL_RTSON );
				WaitForSlave();
				selectData->combatComplete = 1 ;
				break;

			case 2:
				/* other PSX is master */
				gameType = SLAVE;
				SetRTSDTR( SIOCL_DTRON );
				WaitForMaster();
				selectData->combatComplete = 1 ;
				break;

			case 3:
				/* No PSX attached */
				gameType = SINGLE ;
				selectData->combatComplete = 1 ;
				break;
		}

		if (gameType == SLAVE)
			printf ("I am SLAVE\n") ;

		if (gameType == MASTER)
			printf ("I am MASTER\n") ;
	}

	selectData->combatInit ++ ;

	if (selectData->combatComplete > 0 && selectData->combatComplete < 32)
	{
	 	switch (gameType)
		{
			case MASTER:
							CentreText ("SERIAL LINK ESTABLISHED", 
								text_data (1,116, 8), RedText) ;
							CentreText ("YOU ARE PLAYER ONE", 
								text_data (1,150, 8), RedText) ;
		 					break ;
			case SLAVE:
							CentreText ("SERIAL LINK ESTABLISHED", 
								text_data (1,116, 8), RedText) ;
							CentreText ("YOU ARE PLAYER TWO", 
								text_data (1,150, 8), RedText) ;
							break ;
			case SINGLE:
							CentreText ("NO OTHER PLAYSTATION PRESENT", 
								text_data (1,116, 8), RedText) ;
							break ;

		}

		selectData->combatComplete ++ ;
			
	}
#endif	// PCwipeout
}


void static _error()
{
#ifdef PCwipeout
	printf("e(stat:%04x)",_comb_control(0,0,0)&0x3ff);
	_comb_control(2,0,0);  /* reset error and cancel read request */
	readMFlag = 1 ;
	readSFlag = 1 ;
	printf("(stat:%04x)",_comb_control(0,0,0)&0x3ff);
	printf("0\n");
#endif	// PCwipeout
}


void _start_remote()
{
#ifdef PCwipeout
	EnableEvent(ev_e);
#if 0
	EnableEvent(ev_r);
	EnableEvent(ev_w);
#endif
	_comb_control(1,3,BR);
	printf("baud rate = %d\n",_comb_control(0,3,0));
#endif // PCwipeout
}


void _init_cont()
{
#ifdef PCwipeout
	/* open and enable an event to detect the end of read operation */
#if 0
	ev_r = OpenEvent(HwSIO,EvSpIOER,EvMdNOINTR,NULL);

	ev_w = OpenEvent(HwSIO,EvSpIOEW,EvMdNOINTR,NULL);
#endif 
	/* open and enable an event to detect the error */
	ev_e = OpenEvent(HwSIO,EvSpERROR,EvMdINTR,_error);
 
	/* attacth the SIO driver to the kernel */
	AddCOMB();
 
	/* open a write path and set baud-rate */
	fw = open("sio:",O_WRONLY);
 
	/* open a read path and set baud-rate */
	fr = open("sio:",O_RDONLY);

	_start_remote () ;

#endif // PCwipeout
}


void InitializeCombatLink (void)
{
#ifdef	PCwipeout
	SetRTSDTR( 0 )	;		
#endif //	PCwipeout
}





