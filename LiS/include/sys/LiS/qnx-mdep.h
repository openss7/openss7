/*
 *    Copyright (C) 1997  Mikel Matthews, Gcom, Inc <mikel@gcom.com>
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
#ifndef _QNX_MDEP_H_
#define _QNX_MDEP_H_

#ident "@(#) LiS qnx-mdep.h 2.2 6/1/99 14:11:09 "

/************************************************************************
*                      QNX Machine Dependencies                         *
*************************************************************************
*									*
* This file contains the missing structure definitions required		*
* to run STREAMS as a QNX driver.					*
*									*
* The file then includes port-mdep.h for the portable version of the	*
* code.									*
*									*
* Author:	Mikel Matthews <mikel@gcom.com>				*
*									*
************************************************************************/

#include <sys/types.h>
#include <errno.h>

#if 0
#include <string.h>	      /* for memcpy */
#endif
#include <signal.h>	      /* for signal numbers */

#if 1
#include <semaphore.h>	      /* QNX */
#endif

#include <sys/termio.h>
#include <sys/io_msg.h>
#include <sys/sys_msg.h>
#include <sys/dev_msg.h>

typedef unsigned long	ulong;
typedef unsigned int	uint;
typedef unsigned short	ushort;

typedef unsigned long	port_dev_t ;		/* device major/minor */
typedef dev_t	        major_t ;		/* device major/minor */
typedef dev_t		minor_t ;		/* device major/minor */

#ifndef bcopy
extern int     bcopy(const char *s, char *d, int n);
#endif
#ifndef memset
extern void *memset( void *__s, int __c, size_t __n );
#endif
#ifndef memcpy
extern void *memcpy( void *__s1, const void *__s2, size_t __n );
#endif

#undef uid 
#undef gid
typedef int     o_uid_t;
typedef int     o_gid_t;
typedef unsigned   char uchar;

typedef struct cred {
	uid_t	cr_uid;			/* effective user id */
	uid_t	cr_ruid;			/* effective user id */
	gid_t	cr_gid;			/* effective group id */
	gid_t	cr_rgid;			/* effective group id */
} cred_t;

#if 0

struct ocb
{
        mode_t           mode;
        int              count;
	int		 strfd;
        int              nonblock ;
        pid_t            pid ;
        pid_t            proxy ;
        int              nbytes ;
        short            ppid,
                         pid_group,
                         rgid,
                         ruid,
                         egid,
                         euid;
	union qnx_msg   *msg ;
        int              bytes_alloc ;
	int		 rec_buf_cnt;
	int		 do_test;
	long		 offset;
	user_DIR	*dir_ptr;
	char		*buf;
};

struct _io_getmsg {
    msg_t       type;
    short int   fd,
                request,
                nbytes;
    long        zero;
    char        data[1];
    } ;

struct _io_getmsg_reply {
    msg_t           status;
    short unsigned  ret_val;
    long            zero[2];
    char            data[1];
    } ;

struct _io_putmsg {
    msg_t       type;
    short int   fd,
                request,
                nbytes;
    long        zero;
    char        data[1];
    } ;

struct _io_putmsg_reply {
    msg_t           status;
    short unsigned  ret_val;
    long            zero[2];
    } ;

struct _io_pollmsg {
    msg_t       type;
    short int   fd,
                request,
                nbytes;
    long        zero;
    char        data[1];
    } ;

struct _io_pollmsg_reply {
    msg_t           status;
    short unsigned  ret_val;
    long            zero[2];
    char            data[1];
    } ;

typedef struct strbuf_proxy {
    int bfrlen;
    int maxlen;
    int len;
    int prior;
    int flags;
    char data[1];
} strbuf_proxy_t;

typedef
struct strioctl_proxy {
    int     pic_cmd;                 /* command */
    int     pic_timout;              /* timeout value */
    int     pic_len;                 /* length of data */
    char    pic_dp[1];                 /* pointer to data */
} strioctl_proxy_t;


typedef
struct poll_proxy {
    ulong     pnfds;                 /* command */
    int       ptimeout;              /* timeout value */
    int       plen;                 /* length of data */
    char      pdata[1];                 /* pointer to data */
} poll_proxy_t;

typedef
struct _io_rsys_init {
    msg_t       type;
} rsys_init_t;

typedef
struct _io_rsys_init_reply {
    msg_t       status;
    pid_t       pid;
} rsys_init_reply_t;


