/************************************************************************
*                         User I/O Routines                             *
*************************************************************************
*									*
* This file contains I/O routines implementing a simple in-memory	*
* user-level I/O system.  The routines are:				*
*									*
*		user_open()						*
*		user_close()						*
*		user_read()						*
*		user_write()						*
*		user_ioctl()						*
*		user_fcntl()						*
*		user_mknod()						*
*		user_getmsg()						*
*		user_getpmsg()						*
*		user_putmsg()						*
*		user_putpmsg()						*
*		user_poll()						*
*		user_get_ptr()						*
*		user_set_ptr()						*
*									*
*		user_opendir()						*
*		user_readdir()						*
*		user_closedir()						*
*		user_rewinddir() 					*
*									*
*		user_stat()						*
*		user_fstat()						*
*									*
* Author:	David Grothe	<dave@gcom.com>				*
*									*
************************************************************************/
/*
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 */

#ident "@(#) LiS usrio.c 2.10 10/29/02 22:38:07 "

#include <sys/stream.h>
#include <errno.h>

#include <stdio.h>
#include <stdarg.h>
#include <sys/fcntl.h>
#ifdef SYS_QNX
#include <unix.h>
#endif
#include <string.h>
#include <malloc.h>
#include <time.h>

#include <sys/LiS/usrio.h>


/************************************************************************
*                         Global Storage                                *
************************************************************************/

#if 0

static dir_t		 directory[NDIR] ;	/* pseudo-directory */
static maj_t		 majors[NMAJOR] ;	/* major device table */
static file_t		*files ;		/* file descriptors */
static inode_t		*inodes ;		/* head of list of inodes */
static int		 last_fd ;		/* last used file number */
static file_t	       **file_tbl ;		/* ptrs to file descrs */
static int		 file_tbl_size ;	/* # entries in file tbl */
static int		 user_next_inum = 32;	/* next inode number */

#else

dir_t		 directory[NDIR] ;	/* pseudo-directory */
maj_t		 majors[NMAJOR] ;	/* major device table */
file_t		*files ;		/* file descriptors */
inode_t		*inodes ;		/* head of list of inodes */
int		 last_fd ;		/* last used file number */
file_t	       **file_tbl ;		/* ptrs to file descrs */
int		 file_tbl_size ;	/* # entries in file tbl */
int		 user_next_inum = 32;	/* next inode number */

#endif

/************************************************************************
*                           Prototypes                                  *
************************************************************************/
dir_t		*user_search_dir(char *name) ;
inode_t		*user_inode(dev_t dev) ;
void		 user_print_inode(inode_t *ip) ;
void		 user_print_dir_entry(dir_t *dp, int optns) ;
file_t		*user_file_of_fd(int fd) ;

#define _return(e) return ((e)<0?-(errno=-(e)):(e))

/************************************************************************
*                             user_mknod                                *
*************************************************************************
*									*
* Add a name to the directory.  Allocate an inode and fill in the	*
* major and minor device numbers.					*
*									*
* Return 0 for success, negative for error.				*
*									*
************************************************************************/
int	user_mknod(char *name, int mode, dev_t dev)
{
    int		 d ;
    int		 m ;
    inode_t	*ip;
    dir_t	*dp ;

    if ( user_search_dir(name) != NULL )
	_return(-EEXIST) ;			/* entry exists already */

    if ((m = major(dev)) > NMAJOR) _return(-EBADF) ;	/* range check major */
    if ( dev != 0 )	/* 0 is special case for a '.' entry */
	if (lis_fstr_sw[m].f_str == NULL) _return(-ENXIO) ;/* ensure streamtab */

    for (d = 1; d < NDIR; d++)
    {
	if ((dp = &directory[d])->d_name == NULL)	/* available dir slot */
	{
	    memset(dp, 0, sizeof(dir_t)) ;
	    dp->d_name  = ALLOCF(strlen(name) + 1, "directory/node name ") ;
	    if (dp->d_name == NULL) _return(ENOMEM) ;
	    dp->d_dev   = dev ;
	    dp->d_mode  = mode ;
	    strcpy(dp->d_name, name) ;
	    dp->d_ftime = time( NULL ) ;	/* time created */
	    dp->d_inode = 0;
	    dp->d_ctime = dp->d_ftime ;
	    dp->d_mtime = dp->d_ftime ;
	    dp->d_atime = dp->d_ftime ;
	    dp->d_refcnt= 1 ;
    
	    ip = user_inode(dev);
	    if (ip == NULL)				/* out of inodes */
	    {
	    	FREE(dp->d_name);
	    	dp->d_name = NULL;
		_return(-EMFILE) ;
	    }
	    ip->i_ftime = time(NULL) ;
	    ip->i_ctime = ip->i_ftime ;
	    ip->i_mtime = ip->i_ftime ;
	    ip->i_atime = ip->i_ftime ;
	    ip->i_mode  = mode ;
	    ip->i_flags = REFERENCE_INODE ;

	    dp->d_inode = ip;
	    if (ip->i_inum != 0)			/* inode has number */
		dp->d_inum = ip->i_inum;
	    else					/* give inode number */
	    {					/* assign inode number */
		dp->d_inum = user_next_inum++ ;
		ip->i_inum = dp->d_inum ;
	    }

	    /*
	     * we leave ip hanging around since we do have have a valid
	     * inode now.  No not decrement the ip->i_use counter.
	     * Need to unlock inode so others may use it.
	     */
	    ULOCK_INO(ip) ;				/* unlock inode */
	    return(0) ;				/* success */
	}
    }

    _return(-ENFILE) ;

} /* user_mknod */

/************************************************************************
*                               user_unlink                             *
*************************************************************************
*									*
* Remove a file from the directory.					*
*									*
************************************************************************/
int user_unlink(char *name)
{
    dir_t	*dp ;
    inode_t	*ip ;

    if ( (dp = user_search_dir(name)) == NULL )
	_return(-ENOENT) ;

    if (--dp->d_refcnt > 0)			/* still in use */
	return(0) ;

    if ((ip = dp->d_inode) != NULL)
    {
	dp->d_inode = NULL ;			/* detach from inode */
	LOCK_INO(ip) ;
	if (ip->i_use == 1)
	{
	    ip->i_use-- ;			/* frees the inode */
	    ip->i_str = NULL ;			/* detach from streams */
	    lis_mark_mem(ip, "unused inode: ", major(ip->i_dev)) ;
	}

	ULOCK_INO(ip) ;
    }

    FREE(dp->d_name) ;
    dp->d_name = NULL ;

    dp->d_dev   = 0 ;
    dp->d_mode  = 0 ;
    dp->d_ftime = 0 ;
    dp->d_ctime = 0 ;
    dp->d_mtime = 0 ;
    dp->d_atime = 0 ;
    dp->d_inum  = 0 ;

    return(0) ;

} /* user_unlink */

