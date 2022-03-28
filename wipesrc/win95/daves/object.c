#include <sys/types.h>

#ifdef __TYPES_H_INCLUDED
	#include "libetc.h"
	#include "libgte.h"
	#include "libgpu.h"
	#define NULL 0L
#endif

#ifdef _SYS_TYPES_H
	#include <libetc.h>
	#include <libgte.h>
	#include <libgpu.h>
	#define NULL 0L
#endif

#include "mytypes.h"


extern SVECTOR		  * vert3DCopy[maxObject];
extern u_char			 slowAnim[maxObject];


void LoadObject( u_long oi, objectHeader ** obj, unsigned char * template, SVECTOR * obOff )
{
	unsigned short offset;
	unsigned short verts, maxVerts, totalVerts;
	unsigned char  code;
	unsigned char  len;
	u_long 			*pOff;
	u_long 			*tOff;
	u_long			*primBase;
	u_long			linkIndex;
	u_long			p,l,i,count,f;
	long				j;
	POLYREF			*pRef,**linkPtr;
	SVECTOR			*vec3D;
	objectHeader   *ob = *obj;
	clutTexPair		*firstTexEntry;
	u_long			centroidOffset = 1;
	u_char			loadMode;

	u_short	clut, tex;
	u_short * texWidth, * texHeight;


/*asm("addu	$3,$0,$0");*/

	*obj = (objectHeader *)(template+8);															/* skip past IFF header */
	ob = *obj;
	ob->vertexTable = (firstLast *)((u_long)ob + (u_long)ob->vertexTable);		/* skip past IFF header and compute absolute address */
	ob->mimeTable = (firstLast *)((u_long)ob + (u_long)ob->mimeTable);	/* skip past IFF header and compute absolute address */
	ob->primTable = (firstLast *)((u_long)ob + (u_long)ob->primTable);			/* skip past IFF header and compute absolute address */
	ob->textureTable = (clutTex *)((u_long)ob + (u_long)ob->textureTable);	/* skip past IFF header and compute absolute address */
	ob->screenArray = NULL;
	ob->linkTable   = NULL;
	maxVerts = 0;
	totalVerts = 0;

	ob->linkTable = (firstLast *)malloc( sizeof(firstLast) * ob->numFrames );
	if( ob->linkTable == NULL )
	{
		printf("Warning... linkTable failed to allocate\n");
		PadPause();
	}

	for(i=0; i<ob->numFrames; i++)
	{
		if( verts > maxVerts )
			maxVerts = verts;
		totalVerts += verts;

		ob->vertexTable[i].first = (void *)((u_long)(ob->vertexTable[i].first) + (u_long)ob + 0 );
		ob->vertexTable[i].last  = (void *)((u_long)(ob->vertexTable[i].last)  + (u_long)ob + 0 );

		ob->primTable[i].first   = (void *)((u_long)(ob->primTable[i].first)   + (u_long)ob + 4 );
		ob->primTable[i].last    = (void *)((u_long)(ob->primTable[i].last )   + (u_long)ob - 4 );

		ob->mimeTable[i].first   = (void *)((u_long)(ob->mimeTable[i].first)   + (u_long)ob + 0 );
		ob->mimeTable[i].last    = (void *)((u_long)(ob->mimeTable[i].last )   + (u_long)ob - 0 );

		verts = ((u_long)ob->vertexTable[i].last - (u_long)ob->vertexTable[i].first)/ sizeof(SVECTOR);

		ob->linkTable[i].first = (void *) malloc( sizeof(POLYREF *) * verts );
		if( ob->linkTable[i].first == NULL )
		{
			printf("Warning... linkTable Entry failted to allocate\n");
			PadPause();
		}
		ob->linkTable[i].last  = (void*)((char *)(ob->linkTable[i].first) + (verts - 1)*4);

	}


	ob->screenArray = (u_long *)malloc( sizeof(u_long) * maxVerts );
	if( ob->screenArray == NULL )
	{
		printf("Warning... screen array failed to alloc\n");
		PadPause();
	}

	vert3DCopy[oi] = (SVECTOR *)malloc( sizeof(SVECTOR) * maxVerts );
	if( vert3DCopy[oi] == NULL )
	{
		printf("Warning... screen array failed to alloc\n");
		PadPause();
	}


	firstTexEntry = &(ob->textureTable->firstPair);
/*	for( i=0; ob->textureTable->numEntries; i++ )
	{*/
		/*clut = LoadClut( (u_long*)(clutOff+4+(u_long)template), 480, 0 );
		tex = LoadTPage( (u_long*)(texOff+4+(u_long)template), 0, 0, 640, 0, *texWidth, *texHeight );
		printf("Texture Size [%d, %d]\n",(u_long)*texWidth, (u_long)*texHeight); */
/*	}*/

	for( f=0; f<ob->numFrames; f++ )
	{
		for( linkPtr =  (POLYREF **)(ob->linkTable[f].first);
			  linkPtr <= (POLYREF **)(ob->linkTable[f].last );
			  linkPtr ++
			)
			*linkPtr = NULL;

		for( vec3D = (SVECTOR *)(ob->vertexTable[f].first);
			  vec3D < (SVECTOR *)(ob->vertexTable[f].last );
			  vec3D ++
			)
		{
			vec3D->vx += obOff->vx;
			vec3D->vy += obOff->vy;
			vec3D->vz += obOff->vz;
		}
	}



	/* now we have the pointers, lets go into 
		the template and alter the offsets and stuff
    */

	for( f=0; f<ob->numFrames; f++ )
	{
		count = 0;
		linkPtr = (POLYREF**)(ob->linkTable[f].first);
		pOff = (u_long*)(ob->primTable[f].first);

		if( (f == 0) || (ob->primTable[f].first != ob->primTable[f-1].first) )
			loadMode = 0;  /* relocatable primitives */
		else
			loadMode = 1; /* prims have been converted to absolute already */
		

		while( pOff!=(u_long *)0x80000000L )
		{
			tOff = pOff;


			if( loadMode == 0 )
			{
				p = ((*pOff) & 0x00ffffff) + (u_long)pOff;
				l = (*pOff) & 0xff000000;

				(*pOff) = (p & 0x00ffffff)+l;
			}
			else
			{
#ifdef __TYPES_H_INCLUDED
				p = ((*pOff) & 0x00ffffffL);
#endif

#ifdef _SYS_TYPES_H
				p = ((*pOff) & 0x00ffffffL) | 0x80000000L;
#endif
			}


			/* look at primitive indexes and fill in link array */
			len 		= *(((unsigned char *)pOff) + 3);
			code 		= *(((unsigned char *)pOff) +	7);
			primBase = (u_long*)((u_long)pOff + 8);

			switch( code )
			{
				case 0x29:		/* F4 */
				case 0x28:
					for(i=0; i<4; i++ )
					{
						linkIndex = *primBase + centroidOffset;
						pRef = (POLYREF *)malloc( sizeof(POLYREF) );
						pRef->next = linkPtr[linkIndex];
						linkPtr[linkIndex] = pRef;
						pRef->addr = primBase;

						primBase++;
					}
					break;

				case 0x21:	/* F3 */
				case 0x20:
					for(i=0; i<3; i++ )
					{
						linkIndex = *primBase + centroidOffset;
						pRef = (POLYREF *)malloc( sizeof(POLYREF) );
						pRef->next = linkPtr[linkIndex];
						linkPtr[linkIndex] = pRef;
						pRef->addr = primBase;

						primBase++;
					}
					break;

				case 0x2D:	/* FT4 */
				case 0x2C:
					for(i=0; i<4; i++ )
					{
						linkIndex = *primBase + centroidOffset;
						pRef = (POLYREF *)malloc( sizeof(POLYREF) );
						pRef->next = linkPtr[linkIndex];
						linkPtr[linkIndex] = pRef;
						pRef->addr = primBase;
						primBase++;

						/* tpage/clut */
						if( i==0 ) /* clut */
							*primBase |= clut<<16;
						else if( i==1 ) /* tpage */
							*primBase |= tex<<16;

						primBase++;
					}
					break;

				case 0x25:	/* FT3 */
				case 0x24:
					for(i=0; i<3; i++ )
					{
						linkIndex = *primBase + centroidOffset;
						pRef = (POLYREF *)malloc( sizeof(POLYREF) );
						pRef->next = linkPtr[linkIndex];
						linkPtr[linkIndex] = pRef;
						pRef->addr = primBase;
						primBase++;

						/* tpage/clut */
						if( i==0 ) /* clut */
							*primBase |= clut<<16;
						else if( i==1 ) /* tpage */
							*primBase |= tex<<16;

						primBase++;
					}
					break;

				case 0x35:	/* GT3 */
				case 0x34:
					for(i=0; i<3; i++ )
					{
						linkIndex = *primBase + centroidOffset;
						pRef = (POLYREF *)malloc( sizeof(POLYREF) );
						pRef->next = linkPtr[linkIndex];
						linkPtr[linkIndex] = pRef;
						pRef->addr = primBase;

						primBase++;

						/* tpage/clut */
						if( i==0 ) /* clut */
							*primBase |= clut<<16;
						else if( i==1 ) /* tpage */
							*primBase |= tex<<16;

						primBase++;

						/* RGB */

						primBase++;
					}
					break;

				case 0x3c:	/* GT4 */
				case 0x3d:
					for(i=0; i<4; i++ )
					{
						linkIndex = *primBase + centroidOffset;
						pRef = (POLYREF *)malloc( sizeof(POLYREF) );
						pRef->next = linkPtr[linkIndex];
						linkPtr[linkIndex] = pRef;
						pRef->addr = primBase;

						primBase++;

						/* tpage/clut */
						if( i==0 ) /* clut */
							*primBase |= clut<<16;
						else if( i==1 ) /* tpage */
							*primBase |= tex<<16;

						primBase++;

						/* RGB */

						primBase++;
					}
					break;

				case 0x39:	/* G4 */
				case 0x38:
					for(i=0; i<4; i++ )
					{
						linkIndex = *primBase + centroidOffset;
						pRef = (POLYREF *)malloc( sizeof(POLYREF) );
						pRef->next = linkPtr[linkIndex];
						linkPtr[linkIndex] = pRef;
						pRef->addr = primBase;
						primBase++;
						/* rgb */
						primBase++;
					}
					break;

				case 0x31:	/* G3 */
				case 0x30:
					for(i=0; i<3; i++ )
					{
						linkIndex = *primBase + centroidOffset;
						pRef = (POLYREF *)malloc( sizeof(POLYREF) );
						pRef->next = linkPtr[linkIndex];
						linkPtr[linkIndex] = pRef;
						pRef->addr = primBase;
						primBase++;
						/* rgb */
						primBase++;
					}
					break;

				case 0x40:	/* FL2 */
					for(i=0; i<2; i++ )
					{
						linkIndex = *primBase + centroidOffset;
						pRef = (POLYREF *)malloc( sizeof(POLYREF) );
						pRef->next = linkPtr[linkIndex];
						linkPtr[linkIndex] = pRef;
						pRef->addr = primBase;
						primBase++;
					}
					break;

				case 0x48:	/* FL3 */
					for(i=0; i<3; i++ )
					{
						linkIndex = *primBase + centroidOffset;
						pRef = (POLYREF *)malloc( sizeof(POLYREF) );
						pRef->next = linkPtr[linkIndex];
						linkPtr[linkIndex] = pRef;
						pRef->addr = primBase;
						primBase++;
					}
					break;

				case 0x4C:	/* FL4 */
					for(i=0; i<4; i++ )
					{
						linkIndex = *primBase + centroidOffset;
						pRef = (POLYREF *)malloc( sizeof(POLYREF) );
						pRef->next = linkPtr[linkIndex];
						linkPtr[linkIndex] = pRef;
						pRef->addr = primBase;
						primBase++;
					}
					break;

				case 0x50:	/* GL2 */
					for(i=0; i<2; i++ )
					{
						linkIndex = *primBase + centroidOffset;
						pRef = (POLYREF *)malloc( sizeof(POLYREF) );
						pRef->next = linkPtr[linkIndex];
						linkPtr[linkIndex] = pRef;
						pRef->addr = primBase;
						primBase++;
						primBase++;
					}
					break;

				case 0x58:	/* GL3 */
					for(i=0; i<3; i++ )
					{
						linkIndex = *primBase + centroidOffset;
						pRef = (POLYREF *)malloc( sizeof(POLYREF) );
						pRef->next = linkPtr[linkIndex];
						linkPtr[linkIndex] = pRef;
						pRef->addr = primBase;
						primBase++;
						primBase++;
					}
					break;

				case 0x5C:	/* GL4 */
					for(i=0; i<4; i++ )
					{
						linkIndex = *primBase + centroidOffset;
						pRef = (POLYREF *)malloc( sizeof(POLYREF) );
						pRef->next = linkPtr[linkIndex];
						linkPtr[linkIndex] = pRef;
						pRef->addr = primBase;
						primBase++;
						primBase++;
					}
					break;

				default:
					printf("Warning Unknown Code...%02x in Primitive %d  [Len=%d]\n",code,count,len);
					for( j=-4; j<5; j++ )
					{
						if( j==0 ) printf(">"); else printf(" ");
						printf("%08x\n", *(pOff+j) );
					}
					PadPause();
					break;
			}


			
			if( p != (u_long)pOff )
				pOff = (u_long *)p;
			else
				pOff = (u_long*)0x80000000L;



			count++;
		}
		printf("Has %d Primitives\n",count);
		ob->primTable[f].last = (void *)tOff;

	}
}





