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

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/rdn_cpy.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/rdn_cpy.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: rdn_cpy.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
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
#include "quipu/name.h"
#include "quipu/malloc.h"

extern LLog *log_dsap;

RDN
rdn_comp_cpy(rdn)
	RDN rdn;
{
	register RDN ptr;
	unsigned last_heap;

	if (rdn == NULLRDN) {
		return (NULLRDN);
	}

	ptr = rdn_comp_alloc();
	ptr->rdn_at = AttrT_cpy(rdn->rdn_at);

	if ((last_heap = mem_heap) == 1)
		mem_heap = 2 + attr_index;

	AttrV_cpy_aux(&rdn->rdn_av, &ptr->rdn_av);
	ptr->rdn_next = NULLRDN;

	mem_heap = last_heap;

	return (ptr);
}

RDN
rdn_cpy(rdn)
	RDN rdn;
{
	RDN start;
	register RDN eptr;
	register RDN ptr, ptr2;

	if (rdn == NULLRDN) {
		return (NULLRDN);
	}
	start = rdn_comp_cpy(rdn);
	ptr2 = start;

	for (eptr = rdn->rdn_next; eptr != NULLRDN; eptr = eptr->rdn_next) {
		ptr = rdn_comp_cpy(eptr);
		ptr2->rdn_next = ptr;
		ptr2 = ptr;
	}
	return (start);
}
