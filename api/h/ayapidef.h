#ifndef _AYAPIDEF_
#define	_AYAPIDEF_

/*definition of on and off for SetDisplayMask*/
#define	AY_ON	1
#define	AY_OFF	0


/*definition of driver types*/
#define	AY_DRIVER_DEFAULT	0
#define	AY_DRIVER_NVIDIA	1
#define	AY_DRIVER_S3		2
#define	AY_DRIVER_VL		3
#define	AY_DRIVER_ATI		4
#define	AY_DRIVER_RENDITION	5
#define	AY_DRIVER_3DFX		6

/*error codes we know about*/
#define	AY_SUCCESS			0
#define	AY_ERR_DRIVER		1
#define	AY_ERR_LOAD			2
#define	AY_ERR_NOCARD		3
#define	AY_ERR_CARDFAIL		4
#define	AY_ERR_MEMORY		5
#define	AY_ERR_RESOURCE		6
#define	AY_ERR_BUFFER		7
#define	AY_ERR_MODE			8
#define	AY_ERR_NODRIVER		9
#define	AY_ERR_COLDEPTH		10
#define	AY_ERR_TPAGE		11
#define	AY_ERR_COORD		12
#define	AY_ERR_PRIMITIVE	13
#define	AY_ERR_CORRUPT		14

#define	AY_ERR_UNKNOWN		-1

/*texture types*/
#define	AY_TEX_4BIT			1
#define	AY_TEX_8BITPAL		2
#define	AY_TEX_8BIT332		3
#define	AY_TEX_16BIT		4
#define	AY_TEX_24BIT		5

/*primitive types*/
#define	AY_PRIM_F1			1
#define	AY_PRIM_F2			2
#define	AY_PRIM_F3			3
#define	AY_PRIM_F4			4
#define	AY_PRIM_G1			5
#define	AY_PRIM_G2			6
#define	AY_PRIM_G3			7
#define	AY_PRIM_G4			8
#define	AY_PRIM_FT1			9
#define	AY_PRIM_FT2			10
#define	AY_PRIM_FT3			11
#define	AY_PRIM_FT4			12
#define	AY_PRIM_GT1			13
#define	AY_PRIM_GT2			14
#define	AY_PRIM_GT3			15
#define	AY_PRIM_GT4			16
#define	AY_PRIM_SP8			17
#define	AY_PRIM_SP16		18
#define	AY_PRIM_SP			19
#define	AY_PRIM_TI1			20
#define	AY_PRIM_TI8			21
#define	AY_PRIM_TI16		22
#define	AY_PRIM_TI			23
#define	AY_PRIM_CLS			24
#define	AY_PRIM_CB			25



#endif
