//---------------------------------------------------------------------
// New multiple player routines
// Now works with new Ridge Racer code (17/08/95)
//---------------------------------------------------------------------

#include	"..\origsrc\common.h"
#define		VAR_EXTERN
#include	"..\origsrc\var.h"
#include	"..\origsrc\handle.h"
#include	"..\origsrc\sobj.h"
#include	"..\origsrc\bgm.h"
#include	"..\origsrc\drive.h"

#define		_SIZE_T_DEFINED_
#include	"c:\watcom\h\stdio.h"
#include	"c:\watcom\h\stdlib.h"
#include	"c:\watcom\h\string.h"

#include	"..\markk\multi.h"
#include	"..\markk\comm.h"
#include	"..\markk\newccar.h"
#include	"..\markk\newheli.h"
#include	"..\markk\grtext.h"
#include	"..\markk\grinfo.h"

//---------------------------------------------------------------------

void	job_multi_game0(void)
{
	int		i;
	int		j;
	
	fn_track("job_multi_game0\n")


	/**********************************************/
	/* Initialise connection to other machine(s). */
	MAK_OpenLink();
	/**********************************************/

	
	game_count = 0;
	SetDispMask(FALSE);

	if(job == JOB_MULTI0)  {
		cd_seek(1 + now_bgm);
	}
	
	init_hds();
	if(job == JOB_MULTI0) {
		set_hds();
		/* 固定値設定 */
		hds.check_tone.p = 60 << 7;
		hds.kansei_l.p  = 60 << 7;
		hds.kansei_r.p  = 60 << 7;
	}
	se_reverb_type(4, 0, 0);

	standard_init(0, 0, 0);

	/* Setup Course Table. */
	set_each_course((game_level < 3), reverse_flag);
	zone_type = 0;
	if(reverse_flag) {
		if (game_level < 3)	{
			course_table = easy_course_table;
			road_size = EASY_COURSE_DIV;
		}
		else {
			course_table = tech_course_table;
			road_size = TECH_COURSE_DIV;
		}
	}
	else {
		course_table = start_course_table;
		road_size = EASY_COURSE_DIV;
	}

	set_light();
	camera_num = 1;
	camera_trg = &mycar[0];

	if(game_level == 0)  {
		dq.flag = dq.sf = FALSE;
	}
	else  {
		init_dq(game_level-1);
	}
	
	akumacar_flag = ((reverse & 0x0f) == 0x0f);

// MOVE TO SETUPMULTIGAME
// MOVE TO SETUPMULTIGAME
// MOVE TO SETUPMULTIGAME

	/* Setup Car structs. (SERIAL) */
//	multi_init_c_car();
	if(CommSerialMachine == _LEAD) {
		CommMyCar = 0;
		CommLinkCar = 1;
//		multi_init_carB(&mcar[CommMyCar], 5);
//		multi_init_carB(&mcar[CommLinkCar], 3);
	}
	else {
		CommMyCar = 1;
		CommLinkCar = 0;
//		multi_init_carB(&mcar[CommMyCar], 3);
//		multi_init_carB(&mcar[CommLinkCar], 5);
	}
	CommMasterCar = 0;
	mcar[CommMyCar].c = (CAR *) &mycar[0];
	mycar[0].pnt_mc = (MULTI_CAR *) &mcar[CommMyCar];
	mcar[CommLinkCar].c = (CAR *) &c_car[0];
	c_car[0].pnt_mc = (MULTI_CAR *) &mcar[CommLinkCar];
#ifdef	CAR_COLLISIONS_ON
	// setup all mcar structure pointers
	for(i=CommNumLinkedCars; i<M_CAR_MAX; i++) {
		mcar[i].c = (CAR *) &c_car[i-1];
		c_car[i-1].pnt_mc = (MULTI_CAR *) &mcar[i];
	}
#endif
// MOVE TO SETUPMULTIGAME
// MOVE TO SETUPMULTIGAME
// MOVE TO SETUPMULTIGAME

	//init_c_car();
	//init_car(&mycar[0]);
	
	if(game_level == 4)  {
		init_heli_start();
	}
	else  {
		init_heli();	// helicopter
	}
	init_jet();		// airplane
	init_ocar();	// red lorry, yellow lorry
	init_kick();	// road block objects

	if(job != JOB_CDPLAY0) {
		init_replay();	/* cdplay で消えるのをパッチ */
	}

	game_stat = 0;
	set_swap(mycar[0].t);

	job_cnt = 0;
	total_time = 0;
	tunnel_rev = FALSE;
	voice_trig = 0x0000;
	
	goal_lap = 3;
	if ( reverse_flag ) {
		limit_time = extend_time[game_level-1].extra_start*30;
	}
	else {
		limit_time = extend_time[game_level-1].start*30;
	}

	cp_cnt = 0;
	
	if(game_level == 1) goal_lap--;
	if(goal_lap <= 0) goal_lap = 1;		/* patch (^^; */

	mirror = FALSE;
	init_addobj(OTZS);

	pause_on = FALSE;
//	wait_count = 256+128;
	job = JOB_MULTI;

// MOVE TO SETUPMULTIGAME
// MOVE TO SETUPMULTIGAME
// MOVE TO SETUPMULTIGAME
	/* Initialise New Cars for multi-player mode. */
	for(i=0; i<M_CAR_MAX; i++) {
		if(reverse_flag) {
			if (game_level < 3)	{
				mcar[i].course_table = easy_course_table;
				mcar[i].road_size = EASY_COURSE_DIV;
			}
			else {
				mcar[i].course_table = tech_course_table;
				mcar[i].road_size = TECH_COURSE_DIV;
			}
		}
		else {
			mcar[i].course_table = start_course_table;
			mcar[i].road_size = EASY_COURSE_DIV;
		}
		mcar[i].game_stat = 0;
		mcar[i].total_time = 0;
		mcar[i].wrong_way_cnt = 0;
		mcar[i].tunnel_rev = FALSE;
		mcar[i].goal_lap = 3;
		if(reverse_flag) mcar[i].limit_time = extend_time[game_level-1].extra_start*30;
		else mcar[i].limit_time = extend_time[game_level-1].start*30;
		mcar[i].cp_cnt = 0;
		if(game_level == 1) mcar[i].goal_lap--;
		if(goal_lap <= 0) mcar[i].goal_lap = 1;		/* patch (^^; */
		mcar[i].hit_by = NULL;
		mcar[i].near_car = NULL;
		mcar[i].near_dist = 0x0FFFFFFF;
	}
	// Serial Pad Link init
	mcar[CommMyCar].pad = 0;
	mcar[CommLinkCar].pad = 1;	

	/**********************************************/
	/* Initialise multiplayer system. 			  */
	MAK_SetupMultiGame();
	/**********************************************/
	//	MAKsetup_multiplay();
	
	camera_mtrg = &mcar[CommMyCar];
	pause_machine = -1;

// MOVE TO SETUPMULTIGAME
// MOVE TO SETUPMULTIGAME
// MOVE TO SETUPMULTIGAME
}

//---------------------------------------------------------------------

void	job_multi_game(void)
{
	CAR				*dc = &mycar[0];	/* now driving car */
	MULTI_CAR		*mc;
	MULTI_CAR		*m;
	CAR				*c;
	long			i,t,w;
	static u_long	primlist;
	long			pause_id;
		
	fn_track("job_multi_game\n")

	MAK_ProcessLinkPads();
		
	if(job_cnt++ == 2) SetDispMask(TRUE);
	action_hds();

	pause_id = multi_check_pause();

	/* Camera Selection. */
	if(pad_info[0].trig & PAD_O) camera_num--;
	if(pad_info[0].trig & PAD_M) camera_num++;
	if(camera_num < 0) camera_num = 6;
	if(camera_num > 6) camera_num = 0;
	if(multi_select_trig(0))  {
		camera_trg = (camera_trg == dc) ? &c_car[0] : dc;	// change camera view (U or ME)
		camera_mtrg = (camera_mtrg == &mcar[CommMyCar]) ? &mcar[CommLinkCar] : &mcar[CommMyCar];	// change camera view (U or ME)
	}

	if(mcar[CommMyCar].game_stat >= 2) {
		if(pad_info[0].trig & key_mask[pad_info[0].kind == NEGCON][6]) {
			camera_num = (camera_num == 1) ? 3 : 1;
		}
	}

	if(pause_on) {
		if(pause_id != -1) pause_machine = pause_id;
		multi_pause_menu(pause_machine);
		return;
	}
	
	g_cnt++;	/* ゲームカウンタ */

	//----------------------------------------------------------------------------------------//
	
	/* Process all multi-player cars in this loop. */
	for(i=0; i<CommNumLinkedCars; i++)
	{
		mc = &mcar[i];
		if(mcar[i].cp_cnt) mcar[i].cp_cnt--;
		switch (mcar[i].game_stat) {
			case 0:		/* Zoom into car start. */
						if(job_cnt >= 2*30+1*30) mcar[i].game_stat = 1;
						if(i==CommMyCar) multi_set_camera_0(camera_mtrg, job_cnt);
						break;

			case 1:		/* Count Down. */
						if(job_cnt > 7*30) {
							multi_start_car(&mcar[i]);
							mcar[i].game_stat = 2;
							if(i==CommMyCar) normal_cd_play();
						}
						multi_horizon(&mcar[i], mcar[i].pad); // my car
						if(i==CommMyCar) {
							multi_set_camera(1, camera_mtrg);
							count_down_se(job_cnt);
							disp_signal(job_cnt);
						}
						break;

			case 2:		/* Start Strip. */
						if(job_cnt == 10*30) {
							if((dc->t < 0x4300) && (i==CommMyCar)) se_play(VOICE01); // wow, what a start
							zone_type = (game_level < 3) ? 1 : 2;
						}
						mcar[i].limit_time--;
						multi_horizon(&mcar[i], mcar[i].pad);	// my car
						if(i==CommMyCar) {
							multi_set_camera(camera_num, camera_mtrg);
							disp_signal(job_cnt);
						}
						if(reverse_flag) {
							if(game_level < 3) mcar[i].course_flag = (multi_crsl(mcar[i].c->t) == 0x0b000);
							else mcar[i].course_flag = (multi_crsl(mcar[i].c->t) == 0x12200);
							if(job_cnt > 20*30) mcar[i].course_flag = TRUE;
						}
						else mcar[i].course_flag = (mcar[i].c->t & 0xff00) == 0x3500;

						// set main course when reached end of start zone
						if(mcar[i].course_flag) {
							mcar[i].game_stat = 3;
							if(game_level < 3) mcar[i].course_table = easy_course_table;
							else {
								mcar[i].course_table = tech_course_table;
								mcar[i].road_size = TECH_COURSE_DIV;
							}
						}
						break;

			case 3:		/* RUN (COURSE MODE) */
						mcar[i].limit_time--;
						if(mcar[i].c->u.m.lap > 0) {
							long	l = mcar[i].c->u.m.lap-1;

							mcar[i].c->u.m.lap_time[l]++;
							mcar[i].c->u.m.disp_lap_time[l] = get_1000_time(mcar[i].c->u.m.lap_time[l], 0);
							mcar[i].total_time++;
						}
						multi_run(&mcar[i], mcar[i].pad); // my car
						break;

			case 4:		/* GOAL!!!! */
						if((mcar[i].goal_cnt & 1) && (mcar[i].engine_master > 0)) mcar[i].engine_master-=2;
						if((mcar[i].goal_cnt == GOAL_TIME-30*2) && (i==CommMyCar)) {
							if(ending_flag) cd_seek(BGM_WINNING_E);	/* ending */
							else cd_seek(BGM_WINNING);
						}
						if(mcar[i].goal_cnt-- < 0) {
							if((mcar[i].c->u.m.ranking == 1) && (!ending_flag) && (i==CommMyCar)) se_play(VOICE04);
							job_winning0();
							game_exit(JOB_WINNING);	/* !!!??? */
							normal_cd_play();
						}
						multi_horizon(&mcar[i], mcar[i].pad); // my car
						multi_set_camera(camera_num, camera_mtrg);
						break;

			case 5:		/* TIME UP */
			case 6:		/* TIME UP in START */
#ifdef					NO_TIME_OVER
						break;
#endif
						if(mcar[i].goal_cnt-- < 0) {
							replay_enable = FALSE;
							//wait_count = 128;
							title_cnt = 0;
							game_exit(JOB_OVER0);
						}
						else if(mcar[i].goal_cnt == TIME_UP_TIME-(3*30-15)) {
							se_all_stop();
						/*	normal_cd_play();	*/
							cd_depause();
							cd_set_fade(127);
						}
						multi_horizon(&mcar[i], mcar[i].pad); // my car
						multi_set_camera_time_up(camera_trg, mcar[i].goal_cnt, &mcar[i]);
						break;

			default:	break;
		}
		
		if((mcar[i].game_stat < 5) && (mcar[i].limit_time <= -30*2)) {
			if(i==CommMyCar) cd_seek(BGM_TIMEUP);
			shutter_y = -64*256;
			shutter_v = 0;
			shutter_wait = 7*30-15;
			mcar[i].game_stat = (mcar[i].game_stat == 2) ? 6 : 5;
			mcar[i].goal_cnt = TIME_UP_TIME;
			init_time_up_camera();
		}
	// end for loop - for each multi car
	}

	//----------------------------------------------------------------------------------------//

	game_stat = mcar[CommMyCar].game_stat;		// new entry (MARK) //
	
	/*** 歓声関係 ***/
	multi_action_kansei(&mcar[CommMyCar]);
	
	/*** 逆走関係 ***/
	w = multi_wrong_way(&mcar[CommMyCar]) && (camera_trg == dc) && (dc->u.m.tire_mode != RUN_SPIN);
	if(w && (mcar[CommMyCar].game_stat < 4)) disp_wrong_way((camera_num == 3) ? -3*8 : 0);
	if(w && (mcar[CommMyCar].game_stat < 4) && (dc->u.m.speed > 50*8)) {
		if (++mcar[CommMyCar].wrong_way_cnt == 3*30) {
			se_play(VOICE18);
			mcar[CommMyCar].wrong_way_cnt = -8*30;
		}
	}
	else mcar[CommMyCar].wrong_way_cnt = 0;
	
	/*** Ｈｕｒｒｙ！！（急げ！） ***/
	if((mcar[CommMyCar].game_stat != 4) && (mcar[CommMyCar].limit_time == 4*30)) se_play(VOICE17);
	if(mcar[CommMyCar].game_stat == 4) {
		t = 256-((mcar[CommMyCar].goal_cnt+30)*256/(GOAL_TIME-30));
		if(t < 0) t = 0;
		disp_fade(t, good_goal_flag ? 1 : 2);
	}
	else if (mcar[CommMyCar].game_stat >= 5) disp_time_up(mcar[CommMyCar].goal_cnt - TIME_UP_TIME + 32);
	else multi_disp_indicator();
	multi_action_engine();
	meter(0, 0, 0);
	set_disp();

	SetRotMatrix(&rot_trans);
	cs->s = (char *)add_far  (cs->s, &cs->apc_head[0], &disp_rrm[0], 64);//AJY
	cs->s = (char *)add_divdq(cs->s, &cs->apc_head[0], &disp_rrm[0], 64);//AJY
	cs->s = (char *)add_divdr(cs->s, &cs->apc_head[0], &disp_rrm[0], 64);//AJY
	SetFarColor(dq.c.r, dq.c.g, dq.c.b);		/* patch (^^; */

	meter(0x00, 0xff, 0xff);
	multi_add_others();
	add_obj_table   (&share_tbl   [0]);			/* 昼夜共用 */
	add_obj_table_dq(&share_tbl_dq[0]);			/* 昼夜共用 */
	if(dq.sf) {
		bldg_add_obj_table(&bldg_night_tbl[0]);		/* 夜 */
		add_obj_table(&night_tbl[0]);		/* 夜 */
	}
	else {
		bldg_add_obj_table_dq(&bldg_day_tbl[0]);		/* 昼 */
		add_obj_table_dq(&day_tbl[0]);		/* 昼 */
	}
	meter(0x00, 0x80, 0x00);	/* dark green */
	multi_move_c_car();
	meter(0, 0, 255);

	// add multi cars not being driven
	m = &mcar[0];
	for(i=0; i<CommNumLinkedCars; i++, m++) {
		c = m->c;
		if((c->use) && (camera_mtrg != m)) add_car(c, FALSE);
	}	

	// add computer cars to OT table
	m = &mcar[CommNumLinkedCars];
	for(i=CommNumLinkedCars; i<M_CAR_MAX; i++, m++) {
		c = m->c;
		if((c->use) && (c->u.c.status == 1)) add_car(c, FALSE); // add computer controlled cars
	}

	meter(255, 255, 0);
	multi_move_heli();
	multi_move_jet();
	move_ocar();	
	for (i=0; i<CommNumLinkedCars; i++)	multi_move_kick(&mcar[i]);
	disp_heli();	
	disp_jet();		
	disp_ocar();	
	disp_kick();
	multi_action_dq();
	add_sky();
	meter(255, 0, 255);
	rec();

	for (i=0; i<CommNumLinkedCars; i++) multi_set_rev(&mcar[i]);
	multi_action_sound(&mcar[CommMyCar]);

	// End of Game drop head display
	if(mcar[CommMyCar].game_stat >= 5) {
		long	i,t;
		
		if(shutter_wait) shutter_wait--;
		else {
			shutter_y += shutter_v += 52;
			t = shutter_y/256;
		}
		if(t < 0) t = 0;
		if(t > 120) {
			t = 120;
			shutter_y = 120*256;
			shutter_v = -shutter_v/5;
		}
		cs->s = (char *)set_draw_env(now_db->ot, (DR_ENV *)cs->s, t, 240-t*2);
		db[0].draw.isbg = db[1].draw.isbg = TRUE;
	}
	else if(mcar[CommMyCar].game_stat == 0) {
		long	t = 128 - job_cnt*4;

		if(t > 120) t = 120;
		if(t < 0) t = 0;
		cs->s = (char *)set_draw_env(now_db->ot, (DR_ENV *)cs->s, t, 240-t*2);
		db[0].draw.isbg = db[1].draw.isbg = TRUE;
	}
	else db[0].draw.isbg = db[1].draw.isbg = FALSE;		/*** 消去しない ***/
	meter(0, 255, 0);

	/* Debugging Analysis. */
#ifdef	FILE_DATA_ANALYSIS	
	OutputBinaryCarData();
#endif
}

