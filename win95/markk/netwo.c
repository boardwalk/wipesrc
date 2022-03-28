//-----------------------------------------------------------------------//
// WIPEOUT PC Network game routines.
// Copyright(C). Sony Interactive. (05/09/95). Mark Kelly.
//-----------------------------------------------------------------------//

typedef unsigned char		uchar;

//-----------------------------------------------------------------------//

#define				sony_news
#include			<sys\types.h>
#include			<libgte.h>
#include			<libgpu.h>
#include			"..\origsrc\object.h"
#include			"..\origsrc\track.h"

#include			"c:\wc10\h\dos.h"
#define				_SIZE_T_DEFINED_
#include			"c:\wc10\h\stdio.h"
#include			"c:\wc10\h\string.h"
#include			"c:\wc10\h\stdlib.h"
#include			"..\markk\netio.h"
#define				_DEFINE_CONFIG_DATA_
#define				_DEFINE_SHIP_DATA_
#include			"..\markk\netwo.h"
#include			"..\markk\merror.h"
#include			"..\markk\debug.h"

//-----------------------------------------------------------------------//

long						CommNetPlayers;
long						CommNetSocket;
char						GameID[ID_SIZE + 1];
long						NumMachineFound;
long						NumMachineResolved;
struct _address __far		FoundMachineAddress[MAX_NET_MACHINES];
struct _address __far		ResolveMachineAddress[MAX_NET_MACHINES];
long						ResolveMachineCounter[MAX_NET_MACHINES];
struct _address				SortedMachineAddress[MAX_NET_MACHINES];
long						master_counter;
long						CommMyNetPlayerNumber;
long						CommMyShip;
struct _master_menu_packet	NetworkConfigTable;
long						game_packets_on;

//-----------------------------------------------------------------------//

/* Setup the IPX control system for packet handling. */
long				MAK_InitializeNetwork(void)
{
	long	error = OK;
	int		i;

	fn_track("mak initializenetwork\n")

	// initialize variable space
 	for(i=0; i<MAX_NUM_TARGETS; i++)
	{
		IPX_Address[i].socket = CommNetSocket;
		IPX_Address[i].station = 0;
		_fmemset(&IPX_Address[i].bridge, 0, NODE_ADDRESS_SIZE);
		_fmemset(&IPX_Address[i].network, 0, NETWORK_ADDRESS_SIZE);
		_fmemset(&IPX_Address[i].node, 0, NODE_ADDRESS_SIZE);
	}
	IPX_BroadCast.socket = CommNetSocket;
	IPX_BroadCast.station = 0;
	_fmemset(&IPX_BroadCast.bridge, 0, NODE_ADDRESS_SIZE);
	_fmemset(&IPX_BroadCast.network, 0xFF, FULL_ADDRESS_SIZE);
	_fmemset(&IPX_BroadCast.node, 0xFF, NODE_ADDRESS_SIZE);
	strcpy(GameID, "WIPE");
	CommMyNetPlayerNumber = 0;
	CommMyShip = MASTER;
	memset(&NetworkConfigTable, 0, sizeof(struct _master_menu_packet));
	AllPlayersReady = 0;
	StartCount = 0;
	game_packets_on = 0;

	// allocate real-mode stack space
	rm_stack = IPX_AllocateBlock(STACK_SIZE);
	fn_track("ipx alloc stack status: ")
	fn_var((unsigned long) rm_stack->mem_segment)

	// get source station number
//	IPX_Address[SOURCE].station = IPX_GetStationNumber();
//	fn_track("station number: ")
//	fn_var(IPX_Address[SOURCE].station);

	// get source address
//	error = IPX_StationToAddress(&IPX_Address[SOURCE]);
//	fn_track("ipx get ip addr status: ")
//	fn_var(error)
//	if(error) return (error);

	// get source internetwork address
	error = IPX_GetInternetworkAddress(&IPX_Address[SOURCE].network);
	fn_track("ipx get ip addr status: ")
	fn_var(error)
	if(error) return (error);

	fn_track("source address: \n")
	for(i=0; i<FULL_ADDRESS_SIZE; i++) {
		fn_var(IPX_Address[SOURCE].network[i])
	}

	// install ESR
	error = IPX_InstallEventHandler();
	fn_track("ipx alloc esr status: ")
	fn_var(error)
	fn_var((unsigned long) esr->mem_segment)
	if(error) return (error);
	
	// allocate ECBs
	error = IPX_AllocateEvents(MAX_IPX_EVENTS);
	fn_track("ipx alloc ecb[0] status: ")
	fn_var(error)
	fn_var((unsigned long) event[0]->mem_segment)
	if(error) return (error);

	// setup listen ECBs
	error = IPX_SetEvents(MAX_IPX_EVENTS);
	fn_track("ipx setup ecb status: ")
	fn_var(error)
	fn_track("ipx event ecb[23] segment: ")
	fn_var(event[23]->mem_segment)
	if(error) return (error);

	// allocate send ECB
	send = IPX_AllocateBlock(sizeof(struct _ecb));
	fn_track("ipx alloc send-ecb status: ")
	fn_var((unsigned long) send->mem_segment)
	send_block = (struct _ecb __far *) send->mem;
	
	// allocate send ipx header block
	ipx_hed = IPX_AllocateBlock(IPX_HEADER_SIZE);
	fn_track("ipx alloc ipx-head status: ")
	fn_var((unsigned long) ipx_hed->mem_segment)
	
	// allocate send data block
	data_hed = IPX_AllocateBlock(MAX_IPX_DATA_SIZE);
	fn_track("ipx alloc game-data status: ")
	fn_var((unsigned long) data_hed->mem_segment)
	data_block = (char __far *) data_hed->mem;
	
	fn_track("initialize network completed\n\n")
	return (OK);
}

