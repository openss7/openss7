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

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/testedb.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/RCS/testedb.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: testedb.c,v
 * Revision 9.0  1992/06/16  12:34:01  isode
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

#include "quipu/util.h"
#include "quipu/entry.h"
#include "psap.h"

LLog *log_dsap;

#ifndef	NO_STATS
LLog *log_stat;
#endif

main(argc, argv)
	int argc;
	char *argv[];
{
	extern IFP unrav_fn;
	extern IFP schema_fn;
	int real_unravel_attribute();
	int real_check_schema();
	extern PS opt;
	extern char dsa_mode;
	Entry thedb;
	Avlnode *treetop;

	dsa_mode = TRUE;

	unrav_fn = (IFP) real_unravel_attribute;
	schema_fn = (IFP) real_check_schema;

	quipu_syntaxes();
	if (load_oid_table("oidtable") == NOTOK)
		fatal(-1, "Can't load oid tables");
	check_dsa_known_oids();

	(void) ll_close(log_dsap);
	ll_dbinit(log_dsap, "testedb");
	log_dsap->ll_events = LLOG_FATAL | LLOG_EXCEPTIONS;

	if ((treetop = getentry_block(NULLENTRY, argc > 1 ? argv[1] : "./EDB"))
	    != NULL) {
		(void) printf("EDB ok\n");
#ifdef DEBUG
		thedb = get_default_entry(NULLENTRY);
		thedb->e_children = treetop;
		directory_free(thedb);
		free_oid_table();
		tailorfree();
		free_isode_alias();
#endif
		exit(0);
	}

	(void) printf("EDB not ok\n");

	return 1;
}

/* stubs for unused external synbols */

int refreshing = FALSE;

/* ARGSUSED */
shadow_entry(eptr)
	Entry eptr;
{
	;
}
