/* new c_car routines for multi-player mode. */
// Tuesday 22/08/95 //
// c_car routines keyed into mycar[0] for their logic and movement
// this need to be changed in multi-mode because each machine treats
// mycar[0] differently (each machine has different car control.)

#include	"..\origsrc\common.h"
#define		VAR_EXTERN
#include	"..\origsrc\var.h"
#include	"..\origsrc\handle.h"
#include	"..\origsrc\sobj.h"

#define		_SIZE_T_DEFINED_
#include	"c:\watcom\h\stdio.h"

#include	"..\markk\multi.h"
#include	"..\markk\comm.h"
#include	"..\markk\newccar.h"
#include	"..\markk\grinfo.h"

//---------------------------------------------------------------------

void	multi_init_c_car(void)
{
	MULTI_CAR	*m;
	CAR			*c;
	LEVEL_PF	level;
	COURSE		*mycar_cs_table;
	long		*machine_type;
	long		mycar_road_size;
	long		i, rank;

	fn_track("multi_init_c_car\n")

	c = &c_car[CommNumLinkedCars-1];
	rank = 0;
	rand_cnt = rand();
	mycar_cs_table = course_table;
	mycar_road_size = road_size;
	if((game_level == 4) && (akumacar_flag == 1)) Satan_flag = 1;
	else Satan_flag = 0;

	if(job != JOB_CDPLAY) {
		reverse_c_car = reverse_flag;
		level.c_car_max = C_CAR_MAX;			/*** naito ***/
		if(game_level == 4)	machine_type = machine_type_T;
		else machine_type = machine_type_D;
		set_level_pform(&level);

		/* create individual c_cars. */
		for(i=CommNumLinkedCars-1; i<C_CAR_MAX; i++, c++) {
			set_cs_table(c);
			c->use = FALSE;
			if(i == C_CAR_MAX-1) {
				if(Satan_flag == 1) {
					c->use = TRUE;
					rank++;
					multi_create_c_car(c, i, machine_type);
					set_c_car_pform(c, i, machine_type, &level);
				}
			}
			else {
				if(machine_type[i] >= 0) {
					c->use = TRUE;
					rank++;
					multi_create_c_car(c, i, machine_type);
					set_c_car_pform(c, i, machine_type, &level);
				}
			}
		}

//		mycar[0].u.m.ranking = ALL_entry = rank + 1;  /* ranking (mycar) */
		ALL_entry = rank + CommNumLinkedCars;
		m = &mcar[0];
		for(i=0; i<CommNumLinkedCars; i++, m++) {
			c = m->c;
			c->u.m.ranking = ALL_entry - i;
		}
	}
	else if(job == JOB_CDPLAY) {
		reverse_c_car = 0;
		for(i=0; i<C_CAR_MAX; i++, c++) {		/* 12台分を必ず設定 */
			set_cs_table(c);
			machine_type = machine_type_CD;
			if(machine_type[i] >= 0) c->use = CD_PLAY;
			else c->use = FALSE;
			create_c_car(c, i, machine_type);
			set_c_car_pform_CD(c, i, machine_type);
		}
	}

	course_table = mycar_cs_table;
	road_size = mycar_road_size;
}

//---------------------------------------------------------------------

void	multi_create_c_car(CAR *c, long ii, long *mc_type)
{
	C_CAR_BUF	*b;
	VECTOR		dummy;

	fn_track("multi_create_c_car\n")

	b = &c->u.c;
	c->flag = TYPE_C_CAR;
	c->model = mc_type[ii];

// **** CHANGE **** //
	if(c->model == mycar[0].model) {
		if(mycar[0].model != 12)
			c->model = 0;		/* Change Default MyCar(F/A) */
	}
// **** CHANGE **** //

	c->hit = FALSE;
	c->hvec.vx = c->hvec.vy = c->hvec.vz = 0;

	if(c->use == TRUE) {
		if(reverse_c_car == 0) {
			b->status = RUN;
			b->machine_type = mc_type[ii];
			if(game_level < 3) {
				c->pos.vx = position_S[ii][0];
				c->pos.vz = position_S[ii][1];
				c->pos.vy -= position_S[ii][2];
				c->t = b->last_t = position_S[ii][3];
			}
			else {
				c->pos.vx = position_L[ii][0];
				c->pos.vz = position_L[ii][1];
				c->pos.vy -= position_L[ii][2];
				c->t = b->last_t = position_L[ii][3];
			}
			c->rot.vx = 0;
			c->rot.vy = 0x000004b0;
			c->rot.vz = 0;
			b->dpos.vx=b->dpos.vy=b->dpos.vz=0;
			b->drot.vx=b->drot.vy=b->drot.vz=0;

			b->std_dir = c->rot.vy;
			b->ds = 0;
			(void)xz2ts(&c->pos, &c->rot, b->last_t, &c->t, &c->s);
			set_def_flag(b, ii, c->model);
		}
		else { /* reverse_c_car == 1 */
			b->status = RUN;
			b->machine_type = mc_type[ii];
			if( game_level < 3 ) {
				c->pos.vx = position_RS[ii][0];
				c->pos.vz = position_RS[ii][1];
				c->pos.vy -= position_RS[ii][2];
				b->last_t = position_RS[ii][3];
			}
			else {
				c->pos.vx = position_RL[ii][0];
				c->pos.vz = position_RL[ii][1];
				c->pos.vy -= position_RL[ii][2];
				c->t = b->last_t = position_RL[ii][3];
			}

			c->rot.vx = 0;
			c->rot.vy = 0x056a;
			c->rot.vz = 0;

			b->dpos.vx=b->dpos.vy=b->dpos.vz=0;
			b->drot.vx=b->drot.vy=b->drot.vz=0;

			b->std_dir = c->rot.vy;
			if( game_level > 2 ) {
				b->last_t += 0x07000;
				c->t = b->last_t;
			}
			b->ds = 0;
			(void)xz2ts(&c->pos, &c->rot, b->last_t, &c->t, &c->s);
			set_def_flag(b, ii, c->model);
		}
	}
	else if(c->use == CD_PLAY) {

		b->status = RUN;
		b->machine_type = mc_type[ii];

		c->pos.vx = position_CD[ii][0];
		c->pos.vy = position_CD[ii][2];
		c->pos.vz = position_CD[ii][1];

		c->rot.vx = 0x00000000;
		c->rot.vy = 0x00000400;
		c->rot.vz = 0x00000000;

		c->t = b->last_t = position_CD[ii][3];
		(void)xz2ts(&c->pos, &c->rot, b->last_t, &c->t, &c->s);
		set_def_flag(b, ii, c->model);
	}
	else {	/* c->use == FALSE */
		b->status = RETIRE;	/* For Debug "くるくる" */
		b->machine_type = mc_type[ii];

		c->pos.vx = position_S[ii][0];
		c->pos.vy = position_S[ii][2];
		c->pos.vz = position_S[ii][1];
		c->rot.vx = 0;
		c->rot.vy = 0;
		c->rot.vz = 0;
		c->t = 0x23ba;
		b->flg.f_mycar_pos = 0;
		b->flg.f_lap = 0;
													/* data clear ?? */
	}
}

