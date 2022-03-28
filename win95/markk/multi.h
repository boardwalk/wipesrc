//-------------------------------------------------------------------
//	New car structure for multiple player Ridge Racer. (10/07/95).
// this code now works with new Ridge Racer code (17/08/95)
//-------------------------------------------------------------------

#define				JOB_MULTI0			JOB_GAME0
#define				JOB_MULTI			JOB_GAME
#define				JOB_MULTIEND		JOB_ENDING0
#define				M_CAR_MAX			C_CAR_MAX + 1

//-------------------------------------------------------------------

#define				ENGINE_MASTER		(40)
#define				SPIN_I_FORCE		14000
#define				TIME_UP_TIME		(8*30+15)	/*切れるなら小さくする*/
#define				GOAL_TIME			(4*30)
#define				SHIFT_DOWN_RPM		3500	/* オートマ時にシフトダウンする回転数 */
#define				SHIFT_UP_RPM		6400	/* オートマ時にシフトアップする回転数 */
#define				EFFECT_JUMP			1
#define				EFFECT_HIT			2
#define				EFFECT_KK			3
#define				EFFECT_STOP			4
#define				EFFECT_HELI			5
#define				HIT_SWING_TIME		30	/* 減衰振動時間 */
#define				REBOUND_TIME		30	/* 跳ね返り時間 */
#define				SPIN_TARGET			(0x180)
#define				SPIN_LOAD			(10000)		/* 8000 */
#define 			RUN		 			1
#define 			HIDE	 			0
#define 			RETIRE				-1
#define 			R_SIDE   			110
#define 			L_SIDE  			-110
#define 			WR_SIDE  			145
#define 			WL_SIDE 			-145
#define 			TL_SIDE   			90
#define 			TR_SIDE  			-90
#define 			LANE_R    			60
#define 			LANE_L   			-60
#define 			CENTER     			0
#define 			LEFT       			1
#define 			RIGHT     			-1
#define 			STRAIGHT			0
#define 			LEFT_TURN			1
#define 			RIGHT_TURN			-1
#define 			CD_PLAY				37

//-------------------------------------------------------------------

#define				multi_crs(t)			(((t)+(mc->road_size<<16))%(mc->road_size<<8))
#define				multi_crsu(tu)			(((tu)+(mc->road_size<<8))%mc->road_size)
#define				multi_crsl(t)			(multi_crs(t) & ~0xff)

#define				multi_decide_trig(x)	(pad_info[x].trig & (PAD_H|PAD_F|PAD_P))
#define				multi_cancel_trig(x)	(pad_info[x].trig & (PAD_E|PAD_G))
#define				multi_start_trig(x)		(pad_info[x].trig & PAD_H)
#define				multi_select_trig(x)	(pad_info[x].trig & PAD_K)
	
//-------------------------------------------------------------------

#ifdef _STATIC_OBJ_UNDEFINED_
typedef struct {
	short	tmd;
	VECTOR	v;
	short	ry;
} STATIC_OBJ;
#endif

typedef struct {
	short	c_car_max;
	long	lvl_pf[12][6];
} LEVEL_PF;

typedef struct {
	long	tt;		/* トリガータイム */
	CVECTOR	c;		/* 変化色 */
	long	ct;		/* 変化時間 */
	short	sp;		/* 空のパレット */
	short	sb;		/* 空の明るさ変化 */
} DQ_TABLE;

typedef struct {
	long	g_rate;		/* ギア比 */
	long	l_drpm;		/* 低回転時回転数変化量 */
	long	m_drpm;		/* 中回転時回転数変化量 */
	long	h_drpm;		/* 高回転時回転数変化量 */
	long	pp;			/* ピーク時回転数 */
} GEAR_DATA;