/************************************************************************
*                          user_add_dir_entry                           *
*************************************************************************
*									*
* Add an entry into the directory structure				*
*									*
************************************************************************/

int	user_add_dir_entry(char *name, int mode, dev_t dev)
{

    int		 d ;
    int		 m ;
    dir_t	*dp ;

    if ( user_search_dir(name) != NULL )
	_return(-EEXIST) ;			/* entry exists already */

    if ((m = major(dev)) > NMAJOR) _return(-EBADF) ;	/* range check major */

    for (d = 1; d < NDIR; d++)
    {
	if ((dp = &directory[d])->d_name == NULL)	/* available dir slot */
	{
	    memset(dp, 0, sizeof(dir_t)) ;
	    dp->d_name  = ALLOCF(strlen(name) + 1, "directory/node name ") ;
	    if (dp->d_name == NULL) _return(-ENOMEM) ;
	    strcpy(dp->d_name, name) ;

	    dp->d_dev   = dev ;
	    dp->d_mode  = mode ;
	    dp->d_ftime = time( NULL ) ;	/* time created */
	    dp->d_inode = 0;
	    dp->d_ctime = dp->d_ftime ;
	    dp->d_mtime = dp->d_ftime ;
	    dp->d_atime = dp->d_ftime ;
	    dp->d_inum  = user_next_inum++ ;	/* assign inode number */
    
	    _return(0) ;				/* success */
	}
    }
    _return(-ENFILE) ;
}

int
user_remove_dir_entry(dev_t dev)
{
    int		 d;
    dir_t	*dp ;

    for (d = 1; d < NDIR; d++)
    {
	if ((dp = &directory[d])->d_dev == dev)	/* available dir slot */
	{
	    if (--dp->d_refcnt > 0)
		return(0) ;			/* done enough */

	    if ( dp->d_name )
		FREE(dp->d_name);

	    dp->d_name  = 0 ;
	    dp->d_dev   = 0 ;
	    dp->d_mode  = 0 ;
	    dp->d_ftime = 0 ;
	    dp->d_inode = 0 ;
	    dp->d_ctime = 0 ;
	    dp->d_mtime = 0 ;
	    dp->d_atime = 0 ;
	    dp->d_inum  = 0 ;
    
	    return(0) ;				/* success */
	}
    }
    return(-1);
}
/************************************************************************
*                          user_search_dir                              *
*************************************************************************
*									*
* Search the directory for the given name, return inode number.		*
*									*
************************************************************************/
dir_t	*user_search_dir(char *name)
{
    int		 d ;
    dir_t	*dp ;

    for (d = 0, dp = &directory[0]; d < NDIR; d++, dp++)
    {
	if (   dp->d_name != NULL		/* name valid */
	    && strcmp(dp->d_name, name) == 0	/* name match */
	   )
	    return( dp ) ;
    }

    return(NULL) ;

} /* user_search_dir */

/************************************************************************
*                        user_search_dir_inode                          *
*************************************************************************
*									*
* Search the directory for an entry that has the same inumber as this	*
* inode.								*
*									*
************************************************************************/
dir_t	*user_search_dir_inode(inode_t *i)
{
    int		 d ;
    dir_t	*dp ;

    for (d = 0, dp = &directory[0]; d < NDIR; d++, dp++)
    {
	if (dp->d_refcnt != 0 && dp->d_inode == i)
	    return( dp ) ;
    }

    return(NULL) ;

} /* user_search_dir_inode */

/************************************************************************
*                             alloc_file_tbl                            *
*************************************************************************
*									*
* Allocate the file table pointer array.  If one exists already then	*
* allocate a bigger one and copy the old pointers into the new array.	*
*									*
* Return 1 for success, 0 for failure.					*
*									*
************************************************************************/
#if 0
static int	alloc_file_tbl(void)
#else
int	alloc_file_tbl(void)
#endif
{
    file_t	**new_tbl ;
    file_t	**old_tbl ;
    int		  new_size ;

    if (last_fd < file_tbl_size) return(1) ;

    new_size = file_tbl_size + 32 ;
    new_tbl = ALLOCF(new_size * sizeof(*new_tbl), "file_tbl: ") ;
    if (new_tbl == NULL)
	return(0) ;				/* couldn't */
    memcpy(new_tbl, file_tbl, file_tbl_size * sizeof(*new_tbl)) ;
    old_tbl = file_tbl ;
    file_tbl = new_tbl ;
    file_tbl_size = new_size ;
    FREE(old_tbl) ;

    return(1) ;

} /* alloc_file_tbl */

/************************************************************************
*                           user_alloc_file                             *
*************************************************************************
*									*
* Allocate a file structure and return a pointer to it.  Use an old	*
* one if possible.							*
*									*
* Return with the structure locked and usage count incremented.		*
*									*
************************************************************************/
file_t		*user_alloc_file(void)
{
    file_t	*fp ;

    for (fp = files; fp != NULL; fp = fp->f_link)
    {
	LOCK_FILE(fp) ;					/* lock the file */
	if (fp->f_count == 0)				/* is a free file */
	{
	    fp->f_count    = 1;				/* incr in-use cnt */
	    fp->f_inode    = NULL;
	    fp->f_ptr      = NULL;
	    fp->f_drvr_ptr = NULL;
	    lis_mark_mem(fp, "file: ", fp->f_fdnr) ;
	    return(fp) ;
	}

	ULOCK_FILE(fp) ;				/* unlock it */
    }

    fp = ALLOC(sizeof(*fp)) ;
    if (fp == NULL) return(NULL) ;		/* out of memory */

    memset(fp, 0, sizeof(*fp)) ;		/* clear entry */
    fp->f_fdnr = ++last_fd ;			/* assign next file number */
    lis_mark_mem(fp, "file: ", fp->f_fdnr) ;
    if (!alloc_file_tbl())			/* ensure index slot */
    {						/* ooops, couldn't  */
	FREE(fp) ;
	return(NULL) ;
    }

    lis_sem_init(&fp->f_sem, 1) ;		/* file lock */
    LOCK_FILE(fp) ;				/* lock the file */
    file_tbl[fp->f_fdnr] = fp ;			/* load index slot */

    fp->f_link = files ;			/* link into list */
    files = fp ;

    fp->f_count++ ;				/* incr usage count */
    return(fp) ;

} /* user_alloc_file */

