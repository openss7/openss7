/* sys_tai.c - System tailoring routines */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/sys_tai.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/sys_tai.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: sys_tai.c,v $
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
#include "quipu/policy.h"
#include "cmd_srch.h"

extern char *isodelogpath,
	    *treedir,
	    *dsaoidtable,
	    *mydsaname;

char	    *authfile = NULLCP;

extern char startup_update;
extern char quipu_faststart;
extern int  oidformat;
extern int dspchaining;
extern int no_last_mod;
extern int search_level;
extern int read_only;
extern int admin_size;
extern int auth_bind;
extern time_t admin_time;
extern time_t cache_timeout;
extern time_t retry_timeout;
extern time_t slave_timeout;
extern time_t conn_timeout;
extern time_t nsap_timeout;
extern int bind_window;
extern unsigned watchdog_time;
extern unsigned watchdog_delta;
extern LLog * log_dsap;
#ifndef NO_STATS
extern LLog * log_stat;
#endif
#ifdef TURBO_INDEX
extern int optimized_only;
#endif

unsigned bind_policy = POLICY_ACCESS_ALL;
unsigned strong_policy = POLICY_ACCESS_ALL;
extern unsigned str2permission();

#define MLOGDIR          1
#define SYSLOG           2
#define OIDTAB           4
#define OIDFMT           5
#define ROOTDIR          8
#define MYDSANAME        11
#define PARENT           12
#define STATS		 13
#define START_UPDATE	 15
#define DSP_CHAIN	 16
#define SEARCH_LEVEL	 17
#define ISODE_TAILOR	 18
#define LAST_MOD	 19
#define	ADMIN_SIZE	 20
#define	ADMIN_TIME	 21
#define PREFER_DSA	 22
#define	CACHE_TIME	 23
#define	CONN_TIME	 24
#define	SLAVE_TIME	 25
#define	RETRY_TIME	 26
#define CA_INFO		 27
#define SECRET_KEY	 28
#define NSAP_TIME	 29
#define READ_ONLY	 30
#define BIND_WINDOW	 31
#define DSA_CERTIFICATE	 32
#define WATCHDOG_TIME	 34
#define WATCHDOG_DELTA	 35
#define BIND_POLICY	 36
#define STRONG_POLICY	 37
#define AUTH	 	 38
#define SHADOW		 39
#define OPTIMIZE_ATTR	 40
#define OPTIMIZED_ONLY	 41
#define INDEX_SUBTREE	 42
#define INDEX_SIBLINGS	 43
#define REJECT_PREFIX	 44
#define REJECT_LENGTH	 45
#define RELAY_FOR	 46
#define ACCEPT_PREFIX	 47
#define GETEDB_SIZE	 48
#define AUTHFILE	 50
#define FASTSTART	 51

static  CMD_TABLE  cmdtab[] =
{
	"LOGDIR",       MLOGDIR,
	"DSAPLOG",      SYSLOG,
	"OIDTABLE",     OIDTAB,
	"OIDFORMAT",    OIDFMT,
	"TREEDIR",      ROOTDIR,
	"MYDSANAME",    MYDSANAME,
	"PARENT",       PARENT,
	"UPDATE",	START_UPDATE,
	"DSPCHAINING",	DSP_CHAIN,
	"LASTMODIFIED",	LAST_MOD,
	"SEARCHLEVEL",	SEARCH_LEVEL,
	"ISODE",	ISODE_TAILOR,
	"ADMINSIZE",	ADMIN_SIZE,
	"ADMINTIME",	ADMIN_TIME,
	"CACHETIME",	CACHE_TIME,
	"SLAVETIME",	SLAVE_TIME,
	"RETRYTIME",	RETRY_TIME,
	"CONNTIME",	CONN_TIME,
	"NSAPTIME",	NSAP_TIME,
	"CAINFO",	CA_INFO,
	"SECRETKEY",	SECRET_KEY,
	"PREFERDSA",	PREFER_DSA,
	"READONLY",	READ_ONLY,
	"BINDWINDOW",	BIND_WINDOW,
	"DSACERTIFICATE",	DSA_CERTIFICATE,
	"WATCHDOG_TIME",	WATCHDOG_TIME,
	"WATCHDOG_DELTA",	WATCHDOG_DELTA,
	"BIND_POLICY",		BIND_POLICY,
	"STRONG_POLICY",	STRONG_POLICY,
	"AUTHENTICATION",	AUTH,
	"SHADOW",		SHADOW,
	"REJECT_PREFIX",	REJECT_PREFIX,
	"ACCEPT_PREFIX",	ACCEPT_PREFIX,
	"REJECT_LENGTH",	REJECT_LENGTH,
	"RELAY_FOR",		RELAY_FOR,
	"GETEDB_SIZE",		GETEDB_SIZE,
	"AUTHFILE",		AUTHFILE,
	"FAST_START",		FASTSTART,
#ifdef TURBO_INDEX
	"OPTIMIZE_ATTR",	OPTIMIZE_ATTR,
	"OPTIMIZED_ONLY",	OPTIMIZED_ONLY,
	"INDEX_SUBTREE",	INDEX_SUBTREE,
	"INDEX_SIBLINGS",	INDEX_SIBLINGS,
#endif
#ifndef NO_STATS
	"STATS",	STATS,
#endif
	0,              -1,
};

