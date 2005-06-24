/*****************************************************************************

 @(#) $RCSfile: strtst.c,v $ $Name:  $($Revision: 1.1.1.5.4.8 $) $Date: 2005/06/23 22:05:24 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/06/23 22:05:24 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strtst.c,v $
 Revision 1.1.1.5.4.8  2005/06/23 22:05:24  brian
 - changes to pass _FORTIFY_SOURCE=2 on gcc 4 testing on FC4

 Revision 1.1.1.5.4.7  2005/05/30 20:29:47  brian
 - I do not think that the band backenable test can always pass

 Revision 1.1.1.5.4.6  2005/05/30 19:44:15  brian
 - tried a little harder to test the band test to run consistently

 Revision 1.1.1.5.4.5  2005/05/30 19:07:27  brian
 - The band test did not take into account that there were double queues so I
   placed delay at the half-way read point to allow one queue to backenable the
   other internally and run its service procedure before reading further.  The
   test was failing because all band 2 messages were read while some were
   stacked a queue before and two band 1 messages are on the queue.  Thus a
   band 1 message was read between band 2 messages and the script thought that
   the test was failing.  This is rather synthetic because it uses knowledge of
   the underlying implementation (2 queues instead of 1).

 Revision 1.1.1.5.4.4  2005/05/14 08:35:12  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: strtst.c,v $ $Name:  $($Revision: 1.1.1.5.4.8 $) $Date: 2005/06/23 22:05:24 $"

static char const ident[] = "$RCSfile: strtst.c,v $ $Name:  $($Revision: 1.1.1.5.4.8 $) $Date: 2005/06/23 22:05:24 $";


/*
 * Copyright (C)  1997-2000  David Grothe, Gcom, Inc <dave@gcom.com>
 * Copyright (C)  2000       John A. Boyd Jr.  protologos, LLC
 */

#define	inline			/* make disappear */

#if defined(LINUX) && !defined(DIRECT_USER)
#define _REENTRANT
#define _THREAD_SAFE
#define _XOPEN_SOURCE	500		/* single unix spec */
#define USE_PTHREADS
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <signal.h>
#include <sys/stropts.h>
#include <sys/LiS/loop.h>	/* an odd place for this file */
#include <sys/LiS/minimux.h>	/* an odd place for this file */
#ifndef DIRECT_USER
#include <sys/LiS/mtdrv.h>
#include <pthread.h>
#endif

#include <string.h>
/*
 * #include <ioctl.h> 
 */
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#ifdef LINUX
#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/LiS/linuxio.h>
#else
# if defined(SCO)
# include <prototypes.h>
# include <varargs.h>
# endif
#include <sys/LiS/usrio.h>
#endif

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <sys/LiS/config.h>
#ifndef LIS_HAVE_MAJOR_T
typedef int major_t;
typedef int minor_t;
#endif
#include <sys/sad.h>

#define	ALL_DEBUG_BITS	( ~ ((unsigned long long) 0) )

/************************************************************************
*                      File Names                                       *
************************************************************************/
#ifdef LINUX
#define	LOOP_1		"/dev/loop.1"
#define	LOOP_2		"/dev/loop.2"
#define	LOOP_9		"/dev/loop.9"
#define	LOOP_255	"/dev/loop.255"
#define LOOP_CLONE	"/dev/loop_clone"
#define MUX_CLONE	"/dev/mux_clone"
#define MUX_1		"/dev/minimux.1"
#define MUX_2		"/dev/minimux.2"
#define	NPRINTK		"/dev/printk"
#define	SAD_CLONE	"/dev/sad"
#define CLONE_FIFO      "/dev/fifo"
#define FIFO_0          "/dev/fifo.0"
#define PUTST		"/dev/putst"
#else
#define	LOOP_1		"loop.1"
#define	LOOP_2		"loop.2"
#define	LOOP_9		"loop.9"
#define	LOOP_255	"loop.255"
#define LOOP_CLONE	"loop_clone"
#define MUX_CLONE	"mux_clone"
#define MUX_1		"minimux.1"
#define MUX_2		"minimux.2"
#define	NPRINTK		"printk"
#define	SAD_CLONE	"sad"
#define CLONE_FIFO      "fifo"
#define FIFO_0          "fifo.0"
#define PUTST		"putst"
#endif

/************************************************************************
*                           Storage                                     *
************************************************************************/

struct test_context {
#ifdef USE_PTHREADS
	pthread_t th;
#endif
	int id;
	char		buf[1000] ;		/* general purpose */
	char		ctlbuf[1000] ;		/* control messages */
	char		rdbuf[1000] ;		/* for reading */
	char		rdctlbuf[1000] ;	/* control messages */
	struct str_mlist	mod_names[10] ;
	struct str_list		mod_list;
	struct strbuf	wr_ctl;
	struct strbuf	wr_dta;
	struct strbuf	rd_ctl;
	struct strbuf	rd_dta;
	struct strpeek	pk_str;
};

struct test_context *ctx_list;
struct test_context ctx_none = { id: -1 };

int		printk_fd = -1 ;	/* file descr for printk */

static void init_ctx(struct test_context *ctx)
{
	memset(ctx, 0, sizeof(*ctx));
	ctx->id = (ctx-ctx_list);
	ctx->mod_list = (struct str_list){10, ctx->mod_names} ;
	ctx->wr_ctl = (struct strbuf){0, 0, ctx->ctlbuf} ;
	ctx->wr_dta = (struct strbuf){0, 0, ctx->buf} ;
	ctx->rd_ctl = (struct strbuf){0, 0, ctx->rdctlbuf} ;
	ctx->rd_dta = (struct strbuf){0, 0, ctx->rdbuf} ;
	ctx->pk_str = (struct strpeek){
		{0, 0, ctx->rdctlbuf},	/* ctlbuf */
		{0, 0, ctx->rdbuf},	/* databuf */
		0			/* flags */
	} ;
}

extern void make_nodes(void) ;
extern int  n_read(int fd) ;

extern int	n_read_msgs(int fd) ;		/* forward decl */

/*
 * command-line tunables
 */
int nthreads = 1;
unsigned long long debug_mask = ALL_DEBUG_BITS;
int all_tests = 1;

/************************************************************************
*                           Dummies                                     *
*************************************************************************
*									*
* Dummy routines to satisfy the linker until system calls are		*
* implemented.								*
*									*
************************************************************************/

#ifdef LINUX


long	lis_mem_alloced ;


#endif

/************************************************************************
*                              now                                      *
*************************************************************************
*									*
* Return pointer to current time and date in ASCII.			*
*									*
************************************************************************/
#ifndef DIRECT_USER
static char *now(void)
{
    time_t	tim ;
    char	*p ;
    static char	buf[100] ;

    tim = time(NULL) ;
    strcpy(buf, ctime(&tim)) ;
    p = strrchr(buf, '\n') ;
    if (p != NULL)
	*p = 0 ;
    return(buf) ;
}
#endif

/************************************************************************
*                         msg_to_syslog                                 *
*************************************************************************
*									*
* Print the given message to syslog.					*
*									*
************************************************************************/
#if 0				/* superceded by printk mechanism */
void
msg_to_syslog(char *msg)
{
#ifdef LINUX
    static int		initialized ;

    if (!initialized)
    {
	openlog("strtst", LOG_NDELAY, LOG_SYSLOG) ;
	initialized = 1 ;
    }

    syslog(LOG_WARNING, msg) ;

#else
    (void) msg ;
#endif
} /* msg_to_syslog */
#endif
/************************************************************************
*                           print                                       *
*************************************************************************
*									*
* Like printf only it also arranges to print to syslog for Linux.	*
*									*
************************************************************************/
#undef vsprintf
void
print(char *fmt, ...) __attribute__ ((format(printf, 1, 2)));
void
print(char *fmt, ...)
{
    char	 bfr[2048];
    extern int	 vsprintf (char *, const char *, va_list args);
    va_list	 args;

    va_start (args, fmt);
    vsprintf (bfr+8, fmt, args);
    va_end (args);

    printf("%s", bfr+8) ;				/* write to stdout */
    fflush(stdout) ;

    if (printk_fd >= 0)				/* write to kernel */
    {
	strcpy(bfr, "strtst:") ;
	bfr[7] = ' ' ;
	if (user_write(printk_fd, bfr, strlen(bfr)) < 0)
		perror(__FUNCTION__);
    }

} /* print */

#define tprint(fmt,args...) print("[%02d]" fmt,ctx->id, ## args)

#ifdef USE_PTHREADS
struct test_context * current_ctx(void)
{
	pthread_t handle;
	struct test_context *ctx;

	if(!ctx_list)
		return &ctx_none;
	handle = pthread_self();
	for(ctx = ctx_list; ctx < ctx_list+nthreads; ctx++)
		if(pthread_equal(handle, ctx->th))
			return ctx;
	return &ctx_none;
}
#else
#define current_ctx() (ctx_list)
#endif

/************************************************************************
*                             xit                                       *
*************************************************************************
*									*
* This routine is called to exit the program when a test fails.		*
*									*
************************************************************************/
void	xit(void)
{
    struct test_context *ctx = current_ctx();

    print("\n\n\n");
    tprint("%s", "****************************************************\n");
    tprint("%s", "*                  Test Failed                     *\n");
    tprint("%s", "****************************************************\n\n");

    tprint("%s", "Dump of memory areas in use:\n\n") ;

#ifndef LINUX
    port_print_mem() ;
#endif

#ifdef DIRECT_USER
    print("\n\n\nDirectory listing:\n\n") ;
    user_print_dir(NULL, USR_PRNT_INODE) ;
#endif
    exit(1) ;

} /* xit */

/************************************************************************
*                                FAIL                                   *
*************************************************************************
*									*
* Print an error message and exit program.				*
*									*
************************************************************************/
#ifndef DIRECT_USER
#define STRERROR(errnum) strerror(errno)
#else
#define STRERROR(errnum) strerror(errnum)
#endif

