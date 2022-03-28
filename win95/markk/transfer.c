//-------------------------------------------------------------------

#include 					<libspu.h>
#include 					<libsnd.h>
#include 					<sys\ioctl.h>
#define						LANGUAGE_C						// PCwipeout
#include					<asm.h>							// PCwipeout
#include 					<kernel.h>

#include 					"..\origsrc\standard.h"
#include 					"..\origsrc\malloc.h"
#include 					"..\origsrc\tim.h"
#include 					"..\origsrc\object.h"
#include 					"..\origsrc\skeleton.h"
#include 					"..\origsrc\track.h"
#include 					"..\origsrc\ships.h"
#include 					"..\origsrc\weapon.h"
#include 					"..\origsrc\control.h"
#include 					"..\origsrc\droid.h"
#include 					"..\origsrc\camera.h"
#include 					"..\origsrc\menus.h"
#include 					"..\origsrc\handlers.h"
#include 					"..\origsrc\dynam.h"
#include 					"..\origsrc\sound.h"
#include 					"..\origsrc\sparks2.h"
#include 					"..\origsrc\global.h"

#define						_SIZE_T_DEFINED_
#include					"c:\watcom\h\stdio.h"
#include					"c:\watcom\h\string.h"
#include					"..\markk\comio.h"
#include					"..\markk\comerr.h"
#include					"..\markk\pccombat.h"
#include					"..\markk\commwo.h"
#include					"..\markk\transfer.h"
#include					"..\markk\debug.h"

//-------------------------------------------------------------------

char				write_flag[NO_OF_SHIPS];
char				read_flag[NO_OF_SHIPS];
char				read_size;
char				write_size;
long				first_read;
long				absoluteShip;
long				abs_Order[NO_OF_SHIPS - 1];
long				cycle_count;
long				exit_status_on;
long				delta_frames_on;
char				transmitBuf[DATA_PACKET_SIZE];	/* transmit buffer */
char				receiveBuf[DATA_PACKET_SIZE];	/* receive buffer */
struct _ship_packet	oldshipInfo[NO_OF_SHIPS];
char				write_lookup[16] =	{	0,	//	0000
											2,	//	0002
											2,	//	0020
											4,	//	0022
											2,	//	0200
											4,	//	0202
											4,	//	0220
											6,	//	0222
											2,	//	2000
											4,	//	2002
											4,	//	2020
											6,	//	2022
											4,	//	2200
											6,	//	2202
											6,	//	2220
											8	//	2222
										};

//-------------------------------------------------------------------

#ifdef		TRANSFER_v7
// delta changes after 1st frame
void		NewMaster(combatData *packetData, ShipData *shipInfo)
{
	ShipData	*sbuf = shipInfo;
	long		*lmr;
	short		*smr;
	short		*sms;
	int			i;
	long		*lms;
	signed char	*cms;
	char		*ums;

	fn_track("newmaster v6\n")

	// terminate on master
	if(exit_status_on)
	{
		MAK_CloseLink();
		return;
	}

	// if master quits/retires terminate next frame
	if((shipInfo[ownShip].haveFired == QUIT) || (shipInfo[ownShip].haveFired == RETIRE)) exit_status_on = 1;

	// read absolute slave packet
	if(delta_frames_on)
	{
		ReadSIOData(&(receiveBuf[0]), ABSOLUTE_SLAVE_PACKET);
		smr = (short *) &(receiveBuf[0]);
		(sbuf + serialShip)->haveFired 	= *(smr++);		// command code
		(sbuf + serialShip)->hdg		= *(smr++);
		(sbuf + serialShip)->pitch		= *(smr++);
		(sbuf + serialShip)->roll		= *(smr++);
		(sbuf + serialShip)->nearSecNo	= *(smr++);
		(sbuf + serialShip)->prevSecNo	= *(smr++);
		lmr = (long *) smr;
		(sbuf + serialShip)->ppivot.vx 	= *(lmr++);
		(sbuf + serialShip)->ppivot.vy 	= *(lmr++);
		(sbuf + serialShip)->ppivot.vz 	= *(lmr);
	}

	// if receive quit/retire from slave quit now
	if((shipInfo[serialShip].haveFired == QUIT) || (shipInfo[serialShip].haveFired == RETIRE))
	{
		MAK_CloseLink();
		return;
	}

	// first frame code
	if(!delta_frames_on) {
		delta_frames_on = 1;
		memset((char *) &oldshipInfo[0], 0, sizeof(struct _ship_packet) * NO_OF_SHIPS);
//		cycle_count = 0;
		
		// write absolute data for first frame
		sms = (short *) &transmitBuf[0];
		for(i=0; i<NO_OF_SHIPS; i++)
		{
			if(i != serialShip)
			{
				*(sms++)	= (sbuf + i)->haveFired;		// command code
				*(sms++)	= (sbuf + i)->hdg;
				*(sms++)	= (sbuf + i)->pitch;
				*(sms++)	= (sbuf + i)->roll;
				*(sms++)	= (sbuf + i)->nearSecNo;
				*(sms++)	= (sbuf + i)->prevSecNo;
				lms = (long *) sms;
				*(lms++)	= (sbuf + i)->ppivot.vx;
				*(lms++)	= (sbuf + i)->ppivot.vy;
				*(lms++)	= (sbuf + i)->ppivot.vz;
				sms = (short *) lms;
			}
		}
		WriteSIOData(&(transmitBuf[0]), MASTER_PACKET);
	}
	else	// construct and write delta ship data with change compression
	{
		memset(&write_flag[0], 0xFF, NO_OF_SHIPS);
		write_size = (NO_OF_SHIPS - 1) * (DELTA_SIZE + 1);
		for(i=0; i<NO_OF_SHIPS; i++)
		{
			if(i != serialShip)
			{
				if(oldshipInfo[i].nearSecNo == (sbuf + i)->nearSecNo) write_flag[i] &= 0xFE;
				if(oldshipInfo[i].prevSecNo == (sbuf + i)->prevSecNo) write_flag[i] &= 0xFD;
				if(oldshipInfo[i].vx == (sbuf + i)->ppivot.vx) write_flag[i] &= 0xFB;
				if(oldshipInfo[i].vy == (sbuf + i)->ppivot.vy) write_flag[i] &= 0xF7;
				if(oldshipInfo[i].vz == (sbuf + i)->ppivot.vz) write_flag[i] &= 0xEF;
				if(oldshipInfo[i].roll == (sbuf + i)->roll) write_flag[i] &= 0xDF;
				if(oldshipInfo[i].hdg == (sbuf + i)->hdg) write_flag[i] &= 0xBF;
				if(oldshipInfo[i].pitch == (sbuf + i)->pitch) write_flag[i] &= 0x7F;
				write_size -=	(
									write_lookup[(~write_flag[i]) & 0x0F] +
								 	write_lookup[((~write_flag[i]) & 0x30) >> 4] +
								 	(write_lookup[(~write_flag[i]) >> 6] >> 1)
								);
			}
		}

		transmitBuf[0] = write_size;
		ums = (char *) &transmitBuf[1];
		for(i=0; i<NO_OF_SHIPS; i++)
		{
			if(i != serialShip)
			{
				*(ums++) = (char) write_flag[i];		// flags at head of ship packet
				cms = (signed char *) ums;
				*(cms++) = (signed char) ((sbuf + i)->haveFired	- oldshipInfo[i].haveFired);
				sms = (short *) cms;
				if(write_flag[i] & 0x01) *(sms++) = (sbuf + i)->nearSecNo;
				if(write_flag[i] & 0x02) *(sms++) = (sbuf + i)->prevSecNo;
				if(write_flag[i] & 0x04) *(sms++) = (short) ((sbuf + i)->ppivot.vx - oldshipInfo[i].vx);
				if(write_flag[i] & 0x08) *(sms++) = (short) ((sbuf + i)->ppivot.vy - oldshipInfo[i].vy);
				if(write_flag[i] & 0x10) *(sms++) = (short) ((sbuf + i)->ppivot.vz - oldshipInfo[i].vz);
				if(write_flag[i] & 0x20) *(sms++) = ((sbuf + i)->roll);
				cms = (signed char *) sms;
				if(write_flag[i] & 0x40) *(cms++) = (signed char) ((sbuf + i)->hdg - oldshipInfo[i].hdg);
				if(write_flag[i] & 0x80) *(cms++) = (signed char) ((sbuf + i)->pitch - oldshipInfo[i].pitch);
				ums = (char *) cms;
			}
		}
		WriteSIOData(&(transmitBuf[0]), write_size + 1);
		fn_track("\nwrite_size: ")
		fn_var(write_size)
	}

	// save current ship data to old shipdata area
	for(i=0; i<NO_OF_SHIPS; i++)
	{
		if(i != serialShip)
		{
			oldshipInfo[i].haveFired	= (sbuf + i)->haveFired;
			oldshipInfo[i].hdg			= (sbuf + i)->hdg;
			oldshipInfo[i].pitch		= (sbuf + i)->pitch;
//			oldshipInfo[i].roll			= (sbuf + i)->roll;
			oldshipInfo[i].vx			= (sbuf + i)->ppivot.vx;
			oldshipInfo[i].vy			= (sbuf + i)->ppivot.vy;
			oldshipInfo[i].vz			= (sbuf + i)->ppivot.vz;
//			oldshipInfo[i].nearSecNo	= (sbuf + i)->nearSecNo;
//			oldshipInfo[i].prevSecNo	= (sbuf + i)->prevSecNo;
		}
	}
	
	#ifdef	FRAME_CHECKING
	cycle_count++;
	
	fn_track("\nserialShip: ")
	fn_var(serialShip)
	fn_track("\ncount: ")
	fn_var(cycle_count)
	
	fn_track("ownship: vx: ")
	fn_var((sbuf+ownShip)->ppivot.vx)
	fn_track("ownship: vy: ")
	fn_var((sbuf+ownShip)->ppivot.vy)
	fn_track("ownship: vz: ")
	fn_var((sbuf+ownShip)->ppivot.vz)
	fn_track("ownship: haveFired: ")
	fn_var((sbuf+ownShip)->haveFired)
	fn_track("ownship: hdg: ")
	fn_var((sbuf+ownShip)->hdg)
	fn_track("ownship: pitch: ")
	fn_var((sbuf+ownShip)->pitch)
	fn_track("ownship: roll: ")
	fn_var((sbuf+ownShip)->roll)
	fn_track("ownship: nearSecNo: ")
	fn_var((sbuf+ownShip)->nearSecNo)
	fn_track("ownship: prevSecNo: ")
	fn_var((sbuf+ownShip)->prevSecNo)
	#endif	//FRAME_CHECKING
}

//-------------------------------------------------------------------

