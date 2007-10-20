/* usconfig.c -- build database directory */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/tools/dsaconfig/RCS/usconfig.c,v 9.0 1992/06/16 12:43:56 isode Rel $";
#endif /* lint */

/*
 * $Header: /xtel/isode/isode/others/quipu/tools/dsaconfig/RCS/usconfig.c,v 9.0 1992/06/16 12:43:56 isode Rel $
 *
 * $Log: usconfig.c,v $
 * Revision 9.0  1992/06/16  12:43:56  isode
 * Release 8.0
 *
 */

/*
 *				NOTICE
 *
 *   Acquisition, use, and distribution of this module and related
 *   materials are subject to the restrictions of a license agreement.
 *   Consult the Preface in the User's Manual for the full terms of
 *   this agreement.
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <errno.h>
#include <varargs.h>
#include <string.h>
#include <sys/file.h>
#include <pwd.h>
#include <grp.h>

#include "general.h"
#include "manifest.h"
#include "internet.h"
#include "psap.h"
#include "tailor.h"

/* ^L */
/* DATA */

char *prgnm;

unsigned char chartered_by_congress;	/* standing derived from congress? */

/* buffers that are used so extensively that they are worth making global */
char buf[BUFSIZ];			/* general purpose buffer */
char line[BUFSIZ];			/* configuration line */
char file[BUFSIZ];			/* a file name */
char oldfile[BUFSIZ];			/* file name (as above) with suffix */

extern void adios (), advise ();

extern int errno;

extern char *isodetcpath;
extern char *isodebinpath;
extern char *isodesbinpath;
extern char *quipuversion;

struct pair {				/* name/value pair */
    char *    p_nm;			/* name of pair */
    char *    p_val;			/* value of pair */
    unsigned char    p_flags;		/* flags */
#define P_NONE	(unsigned char)0x00	/* no flags */
#define P_OPT	(unsigned char)0x80	/* optional */
#define P_MBOX	(unsigned char)0x40	/* a mailbox */
#define P_POST	(unsigned char)0x20	/* a postal address */
};

static struct pair pairs[] = {
	{ "country",		(char *)NULL,	P_OPT },
	{ "wildlife",		(char *)NULL,	P_NONE },
	{ "owner",		(char *)NULL,	P_OPT },
	{ "groupid",		(char *)NULL,	P_OPT },
	{ "state",		(char *)NULL,	P_NONE },
	{ "dsa",		(char *)NULL,	P_NONE },
	{ "ipaddr",		(char *)NULL,	P_OPT },
	{ "port",		(char *)NULL,	P_OPT },
	{ "organization",	(char *)NULL,	P_NONE },
	{ "unit",		(char *)NULL,	P_NONE },
	{ "domain",		(char *)NULL,	P_NONE },
	{ "street",		(char *)NULL,	P_OPT },
	{ "town",		(char *)NULL,	P_NONE },
	{ "zipcode",		(char *)NULL,	P_NONE },
	{ "telephone",		(char *)NULL,	P_OPT },
	{ "pob",		(char *)NULL,	P_OPT },
	{ "fax",		(char *)NULL,	P_OPT },
	{ "description",	(char *)NULL,	P_OPT },
	{ "orgcode",		(char *)NULL,	P_OPT },
	{ "firstname",		(char *)NULL,	P_NONE },
	{ "lastname",		(char *)NULL,	P_NONE },
	{ "middlename",		(char *)NULL,	P_OPT },
	{ "middleinitial",	(char *)NULL,	P_OPT },
	{ "mailbox",		(char *)NULL,	P_MBOX },
	{ "title",		(char *)NULL,	P_OPT },
	{ "password",		(char *)NULL,	P_NONE },
	{ "persphone",		(char *)NULL,	P_OPT },
	{ "extension",		(char *)NULL,	P_OPT },
	{ "userid",		(char *)NULL,	P_NONE },
	{ "postaladdress",	(char *)NULL,	(P_OPT | P_POST) },
/* DO NOT DELETE THIS -- THIS TERMINATES THE ARRAY */
	{ (char *)NULL,		(char *)NULL,	P_NONE }
};

struct state {
    char *    s_nm;			/* full name of state */
    char *    s_abb;			/* 2 letter abbreviation */
    short     s_fips;			/* FIPS numeric code */
    short     s_region;			/* region */
};

struct state usstates[] = {
	{ "Alabama",				"AL",		 1,	0 },
	{ "Alaska",				"AK",		 2,	1 },
	{ "Arizona",				"AZ",		 4,	2 },
	{ "Arkansas",				"AR",		 5,	2 },
	{ "California",				"CA",		 6,	1 },
	{ "Colorado",				"CO",		 8,	2 },
	{ "Connecticut",			"CT",		 9,	0 },
	{ "Delaware",				"DE",		10,	0 },
	{ "District of Columbia",		"DC",		11,	0 },
	{ "Florida",				"FL",		12,	0 },
	{ "Georgia",				"GA",		13,	0 },
	{ "Hawaii",				"HI",		15,	1 },
	{ "Idaho",				"ID",		16,	2 },
	{ "Illinois",				"IL",		17,	2 },
	{ "Indiana",				"IN",		18,	2 },
	{ "Iowa",				"IA",		19,	2 },
	{ "Kansas",				"KS",		20,	2 },
	{ "Kentucky",				"KY",		21,	0 },
	{ "Louisiana",				"LA",		22,	2 },
	{ "Maine",				"ME",		23,	0 },
	{ "Maryland",				"MD",		24,	0 },
	{ "Massachusetts",			"MA",		25,	0 },
	{ "Michigan",				"MI",		26,	2 },
	{ "Minnesota",				"MN",		27,	2 },
	{ "Mississippi",			"MS",		28,	2 },
	{ "Missouri",				"MO",		29,	2 },
	{ "Montana",				"MT",		30,	2 },
	{ "Nebraska",				"NE",		31,	2 },
	{ "Nevada",				"NV",		32,	1 },
	{ "New Hampshire",			"NH",		33,	0 },
	{ "New Jersey",				"NJ",		34,	0 },
	{ "New Mexico",				"NM",		35,	2 },
	{ "New York",				"NY",		36,	0 },
	{ "North Carolina",			"NC",		37,	0 },
	{ "North Dakota",			"ND",		38,	2 },
	{ "Ohio",				"OH",		39,	0 },
	{ "Oklahoma",				"OK",		40,	2 },
	{ "Oregon",				"OR",		41,	1 },
	{ "Pennsylvania",			"PA",		42,	0 },
	{ "Rhode Island",			"RI",		44,	0 },
	{ "South Carolina",			"SC",		45,	0 },
	{ "South Dakota",			"SD",		46,	2 },
	{ "Tennessee",				"TN",		47,	0 },
	{ "Texas",				"TX",		48,	2 },
	{ "Utah",				"UT",		49,	2 },
	{ "Vermont",				"VT",		50,	0 },
	{ "Virginia",				"VA",		51,	0 },
	{ "Washington",				"WA",		53,	1 },
	{ "West Virginia",			"WV",		54,	0 },
	{ "Wisconsin",				"WI",		55,	2 },
	{ "Wyoming",				"WY",		56,	2 },
	{ "American Samoa",			"AS",		60,	1 },
	{ "Federated States of Micronesia",	"FM",		64,	1 },
	{ "Guam",				"GU",		66,	1 },
	{ "Marshall Islands",			"MH",		68,	1 },
	{ "Northern Mariana Islands",		"MP",		69,	1 },
	{ "Palau",				"PW",		70,	1 },
	{ "Puerto Rico",			"PR",		72,	2 },
	{ "U.S. Minor Outlying Islands",	"PM",		74,	1 },
	{ "Virgin Islands of the U.S.",		"VI",		78,	1 },
/* DO NOT DELETE THIS -- THIS TERMINATES THE ARRAY */
	{ (char *)NULL,				(char *)NULL,	 0,	0 }
};