#define ERR(fmt, args...)				\
do {							\
	struct test_context *ctx = current_ctx();	\
	tprint(fmt, ## args);				\
} while(0)

#define FAIL(fmt, args...)				\
do {							\
	struct test_context *ctx = current_ctx();	\
	tprint(fmt, ## args);				\
	tprint("*** TEST FAILED in %s at %s:%d\n",	\
	       __FUNCTION__,__FILE__,__LINE__);		\
	xit();						\
} while(0)

/************************************************************************
*                           register_drivers                            *
*************************************************************************
*									*
* Register the drivers that we are going to use in the test with	*
* streams.								*
*									*
************************************************************************/
void	register_drivers(void)
{
#ifndef LINUX
    port_init() ;			/* stream head init routine */
#endif

} /* register_drivers */


/************************************************************************
*                          set_debug_mask                               *
*************************************************************************
*									*
* Use stream ioctl to set the debug mask for streams.			*
*									*
************************************************************************/
void	set_debug_mask(unsigned long long msk)
{
    int			fd ;
    int			rslt ;
    unsigned long	mask1, mask2;

    /* filter out unwanted debug msgs */
    msk &= debug_mask;
    mask1 = (unsigned long)(msk & 0xFFFFFFFF) ;
    mask2 = (msk >> 32) ;

    fd = user_open(LOOP_1, O_RDWR, 0) ;
    if (fd < 0)
    {
	FAIL("loop.1: %s\n", STRERROR(-fd)) ;
    }

    rslt = user_ioctl(fd, I_LIS_SDBGMSK, mask1) ;
    if (rslt < 0)
    {
	FAIL("loop.1: I_LIS_SDBGMSK: %s\n", STRERROR(-rslt)) ;
    }

    rslt = user_ioctl(fd, I_LIS_SDBGMSK2, mask2) ;
    print("\nSTREAMS debug mask set to 0x%08lx%08lx\n", mask2, mask1) ;

    user_close(fd) ;

} /* set_debug_mask */

/************************************************************************
*                           file_size                                   *
*************************************************************************
*									*
* Return the number of bytes in the file or -1 if file does not exist.	*
*									*
************************************************************************/
off_t
file_size(char *file_name)
{
    struct stat		b ;

    if (stat(file_name, &b) < 0) return(-1) ;

    return(b.st_size) ;

} /* file_size */

/************************************************************************
*                          wait_for_logfile 				*
*************************************************************************
*									*
* Wait for the log files to settle down.  This is useful in Linux	*
* or other testing environments in which the test program is running	*
* in user space and the STREAMS code is running in the kernel.  The	*
* STREAMS code writes out voluminous messages to the system log and	*
* we have to let it catch up from time to time or we lose messages.	*
*									*
************************************************************************/
void
wait_for_logfile(char *msg)
{
    off_t	messages_size ;
    off_t	syslog_size ;
    off_t	new_messages_size = -1 ;
    off_t	new_syslog_size = -1 ;
    int		n ;

    if (msg != NULL)
    {
	print("Wait for log file to stabilize: %s ", msg) ;
	fflush(stdout) ;
    }

    messages_size = file_size("/var/log/messages") ;
    syslog_size = file_size("/var/log/syslog") ;

    for (n = 0;;n++)
    {
	sleep(1) ;
	printf(".") ; fflush(stdout) ;
	new_messages_size = file_size("/var/log/messages") ;
	new_syslog_size = file_size("/var/log/syslog") ;
	if (   new_messages_size == messages_size
	    && new_syslog_size   == syslog_size
	   )
	    break ;

	messages_size = new_messages_size ;
	syslog_size   = new_syslog_size ;
    }

    if (n)
	sync() ;			/* ensure log file integrity */

    if (msg != NULL)
	print("\n") ;

} /* wait_for_logfile */

/************************************************************************
*                             print_mem                                 *
*************************************************************************
*									*
* Issue streams ioctl to print out allocated memory.			*
*									*
************************************************************************/
void	print_mem(void)
{
    int		fd ;
    int		rslt ;

    fd = user_open(LOOP_1, O_RDWR, 0) ;
    if (fd < 0)
    {
	FAIL("loop.1: %s\n", STRERROR(-fd)) ;
    }

    print("\n\nBegin dump of in-use memory areas\n\n") ;
    rslt = user_ioctl(fd, I_LIS_PRNTMEM, 0) ;
    if (rslt < 0)
    {
	FAIL("loop.1: I_LIS_PRNTMEM: %s\n", STRERROR(-rslt)) ;
    }

    print("\n\nEnd dump of in-use memory areas\n\n") ;
    user_close(fd) ;

} /* print_mem */

/************************************************************************
*                              print_stream                             *
*************************************************************************
*									*
* Issue the print-stream ioctl on the file.				*
*									*
************************************************************************/
void	print_stream(int fd)
{
    int		rslt;

    rslt = user_ioctl(fd, I_LIS_PRNTSTRM, 0) ;
    if (rslt < 0)
    {
	FAIL("I_LIS_PRNTSTRM: %s\n", STRERROR(-rslt)) ;
    }

} /* print_stream */

/************************************************************************
*                           nread_wait                                  *
*************************************************************************
*									*
* Spin on "fd" for a few iterations hoping for at least "n" bytes of	*
* data to show up at the queue head.					*
*									*
************************************************************************/
int	nread_wait(int fd, int n)
{
    int w = 0 ;
    int rslt ;

    do
    {
	rslt = n_read(fd) ;
	if (rslt < 0) FAIL("%s", "n_read\n");
	if (w > 0) sleep(1) ;

    } while (rslt < n && w++ < 5) ;

    return(rslt) ;
}

/************************************************************************
*                          nread_wait_msgs                              *
*************************************************************************
*									*
* Wait for 'n' messages to show up at the stream head.			*
*									*
************************************************************************/
int	nread_wait_msgs(int fd, int n)
{
    int w = 0 ;
    int rslt ;

    do
    {
	rslt = n_read_msgs(fd) ;
	if (rslt < 0) FAIL("%s", "n_read_msgs\n") ;
	if (w > 0) sleep(1) ;

    } while (rslt < n && w++ < 5) ;

    return(rslt) ;
}

/************************************************************************
*                           flush_wait                                  *
*************************************************************************
*									*
* Spin on "fd" for a few iterations hoping for the number of bytes	*
* at the queue head to go to zero.					*
*									*
************************************************************************/
int	flush_wait(int fd)
{
    int w = 0 ;
    int rslt ;

    do
    {
	rslt = n_read(fd) ;
	if (rslt < 0) FAIL("%s", "n_read\n") ;

    } while (rslt > 0 && w++ < 50) ;

    return(rslt) ;
}


/************************************************************************
*                         open_close_test                               *
*************************************************************************
*									*
* Repeatedly open and close the same stream.  This ensures that streams	*
* is not accumulating resources.					*
*									*
************************************************************************/
void	open_close_test(struct test_context *ctx)
{
    int		fd1 ;
    int		fd2 ;
    int		fd3 ;
    int		rslt ;
    int		i ;
    struct strioctl	ioc ;

    /*
     * Make this loop iteration large to wring out memory
     * leaks connected with open/close.
     */
    for (i = 1; i <= 1; i++)
    {
	tprint("open_close_test iteration #%d\n", i) ;

	fd1 = user_open(LOOP_1, O_RDWR, 0) ;
	if (fd1 < 0)
	{
	    FAIL("loop.1: %s\n", STRERROR(-fd1)) ;
	    break ;
	}

	fd2 = user_open(LOOP_2, O_RDWR, 0) ;
	if (fd2 < 0)
	{
	    FAIL("loop.2: %s\n", STRERROR(-fd2)) ;
	    break ;
	}

	fd3 = user_open(LOOP_1, O_RDWR, 0) ;
	if (fd3 < 0)
	{
	    FAIL("loop.1 (second open): %s\n", STRERROR(-fd3)) ;
	    break ;
	}

	rslt = user_open(LOOP_255, O_RDWR, 0) ;
	if (rslt >= 0)			/* this is supposed to fail */
	{
	    tprint("%s", "loop.255 succeeded, but should have failed\n") ;
	    user_close(rslt) ;
	    break ;
	}

	ioc.ic_timout	  = 10 ;
	ioc.ic_dp	  = NULL;
	ioc.ic_cmd 	  = LOOP_DENY_OPEN ;
	ioc.ic_len	  = 0 ;
	rslt = user_ioctl(fd2, I_STR, &ioc) ;
	if (rslt < 0)
	{
	    FAIL("loop.2: ioctl LOOP_DENY_OPEN: %s\n", STRERROR(-rslt)) ;
	    break ;
	}

	rslt = user_open(LOOP_2, O_RDWR, 0) ;
	if (rslt >= 0)			/* this is supposed to fail */
	{
	    tprint("%s", "loop.2 succeeded, but should have failed\n") ;
	    user_close(rslt) ;
	    break ;
	}

	user_close(fd1) ;
	user_close(fd2) ;
	user_close(fd3) ;
    }

} /* open_close_test */

/************************************************************************
*                            open_files                                 *
*************************************************************************
*									*
* Standard prologue to various tests.  Open the loop driver and		*
* connect the two streams together with an ioctl.			*
*									*
************************************************************************/
int	open_files(int *fd1, int *fd2)
{
    int			arg ;
    int			rslt ;
    struct strioctl	ioc ;

    *fd1 = user_open(LOOP_1, O_RDWR, 0) ;
    if (*fd1 < 0)
    {
	ERR("loop.1: %s\n", STRERROR(-*fd1)) ;
	return(*fd1) ;
    }

    *fd2 = user_open(LOOP_2, O_RDWR, 0) ;
    if (*fd2 < 0)
    {
	ERR("loop.2: %s\n", STRERROR(-*fd2)) ;
	user_close(*fd1) ;
	return(*fd2) ;
    }

    ioc.ic_cmd 	  = LOOP_SET ;
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;

    arg = 2 ;
    rslt = user_ioctl(*fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	ERR("loop.1: ioctl LOOP_SET: %s\n", STRERROR(-rslt)) ;
	return(rslt) ;
    }

    return(1) ;

} /* open_files */

/************************************************************************
*                            open_clones                                *
*************************************************************************
*									*
* Open the loop driver as clone devices.				*
*									*
************************************************************************/
int	open_clones(int *fd1, int *fd2)
{
    int			arg ;
    int			rslt ;
    struct strioctl	ioc ;

    *fd1 = user_open(LOOP_CLONE, O_RDWR, 0) ;
    if (*fd1 < 0)
    {
	FAIL("loop_clone.1: %s\n", STRERROR(-*fd1)) ;
    }

    *fd2 = user_open(LOOP_CLONE, O_RDWR, 0) ;
    if (*fd2 < 0)
    {
	FAIL("loop_clone.2: %s\n", STRERROR(-*fd2)) ;
    }

    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;

    ioc.ic_cmd 	  = LOOP_GET_DEV ;
    arg = -1 ;
    rslt = user_ioctl(*fd2, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.2: ioctl LOOP_GET_DEV: %s\n", STRERROR(-rslt)) ;
    }

    if (arg < 0)
    {
	FAIL("loop_clone.2: ioctl LOOP_GET_DEV returned %d\n", arg) ; /* missing ? */
    }

    ioc.ic_cmd 	  = LOOP_SET ;
    ioc.ic_len	  = sizeof(int) ;
    rslt = user_ioctl(*fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.1: ioctl LOOP_SET: %s\n", STRERROR(-rslt)) ;
    }

    return(1) ;

} /* open_clones */

/************************************************************************
*                           n_read                                      *
*************************************************************************
*									*
* Do an I_NREAD on the file.  Return negative if error, or the byte	*
* count if successful.							*
*									*
************************************************************************/
int	n_read(int fd)
{
    int		rslt ;
    int		arg ;

    rslt = user_ioctl(fd, I_NREAD, &arg) ;
    if (rslt < 0)
    {
	ERR("I_NREAD: %s\n", STRERROR(-rslt)) ;
	return(rslt) ;
    }

    return(arg) ;

} /* n_read */

/************************************************************************
*                            n_read_msgs                                *
*************************************************************************
*									*
* Do an I_NREAD and return the number of messages queued.		*
*									*
************************************************************************/
int	n_read_msgs(int fd)
{
    int		rslt ;
    int		arg ;

    rslt = user_ioctl(fd, I_NREAD, &arg) ;
    if (rslt < 0)
	ERR("I_NREAD: %s\n", STRERROR(-rslt)) ;

    return(rslt) ;

} /* n_read_msgs */

/************************************************************************
*                             write_data                                *
*************************************************************************
*									*
* Write data and check results.						*
*									*
************************************************************************/
int	write_data(int fd, char *bfr, int cnt)
{
    int		rslt;

    rslt = user_write(fd, bfr, cnt) ;
    if (rslt < 0)
	ERR("write_data: %s\n", STRERROR(-rslt)) ;
    else
    if (rslt != cnt)
	print("write_data: write returned %d, expected %d\n", rslt, cnt) ;

    return(rslt) ;

} /* write_data */

/************************************************************************
*                             put_msg                                   *
*************************************************************************
*									*
* An interface routine to putpmsg().  This one checks return conditions.*
*									*
************************************************************************/
int	put_msg(int fd, struct strbuf *ctlptr,
			struct strbuf *dataptr,
			int band,
			int flags)
{
    int		rslt ;

    rslt = user_putpmsg(fd, ctlptr, dataptr, band, flags) ;
    if (rslt < 0)
	ERR("put_msg: %s\n", STRERROR(-rslt)) ;
    else
    if (rslt > 0)
	print("put_msg: putpmsg returned %d, expected <= 0\n", rslt) ;

    return(rslt) ;

} /* put_msg */

/************************************************************************
*                           input_sig                                   *
*************************************************************************
*									*
* This is the signal catcher for the S_INPUT ioctl.  SIGPOLL is the	*
* signal that gets caused.						*
*									*
************************************************************************/
void input_sig(int signo)
{
    print("\ninput_sig:  SIGPOLL (%d) caught\n", signo) ;

} /* input_sig */


/************************************************************************
*                             ioctl_test                                *
*************************************************************************
*									*
* Send an ioctl to the "loop" driver to cross-connect the two streams.	*
*									*
************************************************************************/
void	ioctl_test(struct test_context *ctx)
{
    int			i ;
    int			fd1 ;
    int			fd2 ;
    int			arg ;
    int			rslt ;
    int			lgth ;
    int			lgth2 ;
    struct strioctl	ioc ;
    struct str_mlist	*mlp ;
    loop_xparent_t	xp ;

    /*
     * Make this loop iteration large to wring out memory
     * leaks connected with ioctl
     */
    for (i = 1; i <= 1; i++)
    {
	tprint("ioctl_test iteration #%d\n", i) ;

		/********************************
		*           Open Files          * 
		********************************/

	fd1 = user_open(LOOP_1, O_RDWR, 0) ;
	if (fd1 < 0)
	{
	    FAIL("loop.1: %s\n", STRERROR(-fd1)) ;
	}

	fd2 = user_open(LOOP_2, O_RDWR, 0) ;
	if (fd2 < 0)
	{
	    FAIL("loop.2: %s\n", STRERROR(-fd2)) ;
	}

		/********************************
		*             I_STR             * 
		********************************/

	ioc.ic_cmd 	  = LOOP_SET ;
	ioc.ic_timout 	  = 10 ;
	ioc.ic_len	  = sizeof(int) ;
	ioc.ic_dp	  = (char *) &arg ;

	arg = 2 ;
	rslt = user_ioctl(fd1, I_STR, &ioc) ;
	if (rslt < 0)
	{
	    FAIL("loop.1: ioctl LOOP_SET: %s\n", STRERROR(-rslt)) ;
	}

	/*
	 * This next one will fail with device busy.  It is rejected
	 * by the loop driver because the previous one set up both
	 * sides of the loopback connection.
	 */
	arg = 1 ;
	ioc.ic_len	 = sizeof(int) ;
	rslt = user_ioctl(fd2, I_STR, &ioc) ;
	if (rslt < 0)
	{
	    ERR("loop.2: ioctl returned expected error: %s\n",
		  STRERROR(-rslt)) ;
	}
	else
	    tprint("%s", "loop.2: ioctl returned unexpected success, "
	          "should fail with EBUSY\n") ;

	user_close(fd1) ;
	user_close(fd2) ;


	fd1 = user_open(LOOP_1, O_RDWR, 0) ;
	if (fd1 < 0)
	{
	    FAIL("loop.1: %s\n", STRERROR(-fd1)) ;
	}

		/********************************
		*             I_PUSH            * 
		********************************/

	tprint("%s", "Testing I_PUSH (relay)\n") ;
	rslt = user_ioctl(fd1, I_PUSH, "relay") ;
	if (rslt < 0)
	{
	    FAIL("loop.1: I_PUSH (relay): %s\n", STRERROR(-rslt)) ;
	}

	print_mem() ;

		/********************************
		*             I_LOOK            * 
		********************************/

	tprint("%s", "Testing I_LOOK\n") ;
	strcpy(ctx->buf,"Nothing at all") ;
	rslt = user_ioctl(fd1, I_LOOK, ctx->buf) ;
	if (rslt < 0)
	{
	    FAIL("loop.1: I_LOOK: %s\n", STRERROR(-fd1)) ;
	}

	tprint("I_LOOK returned \"%s\"\n", ctx->buf) ;
	print_mem() ;

		/********************************
		*             I_PUSH            * 
		********************************/

	tprint("%s", "Testing I_PUSH (relay2)\n") ;
	rslt = user_ioctl(fd1, I_PUSH, "relay2") ;
	if (rslt < 0)
	{
	    FAIL("loop.1: I_PUSH (relay2): %s\n", STRERROR(-rslt)) ;
	}

	print_stream(fd1) ;
	print_mem() ;

		/********************************
		*             I_FIND            * 
		********************************/

	tprint("%s", "Testing I_FIND\n") ;
	rslt = user_ioctl(fd1, I_FIND, "relay") ;
	if (rslt < 0)
	{
	    FAIL("loop.1: I_FIND: %s\n", STRERROR(-rslt)) ;
	}

	if (rslt > 0)	tprint("%s", "Module \"relay\" is present in the stream\n");
	else		tprint("%s", "Module \"relay\" is not present in the stream\n");

	print_mem() ;

		/********************************
		*         Read/Write            * 
		********************************/

	fd2 = user_open(LOOP_2, O_RDWR, 0) ;
	if (fd2 < 0)
	{
	    FAIL("loop.2: %s\n", STRERROR(-fd2)) ;
	}

	ioc.ic_cmd 	  = LOOP_SET ;
	ioc.ic_timout 	  = 10 ;
	ioc.ic_len	  = sizeof(int) ;
	ioc.ic_dp	  = (char *) &arg ;

	arg = 2 ;
	rslt = user_ioctl(fd1, I_STR, &ioc) ;
	if (rslt < 0)
	{
	    FAIL("loop.1: ioctl LOOP_SET: %s\n", STRERROR(-rslt)) ;
	}

	tprint("%s", "Testing read and write\n") ;
	strcpy(ctx->buf, "Data to send down the file") ;
	rslt = write_data(fd1, ctx->buf, lgth = strlen(ctx->buf)) ;
	if (rslt < 0)
	    FAIL("%s", "write_data\n") ;

	/*
	 * Streams read will wait until the entire count is
	 * exhausted as the default.  Later we will test changing
	 * this option.  For now, just read what was written.
	 */
	ctx->rdbuf[0] = 0 ;
	rslt = user_read(fd2, ctx->rdbuf, lgth);
	if (rslt < 0)
	{
	    FAIL("loop.2: read: %s\n", STRERROR(-rslt)) ;
	}

	if (rslt != lgth)
	{
	    FAIL("loop.2:  read returned %d, expected %d\n", rslt, lgth) ;
	}

	if (strcmp(ctx->buf, ctx->rdbuf))
	{
	    tprint("%s", "loop.2: read: buffer compare error\n") ;
	    tprint("              wrote \"%s\"\n", ctx->buf) ;
	    tprint("              read  \"%s\"\n", ctx->rdbuf) ;
	}
	else
	    tprint("loop.2: read %d bytes: buffer compared OK\n", rslt) ;

	for (i = 1; i <= 20; i++)
	    strcat(ctx->buf, " Add more data to make the message longer. ") ;

	lgth = strlen(ctx->buf) ;
	tprint("Write %d bytes and read back\n", lgth) ;
	rslt = write_data(fd1, ctx->buf, lgth) ;
	if (rslt < 0)
	    FAIL("%s", "write_data\n") ;

	/*
	 * Streams read will wait until the entire count is
	 * exhausted as the default.  Later we will test changing
	 * this option.  For now, just read what was written.
	 */
	ctx->rdbuf[0] = 0 ;
	rslt = user_read(fd2, ctx->rdbuf, lgth);
	if (rslt < 0)
	{
	    FAIL("loop.2: read: %s\n", STRERROR(-rslt)) ;
	}

	if (rslt != lgth)
	{
	    FAIL("loop.2:  read returned %d, expected %d\n", rslt, lgth) ;
	}

	if (strcmp(ctx->buf, ctx->rdbuf))
	{
	    tprint("%s", "loop.2: read: buffer compare error\n") ;
	    tprint("              wrote \"%s\"\n", ctx->buf) ;
	    tprint("              read  \"%s\"\n", ctx->rdbuf) ;
	}
	else
	    tprint("loop.2: read %d bytes: buffer compared OK\n", rslt) ;


		/********************************
		*             I_NREAD           * 
		********************************/

	tprint("%s", "Testing I_NREAD\n") ;
	/*
	 * Write two blocks into the file.
	 */
	strcpy(ctx->buf, "Data to send down the file") ;
	rslt = write_data(fd1, ctx->buf, lgth = strlen(ctx->buf)) ;
	if (rslt < 0)
	    FAIL("%s", "write_data\n") ;

	strcpy(ctx->buf, "More data to send down the file") ;
	rslt = write_data(fd1, ctx->buf, lgth2 = strlen(ctx->buf)) ;
	if (rslt < 0)
	    FAIL("%s", "write_data\n") ;

	rslt = nread_wait(fd2, lgth) ;
	if (rslt < 0)
	{
	    print_mem() ;
	    FAIL("%s", "n_read\n") ;
	}

	if (rslt != lgth)		/* just the 1st  msg */
	{
	    print_mem() ;
	    FAIL("loop.2:  I_NREAD returned %d, expected %d\n", rslt, lgth) ;
	}
	else
	    tprint("loop.2: I_NREAD returned %d, OK\n", rslt) ;

	memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;
	rslt = user_read(fd2, ctx->rdbuf, lgth);
	if (rslt < 0)
	{
	    FAIL("loop.2: read: %s\n", STRERROR(-rslt)) ;
	}

	if (rslt != lgth)
	{
	    FAIL("loop.2:  read returned %d, expected %d\n", rslt, lgth) ;
	}


		/********************************
		*             I_FLUSH           * 
		********************************/

	tprint("%s", "Testing I_FLUSH\n") ;

	/*
	 * One message left unread from previous test.
	 */
	strcpy(ctx->buf, "More data to send down the file") ;
	rslt = write_data(fd1, ctx->buf, lgth = strlen(ctx->buf)) ;
	if (rslt < 0)
	    FAIL("%s", "write_data\n") ;

	rslt = write_data(fd1, ctx->buf, lgth) ;
	if (rslt < 0)
	    FAIL("%s", "write_data\n") ;

	rslt = nread_wait_msgs(fd2, 3);
	if (rslt != 3)
	{
	    FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 3) ;
	}

	rslt = user_ioctl(fd2, I_FLUSH, FLUSHRW) ;
	if (rslt < 0)
	{
	    FAIL("loop.2: I_FLUSH: %s\n", STRERROR(-rslt)) ;
	}

	rslt = flush_wait(fd2) ;
	if (rslt != 0)
	{
	    FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 0) ;
	}

	tprint("%s", "\nTesting LOOP_FLUSH\n") ;

	rslt = write_data(fd1, ctx->buf, lgth) ;
	if (rslt < 0)
	    xit() ;

	rslt = nread_wait_msgs(fd2, 1) ;
	if (rslt != 1)
	{
	    FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 1) ;
	}

	strcpy(ctx->buf, "Data following flush") ;
	ioc.ic_timout	  = 10 ;
	ioc.ic_dp	  = ctx->buf;
	ioc.ic_cmd 	  = LOOP_FLUSH ;
	ioc.ic_len	  = strlen(ctx->buf) ;
	rslt = user_ioctl(fd1, I_STR, &ioc) ;
	if (rslt < 0)
	{
	    print("loop_clone.1: ioctl LOOP_FLUSH: %s\n", STRERROR(-rslt)) ;
	}

	memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;
	rslt = user_read(fd2, ctx->rdbuf, ioc.ic_len);
	if (rslt < 0)
	{
	    print("loop.2: read: %s\n", STRERROR(-rslt)) ;
	    xit() ;
	}

	if (rslt != ioc.ic_len)
	{
	    print("loop.2:  read returned %d, expected %d\n", rslt, ioc.ic_len);
	    xit() ;
	}

	if (strcmp(ctx->buf, ctx->rdbuf))
	{
	    FAIL("loop.2:  Expected: %s\n              Got: %s\n", ctx->buf, ctx->rdbuf);
	}

	tprint("%s", "LOOP_FLUSH test succeeded\n") ;


		/********************************
		*           I_CANPUT            * 
		********************************/

	tprint("%s", "Testing I_CANPUT\n") ;
	rslt = user_ioctl(fd2, I_CANPUT, 0) ;
	if (rslt < 0)
	{
	    FAIL("loop.2: I_CANPUT: %s\n", STRERROR(-rslt)) ;
	}

	tprint("loop.2: I_CANPUT returned %d\n", rslt) ;

		/********************************
		*           I_SETCLTIME         * 
		********************************/

	tprint("%s", "Testing I_SETCLTIME\n") ;
	rslt = user_ioctl(fd2, I_SETCLTIME, 50) ;
	if (rslt < 0)
	{
	    FAIL("loop.2: I_SETCLTIME: %s\n", STRERROR(-rslt)) ;
	}

	tprint("loop.2: I_SETCLTIME returned %d\n", rslt) ;

		/********************************
		*           I_GETCLTIME         * 
		********************************/

	tprint("%s", "Testing I_GETCLTIME\n") ;
	arg = 0 ;
	rslt = user_ioctl(fd2, I_GETCLTIME, &arg) ;
	if (rslt < 0)
	{
	    FAIL("loop.2: I_GETCLTIME: %s\n", STRERROR(-rslt)) ;
	}

	if (arg != 50)
	    tprint("loop.2: I_GETCLTIME returned %d, expected %d\n", arg, 50) ;
	else
	    tprint("loop.2: I_GETCLTIME returned %d\n", arg) ;

		/********************************
		*             I_LIST            * 
		********************************/

	tprint("%s", "Testing I_LIST\n") ;
	rslt = user_ioctl(fd1, I_LIST, NULL) ;
	if (rslt < 0)
	{
	    FAIL("loop.1: I_LIST: %s\n", STRERROR(-rslt)) ;
	}
	tprint("I_LIST(loop.1, NULL) = %d\n", rslt) ;
	lgth = rslt ;			/* length of list */

	rslt = user_ioctl(fd2, I_LIST, NULL) ;
	if (rslt < 0)
	{
	    FAIL("loop.2: I_LIST: %s\n", STRERROR(-rslt)) ;
	}
	tprint("I_LIST(loop.1, NULL) = %d\n", rslt) ;
	lgth2 = rslt ;			/* length of list */

	ctx->mod_list.sl_nmods = sizeof(ctx->mod_names)/sizeof(ctx->mod_names[0]);
	rslt = user_ioctl(fd1, I_LIST, &ctx->mod_list) ;
	if (rslt < 0)
	{
	    FAIL("loop.1: I_LIST: %s\n", STRERROR(-rslt)) ;
	}
	if (rslt > 0)
	{
	    tprint("%s", "loop.1: I_LIST: Returned positive value\n") ;
	}
	if (ctx->mod_list.sl_nmods != lgth)
	{
	    FAIL("loop.1: I_LIST returned %d modules, expected %d \n",
		    ctx->mod_list.sl_nmods, lgth) ;
	}

	for (mlp = ctx->mod_names; ctx->mod_list.sl_nmods > 0;
	     ctx->mod_list.sl_nmods--, mlp++)
	{
	    tprint("loop.1 module[%d] is \"%s\"\n",
		  lgth-ctx->mod_list.sl_nmods, mlp->l_name) ;
	}

	ctx->mod_list.sl_nmods = sizeof(ctx->mod_names)/sizeof(ctx->mod_names[0]);
	rslt = user_ioctl(fd2, I_LIST, &ctx->mod_list) ;
	if (rslt < 0)
	{
	    FAIL("loop.2: I_LIST: %s\n", STRERROR(-rslt)) ;
	}
	if (rslt > 0)
	{
	    FAIL("%s", "loop.2: I_LIST: Returned positive value\n") ;
	}
	if (ctx->mod_list.sl_nmods != lgth2)
	{
	    FAIL("loop.2: I_LIST returned %d modules, expected %d \n",
		    ctx->mod_list.sl_nmods, lgth2) ;
	}

	for (mlp = ctx->mod_names; ctx->mod_list.sl_nmods > 0;
	     ctx->mod_list.sl_nmods--, mlp++)
	{
	    tprint("loop.2 module[%d] is \"%s\"\n",
		  lgth2-ctx->mod_list.sl_nmods, mlp->l_name) ;
	}

		/********************************
		*           I_ATMARK            *
		********************************/

	tprint("%s", "Testing I_ATMARK\n") ;

	/*
	 * Because of writing into one stream and checking for the marked
	 * messages at the other, we need to ensure that the message gets
	 * forwarded right away.  On a 2 CPU system we can execute the ATMARK
	 * ioctl before the message gets to the other stream via the service
	 * queue.
	 */
	ioc.ic_timout	  = 10 ;
	ioc.ic_dp	  = NULL;
	ioc.ic_cmd 	  = LOOP_PUTNXT ;  /* use putnext rather then svcq */
	ioc.ic_len	  = 0 ;
	rslt = user_ioctl(fd1, I_STR, &ioc) ;
	if (rslt < 0)
	{
	    FAIL("loop_clone.1: ioctl LOOP_PUTNXT: %s\n", STRERROR(-rslt)) ;
	}

	strcpy(ctx->buf, "Data to send down the file for testing I_ATMARK") ;
	lgth = strlen(ctx->buf) ;

	rslt = user_ioctl(fd2, I_ATMARK, ANYMARK) ;
	tprint("%s", "loop.2: I_ATMARK w/no messages: ") ;
	if (rslt < 0)
	{
	    FAIL("%s\n", STRERROR(-rslt)) ;
	}

	tprint("%s", "OK\n") ;

	rslt = write_data(fd1, ctx->buf, lgth) ;
	if (rslt < 0)
	    FAIL("%s", "write_data\n") ;

	if (nread_wait_msgs(fd2, 1) != 1)
	{
	    FAIL("%s", "loop.2: message failed to appear\n") ;
	}

	rslt = user_ioctl(fd2, I_ATMARK, ANYMARK) ;
	tprint("%s", "loop.2: I_ATMARK w/non-marked message: ") ;
	if (rslt < 0)
	{
	    FAIL("%s\n", STRERROR(-rslt)) ;
	}

	tprint("%s", "OK\n") ;

	rslt = user_ioctl(fd2, I_FLUSH, FLUSHRW) ;
	if (rslt < 0)
	{
	    FAIL("loop.2: I_FLUSH: %s\n", STRERROR(-rslt)) ;
	}

	rslt = flush_wait(fd2);

	ioc.ic_cmd 	  = LOOP_MARK ;		/* mark the next msg */
	ioc.ic_len	  = 0 ;
	ioc.ic_dp	  = NULL ;

	rslt = user_ioctl(fd1, I_STR, &ioc) ;
	if (rslt < 0)
	{
	    FAIL("loop.1: ioctl LOOP_MARK: %s\n", STRERROR(-rslt)) ;
	}

	rslt = write_data(fd1, ctx->buf, lgth) ;
	if (rslt < 0)
	    FAIL("%s", "write_data\n") ;

	if (nread_wait_msgs(fd2, 1) != 1)
	{
	    FAIL("%s", "loop.2: message failed to appear\n") ;
	}
	rslt = user_ioctl(fd2, I_ATMARK, ANYMARK) ;
	tprint("%s", "loop.2: I_ATMARK(ANYMARK) w/marked message: ") ;
	if (rslt < 0)
	{
	    FAIL("%s\n", STRERROR(-rslt)) ;
	}

	if (rslt == 1)
	    tprint("%s", "OK\n") ;
	else
	{
	    FAIL("returned %d, expected 1\n", rslt) ;
	}

	rslt = user_ioctl(fd2, I_ATMARK, LASTMARK) ;
	tprint("%s", "loop.2: I_ATMARK(LASTMARK) w/marked message last: ") ;
	if (rslt < 0)
	{
	    FAIL("%s\n", STRERROR(-rslt)) ;
	}

	if (rslt == 1)
	    tprint("%s", "OK\n") ;
	else
	{
	    FAIL("returned %d, expected 1\n", rslt) ;
	}

	rslt = write_data(fd1, ctx->buf, lgth) ;	/* non-marked msg */
	if (rslt < 0)
	    FAIL("%s", "write_data\n") ;

	if (nread_wait_msgs(fd2, 2) != 2)
	{
	    FAIL("%s", "loop.2: messages failed to appear\n") ;
	}
	rslt = user_ioctl(fd2, I_ATMARK, ANYMARK) ;
	tprint("%s", "loop.2: I_ATMARK(ANYMARK) w/marked message: ") ;
	if (rslt < 0)
	{
	    FAIL("%s\n", STRERROR(-rslt)) ;
	}

	if (rslt == 1)
	    tprint("%s", "OK\n") ;
	else
	{
	    FAIL("returned %d, expected 1\n", rslt) ;
	}

	rslt = user_ioctl(fd2, I_ATMARK, LASTMARK) ;
	tprint("%s", "loop.2: I_ATMARK(LASTMARK) w/marked message last: ") ;
	if (rslt < 0)
	{
	    FAIL("%s\n", STRERROR(-rslt)) ;
	}

	if (rslt == 1)
	    tprint("%s", "OK\n") ;
	else
	{
	    FAIL("returned %d, expected 1\n", rslt) ;
	}

	ioc.ic_len	  = 0 ;
	rslt = user_ioctl(fd1, I_STR, &ioc) ;		/* mark nxt msg */
	if (rslt < 0)
	{
	    FAIL("loop.1: ioctl LOOP_MARK: %s\n", STRERROR(-rslt)) ;
	}

	rslt = write_data(fd1, ctx->buf, lgth) ;
	if (rslt < 0)
	    FAIL("%s", "write_data\n") ;

	if (nread_wait_msgs(fd2, 3) != 3)
	{
	    FAIL("%s", "loop.2: messages failed to appear\n") ;
	}
	rslt = user_ioctl(fd2, I_ATMARK, ANYMARK) ;
	tprint("%s", "loop.2: I_ATMARK(ANYMARK) w/marked message: ") ;
	if (rslt < 0)
	{
	    FAIL("%s\n", STRERROR(-rslt)) ;
	}

	if (rslt == 1)
	    tprint("%s", "OK\n") ;
	else
	{
	    FAIL("returned %d, expected 1\n", rslt) ;
	}

	rslt = user_ioctl(fd2, I_ATMARK, LASTMARK) ;
	tprint("%s", "loop.2: I_ATMARK(LASTMARK) w/marked message not last: ") ;
	if (rslt < 0)
	{
	    FAIL("%s\n", STRERROR(-rslt)) ;
	}

	if (rslt == 0)
	    tprint("%s", "OK\n") ;
	else
	{
	    FAIL("returned %d, expected 1\n", rslt) ;
	}

	rslt = user_ioctl(fd2, I_FLUSH, FLUSHRW) ;
	if (rslt < 0)
	{
	    FAIL("loop.2: I_FLUSH: %s\n", STRERROR(-rslt)) ;
	}

	flush_wait(fd2) ;


		/********************************
		*           I_SETSIG            * 
		********************************/

	tprint("%s", "Testing I_SETSIG/I_GETSIG\n") ;
	rslt = user_ioctl(fd1, I_SETSIG, S_INPUT) ;
	if (rslt < 0)
	{
	    FAIL("loop.1: I_SETSIG: %s\n", STRERROR(-rslt)) ;
	}

	rslt = user_ioctl(fd1, I_GETSIG, &arg) ;
	if (rslt < 0)
	{
	    FAIL("loop.1: I_GETSIG: %s\n", STRERROR(-rslt)) ;
	}

	if (arg == S_INPUT)
	    tprint("loop.1: I_GETSIG returned 0x%x, OK\n", arg) ;
	else
	{
	    FAIL("loop.1: I_GETSIG returned 0x%x, expected 0x%x\n",
	    		arg, S_INPUT) ;
	}

	signal(SIGPOLL, input_sig) ;
	rslt = write_data(fd2, ctx->buf, lgth) ;	/* produce some input on fd1 */
	if (rslt < 0)
	    FAIL("%s", "write_data\n") ;

	sleep(1) ;		/* allow signal to happen */


		/********************************
		*             I_POP             * 
		********************************/

	tprint("%s", "Testing I_POP\n") ;
	rslt = user_ioctl(fd1, I_POP, 0) ;
	if (rslt < 0)
	{
	    FAIL("loop.1: I_POP: %s\n", STRERROR(-rslt)) ;
	}

		/********************************
		*         TRANSPARENT           * 
		********************************/

	tprint("%s", "Testing TRANSPARENT ioctls\n") ;
	strcpy(ctx->buf, "Data for loop driver transparent test") ;
	xp.cmnd  = LOOP_XPARENT_COPYIN ;
	xp.i_arg = strlen(ctx->buf) ;
	xp.p_arg = ctx->buf ;
	rslt = user_ioctl(fd1, LOOP_XPARENT_COPYIN, &xp) ;
	if (rslt < 0)
	{
	    FAIL("loop.1: LOOP_XPARENT_COPYIN: %s\n", STRERROR(-rslt)) ;
	}

	xp.cmnd  = LOOP_XPARENT_COPYOUT ;
	xp.i_arg = sizeof(ctx->rdbuf) ;
	xp.p_arg = ctx->rdbuf ;
	rslt = user_ioctl(fd1, LOOP_XPARENT_COPYOUT, &xp) ;
	if (rslt < 0)
	{
	    FAIL("loop.1: LOOP_XPARENT_COPYOUT: %s\n", STRERROR(-rslt)) ;
	}

	tprint("Snt: %s\n", ctx->buf) ;
	tprint("Rcv: %s\n", ctx->rdbuf) ;
	if (strcmp(ctx->buf, ctx->rdbuf) != 0)
	{
	    FAIL("%s", "Buffers do not compare\n") ;
	}

	tprint("%s", "Buffers compare OK\n") ;



	print_mem() ;


		/********************************
		*         Close Files           * 
		********************************/

	user_close(fd1) ;
	user_close(fd2) ;
    }

} /* ioctl_test */