void		NewSlave(combatData *packetData, ShipData *shipInfo)
{
	ShipData	*sbuf = shipInfo;
	long		*lss;
	short		*sss;
	short		*ssr;
	int			i;
	long		*lsr;
	signed char	*csr;
	char		*usr;

	fn_track("newslave v6\n")

	// terminate on slave
	if(exit_status_on)
	{
		MAK_CloseLink();
		return;
	}

	// if slave quits/retires send info to master and terminate next frame
	if((shipInfo[ownShip].haveFired == QUIT) || (shipInfo[ownShip].haveFired == RETIRE)) exit_status_on = 1;

	// read delta packets
	if(delta_frames_on)
	{
		if(first_read)
		{
			ReadSIOData(&(receiveBuf[0]), MASTER_PACKET);
			ssr = (short *) &receiveBuf[0];
			for(i=0; i<NO_OF_SHIPS; i++) {
				if(i != ownShip)
				{
					(sbuf + i)->haveFired	= *(ssr++);
					(sbuf + i)->hdg			= *(ssr++);
					(sbuf + i)->pitch		= *(ssr++);
					(sbuf + i)->roll		= *(ssr++);
					(sbuf + i)->nearSecNo	= *(ssr++);
					(sbuf + i)->prevSecNo	= *(ssr++);
					lsr = (long *) ssr;
					(sbuf + i)->ppivot.vx	= *(lsr++);
					(sbuf + i)->ppivot.vy	= *(lsr++);
					(sbuf + i)->ppivot.vz	= *(lsr++);
					ssr = (short *) lsr;
				}
			first_read = 0;
			}
		}
		else {	// read delta packet from master
			ReadSIOData(&read_size, 1);
			ReadSIOData(&(receiveBuf[0]), read_size);
			usr = (char *) &receiveBuf[0];
			for(i=0; i<NO_OF_SHIPS; i++)
			{
				if(i != ownShip)
				{
					read_flag[i] = *(usr++);
					csr = (signed char *) usr;
					(sbuf + i)->haveFired += *(csr++);
					ssr = (short *) csr;
					if(read_flag[i] & 0x01) (sbuf + i)->nearSecNo = *(ssr++);
					if(read_flag[i] & 0x02) (sbuf + i)->prevSecNo = *(ssr++);
					if(read_flag[i] & 0x04) (sbuf + i)->ppivot.vx += *(ssr++);
					if(read_flag[i] & 0x08) (sbuf + i)->ppivot.vy += *(ssr++);
					if(read_flag[i] & 0x10) (sbuf + i)->ppivot.vz += *(ssr++);
					if(read_flag[i] & 0x20) (sbuf + i)->roll = *(ssr++);
					csr = (signed char *) ssr;
					if(read_flag[i] & 0x40) (sbuf + i)->hdg += *(csr++);
					if(read_flag[i] & 0x80) (sbuf + i)->pitch += *(csr++);
//					if(read_flag[i] & (1 << 7)) (sbuf + i)->roll += *(csr++);
					usr = (char *) csr;
				}
			}
		}
	}

	// if receive quit/retire from master quit now
	if((shipInfo[serialShip].haveFired == QUIT) || (shipInfo[serialShip].haveFired == RETIRE))
	{
		MAK_CloseLink();
		return;
	}

	// first frame code
	if(!delta_frames_on)
	{
		delta_frames_on = 1;
		first_read = 1;
		cycle_count = 0;
	}
	
	// write absolute slave packet
	sss = (short *) &(transmitBuf[0]);
	*(sss++)	= (sbuf + ownShip)->haveFired;
	*(sss++)	= (sbuf + ownShip)->hdg;
	*(sss++)	= (sbuf + ownShip)->pitch;
	*(sss++)	= (sbuf + ownShip)->roll;
	*(sss++)	= (sbuf + ownShip)->nearSecNo;
	*(sss++)	= (sbuf + ownShip)->prevSecNo;
	lss = (long *) sss;
	*(lss++)	= (sbuf + ownShip)->ppivot.vx;
	*(lss++)	= (sbuf + ownShip)->ppivot.vy;
	*(lss)		= (sbuf + ownShip)->ppivot.vz;
	WriteSIOData(&(transmitBuf[0]), ABSOLUTE_SLAVE_PACKET);

	#ifdef	FRAME_CHECKING
	fn_track("\nownShip: ")
	fn_var(ownShip)
	cycle_count++;
	
	fn_track("\nserialShip: ")
	fn_var(serialShip)
	fn_track("\ncount: ")
	fn_var(cycle_count)
		
	fn_track("serialShip: vx: ")
	fn_var((sbuf+serialShip)->ppivot.vx)
	fn_track("serialShip: vy: ")
	fn_var((sbuf+serialShip)->ppivot.vy)
	fn_track("serialShip: vz: ")
	fn_var((sbuf+serialShip)->ppivot.vz)
	fn_track("serialShip: haveFired: ")
	fn_var((sbuf+serialShip)->haveFired)
	fn_track("serialShip: hdg: ")
	fn_var((sbuf+serialShip)->hdg)
	fn_track("serialShip: pitch: ")
	fn_var((sbuf+serialShip)->pitch)
	fn_track("serialShip: roll: ")
	fn_var((sbuf+serialShip)->roll)
	fn_track("serialShip: nearSecNo: ")
	fn_var((sbuf+serialShip)->nearSecNo)
	fn_track("serialShip: prevSecNo: ")
	fn_var((sbuf+serialShip)->prevSecNo)
	#endif	//FRAME_CHECKING
}
#endif	//TRANSFER_v7

//-------------------------------------------------------------------

#ifdef	TRANSFER_v6
// delta changes after 1st frame
void		NewMaster(combatData *packetData, ShipData *shipInfo)
{
	ShipData	*sbuf = shipInfo;
	long		*lmr;
	short		*smr;
	short		*sms;
	int			i;
	long		*lms;
	signed char	*cms;

	fn_track("newmaster v6\n")

	// terminate on master
	if(exit_status_on)
	{
		MAK_CloseLink();
		return;
	}

	// if master quits/retires terminate next frame
	if((shipInfo[ownShip].haveFired == QUIT) || (shipInfo[ownShip].haveFired == RETIRE)) exit_status_on = 1;

	// read absolute slave packet
	if(delta_frames_on)
	{
		ReadSIOData(&(receiveBuf[0]), ABSOLUTE_SLAVE_PACKET);
		smr = (short *) &(receiveBuf[0]);
		(sbuf + serialShip)->haveFired 	= *(smr++);		// command code
		(sbuf + serialShip)->hdg		= *(smr++);
		(sbuf + serialShip)->pitch		= *(smr++);
		(sbuf + serialShip)->roll		= *(smr++);
		(sbuf + serialShip)->nearSecNo	= *(smr++);
		(sbuf + serialShip)->prevSecNo	= *(smr++);
		lmr = (long *) smr;
		(sbuf + serialShip)->ppivot.vx 	= *(lmr++);
		(sbuf + serialShip)->ppivot.vy 	= *(lmr++);
		(sbuf + serialShip)->ppivot.vz 	= *(lmr);
	}

	// if receive quit/retire from slave quit now
	if((shipInfo[serialShip].haveFired == QUIT) || (shipInfo[serialShip].haveFired == RETIRE))
	{
		MAK_CloseLink();
		return;
	}

	// first frame code
	if(!delta_frames_on) {
		delta_frames_on = 1;
		memset((char *) &oldshipInfo[0], 0, sizeof(struct _ship_packet) * NO_OF_SHIPS);
		cycle_count = 0;
		
		// write absolute data for first frame
		sms = (short *) &transmitBuf[0];
		for(i=0; i<NO_OF_SHIPS; i++)
		{
			if(i != serialShip)
			{
				*(sms++)	= (sbuf + i)->haveFired;		// command code
				*(sms++)	= (sbuf + i)->hdg;
				*(sms++)	= (sbuf + i)->pitch;
				*(sms++)	= (sbuf + i)->roll;
				*(sms++)	= (sbuf + i)->nearSecNo;
				*(sms++)	= (sbuf + i)->prevSecNo;
				lms = (long *) sms;
				*(lms++)	= (sbuf + i)->ppivot.vx;
				*(lms++)	= (sbuf + i)->ppivot.vy;
				*(lms++)	= (sbuf + i)->ppivot.vz;
				sms = (short *) lms;
			}
		}
		WriteSIOData(&(transmitBuf[0]), MASTER_PACKET);
	}
	else	// construct and write delta ship data
	{
		cms = (signed char *) &transmitBuf[0];
		for(i=0; i<NO_OF_SHIPS; i++)
		{
			if(i != serialShip)
			{
				// trap these vars if uchar >= 128
				*(cms++) = (signed char) ((sbuf + i)->haveFired	- oldshipInfo[i].haveFired);
				*(cms++) = (signed char) ((sbuf + i)->hdg  		- oldshipInfo[i].hdg);
				*(cms++) = (signed char) ((sbuf + i)->pitch		- oldshipInfo[i].pitch);
//				*(cms++) = ((sbuf + i)->position);
				sms = (short *) cms;
				*(sms++) = (short) ((sbuf + i)->roll);
				*(sms++) = (short) ((sbuf + i)->ppivot.vx - oldshipInfo[i].vx);
				*(sms++) = (short) ((sbuf + i)->ppivot.vy - oldshipInfo[i].vy);
				*(sms++) = (short) ((sbuf + i)->ppivot.vz - oldshipInfo[i].vz);
				*(sms++) = (sbuf + i)->nearSecNo;
//				*(sms++) = (sbuf + i)->prevSecNo;
				cms = (signed char *) sms;
			}
		}
		WriteSIOData(&(transmitBuf[0]), DELTA_MASTER_PACKET);
	}

	// save current ship data to old shipdata area
	for(i=0; i<NO_OF_SHIPS; i++)
	{
		if(i != serialShip)
		{
			oldshipInfo[i].haveFired	= (sbuf + i)->haveFired;
			oldshipInfo[i].hdg			= (sbuf + i)->hdg;
			oldshipInfo[i].pitch		= (sbuf + i)->pitch;
			oldshipInfo[i].roll			= (sbuf + i)->roll;
			oldshipInfo[i].vx			= (sbuf + i)->ppivot.vx;
			oldshipInfo[i].vy			= (sbuf + i)->ppivot.vy;
			oldshipInfo[i].vz			= (sbuf + i)->ppivot.vz;
//			oldshipInfo[i].nearSecNo	= (sbuf + i)->nearSecNo;
//			oldshipInfo[i].prevSecNo	= (sbuf + i)->prevSecNo;
		}
	}
	
	#ifdef	FRAME_CHECKING
	cycle_count++;
	
	fn_track("\nserialShip: ")
	fn_var(serialShip)
	fn_track("\ncount: ")
	fn_var(cycle_count)
	
	fn_track("ownship: vx: ")
	fn_var((sbuf+ownShip)->ppivot.vx)
	fn_track("ownship: vy: ")
	fn_var((sbuf+ownShip)->ppivot.vy)
	fn_track("ownship: vz: ")
	fn_var((sbuf+ownShip)->ppivot.vz)
	fn_track("ownship: haveFired: ")
	fn_var((sbuf+ownShip)->haveFired)
	fn_track("ownship: hdg: ")
	fn_var((sbuf+ownShip)->hdg)
	fn_track("ownship: pitch: ")
	fn_var((sbuf+ownShip)->pitch)
	fn_track("ownship: roll: ")
	fn_var((sbuf+ownShip)->roll)
	fn_track("ownship: nearSecNo: ")
	fn_var((sbuf+ownShip)->nearSecNo)
	fn_track("ownship: prevSecNo: ")
	fn_var((sbuf+ownShip)->prevSecNo)
	#endif	//FRAME_CHECKING
}

//-------------------------------------------------------------------