//-----------------------------------------------------------------------//

long				MAK_TerminateNetwork(void)
{
	long	error = OK;
	
	fn_track("mak terminatenetwork\n")

	// free send data block
	error = IPX_FreeBlock(data_hed);
	fn_track("ipx free send data status: ")
	fn_var(error)
	if(error) return (error);
	
	// free send ipx header block
	error = IPX_FreeBlock(ipx_hed);
	fn_track("ipx free send ipx header status: ")
	fn_var(error)
	if(error) return (error);

	// free send control block
	error = IPX_FreeBlock(send);
	fn_track("ipx free send ecb status: ")
	fn_var(error)
	if(error) return (error);
	
	// cancel all event control blocks
	error = IPX_ClearEvents(MAX_IPX_EVENTS);
	fn_track("ipx cancel event status: ")
	fn_var(error)
	if(error) return (error);

	// free event control blocks
	error = IPX_FreeEvents(MAX_IPX_EVENTS);
	fn_track("ipx free event status: ")
	fn_var(error)
	if(error) return (error);

	// remove ESR routine
	error = IPX_RemoveEventHandler();
	fn_track("ipx remove ESR status: ")
	fn_var(error)
	if(error) return (error);

	// free real-mode stack space
	error = IPX_FreeBlock(rm_stack);	
	fn_track("ipx free rm_stack status: ")
	fn_var(error)
	if(error) return (error);

	return (OK);	
}

//-----------------------------------------------------------------------//

/* Determine which machine is MASTER and the order of SLAVE machines. */
long				MAK_HandshakeNetwork(void)
{
	struct _id_packet __far			*id;
	struct _resolve_packet __far	*rsv;
	char __far 						*data2;
	long							no_handshake;
	long							retries = _NETWORK_HANDSHAKE_RETRIES;
	long							i;
	long							found_count;
	long							machine_found;
	
	fn_track("mak handshakenetwork\n");
	
	/* Initialize addresses found array. */
	for(i=0; i<MAX_NET_MACHINES; i++) {
		FoundMachineAddress[i].socket = CommNetSocket;
		_fmemset(&FoundMachineAddress[i].network, 0, FULL_ADDRESS_SIZE);
	}
	
	/* Setup handshake packet containing my internet node address. */
	id = (struct _id_packet __far *) data_block;
	id->PktSize = HANDSHAKE_PACKET_SIZE;
	_fmemcpy(id->GameID, GameID, ID_SIZE);
	id->PktType = HANDSHAKE_PACKET;
	id->PktSyncCounter = 0;
	data2 = (char __far *) (data_block + sizeof(struct _id_packet));
	rsv = (struct _resolve_packet __far *) data2;
	rsv->activity = 0;
	_fmemcpy(rsv->command, "RSV0", COMMAND_SIZE);
	_fmemcpy(rsv->network, &IPX_Address[SOURCE].network, FULL_ADDRESS_SIZE);

	/* Handshake Loop. */	
	master_counter = 0;
	found_count = 0;
	no_handshake = 1;
	NumMachineFound = 0;

	while(no_handshake) {
		delay(100);
		nwrite(&IPX_BroadCast, HANDSHAKE_PACKET_SIZE);
		
		/* Look for next received resolve packet (if any). */
		machine_found = 0;
		if((esr_block->esr_receive_tail > esr_block->esr_receive_head) && (esr_block->esr_receive_tail - esr_block->esr_receive_head >= RESOLVE_PACKET_SIZE)) {
			machine_found = MAK_CheckNetworkHandshake();
		}
		if((esr_block->esr_receive_tail < esr_block->esr_receive_head) && ((ESR_RECEIVE_BUFFER_SIZE - esr_block->esr_receive_head + esr_block->esr_receive_tail) >= RESOLVE_PACKET_SIZE)) {
			machine_found = MAK_CheckNetworkHandshake();
		}
		
		/* If a machine was found add it to handshake completed list. */
		if(machine_found) found_count++;
		if(found_count == CommNetPlayers) no_handshake = 0;
		
		fn_track("machine_found: ")
		fn_var(machine_found)
		fn_track("found_count: ")
		fn_var(found_count)
		
		/* Count for master selection later. */
		master_counter++;
	}
	
	fn_track("handshake completed\n\n")
	return (OK);
}

//-----------------------------------------------------------------------//

/* Read next resolve packet, validate and update Addresses Found list (if valid machine). */
long				MAK_CheckNetworkHandshake(void)
{
	char __far						rcv_buf[1024];
	char __far						*rcv_buf_2;
	struct _id_packet __far			*id;
	struct _resolve_packet __far	*rsv;
	int								i;
	
	/* Fetch data from ESR Receive Circular Buffer. */
	nread(rcv_buf, HANDSHAKE_PACKET_SIZE);
	rcv_buf_2 = (rcv_buf + ID_PACKET_SIZE);
	id = (struct _id_packet __far *) rcv_buf;
	rsv = (struct _resolve_packet __far *) rcv_buf_2;
	
	/* Validate resolve packet. */

	if(_fmemcmp(id->GameID, "WIPE", ID_SIZE) != 0) return (FALSE);
	if(id->PktType != HANDSHAKE_PACKET) return (FALSE);
	if(_fmemcmp(rsv->command, "RSV0", COMMAND_SIZE) != 0) return (FALSE);
	
	fn_track("received address: \n")
	for(i=0; i<FULL_ADDRESS_SIZE; i++) {
		fn_var(rsv->network[i])
	}
	
	/* See if we have already acquired this address. */
	for(i=0; i<MAX_NET_MACHINES; i++) {
		if(_fmemcmp(&FoundMachineAddress[i].network, rsv->network, FULL_ADDRESS_SIZE) == 0) return (FALSE);
	}

	fn_track("copying mew machine address\n")
		
	/* Packet Passed all tests. */
	if(NumMachineFound < MAX_NET_MACHINES) _fmemcpy(&FoundMachineAddress[NumMachineFound].network, rsv->network, FULL_ADDRESS_SIZE);
	NumMachineFound++;
	
	return (TRUE);
}