//---------------------------------------------------------------------

void 	multi_move_c_car(void)
{
	CAR 		*c;
	COURSE		*mycar_cs_table;
	VECTOR		npos, nrot;
	short		ii, entry, ranking;
	short		top_c_car, top_lap;
	long		new_t, new_s, top_t, tmp_ds;
	long		check_pnt, hide_spd;
	long		mycar_road_size;

	fn_track("multi_move_c_car\n")

 	/* マイカー用コーステーブル退避 */
	mycar_cs_table = course_table;	// save crs table
	mycar_road_size = road_size;	// save road size

	/*** move C_CAR ***/
	c = &c_car[CommNumLinkedCars-1];
	
	entry = ranking = 0;
	top_c_car = -1;
	top_lap = -1;
	Side_stop_car = NULL;
	if(reverse_c_car == 0) top_t = 0x20000;
	else if(reverse_c_car == 1) top_t = -0x00001;

	multi_mycar_position();

	// ----------------------------------------------------------------------------------
	
	for(ii=CommNumLinkedCars-1; ii<C_CAR_MAX; ii++, c++) {
		set_cs_table(c);
		if(c->use == TRUE) {
			// new function to locate nearest link car for each of c_cars
			multi_get_nearest_link_car(c->pnt_mc);
			
			if(ii == 0 || ii == 11) rival_flag = 1;
			else rival_flag = 0;
			entry++;
			if(reverse_c_car == 0) {
				if((crsl(c->t) == 0x2700) && (crsl(c->u.c.last_t) == 0x2700+0x100)) {
					c->u.c.flg.f_lap++;
				}
			}
			else {
				if(course_table == easy_course_table) check_pnt = 0x0b900;
				else check_pnt = 0x012900;
				if((crsl(c->t) == check_pnt) &&	(crsl(c->u.c.last_t) == check_pnt-0x100)) {
					c->u.c.flg.f_lap++;
				}
			}

			if(mcar[CommMyCar].game_stat < 4) mycar_ranking(c, ii);

			/*** Check Retire C_Car ***/
			if(game_level != 4)	hide_c_car(c, ii);

			/***** 走行中の車の処理 *****/
			if(c->u.c.status == RUN) {
				main_straight(c);
				if(c->u.c.flg.f_pass1 == -3 && ii != 11) start_after_flag(c, ii);
				direction_c_car(c);
				multi_sch_front_car(c, ii);
				if(ii == 11) reset_direction_SATAN(c);
				multi_pass_front_car2(c, ii);
				handle_c_car(c);
				if(mcar[CommMasterCar].game_stat >= 2) multi_set_speed_c_car(c, ii);
				roll_c_car(c);
				drive_c_car(c);
				if(c->hit == FALSE) {
					if(c->u.c.hit_drot != 0) reset_hit_rot(c);
					action_jump_c(c);
				}
				/*** c_car been hit consequences ***/
				else if(c->hit == TRUE) {
					multi_hit_c_car(c->pnt_mc);
					if(c->u.c.flg.f_pass1 != -3) {
						npos.vx = c->pos.vx + c->hvec.vx/256;
						npos.vz = c->pos.vz + c->hvec.vz/256;
						npos.vy = c->pos.vy + c->hvec.vy/256/8;
						nrot.vy = c->rot.vy + c->u.c.hit_drot;
					}
					else {
						npos.vx = c->pos.vx;
						npos.vz = c->pos.vz;
						nrot.vy = c->rot.vy;
					}
					vscale(&c->hvec, 3/4);
					xz2ts(&npos, &nrot, c->u.c.last_t, &new_t, &new_s);
					if(reverse_c_car == 0) {
						if(new_s > R_Side || new_s < L_Side) {
							set_lane_c_car(c, new_s);
							tmp_ds = c->u.c.ds;
						}
						else tmp_ds = c->u.c.hit_ds;
					}
					else if(reverse_c_car == 1) {
						if(new_s > L_Side || new_s < R_Side) {
							set_lane_c_car(c, new_s);
							tmp_ds = c->u.c.ds;
						}
						else tmp_ds = c->u.c.hit_ds;
					}
					npos.vx += rcos(nrot.vy)*tmp_ds*2/ONE;
					npos.vz -= rsin(nrot.vy)*tmp_ds*2/ONE;
					c->pos = npos;
					c->pos.vy -= 12;
					c->shadow_y = c->pos.vy;
					c->rot = nrot;
					c->rot.vz = 0;
					c->tire_rot.vz = (c->tire_rot.vz + c->u.c.speed) & 0xfff;
					if(c->u.c.speed > 100*8) c->tire_rot.vz |= ONE;
				}
				if(mcar[CommMyCar].game_stat < 4) multi_check_chase(c);
			}
			/***** 見えない車の処理 *****/
			else if(c->u.c.status == HIDE) {
				if(mcar[CommMyCar].game_stat < 2) c->u.c.speed = 0;
				else {
					c->u.c.flg.f_condition = 0;
					if(c->u.c.speed < c->u.c.max_speed*8/10) c->u.c.speed += c->u.c.start_acl;
				}
				if(c->u.c.flg.f_pass1 == -3) start_after_flag(c, ii);
				direction_c_car(c);
				multi_sch_front_car(c, ii);
				lane_keep_c_car(&c->u.c, (c->t&0xff));
				c->pos.vx += (c->u.c.speed*rsin(c->u.c.std_dir)/256)/256;
				c->pos.vz += (c->u.c.speed*rcos(c->u.c.std_dir)/256)/256;
				xz2ts(&c->pos, &c->rot, c->u.c.last_t, &new_t, &new_s);
				c->rot.vy = c->u.c.std_dir;
				c->u.c.last_t = c->t;
				c->t = crs(new_t);
				c->s = new_s;
			}
			/***** リタイヤした車の処理 *****/
			/*** 調整必要 ***/
			else { /* status == RETIRE */
				c->u.c.flg.f_mycar_pos = 0;
			}
			if(mcar[CommMyCar].game_stat < 4) {
				if(c->u.c.flg.f_mycar_pos == 1) ranking++;
				if(check_top_c_car(c, &top_lap, &top_t, ii) == 1) top_c_car = ii;
			}
		}
		else { /* use = FALSE */
			c->hit = FALSE;
			c->hvec.vx = c->hvec.vy = c->hvec.vz = 0;
			c->u.c.flg.f_mycar_pos = 0;
		}

		if(c->u.c.flg.f_condition == -2) Side_stop_car = c;
	} /* end <for> loop */

// **** CHANGE **** //
	if(mcar[CommMyCar].game_stat < 4) {
		mycar[0].u.m.ranking = ALL_entry - ranking;
		TOP_car = multi_check_top_car(top_c_car);
	}
// **** CHANGE **** //

 	/* マイカー用コーステーブル設定 */
	course_table = mycar_cs_table;
	road_size = mycar_road_size;
}

