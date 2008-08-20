/*****************************************************************************

 @(#) $RCSfile: sockpath.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-08-20 10:56:56 $

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

 Last Modified $Date: 2008-08-20 10:56:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sockpath.c,v $
 Revision 0.9.2.6  2008-08-20 10:56:56  brian
 - fixes and build updates from newnet trip

 Revision 0.9.2.5  2008-04-28 22:33:33  brian
 - updated headers for release

 Revision 0.9.2.4  2007/08/14 05:17:23  brian
 - GPLv3 header update

 Revision 0.9.2.3  2006/12/18 08:14:07  brian
 - resolve device numbering

 Revision 0.9.2.2  2006/09/18 13:52:53  brian
 - added doxygen markers to sources

 Revision 0.9.2.1  2006/09/18 00:10:36  brian
 - added libsocket source files and manuals

 *****************************************************************************/

#ident "@(#) $RCSfile: sockpath.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-08-20 10:56:56 $"

static char const ident[] =
    "$RCSfile: sockpath.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-08-20 10:56:56 $";

/* This file can be processed with doxygen(1). */

/*
 *  This is a simple library for accessing the /etc/sock2path file in a manner similar to the
 *  netconfig(3) functions.  It is used both by the soconfig(8) utility as well as the libsocklib
 *  library.
 */

#define _XOPEN_SOURCE 600
#define _REENTRANT
#define _THREAD_SAFE

#if 0
#define __USE_UNIX98
#define __USE_XOPEN2K
#define __USE_GNU
#endif

#include <stdlib.h>
#include "gettext.h"
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
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

#include <sockdb.h>
#include "sockpath.h"

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

/*
 *  Once condition for Thread-Specific Data key creation.
 */
static pthread_once_t __sockpath_tsd_once = PTHREAD_ONCE_INIT;

/*
 *  SOCKPATH library Thread-Specific Data key.
 */
static pthread_key_t __sockpath_tsd_key = 0;

static void
__sockpath_tsd_free(void *buf)
{
	pthread_setspecific(__sockpath_tsd_key, NULL);
	free(buf);
}

static void
__sockpath_tsd_key_create(void)
{
	pthread_key_create(&__sockpath_tsd_key, __sockpath_tsd_free);
}

static struct __sockpath_tsd *
__sockpath_tsd_alloc(void)
{
	struct __sockpath_tsd *tsdp;

	tsdp = (typeof(tsdp)) malloc(sizeof(*tsdp));
	memset(tsdp, 0, sizeof(*tsdp));
	pthread_setspecific(__sockpath_tsd_key, (void *) tsdp);
	return (tsdp);
}

static struct __sockpath_tsd *
__sockpath_get_tsd(void)
{
	struct __sockpath_tsd *tsdp;

	pthread_once(&__sockpath_tsd_once, __sockpath_tsd_key_create);
	if (unlikely((tsdp = (typeof(tsdp)) pthread_getspecific(__sockpath_tsd_key)) == NULL))
		tsdp = __sockpath_tsd_alloc();
	return (tsdp);
}

int *
__sockpath___sperror(void)
{
	return &(__sockpath_get_tsd()->sperror);
}

__asm__(".symver __sockpath___sperror,__sperror@@SOCKPATH_1.0");

/*
 *  Socket Path functions:
 *  ======================
 */

const char *__sockpath_sock2path = SOCK2PATH;

#undef SOCK2PATH
#define SOCK2PATH __sockpath_sock2path

static pthread_rwlock_t __sockpath_sp_lock = PTHREAD_RWLOCK_INITIALIZER;
static struct sockpathent **__sockpath_sp_list = NULL;

