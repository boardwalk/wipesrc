#define NO_OF_EFFECTS 140
#define NO_OF_EFFECT_TEXTURES 7

#define FIRE 0
#define SMOKE 1
#define FIRE_WHITE 2
#define SHADOW 3
#define EBOLT 4
#define HALO 5
#define GREENY 6

typedef struct Effect
{
	VECTOR 				pos;
	SVECTOR 				vel;
	POLY_FT4				sprite[2];
	short					count;
	short					type;
	char					u, v;
	short 				size;
} Effect;  

typedef struct Shadow
{
	POLY_FT3				poly[4][2];
} Shadow;  


void InitEffects			(Effect* effects, Skeleton *camPos, Texture** effectTextures);
void UpdateEffects		(Effect* effects);
void AddEffects			(Effect* effects, WeaponData *weaponIndex, Texture** effectTextures, ShipData *shipIndex);
void ColourStartBoom		(int noToLight);
void ResetStartBoom		();
void InitStartBoom		(Object** prm, short prmCount);
Shadow *InitShadow		(Skeleton *camPos, Texture** effectTextures);
void DrawShadow			(Object **shipShapes, ShipData *shipIndex, Shadow *shadow);
void InitWeaponIcons		(POLY_FT4 *weaponIcon, Texture** weaponIconTextures);
void DrawWeaponIcons		(POLY_FT4 *weaponIcon, Texture** weaponIconTextures, ShipData *playerShip);
void InitMapShips			(POLY_F3 *mapShips, LINE_F2 *trackMap );
void DrawMapShips			(POLY_F3 *mapShips, ShipData *shipIndex, LINE_F2 *trackMap );
void SetShake				(int duration);
void ShakeScreen			();
void InitAuroraBorialis	(Object* object);
void AuroraBorialis		(Object* object);