struct region {
    char *    dsa_name;				/* name of parent dsa */
    char *    dsa_addr;				/* address of parent dsa */
};

#define N_REGIONS	3			/* regions for states above */

struct region regional_dsas[N_REGIONS+1];

struct edbdir {
    char *    dir;				/* path */
    char *    edb;				/* name of 'template' file */
};

struct edbdir topdirs[] = {
	{ ".",			"../templates/root.edb" },
	{ "c=US",		"../templates/us/us.edb" },
/* DO NOT DELETE THIS -- THIS TERMINATES THE ARRAY */
	{ (char *)NULL,		(char *)NULL, }
};
 
/* locations of temporary entries */
#define ORGENTRY	"org.tmp"		/* temporary org. entry */
#define DSAENTRY	"dsa.tmp"		/* temporary dsa entry */

/* template files */
#define ORGTMPL		"../templates/us/organization.edb" /* org. entry */
#define ANSI_ORGTMPL	"../templates/us/ansiorg.edb" /* ansi org. entry */
#define DSATMPL		"../templates/us/dsa.edb" /*  DSA template */
#define ORGEDBTMPL	"../templates/us/orgedb.edb" /* org. entries template */
#define OUEDBTMPL	"../templates/us/ouedb.edb" /* ou. entries template */
#define QUIPUTMPL	"../templates/us/quiputailor" /* US quiputailor */
#define DSAPTMPL	"../templates/us/dsaptailor" /* US dsaptailor */
#define STARTUPTMPL	"../templates/startup.sh" /* startup.sh */
#define NIGHTLYTMPL	"../templates/nightly.sh" /* nightly.sh */
#define FREDTMPL	"../templates/fredrc"	/* fredrc */
#define UFNTMPL		"../templates/us/ufnrc"	/* ufnrc */

/* general #defines */
#define DIRPERM			0711		/* directory permissions */
#define SEDFILE			"sed.tmp"	/* sed filename */
#define USDN			"c=US"		/* DN for the US */

/*  */
/* ARGSUSED */

/* main () -- main function for program */
void
main (argc, argv)
int  argc;
char **argv;
{
    struct pair *pp;			/* pair pointer */
    extern struct pair * findpair ();
    extern void arginit (), read_config (), read_dsas ();
    extern void build_top (), add_us (), add_state (), build_dsa ();
    extern void build_orgedb (), build_ouedb ();
    extern void build_tailor (), build_scripts (), build_uifl();
    extern void create_sedfile (), build_mesgfl ();
    extern void set_permissions ();

    chartered_by_congress = 0x00;	/* assume not congress-chartered */

/* get configuration */
    arginit (argc, argv);		/* initialize from command line */
    read_dsas ();			/* read in regional dsas */
    read_config ();			/* read configuration file */

/* create database directory */
    if ((pp = findpair ("wildlife", 0x00)) == (struct pair *)NULL)
       adios (NULLCP, "no database directory found in configuration");
    else
      if (access (pp->p_val, F_OK) == 0)
        adios (NULLCP, "\"%s\" already exists -- use another name", pp->p_val);

/*build root, c=US */
/* 
   an important side-effect of this function is to change the working
   directory for this program to $(ETCDIR)/quipu/wildlife
*/
    build_top ();			/* build top-level of DIT */

/* create sed file. depends on being in $(ETCDIR)/quipu/wildlife */
    create_sedfile ();			/* create sed file */

/* build organizational entry */
    if ((pp = findpair ("orgcode", 0x00)) == (struct pair *)NULL)
      if (chartered_by_congress) {
        add_us ((struct pair *)NULL);	/* build US EDB with org. in it */
        add_state (0x00);		/* build empty state EDB */
      }
      else {				/* org. has regional standing */
        /* organization with regional standing */
        add_state (0x01);		/* build state EDB with organization */
      }
    else {				/* organization with national standing*/
      add_us (pp);			/*build US EDB with organization in it*/
      add_state (0x00);			/* build empty state EDB */
    }

/* build DSA entry in c=US */
    build_dsa ();

/* build skeleton EDB for organization and organizational unit */
    build_orgedb ();
    build_ouedb ();

/* build tailor, script and configuration files */
    build_tailor ();			/* build tailor files */

    build_scripts ();			/* build scripts */

    build_uifl ();			/* build user interface configurations*/

    unlink (SEDFILE);

/* create message file */
    build_mesgfl();

/* set owner/group permissions */
    set_permissions ();

    exit (0);
}