/************************************************************************
*                            user_inode                                 *
*************************************************************************
*									*
* Return a pointer to an inode structure for the given device.		*
*									*
* We return with the inode structure locked and the use count incremented*
*									*
************************************************************************/
inode_t		*user_inode(dev_t dev)
{
    inode_t	*ip ;

    for (ip = inodes; ip != NULL; ip = ip->i_link)
    {
	if (ip->i_use != 0 && ip->i_dev == dev)
	{
	    LOCK_INO(ip) ;				/* lock it */
	    if (ip->i_use == 0 || ip->i_dev != dev)
	    {						/* lost race */
		ULOCK_INO(ip) ;
		continue ;
	    }

	    lis_mark_mem(ip, "inode: ", major(dev)) ;
	    ip->i_use++ ;				/* incr use count */
	    return(ip) ;				/* found the inode */
	}
    }

    for (ip = inodes; ip != NULL; ip = ip->i_link)
    {
	if (ip->i_use == 0)				/* is a free inode */
	{
	    LOCK_INO(ip) ;				/* lock the inode */
	    if (ip->i_use != 0)
	    {						/* lost race */
		ULOCK_INO(ip) ;
		continue ;
	    }

	    ip->i_dev	= dev ;				/* set dev */
	    ip->i_mode	= 0 ;				/* clear mode */
	    ip->i_str	= NULL ;			/* clr strms ptr */
	    ip->i_use++ ;				/* incr in-use cnt */

	    ip->i_ftime = time(NULL) ;
	    ip->i_ctime = ip->i_ftime ;
	    ip->i_atime = ip->i_ftime ;
	    ip->i_mtime = ip->i_ftime ;

	    lis_mark_mem(ip, "inode: ", major(dev)) ;
	    return(ip) ;				/* rtn w/inode locked */
	}
    }

    ip = ALLOC(sizeof(*ip)) ;
    if (ip == NULL) return(NULL) ;		/* out of memory */

    memset(ip, 0, sizeof(*ip)) ;		/* clear entry */
    lis_mark_mem(ip, "inode: ", major(dev)) ;
    ip->i_dev = dev ;				/* set dev */
    lis_sem_init(&ip->i_sem, 1) ;		/* inode lock */
    LOCK_INO(ip) ;				/* lock the inode */
    ip->i_link = inodes ;			/* link into list */
    inodes = ip ;
#if 0
    ip->i_inum = user_next_inum++ ;		/* assign inode number */
#endif

    ip->i_ftime = time(NULL) ;
    ip->i_ctime = ip->i_ftime ;
    ip->i_atime = ip->i_ftime ;
    ip->i_mtime = ip->i_ftime ;

    ip->i_use++ ;				/* incr usage count */
    return(ip) ;

} /* user_inode */

/************************************************************************
*                            user_open                                  *
*************************************************************************
*									*
* This is like the system open() routine.  It looks up the file in	*
* the directory and returns a file handle for it.			*
*									*
************************************************************************/
int	user_open(char *name, int flags, int mode)
{
    int		 m ;
    int		 fd ;
    int		 rslt = -1;
    dir_t	*dp ;
    inode_t	*ip ;
    file_t	*fp ;
    maj_t	*mp ;
    dev_t	 odev ;

#define	RTN(e)		{rslt = e; goto return_point; }
    dp = user_search_dir(name) ;		/* find name in directory */
    if (dp == NULL)
	_return(-ENOENT) ;		/* not found */
    ip = user_inode(dp->d_dev) ;		/* ptr to inode */
    if (ip == NULL)				/* out of inodes */
	_return(-EMFILE) ;

    /*
     * Inode returned locked and usage count bumped up
     */
    ip->i_mode = dp->d_mode ;			/* file's mode */

    if ( dp->d_inode == 0 )
    {
	dp->d_inode = ip;
	ip->i_ctime = dp->d_ftime ;
	ip->i_ftime = ip->i_ctime ;
	ip->i_mtime = ip->i_ctime ;
	ip->i_atime = ip->i_ctime ;
    }

    ip->i_inum  = dp->d_inum  ;

    m = major(ip->i_dev) ;			/* get major number */
    if (m <= 0 || m >= NMAJOR)
    {
	ip->i_use-- ;
	ULOCK_INO(ip) ;
	_return(-ENXIO) ;			/* no device */
    }

    mp = &majors[m] ;				/* to maj device entry */

    fp = user_alloc_file() ;
    if (fp == NULL)
    {
	ip->i_use-- ;
	ULOCK_INO(ip) ;
	_return(-EMFILE) ;			/* out of file descrs */
    }

    /*
     * Use RTN() macro below here
     */

    fd = fp->f_fdnr ;				/* file number */
    fp->f_flags = flags ;			/* save flags */
    fp->f_inode = ip ;				/* tie to inode */
    fp->f_ptr   = NULL ;			/* null out pointer */
    fp->f_op    = mp->m_op ;			/* xfer file open/close ops */
    fp->f_name	= ALLOCF(strlen(name) + 1, "filename 1 ") ;
    if (fp->f_name == NULL)
	RTN(-ENOMEM) ;
    strcpy(fp->f_name, name) ;			/* save name */

    if (fp->f_op->open != NULL)			/* is there an open proc?  */
    {
	odev = ip->i_dev ;			/* original device */
	rslt = fp->f_op->open(ip, fp) ;		/* call it */
	if (rslt < 0)				/* open error */
	{
	    if ( odev != ip->i_dev )
	        ip->i_dev = odev;
	    RTN(rslt) ;
	}
	RTN(fd) ;				/* return file descr */
    }
    else					/* no open routine */
	RTN(-ENXIO) ;

return_point:

    if (rslt < 0)
    {
	ip->i_use-- ;				/* give up inode */
	fp->f_inode = NULL ;
	fp->f_count-- ;				/* give up file struct */
	if (fp->f_name != NULL)
	{
	    FREE(fp->f_name) ;
	    fp->f_name = NULL ;
	}
    }

    ULOCK_INO(ip) ;				/* unlock inode */
    ULOCK_FILE(fp) ;				/* unlock file */

    _return(rslt) ;

} /* user_open */

/************************************************************************
*                          user_file_of_inode                           *
*************************************************************************
*									*
* Find a file that points to the given inode.				*
*									*
************************************************************************/
file_t *
user_file_of_inode(inode_t * i)
{
    file_t	*fp ;

    for (fp = files; fp != NULL; fp = fp->f_link)
    {
	if (fp->f_count != 0 && fp->f_inode == i)
	{
	    return(fp) ;
	}
    }

    return(NULL) ;

} /* user_file_of_inode */

