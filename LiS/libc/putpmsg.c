#if !defined(USE_SYSCALLS)

#include <sys/stropts.h>		/* must be LiS version */
#include <unistd.h>

int	putpmsg(int fd, strbuf_t *ctlptr, strbuf_t *dataptr,
		int band, int flags)
{
    putpmsg_args_t	args ;

    args.fd	= fd ;
    args.ctl	= ctlptr ;
    args.dat	= dataptr ;
    args.band	= band ;
    args.flags	= flags ;

    return(write(fd, &args, LIS_GETMSG_PUTMSG_ULEN)) ;
}

#else

#include <linux/types.h>
#include <errno.h>
#include <asm/unistd.h>

#ident "@(#) LiS putpmsg.c 2.4 3/9/03 22:22:44 "

#ifndef __NR_putpmsg

# ifndef __NR_streams2
# ifndef DEP
# error "unistd.h does not define __NR_putpmsg or __NR_streams2"
# endif
# else		/* Linus defined __NR_streams2 rather than __NR_putpmsg */
# define __NR_putpmsg	__NR_streams2
# endif

#endif

#define	__NR_sys_putpmsg	__NR_putpmsg

static inline _syscall5(int,sys_putpmsg,int,fd,
                        void *,ctlptr,void *,dataptr,int,band,int,flags);


int	putpmsg(int fd, void *ctlptr, void *dataptr, int band, int flags)
{
    return(sys_putpmsg(fd, ctlptr, dataptr, band, flags)) ;
}

#endif
