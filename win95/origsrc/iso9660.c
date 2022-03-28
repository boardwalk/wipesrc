#include "standard.h"
#include <sys/types.h>
#include <kernel.h>
#include "malloc.h"
#include "iso9660.h"
#include <libsn.h>

#define	WaitEvent(a)	// PCwipeout

u_long 			*PathTable;							/* The path table in memory (path recs are var. length) */
long 				path_tab_size;
long 				cached_dir_loc = 0;				/* The actual cached location */
long 				cached_dir_start_loc = 0;		/* The location of the start of cached dir*/
long 				cached_dir_file_len = 0;		/* The length of the file for the directory that is cached */
u_long 			dir_cache[DIR_CACHE_SIZE/4];	/* cache size should be x 2048 bytes */
long 				cache_size = 0;
long 				num_of_open_files = 0;
FileDes 			fd_tab[NUM_OF_FD];
u_long 			ev_comp, ev_error;
CdlLOC 			toc_loc[100];
long 				track1_start_sec = 0;
DynamicHeap*   iso_heap;

#ifndef USE_ATI


CdlLOC *iso_toc()
{
	return toc_loc;
}

#endif //USE_ATI
#ifndef USE_ATI



long iso_filesize(long fd)
{
	if (fd_tab[fd-1].fd != fd)
	{
		printf("iso9660.c:iso_filesize:this fd is not valid\n");
		return (-1);
	}

	return fd_tab[fd-1].fi.size;	
}

#endif //USE_ATI

#ifndef USE_ATI



long iso_fileloc(long fd)
{
	if (fd_tab[fd-1].fd != fd)
	{
		printf("iso9660.c:iso_fileloc:this fd is not valid\n");
		return (-1);
	}

	return fd_tab[fd-1].fi.loc;	
}
#endif //USE_ATI
#ifndef USE_ATI


int my_error_handler(void)
{
	exit(-1);
	return(-1);	// PCwipeout
}
#endif //USE_ATI


/*
** This will take an ISO9660 sector number and convert it into an absolute CD pos in loc. It automatically
** adds on the start sector position of track1 (usually 2 seconds)
*/
void	LogicalSectorToLoc(u_long sector, CdlLOC *loc)
{
	sector += track1_start_sec;
	loc->track = 1;
	loc->minute = itob(sector / 4500);
	loc->second = itob((sector % 4500)/75);
	loc->sector = itob(sector % 75);
}



#ifndef USE_ATI


/*
** iso_init: This ASSUMES that you have done a CdInit. It loads in the path table, setting
** the size - using dynamic allocation of memory - this is only done once!
** It should also clear out/ init. the file descriptor table
*/
long iso_init(DynamicHeap *heap)
{
#ifdef	PCwipeout	// PCwipeout
	long i;
	u_char prim_vol[2048];
	long path_tab_alloc;
	long res,cnt;
	CdlLOC loc;

	EnterCriticalSection();

	ev_comp = OpenEvent(HwCdRom, EvSpCOMP, EvMdNOINTR, NULL);
	ev_error = OpenEvent(HwCdRom, EvSpERROR, EvMdINTR, my_error_handler);
	EnableEvent(ev_comp);
	EnableEvent(ev_error);

	ExitCriticalSection();
	/*
	** Read the table to contents and set up the start sector number for track 1
	*/
	res = CdGetToc2(1, toc_loc); 
	
	track1_start_sec =  btoi(toc_loc[1].second)*75 + btoi(toc_loc[1].sector);
	/* Now, we can call LogicalSectorToLoc */

	LogicalSectorToLoc(16, &loc);
	CdControlF(CdlSeekL, (u_char *)&loc);			/* SEEK to PRIM_VOL */
	printf("before wait event\n");
	WaitEvent(ev_comp);
	printf("after wait event\n");
	CdRead(1, (u_long*)prim_vol, CdlModeSpeed);
	CdReadSync(0,0);
	
	if (DESC_TYPE(prim_vol) != 1)	/* Test for PRIM VOL */
	{
		printf("iso9660.c:iso_init: Wrong prim volume descriptor\n");
		return (0);
	}
	path_tab_size = DESC_PATH_SIZE(prim_vol);
	iso_heap = heap;
	path_tab_alloc = ((path_tab_size + 2047) >> 11) << 11;
	PathTable = (u_long *)DAlloc(iso_heap, path_tab_alloc );
	if (PathTable==NULL)
	{
		printf("iso9660.c:iso_init: failed to allocate memory for path table\n");
		return (0);
	}

	LogicalSectorToLoc((DESC_PATH_LOC(prim_vol)), &loc);
	CdControlF(CdlSeekL, (u_char *)&loc);

	printf("before wait event\n");
	WaitEvent(ev_comp);
	printf("after wait event\n");

	CdRead(path_tab_alloc/2048, PathTable, CdlModeSpeed);
	CdReadSync(0,0);
	
	num_of_open_files = 0;
	printf("iso9660.c:NUM_OF_FD : %d\n", NUM_OF_FD);
	for (i = 0; i < NUM_OF_FD; i++)
	{
		fd_tab[i].fd = 0;
	}
#endif
	return(0);
}

