/*****************************************************************************

 @(#) $RCSfile: netselect.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2008-08-20 10:57:03 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-08-20 10:57:03 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: netselect.c,v $
 Revision 0.9.2.8  2008-08-20 10:57:03  brian
 - fixes and build updates from newnet trip

 Revision 0.9.2.7  2008/07/11 00:23:41  brian
 - gettext declarations

 Revision 0.9.2.6  2008-04-28 18:38:38  brian
 - header updates for release

 Revision 0.9.2.5  2007/08/14 04:00:51  brian
 - GPLv3 header update

 Revision 0.9.2.4  2006/09/24 21:57:23  brian
 - documentation and library updates

 Revision 0.9.2.3  2006/09/22 20:54:27  brian
 - tweaked source file for use with doxygen

 Revision 0.9.2.2  2006/09/18 13:52:56  brian
 - added doxygen markers to sources

 Revision 0.9.2.1  2006/09/18 00:03:15  brian
 - added libxnsl source files

 *****************************************************************************/

#ident "@(#) $RCSfile: netselect.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2008-08-20 10:57:03 $"

static char const ident[] =
    "$RCSfile: netselect.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2008-08-20 10:57:03 $";

/* This file can be processed with doxygen(1). */

/** @weakgroup nsf Network Selection Facility
  * @{ */

/** @file
  * Network selection facility implementation file.  */

#define _XOPEN_SOURCE 600
#define _REENTRANT
#define _THREAD_SAFE

#if 0
#define __USE_UNIX98
#define __USE_XOPEN2K
#define __USE_GNU
#endif

#undef __USE_STRING_INLINES

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
 *  Once condition for Thread-Specific Data key creation.
 */
static pthread_once_t __nsl_tsd_once = PTHREAD_ONCE_INIT;

/*
 *  XNSL library Thread-Specific Data key.
 */
static pthread_key_t __nsl_tsd_key = 0;

static void
__nsl_tsd_free(void *buf)
{
	pthread_setspecific(__nsl_tsd_key, NULL);
	free(buf);
}

static void
__nsl_tsd_key_create(void)
{
	pthread_key_create(&__nsl_tsd_key, __nsl_tsd_free);
}

static struct __nsl_tsd *
__nsl_tsd_alloc(void)
{
	struct __nsl_tsd *tsdp;

	tsdp = (typeof(tsdp)) malloc(sizeof(*tsdp));
	memset(tsdp, 0, sizeof(*tsdp));
	pthread_setspecific(__nsl_tsd_key, (void *) tsdp);
	return (tsdp);
}

/** @internal
  * @brief Get thread specific data for the xnsl library.
  *
  * This function obtains (and allocates if necessary), thread specific data for
  * the executing thread.  */
static struct __nsl_tsd *
__nsl_get_tsd(void)
{
	struct __nsl_tsd *tsdp;

	pthread_once(&__nsl_tsd_once, __nsl_tsd_key_create);
	if (unlikely((tsdp = (typeof(tsdp)) pthread_getspecific(__nsl_tsd_key)) == NULL))
		tsdp = __nsl_tsd_alloc();
	return (tsdp);
};

int *
__nsl___ncerror(void)
{
	return &(__nsl_get_tsd()->ncerror);
}

__asm__(".symver __nsl___ncerror,__ncerror@@XNSL_1.0");

int *
__nsl___nderror(void)
{
	return &(__nsl_get_tsd()->nderror);
}

__asm__(".symver __nsl___nderror,__nderror@@XNSL_1.0");

char *
__nsl___ncerrbuf(void)
{
	return &(__nsl_get_tsd()->ncerrbuf[0]);
}

__asm__(".symver __nsl___ncerrbuf,__ncerrbuf@@XNSL_1.0");

char *
__nsl___nderrbuf(void)
{
	return &(__nsl_get_tsd()->nderrbuf[0]);
}

__asm__(".symver __nsl___nderrbuf,__nderrbuf@@XNSL_1.0");

/*
 *  Network Selection Facility functions:
 *  =====================================
 */

