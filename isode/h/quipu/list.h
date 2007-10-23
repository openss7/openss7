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

#ifndef __ISODE_QUIPU_LIST_H__
#define __ISODE_QUIPU_LIST_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* list.h - */

/*
 * Header: /xtel/isode/isode/h/quipu/RCS/list.h,v 9.0 1992/06/16 12:23:11 isode Rel
 *
 *
 * Log: list.h,v
 * Revision 9.0  1992/06/16  12:23:11  isode
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

#ifndef QUIPULIST
#define QUIPULIST

#include "quipu/commonarg.h"
#include "quipu/ds_error.h"
#include "quipu/dap.h"

struct ds_list_arg {
	CommonArgs lsa_common;
	DN lsa_object;
};

/*
.VE
.VS
*/
struct subordinate {
	RDN sub_rdn;
	char sub_aliasentry;		/* set to TRUE or FALSE */
	char sub_copy;			/* set to TRUE or FALSE */
	struct subordinate *sub_next;
};

#define NULLSUBORD (struct subordinate *) NULL
struct subordinate *subord_comp_new();

/*
.VE
.VS
*/
struct ds_list_result {
	CommonResults lsr_common;
	DN lsr_object;
	time_t lsr_age;
	struct subordinate *lsr_subordinates;
	POQ lsr_poq;
#define lsr_limitproblem lsr_poq.poq_limitproblem
#define lsr_cr lsr_poq.poq_cref
	struct ds_list_result *lsr_next;
};

				/* Note uncorrelated will need to be */
				/* added in to do the secure stuff */
				/* in a distributed manner */
				/* this also applies to search */

struct list_cache {
	DN list_dn;
	struct subordinate *list_subs;
	struct subordinate *list_sub_top;
	int list_count;
	int list_problem;
	struct list_cache *list_next;
};

#define NULLCACHE (struct list_cache *) NULL

struct list_cache *find_list_cache();

#endif

#endif				/* __ISODE_QUIPU_LIST_H__ */
