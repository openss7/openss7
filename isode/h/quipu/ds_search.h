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

#ifndef __ISODE_QUIPU_DS_SEARCH_H__
#define __ISODE_QUIPU_DS_SEARCH_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* ds_search.h - structures for searching */

/*
 * Header: /xtel/isode/isode/h/quipu/RCS/ds_search.h,v 9.0 1992/06/16 12:23:11 isode Rel
 *
 *
 * Log: ds_search.h,v
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

#ifndef QUIPUSRCH
#define QUIPUSRCH

#include "quipu/config.h"
#include "quipu/commonarg.h"
#include "quipu/ds_error.h"
#include "quipu/dap.h"
#include "quipu/entry.h"

typedef struct {
	AttributeType fi_sub_type;
	AV_Sequence fi_sub_initial;
	AV_Sequence fi_sub_any;
	AV_Sequence fi_sub_final;
	/* initial and final should be zero or */
	/* one components only */
	char *fi_sub_match;		/* for DSA use */
} Filter_Substrings;

struct filter_item {
	int fi_type;
#define FILTERITEM_EQUALITY 1
#define FILTERITEM_SUBSTRINGS 2
#define FILTERITEM_GREATEROREQUAL 3
#define FILTERITEM_LESSOREQUAL 4
#define FILTERITEM_PRESENT 5
#define FILTERITEM_APPROX 6
	union {
		AttributeType fi_un_type;
		AVA fi_un_ava;
		Filter_Substrings fi_un_substrings;
	} fi_un;
	/* field for DSA use - no need to fill is DUA */
	IFP fi_ifp;
};

#define NULLFITEM (struct filter_item *) NULL
#define UNSUB    fi_un.fi_un_substrings
#define UNAVA    fi_un.fi_un_ava
#define UNTYPE   fi_un.fi_un_type
#define filter_item_alloc() (struct filter_item *) smalloc (sizeof (struct filter_item));

typedef struct s_filter {
	int flt_type;
#define FILTER_ITEM 1
#define FILTER_AND 2
#define FILTER_OR 3
#define FILTER_NOT 4
	struct s_filter *flt_next;
	union {
		struct filter_item flt_un_item;
		/* a basic item */
		struct s_filter *flt_un_filter;
		/* or a pointer to a chain of */
		/* filters */
	} flt_un;
} s_filter, *Filter;

#define NULLFILTER (Filter)NULL
#define FUITEM   flt_un.flt_un_item
#define FUFILT   flt_un.flt_un_filter
#define filter_alloc() (Filter) smalloc (sizeof (s_filter));

struct ds_search_arg {
	CommonArgs sra_common;
	DN sra_baseobject;
	int sra_subset;
#define SRA_BASEOBJECT          0
#define SRA_ONELEVEL            1
#define SRA_WHOLESUBTREE        2
	Filter sra_filter;
	char sra_searchaliases;
	char sra_hitalias;		/* DSA internal use only */
	EntryInfoSelection sra_eis;
};

struct ds_search_unit {
	CommonResults srr_common;
	DN srr_object;
	EntryInfo *srr_entries;
	POQ srr_poq;
};

struct ds_search_result {
	char srr_correlated;
	union {
		struct ds_search_unit *srr_unit;
		struct ds_search_result *srr_parts;
	} srr_un;
#define CSR_common	srr_un.srr_unit->srr_common
#define CSR_object	srr_un.srr_unit->srr_object
#define CSR_entries	srr_un.srr_unit->srr_entries
#define CSR_limitproblem	srr_un.srr_unit->srr_poq.poq_limitproblem
#define srr_limitproblem	srr_poq.poq_limitproblem
#define CSR_cr	srr_un.srr_unit->srr_poq.poq_cref
#define srr_cr	srr_poq.poq_cref
	struct ds_search_result *srr_next;
};

#define NULLSRR	((struct ds_search_result *) 0)

/* 
 * the following is used to keep track of results for search acl purposes.
 * one of these exists for each node below the searchbase that contains
 * a search acl (subtree searches only).
 */

typedef struct type_data {
	AttributeType td_type;
	int td_limit;
	char td_nopartial;
	char td_access;
	int td_minkey;
} typedata, *Typedata;

#define NULLTYPEDATA	((Typedata) 0)

typedef struct result_count {
	Entry rc_base;			/* entry this sacl belongs to */
	int rc_count;			/* # results so far */
	int rc_numtypes;		/* number of types following */
	struct type_data *rc_types;	/* really bigger */
} *Rcount;

#define NULLRCOUNT	((Rcount) 0)

/*
 * this structure is used to keep track of the types in a filter, and
 * to record the length of substring type values.  it's easier to do
 * this here than to look through the filter every time.
 */

typedef struct ftype {
	AttributeType ft_type;
	int ft_len;
	int ft_numstrs;			/* number of inequstrs */
	char **ft_inequstrs;		/* for <= and >= */
	struct ftype *ft_next;
} Ftype, *Ftypelist;

#define NULLFTL	((Ftypelist) 0)

/* allows a reference count for things all search tasks reference */
typedef struct thing_header {
	int th_refcount;
	caddr_t th_thing;
} *Thing;

/* following used by search for scheduling */

struct ds_search_task {
	DN st_baseobject;
	DN st_originalbase;
	DN st_alias;
	DN st_bind;
	int st_subset;
	int st_size;
#ifdef TURBO_INDEX
	int st_optimized;
#endif
	struct di_block *st_di;
	struct ds_search_task *st_next;
	ContinuationRef st_cr;
	struct ds_search_task *st_save;
	char st_entryonly;

	/* following is for search acl stuff */

	/* keeps a running count of search results per subtree sacl */
	struct thing_header *st_saclheader;

#define st_sacls	  st_saclheader->th_thing
#define st_saclrefcount	  st_saclheader->th_refcount

	/* unrolled list of types in the search filter */
	struct thing_header *st_ftypeheader;

#define st_ftypes	  st_ftypeheader->th_thing
#define st_ftyperefcount  st_ftypeheader->th_refcount

	Entry st_baseptr;
};

#define NULL_ST ((struct ds_search_task *) NULL)
#define st_alloc() (struct ds_search_task *) smalloc (sizeof(struct ds_search_task));

/* search max 1000 entries before worrying about time limits */
#define SEARCH_DELTA_SIZE 1000

/* character used to mark T.61 strings */
#define T61_MARK '$'

/* used by search to pass info to routines called by avl routines */
struct search_kid_arg {
	EntryInfo **ska_einfo;
	struct ds_search_arg *ska_arg;
	struct ds_search_task **ska_local;
	struct ds_search_task **ska_refer;
	int ska_extent;
	int ska_tmp;
	int ska_domore;
	DN ska_path;
	DN ska_dnend;
	int ska_ismanager;
	int ska_saclerror;
	char ska_authtype;
};

#endif

#endif				/* __ISODE_QUIPU_DS_SEARCH_H__ */
