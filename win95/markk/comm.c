//-------------------------------------------------------------------
// ridge racer serial module
// this code now works with new Ridge Racer code (17/08/95)
//-------------------------------------------------------------------

#include			"..\origsrc\common.h"
#define				VAR_EXTERN
#include			"..\origsrc\var.h"
#include			"..\origsrc\handle.h"
#include			"..\origsrc\sobj.h"
#include			"..\origsrc\bgm.h"
#include			"..\origsrc\drive.h"

#define				_SIZE_T_DEFINED_
#include			"c:\watcom\h\dos.h"
#include			"c:\watcom\h\stdio.h"
#include			"c:\watcom\h\stdlib.h"
#include			"c:\watcom\h\string.h"

#include			"..\markk\comio.h"
#include			"..\markk\input.h"
#include			"..\markk\multi.h"
#include			"..\markk\comm.h"
#include			"..\markk\comerr.h"
#include			"..\markk\newccar.h"
#include			"..\markk\modem.h"
#include			"..\markk\grtext.h"
#include			"..\markk\grinfo.h"

//-------------------------------------------------------------------

/* Initialise multiplayer data. */
void				MAK_InitMulti(void)
{
	fn_create
	fn_track("mak_initmulti\n")

	MAK_InitCarData();
	MAK_InitCommData();
}

//-------------------------------------------------------------------

/* Open a link between two or more machines. */
void				MAK_OpenLink(void)
{
	long	connect_error = OK;

	fn_track("mak_openlink\n")

	// will be setup via option screen when finished
#ifdef	MAK_LEAD
	CommSerialMachine = _LEAD;
#endif
#ifdef	MAK_FOLLOW
	CommSerialMachine = _FOLLOW;
#endif
#ifdef	TEST_SERIAL		/* test option setup. */
	CommMultiPlaySelect = _HEAD_2_HEAD;
	CommSerialConnectMethod = _DIRECT;
	CommSerialDevice = _COM1;
//	CommSerialBaudRate = _19200_BAUD;
	CommSerialBaudRate = _57600_BAUD;
	CommSerialDataSize = _8BIT_DATA;
	CommSerialParity = _NO_PARITY;
	CommSerialStopSize = _1BIT_STOP;
#endif
#ifdef	TEST_NETWORK
	CommNetPlayers = 2;
#endif
#ifdef	TEST_MODEM
	CommSerialConnectMethod = _DIALUP;
	if(CommSerialMachine == _LEAD) CommModemMachine = _DIAL;
	if(CommSerialMachine == _FOLLOW) CommModemMachine = _ANSWER;

	if(CommModemMachine == _DIAL) strcpy(CommModemInitString, InitStringA);
	if(CommModemMachine == _ANSWER) strcpy(CommModemInitString, InitStringB);
//	strcat(CommModemInitString, Enter);
	strcpy(CommModemSetupString, SetupString1);
//	strcat(CommModemSetupString, Enter);
	strcpy(CommModemDialupString, DialupStringA);
//	strcpy(CommModemDialupString, Enter);
	strcpy(CommModemAnswerString, AnswerString);
//	strcpy(CommModemAnswerString, Enter);
	strcpy(CommModemHangupString, HangupString);
//	strcpy(CommModemHangupString, Enter);
#endif	

	// initialise communication system.
	connect_error = MAK_OpenDevice();
	if(connect_error) {
		MAK_ReportConnectFailure(connect_error);
		CommLinkError = TRUE;
		return;
	}

	// connect modem and telephone lines if required
	if(CommSerialConnectMethod == _DIALUP) {
		connect_error = MAK_ConnectModem();
		if(connect_error) {
			MAK_ReportConnectFailure(connect_error);
			CommLinkError = TRUE;
			return;
		}
	}

	// connect the machines to be linked
	connect_error = MAK_SerialHandshake();
	if(connect_error) {
		MAK_ReportConnectFailure(connect_error);
		CommLinkError = TRUE;
		return;
	}
}

//-------------------------------------------------------------------

/* Close Link and terminate environment. */
void				MAK_CloseLink(void)
{
	fn_track("mak_closelink\n")

	if(CommSerialConnectMethod == _DIALUP) {
		MAK_Hangup();
		MAK_DisconnectModem();
	}	
	MAK_CloseDevice();
}

//-------------------------------------------------------------------