void		NewSlave(combatData *packetData, ShipData *shipInfo)
{
	ShipData	*sbuf = shipInfo;
	long		*lss;
	short		*sss;
	short		*ssr;
	int			i;
	long		*lsr;
	signed char	*csr;

	fn_track("newslave v6\n")

	// terminate on slave
	if(exit_status_on)
	{
		MAK_CloseLink();
		return;
	}

	// if slave quits/retires send info to master and terminate next frame
	if((shipInfo[ownShip].haveFired == QUIT) || (shipInfo[ownShip].haveFired == RETIRE)) exit_status_on = 1;

	// read delta packets
	if(delta_frames_on)
	{
		if(first_read)
		{
			ReadSIOData(&(receiveBuf[0]), MASTER_PACKET);
			ssr = (short *) &receiveBuf[0];
			for(i=0; i<NO_OF_SHIPS; i++) {
				if(i != ownShip)
				{
					(sbuf + i)->haveFired	= *(ssr++);
					(sbuf + i)->hdg			= *(ssr++);
					(sbuf + i)->pitch		= *(ssr++);
					(sbuf + i)->roll		= *(ssr++);
					(sbuf + i)->nearSecNo	= *(ssr++);
					(sbuf + i)->prevSecNo	= *(ssr++);
					lsr = (long *) ssr;
					(sbuf + i)->ppivot.vx	= *(lsr++);
					(sbuf + i)->ppivot.vy	= *(lsr++);
					(sbuf + i)->ppivot.vz	= *(lsr++);
					ssr = (short *) lsr;
				}
			first_read = 0;
			}
		}
		else {	// read delta packet from master
			ReadSIOData(&(receiveBuf[0]), DELTA_MASTER_PACKET);
			csr = (signed char *) &receiveBuf[0];
			for(i=0; i<NO_OF_SHIPS; i++)
			{
				if(i != ownShip)
				{
					(sbuf + i)->haveFired	+= *(csr++);
					(sbuf + i)->hdg  		+= *(csr++);
					(sbuf + i)->pitch		+= *(csr++);
//					(sbuf + i)->position	= *(csr++);
					ssr = (short *) csr;
					(sbuf + i)->roll 		= *(ssr++);
					(sbuf + i)->ppivot.vx	+= *(ssr++);
					(sbuf + i)->ppivot.vy	+= *(ssr++);
					(sbuf + i)->ppivot.vz	+= *(ssr++);
					(sbuf + i)->nearSecNo	= *(ssr++);
//					(sbuf + i)->prevSecNo	= *(ssr++);
					(sbuf + i)->prevSecNo	= (sbuf + i)->nearSecNo;
					csr = (signed char *) ssr;
				}
			}
		}
	}

	// if receive quit/retire from master quit now
	if((shipInfo[serialShip].haveFired == QUIT) || (shipInfo[serialShip].haveFired == RETIRE))
	{
		MAK_CloseLink();
		return;
	}

	// first frame code
	if(!delta_frames_on)
	{
		delta_frames_on = 1;
		first_read = 1;
		cycle_count = 0;
	}
	
	// write absolute slave packet
	sss = (short *) &(transmitBuf[0]);
	*(sss++)	= (sbuf + ownShip)->haveFired;
	*(sss++)	= (sbuf + ownShip)->hdg;
	*(sss++)	= (sbuf + ownShip)->pitch;
	*(sss++)	= (sbuf + ownShip)->roll;
	*(sss++)	= (sbuf + ownShip)->nearSecNo;
	*(sss++)	= (sbuf + ownShip)->prevSecNo;
	lss = (long *) sss;
	*(lss++)	= (sbuf + ownShip)->ppivot.vx;
	*(lss++)	= (sbuf + ownShip)->ppivot.vy;
	*(lss)		= (sbuf + ownShip)->ppivot.vz;
	WriteSIOData(&(transmitBuf[0]), ABSOLUTE_SLAVE_PACKET);

	#ifdef	FRAME_CHECKING
	fn_track("\nownShip: ")
	fn_var(ownShip)
	cycle_count++;
	
	fn_track("\nserialShip: ")
	fn_var(serialShip)
	fn_track("\ncount: ")
	fn_var(cycle_count)
		
	fn_track("serialShip: vx: ")
	fn_var((sbuf+serialShip)->ppivot.vx)
	fn_track("serialShip: vy: ")
	fn_var((sbuf+serialShip)->ppivot.vy)
	fn_track("serialShip: vz: ")
	fn_var((sbuf+serialShip)->ppivot.vz)
	fn_track("serialShip: haveFired: ")
	fn_var((sbuf+serialShip)->haveFired)
	fn_track("serialShip: hdg: ")
	fn_var((sbuf+serialShip)->hdg)
	fn_track("serialShip: pitch: ")
	fn_var((sbuf+serialShip)->pitch)
	fn_track("serialShip: roll: ")
	fn_var((sbuf+serialShip)->roll)
	fn_track("serialShip: nearSecNo: ")
	fn_var((sbuf+serialShip)->nearSecNo)
	fn_track("serialShip: prevSecNo: ")
	fn_var((sbuf+serialShip)->prevSecNo)
	#endif	//FRAME_CHECKING
}
#endif	//TRANSFER_v6

//-------------------------------------------------------------------

#ifdef	TRANSFER_v5
// delta compression and send absolute ship cyclically.
void		NewMaster(combatData *packetData, ShipData *shipInfo)
{
	short		*s;
	char		*c;
	signed char	*sc;
	int			i;

	fn_track("newmaster\n")
	
	// terminate on master
	if(exit_status_on)
	{
		MAK_CloseLink();
		return;
	}

	// if master quits/retires terminate next frame
	if((shipInfo[ownShip].haveFired == QUIT) || (shipInfo[ownShip].haveFired == RETIRE)) exit_status_on = 1;

	// read absolute slave packet
	if(delta_frames_on)
	{
		ReadSIOData(&(receiveBuf[0]), ABSOLUTE_SLAVE_PACKET);
		c = (char *) &(receiveBuf[0]);
		Long_From_3B(c, shipInfo[serialShip].ppivot.vx);
		Long_From_3B(c, shipInfo[serialShip].ppivot.vy);
		Long_From_3B(c, shipInfo[serialShip].ppivot.vz);
		Short_From_B(c, shipInfo[serialShip].haveFired);
		s = (short *) c;
		shipInfo[serialShip].hdg		= *(s++);
		shipInfo[serialShip].pitch		= *(s++);
		shipInfo[serialShip].roll		= *(s++);
		shipInfo[serialShip].nearSecNo	= *(s++);
		shipInfo[serialShip].prevSecNo	= *(s);
	}

	// if receive quit/retire from slave quit now
	if((shipInfo[serialShip].haveFired == QUIT) || (shipInfo[serialShip].haveFired == RETIRE))
	{
		MAK_CloseLink();
		return;
	}

	// first frame code
	if(!delta_frames_on)
	{
		int	next = 0;

		for(i=0; i<NO_OF_SHIPS; i++)
		{
			if(i != serialShip) abs_Order[next++] = i;
		}
		delta_frames_on = 1;
	}

	// write delta packets
	absoluteShip = abs_Order[cycle_count % 7];
	
	c = (char *) &transmitBuf[0];
	Long_2_3B(c, shipInfo[absoluteShip].ppivot.vx);
	Long_2_3B(c, shipInfo[absoluteShip].ppivot.vy);
	Long_2_3B(c, shipInfo[absoluteShip].ppivot.vz);
	Short_2_B(c, shipInfo[absoluteShip].haveFired);
	s		= (short *) c;
	*(s++)	= shipInfo[absoluteShip].hdg;
	*(s++)	= shipInfo[absoluteShip].pitch;
	*(s++)	= shipInfo[absoluteShip].roll;
	*(s++)	= shipInfo[absoluteShip].nearSecNo;
	*(s++)	= shipInfo[absoluteShip].prevSecNo;
	for(i=0; i<NO_OF_SHIPS; i++)
	{
		if((i != serialShip) && (i != absoluteShip))
		{
			*(s++) = (short) (shipInfo[i].ppivot.vx - shipInfo[absoluteShip].ppivot.vx);
			*(s++) = (short) (shipInfo[i].ppivot.vy - shipInfo[absoluteShip].ppivot.vy);
			*(s++) = (short) (shipInfo[i].ppivot.vz - shipInfo[absoluteShip].ppivot.vz);
			c = (char *) s;
			Short_2_B(c, shipInfo[i].haveFired);
			sc = (signed char *) c;
			*(sc++) = (signed char) (shipInfo[i].hdg - shipInfo[absoluteShip].hdg);
			*(sc++) = (signed char) (shipInfo[i].pitch - shipInfo[absoluteShip].pitch);
			*(sc++) = (signed char) (shipInfo[i].roll - shipInfo[absoluteShip].roll);
			s = (short *) sc;
			*(s++) = shipInfo[i].nearSecNo;
			*(s++) = shipInfo[i].prevSecNo;
		}
	}
	WriteSIOData(&(transmitBuf[0]), DELTA_MASTER_PACKET);

	cycle_count++;
}

//-------------------------------------------------------------------

void		NewSlave(combatData *packetData, ShipData *shipInfo)
{
	short 		*s;
	char		*c;
	signed char	*sc;
	int			i;

	fn_track("newslave\n")

	// terminate on slave
	if(exit_status_on)
	{
		MAK_CloseLink();
		return;
	}

	// if slave quits/retires send info to master and terminate next frame
	if((shipInfo[ownShip].haveFired == QUIT) || (shipInfo[ownShip].haveFired == RETIRE)) exit_status_on = 1;

	// read delta packets
	if(delta_frames_on)
	{
		absoluteShip = abs_Order[cycle_count % 7];
	
		ReadSIOData(&(receiveBuf[0]), DELTA_MASTER_PACKET);
		c = (char *) &(receiveBuf[0]);
		Long_From_3B(c, shipInfo[absoluteShip].ppivot.vx);
		Long_From_3B(c, shipInfo[absoluteShip].ppivot.vy);
		Long_From_3B(c, shipInfo[absoluteShip].ppivot.vz);
		Short_From_B(c, shipInfo[absoluteShip].haveFired);
		s = (short *) c;
		shipInfo[absoluteShip].hdg		 = *(s++);
		shipInfo[absoluteShip].pitch	 = *(s++);
		shipInfo[absoluteShip].roll		 = *(s++);
		shipInfo[absoluteShip].nearSecNo = *(s++);
		shipInfo[absoluteShip].prevSecNo = *(s++);
		for(i=0; i<NO_OF_SHIPS; i++)
		{
			if((i != ownShip) && (i != absoluteShip))
			{
				shipInfo[i].ppivot.vx = (long) (shipInfo[absoluteShip].ppivot.vx + *(s++));
				shipInfo[i].ppivot.vy = (long) (shipInfo[absoluteShip].ppivot.vy + *(s++));
				shipInfo[i].ppivot.vz = (long) (shipInfo[absoluteShip].ppivot.vz + *(s++));
				c = (char *) s;
				Short_From_B(c, shipInfo[i].haveFired);
				sc = (signed char *) c;
				shipInfo[i].hdg = (short) (shipInfo[absoluteShip].hdg + *(sc++));
				shipInfo[i].pitch = (short) (shipInfo[absoluteShip].pitch + *(sc++));
				shipInfo[i].roll = (short) (shipInfo[absoluteShip].roll + *(sc++));
				s = (short *) sc;
				shipInfo[i].nearSecNo = *(s++);
				shipInfo[i].prevSecNo = *(s++);
			}
		}
	}

	// if receive quit/retire from master quit now
	if((shipInfo[serialShip].haveFired == QUIT) || (shipInfo[serialShip].haveFired == RETIRE))
	{
		MAK_CloseLink();
		return;
	}

	// first frame code
	if(!delta_frames_on)
	{
		int	next = 0;

		for(i=0; i<NO_OF_SHIPS; i++)
		{
			if(i != ownShip) abs_Order[next++] = i;
		}
		delta_frames_on = 1;
	}
	
	// write absolute slave packet
	c = (char *) &(transmitBuf[0]);
	Long_2_3B(c, shipInfo[ownShip].ppivot.vx);
	Long_2_3B(c, shipInfo[ownShip].ppivot.vy);
	Long_2_3B(c, shipInfo[ownShip].ppivot.vz);
	Short_2_B(c, shipInfo[ownShip].haveFired);
	s		= (short *) c;
	*(s++)	= shipInfo[ownShip].hdg;
	*(s++)	= shipInfo[ownShip].pitch;
	*(s++)	= shipInfo[ownShip].roll;
	*(s++)	= shipInfo[ownShip].nearSecNo;
	*(s)	= shipInfo[ownShip].prevSecNo;
	WriteSIOData(&(transmitBuf[0]), ABSOLUTE_SLAVE_PACKET);

	cycle_count++;
}
#endif	//TRANSFER_v5

