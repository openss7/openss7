/* t4014.c - display on tetronix 4014 terminals */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/photo/RCS/t4014.c,v 9.0 1992/06/16 12:43:35 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/photo/RCS/t4014.c,v 9.0 1992/06/16 12:43:35 isode Rel $
 *
 *
 * $Log: t4014.c,v $
 * Revision 9.0  1992/06/16  12:43:35  isode
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
#include "quipu/photo.h"
#include "manifest.h"
#include <signal.h>

extern int NUMLINES,PIC_LINESIZE;
extern unsigned position;

/* Any errors should be written to *stdout* */
/* if the process exits, with out giving an error message, quipu may hang */

#define SCALE 5
#define Y_OFFSET 2700
#define X_OFFSET 3000
#define X_SKIP 8

int y = Y_OFFSET;

SFD photo_quit ()
{
	putch (030);	/* Return to non-graphic mode */
	exit (0);
}

/* ARGSUSED */
photo_start (name)
char * name;
{
	putch (035);	/* Enter graphic mode */

	openpl ();
	erase ();
	linemod ("solid");

	(void) signal (SIGTERM,photo_quit);
	/* return 0 if sucessful -1 if not */

	return (0);
}


/* ARGSUSED */
photo_end (name)
char * name;
{
	/* Decoding has finished - display the photo */
	move (0,Y_OFFSET - 100);
	closepl();

	(void) printf ("\n");
	(void) fflush (stdout);
	(void) close (1);	/* this is needed for QUIPU */
	/* wait until signalled to Terminate */
	for (;;)
		;
}

/* ARGSUSED */
photo_black (length)
int length;
{
;
}

photo_white (length)
int length;
{
	line ((position*SCALE)+X_OFFSET,y,((length+position-1)*SCALE)+X_OFFSET,y);
}


/* ARGSUSED */
photo_line_end (line)
bit_string * line;
{
	y -= SCALE;
}

