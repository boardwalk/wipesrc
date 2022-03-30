typedef struct
{
   int16_t          coneIndex[ 3 ][ 8 ];

   SVECTOR        oldVertex[ 3 ][ 8 ];

   Object         *object[ 8 ];
   ShipData       *ship[ 8 ];
} Cone;


void InitCone ( Cone *cone );
void UpdateCone ( Cone *cone );
