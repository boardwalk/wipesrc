/*** GTE PACKET to-GPU command '<packet-name>.code' ***/
#define GPU_COM_F3    0x20
#define GPU_COM_FT3   0x24
#define GPU_COM_G3    0x30
#define GPU_COM_GT3   0x34

#define GPU_COM_F4    0x28
#define GPU_COM_FT4   0x2c
#define GPU_COM_G4    0x38
#define GPU_COM_GT4   0x3c



#define TMD_OBJSIZE   28
#define TMD_VERTOP     0
#define TMD_VERN       1
#define TMD_NORTOP     2
#define TMD_NORN       3
#define TMD_PRIMTOP    4
#define TMD_PRIMN      5
#define TMD_SCALE      6



typedef	struct {
SVECTOR	*points;
int		no_points;
SVECTOR	*normal;
int		no_normals;
ulong		*faces;
int		no_faces;
int		scale;
} TMDH;



typedef struct {
					u_long 	op;
					u_char	r0, g0, b0, code;
					short		n0,v0,v1,v2;
					} EPOLY_F3;							/* Flat Triangle */


typedef struct {
					u_long	op;
					u_char	r0, g0, b0, code;
					short		n0,v0;
					short		n1,v1;
					short		n2,v2;
					} EPOLY_G3;							/* Flat Triangle */


typedef struct {
					u_long	op;
					u_char	uu0,vv0;
					short		cba;
			 		u_char	uu1,vv1;
					short		tsb;
				 	u_char	uu2,vv2;
					short		pad;
					short		n0,v0;
					short		v1,v2;
					} EPOLY_FT3;							/* Flat Triangle */




typedef struct {
					u_long	op;
					u_char	uu0,vv0;
					short		cba;
			 		u_char	uu1,vv1;
					short		tsb;
				 	u_char	uu2,vv2;
					short		pad;
			 		short		n0,v0;
					short		n1,v1;
					short		n2,v2;
					} EPOLY_GT3;							/* Flat Triangle */




typedef struct {
					u_long	op;
				 	u_char	r0, g0, b0, code;
					short	n0,v0;
					short	v1,v2;
					short	v3,pad1;
					} EPOLY_F4;



typedef struct {
					u_long	op;
				 	u_char	r0, g0, b0, code;
					short	n0,v0;
					short	n1,v1;
					short	n2,v2;
					short	n3,v3;
					} EPOLY_G4;




typedef struct {
					u_long	op;
					u_char	uu0,vv0;
					short		cba;
			 		u_char	uu1,vv1;
					short		tsb;
				 	u_char	uu2,vv2;
					short		pad;
			 	 	u_char	uu3,vv3;
					short		pad2;
					short		n0,v0;
					short		v1,v2;
					short		v3,pad3;
					} EPOLY_FT4;





typedef struct {
					u_long	op;
			  		u_char	uu0,vv0;
					short		cba;
			 		u_char	uu1,vv1;
					short		tsb;
				 	u_char	uu2,vv2;
					short		pad;
			 	 	u_char	uu3,vv3;
					short		pad2;
					short		n0,v0;
					short		n1,v1;
					short		n2,v2;
					short		n3,v3;
					} EPOLY_GT4;



/*--------------------------------------------------------------------------


		GPU primitives



--------------------------------------------------------------------------*/

typedef struct {
	u_long	*tag;
	u_char	r0, g0, b0, code;
	long		xy0;
 	long		xy1;
	long		xy2;
} UPOLY_F3;				/* Flat Triangle */



typedef struct {
	u_long	tag;
	u_char	r0, g0, b0, code;
	long		xy0;
	u_char	r1, g1, b1, pad1;
	long		xy1;
	u_char	r2, g2, b2, pad2;
	long		xy2;
} UPOLY_G3;				/* Gouraud Triangle */


typedef struct {
	u_long	tag;
	u_char	r0, g0, b0, code;
	long		xy0;
	long		uv0;
	long		xy1;
	long		uv1;	
	long		xy2;
	long		uv2;
				} UPOLY_FT3;				/* Flat Textured Triangle */


typedef struct {
	u_long	*tag;
	u_char	r0, g0, b0, code;
	long		xy0;
	long		uv0;
	u_char	r1, g1, b1, p1;
	long		xy1;
	long		uv1;
	u_char	r2, g2, b2, p2;
	long		xy2;
	long	uv2;
} UPOLY_GT3;						/* Gouraud Textured Triangle */


typedef struct {
	u_long	tag;
	u_char	r0, g0, b0, code;
	long		xy0;
 	long		xy1;
	long		xy2;
	long		xy3;
} UPOLY_F4;							/* Flat Quadrangle */

typedef struct {
	u_long	tag;
	u_char	r0, g0, b0, code;
	long		xy0;
	long		uv0;
	long		xy1;
	long		uv1;
	long		xy2;
	long		uv2;
	long		xy3;
	long		uv3;
} UPOLY_FT4;					/* Flat Textured Quadrangle */

typedef struct {
	u_long	tag;
	u_char	r0, g0, b0, code;
	long		xy0;
	u_char	r1, g1, b1, pad1;
	long		xy1;
	u_char	r2, g2, b2, pad2;
	long		xy2;
	u_char	r3, g3, b3, pad3;
	long		xy3;
} UPOLY_G4;					/* Gouraud Quadrangle */


typedef struct {
	u_long	tag;
	u_char	r0, g0, b0, code;
	long		xy0;
	long		uv0;
	u_char	r1, g1, b1, p1;
	long		xy1;
	long		uv1;
	u_char	r2, g2, b2, p2;
	long		xy2;
	long		uv2;
	u_char	r3, g3, b3, p3;
	long		xy3;
	long		uv3;
} UPOLY_GT4;				/* Gouraud Textured Quadrangle */
