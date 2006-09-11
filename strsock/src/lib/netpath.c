/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

#define _XOPEN_SOURCE 600
#define _REENTRANT
#define _THREAD_SAFE

#if 0
#define __USE_UNIX98
#define __USE_XOPEN2K
#define __USE_GNU
#endif

#define NEED_T_USCALAR_T 1

#include <stdlib.h>

#include "gettext.h"
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif
#include <sys/stropts.h>
#include <sys/poll.h>
#include <fcntl.h>

#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#else
# ifdef HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif

#ifndef __EXCEPTIONS
#define __EXCEPTIONS 1
#endif

#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stropts.h>
#include <pthread.h>
#include <linux/limits.h>
#include <values.h>

#ifndef __P
#define __P(__prototype) __prototype
#endif

#include <sys/sockmod.h>
#include <sys/socksys.h>
#include <sys/socket.h>
//#include <tihdr.h>
//#include <timod.h>

#if defined __i386__ || defined __x86_64__ || defined __k8__
#define fastcall __attribute__((__regparm__(3)))
#else
#define fastcall
#endif

#define __hot __attribute__((section(".text.hot")))
#define __unlikely __attribute__((section(".text.unlikely")))

#if __GNUC__ < 3
#define inline static inline fastcall __hot
#define noinline extern fastcall __unlikely
#else
#define inline static inline __attribute__((always_inline)) fastcall __hot
#define noinline static __attribute__((noinline)) fastcall __unlikely
#endif

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#undef min
#define min(a, b) (a < b ? a : b)

/**
 * @fn struct netconfig *getnetpath(void *handle);
 * @brief Get the next entry associated with handle.
 * @param handle a pointer to the handle returned by setnetpath().
 *
 * This function returns a pointer to the netconfig database entry corresponding to the first valid
 * NETPATH component.  The netconfig entry is formattted as a struct netconfig.  On each subsequenc
 * all, this function returns a pointer to the netconfig entry that corresponds to the next valud
 * NETPATH component.  This function can thus be used to search the netconfig database for all
 * networks included in the NETPATH variable.  When NETPATH has been exhausted, this function
 * returns NULL.
 *
 * This function silently ignores invalid NETPATH components.  A NETPATH component is invalid if
 * there is no corresponding entry in the netconfig database.
 *
 * If the NETPATH environment variable is unset, this function bethaves as if NETPATH were set to
 * the sequence of "default" or "visible" networks in the netconfig database, in the order in which
 * they are listed.
 *
 * When first called, this function returns a pointer to the netconfig database entry corresponding
 * to the first valid NETPATH component.  When NETPATH has been exhausted, it returns NULL.
 */
struct netconfig *
__nsl_getnetpath(void *handle)
{
	errno = EOPNOTSUPP;
	return (NULL);
}

struct netconfig *getnetpath(void *handle)
    __attribute__ ((weak, alias("__nsl_getnetpath")));

/**
 * @fn void *setnetpath(void);
 * @brief Get a handle for network configuration database.
 *
 * A call to this function binds to or rewinds NETPATH.  This function must be called before the
 * first call to getnetpath() and may be called at any other time.  It returns a handle that is used
 * by getnetpath().
 *
 * This function returns a handle that is used by getnetpath().  In case of an error, this function
 * returns NULL.  nc_perror() or nc_sperror() can be used to print out the reason for failure.  See
 * getnetconfig().
 */
void *
__nsl_setnetpath(void)
{
	errno = EOPNOTSUPP;
	return (NULL);
}

void *setnetpath(void)
    __attribute__ ((weak, alias("__nsl_setnetpath")));

/**
 * @fn int endnetpath(void *handle);
 * @brief Free netpath resources associated with handle.
 * @param handle a pointer to the handle returned by setnetpath().
 *
 * This function may be called to unbind from NETPATH when processing is complete, releasing
 * resources for reuse.  Programmers should be aware, however, that this function frees all memory
 * allocated by getnetpath() for the struct netconfig data structure.
 *
 * This function returns 0 on success and -1 on failure (for example, if setnetpath() was not called
 * previously).
 *
 */
int
__nsl_endnetpath(void *handle)
{
	errno = EOPNOTSUPP;
	return (-1);
}

int endnetpath(void *handle)
    __attribute__ ((weak, alias("__nsl_endnetpath")));