//-----------------------------------------------------------------------//

/* Resolve and Synchronsize between MASTER and each SLAVE. */
long				MAK_ResolveNetwork(void)
{
	struct _id_packet __far			*id;
	struct _resolve_packet __far	*rsv;
	char __far 						*data2;
	long							no_resolve;
	long							retries = _NETWORK_RESOLVE_RETRIES;
	long							i;
	long							found_count;
	long							machine_found;

	long							mch;
	struct _address __far			*Next_Machine;
	
	long							who_is_master;
	long							largest_count;
	long							sindex;
	
	fn_track("\n\nmak resolvenetwork\n");
	
	/* Initialize sorted address arrays. */
	for(i=0; i<MAX_NET_MACHINES; i++) {
		ResolveMachineAddress[i].socket = CommNetSocket;
		_fmemset(&ResolveMachineAddress[i].network, 0, FULL_ADDRESS_SIZE);
		ResolveMachineCounter[i]= 0;
	}
	
	/* Setup resolve packet containing my internet node address. */
	id = (struct _id_packet __far *) data_block;
	id->PktSize = RESOLVE_PACKET_SIZE;
	_fmemcpy(id->GameID, GameID, ID_SIZE);
	id->PktType = RESOLVE_PACKET;
	id->PktSyncCounter = 0;
	data2 = (char __far *) (data_block + sizeof(struct _id_packet));
	rsv = (struct _resolve_packet __far *) data2;
	rsv->activity = master_counter;
	_fmemcpy(rsv->command, "RSV1", COMMAND_SIZE);
	_fmemcpy(rsv->network, &IPX_Address[SOURCE].network, FULL_ADDRESS_SIZE);

	/* write my resolve information to all machines in the found list. */
	mch = 0;
	Next_Machine = (struct _address __far *) &FoundMachineAddress[0];
	for(i=0; i<CommNetPlayers; i++)	{
//		delay(100);
		nwrite(Next_Machine, RESOLVE_PACKET_SIZE);
		mch++;
		Next_Machine = (struct _address __far *) &FoundMachineAddress[mch];
	}

	/* Collect Resolve Packets Loop. */
	NumMachineFound = 0;
	NumMachineResolved = 0;
	found_count = 0;
	no_resolve = 1;
	while(no_resolve)
	{
//		delay(100);
		machine_found = 0;
		machine_found = MAK_CheckNetworkResolve();
		if(machine_found) found_count++;
		if(found_count == CommNetPlayers) {
			no_resolve = 0;
		 	break;
		}
	}
	
	fn_track("\npre-sorted address[0]: ")
	for(i=0; i<FULL_ADDRESS_SIZE; i++) 
	{
		fn_var(ResolveMachineAddress[0].network[i])
	}

	/* Initialize sorted address arrays. */
	for(i=0; i<MAX_NET_MACHINES; i++) {
		SortedMachineAddress[i].socket = CommNetSocket;
		_fmemset(&SortedMachineAddress[i].network, 0, FULL_ADDRESS_SIZE);
	}

	// finally setup MASTER
	who_is_master = 0;
	largest_count = 0;
	for(i=0; i<CommNetPlayers; i++)
	{
		if(ResolveMachineCounter[i] > largest_count)
		{
			largest_count = ResolveMachineCounter[i];
			who_is_master = i;
		}
	}
	_fmemcpy(&SortedMachineAddress[MASTER].network, &ResolveMachineAddress[who_is_master].network, FULL_ADDRESS_SIZE);

	fn_track("\npre-sorted address[0]: ")
	for(i=0; i<FULL_ADDRESS_SIZE; i++) 
	{
		fn_var(SortedMachineAddress[0].network[i])
	}
		
	/* bubble sort remaining machines on ip address. */
	sindex = 1;
	for(i=0; i<CommNetPlayers; i++)
	{
		if(i != who_is_master)
		{
			_fmemcpy(&SortedMachineAddress[sindex].network, &ResolveMachineAddress[i].network, FULL_ADDRESS_SIZE);
			sindex++;
		}
	}
	qsort(&SortedMachineAddress[1], (CommNetPlayers - 1), sizeof(struct _address), MAK_address_compare);
	
	/* Find which is my ship number given sorted machine addresses and my source address. */
	for(i=0; i<CommNetPlayers; i++)
	{
		if(_fmemcmp(&SortedMachineAddress[i].network, IPX_Address[SOURCE].network, FULL_ADDRESS_SIZE) == 0) CommMyShip = i;
	}
	
	/* Set gameType variable. */
	if(CommMyShip == MASTER) gameType = MASTER;
	else gameType = SLAVE;
	
	/* Machine resolve order established - should be the same on all machines. */
	fn_track("\ni am ship number: ")
	fn_var(CommMyShip)
	fn_track("\nresolved machine addresses: \n")
	fn_track("\nMASTER: \t")
	for(i=0; i<FULL_ADDRESS_SIZE; i++) {
		fn_var2(SortedMachineAddress[MASTER].network[i])
	}
	fn_track("\nSLAVE 1: \t")
	for(i=0; i<FULL_ADDRESS_SIZE; i++) {
		fn_var2(SortedMachineAddress[1].network[i])
	}
	fn_track("\nSLAVE 2: \t")
	for(i=0; i<FULL_ADDRESS_SIZE; i++) {
		fn_var2(SortedMachineAddress[2].network[i])
	}
	fn_track("\nSLAVE 3: \t")
	for(i=0; i<FULL_ADDRESS_SIZE; i++) {
		fn_var2(SortedMachineAddress[3].network[i])
	}
	fn_track("\nSLAVE 4: \t")
	for(i=0; i<FULL_ADDRESS_SIZE; i++) {
		fn_var2(SortedMachineAddress[4].network[i])
	}
	fn_track("\nSLAVE 5: \t")
	for(i=0; i<FULL_ADDRESS_SIZE; i++) {
		fn_var2(SortedMachineAddress[5].network[i])
	}
	fn_track("\nSLAVE 6: \t")
	for(i=0; i<FULL_ADDRESS_SIZE; i++) {
		fn_var2(SortedMachineAddress[6].network[i])
	}
	fn_track("\nSLAVE 7: \t")
	for(i=0; i<FULL_ADDRESS_SIZE; i++) {
		fn_var2(SortedMachineAddress[7].network[i])
	}
	
	fn_track("\nresolve completed\n\n")
//	MAK_CloseLink();
	
	return (OK);
}

