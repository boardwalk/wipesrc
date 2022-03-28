//-----------------------------------------------------------------------//
// WIPEOUT PC Serial game routines.
// Copyright(C). Sony Interactive. (05/09/95). Mark Kelly.
//-----------------------------------------------------------------------//

#include			<libsnd.h>
#include			"..\origsrc\standard.h"
//#include			<sys\file.h>
#include			<sys\ioctl.h>
#define				LANGUAGE_C
#include			<asm.h>
#include			<kernel.h>

#include			"..\origsrc\malloc.h"
#include			"..\origsrc\tim.h"
#include			"..\origsrc\object.h"
#include			"..\origsrc\skeleton.h"
#include			"..\origsrc\track.h"
#include			"..\origsrc\ships.h"
#include			"..\origsrc\weapon.h"
#include			"..\origsrc\control.h"
#include			"..\origsrc\droid.h"
#include			"..\origsrc\camera.h"
#include			"..\origsrc\menus.h"
#include			"..\origsrc\handlers.h"
#include			"..\origsrc\dynam.h"
#include			"..\origsrc\sound.h"
#include			"..\origsrc\sparks2.h"
#include			"..\origsrc\global.h"

#define				_SIZE_T_DEFINED_
#include			"c:\wc10\h\stdio.h"
#define				_DEFINE_COMBAT_DATA_
#include			"..\markk\serialwo.h"
#include			"..\markk\comio.h"
#include			"..\markk\merror.h"
#include			"..\markk\debug.h"

//-----------------------------------------------------------------------//

long				   	CommSerialConnectMethod;
long					CommSerialMachine;
long					CommSerialDevice;
long					CommSerialBaudRate;
long					CommSerialDataSize;
long					CommSerialParity;
long					CommSerialStopSize;
char					CommMasterCount;
char					CommMasterCount2;
char					transmitBuf[DATA_PACKET_SIZE];	/* transmit buffer */
char					receiveBuf[DATA_PACKET_SIZE];	/* receive buffer */

char					write_flag[NO_OF_SHIPS];
char					read_flag[NO_OF_SHIPS];
char					read_size;
char					write_size;
long					first_read;
long					cycle_count;
long					exit_status_on;
long					delta_frames_on;

