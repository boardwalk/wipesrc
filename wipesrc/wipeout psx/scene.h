/* (C) Psygnosis 1994. By Jason Carl Denton & Rob Smith
*/



#define     ToIntel     0
#define     ToUnix      1




Object* LoadPrm( char* name, short textureStart );

Object* LoadScn( char* name, short textureStart );

void IntelObject( char *start, char *finish, short convert );

void IntelShort( short* svalue );

void IntelLong( long* lvalue );