/* This is to allow us to change the name of the network configuration
 * database file primarily for testing with test scripts. */

const char *__nsl_netconfig = NETCONFIG;

#undef NETCONFIG
#define NETCONFIG __nsl_netconfig

static pthread_rwlock_t __nsl_nc_lock = PTHREAD_RWLOCK_INITIALIZER;
static struct netconfig **__nsl_nc_list = NULL;

static struct netconfig **
__nsl_loadnetconfiglist(void)
{
	char buffer[1024];
	char *line;
	int linenum, entries = 0;
	FILE *file = NULL;
	struct netconfig *nc = NULL, *nclist = NULL, **nclistp = &nclist;
	struct __nsl_tsd *tsd = __nsl_get_tsd();

	/* try with .xnsl extension first */
	snprintf(buffer, sizeof(buffer), "%s.xnsl", NETCONFIG);
	if ((file = fopen(buffer, "r")) == NULL) {
		snprintf(buffer, sizeof(buffer), "%s", NETCONFIG);
		if ((file = fopen(buffer, "r")) == NULL)
			goto openfail;
	}

	/* read file one line at a time */
	for (linenum = 1; (line = fgets(buffer, sizeof(buffer), file)) != NULL; linenum++) {
		char *str, *field = NULL, *tmp = NULL;
		int fieldnum;

		/* allocate one if we don't already have one */
		if (nc == NULL) {
			if ((nc = malloc(sizeof(*nc))) == NULL)
				goto nomem;
			memset(nc, 0, sizeof(*nc));
		}
		if ((nc_line_set(nc, strdup(line))) == 0)
			goto nomem;

		for (str = nc_line(nc), fieldnum = 0; fieldnum <= 6
		     && (field = strtok_r(str, " \t\f\n\r\v", &tmp)) != NULL;
		     str = NULL, fieldnum++) {
			switch (fieldnum) {
			case 0:	/* network id field */
				if (field[0] == '#')
					break;
				nc->nc_netid = field;
				continue;
			case 1:	/* semantics field */
				if (strcmp(field, "tpi_clts") == 0) {
					nc->nc_semantics = NC_TPI_CLTS;
				} else if (strcmp(field, "tpi_cots") == 0) {
					nc->nc_semantics = NC_TPI_COTS;
				} else if (strcmp(field, "tpi_cots_ord") == 0) {
					nc->nc_semantics = NC_TPI_COTS_ORD;
				} else if (strcmp(field, "tpi_raw") == 0) {
					nc->nc_semantics = NC_TPI_RAW;
				} else if (strcmp(field, "tpi_cots_pkt") == 0) {
					nc->nc_semantics = NC_TPI_COTS_PKT;
				} else {
					tsd->ncerror = NC_BADLINE;
					tsd->fieldnum = fieldnum;
					tsd->linenum = linenum;
					break;
				}
				continue;
			case 2:	/* flag field */
				nc->nc_flag = 0;
				if (strcmp(field, "-") == 0)
					continue;
				if (strspn(field, "vbm") != strlen(field)) {
					tsd->ncerror = NC_BADLINE;
					tsd->fieldnum = fieldnum;
					tsd->linenum = linenum;
					break;
				}
				if (strchr(field, 'v'))
					nc->nc_flag |= NC_VISIBLE;
				if (strchr(field, 'b'))
					nc->nc_flag |= NC_BROADCAST;
				if (strchr(field, 'm'))
					nc->nc_flag |= NC_MULTICAST;
				continue;
			case 3:	/* protofamily field */
				if (strcmp(field, "-") == 0)
					continue;
				nc->nc_protofmly = field;
				continue;
			case 4:	/* protocol field */
				if (strcmp(field, "-") == 0)
					continue;
				nc->nc_proto = field;
				continue;
			case 5:	/* device field */
				nc->nc_device = field;
				continue;
			case 6:	/* nametoaddrlibs field */
				if (strcmp(field, "-") == 0)
					continue;
				{
					int i;
					char *lib;
					char *list;
					char *tmp2 = NULL;

					/* count them */
					for (i = 1, lib = field; (lib = strpbrk(lib, ","));
					     i++, lib++) ;
					if ((nc->nc_lookups = calloc(i, sizeof(char *))) == NULL) {
						tsd->ncerror = NC_NOMEM;
						break;
					}
					nc->nc_nlookups = i;

					for (list = field, i = 0; i < 16 &&
					     (lib = strtok_r(list, ",", &tmp2)); list = NULL, i++)
						nc->nc_lookups[i] = lib;
				}
				continue;
			}
			break;
		}
		if (fieldnum == 0) {
			if (field == NULL || field[0] == '#') {
				free(nc_line(nc));
				nc_line_set(nc, NULL);
				continue;	/* skip blank or comment lines */
			}
		}
		if (fieldnum != 6)
			goto error;
		/* good entry - add it to the list */
		*nclistp = nc;
		nclistp = nc_nextp(nc);
		nc = NULL;
		entries++;
	}
	{
		struct netconfig **ncp;
		int i;

		/* create master array */
		if ((ncp = calloc(entries + 1, sizeof(struct netconfig *))) == NULL) {
			tsd->ncerror = NC_NOMEM;
			goto error;
		}
		for (nc = nclist, i = 0; i < entries; i++) {
			ncp[i] = nc;
			nc = nc_next(nc);
		}
		ncp[entries] = NULL;

		__nsl_nc_list = ncp;
		return (ncp);
	}
      nomem:
	tsd->ncerror = NC_NOMEM;
	goto error;
      openfail:
	tsd->ncerror = NC_OPENFAIL;
	goto error;
      error:
	if (nc != NULL) {
		*nclistp = nc;
		nclistp = nc_nextp(nc);
		nc = NULL;
	}
	while ((nc = nclist)) {
		nclist = nc_next(nc);
		if (nc_line(nc) != NULL)
			free(nc_line(nc));
		if (nc->nc_lookups != NULL)
			free(nc->nc_lookups);
		free(nc);
	}
	if (file != NULL)
		fclose(file);
	return (NULL);
}

