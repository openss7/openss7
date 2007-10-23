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

/* ssaplose.c - SPM: you lose */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ssap/RCS/ssaplose.c,v 9.0 1992/06/16 12:39:41 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ssap/RCS/ssaplose.c,v 9.0 1992/06/16 12:39:41 isode Rel
 *
 *
 * Log: ssaplose.c,v
 * Revision 9.0  1992/06/16  12:39:41  isode
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
#include "spkt.h"
#include "tailor.h"

/*  */

#ifndef	lint
static int _ssaplose();
#endif

#ifndef	lint
int
spktlose(va_alist)
	va_dcl
{
	int reason, result, sd, secs, value;
	register struct ssapblk *sb;
	register struct ssapkt *s;
	struct SSAPindication sis;
	register struct SSAPindication *si;
	register struct SSAPabort *sa;
	struct TSAPdata txs;
	register struct TSAPdata *tx = &txs;
	struct TSAPdisconnect tds;
	register struct TSAPdisconnect *td = &tds;
	va_list ap;

	va_start(ap);

	sd = va_arg(ap, int);

	si = va_arg(ap, struct SSAPindication *);

	if (si == NULL)
		si = &sis;

	reason = va_arg(ap, int);

	value = reason & SC_REFUSE;
	reason &= ~SC_REFUSE;

	result = _ssaplose(si, reason, ap);

	va_end(ap);

	if ((sa = &si->si_abort)->sa_cc > 0) {
		SLOG(ssap_log, LLOG_EXCEPTIONS, NULLCP,
		     ("spktlose [%s] %*.*s", SErrString(sa->sa_reason),
		      sa->sa_cc, sa->sa_cc, sa->sa_prdata));
	} else
		SLOG(ssap_log, LLOG_EXCEPTIONS, NULLCP,
		     ("spktlose [%s]", SErrString(sa->sa_reason)));

	if (value && SC_OFFICIAL(reason)) {
		if ((s = newspkt(SPDU_RF)) == NULL)
			return result;

		s->s_mask |= SMASK_RF_DISC;
		s->s_rf_disconnect |= RF_DISC_RELEASE;

		if (reason == SC_VERSION) {
			s->s_mask |= SMASK_RF_VRSN;
			s->s_rf_version = SB_ALLVRSNS;
		}

		if (s->s_rdata = malloc((unsigned) (s->s_rlen = 1)))
			*s->s_rdata = reason & 0xff;

		secs = ses_rf_timer;
	} else {
		if ((s = newspkt(SPDU_AB)) == NULL)
			return result;

		s->s_mask |= SMASK_SPDU_AB | SMASK_AB_DISC;
		s->s_ab_disconnect = AB_DISC_RELEASE;
		switch (reason) {
		case SC_PROTOCOL:
		case SC_VERSION:
			s->s_ab_disconnect |= AB_DISC_PROTO;
			break;

		case SC_CONGEST:
		case SC_SSAPID:
			break;

		default:
			s->s_ab_disconnect |= AB_DISC_UNKNOWN;
			break;
		}

		secs = ses_ab_timer;
	}

	value = spkt2sd(s, sd, (sb = findsblk(sd)) && (sb->sb_flags & SB_EXPD), si);

	freespkt(s);
	if (value == NOTOK)
		return result;

	if (secs >= 0)
		switch (TReadRequest(sd, tx, secs, td)) {
		case OK:
		default:
			TXFREE(tx);
			break;

		case NOTOK:
			break;
		}

	return result;
}
#else
/* VARARGS5 */

int
spktlose(sd, si, reason, what, fmt)
	int sd, reason;
	struct SSAPindication *si;
	char *what, *fmt;
{
	return spktlose(sd, si, reason, what, fmt);
}
#endif

/*  */

#ifndef	lint
int
ssaplose(va_alist)
	va_dcl
{
	int reason, result;
	struct SSAPindication *si;
	va_list ap;

	va_start(ap);

	si = va_arg(ap, struct SSAPindication *);
	reason = va_arg(ap, int);

	result = _ssaplose(si, reason, ap);

	va_end(ap);

	return result;
}
#else
/* VARARGS4 */

int
ssaplose(si, reason, what, fmt)
	struct SSAPindication *si;
	int reason;
	char *what, *fmt;
{
	return ssaplose(si, reason, what, fmt);
}
#endif

/*  */

#ifndef	lint
static int
_ssaplose(si, reason, ap)		/* what, fmt, args ... */
	register struct SSAPindication *si;
	int reason;
	va_list ap;
{
	register char *bp;
	char buffer[BUFSIZ];
	register struct SSAPabort *sa;

	if (si) {
		bzero((char *) si, sizeof *si);
		si->si_type = SI_ABORT;
		sa = &si->si_abort;

		asprintf(bp = buffer, ap);
		bp += strlen(bp);

		sa->sa_peer = 0;
		sa->sa_reason = reason;

		if ((sa->sa_cc = min(bp - buffer, sizeof sa->sa_prdata)) > 0)
			bcopy(buffer, sa->sa_prdata, sa->sa_cc);
	}

	return NOTOK;
}
#endif
