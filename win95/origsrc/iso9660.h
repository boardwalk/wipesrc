#ifndef _ISO9660_H_
#define _ISO9660_H_

#define DESC_TYPE(base)					(*base)
#define DESC_PATH_SIZE(base)			(*((u_long *)(base+132)))
#define DESC_PATH_LOC(base)			(*((u_long *)(base+140)))
#define DESC_OPT_PATH_LOC(base)		(*((u_long *)(base+144)))
#define DESC_LEN_EXT_DIR(base)		(*(base+157))

#define DESC_DIR_EXT_LOC(base)		(  (*((u_short *)(base+158))) | (*((u_short *)(base+160)) << 16) )
#define DESC_DIR_DATA_LEN(base)	(  (*((u_short *)(base+166))) | (*((u_short *)(base+168)) << 16) )

#define DIR_LEN(base)					(*base)
#define DIR_EXT_LEN(base)				(*(base+1))
#define DIR_FILE_LOC(base)				( (*((u_short *)(base+2))) | (*((u_short *)(base+4)) << 16) )
#define DIR_DATA_LEN(base)				( (*((u_short *)(base+10))) | (*((u_short *)(base+12)) << 16) )
#define DIR_FILEID_LEN(base)			(*(base+32))
#define DIR_FILEID(base)				((char*)base+33)
#define DIR_FLAG(base)					(*(base+25))

#define PATH_LEN_DIRID(base) 			(*base)
#define PATH_EXT_DIR_LEN(base)		(*(base+1))
#define PATH_FILE_LOC(base)			( (*((u_short *)(base+2))) | (*((u_short *)(base+4)) << 16) )
#define PATH_PARENT_DIR(base)			(*((u_short *)(base+6)))
#define PATH_DIRID(base)				((char*)base+8)

#define MAX_DIR_LEN						64				/* This is an empirical value! Sorry. */
#define MIN_DIR_LEN						34				/* This is the absolute min with NO SU at all */
#define NUM_OF_FD							16
#define FLAG_IS_NOT_LAST_REC			0x80
#define FLAG_IS_DIR						0x02
#define DIR_CACHE_SIZE					8192		/* 4 sectors */

typedef struct _fi
{
	long loc;
	long size;
} FileInfo;

typedef struct _fd
{
	long 		fd;
	FileInfo 	fi;
	u_long	fp;		/* Current CD pos in file */
} FileDes;

long iso_init(DynamicHeap *heap);
long iso_filesize(long fd);
long iso_fileloc(long fd);

/*
** opens the file denoted by the absolute pathname with \ seperators 
** \\ in C source. There is no ;1 and no cdrom: it is also case insensitive
** The flags currently does NOTHING.
*/
long iso_open(char *pathname, long flags);

long iso_get_file_info(char *pathname, FileInfo *fi);

void iso_close(long fd);

/*
** Read from the file 'fd' from the current file position, nsec sectors
** of data into buffer. If successful, return the number of sectors read.
** If it fails, return 0;
*/
long iso_read(u_char *buffer, long nsec, long fd);
CdlLOC *iso_toc();

#endif
