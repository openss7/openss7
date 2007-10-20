/* dsa.c - Main routine for QUIPU DSA process */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/dsa.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/dsa.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: dsa.c,v $
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

#include <signal.h>
#include <stdio.h>
#include <varargs.h>
#include "quipu/util.h"
#include <sys/stat.h>
#include "sys.file.h"

#include "dgram.h"
#ifdef	TCP
#include "internet.h"
#endif

#include "rosap.h"
#include "tsap.h"
#include "tailor.h"
#include "quipu/connection.h"

#ifdef  USE_PP
#ifndef USE_PP_IN_DSA
#undef  USE_PP		/* Not sure it is a good idea for a DSA yet... */
#endif
#endif	/* USE_PP */

#ifdef DEBUG
#if (!defined(mips) && !defined(ultrix))
	/* Use sbrk() to trace memory growth - does not work on Ultrix! */
#define SBRK_DEBUG
#endif
#endif

#include "osisec-stub.h"

#ifdef SUN_X25
#include "x25.h"
#endif

PS      opt;
static  int   debug = 1;
static  int   nbits = FD_SETSIZE;

extern LLog * log_dsap;

#ifdef QUIPU_CONSOLE
#include "quipu/IF-types.h"   
extern AV_Sequence open_call_avs ;    /* SPT: This holds a sequence of 
					 all the open call values. */
void   open_call_avs_clearup() ;
void   Remove_openCall_attribute() ;
#endif /* QUIPU_CONSOLE */

static  char *myname;

void    adios (), advise ();
void    mk_dsa_tmp_dir();
static  envinit (), setdsauid();
SFD attempt_restart();
extern int print_parse_errors;
extern int parse_line;
struct task_act	* task_select();

char quipu_shutdown = FALSE;

extern SFP abort_vector;

#ifndef NO_STATS
extern LLog    *log_stat;
#endif

#ifdef DEBUG
extern Entry database_root;
extern Attr_Sequence dsa_real_attr;
extern Attr_Sequence dsa_pseudo_attr;
extern DN mydsadn;
extern AV_Sequence super_user;
#endif

/*
* Basic data structure of the DSA server.
*/
char			* mydsaname = "undefined";
struct PSAPaddr		* mydsaaddr = NULLPA;
struct PSAPaddr		* dsaladdr = NULLPA;
struct connection	* connlist;
int			  conns_used;
struct connection	* connwaitlist;
struct di_block		* deferred_dis = NULL_DI_BLOCK;
struct oper_act		* get_edb_ops;
extern struct SecurityServices *dsap_security;

char ** sargv;