//-------------------------------------------------------------------

#ifdef	TRANSFER_v4
// delta compression and send full absolute packets every N frames (N=8).
void		NewMaster(combatData *packetData, ShipData *shipInfo)
{
	short		*s;
	char		*c;
	signed char	*sc;
	int			i;

	fn_track("newmaster\n")
	
	if((absolutecounter++ % 8) == 1) absolute_frame_on = 1;
	else absolute_frame_on = 0;
	
	// terminate on master
	if(exit_status_on) {
		fn_track("exit: initiated by master.\n")
		MAK_CloseLink();
		return;
	}

	// if master quits/retires terminate next frame
	if(	(shipInfo[ownShip].haveFired == QUIT) || (shipInfo[ownShip].haveFired == RETIRE) ) {
		exit_status_on = 1;
	}

	// read data after first frame (always 1 absolute slave ship)
	if(delta_frames_on) {
		// READ 1 SLAVE SHIP
		ReadSIOData(&(receiveBuf[0]), MASTER_RECEIVE_SIZE);
		c = (char *) &(receiveBuf[0]);
		Long_From_3B(c, shipInfo[serialShip].ppivot.vx);
		Long_From_3B(c, shipInfo[serialShip].ppivot.vy);
		Long_From_3B(c, shipInfo[serialShip].ppivot.vz);
		Short_From_B(c, shipInfo[serialShip].haveFired);
		s = (short *) c;
		shipInfo[serialShip].hdg		= *(s++);
		shipInfo[serialShip].pitch		= *(s++);
		shipInfo[serialShip].roll		= *(s++);
		shipInfo[serialShip].nearSecNo	= *(s++);
		shipInfo[serialShip].prevSecNo	= *(s);
	}

	// if receive quit/retire from slave quit now
	if( (shipInfo[serialShip].haveFired == QUIT) || (shipInfo[serialShip].haveFired == RETIRE) ) {
		fn_track("exit: received from slave.\n")
		MAK_CloseLink();
		return;
	}

	// first frame code
	if(!delta_frames_on) {
		delta_frames_on = 1;
		// calculate the absolute ship
		for(i=0; i<NO_OF_SHIPS; i++) {
			if((i != ownShip) && (i != serialShip)) break;
		}
		absoluteShip = i;
//		absoluteShip = ownShip;
	}

	// write data section
	if(absolute_frame_on) {
		// write 7 absolute ships
		c = (char *) &transmitBuf[0];
		for(i=0; i<NO_OF_SHIPS; i++) {
			if(i != serialShip) {
				Long_2_3B(c, shipInfo[i].ppivot.vx);
				Long_2_3B(c, shipInfo[i].ppivot.vy);
				Long_2_3B(c, shipInfo[i].ppivot.vz);
				Short_2_B(c, shipInfo[i].haveFired);
				s		= (short *) c;
				*(s++)	= shipInfo[i].hdg;
				*(s++)	= shipInfo[i].pitch;
				*(s++)	= shipInfo[i].roll;
				*(s++)	= shipInfo[i].nearSecNo;
				*(s++)	= shipInfo[i].prevSecNo;
				c		= (char *) s;
			}
		}
		WriteSIOData(&(transmitBuf[0]), MASTER_TRANSMIT_SIZE);
	}
	else {
		// write 1 absolute ship
		c = (char *) &transmitBuf[0];
		Long_2_3B(c, shipInfo[absoluteShip].ppivot.vx);
		Long_2_3B(c, shipInfo[absoluteShip].ppivot.vy);
		Long_2_3B(c, shipInfo[absoluteShip].ppivot.vz);
		Short_2_B(c, shipInfo[absoluteShip].haveFired);
		s		= (short *) c;
		*(s++)	= shipInfo[absoluteShip].hdg;
		*(s++)	= shipInfo[absoluteShip].pitch;
		*(s++)	= shipInfo[absoluteShip].roll;
		*(s++)	= shipInfo[absoluteShip].nearSecNo;
		*(s++)	= shipInfo[absoluteShip].prevSecNo;
		
		// write 6 delta ships
		for(i=0; i<NO_OF_SHIPS; i++) {
			if((i != serialShip) && (i != absoluteShip)) {
				*(s++) = (short) (shipInfo[i].ppivot.vx - shipInfo[absoluteShip].ppivot.vx);
				*(s++) = (short) (shipInfo[i].ppivot.vy - shipInfo[absoluteShip].ppivot.vy);
				*(s++) = (short) (shipInfo[i].ppivot.vz - shipInfo[absoluteShip].ppivot.vz);
				sc = (signed char *) s;
				Short_2_B(sc, shipInfo[i].haveFired);
				*(sc++) = (signed char) (shipInfo[i].hdg - shipInfo[absoluteShip].hdg);
				*(sc++) = (signed char) (shipInfo[i].pitch - shipInfo[absoluteShip].pitch);
				*(sc++) = (signed char) (shipInfo[i].roll - shipInfo[absoluteShip].roll);
				s = (short *) sc;
				*(s++) = shipInfo[i].nearSecNo;
				*(s++) = shipInfo[i].prevSecNo;
			}
		}
		WriteSIOData(&(transmitBuf[0]), MASTER_TRANSMIT_SIZE_DELTA);
	}
}

//-------------------------------------------------------------------

void		NewSlave(combatData *packetData, ShipData *shipInfo)
{
	short 		*s;
	char		*c;
	signed char	*sc;
	int			i;

	fn_track("newslave\n")

	if((absolutecounter++ % 8) == 1) absolute_frame_on = 1;
	else absolute_frame_on = 0;	
	
	// terminate on slave
	if(exit_status_on) {
		fn_track("exit: initiated by slave.\n")
		MAK_CloseLink();
		return;
	}

	// if slave quits/retires send info to master and terminate next frame
	if(	(shipInfo[ownShip].haveFired == QUIT) || (shipInfo[ownShip].haveFired == RETIRE) ) {
		fn_track("exit status is on\n")
		exit_status_on = 1;
	}

	// read data after first frame
	if(delta_frames_on) {
		if(absolute_frame_on) {
			ReadSIOData(&(receiveBuf[0]), SLAVE_RECEIVE_SIZE);

			// read 7 absolute ships
			c = (char *) &receiveBuf[0];
			for(i=0; i<NO_OF_SHIPS; i++) {
				if(i != ownShip) {
					Long_From_3B(c, shipInfo[absoluteShip].ppivot.vx);
					Long_From_3B(c, shipInfo[absoluteShip].ppivot.vy);
					Long_From_3B(c, shipInfo[absoluteShip].ppivot.vz);
					Short_From_B(c, shipInfo[absoluteShip].haveFired);
					s = (short *) c;
					shipInfo[absoluteShip].hdg			= *(s++);
					shipInfo[absoluteShip].pitch		= *(s++);
					shipInfo[absoluteShip].roll			= *(s++);
					shipInfo[absoluteShip].nearSecNo	= *(s++);
					shipInfo[absoluteShip].prevSecNo	= *(s++);
					c = (char *) s;
				}
			}			
		}
		
		else {
			ReadSIOData(&(receiveBuf[0]), SLAVE_RECEIVE_SIZE_DELTA);
		
			// read 1 absolute ship
			c = (char *) &(receiveBuf[0]);
			Long_From_3B(c, shipInfo[absoluteShip].ppivot.vx);
			Long_From_3B(c, shipInfo[absoluteShip].ppivot.vy);
			Long_From_3B(c, shipInfo[absoluteShip].ppivot.vz);
			Short_From_B(c, shipInfo[absoluteShip].haveFired);
			s = (short *) c;
			shipInfo[absoluteShip].hdg			= *(s++);
			shipInfo[absoluteShip].pitch		= *(s++);
			shipInfo[absoluteShip].roll			= *(s++);
			shipInfo[absoluteShip].nearSecNo	= *(s++);
			shipInfo[absoluteShip].prevSecNo	= *(s++);
	
			// read 6 delta ships
			for(i=0; i<NO_OF_SHIPS; i++) {
				if((i != ownShip) && (i != absoluteShip)) {
					shipInfo[i].ppivot.vx = (long) (shipInfo[absoluteShip].ppivot.vx + *(s++));
					shipInfo[i].ppivot.vy = (long) (shipInfo[absoluteShip].ppivot.vy + *(s++));
					shipInfo[i].ppivot.vz = (long) (shipInfo[absoluteShip].ppivot.vz + *(s++));
					sc = (signed char *) s;
					Short_From_B(sc, shipInfo[i].haveFired);
					shipInfo[i].hdg = (short) (shipInfo[absoluteShip].hdg + *(sc++));
					shipInfo[i].pitch = (short) (shipInfo[absoluteShip].pitch + *(sc++));
					shipInfo[i].roll = (short) (shipInfo[absoluteShip].roll + *(sc++));
					s = (short *) sc;
					shipInfo[i].nearSecNo	= *(s++);
					shipInfo[i].prevSecNo	= *(s++);
				}
			}
		}
	}


	// if receive quit/retire from master quit now
	if( (shipInfo[serialShip].haveFired == QUIT) || (shipInfo[serialShip].haveFired == RETIRE) ) {
		fn_track("exit: receive from master.\n")
		MAK_CloseLink();
		return;
	}

	// first frame code
	if(!delta_frames_on) {
		delta_frames_on = 1;
		// calculate the absolute ship
		for(i=0; i<NO_OF_SHIPS; i++) {
			if((i != ownShip) && (i != serialShip)) break;
		}
		absoluteShip = i;
//		absoluteShip = serialShip;
	}

	// write data every frame (slave ship)
	c = (char *) &(transmitBuf[0]);
	Long_2_3B(c, shipInfo[ownShip].ppivot.vx);
	Long_2_3B(c, shipInfo[ownShip].ppivot.vy);
	Long_2_3B(c, shipInfo[ownShip].ppivot.vz);
	Short_2_B(c, shipInfo[ownShip].haveFired);
	s		= (short *) c;
	*(s++)	= shipInfo[ownShip].hdg;
	*(s++)	= shipInfo[ownShip].pitch;
	*(s++)	= shipInfo[ownShip].roll;
	*(s++)	= shipInfo[ownShip].nearSecNo;
	*(s)	= shipInfo[ownShip].prevSecNo;
	WriteSIOData(&(transmitBuf[0]), SLAVE_TRANSMIT_SIZE);
}
#endif	//TRANSFER_v4