/*  */
/* arginit () -- parse command line, and initialize from it */
void
arginit (ac,av)
int ac;
char **av;
{
    int i,j,k;
    struct pair *wildlife;

    extern struct pair *findpair ();
    extern void make_usstates ();

/* get name of program */
    if ((prgnm = strrchr (av[0], (int)'/')) == (char *)NULL)
      prgnm = av[0];			/* use entire token as program name */
    else
      prgnm++;				/* skip past trailing '/' */

/* tailor application */
    isodetailor (prgnm, 1);

/* get pointer to wildlife pair structure */
    if ((wildlife = findpair ("wildlife", 0x01)) == (struct pair *)NULL)
      adios (NULLCP, "internal error: cannot find wildlife pair");

/* parse command line -- currently there are no command line options */
    for (i = 1; i < ac; ++i)		/* iterate through command line */
       if (av[i][0] == '-')		/* command line option? */
         for (j = 1; av[i][j] != '\0'; ++j) /* iterate through all options */
            switch (av[i][j]) {		/* identify command line option */
	      case 'c':			/* chartered by congress */
                chartered_by_congress = 0x7f;
	        break;

	      case 'l':			/* list U.S. states */
                for (k = 0; usstates[k].s_nm != (char *)NULL; ++k)
		   printf ("%s\n", usstates[k].s_nm);
	        exit (0);

/** INTERNAL OPTIONS -- for the use of wpp-manager */
              case 's':			/* generate U.S. state entries */
                make_usstates ();
                exit (0);
              default:
		advise(NULLCP, "illegal switch -%c", av[i][j]);
		break;
            }				/* end, switch */
       else
         if (wildlife->p_val != (char *)NULL) /* file already specified? */
           advise (NULLCP,
		   "wildlife directory already specified -- ignoring %s",
		   av[i]);
         else
	   wildlife->p_val = av[i];

    if (wildlife->p_val == (char *)NULL)
      adios (NULLCP, "usage: %s config-file",prgnm);
}

