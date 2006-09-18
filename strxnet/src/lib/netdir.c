/*****************************************************************************

 @(#) $RCSfile: netdir.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/09/18 00:03:15 $

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

 Last Modified $Date: 2006/09/18 00:03:15 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: netdir.c,v $
 Revision 0.9.2.1  2006/09/18 00:03:15  brian
 - added libxnsl source files

 *****************************************************************************/

#ident "@(#) $RCSfile: netdir.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/09/18 00:03:15 $"

static char const ident[] = "$RCSfile: netdir.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/09/18 00:03:15 $";

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

#include <dlfcn.h>

#ifndef __P
#define __P(__prototype) __prototype
#endif

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
#include <xti_inet.h>
#include <netconfig.h>
#include <netdir.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include "netselect.h"

/*
 *  Name-to-Address Translation Functions:
 *  ======================================
 */

static pthread_rwlock_t __nsl_xlate_lock = PTHREAD_RWLOCK_INITIALIZER;

struct __nsl_xlate {
	struct __nsl_xlate *next;	/* next in list */
	char *path;			/* full path to library */
	nd_gbn_t gbn;			/* _netdir_getbyname */
	nd_gba_t gba;			/* _netdir_getbyaddr */
	nd_opt_t opt;			/* _netdir_options */
	nd_t2u_t t2u;			/* _taddr2uaddr */
	nd_u2t_t u2t;			/* _uaddr2taddr */
	void *lib;			/* dlopen pointer */
} *__nsl_xlate_list = NULL;

/*
 *  A little explanation is in order here.  SVR4 provides NSS defaults for inet
 *  if name to address translation libraries are not specified.  The functions
 *  below accomplish this.  However, they are here defined as weak undefined
 *  symbols and they only come into effect if the libn2a_inet library is linked.
 *  Also, as the libn2a_inet library also defines the netdir symbols, this
 *  library can also be specified as a name-to-address translation library in
 *  netconfig.  libn2a_inet is provided separately by the strinet package.
 */
extern struct nd_addrlist *__inet_netdir_getbyname(struct netconfig *nc, struct nd_hostserv *service);
extern struct nd_hostservlist *__inet_netdir_getbyaddr(struct netconfig *nc, struct netbuf *addr);
extern int __inet_netdir_options(struct netconfig *nc, int option, int fd, char *pta);
extern char *__inet_taddr2uaddr(struct netconfig *nc, struct netbuf *taddr);
extern struct netbuf *__inet_uaddr2taddr(struct netconfig *nc, struct netbuf *uaddr);
extern char *__inet_netdir_mergeaddr(struct netconfig *nc, char *caddr, char *saddr);

#pragma weak __inet_netdir_getbyname
#pragma weak __inet_netdir_getbyaddr
#pragma weak __inet_netdir_options
#pragma weak __inet_taddr2uaddr
#pragma weak __inet_uaddr2taddr
#pragma weak __inet_netdir_mergeaddr

/**
 * @fn static struct __nsl_xlate *__nsl_load_xlate(const char *name);
 * @brief Load a name-to-address translation library.
 * @param name the name of the library to load.
 */
static struct __nsl_xlate *
__nsl_load_xlate(const char *name)
{
	struct __nsl_xlate *xl;

	pthread_rwlock_wrlock(&__nsl_xlate_lock);
	for (xl = __nsl_xlate_list; xl; xl = xl->next)
		if (strcmp(name, xl->path) == 0)
			goto found;
	if ((xl = malloc(sizeof(*xl))) == NULL)
		goto nomem;
	memset(xl, 0, sizeof(*xl));
	if ((xl->path = strdup(name)) == NULL)
		goto nomem;
	if ((xl->lib = dlopen(name, RTLD_LAZY)) == NULL)
		goto open;
	/* resolve all symbols from the n2a library */
	if ((xl->gbn = (nd_gbn_t) dlsym(xl->lib, "_netdir_getbyname")) == NULL)
		goto nosym;
	if ((xl->gba = (nd_gba_t) dlsym(xl->lib, "_netdir_getbyaddr")) == NULL)
		goto nosym;
	if ((xl->opt = (nd_opt_t) dlsym(xl->lib, "_netdir_options")) == NULL)
		goto nosym;
	if ((xl->t2u = (nd_t2u_t) dlsym(xl->lib, "_netdir_taddr2uaddr")) == NULL)
		goto nosym;
	if ((xl->u2t = (nd_u2t_t) dlsym(xl->lib, "_netdir_uaddr2taddr")) == NULL)
		goto nosym;
	xl->next = __nsl_xlate_list;
	__nsl_xlate_list = xl;
	goto found;
      nosym:
	_nderror = ND_NOSYM;
	goto error;
      open:
	_nderror = ND_OPEN;
	goto error;
      nomem:
	_nderror = ND_NOMEM;
	goto error;
      error:
	if (xl != NULL) {
		if (xl->lib != NULL)
			dlclose(xl->lib);
		if (xl->path != NULL)
			free(xl->path);
		free(xl);
		xl = NULL;
	}
      found:
	pthread_rwlock_unlock(&__nsl_xlate_lock);
	return (xl);
}