//---------------------------------------------------------------------
	
void	multi_init_car(MULTI_CAR *mc)
{
	CAR			*c = mc->c;
	MYCAR_BUF	*m = &c->u.m;
	long		i,max;
	GEAR_DATA	*g;
	CAR_ABILITY	*a = &car_ability[c->model];
	static long	max_speed[2][4] = {	{   145,   182,   182, 200 },		/* オートマ */
								{ 145+4, 182+4, 182+4, 200+4 } };		/* マニュアル */
	static long	h_drpm[] = {    0,  80,  80,  55,  40,  30,  22 };
	static long	m_drpm[] = { 4000, 800, 300,  90,  80,  70,  50 };
	static long	l_drpm[] = { 4000, 600, 100,  50,  20,  10,   4 };

	fn_track("multi_init_car\n")
		
	c->use = TRUE;
	c->flag = TYPE_MYCAR;
	
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
	
	m->last_t = c->t;
	c->pos.vy += 0x14;	/* 視点の高さ */
	c->shadow_y = c->pos.vy;
	
	m->dpos.vx = m->dpos.vy = m->dpos.vz = 0;
	m->drot.vx = m->drot.vy = m->drot.vz = 0;
	
	m->speed = 0;
	m->lap = m->unlap = 0;
	m->uncp1 = m->uncp2 = 0;
	m->make_best_lap = -1;
	
	for(i=0; i<5; i++) m->lap_time[i] = m->disp_lap_time[i] = 0;
	
	mc->i_cnt = 0;
	mc->i_force = 0;
	mc->dpitch = 0;
	mc->bpitch = 0;
	mc->jpitch = 0;
	mc->epitch = 0;
	
	m->tire_mode = RUN_SLIP;
	m->shift = 1;
	m->v_ry = m->tire_dir = c->rot.vy;
	m->rpm = 0;
	m->tire_rpm = m->old_tire_rpm = 0;
	m->shift_load = 0;
//	c->rot.vy * 256;
	
	max = 8 * max_speed[m->drive_mode][game_level-1] * a->speed / 1000;
	m->max_speed = max;
	
	g = &mc->gear_data[0];
	for(i=0; i<6+1; i++,g++) {
		g->g_rate = 1 + max*i/6;
		g->l_drpm = l_drpm[i] * a->accel /100;
		g->m_drpm = m_drpm[i] * a->accel /100;
		if(i==6) g->h_drpm = h_drpm[i] * 10;
		else g->h_drpm = h_drpm[i] * a->accel /100;
		g->pp = 6500 + (i-1)*320;
	}
	m->slip_level = SPIN_I_FORCE * a->slip / 1000;
	m->handle = a->handle;
	
//	init_effect();
	mc->swing_event = 0;
	mc->rebound_event = 0;
	
	m->jump_flag = FALSE;
	m->gas_trig = m->brk_trig = 0;
	mc->mycar_roll = 0;
	mc->disp_rpm = 0;
	mc->accel_open = 0;
	mc->static_frec = 0;
	mc->decload = 0;
	mc->engine_master = ENGINE_MASTER;
	mc->landing_cnt = 0;
}

//---------------------------------------------------------------------

void	multi_init_carB(MULTI_CAR *mc, long model)
{
	CAR			*c = mc->c;
	MYCAR_BUF	*m = &c->u.m;
	long		i,max;
	GEAR_DATA	*g;
	CAR_ABILITY	*a = &car_ability[model];
	static long	max_speed[2][4] = {	{   145,   182,   182, 200 },		/* オートマ */
								{ 145+4, 182+4, 182+4, 200+4 } };		/* マニュアル */
	static long	h_drpm[] = {    0,  80,  80,  55,  40,  30,  22 };
	static long	m_drpm[] = { 4000, 800, 300,  90,  80,  70,  50 };
	static long	l_drpm[] = { 4000, 600, 100,  50,  20,  10,   4 };

	fn_track("multi_init_carb\n")
		
	c->model = model;
	c->use = TRUE;
	c->flag = TYPE_MYCAR;
	
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
	
	m->last_t = c->t;
	c->pos.vy += 0x14;	/* 視点の高さ */
	c->shadow_y = c->pos.vy;
	
	m->dpos.vx = m->dpos.vy = m->dpos.vz = 0;
	m->drot.vx = m->drot.vy = m->drot.vz = 0;
	
	m->speed = 0;
	m->lap = m->unlap = 0;
	m->uncp1 = m->uncp2 = 0;
	m->make_best_lap = -1;
	
	for(i=0; i<5; i++) m->lap_time[i] = m->disp_lap_time[i] = 0;
	
	mc->i_cnt = 0;
	mc->i_force = 0;
	mc->dpitch = 0;
	mc->bpitch = 0;
	mc->jpitch = 0;
	mc->epitch = 0;
	
	m->tire_mode = RUN_SLIP;
	m->shift = 1;
	m->v_ry = m->tire_dir = c->rot.vy;
	m->rpm = 0;
	m->tire_rpm = m->old_tire_rpm = 0;
	m->shift_load = 0;
//	c->rot.vy * 256;
	
	max = 8 * max_speed[m->drive_mode][game_level-1] * a->speed / 1000;
	m->max_speed = max;
	
	g = &mc->gear_data[0];
	for(i=0; i<6+1; i++,g++) {
		g->g_rate = 1 + max*i/6;
		g->l_drpm = l_drpm[i] * a->accel /100;
		g->m_drpm = m_drpm[i] * a->accel /100;
		if(i==6) g->h_drpm = h_drpm[i] * 10;
		else g->h_drpm = h_drpm[i] * a->accel /100;
		g->pp = 6500 + (i-1)*320;
	}
	m->slip_level = SPIN_I_FORCE * a->slip / 1000;
	m->handle = a->handle;
	
//	init_effect();
	mc->swing_event = 0;
	mc->rebound_event = 0;
	
	m->jump_flag = FALSE;
	m->gas_trig = m->brk_trig = 0;
	mc->mycar_roll = 0;
	mc->disp_rpm = 0;
	mc->accel_open = 0;
	mc->static_frec = 0;
	mc->decload = 0;
	mc->engine_master = ENGINE_MASTER;
	mc->landing_cnt = 0;
}




//---------------------------------------------------------------------

void	multi_run(MULTI_CAR *mc, long padno)
{
	long	l;
	CAR		*c = mc->c;
	
	fn_track("multi_run\n")

	multi_horizon(mc, padno);
	multi_set_camera(camera_num, camera_mtrg);
	multi_check_lap(mc);
	if (c->u.m.lap > mc->goal_lap) {	/* FINISH? */
		long	i,t;
		
		for (t=i=0; i<mc->goal_lap; i++) t += c->u.m.disp_lap_time[i];
		mc->total_1000_time = t;
		if (best_total[mirror][reverse_flag][game_level-1] > mc->total_1000_time) {
			best_total[mirror][reverse_flag][game_level-1] = mc->total_1000_time;
		}
		if(padno == 0) total_1000_time = mcar[CommMyCar].total_1000_time;
		
		good_goal_flag = FALSE;
		ending_flag = FALSE;
		if(c->u.m.ranking == 1) {
			if((reverse & 0x0f) == 0x0f) {		/* 前半クリア */
				if (game_level == 4) akumawin_flag = TRUE;
			}

			l = game_level-1 + (reverse_flag == TRUE)*4;
			if(!(reverse & (1 << l))) {
				reverse |= (1 << l);
				good_goal_flag = TRUE;
				if ((reverse & 0xff) == 0xff) ending_flag = 2;
				else if ((reverse & 0xff) == 0x0f) ending_flag = 1;
				else ending_flag = 0;
			}
		}
		mc->game_stat = 4;
		mc->goal_cnt = GOAL_TIME;
		cd_set_fade(-2);
	}

	if(!reverse_flag) {
		multi_check_cp1(mc);
		if(game_level >= 3) multi_check_cp2(mc);
	}
	
	multi_action_voice(mc);
	c->u.m.last_t = c->t;
}

//---------------------------------------------------------------------

void	multi_horizon(MULTI_CAR *mc, long padno)
{
	CAR			*c = mc->c;
	VECTOR		b;
	VECTOR		npos;
	VECTOR		nrot;
	long		hit;
	long		car_hit;
	long		new_t;
	long		new_s;
	MYCAR_BUF	*m = &c->u.m;
	long		road_height;
	long		dt;
	long		i;
	long		sp;
	long		cry;
	long		start_back_hit = FALSE;
	long		pad = (pad_info[padno].kind == NEGCON);
	
	fn_track("multi_horizon\n")

	if(mc->game_stat >= 2) {
		if(m->drive_mode) {		/* マニュアル */
			if(pad_info[padno].trig & key_mask[pad][KEY_SDOWN]) {
				if(m->shift < 6) m->shift++;
			}
			if(pad_info[padno].trig & key_mask[pad][KEY_SUP]) {
				if(m->shift > 1) m->shift--;
			}
		}
		else {		/* オートマ */
			if((mc->game_stat != 1) && (m->tire_mode != RUN_SLIP)) {
				if(m->rpm < SHIFT_DOWN_RPM-(6-m->shift)*200) {
					if(m->shift > 1) m->shift--;
				}
				else if(m->rpm > SHIFT_UP_RPM) {
					if(m->shift < 6) m->shift++;
				}
			}
		}
	}
	
	multi_handle(mc, padno);
	if(mc->game_stat < 5) {
		if((m->speed < 32*8)&&(m->tire_mode == RUN_GRIP)) {
			m->tire_dir += ((m->drot.vy)*mc->hr/256)*m->speed/(32*8)/256;
		}
		else if((m->speed < 64*8)&&(m->tire_mode == RUN_SLIP)) {
			m->tire_dir += ((m->drot.vy)*mc->hr/256)*m->speed/(64*8)/256;
		}
		else {
			m->tire_dir += (m->drot.vy)*mc->hr/256/256;
		}
	}

	multi_engine(mc, padno);
	if(mc->game_stat == 1) m->speed = 0;
	dt = m->speed*3;
	if (dt > ONE) dt = ONE/7;
	c->tire_rot.vz += dt;
	c->tire_rot.vz &= 0x0fff;
	if (m->speed > 100*8) c->tire_rot.vz |= ONE;
	npos.vx = c->pos.vx + m->dpos.vx/256;
	npos.vz = c->pos.vz + m->dpos.vz/256;
	nrot = c->rot;
	multi_xz2ts(&npos, &nrot, m->last_t, &new_t, &new_s, mc);
	hit = multi_in_road(npos.vx, npos.vz, 0x0800 - c->rot.vy, new_t, mc);
	if(hit) multi_set_swing(EFFECT_HIT, mc);
	road_height = npos.vy - 8;
	if(m->jump_flag) {
		m->jump_cnt++;
		npos.vy = c->pos.vy + m->jump_vy/8;
		m->jump_vy += 12;				/* 重力加速度 */
		if((m->jump_vy > 0) && (road_height < npos.vy)) {
			mc->jpitch = 0;
			m->jump_flag = FALSE;
			multi_set_swing(EFFECT_JUMP, mc);
			se_play((m->jump_cnt > 15) ? JUMP_HI : 27);
			mc->landing_cnt = m->jump_cnt/3;
			if(m->jump_cnt > 20) set_black_mark(c);
		}
	}

	if(mc->game_stat == 2) {
		if(((!reverse_flag)&&(multi_crsl(new_t) == 0x4500))
		 || (( reverse_flag)&&(game_level <= 2)&&(multi_crsl(new_t) ==0x09d00))
		 || (( reverse_flag)&&(game_level >  2)&&(multi_crsl(new_t) ==0x10d00))) {
			if(m->speed > 8*100*146/160) {
				c->rot.vy   += ONE/2;
				m->v_ry     += ONE/2;
				m->tire_dir += ONE/2;
				if(padno == 0) bv_ry += ONE/2;
				mirror = !mirror;
				init_addobj( OTZS );
			}
			else {
				hit |= 1;
				start_back_hit = TRUE;
			}
		}
	}

	// collision detection against other cars
	if(mc->game_stat >= 2) car_hit = multi_check_car_hit(mc);
	else car_hit = FALSE;
	
	// calculate new shadow vector
	if(m->jump_flag) c->shadow_y = road_height;
	else c->shadow_y = npos.vy = road_height;

	if(!hit && !car_hit) {
		// update new car position data
		c->pos = npos;
		c->pos.vy += mc->start_spin_dy;
		c->rot.vx = nrot.vx;
		c->t = multi_crs(new_t);
		c->s = new_s;
		multi_action_rebound(mc);
		multi_action_effect(mc);
		multi_action_jump(mc);
		multi_action_pitch(mc);
		multi_action_bank(mc);
	}
	else {	/*** ぶつかったーーーー ***/
		// collision with road and/or another car
		mc->i_force -= 5000;
		multi_ts2xz(c->t, 0, &npos, &nrot, mc);
		if(start_back_hit) {
			cry = multi_get_cry(c->t, mc);
			if(reverse_flag) multi_set_rebound(mc, -rsin(cry)/16, -rcos(cry)/16);
			else multi_set_rebound(mc, rsin(cry)/16, rcos(cry)/16);
		}
		else multi_set_rebound(mc, npos.vx - c->pos.vx, npos.vz - c->pos.vz);
		m->tire_rpm = m->tire_rpm*80/100;
		m->speed = m->speed*70/100;
	}
}