/*  */
/* read_dsas () -- read in identities and locations of regional DSAs */
void
read_dsas ()
{
    u_short lncnt;			/* linecount */
    char *cp;				/* character pointer */
    char *toks[NVEC+1];			/* tokens on line */
    FILE *fp;				/* file pointer */
    int i;				/* index and counter */

/* initializations */
    bzero (line, BUFSIZ);
    bzero (file, BUFSIZ);
    bzero ((char *)(regional_dsas), (N_REGIONS+1 * sizeof (struct region)));

/* open file containing names/identities of regional dsas */
    sprintf (file, "templates/us/dsas");

    if ((fp = fopen (file, "r")) == (FILE *)NULL)
      adios (file, "unable to read");

    lncnt = 0;
    for (lncnt = 1, i = 0;
         i < N_REGIONS && fgets (line, BUFSIZ-1, fp) != (char *)NULL;
         lncnt++) {

/* check for comment line ... */
       if (line[0] == '#')		/* a comment? */
         continue;

/* get rid of trailing NL */
       if ((cp = index (line, '\n')) != (char *)NULL)
         *cp = '\0';			/* get rid of NL */

/* tokenize line */
       switch (str2vec (line, toks)) {	/* tokenize line */
         case 0:			/* blank line */
           continue;

         case 1:			/* syntax error ... */
           advise (NULLCP, "syntax error, line %d: insufficient parameters",
		   lncnt);
           continue;

         case 2:
           break;

         default:
           advise (NULLCP, "syntax error, line %d: too many parameters",
		   lncnt);
           continue;
       }				/* end, tokenize line */

/* fill in regional_dsas structure */
       regional_dsas[i].dsa_name = strdup (toks[0]);
       regional_dsas[i++].dsa_addr = strdup (toks[1]);
    }					/* end, foreach line */

    if (i != N_REGIONS)			/* too few dsas */
      adios (NULLCP, "dsas specified for too few regions, %d", i);

    fclose (fp);
}
/*  */
/* read_config () -- parse configuration file, and initialize from it */
void
read_config ()
{
    u_short lncnt;			/* linecount */
    char *toks[NVEC+1];			/* tokens on line */
    char *cp,*cp2;			/* character pointers */
    struct pair *pp, *pp2, *pp3;	/* pointers to a pair struct. */
    FILE *cfgfp;			/* configuration file ptr. */
    int i;				/* index and counter */
    short nmlen;			/* length of a name */
    struct hostent *hp;			/* for host structure */
    struct sockaddr_in sin;		/* for local address */
    struct state *sp;			/* to save 'state' */

    extern struct pair *findpair ();
    extern char * strdup ();
    extern void bad_postaladdress ();

    lncnt = 0;
    bzero (buf, BUFSIZ);		/* clear global general purpose buffer*/
    bzero (line, BUFSIZ);		/* clear global general purpose line */
    bzero (file, BUFSIZ);		/* clear global general purpose file */

/* open configuration file */
    if ((pp = findpair ("wildlife", 0x00)) == (struct pair *)NULL)
      adios (NULLCP, "no configuration file specified");

    sprintf(file,"%s.dsa",pp->p_val);
    if ((cfgfp = fopen (file, "r")) == (FILE *)NULL)
      adios (file, "unable to read");

/* run through each line of configuration file */
    while (fgets(line, BUFSIZ-1, cfgfp) != (char *)NULL) {

       lncnt++;				/* add to linecount */

       if (line[0] == '#')		/* comment line? */
         continue;

       if ((cp = index (line, '\n')) != (char *)NULL)
         *cp = '\0';			/* get rid of NL */

       switch (str2vec (line, toks)) {	/* tokenize line */
          case 0:
	    continue;

          case 1:
            if ((pp = findpair (toks[0], 0x01)) == (struct pair *)NULL)
              advise (NULLCP, "unknown variable %s, line %d", toks[0], lncnt);
            else
	      advise (NULLCP, "no value specified for variable %s, line %d",
			toks[0], lncnt);
            continue;

	  case 2:
	    break;

	  default:
	    advise (NULLCP, "syntax error: multiple values specified, line %d",
			lncnt);
	    continue;
       }				/* end, tokenize line */

/* get variable */
       if ((pp = findpair (toks[0], 0x01)) == (struct pair *)NULL) {
         advise (NULLCP, "unknown variable %s, line %d", toks[0], lncnt);
         continue;			/* process another line */
       }

       if (pp->p_val != (char *)NULL) {	/* multiple values? */
	 advise (NULLCP, "duplicate value for variable %s found, line %d",
			pp->p_nm, lncnt);
         continue;			/* process another line */
       }

/* make sure character set used for value is legal */
       if ((pp->p_flags & P_MBOX) == P_MBOX) {
         for (i = 0; toks[1][i] != '\0'; ++i)
            if (!isascii (toks[1][i]) || toks[1][i] == '$' ||
		toks[1][i] == '&') {
	      advise (NULLCP,
		      "illegal character %c (0%o) in mailbox value, line %d",
		      toks[1][i], (toks[1][i] & 0xff), lncnt);
	      break;
	    }
         if (toks[1][i] != '\0')
	   continue;			/* process another line */
       }				/* end, mailbox */
       else {				/* not mailbox */
	 for (i = 0; toks[1][i] != '\0'; ++i) {
            if (isalpha (toks[1][i]) || isdigit (toks[1][i]))
	      continue;			/* onto next character */

            switch (toks[1][i]) {	/* identify character */
              case 047:				/* ' */
              case '(':
              case ')':
              case '+':
              case ',':
              case '-':
              case '.':
              case '/':
              case ':':
              case '?':
              case ' ':
		continue;

	      default:			/* not a legal character */
                if ((pp->p_flags & P_POST) == P_POST && toks[1][i] == '$')
		   continue;		/* '$' legal in postal addr. */

		advise (NULLCP, "illegal character %c (0%o), line %d",
			toks[1][i], (toks[1][i] & 0xff), lncnt);
		goto out;
	    }
         }				/* end, foreach character */
out:;
         if (toks[1][i] != '\0')
	   continue;			/* process next line */
       }				/* end, not mailbox */
       pp->p_val = strdup (toks[1]);
    }					/* end, foreach line */

/*
   done parsing configuration file. Now do error checks and fill in things
*/
/* make sure all mandatory values have been specified */
   for (i = 0; pairs[i].p_nm != (char *)NULL; ++i)
      if (pairs[i].p_val == (char *)NULL &&
	  (pairs[i].p_flags & P_OPT) != P_OPT)
        adios (NULLCP, "value not specified for variable %s",pairs[i].p_nm);

/* initialize country value */
    if ((pp = findpair ("country", 0x01)) == (struct pair *)NULL)
      adios (NULLCP, "internal error: cannot get 'country' variable");

    if (pp->p_val == (char *)NULL)
      pp->p_val = strdup ("US");
    else
      if (strlen (pp->p_val) != 2 || strcmp (pp->p_val, "US") != 0)
        adios (NULLCP, "This program can only be used for configuration in the US DMD!");

/* check for legality of state value */
    if ((pp = findpair ("state", 0x00)) == (struct pair *)NULL)
      adios (NULLCP, "mandatory 'state' variable not specified");
      
    nmlen = strlen(pp->p_val);

    for (i = 0; usstates[i].s_nm != (char *)NULL; ++i)
       if (nmlen == strlen(usstates[i].s_nm) && 
	   strcmp (pp->p_val,usstates[i].s_nm) == 0)
          break;

    sp = (struct state *)NULL;
    if (usstates[i].s_nm == (char *)NULL)
      adios (NULLCP, "%s not a legal U.S. state or state equivalent",
		pp->p_val);
    else
      sp = (usstates + i);		/* save for later use */

/* initialize address, port, on which DSA listens if necessary */
    if ((pp = findpair ("ipaddr",0x01)) == (struct pair *)NULL)
      adios (NULLCP, "internal error: cannot find 'ipaddr' pair");

    if (pp->p_val == (char *)NULL) {	/* ip address not specified? */
      if ((hp = gethostbyname (cp = getlocalhost())) == (struct hostent *)NULL)
        adios (NULLCP, "%s: unknown host", cp);

      bzero ((char *)&sin, sizeof (struct sockaddr_in));
      inaddr_copy (hp, &sin);

      pp->p_val = strdup (inet_ntoa (sin.sin_addr));
    }					/* end, ip address not specified */
    else {				/* ip address specified */
      if ((hp = gethostbystring (pp->p_val)) == (struct hostent *)NULL)
	adios (NULLCP, "%s: illegal ip address", pp->p_val);
    }					/* end, ip address specified */

    if ((pp = findpair ("port", 0x01)) != (struct pair *)NULL &&
	pp->p_val == (char *)NULL) {
      pp->p_val = strdup ("17003");
    }

/* initialize middle initial based on middle name, if necessary */
    if ((pp = findpair ("middlename", 0x00)) != (struct pair *)NULL &&
	(pp2 = findpair ("middleinitial", 0x01)) != (struct pair *)NULL &&
	pp2->p_val == (char *)NULL) {
      buf[0] = pp->p_val[0];
      buf[1] = '\0';

      pp2->p_val = strdup (buf);
    }

/* initialize personal phone, if possible */
    if ((pp = findpair ("persphone", 0x01)) != (struct pair *)NULL &&
	pp->p_val == (char *)NULL)
      if ((pp2 = findpair ("telephone", 0x00)) != (struct pair *)NULL)
        if ((pp3 = findpair ("extension", 0x00)) != (struct pair *)NULL) {
          sprintf(buf,"%s x%s",pp2->p_val,pp3->p_val);
	  pp->p_val = strdup (buf);
        }
        else {
	  sprintf (buf, "%s", pp2->p_val);
	  pp->p_val = strdup (buf);
        }

/* initialize postal address, if necessary */
    if ((pp = findpair ("postaladdress", 0x01)) != (struct pair *)NULL &&
	pp->p_val == (char *)NULL) {

      cp = buf;				/* start at the beginning ... */
      bzero (buf, BUFSIZ);		/* clear buffer */

/* put in organization */
      if ((pp2 = findpair ("organization", 0x00)) == (struct pair *)NULL)
        adios (NULLCP, "'organization' not specified");
      else
        if (strlen(pp2->p_val) > 30) {
	  bad_postaladdress (pp2->p_val, 0x00);
        }

      sprintf(cp, "%s ", pp2->p_val);
      cp += (strlen (pp2->p_val) + 1);

/* put in po box or street address */
      if ((pp2 = findpair ("pob", 0x00)) == (struct pair *)NULL)
        if ((pp2 = findpair ("street", 0x00)) == (struct pair *)NULL)
          adios (NULLCP,
		 "must specify one of 'postaladdress', 'street' or 'pob'");
        else {				/* street address exists */
          if (strlen(pp2->p_val) > 30)
            bad_postaladdress (pp2->p_val, 0x00);

          sprintf(cp, "$ %s ", pp2->p_val);
          cp += (strlen (pp2->p_val) + 3);
        }				/* end, street address exists */
      else {				/* P.O. Box exists */
        if (strlen(pp2->p_val) + 4 > 30)
          bad_postaladdress (pp2->p_val, 0x00);

        sprintf(cp, "$ POB %s ", pp2->p_val);
        cp += (strlen (pp2->p_val) + 7);
      }					/* end, P.O. Box exists */

/* put in the town */
      if ((pp2 = findpair ("town", 0x00)) == (struct pair *)NULL)
        adios (NULLCP, "'town' not specified");
      else
       if (strlen(pp2->p_val) > 30)
         bad_postaladdress (pp2->p_val, 0x00);

      sprintf (cp, "$ %s ", pp2->p_val);
      cp += (strlen (pp2->p_val) + 3);

/* put in state */
      if (sp == (struct state *)NULL)	/* no state? */
        adios (NULLCP, "'state' not specified");

      sprintf (cp, "$ %s ",sp->s_abb);
      cp += (strlen (sp->s_abb) + 3);

/* put in zip code */
      if ((pp2 = findpair ("zipcode", 0x00)) == (struct pair *)NULL)
        adios (NULLCP, "'zipcode' not specified");

      sprintf (cp, "%s ", pp2->p_val);
      cp += (strlen (pp2->p_val) + 1);

/* put in 'US' for country */
      sprintf (cp, "$ US");

/* now put value in */
      pp->p_val = strdup (buf);
    }					/* end, initialize postal address */
    else {				/* don't initialize postal address */
      if (pp == (struct pair *)NULL)	/* no 'postaladdress' pair? */
        adios (NULLCP, "internal error: 'postaladdress' variable not found");

/* check to make sure postal address is legal ... */
      cp2 = pp->p_val;			/* point at postal address */

      for (cp2 = pp->p_val, i = 1; cp = strchr (cp2, (int)'$');
	   cp2 = cp + 1, ++i) {
        *cp = '\0';			/* null terminate */

        if (strlen (cp2) > 30)
	  bad_postaladdress (cp2, 0x01);

        *cp = '$';
      }

      if (strlen (cp2) > 30)
        bad_postaladdress (cp2, 0x01);

      if (i > 6)
        adios (NULLCP, "too many items (%d) in postaladdress, 6 maximum", i);
    }					/* end, don't init postal addr*/

/* done. cleanup */
    fclose (cfgfp);			/* close configuration file */
}

