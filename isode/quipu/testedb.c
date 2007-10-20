#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/testedb.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/RCS/testedb.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: testedb.c,v $
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

LLog * log_dsap;
#ifndef	NO_STATS
LLog * log_stat;
#endif

main (argc, argv)
int argc;
char *argv[];
{
extern IFP unrav_fn;
extern IFP schema_fn;
int real_unravel_attribute ();
int real_check_schema ();
extern PS opt;
extern char dsa_mode;
Entry thedb;
Avlnode	*treetop;


	dsa_mode = TRUE;

	unrav_fn = (IFP) real_unravel_attribute;
	schema_fn = (IFP) real_check_schema;

	quipu_syntaxes();
	if (load_oid_table ("oidtable") == NOTOK)
		fatal (-1, "Can't load oid tables");	
	check_dsa_known_oids ();

	(void) ll_close (log_dsap);
	ll_dbinit (log_dsap, "testedb");
	log_dsap -> ll_events = LLOG_FATAL | LLOG_EXCEPTIONS;

	if ((treetop = getentry_block (NULLENTRY, argc > 1 ? argv [1] : "./EDB"))
	    != NULL) {
		(void) printf ("EDB ok\n");
#ifdef DEBUG
		thedb = get_default_entry(NULLENTRY);
		thedb->e_children = treetop;
		directory_free (thedb);
		free_oid_table();
		tailorfree();
		free_isode_alias();
#endif
		exit (0);
	    }
	
	(void) printf ("EDB not ok\n");

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


