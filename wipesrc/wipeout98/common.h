/*
 *	common.h
 */


#include	"wipesrc/psx26/include/sys/types.h"
#include	"wipesrc/psx26/include/libetc.h"
#include	"wipesrc/psx26/include/libgte.h"
#include	"wipesrc/psx26/include/libgpu.h"


//#define	AddPrim(ot,p)	AJYAddPrim(ot,p)
#define	SetSemiTrans(p,abe)	setSemiTrans(p,abe)
#define	SetShadeTex(p,tge)	setShadeTex(p,tge)
#define	GetClut(x,y)	getClut(x,y)
#define	GetTPage(tp,abr,x,y)	getTPage(tp,abr,x,y)
#define	SetLineF2(p)	setLineF2(p)
#define	SetPolyF4(p)	setPolyF4(p)
#define	SetPolyFT4(p)	setPolyFT4(p)
#define	SetPolyG4(p)	setPolyG4(p)
#define	SetPolyGT4(p)	setPolyGT4(p)
#define	SetSprt(p)	 	setSprt(p)
#define	SetSprt8(p)		setSprt8(p)
#define	SetTile(p)		setTile(p)

#define		SOUND_ON
/*
#define		PAUSE_ON
#define		DEBUG

#define		SECRET_OK
*/
#define		SECRET_OK

#ifdef DEBUG
#else
/*----- �f�o�b�O�}�X�N -----*/
#define		meter(r,g,b)
#define		start_work_meter()
#define		add_work_meter(ot,wm)
#define		init_buffer_check()
#define		buffer_check()
#define		add_debug()
#define		debug_num(x,y,d)
/*--------------------------*/
#endif



#define		TRUE			1
#define		FALSE			0
#ifndef		NULL
#define		NULL			0
#endif


/*** �I�[�_�����O�e�[�u���֌W ***/
#define		MAIN_OTSIZE		(512-64)
#define		Z_SHIFT			(128)	/* �y�V�t�g�͈� */
#define		OTZS			(4+1)	/* �I�[�_�����O�e�[�u���y�␳�V�t�g�l */
#define		OTSIZE			(Z_SHIFT*2 + MAIN_OTSIZE)

#define		adjust_otz(z,zs)	\
	( ((((z) >>= OTZS) >= MAIN_OTSIZE) || ((z) <=0 ) )	\
												? 0 : ((z) += (zs)+Z_SHIFT) )


#define		SCR_Z			(256+64)
#define		SCR_X			(320)
#define		SCR_Y			(240)

#define		METER_MAX		32
#define		METER_DIV		4


/*** game ***/
#define		C_CAR_MAX		(12)
#define		REPLAY_FRAME	(30*60*3)		/* 60sec. */
#define		crs(t)			(((t)+(road_size<<16))%(road_size<<8))
#define		crsu(tu)		(((tu)+(road_size<<8))%road_size)
#define		crsl(t)			(crs(t) & ~0xff)

/*** word palette ***/
//#define		WHITE			100
//#define		RED				127
//#define		BLACK			285

#define		WHITE			015
#define		RED				017
#define		BLACK			004

/*#define		CPage(n)		GetClut( ((n)&0xf)*16, 480+(n)/16 )*/
#define		CPage(n)		(((480+(n)/16)<<6)+((n)&0xf))
#define		DCPage(s,d)		((CPage( d ) - CPage( s )) << 16)


/*** map ***/
#define		MAP_DIV			32
#define		MAP_SIZE		2048
#define		ALL_MAP_SIZE	((MAP_DIV-2)*MAP_SIZE)


#define		SIGNAL_START	(2*30+1*30)


/*** course data ***/
/*typedef struct {
	int		x,z;
	short	height;
	short	width;
	short	dir;
	short	pitch;
	short	bank;
	short	dummy;
} COURSE;*/

typedef struct {
	int		x,z;
	short	height;
	short	dir;
	short	pitch;
	short	l_side;
	short	r_side;
	short	d_center;
} COURSE;


typedef struct {
	VECTOR		eye;
	VECTOR		rot;
	VECTOR		trg;	/* not use */
} RR_VIEW;

typedef struct {
	DR_MODE		panel_dm;
	SPRT		map;
	SPRT		tacho;
	SPRT		time;
	SPRT		position;
	SPRT		total;
	SPRT		unit;
} STATIC_PRIM;


/*******************************************************************
	�_�u���o�b�t�@�\����
*******************************************************************/
#define		PRIM_MAX	(0x22000)	/* 0x20000�Ŏ~�܂������Ƃ͂Ȃ� */
typedef struct {
	DRAWENV		draw;			/* �`��� */
	DISPENV		disp;			/* �\���� */
	u_long		ot[OTSIZE];		/* �I�[�_�����O�e�[�u�� */

	char		s[PRIM_MAX];	/* primitive buffer */

	STATIC_PRIM	sp;

	/* for work meter */
#ifdef DEBUG
	LINE_F2		wm[METER_MAX+METER_DIV];
#endif
} DB;


