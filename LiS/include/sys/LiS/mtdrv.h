/*
 * IOCTL definitions for mtdrv
 */

#ident "@(#) LiS mtdrv.h 1.1 5/14/03"

#define	MTDRV_IOCTL(n)	(('M' << 8) | (n))
#define MTDRV_SET_CLONE_DEV	MTDRV_IOCTL(1)
#define MTDRV_SET_OPEN_SLEEP	MTDRV_IOCTL(2)

