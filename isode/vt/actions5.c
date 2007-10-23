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

/* actions5.c - VTPM: FSM sector 5 actions */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/vt/RCS/actions5.c,v 9.0 1992/06/16 12:41:08 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/vt/RCS/actions5.c,v 9.0 1992/06/16 12:41:08 isode Rel
 *
 *
 * Log: actions5.c,v
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
#include "sector1.h"

/************************************************************************/
/*	This file contains the functions that are executed when the	*/
/*	VT Protocol machine is in a Sector 5 state and a protocol	*/
/*	event occurs.  The state transition matrix is specified in	*/
/*	Table 32 of ISO 9041 (July 1987 version).  The actions which	*/
/*	this collection of functions perform are specified in Table 40	*/
/*	of ISO 9041.							*/
/************************************************************************/

extern int sd;				/* Global Session Descriptor (ISODE) */

		/* xx1x xxxx = awaiting ack from peer i.e., 420 */
		/* xxxx xx1x = awaiting ack from user */

		/* T = got token, N = no got token */

		/* 
		   req: usr==>vtpm ind: vtpm==>usr */
int
ce_104(pe)				/* common event 104 */
	PE pe;
{
	/* if (vnt > 0) */
	if (pe != NULLPE)
		vdatind(SEQUENCED, pe);
	vnt = 0;
	return (OK);
}

int
ce_105()
{				/* common event 105 */
	/* if (vns > 0) for(... */
	if (p_ondq != NULLPE)
		(void) p_data(p_ondq);	/* send NDQ */
	vns = 0;
	return (OK);
}

/* ARGSUSED */
int
a5_0(pe)
		    /* VDATreq-sqtr in states 400B or 402B */
 /* V data request addressing sequenced trigger co */
	PE pe;
{
	return (ce_105());
/*
	==> SAMESTATE;
*/
}

/* ARGSUSED */
int
a5_1(pe)
		    /* VDATreq-n in states 400B, 402B or 40T */
 /* V data request addressing sequenced trigger co */
	PE pe;
{

	/* 
	   vns++; ==> SAMESTATE */
	return (ce_105());	/* Autonomous Event to Ship it */
}

int
a5_2(pe)				/* NDQ-tr in states 400B, 420B */
	PE pe;
{
	/* 
	   vnt++; */

	return (ce_104(pe));
	/* 
	   ==> SAMESTATE */
}

int
a5_3(pe)				/* NDQ-ntr in states 400B, 420B */
	PE pe;
{
	/* 
	   vnt++; */
	/* 
	   ==> SAMESTATE */
	return (ce_104(pe));	/* Autonomous Event to Deliver to User */
}

int
a5_5(pe)				/* VBRKreq */
	PE pe;
{
	vtok = 0;		/* giving the token away */
	vnt = 0;
	vns = 0;
	/* vtkp was set in vbrkreq so it could be coded in to the pe */
	(void) p_resync_req(pe, SYNC_RESTART);	/* send break request */
	state = S5_61;
	return (OK);
}

int
a5_6(pe)				/* VBRKrsp in state 62 */
	PE pe;
{
	(void) p_resync_resp(pe);	/* send out break response */
	if (vsmd && vtok)
		state = S5_40T;
	else if (vsmd)
		state = S5_40N;
	else {
		vtkp = INITIATOR;
		if (vtok)
			vtkp = ACCEPTOR;
		state = S5_400B;
	}
	return (OK);
}

int
a5_9(pe)				/* VDELreq in states 400B, 402B */
	PE pe;
{
	if (dcno) {		/* no delivery control */
		advise(LLOG_DEBUG, NULLCP, "a5_9: dcno hasn't been set");
		/* ==> SAMESTATE */
		return (NOTOK);
	}
	(void) ce_105();
	/* send out dlq */
	/* this will be replace by the new-fangled pepy schtuff; will use this now for
	   compatability */

	(void) p_data(pe);
	state = (vra) ? state + 2 : state;	/* pretty neeto eh? */
	return (OK);
}

int
a5_11(pe)				/* HDQ request in 400B */
	PE pe;
{
	(void) p_typed_data(pe);
	return (OK);
}

 /*ARGSUSED*/ int
a5_17(pe)				/* VRELreq in states 400B */
	PE pe;
{
/*	ce_105(); */
	sector = 1;
	if (vtok) {
		state = S1_51Q;	/* Must change state first because vt_disconnect gets RLR & calls
				   state machine again. */
		vt_disconnect();	/* May be only TEMP */
	} else {
		request_token();
		/* Need call to ISODE to request token */
		state = S1_50B;
	}

	return (OK);
}

int
a5_28(pe)				/* UDQ request in 400B */
	PE pe;
{
	(void) p_typed_data(pe);
	return (OK);
}

int
a5_31(pe)				/* BKR in 61 */
	PE pe;
{
	if (vsmd && vtok)
		state = S5_40T;
	else if (vsmd)
		state = S5_40N;
	else
		state = S5_400B;
	vbrkcnf(pe);
	return (OK);
}

int
a5_32(pe)				/* BKQ could occur in any state except 62 */
	PE pe;
{
	vnt = 0;
	vns = 0;
	/* 
	   vbrkind clears queues etc. and then map the break character to user and sets vtok to 1
	   (we should have received the token) */
	state = S5_62;
	vbrkind(pe);
	return (OK);
}

int
a5_34(pe)				/* UDQ in 400B */
	PE pe;
{
	if (pe != NULLPE)
		vudatind(pe);
	return (OK);
}

int
a5_35(pe)				/* DEL in states 400B, 420B */
	PE pe;
{

	if ((vra = prim2flag(pe)) == NOTOK)
		adios("a5_35: bogus PDU (%s)", pe_error(pe->pe_errno));
	(void) ce_104(NULLPE);
	vdelind(pe, vra);
	state = (vra) ? state + 2 : state;
	return (OK);
}

int
a5_38(pe)				/* RLQ in states 400B */
	PE pe;
{

	(void) ce_104(pe);
	sector = 1;
	state = S1_51R;
	(void) vrelind();
	return (OK);
}

int
a5_106(pe)
	PE pe;
{
	if (pe != NULLPE)
		vhdatind(pe);
	return (OK);
}
