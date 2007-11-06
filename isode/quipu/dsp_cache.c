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

/* dsp_cache.c - */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/dsp_cache.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/RCS/dsp_cache.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: dsp_cache.c,v
 * Revision 9.0  1992/06/16  12:34:01  isode
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

#include "quipu/util.h"
#include "quipu/dua.h"
#include "quipu/list.h"
#include "quipu/entry.h"
#include "quipu/common.h"
#include "quipu/dsargument.h"
#include "quipu/dsap.h"

extern time_t timenow;
extern LLog *log_dsap;
extern int local_cache_size;
extern Attr_Sequence as_merge_aux();
extern Entry local_find_entry_aux();
extern void as_write_files();
extern char *edbtmp_path;
extern AttributeType at_acl;
extern AttributeType at_objectclass;

Entry
cache_dsp_entry(ptr, complete)
	EntryInfo *ptr;
	char complete;
{
	/* assumes entry passed is complete */

	Entry make_path();
	Entry eptr;

	Attr_Sequence asptr;

	struct DSError error;
	DN dnptr;
	char aclfound = FALSE;
	char ocfound = FALSE;

	for (asptr = ptr->ent_attr; asptr != NULLATTR; asptr = asptr->attr_link) {
		if (asptr->attr_type == at_acl) {
			aclfound = TRUE;
			continue;
		}
		if (asptr->attr_type == at_objectclass) {
			ocfound = TRUE;
			continue;
		}
	}

	if (!aclfound) {
		LLOG(log_dsap, LLOG_TRACE, ("No ACL in dsp_cache"));
		return NULLENTRY;	/* don't cache if no acl */
	}
	if (!ocfound) {
		LLOG(log_dsap, LLOG_TRACE, ("No ObjectClass in dsp_cache"));
		return NULLENTRY;	/* don't cache if no oc */
		/* ... we could do if we were clever ... wait for next release !?! */
	}

	for (dnptr = ptr->ent_dn; dnptr->dn_parent != NULLDN; dnptr = dnptr->dn_parent) ;

	if ((eptr = local_find_entry_aux(ptr->ent_dn, FALSE)) != NULLENTRY) {
		if ((eptr->e_data == E_TYPE_CACHE_FROM_MASTER) ||
		    (eptr->e_data == E_TYPE_CONSTRUCTOR)) {
			as_free(eptr->e_attributes);
			as_write_files(ptr->ent_attr, edbtmp_path);
			eptr->e_attributes = as_cpy(ptr->ent_attr);
			eptr->e_complete = complete;
			eptr->e_data = E_TYPE_CACHE_FROM_MASTER;
			eptr->e_age = timenow;
			if (eptr->e_inherit)
				avs_free(eptr->e_inherit);
			eptr->e_inherit = NULLAV;
		} else {
			return NULLENTRY;
		}
	} else {
		if ((eptr = make_path(ptr->ent_dn)) == NULLENTRY)
			return NULLENTRY;
		local_cache_size++;
		eptr->e_name = rdn_cpy(dnptr->dn_rdn);
		eptr->e_complete = complete;
		eptr->e_data = E_TYPE_CACHE_FROM_MASTER;
		as_write_files(ptr->ent_attr, edbtmp_path);
		eptr->e_attributes = as_cpy(ptr->ent_attr);
		eptr->e_age = timenow;
	}

	if (unravel_attribute(eptr, &error) == NOTOK) {
		/* Keep name, but throw away attributes */
		local_cache_size--;
		eptr->e_data = E_TYPE_CONSTRUCTOR;
		eptr->e_complete = FALSE;
		eptr->e_leaf = FALSE;	/* Could be - but can't tell */
		as_free(eptr->e_attributes);
		eptr->e_attributes = NULLATTR;
		eptr->e_inherit = NULLAV;
		eptr->e_acl = (struct acl *) NULL;
		eptr->e_dsainfo = NULLDSA;
		eptr->e_external = 0;
		eptr->e_master = NULLAV;
		eptr->e_slave = NULLAV;
		eptr->e_oc = NULLAV;
		eptr->e_sacl = NULLAV;
		eptr->e_lacl = NULLAV;
		eptr->e_authp = NULLAUTHP;
		eptr->e_alias = NULLDN;
#ifdef DEBUG
		/* Unravel will log what is wrong */
		if (log_dsap->ll_events & LLOG_DEBUG)
			log_ds_error(&error);
#endif
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("Non-fatal error -> can not CACHE entry"));
		ds_error_free(&error);
		return NULLENTRY;
	}
	return (eptr);
}

dsp_cache(arg, res, ctx, binddn)
	struct DSArgument *arg;
	struct DSResult *res;
	char ctx;
	DN binddn;
{
	EntryInfo *ptr;
	Entry entryptr;
	Attr_Sequence as, eis_select(), attr_eis_select();
	char cache_search = FALSE;

	switch (arg->arg_type) {
	case OP_READ:
		entryptr = cache_dsp_entry(&res->res_rd.rdr_entry, TRUE);
		if (ctx == DS_CTX_X500_DAP)
			if (entryptr != NULLENTRY) {

				/* remove acl if DAP user not allowed it */
				as_free(res->res_rd.rdr_entry.ent_attr);
				if ((res->res_rd.rdr_entry.ent_attr =
				     eis_select(arg->arg_rd.rda_eis, entryptr, binddn, FALSE,
						res->res_rd.rdr_entry.ent_dn)) == NULLATTR) {
					/* TODO return error nosuchattributes */
					;
				}
			} else {
				as = res->res_rd.rdr_entry.ent_attr;
				if ((res->res_rd.rdr_entry.ent_attr =
				     attr_eis_select(arg->arg_rd.rda_eis, as, binddn,
						     res->res_rd.rdr_entry.ent_dn)) == NULLATTR) {
					/* TODO return error nosuchattributes */
					;
				}
				as_free(as);
			}
		break;
	case OP_SEARCH:
		if ((arg->arg_sr.sra_eis.eis_allattributes == TRUE) &&
		    (arg->arg_sr.sra_eis.eis_infotypes == EIS_ATTRIBUTESANDVALUES))
			cache_search = TRUE;

		for (ptr = res->res_sr.CSR_entries; ptr != NULLENTRYINFO; ptr = ptr->ent_next)
			(void) cache_dsp_entry(ptr, cache_search);
		break;
	case OP_LIST:
		if (ctx == DS_CTX_QUIPU_DSP)
			/* Only cache QUIPU DSP - only one we know the list ACL policy of */
			cache_list(res->res_ls.lsr_subordinates,
				   res->res_ls.lsr_limitproblem,
				   arg->arg_ls.lsa_object,
				   arg->arg_ls.lsa_common.ca_servicecontrol.svc_sizelimit);
		break;

		/* the following change an entry - the easiest thing is to deleted the cached entry 
		   and start again */
	case OP_ADDENTRY:
		delete_cache(arg->arg_ad.ada_object);
		break;
	case OP_REMOVEENTRY:
		delete_cache(arg->arg_rm.rma_object);
		break;
	case OP_MODIFYENTRY:
		delete_cache(arg->arg_me.mea_object);
		break;
	case OP_MODIFYRDN:
		delete_cache(arg->arg_mr.mra_object);
		break;
	default:
		break;
	}
}
