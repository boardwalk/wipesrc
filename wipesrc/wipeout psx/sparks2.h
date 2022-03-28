#define Max3DLines      200
#define Max3DLineSkels  10


typedef struct Line3D
{
   short       used;
   short       skel;

   VECTOR      start;
   VECTOR      pos;
   VECTOR      vel;

   long        accl;
   long        friction;
   long        gravity;
   long        wallRes;

   long        life;

	LINE_G2     line[2];
} Line3D;



typedef struct LineSkel
{
   short       count;
   short       used;

   Skeleton    *skeleton;
} LineSkel;



void CreateSparks3D( Skeleton *camPos );
void StartSparks3D( VECTOR *start, short numSparks, VECTOR *velocity );
void UpdateSparks3D( void );