/* Initialise Comm Device and Linkup Environment. */
long				MAK_OpenDevice(void)
{
	long	open_error = OK;
	fn_track("mak_opendevice\n")

	/* Initiate Communication Device. */
	switch (CommMultiPlaySelect) {
		case	_SINGLE_PLAY	:	CommNumLinkedCars = 1;
									break;
		case	_HEAD_2_HEAD	:	CommNumLinkedCars = 2;
									open_error = copen(CommSerialDevice, CommSerialBaudRate, CommSerialDataSize, CommSerialParity, CommSerialStopSize);
									if(open_error) return (open_error);
									break;
		case	_TCP_NETWORK	:	CommNumLinkedCars = CommNetPlayers;
									//nopen();
									break;
		default					:	break;
	}

	delay(_1_FRAME*15);
	
	return (OK);
}

//-------------------------------------------------------------------

/* Terminate Comm Device. */
void				MAK_CloseDevice(void)
{
	fn_track("mak_closedevice\n")

	/* Terminate Communication Device. */
	switch (CommMultiPlaySelect)
	{
		case	_SINGLE_PLAY	:	break;	
		case	_HEAD_2_HEAD	:	cclose(CommSerialDevice);
									break;
		case	_TCP_NETWORK	:	//nclose();
									break;
		default					:	break;
	}
}	

//-------------------------------------------------------------------

/* Collect pad data from link machine. */
void				MAK_ReadFrame(void)
{
	char	indata[2] = { 0, 0 };
	long	read_error = 0;

	fn_track("mak_readframe\n")

	/* read next packet. */
	read_error = cread(CommSerialDevice, &indata[0], 2);
	if(read_error) {
		MAK_ReportConnectFailure(read_error);
		CommLinkError = TRUE;
		return;
	}

	/* Copy CRC checked data into pad_buf[1]. */
	pad_buf[1].status = OK;
	pad_buf[1].kind = SONY_PAD;
	pad_buf[1].data[0] = indata[0];
	pad_buf[1].data[1] = indata[1];
}

//-------------------------------------------------------------------

/* Export pad data to other machine. */
void				MAK_WriteFrame(void)
{
	char	outdata[2] = { 0, 0 };

	fn_track("mak_writeframe\n")

	/* copy pad_buf data. */
	outdata[0] = transfer_buf[0].data[0];
	outdata[1] = transfer_buf[0].data[1];
	
	/* write next packet. */
	cwrite(CommSerialDevice, &outdata[0], 2);
}

//-------------------------------------------------------------------

long				MAK_SerialHandshake(void)
{
	long	connect_error = OK;
	long	no_connection = 1;
	long	retries = _HANDSHAKE_RETRIES;

	fn_track("mak_serialhandshake\n")

	// first handshake and synchronise both machines
	while(no_connection) {
		cwrite(CommSerialDevice, "HAND", _SYNC_PACKET_SIZE);

		// collect 8 bytes of data if there is any
		delay(_1_FRAME);
		if((com_ReceiveTail > com_ReceiveHead) && (com_ReceiveTail - com_ReceiveHead >= 8)) {
			no_connection = MAK_CheckHandshake();
		}
		if((com_ReceiveTail < com_ReceiveHead) && ((_RECEIVE_BUFFER_SIZE - com_ReceiveHead + com_ReceiveTail) >= 8)) {
			no_connection = MAK_CheckHandshake();
		}
		
		// keep retrying until we decide we've not got a connection.
		if(!retries--) return (_ERR_HANDSHAKING);
	}
	
	// now synchronise both machine's circular buffer pointers.
	MAK_SendSync();
	delay(_1_FRAME);
	connect_error = MAK_SearchSync();
	if(connect_error) return (_ERR_SYNCHRONISING);
	MAK_SendAckn();
	delay(_1_FRAME);
	connect_error = MAK_SearchAckn();
	if(connect_error) return (_ERR_SYNCHRONISING);
	return (OK);
}

//-------------------------------------------------------------------

void				MAK_SetupMultiGame(void)
{
	fn_track("mak_setupmultigame\n")
	
	MAK_SetupMultiData();
	MAK_SetupMultiCars();
}

//-------------------------------------------------------------------

