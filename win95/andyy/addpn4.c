#include "common.h"
#include "rrbios.h"

#define		GET_VERTEX( s, d )	{				\
	long	r;												\
	r = ((long *)s)[0],	*(long *)(&(d)[0].vx) = r;		\
	r = ((long *)s)[1],	(d)[0].vz = r,	d[1].vx = r >> 16;	\
	r = ((long *)s)[2],	(d)[1].vy = r,	d[1].vz = r >> 16;	\
	r = ((long *)s)[3],	*(long *)(&(d)[2].vx) = r;		\
	r = ((long *)s)[4],	(d)[2].vz = r,	d[3].vx = r >> 16;	\
	r = ((long *)s)[5],	(d)[3].vy = r,	d[3].vz = r >> 16;	\
}

typedef struct {
	P_TAG		*ot;
	RRM_X		*rrm_x;
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
long ADDPNclip4(s, c, n, p, d)
DTEX4			*d;
ADDPOLY			*c;
char				*s;
long				n;
long				p;
{
	GET_VERTEX( &d->v, c->v );
/*	ÀÛ‚É‚Íc->v[0],v[1],v[2],v[3]‚É‚Í‘‚«‚Ü‚ê‚È‚¢B*/
	if (c->mirror)
		{
		c->v[0].vx = -c->v[0].vx;
		c->v[1].vx = -c->v[1].vx;
		c->v[2].vx = -c->v[2].vx;
		c->v[3].vx = -c->v[3].vx;
		}
 	return( RotAverageNclip4(
 			&c->v[0], &c->v[1], &c->v[2], &c->v[3],
 			&c->xy[0], &c->xy[1], &c->xy[2], &c->xy[3],
 			&c->p, &c->otz, &c->dummy ));
 		
}

long ADDP_NormalClip4(s,c,m,n)
char		*s;
ADDPOLY	*c;
VECTOR  *m;
long		n;
{
    long	nc1, nc2, answer;
		nc1 = NormalClip( c->xy[0], c->xy[1], c->xy[2] );
		nc2 = NormalClip( c->xy[3], c->xy[2], c->xy[1] );
	answer = ((nc1 <= 0)&&(nc2 <= 0));
	if (c->mirror)
 		return ( answer );
	else
 		return ( !answer );
}		


