#include <linux/types.h>
#include <errno.h>
#include <asm/unistd.h>

#ident "@(#) LiS poll.c 2.3 1/23/02 20:15:23 "

#define	__NR_sys_poll	__NR_poll

static inline _syscall3(int,sys_poll,void *,pollbuf,long,n,int,timout)


int	poll(void *pollfds, long nfds, int timeout)
{
    return(sys_poll(pollfds, nfds, timeout)) ;
}
