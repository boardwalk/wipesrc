//-----------------------------------------------------------------------//
// Network low-level library header.
// Copyright(C). Sony Interactive. (05/09/95). Mark Kelly.
//-----------------------------------------------------------------------//

#define				FALSE						0
#define				TRUE						1

#define				_DOS_INT					0x21
#define				_MULTIPLEX_INT				0x2F
#define				_DPMI_INT					0x31
#define				_IPX_INT					0x7A

#define				MAX_NET_PLAYERS				8
#define				MAX_NUM_TARGETS				(8 + 1)

#define				_CLOSE_OR_TERMINATE			0x00
#define				_CLOSE						0xFF

#define				_DYNAMIC_ALLOCATION			0x0000

#define				MAX_FRAGMENTS				8
#define				MAX_IPX_EVENTS				24

#define				ESR_RECEIVE_BUFFER_SIZE		(16*1024)
#define				ESR_HANDLER_BUFFER_SIZE		(4*1024)

#define				SUCCESS						0x00
#define				EVENT_NO_CANCEL				0xF9
#define				CANCELLED					0xFC
#define				MALFORMED_PACKET			0xFD
#define				PACKET_UNDELIVERABLE		0xFE
#define				PHYSICAL_ERROR				0xFF

#define				UNKNOWN_PACKET_TYPE			0x00
#define				ROUTING_INFORMATION_PACKET	0x01
#define				ECHO_PACKET					0x02
#define				ERROR_PACKET				0x03
#define				PACKET_EXCHANGE_PACKET		0x04
#define				SPX_PACKET					0x05
#define				INTERNAL_PACKET				0x11

#define				IPX_HEADER_SIZE				sizeof(struct _ipx_head)

#define				NETWORK_ADDRESS_SIZE		4
#define				NODE_ADDRESS_SIZE			6
#define				FULL_ADDRESS_SIZE			NETWORK_ADDRESS_SIZE + NODE_ADDRESS_SIZE

#define				SOURCE						0

#define				MAX_IPX_DATA_SIZE			(1*1024)
#define				STACK_SIZE					(8*1024)

#define				IPX_READ_TIMEOUT			20*(10000/24) // cread gr_info version

//-----------------------------------------------------------------------//

typedef struct _dpmi {
	unsigned long	edi;
	unsigned long	esi;
	unsigned long	ebp;
	unsigned long	_reserved_by_system;
	unsigned long	ebx;
	unsigned long	edx;
	unsigned long	ecx;
	unsigned long	eax;
	unsigned short	flags;
	unsigned short	es;
	unsigned short	ds;
	unsigned short	fs;
	unsigned short	gs;
	unsigned short	ip;
	unsigned short	cs;
	unsigned short	sp;
	unsigned short	ss;
	unsigned short	_not_used;
} DPMI;

typedef struct _mem_head {
	void __far			*mem;
	unsigned short	 	mem_segment;
	unsigned short	 	mem_selector;
	unsigned short	 	mem_head_selector;
} MEM_HEAD;

typedef struct _address {
	long			socket;
	long			station;
	char			bridge[NODE_ADDRESS_SIZE];
	char			network[NETWORK_ADDRESS_SIZE];
	char			node[NODE_ADDRESS_SIZE];
} ADDRESS;

//-----------------------------------------------------------------------//

typedef struct _esr {
	unsigned long	esr_start;
	unsigned long	esr_size;
	unsigned long	esr_receive_head;
	unsigned long	esr_receive_tail;
	unsigned char	esr_receive_buffer[ESR_RECEIVE_BUFFER_SIZE];
	unsigned char	esr_handler_buffer[ESR_HANDLER_BUFFER_SIZE];
} ESR;

typedef struct _frag {
	unsigned short	frag_offset;
	unsigned short	frag_segment;
	unsigned short	frag_size;
} FRAG;

