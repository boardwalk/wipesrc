typedef struct Line3D
{
   short    used;

   long     xpos;
   long     ypos;
   long     zpos;

   long     xvel;
   long     yvel;
   long     zvel;

   long     accl;
   long     friction;
   long     gravity;
   long     wallRes;

   long     life;
} Line3D;



Object* CreateSparks3D( short numSparks );
void StartSparks3D( Object* object, Line3D *data, VECTOR *start );
void UpdateSparks3D( Object *sparks, Line3D *data );