/*******************************************************************
	�O���[�o���L���b�V���\����
*******************************************************************/
typedef struct {
	char		*s;
	u_long		*ot;
	long		p,otz,flg,surf,otdz;
	u_short		clut,tpage;

/* �萔 */
	CVECTOR		amb;			/* 0xff, 0xff, 0xff */
	CVECTOR		dqa;			/* 0x80, 0x80, 0x80 */

	DVECTOR		dv[4];

	long		apc_head[10];	/* constant */
	char		buf[784];		/* �N���b�s���O�o�b�t�@�Ƃ��čő� 784 */

	char		rest[144];
} CACHE_COMMON;

/*** �I�u�W�F�N�g�\���֐��Ăяo���}�N�� ***/
#define			add_obj( p, m, n, z )		\
					set_obj_matrix( &cs->buf[0], p, m ),	\
					cs->s = (char *)addobj( cs->s, &cs->apc_head[0], \
												((n) < obj_max) ? (n) : 1, 0 )
#define			add_obj_semi( p, m, n, z )		\
					set_obj_matrix( &cs->buf[0], p, m ),	\
					cs->s = (char *)addobj_semi( cs->s, &cs->apc_head[0], \
												((n) < obj_max) ? (n) : 1, 0 )
#define			add_obj_dq( p, m, n, z )	\
					set_obj_matrix( &cs->buf[0], p, m ),	\
					cs->s = (char *)addobj_dq( cs->s, &cs->apc_head[0],	\
												((n) < obj_max) ? (n) : 1, 0 )
/*** �p���b�g�␳ ***/
#define			add_obj_p( p, m, n, dp )		\
					set_obj_matrix( &cs->buf[0], p, m ),	\
					cs->s = (char *)addobj( cs->s, &cs->apc_head[0], \
												((n) < obj_max) ? (n) : 1, dp )
#define			add_obj_h( p, m, n, dp )		\
					set_obj_matrix( &cs->buf[0], p, m ),	\
					cs->s = (char *)addobj_hole( cs->s, &cs->apc_head[0], \
												((n) < obj_max) ? (n) : 1, dp )
#define			add_obj_p_dq( p, m, n, dp )	\
					set_obj_matrix( &cs->buf[0], p, m ),	\
					cs->s = (char *)addobj_dq( cs->s, &cs->apc_head[0],	\
												((n) < obj_max) ? (n) : 1, dp )


/*******************************************************************
	�ԍ\����
*******************************************************************/
typedef struct {
	long	dt,ds;
	VECTOR	dpos;
	VECTOR	drot;

	short	drive_mode;	/* 0:�I�[�g�} 1:�}�j���A�� */
	short	shift;		/* �M�A�|�W�V���� */
	long	rpm;		/* �G���W����]�� */
	long	shift_load;	/* �M�A�V�t�g���� */
	long	old_tire_rpm;	/* �ȑO�̃^�C���̉�]�� */

	short	slip_mode;	/* ���݊����Ă��邩 */
	long	slip_level;
	long	handle;

	/*** �ԑ̂̕����I���� ***/
	long	max_speed;	/* �ō��� */
	/* �ɍ��W�n */
	long	speed;		/* �Ԃ̃X�s�[�h */
	long	a;			/* �Ԃ̉����x */
	long	v_ry;		/* ���x���� */

	/*** ��^�C���l ***/
	/* �O�� */
	long	tire_dir;	/* �^�C���̎ԑ̂ɑ΂��鑊�Ίp�x */
	/* ��� */
	long	tire_rpm;	/* �^�C���̉�]�� */
	long	tire_mode;	/* �^�C���̓����� */


	short	jump_flag;
	short	jump_cnt;
	long	jump_vy;
	long	jump_pitch;

	short	gas_trig,brk_trig;
	short	gas,brk;

	short	ranking;	/* �������� (1 ~ 12) */

	short	make_best_lap;
	long	last_t;
	short	lap,unlap;
	short	uncp1,uncp2;
	long	lap_time[5];
	long	disp_lap_time[5];
} MYCAR_BUF;


