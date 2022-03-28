#include "..\origsrc\common.h"
#define VAR_EXTERN
#include "..\origsrc\var.h"



void TweenByte(u_char *in1, u_short *tween, u_char *rtn)
{
	rtn[0] = in1[0] + (tween[0] >> 8);
	rtn[1] = in1[1] + (tween[1] >> 8);
}



void TweenShort(SVECTOR *vi0, VECTOR *tween, SVECTOR *vt1)
{
	vt1->vx = vi0->vx + (tween->vx >> 16);
	vt1->vy = vi0->vy + (tween->vy >> 16);
	vt1->vz = vi0->vz + (tween->vz >> 16);
}

void MakeShortStep(SVECTOR *ip1, SVECTOR *ip2, VECTOR *op, int div)
{
	op->vx = (ip2->vx - ip1->vx) << (16 - div);		//generate our top and bottom steps
	op->vy = (ip2->vy - ip1->vy) << (16 - div);		//might need a bit of fixed pointedness
	op->vz = (ip2->vz - ip1->vz) << (16 - div);
}

void MakeByteStep(u_char *ip1, u_char *ip2, u_short *op, int div)
{
	op[0] = (ip2[0] - ip1[0]) << (8-div);		//generate our top and bottom steps
	op[1] = (ip2[1] - ip1[1]) << (8-div);		//generate our top and bottom steps
}


void IncrByte(u_short *ref, u_short *step)
{
	ref[0] = ref[0] + step[0];
	ref[1] = ref[1] + step[1];
} 

void IncrShort(VECTOR *ref, VECTOR *step)
{
	ref->vx = ref->vx + step->vx;
	ref->vy = ref->vy + step->vy;
	ref->vz = ref->vz + step->vz;
}





