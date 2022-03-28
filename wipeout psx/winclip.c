#include <io.h>
#include <fcntl.h>
#include <math.h>

#include "standard.h"
#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "scene.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "combat.h"

#define SCR_W 320
#define SCR_H 240

#define LEFT		1
#define TOP		8
#define RIGHT		2
#define BOTTOM		4

typedef struct vertinfo
{
	SVECTOR pos;
	short code;
	short pad;
} vertinfo;


short CalcCode(short x, short y)
{

	short code = 0;

	if (x > SCR_W)
		code += RIGHT;
	if (x < 0)
		code += LEFT;
	if (y > SCR_H)
		code += TOP;
	if (y < 0)
		code += BOTTOM;

	return code;
}

short OnScreen(PolyFT3 *ft3)
{
	if (ft3->xy0.vx > SCR_W || ft3->xy0.vx < 0)
		return 0;
	if (ft3->xy1.vx > SCR_W || ft3->xy1.vx < 0)
		return 0;
	if (ft3->xy2.vx > SCR_W || ft3->xy2.vx < 0)
		return 0;
	if (ft3->xy0.vy > SCR_H || ft3->xy0.vy < 0)
		return 0;
	if (ft3->xy1.vy > SCR_H || ft3->xy1.vy < 0)
		return 0;
	if (ft3->xy2.vy > SCR_H || ft3->xy2.vy < 0)
		return 0;

	return 1;
}


short Inside(short boundary, vertinfo p1)
{
	if (p1.code & boundary)
		return 0;
	return 1;
}

short Intersects(short boundary, vertinfo p1, vertinfo p2)
{

	if ((p1.code & boundary) && !(p2.code & boundary)
		|| (p2.code & boundary) && !(p1.code & boundary))
		//does intersect
		return 1;

	return 0;
}

vertinfo ClipBound(short boundary, SVECTOR p1, SVECTOR p2)
{
	short x, y, x0, x1, y0, y1, tmp;
	float fx, fy;
	vertinfo thePoint;
	short theCode;


	x0 = p1.vx;
	x1 = p2.vx;
	y0 = p1.vy;
	y1 = p2.vy;


	switch (boundary)
	{
		case TOP:
			fx = (float)x0 + ((float)x1 - (float)x0) * ((float)SCR_H - (float)y0)/((float)y1 - (float)y0);
			//fx = (float)x0 + 1.0 +((float)x1 - (float)x0) * ((float)SCR_H -
				//(float)y0)/((float)y1 - (float)y0);
			fy = (float)SCR_H;
			break;
		case BOTTOM:
			fx = (float)x0 + ((float)x1 - (float)x0) * (0.0 - (float)y0)/((float)y1-(float)y0);
			//fx = (float)x0 + (float)1 +((float)x1 - (float)x0) * ((float)0 -
				//(float)y0)/((float)y1-(float)y0);
			fy = (float)-0.5;
			break;
		case RIGHT:
//			fy = (float)y0 + (float)1 +((float)y1 - (float)y0) * ((float)SCR_W -
//				(float)x0)/((float)x1 - (float)x0);
			fy = (float)y0 + ((float)y1 - (float)y0) * ((float)SCR_W -
				(float)x0)/((float)x1 - (float)x0);
			fx = (float)SCR_W;
			break;
		case LEFT:
			fy = (float)y0 +((float)y1 - (float)y0) * ((float)0 -
				(float)x0)/((float)x1 - (float)x0);
//			fy = (float)y0 + (float)1 +((float)y1 - (float)y0) * ((float)0 -
//				(float)x0)/((float)x1 - (float)x0);
			fx = (float)-0.5;
			break;
	}

	x = (float)(fx + 0.5);
	y = (float)(fy + 0.5);
	thePoint.pos.vx = x;
	thePoint.pos.vy = y;
	thePoint.code = CalcCode(x,y);

	return thePoint;
}




void ClipPolyF4(u_long *ordt, PolyF4 *prim)
{
	vertinfo vinfo[8];
	vertinfo tmpverts[8];
	vertinfo tmpvert;
	vertinfo *vip;
	short curidx = 0;
	short j;
	short i;
	short docorner;
	vertinfo tmp;
	short xornum;
	short add;
	short cn;
	short doadd;
	short totcode;
	short txv;

	short code;
	short clipline, vertex;
	short nextvert, lastvert;
	vertinfo nextPt, lastPt;
	short nextCode;

	short numpolys;
	PolyF4 *	f4;
	//clip the poly using Sutherland-Hodgman


	//do we want to do a trivial accept?
	vip = &vinfo[0];
	vip->code = CalcCode( prim->xy0.vx, prim->xy0.vy);
	vip++;
	vip->code = CalcCode( prim->xy1.vx, prim->xy1.vy);
	vip++;
	vip->code = CalcCode( prim->xy3.vx, prim->xy3.vy);
	vip++;
	vip->code = CalcCode( prim->xy2.vx, prim->xy2.vy);

	doadd = 0;

	vip = &vinfo[0];
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;

	if (!doadd)
	{
		//poly is all on screen so send it
		AddPrim (ordt, prim);
			prims.f4 += 1;
			return;
	}

	cn = 4;

	totcode = 15; //all planes
	vip = &vinfo[0];
	for (i = 0; i < cn; i++)
	{
		totcode = totcode & vip->code;
		vip++;
	}

	if (totcode)
		return;//doesn't show


	//build up an array of corners

	vip = &vinfo[0];
	vip->pos.vx = prim->xy0.vx;
	vip->pos.vy = prim->xy0.vy;
	vip++;
	vip->pos.vx = prim->xy1.vx;
	vip->pos.vy = prim->xy1.vy;
	vip++;
	vip->pos.vx = prim->xy3.vx;
	vip->pos.vy = prim->xy3.vy;
	vip++;
	vip->pos.vx = prim->xy2.vx;
	vip->pos.vy = prim->xy2.vy;



	//should make this recursive really I s'pose
	for (clipline = 1; clipline <= 8; clipline*=2)
	{
		curidx = 0;

		for (vertex = 0; vertex < cn; vertex++)
		{
			nextvert = vertex + 1;
			if (nextvert == cn)
				nextvert = 0;
			if (Inside(clipline, vinfo[vertex]))
				tmpverts[curidx++] = vinfo[vertex];

			if (Intersects(clipline, vinfo[vertex], vinfo[nextvert]))
				tmpverts[curidx++] = ClipBound(clipline, vinfo[vertex].pos, vinfo[nextvert].pos);
		}

		//do trivial reject if new poly is hidden
		totcode = 15; //all planes
		for (i = 0; i < curidx; i++)
		{
			totcode = totcode &tmpverts[i].code;
		}

		if (totcode)
			return;//doesn't show

		cn = curidx;
		memcpy(vinfo, tmpverts, sizeof(vertinfo) * curidx);
	}

	//fan poly
	numpolys = curidx - 2;

	j = 0;
	for ( i = 0; i < numpolys; i++)
	{
		SetPolyF3( (POLY_F3* )prims.ptr );

		prims.f3->colour0.r = prim->colour0.r;
		prims.f3->colour0.g = prim->colour0.g;
		prims.f3->colour0.b = prim->colour0.b;

		prims.f3->xy0.vx = vinfo[0].pos.vx;
		prims.f3->xy0.vy = vinfo[0].pos.vy;
		prims.f3->xy1.vx = vinfo[i+1].pos.vx;
		prims.f3->xy1.vy = vinfo[i+1].pos.vy;
		prims.f3->xy2.vx = vinfo[i+2].pos.vx;
		prims.f3->xy2.vy = vinfo[i+2].pos.vy;
		//prims.f3->xy1 = vinfo[i+1].pos;
		//prims.f3->xy2 = vinfo[ i+2 ].pos;

        AddPrim( ordt, ( ulong* ) prims.prim );

		prims.f3 += 1;
	}

}




