/* dish.c - */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/dish/RCS/dishlib.c,v 9.0 1992/06/16 12:35:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/dish/RCS/dishlib.c,v 9.0 1992/06/16 12:35:39 isode Rel $
 *
 *
 * $Log: dishlib.c,v $
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


#include <stdio.h>
#include <signal.h>
#include "quipu/util.h"
#include "quipu/entry.h"
#include <varargs.h>

#ifdef	SOCKETS
#include "internet.h"
extern int sd_current;
#endif

#ifdef OSISEC
#include "osisec/osisec.h"
extern LLog *log_osisec;
#endif

#define MAXARGS 50

extern LLog    *log_dsap;
DN              dn = NULLDN;	/* This actually stores the current position. */

extern Entry database_root;

#define	OPT	(!frompipe || rps -> ps_byteno == 0 ? opt : rps)
#define	RPS	(!frompipe || opt -> ps_byteno == 0 ? rps : opt)
char	frompipe;
PS	opt, rps;

DN              savename = NULLDN;

SFD             dish_quit ();
SFD             dish_intr ();
char		dad_flag = FALSE;
unsigned	cache_time = 3600;	/* time to keep process alive */
unsigned	connect_time = 120;     /* time to keep connection open */

char            inbuf[LINESIZE];
char		bound = FALSE;
char 		remote_prob;
char 		doneget;
char		*TidyString();
char 		search_result;		/* another horrid global ! */

extern int call_list (), call_compare (), call_search (),
           call_add (), call_delete (), call_showentry (), call_showname (),
           call_showattribute (), call_unbind (), call_help (), call_ds (),
           unknown_cmd (), dsa_control (), call_modify (), call_modifyrdn (),
	   call_quit (), call_bind (), call_moveto (), call_fred ();

static struct {
	char           *command;
	int             (*handler) ();
	int		unique;
	char		defaults [LINESIZE];
} Commands[MAXARGS];
static int num_cmd = 0;

add_dish_command (name,func,len)
char * name;
IFP func;
int len;
{
	Commands[num_cmd].command = name;
	Commands[num_cmd].handler = func;
	Commands[num_cmd].unique  = (len == 0 ? strlen(name) : len);
	bzero (Commands[num_cmd].defaults,LINESIZE);
	num_cmd++;
}

dish_cmd_init ()
{
	add_dish_command ("list", 	call_list,		1);
	add_dish_command ("compare", 	call_compare,		1);
	add_dish_command ("search", 	call_search,		2);
	add_dish_command ("add",	call_add,		1);
	add_dish_command ("delete", 	call_delete,		2);
	add_dish_command ("modify", 	call_modify,		3);
	add_dish_command ("modifyrdn", 	call_modifyrdn,		7);
	add_dish_command ("showentry", 	call_showentry,		2);
	add_dish_command ("showname", 	call_showname,		5);
	add_dish_command ("bind",	call_bind,		1);
	add_dish_command ("unbind", 	call_unbind,		1);
	add_dish_command ("moveto", 	call_moveto,		3);
	add_dish_command ("dsacontrol", dsa_control,		2);
	add_dish_command ("quit", 	call_quit,		1); /* quick way out for interactive program */	
	add_dish_command ("squid",	call_ds,		2);
	add_dish_command ("?", 		call_help,		1);
	add_dish_command ("help", 	call_help,		1);
	add_dish_command ("fred", 	call_fred,		4);
	dish_help_init ();
}

jmp_buf  dish_env;

#ifndef IDLE
#define IDLE 0
#endif
#ifndef BUSY
#define BUSY 1
#endif
static char	dish_state;

#ifndef NO_STATS
extern LLog *log_stat;
#endif
extern LLog *log_dsap;