//---------------------------------------------------------------------

void 	multi_start_car(MULTI_CAR *mc)
{
	long	s;
	CAR		*c = mc->c;
	
	fn_track("multi_start_car\n")

	s = mc->disp_rpm - 7500;
	if ( s < 0 ) {
		s = -s;
	} else {
		s = s * 6;
		c->u.m.tire_rpm = c->u.m.tire_rpm / 2;
	}
	mc->static_frec = 1000 + s;
}

//---------------------------------------------------------------------

void	multi_engine(MULTI_CAR *mc, long padno)
{
	CAR			*c = mc->c;
	long		eF,eN;
	long		drpm;
	MYCAR_BUF	*m = &c->u.m;
	GEAR_DATA	*g = &mc->gear_data[m->shift];
	VECTOR		b,r;
	long		sn,cn;
	long		rr;
	long		i;
	long		old_tire_rpm;
	long		jump_gas;
	
	fn_track("multi_engine\n")

	if(mc->game_stat == 4) {	/* ゴール後自動運転 */
		m->gas = m->brk = 0;
		mc->accel_open = 256;		// turn off throttle
	}
	else if(mc->limit_time < 30) {
		m->gas = m->brk = 0;	// turn off throttle
		mc->accel_open = 256;
	}
	else if(pad_info[padno].kind == SONY_PAD) {
		m->gas = ((pad_info[padno].data & key_mask[0][KEY_GAS]) != 0)<<8;	// control acceleration key
		m->brk = ((pad_info[padno].data & key_mask[0][KEY_BRAKE]) != 0)<<8; // control brake key
		if(m->gas > 127) {
			if(mc->accel_open > 0) mc->accel_open -= 25;
		} 
		else if(mc->accel_open < 256) mc->accel_open += 25;
	}
	else if(pad_info[padno].kind == NEGCON) {
		m->gas = ((pad_info[padno].data & key_mask[1][KEY_GAS]) != 0) << 8;
		m->brk = ((pad_info[padno].data & key_mask[1][KEY_BRAKE]) != 0) << 8;
		switch (negcon_type) {
			case 0:
			case 1:		m->gas = pad_info[padno].gas  *256/LIMIT_GAS;
						m->brk = pad_info[padno].brake*256/LIMIT_BRAKE;
						break;
			case 2:
			case 3:		m->brk = pad_info[padno].gas  *256/LIMIT_GAS;
						m->gas = pad_info[padno].brake*256/LIMIT_BRAKE;
						break;
			case 4:		m->brk = pad_info[padno].dummy*256/LIMIT_DUMMY;
						break;
			case 5:		m->brk = pad_info[padno].dummy*256/LIMIT_DUMMY;
						m->gas = pad_info[padno].brake*256/LIMIT_BRAKE;
						break;
			case 6:
			case 7:		break;
		}
		mc->accel_open = 256 - m->gas;
	}
	else {
		m->gas = m->brk = 0;
		mc->accel_open = 256;
	}
	
	if(m->jump_flag) {
		jump_gas = m->gas;
		m->brk = 0;
		m->gas = 0;
	}
	
#define		TRIGD		4

	if(m->gas_trig == 0) {
		if(m->gas > 128+TRIGD) m->gas_trig = 1;
	} 
	else if(m->gas_trig == 1) m->gas_trig = 2;
	else if(m->gas < 128-TRIGD) m->gas_trig = 0;

	if(m->brk_trig == 0) {
		if(m->brk > 128+TRIGD) m->brk_trig = 1;
	}
	else if(m->brk_trig == 1) m->brk_trig = 2;
	else if(m->brk < 128-TRIGD) m->brk_trig = 0;
	
	if(m->jump_flag) m->gas = jump_gas;
	
	mc->eload = m->rpm - 8000;
	if(mc->eload < 0) mc->eload = 0;
	mc->eload /= 8;
	mc->eload += m->gas + mc->sub_eload/0x00008000;
	if(mc->eload < 0) mc->eload = 0;
	if(mc->eload > 256) mc->eload = 256;
	if(mc->decload < mc->eload) mc->decload -= (mc->decload - mc->eload)/8;
	else mc->decload -= (mc->decload - mc->eload)/16;
	rr = 380 - 100*m->gas/256;
	rr += m->brk*100/256;
	if(m->tire_mode == RUN_SPIN) {
		rr+=400;	/* くるくる */
		mc->hr = (mc->hr + rr)/2;
	}
	else mc->hr = (mc->hr + rr*m->handle/1000)/2;
	eN = 0;
	old_tire_rpm = m->tire_rpm;
	drpm = m->rpm*g->g_rate - m->tire_rpm;
	mc->engine_load += (drpm > 0) ? drpm : -drpm;
	mc->sub_eload += (drpm > 0) ? 0 : -drpm;
	if(mc->engine_load > 0) {
		mc->engine_load -= 0x00040000;
		if(mc->engine_load < 0) mc->engine_load = 0;
	}
	if(mc->sub_eload > 0) mc->sub_eload -= 0x00010000;
	if(m->tire_mode == RUN_SPIN) eN = drpm/256/16;
	else {
		if(drpm > -REG_RPM/8) {
			if(drpm > REG_RPM) eN = drpm/256*70/100/2;
		}
		else eN = drpm/256/4 /2;
	}
	
	if(m->rpm < 5000) drpm = (g->m_drpm* m->rpm + g->l_drpm*( 5000-m->rpm)) / 50000;
	else drpm = (g->h_drpm*(m->rpm-5000) + g->m_drpm*(10000-m->rpm)) / 50000;
	if(drpm < 0) drpm = 0;

	if(mc->game_stat >= 2) {
		if(!m->jump_flag) eN += m->rpm/256;		/* 回転抵抗 */
		eF = drpm*m->gas/256;
		eN += m->rpm*m->brk/256 / 32;
		if(mc->limit_time < 30) eN += m->rpm/16;
	}
	else {
		eF = 2*333*(m->gas- 77 )/256;	/*大きくすると差がなくなる */
		if(m->rpm > 9200) eN += (m->rpm-9200);
	}
	
	mc->reg_dry = get_dry(c->rot.vy, m->v_ry);
	if(mc->reg_dry > ONE/4) mc->reg_dry = ONE/2 - mc->reg_dry;
	eN += mc->reg_dry/256;	/***/
	
	if((mc->game_stat == 2) && (m->tire_mode == RUN_SLIP)) eN += (mc->static_frec & 0x1f)*32;
	if(mc->landing_cnt > 0) {
		eN += 200 + mc->landing_cnt*20;
		mc->landing_cnt--;
	}
	
	m->rpm += eF - eN;
	if(m->rpm < 0) m->rpm = 0;
	else if(m->rpm > 15000) m->rpm = 15000;
	
	m->tire_rpm = m->rpm*g->g_rate;

	if(m->tire_mode == RUN_SPIN) {
		m->a = m->tire_rpm/256/32 / 128;
		m->speed = m->speed * 996 / 1000;
	}
	else {
		m->a = m->tire_rpm/256/32 /16;
		m->speed = m->speed * 94 / 100;		/* 94/100 空気抵抗 */
	}
	if(m->jump_flag) m->a = m->a *143/160;	
	
	if(m->speed < 8) m->v_ry = c->rot.vy;
	
	if(mc->game_stat != 1) {
		switch (m->tire_mode) {
			case RUN_GRIP:	multi_run_grip(mc);
							break;
			case RUN_SPIN:	multi_run_spin(mc);
							break;
			case RUN_FLOW:	multi_run_flow(mc);
							break;
			case RUN_SLIP:	multi_run_slip(mc);
							break;
		}
	}
	
	if(m->tire_mode == RUN_GRIP) {
		if(mc->engine_load > 0x00100000) {
			if(m->speed < mc->engine_load/2048) {
				hds.skid.v = rand() & 0x7f;
				hds.skid.v = hds.skid.v * 40/100;
				hds.skid.p = 48*128 + (60-48)*128*hds.skid.v/128;
			}
		}
		else {
			hds.skid.v = 0;
			hds.skid.p = 60 << 7;
		}
	}
	
	if(m->speed < 8) m->v_ry = c->rot.vy;

	if(mc->landing_cnt > 0) {
		hds.skid.v =  55;
		hds.skid.p = (56+(rand()&0x3)) << 7;
	}
	if(m->jump_flag) hds.skid.v = 0;
}


//---------------------------------------------------------------------

#define	HANDLE		(16*256)

void	multi_handle(MULTI_CAR *mc, long padno)
{
	CAR				*c = mc->c;
	extern	short	max_limit[4];
	MYCAR_BUF		*m = &c->u.m;
	long			l,r;
	long			ry,cy;
	
	fn_track("multi_handle\n")

	if(mc->game_stat < 2) m->drot.vy = mc->mycar_roll = 0;
	else if(mc->game_stat == 4) {	/* ゴール後自動運転 */
		if(reverse_flag) ry = multi_get_cry(c->t + 0x0180/2, mc) + 0x0800;
		else ry = multi_get_cry(c->t - 0x0200/2, mc);

		cy = get_dry_sign(c->rot.vy, ry)*256 *2 /16;
		ry = (ONE - rcos(c->s*ONE/16/0x80))*4;
		
		if(c->s > 0) ry = -ry;
		ry += cy;
		
		if(ry < -HANDLE) ry = -HANDLE;
		if(ry >  HANDLE) ry =  HANDLE;
		m->drot.vy = ry;
		mc->mycar_roll = ry/128;
	}
	else if(pad_info[padno].kind == SONY_PAD) {
		if(mirror) {
			r = pad_info[padno].data & key_mask[0][KEY_LEFT];
			l = pad_info[padno].data & key_mask[0][KEY_RIGHT];
		}
		else {
			l = pad_info[padno].data & key_mask[0][KEY_LEFT];
			r = pad_info[padno].data & key_mask[0][KEY_RIGHT];
		}
		
		if(l) {
			if(m->drot.vy > 0) m->drot.vy = 0;
		 	else if(m->drot.vy > -HANDLE) m->drot.vy -= 5*256;
			mc->mycar_roll -= 10-4;
		}
		else if(r) {
			if(m->drot.vy < 0) m->drot.vy = 0;
		 	else if (m->drot.vy < HANDLE) m->drot.vy += 5*256;
			mc->mycar_roll += 10-4;
		}
		else m->drot.vy = m->drot.vy / 2;

		if(mc->mycar_roll != 0) mc->mycar_roll = mc->mycar_roll*7/8;
	}
	else if(pad_info[padno].kind == NEGCON) {
		m->drot.vy = HANDLE * pad_info[padno].handle *3/2 / max_limit[negcon_limit];
		if(mirror) m->drot.vy = -m->drot.vy;
		mc->mycar_roll = m->drot.vy/128;
	}
	else m->drot.vy = mc->mycar_roll = 0;
	
	if(m->jump_flag) {
		m->drot.vy /= 4;
		mc->mycar_roll = m->drot.vy/128;
	}
	
	if(c->u.m.speed < 100*8) mc->mycar_roll = mc->mycar_roll*c->u.m.speed/(100*8);
	c->rot.vz = mc->mycar_roll;
}

//---------------------------------------------------------------------

void	multi_action_kansei(MULTI_CAR *mc)
{
	CAR		*c = mc->c;
	long	hd;
	long	x,z;
	
	fn_track("multi_action_kansei\n")

	if (mc->game_stat <= 2) {
		
		x = 0x00006991 - c->pos.vx;
		z = 0x0000a23a - c->pos.vz;
		hd = 80 - (csqrt( x*x/4 + z*z/4 ) >> 10);
		if (hd > 40) hd = 40;
		if (hd < 0) hd = 0;
		
		hds.kansei_l.v = hds.kansei_r.v = hd;
	} else {
		hds.kansei_l.v = hds.kansei_r.v = 0;
	}
}

//---------------------------------------------------------------------

void	multi_action_sound(MULTI_CAR *mc)
{
	CAR	*c = mc->c;

	fn_track("multi_action_sound\n")

	/*** リヴァーブ ***/
//	multi_set_rev(mc);
	
	/*** チェックトーン ***/
	hds.check_tone.v = (mc->cp_cnt > 4*30+15) ? 100 : 0;
}

//---------------------------------------------------------------------

void	multi_check_lap(MULTI_CAR *mc)
{
	CAR		*c = mc->c;
	long	t,dd;
	long	ls;
	static	short	lap_se[] = { 0, VOICE08, VOICE05, VOICE06, VOICE07, };
	
	fn_track("multi_check_lap\n")

	dd = reverse_flag ? -0x0100 : 0x0100;
	
	if ((multi_crsl(c->t) == start_line)
	 && (multi_crsl(c->u.m.last_t) == (start_line+dd))) {
		if (c->u.m.unlap > 0) {
			c->u.m.unlap--;
		} else {	/*** ラップ！ ***/
			ls = lap_se[mc->goal_lap - c->u.m.lap];
			if (c->u.m.lap > 0) {	/* ただし，最初の通過はラップではない */
				/* ベストラップと比較 */
				c->u.m.disp_lap_time[c->u.m.lap-1] += (rand() & 0x1f);
				t = c->u.m.disp_lap_time[c->u.m.lap-1];
				if (best_lap[mirror][reverse_flag][game_level-1] > t) {
					best_lap[mirror][reverse_flag][game_level-1] = t;
					c->u.m.make_best_lap = c->u.m.lap-1;
					ls = VOICE02;
				}
			}
			if (ls > 0) se_play( ls );
			
			c->u.m.lap++;
			if ((c->u.m.lap > 1)&&(c->u.m.lap <= mc->goal_lap)) {
				mc->cp_cnt = 5*30;
				if(reverse_flag) {
					mc->limit_time += extend_time[game_level-1].extra*30;
				}
				else {
					mc->limit_time += extend_time[game_level-1].finish*30;
				}
			}
		}
	}
	else if((multi_crsl(c->t) == (start_line+dd)) && (multi_crsl(c->u.m.last_t) == start_line)) {
		c->u.m.unlap++;
	}
}