void ClipPolyF3(u_long *ordt, PolyF3 *prim)
{
	vertinfo vinfo[8];
	vertinfo tmpverts[8];
	vertinfo tmpvert;
	vertinfo *vip;
	short curidx = 0;
	short j;
	short i;
	short docorner;
	vertinfo tmp;
	short xornum;
	short add;
	short cn;
	short doadd;
	short totcode;

	short code;
	short clipline, vertex;
	short nextvert, lastvert;
	vertinfo nextPt, lastPt;
	short nextCode;

	short numpolys;
	PolyF3 *	f3;

	//do we want to do a trivial accept?
	vip = &vinfo[0];
	vip->code = CalcCode( prim->xy0.vx, prim->xy0.vy);
	vip++;
	vip->code = CalcCode( prim->xy1.vx, prim->xy1.vy);
	vip++;
	vip->code = CalcCode( prim->xy2.vx, prim->xy2.vy);

	doadd = 0;

	vip = &vinfo[0];
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;

	if (!doadd)
	{
		//poly is all on screen so send it
		AddPrim (ordt, prim);
			prims.f3 += 1;
			return;
	}

	cn = 3;

	totcode = 15; //all planes
	vip = &vinfo[0];
	for (i = 0; i < cn; i++)
	{
		totcode = totcode & vip->code;
		vip++;
	}

	if (totcode)
		return;//doesn't show


	//build up an array of corners

	vip = &vinfo[0];
	vip->pos.vx = prim->xy0.vx;
	vip->pos.vy = prim->xy0.vy;
	vip++;
	vip->pos.vx = prim->xy1.vx;
	vip->pos.vy = prim->xy1.vy;
	vip++;
	vip->pos.vx = prim->xy2.vx;
	vip->pos.vy = prim->xy2.vy;



	//swap round the two bottom vertices to make it easier to work with
	//was in order tl, tr, bl, br
	if (cn == 4)
	{
		tmpvert = vinfo[2];
		vinfo[2] = vinfo[3];
		vinfo[3] = tmpvert;
	}

	//clip the poly using Sutherland-Hodgman
	//should make this recursive really I s'pose
	for (clipline = 1; clipline <= 8; clipline*=2)
	{
		curidx = 0;

		for (vertex = 0; vertex < cn; vertex++)
		{
			nextvert = vertex + 1;
			if (nextvert == cn)
				nextvert = 0;
			if (Inside(clipline, vinfo[vertex]))
				tmpverts[curidx++] = vinfo[vertex];

			if (Intersects(clipline, vinfo[vertex], vinfo[nextvert]))
				tmpverts[curidx++] = ClipBound(clipline, vinfo[vertex].pos, vinfo[nextvert].pos);
		}

		//do trivial reject if new poly is hidden
		totcode = 15; //all planes
		for (i = 0; i < curidx; i++)
		{
			totcode = totcode &tmpverts[i].code;
		}

		if (totcode)
			return;//doesn't show

		memcpy(vinfo, tmpverts, sizeof(vertinfo) * curidx);
		cn = curidx;
	}

	//fan poly
	numpolys = curidx - 2;

	j = 0;
	for ( i = 0; i < numpolys; i++)
	{
		SetPolyF3( (POLY_F3* )prims.ptr );

		prims.f3->colour0.r = prim->colour0.r;
		prims.f3->colour0.g = prim->colour0.g;
		prims.f3->colour0.b = prim->colour0.b;

		prims.f3->xy0.vx = vinfo[0].pos.vx;
		prims.f3->xy0.vy = vinfo[0].pos.vy;
		prims.f3->xy1.vx = vinfo[i+1].pos.vx;
		prims.f3->xy1.vy = vinfo[i+1].pos.vy;
		prims.f3->xy2.vx = vinfo[i+2].pos.vx;
		prims.f3->xy2.vy = vinfo[i+2].pos.vy;
		//prims.f3->xy1 = vinfo[i+1].pos;
		//prims.f3->xy2 = vinfo[ i+2 ].pos;

        AddPrim( ordt, ( ulong* ) prims.prim );

		prims.f3 += 1;
	}

}




