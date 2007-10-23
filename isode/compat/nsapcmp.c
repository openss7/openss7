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

/* nsapcmp.c - compare NSAPs */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/compat/RCS/nsapcmp.c,v 9.0 1992/06/16 12:07:00 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/compat/RCS/nsapcmp.c,v 9.0 1992/06/16 12:07:00 isode Rel
 *
 *
 * Log: nsapcmp.c,v
 * Revision 9.0  1992/06/16  12:07:00  isode
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
#include "general.h"
#include "manifest.h"
#include "isoaddrs.h"

/*  */

int
nsap_addr_cmp(na1, na2)
	struct NSAPaddr *na1, *na2;
{
	if (na1->na_stack != na2->na_stack)
		return NOTOK;

	switch (na1->na_stack) {
	case NA_NSAP:
		if (na1->na_addrlen == 0 ||
		    (na1->na_addrlen == na2->na_addrlen &&
		     bcmp(na1->na_address, na2->na_address, na1->na_addrlen) == 0))
			return OK;
		break;

	case NA_TCP:
		if (na1->na_port == na2->na_port && strcmp(na1->na_domain, na2->na_domain) == 0)
			return OK;
		break;

	case NA_X25:
		if (na1->na_dtelen == na2->na_dtelen &&
		    bcmp(na1->na_dte, na2->na_dte,
			 na1->na_dtelen) == 0 &&
		    na1->na_pidlen == na2->na_pidlen &&
		    bcmp(na1->na_pid, na2->na_pid, na1->na_pidlen) == 0)
			return OK;
		break;
	}
	return NOTOK;
}

int
tsap_addr_cmp(ta, tb)
	struct TSAPaddr *ta, *tb;
{
	struct NSAPaddr *na1, *na2;
	int n1, n2;

	if (ta->ta_selectlen != tb->ta_selectlen ||
	    bcmp(ta->ta_selector, tb->ta_selector, ta->ta_selectlen) != 0)
		return NOTOK;
	if (ta->ta_naddr == 0 && tb->ta_naddr == 0)
		return OK;
	for (na1 = ta->ta_addrs, n1 = ta->ta_naddr; n1-- > 0; na1++) {
		for (na2 = tb->ta_addrs, n2 = tb->ta_naddr; n2-- > 0; na2++)
			if (nsap_addr_cmp(na1, na2) == OK)
				return OK;
	}
	return NOTOK;
}

int
ssap_addr_cmp(sa, sb)
	struct SSAPaddr *sa, *sb;
{
	if (sa->sa_selectlen != sb->sa_selectlen ||
	    bcmp(sa->sa_selector, sb->sa_selector, sa->sa_selectlen) != 0)
		return NOTOK;
	return tsap_addr_cmp(&sa->sa_addr, &sb->sa_addr);
}

int
psap_addr_cmp(pa, pb)
	struct PSAPaddr *pa, *pb;
{
	if (pa->pa_selectlen != pb->pa_selectlen ||
	    bcmp(pa->pa_selector, pb->pa_selector, pa->pa_selectlen) != 0)
		return NOTOK;
	return ssap_addr_cmp(&pa->pa_addr, &pb->pa_addr);
}
