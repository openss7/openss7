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

#ident "@(#) LiS timetst.c 2.5 09/02/04 14:46:08 "

#define	inline				/* make disappear */

#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>			/* for strerror */
#ifdef LINUX
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
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

#define	ALL_DEBUG_BITS	( ~ ((unsigned long long) 0) )

#ifdef DIRECT_USER		/* tie-in to cmn_err */
#define	ENO(neg_e)	(-(neg_e))
#else
#define	ENO(dmy)	(errno)
#endif

/************************************************************************
*                           Storage                                     *
************************************************************************/

unsigned long long	debug_mask ;
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
*                        Histogram Maintenance                          *
************************************************************************/

int	latency_opt = 0;		/* option to measure latency */

typedef struct
{
    int		micro_secs ;
    unsigned	counter ;

} histogram_bucket_t ;

#define	HISTOGRAM_INIT	{ \
			    {1, 0}, \
			    {2, 0}, \
			    {3, 0}, \
			    {4, 0}, \
			    {5, 0}, \
			    {6, 0}, \
			    {7, 0}, \
			    {8, 0}, \
			    {9, 0}, \
			    {10, 0}, \
			    {20, 0}, \
			    {30, 0}, \
			    {40, 0}, \
			    {50, 0}, \
			    {60, 0}, \
			    {70, 0}, \
			    {80, 0}, \
			    {90, 0}, \
			    {100, 0}, \
			    {200, 0}, \
			    {300, 0}, \
			    {400, 0}, \
			    {500, 0}, \
			    {600, 0}, \
			    {700, 0}, \
			    {800, 0}, \
			    {900, 0}, \
			    {1000, 0}, \
			    {2000, 0}, \
			    {3000, 0}, \
			    {4000, 0}, \
			    {5000, 0}, \
			    {6000, 0}, \
			    {7000, 0}, \
			    {8000, 0}, \
			    {9000, 0}, \
			    {10000, 0}, \
			    {20000, 0}, \
			    {30000, 0}, \
			    {40000, 0}, \
			    {50000, 0}, \
			    {60000, 0}, \
			    {70000, 0}, \
			    {80000, 0}, \
			    {90000, 0}, \
			    {100000, 0}, \
			    {200000, 0}, \
			    {300000, 0}, \
			    {400000, 0}, \
			    {500000, 0}, \
			    {600000, 0}, \
			    {700000, 0}, \
			    {800000, 0}, \
			    {900000, 0}, \
			    {1000000, 0}, \
			    {2000000, 0}, \
			    {3000000, 0}, \
			    {4000000, 0}, \
			    {5000000, 0}, \
			    {6000000, 0}, \
			    {7000000, 0}, \
			    {8000000, 0}, \
			    {9000000, 0}, \
			    {0, 0} \
			}

histogram_bucket_t	hist_init[] = HISTOGRAM_INIT ;
histogram_bucket_t	hist[] = HISTOGRAM_INIT ;

/*
 * Initialize the 'hist' buffer.
 */
void init_hist(histogram_bucket_t *h)
{
    memcpy(h, hist_init, sizeof(hist_init)) ;
}

/*
 * Enter a time interval into a histogram bucket
 */
void enter_hist(histogram_bucket_t *h, int interval)
{
    for ( ; h->micro_secs != 0; h++)
    {
	if (interval <= h->micro_secs)
	    break ;
    }

    h->counter++ ;
}

/*
 * Print out the histogram
 */
void print_hist(histogram_bucket_t *h, int minimum)
{
    histogram_bucket_t	*p = h ;
    int			 max_counter = 0 ;

    for (; p->micro_secs != 0; p++)
    {
	if (p->counter > max_counter)
	    max_counter = p->counter ;
    }

    for (p = h; p->micro_secs != 0; p++)
    {
	if (p->counter >= minimum)
	    printf("%8d %12u\n", p->micro_secs, p->counter) ;
    }

    if (p->counter != 0)
	printf("%8s %12u\n", "Larger", p->counter) ;

}

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
    struct timeval	*tp = NULL ;
    struct timeval	*rtp = NULL ;
    int			xmit_time ;
    int			rcv_time ;
    int			interval ;
    struct timeval	x ;

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

    if (latency_opt)
    {
	tp = (struct timeval *) buf ;
	strcpy(&buf[sizeof(*tp)], "Data to send down the file") ;
	lgth = sizeof(*tp) + strlen(&buf[sizeof(*tp)]) ;
	rtp = (struct timeval *) rdbuf ;
    }
    else
    {
	strcpy(buf, "Data to send down the file") ;
	lgth = strlen(buf) ;
    }


    printf("Time test:  write %d bytes, read/write and service queue: ",
    		lgth) ;
    fflush(stdout) ;

    sync() ;				/* do file sync now rather than
    					 * in the middle of the test.
    					 */
    init_hist(hist) ;
    time_on = time(NULL) ;