long				MAK_CheckHandshake(void)
{
	long	SizeToEnd;
	long	WrapSize;
	char	handshake[9];
	char	*hptr = handshake;
	long	i;

	fn_track("mak_checkhandshake\n")

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

//	SerialDataReport(handshake, 8);
	
	// now look for HAND appearing somewhere in this array.
	for(i=0; i<8; i++, hptr++) {
		if(memcmp("HAND", hptr, 4) == 0) return 0;
	}
	return 1;
}

//-------------------------------------------------------------------

/* handshake failure message. */
void				MAK_ReportConnectFailure(long error_code)
{
	fn_track("mak_reportconnectfailure\n")

	WriteVGAString(0, 0, "connection error: ");	
	//WriteVGAString(0, 0, "Connection Failure!");
//	printf("\nConnection Failure!\n\n");
//	while(1); // temp lock on failure
	job = JOB_MULTI_TIMEOUT0;
}

//-------------------------------------------------------------------


/* Write out a SYNC packet. */
void				MAK_SendSync(void)
{
	fn_track("mak_sendsync\n")

	cwrite(CommSerialDevice, "SYNC", _SYNC_PACKET_SIZE);
}

//-------------------------------------------------------------------

/* Write out an ACKN packet. */
void				MAK_SendAckn(void)
{
	fn_track("mak_sendackn\n")

	cwrite(CommSerialDevice, "ACKN", _SYNC_PACKET_SIZE);
}

//-------------------------------------------------------------------

/* Locate SYNC packet in receive buffer. */
long				MAK_SearchSync(void)
{
	long	no_sync = 1;
	char	sync[4] =  { 0, 0, 0, 0 };
	
	fn_track("mak_searchsync\n")

	while(no_sync) {
		memset(sync, '\0', 4);
		while(sync[0] != 'S') cread(CommSerialDevice, &sync[0], 1);
		cread(CommSerialDevice, &sync[1], 3);
		if(memcmp("SYNC", &sync[0], 4) == 0) no_sync = 0;
		delay(_1_FRAME);
	}
	return (OK);
}

//-------------------------------------------------------------------

/* Locate ACKN packet in receive buffer. */
long				MAK_SearchAckn(void)
{
	long	no_sync = 1;
	char	sync[4] =  { 0, 0, 0, 0 };
	
	fn_track("mak_searchackn\n")

	while(no_sync) {
		memset(sync, '\0', 4);
		while(sync[0] != 'A') cread(CommSerialDevice, &sync[0], 1);
		cread(CommSerialDevice, &sync[1], 3);
		if(memcmp("ACKN", &sync[0], 4) == 0) no_sync = 0;
		delay(_1_FRAME);
	}	
	
	return (OK);
}

//-------------------------------------------------------------------

/* deal with the link machine'(s) pad data. */
void				MAK_ProcessLinkPads(void)
{
	fn_track("mak_processlinkpads\n")

	// 2 PLAYER
	MAK_ReadFrame();
	MAK_CopyPadBuf(1);
	MAK_ReadPadInfo(1);
}

//-------------------------------------------------------------------

/* called in main loop and broadcasts its pad data to other machines. */
void				MAK_ProcessThisPad(void)
{
	fn_track("mak_processthispad\n")

	etc_KeyboardSwitch = _DEVICE_OFF;	// software controlled interrupt mask (not hardware)
	MAK_CopyPadBuf(0);
	MAK_WriteFrame();
	MAK_ReadPadInfo(0);
	etc_KeyboardSwitch = _DEVICE_ON;
}

//-------------------------------------------------------------------