//---------------------------------------------------------------------

void	multi_check_cp1(MULTI_CAR *mc)
{
	CAR	*c = mc->c;

	fn_track("multi_check_cp1\n")

	if((multi_crsl(c->t) == check_point_1) && (multi_crsl(c->u.m.last_t) == (check_point_1+0x0100))) {
		if(c->u.m.uncp1 > 0) {
			c->u.m.uncp1--;
		}
		else {
			mc->limit_time += extend_time[game_level-1].cp1*30;
			mc->cp_cnt = 5*30;
		}
	}
	else if((multi_crsl(c->t) == (check_point_1+0x0100)) && (multi_crsl(c->u.m.last_t) == check_point_1)) {
		c->u.m.uncp1++;
	}
}

//---------------------------------------------------------------------

void	multi_check_cp2(MULTI_CAR *mc)
{
	CAR *c = mc->c;

	fn_track("multi_check_cp2\n")

	if ((multi_crsl(c->t) == check_point_2) && (multi_crsl(c->u.m.last_t) == (check_point_2+0x0100))) {
		if (c->u.m.uncp2 > 0) {
			c->u.m.uncp2--;
		}
		else {
			mc->limit_time += extend_time[game_level-1].cp2*30;
			mc->cp_cnt = 5*30;
		}
	}
	else if ((multi_crsl(c->t) == (check_point_2+0x0100)) && (multi_crsl(c->u.m.last_t) == check_point_2)) {
		c->u.m.uncp2++;
	}
}

//---------------------------------------------------------------------

void	multi_action_rebound(MULTI_CAR *mc)
{
	CAR *c = mc->c;

	fn_track("multi_action_rebound\n")

	if(mc->rebound_event) {
		if(--mc->rebound_cnt <= 0) mc->rebound_event = FALSE;
		c->pos.vx += mc->rebound_v.vx/8;
		c->pos.vz += mc->rebound_v.vz/8;
		mc->rebound_v.vx = mc->rebound_v.vx * 7 / 8;
		mc->rebound_v.vz = mc->rebound_v.vz * 7 / 8;
	}
}

//---------------------------------------------------------------------

void	multi_set_rebound(MULTI_CAR *mc, long vx, long vz)
{
	fn_track("multi_set_rebound\n")

	mc->rebound_event = TRUE;
	mc->rebound_cnt = REBOUND_TIME;
	mc->rebound_v.vx = vx;
	mc->rebound_v.vz = vz;
	multi_action_rebound(mc);
}

//---------------------------------------------------------------------

void	multi_action_effect(MULTI_CAR *mc)
{
	CAR		*c = mc->c;
	long	dr,w;
	
	fn_track("multi_action_offset\n")

	if(mc->swing_event) {
		if(--mc->swing_cnt == 0) {
			mc->swing_event = 0;
			mc->epitch = 0;
		}
		w = mc->swing_cnt * mc->swing_width / 128;
		dr = rsin(mc->swing_cnt*3*ONE/HIT_SWING_TIME) * w / 2048;
		switch (mc->swing_event) {
			case EFFECT_JUMP:
			case EFFECT_HELI:	mc->epitch = dr+w;
								c->rot.vx += mc->epitch;
								break;
			case EFFECT_HIT:	c->rot.vz += dr;
								break;
			case EFFECT_KK:		mc->epitch = dr+w;
								c->rot.vx += mc->epitch;
								break;
			case EFFECT_STOP:	c->rot.vz += dr;
								break;
		}
	}
}

//---------------------------------------------------------------------

void	multi_action_jump(MULTI_CAR *mc)
{
	CAR		*c = mc->c;
	long	p,d;
	static	long	vy[] = {32, 32, 512/8, 64, 8, 16, };
	MYCAR_BUF	*m = &c->u.m;
	
	fn_track("multi_action_jump\n")

	if(m->jump_flag) {
		d = c->rot.vx;
		c->rot.vx = m->jump_pitch + m->jump_vy;
		mc->jpitch = c->rot.vx - d;
	}
	else {
		if(p = multi_check_jump(mc)) {
			m->jump_flag = TRUE;
			m->jump_vy = - m->speed/vy[p-1];
						/* テーブルにしよう速度で変えよう */
			m->jump_pitch = c->rot.vx - m->jump_vy;
			m->jump_cnt = 0;
		}
	}
}

//---------------------------------------------------------------------

void	multi_action_pitch(MULTI_CAR *mc)
{
	CAR			*c = mc->c;
	MYCAR_BUF	*m = &c->u.m;
	
	fn_track("multi_action_pitch\n")

	if(mc->game_stat < 2) {
		mc->dpitch = 0;
		return;
	}
	
	/*** 微小ピッチ変化 ***/
	if (mc->limit_time < 30) {
		mc->dpitch += 1;
		if(mc->dpitch > 16) mc->dpitch = 16;
	}
	else if((m->brk > 128) && (m->speed > 10*8)) {
		mc->dpitch += 1;
		if(mc->dpitch > 8) mc->dpitch = 8;
	}
	else if(m->gas > 128) {
		mc->dpitch -= 2;
		if(mc->dpitch < -16) mc->dpitch = -16;
	}
	else {
		mc->dpitch = mc->dpitch * 3 / 4;
	}
	c->rot.vx += mc->dpitch;
}

//---------------------------------------------------------------------

void	multi_action_bank(MULTI_CAR *mc)
{
	CAR		*c = mc->c;
	long	bk;
//	extern	VECTOR	car_tire_pos[4];
	VECTOR	*v = &mc->car_tire_pos[0];
	long	ave,dy;
	
	fn_track("multi_action_bank\n")

	/*** 縁石乗り上げチェック ***/
	/* 左右 */
	ave = (v[0].vy - v[1].vy) + (v[2].vy - v[3].vy);
	c->rot.vz -= ave*4;
	if(ave > 4) c->u.m.tire_dir += ave*c->u.m.a/64;
	dy = v[0].vy + v[1].vy + v[2].vy + v[3].vy;
	dy = dy*30/100;	/*- ave*50/100;*/
	c->pos.vy += dy;
	c->shadow_y += dy;
	/* 前後 */
	ave = (v[0].vy - v[2].vy) + (v[1].vy - v[3].vy);
	mc->bpitch = -ave*4;
	c->rot.vx += mc->bpitch;
	
	/* バンクによって高さを変えるとゆー・・・ */
	bk = rsin(c->rot.vz) / 64;
	if(bk < 0) bk = -bk;
	c->pos.vy += bk;
}

//---------------------------------------------------------------------

void	multi_set_swing(long mode, MULTI_CAR *mc)
{
	CAR		*c = mc->c;
	long	dy,cy;
	
	fn_track("multi_set_swing\n")

	mc->swing_event = mode;
	switch (mode) {
		case EFFECT_JUMP:	/* ジャンプ後 */
							mc->swing_width = c->u.m.jump_cnt * 8;
							mc->swing_cnt = HIT_SWING_TIME;
							break;
		case EFFECT_HIT:	/* 壁に当たった */
							cy = multi_get_cry(c->t, mc);
							dy = get_dry( cy, c->rot.vy );
							if (dy > ONE/4) dy = ONE/2 - dy;
							mc->swing_width = c->u.m.speed * dy/(ONE/2)/2;
							mc->swing_cnt = HIT_SWING_TIME;
							if(rand() & 0x0080) {
								mc->swing_width = - mc->swing_width;
							}
							break;
		case EFFECT_KK:		/* 蹴飛ばし看板踏み */
							mc->swing_width = c->u.m.speed / 8;
							mc->swing_cnt = HIT_SWING_TIME;
							break;
		case EFFECT_STOP:	/* スリップしてて急に止まった */
							mc->swing_width = c->u.m.speed / 8;
							mc->swing_cnt = HIT_SWING_TIME;
							break;
		case EFFECT_HELI:	/* ヘリにぶつかった */
							mc->swing_width = 200;
							mc->swing_cnt = HIT_SWING_TIME;
							break;
	}
}

//---------------------------------------------------------------------

void	multi_run_grip(MULTI_CAR *mc)
{
	CAR			*c= mc->c;
	MYCAR_BUF	*m = &c->u.m;
	VECTOR		b;
	long		sn,cn;
	long		bx;
	long		h;
	
	fn_track("multi_run_grip\n")

	c->rot.vy += get_dry_sign( c->rot.vy, m->tire_dir )*20/100;
	
	go_v(c);
	
	sn = rsin( c->rot.vy );
	cn = rcos( c->rot.vy );
	
	m->dpos.vx = m->speed*rsin( m->v_ry )/256;
	m->dpos.vz = m->speed*rcos( m->v_ry )/256;
	
	/* 回転 */
	b.vx = ( cn*m->dpos.vx - sn*m->dpos.vz)/ONE;
	b.vz = ( sn*m->dpos.vx + cn*m->dpos.vz)/ONE;
	
	/* 逆回転 */
	m->dpos.vx = sn*b.vz/ONE;
	m->dpos.vz = cn*b.vz/ONE;
	
	if(m->gas_trig == 1) {
		mc->i_force = m->speed * mc->i_cnt;
		mc->i_cnt = 0;
		if((m->speed > 120*8) && (mc->i_force > m->slip_level)) {
			m->tire_mode = RUN_SPIN;
			h = 1000 - (m->handle-1000)*8;
			if (h < 1000) h = 1000;
			mc->spin_r = -b.vx*(h)/1000*2;		/* 160/100->2 */
			mc->spin_way = reverse_flag ^ multi_wrong_way(mc);
		}
	}
	else if ( (m->gas < 128) && (m->brk_trig == 1) ) {
		bx = (b.vx > 0) ? b.vx : -b.vx;
		mc->i_force = m->speed * bx / 64;
		mc->i_cnt = 0;
		if ((m->speed > 40*8) && (mc->i_force > m->slip_level)) {
			m->tire_mode = RUN_SPIN;
			mc->spin_r = -b.vx* 100/100;		/* old:40!!! */
			mc->spin_way = reverse_flag ^ multi_wrong_way(mc);
		}
	}
	else if (m->gas < 128) mc->i_cnt++;
	else mc->i_cnt = 0;
}

//---------------------------------------------------------------------

void	multi_run_spin(MULTI_CAR *mc)
{
	CAR			*c = mc->c;
	MYCAR_BUF	*m = &c->u.m;
	VECTOR		b;
	long		sn,cn;
	long		dry;
	long		d;
	long		asr;	/* spin_r の絶対値 */
	long		bry;
	long		sper;
	
	/*** 道に対して浅い角度になったとき，グリップする ***/
	fn_track("multi_run_spin\n")
	
	asr = (mc->spin_r >= 0) ? mc->spin_r : -mc->spin_r;
	
	dry = get_dry( c->rot.vy, m->v_ry );
	/*** スキッド音ボリューム＆ピッチ設定 ***/
	hds.skid.v = (dry > ONE/8) ? (127) : (dry*127/(ONE/8));
	hds.skid.p = 48*128 + ((dry > ONE/8) ? (60-48)*128 : (dry*(60-48)*128/(ONE/8))) /2;
	hds.skid.v = hds.skid.v * 60/100;
	
	if ( (dry < ONE/4/8) && ( asr < 8*256 ) ) {
/*		m->tire_mode = RUN_GRIP;
		m->tire_rpm = m->speed * SPIN_LOAD;
*/
		mc->i_force -= (8*256-asr)*4000/256;
	}
	if(m->speed < 50*8 ) mc->i_force -= (50*8 - m->speed) * 100;
	
	{
		if (mc->i_force > 0) {
			dry = get_dry_sign( c->rot.vy, m->tire_dir )* 98/100;
			dry = dry * (ONE/4+ONE/4+mc->reg_dry) / (ONE/2);
/*			if (m->gas < 128) dry = dry * 1 / 2;	*/
			mc->spin_r += dry*16;
			
			/*** まっすぐ補正? ***/
			if ( (m->drot.vy < ONE/32) && (m->drot.vy > -ONE/32) ) {
	/*			add_str( 80, 80, "NO HANDLE", WHITE );	*/
				if ( get_dry( c->rot.vy, m->v_ry ) < ONE/4/2 ) {
					mc->spin_r = mc->spin_r * 31 / 32;
					mc->spin_r += get_dry_sign( c->rot.vy, m->v_ry );
				} else {
					if (asr < 8*256) mc->spin_r += dry*16 / 32;
				}
			}
#define	ROT_MAX		(54*256)
			if (mc->spin_r > ROT_MAX) mc->spin_r = ROT_MAX;
			if (mc->spin_r <-ROT_MAX) mc->spin_r =-ROT_MAX;
			
			c->rot.vy += mc->spin_r/256;
			
			mc->i_force -= 64;
			d = get_dry( c->rot.vy, m->v_ry );
			mc->i_force -= d*d*8/128/4096;
			mc->i_force -= (ROT_MAX - asr)*2 *2/256;	/* 10/26 */
			if (m->speed < m->max_speed/2) {
				mc->i_force -= (m->max_speed/2 - m->speed)/8;
			}
			/*** ブレーキを踏むととてもグリップしやすい ***/
			mc->i_force -= m->brk*4;
		} else {
			mc->spin_r = mc->spin_r * 15 / 16;
			if ( asr < 16*256 ) {
				m->tire_rpm = m->speed * SPIN_LOAD*(100-(m->shift-1)*4)/100;
				
				mc->flow_dry = get_dry_sign( m->v_ry, c->rot.vy );
				mc->flow_ry = m->v_ry;
				m->v_ry = c->rot.vy;
				
				sper = (mc->flow_dry > 0) ? mc->flow_dry : -mc->flow_dry;
				if (sper > ONE/4) sper = ONE/2 - sper;
				
/*				sper = ONE/4 - sper;*/
				sper = sper*sper;
/*				sper = ONE*ONE/4/4 - sper;*/
				
				mc->flow_speed = m->speed * sper          /(ONE*ONE/4/4);
				m->speed   = m->speed * (ONE*ONE/4/4 - sper)/(ONE*ONE/4/4);
				
				m->tire_mode = RUN_FLOW;
			}
		}
	}
	m->tire_dir = c->rot.vy;
 	
	multi_run_arong(mc);
	
	dry = get_dry( c->rot.vy, m->v_ry);
	if ( dry > ONE/4 ) {
		m->a = m->a/2 + 4*m->a * (ROT_MAX-asr) * (dry-ONE/4)/(ONE/4) /ROT_MAX;
	} else {
		m->a = m->a * (ONE/8 - dry) / (ONE/8);
	}
	
	bry = m->v_ry;
	go_v(c);
	m->v_ry = bry;
	
	sn = rsin( c->rot.vy );
	cn = rcos( c->rot.vy );
	
	/* 以前の速度ベクトル */
	m->dpos.vx = m->speed*rsin( m->v_ry )/256;
	m->dpos.vz = m->speed*rcos( m->v_ry )/256;
	
	/* 車体方向をＸ軸上へ回転 */
	b.vx = ( cn*m->dpos.vx - sn*m->dpos.vz)/ONE;
	b.vz = ( sn*m->dpos.vx + cn*m->dpos.vz)/ONE;
	
/*	if (mc->reg_dry < ONE/4/4) {
		if (b.vx > 0) {
			b.vx = (b.vx <  FRIC) ? 0 : (b.vx - FRIC);
		} else {
			b.vx = (b.vx > -FRIC) ? 0 : (b.vx + FRIC);
		}
	}
*/	
	/* 元に戻すために逆回転 */
	m->dpos.vx = ( cn*b.vx + sn*b.vz)/ONE;
	m->dpos.vz = (-sn*b.vx + cn*b.vz)/ONE;
	
}

