/************************************************************************
*                     Multi-Threaded Test                               *
*************************************************************************
*									*
* This program runs data through the STREAMS loopback driver using	*
* multiple threads.  The threads act in pairs with one thread of each	*
* pair generating data and the other thread reading data.  The "reading"*
* thread echos the data back so that the generating thread can read	*
* back what it originally wrote.					*
*									*
* The main program monitors the progress and maintains a summary screen.*
*									*
************************************************************************/

/*
 * Copyright (C)  2001  David Grothe, Gcom, Inc <dave@gcom.com>
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

#ident "@(#) LiS thrtst.c 1.2 11/5/02 16:58:18 "

#define _REENTRANT
#define _THREAD_SAFE
#define _XOPEN_SOURCE	500		/* single unix spec */

#include <stropts.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/LiS/loop.h>		/* an odd place for this file */


/************************************************************************
*                          Constants                                    *
************************************************************************/
#define MAX_THR		16		/* less than # of loop-clones */
#define	MAX_MSG		1024

typedef struct thread_info
{
    int		 thread_number ;
    pthread_mutex_t	startup ;	/* so everyone can start at once */

    int		 xmcnt ;		/* transmit msg count */
    int		 xbcnt ;		/* transmit byte count */
    int		 rmcnt ;		/* receive msg count */
    int		 rbcnt ;		/* receive byte count */

    int		 fd ;			/* file descr to loop driver */
    int		 minor ;		/* minor device number of loop */
    int		 looped ;		/* have been looped to partner */
    int		 burst ;		/* initial burst size for writer */

    char	*xdbuf ;		/* xmit data buffer */
    char	*xcbuf ;		/* xmit ctl buffer */
    char	*rdbuf ;		/* receive data buffer */
    char	*rcbuf ;		/* receive ctl buffer */
    struct strbuf xd ;
    struct strbuf xc ;
    struct strbuf rd ;
    struct strbuf rc ;

    struct thread_info *other ;		/* pointer to partner's struct */

} thread_info_t ;

/************************************************************************
*                       Global Storage                                  *
************************************************************************/
pthread_t	thread_ids[MAX_THR+1] ;		/* numbered from 1 */
thread_info_t	thread_info[MAX_THR+1] ;	/* numbered from 1 */
time_t		start_time ;
int		max_threads = MAX_THR ;
int		time_factor = 1 ;
int		initial_burst = 0 ;

/************************************************************************
*                           setup_thread_info				*
*************************************************************************
*									*
* Initialize a thread info structure.					*
*									*
************************************************************************/
void	setup_thread_info(int thread_nr)
{
    thread_info_t	*tp = &thread_info[thread_nr];
    int			 i ;
    char		*p ;
    int			 arg ;
    int			 rslt ;
    int			 other_inx ;	/* thr nr of my partner */
    struct strioctl	 ioc ;
    pthread_mutexattr_t	 attr ;

    memset(tp, 0, sizeof(*tp)) ;
    tp->thread_number = thread_nr ;
    tp->burst = initial_burst ;

    tp->fd = open("/dev/loop_clone", O_RDWR, 0) ;
    if (tp->fd < 0)
    {
	perror("/dev/loop_clone") ;
	exit(1) ;
    }

    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;

    ioc.ic_cmd 	  = LOOP_GET_DEV ;
    arg = -1 ;
    rslt = ioctl(tp->fd, I_STR, &ioc) ;
    if (rslt < 0)
    {
	perror("loop_clone: ioctl LOOP_GET_DEV") ;
	exit(1) ;
    }

    if (arg < 0)
    {
	fprintf(stderr, "loop_clone: ioctl LOOP_GET_DEV returned %d\n", arg) ;
	exit(1) ;
    }

    tp->minor = arg ;			/* minor number of this loop clone */
    other_inx = ((tp->thread_number-1) ^ 1) + 1 ;
    tp->other = &thread_info[other_inx] ;

    tp->xdbuf = malloc(MAX_MSG) ;
    tp->xcbuf = malloc(MAX_MSG) ;
    tp->rdbuf = malloc(MAX_MSG) ;
    tp->rcbuf = malloc(MAX_MSG) ;
    if (   tp->xdbuf == NULL || tp->xcbuf == NULL
	|| tp->rdbuf == NULL || tp->rcbuf == NULL)
    {
	fprintf(stderr, "setup_thread_info: cannot allocate memory\n");
	exit(1);
    }

    tp->xd.buf = tp->xdbuf ;
    tp->xc.buf = tp->xcbuf ;
    tp->rd.buf = tp->rdbuf ;
    tp->rc.buf = tp->rcbuf ;
    tp->rd.maxlen = MAX_MSG ;
    tp->rc.maxlen = MAX_MSG ;

    /*
     * Place a counting pattern in the xmit buffers
     */
    for (p = tp->xdbuf, i = 0; i < MAX_MSG; i++, p++) *p = (char) i ;
    for (p = tp->xcbuf, i = 0; i < MAX_MSG; i++, p++) *p = (char) i ;

    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_DEFAULT) ;
    pthread_mutex_init(&tp->startup, &attr) ;
    pthread_mutex_lock(&tp->startup) ;

}

