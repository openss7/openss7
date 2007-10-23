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

#include <sys/types.h>

#include "quipu/util.h"
#include "quipu/common.h"
#include "quipu/entry.h"
#include "quipu/name.h"

#include "sequence.h"
#include "filt.h"
#include "defs.h"

extern str_seq dnseq, backseq, showseq;
extern int entry_number, back_buf_num, dn_number;
extern int sizelimit;
extern unsigned int typeindx;
extern filt_struct *filt_arr[];
extern char *filttype[];

#ifndef NO_STATS
extern LLog *log_stat;
#endif

extern char goto_path[], base_path[], friendly_base_path[], mvalue[];
extern char dir_error_message[];

str_seq SortList();
dsEnqError list_start();
void dn2buf();

dsEnqError
srch_start()
{
	struct ds_search_arg search_arg;
	struct ds_search_result result;
	struct DSError error;
	dsEnqError return_error;
	extern Filter make_filter();
	DN curr_rdn;

	if (*mvalue == '\0') {
		return list_start();
	}

	if (get_default_service(&search_arg.sra_common) != 0) {
		return nothingfound;
	}

	search_arg.sra_common.ca_servicecontrol.svc_options = SVC_OPT_PREFERCHAIN;

	curr_rdn = search_arg.sra_baseobject = (strncmp(base_path, "The World", 9) ?
						str2dn(base_path) : NULLDN);

	search_arg.sra_eis.eis_allattributes = FALSE;
	search_arg.sra_eis.eis_infotypes = EIS_ATTRIBUTETYPESONLY;
	search_arg.sra_eis.eis_select = 0;
	search_arg.sra_searchaliases = TRUE;

	search_arg.sra_subset = SRA_ONELEVEL;
	while (curr_rdn != NULLDN) {
		if (!strcmp(curr_rdn->dn_rdn->rdn_at->oa_ot.ot_stroid, "2.5.4.10")) {
			search_arg.sra_subset = SRA_WHOLESUBTREE;
			break;
		}
		curr_rdn = curr_rdn->dn_parent;
	}

	*dir_error_message = '\0';
	if ((search_arg.sra_filter = make_filter(filt_arr[typeindx])) == NULLFILTER)
		if (*dir_error_message == '\0') {
			return duaerror;
		} else {
			return error_string_given;
		}
#ifndef NO_STATS
	LLOG(log_stat, LLOG_NOTICE, ("search +%s, extent %d, val %s",
				     base_path, search_arg.sra_subset, mvalue));
#endif

	if (ds_search(&search_arg, &error, &result) != DS_OK) {
		/* deal with error */
		free_seq(dnseq);
		dnseq = NULLDS;
		dn_number = 0;
		log_ds_error(&error);
		ds_error_free(&error);
		switch (error.dse_type) {
		case DSE_LOCALERROR:
			return_error = duaerror;
			break;
		case DSE_REMOTEERROR:
			return_error = localdsaerror;
			break;
		case DSE_ATTRIBUTEERROR:
			return_error = attributerror;
			break;
		case DSE_REFERRAL:
		case DSE_DSAREFERRAL:
			return_error = remotedsaerror;
			break;
		case DSE_SECURITYERROR:
			return_error = security;
			break;
		case DSE_NAMEERROR:
			return_error = namerror;
			break;
		case DSE_SERVICEERROR:
			return_error = serviceerror;
			break;
		default:
			return_error = localdsaerror;
			break;
		}
	} else {
		correlate_search_results(&result);
		dn_number = 0;

		if (result.CSR_entries != NULLENTRYINFO) {
			register EntryInfo *ptr;

			return_error = Okay;
			free_seq(dnseq);
			dnseq = NULLDS;
			dn_number = 0;

			for (ptr = result.CSR_entries; ptr != NULLENTRYINFO; ptr = ptr->ent_next) {
				dn_number++;
				dn2buf((caddr_t) ptr->ent_dn, goto_path);
				add_seq(&dnseq, goto_path);
			}

			if (dn_number)
				dnseq = SortList(dnseq);
		} else if (result.CSR_limitproblem == LSR_NOLIMITPROBLEM) {
			free_seq(dnseq);
			dnseq = NULLDS;
			dn_number = 0;
			return_error = nothingfound;
		}

		if (result.CSR_limitproblem != LSR_NOLIMITPROBLEM) {
			switch (result.CSR_limitproblem) {
			case LSR_TIMELIMITEXCEEDED:
				if (dn_number > 0)
					return_error = timelimit_w_partial;
				else {
					free_seq(dnseq);
					dnseq = NULLDS;
					return_error = timelimit;
				}
				break;
			case LSR_SIZELIMITEXCEEDED:
				return_error = listsizelimit;
				break;
			case LSR_ADMINSIZEEXCEEDED:
				if (dn_number > 0)
					return_error = adminlimit_w_partial;
				else {
					free_seq(dnseq);
					dnseq = NULLDS;
					return_error = adminlimit;
				}
				break;
			}
			entryinfo_free(result.CSR_entries, 0);
		}
	}
	entry_number = dn_number;
	filter_free(search_arg.sra_filter);
	dn_free(search_arg.sra_baseobject);
	ds_error_free(&error);
	return return_error;
}
