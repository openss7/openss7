/* template.c - your comments here */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/de/RCS/pager.c,v 9.0 1992/06/16 12:45:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/pager.c,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: pager.c,v $
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


#include <stdio.h>
#include <varargs.h>
#include <sys/types.h>
#include <termios.h>
#include "quipu/util.h"
#include "demanifest.h"

extern int lines, cols;

static int redisplay = FALSE;
static int numOK;
static int pagerNumber = -1;
static int wrapLines = FALSE;

int lineno;
int discardInput;

testRedisplay()
{
  if (redisplay)
    return TRUE;
  else
    return FALSE;
}

setRedisplay()
{
  redisplay = TRUE;
}

pagerOn(number)
int number;
{
  lineno = 0;
  discardInput = FALSE;
  pagerNumber = -1;
  if (number == NUMBER_ALLOWED)
    numOK = TRUE;
  else
    numOK = FALSE;
}

linewrapOn()
{
  wrapLines = TRUE;
}

linewrapOff()
{
  wrapLines = FALSE;
}

isWrapOn()
{ 
  return wrapLines;
}

resetprint(va_alist)
va_dcl
{
va_list ap;
char buf[BUFSIZ];

  va_start (ap);
  _asprintf (buf, NULLCP, ap);
  (void) fputs(buf, stdout);
  pagerOn(numOK);
  redisplay = TRUE;
  va_end(ap);
}

pageprint(va_alist)
va_dcl /* no ; */
{
va_list ap;
char buf[BUFSIZ];
int i, c;
static int charsInLine = 0;

  va_start (ap);
  redisplay = FALSE;
  if (discardInput == TRUE)
    return;
  _asprintf (buf, NULLCP, ap);
  for (i = 0; buf[i] != '\0'; i++)
  {
    if (buf[i] == '\n')
    {
      charsInLine = 0;
      (void) putchar(buf[i]);
      lineno++;
      if (lineno >= lines - 1)
      {
        putPagePrompt();
        c = getPagerInput();
        switch(c)
        {
          case '\n':
	    lineno--;
	    break;
          case ' ':
	    lineno = 1;
	    break;
	  default:
	    break;
        }
	if ((pagerNumber == -1) && ((c == VEOF) || (c == VKILL)))
	  (void) kill(getpid(), 2); /* let the signal handling sort it out */
        if (((pagerNumber == -1)&& (c == 'q')) || (pagerNumber != -1))
	{
	  discardInput = TRUE;
	  break;
	}
      }
    }
    else
      /* optionally discard chars that would cause line wrapping */
      if (isWrapOn() ||
         (charsInLine < (cols - 1))) 
      {
        (void) putchar(buf[i]);
	charsInLine++;
	if (isWrapOn())
	{
	  if (charsInLine == cols)
	  {
	    charsInLine = 0;
	    lineno++;
	  }
	}
      }
  }
  va_end(ap);
}

putPagePrompt()
{
    writeInverse("SPACE for next screen; q to quit pager");
    if (numOK == TRUE)
      writeInverse("; or the number of the entry");
    writeInverse(": ");
}

int
getPagerInput()
{
int c, i;
char numstr[LINESIZE];

  pagerNumber = -1;
  setRawMode();
  c = 'Z';
  while (! ((c == 'q') || (c == ' ') || (c == '\n') 
                       || (c == VKILL) || (c == VEOF)))
  {
    if ((numOK == TRUE) && isdigit(c))
    {
      for (i = 0; isdigit(c);i++)
      {
        (void) putchar(c);
        numstr[i] = c;
	c = getchar();
      }
      if (c == '\n')
      {
        numstr[i] = '\0';
        c = pagerNumber = atoi(numstr);
	break;
      }
      else
      {
        clearLine();
	putPagePrompt();
        c = getchar();
        continue;
      }
    }
    else
      c = getchar();
  }
  unsetRawMode();
  clearLine();
  return c;
}

int
getpnum()
{
  return pagerNumber;
}
