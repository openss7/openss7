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

/* acl_info.c - ? */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/acl_info.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/quipu/RCS/acl_info.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: acl_info.c,v
 * Revision 9.0  1992/06/16  12:34:01  isode
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
#include "quipu/entry.h"

extern AV_Sequence super_user;
extern LLog *log_dsap;

check_acl(who, mode, acl, node)
	register DN who;
	register int mode;
	struct acl_info *acl;
	DN node;
{
	register struct acl_info *ptr;
	AV_Sequence avs;

	for (ptr = acl; ptr != NULLACL_INFO; ptr = ptr->acl_next) {
		switch (ptr->acl_selector_type) {
		case ACL_ENTRY:
			if (mode <= ptr->acl_categories) {
				if (who == NULLDN) {
					break;
				}
				if (dn_cmp(who, node) == OK)
					return (OK);
			}
			break;
		case ACL_OTHER:
			if (mode <= ptr->acl_categories)
				return (OK);
			break;
		case ACL_PREFIX:
			if (mode <= ptr->acl_categories) {
				if (who == NULLDN)
					break;
				if (check_dnseq_prefix(ptr->acl_name, who) == OK)
					return (OK);
			}
			break;
		case ACL_GROUP:
			if (mode <= ptr->acl_categories) {
				if (who == NULLDN) {
					break;
				}
				if (check_dnseq(ptr->acl_name, who) == OK)
					return (OK);
			}
			break;
		}
	}

	/* one last try for access */
	for (avs = super_user; avs != NULLAV; avs = avs->avseq_next)
		if (dn_cmp(who, (DN) avs->avseq_av.av_struct) == OK)
			return (OK);

#ifdef DEBUG
	if (log_dsap->ll_events & LLOG_TRACE) {
		pslog(log_dsap, LLOG_TRACE, "access denied for user ",
		      (IFP) dn_print, (caddr_t) who);
		LLOG(log_dsap, LLOG_TRACE, ("  attempting mode=%d", mode));
		pslog(log_dsap, LLOG_TRACE, "  on entry ", (IFP) dn_print, (caddr_t) node);
	}
#endif

	return (NOTOK);
}

manager(dn)
	DN dn;
{
	register AV_Sequence avs;

	for (avs = super_user; avs != NULLAV; avs = avs->avseq_next)
		if (dn_cmp(dn, (DN) avs->avseq_av.av_struct) == OK)
			return (TRUE);

	return (FALSE);
}
