//-----------------------------------------------------------------------//
// Network low-level library.
// Copyright(C). Sony Interactive. (05/09/95). Mark Kelly.
//-----------------------------------------------------------------------//

//-----------------------------------------------------------------------//
/* IPX transport layer library code (gamecode-independent level). */
//-----------------------------------------------------------------------//

#include	"c:\wc10\h\stdio.h"
#include	"c:\wc10\h\string.h"
#include	"c:\wc10\h\dos.h"
#include	"..\markk\netio.h"
#include	"..\markk\merror.h"
#include	"..\markk\debug.h"

//-----------------------------------------------------------------------//

volatile union REGS					net_regs;
volatile struct SREGS				net_sregs;
volatile struct _dpmi				dpmi_regs;

volatile long						IPX_OpenMode;
volatile struct _address __far		IPX_Address[MAX_NUM_TARGETS];
volatile struct _address __far		IPX_BroadCast;
volatile struct _mem_head __far		*rm_stack;

// send control
volatile struct _mem_head __far		*send;
volatile struct _mem_head __far		*ipx_hed;
volatile struct _mem_head __far		*data_hed;

// receive control
volatile struct _mem_head __far		*esr;
volatile struct _mem_head __far		*event[MAX_IPX_EVENTS];
volatile struct _mem_head __far		*ipx_rec[MAX_IPX_EVENTS];
volatile struct _mem_head __far		*data_rec[MAX_IPX_EVENTS];

// control blocks
volatile struct _ecb __far			*event_block[MAX_IPX_EVENTS];
volatile struct _esr __far			*esr_block;
volatile struct _ecb __far			*send_block;
volatile char __far					*data_block;

long								debug_socket_open = 0;

//-----------------------------------------------------------------------//

long				nopen(long socket)
{
	long	error = OK;
		
	fn_track("nopen\n")
	
	// check for IPX installation
	error = IPX_InstallationCheck();
	fn_track("ipx install status: ")
	fn_var(error)
	if(error) return (error);

	// open a network connection socket
	error = IPX_OpenSocket(socket);
	fn_track("ipx open socket status: ")
	fn_var(error)
	if(error) return (error);

	return (OK);
}

//-----------------------------------------------------------------------//

long				nclose(long socket)
{
	long	error = OK;

	fn_track("nclose\n")
	
	// close a network connection socket
	error = IPX_CloseSocket(socket);
	fn_track("ipx close socket status: ")
	fn_var(error)
	if(error) return (error);

	return (OK);
}

//-----------------------------------------------------------------------//

long				nread(void __far *buffer, long size)
{
	char __far	*buf = (char __far *) buffer;
	long		BufSize = size;
 	long		SizeToEnd;
	long		WrapSize;
	long		time = 0;
	long		not_ready = 1;
	char __far	*circ_buffer;	

	fn_track("nread\n")
	
	if(size > MAX_IPX_DATA_SIZE) return (_ERR_IPX_DATA_TOO_BIG);

	/* Wait until at least enough data has arrived. */
	circ_buffer = (char __far *) &esr_block->esr_receive_buffer[0];
	while(not_ready) {
		if((esr_block->esr_receive_tail > esr_block->esr_receive_head) && (esr_block->esr_receive_tail - esr_block->esr_receive_head >= BufSize)) break;
		if((esr_block->esr_receive_tail < esr_block->esr_receive_head) && ((ESR_RECEIVE_BUFFER_SIZE - esr_block->esr_receive_head + esr_block->esr_receive_tail) >= BufSize)) break;
		time++;
		if(time > IPX_READ_TIMEOUT) return (_ERR_IPX_TIMEOUT_NETWORK);
		delay(1);
	}

	/* Read data into buffer. */
	// WATCH FOR BUFFER WRAP!!!
	if((esr_block->esr_receive_head + BufSize) > ESR_RECEIVE_BUFFER_SIZE) {
		// Wraparound read
		SizeToEnd = ESR_RECEIVE_BUFFER_SIZE - esr_block->esr_receive_head;
		_fmemcpy(buf, circ_buffer + esr_block->esr_receive_head, SizeToEnd);
		WrapSize = BufSize - SizeToEnd;
		_fmemcpy(buf + SizeToEnd, circ_buffer, WrapSize);
		esr_block->esr_receive_head = WrapSize;
	}
	else {
		_fmemcpy(buf, circ_buffer + esr_block->esr_receive_head, BufSize);
		esr_block->esr_receive_head += BufSize;
		if(esr_block->esr_receive_head == ESR_RECEIVE_BUFFER_SIZE) esr_block->esr_receive_head = 0;
	}
	
	return (OK);
}

