
#ident "@(#) LiS putmsg.c 2.2 6/27/00"

extern int
putpmsg(int fd, void *ctlptr, void *dataptr, int band, int flags) ;

int	putmsg(int fd, void *ctlptr, void *dataptr, int flags)
{
    return(putpmsg(fd, ctlptr, dataptr, -1, flags)) ;
}
