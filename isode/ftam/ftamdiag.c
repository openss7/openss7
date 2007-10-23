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

/* ftamdiag.c - FPM: encode/decode diagnostics */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ftam/RCS/ftamdiag.c,v 9.0 1992/06/16 12:14:55 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ftam/RCS/ftamdiag.c,v 9.0 1992/06/16 12:14:55 isode Rel
 *
 *
 * Log: ftamdiag.c,v
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

struct type_FTAM_Diagnostic *
diag2fpm(fsb, magic, diag, ndiag, fti)
	register struct ftamblk *fsb;
	int magic;
	struct FTAMdiagnostic diag[];
	int ndiag;
	struct FTAMindication *fti;
{
	register int i;
	register struct FTAMdiagnostic *dp;
	struct type_FTAM_Diagnostic *fpmp;
	register struct type_FTAM_Diagnostic *fpm, **fpc;
	register struct diag_element *f3;

	fpmp = NULL, fpc = &fpmp;
	for (dp = diag, i = ndiag - 1; i >= 0; dp++, i--) {
		if ((fpm = (struct type_FTAM_Diagnostic *) calloc(1, sizeof *fpm))
		    == NULL) {
		      no_mem:;
			(void) ftamlose(fti, FS_GEN(fsb), 1, NULLCP, "out of memory");
		      out:;
			if (fpmp)
				free_FTAM_Diagnostic(fpmp);
			return NULL;
		}
		*fpc = fpm;

		if ((f3 = (struct diag_element *) calloc(1, sizeof *f3)) == NULL)
			goto no_mem;
		fpm->diagnostic = f3;

		switch (dp->ftd_type) {
		case DIAG_INFORM:
		case DIAG_TRANS:
		case DIAG_PERM:
			break;

		default:
		      bad_dp:;
			(void) ftamlose(fti, FS_GEN(fsb), 0, NULLCP,
					"bad diagnostic type/observer/source (%d/%d/%d) at slot %d",
					dp->ftd_type, dp->ftd_observer, dp->ftd_source,
					ndiag - i - 1);
			goto out;
		}
		f3->diagnostic__type = dp->ftd_type;

		f3->error__identifier = dp->ftd_identifier;

		switch (dp->ftd_observer) {
		case EREF_IFSU:
		case EREF_IFPM:
			if (!(fsb->fsb_flags & FSB_INIT))
				goto bad_dp;
			break;

		case EREF_RFSU:
		case EREF_RFPM:
			if (fsb->fsb_flags & FSB_INIT)
				goto bad_dp;
			break;

		default:
			if (!magic)
				goto bad_dp;
			break;
		}
		if ((f3->error__observer = (struct type_FTAM_Entity__Reference *)
		     calloc(1, sizeof *f3->error__observer))
		    == NULL)
			goto no_mem;
		f3->error__observer->parm = dp->ftd_observer;

		switch (dp->ftd_source) {
		case EREF_NONE:
		case EREF_IFSU:
		case EREF_IFPM:
		case EREF_SERV:
		case EREF_RFPM:
		case EREF_RFSU:
			break;

		default:
			goto bad_dp;
		}
		if ((f3->error__source = (struct type_FTAM_Entity__Reference *)
		     calloc(1, sizeof *f3->error__source))
		    == NULL)
			goto no_mem;
		f3->error__source->parm = dp->ftd_source;

		if (dp->ftd_delay != DIAG_NODELAY) {
			f3->optionals |= opt_FTAM_diag_element_suggested__delay;
			f3->suggested__delay = dp->ftd_delay;
		}

		if (dp->ftd_cc > FTD_SIZE)
			goto bad_dp;
		else if (dp->ftd_cc > 0
			 && (f3->further__details = str2qb(dp->ftd_data, dp->ftd_cc, 1))
			 == NULL)
			goto no_mem;

		fpc = &fpm->next;
	}

	return fpmp;
}

/*  */

int
fpm2diag(fsb, fpm, diag, ndiag, fti)
	register struct ftamblk *fsb;
	register struct type_FTAM_Diagnostic *fpm;
	struct FTAMdiagnostic diag[];
	int *ndiag;
	struct FTAMindication *fti;
{
	register int i;
	register struct FTAMdiagnostic *dp;
	register struct diag_element *f3;

	*ndiag = 0;

	dp = diag, i = 0;
	for (; fpm; fpm = fpm->next) {
		if (i >= NFDIAG)
			return ftamlose(fti, FS_GEN(fsb), 1, NULLCP, "too many diagnostics");

		f3 = fpm->diagnostic;
		dp->ftd_type = f3->diagnostic__type;
		dp->ftd_identifier = f3->error__identifier;
		dp->ftd_observer = f3->error__observer->parm;
		dp->ftd_source = f3->error__source->parm;
		if (f3->optionals & opt_FTAM_diag_element_suggested__delay)
			dp->ftd_delay = f3->suggested__delay;
		else
			dp->ftd_delay = DIAG_NODELAY;
		if (f3->further__details) {
			register char *cp;

			if ((cp = qb2str(f3->further__details)) == NULL)
				return ftamlose(fti, FS_GEN(fsb), 1, NULLCP, "out of memory");
			(void) strncpy(dp->ftd_data, cp, sizeof dp->ftd_data);
			dp->ftd_data[sizeof dp->ftd_data - 1] = NULL;
			dp->ftd_cc = strlen(dp->ftd_data);
			free(cp);
		} else
			dp->ftd_cc = 0;

		dp++, i++;
	}

	*ndiag = i;

	return OK;
}
