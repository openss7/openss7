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
    "Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/bind.c,v 9.0 1992/06/16 12:45:27 isode Rel";
#endif

/* Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/bind.c,v 9.0 1992/06/16 12:45:27 isode Rel */

/*
 * Log: bind.c,v
 * Revision 9.0  1992/06/16  12:45:27  isode
 * Release 8.0
 *
 */

/*****************************************************************************

  bind.c -

*****************************************************************************/

#include <stdio.h>

#include "types.h"
#include "bind.h"
#include "init.h"
#include "util.h"

#include "quipu/dap2.h"

#include "logger.h"

extern char *dsa_address;
extern char *local_dit;

#ifndef NO_STATS
extern LLog *log_stat;
#endif

extern char username[], userpassword[];

/*
 * bind_to_ds - Bind to directory using indicated degree of authentication.
 *
 */
/* ARGSUSED */
QBool
bind_to_ds(argc, argv, auth_type)
	int argc;
	char *argv[];
	auth_bind_type auth_type;
{
	struct ds_bind_arg bind_arg, bind_result;
	struct ds_bind_error bind_error;

	extern char *dsa_address, *myname;

	int secure_ds_bind();

	bind_arg.dba_version = DBA_VERSION_V1988;
	bind_arg.dba_auth_type = DBA_AUTH_SIMPLE;

	if (username != NULLCP || *username != '\0')
		bind_arg.dba_dn = str2dn(username);
	else
		bind_arg.dba_dn = NULLDN;

	if (dsa_address == NULLCP || isnull(*dsa_address)) {
		(void) fprintf(stderr, "BIND ERROR - Cannot find name of DSA to bind to!\n");
		return FALSE;
	}

	switch (auth_type) {
	case EXTERNAL_AUTH:
	case STRONG_AUTH:
	case PROTECTED_AUTH:

		/* Not yet implemented */
		return FALSE;

	case SIMPLE_AUTH:

		(void) strcpy(bind_arg.dba_passwd, userpassword);
		bind_arg.dba_passwd_len = strlen(userpassword);

		bind_arg.dba_auth_type = DBA_AUTH_SIMPLE;
		bind_arg.dba_time1 = NULLCP;
		bind_arg.dba_time2 = NULLCP;

		break;

	default:
		break;
	}

	if (secure_ds_bind(&bind_arg, &bind_error, &bind_result) != DS_OK) {
		if (bind_error.dbe_type == DBE_TYPE_SECURITY)
			(void) fprintf(stderr, "SECURITY ERROR - Cannot bind. Check credentials\n");
		else
			(void) fprintf(stderr, "SERVICE ERROR - Cannot bind.\n");

		return FALSE;
	}

#ifndef NO_STATS
	LLOG(log_stat, LLOG_NOTICE, ("bound (%s to %s)", username, dsa_address));
#endif

	return TRUE;
}				/* bind_to_ds */

/*
 * - get_association_descriptor() -
 * Get association descriptor for a particular request.
 *
 */
/* ARGSUSED */
int
get_association_descriptor(request_id)
	QCardinal request_id;
{
	extern int dsap_ad;

	return dsap_ad;
}				/* get_association_descriptor */
