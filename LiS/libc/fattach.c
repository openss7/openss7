#ident "@(#) LiS fattach.c 2.6 11/10/03"

#include <stropts.h>
#include <sys/ioctl.h>


int fattach( int fd, const char *path )
{
    return (ioctl( fd, I_LIS_FATTACH, path ));
}

