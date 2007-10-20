/* template.c - your comments here */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/de/RCS/init.c,v 9.0 1992/06/16 12:45:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/init.c,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: init.c,v $
 * Revision 9.0  1992/06/16  12:45:59  isode
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


/****************************************************************************

  init.c - Do some initialising
           Read tailor files
           Eventually do argument parsing

*****************************************************************************/

/* #include <stdlib.h> */
#include "demanifest.h"
#include "types.h"
#include "util.h"
#include "quipu/util.h"
#include "tailor.h"
#include "logger.h"
#include "query.h"

static dsaAddFound = FALSE;

extern int print_parse_errors;          /* Quipu variable to disable
                                           printing of errors */

extern LLog * de_log;

extern struct query qinfo[];

extern char *oidtable,   /* Name of oidtable to use */
            *tailfile,   /* Name of dsaptailor file to use*/
            *username,
  	    *dsa_address,
            *backup_dsa_address,
  	    *local_dit;

extern int preferInvVideo;
extern int greyBook;
extern int browseMess;
extern int maxOrgs;
extern int maxDepts;
extern int maxPersons;
extern int localAlarmTime;
extern int remoteAlarmTime;
extern int bindTimeout;
extern int byeByeMessage;
extern int quitChars;
extern int deLogLevel;
extern int controlCtoQuit;
extern int wanAccess;

extern char callingDteNumber[],
	    origDefaultCo[], 
	    origDefaultOrg[],
            welcomeMessage[];
static void read_de_option();

extern struct namelist * orgatts;
extern struct namelist * ouatts;
extern struct namelist * prratts;

int initialisations(argc, argv)
int argc;
char ** argv;
{
FILE *config_file;
char linebuf[LINESIZE], user_file[LINESIZE];
char * cp;

  print_parse_errors = FALSE;   /* Stop auto printing of errors */
  quipu_syntaxes();

#ifdef USE_PP
  pp_quipu_init ("de");
#endif

  /* Initialize dsap library. */
  dsap_init((int *)NULL, (char ***)NULL);

  specialSyntaxHandlers();

#ifdef USE_PP
  pp_quipu_run ();
#endif

  backup_dsa_address = NULLCP;

  qinfo[COUNTRY].defvalue[0] = '\0';
  qinfo[ORG].defvalue[0] = '\0';
  qinfo[ORGUNIT].defvalue[0] = '\0';
  qinfo[PERSON].defvalue[0] = '\0';

/* Read in dsaptailor file */
  if ((config_file = fopen(isodefile(tailfile, 0), "r")) == (FILE *) NULL) {
    (void) fprintf(stderr,
            "Cannot open `dsaptailor' file. Attempting to continue.\n");
  } else {
    while(fgets(linebuf, sizeof(linebuf), config_file) != NULLCP)
      if ((*linebuf != '#') && (*linebuf != '\n')) (void) tai_string (linebuf);

    (void) fclose(config_file);
  }
  
  /* turn off logging that we are not interested in */
  (void) isodesetvar("compatlevel", "none", 0);
  (void) isodesetvar("addrlevel", "none", 0);
  (void) isodesetvar("tsaplevel", "none", 0);
  (void) isodesetvar("ssaplevel", "none", 0);
  (void) isodesetvar("psaplevel", "none", 0);
  (void) isodesetvar("psap2level", "none", 0);
  (void) isodesetvar("acsaplevel", "none", 0);
  (void) isodesetvar("rosaplevel", "none", 0);
  isodexport("de");

  if ((config_file = fopen(isodefile("de/detailor", 0), "r")) == (FILE *) NULL) {
    (void) fprintf(stderr,
            "Cannot open `detailor' file. Attempting to continue.\n");
  } else {
    while(fgets(linebuf, sizeof(linebuf), config_file) != NULLCP)
      if ((*linebuf != '#') && (*linebuf != '\n')) read_de_option(linebuf);
    
    (void) fclose(config_file);
  }

  if ((cp = getenv("HOME")) != NULLCP)
  {
    (void) strcpy(user_file, cp);
    (void) strcat(user_file, "/.derc");
    if ((config_file = fopen(isodefile(user_file, 0), "r")) != (FILE *) NULL) {
      while(fgets(linebuf, sizeof(linebuf), config_file) != NULLCP)
        if ((*linebuf != '#') && (*linebuf != '\n')) read_de_option(linebuf);
      (void) fclose(config_file);
    }
  }

  for (argc--, argv++; argc > 0; )
  {
    cp = *argv;
    if (*cp == '-')
      switch (*++cp)
      {
        case 'h':
	  argc--;
	  argv++;
	  if (argv != (char **)NULL)
	  {
	    (void) strcpy(callingDteNumber, *argv);
	    argc--;
	    argv++;
	  }
	  else
	    (void) strcpy(callingDteNumber, "");
	  break;
	default:
	  (void)fprintf(stderr, "Argument parsing problem.  Don't understand -%c flag\n", *cp);
          exit(-1);
      }
  }


  (void) printf("%*s%s\n\n", (80 - (int)strlen(welcomeMessage)) / 2, "", 
                welcomeMessage);

  initVideo();

  return OK;
}

/*
 * - read_de_option() -
 * Read in an option from detailor or derc file.
 *
 */
