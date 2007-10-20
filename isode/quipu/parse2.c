/* parse2.c - */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/parse2.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/RCS/parse2.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: parse2.c,v $
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
#include "quipu/config.h"
#include "cmd_srch.h"
#include "quipu/malloc.h"
#ifdef TURBO_DISK
#include <gdbm.h>
#endif

Avlnode *getentry_block();
Entry get_entry_aux();
extern LLog * log_dsap;
char * getline ();
static test_duplicate ();
int rdn_print ();
int master_edbs = 0;
int slave_edbs = 0;

#ifdef TURBO_DISK
extern datum	turbo_header_key;
#endif

Avlnode *getentry_block (p_parent,fname)
Entry p_parent;
char * fname;
{
extern char * parse_file;
extern int parse_status;
extern int parse_line;
int   dtype;
char *version;
Avlnode	*tree;
Avlnode	*get_entries_aux();
extern int errno;
time_t cache_age;

#ifdef TURBO_DISK
GDBM_FILE	file;
char		gfname[1024];
int		save_heap;
#else
FILE 		*file;
#endif

#ifdef TURBO_DISK
	strcpy(gfname, fname);
	strcat(gfname, ".gdbm");
	save_heap = mem_heap;
	GENERAL_HEAP;
	file = gdbm_open(gfname, 0, GDBM_READER, 0, 0);
	mem_heap = save_heap;
	fname = gfname;
#else
	file = fopen (fname, "r");
#endif

	if (file == NULL) {
		extern int	refreshing;

		LLOG (log_dsap,LLOG_NOTICE,("WARNING - Can't open \"%s\" (%d)- should I be able to ?",fname,errno));
		if (refreshing)
			parse_error("Can't open \"%s\"", fname);
		return(NULLAVL);
	}

	LLOG (log_dsap,LLOG_NOTICE,("Loading \"%s\"",fname));

	parse_status = 0;
	parse_line   = 0;
	parse_file = fname;

	if (get_header (file,&dtype,&version) != OK) {
		parse_line = 0;
		parse_error ("Bad HEADER - File %s not loaded",fname);
		parse_file = NULLCP;
#ifdef TURBO_DISK
		save_heap = mem_heap;
		GENERAL_HEAP;
		(void) gdbm_close (file);
		mem_heap = save_heap;
		return(NULLAVL);
#else
		(void) fclose (file);
		return (NULLAVL);
#endif
	}

	if (dtype == E_TYPE_CACHE_FROM_MASTER) {
		UTC sutc;
		struct tm *tm;
		if (sutc = str2utct (version,strlen(version)))
			if (tm = ut2tm (sutc))
				cache_age = gtime (tm);
		p_parent->e_allchildrenpresent = FALSE;
	}

	tree = get_entries_aux (file,p_parent,version,dtype,cache_age);

	if ((parse_status == 0) && (tree == NULLAVL)) {
		LLOG(log_dsap, LLOG_NOTICE,("Header OK, but null EDB File %s.",fname));
		p_parent->e_leaf = FALSE ;
		p_parent->e_allchildrenpresent = 2 ;
                master_edbs++;
#ifdef TURBO_DISK
		p_parent->e_children = NULLAVL ;
		save_heap = mem_heap;
		GENERAL_HEAP;
		(void) gdbm_close (file);
		mem_heap = save_heap;
		return(NULLAVL);
#else
		p_parent->e_children = NULLAVL;
		(void) fclose (file);
		return(NULLAVL) ;
#endif
	}

	if ((parse_status != 0) || (tree == NULLAVL)) {
		parse_line = 0;
		parse_error ("File %s not loaded",fname);
		parse_file = NULLCP;
		p_parent->e_allchildrenpresent = FALSE;
#ifdef TURBO_DISK
		save_heap = mem_heap;
		GENERAL_HEAP;
		(void) gdbm_close (file);
		mem_heap = save_heap;
		return(NULLAVL);
#else
		(void) fclose (file);
		return(NULLAVL);
#endif
	}

        if ( p_parent != NULLENTRY ) {
       	        p_parent->e_edbversion = version;
               	if ((dtype == E_DATA_MASTER) || (dtype == E_TYPE_SLAVE)) 
                       	p_parent->e_allchildrenpresent = 1; 	/* at least */
       	}

	parse_file = NULLCP;

	if (dtype == E_DATA_MASTER)
		master_edbs++;
	if (dtype == E_TYPE_SLAVE)
		slave_edbs++;

#ifdef TURBO_DISK
	save_heap = mem_heap;
	GENERAL_HEAP;
	(void) gdbm_close (file);
	mem_heap = save_heap;
	return(tree);
#else
	(void) fclose (file);
	return (tree);
#endif
}