//---------------------------------------------------------------------

void	multi_hit_c_car(MULTI_CAR *mc)
{
	CAR			*c = mc->c;
	short		ii, hit_f;
	long		rot, tmp;
	long		dd = 40;
	long		dx;
	MULTI_CAR	*b = mc->hit_by;
	CAR			*bb = b->c;
//	extern long rebound_event;

	fn_track("multi_hit_c_car\n")

	if(c->u.c.flg.f_drift == 1) rot = 3;
	else rot = 2;
	if(c->u.c.flg.f_hit_lock >= 1) {
		c->u.c.speed -= c->u.c.speed/50;	/* -2% */
		rot = rot * c->u.c.flg.f_hit_lock;
		c->u.c.flg.f_hit_lock--;
	}
	else {
		c->hit = FALSE;
		c->u.c.flg.f_hit_lock = 20;
	}
	if(b->rebound_event == FALSE) rot = 0; // no rebound means no hit rotation
	if(c->u.c.flg.f_direction == RIGHT_TURN) c->u.c.hit_drot += rot;
	else if(c->u.c.flg.f_direction == LEFT_TURN ) c->u.c.hit_drot -= rot;
	else {
//		if(c->s > bb->s) c->u.c.hit_drot += rot;
		if(c->s > bb->s) c->u.c.hit_drot += rot;
		else c->u.c.hit_drot -= rot;
	}
	if(c->u.c.flg.f_pass1 >= 0) c->u.c.flg.f_pass1 = -1;
}

//---------------------------------------------------------------------

