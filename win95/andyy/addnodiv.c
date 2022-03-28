#include "..\origsrc\common.h"
#include "..\origsrc\rrbios.h"
#include "..\origsrc\bios.h"


#define		d0s		((POLY_FT4 *)s)
#define		NEW_SET_VERTEX2( s, xy1, xy2, xy3, xy4 )	{	\
*(long *)(&(s)->x0) = (xy1);		\
*(long *)(&(s)->x1) = (xy2);		\
*(long *)(&(s)->x2) = (xy3);		\
*(long *)(&(s)->x3) = (xy4);		\
}



typedef struct {
	P_TAG		*ot;
	NEW_RRM_X	*rrm_x;
	RRO_HEAD	*rro_head;
	CVECTOR		amb;
	CVECTOR		dqa;
	RECT		twse;
	long		check_otz;
	long		otzs;
	long		mirror;
	long		p,otz,dummy;
	long		xy[4];
	SVECTOR		v[4];
	VECTOR		v0,v1,v2,v3;
} ADDPOLY;


typedef struct _uv
{
	char u, v;
}UV;



char *addNoDiv(char *s, ADDPOLY *c, VECTOR *m, long n)
{
	long		i,j;
	NEW_RRM_X	*r;
	NEW_DTEX4	*d;
	MATRIX		*mat = (MATRIX *)&c->v;
	
	long		ds,otz,nc1,nc2;
	long 		sh,sv;
	DIVPOLYGON	*divp = (DIVPOLYGON *)(c+1);
	SVECTOR		*sb = (SVECTOR *)&divp->vi0;
	NEW_SVECTOR *vec;
	long		vcount;
	long p;
	unsigned char	temp1, temp2;


	c->dqa.cd = 0x2c;		/* FT4 */

	for (i = 0; i < n; i++, m++)
	{
		if (m->pad < 0) continue;
		mat->t[0] = m->vx;	mat->t[1] = m->vy;	mat->t[2] = m->vz;
		SetTransMatrix( mat );
		r = &c->rrm_x[ m->pad ];

		if(c->mirror){ FlipRotMatrixX(); FlipTRX(); }

		for (vcount =0 , vec = r->verts;vcount <r->vert_num ;vcount ++, vec++ )
	 	{
		  	vec->sz = RotTransPers(&vec->vx,&vec->sxy,&p,&c->dummy);
		  	vec->p = (short)p;
		}

		for (d=r->nodiv_top,j=0;j<r->nodiv_num;j++,d++)
		{
			if (!AJYRDG_NewRRMclip4(d, c, r)) continue; //orientate the whole primitive	
			otz = c->otz;
			if (otz == 0) continue;						//behind camera, forget it
			otz >>= c->otzs;							//make game otz
			if ((otz <= 0)||(otz >= MAIN_OTSIZE)) continue;	// does prim fit in world
			
			if(c->mirror)
			{
				NEW_SET_VERTEX2( d0s, ((NEW_SVECTOR *)(r->verts))[d->v.v1].sxy,
									  ((NEW_SVECTOR *)(r->verts))[d->v.v0].sxy,
									  ((NEW_SVECTOR *)(r->verts))[d->v.v3].sxy,
									  ((NEW_SVECTOR *)(r->verts))[d->v.v2].sxy);

				*(long *)(&d0s->u0) = *(long *)(&d->uv0) ;	//used to have a plus p AJY
				*(long *)(&d0s->u1) = *(long *)(&d->uv1);
				*(short *)(&d0s->u2) = d->uv3;
				*(short *)(&d0s->u3) = d->uv2;

				temp1 = d0s->u0;
				temp2 = d0s->v0;
				d0s->u0 = d0s->u1;
				d0s->v0 = d0s->v1;
				d0s->u1 = temp1;
				d0s->v1 = temp2;
			}
			else
			{
				NEW_SET_VERTEX2( d0s, ((NEW_SVECTOR *)(r->verts))[d->v.v0].sxy,
									  ((NEW_SVECTOR *)(r->verts))[d->v.v1].sxy,
									  ((NEW_SVECTOR *)(r->verts))[d->v.v2].sxy,
									  ((NEW_SVECTOR *)(r->verts))[d->v.v3].sxy);

				*(long *)(&d0s->u0) = *(long *)(&d->uv0) ;	//used to have a plus p AJY
				*(long *)(&d0s->u1) = *(long *)(&d->uv1);
				*(short *)(&d0s->u2) = d->uv2;
				*(short *)(&d0s->u3) = d->uv3;
			}

			d0s->tag = 0x09000000;	d0s->code = 0x2c|0x01;
			DpqColor( &c->dqa, c->p, (long *)(&d0s->r0));  //compute color
			*(long *)(&d0s->r0) = 0x2d0000ff;				/*TEMP CHANGE*/
			ClipPolyFT4( c->ot - (otz + d->z_shift + Z_SHIFT), d0s++ );
			//AddPrim( c->ot - (otz + d->z_shift + Z_SHIFT), d0s++ );
		}

		if(c->mirror){ FlipRotMatrixX(); FlipTRX(); }
	}

	return( s );
}
