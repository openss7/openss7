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