/************************************************************************
*                             rdopt_test                                *
*************************************************************************
*									*
* Test various read options (and the associated ioctls).		*
*									*
************************************************************************/
void	rdopt_test(struct test_context *ctx)
{
    int			i ;
    int			fd1 ;
    int			fd2 ;
    int			arg ;
    int			rslt ;
    int			lgth ;
    int			lgth2 ;
    struct strioctl	ioc ;

    tprint("%s", "Read option test\n") ;

	    /********************************
	    *         Open Files            * 
	    ********************************/

    rslt = open_files(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_files\n") ;

    ioc.ic_timout = 10 ;
    ioc.ic_dp	  = NULL;
    ioc.ic_cmd 	  = LOOP_PUTNXT ;	/* use putnxt rather then svcq */
    ioc.ic_len	  = 0 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.1: ioctl LOOP_PUTNXT: %s\n", STRERROR(-rslt)) ;
    }

    ioc.ic_len	  = 0 ;
    rslt = user_ioctl(fd2, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.2: ioctl LOOP_PUTNXT: %s\n", STRERROR(-rslt)) ;
    }

	    /********************************
	    *         RNORM Test            * 
	    ********************************/

    tprint("%s", "Testing I_SRDOPT/I_GRDOPT(RNORM)\n") ;
    rslt = user_ioctl(fd1, I_SRDOPT, RNORM) ;
    if (rslt < 0)
    {
	FAIL("loop.1: I_SRDOPT(RNORM): %s\n", STRERROR(-rslt)) ;
    }

    rslt = user_ioctl(fd1, I_GRDOPT, &arg) ;
    if (rslt < 0)
    {
	FAIL("loop.1: I_GRDOPT(RNORM): %s\n", STRERROR(-rslt)) ;
    }

    if ((arg & RMODEMASK) == RNORM)
	tprint("%s", "I_SRDOPT(RNORM) == I_GRDOPT\n") ;
    else
	tprint("I_SRDOPT(RNORM): set opt to %d, read back %d\n",
		    RNORM, arg & RMODEMASK) ;

    /*
     * Demonstrate this mode of operation by writing two messasges
     * downstream and then reading back both of them in one read.
     */
    strcpy(ctx->buf, "Test data for I_SRDOPT(RNORM)") ;
    lgth = strlen(ctx->buf) ;
    for (i = 1; i <= 3; i++)		/* write it three times */
    {
	if (write_data(fd2, ctx->buf, lgth) < 0) FAIL("%s", "write_data\n") ;
    }

    /*
     * Streams read will wait until the entire count is
     * exhausted as the default (or out of data to read).  
     */
    if (nread_wait_msgs(fd1, 3) != 3)
    {
	FAIL("%s", "loop.1: messages failed to appear\n") ;
    }
    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;
    rslt = user_read(fd1, ctx->rdbuf, 2*lgth+1);
    if (rslt < 0)
    {
	FAIL("loop.1: read: %s\n", STRERROR(-rslt)) ;
    }

    if (rslt != 2*lgth+1)
    {
	FAIL("loop.1:  read returned %d, expected %d\n", rslt, 2*lgth+1) ;
    }

    /*
     * Read the rest in with a count that is more than the length
     * of the remaining data.
     */
    rslt = user_read(fd1, &ctx->rdbuf[2*lgth+1], 2*lgth);
    if (rslt < 0)
    {
	FAIL("loop.1: read: %s\n", STRERROR(-rslt)) ;
    }

    if (rslt != lgth-1)
    {
	FAIL("loop.1:  read returned %d, expected %d\n", rslt, lgth-1) ;
    }

    if (   strncmp(ctx->buf, &ctx->rdbuf[0],      lgth) == 0
        && strncmp(ctx->buf, &ctx->rdbuf[lgth],   lgth) == 0
        && strncmp(ctx->buf, &ctx->rdbuf[2*lgth], lgth) == 0
       )
	   tprint("%s", "Buffers compare OK\n") ;
    else
    {
	tprint("%s", "Data read does not match data written\n") ;
	tprint("Wrote: %s\n", ctx->buf) ;
	tprint("Read:  %s\n", ctx->rdbuf) ;
	FAIL("%s", "\n") ;
    }

	    /********************************
	    *         RMSGD Test            * 
	    ********************************/

    tprint("%s", "Testing I_SRDOPT/I_GRDOPT(RMSGD)\n") ;
    rslt = user_ioctl(fd1, I_SRDOPT, RMSGD) ;
    if (rslt < 0)
    {
	FAIL("loop.1: I_SRDOPT(RMSGD): %s\n", STRERROR(-rslt)) ;
    }

    rslt = user_ioctl(fd1, I_GRDOPT, &arg);
    if (rslt < 0)
    {
	FAIL("loop.1: I_GRDOPT(RMSGD): %s\n", STRERROR(-rslt)) ;
    }

    if ((arg & RMODEMASK) == RMSGD)
	tprint("%s", "I_SRDOPT(RMSGD) == I_GRDOPT\n") ;
    else
	tprint("I_SRDOPT(RMSGD): set opt to %d, read back %d\n",
		    RMSGD, arg & RMODEMASK) ;

    /*
     * Demonstrate this mode of operation by writing a message
     * and reading just a part of it back.  I_NREAD will then
     * tell us that there is nothing to be read anymore.
     */
    strcpy(ctx->buf, "Test data for I_SRDOPT(RMSGD)") ;
    lgth = strlen(ctx->buf) ;
    if (write_data(fd2, ctx->buf, lgth) < 0) FAIL("%s", "write_data\n") ;

    /*
     * Streams read will return with whatever is there.  Leftover
     * message fragment will be discarded.
     */
    if (nread_wait_msgs(fd1, 1) != 1)
    {
	FAIL("%s", "loop.1: message failed to appear\n") ;
    }
    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;	
    rslt = user_read(fd1, ctx->rdbuf, lgth/2);	/* read half the message */
    if (rslt < 0)
    {
	FAIL("loop.1: read: %s\n", STRERROR(-rslt)) ;
    }

    if (rslt != lgth / 2)
    {
	FAIL("loop.1:  read returned %d, expected %d\n", rslt, lgth/2) ;
    }

    if (strncmp(ctx->buf, ctx->rdbuf, lgth/2) == 0)
	tprint("%s", "Buffers compare OK\n") ;
    else
    {
	tprint("%s", "Data read does not match data written\n") ;
	tprint("Wrote: %s\n", ctx->buf) ;
	tprint("Read:  %s\n", ctx->rdbuf) ;
	FAIL("%s", "\n") ;
    }

    rslt = n_read(fd1) ;
    if (rslt < 0) FAIL("%s", "n_read\n") ;
    if (rslt > 0)
	tprint("%d bytes still waiting to be read, should be zero\n", rslt);
    else
	tprint("%s", "No bytes waiting to be read\n") ;


	    /********************************
	    *         RMSGN Test            * 
	    ********************************/

    tprint("%s", "Testing I_SRDOPT/I_GRDOPT(RMSGN)\n") ;
    rslt = user_ioctl(fd1, I_SRDOPT, RMSGN) ;
    if (rslt < 0)
    {
	FAIL("loop.1: I_SRDOPT(RMSGN): %s\n", STRERROR(-rslt)) ;
    }

    rslt = user_ioctl(fd1, I_GRDOPT, &arg);
    if (rslt < 0)
    {
	FAIL("loop.1: I_GRDOPT(RMSGN): %s\n", STRERROR(-rslt)) ;
    }

    if ((arg & RMODEMASK) == RMSGN)
	tprint("%s", "I_SRDOPT(RMSGN) == I_GRDOPT\n") ;
    else
	tprint("I_SRDOPT(RMSGN): set opt to %d, read back %d\n",
		    RMSGN, arg & RMODEMASK) ;

    /*
     * Demonstrate this mode of operation by writing a message
     * and reading just a part of it back.  I_NREAD will then
     * tell us that there are more bytes waiting to be read.
     */
    strcpy(ctx->buf, "Test data for I_SRDOPT(RMSGN)") ;
    lgth = strlen(ctx->buf) ;
    if (write_data(fd2, ctx->buf, lgth) < 0) FAIL("%s", "write_data\n") ;

    /*
     * Streams read will return with whatever is there.  Leftover
     * message fragment will be saved.
     */
    if (nread_wait_msgs(fd1, 1) != 1)
    {
	FAIL("%s", "loop.1: message failed to appear\n") ;
    }
    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;	
    rslt = user_read(fd1, ctx->rdbuf, lgth/2);	/* read half the message */
    if (rslt < 0)
    {
	FAIL("loop.1: read: %s\n", STRERROR(-rslt)) ;
    }

    if (rslt != lgth/2)
    {
	FAIL("loop.1:  read returned %d, expected %d\n", rslt, lgth/2) ;
    }

    rslt = n_read(fd1) ;			/* see what's left */
    if (rslt < 0) FAIL("%s", "n_read\n") ;
    if (rslt != lgth - lgth/2)
    {
	FAIL("%d bytes still waiting to be read, should be %d\n",
		 rslt, lgth - lgth/2);
    }

    tprint("%d bytes waiting to be read, OK\n", rslt) ;

    rslt = user_read(fd1, &ctx->rdbuf[lgth/2], lgth) ;	/* read the rest */
    if (rslt < 0)
    {
	FAIL("loop.1: read: %s\n", STRERROR(-rslt)) ;
    }

    if (rslt != lgth - lgth/2)
    {
	FAIL("%d bytes on second read, should be %d\n",
		 rslt, lgth - lgth/2);
    }

    if (strncmp(ctx->buf, ctx->rdbuf, lgth) == 0)
	tprint("%s", "Buffers compare OK\n") ;
    else
    {
	tprint("%s", "Data read does not match data written\n") ;
	tprint("Wrote: %s\n", ctx->buf) ;
	tprint("Read:  %s\n", ctx->rdbuf) ;
	FAIL("%s", "\n") ;
    }

	    /********************************
	    *           RPROTDAT            * 
	    ********************************/

    tprint("%s", "Testing I_SRDOPT/I_GRDOPT(RPROTDAT)\n") ;
    rslt = user_ioctl(fd1, I_SRDOPT, RNORM|RPROTDAT) ;
    if (rslt < 0)
    {
	FAIL("loop.1: I_SRDOPT(RPROTDAT): %s\n", STRERROR(-rslt)) ;
    }

    rslt = user_ioctl(fd1, I_GRDOPT, &arg) ;
    if (rslt < 0)
    {
	FAIL("loop.1: I_GRDOPT(RPROTDAT): %s\n", STRERROR(-rslt)) ;
    }

    if (arg == (RNORM|RPROTDAT))
	tprint("%s", "I_SRDOPT(RPROTDAT) == I_GRDOPT\n") ;
    else
	tprint("I_SRDOPT(RPROTDAT): set opt to %d, read back %d\n",
		    RNORM|RPROTDAT, arg) ;

    /*
     * Use putmsg to send a control message, read it back as normal
     * data.
     */
    tprint("%s", "Control message only...\n") ;
    strcpy(ctx->ctlbuf, "Control message for RPROTDAT test") ;
    lgth = strlen(ctx->ctlbuf) ;
    ctx->wr_ctl.len	= lgth ;
    ctx->wr_dta.len	= -1 ;				/* no data part */
    if (put_msg(fd2, &ctx->wr_ctl, &ctx->wr_dta, 0, MSG_BAND) < 0) FAIL("%s", "put_msg\n") ;

    if (nread_wait_msgs(fd1, 1) != 1)
    {
	FAIL("%s", "loop.1: message failed to appear\n") ;
    }
    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;	
    rslt = user_read(fd1, ctx->rdbuf, lgth);		/* read the message */
    if (rslt < 0)
    {
	FAIL("loop.1: read: %s\n", STRERROR(-rslt)) ;
    }

    if (rslt != lgth)
    {
	FAIL("loop.1:  read returned %d, expected %d\n", rslt, lgth) ;
    }

    if (strcmp(ctx->ctlbuf, ctx->rdbuf) == 0)
	tprint("%s", "Buffers compare OK\n") ;
    else
    {
	tprint("%s", "Data read does not match data written\n") ;
	tprint("Wrote: %s\n", ctx->ctlbuf) ;
	tprint("Read:  %s\n", ctx->rdbuf) ;
	FAIL("%s", "\n") ;
    }

    /*
     * Use putmsg to send a control and data message, read it back as normal
     * data.
     */
    tprint("%s", "Control and data message...\n") ;
    strcpy(ctx->ctlbuf, "Control message for RPROTDAT test") ;
    strcpy(ctx->buf,    "/Data message for RPROTDAT test") ;
    lgth = strlen(ctx->ctlbuf) ;
    lgth2 = strlen(ctx->buf) ;
    ctx->wr_ctl.len	= lgth ;
    ctx->wr_dta.len	= lgth2 ;
    if (put_msg(fd2, &ctx->wr_ctl, &ctx->wr_dta, 0, MSG_BAND) < 0) FAIL("%s", "put_msg\n") ;

    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;	
    lgth = lgth + lgth2 ;			/* combined message lgth */
    if (nread_wait_msgs(fd1, 1) != 1)
    {
	FAIL("%s", "loop.1: message failed to appear\n") ;
    }
    rslt = user_read(fd1, ctx->rdbuf, lgth);		/* read the message */
    if (rslt < 0)
    {
	FAIL("loop.1: read: %s\n", STRERROR(-rslt)) ;
    }

    if (rslt != lgth)
    {
	FAIL("loop.1:  read returned %d, expected %d\n", rslt, lgth) ;
    }

    strcat(ctx->ctlbuf, ctx->buf) ;			/* concatenate the msgs */
    if (strcmp(ctx->ctlbuf, ctx->rdbuf) == 0)
	tprint("%s", "Buffers compare OK\n") ;
    else
    {
	tprint("%s", "Data read does not match data written\n") ;
	tprint("Wrote: %s\n", ctx->ctlbuf) ;
	tprint("Read:  %s\n", ctx->rdbuf) ;
	FAIL("%s", "\n") ;
    }

	    /********************************
	    *           RPROTDIS            * 
	    ********************************/

    tprint("%s", "Testing I_SRDOPT/I_GRDOPT(RPROTDIS)\n") ;
    rslt = user_ioctl(fd1, I_SRDOPT, RMSGN|RPROTDIS) ;
    if (rslt < 0)
    {
	FAIL("loop.1: I_SRDOPT(RPROTDIS): %s\n", STRERROR(-rslt)) ;
    }

    rslt = user_ioctl(fd1, I_GRDOPT, &arg);
    if (rslt < 0)
    {
	FAIL("loop.1: I_GRDOPT(RPROTDIS): %s\n", STRERROR(-rslt)) ;
    }

    if (arg == (RMSGN|RPROTDIS))
	tprint("%s", "I_SRDOPT(RPROTDIS) == I_GRDOPT\n") ;
    else
	tprint("I_SRDOPT(RPROTDIS): set opt to %d, read back %d\n",
		    RMSGN|RPROTDIS, arg) ;

    /*
     * Use putmsg to send a control message, should leave nothing to
     * be read.
     *
     * It is not clear whether this should result in a zero-length
     * message or no message at all.  For now it is the second.
     */
    tprint("%s", "Control message only...\n") ;
    strcpy(ctx->ctlbuf, "Control message for RPROTDIS test") ;
    lgth = strlen(ctx->ctlbuf) ;
    ctx->wr_ctl.len	= lgth ;
    ctx->wr_dta.len	= -1 ;				/* no data part */
    if (put_msg(fd2, &ctx->wr_ctl, &ctx->wr_dta, 0, MSG_BAND) < 0) FAIL("%s", "put_msg\n") ;

    if (nread_wait_msgs(fd1, 1) != 1)
    {
	FAIL("%s", "loop.1: message failed to appear\n") ;
    }

    rslt = n_read(fd1);				/* check how many bytes */
    if (rslt < 0) FAIL("%s", "n_read\n") ;

    if (rslt != 0)				/* should read 0 bytes */
    {
	FAIL("loop.1:  I_NREAD returned %d, expected %d\n", rslt, 0) ;
    }

    rslt = user_read(fd1, ctx->rdbuf, sizeof(ctx->rdbuf));	/* read the message */
    if (rslt < 0)
    {
	FAIL("loop.1: read: %s\n", STRERROR(-rslt)) ;
    }
    if (rslt > 0)
    {
	FAIL("loop.1:  read returned %d, expected 0\n", rslt) ;
    }

    /*
     * Use putmsg to send a control and data message, read it back as normal
     * data.  The control part should be discarded and the data part
     * delivered.
     */
    tprint("%s", "Control and data message...\n") ;
    strcpy(ctx->ctlbuf, "Control message for RPROTDIS test") ;
    strcpy(ctx->buf,    "/Data message for RPROTDIS test") ;
    lgth = strlen(ctx->ctlbuf) ;
    lgth2 = strlen(ctx->buf) ;
    ctx->wr_ctl.len	= lgth ;
    ctx->wr_dta.len	= lgth2 ;
    if (put_msg(fd2, &ctx->wr_ctl, &ctx->wr_dta, 0, MSG_BAND) < 0) FAIL("%s", "put_msg\n") ;

    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;	
    if (nread_wait_msgs(fd1, 1) != 1)
    {
	FAIL("%s", "loop.1: messages failed to appear\n") ;
    }
    rslt = user_read(fd1, ctx->rdbuf, sizeof(ctx->rdbuf));	/* read the message */
    if (rslt < 0)
    {
	FAIL("loop.1: read: %s\n", STRERROR(-rslt)) ;
    }

    if (rslt != lgth2)
    {
	FAIL("loop.1:  read returned %d, expected %d\n", rslt, lgth2) ;
    }

    if (strcmp(ctx->buf, ctx->rdbuf) == 0)			/* only the data */
	tprint("%s", "Buffers compare OK\n") ;
    else
    {
	tprint("%s", "Data read does not match data written\n") ;
	tprint("Wrote: %s\n", ctx->buf) ;
	tprint("Read:  %s\n", ctx->rdbuf) ;
	FAIL("%s", "\n") ;
    }

	    /********************************
	    *         Close Files           * 
	    ********************************/

    tprint("%s", "rdopt_test: close files\n") ;
    user_close(fd1) ;
    user_close(fd2) ;

} /* rdopt_test */

/************************************************************************
*                               write_test                              *
*************************************************************************
*									*
* Test various forms of write.						*
*									*
************************************************************************/
void	write_test(struct test_context *ctx)
{
    int			fd1 ;
    int			fd2 ;
    int			arg ;
    int			rslt ;
    int			i ;
    struct strioctl	ioc ;
    int			lgth ;
#if 0					/* needed for ioctl tst below */
    int			unblocked ;
#endif

    tprint("%s", "Write option test\n") ;

	    /********************************
	    *         Open Files            * 
	    ********************************/

    rslt = open_files(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_files\n") ;

	    /********************************
	    *       Zero-lgth Write         * 
	    ********************************/

    tprint("%s", "Testing write zero bytes w/o SNDZERO option\n") ;
    rslt = user_write(fd1, ctx->buf, 0) ;
#if 1
    if (rslt == 0)
	tprint("%s", "loop.1: *** write zero bytes returned zero.  Should it?\n") ;
    else
	tprint("loop.1: *** write zero bytes returned %d.  Should it?\n", rslt) ;
#else
    if (rslt < 0)
	ERR("loop.1: write zero bytes: %s: expected error\n", 
		    STRERROR(-rslt)) ;
    else
    {
	FAIL("loop.1: write zero bytes: returned %d instead of error\n",
		rslt) ;
    }
#endif

    tprint("%s", "Testing write zero bytes with SNDZERO option\n") ;
    rslt = user_ioctl(fd1, I_SWROPT, SNDZERO) ;
    if (rslt < 0)
    {
	FAIL("loop.1: I_SWROPT(SNDZERO): %s\n", STRERROR(-rslt)) ;
    }

    rslt = user_ioctl(fd1, I_GWROPT, &arg) ;
    if (rslt < 0)
    {
	FAIL("loop.1: I_SWROPT(SNDZERO): %s\n", STRERROR(-rslt)) ;
    }

    if (arg == SNDZERO)
	tprint("%s", "I_SWROPT(SNDZERO) == I_GWROPT\n") ;
    else
    {
	FAIL("I_SWROPT(SNDZERO): set opt to %d, read back %d\n",
		    SNDZERO, arg) ;
    }

    rslt = user_write(fd1, ctx->buf, 0) ;
    if (rslt < 0)
    {
	FAIL("loop.1: write zero bytes: %s\n", STRERROR(-rslt)) ;
    }
    else
	tprint("loop.1: write zero bytes: returned %d\n", rslt) ;

    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;	
    if (nread_wait_msgs(fd2, 1) != 1)
    {
	FAIL("%s", "loop.2: message failed to appear\n") ;
    }
    rslt = user_read(fd2, ctx->rdbuf, sizeof(ctx->rdbuf)) ;
    if (rslt < 0)
    {
	FAIL("loop.2: read: %s\n", STRERROR(-rslt)) ;
    }

    if (rslt != 0)
    {
	FAIL("loop.2:  read returned %d, expected %d\n", rslt, 0) ;
    }

    tprint("%s", "Read 0 bytes from 0-length write\n") ;

	    /********************************
	    *         Message Burst	    * 
	    ********************************/

    print("\nTesting a burst of messages at the stream head\n") ;

    ioc.ic_cmd 	  = LOOP_BURST ;
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;

    arg = 60 ;			/* number of burst messages */
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop.1: ioctl LOOP_BURST: %s\n", STRERROR(-rslt)) ;
	xit() ;
    }

    strcpy(ctx->buf, "Burst data test:  ............data pattern.......") ;
    lgth = strlen(ctx->buf)+1 ;

    rslt = user_write(fd1, ctx->buf, lgth) ;
    if (rslt < 0)
    {
	print("loop.1: write: %s\n", STRERROR(-rslt)) ;
	xit() ;
    }

    sleep(1) ;			/* allow messages to flow */
    print("Read back burst data\n") ;

    /*
     * Set RMSGN so that read will just return a single message.
     */
    rslt = user_ioctl(fd2, I_SRDOPT, RMSGN) ;
    if (rslt < 0)
    {
	print("loop.2: I_SRDOPT(RMSGN): %s\n", STRERROR(-rslt)) ;
	xit() ;
    }

    for (i = 0; n_read(fd2) > 0; i++)
    {
	rslt = user_read(fd2, ctx->rdbuf, sizeof(ctx->rdbuf)) ;
	if (rslt < 0)
	{
	    print("loop.2: read: %s\n", STRERROR(-rslt)) ;
	    xit() ;
	}

	if (rslt != lgth)
	{
	    print("Expected %d bytes, got %d\n", lgth, rslt) ;
	    xit() ;
	}

	if (strcmp(ctx->buf, ctx->rdbuf) != 0)
	{
	    print("Wrote: %s\n", ctx->buf) ;
	    print(" Read: %s\n", ctx->rdbuf) ;
	    xit() ;
	}
    }

    if (i != arg)
    {
	print("Read %d messages, expected %d\n", i, arg) ;
	xit() ;
    }

#if 0

    /*
     * Can''t easily do this test.  It requires the writer to block and the reader to relieve the flow control.  In 
     * a single process we will hang on the write and never get control back to do the read. 
     */

	    /********************************
	    *         Flow Control	    * 
	    ********************************/

    tprint("Testing downstream flow control\n") ;

    ioc.ic_cmd 	  = LOOP_MSGLVL ;		/* set queue message level */
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;

    arg = 20 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop.1: ioctl LOOP_MSGLVL: %s\n", STRERROR(-rslt)) ;
    }

    strcpy(ctx->buf, "Flow control data test:  ............data pattern.......") ;
    for (lgth = 0; lgth < 20; lgth++)
	strcat(ctx->buf, ".....more data.....") ;
    lgth = strlen(ctx->buf) ;
    while ((rslt = user_ioctl(fd1, I_CANPUT, 0)) > 0)
    {
	rslt = user_write(fd1, ctx->buf, lgth) ;
	if (rslt < 0) break ;
	arg-- ;				/* count down nr of messages */
    }

    if (rslt < 0)
    {
	FAIL("loop.1: ioctl or write: %s\n", STRERROR(-rslt)) ;
    }

    tprint("Flow control blocked with %d messages queued at driver\n",
		20 - arg) ;

    ioc.ic_cmd 	  = LOOP_MSGLVL ;		/* set queue message level */
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;

    arg = 0 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop.1: ioctl LOOP_MSGLVL: %s\n", STRERROR(-rslt)) ;
    }

    while (arg--)
    {
	rslt = user_write(fd1, ctx->buf, lgth) ;
	if (rslt < 0)
	{
	    FAIL("loop.1: write: %s\n", STRERROR(-rslt)) ;
	}
    }

    tprint("%d bytes of data queued to read\n", n_read(fd2)) ;
    unblocked = 0 ;
    while (n_read(fd2) > 0)		/* read all the data */
    {
	rslt = user_ioctl(fd1, I_CANPUT, 0) ;
	if (rslt < 0)
	{
	    FAIL("loop.1: ioctl during readback: %s\n", STRERROR(-rslt)) ;
	}

	if (!unblocked && rslt > 0)
	{
	    tprint("Unblocked with %d bytes of data queued to read\n",
		    n_read(fd2)) ;
	    unblocked = 1 ;
	}

	rslt = user_read(fd2, ctx->rdbuf, sizeof(ctx->rdbuf)) ;
	if (rslt < 0)
	{
	    FAIL("loop.2: read: %s\n", STRERROR(-rslt)) ;
	}
    }

    if (!unblocked)
    {
	FAIL("read all data but write still not unblocked\n") ;
    }

    tprint("Flow control test succeeded\n") ;

#endif

	    /********************************
	    *         Close Files           * 
	    ********************************/

    user_close(fd1) ;
    user_close(fd2) ;

} /* write_test */

/************************************************************************
*                           close_timer_test                            *
*************************************************************************
*									*
* Test the functioning of the close timer.				*
*									*
************************************************************************/
void	close_timer_test(struct test_context *ctx)
{
    int			fd1 ;
    int			fd2 ;
    int			arg ;
    int			rslt ;
    struct strioctl	ioc ;

    tprint("%s", "Close timer test\n") ;

	    /********************************
	    *         Open Files            * 
	    ********************************/

    tprint("%s", "Close and let timer expire\n") ;
    rslt = open_files(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_files\n") ;

	    /********************************
	    *         Set 'loop' Optns      *
	    ********************************/


    ioc.ic_cmd 	  = LOOP_MSGLVL ;		/* set queue message level */
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;

    arg = 2 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop.1: ioctl LOOP_MSGLVL: %s\n", STRERROR(-rslt)) ;
    }

	    /********************************
	    *         Write Message         * 
	    ********************************/

    rslt = write_data(fd1, ctx->buf, 20) ;
    if (rslt < 0) FAIL("%s", "write_data\n") ;

    rslt = n_read(fd2) ;
    if (rslt < 0) FAIL("%s", "n_read") ;
    if (rslt > 0)
	tprint("loop.2: I_NREAD returned %d, expected 0\n", rslt) ;


	    /********************************
	    *         Close Files           * 
	    ********************************/

    user_close(fd1) ;
    user_close(fd2) ;


	    /********************************
	    *         Open Files            * 
	    ********************************/

    tprint("%s", "Close and have queue drain before timer expires\n") ;
    rslt = open_files(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_files\n") ;

	    /********************************
	    *         Set 'loop' Optns      *
	    ********************************/


    ioc.ic_cmd 	  = LOOP_MSGLVL ;		/* set queue message level */
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;

    arg = 2 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop.1: ioctl LOOP_MSGLVL: %s\n", STRERROR(-rslt)) ;
    }

    ioc.ic_cmd 	  = LOOP_TIMR ;		/* set timer for queue */
    arg = 10 ;				/* # timer ticks */
    ioc.ic_len	  = sizeof(int) ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop.1: ioctl LOOP_TIMR: %s\n", STRERROR(-rslt)) ;
    }

	    /********************************
	    *         Write Message         * 
	    ********************************/

    rslt = write_data(fd1, ctx->buf, 20) ;
    if (rslt < 0) FAIL("%s", "write_data\n") ;

    rslt = n_read(fd2) ;
    if (rslt < 0) FAIL("%s", "n_read\n") ;
    if (rslt > 0)
	tprint("loop.2: I_NREAD returned %d, expected 0\n", rslt) ;


	    /********************************
	    *         Close Files           * 
	    ********************************/

    user_close(fd1) ;
    user_close(fd2) ;

} /* close_timer_test */

/************************************************************************
*                          check_getmsg_rslts                           *
*************************************************************************
*									*
* Check the results of a getmsg.  We make assumptions about the		*
* locations of the data.  We call FAIL() if anything goes wrong.	*
*									*
* band is the band the message was sent on, rband is the band it	*
* was received on.							*
*									*
************************************************************************/
void	check_getmsg_rslts(struct test_context *ctx,
			   int		  rslt,
			   struct strbuf *rd_ctlp,
			   struct strbuf *rd_dtap,
			   int		 *flags,
			   int		  band,
			   int		  rband)
{
    (void) rslt ;

    if (rd_ctlp != NULL && rd_ctlp->len != ctx->wr_ctl.len)
    {
	FAIL("check_getmsg_rslts:  ctl lgth returned %d, expected %d\n",
		rd_ctlp->len, ctx->wr_ctl.len) ;
    }

    if (rd_dtap != NULL && rd_dtap->len != ctx->wr_dta.len)
    {
	FAIL("check_getmsg_rslts:  data lgth returned %d, expected %d\n",
		rd_dtap->len, ctx->wr_dta.len) ;
    }

    tprint("check_getmsg_rslts: getmsg return flags = 0x%x\n", *flags) ;
    if (   rd_dtap != NULL
	&& rd_dtap->len > 0
	&& strncmp(ctx->buf, ctx->rdbuf, rd_dtap->len)
       )
    {
	tprint("%s", "check_getmsg_rslts: read data: buffer compare error\n") ;
	tprint("              wrote \"%s\"\n", ctx->buf) ;
	tprint("              read  \"%s\"\n", ctx->rdbuf) ;
	FAIL("%s", "\n") ;
    }
    else
    if (rd_dtap != NULL)
	tprint("check_getmsg_rslts: read %d data bytes: buffer compared OK\n",
		rd_dtap->len) ;
    else
	tprint("%s", "check_getmsg_rslts: no data strbuf\n") ;

    if (   rd_ctlp != NULL
	&& rd_ctlp->len > 0
	&& strncmp(ctx->ctlbuf, ctx->rdctlbuf, rd_ctlp->len)
       )
    {
	tprint("%s", "check_getmsg_rslts: read control: buffer compare error\n") ;
	tprint("              wrote \"%s\"\n", ctx->ctlbuf) ;
	tprint("              read  \"%s\"\n", ctx->rdctlbuf) ;
	FAIL("%s", "\n") ;
    }
    else
    if (rd_ctlp != NULL)
	tprint("check_getmsg_rslts: read %d ctl bytes: buffer compared OK\n",
		rd_ctlp->len) ;
    else
	tprint("%s", "check_getmsg_rslts: no control strbuf\n") ;

    if (rband != band)
	tprint("check_getmsg_rslts: sent on band %d, received on band %d\n",
		band, rband) ;

} /* check_getmsg_rslts */

