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

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/file_str.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/file_str.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: file_str.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include "quipu/util.h"
#include "quipu/attrvalue.h"
#ifdef	CHECK_FILE_ATTRIBUTES
#include <sys/stat.h>
#endif

extern LLog *log_dsap;
extern char dsa_mode;

str2file_aux(str, at, x)
	char *str;
	AttributeType at;
	AttributeValue x;
{
	struct file_syntax *fs;

#ifdef CHECK_FILE_ATTRIBUTES
	struct stat filestat;
#endif

	x->av_syntax = AV_FILE;
	fs = (struct file_syntax *) smalloc(sizeof(struct file_syntax));
	fs->fs_ref = 1;
	fs->fs_attr = NULLAttrV;
	x->av_struct = (caddr_t) fs;
	if ((fs->fs_real_syntax = at->oa_syntax) >= AV_WRITE_FILE)
		fs->fs_real_syntax -= AV_WRITE_FILE;

	if (*str != 0 && *str == '/') {
		fs->fs_mode = 0;
		fs->fs_name = strdup(str);
	} else {
		if (dflt_attr_file(at, x, 1) == NOTOK)
			return (NOTOK);

		if (*str != 0) {
			char *p;

			if ((p = rindex(fs->fs_name, '/')) == NULLCP)
				p = fs->fs_name;
			else
				p++;
			if (strlen(p) < strlen(str))
				if ((fs->fs_name =
				     realloc(fs->fs_name,
					     (unsigned) ((p - fs->fs_name) + strlen(str) + 1)))
				    == NULLCP)
					return NOTOK;
			(void) strcpy(p, str);
			fs->fs_mode = 0;

		} else
			fs->fs_mode = FS_DEFAULT;
	}

#ifdef CHECK_FILE_ATTRIBUTES
	/* now check the file is OK */
	if (stat(fs->fs_name, &filestat) != -1)
		return (OK);
	else {
		parse_error("can't find attribute file %s", fs->fs_name);
		return (NOTOK);
	}
	/* NOTREACHED */
#else
	return (OK);
#endif

}

AttributeValue
str2file(str, at)
	char *str;
	AttributeType at;
{
	AttributeValue x;

	x = AttrV_alloc();
	if (str2file_aux(str, at, x) == OK)
		return (x);
	free((char *) x);
	return (NULLAttrV);
}

file_free(fs)
	struct file_syntax *fs;
{
	if (fs == (struct file_syntax *) NULL)
		return;

	fs->fs_ref--;

	if (fs->fs_ref > 0)
		return;

	if ((fs->fs_mode & FS_TMP) || (!dsa_mode && (fs->fs_mode & FS_CREATE))) {
		(void) unlink(fs->fs_name);
		DLOG(log_dsap, LLOG_DEBUG, ("Removed photo file '%s'", fs->fs_name));
		fs->fs_mode &= ~FS_CREATE;
	}

	if (fs->fs_attr)
		AttrV_free(fs->fs_attr);

	if (fs->fs_name)
		free(fs->fs_name);

	free((char *) fs);
}
