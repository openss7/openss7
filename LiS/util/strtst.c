/*
 * Copyright (C)  1997-2000  David Grothe, Gcom, Inc <dave@gcom.com>
 * Copyright (C)  2000       John A. Boyd Jr.  protologos, LLC
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
#ident "@(#) LiS strtst.c 2.28 5/30/03 21:38:45 "

#define	inline			/* make disappear */

#if defined(LINUX) && !defined(DIRECT_USER)
#define _REENTRANT
#define _THREAD_SAFE
#define _XOPEN_SOURCE	500		/* single unix spec */
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <signal.h>
#ifdef __KERNEL__			/* directly coupled to usrio.h */
#include <sys/stream.h>
#endif
#include <sys/stropts.h>
#include <sys/LiS/loop.h>		/* an odd place for this file */
#include <sys/LiS/minimux.h>		/* an odd place for this file */
#ifndef DIRECT_USER
#include <sys/LiS/mtdrv.h>
#include <pthread.h>
#endif


#include <string.h>
/* #include <ioctl.h> */
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#ifdef LINUX
#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/LiS/linuxio.h>
#else
# if defined(SCO)
# include <prototypes.h>
# include <varargs.h>
# endif
#include <sys/LiS/usrio.h>
#endif

#include <sys/LiS/config.h>
#include <sys/strport.h>
#include <sys/sad.h>

#define	ALL_DEBUG_BITS	( ~ ((unsigned long long) 0) )

/************************************************************************
*                      File Names                                       *
************************************************************************/
#ifdef LINUX
#define	LOOP_1		"/dev/loop.1"
#define	LOOP_2		"/dev/loop.2"
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

char		buf[1000] ;		/* general purpose */
char		ctlbuf[1000] ;		/* control messages */
char		rdbuf[1000] ;		/* for reading */
char		rdctlbuf[1000] ;	/* control messages */
int		printk_fd = -1 ;	/* file descr for printk */

/*
 * For I_LIST
 */
struct str_mlist	mod_names[10] ;
struct str_list		mod_list = {10, mod_names} ;

/*
 * For getmsg, putmsg
 */
struct strbuf	wr_ctl = {0, 0, ctlbuf} ;
struct strbuf	wr_dta = {0, 0, buf} ;
struct strbuf	rd_ctl = {0, 0, rdctlbuf} ;
struct strbuf	rd_dta = {0, 0, rdbuf} ;
struct strpeek	pk_str = {
			      {0, 0, rdctlbuf},	/* ctlbuf */
			      {0, 0, rdbuf},	/* databuf */
			      0			/* flags */
			  } ;

extern void make_nodes(void) ;
extern int  n_read(int fd) ;

#ifdef DIRECT_USER		/* tie-in to cmn_err */
typedef      void  (*lis_print_trace_t)   (char *bfrp) ;
extern       lis_print_trace_t      lis_print_trace;
#endif

extern int	n_read_msgs(int fd) ;		/* forward decl */

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
void
print(char *fmt, ...)
{
    char	 bfr[2048];
    extern int	 vsprintf (char *, const char *, va_list);
    va_list	 args;

    va_start (args, fmt);
    vsprintf (bfr, fmt, args);
    va_end (args);

    printf("%s", bfr) ;				/* write to stdout */
    fflush(stdout) ;

    if (printk_fd >= 0)				/* write to kernel */
	user_write(printk_fd, bfr, strlen(bfr)) ;

} /* print */