/************************************************************************
*                             do_get_put                                *
*************************************************************************
*									*
* This is a generalized getmsg/putmsg test routine.  It calls FAIL()	*
* if anything goes wrong.						*
*									*
* Caller must load up buf (data) and ctx->ctlbuf (control) for writing.	*
*									*
************************************************************************/
void	do_get_put(struct test_context *ctx, int putfd, int getfd,
		   int ctl_lgth, int data_lgth, int band)
{
    int			flags = MSG_ANY;
    int			rband = 0 ;
    int			rslt ;

    ctx->wr_ctl.len	= ctl_lgth ;
    ctx->wr_dta.len	= data_lgth ;
    if (put_msg(putfd, &ctx->wr_ctl, &ctx->wr_dta, band, MSG_BAND) < 0) FAIL("%s", "put_msg\n") ;

    ctx->rd_ctl.len		= -1 ;
    ctx->rd_ctl.maxlen	= sizeof(ctx->rdctlbuf) ;
    ctx->rd_dta.len		= -1 ;
    ctx->rd_dta.maxlen	= sizeof(ctx->rdbuf) ;

    memset(ctx->rdctlbuf, 0, sizeof(ctx->rdctlbuf)) ;
    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;

    if (nread_wait_msgs(getfd, 1) != 1)
    {
	FAIL("%s", "do_get_put: message failed to appear\n") ;
    }
    rslt = user_getpmsg(getfd, &ctx->rd_ctl, &ctx->rd_dta, &rband, &flags) ;
    check_getmsg_rslts(ctx, rslt, &ctx->rd_ctl, &ctx->rd_dta, &flags, band, rband) ;

} /* do_get_put */

/************************************************************************
*                           do_put                                      *
*************************************************************************
*									*
* Do just the putmsg.  Useful for building up concatenated messages.	*
*									*
************************************************************************/
void	do_put(struct test_context *ctx, int putfd, int ctl_lgth, int data_lgth, int band)
{
    int		flags = MSG_ANY;

    (void) flags ;			/* compiler happiness */
    ctx->wr_ctl.len	= ctl_lgth ;
    ctx->wr_dta.len	= data_lgth ;
    if (put_msg(putfd, &ctx->wr_ctl, &ctx->wr_dta, band, MSG_BAND) < 0) FAIL("%s", "put_msg\n") ;

} /* do_put */

/************************************************************************
*                              do_peek                                  *
*************************************************************************
*									*
* This routine does a putmsg, then an I_PEEK, then a getmsg.  It calls	*
* FAIL() if anything goes wrong.					*
*									*
* Caller must load up buf (data) and ctx->ctlbuf (control) for writing.	*
*									*
************************************************************************/
void	do_peek_fcn(struct test_context *ctx, int putfd, int getfd,
		    int wr_ctl_lgth, int wr_data_lgth,
		    int rd_ctl_lgth, int rd_data_lgth,
		    int band)
{
    int			flags = MSG_ANY;
    int			rband = 0 ;
    int			rslt ;
    int			msgs ;
    struct strbuf	*ctlp ;
    struct strbuf	*dtap ;

    ctx->wr_ctl.len	= wr_ctl_lgth ;
    ctx->wr_dta.len	= wr_data_lgth ;
    if (put_msg(putfd, &ctx->wr_ctl, &ctx->wr_dta, band, MSG_BAND) < 0) FAIL("%s", "put_msg\n") ;

    ctx->pk_str.ctlbuf.len		= -1 ;
    ctx->pk_str.ctlbuf.maxlen	= rd_ctl_lgth ;
    ctx->pk_str.databuf.len		= -1 ;
    ctx->pk_str.databuf.maxlen	= rd_data_lgth ;
    ctx->pk_str.flags		= MSG_ANY ;

    memset(ctx->rdctlbuf, 0, sizeof(ctx->rdctlbuf)) ;
    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;

    if (wr_ctl_lgth >= 0 || wr_data_lgth >= 0)
	msgs = 1 ;
    else
	msgs = 0 ;

    if (nread_wait_msgs(getfd, msgs) != msgs)
    {
	FAIL("loop.1: %d message(s) failed to appear\n", msgs) ;
    }
    rslt = user_ioctl(getfd, I_PEEK, &ctx->pk_str) ;
    if (rslt < 0)
    {
	FAIL("do_peek: ioctl I_PEEK: %s\n", STRERROR(-rslt)) ;
    }

    tprint("do_peek: maxlen    len  I_PEEK returned %d\n", rslt) ;
    tprint("control: %6d %6d\n", ctx->pk_str.ctlbuf.maxlen, ctx->pk_str.ctlbuf.len) ;
    tprint("   data: %6d %6d\n", ctx->pk_str.databuf.maxlen, ctx->pk_str.databuf.len) ;

    flags = (int) ctx->pk_str.flags ;		/* int <-- long */

    if (rd_ctl_lgth >= wr_ctl_lgth)
	ctlp = &ctx->pk_str.ctlbuf ;
    else
	ctlp = NULL ;

    if (rd_data_lgth >= wr_data_lgth)
	dtap = &ctx->pk_str.databuf ;
    else
	dtap = NULL ;

    check_getmsg_rslts(ctx, rslt, ctlp, dtap, &flags, 0, 0) ;

    ctx->rd_ctl.len		= -1 ;
    if (wr_ctl_lgth >= 0)			/* ctl part written?  */
	ctx->rd_ctl.maxlen	= sizeof(ctx->rdctlbuf) ;	/* read ctl part */
    else
	ctx->rd_ctl.maxlen	= -1 ;

    ctx->rd_dta.len		= -1 ;
    if (wr_data_lgth >= 0)			/* data part written?  */
	ctx->rd_dta.maxlen	= sizeof(ctx->rdbuf) ;	/* read data part */
    else
	ctx->rd_dta.maxlen	= -1 ;

    flags		= 0 ;

    memset(ctx->rdctlbuf, 0, sizeof(ctx->rdctlbuf)) ;
    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;

    flags = MSG_ANY ;
    rslt = user_getpmsg(getfd, &ctx->rd_ctl, &ctx->rd_dta, &rband, &flags) ;
    check_getmsg_rslts(ctx, rslt, &ctx->rd_ctl, &ctx->rd_dta, &flags, band, rband) ;

} /* do_peek */

/************************************************************************
*                              do_peek                                  *
*************************************************************************
*									*
* This routine does a putmsg, then an I_PEEK, then a getmsg.  It calls	*
* FAIL() if anything goes wrong.					*
*									*
* Caller must load up buf (data) and ctx->ctlbuf (control) for writing.	*
*									*
************************************************************************/
void	do_peek(struct test_context *ctx, int putfd, int getfd,
		   int ctl_lgth, int data_lgth, int band)
{
    do_peek_fcn(ctx, putfd, getfd, ctl_lgth, data_lgth,
			sizeof(ctx->rdctlbuf), sizeof(ctx->rdbuf), band) ;

} /* do_peek */

/************************************************************************
*                            putmsg_test                                *
*************************************************************************
*									*
* Test putmsg and getmsg.						*
*									*
************************************************************************/
void	putmsg_test(struct test_context *ctx)
{
    int			fd1 ;
    int			fd2 ;
    int			rslt ;
    int			lgth ;
    int			lgth2 ;
    int			arg ;
    int			flags ;
    int			rband ;
    struct strioctl	ioc ;

    tprint("%s", "putmsg/getmsg test\n") ;

	    /********************************
	    *         Open Files            * 
	    ********************************/

    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_clones\n") ;

	    /********************************
	    *         putmsg/read           *
	    ********************************/

    tprint("%s", "Use putmsg to send data, use read to read back data\n") ;
    strcpy(ctx->buf, "Test data for putmsg/read") ;
    lgth = strlen(ctx->buf) ;
    ctx->wr_ctl.len	= -1 ;
    ctx->wr_dta.len	= lgth ;
    if (put_msg(fd1, &ctx->wr_ctl, &ctx->wr_dta, 0, MSG_BAND) < 0) FAIL("%s", "put_msg\n") ;

    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;
    if (nread_wait_msgs(fd2, 1) != 1)
    {
	FAIL("%s", "loop.2: message failed to appear\n") ;
    }
    rslt = user_read(fd2, ctx->rdbuf, lgth);
    if (rslt < 0)
    {
	FAIL("loop_clone.2: read: %s\n", STRERROR(-rslt)) ;
    }

    if (rslt != lgth)
    {
	FAIL("loop_clone.2:  read returned %d, expected %d\n", rslt, lgth) ;
    }

    if (strcmp(ctx->buf, ctx->rdbuf))
    {
	tprint("%s", "loop_clone.2: read: buffer compare error\n") ;
	tprint("              wrote \"%s\"\n", ctx->buf) ;
	tprint("              read  \"%s\"\n", ctx->rdbuf) ;
	FAIL("%s", "\n"); /* missing ? */
    }
    else
	tprint("loop_clone.2: read %d bytes: buffer compared OK\n", rslt) ;



    tprint("%s", "Use putmsg to send control, read should return error\n") ;
    strcpy(ctx->ctlbuf, "Control message") ;
    lgth2 = strlen(ctx->ctlbuf) ;
    ctx->wr_ctl.len	= lgth2 ;
    ctx->wr_dta.len	= lgth ;			/* from before */
    if (put_msg(fd1, &ctx->wr_ctl, &ctx->wr_dta, 0, MSG_BAND) < 0) FAIL("%s", "put_msg\n") ;

    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;
    if (nread_wait_msgs(fd2, 1) != 1)
    {
	FAIL("%s", "loop.2: message failed to appear\n") ;
    }
    rslt = user_read(fd2, ctx->rdbuf, lgth);
    if (rslt < 0)
	ERR("loop_clone.2: read returned expected error: %s\n", STRERROR(-rslt)) ;
    else
    {
	FAIL("loop_clone.2:  read returned %d, but expected error rtn\n",
		rslt) ;
    }

    /*
     * The message is still queued, so get it using getmsg.
     */
    tprint("%s", "Read re-queued message with getmsg\n") ;
    ctx->rd_ctl.len		= -1 ;
    ctx->rd_ctl.maxlen	= sizeof(ctx->rdctlbuf) ;
    ctx->rd_dta.len		= -1 ;
    ctx->rd_dta.maxlen	= sizeof(ctx->rdbuf) ;
    memset(ctx->rdctlbuf, 0, sizeof(ctx->rdctlbuf)) ;
    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;
    flags = MSG_ANY;
    rband = 0 ;
    rslt = user_getpmsg(fd2, &ctx->rd_ctl, &ctx->rd_dta, &rband, &flags) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.2:  getmsg returned %d\n", rslt) ;
    }

    if (ctx->rd_ctl.len != ctx->wr_ctl.len)
    {
	FAIL("loop_clone.2: read ctl len = %d, write ctl len = %d\n", 
	      ctx->rd_ctl.len, ctx->wr_ctl.len) ;
    }

    if (ctx->rd_dta.len != ctx->wr_dta.len)
    {
	FAIL("loop_clone.2: read data len = %d, write data len = %d\n", 
	      ctx->rd_dta.len, ctx->wr_dta.len) ;
    }

    rslt = 0 ;
    if (strcmp(ctx->ctlbuf, ctx->rdctlbuf))
    {
	tprint("%s", "loop_clone.2: getmsg: ctl buffer compare error\n") ;
	tprint("              wrote \"%s\"\n", ctx->ctlbuf) ;
	tprint("              read  \"%s\"\n", ctx->rdctlbuf) ;
	rslt = -1 ;
    }

    if (strcmp(ctx->buf, ctx->rdbuf))
    {
	tprint("%s", "loop_clone.2: getmsg: data buffer compare error\n") ;
	tprint("              wrote \"%s\"\n", ctx->buf) ;
	tprint("              read  \"%s\"\n", ctx->rdbuf) ;
	rslt = -1 ;
    }

    if (rslt < 0)
	FAIL("%s", "\n") ;

    tprint("%s", "Re-queued message buffers compare OK\n") ;

	    /********************************
	    *         putmsg/getmsg         *
	    *           Data only           *
	    ********************************/

    tprint("%s", "Use putmsg to send data, use getmsg to read back data\n") ;

    ioc.ic_timout = 10 ;
    ioc.ic_dp	  = NULL;
    ioc.ic_cmd 	  = LOOP_PUTNXT ;		/* use putnxt rather then svcq */
    ioc.ic_len	  = 0 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.1: ioctl LOOP_PUTNXT: %s\n", STRERROR(-rslt)) ;
    }

    ioc.ic_len	  = 0 ;
    rslt = user_ioctl(fd2, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.2: ioctl LOOP_PUTNXT: %s\n", STRERROR(-rslt)) ;
    }

    tprint("%s", "Peek with no message present\n") ;
    do_peek(ctx, fd1, fd2, -1, -1, 0) ;

    tprint("%s", "Data part only\n") ;
    strcpy(ctx->buf, "Test data for putmsg/getmsg") ;
    lgth = strlen(ctx->buf) ;
    do_get_put(ctx, fd1, fd2, -1, lgth, 0) ;
    do_peek(ctx, fd1, fd2, -1, lgth, 0) ;
    do_get_put(ctx, fd1, fd2, -1, lgth, 1) ;
    do_peek(ctx, fd1, fd2, -1, lgth, 1) ;
    do_get_put(ctx, fd1, fd2, -1, 0, 0) ;
    do_peek(ctx, fd1, fd2, -1, 0, 0) ;
    do_get_put(ctx, fd1, fd2, -1, lgth, 0) ;
    do_peek_fcn(ctx, fd1, fd2, -1, lgth, 4, 0, 0) ;

	    /********************************
	    *         Ctl and Data          *
	    ********************************/

    tprint("%s", "Control and data parts\n") ;
    do_get_put(ctx, fd1, fd2, lgth2, lgth, 0) ;
    do_peek(ctx, fd1, fd2, lgth2, lgth, 0) ;
    do_get_put(ctx, fd1, fd2, lgth2, lgth, 1) ;
    do_peek(ctx, fd1, fd2, lgth2, lgth, 1) ;


	    /********************************
	    *           Ctl only            *
	    ********************************/

    tprint("%s", "Control part only\n") ;
    do_get_put(ctx, fd1, fd2, lgth2, -1, 0) ;
    do_peek(ctx, fd1, fd2, lgth2, -1, 0) ;
    do_get_put(ctx, fd1, fd2, lgth2, -1, 1) ;
    do_peek(ctx, fd1, fd2, lgth2, -1, 1) ;



	    /********************************
	    *           Multi-segment       *
	    ********************************/

    tprint("%s", "Three-part message: Data only\n") ;
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;
    ioc.ic_cmd 	  = LOOP_CONCAT ;	/* concatenate messages */
    arg		  = 3 ;			/* concat 3 messages */
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.1: ioctl LOOP_CONCAT: %s\n", STRERROR(-rslt)) ;
    }

    strcpy(ctx->buf, "Test data for putmsg/getmsg") ;
    lgth = strlen(ctx->buf) ;
    do_put(ctx, fd1, -1, lgth, 0) ;
    do_put(ctx, fd1, -1, lgth, 0) ;
    do_put(ctx, fd1, -1, lgth, 0) ;
    /*
     * Read back results
     */
    ctx->rd_ctl.len		= -1 ;
    ctx->rd_ctl.maxlen	= sizeof(ctx->rdctlbuf) ;
    ctx->rd_dta.len		= -1 ;
    ctx->rd_dta.maxlen	= sizeof(ctx->rdbuf) ;
    flags		= 0 ;

    memset(ctx->rdctlbuf, 0, sizeof(ctx->rdctlbuf)) ;
    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;

    flags = MSG_ANY;
    rband = 0 ;
    if (nread_wait_msgs(fd2, 1) != 1)
    {
	FAIL("%s", "loop.2: messages failed to appear\n") ;
    }
    rslt = user_getpmsg(fd2, &ctx->rd_ctl, &ctx->rd_dta, &rband, &flags) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.2: getpmsg: %s\n", STRERROR(-rslt)) ;
    }

    if (ctx->rd_ctl.len != -1)
    {
	FAIL("loop_clone.2: got %d control bytes, expected none\n",
		ctx->rd_ctl.len) ;
    }

    if (ctx->rd_dta.len != 3*lgth)
    {
	FAIL("loop_clone.2: got %d data bytes, expected %d\n",
		ctx->rd_dta.len, 3*lgth) ;
    }

    if (   strncmp(&ctx->rdbuf[0], ctx->buf, lgth)
	|| strncmp(&ctx->rdbuf[lgth], ctx->buf, lgth)
	|| strncmp(&ctx->rdbuf[2*lgth], ctx->buf, lgth)
       )
    {
	tprint("%s", "loop_clone.2: buffer compare error\n") ;
	tprint("Exp: %s%s%s\n", ctx->buf,ctx->buf,ctx->buf) ;
	tprint("Got: %s\n", ctx->rdbuf) ;
	FAIL("%s", "\n") ;
    }
    else
	tprint("%s", "Three-part message: OK\n") ;


    tprint("%s", "Three-part message: Control only\n") ;
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;
    ioc.ic_cmd 	  = LOOP_CONCAT ;	/* concatenate messages */
    arg		  = 3 ;			/* concat 3 messages */
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.1: ioctl LOOP_CONCAT: %s\n", STRERROR(-rslt)) ;
    }

    strcpy(ctx->ctlbuf, "Control message") ;
    lgth2 = strlen(ctx->ctlbuf) ;
    do_put(ctx, fd1, lgth2, -1, 0) ;
    do_put(ctx, fd1, lgth2, -1, 0) ;
    do_put(ctx, fd1, lgth2, -1, 0) ;
    /*
     * Read back results
     */
    ctx->rd_ctl.len		= -1 ;
    ctx->rd_ctl.maxlen	= sizeof(ctx->rdctlbuf) ;
    ctx->rd_dta.len		= -1 ;
    ctx->rd_dta.maxlen	= sizeof(ctx->rdbuf) ;
    flags		= 0 ;

    memset(ctx->rdctlbuf, 0, sizeof(ctx->rdctlbuf)) ;
    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;

    flags = MSG_ANY;
    rband = 0 ;
    if (nread_wait_msgs(fd2, 1) != 1)
    {
	FAIL("%s", "loop.2: message failed to appear\n") ;
    }
    rslt = user_getpmsg(fd2, &ctx->rd_ctl, &ctx->rd_dta, &rband, &flags) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.2: getpmsg: %s\n", STRERROR(-rslt)) ;
    }

    if (ctx->rd_dta.len != -1)
    {
	FAIL("loop_clone.2: got %d data bytes, expected none\n",
		ctx->rd_dta.len) ;
    }

    if (ctx->rd_ctl.len != 3*lgth2)
    {
	FAIL("loop_clone.2: got %d control bytes, expected %d\n",
		ctx->rd_ctl.len, 3*lgth2) ;
    }

    if (   strncmp(&ctx->rdctlbuf[0], ctx->ctlbuf, lgth2)
	|| strncmp(&ctx->rdctlbuf[lgth2], ctx->ctlbuf, lgth2)
	|| strncmp(&ctx->rdctlbuf[2*lgth2], ctx->ctlbuf, lgth2)
       )
    {
	tprint("%s", "loop_clone.2: buffer compare error\n") ;
	tprint("Exp: %s%s%s\n", ctx->ctlbuf,ctx->ctlbuf,ctx->ctlbuf) ;
	tprint("Got: %s\n", ctx->rdctlbuf) ;
	FAIL("%s", "\n") ;
    }
    else
	tprint("%s", "Three-part message: OK\n") ;

    tprint("%s", "Six-part message: Control and data both\n") ;
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;
    ioc.ic_cmd 	  = LOOP_CONCAT ;	/* concatenate messages */
    arg		  = 6 ;			/* concat 3 messages */
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.1: ioctl LOOP_CONCAT: %s\n", STRERROR(-rslt)) ;
    }

    strcpy(ctx->ctlbuf, "Control message") ;
    lgth2 = strlen(ctx->ctlbuf) ;
    strcpy(ctx->buf, "Test data message") ;
    lgth = strlen(ctx->buf) ;
    do_put(ctx, fd1, lgth2, -1, 0) ;
    do_put(ctx, fd1, lgth2, -1, 0) ;
    do_put(ctx, fd1, lgth2, -1, 0) ;
    do_put(ctx, fd1, -1, lgth, 0) ;
    do_put(ctx, fd1, -1, lgth, 0) ;
    do_put(ctx, fd1, -1, lgth, 0) ;
    /*
     * Read back results
     */
    ctx->rd_ctl.len		= -1 ;
    ctx->rd_ctl.maxlen	= sizeof(ctx->rdctlbuf) ;
    ctx->rd_dta.len		= -1 ;
    ctx->rd_dta.maxlen	= sizeof(ctx->rdbuf) ;
    flags		= 0 ;

    memset(ctx->rdctlbuf, 0, sizeof(ctx->rdctlbuf)) ;
    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;

    flags = MSG_ANY;
    rband = 0 ;
    if (nread_wait_msgs(fd2, 1) != 1)
    {
	FAIL("%s", "loop.2: messages failed to appear\n") ;
    }
    rslt = user_getpmsg(fd2, &ctx->rd_ctl, &ctx->rd_dta, &rband, &flags) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.2: getpmsg: %s\n", STRERROR(-rslt)) ;
    }

    if (ctx->rd_dta.len != 3*lgth)
    {
	FAIL("loop_clone.2: got %d data bytes, expected %d\n",
		ctx->rd_dta.len, 3*lgth) ;
    }

    if (ctx->rd_ctl.len != 3*lgth2)
    {
	FAIL("loop_clone.2: got %d control bytes, expected %d\n",
		ctx->rd_ctl.len, 3*lgth2) ;
    }

    if (   strncmp(&ctx->rdctlbuf[0], ctx->ctlbuf, lgth2)
	|| strncmp(&ctx->rdctlbuf[lgth2], ctx->ctlbuf, lgth2)
	|| strncmp(&ctx->rdctlbuf[2*lgth2], ctx->ctlbuf, lgth2)
	|| strncmp(&ctx->rdbuf[0], ctx->buf, lgth)
	|| strncmp(&ctx->rdbuf[lgth], ctx->buf, lgth)
	|| strncmp(&ctx->rdbuf[2*lgth], ctx->buf, lgth)
       )
    {
	tprint("%s", "loop_clone.2: buffer compare error\n") ;
	tprint("CtlExp: %s%s%s\n", ctx->ctlbuf,ctx->ctlbuf,ctx->ctlbuf) ;
	tprint("CtlGot: %s\n", ctx->rdctlbuf) ;
	tprint("DtaExp: %s%s%s\n", ctx->buf,ctx->buf,ctx->buf) ;
	tprint("DtaGot: %s\n", ctx->rdbuf) ;
	FAIL("%s", "\n") ;
    }
    else
	tprint("%s", "Six-part message: OK\n") ;

	    /********************************
	    *         Close Files           * 
	    ********************************/

    tprint("%s", "putmsg_test: closing files\n") ;
    user_close(fd1) ;
    user_close(fd2) ;

} /* putmsg_test */

/************************************************************************
*                            poll_test                                  *
*************************************************************************
*									*
* Test the poll function.						*
*									*
************************************************************************/

char	*poll_events(short events, char *ascii_events)
{
    ascii_events[0] = 0 ;

    if (events & POLLIN) strcat(ascii_events, "POLLIN ") ;
    if (events & POLLRDNORM) strcat(ascii_events, "POLLRDNORM ") ;
    if (events & POLLRDBAND) strcat(ascii_events, "POLLRDBAND ") ;
    if (events & POLLPRI) strcat(ascii_events, "POLLPRI ") ;
    if (events & POLLOUT) strcat(ascii_events, "POLLOUT ") ;
    if (events & POLLWRNORM) strcat(ascii_events, "POLLWRNORM ") ;
    if (events & POLLWRBAND) strcat(ascii_events, "POLLWRBAND ") ;
    if (events & POLLMSG) strcat(ascii_events, "POLLMSG ") ;
    if (events & POLLERR) strcat(ascii_events, "POLLERR ") ;
    if (events & POLLHUP) strcat(ascii_events, "POLLHUP ") ;
    if (events & POLLNVAL) strcat(ascii_events, "POLLNVAL ") ;

    if (ascii_events[0] == 0)
	sprintf(ascii_events, "0x%x", events) ;

    return(ascii_events) ;

} /* poll_events */

