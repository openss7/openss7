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

/* tsapmisc.c - miscellany tsap functions */

/* 
 * Header: /xtel/isode/isode/tsap/RCS/tsapmisc.c,v 9.0 1992/06/16 12:40:39 isode Rel
 *
 *
 * Log: tsapmisc.c,v
 * Revision 9.0  1992/06/16  12:40:39  isode
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
#include <signal.h>
#include "tpkt.h"
#ifdef SYS5
#include  <termio.h>
#else
#include <sys/ioctl.h>
#endif

/*    estimate of octets that might be returned */

int
TSelectOctets(sd, nbytes, td)
	int sd;
	long *nbytes;
	register struct TSAPdisconnect *td;
{
	int result;
	long value;
	SBV smask;
	register struct tsapblk *tb;

	missingP(nbytes);
	missingP(td);

	smask = sigioblock();

	tsapPsig(tb, sd);

	result = OK;
	if (tb->tb_nreadfnx) {
		if ((result = (*tb->tb_nreadfnx) (tb, &value)) == NOTOK)
			value = 0L;
	} else {
#ifdef	FIONREAD
		if (ioctl(tb->tb_fd, FIONREAD, (char *) &value) == NOTOK)
			value = 0L;
#endif

		switch (tb->tb_flags & (TB_TP0 | TB_TP4)) {
		case TB_TCP:
		case TB_X25:
			if (value > DT_MAGIC && tb->tb_len == 0)
				value -= DT_MAGIC;
			break;

		default:
			break;
		}
	}

	if (result == OK)
		value += (long) tb->tb_len;
	*nbytes = value;

	(void) sigiomask(smask);

	return result;
}

/*    get TSAPs */

int
TGetAddresses(sd, initiating, responding, td)
	int sd;
	struct TSAPaddr *initiating, *responding;
	register struct TSAPdisconnect *td;
{
	SBV smask;
	register struct tsapblk *tb;

	missingP(td);

	smask = sigioblock();

	tsapPsig(tb, sd);

	if (initiating)
		copyTSAPaddrX(&tb->tb_initiating, initiating);
	if (responding)
		copyTSAPaddrX(&tb->tb_responding, responding);

	(void) sigiomask(smask);

	return OK;
}

/*    define transport manager */

#ifdef	MGMT
int
TSetManager(sd, fnx, td)
	int sd;
	IFP fnx;
	register struct TSAPdisconnect *td;
{
	SBV smask;
	register struct tsapblk *tb;

	missingP(td);

	smask = sigioblock();

	tsapPsig(tb, sd);

	tb->tb_manfnx = fnx;

	(void) sigiomask(smask);

	return OK;
}
#endif
