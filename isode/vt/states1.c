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

/* states1.c - VTPM: FSM sector 1 states */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/vt/RCS/states1.c,v 9.0 1992/06/16 12:41:08 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/vt/RCS/states1.c,v 9.0 1992/06/16 12:41:08 isode Rel
 *
 *
 * Log: states1.c,v
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

#define	undefined(s1,e1) \
	adios (NULLCP, \
	      "undefined state/event: sector is 1, state is %s, event is %d", \
	       s1, e1)

int
s1_01(event, pe)			/* sector 1, state 01 */
	int event;
	PE pe;
{
	switch (event) {
	case ASQ:
		return (a1_17(pe));
	case VASSreq:
		return (a1_2(pe));
	case APQ:
		return (a1_107(pe));
	case AUQ:
		return (a1_107(pe));
	case PAB:
		return (a1_107(pe));
	case VTAB:
		return (a1_107(pe));
	case VUABreq:
		return (a1_107(pe));
	default:
		undefined("01", event);	/* NOTREACHED */
	}
}

int
s1_02B(event, pe)
	int event;
	PE pe;
{
	switch (event) {
	case ASR:
		return (a1_15(pe));
	case APQ:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case AUQ:
		a1_101(pe);
		return (OK);	/* NOTREACHED */
	case PAB:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case VTAB:
		a1_103(pe);
		return (OK);	/* NOTREACHED */
	case VUABreq:
		a1_102(pe);
		return (OK);	/* NOTREACHED */
	default:
		undefined("02B", event);	/* NOTREACHED */
	}
}

int
s1_02S(event, pe)
	int event;
	PE pe;
{
	switch (event) {
	case ASR:
		return (a1_16(pe));
	case APQ:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case AUQ:
		a1_101(pe);
		return (OK);	/* NOTREACHED */
	case PAB:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case VTAB:
		a1_103(pe);
		return (OK);	/* NOTREACHED */
	case VUABreq:
		a1_102(pe);
		return (OK);	/* NOTREACHED */
	default:
		undefined("02S", event);	/* NOTREACHED */
	}
}

int
s1_03B(event, pe)
	int event;
	PE pe;
{
	switch (event) {
	case VASSrsp:
		return (a1_3(pe));
	case APQ:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case AUQ:
		a1_101(pe);
		return (OK);	/* NOTREACHED */
	case PAB:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case VTAB:
		a1_103(pe);
		return (OK);	/* NOTREACHED */
	case VUABreq:
		a1_102(pe);
		return (OK);	/* NOTREACHED */
	default:
		undefined("03B", event);	/* NOTREACHED */
	}
}

int
s1_03S(event, pe)
	int event;
	PE pe;
{
	switch (event) {
	case VASSrsp:
		return (a1_4(pe));
	case APQ:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case AUQ:
		a1_101(pe);
		return (OK);	/* NOTREACHED */
	case PAB:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case VTAB:
		a1_103(pe);
		return (OK);	/* NOTREACHED */
	case VUABreq:
		a1_102(pe);
		return (OK);	/* NOTREACHED */
	default:
		undefined("03S", event);	/* NOTREACHED */
	}
}

int
s1_10B(event, pe)
	int event;
	PE pe;
{
	switch (event) {
	case GTQ:
		return (a1_107(pe));
	case RLQ:
		return (a1_25(pe));
	case RTQ:
		return (a1_5(pe));
	case VRELreq:
		return (a1_7(pe));
	case SNQ:
		return (a1_29(pe));
	case SPQ:
		return (a1_30(pe));
	case VSNEGreq:
		return (a1_11(pe));
	case VSWPreq:
		return (a1_13(pe));
	case APQ:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case AUQ:
		a1_101(pe);
		return (OK);	/* NOTREACHED */
	case PAB:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case VTAB:
		a1_103(pe);
		return (OK);	/* NOTREACHED */
	case VUABreq:
		a1_102(pe);
		return (OK);	/* NOTREACHED */
	default:
		undefined("10B", event);	/* NOTREACHED */
	}
}

int
s1_10N(event, pe)
	int event;
	PE pe;
{
	switch (event) {
	case GTQ:
		return (a1_19(pe));
	case RLQ:
		return (a1_26(pe));
	case SNQ:
		return (a1_28(pe));
	case SPQ:
		return (a1_30(pe));
	case VRQTreq:
		return (a1_10(pe));
	case APQ:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case AUQ:
		a1_101(pe);
		return (OK);	/* NOTREACHED */
	case PAB:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case VTAB:
		a1_103(pe);
		return (OK);	/* NOTREACHED */
	case VUABreq:
		a1_102(pe);
		return (OK);	/* NOTREACHED */
	default:
		undefined("10N", event);	/* NOTREACHED */
	}
}

