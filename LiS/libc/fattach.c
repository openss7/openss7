#ident "@(#) LiS fattach.c 2.4 1/23/02"

#include <stropts.h>		/* includes unistd.h */
#include <errno.h>
#include <sys/ioctl.h>

#ifdef __NR_fattach

#define	__NR_sys_fattach	__NR_fattach

static inline _syscall2(int,sys_fattach,int,fd,const char *,path);

int fattach( int fd, const char *path )
{
    return (sys_fattach( fd, path ));
}

#else  /*  no __NR_fattach */

int fattach( int fd, const char *path )
{
    return (ioctl( fd, I_LIS_FATTACH, path ));
}

#endif  /* __NR_fattach */
