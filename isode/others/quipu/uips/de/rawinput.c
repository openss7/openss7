/* template.c - your comments here */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/de/RCS/rawinput.c,v 9.0 1992/06/16 12:45:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/rawinput.c,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: rawinput.c,v $
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
#include <sys/termio.h>

static short savemode;
static unsigned char savemin;
static struct termio t;

/* ought to be a bit friendlier than this - but at least this attempts
   to be safe */

setRawMode()
{

  if (ioctl(0, TCGETA, &t) == -1)
  {
    (void) fprintf(stderr, "Couldn't go into raw mode (1), aaaaaagggggghhhhh!!!!\n");
    cleanup(-1);
  }
  savemode = t.c_lflag;
  savemin = t.c_cc[VMIN];
  t.c_lflag &= ~(ICANON|ECHO|ISIG);
  t.c_cc[VMIN] = 1;
  if (ioctl(0, TCSETA, &t) == -1)
  {
    (void) fprintf(stderr, "Couldn't go into raw mode (2), aaaaaagggggghhhhh!!!!\n");
    cleanup(-1);
  }
}

unsetRawMode()
{

  t.c_lflag = savemode;
  t.c_cc[VMIN] = savemin;
  if (ioctl(0, TCSETA, &t) == -1)
  {
    (void) fprintf(stderr, "Couldn't get out of raw mode, aaaaaagggggghhhhh!!!!\n");
    cleanup(-1);
  }
}
