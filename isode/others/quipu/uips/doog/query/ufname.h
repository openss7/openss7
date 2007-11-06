/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

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

#ifndef __QUIPU_UIPS_DOOG_QUERY_UFNAME_H__
#define __QUIPU_UIPS_DOOG_QUERY_UFNAME_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/ufname.h,v 9.0 1992/06/16 12:45:27 isode Rel */

#ifndef _query_ufname_h_
#define _query_ufname_h_

#include "types.h"
#include "dn_list.h"
#include "error.h"
#include "sequence.h"

/*
 *	Status return structure
 */
typedef struct _ufn_status {
	char *just_resolved;
	entryList resolved_against;
} ufn_status, *ufnStatus;

#define NULLUfnStatus (ufnStatus) NULL
#define ufn_status_alloc() (ufnStatus) sizeof(ufn_status)

/*
 *	Search path structures
 */

#define UfnPlus -1
#define UfnNobound -2

typedef struct _search_path {
	int lower_bound;
	int upper_bound;
	entryList path;

	struct _search_path *next;
} search_path, *searchPath;

#define NULLSearchPath (searchPath) NULL
#define search_path_alloc() (searchPath) smalloc(sizeof(search_path))

/*
 * name_part structs double up as search args and records for the
 * progress of UFN searches.
 *
 */
typedef struct _name_part {
	struct _name_part *next;
	char *part_name;

	QBool is_resolved;
	known is_bottom_level;

#define MATCHLIMIT 20
	entryList exact_matches;
	QCardinal exact_match_num;

	entryList good_matches;
	QCardinal good_match_num;

	entryList poor_matches;
	QCardinal poor_match_num;
} name_part, *namePart;

#define NULLNamePart (namePart) NULL
#define name_part_alloc() (namePart) smalloc(sizeof(name_part))

/*
 * Failed - Unable to resolve name.
 * GoodMatches - Resolved with good matches.
 * PoorComplete - Resolved, but only poor matches found for final
 *		  component of name.
 * PoorPartial - Made partial resolution and made poor matches for next
 *	     	 intermediate component.
 */

typedef enum { Failed, GoodMatches, PoorComplete, PoorPartial } ufnMatchState;
typedef struct _ufn_results {
	ufnMatchState match_status;
	QBool tried_intermediate;

	char *resolved_part;
	char *unresolved_part;

	QCardinal tasks_sent;
	QCardinal tasks_failed;

	QCardinal match_num;
	entryList matches;

	errorList errors;
} ufn_results, *ufnResults;

#define NULLUfnResults (ufnResults) NULL
#define ufn_res_alloc() (ufnResults) smalloc(sizeof(ufn_results))

/*
 *	UFN search record.
 */
typedef struct _ufname_rec {
	namePart name_parts;

	QCardinal tasks_sent;
	QCardinal tasks_failed;

	entryList path;

#define MAX_TASKS_PER_REQ 100

	int dap_exact_task_ids[MAX_TASKS_PER_REQ];
	int dap_approx_task_ids[MAX_TASKS_PER_REQ];

	QCardinal exact_task_count;
	QCardinal approx_task_count;

	QCardinal request_id;
	ufnResults results;
} ufname_rec, *ufnameRec;

#define NULLUfnameRec (ufnameRec) NULL
#define ufname_rec_alloc() (ufnameRec) smalloc(sizeof(ufname_rec))

/* Public procedures */
QE_error_code do_ufn_resolve();

request_state process_ufn_ds_result(), process_ufn_ds_error();
request_state continue_ufn_search();

ufnResults get_ufn_results();
ufnStatus get_ufn_status();
namePart str2ufname();

void ufname_rec_free(), name_part_free();
void add_ufn_path_element();
entryList get_ufn_path();

#endif				/* _query_ufname_h_ */

#endif				/* __QUIPU_UIPS_DOOG_QUERY_UFNAME_H__ */