typedef struct {
	short	f_drift;		/* �h���t�g�t���O(0...OK,1...NG) */
	short	f_hit_lock;		/* �q�b�g���b�N */
	short	f_direction;	/* ���s���@ */
	short	f_front_car;	/* ��s�Ԕԍ� */
	short	f_back_car;		/* �㑱�Ԕԍ� */
	short	f_pass1;		/* �ǉz���t���O�i�P�i�K�j(-2~16) */
	short	f_lap;			/* ����J�E���^�[ */
	short	f_mycar_pos;	/* �}�C�J�[�ڋ߃t���O�i�����L���O�p�j*/
	short	f_passd_mycar;	/* �}�C�J�[�����L���O */
	short	f_condition;	/* �ǂ����� (�Ԏ����) */
	short	f_chase_mycar;	/* �}�C�J�[�ڋ߃t���O �i�{�C�X�p�j*/
	short	f_curve_lock;	/* �J�[�u�̃��b�N�t���O */
	short	f_jump;			/* �W�����v�t���O */
	short	f_straight;		/* �X�s�[�h�o�����Ⴄ�t���O */
	short	f_side_level;	/* ��������t���O */
	short	f_satan_run;	/* �����J�[����I�ڃt���O */
} C_CAR_FLAG;


typedef struct {
	long	status;
	long	dt,ds;
	VECTOR	dpos;
	VECTOR	drot;
	long	last_t;
	long	speed;		/* ���݂̃X�s�[�h */
	long	handle;		/* �n���h�� */

	C_CAR_FLAG	flg;

	long	target_s;		/*�ڕW���[��*/
	long	target_spd;		/*�ڕW�X�s�[�h*/
	long	final_spd;		/*�ŏI�����X�s�[�h*/

	long	hit_drot;		/*�q�b�g�Ŏ󂯂�����*/
	long	hit_ds;			/*�q�b�g�Ŕ�΂���镝*/
	long	std_dir;		/*�i�s����*/
	long	eye_dir[4];		/*�ڕW���� ([0].now [1].next [2].next*2 [3].back)*/
	long	c_car_dist;		/*��s�ԂƂ̋���*/
	long	frnt_dist;		/*�`�F�b�N�p*/
	long	mycar_dist;		/*�}�C�J�[�Ƃ̋���*/

	short	jump_cnt;		/*�W�����v�J�E���^�[*/
	short	sw_cnt;			/*�h��J�E���g*/
	long	sw_width;		/*�h�ꕝ*/
	long	jump_vy;		/*����*/
	long	jump_pitch;		/*�s�b�`*/

/******** �Ԃ̐��\ ********/
	short	machine_type;	/*�Ԏ퐫�\�ݒ�*/
	long	max_speed;		/*�ō����x*/
	long	paccel;			/*�ʏ����*/
	long	maccel;			/*�ʏ팸�� (���ݖ��g�p)*/
	long	dacl;			/*�����x����(tmp)*/
	u_long	start_acl;		/*�����X�^�[�g���ʉ���*/
	u_long	chase_spd;		/*�ǂ��������ʑ��x*/
	u_long	away_spd;		/*�������ʑ��x*/
} C_CAR_BUF;


typedef struct {
/*----------------------*/
/*	system reserved 	*/
	short	use;
	short	model;
	long	flag;

	long	t,s;
	VECTOR	pos;
	VECTOR	rot;
	VECTOR	tire_rot;
	long	shadow_y;

	short	hit;
	VECTOR	hvec;
/*----------------------*/
	union {
		MYCAR_BUF	m;
		C_CAR_BUF	c;
	} u;
} CAR;

#define		vscale(v,s)		{	\
	(v)->vx = (v)->vx * s;		\
	(v)->vy = (v)->vy * s;		\
	(v)->vz = (v)->vz * s;		\
}

typedef struct {
	SVECTOR		mp;		/* pad:mycar shadow_y */
	SVECTOR		mr;		/* pad:mycar tire_rot.vz */
	SVECTOR		cp;		/* pad:c_car shadow_y */
	SVECTOR		cr;		/* pad:c_car tire_rot.vz */
	long		mt,ct;	/* current t */
} REPLAY_BUF;

typedef struct {
	short	flag;	/* �V�F�[�f�B���O�t���O */
	CVECTOR	c;		/* �V�F�[�f�B���O�J���[ */
	CVECTOR	sc,dc;
	short	si,di;	/* si�͂O����di�ւP������ */
	short	sp;		/* ��̃p���b�g */
	short	sf;		/* ��t���O */
	short	ssi,sdi;
} DEPTH_QUEUE;

typedef struct {
	short	*top_com,*com;	/* command table pointer */
	short	now_com;
	long	cnt;
	long	target_cnt;
	VECTOR	pos,old_pos,target_pos;
	VECTOR	rot,drot,target_rot;
	long	zn;
	long	hit;
} MOVE;


typedef	long	TIME;

typedef struct {
	char	name[4];
	TIME	time;
	long	car_type;
} NAME;


#define cs cache_buffer
extern	void	AJYAddPrim();
extern	CACHE_COMMON	*cache_buffer;
//#define	ApplyMatrix(a,b,c)	ApplyMatrixCC(a,b,c)

#define	SKY_COLOUR 155
