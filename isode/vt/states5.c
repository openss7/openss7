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

/* states5.c - VTPM: FSM sector 5 states */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/vt/RCS/states5.c,v 9.0 1992/06/16 12:41:08 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/vt/RCS/states5.c,v 9.0 1992/06/16 12:41:08 isode Rel
 *
 *
 * Log: states5.c,v
 * Revision 9.0  1992/06/16  12:41:08  isode
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

#include "vtpm.h"

#define	undefined(s1, e1) \
	adios (NULLCP, \
	      "undefined state/event: sector is 5, state is %s, event is %d", \
	       s1, e1)

int
s5_400B(event, pe)			/* sector 5, state 400B */
	int event;
	PE pe;
{
	switch (event) {
	case DLQ:
		return (a5_35(pe));
	case NDQ_ntr:
		return (a5_3(pe));
	case NDQ_tr:
		return (a5_2(pe));
	case UDQ:
		return (a5_34(pe));
	case HDQ:
		return (a5_106(pe));
	case VDATreq_h:
		return (a5_11(pe));
	case VDATreq_u:
		return (a5_28(pe));
	case RLQ:
		return (a5_38(pe));
	case BKQ:
		return (a5_32(pe));
	case VDATreq_n:
		return (a5_1(pe));
	case VDELreq:
		return (a5_9(pe));
	case VRELreq:
		return (a5_17(pe));
	case VBRKreq:
		return (a5_5(pe));
	default:
		undefined("400B", event);	/* NOTREACHED */
	}
}

/* ARGSUSED */
int
s5_402B(event, pe)
	int event;
	PE pe;
{
	undefined("402B", event);	/* NOTREACHED */
}

/* ARGSUSED */
int
s5_420B(event, pe)
	int event;
	PE pe;
{
	undefined("420B", event);	/* NOTREACHED */
}

/* ARGSUSED */
int
s5_422B(event, pe)			/* sector 5, state 422B */
	int event;
	PE pe;
{
	undefined("422B", event);	/* NOTREACHED */
}

/* ARGSUSED */
int
s5_40N(event, pe)
	int event;
	PE pe;
{
	undefined("40N", event);	/* NOTREACHED */
}

/* ARGSUSED */
int
s5_40T(event, pe)
	int event;
	PE pe;
{
	undefined("40T", event);	/* NOTREACHED */
}

/* ARGSUSED */
int
s5_42T(event, pe)
	int event;
	PE pe;
{
	undefined("42T", event);	/* NOTREACHED */
}

/* ARGSUSED */
int
s5_42N(event, pe)
	int event;
	PE pe;
{
	undefined("42N", event);	/* NOTREACHED */
}

s5_61(event, pe)
	int event;
	PE pe;
{
	switch (event) {
	case BKR:
		return (a5_31(pe));
	default:
		undefined("61", event);	/* NOTREACHED */
	}
}
int
s5_62(event, pe)
	int event;
	PE pe;
{
	switch (event) {
	case VBRKrsp:
		return (a5_6(pe));
	default:
		undefined("62", event);	/* NOTREACHED */
	}
}