/* function to convert RAW_PAD to PAD_INFO from link machine's pad data. */
void				MAK_ReadPadInfo(long padno)
{
	RAW_PAD		*p;
	PAD_INFO	*f;
	static		long	swing[4] = { 0 ,3*2, 5*2, 7*2 };	/* 1h=0.5mm */
	short		s;
	extern	short	max_limit[4];
	
	fn_track("mak_readpadinfo\n")

	p = &transfer_buf[padno];
	f = &pad_info[padno];
/*	dump( p, 8, 1 );	*/
 	debug[2] = p->kind;
	f->status = p->status;
	f->kind   = p->kind;

	if ( p->status ) return;	/*** PAD ERROR ***/
	
	if ( p->kind == SONY_PAD ) {
		f->old_data = f->data;
		f->data = ~((p->data[0] << 8)|(p->data[1]));
		f->trig = f->data & ~f->old_data;
		
		f->h      = HANDLE_CENTER +
			(((f->data & PAD_D) != 0)-((f->data & PAD_C) !=0))
													*max_limit[negcon_limit];
		f->gas    = ((f->data & PAD_F) != 0)*LIMIT_GAS;
		f->brake  = ((f->data & PAD_G) != 0)*LIMIT_BRAKE;
		f->dummy  = ((f->data & PAD_N) != 0)*LIMIT_DUMMY;
	} else if ( p->kind == NEGCON ) {
		f->old_data = f->data;
/*		f->data = conv_negcon( ~((p->data[0] << 8)|(p->data[1])) );	*/
		f->data = ~((p->data[0] << 8)|(p->data[1]));
		
		f->h      = p->ch[0];
		f->gas    = p->ch[1] - negcon_center_gb[0];
		f->brake  = p->ch[2] - negcon_center_gb[1];
		f->dummy  = p->ch[3] - negcon_center_gb[2];
		
		if (f->gas < 0) f->gas = 0;
		if (f->brake < 0) f->brake = 0;
		if (f->dummy < 0) f->dummy = 0;
		
		if (f->h      > HANDLE_CENTER+HANDLE_LIMIT/2) f->data |= PAD_D;
		if (f->h      < HANDLE_CENTER-HANDLE_LIMIT/2) f->data |= PAD_C;
		if (f->gas    > LIMIT_GAS  /2) f->data |= PAD_F;
		if (f->brake  > LIMIT_BRAKE/2) f->data |= PAD_G;
		if (f->dummy  > LIMIT_DUMMY/2) f->data |= PAD_L;
		
		f->trig = f->data & ~f->old_data;
	} else {		/* unidentified */
		f->status = TRUE;
		f->data = 0;
		f->h      = HANDLE_CENTER;
		f->gas    = 0x00;
		f->brake  = 0x00;
		f->dummy  = 0x00;
	}
	
	/*** ハンドルのあそびを加味していきたい ***/
	/* ハンドルの 1h は 0.5ミリらしい */
	
	s = f->h - HANDLE_CENTER - negcon_center;
	if (s > 0) {
		s -= swing[ negcon_room ];
		if (s < 0) s = 0;
		if (s >  max_limit[negcon_limit]) s =  max_limit[negcon_limit];
	} else {
		s += swing[ negcon_room ];
		if (s > 0) s = 0;
		if (s < -max_limit[negcon_limit]) s = -max_limit[negcon_limit];
	}
	f->handle = s;
	
	if (f->gas   > LIMIT_GAS  ) f->gas   = LIMIT_GAS;
	if (f->brake > LIMIT_BRAKE) f->brake = LIMIT_BRAKE;
	if (f->dummy > LIMIT_DUMMY) f->dummy = LIMIT_DUMMY;
}

//-------------------------------------------------------------------

/* copy pad data from pad_buf -> transfer_buf. */
void				MAK_CopyPadBuf(long padno)
{
	fn_track("mak_copypadbuf\n")

	memcpy(&transfer_buf[padno], &pad_buf[padno], sizeof(RAW_PAD));
}

//-------------------------------------------------------------------

/* Setup of game data for multiple player mode. */
void				MAK_SetupMultiData(void)
{
	fn_track("mak_setupmultidata\n")
	
	// setup multi vars
	exit_allowed = 0;
	link_model = 0;
	CommFirstRead = 0;
	CommMultiPadFlag = TRUE;
	CommLinkError = FALSE;

	// setup car models
	cwrite(CommSerialDevice, (char *) &mycar[0].model, 1);
	cread(CommSerialDevice, (char *) &link_model, 1);
}

//-------------------------------------------------------------------

