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

/* ftamlose.c - FPM: you lose */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ftam/RCS/ftamlose.c,v 9.0 1992/06/16 12:14:55 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ftam/RCS/ftamlose.c,v 9.0 1992/06/16 12:14:55 isode Rel
 *
 *
 * Log: ftamlose.c,v
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
#include <varargs.h>
#include "fpkt.h"

/*  */

#ifndef	lint
int
fpktlose(va_alist)
	va_dcl
{
	int observer, reason, result, source;
	struct FTAMindication ftis;
	register struct ftamblk *fsb;
	register struct FTAMindication *fti;
	va_list ap;

	va_start(ap);

	fsb = va_arg(ap, struct ftamblk *);
	fti = va_arg(ap, struct FTAMindication *);
	reason = va_arg(ap, int);

	if (fsb->fsb_flags & FSB_INIT)
		observer = EREF_IFPM, source = EREF_RFPM;
	else
		observer = EREF_RFPM, source = EREF_IFPM;

	result = _ftamoops(fti, reason, 1, observer, source, ap);

	va_end(ap);

	if (fsb->fsb_fd == NOTOK)
		return result;

	(void) FAbortRequestAux(fsb, type_FTAM_PDU_f__p__abort__request,
				FACTION_PERM, fti->fti_abort.fta_diags, 1, &ftis);

	return result;
}
#else
/* VARARGS5 */

int
fpktlose(fsb, fti, reason, what, fmt)
	struct ftamblk *fsb;
	struct FTAMindication *fti;
	int reason;
	char *what, *fmt;
{
	return fpktlose(fsb, fti, reason, what, fmt);
}
#endif

/*  */

#ifndef	lint
int
ftamlose(va_alist)
	va_dcl
{
	int fatal, reason, result;
	struct FTAMindication *fti;
	va_list ap;

	va_start(ap);

	fti = va_arg(ap, struct FTAMindication *);
	reason = va_arg(ap, int);
	fatal = va_arg(ap, int);

	result = _ftamoops(fti, reason, fatal, EREF_NONE, EREF_NONE, ap);

	va_end(ap);

	return result;
}
#else
/* VARARGS4 */

int
ftamlose(fti, reason, fatal, what, fmt)
	register struct FTAMindication *fti;
	int reason, fatal;
	char *what, *fmt;
{
	return ftamlose(fti, reason, fatal, what, fmt);
}
#endif

/*  */

#ifndef	lint
int
ftamoops(va_alist)
	va_dcl
{
	int reason, result, fatal, observer, source;
	struct FTAMindication *fti;
	va_list ap;

	va_start(ap);

	fti = va_arg(ap, struct FTAMindication *);
	reason = va_arg(ap, int);
	fatal = va_arg(ap, int);
	observer = va_arg(ap, int);
	source = va_arg(ap, int);

	result = _ftamoops(fti, reason, fatal, source, observer, ap);

	va_end(ap);

	return result;
}


static int  _ftamoops (fti, reason, fatal, observer, source, ap)
register struct FTAMindication *fti;
int	reason,
	fatal,
	observer,
	source;
va_list	ap;
{
    register char  *bp;
    char    buffer[BUFSIZ];
    register struct FTAMabort  *fta;
    register struct FTAMdiagnostic *ftd;

    if (fti) {
	bzero ((char *) fti, sizeof *fti);
	fti -> fti_type = FTI_ABORT;
	fta = &fti -> fti_abort;

	xsprintf (bp = buffer, ap);
	bp += strlen (bp);

	fta -> fta_peer = 0;
	fta -> fta_action = fatal ? FACTION_PERM : FACTION_TRANS;

static int
_ftamoops(fti, reason, fatal, observer, source, ap)
	register struct FTAMindication *fti;
	int reason, fatal, observer, source;
	va_list ap;
{
	register char *bp;
	char buffer[BUFSIZ];
	register struct FTAMabort *fta;
	register struct FTAMdiagnostic *ftd;

	if (fti) {
		bzero((char *) fti, sizeof *fti);
		fti->fti_type = FTI_ABORT;
		fta = &fti->fti_abort;

		asprintf(bp = buffer, ap);
		bp += strlen(bp);

		fta->fta_peer = 0;
		fta->fta_action = fatal ? FACTION_PERM : FACTION_TRANS;

		ftd = &fta->fta_diags[0];
		ftd->ftd_type = fatal ? DIAG_PERM : DIAG_TRANS;
		ftd->ftd_identifier = reason;
		ftd->ftd_observer = observer;
		ftd->ftd_source = source;
		ftd->ftd_delay = DIAG_NODELAY;
		copyFTAMdata(buffer, bp - buffer, ftd);

		fta->fta_ndiag = 1;
	}

	return NOTOK;
}
#else
/* VARARGS7 */

int
ftamoops(fti, reason, fatal, observer, source, what, fmt)
	struct FTAMindication *fti;
	int reason, fatal, observer, source;
	char *what, *fmt;
{
	return ftamoops(fti, reason, fatal, observer, source, what, fmt);
}
#endif