static struct sockpathent **
__sockpath_loadsockpathlist(void)
{
	char buffer[1024];
	char *line;
	int linenum, entries = 0;
	FILE *file = NULL;
	struct sockpathent *se = NULL, *selist = NULL, **selistp = &selist;
	struct __sockpath_tsd *tsd = __sockpath_get_tsd();

	if ((file = fopen(SOCK2PATH, "r")) == NULL)
		goto openfail;

	/* read file one line at a time */
	for (linenum = 1; (line = fgets(buffer, sizeof(buffer), file)) != NULL; linenum++) {
		char *str, *field = NULL, *tmp = NULL;
		int fieldnum;

		/* allocate one if we don't already have one */
		if (se == NULL) {
			if ((se = malloc(sizeof(*se))) == NULL)
				goto nomem;
			memset(se, 0, sizeof(*se));
		}
		for (str = line, fieldnum = 0; fieldnum < 4
		     && (field = strtok_r(str, " \t\f\n\r\v", &tmp)) != NULL;
		     str = NULL, fieldnum++) {
			char *end;

			switch (fieldnum) {
			case 0:
				se->se_sp.sp_domain = strtoul(field, &end, 0);
				if (end[0] != '\0' || field == end)
					break;
				continue;
			case 1:
				se->se_sp.sp_type = strtoul(field, &end, 0);
				if (end[0] != '\0' || field == end)
					break;
				continue;
			case 2:
				se->se_sp.sp_protocol = strtoul(field, &end, 0);
				if (end[0] != '\0' || field == end)
					break;
				continue;
			case 3:
				if ((se->se_sp.sp_path = strdup(field)) == NULL)
					break;
				continue;
			}
			break;
		}
		if (fieldnum == 0 && (field == NULL || field[0] == '#'))
			continue;	/* skip blank or comment lines */
		if (fieldnum != 4)
			goto error;
		/* good entry, add it to the list */
		*selistp = se;
		selistp = &se->se_next;
		se = NULL;
		entries++;
	}
	{
		struct sockpath **spp;
		int i;

		/* create master array */
		if ((spp = calloc(entries + 1, sizeof(struct sockpath *))) == NULL) {
			tsd->sperror = SP_NOMEM;
			goto error;
		}
		for (se = selist, i = 0; i < entries; i++) {
			spp[i] = (struct sockpath *) se;
			se = se->se_next;
		}
		spp[entries] = NULL;

		return ((__sockpath_sp_list = (struct sockpathent **) spp));
	}
      nomem:
	tsd->sperror = SP_NOMEM;
	goto error;
      openfail:
	tsd->sperror = SP_OPENFAIL;
	goto error;
      error:
	if (se != NULL) {
		*selistp = se;
		selistp = &se->se_next;
		se = NULL;
	}
	while ((se = selist)) {
		selist = se->se_next;
		if (se->se_sp.sp_path)
			free(se->se_sp.sp_path);
		free(se);
	}
	if (file)
		fclose(file);
	return (NULL);
}

static struct sockpathent **
__sockpath_getsockpathlist(void)
{
	struct sockpathent **sep;

	pthread_rwlock_rdlock(&__sockpath_sp_lock);
	if ((sep = __sockpath_sp_list) != NULL) {
		pthread_rwlock_unlock(&__sockpath_sp_lock);
		return (sep);
	}
	pthread_rwlock_unlock(&__sockpath_sp_lock);
	pthread_rwlock_wrlock(&__sockpath_sp_lock);
	if ((sep = __sockpath_sp_list) != NULL) {
		pthread_rwlock_unlock(&__sockpath_sp_lock);
		return (sep);
	}
	sep = __sockpath_loadsockpathlist();
	pthread_rwlock_unlock(&__sockpath_sp_lock);
	return (sep);
}

/**
 * @fn void *setsockpath(void)
 * @brief Provide handle to the sock2path database.
 *
 * This function has the effect of binding to or rewinding the sock2path databse.  This function
 * must be called before the first call to getsockpath() and may be called at any other time.  This
 * function need no be called before a call to getsockpathent().  This function returns NULL at the
 * end of the file, or upon failure.
 */
void *
__sockpath_setsockpath(void)
{
	struct sockpathent **sep;
	struct sockpath_handle *sh = NULL;

	if ((sep = __sockpath_getsockpathlist()) == NULL)
		return (NULL);
	if ((sh = malloc(sizeof(*sh))) == NULL) {
		sp_error = SP_NOMEM;
		return (NULL);
	}
	pthread_rwlock_init(&sh->sh_lock, NULL);
	sh->sh_head = sep;
	sh->sh_curr = sep;
	sp_error = SP_NOERROR;
	return ((void *) sh);
}

__asm__(".symver __sockpath_setsockpath,setsockpath@@SOCKPATH_1.0");

/**
 * @fn struct sockpath *getsockpath(void *handle)
 * @brief Retrieve next entry in the sock2path database.
 * @param handle handle returned by setsockpath().
 *
 * This function returns a pointer to the current entry in the sock2path database, formatted as a
 * struct sockpath.  Successive calls will return successive sock2path entries in the sock2path
 * databse.  This function can be used to search the entire sock2path file.  This function returns
 * NULL at the end of the file.  handle is the handle obtained through setsockpath().
 */