/* Setup multi-player cars and computer cars for multi-game. */
void				MAK_SetupMultiCars(void)
{
	CAR		*c;

	fn_track("mak_setupcars\n")

	multi_init_c_car();							// initialise all computer cars
	if(CommSerialMachine == _LEAD) {
		multi_init_carB(&mcar[CommMyCar], mycar[0].model);	// selected model from this machine
		multi_init_carB(&mcar[CommLinkCar], link_model);
	}
	if(CommSerialMachine == _FOLLOW) {
		multi_init_carB(&mcar[CommMyCar], mycar[0].model);	// selected model from this machine
		multi_init_carB(&mcar[CommLinkCar], link_model);
	}

#ifdef	CAR_POSITIONS_ON
	// setup master car
	c = mcar[CommMasterCar].c;
	if(reverse_flag) {
		c->pos.vx = 0x00005c98;		/* virago */
		c->pos.vy = 0xffffff11;
		c->pos.vz = 0x00003826;		/* virago */
		c->rot.vx = 0x0000001f;		/* 坂だった */
		c->rot.vy = 0x0000056a;
		c->rot.vz = 0;
		c->t = (game_level < 3) ? 0x9fb1 : 0x10fb1;
	}
	else {
		c->pos.vx = 0x00006935;
		c->pos.vy = 0x00000060;
		c->pos.vz = 0x0000a253;
		c->rot.vx = 0;
		c->rot.vy = 0x000004b1;
		c->rot.vz = 0;
		c->t = 0x4400;
	}
	c->u.m.last_t = c->t;
	c->pos.vy += 0x14;
	c->shadow_y = c->pos.vy;
	c->u.m.ranking = ALL_entry;

	// setup slave car
	c = mcar[CommSlaveCar].c;
	if(reverse_flag) {
		if( game_level < 3 ) {
			c->pos.vx = position_RS[0][0];
			c->pos.vz = position_RS[0][1];
			c->pos.vy -= position_RS[0][2];
			c->t = position_RS[0][3];
		}
		else {
			c->pos.vx = position_RL[0][0];
			c->pos.vz = position_RL[0][1];
			c->pos.vy -= position_RL[0][2];
			c->t = position_RL[0][3];
		}
	}
	else {
		if(game_level < 3) {
			c->pos.vx = position_S[0][0];
			c->pos.vz = position_S[0][1];
			c->pos.vy -= position_S[0][2];
			c->t = position_S[0][3];
		}
		else {
			c->pos.vx = position_L[0][0];
			c->pos.vz = position_L[0][1];
			c->pos.vy -= position_L[0][2];
			c->t = position_L[0][3];
		}
	}
	c->u.m.last_t = c->t;
	c->pos.vy += 0x14;
	c->shadow_y = c->pos.vy;
	c->u.m.ranking = ALL_entry - 1;
#endif
}

//-------------------------------------------------------------------

/* Initialise all multiplayer car data. */
void				MAK_InitCarData(void)
{
	int	i,j;

	fn_track("mak_initcardata\n")
	
	for(i=0; i<2; i++) MAK_ClearCar(&mycar[i]);
	for(i=0; i<C_CAR_MAX; i++) MAK_ClearCar(&c_car[i]);
	for(i=0; i<M_CAR_MAX; i++) {
		mcar[i].c = NULL;
		for (j=0; j<6+1; j++) {
			mcar[i].gear_data[j].g_rate = 0;
			mcar[i].gear_data[j].l_drpm = 0;
			mcar[i].gear_data[j].m_drpm = 0;
			mcar[i].gear_data[j].h_drpm = 0;
			mcar[i].gear_data[j].pp = 0;
		} 
		mcar[i].engine_master = 0;
		mcar[i].landing_cnt = 0;
		mcar[i].mycar_roll = 0;
		mcar[i].static_frec = 0;
		mcar[i].start_spin_dy = 0;
		mcar[i].i_cnt = 0;
		mcar[i].i_force = 0;
		mcar[i].reg_dry = 0;
		mcar[i].dpitch = 0;
		mcar[i].bpitch = 0;
		mcar[i].jpitch = 0;
		mcar[i].epitch = 0;
		mcar[i].disp_rpm = 0;
		mcar[i].accel_open = 0;
		mcar[i].engine_load = 0;
		mcar[i].decload = 0;
		mcar[i].eload = 0;
		mcar[i].sub_eload = 0;
		mcar[i].hr = 0;
		mcar[i].swing_event = 0;
		mcar[i].swing_cnt = 0;
		mcar[i].swing_width = 0;
		mcar[i].rebound_event = 0;
		mcar[i].rebound_cnt = 0;
		mcar[i].rebound_v.vx = 0;
		mcar[i].rebound_v.vy = 0;
		mcar[i].rebound_v.vz = 0;
		mcar[i].rebound_v.pad = 0;
		mcar[i].game_stat = 0;
		mcar[i].goal_cnt = 0;
		mcar[i].goal_lap = 0;		
		mcar[i].total_time = 0;
		mcar[i].total_1000_time = 0;
		mcar[i].limit_time = 0;
		mcar[i].cp_cnt = 0;
		mcar[i].wrong_way_cnt = 0;
		mcar[i].tunnel_rev = 0;
		mcar[i].pad = 0;
		mcar[i].spin_r = 0;
		mcar[i].spin_way = 0;
		mcar[i].flow_ry = 0;
		mcar[i].flow_speed = 0;
		mcar[i].flow_dry = 0;
		mcar[i].course_table = NULL;
		mcar[i].road_size = 0;
		mcar[i].course_flag = 0;
		for(j=0; j<4; j++) {
			mcar[i].car_tire_pos[j].vx = 0;
			mcar[i].car_tire_pos[j].vy = 0;
			mcar[i].car_tire_pos[j].vz = 0;
			mcar[i].car_tire_pos[j].pad = 0;
		}
		mcar[i].hit_by = NULL;
	}	
}

