#include	"..\origsrc\common.h"
#define		VAR_EXTERN
#include	"..\origsrc\var.h"
#include	"..\origsrc\handle.h"
#include	"..\origsrc\sobj.h"

#define		_SIZE_T_DEFINED_
#include	"c:\watcom\h\stdio.h"

#include	"..\markk\multi.h"
#include	"..\markk\comm.h"
#include	"..\markk\newheli.h"
#include	"..\markk\grinfo.h"

//---------------------------------------------------------------------

void	multi_move_heli(void)
{
	MOVE	*m = &heli;
	long	s,r,d,n=4;
	VECTOR	t;	/* 移動目標 */
	long	dt,dcnt;
	VECTOR	v;
	long	hd,hd_inv,pitch;
	long	dhd;	/* 相対距離変化量 */
	long	dist;
	MULTI_CAR	*mc = &mcar[CommMasterCar];
	CAR		*c = mc->c;
	
	fn_track("multi_move_heli\n")

	v.vx = c->pos.vx - m->pos.vx;
	v.vy = c->pos.vy - m->pos.vy;
	v.vz = c->pos.vz - m->pos.vz;
	if((v.vx > 0x1000) || (v.vz > 0x1000) || (v.vx < -0x1000) || (v.vz < -0x1000)) {
		hd = 0;
	}
	else {
		dist = csqrt(v.vx*v.vx/4 + v.vy*v.vy/4/2 + v.vz*v.vz/4);
		if(m->hit == FALSE) {
			if(dist < 0x00000800) {
				if(CommMasterCar == CommMyCar) se_play(17);
				multi_set_swing(EFFECT_HELI, &mcar[CommMasterCar]);
				m->hit = TRUE;
			}
		}
		hd = 100 - (dist >> 10);
		if(hd > 100) hd = 100;
		if(hd < 0) hd = 0;
	}
	hd_inv = hd * 50 / 100;
	
	dhd = hd - old_hd;
	if(dhd < -20) dhd = -20;
	if(dhd >  20) dhd =  20;
	pitch = (60+dhd/2) << 7;
	
	hds.heli.v = hd;
	hds.heli.p = pitch;
	hds.heli_inv.v = hd_inv;
	hds.heli_inv.p = pitch;
	
	old_hd = hd;
	
	if(multi_crsl(c->t) == 0x1100) {
		if(m->top_com != &heli_start[0]) {
			init_heli();
		}
	}
	
	dt = get_distance_sign(c->t, m->zn);
	if(m->top_com == &heli_start[0]) dt = 0;
	if(dt < 0) {
		dcnt = 1;
	}
	else if(dt < 0x0800) {
		dcnt = 2;
	}
	else if(dt < 0x1000) {
		dcnt = 4;
	}
	else {
		dcnt = 16;
	}
	
	while (m->now_com == -1) {
		get_new_com(m);
	}
	
	switch (m->now_com) {
		case M_GO:
			m->cnt += dcnt;
			if(m->cnt >= m->target_cnt) {
				m->now_com = -1;
				m->cnt = m->target_cnt;
			}
			s = m->target_cnt - m->cnt;	r = m->cnt;
			t.vx = (m->old_pos.vx*s + m->target_pos.vx*r)/m->target_cnt;
			t.vy = (m->old_pos.vy*s + m->target_pos.vy*r)/m->target_cnt;
			t.vz = (m->old_pos.vz*s + m->target_pos.vz*r)/m->target_cnt;
			if(m->cnt < 32) {
				n = 12 - m->cnt/4;
			}
			else if(m->cnt > m->target_cnt-32) {
				n = 12 - (m->target_cnt - m->cnt)/4;
			}
			else {
				n = 4;
			}
			break;
		case M_STOP:
			m->cnt += dcnt;
			if(m->cnt >= m->target_cnt) m->now_com = -1;
			t = m->target_pos;
			n = 12;
			break;
		default:
			break;
	}
	m->pos.vx += (t.vx - m->pos.vx)/n;
	m->pos.vy += (t.vy - m->pos.vy)/n;
	m->pos.vz += (t.vz - m->pos.vz)/n;
	
/*
	m->pos.vx = 0x00007e26;
	m->pos.vy = 0xfffffc3a;
	m->pos.vz = 0x00004d58;
*/
	
	/* 姿勢制御 */
	if(m->target_rot.vx != m->rot.vx) {
		m->rot.vx = (m->rot.vx + m->drot.vx*dcnt) & 0x0fff;
		if(get_ddir(m->target_rot.vx, m->rot.vx) < MIN_DDIR*dcnt) {
			m->rot.vx = m->target_rot.vx;
		}
	}
	if(m->target_rot.vy != m->rot.vy) {
		m->rot.vy = (m->rot.vy + m->drot.vy*dcnt) & 0x0fff;
		if (get_ddir(m->target_rot.vy, m->rot.vy) < MIN_DDIR*dcnt) {
			m->rot.vy = m->target_rot.vy;
		}
	}
	if(m->target_rot.vz != m->rot.vz) {
		m->rot.vz = (m->rot.vz + m->drot.vz*dcnt) & 0x0fff;
		if (get_ddir(m->target_rot.vz, m->rot.vz) < MIN_DDIR*dcnt) {
			m->rot.vz = m->target_rot.vz;
		}
	}
}

//---------------------------------------------------------------------

