#include <sys/stropts.h>		/* must be LiS version */
#include <unistd.h>
#include <string.h>

#ifdef BLD32OVER64
typedef struct strbuf6 {
    int       maxlen;
    int       len;
    long long buf;
} strbuf6_t;

typedef struct getpmsg_args6
{
    int                  fd ;
    long long            ctl ;
    long long            dat ;
    long long            bandp ;
    long long            flagsp ;

} getpmsg_args6_t ;

int	getpmsg(int fd, strbuf_t *ctlptr, strbuf_t *dataptr,
		int *bandp, int *flagsp)
{
    strbuf6_t ctl6;
    strbuf6_t * ptrc6 = 0;
    strbuf6_t dat6;
    strbuf6_t * ptrd6 = 0;
    getpmsg_args6_t  args ;
    int rc;

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

    memset((void*)&args,0,sizeof(getpmsg_args6_t));
    args.fd	= fd ;
    args.ctl	= (long long)ptrc6 ;
    args.dat	= (long long)ptrd6 ;
    args.bandp	= (long long)bandp ;
    args.flagsp	= (long long)flagsp ;

    rc = read(fd, &args, LIS_GETMSG_PUTMSG_ULEN);

    if (ctlptr)
    {
      ctlptr->len  = ctl6.len;
    }
    if (dataptr)
    {
      dataptr->len = dat6.len;
    }

    return(rc);
}
#else
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
#endif