//---------------------------------------------------------------------

/* Initialise a car data structure. */
void				MAK_ClearCar(CAR *c)
{
	int	i;
	
	fn_track("mak_clearcar\n")

	c->use = 0;
	c->model = 0;
	c->flag = 0;
	c->t = 0;
	c->s = 0;
	c->pos.vx = 0;
	c->pos.vy = 0;
	c->pos.vz = 0;
	c->pos.pad = 0;
	c->rot.vx = 0;
	c->rot.vy = 0;
	c->rot.vz = 0;
	c->rot.pad = 0;
	c->tire_rot.vx = 0;
	c->tire_rot.vy = 0;
	c->tire_rot.vz = 0;
	c->tire_rot.pad = 0;
	c->shadow_y = 0;
	c->hit = 0;
	c->hvec.vx = 0;
	c->hvec.vy = 0;
	c->hvec.vz = 0;
	c->hvec.pad = 0;
	
	c->u.m.dt = 0;
	c->u.m.ds = 0;
	c->u.m.dpos.vx = 0;
	c->u.m.dpos.vy = 0;
	c->u.m.dpos.vz = 0;
	c->u.m.dpos.pad = 0;
	c->u.m.drot.vx = 0;
	c->u.m.drot.vy = 0;
	c->u.m.drot.vz = 0;
	c->u.m.drot.pad = 0;
	c->u.m.drive_mode = 0;
	c->u.m.shift = 0;
	c->u.m.rpm = 0;
	c->u.m.shift_load = 0;
	c->u.m.old_tire_rpm = 0;
	c->u.m.slip_mode = 0;
	c->u.m.slip_level = 0;
	c->u.m.handle = 0;
	c->u.m.max_speed = 0;
	c->u.m.speed = 0;
	c->u.m.a = 0;
	c->u.m.v_ry = 0;
	c->u.m.tire_dir = 0;
	c->u.m.tire_rpm = 0;
	c->u.m.tire_mode = 0;
	c->u.m.jump_flag = 0;
	c->u.m.jump_cnt = 0;
	c->u.m.jump_vy = 0;
	c->u.m.jump_pitch = 0;
	c->u.m.gas_trig = 0;
	c->u.m.brk_trig = 0;
	c->u.m.gas = 0;
	c->u.m.brk = 0;
	c->u.m.ranking = 0;
	c->u.m.make_best_lap = 0;
	c->u.m.last_t = 0;
	c->u.m.lap = 0;
	c->u.m.unlap = 0;
	c->u.m.uncp1 = 0;
	c->u.m.uncp2 = 0;
	for(i=0; i<5; i++) c->u.m.lap_time[i] = 0;
	for(i=0; i<5; i++) c->u.m.disp_lap_time[i] = 0;
	
	c->u.c.status = 0;
	c->u.c.dt = 0;
	c->u.c.ds = 0;
	c->u.c.dpos.vx = 0;
	c->u.c.dpos.vy = 0;
	c->u.c.dpos.vz = 0;
	c->u.c.dpos.pad = 0;
	c->u.c.drot.vx = 0;
	c->u.c.drot.vy = 0;
	c->u.c.drot.vz = 0;
	c->u.c.drot.pad = 0;
	c->u.c.last_t = 0;
	c->u.c.speed = 0;
	c->u.c.handle = 0;
	c->u.c.flg.f_drift = 0;
	c->u.c.flg.f_hit_lock = 0;
	c->u.c.flg.f_direction = 0;
	c->u.c.flg.f_front_car = 0;
	c->u.c.flg.f_back_car = 0;
	c->u.c.flg.f_pass1 = 0;
	c->u.c.flg.f_lap = 0;
	c->u.c.flg.f_mycar_pos = 0;
	c->u.c.flg.f_passd_mycar = 0;
	c->u.c.flg.f_condition = 0;
	c->u.c.flg.f_chase_mycar = 0;
	c->u.c.flg.f_curve_lock = 0;
	c->u.c.flg.f_jump = 0;
	c->u.c.flg.f_straight = 0;
	c->u.c.flg.f_side_level = 0;
	c->u.c.flg.f_satan_run = 0;
	c->u.c.target_s = 0;
	c->u.c.target_spd = 0;
	c->u.c.final_spd = 0;
	c->u.c.hit_drot = 0;
	c->u.c.hit_ds = 0;
	c->u.c.std_dir = 0;
	for(i=0; i<4; i++) c->u.c.eye_dir[i] = 0;
	c->u.c.c_car_dist = 0;
	c->u.c.frnt_dist = 0;
	c->u.c.mycar_dist = 0;
	c->u.c.jump_cnt = 0;
	c->u.c.sw_cnt = 0;
	c->u.c.sw_width = 0;
	c->u.c.jump_vy = 0;
	c->u.c.jump_pitch = 0;
	c->u.c.machine_type = 0;
	c->u.c.max_speed = 0;
	c->u.c.paccel = 0;
	c->u.c.maccel = 0;
	c->u.c.dacl = 0;
	c->u.c.start_acl = 0;
	c->u.c.chase_spd = 0;
	c->u.c.away_spd = 0;
}