#endif //USE_ATI

#ifndef USE_ATI


long close_iso_events()
{
#ifdef	PCwipeout	// PCwipeout
	EnterCriticalSection();

	CloseEvent(ev_comp);
	CloseEvent(ev_error);

	ExitCriticalSection();
#endif					// PCwipeout
	return(1);
}

#endif //USE_ATI



/*
** Get the next path_rec by looking at the current structure.
** NB: PATH_RECs CAN span sectors, no padding required!
*/
u_char  * NextPath(u_char  *path_rec)
{
	u_char len;

	len = 9 + PATH_LEN_DIRID(path_rec);
	len = len & 0xfe;
	path_rec += len;
	if ((u_long)path_rec- (u_long)PathTable >= path_tab_size)
	{
		return NULL;		/* At end of path table */
	}
	else
	{
		return path_rec;
	}
}

long match_dir(char *dirname, char *testname, u_char test_len)
{
	long i,len;
	char *dn, *tn;

	dn = dirname;
	tn = testname;
	len = strlen(dirname);
	if (len != test_len)
	{
		return (0);
	}
	for (i = 0; i < len; i++)
	{
		if (*dn++ != *tn++)
		{
			return (0);
		}
	}
	return (1);
}

/*
** match_file: will compare the two strings and report a match (return 1) or no match (0).
** NB: the filename has already been converted to upper case and has NO version number.
** The testname has a version number on the end ';1'
*/
long match_file(char *filename, char *testname, u_char test_len)
{
	long i;
	char *fn, *tn;

	fn = filename;
	tn = testname;
	/* Some interpretations of iso9660 don't have the ;1 in the ID length */
	if (testname[test_len-2] == ';')
	{
		test_len -= 2;
	}
	if (strlen(filename) != test_len)
	{
		return (0);  	/* if they are not same len - cannot match */
	}
	for (i = 0; i < test_len; i++)
	{
		if (*fn++ != *tn++)
		{
			return (0);
		}
	}
	return (1);	
}


