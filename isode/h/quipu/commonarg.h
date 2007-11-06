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

#ifndef __ISODE_QUIPU_COMMONARG_H__
#define __ISODE_QUIPU_COMMONARG_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* commonarg.h - directory operation common arguments */

/*
 * Header: /xtel/isode/isode/h/quipu/RCS/commonarg.h,v 9.0 1992/06/16 12:23:11 isode Rel
 *
 *
 * Log: commonarg.h,v
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

#ifndef COMMONARG
#define COMMONARG

#include "quipu/attrvalue.h"
#include "quipu/dsp.h"
#include "quipu/authen.h"

#define OP_READ 1
#define OP_COMPARE 2
#define OP_ABANDON 3
#define OP_LIST 4
#define OP_SEARCH 5
#define OP_ADDENTRY 6
#define OP_REMOVEENTRY 7
#define OP_MODIFYENTRY 8
#define OP_MODIFYRDN 9
#define OP_GETEDB 10

struct security_parms {
	struct certificate_list *sp_path;
	DN sp_name;
	char *sp_time;
	struct random_number *sp_random;
	int sp_target;
};

typedef struct extension {
	int ext_id;
	char ext_critical;
	PE ext_item;
	struct extension *ext_next;
} *Extension;

#define NULLEXT		((Extension) 0)
#define ext_alloc()	(Extension) smalloc(sizeof(struct extension))

typedef struct common_args {		/* Common arguments for operations */
	ServiceControl ca_servicecontrol;
	DN ca_requestor;
	struct op_progress ca_progress;
	int ca_aliased_rdns;
#define CA_NO_ALIASDEREFERENCED -1
	struct security_parms *ca_security;
	struct signature *ca_sig;
	struct extension *ca_extensions;
} common_args, CommonArgs;

#define NULL_COMMONARG ((struct common_args *) NULL)

typedef struct common_results {
	DN cr_requestor;
	/* Secuity stuff to go here.  The */
	/* is not relevant until this is added */
	char cr_aliasdereferenced;
	/* set to TRUE or FALSE */

	/* essentially for pepsy, but may be used for real later */
	struct security_parms *cr_security;
	struct alg_id *cr_alg;
	char *cr_tmp;
	int cr_len;
} common_results, CommonResults;

typedef struct entrystruct {		/* Represents EntryInformation */
	DN ent_dn;
	Attr_Sequence ent_attr;
	char ent_iscopy;
#define INFO_MASTER 0x001
#define INFO_COPY   0x002
	/* This is the only info derivable by */
	/* protocol */
	/* INCA also distingusihes local cached */
#define INFO_CACHE 0x003
	char ent_pepsycopy;
	time_t ent_age;
	/* age of chaced info */
	struct entry *ent_eptr;
	/* for search acl purposes... yuck! */
	struct entrystruct *ent_next;
} entrystruct, EntryInfo;

#define NULLENTRYINFO ((EntryInfo *) 0)
#define entryinfo_alloc()          (EntryInfo *) smalloc(sizeof(EntryInfo))
#define entryinfo_cmp(x,y)          (((dn_cmp (x.ent_dn ,y.ent_dn) == OK) && (as_cmp (x.ent_attr ,y.ent_attr) == OK)) ? OK : NOTOK)

typedef struct entryinfoselection {
	/* Rerpesents EntryInformationSelection */
	char eis_allattributes;
	/* if set to TRUE, all attributes */
	/* returned, if not as per next arg */
	Attr_Sequence eis_select;
	/* Sequence of attributes used to show */
	/* which TYPES are wanted */
	int eis_infotypes;
#define EIS_ATTRIBUTETYPESONLY 0
#define EIS_ATTRIBUTESANDVALUES 1
} entryinfoselection, EntryInfoSelection;

#define LSR_NOLIMITPROBLEM      -1
#define LSR_TIMELIMITEXCEEDED   0
#define LSR_SIZELIMITEXCEEDED   1
#define LSR_ADMINSIZEEXCEEDED   2
typedef struct part_outcome {
	int poq_limitproblem;
	ContinuationRef poq_cref;
	char poq_no_ext;
} POQ;

#endif

#endif				/* __ISODE_QUIPU_COMMONARG_H__ */
