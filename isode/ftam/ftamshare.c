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

/* ftamshare.c - FPM: encode/decode shared ASE information */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ftam/RCS/ftamshare.c,v 9.0 1992/06/16 12:14:55 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ftam/RCS/ftamshare.c,v 9.0 1992/06/16 12:14:55 isode Rel
 *
 *
 * Log: ftamshare.c,v
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

struct type_FTAM_Shared__ASE__Information *
shared2fpm(fsb, sharedASE, fti)
	register struct ftamblk *fsb;
	PE sharedASE;
	struct FTAMindication *fti;
{
	register struct type_FTAM_Shared__ASE__Information *fpm;

	if ((fpm = (struct type_FTAM_Shared__ASE__Information *)
	     calloc(1, sizeof *fpm)) == NULL) {
		(void) ftamlose(fti, FS_GEN(fsb), 1, NULLCP, "out of memory");
		if (fpm)
			free_FTAM_Shared__ASE__Information(fpm);
		return NULL;
	}

	fpm->indirect__reference = sharedASE->pe_context;
	fpm->encoding->offset = choice_UNIV_0_single__ASN1__type;
	(fpm->encoding->un.single__ASN1__type = sharedASE)->pe_refcnt++;

	return fpm;
}

/*  */

int
fpm2shared(fsb, fpm, sharedASE, fti)
	struct ftamblk *fsb;
	register struct type_FTAM_Shared__ASE__Information *fpm;
	PE *sharedASE;
	struct FTAMindication *fti;
{
	PE pe;

	if (fpm->encoding->offset != choice_UNIV_0_single__ASN1__type)
		return ftamlose(fti, FS_GEN(fsb), 1, NULLCP,
				"shared ASE information not single-ASN1-type");

	if ((pe = pe_cpy(fpm->encoding->un.single__ASN1__type)) == NULLPE)
		(void) ftamlose(fti, FS_GEN(fsb), 1, NULLCP, "out of memory");
	(*sharedASE = pe)->pe_context = fpm->indirect__reference;

	return OK;
}