static struct netconfig **
__nsl_getnetconfiglist(void)
{
	struct netconfig **ncp;

	pthread_rwlock_rdlock(&__nsl_nc_lock);
	if ((ncp = __nsl_nc_list) != NULL) {
		pthread_rwlock_unlock(&__nsl_nc_lock);
		return (ncp);
	}
	pthread_rwlock_unlock(&__nsl_nc_lock);
	pthread_rwlock_wrlock(&__nsl_nc_lock);
	if ((ncp = __nsl_nc_list) != NULL) {
		pthread_rwlock_unlock(&__nsl_nc_lock);
		return (ncp);
	}
	ncp = __nsl_loadnetconfiglist();
	pthread_rwlock_unlock(&__nsl_nc_lock);
	return (ncp);
}

/** @brief Provide a handle to the netconfig database.
  * @version XNSL_1.0
  * @par Alias:
  * This function is an implementation of setnetconfig().
  *
  * This function hase the effect of binding to or rewinding the netconfig
  * database.  This function must be called before the first call to
  * getnetconfig() and may be called at any other time.  This function need not
  * be called before a call to getnetconfigent().  This function returns a
  * unique handle to be used by getnetconfig().
  *
  * This function returns a pointer to the current entry in the netconfig
  * database, formatted as a struct netconfig.  This function returns NULL at
  * the end of the file, or upon failure.
  *
  */
void *
__nsl_setnetconfig(void)
{
	struct netconfig **ncp;
	struct netconf_handle *nh = NULL;

	if ((ncp = __nsl_getnetconfiglist()) == NULL)
		return (NULL);
	if ((nh = malloc(sizeof(*nh))) == NULL) {
		nc_error = NC_NOMEM;
		return (NULL);
	}
	pthread_rwlock_init(&nh->nh_lock, NULL);
	nh->nh_head = ncp;
	nh->nh_curr = ncp;
	nc_error = NC_NOERROR;
	return ((void *) nh);
}

