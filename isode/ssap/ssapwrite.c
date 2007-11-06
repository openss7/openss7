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

/* ssapwrite.c - SPM: write various SPDUs  */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ssap/RCS/ssapwrite.c,v 9.0 1992/06/16 12:39:41 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ssap/RCS/ssapwrite.c,v 9.0 1992/06/16 12:39:41 isode Rel
 *
 *
 * Log: ssapwrite.c,v
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

/*  */

int
SWriteRequestAux(sb, code, data, cc, type, ssn, settings, id, oid, ref, si)
	register struct ssapblk *sb;
	int code;
	char *data;
	int cc, type, settings;
	long ssn;
	struct SSAPactid *id, *oid;
	struct SSAPref *ref;
	struct SSAPindication *si;
{
	int result;
	register struct ssapkt *s, *p;
	struct TSAPdisconnect tds;
	register struct TSAPdisconnect *td = &tds;
	struct udvec uvs[3];
	register struct udvec *uv;

	if (sb->sb_flags & SB_EXPD)
		switch (code) {
		case SPDU_MAA:
			result = PR_MAA;
			goto send_pr;

		case SPDU_AI:
		case SPDU_AD:
		case SPDU_RS:
			result = PR_RS;
			goto send_pr;

		case SPDU_AIA:
		case SPDU_ADA:
		case SPDU_RA:
			result = PR_RA;
		      send_pr:;
			if ((p = newspkt(SPDU_PR)) == NULL)
				return ssaplose(si, SC_CONGEST, NULLCP, "out of memory");
			p->s_mask |= SMASK_PR_TYPE;
			p->s_pr_type = result;
			result = spkt2sd(p, sb->sb_fd, 1, si);
			freespkt(p);
			if (result == NOTOK)
				return NOTOK;
			break;

		default:
			break;
		}

	uv = uvs;
	uvs[0].uv_base = uvs[1].uv_base = NULL;

	switch (code) {
	case SPDU_MAP:
	case SPDU_MIP:
	case SPDU_RS:
	case SPDU_AS:
	case SPDU_AR:
	case SPDU_AD:
	case SPDU_AI:
#ifdef	notdef			/* aka SPDU_MAP */
	case SPDU_AE:
#endif
	case SPDU_CD:
		if (s = newspkt(SPDU_GT))
			s->s_mask |= SMASK_SPDU_GT;
		break;

	default:
		s = newspkt(SPDU_PT);
		break;
	}
	if (s == NULL)
		return ssaplose(si, SC_CONGEST, NULLCP, "out of memory");

	if (spkt2tsdu(s, &uv->uv_base, &uv->uv_len) == NOTOK) {
		(void) ssaplose(si, s->s_errno, NULLCP, NULLCP);
		goto out1;
	}
	freespkt(s);
	uv++;

	if ((s = newspkt(code)) == NULL) {
		(void) ssaplose(si, SC_CONGEST, NULLCP, "out of memory");
		goto out2;
	}
	switch (code) {
	case SPDU_MAP:
		if (type) {
			s->s_mask |= SMASK_MAP_SYNC;
			s->s_map_sync = type;
		}
		s->s_mask |= SMASK_MAP_SERIAL;
		s->s_map_serial = ssn;
		break;

	case SPDU_MAA:
		s->s_mask |= SMASK_MAA_SERIAL;
		s->s_maa_serial = ssn;
		break;

	case SPDU_MIP:
		if (type == SYNC_NOCONFIRM) {
			s->s_mask |= SMASK_MIP_SYNC;
			s->s_mip_sync = MIP_SYNC_NOEXPL;
		}
		s->s_mask |= SMASK_MIP_SERIAL;
		s->s_mip_serial = ssn;
		break;

	case SPDU_MIA:
		s->s_mask |= SMASK_MIA_SERIAL;
		s->s_mia_serial = ssn;
		break;

	case SPDU_RS:
		if (sb->sb_requirements & SR_TOKENS) {
			s->s_mask |= SMASK_RS_SET;
			s->s_rs_settings = settings;
		}
		s->s_mask |= SMASK_RS_TYPE;
		s->s_rs_type = type;
		s->s_mask |= SMASK_RS_SSN;
		s->s_rs_serial = ssn;
		break;

	case SPDU_RA:
		if (sb->sb_requirements & SR_TOKENS) {
			s->s_mask |= SMASK_RA_SET;
			s->s_ra_settings = settings;
		}
		s->s_mask |= SMASK_RA_SSN;
		s->s_ra_serial = ssn;
		break;

	case SPDU_AS:
		s->s_mask |= SMASK_AS_ID;
		s->s_as_id = *id;	/* struct copy */
		break;

	case SPDU_AR:
		s->s_mask |= SMASK_AR_OID | SMASK_AR_SSN | SMASK_AR_ID;
		s->s_ar_oid = *oid;	/* struct copy */
		s->s_ar_serial = ssn;
		s->s_ar_id = *id;	/* struct copy */
		if (ref) {
			s->s_mask |= SMASK_AR_REF;
			s->s_ar_reference = *ref;	/* struct copy */
		}
		break;

	case SPDU_AI:
		s->s_mask |= SMASK_AI_REASON;
		s->s_ai_reason = type;
		break;

	case SPDU_AD:
		s->s_mask |= SMASK_AD_REASON;
		s->s_ad_reason = type;
		break;

	case SPDU_ED:
		s->s_mask |= SMASK_ED_REASON;
		s->s_ed_reason = type;
		break;

	default:
		break;
	}

	if (cc > 0) {
		s->s_mask |= SMASK_UDATA_PGI;
		s->s_udata = data, s->s_ulen = cc;
	} else
		s->s_udata = NULL, s->s_ulen = 0;
	result = spkt2tsdu(s, &uv->uv_base, &uv->uv_len);
	s->s_mask &= ~SMASK_UDATA_PGI;
	s->s_udata = NULL, s->s_ulen = 0;

	if (result == NOTOK) {
		(void) ssaplose(si, s->s_errno, NULLCP, NULLCP);
		goto out3;
	}
	freespkt(s);
	uv++;

	uv->uv_base = NULL;

	if ((result = TWriteRequest(sb->sb_fd, uvs, td)) == NOTOK)
		(void) ts2sslose(si, "TWriteRequest", td);

	free(uvs[0].uv_base);
	free(uvs[1].uv_base);

	return result;

      out3:;
	if (uvs[1].uv_base)
		free(uvs[1].uv_base);
      out2:;
	if (uvs[0].uv_base)
		free(uvs[0].uv_base);
      out1:;
	freespkt(s);

	return NOTOK;
}