void	poll_test(struct test_context *ctx)
{
    int			fd1 ;
    int			fd2 ;
    int			rslt ;
    int			lgth ;
    int			lgth2 ;
    int			flags ;
    int			rband = 0 ;
    int			arg ;
    struct pollfd	fds[4] ;
    struct strioctl	ioc ;
    char		ascii_events[200] ;

    tprint("%s", "Poll function test\n") ;

    tprint("%s", "Poll with no descriptors, just timeout\n") ;
    rslt = user_poll(fds, 0, 50) ;
    if (rslt < 0)
    {
	FAIL("poll: %s\n", STRERROR(-rslt)) ;
    }
    else
	tprint("Poll returned %d\n", rslt) ;


	    /********************************
	    *         Open Files            * 
	    ********************************/

    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_clones\n") ;

	    /********************************
	    *         Normal Data           * 
	    ********************************/

    tprint("%s", "Use putmsg to send normal data, poll for any input\n") ;
    strcpy(ctx->buf, "Test data for putmsg/read") ;
    lgth = strlen(ctx->buf) ;
    ctx->wr_ctl.len	= -1 ;
    ctx->wr_dta.len	= lgth ;
    if (put_msg(fd1, &ctx->wr_ctl, &ctx->wr_dta, 0, MSG_BAND) < 0) FAIL("%s", "put_msg\n") ;

    fds[0].fd		= fd1 ;		/* writing fd */
    fds[0].events	= 0 ;		/* no events */
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].fd		= fd2 ;		/* reading fd */
    fds[1].events	= POLLIN ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	FAIL("poll: %s\n", STRERROR(-rslt)) ;
    }
    else
    if (rslt != 1)
    {
	tprint("Poll returned %d, expected 1\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
	FAIL("%s", "\n") ;
    }
    else
    {
	tprint("Poll returned %d\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
    }

    tprint("%s", "Poll for normal data\n") ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].events	= POLLRDNORM ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	FAIL("poll: %s\n", STRERROR(-rslt)) ;
    }
    else
    if (rslt != 1)
    {
	tprint("Poll returned %d, expected 1\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
	FAIL("%s", "\n") ;
    }
    else
    {
	tprint("Poll returned %d\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
    }

    tprint("%s", "Poll for priority band data\n") ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].events	= POLLRDBAND ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	FAIL("poll: %s\n", STRERROR(-rslt)) ;
    }
    else
    if (rslt != 0)
    {
	tprint("Poll returned %d, expected 0\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
	FAIL("%s", "\n") ;
    }
    else
    {
	tprint("Poll returned %d\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
    }

    tprint("%s", "Poll for high priority message\n") ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].events	= POLLPRI ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	FAIL("poll: %s\n", STRERROR(-rslt)) ;
    }
    else
    if (rslt != 0)
    {
	tprint("Poll returned %d, expected 0\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
	FAIL("%s", "\n") ;
    }
    else
    {
	tprint("Poll returned %d\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
    }

    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;
    rslt = user_read(fd2, ctx->rdbuf, lgth);
    if (rslt < 0)
    {
	FAIL("loop_clone.2: read: %s\n", STRERROR(-rslt)) ;
    }

    if (rslt != lgth)
    {
	FAIL("loop_clone.2:  read returned %d, expected %d\n", rslt, lgth) ;
    }

    if (strcmp(ctx->buf, ctx->rdbuf))
    {
	tprint("%s", "loop_clone.2: read: buffer compare error\n") ;
	tprint("              wrote \"%s\"\n", ctx->buf) ;
	tprint("              read  \"%s\"\n", ctx->rdbuf) ;
    }
    else
	tprint("loop_clone.2: read %d bytes: buffer compared OK\n", rslt) ;

	    /********************************
	    *         Hi-priority           * 
	    ********************************/

    tprint("%s", "Use putmsg to send priority control, poll for any input\n") ;
    strcpy(ctx->ctlbuf, "Control message") ;
    lgth2 = strlen(ctx->ctlbuf) ;
    ctx->wr_ctl.len	= lgth2 ;
    ctx->wr_dta.len	= lgth ;			/* from before */
    if (put_msg(fd1, &ctx->wr_ctl, &ctx->wr_dta, 0, MSG_HIPRI) < 0) FAIL("%s", "put_msg\n") ;

    fds[0].fd		= fd1 ;		/* writing fd */
    fds[0].events	= 0 ;		/* no events */
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].fd		= fd2 ;		/* reading fd */
    fds[1].events	= POLLIN ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	FAIL("poll: %s\n", STRERROR(-rslt)) ;
    }
    else
    if (rslt != 0)
    {
	tprint("Poll returned %d, expected 0\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
	FAIL("%s", "\n") ;
    }
    else
    {
	tprint("Poll returned %d\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
    }

    tprint("%s", "Poll for normal data\n") ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].events	= POLLRDNORM ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100);
    if (rslt < 0)
    {
	FAIL("poll: %s\n", STRERROR(-rslt)) ;
    }
    else
    if (rslt != 0)
    {
	tprint("Poll returned %d, expected 0\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
	FAIL("%s", "\n") ;
    }
    else
    {
	tprint("Poll returned %d\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
    }

    tprint("%s", "Poll for priority band data\n") ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].events	= POLLRDBAND ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	FAIL("poll: %s\n", STRERROR(-rslt)) ;
    }
    else
    if (rslt != 0)
    {
	tprint("Poll returned %d, expected 0\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
	FAIL("%s", "\n") ;
    }
    else
    {
	tprint("Poll returned %d\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
    }

    tprint("%s", "Poll for high priority message\n") ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].events	= POLLPRI ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	FAIL("poll: %s\n", STRERROR(-rslt)) ;
    }
    else if (rslt != 1)
    {
	tprint("Poll returned %d, expected 1\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
	FAIL("%s", "\n") ;
    }
    else
    {
	tprint("Poll returned %d\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
    }

    tprint("%s", "Poll for high priority message and other stream writeable\n") ;
    fds[0].events	= POLLWRNORM ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].events	= POLLPRI ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	FAIL("poll: %s\n", STRERROR(-rslt)) ;
    }
    else
    if (rslt != 2)
    {
	tprint("Poll returned %d, expected 2\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
	FAIL("%s", "\n") ;
    }
    else
    {
	tprint("Poll returned %d\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
    }

    ctx->rd_ctl.len		= -1 ;
    ctx->rd_ctl.maxlen	= sizeof(ctx->rdctlbuf) ;
    ctx->rd_dta.len		= -1 ;
    ctx->rd_dta.maxlen	= sizeof(ctx->rdbuf) ;
    flags		= 0 ;

    memset(ctx->rdctlbuf, 0, sizeof(ctx->rdctlbuf)) ;
    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;

    rslt = user_getpmsg(fd2, &ctx->rd_ctl, &ctx->rd_dta, &rband, &flags) ;
    check_getmsg_rslts(ctx, rslt, &ctx->rd_ctl, &ctx->rd_dta, &flags, 0, rband) ;

    tprint("%s", "Poll for high priority message with delayed delivery\n") ;

    ioc.ic_cmd 	  = LOOP_TIMR ;		/* set timer for queue */
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;

    arg = 50 ;				/* # timer ticks */
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop.1: ioctl LOOP_TIMR: %s\n", STRERROR(-rslt)) ;
    }

    if (put_msg(fd1, &ctx->wr_ctl, &ctx->wr_dta, 0, MSG_HIPRI) < 0) FAIL("%s", "put_msg\n") ;

    fds[0].events	= 0 ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].events	= POLLPRI ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	FAIL("poll: %s\n", STRERROR(-rslt)) ;
    }
    else
    if (rslt != 1)
    {
	tprint("Poll returned %d, expected 1\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
	FAIL("%s", "\n") ;
    }
    else
    {
	tprint("Poll returned %d\n", rslt) ;
	tprint("fds[0].events  = %s\n", poll_events(fds[0].events, ascii_events)) ;
	tprint("fds[0].revents = %s\n", poll_events(fds[0].revents, ascii_events)) ;
	tprint("fds[1].events  = %s\n", poll_events(fds[1].events, ascii_events)) ;
	tprint("fds[1].revents = %s\n", poll_events(fds[1].revents, ascii_events)) ;
    }

    ctx->rd_ctl.len		= -1 ;
    ctx->rd_ctl.maxlen	= sizeof(ctx->rdctlbuf) ;
    ctx->rd_dta.len		= -1 ;
    ctx->rd_dta.maxlen	= sizeof(ctx->rdbuf) ;
    flags		= 0 ;

    memset(ctx->rdctlbuf, 0, sizeof(ctx->rdctlbuf)) ;
    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;

    rslt = user_getpmsg(fd2, &ctx->rd_ctl, &ctx->rd_dta, &rband, &flags) ;
    check_getmsg_rslts(ctx, rslt, &ctx->rd_ctl, &ctx->rd_dta, &flags, 0, rband) ;

	    /********************************
	    *         Close Files           * 
	    ********************************/

    tprint("%s", "poll_test: closing files\n") ;
    user_close(fd1) ;
    user_close(fd2) ;


} /* poll_test */

/************************************************************************
*                             mux_test                                  *
*************************************************************************
*									*
* Test multiplexors.							*
*									*
************************************************************************/
void	mux_test(struct test_context *ctx)
{
    int			fd1 ;
    int			fd2 ;
    int			muxfd1 ;
    int			muxfd2 ;
    int			muxfd3 ;
    int			muxfd4 ;
    int			muxid1 ;
    int			muxid2 ;
    int			muxid3 ;
    int			muxid4 ;
    int			rslt ;
    int			lgth ;
    int			arg ;
    struct strioctl	ioc ;

    tprint("%s", "STREAMS multiplexor test\n") ;


	    /********************************
	    *         Open Files            * 
	    ********************************/

    /*
     * Here is what we are building:
     *
     *     	    muxfd1		   muxfd2	fd1  fd2
     *		      |			      |		 |    |
     *		+-----------+		+-----------+    X    X
     *		|  mini-mux |		|  mini-mux |
     *		+-----------+		+-----------+
     *		      | muxid1		      | muxid2
     *		      |			      |
     *		      | (fd1)		      | (fd2)
     *		+-----------+		+-----------+
     *		|    loop   |<--------->|    loop   |
     *		+-----------+		+-----------+
     *
     * muxfd1 and muxfd2 are the control streams for the multiplxor.
     * When they are closed the whole mux is dismantled.
     *
     * fd1 and fd1 can be closed with no impact on the multiplexor.
     */

    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_clones\n") ;

    tprint("%s", "mux_test: open a mux-clone device\n") ;
    muxfd1 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	FAIL("mux_clone: %s\n", STRERROR(-muxfd1)) ;
    }

    tprint("%s", "mux_test: open another mux-clone device\n") ;
    muxfd2 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd2 < 0)
    {
	FAIL("mux_clone: %s\n", STRERROR(-muxfd2)) ;
    }

    tprint("%s", "mux_test: I_LINK loop driver under mux (muxfd1->fd1)\n") ;
    muxid1 = user_ioctl(muxfd1, I_LINK, fd1) ;	/* link fd1 below muxfd1 */
    if (muxid1 < 0)
    {
	FAIL("mux_clone: I_LINK: %s\n", STRERROR(-muxid1)) ;
    }
    else
	tprint("                   muxid=%d\n", muxid1) ;

    tprint("%s", "mux_test: I_LINK loop driver under mux (muxfd2->fd2)\n") ;
    muxid2 = user_ioctl(muxfd2, I_LINK, fd2) ;	/* link fd2 below muxfd2 */
    if (muxid2 < 0)
    {
	FAIL("mux_clone: I_LINK: %s\n", STRERROR(-muxid2)) ;
    }
    else
	tprint("                   muxid=%d\n", muxid2) ;

    print_stream(muxfd1) ;
    print_stream(muxfd2) ;
    tprint("%s", "mux_test: close files to loop driver (now detached)\n") ;
    user_close(fd1) ;
    user_close(fd2) ;

    tprint("%s", "mux_test: send data down through the mux/loop and read back\n") ;
    strcpy(ctx->buf, "Test data for sending through the mini-mux") ;
    lgth = strlen(ctx->buf) ;
    do_get_put(ctx, muxfd1, muxfd2, -1, lgth, 0) ;

	    /********************************
	    *         Close Files           * 
	    ********************************/

    tprint("%s", "mux_test: closing control streams\n") ;
    user_close(muxfd1) ;
    user_close(muxfd2) ;


    tprint("%s", "Test cascaded multiplexors\n") ;

	    /********************************
	    *         Open Files            * 
	    ********************************/

    /*
     * Here is what we are building:
     *
     *     	    muxfd3		   muxfd4	fd1  fd2 muxfd1 muxfd2
     *		      |			      |		 |    |     |      |
     *		+-----------+		+-----------+    X    X     X      X
     *		|  mini-mux |		|  mini-mux |
     *		+-----------+		+-----------+
     *		      | muxid3		      | muxid4
     *		      |			      |
     *		      |	(muxfd1)	      | (muxfd2)
     *		+-----------+		+-----------+
     *		|  mini-mux |		|  mini-mux |
     *		+-----------+		+-----------+
     *		      | muxid1		      | muxid2
     *		      |			      |
     *		      | (fd1)		      | (fd2)
     *		+-----------+		+-----------+
     *		|    loop   |<--------->|    loop   |
     *		+-----------+		+-----------+
     *
     * muxfd3 and muxfd4 are the control streams for the upper level of
     * the mux.  muxfd1 and muxfd2 are also control streams but for the
     * lower level of the mux.  When muxfd3 closes (or muxfd4) the
     * whole multiplexor is closed.
     *
     * fd1, fd2, muxfd1 and muxfd2 can all be closed with no loss to
     * the multiplexor topology.
     */

    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_clones\n") ;

    tprint("%s", "mux_test: open a mux-clone device\n") ;
    muxfd1 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	FAIL("mux_clone: %s\n", STRERROR(-muxfd1)) ;
    }

    tprint("%s", "mux_test: open another mux-clone device\n") ;
    muxfd2 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd2 < 0)
    {
	FAIL("mux_clone: %s\n", STRERROR(-muxfd2)) ;
    }

    tprint("%s", "mux_test: I_LINK loop driver under mux (muxfd1->fd1)\n") ;
    muxid1 = user_ioctl(muxfd1, I_LINK, fd1) ;	/* link fd1 below muxfd1 */
    if (muxid1 < 0)
    {
	FAIL("mux_clone: I_LINK: %s\n", STRERROR(-muxid1)) ;
    }
    else
	tprint("                   muxid=%d\n", muxid1) ;

    tprint("%s", "mux_test: I_LINK loop driver under mux (muxfd2->fd2)\n") ;
    muxid2 = user_ioctl(muxfd2, I_LINK, fd2) ;	/* link fd2 below muxfd2 */
    if (muxid2 < 0)
    {
	FAIL("mux_clone: I_LINK: %s\n", STRERROR(-muxid2)) ;
    }
    else
	tprint("                   muxid=%d\n", muxid2) ;

    tprint("%s", "mux_test: open a mux-clone device to cascade\n") ;
    muxfd3 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd3 < 0)
    {
	FAIL("mux_clone: %s\n", STRERROR(-muxfd3)) ;
    }

    tprint("%s", "mux_test: open another mux-clone device to cascade\n") ;
    muxfd4 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd4 < 0)
    {
	FAIL("mux_clone: %s\n", STRERROR(-muxfd4)) ;
    }

    tprint("%s", "mux_test: I_LINK mux driver under mux (muxfd3->muxfd1)\n") ;
    muxid3 = user_ioctl(muxfd3, I_LINK, muxfd1) ; /* link muxfd1 below muxfd3 */
    if (muxid3 < 0)
    {
	FAIL("mux_clone: I_LINK: %s\n", STRERROR(-muxid3)) ;
    }
    else
	tprint("                   muxid=%d\n", muxid3) ;

    tprint("%s", "mux_test: I_LINK mux driver under mux (muxfd4->muxfd2)\n") ;
    muxid4 = user_ioctl(muxfd4, I_LINK, muxfd2) ; /* link muxfd2 below muxfd4 */
    if (muxid4 < 0)
    {
	FAIL("mux_clone: I_LINK: %s\n", STRERROR(-muxid4)) ;
    }
    else
	tprint("                   muxid=%d\n", muxid4) ;

    print_stream(muxfd3) ;
    print_stream(muxfd4) ;
    tprint("%s", "mux_test: close files to loop driver (now detached)\n") ;
    user_close(fd1) ;
    user_close(fd2) ;
    tprint("%s", "mux_test: close files to lower mux driver (now detached)\n") ;
    user_close(muxfd1) ;
    user_close(muxfd2) ;

    tprint("%s", "mux_test: send data down through the mux/mux/loop and read back\n") ;
    strcpy(ctx->buf, "Test data for sending through the mini-mux") ;
    lgth = strlen(ctx->buf) ;
    do_get_put(ctx, muxfd3, muxfd4, -1, lgth, 0) ;

	    /********************************
	    *         Close Files           * 
	    ********************************/

    tprint("%s", "mux_test: closing control streams of cascaded muxs\n") ;
    user_close(muxfd3) ;
    user_close(muxfd4) ;


	    /********************************
	    *         Open Files            * 
	    ********************************/

    /*
     * Here is what we are building:
     *
     *     	    muxfd1		         	fd1  fd2
     *		      |			      		 |    |
     *		+-----------------------------------+    X    X
     *		|               mini-mux            |
     *		+-----------------------------------+
     *		      | muxid1		      | muxid2
     *		      |			      |
     *		      | (fd1)		      | (fd2)
     *		+-----------+		+-----------+
     *		|    loop   |<--------->|    loop   |
     *		+-----------+		+-----------+
     *
     * muxfd1 is the control stream for the multiplxor.
     * When they it is closed the whole mux is dismantled.
     *
     * muxid1 and muxid2 are two lowers under the same mux.  We use
     * ioctls to route downstream data from muxfd1 to muxid1 and
     * to route upstream data from muxid2 to muxfd1.
     *
     * fd1 and fd1 can be closed with no impact on the multiplexor.
     */


    tprint("%s", "Test 2 lowers under one control stream\n") ;
    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_clones\n") ;

    tprint("%s", "mux_test: open a mux-clone device\n") ;
    muxfd1 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	FAIL("mux_clone: %s\n", STRERROR(-muxfd1)) ;
    }

    tprint("%s", "mux_test: I_LINK loop driver under mux (muxfd1->fd1)\n") ;
    muxid1 = user_ioctl(muxfd1, I_LINK, fd1) ;	/* link fd1 below muxfd1 */
    if (muxid1 < 0)
    {
	FAIL("mux_clone: I_LINK: %s\n", STRERROR(-muxid1)) ;
    }
    else
	tprint("                   muxid=%d\n", muxid1) ;

    tprint("%s", "mux_test: I_LINK loop driver under mux (muxfd1->fd2)\n") ;
    muxid2 = user_ioctl(muxfd1, I_LINK, fd2) ;	/* link fd2 below muxfd2 */
    if (muxid2 < 0)
    {
	FAIL("mux_clone: I_LINK: %s\n", STRERROR(-muxid2)) ;
    }
    else
	tprint("                   muxid=%d\n", muxid2) ;

    print_stream(muxfd1) ;
    tprint("%s", "mux_test: close files to loop driver (now detached)\n") ;
    user_close(fd1) ;
    user_close(fd2) ;

    ioc.ic_cmd 	  = MINIMUX_DOWN ;	/* set downstream linkage */
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;

    arg = muxid1 ;			/* downstream muxfd1 -> muxid1 */
    rslt = user_ioctl(muxfd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("minimux.1: ioctl MINIMUX_DOWN: %s\n", STRERROR(-rslt)) ;
    }

    ioc.ic_cmd 	  = MINIMUX_UP ;	/* set upstream linkage */
    arg = muxid2 ;			/* upstream muxfd1 <- muxid2 */
    ioc.ic_len	  = sizeof(int) ;
    rslt = user_ioctl(muxfd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("minimux.1: ioctl MINIMUX_UP: %s\n", STRERROR(-rslt)) ;
    }

    tprint("%s", "mux_test: send data down through the mux/loop and read back\n") ;
    strcpy(ctx->buf, "Test data for sending through the mini-mux") ;
    lgth = strlen(ctx->buf) ;
    do_get_put(ctx, muxfd1, muxfd1, -1, lgth, 0) ;

	    /********************************
	    *         Close Files           * 
	    ********************************/

    tprint("%s", "mux_test: closing control streams\n") ;
    user_close(muxfd1) ;


	    /********************************
	    *         Open Files            * 
	    ********************************/

    /*
     * Here is what we are building:
     *
     *     	    muxfd1		   muxfd2	fd1  fd2
     *		      |			      |		 |    |
     *		+-----------+		+-----------+    X    X
     *		|  mini-mux |		|  mini-mux |
     *		+-----------+		+-----------+
     *		      | muxid1		      | muxid2
     *		      |			      |
     *		      | (fd1)		      | (fd2)
     *		+-----------+		+-----------+
     *		|    loop   |<--------->|    loop   |
     *		+-----------+		+-----------+
     *
     * muxfd1 and muxfd2 are the control streams for the multiplxor.
     * When they are closed the whole mux is dismantled.
     *
     * fd1 and fd1 will be held open and reconnected when the multiplexor
     * is unlinked via explicit ioctls.
     */

    tprint("%s", "Test reconnecting lower after explicit I_UNLINK\n") ;
    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_clones\n") ;

    tprint("%s", "mux_test: open a mux-clone device\n") ;
    muxfd1 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	FAIL("mux_clone: %s\n", STRERROR(-muxfd1)) ;
    }

    tprint("%s", "mux_test: open another mux-clone device\n") ;
    muxfd2 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd2 < 0)
    {
	FAIL("mux_clone: %s\n", STRERROR(-muxfd2)) ;
    }

    tprint("%s", "mux_test: I_LINK loop driver under mux (muxfd1->fd1)\n") ;
    muxid1 = user_ioctl(muxfd1, I_LINK, fd1) ;	/* link fd1 below muxfd1 */
    if (muxid1 < 0)
    {
	FAIL("mux_clone: I_LINK: %s\n", STRERROR(-muxid1)) ;
    }
    else
	tprint("                   muxid=%d\n", muxid1) ;

    tprint("%s", "mux_test: I_LINK loop driver under mux (muxfd2->fd2)\n") ;
    muxid2 = user_ioctl(muxfd2, I_LINK, fd2) ;	/* link fd2 below muxfd2 */
    if (muxid2 < 0)
    {
	FAIL("mux_clone: I_LINK: %s\n", STRERROR(-muxid2)) ;
    }
    else
	tprint("                   muxid=%d\n", muxid2) ;

    print_stream(muxfd1) ;
    print_stream(muxfd2) ;
    tprint("%s", "mux_test: leave open files to loop driver\n") ;

    tprint("%s", "mux_test: send data down through the mux/loop and read back\n") ;
    strcpy(ctx->buf, "Test data for sending through the mini-mux") ;
    lgth = strlen(ctx->buf) ;
    do_get_put(ctx, muxfd1, muxfd2, -1, lgth, 0) ;

    tprint("%s", "mux_test: I_UNLINK muxid1 from muxfd1\n") ;
    rslt = user_ioctl(muxfd1, I_UNLINK, muxid1) ;
    if (rslt < 0)
    {
	FAIL("minimux: I_UNLINK: %s\n", STRERROR(-rslt)) ;
    }

    tprint("%s", "mux_test: I_UNLINK muxid2 from muxfd2\n") ;
    rslt = user_ioctl(muxfd2, I_UNLINK, muxid2) ;
    if (rslt < 0)
    {
	FAIL("minimux: I_UNLINK: %s\n", STRERROR(-rslt)) ;
    }

    tprint("%s", "mux_test: send data down through the loop and read back\n") ;
    do_get_put(ctx, fd1, fd2, -1, lgth, 0) ;

	    /********************************
	    *         Close Files           * 
	    ********************************/

    tprint("%s", "mux_test: close control streams\n") ;
    user_close(muxfd1) ;
    user_close(muxfd2) ;
    tprint("%s", "mux_test: send data down through the loop and read back\n") ;
    do_get_put(ctx, fd1, fd2, -1, lgth, 0) ;
    user_close(fd1) ;
    user_close(fd2) ;


	    /********************************
	    *         Open Files            * 
	    ********************************/

    /*
     * Here is what we are building:
     *
     *     	    muxfd1		   muxfd2	fd1  fd2
     *		      |			      |		 |    |
     *		+-----------+		+-----------+    X    X
     *		|  mini-mux |		|  mini-mux |
     *		+-----------+		+-----------+
     *		      | muxid1		      | muxid2
     *		      |			      |
     *		      | (fd1)		      | (fd2)
     *		+-----------+		+-----------+
     *		|    loop   |<--------->|    loop   |
     *		+-----------+		+-----------+
     *
     * muxfd1 and muxfd2 are the control streams for the multiplxor.
     *
     * fd1 and fd1 can be closed with no impact on the multiplexor.
     * Since we used I_PLINK we can also close muxfd1 and muxfd2
     * and the mux will stay together.  However, when we do that
     * the memory stays allocated and we can't reach the streams
     * anymore due to the clone opens.
     */

    tprint("%s", "Test I_PLINK/I_PUNLINK\n") ;
    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_clones\n") ;

    tprint("%s", "mux_test: open minor 1 of mux device\n") ;
    muxfd1 = user_open(MUX_1, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	FAIL("mux_clone: %s\n", STRERROR(-muxfd1)) ;
    }

    tprint("%s", "mux_test: open a mux-clone device\n") ;
    muxfd2 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd2 < 0)
    {
	FAIL("mux_clone: %s\n", STRERROR(-muxfd2)) ;
    }

    tprint("%s", "mux_test: I_PLINK loop driver under mux (muxfd1->fd1)\n") ;
    muxid1 = user_ioctl(muxfd1, I_PLINK, fd1) ;	/* link fd1 below muxfd1 */
    if (muxid1 < 0)
    {
	FAIL("mux_clone: I_PLINK: %s\n", STRERROR(-muxid1)) ;
    }
    else
	tprint("                   muxid=%d\n", muxid1) ;

    tprint("%s", "mux_test: I_PLINK loop driver under mux (muxfd2->fd2)\n") ;
    muxid2 = user_ioctl(muxfd2, I_PLINK, fd2) ;	/* link fd2 below muxfd2 */
    if (muxid2 < 0)
    {
	FAIL("mux_clone: I_PLINK: %s\n", STRERROR(-muxid2)) ;
    }
    else
	tprint("                   muxid=%d\n", muxid2) ;

    print_stream(muxfd1) ;
    print_stream(muxfd2) ;
    tprint("%s", "mux_test: leave open files to loop driver\n") ;

    tprint("%s", "mux_test: send data down through the mux/loop and read back\n") ;
    strcpy(ctx->buf, "Test data for sending through the mini-mux") ;
    lgth = strlen(ctx->buf) ;
    do_get_put(ctx, muxfd1, muxfd2, -1, lgth, 0) ;

    tprint("%s", "mux_test: close muxfd1 \n") ;
    user_close(muxfd1) ;
    tprint("%s", "mux_test: re-open muxfd1 to minor 1\n") ;
    muxfd1 = user_open(MUX_1, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	FAIL("mux_clone: %s\n", STRERROR(-muxfd1)) ;
    }

    tprint("%s", "mux_test: I_PUNLINK muxid1 from muxfd1\n") ;
    print_stream(muxfd1) ;
    rslt = user_ioctl(muxfd1, I_PUNLINK, muxid1) ;
    if (rslt < 0)
    {
	FAIL("minimux: I_PUNLINK: %s\n", STRERROR(-rslt)) ;
    }

    tprint("%s", "mux_test: stream linkage after I_PUNLINK\n") ;
    print_stream(muxfd1) ;

    tprint("%s", "mux_test: I_PUNLINK muxid2 from muxfd2\n") ;
    print_stream(muxfd2) ;
    rslt = user_ioctl(muxfd2, I_PUNLINK, muxid2) ;
    if (rslt < 0)
    {
	FAIL("minimux: I_PUNLINK: %s\n", STRERROR(-rslt)) ;
    }

    tprint("%s", "mux_test: stream linkage after I_PUNLINK\n") ;
    print_stream(muxfd2) ;

    tprint("%s", "mux_test: send data down through the loop and read back\n") ;
    do_get_put(ctx, fd1, fd2, -1, lgth, 0) ;

	    /********************************
	    *         Close Files           * 
	    ********************************/

    tprint("%s", "mux_test: close control streams\n") ;
    user_close(muxfd1) ;
    user_close(muxfd2) ;
    tprint("%s", "mux_test: send data down through the loop and read back\n") ;
    do_get_put(ctx, fd1, fd2, -1, lgth, 0) ;
    user_close(fd1) ;
    user_close(fd2) ;


	    /********************************
	    *         Open Files            * 
	    ********************************/

    /*
     * Here is what we are building:
     *
     *     	    muxfd1		   muxfd2	fd1  fd2
     *		      |			      |		 |    |
     *		+-----------+		+-----------+    X    X
     *		|  mini-mux |		|  mini-mux |
     *		+-----------+		+-----------+
     *		      | muxid1		      | muxid2
     *		      |			      |
     *		      | (fd1)		      | (fd2)
     *		+-----------+		+-----------+
     *		|    loop   |<--------->|    loop   |
     *		+-----------+		+-----------+
     *
     * muxfd1 and muxfd2 are the control streams for the multiplxor.
     *
     * fd1 and fd1 can be closed with no impact on the multiplexor.
     * Since we used I_PLINK we can also close muxfd1 and muxfd2
     * and the mux will stay together.  We then re-open muxfd1 and
     * muxfd2.  We should re-attach to the same streams as before
     * since the stdata structures get allocated by major/minor.
     */

    tprint("%s", "Test I_PLINK/I_PUNLINK with re-open\n") ;
    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_clones\n") ;

    tprint("%s", "mux_test: open minimux.1\n") ;
    muxfd1 = user_open(MUX_1, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	FAIL(MUX_1 ": %s\n", STRERROR(-muxfd1)) ;
    }

    tprint("%s", "mux_test: open minimux.2\n") ;
    muxfd2 = user_open(MUX_2, O_RDWR, 0) ;
    if (muxfd2 < 0)
    {
	FAIL(MUX_2 ": %s\n", STRERROR(-muxfd2)) ;
    }

    tprint("%s", "mux_test: I_PLINK loop driver under mux (muxfd1->fd1)\n") ;
    muxid1 = user_ioctl(muxfd1, I_PLINK, fd1) ;	/* link fd1 below muxfd1 */
    if (muxid1 < 0)
    {
	FAIL("mux_clone: I_PLINK: %s\n", STRERROR(-muxid1)) ;
    }
    else
	tprint("                   muxid=%d\n", muxid1) ;

    tprint("%s", "mux_test: I_PLINK loop driver under mux (muxfd2->fd2)\n") ;
    muxid2 = user_ioctl(muxfd2, I_PLINK, fd2) ;	/* link fd2 below muxfd2 */
    if (muxid2 < 0)
    {
	FAIL("mux_clone: I_PLINK: %s\n", STRERROR(-muxid2)) ;
    }
    else
	tprint("                   muxid=%d\n", muxid2) ;

    print_stream(muxfd1) ;
    print_stream(muxfd2) ;
    tprint("%s", "mux_test: leave open files to loop driver\n") ;

    tprint("%s", "mux_test: send data down through the mux/loop and read back\n") ;
    strcpy(ctx->buf, "Test data for sending through the mini-mux") ;
    lgth = strlen(ctx->buf) ;
    do_get_put(ctx, muxfd1, muxfd2, -1, lgth, 0) ;

    tprint("%s", "mux_test: close mux streams\n") ;

    user_close(muxfd1) ;
    user_close(muxfd2) ;

    tprint("%s", "mux_test: re-open first mux-clone stream\n") ;
    tprint("%s", "mux_test: re-open minimux.1\n") ;
    muxfd1 = user_open(MUX_1, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	FAIL(MUX_1 ": %s\n", STRERROR(-muxfd1)) ;
    }

    tprint("%s", "mux_test: re-open minimux.2\n") ;
    muxfd2 = user_open(MUX_2, O_RDWR, 0) ;
    if (muxfd2 < 0)
    {
	FAIL(MUX_2 ": %s\n", STRERROR(-muxfd2)) ;
    }

    tprint("%s", "mux_test: print mux topology\n") ;
    print_stream(muxfd1) ;
    print_stream(muxfd2) ;

    tprint("%s", "mux_test: "
    	  "send data down through the re-opened mux/loop and read back\n") ;
    strcpy(ctx->buf, "Test data for sending through the re-opened mini-mux") ;
    lgth = strlen(ctx->buf) ;
    do_get_put(ctx, muxfd1, muxfd2, -1, lgth, 0) ;

    tprint("%s", "mux_test: I_PUNLINK muxid1 from muxfd1\n") ;
    rslt = user_ioctl(muxfd1, I_PUNLINK, muxid1) ;
    if (rslt < 0)
    {
	FAIL("minimux: I_PUNLINK: %s\n", STRERROR(-rslt)) ;
    }

    tprint("%s", "mux_test: I_PUNLINK muxid2 from muxfd2\n") ;
    rslt = user_ioctl(muxfd2, I_PUNLINK, muxid2) ;
    if (rslt < 0)
    {
	FAIL("minimux: I_PUNLINK: %s\n", STRERROR(-rslt)) ;
    }

    tprint("%s", "mux_test: send data down through the loop and read back\n") ;
    do_get_put(ctx, fd1, fd2, -1, lgth, 0) ;

	    /********************************
	    *         Close Files           * 
	    ********************************/

    tprint("%s", "mux_test: close control streams\n") ;
    user_close(muxfd1) ;
    user_close(muxfd2) ;
    tprint("%s", "mux_test: send data down through the loop and read back\n") ;
    do_get_put(ctx, fd1, fd2, -1, lgth, 0) ;
    user_close(fd1) ;
    user_close(fd2) ;

	    /********************************
	    *         Open Files            * 
	    ********************************/

    /*
     * Here is what we are building:
     *
     *     	    muxfd1		   muxfd2	fd1  fd2
     *		      |			      |		 |    |
     *		+-----------+		+-----------+    X    X
     *		|  mini-mux |		|  mini-mux |
     *		+-----------+		+-----------+
     *		      | muxid1		      | muxid2
     *		      |			      |
     *		      | (fd1)		      | (fd2)
     *		+-----------+		+-----------+
     *		|    loop   |<--------->|    loop   |
     *		+-----------+		+-----------+
     *
     * muxfd1 and muxfd2 are the control streams for the multiplxor.
     *
     * fd1 and fd1 can be closed with no impact on the multiplexor.
     * Since we used I_PLINK we can also close muxfd1 and muxfd2
     * and the mux will stay together.  
     *
     * We then open any old stream of the mini-mux and issue an
     * I_PUNLINK(MUXID_ALL).  This causes the whole multiplexor
     * to be dismantled.
     */

    tprint("%s", "Test I_PLINK/I_PUNLINK MUXID_ALL \n") ;
    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_clones\n") ;

    tprint("%s", "mux_test: open minimux.1\n") ;
    muxfd1 = user_open(MUX_1, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	FAIL(MUX_1 ": %s\n", STRERROR(-muxfd1)) ;
    }

    tprint("%s", "mux_test: open minimux.2\n") ;
    muxfd2 = user_open(MUX_2, O_RDWR, 0) ;
    if (muxfd2 < 0)
    {
	FAIL(MUX_2 ": %s\n", STRERROR(-muxfd2)) ;
    }

    tprint("%s", "mux_test: I_PLINK loop driver under mux (muxfd1->fd1)\n") ;
    muxid1 = user_ioctl(muxfd1, I_PLINK, fd1) ;	/* link fd1 below muxfd1 */
    if (muxid1 < 0)
    {
	FAIL("mux_clone: I_PLINK: %s\n", STRERROR(-muxid1)) ;
    }
    else
	tprint("                   muxid=%d\n", muxid1) ;

    tprint("%s", "mux_test: I_PLINK loop driver under mux (muxfd2->fd2)\n") ;
    muxid2 = user_ioctl(muxfd2, I_PLINK, fd2) ;	/* link fd2 below muxfd2 */
    if (muxid2 < 0)
    {
	FAIL("mux_clone: I_PLINK: %s\n", STRERROR(-muxid2)) ;
    }
    else
	tprint("                   muxid=%d\n", muxid2) ;

    print_stream(muxfd1) ;
    print_stream(muxfd2) ;
    tprint("%s", "mux_test: leave open files to loop driver\n") ;

    tprint("%s", "mux_test: send data down through the mux/loop and read back\n") ;
    strcpy(ctx->buf, "Test data for sending through the mini-mux") ;
    lgth = strlen(ctx->buf) ;
    do_get_put(ctx, muxfd1, muxfd2, -1, lgth, 0) ;

    tprint("%s", "mux_test: close mux streams\n") ;

    user_close(muxfd1) ;
    user_close(muxfd2) ;

    tprint("%s", "mux_test: re-open first mux-clone stream\n") ;
    muxfd1 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	FAIL(MUX_1 ": %s\n", STRERROR(-muxfd1)) ;
    }

    tprint("%s", "mux_test: I_PUNLINK MUXID_ALL from mux-clone stream\n") ;
    rslt = user_ioctl(muxfd1, I_PUNLINK, MUXID_ALL) ;
    if (rslt < 0)
    {
	FAIL("minimux: I_PUNLINK(MUXID_ALL): %s\n", STRERROR(-rslt)) ;
    }

	    /********************************
	    *         Close Files           * 
	    ********************************/

    tprint("%s", "mux_test: close control streams\n") ;
    user_close(muxfd1) ;
    tprint("%s", "mux_test: send data down through the loop and read back\n") ;
    do_get_put(ctx, fd1, fd2, -1, lgth, 0) ;
    user_close(fd1) ;
    user_close(fd2) ;


} /* mux_test */