static void read_de_option(line)
     char *line;
{
char *part1, *part2;
extern char *TidyString(), *SkipSpace();
int n;

  part1 = SkipSpace(line);
  
  if ((part2 = index(part1, ':')) == NULLCP) return;

  *part2++ = '\0';
  part2 = TidyString(part2);

  if (lexequ(part1, "welcomeMessage") == 0) {
    (void) strcpy(welcomeMessage, part2);
  }
  else if (lexequ(part1, "default_country") == 0) {
    (void) strcpy(qinfo[COUNTRY].defvalue, part2);
  }
  else if (lexequ(part1, "default_org") == 0) {
    (void) strcpy(qinfo[ORG].defvalue, part2);
  }
  else if (lexequ(part1, "default_dept") == 0) {
    (void) strcpy(qinfo[ORGUNIT].defvalue, part2);
  }
  else if (lexequ(part1, "dsa_address") == 0) {
    if (dsaAddFound == FALSE)
    {
      dsa_address = copy_string(part2);
      dsaAddFound = TRUE;
    }
    else if (backup_dsa_address == NULLCP)
      backup_dsa_address = copy_string(part2);
  }
  else if (lexequ(part1, "username") == 0) {
    username = copy_string(part2);
  }
  else if (lexequ(part1, "delogfile") == 0) {
    de_log->ll_file = copy_string(part2);
  }
  else if (lexequ(part1, "byeByeMessage") == 0) {
    if (lexequ(part2, "off") == 0)
      byeByeMessage = FALSE;
    else
      byeByeMessage = TRUE;
  }
  else if (lexequ(part1, "inverseVideo") == 0) {
    if (lexequ(part2, "off") == 0)
      preferInvVideo = FALSE;
    else
      preferInvVideo = TRUE;
  }
  else if (lexequ(part1, "greyBook") == 0) {
    if (lexequ(part2, "off") == 0)
      greyBook = FALSE;
    else
      greyBook = TRUE;
  }
  else if (lexequ(part1, "browseMess") == 0) {
    if (lexequ(part2, "off") == 0)
      browseMess = FALSE;
    else
      browseMess = TRUE;
  }
  else if (lexequ(part1, "allowControlCtoQuit") == 0) {
    if (lexequ(part2, "off") == 0)
      controlCtoQuit = FALSE;
    else
      controlCtoQuit = TRUE;
  }
  else if (lexequ(part1, "wanAccess") == 0) {
    if (lexequ(part2, "off") == 0)
      wanAccess = FALSE;
    else
      wanAccess = TRUE;
  }
  else if (lexequ(part1, "country") == 0) {
    addToCoList(part2);
  }
  else if (lexequ(part1, "mapattname") == 0) {
    addToAttList(part2);
  }
  else if (lexequ(part1, "commonatt") == 0) {
    addToList(&prratts, part2);
    addToList(&ouatts, part2);
    addToList(&orgatts, part2);
  }
  else if (lexequ(part1, "orgatt") == 0) {
    addToList(&orgatts, part2);
  }
  else if (lexequ(part1, "ouatt") == 0) {
    addToList(&ouatts, part2);
  }
  else if (lexequ(part1, "prratt") == 0) {
    addToList(&prratts, part2);
  }
  else if (lexequ(part1, "mapphone") == 0) {
    addToPhoneList(part2);
  }
/*
  else if (lexequ(part1, "maxOrgs") == 0) {
    n = atoi(part2);
    if (n > 0)
      maxOrgs = n;
  }
  else if (lexequ(part1, "maxDepts") == 0) {
    n = atoi(part2);
    if (n > 0)
      maxDepts = n;
  }
*/
  else if (lexequ(part1, "maxPersons") == 0) {
    n = atoi(part2);
    if (n > 0)
      maxPersons = n;
  }
  else if (lexequ(part1, "localAlarmTime") == 0) {
    n = atoi(part2);
    if (n > 0)
      localAlarmTime = n;
  }
  else if (lexequ(part1, "remoteAlarmTime") == 0) {
    n = atoi(part2);
    if (n > 0)
      remoteAlarmTime = n;
  }
  else if (lexequ(part1, "bindTimeout") == 0) {
    n = atoi(part2);
    if (n > 0)
      bindTimeout = n;
  }
  else if (lexequ(part1, "quitChars") == 0) {
    n = atoi(part2);
    if (n > 0)
      quitChars = n;
  }
  else if (lexequ(part1, "logLevel") == 0) {
    deLogLevel = atoi(part2);
  }
} /* read_de_option */



/* might want this at some point
Usage (rtn)
char *rtn;
{
  (void) fprintf(stderr, "Some helpful message\n");
}
*/

welcome()
{
FILE * welcome_file;
char linebuf[LINESIZE];

  if ((welcome_file = fopen(isodefile("de/dewelcome", 0), "r")) == (FILE *) NULL)
    (void) fprintf(stderr,
            "Cannot open `dewelcome' file. Attempting to continue.\n");
  else 
  {
    while(fgets(linebuf, sizeof(linebuf), welcome_file) != NULLCP)
      (void) fputs(linebuf, stdout);
    (void) fclose(welcome_file);
  }
}
