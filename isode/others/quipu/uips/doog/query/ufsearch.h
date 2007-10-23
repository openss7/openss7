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

#ifndef __QUIPU_UIPS_DOOG_QUERY_UFSEARCH_H__
#define __QUIPU_UIPS_DOOG_QUERY_UFSEARCH_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

#ifndef _query_ufsearch_h_
#define _query_ufsearch_h_

#include "types.h"
#include "util.h"
#include "dn_list.h"
#include "sequence.h"
#include "error.h"

typedef struct _search_pair {
	AttributeType type;
	stringList values;
	QBool use_subtree;
	QBool checked;
	struct _search_pair *next;
} search_pair, *searchPair, *searchPairList;

#define NULLSearchPair (searchPair) NULL
#define search_pair_alloc() (searchPair) smalloc(sizeof(search_pair))

typedef enum { UFS_Succeeded, UFS_Failed } ufsearchState;
typedef struct _ufsearch_result {
	ufsearchState search_status;

	QCardinal tasks_sent;
	QCardinal tasks_failed;

	entryList hits;
	QCardinal hit_num;

	errorList errors;
} ufsearch_result, *ufsearchResult;

#define NULLUfsearchResult (ufsearchResult) NULL
#define ufsearch_res_alloc() (ufsearchResult) smalloc(sizeof(ufsearch_result))

typedef struct _ufsearch_rec {
	searchPairList search_data;
	searchPair target_data;

	QBool tried_target_search;
	QBool trying_target_search;

	QCardinal tasks_sent;
	QCardinal tasks_failed;

#define MaxUfsearchTasks 128
	int exact_task_ids[MaxUfsearchTasks];
	int approx_task_ids[MaxUfsearchTasks];

	QCardinal exact_task_count;
	QCardinal approx_task_count;

	entryList followed;
	entryList to_follow;

	entryList path;

	QCardinal to_follow_num;

	QCardinal request_id;
	ufsearchResult result;
} ufsearch_rec, *ufsearchRec;

#define NULLUfsearchRec (ufsearchRec) NULL
#define ufsearch_rec_alloc() (ufsearchRec) smalloc(sizeof(ufsearch_rec))

/* Public procedures */
QE_error_code do_ufsearch();

request_state process_ufs_ds_result();
request_state process_ufs_ds_error();

ufsearchResult get_ufsearch_result();

void ufsearch_rec_free(), ufsearch_result_free();
void search_pair_list_free();

#endif

#endif				/* __QUIPU_UIPS_DOOG_QUERY_UFSEARCH_H__ */
