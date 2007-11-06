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

/* support.c - remote distribution -- support routines */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/idist/RCS/support.c,v 9.0 1992/06/16 14:38:53 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/others/idist/RCS/support.c,v 9.0 1992/06/16 14:38:53 isode Rel
 *
 * Support routines required by both client and server.
 *
 * Julian Onions <jpo@cs.nott.ac.uk>
 * Nottingham University Computer Science.
 *
 * 
 * Log: support.c,v
 * Revision 9.0  1992/06/16  14:38:53  isode
 * Release 8.0
 *
 * 
 */

#include <stdio.h>
#include "Idist-types.h"	/* type definitions */
#include "Idist-ops.h"		/* operation definitions */
#include "defs.h"

struct type_Idist_FileSpec *
makefs(type, opts, mode, size, mtime, uname, group, name, lname)
	unsigned short type, mode;
	off_t size;
	time_t mtime;
	int opts;
	char *uname, *group, *name, *lname;
{
	struct type_Idist_FileSpec *fs;
	struct type_Idist_FileType *makeftype();
	struct type_Idist_FileTime *makefmtime();
	struct type_Idist_Options *makeopts();

	if ((fs = (struct type_Idist_FileSpec *) malloc(sizeof *fs)) == NULL)
		adios("memory", "out of");
	fs->filetype = makeftype(type);
	fs->fileopts = makeopts(opts);
	fs->filemode = mode;
	fs->filesize = size;
	fs->filemtime = makefmtime(mtime);
	fs->fileowner = str2qb(uname, strlen(uname), 1);
	fs->filegroup = str2qb(group, strlen(group), 1);
	fs->filename = str2qb(name, strlen(name), 1);
	fs->linkname = str2qb(lname, strlen(lname), 1);
	return fs;
}

struct type_Idist_Options *
makeopts(opts)
	int opts;
{
	struct type_Idist_Options *rdo;

	rdo = pe_alloc(PE_CLASS_UNIV, PE_FORM_PRIM, PE_PRIM_BITS);
	if (rdo == NULL)
		adios("memory", "out of");
#define dobit(X,Y) if (opts & (X)) (void) bit_on (rdo, (Y))
	dobit(VERIFY, bit_Idist_Options_verify);
	dobit(WHOLE, bit_Idist_Options_whole);
	dobit(YOUNGER, bit_Idist_Options_younger);
	dobit(COMPARE, bit_Idist_Options_compare);
	dobit(FOLLOW, bit_Idist_Options_follow);
	dobit(IGNLNKS, bit_Idist_Options_ignlinks);
#ifdef UW
	dobit(NOINSTALL, bit_Idist_Options_noinstall);
#endif
#undef dobit
	return rdo;
}

struct type_Idist_FileType *
makeftype(type)
	unsigned short type;
{
	struct type_Idist_FileType *ft;

	if ((ft = (struct type_Idist_FileType *) malloc(sizeof *ft)) == NULL)
		adios("memory", "out of");
	switch (type) {
	case S_IFDIR:
		ft->parm = int_Idist_FileType_directory;
		break;
	case S_IFLNK:
		ft->parm = int_Idist_FileType_symlink;
		break;
	case S_IFREG:
		ft->parm = int_Idist_FileType_regular;
		break;
	case 0:
		ft->parm = int_Idist_FileType_hardlink;
		break;
	default:
		adios(NULLCP, "Illegal file type (%d)", type);
		break;
	}
	return ft;
}

struct type_Idist_FileTime *
makefmtime(mtime)
	long mtime;
{
	struct type_Idist_FileTime *fm;

	if ((fm = (struct type_Idist_FileTime *) malloc(sizeof *fm)) == NULL)
		adios("memory", "out of");
	/* Convert to time since Jan 1 1900 */
	fm->parm = mtime + 2208988800L;
	return fm;
}

long
convtime(fm)
	struct type_Idist_FileTime *fm;
{
	return fm->parm - 2208988800L;
}

/*
 * Expand file names beginning with `~' into the
 * user's home directory path name. Return a pointer in buf to the
 * part corresponding to `file'.
 */
char *
exptilde(buf, file)
	char buf[];
	register char *file;
{
	register char *s1, *s2, *s3;
	extern char homedir[];

	if (*file != '~') {
		(void) strcpy(buf, file);
		return (buf);
	}
	if (*++file == '\0') {
		s2 = homedir;
		s3 = NULL;
	} else if (*file == '/') {
		s2 = homedir;
		s3 = file;
	} else {
		s3 = file;
		while (*s3 && *s3 != '/')
			s3++;
		if (*s3 == '/')
			*s3 = '\0';
		else
			s3 = NULL;
		if (pw == NULL || strcmp(pw->pw_name, file) != 0) {
			if ((pw = getpwnam(file)) == NULL) {
				if (s3 != NULL)
					*s3 = '/';
				return (NULL);
			}
		}
		if (s3 != NULL)
			*s3 = '/';
		s2 = pw->pw_dir;
	}
	for (s1 = buf; *s1++ = *s2++;) ;
	s2 = --s1;
	if (s3 != NULL) {
		s2++;
		while (*s1++ = *s3++) ;
	}
	return (s2);
}
