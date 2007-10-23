/*
 *			D R A W P S . C 
 *
 * $Revision: 1.4 $
 *
 * $Log:	drawPS.c,v $
 * Revision 1.4  87/05/07  15:06:24  dorab
 * relinked back hh to h and vv to v undoing a previous change.
 * this was causing subtle quantization problems. the main change
 * is in the definition of hconvPS and vconvPS.
 * 
 * changed the handling of the UC seal. the PS file should now be
 * sent via the -i option.
 * 
 * Revision 1.3  86/04/29  23:20:55  dorab
 * first general release
 * 
 * Revision 1.3  86/04/29  22:59:21  dorab
 * first general release
 * 
 * Revision 1.2  86/04/29  13:23:40  dorab
 * Added distinctive RCS header
 * 
 */
#ifndef lint
static char RCSid[] = "@(#)$Header: drawPS.c,v 1.4 87/05/07 15:06:24 dorab Exp $ (UCLA)";
#endif

/*
 the driver for handling the \special commands put out by
 the tpic program as modified by Tim Morgan <morgan@uci.edu>
 the co-ordinate system is with the origin at the top left
 and the x-axis is to the right, and the y-axis is to the bottom.
 when these routines are called, the origin is set at the last dvi position,
 which has to be gotten from the dvi-driver (in this case, dvi2ps) and will
 have to be converted to device co-ordinates (in this case, by [hv]convPS).
 all dimensions are given in milli-inches and must be converted to what
 dvi2ps has set up (i.e. there are convRESOLUTION units per inch).

 it handles the following \special commands
    pn n			set pen size to n
    pa x y			add path segment to (x,y)
    fp				flush path
    da l			flush dashed - each dash is l (inches)
    dt l			flush dotted - one dot per l (inches)
    sp				flush spline
    ar x y xr yr sa ea		arc centered at (x,y) with x-radius xr
    				and y-radius yr, start angle sa (in rads),
    				end angle ea (in rads)
    sh				shade last path (box, circle, ellipse)
    wh				whiten last path (box, circle, ellipse)
    bk				blacken last path (box,circle, ellipse)
    tx				texture command - ignored

  this code is in the public domain

  written by dorab patel <dorab@cs.ucla.edu>
  december 1985
  released feb 1986

  warning: as of dec 1985, the USEGLOBALMAG stuff hasnt been tested

  */

#include <stdio.h>		/* only for outfp!! */
#include "dvi2ps.h"             /* for EMIT convRESOLUTION TRUE FALSE */

#define tpicRESOLUTION 1000	/* number of tpic units per inch */

/* convert from tpic units to dvi2ps units */
#ifdef USEGLOBALMAG
#define convPS(x) PixRound((int)((x)*convRESOLUTION*ActualFactor(mag)),tpicRESOLUTION)
#else ~USEGLOBALMAG
#define convPS(x) PixRound((x)*convRESOLUTION,tpicRESOLUTION)
#endif ~USEGLOBALMAG

/* the following have to be kludgey to avoid overflow problems */

/* convert from tpic locn to abs horiz PS locn */
#define hconvPS(x) \
	(PixRound(PixRound(10*hh,hconv)+(10*convPS(x)),10))
/* convert from tpic locn to abs vert PS locn */
#define vconvPS(x) \
	(PixRound(PixRound(10*vv,vconv)+(10*convPS(x)),10))
#define convDeg(x) (int)(360*(x)/(2*3.14159265358)+0.5) /* convert to degs */

/* if PostScript had splines, i wouldnt need to store the path */
#define MAXPATHS 300		/* maximum number of path segments */
#define NONE 0			/* for shading */
#define GREY 1			/* for shading */
#define WHITE 2			/* for shading */
#define BLACK 3			/* for shading */

/* the following defines are used to make the PostScript shorter - make sure
 that the corresponding defines are in the PostScript prologue */
#define MOVETO "p"
#define LINETO "l"
#define RCURVETO "rc"
#define RLINETO "rl"
#define STROKE "st"
#define FILL "f"
#define NEWPATH "np"
#define CLOSEPATH "closepath"
/*
 * STROKE and FILL must restore the current point to that
 * saved by NEWPATH
 */

extern void Warning();
extern void Fatal();
extern int PixRound();		/* (int)(x/y)PixRound((int)x,(int)y) */
extern int hh,vv;		/* the current x,y position in dvi units */
extern int hconv, vconv;	/* conversion from dvi to dvi2ps units */
extern FILE *outfp;		/* the output file pointer */

