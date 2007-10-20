/* ps.c - fax to postscript */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/photo/RCS/ps.c,v 9.0 1992/06/16 12:43:35 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/photo/RCS/ps.c,v 9.0 1992/06/16 12:43:35 isode Rel $
 *
 *
 * $Log: ps.c,v $
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
#include <quipu/photo.h>

#define HEIGHT 2200

static int x, y;

extern int two_passes;

photo_start (name)
char * name;
{
	x = 0;
	y = HEIGHT;
	two_passes = 0;
	puts ("%!\n0 setlinewidth 72 200 div 72 200 div scale");
	return 0;
}


photo_end (name)
char * name;
{
	/* Decoding has finished - display the image */

	if (y < HEIGHT) puts ("showpage");
	return 0;
}

photo_black (length)
int length;
{
    if (length > 0)
        (void) printf ("%d %d moveto %d %d lineto stroke\n", x, y, x + length - 1, y);
    x += length;
}

photo_white (length)
int length;
{
    x += length;
}


photo_line_end (line)
bit_string * line;
{
    x = 0;
    --y;
    if (y < 0) {
	puts ("showpage");
	y = HEIGHT;
    }
}