//-------------------------------------------------------------------

/* Initialise all communications data. */
void				MAK_InitCommData(void)
{
	int	i;
	
	fn_track("mak_initcommdata\n")

	for(i=0; i<M_CAR_MAX; i++) {
		pad_buf[i].status = 0;
		pad_buf[i].kind = 0;
		pad_buf[i].data[0] = 0;
		pad_buf[i].data[1] = 0;
		pad_buf[i].ch[0] = 0;
		pad_buf[i].ch[1] = 0;
		pad_buf[i].ch[2] = 0;
		pad_buf[i].ch[3] = 0;
		transfer_buf[i].status = 0;
		transfer_buf[i].kind = 0;
		transfer_buf[i].data[0] = 0;
		transfer_buf[i].data[1] = 0;
		transfer_buf[i].ch[0] = 0;
		transfer_buf[i].ch[1] = 0;
		transfer_buf[i].ch[2] = 0;
		transfer_buf[i].ch[3] = 0;
		pad_info[i].status = 0;
		pad_info[i].kind = 0;
		pad_info[i].data = 0;
		pad_info[i].old_data = 0;
		pad_info[i].trig = 0;
		pad_info[i].h = 0;
		pad_info[i].gas = 0;
		pad_info[i].brake = 0;
		pad_info[i].dummy = 0;
		pad_info[i].s_down = 0;
		pad_info[i].s_up = 0;
		pad_info[i].handle = 0;
	}
	
	/* Default multiplay setup. */
	CommMultiPlaySelect = _SINGLE_PLAY;
	CommMultiPadFlag = FALSE;
	CommNumLinkedCars = 2;
	CommNetPlayers = 2;
	CommMyCar = 0;
	CommLinkCar = 1;
	CommMasterCar = 0;
	CommSlaveCar = 1;
	CommSerialConnectMethod = _DIALUP;
	CommSerialMachine = _LEAD;
	CommSerialDevice = _COM1;
	CommSerialBaudRate = _14400_BAUD;
	CommSerialDataSize = _8BIT_DATA;
	CommSerialParity = _NO_PARITY;
	CommSerialStopSize = _1BIT_STOP;
	strcpy(CommModemInitString, _MODEM_INIT_STRING0);
	strcpy(CommModemDialupString, _MODEM_DIAL_STRING0);
	CommFirstRead = 0;
	CommRandomSeed = 1;
	CommMultiPause = 0;
}

//-------------------------------------------------------------------

void				SerialDataReport(char *data, long n)
{
	char	num;
	char	string[64];
	int		i;
	
	fn_file = fopen("FNTRACK", "at");
	for(i=0; i<n; i++) {
		num = *(data++);
		fprintf(fn_file, "%c", num);
	}
	
	fprintf(fn_file, "\n");
	fclose(fn_file);
}

//-------------------------------------------------------------------