struct sockpath *
__sockpath_getsockpath(void *handle)
{
	struct sockpath_handle *sh = (struct sockpath_handle *) handle;
	struct sockpath *sp;

	pthread_rwlock_rdlock(&__sockpath_sp_lock);
	if (__sockpath_sp_list == NULL || sh == NULL) {
		sp_error = SP_NOSET;
		pthread_rwlock_unlock(&__sockpath_sp_lock);
		return (NULL);
	}
	pthread_rwlock_unlock(&__sockpath_sp_lock);
	pthread_rwlock_wrlock(&sh->sh_lock);
	if (!(sp = (struct sockpath *) (*(sh->sh_curr)))) {
		sp_error = SP_NOMOREENTRIES;
		pthread_rwlock_unlock(&sh->sh_lock);
		return (NULL);
	}
	sh->sh_curr++;
	sp_error = SP_NOERROR;
	pthread_rwlock_unlock(&sh->sh_lock);
	return (sp);
}

__asm__(".symver __sockpath_getsockpath,getsockpath@@SOCKPATH_1.0");

/**
 * @fn int endsockpath(void *handle)
 * @brief Release socket path database.
 * @param handle handle returned by setsockpath().
 *
 * This function should be called when processing is complete to release resources held for reuse.
 * handle is the handle obtained through setsockpath().  Programmers should be aware, however, that
 * the last call to endsockpath() frees all memory allocated by getsockpath() for the struct
 * sockpath data structures.  This function may not be called before setsockpath().
 */
int
__sockpath_endsockpath(void *handle)
{
	struct sockpath_handle *sh = (struct sockpath_handle *) handle;

	pthread_rwlock_rdlock(&__sockpath_sp_lock);
	if (__sockpath_sp_list == NULL || sh == NULL) {
		sp_error = SP_NOSET;
		pthread_rwlock_unlock(&__sockpath_sp_lock);
		return (-1);
	}
	pthread_rwlock_unlock(&__sockpath_sp_lock);
	/* strange thing to do, no? */
	pthread_rwlock_wrlock(&sh->sh_lock);
	free(sh);
	sp_error = SP_NOERROR;
	return (0);
}

__asm__(".symver __sockpath_endsockpath,endsockpath@@SOCKPATH_1.0");

/**
 * @fn struct sockpath *getsockpathent(int domain, int type, int protocol)
 * @brief Return a socket path database entry for a domain, type and protocol.
 * @param domain domain from the socket(3) call.
 * @param type type from the socket(3) call.
 * @param protocol protocol from the socket(3) call.
 *
 * This function returns a pointer to the struct sockpath structure corresponding to the
 * arguments domain, type and protocol.  It returns NULL if any argument is invalid (that is, does
 * not have an entry in the socket path database).
 *
 * This function returns a pointer to the struct sockpath structure on success and NULL on
 * failure.
 */
struct sockpath *
__sockpath_getsockpathent(int domain, int type, int protocol)
{
	struct sockpathent **sep;
	struct sockpath *sp = NULL;

	if ((sep = __sockpath_getsockpathlist())) {
		sp_error = SP_NOTFOUND;
		for (; *sep; ++sep) {
			if (((*sep)->se_sp.sp_domain == domain || domain == 0) &&
			    ((*sep)->se_sp.sp_type == type) &&
			    ((*sep)->se_sp.sp_protocol == protocol || protocol == 0)) {
				if ((sp = malloc(sizeof(*sp))) == NULL) {
					sp_error = SP_NOMEM;
					break;
				}
				memcpy(sp, (*sep), sizeof(*sp));
				if ((sp->sp_path = strdup((*sep)->se_sp.sp_path)) == NULL) {
					free(sp);
					sp_error = SP_NOMEM;
					break;
				}
			}
			sp_error = SP_NOERROR;
			break;
		}
	}
	return (sp);
}

__asm__(".symver __sockpath_getsockpathent,getsockpathent@@SOCKPATH_1.0");

/**
 * @fn void freesockpathent(struct sockpath *sp)
 * @brief Free a socket path database entry.
 * @param sp the database entry to free.
 *
 * This function frees the sockpath structure pointed to by sp (previously returned by
 * getsockpathent()).
 */
void
__sockpath_freesockpathent(struct sockpath *sp)
{
	if (sp) {
		if (sp->sp_path)
			free(sp->sp_path);
		free(sp);
	}
}

__asm__(".symver __sockpath_freesockpathent,freesockpathent@@SOCKPATH_1.0");