char *DivPloyFT4(s, divp, sh, sv)
POLY_FT4 *s;
DIVPOLYGON *divp;
long sh, sv;
{
	long		h,v,i,j;
	long		p,p0;
	SVECTOR		vf0,vf1,vf2,vf3;
	VECTOR		xbtween, ybtween;
	VECTOR		xttween, yttween;
	VECTOR		xbtstep, ybtstep;
	VECTOR		xttstep, yttstep;
	u_long		uttween, vbtween;
	u_long		ubtween, vttween;
	u_long		uttstep, vttstep;
	u_long		ubtstep, vbtstep;
	static char	col;

	long flag;
	


/* 		Plan of action is to create this by using as little 
		maths as possible.

	   vf0	  vf1
	   vt0	  vt1
	   vi0____>_________>_________>_______vi1
		|                                  |
		|                                  |
		|                                  |
	vf2 |      X         X         X       |
		|     vf3                          |
		|                                  |
		|      |                           |
		|      |                           |
		V      V                           |
		|                                  |
		|      X         X         X       |
		|                                  |
		|      |                           |
		|      |                           |
		V      V                           |
		|                                  |
		|      X         X         X       |
		|                                  |
		|      |                           |
		V      |                           |
		|      V                           |
		------>--------->--------->---------
	   vi2                                vi3
	   vt2	   vt3
	    

two loops:
	Outer loop is to compute traversal in X.
	At end of loop 1->0 and 3->2

		Inner loop will be traveral in Y down the screen.
		at end of loop 2->0 and 3->1.  Therefore we need a 
		RotTransPers2 for the occasion, as we will have computed
		the first pairs SX and SY in the previous incarnation
		of the loop.

*/
	h = 1<<sh;	v = 1<<sv;

	divp->vt1 = divp->vi0;
	divp->vt3 = divp->vi2;
	*(u_short *)divp->uvt1 = *(u_short *)divp->uvi0;
	*(u_short *)divp->uvt3 = *(u_short *)divp->uvi2;

	MakeShortStep(&divp->vi0, &divp->vi1, &xbtstep, sh);
	MakeShortStep(&divp->vi2, &divp->vi3, &xttstep, sh);

	MakeByteStep(divp->uvi0, divp->uvi1, (u_short *)&ubtstep, sh);
	MakeByteStep(divp->uvi2, divp->uvi3, (u_short *)&uttstep, sh);
	
	ubtween  = ubtstep;		/*make 1 step the current tween value*/
	uttween  = uttstep;
	xbtween  = xbtstep;
	xttween  = xttstep;

	for (i=0;i<h;i++) {
		
		divp->vt0 = divp->vt1;		//the lsat vertex pair is the first of the next
		divp->vt2 = divp->vt3;
		*(u_short *)divp->uvt0 = *(u_short *)divp->uvt1;  
		*(u_short *)divp->uvt2 = *(u_short *)divp->uvt3;  
		
		TweenByte ( divp->uvi0, (u_short *)&ubtween, divp->uvt1 );	//add the step back onto the last to make the next
		TweenByte ( divp->uvi2, (u_short *)&uttween, divp->uvt3 );
		TweenShort( &divp->vi0, &xbtween, &divp->vt1 );
		TweenShort( &divp->vi2, &xttween, &divp->vt3 );
		
		vf0 = divp->vt0;		//f is the actual polygon that we are generating
		vf1 = divp->vt1;

		//first UVs now sorted
		*(u_short *)divp->uvf0 = *(u_short *)divp->uvt0;
		*(u_short *)divp->uvf1 = *(u_short *)divp->uvt1;


		/*The first pair of points is now establlished, so transform*/
		RRRotTransPers2( &vf0,&vf1, &divp->dv0, &divp->dv1); //Special Fn that is only applicable here

		MakeShortStep(&divp->vt0, &divp->vt2, &ybtstep, sv);	//Make up the steps for the inner loop
		MakeShortStep(&divp->vt1, &divp->vt3, &yttstep, sv);

		MakeByteStep(divp->uvt0, divp->uvt2, (u_short *)&vbtstep, sv);
		MakeByteStep(divp->uvt1, divp->uvt3, (u_short *)&vttstep, sv);

		vbtween = vbtstep;		//make the tweens the first step
		vttween = vttstep;
		ybtween = ybtstep;
		yttween = yttstep;
		
		for (j=0;j<v;j++) {
			
			*(u_short *)(&s->u0) = *(u_short *)divp->uvf0;  //copy last 2nds to be this first pair of UVs
			*(u_short *)(&s->u2) = *(u_short *)divp->uvf1;
			
			TweenByte ( divp->uvt0, (u_short *)&vbtween, divp->uvf0);
			TweenByte ( divp->uvt1, (u_short *)&vttween, divp->uvf1);
			
			*(u_short *)(&s->u1) = *(u_short *)divp->uvf0;  //write in new 2nds
			*(u_short *)(&s->u3) = *(u_short *)divp->uvf1;
			
			TweenShort( &divp->vt0, &ybtween, &vf0);		//make vf0 from 
			TweenShort( &divp->vt1, &yttween, &vf1);
			
			*(DVECTOR *)(&s->x0) = divp->dv0;				//write in the pair of points from the last loop
			*(DVECTOR *)(&s->x2) = divp->dv1;

			RRRotTransPers2(&vf0, &vf1,	&divp->dv2, &divp->dv3 );


			/*check against rejection*/			
			flag = 0;
			if((divp->dv0.vx>320)&&(divp->dv1.vx>320)&&(divp->dv2.vx>320)&&(divp->dv3.vx>320)) flag =1;
			if((divp->dv0.vx<0)&&(divp->dv1.vx<0)&&(divp->dv2.vx<0)&&(divp->dv3.vx<0)) flag =1;
			if((divp->dv0.vy>240)&&(divp->dv1.vy>240)&&(divp->dv2.vy>240)&&(divp->dv3.vy>240)) flag =1;
			if((divp->dv0.vy<0)&&(divp->dv1.vy<0)&&(divp->dv2.vy<0)&&(divp->dv3.vy<0)) flag =1;

			/*if successful build the polygon*/
			if(flag==0){
			   	*(DVECTOR *)(&s->x1) = divp->dv2;
				*(DVECTOR *)(&s->x3) = divp->dv3;
				s->clut = divp->clut;	s->tpage = divp->tpage;
				s->tag = 0x09000000;	*(CVECTOR *)&s->r0 = divp->color;
				*(long *)&s->r0 = 0x2d0000ff ; 
				AddPrim( divp->ot, s++ );
			}

			/*prepare for next incantation*/
			divp->dv0 = divp->dv2;
			divp->dv1 = divp->dv3;

			/*increment the divide by step*/
			IncrByte((u_short *)&vbtween, (u_short *)&vbtstep);
			IncrByte((u_short *)&vttween, (u_short *)&vttstep);
			IncrShort(&ybtween, &ybtstep);
			IncrShort(&yttween, &yttstep);
		}

		/*increment the X/U cutup coords*/
		IncrByte((u_short *)&ubtween, (u_short *)&ubtstep);
		IncrByte((u_short *)&uttween, (u_short *)&uttstep);
		IncrShort(&xbtween, &xbtstep);
		IncrShort(&xttween, &xttstep);
	}
//	SetFarColor( dq.c.r, dq.c.g, dq.c.b );		/* patch (^^; */
	return( (char *)s );
}