/************************************************************************
*                           user_inode_of_file                          *
*************************************************************************
*									*
* Given a numerical file descriptor, return a pointer to the inode	*
* structure for it, or NULL if the fd is invalid.			*
*									*
************************************************************************/
inode_t	*
user_inode_of_file(int fd)
{
    file_t	*fp = user_file_of_fd(fd) ;

    if (fp == NULL)
	return(NULL) ;			/* no device */

    return(fp->f_inode) ;

} /* user_inode_of_file */

/************************************************************************
*                           user_file_of_fd                             *
*************************************************************************
*									*
* Given a numerical file descriptor, return the file ptr for it.	*
*									*
************************************************************************/
file_t *
user_file_of_fd(int fd)
{
    file_t	*fp ;

    if (fd < 0 || fd > last_fd || (fp = file_tbl[fd]) == NULL)
	return(NULL) ;

    return(fp) ;

} /* user_file_of_fd */

/************************************************************************
*                          user_close                                   *
*************************************************************************
*									*
* Like the system close() routine.					*
*									*
************************************************************************/
int	user_close(int fd)
{
    inode_t	*ip ;
    file_t	*fp = user_file_of_fd(fd) ;

    if (fp == NULL)
	return(-ENXIO) ;			/* no device */

    ip = fp->f_inode ;
    if (ip == NULL) return(-EBADF) ;		/* file not open */

    LOCK_FILE(fp) ;
    if (fp->f_count > 0)			/* be cautious */
	fp->f_count-- ;				/* keep use count */

    if (fp->f_count == 0)			/* done with file */
    {
	if (fp->f_op->release != NULL)
	    fp->f_op->release(ip, fp) ;		/* call close proc */

	if (fp->f_name != NULL) FREE(fp->f_name) ;
	fp->f_inode = NULL ;			/* done with file */
	lis_mark_mem(fp, "unused file: ", fp->f_fdnr) ;
    }

    ULOCK_FILE(fp) ;

    LOCK_INO(ip) ;
    if (ip->i_use > 0)				/* inode usage count */
    {
	ip->i_use-- ;

	if ( ( ip->i_use == 1 ) && ( ip->i_flags & REFERENCE_INODE ) )
	{
	    ip->i_str = NULL ;
	}
	else if (ip->i_use <= 1)
	{
	    if (ip->i_use == 1 && user_search_dir_inode(ip) != NULL)
		ip->i_use-- ;		/* dir entry is only user left */

	    if (ip->i_use <= 0)		/* all usages gone */
	    {				/* remove the directory entry */
		ip->i_str = NULL ;
		lis_mark_mem(ip, "unused inode: ", major(ip->i_dev)) ;
		if ( user_remove_dir_entry(ip->i_dev) != 0 )
		{
		    printk("user_close: directory entry was not found\n");
		}
	    }
	}
    }

    ULOCK_INO(ip) ;

    return(0) ;					/* success */

} /* user_close */

/************************************************************************
*                            user_read                                  *
*************************************************************************
*									*
* Read bytes from the file.						*
*									*
************************************************************************/
int	user_read(int fd, void *buf, int nbytes)
{
    inode_t	*ip ;
    file_t	*fp = user_file_of_fd(fd) ;
    int		 err ;

    if (fp == NULL)
	return(-ENXIO) ;			/* no device */

    ip = fp->f_inode ;
    if (ip == NULL || fp->f_count == 0)
	return(-EBADF) ;			/* file not open */

    if (fp->f_op->read != NULL)
	err = fp->f_op->read(fp, buf, nbytes, NULL) ;
    else
	err = EINVAL ;				/* no ioctl routine */

    return(err) ;

} /* user_read */

/************************************************************************
*                            user_write                                 *
*************************************************************************
*									*
* Write bytes to the file.						*
*									*
************************************************************************/
int	user_write(int fd, void *buf, int nbytes)
{
    inode_t	*ip ;
    file_t	*fp = user_file_of_fd(fd) ;
    int		 err ;

    if (fp == NULL)
	return(-ENXIO) ;			/* no device */

    ip = fp->f_inode ;
    if (ip == NULL || fp->f_count == 0)
	return(-EBADF) ;			/* file not open */

    LOCK_INO(ip) ;				/* lock it (POSIX) */
    if (fp->f_op->write != NULL)
	err = fp->f_op->write(fp, buf, nbytes, NULL) ;
    else
	err = -EINVAL ;				/* no ioctl routine */

    ULOCK_INO(ip) ;
    return(err) ;

} /* user_write */

/************************************************************************
*                            user_ioctl                                 *
*************************************************************************
*									*
* Issue an ioctl to the file.						*
*									*
************************************************************************/
int	user_ioctl(int fd, int cmd, ...)
{
    inode_t	*ip ;
    file_t	*fp = user_file_of_fd(fd) ;
    int		 err ;
    long	 arg ;
    va_list	 args ;

    va_start(args, cmd) ;
    arg = va_arg(args, long) ;
    va_end(args) ;

    if (fp == NULL)
	return(-ENXIO) ;			/* no device */

    ip = fp->f_inode ;
    if (ip == NULL || fp->f_count == 0)
	return(-EBADF) ;			/* file not open */

    if (fp->f_op->ioctl != NULL)
	err = fp->f_op->ioctl(ip, fp, cmd, arg) ;
    else
	err = EINVAL ;				/* no ioctl routine */

    return(err) ;

} /* user_ioctl */

/************************************************************************
*                          user_fcntl                                   *
*************************************************************************
*									*
* Implement SVR4 compatible fcntl functions.				*
*									*
************************************************************************/

#define	FC_FLAGS	(O_APPEND | O_NDELAY | O_NONBLOCK | O_SYNC)