/************************************************************************
*                             xit                                       *
*************************************************************************
*									*
* This routine is called to exit the program when a test fails.		*
*									*
************************************************************************/
void	xit(void)
{
    print("\n\n\n");
    print("****************************************************\n");
    print("*                  Test Failed                     *\n");
    print("****************************************************\n\n");

    print("Dump of memory areas in use:\n\n") ;

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
    unsigned long	mask1 = (unsigned long)(msk & 0xFFFFFFFF) ;
    unsigned long	mask2 = (msk >> 32) ;

    fd = user_open(LOOP_1, O_RDWR, 0) ;
    if (fd < 0)
    {
	print("loop.1: %s\n", strerror(-fd)) ;
	xit() ;
    }

    rslt = user_ioctl(fd, I_LIS_SDBGMSK, mask1) ;
    if (rslt < 0)
    {
	print("loop.1: I_LIS_SDBGMSK: %s\n", strerror(-rslt)) ;
	xit() ;
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

    messages_size = file_size("/usr/adm/messages") ;
    syslog_size = file_size("/usr/adm/syslog") ;

    for (n = 0;;n++)
    {
	sleep(1) ;
	new_messages_size = file_size("/usr/adm/messages") ;
	new_syslog_size = file_size("/usr/adm/syslog") ;
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
	print("loop.1: %s\n", strerror(-fd)) ;
	xit() ;
    }

    print("\n\nBegin dump of in-use memory areas\n\n") ;
    rslt = user_ioctl(fd, I_LIS_PRNTMEM, 0) ;
    if (rslt < 0)
    {
	print("loop.1: I_LIS_PRNTMEM: %s\n", strerror(-rslt)) ;
	xit() ;
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
    int		rslt ;

    rslt = user_ioctl(fd, I_LIS_PRNTSTRM, 0) ;
    if (rslt < 0)
    {
	print("I_LIS_PRNTSTRM: %s\n", strerror(-rslt)) ;
	xit() ;
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
	if (rslt < 0) xit() ;
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
	if (rslt < 0) xit() ;
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
	if (rslt < 0) xit() ;

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
void	open_close_test(void)
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
	print("\nopen_close_test iteration #%d\n", i) ;

	fd1 = user_open(LOOP_1, O_RDWR, 0) ;
	if (fd1 < 0)
	{
	    print("loop.1: %s\n", strerror(-fd1)) ;
	    break ;
	}

	fd2 = user_open(LOOP_2, O_RDWR, 0) ;
	if (fd2 < 0)
	{
	    print("loop.2: %s\n", strerror(-fd2)) ;
	    break ;
	}

	fd3 = user_open(LOOP_1, O_RDWR, 0) ;
	if (fd3 < 0)
	{
	    print("loop.1 (second open): %s\n", strerror(-fd3)) ;
	    break ;
	}

	rslt = user_open(LOOP_255, O_RDWR, 0) ;
	if (rslt >= 0)			/* this is supposed to fail */
	{
	    print("loop.255 succeeded, but should have failed\n") ;
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
	    print("loop.2: ioctl LOOP_DENY_OPEN: %s\n", strerror(-rslt)) ;
	    break ;
	}

	rslt = user_open(LOOP_2, O_RDWR, 0) ;
	if (rslt >= 0)			/* this is supposed to fail */
	{
	    print("loop.2 succeeded, but should have failed\n") ;
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
	print("loop.1: %s\n", strerror(-*fd1)) ;
	return(*fd1) ;
    }

    *fd2 = user_open(LOOP_2, O_RDWR, 0) ;
    if (*fd2 < 0)
    {
	print("loop.2: %s\n", strerror(-*fd2)) ;
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
	print("loop.1: ioctl LOOP_SET: %s\n", strerror(-rslt)) ;
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
	print("loop_clone.1: %s\n", strerror(-*fd1)) ;
	xit() ;
    }

    *fd2 = user_open(LOOP_CLONE, O_RDWR, 0) ;
    if (*fd2 < 0)
    {
	print("loop_clone.2: %s\n", strerror(-*fd2)) ;
	xit() ;
    }

    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;

    ioc.ic_cmd 	  = LOOP_GET_DEV ;
    arg = -1 ;
    rslt = user_ioctl(*fd2, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop_clone.2: ioctl LOOP_GET_DEV: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (arg < 0)
    {
	print("loop_clone.2: ioctl LOOP_GET_DEV returned %d\n", arg) ;
	xit() ;
    }

    ioc.ic_cmd 	  = LOOP_SET ;
    ioc.ic_len	  = sizeof(int) ;
    rslt = user_ioctl(*fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop_clone.1: ioctl LOOP_SET: %s\n", strerror(-rslt)) ;
	xit() ;
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
	print("I_NREAD: %s\n", strerror(-rslt)) ;
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
	print("I_NREAD: %s\n", strerror(-rslt)) ;

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
    int		rslt ;

    rslt = user_write(fd, bfr, cnt) ;
    if (rslt < 0)
	print("write_data: %s\n", strerror(-rslt)) ;
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
	print("put_msg: %s\n", strerror(-rslt)) ;
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
void	ioctl_test(void)
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
	print("\nioctl_test iteration #%d\n", i) ;

		/********************************
		*           Open Files          * 
		********************************/

	fd1 = user_open(LOOP_1, O_RDWR, 0) ;
	if (fd1 < 0)
	{
	    print("loop.1: %s\n", strerror(-fd1)) ;
	    xit() ;
	}

	fd2 = user_open(LOOP_2, O_RDWR, 0) ;
	if (fd2 < 0)
	{
	    print("loop.2: %s\n", strerror(-fd2)) ;
	    xit() ;
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
	    print("loop.1: ioctl LOOP_SET: %s\n", strerror(-rslt)) ;
	}

	/*
	 * This next one will fail with device busy.  It is rejected
	 * by the loop driver because the previous one set up both
	 * sides of the loopback connection.
	 */
	arg = 1 ;
	ioc.ic_len	  = sizeof(int) ;
	rslt = user_ioctl(fd2, I_STR, &ioc) ;
	if (rslt < 0)
	{
	    print("loop.2: ioctl returned expected error: %s\n",
		  strerror(-rslt)) ;
	}
	else
	    print("loop.2: ioctl returned unexpected success, "
	          "should fail with EBUSY\n") ;

	user_close(fd1) ;
	user_close(fd2) ;


	fd1 = user_open(LOOP_1, O_RDWR, 0) ;
	if (fd1 < 0)
	{
	    print("loop.1: %s\n", strerror(-fd1)) ;
	    xit() ;
	}

		/********************************
		*             I_PUSH            * 
		********************************/

	print("\nTesting I_PUSH (relay)\n") ;
	rslt = user_ioctl(fd1, I_PUSH, "relay") ;
	if (rslt < 0)
	{
	    print("loop.1: I_PUSH (relay): %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	print_mem() ;

		/********************************
		*             I_LOOK            * 
		********************************/

	print("\nTesting I_LOOK\n") ;
	strcpy(buf,"Nothing at all") ;
	rslt = user_ioctl(fd1, I_LOOK, buf) ;
	if (rslt < 0)
	{
	    print("loop.1: I_LOOK: %s\n", strerror(-fd1)) ;
	    xit() ;
	}

	print("I_LOOK returned \"%s\"\n", buf) ;
	print_mem() ;

		/********************************
		*             I_PUSH            * 
		********************************/

	print("\nTesting I_PUSH (relay2)\n") ;
	rslt = user_ioctl(fd1, I_PUSH, "relay2") ;
	if (rslt < 0)
	{
	    print("loop.1: I_PUSH (relay2): %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	print_stream(fd1) ;
	print_mem() ;

		/********************************
		*             I_FIND            * 
		********************************/

	print("\nTesting I_FIND\n") ;
	rslt = user_ioctl(fd1, I_FIND, "relay") ;
	if (rslt < 0)
	{
	    print("loop.1: I_FIND: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	if (rslt > 0)	print("Module \"relay\" is present in the stream\n");
	else		print("Module \"relay\" is not present in the stream\n");

	print_mem() ;

		/********************************
		*         Read/Write            * 
		********************************/

	fd2 = user_open(LOOP_2, O_RDWR, 0) ;
	if (fd2 < 0)
	{
	    print("loop.2: %s\n", strerror(-fd2)) ;
	    xit() ;
	}

	ioc.ic_cmd 	  = LOOP_SET ;
	ioc.ic_timout 	  = 10 ;
	ioc.ic_len	  = sizeof(int) ;
	ioc.ic_dp	  = (char *) &arg ;

	arg = 2 ;
	rslt = user_ioctl(fd1, I_STR, &ioc) ;
	if (rslt < 0)
	{
	    print("loop.1: ioctl LOOP_SET: %s\n", strerror(-rslt)) ;
	}

	print("\nTesting read and write\n") ;
	strcpy(buf, "Data to send down the file") ;
	rslt = write_data(fd1, buf, lgth = strlen(buf)) ;
	if (rslt < 0)
	    xit() ;

	/*
	 * Streams read will wait until the entire count is
	 * exhausted as the default.  Later we will test changing
	 * this option.  For now, just read what was written.
	 */
	rdbuf[0] = 0 ;
	rslt = user_read(fd2, rdbuf, lgth);
	if (rslt < 0)
	{
	    print("loop.2: read: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	if (rslt != lgth)
	{
	    print("loop.2:  read returned %d, expected %d\n", rslt, lgth) ;
	    xit() ;
	}

	if (strcmp(buf, rdbuf))
	{
	    print("loop.2: read: buffer compare error\n") ;
	    print("              wrote \"%s\"\n", buf) ;
	    print("              read  \"%s\"\n", rdbuf) ;
	}
	else
	    print("loop.2: read %d bytes: buffer compared OK\n", rslt) ;

	for (i = 1; i <= 20; i++)
	    strcat(buf, " Add more data to make the message longer. ") ;

	lgth = strlen(buf) ;
	print("Write %d bytes and read back\n", lgth) ;
	rslt = write_data(fd1, buf, lgth) ;
	if (rslt < 0)
	    xit() ;

	/*
	 * Streams read will wait until the entire count is
	 * exhausted as the default.  Later we will test changing
	 * this option.  For now, just read what was written.
	 */
	rdbuf[0] = 0 ;
	rslt = user_read(fd2, rdbuf, lgth);
	if (rslt < 0)
	{
	    print("loop.2: read: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	if (rslt != lgth)
	{
	    print("loop.2:  read returned %d, expected %d\n", rslt, lgth) ;
	    xit() ;
	}

	if (strcmp(buf, rdbuf))
	{
	    print("loop.2: read: buffer compare error\n") ;
	    print("              wrote \"%s\"\n", buf) ;
	    print("              read  \"%s\"\n", rdbuf) ;
	}
	else
	    print("loop.2: read %d bytes: buffer compared OK\n", rslt) ;


		/********************************
		*             I_NREAD           * 
		********************************/

	print("\nTesting I_NREAD\n") ;
	/*
	 * Write two blocks into the file.
	 */
	strcpy(buf, "Data to send down the file") ;
	rslt = write_data(fd1, buf, lgth = strlen(buf)) ;
	if (rslt < 0)
	    xit() ;

	strcpy(buf, "More data to send down the file") ;
	rslt = write_data(fd1, buf, lgth2 = strlen(buf)) ;
	if (rslt < 0)
	    xit() ;

	rslt = n_read(fd2) ;
	if (rslt < 0)
	{
	    print_mem() ;
	    xit() ;
	}

	if (rslt != lgth)		/* just the 1st  msg */
	{
	    print("loop.2:  I_NREAD returned %d, expected %d\n", rslt, lgth) ;
	    print_mem() ;
	    xit() ;
	}
	else
	    print("loop.2: I_NREAD returned %d, OK\n", rslt) ;

	memset(rdbuf, 0, sizeof(rdbuf)) ;
	rslt = user_read(fd2, rdbuf, lgth);
	if (rslt < 0)
	{
	    print("loop.2: read: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	if (rslt != lgth)
	{
	    print("loop.2:  read returned %d, expected %d\n", rslt, lgth) ;
	    xit() ;
	}


		/********************************
		*             I_FLUSH           * 
		********************************/

	print("\nTesting I_FLUSH\n") ;

	/*
	 * One message left unread from previous test.
	 */
	strcpy(buf, "More data to send down the file") ;
	rslt = write_data(fd1, buf, lgth = strlen(buf)) ;
	if (rslt < 0)
	    xit() ;

	rslt = write_data(fd1, buf, lgth) ;
	if (rslt < 0)
	    xit() ;

	rslt = nread_wait_msgs(fd2, 3) ;
	if (rslt != 3)
	{
	    print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 3) ;
	    xit() ;
	}

	rslt = user_ioctl(fd2, I_FLUSH, FLUSHRW) ;
	if (rslt < 0)
	{
	    print("loop.2: I_FLUSH: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	rslt = flush_wait(fd2) ;
	if (rslt != 0)
	{
	    print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 0) ;
	    xit() ;
	}

	print("\nTesting LOOP_FLUSH\n") ;

	rslt = write_data(fd1, buf, lgth) ;
	if (rslt < 0)
	    xit() ;

	rslt = nread_wait_msgs(fd2, 1) ;
	if (rslt != 1)
	{
	    print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 1) ;
	    xit() ;
	}

	strcpy(buf, "Data following flush") ;
	ioc.ic_timout	  = 10 ;
	ioc.ic_dp	  = buf;
	ioc.ic_cmd 	  = LOOP_FLUSH ;
	ioc.ic_len	  = strlen(buf) ;
	rslt = user_ioctl(fd1, I_STR, &ioc) ;
	if (rslt < 0)
	{
	    print("loop_clone.1: ioctl LOOP_FLUSH: %s\n", strerror(-rslt)) ;
	}

	memset(rdbuf, 0, sizeof(rdbuf)) ;
	rslt = user_read(fd2, rdbuf, ioc.ic_len);
	if (rslt < 0)
	{
	    print("loop.2: read: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	if (rslt != ioc.ic_len)
	{
	    print("loop.2:  read returned %d, expected %d\n", rslt, ioc.ic_len);
	    xit() ;
	}

	if (strcmp(buf, rdbuf))
	{
	    print("loop.2:  Expected: %s\n              Got: %s\n", buf, rdbuf);
	    xit() ;
	}

	print("LOOP_FLUSH test succeeded\n") ;


		/********************************
		*           I_CANPUT            * 
		********************************/

	print("\nTesting I_CANPUT\n") ;
	rslt = user_ioctl(fd2, I_CANPUT, 0) ;
	if (rslt < 0)
	{
	    print("loop.2: I_CANPUT: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	print("loop.2: I_CANPUT returned %d\n", rslt) ;

		/********************************
		*           I_SETCLTIME         * 
		********************************/

	print("\nTesting I_SETCLTIME\n") ;
	rslt = user_ioctl(fd2, I_SETCLTIME, 50) ;
	if (rslt < 0)
	{
	    print("loop.2: I_SETCLTIME: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	print("loop.2: I_SETCLTIME returned %d\n", rslt) ;

		/********************************
		*           I_GETCLTIME         * 
		********************************/

	print("\nTesting I_GETCLTIME\n") ;
	arg = 0 ;
	rslt = user_ioctl(fd2, I_GETCLTIME, &arg) ;
	if (rslt < 0)
	{
	    print("loop.2: I_GETCLTIME: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	if (arg != 50)
	    print("loop.2: I_GETCLTIME returned %d, expected %d\n", arg, 50) ;
	else
	    print("loop.2: I_GETCLTIME returned %d\n", arg) ;

		/********************************
		*             I_LIST            * 
		********************************/

	print("\nTesting I_LIST\n") ;
	rslt = user_ioctl(fd1, I_LIST, NULL) ;
	if (rslt < 0)
	{
	    print("loop.1: I_LIST: %s\n", strerror(-rslt)) ;
	    xit() ;
	}
	print("I_LIST(loop.1, NULL) = %d\n", rslt) ;
	lgth = rslt ;			/* length of list */

	rslt = user_ioctl(fd2, I_LIST, NULL) ;
	if (rslt < 0)
	{
	    print("loop.2: I_LIST: %s\n", strerror(-rslt)) ;
	    xit() ;
	}
	print("I_LIST(loop.1, NULL) = %d\n", rslt) ;
	lgth2 = rslt ;			/* length of list */

	mod_list.sl_nmods = sizeof(mod_names)/sizeof(mod_names[0]);
	rslt = user_ioctl(fd1, I_LIST, &mod_list) ;
	if (rslt < 0)
	{
	    print("loop.1: I_LIST: %s\n", strerror(-rslt)) ;
	    xit() ;
	}
	if (rslt > 0)
	{
	    print("loop.1: I_LIST: Returned positive value\n") ;
	    xit() ;
	}
	if (mod_list.sl_nmods != lgth)
	{
	    print("loop.1: I_LIST returned %d modules, expected %d \n",
		    mod_list.sl_nmods, lgth) ;
	    xit() ;
	}

	for (mlp = mod_names; mod_list.sl_nmods > 0;
	     mod_list.sl_nmods--, mlp++)
	{
	    print("loop.1 module[%d] is \"%s\"\n",
		  lgth-mod_list.sl_nmods, mlp->l_name) ;
	}

	mod_list.sl_nmods = sizeof(mod_names)/sizeof(mod_names[0]);
	rslt = user_ioctl(fd2, I_LIST, &mod_list) ;
	if (rslt < 0)
	{
	    print("loop.2: I_LIST: %s\n", strerror(-rslt)) ;
	    xit() ;
	}
	if (rslt > 0)
	{
	    print("loop.2: I_LIST: Returned positive value\n") ;
	    xit() ;
	}
	if (mod_list.sl_nmods != lgth2)
	{
	    print("loop.2: I_LIST returned %d modules, expected %d \n",
		    mod_list.sl_nmods, lgth2) ;
	    xit() ;
	}

	for (mlp = mod_names; mod_list.sl_nmods > 0;
	     mod_list.sl_nmods--, mlp++)
	{
	    print("loop.2 module[%d] is \"%s\"\n",
		  lgth2-mod_list.sl_nmods, mlp->l_name) ;
	}

		/********************************
		*           I_ATMARK            *
		********************************/

	print("\nTesting I_ATMARK\n") ;

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
	    print("loop_clone.1: ioctl LOOP_PUTNXT: %s\n", strerror(-rslt)) ;
	}

	strcpy(buf, "Data to send down the file for testing I_ATMARK") ;
	lgth = strlen(buf) ;

	rslt = user_ioctl(fd2, I_ATMARK, ANYMARK) ;
	print("loop.2: I_ATMARK w/no messages: ") ;
	if (rslt < 0)
	{
	    print("%s\n", strerror(-rslt)) ;
	    xit() ;
	}

	print("OK\n") ;

	rslt = write_data(fd1, buf, lgth) ;
	if (rslt < 0)
	    xit() ;

	rslt = user_ioctl(fd2, I_ATMARK, ANYMARK) ;
	print("loop.2: I_ATMARK w/non-marked message: ") ;
	if (rslt < 0)
	{
	    print("%s\n", strerror(-rslt)) ;
	    xit() ;
	}

	print("OK\n") ;

	rslt = user_ioctl(fd2, I_FLUSH, FLUSHRW) ;
	if (rslt < 0)
	{
	    print("loop.2: I_FLUSH: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	rslt = flush_wait(fd2) ;

	ioc.ic_cmd 	  = LOOP_MARK ;		/* mark the next msg */
	ioc.ic_len	  = 0 ;
	ioc.ic_dp	  = NULL ;

	rslt = user_ioctl(fd1, I_STR, &ioc) ;
	if (rslt < 0)
	{
	    print("loop.1: ioctl LOOP_MARK: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	rslt = write_data(fd1, buf, lgth) ;
	if (rslt < 0)
	    xit() ;

	nread_wait_msgs(fd2, 1) ;
	rslt = user_ioctl(fd2, I_ATMARK, ANYMARK) ;
	print("loop.2: I_ATMARK(ANYMARK) w/marked message: ") ;
	if (rslt < 0)
	{
	    print("%s\n", strerror(-rslt)) ;
	    xit() ;
	}

	if (rslt == 1)
	    print("OK\n") ;
	else
	{
	    print("returned %d, expected 1\n", rslt) ;
	    xit() ;
	}

	rslt = user_ioctl(fd2, I_ATMARK, LASTMARK) ;
	print("loop.2: I_ATMARK(LASTMARK) w/marked message last: ") ;
	if (rslt < 0)
	{
	    print("%s\n", strerror(-rslt)) ;
	    xit() ;
	}

	if (rslt == 1)
	    print("OK\n") ;
	else
	{
	    print("returned %d, expected 1\n", rslt) ;
	    xit() ;
	}

	rslt = write_data(fd1, buf, lgth) ;	/* non-marked msg */
	if (rslt < 0)
	    xit() ;

	rslt = user_ioctl(fd2, I_ATMARK, ANYMARK) ;
	print("loop.2: I_ATMARK(ANYMARK) w/marked message: ") ;
	if (rslt < 0)
	{
	    print("%s\n", strerror(-rslt)) ;
	    xit() ;
	}

	if (rslt == 1)
	    print("OK\n") ;
	else
	{
	    print("returned %d, expected 1\n", rslt) ;
	    xit() ;
	}

	rslt = user_ioctl(fd2, I_ATMARK, LASTMARK) ;
	print("loop.2: I_ATMARK(LASTMARK) w/marked message last: ") ;
	if (rslt < 0)
	{
	    print("%s\n", strerror(-rslt)) ;
	    xit() ;
	}

	if (rslt == 1)
	    print("OK\n") ;
	else
	{
	    print("returned %d, expected 1\n", rslt) ;
	    xit() ;
	}

	ioc.ic_len	  = 0 ;
	rslt = user_ioctl(fd1, I_STR, &ioc) ;		/* mark nxt msg */
	if (rslt < 0)
	{
	    print("loop.1: ioctl LOOP_MARK: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	rslt = write_data(fd1, buf, lgth) ;
	if (rslt < 0)
	    xit() ;

	rslt = user_ioctl(fd2, I_ATMARK, ANYMARK) ;
	print("loop.2: I_ATMARK(ANYMARK) w/marked message: ") ;
	if (rslt < 0)
	{
	    print("%s\n", strerror(-rslt)) ;
	    xit() ;
	}

	if (rslt == 1)
	    print("OK\n") ;
	else
	{
	    print("returned %d, expected 1\n", rslt) ;
	    xit() ;
	}

	rslt = user_ioctl(fd2, I_ATMARK, LASTMARK) ;
	print("loop.2: I_ATMARK(LASTMARK) w/marked message not last: ") ;
	if (rslt < 0)
	{
	    print("%s\n", strerror(-rslt)) ;
	    xit() ;
	}

	if (rslt == 0)
	    print("OK\n") ;
	else
	{
	    print("returned %d, expected 1\n", rslt) ;
	    xit() ;
	}

	rslt = user_ioctl(fd2, I_FLUSH, FLUSHRW) ;
	if (rslt < 0)
	{
	    print("loop.2: I_FLUSH: %s\n", strerror(-rslt)) ;
	    xit() ;
	}


		/********************************
		*           I_SETSIG            * 
		********************************/

	print("\nTesting I_SETSIG/I_GETSIG\n") ;
	rslt = user_ioctl(fd1, I_SETSIG, S_INPUT) ;
	if (rslt < 0)
	{
	    print("loop.1: I_SETSIG: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	rslt = user_ioctl(fd1, I_GETSIG, &arg) ;
	if (rslt < 0)
	{
	    print("loop.1: I_GETSIG: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	if (arg == S_INPUT)
	    print("loop.1: I_GETSIG returned 0x%x, OK\n", arg) ;
	else
	{
	    print("loop.1: I_GETSIG returned 0x%x, expected 0x%x\n",
	    		arg, S_INPUT) ;
	    xit() ;
	}

	signal(SIGPOLL, input_sig) ;
	rslt = write_data(fd2, buf, lgth) ;	/* produce some input on fd1 */
	if (rslt < 0)
	    xit() ;



		/********************************
		*             I_POP             * 
		********************************/

	print("\nTesting I_POP\n") ;
	rslt = user_ioctl(fd1, I_POP, 0) ;
	if (rslt < 0)
	{
	    print("loop.1: I_POP: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

		/********************************
		*         TRANSPARENT           * 
		********************************/

	print("\nTesting TRANSPARENT ioctls\n") ;
	strcpy(buf, "Data for loop driver transparent test") ;
	xp.cmnd  = LOOP_XPARENT_COPYIN ;
	xp.i_arg = strlen(buf) ;
	xp.p_arg = buf ;
	rslt = user_ioctl(fd1, LOOP_XPARENT_COPYIN, &xp) ;
	if (rslt < 0)
	{
	    print("loop.1: LOOP_XPARENT_COPYIN: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	xp.cmnd  = LOOP_XPARENT_COPYOUT ;
	xp.i_arg = sizeof(rdbuf) ;
	xp.p_arg = rdbuf ;
	rslt = user_ioctl(fd1, LOOP_XPARENT_COPYOUT, &xp) ;
	if (rslt < 0)
	{
	    print("loop.1: LOOP_XPARENT_COPYOUT: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	print("Snt: %s\n", buf) ;
	print("Rcv: %s\n", rdbuf) ;
	if (strcmp(buf, rdbuf) != 0)
	{
	    print("Buffers do not compare\n") ;
	    xit() ;
	}

	print("Buffers compare OK\n") ;



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
void	rdopt_test(void)
{
    int			i ;
    int			fd1 ;
    int			fd2 ;
    int			arg ;
    int			rslt ;
    int			lgth ;
    int			lgth2 ;
    struct strioctl	ioc ;

    print("\nRead option test\n") ;

	    /********************************
	    *         Open Files            * 
	    ********************************/

    rslt = open_files(&fd1, &fd2) ;
    if (rslt < 0) xit() ;

    ioc.ic_timout = 10 ;
    ioc.ic_dp	  = NULL;
    ioc.ic_cmd 	  = LOOP_PUTNXT ;	/* use putnxt rather then svcq */
    ioc.ic_len	  = 0 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop_clone.1: ioctl LOOP_PUTNXT: %s\n", strerror(-rslt)) ;
    }

    ioc.ic_len	  = 0 ;
    rslt = user_ioctl(fd2, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop_clone.2: ioctl LOOP_PUTNXT: %s\n", strerror(-rslt)) ;
    }

	    /********************************
	    *         RNORM Test            * 
	    ********************************/

    print("\nTesting I_SRDOPT/I_GRDOPT(RNORM)\n") ;
    rslt = user_ioctl(fd1, I_SRDOPT, RNORM) ;
    if (rslt < 0)
    {
	print("loop.1: I_SRDOPT(RNORM): %s\n", strerror(-rslt)) ;
	xit() ;
    }

    rslt = user_ioctl(fd1, I_GRDOPT, &arg) ;
    if (rslt < 0)
    {
	print("loop.1: I_GRDOPT(RNORM): %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if ((arg & RMODEMASK) == RNORM)
	print("I_SRDOPT(RNORM) == I_GRDOPT\n") ;
    else
	print("I_SRDOPT(RNORM): set opt to %d, read back %d\n",
		    RNORM, arg & RMODEMASK) ;

    /*
     * Demonstrate this mode of operation by writing two messasges
     * downstream and then reading back both of them in one read.
     */
    strcpy(buf, "Test data for I_SRDOPT(RNORM)") ;
    lgth = strlen(buf) ;
    for (i = 1; i <= 3; i++)		/* write it three times */
    {
	if (write_data(fd2, buf, lgth) < 0) xit() ;
    }

    /*
     * Streams read will wait until the entire count is
     * exhausted as the default (or out of data to read).  
     */
    nread_wait_msgs(fd1, 3) ;
    memset(rdbuf, 0, sizeof(rdbuf)) ;
    rslt = user_read(fd1, rdbuf, 2*lgth+1);
    if (rslt < 0)
    {
	print("loop.1: read: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (rslt != 2*lgth+1)
    {
	print("loop.1:  read returned %d, expected %d\n", rslt, 2*lgth+1) ;
	xit() ;
    }

    /*
     * Read the rest in with a count that is more than the length
     * of the remaining data.
     */
    rslt = user_read(fd1, &rdbuf[2*lgth+1], 2*lgth);
    if (rslt < 0)
    {
	print("loop.1: read: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (rslt != lgth-1)
    {
	print("loop.1:  read returned %d, expected %d\n", rslt, lgth-1) ;
	xit() ;
    }

    if (   strncmp(buf, &rdbuf[0],      lgth) == 0
        && strncmp(buf, &rdbuf[lgth],   lgth) == 0
        && strncmp(buf, &rdbuf[2*lgth], lgth) == 0
       )
	   print("Buffers compare OK\n") ;
    else
    {
	print("Data read does not match data written\n") ;
	print("Wrote: %s\n", buf) ;
	print("Read:  %s\n", rdbuf) ;
	xit() ;
    }

	    /********************************
	    *         RMSGD Test            * 
	    ********************************/

    print("\nTesting I_SRDOPT/I_GRDOPT(RMSGD)\n") ;
    rslt = user_ioctl(fd1, I_SRDOPT, RMSGD) ;
    if (rslt < 0)
    {
	print("loop.1: I_SRDOPT(RMSGD): %s\n", strerror(-rslt)) ;
	xit() ;
    }

    rslt = user_ioctl(fd1, I_GRDOPT, &arg) ;
    if (rslt < 0)
    {
	print("loop.1: I_GRDOPT(RMSGD): %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if ((arg & RMODEMASK) == RMSGD)
	print("I_SRDOPT(RMSGD) == I_GRDOPT\n") ;
    else
	print("I_SRDOPT(RMSGD): set opt to %d, read back %d\n",
		    RMSGD, arg & RMODEMASK) ;

    /*
     * Demonstrate this mode of operation by writing a message
     * and reading just a part of it back.  I_NREAD will then
     * tell us that there is nothing to be read anymore.
     */
    strcpy(buf, "Test data for I_SRDOPT(RMSGD)") ;
    lgth = strlen(buf) ;
    if (write_data(fd2, buf, lgth) < 0) xit() ;

    /*
     * Streams read will return with whatever is there.  Leftover
     * message fragment will be discarded.
     */
    nread_wait_msgs(fd1, 1) ;
    memset(rdbuf, 0, sizeof(rdbuf)) ;	
    rslt = user_read(fd1, rdbuf, lgth/2);	/* read half the message */
    if (rslt < 0)
    {
	print("loop.1: read: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (rslt != lgth/2)
    {
	print("loop.1:  read returned %d, expected %d\n", rslt, lgth/2) ;
	xit() ;
    }

    if (strncmp(buf, rdbuf, lgth/2) == 0)
	print("Buffers compare OK\n") ;
    else
    {
	print("Data read does not match data written\n") ;
	print("Wrote: %s\n", buf) ;
	print("Read:  %s\n", rdbuf) ;
	xit() ;
    }

    rslt = n_read(fd1) ;
    if (rslt < 0) xit() ;
    if (rslt > 0)
	print("%d bytes still waiting to be read, should be zero\n", rslt);
    else
	print("No bytes waiting to be read\n") ;


	    /********************************
	    *         RMSGN Test            * 
	    ********************************/

    print("\nTesting I_SRDOPT/I_GRDOPT(RMSGN)\n") ;
    rslt = user_ioctl(fd1, I_SRDOPT, RMSGN) ;
    if (rslt < 0)
    {
	print("loop.1: I_SRDOPT(RMSGN): %s\n", strerror(-rslt)) ;
	xit() ;
    }

    rslt = user_ioctl(fd1, I_GRDOPT, &arg) ;
    if (rslt < 0)
    {
	print("loop.1: I_GRDOPT(RMSGN): %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if ((arg & RMODEMASK) == RMSGN)
	print("I_SRDOPT(RMSGN) == I_GRDOPT\n") ;
    else
	print("I_SRDOPT(RMSGN): set opt to %d, read back %d\n",
		    RMSGN, arg & RMODEMASK) ;

    /*
     * Demonstrate this mode of operation by writing a message
     * and reading just a part of it back.  I_NREAD will then
     * tell us that there are more bytes waiting to be read.
     */
    strcpy(buf, "Test data for I_SRDOPT(RMSGN)") ;
    lgth = strlen(buf) ;
    if (write_data(fd2, buf, lgth) < 0) xit() ;

    /*
     * Streams read will return with whatever is there.  Leftover
     * message fragment will be saved.
     */
    nread_wait_msgs(fd1, 1) ;
    memset(rdbuf, 0, sizeof(rdbuf)) ;	
    rslt = user_read(fd1, rdbuf, lgth/2);	/* read half the message */
    if (rslt < 0)
    {
	print("loop.1: read: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (rslt != lgth/2)
    {
	print("loop.1:  read returned %d, expected %d\n", rslt, lgth/2) ;
	xit() ;
    }

    rslt = n_read(fd1) ;			/* see what's left */
    if (rslt < 0) xit() ;
    if (rslt != lgth - lgth/2)
    {
	print("%d bytes still waiting to be read, should be %d\n",
		 rslt, lgth - lgth/2);
	xit() ;
    }

    print("%d bytes waiting to be read, OK\n", rslt) ;

    rslt = user_read(fd1, &rdbuf[lgth/2], lgth) ;	/* read the rest */
    if (rslt < 0)
    {
	print("loop.1: read: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (rslt != lgth - lgth/2)
    {
	print("%d bytes on second read, should be %d\n",
		 rslt, lgth - lgth/2);
	xit() ;
    }

    if (strncmp(buf, rdbuf, lgth) == 0)
	print("Buffers compare OK\n") ;
    else
    {
	print("Data read does not match data written\n") ;
	print("Wrote: %s\n", buf) ;
	print("Read:  %s\n", rdbuf) ;
	xit() ;
    }

	    /********************************
	    *           RPROTDAT            * 
	    ********************************/

    print("\nTesting I_SRDOPT/I_GRDOPT(RPROTDAT)\n") ;
    rslt = user_ioctl(fd1, I_SRDOPT, RNORM|RPROTDAT) ;
    if (rslt < 0)
    {
	print("loop.1: I_SRDOPT(RPROTDAT): %s\n", strerror(-rslt)) ;
	xit() ;
    }

    rslt = user_ioctl(fd1, I_GRDOPT, &arg) ;
    if (rslt < 0)
    {
	print("loop.1: I_GRDOPT(RPROTDAT): %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (arg == (RNORM|RPROTDAT))
	print("I_SRDOPT(RPROTDAT) == I_GRDOPT\n") ;
    else
	print("I_SRDOPT(RPROTDAT): set opt to %d, read back %d\n",
		    RNORM|RPROTDAT, arg) ;

    /*
     * Use putmsg to send a control message, read it back as normal
     * data.
     */
    print("Control message only...\n") ;
    strcpy(ctlbuf, "Control message for RPROTDAT test") ;
    lgth = strlen(ctlbuf) ;
    wr_ctl.len	= lgth ;
    wr_dta.len	= -1 ;				/* no data part */
    if (put_msg(fd2, &wr_ctl, &wr_dta, 0, MSG_BAND) < 0) xit() ;

    nread_wait_msgs(fd1, 1) ;
    memset(rdbuf, 0, sizeof(rdbuf)) ;	
    rslt = user_read(fd1, rdbuf, lgth);		/* read the message */
    if (rslt < 0)
    {
	print("loop.1: read: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (rslt != lgth)
    {
	print("loop.1:  read returned %d, expected %d\n", rslt, lgth) ;
	xit() ;
    }

    if (strcmp(ctlbuf, rdbuf) == 0)
	print("Buffers compare OK\n") ;
    else
    {
	print("Data read does not match data written\n") ;
	print("Wrote: %s\n", ctlbuf) ;
	print("Read:  %s\n", rdbuf) ;
	xit() ;
    }

    /*
     * Use putmsg to send a control and data message, read it back as normal
     * data.
     */
    print("Control and data message...\n") ;
    strcpy(ctlbuf, "Control message for RPROTDAT test") ;
    strcpy(buf,    "/Data message for RPROTDAT test") ;
    lgth = strlen(ctlbuf) ;
    lgth2 = strlen(buf) ;
    wr_ctl.len	= lgth ;
    wr_dta.len	= lgth2 ;
    if (put_msg(fd2, &wr_ctl, &wr_dta, 0, MSG_BAND) < 0) xit() ;

    memset(rdbuf, 0, sizeof(rdbuf)) ;	
    lgth = lgth + lgth2 ;			/* combined message lgth */
    nread_wait_msgs(fd1, 1) ;
    rslt = user_read(fd1, rdbuf, lgth);		/* read the message */
    if (rslt < 0)
    {
	print("loop.1: read: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (rslt != lgth)
    {
	print("loop.1:  read returned %d, expected %d\n", rslt, lgth) ;
	xit() ;
    }

    strcat(ctlbuf, buf) ;			/* concatenate the msgs */
    if (strcmp(ctlbuf, rdbuf) == 0)
	print("Buffers compare OK\n") ;
    else
    {
	print("Data read does not match data written\n") ;
	print("Wrote: %s\n", ctlbuf) ;
	print("Read:  %s\n", rdbuf) ;
	xit() ;
    }

	    /********************************
	    *           RPROTDIS            * 
	    ********************************/

    print("\nTesting I_SRDOPT/I_GRDOPT(RPROTDIS)\n") ;
    rslt = user_ioctl(fd1, I_SRDOPT, RMSGN|RPROTDIS) ;
    if (rslt < 0)
    {
	print("loop.1: I_SRDOPT(RPROTDIS): %s\n", strerror(-rslt)) ;
	xit() ;
    }

    rslt = user_ioctl(fd1, I_GRDOPT, &arg) ;
    if (rslt < 0)
    {
	print("loop.1: I_GRDOPT(RPROTDIS): %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (arg == (RMSGN|RPROTDIS))
	print("I_SRDOPT(RPROTDIS) == I_GRDOPT\n") ;
    else
	print("I_SRDOPT(RPROTDIS): set opt to %d, read back %d\n",
		    RMSGN|RPROTDIS, arg) ;

    /*
     * Use putmsg to send a control message, should leave nothing to
     * be read.
     *
     * It is not clear whether this should result in a zero-length
     * message or no message at all.  For now it is the second.
     */
    print("Control message only...\n") ;
    strcpy(ctlbuf, "Control message for RPROTDIS test") ;
    lgth = strlen(ctlbuf) ;
    wr_ctl.len	= lgth ;
    wr_dta.len	= -1 ;				/* no data part */
    if (put_msg(fd2, &wr_ctl, &wr_dta, 0, MSG_BAND) < 0) xit() ;

    nread_wait_msgs(fd1, 1) ;
    rslt = n_read(fd1);				/* check how many bytes */
    if (rslt < 0) xit() ;

    if (rslt != 0)				/* should read 0 bytes */
    {
	print("loop.1:  I_NREAD returned %d, expected %d\n", rslt, 0) ;
	xit() ;
    }

    /*
     * Use putmsg to send a control and data message, read it back as normal
     * data.  The control part should be discarded and the data part
     * delivered.
     */
    print("Control and data message...\n") ;
    strcpy(ctlbuf, "Control message for RPROTDIS test") ;
    strcpy(buf,    "/Data message for RPROTDIS test") ;
    lgth = strlen(ctlbuf) ;
    lgth2 = strlen(buf) ;
    wr_ctl.len	= lgth ;
    wr_dta.len	= lgth2 ;
    if (put_msg(fd2, &wr_ctl, &wr_dta, 0, MSG_BAND) < 0) xit() ;

    memset(rdbuf, 0, sizeof(rdbuf)) ;	
    nread_wait_msgs(fd1, 1) ;
    rslt = user_read(fd1, rdbuf, sizeof(rdbuf));	/* read the message */
    if (rslt < 0)
    {
	print("loop.1: read: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (rslt != lgth2)
    {
	print("loop.1:  read returned %d, expected %d\n", rslt, lgth2) ;
	xit() ;
    }

    if (strcmp(buf, rdbuf) == 0)			/* only the data */
	print("Buffers compare OK\n") ;
    else
    {
	print("Data read does not match data written\n") ;
	print("Wrote: %s\n", buf) ;
	print("Read:  %s\n", rdbuf) ;
	xit() ;
    }

	    /********************************
	    *         Close Files           * 
	    ********************************/

    print("\nrdopt_test: close files\n") ;
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
void	write_test(void)
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

    print("\nWrite option test\n") ;

	    /********************************
	    *         Open Files            * 
	    ********************************/

    rslt = open_files(&fd1, &fd2) ;
    if (rslt < 0) xit() ;

	    /********************************
	    *       Zero-lgth Write         * 
	    ********************************/

    print("\nTesting write zero bytes w/o SNDZERO option\n") ;
    rslt = user_write(fd1, buf, 0) ;
#if 1
    if (rslt == 0)
	print("loop.1: *** write zero bytes returned zero.  Should it?\n") ;
    else
	print("loop.1: *** write zero bytes returned %d.  Should it?\n", rslt) ;
#else
    if (rslt < 0)
	print("loop.1: write zero bytes: %s: expected error\n",
		    strerror(-rslt)) ;
    else
    {
	print("loop.1: write zero bytes: returned %d instead of error\n",
		rslt) ;
	xit() ;
    }
#endif

    print("\nTesting write zero bytes with SNDZERO option\n") ;
    rslt = user_ioctl(fd1, I_SWROPT, SNDZERO) ;
    if (rslt < 0)
    {
	print("loop.1: I_SWROPT(SNDZERO): %s\n", strerror(-rslt)) ;
	xit() ;
    }

    rslt = user_ioctl(fd1, I_GWROPT, &arg) ;
    if (rslt < 0)
    {
	print("loop.1: I_SWROPT(SNDZERO): %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (arg == SNDZERO)
	print("I_SWROPT(SNDZERO) == I_GWROPT\n") ;
    else
    {
	print("I_SWROPT(SNDZERO): set opt to %d, read back %d\n",
		    SNDZERO, arg) ;
	xit() ;
    }

    rslt = user_write(fd1, buf, 0) ;
    if (rslt < 0)
    {
	print("loop.1: write zero bytes: %s\n", strerror(-rslt)) ;
	xit() ;
    }
    else
	print("loop.1: write zero bytes: returned %d\n", rslt) ;

    memset(rdbuf, 0, sizeof(rdbuf)) ;	
    nread_wait_msgs(fd2, 1) ;
    rslt = user_read(fd2, rdbuf, sizeof(rdbuf)) ;
    if (rslt < 0)
    {
	print("loop.2: read: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (rslt != 0)
    {
	print("loop.2:  read returned %d, expected %d\n", rslt, 0) ;
	xit() ;
    }

    print("Read 0 bytes from 0-length write\n") ;

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
	print("loop.1: ioctl LOOP_BURST: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    strcpy(buf, "Burst data test:  ............data pattern.......") ;
    lgth = strlen(buf)+1 ;

    rslt = user_write(fd1, buf, lgth) ;
    if (rslt < 0)
    {
	print("loop.1: write: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    print("Read back burst data\n") ;

    /*
     * Set RMSGN so that read will just return a single message.
     */
    rslt = user_ioctl(fd2, I_SRDOPT, RMSGN) ;
    if (rslt < 0)
    {
	print("loop.2: I_SRDOPT(RMSGN): %s\n", strerror(-rslt)) ;
	xit() ;
    }

    for (i = 0; n_read(fd2) > 0; i++)
    {
	rslt = user_read(fd2, rdbuf, sizeof(rdbuf)) ;
	if (rslt < 0)
	{
	    print("loop.2: read: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	if (rslt != lgth)
	{
	    print("Expected %d bytes, got %d\n", lgth, rslt) ;
	    xit() ;
	}

	if (strcmp(buf, rdbuf) != 0)
	{
	    print("Wrote: %s\n", buf) ;
	    print(" Read: %s\n", rdbuf) ;
	    xit() ;
	}
    }

    if (i != arg)
    {
	print("Read %d messages, expected %d\n", i, arg) ;
	xit() ;
    }

#if 0

Can''t easily do this test.  It requires the writer to block and the
reader to relieve the flow control.  In a single process we will
hang on the write and never get control back to do the read.

	    /********************************
	    *         Flow Control	    * 
	    ********************************/

    print("\nTesting downstream flow control\n") ;

    ioc.ic_cmd 	  = LOOP_MSGLVL ;		/* set queue message level */
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;

    arg = 20 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop.1: ioctl LOOP_MSGLVL: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    strcpy(buf, "Flow control data test:  ............data pattern.......") ;
    for (lgth = 0; lgth < 20; lgth++)
	strcat(buf, ".....more data.....") ;
    lgth = strlen(buf) ;
    while ((rslt = user_ioctl(fd1, I_CANPUT, 0)) > 0)
    {
	rslt = user_write(fd1, buf, lgth) ;
	if (rslt < 0) break ;
	arg-- ;				/* count down nr of messages */
    }

    if (rslt < 0)
    {
	print("loop.1: ioctl or write: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    print("Flow control blocked with %d messages queued at driver\n",
		20 - arg) ;

    ioc.ic_cmd 	  = LOOP_MSGLVL ;		/* set queue message level */
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;

    arg = 0 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop.1: ioctl LOOP_MSGLVL: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    while (arg--)
    {
	rslt = user_write(fd1, buf, lgth) ;
	if (rslt < 0)
	{
	    print("loop.1: write: %s\n", strerror(-rslt)) ;
	    xit() ;
	}
    }

    print("%d bytes of data queued to read\n", n_read(fd2)) ;
    unblocked = 0 ;
    while (n_read(fd2) > 0)		/* read all the data */
    {
	rslt = user_ioctl(fd1, I_CANPUT, 0) ;
	if (rslt < 0)
	{
	    print("loop.1: ioctl during readback: %s\n", strerror(-rslt)) ;
	    xit() ;
	}

	if (!unblocked && rslt > 0)
	{
	    print("Unblocked with %d bytes of data queued to read\n",
		    n_read(fd2)) ;
	    unblocked = 1 ;
	}

	rslt = user_read(fd2, rdbuf, sizeof(rdbuf)) ;
	if (rslt < 0)
	{
	    print("loop.2: read: %s\n", strerror(-rslt)) ;
	    xit() ;
	}
    }

    if (!unblocked)
    {
	print("read all data but write still not unblocked\n") ;
	xit() ;
    }

    print("Flow control test succeeded\n") ;

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
void	close_timer_test(void)
{
    int			fd1 ;
    int			fd2 ;
    int			arg ;
    int			rslt ;
    struct strioctl	ioc ;

    print("\nClose timer test\n") ;

	    /********************************
	    *         Open Files            * 
	    ********************************/

    print("Close and let timer expire\n") ;
    rslt = open_files(&fd1, &fd2) ;
    if (rslt < 0) xit() ;

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
	print("loop.1: ioctl LOOP_MSGLVL: %s\n", strerror(-rslt)) ;
	xit() ;
    }

	    /********************************
	    *         Write Message         * 
	    ********************************/

    rslt = write_data(fd1, buf, 20) ;
    if (rslt < 0) xit() ;

    rslt = n_read(fd2) ;
    if (rslt < 0) xit() ;
    if (rslt > 0)
	print("loop.2: I_NREAD returned %d, expected 0\n", rslt) ;


	    /********************************
	    *         Close Files           * 
	    ********************************/

    user_close(fd1) ;
    user_close(fd2) ;


	    /********************************
	    *         Open Files            * 
	    ********************************/

    print("Close and have queue drain before timer expires\n") ;
    rslt = open_files(&fd1, &fd2) ;
    if (rslt < 0) xit() ;

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
	print("loop.1: ioctl LOOP_MSGLVL: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    ioc.ic_cmd 	  = LOOP_TIMR ;		/* set timer for queue */
    arg = 10 ;				/* # timer ticks */
    ioc.ic_len	  = sizeof(int) ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop.1: ioctl LOOP_TIMR: %s\n", strerror(-rslt)) ;
	xit() ;
    }

	    /********************************
	    *         Write Message         * 
	    ********************************/

    rslt = write_data(fd1, buf, 20) ;
    if (rslt < 0) xit() ;

    rslt = n_read(fd2) ;
    if (rslt < 0) xit() ;
    if (rslt > 0)
	print("loop.2: I_NREAD returned %d, expected 0\n", rslt) ;


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
* locations of the data.  We call xit() if anything goes wrong.		*
*									*
* band is the band the message was sent on, rband is the band it	*
* was received on.							*
*									*
************************************************************************/
void	check_getmsg_rslts(int		  rslt,
			   struct strbuf *rd_ctlp,
			   struct strbuf *rd_dtap,
			   int		 *flags,
			   int		  band,
			   int		  rband)
{
    (void) rslt ;

    if (rd_ctlp != NULL && rd_ctlp->len != wr_ctl.len)
    {
	print("check_getmsg_rslts:  ctl lgth returned %d, expected %d\n",
		rd_ctlp->len, wr_ctl.len) ;
	xit() ;
    }

    if (rd_dtap != NULL && rd_dtap->len != wr_dta.len)
    {
	print("check_getmsg_rslts:  data lgth returned %d, expected %d\n",
		rd_dtap->len, wr_dta.len) ;
	xit() ;
    }

    print("check_getmsg_rslts: getmsg return flags = 0x%x\n", *flags) ;
    if (   rd_dtap != NULL
	&& rd_dtap->len > 0
	&& strncmp(buf, rdbuf, rd_dtap->len)
       )
    {
	print("check_getmsg_rslts: read data: buffer compare error\n") ;
	print("              wrote \"%s\"\n", buf) ;
	print("              read  \"%s\"\n", rdbuf) ;
	xit() ;
    }
    else
    if (rd_dtap != NULL)
	print("check_getmsg_rslts: read %d data bytes: buffer compared OK\n",
		rd_dtap->len) ;
    else
	print("check_getmsg_rslts: no data strbuf\n") ;

    if (   rd_ctlp != NULL
	&& rd_ctlp->len > 0
	&& strncmp(ctlbuf, rdctlbuf, rd_ctlp->len)
       )
    {
	print("check_getmsg_rslts: read control: buffer compare error\n") ;
	print("              wrote \"%s\"\n", ctlbuf) ;
	print("              read  \"%s\"\n", rdctlbuf) ;
	xit() ;
    }
    else
    if (rd_ctlp != NULL)
	print("check_getmsg_rslts: read %d ctl bytes: buffer compared OK\n",
		rd_ctlp->len) ;
    else
	print("check_getmsg_rslts: no control strbuf\n") ;

    if (rband != band)
	print("check_getmsg_rslts: sent on band %d, received on band %d\n",
		band, rband) ;

} /* check_getmsg_rslts */

/************************************************************************
*                             do_get_put                                *
*************************************************************************
*									*
* This is a generalized getmsg/putmsg test routine.  It calls xit()	*
* if anything goes wrong.						*
*									*
* Caller must load up buf (data) and ctlbuf (control) for writing.	*
*									*
************************************************************************/
void	do_get_put(int putfd, int getfd,
		   int ctl_lgth, int data_lgth, int band)
{
    int			flags = MSG_ANY;
    int			rband = 0 ;
    int			rslt ;

    wr_ctl.len	= ctl_lgth ;
    wr_dta.len	= data_lgth ;
    if (put_msg(putfd, &wr_ctl, &wr_dta, band, MSG_BAND) < 0) xit() ;

    rd_ctl.len		= -1 ;
    rd_ctl.maxlen	= sizeof(rdctlbuf) ;
    rd_dta.len		= -1 ;
    rd_dta.maxlen	= sizeof(rdbuf) ;

    memset(rdctlbuf, 0, sizeof(rdctlbuf)) ;
    memset(rdbuf, 0, sizeof(rdbuf)) ;

    nread_wait_msgs(getfd, 1) ;
    rslt = user_getpmsg(getfd, &rd_ctl, &rd_dta, &rband, &flags) ;
    check_getmsg_rslts(rslt, &rd_ctl, &rd_dta, &flags, band, rband) ;

} /* do_get_put */

/************************************************************************
*                           do_put                                      *
*************************************************************************
*									*
* Do just the putmsg.  Useful for building up concatenated messages.	*
*									*
************************************************************************/
void	do_put(int putfd, int ctl_lgth, int data_lgth, int band)
{
    int		flags = MSG_ANY;

    (void) flags ;			/* compiler happiness */
    wr_ctl.len	= ctl_lgth ;
    wr_dta.len	= data_lgth ;
    if (put_msg(putfd, &wr_ctl, &wr_dta, band, MSG_BAND) < 0) xit() ;

} /* do_put */

/************************************************************************
*                              do_peek                                  *
*************************************************************************
*									*
* This routine does a putmsg, then an I_PEEK, then a getmsg.  It calls	*
* xit() if anything goes wrong.						*
*									*
* Caller must load up buf (data) and ctlbuf (control) for writing.	*
*									*
************************************************************************/
void	do_peek_fcn(int putfd, int getfd,
		    int wr_ctl_lgth, int wr_data_lgth,
		    int rd_ctl_lgth, int rd_data_lgth,
		    int band)
{
    int			flags = MSG_ANY;
    int			rband = 0 ;
    int			rslt ;
    struct strbuf	*ctlp ;
    struct strbuf	*dtap ;

    wr_ctl.len	= wr_ctl_lgth ;
    wr_dta.len	= wr_data_lgth ;
    if (put_msg(putfd, &wr_ctl, &wr_dta, band, MSG_BAND) < 0) xit() ;

    pk_str.ctlbuf.len		= -1 ;
    pk_str.ctlbuf.maxlen	= rd_ctl_lgth ;
    pk_str.databuf.len		= -1 ;
    pk_str.databuf.maxlen	= rd_data_lgth ;
    pk_str.flags		= MSG_ANY ;

    memset(rdctlbuf, 0, sizeof(rdctlbuf)) ;
    memset(rdbuf, 0, sizeof(rdbuf)) ;

    nread_wait_msgs(getfd, 1) ;
    rslt = user_ioctl(getfd, I_PEEK, &pk_str) ;
    if (rslt < 0)
    {
	print("do_peek: ioctl I_PEEK: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    print("do_peek: maxlen    len  I_PEEK returned %d\n", rslt) ;
    print("control: %6d %6d\n", pk_str.ctlbuf.maxlen, pk_str.ctlbuf.len) ;
    print("   data: %6d %6d\n", pk_str.databuf.maxlen, pk_str.databuf.len) ;

    flags = (int) pk_str.flags ;		/* int <-- long */

    if (rd_ctl_lgth >= wr_ctl_lgth)
	ctlp = &pk_str.ctlbuf ;
    else
	ctlp = NULL ;

    if (rd_data_lgth >= wr_data_lgth)
	dtap = &pk_str.databuf ;
    else
	dtap = NULL ;

    check_getmsg_rslts(rslt, ctlp, dtap, &flags, 0, 0) ;

    rd_ctl.len		= -1 ;
    if (wr_ctl_lgth >= 0)			/* ctl part written?  */
	rd_ctl.maxlen	= sizeof(rdctlbuf) ;	/* read ctl part */
    else
	rd_ctl.maxlen	= -1 ;

    rd_dta.len		= -1 ;
    if (wr_data_lgth >= 0)			/* data part written?  */
	rd_dta.maxlen	= sizeof(rdbuf) ;	/* read data part */
    else
	rd_dta.maxlen	= -1 ;

    flags		= 0 ;

    memset(rdctlbuf, 0, sizeof(rdctlbuf)) ;
    memset(rdbuf, 0, sizeof(rdbuf)) ;

    flags = MSG_ANY ;
    rslt = user_getpmsg(getfd, &rd_ctl, &rd_dta, &rband, &flags) ;
    check_getmsg_rslts(rslt, &rd_ctl, &rd_dta, &flags, band, rband) ;

} /* do_peek */

/************************************************************************
*                              do_peek                                  *
*************************************************************************
*									*
* This routine does a putmsg, then an I_PEEK, then a getmsg.  It calls	*
* xit() if anything goes wrong.						*
*									*
* Caller must load up buf (data) and ctlbuf (control) for writing.	*
*									*
************************************************************************/
void	do_peek(int putfd, int getfd,
		   int ctl_lgth, int data_lgth, int band)
{
    do_peek_fcn(putfd, getfd, ctl_lgth, data_lgth,
			sizeof(rdctlbuf), sizeof(rdbuf), band) ;

} /* do_peek */

/************************************************************************
*                            putmsg_test                                *
*************************************************************************
*									*
* Test putmsg and getmsg.						*
*									*
************************************************************************/
void	putmsg_test(void)
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

    print("\nputmsg/getmsg test\n") ;

	    /********************************
	    *         Open Files            * 
	    ********************************/

    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) xit() ;

	    /********************************
	    *         putmsg/read           *
	    ********************************/

    print("\nUse putmsg to send data, use read to read back data\n") ;
    strcpy(buf, "Test data for putmsg/read") ;
    lgth = strlen(buf) ;
    wr_ctl.len	= -1 ;
    wr_dta.len	= lgth ;
    if (put_msg(fd1, &wr_ctl, &wr_dta, 0, MSG_BAND) < 0) xit() ;

    memset(rdbuf, 0, sizeof(rdbuf)) ;
    nread_wait_msgs(fd2, 1) ;
    rslt = user_read(fd2, rdbuf, lgth);
    if (rslt < 0)
    {
	print("loop_clone.2: read: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (rslt != lgth)
    {
	print("loop_clone.2:  read returned %d, expected %d\n", rslt, lgth) ;
	xit() ;
    }

    if (strcmp(buf, rdbuf))
    {
	print("loop_clone.2: read: buffer compare error\n") ;
	print("              wrote \"%s\"\n", buf) ;
	print("              read  \"%s\"\n", rdbuf) ;
    }
    else
	print("loop_clone.2: read %d bytes: buffer compared OK\n", rslt) ;



    print("\nUse putmsg to send control, read should return error\n") ;
    strcpy(ctlbuf, "Control message") ;
    lgth2 = strlen(ctlbuf) ;
    wr_ctl.len	= lgth2 ;
    wr_dta.len	= lgth ;			/* from before */
    if (put_msg(fd1, &wr_ctl, &wr_dta, 0, MSG_BAND) < 0) xit() ;

    memset(rdbuf, 0, sizeof(rdbuf)) ;
    nread_wait_msgs(fd2, 1) ;
    rslt = user_read(fd2, rdbuf, lgth);
    if (rslt < 0)
	print("loop_clone.2: read returned expected error: %s\n", strerror(-rslt)) ;
    else
    {
	print("loop_clone.2:  read returned %d, but expected error rtn\n",
		rslt) ;
	xit() ;
    }

    /*
     * The message is still queued, so get it using getmsg.
     */
    print("Read re-queued message with getmsg\n") ;
    rd_ctl.len		= -1 ;
    rd_ctl.maxlen	= sizeof(rdctlbuf) ;
    rd_dta.len		= -1 ;
    rd_dta.maxlen	= sizeof(rdbuf) ;
    memset(rdctlbuf, 0, sizeof(rdctlbuf)) ;
    memset(rdbuf, 0, sizeof(rdbuf)) ;
    flags = MSG_ANY;
    rband = 0 ;
    rslt = user_getpmsg(fd2, &rd_ctl, &rd_dta, &rband, &flags) ;
    if (rslt < 0)
    {
	print("loop_clone.2:  getmsg returned %d\n", rslt) ;
	xit() ;
    }

    if (rd_ctl.len != wr_ctl.len)
    {
	print("loop_clone.2: read ctl len = %d, write ctl len = %d\n", 
	      rd_ctl.len, wr_ctl.len) ;
	xit() ;
    }

    if (rd_dta.len != wr_dta.len)
    {
	print("loop_clone.2: read data len = %d, write data len = %d\n", 
	      rd_dta.len, wr_dta.len) ;
	xit() ;
    }

    rslt = 0 ;
    if (strcmp(ctlbuf, rdctlbuf))
    {
	print("loop_clone.2: getmsg: ctl buffer compare error\n") ;
	print("              wrote \"%s\"\n", ctlbuf) ;
	print("              read  \"%s\"\n", rdctlbuf) ;
	rslt = -1 ;
    }

    if (strcmp(buf, rdbuf))
    {
	print("loop_clone.2: getmsg: data buffer compare error\n") ;
	print("              wrote \"%s\"\n", buf) ;
	print("              read  \"%s\"\n", rdbuf) ;
	rslt = -1 ;
    }

    if (rslt < 0)
	xit() ;

    print("Re-queued message buffers compare OK\n") ;

	    /********************************
	    *         putmsg/getmsg         *
	    *           Data only           *
	    ********************************/

    print("\nUse putmsg to send data, use getmsg to read back data\n") ;

    ioc.ic_timout = 10 ;
    ioc.ic_dp	  = NULL;
    ioc.ic_cmd 	  = LOOP_PUTNXT ;		/* use putnxt rather then svcq */
    ioc.ic_len	  = 0 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop_clone.1: ioctl LOOP_PUTNXT: %s\n", strerror(-rslt)) ;
    }

    ioc.ic_len	  = 0 ;
    rslt = user_ioctl(fd2, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop_clone.2: ioctl LOOP_PUTNXT: %s\n", strerror(-rslt)) ;
    }

    print("Peek with no message present\n") ;
    do_peek(fd1, fd2, -1, -1, 0) ;

    print("Data part only\n") ;
    strcpy(buf, "Test data for putmsg/getmsg") ;
    lgth = strlen(buf) ;
    do_get_put(fd1, fd2, -1, lgth, 0) ;
    do_peek(fd1, fd2, -1, lgth, 0) ;
    do_get_put(fd1, fd2, -1, lgth, 1) ;
    do_peek(fd1, fd2, -1, lgth, 1) ;
    do_get_put(fd1, fd2, -1, 0, 0) ;
    do_peek(fd1, fd2, -1, 0, 0) ;
    do_get_put(fd1, fd2, -1, lgth, 0) ;
    do_peek_fcn(fd1, fd2, -1, lgth, 4, 0, 0) ;


	    /********************************
	    *         Ctl and Data          *
	    ********************************/

    print("Control and data parts\n") ;
    do_get_put(fd1, fd2, lgth2, lgth, 0) ;
    do_peek(fd1, fd2, lgth2, lgth, 0) ;
    do_get_put(fd1, fd2, lgth2, lgth, 1) ;
    do_peek(fd1, fd2, lgth2, lgth, 1) ;


	    /********************************
	    *           Ctl only            *
	    ********************************/

    print("Control part only\n") ;
    do_get_put(fd1, fd2, lgth2, -1, 0) ;
    do_peek(fd1, fd2, lgth2, -1, 0) ;
    do_get_put(fd1, fd2, lgth2, -1, 1) ;
    do_peek(fd1, fd2, lgth2, -1, 1) ;



	    /********************************
	    *           Multi-segment       *
	    ********************************/

    print("Three-part message: Data only\n") ;
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;
    ioc.ic_cmd 	  = LOOP_CONCAT ;	/* concatenate messages */
    arg		  = 3 ;			/* concat 3 messages */
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop_clone.1: ioctl LOOP_CONCAT: %s\n", strerror(-rslt)) ;
    }

    strcpy(buf, "Test data for putmsg/getmsg") ;
    lgth = strlen(buf) ;
    do_put(fd1, -1, lgth, 0) ;
    do_put(fd1, -1, lgth, 0) ;
    do_put(fd1, -1, lgth, 0) ;
    /*
     * Read back results
     */
    rd_ctl.len		= -1 ;
    rd_ctl.maxlen	= sizeof(rdctlbuf) ;
    rd_dta.len		= -1 ;
    rd_dta.maxlen	= sizeof(rdbuf) ;
    flags		= 0 ;

    memset(rdctlbuf, 0, sizeof(rdctlbuf)) ;
    memset(rdbuf, 0, sizeof(rdbuf)) ;

    flags = MSG_ANY;
    rband = 0 ;
    nread_wait_msgs(fd2, 3) ;
    rslt = user_getpmsg(fd2, &rd_ctl, &rd_dta, &rband, &flags) ;
    if (rslt < 0)
    {
	print("loop_clone.2: getpmsg: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (rd_ctl.len != -1)
    {
	print("loop_clone.2: got %d control bytes, expected none\n",
		rd_ctl.len) ;
	xit() ;
    }

    if (rd_dta.len != 3*lgth)
    {
	print("loop_clone.2: got %d data bytes, expected %d\n",
		rd_dta.len, 3*lgth) ;
	xit() ;
    }

    if (   strncmp(&rdbuf[0], buf, lgth)
	|| strncmp(&rdbuf[lgth], buf, lgth)
	|| strncmp(&rdbuf[2*lgth], buf, lgth)
       )
    {
	print("loop_clone.2: buffer compare error\n") ;
	print("Exp: %s%s%s\n", buf,buf,buf) ;
	print("Got: %s\n", rdbuf) ;
	xit() ;
    }
    else
	print("Three-part message: OK\n") ;


    print("Three-part message: Control only\n") ;
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;
    ioc.ic_cmd 	  = LOOP_CONCAT ;	/* concatenate messages */
    arg		  = 3 ;			/* concat 3 messages */
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop_clone.1: ioctl LOOP_CONCAT: %s\n", strerror(-rslt)) ;
    }

    strcpy(ctlbuf, "Control message") ;
    lgth2 = strlen(ctlbuf) ;
    do_put(fd1, lgth2, -1, 0) ;
    do_put(fd1, lgth2, -1, 0) ;
    do_put(fd1, lgth2, -1, 0) ;
    /*
     * Read back results
     */
    rd_ctl.len		= -1 ;
    rd_ctl.maxlen	= sizeof(rdctlbuf) ;
    rd_dta.len		= -1 ;
    rd_dta.maxlen	= sizeof(rdbuf) ;
    flags		= 0 ;

    memset(rdctlbuf, 0, sizeof(rdctlbuf)) ;
    memset(rdbuf, 0, sizeof(rdbuf)) ;

    flags = MSG_ANY;
    rband = 0 ;
    nread_wait_msgs(fd2, 3) ;
    rslt = user_getpmsg(fd2, &rd_ctl, &rd_dta, &rband, &flags) ;
    if (rslt < 0)
    {
	print("loop_clone.2: getpmsg: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (rd_dta.len != -1)
    {
	print("loop_clone.2: got %d data bytes, expected none\n",
		rd_dta.len) ;
	xit() ;
    }

    if (rd_ctl.len != 3*lgth2)
    {
	print("loop_clone.2: got %d control bytes, expected %d\n",
		rd_ctl.len, 3*lgth2) ;
	xit() ;
    }

    if (   strncmp(&rdctlbuf[0], ctlbuf, lgth2)
	|| strncmp(&rdctlbuf[lgth2], ctlbuf, lgth2)
	|| strncmp(&rdctlbuf[2*lgth2], ctlbuf, lgth2)
       )
    {
	print("loop_clone.2: buffer compare error\n") ;
	print("Exp: %s%s%s\n", ctlbuf,ctlbuf,ctlbuf) ;
	print("Got: %s\n", rdctlbuf) ;
	xit() ;
    }
    else
	print("Three-part message: OK\n") ;

    print("Six-part message: Control and data both\n") ;
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;
    ioc.ic_cmd 	  = LOOP_CONCAT ;	/* concatenate messages */
    arg		  = 6 ;			/* concat 3 messages */
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop_clone.1: ioctl LOOP_CONCAT: %s\n", strerror(-rslt)) ;
    }

    strcpy(ctlbuf, "Control message") ;
    lgth2 = strlen(ctlbuf) ;
    strcpy(buf, "Test data message") ;
    lgth = strlen(buf) ;
    do_put(fd1, lgth2, -1, 0) ;
    do_put(fd1, lgth2, -1, 0) ;
    do_put(fd1, lgth2, -1, 0) ;
    do_put(fd1, -1, lgth, 0) ;
    do_put(fd1, -1, lgth, 0) ;
    do_put(fd1, -1, lgth, 0) ;
    /*
     * Read back results
     */
    rd_ctl.len		= -1 ;
    rd_ctl.maxlen	= sizeof(rdctlbuf) ;
    rd_dta.len		= -1 ;
    rd_dta.maxlen	= sizeof(rdbuf) ;
    flags		= 0 ;

    memset(rdctlbuf, 0, sizeof(rdctlbuf)) ;
    memset(rdbuf, 0, sizeof(rdbuf)) ;

    flags = MSG_ANY;
    rband = 0 ;
    nread_wait_msgs(fd2, 6) ;
    rslt = user_getpmsg(fd2, &rd_ctl, &rd_dta, &rband, &flags) ;
    if (rslt < 0)
    {
	print("loop_clone.2: getpmsg: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (rd_dta.len != 3*lgth)
    {
	print("loop_clone.2: got %d data bytes, expected %d\n",
		rd_dta.len, 3*lgth) ;
	xit() ;
    }

    if (rd_ctl.len != 3*lgth2)
    {
	print("loop_clone.2: got %d control bytes, expected %d\n",
		rd_ctl.len, 3*lgth2) ;
	xit() ;
    }

    if (   strncmp(&rdctlbuf[0], ctlbuf, lgth2)
	|| strncmp(&rdctlbuf[lgth2], ctlbuf, lgth2)
	|| strncmp(&rdctlbuf[2*lgth2], ctlbuf, lgth2)
	|| strncmp(&rdbuf[0], buf, lgth)
	|| strncmp(&rdbuf[lgth], buf, lgth)
	|| strncmp(&rdbuf[2*lgth], buf, lgth)
       )
    {
	print("loop_clone.2: buffer compare error\n") ;
	print("CtlExp: %s%s%s\n", ctlbuf,ctlbuf,ctlbuf) ;
	print("CtlGot: %s\n", rdctlbuf) ;
	print("DtaExp: %s%s%s\n", buf,buf,buf) ;
	print("DtaGot: %s\n", rdbuf) ;
	xit() ;
    }
    else
	print("Six-part message: OK\n") ;

	    /********************************
	    *         Close Files           * 
	    ********************************/

    print("\nputmsg_test: closing files\n") ;
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

char	*poll_events(short events)
{
    static char		ascii_events[200] ;

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

void	poll_test(void)
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

    print("\nPoll function test\n") ;

    print("Poll with no descriptors, just timeout\n") ;
    rslt = user_poll(fds, 0, 50) ;
    if (rslt < 0)
    {
	print("poll: %s\n", strerror(-rslt)) ;
	xit() ;
    }
    else
	print("Poll returned %d\n", rslt) ;


	    /********************************
	    *         Open Files            * 
	    ********************************/

    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) xit() ;

	    /********************************
	    *         Normal Data           * 
	    ********************************/

    print("\nUse putmsg to send normal data, poll for any input\n") ;
    strcpy(buf, "Test data for putmsg/read") ;
    lgth = strlen(buf) ;
    wr_ctl.len	= -1 ;
    wr_dta.len	= lgth ;
    if (put_msg(fd1, &wr_ctl, &wr_dta, 0, MSG_BAND) < 0) xit() ;

    fds[0].fd		= fd1 ;		/* writing fd */
    fds[0].events	= 0 ;		/* no events */
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].fd		= fd2 ;		/* reading fd */
    fds[1].events	= POLLIN ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	print("poll: %s\n", strerror(-rslt)) ;
	xit() ;
    }
    else
    if (rslt != 1)
    {
	print("Poll returned %d, expected 1\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
	xit() ;
    }
    else
    {
	print("Poll returned %d\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
    }

    print("\nPoll for normal data\n") ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].events	= POLLRDNORM ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	print("poll: %s\n", strerror(-rslt)) ;
	xit() ;
    }
    else
    if (rslt != 1)
    {
	print("Poll returned %d, expected 1\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
	xit() ;
    }
    else
    {
	print("Poll returned %d\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
    }

    print("\nPoll for priority band data\n") ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].events	= POLLRDBAND ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	print("poll: %s\n", strerror(-rslt)) ;
	xit() ;
    }
    else
    if (rslt != 0)
    {
	print("Poll returned %d, expected 0\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
	xit() ;
    }
    else
    {
	print("Poll returned %d\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
    }

    print("\nPoll for high priority message\n") ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].events	= POLLPRI ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	print("poll: %s\n", strerror(-rslt)) ;
	xit() ;
    }
    else
    if (rslt != 0)
    {
	print("Poll returned %d, expected 0\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
	xit() ;
    }
    else
    {
	print("Poll returned %d\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
    }

    memset(rdbuf, 0, sizeof(rdbuf)) ;
    rslt = user_read(fd2, rdbuf, lgth);
    if (rslt < 0)
    {
	print("loop_clone.2: read: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (rslt != lgth)
    {
	print("loop_clone.2:  read returned %d, expected %d\n", rslt, lgth) ;
	xit() ;
    }

    if (strcmp(buf, rdbuf))
    {
	print("loop_clone.2: read: buffer compare error\n") ;
	print("              wrote \"%s\"\n", buf) ;
	print("              read  \"%s\"\n", rdbuf) ;
    }
    else
	print("loop_clone.2: read %d bytes: buffer compared OK\n", rslt) ;

	    /********************************
	    *         Hi-priority           * 
	    ********************************/

    print("\nUse putmsg to send priority control, poll for any input\n") ;
    strcpy(ctlbuf, "Control message") ;
    lgth2 = strlen(ctlbuf) ;
    wr_ctl.len	= lgth2 ;
    wr_dta.len	= lgth ;			/* from before */
    if (put_msg(fd1, &wr_ctl, &wr_dta, 0, MSG_HIPRI) < 0) xit() ;

    fds[0].fd		= fd1 ;		/* writing fd */
    fds[0].events	= 0 ;		/* no events */
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].fd		= fd2 ;		/* reading fd */
    fds[1].events	= POLLIN ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	print("poll: %s\n", strerror(-rslt)) ;
	xit() ;
    }
    else
    if (rslt != 0)
    {
	print("Poll returned %d, expected 0\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
	xit() ;
    }
    else
    {
	print("Poll returned %d\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
    }

    print("\nPoll for normal data\n") ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].events	= POLLRDNORM ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	print("poll: %s\n", strerror(-rslt)) ;
	xit() ;
    }
    else
    if (rslt != 0)
    {
	print("Poll returned %d, expected 0\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
	xit() ;
    }
    else
    {
	print("Poll returned %d\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
    }

    print("\nPoll for priority band data\n") ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].events	= POLLRDBAND ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	print("poll: %s\n", strerror(-rslt)) ;
	xit() ;
    }
    else
    if (rslt != 0)
    {
	print("Poll returned %d, expected 0\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
	xit() ;
    }
    else
    {
	print("Poll returned %d\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
    }

    print("\nPoll for high priority message\n") ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].events	= POLLPRI ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	print("poll: %s\n", strerror(-rslt)) ;
	xit() ;
    }
    else
    if (rslt != 1)
    {
	print("Poll returned %d, expected 1\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
	xit() ;
    }
    else
    {
	print("Poll returned %d\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
    }

    print("\nPoll for high priority message and other stream writeable\n") ;
    fds[0].events	= POLLWRNORM ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].events	= POLLPRI ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	print("poll: %s\n", strerror(-rslt)) ;
	xit() ;
    }
    else
    if (rslt != 2)
    {
	print("Poll returned %d, expected 2\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
	xit() ;
    }
    else
    {
	print("Poll returned %d\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
    }

    rd_ctl.len		= -1 ;
    rd_ctl.maxlen	= sizeof(rdctlbuf) ;
    rd_dta.len		= -1 ;
    rd_dta.maxlen	= sizeof(rdbuf) ;
    flags		= 0 ;

    memset(rdctlbuf, 0, sizeof(rdctlbuf)) ;
    memset(rdbuf, 0, sizeof(rdbuf)) ;

    rslt = user_getpmsg(fd2, &rd_ctl, &rd_dta, &rband, &flags) ;
    check_getmsg_rslts(rslt, &rd_ctl, &rd_dta, &flags, 0, rband) ;



    print("\nPoll for high priority message with delayed delivery\n") ;


    ioc.ic_cmd 	  = LOOP_TIMR ;		/* set timer for queue */
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;

    arg = 50 ;				/* # timer ticks */
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop.1: ioctl LOOP_TIMR: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (put_msg(fd1, &wr_ctl, &wr_dta, 0, MSG_HIPRI) < 0) xit() ;

    fds[0].events	= 0 ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].events	= POLLPRI ;
    fds[1].revents	= 0 ;		/* returned events */

    rslt = user_poll(fds, 2, 100) ;
    if (rslt < 0)
    {
	print("poll: %s\n", strerror(-rslt)) ;
	xit() ;
    }
    else
    if (rslt != 1)
    {
	print("Poll returned %d, expected 1\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
	xit() ;
    }
    else
    {
	print("Poll returned %d\n", rslt) ;
	print("fds[0].events  = %s\n", poll_events(fds[0].events)) ;
	print("fds[0].revents = %s\n", poll_events(fds[0].revents)) ;
	print("fds[1].events  = %s\n", poll_events(fds[1].events)) ;
	print("fds[1].revents = %s\n", poll_events(fds[1].revents)) ;
    }

    rd_ctl.len		= -1 ;
    rd_ctl.maxlen	= sizeof(rdctlbuf) ;
    rd_dta.len		= -1 ;
    rd_dta.maxlen	= sizeof(rdbuf) ;
    flags		= 0 ;

    memset(rdctlbuf, 0, sizeof(rdctlbuf)) ;
    memset(rdbuf, 0, sizeof(rdbuf)) ;

    rslt = user_getpmsg(fd2, &rd_ctl, &rd_dta, &rband, &flags) ;
    check_getmsg_rslts(rslt, &rd_ctl, &rd_dta, &flags, 0, rband) ;


	    /********************************
	    *         Close Files           * 
	    ********************************/

    print("\npoll_test: closing files\n") ;
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
void	mux_test(void)
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

    print("\nSTREAMS multiplexor test\n") ;


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
    if (rslt < 0) xit() ;

    print("mux_test: open a mux-clone device\n") ;
    muxfd1 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	print("mux_clone: %s\n", strerror(-muxfd1)) ;
	xit() ;
    }

    print("mux_test: open another mux-clone device\n") ;
    muxfd2 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd2 < 0)
    {
	print("mux_clone: %s\n", strerror(-muxfd2)) ;
	xit() ;
    }

    print("mux_test: I_LINK loop driver under mux (muxfd1->fd1)\n") ;
    muxid1 = user_ioctl(muxfd1, I_LINK, fd1) ;	/* link fd1 below muxfd1 */
    if (muxid1 < 0)
    {
	print("mux_clone: I_LINK: %s\n", strerror(-muxid1)) ;
	xit() ;
    }
    else
	print("                   muxid=%d\n", muxid1) ;

    print("mux_test: I_LINK loop driver under mux (muxfd2->fd2)\n") ;
    muxid2 = user_ioctl(muxfd2, I_LINK, fd2) ;	/* link fd2 below muxfd2 */
    if (muxid2 < 0)
    {
	print("mux_clone: I_LINK: %s\n", strerror(-muxid2)) ;
	xit() ;
    }
    else
	print("                   muxid=%d\n", muxid2) ;

    print_stream(muxfd1) ;
    print_stream(muxfd2) ;
    print("mux_test: close files to loop driver (now detached)\n") ;
    user_close(fd1) ;
    user_close(fd2) ;

    print("mux_test: send data down through the mux/loop and read back\n") ;
    strcpy(buf, "Test data for sending through the mini-mux") ;
    lgth = strlen(buf) ;
    do_get_put(muxfd1, muxfd2, -1, lgth, 0) ;

	    /********************************
	    *         Close Files           * 
	    ********************************/

    print("\nmux_test: closing control streams\n") ;
    user_close(muxfd1) ;
    user_close(muxfd2) ;


    print("\nTest cascaded multiplexors\n") ;

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
    if (rslt < 0) xit() ;

    print("mux_test: open a mux-clone device\n") ;
    muxfd1 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	print("mux_clone: %s\n", strerror(-muxfd1)) ;
	xit() ;
    }

    print("mux_test: open another mux-clone device\n") ;
    muxfd2 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd2 < 0)
    {
	print("mux_clone: %s\n", strerror(-muxfd2)) ;
	xit() ;
    }

    print("mux_test: I_LINK loop driver under mux (muxfd1->fd1)\n") ;
    muxid1 = user_ioctl(muxfd1, I_LINK, fd1) ;	/* link fd1 below muxfd1 */
    if (muxid1 < 0)
    {
	print("mux_clone: I_LINK: %s\n", strerror(-muxid1)) ;
	xit() ;
    }
    else
	print("                   muxid=%d\n", muxid1) ;

    print("mux_test: I_LINK loop driver under mux (muxfd2->fd2)\n") ;
    muxid2 = user_ioctl(muxfd2, I_LINK, fd2) ;	/* link fd2 below muxfd2 */
    if (muxid2 < 0)
    {
	print("mux_clone: I_LINK: %s\n", strerror(-muxid2)) ;
	xit() ;
    }
    else
	print("                   muxid=%d\n", muxid2) ;

    print("mux_test: open a mux-clone device to cascade\n") ;
    muxfd3 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd3 < 0)
    {
	print("mux_clone: %s\n", strerror(-muxfd3)) ;
	xit() ;
    }

    print("mux_test: open another mux-clone device to cascade\n") ;
    muxfd4 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd4 < 0)
    {
	print("mux_clone: %s\n", strerror(-muxfd4)) ;
	xit() ;
    }

    print("mux_test: I_LINK mux driver under mux (muxfd3->muxfd1)\n") ;
    muxid3 = user_ioctl(muxfd3, I_LINK, muxfd1) ; /* link muxfd1 below muxfd3 */
    if (muxid3 < 0)
    {
	print("mux_clone: I_LINK: %s\n", strerror(-muxid3)) ;
	xit() ;
    }
    else
	print("                   muxid=%d\n", muxid3) ;

    print("mux_test: I_LINK mux driver under mux (muxfd4->muxfd2)\n") ;
    muxid4 = user_ioctl(muxfd4, I_LINK, muxfd2) ; /* link muxfd2 below muxfd4 */
    if (muxid4 < 0)
    {
	print("mux_clone: I_LINK: %s\n", strerror(-muxid4)) ;
	xit() ;
    }
    else
	print("                   muxid=%d\n", muxid4) ;

    print_stream(muxfd3) ;
    print_stream(muxfd4) ;
    print("mux_test: close files to loop driver (now detached)\n") ;
    user_close(fd1) ;
    user_close(fd2) ;
    print("mux_test: close files to lower mux driver (now detached)\n") ;
    user_close(muxfd1) ;
    user_close(muxfd2) ;

    print("mux_test: send data down through the mux/mux/loop and read back\n") ;
    strcpy(buf, "Test data for sending through the mini-mux") ;
    lgth = strlen(buf) ;
    do_get_put(muxfd3, muxfd4, -1, lgth, 0) ;

	    /********************************
	    *         Close Files           * 
	    ********************************/

    print("\nmux_test: closing control streams of cascaded muxs\n") ;
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


    print("\nTest 2 lowers under one control stream\n") ;
    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) xit() ;

    print("mux_test: open a mux-clone device\n") ;
    muxfd1 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	print("mux_clone: %s\n", strerror(-muxfd1)) ;
	xit() ;
    }

    print("mux_test: I_LINK loop driver under mux (muxfd1->fd1)\n") ;
    muxid1 = user_ioctl(muxfd1, I_LINK, fd1) ;	/* link fd1 below muxfd1 */
    if (muxid1 < 0)
    {
	print("mux_clone: I_LINK: %s\n", strerror(-muxid1)) ;
	xit() ;
    }
    else
	print("                   muxid=%d\n", muxid1) ;

    print("mux_test: I_LINK loop driver under mux (muxfd1->fd2)\n") ;
    muxid2 = user_ioctl(muxfd1, I_LINK, fd2) ;	/* link fd2 below muxfd2 */
    if (muxid2 < 0)
    {
	print("mux_clone: I_LINK: %s\n", strerror(-muxid2)) ;
	xit() ;
    }
    else
	print("                   muxid=%d\n", muxid2) ;

    print_stream(muxfd1) ;
    print("mux_test: close files to loop driver (now detached)\n") ;
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
	print("minimux.1: ioctl MINIMUX_DOWN: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    ioc.ic_cmd 	  = MINIMUX_UP ;	/* set upstream linkage */
    arg = muxid2 ;			/* upstream muxfd1 <- muxid2 */
    ioc.ic_len	  = sizeof(int) ;
    rslt = user_ioctl(muxfd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("minimux.1: ioctl MINIMUX_UP: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    print("mux_test: send data down through the mux/loop and read back\n") ;
    strcpy(buf, "Test data for sending through the mini-mux") ;
    lgth = strlen(buf) ;
    do_get_put(muxfd1, muxfd1, -1, lgth, 0) ;

	    /********************************
	    *         Close Files           * 
	    ********************************/

    print("\nmux_test: closing control streams\n") ;
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

    print("\nTest reconnecting lower after explicit I_UNLINK\n") ;
    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) xit() ;

    print("mux_test: open a mux-clone device\n") ;
    muxfd1 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	print("mux_clone: %s\n", strerror(-muxfd1)) ;
	xit() ;
    }

    print("mux_test: open another mux-clone device\n") ;
    muxfd2 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd2 < 0)
    {
	print("mux_clone: %s\n", strerror(-muxfd2)) ;
	xit() ;
    }

    print("mux_test: I_LINK loop driver under mux (muxfd1->fd1)\n") ;
    muxid1 = user_ioctl(muxfd1, I_LINK, fd1) ;	/* link fd1 below muxfd1 */
    if (muxid1 < 0)
    {
	print("mux_clone: I_LINK: %s\n", strerror(-muxid1)) ;
	xit() ;
    }
    else
	print("                   muxid=%d\n", muxid1) ;

    print("mux_test: I_LINK loop driver under mux (muxfd2->fd2)\n") ;
    muxid2 = user_ioctl(muxfd2, I_LINK, fd2) ;	/* link fd2 below muxfd2 */
    if (muxid2 < 0)
    {
	print("mux_clone: I_LINK: %s\n", strerror(-muxid2)) ;
	xit() ;
    }
    else
	print("                   muxid=%d\n", muxid2) ;

    print_stream(muxfd1) ;
    print_stream(muxfd2) ;
    print("mux_test: leave open files to loop driver\n") ;

    print("mux_test: send data down through the mux/loop and read back\n") ;
    strcpy(buf, "Test data for sending through the mini-mux") ;
    lgth = strlen(buf) ;
    do_get_put(muxfd1, muxfd2, -1, lgth, 0) ;

    print("mux_test: I_UNLINK muxid1 from muxfd1\n") ;
    rslt = user_ioctl(muxfd1, I_UNLINK, muxid1) ;
    if (rslt < 0)
    {
	print("minimux: I_UNLINK: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    print("mux_test: I_UNLINK muxid2 from muxfd2\n") ;
    rslt = user_ioctl(muxfd2, I_UNLINK, muxid2) ;
    if (rslt < 0)
    {
	print("minimux: I_UNLINK: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    print("mux_test: send data down through the loop and read back\n") ;
    do_get_put(fd1, fd2, -1, lgth, 0) ;

	    /********************************
	    *         Close Files           * 
	    ********************************/

    print("\nmux_test: close control streams\n") ;
    user_close(muxfd1) ;
    user_close(muxfd2) ;
    print("mux_test: send data down through the loop and read back\n") ;
    do_get_put(fd1, fd2, -1, lgth, 0) ;
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

    print("\nTest I_PLINK/I_PUNLINK\n") ;
    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) xit() ;

    print("mux_test: open minor 1 of mux device\n") ;
    muxfd1 = user_open(MUX_1, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	print("mux_clone: %s\n", strerror(-muxfd1)) ;
	xit() ;
    }

    print("mux_test: open a mux-clone device\n") ;
    muxfd2 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd2 < 0)
    {
	print("mux_clone: %s\n", strerror(-muxfd2)) ;
	xit() ;
    }

    print("mux_test: I_PLINK loop driver under mux (muxfd1->fd1)\n") ;
    muxid1 = user_ioctl(muxfd1, I_PLINK, fd1) ;	/* link fd1 below muxfd1 */
    if (muxid1 < 0)
    {
	print("mux_clone: I_PLINK: %s\n", strerror(-muxid1)) ;
	xit() ;
    }
    else
	print("                   muxid=%d\n", muxid1) ;

    print("mux_test: I_PLINK loop driver under mux (muxfd2->fd2)\n") ;
    muxid2 = user_ioctl(muxfd2, I_PLINK, fd2) ;	/* link fd2 below muxfd2 */
    if (muxid2 < 0)
    {
	print("mux_clone: I_PLINK: %s\n", strerror(-muxid2)) ;
	xit() ;
    }
    else
	print("                   muxid=%d\n", muxid2) ;

    print_stream(muxfd1) ;
    print_stream(muxfd2) ;
    print("mux_test: leave open files to loop driver\n") ;

    print("mux_test: send data down through the mux/loop and read back\n") ;
    strcpy(buf, "Test data for sending through the mini-mux") ;
    lgth = strlen(buf) ;
    do_get_put(muxfd1, muxfd2, -1, lgth, 0) ;

    print("mux_test: close muxfd1 \n") ;
    user_close(muxfd1) ;
    print("mux_test: re-open muxfd1 to minor 1\n") ;
    muxfd1 = user_open(MUX_1, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	print("mux_clone: %s\n", strerror(-muxfd1)) ;
	xit() ;
    }

    print("mux_test: I_PUNLINK muxid1 from muxfd1\n") ;
    print_stream(muxfd1) ;
    rslt = user_ioctl(muxfd1, I_PUNLINK, muxid1) ;
    if (rslt < 0)
    {
	print("minimux: I_PUNLINK: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    print("mux_test: stream linkage after I_PUNLINK\n") ;
    print_stream(muxfd1) ;

    print("mux_test: I_PUNLINK muxid2 from muxfd2\n") ;
    print_stream(muxfd2) ;
    rslt = user_ioctl(muxfd2, I_PUNLINK, muxid2) ;
    if (rslt < 0)
    {
	print("minimux: I_PUNLINK: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    print("mux_test: stream linkage after I_PUNLINK\n") ;
    print_stream(muxfd2) ;

    print("mux_test: send data down through the loop and read back\n") ;
    do_get_put(fd1, fd2, -1, lgth, 0) ;


	    /********************************
	    *         Close Files           * 
	    ********************************/

    print("\nmux_test: close control streams\n") ;
    user_close(muxfd1) ;
    user_close(muxfd2) ;
    print("mux_test: send data down through the loop and read back\n") ;
    do_get_put(fd1, fd2, -1, lgth, 0) ;
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

    print("\nTest I_PLINK/I_PUNLINK with re-open\n") ;
    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) xit() ;

    print("mux_test: open minimux.1\n") ;
    muxfd1 = user_open(MUX_1, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	print(MUX_1 ": %s\n", strerror(-muxfd1)) ;
	xit() ;
    }

    print("mux_test: open minimux.2\n") ;
    muxfd2 = user_open(MUX_2, O_RDWR, 0) ;
    if (muxfd2 < 0)
    {
	print(MUX_2 ": %s\n", strerror(-muxfd2)) ;
	xit() ;
    }

    print("mux_test: I_PLINK loop driver under mux (muxfd1->fd1)\n") ;
    muxid1 = user_ioctl(muxfd1, I_PLINK, fd1) ;	/* link fd1 below muxfd1 */
    if (muxid1 < 0)
    {
	print("mux_clone: I_PLINK: %s\n", strerror(-muxid1)) ;
	xit() ;
    }
    else
	print("                   muxid=%d\n", muxid1) ;

    print("mux_test: I_PLINK loop driver under mux (muxfd2->fd2)\n") ;
    muxid2 = user_ioctl(muxfd2, I_PLINK, fd2) ;	/* link fd2 below muxfd2 */
    if (muxid2 < 0)
    {
	print("mux_clone: I_PLINK: %s\n", strerror(-muxid2)) ;
	xit() ;
    }
    else
	print("                   muxid=%d\n", muxid2) ;

    print_stream(muxfd1) ;
    print_stream(muxfd2) ;
    print("mux_test: leave open files to loop driver\n") ;

    print("mux_test: send data down through the mux/loop and read back\n") ;
    strcpy(buf, "Test data for sending through the mini-mux") ;
    lgth = strlen(buf) ;
    do_get_put(muxfd1, muxfd2, -1, lgth, 0) ;

    print("mux_test: close mux streams\n") ;

    user_close(muxfd1) ;
    user_close(muxfd2) ;

    print("mux_test: re-open first mux-clone stream\n") ;
    print("mux_test: re-open minimux.1\n") ;
    muxfd1 = user_open(MUX_1, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	print(MUX_1 ": %s\n", strerror(-muxfd1)) ;
	xit() ;
    }

    print("mux_test: re-open minimux.2\n") ;
    muxfd2 = user_open(MUX_2, O_RDWR, 0) ;
    if (muxfd2 < 0)
    {
	print(MUX_2 ": %s\n", strerror(-muxfd2)) ;
	xit() ;
    }

    print("mux_test: print mux topology\n") ;
    print_stream(muxfd1) ;
    print_stream(muxfd2) ;

    print("mux_test: "
    	  "send data down through the re-opened mux/loop and read back\n") ;
    strcpy(buf, "Test data for sending through the re-opened mini-mux") ;
    lgth = strlen(buf) ;
    do_get_put(muxfd1, muxfd2, -1, lgth, 0) ;

    print("mux_test: I_PUNLINK muxid1 from muxfd1\n") ;
    rslt = user_ioctl(muxfd1, I_PUNLINK, muxid1) ;
    if (rslt < 0)
    {
	print("minimux: I_PUNLINK: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    print("mux_test: I_PUNLINK muxid2 from muxfd2\n") ;
    rslt = user_ioctl(muxfd2, I_PUNLINK, muxid2) ;
    if (rslt < 0)
    {
	print("minimux: I_PUNLINK: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    print("mux_test: send data down through the loop and read back\n") ;
    do_get_put(fd1, fd2, -1, lgth, 0) ;

	    /********************************
	    *         Close Files           * 
	    ********************************/

    print("\nmux_test: close control streams\n") ;
    user_close(muxfd1) ;
    user_close(muxfd2) ;
    print("mux_test: send data down through the loop and read back\n") ;
    do_get_put(fd1, fd2, -1, lgth, 0) ;
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

    print("\nTest I_PLINK/I_PUNLINK MUXID_ALL \n") ;
    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) xit() ;

    print("mux_test: open minimux.1\n") ;
    muxfd1 = user_open(MUX_1, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	print(MUX_1 ": %s\n", strerror(-muxfd1)) ;
	xit() ;
    }

    print("mux_test: open minimux.2\n") ;
    muxfd2 = user_open(MUX_2, O_RDWR, 0) ;
    if (muxfd2 < 0)
    {
	print(MUX_2 ": %s\n", strerror(-muxfd2)) ;
	xit() ;
    }

    print("mux_test: I_PLINK loop driver under mux (muxfd1->fd1)\n") ;
    muxid1 = user_ioctl(muxfd1, I_PLINK, fd1) ;	/* link fd1 below muxfd1 */
    if (muxid1 < 0)
    {
	print("mux_clone: I_PLINK: %s\n", strerror(-muxid1)) ;
	xit() ;
    }
    else
	print("                   muxid=%d\n", muxid1) ;

    print("mux_test: I_PLINK loop driver under mux (muxfd2->fd2)\n") ;
    muxid2 = user_ioctl(muxfd2, I_PLINK, fd2) ;	/* link fd2 below muxfd2 */
    if (muxid2 < 0)
    {
	print("mux_clone: I_PLINK: %s\n", strerror(-muxid2)) ;
	xit() ;
    }
    else
	print("                   muxid=%d\n", muxid2) ;

    print_stream(muxfd1) ;
    print_stream(muxfd2) ;
    print("mux_test: leave open files to loop driver\n") ;

    print("mux_test: send data down through the mux/loop and read back\n") ;
    strcpy(buf, "Test data for sending through the mini-mux") ;
    lgth = strlen(buf) ;
    do_get_put(muxfd1, muxfd2, -1, lgth, 0) ;

    print("mux_test: close mux streams\n") ;

    user_close(muxfd1) ;
    user_close(muxfd2) ;

    print("mux_test: re-open first mux-clone stream\n") ;
    muxfd1 = user_open(MUX_CLONE, O_RDWR, 0) ;
    if (muxfd1 < 0)
    {
	print(MUX_1 ": %s\n", strerror(-muxfd1)) ;
	xit() ;
    }

    print("mux_test: I_PUNLINK MUXID_ALL from mux-clone stream\n") ;
    rslt = user_ioctl(muxfd1, I_PUNLINK, MUXID_ALL) ;
    if (rslt < 0)
    {
	print("minimux: I_PUNLINK(MUXID_ALL): %s\n", strerror(-rslt)) ;
	xit() ;
    }

	    /********************************
	    *         Close Files           * 
	    ********************************/

    print("\nmux_test: close control streams\n") ;
    user_close(muxfd1) ;
    print("mux_test: send data down through the loop and read back\n") ;
    do_get_put(fd1, fd2, -1, lgth, 0) ;
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
void	clone_test(void)
{
    int			fd1 ;
    int			fd2 ;
    int			fd3 ;
    int			fd4 ;
    int			fd5 ;
    int			rslt ;

    print("\nclone open test\n") ;

#ifdef DIRECT_USER
    print("\n\n\nDirectory listing at start of test\n\n") ;
    user_print_dir(NULL, USR_PRNT_INODE) ;
#endif

    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) xit() ;

    rslt = open_clones(&fd3, &fd4) ;
    if (rslt < 0) xit() ;

    fd5 = user_open(LOOP_1, O_RDWR, 0) ;
    if (fd5 < 0) xit() ;

#ifdef DIRECT_USER
    print("\n\n\nDirectory listing after opens\n\n") ;
    user_print_dir(NULL, USR_PRNT_INODE) ;
    user_print_inodes() ;
#endif

    user_close(fd1) ;
    user_close(fd2) ;
    user_close(fd3) ;
    user_close(fd4) ;
    user_close(fd5) ;

#ifdef DIRECT_USER
    print("\n\n\nDirectory listing after closes\n\n") ;
    user_print_dir(NULL, USR_PRNT_INODE) ;
    user_print_inodes() ;
#endif

    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) xit() ;

    rslt = open_clones(&fd3, &fd4) ;
    if (rslt < 0) xit() ;

    fd5 = user_open(LOOP_1, O_RDWR, 0) ;
    if (fd5 < 0) xit() ;

#ifdef DIRECT_USER
    print("\n\n\nDirectory listing after 2nd round of opens\n\n") ;
    user_print_dir(NULL, USR_PRNT_INODE) ;
    user_print_inodes() ;
#endif

    user_close(fd1) ;
    user_close(fd2) ;
    user_close(fd3) ;
    user_close(fd4) ;
    user_close(fd5) ;

#ifdef DIRECT_USER
    print("\n\n\nDirectory listing after 2nd round of closes\n\n") ;
    user_print_dir(NULL, USR_PRNT_INODE) ;
    user_print_inodes() ;
#endif

} /* clone_test */

/************************************************************************
*                          bufcall_test                                 *
************************************************************************/
void bufcall_test(void)
{
    int			fd1 ;
    int			fd2 ;
    int			rslt ;
    int			lgth ;
    struct strioctl	ioc ;

    print("\nbufcall test\n") ;

	    /********************************
	    *         Open Files            * 
	    ********************************/

    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) xit() ;

	    /********************************
	    *         putmsg/read           *
	    ********************************/

    print("\nUse putmsg to send data, use read to read back data\n") ;
    memset(rdctlbuf, 0, sizeof(rdctlbuf)) ;

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
	print("loop_clone.1: ioctl LOOP_BUFCALL: %s\n", strerror(-rslt)) ;
    }

    strcpy(buf, "Test data for bufcall putmsg/read") ;
    lgth = strlen(buf) ;
    wr_ctl.len	= -1 ;
    wr_dta.len	= lgth ;
    if (put_msg(fd1, &wr_ctl, &wr_dta, 0, MSG_BAND) < 0) xit() ;

    memset(rdbuf, 0, sizeof(rdbuf)) ;
    nread_wait_msgs(fd2, 1) ;
    rslt = user_read(fd2, rdbuf, lgth);
    if (rslt < 0)
    {
	print("loop_clone.2: read: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    if (rslt != lgth)
    {
	print("loop_clone.2:  read returned %d, expected %d\n", rslt, lgth) ;
	xit() ;
    }

    if (strcmp(buf, rdbuf))
    {
	print("loop_clone.2: read: buffer compare error\n") ;
	print("              wrote \"%s\"\n", buf) ;
	print("              read  \"%s\"\n", rdbuf) ;
    }
    else
	print("loop_clone.2: read %d bytes: buffer compared OK\n", rslt) ;




	    /********************************
	    *         Close Files           * 
	    ********************************/

    print("\nbufcall_test: closing files\n") ;
    user_close(fd1) ;
    user_close(fd2) ;

} /* bufcall_test */

/************************************************************************
*                           sad_vml_test                                *
************************************************************************/
int sad_vml_test(int fd, struct str_list *list)
{
	int rslt;

	rslt = user_ioctl(fd, SAD_VML, list);
	if (rslt < 0) {
		print("sad_vml_test: SAD_VML ioctl failed: %s\n", strerror(-rslt));
		xit();
	}
	return rslt;
}

/************************************************************************
*                             sad_test                                  *
************************************************************************/
void sad_test(void)
{
	int fd, fd2;
	int rslt;
	struct str_mlist mlist[4];
	struct str_list list;
	struct strapush apush;

	print("\nSTREAMS Administrative Driver test\n");

	print("sad_test: open a sad device\n");
	fd = user_open(SAD_CLONE, O_RDWR, 0);
	if (fd < 0) {
		print("sad_test: SAD driver open: %s\n", strerror(-fd));
		xit();
	}


	print("sad_test: Testing SAD_VML IOCTL\n");

	print("sad_test: Testing SAD_VML ioctl (a bad module)\n");
	strcpy(mlist[0].l_name, "nosuchmodule");
	list.sl_nmods = 1;
	list.sl_modlist = mlist;
	rslt = sad_vml_test(fd, &list);
	if (rslt == 0) {
		print("sad_test: SAD says a module "
		      "named \"nosuchmodule\" exists.\n");
		print("sad_test: Does it?\n");
		xit();
	}

	print("sad_test: Testing SAD_VML ioctl (a good and a bad module)\n");
	strcpy(mlist[0].l_name, "relay");
	strcpy(mlist[1].l_name, "nosuchmodule");
	list.sl_nmods = 2;
	rslt = sad_vml_test(fd, &list);
	if (rslt == 0) {
		print("sad_test: SAD says modules"
		      " \"relay\" and \"nosuchmodule\" exists.\n");
		print("sad_test: Do they both exist?\n");
		xit();
	}

	print("sad_test: Testing SAD_VML ioctl (two good modules)\n");
	strcpy(mlist[1].l_name, "relay2");
	rslt = sad_vml_test(fd, &list);
	if (rslt == 1) {
		print("sad_test: SAD says modules"
		      " \"relay\" and \"relay2\" do not both exist.\n");
		print("sad_test: Shouldn't they both exist?\n");
		xit();
	}

	print("sad_test: SAD_VML IOCTL tests passed\n");


	print("sad_test: Testing SAD_GAP and SAD_SAP IOCTLs\n");

	/* Check that autopush is unconfigured */
	apush.sap_major = SLOOP__CMAJOR_0;
	apush.sap_minor = 1;
	rslt = user_ioctl(fd, SAD_GAP, &apush);
	if (rslt >= 0) {
		print("sad_test: SAD says autopush is configured "
		      "for loop minor 1.\n");
		print("sad_test: Shouldn't it be unconfigured?\n");
		xit();
	}
#ifdef DIRECT_USER
	if (-rslt == ENODEV)
		print("sad_test: Autopush is unconfigured for loop minor 1.\n");
	else {
		print("sad_test: SAD_GAP ioctl failed: %s\n", strerror(-rslt));
		xit();
	}
#else
	if (errno == ENODEV)
		print("sad_test: Autopush is unconfigured for loop minor 1.\n");
	else {
		print("sad_test: SAD_GAP ioctl failed: %s\n", strerror(errno));
		xit();
	}
#endif

	/* Configure autopush */
	apush.sap_cmd = SAP_ONE;
	apush.sap_major = SLOOP__CMAJOR_0;
	apush.sap_minor = 1;
	apush.sap_npush = 2;
	strcpy(apush.sap_list[0], "relay");
	strcpy(apush.sap_list[1], "relay2");
	rslt = user_ioctl(fd, SAD_SAP, &apush);
	if (rslt < 0) {
		print("sad_test: SAD_SAP ioctl failed: %s\n", strerror(-rslt));
		xit();
	}

	/* Test autopush */
	fd2 = user_open(LOOP_1, O_RDWR, 0);
	if (fd2 < 0) {
		print("sad_test: loop driver open: %s\n", strerror(-fd));
		xit();
	}
	list.sl_nmods = 4;
	rslt = user_ioctl(fd2, I_LIST, &list);
	if (rslt < 0) {
		print("sad_test: I_LIST: %s\n", strerror(-rslt));
		xit();
	}
	if (list.sl_nmods != 3) {
		print("sad_test: loop driver open autopushed %d modules.\n");
		print("sad_test: Expected 2 autopushed modules.\n");
		xit();
	}
	if (strcmp(mlist[0].l_name, "relay2")) {
		print("sad_test: Second autopushed module is \"%s\".\n",
		      mlist[0].l_name);
		print("sad_test: Expected \"relay2\".\n");
		xit();
	}
	if (strcmp(mlist[1].l_name, "relay")) {
		print("sad_test: First autopushed module is \"%s\".\n",
		      mlist[0].l_name);
		print("sad_test: Expected \"relay\".\n");
		xit();
	}
	user_close(fd2);
	print("sad_test: Autopush tested OK.\n");

	/* Remove autopush configuration */
	apush.sap_cmd = SAP_CLEAR;
	apush.sap_major = SLOOP__CMAJOR_0;
	apush.sap_minor = 1;
	rslt = user_ioctl(fd, SAD_SAP, &apush);
	if (rslt < 0) {
		print("sad_test: SAD_SAP ioctl failed: %s\n", strerror(-rslt));
		xit();
	}
	
	/* Check that autopush configuration was removed */
	apush.sap_major = SLOOP__CMAJOR_0;
	apush.sap_minor = 1;
	rslt = user_ioctl(fd, SAD_GAP, &apush);
	if (rslt >= 0) {
		print("sad_test: SAD says autopush is configured "
		      "for loop minor 1.\n");
		print("sad_test: But I just cleared the configuration\n");
		xit();
	}
#ifdef DIRECT_USER
	if (-rslt != ENODEV) {
		print("sad_test: SAD_GAP ioctl failed: %s\n", strerror(-rslt));
		xit();
	}
#else
	if (errno != ENODEV) {
		print("sad_test: SAD_GAP ioctl failed: %s\n", strerror(errno));
		xit();
	}
#endif

	print("sad_test: SAD_GAP and SAD_SAP IOCTL tests passed\n");

	print("sad_test: closing files\n");
	user_close(fd);
}

/************************************************************************
*                             fifo_test                                 *
************************************************************************/
void fifo_test(void)
{
    int fd[2];
    int i;
    struct user_stat stat;

    print("\nSTREAMS-based FIFO/pipe test\n");
    
    for (i = 0;  i < 2;  i++) {
	fd[i] = user_open(CLONE_FIFO, O_RDWR, 0);
	if (fd[i] < 0) {
	    print("fifo_test: fifo open: %s\n", strerror(errno) );
	    xit();
	}
	user_fstat( fd[i], &stat );
	print("fifo_test: open(\"%s\",O_RDWR) #%d: fd %d mode 0%o dev 0x%x\n",
	      CLONE_FIFO, i+1, fd[i],
	      (int)stat.st_mode, (int)stat.st_rdev );
    }
    user_close(fd[0]);
    user_close(fd[1]);
    
    for (i = 0;  i < 2;  i++) {
	fd[i] = user_open(FIFO_0, O_RDWR, 0);
	if (fd[i] < 0) {
	    print("fifo_test: fifo open: %s\n", strerror(errno) );
	    xit();
	}
	user_fstat( fd[i], &stat );
	print("fifo_test: open(\"%s\",O_RDWR) #%d: fd %d mode 0%o dev 0x%x\n",
	      FIFO_0, i+1, fd[i],
	      (int)stat.st_mode, (int)stat.st_rdev );
    }
    user_close(fd[0]);
    user_close(fd[1]);
    
    if ((user_pipe(fd)) < 0) {
	print("pipe: %s\n", strerror(errno) );
	xit();
    }
    user_ioctl( fd[0], I_PUSH, "pipemod" );

    user_fstat( fd[0], &stat );
    print("fifo_test: pipe() 0: fd %d mode 0%o dev 0x%x\n",
	  fd[0], (int)stat.st_mode, (int)stat.st_rdev );
    user_fstat( fd[1], &stat );
    print("fifo_test: pipe() 1: fd %d mode 0%o dev 0x%x\n",
	  fd[1], (int)stat.st_mode, (int)stat.st_rdev );

    user_close(fd[0]);
    user_close(fd[1]);
}

/************************************************************************
*                           passfd_test                                 *
************************************************************************/
void passfd_test(void)
{
    int fd[2], sendfd;
    struct strrecvfd recv;
    struct user_stat stat;
    
    print("\nI_SENDFD/I_RECVFD test\n");
    
    if ((user_pipe(fd)) < 0) {
	print("pipe: %s\n", strerror(errno) );
	xit();
    }
    user_fstat( fd[0], &stat );
    print("passfd_test: pipe() 0: fd %d mode 0%o dev 0x%x\n",
	  fd[0], (int)stat.st_mode, (int)stat.st_rdev );
    user_fstat( fd[1], &stat );
    print("passfd_test: pipe() 1: fd %d mode 0%o dev 0x%x\n",
	  fd[1], (int)stat.st_mode, (int)stat.st_rdev );

    sendfd = user_open(FIFO_0, O_RDWR, 0);
    if (sendfd < 0) {
	print("passfd_test: open(\"%s\",O_RDWR) failed: %s\n",
	      FIFO_0, strerror(errno) );
	xit();
    }
    user_fstat( sendfd, &stat );
    print("passfd_test: sending fd %d mode 0%o dev 0x%x\n",
	  sendfd, (int)stat.st_mode, (int)stat.st_rdev );
    
    if (user_ioctl( fd[0], I_SENDFD, sendfd ) < 0) {
	print("ioctl( %d, I_SENDFD, %d ) failed: %s\n",
	      fd[0], sendfd, strerror(errno) );
	xit();
    }
    sleep(1) ;			/* allow time for service queues */
    print("passfd_test: closing sendfd %d\n", sendfd);
    user_close(sendfd);
    print("passfd_test: closing pipe fd[0] %d\n", fd[0]);
    user_close(fd[0]);
    
    if (user_ioctl( fd[1], I_RECVFD, &recv ) < 0) {
	print("ioctl( %d, I_RECVFD, ... ) failed: %s\n",
	      fd[1], strerror(errno) );
	xit();
    }
    user_fstat( recv.fd, &stat );
    print("passfd_test: received fd %d mode 0%o dev 0x%x\n",
	  recv.fd, (int)stat.st_mode, (int)stat.st_rdev );

    print("passfd_test: closing pipe fd[1] %d\n", fd[1]);
    user_close(fd[1]);
    print("passfd_test: closing recv.fd %d\n", recv.fd);
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

void ck_band(int fd, int band, int expected)
{
    int		rslt ;

    rslt = user_ioctl(fd, I_CKBAND, band) ;
    if (rslt < 0)
    {
	print("ck_band(%d): I_CKBAND: %s\n", band, strerror(-rslt)) ;
	xit() ;
    }

    if (rslt == expected)
    {
	print("ck_band(%d): I_CKBAND: got expected result %d\n", band, rslt) ;
	return ;
    }

    print("ck_band(%d): I_CKBAND: expected %d got %d\n", band, expected, rslt) ;
    xit() ;
}

void send_band(int fd, int band, int seq, int mtype)
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
	sprintf(buf, "Seq=%d Band=%d M_DATA", seq, band) ;
	wr_dta.len = strlen(buf) ;
	wr_ctl.len = -1 ;		/* no control msg */
    }
    else
    {
	sprintf(ctlbuf, "Seq=%d Band=%d M_PCPROTO", seq, band) ;
	wr_ctl.len = strlen(ctlbuf) ;
	wr_dta.len = -1 ;		/* no data msg */
    }

    if (put_msg(fd, &wr_ctl, &wr_dta, band, flags) < 0)
	xit() ;
}

int	receive_band_quiet ;

void receive_band(int fd, int band, int seq, int mtype)
{
    int			flags = MSG_BAND;
    int			rband = band ;
    int			rslt ;
    int			lgth ;
    char	       *ptr ;
    char	       *mnemonic ;
    int			len ;

    rd_ctl.len		= -1 ;
    rd_ctl.maxlen	= sizeof(rdctlbuf) ;
    rd_dta.len		= -1 ;
    rd_dta.maxlen	= sizeof(rdbuf) ;

    memset(rdctlbuf, 0, sizeof(rdctlbuf)) ;
    memset(rdbuf, 0, sizeof(rdbuf)) ;

    rslt = user_getpmsg(fd, &rd_ctl, &rd_dta, &rband, &flags) ;
    if (rslt < 0)
    {
	print("receive_band(%d): %s\n", band, strerror(-rslt)) ;
	xit() ;
    }

    if (band != rband)
    {
	print("receive_band: expected msg on band %d, got one on band %d\n",
		band, rband) ;
	xit() ;
    }

    if (mtype == _M_PCPROTO && !(flags & MSG_HIPRI))
    {
	print("receive_band: expected hi-pri msg, got normal-pri instead\n");
	xit() ;
    }

    if (mtype != _M_PCPROTO && (flags & MSG_HIPRI))
    {
	print("receive_band: expected normal-pri msg, got hi-pri instead\n");
	xit() ;
    }

    if (mtype == _M_DATA)
    {
	if (rd_ctl.len > 0)
	{
	    print("receive_band: expected M_DATA, got M_PROTO instead\n");
	    xit() ;
	}

	ptr = rdbuf ;
	len = rd_dta.len ;
	mnemonic = "M_DATA" ;
    }
    else
    {
	if (rd_dta.len > 0)
	{
	    print("receive_band: expected M_PROTO, got M_DATA instead\n");
	    xit() ;
	}

	ptr = rdctlbuf ;
	len = rd_ctl.len ;
	mnemonic = "M_PCPROTO" ;
    }

    sprintf(buf, "Seq=%d Band=%d %s", seq, band, mnemonic) ;
    lgth = strlen(buf) ;

    if (len != lgth)
    {
	print("receive_band: expected %d bytes, got %d bytes\n", lgth, len) ;
	xit() ;
    }

    if (strcmp(buf, ptr) != 0)
    {
	print("receive_band: buffer compare error\n") ;
	print("              wrote \"%s\"\n", buf) ;
	print("              read  \"%s\"\n", ptr) ;
	xit() ;
    }

    if (!receive_band_quiet)
	print("receive_band: OK: %s\n", ptr) ;
}

void band_test(void)
{
    int			fd1 ;
    int			fd2 ;
    int			rslt ;
    int			send_seq ;
    int			n_sent ;
    int			i ;
    int			flow_ok ;
    struct strioctl	ioc ;

    print("\nQueue band test\n") ;

    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) xit() ;

    ioc.ic_timout = 10 ;
    ioc.ic_dp	  = NULL;
    ioc.ic_cmd 	  = LOOP_PUTNXT ;  /* use putnext rather then svcq */
    ioc.ic_len	  = 0 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop_clone.1: ioctl LOOP_PUTNXT: %s\n", strerror(-rslt)) ;
    }

    rslt = user_ioctl(fd2, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop_clone.2: ioctl LOOP_PUTNXT: %s\n", strerror(-rslt)) ;
    }


    print("\nband_test: band 0 (easy case)\n") ;
    send_band(fd1, 0, 1, _M_DATA) ;
    ck_band(fd2, 0, 1) ;
    receive_band(fd2, 0, 1, _M_DATA) ;
    ck_band(fd2, 0, 0) ;

    print("\nband_test: write band 0, 1; read band 1, 0\n") ;
    send_band(fd1, 0, 2, _M_DATA) ;
    send_band(fd1, 1, 3, _M_DATA) ;
    ck_band(fd2, 1, 1) ;
    receive_band(fd2, 1, 3, _M_DATA) ;
    ck_band(fd2, 1, 0) ;
    receive_band(fd2, 0, 2, _M_DATA) ;

    print("\nband_test: write band 1, 2, 3; read band 3, 2, 1\n") ;
    send_band(fd1, 1, 4, _M_DATA) ;
    send_band(fd1, 2, 5, _M_DATA) ;
    send_band(fd1, 3, 6, _M_DATA) ;
    ck_band(fd2, 3, 1) ;
    receive_band(fd2, 3, 6, _M_DATA) ;
    ck_band(fd2, 3, 0) ;
    ck_band(fd2, 2, 1) ;
    receive_band(fd2, 2, 5, _M_DATA) ;
    ck_band(fd2, 2, 0) ;
    ck_band(fd2, 1, 1) ;
    receive_band(fd2, 1, 4, _M_DATA) ;
    ck_band(fd2, 1, 0) ;

    print("\nband_test: write band 1, 3; read band 3, 1\n") ;
    send_band(fd1, 1, 7, _M_DATA) ;
    send_band(fd1, 3, 8, _M_DATA) ;
    receive_band(fd2, 3, 8, _M_DATA) ;
    receive_band(fd2, 1, 7, _M_DATA) ;

    print("\nband_test: write band 1, 3, 1; read band 3, 1, 1\n") ;
    send_band(fd1, 1, 9, _M_DATA) ;
    send_band(fd1, 3, 10, _M_DATA) ;
    send_band(fd1, 1, 11, _M_DATA) ;
    receive_band(fd2, 3, 10, _M_DATA) ;
    receive_band(fd2, 1, 9, _M_DATA) ;
    receive_band(fd2, 1, 11, _M_DATA) ;

    print("\nband_test: "
	    "write band 1, 2, M_PCPROTO; read M_PCPROTO, band 2, 1\n");
    send_band(fd1, 1, 12, _M_DATA) ;
    send_band(fd1, 2, 13, _M_DATA) ;
    send_band(fd1, 0, 14, _M_PCPROTO) ;
    receive_band(fd2, 0, 14, _M_PCPROTO) ;
    receive_band(fd2, 2, 13, _M_DATA) ;
    receive_band(fd2, 1, 12, _M_DATA) ;

    print("\nband_test: "
	    "fill up band 1, then write band 2; read 2, 1, 1, ...\n");
    send_seq = 10000 ;
    for (n_sent = 0; user_ioctl(fd1, I_CANPUT, 1) == 1 ; n_sent++)
    {
	send_band(fd1, 1, send_seq + n_sent, _M_DATA) ;
	if (n_sent == 0)
	    set_debug_mask(0x00000000L) ;
    }

    print("\nband_test: band 1 full (%d msgs), write band 2\n", n_sent) ;
    set_debug_mask(ALL_DEBUG_BITS) ;
    send_band(fd1, 2, send_seq + n_sent + 1, _M_DATA) ;
    receive_band(fd2, 2, send_seq + n_sent + 1, _M_DATA) ;
    for (i = 0; i < n_sent; i++)
    {
	receive_band(fd2, 1, send_seq + i, _M_DATA) ;
	if (i == 0)
	{
	    set_debug_mask(0x00000000L) ;
	    receive_band_quiet = 1 ;
	}
    }

    print("\nband_test: wr band 1, fill band 2, wr band 1; read 2,2,... 1\n");
    set_debug_mask(ALL_DEBUG_BITS) ;
    receive_band_quiet = 0 ;
    send_seq = 20000 ;
    send_band(fd1, 1, 20, _M_DATA) ;
    for (n_sent = 0; user_ioctl(fd1, I_CANPUT, 2) == 1 ; n_sent++)
    {
	send_band(fd1, 2, send_seq + n_sent, _M_DATA) ;
	if (n_sent == 0)
	    set_debug_mask(0x00000000L) ;
    }

    print("\nband_test: band 2 full (%d msgs), write band 1\n", n_sent) ;
    send_band(fd1, 1, 21, _M_DATA) ;
    for (i = 0; i < n_sent; i++)
    {
	receive_band(fd2, 2, send_seq + i, _M_DATA) ;
	if (i == 0)
	{
	    set_debug_mask(0x00000000L) ;
	    receive_band_quiet = 1 ;
	}
    }
    set_debug_mask(ALL_DEBUG_BITS) ;
    receive_band_quiet = 0 ;
    receive_band(fd2, 1, 20, _M_DATA) ;
    receive_band(fd2, 1, 21, _M_DATA) ;


    print("\nband_test: wr band 1, fill band 2, wr band 1; test backenable\n");
    set_debug_mask(ALL_DEBUG_BITS) ;
    receive_band_quiet = 0 ;
    send_seq = 30000 ;
    send_band(fd1, 1, 22, _M_DATA) ;
    for (n_sent = 0; user_ioctl(fd1, I_CANPUT, 2) == 1 ; n_sent++)
    {
	send_band(fd1, 2, send_seq + n_sent, _M_DATA) ;
	if (n_sent == 0)
	    set_debug_mask(0x00000000L) ;
    }

    print("band_test: band 2 full (%d msgs), write band 1\n", n_sent) ;
    send_band(fd1, 1, 23, _M_DATA) ;
    flow_ok = 0 ;
    for (i = 0; i < n_sent; i++)
    {
	receive_band(fd2, 2, send_seq + i, _M_DATA) ;
	if (i == 0)
	{
	    set_debug_mask(0x00000000L) ;
	    receive_band_quiet = 1 ;
	}
	if (!flow_ok && user_ioctl(fd1, I_CANPUT, 2) == 1)
	{
	    flow_ok = 1 ;
	    print("band_test: backpressure relieved at %d messages queued\n",
		    n_sent - i) ;
	}
    }

    set_debug_mask(ALL_DEBUG_BITS) ;
    receive_band_quiet = 0 ;
    receive_band(fd2, 1, 22, _M_DATA) ;
    receive_band(fd2, 1, 23, _M_DATA) ;

    if (!flow_ok)
    {
	print("band_test: all messages read but backpressure still in place\n");
	xit() ;
    }


    user_close(fd1);
    user_close(fd2);

    print("End of band test\n") ;
}

/************************************************************************
*                          flush_test                                   *
************************************************************************/
void flush_test(void)
{
    int			fd1 ;
    int			fd2 ;
    int			rslt ;
    struct strioctl	ioc ;
    bandinfo_t		bi ;

    print("\nQueue flushing test\n") ;

    rslt = open_clones(&fd1, &fd2) ;
    if (rslt < 0) xit() ;

    ioc.ic_timout = 10 ;
    ioc.ic_dp	  = NULL;
    ioc.ic_cmd 	  = LOOP_PUTNXT ;  /* use putnext rather then svcq */
    ioc.ic_len	  = 0 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop_clone.1: ioctl LOOP_PUTNXT: %s\n", strerror(-rslt)) ;
    }

    rslt = user_ioctl(fd2, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop_clone.2: ioctl LOOP_PUTNXT: %s\n", strerror(-rslt)) ;
    }

    print("\nflush_test: simple band 0 data\n") ;
    send_band(fd1, 0, 1, _M_DATA) ;
    send_band(fd1, 0, 2, _M_DATA) ;

    rslt = nread_wait_msgs(fd2, 2) ;
    if (rslt != 2)
    {
	print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 2) ;
	xit() ;
    }

    rslt = user_ioctl(fd2, I_FLUSH, FLUSHRW) ;
    if (rslt < 0)
    {
	print("loop.2: I_FLUSH: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    rslt = flush_wait(fd2) ;
    if (rslt != 0)
    {
	print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 0) ;
	xit() ;
    }

    print("flush_test: simple band 0 data -- OK\n") ;


    print("\nflush_test: band 1 data via I_FLUSH\n") ;

    send_band(fd1, 1, 3, _M_DATA) ;
    send_band(fd1, 1, 4, _M_DATA) ;

    rslt = nread_wait_msgs(fd2, 2) ;
    if (rslt != 2)
    {
	print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 2) ;
	xit() ;
    }

    rslt = user_ioctl(fd2, I_FLUSH, FLUSHRW) ;
    if (rslt < 0)
    {
	print("loop.2: I_FLUSH: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    rslt = flush_wait(fd2) ;
    if (rslt != 0)
    {
	print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 0) ;
	xit() ;
    }

    print("flush_test: band 1 data via I_FLUSH -- OK\n") ;


    print("\nflush_test: band 1 data via I_FLUSHBAND\n") ;

    send_band(fd1, 1, 5, _M_DATA) ;
    send_band(fd1, 1, 6, _M_DATA) ;

    rslt = nread_wait_msgs(fd2, 2) ;
    if (rslt != 2)
    {
	print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 2) ;
	xit() ;
    }

    bi.bi_pri = 1 ;			/* flush band 1 */
    bi.bi_flag = FLUSHRW ;		/* flush both read/write */

    rslt = user_ioctl(fd2, I_FLUSHBAND, &bi) ;
    if (rslt < 0)
    {
	print("loop.2: I_FLUSHBAND: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    rslt = flush_wait(fd2) ;
    if (rslt != 0)
    {
	print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 0) ;
	xit() ;
    }

    print("flush_test: band 1 data via I_FLUSHBAND -- OK\n") ;


    print("\nflush_test: band 2 data via I_FLUSHBAND w/band 1 data present\n") ;

    send_band(fd1, 1, 7, _M_DATA) ;
    send_band(fd1, 1, 8, _M_DATA) ;
    send_band(fd1, 2, 9, _M_DATA) ;
    send_band(fd1, 2, 10, _M_DATA) ;

    rslt = nread_wait_msgs(fd2, 4) ;
    if (rslt != 4)
    {
	print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 4) ;
	xit() ;
    }

    bi.bi_pri = 2 ;			/* flush band 2 */
    bi.bi_flag = FLUSHRW ;		/* flush both read/write */

    rslt = user_ioctl(fd2, I_FLUSHBAND, &bi) ;
    if (rslt < 0)
    {
	print("loop.2: I_FLUSHBAND: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    receive_band(fd2, 1, 7, _M_DATA) ;
    receive_band(fd2, 1, 8, _M_DATA) ;

    rslt = flush_wait(fd2) ;
    if (rslt != 0)
    {
	print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 0) ;
	xit() ;
    }

    print("flush_test: "
	    "band 2 data via I_FLUSHBAND w/band 1 data present -- OK\n") ;

    /*
     * I_FLUSH does flush of data only.  Data includes M_DATA, M_PROTO
     * and M_PCPROTO.  So anything that we can send from user level
     * gets flushed.
     */
    print("\nflush_test: band 0 data + M_PCPROTO\n") ;
    send_band(fd1, 0, 11, _M_DATA) ;
    send_band(fd1, 0, 12, _M_DATA) ;
    send_band(fd1, 0, 13, _M_PCPROTO) ;

    rslt = nread_wait_msgs(fd2, 3) ;
    if (rslt != 3)
    {
	print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 3) ;
	xit() ;
    }

    rslt = user_ioctl(fd2, I_FLUSH, FLUSHRW) ;
    if (rslt < 0)
    {
	print("loop.2: I_FLUSH: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    rslt = flush_wait(fd2) ;
    if (rslt != 0)
    {
	print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 0) ;
	xit() ;
    }

    print("flush_test: band 0 data + M_PCPROTO -- OK\n") ;


    /*
     * I_FLUSHBAND on band 0 flushes "ordinary" messages only.  This
     * will leave an M_PCPROTO at the head of the queue.
     */
    print("\nflush_test: band 0 data + M_PCPROTO via I_FLUSHBAND\n") ;
    send_band(fd1, 0, 14, _M_DATA) ;
    send_band(fd1, 0, 15, _M_DATA) ;
    send_band(fd1, 0, 16, _M_PCPROTO) ;

    rslt = nread_wait_msgs(fd2, 3) ;
    if (rslt != 3)
    {
	print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 3) ;
	xit() ;
    }

    bi.bi_pri = 0 ;			/* flush band 0 */
    bi.bi_flag = FLUSHRW ;		/* flush both read/write */

    rslt = user_ioctl(fd2, I_FLUSHBAND, &bi) ;
    if (rslt < 0)
    {
	print("loop.2: I_FLUSH: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    receive_band(fd2, 0, 16, _M_PCPROTO) ;
    rslt = flush_wait(fd2) ;
    if (rslt != 0)
    {
	print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 0) ;
	xit() ;
    }

    print("flush_test: band 0 data + M_PCPROTO via I_FLUSHBAND -- OK\n") ;


    /*
     * Flushing band 1 data should also leave an M_PCPROTO unflushed
     * since it appears as band 0.
     */
    print("\nflush_test: band 1 data + M_PCPROTO via I_FLUSHBAND\n") ;
    send_band(fd1, 1, 17, _M_DATA) ;
    send_band(fd1, 1, 18, _M_DATA) ;
    send_band(fd1, 0, 19, _M_PCPROTO) ;
    send_band(fd1, 2, 20, _M_DATA) ;

    rslt = nread_wait_msgs(fd2, 4) ;
    if (rslt != 4)
    {
	print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 4) ;
	xit() ;
    }

    bi.bi_pri = 1 ;			/* flush band 1 */
    bi.bi_flag = FLUSHRW ;		/* flush both read/write */

    rslt = user_ioctl(fd2, I_FLUSHBAND, &bi) ;
    if (rslt < 0)
    {
	print("loop.2: I_FLUSH: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    receive_band(fd2, 0, 19, _M_PCPROTO) ;
    receive_band(fd2, 2, 20, _M_DATA) ;
    rslt = flush_wait(fd2) ;
    if (rslt != 0)
    {
	print("loop.2: I_NREAD returned %d, expected %d\n", rslt, 0) ;
	xit() ;
    }

    print("flush_test: band 0 data + M_PCPROTO via I_FLUSHBAND -- OK\n") ;


    user_close(fd1);
    user_close(fd2);

    print("End of queue flushing test\n") ;
}

/************************************************************************
*                             pullupmsg_test                            *
*************************************************************************
*									*
* This test just opens a special device file to a streams driver that	*
* runs the test from kernel mode.					*
*									*
************************************************************************/
void pullupmsg_test(void)
{
    int		rslt ;

    print("Begin pullupmsg test\n") ;
    rslt = user_open(PUTST, O_RDWR, 0) ;
    if (rslt < 0)
    {
	print("putst: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    user_close(rslt) ;
    print("pullupmsg test OK\n") ;
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

    while ((st = mt_get_state()) != statenr && st >= 0)
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
    int		thrno = (int) arg ;
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

	    if (mt_ioctl(ctl_fd, MTDRV_SET_OPEN_SLEEP, 200) < 0)
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
	     * This is the third open of dev 3.
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
	    if (fdc < 0)
	    {
		perror("mtdrv_clone") ;
		mt_set_state(-1) ;
		break ;
	    }

	    print("%s: Thread%d: mtdrv_clone opened fd=%d\n", now(), thrno, fdc) ;
	    sleep(1) ;
	    print("%s: Thread%d: close fd=%d\n", now(), thrno, fd) ;
	    close(fd) ;
	    fd = -1 ;
	    print("%s: Thread%d: close fd=%d\n", now(), thrno, fdc) ;
	    close(fdc) ;
	    fdc = -1 ;
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

	    if (mt_ioctl(ctl_fd, MTDRV_SET_OPEN_SLEEP, 200) < 0)
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
	     * Dev 3 is half-open at the moment by thread 1.
	     */
	    if (prev_state == 5)
	    {
		sleep(1) ;
		break ;
	    }


	    if (thrno == 1)
	    {
		mt_await_state(-1) ;
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
	    if (fdc < 0)
	    {
		perror("mtdrv_clone") ;
		mt_set_state(-1) ;
		break ;
	    }

	    print("%s: Thread%d: mtdrv_clone opened fd=%d\n", now(), thrno, fdc) ;
	    print("%s: Thread%d: close fd=%d\n", now(), thrno, fdc) ;
	    close(fdc) ;
	    fdc = -1 ;
	    sleep(1) ;			/* give thread 1 some time */
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


void mt_open_test(void)
{
    int		rslt ;
    pthread_t	thr1 ;
    pthread_t	thr2 ;
    pthread_mutexattr_t	 attr ;

    print("Begin multi-thread open test\n") ;
    system("rmmod streams-mtdrv") ;
    if (system("modprobe streams-mtdrv") != 0)
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
	system("rmmod streams-mtdrv") ;
	xit() ;
    }

    rslt = pthread_create(&thr2, NULL, mt_thread, (void *) 2) ;
    if (rslt < 0)
    {
	fprintf(stderr, "Thread #%d: ", 2) ;
	perror("pthread_create") ;
	system("rmmod streams-mtdrv") ;
	xit() ;
    }

    sleep(1) ;
    mt_set_state(1) ;
    pthread_mutex_lock(&mt_quit) ;		/* waits until thread exit */
    print("multi-thread open test OK\n") ;
    system("rmmod streams-mtdrv") ;
}
#endif

/************************************************************************
*                              main                                     *
************************************************************************/
void test(void)
{
    extern long		lis_mem_alloced ;		/* from port.c */

    print("\nBegin STREAMS test\n\n") ;

    set_debug_mask(ALL_DEBUG_BITS) ;

#ifdef DIRECT_USER
    print("\n\n\nDirectory listing:\n\n") ;
    user_print_dir(NULL, USR_PRNT_INODE) ;
#endif
    print("Memory allocated = %ld\n", lis_mem_alloced) ;

    open_close_test() ;
    print_mem() ;
    print("Memory allocated = %ld\n", lis_mem_alloced) ;
    wait_for_logfile("open/close test") ;

#ifndef DIRECT_USER
    mt_open_test();
    print_mem() ;
    print("Memory allocated = %ld\n", lis_mem_alloced) ;
    wait_for_logfile("mt open test") ;
#endif

    ioctl_test() ;
    print_mem() ;
    print("Memory allocated = %ld\n", lis_mem_alloced) ;
    wait_for_logfile("ioctl test") ;

    rdopt_test() ;
    print_mem() ;
    print("Memory allocated = %ld\n", lis_mem_alloced) ;
    wait_for_logfile("read options test") ;

    write_test() ;
    print_mem() ;
    print("Memory allocated = %ld\n", lis_mem_alloced) ;
    wait_for_logfile("write test") ;

    close_timer_test() ;
    print_mem() ;
    print("Memory allocated = %ld\n", lis_mem_alloced) ;
    wait_for_logfile("close timer test") ;

    putmsg_test() ;
    print_mem() ;
    print("Memory allocated = %ld\n", lis_mem_alloced) ;
    wait_for_logfile("putmsg test") ;

    poll_test() ;
    print_mem() ;
    print("Memory allocated = %ld\n", lis_mem_alloced) ;
    wait_for_logfile("poll test") ;

    mux_test() ;
    print_mem() ;
    print("Memory allocated = %ld\n", lis_mem_alloced) ;
    wait_for_logfile("multiplexor test") ;

    clone_test() ;
    print("Memory allocated = %ld\n", lis_mem_alloced) ;
    wait_for_logfile("clone driver test") ;

    bufcall_test() ;
    print("Memory allocated = %ld\n", lis_mem_alloced) ;
    wait_for_logfile("bufcall test") ;

    sad_test() ;
    print("Memory allocated = %ld\n", lis_mem_alloced) ;
    wait_for_logfile("sad driver test") ;

    fifo_test() ;
    print("Memory allocated = %ld\n", lis_mem_alloced) ;
    wait_for_logfile("FIFO/pipe test") ;

    passfd_test();
    print("Memory allocated = %ld\n", lis_mem_alloced);
    wait_for_logfile("FD passing test");

    band_test() ;
    print("Memory allocated = %ld\n", lis_mem_alloced);
    wait_for_logfile("Q-band test");

    flush_test() ;
    print("Memory allocated = %ld\n", lis_mem_alloced);
    wait_for_logfile("Flushing test");

#ifdef DIRECT_USER
    pullupmsg_test() ;
    print_mem() ;		/* looking for leaked mblks, etc */
    wait_for_logfile("pullupmsg test");
    print("\n\n\nDirectory listing:\n\n") ;
    user_print_dir(NULL, USR_PRNT_INODE) ;
#endif

    set_debug_mask(0L) ;
    print("\n\n*** strtst completed successfully ***\n") ;
}

/************************************************************************
*                         tst_print_trace                               *
*************************************************************************
*									*
* This routine is pointed to by the 'lis_print_trace' pointer in the	*
* cmn_err module when the test program is linked directly with STREAMS.	*
*									*
************************************************************************/
void	tst_print_trace (char *bfrp)
{
    print("%s", bfrp) ;

} /* tst_print_trace  */


/************************************************************************
*                           main                                        *
************************************************************************/
int main(void)
{
    extern long		lis_mem_alloced ;		/* from port.c */

#ifdef DIRECT_USER
    lis_print_trace = tst_print_trace ;
#endif

    register_drivers() ;
    print("Memory allocated = %ld\n", lis_mem_alloced) ;

#ifndef LINUX
    make_nodes() ;
    print("Memory allocated = %ld\n", lis_mem_alloced) ;
#endif

#ifndef DIRECT_USER
    printk_fd = user_open(NPRINTK, O_RDWR, 0) ;
    if (printk_fd < 0)
    {
	printf( NPRINTK ": %s\n", strerror(-printk_fd)) ;
	xit() ;
    }
#endif

    test() ;
/*     test() ; */

    return 0;
}
