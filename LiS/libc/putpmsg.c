#include <sys/stropts.h>		/* must be LiS version */
#include <unistd.h>
#include <string.h>

#ifdef BLD32OVER64
typedef struct strbuf6 {
    int       maxlen;
    int       len;
    long long buf;
} strbuf6_t;

typedef struct putpmsg_args6
{
    int                  fd ;
    long long            ctl ;
    long long            dat ;
    int                  band ;
    int                  flags ;

} putpmsg_args6_t ;

int	putpmsg(int fd, strbuf_t *ctlptr, strbuf_t *dataptr,
		int band, int flags)
{
    putpmsg_args6_t	args ;
    strbuf6_t ctl6;
    strbuf6_t * ptrc6 = 0;
    strbuf6_t dat6;
    strbuf6_t * ptrd6 = 0;

    if (ctlptr)
    {
      ctl6.maxlen = ctlptr->maxlen;
      ctl6.len    = ctlptr->len;
      ctl6.buf    = (long long)ctlptr->buf;
      ptrc6 = &ctl6;
    }

    if (dataptr)
    {
      dat6.maxlen = dataptr->maxlen;
      dat6.len    = dataptr->len;
      dat6.buf    = (long long)dataptr->buf;
      ptrd6 = &dat6;
    }

    memset((void*)&args,0,sizeof(putpmsg_args6_t));
    args.fd	= fd ;
    args.ctl	= (long long)ptrc6 ;
    args.dat	= (long long)ptrd6 ;
    args.band	= band ;
    args.flags	= flags ;

    return(write(fd, &args, LIS_GETMSG_PUTMSG_ULEN)) ;
}
#else
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
#endif