void ClipPolyG4(u_long *ordt, PolyG4 *prim)
{
	vertinfo vinfo[8];
	vertinfo tmpverts[8];
	vertinfo tmpvert;
	vertinfo *vip;
	short curidx = 0;
	short j;
	short i;
	short docorner;
	vertinfo tmp;
	short xornum;
	short add;
	short cn;
	short doadd;
	short totcode;

	short code;
	short clipline, vertex;
	short nextvert, lastvert;
	vertinfo nextPt, lastPt;
	short nextCode;

	short numpolys;
	PolyG3 *	g3;



	//do we want to do a trivial accept?
	vip = &vinfo[0];
	vip->code = CalcCode( prim->xy0.vx, prim->xy0.vy);
	vip++;
	vip->code = CalcCode( prim->xy1.vx, prim->xy1.vy);
	vip++;
	vip->code = CalcCode( prim->xy3.vx, prim->xy3.vy);
	vip++;
	vip->code = CalcCode( prim->xy2.vx, prim->xy2.vy);

	doadd = 0;

	vip = &vinfo[0];
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;

	if (!doadd)
	{
		//poly is all on screen so send it
		AddPrim (ordt, prim);
			prims.g4 += 1;
			return;
	}

	cn = 4;

	totcode = 15; //all planes
	vip = &vinfo[0];
	for (i = 0; i < cn; i++)
	{
		totcode = totcode & vip->code;
		vip++;
	}

	if (totcode)
		return;//doesn't show


	//build up an array of corners

	vip = &vinfo[0];
	vip->pos.vx = prim->xy0.vx;
	vip->pos.vy = prim->xy0.vy;
	vip++;
	vip->pos.vx = prim->xy1.vx;
	vip->pos.vy = prim->xy1.vy;
	vip++;
	vip->pos.vx = prim->xy3.vx;
	vip->pos.vy = prim->xy3.vy;
	vip++;
	vip->pos.vx = prim->xy2.vx;
	vip->pos.vy = prim->xy2.vy;


	//clip the poly using Sutherland-Hodgman
	//should make this recursive really I s'pose
	for (clipline = 1; clipline <= 8; clipline*=2)
	{
		curidx = 0;

		for (vertex = 0; vertex < cn; vertex++)
		{
			nextvert = vertex + 1;
			if (nextvert == cn)
				nextvert = 0;
			if (Inside(clipline, vinfo[vertex]))
				tmpverts[curidx++] = vinfo[vertex];

			if (Intersects(clipline, vinfo[vertex], vinfo[nextvert]))
				tmpverts[curidx++] = ClipBound(clipline, vinfo[vertex].pos, vinfo[nextvert].pos);
		}

		//do trivial reject if new poly is hidden
		totcode = 15; //all planes
		for (i = 0; i < curidx; i++)
		{
			totcode = totcode &tmpverts[i].code;
		}

		if (totcode)
			return;//doesn't show

		memcpy(vinfo, tmpverts, sizeof(vertinfo) * curidx);
		cn = curidx;
	}

	//fan poly
	numpolys = curidx - 2;

	for ( i = 0; i < numpolys; i++)
	{
		SetPolyG3( (POLY_G3* )prims.ptr );

		prims.g3->colour0.r = prim->colour0.r;
		prims.g3->colour0.g = prim->colour0.g;
		prims.g3->colour0.b = prim->colour0.b;
		prims.g3->colour1 = prim->colour1;
		prims.g3->colour2 = prim->colour2;

		prims.g3->xy0.vx = vinfo[0].pos.vx;
		prims.g3->xy0.vy = vinfo[0].pos.vy;
		prims.g3->xy1.vx = vinfo[i+1].pos.vx;
		prims.g3->xy1.vy = vinfo[i+1].pos.vy;
		prims.g3->xy2.vx = vinfo[i+2].pos.vx;
		prims.g3->xy2.vy = vinfo[i+2].pos.vy;
		//prims.g3->xy1 = vinfo[i+1].pos;
		//prims.g3->xy2 = vinfo[ i+2 ].pos;

        AddPrim( ordt, ( ulong* ) prims.prim );

		prims.g3 += 1;
	}

}