int	user_fcntl(int fd, int cmd, ...)
{
    inode_t	*ip ;
    file_t	*fp = user_file_of_fd(fd) ;
    int		 err ;
    int		 argi ;
    va_list	 args ;

    if (fp == NULL)
	return(-ENXIO) ;			/* no device */

    ip = fp->f_inode ;
    if (ip == NULL || fp->f_count == 0)
	return(-EBADF) ;			/* file not open */

    va_start(args, cmd) ;

    switch (cmd)
    {
    case F_DUPFD:
      {
	file_t	*xfp ;
	file_t	*nfp = NULL ;
	int	 nfnr = 10000 ;

	argi = va_arg(args, int) ;
	for (xfp = files; xfp != NULL; xfp = xfp->f_link)
	{
	    LOCK_FILE(xfp) ;			/* lock the file */

	    if (   xfp->f_count == 0		/* is a free file */
		&& xfp->f_fdnr >= argi		/* file nr geq arg */
		&& xfp->f_fdnr < nfnr		/* smaller than b4 */
	       )
	    {
		nfnr = xfp->f_fdnr ;
		nfp = xfp ;
	    }

	    ULOCK_FILE(xfp) ;			/* unlock it */
	}

	if (nfp != NULL)			/* found one */
	{
	    nfp->f_count++ ;			/* incr in-use cnt */
	    lis_mark_mem(nfp, "file: ", nfp->f_fdnr) ;
	}
	else					/* get one */
	{
	    nfp = user_alloc_file() ;
	    if (nfp != NULL)
		nfnr = nfp->f_fdnr ;
	}

	if (nfp == NULL)			/* couldn't do it */
	{
	    err = -ENXIO ;
	    nfp->f_count-- ;			/* did not use file after all */
	    break ;
	}

	nfp->f_name = ALLOCF(strlen(fp->f_name) + 1, "filename 2 ") ;
	if (nfp->f_name == NULL)
	{
	    err = -ENOMEM ;
	    break ;
	}

	strcpy(nfp->f_name, fp->f_name) ;	/* save name */

	nfp->f_flags  = fp->f_flags ;		/* save flags */
	nfp->f_inode  = fp->f_inode ;		/* tie to inode */
	ip->i_use++ ;				/* another use of i-node */
	nfp->f_op     = fp->f_op ;		/* xfer file open/close ops */

	ULOCK_FILE(nfp) ;			/* unlock new file */

	err = nfnr ;				/* return file number */
      }
      break ;

    case F_GETFD:				/* close-on-exec flag */
	err = 0 ;				/* pretend zero */
	break ;

    case F_SETFD:				/* close-on-exec flag */
	err = 0 ;				/* pretend OK */
	break ;

    case F_GETFL:				/* get flags */
	err = fp->f_flags & FC_FLAGS ;
	break ;

    case F_SETFL:				/* set flags */
	argi = va_arg(args, int) ;
	fp->f_flags &= ~FC_FLAGS ;
	fp->f_flags |= argi & FC_FLAGS ;
	err = 0 ;
	break ;

    case F_GETOWN:				/* get owner */
	err = 0 ;				/* pretend root is owner */
	break ;

    case F_SETOWN:				/* set owner */
	err = 0 ;				/* pretend OK */
	break ;

    case F_FREESP:				/* free space */
    case F_SETLK:				/* set lock */
    case F_SETLKW:
    case F_RSETLK:
    case F_RSETLKW:
    case F_GETLK:
	err = -EINVAL ;				/* disk files only */
	break ;

    default:
    case F_CHKFL:				/* reserved */
    case F_ALLOCSP:				/* reserved */
	err = -EINVAL ;
    }

    va_end(args) ;

    return(err) ;

} /* user_fcntl */

/************************************************************************
*                           user_putpmsg                                *
*************************************************************************
*									*
* Interface routine to streams putpmsg() routine.			*
*									*
************************************************************************/
int	user_putpmsg(int fd, struct strbuf *ctlptr,
			     struct strbuf *dataptr,
			     int band,
			     int flags)
{
    inode_t	*ip ;
    file_t	*fp = user_file_of_fd(fd) ;
    int		 err ;

    if (fp == NULL)
	return(-ENXIO) ;			/* no device */

    ip = fp->f_inode ;
    if (ip == NULL || fp->f_count == 0)
	return(-EBADF) ;			/* file not open */

    if (fp->f_op->putpmsg != NULL)
	err = fp->f_op->putpmsg(ip, fp, ctlptr, dataptr, band, flags) ;
    else
	err = EINVAL ;				/* no ioctl routine */

    return(err) ;

} /* user_putpmsg */

/************************************************************************
*                             user_getpmsg                              *
*************************************************************************
*									*
* Interface routine to streams getpmsg() routine.			*
*									*
************************************************************************/
int	user_getpmsg(int fd, struct strbuf *ctlptr,
			     struct strbuf *dataptr,
			     int *bandp,
			     int *flagsp)
{
    inode_t	*ip ;
    file_t	*fp = user_file_of_fd(fd) ;
    int		 err ;

    if (fp == NULL)
	return(-ENXIO) ;			/* no device */

    ip = fp->f_inode ;
    if (ip == NULL || fp->f_count == 0)
	return(-EBADF) ;			/* file not open */

    if (fp->f_op->getpmsg != NULL)
	err = fp->f_op->getpmsg(ip, fp, ctlptr, dataptr, bandp, flagsp, 1) ;
    else
	err = -EINVAL ;				/* no ioctl routine */

    return(err) ;

} /* user_getpmsg */

/************************************************************************
*                           user_poll_wakeup                            *
*************************************************************************
*									*
* Called via the polldat structure to wakeup sleeping poll routine.	*
*									*
************************************************************************/
void	user_poll_wakeup(long arg)
{
    lis_semaphore_t	*sem = (lis_semaphore_t *) arg ;

    lis_up(sem) ;

} /* user_poll_wakeup */