static  CMD_TABLE  authtab[] =
{
	"NONE",		0,
	"DN",		1,
	"SIMPLE",	2,
	"PROTECTED",	3,
	"STRONG",	4,
	0,              0,
};


/*
 * do system wide initialisations
 */

dsa_sys_tai (argc, argv)
char    **argv;
{
	char    *arg;

	if(argc < 2)
		return(NOTOK);
	arg = argv[1];

	switch(cmd_srch(argv[0], cmdtab))
	{
	case MLOGDIR:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor LOGDIR %s", arg));
		isodelogpath = strdup (arg);
		break;
#ifndef NO_STATS
	case STATS:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor STATS %s", arg));
		log_tai(log_stat, &argv[1], argc-1);
		break;
#endif
	case SYSLOG:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor SYSLOG %s", arg));
		log_tai(log_dsap, &argv[1], argc-1);
		break;
	case OIDTAB:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor OIDTable=%s", arg));
		dsaoidtable = strdup (arg);
		if (load_oid_table (dsaoidtable) == NOTOK)
			fatal (-1, "Can't load oid tables");
		break;
	case OIDFMT:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor OIDFMT=%s", arg));
		oidformat = atoi (arg);
		break;
	case ROOTDIR:
	    { 
		unsigned int i;
		treedir = strdup(arg);
		i = strlen(treedir);	
		if ( treedir[i - 1] != '/' ) {
		    treedir = realloc (treedir, i + 1);	
		    treedir[i] = '/';
		    treedir[i + 1] = 0;
		}
		DLOG (log_dsap,LLOG_TRACE,( "Tailor Rootdir %s", treedir));
	    }
		break;
	case MYDSANAME:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor Myname %s", arg));
		mydsaname = strdup(arg);
		break;
	case DSP_CHAIN:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor DSPChaining %s", arg));
		if (lexequ (arg,"whenNeeded") == 0)
			dspchaining = 2;
		else if (lexequ (arg,"off") == 0)
			dspchaining = 1;
		else
			dspchaining = 0;
		break;
	case LAST_MOD:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor last modified %s", arg));
		if (lexequ (arg,"on") == 0)
			no_last_mod = FALSE;
		else
			no_last_mod = TRUE;
		break;
	case START_UPDATE:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor Update %s", arg));
		if (lexequ (arg,"on") == 0)
			startup_update = TRUE;
		else
			startup_update = FALSE;
		break;
	case READ_ONLY:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor read only", arg));
		if (lexequ (arg,"on") == 0)
			read_only = TRUE;
		else
			read_only = FALSE;
		break;
	case SEARCH_LEVEL:
		search_level = atoi (arg);
		DLOG (log_dsap,LLOG_TRACE,(" Tailor search level %d", search_level));
		break;
	case ADMIN_SIZE:
		admin_size = atoi (arg);
		DLOG (log_dsap,LLOG_TRACE,(" Tailor admin size %d", admin_size));
		break;
	case ADMIN_TIME:
		(void) sscanf (arg, "%ld", &admin_time);
		DLOG (log_dsap,LLOG_TRACE,(" Tailor admin time %ld", admin_time));
		break;
	case CACHE_TIME:
		(void) sscanf (arg, "%ld", &cache_timeout);
		DLOG (log_dsap,LLOG_TRACE,(" Tailor cache time %ld", cache_timeout));
		break;
	case RETRY_TIME:
		(void) sscanf (arg, "%ld", &retry_timeout);
		DLOG (log_dsap,LLOG_TRACE,(" Tailor retry time %ld", retry_timeout));
		break;
	case SLAVE_TIME:
		(void) sscanf (arg, "%ld", &slave_timeout);
		DLOG (log_dsap,LLOG_TRACE,(" Tailor slave time %ld", slave_timeout));
		break;
	case CONN_TIME:
		(void) sscanf (arg, "%ld", &conn_timeout);
		DLOG (log_dsap,LLOG_TRACE,(" Tailor conn time %ld", conn_timeout));
		break;
	case NSAP_TIME:
		(void) sscanf (arg, "%ld", &nsap_timeout);
		DLOG (log_dsap,LLOG_TRACE,(" Tailor nsap time %ld", nsap_timeout));
		break;
	case SECRET_KEY:
		DLOG (log_dsap,LLOG_TRACE,(" Tailor dsa secret key ignored - obsolete feature"));
		break;
	case DSA_CERTIFICATE:
		DLOG (log_dsap,LLOG_TRACE,(" Tailor dsa certificate ignored - obsolete feature"));
		break;
	case WATCHDOG_TIME:
		(void) sscanf (arg, "%d", &watchdog_time);
		DLOG (log_dsap,LLOG_TRACE,(" Tailor watchdog time %d", watchdog_time));
		break;
	case WATCHDOG_DELTA:
		(void) sscanf (arg, "%d", &watchdog_delta);
		DLOG (log_dsap,LLOG_TRACE,(" Tailor watchdog delta %d", watchdog_delta));
		break;
	case BIND_POLICY:
		bind_policy = str2permission(arg);
		DLOG(log_dsap,LLOG_TRACE,("Bind policy %d", bind_policy));
		break;
	case STRONG_POLICY:
		strong_policy = str2permission(arg);
		DLOG(log_dsap,LLOG_TRACE,("Strong auth. policy %d", strong_policy));
		break;
	case BIND_WINDOW:
		bind_window = atoi(arg);
		if (bind_window <= 0)
			LLOG(log_dsap,LLOG_EXCEPTIONS,("Invalid bind window %s", arg));
		else
			DLOG (log_dsap, LLOG_TRACE,("Bind window %d", bind_window));
		break;
	case PREFER_DSA:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor prefer dsa %s", arg));
		prefer_dsa (arg);
		break;
	case REJECT_PREFIX:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor reject prefix %s", arg));
		reject_prefix (arg);
		break;
	case ACCEPT_PREFIX:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor accept prefix %s", arg));
		accept_prefix (arg);
		break;
	case REJECT_LENGTH:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor reject length %s", arg));
		reject_length (arg);
		break;
	case RELAY_FOR:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor relay for %s", arg));
		allowrelay (arg);
		break;
	case PARENT:
 		DLOG (log_dsap,LLOG_TRACE,( "Tailor parent name %s, address %s", arg,argv[2]));
 		add_str_parent (arg,argv[2]);
		break;
	case ISODE_TAILOR:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor Isode %s%s",arg,argv[2]));
		if (argc != 3)
			LLOG (log_dsap,LLOG_EXCEPTIONS,( 
			      "Invalid isode option in quiputailor"));
		else
			(void) isodesetvar(arg,strdup(argv[2]),0);
		break;
	case AUTH:
		auth_bind = cmd_srch(arg, authtab);
		DLOG (log_dsap,LLOG_TRACE,( "Tailor authentication", arg));
		break;
	case SHADOW:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor shadow %s", arg));
		shadow_attribute (arg);
		break;
	case GETEDB_SIZE:
		getedb_size (atoi (arg));
		DLOG (log_dsap,LLOG_TRACE,(" Tailor getedb size %d", atoi(arg)));
		break;
	case AUTHFILE:
		DLOG (log_dsap,LLOG_TRACE,( "Tailor authfile %s", arg));
		authfile = strdup(arg);
		break;
	case FASTSTART:
                if (lexequ(arg, "on") == 0)
                        quipu_faststart = TRUE;
                else
                        quipu_faststart = FALSE;
                DLOG(log_dsap, LLOG_TRACE, ("Tailor fast_start %s", arg));
                break;
#ifdef TURBO_INDEX
        case OPTIMIZED_ONLY:
                if (lexequ(arg, "on") == 0)
                        optimized_only = TRUE;
                else
                        optimized_only = FALSE;
                DLOG(log_dsap, LLOG_TRACE, ("Tailor optimized only %s", arg));
                break;
	case OPTIMIZE_ATTR:
		turbo_optimize(arg);
                DLOG(log_dsap, LLOG_TRACE, (" Tailor optimize (%s)", arg));
                break;
        case INDEX_SUBTREE:
                index_subtree(arg);
                DLOG(log_dsap, LLOG_TRACE, ("Tailor index subtree %s", arg));
                break;
        case INDEX_SIBLINGS:
		index_siblings(arg);
                DLOG(log_dsap, LLOG_TRACE, ("Tailor index siblings %s", arg));
                break;
#endif
	    default:
		return NOTOK;
	}
	return (OK);
}

