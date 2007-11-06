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

/* turbo_disk.c */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/turbo_disk.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/RCS/turbo_disk.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: turbo_disk.c,v
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

#include <stdio.h>
#include "quipu/config.h"
#include "quipu/entry.h"
#include "psap.h"
#include "quipu/malloc.h"
#include "logger.h"

#ifdef	TURBO_DISK

#include <gdbm.h>
#include "sys.file.h"

extern RDN parse_rdn;
extern LLog *log_dsap;
extern int gdbm_errno;
extern datum turbo_header_key;
extern char *parse_file;

/*
 * turbo_open -- open a dbm edb file for writing.  e is a pointer to the
 * entry to be written.  Parent is used to construct the name of
 * the file to write.  If create is 0 a new database will not be created,
 * otherwise it will and a backup of the old one will be made.
 */

static GDBM_FILE
turbo_open(parent, create, backup)
	Entry parent;
	int create;
	int backup;
{
	GDBM_FILE db;
	DN dn, get_copy_dn();
	char *filename, *dn2edbfile();
	char bakname[1024];
	static char turbo_gfname[1024];

	LLOG(log_dsap, LLOG_TRACE, ("turbo: open"));

	/* get the name of the edb file to write to */
	dn = get_copy_dn(parent);
	if ((filename = dn2edbfile(dn)) == NULLCP) {
		dn_free(dn);
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("turbo: dn2edbfile failed"));
		return (NULL);
	}
	dn_free(dn);
	strcpy(turbo_gfname, filename);
	strcat(turbo_gfname, ".gdbm");
	strcpy(bakname, turbo_gfname);
	strcat(bakname, ".bak");

	/* try to open it */
	if (create) {
		if (backup) {
			/* first make a backup of the old one */
			(void) unlink(bakname);
			if (link(turbo_gfname, bakname) != 0)
				LLOG(log_dsap, LLOG_EXCEPTIONS, ("turbo: could not make backup"));
			(void) unlink(turbo_gfname);
		}

		/* then open the new one */
		db = gdbm_open(turbo_gfname, 0, GDBM_WRCREAT, 0600, 0);
	} else
		db = gdbm_open(turbo_gfname, 0, GDBM_WRITER, 0, 0);

	if (db == NULL) {
		LLOG(log_dsap, LLOG_EXCEPTIONS,
		     ("turbo: gdbm_open failed gdbm_errno %d", gdbm_errno));
		return (NULL);
	}

	parse_file = turbo_gfname;
	return (db);
}

static
turbo_write_entry(e, db)
	Entry e;
	GDBM_FILE db;
{
	static char kbuf[512];
	static char *buf = NULLCP;
	int rc;
	PS ps;
	datum rdn;
	datum ent;

	LLOG(log_dsap, LLOG_TRACE, ("turbo: write_entry"));

	if (buf == NULLCP)
		buf = smalloc(10000);

	/* allocate the string presentation stream to print to */
	if ((ps = ps_alloc(str_open)) == NULLPS) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("turbo: ps_alloc failed"));
		return (NOTOK);
	}
	if (str_setup(ps, buf, 10000, 0) == NOTOK) {
		LLOG(log_dsap, LLOG_EXCEPTIONS, ("turbo: str_setup failed"));
		ps_free(ps);
		return (NOTOK);
	}
	*ps->ps_ptr = 0;

	parse_rdn = e->e_name;
	rdn_print(ps, e->e_name, EDBOUT);
	*ps->ps_ptr = '\0';
	strcpy(kbuf, ps->ps_base);

	rdn.dptr = kbuf;
	rdn.dsize = strlen(rdn.dptr) + 1;

	ps_print(ps, "\n");
	as_print(ps, e->e_attributes, EDBOUT);
	ps_print(ps, "\n");
	*ps->ps_ptr = '\0';
	parse_rdn = NULLRDN;

	ent.dptr = ps->ps_base;
	ent.dsize = strlen(ent.dptr) + 1;

	if ((rc = gdbm_store(db, rdn, ent, GDBM_REPLACE)) != 0) {
		LLOG(log_dsap, LLOG_EXCEPTIONS,
		     ("turbo: gdbm_store %d, gdbm_errno %d", rc, gdbm_errno));
		ps_free(ps);
		return (NOTOK);
	}

	ps_free(ps);
	return (OK);
}

/*
 * turbo_writeall -- write the entries in tree e to the proper edb
 * dbm file. the file is opened by turbo_open, then the tree is traversed,
 * via avl_apply, calling turbo_write_entry to write each entry
 */

turbo_writeall(e)
	Entry e;
{
	GDBM_FILE db, turbo_open();
	int save_heap;
	Entry akid;

	LLOG(log_dsap, LLOG_TRACE, ("turbo: writeall"));

	save_heap = mem_heap;
	GENERAL_HEAP;

	if ((db = turbo_open(e, 1, 1)) == NULL) {
		mem_heap = save_heap;
		return (NOTOK);
	}

	akid = (Entry) avl_getone(e->e_children);
	if (turbo_write_header(db, e, akid ? akid->e_data : e->e_data) != OK) {
		(void) gdbm_close(db);
		mem_heap = save_heap;
		parse_file = NULLCP;
		return (NOTOK);
	}

	if (avl_apply(e->e_children, turbo_write_entry, (caddr_t) db, NOTOK,
		      AVL_PREORDER) != AVL_NOMORE) {
		(void) gdbm_close(db);
		mem_heap = save_heap;
		parse_file = NULLCP;
		return (NOTOK);
	}

	(void) gdbm_close(db);
	mem_heap = save_heap;
	parse_file = NULLCP;
	return (OK);
}

