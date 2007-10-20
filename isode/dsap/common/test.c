/* test.c - */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/test.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/dsap/common/RCS/test.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: test.c,v $
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


#include "quipu/util.h"
#include "quipu/entry.h"
#include "quipu/syntaxes.h"
#include "tailor.h"

extern int parse_line;
extern int parse_status;

main (argc, argv)
int argc;
char **argv;
{
	char * buffer;
	char * getline();
	int opt;
	extern int optind;
	extern char *optarg;
	char *myname;
	char *oidtable = "oidtable";

	isodetailor (myname = *argv, 1);

	while((opt = getopt(argc, argv, "o:d")) != EOF)
		switch (opt) {
		    case 'o':
			oidtable = optarg;
			break;
		    case 'd':
			psap_log -> ll_events = LLOG_ALL;
			psap_log -> ll_stat |= LLOGTTY;
			break;
		    default:
			fprintf (stderr, 
				 "Usage: %s [-o oidtable] [-d] [type=value]\n",
				 myname);
			exit (1);
		}
	argc -= optind;
	argv += optind;

	quipu_syntaxes ();
	if (load_oid_table (oidtable) == NOTOK) {
		fprintf (stderr, "%s: can't load oidtable %s\n",
			 myname, oidtable);
		exit (1);
	}

	parse_line = 0;
	parse_error ("Attribute parser testing tool");

	if (argc > 0) {
		while (argc-- > 0)
			do_parse (*argv++);
	}
	else
		for (;;) {
			(void) fprintf (stderr,"-> ");

			if ((buffer = getline(stdin)) == NULLCP)
				break;

			if (*buffer == 'q' && strlen(buffer) == 1)
				break;

			do_parse (buffer);
		}

#ifdef DEBUG
	free_oid_table();
	tailorfree();
	free_isode_alias();
#endif

	exit (0);

}

do_parse (str)
char *str;
{
	Attr_Sequence as;
	Attr_Sequence as2;
	Attr_Sequence as_combine();
	PE pe = NULLPE;
	static PS ps = NULLPS;
	extern PS opt;

	parse_status = 0;

	as2 = as_combine (NULLATTR,TidyString(str),FALSE);

	as = as_cpy (as2);

	if (as == NULLATTR) {
		(void) fprintf (stderr,"NULL value\n");
		return;
	}
	if (parse_status != 0) {
		(void) fprintf (stderr,"parse error - non null as\n");
		return;
	}

	if (encode_IF_Attribute (&pe, 0, 0, NULLCP, as) == NOTOK) {
		fprintf (stderr,"encode problem [%s]\n", PY_pepy);
		return;
	}

	pe2pl (opt,pe);

	as_free (as);	

	if (decode_IF_Attribute (pe, 1, NULLIP, NULLVP, &as) == NOTOK) {
		fprintf (stderr,"decode problem [%s]\n", PY_pepy);
		return;
	}

	ps_print (opt,"READOUT:\n");
	as_print (opt,as,READOUT);

	if (as_cmp (as,as2) != 0)
		fprintf (stderr,"*** Compare/Copy problem ***\n");

	ps_print (opt,"EDBOUT:\n");
	as_print (opt,as2,EDBOUT);

	if (ps == NULL
	    && ((ps = ps_alloc (str_open)) == NULLPS)
	    || str_setup (ps, NULLCP, BUFSIZ, 0) == NOTOK) {
		if (ps)
			ps_free (ps), ps = NULLPS;

		fprintf (stderr,"*** test Internal error ***\n");
		return;
	}

	as_print (ps,as2,EDBOUT);

	as_free (as2);
	pe = NULLPE;

	/* remove \n */
	*--ps -> ps_ptr = NULL, ps -> ps_cnt++;

	parse_status = 0;

	quipu_faststart = TRUE;

	as2 = as_combine (NULLATTR,ps->ps_base,FALSE);

	quipu_faststart = FALSE;

	if (as2 == NULLATTR) {
		(void) fprintf (stderr,"NULL value from 2nd parse\n");
		return;
	}
	if (parse_status != 0) {
		(void) fprintf (stderr,"parse error 2nd time - non null as\n");
		return;
	}

	ps -> ps_base = NULL, ps -> ps_cnt = 0;
	ps -> ps_ptr = NULL, ps -> ps_bufsiz = 0;
}
