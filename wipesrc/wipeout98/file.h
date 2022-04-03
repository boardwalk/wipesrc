#ifndef FILE_H
#define FILE_H

int32_t FileLength( const char* name );

int32_t LoadFile( const char* name, char* buffer );

void SaveFile( const char* name, char* buffer, int32_t length );

void InitCDFS();

#endif
