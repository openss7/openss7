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

/* signals.c - signal handling */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/compat/RCS/signals.c,v 9.0 1992/06/16 12:07:00 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/compat/RCS/signals.c,v 9.0 1992/06/16 12:07:00 isode Rel
 *
 *
 * Log: signals.c,v
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

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#ifndef	BADSIG
#define	BADSIG	((void *) -1)
#endif
#include "manifest.h"

int _iosignals_set = 0;

/* Berkeley UNIX: 4.2 */

#if !defined XOS_2
#if defined BSDSIGS

/* Simply including <signal.h> is sufficient for everything but AIX */

#ifdef	AIX			/* #define'd to be _signal */
IFP
signal(sig, func)
	int sig;
	IFP func;
{
	struct sigvec sv1, sv2;

	sv1.sv_handler = func;
	sv1.sv_mask = sv1.sv_onstack = 0;
	return (sigvec(sig, &sv1, &sv2) != NOTOK ? sv2.sv_handler : BADSIG);
}
#endif				/* AIX */

#else				/* defined BSDSIGS */

/* AT&T UNIX: 5 */

/* Probably a race condition or two in this code */

static int blocked = 0;
static int pending = 0;

static void *handler[NSIG];

static RETSIGTYPE
sigser(sig)
	int sig;
{
	(void) signal(sig, sigser);

	pending |= sigmask(sig);
}

#if !defined SVR4_UCB

int
sigblock(mask)
	int mask;
{
	register int sig, smask;
	long omask = blocked;

	(void) rcsid;
	if (mask == 0)
		return blocked;

	for (sig = 1, smask = sigmask(sig); sig < NSIG; sig++, smask <<= 1)
		if ((smask & mask) && !(smask & blocked)) {
			pending &= ~smask;
			handler[sig] = signal(sig, sigser);
			blocked |= smask;
		}

	return omask;
}

int
sigsetmask(mask)
	int mask;
{
	register int sig, smask;
	long omask = blocked;

	for (sig = 1, smask = sigmask(sig); sig < NSIG; sig++, smask <<= 1)
		if (smask & mask) {
			if (smask & blocked)
				continue;

			pending &= ~smask;
			handler[sig] = signal(sig, sigser);
			blocked |= smask;
		} else if (smask & blocked) {
			blocked &= ~smask;
			(void) signal(sig, handler[sig] != BADSIG ? handler[sig] : SIG_DFL);
			if (smask & pending) {
				pending &= ~smask;
				(void) kill(getpid(), sig);
			}
		}

	return omask;
}

#endif				/* !defined SVR4_UCB */
#endif				/* defined BSDSIGS */
#endif				/* !defined XOS_2 */

static inline void
dummy(void)
{
	(void) rcsid;
}
