#ifndef		RSCALE
#define		RSCALE				(512*8)
#endif
#define		HIT_SWING_TIME_C	20
#define 	MYCAR_NUMBER		C_CAR_MAX
#define 	FIND_DIST			500
#define 	CHASE_DIST			350
#define 	NEAR_DIST			275
#define 	AWAY_DIST			-200
#define 	FRNT_DIST  			400
#define 	DANGER_DIST 		250
#define 	BACK_DIST 			-150
#define 	VOICE_TIME	 		120		/* 4 (sec) */
#define 	CHECK_TIME			-30			/* 1 (sec) */
#define 	PCT_10 				9
#define 	PCT_5  				16
#define 	SLOW_SPD 			640	/* 80最低スピード */
#define		DEF_ROLL			15
#define 	LIMIT_SPEED 		1400		/* 180*8 */

extern long			rand_cnt;
extern long			mycar_p;
extern short		Satan_flag;
extern short		rival_flag;
extern CAR			*Side_stop_car;
extern long			all_map_size;
extern short		*map_table;
extern int			road_cnt;
extern int			road_delta;
extern int			tx;
extern short		reverse_c_car;
extern long			R_Side;
extern long			L_Side;
extern long			nearest_dist;
extern MULTI_CAR	*nearest_car;

extern void			multi_init_c_car(void);
extern void			multi_create_c_car(CAR *c, long ii, long *mc_type);
extern void			multi_move_c_car(void);
extern void			multi_hit_c_car(MULTI_CAR *mc);
extern void			multi_sch_front_car(CAR *c, short ii);
extern void			multi_mycar_position(void);
extern void			multi_set_passing_wide(CAR *c, short ii, short flag);
extern void			multi_pass_front_car2(CAR *c, long ii);
extern void			multi_check_chase(CAR *c);
extern void			multi_special_speed(CAR *c, long *spd, long *acl);
extern short		multi_check_top_car(short c_car_number);
extern void			multi_set_speed_c_car(CAR *c, short ii);
extern void			multi_get_nearest_link_car(MULTI_CAR *ccar);
