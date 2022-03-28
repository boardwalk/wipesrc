/*
 *	common.h
 */


#include	"\wipesrc\psx26\include\sys\types.h"
#include	"\wipesrc\psx26\include\libetc.h"
#include	"\wipesrc\psx26\include\libgte.h"
#include	"\wipesrc\psx26\include\libgpu.h"


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
/*----- デバッグマスク -----*/
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


/*** オーダリングテーブル関係 ***/
#define		MAIN_OTSIZE		(512-64)
#define		Z_SHIFT			(128)	/* Ｚシフト範囲 */
#define		OTZS			(4+1)	/* オーダリングテーブルＺ補正シフト値 */
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
	ダブルバッファ構造体											
*******************************************************************/
#define		PRIM_MAX	(0x22000)	/* 0x20000で止まったことはない */
typedef struct {
	DRAWENV		draw;			/* 描画環境 */
	DISPENV		disp;			/* 表示環境 */
	u_long		ot[OTSIZE];		/* オーダリングテーブル */
	
	char		s[PRIM_MAX];	/* primitive buffer */
	
	STATIC_PRIM	sp;
	
	/* for work meter */
#ifdef DEBUG
	LINE_F2		wm[METER_MAX+METER_DIV];
#endif
} DB;


/*******************************************************************
	グローバルキャッシュ構造体										
*******************************************************************/
typedef struct {
	char		*s;
	u_long		*ot;
	long		p,otz,flg,surf,otdz;
	u_short		clut,tpage;
	
/* 定数 */
	CVECTOR		amb;			/* 0xff, 0xff, 0xff */
	CVECTOR		dqa;			/* 0x80, 0x80, 0x80 */
	
	DVECTOR		dv[4];
	
	long		apc_head[10];	/* constant */
	char		buf[784];		/* クリッピングバッファとして最大 784 */
	
	char		rest[144];
} CACHE_COMMON;

/*** オブジェクト表示関数呼び出しマクロ ***/
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
/*** パレット補正 ***/
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
	車構造体														
*******************************************************************/
typedef struct {
	long	dt,ds;
	VECTOR	dpos;
	VECTOR	drot;
	
	short	drive_mode;	/* 0:オートマ 1:マニュアル */
	short	shift;		/* ギアポジション */
	long	rpm;		/* エンジン回転数 */
	long	shift_load;	/* ギアシフト負荷 */
	long	old_tire_rpm;	/* 以前のタイヤの回転数 */
	
	short	slip_mode;	/* 現在滑っているか */
	long	slip_level;
	long	handle;
	
	/*** 車体の物理的諸量 ***/
	long	max_speed;	/* 最高速 */
	/* 極座標系 */
	long	speed;		/* 車のスピード */
	long	a;			/* 車の加速度 */
	long	v_ry;		/* 速度方向 */
	
	/*** 御タイヤ様 ***/
	/* 前輪 */
	long	tire_dir;	/* タイヤの車体に対する相対角度 */
	/* 後輪 */
	long	tire_rpm;	/* タイヤの回転数 */
	long	tire_mode;	/* タイヤの動作状態 */
	
	
	short	jump_flag;
	short	jump_cnt;
	long	jump_vy;
	long	jump_pitch;
	
	short	gas_trig,brk_trig;
	short	gas,brk;
	
	short	ranking;	/* 総合順位 (1 ~ 12) */
	
	short	make_best_lap;
	long	last_t;
	short	lap,unlap;
	short	uncp1,uncp2;
	long	lap_time[5];
	long	disp_lap_time[5];
} MYCAR_BUF;


typedef struct {
	short	f_drift;		/* ドリフトフラグ(0...OK,1...NG) */
	short	f_hit_lock;		/* ヒットロック */
	short	f_direction;	/* 走行方法 */
	short	f_front_car;	/* 先行車番号 */
	short	f_back_car;		/* 後続車番号 */
	short	f_pass1;		/* 追越しフラグ（１段階）(-2~16) */
	short	f_lap;			/* 周回カウンター */
	short	f_mycar_pos;	/* マイカー接近フラグ（ランキング用）*/
	short	f_passd_mycar;	/* マイカーランキング */
	short	f_condition;	/* 追いかけ (車種限定) */
	short	f_chase_mycar;	/* マイカー接近フラグ （ボイス用）*/
	short	f_curve_lock;	/* カーブのロックフラグ */
	short	f_jump;			/* ジャンプフラグ */
	short	f_straight;		/* スピード出せちゃうフラグ */
	short	f_side_level;	/* 道幅限定フラグ */
	short	f_satan_run;	/* 悪魔カーお披露目フラグ */
} C_CAR_FLAG;


typedef struct {
	long	status;
	long	dt,ds;
	VECTOR	dpos;
	VECTOR	drot;
	long	last_t;
	long	speed;		/* 現在のスピード */
	long	handle;		/* ハンドル */

	C_CAR_FLAG	flg;

	long	target_s;		/*目標レーン*/
	long	target_spd;		/*目標スピード*/
	long	final_spd;		/*最終直線スピード*/

	long	hit_drot;		/*ヒットで受けた向き*/
	long	hit_ds;			/*ヒットで飛ばされる幅*/
	long	std_dir;		/*進行方向*/
	long	eye_dir[4];		/*目標方向 ([0].now [1].next [2].next*2 [3].back)*/
	long	c_car_dist;		/*先行車との距離*/
	long	frnt_dist;		/*チェック用*/
	long	mycar_dist;		/*マイカーとの距離*/

	short	jump_cnt;		/*ジャンプカウンター*/
	short	sw_cnt;			/*揺れカウント*/
	long	sw_width;		/*揺れ幅*/
	long	jump_vy;		/*高さ*/
	long	jump_pitch;		/*ピッチ*/

/******** 車の性能 ********/
	short	machine_type;	/*車種性能設定*/
	long	max_speed;		/*最高速度*/
	long	paccel;			/*通常加速*/
	long	maccel;			/*通常減速 (現在未使用)*/
	long	dacl;			/*加速度調整(tmp)*/
	u_long	start_acl;		/*同時スタート特別加速*/
	u_long	chase_spd;		/*追いかけ特別速度*/
	u_long	away_spd;		/*逃げ特別速度*/
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
	short	flag;	/* シェーディングフラグ */
	CVECTOR	c;		/* シェーディングカラー */
	CVECTOR	sc,dc;
	short	si,di;	/* siは０からdiへ１ずつ増加 */
	short	sp;		/* 空のパレット */
	short	sf;		/* 空フラグ */
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
