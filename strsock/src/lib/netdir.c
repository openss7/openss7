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

/*
 *  This is the classical Name Services Library.
 */

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

#include <xti.h>
#include <netconfig.h>
#include <netdir.h>

/**
 * @fn int netdir_getbyname(struct netconfig *config, struct nd_hostserv *service, struct nd_addrlist **addrs);
 * @brief Map machine and service name to transport addresses.
 * @param config pointer to transport configuration data structure.
 * @param service pointer to nd_hostserv structure identifying machine and service name.
 * @param addrs returned pointer to allocated nd_addrlist representing the resulting transport addresses.
 *
 * This function converts the machine anem and service name in the nd_hostserv
 * structure to a collection of addresses of the type understood by the
 * transport identified in the netconfig structure.
 */
int
__nsl_netdir_getbyname(struct netconfig *config, struct nd_hostserv *service,
		       struct nd_addrlist **addrs)
{
	errno = EOPNOTSUPP;
	return (-1);
}

int netdir_getbyname(struct netconfig *config, struct nd_hostserv *service,
		     struct nd_addrlist **addrs)
    __attribute__ ((alias("__nsl_netdir_getbyname")));

/**
 * @fn int netdir_getbyaddr(struct netconfig *config, struct nd_hostservlist **service, struct netbuf *netaddr);
 * @brief Map transport address to a collection of machine and service names.
 * @param config pointer to transport configuration data structure.
 * @param service returned pointer to a nd_hostservlist structure identifying the machine and service names.
 * @param netaddr pointer to a netbuf structure describing the transport address.
 *
 * This function maps addresses to service names.  The function returns a
 * service, a list of host and service pairs that yeild these addresses.  If
 * more than one tuple of host and service name is returned, the first type
 * contains the preferred host and service names.
 */
int
__nsl_netdir_getbyaddr(struct netconfig *config, struct nd_hostservlist **service,
		       struct netbuf *netaddr)
{
	errno = EOPNOTSUPP;
	return (-1);
}

int netdir_getbyaddr(struct netconfig *config, struct nd_hostservlist **service,
		     struct netbuf *netaddr)
    __attribute__ ((alias("__nsl_netdir_getbyaddr")));

/**
 * @fn void netdir_free(void *ptr, int struct_type);
 * @brief frees a structure returned by other netdir functions.
 * @param ptr pointer to the structure to free.
 * @param struct_type type of structure.
 *
 * This function is used to free the structures allocated by the name to
 * address translation functions.  The ptr parameter points to the structure
 * that has to be freed.  The parameter struct_type identifies the structure
 * and is one of ND_ADDR, ND_ADDRLIST, ND_HOSTSERV, ND_HOSTSERVLIST.  Note
 * that universal addresses are freed with free().
 */
void
__nsl_netdir_free(void *ptr, int struct_type)
{
	errno = EOPNOTSUPP;
}

void netdir_free(void *ptr, int struct_type)
    __attribute__ ((alias("__nsl_netdir_free")));

/**
 * @fn int netdir_options(struct netconfig *config, int option, int fd, char *point_to_args);
 * @brief manage universal transport options.
 * @param config pointer to transport configuration structure.
 * @param universal option option to manage.
 * @param fd transport endpoint file descriptor.
 * @param point_to_args arguments to set.
 *
 * This function is used to do all transport specific setups and option
 * management.  fd is the associated file descriptor.  option, fd, and
 * pointer_to_args are passed ot the netdir_options() function for the
 * transport specified in config.
 */
int
__nsl_netdir_options(struct netconfig *config, int option, int fd, char *point_to_args)
{
	errno = EOPNOTSUPP;
	return (-1);
}

int netdir_options(struct netconfig *config, int option, int fd, char *point_to_args)
    __attribute__ ((alias("__nsl_netdir_options")));

/**
 * @fn char *taddr2uaddr(struct netconfig *config, struct netbuf *addr);
 * @brief Converts a transport address to a universal address.
 * @param config a pointer to the transport configuration data structure.
 * @param addr the transport address to convert.
 *
 * This function supports transaltion between universal addresses and TLI type
 * netbufs.  The taddr2uaddr() function takes a struct netbuf data structure
 * and returns a pointer to a string that contains the universal address.  It
 * returns NULL if the conversion is not possible.  This is not a fatal
 * condition as some transports do not support a universal address form.
 */
char *
__nsl_taddr2uaddr(struct netconfig *config, struct netbuf *addr)
{
	errno = EOPNOTSUPP;
	return (NULL);
}

char *taddr2uaddr(struct netconfig *config, struct netbuf *addr)
    __attribute__ ((alias("__nsl_taddr2uaddr")));

/**
 * @fn struct netbuf *uaddr2taddr(struct netconfig *config, struct netbuf *addr);
 * @brief Converts a universal address to a transport address.
 * @param config a pointer to the transport configuration data structure.
 * @param addr the universal address to convert.
 *
 * This function is the reverse of the taddr2uaddr() function. It returns the
 * struct netbuf data structure for the given universal address.
 */
struct netbuf *
__nsl_uaddr2taddr(struct netconfig *config, struct netbuf *addr)
{
	errno = EOPNOTSUPP;
	return (NULL);
}

struct netbuf *uaddr2taddr(struct netconfig *config, struct netbuf *addr)
    __attribute__ ((alias("__nsl_uaddr2taddr")));

/**
 * @fn void netdir_perror(char *s);
 * @brief Print a netdir function error string to standard output.
 * @param s prefix string.
 *
 * This function prints an error message to standard output that states the
 * cause of a name-to-address mapping failure.  The error message is preceded
 * by the string given as an argument.
 *
 */
void
__nsl_netdir_perror(char *s)
{
	errno = EOPNOTSUPP;
}
void netdir_perror(char *s)
    __attribute__ ((alias("__nsl_netdir_perror")));

/**
 * @fn char *netdir_sperror(void);
 * @brief Return a netdir function error string.
 *
 * This function returns a pointer to a buffer that contains the error message
 * string.  The buffer is overwritten on each call.  In multhreaded
 * applications, this buffers is implemented as thread-specific data.
 */
char *
__nsl_netdir_sperror(void)
{
	errno = EOPNOTSUPP;
	return (NULL);
}
char *netdir_sperror(void)
    __attribute__ ((alias("__nsl_netdir_sperror")));