/* union for receiving messages */
union qnx_msg
{
        msg_t                           type ;
        msg_t                           status ;
        struct _sysmsg_hdr              sysmsg ;
        struct _sysmsg_hdr_reply        version_reply ;
        struct _io_open                 open ;
        struct _io_open_reply           open_reply ;
        struct _io_dup                  dup ;
        struct _io_dup_reply            dup_reply ;
        struct _io_flags                flags ;
        struct _io_flags_reply          flags_reply ;
        struct _io_close                close ;
        struct _io_close_reply          close_reply ;
        struct _io_read                 read ;
        struct _io_read_reply           read_reply ;
        struct _io_write                write ;
        struct _io_write_reply          write_reply ;
        struct _io_qioctl               qioctl ;
        struct _io_qioctl_reply         qioctl_reply ;
	struct _io_getmsg 		getmsg ;
	struct _io_getmsg_reply 	getmsg_reply ;
	struct _io_putmsg 		putmsg ;
	struct _io_putmsg_reply 	putmsg_reply ;
	struct _io_pollmsg 		pollmsg ;
	struct _io_pollmsg_reply 	pollmsg_reply ;
	struct _io_rsys_init		rsys_init;
	struct _io_rsys_init_reply	rsys_init_reply;
	struct _io_lseek		seek;
	struct _io_lseek_reply		seek_reply;
	struct _io_readdir		readdir;
	struct _io_readdir_reply	readdir_reply;
	struct _io_rewinddir		rewinddir;
	struct _io_rewinddir_reply	rewinddir_reply;
} ;

#define _IO_GETMSG    0xff20
#define _IO_PUTMSG    0xff21
#define _IO_PEEKMSG   0xff22
#define _IO_POLLMSG   0xff23
#define _IO_RSYS_INIT 0xff24

#define GCOM_NAME "GCOM_FSYS"
#define MSG_CTL  1
#define MSG_DATA 2

#endif

/*
 * name changes for these structures.
 */
#define	file		u_file
#define	file_t		u_file_t
#define	file_operations	u_file_operations

#define MAJOR	major
#define MINOR	minor
#define makedevice(maj,min) makedev(1,(maj), (min))

#if 0
struct new_proc
{
	struct new_proc *next;
	struct new_proc *prev;
	pid_t		 pid;
	pid_t		 clpid;
	char		*stack;
	int		 ref;
} ;

typedef struct new_proc  new_proc_t;
typedef struct new_proc * new_proc_p;
#endif

#if 0
#define	semaphore	u_semaphore
#define	semaphore_t	u_semaphore_t

typedef struct u_semaphore
{
    int			sem_count ;		/* semaphore counter */
    long		sem_xxx[8] ;		/* just a placeholder */

} u_semaphore_t ;

#else

#define	semaphore	u_semaphore
#define	semaphore_t	sem_t			/* QNX semaphore */

typedef struct u_semaphore
{
    sem_t	sem ;				/* QNX semaphore */

} u_semaphore_t ;

#endif

#define EBADMSG		1200
#define ENODATA		1201
#define ENOPKG		1202
#define ENOSR		1203
#define ETIME		1204
#define EPROTO		1205
#define ENOSTR		1199

/*
 * TBD: Turn a virtual memory address into a physical memory address
 */
#define	kvtophys(addr)		(addr)		/* user level fakery */

/*
 * bzero and bcopy
 */
#define	bzero(addr,nbytes)	memset(addr, 0, nbytes)
#define	bcopy(src,dst,n)	memcpy(dst,src,n)


/*
 * This define is used to determine the max amount of data that the copyout
 * routines will send back to the client
 */

#define MAXRECBUFSIZE	30*1024

#ifndef PORTABLE
#define	PORTABLE	1
#endif
/*
 * Defines for i_mode, compliments of include/linux/stat.h
 */
#undef S_IFMT
#undef S_IFSOCK
#undef S_IFLNK
#undef S_IFREG
#undef S_IFBLK
#undef S_IFDIR
#undef S_IFCHR
#undef S_IFIFO
#undef S_ISUID
#undef S_ISGID
#undef S_ISVTX

#undef S_ISLNK
#undef S_ISREG
#undef S_ISDIR
#undef S_ISCHR
#undef S_ISBLK
#undef S_ISFIFO
#undef S_ISSOCK

#undef S_IRWXU
#undef S_IRUSR
#undef S_IWUSR
#undef S_IXUSR

#undef S_IRWXG
#undef S_IRGRP
#undef S_IWGRP
#undef S_IXGRP

#undef S_IRWXO
#undef S_IROTH
#undef S_IWOTH
#undef S_IXOTH

#undef S_IRWXUGO
#undef S_IALLUGO
#undef S_IRUGO
#undef S_IWUGO
#undef S_IXUGO

/*
 * A dummy to support the select structure in stdata_t.
 */
typedef struct lis_select_struct
{
    int		dummy ;

} lis_select_t ;


/*
 * Now include the portable stuff
 */
#include <sys/LiS/user-cmn.h>
#include <sys/LiS/port-mdep.h>

#endif
