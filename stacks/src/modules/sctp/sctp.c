/*****************************************************************************

 @(#) $RCSfile: sctp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:04 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/08/26 23:38:04 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:04 $"

static char const ident[] = "$RCSfile: sctp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:04 $";

#include "compat.h"

#include "sctp.h"
#include "sctp_defs.h"
#include "sctp_hash.h"
#include "sctp_cache.h"

#include "sctp_n.h"
#include "sctp_t.h"
#include "sctp_input.h"

#define SCTP_DESCRIP	"SCTP/IP STREAMS (NPI/TPI) DRIVER." "\n" \
			"Part of the OpenSS7 Stack for LiS STREAMS."
#define SCTP_REVISION	"LfS $RCSfile: sctp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/08/26 23:38:04 $"
#define SCTP_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corp. All Rights Reserved."
#define SCTP_DEVICE	"Supports LiS STREAMS and Linux NET4."
#define SCTP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SCTP_LICENSE	"GPL"
#define SCTP_BANNER	SCTP_DESCRIP	"\n" \
			SCTP_REVISION	"\n" \
			SCTP_COPYRIGHT	"\n" \
			SCTP_DEVICE	"\n" \
			SCTP_CONTACT	"\n"

#ifdef LINUX
MODULE_AUTHOR(SCTP_CONTACT);
MODULE_DESCRIPTION(SCTP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SCTP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SCTP_LICENSE);
#endif
#endif				/* LINUX */

STATIC void
sctp_init(void)
{
	sctp_init_locks();
	sctp_init_caches();
	sctp_init_proto();
}
STATIC void
sctp_term(void)
{
	sctp_term_locks();
	sctp_term_caches();
	sctp_term_proto();
}

/*
 *  =========================================================================
 *
 *  Kernel Module Initialization
 *
 *  =========================================================================
 */
int
init_module(void)
{
	cmn_err(CE_NOTE, SCTP_BANNER);	/* console splash */
	sctp_init();
	sctp_n_init();
	sctp_t_init();
	return (0);
}

void
cleanup_module(void)
{
	sctp_term();
	sctp_n_term();
	sctp_t_term();
	return;
}