/************************************************************************
*                           clone_test                                  *
*************************************************************************
*									*
* Test opening and closing clone drivers in which some minors have	*
* visible directory entries and others don't.				*
*									*
************************************************************************/
void	clone_test(struct test_context *ctx)
{
    int			fd1 ;
    int			fd2 ;
    int			fd3 ;
    int			fd4 ;
    int			fd5 ;
    int			rslt ;

    tprint("%s", "clone open test\n") ;

#ifdef DIRECT_USER
    tprint("Directory listing at start of test\n\n") ;
    user_print_dir(NULL, USR_PRNT_INODE) ;
#endif

    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_clones\n") ;

    rslt = open_clones(&fd3, &fd4) ;
    if (rslt < 0) FAIL("%s", "open_clones\n") ;

    fd5 = user_open(LOOP_1, O_RDWR, 0) ;
    if (fd5 < 0) FAIL("%s", "open " LOOP_1 "\n") ;

#ifdef DIRECT_USER
    tprint("Directory listing after opens\n\n") ;
    user_print_dir(NULL, USR_PRNT_INODE) ;
    user_print_inodes() ;
#endif

    user_close(fd1) ;
    user_close(fd2) ;
    user_close(fd3) ;
    user_close(fd4) ;
    user_close(fd5) ;

#ifdef DIRECT_USER
    tprint("Directory listing after closes\n\n") ;
    user_print_dir(NULL, USR_PRNT_INODE) ;
    user_print_inodes() ;
#endif

    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_clones\n") ;

    rslt = open_clones(&fd3, &fd4) ;
    if (rslt < 0) FAIL("%s", "open_clones\n") ;

    fd5 = user_open(LOOP_1, O_RDWR, 0) ;
    if (fd5 < 0) FAIL("%s", "open " LOOP_1 "\n") ;

#ifdef DIRECT_USER
    tprint("Directory listing after 2nd round of opens\n\n") ;
    user_print_dir(NULL, USR_PRNT_INODE) ;
    user_print_inodes() ;
#endif

    user_close(fd1) ;
    user_close(fd2) ;
    user_close(fd3) ;
    user_close(fd4) ;
    user_close(fd5) ;

#ifdef DIRECT_USER
    tprint("Directory listing after 2nd round of closes\n\n") ;
    user_print_dir(NULL, USR_PRNT_INODE) ;
    user_print_inodes() ;
#endif

}				/* clone_test */

/************************************************************************
*                          bufcall_test                                 *
************************************************************************/
void bufcall_test(struct test_context *ctx)
{
    int			fd1 ;
    int			fd2 ;
    int			rslt ;
    int			lgth ;
    struct strioctl	ioc ;

    tprint("%s", "bufcall test\n") ;

	    /********************************
	    *         Open Files            * 
	    ********************************/

    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_clones\n") ;

	    /********************************
	    *         putmsg/read           *
	    ********************************/

    tprint("%s", "Use putmsg to send data, use read to read back data\n") ;
    memset(ctx->rdctlbuf, 0, sizeof(ctx->rdctlbuf)) ;

    /*
     * Tell the loop driver to use bufcall to drive the service
     * queue enable.
     */
    ioc.ic_timout = 10 ;
    ioc.ic_dp	  = NULL;
    ioc.ic_cmd 	  = LOOP_BUFCALL ;	/* use bufcall to enable svcq */
    ioc.ic_len	  = 0 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.1: ioctl LOOP_BUFCALL: %s\n", STRERROR(-rslt)) ;
    }

    strcpy(ctx->buf, "Test data for bufcall putmsg/read") ;
    lgth = strlen(ctx->buf) ;
    ctx->wr_ctl.len	= -1 ;
    ctx->wr_dta.len	= lgth ;
    if (put_msg(fd1, &ctx->wr_ctl, &ctx->wr_dta, 0, MSG_BAND) < 0) FAIL("%s", "put_msg\n") ;

    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;
#ifndef DIRECT_USER
    if (nread_wait_msgs(fd2, 1) != 1)
    {
	FAIL("%s", "loop.2: message failed to appear\n") ;
    }
#endif
    rslt = user_read(fd2, ctx->rdbuf, lgth);
    if (rslt < 0)
    {
	FAIL("loop_clone.2: read: %s\n", STRERROR(-rslt)) ;
    }

    if (rslt != lgth)
    {
	FAIL("loop_clone.2:  read returned %d, expected %d\n", rslt, lgth) ;
    }

    if (strcmp(ctx->buf, ctx->rdbuf))
    {
	tprint("%s", "loop_clone.2: read: buffer compare error\n") ;
	tprint("              wrote \"%s\"\n", ctx->buf) ;
	tprint("              read  \"%s\"\n", ctx->rdbuf) ;
	FAIL("%s", "\n"); /* missing ? */
    }
    else
	tprint("loop_clone.2: read %d bytes: buffer compared OK\n", rslt) ;




	    /********************************
	    *         Close Files           * 
	    ********************************/

    tprint("%s", "bufcall_test: closing files\n") ;
    user_close(fd1) ;
    user_close(fd2) ;

} /* bufcall_test */

/************************************************************************
*                           sad_vml_test                                *
************************************************************************/
int sad_vml_test(struct test_context *ctx, int fd, struct str_list *list)
{
	int rslt;

	rslt = user_ioctl(fd, SAD_VML, list);
	if (rslt < 0) {
		FAIL("sad_vml_test: SAD_VML ioctl failed: %s\n", STRERROR(-rslt));
	}
	return rslt;
}

/************************************************************************
*                             autopush_test                             *
************************************************************************/
void autopush_test(struct test_context *ctx)
{
	int	fd ;
	int	rslt ;

	print("\nSTREAMS Auto-Push test\n");

	print("autopush_test: open loop.9 w/autopush relay module\n");
	fd = user_open(LOOP_9, O_RDWR, 0);
	if (fd < 0) {
		FAIL("autopush_test: loop.9 open: %s\n", STRERROR(fd));
	}

	rslt = user_ioctl(fd, I_FIND, "relay") ;
	if (rslt < 0)
	{
	    FAIL("loop.9: I_FIND: %s\n", STRERROR(rslt));
	}

	if (rslt > 0)
	    tprint("%s", "Module \"relay\" is present in the stream\n");
	else
	{
      	    FAIL("%s", "Error: module \"relay\" is not present in the stream\n");
	}

	print("autopush_test: closing files\n");
	user_close(fd) ;
}

/************************************************************************
*                             sad_test                                  *
************************************************************************/
void sad_test(struct test_context *ctx)
{
	int fd, fd2;
	int rslt;
	struct str_mlist mlist[4];
	struct str_list list;
	struct strapush apush;

	tprint("%s", "STREAMS Administrative Driver test\n");

	tprint("%s", "sad_test: open a sad device\n");
	fd = user_open(SAD_CLONE, O_RDWR, 0);
	if (fd < 0) {
		FAIL("sad_test: SAD driver open: %s\n", STRERROR(-fd));
	}


	tprint("%s", "sad_test: Testing SAD_VML IOCTL\n");

	tprint("%s", "sad_test: Testing SAD_VML ioctl (a bad module)\n");
	strncpy(mlist[0].l_name, "nosuchmodule", sizeof(mlist[0].l_name));
	list.sl_nmods = 1;
	list.sl_modlist = mlist;
	rslt = sad_vml_test(ctx, fd, &list);
	if (rslt == 0) {
		tprint("%s", "sad_test: SAD says a module "
		      "named \"nosuchmodule\" exists.\n");
		tprint("%s", "sad_test: Does it?\n");
		FAIL("%s", "\n");
	}

	tprint("%s", "sad_test: Testing SAD_VML ioctl (a good and a bad module)\n");
	strncpy(mlist[0].l_name, "relay", sizeof(mlist[0].l_name));
	strncpy(mlist[1].l_name, "nosuchmodule", sizeof(mlist[1].l_name));
	list.sl_nmods = 2;
	rslt = sad_vml_test(ctx, fd, &list);
	if (rslt == 0) {
		tprint("%s", "sad_test: SAD says modules"
		      " \"relay\" and \"nosuchmodule\" exists.\n");
		tprint("%s", "sad_test: Do they both exist?\n");
		FAIL("%s", "\n");
	}

	tprint("%s", "sad_test: Testing SAD_VML ioctl (two good modules)\n");
	strncpy(mlist[1].l_name, "relay2", sizeof(mlist[1].l_name));
	rslt = sad_vml_test(ctx, fd, &list);
	if (rslt == 1) {
		tprint("%s", "sad_test: SAD says modules"
		      " \"relay\" and \"relay2\" do not both exist.\n");
		tprint("%s", "sad_test: Shouldn't they both exist?\n");
		FAIL("%s", "\n");
	}

	tprint("%s", "sad_test: SAD_VML IOCTL tests passed\n");


	tprint("%s", "sad_test: Testing SAD_GAP and SAD_SAP IOCTLs\n");

	/* Check that autopush is unconfigured */
	apush.sap_major = SLOOP__CMAJOR_0;
	apush.sap_minor = 1;
	rslt = user_ioctl(fd, SAD_GAP, &apush);
	if (rslt >= 0) {
		tprint("%s", "sad_test: SAD says autopush is configured "
		      "for loop minor 1.\n");
		tprint("%s", "sad_test: Shouldn't it be unconfigured?\n");
		FAIL("%s", "\n");
	}
#ifdef DIRECT_USER
	if (-rslt == ENODEV)
		tprint("sad_test: Autopush is unconfigured for loop minor 1.\n");
	else {
		FAIL("sad_test: SAD_GAP ioctl failed: %s\n", STRERROR(-rslt));
	}
#else
	if (errno == ENODEV)
		tprint("%s", "sad_test: Autopush is unconfigured for loop minor 1.\n");
	else {
		FAIL("sad_test: SAD_GAP ioctl failed: %s\n", STRERROR(errno));
	}
#endif

	/* Configure autopush */
	apush.sap_cmd = SAP_ONE;
	apush.sap_major = SLOOP__CMAJOR_0;
	apush.sap_minor = 1;
	apush.sap_npush = 2;
	strncpy(apush.sap_list[0], "relay", sizeof(apush.sap_list[0]));
	strncpy(apush.sap_list[1], "relay2", sizeof(apush.sap_list[0]));
	rslt = user_ioctl(fd, SAD_SAP, &apush);
	if (rslt < 0) {
		FAIL("sad_test: SAD_SAP ioctl failed: %s\n", STRERROR(-rslt));
	}

	/* Test autopush */
	fd2 = user_open(LOOP_1, O_RDWR, 0);
	if (fd2 < 0) {
		FAIL("sad_test: loop driver open: %s\n", STRERROR(-fd));
	}
	list.sl_nmods = 4;
	rslt = user_ioctl(fd2, I_LIST, &list);
	if (rslt < 0) {
		FAIL("sad_test: I_LIST: %s\n", STRERROR(-rslt));
	}
	if (list.sl_nmods != 3) {
		tprint("sad_test: loop driver open autopushed %d modules.\n", list.sl_nmods);
		tprint("%s", "sad_test: Expected 2 autopushed modules.\n");
		FAIL("%s", "\n");
	}
	if (strcmp(mlist[0].l_name, "relay2")) {
		tprint("sad_test: Second autopushed module is \"%s\".\n",
		      mlist[0].l_name);
		tprint("%s", "sad_test: Expected \"relay2\".\n");
		FAIL("%s", "\n");
	}
	if (strcmp(mlist[1].l_name, "relay")) {
		tprint("sad_test: First autopushed module is \"%s\".\n",
		      mlist[0].l_name);
		tprint("%s", "sad_test: Expected \"relay\".\n");
		FAIL("%s", "\n");
	}
	user_close(fd2);
	tprint("%s", "sad_test: Autopush tested OK.\n");

	/* Remove autopush configuration */
	apush.sap_cmd = SAP_CLEAR;
	apush.sap_major = SLOOP__CMAJOR_0;
	apush.sap_minor = 1;
	rslt = user_ioctl(fd, SAD_SAP, &apush);
	if (rslt < 0) {
		FAIL("sad_test: SAD_SAP ioctl failed: %s\n", STRERROR(-rslt));
	}
	
	/* Check that autopush configuration was removed */
	apush.sap_major = SLOOP__CMAJOR_0;
	apush.sap_minor = 1;
	rslt = user_ioctl(fd, SAD_GAP, &apush);
	if (rslt >= 0) {
		tprint("%s", "sad_test: SAD says autopush is configured "
		      "for loop minor 1.\n");
		tprint("%s", "sad_test: But I just cleared the configuration\n");
		FAIL("%s", "\n");
	}
#ifdef DIRECT_USER
	if (-rslt != ENODEV) {
		FAIL("sad_test: SAD_GAP ioctl failed: %s\n", STRERROR(-rslt));
	}
#else
	if (errno != ENODEV) {
		FAIL("sad_test: SAD_GAP ioctl failed: %s\n", STRERROR(errno));
	}
#endif

	tprint("%s", "sad_test: SAD_GAP and SAD_SAP IOCTL tests passed\n");

	tprint("%s", "sad_test: closing files\n");
	user_close(fd);
}

/************************************************************************
*                             fifo_test                                 *
************************************************************************/
void fifo_test(struct test_context *ctx)
{
    int fd[2];
    int i;
    struct user_stat stat;

    tprint("%s", "STREAMS-based FIFO/pipe test\n");
    
    for (i = 0;  i < 2;  i++) {
	fd[i] = user_open(CLONE_FIFO, O_RDWR, 0);
	if (fd[i] < 0) {
	    FAIL("fifo_test: fifo open: %s\n", STRERROR(errno));
	}
	user_fstat( fd[i], &stat );
	tprint("fifo_test: open(\"%s\",O_RDWR) #%d: fd %d mode 0%o dev 0x%x\n",
	      CLONE_FIFO, i+1, fd[i],
	      (int)stat.st_mode, (int)stat.st_rdev );
    }
    user_close(fd[0]);
    user_close(fd[1]);
    
    for (i = 0;  i < 2;  i++) {
	fd[i] = user_open(FIFO_0, O_RDWR, 0);
	if (fd[i] < 0) {
	    FAIL("fifo_test: fifo open: %s\n", STRERROR(errno));
	}
	user_fstat( fd[i], &stat );
	tprint("fifo_test: open(\"%s\",O_RDWR) #%d: fd %d mode 0%o dev 0x%x\n",
	      FIFO_0, i+1, fd[i],
	      (int)stat.st_mode, (int)stat.st_rdev );
    }
    user_close(fd[0]);
    user_close(fd[1]);
    
    if ((user_pipe(fd)) < 0) {
	FAIL("pipe: %s\n", STRERROR(errno));
    }
    user_ioctl( fd[0], I_PUSH, "pipemod" );

    user_fstat( fd[0], &stat );
    tprint("fifo_test: pipe() 0: fd %d mode 0%o dev 0x%x\n",
	  fd[0], (int)stat.st_mode, (int)stat.st_rdev );
    user_fstat( fd[1], &stat );
    tprint("fifo_test: pipe() 1: fd %d mode 0%o dev 0x%x\n",
	  fd[1], (int)stat.st_mode, (int)stat.st_rdev );

    user_close(fd[0]);
    user_close(fd[1]);
}

/************************************************************************
*                           passfd_test                                 *
************************************************************************/
void passfd_test(struct test_context *ctx)
{
    int             fd[2],
                    sendfd;
    struct strrecvfd recv;
    struct user_stat stat;
    
    tprint("%s", "I_SENDFD/I_RECVFD test\n");
    
    if ((user_pipe(fd)) < 0) {
	FAIL("pipe: %s\n", STRERROR(errno));
    }
    user_fstat( fd[0], &stat );
    tprint("passfd_test: pipe() 0: fd %d mode 0%o dev 0x%x\n",
	  fd[0], (int)stat.st_mode, (int)stat.st_rdev );
    user_fstat( fd[1], &stat );
    tprint("passfd_test: pipe() 1: fd %d mode 0%o dev 0x%x\n",
	  fd[1], (int)stat.st_mode, (int)stat.st_rdev );

    sendfd = user_open(FIFO_0, O_RDWR, 0);
    if (sendfd < 0) {
	FAIL("passfd_test: open(\"%s\",O_RDWR) failed: %s\n",
	      FIFO_0, strerror(errno) );
    }
    user_fstat( sendfd, &stat );
    tprint("passfd_test: sending fd %d mode 0%o dev 0x%x\n",
	  sendfd, (int)stat.st_mode, (int)stat.st_rdev );
    
    if (user_ioctl( fd[0], I_SENDFD, sendfd ) < 0) {
	FAIL("ioctl( %d, I_SENDFD, %d ) failed: %s\n",
	      fd[0], sendfd, strerror(errno) );
    }
    sleep(1) ;			/* allow time for service queues */
    tprint("passfd_test: closing sendfd %d\n", sendfd);
    user_close(sendfd);
    tprint("passfd_test: closing pipe fd[0] %d\n", fd[0]);
    user_close(fd[0]);
    
    if (user_ioctl( fd[1], I_RECVFD, &recv ) < 0) {
	FAIL("ioctl( %d, I_RECVFD, ... ) failed: %s\n",
	      fd[1], strerror(errno) );
    }
    user_fstat( recv.fd, &stat );
    tprint("passfd_test: received fd %d mode 0%o dev 0x%x\n",
	  recv.fd, (int)stat.st_mode, (int)stat.st_rdev );

    tprint("passfd_test: closing pipe fd[1] %d\n", fd[1]);
    user_close(fd[1]);
    tprint("passfd_test: closing recv.fd %d\n", recv.fd);
    user_close(recv.fd);
}

