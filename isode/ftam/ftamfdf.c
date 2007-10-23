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

/* ftamfdf.c - FDF support */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ftam/RCS/ftamfdf.c,v 9.0 1992/06/16 12:14:55 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ftam/RCS/ftamfdf.c,v 9.0 1992/06/16 12:14:55 isode Rel
 *
 *
 * Log: ftamfdf.c,v
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

int
fdf_p2names(fd, bits, names, fti)
	int fd;
	PE bits;
	int *names;
	struct FTAMindication *fti;
{
	register struct ftamblk *fsb;

	if ((fsb = findfsblk(fd)) == NULL)
		return ftamlose(fti, FS_GEN_NOREASON, 0, NULLCP, "invalid ftam descriptor");

	return fpm2bits(fsb, fname_pairs, bits, names, fti);
}

/*  */

int
fdf_names2p(fd, names, bits, fti)
	int fd;
	int names;
	PE *bits;
	struct FTAMindication *fti;
{
	register struct ftamblk *fsb;

	if ((fsb = findfsblk(fd)) == NULL)
		return ftamlose(fti, FS_GEN_NOREASON, 0, NULLCP, "invalid ftam descriptor");

	if ((*bits) = bits2fpm(fsb, fname_pairs, names, fti))
		return OK;
	return NOTOK;
}

/*  */

int
fdf_attrs2d(fd, fa, attrs, fti)
	int fd;
	struct FTAMattributes *fa;
	struct type_FTAM_Read__Attributes **attrs;
	struct FTAMindication *fti;
{
	register struct ftamblk *fsb;

	if ((fsb = findfsblk(fd)) == NULL)
		return ftamlose(fti, FS_GEN_NOREASON, 0, NULLCP, "invalid ftam descriptor");

	if ((*attrs) = attr2fpm(fsb, fa, fti))
		return OK;
	return NOTOK;
}

/*  */

int
fdf_d2attrs(fd, attrs, fa, fti)
	int fd;
	struct type_FTAM_Read__Attributes *attrs;
	struct FTAMattributes *fa;
	struct FTAMindication *fti;
{
	register struct ftamblk *fsb;

	if ((fsb = findfsblk(fd)) == NULL)
		return ftamlose(fti, FS_GEN_NOREASON, 0, NULLCP, "invalid ftam descriptor");

	return fpm2attr(fsb, attrs, fa, fti);
}