//-----------------------------------------------------------------------//

/* Read next resolve packet, validate and update Addresses Found list (if valid machine). */
long				MAK_CheckNetworkResolve(void)
{
	char __far						rcv_buf[1024];
	char __far						*rcv_buf_2;
	struct _id_packet __far			*id;
	struct _resolve_packet __far	*rsv;
	int								i;
	
	fn_track("mak checknetworkresolve\n")
	
	/* Fetch data from ESR Receive Circular Buffer. */
	nread(rcv_buf, RESOLVE_PACKET_SIZE);
	rcv_buf_2 = (rcv_buf + ID_PACKET_SIZE);
	id = (struct _id_packet __far *) rcv_buf;
	rsv = (struct _resolve_packet __far *) rcv_buf_2; 
	
	fn_track("mak checknetworkresolve1\n")
	/* Validate resolve packet. */
	if(_fmemcmp(id->GameID, "WIPE", ID_SIZE) != 0) return (FALSE);
	fn_track("mak checknetworkresolve2\n")
	fn_track("Packet Type: ")
	fn_var(id->PktType)	
	if(id->PktType != RESOLVE_PACKET) return (FALSE);
	fn_track("mak checknetworkresolve3\n")
	if(_fmemcmp(rsv->command, "RSV1", COMMAND_SIZE) != 0) return (FALSE);
	fn_track("mak checknetworkresolve4\n")
	
	fn_track("received address: \n")
	for(i=0; i<FULL_ADDRESS_SIZE; i++) {
		fn_var(rsv->network[i])
	}
	
	/* See if we have already acquired this address. */
	for(i=0; i<MAX_NET_MACHINES; i++) {
		if(_fmemcmp(&ResolveMachineAddress[i].network, rsv->network, FULL_ADDRESS_SIZE) == 0) return (FALSE);
	}

	fn_track("copying mew machine address\n")
		
	/* Packet passed all tests. */
	if(NumMachineFound < MAX_NET_MACHINES) {
		_fmemcpy(&ResolveMachineAddress[NumMachineFound].network, rsv->network, FULL_ADDRESS_SIZE);
		ResolveMachineCounter[NumMachineFound] = rsv->activity;
	}

	fn_track("received address: \n")
	for(i=0; i<FULL_ADDRESS_SIZE; i++) {
		fn_var(ResolveMachineAddress[NumMachineFound].network[i])
	}
	fn_track("receive activity: \n")
	fn_var(ResolveMachineCounter[NumMachineFound])

	NumMachineFound++;
	
	return (TRUE);
}

//-----------------------------------------------------------------------//

void				TestHardWirePacketSend(void)
{
	struct _address __far			*w;
	struct _id_packet __far			*id;
	struct _resolve_packet __far	*rsv;
	char __far 						*data2;
	
	char __far						data_block1[1024];
	char __far						data_block2[1024];
	
	struct _address __far			HardMachineAddress1;
	struct _address __far			HardMachineAddress2;
	
	char __far						address1[FULL_ADDRESS_SIZE] = { 0xFF, 0xFF, 0xFF, 0x02, 0x00, 0x20, 0xAF, 0x88, 0x6D, 0xB1 };
	char __far						address2[FULL_ADDRESS_SIZE] = { 0xFF, 0xFF, 0xFF, 0x02, 0x00, 0x20, 0xAF, 0x88, 0x6D, 0x83 };
	
	w = (struct _address __far *) &FoundMachineAddress[0];
	
	/* Setup resolve packet containing my internet node address. */
	id = (struct _id_packet __far *) data_block;
	id->PktSize = RESOLVE_PACKET_SIZE;
	_fmemcpy(id->GameID, GameID, ID_SIZE);
	id->PktType = RESOLVE_PACKET;
	id->PktSyncCounter = 0;
	data2 = (char __far *) (data_block + sizeof(struct _id_packet));
	rsv = (struct _resolve_packet __far *) data2;
	rsv->activity = master_counter;
	_fmemcpy(rsv->command, "RSV1", COMMAND_SIZE);
	_fmemcpy(rsv->network, &IPX_Address[SOURCE].network, FULL_ADDRESS_SIZE);
	
	_fmemcpy(&HardMachineAddress1.network, address1, FULL_ADDRESS_SIZE);
	_fmemcpy(&HardMachineAddress2.network, address2, FULL_ADDRESS_SIZE);
	
	nwrite(&HardMachineAddress1, RESOLVE_PACKET_SIZE);
	nwrite(&HardMachineAddress2, RESOLVE_PACKET_SIZE);
	
	nread(data_block1, RESOLVE_PACKET_SIZE);
	nread(data_block2, RESOLVE_PACKET_SIZE);
	
}