/*  */
/* bad_postaladdress () -- print error message for a bad postal address */
void
bad_postaladdress (line, postdef)
char *line;				/* offending line */
unsigned char postdef;			/* postal address defined? */
{
    advise (NULLCP,"line:\n\t%s\nis illegal in a postal address", line);

    if (!postdef)
      advise (NULLCP, 
       "You must explicitly define the 'postaladdress' attribute\n\
	in the configuration file for your DSA.\n\n");
	
    adios (NULLCP,
       "The format of the 'postaladdress' attribute is:\n\n\
	   postaladdress \"item1 $ item2 $ ... $ item\"\n\n\
	where each item is <= 30 characters in length and there are no\n\
	more than six items. Please refer to the Administrator's Guide.\n");
}

/*  */
/* build_top () -- build root, c=US */
void
build_top ()
{
    struct pair *pp;
    int i;
    extern void copy_edb ();
    extern struct pair *findpair();

/* setup base directory */
    if ((pp = findpair ("wildlife", 0x00)) == (struct pair *)NULL)
      adios (NULLCP, "cannot find name of wildlife for base directory");

    if (isodetcpath == (char *)NULL)
      adios (NULLCP, "cannot determine ISODE ETCPATH");

    sprintf (buf, "%s/quipu/%s", isodetcpath, pp->p_val);

    if (mkdir (buf, DIRPERM) < 0 && errno != EEXIST)
      adios (NULLCP, "cannot create wildlife directory %s",buf);
    chdir (buf);

/* iterate through, creating top-level EDB files from templates. */
    copy_edb (topdirs[0].dir, topdirs[0].edb, 0x00); /* root, special case */

    for (i = 1; topdirs[i].dir != (char *)NULL; ++i) {
       if (mkdir (topdirs[i].dir, DIRPERM) < 0 && errno != EEXIST)
         adios (NULLCP, "cannot create directory %s", topdirs[i].dir);

       copy_edb (topdirs[i].dir, topdirs[i].edb, 0x00);
    }					/* end, foreach directory */
}

/*  */
/* add_us () -- add national organization to US EDB */
void
add_us (orgcode)
struct pair *orgcode;			/* ansi organization code */
{
    extern void copy_edb ();

    bzero (buf, BUFSIZ);
    unlink (ORGENTRY);

    if (orgcode != (struct pair *)NULL && orgcode->p_val != (char *)NULL)
      sprintf (buf, "sed -f %s %s > %s\n\n",
	       SEDFILE, ANSI_ORGTMPL, ORGENTRY);
    else
      sprintf (buf, "sed -f %s %s > %s\n\n",
	       SEDFILE, ORGTMPL, ORGENTRY);

    if (system (buf) != 0)
      adios (NULLCP, "system failed: cannot create %s", ORGENTRY);

    copy_edb (USDN, ORGENTRY, 0x01);
}

/*  */
/* add_state () -- build state entry, adding organization if necessary*/
void
add_state (addorg)
unsigned char addorg;			/* add organization to state entry? */
{
    struct pair *state;			/* name of state */
    extern void copy_edb ();
    extern struct pair *findpair ();
    FILE *statefp;

/* get name of state */
    if ((state = findpair ("state", 0x00)) == (struct pair *)NULL)
      adios (NULLCP, "internal error: cannot get 'state' variable");

/* create directory for state */
    bzero (buf, BUFSIZ);
    sprintf (buf, "%s/st=%s", USDN, state->p_val);

    if (mkdir (buf, DIRPERM) < 0 && errno != EEXIST)
      adios (NULLCP, "cannot create directory %s",buf);

/* make sure EDB file doesn't already exist */
    sprintf (file, "%s/st=%s/EDB", USDN, state->p_val);

    if (access (file, F_OK) == 0) {
      sprintf (oldfile, "%s.bak", file);

      if (rename (file, oldfile) < 0)
        adios (NULLCP, "cannot rename %s to %s", file, oldfile);
    }

/* create skeleton state EDB */
    if ((statefp = fopen (file, "w")) == (FILE *)NULL)
      adios (NULLCP, "cannot create %s",file);

    fprintf (statefp, "SLAVE\n000000000000Z\n");
    fclose (statefp);

    if (addorg) {			/* need to add organization? */
       unlink (ORGENTRY);		/* remove temporary org. entry */

/* create organizational entry */
       bzero (buf, BUFSIZ);
       sprintf (buf, "sed -f %s %s > %s", SEDFILE, ORGTMPL, ORGENTRY);

       if (system (buf) != 0)
         adios (NULLCP, "system failed: cannot create %s", ORGENTRY);

       sprintf (buf, "%s/st=%s", USDN, state->p_val);
       copy_edb (buf, ORGENTRY, 0x01);
    }
}