#ifndef	NO_STATS
static	LLog	_dad_log = {
    "dad.log", NULLCP, NULLCP, LLOG_FATAL | LLOG_EXCEPTIONS | LLOG_NOTICE,
    LLOG_NONE, -1, LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
	LLog   *dad_log = &_dad_log;
#endif

char no_rcfile;

dish_init (argc, argv)
int             argc;
char          **argv;
{
	int             i;
	char           *ttyname (), *getenv();
	char	       *vec [1];
	char	      **vecptr;
	extern char *  tailfile;
#ifdef TURBO_DISK
	extern char	fromfile;
#endif

	dish_cmd_init ();

	(void) signal (SIGHUP,	dish_quit);
	(void) signal (SIGQUIT,	dish_quit);
	(void) signal (SIGILL, 	dish_quit);
	(void) signal (SIGBUS,	dish_quit);
	(void) signal (SIGSEGV,	dish_quit);
	(void) signal (SIGSYS,	dish_quit);
	(void) signal (SIGTERM,	dish_quit);

#ifdef TURBO_DISK
	fromfile = 1;
#endif
	no_rcfile = FALSE;
	for (i=1; i<argc; i++) {
	    if (test_arg (argv[i],"-dad",1)) {
		cache_time = 15 * 60;
		connect_time = 5 * 60;
		dad_flag = TRUE;
#ifndef	NO_STATS
		ll_hdinit (dad_log, argv[0]);
#endif
		shuffle_up (argc--, argv, i);
		continue;
	    }
	    if (test_arg (argv[i],"-fast",1)) {
		no_rcfile = TRUE;
		shuffle_up (argc--, argv, i);
		continue;
	    }
	    if (test_arg (argv[i],"-help",1)) {
		if ((opt = ps_alloc (std_open)) == NULLPS)
			fatal (-62, "ps_alloc failed");
		if (std_setup (opt, stderr) == NOTOK)
			fatal (-63, "std_setup failed");

		if ((rps = ps_alloc (std_open)) == NULLPS)
			fatal (-64, "ps_alloc 2 failed");
		if (std_setup (rps, stdout) == NOTOK)
			fatal (-65, "std_setup 2 failed");

		help_arg ("dish");
		exit (0);
	    }
	    if (test_arg(argv[i],"-tailor",1)) {
		shuffle_up (argc--, argv, i);
		if (i == argc) 
			fatal (-66,"Tailor file name missing");
		tailfile = argv[i];
		shuffle_up (argc--, argv, i);
	    }
	}

	if ( (argc >1) && (test_arg (argv[1], "-pipe",3))) {
		if (init_pipe () != OK)
			exit (-61);
		frompipe = TRUE;
		opt = rps = NULLPS;
	} else {
		frompipe = FALSE;

		if ((opt = ps_alloc (std_open)) == NULLPS)
			fatal (-62, "ps_alloc failed");
		if (std_setup (opt, stderr) == NOTOK)
			fatal (-63, "std_setup failed");

		if ((rps = ps_alloc (std_open)) == NULLPS)
			fatal (-64, "ps_alloc 2 failed");
		if (std_setup (rps, stdout) == NOTOK)
			fatal (-65, "std_setup 2 failed");

		(void) printf ("Welcome to Dish (DIrectory SHell)\n");
		(void) fflush (stdout);
	}

	i = 1;
	vec[0] = argv[0];
	vecptr = vec;

	want_oc_hierarchy ();	/* for add/modify ! */

#ifndef NO_STATS
	log_stat -> ll_file = "dish.log";
	log_stat -> ll_stat &= ~LLOGCRT;
#endif
	log_dsap -> ll_stat &= ~LLOGCRT;
	dsap_init (&i, &vecptr);

#ifdef OSISEC
	osisecinit((int*)0, (char***)0, 1);
#endif

#ifndef NO_STATS
	ll_hdinit (log_stat,vec[0]);
#endif
	
	check_known_oids();

	if (user_tailor () != OK) {
		
		(void) fprintf (stderr, "Tailoring failed\n");
		
		if (frompipe)
			exit_pipe ();

		exit (-66);
	}

#ifdef OSISEC
	osisec_tailor(".authrc", log_osisec);
#endif

	if (! frompipe) {
	
#ifndef NO_STATS
		char buf [LINESIZE];
		*buf = 0;

		for (i=0; i<argc; i++) {
			(void) strcat (buf,argv[i]);
			(void) strcat (buf," ");
			if (test_arg (argv[i], "-password",2) && ++i < argc)
			    (void) strcat (buf, "????");
		}
		LLOG (log_stat,LLOG_NOTICE,("%s",buf)); 
#endif
		if (setjmp (dish_env)) 
			exit (-66);

		if (call_bind (argc,argv) != OK)
			exit (-67);
	}
}

/* ARGSUSED */
unknown_cmd (argc, argv)
int             argc;
char          **argv;
{
	if (frompipe)
		ps_print (opt,"Serious dish error\n");
	else {
		if (print_arg_error (opt) != OK)
			ps_print (opt,"Unknown command --- type '?' for help\n");
	}
}

#ifdef GNUREADLINE

void gnu_gets_setup ()
{
   extern int           rl_bind_key ();
   extern int               *rl_insert ();
   rl_bind_key ( '\t', rl_insert );
}

static char *gnu_gets ( buf, len )
      char                   *buf;
      int                    len;
{
   extern char          *readline ();
   static char       *gets_line;
   gets_line = readline ( "Dish -> " );
   if ( gets_line && *gets_line )
      add_history ( gets_line );
   if ( gets_line )
   {
      strncpy ( buf, gets_line, strlen(gets_line) + 1 );
      free ( gets_line );
      return buf;
   }
   else
      return (char *) NULL;
}
#endif


do_dish ()
{
	char	       *brkset;
	char           *command;
	char		cmd_buf [LINESIZE];
	char 	       *ptr;
	char           *vector[MAXARGS];
	int             no_of_args;
	int             x;
	char		noarg;
	extern int 	parse_line;
	extern int	dsa_dead;
	extern int	errno;

	Commands[num_cmd].command = NULLCP;
	Commands[num_cmd].handler = unknown_cmd;
	Commands[num_cmd].unique  = 0;

#ifdef GNUREADLINE
	gnu_gets_setup ();
#endif

	(void) signal (SIGINT, dish_intr);
	if (setjmp (dish_env) == 1)
		goto tidy_up;

	while (1) {
		dish_state = IDLE;
		if (dsa_dead) {
			(void) ds_unbind ();
			bound = FALSE;
			dsa_dead = FALSE;
		}

		parse_line = 0;
		reset_arg ();
		set_current_pos();
		remote_prob = FALSE;
		doneget = FALSE;

		if (frompipe) {
			set_alarm ();

			if (read_pipe (inbuf,sizeof inbuf) == -1)
				continue;


			(void) signal (SIGALRM, SIG_IGN);	
				/* unset alarm */
		        if (dad_flag)
			    cache_time = 15 * 60;
#ifdef SOCKETS
			command = inbuf;
#else
			command = index (inbuf, ':');
			*command++ = 0;
#endif

#ifdef	SOCKETS
			if ((opt = ps_alloc (fdx_open)) == NULLPS) {
				exit_pipe ();
				fatal (-68, "ps_alloc failed");
			}
			if (fdx_setup (opt, sd_current) == NOTOK) {
				exit_pipe ();
				fatal (-69, "fdx_setup failed");
			}
			(void) (*opt -> ps_writeP) (opt, "2", 1, 0);

			if ((rps = ps_alloc (fdx_open)) == NULLPS) {
				exit_pipe ();
				fatal (-70, "ps_alloc 2 failed");
			}
			if (fdx_setup (rps, sd_current) == NOTOK) {
				exit_pipe ();
				fatal (-71, "fdx_setup 2 failed");
			}
			(void) (*rps -> ps_writeP) (rps, "1", 1, 0);
#else
			if ((opt = ps_alloc (str_open)) == NULLPS) {
				exit_pipe ();
				fatal (-68, "ps_alloc failed");
			}
			if (str_setup (opt, NULLCP, BUFSIZ, 0) == NOTOK) {
				exit_pipe ();
				fatal (-69, "str_setup failed");
			}
			opt->ps_ptr++, opt->ps_cnt--;

			if ((rps = ps_alloc (str_open)) == NULLPS) {
				exit_pipe ();
				fatal (-70, "ps_alloc 2 failed");
			}
			if (str_setup (rps, NULLCP , BUFSIZ, 0) == NOTOK) {
				exit_pipe ();
				fatal (-71, "str_setup 2 failed");
			}
			rps->ps_ptr++, rps->ps_cnt--;
#endif
			if (!no_rcfile)
				test_rc_file (opt);
		} else {
			do {
			    set_alarm ();
#ifdef GNUREADLINE
			    if (gnu_gets ( inbuf, sizeof inbuf ) == 0)
				call_quit(0, NULLVP);
#else
			    (void) printf ("Dish -> ");
			    (void) fflush (stdout);
			    while (fgets (inbuf, sizeof inbuf, stdin) == 0)
				    if (errno != EINTR)
					call_quit(0, NULLVP);
#endif
			    for (ptr = inbuf; isspace (*ptr); ptr++)
				continue;
			} while (*ptr == 0);
			(void) signal (SIGALRM, SIG_IGN); /* unset alarm */
			command = TidyString(inbuf);
		}
		if (savename)
			dn_free (savename);
		savename = dn_cpy (dn);
		hide_picture();
		dish_state = BUSY;
		
		ptr = command;
		while (*ptr)
			if (isspace (*ptr))
 				break;
			else
				ptr++;

		if (*ptr == 0) {
			noarg = TRUE;
		} else {
			*ptr = 0;	
			noarg = FALSE;
		}
 	        for (x = 0; Commands[x].command != 0; x++)
			if (test_arg (command, Commands[x].command, Commands[x].unique))
				break;

		if (! noarg)	
			*ptr++ = ' ';

		if (* Commands[x].defaults != 0) {
			if (noarg) {
				(void) sprintf (cmd_buf,"%s %s",Commands[x].command,Commands[x].defaults);
			} else {
				(void) sprintf (cmd_buf,"%s %s %s",Commands[x].command,Commands[x].defaults,ptr);
			}
			command = cmd_buf;
		}

		if (strncmp (command, "fred -ufn ",
			     sizeof "fred -ufn " - 1) == 0) {
		    command[4] = command[9] = ',';
		    brkset = ",";
		}
		else
		    brkset = " \t";
		if ((no_of_args = sstr2arg (command, MAXARGS, vector, brkset)) == NOTOK)
			ps_printf (OPT, "Too many arguments... Can't cope.\n");
		else {
			char help_flag = FALSE;
			int y;
#ifndef NO_STATS
			char buf [LINESIZE];

			buf[0] = NULL;
#endif

			vector[0] = Commands[x].command;

#ifndef NO_STATS
			if (vector[0] != NULLCP) {
				(void) strcpy (buf,vector[0]);
				(void) strcat (buf," ");
			}
#endif
			for (y=1; y<no_of_args; y++) {

#ifndef NO_STATS
				(void) strcat (buf,vector[y]);
				(void) strcat (buf," ");

				if (test_arg (vector[y], "-password",2) && y+1 < no_of_args) {
				    (void) strcat (buf, "????");
				    y++;
				    continue;
				}
#endif
				if (test_arg (vector[y],"-help",1)) {
					if (vector[0] != NULLCP)
					   	help_arg (vector[0]);
					else
						unknown_cmd(0,NULLVP);
					help_flag = TRUE;
				}
			}
#ifndef NO_STATS
			LLOG (log_stat,LLOG_NOTICE,("%s",buf)); 
			if (dad_flag
			        && buf[0]
			        && strncmp (buf, "moveto ",
					    sizeof "moveto " -1))
			    (void) ll_log (dad_log, LLOG_NOTICE, NULLCP,
					   "%s", buf);
#endif
			if ( ! help_flag)
			       (*Commands[x].handler) (no_of_args, vector);
		}

		/* if from pipe, return results */
tidy_up:;		

		if (frompipe && !remote_prob) {
#ifdef	SOCKETS
		    if (rps -> ps_byteno > 0) {
			(void) ps_flush (rps);
		    } else
			if (opt -> ps_byteno > 0)
			    (void) ps_flush (opt);
#else
			if (opt->ps_byteno == 0) {
				*rps->ps_ptr = 0;
				send_pipe (rps->ps_base);
			} else {
				*opt->ps_ptr = 0;
				if (search_result == OK)
					*opt->ps_base = '2';
				else
					*opt->ps_base = '3';	/* Signal search ok but >1 hit, with -hitone option */
				send_pipe (opt->ps_base);
			}
#endif
			ps_free (opt);
			ps_free (rps);
#ifdef	SOCKETS
			(void) close_tcp_socket (sd_current);
			sd_current = NOTOK;
#endif
		} else {
			(void) fflush (stdout);
			(void) ps_flush (opt);
			(void) ps_flush (rps);
		}
	}
}

/* ARGSUSED */
call_quit (argc, argv)
int argc;
char **argv;
{
	/* can only get called if run interactively - dont worry about pipe */
	(void) signal (SIGINT, SIG_DFL);
	
	DLOG (log_dsap, LLOG_DEBUG, ("Dish:- Exiting Dish successfully..."));
	if (bound)
		(void) ds_unbind ();
	bound = FALSE;
	ps_free (opt);
	ps_free (rps);
	hide_picture();
	directory_free (database_root);	   /* clean /tmp files */

	exit (0);
}

set_cmd_default (cmd, dflt)
char * cmd;
char * dflt;
{
int x;

	for (x = 0; Commands[x].command != 0; x++)
		if (strcmp (cmd, Commands[x].command) == 0) {
			if (* Commands[x].defaults != 0)
				(void) strcat (Commands[x].defaults, " ");
			(void) strcat (Commands[x].defaults, dflt);
			return (OK);
		}

	return (NOTOK);

}

/* ARGSUSED */
SFD dish_intr (sd)
int sd;
{
#ifndef BSDSIGS
	(void) signal (SIGINT, dish_intr);
#endif

	if (dish_state == IDLE)
		ps_printf (OPT, "\n");
	else {
	    	ps_printf (OPT, "(Interrupted)\n");
		dish_state = IDLE;
	}

	longjmp (dish_env,2);
}

void    advise (va_alist)
va_dcl
{
    int     code;
    va_list ap;
    extern LLog    *log_dsap;

    va_start (ap);

    code = va_arg (ap, int);

    (void) _ll_log (log_dsap, code, ap);

    va_end (ap);
}
