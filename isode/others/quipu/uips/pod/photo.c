/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/photo.c,v 9.0 1992/06/16 12:44:54 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/photo.c,v 9.0 1992/06/16 12:44:54 isode Rel
 */

#include "quipu/photo.h"
#include "pod.h"

extern Widget toplevel;
extern Widget PhotoWindow;

GC XCreateGC();
void make_photo_widget();

GC gc;
char photo_name[1024];
Pixmap photo_pixmap = 0;
Display *dpy;
Screen *scr;
int winX, winY, winW, winH;
extern int NUMLINES, PIC_LINESIZE;
extern unsigned position;
unsigned long XorVal;

static int passno = 1;
static int x, y;
int px, py, maxx;
int two_passes;

 /*ARGSUSED*/ int
photo_start(name)
	char *name;
{
	x = y = 0;
	if (passno == 1)
		maxx = 0, two_passes = 1;
	return 0;
}

int
photo_end(name)
	char *name;
{
	Pixel fore, back;
	Arg args[MAXARGS];
	int count;

	if (passno == 1) {
		passno = 2;
		px = x = maxx;
		py = --y;

		make_photo_widget();

		dpy = XtDisplay(toplevel);
		scr = DefaultScreenOfDisplay(dpy);

		winW = x;
		winH = y;

		photo_pixmap = XCreatePixmap(dpy, XtWindow(PhotoWindow),
					     (Dimension) x, (Dimension) y,
					     DefaultDepthOfScreen(scr));
		gc = XCreateGC(dpy, photo_pixmap, 0, NULL);

		count = 0;
		XtSetArg(args[count], XtNbackground, &back);
		count++;
		XtSetArg(args[count], XtNforeground, &fore);
		count++;
		XtGetValues(PhotoWindow, args, count);

		XSetLineAttributes(dpy, gc, 0, LineSolid, CapButt, JoinBevel);

		XSetForeground(dpy, gc, (unsigned long) fore);
		XSetBackground(dpy, gc, (unsigned long) back);

		if (fore == 0)
			XSetFunction(dpy, gc, GXset);
		else
			XSetFunction(dpy, gc, GXclear);

		XFillRectangle(dpy, photo_pixmap, gc, 0, 0,
			       (unsigned int) winW, (unsigned int) winH);

		if (fore == 0)
			XSetFunction(dpy, gc, GXclear);
		else
			XSetFunction(dpy, gc, GXset);

		return 0;
	}
	if (name && *name)
		(void) strcpy(photo_name, name);
	passno = 1;
	x = y = maxx = 0;
	return 0;
}

 /*ARGSUSED*/ int
photo_line_end(line)
	bit_string *line;
{
	/* the end of a line has been reached */
	/* A bit string is stored in line->dbuf_top */

	if (passno == 1 && x > maxx)
		maxx = x;
	x = 0, y++;

	return 0;
}

int
photo_black(length)
	int length;
{
	if (passno == 1) {
		x += length;
		return 0;
	}
	/* draw a black line of 'length' pixels */
	return 0;
}

int
photo_white(length)
	int length;
{
	if (passno == 1) {
		x += length;
		return 0;
	}

	/* draw a white line of 'length' pixels */
	XDrawLine(dpy, photo_pixmap, gc, position, NUMLINES, length + position - 1, NUMLINES);

	return 0;
}
