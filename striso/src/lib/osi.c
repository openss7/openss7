/*****************************************************************************

 @(#) $RCSfile: osi.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-08-20 10:56:48 $

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

 Last Modified $Date: 2008-08-20 10:56:48 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: osi.c,v $
 Revision 0.9.2.4  2008-08-20 10:56:48  brian
 - fixes and build updates from newnet trip

 Revision 0.9.2.3  2008-04-29 00:02:01  brian
 - updated headers for release

 Revision 0.9.2.2  2007/08/14 07:05:12  brian
 - GNUv3 header update

 Revision 0.9.2.1  2006/10/10 10:12:44  brian
 - added losts of striso files

 *****************************************************************************/

#ident "@(#) $RCSfile: osi.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-08-20 10:56:48 $"

static char const ident[] =
    "$RCSfile: osi.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-08-20 10:56:48 $";

/* This file can be processed with doxygen(1). */

/** @weakgroup osi OSI Address Translation Library
  * @{ */

/** @file
  * OSI Address Translation Library implementation file.
  *
  * This is libosi a compatibility library providing functions that are
  * compatible with Sun's SunLink OSI 8.1 product.  These function are
  * implemented from scratch.
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
#include <xti_osi.h>
#include <osi.h>

#include "osilib.h"

/*
 *  Once condition for Thread-Specific Data key creation.
 */
static pthread_once_t __osi_tsd_once = PTHREAD_ONCE_INIT;

/*
 *  XNSL library Thread-Specific Data key.
 */
static pthread_key_t __osi_tsd_key = 0;

static void
__osi_tsd_free(void *buf)
{
	pthread_setspecific(__osi_tsd_key, NULL);
	free(buf);
}

static void
__osi_tsd_key_create(void)
{
	pthread_key_create(&__osi_tsd_key, __osi_tsd_free);
}

static struct __osi_tsd *
__osi_tsd_alloc(void)
{
	struct __osi_tsd *tsdp;

	tsdp = (typeof(tsdp)) malloc(sizeof(*tsdp));
	memset(tsdp, 0, sizeof(*tsdp));
	pthread_setspecific(__osi_tsd_key, (void *) tsdp);
	return (tsdp);
}

/** @internal
  * @brief Get thread specific data for the osi library.
  *
  * This function obtains (and allocates if necessary), thread specific data
  * for the executing thread.  */
static struct __osi_tsd *
__osi_get_tsd(void)
{
	struct __osi_tsd *tsdp;

	pthread_once(&__osi_tsd_once, __osi_tsd_key_create);
	if (unlikely((tsdp = (typeof(tsdp)) pthread_getspecific(__osi_tsd_key)) == NULL))
		tsdp = __osi_tsd_alloc();
	return (tsdp);
};

/*
 *  OSI Library functions:
 *  ======================
 */

static pthread_rwlock_t __osi_host_lock = PTHREAD_RWLOCK_INITIALIZER;
static pthread_rwlock_t __osi_serv_lock = PTHREAD_RWLOCK_INITIALIZER;

/*
 *  Ok, what needs to be done here to acquire information from the database is
 *  to lock the database lock, open a file descriptor, read all of the entries
 *  into data structures, close the database and unlock the database.  This has
 *  to be done once for the /etc/net/oclt/hosts database and once for the
 *  /etc/net/oclt/services database, but only on demand.
 */

struct __osi_hostlist {
	struct __osi_hostlist *hl_next;	/* next entry */
	char *hl_line;			/* saved copy of the file line */
	char *hl_hostname;		/* hostname (null-terminated) */
	char *hl_nsapstr;		/* original NSAP string (null-terminated) */
	char hl_nsap[20];		/* normalized NSAP */
};

static struct __osi_hostlist *__osi_hostlist = NULL;

int __osi_nsap2net(int net_len, char *net, char *nsap);

