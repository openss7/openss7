/*
 *    Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 */
#ident "@(#) LiS loop.h 2.5 2/8/03 15:32:37 "

#define	LOOP_IOCTL(n)	(('l' << 8) | (n))
#define LOOP_SET	LOOP_IOCTL(1)
#define LOOP_PUTNXT	LOOP_IOCTL(2)
#define	LOOP_MSGLVL	LOOP_IOCTL(3)
#define	LOOP_TIMR	LOOP_IOCTL(4)
#define	LOOP_MARK	LOOP_IOCTL(5)
#define	LOOP_GET_DEV	LOOP_IOCTL(6)
#define	LOOP_BUFCALL	LOOP_IOCTL(7)
#define	LOOP_CONCAT	LOOP_IOCTL(8)
#define	LOOP_COPY	LOOP_IOCTL(9)
#define	LOOP_DENY_OPEN	LOOP_IOCTL(10)	/* deny 2nd open of device */
#define	LOOP_BURST	LOOP_IOCTL(11)	/* fwd next msg as a burst */
#define	LOOP_FLUSH	LOOP_IOCTL(12)	/* flush and send data */

/*
 * The following structure is sent to loop via a TRANSPARENT ioctl.
 */
typedef struct
{
    int		 cmnd ;		/* see defines below */
    int		 i_arg ;	/* integer argument for cmnd */
    void	*p_arg ;	/* pointer argument for cmnd */

} loop_xparent_t ;

#define	LOOP_XPARENT_COPYIN	101	/* undefined ioctl values == xparent */
#define	LOOP_XPARENT_COPYOUT	102	/* undefined ioctl values == xparent */