/** @fn void *setnetconfig(void)
  * @version XNSL_1.0
  * @par Alias:
  * This symbol is a weak alias of __nsl_setnetconfig().  */
#pragma weak setnetconfig
__asm__(".symver __nsl_setnetconfig,setnetconfig@@XNSL_1.0");

/** @brief Retrieve next entry in the netconfig database.
  * @param handle handle returned by setnetconfig().
  * @version XNSL_1.0
  * @par Alias:
  * This function is an implementation of getnetconfig().
  *
  * This function returns a pointer to the current entry in the netconfig
  * database, formatted as a struct netconfig.  Successive calls will return
  * successive netconfig entries in the netconfig database.  This function can
  * be used to search the entire netconfig file.  This function returns NULL at
  * the end of the file.  handle is a the handle obtained through
  * setnetconfig().
  */
struct netconfig *
__nsl_getnetconfig(void *handle)
{
	struct netconf_handle *nh = (struct netconf_handle *) handle;
	struct netconfig *nc;

	pthread_rwlock_rdlock(&__nsl_nc_lock);
	if (__nsl_nc_list == NULL || nh == NULL) {
		nc_error = NC_NOSET;
		pthread_rwlock_unlock(&__nsl_nc_lock);
		return (NULL);
	}
	pthread_rwlock_unlock(&__nsl_nc_lock);
	pthread_rwlock_wrlock(&nh->nh_lock);
	if (!(nc = *(nh->nh_curr))) {
		nc_error = NC_NOMOREENTRIES;
		pthread_rwlock_unlock(&nh->nh_lock);
		return (NULL);
	}
	nh->nh_curr++;
	nc_error = NC_NOERROR;
	pthread_rwlock_unlock(&nh->nh_lock);
	return (nc);
}

/** @fn struct netconfig *getnetconfig(void *handle)
  * @param handle handle returned by setnetconfig().
  * @version XNSL_1.0
  * @par Alias:
  * This symbol is a weak alias of __nsl_getnetconfig().  */
#pragma weak getnetconfig
__asm__(".symver __nsl_getnetconfig,getnetconfig@@XNSL_1.0");

/** @brief Release network configuration database.
  * @param handle handle returned by setnetconfig().
  * @version XNSL_1.0
  * @par Alias:
  * This function is an implementation of endnetconfig().
  *
  * This function should be called when processing is complete to release
  * resources held for reuse.  handle is the handle obtained through
  * setnetconfig().  Programmers should be aware, however, that the last call to
  * endnetconfig() frees all memory alocated by getnetconfig() for the struct
  * netconfig data structure.  This function may not be called before
  * setnetconfig().
  *
  * This function returns a unique handle to be used by getnetconfig().  IN the
  * case of an error, this function returns NULL and nc_perror() or nc_sperror()
  * can be used to print the reason for failure.
  */
int
__nsl_endnetconfig(void *handle)
{
	struct netconf_handle *nh = (struct netconf_handle *) handle;

	pthread_rwlock_rdlock(&__nsl_nc_lock);
	if (__nsl_nc_list == NULL || nh == NULL) {
		nc_error = NC_NOSET;
		pthread_rwlock_unlock(&__nsl_nc_lock);
		return (-1);
	}
	pthread_rwlock_unlock(&__nsl_nc_lock);
	/* strange thing to do, no? */
	pthread_rwlock_wrlock(&nh->nh_lock);
	free(nh);
	nc_error = NC_NOERROR;
	return (0);
}

/** @fn int endnetconfig(void *handle)
  * @param handle handle returned by setnetconfig().
  * @version XNSL_1.0
  * @par Alias:
  * This symbol is a weak alias of __nsl_endnetconfig().  */
#pragma weak endnetconfig
__asm__(".symver __nsl_endnetconfig,endnetconfig@@XNSL_1.0");

