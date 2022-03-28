#define		M_INIT			0	/* px,py,pz, rx,ry,rz */
#define		M_GO			1	/* target_px,py,pz, zone, frame */
#define		M_STOP			2	/* frame */
#define		M_SET_ROT_X		3	/* rx */
#define		M_SET_ROT_Y		4	/* ry */
#define		M_SET_ROT_Z		5	/* rz */
#define		M_ROT_X			6	/* target_rx, drx */
#define		M_ROT_Y			7	/* target_ry, dry */
#define		M_ROT_Z			8	/* target_rz, drz */
#define		M_RESET			9	/* */
#define		MIN_DDIR		0x10

#define		KICK_MAX		(6)

extern KICK		kick[];
extern long		kick_press;
extern long		fall_down_th[];

extern long		old_hd;
extern short	heli_start[];
extern JETS		jet;

extern void		multi_move_heli(void);
extern void		multi_move_jet(void);
extern void		multi_move_kick(MULTI_CAR *mc);
extern long		multi_hit_kick(KICK *k, MULTI_CAR *mc);
extern long		multi_hit_kick_tire(KICK *k, MULTI_CAR *mc);