//-----------------------------------------------------------------------//

/*	The Resolved MASTER MACHINE now sends out 1 SYNC packet to each SLAVE.
	Each SLAVE waits for 1 SYNC and then returns 1 ACKN. */
long				MAK_SynchroniseNetwork(void)
{
	struct _id_packet __far			*id;
	struct _resolve_packet __far	*rsv;
	char __far 						*data2;
	long							ackn;
	long							sync;
	long							ackn_count;
	int								i;
	
	fn_track("mak synchronisenetwork\n")
	
	/* Setup sync packet containing my internet node address. */
	id = (struct _id_packet __far *) data_block;
	id->PktSize = SYNC_PACKET_SIZE;
	_fmemcpy(id->GameID, GameID, ID_SIZE);
	id->PktSyncCounter = 0;
	
	data2 = (char __far *) (data_block + sizeof(struct _id_packet));
	rsv = (struct _resolve_packet __far *) data2;
	_fmemcpy(rsv->network, &IPX_Address[SOURCE].network, FULL_ADDRESS_SIZE);
	
	fn_track("commmyship : ")
	fn_var(CommMyShip)

	if(CommMyShip == MASTER)
	{
		fn_track("master :syncing\n")

		// sync with all active slaves
		for(i=(MASTER+1); i<CommNetPlayers; i++)
		{
			// send SYNC to next slave
			id->PktType = SYNC_PACKET;
			_fmemcpy(rsv->command, "SYNC", COMMAND_SIZE);
			nwrite(&SortedMachineAddress[i], SYNC_PACKET_SIZE);

			// wait for next slave ACKN
			ackn = 0;
			while(!ackn)
			{
				ackn = MAK_CheckAcknPacket();
			}
		}
	}
	else 
	{
		fn_track("slave :syncing\n")
		
		// wait for SYNC from MASTER
		sync = 0;
		while(!sync)
		{
			sync = MAK_CheckSyncPacket();
		}
		
		// send ACKN to MASTER
		id->PktType = ACKN_PACKET;
		_fmemcpy(rsv->command, "ACKN", COMMAND_SIZE);
		nwrite(&SortedMachineAddress[MASTER], SYNC_PACKET_SIZE);
	}

	// temp
	//delay(1000);	
	//MAK_CloseLink();
	
	fn_track("synchronise network completed\n\n")
	return (OK);
}

//-----------------------------------------------------------------------//

long				MAK_CheckSyncPacket(void)
{
	char __far						rcv_buf[1024];
	char __far						*rcv_buf_2;
	struct _id_packet __far			*id;
	struct _resolve_packet __far	*rsv;
	int								i;
	
	fn_track("mak checksyncpacket\n")
	
	/* Fetch data from ESR Receive Circular Buffer. */
	nread(rcv_buf, SYNC_PACKET_SIZE);
	rcv_buf_2 = (rcv_buf + ID_PACKET_SIZE);
	id = (struct _id_packet __far *) rcv_buf;
	rsv = (struct _resolve_packet __far *) rcv_buf_2;
	
	/* Validate SYNC packet. */
	if(_fmemcmp(id->GameID, "WIPE", ID_SIZE) != 0) return (FALSE);
	if(id->PktType != SYNC_PACKET) return (FALSE);
	if(_fmemcmp(rsv->command, "SYNC", COMMAND_SIZE) != 0) return (FALSE);
	
	/* packet identified as a SYNC packet. */
	return (TRUE);
}

//-----------------------------------------------------------------------//

long				MAK_CheckAcknPacket(void)
{
	char __far						rcv_buf[1024];
	char __far						*rcv_buf_2;
	struct _id_packet __far			*id;
	struct _resolve_packet __far	*rsv;
	int								i;
	
	fn_track("mak checkacknpacket\n")
	
	/* Fetch data from ESR Receive Circular Buffer. */
	nread(rcv_buf, SYNC_PACKET_SIZE);
	rcv_buf_2 = (rcv_buf + ID_PACKET_SIZE);
	id = (struct _id_packet __far *) rcv_buf;
	rsv = (struct _resolve_packet __far *) rcv_buf_2;
	
	/* Validate ACKN packet. */
	if(_fmemcmp(id->GameID, "WIPE", ID_SIZE) != 0) return (FALSE);
	if(id->PktType != ACKN_PACKET) return (FALSE);
	if(_fmemcmp(rsv->command, "ACKN", COMMAND_SIZE) != 0) return (FALSE);
	
	/* packet identified as an ACKN packet. */
	return (TRUE);
}