/** @brief Return a network configuration entry for a network id.
  * @param netid the network id.
  * @version XNSL_1.0
  * @par Alias:
  * This function is an implementation of getnetconfigent().
  *
  * This function returns a pointer to the struct netconfig structure
  * corresponding to the argument netid.  It returns NULL if netid is invalid
  * (that is, does not name an entry in the netconfig database).
  *
  * This function returns 0 on success and -1 on failure (for example, if
  * setnetconfig() was not called previously).  */
struct netconfig *
__nsl_getnetconfigent(const char *netid)
{
	struct netconfig **ncp, *nc = NULL;

	if ((ncp = __nsl_getnetconfiglist())) {
		nc_error = NC_NOTFOUND;
		for (; *ncp; ++ncp) {
			if (strcmp((*ncp)->nc_netid, netid) == 0) {
				if ((nc = malloc(sizeof(*nc))) == NULL) {
					nc_error = NC_NOMEM;
					break;
				}
				memcpy(nc, (*ncp), sizeof(*nc));
				if ((nc_line_set(nc, strdup(nc_line(*ncp)))) == NULL) {
					free(nc);
					nc_error = NC_NOMEM;
					break;
				}
				if ((*ncp)->nc_nlookups > 0) {
					int i;

					nc->nc_nlookups = (*ncp)->nc_nlookups;
					nc->nc_lookups =
					    calloc((*ncp)->nc_nlookups, sizeof(char *));
					if (nc->nc_lookups == NULL) {
						free(nc_line(nc));
						free(nc);
						nc_error = NC_NOMEM;
						break;
					}
					memcpy(nc->nc_lookups, (*ncp)->nc_lookups,
					       (*ncp)->nc_nlookups * sizeof(char *));
					/* reindex everything into the new line */
					nc->nc_netid =
					    (*ncp)->nc_netid - nc_line(*ncp) + nc_line(nc);
					nc->nc_protofmly =
					    (*ncp)->nc_protofmly - nc_line(*ncp) + nc_line(nc);
					nc->nc_proto =
					    (*ncp)->nc_proto - nc_line(*ncp) + nc_line(nc);
					nc->nc_device =
					    (*ncp)->nc_device - nc_line(*ncp) + nc_line(nc);
					for (i = 0; i < (*ncp)->nc_nlookups; i++)
						nc->nc_lookups[i] =
						    (*ncp)->nc_lookups[i] - nc_line(*ncp) +
						    nc_line(nc);
				}
				nc_error = NC_NOERROR;
				break;
			}
		}
	}
	return (nc);
}

/** @fn struct netconfig *getnetconfigent(const char *netid)
  * @param netid the network id.
  * @version XNSL_1.0
  * @par Alias:
  * This symbol is a weak alias of __nsl_getnetconfigent().  */
#pragma weak getnetconfigent
__asm__(".symver __nsl_getnetconfigent,getnetconfigent@@XNSL_1.0");

/** @brief Free a netconfig database entry.
  * @param nc the database entry to free.
  * @version XNSL_1.0
  * @par Alias:
  * This function is an implementation of freenetconfigent().
  *
  * This function frees the netconfig structure pointed to by nc (previously
  * returned by getneconfigent()).  */
void
__nsl_freenetconfigent(struct netconfig *nc)
{
	if (nc) {
		if (nc_line(nc))
			free(nc_line(nc));
		if (nc->nc_lookups)
			free(nc->nc_lookups);
		free(nc);
	}
}

/** @fn void freenetconfigent(struct netconfig *nc)
  * @param nc the database entry to free.
  * @version XNSL_1.0
  * @par Alias:
  * This symbol is a weak alias of __nsl_freenetconfigent().  */
#pragma weak freenetconfigent
__asm__(".symver __nsl_freenetconfigent,freenetconfigent@@XNSL_1.0");

