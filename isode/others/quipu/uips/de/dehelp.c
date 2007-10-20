/* template.c - your comments here */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/de/RCS/dehelp.c,v 9.0 1992/06/16 12:45:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/dehelp.c,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: dehelp.c,v $
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


#include <ctype.h>
#include <signal.h>
#include "demanifest.h"
#include "tailor.h"
#include "quipu/util.h"
#include "logger.h"

extern LLog *de_log;
extern int lines, cols;
extern int preferInvVideo;
extern int deLogLevel;
extern int controlCtoQuit;
extern char term[];
char *TidyString();
char *checkSetTerm();
void displayCurrentSettings();
void soundBell();

static struct {
	char *topic;
	char *fullname;
	int unique;
} helpTopics[] = {
	"commands", NULLCP, 3,
	"country", NULLCP, 3,
	"cricket", NULLCP, 2,
	"department", NULLCP, 1,
	"dept", "department", 4,
	"ex1", NULLCP, 3,
	"ex2", NULLCP, 3,
	"help", NULLCP, 1,
	"intro", NULLCP, 1,
	"list", NULLCP, 1,
        "matching", NULLCP, 1,
	"name", NULLCP, 1,
	"organisation", NULLCP, 3,
	"organization", "organisation", 3,
	"orginfo", NULLCP, 4,
	"person", "name", 2,
        "problems", NULLCP, 2,
        "settings", NULLCP, 2,
	"screen", NULLCP, 2,
	"termtypes", NULLCP, 1,
	"welcome", NULLCP, 2,
	"wildcards", NULLCP, 2,
	NULLCP, NULLCP, 0
};

tailorHelp()
{
int i;

	if (controlCtoQuit)
	for (i = 0; helpTopics[i].unique != 0; i++)
	{
		if (strcmp(helpTopics[i].topic, "intro") == 0)
			helpTopics[i].fullname = "introcc";
		if (strcmp(helpTopics[i].topic, "name") == 0)
			helpTopics[i].fullname = "namecc";
	}
}

char *
findHelp(helpstr)
char * helpstr;
{
int i, n;
char * cp;

	for (cp = helpstr; *cp != '\0'; cp++)
		if (isupper(*cp))
			*cp = tolower(*cp);
	for (i = 0; helpTopics[i].unique != 0; i++)
	{
		if ((n = strlen(helpstr)) >= helpTopics[i].unique)
			if (strncmp(helpTopics[i].topic, helpstr, n) == 0)
				if (helpTopics[i].fullname == NULLCP)
					return(helpTopics[i].topic);
				else
					return(helpTopics[i].fullname);
	}
	return(helpstr);
}

displayFile(filename, help)
char * filename;
int help;
{
FILE *fname;
char linebuf[LINESIZE];
char str[LINESIZE];

  (void) strcpy(str,"de/de");
  (void) strcat(str, filename);
  if ((fname = fopen(isodefile(str, 0), "r")) == (FILE *) NULL)
  {
    if (help == TRUE)
    {
      if (deLogLevel)
        (void) ll_log (de_log, LLOG_NOTICE, NULLCP, "NoHelp: %s", str + 5);
      (void) fprintf(stderr, "No help available for `%s'\n", filename);
      (void) fprintf(stderr, "\nHELP about HELP available by typing ?? or ?HELP\n\n");
    }
    else
      if (deLogLevel)
        (void) ll_log (de_log, LLOG_NOTICE, NULLCP, "No such file: %s", str + 5);
  }
  else 
  {
    pagerOn(NUMBER_NOT_ALLOWED);
    if (help == TRUE)
      if (deLogLevel)
        (void) ll_log (de_log, LLOG_NOTICE, NULLCP, "Help: %s", str + 5);
    while(fgets(linebuf, sizeof(linebuf), fname) != NULLCP)
      pageprint("%s",linebuf);
    (void) fclose(fname);
    if (help == TRUE)
      pageprint("\nHELP about HELP available by typing ??\n\n");
  }
}

static struct {
	char *varname;
	int musttype;
	int vartype;
	char * varval;
	char *desc;
} settableVars[] = {
	"termtype", 1, 2, term, "The user's terminal type",
	"invvideo", 1, 3, (char *) & preferInvVideo, "Prompts in inverse video if possible",
	"cols", 1, 1,  (char *) & cols, "The width of the user's screen in columns",
	"lines", 1, 1, (char *) & lines, "The depth of the user's screen in lines",
	NULLCP, 0, 0, NULLCP, NULLCP
};