//-----------------------------------------------------------------------//

/* comparison function supplied to qsort algorithm above. */
int					MAK_address_compare(void *address1, void *address2)
{
	struct _address __far	*a;
	struct _address __far	*b;
	char					abyte;
	char					bbyte;
	
	a = (struct _address __far *) address1;
	b = (struct _address __far *) address2;
	
	abyte = a->network[FULL_ADDRESS_SIZE-1];
	bbyte = b->network[FULL_ADDRESS_SIZE-1];
	
	if(abyte <  bbyte) return (-1);
	if(abyte == bbyte) return (0);
	if(abyte >  bbyte) return (+1);
	
	return (0);
}

//-----------------------------------------------------------------------//

void				MAK_MasterNetworkMenus(ConfigData *gameData)
{
	struct _id_packet __far				*send_id;
	char __far 							*send_data;
	struct _master_menu_packet __far	*send_master;
	
	char	 							Read_Buffer[MAX_NET_MACHINES][1024];
	char								*receive_data;
	struct _slave_menu_packet			*receive_slave;

	struct _master_menu_packet			*cfg;
	int									i;
	
	fn_track("mak masternetworkmenus")
	
	/* setup send data block using current master menu. */
	cfg = (struct _master_menu_packet *) &NetworkConfigTable;
	send_id = (struct _id_packet __far *) data_block;
	send_id->PktSize = MASTER_MENU_PACKET_SIZE;
	_fmemcpy(send_id->GameID, GameID, ID_SIZE);
	send_id->PktType = MENU_PACKET;
	send_data = (char __far *) (data_block + sizeof(struct _id_packet));
	send_master = (struct _master_menu_packet __far *) send_data;
	send_master->Class				= gameData->class;
	send_master->TrackNum			= gameData->trackNum;
	send_master->ActTrackNum		= gameData->actTrackNum;
	send_master->ShipNum[MASTER]	= gameData->shipNum;
	send_master->Team[MASTER]		= gameData->team;
	send_master->ExitMenu[MASTER]	= gameData->exitMenu;
	for(i=(MASTER+1); i<CommNetPlayers; i++)
	{
		send_master->ShipNum[i]		= cfg->ShipNum[i];
		send_master->Team[i]		= cfg->Team[i];
		send_master->ExitMenu[i]	= cfg->ExitMenu[i];
	}
	
	fn_track("master: sending gameData exitMenu to all SLAVEs: ")
	fn_var(gameData->exitMenu)
	
	/* Get all slave menu information. */
	for(i=(MASTER+1); i<CommNetPlayers; i++)
	{
		/* Send last frame's MASTER PACKET TO next slave machine. */
		nwrite(&SortedMachineAddress[i], MASTER_MENU_PACKET_SIZE);
	
		/* Wait for next slave machine's MENU PACKET. */
		nread(&Read_Buffer[i], SLAVE_MENU_PACKET_SIZE);
	}

	/* Update master menu table and gameData, given all slave packets. */
	cfg->Class				= gameData->class;
	cfg->TrackNum			= gameData->trackNum;
	cfg->ActTrackNum		= gameData->actTrackNum;
	cfg->ShipNum[MASTER]	= gameData->shipNum;
	cfg->Team[MASTER]		= gameData->team;
	cfg->ExitMenu[MASTER]	= gameData->exitMenu;
	for(i=(MASTER+1); i<CommNetPlayers; i++)
	{
		/* setup ptrs into slave packet. */
		receive_data = (char *) (&Read_Buffer[i]);
		receive_data = (char *) (receive_data + sizeof(struct _id_packet));
		receive_slave = (struct _slave_menu_packet *) receive_data;
		cfg->ShipNum[i]		= receive_slave->SlaveShipNum;
		cfg->Team[i]		= receive_slave->SlaveTeam;
		cfg->ExitMenu[i]	= receive_slave->SlaveExitMenu;
		fn_track("MASTER receiving slave exitmenu value: ")
		fn_var(receive_slave->SlaveExitMenu)
	}
}

//-----------------------------------------------------------------------//