struct _ship_packet		oldshipInfo[NO_OF_SHIPS];
char					write_lookup[16] =	{	0,	//	0000
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

char				   	first = SINGLE;
long				   	fr,fw;							/* file descriptor */
static unsigned long   	ev1, ev2;						/* read/write event descriptor */
static unsigned long   	linkev1, linkev2;				/* read/write event descriptor */
static long			   	initfr,initfw;					/* file descriptor */
static unsigned long   	initev1, initev2;				/* read/write event descriptor */
static unsigned long   	errorev;						/* error event descriptor */
static unsigned long   	timeev;							/* timeout event descriptor */
static unsigned long   	ev_r, ev_e, ev_w;				/* read/error event descriptor */
int					   	writeFlag = 0;
static				   	readMFlag = 1;
static				   	readSFlag = 1;

//-----------------------------------------------------------------------//

/* Connect and synchronsize serial machines. */
long				MAK_SerialSynchronize(void)
{
	long	connect_error = OK;
	long	no_connection = 1;
	long	retries = _HANDSHAKE_RETRIES;

	fn_track("mak_serialsynchronize\n")

	CommMasterCount = 0;
	CommMasterCount2 = 0;
	
	// first handshake and synchronise both machines (keep a count for master/slave select later)
	while(no_connection) {
		delay(_1_FRAME);
		cwrite(CommSerialDevice, "HANDHAND", _SYNC_PACKET_SIZE*2);
		
		// collect 8 bytes of data if there is any
		if((com_ReceiveTail > com_ReceiveHead) && (com_ReceiveTail - com_ReceiveHead >= 8)) {
			no_connection = MAK_CheckSerialHandshake();
		}
		if((com_ReceiveTail < com_ReceiveHead) && ((_RECEIVE_BUFFER_SIZE - com_ReceiveHead + com_ReceiveTail) >= 8)) {
			no_connection = MAK_CheckSerialHandshake();
		}
		
		// increment master counter
		CommMasterCount++;
		
		// keep retrying until we decide we've not got a connection.
		if(!retries--) return (_ERR_HANDSHAKING_SERIAL);
	}
	
	// now synchronise both machine's circular buffer pointers.
	delay(_1_FRAME);
	MAK_SendSerialSync();
	delay(_1_FRAME);
	connect_error = MAK_SearchSerialSync();
	if(connect_error) return (_ERR_SYNCHRONISING_SERIAL);
	delay(_1_FRAME);
	MAK_SendSerialAckn();
	delay(_1_FRAME);
	connect_error = MAK_SearchSerialAckn();
	if(connect_error) return (_ERR_SYNCHRONISING_SERIAL);
	
	fn_track("serialhandshake completed\n")
	return (OK);
}

//-----------------------------------------------------------------------//

/* read next handshake packet. */
long				MAK_CheckSerialHandshake(void)
{
	long	SizeToEnd;
	long	WrapSize;
	char	handshake[9];
	char	*hptr = handshake;
	long	i;

	fn_track("mak_checkserialhandshake\n")

	if((com_ReceiveHead + 8) > _RECEIVE_BUFFER_SIZE) {
		// Wrap Read
		SizeToEnd = _RECEIVE_BUFFER_SIZE - com_ReceiveHead;
		memcpy(handshake, &com_ReceiveBuffer[com_ReceiveHead], SizeToEnd);
		WrapSize = 8 - SizeToEnd;
		memcpy(handshake+SizeToEnd, &com_ReceiveBuffer, WrapSize);
		com_ReceiveHead = WrapSize;
	}
	else {
		memcpy(handshake, &com_ReceiveBuffer[com_ReceiveHead], 8);
		com_ReceiveHead += 8;
	}

	// now look for HAND appearing somewhere in this array.
	for(i=0; i<8; i++, hptr++) {
		if(memcmp("HAND", hptr, 4) == 0) return 0;
	}
	return 1;
}

//-----------------------------------------------------------------------//

/* Write out a SYNC packet. */
long				MAK_SendSerialSync(void)
{
	fn_track("mak_sendserialsync\n")

	cwrite(CommSerialDevice, "SYNC", _SYNC_PACKET_SIZE);

	return (OK);	
}

//-----------------------------------------------------------------------//

/* Write out an ACKN packet. */
long				MAK_SendSerialAckn(void)
{
	fn_track("mak_sendserialackn\n")

	cwrite(CommSerialDevice, "ACKN", _SYNC_PACKET_SIZE);

	return (OK);	
}

//-----------------------------------------------------------------------//

/* Locate SYNC packet in receive buffer. */
long				MAK_SearchSerialSync(void)
{
	long	no_sync = 1;
	char	sync[4] = { 0, 0, 0, 0 };
	
	fn_track("mak_searchserialsync\n")

	while(no_sync) {
//		delay(_1_FRAME);
		delay(5);
		memset(sync, '\0', 4);
		while(sync[0] != 'S') cread(CommSerialDevice, &sync[0], 1);
		cread(CommSerialDevice, &sync[1], 3);
		if(memcmp("SYNC", &sync[0], 4) == 0) no_sync = 0;
	}
	return (OK);
}

//-----------------------------------------------------------------------//

/* Locate ACKN packet in receive buffer. */
long				MAK_SearchSerialAckn(void)
{
	long	no_sync = 1;
	char	sync[4] = { 0, 0, 0, 0 };
	
	fn_track("mak_searchserialackn\n")

	while(no_sync) {
//		delay(_1_FRAME);
		delay(5);
		memset(sync, '\0', 4);
		while(sync[0] != 'A') cread(CommSerialDevice, &sync[0], 1);
		cread(CommSerialDevice, &sync[1], 3);
		if(memcmp("ACKN", &sync[0], 4) == 0) no_sync = 0;
	}	
	
	return (OK);
}

//-----------------------------------------------------------------------//

/* Select which machine is master and which is slave using rand number comparison. */
long				MAK_ResolveSerial(void)
{
	long	connect_error = OK;

	fn_track("mak resolveserial\n")

	connect_error = MAK_ResolveSerialMasterSlave();
	if(connect_error) {
		CommLinkError = TRUE;
		fn_track("failure resolving\n")
		MAK_ReportConnectFailure(connect_error);
	}

	fn_track("resolve completed\n")
	return (OK);
}

//-----------------------------------------------------------------------//

/* Select which machine is master and which is slave using rand number comparison. */
long				MAK_ResolveSerialMasterSlave(void)
{
	long	not_resolved = 1;
	
	fn_track("mak resolveserial\n")

	cwrite(CommSerialDevice, &CommMasterCount, 1);
	delay(_1_FRAME);
	cread(CommSerialDevice, &CommMasterCount2, 1);
	
	if(CommMasterCount2 > CommMasterCount) {
		gameType = SLAVE;
		not_resolved = 0;
		fn_track("resolved as slave\n")
	}
	else if(CommMasterCount2 < CommMasterCount) {
		gameType = MASTER;
		not_resolved = 0;
		fn_track("resolved as master\n")
	}
	else return (_ERR_RESOLVING_SERIAL);
	
	return (OK);
}

//-----------------------------------------------------------------------//

#ifdef		TRANSFER_v7
/* Latest serial transfer function uses change compressed delta data on master ships. */
void				NewMaster(combatData *packetData, ShipData *shipInfo)
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

	if(CommMultiPlaySelect == _HEAD_2_HEAD)
	{

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
	} // end NewMaster SERIAL
	
	if(CommMultiPlaySelect == _TCP_NETWORK) MAK_RaceNetworkMaster(shipInfo);
}

//-----------------------------------------------------------------------//

/* Latest serial transfer function uses change de-compressed delta data from master ships. */
void				NewSlave(combatData *packetData, ShipData *shipInfo)
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

	if(CommMultiPlaySelect == _HEAD_2_HEAD)
	{
	
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

	if(CommMultiPlaySelect == _TCP_NETWORK) MAK_RaceNetworkSlave(shipInfo);
}
#endif	//TRANSFER_v7

//-----------------------------------------------------------------------//