#ifdef DEBUG
extern int Debug;		/* for debugging */
#endif DEBUG

#ifdef USEGLOBALMAG
extern int mag;
extern float ActualFactor();
#endif USEGLOBALMAG

static int xx[MAXPATHS], yy[MAXPATHS]; /* the current path in milli-inches */
static int pathLen = 0;		/* the current path length */
static int shading = NONE;	/* what to shade the last figure */
static int penSize = 2;		/* pen size in PS units */

static void doShading();	/* just a forward declaration */

void
setPenSize(cp)
     char *cp;
{
  int ps;

  if (sscanf(cp, " %d ", &ps) != 1) 
    {
      Warning("Illegal .ps command format: %s", cp);
      return;
    }

  penSize = convPS(ps);
  EMIT(outfp,"%d setlinewidth\n", penSize);
}				/* end setPenSize */

void
addPath(cp)
     char *cp;
{
  int x,y;

  if (++pathLen >= MAXPATHS) Fatal("Too many points");
  if (sscanf(cp, " %d %d ", &x, &y) != 2)
    Fatal("Malformed path expression");
  xx[pathLen] = x;
  yy[pathLen] = y;
}				/* end of addPath */

void
arc(cp)
     char *cp;
{
  int xc, yc, xrad, yrad;
  float startAngle, endAngle;

  if (sscanf(cp, " %d %d %d %d %f %f ", &xc, &yc, &xrad, &yrad,
	     &startAngle, &endAngle) != 6)
    {
      Warning("Illegal arc specification: %s", cp);
      return;
    }

/* we need the newpath since STROKE doesnt do a newpath */

  if (xrad == yrad)		/* for arcs and circles */
    EMIT(outfp, "%s %d %d %d %d %d arc ",
	 NEWPATH,
	 hconvPS(xc), vconvPS(yc), convPS(xrad),
	 convDeg(startAngle), convDeg(endAngle));
  else
      EMIT(outfp, "%s %d %d %d %d %d %d ellipse ",
	   NEWPATH,
	   hconvPS(xc), vconvPS(yc), convPS(xrad), convPS(yrad),
	   convDeg(startAngle), convDeg(endAngle));

  doShading();
}				/* end of arc */

void
flushPath()
{
  register int i;

  if (pathLen < 2) 
    {
      Warning("Path less than 2 points - ignored\n");
      return;
    }
  
#ifdef DEBUG
    if (Debug)
	fprintf(stderr,
		    "flushpath(1): hh=%d, vv=%d, x=%d, y=%d, xPS=%d, yPS=%d\n",
		    hh, vv, xx[1], yy[1], hconvPS(xx[1]), vconvPS(yy[1]));
#endif DEBUG
  EMIT(outfp, "%s ", NEWPATH);  /* to save the current point */
  EMIT(outfp, "%d %d %s ", hconvPS(xx[1]), vconvPS(yy[1]), MOVETO);
  for (i=2; i < pathLen; i++) {
#ifdef DEBUG
    if (Debug)
	fprintf(stderr,
		    "flushpath(%d): hh=%d, vv=%d, x=%d, y=%d, xPS=%d, yPS=%d\n",
		    i, hh, vv, xx[i], yy[i], hconvPS(xx[i]), vconvPS(yy[i]));
#endif DEBUG
    EMIT(outfp, "%d %d %s\n", hconvPS(xx[i]), vconvPS(yy[i]), LINETO);
    }
  if (xx[1] == xx[pathLen] && yy[1] == yy[pathLen])
    EMIT(outfp, "%s ", CLOSEPATH);
  else
    EMIT(outfp, "%d %d %s ",
	 hconvPS(xx[pathLen]),
	 vconvPS(yy[pathLen]),
	 LINETO);
  doShading();
  pathLen = 0;
}				/* end of flushPath */