/**
 * @internal Deep free a netbuf structure.
 * @param b the netbuf structure to free.
 */
static void
__freenetbuf(struct netbuf *b)
{
	if (b) {
		if (b->buf)
			free(b->buf);
		free(b);
	}
}

/**
 * @internal Deep free an nd_addrlist structure.
 * @param n the nd_addrlist structure to free.
 */
static void
__freeaddrlist(struct nd_addrlist *n)
{
	if (n) {
		int i;
		struct netbuf *b;

		if ((b = n->n_addrs)) {
			for (i = 0; i < n->n_cnt; i++, b++) {
				if (b->buf)
					free(b->buf);
			}
			free(n->n_addrs);
		}
		free(n);
	}
}

/**
 * @internal Deep free an nd_hostserv structure.
 * @param n the nd_hostserv structure to free.
 */
static void
__freehostserv(struct nd_hostserv *h)
{
	if (h) {
		if (h->h_name)
			free(h->h_name);
		if (h->h_serv)
			free(h->h_serv);
		free(h);
	}
}

/**
 * @internal Deep free an nd_hostservlist structure.
 * @param n the nd_hostservlist structure to free.
 */
static void
__freehostservlist(struct nd_hostservlist *hl)
{
	if (hl) {
		int i;
		struct nd_hostserv *h;

		if ((h = hl->h_hostservs)) {
			for (i = 0; i < hl->h_cnt; i++, h++) {
				if (h->h_name)
					free(h->h_name);
				if (h->h_serv)
					free(h->h_serv);
			}
			free(hl->h_hostservs);
		}
		free(hl);
	}
}

/**
 * @fn static struct __nsl_xlate * __nsl_lookup_xlate(const char *name);
 * @brief Lookup a name-to-address translation library.
 * @param name the name of the library to look up.
 *
 * This function looks up the name-to-address translation library under read
 * lock.  Libraries, once loaded are never unloaded so this will work.
 */
static struct __nsl_xlate *
__nsl_lookup_xlate(const char *name)
{
	struct __nsl_xlate *xl;

	pthread_rwlock_rdlock(&__nsl_xlate_lock);
	for (xl = __nsl_xlate_list; xl; xl = xl->next)
		if (strcmp(name, xl->path) == 0)
			break;
	pthread_rwlock_unlock(&__nsl_xlate_lock);
	if (xl != NULL)
		return (xl);
	return (__nsl_load_xlate(name));
}

/**
 * @fn int netdir_getbyname(struct netconfig *nc, struct nd_hostserv *service, struct nd_addrlist **addrs);
 * @brief Map machine and service name to transport addresses.
 * @param nc pointer to transport configuration data structure.
 * @param service pointer to nd_hostserv structure identifying machine and service name.
 * @param addrs returned pointer to allocated nd_addrlist representing the resulting transport addresses.
 *
 * This function converts the machine name and service name in the nd_hostserv
 * structure to a collection of addresses of the type understood by the
 * transport identified in the netconfig structure.
 */
int
__nsl_netdir_getbyname(struct netconfig *nc, struct nd_hostserv *service,
		       struct nd_addrlist **addrs)
{
	if (nc == NULL) {
		_nderror = ND_BADARG;
	} else if (nc->nc_nlookups == 0 && strcmp(nc->nc_protofmly, NC_INET) == 0 && __inet_netdir_getbyname) {
		return ((*addrs = __inet_netdir_getbyname(nc, service)) ? 0 : -1);
	} else {
		struct __nsl_xlate *xl;
		int i;

		for (i = 0; i < nc->nc_nlookups; i++) {
			if ((xl = __nsl_lookup_xlate(nc->nc_lookups[i]))
			    && (*addrs = (*xl->gbn) (nc, service)))
				return (0);
			if (_nderror < 0)
				return (-1);
		}
		if (i == 1)
			_nderror = ND_NO_RECOVERY;
	}
	return (-1);
}