/************************************************************************
*                            user_poll                                  *
*************************************************************************
*									*
* Poll file descriptors for operations.					*
*									*
************************************************************************/
int	user_poll(struct pollfd *fds, int nfds, int time_out)
{
    int			 err;
    unsigned long	 size = nfds * sizeof(pollfd_t);
    long		 target_time ;			/* in ms */
    int			 timer_id = 0 ;
    long		 time_interval = time_out ;	/* in ms */
    long		 ms_per_tick = 1000/HZ ;
    long		 ticks ;			/* in system ticks */
    pollfd_t		*pfds;
    pollfd_t		*pfd_ptr ;
    polldat_t		*pdat_head;
    polldat_t		*pdat_ptr ;
    file_t		*fp ;
    int			 i;
    int			 done ;
    int			 ready_fd_cnt = 0 ;
    lis_semaphore_t	 poll_sem ;

    /*
     * Round time interval up to a multiple of system ticks.
     */
    ticks = (time_interval + ms_per_tick - 1) / ms_per_tick ;
    target_time = lis_target_time(ticks*ms_per_tick) ;

    if (   (err=lis_check_umem(NULL,VERIFY_WRITE,fds,size))<0
	|| (err=lis_check_umem(NULL,VERIFY_READ,fds,size))<0
       )
	return(err);

    if ((pfds = ALLOCF(size,"poll-fd ")) == NULL) 
	return(-ENOMEM);

    if ((pdat_head = ALLOCF(nfds * sizeof(polldat_t), "poll-data ")) == NULL)
    {
	FREE(pfds);
	return(-ENOMEM);
    }

    memset(pdat_head, 0, nfds * sizeof(polldat_t)) ;	/* clear to zero */

    /*
     * Obtain copy of user's poll descriptor list
     */
    lis_copyin(NULL,pfds, fds, size);

    lis_sem_init(&poll_sem, 0); 		/* initialize semaphore */

    /*
     * Run through the descriptor list and check each file descriptor
     * for validity.  Mark the ones that are bad.
     */
    for (i = 0, pdat_ptr = pdat_head, pfd_ptr = pfds;
	 i < nfds;
	 i++, pdat_ptr++, pfd_ptr++
	)
    {
	if (   (fp = user_file_of_fd(pfd_ptr->fd)) == NULL
	    || fp->f_op->pollfd == NULL
	   )
	{				/* not an open file descriptor */
	    pfd_ptr->revents = POLLNVAL ;
	    continue ;
	}

	pdat_ptr->pd_fn	    = user_poll_wakeup ;
	pdat_ptr->pd_arg    = (long) &poll_sem ;
    }

    /*
     * Loop until we find a valid file descriptor which statisfies the
     * required conditions.  If there were no good file descriptors
     * then just fall through and return to the user.
     *
     * If no file descriptors are found then P(poll_sem).  When something
     * happens on one of the streams then the user_poll_wakeup function
     * will get called, which will V(poll_sem).  We then go around and
     * poll again.
     *
     * When we exit the loop we will unlink the polldat entries and
     * return to the user.
     *
     * The file system poll function is called with the file info and
     * a pointer to a polldat structure.  The file system routine
     * interrogates the file for the requested conditions according
     * to the 'pd_events' field in the polldat structure.  It returns
     * a bit-mask of the events that satisfy the conditions, if any.
     * The file system also sets the pd_headp pointer to point to
     * its list head in its stream structure for the polldat list.
     * We take care of linking the structure into the list.
     *
     */
    for (done = 0; !done; )
    {
	for (i = 0, ready_fd_cnt = 0, pdat_ptr = pdat_head, pfd_ptr = pfds;
	     i < nfds;
	     i++, pdat_ptr++, pfd_ptr++
	    )
	{
	    if (pfd_ptr->revents & POLLNVAL)
		continue ;			/* only look at good ones */

	    pdat_ptr->pd_events = pfd_ptr->events ;
	    fp = user_file_of_fd(pfd_ptr->fd) ;
	    if ( (pfd_ptr->revents =
		    (short) fp->f_op->pollfd(fp->f_inode, fp, pdat_ptr)) != 0 )
	    {
		done = 1 ;			/* loop breaker */
		ready_fd_cnt++ ;		/* another ready descriptor */
		continue ;			/* skip list manipulations */
	    }

	    if (ready_fd_cnt)			/* skip list manipulations */
		continue ;			/* if have ready fds */

	    if (   pdat_ptr->pd_headp == NULL
	        || pdat_ptr->pd_headp->ph_list == pdat_ptr
	        || pdat_ptr->pd_next != NULL
	        || pdat_ptr->pd_prev != NULL
	       )
		   continue ;		/* no list or already in it */

	    /* lis_spin_lock(&pdat_ptr->pd_headp->sd_lock) ; */

	    if (pdat_ptr->pd_headp->ph_list == NULL)
	    {				/* first entry */
		pdat_ptr->pd_next =
		pdat_ptr->pd_prev = NULL ;
		pdat_ptr->pd_headp->ph_list = pdat_ptr ;
	    }
	    else			/* multiple entries */
	    {
		pdat_ptr->pd_next = pdat_ptr->pd_headp->ph_list ;
		pdat_ptr->pd_prev = NULL ;

		pdat_ptr->pd_headp->ph_list->pd_prev = pdat_ptr ;
		pdat_ptr->pd_headp->ph_list	     = pdat_ptr ;
	    }

	    /* lis_spin_unlock(&pdat_ptr->pd_headp->sd_lock) ; */
	}

	if (done)			/* done */
	    break ;			/* exit while loop */

	/*
	 * We recompute the interval until the original time out
	 * since we may wake up and go around several times before
	 * we finally decide to return.
	 *
	 * A timeout value of -1 means wait forever.
	 */
	if (time_out != -1)
	{
	    if ( (time_interval = lis_time_till(target_time)) <= 0 )
		break ;			/* time's up */

	    ticks = (time_interval + ms_per_tick - 1) / ms_per_tick ;
	    timer_id = timeout((timo_fcn_t *) user_poll_wakeup,
			       (caddr_t) &poll_sem,
			       ticks) ;
	}

	err = lis_down(&poll_sem) ;		/* wait on semaphore */
	if (err < 0) break ;

	if (timer_id != 0)
	    untimeout(timer_id) ;	/* cancel timer */
    }

    /*
     * Go through and unlink all the polldat structures
     */
    for (i = 0, pdat_ptr = pdat_head;
	 i < nfds;
	 i++, pdat_ptr++
	)
    {
	if (pdat_ptr->pd_headp != NULL)	/* have list head specified */
	{
	    /* lis_spin_lock(&pdat_ptr->pd_headp->sd_lock) ; */

	    if (pdat_ptr->pd_headp->ph_list == pdat_ptr)	/* at head */
		pdat_ptr->pd_headp->ph_list = pdat_ptr->pd_next ;

	    if (pdat_ptr->pd_next)	/* next element exists */
		pdat_ptr->pd_next->pd_prev = pdat_ptr->pd_prev ;

	    if (pdat_ptr->pd_prev)	/* previous element exists */
		pdat_ptr->pd_prev->pd_next = pdat_ptr->pd_next ;

	    /* lis_spin_unlock(&pdat_ptr->pd_headp->sd_lock) ; */
	}
    }

    SEM_DESTROY(&poll_sem) ;		/* de-initialize semaphore */
    FREE(pdat_head);			/* done with polldat structures */

    lis_copyout(NULL,pfds, fds, size);	/* update user's poll list */
    FREE(pfds) ;			/* done with fd list */

    if (err < 0)
	return(err) ;			/* probably semaphore error */
    else
	return(ready_fd_cnt) ;		/* return # of ready fds */

} /* user_poll */

/************************************************************************
*                            user_set_ptr                               *
*************************************************************************
*									*
* Our file descriptors have a field 'f_ptr' that the user can use for	*
* his/her own purposes.  This routine stores the user's pointer into	*
* that field.								*
*									*
* The fd must designate an open file.					*
*									*
* Return is negative errno for error, zero for success.			*
*									*
************************************************************************/
int	user_set_ptr(int fd, void *ptr)
{
    file_t	*fp = user_file_of_fd(fd) ;

    if (fp == NULL)
	return(-ENXIO) ;			/* no device */

    if (fp->f_inode == NULL || fp->f_count == 0)
	return(-EBADF) ;			/* file not open */

    fp->f_ptr = ptr ;
    return(0) ;					/* success */

} /* user_set_ptr */