/*  */
void
build_dsa ()
{
    unlink (DSAENTRY);
    bzero (buf, BUFSIZ);
    sprintf (buf, "sed -f %s %s  > %s", SEDFILE, DSATMPL, DSAENTRY);

    if (system (buf) != 0)
      adios (NULLCP, "system failed: cannot create %s", DSAENTRY);

    copy_edb (USDN, DSAENTRY, 0x01);
}

/*  */
/* build_orgedb () -- build skeleton EDB for organization */
void
build_orgedb ()
{
    struct pair *pp;		/* to get pair values */
    char *orgnm;		/* name of organization */
    extern struct pair *findpair ();

/* get name of organization */
    bzero (file, BUFSIZ);
    if ((pp = findpair ("organization", 0x00)) == (struct pair *)NULL)
      adios (NULLCP, "internal error: cannot get 'organization' variable");
    orgnm = pp->p_val;

/* set up location of organization */
    bzero (file, BUFSIZ);
    if ((pp = findpair ("orgcode", 0x00)) != (struct pair *)NULL ||
	chartered_by_congress)
      sprintf (file,"%s/o=%s", USDN,orgnm);
    else {				/* org. has regional standing */
      if ((pp = findpair ("state", 0x00)) == (struct pair *)NULL)
        adios (NULLCP, "internal error: cannot get 'state' variable");

      sprintf (file, "%s/st=%s/o=%s", USDN, pp->p_val, orgnm);
    }

    if (mkdir (file, DIRPERM) < 0 && errno != EEXIST)
      adios (NULLCP, "internal error: cannot create %s",file);

/* make sure EDB file doesn't already exist */
    strcat (file, "/EDB");

    if (access (file, F_OK) == 0) {
      sprintf (oldfile, "%s.bak", file);

      if (rename (file, oldfile) < 0)
        adios (NULLCP, "cannot rename %s to %s", file, oldfile);
    }

/* now create skeleton organizational entries */
    sprintf (buf, "sed -f %s %s > \"%s\"", SEDFILE, ORGEDBTMPL, file);
    if (system (buf) != 0)
      adios (NULLCP, "system failed: cannot create %s", file);
}

/* build_ouedb () -- build skeleton EDB for organizational unit */
void
build_ouedb ()
{
    struct pair *pp;		/* to get pair values */
    char *orgnm;		/* name of organization */
    char *ounm;			/* name of organizational unit */
    extern struct pair *findpair ();

/* get name of organization and organizational unit */
    bzero (file, BUFSIZ);
    if ((pp = findpair ("organization", 0x00)) == (struct pair *)NULL)
      adios (NULLCP, "internal error: cannot get 'organization' variable");
    orgnm = pp->p_val;

    if ((pp = findpair ("unit", 0x00)) == (struct pair *)NULL)
      adios (NULLCP, "internal error: cannot get 'unit' variable");
    ounm = pp->p_val;

/* set up location of organization */
    bzero (file, BUFSIZ);

    if ((pp = findpair ("orgcode", 0x00)) != (struct pair *)NULL ||
	chartered_by_congress)
      sprintf (file,"%s/o=%s/ou=%s", USDN, orgnm, ounm);
    else {				/* org. has regional standing */
      if ((pp = findpair ("state", 0x00)) == (struct pair *)NULL)
        adios (NULLCP, "internal error: cannot get 'state' variable");

      sprintf (file, "%s/st=%s/o=%s/ou=%s", USDN, pp->p_val, orgnm, ounm);
    }

    if (mkdir (file, DIRPERM) < 0 && errno != EEXIST)
      adios (NULLCP, "internal error: cannot create %s",file);

/* make sure EDB file doesn't already exist */
    strcat (file, "/EDB");

    if (access (file, F_OK) == 0) {
      sprintf (oldfile, "%s.bak", file);

      if (rename (file, oldfile) < 0)
        adios (NULLCP, "cannot rename %s to %s", file, oldfile);
    }

/* now create skeleton organizational unit entries */
    sprintf (buf, "sed -f %s %s > \"%s\"", SEDFILE, OUEDBTMPL, file);
    if (system (buf) != 0)
      adios (NULLCP, "system failed: cannot create %s", file);
}

/*  */
/* build_tailor () -- build tailor files */
void
build_tailor ()
{
    extern void make_file ();

    make_file ("quiputailor", QUIPUTMPL);
    make_file (isodefile ("dsaptailor", 0), DSAPTMPL);
}

/* build_scripts () -- build script files */
void
build_scripts ()
{
    extern void make_file ();

    make_file ("startup.sh", STARTUPTMPL);
    make_file ("nightly.sh", NIGHTLYTMPL);

}

/* build_uifl () -- build user interface configuration files */
void
build_uifl ()
{
    extern void make_file ();

    sprintf (file, "%s/fredrc", isodetcpath);
    make_file (file, FREDTMPL);

    sprintf (file, "%s/ufnrc", isodetcpath);
    make_file (file, UFNTMPL);

}