#pragma weak __nsl_netdir_getbyname
__asm__(".symver __nsl_netdir_getbyname,netdir_getbyname@@XNSL_1.0");

/**
 * @fn int netdir_getbyaddr(struct netconfig *nc, struct nd_hostservlist **service, struct netbuf *netaddr);
 * @brief Map transport address to a collection of machine and service names.
 * @param nc pointer to transport configuration data structure.
 * @param service returned pointer to a nd_hostservlist structure identifying the machine and service names.
 * @param netaddr pointer to a netbuf structure describing the transport address.
 *
 * This function maps addresses to service names.  The function returns a
 * service, a list of host and service pairs that yeild these addresses.  If
 * more than one tuple of host and service name is returned, the first type
 * contains the preferred host and service names.
 */
int
__nsl_netdir_getbyaddr(struct netconfig *nc, struct nd_hostservlist **service,
		       struct netbuf *netaddr)
{
	if (nc == NULL) {
		_nderror = ND_BADARG;
	} else if (nc->nc_lookups == 0 && strcmp(nc->nc_protofmly, NC_INET) == 0
		   && __inet_netdir_getbyaddr) {
		return ((*service = __inet_netdir_getbyaddr(nc, netaddr)) ? 0 : -1);
	} else {
		struct __nsl_xlate *xl;
		int i;

		for (i = 0; i < nc->nc_nlookups; i++) {
			if ((xl = __nsl_lookup_xlate(nc->nc_lookups[i]))
			    && (*service = (*xl->gba) (nc, netaddr)) != NULL)
				return (0);
			if (_nderror < 0)
				return (-1);
		}
		if (i == 1)
			_nderror = ND_NO_RECOVERY;
	}
	return (-1);
}

#pragma weak __nsl_netdir_getbyaddr
__asm__(".symver __nsl_netdir_getbyaddr,netdir_getbyaddr@@XNSL_1.0");

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
	if (ptr == NULL) {
		_nderror = ND_BADARG;
		return;
	}
	switch (struct_type) {
	case ND_ADDR:
		return __freenetbuf((struct netbuf *)ptr);
	case ND_ADDRLIST:
		return __freeaddrlist((struct nd_addrlist *)ptr);
	case ND_HOSTSERV:
		return __freehostserv((struct nd_hostserv *)ptr);
	case ND_HOSTSERVLIST:
		return __freehostservlist((struct nd_hostservlist *)ptr);
	default:
		_nderror = ND_UKNWN;
		break;
	}
	return;
}

#pragma weak __nsl_netdir_free
__asm__(".symver __nsl_netdir_free,netdir_free@@XNSL_1.0");

/**
 * @fn int netdir_options(struct netconfig *nc, int option, int fd, char *pta);
 * @brief manage universal transport options.
 * @param nc pointer to transport configuration structure.
 * @param universal option option to manage.
 * @param fd transport endpoint file descriptor.
 * @param pta arguments to set.
 *
 * This function is used to do all transport specific setups and option
 * management.  fd is the associated file descriptor.  option, fd, and
 * pointer_to_args are passed to the netdir_options() function for the
 * transport specified in nc.
 */
int
__nsl_netdir_options(struct netconfig *nc, int option, int fd, char *pta)
{
	if (nc == NULL) {
		_nderror = ND_BADARG;
	} else if (nc->nc_nlookups == 0 && strcmp(nc->nc_protofmly, NC_INET) == 0 && __inet_netdir_options) {
		return (__inet_netdir_options(nc, option, fd, pta));
	} else {
		struct __nsl_xlate *xl;
		int i;

		for (i = 0; i < nc->nc_nlookups; i++) {
			if ((xl = __nsl_lookup_xlate(nc->nc_lookups[i]))
			    && (*xl->opt) (nc, option, fd, pta) == 0)
				return (0);
			if (_nderror < 0)
				return (-1);
		}
		_nderror = ND_FAILCTRL;
	}
	return (-1);
}

#pragma weak __nsl_netdir_options
__asm__(".symver __nsl_netdir_options,netdir_options@@XNSL_1.0");

/**
 * @fn char *taddr2uaddr(struct netconfig *nc, struct netbuf *taddr);
 * @brief Converts a transport address to a universal address.
 * @param nc a pointer to the transport configuration data structure.
 * @param taddr the transport address to convert.
 *
 * This function supports transaltion between universal addresses and TLI type
 * netbufs.  The taddr2uaddr() function takes a struct netbuf data structure
 * and returns a pointer to a string that contains the universal address.  It
 * returns NULL if the conversion is not possible.  This is not a fatal
 * condition as some transports do not support a universal address form.
 */
