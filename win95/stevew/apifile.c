#define O_RDONLY r
#define O_WRONLY a
#define O_RDWR r+w
#define O_CREAT wb
#define O_NOBUF NULL
#define O_NOWAIT NULL

/*make the changes*/
#include "c:\w\h\stdio.h"

/* Return drive name */
unsigned char GetDrive(char *buffer)
{
	/* temporary drive assignment */
	if (buffer[-2]='o')
		return 'd';
	else
		return 'c';		
}


/* Convert playstation filename to PC format */
char* ConvName(char *filename)
{
	unsigned char *ptr, *srch;

	ptr = strchr(filename,':');
	if (ptr != NULL)
	{
		ptr--;
		ptr[0] = GetDrive(ptr);
	}
	else
		ptr = filename;

	srch = strchr(ptr,';');

	if (srch != NULL)
		srch[0] = 0;
	return (ptr);

}

/* Change Current Directory */
long cd(char *path)
{
	if (chdir(ConvName(path)))
		return 0;
	else
		return 1;
}


/* Close File */
long close(unsigned long fd)
{
	if (fclose((FILE *) fd))
		return -1;
	else
		return fd;		
}


/* Delete a file */
long delete (char *name)
{
	if (remove(convname(name)))
		return 0;
	else
		return 1;
}


/* Look up first file */
struct DIRENTRY *firstfile(char *name, struct DIRENTRY *dir)
{
}


/* Initialise file system */
long format(char *fs)
{
	return 1;
}


/* Move a file pointer */
long lseek(unsigned long fd,long offset,long flag)
{
	if (fseek((FILE *)fd,offset,flag))
		return -1;
	else
		return fd;
}


/* Look up next file */
struct DIRENTRY *nextfile(struct DIRENTRY *dir)
{
}


/* Open a file */
unsigned long open(char *devname, unsigned long flag)
{
	char mode[4];
	FILE *fp;

	if ((flag & O_RDWR)==O_RDWR)
		strcpy(mode,"r+w");
	else
	{
		if ((flag & O_RDONLY)==O_RDONLY)
			strcpy(mode,"r");
		else
			if ((flag & O_WRONLY)==O_WRONLY)
				strcpy(mode,"w");
	}
			
 	if (fp=fopen(devname,mode));
		return (unsigned long) fp;
	else
		return -1;
}


/* Read data from a file */
long read(unsigned long fd, void *buf, long n)
{
	return (fread(buf,1,n,(FILE *) fd));
}

#define rename(a,b) !rename(a,b)


long undelete(char *name)
{
	return 0;
}


/* Write data to a file */
long write(unsigned long fd, void *buf, long n)
{
	if (n!=fwrite(buf,1,n,(FILE *) fd))
		return -1;
	else
		return n;
}