//-----------------------------------------------------------------------//

/* write a buffer out to the send ECB control structure and send the IPX packet. */
/* NOTE: setup *data_block data before this call. */
long				nwrite(struct _address __far *address, long size)
{
	long	error;

	fn_track("nwrite\n")

	if(size > MAX_IPX_DATA_SIZE) return (_ERR_IPX_DATA_TOO_BIG);

	// may need to wait until previous send complete?
	error = IPX_SendPacket(send, ipx_hed, data_hed, address, size);
//	fn_track("cwrite data status: ")
//	fn_var(error)
	if(error) return (error);
	else return (OK);
}

//-----------------------------------------------------------------------//

/* Determine whether Novell IPX software is installed. */
long				IPX_InstallationCheck(void)
{
	fn_track("ipx installationcheck\n")

	/* check for installation of IPX. */
	memset(&net_regs, 0, sizeof(union REGS));
	memset(&net_sregs, 0, sizeof(struct SREGS));
	net_regs.w.ax = 0x7a00;
#ifdef PORT_NET
	int386x(_MULTIPLEX_INT, &net_regs, &net_regs, &net_sregs);
#endif
	if(net_regs.h.al == 0xFF)
	{
		fn_track("IPX is Installed.\n")
		return (OK);
	}
	else
	{
		fn_track("IPX is not Installed.\n")
		return (_ERR_IPX_NOT_PRESENT);
	}
}

//-----------------------------------------------------------------------//

/* Establish a connection over the network. */
long				IPX_OpenSocket(long socket)
{
	fn_track("ipx opensocket\n")

	memset(&net_regs, 0, sizeof(union REGS));
	memset(&net_sregs, 0, sizeof(struct SREGS));
	memset(&dpmi_regs, 0, sizeof(struct _dpmi));
	dpmi_regs.ebx = 0x00000000;
	dpmi_regs.eax = IPX_OpenMode;
	dpmi_regs.edx = ((socket & 0x00FF) << 8) | ((socket & 0xFF00) >> 8);
	net_regs.w.ax = 0x0300;
	net_regs.h.bl = _IPX_INT;
	net_regs.h.bh = 0;
	net_regs.w.cx = 0;
	net_sregs.es = FP_SEG(&dpmi_regs);
	net_regs.x.edi = FP_OFF(&dpmi_regs);
#ifdef PORT_NET
	int386x(_DPMI_INT, &net_regs, &net_regs, &net_sregs);
#endif
	switch (dpmi_regs.eax)
	{
		case	0x00000000	:	{
									fn_track("IPX socket opened.\n")
									debug_socket_open = 1;
									return (OK);
								}
		case	0x000000FE	:	{
									fn_track("IPX socket not opened.\n")
									return (_ERR_IPX_SOCKET_FULL);
								}
		case	0x000000FF	:	{
									fn_track("IPX socket not opened.\n")
									return (_ERR_IPX_SOCKET_OPEN);
								}
		default				:	{
									fn_track("IPX socket not opened.\n")
									return (_ERR_IPX_OPEN_ERROR);
								}
	}
}

//-----------------------------------------------------------------------//