#if 1
    for (i = 0; i < iter_cnt; i++)
    {
	if (latency_opt)
	    gettimeofday(tp, NULL) ;

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

	if (latency_opt)
	{
	    gettimeofday(&x, NULL) ;		/* current time */
	    xmit_time = rtp->tv_sec * 1000000 + rtp->tv_usec ;
	    rcv_time  = x.tv_sec * 1000000 + x.tv_usec ;
	    interval  = rcv_time - xmit_time ;
	    enter_hist(hist, interval) ;
	}
    }
#endif
    et = (time(NULL) - time_on) * 1000000 ;	/* time in usecs */

    printf("%ld micro-secs\n", et/iter_cnt) ;
    if (latency_opt)
	print_hist(hist, 1000) ;
    printf("\n") ;



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
    init_hist(hist) ;
    sync() ;
    time_on = time(NULL) ;

    for (i = 0; i < iter_cnt; i++)
    {
	if (latency_opt)
	    gettimeofday(tp, NULL) ;

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

	if (latency_opt)
	{
	    gettimeofday(&x, NULL) ;		/* current time */
	    xmit_time = rtp->tv_sec * 1000000 + rtp->tv_usec ;
	    rcv_time  = x.tv_sec * 1000000 + x.tv_usec ;
	    interval  = rcv_time - xmit_time ;
	    enter_hist(hist, interval) ;
	}
    }

    et = (time(NULL) - time_on) * 1000000 ;	/* time in usecs */

    printf("%ld micro-secs\n", et/iter_cnt) ;
    if (latency_opt)
	print_hist(hist, 1000) ;
    printf("\n") ;



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
    init_hist(hist) ;
    sync() ;
    time_on = time(NULL) ;

    rd_ctl.maxlen	= sizeof(rdctlbuf) ;
    rd_dta.maxlen	= sizeof(rdbuf) ;
    wr_ctl.len	= -1 ;
    wr_dta.len	= lgth ;
    for (i = 0; i < iter_cnt; i++)
    {
	if (latency_opt)
	    gettimeofday(tp, NULL) ;

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

	if (latency_opt)
	{
	    gettimeofday(&x, NULL) ;		/* current time */
	    xmit_time = rtp->tv_sec * 1000000 + rtp->tv_usec ;
	    rcv_time  = x.tv_sec * 1000000 + x.tv_usec ;
	    interval  = rcv_time - xmit_time ;
	    enter_hist(hist, interval) ;
	}
    }

    et = (time(NULL) - time_on) * 1000000 ;	/* time in usecs */

    printf("%ld micro-secs\n", et/iter_cnt) ;
    if (latency_opt)
	print_hist(hist, 1000) ;
    printf("\n") ;


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
void	set_debug_mask(unsigned long long msk)
{
    int			fd ;
    int			rslt ;
    unsigned long	mask1 = (unsigned long)(msk & 0xFFFFFFFF) ;
    unsigned long	mask2 = (msk >> 32) ;

    fd = user_open(LOOP_1, O_RDWR, 0) ;
    if (fd < 0)
    {
	printf("loop.1: %s\n", strerror(ENO(fd))) ;
	exit(1) ;
    }

    rslt = user_ioctl(fd, I_LIS_SDBGMSK, mask1) ;
    if (rslt < 0)
    {
	printf("loop.1: I_LIS_SDBGMSK: %s\n", strerror(ENO(rslt))) ;
	exit(1) ;
    }

    rslt = user_ioctl(fd, I_LIS_SDBGMSK2, mask2) ;
    printf("\nSTREAMS debug mask set to 0x%08lx%08lx\n", mask2, mask1) ;

    user_close(fd) ;

} /* set_debug_mask */

/************************************************************************
*                             print_options                             *
************************************************************************/
void print_options(void)
{
    printf("strtst [<options>]\n");
    printf("  -d<mask>     Set debug mask (long long argument)\n");
    printf("  -i<cnt>      Set iteration count, %ld default\n", iter_cnt);
    printf("  -l           Compute round trip message latency histogram\n");
    printf("  -h           Print this message\n");
}

/************************************************************************
*                            get_options                                *
************************************************************************/
void get_options(int argc, char **argv)
{
    int		opt ;

    while ((opt = getopt(argc, argv, "d:i:hl")) > 0)
    {
	switch (opt)
	{
	case 'd':
	    debug_mask = strtoull(optarg, NULL, 0);
	    break ;
	case 'i':
	    iter_cnt = strtol(optarg, NULL, 0) ;
	    break ;
	case 'l':
	    latency_opt = 1 ;
	    break ;
	case 'h':
	    print_options() ;
	    exit(0) ;
	default:
	    print_options() ;
	    exit(1) ;
	}
    }
}


/************************************************************************
*                              main                                     *
************************************************************************/
int main(int argc, char **argv)
{
    get_options(argc, argv) ;

#if	!defined(LINUX) && !defined(QNX)
    register_drivers() ;
    make_nodes() ;
#endif

    printf("Timing test version %s\n\n", "2.5 09/02/04");
    printf("Using safe constructs and message tracing\n") ;
    set_debug_mask(debug_mask | 0x30000L) ;
    timing_test() ;

    printf("\n\nNot using safe constructs or message tracing\n") ;
    set_debug_mask(debug_mask & ~0x30000L) ;
    timing_test() ;

    return 0;
}
