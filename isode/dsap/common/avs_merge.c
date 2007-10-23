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
    "Header: /xtel/isode/isode/dsap/common/RCS/avs_merge.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/avs_merge.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: avs_merge.c,v
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
#include "quipu/dsp.h"		/* for ds_error.h */
#include "quipu/ds_error.h"

extern LLog *log_dsap;

AV_Sequence
avs_merge(a, b)
	AV_Sequence a, b;
{
	register AV_Sequence aptr, bptr, result, trail, tmp;

	if (a == NULLAV)
		return (b);
	if (b == NULLAV)
		return (a);

	/* start sequence off, make sure 'a' is the first */
	switch (avs_cmp_comp(a, b)) {
	default:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("matching failed in avs_merge"));
		/* continue as if equal */

	case 0:		/* equal */
		result = a;
		aptr = a->avseq_next;
		bptr = b->avseq_next;
		avs_comp_free(b);
		break;
	case -1:
		result = b;
		aptr = a;
		bptr = b->avseq_next;
		break;
	case 2:		/* no compare function defined - treat as if a > b */
	case 1:
		result = a;
		aptr = a->avseq_next;
		bptr = b;
		break;
	}

	trail = result;
	while ((aptr != NULLAV) && (bptr != NULLAV)) {

		switch (avs_cmp_comp(aptr, bptr)) {
		default:
			LLOG(log_dsap, LLOG_EXCEPTIONS, ("matching failed in avs_merge (2)"));
			/* continue as if equal */

		case 0:	/* equal */
			trail->avseq_next = aptr;
			trail = aptr;
			tmp = bptr->avseq_next;
			avs_comp_free(bptr);
			aptr = aptr->avseq_next;
			bptr = tmp;
			break;
		case -1:
			trail->avseq_next = bptr;
			trail = bptr;
			bptr = bptr->avseq_next;
			break;
		case 2:	/* no compare function defined - treat as if a > b */
		case 1:
			trail->avseq_next = aptr;
			trail = aptr;
			aptr = aptr->avseq_next;
			break;
		}
	}
	if (aptr == NULLAV)
		trail->avseq_next = bptr;
	else
		trail->avseq_next = aptr;

	return (result);
}

AV_Sequence
avs_fast_merge(a, b, c, d)
	AV_Sequence a, b, c, d;
{
	register AV_Sequence aptr, bptr, result, trail, tmp;

	if (a == NULLAV)
		return (b);
	if (b == NULLAV)
		return (a);

	if (quipu_faststart && (a == c) && d->avseq_next == NULLAV)
#ifndef TURBO_DISK
		if (avs_cmp_comp(d, b) == 1)
#endif
		{
			d->avseq_next = b;
			return a;
		}

	/* start sequence off, make sure 'a' is the first */
	switch (avs_cmp_comp(a, b)) {
	default:
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("matching failed in avs_merge"));
		/* continue as if equal */

	case 0:		/* equal */
		result = a;
		aptr = a->avseq_next;
		bptr = b->avseq_next;
		avs_comp_free(b);
		break;
	case -1:
		result = b;
		aptr = a;
		bptr = b->avseq_next;
		break;
	case 2:		/* no compare function defined - treat as if a > b */
	case 1:
		result = a;
		aptr = a->avseq_next;
		bptr = b;
		break;
	}

	trail = result;
	while ((aptr != NULLAV) && (bptr != NULLAV)) {

		switch (avs_cmp_comp(aptr, bptr)) {
		default:
			LLOG(log_dsap, LLOG_EXCEPTIONS, ("matching failed in avs_merge (2)"));
			/* continue as if equal */

		case 0:	/* equal */
			trail->avseq_next = aptr;
			trail = aptr;
			tmp = bptr->avseq_next;
			avs_comp_free(bptr);
			aptr = aptr->avseq_next;
			bptr = tmp;
			break;
		case -1:
			trail->avseq_next = bptr;
			trail = bptr;
			bptr = bptr->avseq_next;
			break;
		case 2:	/* no compare function defined - treat as if a > b */
		case 1:
			trail->avseq_next = aptr;
			trail = aptr;
			aptr = aptr->avseq_next;
			break;
		}
	}
	if (aptr == NULLAV)
		trail->avseq_next = bptr;
	else
		trail->avseq_next = aptr;

	return (result);
}
