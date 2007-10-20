/* winx.c - xwindow version of display code */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/photo/RCS/faxx.c,v 9.0 1992/06/16 12:43:35 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/photo/RCS/faxx.c,v 9.0 1992/06/16 12:43:35 isode Rel $
 *
 *
 * $Log: faxx.c,v $
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
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>


/* It appears that window manager writers have problems */

#ifndef	NEVER_USE_WM
#define	WM_DISAPPEARED_BUG	/* wm screw up & loses the window ! */
#define	WM_WINGE_BUG		/* When wm screws up, it also winges */
#endif	NEVER_USE_WM

#ifndef	NEVER_USE_TWM
#define	TWM_RESIZE_BUG		/* twm looses the resize if done too early. */
#endif	NEVER_USE_TWM

static int *silentExit();

extern GC XCreateGC();
Pixmap pixmap;
Display *dpy;
Screen *scr;
Window win;
int winX, winY, winW, winH;
XSetWindowAttributes xswa;
GC gc;
extern int NUMLINES,PIC_LINESIZE;
extern unsigned position;
unsigned long XorVal;
int buttons_down	= 0;
int ignore_action	= 0;

static int passno = 1;
static int x, y, maxx;

static int decimation = 2;

extern int two_passes;

/* ARGSUSED */
photo_start (name)
char * name;
{
	x = y = 0;
	if (passno == 1) {
	    maxx = 0;
	    two_passes = 1;
        }
	return 0;
}


photo_end (name)
char * name;
{

	/* Decoding has finished - display the photo */
	char buff[128];

	if (passno == 1) {
	    passno = 2;
	    x = maxx;
	    --y;

	    /* Initialise a window to recieve a photo of 'name' */

	    if (!(dpy= XOpenDisplay((char *) 0))) {
		(void) printf ("Cannot open X display");
		return (-1);
	    }
	    XSetIOErrorHandler(silentExit);
	    scr = DefaultScreenOfDisplay(dpy);
	    winW = x / decimation;
	    winH = y / decimation;
	    winX = WidthOfScreen(scr) - winW;
	    winY = HeightOfScreen(scr) - winH;

	    xswa.event_mask = ExposureMask | ButtonReleaseMask | ButtonPressMask;

	    xswa.background_pixel = BlackPixelOfScreen(scr);
	    xswa.backing_store	= WhenMapped;

	    win = XCreateWindow(dpy, RootWindowOfScreen(scr),
				winX, winY, winW, winH, 0,
				DefaultDepthOfScreen(scr), InputOutput,
				DefaultVisualOfScreen(scr),
				CWEventMask | CWBackPixel | CWBackingStore, &xswa);
	    pixmap = XCreatePixmap(dpy, win,
				   winW,winH, DefaultDepthOfScreen(scr));
	    if (!pixmap) {
		(void) fprintf (stderr,"decode_fax: Pixmap failed");
		return (-1);
	    }

	    /* Set up a graphics context: */

	    gc = XCreateGC(dpy, pixmap, 0, NULL);

	    /* Clear pixmap */

	    XSetForeground(dpy, gc, BlackPixelOfScreen(scr));
	    XFillRectangle(dpy, pixmap, gc, 0, 0, winW,winH);

	    XSetForeground(dpy, gc, WhitePixelOfScreen(scr));
	    XSetBackground(dpy, gc, BlackPixelOfScreen(scr));

	    XorVal = WhitePixelOfScreen(scr) ^ BlackPixelOfScreen(scr);
	    
	    return (0);
        }

	if (strcmp (name, "unknown") == 0)
	    name = NULL;

	if (name == NULL) {
	    (void) printf ("(See X window, pid %d)", getpid());
	    (void) fflush (stdout);
	}
	(void) close (1); 

	(void) sprintf(buff, name ? "%s" : "%s (%d)",
		       name ? name : "Photo", getpid());
	XChangeProperty(dpy, win, XA_WM_NAME, XA_STRING, 8,
			 PropModeReplace, buff, strlen(buff));
	XMapWindow(dpy, win);
	XSetForeground(dpy, gc, XorVal);
	for (;;)
	{       XEvent xev;

		XNextEvent(dpy, &xev);
		switch (xev.type)
		{
		case ButtonPress:
			if (buttons_down++)
			{	if (!ignore_action++)
				{	XSetFunction(dpy, gc, GXcopy);
					XCopyArea(dpy, pixmap, win,
						gc, 0, 0,
						PIC_LINESIZE, NUMLINES,
						0, 0);
				}
			}
			else
			{	XSetFunction(dpy, gc, GXxor);
				XFillRectangle(dpy, win, gc,
					0, 0, PIC_LINESIZE, NUMLINES);
			}
			continue;
		case ButtonRelease:
			if (!buttons_down)	buttons_down++;
			if (--buttons_down)	continue;
			if (!ignore_action)	break;
			ignore_action = 0;
			continue;
		case Expose:
			XSetFunction(dpy, gc, GXcopy);
			XCopyArea(dpy, pixmap, win, gc,
				xev.xexpose.x, xev.xexpose.y,
				xev.xexpose.width, xev.xexpose.height,
				xev.xexpose.x, xev.xexpose.y); 
		default:
			continue;
		}
		break;
	}
	return 0;
}

photo_black (length)
int length;
{
    x += length;
}

photo_white (length)
int length;
{
    int x1;
    int x2;
    int y1;

    if (passno == 2 && length > 0 && y % decimation == 0) {
	y1 = y / decimation;
	x1 = x / decimation;
	x2 = (x + length - 1) / decimation;
	XDrawLine (dpy, pixmap, gc, x1, y1, x2, y1);
    }
    x += length;
}


/* ARGSUSED */
photo_line_end (line)
bit_string * line;
{
    if (passno == 1 && x > maxx)
	maxx = x;
    x = 0;
    ++y;
}

/* ARGSUSED */
static int *silentExit (dis)
	Display *dis;
{
	exit (0);
}
