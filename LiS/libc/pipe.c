#ident "@(#) LiS pipe.c 1.3 10/23/00"

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stropts.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#ifdef __NR_spipe

#define	__NR_sys_spipe	__NR_spipe

static inline _syscall1(int,sys_spipe,int *,fd);

int pipe( int *fd )
{
    return (sys_spipe(fd));
}

#else  /* no __NR_spipe */

#define DUMMY_STREAM "/dev/fifo.0"    /* FIXME: /dev/stream,... */
#define DUMMY_MODE   O_RDWR|O_NONBLOCK

int pipe( int *fd )
{
    int ffd, error = 0;

    error = 0;

    if ((ffd = open( DUMMY_STREAM, DUMMY_MODE )) < 0)  return -1;

    if (ioctl( ffd, I_LIS_PIPE, fd ) < 0)  error = errno;

    close(ffd);

    if (error) {
	errno = error;  return -1;
    }
    return 0;
}

#endif  /* __NR_spipe */