/* Terminate a connection over the network. */
long				IPX_CloseSocket(long socket)
{
	fn_track("ipx closesocket\n")
	
	memset(&net_regs, 0, sizeof(union REGS));
	memset(&net_sregs, 0, sizeof(struct SREGS));
	memset(&dpmi_regs, 0, sizeof(struct _dpmi));
	dpmi_regs.ebx = 0x00000001;
	dpmi_regs.edx = ((socket & 0x00FF) << 8) | ((socket & 0xFF00) >> 8);
	net_regs.w.ax = 0x0300;
	net_regs.h.bl = _IPX_INT;
	net_regs.h.bh = 0;
	net_regs.w.cx = 0;
	net_sregs.es = FP_SEG(&dpmi_regs);
	net_regs.x.edi = FP_OFF(&dpmi_regs);
#ifdef PORT_NET
	int386x(_DPMI_INT, &net_regs, &net_regs, &net_sregs);
#endif

	fn_track("IPX socket closed.\n")
	return (OK);	
}	

//-----------------------------------------------------------------------//

/* Determine the address in the caller's own network to which to transmit to
   in order to reach the specified machine. */
long				IPX_GetInternetworkAddress(char __far *address)
{
	struct _mem_head __far	*low_addr;

	fn_track("ipx getinternetworkaddress\n")
	
	// Setup DOS address buffer. */
	low_addr = IPX_AllocateBlock(FULL_ADDRESS_SIZE);
	
	// Call getinternetwork address function. */
	memset(&net_regs, 0, sizeof(union REGS));
	memset(&net_sregs, 0, sizeof(struct SREGS));
	memset(&dpmi_regs, 0, sizeof(struct _dpmi));
	dpmi_regs.ebx = 0x00000009;
	dpmi_regs.es = low_addr->mem_segment;
	dpmi_regs.esi = 0x00000000;
	net_regs.w.ax = 0x0300;
	net_regs.h.bl = _IPX_INT;
	net_regs.h.bh = 0;
	net_regs.w.cx = 0;
	net_sregs.es = FP_SEG(&dpmi_regs);
	net_regs.x.edi = FP_OFF(&dpmi_regs);
#ifdef PORT_NET
	int386x(_DPMI_INT, &net_regs, &net_regs, &net_sregs);
#endif
		
	/* Return with address buffer. */
	_fmemcpy(address, low_addr->mem, FULL_ADDRESS_SIZE);
	
	/* Free DOS buffer used. */
	IPX_FreeBlock(low_addr);
	
	return (OK);
}

//-----------------------------------------------------------------------//

#ifndef USE_ATI
/* Determine the network station number of the caller's machine. */
long				IPX_GetStationNumber(void)
{
	fn_track("ipx getstationnumber\n")

	memset(&net_regs, 0, sizeof(union REGS));
	memset(&net_sregs, 0, sizeof(struct SREGS));
	memset(&dpmi_regs, 0, sizeof(struct _dpmi));
	dpmi_regs.eax = 0x0000DC00;
	net_regs.w.ax = 0x0300;
	net_regs.h.bl = _DOS_INT;
	net_regs.h.bh = 0;
	net_regs.w.cx = 0;
	net_sregs.es = FP_SEG(&dpmi_regs);
	net_regs.x.edi = FP_OFF(&dpmi_regs);

#ifdef PORT_NET
	int386x(_DPMI_INT, &net_regs, &net_regs, &net_sregs);
#endif
	if(dpmi_regs.eax == 0x00000000)
	{
		fn_track("IPX Error: Netware not loaded.\n")
		return (_ERR_NETWARE_NOT_LOADED);
	}
	else 
	{
		return (dpmi_regs.eax & 0xFF);
	}
}
#endif //USE_ATI

//-----------------------------------------------------------------------//
#ifndef USE_ATI

