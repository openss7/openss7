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

/* ftamchrg.c - FPM: encode/decode charging */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ftam/RCS/ftamchrg.c,v 9.0 1992/06/16 12:14:55 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ftam/RCS/ftamchrg.c,v 9.0 1992/06/16 12:14:55 isode Rel
 *
 *
 * Log: ftamchrg.c,v
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

struct type_FTAM_Charging *
chrg2fpm(fsb, charging, fti)
	register struct ftamblk *fsb;
	register struct FTAMcharging *charging;
	struct FTAMindication *fti;
{
	register int i;
	register struct fc_charge *fc;
	struct type_FTAM_Charging *fpmp;
	register struct type_FTAM_Charging *fpm, **fpc;
	register struct charge_element *f1;

	fpmp = NULL, fpc = &fpmp;
	for (fc = charging->fc_charges, i = charging->fc_ncharge - 1; i >= 0; fc++, i--) {
		if (fc->fc_resource == NULL || fc->fc_unit == NULL) {
			(void) ftamlose(fti, FS_GEN(fsb), 0, NULLCP,
					"empty charge at slot %d", charging->fc_ncharge - i - 1);
			goto out;
		}

		if ((fpm = (struct type_FTAM_Charging *) calloc(1, sizeof *fpm))
		    == NULL) {
		      no_mem:;
			(void) ftamlose(fti, FS_GEN(fsb), 1, NULLCP, "out of memory");
		      out:;
			if (fpmp)
				free_FTAM_Charging(fpmp);
			return NULL;
		}
		*fpc = fpm;

		if ((f1 = (struct charge_element *) calloc(1, sizeof *f1)) == NULL)
			goto no_mem;
		fpm->charge = f1;

		if ((f1->resource__identifier = str2qb(fc->fc_resource, strlen(fc->fc_resource), 1))
		    == NULL || (f1->charging__unit = str2qb(fc->fc_unit, strlen(fc->fc_unit), 1))
		    == NULL)
			goto no_mem;
		f1->charging__value = fc->fc_value;

		fpc = &fpm->next;
	}

	return fpmp;
}

/*  */

int
fpm2chrg(fsb, fpm, charging, fti)
	register struct ftamblk *fsb;
	register struct type_FTAM_Charging *fpm;
	register struct FTAMcharging *charging;
	struct FTAMindication *fti;
{
	register int i;
	register struct fc_charge *fc;
	register struct charge_element *f1;

	bzero((char *) charging, sizeof *charging);

	fc = charging->fc_charges, i = 0;
	for (; fpm; fpm = fpm->next) {
		if (i >= NFCHRG)
			return ftamlose(fti, FS_GEN(fsb), 1, NULLCP, "too many charges");

		f1 = fpm->charge;
		if ((fc->fc_resource = qb2str(f1->resource__identifier)) == NULL
		    || (fc->fc_unit = qb2str(f1->charging__unit)) == NULL) {
			if (fc->fc_resource)
				free(fc->fc_resource), fc->fc_resource = NULL;
			while (i-- > 0) {
				fc--;
				free(fc->fc_resource), fc->fc_resource = NULL;
				free(fc->fc_unit), fc->fc_unit = NULL;
			}

			return ftamlose(fti, FS_GEN(fsb), 1, NULLCP, "out of memory");
		}
		fc->fc_value = f1->charging__value;

		fc++, i++;
	}

	return OK;
}
