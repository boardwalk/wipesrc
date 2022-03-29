/*
 * File:file.h
*/

#ifndef _FILE_H
#define _FILE_H

#include "wipesrc/psx26/include/sys/fcntl.h"

/* Flag for open() */
#define O_RDONLY        FREAD
#define O_WRONLY        FWRITE
#define O_RDWR          FREAD|FWRITE
#define O_CREAT         FCREAT  /* open with file create */
#define O_NOBUF         FNBUF	/* no device buffer and console interrupt */
#define O_NOWAIT        FASYNC	/* asyncronous i/o */

#define SEEK_SET	0
#define SEEK_END	1
#define SEEK_CUR	2

#endif