char *
__nsl_taddr2uaddr(struct netconfig *nc, struct netbuf *taddr)
{
	if (nc == NULL) {
		_nderror = ND_BADARG;
	} else if (nc->nc_nlookups == 0 && strcmp(nc->nc_protofmly, NC_INET) == 0
		   && __inet_taddr2uaddr) {
		return (__inet_taddr2uaddr(nc, taddr));
	} else {
		struct __nsl_xlate *xl;
		char *uaddr;
		int i;

		for (i = 0; i < nc->nc_nlookups; i++) {
			if ((xl = __nsl_lookup_xlate(nc->nc_lookups[i]))
			    && (uaddr = (*xl->t2u) (nc, taddr)))
				return (uaddr);
			if (_nderror < 0)
				return (NULL);
		}
		if (i == 1)
			_nderror = ND_NO_RECOVERY;
	}
	return (NULL);
}

#pragma weak __nsl_taddr2uaddr
__asm__(".symver __nsl_taddr2uaddr,taddr2uaddr@@XNSL_1.0");

/**
 * @fn struct netbuf *uaddr2taddr(struct netconfig *nc, struct netbuf *uaddr);
 * @brief Converts a universal address to a transport address.
 * @param nc a pointer to the transport configuration data structure.
 * @param uaddr the universal address to convert.
 *
 * This function is the reverse of the taddr2uaddr() function. It returns the
 * struct netbuf data structure for the given universal address.
 *
 * It is wierd that noone documents what happens if there are no
 * name-to-address translation libraries and this function is called.
 */
struct netbuf *
__nsl_uaddr2taddr(struct netconfig *nc, struct netbuf *uaddr)
{
	if (nc == NULL) {
		_nderror = ND_BADARG;
	} else if (nc->nc_nlookups == 0 && strcmp(nc->nc_protofmly, NC_INET) == 0
		   && __inet_uaddr2taddr) {
		return (__inet_uaddr2taddr(nc, uaddr));
	} else {
		struct __nsl_xlate *xl = NULL;
		struct netbuf *taddr;
		int i;

		for (i = 0; i < nc->nc_nlookups; i++) {
			if ((xl = __nsl_lookup_xlate(nc->nc_lookups[i]))
			    && (taddr = (*xl->u2t) (nc, uaddr)))
				return (taddr);
			if (_nderror < 0)
				return (NULL);
		}
		if (i == 1)
			_nderror = ND_NO_RECOVERY;
	}
	return (NULL);
}

#pragma weak __nsl_uaddr2taddr
__asm__(".symver __nsl_uaddr2taddr,uaddr2taddr@@XNSL_1.0");