void GetObject( u_long obNum, char * filename, objectHeader ** object, unsigned char ** objectTemplate,
					 short x, short y, short z, u_char animSlow )
{
	int				file;
	long				fileSize;
	SVECTOR			obOff;

	PCinit();
	file = PCopen( filename, 0, 0 );
	if( file == -1 )
	{
		printf("Warning... file '%s' did not open\n",filename);
		PadPause();
	}
	fileSize = PClseek( file, 0, 2 );
	PClseek( file, 0, 0 );
	*objectTemplate = (unsigned char *)malloc( fileSize );
	PCread( file, *objectTemplate, fileSize );
	PCclose( file );
	obOff.vx = x; obOff.vy = y; obOff.vz = z;
	LoadObject( obNum, object, *objectTemplate, &obOff );
	slowAnim[obNum] = animSlow;
}



void AddObjectCC( u_long * ot, u_long * firstPrim, u_long * lastPrim )
{
	u_long tmp;
	u_long len;

	/* get address of NEXT ot entry */
	tmp = *ot;

	/* set address of this entry to first primitive */
	*ot = ((u_long)(firstPrim)) & 0x00ffffff;

	/* preserve the length of the last primitive */
	len = (*lastPrim) & 0xff000000;

	/* set final link of last primitive to next table entry */
	(*lastPrim) = tmp+len;
}


