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

/* ssapexec.c - SPM: exec */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ssap/RCS/ssapexec.c,v 9.0 1992/06/16 12:39:41 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ssap/RCS/ssapexec.c,v 9.0 1992/06/16 12:39:41 isode Rel
 *
 *
 * Log: ssapexec.c,v
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
#include "spkt.h"
#include "isoservent.h"
#include "tailor.h"

/*    SERVER only */

int
SExec(ts, si, hook, setperms)
	struct TSAPstart *ts;
	struct SSAPindication *si;
	IFP hook, setperms;
{
	int sd;
	char *cp;
	register struct isoservent *is;
	register struct ssapkt *s;
	struct TSAPdata txs;
	register struct TSAPdata *tx = &txs;
	struct TSAPdisconnect tds;
	register struct TSAPdisconnect *td = &tds;

	if (TReadRequest(sd = ts->ts_sd, tx, NOTOK, td) == NOTOK)
		return ts2sslose(si, "TReadRequest", td);

	s = tsdu2spkt(&tx->tx_qbuf, tx->tx_cc, NULLIP);
	TXFREE(tx);

	if (s == NULL || s->s_errno != SC_ACCEPT) {
		(void) spktlose(sd, si, (s ? s->s_errno : SC_CONGEST) | SC_REFUSE, NULLCP, NULLCP);
		goto out1;
	}

	switch (s->s_code) {
	case SPDU_CN:
		if ((s->s_mask & SMASK_CN_VRSN)
		    && !(s->s_cn_version & SB_ALLVRSNS)) {
			(void) spktlose(sd, si, SC_VERSION | SC_REFUSE, NULLCP,
					"version mismatch: expecting 0x%x, got 0x%x",
					SB_ALLVRSNS, s->s_cn_version);
			break;
		}

		if ((s->s_mask & SMASK_CN_CALLED) && s->s_calledlen > 0) {
			if ((is = getisoserventbyselector("ssap", s->s_called,
							  s->s_calledlen)) == NULL) {
				char buffer[BUFSIZ];

				buffer[explode(buffer, (u_char *) s->s_called,
					       s->s_calledlen)] = NULL;
				(void) spktlose(sd, si, SC_SSAPID | SC_REFUSE, NULLCP,
						"ISO service ssap/%s not found", buffer);
				break;
			}
		} else if ((is = getisoserventbyname("presentation", "ssap"))
			   == NULL) {
			(void) spktlose(sd, si, SC_SSUSER | SC_REFUSE, NULLCP,
					"default presentation service not found");
			break;
		}

		if (TSaveState(sd, is->is_tail, td) == NOTOK) {
			(void) spktlose(sd, si, SC_CONGEST | SC_REFUSE, NULLCP, NULLCP);
			break;
		}
		cp = *is->is_tail++;
		if ((*is->is_tail++ = spkt2str(s)) == NULL) {
			(void) spktlose(sd, si, SC_CONGEST | SC_REFUSE, NULLCP, NULLCP);
			break;
		}
		*is->is_tail = NULL;

		switch (hook ? (*hook) (is, si) : OK) {
		case NOTOK:
			return NOTOK;

		case DONE:
			return OK;

		case OK:
		default:
			if (setperms)
				(void) (*setperms) (is);
			(void) execv(*is->is_vec, is->is_vec);
			SLOG(ssap_log, LLOG_FATAL, *is->is_vec, ("unable to exec"));
			(void) TRestoreState(cp, ts, td);
			(void) spktlose(ts->ts_sd, si, SC_CONGEST | SC_REFUSE,
					*is->is_vec, "unable to exec");
			break;
		}
		break;

	default:
		(void) spktlose(sd, si, SC_PROTOCOL | SC_REFUSE, NULLCP,
				"session protocol mangled: expecting 0x%x, got 0x%x",
				SPDU_CN, s->s_code);
		break;
	}

      out1:;
	freespkt(s);

	return NOTOK;
}