/* *INDENT-OFF* */
static const char *__sockpath_sp_errlist[] = {
/*
TRANS Corresponds to the SP_NOERROR constant.  No error is indicated in the
TRANS nc_error(3) variable.  The last sockpath(3) operation was successful.
 */
	[SP_NOERROR] = gettext_noop("no error"),
/*
TRANS Corresponds to the SP_NOMEM constant.  Out of memory.  Memory could not be
TRANS allocated to complete the last sockpath(3) operation.
 */
	[SP_NOMEM] = gettext_noop("out of memory"),
/*
TRANS Corresponds to the SP_NOSET constant.  The getsockpath(3) or
TRANS endsockpath(3) function was called out of order: that is, before
TRANS setsockpath(3) was called.
 */
	[SP_NOSET] = gettext_noop("routine called out of order"),
/*
TRANS Corresponds to the SP_OPENFAIL constant.  The /etc/sock2path file could
TRANS not be opened for reading.  %1$s is the name of the /etc/sock2path file.
 */
	[SP_OPENFAIL] = gettext_noop("could not open %s"),
/*
TRANS Corresponds to the SP_BADLINE constant.  A syntax error exists in the
TRANS /etc/sock2path file at the location shown.  The field number is the
TRANS whitespace separated field within the line in the /etc/sock2path file at
TRANS which the syntax error was detected.  The line number is the line number
TRANS in the /etc/sock2path file at which the syntax error was detected.  %1$s
TRANS is the name of the /etc/sock2path file.  %2$d is the field number.  %3$d
TRANS is the line number.
 */
	[SP_BADLINE] = gettext_noop("syntax error in %s: field %d of line %d"),
/*
TRANS Corresponds to the SP_NOTFOUND constant.  The "domain", "type" or
TRANS "protocol" specified as an argument to the getsockpathent(3) subroutine
TRANS was not found in any entry in the /etc/sock2path file.  %1$s is the name
TRANS of the /etc/sock2path file.
 */
	[SP_NOTFOUND] = gettext_noop("netid not found in %s"),
/*
TRANS Corresponds to the SP_NOMOREENTRIES constant.  All entries in the
TRANS /etc/sock2path file has been exhausted.  They have either already been
TRANS retrieved with getsockpath(3), getsockpathent(3), or the /etc/sock2path
TRANS file contains no entries.  %1$s is the name of the /etc/sock2path file.
 */
	[SP_NOMOREENTRIES] = gettext_noop("no more entries in %s"),
/*
TRANS Corresponds to any other constant.  An internal error in the libxnsl(3)
TRANS library has occurred resulting in an undefined error number.  This
TRANS situation should not occur and represents a bug in the library.  %1$d is
TRANS the decimal value of the unknown error number.
 */
	[SP_ERROR_MAX] = gettext_noop("unknown error number %d"),
};
/* *INDENT-ON* */

/**
 * @fn char *sp_sperror(void)
 * @brief Return and error string.
 *
 * This function is similar to sp_perror() but instead of printing the message to standard error,
 * will return a pointer to a string that contains the error message.
 *
 * This function returns a pointer to a buffer that contains the error message string.  This buffer
 * is overwritten on each call.  In multithreaded applications, this bufer is implemented as
 * thread-specific data.
 */
char *
__sockpath_sp_sperror(void)
{
	int err, idx;
	struct __sockpath_tsd *tsd = __sockpath_get_tsd();
	char *errbuf = tsd->sperrbuf;

	if (errbuf != NULL) {
		switch ((idx = err = tsd->sperror)) {
		case SP_NOERROR:
		case SP_NOMEM:
		case SP_NOSET:
			(void) strncpy(errbuf, __sockpath_sp_errlist[idx], SPERR_BUFSZ);
			break;
		case SP_OPENFAIL:
		case SP_NOTFOUND:
		case SP_NOMOREENTRIES:
			(void) snprintf(errbuf, SPERR_BUFSZ, __sockpath_sp_errlist[idx], SOCK2PATH);
			break;
		case SP_BADLINE:
			(void) snprintf(errbuf, SPERR_BUFSZ, __sockpath_sp_errlist[idx], SOCK2PATH,
					tsd->fieldnum, tsd->linenum);
			break;
		default:
			idx = SP_ERROR_MAX;
			(void) snprintf(errbuf, SPERR_BUFSZ, __sockpath_sp_errlist[idx], err);
			break;
		}
	}
	return (errbuf);

}

__asm__(".symver __sockpath_sp_sperror,sp_sperror@@SOCKPATH_1.0");

/**
 * @fn void sp_perror(const char *msg)
 * @brief Print an error message to standard error.
 * @param msg message to prefix to error message.
 *
 * This function prints an error message to standard error indicating why any of the above routines
 * failed.  The message is prepended with the string provided in the msg argument plus a colon.  A
 * NEWLINE is appended to the end of the message.
 */
void
__sockpath_sp_perror(const char *msg)
{
	if (msg)
		fprintf(stderr, "%s: %s\n", msg, __sockpath_sp_sperror());
	else
		fprintf(stderr, "%s\n", __sockpath_sp_sperror());
	return;
}

__asm__(".symver __sockpath_sp_perror,sp_perror@@SOCKPATH_1.0");

/*
 * vim: comments+=b\:TRANS
 */
