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

#include <stdio.h>
#include "generic.h"		/* defines OPERATIONS and ERRORS */
#include <isode/rosap.h>
#include <isode/tsap.h>		/* for listening */
#include <setjmp.h>

static char *myservice = "service";
static char *mycontext = "context";

static jmp_buf toplevel;

int ros_init(), ros_work(), ros_lose();

main(argc, argv, envp)
	int argc;
	char **argv, **envp;
{
	AEI aei;
	struct TSAPdisconnect tds;
	register struct TSAPdisconnect *td = &tds;

	if ((aei = _str2aei(PLocalHostName(), myservice, mycontext, 0, NULLCP, NULLCP)) == NULLAEI)
		fatal("unable to resolve service: %s", PY_pepy);

	if (isodeserver(argc, argv, aei, ros_init, ros_work, ros_lose, td)
	    == NOTOK) {
		if (td->td_cc > 0)
			fatal("isodeserver: [%s] %*.*s", TErrString(td->td_reason),
			      td->td_cc, td->td_cc, td->td_data);
		else
			fatal("isodeserver: [%s]", TErrString(td->td_reason));
	}

	exit(0);
}

...