//---------------------------------------------------------------------

void	multi_run_flow(MULTI_CAR *mc)
{
	CAR			*c = mc->c;
	MYCAR_BUF	*m = &c->u.m;
	VECTOR		b;
	long		sn,cn;
	long		d,e;
	
	fn_track("multi_run_flow\n")

	c->rot.vy += get_dry_sign( c->rot.vy, m->tire_dir )*20/100;
	
	go_v( c );
	
	sn = rsin( c->rot.vy );
	cn = rcos( c->rot.vy );
	
	m->dpos.vx = m->speed*rsin( m->v_ry + mc->flow_dry )/256;
	m->dpos.vz = m->speed*rcos( m->v_ry + mc->flow_dry )/256;
	
	/* 回転 */
	b.vx = ( cn*m->dpos.vx - sn*m->dpos.vz)/ONE;
	b.vz = ( sn*m->dpos.vx + cn*m->dpos.vz)/ONE;
	
	/* 逆回転 */
	m->dpos.vx = mc->flow_speed*rsin( mc->flow_ry )/256 + sn*b.vz/ONE;
	m->dpos.vz = mc->flow_speed*rcos( mc->flow_ry )/256 + cn*b.vz/ONE;
	
	if ( (m->gas_trig == 1) || (m->brk_trig == 1) ) {
		mc->i_cnt = 0;
	} else if (m->gas < 128) {
		mc->i_cnt++;
	} else {
		mc->i_cnt = 0;
	}
	
	mc->flow_speed = mc->flow_speed * 31 / 32;
	mc->flow_dry = mc->flow_dry * 31 / 32;
	if ((mc->flow_speed == 0)&&(mc->flow_dry == 0)) {
		m->tire_mode = RUN_GRIP;
	}
	
	hds.skid.v = 0;
}

//---------------------------------------------------------------------

/*** ホイルスピン ***/
void	multi_run_slip(MULTI_CAR *mc)
{
	CAR			*c = mc->c;
	MYCAR_BUF	*m = &c->u.m;
	VECTOR		b;
	long		sn,cn;
	long		bx;
	long		h;
	long		f;
	
	fn_track("multi_run_slip\n")

	c->rot.vy += get_dry_sign( c->rot.vy, m->tire_dir )*20/100;
	
	go_v( c );
	
	sn = rsin( c->rot.vy );
	cn = rcos( c->rot.vy );
	
	m->dpos.vx = m->speed*rsin( m->v_ry )/256;
	m->dpos.vz = m->speed*rcos( m->v_ry )/256;
	
	/* 回転 */
	b.vx = ( cn*m->dpos.vx - sn*m->dpos.vz)/ONE;
	b.vz = ( sn*m->dpos.vx + cn*m->dpos.vz)/ONE;
	
	/* 逆回転 */
	m->dpos.vx = sn*b.vz/ONE /4;
	m->dpos.vz = cn*b.vz/ONE /4;
	
	if (mc->static_frec > 10) {
		
		mc->static_frec -= m->brk;
		
		f = 8 + m->gas;
		mc->static_frec -= f;
		if (mc->static_frec <= 0) m->tire_mode = RUN_GRIP;
		mc->start_spin_dy = (rand() & 3)*f/256;
	/*	f = f * (rand() & 0xff)/256/2;
		if (f > 127) f = 127;
		hds.skid.v = f*60/100;
		hds.skid.p = 48*128 + (60-48)*128*f/256;
	*/
	} else {
		m->tire_mode = RUN_GRIP;
		mc->start_spin_dy = 0;
	}
	
	/*** スタート時ホイルスピンによるスキッド音 ***/
	if ((mc->game_stat == 2) && (mc->pad == 0)) {	// MARK only do for car 0 //
		hds.skid.v = (32+64-(mc->static_frec & 0x1f)*2)*m->gas/256;
		hds.skid.p = (48+5)*128;
	}
	
}

//---------------------------------------------------------------------

void	multi_set_rev(MULTI_CAR *mc)
{
	CAR		*c = mc->c;
	long	s;
	
	fn_track("multi_set_rev\n")

	s = multi_check_tunnel(c->t, mc);
	if(s) {
		if(!mc->tunnel_rev) {		/* リバーブオン */
			se_reverb_depth( 70, 70 );
		}
	}
	else {
		if(mc->tunnel_rev) {			/* リバーブオフ */
			se_reverb_depth(0, 0);
		}
	}
	mc->tunnel_rev = s;
}

//---------------------------------------------------------------------

long	multi_check_jump(MULTI_CAR *mc)
{
	CAR		*c = mc->c;
	long	ct;
	
	fn_track("multi_check_jump\n")

	if ( c->u.m.speed < 152*8 ) return( FALSE );
	if ( mc->swing_event ) return( FALSE );
	
	ct = multi_crsl(c->t);
	if(reverse_flag ^ multi_wrong_way(mc)) {
		if (game_level < 3) {
			if (ct == 0x03400) return( 5 );
			if (ct == 0x07900) return( 6 );
		}
		else {
			if (ct == 0x03400) return( 5 );
			if (ct == 0x0e900) return( 6 );
		}
	}
	else {
		if (game_level < 3) {
			if (ct == 0x07900) return( 1 );
		}
		else {
			if (ct == 0x0e900) return( 2 );
			if (ct == 0x07700) return( 3 );
			if (ct == 0x05d00) return( 4 );
		}
	}
	return (FALSE);
}

//---------------------------------------------------------------------

void	multi_action_engine(void)
{
	fn_track("multi_action_engine\n")

	/*** エンジン音 ***/
	hds.engine.v = (20 + (mcar[CommMyCar].disp_rpm)*(20)/10000
						+ mcar[CommMyCar].decload*120/256 )*mcar[CommMyCar].engine_master/256;
	hds.engine.p = 36*128+(mcar[CommMyCar].disp_rpm)*(84-36)*128/10000;
	
	/*** ロードノイズ音 ***/
	hds.road_noise.v = 0;
	
	/*** オフノート ***/
	hds.off_note.v = (35 + (mcar[CommMyCar].disp_rpm)*(40)/10000 
					- mcar[CommMyCar].decload*30/256
					)*mcar[CommMyCar].engine_master/256;
	hds.off_note.p = hds.engine.p;
	
	/*** オンノート ***/
	hds.on_note.v = ((mcar[CommMyCar].disp_rpm)*(10)/10000
					+ 10 + mcar[CommMyCar].decload*80/256
					+ hds.air_note.v*0)*mcar[CommMyCar].engine_master/256;
	hds.on_note.p = hds.engine.p;
	
	/*** エアノート ***/
	hds.air_note.v = (0* mcar[CommMyCar].accel_open/25)*mcar[CommMyCar].engine_master/256;
	hds.air_note.p = hds.engine.p;
	
	/*** エアノート２（鳴りっぱなし音） ***/
	hds.air_note.v = (0*mcar[CommMyCar].disp_rpm*10/10000
					+ mcar[CommMyCar].decload*30/256)*mcar[CommMyCar].engine_master/256;
	hds.air_note.p = hds.engine.p;
}

//---------------------------------------------------------------------

/*** プレイ中及びデバッグ用 ***/
void	multi_set_camera(long cn, MULTI_CAR *mc)
{
	CAR			*trg = mc->c;
	long		rz_buf;
	long		dx,dz;
	VECTOR		d;
	long		t;
	long		s;
	
	fn_track("multi_set_camera\n")

	t = trg->t;
	
	switch (cn) {
		case 0:		/* 最も近いカメラが追う */
					view.eye = camera_table[multi_get_near_camera(mc)];
			
					s = calc_dist( &view.eye, &trg->pos )*2;
					view.eye.vy -= f_totsu( s )*0x20/ONE;
			
					t = view.eye.pad;
					d.vx = trg->pos.vx - view.eye.vx;
					d.vy = trg->pos.vy - view.eye.vy;
					d.vz = trg->pos.vz - view.eye.vz;
					view.rot.vy = 0x400 - atan( d.vx, d.vz );
					view.rot.vx = 0x400 - atan( d.vy,
					csqrt( d.vx*d.vx + d.vz*d.vz ) >> 6 );
					view.rot.vz = 0;
					break;
		
		case  1:	/*** DRIVER'S EYES ***/
					view.eye = trg->pos;
		/*			view.eye.vy -= 0xc+4;		/* OK!(8/25) */
					view.eye.vy -= 0xc+4+8;		/* 9/8 新データ構造より */
			
					view.rot = trg->rot;
					view.rot.vz -= mcar[CommMyCar].mycar_roll*2;
		/*			view.rot.vx -= 0x008;	/* ＋で下向き －で上向き */
					break;
		
		case  2:	/*** Virtual Button 1 (Top View) ***/
					view.eye = trg->pos;
					view.rot = trg->rot;
					view.eye.vy -= 0x140/4 + 0x200/64;
					view.eye.vx += rsin( view.rot.vy )*3/4 /8/4;
					view.eye.vz += rcos( view.rot.vy )*3/4 /8/4;
					view.rot.vx += 0x400;
					view.rot.vz = 0;
			
					view.eye.vy -= 0x800;
					view.rot.vx -= 0x0a0;
					break;
		
		case  3:	/*** Virtual Button 2 (Final Lap俯瞰) ***/
					/* 酔う
					ts2xz( t+0x080, 0, &view.eye, &view.rot );
					view.eye.vy -= 0x040;
			
					d.vx = trg->pos.vx - view.eye.vx;
					d.vy = trg->pos.vy - view.eye.vy;
					d.vz = trg->pos.vz - view.eye.vz;
					view.rot.vy = 0x400 - atan( d.vx, d.vz );
					view.rot.vx = 0x400 - atan( d.vy,
					csqrt( d.vx*d.vx + d.vz*d.vz ) >> 6 );
					view.rot.vz = 0;
					break;
					*/
					/****** 速度ベクトル後方よりの視点 ******/
					{
						long	d,dy,vx;
						long	drx;
						
//						extern	long	dpitch;
//						extern	long	bpitch;
//						extern	long	jpitch;
//						extern	long	epitch;
			
						if (cn != old_camera_num) bv_ry = trg->u.m.v_ry;
						else if (!pause_on) bv_ry += get_dry_sign( bv_ry, trg->u.m.v_ry )/4;
			
						d = get_dry_sign( bv_ry, trg->rot.vy );
						if (d > ONE/4) d = ONE/2 - d;
						if (d <-ONE/4) d =-ONE/2 - d;
						d /= 6;
			
						dy = trg->rot.vx & 0x0fff;
						if (dy > ONE/2) dy -= ONE;
						view.eye = trg->pos;
						view.rot = trg->rot;
						view.rot.vx += 0x050 - mcar[CommMyCar].dpitch - mcar[CommMyCar].bpitch - mcar[CommMyCar].jpitch - mcar[CommMyCar].epitch;
	/*			        drx += 0x050 - dpitch - bpitch - jpitch - epitch;	*/
						drx = view.rot.vx;
						vx  = rcos( drx )*0x0f0/ONE;
			
						view.eye.vy -= rsin( drx )*0x1e0/ONE;
			
						view.eye.vx -= vx*rsin( bv_ry-d )/ONE;
						view.eye.vz -= vx*rcos( bv_ry-d )/ONE;
						view.rot.vz = 0;
						view.rot.vy = bv_ry;
					}
					break;
/****************************************/
			
		/*			view.eye = trg->pos;
					view.rot = trg->rot;
					view.eye.vy -= 0x040;
					view.eye.vx -= rsin( view.rot.vy )/16;
					view.eye.vz -= rcos( view.rot.vy )/16;
					view.rot.vx += 0x040;
					view.rot.vz = 0;
					break;
		*/		
		case  4:	/*** Virtual Button 3 ( DEBUG ) ***/
					view.eye = jumbo;
					view.eye.vy -= 0x800;
					view.rot.vx = 0x0400;
					view.rot.vy = jumbo.pad;
					view.rot.vz = 0x0000;
					break;
		
		case 5:
			/*
					view.eye = trg->pos;
					view.rot = trg->rot;
					view.eye.vy -= 0xc;
					view.eye.vx += rsin( view.rot.vy + ONE*3/4 )/100;
					view.eye.vz += rcos( view.rot.vy + ONE*3/4 )/100;
					view.rot.vx += -0x080;
					view.rot.vz +=  0x0000;
			*/
					view.eye = trg->pos;
					view.rot = trg->rot;
					view.eye.vy -= 0x010;
					s = rsin( g_cnt*16 ) / 32;		/* 45 degrees */
					s = ONE/32;
					view.eye.vx += rsin( view.rot.vy+s )/0x18;
					view.eye.vz += rcos( view.rot.vy+s )/0x18;
					view.rot.vy += 0x800;
					view.rot.vx -= 0x008;
					view.rot.vz = 0;

					break;
		
		case  6:	/*** Virtual Button 4 ( HELI ) ***/
					view.eye = heli.pos;
					view.eye.vy -= 0x1000;
					view.rot.vx = 0x0400;
					view.rot.vy = 0x0800 - heli.rot.vy;
					view.rot.vz = 0x0000;
					break;
	} // end switch
	
	view_trans();
	if((cn != 1) || (trg != &mycar[0])) add_car(&mycar[0], TRUE);
	action_swap(t);
	old_camera_num = cn;
}

//---------------------------------------------------------------------