long				IPX_StationToAddress(struct _address __far *address)
{
	struct _mem_head __far	*rq;
	struct _mem_head __far	*rp;
	struct _req __far		*request;
	struct _resp __far		*response;
	
	fn_track("ipx stationtoaddress\n")
	
	/* Setup DOS Request Buffer. */
	rq = IPX_AllocateBlock(sizeof(struct _req));
	request = (struct _req __far *) rq->mem;
	request->length = sizeof(struct _req) - 2;
	request->type = 0x13;
	request->connect = address->station;

	/* Setup DOS Response Buffer. */
	rp = IPX_AllocateBlock(sizeof(struct _resp));
	response = (struct _resp __far *) rp->mem;
	response->length = sizeof(struct _resp) - 2;

	/* Call Netware Connection Control .*/
	memset(&net_regs, 0, sizeof(union REGS));
	memset(&net_sregs, 0, sizeof(struct SREGS));
	memset(&dpmi_regs, 0, sizeof(struct _dpmi));
	dpmi_regs.eax = 0x0000E300;
	dpmi_regs.ds = rq->mem_segment;
	dpmi_regs.esi = 0;
	dpmi_regs.es = rp->mem_segment;
	dpmi_regs.edi = 0;
	net_regs.w.ax = 0x0300;
	net_regs.h.bl = _DOS_INT;
	net_regs.h.bh = 0;
	net_regs.w.cx = 0;
	net_sregs.es = FP_SEG(&dpmi_regs);
	net_regs.x.edi = FP_OFF(&dpmi_regs);
#ifdef PORT_NET
	int386x(_DPMI_INT, &net_regs, &net_regs, &net_sregs);
#endif
	_fmemcpy(address->network, response->network, NETWORK_ADDRESS_SIZE);
	_fmemcpy(address->node, response->node, NODE_ADDRESS_SIZE);
	
	/* Free DOS buffers used. */
	IPX_FreeBlock(rq);
	IPX_FreeBlock(rp);
	
	return (OK);
}
#endif //USE_ATI

//-----------------------------------------------------------------------//

#ifndef USE_ATI
char __far 			*IPX_GetLocalTarget(struct _address *address)
{
	struct _mem_head __far	*target;
	char __far				*local;

	fn_track("ipx getlocaltarget\n")
	
	/* Setup DOS buffer for target internetwork address. */
	target = IPX_AllocateBlock(FULL_ADDRESS_SIZE * 2);
	_fmemcpy(target->mem, address->network, FULL_ADDRESS_SIZE);
	
	/* Setup low DOS buffer for local target address. */
	local = (char __far *) target->mem + FULL_ADDRESS_SIZE;
		
	/* Call get local target function. */
	memset(&net_regs, 0, sizeof(union REGS));
	memset(&net_sregs, 0, sizeof(struct SREGS));
	memset(&dpmi_regs, 0, sizeof(struct _dpmi));
	dpmi_regs.ebx = 0x00000002;
	dpmi_regs.es = target->mem_segment;
	dpmi_regs.esi = 0;
	dpmi_regs.edi = FULL_ADDRESS_SIZE;
	dpmi_regs.ss = rm_stack->mem_segment;
	dpmi_regs.sp = 0;
	net_regs.w.ax = 0x0300;
	net_regs.h.bl = _IPX_INT;
	net_regs.h.bh = 0;
	net_regs.w.cx = 0;
	net_sregs.es = FP_SEG(&dpmi_regs);
	net_regs.x.edi = FP_OFF(&dpmi_regs);
#ifdef PORT_NET
	int386x(_DPMI_INT, &net_regs, &net_regs, &net_sregs);
#endif
	
	/* Free DOS Buffer used. */
	IPX_FreeBlock(target);
	
	return (local);
}
#endif //USE_ATI

//-----------------------------------------------------------------------//

long						IPX_AllocateEvents(long events)
{
	int		i;
	
	fn_track("ipx allocateevents\n")
	
	if(events > MAX_IPX_EVENTS) return (_ERR_IPX_TOO_MANY_EVENTS);
	for(i=0; i<events; i++) event[i] = IPX_AllocateBlock(sizeof(struct _ecb));
	
	return (OK);
}

