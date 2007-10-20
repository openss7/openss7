#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/photo.c,v 9.0 1992/06/16 12:44:54 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/photo.c,v 9.0 1992/06/16 12:44:54 isode Rel $
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
extern int NUMLINES,PIC_LINESIZE;
extern unsigned position;
unsigned long XorVal;

static int passno = 1;
static int x, y;
int px, py, maxx;
int two_passes;

/*ARGSUSED*/
int photo_start(name)
     char *name;
{
  x = y = 0;
  if (passno == 1)
    maxx = 0, two_passes = 1;
  return 0;
}

int photo_end(name)
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
    XtSetArg(args[count], XtNbackground, &back); count++;
    XtSetArg(args[count], XtNforeground, &fore); count++;
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
  if (name && *name) (void) strcpy(photo_name, name);
  passno = 1;
  x = y = maxx = 0;
  return 0;
}

/*ARGSUSED*/
int photo_line_end(line)
     bit_string *line;
{
  /* the end of a line has been reached */
  /* A bit string is stored in line->dbuf_top */
  
  if (passno == 1 && x > maxx)
    maxx = x;
  x = 0, y++;
  
  return 0;
}

int photo_black(length)
     int length;
{
  if (passno == 1) {
    x += length;
    return 0;
  }
  /* draw a black line of 'length' pixels */
  return 0;
}

int photo_white(length)
     int length;
{
  if (passno == 1) {
    x += length;
    return 0;
  }
  
  /* draw a white line of 'length' pixels */
  XDrawLine (dpy, photo_pixmap, gc, position, 
	     NUMLINES, length+position-1, NUMLINES);
  
  return 0;
}