void	multi_add_others(void)
{
/*	STATIC_OBJ	*v;	*/
	MATRIX	rot,rot2;
	long	i,ry,n,s;
	VECTOR	d;
	long	vx,vz;
	long	pad;
	CAR		gc;
	VECTOR	num;
	long	model;
	CAR		*c;
	
	fn_track("multi_add_others\n")

	/*** スタート位置ガードレール ***/
	if(mcar[CommMyCar].game_stat == 6) {
		add_obj_table(&start0_tbl[0]);
		bldg_add_obj_table(&start0div_tbl[0]);
	}
	else {
		if(mcar[CommMyCar].game_stat < 3) {
			add_obj_table(&start0_tbl[0]);
			bldg_add_obj_table(reverse_flag ? start0div_tbl_r[game_stat >= 3] : &start0div_tbl[0]);
			bldg_add_obj_table(&start0div_tbl[0]);
		}
		else add_obj_table(&start1_tbl[0]);
	}
	
	/*** 初級？　上級？ ***/
	add_obj_table((replay_game_level < 3) ? &branch0_tbl[0] : &branch1_tbl[0]);
	
	/*** 逆走ざーます ***/
	if( reverse_flag) {
		add_obj_table_dq(reverse_tbl);
		/*** ジャンボカーのモニタ内 ***/
		set_rot_y(&rot, 0x0874);
		MulMatrix2(&rot_trans, &rot);
		add_obj_p(&gala_r, &rot, GALA_BACK, 0);
		add_obj_p(&gala_r, &rot, GALA_ANIM+((g_cnt/4)&0xf), 0);
	}
	
	/*** 気球 ***/
	if(reverse_flag && (voice_trig & 0x0020)) {	/* 切り通しヘアピン */
		if(bmh(baloon1.vx, baloon1.vz)) {	/*rot:0x924*/
			d = baloon1;
			d.vy -= mycar[0].u.m.lap * 0x100;
			add_obj_p_dq(&d, &rot_trans, BALOON_1, 0);
		}
	}
	if(reverse_flag && (voice_trig & 0x0040)) {	/* 工事現場後 */
		if(bmh(baloon2.vx, baloon2.vz)) {	/*rot:0x924*/
			d = baloon2;
			d.vy -= mycar[0].u.m.lap * 0x220;
			add_obj_p_dq(&d, &rot_trans, BALOON_1, DCPage(500,501));
		}
	}
	
	/*** BLACK MARK ***/
	if(bmh(black_mark_pos.vx, black_mark_pos.vz)) {
		set_rot_x(&rot, ONE/4);
		set_rot_y(&rot2, black_mark_pos.pad + ONE/4);
		MulMatrix2(&rot2, &rot);
		MulMatrix2(&rot_trans, &rot);
//#define		BM_ZS		((-5)*sizeof(u_long))
#define		BM_ZS		((-5)*4)
		cs->apc_head[0] -= BM_ZS;
		add_obj_p_dq(&black_mark_pos, &rot, BLACK_MARK, 0);
		cs->apc_head[0] += BM_ZS;
	}
	
	/*** ITALIAN TOMATO (sign board) ***/
	if(bmh(itatoma.vx, itatoma.vz)) {
		set_rot_y(&rot, g_cnt*16*(reverse_flag ? -1 : 1));
		MulMatrix2(&rot_trans, &rot);
		add_obj_p_dq(&itatoma, &rot, TOMATO, 0);
	}
	
	/*** 滝 ***/
	if(bmh(fall.vx, fall.vz)) {
		add_obj_p_dq(&fall, &rot_trans, FALL, DCPage(274, 271+((g_cnt>>2) & 3)));
	}
	
	/*** ジャンボトロンの中身 ***/
	if(bmh(gala.vx, gala.vz)) {
		if((job == JOB_CDPLAY) || reverse_flag || (mcar[CommMyCar].total_time < 30*30)) {
			/* パターンＡ（ギャラクシアン） */
			add_obj_p(&gala, &rot_trans, GALA_BACK, 0);
			add_obj_p(&gala, &rot_trans, GALA_ANIM+((g_cnt/4)&0xf), 0);
		}
		else {
			/* パターンＢ（ギャル） */
			add_obj_p(&gala, &rot_trans, GAL_BACK, 0);
			add_obj_p(&gala_cn, &rot_trans, GAL_CARNO, 0);
			add_obj_p(&gala_wn, &rot_trans, GAL_WINDOWF, 0);
			add_obj_semi(&gala_wn, &rot_trans, GAL_WINDOW, 0);
			model = TOP_car;
			n = jumbo_car[model].car_no;
			if(n >= 10) {
				add_obj_p(&gala_cnn, &rot_trans, GAL_NUMBER+1, 0);
				n -= 10;
				num = gala_cnn;
				num.vz -= 0x30;
				add_obj_p(&num, &rot_trans, GAL_NUMBER+n, 0);
			}
			else add_obj_p(&gala_cnn, &rot_trans, GAL_NUMBER+n, 0);
			
			add_obj_h(&gala_ld, &rot_trans, GAL_LEADER, 0);
			add_obj_h(&gala, &rot_trans, GAL_ANIM+"1234541111111111"[((g_cnt/4)&0xf)]-'1', 0);
			gc.model = model;
			gc.pos = gc_pos;
			gc.rot = gc_rot;
			gc.rot.vy = 0;
			gc.rot.vx = 0;
			gc.rot.vz = ONE/16;
			gc.tire_rot.vz = g_cnt*16;
			add_jumbo_car(&gc);
		}
	}
	
	/* ショベル */
	if(bmh(shovel.vx, shovel.vz)) {
		set_rot_y(&rot, rsin(g_cnt*16*4)/8+0x800+0x0400);
		MulMatrix2(&rot_trans, &rot);
		add_obj_p_dq(&shovel, &rot, SHOVEL, 0);
	}
	
	/*** 歩きギャル ***/
	if((replay_game_level > 1-1)&&(game_stat < 2)) {
		static	char	board[2][4] = { {0, 1, 2, 3}, {4, 4, 4, 4} };
		
		if(reverse_flag) n = (job_cnt - (4*30))/3;
		else n = (job_cnt - (3*30))/3;
		if(n < 0) n = 0;
		if(n < 12*30) {
			d = walk_gal[reverse_flag];
			d.vx -= (n/8)*0x26*65/100;
			d.vz -= (n/8)*0x08*65/100;
			set_rot_y(&rot, d.pad);
			MulMatrix2(&rot_trans, &rot);
#define		GAL_ZS		((-30)*4)
			cs->apc_head[0] -= GAL_ZS;
			add_obj_p(&d, &rot, WALK_GAL+(n&7), 0);
			add_obj_p(&d, &rot, WALK_BOARD+board[reverse_flag][replay_game_level-1]*8+(n&7), 0);
			cs->apc_head[0] += GAL_ZS;
		}
	}
	
/*	if (Side_stop_car != NULL) {
		c = Side_stop_car;
		d = c->pos;
		d.vx -= rsin( c->rot.vy )/64;
		d.vz -= rcos( c->rot.vy )/64;
		
		set_rot_y( &rot, 0x800-c->rot.vy );
		MulMatrix2( &rot_trans, &rot );
#define		SANKAKU_ZS		((-20)*sizeof( u_long ))
		cs->apc_head[0] -= SANKAKU_ZS;
		add_obj_p( &d, &rot, SANKAKU, 0 );
		cs->apc_head[0] -= SANKAKU_ZS;
	}
*/
	
	/*** スタート位置チェックポイント ***/
	if(reverse_flag) {
		if(bmh(start_cp_r.vx, start_cp_r.vz)) {
			set_rot_y(&rot, 0x2cc);
			MulMatrix2(&rot_trans, &rot);
			add_obj_p_dq(&start_cp_r, &rot, CHECK_POINT+(mycar[0].u.m.lap == mcar[CommMyCar].goal_lap), 0);
		}
	}
	else {
		if(bmh( start_cp.vx, start_cp.vz)) {
			set_rot_y(&rot, ONE/4);
			MulMatrix2(&rot_trans, &rot);
			add_obj_p(&start_cp, &rot,	CHECK_POINT+(mycar[0].u.m.lap == mcar[CommMyCar].goal_lap), 0);
		}
	}
	
	/*** タイムエクステンド ***/
	n = TIME_EXTEND + (mcar[CommMyCar].cp_cnt ? ((g_cnt/4)&1) : 2);
	if(bmh(time_ext1.vx, time_ext1.vz)) {
		set_rot_y(&rot, ONE/2+ONE/8 +ONE/32+ONE/64);
		MulMatrix2(&rot_trans, &rot);
		add_obj_p(&time_ext1, &rot, n, 0);
	}
	if(bmh(time_ext2.vx, time_ext2.vz)) {
		set_rot_y(&rot, ONE/2+ONE/4 -ONE/16 + ONE/128);
		MulMatrix2(&rot_trans, &rot);
		add_obj_p(&time_ext2, &rot, n, 0);
	}
	
	/*** end of this function ***/
}

//---------------------------------------------------------------------

void	multi_action_dq(void)
{
	long	sj;
	
	fn_track("multi_action_dq\n")

	if(dq_trig->tt == (mcar[CommMyCar].total_time % (30*60*5))) {
		set_dq(dq_trig++);
		if(dq_trig->tt < 0) dq_trig = dq_table[0] + 1;
	}
	
	if(dq.flag) {
		if(dq.si < dq.di) {
			dq.si++;
			sj = dq.di - dq.si;
			dq.c.r = (dq.sc.r * sj + dq.dc.r * dq.si)/dq.di;
			dq.c.g = (dq.sc.g * sj + dq.dc.g * dq.si)/dq.di;
			dq.c.b = (dq.sc.b * sj + dq.dc.b * dq.si)/dq.di;
			SetFarColor(dq.c.r, dq.c.g, dq.c.b);
			if(dq.sf) dq.c.cd = (dq.ssi * sj + dq.sdi * dq.si)/dq.di;
			if(dq.si == dq.di) {
				if((dq.c.r == 0x80)&&(dq.c.g == 0x80)&&(dq.c.b == 0x80)) dq.flag = FALSE;
			}
		}
	}
}

//---------------------------------------------------------------------

void	multi_disp_indicator(void)
{
	int	i;

	CAR	*dc = &mycar[0];
	CAR	*c;
	
	fn_track("multi_disp_indicator\n")

	for(i=0; i<CommNumLinkedCars; i++) {
		c = mcar[i].c;
		if (!pause_on) {
			mcar[i].disp_rpm += (c->u.m.rpm - mcar[i].disp_rpm)/4;
			if (mcar[i].disp_rpm > 10000) mcar[i].disp_rpm = 10000;
			if (mcar[i].disp_rpm <   500) mcar[i].disp_rpm = 500;
		}
	}

	c = camera_mtrg->c;
//	disp_position( dc->u.m.ranking );
	disp_position(c->u.m.ranking);
	disp_total_time();
//	disp_limit_time( (mcar[CommMyCar].limit_time < 0) ? 0 : mcar[CommMyCar].limit_time );
	disp_limit_time((camera_mtrg->limit_time < 0) ? 0 : camera_mtrg->limit_time);
/*	disp_lap( dc->u.m.lap, goal_lap );	*/
//	multi_disp_lap_time(&mcar[CommMyCar]);
	multi_disp_lap_time(camera_mtrg);
//	disp_cursor( dc );
	for (i=0; i< CommNumLinkedCars; i++) disp_cursor(mcar[i].c);
	disp_mini_map();
	add_static_prim();
//	disp_speed( dc->u.m.speed*160/146 / 8 );
	disp_speed(c->u.m.speed*160/146 / 8);
//	disp_meter( mcar[CommMyCar].disp_rpm );
	disp_meter(camera_mtrg->disp_rpm);
//	disp_gear( dc->u.m.shift-1 );
	disp_gear(c->u.m.shift-1);
	
	add_total_car_num();
	
	if ((job_cnt > 10*30)&&(job_cnt < 30*30)) {
		disp_bgm_name_anim( 5*8, 27*8, now_bgm, job_cnt -10*30 );
	}
}

//---------------------------------------------------------------------

void	multi_disp_lap_time(MULTI_CAR *mc)
{
	CAR			*c = mc->c;
	MYCAR_BUF	*b = &c->u.m;
	long		i,pal;
	
	fn_track("multi_disp_lap_time\n")

	add_str     ( 31*8, 3*8, "LAP TIME", RED );
	for (i=0;(i<b->lap)&&(i<mcar[CommMyCar].goal_lap);i++) {
		pal = (b->make_best_lap == i) ? RED : WHITE;
		add_time_str( 31*8, (4+i)*8, b->disp_lap_time[i], pal, 1000 );
	}
}

//---------------------------------------------------------------------

/*** 演出用カメラワーク ***/
void	multi_set_camera_super(long cn, MULTI_CAR *mc)
{
	CAR		*trg = mc->c;
	long	rz_buf;
	long	dx,dz;
	VECTOR	d;
	long	t;
	long	s;
	
	fn_track("multi_set_camera_super\n")

	t = trg->t;
	
	switch (cn) {
		case 0:		/* 追尾カメラ */
					view.eye = camera_table[multi_get_near_camera(mc)];
					view.eye.vx += (trg->pos.vx - view.eye.vx)/2;
					view.eye.vy += (trg->pos.vy - view.eye.vy)/2;
					view.eye.vz += (trg->pos.vz - view.eye.vz)/2;
					s = calc_dist(&view.eye, &trg->pos)*2;
					view.eye.vy -= f_totsu(s)*0x20/ONE;
					d.vx = trg->pos.vx - view.eye.vx;
					d.vy = trg->pos.vy - view.eye.vy;
					d.vz = trg->pos.vz - view.eye.vz;
					view.rot.vy = 0x400 - atan(d.vx, d.vz);
					view.rot.vx = 0x400 - atan(d.vy, csqrt(d.vx*d.vx + d.vz*d.vz) >> 6);
					view.rot.vz = 0;
					break;
		
		case  1:	/*** Virtual Button 1 (Top View) ***/
					/*** DRIVER'S EYES ***/
					view.eye = trg->pos;
					view.eye.vy -= 0xc+4+8;		/* 9/8 新データ構造より */
					view.rot = trg->rot;
					view.rot.vz = - view.rot.vz;
					break;

					view.eye = trg->pos;
					view.rot = trg->rot;
					view.eye.vy -= 0x140/4 + 0x200/64;
					view.eye.vx += rsin( view.rot.vy )*3/4 /8/4;
					view.eye.vz += rcos( view.rot.vy )*3/4 /8/4;
					view.rot.vx += 0x400;
					view.rot.vz = 0;

					if (job == JOB_MULTI)
					{
						view.eye.vy -= 0x800;
						view.rot.vx -= 0x0a0;
					} 
					else 
					{
						view.eye = trg->pos;
						view.rot = trg->rot;
						view.eye.vy -= 0x010;
						s = ONE/32;

						view.eye.vx += rsin(view.rot.vy+s)/0x16;
						printf("\nview.rot.vy = %d, s = %d, rsin(vy+s) = %d, MyRsin(vy+s) = %d\n", view.rot.vy, s, slow_rsin(view.rot.vy+s), rsin(view.rot.vy+s));
						getch();
						view.eye.vz += rcos(view.rot.vy+s)/0x16;
						printf("rcos(vy+s) = %d, MyRcos(vy+s) = %d\n", view.rot.vy, s, slow_rcos(view.rot.vy+s), rcos(view.rot.vy+s));
						getch();

						view.rot.vy += 0x800;
						view.rot.vx -= 0x008;
						view.rot.vz = 0;
					}
					break;
		
		case  2:	/*** Virtual Button 2 (Final Lap) ***/
					view.eye = trg->pos;
					view.rot = trg->rot;
					view.eye.vy -= 0x040;
					view.eye.vx -= rsin( view.rot.vy )/16;
					view.eye.vz -= rcos( view.rot.vy )/16;
					view.rot.vx += 0x040;	/* 0x200 = 45 degree */
					view.rot.vz = 0;
					break;

		case  3:	/*** Virtual Button 3 ( Front View ) ***/
					view.eye = trg->pos;
					view.rot = trg->rot;
					view.eye.vy -= 0x040;
					s = rsin(g_cnt*16) / 32;		/* 45 degrees */
					view.eye.vx += rsin(view.rot.vy+s)/12;
					view.eye.vz += rcos(view.rot.vy+s)/12;
					view.rot.vy += 0x800;
					view.rot.vx += 0x010;
					view.rot.vz = 0;
					break;

		case  4:	/*** Virtual Button 4 ( BACK ) ***/
					/*** heli ***/
					view.eye = heli.pos;
					view.eye.vy -= 0x1000;
					view.rot.vx = 0x0400;
					view.rot.vy = 0x0800 - heli.rot.vy;
					view.rot.vz = 0x0000;
					break;
		
		case  5:	/*** driver's eyes ***/
					view.eye = trg->pos;
					view.eye.vy -= 0xc+4+8;		/* 9/8 新データ構造より */
					view.rot = trg->rot;
					view.rot.vz -= mcar[CommMyCar].mycar_roll*2;
					break;
	}
	
	view_trans();
	if ((cn != 1) || (trg != &mycar[0]))
		if ((job != JOB_CDPLAY) && (job != JOB_DEMO)) add_car(&mycar[0], TRUE);
	action_swap(t);
}

