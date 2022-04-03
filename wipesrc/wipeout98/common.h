/*
 *	common.h
 */


#include	"wipesrc/psx26/include/libetc.h"
#include	"wipesrc/psx26/include/libgte.h"
#include	"wipesrc/psx26/include/libgpu.h"

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
	int32_t		x,z;
	int16_t	height;
	int16_t	width;
	int16_t	dir;
	int16_t	pitch;
	int16_t	bank;
	int16_t	dummy;
} COURSE;*/

typedef struct {
	int32_t		x,z;
	int16_t	height;
	int16_t	dir;
	int16_t	pitch;
	int16_t	l_side;
	int16_t	r_side;
	int16_t	d_center;
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
	uint32_t		ot[OTSIZE];		/* �I�[�_�����O�e�[�u�� */

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
	uint32_t		*ot;
	int32_t		p,otz,flg,surf,otdz;
	uint16_t		clut,tpage;

/* �萔 */
	CVECTOR		amb;			/* 0xff, 0xff, 0xff */
	CVECTOR		dqa;			/* 0x80, 0x80, 0x80 */

	DVECTOR		dv[4];

	int32_t		apc_head[10];	/* constant */
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
	int32_t	dt,ds;
	VECTOR	dpos;
	VECTOR	drot;

	int16_t	drive_mode;	/* 0:�I�[�g�} 1:�}�j���A�� */
	int16_t	shift;		/* �M�A�|�W�V���� */
	int32_t	rpm;		/* �G���W����]�� */
	int32_t	shift_load;	/* �M�A�V�t�g���� */
	int32_t	old_tire_rpm;	/* �ȑO�̃^�C���̉�]�� */

	int16_t	slip_mode;	/* ���݊����Ă��邩 */
	int32_t	slip_level;
	int32_t	handle;

	/*** �ԑ̂̕����I���� ***/
	int32_t	max_speed;	/* �ō��� */
	/* �ɍ��W�n */
	int32_t	speed;		/* �Ԃ̃X�s�[�h */
	int32_t	a;			/* �Ԃ̉����x */
	int32_t	v_ry;		/* ���x���� */

	/*** ��^�C���l ***/
	/* �O�� */
	int32_t	tire_dir;	/* �^�C���̎ԑ̂ɑ΂��鑊�Ίp�x */
	/* ��� */
	int32_t	tire_rpm;	/* �^�C���̉�]�� */
	int32_t	tire_mode;	/* �^�C���̓����� */


	int16_t	jump_flag;
	int16_t	jump_cnt;
	int32_t	jump_vy;
	int32_t	jump_pitch;

	int16_t	gas_trig,brk_trig;
	int16_t	gas,brk;

	int16_t	ranking;	/* �������� (1 ~ 12) */

	int16_t	make_best_lap;
	int32_t	last_t;
	int16_t	lap,unlap;
	int16_t	uncp1,uncp2;
	int32_t	lap_time[5];
	int32_t	disp_lap_time[5];
} MYCAR_BUF;


typedef struct {
	int16_t	f_drift;		/* �h���t�g�t���O(0...OK,1...NG) */
	int16_t	f_hit_lock;		/* �q�b�g���b�N */
	int16_t	f_direction;	/* ���s���@ */
	int16_t	f_front_car;	/* ��s�Ԕԍ� */
	int16_t	f_back_car;		/* �㑱�Ԕԍ� */
	int16_t	f_pass1;		/* �ǉz���t���O�i�P�i�K�j(-2~16) */
	int16_t	f_lap;			/* ����J�E���^�[ */
	int16_t	f_mycar_pos;	/* �}�C�J�[�ڋ߃t���O�i�����L���O�p�j*/
	int16_t	f_passd_mycar;	/* �}�C�J�[�����L���O */
	int16_t	f_condition;	/* �ǂ����� (�Ԏ����) */
	int16_t	f_chase_mycar;	/* �}�C�J�[�ڋ߃t���O �i�{�C�X�p�j*/
	int16_t	f_curve_lock;	/* �J�[�u�̃��b�N�t���O */
	int16_t	f_jump;			/* �W�����v�t���O */
	int16_t	f_straight;		/* �X�s�[�h�o�����Ⴄ�t���O */
	int16_t	f_side_level;	/* ��������t���O */
	int16_t	f_satan_run;	/* �����J�[����I�ڃt���O */
} C_CAR_FLAG;