/************************************************************************
*                            user_get_ptr                               *
*************************************************************************
*									*
* Retrieve the user pointer set by user_set_ptr() and return it to	*
* the user.  Return NULL if fd does not designate an open file.		*
*									*
************************************************************************/
void	*user_get_ptr(int fd)
{
    file_t	*fp = user_file_of_fd(fd) ;

    if (fp == NULL)
	return(NULL) ;				/* no device */

    if (fp->f_inode == NULL || fp->f_count == 0)
	return(NULL) ;				/* file not open */

    return(fp->f_ptr) ;

} /* user_get_ptr */




/************************************************************************
*                        port_register_chrdev                           *
*************************************************************************
*									*
* Called to register a streams driver with the filing system.		*
* If 'major' is non-zero then this is the major device to attach to.	*
* if 'major' is zero, then we choose the major device number.		*
*									*
* 'name' is the name of the streams driver.				*
*									*
* Return negative err number or positive major device number.		*
*									*
************************************************************************/
int	port_register_chrdev(unsigned major, const char *name, 
				struct file_operations *fops)
{
    maj_t	*m ;

    if (major == 0)
    {
	for (major = 1; major < NMAJOR; major++)
	    if (majors[major].m_op == NULL) break ;	/* available */
    }

    if (major >= NMAJOR)
	return(-EBADF) ;

    m = &majors[major] ;

    m->m_name = ALLOCF(strlen(name) + 1, "module name ") ;
    if (m->m_name == NULL) return(-ENOMEM) ;
    strcpy(m->m_name, name) ;
    m->m_op = fops ;

    return(major) ;

} /* port_register_chrdev */

/************************************************************************
*                        port_unregister_chrdev                         *
*************************************************************************
*									*
* Un-register the streams driver whose 'major' number is passed in.	*
* The driver name is passed in as well as a consistency check item.	*
* The major slot  must be valid and be associated with this driver.	*
*									*
* Return is negative error number, or zero for success.			*
*									*
************************************************************************/
int	port_unregister_chrdev(unsigned major, char *name)
{
    maj_t	*m ;

    (void) name ;

    if (major >= NMAJOR)
	return(-EBADF) ;

    m = &majors[major] ;
    if (m->m_name != NULL) FREE(m->m_name) ;
    memset(m, 0, sizeof(*m)) ;		/* clear major entry */

    return(0) ;

} /* port_unregister_chrdev */

/************************************************************************
*                          user_opendir                                 *
*************************************************************************
*									*
* Open a "file" for reading the directory.  Return a pointer to a	*
* DIR structure which we then use in future calls to save state info.	*
*									*
* The 'name' argument is supposed to name the directory to open. 	*
* However, since there is only one directory, we ignore this argument.	*
*									*
* Return NULL if the open fails.					*
*									*
************************************************************************/
user_DIR	*user_opendir(char *name)
{
    user_DIR		*dp ;

    (void) name ;

    dp = ALLOCF(sizeof(*dp), "DIR-search-struct ") ;
    if (dp == (user_DIR *) NULL)
	return((user_DIR *) NULL) ;

    memset(dp, 0, sizeof(*dp)) ;		/* clear entry */

    dp->dd_size = NDIR ;
    dp->dd_buf  = ALLOCF(sizeof(user_dirent_t) + 200, "dirent_t ") ;

    if (dp->dd_buf == NULL)
    {
	FREE(dp) ;
	return((user_DIR *) NULL) ;
    }

    memset(dp->dd_buf, 0, sizeof(user_dirent_t) + 200) ;

    return(dp) ;

} /* user_opendir */

/************************************************************************
*                             user_readdir                              *
*************************************************************************
*									*
* Read the next entry from the directory.  Return NULL if at end of	*
* directory.								*
*									*
************************************************************************/
struct user_dirent	*user_readdir(user_DIR *dp)
{
    if (dp == (user_DIR *) NULL)
	return((struct user_dirent *) NULL) ;

    for (; dp->dd_loc < NDIR; dp->dd_loc++)
    {
	dir_t	*dirp ;

	dirp = &directory[dp->dd_loc] ;
	if (dirp->d_name != NULL)		/* name valid */
	{
	    user_dirent_t	 *dep ;		/* ptr to return value */
	    if ( dirp->d_refcnt == 0 )
	    {
	    	continue;
	    }

	    dep = (user_dirent_t *) dp->dd_buf ;/* rtn bfr hanging off of dp */
	    dep->d_ino    = dirp->d_inum;	/* directory inode number */
	    dep->d_off    = dp->dd_loc ;	/* offset of directory entry */
	    dep->d_reclen = (unsigned short)
				(sizeof(*dep) + strlen(dirp->d_name)) ;
	    strcpy(dep->d_name, dirp->d_name) ;
	    dp->dd_loc++ ;			/* advance to next entry */
	    return(dep) ;			/* ptr to dirent structure */
	}
    }

    return((struct user_dirent *) NULL) ;	/* no more entries */

} /* user_readdir */

/************************************************************************
*                           user_rewinddir                              *
*************************************************************************
*									*
* Rewind the search back to the beginning of the directory.		*
*									*
************************************************************************/
void	 user_rewinddir(user_DIR *dp)
{
    if (dp != (user_DIR *) NULL)
	dp->dd_loc = 0 ;

} /* user_rewinddir */

/************************************************************************
*                           user_closedir                               *
*************************************************************************
*									*
* Close the directory "file".						*
*									*
************************************************************************/
int	 user_closedir(user_DIR *dp)
{
    if (dp != (user_DIR *) NULL)
    {
	if (dp->dd_buf != NULL)
	    FREE(dp->dd_buf) ;

	FREE(dp) ;
    }

    return(0) ;

} /* user_closedir */

/************************************************************************
*                         user_inode_stat                               *
*************************************************************************
*									*
* Given an inode, build the stat structure corresponding to it.		*
*									*
************************************************************************/
int	user_inode_stat(inode_t *ip, struct user_stat *buf)
{
    memset(buf, 0, sizeof(*buf)) ;

    buf->st_dev		= ip->i_dev ;
    buf->st_ino		= ip->i_inum ;
    buf->st_mode	= ip->i_mode ;
    buf->st_nlink	= 1 ;			/* presumably */
    buf->st_rdev	= ip->i_dev ;
    strcpy(buf->st_fstype, "STREAMS") ;
    buf->st_ctime	= ip->i_ctime;
    buf->st_ftime	= ip->i_ftime;
    buf->st_mtime	= ip->i_mtime;
    buf->st_atime	= ip->i_atime;
    return(0) ;

} /* user_inode_stat */