//---------------------------------------------------------------------

// Check incoming CAR struct against all other CAR structs for collision
long	multi_check_car_hit(MULTI_CAR *mc)
{
	CAR				*mm = mc->c;
//	short			ii, jj;
	short			nn, jj;
	long			hit;
	long			tx, tz;
	long			t0x,t0z;
	long			c_rot, m_rot, d_rot;
	MULTI_CAR		*aa;
	CAR				*cc;				/* c_car */
	long			tmp1, tmp2;
	extern short	reverse_c_car;
						/***  right    left  ***/
	long			hit_tire[6][2] = {	{-19,22},{-19,-22},		/* rear */
										{ 35,24},{ 35,-24},		/* center *//*30->35*/
										{ 79,25},{ 79,-25} };	/* front */
	fn_track("multi_check_car_hit\n")

	aa = &mcar[0];
	hit = 0;
	for(nn=0; nn<M_CAR_MAX; nn++, aa++ ) {
		cc = aa->c;
		if((cc->use == TRUE) && (cc->u.c.status == 1) && (aa != mc)) {
			for(jj=0; jj<6; jj++) {	/* 0 r_r, 1 r_l, 2 f_r, 3 f_l */
				m_rot = mm->rot.vy & 0xfff;
				t0x = hit_tire[jj][1];
				t0z = hit_tire[jj][0];
				tx = (rcos(m_rot)*t0x + rsin(m_rot)*t0z)/ONE;
				tz = (-rsin(m_rot)*t0x + rcos(m_rot)*t0z)/ONE;
				tx += mm->pos.vx;
				tz += mm->pos.vz;	/* mycar tire position */

				c_rot = cc->rot.vy & 0xfff;
				t0x = tx - cc->pos.vx;
				t0z = tz - cc->pos.vz;
				tx = (rcos(c_rot)*t0x - rsin(c_rot)*t0z)/ONE;
				tz = (rsin(c_rot)*t0x + rcos(c_rot)*t0z)/ONE;

				if((cc->pos.vy -15 <= mm->pos.vy) && (cc->pos.vy +15 >= mm->pos.vy) &&
				   (-25 <= tx) && (tx <=  25) && (-21 <= tz) && (tz <=  80)) {		/* rear&front */
					if(jj >= 4) {	/* mycar front hit */
						d_rot = get_dry(c_rot, m_rot);
						if((-0x500 < d_rot) && (d_rot < 0x500)) {
							cc->hit = hit = TRUE;
							aa->hit_by = mc;
							cc->hvec = mm->u.m.dpos;
							if( cc->s > mm->s) cc->u.c.hit_ds = 2;
							else cc->u.c.hit_ds = -2;
							if(reverse_c_car == 1) cc->u.c.hit_ds *= -1;
							multi_set_swing(EFFECT_HIT, mc);
							break;
						}
						else {	/* mycar reverse hit */
							if(cc->u.c.speed < 100*8) {
								cc->hit = hit = TRUE;
								aa->hit_by = mc;
								multi_set_swing(EFFECT_HIT, mc);
								break;
							}
							else {
								cc->hit = TRUE;
								aa->hit_by = mc;
								cc->u.c.speed = cc->u.c.speed*2/3;
								cc->hvec.vx = cc->u.c.dpos.vx*256;
								cc->hvec.vz = cc->u.c.dpos.vz*256;
								if(cc->s > mm->s) cc->u.c.hit_ds = 6;
								else cc->u.c.hit_ds = -6;
								if(reverse_c_car == 1) cc->u.c.hit_ds *= -1;
								break;
							}
						}
					}
					else { /* mycar rear hit */
						cc->hit = TRUE;
						aa->hit_by = mc;
						cc->u.c.speed /= 2;
						cc->hvec.vx = cc->u.c.dpos.vx*256;
						cc->hvec.vz = cc->u.c.dpos.vz*256;
						vscale(&cc->hvec, 1/3);
						if(cc->s > mm->s) cc->u.c.hit_ds = 7;
						else cc->u.c.hit_ds = -7;
						if(reverse_c_car == 1) cc->u.c.hit_ds *= -1;
						break;
					}
				}
			} /* end for(4) loop */
		}
		if(hit == TRUE) break; // exit when found a collision
	}

	if(hit) se_play((mm->u.m.speed > 100*8) ? 23 : 24);	/* 車ヒット音 */
	return	hit;
}

//---------------------------------------------------------------------

void	multi_run_arong(MULTI_CAR *mc)
{
	CAR			*c = mc->c;
	MYCAR_BUF	*m = &c->u.m;
	long		l;		/* センターからの距離 */
	long		cry;	/* 道の方向 */
	long		try;	/* ターゲット方向 */
	long		tn;
	VECTOR		v;
	
	fn_track("multi_run_arong\n")

	/* 変数設定 */
	l = c->s;
	tn = c->t + (mc->spin_way ? SPIN_TARGET : -SPIN_TARGET);
	
	/* 数ステップ先のセンター座標を求める */
	multi_get_cxyz(tn, &v, mc);
	
	/* 数ステップ先の道の向きを求める */
	cry = multi_get_cry_dd(tn, mc);
	
	/* センターから垂直にｌだけ移動 */
	v.vx += rcos( cry )*l*32/16/ONE;
	v.vz -= rsin( cry )*l*32/16/ONE;
	
	try = 0x400 - atan( v.vx - c->pos.vx, v.vz - c->pos.vz );
	
	m->v_ry += get_dry_sign( m->v_ry, try )*2 / 10 ;	/*12*/
	
}

//---------------------------------------------------------------------

long	multi_in_road(long nx, long nz, long dir, long nt, MULTI_CAR *mc)
{
	long	rs,rc;
	long	t,tu,s,i,j,dy;
	COURSE	*p;
	long	hit,hhh;
	static	DVECTOR	tire[4] = {	{	-0x20,	0x10 *2 },
								{	 0x20,	0x10 *2 },
								{	-0x20,	-0x40 },
								{	 0x20,	-0x40 }, };
	VECTOR	h;
	
	fn_track("multi_in_road\n")
	
	tu = nt / 256;
	p = &mc->course_table[multi_crsu(tu)];
	rs = rsin(-dir);
	rc = rcos(-dir);
	for(i=hit=0; i<4; i++) {
		h.vx = nx + ( rc*tire[i].vx + rs*tire[i].vy)/ONE/2;
		h.vz = nz + (-rs*tire[i].vx + rc*tire[i].vy)/ONE/2;
		
		t = multi_get_t(&h, nt, mc);
		hhh = multi_road_hit(h.vx, h.vz, tu, p, &t, &s, mc);
		hit |= hhh;
		if(1 & hhh) j=i;
		
		mc->car_tire_pos[i].vx = h.vx;
		mc->car_tire_pos[i].vy = ((hit & 2) && (s < -125)) ? (s + 125) : 0;
		mc->car_tire_pos[i].vz = h.vz;
		
/*		h.vy = -0x40;	add_obj_p( &h, &rot_trans, 69, 0 );	*/
	}
	
	if(1 & hit) {
		dy = get_dry(multi_get_cry(nt, mc), dir);
		if((dy > ONE/8) && (dy < ONE*3/8) && (mc->pad == 0)) {	/* 正面! */
			se_play((mycar[0].u.m.speed > 100*8) ? 17 : 18);
		}
		else {
			if((j & 1) && (mc->pad == 0)) {	/* right? */
				se_play((mycar[0].u.m.speed > 100*8) ? 19 : 20);
			}
			else {		/* left? */
				if(mc->pad == 0) se_play((mycar[0].u.m.speed > 100*8) ? 21 : 22);
			}
		}
	}
	
	return (1 & hit);
}

//---------------------------------------------------------------------

long	multi_get_cry_dd(long t, MULTI_CAR *mc)
{
	long	c,d;
	
	fn_track("multi_get_cry_dd\n")

	c = multi_get_cry(t, mc);
	d = get_dir(get_dir(c, multi_get_cry(t-0x180, mc), 128),
				 get_dir(c, multi_get_cry(t+0x180, mc), 128), 128);
	return (d);
}

//---------------------------------------------------------------------

long	multi_get_cry(long t, MULTI_CAR *mc)
{
	COURSE	*p0,*p1;
	long	tu,r0,r1;
	
	fn_track("multi_get_cry\n")

	tu =  t >> 8;
	r0  = t & 0xff;
	r1  = 256 - r0;
	p0 = &mc->course_table[multi_crsu(tu)];
	p1 = &mc->course_table[multi_crsu(tu+1)];
	return (0x0800 - get_dir( p0->dir, p1->dir, r0));
}

//---------------------------------------------------------------------

void	multi_get_cxyz(long t, VECTOR *v, MULTI_CAR *mc)
{
	COURSE	*p0,*p1;
	long	tu,r0,r1;
	
	fn_track("multi_get_cxyz\n")

	tu =  t >> 8;
	r0  = t & 0xff;
	r1  = 256 - r0;
	p0 = &mc->course_table[multi_crsu(tu)];
	p1 = &mc->course_table[multi_crsu(tu+1)];
	v->vx = all_map_size - (r1*(p0->x/0x4000) + r0*(p1->x/0x4000))/256;
	v->vy =              - (r1* p0->height    + r0* p1->height   )/256/2;
	v->vz =                (r1*(p0->z/0x4000) + r0*(p1->z/0x4000))/256;
}

//---------------------------------------------------------------------

void	multi_set_camera_time_up(CAR *trg, long gc, MULTI_CAR *mc)
{
	VECTOR	d;
	long	s;
	
	fn_track("multi_set_camera_time_up\n")

	if(gc < 5*30) gc = 5*30;
	gc = (8*30) - gc;
	
	multi_get_cxyz(trg->t - gc*2, &d, mc);
/*	d = trg->pos;	*/
	
	d.vx += rsin(trg->rot.vy)/64;
	d.vz += rcos(trg->rot.vy)/64;
	d.vx += rsin(gc*32)*60/100/16;
	d.vz += rcos(gc*32)*60/100/16;
	d.vy += -0x3c-gc*0/100;
	
	time_up_camera.vx += (d.vx - time_up_camera.vx)/16;
	time_up_camera.vy += (d.vy - time_up_camera.vy)/16;
	time_up_camera.vz += (d.vz - time_up_camera.vz)/16;
	
	view.eye = time_up_camera;
	
/*	s = calc_dist( &view.eye, &trg->pos )*2;
	view.eye.vy -= f_totsu( s )*0x20/ONE;
*/	
	d.vx = trg->pos.vx - view.eye.vx;
	d.vy = trg->pos.vy - view.eye.vy;
	d.vz = trg->pos.vz - view.eye.vz;
	view.rot.vy = 0x400 - atan(d.vx, d.vz);
	view.rot.vx = 0x400 - atan(d.vy, csqrt(d.vx*d.vx + d.vz*d.vz) >> 6);
	view.rot.vz = 0;
	if(view.rot.vx >  80) view.rot.vx =  80;
	if(view.rot.vx < -80) view.rot.vx = -80;
	
	view_trans();
	add_car(&mycar[0], TRUE);
	action_swap(trg->t);
}

//---------------------------------------------------------------------

long	multi_wrong_way(MULTI_CAR *mc)
{
	CAR		*c = mc->c;
	long	ry,cy,dy;
	long	w;
	
	fn_track("multi_wrong_way\n")

	ry = 0x0800 - mc->course_table[multi_crsu(c->t >> 8)].dir;
	cy = c->u.m.v_ry;
	dy = (cy - ry) & 0x0fff;
	w = (dy > 0x0400) && (dy < 0x1000-0x0400);
	return (reverse_flag ? !w : w);
}

//---------------------------------------------------------------------

long	multi_get_t(VECTOR *pos, long last_tu, MULTI_CAR *mc)
{
	COURSE	*p0,*p1;
	VECTOR	c0,c1;
	VECTOR	v0,v1;
	VECTOR	u0,u1;
	long	stp,stm;
	long	tu,tl,hit;
	
	fn_track("multi_get_t\n")

	tu = last_tu / 256;
	while (TRUE) {
		p0 = &mc->course_table[multi_crsu(tu)];
		p1 = &mc->course_table[multi_crsu(tu+1)];
		get_line(pos, p0, &c0, &v0);
		get_line(pos, p1, &c1, &v1);
		stp = line_stat(&c0, &v0, 0, 0);
		stm = line_stat(&c0, &v0, (c0.vx+c1.vx)/2, (c0.vz+c1.vz)/2);
		if (stp == stm) break;
		tu--;
	}
	
	while (TRUE) {
		p0 = &mc->course_table[multi_crsu(tu)];
		p1 = &mc->course_table[multi_crsu(tu+1)];
		get_line(pos, p0, &c0, &v0);
		get_line(pos, p1, &c1, &v1);
		stp = line_stat(&c1, &v1, 0, 0);
		stm = line_stat(&c1, &v1, (c0.vx+c1.vx)/2, (c0.vz+c1.vz)/2);
		if (stp == stm) break;
		tu++;
	}
	
	return (tu << 8);
}

//---------------------------------------------------------------------