//-------------------------------------------------------------------

#ifdef	TRANSFER_v2
void		NewMaster(combatData *packetData, ShipData *shipInfo)
{
	ShipData	*t = shipInfo;
	ShipData	*r = shipInfo+serialShip;
	ulong		*l;
	ushort		*s;
	int			i;

	l = (ulong *) &transmitBuf[0];
	for(i=0; i<NO_OF_SHIPS; i++, t++) {
		if(i != serialShip) {
			*l++	= t->ppivot.vx;
			*l++	= t->ppivot.vy;
			*l++	= t->ppivot.vz;
			s		= (ushort *) l;
			*s++	= t->hdg;
			*s++	= t->pitch;
			*s++	= t->roll;
			*s++	= t->haveFired;
			*s++	= t->nearSecNo;
			*s++	= t->prevSecNo;
			l		= (ulong *) s;
		}
	}
	WriteSIOData(&(transmitBuf[0]), DATA_PACKET_SIZE);


	ReadSIOData(&(receiveBuf[0]), MASTER_RECEIVE_SIZE);
	l = (ulong *) &(receiveBuf[0]);
	r->ppivot.vx	= *l++;
	r->ppivot.vy	= *l++;
	r->ppivot.vz	= *l++;
	s				= (ushort *) l;
	r->hdg			= *s++;
	r->pitch		= *s++;
	r->roll			= *s++;
	r->haveFired	= *s++;
	r->nearSecNo	= *s++;
	r->prevSecNo	= *s;
}

//-------------------------------------------------------------------

void		NewSlave(combatData *packetData, ShipData *shipInfo)
{
	ShipData	*r = shipInfo;
	ShipData	*t = shipInfo+ownShip;
	ulong 		*l;
	ushort 		*s;
	int			i;

	ReadSIOData(&(receiveBuf[0]), DATA_PACKET_SIZE);
	l = (ulong *) &(receiveBuf[0]);
	for(i=0; i < NO_OF_SHIPS; i++, r++) {
		if(i != ownShip) {
			r->ppivot.vx	= *l++;
			r->ppivot.vy	= *l++;
			r->ppivot.vz	= *l++;
			s				= (ushort *) l;
			r->hdg			= *s++;
			r->pitch		= *s++;
			r->roll			= *s++;
			r->haveFired	= *s++;
			r->nearSecNo	= *s++;
			r->prevSecNo	= *s++;
			l				= (ulong *) s;
		}
	}


	l = (ulong *) &(transmitBuf[0]);
	*l++	= t->ppivot.vx;
	*l++	= t->ppivot.vy;
	*l++	= t->ppivot.vz;
	s		= (ushort *) l;
	*s++	= t->hdg;
	*s++	= t->pitch;
	*s++	= t->roll;
	*s++	= t->haveFired;
	*s++	= t->nearSecNo;
	*s		= t->prevSecNo;
	WriteSIOData(&(transmitBuf[0]), SLAVE_TRANSMIT_SIZE);
}
#endif	//TRANSFER_v2

//-------------------------------------------------------------------

#ifdef	TRANSFER_v1
void		NewMaster(combatData *packetData, ShipData *shipInfo)
{
	ShipData	*t = shipInfo;
	ShipData	*r = shipInfo+serialShip;
	ulong		*long_ptr;
	ushort		*short_ptr;
	int			i;

	long_ptr = (ulong *) &(transmitBuf[0]);
	for(i=0; i < NO_OF_SHIPS; i++, t++) {
		if(i != serialShip) {
			*long_ptr++		= t->ppivot.vx;
			*long_ptr++		= t->ppivot.vy;
			*long_ptr++		= t->ppivot.vz;
			short_ptr		= (ushort *) long_ptr;
			*short_ptr++	= t->hdg;
			*short_ptr++	= t->pitch;
			*short_ptr++	= t->roll;
			*short_ptr++	= t->haveFired;
			*short_ptr++	= t->nearSecNo;
			*short_ptr++	= t->prevSecNo;
			long_ptr		= (ulong *) short_ptr;
		}
	}
	WriteSIOData(&(transmitBuf[0]), DATA_PACKET_SIZE);


	ReadSIOData(&(receiveBuf[0]), MASTER_RECEIVE_SIZE);
	long_ptr		= (ulong *) &(receiveBuf[0]);
	r->ppivot.vx	= *long_ptr++;
	r->ppivot.vy	= *long_ptr++;
	r->ppivot.vz	= *long_ptr++;
	short_ptr		= (ushort *) long_ptr;
	r->hdg			= *short_ptr++;
	r->pitch		= *short_ptr++;
	r->roll			= *short_ptr++;
	r->haveFired	= *short_ptr++;
	r->nearSecNo	= *short_ptr++;
	r->prevSecNo	= *short_ptr;
}

//-------------------------------------------------------------------

void		NewSlave(combatData *packetData, ShipData *shipInfo)
{
	ShipData	*r = shipInfo;
	ShipData	*t = shipInfo+ownShip;
	ulong 		*long_ptr;
	ushort 		*short_ptr;
	int			i;

	ReadSIOData(&(receiveBuf[0]), DATA_PACKET_SIZE);
	long_ptr = (ulong *) &(receiveBuf[0]);
	for(i=0; i < NO_OF_SHIPS; i++, r++) {
		if(i != ownShip) {
			r->ppivot.vx	= *long_ptr++;
			r->ppivot.vy	= *long_ptr++;
			r->ppivot.vz	= *long_ptr++;
			short_ptr		= (ushort *) long_ptr;
			r->hdg			= *short_ptr++;
			r->pitch		= *short_ptr++;
			r->roll			= *short_ptr++;
			r->haveFired	= *short_ptr++;
			r->nearSecNo	= *short_ptr++;
			r->prevSecNo	= *short_ptr++;
			long_ptr		= (ulong *) short_ptr;
		}
	}


	long_ptr		= (ulong *) &(transmitBuf[0]);
	*long_ptr++		= t->ppivot.vx;
	*long_ptr++		= t->ppivot.vy;
	*long_ptr++		= t->ppivot.vz;
	short_ptr		= (ushort *) long_ptr;
	*short_ptr++	= t->hdg;
	*short_ptr++	= t->pitch;
	*short_ptr++	= t->roll;
	*short_ptr++	= t->haveFired;
	*short_ptr++	= t->nearSecNo;
	*short_ptr		= t->prevSecNo;
	WriteSIOData(&(transmitBuf[0]), SLAVE_TRANSMIT_SIZE);
}
#endif	//TRANSFER_v1

//-------------------------------------------------------------------

// currently using this
#ifdef	NEW_TRANSFER
// STU'S
void		NewMaster(combatData *packetData, ShipData *shipInfo)
{
	int			i;
	ulong		*l;
	ushort		*s;
	
	fn_track("newmaster\n")
	
//	c = (uchar *) &transmitBuf[0];

	// setup transmit data (MASTER WRITES ALL SHIPS EXCEPT SLAVE SHIP)
	l = (ulong *) &transmitBuf[0];
	for(i=0; i<NO_OF_SHIPS; i++) {
		if(i != serialShip) {
			*(l++)	= shipInfo[i].ppivot.vx;
//			fn_var(shipInfo[i].ppivot.vx)
			*(l++)	= shipInfo[i].ppivot.vy;
//			fn_var(shipInfo[i].ppivot.vy)
			*(l++)	= shipInfo[i].ppivot.vz;
//			fn_var(shipInfo[i].ppivot.vz)

//			Long_2_3B(c, shipInfo[i].ppivot.vx)
//			Long_2_3B(c, shipInfo[i].ppivot.vy)
//			Long_2_3B(c, shipInfo[i].ppivot.vz)
//			Short_2_B(c, shipInfo[i].haveFired)
//			s		= (ushort *) c;

			s		= (ushort *) l;
			*(s++)	= shipInfo[i].haveFired;
			*(s++)	= shipInfo[i].hdg;
			*(s++)	= shipInfo[i].pitch;
			*(s++)	= shipInfo[i].roll;
			*(s++)	= shipInfo[i].nearSecNo;
			*(s++)	= shipInfo[i].prevSecNo;
//			c		= (uchar *) s;
			l		= (ulong *) s;
		}
	}
	WriteSIOData(&transmitBuf[0], MASTER_TRANSMIT_SIZE);
	

	ReadSIOData(&receiveBuf[0], MASTER_RECEIVE_SIZE);

	l = (ulong *) &receiveBuf[0];
	shipInfo[serialShip].ppivot.vx	= *(l++);
	shipInfo[serialShip].ppivot.vy	= *(l++);
	shipInfo[serialShip].ppivot.vz	= *(l++);

//	c = (uchar *) &receiveBuf[0];
//	Long_From_3B(c, shipInfo[serialShip].ppivot.vx)
//	Long_From_3B(c, shipInfo[serialShip].ppivot.vy)
//	Long_From_3B(c, shipInfo[serialShip].ppivot.vz)
//	Short_From_B(c, shipInfo[serialShip].haveFired)
//	s = (ushort *) c;

	s = (ushort *) l;
	shipInfo[serialShip].haveFired	= *(s++);
	shipInfo[serialShip].hdg		= *(s++);
	shipInfo[serialShip].pitch		= *(s++);
	shipInfo[serialShip].roll		= *(s++);
	shipInfo[serialShip].nearSecNo	= *(s++);
	shipInfo[serialShip].prevSecNo	= *s;
}

//-------------------------------------------------------------------

void		NewSlave(combatData *packetData, ShipData *shipInfo)
{
	int			i;
	ulong		*l;
	ushort		*s;
		
	fn_track("newslave\n")
	
	ReadSIOData(&receiveBuf[0], SLAVE_RECEIVE_SIZE);
//	c = (uchar *) &receiveBuf[0];
	l = (ulong *) &receiveBuf[0];
	for(i=0; i<NO_OF_SHIPS; i++) {
		if(i != ownShip) {
			shipInfo[i].ppivot.vx	= *(l++);
			shipInfo[i].ppivot.vy	= *(l++);
			shipInfo[i].ppivot.vz	= *(l++);
//			Long_From_3B(c, shipInfo[i].ppivot.vx)
//			Long_From_3B(c, shipInfo[i].ppivot.vy)
//			Long_From_3B(c, shipInfo[i].ppivot.vz)
//			Short_From_B(c, shipInfo[i].haveFired)
//			s = (ushort *) c;
			s = (ushort *) l;
			shipInfo[i].haveFired	= *(s++);
			shipInfo[i].hdg 		= *(s++);
			shipInfo[i].pitch		= *(s++);
			shipInfo[i].roll		= *(s++);
			shipInfo[i].nearSecNo	= *(s++);
			shipInfo[i].prevSecNo	= *(s++);
//			c = (uchar *) s;
			l = (ulong *) s;
		}
	}


	l = (ulong *) &transmitBuf[0];
	*(l++) = shipInfo[ownShip].ppivot.vx;
	*(l++) = shipInfo[ownShip].ppivot.vy;
	*(l++) = shipInfo[ownShip].ppivot.vz;
//	c = (uchar *) &transmitBuf[0];
//	Long_2_3B(c, shipInfo[ownShip].ppivot.vx)
//	Long_2_3B(c, shipInfo[ownShip].ppivot.vy)
//	Long_2_3B(c, shipInfo[ownShip].ppivot.vz)
//	Short_2_B(c, shipInfo[ownShip].haveFired)
//	s = (ushort *) c;
	s = (ushort *) l;
	*(s++) = shipInfo[ownShip].haveFired;
	*(s++) = shipInfo[ownShip].hdg;
	*(s++) = shipInfo[ownShip].pitch;
	*(s++) = shipInfo[ownShip].roll;
	*(s++) = shipInfo[ownShip].nearSecNo;
	*s = shipInfo[ownShip].prevSecNo;
	WriteSIOData(&transmitBuf[0], SLAVE_TRANSMIT_SIZE);
}
#endif	// NEW_TRANSFER