static struct __osi_hostlist *
__osi_gethostlist(void)
{
	char buffer[1024];
	char *line;
	int linenum, entries = 0, err = 0;
	FILE *file = NULL;
	struct __osi_hostlist *hl = NULL, **hlp = &__osi_hostlist;

	if ((file = fopen(OSIHOSTS, "r")) == NULL)
		goto openfail;

	/* read file one line at a time */
	for (linenum = 1; (line = fgets(buffer, sizeof(buffer), file)) != NULL; linenum++) {
		char *str, *field, *tmp = NULL;
		int fieldnum;

		/* allocate one if we do not already have one */
		if (hl == NULL) {
			if ((hl = malloc(sizeof(*hl))) == NULL)
				goto nomem;
			memset(hl, 0, sizeof(*hl));
		}
		if ((hl_line_set(hl, strdup(line))) == 0)
			goto nomem;

		for (str = hl->hl_line, fieldnum = 0; fieldnum <= 3
		     && (field = strtok_r(str, " \t\f\n\r\v", &tmp)) != NULL;
		     str = NULL, fieldnum++) {
			switch (fieldnum) {
			case 0:	/* host name field */
				if (field[0] == '#')
					break;
				hl->hl_hostname = field;
				continue;
			case 1:	/* NSAP */
				if (__osi_nsap2net(sizeof(hl->hl_nsap), (char *) hl->hl_nsap, field)
				    == -1) {
					err = EINVAL;
					break;
				}
				hl->hl_nsapstr = field;
				continue;
			case 2:	/* possible trailing comment */
				if (field[0] == '#')
					break;
				err = EINVAL;
				break;
			}
			break;
		}
		if (fieldnum == 0) {
			if (field == NULL || field[0] == '#') {
				free(hl->hl_line);
				hl_line_set(hl, NULL);
				continue;	/* skip blank or comment lines */
			}
		}
		if (fieldnum != 1 && (fieldnum != 2 || field[0] != '#'))
			goto error;
		/* good entry - add it to the list */
		*hlp = hl;
		hlp = &hl->hl_next;
		hl = NULL;
		entries++;
	}
	fclose(file);
	return (*hlp);
      nomem:
	err = ENOMEM;
	goto error;
      openfail:
	err = errno;
	goto error;
      error:
	if (hl != NULL) {
		*hlp = hl;
		hlp = &hl->hl_next;
		hl = NULL;
	}
	while ((hl = *hlp)) {
		hlp = &hl->hl_next;
		if (hl->hl_line != NULL)
			free(hl->hl_line);
		free(hl);
	}
	if (file != NULL)
		fclose(file);
	errno = err;
	return (NULL);
}

struct __osi_servlist {
	struct __osi_servlist *sl_next;	/* next entry */
	char *sl_line;			/* saved copy of the file line */
	char *sl_servname;		/* servname (null-terminated) */
	char *sl_tselstr;		/* original TSEL string (null-terminated) */
	char sl_tsel[32];		/* normalized NSAP */
};

static struct __osi_servlist *__osi_servlist = NULL;

static int __osi_tsel2net(int net_len, char *net, char *tsel);

static struct __osi_servlist *
__osi_getservlist(void)
{
	char buffer[1024];
	char *line;
	int linenum, entries = 0, err = 0;
	FILE *file = NULL;
	struct __osi_servlist *sl = NULL, **slp = &__osi_servlist;

	if ((file = fopen(OSIHOSTS, "r")) == NULL)
		goto openfail;

	/* read file one line at a time */
	for (linenum = 1; (line = fgets(buffer, sizeof(buffer), file)) != NULL; linenum++) {
		char *str, *field, *tmp = NULL;
		int fieldnum;

		/* allocate one if we do not already have one */
		if (sl == NULL) {
			if ((sl = malloc(sizeof(*sl))) == NULL)
				goto nomem;
			memset(sl, 0, sizeof(*sl));
		}
		if ((sl_line_set(sl, strdup(line))) == 0)
			goto nomem;

		for (str = sl->sl_line, fieldnum = 0; fieldnum <= 3
		     && (field = strtok_r(str, " \t\f\n\r\v", &tmp)) != NULL;
		     str = NULL, fieldnum++) {
			switch (fieldnum) {
			case 0:	/* serv name field */
				if (field[0] == '#')
					break;
				sl->sl_servname = field;
				continue;
			case 1:	/* TSEL */
				if (__osi_tsel2net(sizeof(sl->sl_tsel), (char *) sl->sl_tsel, field)
				    == -1) {
					err = EINVAL;
					break;
				}
				sl->sl_tselstr = field;
				continue;
			case 2:	/* possible trailing comment */
				if (field[0] == '#')
					break;
				err = EINVAL;
				break;
			}
			break;
		}
		if (fieldnum == 0) {
			if (field == NULL || field[0] == '#') {
				free(sl->sl_line);
				sl_line_set(sl, NULL);
				continue;	/* skip blank or comment lines */
			}
		}
		if (fieldnum != 1 && (fieldnum != 2 || field[0] != '#'))
			goto error;
		/* good entry - add it to the list */
		*slp = sl;
		slp = &sl->sl_next;
		sl = NULL;
		entries++;
	}
	fclose(file);
	return (*slp);
      nomem:
	err = ENOMEM;
	goto error;
      openfail:
	err = errno;
	goto error;
      error:
	if (sl != NULL) {
		*slp = sl;
		slp = &sl->sl_next;
		sl = NULL;
	}
	while ((sl = *slp)) {
		slp = &sl->sl_next;
		if (sl->sl_line != NULL)
			free(sl->sl_line);
		free(sl);
	}
	if (file != NULL)
		fclose(file);
	errno = err;
	return (NULL);
}

