/*
 * Copyright 1997 David Grothe, Gcom, Inc <dave@gcom.com>
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

#ident "@(#) LiS timetst.c 2.3 12/15/02 18:00:05 "

#define	inline				/* make disappear */

#include <unistd.h>
#include <time.h>
#include <string.h>			/* for strerror */
#ifdef LINUX
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#endif
#ifdef SYS_QNX
#include <ioctl.h>
#endif

#include <sys/stream.h>
#include <sys/stropts.h>

#if	defined(LINUX)
#include <sys/LiS/linuxio.h>
#elif	defined(SYS_QNX)
#include <sys/LiS/qnxio.h>
#else
#include <sys/LiS/usrio.h>
#endif

#include <sys/LiS/loop.h>		/* an odd place for this file */

/************************************************************************
*                      File Names                                       *
************************************************************************/
#if	defined(LINUX)
#define	LOOP_1		"/dev/loop.1"
#define	LOOP_2		"/dev/loop.2"
#define LOOP_CLONE	"/dev/loop_clone"
#elif	defined(QNX)
#define	LOOP_1		"/dev/gcom/loop.1"
#define	LOOP_2		"/dev/gcom/loop.2"
#define LOOP_CLONE	"/dev/gcom/loop_clone"
#else
#define	LOOP_1		"loop.1"
#define	LOOP_2		"loop.2"
#define LOOP_CLONE	"loop_clone"
#endif


/************************************************************************
*                           Storage                                     *
************************************************************************/

char		buf[1000] ;		/* general purpose */
char		rdbuf[1000] ;		/* for reading */
long		iter_cnt = 100000 ;	/* default iteration count */
char		ctlbuf[1000] ;		/* control messages */
char		rdctlbuf[1000] ;	/* control messages */

struct strbuf	wr_ctl = {0, 0, ctlbuf} ;
struct strbuf	wr_dta = {0, 0, buf} ;
struct strbuf	rd_ctl = {0, 0, rdctlbuf} ;
struct strbuf	rd_dta = {0, 0, rdbuf} ;

extern void make_nodes(void) ;

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
#if	!defined(LINUX) && !defined(QNX)
    port_init() ;			/* stream head init routine */
#endif

} /* register_drivers */


/************************************************************************
*                           timing_test                                 *
*************************************************************************
*									*
* Time reading and writing messages through streams.			*
*									*
************************************************************************/
void	timing_test(void)
{
    time_t		time_on ;
    time_t		et ;
    long		i ;
    int			fd1 ;
    int			fd2 ;
    int			rslt ;
    int			lgth ;
    int			arg ;
    int			flags = MSG_ANY;
    int			rband = 0 ;
    struct strioctl	ioc ;

    printf("\nBegin timing test\n") ;

    fd1 = user_open(LOOP_1, O_RDWR, 0) ;
    if (fd1 < 0)
    {
	printf("loop.1: %s\n", strerror(-fd1)) ;
	return ;
    }

    fd2 = user_open(LOOP_2, O_RDWR, 0) ;
    if (fd2 < 0)
    {
	printf("loop.2: %s\n", strerror(-fd2)) ;
	return ;
    }

    ioc.ic_cmd 	  = LOOP_SET ;
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;

    arg = 2 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	printf("loop.1: ioctl LOOP_SET: %s\n", strerror(-rslt)) ;
    }

    strcpy(buf, "Data to send down the file") ;
    lgth = strlen(buf) ;

    printf("Time test:  write %d bytes, read/write and service queue: ",
    		lgth) ;
    fflush(stdout) ;

    sync() ;				/* do file sync now rather than
    					 * in the middle of the test.
    					 */
    time_on = time(NULL) ;
#if 1
    for (i = 0; i < iter_cnt; i++)
    {
	rslt = user_write(fd1, buf, lgth) ;

	if (rslt != lgth)
	{
	    if (rslt < 0)
		printf("loop.1: write: %s\n", strerror(-rslt)) ;
	    else
		printf("loop.1:  write returned %d, expected %d\n", rslt, lgth) ;

	    break ;
	}

	rslt = user_read(fd2, rdbuf, lgth);

	if (rslt != lgth)
	{
	    if (rslt < 0)
		printf("loop.2: read: %s\n", strerror(-rslt)) ;
	    else
		printf("loop.2:  read returned %d, expected %d\n", rslt, lgth) ;
	    break ;
	}
    }
