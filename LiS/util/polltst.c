/************************************************************************
*                            Poll Test                                  *
*************************************************************************
*									*
* This simple program tests the ability to use poll() on both STREAMS	*
* and non-STREAMS file descriptors.					*
*									*
* The idea is to use the loopback driver and the keyboard such that	*
* a keystroke goes from the keyboard to the loopback driver, then	*
* back to the application and then out to the screen.			*
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

#ident "@(#) LiS polltst.c 2.2 11/5/02 19:16:53 "

#include <sys/types.h>
#include <sys/poll.h>
#include <sys/stropts.h>
#include <sys/ioctl.h>
#include <sys/LiS/loop.h>		/* an odd place for this file */
#include <signal.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

/************************************************************************
*                           Defines                                     *
************************************************************************/

#define	LOOP_1		"/dev/loop.1"
#define	LOOP_2		"/dev/loop.2"
#define LOOP_CLONE	"/dev/loop_clone"
#define MUX_CLONE	"/dev/mux_clone"
#define	NPRINTK		"/dev/printk"

#define	print		printf
#define	STDIN		0
#define	STDOUT		1


/************************************************************************
*                        Storage Declarations                           *
************************************************************************/
struct pollfd		fds[4] ;
int			loop1 ;		/* loop driver stream */
int			loop2 ;		/* loop driver stream */
struct termios		old_tty ;
struct termios		new_tty ;

/************************************************************************
*                            poll_events                                *
*************************************************************************
*									*
* Convert a poll mask to ASCII.						*
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

    *fd1 = open(LOOP_1, O_RDWR, 0) ;
    if (*fd1 < 0)
    {
	print("loop.1: %s\n", strerror(-*fd1)) ;
	return(*fd1) ;
    }

    *fd2 = open(LOOP_2, O_RDWR, 0) ;
    if (*fd2 < 0)
    {
	print("loop.2: %s\n", strerror(-*fd2)) ;
	close(*fd1) ;
	return(*fd2) ;
    }

    ioc.ic_cmd 	  = LOOP_SET ;
    ioc.ic_timout = 10 ;
    ioc.ic_len	  = sizeof(int) ;
    ioc.ic_dp	  = (char *) &arg ;

    arg = 2 ;
    rslt = ioctl(*fd1, I_STR, &ioc) ;
    if (rslt < 0)
    {
	print("loop.1: ioctl LOOP_SET: %s\n", strerror(-rslt)) ;
	return(rslt) ;
    }

    return(1) ;

} /* open_files */

/************************************************************************
*                           term_sig                                    *
*************************************************************************
*									*
* Catch the TERM signal.  Put the tty back and exit.			*
*									*
************************************************************************/
void term_sig(int signo)
{
    tcsetattr(STDIN, 0, &old_tty) ;
    printf("\nTerminated\n") ;
    exit(0) ;

} /* term_sig */

/************************************************************************
*                           do_poll                                     *
*************************************************************************
*									*
* Poll between STDIN and the loop streams.				*
*									*
************************************************************************/
void do_poll(void)
{
    int		rslt ;
    char	c ;
    int		iterations ;
    time_t	time_on ;
    time_t	time_off ;

    fds[0].fd		= loop1 ;
    fds[0].events	= POLLIN ;
    fds[0].revents	= 0 ;		/* returned events */
    fds[1].fd		= loop2 ;
    fds[1].events	= POLLIN ;
    fds[1].revents	= 0 ;		/* returned events */
    fds[2].fd		= STDIN ;
    fds[2].events	= POLLIN ;
    fds[2].revents	= 0 ;		/* returned events */

    rslt = ioctl(loop1, I_SRDOPT, RMSGN) ;
    if (rslt < 0)
    {
	print("loop.1: I_SRDOPT(RMSGN): %s\n", strerror(-rslt)) ;
	return ;
    }

    rslt = ioctl(loop2, I_SRDOPT, RMSGN) ;
    if (rslt < 0)
    {
	print("loop.2: I_SRDOPT(RMSGN): %s\n", strerror(-rslt)) ;
	return ;
    }

    time_on = time(NULL) ;

    for (iterations = 0;;iterations++)
    {
	if (poll(fds, 3, -1) < 0)
	{
	    perror("do_poll: poll():") ;
	    break ;
	}

	if (fds[0].revents & POLLIN)
	{				/* loop.1 has data */
	    rslt = read(loop1, &c, 1) ;
	    if (rslt < 0)
	    {
		perror("loop.1: read:") ;
		break ;
	    }

	    write(loop1, &c, 1) ;	/* write back to loop.1 */
	}

	if (fds[1].revents & POLLIN)
	{				/* loop.2 has data */
	    rslt = read(loop2, &c, 1) ;
	    if (rslt < 0)
	    {
		perror("loop.2: read:") ;
		break ;
	    }

	    write(STDOUT, &c, 1) ;	/* write to tty */
	}

	if (fds[2].revents & POLLIN)
	{				/* stdin has data */
	    rslt = read(STDIN, &c, 1) ;
	    if (rslt < 0)
	    {
		perror("STDIN: read:") ;
		break ;
	    }

	    if (rslt == 0)
		break ;

	    write(loop2, &c, 1) ;	/* write to loop.2 */
	}
    }

    time_off = time(NULL) ;
    printf("\nIteration count = %d", iterations) ;
    if ((time_off -= time_on) != 0)
	printf(" Elapsed time = %ld seconds, %ld polls/sec\n",
		(long)time_off, (long)iterations/time_off) ;
    else
	printf(" Elapsed time = 0 seconds\n") ;

} /* do_poll */

/************************************************************************
*                           main                                        *
************************************************************************/
int main(void)
{
    open_files(&loop1, &loop2) ;

    tcgetattr(STDIN, &old_tty) ;
    new_tty = old_tty ;
    new_tty.c_lflag &= ~(ICANON | ECHO) ;
    tcsetattr(STDIN, 0, &new_tty) ;

    signal(SIGTERM, term_sig) ;
    signal(SIGINT, term_sig) ;
    signal(SIGQUIT, term_sig) ;
    do_poll() ;

    tcsetattr(STDIN, 0, &old_tty) ;

    return 0;
} /* main */
