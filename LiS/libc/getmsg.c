#ident "@(#) LiS getmsg.c 2.2 6/27/00"

extern int
getpmsg(int fd, void *ctlptr, void *dataptr, int *bandp, int *flagsp) ;

int	getmsg(int fd, void *ctlptr, void *dataptr, int *flagsp)
{
    return(getpmsg(fd, ctlptr, dataptr, (void *) 0, flagsp)) ;
}