void				MAK_SlaveNetworkMenus(ConfigData *gameData)
{
	struct _id_packet __far				*send_id;
	char __far 							*send_data;
	struct _slave_menu_packet __far		*send_slave;
	
	char								Read_Buffer[1024];
	char								*receive_data;
	struct _master_menu_packet			*receive_master;
	
	struct _master_menu_packet			*cfg;
	int									i;
	
	fn_track("mak slavenetworkmenus\n")
	
	// wait for master menu packet to arrive
	nread(&Read_Buffer, MASTER_MENU_PACKET_SIZE);

	// copy master menu packet data to gameData
	receive_data = (char *) (&Read_Buffer);
	receive_data = (char *) (receive_data + sizeof(struct _id_packet));
	receive_master = (struct _master_menu_packet *) receive_data;
	gameData->class				= receive_master->Class;		// gameData->class from MASTER side
	gameData->trackNum			= receive_master->TrackNum;		// gameData->trackNum from MASTER side
	gameData->actTrackNum		= receive_master->ActTrackNum;	// gameData->actTrackNum from MASTER side
	

	/* setup slave packet send data block. */
	send_id = (struct _id_packet __far *) data_block;
	send_id->PktSize = SLAVE_MENU_PACKET_SIZE;
	_fmemcpy(send_id->GameID, GameID, ID_SIZE);
	send_id->PktType = MENU_PACKET;
	send_data = (char __far *) (data_block + sizeof(struct _id_packet));
	send_slave = (struct _slave_menu_packet __far *) send_data;

	/* Copy current menu config into send data_block. */
	send_slave->SlaveShipNum		= gameData->shipNum;
	send_slave->SlaveTeam			= gameData->team;
	send_slave->SlaveExitMenu		= gameData->exitMenu;
	fn_track("slave: sending gameData exitMenu to MASTER: ")
	fn_var(gameData->exitMenu)
	
	/* send packet to MASTER. */
	nwrite(&SortedMachineAddress[MASTER], SLAVE_MENU_PACKET_SIZE);

	/* copy my latest slave data into NetworkConfigTable. */
	cfg = (struct _master_menu_packet *) &NetworkConfigTable;
	cfg->Class					= receive_master->Class;		// gameData->class from MASTER side
	cfg->TrackNum				= receive_master->TrackNum;		// gameData->trackNum from MASTER side
	cfg->ActTrackNum			= receive_master->ActTrackNum;	// gameData->actTrackNum from MASTER side
	fn_track("wrap flag: ")
	fn_var(esr_block->esr_start)
	for(i=0; i<CommNetPlayers; i++)
	{
		if(i != CommMyShip)
		{
			cfg->ShipNum[i]		= receive_master->ShipNum[i];	// gameData->shipNum from machine (i)
			cfg->Team[i]		= receive_master->Team[i];		// gameData->team from machine (i)
			cfg->ExitMenu[i]	= receive_master->ExitMenu[i];	// gameData->exitMenu from machine (i)
			fn_track("receive from master: exit: ")
			fn_var(receive_master->ExitMenu[i])
		}
	}
	cfg->ShipNum[CommMyShip]	= gameData->shipNum;
	cfg->Team[CommMyShip]		= gameData->team;
	cfg->ExitMenu[CommMyShip]	= gameData->exitMenu;
}

//-----------------------------------------------------------------------//

void				MAK_RaceNetworkMaster(struct _ShipData *shipInfo)
{
	struct _id_packet __far			*send_id;
	char __far						*send_data;
	struct _race_packet __far		*send_master;
	struct _ShipData				*s;
	int								i;
	char							Read_Buffer[1024];
	char							*receive_data;
	struct _race_packet				*receive_slave;
	long							id;
	long							no_play;
	
	fn_track("mak racenetworkmaster\n")

	if(!game_packets_on)
	{
		// collect all SLAVE 'PLAY' packets before starting to write RACE packets.
		for(i=(MASTER+1); i<CommNetPlayers; i++)
		{
			no_play = 1;
			while(no_play)
			{
				no_play = MAK_CheckPlayPacket();
			}		
		}
		game_packets_on = 1;
	}

	// fill in send packet
	send_id = (struct _id_packet __far *) data_block;
	send_id->PktSize = MASTER_RACE_PACKET_SIZE;
	_fmemcpy(send_id->GameID, GameID, ID_SIZE);
	send_id->PktType = RACE_PACKET;
	send_data = (char __far *) (data_block + sizeof(struct _id_packet));
	send_master = (struct _race_packet __far *) send_data;
	s = (struct _ShipData *) shipInfo;
	for(i=0; i<NO_OF_SHIPS; i++, s++, send_master++)
	{	
		// fill in next part of master packet using next shipInfo struct
		send_master->MachineID		= MASTER;
		send_master->haveFired		= s->haveFired;
		send_master->hdg			= s->hdg;
		send_master->pitch			= s->pitch;
		send_master->roll			= s->roll;
		send_master->nearSecNo		= s->nearSecNo;
		send_master->prevSecNo		= s->prevSecNo;
		send_master->vx				= s->ppivot.vx;
		send_master->vy				= s->ppivot.vy;
		send_master->vz				= s->ppivot.vz;
	}	

	// send data out to each SLAVE
	for(i=(MASTER+1); i<CommNetPlayers; i++) nwrite(&SortedMachineAddress[i], MASTER_RACE_PACKET_SIZE);

	// terminate on master after last write
	if(exit_status_on)
	{
		MAK_CloseLink();
		return;
	}

	// read slave ship data from all SLAVES (in any order)
	s = (struct _ShipData *) shipInfo;
	for(i=(MASTER+1); i<CommNetPlayers; i++)
	{
		// read next slave ship packet
		nread(&Read_Buffer, SLAVE_RACE_PACKET_SIZE);
		
		// get data ptr
		receive_data = (char *) &Read_Buffer[0];
		receive_data = (char *) (receive_data + sizeof(struct _id_packet));
		receive_slave = (struct _race_packet *) receive_data;
		id = receive_slave->MachineID;
		s = (struct _ShipData *) (shipInfo+id);
		s->haveFired	=	receive_slave->haveFired;
		s->hdg			=	receive_slave->hdg;
		s->pitch		=	receive_slave->pitch;
		s->roll			=	receive_slave->roll;
		s->nearSecNo	=	receive_slave->nearSecNo;
		s->prevSecNo	=	receive_slave->prevSecNo;
		s->ppivot.vx	=	receive_slave->vx;
		s->ppivot.vy	=	receive_slave->vy;
		s->ppivot.vz	=	receive_slave->vz;
	}

	#if	0
	// PAUSE/QUIT/RETIRE control
	for(i=(MASTER); i<CommNetPlayers; i++)
	{
		// If anyone quits then exit next frame after telling everyone else
		if(shipInfo[i].haveFired == QUIT) exit_status_on = 1;
		// If anyone pauses tell everyone else next frame
		if((shipInfo[i].haveFired == PAUSE) && (i != ownShip)) serialShip = i;
	}
	#endif
}

