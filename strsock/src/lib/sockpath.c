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
 *  This is a simple library for accessing the /etc/sock2path file in a manner similar to the
 *  netconfig(3) functions.  It is used both by the soconfig(8) utility as well as the libsocklib
 *  library.
 */

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

#include <sys/sockpath.h>

struct sockpathent {
	int se_domain;
	int se_type;
	int se_protocol;
	char *se_path;
};

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
__sockpath_tsd_alloc(void)
{
	int ret;
	char *buf;

	ret = pthread_key_create(&__sockpath_tsd_key, __sockpath_tsd_free);
	buf = malloc(sizeof(struct __sockpath_tsd));
	memset(buf, 0, sizeof(*buf));
	ret = pthread_setspecific(__sockpath_tsd_key, (void *)buf);
	return;
}

static struct __sockpath_tsd *
__sockpath_get_tsd(void)
{
	pthread_once(&__sockpath_tsd_once, __sockpath_tsd_alloc);
	return (struct __sockpath_tsd *)pthread_getspecific(__sockpath_tsd_key);
}

int *
__sockpath___sperror(void)
{
	return &(__sockpath_get_tsd()->sperror);
}

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
__sockpath_loadsock2pathlist(void)
{
	char buffer[1024];
	char *line;
	int linenum, entries = 0;
	FILE *file = NULL;
	struct sockpathent *sp = NULL, *splist = NULL, **splistp = &splist;
	struct __sockpath_tsd *tsd = __sockpath_get_tsd();

	if ((file = fopen(SOCK2PATH, "r")) == NULL)
		goto openfail;

	/* read file one line at a time */
	for (linenum = 1; (line = fgets(buffer, sizeof(buffer), file)) != NULL; linenum++) {
		char *str, *field, *tmp = NULL;
		int fieldnum;

		/* allocate one if we don't already have one */
		if (sp == NULL) {
			if ((sp = malloc(sizeof(*sp))) == NULL)
				goto nomem;
			memset(sp, 0, sizeof(*sp));
		}
		for (str = line, fieldnum = 0; fieldnum < 4
		     && (field = strtok_r(str, " \t\f\n\r\v", &tmp)) != NULL;
		     str = NULL, fieldnum++) {
			char *end;

			switch (fieldnum) {
			case 0:
				sp->se_domain = strtoul(field, &end, 0);
				if (end[0] != '\0' || field == end)
					break;
				continue;
			case 1:
				sp->se_type = strtoul(field, &end, 0);
				if (end[0] != '\0' || field == end)
					break;
				continue;
			case 2:
				sp->se_protocol = strtoul(field, &end, 0);
				if (end[0] != '\0' || field == end)
					break;
				continue;
			case 3:
				if ((sp->se_path = strdump(field)) == NULL)
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
		*splistp = sp;
		splistp = &sp->se_next;
		sp = NULL;
		entries++;
	}
	{
		struct sockpathent **spp;
		int i;

		/* create master array */
		if ((spp = calloc(entries + 1, sizeof(struct sockpathent *))) == NULL) {
			tsd->sperror = SP_NOMEM;
			goto error;
		}
		for (sp = splist; i = 0; i < entries; i++) {
			spp[i] = sp;
			sp = sp->se_next;
		}
		spp[entries] = NULL;

		__sockpath_sp_list = spp;
		return (spp);
	}
      nomem:
	tsd->sperror = SP_NOMEM;
	goto error;
      openfail:
	tsd->sperror = SP_OPENFAIL;
	goto error;
      error:
	if (sp != NULL) {
		*splistp = sp;
		splistp = &sp->se_next;
		sp = NULL;
	}
	while ((sp = splist)) {
		splist = sp->se_next;
		if (sp->se_path)
			free(sp->se_path);
		free(sp);
	}
	if (file)
		fclose(file);
	return (NULL);
}

__asm__(".symver __sockpath___sperror,__sperror@@SOCKPATH_1.0");

void *
__sockpath_setsockpath(void)
{
	return (NULL);
}

__asm__(".symver __sockpath_setsockpath,setsockpath@@SOCKPATH_1.0");

struct sockpathent *
__sockpath_getsockpath(void *handle)
{
	return (NULL);
}

__asm__(".symver __sockpath_getsockpath,getsockpath@@SOCKPATH_1.0");

int
__sockpath_endsockpath(void *handle)
{
	return (-1);
}

__asm__(".symver __sockpath_endsockpath,endsockpath@@SOCKPATH_1.0");

struct sockpathent *
__sockpath_getsockpathent(int domain, int type, int protocol)
{
}

__asm__(".symver __sockpath_getsockpathent,getsockpathent@@SOCKPATH_1.0");

void
__sockpath_freesockpathent(struct sockpathent *se)
{
}

__asm__(".symver __sockpath_freesockpathent,freesockpathent@@SOCKPATH_1.0");