void
flushDashed(cp, dotted)
     char *cp;
     int dotted;
{
  float inchesPerDash;

  if (sscanf(cp, " %f ", &inchesPerDash) != 1) 
    {
      Warning ("Illegal format for dotted/dashed line: %s", cp);
      return;
    }
  
  if (inchesPerDash <= 0.0)
    {
      Warning ("Length of dash/dot cannot be negative");
      return;
    }

  inchesPerDash = 1000 * inchesPerDash;	/* to get milli-inches */
  
  if (dotted)
    EMIT(outfp, "[%d %d] 0 setdash\n",
	 penSize,
	 convPS(inchesPerDash) - penSize);
  else				/* if dashed */
    EMIT(outfp, "[%d] 0 setdash\n", convPS(inchesPerDash));

  flushPath();

  EMIT(outfp, "[] 0 setdash\n");
}				/* end of flushDashed */

void
flushSpline()
{				/* as exact as psdit!!! */
  register int i, dxi, dyi, dxi1, dyi1;

  if (pathLen < 2)
    {
      Warning("Spline less than two points - ignored\n");
      return;
    }
  
  EMIT(outfp, "%s ", NEWPATH);	/* to save the current point */
  EMIT(outfp, "%d %d %s %d %d rl\n",
       hconvPS(xx[1]), vconvPS(yy[1]),
       MOVETO,
       convPS((xx[2]-xx[1])/2), convPS((yy[2]-yy[1])/2),
       RLINETO);

  for (i=2; i < pathLen; i++)
    {
      dxi = convPS(xx[i] - xx[i-1]);
      dyi = convPS(yy[i] - yy[i-1]);
      dxi1 = convPS(xx[i+1] - xx[i]);
      dyi1 = convPS(yy[i+1] - yy[i]);

      EMIT(outfp, "%d %d %d %d %d %d %s\n",
	   dxi/3, dyi/3,
	   (3*dxi+dxi1)/6, (3*dyi+dyi1)/6,
	   (dxi+dxi1)/2, (dyi+dyi1)/2,
	   RCURVETO);
    }

  EMIT(outfp, "%d %d %s ", hconvPS(xx[pathLen]), vconvPS(yy[pathLen]), LINETO);

  doShading();
  pathLen = 0;
	 
}				/* end of flushSpline */

void
shadeLast()
{
  shading = GREY;
  EMIT(outfp, "0.75 setgray\n");
}				/* end of shadeLast */

void
whitenLast()
{
  shading = WHITE;
  EMIT(outfp, "1 setgray\n");
}				/* end of whitenLast */

void
blackenLast()
{
  shading = BLACK;
  EMIT(outfp, "0 setgray\n");	/* actually this aint needed */
}				/* end of whitenLast */

static void
doShading()
{
  if (shading) 
    {
      EMIT(outfp, "%s\n", FILL);
      shading = NONE;
      EMIT(outfp, "0 setgray\n");	/* default of black */
    }
  else
    EMIT(outfp, "%s\n", STROKE);
}				/* end of doShading */

#ifdef	TWG
extern int h, v;

#define	SCREEN_DPI	72

FILE   *popen ();

void
pbm(cp)
     char *cp;
{
    register int n;
    int    width, height, hres, vres;
    double ratio;
    char   command[BUFSIZ],
	   file[BUFSIZ];
    FILE  *fp;

    if (sscanf (cp, "%d %d %d %d %s", &width, &height, &hres, &vres, file)
		!= 5
	    || width <= 0 || height <= 0 || hres <= 0 || vres <= 0) {
	Warning ("\\special pbm command invalid: \"pbm %s\"\n",cp);
	return;
    }

    if (access (file, 0x04) == -1) {
	Warning ("\\special X Windows bitmap: unable to read file \"%s\"",
		  file);
	return;
    }

    ratio = (double) ((hres + vres) / 2) / SCREEN_DPI;
    ratio = ratio * ratio;
    (void) sprintf (command, "pgmtops -scale %f < %s", ratio, file);
    if ((fp = popen (command, "r"))== NULL) {
	Warning("\\special X Windows bitmap command: unable to create pipe\n");
	return;
    }

    EMIT (outfp, "\ngsave %%%% %s\n%d %d translate 180 rotate\n", file,
	  PixRound (h, hconv) + (int) (width * ratio + 0.5),
	  PixRound (v, vconv) + (int) (height * ratio + 0.5));
    while (fgets (command, sizeof command, fp)) {
	if (strcmp (command, "showpage\n") == 0)
	    EMIT (outfp, "%%%%");

	EMIT (outfp, "%s", command);
	if (strcmp (command, "gsave\n") == 0)
	    EMIT (outfp, "%%%%");
    }

    (void) pclose (fp);

    EMIT (outfp, "grestore %%%%\n");
}
#endif