void	multi_sch_front_car(CAR *c, short ii)
{
	short	jj;
	short	ret1;
	short	ret2;
	short	cond;
	long	tmp1;
	long	find;
	long	away;
	long	half;
	long	full;

	// new locals
	int			i;
	long		tmp2;
	long		nearest;
	MULTI_CAR	*mm;
	CAR			*cc;

	fn_track("multi_sch_front_car\n")

	ret1 = ret2 = -1;
	cond = 0;
	find = FIND_DIST;
	away = AWAY_DIST;
	if(game_level > 2) {
		full = 0x17000;
		half = 0xb800;
	}
	else {
		full = 0x10000;
		half = 0x8000;
	}

	/* check front C_Car */
	for(jj=CommNumLinkedCars-1; jj<C_CAR_MAX; jj++) {
		if((ii != jj) && (c_car[jj].use != FALSE)) {
			if(reverse_c_car == 0) {
				tmp1 = c->t - c_car[jj].t;
				if(tmp1 < -0xf000) tmp1 += full;
			}
			else if(reverse_c_car == 1) {
				tmp1 = c_car[jj].t - c->t;
				if(tmp1 < -0xf000) tmp1 += full;
			}

			if((0 < tmp1) && (tmp1 < FIND_DIST) && (find > tmp1)) {
				ret1 = jj;
				find = tmp1;
				if((tmp1 < CHASE_DIST)) {
					if(c->u.c.flg.f_back_car != jj) cond = 1; /* Chase Speed */
				}
			}
		}
	}

	/* check back C_Car */
	for(jj=CommNumLinkedCars-1; jj<C_CAR_MAX; jj++) {
		if(c->use != FALSE) {
			if(reverse_c_car == 0) {
				tmp1 = c->t - c_car[jj].t;
				if(tmp1 > 0xf000) tmp1 = full - tmp1;
			}
			else if(reverse_c_car == 1) {
				tmp1 = c_car[jj].t - c->t;
				if(tmp1 > 0xf000) tmp1 = full - tmp1;
			}

			if((ii != jj) && (c_car[jj].use != FALSE)) {
				if((0 > tmp1) && (tmp1 > AWAY_DIST) && (away < tmp1)) {
					away = tmp1;
					ret2 = jj;
				}
			}
		}
	}

	// new code MARK (22/08/95)
	// need to get closest distance from ALL linked cars to this c_car
	c->u.c.c_car_dist = FIND_DIST;
	if(reverse_c_car == 0) {

		tmp1 = c->pnt_mc->near_dist;
//		tmp1 = c->t - mycar[0].t;

		if(tmp1 >= 0) tmp1 = (tmp1 >= half) ? -(full-tmp1) : tmp1;
		else tmp1 = (-tmp1 <= half) ? tmp1 : full+tmp1;
		c->u.c.mycar_dist = tmp1;
	}
	else if(reverse_c_car == 1) {

		tmp1 = -c->pnt_mc->near_dist;
//		tmp1 = mycar[0].t - c->t;

		if(tmp1 >= 0) tmp1 = (tmp1 >= half) ? -(full-tmp1) : tmp1;
		else tmp1 = (-tmp1 <= half) ? tmp1 : full+tmp1;
		c->u.c.mycar_dist = tmp1;
	}

	if((0 < tmp1) && (tmp1 < FIND_DIST)) {
		if(ret1 == -1) {
			ret1 = MYCAR_NUMBER;
			cond = 1;
		}
		else {
			if(tmp1 < NEAR_DIST) {
				ret1 = MYCAR_NUMBER;
				cond = 3; /*** Down Speed ***/
			}
		}
	}
	else if((0 >= tmp1) && (tmp1 > AWAY_DIST)) {
		if(ret2 == -1) {
			ret2 = MYCAR_NUMBER;
			cond = 2;
		}
		else {
			if(away > tmp1) {
				ret2 = MYCAR_NUMBER;
				cond = (cond == 0) ? 2 : 1;
			}
		}
	}

	if(c->u.c.flg.f_pass1 != -3) {
		if(ret1 == -1) c->u.c.c_car_dist = away;
		else c->u.c.c_car_dist = find;
	}

	c->u.c.flg.f_front_car = ret1;
	c->u.c.flg.f_back_car = ret2;

	if(c->u.c.flg.f_condition >= 0) {
		c->u.c.flg.f_condition = cond;
	}
}

//---------------------------------------------------------------------

void	multi_mycar_position(void)
{
	long ch_pnt, mt;
	long cs_long;

	fn_track("multi_mycar_position\n")

	if( game_level < 3 ) cs_long = 0x10000;
	else                 cs_long = 0x17000;

	if( reverse_c_car == 0 ){
		ch_pnt = 0x2700+0x100;
		if( mycar[0].t >= ch_pnt ) mt = (cs_long + ch_pnt) - mycar[0].t;
		else                       mt = ch_pnt - mycar[0].t;

		mycar_p = mt + (cs_long * mycar[0].u.m.lap-mycar[0].u.m.unlap);
	}
	else if( reverse_c_car == 1 ){
		if( game_level > 2 ) ch_pnt = 0x12900;
		else                 ch_pnt = 0xb900;

		if( mycar[0].t >= ch_pnt ) mt = mycar[0].t - ch_pnt;
		else                       mt = cs_long - ch_pnt + mycar[0].t;

		mycar_p = mt + (cs_long * mycar[0].u.m.lap-mycar[0].u.m.unlap);
	}
/*debug[4]=mycar[0].u.m.lap<<28 | mycar_p;
debug[5]=mycar[0].t;*/
}

//---------------------------------------------------------------------