//-----------------------------------------------------------------------//

long						IPX_FreeEvents(long events)
{
	int		i;
	
	fn_track("ipx freeevents\n")
	
	if(events > MAX_IPX_EVENTS) return (_ERR_IPX_TOO_MANY_EVENTS);
	for(i=0; i<events; i++) IPX_FreeBlock(event[i]);
	
	return (OK);
}

//-----------------------------------------------------------------------//

#ifndef USE_ATI
struct _mem_head __far		*IPX_InstallESR(struct _esr __far *routine)
{
	struct _mem_head __far	*low_esr;
	
	fn_track("ipx installesr\n")

	/* Setup low DOS buffer for ESR. */
	low_esr = IPX_AllocateBlock(sizeof(struct _esr));
	_fmemcpy(low_esr->mem, routine, sizeof(struct _esr));
	esr_block = (struct _esr __far *) low_esr->mem;
	return (low_esr);
}
#endif //USE_ATI

//-----------------------------------------------------------------------//

long						IPX_InstallEventHandler(void)
{
	fn_track("ipx installeventhandler\n")

	esr = IPX_InstallESR((struct _esr __far *) &esr_start);
	return (OK);
}

//-----------------------------------------------------------------------//

long						IPX_RemoveEventHandler(void)
{
	long	error = OK;

	fn_track("ipx removeeventhandler\n")
	
	error = IPX_FreeBlock(esr);
	return (error);
}

//-----------------------------------------------------------------------//

long						IPX_SetEvents(long events)
{
	int		i;
	long	error = OK;

	fn_track("ipx setevents\n")
	
	if(events > MAX_IPX_EVENTS) return (_ERR_IPX_TOO_MANY_EVENTS);
	for(i=0; i<events; i++) {
		ipx_rec[i] = IPX_AllocateBlock(sizeof(struct _ipx_head));
		data_rec[i] = IPX_AllocateBlock(MAX_IPX_DATA_SIZE);
		error = IPX_SetupEvent(event[i], ipx_rec[i], data_rec[i]);
		if(error) return (error);
		event_block[i] = (struct _ecb __far *) event[i]->mem;
	}
	
	return (OK);
}

//-----------------------------------------------------------------------//

long						IPX_ClearEvents(long events)
{
	int		i;
	long	error = OK;
	
	fn_track("ipx clearevents\n")
	
	if(events > MAX_IPX_EVENTS) return (_ERR_IPX_TOO_MANY_EVENTS);
	for(i=0; i<events; i++) {
		error = IPX_CancelEvent(event[i]);
		if(error) return (error);
		IPX_FreeBlock(ipx_rec[i]);
		IPX_FreeBlock(data_rec[i]);
	}
	
	return (error);
}

//-----------------------------------------------------------------------//