/*
** search : looks through the dir_cache up to cache_size attempting to match
** the 'filename' with each file id in the directory - ignoring version numbers
** also, it will ignore directories.
** If it finds a match, return 1 and fill in 'fi' else return 0 if not found
**	If during this search, we detect the end of the directory, then return -1
*/
long search(char *filename, FileInfo *fi)
{
	long dir_pos;
	u_char *dir_rec;
	u_char flag;

	dir_pos = 0;
	dir_rec = (u_char *)dir_cache;

	while (1)
	{
		flag = DIR_FLAG(dir_rec);
		if ( !(flag & FLAG_IS_DIR) )
		{
			/* Just check FILES, not DIRECTORIES */ 
			if (match_file(filename, DIR_FILEID(dir_rec), DIR_FILEID_LEN(dir_rec)))
			{
				fi->loc = DIR_FILE_LOC(dir_rec);
				fi->size = DIR_DATA_LEN(dir_rec);
				return (1);
			}
		}			
		dir_pos += DIR_LEN(dir_rec);
		/* We are either pointing at the next dir_rec OR, we are pointing at
		** 00 padding at end of sector. If its 00 padding, go to next sector
		** if its within the cache.

		** If our complete dir_pos (including previous caches) exceeds the file
		** size of the directory, then we are past the end of the directory.
		*/
		if ((cached_dir_loc-cached_dir_start_loc)*2048 + dir_pos >= cached_dir_file_len)
		{
			return (-1);
		}
		dir_rec = ((u_char *)dir_cache) + dir_pos;
		if (dir_pos >= DIR_CACHE_SIZE)
		{
			return (0);			/* Gone beyond cache, but we don't know if this is the end */
		}
		if (*dir_rec == 0)
		{
			/* Try the next sector for a valid dir_rec */
			dir_pos = ((dir_pos + 2047) >> 11) << 11;
			if ((cached_dir_loc-cached_dir_start_loc)*2048 + dir_pos >= cached_dir_file_len)
			{
				/* This is the end of the directory */
				return (-1);
			}
			if ( dir_pos >= DIR_CACHE_SIZE)
			{
				/* We've reached the end of this CACHE - but not necessarily
				** the end of the directory */
				return (0);
			}
			dir_rec = ((u_char *)dir_cache) + dir_pos;
			/* If we get here, then there are more dir_recs in the next sector, carry on */
		}
	}	/* end while(1) */
	/* We never get out of the above loop - we just return */
}

/*
** find_file  will look at the directory at 'loc' and try find a file matching filename.
** If it finds it, it returns 1 and fills in 'fi' else it returns 0
*/
long find_file(char *filename, long loc, FileInfo *fi)
{
	long res, dont_search, search_loc;
	CdlLOC cd_loc;
	long cnt;
	u_char *dir_rec;

	/* Make sure that directory record is loaded or part loaded */
	if ((loc != cached_dir_loc) && (loc != cached_dir_start_loc))	
	{
		/* We have not got this directory cached at all */
		cache_size = DIR_CACHE_SIZE;

		LogicalSectorToLoc(loc, &cd_loc);
		CdControlF(CdlSeekL, (u_char *)&cd_loc);
		WaitEvent(ev_comp);
		CdRead(DIR_CACHE_SIZE/2048, dir_cache, CdlModeSpeed);
		CdReadSync(0,0);
	
		cached_dir_loc = cached_dir_start_loc = loc;
		/* Here, we are always guaranteed to be looking at a . directory
		** record. It is the first time for the newly cached directory and
		** the time is right for recording the cached_dir_file_len
		*/
		dir_rec = (u_char *)dir_cache;
		cached_dir_file_len = DIR_DATA_LEN(dir_rec);
	}
	/* We now definitely have part of the required dir loaded */
	res = search(filename, fi);
	if (res == 1)
	{
		/* We found the file, filled in fi - return 1 (success) */
		return (1);
	}
	else if (res == -1)
		{
			/* Not only did we fail, but also, the end of the directory was found.
			** IF the directory size is <= cache size, then this really IS the end
			** of our search and we have failed. If not, then we must go and check 
			** the rest of the directory */
			if (cached_dir_file_len <= DIR_CACHE_SIZE)
			{
				/* We have searched all parts of the directory, return 0 (fail) */
				printf("iso9660:find_file: Can't find file in directory path (and dir fitted in cache!)\n");
				return (0);
			}
		}
	/* Otherwise, if we get here, we didn't find the file in the cached
	** Part of the diredctory - load some more and continue the search */

	dont_search = cached_dir_loc;		/* don't serach the cached bit again */
	search_loc = cached_dir_start_loc;	/* start at beginning of dir */

	/* While the search fails to find a file AND did not get to the end of dir*/
	res = 0;
	while(res==0) 
	{
		if (search_loc == dont_search)
		{
			search_loc += (DIR_CACHE_SIZE/2048);
			if ((search_loc - cached_dir_start_loc)*2048 >= cached_dir_file_len)
			{
				/* We are trying to skip the LAST 'cache' in the directory - don't we've finished */
				printf("iso9660:find_file: Can't find file in directory path (skipped last cache)\n");
				return (0);
			}
		}
		cache_size =  DIR_CACHE_SIZE;

		LogicalSectorToLoc(search_loc, &cd_loc);
		CdControlF(CdlSeekL, (u_char *)&cd_loc);
		WaitEvent(ev_comp);
		CdRead(DIR_CACHE_SIZE/2048, dir_cache, CdlModeSpeed);
		CdReadSync(0,0);
	
		cached_dir_loc = search_loc;
		res = search(filename, fi);
		search_loc += (DIR_CACHE_SIZE/2048);
	}
	/* If we have fallen out, then if res==1 we've succeeded or res==-1, we
	** Failed and directory end was encountered */
	if (res == 1)
	{
		return (1);
	}
	else
	{
		printf("iso9660:find_file: Can't find file in directory path(exhausted)\n");
		return(0);
	}
}

	

