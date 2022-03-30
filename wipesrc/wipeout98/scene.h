/* (C) Psygnosis 1994. By Jason Carl Denton & Rob Smith
*/



#define     ToIntel     0
#define     ToUnix      1




Object* LoadPrm( char* name, int16_t textureStart );

Object* LoadScn( char* name, int16_t textureStart );

void IntelObject( char *start, char *finish, int16_t convert );

void IntelShort( int16_t* svalue );

void IntelLong( int32_t* lvalue );