/************************************************************************
*                         set_loop                                      *
*************************************************************************
*									*
* Set the loopback file to point to its partner.			*
*									*
************************************************************************/
int	set_loop(int fd, int other_minor)
{
    int			 rslt ;
    struct strioctl	 ioc ;

    /* printf("set_loop(%d, %d)\n", fd, other_minor) ; */
    ioc.ic_timout = 10 ;
    ioc.ic_cmd 	  = LOOP_SET ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &other_minor ;
    rslt = ioctl(fd, I_STR, &ioc) ;
    if (rslt < 0)
	perror("set_loop: ioctl LOOP_SET") ;

    return(rslt) ;
}

/************************************************************************
*                           set_timer                                   *
*************************************************************************
*									*
* Set the timer for delay of messages in loopback driver.  Once set	*
* this value is used until the file is closed.				*
*									*
************************************************************************/
int	set_timer(int fd, int timeout)
{
    int			 rslt ;
    struct strioctl	 ioc ;

    timeout = time_factor * timeout ;
    ioc.ic_cmd 	  = LOOP_TIMR ;		/* set timer for queue */
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &timeout ;
    rslt = ioctl(fd, I_STR, &ioc) ;
    if (rslt < 0)
	perror("set_timer: ioctl LOOP_TIMR") ;

    return(rslt) ;
}

/************************************************************************
*                           reader                                      *
*************************************************************************
*									*
* The reader process.  It reads messages and writes them back.		*
*									*
************************************************************************/
void	*reader(void *arg)
{
    thread_info_t	*tp = (thread_info_t *) arg ;
    int			 nbytes ;

    /* printf("Reader[%d]: Waiting for startup\n", tp->thread_number) ; */
    pthread_mutex_lock(&tp->startup) ;
    printf("Reader[%d]: Startup\n", tp->thread_number) ;

    for (;;)
    {
	if ((nbytes = read(tp->fd, tp->rdbuf, MAX_MSG)) < 0)
	    break ;

	tp->rmcnt++ ;
	tp->rbcnt += nbytes ;

	if (write(tp->fd, tp->xdbuf, nbytes) < 0)
	    break ;

	tp->xmcnt++ ;
	tp->xbcnt += nbytes ;
    }

    printf("Reader[%d]: Exiting\n", tp->thread_number) ;
    return(NULL) ;
}

/************************************************************************
*                           writer                                      *
*************************************************************************
*									*
* The writer process.  It writes messages and then reads them back.	*
*									*
************************************************************************/
void	*writer(void *arg)
{
    thread_info_t	*tp = (thread_info_t *) arg ;
    int			 msg_size = MAX_MSG ;
    int			 i ;
    int			 nbytes ;

    /* printf("Writer[%d]: Waiting for startup\n", tp->thread_number) ; */
    pthread_mutex_lock(&tp->startup) ;
    printf("Writer[%d]: Startup\n", tp->thread_number) ;

    for (i = 0; i < tp->burst; i++)
    {
	if (write(tp->fd, tp->xdbuf, msg_size) < 0)
	    break ;

	tp->xmcnt++ ;
	tp->xbcnt += msg_size ;
    }

    for (;;)
    {
	if (write(tp->fd, tp->xdbuf, msg_size) < 0)
	    break ;

	tp->xmcnt++ ;
	tp->xbcnt += msg_size ;

	if ((nbytes = read(tp->fd, tp->rdbuf, MAX_MSG)) < 0)
	    break ;

	tp->rmcnt++ ;
	tp->rbcnt += nbytes ;
    }

    printf("Writer[%d]: Exiting\n", tp->thread_number) ;
    return(NULL) ;
}