/* *INDENT-OFF* */
static const char *__nsl_nc_errlist[] = {
/*
TRANS Corresponds to the NC_NOERROR constant.  No error is indicated in the
TRANS nc_error(3) variable.  The last netconfig(3) or netpath(3) operation was
TRANS successful.
 */
	[NC_NOERROR] = gettext_noop("no error"),
/*
TRANS Corresponds to the NC_NOMEM constant.  Out of memory.  Memory could not be
TRANS allocated to complete the last netconfig(3) or netpath(3) operation.
 */
	[NC_NOMEM] = gettext_noop("out of memory"),
/*
TRANS Corresponds to the NC_NOSET constant.  The getnetconfig(3), getnetpath(3),
TRANS endnetconfig(3), or endnetpath(3) function was called out of order: that
TRANS is, before setnetconfig(3) or setnetpath(3) was called.
 */
	[NC_NOSET] = gettext_noop("routine called out of order"),
/*
TRANS Corresponds to the NC_OPENFAIL constant.  The /etc/netconfig file could
TRANS not be opened for reading.  %1$s is the name of the /etc/netconfig file.
 */
	[NC_OPENFAIL] = gettext_noop("could not open %s"),
/*
TRANS Corresponds to the NC_BADLINE constant.  A syntax error exists in the
TRANS /etc/netconfig file at the location shown.  The field number is the
TRANS whitespace separated field within the line in the /etc/netconfig file at
TRANS which the syntax error was detected.  The line number is the line number
TRANS in the /etc/netconfig file at which the syntax error was detected.  %1$s
TRANS is the name of the /etc/netconfig file.  %2$d is the field number.  %3$d
TRANS is the line number.
 */
	[NC_BADLINE] = gettext_noop("syntax error in %s: field %d of line %d"),
/*
TRANS Corresponds to the NC_NOTFOUND constant.  The "netid" specified as an
TRANS argument to the getnetconfigent(3) subroutine was not found in any entry
TRANS in the /etc/netconfig file.  %1$s is the name of the /etc/netconfig file.
 */
	[NC_NOTFOUND] = gettext_noop("netid not found in %s"),
/*
TRANS Corresponds to the NC_NOMOREENTRIES constant.  All entries in the
TRANS /etc/netconfig file has been exhausted.  They have either already been
TRANS retrieved with getnetconfig(3), getnetconfigent(3), getnetpath(3), or the
TRANS /etc/netconfig file contains no entries.  %1$s is the name of the
TRANS /etc/netconfig file.
 */
	[NC_NOMOREENTRIES] = gettext_noop("no more entries in %s"),
/*
TRANS Corresponds to any other constant.  An internal error in the libxnsl(3)
TRANS library has occurred resulting in an undefined error number.  This
TRANS situation should not occur and represents a bug in the library.  %1$d is
TRANS the decimal value of the unknown error number.
 */
	[NC_ERROR_MAX] = gettext_noop("unknown error number %d"),
};
/* *INDENT-ON* */

/** @brief Return an error string.
  * @version XNSL_1.0
  * @par Alias:
  * This function is an implementation of nc_sperror().
  *
  * This function is similar to nc_perror() but instead of printing the message
  * to standard error, will return a pointer to a string that contains the error
  * message.
  *
  * This function can also be used with the netpath access routines.
  *
  * This function returns a pointer to a buffer that contains the error messages
  * tirng.  This buffer is overwritten on each call.  In multithreaded
  * applications, this buffer is implemented as thread-specific data.
  *
  */
char *
__nsl_nc_sperror(void)
{
	int err, idx;
	struct __nsl_tsd *tsd = __nsl_get_tsd();
	char *errbuf = tsd->ncerrbuf;

	if (errbuf != NULL) {
		switch ((idx = err = tsd->ncerror)) {
		case NC_NOERROR:
		case NC_NOMEM:
		case NC_NOSET:
			(void) strncpy(errbuf, gettext(__nsl_nc_errlist[idx]), NCERR_BUFSZ);
			break;
		case NC_OPENFAIL:
		case NC_NOTFOUND:
		case NC_NOMOREENTRIES:
			(void) snprintf(errbuf, NCERR_BUFSZ, gettext(__nsl_nc_errlist[idx]), NETCONFIG);
			break;
		case NC_BADLINE:
			(void) snprintf(errbuf, NCERR_BUFSZ, gettext(__nsl_nc_errlist[idx]), NETCONFIG,
					tsd->fieldnum, tsd->linenum);
			break;
		default:
			idx = NC_ERROR_MAX;
			(void) snprintf(errbuf, NCERR_BUFSZ, gettext(__nsl_nc_errlist[idx]), err);
			break;
		}
	}
	return (errbuf);
}