/** @brief Get the CLNS NSAP for the local host.
  * @param nsap_maxlen the length of the NSAP buffer.
  * @param nsap the NSAP buffer.
  * @version OSI_1.0
  * @par Alias:
  * This symbol is an implementation of getmyclnsnsap().
  *
  * This function returns the local host NSAP for CLNS service into the buffer
  * pointed to by nsap of maximum length nsap_maxlength.  The resulting NSAP is
  * suitable for use by nsap2net() and tsap2net().
  *
  * This function works by searching the /etc/net/oclt/hosts file for the host
  * name "clns" and basically returns whatever NSAP string is placed
  * against that entry.  For this to work, the local CLNS NSAP must be placed in
  * the /etc/net/oclt/hosts file against hostname "clns".
  *
  * On success, this function returns the size of the NSAP that was placed into
  * the buffer.  On failure, it returns -1 and sets errno appropriately.
  */
int
__osi_getmyclnsnsap(int nsap_maxlen, char *nsap)
{
	static const char localhost[] = "cons";

	return __osi_getnsapbyname(nsap_maxlen, localhost, nsap);
}

/** @fn int getmyclnsnsap(int nsap_maxlen, char *nsap)
  * @param nsap_maxlen the length of the NSAP buffer.
  * @param nsap the NSAP buffer.
  * @version OSI_1.0
  * @par Alias:
  * This symbol is a weak alias of __osi_getmyclnsnsap(). */
#pragma weak getmyclnsnsap
__asm__(".symver __osi_getmyclnsnsap,getmyclnsnsap@@OSI_1.0");

/** @brief Get the CONS NSAP for the local host.
  * @param nsap_maxlen the length of the NSAP buffer.
  * @param nsap the NSAP buffer.
  * @version OSI_1.0
  * @par Alias:
  * This symbol is an implementation of getmyconsnsap().
  *
  * This function returns the local host NSAP for CONS service into the buffer
  * pointed to by nsap of maximum length nsap_maxlength.  The resulting NSAP is
  * suitable for use by nsap2net() and tsap2net().
  *
  * This function works by searching the /etc/net/oclt/hosts file for the host
  * name "cons" and basically returns whatever NSAP string is placed
  * against that entry.  For this to work, the local CONS NSAP must be placed in
  * the /etc/net/oclt/hosts file against hostname "cons".
  *
  * On success, this function returns the size of the NSAP that was placed into
  * the buffer.  On failure, it returns -1 and sets errno appropriately.
  */
int
__osi_getmyconsnsap(int nsap_maxlen, char *nsap)
{
	static const char localhost[] = "cons";

	return __osi_getnsapbyname(nsap_maxlen, localhost, nsap);
}

/** @fn int getmyconsnsap(int nsap_maxlen, char *nsap)
  * @param nsap_maxlen the length of the NSAP buffer.
  * @param nsap the NSAP buffer.
  * @version OSI_1.0
  * @par Alias:
  * This symbol is a weak alias of __osi_getmyconsnsap(). */