int
s1_10T(event, pe)
	int event;
	PE pe;
{
	switch (event) {
	case RTQ:
		return (a1_27(pe));
	case VGVTreq:
		return (a1_6(pe));
	case VRELreq:
		return (a1_8(pe));
	case VRQTreq:
		return (a1_19(pe));
	case VSNEGreq:
		return (a1_12(pe));
	case VSWPreq:
		return (a1_14(pe));
	case APQ:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case AUQ:
		a1_101(pe);
		return (OK);	/* NOTREACHED */
	case PAB:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case VTAB:
		a1_103(pe);
		return (OK);	/* NOTREACHED */
	case VUABreq:
		a1_102(pe);
		return (OK);	/* NOTREACHED */
	default:
		undefined("10T", event);	/* NOTREACHED */
	}
}

int
s1_50B(event, pe)
	int event;
	PE pe;
{
	switch (event) {
	case DLQ:
		return (a1_21(pe));
	case GTQ:
		return (a1_7(pe));
	case NDQ_ntr:
		return (a1_0(pe));
	case NDQ_tr:
		return (a1_1(pe));
	case RLQ:
		return (a1_22(pe));
	case SNQ:
		return (a1_23(pe));
	case SPQ:
		return (a1_24(pe));
	case UDQ:
		return (a1_18(pe));
	case APQ:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case AUQ:
		a1_101(pe);
		return (OK);	/* NOTREACHED */
	case PAB:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case VTAB:
		a1_103(pe);
		return (OK);	/* NOTREACHED */
	case VUABreq:
		a1_102(pe);
		return (OK);	/* NOTREACHED */
	default:
		undefined("50B", event);	/* NOTREACHED */
	}
}

int
s1_51Q(event, pe)
	int event;
	PE pe;
{
	switch (event) {
	case DLQ:
		return (a1_21(pe));
	case NDQ_ntr:
		return (a1_0(pe));
	case NDQ_tr:
		return (a1_1(pe));
	case RLR:
		return (a1_20(pe));
	case RTQ:
		return (a1_107(pe));
	case APQ:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case AUQ:
		a1_101(pe);
		return (OK);	/* NOTREACHED */
	case PAB:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case VTAB:
		a1_103(pe);
		return (OK);	/* NOTREACHED */
	case VUABreq:
		a1_102(pe);
		return (OK);	/* NOTREACHED */
	default:
		undefined("51Q", event);	/* NOTREACHED */
	}
}

int
s1_51R(event, pe)
	int event;
	PE pe;
{
	switch (event) {
	case VRELrsp:
		return (a1_9(pe));
	case APQ:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case AUQ:
		a1_101(pe);
		return (OK);	/* NOTREACHED */
	case PAB:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case VTAB:
		a1_103(pe);
		return (OK);	/* NOTREACHED */
	case VUABreq:
		a1_102(pe);
		return (OK);	/* NOTREACHED */
	default:
		undefined("51R", event);	/* NOTREACHED */
	}
}

int
s1_51N(event, pe)
	int event;
	PE pe;
{
	switch (event) {
	case VRELrsp:
		return (a1_9(pe));
	case APQ:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case AUQ:
		a1_101(pe);
		return (OK);	/* NOTREACHED */
	case PAB:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case VTAB:
		a1_103(pe);
		return (OK);	/* NOTREACHED */
	case VUABreq:
		a1_102(pe);
		return (OK);	/* NOTREACHED */
	default:
		undefined("51N", event);	/* NOTREACHED */
	}
}

int
s1_51T(event, pe)
	int event;
	PE pe;
{
	switch (event) {
	case RLR:
		return (a1_20(pe));
	case RTQ:
		return (a1_0(pe));
	case UDQ:
		return (a1_18(pe));
	case APQ:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case AUQ:
		a1_101(pe);
		return (OK);	/* NOTREACHED */
	case PAB:
		a1_100(pe);
		return (OK);	/* NOTREACHED */
	case VTAB:
		a1_103(pe);
		return (OK);	/* NOTREACHED */
	case VUABreq:
		a1_102(pe);
		return (OK);	/* NOTREACHED */
	default:
		undefined("51T", event);	/* NOTREACHED */
	}
}