/* Given an ECB Header Block, setup the ECB and assign the ECB to Listen for a packet. */
long						IPX_SetupEvent(struct _mem_head __far *e, struct _mem_head __far *i, struct _mem_head __far *d)
{
	struct _ecb __far		*ecb;
	
//	fn_track("ipx setupevent\n")
	
	/* Setup the ECB structure. */
	ecb = (struct _ecb __far *) e->mem;
	ecb->esr_segment = esr->mem_segment + ((ESR_RECEIVE_BUFFER_SIZE + 16) >> 4);
	ecb->esr_offset = 0;
	ecb->socket = ((IPX_Address[SOURCE].socket & 0x00FF) << 8) | ((IPX_Address[SOURCE].socket & 0xFF00) >> 8);
	_fmemcpy(ecb->local, &IPX_Address[SOURCE].node, NODE_ADDRESS_SIZE);
	ecb->fragments = 2;
	ecb->fdescriptor[0].frag_offset = 0;
	ecb->fdescriptor[0].frag_segment = i->mem_segment;
	ecb->fdescriptor[0].frag_size = sizeof(struct _ipx_head);
	ecb->fdescriptor[1].frag_offset = 0;
	ecb->fdescriptor[1].frag_segment = d->mem_segment;
	fn_track("ecb frag1 segment: ")
	fn_var(d->mem_segment)
	ecb->fdescriptor[1].frag_size = MAX_IPX_DATA_SIZE;
		
	/* Make the ECB listen for a packet. */	
	memset(&net_regs, 0, sizeof(union REGS));
	memset(&net_sregs, 0, sizeof(struct SREGS));
	memset(&dpmi_regs, 0, sizeof(struct _dpmi));
	dpmi_regs.ebx = 0x00000004;
	dpmi_regs.es = e->mem_segment;
	dpmi_regs.esi = 0x00000000;
	dpmi_regs.ss = rm_stack->mem_segment;
	dpmi_regs.sp = 0;
	net_regs.w.ax = 0x0300;
	net_regs.h.bl = _IPX_INT;
	net_regs.h.bh = 0;
	net_regs.w.cx = 0;
	net_sregs.es = FP_SEG(&dpmi_regs);
	net_regs.x.edi = FP_OFF(&dpmi_regs);
#ifdef PORT_NET
	int386x(_DPMI_INT, &net_regs, &net_regs, &net_sregs);
#endif

	return (OK);
}

//-----------------------------------------------------------------------//

long						IPX_CancelEvent(struct _mem_head __far *e)
{
//	fn_track("ipx cancelevent\n")

	/* Cancel ECB Event. */	
	memset(&net_regs, 0, sizeof(union REGS));
	memset(&net_sregs, 0, sizeof(struct SREGS));
	memset(&dpmi_regs, 0, sizeof(struct _dpmi));
	dpmi_regs.ebx = 0x00000006;
	dpmi_regs.es = e->mem_segment;
	dpmi_regs.esi = 0x00000000;
	net_regs.w.ax = 0x0300;
	net_regs.h.bl = _IPX_INT;
	net_regs.h.bh = 0;
	net_regs.w.cx = 0;
	net_sregs.es = FP_SEG(&dpmi_regs);
	net_regs.x.edi = FP_OFF(&dpmi_regs);
#ifdef PORT_NET
	int386x(_DPMI_INT, &net_regs, &net_regs, &net_sregs);
#endif

	return (OK);
}

//-----------------------------------------------------------------------//

struct _mem_head __far		*IPX_AllocateBlock(long size)
{
	struct _mem_head __far	*block;
	unsigned short			block_head_selector;

//	fn_track("ipx allocateblock\n")

	/* Allocate low DOS buffer for MEM Head. */
	memset(&net_regs, 0, sizeof(union REGS));
	memset(&net_sregs, 0, sizeof(struct SREGS));
	net_regs.w.ax = 0x0100;
	net_regs.w.bx = 0x0001;
#ifdef PORT_NET
	int386x(_DPMI_INT, &net_regs, &net_regs, &net_sregs);
#endif
	block_head_selector = net_regs.w.dx;
	block = (struct _mem_head __far *) MK_FP(block_head_selector, 0);

	/* Allocate low DOS buffer for MEM. */
	memset(&net_regs, 0, sizeof(union REGS));
	memset(&net_sregs, 0, sizeof(struct SREGS));
	net_regs.w.ax = 0x0100;
	net_regs.w.bx = (size + 15) >> 4;
#ifdef PORT_NET
	int386x(_DPMI_INT, &net_regs, &net_regs, &net_sregs);
#endif
	block->mem_segment = net_regs.w.ax;
	block->mem_selector = net_regs.w.dx;
	block->mem_head_selector = block_head_selector;
	block->mem = (void __far *) MK_FP(block->mem_selector, 0);

	return (block);
}

//-----------------------------------------------------------------------//