#pragma weak getmyconsnsap
__asm__(".symver __osi_getmyconsnsap,getmyconsnsap@@OSI_1.0");

/** @brief Get a host name by NSAP.
  * @param hostname_maxlen the length of the host name.
  * @param hostname the host name buffer.
  * @param nsap the NSAP to convert.
  * @version OSI_1.0
  * @par Alias:
  * This symbol is an implementation of getnamebynsap().
  *
  * This function takes an NSAP and returns a host name into the buffer pointed
  * to by hostname of maximum length hostname_maxlen.  The resulting host name
  * is suitable for use by getnsapbyname() and for display as a host name
  * string.  The host name is a null-terminated character string.
  *
  * This function works by reverse searching the /etc/net/oclt/hosts file and
  * basically returns whatever string occurs in that file along with the NSAP
  * entry.  To do this both the NSAP entry specified and the NSAP entries in the
  * file must be normalized.
  *
  * On success, this function returns the size of the host name that was placed
  * into the buffer.  On failure, it returns -1 and sets errno appropriately.
  */
int
__osi_getnamebynsap(int hostname_maxlen, char *hostname, char *nsap)
{
	errno = ENOSYS;
	return (-1);
}

/** @fn int getnamebynsap(int hostname_maxlen, char *hostname, char *nsap)
  * @param hostname_maxlen the length of the host name.
  * @param hostname the host name buffer.
  * @param nsap the NSAP to convert.
  * @version OSI_1.0
  * @par Alias:
  * This symbol is a weak alias of __osi_getnamebynsap(). */
#pragma weak getnamebynsap
__asm__(".symver __osi_getnamebynsap,getnamebynsap@@OSI_1.0");

/** @brief Get a network service access point (NSAP) by host name.
  * @param nsap_maxlen the length of the NSAP buffer.
  * @param hostname the host name to convert.
  * @param nsap the NSAP buffer.
  * @version OSI_1.0
  * @par Alias:
  * This symbol is an implementation of getnsapbyname().
  *
  * This function takes a host name and formats a network service access point
  * (NSAP) address into the buffer pointed to by nsap of maximum length
  * nsap_maxlen.  The resulting NSAP is suitable for use by nsap2net() or
  * tsap2net().  Note that the NSAP is a null-terminated character string.
  *
  * This function works by searching the /etc/net/oclt/hosts file entries and
  * basically returns whatever string occurs in that file along with the host
  * name entry.
  *
  * On success, this function returns the size of the NSAP that was placed into
  * the buffer.  On failure, it returns -1 and sets errno appropriately.
  */
int
__osi_getnsapbyname(int nsap_maxlen, char *hostname, char *nsap)
{
	errno = ENOSYS;
	return (-1);
}

/** @fn int getnsapbyname(int nsap_maxlen, char *hostname, char *nsap)
  * @param nsap_maxlen the length of the NSAP buffer.
  * @param hostname the host name to convert.
  * @param nsap the NSAP buffer.
  * @version OSI_1.0
  * @par Alias:
  * This symbol is a weak alias of __osi_getnsapbyname(). */
#pragma weak getnsapbyname
__asm__(".symver __osi_getnsapbyname,getnsapbyname@@OSI_1.0");

/** @brief Get a transport selector (TSEL) by service name.
  * @param tsel_maxlen the length of the TSEL buffer.
  * @param name the service name to convert.
  * @param tsel the TSEL buffer.
  * @version OSI_1.0
  * @par Alias:
  * This symbol is an implementation of gettselbyname().
  *
  * This function takes a service name and formats a trasnport selector (TSEL)
  * into the buffer pointed to by tsel of maximum length tsel_maxlen.  The
  * resulting TSEL is suitable for use by tsap2net().  Note that the TSEL is a
  * null-termianted character string.
  *
  * This function works by searching the /etc/net/oclt/services file entries and
  * basically returns whatever string occurs in that file along with the service
  * name entry.
  *
  * On success, this function returns the size of the TSEL that was placed
  * into the buffer.  On failure, it returns -1 and sets errno appropriately.
  */
int
__osi_gettselbyname(int tsel_maxlen, char *name, char *tsel)
{
	errno = ENOSYS;
	return (-1);
}