void UpdateObjectVertexCC( POLYREF ** firstLink, POLYREF ** lastLink, u_long * firstVertex )
{
	POLYREF ** linkHnd;
	POLYREF *  linkPtr;
	u_long  *  vertPtr;

	for( 	linkHnd=firstLink, vertPtr=firstVertex; 
			linkHnd<=lastLink; 
			linkHnd++,         vertPtr++
		)
	{
		linkPtr = *linkHnd;
		while( linkPtr != NULL )
		{
			*(linkPtr->addr) = *vertPtr;

			linkPtr = linkPtr->next;
		}
	}
}



void UpdateObjectCodesCC( u_long * first, u_long * last )
{
	POLYREF 			*linkPtr;
	POLYREF			**linkHdl;
	SVECTOR			*scrCoorPtr;
	u_long  			*terminate;
	long 				outerProd;
	unsigned char  GPUcode;
	unsigned char  GPUlen;
	u_long 			*primPtr;

	primPtr = first;

#ifdef __TYPES_H_INCLUDED
	terminate = (u_long *)(((*last)&0x00ffffffL));
#endif

#ifdef _SYS_TYPES_H
	terminate = (u_long *)(((*last)&0x00ffffffL) | 0x80000000L);
#endif

	while( primPtr != terminate )
	{
		/* copy original GPU code back into primatives */
		*(((unsigned char *)primPtr)+7) = *(((unsigned char *)primPtr)-4);
		*(((unsigned char *)primPtr)+3) = *(((unsigned char *)primPtr)-3);

		GPUcode = *(((unsigned char *)primPtr) + 7);
		GPUlen = *(((unsigned char *)primPtr) + 3);


		/* check screen coordinates... If backface then 
			fill primative LENGTH field with 0 and move on else
			fill LENGTH with value copied from 4 bytes previous 
		*/
 
		switch( GPUcode )
		{
			case 0x29:	/* flat quads and triangles */
			case 0x28:
			case 0x21:
			case 0x20:
				outerProd = NormalClip( *(primPtr+2), *(primPtr+3), *(primPtr+4) );
				if( outerProd >= 0 )
					*(((unsigned char *)primPtr)+7) = 0x00;
				break;

			case 0x39:	/* gouraud Shaded */
			case 0x38:
			case 0x31:
			case 0x30:
				outerProd = NormalClip( *(primPtr+2), *(primPtr+4), *(primPtr+6) );
				if( outerProd >= 0 )
					*(((unsigned char *)primPtr)+3) = 0x00;
				break;

			case 0x2D:	/* textured */
			case 0x2C:
			case 0x25:
			case 0x24:
				outerProd = NormalClip( *(primPtr+2), *(primPtr+4), *(primPtr+6) );
				if( outerProd >= 0 )
					*(((unsigned char *)primPtr)+3) = 0x00;
				break;

			case 0x3D:	/* gouraud textured */
			case 0x3C:
			case 0x35:
			case 0x34:
				outerProd = NormalClip( *(primPtr+2), *(primPtr+5), *(primPtr+8) );
				if( outerProd >= 0 )
					*(((unsigned char *)primPtr)+3) = 0x00;
				break;

			case 0x40:	/* normal and gouraud lines */
			case 0x48:
			case 0x4C:
			case 0x50:
			case 0x58:
			case 0x5C:
			case 0x00:
				outerProd = 0;
				break;

			default:
				printf("Warning Unknown Code...%02x\n",GPUcode);
				PadPause();
				break;
		}

#ifdef __TYPES_H_INCLUDED
		primPtr = (u_long *)(((*primPtr) & 0x00ffffffL));
#endif

#ifdef _SYS_TYPES_H
		primPtr = (u_long *)(((*primPtr) & 0x00ffffffL) | 0x80000000L);
#endif
	}

}



void CopyDataAndMIME( SVECTOR * vertPtr, SVECTOR * lastVert, SVECTOR * difPtr, long coef, SVECTOR * firstMIMEVert )
{
	SVECTOR * t1, * t2;
	long	numData = ((u_long)lastVert - (u_long)vertPtr) / sizeof(SVECTOR);

	for( 	t1 =  vertPtr, t2 = firstMIMEVert;
			t1 < lastVert;
			t1++, t2++
		)
		*t2 = *t1;

	gteMIMefunc(firstMIMEVert, difPtr, numData, coef);
}