/*
** iso_get_file_info  will search the ISO9660 directory structure for the full absolute
** pathname. If it finds it, it will return 1 and fill in 'fi' else return 0
*/
long iso_get_file_info(char *pathname, FileInfo *fi)
{
	char dir_name[8][10];
	char tmp_string[13];
	char *tmp_s,*c, *filename, *search_s;
	long num_levels = 0;
	u_char  *path;
	long fname_len;
	long prev_dir, dir_num;
	long path_level = 0, loc;

	tmp_s = tmp_string;
	c = pathname;

	while (*c != '\\')
	{
		if (*c == 0)
			return 0;
		c++;					/* Skip any drive specs eg. c: or cdrom: etc */
	}
	while (*c == '\\')
	{
			c++;					/* Skip any leading \ characters */
	}
	while(*c != 0)		/* while not end of string */
	{
		if (*c == '\\')
		{
			while(*c == '\\')	
				c++;					/*skip any extra \ s */
			*tmp_s = 0;				/* Terminate the string */
			if ((tmp_s - tmp_string) > 8)
			{
				printf("iso9660.c:iso_get_file_info: Directory name is too long\n");
				return(0);
			}
			strcpy(dir_name[num_levels], tmp_string);	/* Store the directory */
			num_levels++;
			tmp_s = tmp_string;	/* reset pointer */
		}
		/* copy characters from string */
		*tmp_s++ = toupper(*c++);
	}
	*tmp_s = 0;		/* terminate */
	fname_len = tmp_s - tmp_string;
	/* The filename is now stored in tmp_string and 'level' directories in dir_names */
	filename = tmp_string;
	if ((fname_len < 1) || (fname_len > 12))
	{
		printf("iso9660.c:iso_get_file_info: Filename is too long or does not exist\n");
		return (0);
	}

	path = (u_char *)PathTable;
	if (path_level == 0)
	{
		loc = PATH_FILE_LOC(path);			 /* the root directory */
	}
	prev_dir = 1;					/* Root is dir 1, in iso9660 there is no 0 */
	dir_num = 2;						/* start search below root */
	path = NextPath(path);
	for (path_level = 0; path_level < num_levels; path_level++)
	{
		search_s = dir_name[path_level];
#if 0	/* *** SEE EXPLANATION OF THIS AFTER FUNCTION */
		while((path != NULL) && (PATH_PARENT_DIR(path) <= prev_dir))	/* While not finished */
#else
  		while(path != NULL)	/* Exhaustive search, makes little difference really */
#endif
		{
			if ((PATH_PARENT_DIR(path) == prev_dir) && (match_dir( search_s, PATH_DIRID(path),PATH_LEN_DIRID(path))))
			{
				loc = PATH_FILE_LOC(path);
				break;
			}
			path = NextPath(path);
			dir_num++;
		}
		/* if path==NULL, then failed to match else found this 'search_s' */
		if (path == NULL)
		{
			return(0);
		}
		prev_dir = dir_num;
		dir_num++;
		path = NextPath(path);		/* We should start on the one after the match for next bit of search */
	}

	/* Now start searching for a filename match in the directory */
	/* We have the directory 'loc' and the filename in upper case */
	/* However, we also need to know the total size of the directory */
	/* we are about to search. Fortunately, I've already been through */
	/* and filled this in */
	return (find_file(filename,loc, fi));
}