void	multi_set_passing_wide(CAR *c, short ii, short flag)
{
	short c_num1, r_lane;
	long tg_s, lr_wide, center_tg;
	long tmp1, tmp2, m_side, p_side;
	COURSE *p;
	C_CAR_BUF *cc = &c->u.c;

	fn_track("multi_set_passing_wide\n")

	p = &course_table[ crsu(c->t>>8) ];
	lr_wide = p->l_side+p->r_side;
	center_tg = p->d_center;
	r_lane = rand_cnt%3;

	side_x_side( cc, c->t );

  /*** 通常走行での目標レーン設定 ***/
	if( flag == 0 ){
		if( lr_wide > 0x2000 ){
			if     ( (ii+r_lane)%3 == 2 ) tg_s = LANE_L;
			else if( (ii+r_lane)%3 == 1 ) tg_s = LANE_R;
			else                          tg_s = CENTER + center_tg;

			if( ii == 11 )
				tg_s = LANE_L;
		}
		else{
			if( (c_num1=cc->flg.f_front_car) >= 0 ){
				if( c_car[c_num1].s > 0 ) tg_s = c_car[c_num1].s -10;
				else                      tg_s = c_car[c_num1].s +10;
			}
			else{
				if( cc->flg.f_direction == CENTER ){
					if( cc->flg.f_drift == 1 ){
						if( ii%2 == 1 ) tg_s = L_Side/3 + ii;
						else            tg_s = R_Side/3 - ii;
					}
					else
						tg_s = CENTER + center_tg;
				}
				else{
					if( cc->flg.f_direction == RIGHT_TURN )
						tmp1 = L_Side;
					else
						tmp1 = R_Side;

					if( reverse_c_car == 1 ){
						if( game_level == 1 ) tg_s = tmp1/3;
						else                  tg_s = tmp1/4;
					}
					else{
						if( game_level == 4 ) tg_s = tmp1/4;
						else                  tg_s = tmp1/2;
					}
				}
			}
		}

		if( cc->flg.f_curve_lock == 0 )
			cc->target_s = tg_s;
	}
  /*** マイカー追越しでの目標レーン設定(step 1) ***/
	else if( flag == 1 ){
		if( reverse_c_car == 0 ){		/* naito */
			m_side = L_Side;
			p_side = R_Side;
		}
		else{
			m_side = R_Side;
			p_side = L_Side;
		}

		c_num1 = cc->flg.f_front_car;
//		if( c_num1 == MYCAR_NUMBER ) tg_s = mycar[0].s;
		if( c_num1 == MYCAR_NUMBER ) tg_s = c->pnt_mc->near_car->c->s;
		else                         tg_s = c_car[c_num1].s;

		if( tg_s > 0 ){
			if( tg_s > c->s ){
				tmp1 = tg_s - 60;
				tmp2 = (tg_s + m_side)/2;
				if( c_num1 == MYCAR_NUMBER )
					c->u.c.target_s = tmp2;
				else{
					if( c->s > tmp1 ) c->u.c.target_s = tmp1;
					else              c->u.c.target_s = tmp2;
				}
			}
			else{
				tmp1 = plus_dist( tg_s, p_side );
				if( tmp1 > 60 )	c->u.c.target_s = (tg_s + p_side)/2;
				else{
					c->u.c.target_s = (tg_s + m_side)/2;
/*					c->u.c.flg.f_condition = 4;*/		/* naito */
					c->u.c.flg.f_condition = 3;
				}
			}
		}
		else{ /* tg_s < 0 */
			if( tg_s < c->s ){
				tmp1 = tg_s + 60;
				tmp2 = (tg_s + p_side)/2;
				if( c_num1 == MYCAR_NUMBER ){
					c->u.c.target_s = tmp2;
				}
				else{
					if( c->s < tmp1 ) c->u.c.target_s = tmp1;
					else              c->u.c.target_s = tmp2;
				}
			}
			else{
				tmp1 = plus_dist( tg_s, m_side );
				if( tmp1 > 60 ) c->u.c.target_s = (tg_s + m_side)/2;
				else{
					c->u.c.target_s = (tg_s + p_side)/2;
/*					c->u.c.flg.f_condition = 4;*/		/* naito */
					c->u.c.flg.f_condition = 3;
				}
			}
		}

		if( reverse_c_car == 0 ) tmp1 = 10;
		else                     tmp1 = -10;
		if( (c->u.c.target_s > R_Side - tmp1) ||
                  (c->u.c.target_s < L_Side + tmp1) ){
			c->u.c.flg.f_condition = 4;
			c->u.c.target_s = CENTER;
			c->u.c.flg.f_pass1 = -1;
		}

			/*** need tunnel patch             naito ***/

	}
}

//---------------------------------------------------------------------

