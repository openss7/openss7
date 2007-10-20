/* template.c - your comments here */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/de/RCS/util.c,v 9.0 1992/06/16 12:45:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/util.c,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: util.c,v $
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


#include <signal.h>
#include "config.h"
#include "demanifest.h"
#include "destrings.h"
#include "types.h"
#include "util.h"
#include "namelist.h"
#include "cnamelist.h"
#include "quipu/util.h"
#include "quipu/ds_search.h"
#include "logger.h"
#include "query.h"

extern struct query qinfo[];
extern LLog *de_log;
extern int limitProblem;
extern int notAllReached;
extern int localAlarmTime;
extern int remoteAlarmTime;
extern char origDefaultCo[], origDefaultOrg[];
extern int deLogLevel;
extern int alarmCount;
extern int bindTimeout;
void onint1();

int
listlen(lp)
struct namelist * lp;
{
int len;

  for (len = 0; lp != NULLLIST; lp = lp->next, len++) {};
  return len;
}

unsigned int
alarmLen()
{
  if ((lexequ(qinfo[ORG].entered, origDefaultOrg) == 0) && 
      (lexequ(qinfo[COUNTRY].entered, origDefaultCo) == 0))
    return localAlarmTime;
  else
    return remoteAlarmTime;
}

void initAlarm()
{
void onalarm();

  alarmCount = 0;
  (void) signal(SIGALRM, (VFP) onalarm);
  (void) alarm(alarmLen());
}

void
alarmCleanUp()
{
	(void) signal(SIGALRM, SIG_IGN);
	(void) alarm(0);
	if (alarmCount > 1)
	{
		alarmCount = 0;
		(void) printf("\n\n");
	}
}

void
handleTimeout()
{
  de_unbind();
  (void) signal(SIGALRM, SIG_IGN);
}

void startUnbindTimer()
{
void handleTimeout();

  (void) signal(SIGALRM, (VFP) handleTimeout);
  (void) alarm((unsigned)bindTimeout);
}

void stopUnbindTimer()
{
  (void) signal(SIGALRM, SIG_IGN);
  (void) alarm(0);
}

char *
copy_string(string)
char *string;
{
  char *new_string;

  if (string == NULLCP) return NULLCP;

  new_string = (char *) smalloc((strlen(string) + 1));
  (void) strcpy(new_string, string);

  return new_string;
}


static PS ps = NULLPS;

char   *dn2pstr (dn)
DN	dn;
{
    char       *cp;

    if (ps == NULL
	    && ((ps = ps_alloc (str_open)) == NULLPS)
		    || str_setup (ps, NULLCP, BUFSIZ, 0) == NOTOK) {
	if (ps)
	    ps_free (ps), ps = NULLPS;

	return NULLCP;
    }

    dn_print (ps, dn, RDNOUT);
    ps_print (ps, " "); 
    *--ps -> ps_ptr = NULL, ps -> ps_cnt++;

    cp = ps -> ps_base;

    ps -> ps_base = NULL, ps -> ps_cnt = 0;
    ps -> ps_ptr = NULL, ps -> ps_bufsiz = 0;

    return cp;
}

/* determine wildcard type of already validated string */

int starstring(istr, ostr1, ostr2)
char * istr, ** ostr1, ** ostr2;
{
char * lastcp, *cp;

  lastcp = istr + strlen(istr) - 1;
  if ((*istr != '*') && (*lastcp == '*')) 
  {
    *lastcp = '\0';
    *ostr1 = istr;
    return LEADSUBSTR;
  } 
  else if ((*istr == '*') && (*lastcp != '*')) 
  {
    *ostr1 = istr + 1;
    return TRAILSUBSTR;
  } 
  else if ((*istr == '*') && (*lastcp == '*')) 
  {
    *lastcp = '\0';
    *ostr1 = istr + 1;
    return ANYSUBSTR;
  } 
  else /* must be of the form foo*bar */
  {
    *ostr1 = istr;
    cp = index(istr, '*');
    *cp = '\0';
    *ostr2 = cp + 1;
    return LEADANDTRAIL;
  }
}


/* print last component of a dn string - optionally indented by type */
void printLastComponent(indent, dnstr, objectType, printNumber)
int indent;
char * dnstr;
int objectType;
int printNumber;
{
char * cp1, * cp2, * savestring;

  if (strcmp(dnstr, "root") == 0)
    return;
  savestring = copy_string(dnstr);
  cp1 = lastComponent(savestring, objectType);
  if (objectType == COUNTRY)
    cp2 = mapCoName(cp1);
  else
    cp2 = cp1;
  if (indent == INDENTON)
  {
    switch (objectType)
    {
      case COUNTRY:
        break;
      case ORG:
        pageprint("  ");
	break;
      case ORGUNIT:
        pageprint("    ");
	break;
      case PERSON:
        pageprint("      ");
	break;
      default:
        (void) fprintf(stderr, "WOT's THIS?  ");
	break;
    }
  }
  if (printNumber != 0)
    pageprint("%3d ", printNumber);
  pageprint("%s\n", cp2);
  free(cp1);
  free(savestring);
}