/************************************************************************
*                           band_test                                   *
*************************************************************************
*									*
* Test sending messages to different queue bands.			*
*									*
************************************************************************/

#define	_M_DATA		1
#define	_M_PROTO	2
#define	_M_PCPROTO	3

void ck_band(struct test_context *ctx, int fd, int band, int expected, int nmsgs)
{
    int		rslt ;

    if ((rslt = nread_wait_msgs(fd, nmsgs)) < nmsgs)
    {
	FAIL("loop.2: expected < %d message(s) got %d\n", nmsgs, rslt) ;
    }

    rslt = user_ioctl(fd, I_CKBAND, band) ;
    if (rslt < 0)
    {
	FAIL("ck_band(%d): I_CKBAND: %s\n", band, STRERROR(-rslt));
    }

    if (rslt == expected)
    {
	tprint("ck_band(%d): I_CKBAND: got expected result %d\n", band, rslt) ;
	return ;
    }

    FAIL("ck_band(%d): I_CKBAND: expected %d got %d\n", band, expected, rslt) ;
}

void send_band(struct test_context *ctx, int fd, int band, int seq, int mtype)
{
    int		flags ;

    flags = MSG_BAND ;
    if (mtype == _M_PCPROTO)
    {
	flags = MSG_HIPRI ;
	band = 0 ;
    }

    if (mtype == _M_DATA)
    {
	sprintf(ctx->buf, "Seq=%d Band=%d M_DATA", seq, band) ;
	ctx->wr_dta.len = strlen(ctx->buf) ;
	ctx->wr_ctl.len = -1 ;		/* no control msg */
    }
    else
    {
	sprintf(ctx->ctlbuf, "Seq=%d Band=%d M_PCPROTO", seq, band) ;
	ctx->wr_ctl.len = strlen(ctx->ctlbuf) ;
	ctx->wr_dta.len = -1 ;		/* no data msg */
    }

    if (put_msg(fd, &ctx->wr_ctl, &ctx->wr_dta, band, flags) < 0)
	FAIL("%s", "put_msg\n") ;
}

int	receive_band_quiet ;

void receive_band(struct test_context *ctx, int fd, int band, int seq, int mtype)
{
    int			flags = MSG_BAND;
    int			rband = band ;
    int			rslt ;
    int			lgth ;
    char	       *ptr ;
    char	       *mnemonic ;
    int			len ;

    ctx->rd_ctl.len		= -1 ;
    ctx->rd_ctl.maxlen	= sizeof(ctx->rdctlbuf) ;
    ctx->rd_dta.len		= -1 ;
    ctx->rd_dta.maxlen	= sizeof(ctx->rdbuf) ;

    memset(ctx->rdctlbuf, 0, sizeof(ctx->rdctlbuf)) ;
    memset(ctx->rdbuf, 0, sizeof(ctx->rdbuf)) ;

    rslt = user_getpmsg(fd, &ctx->rd_ctl, &ctx->rd_dta, &rband, &flags) ;
    if (rslt < 0)
    {
	FAIL("receive_band(%d): %s\n", band, STRERROR(-rslt)) ;
    }

    if (band != rband)
    {
	FAIL("receive_band: expected msg on band %d, got one on band %d\n",
		band, rband) ;
    }

    if (mtype == _M_PCPROTO && !(flags & MSG_HIPRI))
    {
	FAIL("%s", "receive_band: expected hi-pri msg, got normal-pri instead\n");
    }

    if (mtype != _M_PCPROTO && (flags & MSG_HIPRI))
    {
	FAIL("%s", "receive_band: expected normal-pri msg, got hi-pri instead\n");
    }

    if (mtype == _M_DATA)
    {
	if (ctx->rd_ctl.len > 0)
	{
	    FAIL("%s", "receive_band: expected M_DATA, got M_PROTO instead\n");
	}

	ptr = ctx->rdbuf ;
	len = ctx->rd_dta.len ;
	mnemonic = "M_DATA" ;
    }
    else
    {
	if (ctx->rd_dta.len > 0)
	{
	    FAIL("%s", "receive_band: expected M_PROTO, got M_DATA instead\n");
	}

	ptr = ctx->rdctlbuf ;
	len = ctx->rd_ctl.len ;
	mnemonic = "M_PCPROTO" ;
    }

    sprintf(ctx->buf, "Seq=%d Band=%d %s", seq, band, mnemonic) ;
    lgth = strlen(ctx->buf) ;

    if (len != lgth)
    {
	FAIL("receive_band: expected %d bytes, got %d bytes\n", lgth, len) ;
    }

    if (strcmp(ctx->buf, ptr) != 0)
    {
	tprint("%s", "receive_band: buffer compare error\n") ;
	tprint("              wrote \"%s\"\n", ctx->buf) ;
	tprint("              read  \"%s\"\n", ptr) ;
	FAIL("%s", "\n") ;
    }

    if (!receive_band_quiet)
	tprint("receive_band: OK: %s\n", ptr) ;
}

void band_test(struct test_context *ctx)
{
    int			fd1 ;
    int			fd2 ;
    int			rslt ;
    int			send_seq ;
    int			n_sent ;
    int			i ;
    int			flow_ok ;
    struct strioctl	ioc ;

    tprint("%s", "Queue band test\n") ;

    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_clones\n") ;

    ioc.ic_timout = 10 ;
    ioc.ic_dp	  = NULL;
    ioc.ic_cmd 	  = LOOP_PUTNXT ;  /* use putnext rather then svcq */
    ioc.ic_len	  = 0 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.1: ioctl LOOP_PUTNXT: %s\n", STRERROR(-rslt)) ;
    }

    rslt = user_ioctl(fd2, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.2: ioctl LOOP_PUTNXT: %s\n", STRERROR(-rslt)) ;
    }


    tprint("%s", "band_test: band 0 (easy case)\n") ;
    send_band(ctx, fd1, 0, 1, _M_DATA) ;
    ck_band(ctx, fd2, 0, 1, 1) ;
    receive_band(ctx, fd2, 0, 1, _M_DATA) ;
    ck_band(ctx, fd2, 0, 0, 0) ;

    tprint("%s", "band_test: write band 0, 1; read band 1, 0\n") ;
    send_band(ctx, fd1, 0, 2, _M_DATA) ;
    send_band(ctx, fd1, 1, 3, _M_DATA) ;
    ck_band(ctx, fd2, 1, 1, 2) ;
    receive_band(ctx, fd2, 1, 3, _M_DATA) ;
    ck_band(ctx, fd2, 1, 0, 1) ;
    receive_band(ctx, fd2, 0, 2, _M_DATA) ;

    tprint("%s", "band_test: write band 1, 2, 3; read band 3, 2, 1\n") ;
    send_band(ctx, fd1, 1, 4, _M_DATA) ;
    send_band(ctx, fd1, 2, 5, _M_DATA) ;
    send_band(ctx, fd1, 3, 6, _M_DATA) ;
    ck_band(ctx, fd2, 3, 1, 3) ;
    receive_band(ctx, fd2, 3, 6, _M_DATA) ;
    ck_band(ctx, fd2, 3, 0, 2) ;
    ck_band(ctx, fd2, 2, 1, 2) ;
    receive_band(ctx, fd2, 2, 5, _M_DATA) ;
    ck_band(ctx, fd2, 2, 0, 1) ;
    ck_band(ctx, fd2, 1, 1, 1) ;
    receive_band(ctx, fd2, 1, 4, _M_DATA) ;
    ck_band(ctx, fd2, 1, 0, 0) ;

    tprint("%s", "band_test: write band 1, 3; read band 3, 1\n") ;
    send_band(ctx, fd1, 1, 7, _M_DATA) ;
    send_band(ctx, fd1, 3, 8, _M_DATA) ;
    if ((rslt = nread_wait_msgs(fd2, 2)) != 2)
    {
	FAIL("loop.2: expected < %d message(s) got %d\n", 2, rslt) ;
    }
    receive_band(ctx, fd2, 3, 8, _M_DATA) ;
    receive_band(ctx, fd2, 1, 7, _M_DATA) ;

    tprint("%s", "band_test: write band 1, 3, 1; read band 3, 1, 1\n") ;
    send_band(ctx, fd1, 1, 9, _M_DATA) ;
    send_band(ctx, fd1, 3, 10, _M_DATA) ;
    send_band(ctx, fd1, 1, 11, _M_DATA) ;
    if ((rslt = nread_wait_msgs(fd2, 3)) != 3)
    {
	FAIL("loop.2: expected < %d message(s) got %d\n", 3, rslt) ;
    }
    receive_band(ctx, fd2, 3, 10, _M_DATA) ;
    receive_band(ctx, fd2, 1, 9, _M_DATA) ;
    receive_band(ctx, fd2, 1, 11, _M_DATA) ;

    tprint("%s", "band_test: "
	    "write band 1, 2, M_PCPROTO; read M_PCPROTO, band 2, 1\n");
    send_band(ctx, fd1, 1, 12, _M_DATA) ;
    send_band(ctx, fd1, 2, 13, _M_DATA) ;
    send_band(ctx, fd1, 0, 14, _M_PCPROTO) ;
    if ((rslt = nread_wait_msgs(fd2, 3)) != 3)
    {
	FAIL("loop.2: expected < %d message(s) got %d\n", 3, rslt) ;
    }
    receive_band(ctx, fd2, 0, 14, _M_PCPROTO) ;
    receive_band(ctx, fd2, 2, 13, _M_DATA) ;
    receive_band(ctx, fd2, 1, 12, _M_DATA) ;

    tprint("%s", "band_test: "
	    "fill up band 1, then write band 2; read 2, 1, 1, ...\n");
    send_seq = 10000 ;
    for (n_sent = 0; user_ioctl(fd1, I_CANPUT, 1) == 1 ; n_sent++)
    {
	send_band(ctx, fd1, 1, send_seq + n_sent, _M_DATA) ;
	if (n_sent == 0)
	    set_debug_mask(0x00000000L) ;
    }

    tprint("band_test: band 1 full (%d msgs), write band 2\n", n_sent) ;
    set_debug_mask(debug_mask) ;
    send_band(ctx, fd1, 2, send_seq + n_sent + 1, _M_DATA) ;
    sleep(1) ;				/* allow messages to flow */
    tprint("   %d messages available to read\n", nread_wait_msgs(fd2, 1)) ;
    receive_band(ctx, fd2, 2, send_seq + n_sent + 1, _M_DATA) ;
    for (i = 0; i < n_sent; i++)
    {
	receive_band(ctx, fd2, 1, send_seq + i, _M_DATA) ;
	if (i == 0)
	{
	    set_debug_mask(0x00000000L) ;
	    receive_band_quiet = 1 ;
	}
    }

    tprint("%s", "band_test: wr band 1, fill band 2, wr band 1; read 2,2,... 1\n");
    set_debug_mask(debug_mask) ;
    receive_band_quiet = 0 ;
    send_seq = 20000 ;
    send_band(ctx, fd1, 1, 20, _M_DATA) ;
    for (n_sent = 0; user_ioctl(fd1, I_CANPUT, 2) == 1 ; n_sent++)
    {
	send_band(ctx, fd1, 2, send_seq + n_sent, _M_DATA) ;
	if (n_sent == 0)
	    set_debug_mask(0x00000000L) ;
    }

    tprint("band_test: band 2 full (%d msgs), write band 1\n", n_sent) ;
    send_band(ctx, fd1, 1, 21, _M_DATA) ;
    sleep(1) ;				/* allow messages to flow */
    tprint("   %d messages available to read\n", nread_wait_msgs(fd2, 1)) ;
    for (i = 0; i < n_sent; i++)
    {
	receive_band(ctx, fd2, 2, send_seq + i, _M_DATA) ;
	if (i == 0)
	{
	    set_debug_mask(0x00000000L) ;
	    receive_band_quiet = 1 ;
	}
    }
    set_debug_mask(debug_mask) ;
    receive_band_quiet = 0 ;
    receive_band(ctx, fd2, 1, 20, _M_DATA) ;
    receive_band(ctx, fd2, 1, 21, _M_DATA) ;


    tprint("%s", "band_test: wr band 1, fill band 2, wr band 1; test backenable\n");
    set_debug_mask(debug_mask) ;
    receive_band_quiet = 0 ;
    send_seq = 30000 ;
    send_band(ctx, fd1, 1, 22, _M_DATA) ;
    for (n_sent = 0; user_ioctl(fd1, I_CANPUT, 2) == 1 ; n_sent++)
    {
	send_band(ctx, fd1, 2, send_seq + n_sent, _M_DATA) ;
	if (n_sent == 0)
	    set_debug_mask(0x00000000L) ;
    }

    tprint("band_test: band 2 full (%d msgs), write band 1\n", n_sent) ;
    send_band(ctx, fd1, 1, 23, _M_DATA) ;
    sleep(1) ;				/* allow messages to flow */
    tprint("   %d messages available to read\n", nread_wait_msgs(fd2, 1)) ;
    flow_ok = 0 ;
    for (i = 0; i < n_sent; i++)
    {
	receive_band(ctx, fd2, 2, send_seq + i, _M_DATA) ;
	if (i == 0)
	{
	    set_debug_mask(0x00000000L) ;
	    receive_band_quiet = 1 ;
	}
	if (!flow_ok && user_ioctl(fd1, I_CANPUT, 2) == 1)
	{
	    flow_ok = 1 ;
	    tprint("band_test: backpressure relieved at %d messages queued\n",
		    n_sent - i) ;
	}
    }

    set_debug_mask(debug_mask) ;
    receive_band_quiet = 0 ;
    receive_band(ctx, fd2, 1, 22, _M_DATA) ;
    receive_band(ctx, fd2, 1, 23, _M_DATA) ;

    if (!flow_ok)
    {
	FAIL("%s", "band_test: all messages read but backpressure still in place\n");
    }


    user_close(fd1);
    user_close(fd2);

    tprint("%s", "End of band test\n") ;
}

/************************************************************************
*                          flush_test                                   *
************************************************************************/
void flush_test(struct test_context *ctx)
{
    int			fd1 ;
    int			fd2 ;
    int			rslt ;
    struct strioctl	ioc ;
    bandinfo_t		bi ;

    tprint("%s", "Queue flushing test\n") ;

    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) FAIL("%s", "open_clones\n") ;

    ioc.ic_timout = 10 ;
    ioc.ic_dp	  = NULL;
    ioc.ic_cmd 	  = LOOP_PUTNXT ;  /* use putnext rather then svcq */
    ioc.ic_len	  = 0 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.1: ioctl LOOP_PUTNXT: %s\n", STRERROR(-rslt)) ;
    }

    rslt = user_ioctl(fd2, I_STR, &ioc) ;
    if (rslt < 0)
    {
	FAIL("loop_clone.2: ioctl LOOP_PUTNXT: %s\n", STRERROR(-rslt)) ;
    }

    tprint("%s", "flush_test: simple band 0 data\n") ;
    send_band(ctx, fd1, 0, 1, _M_DATA) ;
    send_band(ctx, fd1, 0, 2, _M_DATA) ;

    rslt = nread_wait_msgs(fd2, 2) ;
    if (rslt != 2)
    {
	FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 2) ;
    }

    rslt = user_ioctl(fd2, I_FLUSH, FLUSHRW) ;
    if (rslt < 0)
    {
	FAIL("loop.2: I_FLUSH: %s\n", STRERROR(-rslt)) ;
    }

    rslt = flush_wait(fd2);
    if (rslt != 0)
    {
	FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 0) ;
    }

    tprint("%s", "flush_test: simple band 0 data -- OK\n") ;


    tprint("%s", "flush_test: band 1 data via I_FLUSH\n") ;

    send_band(ctx, fd1, 1, 3, _M_DATA) ;
    send_band(ctx, fd1, 1, 4, _M_DATA) ;

    rslt = nread_wait_msgs(fd2, 2) ;
    if (rslt != 2)
    {
	FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 2) ;
    }

    rslt = user_ioctl(fd2, I_FLUSH, FLUSHRW) ;
    if (rslt < 0)
    {
	FAIL("loop.2: I_FLUSH: %s\n", STRERROR(-rslt)) ;
    }

    rslt = flush_wait(fd2);
    if (rslt != 0)
    {
	FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 0) ;
    }

    tprint("%s", "flush_test: band 1 data via I_FLUSH -- OK\n") ;

    tprint("%s", "flush_test: band 1 data via I_FLUSHBAND\n") ;

    send_band(ctx, fd1, 1, 5, _M_DATA) ;
    send_band(ctx, fd1, 1, 6, _M_DATA) ;

    rslt = nread_wait_msgs(fd2, 2) ;
    if (rslt != 2)
    {
	FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 2) ;
    }

    bi.bi_pri = 1 ;			/* flush band 1 */
    bi.bi_flag = FLUSHRW ;		/* flush both read/write */

    rslt = user_ioctl(fd2, I_FLUSHBAND, &bi) ;
    if (rslt < 0)
    {
	FAIL("loop.2: I_FLUSHBAND: %s\n", STRERROR(-rslt)) ;
    }

    rslt = flush_wait(fd2) ;
    if (rslt != 0)
    {
	FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 0) ;
    }

    tprint("%s", "flush_test: band 1 data via I_FLUSHBAND -- OK\n") ;


    tprint("%s", "flush_test: band 2 data via I_FLUSHBAND w/band 1 data present\n") ;

    send_band(ctx, fd1, 1, 7, _M_DATA) ;
    send_band(ctx, fd1, 1, 8, _M_DATA) ;
    send_band(ctx, fd1, 2, 9, _M_DATA) ;
    send_band(ctx, fd1, 2, 10, _M_DATA) ;

    rslt = nread_wait_msgs(fd2, 4) ;
    if (rslt != 4)
    {
	FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 4) ;
    }

    bi.bi_pri = 2 ;			/* flush band 2 */
    bi.bi_flag = FLUSHRW ;		/* flush both read/write */

    rslt = user_ioctl(fd2, I_FLUSHBAND, &bi) ;
    if (rslt < 0)
    {
	FAIL("loop.2: I_FLUSHBAND: %s\n", STRERROR(-rslt)) ;
    }

    receive_band(ctx, fd2, 1, 7, _M_DATA) ;
    receive_band(ctx, fd2, 1, 8, _M_DATA) ;

    rslt = flush_wait(fd2) ;
    if (rslt != 0)
    {
	FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 0) ;
    }

    tprint("%s", "flush_test: "
	    "band 2 data via I_FLUSHBAND w/band 1 data present -- OK\n") ;

    /*
     * I_FLUSH does flush of data only.  Data includes M_DATA, M_PROTO
     * and M_PCPROTO.  So anything that we can send from user level
     * gets flushed.
     */
    tprint("%s", "flush_test: band 0 data + M_PCPROTO\n") ;
    send_band(ctx, fd1, 0, 11, _M_DATA) ;
    send_band(ctx, fd1, 0, 12, _M_DATA) ;
    send_band(ctx, fd1, 0, 13, _M_PCPROTO) ;

    rslt = nread_wait_msgs(fd2, 3) ;
    if (rslt != 3)
    {
	FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 3) ;
    }

    rslt = user_ioctl(fd2, I_FLUSH, FLUSHRW) ;
    if (rslt < 0)
    {
	FAIL("loop.2: I_FLUSH: %s\n", STRERROR(-rslt)) ;
    }

    rslt = flush_wait(fd2);
    if (rslt != 0)
    {
	FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 0) ;
    }

    tprint("%s", "flush_test: band 0 data + M_PCPROTO -- OK\n") ;


    /*
     * I_FLUSHBAND on band 0 flushes "ordinary" messages only.  This
     * will leave an M_PCPROTO at the head of the queue.
     */
    tprint("%s", "flush_test: band 0 data + M_PCPROTO via I_FLUSHBAND\n") ;
    send_band(ctx, fd1, 0, 14, _M_DATA) ;
    send_band(ctx, fd1, 0, 15, _M_DATA) ;
    send_band(ctx, fd1, 0, 16, _M_PCPROTO) ;

    rslt = nread_wait_msgs(fd2, 3);
    if (rslt != 3)
    {
	FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 3) ;
    }

    bi.bi_pri = 0 ;			/* flush band 0 */
    bi.bi_flag = FLUSHRW ;		/* flush both read/write */

    rslt = user_ioctl(fd2, I_FLUSHBAND, &bi) ;
    if (rslt < 0)
    {
	FAIL("loop.2: I_FLUSH: %s\n", STRERROR(-rslt)) ;
    }

    receive_band(ctx, fd2, 0, 16, _M_PCPROTO) ;
    rslt = flush_wait(fd2) ;
    if (rslt != 0)
    {
	FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 0) ;
    }

    tprint("%s", "flush_test: band 0 data + M_PCPROTO via I_FLUSHBAND -- OK\n") ;


    /*
     * Flushing band 1 data should also leave an M_PCPROTO unflushed
     * since it appears as band 0.
     */
    tprint("%s", "flush_test: band 1 data + M_PCPROTO via I_FLUSHBAND\n") ;
    send_band(ctx, fd1, 1, 17, _M_DATA) ;
    send_band(ctx, fd1, 1, 18, _M_DATA) ;
    send_band(ctx, fd1, 0, 19, _M_PCPROTO) ;
    send_band(ctx, fd1, 2, 20, _M_DATA) ;

    rslt = nread_wait_msgs(fd2, 4) ;
    if (rslt != 4)
    {
	FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 4) ;
    }

    bi.bi_pri = 1 ;			/* flush band 1 */
    bi.bi_flag = FLUSHRW ;		/* flush both read/write */

    rslt = user_ioctl(fd2, I_FLUSHBAND, &bi) ;
    if (rslt < 0)
    {
	FAIL("loop.2: I_FLUSH: %s\n", STRERROR(-rslt)) ;
    }

    receive_band(ctx, fd2, 0, 19, _M_PCPROTO) ;
    receive_band(ctx, fd2, 2, 20, _M_DATA) ;
    rslt = flush_wait(fd2) ;
    if (rslt != 0)
    {
	FAIL("loop.2: I_NREAD returned %d, expected %d\n", rslt, 0) ;
    }

    tprint("%s", "flush_test: band 0 data + M_PCPROTO via I_FLUSHBAND -- OK\n") ;


    user_close(fd1);
    user_close(fd2);

    tprint("%s", "End of queue flushing test\n") ;
}

/************************************************************************
*                             pullupmsg_test                            *
*************************************************************************
*									*
* This test just opens a special device file to a streams driver that	*
* runs the test from kernel mode.					*
*									*
************************************************************************/
void pullupmsg_test(struct test_context *ctx)
{
    int		rslt ;

    tprint("%s", "Begin pullupmsg test\n") ;
    rslt = user_open(PUTST, O_RDWR, 0) ;
    if (rslt < 0)
    {
	FAIL("putst: %s\n", STRERROR(-rslt)) ;
    }

    user_close(rslt) ;
    tprint("%s", "pullupmsg test OK\n") ;
}

/************************************************************************
*                             mt_open_test                              *
*************************************************************************
*									*
* Using the mtdrv as a helper, test some open races that can only be	*
* tested using multiple threads.					*
*									*
************************************************************************/
#ifndef DIRECT_USER

pthread_mutex_t		mt_state_lock ;
volatile int		mt_state ;
pthread_mutex_t		mt_quit ;

void	 mt_set_state(int state)
{
    pthread_mutex_lock(&mt_state_lock) ;
    mt_state = state ;
    pthread_mutex_unlock(&mt_state_lock) ;
}

int	 mt_get_state(void)
{
    int		state ;
    pthread_mutex_lock(&mt_state_lock) ;
    state = mt_state ;
    pthread_mutex_unlock(&mt_state_lock) ;
    return(state) ;
}

int	mt_await_state(int statenr)
{
    int		st ;

    while ((st = mt_get_state()) < statenr && st >= 0)
	sleep(1) ;

    return(st) ;
}

int	mt_ioctl(int fd, int cmd, int arg)
{
    struct strioctl	ioc ;

    ioc.ic_cmd 	  = cmd ;
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(arg) ;
    ioc.ic_dp	  = (char *) &arg ;

    return(ioctl(fd, I_STR, &ioc)) ;
}