/** @fn char *nc_sperror(void)
  * @version XNSL_1.0
  * @par Alias:
  * This symbol is a weak alias of __nsl_nc_sperror().  */
#pragma weak nc_sperror
__asm__(".symver __nsl_nc_sperror,nc_sperror@@XNSL_1.0");

/** @brief Print an error message to standard error.
  * @param msg message to prefix to error message.
  * @version XNSL_1.0
  * @par Alias:
  * This function is an implementation of nc_perror().
  *
  * This function prints and error message to standard error indicating why any
  * of the above routines failed.  The message is prepended with the string
  * provided in the msg argument and a colon.  A NEWLINE is appended to the end
  * of the message.
  *
  * This function can also be used with the netpath access routines.
  */
void
__nsl_nc_perror(const char *msg)
{
	if (msg)
		fprintf(stderr, "%s: %s\n", msg, __nsl_nc_sperror());
	else
		fprintf(stderr, "%s\n", __nsl_nc_sperror());
	return;
}

/** @fn void nc_perror(const char *msg)
  * @param msg message to prefix to error message.
  * @version XNSL_1.0
  * @par Alias:
  * This symbol is a weak alias of __nsl_nc_perror().  */
#pragma weak nc_perror
__asm__(".symver __nsl_nc_perror,nc_perror@@XNSL_1.0");

/** @brief Get a handle for network configuration database.
  * @version XNSL_1.0
  * @par Alias:
  * This function is an implementation of setnetpath().
  *
  * A call to this function binds to or rewinds NETPATH.  This function must be
  * called before the first call to getnetpath() and may be called at any other
  * time.  It returns a handle that is used by getnetpath().
  *
  * This function returns a handle that is used by getnetpath().  In case of an
  * error, this function returns NULL.  nc_perror() or nc_sperror() can be used
  * to print out the reason for failure.  See getnetconfig().
  */
void *
__nsl_setnetpath(void)
{
	struct netconfig **ncp, **ncplist, **ncpath = NULL, **ncpath_new;
	struct netconf_handle *nh = NULL;
	char *path;
	int entries = 0;

	if ((ncplist = __nsl_getnetconfiglist()) == NULL)
		return (NULL);
	if ((path = getenv(NETPATH)) == NULL) {
		for (ncp = ncplist; *ncp; ncp++) {
			if ((*ncp)->nc_flag & NC_VISIBLE) {
				if (!
				    (ncpath_new = realloc(ncpath, (entries + 1) * sizeof(*ncpath))))
					goto nomem;
				ncpath = ncpath_new;
				ncpath[entries++] = *ncp;
			}
		}
	} else {
		char *str, *token, *tmp = NULL;

		if ((path = strdup(path)) == NULL)
			goto nomem;
		for (str = path; (token = strtok_r(str, ":", &tmp)); str = NULL) {
			for (ncp = ncplist; *ncp; ncp++) {
				if (strcmp((*ncp)->nc_netid, token) == 0) {
					if (!(ncpath_new =
					      realloc(ncpath, (entries + 1) * sizeof(*ncpath))))
						goto nomem;
					ncpath = ncpath_new;
					ncpath[entries++] = *ncp;
					break;
				}
			}
		}
	}
	if (entries == 0)
		goto notfound;
	if (!(ncpath_new = realloc(ncpath, (entries + 1) * sizeof(*ncpath))))
		goto nomem;
	ncpath = ncpath_new;
	ncpath[entries++] = NULL;
	if ((nh = malloc(sizeof(*nh))) == NULL)
		goto nomem;
	memset(nh, 0, sizeof(*nh));
	pthread_rwlock_init(&nh->nh_lock, NULL);
	nh->nh_head = ncpath;
	nh->nh_curr = ncpath;
	nc_error = NC_NOERROR;
	return ((void *) nh);

      notfound:
	nc_error = NC_NOTFOUND;
	goto error;
      nomem:
	nc_error = NC_NOMEM;
	goto error;
      error:
	if (path)
		free(path);
	if (ncpath)
		free(ncpath);
	if (nh)
		free(nh);
	return (NULL);

}