char *
lastComponent(dnstr, objectType)
char * dnstr;
int objectType;
{
char * cp, * cp2, *cp3, * savestring;
int gotmatch;

  savestring = copy_string(dnstr);
  cp = cp3 = rindex(savestring, '@');
  if (cp == NULLCP)
    cp = dnstr;
  else
    cp++;
  for (;;)
  {
    gotmatch = FALSE;
    switch (objectType)
    {
      case PERSON:
        if (strncmp(cp, SHORT_CN, strlen(SHORT_CN)) == 0)
	{
          cp += strlen(SHORT_CN) + 1;
	  gotmatch = TRUE;
	}
        break;
      case ORGUNIT:
        if (strncmp(cp, SHORT_OU, strlen(SHORT_OU)) == 0)
	{
	  cp += strlen(SHORT_OU) + 1;
	  gotmatch = TRUE;
	}
	break;
      case ORG:
        if (strncmp(cp, SHORT_ORG, strlen(SHORT_ORG)) == 0)
	{
	  cp += strlen(SHORT_ORG) + 1;
	  gotmatch = TRUE;
	}
	break;
      case COUNTRY:
        if (strncmp(cp, SHORT_CO, strlen(SHORT_CO)) == 0)
	{
	  cp += strlen(SHORT_CO) + 1;
	  gotmatch = TRUE;
	}
	else if (strncmp(cp, SHORT_LOC, strlen(SHORT_LOC)) == 0)
	{
	  cp += strlen(SHORT_LOC) + 1;
	  gotmatch = TRUE;
	}
	break;
    }
    cp2 = index(cp, '%');
    if (gotmatch == TRUE)
    {
      if (cp2 != NULLCP)
        *cp2 = '\0';
      break;
    }
    else 
    {
      if (cp2 != NULLCP)
        cp = cp2 + 1;
      else
        break;
    }
  }
  if (gotmatch == FALSE)
  {
    cp = index(cp3, '%');
    if (cp != NULLCP)
      *cp = '\0';
    cp = index(cp3, '=') + 1;
  }
  cp = copy_string(cp);
  free (savestring);
  return cp;
}

char *
removeLastRDN(dnstr)
char * dnstr;
{
char * cp;

	cp = copy_string(dnstr);
	* rindex(cp, '@') = '\0';
	return cp;
}

char *
lastRDN(dnstr)
char * dnstr;
{
char * cp;

	cp = rindex(dnstr, '@');
	if (cp == NULLCP)
		return NULLCP;
	else
		return (++cp);
}

void
clearProblemFlags()
{
	limitProblem = notAllReached = FALSE;
}

void
setProblemFlags(sresult)
struct ds_search_result sresult;
{
	if ((sresult.CSR_limitproblem == LSR_SIZELIMITEXCEEDED) ||
	    (sresult.CSR_limitproblem == LSR_ADMINSIZEEXCEEDED))
		limitProblem = TRUE;
	if ((sresult.CSR_cr != NULLCONTINUATIONREF) ||
	    (sresult.CSR_limitproblem == LSR_TIMELIMITEXCEEDED))
		notAllReached = TRUE;
}


showAnyProblems(str)
char * str;
{
    if (limitProblem == TRUE)
    {
      pageprint("\nA limit has been imposed by the managers of the data which prevents the \n");
      pageprint("listing of all the entries in the Directory beyond this point.  Try and\n");
      if (strcmp(str, "*") == 0)
      {
	pageprint("guess the name of the entry you want.  The directory may find the entry\n");
	pageprint("even if you don't get the name exactly right.\n");
      }
      else
        pageprint("specify the query more precisely.\n");
      pageprint("Please refer to the ?matching help screen for more details.\n");
    }
    else
    {
      if (notAllReached == TRUE)
      {
        pageprint("\nIt was not possible to search all the parts of the Directory necessary\n");
	pageprint("to complete this search - part of the database is currently inaccessible.\n");
  	pageprint("There may thus be more results than those displayed.\n\n");
      }
    }
}


void
logSearchSuccess(outcome, objecttype, string, searchNumber, noMatches)
char * outcome;
char * objecttype;
char * string;
int searchNumber;
int noMatches;
{
char filterNumberString[20];

	if (deLogLevel > 1)
	{
		if (searchNumber == 0)
			(void) strcpy(filterNumberString, "explicit");
		else
			(void) sprintf(filterNumberString, "%d", searchNumber);
		
		(void) ll_log (de_log, LLOG_NOTICE, NULLCP,
			"searchOutcome:%s:%s:%s:%s:%d", objecttype,
			outcome, string, filterNumberString, noMatches);
	}
}

void logListSuccess(outcome, objecttype, noMatches)
char * outcome;
char * objecttype;
int noMatches;
{
	if (deLogLevel > 1)
	{
		(void) ll_log (de_log, LLOG_NOTICE, NULLCP,
			"listOutcome:%s:%s:%d", objecttype, outcome, noMatches);
	}
}