typedef struct {
	int32_t	status;
	int32_t	dt,ds;
	VECTOR	dpos;
	VECTOR	drot;
	int32_t	last_t;
	int32_t	speed;		/* ���݂̃X�s�[�h */
	int32_t	handle;		/* �n���h�� */

	C_CAR_FLAG	flg;

	int32_t	target_s;		/*�ڕW���[��*/
	int32_t	target_spd;		/*�ڕW�X�s�[�h*/
	int32_t	final_spd;		/*�ŏI�����X�s�[�h*/

	int32_t	hit_drot;		/*�q�b�g�Ŏ󂯂�����*/
	int32_t	hit_ds;			/*�q�b�g�Ŕ�΂���镝*/
	int32_t	std_dir;		/*�i�s����*/
	int32_t	eye_dir[4];		/*�ڕW���� ([0].now [1].next [2].next*2 [3].back)*/
	int32_t	c_car_dist;		/*��s�ԂƂ̋���*/
	int32_t	frnt_dist;		/*�`�F�b�N�p*/
	int32_t	mycar_dist;		/*�}�C�J�[�Ƃ̋���*/

	int16_t	jump_cnt;		/*�W�����v�J�E���^�[*/
	int16_t	sw_cnt;			/*�h��J�E���g*/
	int32_t	sw_width;		/*�h�ꕝ*/
	int32_t	jump_vy;		/*����*/
	int32_t	jump_pitch;		/*�s�b�`*/

/******** �Ԃ̐��\ ********/
	int16_t	machine_type;	/*�Ԏ퐫�\�ݒ�*/
	int32_t	max_speed;		/*�ō����x*/
	int32_t	paccel;			/*�ʏ����*/
	int32_t	maccel;			/*�ʏ팸�� (���ݖ��g�p)*/
	int32_t	dacl;			/*�����x����(tmp)*/
	uint32_t	start_acl;		/*�����X�^�[�g���ʉ���*/
	uint32_t	chase_spd;		/*�ǂ��������ʑ��x*/
	uint32_t	away_spd;		/*�������ʑ��x*/
} C_CAR_BUF;


typedef struct {
/*----------------------*/
/*	system reserved 	*/
	int16_t	use;
	int16_t	model;
	int32_t	flag;

	int32_t	t,s;
	VECTOR	pos;
	VECTOR	rot;
	VECTOR	tire_rot;
	int32_t	shadow_y;

	int16_t	hit;
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
	int32_t		mt,ct;	/* current t */
} REPLAY_BUF;

typedef struct {
	int16_t	flag;	/* �V�F�[�f�B���O�t���O */
	CVECTOR	c;		/* �V�F�[�f�B���O�J���[ */
	CVECTOR	sc,dc;
	int16_t	si,di;	/* si�͂O����di�ւP������ */
	int16_t	sp;		/* ��̃p���b�g */
	int16_t	sf;		/* ��t���O */
	int16_t	ssi,sdi;
} DEPTH_QUEUE;

typedef struct {
	int16_t	*top_com,*com;	/* command table pointer */
	int16_t	now_com;
	int32_t	cnt;
	int32_t	target_cnt;
	VECTOR	pos,old_pos,target_pos;
	VECTOR	rot,drot,target_rot;
	int32_t	zn;
	int32_t	hit;
} MOVE;


typedef	int32_t	TIME;

typedef struct {
	char	name[4];
	TIME	time;
	int32_t	car_type;
} NAME;


#define cs cache_buffer
extern	void	AJYAddPrim();
extern	CACHE_COMMON	*cache_buffer;
//#define	ApplyMatrix(a,b,c)	ApplyMatrixCC(a,b,c)

#define	SKY_COLOUR 155