typedef struct _multi_car {
	CAR		*c;			// original CAR structure.

	// engine data
	GEAR_DATA	gear_data[6+1];
	long	engine_master;
	long	landing_cnt;
	long	mycar_roll;
	long	static_frec;	/* 静止摩擦 */
	long	start_spin_dy;
	long	i_cnt;		/* 慣性力カウンタ */
	long	i_force;	/* 慣性力 */
	long	reg_dry;	/* 速度に対する車体の向き */
	long	dpitch;
	long	bpitch;
	long	jpitch;
	long	epitch;
	long	disp_rpm;
	long	accel_open;
	long	engine_load;	/* エンジンの負荷 */
	long	decload;		/* 減衰負荷 */
	long	eload;			/* エンジンの負荷２ */
	long	sub_eload;
	long	hr; // MARK

	// collision effects
	long	swing_event;			/*** 0:揺れてない 1:Ｘ揺れ 2:Ｚ揺れ ***/
	long	swing_cnt;				/*** 減衰振動カウンタ ***/
	long	swing_width;			/*** 揺れ幅（振幅）***/
	long	rebound_event;			/*** 跳ね返りイベント ***/
	long	rebound_cnt;			/*** 跳ね返りカウンタ ***/
	VECTOR	rebound_v;				/*** 跳ね返りベクタ ***/

	// game loop vars
	short	game_stat;		/* ゲーム状態 */ // MARK
	long	goal_cnt;
	long	goal_lap;		/* 規定周回数 */
	long	total_time;		/* 総経過時間 */
	long	total_1000_time;
	long	limit_time;		/* 残り制限時間 */
	short	cp_cnt;			/* チェックポイント看板点滅カウンタ */
	long	wrong_way_cnt;	// MARK
	long	tunnel_rev;		/* トンネルリヴァーブ状態　 */
	long	pad;			// which pad struct is associated with this multi car

	// new vars (18/07/95)
	long	spin_r;
	long	spin_way;
	long	flow_ry;
	long	flow_speed;
	long	flow_dry;

	// new course vars (18/07/95)
	COURSE	*course_table;
	long	road_size;		/* easy:256<<8, expert:?<<8 */
//	long	zone_type;		/* 0:start 1:easy 2:tech */
	long	course_flag;

	// new tire vars (19/07/95)
	VECTOR	car_tire_pos[4];

 	struct	_multi_car	*hit_by;		// MARK which car struct has hit this car.
	struct	_multi_car	*near_car;		// MARK which car struct is nearest to this car.
	long	near_dist;					// how close the nearest car is (t relational value).
} MULTI_CAR;

//-------------------------------------------------------------------

extern MULTI_CAR	mcar[M_CAR_MAX];
extern MULTI_CAR	*camera_mtrg;
extern RAW_PAD		transfer_buf[M_CAR_MAX];
extern VECTOR		black_mark_pos;

extern FILE			*DataFile;
extern FILE			*BinaryFile;
extern FILE			*fn_file;
extern long			CommDataFileOpen;

extern long			pause_machine;

extern long			CommMultiPlaySelect;
extern long			CommMultiPadFlag;
extern long			CommNumLinkedCars;
extern long			CommNetPlayers;
extern long			CommMyCar;
extern long			CommLinkCar;
extern long			CommMasterCar;
extern long			CommSlaveCar;
extern long			CommSerialConnectMethod;
extern long			CommSerialMachine;
extern long			CommSerialDevice;
extern long			CommSerialBaudRate;
extern long			CommSerialDataSize;
extern long			CommSerialParity;
extern long			CommSerialStopSize;
extern char			CommModemInitString[256];
extern char			CommModemDialupString[256];
extern long			CommFirstRead;
extern long			CommRandomSeed;
extern long			CommMultiPause;

//-------------------------------------------------------------------

extern long			game_count;
extern long			tunnel_rev;
extern long			pause_y;
extern long			good_goal_flag;
extern long			ending_flag;
extern long			shutter_y;
extern long			shutter_v;
extern long			shutter_wait;
extern long			tun_a_start;
extern long			tun_a_end;
extern long			tun_b_start;
extern long			tun_b_end;
extern long			voice_tough;
extern long			voice_last;
extern long			voice_C[4];

extern long			now_bgm;
extern long			rand_cnt;
extern short		ALL_entry;			/* 登録台数 */
extern short		Satan_flag;
extern long			bv_ry;
extern short		TOP_car;
extern short		rival_flag;
extern CAR			*Side_stop_car;

extern long			old_camera_num;
extern long			spin_r;
extern long			spin_way;
extern long			flow_ry;
extern long			flow_speed;
extern long			flow_dry;
extern long			replay_game_level;

extern VECTOR		jumbo;
extern MOVE			heli;
extern VECTOR		baloon1;
extern VECTOR		baloon2;
extern VECTOR		black_mark_pos;

extern VECTOR		itatoma;
extern VECTOR		start_cp;
extern VECTOR		start_cp_r;
extern VECTOR		time_ext1;
extern VECTOR		time_ext2;
extern VECTOR		shovel;

extern VECTOR		gala;
extern VECTOR		gala_ld;
extern VECTOR		gala_cn;
extern VECTOR		gala_cnn;
extern VECTOR		gala_wn;
extern VECTOR		gc_pos;
extern VECTOR		gc_rot;
extern VECTOR		gala_r;
extern VECTOR		fall;
extern VECTOR		walk_gal[];

extern STATIC_OBJ	start0_tbl[];
extern STATIC_OBJ	start0div_tbl[];
extern STATIC_OBJ	rev_noway_0[];
extern STATIC_OBJ	rev_noway_1[];
extern STATIC_OBJ	*start0div_tbl_r[];
extern STATIC_OBJ	start1_tbl[];
extern STATIC_OBJ	branch0_tbl[];
extern STATIC_OBJ	branch1_tbl[];
extern STATIC_OBJ	reverse_tbl[];

