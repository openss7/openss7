#ident "@(#) LiS fdetach.c 2.4 10/23/00"

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stropts.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#ifdef __NR_fdetach

#define	__NR_sys_fdetach	__NR_fdetach

static inline _syscall1(int,sys_fdetach,const char *,path);

int fdetach( const char *path )
{
    return (sys_fdetach( path ));
}

#else  /* no __NR_fdetach */

#define DUMMY_STREAM "/dev/fifo.0"    /* FIXME: /dev/stream,... */
#define DUMMY_MODE   O_RDWR|O_NONBLOCK

int fdetach( const char *path )
{
    int ffd, error = 0;

    errno = 0;

    if ((ffd = open( DUMMY_STREAM, DUMMY_MODE )) < 0)  return -1;

    if (ioctl( ffd, I_LIS_FDETACH, path ) < 0)  error = errno;

    close(ffd);

    if (error) {
	errno = error;  return -1;
    }
    return 0;
}

#endif  /* __NR_fdetach */