/*  */
/* create_sedfile () -- create sed file to convert templates */
void
create_sedfile ()
{
    FILE *sedfp;			/* sed file pointer */
    struct pair *pp;			/* to get pair values */
    char *orgnm;			/* organization name */
    char *town;				/* name of town */
    struct state *sp;			/* to save state */
    short stnmlen;			/* length of state name */
    int i;
    extern char *timestamp ();

    if ((sedfp = fopen (SEDFILE, "w")) == (FILE *)NULL)
      adios (NULLCP, "cannot create sed file %s", SEDFILE);

/* put in pairs */
    for (i = 0; pairs[i].p_nm != (char *)NULL; ++i)
       if (pairs[i].p_val != (char *)NULL)
         fprintf (sedfp,"s!@(%s)!%s!g\n", pairs[i].p_nm, pairs[i].p_val);
       else
         fprintf (sedfp, "s!@(%s)!!g\n", pairs[i].p_nm);

/* put in things from isodetailor () */
    fprintf (sedfp,"s!@(etcdir)!%s!g\n",isodetcpath);
    fprintf (sedfp,"s!@(bindir)!%s!g\n",isodebinpath);
    fprintf (sedfp,"s!@(sbindir)!%s!g\n",isodesbinpath);
    fprintf (sedfp,"s!@(quipuversion)!%s!g\n",quipuversion);

/* get state, in order to find apppropriate regional DSAs */
    if ((pp = findpair ("state", 0x00)) == (struct pair *)NULL)
      adios (NULLCP, "internal error: cannot get 'state' variable");

    stnmlen = strlen (pp->p_val);

    for (i = 0; usstates[i].s_nm != (char *)NULL; ++i)
       if (stnmlen == strlen (pp->p_val) &&
	   strcmp (pp->p_val, usstates[i].s_nm) == 0)
         break;

    if (usstates[i].s_nm == (char *)NULL)
      adios (NULLCP, "state %s not a U.S. state\n", pp->p_val);
    else
      sp = (usstates +i);

/* put in appropriate regional DSA */
    fprintf (sedfp,"s!@(parentdsa)!%s!g\n",
		regional_dsas[sp->s_region].dsa_name);
    fprintf (sedfp,"s!@(parentaddr)!%s!g\n",
		regional_dsas[sp->s_region].dsa_addr);

/* get organization name */
    if ((pp = findpair ("organization", 0x00)) == (struct pair *)NULL)
      adios (NULLCP, "internal error: cannot get 'organization' variable");
    orgnm = pp->p_val;

    if ((pp = findpair ("orgcode", 0x00)) != (struct pair *)NULL ||
	chartered_by_congress) {
      fprintf (sedfp, "s!@(dnprefix)!%s@o=%s!g\n", USDN, orgnm);
    }
    else {
      fprintf (sedfp, "s!@(dnprefix)!%s@st=%s@o=%s!g\n", USDN, sp->s_nm, orgnm);
    }

/* put in locality and physicalDeliveryOfficeName */
    if ((pp = findpair ("town", 0x00)) == (struct pair *)NULL)
      adios (NULLCP, "internal error: cannot get 'town' variable");
    fprintf (sedfp,"s!@(locality)!%s, %s!g\n", pp->p_val, sp->s_nm);

/* put in timestamp */
    fprintf (sedfp, "s!@(timestamp)!%s!g\n", timestamp ());

    fclose (sedfp);
}

/*  */
/* make_file () -- create a file from a template */
void
make_file (fl, tmpl)
char *fl;				/* file to be created */
char *tmpl;				/* template to use */
{
    if (access (fl, F_OK) == 0)	{	/* file already exists? */
      sprintf (oldfile, "%s.old", fl);

      if (rename (fl, oldfile) < 0)
        adios (NULLCP, "cannot rename %s to %s", fl, oldfile);
    }

    sprintf (buf, "sed -f %s %s > %s", SEDFILE, tmpl, fl);

    if (system (buf) != 0)
      adios (NULLCP, "system failed: cannot create %s", fl);
}

/*  */
/* build_mesgfl () -- build file that needs to be sent to wpp-manager */
void
build_mesgfl ()
{
    FILE *msgfp;			/* pointer to message file */
    FILE *fp;				/* pointer to various EDB entries */
    struct pair *pp;			/* to get pair values */
    char *orgnm;			/* name of organization */
    char *statenm;			/* name of state */
    unsigned char nationalorg;		/*national organization? nonzero if so*/

/* open up message file */
    if ((msgfp = fopen ("to.wpp-manager", "w")) == (FILE *)NULL)
      adios (NULLCP, "cannot create to.wpp-manager message file");

/* get name of organization */
   if ((pp = findpair ("organization", 0x00)) == (struct pair *)NULL)
     adios (NULLCP, "internal error: cannot get 'organization' value");
   orgnm = pp->p_val;

/* get name of state */
    if ((pp = findpair ("state", 0x00)) == (struct pair *)NULL)
      adios (NULLCP, "internal error: cannot get 'state' value");
    statenm = pp->p_val;

/* determine if this is a regional or national organization */
    if ((pp = findpair ("orgcode", 0x00)) != (struct pair *)NULL ||
	chartered_by_congress) {
      nationalorg = 0x7f;		/* a national organization */
    }
    else
      nationalorg = 0x00;		/* a regional organization */

/* put organizational EDB into message */
    if (nationalorg) 
      fprintf (msgfp,"--- Addition to %s/EDB ---\n", USDN);
    else
      fprintf (msgfp,"--- Addition to %s/st=%s/EDB ---\n",
		USDN, statenm);

    if ((fp = fopen (ORGENTRY, "r")) == (FILE *)NULL)
      adios (NULLCP, "cannot open %s", ORGENTRY);

    while (fgets (line, BUFSIZ-1, fp) != (char *)NULL)
      fprintf (msgfp, "%s", line);

    fclose (fp);

/* put DSA entry into message */
    if ((fp = fopen (DSAENTRY, "r")) == (FILE *)NULL)
      adios (NULLCP, "cannot open %s", DSAENTRY);

    fprintf (msgfp, "--- Addition to %s/EDB ---\n", USDN);
    while (fgets (line, BUFSIZ-1, fp) != (char *)NULL)
      fprintf (msgfp, "%s", line);
    fclose (fp);

    fclose (msgfp);

    printf ("Remember to send the to.wpp-manager file off.\n");
    printf ("(It may be deleted anytime after it is sent)\n");

    unlink (ORGENTRY);
    unlink (DSAENTRY);
}

/*  */
/* copy_edb () -- create an EDB, or append to an existing EDB */
void
copy_edb (dir, edbtmpl, append)
char *dir;				/* place where EDB should go */
char *edbtmpl;				/* location of EDB template */
unsigned char append;			/* append, nonzero if so */
{
    FILE *edbfp;			/* EDB file pointer */
    FILE *tmplfp;			/* template file pointer */

    bzero (file, BUFSIZ);		/* clear general purpose filename */
    bzero (line, BUFSIZ);		/* clear general purpose line */

    sprintf(file, "%s/EDB", dir);	/* get path of EDB file */

    if (!append) {			/* don't append -- copy */
/* rename EDB if it already exists */
      if (access (file, F_OK) == 0) {
        bzero (oldfile, BUFSIZ);
        sprintf (oldfile,"%s.bak", file);

        if (rename (file, oldfile) < 0)
          adios (NULLCP, "cannot rename %s to %s", file, oldfile);
      }
    
/* open new EDB file */
      if ((edbfp = fopen (file, "w")) == (FILE *)NULL)
        adios (NULLCP, "cannot open %s", file);
    }
    else {				/* append */
/* open new EDB file */
      if ((edbfp = fopen (file, "a")) == (FILE *)NULL)
        adios (NULLCP, "cannot open %s", file);
    }

/* open template file */
    if ((tmplfp = fopen (edbtmpl, "r")) == (FILE *)NULL)
      adios (NULLCP, "cannot open template file %s", edbtmpl);

/* copy in EDB */
    while (fgets (line, BUFSIZ-1, tmplfp) != NULL)
      fprintf(edbfp, "%s", line);

    fclose (tmplfp);
    fclose (edbfp);
}