long						IPX_FreeBlock(struct _mem_head __far *block)
{
//	fn_track("ipx freeblock\n")
	
	/* Free DOS Buffer associated with MEM. */
	memset(&net_regs, 0, sizeof(union REGS));
	memset(&net_sregs, 0, sizeof(struct SREGS));
	net_regs.w.ax = 0x0101;
	net_regs.w.dx = block->mem_selector;
#ifdef PORT_NET
	int386x(_DPMI_INT, &net_regs, &net_regs, &net_sregs);
#endif
	
	/* Free DOS Buffer associated with MEM Head. */
	memset(&net_regs, 0, sizeof(union REGS));
	memset(&net_sregs, 0, sizeof(struct SREGS));
	net_regs.w.ax = 0x0101;
	net_regs.w.dx = block->mem_head_selector;
#ifdef PORT_NET
	int386x(_DPMI_INT, &net_regs, &net_regs, &net_sregs);
#endif

	return (OK);
}

//-----------------------------------------------------------------------//

long						IPX_SendPacket(struct _mem_head __far *e, struct _mem_head __far *i, struct _mem_head __far *d, struct _address __far *address, long size)
{
	struct _ecb __far		*ecb;
	struct _ipx_head __far	*ipx;
	long	error = OK;
	
	fn_track("ipx sendpacket\n")

	/* Setup IPX Header. */
	ipx = (struct _ipx_head __far *) i->mem;
	_fmemset(ipx, 0, sizeof(struct _ipx_head));
	ipx->ptype = PACKET_EXCHANGE_PACKET;
	_fmemcpy(ipx->dest_network, address->network, NETWORK_ADDRESS_SIZE);
	_fmemcpy(ipx->dest_node, address->node, NODE_ADDRESS_SIZE);
	ipx->dest_socket = ((address->socket & 0x00FF) << 8) | ((address->socket & 0xFF00) >> 8);
	
	/* Setup ECB. */
	ecb = (struct _ecb __far *) e->mem;
	ecb->link = 0;
	ecb->esr_offset = 0;
	ecb->esr_segment = 0;
	ecb->inuse = 0;
	ecb->code = 0;
	ecb->socket = ((address->socket & 0x00FF) << 8) | ((address->socket & 0xFF00) >> 8);
//	_fmemcpy(ecb->local, IPX_Address[SOURCE].node, NODE_ADDRESS_SIZE);
	_fmemcpy(ecb->local, address->node, NODE_ADDRESS_SIZE);
	ecb->fragments = 2;
	ecb->fdescriptor[0].frag_offset = 0;
	ecb->fdescriptor[0].frag_segment = i->mem_segment;
	ecb->fdescriptor[0].frag_size = sizeof(struct _ipx_head);
	ecb->fdescriptor[1].frag_offset = 0;
	ecb->fdescriptor[1].frag_segment = d->mem_segment;
	ecb->fdescriptor[1].frag_size = size;

	/* Call IPX Send Packet Function. */
	memset(&net_regs, 0, sizeof(union REGS));
	memset(&net_sregs, 0, sizeof(struct SREGS));
	memset(&dpmi_regs, 0, sizeof(struct _dpmi));
	dpmi_regs.ebx = 0x00000003;
	dpmi_regs.es = e->mem_segment;
	dpmi_regs.esi = 0x00000000;
	dpmi_regs.ss = rm_stack->mem_segment;
	dpmi_regs.sp = 0;
	net_regs.w.ax = 0x0300;
	net_regs.h.bl = _IPX_INT;
	net_regs.h.bh = 0;
	net_regs.w.cx = 0;
	net_sregs.es = FP_SEG(&dpmi_regs);
	net_regs.x.edi = FP_OFF(&dpmi_regs);
#ifdef PORT_NET
	int386x(_DPMI_INT, &net_regs, &net_regs, &net_sregs);
#endif
	
	return (error);
}

//-----------------------------------------------------------------------//