//-------------------------------------------------------------------

#ifdef	STU_TRANSFER
void	NewMaster(combatData *packetData, ShipData *shipInfo)
{
	int			i;
	ulong		*l;
	ushort		*s;
	uchar		*c;
	
	fn_track("newmaster\n")
	
	// setup transmit data (MASTER WRITES ALL SHIPS EXCEPT SLAVE SHIP)
	l = (ulong *) &transmitBuf[0];
	for(i=0; i<NO_OF_SHIPS; i++) {
		if(i != serialShip) {
			*(l++)	= shipInfo[i].ppivot.vx;
			*(l++)	= shipInfo[i].ppivot.vy;
			*(l++)	= shipInfo[i].ppivot.vz;
			s		= (ushort *) l;
			*(s++)	= shipInfo[i].hdg;
			*(s++)	= shipInfo[i].pitch;
			*(s++)	= shipInfo[i].roll;
			*(s++)	= shipInfo[i].haveFired;
			*(s++)	= shipInfo[i].nearSecNo;
			*(s++)	= shipInfo[i].prevSecNo;
			l		= (ulong *) s;
		}
	}
	WriteSIOData(&transmitBuf[0], MASTER_TRANSMIT_SIZE);

	ReadSIOData(&receiveBuf[0], MASTER_RECEIVE_SIZE);
	l = (ulong *) &receiveBuf[0];
	shipInfo[serialShip].ppivot.vx	= *(l++);
	shipInfo[serialShip].ppivot.vy	= *(l++);
	shipInfo[serialShip].ppivot.vz	= *(l++);
	s = (ushort *) l;
	shipInfo[serialShip].hdg		= *(s++);
	shipInfo[serialShip].pitch		= *(s++);
	shipInfo[serialShip].roll		= *(s++);
	shipInfo[serialShip].haveFired	= *(s++);
	shipInfo[serialShip].nearSecNo	= *(s++);
	shipInfo[serialShip].prevSecNo	= *s;
}

//-------------------------------------------------------------------

void	NewSlave(combatData *packetData, ShipData *shipInfo)
{
	int			i;
	ulong		*l;
	ushort		*s;
	
	fn_track("newslave\n")
		
	ReadSIOData(&receiveBuf[0], SLAVE_RECEIVE_SIZE);
	l = (ulong *) &receiveBuf[0];
	for(i=0; i<NO_OF_SHIPS; i++) {
		if(i != ownShip) {
			shipInfo[i].ppivot.vx	= *(l++);
			shipInfo[i].ppivot.vy	= *(l++);
			shipInfo[i].ppivot.vz	= *(l++);
			s = (ushort *) l;
			shipInfo[i].hdg 		= *(s++);
			shipInfo[i].pitch		= *(s++);
			shipInfo[i].roll		= *(s++);
			shipInfo[i].haveFired	= *(s++);
			shipInfo[i].nearSecNo	= *(s++);
			shipInfo[i].prevSecNo	= *(s++);
			l = (ulong *) s;
		}
	}

	l = (ulong *) &transmitBuf[0];
	*(l++) = shipInfo[ownShip].ppivot.vx;
	*(l++) = shipInfo[ownShip].ppivot.vy;
	*(l++) = shipInfo[ownShip].ppivot.vz;
	s = (ushort *) l;
	*(s++) = shipInfo[ownShip].hdg;
	*(s++) = shipInfo[ownShip].pitch;
	*(s++) = shipInfo[ownShip].roll;
	*(s++) = shipInfo[ownShip].haveFired;
	*(s++) = shipInfo[ownShip].nearSecNo;
	*s = shipInfo[ownShip].prevSecNo;
	WriteSIOData(&transmitBuf[0], SLAVE_TRANSMIT_SIZE);

}
#endif	// STU_TRANSFER

//-------------------------------------------------------------------

#ifdef	MARK_OLDTRANSFER
void		NewMaster(combatData *packetData, ShipData *shipInfo)
{
	int					i;
	shipPacket			*t = (shipPacket *) &transmitBuf;
	shipPacket			*r = (shipPacket *) &receiveBuf;
	
	fn_track("newmaster\n")

	// setup transmit data (MASTER WRITES ALL SHIPS EXCEPT SLAVE SHIP)
	for (i=0; i<NO_OF_SHIPS; i++, t++) {
		if(i != serialShip) {
			t->vx			= shipInfo[i].ppivot.vx;
			t->vy			= shipInfo[i].ppivot.vy;
			t->vz			= shipInfo[i].ppivot.vz;
			t->hdg			= shipInfo[i].hdg;
			t->pitch		= shipInfo[i].pitch;
			t->roll			= shipInfo[i].roll;
			t->haveFired	= shipInfo[i].haveFired;
			t->nearSecNo	= shipInfo[i].nearSecNo;
			t->prevSecNo	= shipInfo[i].prevSecNo;
		}
	}

	if(CommFirstRead) {
		WriteSIOData((char *) transmitBuf, 24*7);
		CommFirstRead = 0;
	}
	ReadSIOData((char *) receiveBuf, 24);
	WriteSIOData((char *) transmitBuf, 24*7);

	// setup receive data (MASTER RECEIVES 1 SERIAL SHIP)
	// receive ABSOLUTE serial ship
	shipInfo[serialShip].ppivot.vx	= r->vx;
	shipInfo[serialShip].ppivot.vy	= r->vy;
	shipInfo[serialShip].ppivot.vz	= r->vz;
	shipInfo[serialShip].hdg		= r->hdg;
	shipInfo[serialShip].pitch		= r->pitch;
	shipInfo[serialShip].roll		= r->roll;
	shipInfo[serialShip].haveFired	= r->haveFired;
	shipInfo[serialShip].nearSecNo	= r->nearSecNo;
	shipInfo[serialShip].prevSecNo	= r->prevSecNo;
}

//-------------------------------------------------------------------

void		MarkNewSlave(combatData *packetData, ShipData *shipInfo)
{
	int					i;
	shipPacket			*t = (shipPacket *) &transmitBuf;
	shipPacket			*r = (shipPacket *) &receiveBuf;
	
	fn_track("newslave\n")

	t->vx			= shipInfo[ownShip].ppivot.vx;
	t->vy			= shipInfo[ownShip].ppivot.vy;
	t->vz			= shipInfo[ownShip].ppivot.vz;
	t->hdg			= shipInfo[ownShip].hdg;
	t->pitch		= shipInfo[ownShip].pitch;
	t->roll			= shipInfo[ownShip].roll;
	t->haveFired	= shipInfo[ownShip].haveFired;
	t->nearSecNo	= shipInfo[ownShip].nearSecNo;
	t->prevSecNo	= shipInfo[ownShip].prevSecNo;
	
	if(CommFirstRead) {
		WriteSIOData((char *) transmitBuf, 24);
		CommFirstRead = 0;	
	}
	ReadSIOData((char *) receiveBuf, 24*7);
	WriteSIOData((char *) transmitBuf, 24);

	// receive RELATIVE ships
	for(i=0; i<NO_OF_SHIPS; i++, r++) {
		if(i != ownShip) {
			shipInfo[i].ppivot.vx	= r->vx;
			shipInfo[i].ppivot.vy	= r->vy;
			shipInfo[i].ppivot.vz	= r->vz;
			shipInfo[i].hdg			= r->hdg;
			shipInfo[i].pitch		= r->pitch;
			shipInfo[i].roll		= r->roll;
			shipInfo[i].haveFired	= r->haveFired;
			shipInfo[i].nearSecNo	= r->nearSecNo;
			shipInfo[i].prevSecNo	= r->prevSecNo;
		}
	}
}
#endif	//MARK_OLDTRANSFER

//-------------------------------------------------------------------

// currently using this
#ifdef	STU_MASTERSLAVE
// STU'S
void		NewMaster(combatData *packetData, ShipData *shipInfo)
{
	int			i;
	ulong		*l;
	ushort		*s;
	
	fn_track("newmaster\n")
	
//	c = (uchar *) &transmitBuf[0];

	// setup transmit data (MASTER WRITES ALL SHIPS EXCEPT SLAVE SHIP)
	l = (ulong *) &transmitBuf[0];
	for(i=0; i<NO_OF_SHIPS; i++) {
		if(i != serialShip) {
			*(l++)	= shipInfo[i].ppivot.vx;
//			fn_var(shipInfo[i].ppivot.vx)
			*(l++)	= shipInfo[i].ppivot.vy;
//			fn_var(shipInfo[i].ppivot.vy)
			*(l++)	= shipInfo[i].ppivot.vz;
//			fn_var(shipInfo[i].ppivot.vz)

//			Long_2_3B(c, shipInfo[i].ppivot.vx)
//			Long_2_3B(c, shipInfo[i].ppivot.vy)
//			Long_2_3B(c, shipInfo[i].ppivot.vz)
//			Short_2_B(c, shipInfo[i].haveFired)
//			s		= (ushort *) c;

			s		= (ushort *) l;
			*(s++)	= shipInfo[i].haveFired;
			*(s++)	= shipInfo[i].hdg;
			*(s++)	= shipInfo[i].pitch;
			*(s++)	= shipInfo[i].roll;
			*(s++)	= shipInfo[i].nearSecNo;
			*(s++)	= shipInfo[i].prevSecNo;
//			c		= (uchar *) s;
			l		= (ulong *) s;
		}
	}
	WriteSIOData(&transmitBuf[0], MASTER_TRANSMIT_SIZE);
	

	ReadSIOData(&receiveBuf[0], MASTER_RECEIVE_SIZE);

	l = (ulong *) &receiveBuf[0];
	shipInfo[serialShip].ppivot.vx	= *(l++);
	shipInfo[serialShip].ppivot.vy	= *(l++);
	shipInfo[serialShip].ppivot.vz	= *(l++);

//	c = (uchar *) &receiveBuf[0];
//	Long_From_3B(c, shipInfo[serialShip].ppivot.vx)
//	Long_From_3B(c, shipInfo[serialShip].ppivot.vy)
//	Long_From_3B(c, shipInfo[serialShip].ppivot.vz)
//	Short_From_B(c, shipInfo[serialShip].haveFired)
//	s = (ushort *) c;

	s = (ushort *) l;
	shipInfo[serialShip].haveFired	= *(s++);
	shipInfo[serialShip].hdg		= *(s++);
	shipInfo[serialShip].pitch		= *(s++);
	shipInfo[serialShip].roll		= *(s++);
	shipInfo[serialShip].nearSecNo	= *(s++);
	shipInfo[serialShip].prevSecNo	= *s;
	
//	while(1){};
}