main(argc, argv)
int    argc;
char    **argv;
{
#ifdef SBRK_DEBUG
    unsigned proc_size = 0;
    unsigned new_size;
    extern caddr_t sbrk();
#endif
    extern char *treedir;
    extern char	* mydsaname;
    extern char	  startup_update;
    extern time_t	timenow;
    struct task_act	* task;

#ifdef QUIPU_CONSOLE
    extern Attr_Sequence dsa_pseudo_attr ;
    extern char * dsa_address ; /* Added by SPT */
    struct quipu_call * open_call ; /* This bit added for SPT for a 
				       spot of debugging. */
    struct quipu_call * oc_tmp ; /* This bit added for SPT for a spot 
				    of debugging. */
    struct UTCtime ut ;
#endif /* QUIPU_CONSOLE */

    int		  secs;
    char start_buf [LINESIZE];
    /*
    * Function to stop DSA server.
    */
    SFD          stop_dsa();
#ifdef	SIGUSR1
    SFD		 list_status ();
#endif
#ifdef	SIGUSR2
    SFD		 list_status2 ();
#endif

    {
	register int	i;
	register char  *cp,
		      **ap,
		      **sp;

	i = 0;
	for (ap = argv; cp = *ap; ap++)
	    if (*cp == '-' && *++cp == 'r') {
		sargv = argv;
		goto no_copy;
	    }
	    else
		i++;
	if ((sargv = (char **) calloc ((unsigned) (i + 2), sizeof *sargv))
	        == NULL)
	    fatal (-11, "out of memory on argv copy");
	for (ap = argv, sp = sargv; *sp = *ap++; sp++)
	    continue;
	*sp++ = "-r";
	*sp = NULL;
    }
no_copy: ;

    if (myname = rindex (argv[0], '/'))
	myname++;
    if (myname == NULL || *myname == NULL)
	myname = argv[0];

    isodetailor (myname,0);

    envinit();  /* detach */

    quipu_syntaxes ();

#ifdef USE_PP
    pp_quipu_init (argv[0]);
#endif

#ifdef OSISEC
    dsap_security = use_serv_X509();
    use_sig_md2withrsa();
#else
    dsap_security = use_serv_null();
#endif /* OSISEC */

    osisecinit(&argc,&argv, 0);

    dsa_sys_init(&argc, &argv);

    osisecinit((int*)0, (char***)0, 1);

#ifdef USE_PP
    pp_quipu_run ();
#endif

    setdsauid();

    mk_dsa_tmp_dir();

    print_parse_errors = FALSE;

#ifndef NO_STATS
    ll_hdinit (log_stat,myname);
#endif

    if ((opt = ps_alloc (std_open)) == NULLPS)
	     fatal (-12,"ps_alloc failed");
    if (std_setup (opt,stdout) == NOTOK)
	     fatal (-13,"std_setup failed");

    DLOG (log_dsap,LLOG_DEBUG,( "About to dsa_init()"));

    (void) chdir(treedir);

#ifdef SBRK_DEBUG
    proc_size = (unsigned) sbrk(0);
    LLOG (log_dsap, LLOG_NOTICE, ("Process size = %d bytes", proc_size));
#endif

    if(dsa_init() == NOTOK)
    {
	fatal(-14,"Couldn't initialise the DSA!!");
    }

    if(net_init() == NOTOK)
    {
	fatal(-15,"Couldn't start the DSA!!");
    }


#ifdef QUIPU_MALLOC
#ifdef DEBUG
       start_malloc_trace (NULLCP);
#endif
#endif

    if (startup_update) 
    {
	/* Will generate a list of EDB operations! */
	(void) time (&timenow);
  	slave_update();
    }

    {
	char    filebuf[BUFSIZ];
	FILE   *fp;

	(void) sprintf (filebuf, "%sPID", treedir);
	if (fp = fopen (filebuf, "w")) {
	    (void) fprintf (fp, "%d\n", getpid ());
	    (void) fclose (fp);
	}
	else
	    LLOG (log_dsap,LLOG_EXCEPTIONS,("Can't open PID file %s",filebuf));
    }

    /*
    * Do stop_dsa() on receiving a Ctrl-C
    */

    (void) signal (SIGINT, stop_dsa);
    (void) signal (SIGTERM,stop_dsa);
    (void) signal (SIGHUP, stop_dsa);

    /* now started don't stop on core dumps !!! */
    (void) signal (SIGQUIT, attempt_restart);
    (void) signal (SIGILL,  attempt_restart);
    (void) signal (SIGBUS,  attempt_restart);
    (void) signal (SIGSEGV, attempt_restart);
    (void) signal (SIGSYS,  attempt_restart);
    (void) signal (SIGPIPE,  attempt_restart);
#ifdef	SIGUSR1
    (void) signal (SIGUSR1, list_status);
#ifdef	SIGUSR2
    (void) signal (SIGUSR2, list_status2);
#endif
#endif

    abort_vector = attempt_restart;
    parse_line = 0;

    (void) sprintf (start_buf,"DSA %s has started on %s",mydsaname,
			    paddr2str(dsaladdr,NULLNA));

    LLOG (log_dsap,LLOG_NOTICE,(start_buf));
#ifndef NO_STATS
    LLOG (log_stat,LLOG_NOTICE,(start_buf));
#endif

     if (debug)
	(void) fprintf (stderr,"%s\n",start_buf);

#ifdef SBRK_DEBUG
    proc_size = (unsigned) sbrk(0);
    LLOG (log_dsap, LLOG_NOTICE, ("Start size = %d bytes", proc_size));
#endif

    for(;;)
    {
	if((task = task_select(&secs)) == NULLTASK)
	{
#ifdef SBRK_DEBUG
	   if ( secs != 0 && (log_dsap -> ll_events & LLOG_NOTICE)) {
		/* Only if we are idle ! */
		new_size = (unsigned) sbrk(0);
		if ( new_size > proc_size) {
			LLOG (log_dsap, LLOG_NOTICE, ("Process grown by %d bytes", new_size - proc_size));
			proc_size = new_size;
		}
	    }
#endif

#ifdef QUIPU_CONSOLE
	    open_call_avs_clearup() ;
#endif /* QUIPU_CONSOLE */

	    (void) dsa_wait(secs);	/* Check network with timeout of secs */
	}
	else
	{
	    dsa_work(task);	/* Process the DSA task selected */
	    (void) dsa_wait(0);
	}
    } /* forever */
} /* main */