void	multi_pass_front_car2(CAR *c, long ii)
{
	short target, passing, danger;
	long dist, f_speed, f_s;
	C_CAR_BUF *cc = &c->u.c;

	fn_track("multi_pass_front_car2\n")

	passing = cc->flg.f_pass1;
	target = cc->flg.f_front_car;
	danger = 0;

  /*** default ***/
	if( passing == -1 ){
		if( target != -1 ){
			if( target == MYCAR_NUMBER ){
				dist = cc->mycar_dist;
//				f_speed = mycar[0].u.m.speed;
				f_speed = c->pnt_mc->near_car->c->u.m.speed;
			}
			else{
				dist = cc->c_car_dist;
				f_speed = c_car[target].u.c.speed;
			}

			if( cc->speed > f_speed ){
				if( (FRNT_DIST > dist) ){
					cc->flg.f_pass1 = target;
					cc->flg.f_curve_lock = cc->flg.f_direction;
					if( cc->flg.f_direction == CENTER ){
						cc->flg.f_curve_lock = 1;
					}
					if( target != MYCAR_NUMBER ){
						if( c_car[target].u.c.flg.f_pass1 == -1 )
							c_car[target].u.c.flg.f_pass1 = -2;
						else{
							cc->flg.f_pass1 = -1;
							cc->flg.f_condition = 4;
						}
					}
				}

				multi_set_passing_wide( c, ii, 1 );
			}
			else{
				if( cc->c_car_dist < DANGER_DIST ){
					cc->target_s = c->s;
					cc->flg.f_curve_lock = 1;
				}
				else
					multi_set_passing_wide( c, ii, 0 );
			}
		}
		else{
			if( cc->flg.f_back_car >= 0 ){
				cc->target_s = c->s;
				if( cc->flg.f_direction == STRAIGHT )
					cc->flg.f_curve_lock = 1;
				else
					cc->flg.f_curve_lock = cc->flg.f_direction;
			}
			else
				multi_set_passing_wide( c, ii, 0 );
		}
	}
  /*** passed another c_car ***/
	else if( passing == -2 ){
		cc->target_s = c->s;
		if( cc->flg.f_direction == STRAIGHT )
			cc->flg.f_curve_lock = 1;
		else
			cc->flg.f_curve_lock = cc->flg.f_direction;

	  /*** End Pass ***/
		dist = cc->c_car_dist;
		if( cc->flg.f_back_car == -1 ){
			cc->flg.f_pass1 = -1;
			cc->flg.f_condition = 0;
		}
		else if( c_car[cc->flg.f_back_car].u.c.flg.f_pass1 != ii ){
			cc->flg.f_pass1 = -1;
			cc->flg.f_condition = 0;
		}
	  /*** Imcomplete Pass ***/
	}
  /*** start ***/
	else if( passing == -3 ){
		if( cc->flg.f_condition <= -1 )
			multi_set_passing_wide( c, ii, 0 );
	}
  /*** pass front car ***/
	else{
		if( target == MYCAR_NUMBER ){
			dist = cc->mycar_dist;
//			f_speed = mycar[0].u.m.speed;
				f_speed = c->pnt_mc->near_car->c->u.m.speed;
//			f_s = mycar[0].s;
			f_s = c->pnt_mc->near_car->c->s;
		}
		else{
			dist = cc->c_car_dist;
			f_speed = c_car[target].u.c.speed;
			f_s = c_car[target].s;
		}

	  /*** imcomplete pass ***/
		if( (dist > FRNT_DIST) || (f_speed > cc->speed) ){
			cc->flg.f_pass1 = -1;
			cc->flg.f_condition = 0;
		}
		else if( (dist < DANGER_DIST) && (45 > plus_dist( c->s, f_s )) ){
			cc->flg.f_pass1 = -1;
			cc->flg.f_condition = 4;
		}
		else if( cc->flg.f_back_car != -1 ){
			if( (target != MYCAR_NUMBER) &&
                    (cc->speed < c_car[target].u.c.speed) ){
				cc->flg.f_condition = 3;
			}
		}
		else if( ((game_level <= 2) &&
                     (crsl(c->t) >= 0x9500) && (crsl(c->t) <= 0x9c00)) ||
				 ((game_level >= 3) &&
                     (crsl(c->t) >= 0x8200) && (crsl(c->t) <= 0x8e00)) ){
			cc->flg.f_pass1 = -1;
			cc->flg.f_condition = 3;
		}

		multi_set_passing_wide( c, ii, 1 );
	}
}

//---------------------------------------------------------------------

void	multi_check_chase(CAR *c)
{
	fn_track("multi_check_chase\n")

	/*** Calling!! ***/
	if( c->u.c.flg.f_chase_mycar >= 0 ){
		chase_voice( c );
	}
	/*** Check Call ***/
	else if( (c->u.c.flg.f_front_car == MYCAR_NUMBER) &&
				(c->u.c.flg.f_condition == 1) &&
//					(mycar[0].u.m.speed/8 > 150) &&
					(c->pnt_mc->near_car->c->u.m.speed/8 > 150) &&
//						(mycar[0].u.m.speed/8 < c->u.c.speed/8) ){
						(c->pnt_mc->near_car->c->u.m.speed/8 < c->u.c.speed/8) ){
		chase_voice( c );
	}
	/*** Set Value ***/
	else{
		if( (c->u.c.flg.f_chase_mycar < CHECK_TIME*2) &&
                (c->u.c.flg.f_front_car != MYCAR_NUMBER) ){
			c->u.c.flg.f_chase_mycar = CHECK_TIME;
		}
	}
}

//---------------------------------------------------------------------