/** @fn void *setnetpath(void)
  * @version XNSL_1.0
  * @par Alias:
  * This symbol is a weak alias of __nsl_setnetpath().  */
#pragma weak setnetpath
__asm__(".symver __nsl_setnetpath,setnetpath@@XNSL_1.0");

/** @brief Get the next entry associated with handle.
  * @param handle a pointer to the handle returned by setnetpath().
  * @version XNSL_1.0
  * @par Alias:
  * This function is an implementation of getnetpath().
  *
  * This function returns a pointer to the netconfig database entry
  * corresponding to the first valid #NETPATH component.  The netconfig entry is
  * formattted as a struct netconfig.  On each subsequenc all, this function
  * returns a pointer to the netconfig entry that corresponds to the next valud
  * #NETPATH component.  This function can thus be used to search the netconfig
  * database for all networks included in the #NETPATH variable.  When #NETPATH
  * has been exhausted, this function returns NULL.
  *
  * This function silently ignores invalid #NETPATH components.  A #NETPATH
  * component is invalid if there is no corresponding entry in the netconfig
  * database.
  *
  * If the #NETPATH environment variable is unset, this function bethaves as if
  * #NETPATH were set to the sequence of "default" or "visible" networks in the
  * netconfig database, in the order in which they are listed.
  *
  * When first called, this function returns a pointer to the netconfig database
  * entry corresponding to the first valid #NETPATH component.  When #NETPATH
  * has been exhausted, it returns NULL.  */
struct netconfig *
__nsl_getnetpath(void *handle)
{
	return __nsl_getnetconfig(handle);
}

/** @fn struct netconfig *getnetpath(void *handle)
  * @param handle a pointer to the handle returned by setnetpath().
  * @version XNSL_1.0
  * @par Alias:
  * This symbol is a weak alias of __nsl_getnetpath().  */
#pragma weak getnetpath
__asm__(".symver __nsl_getnetpath,getnetpath@@XNSL_1.0");

/** @brief Free netpath resources associated with handle.
  * @param handle a pointer to the handle returned by setnetpath().
  * @version XNSL_1.0
  * @par Alias:
  * This function is an implementation of endnetpath().
  *
  * This function may be called to unbind from NETPATH when processing is
  * complete, releasing resources for reuse.  Programmers should be aware,
  * however, that this function frees all memory allocated by getnetpath() for
  * the struct netconfig data structure.
  *
  * This function returns 0 on success and -1 on failure (for example, if
  * setnetpath() was not called previously).
  *
  */
int
__nsl_endnetpath(void *handle)
{
	struct netconf_handle *nh = (struct netconf_handle *) handle;

	pthread_rwlock_rdlock(&__nsl_nc_lock);
	if (__nsl_nc_list == NULL || nh == NULL) {
		nc_error = NC_NOSET;
		pthread_rwlock_unlock(&__nsl_nc_lock);
		return (-1);
	}
	pthread_rwlock_unlock(&__nsl_nc_lock);
	/* strange thing to do, no? */
	pthread_rwlock_wrlock(&nh->nh_lock);
	free(nh->nh_head);
	free(nh);
	nc_error = NC_NOERROR;
	return (0);
}

/** @fn int endnetpath(void *handle)
  * @param handle a pointer to the handle returned by setnetpath().
  * @version XNSL_1.0
  * @par Alias:
  * This symbol is a weak alias of __nsl_endnetpath().  */
#pragma weak endnetpath
__asm__(".symver __nsl_endnetpath,endnetpath@@XNSL_1.0");

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
