#ifndef _USRIO_H_
#define _USRIO_H_
/************************************************************************
*                        User Functions                                 *
*************************************************************************
*									*
* These are file-system-like funcitons defined in head/user/port.c.	*
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

#ident "@(#) LiS usrio.h 2.3 12/26/01 19:51:13 "

#ifndef _SYS_POLL_H
#include <sys/poll.h>
#endif

#ifndef _STRPORT_H
#include <sys/strport.h>		/* pick up inode definition */
#endif

#ifndef NULL
#define	NULL		( (void *) 0 )
#endif

#ifndef _IOW
#define _IOW(x,y,t)     (((((int)sizeof(t))&0xFFFF)<<16)|(x<<8)|y)
#endif

/*
 * If you define this from the outside, PLEASE do it consistently
 * in all compiles involving this header file.
 */
#ifndef MAXNAMLEN
#define MAXNAMLEN	100
#endif

/************************************************************************
*                        Data Structures                                *
************************************************************************/

typedef struct dir			/* directory entry */
{
    char	*d_name ;		/* file name */
    dev_t	 d_dev ;		/* device number major/minor */
    int		 d_mode ;		/* file mode */
    time_t	 d_ftime ;		/* time file was created */
    time_t	 d_ctime ;		/* time file was created */
    time_t	 d_atime ;		/* time file was accessed */
    time_t	 d_mtime ;		/* time file was modified */
    int		 d_refcnt;		/* reference count */
    u_inode_t   *d_inode ;
    int		 d_inum ;

} dir_t ;

typedef struct maj			/* major device entry */
{
    char	*m_name ;		/* device name */
    struct file_operations *m_op ;	/* open, close,... ptrs */

} maj_t ;

#define	NDIR	1024			/* number of directory entries */
#define	NMAJOR	100			/* number of major device numbers */

/************************************************************************
*                      Directory Searching                              *
*************************************************************************
*									*
* The following structures and prototypes are used to define a POSIX-	*
* like directory searching mechanism.  This allows users to search	*
* our internal directory.						*
*									*
************************************************************************/
typedef struct
{
    int		 dd_fd;			/* file descriptor */
    int		 dd_loc;		/* offset in block */
    int		 dd_size;		/* amount of valid data */
    char	*dd_buf;		/* directory block */

} user_DIR ;


/*
 * Returned by readdir routine
 */
typedef struct user_dirent
{
    int			d_ino ;		/* inode number */
    off_t		d_off ;		/* offset of directory entry */
    unsigned short	d_reclen ;	/* length of this record */
    char		d_name[1] ;	/* name of file */

} user_dirent_t ;

extern user_DIR			*user_opendir(char *);
extern struct user_dirent	*user_readdir(user_DIR *);
extern int			 user_closedir(user_DIR *);
extern void			 user_rewinddir(user_DIR *) ;

/************************************************************************
*                          File Status                                  *
*************************************************************************
*									*
* Routines and structures to return the status of a file.		*
*									*
* Almost all of these fields are unused.  They are present just to	*
* give an SVR4 look to the structure.					*
*									*
************************************************************************/

/*
 *  st_atime, st_mtime and st_ctime are macros on some systems.
 *  This will repair.
 */
#ifdef st_atime
#undef st_atime
#endif
#ifdef st_mtime
#undef st_mtime
#endif
#ifdef st_ctime
#undef st_ctime
#endif

typedef struct user_stat
{
    int		st_dev ;	/* device which contains the directory */
    int		st_ino ;	/* inode number of file (phoney) */
    long	st_mode ;	/* mode of file */
    int		st_nlink ;	/* number of links */
    int		st_uid ;	/* owner */
    int		st_gid ;	/* group */
    dev_t	st_rdev ;	/* device if special file (ours are) */
    int		st_size ;	/* file size */
    long	st_atime ;	/* last access time */
    long	st_mtime ;	/* last modification */
    long	st_ctime ;	/* last status change */
    long	st_ftime ;	/* creation change */
    long	st_blksize ;	/* preferred block size */
    long	st_blocks ;	/* number of st_blksize blks allocated */
    char	st_fstype[16];	/* type of file system */
    int		st_aclcnt ;	/* number of ACL entries */
    int		st_level ;	/* MAC level */
    long	st_flags ;	/* general purpose flags */
    int		st_cmwlevel ;	/* future MAC level */

} user_stat_t ;

extern int	user_stat(char *name, struct user_stat *buf) ;
extern int	user_fstat(int fd, struct user_stat *buf) ;


/************************************************************************
*                         fcntl Types                                   *
*************************************************************************
*									*
* The following codes will be defined here for calls to fcntl if they	*
* have not been defined elsewhere first.  These are SVR4 command codes	*
* that are not included in every environment's fcntl.h.			*
*									*
************************************************************************/

#ifndef F_ALLOCSP
#define F_ALLOCSP	10
#endif
#ifndef F_FREESP
#define F_FREESP	11
#endif
#ifndef F_RSETLK
#define F_RSETLK	20
#endif
#ifndef F_RGETLK
#define F_RGETLK	21
#endif
#ifndef F_RSETLKW
#define F_RSETLKW	22
#endif
#ifndef F_GETOWN
#define F_GETOWN	23
#endif
#ifndef F_SETOWN
#define F_SETOWN	24
#endif
#ifndef F_CHKFL
#define F_CHKFL		99			/* completely bogus value */
#endif


/************************************************************************
*                         Printing Options                              *
*************************************************************************
*									*
* Options to control how much the directory print routine prints.	*
*									*
************************************************************************/
#define	USR_PRNT_INODE		0x01	/* print inode structures */
#define	USR_PRNT_FILE		0x02	/* print file structures */

/************************************************************************
*                           Prototypes                                  *
************************************************************************/

#define	user_putmsg(f,c,d,g)	user_putpmsg(f,c,d,-1,g)
#define	user_getmsg(f,c,d,g)	user_getpmsg(f,c,d,NULL,g)

int	user_mknod(char *name, int mode, dev_t dev) ;
int	user_unlink(char *name) ;
int	user_open(char *name, int flags, int mode) ;
int	user_close(int fd) ;
int	user_read(int fd, void *buf, int nbytes) ;
int	user_write(int fd, void *buf, int nbytes) ;
int	user_ioctl(int fd, int cmd, ...) ;
int	user_fcntl(int fd, int cmd, ...) ;
int	user_putpmsg(int fd, struct strbuf *ctlptr,
			     struct strbuf *dataptr,
			     int band,
			     int flags) ;
int	user_getpmsg(int fd, struct strbuf *ctlptr,
			     struct strbuf *dataptr,
			     int *bandp,
			     int *flagsp) ;
int	user_poll(struct pollfd *fds, int nfds, int time_out) ;
int	user_set_ptr(int fd, void *ptr) ;
void	*user_get_ptr(int fd) ;
void	 user_print_dir(char *name, int optns) ;
void	 user_print_inode(inode_t *ip) ;
void	 user_print_inodes(void) ;

int	 user_add_dir_entry(char *name, int mode, dev_t dev) ;
dir_t	*user_search_dir(char *name) ;
dir_t	*user_search_dir_inode(inode_t *i) ;
file_t	*user_file_of_inode(inode_t * i) ;
file_t	*user_alloc_file(void) ;

int user_pipe(unsigned int *);
int user_isastream(unsigned int);
int user_fattach(unsigned int, const char *);
int user_fdetach(const char *);

#endif /* _USRIO_H_ */
