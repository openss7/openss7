/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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
 * RFA - Remote File Access
 *
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * dirname.c : create local filenames 
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * Header: /xtel/isode/isode/others/rfa/RCS/dirname.c,v 9.0 1992/06/16 12:47:25 isode Rel
 *
 * Log: dirname.c,v
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/rfa/RCS/dirname.c,v 9.0 1992/06/16 12:47:25 isode Rel";
#endif

/*
 *                              NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <sys/param.h>
#include "general.h"
#include "rfa.h"

static char p[MAXPATHLEN];
char *fsBase = FS_BASE;

extern char *rindex();
extern char *realpath();
extern int errno;
int commandMode = 0;

char *
basename(fn)
	char *fn;
{
	register char *f;

	if (f = rindex(fn, '/'))
		return f + 1;
	else
		return fn;
}

char *
dirname(fn)
	char *fn;
{
	static char buf[MAXPATHLEN];
	register char *f;

	strcpy(buf, fn);
	if ((f = rindex(buf, '/')) && (f != buf)) {

		*f = '\0';
		return buf;
	} else
		return "/";
}

char *
makeFN(fn)
	char *fn;
{
	return makeFN2("", fn);
}

char *
makeFN2(dir, fn)
	char *dir;
	char *fn;
{
	register char *s = p;

	strcpy(s, fsBase);
	s += strlen(fsBase);
	if (*(s - 1) != '/')
		*(s++) = '/';
	while (*dir == '/')
		dir++;
	if (*dir != '\0') {
		strcpy(s, dir);
		s += strlen(dir);
	}
	if (*(s - 1) != '/')
		*(s++) = '/';

	while (*fn == '/')
		fn++;
	strcpy(s, fn);

	return p;
}

char *
getRelativeFN(fn)
	char *fn;
{
	if (strncmp(fsBase, fn, strlen(fsBase)))
		return NULL;
	return fn + strlen(fsBase);
}

char *
expandSymLinks(path)
	char *path;
{
	static char exp[MAXPATHLEN];
	char *r;

    /*--- expand symbolic links in fn ---*/
	if (realpath(makeFN(path), exp) == NULL) {
		if (errno != ENOENT) {
			sprintf(rfaErrStr, "%s - %s", sys_errname(errno), getRelativeFN(exp));
			return NULL;
		}
	}
	if ((r = getRelativeFN(exp)) == NULL) {
		sprintf(rfaErrStr, "%s not within RFA subtree", exp);
		return NULL;
	}
	return r;
}

char *
realPath3(dir, path1, path2)
	char *dir, *path1, *path2;
{
	register char *s, *s1, *rp;
	static char realp[MAXPATHLEN];
	char givenp[MAXPATHLEN];

	s = givenp;
	strcpy(s, dir);
	s += strlen(dir);
	if (*path1) {
		*(s++) = '/';
		strcpy(s, path1);
		s += strlen(path1);
	}
	if (*path2) {
		*(s++) = '/';
		strcpy(s, path2);
	}

	rp = realp;
	*(rp++) = '/';

	for (s = givenp; *s;) {
		while (*s == '/')
			s++;
		if (*s == '.') {
			s1 = s + 1;
			if (*(s1) == '/') {
				s += 2;
				continue;
			}
			if (*s1 == '.' && ((*(s1 + 1) == '/') || (*(s1 - 1) == '\0'))) {
				if ((rp - 1) != realp) {
					for (rp -= 2; *rp != '/'; rp--) ;
					rp++;
				}
				s += 3;
				continue;
			}
		}
		for (; *s && *s != '/'; s++, rp++)
			*rp = *s;
		*(rp++) = *(s++);
	}
	*rp = '\0';

	return realp;
}

char *
realPath(dir, path)
	char *dir, *path;
{
	return realPath3("", dir, path);
}

/*--------------------------------------------------------------*/
/*  getRfaContext						*/
/*--------------------------------------------------------------*/
char *
getRfaContext(cwd, fn)
	char *cwd, *fn;
{
	char *rp;
	char buf[MAXPATHLEN];

	if (*fn == '@')
		rp = realPath(fsBase, fn + 1);
	else if (*fn == '/')
		rp = realPath("/", fn);
	else if (commandMode) {
		(void) getwd(buf);
		rp = realPath(buf, fn);
	} else
		rp = realPath3(fsBase, cwd, fn);

	if (strncmp(fsBase, rp, strlen(fsBase)))
		return NULL;

    /*--- extract realtive path ---*/
	rp += strlen(fsBase);

	return rp;
}
