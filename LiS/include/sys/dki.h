/************************************************************************
*                     SVR4 Driver-Kernel Interface			*
*************************************************************************
*									*
* These are the external definitions for SVR4 compatible DKI functions.	*
*									*
* Author:	David Grothe <dave@gcom.com>				*
*									*
* Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>		*
*									*
************************************************************************/
/*
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

#ifndef	DKI_H
#define	DKI_H		1

#ident "@(#) LiS dki.h 2.13 09/13/04 10:13:51 "

#ifndef _SYS_TYPES_H
#include <sys/types.h>
#endif


#ifndef NULL
#define	NULL		( (void *) 0 )
#endif

/************************************************************************
*                           Timeouts                                    *
*************************************************************************
*									*
* The function timeout is called with a pointer to a function to	*
* invoke at a later time, an argument to be passed to the function,	*
* and the number of ticks to wait.  'timeout' returns an integer	*
* that serves as a "handle" for the timer.  The untimeout function	*
* takes a timer handle as an argument and stops the timer.		*
*									*
* These two functions are defined in the dki.c file.  Their		*
* implementation depends upon lis_tmout() and lis_untimout() which	*
* are defined in either linux-mdep.c, which uses Linux timers to	*
* implement the functions, or in the port-mdep.c file, which implements	*
* a portable timer algorithm based on the Linux timer mechanism.	*
*									*
************************************************************************/

#ifdef __KERNEL__

typedef void	_RP timo_fcn_t(caddr_t arg) ;
typedef unsigned long	 toid_t ;		/* SVR4 */
typedef unsigned long	 timeout_id_t ;		/* Solaris */

#define	timeout(fcn,arg,ticks)	lis_timeout_fcn(fcn,arg,ticks,__FILE__, __LINE__)
#define	untimeout		lis_untimeout

extern toid_t	lis_timeout_fcn(timo_fcn_t *timo_fcn, caddr_t arg, long ticks,
			    char *file_name, int line_nr) _RP;
extern toid_t	lis_untimeout(toid_t id) _RP;

/*
 * The following are internal routines not exported
 */
void lis_initialize_dki(void) ;			/* dki.c */
void lis_terminate_dki(void) ;			/* dki.c */

void *lis_alloc_timer(char *file, int line) ;	/* mdep rouitine */
void *lis_free_timer(void *timerp) ;		/* mdep rouitine */
void lis_init_timers(int size) ;		/* mdep routine */
void lis_terminate_timers(void) ;		/* mdep rouitine */

#endif				/* __KERNEL__ */



#ifndef HZ
#define	HZ	100			/* ticks per second */
#endif

/*
 * Convert micro-seconds to proper ticks value.  The return value of
 * this routine is suitable for use with the "timeout" function.
 *
 * The routine is located in osif.c.
 */
unsigned lis_usectohz(unsigned usec) _RP;

/************************************************************************
*                        Creating Nodes                                 *
*************************************************************************
*									*
* The following two routines can be used to create and remove nodes	*
* in the "/dev" directory.  These routines are used by drivers that	*
* wish to create these nodes dynamically at driver initialization time.	*
* They are especially useful for dynamically loaded drivers which obtain*
* their major device number at module load time.			*
*									*
* These routines have the same calling sequence as their user level	*
* versions (do "man mknod" for example).  These all return 0 upon	*
* success and a negative error number upon failure.			*
*									*
* When using mknod, remember to or in S_IFCHR with the mode (<stat.h>).	*
*									*
************************************************************************/

extern int	lis_mknod(char *name, int mode, dev_t dev) _RP;
extern int	lis_unlink(char *name) _RP;
extern int	lis_mount(char *dev_name,
			  char *dir_name,
			  char *fstype,
			  unsigned long rwflag,
			  void *data) ;
extern int	lis_umount2(char *path, int flags) ;




#endif				/* from top of file */
