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

/* pe_alloc.c - allocate a presentation element */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/pe_alloc.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/pe_alloc.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: pe_alloc.c,v
 * Revision 9.0  1992/06/16  12:25:44  isode
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
#include "psap.h"
#include "tailor.h"

/* A Presentation Element (or PElement) is an internal representation for
   a presentation type from ISO8825.  The fields of the structure are:

	pe_class:	UNIVersal, APPLication, CONText, or PRIVate
	pe_form:	PRIMative, CONStructor, InlineCONStructor
	pe_id:		identifier

	pe_len:		if a PRIMative, then the length of pe_prim,
			else a scratch value; "indefinite" length elements
			have a pe_len of -1 (PE_LEN_INDF)
	pe_ilen:	if an InlineCONStructor, then the offset to the real
			data portion

	pe_prim:	if a PRIMative or an Inline CONStructor, the
			byte-string
	pe_cons:	if a CONStructor, the first element in the
			singly-linked list of elements

	pe_next:	if the immediate parent is a constructor, the
			next element in the singly-linked list of elements

	pe_cardinal:	if a LIST (SET or SEQ CONStructor), the cardinality
			of the list
	pe_offset:	if a member of a SEQ LIST, the offset in the SEQUENCE

	pe_nbits:	if a BITSTRING, the number of bits in the string

	pe_refcnt:	a hack for ANYs in pepy
 */

#define PE_LIST_CNT 100

int pe_allocs;
int pe_frees;
int pe_most;

PE pe_list = NULLPE;

#ifdef	DEBUG
PE pe_active = NULLPE;
#endif

/*  */

PE
pe_alloc(class, form, id)
	PElementClass class;
	PElementForm form;
	PElementID id;
{
	register int i;
	register PE pe;

	if (pe = pe_list)
		pe_list = pe->pe_next;
	else {
		pe_list = (pe = (PE) calloc(PE_LIST_CNT, sizeof *pe));
		if (pe == NULLPE)
			return NULLPE;

		for (i = 0; i < (PE_LIST_CNT - 1); i++, pe++)
			pe->pe_next = pe + 1;

		pe = pe_list;
		pe_list = pe->pe_next;
	}

	bzero((char *) pe, sizeof *pe);
	pe->pe_class = class;
	pe->pe_form = form;
	pe->pe_id = id;

	if ((i = ++pe_allocs - pe_frees) > pe_most)
		pe_most = i;
#ifdef	DEBUG
	if (psap_log->ll_events & LLOG_DEBUG) {
		pe->pe_link = pe_active;
		pe_active = pe;
	}
#endif

	return pe;
}
