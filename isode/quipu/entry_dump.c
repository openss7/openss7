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

/* entry_dump.c - routines to dump the database */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/entry_dump.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/quipu/RCS/entry_dump.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: entry_dump.c,v
 * Revision 9.0  1992/06/16  12:34:01  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include "quipu/util.h"
#include "quipu/entry.h"
#include <errno.h>
#include "tailor.h"

extern LLog *log_dsap;

#ifndef TURBO_DISK

extern RDN parse_rdn;
extern char *new_version();

static
header_print(psa, edb)
	PS psa;
	Entry edb;
{
	switch (edb->e_data) {
	case E_DATA_MASTER:
		ps_print(psa, "MASTER\n");
		break;
	case E_TYPE_SLAVE:
		ps_print(psa, "SLAVE\n");
		break;
	default:
		ps_print(psa, "CACHE\n");
		break;
	}
	if (edb->e_parent != NULLENTRY)
		ps_printf(psa, "%s\n", edb->e_parent->e_edbversion);
	else
		ps_printf(psa, "%s\n", new_version());
}

static
entry_print(psa, entryptr)
	PS psa;
	Entry entryptr;
{
	rdn_print(psa, entryptr->e_name, EDBOUT);
	parse_rdn = entryptr->e_name;
	ps_print(psa, "\n");
	as_print(psa, entryptr->e_attributes, EDBOUT);
	parse_rdn = NULLRDN;
}

static
entry_block_print(psa, block)
	PS psa;
	Entry block;
{
	Entry ptr;

	header_print(psa, block);

	if (block != NULLENTRY) {
		for (ptr = (Entry) avl_getfirst(block->e_parent->e_children); ptr != NULLENTRY;
		     ptr = (Entry) avl_getnext()) {
			if (ptr->e_data != E_TYPE_CONSTRUCTOR) {
				entry_print(psa, ptr);
				ps_print(psa, "\n");
			}
		}
	}
}

write_edb(ptr, filename)
	Entry ptr;
	char *filename;
{
	int um;
	FILE *fptr;
	PS entryps;
	extern char *parse_file;
	extern int errno;

	um = umask(0177);
	if ((fptr = fopen(filename, "w")) == (FILE *) NULL) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("file_open failed: \"%s\" (%d)", filename, errno));
		return NOTOK;
	}
	(void) umask(um);

	if ((entryps = ps_alloc(std_open)) == NULLPS) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("ps_alloc failed"));
		(void) fclose(fptr);
		return NOTOK;
	}
	if (std_setup(entryps, fptr) == NOTOK) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("std_setup failed"));
		(void) fclose(fptr);
		return NOTOK;
	}

	parse_file = filename;

	entry_block_print(entryps, ptr);

	if (entryps->ps_errno != PS_ERR_NONE) {
		LLOG(log_dsap, LLOG_EXCEPTIONS,
		     ("write_edb ps error: %s", ps_error(entryps->ps_errno)));
		(void) fclose(fptr);
		return NOTOK;
	}
	ps_free(entryps);

	if (fflush(fptr) != 0) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("write_edb flush error: %d", errno));
		(void) fclose(fptr);
		return NOTOK;
	}
#if     defined(SYS5) && !defined(SVR4)
	sync();
#else
	if (fsync(fileno(fptr)) != 0) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("write_edb fsync error: %d", errno));
		(void) fclose(fptr);
		return NOTOK;
	}
#endif
	if (fclose(fptr) != 0) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("write_edb EDB close error: %d", errno));
		return NOTOK;
	}

	LLOG(log_dsap, LLOG_TRACE, ("Written %s", filename));

	return (OK);
}

#else

write_edb()
{
	LLOG(log_dsap, LLOG_FATAL, ("write_edb implementation error"));
}

#endif				/* NOT TURBO_DISK */