void ClipPolyG3(u_long *ordt, PolyG3 *prim)
{
	vertinfo vinfo[8];
	vertinfo tmpverts[8];
	vertinfo tmpvert;
	vertinfo *vip;
	short curidx = 0;
	short j;
	short i;
	short docorner;
	vertinfo tmp;
	short xornum;
	short add;
	short cn;
	short doadd;
	short totcode;

	short code;
	short clipline, vertex;
	short nextvert, lastvert;
	vertinfo nextPt, lastPt;
	short nextCode;

	short numpolys;
	PolyG3 *	g3;

	//do we want to do a trivial accept?
	vip = &vinfo[0];
	vip->code = CalcCode( prim->xy0.vx, prim->xy0.vy);
	vip++;
	vip->code = CalcCode( prim->xy1.vx, prim->xy1.vy);
	vip++;
	vip->code = CalcCode( prim->xy2.vx, prim->xy2.vy);

	doadd = 0;

	vip = &vinfo[0];
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;

	if (!doadd)
	{
		//poly is all on screen so send it
		AddPrim (ordt, prim);
			prims.g3 += 1;
			return;
	}

	cn = 3;

	totcode = 15; //all planes
	vip = &vinfo[0];
	for (i = 0; i < cn; i++)
	{
		totcode = totcode & vip->code;
		vip++;
	}

	if (totcode)
		return;//doesn't show
	//build up an array of corners

	vip = &vinfo[0];
	vip->pos.vx = prim->xy0.vx;
	vip->pos.vy = prim->xy0.vy;
	vip++;
	vip->pos.vx = prim->xy1.vx;
	vip->pos.vy = prim->xy1.vy;
	vip++;
	vip->pos.vx = prim->xy2.vx;
	vip->pos.vy = prim->xy2.vy;




	//clip the poly using Sutherland-Hodgman
	//should make this recursive really I s'pose
	for (clipline = 1; clipline <= 8; clipline*=2)
	{
		curidx = 0;

		for (vertex = 0; vertex < cn; vertex++)
		{
			nextvert = vertex + 1;
			if (nextvert == cn)
				nextvert = 0;
			if (Inside(clipline, vinfo[vertex]))
				tmpverts[curidx++] = vinfo[vertex];

			if (Intersects(clipline, vinfo[vertex], vinfo[nextvert]))
				tmpverts[curidx++] = ClipBound(clipline, vinfo[vertex].pos, vinfo[nextvert].pos);
		}

		//do trivial reject if new poly is hidden
		totcode = 15; //all planes
		for (i = 0; i < curidx; i++)
		{
			totcode = totcode &tmpverts[i].code;
		}

		if (totcode)
			return;//doesn't show

		memcpy(vinfo, tmpverts, sizeof(vertinfo) * curidx);
		cn = curidx;
	}

	//fan poly
	numpolys = curidx - 2;

	for ( i = 0; i < numpolys; i++)
	{
		SetPolyG3( (POLY_G3* )prims.ptr );

		prims.g3->colour0.r = prim->colour0.r;
		prims.g3->colour0.g = prim->colour0.g;
		prims.g3->colour0.b = prim->colour0.b;
		prims.g3->colour1 = prim->colour1;
		prims.g3->colour2 = prim->colour2;

		prims.g3->xy0.vx = vinfo[0].pos.vx;
		prims.g3->xy0.vy = vinfo[0].pos.vy;
		prims.g3->xy1.vx = vinfo[i+1].pos.vx;
		prims.g3->xy1.vy = vinfo[i+1].pos.vy;
		prims.g3->xy2.vx = vinfo[i+2].pos.vx;
		prims.g3->xy2.vy = vinfo[i+2].pos.vy;
		//prims.g3->xy1 = vinfo[i+1].pos;
		//prims.g3->xy2 = vinfo[ i+2 ].pos;

        AddPrim( ordt, ( ulong* ) prims.prim );

		prims.g3 += 1;
	}

}



void ClipPolyFT4(u_long *ordt, PolyFT4 *prim)
{
	vertinfo vinfo[8];
	vertinfo tmpverts[8];
	vertinfo textvert1[8];
	vertinfo textvert2[8];
	vertinfo *tmpptr;
	vertinfo tmpvert;
	vertinfo *vip;
	DVECTOR tmpdv;
	short minu, maxu, minv, maxv;
	short curidx = 0;
	short j;
	short i;
	short docorner;
	vertinfo tmp;
	short xornum;
	short add;
	short cn;
	short doadd;
	short totcode;
	float rpx, rpy;

	short code;
	short clipline, vertex;
	short nextvert, lastvert;
	vertinfo nextPt, lastPt;
	short nextCode;
	float dx, dy;

	short numpolys;
	PolyFT3 *	ft3;


	//do we want to do a trivial accept?
	vip = &vinfo[0];
	vip->code = CalcCode( prim->xy0.vx, prim->xy0.vy);
	vip++;
	vip->code = CalcCode( prim->xy1.vx, prim->xy1.vy);
	vip++;
	vip->code = CalcCode( prim->xy3.vx, prim->xy3.vy);
	vip++;
	vip->code = CalcCode( prim->xy2.vx, prim->xy2.vy);

	doadd = 0;

	vip = &vinfo[0];
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;

	if (!doadd)
	{
		//poly is all on screen so send it
		AddPrim (ordt, prim);
			prims.ft4 += 1;
			return;
	}

	//trivial reject?
	cn = 4;
	totcode = 15; //all planes
	vip = &vinfo[0];
	for (i = 0; i < cn; i++)
	{
		totcode = totcode & vip->code;
		vip++;
	}

	if (totcode)
		return;//doesn't show


	//build up an array of corners

	vip = &vinfo[0];
	vip->pos.vx = prim->xy0.vx;
	vip->pos.vy = prim->xy0.vy;
	vip->pos.vz = prim->xy0.vz;
	vip->pos.pad = prim->xy0.pad;
	vip++;
	vip->pos.vx = prim->xy1.vx;
	vip->pos.vy = prim->xy1.vy;
	vip->pos.vz = prim->xy1.vz;
	vip->pos.pad = prim->xy1.pad;
	vip++;
	vip->pos.vx = prim->xy3.vx;
	vip->pos.vy = prim->xy3.vy;
	vip->pos.vz = prim->xy3.vz;
	vip->pos.pad = prim->xy3.pad;
	vip++;
	vip->pos.vx = prim->xy2.vx;
	vip->pos.vy = prim->xy2.vy;
	vip->pos.vz = prim->xy2.vz;
	vip->pos.pad = prim->xy2.pad;


	//hold texture corners
	vip = &textvert1[0];
	vip->pos.vx = prim->u0;
	vip->pos.vy = prim->v0;
	vip++;
	vip->pos.vx = prim->u1;
	vip->pos.vy = prim->v1;
	vip++;
	vip->pos.vx = prim->u3;
	vip->pos.vy = prim->v3;
	vip++;
	vip->pos.vx = prim->u2;
	vip->pos.vy = prim->v2;



	//clip the poly using Sutherland-Hodgman
	//should make this recursive really I s'pose
	for (clipline = 1; clipline <= 8; clipline*=2)
	{
		curidx = 0;

		for (vertex = 0; vertex < cn; vertex++)
		{
			nextvert = vertex + 1;
			if (nextvert == cn)
				nextvert = 0;
			if (Inside(clipline, vinfo[vertex]))
			{
				textvert2[curidx] = textvert1[vertex];
				tmpverts[curidx++] = vinfo[vertex];
			}

			if (Intersects(clipline, vinfo[vertex], vinfo[nextvert]))
			{
				tmpverts[curidx] = ClipBound(clipline, vinfo[vertex].pos, vinfo[nextvert].pos);

				dx = fabs((double)((float)vinfo[nextvert].pos.vx - (float)vinfo[vertex].pos.vx));
				dy = fabs((double)((float)vinfo[nextvert].pos.vy - (float)vinfo[vertex].pos.vy));

				if (dx >= dy)
				{
					if (vinfo[vertex].pos.vx != vinfo[nextvert].pos.vx)
						rpx = fabs((double)((float)tmpverts[curidx].pos.vx - (float)vinfo[vertex].pos.vx) /dx);
					else
						rpx = 0;
				} else {
					if (vinfo[vertex].pos.vy != vinfo[nextvert].pos.vy)
						rpx = fabs((double)((float)tmpverts[curidx].pos.vy - (float)vinfo[vertex].pos.vy) /dy);
					else
						rpx = 0;
				}

				textvert2[curidx].pos.vx = (short)
					((float)textvert1[vertex].pos.vx + ((float)textvert1[nextvert].pos.vx - (float)textvert1[vertex].pos.vx) * rpx + 0.5);
				textvert2[curidx].pos.vy = (short)
					((float)textvert1[vertex].pos.vy + ((float)textvert1[nextvert].pos.vy - (float)textvert1[vertex].pos.vy) * rpx + 0.5);
				tmpverts[curidx].pos.vz = (short)
					((float)vinfo[vertex].pos.vz + ((float)vinfo[nextvert].pos.vz - (float)vinfo[vertex].pos.vz) * rpx + 0.5);
				tmpverts[curidx].pos.pad = vinfo[vertex].pos.pad;

				curidx++;
			}
		}

		//do trivial reject if new poly is hidden
		totcode = 15; //all planes
		for (i = 0; i < curidx; i++)
		{
			totcode = totcode & tmpverts[i].code;
		}

		if (totcode)
			return;//doesn't show

		memcpy(vinfo, tmpverts, sizeof(vertinfo) * curidx);
		memcpy(textvert1, textvert2, sizeof(vertinfo) * curidx);
		cn = curidx;
	}

	//fan poly
	if (curidx < 3)
		return;
	numpolys = curidx - 2;

	for ( i = 0; i < numpolys; i++)
	{
		if ( vinfo[i].pos.vy <  10 ||
		vinfo[i + 1].pos.vy <  10 ||
		vinfo[i + 2].pos.vy <  10)
		{
			rpx = i;
			rpy = rpx;
		}

		SetPolyFT3( (POLY_FT3* )prims.ptr );

		prims.ft3->colour0.r = prim->colour0.r;
		prims.ft3->colour0.g = prim->colour0.g;
		prims.ft3->colour0.b = prim->colour0.b;

		prims.ft3->xy0 = vinfo[0].pos;
		prims.ft3->xy1 = vinfo[i+1].pos;
		prims.ft3->xy2 = vinfo[ i+2 ].pos;

		prims.ft3->u0 = textvert1[0].pos.vx;
		prims.ft3->v0 = textvert1[0].pos.vy;
		prims.ft3->u1 = textvert1[i + 1].pos.vx;
		prims.ft3->v1 = textvert1[i + 1].pos.vy;
		prims.ft3->u2 = textvert1[i + 2].pos.vx;
		prims.ft3->v2 = textvert1[i + 2].pos.vy;

		prims.ft3->cba = prim->cba;
		prims.ft3->tsb = prim->tsb;


        AddPrim( ordt, ( ulong* ) prims.prim );

		prims.ft3 += 1;
	}

}



