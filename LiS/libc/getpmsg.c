#if !defined(USE_SYSCALLS)

#include <sys/stropts.h>		/* must be LiS version */
#include <unistd.h>

int	getpmsg(int fd, strbuf_t *ctlptr, strbuf_t *dataptr,
		int *bandp, int *flagsp)
{
    getpmsg_args_t	args ;

    args.fd	= fd ;
    args.ctl	= ctlptr ;
    args.dat	= dataptr ;
    args.bandp	= bandp ;
    args.flagsp	= flagsp ;

    return(read(fd, &args, LIS_GETMSG_PUTMSG_ULEN)) ;
}

#else

#include <linux/types.h>
#include <errno.h>
#include <asm/unistd.h>

#ident "@(#) LiS getpmsg.c 2.4 3/9/03 22:22:36 "

#ifndef __NR_getpmsg

# ifndef __NR_streams1
# ifndef DEP
# error "unistd.h does not define __NR_getpmsg or __NR_streams1"
# endif
# else		/* Linus defined __NR_streams1 rather than __NR_getpmsg */
# define __NR_getpmsg	__NR_streams1
# endif

#endif

#define	__NR_sys_getpmsg	__NR_getpmsg

static inline _syscall5(int,sys_getpmsg,int,fd,
                        void *,ctlptr,void *,dataptr,int *,bandp,int *,flagsp);


int	getpmsg(int fd, void *ctlptr, void *dataptr, int *bandp, int *flagsp)
{
    return(sys_getpmsg(fd, ctlptr, dataptr, bandp, flagsp)) ;
}

#endif