long	multi_xz2ts(VECTOR *pos, VECTOR *rot, long last_tu, long *t, long *s, MULTI_CAR *mc)
{
	COURSE	*p0,*p1;
	VECTOR	c0,c1;
	VECTOR	v0,v1;
	VECTOR	u0,u1;
	long	stp,stm;
	long	tu,tl,hit;
	long	dry,rx;
	
	fn_track("multi_xz2ts\n")

	tu = last_tu / 256;
	while (TRUE) {
		p0 = &mc->course_table[multi_crsu(tu)];
		p1 = &mc->course_table[multi_crsu(tu+1)];
		get_line(pos, p0, &c0, &v0);
		get_line(pos, p1, &c1, &v1);
		stp = line_stat(&c0, &v0, 0, 0);
		stm = line_stat(&c0, &v0, (c0.vx+c1.vx)/2, (c0.vz+c1.vz)/2);
		if (stp == stm) break;
		tu--;
	}
	
	while (TRUE) {
		p0 = &mc->course_table[multi_crsu(tu)];
		p1 = &mc->course_table[multi_crsu(tu+1)];
		get_line(pos, p0, &c0, &v0);
		get_line(pos, p1, &c1, &v1);
		stp = line_stat(&c1, &v1, 0, 0);
		stm = line_stat(&c1, &v1, (c0.vx+c1.vx)/2, (c0.vz+c1.vz)/2);
		if (stp == stm) break;
		tu++;
	}
	
	hit = 1 & multi_road_hit(pos->vx, pos->vz, tu, p0, &tl, s, mc);
	*t = (tu << 8)+tl;
	
	/*** set height ***/
	pos->vy = - ((256-tl) * p0->height + tl * p1->height)/256/2;
	
	/*** set pitch ***/
	dry = 0x0800 - get_dir(p0->dir, p1->dir, tl) - rot->vy;
	rx = get_dir(p0->pitch*8, p1->pitch*8, tl);
	if(rx > 0x07ff) rx = rx - 0x1000;
	rot->vx = rx * rcos(dry) / ONE;
	
	return (hit);
}

//---------------------------------------------------------------------

void	multi_ts2xz(long t, long s, VECTOR *pos, VECTOR *rot, MULTI_CAR *mc)
{
	long	tu;
	long	r0,r1;
	COURSE	*p0,*p1;
	
	fn_track("multi_ts2xz\n")

	tu =  t >> 8;
	r0  = t & 0xff;
	r1  = 256 - r0;
	p0 = &mc->course_table[multi_crsu(tu)];
	p1 = &mc->course_table[multi_crsu(tu+1)];
	
	/* t */
	pos->vx = all_map_size - (r1*(p0->x/0x4000) + r0*(p1->x/0x4000))/256;
	pos->vy = - (r1* p0->height + r0* p1->height)/256/2;
	pos->vz = (r1*(p0->z/0x4000) + r0*(p1->z/0x4000))/256;
	
	rot->vy = 0x0800 - get_dir(p0->dir, p1->dir, r0);
	rot->vx = get_dir(p0->pitch*8, p1->pitch*8, r0);
/*	rot->vz = get_dir( p0->bank, p1->bank, r0 );*/
	rot->vz = 0;
	
	/* s */
	pos->vx += rcos(rot->vy)*s*32/16/ONE;
	pos->vz -= rsin(rot->vy)*s*32/16/ONE;
}

//---------------------------------------------------------------------

long	multi_get_near_camera(MULTI_CAR *mc)
{
	CAR		*c = mc->c;
	long	i,m,md,d;
	VECTOR	*v;
	
	fn_track("multi_get_near_camera\n")

	m = 0;
	v = &camera_table[0];
	md = multi_get_distance(mc, c->t, v->pad);
	for (v++, i=1; v->pad != -1; i++, v++) {
		d = multi_get_distance(mc, c->t, v->pad);
		if(md > d) {
			m = i;
			md = d;
		}
	}
	return (m);
}

//---------------------------------------------------------------------

void	multi_set_camera_0(MULTI_CAR *mc, long cnt)
{
	CAR		*trg = mc->c;
	long	rz_buf,dx,dz;
	VECTOR	d;
	long	rc = 3*30 - cnt;
	long	rrc;
	
	fn_track("multi_set_camera0\n")

	/*** driver's eyes ***/
	view.eye = trg->pos;
	view.eye.vy -= 0xc+4+8;
	view.rot = trg->rot;
	view.rot.vz = - view.rot.vz;
	view.rot.vx += 0x000;
/*	view.rot.vy += rc*ONE/(2*30);	/* まわるまわる */
/*	view.rot.vy += 0x0800; */
/*	view.eye.vx += rc*32 ;
	view.eye.vz -= rc*8  ; */
	rrc = rc*rc/64;
	view.eye.vx -= rc*2*4;
	view.eye.vz += rc*2;
	view.eye.vy -= rrc*3;
	view_trans();
	action_swap(trg->t);
}

//---------------------------------------------------------------------

long	multi_get_distance(MULTI_CAR *mc, long t0, long t1)
{
	long	d;
	
	fn_track("multi_get_distance\n")

	t0 = multi_crs(t0);
	t1 = multi_crs(t1);
	if(t0 < t1) {
		d = t1 - t0;
		if(d > mc->road_size*256/2) d = mc->road_size*256 - d;
	}
	else {
		d = t0 - t1;
		if(d > mc->road_size*256/2) d = mc->road_size*256 - d;
	}
	return (d);
}

//---------------------------------------------------------------------

long	multi_check_tunnel(long t, MULTI_CAR *mc)
{
	fn_track("multi_check_tunnel\n")

	t = multi_crs(t);
	if((t < tun_a_start) && (t > tun_a_end)) {
		if(t > tun_a_start-256) {
			return (tun_a_start - t);
		}
		else if(t < tun_a_end+256) {
			return (t - tun_a_end);
		}
		else {
			return (256);
		}
	}
	if((t < tun_b_start) && (t > tun_b_end)) {
		if(t > tun_b_start-256) {
			return (tun_b_start - t);
		}
		else if(t < tun_b_end+256) {
			return (t - tun_b_end);
		}
		else {
			return (256);
		}
	}
	return (0);
}

//---------------------------------------------------------------------

void	multi_action_voice(MULTI_CAR *mc)
{
	CAR		*c = mc->c;
	long	i;
	
	fn_track("multi_action_voice\n")

	if (reverse_flag || mc->pad != 0) {
		/*** 無声 ***/
	}
	else {
		if(!(voice_trig & 0x0001)) {	/* 海岸線きついコーナー #9 */
			if(multi_crsl(c->t) == voice_tough) {
				voice_trig |= 0x0001;
				se_play(9);
			}
		}
		if(!(voice_trig & 0x0002)) {	/* テクニカルラスト１周目 #10 */
			if((multi_crsl(c->t) == voice_last) && (c->u.m.lap == 1)) {
				voice_trig |= 0x0002;
				se_play(10);
			}
		}
		if(!(voice_trig & 0x0004)) {	/* テクニカルラスト２周目 #10 */
			if((multi_crsl(c->t) == voice_last) && (c->u.m.lap == 2)) {
				voice_trig |= 0x0004;
				se_play(10);
			}
		}
		if(!(voice_trig & 0x0008)) {	/* テクニカルラスト３周目 #10 */
			if((multi_crsl(c->t) == voice_last) && (c->u.m.lap == 3)) {
				voice_trig |= 0x0008;
				se_play(10);
			}
		}
		if(game_level > 1) {
			for (i=0; i<4; i++) {
				if(!(voice_trig & (0x0010 << i))) {
					if((multi_crsl(c->t) == voice_C[i])				/* 通過 */
					 && (multi_crsl(c->u.m.last_t) == (voice_C[i]+0x0100))) {
						if(c->u.m.speed > c->u.m.max_speed*95/100) {
							voice_trig |= (0x0010 << i);
							se_play(2);
						}
						break;
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------

long	multi_road_hit(long x, long z, long tu, COURSE *p, long *tl, long *s, MULTI_CAR *mc)
{
	long	rs,rc;
	VECTOR	rp;
	long	l,m;
	long	rw;		/* 道幅 */
	long	t;
	
	fn_track("multi_road_hit\n")

	rs = rsin(-(0x0800 - p->dir) - 0x1000/4);
	rc = rcos(-(0x0800 - p->dir) - 0x1000/4);
	
	rp.vx = (x - (all_map_size - p->x/0x4000));
	rp.vz = (z - (               p->z/0x4000));
	
	l = ( rp.vx*rc + rp.vz*rs)*256/ONE/528;
	m = (-rp.vx*rs + rp.vz*rc)/ONE/2;
	
	if(l < 0) l = 0;
	else if(l > 255) l = 255;
	*tl = l;
	*s  = m;
	
	if(m > 0) {
		rw = p->r_side/32;		/* right hit check */
		t = multi_crs((tu << 8) + l);
	}
	else {
		rw = p->l_side/32;		/* left hit check */
		t = multi_crs((tu << 8) + l);
	/*	if (m < 0) {	/* アウト */
			if(((bank_a_start > t) && (bank_a_end < t))
			  || ((bank_b_start > t) && (bank_b_end < t))) {
				return (2 | (((m >= 0) ? m : -m) > rw+64));
			}
	/*	}*/
	}

	return (((m >= 0) ? m : -m) > rw);
}

//---------------------------------------------------------------------

void	multi_add_car(MULTI_CAR *mc, long model_change_flag)
{
	CAR			*c = mc->c;
	long		dist;
	long		dz = -16;
	MATRIX		rot,rot2;
	MATRIX		crt,srt,trt;	/* car, shadow, tire */
	SVECTOR		spos;
	VECTOR		tpos;
	VECTOR		d;
	VECTOR		shadow_pos;
	CAR_TYPE	*ct;
	long		l;
	long		tx;
	
	fn_track("multi_add_car\n")

	if(l = multi_check_tunnel(c->t, mc)) set_tunnel_light(l);
	
	ct = &car_type[c->model];
	
	if(model_change_flag) {
		dist = 0;
	}
	else {
		d.vx = c->pos.vx - view.eye.vx;
		d.vz = c->pos.vz - view.eye.vz;
		if(d.vx < 0) d.vx = -d.vx;
		if(d.vz < 0) d.vz = -d.vz;
		dist = d.vx+ d.vz;
	}
	
	if(dist < 0x0d00) {			/* near model */
		/*** make rotation matrix ***/
		set_rot_y(&rot, 0x0800 - c->rot.vy);	/* direction */
		set_rot_x(&srt, c->rot.vx);			/* pitch */
		MulMatrix2(&rot, &srt);
		rot = srt;
		MulMatrix2(&rot_trans, &rot);		/* Mul view matrix */
		
		shadow_pos.vx = c->pos.vx;
		shadow_pos.vy = c->shadow_y;
		shadow_pos.vz = c->pos.vz;
		add_obj_semi(&shadow_pos, &rot, ct->parts+2, dz+2);/*** shadow ***/
		add_obj_semi(&shadow_pos, &rot, ct->parts+2, dz+2);/*** shadow ***/
		
		set_rot_z(&crt, c->rot.vz);			/* bank */
		MulMatrix2(&rot, &crt);
		
		add_obj(&c->pos, &crt, ct->model[0], dz);		/*** body ***/
		if(c->model < 12) {	/* 悪魔カーは窓が分かれていない */
			add_obj(&c->pos, &crt, ct->parts, dz);		/* window */
		}
		add_obj(&c->pos, &crt, ct->parts+3, dz+2);	/* tire house */
		
		/*** rear tire ***/
		set_rot_z(&rot2, c->rot.vz/2);
		MulMatrix2(&rot, &rot2);
		set_rot_x(&trt, c->tire_rot.vz);		/* tire revolution */
		MulMatrix2(&rot2, &trt);
		if(c->tire_rot.vz & ONE) {
			tx = ct->tire_x;
		}
		else {
			tx = ct->parts+1;
		}
		add_obj(&c->pos, &trt, tx, dz);
		
		/*** front tire ***/
		spos.vx = 0;
		spos.vy = 0;
		spos.vz = ct->tire_game;			/* フロントタイヤの位置を変える */
		ApplyMatrix(&srt, &spos, &tpos);
		tpos.vx += c->pos.vx;
		tpos.vy += c->pos.vy;
		tpos.vz += c->pos.vz;
		add_obj( &tpos, &trt, tx, dz+2);
	}
	else if(dist < 0x2500) {		/* far model */
		/*** make rotation matrix ***/
		set_rot_y(&rot, 0x0800 - c->rot.vy);	/* direction */
		set_rot_x(&srt, c->rot.vx);			/* pitch */
		MulMatrix2(&rot, &srt);
		set_rot_z(&rot, c->rot.vz);			/* bank */
		MulMatrix2(&srt, &rot);
		MulMatrix2(&rot_trans, &rot);			/* Mul view matrix */
		add_obj(&c->pos, &rot, ct->model[1], dz);
	}
	
	if(l) set_normal_light();
}

//---------------------------------------------------------------------

long	multi_check_pause(void)
{
	int	i;

	fn_track("multi_check_pause\n")

	// Pause Control
	for(i=0; i<CommNumLinkedCars; i++) {
		if((mcar[i].game_stat >= 1) && (mcar[i].game_stat <= 3) && multi_start_trig(mcar[i].pad)) {
			pause_on = !pause_on;
			if(pause_on) {
				pause_y = 0;
				se_play(SE_CANCEL);
			}
			else {
				if(pause_y == 2) {			/* EXIT */
					CommDataFileOpen = -1;
					game_exit(JOB_SELECT0);
					init_seek();
					set_seek_wait();
					select_seek();
					replay_enable = FALSE;
				}
				else if(pause_y == 1) {	/* RESTART */
					game_exit(JOB_GAME0);
				}
				else  {
					pause_machine = -1;
					se_play(SE_CANCEL);	
				}
			}
			if(mcar[i].game_stat >= 2) pause_on ? cd_pause() : cd_depause();
			return (i); // let it happen once only and return machine paused
		}
	}
	return (-1); // no pause was detected on any link machine
}

//---------------------------------------------------------------------

void	multi_pause_menu(long id)
{
	long	t;

	fn_track("multi_pause_menu\n")

	if(pad_info[mcar[id].pad].trig & PAD_A) {
		if(pause_y > 0) {
			pause_y--;
			se_play(CLICK1);
		}
	}
	if(pad_info[mcar[id].pad].trig & PAD_B) {
		if (pause_y < 2) {
			pause_y++;
			se_play(CLICK1);
		}
	}
	job_cnt--;
	disp_pause(pause_y);
	switch (mcar[id].game_stat) {
		case 1:		disp_signal(job_cnt);
					break;
		case 2:		disp_signal(job_cnt);
					break;
		case 5:		t = mcar[id].goal_cnt-TIME_UP_TIME+32;
					if(t < 0) t = 0;
					disp_time_up(t);
					break;
		default:	break;
	}
	multi_set_camera(camera_num, camera_mtrg);
	multi_disp_indicator();
	multi_action_engine();
	meter(0, 0, 0);
	/*** set clipping map ***/
	clip_road();
	meter(0x00, 0xff, 0xff);
	multi_add_others();
	add_bldg();
	meter(0x00, 0x80, 0x00);	/* dark green */
	disp_c_car();
	meter(255, 255, 0);
	disp_heli();
	disp_jet();
	disp_ocar();
	disp_kick();
	add_sky();
	meter(0, 255, 0);
}

//---------------------------------------------------------------------
