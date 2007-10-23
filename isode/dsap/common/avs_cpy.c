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
    "Header: /xtel/isode/isode/dsap/common/RCS/avs_cpy.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/avs_cpy.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: avs_cpy.c,v
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
#include "quipu/attrvalue.h"

extern LLog *log_dsap;

AV_Sequence
avs_comp_cpy(avs)
	AV_Sequence avs;
{
	AV_Sequence ptr;

	if (avs == NULLAV) {
		return (NULLAV);
	}
	ptr = (AV_Sequence) smalloc(sizeof(avseqcomp));
	AttrV_cpy_aux(&avs->avseq_av, &ptr->avseq_av);
	ptr->avseq_next = NULLAV;
	return (ptr);
}

static AV_Sequence
avs_comp_cpy_enc(avs)
	AV_Sequence avs;
{
	AV_Sequence ptr;

	if (avs == NULLAV) {
		return (NULLAV);
	}
	ptr = (AV_Sequence) smalloc(sizeof(avseqcomp));
	AttrV_cpy_enc(&avs->avseq_av, &ptr->avseq_av);
	ptr->avseq_next = NULLAV;
	return (ptr);
}

AV_Sequence
avs_cpy(avs)
	AV_Sequence avs;
{
	AV_Sequence start;
	AV_Sequence ptr, ptr2;
	register AV_Sequence eptr;

	if (avs == NULLAV) {
		return (NULLAV);
	}
	start = avs_comp_cpy(avs);
	ptr2 = start;

	for (eptr = avs->avseq_next; eptr != NULLAV; eptr = eptr->avseq_next) {
		ptr = avs_comp_cpy(eptr);
		ptr2->avseq_next = ptr;
		ptr2 = ptr;
	}
	return (start);
}

AV_Sequence
avs_cpy_enc(avs)
	AV_Sequence avs;
{
	AV_Sequence start;
	AV_Sequence ptr, ptr2;
	register AV_Sequence eptr;

	/* an AVS, and encode at the same time */

	if (avs == NULLAV) {
		return (NULLAV);
	}
	start = avs_comp_cpy_enc(avs);
	ptr2 = start;

	for (eptr = avs->avseq_next; eptr != NULLAV; eptr = eptr->avseq_next) {
		ptr = avs_comp_cpy_enc(eptr);
		ptr2->avseq_next = ptr;
		ptr2 = ptr;
	}
	return (start);
}
