#include <sys\types.h>
#include <libgte.h>
#include <libgpu.h>
#include "gpu.h"

void DrawOT(unsigned long *primitive)
{
	u_long addr;
	addr = getaddr(primitive);
	if (addr != 0xffffff) do
		{
		DrawPrim((u_long *)addr);
		primitive = addr;
		}
	while ((addr = getaddr(primitive)) != 0xffffff);

}

void DrawPrim(unsigned long *primitive)
{
	int length;

	switch (((P_TAG *)primitive)->code)
		{
//			case 0x20:	/*Poly F3*/ 
		case 0x24:	/*Poly_FT3*/
//			case 0x30:	/*Poly_g3*/
		case 0x34:	/*Poly_gt3*/
//			case 0x28:	/*Poly_f4*/
		case 0x2c:  /*Poly_ft4*/
//			case 0x38:	/*Poly_g4*/
		case 0x3c:	/*Poly_gt4*/
//			case 0x40:	/*Line_f2*/
//			case 0x50:	/*line_g2*/
// 		case 0x48:	/*Line_F3*/
// 		case 0x58:	/*Line_g3*/
// 		case 0x4c:	/*line_f4*/
// 		case 0x5c:	/*Line_G4*/
		case 0x02:	/*blk_fill*/
		case 0x64:	/*sprt*/
		case 0x7c:	/*spty_16*/
		case 0x74:	/*Sprt_8*/
		case 0x60:	/*tile*/
		case 0x78:	/*tile_16*/
		case 0x70:	/*Tile_8*/
		case 0x68:	/*Tile_1*/
		default:
			printf("Primitive Type %x not supported\n",((P_TAG *)primitive)->code);
			exit(-1);
		}

}