void	*mt_thread(void *arg)
{
    intptr_t	thrno = (intptr_t) arg ;
    int		state = 0 ;
    int		prev_state ;
    int		fd = -1 ;
    int		fdc = -1 ;
    int		ctl_fd ;
    char	buf[300] ;

    print("%s: Thread%d: Starting up\n", now(), thrno) ;
    sprintf(buf, "/dev/mtdrv.%d", thrno) ;
    ctl_fd = open(buf, O_RDWR, 0) ;
    if (ctl_fd < 0)
    {
	perror(buf) ;
	mt_set_state(-1) ;
    }

    print("%s: Thread%d: opened %s\n", now(), thrno, buf) ;

    do
    {
	prev_state = state ;
	state = mt_get_state() ;
	print("%s: Thread%d: state %d\n", now(), thrno, state) ;
	switch (state)
	{
	case 0:				/* initial idle state */
	    mt_await_state(1) ;
	    break ;

	case 1:
	    /*
	     * Thread 1 sets open sleep time while thread 2 iterates.
	     * Then thread 1 opens the driver, which will sleep for
	     * the indicated number of ticks.
	     */
	    if (prev_state == 1)
	    {
		mt_await_state(2) ;
		break ;
	    }

	    if (thrno == 2)
	    {
		sleep(1) ;
		break ;
	    }

	    if (mt_ioctl(ctl_fd, MTDRV_SET_OPEN_SLEEP, 400) < 0)
	    {
		perror("ioctl: MTDRV_SET_OPEN_SLEEP") ;
		mt_set_state(-1) ;
		break ;
	    }

	    mt_set_state(2) ;

	    print("%s: Thread%d: opening mtdrv.3\n", now(), thrno) ;
	    fd = open("/dev/mtdrv.3", O_RDWR, 0) ;
	    if (fd < 0)
	    {
		perror("mtdrv.3") ;
		mt_set_state(-1) ;
		break ;
	    }

	    print("%s: Thread%d: mtdrv.3 opened fd=%d\n", now(), thrno, fd) ;
	    break ;

	case 2:
	    /*
	     * Thread 1 does nothing. 
	     * Thread 2 opens mtdrv.3, the same device that thread 1
	     * should be sleeping on the open.  This tests serializing
	     * opens to the same device from different threads.
	     */
	    if (prev_state == 2)
	    {
		mt_await_state(3) ;
		break ;
	    }

	    if (thrno == 1)
	    {
		sleep(1) ;
		break ;
	    }

	    sleep(1) ;
	    print("%s: Thread%d: opening mtdrv.3\n", now(), thrno) ;
	    fd = open("/dev/mtdrv.3", O_RDWR, 0) ;
	    if (fd < 0)
	    {
		perror("mtdrv.3") ;
		mt_set_state(-1) ;
		break ;
	    }

	    print("%s: Thread%d: mtdrv.3 opened fd=%d\n", now(), thrno, fd) ;
	    mt_set_state(3) ;
	    break ;

	case 3:
	    /*
	     * Thread 1 does nothing.
	     * Thread 2 opens the clone device, which selects dev 3.
	     * This is not allowed because dev 3 is in use already.
	     */
	    if (prev_state == 3)
	    {
		sleep(1) ;
		break ;
	    }

	    if (thrno == 1)
	    {
		sleep(2) ;
		print("%s: Thread%d: close fd=%d\n", now(), thrno, fd) ;
		close(fd) ;
		fd = -1 ;
		break ;
	    }

	    if (mt_ioctl(ctl_fd, MTDRV_SET_CLONE_DEV, 3) < 0)
	    {
		perror("ioctl: MTDRV_SET_CLONE_DEV") ;
		mt_set_state(-1) ;
		break ;
	    }

	    print("%s: Thread%d: opening mtdrv_clone\n", now(), thrno) ;
	    fdc = open("/dev/mtdrv_clone", O_RDWR, 0) ;
	    if (fdc >= 0)
	    {
		print("%s: Thread%d: "
			"mtdrv_clone opened dev 3 but should have failed\n",
			now(), thrno) ;
		mt_set_state(-1) ;
		break ;
	    }

	    if (errno != EBUSY)
	    {
		print("%s: Thread%d: "
			"mtdrv_clone failed opening dev 3 "
			"but with wrong errno %d\n",
			now(), thrno, errno) ;
		mt_set_state(-1) ;
		break ;
	    }

	    print("%s: Thread%d: mtdrv_clone "
		  "got expected EBUSY return opening dev 3\n",
		    now(), thrno) ;
	    sleep(1) ;
	    print("%s: Thread%d: close fd=%d\n", now(), thrno, fd) ;
	    close(fd) ;
	    fd = -1 ;
	    fdc = -1;
	    mt_set_state(4) ;
	    break ;

	case 4:
	    /*
	     * Thread 1 sets open sleep time while thread 2 iterates.
	     * Then thread 1 opens the driver, which will sleep for
	     * the indicated number of ticks.
	     */
	    if (prev_state == 4)
	    {
		sleep(1) ;
		break ;
	    }


	    if (thrno == 2)
	    {
		mt_await_state(5) ;
		break ;
	    }

	    if (mt_ioctl(ctl_fd, MTDRV_SET_OPEN_SLEEP, 300) < 0)
	    {
		perror("ioctl: MTDRV_SET_OPEN_SLEEP") ;
		mt_set_state(-1) ;
		break ;
	    }

	    mt_set_state(5) ;

	    print("%s: Thread%d: opening mtdrv.3\n", now(), thrno) ;
	    fd = open("/dev/mtdrv.3", O_RDWR, 0) ;	/* open will sleep */
	    if (fd < 0)
	    {
		perror("mtdrv.3") ;
		mt_set_state(-1) ;
		break ;
	    }

	    print("%s: Thread%d: mtdrv.3 opened fd=%d\n", now(), thrno, fd) ;
	    break ;

	case 5:
	    /*
	     * Thread 1 does nothing.
	     * Thread 2 opens the clone device, which selects dev 3.
	     * Dev 3 is half-open at the moment by thread 1.  When
	     * clone-open picks the same minor it is regarded as an
	     * error.
	     */
	    if (prev_state == 5)
	    {
		sleep(1) ;
		break ;
	    }


	    if (thrno == 1)
	    {
		mt_await_state(6) ;
		break ;
	    }

	    if (mt_ioctl(ctl_fd, MTDRV_SET_CLONE_DEV, 3) < 0)
	    {
		perror("ioctl: MTDRV_SET_CLONE_DEV") ;
		mt_set_state(-1) ;
		break ;
	    }

	    print("%s: Thread%d: opening mtdrv_clone\n", now(), thrno) ;
	    fdc = open("/dev/mtdrv_clone", O_RDWR, 0) ;
	    if (fdc >= 0)
	    {
		print("%s: Thread%d: "
			"mtdrv_clone opened dev 3 but should have failed\n",
			now(), thrno) ;
		mt_set_state(-1) ;
		break ;
	    }

	    if (errno != EBUSY)
	    {
		print("%s: Thread%d: "
			"mtdrv_clone failed opening dev 3 "
			"but with wrong errno %d\n",
			now(), thrno, errno) ;
		mt_set_state(-1) ;
		break ;
	    }

	    print("%s: Thread%d: mtdrv_clone "
		  "got expected EBUSY return opening dev 3\n",
		    now(), thrno) ;

	    print("%s: Thread%d: close fd=%d\n", now(), thrno, fdc) ;
	    close(fdc) ;
	    fdc = -1 ;
	    sleep(1) ;			/* give thread 1 some time */
	    mt_set_state(6) ;
	    break ;

	case 6:
	    sleep(1) ;
	    mt_set_state(-1) ;
	    break ;
	}

    } while (state >= 0) ;

    print("%s: Thread%d: close fd=%d\n", now(), thrno, fd) ;
    close(fd) ;
    fd = -1 ;
    close(ctl_fd) ;
    print("%s: Thread%d: Exiting\n", now(), thrno) ;
    pthread_mutex_unlock(&mt_quit) ;	/* awaken parent */

    return((void *)0) ;
}


void mt_open_test(struct test_context *ctx)
{
    int		rslt ;
    pthread_t	thr1 ;
    pthread_t	thr2 ;
    pthread_mutexattr_t	 attr ;

    print("Begin multi-thread open test\n") ;
    if (system("rmmod streams-mtdrv 2>&1") != 0)
    {
	print("rmmod failed: %s\n", strerror(errno)) ;
    }
    if (system("modprobe streams-mtdrv 2>&1") != 0)
    {
	print("modprobe failed: %s\n", strerror(errno)) ;
	xit() ;
    }

    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_DEFAULT) ;
    pthread_mutex_init(&mt_state_lock, &attr) ;
    pthread_mutex_init(&mt_quit, &attr) ;
    pthread_mutex_lock(&mt_quit) ;

    rslt = pthread_create(&thr1, NULL, mt_thread, (void *) 1) ;
    if (rslt < 0)
    {
	fprintf(stderr, "Thread #%d: ", 1) ;
	perror("pthread_create") ;
	if (system("rmmod streams-mtdrv 2>&1") != 0) {
	    print("rmmod failed: %s\n", strerror(errno)) ;
	}
	xit() ;
    }

    rslt = pthread_create(&thr2, NULL, mt_thread, (void *) 2) ;
    if (rslt < 0)
    {
	fprintf(stderr, "Thread #%d: ", 2) ;
	perror("pthread_create") ;
	if (system("rmmod streams-mtdrv 2>&1") != 0) {
	    print("rmmod failed: %s\n", strerror(errno)) ;
	}
	xit() ;
    }

    sleep(1) ;
    mt_set_state(1) ;
    pthread_mutex_lock(&mt_quit) ;		/* waits until thread exit */
    print("multi-thread open test OK\n") ;
    if (system("rmmod streams-mtdrv 2>&1") != 0) {
	print("rmmod failed: %s\n", strerror(errno)) ;
    }
}
#endif

/************************************************************************
*                           module_test                                 *
*************************************************************************
*									*
* A test for pushable modules, both loadable and linked in.		*
*									*
************************************************************************/
int module_is_present(int fd, char *name)
{
    int		rslt ;

    rslt = user_ioctl(fd, I_FIND, name) ;
    if (rslt < 0)
    {
	print("module_is_present(%s): I_FIND: %s\n", name, STRERROR(-rslt)) ;
	xit() ;
    }

    return(rslt > 0) ;
}

int module_load(char *name)
{
#ifndef DIRECT_USER
    char	buf[100] ;

    print("Loading module %s\n", name) ;
    sprintf(buf, "modprobe streams-%s 2>&1", name) ;
    return(system(buf)) ;
#else
    return(0) ;
#endif
}

int module_unload(char *name)
{
#ifndef DIRECT_USER
    char	buf[100] ;

    print("Unloading module %s\n", name) ;
    sprintf(buf, "rmmod streams-%s 2>&1", name) ;
    return(system(buf)) ;
#else
    return(0) ;
#endif
}

void module_push(int fd, char *name)
{
    int		rslt ;

    if ((rslt = user_ioctl(fd, I_PUSH, name)) < 0)
    {
	print("module_push(%s): I_PUSH: %s\n", name, STRERROR(-rslt)) ;
	xit() ;
    }

    if (!module_is_present(fd, name))
    {
	print("module_push(%s): I_PUSH succeeded but module not present\n",
			name) ;
	xit() ;
    }
    else
	print("module_push(%s) OK\n", name) ;
}

void module_pop(int fd)
{
    int		rslt ;

    rslt = user_ioctl(fd, I_POP, 0) ;
    if (rslt < 0)
    {
	print("module_pop: I_POP: %s\n", STRERROR(-rslt)) ;
	xit() ;
    }
    else
	print("module_pop OK\n") ;
}

void module_test_data(struct test_context *ctx, int fd1, int fd2)
{
    int		 rslt ;
    static char	*msg = "Test data for modules" ;
    int		 lgth = strlen(msg) ;

    if ((rslt = user_write(fd1, msg, lgth)) < 0)
    {
	print("module_test_data: write fd1: %s\n", STRERROR(-rslt)) ;
	xit() ;
    }

    memset(ctx->rdbuf, 0, lgth+1) ;
    if ((rslt = user_read(fd2, ctx->rdbuf, lgth)) < 0)
    {
	print("module_test_data: read fd2: %s\n", STRERROR(-rslt)) ;
	xit() ;
    }

    if (strcmp(ctx->rdbuf, msg))
    {
	print("module_test_data: read fd2: Data compare error\n"
	      "Exp: %s\n"
	      "Got: %s\n",
	      msg, ctx->rdbuf) ;
	xit() ;
    }

    if ((rslt = user_write(fd2, msg, lgth)) < 0)
    {
	print("module_test_data: write fd2: %s\n", STRERROR(-rslt)) ;
	xit() ;
    }

    memset(ctx->rdbuf, 0, lgth+1) ;
    if ((rslt = user_read(fd1, ctx->rdbuf, lgth)) < 0)
    {
	print("module_test_data: read fd1: %s\n", STRERROR(-rslt)) ;
	xit() ;
    }

    if (strcmp(ctx->rdbuf, msg))
    {
	print("module_test_data: read fd1: Data compare error\n"
	      "Exp: %s\n"
	      "Got: %s\n",
	      msg, ctx->rdbuf) ;
	xit() ;
    }

    print("module_test_data OK\n") ;
}

void module_test(struct test_context *ctx)
{
    int		fds[2] ;
#define fd1	fds[0]
#define fd2	fds[1]

    print("\nTesting pushable modules\n") ;

    if (open_files(&fd1, &fd2) < 0)
	xit() ;

    print("\nSimple test of relay module linked with LiS\n") ;
    module_push(fd1, "relay") ;
    module_test_data(ctx, fd1, fd2) ;
    user_close(fd1) ;
    user_close(fd2) ;


    print("\nTest loading of relay3 via modprobe\n") ;
    if ((user_pipe(fds)) < 0) {
	print("pipe: %s\n", strerror(errno) );
	xit();
    }
    if (module_load("relay3") < 0)
    {
	print("module_load(relay3): %s\n", strerror(errno) );
	xit() ;
    }
    module_push(fd1, "relay3") ;
    module_test_data(ctx, fd1, fd2) ;
    user_close(fd1) ;
    user_close(fd2) ;
    module_unload("relay3") ;

    print("\nRepeat loading of relay3 via modprobe\n") ;
    if ((user_pipe(fds)) < 0) {
	print("pipe: %s\n", strerror(errno) );
	xit();
    }
    if (module_load("relay3") < 0)
    {
	print("module_load(relay3): %s\n", strerror(errno) );
	xit() ;
    }
    module_push(fd1, "relay3") ;
    module_test_data(ctx, fd1, fd2) ;
    user_close(fd1) ;
    user_close(fd2) ;
    module_unload("relay3") ;


    print("\nTest loading of pipemod via I_PUSH\n") ;
    module_unload("pipemod") ;
    if ((user_pipe(fds)) < 0) {
	print("pipe: %s\n", strerror(errno) );
	xit();
    }
    module_push(fd1, "pipemod") ;
    module_test_data(ctx, fd1, fd2) ;
    user_close(fd1) ;
    user_close(fd2) ;
    module_unload("pipemod") ;

    print("\nRepeat loading of pipemod via I_PUSH\n") ;
    module_unload("pipemod") ;
    if ((user_pipe(fds)) < 0) {
	print("pipe: %s\n", strerror(errno) );
	xit();
    }
    module_push(fd1, "pipemod") ;
    module_test_data(ctx, fd1, fd2) ;
    user_close(fd1) ;
    user_close(fd2) ;
    module_unload("pipemod") ;

    print("\nTest loading of pipemod via modprobe\n") ;
    if ((user_pipe(fds)) < 0) {
	print("pipe: %s\n", strerror(errno) );
	xit();
    }
    if (module_load("pipemod") < 0)
    {
	print("module_load(pipemod): %s\n", strerror(errno) );
	xit() ;
    }
    module_push(fd1, "pipemod") ;
    module_test_data(ctx, fd1, fd2) ;
    user_close(fd1) ;
    user_close(fd2) ;
    module_unload("pipemod") ;

    print("\nRepeat loading of pipemod via modprobe\n") ;
    if ((user_pipe(fds)) < 0) {
	print("pipe: %s\n", strerror(errno) );
	xit();
    }
    if (module_load("pipemod") < 0)
    {
	print("module_load(pipemod): %s\n", strerror(errno) );
	xit() ;
    }
    module_push(fd1, "pipemod") ;
    module_test_data(ctx, fd1, fd2) ;
    user_close(fd1) ;
    user_close(fd2) ;
    module_unload("pipemod") ;

#undef fd1
#undef fd2
}

/************************************************************************
*                              test list                                *
************************************************************************/

struct test_spec {
	char *name;
	char *desc;
	void (*func) (struct test_context *ctx);
	int flags;
};

#define TST_DO_TEST	1
#define TST_PRINT_MEM	2
#define TST_MT_SAFE	4

struct test_spec test_list[] = {
	{ "open", "open/close test", open_close_test, TST_PRINT_MEM|TST_MT_SAFE },
	{ "ioctl", "ioctl test", ioctl_test, TST_PRINT_MEM },
	{ "module", "module test", module_test, TST_PRINT_MEM },
	{ "rdopt", "read options test", rdopt_test, TST_PRINT_MEM },
	{ "write", "write test", write_test, TST_PRINT_MEM },
	{ "timer", "close timer test", close_timer_test, TST_PRINT_MEM },
	{ "putmsg", "putmsg test", putmsg_test, TST_PRINT_MEM|TST_MT_SAFE },
	{ "poll", "poll test", poll_test, TST_PRINT_MEM|TST_MT_SAFE },
	{ "mux", "multiplexor test", mux_test, TST_PRINT_MEM },
	{ "clone", "clone driver test", clone_test , 0}, /* crash */
	{ "bufcall", "bufcall test", bufcall_test, TST_MT_SAFE },
	{ "sad", "sad driver test", sad_test, 0 },
	{ "fifo", "FIFO/pipe test", fifo_test, TST_MT_SAFE },
	{ "passfd", "FD passing test", passfd_test, TST_MT_SAFE },
	{ "band", "Q-band test", band_test, TST_MT_SAFE },
	{ "flush", "flushing test", flush_test, TST_MT_SAFE },
	{ "autopush", "autopush test", autopush_test , 0 },
	{ "mt_open", "MT open test", mt_open_test , 0},
#ifdef DIRECT_USER
	{ "pullupmsg", "pullupmsg", pullupmsg_test, TST_PRINT_MEM },
#endif
	{ NULL, NULL, NULL, 0 }
};

/************************************************************************
*                              main                                     *
************************************************************************/
void *test(void *thread_ctx)
{
    extern long		lis_mem_alloced ;		/* from port.c */
    struct test_spec *tst;
    struct test_context *ctx = (struct test_context *)thread_ctx;

    tprint("Begin STREAMS test #%u\n\n", ctx->id) ;

    set_debug_mask(debug_mask) ;

#ifdef DIRECT_USER
    tprint("Directory listing:\n\n") ;
    user_print_dir(NULL, USR_PRNT_INODE) ;
#endif
    tprint("Memory allocated = %ld\n", lis_mem_alloced) ;

    for(tst = &(test_list[0]) ; tst->name != NULL; tst++) {
	    if((!all_tests) && (!(tst->flags & TST_DO_TEST)))
		    continue;
	    if((nthreads > 1) && (!(tst->flags & TST_MT_SAFE)))
		    continue;
	    tprint("*** BEGIN %s:%u TEST ***\n", tst->name, ctx->id);
	    tst->func(ctx);
	    if(tst->flags & TST_PRINT_MEM)
		    print_mem();
	    tprint("Memory allocated = %ld\n", lis_mem_alloced);
	    wait_for_logfile(tst->desc);
	    tprint("*** END %s:%u TEST ***\n", tst->name, ctx->id);
    }

#ifdef DIRECT_USER
    tprint("Directory listing:\n\n") ;
    user_print_dir(NULL, USR_PRNT_INODE) ;
#endif

    set_debug_mask(0L) ;
    tprint("*** strtst #%u completed successfully ***\n", ctx->id) ;
    return NULL; /* ignored */
}


/************************************************************************
*                             print_options                             *
************************************************************************/
void print_options(void)
{
    printf("strtst [<options>]\n");
    printf("  -d<mask>     Set debug mask (long long argument)\n");
    printf("  -h           Print this message\n");
}

/************************************************************************
*                           main                                        *
************************************************************************/

int output = 1;

void copying(int argc, char *argv[])
{
    if (!output)
	return;
    fprintf(stdout, "\
\n\
%1$s %2$s:\n\
\n\
Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 2000       John A. Boyd Jr.  protologos, LLC\n\
Copyright (c) 1997-2000  David Grothe, Gcom, Inc <dave@gcom.com>\n\
\n\
All Rights Reserved.\n\
\n\
This program is free software;  you can  redistribute  it and/or modify it under\n\
the terms of the GNU General  Public License as  published by the  Free Software\n\
Foundation; either  version 2 of  the  License, or  (at  your option) any  later\n\
version.\n\
\n\
This program is distributed in the hope that it will be  useful, but WITHOUT ANY\n\
WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU General Public License for more details.\n\
\n\
You should  have received  a copy of the GNU  General  Public License along with\n\
this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave,\n\
Cambridge, MA 02139, USA.\n\
\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the  U.S. Government  (\"Government\"),  the following provisions apply to you.\n\
If the Software is  supplied by the Department of Defense (\"DoD\"), it is classi-\n\
fied as  \"Commercial Computer Software\"  under paragraph 252.227-7014 of the DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the license rights  granted\n\
herein (the license  rights customarily  provided to non-Government  users).  If\n\
the Software is supplied to any unit or agency of the Government other than DoD,\n\
it is classified as  \"Restricted Computer Software\" and the  Government's rights\n\
in the  Software are defined in  paragraph 52.227-19 of the Federal  Acquisition\n\
Regulations  (\"FAR\") (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of the  NASA Supplement  to the  FAR (or  any  successor\n\
regulations).\n\
\n\
", argv[0], ident);
}

void version(int argc, char *argv[])
{
    if (!output)
	return;
    fprintf(stdout, "\
\n\
%1$s %2$s:\n\
    Copyright (c) 2001-2005  OpenSS7 Corporation.  All Rights Reserved.\n\
    Copyright (c) 2000       John A. Boyd Jr.  protologos, LLC\n\
    Copyright (c) 1997-2000  David Grothe, Gcom, Inc <dave@gcom.com>\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 2, included\n\
    here by reference.\n\
\n\
    See `%1$s --copying' for copying permissions.\n\
\n\
", argv[0], ident);
}

void usage(int argc, char *argv[])
{
    if (!output)
	return;
    fprintf(stderr, "\
Usage:\n\
    %1$s [options] [TEST-CASES]\n\
    %1$s {-h,--help}\n\
    %1$s {-V,--version}\n\
    %1$s {-C,--copying}\n\
", argv[0]);
}

void help(int argc, char *argv[])
{
    struct test_spec *tst;
    if (!output)
	return;
    fprintf(stdout, "\
\n\
Usage:\n\
    %1$s [options] [TEST-CASES]\n\
    %1$s {-h,--help}\n\
    %1$s {-V,--version}\n\
    %1$s {-C,--copying}\n\
Options:\n\
    -n, --threads=NUMBER\n\
        number of threads of execution\n\
    -m, --mask=MASK\n\
        debug mask\n\
    -q, --quiet\n\
        suppress normal output\n\
    -h, --help\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
Available Tests:\n\
", argv[0]);
    for (tst = test_list; tst->name != NULL; tst++)
	fprintf(stdout, "    %s\t%s%s\n", tst->name, tst->desc, (tst->flags & TST_MT_SAFE ? " [MT]" : ""));
    fprintf(stdout, "\n");
}

#ifdef DIRECT_USER
int main(void)
#else
int main(int argc, char *argv[])
#endif
{
    extern long     lis_mem_alloced;	/* from port.c */
    struct test_context *ctx;

    while (1)
    {
	int             c;

#ifdef _GNU_SOURCE
	int             option_index = 0;
	/* *INDENT-OFF* */
	static struct option long_options[] = {
#ifndef DIRECT_USER
#ifdef USE_PTHREADS
	    { "threads",    required_argument,	NULL, 'n'},
#endif
	    { "mask",	    required_argument,	NULL, 'm'},
#endif
	    { "quiet",	    no_argument,	NULL, 'q'},
	    { "help",	    no_argument,	NULL, 'h'},
	    { "version",    no_argument,	NULL, 'V'},
	    { "copying",    no_argument,	NULL, 'C'},
	    { 0, }
	};
	/* *INDENT-ON* */
	c = getopt_long_only(argc, argv, "n:m:qhVC", long_options, &option_index);
#else				/* _GNU_SOURCE */
	c = getopt(argc, argv, "n:m:qhVC");
#endif				/* _GNU_SOURCE */
	if (c == -1)
	    break;
	switch (c)
	{
	    char *ptr;
	case 0:
	    usage(argc, argv);
	    exit(2);
#ifndef DIRECT_USER
#ifdef USE_PTHREADS
	case 'n':		/* -n, --threads */
	    nthreads = strtoul(optarg, &ptr, 0);
	    if((*ptr != '\0') || (nthreads <= 0))
		    goto bad_option;
	    break;
#endif
	case 'm':		/* -m, --mask */
	    debug_mask = strtoul(optarg, &ptr, 0);
	    if(*ptr != '\0')
		    goto bad_option;
	    break;
#endif
	case 'q':		/* -q, --quiet */
	    output = 0;
	    break;
	case 'h':		/* -h, --help */
	    help(argc, argv);
	    exit(0);
	case 'V':		/* -V, --version */
	    version(argc, argv);
	    exit(0);
	case 'C':		/* -C, --copying */
	    copying(argc, argv);
	    exit(0);
	case '?':
	  bad_option:
	    optind--;
	  bad_nonoption:
	    if (optind < argc && output)
	    {
		fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
		for (; optind < argc; optind++)
		    fprintf(stderr, "%s ", argv[optind]);
		fprintf(stderr, "\n");
	    }
	    usage(argc, argv);
	    exit(2);
	}
    }
    /*
     * don't ignore (permuted) non-option arguments 
     */
    if (optind < argc) {
	int c;
	struct test_spec *tst;
	all_tests = 0;
	for(c = optind ; c < argc; c++) {
		for(tst = test_list ; tst->name != NULL; tst++) {
			if(!strcmp(tst->name, argv[c]))
				break;
		}
		if(tst->name == NULL)
			goto bad_nonoption;
		if((nthreads > 1) && !(tst->flags & TST_MT_SAFE))
			goto bad_nonoption;
		tst->flags |= TST_DO_TEST;
	}
    }

#ifdef DIRECT_USER
    lis_print_trace = tst_print_trace;
#endif

    register_drivers() ;
    print("Memory allocated = %ld\n", lis_mem_alloced) ;

#ifndef LINUX
    make_nodes();
    print("Memory allocated = %ld\n", lis_mem_alloced);
#endif

    ctx_list = malloc(nthreads*sizeof(*ctx_list));
    if(ctx_list == NULL) {
	    FAIL("malloc: %s\n", STRERROR(errno));
    }
#ifdef USE_PTHREADS
    for(ctx = ctx_list ; ctx < ctx_list+nthreads ; ctx++) {
	    int ret;
	    init_ctx(ctx);
	    ret = pthread_create(&ctx->th, NULL, test, (void*) ctx);
	    if(ret) {
		    FAIL("pthread_create: %s\n", STRERROR(errno));
	    }
    }
    for(ctx = ctx_list ; ctx < ctx_list+nthreads ; ctx++) {
	    int ret;
	    /* thread retval ignored, assuming that any failing test calls exit() */
	    ret = pthread_join(ctx->th, NULL);
 	    if(ret) {
		    FAIL("pthread_join: %s\n", STRERROR(errno));
	    }
    }
#else
    ctx = ctx_list;
    init_ctx(ctx);
    test(ctx);
#endif
    free(ctx_list);
    print("*** TESTS PASSED ***\n");

    return 0;
}