//-----------------------------------------------------------------------//

void				MAK_RaceNetworkSlave(struct _ShipData *shipInfo)
{
	char							Read_Buffer[NO_OF_SHIPS][1024];
	char							*receive_data;
	struct _race_packet				*receive_master;
	struct _id_packet __far 		*send_id;
	char __far 						*send_data;
	struct _race_packet __far		*send_slave;
	struct _ShipData				*s;
	int								i;
	struct _resolve_packet __far 	*send_rsv;

	fn_track("mak racenetworkslave\n")

	if(!game_packets_on)
	{
		// may need to block here until MASTER signals
	
		send_id = (struct _id_packet __far *) data_block;
		send_id->PktSize = PLAY_PACKET_SIZE;
		_fmemcpy(send_id->GameID, GameID, ID_SIZE);
		send_id->PktType = PLAY_PACKET;
		send_data = (char __far *) (data_block + sizeof(struct _id_packet));
		send_rsv = (struct _resolve_packet __far *) send_data;
		_fmemcpy(send_rsv->command, "PLAY", COMMAND_SIZE);
		nwrite(&SortedMachineAddress[MASTER], PLAY_PACKET_SIZE);
		
		game_packets_on = 1;
	}
	
	// read packet for all ships from MASTER
	nread(&Read_Buffer, MASTER_RACE_PACKET_SIZE);
	receive_data = (char *) &Read_Buffer[0];
	receive_data = (char *) (receive_data + sizeof(struct _id_packet));
	receive_master = (struct _race_packet *) receive_data;
	s = (struct _ShipData *) shipInfo;
	for(i=0; i<NO_OF_SHIPS; i++, s++, receive_master++)
	{
		// fill in next ship from MASTER (except me)
		if(i != CommMyShip)	// dont update my slave ship please!
		{
			s->haveFired	=	receive_master->haveFired;
			s->hdg			=	receive_master->hdg;
			s->pitch		=	receive_master->pitch;
			s->roll			=	receive_master->roll;
			s->nearSecNo	=	receive_master->nearSecNo;
			s->prevSecNo	=	receive_master->prevSecNo;
			s->ppivot.vx	=	receive_master->vx;
			s->ppivot.vy	=	receive_master->vy;
			s->ppivot.vz	=	receive_master->vz;
		}
	}

	// if slave quits/retires send info to master and terminate next frame
	if((shipInfo[CommMyShip].haveFired == QUIT) || (shipInfo[CommMyShip].haveFired == RETIRE)) exit_status_on = 1;
	
	// fill in send packet
	send_id = (struct _id_packet __far *) data_block;
	send_id->PktSize = SLAVE_RACE_PACKET_SIZE;
	_fmemcpy(send_id->GameID, GameID, ID_SIZE);
	send_id->PktType = RACE_PACKET;
	send_data = (char __far *) (data_block + sizeof(struct _id_packet));
	send_slave = (struct _race_packet __far *) send_data;
	s = (struct _ShipData *) (shipInfo+CommMyShip);

	// fill in next part of master packet using next shipInfo struct
	send_slave->MachineID		= CommMyShip;		//send me
	send_slave->haveFired		= s->haveFired;
	send_slave->hdg				= s->hdg;
	send_slave->pitch			= s->pitch;
	send_slave->roll			= s->roll;
	send_slave->nearSecNo		= s->nearSecNo;
	send_slave->prevSecNo		= s->prevSecNo;
	send_slave->vx				= s->ppivot.vx;
	send_slave->vy				= s->ppivot.vy;
	send_slave->vz				= s->ppivot.vz;

	// write SLAVE packet to MASTER (with my ID)
	nwrite(&SortedMachineAddress[MASTER], SLAVE_RACE_PACKET_SIZE);

	// terminate on slave after last write
	if(exit_status_on)
	{
		MAK_CloseLink();
		return;
	}

	#if	0
	// PAUSE/QUIT/RETIRE control
	for(i=(MASTER); i<CommNetPlayers; i++)
	{
		// If anyone quits then exit next frame after telling everyone else
		if(shipInfo[i].haveFired == QUIT) exit_status_on = 1;
		// If anyone pauses tell everyone else next frame
		if((shipInfo[i].haveFired == PAUSE) && (i != ownShip)) serialShip = i;
	}
	#endif

}

//-----------------------------------------------------------------------//

long				MAK_CheckPlayPacket(void)
{
	char __far						rcv_buf[1024];
	char __far						*rcv_buf_2;
	struct _id_packet __far			*id;
	struct _resolve_packet __far	*rsv;
	int								i;
	
	fn_track("mak checkplaypacket\n")
	
	/* Fetch data from ESR Receive Circular Buffer. */
	nread(rcv_buf, PLAY_PACKET_SIZE);
	rcv_buf_2 = (rcv_buf + ID_PACKET_SIZE);
	id = (struct _id_packet __far *) rcv_buf;
	rsv = (struct _resolve_packet __far *) rcv_buf_2;
	
	/* Validate ACKN packet. */
	if(_fmemcmp(id->GameID, "WIPE", ID_SIZE) != 0) return (TRUE);
	if(id->PktType != PLAY_PACKET) return (TRUE);
	if(_fmemcmp(rsv->command, "PLAY", COMMAND_SIZE) != 0) return (TRUE);
	
	/* packet identified as a PLAY packet. */
	return (FALSE);
}

//-----------------------------------------------------------------------//