//-------------------------------------------------------------------

// STU'S
void		NewSlave(combatData *packetData, ShipData *shipInfo)
{
	int			i;
	ulong		*l;
	ushort		*s;
		
	fn_track("newslave\n")
	
	ReadSIOData(&receiveBuf[0], SLAVE_RECEIVE_SIZE);
//	c = (uchar *) &receiveBuf[0];
	l = (ulong *) &receiveBuf[0];
	for(i=0; i<NO_OF_SHIPS; i++) {
		if(i != ownShip) {
			shipInfo[i].ppivot.vx	= *(l++);
			shipInfo[i].ppivot.vy	= *(l++);
			shipInfo[i].ppivot.vz	= *(l++);
//			Long_From_3B(c, shipInfo[i].ppivot.vx)
//			Long_From_3B(c, shipInfo[i].ppivot.vy)
//			Long_From_3B(c, shipInfo[i].ppivot.vz)
//			Short_From_B(c, shipInfo[i].haveFired)
//			s = (ushort *) c;
			s = (ushort *) l;
			shipInfo[i].haveFired	= *(s++);
			shipInfo[i].hdg 		= *(s++);
			shipInfo[i].pitch		= *(s++);
			shipInfo[i].roll		= *(s++);
			shipInfo[i].nearSecNo	= *(s++);
			shipInfo[i].prevSecNo	= *(s++);
//			c = (uchar *) s;
			l = (ulong *) s;
		}
	}


	l = (ulong *) &transmitBuf[0];
	*(l++) = shipInfo[ownShip].ppivot.vx;
	*(l++) = shipInfo[ownShip].ppivot.vy;
	*(l++) = shipInfo[ownShip].ppivot.vz;
//	c = (uchar *) &transmitBuf[0];
//	Long_2_3B(c, shipInfo[ownShip].ppivot.vx)
//	Long_2_3B(c, shipInfo[ownShip].ppivot.vy)
//	Long_2_3B(c, shipInfo[ownShip].ppivot.vz)
//	Short_2_B(c, shipInfo[ownShip].haveFired)
//	s = (ushort *) c;
	s = (ushort *) l;
	*(s++) = shipInfo[ownShip].haveFired;
	*(s++) = shipInfo[ownShip].hdg;
	*(s++) = shipInfo[ownShip].pitch;
	*(s++) = shipInfo[ownShip].roll;
	*(s++) = shipInfo[ownShip].nearSecNo;
	*s = shipInfo[ownShip].prevSecNo;
	WriteSIOData(&transmitBuf[0], SLAVE_TRANSMIT_SIZE);

//	while(1){};
}
#endif	// STU_MASTERSLAVE

//-------------------------------------------------------------------

#ifdef	ORIGINAL_TRANSFER
void		NewMaster(combatData *packetData, ShipData *shipInfo)
{
	ulong		*long_ptr;
	ushort		*short_ptr;
	short		i;

	long_ptr = (ulong *) &(transmitBuf[0]);
	for(i=0; i < NO_OF_SHIPS; i++) {
		if(i != serialShip) {
			*long_ptr++ = shipInfo[i].ppivot.vx;
			*long_ptr++ = shipInfo[i].ppivot.vy;
			*long_ptr++ = shipInfo[i].ppivot.vz;
			short_ptr = (ushort *)long_ptr;
			*short_ptr++ = shipInfo[i].hdg;
			*short_ptr++ = shipInfo[i].pitch;
			*short_ptr++ = shipInfo[i].roll;
			*short_ptr++ = shipInfo[i].haveFired;
			*short_ptr++ = shipInfo[i].nearSecNo;
			*short_ptr++ =  shipInfo[i].prevSecNo;
			long_ptr = (ulong *) short_ptr;
		}
	}
	WriteSIOData(&(transmitBuf[0]), DATA_PACKET_SIZE);


	ReadSIOData(&(receiveBuf[0]), MASTER_RECEIVE_SIZE);
	long_ptr = (ulong *) &(receiveBuf[0]);
	shipInfo[serialShip].ppivot.vx = *long_ptr++;
	shipInfo[serialShip].ppivot.vy = *long_ptr++;
	shipInfo[serialShip].ppivot.vz = *long_ptr++;
	short_ptr = (ushort *)long_ptr;
	shipInfo[serialShip].hdg = *short_ptr++;
	shipInfo[serialShip].pitch = *short_ptr++;
	shipInfo[serialShip].roll = *short_ptr++;
	shipInfo[serialShip].haveFired = *short_ptr++;
	shipInfo[serialShip].nearSecNo = *short_ptr++;
	shipInfo[serialShip].prevSecNo = *short_ptr;
}

//-------------------------------------------------------------------

void		NewSlave(combatData *packetData, ShipData *shipInfo)
{
	ulong 		*long_ptr;
	ushort 		*short_ptr;
	short		i;

	ReadSIOData(&(receiveBuf[0]), DATA_PACKET_SIZE);
	long_ptr = (ulong *) &(receiveBuf[0]);
	for(i=0; i < NO_OF_SHIPS; i++) {
		if(i != ownShip) {
			shipInfo[i].ppivot.vx = *long_ptr++;
			shipInfo[i].ppivot.vy = *long_ptr++;
			shipInfo[i].ppivot.vz = *long_ptr++;
			short_ptr = (ushort *)long_ptr;
			shipInfo[i].hdg = *short_ptr++;
			shipInfo[i].pitch = *short_ptr++;
			shipInfo[i].roll = *short_ptr++;
			shipInfo[i].haveFired = *short_ptr++;
			shipInfo[i].nearSecNo = *short_ptr++;
			shipInfo[i].prevSecNo = *short_ptr++;
			long_ptr = (ulong *)short_ptr;
		}
	}


	long_ptr = (ulong *) &(transmitBuf[0]);
	*long_ptr++ = shipInfo[ownShip].ppivot.vx;
	*long_ptr++ = shipInfo[ownShip].ppivot.vy;
	*long_ptr++ = shipInfo[ownShip].ppivot.vz;
	short_ptr = (ushort *)long_ptr;
	*short_ptr++ = shipInfo[ownShip].hdg;
	*short_ptr++ = shipInfo[ownShip].pitch;
	*short_ptr++ = shipInfo[ownShip].roll;
	*short_ptr++ = shipInfo[ownShip].haveFired;
	*short_ptr++ = shipInfo[ownShip].nearSecNo;
	*short_ptr = shipInfo[ownShip].prevSecNo;
	WriteSIOData(&(transmitBuf[0]), SLAVE_TRANSMIT_SIZE);
}
#endif	//ORIGINAL_TRANSFER

//-------------------------------------------------------------------

#ifdef	TRANSFER_v1a
void		NewMaster(combatData *packetData, ShipData *shipInfo)
{
	ShipData	*t = shipInfo;
	ShipData	*r = shipInfo+serialShip;
	ulong		*l;
	ushort		*s;
	int			i;

	l = (ulong *) &transmitBuf[0];
	for(i=0; i<NO_OF_SHIPS; i++, t++) {
		if(i != serialShip) {
			*l++	= t->ppivot.vx;
			*l++	= t->ppivot.vy;
			*l++	= t->ppivot.vz;
			s		= (ushort *) l;
			*s++	= t->hdg;
			*s++	= t->pitch;
			*s++	= t->roll;
			*s++	= t->haveFired;
			*s++	= t->nearSecNo;
			*s++	= t->prevSecNo;
			l		= (ulong *) s;
		}
	}
	WriteSIOData(&(transmitBuf[0]), DATA_PACKET_SIZE);


	ReadSIOData(&(receiveBuf[0]), MASTER_RECEIVE_SIZE);
	l = (ulong *) &(receiveBuf[0]);
	r->ppivot.vx	= *l++;
	r->ppivot.vy	= *l++;
	r->ppivot.vz	= *l++;
	s				= (ushort *) l;
	r->hdg			= *s++;
	r->pitch		= *s++;
	r->roll			= *s++;
	r->haveFired	= *s++;
	r->nearSecNo	= *s++;
	r->prevSecNo	= *s;
	
	if(delta_frames_on) {
		// determine the absolute ship
		for(i=0; i<NO_OF_SHIPS; i++) {
			if((i != serialShip) && (i != ownShip)) break;
		}
		absoluteShip = i;

		// display header
		printf("This machine is the MASTER.\n");
		printf("absoluteShip: %d\n", absoluteShip);
		printf("ownShip: %d\n", ownShip);
		printf("serialShip: %d\n", serialShip);
		
		// display slave ship data (after delta de-compression from master)
		printf("Slave Ship Data AFTER transfer.\n");
		printf("ppivot.vx: %lx\n", shipInfo[serialShip].ppivot.vx);
		printf("ppivot.vx: %lx\n", shipInfo[serialShip].ppivot.vy);
		printf("ppivot.vx: %lx\n", shipInfo[serialShip].ppivot.vz);

		// second frame delta compression testing
		while(1){;};	// lock
	}
	else delta_frames_on = 1;
}

//-------------------------------------------------------------------

void		NewSlave(combatData *packetData, ShipData *shipInfo)
{
	ShipData	*r = shipInfo;
	ShipData	*t = shipInfo+ownShip;
	ulong 		*l;
	ushort 		*s;
	int			i;

	ReadSIOData(&(receiveBuf[0]), DATA_PACKET_SIZE);
	l = (ulong *) &(receiveBuf[0]);
	for(i=0; i < NO_OF_SHIPS; i++, r++) {
		if(i != ownShip) {
			r->ppivot.vx	= *l++;
			r->ppivot.vy	= *l++;
			r->ppivot.vz	= *l++;
			s				= (ushort *) l;
			r->hdg			= *s++;
			r->pitch		= *s++;
			r->roll			= *s++;
			r->haveFired	= *s++;
			r->nearSecNo	= *s++;
			r->prevSecNo	= *s++;
			l				= (ulong *) s;
		}
	}


	l = (ulong *) &(transmitBuf[0]);
	*l++	= t->ppivot.vx;
	*l++	= t->ppivot.vy;
	*l++	= t->ppivot.vz;
	s		= (ushort *) l;
	*s++	= t->hdg;
	*s++	= t->pitch;
	*s++	= t->roll;
	*s++	= t->haveFired;
	*s++	= t->nearSecNo;
	*s		= t->prevSecNo;
	WriteSIOData(&(transmitBuf[0]), SLAVE_TRANSMIT_SIZE);

	if(delta_frames_on) {
		// determine the absolute ship
		for(i=0; i<NO_OF_SHIPS; i++) {
			if((i != serialShip) && (i != ownShip)) break;
		}
		absoluteShip = i;

		// display header
		printf("This machine is a SLAVE.\n");
		printf("absoluteShip: %d\n", absoluteShip);
		printf("ownShip: %d\n", ownShip);
		printf("serialShip: %d\n", serialShip);
		
		// display slave ship data (before delta de-compression to master)
		printf("Slave Ship Data BEFORE transfer.\n");
		printf("ppivot.vx: %lx\n", shipInfo[ownShip].ppivot.vx);
		printf("ppivot.vx: %lx\n", shipInfo[ownShip].ppivot.vy);
		printf("ppivot.vx: %lx\n", shipInfo[ownShip].ppivot.vz);

		// second frame delta compression testing
		while(1){;};	// lock
	}
	else delta_frames_on = 1;
}
#endif	//TRANSFER_v1a