void ClipPolyFT3(u_long *ordt, PolyFT3 *prim)
{
	vertinfo vinfo[8];
	vertinfo tmpverts[8];
	vertinfo textvert1[8];
	vertinfo textvert2[8];
	vertinfo *vip;
	vertinfo *tmpptr;
	vertinfo tmpvert;
	DVECTOR tmpdv;
	short minu, maxu, minv, maxv;
	short curidx = 0;
	short j;
	short i;
	short docorner;
	vertinfo tmp;
	short xornum;
	short add;
	short cn;
	short doadd;
	short totcode;
	float rpx, rpy;

	short code;
	short clipline, vertex;
	short nextvert, lastvert;
	vertinfo nextPt, lastPt;
	short nextCode;
	float dx, dy;

	short numpolys;
	PolyFT3 *	ft3;


	//do we want to do a trivial accept?
	vip = &vinfo[0];
	vip->code = CalcCode( prim->xy0.vx, prim->xy0.vy);
	vip++;
	vip->code = CalcCode( prim->xy1.vx, prim->xy1.vy);
	vip++;
	vip->code = CalcCode( prim->xy2.vx, prim->xy2.vy);

	doadd = 0;

	vip = &vinfo[0];
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;

	if (!doadd)
	{
		//poly is all on screen so send it
		AddPrim (ordt, prim);
			prims.ft3 += 1;
			return;
	}

	cn = 3;
	//trivial reject?

	totcode = 15; //all planes
	vip = &vinfo[0];
	for (i = 0; i < cn; i++)
	{
		totcode = totcode & vip->code;
		vip++;
	}

	if (totcode)
		return;//doesn't show

	//build up an array of corners

	vip = &vinfo[0];
	vip->pos.vx = prim->xy0.vx;
	vip->pos.vy = prim->xy0.vy;
	vip->pos.vz = prim->xy0.vz;
	vip->pos.pad = prim->xy0.pad;
	vip++;
	vip->pos.vx = prim->xy1.vx;
	vip->pos.vy = prim->xy1.vy;
	vip->pos.vz = prim->xy1.vz;
	vip->pos.pad = prim->xy1.pad;
	vip++;
	vip->pos.vx = prim->xy2.vx;
	vip->pos.vy = prim->xy2.vy;
	vip->pos.vz = prim->xy2.vz;
	vip->pos.pad = prim->xy2.pad;


	//hold texture corners
	vip = &textvert1[0];
	vip->pos.vx = prim->u0;
	vip->pos.vy = prim->v0;
	vip++;
	vip->pos.vx = prim->u1;
	vip->pos.vy = prim->v1;
	vip++;
	vip->pos.vx = prim->u2;
	vip->pos.vy = prim->v2;

	cn = 3;

	//clip the poly using Sutherland-Hodgman
	//should make this recursive really I s'pose
	for (clipline = 1; clipline <= 8; clipline*=2)
	{
		curidx = 0;

		for (vertex = 0; vertex < cn; vertex++)
		{
			nextvert = vertex + 1;
			if (nextvert == cn)
				nextvert = 0;
			if (Inside(clipline, vinfo[vertex]))
			{
				textvert2[curidx] = textvert1[vertex];
				tmpverts[curidx++] = vinfo[vertex];
			}

			if (Intersects(clipline, vinfo[vertex], vinfo[nextvert]))
			{
				tmpverts[curidx] = ClipBound(clipline, vinfo[vertex].pos, vinfo[nextvert].pos);

				dx = fabs((double)((float)vinfo[nextvert].pos.vx - (float)vinfo[vertex].pos.vx));
				dy = fabs((double)((float)vinfo[nextvert].pos.vy - (float)vinfo[vertex].pos.vy));

				if (dx >= dy)
				{
					if (vinfo[vertex].pos.vx != vinfo[nextvert].pos.vx)
						rpx = fabs((double)((float)tmpverts[curidx].pos.vx - (float)vinfo[vertex].pos.vx) /dx);
					else
						rpx = 0;
				} else {
					if (vinfo[vertex].pos.vy != vinfo[nextvert].pos.vy)
						rpx = fabs((double)((float)tmpverts[curidx].pos.vy - (float)vinfo[vertex].pos.vy) /dy);
					else
						rpx = 0;
				}

				textvert2[curidx].pos.vx = (short)
					((float)textvert1[vertex].pos.vx + ((float)textvert1[nextvert].pos.vx - (float)textvert1[vertex].pos.vx) * rpx + 0.5);
				textvert2[curidx].pos.vy = (short)
					((float)textvert1[vertex].pos.vy + ((float)textvert1[nextvert].pos.vy - (float)textvert1[vertex].pos.vy) * rpx + 0.5);
				tmpverts[curidx].pos.vz = (short)
					((float)vinfo[vertex].pos.vz + ((float)vinfo[nextvert].pos.vz - (float)vinfo[vertex].pos.vz) * rpx + 0.5);
				tmpverts[curidx].pos.pad = vinfo[vertex].pos.pad;

				curidx++;
			}
		}

		//do trivial reject if new poly is hidden
		totcode = 15; //all planes
		for (i = 0; i < curidx; i++)
		{
			totcode = totcode & tmpverts[i].code;
		}

		if (totcode)
			return;//doesn't show

		memcpy(vinfo, tmpverts, sizeof(vertinfo) * curidx);
		memcpy(textvert1, textvert2, sizeof(vertinfo) * curidx);
		cn = curidx;
	}

	//fan poly
	if (curidx < 3)
		return;
	numpolys = curidx - 2;

	for ( i = 0; i < numpolys; i++)
	{
		SetPolyFT3( (POLY_FT3* )prims.ptr );

		prims.ft3->colour0.r = prim->colour0.r;
		prims.ft3->colour0.g = prim->colour0.g;
		prims.ft3->colour0.b = prim->colour0.b;

		prims.ft3->xy0 = vinfo[0].pos;
		prims.ft3->xy1 = vinfo[i+1].pos;
		prims.ft3->xy2 = vinfo[ i+2 ].pos;

		prims.ft3->u0 = textvert1[0].pos.vx;
		prims.ft3->v0 = textvert1[0].pos.vy;
		prims.ft3->u1 = textvert1[i + 1].pos.vx;
		prims.ft3->v1 = textvert1[i + 1].pos.vy;
		prims.ft3->u2 = textvert1[i + 2].pos.vx;
		prims.ft3->v2 = textvert1[i + 2].pos.vy;

		prims.ft3->cba = prim->cba;
		prims.ft3->tsb = prim->tsb;


        AddPrim( ordt, ( ulong* ) prims.prim );

		prims.ft3 += 1;
	}

}