void	multi_special_speed(CAR *c, long *spd, long *acl)
{
	long dist;
	short race_cnt;

	fn_track("multi_special_speed\n")

	if( c->u.c.flg.f_straight != 2 ){
	  /*** too far front ***/
		if( akumacar_flag == 0 )
//			dist = 0x300 * mycar[0].u.m.lap;
			dist = 0x300 * c->pnt_mc->near_car->c->u.m.lap;
		else
			dist = 0x800;

//		if( ((c->u.c.flg.f_lap == mycar[0].u.m.lap) &&
		if( ((c->u.c.flg.f_lap == c->pnt_mc->near_car->c->u.m.lap) &&
                           (c->u.c.mycar_dist > dist)) ||
//                             (c->u.c.flg.f_lap < mycar[0].u.m.lap) ){
                             (c->u.c.flg.f_lap < c->pnt_mc->near_car->c->u.m.lap) ) {
			*spd = c->u.c.chase_spd;
			*spd += (*spd)/100;		/* +100% */
			*acl = c->u.c.paccel;
		}
	  /*** too far back ***/
//		else if( ((c->u.c.flg.f_lap == mycar[0].u.m.lap) &&
		else if( ((c->u.c.flg.f_lap == c->pnt_mc->near_car->c->u.m.lap) &&
                                (c->u.c.mycar_dist < -0x600)) ||
//                    (c->u.c.flg.f_lap > mycar[0].u.m.lap) ){
                    (c->u.c.flg.f_lap > c->pnt_mc->near_car->c->u.m.lap) ) {
			*spd = (*spd > c->u.c.max_speed)? c->u.c.max_speed: *spd;
			*spd = (*spd) * 8/10;					/* -20% */
			if( *spd > c->u.c.speed ) *acl = c->u.c.paccel;
			else                      *acl = c->u.c.maccel;
		}
		c->u.c.final_spd = 0;
	}
	else if( c->u.c.flg.f_straight == 2 ){
		if( c->u.c.mycar_dist > 0 ){
		  /*** 最終ストレート（マイカーに逃げられる）***/
//			*spd = mycar[0].u.m.max_speed;
			*spd = c->pnt_mc->near_car->c->u.m.max_speed;
			*spd -= (*spd)*3/100;				/* -3% */
			if( c->u.c.speed > *spd ) *acl = c->u.c.maccel;
			c->u.c.final_spd = *spd;
		}
		else{
		  /*** 最終ストレート（マイカーに抜かされる）***/
//			*spd = mycar[0].u.m.max_speed;
			*spd = c->pnt_mc->near_car->c->u.m.max_speed;
			*spd -= (*spd)*13/100;				/* -13% */
			if( c->u.c.speed > *spd ) *acl = c->u.c.maccel;
			c->u.c.final_spd = *spd;
		}
	}

  /* easy -> hard version */
	if( akumacar_flag == 0 ){
		if( game_level == 1 ) race_cnt = 2;
		else                  race_cnt = 3;

//		if( mycar[0].u.m.lap == race_cnt ){
		if( c->pnt_mc->near_car->c->u.m.lap == race_cnt ){
			if( *spd > c->u.c.max_speed ){
				*spd = c->u.c.max_speed;
				if( *spd < c->u.c.speed ) *acl = c->u.c.maccel;
			}
			if( c->u.c.final_spd != 0 )
//				c->u.c.final_spd = mycar[0].u.m.max_speed*9/10;
				c->u.c.final_spd = c->pnt_mc->near_car->c->u.m.max_speed*9/10;
		}
	}
}

//---------------------------------------------------------------------

short	multi_check_top_car(short c_car_number)
{
	CAR *c = &c_car[c_car_number];
	long tmp1, tmp2;
	short ret = 0;

	fn_track("multi_check_top_car\n")

	if( c->u.c.flg.f_lap > mycar[0].u.m.lap )
		ret = c->model;							/* TOP c_car */
	else if( c->u.c.flg.f_lap < mycar[0].u.m.lap )
		ret = mycar[0].model;							/* TOP mycar */
	else if( c->u.c.flg.f_lap == mycar[0].u.m.lap ){
		if( reverse_c_car == 0 ){
			if( (tmp1=c->t) < 0x02800 ){
				if( game_level > 2 ) tmp1 += 0x17000;
				else                 tmp1 += 0x10000;
			}
			if( (tmp2=mycar[0].t) < 0x02800 ){
				if( game_level > 2 ) tmp2 += 0x17000;
				else                 tmp2 += 0x10000;
			}

			if( tmp1 < tmp2 ) ret = c->model;		/* TOP c_car */
			else              ret = mycar[0].model;		/* TOP mycar */
		}
		else if( reverse_c_car == 1 ){
			if( (tmp1=c->t) < 0x0b100 ){
				if( game_level > 2 ) tmp1 += 0x17000;
				else                 tmp1 += 0x10000;
			}
			if( (tmp2=mycar[0].t) < 0x0b100 ){
				if( game_level > 2 ) tmp2 += 0x17000;
				else                 tmp2 += 0x10000;
			}

			if( tmp1 > tmp2 ) ret = c->model;		/* TOP c_car */
			else              ret = mycar[0].model;		/* TOP mycar */
		}
	}

	return( ret );
}

//---------------------------------------------------------------------