extern DQ_TABLE		*dq_table[];
extern DQ_TABLE		*dq_trig;

extern STATIC_OBJ	share_tbl[];
extern STATIC_OBJ	share_tbl_dq[];
extern STATIC_OBJ	day_tbl[];
extern STATIC_OBJ	night_tbl[];
extern STATIC_OBJ	bldg_day_tbl[];
extern STATIC_OBJ	bldg_night_tbl[];

extern long			machine_type_D[];
extern long			machine_type_T[];
extern long			machine_type_CD[];

extern long			position_S[][4];
extern long			position_L[][4];
extern long			position_RS[][4];
extern long			position_RL[][4];
extern long			position_CD[][4];

extern long			es_pform[12][6];
extern long			nm_pform[12][6];
extern long			hd_pform[12][6];
extern long			tt_pform[12][6];
extern long			cd_pform[12][6];
extern long			es_pformR[12][6];
extern long			nm_pformR[12][6];

extern VECTOR		time_up_camera;

//-------------------------------------------------------------------
// function prototypes.

extern void			job_multi_game0(void);
extern void			job_multi_game(void);
extern void			multi_init_car(MULTI_CAR *mc);
extern void			multi_init_carB(MULTI_CAR *mc, long model);
extern void			multi_run(MULTI_CAR *mc, long padno);
extern void			multi_horizon(MULTI_CAR *mc, long padno);
extern void			multi_start_car(MULTI_CAR *mc);
extern void			multi_engine(MULTI_CAR *mc, long padno);
extern void			multi_handle(MULTI_CAR *mc, long padno);
extern void			multi_action_kansei(MULTI_CAR *mc);
extern void			multi_action_sound(MULTI_CAR *mc);
extern void			multi_check_lap(MULTI_CAR *mc);
extern void			multi_check_cp1(MULTI_CAR *mc);
extern void			multi_check_cp2(MULTI_CAR *mc);
extern void			multi_action_rebound(MULTI_CAR *mc);
extern void			multi_set_rebound(MULTI_CAR *mc, long vx, long vz);
extern void			multi_action_effect(MULTI_CAR *mc);
extern void			multi_action_jump(MULTI_CAR *mc);
extern void			multi_action_pitch(MULTI_CAR *mc);
extern void			multi_action_bank(MULTI_CAR *mc);
extern void			multi_set_swing(long mode, MULTI_CAR *mc);
extern void			multi_run_grip(MULTI_CAR *mc);
extern void			multi_run_spin(MULTI_CAR *mc);
extern void			multi_run_flow(MULTI_CAR *mc);
extern void			multi_run_slip(MULTI_CAR *mc);
extern void			multi_set_rev(MULTI_CAR *mc);
extern long			multi_check_jump(MULTI_CAR *mc);
extern void			multi_action_engine(void);
extern void			multi_set_camera(long cn, MULTI_CAR *mc);
extern void			multi_add_others(void);
extern void			multi_action_dq(void);
extern void			multi_disp_indicator(void);
extern void			multi_disp_lap_time(MULTI_CAR *mc);
extern void			multi_set_camera_super(long cn, MULTI_CAR *mc);
extern long			multi_check_car_hit(MULTI_CAR *mc);
extern void			multi_run_arong(MULTI_CAR *mc);
extern long			multi_in_road(long nx, long nz, long dir, long nt, MULTI_CAR *mc);
extern long			multi_get_cry_dd(long t, MULTI_CAR *mc);
extern long			multi_get_cry(long t, MULTI_CAR *mc);
extern void			multi_get_cxyz(long t, VECTOR *v, MULTI_CAR *mc);
extern void			multi_set_camera_time_up(CAR *trg, long gc, MULTI_CAR *mc);
extern long			multi_wrong_way(MULTI_CAR *mc);
extern long			multi_get_t(VECTOR *pos, long last_tu, MULTI_CAR *mc);
extern long			multi_xz2ts(VECTOR *pos, VECTOR *rot, long last_tu, long *t, long *s, MULTI_CAR *mc);
extern void			multi_ts2xz(long t, long s, VECTOR *pos, VECTOR *rot, MULTI_CAR *mc);
extern long			multi_get_near_camera(MULTI_CAR *mc);
extern void			multi_set_camera_0(MULTI_CAR *mc, long cnt);
extern long			multi_get_distance(MULTI_CAR *mc, long t0, long t1);
extern long			multi_check_tunnel(long t, MULTI_CAR *mc);
extern void			multi_action_voice(MULTI_CAR *mc);
extern long			multi_road_hit(long x, long z, long tu, COURSE *p, long *tl, long *s, MULTI_CAR *mc);
extern void			multi_add_car(MULTI_CAR *mc, long model_change_flag);
extern long			multi_check_pause(void);
extern void			multi_pause_menu(long id);

//-------------------------------------------------------------------