#ifndef USE_ATI

	
/* *** EXPLANATION
** According to iso9660 (6.9.1), the order of path records in the path
** table will abide by the following rules in order of importance:
** 1) The level of the directory described by the record. Low numbers first.
** 2) The number of the parent directory - lowest first.
** 3) Alphabetically in the strict sense.
** So, the line commented out, short circuits the search by utilising the
** above order. If the parent dir num is greater than the last directory
** we 'found', then we have been through the directory and not found a
** match. Exit now. However, it would seem that this ordering is not always
** adhered to (buildcd for one). Here, directories are traversed with the 
** parent number order being more significant than the hierarchy level.
** Thus, the alteration to the algorithm.
*/

/*
** opens the file denoted by the absolute pathname with \ seperators 
** \\ in C source. There is no ;1 and no cdrom: it is also case insensitive
** The flags currently does NOTHING.
*/
long iso_open(char *pathname, long flags)
{
	long fd;

	fd = 0;
	while (fd_tab[fd].fd != 0)
	{
		fd++;
		if (fd == NUM_OF_FD)
		{
			printf("iso9660.c:iso_open:No file descriptors free\n");
			return (-1);
		}
	}
	if (iso_get_file_info(pathname, &(fd_tab[fd].fi)))
	{
		fd_tab[fd].fd = fd+1;
		num_of_open_files++;
		fd_tab[fd].fp = fd_tab[fd].fi.loc;	/* Reset file pointer */
		return fd+1;
	}
	else
	{
		printf("iso9660.c:iso_open:Failed to open file %s \n", pathname);
		return (-1);
	}
}
#endif //USE_ATI

#ifndef USE_ATI


void iso_close(long fd)
{
	if (fd_tab[fd-1].fd != 0)
	{
		fd_tab[fd-1].fd = 0;
		num_of_open_files--;
	}
}
#endif //USE_ATI
#ifndef USE_ATI


/*
** Read from the file 'fd' from the current file position, nsec sectors
** of data into buffer. If successful, return the number of sectors read.
** If it fails, return 0;
*/
long iso_read(u_char *buffer, long nsec, long fd)
{
	CdlLOC cd_loc;
	long cnt;
	int	check = -1;
	
	if (nsec == 0)
		return (0);
	if (fd_tab[fd-1].fd != fd)
	{
		printf("iso9660.c:iso_read:this fd is not valid\n");
		return 0;
	}
	fd--;
	if ((fd_tab[fd].fp - fd_tab[fd].fi.loc + ((fd_tab[fd].fi.size+2047) >> 11)) < nsec)
	{
		/* If the number of remaining sectors in the file is less than nsec,
		** then we are trying to read too much. Fail now and return 0.
		*/
		return (0);
	}
	LogicalSectorToLoc(fd_tab[fd].fp, &cd_loc);
	while (check == -1)
	{
		CdControlF(CdlSeekL, (u_char *)&cd_loc);
		WaitEvent(ev_comp);
		CdRead(nsec, (u_long*)buffer, CdlModeSpeed);
		check=CdReadSync(0,0);
	}

	/* Botch the error condition for now as its handled by the
	** interrupt driven error handler currently
	*/
	fd_tab[fd].fp += nsec;
	return nsec;
	
}

#endif //USE_ATI