/* *INDENT-OFF* */
static const char *__nsl_nd_errlist[] = {
/*
TRANS Corresponds to the ND_TRY_AGAIN constant.  This is a fatal error in the
TRANS sense that name-to-address mapping translations will stop at this point.
 */
	[ND_TRY_AGAIN-ND_ERROR_OFS] = gettext_noop("n2a: non-authoritative host not found"),
/*
TRANS Corresponds to the ND_NO_RECOVERY constant.  This is a fatal error in the
TRANS sense that name-to-address mapping translations will stop at this point.
 */
	[ND_NO_RECOVERY-ND_ERROR_OFS] = gettext_noop("n2a: non-recoverable error"),
/*
TRANS Corresponds to the ND_NO_DATA and ND_NO_ADDRESS constant.  This is a fatal
TRANS error in the sense that name-to-address mapping translations will stop at
TRANS this point.
 */
	[ND_NO_DATA-ND_ERROR_OFS] = gettext_noop("n2a: no data record of requested type"),
/*
TRANS Corresponds to the ND_BADARG constant.  This is a fatal error in the sense
TRANS that name-to-address mapping translations will stop at this point.
 */
	[ND_BADARG-ND_ERROR_OFS] = gettext_noop("n2a: bad arguments passed to routine"),
/*
TRANS Corresponds to the ND_NOMEM constant.  This is a fatal error in the sense
TRANS that name-to-address mapping translations will stop at this point.
 */
	[ND_NOMEM-ND_ERROR_OFS] = gettext_noop("n2a: memory allocation failed"),
/*
TRANS Corresponds to the ND_OK constant.  This is not an error.
 */
	[ND_OK-ND_ERROR_OFS] = gettext_noop("n2a: successful completion"),
/*
TRANS Corresponds to the ND_NOHOST constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	[ND_NOHOST-ND_ERROR_OFS] = gettext_noop("n2a: hostname not found"),
/*
TRANS Corresponds to the ND_NOSERV constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	[ND_NOSERV-ND_ERROR_OFS] = gettext_noop("n2a: service name not found"),
/*
TRANS Corresponds to the ND_NOSYM constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.  %1$s is a dlerror(3) string.
 */
	[ND_NOSYM-ND_ERROR_OFS] = gettext_noop("n2a: symbol missing in shared object: %s"),
/*
TRANS Corresponds to the ND_OPEN constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.  %1$s is a dlerror(3) string.
 */
	[ND_OPEN-ND_ERROR_OFS] = gettext_noop("n2a: could not open shared object: %s"),
/*
TRANS Corresponds to the ND_ACCESS constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	[ND_ACCESS-ND_ERROR_OFS] = gettext_noop("n2a: access denied for shared object"),
/*
TRANS Corresponds to the ND_UKNWN constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	[ND_UKNWN-ND_ERROR_OFS] = gettext_noop("n2a: attempt to free unknown object"),
/*
TRANS Corresponds to the ND_NOCTRL constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	[ND_NOCTRL-ND_ERROR_OFS] = gettext_noop("n2a: unknown option passed"),
/*
TRANS Corresponds to the ND_FAILCTRL constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	[ND_FAILCTRL-ND_ERROR_OFS] = gettext_noop("n2a: control operation failed"),
/*
TRANS Corresponds to the ND_SYSTEM constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	[ND_SYSTEM-ND_ERROR_OFS] = gettext_noop("n2a: system error"),
/*
TRANS Corresponds to the ND_NOCONVERT constant.  This is a non-fatal error in
TRANS the sense that name-to-address mapping translations will contrinue at this
TRANS point.
 */
	[ND_NOCONVERT-ND_ERROR_OFS] = gettext_noop("n2a: conversion not possible"),
/*
TRANS Corresponds to the any other constant.  This is a non-fatal error in the
TRANS sense that name-to-address mapping translations will contrinue at this
TRANS point. %1$d is the error number.
 */
	[ND_ERROR_MAX-ND_ERROR_OFS] = gettext_noop("n2a: unknown error %d"),
};
/* *INDENT-ON* */

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
	int err, idx;

	if (nd_errbuf != NULL) {
		err = nd_error;
		idx = err - ND_ERROR_OFS;
		switch (err) {
		case ND_TRY_AGAIN:
		case ND_NO_RECOVERY:
		case ND_NO_DATA:
		case ND_BADARG:
		case ND_NOMEM:
		case ND_OK:
		case ND_NOHOST:
		case ND_NOSERV:
		case ND_ACCESS:
		case ND_UKNWN:
		case ND_NOCTRL:
		case ND_FAILCTRL:
		case ND_SYSTEM:
		case ND_NOCONVERT:
			(void) strncpy(nd_errbuf, __nsl_nd_errlist[idx], NDERR_BUFSZ);
			break;
		case ND_NOSYM:
		case ND_OPEN:
			(void) snprintf(nd_errbuf, NDERR_BUFSZ, __nsl_nd_errlist[idx], dlerror());
			break;
		default:
			idx = ND_ERROR_MAX - ND_ERROR_OFS;
			(void) snprintf(nd_errbuf, NDERR_BUFSZ, __nsl_nd_errlist[idx], err);
			break;
		}
	}
	return (nd_errbuf);
}

#pragma weak __nsl_netdir_sperror
__asm__(".symver __nsl_netdir_sperror,netdir_sperror@@XNSL_1.0");

/**
 * @fn void netdir_perror(char *msg);
 * @brief Print a netdir function error string to standard output.
 * @param msg prefix string.
 *
 * This function prints an error message to standard output that states the
 * cause of a name-to-address mapping failure.  The error message is preceded by
 * the string given as an argument.
 *
 */
void
__nsl_netdir_perror(char *msg)
{
	if (msg)
		fprintf(stderr, "%s: %s\n", msg, __nsl_netdir_sperror());
	else
		fprintf(stderr, "%s\n", __nsl_netdir_sperror());
	return;
}
#pragma weak __nsl_netdir_perror
__asm__(".symver __nsl_netdir_perror,netdir_perror@@XNSL_1.0");

/*
 * vim: comments+=b\:TRANS
 */