/** @fn int gettselbyname(int tsel_maxlen, char *name, char *tsel)
  * @param tsel_maxlen the length of the TSEL buffer.
  * @param name the service name to convert.
  * @param tsel the TSEL buffer.
  * @version OSI_1.0
  * @par Alias:
  * This symbol is a weak alias of __osi_gettselbyname(). */
#pragma weak gettselbyname
__asm__(".symver __osi_gettselbyname,gettselbyname@@OSI_1.0");

/** @brief Write an NSAP into a buffer suitable for a netbuf.
  * @param net_len the length of the netbuf buffer.
  * @param net the netbuf buffer.
  * @param nsap the NSAP to place in the buffer.
  * @version OSI_1.0
  * @par Alias:
  * This symbol is an implementation of nsap2net().
  *
  * This function takes the NSAP specified and formats it into a netbuf buffer
  * of size net_len pointed to by net suitable for use as an address with the
  * OSI transport provider.  This is intended to be used with the CLNP
  * pseudo-transport provider (really TPI interface to NPI CLNS).
  *
  * On success, this function returns the length of the NSAP that was written to
  * the buffer.  On failure, it returnes -1 and sets errno appropriately.
  */
int
__osi_nsap2net(int net_len, char *net, char *nsap)
{
	if (net == NULL) {
		errno = EINVAL;
		return (-1);
	}
	if (net_len < 1) {
		errno = EMSGSIZE;
		return (-1);
	}
	if (nsap == NULL) {
		/* Null NSAP -- easy stuff */
		*net = 0;
		return (1);
	}
	/* FIXME: this is harder */
	errno = ENOSYS;
	return (-1);
}

/** @fn int nsap2net(int net_len, char *net, char *nsap)
  * @param net_len the length of the netbuf buffer.
  * @param net the netbuf buffer.
  * @param nsap the NSAP to place in the buffer.
  * @version OSI_1.0
  * @par Alias:
  * This symbol is a weak alias of __osi_nsap2net(). */
#pragma weak nsap2net
__asm__(".symver __osi_nsap2net,nsap2net@@OSI_1.0");

static int
__osi_tsel2net(int net_len, char *net, char *tsel)
{
	if (net == NULL) {
		errno = EINVAL;
		return (-1);
	}
	if (net_len < 1) {
		errno = EMSGSIZE;
		return (-1);
	}
	if (tsel == NULL) {
		/* Null TSAP -- easy stuff */
		*net = 0;
		return (1);
	}
	/* FIXME: this is harder */
	errno = ENOSYS;
	return (-1);
}

/** @brief Write an NSAP and TSEL into a buffer suitable for a netbuf.
  * @param net_len the length of the netbuf buffer.
  * @param net the netbuf buffer.
  * @param nsap the NSAP to place in the buffer.
  * @param tsel the TSEL to place in the buffer.
  * @version OSI_1.0
  * @par Alias:
  * This function is an implementation of tsap2net().
  *
  * This function take the NSAP and TSEL specified and formats them into a
  * netbuf buffer of size net_len pointed to by net suitable for use as an
  * address with the OSI transport provider.  This is intended to be used with
  * TP COTS and CLTS transport providers.
  *
  * On success, this function returns the length of the TSAP that was written
  * to the buffer.  On failure, it returns -1 and sets errno appropriately.
  */
int
__osi_tsap2net(int net_len, char *net, char *nsap, char *tsel)
{
	int tsel_size = 0, nsap_size = 0;

	if ((tsel_size = __osi_tsel2net(net_len, net, tsel)) == -1)
		return (-1);
	net_len -= tsel_size;
	net += tsel_size;
	if ((nsap_size = __osi_nsap2net(net_len, net, nsap)) == -1)
		return (-1);
	return (tsel_size + nsap_size);
}

/** @fn int tsap2net(int net_len, char *net, char *nsap, char *tsel)
  * @param net_len the length of the netbuf buffer.
  * @param net the netbuf buffer.
  * @param nsap the NSAP to place in the buffer.
  * @param tsel the TSEL to place in the buffer.
  * @version OSI_1.0
  * @par Alias:
  * This symbol is a weak alias of __osi_tsap2net(). */
#pragma weak tsap2net
__asm__(".symver __osi_tsap2net,tsap2net@@OSI_1.0");

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