/*
 * turbo_write -- write the single entry pointed to by e to the proper
 * edb dbm file.  The edb dbm file is opened by turbo_open, then a new
 * header is written by turbo_write_header, and finally the entry is 
 * written with a call to turbo_write_entry.
 */

turbo_write(e)
	Entry e;
{
	GDBM_FILE db, turbo_open();
	int save_heap;

	LLOG(log_dsap, LLOG_TRACE, ("turbo: write"));

	save_heap = mem_heap;
	GENERAL_HEAP;

	if ((db = turbo_open(e->e_parent, 1, 0)) == NULL) {
		mem_heap = save_heap;
		return (NOTOK);
	}

	if (turbo_write_header(db, e->e_parent, e->e_data) != OK) {
		(void) gdbm_close(db);
		mem_heap = save_heap;
		parse_file = NULLCP;
		return (NOTOK);
	}

	if (turbo_write_entry(e, db) != OK) {
		(void) gdbm_close(db);
		mem_heap = save_heap;
		parse_file = NULLCP;
		return (NOTOK);
	}

	(void) gdbm_close(db);
	mem_heap = save_heap;
	parse_file = NULLCP;
	return (OK);
}

/*
 * turbo_delete -- delete the entry pointed to by p from the proper
 * edb dbm file.
 */

turbo_delete(e)
	Entry e;
{
	static char deletekey[256];
	int rc;
	GDBM_FILE db, turbo_open();
	PS ps;
	datum key;
	int save_heap;

	LLOG(log_dsap, LLOG_TRACE, ("turbo: delete"));

	save_heap = mem_heap;
	GENERAL_HEAP;

	if ((db = turbo_open(e->e_parent, 0, 0)) == NULL) {
		mem_heap = save_heap;
		return (NOTOK);
	}

	if (turbo_write_header(db, e->e_parent, e->e_data) != OK) {
		(void) gdbm_close(db);
		mem_heap = save_heap;
		return (NOTOK);
	}

	/* allocate the string presentation stream to print the key to */
	if ((ps = ps_alloc(str_open)) == NULLPS) {
		mem_heap = save_heap;
		return (NOTOK);
	}
	if (str_setup(ps, deletekey, sizeof(deletekey), 0) == NOTOK) {
		ps_free(ps);
		mem_heap = save_heap;
		return (NOTOK);
	}
	*ps->ps_ptr = 0;

	rdn_print(ps, e->e_name, EDBOUT);
	*ps->ps_ptr = '\0';
	key.dptr = ps->ps_base;
	key.dsize = strlen(key.dptr) + 1;

	LLOG(log_dsap, LLOG_TRACE, ("turbo: deleting (%s)", key.dptr));
	if ((rc = gdbm_delete(db, key)) != 0) {
		LLOG(log_dsap, LLOG_EXCEPTIONS,
		     ("turbo: gdbm_delete %d gdbm_errno %d", rc, gdbm_errno));
		ps_free(ps);
		(void) gdbm_close(db);
		mem_heap = save_heap;
		return (NOTOK);
	}

	ps_free(ps);
	(void) gdbm_close(db);
	mem_heap = save_heap;
	return (OK);
}

/*
 * turbo_write_header -- write a new header for the edb dbm file db.
 * The version is taken from e's parent version
 */

turbo_write_header(db, parent, datatype)
	GDBM_FILE db;
	Entry parent;
{
	static char hbuf[256];
	int rc;
	char *type;
	char *version, *new_version();
	datum newheader;
	int save_heap;

	LLOG(log_dsap, LLOG_TRACE, ("turbo: write_header"));

	save_heap = mem_heap;
	GENERAL_HEAP;

	switch (datatype) {
	case E_DATA_MASTER:
		type = "MASTER";
		break;
	case E_TYPE_SLAVE:
		type = "SLAVE";
		break;
	default:
		type = "CACHE";
		break;
	}

	if (parent != NULLENTRY)
		version = parent->e_edbversion;
	else
		version = new_version();

	(void) sprintf(hbuf, "%s\n%s\n", type, version);

	newheader.dptr = hbuf;
	newheader.dsize = strlen(newheader.dptr) + 1;

	if ((rc = gdbm_store(db, turbo_header_key, newheader, GDBM_REPLACE))
	    != 0) {
		LLOG(log_dsap, LLOG_EXCEPTIONS,
		     ("turbo: gdbm_store %d gdbm_errno %d", rc, gdbm_errno));
		mem_heap = save_heap;
		return (NOTOK);
	}
	mem_heap = save_heap;
	return (OK);
}

#else

turbo_delete_dummy()
{
}
#endif
