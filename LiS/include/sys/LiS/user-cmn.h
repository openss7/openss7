/************************************************************************
*                      User-level Common Structures                     *
*************************************************************************
*									*
* The defines and data structures in this file are common to all uses	*
* of the user-level file system.  Thus, user-mdep.h, qnx-mdep.h,	*
* sco-mdep.h and sys54-mdep.h all include this file.			*
*									*
* Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>		*
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

#ifndef _USER_CMN_H
#define	_USER_CMN_H		1

#ident "@(#) LiS user-cmn.h 2.11 09/02/04 14:11:57 "

#include <string.h>
#ifndef SYS_LISLOCKS_H
#include <sys/lislocks.h>			/* for lis_semaphore_t */
#endif

#define	inode		u_inode
#define	inode_t		u_inode_t
#define	file		u_file
#define	file_t		u_file_t
#define	file_operations	u_file_operations

#define REFERENCE_INODE	1	/* inode that will not be removed */

typedef struct u_inode
{
    struct u_inode	*i_link ;		/* in case of linked list */
    long		 i_mode ;		/* file mode */
    port_dev_t		 i_dev ;		/* major/minor */
    struct stdata	*i_str ;		/* ptr to stream struct */
    lis_semaphore_t	 i_sem ;		/* inode semaphore */
    int			 i_use ;		/* use count */
    int			 i_inum ;		/* inode number (phoney) */
    long		 i_ctime ;		/* status change time */
    long		 i_mtime ;		/* modified time */
    long		 i_atime ;		/* accessed time */
    long		 i_ftime ;		/* creation time */
    int			 i_flags ;		/* flags field */
} u_inode_t ;

#define i_rdev		i_dev			/* head.c uses i_rdev */
#define i_count		i_use			/* head.c uses i_count */

typedef struct u_file
{
    struct u_file	*f_link ;		/* in case of linked list */
    char		*f_name ;		/* file name */
    int			 f_flags ;		/* file open flags */
    int			 f_mode ;		/* file rd/wr mode */
    int			 f_count ;		/* reference count */
    int			 f_fdnr ;		/* file number */
    struct inode	*f_inode ;		/* inode of file */
    struct file_operations * f_op;		/* handler routines */
    void		*f_drvr_ptr ;		/* driver's pointer */
    lis_semaphore_t	 f_sem ;		/* to lock file struct */
    void		*f_ptr ;		/* ptr for user's use */

} u_file_t ;

#define LOCK_FILE(f)	lis_down(&(f)->f_sem)
#define ULOCK_FILE(f)	lis_up(&(f)->f_sem)

#define FILE_INODE(f)   (f)->f_inode
#define FILE_NAME(f)    (f)->f_name

#define FILE_STR(f)     (struct stdata *)(f)->f_drvr_ptr
#define SET_FILE_STR(f,s)  (f)->f_drvr_ptr = (void *) (s)
#define INODE_STR(i)    (i)->i_str
#define SET_INODE_STR(i,s) (i)->i_str = (void *) (s)

#define	I_COUNT(i)	((i)->i_count)
#define	F_COUNT(f)	((f)->f_count)

struct u_file_operations			/* from linux/fs.h */
{
	int (*lseek) (void);
	ssize_t (*read) (struct file *, char *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char *, size_t, loff_t *);
	int (*readdir) (void);
	int (*select) (void);
	int (*ioctl) (struct inode *, struct file *, unsigned int, unsigned long);

	int (*mmap) (void);

	int (*open) (struct inode *, struct file *);
	int (*release) (struct inode *, struct file *);

	int (*fsync) (void);
	int (*fasync) (void);
	int (*check_media_change) (void);
	int (*revalidate) (void);
	int (*putpmsg)(struct inode *, struct file *, void *, void *, int, int);
	int (*getpmsg)(struct inode *, struct file *, void *, void *, int *, int *, int);
	int (*pollfd)(struct inode *, struct file *, void *);
};

/*
 * Defines for i_mode, compliments of include/linux/stat.h
 */
#ifndef S_IFREG

#define S_IFMT  00170000
#define S_IFSOCK 0140000
#define S_IFLNK	 0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#endif

#ifndef S_ISREG

#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)

#endif

#ifndef S_IRWXU

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#endif

#ifndef S_IRWXG

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#endif

#ifndef S_IRWXO

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

#endif

#ifndef S_IRWXUGO

#define S_IRWXUGO	(S_IRWXU|S_IRWXG|S_IRWXO)
#define S_IALLUGO	(S_ISUID|S_ISGID|S_ISVTX|S_IRWXUGO)
#define S_IRUGO		(S_IRUSR|S_IRGRP|S_IROTH)
#define S_IWUGO		(S_IWUSR|S_IWGRP|S_IWOTH)
#define S_IXUGO		(S_IXUSR|S_IXGRP|S_IXOTH)

#endif

/*
 * Device node support
 */
#define	lis_mknod	user_mknod
#define	lis_unlink	user_unlink


#endif