void ClipPolyGT4(u_long *ordt, PolyGT4 *prim)
{
	vertinfo vinfo[8];
	vertinfo *vip;
	vertinfo tmpverts[8];
	vertinfo textvert1[8];
	vertinfo textvert2[8];
	vertinfo *tmpptr;
	vertinfo tmpvert;
	DVECTOR tmpdv;
	short minu, maxu, minv, maxv;
	short curidx = 0;
	short j;
	short i;
	short docorner;
	vertinfo tmp;
	short xornum;
	short add;
	short cn;
	short doadd;
	short totcode;
	float rpx, rpy;

	short code;
	short clipline, vertex;
	short nextvert, lastvert;
	vertinfo nextPt, lastPt;
	short nextCode;
	float dx, dy;

	short numpolys;
	PolyGT3 *	gt3;

	//do we want to do a trivial accept?
	vip = &vinfo[0];
	vip->code = CalcCode( prim->xy0.vx, prim->xy0.vy);
	vip++;
	vip->code = CalcCode( prim->xy1.vx, prim->xy1.vy);
	vip++;
	vip->code = CalcCode( prim->xy3.vx, prim->xy3.vy);
	vip++;
	vip->code = CalcCode( prim->xy2.vx, prim->xy2.vy);

	doadd = 0;

	vip = &vinfo[0];
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;

	if (!doadd)
	{
		//poly is all on screen so send it
		AddPrim (ordt, prim);
			prims.gt4 += 1;
			return;
	}

	cn = 4;

	totcode = 15; //all planes
	vip = &vinfo[0];
	for (i = 0; i < cn; i++)
	{
		totcode = totcode & vip->code;
		vip++;
	}

	if (totcode)
		return;//doesn't show


	//build up an array of corners

	vip = &vinfo[0];
	vip->pos.vx = prim->xy0.vx;
	vip->pos.vy = prim->xy0.vy;
	vip->pos.vz = prim->xy0.vz;
	vip->pos.pad = prim->xy0.pad;
	vip++;
	vip->pos.vx = prim->xy1.vx;
	vip->pos.vy = prim->xy1.vy;
	vip->pos.vz = prim->xy1.vz;
	vip->pos.pad = prim->xy1.pad;
	vip++;
	vip->pos.vx = prim->xy3.vx;
	vip->pos.vy = prim->xy3.vy;
	vip->pos.vz = prim->xy3.vz;
	vip->pos.pad = prim->xy3.pad;
	vip++;
	vip->pos.vx = prim->xy2.vx;
	vip->pos.vy = prim->xy2.vy;
	vip->pos.vz = prim->xy2.vz;
	vip->pos.pad = prim->xy2.pad;


	//hold texture corners
	vip = &textvert1[0];
	vip->pos.vx = prim->u0;
	vip->pos.vy = prim->v0;
	vip++;
	vip->pos.vx = prim->u1;
	vip->pos.vy = prim->v1;
	vip++;
	vip->pos.vx = prim->u3;
	vip->pos.vy = prim->v3;
	vip++;
	vip->pos.vx = prim->u2;
	vip->pos.vy = prim->v2;


	//clip the poly using Sutherland-Hodgman
	//should make this recursive really I s'pose
	for (clipline = 1; clipline <= 8; clipline*=2)
	{
		curidx = 0;

		for (vertex = 0; vertex < cn; vertex++)
		{
			nextvert = vertex + 1;
			if (nextvert == cn)
				nextvert = 0;
			if (Inside(clipline, vinfo[vertex]))
			{
				textvert2[curidx] = textvert1[vertex];
				tmpverts[curidx++] = vinfo[vertex];
			}

			if (Intersects(clipline, vinfo[vertex], vinfo[nextvert]))
			{
				tmpverts[curidx] = ClipBound(clipline, vinfo[vertex].pos, vinfo[nextvert].pos);

				dx = fabs((double)((float)vinfo[nextvert].pos.vx - (float)vinfo[vertex].pos.vx));
				dy = fabs((double)((float)vinfo[nextvert].pos.vy - (float)vinfo[vertex].pos.vy));

				if (dx >= dy)
				{
					if (vinfo[vertex].pos.vx != vinfo[nextvert].pos.vx)
						rpx = fabs((double)((float)tmpverts[curidx].pos.vx - (float)vinfo[vertex].pos.vx) /dx);
					else
						rpx = 0;
				} else {
					if (vinfo[vertex].pos.vy != vinfo[nextvert].pos.vy)
						rpx = fabs((double)((float)tmpverts[curidx].pos.vy - (float)vinfo[vertex].pos.vy) /dy);
					else
						rpx = 0;
				}

				textvert2[curidx].pos.vx = (short)
					((float)textvert1[vertex].pos.vx + ((float)textvert1[nextvert].pos.vx - (float)textvert1[vertex].pos.vx) * rpx + 0.5);
				textvert2[curidx].pos.vy = (short)
					((float)textvert1[vertex].pos.vy + ((float)textvert1[nextvert].pos.vy - (float)textvert1[vertex].pos.vy) * rpx + 0.5);
				tmpverts[curidx].pos.vz = (short)
					((float)vinfo[vertex].pos.vz + ((float)vinfo[nextvert].pos.vz - (float)vinfo[vertex].pos.vz) * rpx + 0.5);
				tmpverts[curidx].pos.pad = vinfo[vertex].pos.pad;

				curidx++;
			}
		}

		//do trivial reject if new poly is hidden
		totcode = 15; //all planes
		for (i = 0; i < curidx; i++)
		{
			totcode = totcode & tmpverts[i].code;
		}

		if (totcode)
			return;//doesn't show

		memcpy(vinfo, tmpverts, sizeof(vertinfo) * curidx);
		memcpy(textvert1, textvert2, sizeof(vertinfo) * curidx);

		cn = curidx;
	}

	//fan poly
	if (curidx < 3)
		return;
	numpolys = curidx - 2;

	for ( i = 0; i < numpolys; i++)
	{
		SetPolyGT3( (POLY_GT3* )prims.ptr );

		prims.gt3->colour0.r = prim->colour0.r;
		prims.gt3->colour0.g = prim->colour0.g;
		prims.gt3->colour0.b = prim->colour0.b;
		prims.gt3->colour1 = prim->colour1;
		prims.gt3->colour2 = prim->colour2;

		prims.gt3->xy0 = vinfo[0].pos;
		prims.gt3->xy1 = vinfo[i+1].pos;
		prims.gt3->xy2 = vinfo[ i+2 ].pos;

		prims.gt3->u0 = textvert1[0].pos.vx;
		prims.gt3->v0 = textvert1[0].pos.vy;
		prims.gt3->u1 = textvert1[i + 1].pos.vx;
		prims.gt3->v1 = textvert1[i + 1].pos.vy;
		prims.gt3->u2 = textvert1[i + 2].pos.vx;
		prims.gt3->v2 = textvert1[i + 2].pos.vy;

		prims.gt3->cba = prim->cba;
		prims.gt3->tsb = prim->tsb;

        AddPrim( ordt, ( ulong* ) prims.prim );

		prims.gt3 += 1;
	}

}



