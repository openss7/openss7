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
 * @fn struct netconfig *getnetconfig(void *handle);
 * @brief Retrieve next entry in the netconfig database.
 * @param handle handle returned by setnetconfig().
 *
 * This function returns a pointer to the current entry in the netconfig database, formatted as a
 * struct netconfig.  Successive calls will return successive netconfig entries in the netconfig
 * database.  This function can be used to search the entire netconfig file.  This function returns
 * NULL at the end of the file.  handle is a the handle obtained through setnetconfig().
 *
 */
struct netconfig *
__nsl_getnetconfig(void *handle)
{
	errno = EOPNOTSUPP;
	return (NULL);
}

struct netconfig *getnetconfig(void *handle)
    __attribute__ ((weak, alias("__nsl_getnetconfig")));

/**
 * @fn void *setnetconfig(void);
 * @brief Provide a handle to the netconfig database.
 *
 * This function hase the effect of binding to or rewinding the netconfig database.  This function
 * must be called before the first call to getnetconfig() and may be called at any other time.  This
 * function need not be called before a call to getnetconfigent().  This function returns a unique
 * handle to be used by getnetconfig().
 *
 * This function returns a pointer to the current entry in the netconfig database, formatted as a
 * struct netconfig.  This function returns NULL at the end of the file, or upon failure.
 */
void *
__nsl_setnetconfig(void)
{
	errno = EOPNOTSUPP;
	return (NULL);
}

void *setnetconfig(void)
    __attribute__ ((weak, alias("__nsl_setnetconfig")));

/**
 * @fn int endnetconfig(void *handle);
 * @brief Release network configuration database.
 * @param handle handle returned by setnetconfig().
 *
 * This function should be called when processing is complete to release resources held for reuse.
 * handle is the handle obtained through setnetconfig().  Programmers should be aware, however, that
 * the last call to endnetconfig() frees all memory alocated by getnetconfig() for the struct
 * netconfig data structure.  This function may not be called before setnetconfig().
 *
 * This function returns a unique handle to be used by getnetconfig().  IN the case of an error,
 * this function returns NULL and nc_perror() or nc_sperror() can be used to print the reason for
 * failure.
 *
 */
int
__nsl_endnetconfig(void *handle)
{
	errno = EOPNOTSUPP;
	return (-1);
}

int endnetconfig(void *handle)
    __attribute__ ((weak, alias("__nsl_endnetconfig")));

/**
 * @fn struct netconfig *getnetconfigent(const char *netid);
 * @brief Return a network configuration entry for a network id.
 * @param netid the network id.
 *
 * This function returns a pointer to the struct netconfig structure corresponding to the argument
 * netid.  It returns NULL if netid is invalid (that is, does not name an entry in the netconfig
 * database).
 *
 * This function returns 0 on success and -1 on failure (for example, if setnetconfig() was not
 * called previously).
 */
struct netconfig *
__nsl_getnetconfigent(const char *netid)
{
	errno = EOPNOTSUPP;
	return (NULL);
}

struct netconfig *getnetconfigent(const char *netid)
    __attribute__ ((weak, alias("__nsl_getnetconfigent")));

/**
 * @fn void freenetconfigent(struct netconfig *netconfig);
 * @brief Free a netconfig database entry.
 * @param netconfig the database entry to free.
 *
 * This function frees the netconfig structure pointed to by netconfig (previously returned by
 * getneconfigent()).
 *
 * Upon success, this function returns a pointer to the struct netconfig structure corresponding to
 * the netid; otherwise, it returns NULL.
 */
void
__nsl_freenetconfigent(struct netconfig *netconfig)
{
	errno = EOPNOTSUPP;
	return;
}

void freenetconfigent(struct netconfig *netconfig)
    __attribute__ ((weak, alias("__nsl_freenetconfigent")));

/**
 * @fn void nc_perror(const char *msg);
 * @brief Print an error message to standard output.
 * @param msg message to prefix to error message.
 *
 * This function prints and error message to standard error indicating why any of the above routines
 * failed.  The message is prepended with the string provided in the msg argument and a colon.  A
 * NEWLINE is appended to the end of the message.
 *
 * This function can also be used with the netpath access routines.
 */
void
__nsl_nc_perror(const char *msg)
{
	errno = EOPNOTSUPP;
	return;
}

void nc_perror(const char *msg)
    __attribute__ ((weak, alias("__nsl_nc_perror")));

/**
 * @fn char *nc_sperror(void);
 * @brief Return an error string.
 *
 * This function is similar to nc_perror() but instead of printing the message to standard error,
 * will return a pointer to a string that contains the error message.
 *
 * This function can also be used with the netpath access routines.
 *
 * This function returns a pointer to a buffer that contains the error messages tirng.  This buffer
 * is overwritten on each call.  In multithreaded applications, this buffer is implemented as
 * thread-specific data.
 *
 */
char *
__nsl_nc_sperror(void)
{
	errno = EOPNOTSUPP;
	return (NULL);
}

char *nc_sperror(void)
    __attribute__ ((weak, alias("__nsl_nc_sperror")));