displayHelp(helpstr)
char * helpstr;
{
SFD cleanupok();
void onint1();
int i, n, found;
char * cp, * cp1;
char buf[1024];
char varname[1024], varval[1024];
extern void turnInverseVideoOn(), turnInverseVideoOff();

  if (lexequ(helpstr, "settings") == 0)
  {
    (void) printf("It is possible to modify some values used by this program, for example the\n");
    (void) printf("number of lines on the user's screen.  ");
    
    (void) displayCurrentSettings();

    if (signal(SIGINT, SIG_IGN) != SIG_IGN)
      (void) signal(SIGINT, onint1);
    for (;;)
    {
      (void) writeInverse("\nDo you want to change any settings (y/n) ?");
      (void) putchar(' ');
      if (gets(buf) == NULLCP)
      {
        clearerr(stdin);
        goto resetSignal;
      }
      cp = TidyString(buf);
      if (strlen(cp) == 0) /* default of 'no' accepted */
        goto resetSignal;
      if (lexnequ(cp, "no", strlen(cp)) == 0)
        goto resetSignal;
      if (lexnequ(cp, "yes", strlen(cp)) != 0)
        continue;
enterVarType:
      (void) writeInverse("Enter name of variable you wish to modify:");
      (void) putchar(' ');
      if (gets(varname) == NULLCP) /* user obviously not interested */
      {
        clearerr(stdin);
	goto resetSignal;
      }
      cp = TidyString(varname);
      if (strlen(cp) == 0) /* default of 'no' accepted */
        goto resetSignal;
      /* convert user input to lower case */
      for (cp1 = cp; *cp1 != '\0'; cp1++)
        if (isupper(*cp1))
          *cp1 = tolower(*cp1);
      /* try and find variable name within table */
      for (i = 0, found = 0; settableVars[i].varname != NULLCP; i++)
      {
        if ((n = strlen(cp)) >= settableVars[i].musttype)
	  if (strncmp(settableVars[i].varname, cp, n) == 0)
	  {
	    (void) strcpy(varname, settableVars[i].varname);
	    found = 1;
	    break;
	  }
      }
      if (found == 0)
      {
        (void) printf("No such variable <%s>\n", varname);
	goto enterVarType;
      }
enterVarVal:
      (void) writeInverse("Enter value of variable:");
      (void) putchar(' ');
      if (gets(varval) == NULLCP) /* retain old value */
      {
        clearerr(stdin);
	continue;
      }
      cp = TidyString(varval);
      if (strlen(cp) == 0) /* retain old value */
        continue;
      if (validateSetting(settableVars[i].vartype, cp) != OK)
        goto enterVarVal; /* invalid input - have another go */

      /* treat termtype specially */
      if (lexequ(varname, "termtype") == 0)
      {
        cp = checkSetTerm(cp, term);
	(void) printf("\nNote that resetting the terminal type may reset the width and length of\n");
	(void) printf("the screen to the default sizes for that terminal type.  Readjust these\n");
	(void) printf("variables if you wish\n");
      }
      /* treat inv video specially */
      if (lexequ(varname, "invvideo") == 0)
      {
        if (strcmp(cp, "on") == 0)
	  turnInverseVideoOn();
	else
	  turnInverseVideoOff();
      }

      switch(settableVars[i].vartype)
      {
        case 1:
          *((int *)settableVars[i].varval) = atoi(cp);
	  break;
	case 2:
          (void) strcpy(settableVars[i].varval, cp);
	  break;
	case 3:
	  if (strcmp(cp, "on") == 0)
	    *((int *)settableVars[i].varval) = 1;
	  else if (strcmp(cp, "off") == 0)
	    *((int *)settableVars[i].varval) = 0;
	  break;
      }
      (void) putchar('\n');
      displayCurrentSettings();
    }
resetSignal:
    if (signal(SIGINT, SIG_IGN) != SIG_IGN)
      (void) signal(SIGINT, cleanupok);
    return;
  }
  else
    displayFile(helpstr, TRUE); /* TRUE says this is a help file */
}

validateSetting(type, value)
int type;
char * value;
{
char * cp;
  switch (type)
  {
    case 1: /* look for non numeric chars */
      for (cp = value; *cp != '\0'; cp++)
        if (!(isdigit(*cp)))
          goto invalidinput;
      return OK;
    case 2: /* strings are OK */
      return OK;
    case 3: /* booleans are "on" or "off" */
      if ((strcmp(value, "on") == 0) || (strcmp(value, "off") == 0))
        return OK;
      else
        goto invalidinput;
    default: /* what the hell's this?? */
      goto invalidinput;
  }
invalidinput:
  soundBell();
  return NOTOK;
}

void
displayCurrentSettings()
{
int i;

    (void) printf("These are the current settings:\n\n");
    (void) printf("VARIABLE     VALUE           DESCRIPTION\n  NAME\n");
    for (i = 0; settableVars[i].varname != NULLCP; i++)
    {
      (void) printf("%-8s  ", settableVars[i].varname);
      switch (settableVars[i].vartype)
      {
        case 1: /* integer */
          (void) printf("%8d", *((int *)(settableVars[i].varval)));
	  break;
	case 2: /* string */
          (void) printf("%8s", settableVars[i].varval);
	  break;
	case 3: /* boolean */
	  if (*((int *)(settableVars[i].varval)) == 1)
	    (void) printf("%8s", "on");
	  else
	    (void) printf("%8s", "off");
	  break;
      }
      (void) printf("  %s\n", settableVars[i].desc);
    }
}