void	multi_move_jet(void)
{
	JETS		*j = &jet;
	long		s = 200;
	long		trig_pos_1 = (game_level < 3) ? 0xc000 : 0x13500;
	long		trig_pos_2 = (game_level < 3) ? 0xc000 : 0x0d200;
	static		VECTOR	start = { 0x59c0, 0x0000, 0x78c0 };
	static		VECTOR	dv = { -0x2100, -0x1000, -0x4500 };
	VECTOR		v;
	long		d;
	MULTI_CAR	*mc = &mcar[CommMasterCar];
	CAR			*c = mc->c;
	
	fn_track("multi_move_jet\n")

	if(multi_crsl(c->t) == trig_pos_1) {
		if (c->u.m.lap != 2) {
			j->cnt = rand() & 0x7f;
			j->pos = start;
			j->se = FALSE;
		}
	}
	if(multi_crsl(c->t) == trig_pos_2) {
		if(c->u.m.lap == 2) {
			j->cnt = rand() & 0x3f;
			j->pos = start;
			j->se = FALSE;
		}
	}
	
	if(j->cnt > 0) {
		j->cnt++;
		if(j->cnt > 30*60) j->cnt = 0;
		j->pos.vx += dv.vx*s/0x10000;
		j->pos.vy += dv.vy*s/0x10000;
		j->pos.vz += dv.vz*s/0x10000;
		
		if(!j->se) {
			v.vx = c->pos.vx - j->pos.vx;
			v.vy = c->pos.vy - j->pos.vy;
			v.vz = c->pos.vz - j->pos.vz;
			if((v.vx <  0x1000) && (v.vz <  0x1000) && (v.vx > -0x1000) && (v.vz > -0x1000)) {
				d = csqrt(v.vx*v.vx/4 + v.vy*v.vy/4/2 + v.vz*v.vz/4) >> 10;
				debug_num(10, 10, d);
				if((d < 0x8000) && (CommMasterCar == CommMyCar)) {
					j->se = TRUE;
					se_play(43);	/* しゅおわーーーーーーんん */
				}
			}
		}
	}
}

//---------------------------------------------------------------------

void	multi_move_kick(MULTI_CAR *mc)
{
	CAR		*c = mc->c;
	KICK	*k;
	long	i;
	
	fn_track("multi_move_kick\n")

	if(kick_press) kick_press--;
	
	k = &kick[0];
	for (i=0; i<KICK_MAX; i++,k++) {
		if(k->flag == 0 ) {			/* 立っている */
			if(multi_hit_kick(k, mc)) {
				if((reverse_flag) && (mc->pad == 0)) {
					se_play(34);	/* ヒット音 */
				}
				else if(mc->pad == 0) {
					se_play(35);	/* ヒット音 */
				}
				k->flag = 1;
				k->v = c->u.m.dpos;
				k->v.vx *= 2;
				k->v.vz *= 2;
				k->dry = (k->v.vx*k->v.vx + k->v.vz*k->v.vz)/0x2000;
			}
		} else if(k->flag == 1) {	/* 回っている */
			k->pos.vx += k->v.vx/256;
			k->pos.vz += k->v.vz/256;
			k->v.vx = k->v.vx * 15 / 16;
			k->v.vz = k->v.vz * 15 / 16;
			k->rot.vy += k->dry/256;		/* 回転速度 */
			k->dry = k->dry * 15 / 16;
			if(k->rot.vx < fall_down_th[reverse_flag]) {
				k->rot.vx += 0x0080;
			}
			else {
				k->rot.vx = fall_down_th[reverse_flag];
			}
			if((k->v.vx == 0)&&(k->v.vz == 0)&&(k->dry == 0)
					&&(k->rot.vx == fall_down_th[reverse_flag])) {
				k->flag = 2;
			}
		}
		else {						/* 倒れている */
			if(multi_hit_kick_tire(k, mc)) {
				multi_set_swing(EFFECT_KK, mc);
				if((kick_press == 0) && (c->u.m.speed > 50*8)) {
					if(mc->pad == 0) se_play(30);
					kick_press = 4;
				}
			}
		}
	}
}

//---------------------------------------------------------------------

long	multi_hit_kick(KICK *k, MULTI_CAR *mc)
{
	CAR		*c = mc->c;
	VECTOR	*v = &c->pos;
	long	s = 0x40;	/* 車のヒットサイズ */
	
	fn_track("multi_hit_kick\n")

	return ((k->pos.vx > v->vx-s) && (k->pos.vx < v->vx+s)
		&& (k->pos.vz > v->vz-s) && (k->pos.vz < v->vz+s));
}

//---------------------------------------------------------------------

long	multi_hit_kick_tire(KICK *k, MULTI_CAR *mc)
{
//	extern	VECTOR	car_tire_pos[4];
	VECTOR	*v;
	long	i;
	long	s = 0x30;	/* タイヤのヒットサイズ */
	
	fn_track("multi_hit_kick_tire\n")

	v = &mc->car_tire_pos[0];
	for (i=0; i<4; i++, v++) {
		if((k->pos.vx > v->vx-s) && (k->pos.vx < v->vx+s)
		&& (k->pos.vz > v->vz-s) && (k->pos.vz < v->vz+s)) return (TRUE);
	}
	return (FALSE);
}

//---------------------------------------------------------------------