#endif
    et = (time(NULL) - time_on) * 1000000 ;	/* time in usecs */

    printf("%ld micro-secs\n", et/iter_cnt) ;



    ioc.ic_cmd 	  = LOOP_PUTNXT ;		/* use putnxt rather then svcq */
    ioc.ic_len	  = 0 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	printf("loop.1: ioctl LOOP_PUTNXT: %s\n", strerror(-rslt)) ;
    }

    printf("Time test:  write %d bytes, read/write w/o service queue: ",
    		lgth) ;
    fflush(stdout) ;
    sync() ;
    time_on = time(NULL) ;

    for (i = 0; i < iter_cnt; i++)
    {
	rslt = user_write(fd1, buf, lgth) ;

	if (rslt != lgth)
	{
	    if (rslt < 0)
		printf("loop.1: write: %s\n", strerror(-rslt)) ;
	    else
		printf("loop.1:  write returned %d, expected %d\n", rslt, lgth) ;

	    break ;
	}

	rslt = user_read(fd2, rdbuf, lgth);

	if (rslt != lgth)
	{
	    if (rslt < 0)
		printf("loop.2: read: %s\n", strerror(-rslt)) ;
	    else
		printf("loop.2:  read returned %d, expected %d\n", rslt, lgth) ;
	    break ;
	}
    }

    et = (time(NULL) - time_on) * 1000000 ;	/* time in usecs */

    printf("%ld micro-secs\n", et/iter_cnt) ;



    ioc.ic_cmd 	  = LOOP_PUTNXT ;	/* use putnxt rather then svcq */
    ioc.ic_len	  = 0 ;
    rslt = user_ioctl(fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	printf("loop.1: ioctl LOOP_PUTNXT: %s\n", strerror(-rslt)) ;
    }

    printf("Time test:  write %d bytes, getmsg/putmsg w/o service queue: ",
    		lgth) ;
    fflush(stdout) ;
    sync() ;
    time_on = time(NULL) ;

    rd_ctl.maxlen	= sizeof(rdctlbuf) ;
    rd_dta.maxlen	= sizeof(rdbuf) ;
    wr_ctl.len	= -1 ;
    wr_dta.len	= lgth ;
    for (i = 0; i < iter_cnt; i++)
    {
	rslt = user_putpmsg(fd1, &wr_ctl, &wr_dta, 0, MSG_BAND) ;

	if (rslt < 0)
	{
	    printf("loop.1: putmsg: %s\n", strerror(-rslt)) ;
	    break ;
	}

	rd_ctl.len	= -1 ;
	rd_dta.len	= -1 ;
	flags		= MSG_ANY ;

	rslt = user_getpmsg(fd2, &rd_ctl, &rd_dta, &rband, &flags) ;

	if (rslt < 0)
	{
	    printf("loop.2: getmsg: %s\n", strerror(-rslt)) ;
	    break ;
	}
	else
	if (rd_dta.len != lgth || rd_ctl.len > 0)
	{
	    printf("expected rd_ctl.len = %d, got %d\n", -1, rd_ctl.len) ;
	    printf("expected rd_dta.len = %d, got %d\n", lgth, rd_dta.len) ;
	}
    }

    et = (time(NULL) - time_on) * 1000000 ;	/* time in usecs */

    printf("%ld micro-secs\n", et/iter_cnt) ;


    user_close(fd1) ;
    user_close(fd2) ;

} /* timing_test */

/************************************************************************
*                          set_debug_mask                               *
*************************************************************************
*									*
* Use stream ioctl to set the debug mask for streams.			*
*									*
************************************************************************/
void	set_debug_mask(long msk)
{
    int		fd ;
    int		rslt ;

    fd = user_open(LOOP_1, 0, 0) ;
    if (fd < 0)
    {
	printf("loop.1: %s\n", strerror(-fd)) ;
	return ;
    }

    rslt = user_ioctl(fd, I_LIS_SDBGMSK, msk) ;
    if (rslt < 0)
    {
	printf("loop.1: I_LIS_SDBGMSK: %s\n", strerror(-rslt)) ;
	return ;
    }

    printf("\nSTREAMS debug mask set to 0x%08lx\n", msk) ;

    user_close(fd) ;

} /* set_debug_mask */

/************************************************************************
*                              main                                     *
************************************************************************/
int main(int argc, char **argv)
{
    if (argc > 1)
	sscanf(argv[1], "%ld", &iter_cnt) ;

#if	!defined(LINUX) && !defined(QNX)
    register_drivers() ;
    make_nodes() ;
#endif

    printf("Timing test version %s\n\n", "2.3 12/15/02");
    printf("Using safe constructs and message tracing\n") ;
    set_debug_mask(0x30000L) ;
    /* set_debug_mask(0x0FFFFFFF) ; */
    timing_test() ;

    printf("\n\nNot using safe constructs or message tracing\n") ;
    set_debug_mask(0L) ;
    timing_test() ;

    return 0;
}