//-------------------------------------------------------------------

#if	0	//NEW_DELTA_CODE_FOR_LATER

// MASTER CODE
	else // deltas now on!!
	{
		WriteSIOData(&(transmitBuf[0]), MASTER_TRANSMIT_DELTA_SIZE);

		// setup absolute ship data
		Long_2_3B(c, (t + absoluteShip)->ppivot.vx);
		Long_2_3B(c, (t + absoluteShip)->ppivot.vy);
		Long_2_3B(c, (t + absoluteShip)->ppivot.vz);
		Short_2_B(c, (t + absoluteShip)->haveFired);
		s		= (ushort *) c;
		*s++	= (t + absoluteShip)->hdg;
		*s++	= (t + absoluteShip)->pitch;
		*s++	= (t + absoluteShip)->roll;
		*s++	= (t + absoluteShip)->nearSecNo;
		*s++	= (t + absoluteShip)->prevSecNo;
		c		= (uchar *) s;
		
		// setup relative ship data
		for(i=0; i<NO_OF_SHIPS; i++, t++) {
			if((i != serialShip) && (i != absoluteShip)) {
				Long_2_3B(c, t->ppivot.vx);
				Long_2_3B(c, t->ppivot.vy);
				Long_2_3B(c, t->ppivot.vz);
				Short_2_B(c, t->haveFired);
				s		= (ushort *) c;
//				*s++	= t->hdg;
//				*s++	= t->pitch;
//				*s++	= t->roll;
				*s++	= t->nearSecNo;
				*s++	= t->prevSecNo;
				c		= (uchar *) s;
			}
		}
	}
	
// SLAVE CODE
	else	// delta frames on!!
	{
		ReadSIOData(&(receiveBuf[0]), SLAVE_RECEIVE_DELTA_SIZE);

		// setup absolute ship data
		Long_From_3B(c, r->ppivot.vx);
		Long_From_3B(c, r->ppivot.vy);
		Long_From_3B(c, r->ppivot.vz);
		Short_From_B(c, r->haveFired);
		s				= (ushort *) c;
		r->hdg			= *s++;
		r->pitch		= *s++;
		r->roll			= *s++;
		r->nearSecNo	= *s++;
		r->prevSecNo	= *s++;
		c				= (uchar *) s;
		
		// setup relative ship data
		for(i=0; i<NO_OF_SHIPS; i++, r++) {
			if((i != ownShip) && (i != absoluteShip)) {
				Long_From_3B(c, r->ppivot.vx);
				Long_From_3B(c, r->ppivot.vy);
				Long_From_3B(c, r->ppivot.vz);
				Short_From_B(c, r->haveFired);
				s				= (ushort *) c;
				r->hdg			= *s++;
				r->pitch		= *s++;
				r->roll			= *s++;
				r->nearSecNo	= *s++;
				r->prevSecNo	= *s++;
				c				= (uchar *) s;
	}

#endif	//NEW_DELTA_CODE_FOR_LATER

//-------------------------------------------------------------------

#ifdef	TRANSFER_v3
void		NewMaster(combatData *packetData, ShipData *shipInfo)
{
	ushort		*s;
	uchar		*c;
	int			i;

	fn_track("newmaster\n")
	
	// terminate on master
	if(exit_status_on) {
		fn_track("exit: initiated by master.\n")
		MAK_CloseLink();
		return;
	}

	// if master quits/retires terminate next frame
	if(	(shipInfo[ownShip].haveFired == QUIT) || (shipInfo[ownShip].haveFired == RETIRE) ) exit_status_on = 1;

	// read data after first frame (absolute slave ship)
	if(delta_frames_on) {
		ReadSIOData(&(receiveBuf[0]), MASTER_RECEIVE_SIZE);
		c = (uchar *) &(receiveBuf[0]);
		Long_From_3B(c, shipInfo[serialShip].ppivot.vx);
		Long_From_3B(c, shipInfo[serialShip].ppivot.vy);
		Long_From_3B(c, shipInfo[serialShip].ppivot.vz);
		Short_From_B(c, shipInfo[serialShip].haveFired);
		s = (ushort *) c;
		shipInfo[serialShip].hdg		= *s++;
		shipInfo[serialShip].pitch		= *s++;
		shipInfo[serialShip].roll		= *s++;
		shipInfo[serialShip].nearSecNo	= *s++;
		shipInfo[serialShip].prevSecNo	= *s;
	}

	// if receive quit/retire from slave quit now
	if( (shipInfo[serialShip].haveFired == QUIT) || (shipInfo[serialShip].haveFired == RETIRE) ) {
		fn_track("exit: received from slave.\n")
		MAK_CloseLink();
		return;
	}

	// first frame code
	if(!delta_frames_on) delta_frames_on = 1;

	#ifdef	FRAME_CHECKING
	if(delta_frames_on) {
		// display header
		printf("This machine is the MASTER.\n");
		printf("absoluteShip: %d\n", absoluteShip);
		printf("ownShip: %d\n", ownShip);
		printf("serialShip: %d\n", serialShip);
		
		// display slave ship data (after delta de-compression from master)
		printf("Slave Ship Data AFTER transfer.\n");
		printf("ppivot.vx: %lx\n", shipInfo[serialShip].ppivot.vx);
		printf("ppivot.vy: %lx\n", shipInfo[serialShip].ppivot.vy);
		printf("ppivot.vz: %lx\n", shipInfo[serialShip].ppivot.vz);
		printf("haveFired: %lx\n", shipInfo[serialShip].haveFired);
		printf("hdg: %lx\n", shipInfo[serialShip].hdg);
		printf("pitch: %lx\n", shipInfo[serialShip].pitch);
		printf("roll: %lx\n", shipInfo[serialShip].roll);
		printf("nearSecNo: %lx\n", shipInfo[serialShip].nearSecNo);
		printf("prevSecNo: %lx\n", shipInfo[serialShip].prevSecNo);

		// second frame delta compression testing
		while(1){;};	// lock
	}
	#endif	//FRAME_CHECKING

	// write absolute data
	c = (uchar *) &transmitBuf[0];
	for(i=0; i<NO_OF_SHIPS; i++) {
		if(i != serialShip) {
			Long_2_3B(c, shipInfo[i].ppivot.vx);
			Long_2_3B(c, shipInfo[i].ppivot.vy);
			Long_2_3B(c, shipInfo[i].ppivot.vz);
			Short_2_B(c, shipInfo[i].haveFired);
			s = (ushort *) c;
			*s++	= shipInfo[i].hdg;
			*s++	= shipInfo[i].pitch;
			*s++	= shipInfo[i].roll;
			*s++	= shipInfo[i].nearSecNo;
			*s++	= shipInfo[i].prevSecNo;
			c		= (uchar *) s;
		}
	}
	WriteSIOData(&(transmitBuf[0]), MASTER_TRANSMIT_SIZE);
}

//-------------------------------------------------------------------

void		NewSlave(combatData *packetData, ShipData *shipInfo)
{
	ushort 		*s;
	uchar		*c;
	int			i;

	fn_track("newslave\n")

	// terminate on slave
	if(exit_status_on) {
		fn_track("exit: initiated by slave.\n")
		MAK_CloseLink();
		return;
	}

	// if slave quits/retires send info to master and terminate next frame
	if(	(shipInfo[ownShip].haveFired == QUIT) || (shipInfo[ownShip].haveFired == RETIRE) ) exit_status_on = 1;

	// read data after first frame
	if(delta_frames_on) {
		ReadSIOData(&(receiveBuf[0]), SLAVE_RECEIVE_SIZE);
		c = (uchar *) &(receiveBuf[0]);
		for(i=0; i<NO_OF_SHIPS; i++) {
			if(i != ownShip) {
				Long_From_3B(c, shipInfo[i].ppivot.vx);
				Long_From_3B(c, shipInfo[i].ppivot.vy);
				Long_From_3B(c, shipInfo[i].ppivot.vz);
				Short_From_B(c, shipInfo[i].haveFired);
				s = (ushort *) c;
				shipInfo[i].hdg			= *s++;
				shipInfo[i].pitch		= *s++;
				shipInfo[i].roll		= *s++;
				shipInfo[i].nearSecNo	= *s++;
				shipInfo[i].prevSecNo	= *s++;
				c = (uchar *) s;
			}
		}
	}

	// if receive quit/retire from master quit now
	if( (shipInfo[serialShip].haveFired == QUIT) || (shipInfo[serialShip].haveFired == RETIRE) ) {
		fn_track("exit: receive from master.\n")
		MAK_CloseLink();
		return;
	}

	// first frame code
	if(!delta_frames_on) delta_frames_on = 1;

	#ifdef	FRAME_CHECKING
	if(delta_frames_on) {
		// display header
		printf("This machine is a SLAVE.\n");
		printf("absoluteShip: %d\n", absoluteShip);
		printf("ownShip: %d\n", ownShip);
		printf("serialShip: %d\n", serialShip);
		
		// display slave ship data (before delta de-compression to master)
		printf("Slave Ship Data BEFORE transfer.\n");
		printf("ppivot.vx: %lx\n", shipInfo[ownShip].ppivot.vx);
		printf("ppivot.vy: %lx\n", shipInfo[ownShip].ppivot.vy);
		printf("ppivot.vz: %lx\n", shipInfo[ownShip].ppivot.vz);
		printf("haveFired: %lx\n", shipInfo[ownShip].haveFired);
		printf("hdg: %lx\n", shipInfo[ownShip].hdg);
		printf("pitch: %lx\n", shipInfo[ownShip].pitch);
		printf("roll: %lx\n", shipInfo[ownShip].roll);
		printf("nearSecNo: %lx\n", shipInfo[ownShip].nearSecNo);
		printf("prevSecNo: %lx\n", shipInfo[ownShip].prevSecNo);

		// second frame delta compression testing
		while(1){;};	// lock
	}
	#endif	//FRAME_CHECKING

	// write data every frame (slave ship)
	c = (uchar *) &(transmitBuf[0]);
	Long_2_3B(c, shipInfo[ownShip].ppivot.vx);
	Long_2_3B(c, shipInfo[ownShip].ppivot.vy);
	Long_2_3B(c, shipInfo[ownShip].ppivot.vz);
	Short_2_B(c, shipInfo[ownShip].haveFired);
	s		= (ushort *) c;
	*s++	= shipInfo[ownShip].hdg;
	*s++	= shipInfo[ownShip].pitch;
	*s++	= shipInfo[ownShip].roll;
	*s++	= shipInfo[ownShip].nearSecNo;
	*s		= shipInfo[ownShip].prevSecNo;
	WriteSIOData(&(transmitBuf[0]), SLAVE_TRANSMIT_SIZE);
}
#endif	//TRANSFER_v3

//-------------------------------------------------------------------