void ClipPolyGT3(u_long *ordt, PolyGT3 *prim)
{
	vertinfo vinfo[8];
	vertinfo tmpverts[8];
	vertinfo textvert1[8];
	vertinfo textvert2[8];
	vertinfo *vip;
	vertinfo *tmpptr;
	vertinfo tmpvert;
	DVECTOR tmpdv;
	short minu, maxu, minv, maxv;
	short curidx = 0;
	short j;
	short i;
	short docorner;
	vertinfo tmp;
	short xornum;
	short add;
	short cn;
	short doadd;
	short totcode;
	float rpx, rpy;

	short code;
	short clipline, vertex;
	short nextvert, lastvert;
	vertinfo nextPt, lastPt;
	short nextCode;
	float dx, dy;

	short numpolys;
	PolyGT3 *	gt3;


	//do we want to do a trivial accept?
	vip = &vinfo[0];
	vip->code = CalcCode( prim->xy0.vx, prim->xy0.vy);
	vip++;
	vip->code = CalcCode( prim->xy1.vx, prim->xy1.vy);
	vip++;
	vip->code = CalcCode( prim->xy2.vx, prim->xy2.vy);

	doadd = 0;

	vip = &vinfo[0];
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;
	vip++;
	if (vip->code)
		doadd = 1;

	if (!doadd)
	{
		//poly is all on screen so send it
		AddPrim (ordt, prim);
			prims.gt3 += 1;
			return;
	}

	cn = 3;

	totcode = 15; //all planes
	vip = &vinfo[0];
	for (i = 0; i < cn; i++)
	{
		totcode = totcode & vip->code;
		vip++;
	}

	if (totcode)
		return;//doesn't show


	//build up an array of corners

	vip = &vinfo[0];
	vip->pos.vx = prim->xy0.vx;
	vip->pos.vy = prim->xy0.vy;
	vip->pos.vz = prim->xy0.vz;
	vip->pos.pad = prim->xy0.pad;
	vip++;
	vip->pos.vx = prim->xy1.vx;
	vip->pos.vy = prim->xy1.vy;
	vip->pos.vz = prim->xy1.vz;
	vip->pos.pad = prim->xy1.pad;
	vip++;
	vip->pos.vx = prim->xy2.vx;
	vip->pos.vy = prim->xy2.vy;
	vip->pos.vz = prim->xy2.vz;
	vip->pos.pad = prim->xy2.pad;


	//hold texture corners
	vip = &textvert1[0];
	vip->pos.vx = prim->u0;
	vip->pos.vy = prim->v0;
	vip++;
	vip->pos.vx = prim->u1;
	vip->pos.vy = prim->v1;
	vip++;
	vip->pos.vx = prim->u2;
	vip->pos.vy = prim->v2;



	//clip the poly using Sutherland-Hodgman
	//should make this recursive really I s'pose
	for (clipline = 1; clipline <= 8; clipline*=2)
	{
		curidx = 0;

		for (vertex = 0; vertex < cn; vertex++)
		{
			nextvert = vertex + 1;
			if (nextvert == cn)
				nextvert = 0;
			if (Inside(clipline, vinfo[vertex]))
			{
				textvert2[curidx] = textvert1[vertex];
				tmpverts[curidx++] = vinfo[vertex];
			}

			if (Intersects(clipline, vinfo[vertex], vinfo[nextvert]))
			{
				tmpverts[curidx] = ClipBound(clipline, vinfo[vertex].pos, vinfo[nextvert].pos);

				dx = fabs((double)((float)vinfo[nextvert].pos.vx - (float)vinfo[vertex].pos.vx));
				dy = fabs((double)((float)vinfo[nextvert].pos.vy - (float)vinfo[vertex].pos.vy));

				if (dx >= dy)
				{
					if (vinfo[vertex].pos.vx != vinfo[nextvert].pos.vx)
						rpx = fabs((double)((float)tmpverts[curidx].pos.vx - (float)vinfo[vertex].pos.vx) /dx);
					else
						rpx = 0;
				} else {
					if (vinfo[vertex].pos.vy != vinfo[nextvert].pos.vy)
						rpx = fabs((double)((float)tmpverts[curidx].pos.vy - (float)vinfo[vertex].pos.vy) /dy);
					else
						rpx = 0;
				}

				textvert2[curidx].pos.vx = (short)
					((float)textvert1[vertex].pos.vx + ((float)textvert1[nextvert].pos.vx - (float)textvert1[vertex].pos.vx) * rpx + 0.5);
				textvert2[curidx].pos.vy = (short)
					((float)textvert1[vertex].pos.vy + ((float)textvert1[nextvert].pos.vy - (float)textvert1[vertex].pos.vy) * rpx + 0.5);
				tmpverts[curidx].pos.vz = (short)
					((float)vinfo[vertex].pos.vz + ((float)vinfo[nextvert].pos.vz - (float)vinfo[vertex].pos.vz) * rpx + 0.5);
				tmpverts[curidx].pos.pad = vinfo[vertex].pos.pad;

				curidx++;
			}
		}

		//do trivial reject if new poly is hidden
		totcode = 15; //all planes
		for (i = 0; i < curidx; i++)
		{
			totcode = totcode & tmpverts[i].code;
		}

		if (totcode)
			return;//doesn't show

		memcpy(vinfo, tmpverts, sizeof(vertinfo) * curidx);
		memcpy(textvert1, textvert2, sizeof(vertinfo) * curidx);
		cn = curidx;
	}

	//fan poly
	if (curidx < 3)
		return;
	numpolys = curidx - 2;

	for ( i = 0; i < numpolys; i++)
	{
		SetPolyGT3( (POLY_GT3* )prims.ptr );

		prims.gt3->colour0.r = prim->colour0.r;
		prims.gt3->colour0.g = prim->colour0.g;
		prims.gt3->colour0.b = prim->colour0.b;
		prims.gt3->colour1 = prim->colour1;
		prims.gt3->colour2 = prim->colour2;

		prims.gt3->xy0 = vinfo[0].pos;
		prims.gt3->xy1 = vinfo[i+1].pos;
		prims.gt3->xy2 = vinfo[ i+2 ].pos;

		prims.gt3->u0 = textvert1[0].pos.vx;
		prims.gt3->v0 = textvert1[0].pos.vy;
		prims.gt3->u1 = textvert1[i + 1].pos.vx;
		prims.gt3->v1 = textvert1[i + 1].pos.vy;
		prims.gt3->u2 = textvert1[i + 2].pos.vx;
		prims.gt3->v2 = textvert1[i + 2].pos.vy;

		prims.gt3->cba = prim->cba;
		prims.gt3->tsb = prim->tsb;

        AddPrim( ordt, ( ulong* ) prims.prim );

		prims.gt3 += 1;
	}

}