/*  */
/* findpair () -- find 'pair' structure corresponding to name provided */
struct pair *
findpair (nm, any)
char *nm;				/* name of 'pair' to find */
char any;				/* 'pair' can have any value? */
{
    int i;				/* index and counter */
    int nmlen;				/* length of name */

    nmlen = strlen (nm);		/* get length of name */

/* iterate, looking for a name match */
    for (i = 0 ; pairs[i].p_nm != (char *)NULL; ++i)
       if (nmlen == strlen(pairs[i].p_nm) && strcmp (pairs[i].p_nm,nm) == 0)
         if (any || pairs[i].p_val != (char *)NULL)
           return (pairs+i);

    return ((struct pair *)NULL);
}

/*  */
/* set_permissions () -- set appropriate permissions on database directory */
void
set_permissions ()
{
    struct pair *pp;			/* to get pair values */
    char *owner, *group;		/* owner and group */
    char *wildlife;			/* database directory */
    struct passwd *pw;
    struct group *grp;

    owner = (char *)NULL;
    group = (char *)NULL;

/* get database directory */
    if ((pp = findpair ("wildlife", 0x00)) == (struct pair *)NULL)
      adios (NULLCP, "internal error: cannot get 'wildlife' directory");

    wildlife = pp->p_val;
/* get owner */
    if ((pp = findpair ("owner", 0x00)) == (struct pair *)NULL)
      if ((pp = findpair ("userid", 0x00)) == (struct pair *)NULL)
        if ((pw = getpwuid (geteuid())) == (struct passwd *)NULL)
          adios (NULLCP, "Warning: chown not performed -- no account");
        else {
          advise (NULLCP, "Warning: 'owner' and 'userid' not specified -- using %s", pw->pw_name);
          owner = pw->pw_name;
        }
      else {
       if (getpwnam (pp->p_val) == (struct passwd *)NULL)
         adios (NULLCP, "Account '%s' is not legal -- cannot be owner", pp->p_val);
       owner = pp->p_val;
      }
    else {
     if (getpwnam (pp->p_val) == (struct passwd *)NULL)
       adios (NULLCP, "Account '%s' is not legal -- cannot be owner", pp->p_val);
      owner = pp->p_val;
    }

/* get group */
    if ((pp = findpair ("groupid", 0x00)) == (struct pair *)NULL) {
      if ((grp = getgrgid (getegid ())) == (struct group *)NULL)
        adios (NULLCP, "Warning: chown not performed -- no group");

      group = grp->gr_name;
    }
    else {
      if (getgrnam (pp->p_val) == (struct group *)NULL)
        adios (NULLCP, "Group '%s' not legal -- cannot be group", pp->p_val);

      group = pp->p_val;
    }
    
/* do chown */
    bzero (buf, BUFSIZ);

    chdir ("..");
    sprintf (buf, "find %s -exec chown %s.%s {} \\;", wildlife, owner, group);
    if (system (buf) != 0)
      adios (NULLCP, "cannot perform chown -- system failed");
    sprintf (buf, "chmod 755 %s/startup.sh %s/nightly.sh", 
		wildlife, wildlife);
    if (system (buf) != 0)
      adios (NULLCP, "cannot perform chmod -- system failed");
}

/*  */
/* make_usstates () -- generate EDB format entries for the U.S. states */
void
make_usstates ()
{
    int i,j;
    char *masterdsa;

    for (i = 0; usstates[i].s_nm != (char *)NULL; ++i) {
       printf ("st=%s\n",usstates[i].s_nm);
       printf ("objectclass= top & locality & quipuObject & quipuNonLeafObject & nadfObject & usStateOrEquivalent\n");
       printf ("l= %s & %s\n", usstates[i].s_nm, usstates[i].s_abb);
       printf ("st=%s\n",usstates[i].s_nm);
       if (usstates[i].s_fips < 60)
         printf ("description= The State of %s\n", usstates[i].s_nm);
       else
	 printf ("description= %s\n", usstates[i].s_nm);
       printf ("acl= others # read # default\n");
       printf ("acl= others # read # entry\n");
       printf ("masterDSA= cn=Alpaca\n");
       printf ("slaveDSA= cn=Fruit Bat\n");
       printf ("slaveDSA= cn=Giant Anteater\n");
       printf ("fipsStateNumericCode= %d\n",usstates[i].s_fips);
       printf ("fipsStateAlphaCode= %s\n\n",usstates[i].s_abb);
    }					/* end, foreach state */
}

/*  */
/* timestamp () -- return timestamp to use as version on EDB file */
char *
timestamp ()
{
    long clock;
    struct UTCtime ut;
    static char timestamp [32];

    (void) time (&clock);
    tm2ut (gmtime (&clock), &ut);
    (void) strcpy (timestamp, gent2str (&ut));

    return (timestamp);
}

/* adios () -- exit with error */
#ifndef lint

void
adios (va_alist)
va_dcl
{
    va_list ap;
    extern void _advise ();

    va_start (ap);

    _advise (ap);

    va_end (ap);

    _exit (1);
}

#else /* lint */

void
adios (what, fmt)
char *what, fmt);
{
    adios (what, fmt);
}

#endif /* lint */

/* advise () -- print out an error message */
#ifndef lint

void
advise (va_alist)
va_dcl
{
    va_list ap;
    extern void _advise ();

    va_start (ap);

    _advise (ap);

    va_end (ap);
}

void
_advise (ap)
va_list ap;
{

    char buffer[BUFSIZ];

    asprintf (buffer, ap);

    (void) fflush (stdout);

    fprintf (stderr, "%s: ", prgnm);
    (void) fputs (buffer, stderr);
    (void) fputc ('\n',stderr);

    (void)fflush(stderr);
}

#else /* lint */

/* VARARGS */

void
advise (what, fmt)
char *what, *fmt;
{
    advise (what, fmt);
}

#endif /* lint */

