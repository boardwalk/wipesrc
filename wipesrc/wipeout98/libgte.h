#ifndef LIBGTE_H
#define LIBGTE_H

void RotTransPersN(SVECTOR *v0,DVECTOR *v1,uint16_t *sz,uint16_t *flag,int32_t n);
int32_t NormalClip( DVECTOR sxy0, DVECTOR sxy1, DVECTOR sxy2 );
int32_t rcos(int32_t a);
int32_t rsin(int32_t a);
int32_t ratan2(int32_t y, int32_t x);
int32_t SquareRoot0( int32_t a );
void SetDrawDistance(int32_t Dist);
void RotTrans( SVECTOR * v0, VECTOR * v1, int32_t * flag );
int32_t RotTransPers(SVECTOR *v0, DVECTOR *sxy, int32_t *p, int32_t *flag);
void CreateSineTable();
void SetGeomOffset( int32_t ofx, int32_t ofy);
void SetGeomScreen( int32_t h );
void SetTransMatrix( MATRIX * m );
void SetRotMatrix( MATRIX * m );
MATRIX * MulMatrix0( MATRIX * m0, MATRIX * m1, MATRIX * m2 );
void SetBackColor( int32_t rbk, int32_t gbk, int32_t bbk );
void SetColorMatrix( MATRIX * m );
void SetLightMatrix( MATRIX * m );
MATRIX * TransMatrix( MATRIX * m, VECTOR * v );
void NormalColorCol( SVECTOR * v0, CVECTOR * v1, CVECTOR * v2 );

#endif
