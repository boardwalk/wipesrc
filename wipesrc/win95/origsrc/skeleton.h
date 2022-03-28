/* (C) Psygnosis 1994. By Jason Carl Denton
*/

Skeleton* NewSkeleton( );

void SetSkeletonPosition( Skeleton* skeleton, long x, long y, long z  );

void SetSkeletonDirection( Skeleton* skeleton, short xa, short ya, short za  );

MATRIX* GetSkeletonMatrix( Skeleton* skeleton );

void SetSkeletonTransform( Skeleton* skeleton );