static int restart = 0;

/* ARGSUSED */
SFD clean_exit(x)
int x;
{
    if (restart)
	do_restart(x);
    else
	exit (0);
}

#ifdef NOT_ANY_MORE

static int check_conns (secs)
int secs;
{
struct connection		* cn;
int result = TRUE;
struct PSAPaddr * pa;
char * protocol;

	/* Looks like connections are all inactive now.
	   Wait 'till all DSP connections closed.
	*/

	for(cn=connlist; cn!=NULLCONN; cn=cn->cn_next)
	   switch (cn -> cn_state) {
	   if (cn -> cn_ad != NOTOK) {
		 pa = &cn->cn_start.cs_ds.ds_start.acs_start.ps_calling;
		 if (pa->pa_addr.sa_addr.ta_addrs[0].na_stack == NA_TCP) {
		     if (cn->cn_ctx != DS_CTX_X500_DAP)
			 protocol = "DAP";
		     else
			 protocol = "DSP";

		     LLOG (log_dsap, LLOG_EXCEPTIONS, 
		            ("Waiting for TCP %s connection (%d) from %s to exit",
			    protocol, cn->cn_ad, paddr2str(pa,NULLNA)));
		     result =  FALSE;
		 }
	   }

	return result;
}
#endif


void dsa_abort(xrestart)
int	xrestart;
{
static int	  been_here = 0;
#ifdef SBRK_DEBUG
unsigned proc_size;
#endif
#ifdef NOTANYMORE
struct task_act	* task;
int		  secs;
#endif
	if (been_here++)	
		return;

	quipu_shutdown = TRUE;
	restart = xrestart;

#ifdef SBRK_DEBUG
        proc_size = (unsigned) sbrk(0);
        LLOG (log_dsap, LLOG_NOTICE, ("Stop size = %d bytes", proc_size));
#endif

	watch_dog_final(clean_exit);	
		/* Set 5 min timer to force death ! */

	stop_listeners();

#ifdef NOTANYMORE

	for (;;)
	{
	    if ((task = task_select(&secs)) == NULLTASK) {
		if (check_conns(secs)) {
			watch_dog_final_reset();
			return;
		    } else
			(void) dsa_wait(1);
	    } else
	    {
		dsa_work(task);	/* Process the DSA task selected */
		(void) dsa_wait(0);
	    }
	} 

#endif
}

SFD stop_dsa (sig)
int sig;
{

	(void) signal (sig, SIG_DFL); /* to stop recursion */

	LLOG (log_dsap,LLOG_FATAL,("*** Stopping on signal %d ***",sig));

	dsa_abort(0);

	if (debug) {
	   (void) fprintf (stderr,"DSA %s has Stopped\n",mydsaname);
#ifdef DEBUG
	   directory_free (database_root);
	   as_free (dsa_real_attr);
	   as_free (dsa_pseudo_attr);
	   dn_free (mydsadn);
	   free ((char *) mydsaname);
	   free_oid_table();
	   tailorfree();
	   free_pepsy_bp();
	   free_static_oid();
	   free_isode_alias();
	   free_parents();
	   free_all_list_cache();
	   free_shadow_lists();
	   avs_free (super_user);
#endif
        }

	exit (0);
}

#ifdef	SIGUSR1
/* ARGSUSED */

SFD	list_status (sig)
int	sig;
{
#ifdef SBRK_DEBUG
    unsigned proc_size;
#endif
    int	    fd;
    struct stat st;
    register struct connection *cn;
    time_t  now;
    
#ifndef	BSD42
    (void) signal (SIGUSR1, list_status);
#endif

#ifdef SBRK_DEBUG
    proc_size = (unsigned) sbrk(0);
    LLOG (log_dsap, LLOG_EXCEPTIONS, ("size = %d bytes", proc_size));
#endif

    for (fd = getdtablesize () - 1; fd >= 0; fd--)
	if (fstat (fd, &st) != NOTOK) 
	    LLOG (log_dsap, LLOG_EXCEPTIONS,
		  ("fd %d: fmt=0%o", fd, st.st_mode & S_IFMT));
        else if (errno != EBADF) {
#ifdef SUN_X25
	    if (errno == EOPNOTSUPP) {
		    X25_PKT_STAT_DB pkt_stats;
		    if (ioctl (fd, X25_RD_PKT_STATISTICS, &pkt_stats) != 0) {
			    if (errno == EINVAL) 
			       LLOG (log_dsap, LLOG_EXCEPTIONS,
				  ("*** fd %d, X25 listener ?", fd, errno));
			    else
			       LLOG (log_dsap, LLOG_EXCEPTIONS,
				  ("*** fd %d, X25 errno=%d ***", fd, errno));
		    } else {
			  LLOG (log_dsap, LLOG_EXCEPTIONS,
			  ("fd %d, X25:  st=%d, sub_st=%d, flag=%x, reset=%d", 
				   fd, pkt_stats.state, pkt_stats.sub_state,
				   pkt_stats.flags,pkt_stats.resetcnt));
		    }
	    } else
#endif
	    LLOG (log_dsap, LLOG_EXCEPTIONS,
		  ("*** fd %d: errno=%d ***", fd, errno));
        }
#ifndef	NO_STATS
    LLOG (log_dsap, LLOG_EXCEPTIONS, ("logs dsap=%d stat=%d",
				  log_dsap -> ll_fd, log_stat -> ll_fd));
#else
    LLOG (log_dsap, LLOG_EXCEPTIONS, ("logs dsap=%d", log_dsap -> ll_fd));
#endif

    (void) time (&now);
    for (cn = connlist; cn; cn = cn -> cn_next)
	if (cn -> cn_ad != NOTOK) 
	    LLOG (log_dsap, LLOG_EXCEPTIONS,
		  ("cn %d: init=%d used=%ld release=%ld",
		   cn -> cn_ad, cn -> cn_initiator,
		   (long) now - cn -> cn_last_used,
		   (long) now - cn -> cn_last_release));
}
#endif


#ifdef	SIGUSR2
/* ARGSUSED */

SFD	list_status2 (sig)
int	sig;
{
#ifdef SBRK_DEBUG
    unsigned proc_size;
#endif
    register struct connection *cn;
    
#ifndef	BSD42
    (void) signal (SIGUSR2, list_status2);
#endif

#ifdef SBRK_DEBUG
    proc_size = (unsigned) sbrk(0);
    LLOG (log_dsap, LLOG_EXCEPTIONS, ("size = %d bytes", proc_size));
#endif

    for (cn = connlist; cn; cn = cn -> cn_next)
	if (cn -> cn_ad != NOTOK) 
		conn_log(cn,LLOG_EXCEPTIONS);
}
#endif

static  envinit () {
    int     i,
	    sd;

    nbits = getdtablesize ();

    if (!(debug = isatty (2))) {
	for (i = 0; i < 5; i++) {
	    switch (fork ()) {
		case NOTOK:
		    sleep (5);
		    continue;

		case OK:
		    goto fork_ok;

		default:
		    _exit (0);
	    }
	    break;
	}

fork_ok:;
	if ((sd = open ("/dev/null", O_RDWR)) == NOTOK)
	    adios ("/dev/null", "unable to read");
	if (sd != 0)
	    (void) dup2 (sd, 0), (void) close (sd);
	(void) dup2 (0, 1);
	(void) dup2 (0, 2);

#ifdef	SETSID
	if (setsid () == NOTOK)
	    advise (LLOG_EXCEPTIONS, "failed", "setsid");
#endif
#ifdef  TIOCNOTTY
	if ((sd = open ("/dev/tty", O_RDWR)) != NOTOK) {
	    (void) ioctl (sd, TIOCNOTTY, NULLCP);
	    (void) close (sd);
	}
#else
#ifdef  SYS5
	(void) setpgrp ();
	(void) signal (SIGINT, SIG_IGN);
	(void) signal (SIGQUIT, SIG_IGN);
#endif
#endif
    }
#ifndef	DEBUG
    /* "Normal" ISODE behavior of full logging only without DEBUG */
    else
	ll_dbinit (log_dsap, myname);
#endif

#ifndef	sun		/* damn YP... */
    for (sd = 3; sd < nbits; sd++) {
	if (log_dsap -> ll_fd == sd)
	    continue;
#ifndef	NO_STATS
	if (log_stat -> ll_fd == sd)
	    continue;
#endif
	(void) close (sd);
    }
#endif

    (void) signal (SIGPIPE, SIG_IGN);

    ll_hdinit (log_dsap, myname);
#ifdef	DEBUG
    advise (LLOG_TRACE, NULLCP, "starting");
#endif
}


/* 	ERRORS */

#ifndef	lint
void    adios (va_alist)
va_dcl
{
    va_list ap;

    va_start (ap);

    _ll_log (log_dsap, LLOG_FATAL, ap);

    va_end (ap);

    if (debug)
	(void) fprintf (stderr,"adios exit - see dsap.log\n");
    dsa_abort(0);
    _exit (-18);
}
#else
/* VARARGS */

void    adios (what, fmt)
char	*what,
	*fmt;
{
    adios (what, fmt);
}
#endif

#ifndef	lint
void    advise (va_alist)
va_dcl
{
    int     code;
    va_list ap;

    va_start (ap);

    code = va_arg (ap, int);

    (void) _ll_log (log_dsap, code, ap);

    va_end (ap);
}
#else
/* VARARGS */

void	advise (code, what, fmt)
char	*what,
	*fmt;
int	 code;	
{
    advise (code, what, fmt);
}
#endif



static setdsauid ()
{
struct stat buf;
extern char * treedir;

	(void) stat (treedir,&buf);

	if (setgid (buf.st_gid) == -1)
		LLOG (log_dsap,LLOG_NOTICE,("Can't set gid %d (database directory \"%s\")",buf.st_uid,treedir));

	if (setuid (buf.st_uid) == -1)
		LLOG (log_dsap,LLOG_EXCEPTIONS,("Can't set uid %d (database directory \"%s\")",buf.st_uid,treedir));
}



#define	RESTART_TIME	30	/* for connections to clear... */
#ifdef  SO_REUSEADDR
#define CLEAR_TIME	300	/* Was RESTART_TIME, but SO_REUSEADDR */
				/* may not be working yet! */
#else
#define	CLEAR_TIME	300	/*   .. */
#endif

SFD attempt_restart (sig)
int sig;
{
int sd;
static int here_again = 0;

	if (here_again) {
		for (sd = 0; sd < NSIG ; sd++)
			(void) signal (sd, SIG_DFL); /* to stop recursion */

		if (here_again++ > 1) {
			fatal (1, "Signal looping");
			exit (1);	/* We're in a real mess */
		}
		do_restart (sig);
	}
	here_again++;

	for (sd = 0; sd < NSIG ; sd++)
		(void) signal (sd, SIG_DFL); /* to stop recursion */

	(void) signal (SIGQUIT, attempt_restart);
	(void) signal (SIGILL,  attempt_restart);
	(void) signal (SIGBUS,  attempt_restart);
	(void) signal (SIGSEGV, attempt_restart);
	(void) signal (SIGSYS,  attempt_restart);

	if (sig >= 0 && debug)
		(void) fprintf (stderr,"DSA %s has a problem\n",mydsaname);

        dsa_abort (1);

        do_restart (sig);
}

do_restart (sig)
int     sig;
{
int fpid, sd;
unsigned int secs;
extern char * mydsaname;

	secs = sig != NOTOK ? CLEAR_TIME : RESTART_TIME;

	for (sd = 3; sd < nbits; sd++) {
	    if (log_dsap -> ll_fd == sd)
		continue;
#ifndef	NO_STATS
	    if (log_stat -> ll_fd == sd)
		continue;
#endif
	    (void) close (sd);
	}

	if ( sig == -2 || (fpid = fork()) == 0) {
		if (sig == -2) {		    /* restart due to congestion... */
			LLOG (log_dsap,LLOG_FATAL, ("*** in-situ restart attempted ***"));
#ifndef NO_STATS
			LLOG (log_stat,LLOG_NOTICE,("RESTARTING (%s)",mydsaname));
#endif
	    	}

		sleep (secs);	/* give connections time to clear */
		(void) execv (isodefile(sargv[0], 1),sargv);
		exit (-19);
	}

	log_dsap -> ll_syslog = LLOG_FATAL;

	if (fpid != -1) {
		LLOG (log_dsap,LLOG_FATAL,("Quipu restart attempted in %d seconds (sig %d)", secs,sig));
#ifndef NO_STATS
		LLOG (log_stat,LLOG_NOTICE,("RESTARTING with pid %d (%s)",fpid,mydsaname));
#endif
	} else {
#ifndef NO_STATS
		LLOG (log_stat,LLOG_NOTICE,("PANIC (%s)",mydsaname));
#endif
		LLOG (log_dsap,LLOG_FATAL,("Quipu aborting - sig (%d)",sig));
	}

	abort ();
	exit (-20);  /* abort should not return */
}

#ifdef QUIPU_CONSOLE
void
open_call_avs_clearup()
{
  AttributeType at ;
  AV_Sequence tmp_avs = open_call_avs ;
  AV_Sequence t1, t2 ;
  struct quipu_call * tmp_oc ;
  extern time_t timenow ;
  time_t tmp_time ;

  if (open_call_avs == NULLAV)
    return ;

  if (open_call_avs->avseq_av.av_struct == (caddr_t) 0)
  {
    fprintf(stderr, "This should never happen. Check the tidy routines!\n") ;
    at = AttrT_new("openCall") ;
    
    Remove_openCall_attribute() ;    
    
    free((char *)open_call_avs) ;
    open_call_avs = NULLAV ;
    return ;
  }

  while (tmp_avs != NULLAV)
  {
    t2 = tmp_avs->avseq_next ;
    tmp_oc = (struct quipu_call *) tmp_avs->avseq_av.av_struct ;
    if ((tmp_oc->finish_time != (char *) 0) &&                          /* The call session has finished */
	(timenow > gtime(ut2tm(str2utct(tmp_oc->finish_time, strlen(tmp_oc->finish_time)))) + 300)) 
    {                                                                   /* and past its sell by date */
      if ((tmp_avs == open_call_avs) && (tmp_avs->avseq_next == NULLAV))
      {
	/* If there is only one item, then remove the attribute completely. */
	Remove_openCall_attribute() ;
	open_call_avs = NULLAV ;           /* Reset this pointer to say that it is empty. */
#ifdef SPT_DEBUG
fprintf(stderr, "There are no more open_call_avs's. All timed out.\n") ;
#endif
      }
      else
      if (tmp_avs == open_call_avs)
      {
	/* There are many attributes, and we want to remove the first one. */
	open_call_avs = tmp_avs->avseq_next ;
	tmp_avs->avseq_next = NULLAV ;
	Remove_openCall_attribute() ;
	at = AttrT_new("openCall") ;
	dsa_pseudo_attr = as_merge (dsa_pseudo_attr, as_comp_new (at, open_call_avs, NULLACL_INFO)) ;
	AttrT_free(at) ;
      }
      else
      {
	/* There are many attribute values and we want to remove one of them, but not the first */
	t1 = open_call_avs ;
	while ((t1 != NULLAV) &&
	       (t1->avseq_next != tmp_avs))
	{
	  t1 = t1->avseq_next ;
	}
	t1->avseq_next = tmp_avs->avseq_next ;
	tmp_avs->avseq_next = NULLAV ;
	avs_comp_free(tmp_avs) ;
      }	  
    }
    tmp_avs = t2 ;
  }
}

void
Remove_openCall_attribute()
{ 
  extern Attr_Sequence dsa_pseudo_attr ;
  AttributeType at ;
  Attr_Sequence as, trail = NULLATTR ;

  at = AttrT_new("openCall") ;

  for (as=dsa_pseudo_attr; as != NULLATTR; as=as->attr_link) 
  {
    if ((AttrT_cmp (as->attr_type,at)) == 0)
      break;
    trail = as;
  }

  if (as == NULLATTR)       /* there is no attribute here to remove. */
  {
#ifdef SPT_DEBUG
    fprintf(stderr, "SPT: dsa.c: Disaster! Should not get this ever!\n") ;
#endif
    return ;
  }

  if (trail == NULLATTR) 
  {
    /* first in sequence */
    dsa_pseudo_attr = as->attr_link;
    as->attr_link = NULLATTR ;
    avs_free(as) ;
  } 
  else
  {
    trail->attr_link = as->attr_link ;
    as->attr_link = NULLATTR ;
    avs_free(as) ;
  }
  AttrT_free(at) ;
}

#endif /* QUIPU_CONSOLE */



static	osisecinit(argc, argv, fn)
int             *argc;
char          ***argv;
int	fn;
{
    register char   *ap;
    char  **argptr;
static char   *args[10];
static int     argp;
    int narg;

    if(fn == 0) {
	narg = 1;
	argp = 0;
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


