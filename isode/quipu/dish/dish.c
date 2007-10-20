/* dish.c - dish main routine */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/dish/RCS/dish.c,v 9.0 1992/06/16 12:35:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/dish/RCS/dish.c,v 9.0 1992/06/16 12:35:39 isode Rel $
 *
 *
 * $Log: dish.c,v $
 * Revision 9.0  1992/06/16  12:35:39  isode
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


#include "manifest.h"
#include "quipu/util.h"

#ifdef OSISEC
#include "psap.h"	/* Damn c compiler - its not unreasonable at all !?! */
#endif

#include "osisec-stub.h"

/*
	OPTIONS...

	USE_PP:	Build a version of dish with the PP enhancements.
		Needs libpp.a and libdl.a from PP (PP 4.2 and above).
		Made automatically in PP/Tools/dlist.

	MANAGE: Build a management version of Dish.
		Needs libmanage.a from ISODE/others/quipu/uips/manage
		Made automatically in ISODE/others/quipu/uips/manage.
			
	OSISEC:	Build Secure DUA/DSA
 */

#ifdef MANAGE
extern int call_add_alias ();
extern int call_del_alias ();
extern int call_alias_chk ();
#endif

#ifdef DONT_USE_PP_ANYMORE
extern int call_dlist ();
#endif

main (argc, argv)
int             argc;
char          **argv;
{
	extern struct SecurityServices *dsap_security;
	
	quipu_syntaxes ();

#ifdef OSISEC
#ifdef REVOKE_SYNTAX
	revoke_syntax();	
#endif
#endif

#ifdef USE_PP
	pp_quipu_init (argv[0]);
#endif


#ifdef OSISEC

	dsap_security = use_serv_X509();
	use_sig_md2withrsa();

#else  /* OSISEC */

	/* Needed for bootstrapping */
	dsap_security = use_serv_null();

#endif /* OSISEC */

	osisecinit(&argc,&argv, 0);

	dish_init (argc,argv);

#ifdef USE_PP
	pp_quipu_run ();
#endif

#ifdef DONT_USE_PP_ANYMORE
        add_dish_command ("lmnpq", call_dlist, 2);
        add_dish_help    ("lmnpq", 
		 "[-dncheck] [-orcheck] [-orupdate] [-check] [-update]", 
			   FALSE, FALSE,
                          "List Manager Now using PP and Quipu,");
#endif

#ifdef MANAGE
	add_dish_command ("add_alias", call_add_alias, 5);
	add_dish_help	 ("add_alias", "<alias_name> <object>", FALSE, FALSE,
			  "add an alias entry,");
	add_dish_command ("del_alias", call_del_alias, 5);
	add_dish_help	 ("del_alias", "<object>", FALSE, FALSE, 
			  "delete an alias entry,");
	add_dish_command ("alias_chk", call_alias_chk, 5);
	add_dish_help	 ("alias_chk", "<object>", FALSE, FALSE, 
			  "Check alias or all aliases below <object>,");
#endif

	add_dish_help ("quit","", FALSE, FALSE, "Quit the program.");
	add_dish_help (NULLCP,NULLCP,0,0,NULLCP);

	do_dish ();

	return 0;
}

osisecinit(argc, argv, fn)
int             *argc;
char          ***argv;
int	fn;
{
    register char   *ap;
    char  **argptr;
static char   *args[10];
static int     argp;
    int	       narg;

    if(fn == 0) {
	argp = 0;
	narg = 1;
	args[argp++] = (*argv)[0];
	for (argptr = *argv, argptr++; ap = *argptr ; argptr++, narg++) {
	    if (*ap == '-')
		switch (*++ap) {
		    case 'A':
			if ((ap = *++argptr) == NULL || *ap == '-')
			    break;
			shuffle_up ((*argc)--, *argv, narg);
			shuffle_up ((*argc)--, *argv, narg);
			args[argp++] = "-A";
			args[argp++] = ap;
			break;
    
		     default:
			continue;
		 }
    
	    break;
	}
	args[argp] = NULLCP;
    }

#ifdef OSISEC

    else
        osisec_init (&argp, (argptr = args, &argptr));

#endif

}
