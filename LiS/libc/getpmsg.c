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
