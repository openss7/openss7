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

/* audio.c - play a sound */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/audio.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/common/RCS/audio.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: audio.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
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

#include <signal.h>
#include "quipu/util.h"
#include "quipu/attr.h"
#include "psap.h"
#include <errno.h>
#ifndef	SVR4
#include <sys/wait.h>
#endif

#ifndef BINPATH
#define BINPATH "/usr/local/bin/"
#endif

/* Assumes the 'play' routine from Mike (bauer@cns.ucalgary.ca) is
   installed in BINPATH directory.
*/

/* Use the {FILE} mechanism to pull in the sound files */

extern r_octprint();
extern PE r_octenc();
extern struct qbuf *r_octsdec();
extern struct qbuf *r_octparse();
extern struct qbuf *qb_cpy();
extern qb_cmp();
extern int file_attr_length;

audio_print(ps, qb, format)
	PS ps;
	struct qbuf *qb;
	int format;
{
	int pd[2];
	char buffer[LINESIZE];
	char execvector[LINESIZE];
	struct qbuf *p;

	int pid;
	int childpid;

#ifdef SVR4
	int status;
#else
	union wait status;
#endif

	SFP pstat;

	if (format != READOUT) {
		for (p = qb->qb_forw; p != qb; p = p->qb_forw)
			(void) ps_write(ps, (PElementData) p->qb_data, p->qb_len);
		return;
	}
#if	!defined(sparc) || !defined(SUNOS41)
	ps_print(ps, "(No audio process defined)");
#else
	if (pipe(pd) == -1) {
		ps_print(ps, "ERROR: could not create pipe");
		return;
	}

	pstat = signal(SIGPIPE, SIG_IGN);

	switch (childpid = fork()) {

	case -1:
		(void) close(pd[1]);
		(void) close(pd[0]);
		(void) signal(SIGPIPE, pstat);
		ps_print(ps, "ERROR: could not fork");
		return;

	case 0:
		(void) signal(SIGPIPE, pstat);

		if (dup2(pd[0], 0) == -1)
			_exit(-1);
		(void) close(pd[0]);
		(void) close(pd[1]);

		(void) sprintf(execvector, "%splay", BINPATH);

		(void) execl(execvector, execvector, NULLCP);

		while (read(0, buffer, sizeof buffer) > 0)
			continue;
		(void) printf("ERROR: can't execute '%s'", execvector);

		(void) fflush(stdout);
		/* safety catch */
		_exit(-1);
		/* NOTREACHED */

	default:
		(void) close(pd[0]);
		for (p = qb->qb_forw; p != qb; p = p->qb_forw) {
			if (write(pd[1], p->qb_data, p->qb_len) != p->qb_len) {
				(void) close(pd[1]);
				(void) signal(SIGPIPE, pstat);
				ps_print(ps, "ERROR: write error");
				return;
			}
		}
		(void) close(pd[1]);
		ps_printf(ps, "%splay invoked", BINPATH);

#ifdef SVR4
		while ((pid = wait(&status)) != NOTOK
#else
		while ((pid = wait(&status.w_status)) != NOTOK
#endif
		       && childpid != pid)

			continue;

		(void) signal(SIGPIPE, pstat);

		return;
	}
#endif
}

static struct qbuf *
audio_parse(str)
	char *str;
{
	if (file_attr_length)
		return str2qb(str, file_attr_length, 1);
	else
		return str2qb(str, strlen(str), 1);
}

audio_syntax()
{
	(void) add_attribute_syntax("audio",
				    (IFP) r_octenc, (IFP) r_octsdec,
				    (IFP) audio_parse, audio_print,
				    (IFP) qb_cpy, qb_cmp, qb_free, NULLCP, NULLIFP, TRUE);

}