typedef struct _ecb {
	unsigned long	link;
	unsigned short	esr_offset;
	unsigned short	esr_segment;
	unsigned char	inuse;
	unsigned char	code;
	unsigned short	socket;
	unsigned long	_reserved_for_ipx;
	unsigned long	_reserved_for_driver1;
	unsigned long	_reserved_for_driver2;
	unsigned long	_reserved_for_driver3;
	unsigned char	local[NODE_ADDRESS_SIZE];
	unsigned short	fragments;
	struct _frag	fdescriptor[MAX_FRAGMENTS];
} ECB;

typedef struct _ipx_head {
	unsigned short	chksum;
	unsigned short	len;
	unsigned char	tctrl;
	unsigned char	ptype;
	unsigned char	dest_network[NETWORK_ADDRESS_SIZE];
	unsigned char	dest_node[NODE_ADDRESS_SIZE];
	unsigned short	dest_socket;
	unsigned char	src_network[NETWORK_ADDRESS_SIZE];
	unsigned char	src_node[NODE_ADDRESS_SIZE];
	unsigned short	src_socket;
} IPX_HEAD;

typedef struct _req
{
	unsigned short		length;
	unsigned char		type;
	unsigned char		connect;
} REQ;

typedef struct _resp
{
	unsigned short		length;
	unsigned char		network[NETWORK_ADDRESS_SIZE];
	unsigned char		node[NODE_ADDRESS_SIZE];
	unsigned short		socket;
} RESP;
	
//-----------------------------------------------------------------------//

extern volatile union REGS				net_regs;
extern volatile struct SREGS			net_sregs;
extern volatile struct _dpmi			dpmi_regs;

extern volatile long					IPX_OpenMode;
extern volatile struct _address __far	IPX_Address[MAX_NUM_TARGETS];
extern volatile struct _address __far	IPX_BroadCast;
extern volatile struct _mem_head __far	*rm_stack;

extern volatile struct _mem_head __far	*send;
extern volatile struct _mem_head __far	*ipx_hed;
extern volatile struct _mem_head __far	*data_hed;

extern volatile struct _mem_head __far	*esr;
extern volatile struct _mem_head __far	*event[MAX_IPX_EVENTS];
extern volatile struct _mem_head __far	*ipx_rec[MAX_IPX_EVENTS];
extern volatile struct _mem_head __far	*data_rec[MAX_IPX_EVENTS];

extern volatile struct _ecb __far		*event_block[MAX_IPX_EVENTS];
extern volatile struct _esr __far		*esr_block;
extern volatile struct _ecb __far		*send_block;
extern volatile char __far				*data_block;

extern unsigned long					esr_start;
extern unsigned long					esr_receive_head;
extern unsigned long					esr_receive_tail;
extern unsigned char					esr_receive_buffer[];

//-----------------------------------------------------------------------//

extern void __far						IPX_RealModeESR(void);

extern long								nopen(long socket);
extern long								nclose(long socket);
extern long								nread(void __far *buffer, long size);
extern long								nwrite(struct _address __far *address, long size);

extern long								IPX_InstallationCheck(void);
extern long								IPX_OpenSocket(long socket);
extern long								IPX_CloseSocket(long socket);
extern long								IPX_GetInternetworkAddress(char __far *address);
extern long								IPX_GetStationNumber(void);
extern long								IPX_StationToAddress(struct _address _far *address);
extern long								IPX_AllocateEvents(long events);
extern long								IPX_FreeEvents(long events);
extern struct _mem_head __far			*IPX_InstallESR(struct _esr __far *h);
extern long								IPX_InstallEventHandler(void);
extern long								IPX_RemoveEventHandler(void);
extern long								IPX_SetEvents(long events);
extern long								IPX_ClearEvents(long events);
extern long								IPX_SetupEvent(struct _mem_head __far *e, struct _mem_head __far *i, struct _mem_head __far *d);
extern long								IPX_CancelEvent(struct _mem_head __far *e);
extern struct _mem_head __far			*IPX_AllocateBlock(long size);
extern long								IPX_FreeBlock(struct _mem_head __far *m);
extern long								IPX_SendPacket(struct _mem_head __far *e, struct _mem_head __far *i, struct _mem_head __far *d, struct _address __far *address, long size);

//-----------------------------------------------------------------------//
