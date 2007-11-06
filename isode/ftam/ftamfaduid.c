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

/* ftamfaduid.c - FPM: encode/decode FADU identities */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ftam/RCS/ftamfaduid.c,v 9.0 1992/06/16 12:14:55 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ftam/RCS/ftamfaduid.c,v 9.0 1992/06/16 12:14:55 isode Rel
 *
 *
 * Log: ftamfaduid.c,v
 * Revision 9.0  1992/06/16  12:14:55  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

/* LINTLIBRARY */

#include <stdio.h>
#include "fpkt.h"

/*  */

struct type_FTAM_FADU__Identity *
faduid2fpm(fsb, fa, fti)
	register struct ftamblk *fsb;
	register struct FADUidentity *fa;
	struct FTAMindication *fti;
{
	register int n;
	register char **ap;
	register struct type_FTAM_FADU__Identity *fpm;
	register struct name_element **f4;

	if ((fpm = (struct type_FTAM_FADU__Identity *) calloc(1, sizeof *fpm))
	    == NULL) {
	      no_mem:;
		(void) ftamlose(fti, FS_GEN(fsb), 1, NULLCP, "out of memory");
	      out:;
		if (fpm)
			free_FTAM_FADU__Identity(fpm);
		return NULL;
	}

	switch (fa->fa_type) {
	case FA_FIRSTLAST:
		fpm->offset = type_FTAM_FADU__Identity_first__last;
		switch (fa->fa_firstlast) {
		case FA_FIRST:
		case FA_LAST:
			break;

		default:
		      bad_value:;
			(void) ftamlose(fti, FS_GEN(fsb), 0, NULLCP, "bad value in FADU identity");
			goto out;
		}
		fpm->un.first__last = fa->fa_firstlast;
		break;

	case FA_RELATIVE:
		fpm->offset = type_FTAM_FADU__Identity_relative;
		switch (fa->fa_relative) {
		case FA_PREVIOUS:
		case FA_CURRENT:
		case FA_NEXT:
			break;

		default:
			goto bad_value;
		}
		fpm->un.relative = fa->fa_relative;
		break;

	case FA_BEGINEND:
		fpm->offset = type_FTAM_FADU__Identity_begin__end;
		switch (fa->fa_beginend) {
		case FA_BEGIN:
		case FA_END:
			break;

		default:
			goto out;
		}
		fpm->un.begin__end = fa->fa_beginend;
		break;

	case FA_SINGLE:
		if (!fa->fa_singlename)
			goto bad_value;
		fpm->offset = type_FTAM_FADU__Identity_single__name;
		if ((fpm->un.single__name =
		     str2qb(fa->fa_singlename, strlen(fa->fa_singlename), 1)) == NULL)
			goto no_mem;
		break;

	case FA_NAMELIST:
		f4 = &fpm->un.name__list;
		for (n = fa->fa_nname - 1, ap = fa->fa_names; n >= 0; ap++, n--) {
			if (!*ap)
				goto bad_value;
			if ((*f4 = (struct name_element *) calloc(1, sizeof **f4))
			    == NULL || ((*f4)->Node__Name = str2qb(*ap, strlen(*ap), 1)) == NULL)
				goto no_mem;
			f4 = &((*f4)->next);
		}
		break;

	case FA_FADUNUMBER:
		fpm->offset = type_FTAM_FADU__Identity_fadu__number;
		fpm->un.fadu__number = fa->fa_fadunumber;
		break;

	default:
		(void) ftamlose(fti, FS_GEN(fsb), 0, NULLCP, "bad type for FADU identity");
		goto out;
	}

	return fpm;
}

/*  */

int
fpm2faduid(fsb, fpm, fa, fti)
	register struct ftamblk *fsb;
	register struct type_FTAM_FADU__Identity *fpm;
	register struct FADUidentity *fa;
	struct FTAMindication *fti;
{
	register int n;
	register char **ap;
	register struct name_element *f4;

	bzero((char *) fa, sizeof *fa);

	switch (fpm->offset) {
	case type_FTAM_FADU__Identity_first__last:
		fa->fa_type = FA_FIRSTLAST;
		fa->fa_firstlast = fpm->un.first__last;
		break;

	case type_FTAM_FADU__Identity_relative:
		fa->fa_type = FA_RELATIVE;
		fa->fa_relative = fpm->un.relative;
		break;

	case type_FTAM_FADU__Identity_begin__end:
		fa->fa_type = FA_BEGINEND;
		fa->fa_beginend = fpm->un.begin__end;
		break;

	case type_FTAM_FADU__Identity_single__name:
		fa->fa_type = FA_SINGLE;
		if ((fa->fa_singlename = qb2str(fpm->un.single__name))
		    == NULL) {
		      no_mem:;
			FUFREE(fa);
			return ftamlose(fti, FS_GEN(fsb), 1, NULLCP, "out of memory");
		}
		break;

	case type_FTAM_FADU__Identity_name__list:
		fa->fa_type = FA_NAMELIST;
		ap = fa->fa_names, n = NANAME;
		for (f4 = fpm->un.name__list; f4; f4 = f4->next) {
			if (n-- <= 0) {
				FUFREE(fa);
				return ftamlose(fti, FS_GEN(fsb), 1, NULLCP,
						"too many nodes in FADU name");
			}
			if ((*ap++ = qb2str(f4->Node__Name)) == NULL)
				goto no_mem;
		}
		break;

	case type_FTAM_FADU__Identity_fadu__number:
		fa->fa_type = FA_FADUNUMBER;
		fa->fa_fadunumber = fpm->un.fadu__number;
		break;
	}

	return OK;
}