/************************************************************************
*                            user_stat                                  *
*************************************************************************
*									*
* Return status of a streams file.					*
*									*
************************************************************************/
int	user_stat(char *name, struct user_stat *buf)
{
    dir_t	*dp ;
    inode_t	*ip ;
    int		 rslt;

    dp = user_search_dir(name) ;		/* find name in directory */
    if (dp == NULL)
	return(-ENOENT) ;			/* not found */

    ip = user_inode(dp->d_dev) ;		/* ptr to inode */
    if (ip == NULL)				/* out of inodes */
	return(-EMFILE) ;
    if (dp->d_inode == NULL )
	dp->d_inode = ip;
    ip->i_use--;

    rslt = user_inode_stat(ip, buf) ;

    ULOCK_INO(ip) ;

    return(rslt) ;	/* return status */

} /* user_stat */


/************************************************************************
*                            user_fstat                                 *
*************************************************************************
*									*
* Return status on an open file descriptor.				*
*									*
************************************************************************/
int	user_fstat(int fd, struct user_stat *buf)
{
    inode_t	*ip ;
    file_t	*fp = user_file_of_fd(fd) ;

    if (fp == NULL)
	return(-ENXIO) ;			/* no device */

    ip = fp->f_inode ;
    if (ip == NULL || fp->f_count == 0)
	return(-EBADF) ;			/* file not open */

    return(user_inode_stat(ip, buf)) ;		/* return status */

} /* user_fstat */


/************************************************************************
*                          user_print_inode                             *
*************************************************************************
*									*
* Print out the inode structure pointed to by 'ip'.			*
*									*
************************************************************************/
static char	dcf[] = "-fc?d?b?" ;

static char	*rwx[8] = 
		{
		 "---"
		,"--x"
		,"-w-"
		,"-wx"
		,"r--"
		,"r-x"
		,"rw-"
		,"rwx"
		} ;

void	user_print_inode(inode_t *ip)
{
    if (ip == NULL)
    {
	printk("user_print_inode: Inode ptr is NULL\n") ;
	return ;
    }

    printk("\ti_link     = %s\n", (ip->i_link == NULL ? "NULL" : "Non-NULL")) ;
    printk("\ti_mode     = %o %c%s%s%s\n", ip->i_mode,
		dcf[(ip->i_mode >> 12) & 0x07],
		rwx[(ip->i_mode >> 6) & 0x07],
		rwx[(ip->i_mode >> 3) & 0x07],
		rwx[(ip->i_mode     ) & 0x07]) ;

    printk("\ti_dev      = %d,%d\n", major(ip->i_dev), minor(ip->i_dev)) ;
    printk("\ti_str      = %s\n", (ip->i_str == NULL ? "NULL" : "Non-NULL")) ;
    printk("\ti_use      = %d\n", ip->i_use) ;
    printk("\ti_inum     = %d\n", ip->i_inum) ;
    printk("\ti_ctime    = %s", ctime(&ip->i_ctime)) ;
    printk("\ti_mtime    = %s", ctime(&ip->i_mtime)) ;
    printk("\ti_atime    = %s", ctime(&ip->i_atime)) ;
    printk("\ti_ftime    = %s", ctime(&ip->i_ftime)) ;
    printk("\ti_flags    = 0x%x\n", ip->i_flags) ;
#ifdef USER
    {
	struct semaphore	*sem = (struct semaphore *) ip->i_sem.sem_mem ;
	printk("\ti_sem      = %d\n", sem->sem_count) ;
    }
#endif
} /* user_print_inode */

/************************************************************************
*                          user_print_inodes                            *
*************************************************************************
*									*
* Run through the chain of inodes and print them all out.		*
*									*
************************************************************************/
void
user_print_inodes(void)
{
    inode_t	*ip ;

    printk("\n") ;
    for (ip = inodes; ip != NULL; ip = ip->i_link)
    {
	user_print_inode(ip) ;
	printk("\n") ;
    }

} /* user_print_inodes */

/************************************************************************
*                        user_print_dir_entry                           *
*************************************************************************
*									*
* Print out a directory entry according to the passed options.		*
*									*
************************************************************************/
void	user_print_dir_entry(dir_t *dp, int optns)
{
    char	timbuf[50] ;
    int		refcnt ;

    strcpy(timbuf, ctime(&dp->d_mtime)) ;
    timbuf[strlen(timbuf)-1] = 0 ;	/* zot the \n on the end */

    if (dp->d_inode != NULL)
	refcnt = dp->d_inode->i_use ;
    else
	refcnt = dp->d_refcnt ;

    printk("%3d %c%s%s%s %2d %2d,%2d %s  %s\n",
	    dp->d_inum,
	    dcf[(dp->d_mode >> 12) & 0x07],
	    rwx[(dp->d_mode >> 6) & 0x07],
	    rwx[(dp->d_mode >> 3) & 0x07],
	    rwx[(dp->d_mode     ) & 0x07],
	    refcnt,
	    major(dp->d_dev), minor(dp->d_dev),
	    timbuf,
	    dp->d_name) ;

    if (optns & USR_PRNT_INODE)
	user_print_inode(dp->d_inode) ;

} /* user_print_dir_entry */

/************************************************************************
*                         user_print_dir                                *
*************************************************************************
*									*
* Print out the directory.						*
*									*
* If 'name' is non-null, then just print the entry for the name.	*
* if 'name' is null then print the whole directory.			*
*									*
* 'optns' controls how much info to print out about each entry.		*
*									*
************************************************************************/
void	user_print_dir(char *name, int optns)
{
    int		 d ;
    dir_t	*dp ;

    for (d = 0, dp = &directory[0]; d < NDIR; d++, dp++)
    {
	if (   dp->d_name != NULL		/* name valid */
	    && (   name == NULL
		|| strcmp(dp->d_name, name) == 0	/* name match */
	       )
	   )
	    user_print_dir_entry(dp, optns) ;
    }

} /* user_print_dir */

/************************************************************************/

/*
 *  user_pipe()
 */
int user_pipe( unsigned int *fd )
{
    struct file *f0, *f1;
    int error = user_get_pipe( &f0, &f1 );

    if (error < 0)  _return(error);

    fd[0] = f0->f_fdnr;
    fd[1] = f1->f_fdnr;

    return 0;
}

/*
 *  user_isastream()
 *
 *  all of the files in the user file system are STREAMS; hence, we
 *  just return 1 here.
 */
int user_isastream( unsigned int fd )
{
    return 1;
}

/*
 *  user_fattach() -
 *
 *  This will probably remain a stub - fattach doesn't really make
 *  sense here.
 */
int user_fattach( unsigned int fd, const char *path )
{
    _return(-ENOSYS);
}

/*
 *  user_fdetach()
 *
 *  This will probably remain a stub - fdetach doesn't make sense
 *  without fattach.
 */
int user_fdetach( const char *path )
{
    _return(-ENOSYS);
}
