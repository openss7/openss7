
#ident "@(#) LiS isastream.c 2.5 11/10/03"
#include <stropts.h>
#include <sys/ioctl.h>


int	isastream(int fd)
{
#ifdef USER
    return (user_ioctl( fd, I_CANPUT, 0 ) != -1);
#else
    return (ioctl( fd, I_CANPUT, 0 ) != -1);
#endif
}