#ifdef TURBO_DISK

get_header (db, typeptr, versionptr)
GDBM_FILE	db;
int		*typeptr;
char		**versionptr;
{
	char		*v, *p;
	datum		h;
	int		save_heap;
	static CMD_TABLE cmd_header[] =  {
		"MASTER",	E_DATA_MASTER,
		"SLAVE",	E_TYPE_SLAVE,
		"CACHE",	E_TYPE_CACHE_FROM_MASTER,
		0,		-1,
	};
	extern char	*parse_entry;

	save_heap = mem_heap;
	GENERAL_HEAP;

	parse_entry = turbo_header_key.dptr;
	if (db == NULL) {
		parse_error("NULL dbm file!!!", NULLCP);
		mem_heap = save_heap;
		return(NOTOK);
	}

	h = gdbm_fetch(db, turbo_header_key);
	if (h.dptr == NULL) {
		parse_error("File has no header!!!", NULLCP);
		mem_heap = save_heap;
		return(NOTOK);
	}

	v = index(h.dptr, '\n');
	if (v == NULLCP) {
		parse_error("Bad file header", NULLCP);
		mem_heap = save_heap;
		return(NOTOK);
	}
	*v++ = '\0';

	if ((*typeptr = cmd_srch(h.dptr, cmd_header)) == -1) {
		parse_error("File type %s not recognised", h.dptr);
		mem_heap = save_heap;
		return(NOTOK);
	}

	if (*v == '\0') {
		parse_error("No version specified", NULLCP);
		mem_heap = save_heap;
		return(NOTOK);
	}
	if ((p = index(v, '\n')) != NULLCP)
		*p = '\0';
	*versionptr = strdup(v);
	free(h.dptr);

	mem_heap = save_heap;
	return(OK);
}

#else

get_header (file,typeptr,versionptr)
FILE * file;
int * typeptr;
char ** versionptr;
{
char * ptr;
static CMD_TABLE cmd_header [] = {
	"MASTER",	E_DATA_MASTER,
	"SLAVE",	E_TYPE_SLAVE,
	"CACHE",	E_TYPE_CACHE_FROM_MASTER,
	0,		-1,
	};

	if ((ptr = getline (file)) == NULLCP) {
		parse_error ("NULL file !!!",NULLCP);
		return (NOTOK);
	}

	if ((*typeptr = cmd_srch (ptr,cmd_header)) == -1) {
		parse_error ("File type %s not recognised",ptr);
		return (NOTOK);
	}

	if ((ptr = getline (file)) == NULLCP) {
		parse_error ("No version specified",NULLCP);
		return (NOTOK);
	}
	*versionptr = strdup (ptr);
	
	return (OK);
}

#endif

/* ARGSUSED */
Avlnode *get_entries_aux (file,parent,version,dtype,cache_age)
#ifdef TURBO_DISK
GDBM_FILE	file;
#else
FILE * file;
#endif
Entry parent;
char * version;
int dtype;
time_t cache_age;
{
register Entry eptr = NULLENTRY;
Avlnode	*tree = NULLAVL;
int	entry_cmp();
#ifndef TURBO_INDEX
register Entry trail;
#endif
Entry find_sibling();
#ifdef TURBO_DISK
extern int dbmeof;
#endif

#ifdef TURBO_DISK
	dbmeof = 0;
	while (dbmeof == 0) {
#else
	while (feof(file) == 0) {
#endif
		if ((eptr = get_entry_aux (file,parent,dtype)) == NULLENTRY)
			continue;
		
		if (dtype == E_TYPE_CACHE_FROM_MASTER)
			eptr->e_age = cache_age;
			
		DATABASE_HEAP;

		if (avl_insert(&tree, (caddr_t) eptr, entry_cmp, avl_dup_error)
		    == NOTOK) {
                        pslog (log_dsap,LLOG_EXCEPTIONS,"Duplicate entry for",
                            rdn_print,(caddr_t)eptr->e_name);
                        parse_error ("Non Unique RDN",NULLCP);
		}

#ifdef TURBO_INDEX
		turbo_add2index(eptr);
#endif
	}

	return(tree);
}


Avlnode *get_entries (file,parent,version,dtype)
#ifdef TURBO_DISK
GDBM_FILE	file;
#else
FILE * file;
#endif
Entry parent;
char * version;
int dtype;
{
extern int parse_status;
extern int parse_line;

	parse_status = 0;
	parse_line   = 0;

	return (get_entries_aux (file,parent,version,dtype,(time_t)NULL));
}