void	multi_set_speed_c_car(CAR *c, short ii)
{
 	long acl, spd, t0;
	long tmp1, tmp2, pcnt;
	C_CAR_BUF *cc = &c->u.c;

	fn_track("multi_set_speed_c_car\n")

  /**************************/
  /*** Set Straight Speed ***/
  /**************************/

  /*** Set Start Speed ***/
	if( cc->flg.f_condition == -1 )
		acl = cc->start_acl;
	else
		acl = cc->paccel;

  /*** Set Chase Speed ***/
	if( cc->flg.f_condition == 1 ){
		spd = cc->chase_spd;
		if( (cc->flg.f_front_car == MYCAR_NUMBER) &&
                              (cc->flg.f_straight == 1) ){
			spd = cc->chase_spd + cc->chase_spd/10;		/* +10% */
		}
	}
  /*** Set Away Speed ***/
	else if( cc->flg.f_condition == 2 ){
		spd = cc->away_spd;
		if( (rival_flag == 1) && (cc->flg.f_straight == 1) )
			spd += spd/14;			/* +7% */
	}
  /*** Set Rendzvous Speed ***/
	else if( cc->flg.f_condition == 3 ){
		acl = cc->maccel;
		if( cc->flg.f_front_car >= 0 ){
			spd = c_car[cc->flg.f_front_car].u.c.speed;
			spd += spd/15;				/* +6% */
			if( spd > cc->max_speed ) spd = cc->max_speed;
			spd = (spd > 70*8)? spd: 70*8;
		}
		else
			spd = cc->speed * 8/10;
	}
  /*** Set Emergency Speed ***/
	else if( cc->flg.f_condition == 4 ){
		acl = cc->maccel;
	  /*** (front car -> 90% speed)  (80% speed) ***/
		if( cc->flg.f_front_car >= 0 ){
			spd = c_car[cc->flg.f_front_car].u.c.speed;
			spd -= spd/10;
		}
		else
			spd = cc->speed * 8/10;

		spd = (spd > 560)? spd: 560;		/*560=70*8 */
	}
  /*** Set Start Speed ***/
	else if( cc->flg.f_condition == -1 ){
		if( c->use == CD_PLAY )
			spd = 1200;		/* 150*8 */
		else{
			spd = cc->max_speed;
			if( ii > 2 )
				spd = spd + spd*(20-ii)/100;	/* 17%(3), 16%(4) */
		}
	}
  /*** Set Default Speed ***/
	else if( cc->flg.f_condition == 0 ){
		spd = cc->max_speed;
		if( cc->mycar_dist < -0xa00 ) spd = spd * 80/100;	/* 80% */
		if( spd < cc->speed ) acl = cc->maccel;
	}
	else{ /* f_condition == -2 */
		spd = 0;
		acl = cc->maccel/20;
	}

  /*** ライバル車（悪魔カー）専用スピード ***/
	if( c == &c_car[11] ){
		acl = cc->paccel;
		special_speed_SATAN( c, &spd, &acl );
	}

	if( (job != JOB_CDPLAY) && (c == &c_car[0]) ){
		acl = cc->paccel;
		if( cc->final_spd == 0 )
			multi_special_speed( c ,&spd, &acl );
		else{
			spd = cc->final_spd;
			acl = (spd > cc->final_spd)? cc->maccel: cc->paccel;
		}
	}

/*	if( cc->flg.f_side_level == 1 ){
		if( cc->flg.f_back_car != -1 )
			spd = c_car[cc->flg.f_back_car].u.c.speed * 8/10;
		else
			spd = spd * 8/10;
		if( cc->speed > spd ) acl = cc->maccel;
	}
*/

//	if(c==&c_car[0]){
		/*debug[4]|=c->u.c.flg.f_condition;
		/*debug[5]=(c->u.c.max_speed<<16) | c->u.c.speed;*/
		/*debug[6]=(c->u.c.flg.f_straight<<16) | cc->final_spd;*/
//	}


  /***********************/
  /*** Set Curve Speed ***/
  /***********************/
	if( (cc->flg.f_condition != -1) &&
                   (cc->flg.f_direction != STRAIGHT) &&
                                      (cc->speed > SLOW_SPD) ){
		acl = cc->maccel;

		pcnt = PCT_10;
		if( cc->flg.f_pass1 >= 0 && cc->flg.f_pass1 != MYCAR_NUMBER )
			pcnt = PCT_5;

	  /*** rival special speed (down) ***/
		if( (ii == 0) && (game_level == 4) && (akumacar_flag == 1) )
			pcnt = PCT_10;


		tmp1 = cc->std_dir;
		tmp2 = 0x800 - cc->eye_dir[0];

		if( tmp1 > tmp2 ){
			if( tmp1-tmp2 > 0x800 ) tmp2 += 0x1000;
		}
		else{
			if( tmp2-tmp1 > 0x800 ) tmp1 += 0x1000;
		}
		t0 = (tmp1-tmp2>0)? tmp1-tmp2: tmp2-tmp1;
		spd = spd - (spd/100 *(t0/pcnt));	/* １％ がいくつ出るか */
	}

	if( acl > 0 ){
		if( cc->speed < spd ) cc->speed += acl;
	}
	else if( acl < 0 ){
		if( cc->speed > spd ) cc->speed += acl;
	}

	if( cc->speed < 0 ) cc->speed = 0;

  /*** 緊急速度制限解除 ***/
	if( (cc->flg.f_condition == 4) && (cc->speed < 520) ){	/* 520=65*8 */
		cc->flg.f_condition = 0;
	}
}

//---------------------------------------------------------------------

void	multi_get_nearest_link_car(MULTI_CAR *ccar)
{
	MULTI_CAR	*lcar;
	MULTI_CAR	*nrcar;
	long		i;
	long		tmp1;
	long		tmp2;
	long		nearest;

	fn_track("multi_get_nearest_link_car\n")

	// search for nearest link car in front or behind
	nearest = 0x0FFFFFFF;
	lcar = (MULTI_CAR *) &mcar[0];
	nrcar = NULL;
	for(i=0; i<CommNumLinkedCars; i++, lcar++) {
		tmp2 = ccar->c->t - lcar->c->t;
		if(tmp2 < 0) {
			if(-tmp2 < nearest)  {
				nearest = -tmp2;
				tmp1 = tmp2;
				nrcar = lcar;
			}
		}
		else {
			if(tmp2 < nearest) {
				nearest = tmp2;
				tmp1 = tmp2;
				nrcar = lcar;
			}
		}
	}
	
	// now have nearest data.
	ccar->near_car = nrcar;
	ccar->near_dist = tmp1;
}

//---------------------------------------------------------------------