/************************************************************************
*                            print_status                               *
*************************************************************************
*									*
* Print a status report on the progress of the threads.			*
*									*
************************************************************************/
void	print_progress(void)
{
    int			 i ;
    time_t		 now ;
    long		 delta ;
    thread_info_t	*tp ;

    printf("Thr   Xmit-Cnt Xmit/Sec    Rcv-Cnt  Rcv/Sec\n") ;
    now = time(NULL) ;
    delta = now - start_time ;
    if (delta == 0) delta = 1 ;
    for (i = 1; i <= max_threads; i++)
    {
	tp = &thread_info[i] ;
	printf("%3u %10u %8u %10u %8u\n",
		i,
		tp->xmcnt, (int)(tp->xmcnt/delta),
		tp->rmcnt, (int)(tp->rmcnt/delta)) ;
    }
}

/************************************************************************
*                           print_usage                                 *
************************************************************************/
void	print_usage(void)
{
    printf("thrtst [<options>]\n") ;
    printf("   -b<n>       Set initial burst for writer thread\n") ;
    printf("   -f<n>       Set time delay factor to 'n'\n") ;
    printf("   -t<n>       Set number of threads to 'n'\n") ;
}

/************************************************************************
*                         process_options                               *
************************************************************************/
void	process_options(int argc, char **argv)
{
    char  *p ;

    for (argc--, argv++; argc > 0; argc--, argv++)
    {
	p = *argv;				/* set option ptr */

	if (*p++ != '-')			/* skip over '-' */
	{
	    fprintf (stderr, "Options must being with a '-'\n");
	    print_usage() ;
	    exit(1) ;
	}

	switch (*p++)				/* point at option arg*/
	{
	case 'b':
	    initial_burst = strtol(p, NULL, 0) ;
	    break ;
	case 'f':
	    time_factor = strtol(p, NULL, 0) ;
	    break ;

	case 't':
	    max_threads = strtol(p, NULL, 0) ;
	    if (max_threads < 0)
		max_threads = 2 ;
	    else
	    if (max_threads & 0x01)
		max_threads++ ;

	    if (max_threads > MAX_THR)
		max_threads = MAX_THR ;
	    break ;

	default:
	    printf ("thrtst: option -%s not recognized\n\n", --p);
	    /* fall thru */

	case 'h':
	    print_usage() ;
	    exit(1) ;
	}
    }
}

/************************************************************************
*                            main                                       *
*************************************************************************
*									*
* The main program sets up all the structurs and file descriptors and	*
* then lets the threads just move the data.				*
*									*
************************************************************************/
int	main(int argc, char **argv)
{
    int			 rslt ;
    int			 i ;
    void		* (*funcp)(void *) ;
    thread_info_t	*tp ;

    process_options(argc, argv) ;

    for (i = 1; i <= max_threads; i++)
    {
	if (i & 0x01)
	    funcp = reader ;
	else
	    funcp = writer ;

	setup_thread_info(i) ;
	tp = &thread_info[i] ;
	rslt = pthread_create(&thread_ids[i],
			      NULL,
			      funcp, tp) ;
	if (rslt < 0)
	{
	    fprintf(stderr, "Thread #%d: ", i) ;
	    perror("pthread_create") ;
	}
#if 0
	else
	    printf("main: Thread #%d id=%ld\n", i, thread_ids[i]) ;
#endif
    }

    for (i = 1; i <= max_threads; i++)
    {
	tp = &thread_info[i] ;
	if (!tp->looped)
	{
	    if (set_loop(tp->fd, tp->other->minor) < 0)
		exit(1) ;
	    tp->looped = 1 ;
	    tp->other->looped = 1 ;
	}
	if (set_timer(tp->fd, tp->thread_number) < 0)
	    exit(1) ;
    }

    start_time = time(NULL) ;
    for (i = 1; i <= max_threads; i++)
    {
	tp = &thread_info[i] ;
	pthread_mutex_unlock(&tp->startup) ;
    }

    for (;;)
    {
	/* printf("main: sleeping for 5 secs\n") ; */
	sleep(5) ;
	print_progress() ;
    }

    printf("main: exiting\n") ;

    return(0) ;
}
