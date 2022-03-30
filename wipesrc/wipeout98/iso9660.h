#ifndef _ISO9660_H_
#define _ISO9660_H_

#define DESC_TYPE(base)					(*base)
#define DESC_PATH_SIZE(base)			(*((uint32_t *)(base+132)))
#define DESC_PATH_LOC(base)			(*((uint32_t *)(base+140)))
#define DESC_OPT_PATH_LOC(base)		(*((uint32_t *)(base+144)))
#define DESC_LEN_EXT_DIR(base)		(*(base+157))

#define DESC_DIR_EXT_LOC(base)		(  (*((uint16_t *)(base+158))) | (*((uint16_t *)(base+160)) << 16) )
#define DESC_DIR_DATA_LEN(base)	(  (*((uint16_t *)(base+166))) | (*((uint16_t *)(base+168)) << 16) )

#define DIR_LEN(base)					(*base)
#define DIR_EXT_LEN(base)				(*(base+1))
#define DIR_FILE_LOC(base)				( (*((uint16_t *)(base+2))) | (*((uint16_t *)(base+4)) << 16) )
#define DIR_DATA_LEN(base)				( (*((uint16_t *)(base+10))) | (*((uint16_t *)(base+12)) << 16) )
#define DIR_FILEID_LEN(base)			(*(base+32))
#define DIR_FILEID(base)				((char*)base+33)
#define DIR_FLAG(base)					(*(base+25))

#define PATH_LEN_DIRID(base) 			(*base)
#define PATH_EXT_DIR_LEN(base)		(*(base+1))
#define PATH_FILE_LOC(base)			( (*((uint16_t *)(base+2))) | (*((uint16_t *)(base+4)) << 16) )
#define PATH_PARENT_DIR(base)			(*((uint16_t *)(base+6)))
#define PATH_DIRID(base)				((char*)base+8)

#define MAX_DIR_LEN						64				/* This is an empirical value! Sorry. */
#define MIN_DIR_LEN						34				/* This is the absolute min with NO SU at all */
#define NUM_OF_FD							16
#define FLAG_IS_NOT_LAST_REC			0x80
#define FLAG_IS_DIR						0x02
#define DIR_CACHE_SIZE					8192		/* 4 sectors */

typedef struct _fi
{
	int32_t loc;
	int32_t size;
} FileInfo;

typedef struct _fd
{
	int32_t 		fd;
	FileInfo 	fi;
	uint32_t	fp;		/* Current CD pos in file */
} FileDes;

int32_t iso_init(DynamicHeap *heap);
int32_t iso_filesize(int32_t fd);
int32_t iso_fileloc(int32_t fd);

/*
** opens the file denoted by the absolute pathname with \ seperators
** \\ in C source. There is no ;1 and no cdrom: it is also case insensitive
** The flags currently does NOTHING.
*/
int32_t iso_open(char *pathname, int32_t flags);

int32_t iso_get_file_info(char *pathname, FileInfo *fi);

void iso_close(int32_t fd);

/*
** Read from the file 'fd' from the current file position, nsec sectors
** of data into buffer. If successful, return the number of sectors read.
** If it fails, return 0;
*/
int32_t iso_read(uint8_t *buffer, int32_t nsec, int32_t fd);
CdlLOC *iso_toc();

#endif
