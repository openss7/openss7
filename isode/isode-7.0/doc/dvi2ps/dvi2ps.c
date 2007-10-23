/*
 *			D V I 2 P S . C 
 *
 * $Revision: 1.6 $
 *
 * $Log:	dvi2ps.c,v $
 * Revision 1.6  87/05/07  15:00:39  dorab
 * Linked back hh to h undoing a previous change. this was causing
 * subtle quantization problems. (also vv to v) changes were
 * mainly to SetPosn.
 * 
 * Changed the code in SetString to allow strings with a length greater
 * than 256. previously such a string caused a core dump.
 * 
 * Changed the handling of the UC seal. the postscript has to be loaded
 * between bop0 and bop1. this is best done by using the -i option.
 * 
 * Revision 1.5  86/10/01  18:14:33  dorab
 * made changes to allow multiple directories in TEXFONTS variable.
 * this code from <trinkle@purdue.edu>
 * ifdeffed with FONTPATHS.
 * also fixed a couple of typos.
 * 
 * Revision 1.4  86/09/08  11:52:33  dorab
 * merged gf stuff
 * ifdeffed with USEPXL
 * 
 * Revision 1.3  86/04/29  22:59:25  dorab
 * first general release
 * 
 * Revision 1.2  86/04/29  12:53:01  dorab
 * Added distinctive RCS header
 * 
 */
#ifndef lint
static char RCSid[] = "@(#)$Header: dvi2ps.c,v 1.6 87/05/07 15:00:39 dorab Exp $ (UCLA)";
#endif
/*
 *
 * AUTHOR(s)
 *     Mark Senn wrote the early versions of this program for the
 *     BBN BitGraph.  Stephan Bechtolsheim, Bob Brown, Richard
 *     Furuta, James Schaad and Robert Wells improved it.  Norm
 *     Hutchinson ported the program to the Sun.  Neal Holtz ported
 *     it to the Apollo, and from there to producing PostScript
 *     output. Scott Jones added intelligent font substitution.
 *     Howard Trickey made it read GF files instead of PXL ones.
 *
 */

/* Basic method:
 * Using the local font cacheing machinery that was in the previewer,
 * we can easily manage to send the bitmap for each chracter only once.
 * Two passes are made over each page in the DVI file.  The first pass
 * simply outputs the bitmaps for all characters on that page that haven't
 * been sent before.  The second pass outputs all the character setting
 * and positioning commands.  This allows us to bracket the setting portion
 * with PostScript save's and restore's, thus reclaiming considerable
 * virtual memory after each page.
 *
 * All coordinates are output in the PostScript system (TeX origin),
 * and in integer units of rasters (300/inch) -- except for character
 * widths, which are sent as floating point numbers.
 *
 * About 2 pages of PostScript support code must be sent to the LaserWriter
 * before this stuff goes.  It is automatically included unless the
 * -h option is given.
 */


/* Change log:
 *
 * Early 1985, (nmh) -- ported sun version to Apollo. 
 * A little later (nmh) -- changed to continue on in the event of missing
 *                      font files.
 * 30-Mar-85 (nmh) -- added -a option to specify a different PXL area
 * 30-Mar-85 (nmh) -- changed default PXL area to /pxl118
 * 31-Mar-85 (nmh) -- fixed bug in OpenFontFile() regarding more than MAXOPEN
 *                    PXL files -- changed to mark files as closed in font_entry.
 *  7-Apr-85 (nmh) -- made command line argument decoding case insensitive.
 *                    cleaned up handling of DVI file name argument.
 * 30-May-85 (nmh) -- new version; hacked to output PostScript commands
 *  6-Jun-85 (nmh) -- added relative positioning (20% smaller PostScript output)
 *                    add -m option to specify mag
 * 11-Jun-85 (nmh) -- fixed bug regarding char spacings in very long "words"
 * 12-Jun-85 (nmh) -- v1.02 - process DVI pages in reverse order
 * 13-Jun-85 (nmh) -- fixed bug re PXL files getting opened too often when no PreLoad
 * 14-Jun-85 (nmh) -- font dict created in PostScript only when 1st char of font downloaded
 *                    add -m0 -mh -m1 etc. to specify magsteps
 * 16-Aug-85 (nmh) -- added -c option t0 create output file in spool area (Apollo specific)
 *                    added -h option to copy header file to output
 *                    added -o option to pass options through to PostScript
 * 20-Aug-85 (nmh) -- v1.03
 * 24-Aug-85 (nmh) -- add -q option (for quiet operation).
 *                    changed -o option to check PostScript option
 *                    changed to output coordinates in TeX system (but
 *                    scaled to raster units) -- (0,0) at 1" in and down from
 *                      top left (better for use with different size paper).
 *                 -- v2.00
 * 25-Aug-85 (nmh) -- added dictionary enclosure to Tex.ps, and output
 *                      suitable prolog here.
 * 26-Aug-85 (nmh) -- changes to tex.ps to support Macintosh documents.
 * 14-Sep-85 (nmh) -- added keyword=value decoding to \special;
 * 15-Sep-85 (nmh) -- added -i file option.
 * 23-Sep-85 (saj) -- added font substitution for case when font is
 *                    unavailable at requested mag. (a frequent occurrence
 *                    with some macro packages like LaTeX)
 * 16-Dec-85 (drp) -- bugfix to handle font names that are numbers and
 *		      also corrected logic to refrain from
 *		      generating moveto's when the position hasnt changed
 * 15-Jan-86 (drp) -- look for TEXFONTS env variable first
 *                    check if isupper before doing tolower (mark senn
 *		      found this one)
 * 10-Feb-86 (drp) -- made the LW start off at (0,0) at the top of page
 * 14-Mar-86 (drp) -- incorporated some changes stolen from dvisun
 *		      by <morgan@uci.edu>. also changed the way hh
 *		      and vv were set and used. previously, h was
 * 		      always equal to hh.
 *  7-Jun-86 (hwt) -- use gf files ("pxl" survives in variable names)
 *  8-Aug-86 (rkf) -- merged together the pxl and the gf versions.  Define
 *                    USEPXL should you want PXL files---gf is the default
 *  3-Sep-86 (drp) -- merged in the gf changes to tpic version.
 *  8-Sep-86 (drp) -- merged changes allowing multiple paths in TEXFONTS
 *		      environment variable. from <trinkle@purdue.edu>,
 *		      ifdeffed with FONTPATHS. also two minor typos.
 */


/********************************************************************* */
/************************  Global Definitions  *********************** */
/********************************************************************* */

/* This version purports to drive a PostScript device (slowly) */

/********************************************************************* */
/***********************  external definitions  ********************** */
/********************************************************************* */

#include "dvi2ps.h"
#include "commands.h"
#include <sys/param.h>
#include <signal.h>
#include <stdio.h>
#include "findfile.h"
#include <ctype.h>

#ifdef DEBUG
int Debug = 0;
#endif

#ifdef TPIC
extern void setPenSize();
extern void flushPath();
extern void flushDashed();
extern void flushDashed();
extern void addPath();
extern void arc();
extern void flushSpline();
extern void shadeLast();
extern void whitenLast();
extern void blackenLast();
#endif TPIC

char *getenv();
int  access();
char *index();
char *malloc();
int  free();
char *rindex();
char *sprintf();
char *strcpy(); 
char *logname();

#ifndef USEPXL
/* interface to gf.c */
extern FILE *gfin;
extern int checksum;
extern long tfm_wd[], char_pointer[];
extern char char_exists[];
extern int num_cols, num_rows, num_bytes, x_offset, y_offset;
extern unsigned char bits[];
extern int gettochar();
extern void readbits();
extern void readpost();
extern void seekpost();
extern int seekchar();
#endif ~USEPXL

/* formatted i/o was killing us, so build some tables */
char    *digit = "0123456789ABCDEF";

int dummyInt;
short dummyShort;
char dummyChar;

/********************************************************************* */
/*************************  Global Procedures  *********************** */
/********************************************************************* */

/* Note: Global procedures are declared here in alphabetical order, with
   those which do not return values typed "void".  Their bodies occur in
   alphabetical order following the main() procedure.  The names are
   kept unique in the first 6 characters for portability. */

void	AbortRun();
float	ActualFactor();
void	AllDone();
FILE*	BINARYOPEN();
int     ChkOpt();               /* Check PostScript option for validity */
void    CopyFile();
void    DecodeArgs();
void    DoSpecial();
void    EmitChar();
void	Fatal();
void	FindPostAmblePtr();
void	GetBytes();
void	GetFontDef();
char   *GetKeyStr();
int     GetKeyVal();
int     HasBeenRead();
int     IsSame();
void    lcase();
void	MoveDown();
void	MoveOver();
int     NoSignExtend();         /* see cautionary note in code, re arithmetic vs logical shifts */
void	OpenFontFile();
int	PixRound();
void    PutInt();
int	ReadFontDef();
void	ReadPostAmble();
void	SetChar();
void	SetFntNum();
void    SetPosn();
void	SetRule();
void    SetString();
int     SignExtend();           /* see cautionary note in code, re arithmetic vs logical shifts */
void	SkipFontDef();
void	Warning();


/********************************************************************* */
/***********************  Font Data Structures  ********************** */
/********************************************************************* */

struct char_entry {		/* character entry */
#ifdef USEPXL
   unsigned short width, height;/* width and height in pixels */
   short xOffset, yOffset;      /* x offset and y offset in pixels */
#endif USEPXL
   struct {
       int isloaded;
       union {
	   int fileOffset;
	   long *pixptr; } address;
       } where;
   int tfmw;			/* TFM width */
   };

struct font_entry {  /* font entry */
   int k, c, s, d, a, l;
   char n[STRSIZE];	/* FNT_DEF command parameters */
   int font_space;	/* computed from FNT_DEF s parameter */
   int font_mag;	/* computed from FNT_DEF s and d parameters */
   char psname[STRSIZE]; /* PostScript name of the font */
   char name[STRSIZE];	/* full name of PXL file */
   FILE *font_file_id;  /* file identifier (NO_FILE if none) */
#ifdef USEPXL
   int magnification;	/* magnification read from PXL file */
   int designsize;	/* design size read from PXL file */
#endif USEPXL
   struct char_entry ch[NPXLCHARS];/* character information */
   struct font_entry *next;
   int ncdl;            /* # of different chars actually downloaded */
#ifdef STATS
   int nbpxl;           /* # of bytes of PXL data downloaded */
   int ncts;            /* total # of characters typeset */
#endif
   };

struct pixel_list
{
    FILE *pixel_file_id;        /* file identifier */
    int use_count;              /* count of "opens" */
    char name[STRSIZE];		/* name of file for cacheing */
    };



/********************************************************************* */
/*************************  Global Variables  ************************ */
/********************************************************************* */

int   FirstPage = -1000000;     /* first page to print (uses count0) */
int   LastPage = 1000000;       /* last page to print */

char filename[STRSIZE];         /* DVI file name */

#ifdef	MTR
char   *G_bflag = NULL;
int     G_bwidth = 1;

int	G_increment = 12;
#endif
int   G_create = FALSE;         /* create an output file in spool area ? */
int   G_errenc = FALSE;	        /* has an error been encountered? */
int   G_header = TRUE;          /* copy header file to output? */
char  G_Logname[STRSIZE];       /* name of log file, if created */
int   G_interactive = FALSE;    /* is the program running interactively */
                                /* (i.e., standard output not redirected)? */
int   G_logging = 0;            /* Are we logging warning messages? */
int   G_logfile = FALSE;        /* Are these messages going to a log file? */
FILE *G_logfp;                  /* log file pointer (for errors) */
char  G_progname[STRSIZE];      /* program name */
int   G_quiet = FALSE;          /* for quiet operation */
int   G_nowarn = FALSE;         /* don't print out warnings */

int   hconv, vconv;		/* converts DVI units to pixels */
int   den;			/* denominator specified in preamble */
FILE *dvifp  = NULL;		/* DVI file pointer */
int   PreLoad = TRUE;	        /* preload the font descriptions? */
struct font_entry *prevfont=NULL;  /* font_entry pointer, previous character */
struct font_entry *fontptr;		/* font_entry pointer */
struct font_entry *hfontptr=NULL;	/* font_entry pointer */
int   h;				/* current horizontal position */
int   hh = 0;                           /* current h on device */
char *Ifile[100];                       /* files to include */
int   v;				/* current vertical position */
int   vv = 0;                           /* current v on device */
int   mag;			/* magnification specified in preamble */
int   ncopies = 1;              /* number of copies to print */
int   ndone = 0;                /* number of pages converted */
int   nif = 0;                  /* number of files to include */
int   nopen;			/* number of open PXL files */
int   nps = 0;                  /* number of PostScript commands to send */
int   num;			/* numerator specified in preamble */
FILE  *outfp = NULL;            /* output file */
struct font_entry *pfontptr = NULL; /* previous font_entry pointer */
struct pixel_list pixel_files[MAXOPEN+1];
                                /* list of open PXL file identifiers */
long  postambleptr;		/* Pointer to the postamble */
FILE *pxlfp;			/* PXL file pointer */
char *PXLpath;			/* PXL path name for search */
long  ppagep;	                /* previous page pointer */
char *PScmd[100];               /* PostScript commands to send */
int   Reverse = TRUE;         /* process DVI pages in reverse order ? */
char  rootname[STRSIZE];      /* DVI filename without extension */
#ifdef STATS
int   Stats = FALSE;          /* are we reporting stats ? */
int   Snbpxl = 0;             /* # of bytes of pixel data */
int   Sonbpx = 0;             /* "optimal" number of bytes of pixel data */
int   Sndc = 0;               /* # of different characters typeset */
int   Stnc = 0;               /* total # of chars typeset */
int   Snbpx0, Sndc0, Stnc0;   /* used for printing incremental changes per dvi page */
#endif

int usermag = 0;              /* user specified magnification */

#ifdef FONTPATHS
/* to handle multiple paths in TEXFONTS env variable */
char *rgsbPXLPaths[MAXFONTPATHS];
int  csbPXLPaths;
#endif FONTPATHS

/********************************************************************* */
/*******************************  main  ****************************** */
/********************************************************************* */

main(argc, argv)
int argc;
char *argv[];

{
    struct stack_entry {  /* stack entry */
	int h, v, w, x, y, z;  /* what's on stack */
    };


    int command;	    /* current command */
    int count[10];          /* the 10 counters at begining of each page */
    long cpagep;	    /* current page pointer */
    int  Emitting = FALSE;  /* are we outputting typsetting instructions? */
    register int i;	    /* command parameter; loop index */
    int k;		    /* temporary parameter */
    char n[STRSIZE];	    /* command parameter */
    int PassNo = 0;         /* which pass over the DVI page are we on? */
    int SkipMode = FALSE;   /* in skip mode flag */
    int sp;		    /* stack pointer */
    struct stack_entry stack[STACKSIZE];   /* stack */
    int t;		    /* temporary */
    char SpecialStr[STRSIZE]; /* "\special" strings */
    register struct char_entry *tcharptr; /* temporary char_entry ptr */
    int val, val2;          /* temporarys to hold command information */
    int w;		    /* current horizontal spacing */
    int x;		    /* current horizontal spacing */
    int y;		    /* current vertical spacing	 */
    int z;		    /* current vertical spacing	 */

    nopen = 0;
    strcpy(G_progname, argv[0]);

    /* if the TEXFONTS env variable is there, use it */
    if ((PXLpath = getenv("TEXFONTS")) == (char *) 0)
	PXLpath = FONTAREA;   /* default font area */

    DecodeArgs( argc, argv );

#ifdef FONTPATHS
    csbPXLPaths = SeparatePaths(PXLpath, rgsbPXLPaths);
#endif FONTPATHS

#ifdef apollo
    set_sbrk_size( 2048*1024 );
#endif

    if ((i = NoSignExtend(dvifp, 1)) != PRE)  {
	fprintf(stderr,"\n");
	Fatal("%s: PRE doesn't occur first--are you sure this is a DVI file?\n\n",
	G_progname);
    }

    i = SignExtend(dvifp, 1);
    if (i != DVIFORMAT)  {
	fprintf(stderr,"\n");
	Fatal("%s: DVI format = %d, can only process DVI format %d files\n\n",
	G_progname, i, DVIFORMAT);
    }

        outfp = stdout;

#ifdef	MTR
    for( i=0; i<nps; i++ )
        if( !ChkOpt(PScmd[i]) )
            Fatal( "%s is an invalid option\n", PScmd[i] );
    if (!G_bflag)
	prologue ();
#else
/* it is important that these be the very first things output !!! */

    if( G_header )
        CopyFile( HDRFILE );

    for( i=0; i<nif; i++ )      /* copy all included files */
        CopyFile( Ifile[i] );

    EMIT(outfp, "TeXDict begin @start\n");
    EMIT(outfp, "%%%%Title: %s\n", filename);
    EMIT(outfp, "%%%%Creator: %s\n", G_progname);
    EMIT(outfp, "%%%%EndProlog\n");

    for( i=0; i<nps; i++ )      /* prefix valid PostScript options with a "@" */
        if( ChkOpt(PScmd[i]) )
            EMIT(outfp, "@%s\n", PScmd[i]);
        else
            Fatal( "%s is an invalid option\n", PScmd[i] );

    if( ncopies > 1 )
        EMIT(outfp, "%d @copies\n", ncopies);
#endif

    if (Reverse) {
        ReadPostAmble(PreLoad);
        fseek(dvifp, ppagep, 0);
        }
    else {
        if (PreLoad) {
            ReadPostAmble(TRUE);
            fseek(dvifp, (long) 14, 0);
            }
        else {
#ifdef	MTR
	    if (G_bflag)
		Fatal ("must preload if using -b\n", 0);
#endif
    	    num = NoSignExtend(dvifp, 4);
    	    den = NoSignExtend(dvifp, 4);
    	    mag = NoSignExtend(dvifp, 4);
#ifdef USEGLOBALMAG
            if( usermag > 0 && usermag != mag )
                fprintf(stderr, "DVI magnification of %d over-ridden by user mag of %d\n", mag, usermag );
#endif
            if( usermag > 0 ) mag = usermag;
#ifndef USEGLOBALMAG
            if( mag != 1000 ) fprintf(stderr, "Magnification of %d ignored.\n", mag);
#endif
    	    hconv = DoConv(num, den, hconvRESOLUTION);
    	    vconv = DoConv(num, den, vconvRESOLUTION);
            }
        k = NoSignExtend(dvifp, 1);
        GetBytes(dvifp, n, k);
        }

    PassNo = 0;
    
    while (TRUE)

	switch (command=NoSignExtend(dvifp, 1))  {

	case SET1:case SET2:case SET3:case SET4:
	    val = NoSignExtend(dvifp, command-SET1+1);
	    if (!SkipMode) SetChar(val, command, PassNo);
	    break;

	case SET_RULE:
	    val = NoSignExtend(dvifp, 4);
	    val2 = NoSignExtend(dvifp, 4);
            if (Emitting) SetRule(val, val2, 1);
	    break;

	case PUT1:case PUT2:case PUT3:case PUT4:
	    val = NoSignExtend(dvifp,command-PUT1+1);
	    if (!SkipMode) SetChar(val, command, PassNo);
	    break;

	case PUT_RULE:
	    val = NoSignExtend(dvifp, 4);
	    val2 = NoSignExtend(dvifp, 4);
            if (Emitting) SetRule(val, val2, 0);
	    break;

	case NOP:
	    break;

	case BOP:
	    cpagep = ftell(dvifp) - 1;
	    for (i=0; i<=9; i++)
		count[i] = NoSignExtend(dvifp, 4);
	    ppagep = NoSignExtend(dvifp, 4);

	    h = v = w = x = y = z = 0;
            hh = vv = 0;
	    sp = 0;
	    fontptr = NULL;
            prevfont = NULL;

            if( count[0] < FirstPage || count[0] > LastPage )
                SkipMode = TRUE;
            else
                SkipMode = FALSE;

            Emitting = (PassNo != 0) && !SkipMode;
#ifdef	MTR
	    if ((PassNo == 0) && G_bflag)
		init_new_file ();
#endif

            if( !SkipMode ) {
                if( PassNo == 0) {
                        EMIT(outfp,"%d @bop0\n", count[0]);
#ifdef STATS
                        if( Stats ) {
                                Sndc0 = Sndc;
                                Stnc0 = Stnc;
                                Snbpx0 = Snbpxl;
                                }
#endif
#ifndef	MTR
                        if( !G_quiet ) 
                                fprintf(stderr, "[%d", count[0] );
#else
                        if( !G_quiet ) {
			        if (ndone)
				    fprintf (stderr, " ");
                                fprintf(stderr, "[%d", count[0] );
				fflush(stderr);
			    }
#endif
                        }
                else
                        EMIT(outfp,"%d @bop1\n", count[0]);
                }
	    break;

	case EOP:
            if( !SkipMode ) {
                if( PassNo == 0 ) {     /* start second pass on current page */
                    fseek(dvifp,cpagep,0);
                    PassNo = 1;
                    }
                else {                  /* end of second pass, and of page processing */
                    EMIT(outfp,"@eop\n");
#ifdef STATS
                    if( Stats )
                        fprintf(stderr," - %d total ch,  %d diff ch,  %d pxl bytes]\n",
                                Stnc-Stnc0, Sndc-Sndc0, Snbpxl-Snbpx0);   
                    else
#endif
                        if( !G_quiet ) {
#ifndef	MTR
                                fprintf(stderr,"] ");
                                if( (++ndone % 10) == 0 ) fprintf(stderr,"\n");
#else
				fprintf(stderr,"]");
				if (++ndone >= 10) {
				    fprintf (stderr, "\n");
				    ndone = 0;
				}
				else
				    fflush(stderr);
#endif
                                }
                    PassNo = 0;
                    }
                }
            else
                PassNo = 0;
            if( PassNo == 0 && Reverse ) { 
                if( ppagep > 0 )
                    fseek(dvifp, ppagep, 0);
                else 
                    AllDone();
                }
	    break;

	case PUSH:
	    if (sp >= STACKSIZE)
		Fatal("stack overflow");
	    stack[sp].h = h;
	    stack[sp].v = v;
	    stack[sp].w = w;
	    stack[sp].x = x;
	    stack[sp].y = y;
	    stack[sp].z = z;
	    sp++;
	    break;

	case POP:
	    --sp;
	    if (sp < 0)
		Fatal("stack underflow");
	    h = stack[sp].h;
	    v = stack[sp].v;
	    w = stack[sp].w;
	    x = stack[sp].x;
	    y = stack[sp].y;
	    z = stack[sp].z;
	    break;

	case RIGHT1:case RIGHT2:case RIGHT3:case RIGHT4:
            val = SignExtend(dvifp,command-RIGHT1+1);
	    if (Emitting) MoveOver(val);
	    break;

	case W0:
            if (Emitting) MoveOver(w);
	    break;

	case W1:case W2:case W3:case W4:
	    w = SignExtend(dvifp,command-W1+1);
            if (Emitting) MoveOver(w);
	    break;

	case X0:
            if (Emitting) MoveOver(x);
	    break;

	case X1:case X2:case X3:case X4:
	    x = SignExtend(dvifp,command-X1+1);
	    if (Emitting) MoveOver(x);
	    break;

	case DOWN1:case DOWN2:case DOWN3:case DOWN4:
            val = SignExtend(dvifp,command-DOWN1+1);
	    if (Emitting) MoveDown(val);
	    break;

	case Y0:
            if (Emitting) MoveDown(y);
	    break;

	case Y1:case Y2:case Y3:case Y4:
	    y = SignExtend(dvifp,command-Y1+1);
            if (Emitting) MoveDown(y);
	    break;

	case Z0:
            if (Emitting) MoveDown(z);
	    break;

	case Z1:case Z2:case Z3:case Z4:
	    z = SignExtend(dvifp,command-Z1+1);
	    if (Emitting) MoveDown(z);
	    break;

	case FNT1:case FNT2:case FNT3:case FNT4:
	    k = NoSignExtend(dvifp,command-FNT1+1);
	    if (!SkipMode)
		SetFntNum(k, Emitting);
	    break;

	case XXX1:case XXX2:case XXX3:case XXX4:
	    k = NoSignExtend(dvifp,command-XXX1+1);
            GetBytes(dvifp, SpecialStr, k);
            if (Emitting) DoSpecial(SpecialStr, k);
	    break;

	case FNT_DEF1:case FNT_DEF2:case FNT_DEF3:case FNT_DEF4:
            k = NoSignExtend(dvifp, command-FNT_DEF1+1);
	    if (PreLoad || HasBeenRead(k) )
	    {
		SkipFontDef (k);
	    }
	    else
	    {
		ReadFontDef (k);
	    }
	    break;

	case PRE:
	    Fatal("PRE occurs within file");
	    break;

	case POST:
            AllDone();
	    break;

	case POST_POST:
 	    Fatal("POST_POST with no preceding POST");
	    break;

	default:
	    if (command >= FONT_00 && command <= FONT_63)
		{if (!SkipMode)
                     SetFntNum(command - FONT_00, Emitting);}
	    else if (command >= SETC_000 && command <= SETC_127)
		{if (!SkipMode) SetString(command, PassNo);}
	    else
		Fatal("%d is an undefined command", command);
	    break;

	}

}

#ifdef	MTR
prologue () {
    int	    i;

    if( G_header )
        CopyFile( HDRFILE );

    for( i=0; i<nif; i++ )      /* copy all included files */
        CopyFile( Ifile[i] );

    EMIT(outfp, "TeXDict begin @start\n");
    EMIT(outfp, "%%%%Title: %s\n", filename);
    EMIT(outfp, "%%%%Creator: %s\n", G_progname);
    EMIT(outfp, "%%%%EndProlog\n");

    for( i=0; i<nps; i++ )      /* prefix valid PostScript options with a "@" */
            EMIT(outfp, "@%s\n", PScmd[i]);

    if( ncopies > 1 )
        EMIT(outfp, "%d @copies\n", ncopies);
}


init_new_file ()
{
    static int page_no = 1;
    static int file_no = 0;
    static char    buffer[BUFSIZ];

    if (file_no) {
	register int	i;
	register struct font_entry *fp;

	if (page_no++ < G_increment)
	    return;

	EMIT(outfp,"@end\n");

	(void) fflush (stdout);
	if (ferror (stdout))
	Fatal ("error writing %s\n", buffer);

	if (!G_quiet)
	    fprintf (stderr, ")\n");
	page_no = 1;

	for (fp = hfontptr; fp; fp = fp -> next) {
#ifdef	USEPXL
	    register struct char_entry *cp;
#endif

	    fp -> ncdl = 0;
	    for (i = FIRSTPXLCHAR; i <= LASTPXLCHAR; i++)
#ifdef	USEPXL
		if ((cp = &fp -> ch[i]) -> where.isloaded) {
		    long *pr = cp -> where.address.pixptr;

		    if (pr)
			free ((char *) pr);
		    cp -> where.isloaded = FALSE;
		}
#else
		fp -> ch[i].where.isloaded = FALSE;
#endif
	}
    }

    (void) sprintf (buffer, "%s-%.*d.ps", G_bflag, G_bwidth, ++file_no);
    if (freopen (buffer, "w", stdout) == NULL)
	Fatal ("unable to write %s\n", perror (buffer));

    if (!G_quiet) {
	fprintf (stderr, "(%s", buffer);
	fflush (stderr);

	ndone = 1;
    }

    prologue ();
}
#endif

/*-->AbortRun */
/********************************************************************* */
/***************************  AbortRun  ****************************** */
/********************************************************************* */

void
AbortRun(code)
int code;
{
    exit(code);
}


/*-->ActualFactor */
/********************************************************************* */
/**************************  ActualFactor  *************************** */
/********************************************************************* */

float		/* compute the actual size factor given the approximation */
ActualFactor(unmodsize)
int unmodsize;  /* actually factor * 1000 */
{
    float realsize;	/* the actual magnification factor */

    realsize = (float)unmodsize / 1000.0;
    /* a real hack to correct for rounding in some cases--rkf */
    if(unmodsize==1095) realsize = 1.095445;	/*stephalf */
    else if(unmodsize==1315) realsize=1.314534;	/*stepihalf */
    else if(unmodsize==1577) realsize=1.577441;	/*stepiihalf */
    else if(unmodsize==1893) realsize=1.892929;	/*stepiiihalf */
    else if(unmodsize==2074) realsize=2.0736;	/*stepiv */
    else if(unmodsize==2488) realsize=2.48832;  /*stepv */
    else if(unmodsize==2986) realsize=2.985984;	/*stepiv */
    /* the remaining magnification steps are represented with sufficient
	   accuracy already */
    return(realsize);
}


/*-->AllDone */
/********************************************************************* */
/****************************** AllDone  ***************************** */
/********************************************************************* */

void
AllDone()
{
    int t;
    struct font_entry *p;
    int per;

    EMIT(outfp,"@end\n");
#ifndef	MTR
    if( !G_quiet ) fprintf(stderr,"\n");
#else
    if (!G_quiet) {
	if (G_bflag)
	    fprintf (stderr, ")");
	fprintf (stderr, "\n");
    }
#endif


    if (G_errenc && G_logging == 1 && G_logfile)  {
	fseek(G_logfp, 0L, 0);
	while ((t=getc(G_logfp)) != EOF)
	    putchar(t);
    }
    if (G_logging == 1 && G_logfile) printf("Log file created\n");

#ifdef STATS
    if (Stats) {
        fprintf( stderr, "Total chars   diff chars   pxl bytes\n" );
        fprintf( stderr, "      #   %%        #   %%       #   %%\n" );
        fprintf( stderr, "------- ---   ------ ---   ----- ---\n" );
        for( p=hfontptr; p!=NULL; p=p->next ) {
                fprintf( stderr, "%7d%4d", p->ncts, (100*p->ncts + Stnc/2)/Stnc );
                fprintf( stderr, "%9d%4d", p->ncdl, (100*p->ncdl + Sndc/2)/Sndc );
                fprintf( stderr, "%8d%4d", p->nbpxl, (100*p->nbpxl + Snbpxl/2)/Snbpxl );
                fprintf( stderr, "  %s\n", p->psname );
                }
        fprintf(stderr, "\nTotal number of characters typeset: %d\n", Stnc);
        fprintf(stderr, "Number of different characters downloaded: %d\n", Sndc);
        fprintf(stderr, "Number of bytes of pxl data downloaded: %d\n", Snbpxl);
        fprintf(stderr, "Optimal # of bytes of pxl data: %d\n", Sonbpx);
        }
#endif

    AbortRun(G_errenc);
}


/*-->ChkOpt */   /* check a user supplied option for validity */
/******************************************************************** */
/****************************** ChkOpt ****************************** */
/******************************************************************** */

#define ISOPT(s) if( EQ(str,s) ) return( TRUE )

int
ChkOpt( str )
char    *str;
{
/*        lcase(str);  doesn't work */

        ISOPT("note");          /* its a shame to build this into the program */
        ISOPT("letter");
        ISOPT("legal");
        ISOPT("landscape");
        ISOPT("manualfeed");
        return( FALSE );
}


/*-->CopyFile */   /* copy a file straight through to output */
/******************************************************************** */
/***************************** CopyFile ***************************** */
/******************************************************************** */

void
CopyFile( str )
char    *str;
{
        FILE    *spfp;
        char    t;

        if( (spfp=fopen(str,"r")) == NULL ) {
                fprintf(stderr,"Unable to open file %s\n", str );
                return;
                }
#ifndef	MTR
        if( !G_quiet ) fprintf(stderr," [%s", str);
#else
        if( !G_quiet ) {
	    if (ndone)
		fprintf (stderr, " ");
	     fprintf(stderr,"[%s", str);
	     fflush (stderr);
	 }
#endif
        while( (t = getc(spfp)) != EOF ) {
                EMITC(t);
                }              
        fclose(spfp);
#ifndef	MTR
        if( !G_quiet ) fprintf(stderr,"]");
#else
	ndone += strlen (str) / 5;
        if( !G_quiet ) {
	    fprintf (stderr,"]");
	    if (++ndone >= 10) {
		fprintf (stderr, "\n");
		ndone = 0;
	    }
	    else
		fflush (stderr);
	}
#endif
}


/*-->DecodeArgs */
/******************************************************************** */
/***************************** DecodeArgs *************************** */
/******************************************************************** */

void
DecodeArgs( argc, argv )
int argc;
char *argv[];
{
    int argind;             /* argument index for flags */
    char curarea[STRSIZE];  /* current file area		 */
    char curname[STRSIZE];  /* current file name		 */
    char *tcp, *tcp1;	    /* temporary character pointers	 */

    argind = 1;
    while (argind < argc) {
	tcp = argv[argind];
        if (*tcp == '-')
	    switch(isupper(*++tcp) ? tolower(*tcp) : *tcp) {

                case 'a':       /* a selects different pxl font area */
                    PXLpath = argv[++argind];
                    break;
#ifdef	MTR
		case 'b':
		    if (G_bflag)
			Fatal ("too many prefixes\n", 0);
		    G_bflag = argv[++argind];
		    break;

		case 'z':
		    if (++argind >= argc
			    || sscanf (argv[argind], "%d", &G_increment) != 1
			    || G_increment < 0)
			Fatal ("Argument is not a valid integer\n", 0);
		    break;
#endif
#ifdef DEBUG
		case 'd':	/* d selects Debug output */
		    Debug = TRUE;
		    break;
#endif
                case 'f':       /* next arg is starting pagenumber */
                    if( ++argind >= argc || sscanf(argv[argind], "%d", &FirstPage) != 1 )
                        Fatal("Argument is not a valid integer\n", 0);
                    break;

                case 'h':       /* don't copy PostScript header file through to output */
                    G_header = FALSE;
                    break;

                case 'i':       /* next arg is a PostScript file to copy */
                    if( ++argind >= argc )
                        Fatal("No argument following -i\n", 0);
                    Ifile[nif++] = argv[argind];
                    break;

		case 'l':	/* l prohibits logging of errors */
		    G_logging = -1;
		    break;
#ifdef USEGLOBALMAG
                case 'm':       /* specify magnification to use */
                    switch( *++tcp ) {

                    case '\0':       /* next arg is a magnification to use */
                        if( ++argind >= argc || sscanf(argv[argind], "%d", &usermag) != 1 )
                            Fatal("Argument is not a valid integer\n", 0);
                        break; 
                    case '0': usermag = 1000; break;
		    case 'H':
                    case 'h': usermag = 1095; break;
                    case '1': usermag = 1200; break;
                    case '2': usermag = 1440; break;
                    case '3': usermag = 1728; break;
                    case '4': usermag = 2074; break;
                    case '5': usermag = 2488; break;
                    default: Fatal("%c is a bad mag step\n", *tcp);
                    }
                    break;
#endif
                case 'n':       /* next arg is number of copies to print */
                    if( ++argind >= argc || sscanf(argv[argind], "%d", &ncopies) != 1 )
                        Fatal("Argument is not a valid integer\n", 0);
                    break;    

                case 'o':       /* next arg is a PostScript command to send */
                    if( ++argind >= argc )
                        Fatal("No argument following -o\n", 0);
                    PScmd[nps++] = argv[argind];
                    break;

		case 'p':	/* p prohibits pre-font loading */
		    PreLoad = 0;
                    Reverse = FALSE;    /* must then process in forward order */
		    break;

                case 'q':       /* quiet operation */
                    G_quiet = TRUE;
                    break;

                case 'r':       /* don't process pages in reverse order */
                    Reverse = FALSE;
                    break;
#ifdef STATS                   
                case 's':       /* print some statistics */
                    Stats = TRUE;
                    break;
#endif
                case 't':       /* next arg is ending pagenumber */
                    if( ++argind >= argc || sscanf(argv[argind], "%d", &LastPage) != 1 )
                        Fatal("Argument is not a valid integer\n", 0);
                    break;

                case 'w':       /* don't print out warnings */
                    G_nowarn = TRUE;
                    break;

		default:
		    printf("%c is not a legal flag\n", *tcp);
		}

        else {

            tcp = rindex(argv[argind], '/');    /* split into directory + file name */
            if (tcp == NULL)  {
        	curarea[0] = '\0';
        	tcp = argv[argind];
                }
            else  {
        	strcpy(curarea, argv[argind]);
                curarea[tcp-argv[argind]+1] = '\0';
        	tcp += 1;
                }
        
            strcpy(curname, tcp);
            tcp1 = rindex(tcp, '.');   /* split into file name + extension */
            if (tcp1 == NULL) {
                strcpy(rootname, curname);
                strcat(curname, ".dvi");
                }
            else {
                *tcp1 = '\0';
                strcpy(rootname, curname);
                *tcp1 = '.';
                }
        
            strcpy(filename, curarea);
            strcat(filename, curname);
        
            if ((dvifp=BINARYOPEN(filename,"r")) == NULL)  {
        	fprintf(stderr,"\n");
        	fprintf(stderr,"%s: can't find DVI file \"%s\"\n\n", G_progname, filename);
        	AbortRun(1);
                }
        
            strcpy(G_Logname, curname);
            strcat(G_Logname, ".log");
	    }
	argind++;
        }

    if (dvifp == NULL)  {
	fprintf(stderr, 
                "\nusage: %s [-a area] [-c] [-h] [-o option] [-p] [-s] [-r] [-f n] [-t n] [-m{0|h|1|2|3|4|  mag] [-a fontarea] dvifile\n\n", 
                G_progname);
	AbortRun(1);
        }
}


/*-->DoConv */
/******************************************************************** */
/********************************  DoConv  ************************** */
/******************************************************************** */

int
DoConv(num, den, convResolution)
{
    register float conv;
    conv = ((float)num/(float)den) * 
#ifdef USEGLOBALMAG
	ActualFactor(mag) *
#endif
	((float)convResolution/254000.0);
    return((int) (1.0 / conv + 0.5));
}


/*-->DoSpecial */
/******************************************************************** */
/*****************************  DoSpecial  ************************** */
/******************************************************************** */

typedef enum {None, String, Integer, Number, Dimension} ValTyp;

typedef struct {
        char    *Key;           /* the keyword string */
        char    *Val;           /* the value string */
        ValTyp  vt;             /* the value type */
        union {                 /* the decoded value */
            int  i;
            float n;
            } v;
        } KeyWord;

typedef struct {
        char    *Entry;
        ValTyp  Type;
        } KeyDesc;

#define PSFILE 0
KeyDesc KeyTab[] = {{"psfile", String},
                    {"hsize", Dimension},
                    {"vsize", Dimension},
                    {"hoffset", Dimension},
                    {"voffset", Dimension},
                    {"hscale", Number},
                    {"vscale", Number}};

#define NKEYS (sizeof(KeyTab)/sizeof(KeyTab[0]))

void
DoSpecial( str, n )          /* interpret a \special command, made up of keyword=value pairs */
char    *str;
int n;
{ 
        char spbuf[STRSIZE]; 
        char *sf = NULL;
        KeyWord k;
        int i;

        str[n] = '\0';
        spbuf[0] = '\0';
        SetPosn(h, v);

#ifdef TPIC
	if (strncmp(str, "pn ", 3) == 0) setPenSize(str+2);
	else if (strncmp(str, "fp",2) == 0) flushPath();
	else if (strncmp(str, "da ", 3) == 0) flushDashed(str+2, 0);
	else if (strncmp(str, "dt ", 3) == 0) flushDashed(str+2, 1);
	else if (strncmp(str, "pa ", 3) == 0) addPath(str+2);
	else if (strncmp(str, "ar ", 3) == 0) arc(str+2);
	else if (strncmp(str, "sp", 2) == 0) flushSpline();
	else if (strncmp(str, "sh", 2) == 0) shadeLast();
	else if (strncmp(str, "wh", 2) == 0) whitenLast();
	else if (strncmp(str, "bk", 2) == 0) blackenLast();
	else if (strncmp(str, "tx ", 3) == 0)
	  Warning("\\special texture command: \"%s\" ignored\n",str);
#ifdef	TWG
	else if (strncmp(str, "pbm ", 4) == 0) pbm(str+3);
#endif
	else goto oldstuff;	/* if no tpic stuff do oldstuff */
	return;			/* if any tpic stuff then return */
      oldstuff:
#endif TPIC

        EMITS("@beginspecial\n");

        while( (str=GetKeyStr(str,&k)) != NULL ) {      /* get all keyword-value pairs */
                              /* for compatibility, single words are taken as file names */
                if( k.vt == None && access(k.Key,0) == 0) {
                        if( sf ) Warning("  More than one \\special file name given. %s ignored", sf );
                        strcpy(spbuf, k.Key);
                        sf = spbuf;
                        }
                else if( GetKeyVal( &k, KeyTab, NKEYS, &i ) && i != -1 ) {
                        if( i == PSFILE ) {
                                if( sf ) Warning("  More than one \\special file name given. %s ignored", sf );
                                strcpy(spbuf, k.Val);
                                sf = spbuf;
                                }
                        else            /* the keywords are simply output as PS procedure calls */
                                EMIT(outfp, "%f @%s\n", k.v.n, KeyTab[i].Entry);
                        }
                else Warning("  Invalid keyword or value in \\special - \"%s\" ignored", k.Key );
                }

        EMITS("@setspecial\n");

        if( sf )
                CopyFile( sf );
        else
                Warning("  No special file name provided.");

        EMITS("@endspecial\n");
}


/*-->EmitChar */
/********************************************************************* */
/****************************  EmitChar  ***************************** */
/********************************************************************* */

#ifndef USEPXL
/* assume we just called  readbits() */
#endif ~USEPXL

void
EmitChar(c, ce)              /* output a character bitmap */
int c;
struct char_entry *ce;
{
#ifdef USEPXL
        int i;
        register int j;
#endif USEPXL
        register unsigned char *sl;
#ifdef USEPXL
        register int cc;
        int nbpl, nwpl;
#else ~USEPXL
	register int cc, i;
	int nbpl;
#endif ~USEPXL
        float cw;       /* char width, in "dots" - we rely on PostScript maintaining sufficient accuracy */

                        /* Output in PostScript coord system (y +ive up, x +ive right)
                           (0,0) of char bitmap at lower left.  Output scan lines
                           from bottom to top */

        if( fontptr->ncdl == 0 )      /* open font dict before first char */
                EMIT(outfp,"[ %d ] /%s @newfont\n", fontptr->font_mag, fontptr->psname );
 
        if( fontptr != prevfont ) {   /* because this isn't done on pass 0 */
                EMIT(outfp,"(%s) @sf\n", fontptr->psname);
                prevfont = fontptr;
                }
        EMITS("[<");
        cc = 2;
#ifdef USEPXL
        nbpl = (ce->width + 7) >> 3;
        nwpl = (ce->width + 31) >> 5;
        for(i = ce->height-1;  i >= 0;  i--) {
                sl = (unsigned char *)(ce->where.address.pixptr + i*nwpl);
                for(j = 0;  j < nbpl;  j++, sl++) {
                        if( cc > 100 ) {
                                EMITS("\n  ");   cc = 2; }
                        EMITH(*sl);
                        cc += 2;
                        }
#else ~USEPXL
		nbpl = (num_cols + 7) >> 3;
		for(i=0, sl=bits; i<num_bytes; i++, sl++) {
		        if( cc > 100 ) { EMITS("\n  ");   cc = 2; }
			EMITH(*sl);
			cc += 2;
#endif ~USEPXL
                }
        cw = (float)ce->tfmw / (float)hconv;
        EMIT(outfp,"> %d %d %d %d %.3f] %d @dc\n", 
#ifdef USEPXL
             nbpl<<3, ce->height, ce->xOffset, (((int)ce->height)-ce->yOffset)-1, cw,
             c);
#else ~USEPXL
	     nbpl<<3, num_rows, x_offset, y_offset, cw, c);
#endif ~USEPXL
        fontptr->ncdl += 1;

#ifdef STATS
#ifdef USEPXL
        Snbpxl += nbpl*ce->height;
        fontptr->nbpxl += nbpl*ce->height;
        Sonbpx += (ce->width*ce->height + 7) >> 3;
#else ~USEPXL
        Snbpxl += nbpl*num_rows;
        fontptr->nbpxl += nbpl*num_rows;
        Sonbpx += (num_cols*num_rows + 7) >> 3;
#endif ~USEPXL
        Sndc += 1;
#endif
}


/*-->Fatal */
/********************************************************************* */
/******************************  Fatal  ****************************** */
/********************************************************************* */

/* VARARGS2 */
void
Fatal(fmt, a, b, c)/* issue a fatal error message */
char *fmt;	/* format */
char *a, *b, *c;	/* arguments */

{
    if (G_logging == 1 && G_logfile)
    {
	fprintf(G_logfp, "%s: FATAL--", G_progname);
	fprintf(G_logfp, fmt, a, b, c);
	fprintf(G_logfp, "\n");
    }

    fprintf(stderr,"\n");
    fprintf(stderr, "%s: FATAL--", G_progname);
    fprintf(stderr, fmt, a, b, c);
    fprintf(stderr, "\n\n");
    if (G_logging == 1) printf("Log file created\n");
    AbortRun(1);
}


/*-->FindPostAmblePtr */
/********************************************************************* */
/************************  FindPostAmblePtr  ************************* */
/********************************************************************* */

void
FindPostAmblePtr(postambleptr)
long	*postambleptr;

/* this routine will move to the end of the file and find the start
    of the postamble */

{
    int     i;

    fseek (dvifp, 0L, 2);   /* goto end of file */
    *postambleptr = ftell (dvifp) - 4;
    fseek (dvifp, *postambleptr, 0);

    while (TRUE) {
	fseek (dvifp, --(*postambleptr), 0);
	if (((i = NoSignExtend(dvifp, 1)) != 223) &&
	    (i != DVIFORMAT))
	    Fatal ("Bad end of DVI file");
	if (i == DVIFORMAT)
	    break;
    }
    fseek (dvifp, (long) ((*postambleptr) - 4), 0);
    (*postambleptr) = NoSignExtend(dvifp, 4);
    fseek (dvifp, *postambleptr, 0);
}


/*-->GetBytes */
/********************************************************************* */
/*****************************  GetBytes  **************************** */
/********************************************************************* */

void
GetBytes(fp, cp, n)	/* get n bytes from file fp */
register FILE *fp;	/* file pointer	 */
register char *cp;	/* character pointer */
register int n;		/* number of bytes */

{
    while (n--)
	*cp++ = getc(fp);
}


/*-->GetFontDef */
/********************************************************************* */
/**************************** GetFontDef  **************************** */
/********************************************************************* */

void
GetFontDef()

/***********************************************************************
   Read the font  definitions as they  are in the  postamble of the  DVI
   file.
********************************************************************** */

{
    char    *calloc ();
    unsigned char   byte;

    while (((byte = NoSignExtend(dvifp, 1)) >= FNT_DEF1) &&
	(byte <= FNT_DEF4)) {
	switch (byte) {
	case FNT_DEF1:
	    ReadFontDef (NoSignExtend(dvifp, 1));
	    break;
	case FNT_DEF2:
	    ReadFontDef (NoSignExtend(dvifp, 2));
	    break;
	case FNT_DEF3:
	    ReadFontDef (NoSignExtend(dvifp, 3));
	    break;
	case FNT_DEF4:
	    ReadFontDef (NoSignExtend(dvifp, 4));
	    break;
	default:
	    Fatal ("Bad byte value in font defs");
	    break;
	}
    }
    if (byte != POST_POST)
	Fatal ("POST_POST missing after fontdefs");
}


/*-->GetKeyStr */
/********************************************************************* */
/*****************************  GetKeyStr  *************************** */
/********************************************************************* */

        /* extract first keyword-value pair from string (value part may be null)
         * return pointer to remainder of string
         * return NULL if none found
 */

char    KeyStr[STRSIZE];
char    ValStr[STRSIZE];

char *GetKeyStr( str, kw )
char    *str;
KeyWord *kw;
{
        char *s, *k, *v, t;

        if( !str ) return( NULL );

        for( s=str; *s == ' '; s++ ) ;                  /* skip over blanks */
        if( *s == '\0' ) return( NULL );

        for( k=KeyStr;                          /* extract keyword portion */
             *s != ' ' && *s != '\0' && *s != '='; 
             *k++ = *s++ ) ;
        *k = '\0';
        kw->Key = KeyStr;
        kw->Val = v = NULL;
        kw->vt = None;

        for( ; *s == ' '; s++ ) ;                       /* skip over blanks */
        if( *s != '=' )                         /* look for "=" */
                return( s );

        for( s++ ; *s == ' '; s++ ) ;                   /* skip over blanks */
        if( *s == '\'' || *s == '\"' )          /* get string delimiter */
                t = *s++;
        else
                t = ' ';
        for( v=ValStr;                          /* copy value portion up to delim */
             *s != t && *s != '\0';
             *v++ = *s++ ) ;
        if( t != ' ' && *s == t ) s++;
        *v = '\0';
        kw->Val = ValStr;
        kw->vt = String;

        return( s );
}


/*-->GetKeyVal */
/********************************************************************* */
/*****************************  GetKeyVal  *************************** */
/********************************************************************* */

        /* get next keyword-value pair
         * decode value according to table entry
 */

int GetKeyVal( kw, tab, nt, tno)
KeyWord *kw; 
KeyDesc tab[];
int     nt;
int     *tno;
{
        int i;
        char c = '\0';

        *tno = -1;

        for(i=0; i<nt; i++)
                if( IsSame(kw->Key, tab[i].Entry) ) {
                        *tno = i;
                        switch( tab[i].Type ) {
                                case None: 
                                        if( kw->vt != None ) return( FALSE );
                                        break;
                                case String:
                                        if( kw->vt != String ) return( FALSE );
                                        break;
                                case Integer:
                                        if( kw->vt != String ) return( FALSE );
                                        if( sscanf(kw->Val,"%d%c", &(kw->v.i), &c) != 1
                                            || c != '\0' ) return( FALSE );
                                        break;
                                case Number:
                                case Dimension:
                                        if( kw->vt != String ) return( FALSE );
                                        if( sscanf(kw->Val,"%f%c", &(kw->v.n), &c) != 1
                                            || c != '\0' ) return( FALSE );
                                        break;
                                }
                        kw->vt = tab[i].Type;
                        return( TRUE );
                        }

        return( TRUE );
}


/*-->HasBeenRead */
/********************************************************************* */
/***************************  HasBeenRead  *************************** */
/********************************************************************* */

int
HasBeenRead(k)
int k;
{
    struct font_entry *ptr;

    ptr = hfontptr;
    while ((ptr!=NULL) && (ptr->k!=k))
	ptr = ptr->next;
    return( ptr != NULL );
}


/*-->IsSame */
/********************************************************************* */
/*******************************  IsSame  **************************** */
/********************************************************************* */

int IsSame(a, b)        /* compare strings, ignore case */
char *a, *b;
{
        for( ; *a != '\0'; ++a,++b)
                if( (isupper(*a) ? tolower(*a) : *a)
		  != (isupper(*b) ? tolower(*b) : *b) ) return( FALSE );
        return( *a == *b ? TRUE : FALSE );
}


/*-->lcase */
/********************************************************************* */
/****************************  lcase  ******************************** */
/********************************************************************* */

void lcase(s)
char *s;
{
        char *t;
        for(t=s; *t != '\0'; t++)
	  *t = (isupper(*t) ? tolower(*t) : *t);
        return;
}


/*-->MoveDown */
/********************************************************************* */
/****************************  MoveDown  ***************************** */
/********************************************************************* */

void
MoveDown(a)
int a;
{
    v += a;
}


/*-->MoveOver */
/********************************************************************* */
/****************************  MoveOver  ***************************** */
/********************************************************************* */

void
MoveOver(b)
int b;
{
    h += b;
}


/*-->NoSignExtend */
/********************************************************************* */
/***************************  NoSignExtend  ************************** */
/********************************************************************* */

int
NoSignExtend(fp, n)	/* return n byte quantity from file fd */
register FILE *fp;	/* file pointer */
register int n;		/* number of bytes */

{
    register int x;	/* number being constructed */

    x = 0;
    while (n--)  {
	x <<= 8;
	x |= getc(fp);
    }
    return(x);
}


/*-->OpenFontFile */
/********************************************************************* */
/************************** OpenFontFile  **************************** */
/********************************************************************* */

void
OpenFontFile()
/***********************************************************************
    The original version of this dvi driver reopened the font file  each
    time the font changed, resulting in an enormous number of relatively
    expensive file  openings.   This version  keeps  a cache  of  up  to
    MAXOPEN open files,  so that when  a font change  is made, the  file
    pointer, pxlfp, can  usually be  updated from the  cache.  When  the
    file is not found in  the cache, it must  be opened.  In this  case,
    the next empty slot  in the cache  is assigned, or  if the cache  is
    full, the least used font file is closed and its slot reassigned for
    the new file.  Identification of the least used file is based on the
    counts of the number  of times each file  has been "opened" by  this
    routine.  On return, the file pointer is always repositioned to  the
    beginning of the file.

********************************************************************** */
{
    register int i,least_used,current;

#ifdef DEBUG
    if (Debug) printf("Open Font file\n");
#endif
    if (pfontptr == fontptr)
        return;                 /* we need not have been called */

    for (current = 1;
	(current <= nopen) &&
	    (pixel_files[current].pixel_file_id != fontptr->font_file_id);
	++current)
	;                       /* try to find file in open list */

    if (current <= nopen)       /* file already open */
    {
	if( (pxlfp = pixel_files[current].pixel_file_id) != NO_FILE )
	        fseek(pxlfp,0L,0);	/* reposition to start of file */
    }
    else                        /* file not in open list */
    {
        if (nopen < MAXOPEN)    /* just add it to list */
            current = ++nopen;
	else                    /* list full -- find least used file, */
	{                       /* close it, and reuse slot for new file */
	    least_used = 1;
            for (i = 2; i <= MAXOPEN; ++i)
	        if (pixel_files[least_used].use_count >
                    pixel_files[i].use_count)
		    least_used = i;
            if (pixel_files[least_used].pixel_file_id != NO_FILE) {
                FILE *fid;
                struct font_entry *fp;

                fid = pixel_files[least_used].pixel_file_id;
                fp=hfontptr;    /* mark file as being closed in the entry */
                while (fp!=NULL && fp->font_file_id != fid) 
                        fp = fp->next;
                if (fp==NULL)
		  Fatal("Open file %x not found in font entry list.\n", fid);
                else {
                        fp->font_file_id = NULL;
#ifdef STATS
                        if (Stats)
                                fprintf(stderr, "PXL file %s closed.\n", fp->name);
#endif
                        }
 	        fclose( fid );
                }
	    current = least_used;
        }
        if ((pxlfp=BINARYOPEN(fontptr->name,"r")) == NULL) {
	    Warning("PXL file %s could not be opened",fontptr->name);
            pxlfp = NO_FILE;
            }
        else {
#ifdef STATS
            if (Stats) 
                fprintf(stderr, "PXL file %s opened.\n", fontptr->name);
#endif
            }
	pixel_files[current].pixel_file_id = pxlfp;
	pixel_files[current].use_count = 0;
    }
    pfontptr = fontptr;			/* make previous = current font */
    fontptr->font_file_id = pxlfp;	/* set file identifier */
    pixel_files[current].use_count++;	/* update reference count */
#ifndef USEPXL
    gfin = pxlfp;		        /* used in gf reader */
#endif ~USEPXL
}


/*-->PixRound */
/********************************************************************* */
/*****************************  PixRound  **************************** */
/********************************************************************* */

int
PixRound(x, conv)	/* return rounded number of pixels */
register int x;		/* in DVI units */
int conv;		/* conversion factor */
{
    return((int)((x + (conv >> 1)) / conv));
}


/*-->PutInt */
/********************************************************************* */
/*****************************  PutInt  ****************************** */
/********************************************************************* */

void
PutInt(n)               /* output an integer followed by a space */
register int n;
{
    char buf[10];
    register char *b;

    if( n == 0 )
        EMITC('0'); 
    else {
        if( n < 0 ) {
            EMITC('-');
            n = -n;
            }
    
        for(b=buf;  n>0;  ) {
            *b++ = digit[n%10];
            n /= 10;
            }
    
        for( ; b>buf; )
            EMITC(*--b);
        }

    EMITC(' ');
}


/*-->PutOct */
/********************************************************************* */
/*****************************  PutOct  ****************************** */
/********************************************************************* */

void
PutOct(n)               /* output an 3 digit octal number preceded by a "\" */
register int n;
{                  
    EMITC( '\\' ); 
    EMITC( digit[(n&0300)>>6] );
    EMITC( digit[(n&0070)>>3] ); 
    EMITC( digit[n&0007] );
}


/*-->ReadFontDef */
/********************************************************************* */
/****************************  ReadFontDef  ************************** */
/********************************************************************* */

int
ReadFontDef(k)
int k;
{
    int t, i;
    register struct font_entry *tfontptr;/* temporary font_entry pointer */
    register struct char_entry *tcharptr;/* temporary char_entry pointer */
    char *direct, *tcp, *tcp1;
    int found;
    char curarea[STRSIZE];
    int cmag;
    int nmag;
    char nname[128];

    if ((tfontptr = NEW(struct font_entry)) == NULL)
	Fatal("can't malloc space for font_entry");
    tfontptr->next = hfontptr;
    tfontptr->font_file_id = NULL;
    fontptr = hfontptr = tfontptr;

    tfontptr->ncdl = 0;
#ifdef STATS
    tfontptr->nbpxl = 0;
    tfontptr->ncts = 0;
#endif

    tfontptr->k = k;
    tfontptr->c = NoSignExtend(dvifp, 4); /* checksum */
    tfontptr->s = NoSignExtend(dvifp, 4); /* space size */
    tfontptr->d = NoSignExtend(dvifp, 4); /* design size */
    tfontptr->a = NoSignExtend(dvifp, 1); /* area length for font name */
    tfontptr->l = NoSignExtend(dvifp, 1); /* device length */
    GetBytes(dvifp, tfontptr->n, tfontptr->a+tfontptr->l);
    tfontptr->n[tfontptr->a+tfontptr->l] = '\0';
    tfontptr->font_space = tfontptr->s/6; /* never used */
    tfontptr->font_mag = (int)((ActualFactor((int)(((float)tfontptr->s/
    			(float)tfontptr->d)*1000.0 + 0.5)) * 
#ifdef USEGLOBALMAG
			ActualFactor(mag) *
#endif
#ifdef USEPXL
			(float)LWresolution * 5.0) + 0.5);
#else ~USEPXL
                        (float)LWresolution) + 0.5);
#endif ~USEPXL

#ifdef FONTPATHS
    if (!findfile(rgsbPXLPaths, csbPXLPaths, "", tfontptr->n,
	     tfontptr->font_mag, tfontptr->name, nname, &nmag))
#else ~FONTPATHS
    if (!findfile(&PXLpath,1,"",tfontptr->n,tfontptr->font_mag,tfontptr->name,
		  nname, &nmag))
#endif ~FONTPATHS
      Fatal("no font %s.%d",tfontptr->n,tfontptr->font_mag);
      
    sprintf(tfontptr->psname, "%s.%d", tfontptr->n, tfontptr->font_mag);
    if (tfontptr != pfontptr)
	OpenFontFile();
#ifdef USEPXL
    if ( pxlfp == NO_FILE ) {                /* allow missing pxl files */
        tfontptr->magnification = 0;
        tfontptr->designsize = 0;
#endif USEPXL
        for (i = FIRSTPXLCHAR; i <= LASTPXLCHAR; i++) {
	    tcharptr = &(tfontptr->ch[i]);
#ifdef USEPXL
	    tcharptr->width = 0;
	    tcharptr->height = 0;
	    tcharptr->xOffset= 0;
	    tcharptr->yOffset = 0;
#endif USEPXL
	    tcharptr->where.isloaded = FALSE;
	    tcharptr->where.address.fileOffset = NONEXISTANT;
	    tcharptr->tfmw = 0;
            }
#ifdef USEPXL
        return;
        }

    if ((t = NoSignExtend(pxlfp, 4)) != PXLID)
	Fatal("PXL ID = %d, can only process PXL ID = %d files",
	      t, PXLID);
    fseek(pxlfp, -20L, 2);
    t = NoSignExtend(pxlfp, 4);
    if ((tfontptr->c != 0) && (t != 0) && (tfontptr->c != t))
	Warning("font = \"%s\",\n-->font checksum = %d,\n-->dvi checksum = %d",
	        tfontptr->name, tfontptr->c, t);
    tfontptr->magnification = NoSignExtend(pxlfp, 4);
    tfontptr->designsize = NoSignExtend(pxlfp, 4);

    fseek(pxlfp, (long) NoSignExtend(pxlfp, 4) * 4, 0);

    for (i = FIRSTPXLCHAR; i <= LASTPXLCHAR; i++) {
	tcharptr = &(tfontptr->ch[i]);
	tcharptr->width = NoSignExtend(pxlfp, 2);
	tcharptr->height = NoSignExtend(pxlfp, 2);
	tcharptr->xOffset= SignExtend(pxlfp, 2);
	tcharptr->yOffset = SignExtend(pxlfp, 2);
	tcharptr->where.isloaded = FALSE;
	tcharptr->where.address.fileOffset = NoSignExtend(pxlfp, 4) * 4;
	tcharptr->tfmw = ((float)NoSignExtend(pxlfp, 4)*(float)tfontptr->s) /
	    (float)(1<<20);
#else ~USEPXL
    if ( pxlfp == NO_FILE )                /* allow missing pxl files */
        return;

    gfin = pxlfp;
    seekpost();
    readpost();
    if ((tfontptr->c != 0) && (checksum != 0) && (tfontptr->c != checksum))
	Warning("font = \"%s\",\n-->font checksum = %d,\n-->dvi checksum = %d",
	        tfontptr->name, tfontptr->c, checksum);

    for(i=FIRSTPXLCHAR; i<=LASTPXLCHAR; i++) {
	if (char_exists[i]) {
	    tcharptr = &(tfontptr->ch[i]);
	    tcharptr->tfmw = ((float)tfm_wd[i]*(float)tfontptr->s) /
	        (float)(1<<20);
	    tcharptr->where.address.fileOffset = char_pointer[i];
	}
#endif ~USEPXL
    }
}


/*-->ReadPostAmble */
/********************************************************************* */
/**************************  ReadPostAmble  ************************** */
/********************************************************************* */

void
ReadPostAmble(load)
int     load;
/***********************************************************************
    This  routine  is  used  to  read  in  the  postamble  values.    It
    initializes the magnification and checks  the stack height prior  to
    starting printing the document.
********************************************************************** */
{
#ifdef	MTR
    int	    npage;
#endif

    FindPostAmblePtr (&postambleptr);
    if (NoSignExtend(dvifp, 1) != POST)
	Fatal ("POST missing at head of postamble");
#ifdef DEBUG
    if (Debug) fprintf (stderr, "got POST command\n");
#endif
    ppagep = NoSignExtend(dvifp, 4);
    num = NoSignExtend(dvifp, 4);
    den = NoSignExtend(dvifp, 4);
    mag = NoSignExtend(dvifp, 4);
#ifdef USEGLOBALMAG
    if( usermag > 0 && usermag != mag )
        fprintf(stderr, "DVI magnification of %d over-ridden by user mag of %d\n", mag, usermag );
#endif
    if( usermag > 0 ) mag = usermag;
#ifndef USEGLOBALMAG
    if( mag != 1000 ) fprintf(stderr, "Magnification of %d ignored.\n", mag);
#endif
    hconv = DoConv(num, den, hconvRESOLUTION);
    vconv = DoConv(num, den, vconvRESOLUTION);

    NoSignExtend(dvifp, 4);	/* height-plus-depth of tallest page */
    NoSignExtend(dvifp, 4);	/* width of widest page */
    if (NoSignExtend(dvifp, 2) >= STACKSIZE)
	Fatal ("Stack size is too small");
#ifndef	MTR
    NoSignExtend(dvifp, 2);	/* this reads the number of pages in */
    /* the DVI file */
#else
    npage = NoSignExtend (dvifp, 2);
    if (G_bflag) {
	register int	j;
	
	npage += G_increment - 1;
        for (npage /= G_increment, j = 10; npage >= j; j *= 10)
		G_bwidth++;
    }
#endif
#ifdef DEBUG
    if (Debug) fprintf (stderr, "now reading font defs");
#endif
    if (load) GetFontDef ();
}


/*-->SetChar */
/********************************************************************* */
/*****************************  SetChar  ***************************** */
/********************************************************************* */

int buffer[8];

LoadAChar(c, ptr)
int c;
register struct char_entry *ptr;
{
    long *pr;
    register int nints;

    if (ptr->where.address.fileOffset == NONEXISTANT) {
	ptr->where.address.pixptr = NULL;
        ptr->where.isloaded = TRUE;
	return;
        }

    OpenFontFile();
#ifdef USEPXL
    fseek(pxlfp, (long) ptr->where.address.fileOffset, 0);
    nints = ((ptr->width + 31) >> 5) * ptr->height;
    if( (pr = (long *)malloc( nints*sizeof(long) )) == NULL )
        Fatal("Unable to allocate memory for char\n");
    fread(pr, 4, nints, pxlfp);
    ptr->where.address.pixptr = pr;
#else ~USEPXL
    fseek(gfin, ptr->where.address.fileOffset, 0);
    gettochar();
    readbits();
#endif ~USEPXL
    ptr->where.isloaded = TRUE;

    EmitChar(c, ptr);
#ifdef USEPXL
        /* we should really free the space used by the PXL data after this
           point, but it is not large, and besides, we may want to be
           more clever in the future, about sending bitmaps.  So keep
           the data around */
#endif USEPXL
}

void
SetChar(c, command, PassNo)
int c, command, PassNo;
{
    register struct char_entry *ptr;  /* temporary char_entry pointer */

    ptr = &(fontptr->ch[c]);
    if (!ptr->where.isloaded) LoadAChar(c, ptr);
    if (PassNo==0) return;

    SetPosn(h,v);
    if (fontptr->font_file_id != NO_FILE) {      /* ignore missing fonts */
        EMITN(c); EMITS("c\n");
        hh += ptr->tfmw;
        }

    if (command <= SET4)
        h += ptr->tfmw;

#ifdef STATS
    Stnc += 1;
    fontptr->ncts += 1;
#endif
}


/*-->SetFntNum */
/********************************************************************* */
/****************************  SetFntNum  **************************** */
/********************************************************************* */

void
SetFntNum(k, Emitting)
int k, Emitting;

/*  this routine is used to specify the font to be used in printing future
    characters */

{
    fontptr = hfontptr;
    while ((fontptr!=NULL) && (fontptr->k!=k))
	fontptr = fontptr->next;
    if (fontptr == NULL)
	Fatal("font %d undefined", k);
    if (Emitting && (fontptr->font_file_id != NO_FILE) ) 
        EMIT(outfp,"(%s) @sf\n", fontptr->psname);
}


/*-->SetPosn */
/********************************************************************* */
/*****************************  SetPosn  ***************************** */
/********************************************************************* */

void
SetPosn(x, y)           /* output a positioning command */
int x, y;
{
        int rx,ry;

#ifdef USERELPOS
        if ( y == vv) {  /* use relative movement if just moving horizontally */
            if ( (x != hh)
		 && (rx=PixRound(x-hh,hconv)) ) { /* if movement non-zero */
                EMITN(rx);
                EMITS("r ");
		hh += rx*hconv;
                }
            }
        else {
#endif
            EMITN(rx=PixRound(x,hconv));
            EMITN(ry=PixRound(y,vconv));
            EMITS("p ");
	    hh = rx*hconv;
	    vv = y;
#ifdef USERELPOS
            }
#endif
}


/*-->SetRule */
/********************************************************************* */
/*****************************  SetRule  ***************************** */
/********************************************************************* */

void
SetRule(a, b, Set)
int a, b;
BOOLEAN Set;

{	    /*	 this routine will draw a rule */

    if( a > 0 && b > 0 ) {
        SetPosn(h,v);                   /* lower left corner */
        EMITN(PixRound(b,hconv));       /* width */
        EMITN(PixRound(a,vconv));       /* height */
        EMITS("ru\n");
        }
    if (Set)
	h += b;
}


/*-->SetString */
/********************************************************************* */
/*****************************  SetString  *************************** */
/********************************************************************* */

void
SetString(firstch, PassNo)              /* read and set a consecutive string of chars */
int firstch, PassNo;
{
register int lastchar, c, len;
register struct char_entry *ptr;

    len = 2;
    if (PassNo != 0) SetPosn(h,v);

    for (lastchar = firstch, c = NoSignExtend(dvifp, 1);
	    c >= SETC_000 && c <= SETC_127;
	    len++, lastchar = c, c = NoSignExtend(dvifp, 1)) {

	/* make sure fonts are loaded */
	ptr = &(fontptr->ch[lastchar]);
	if ( !(ptr->where.isloaded) ) LoadAChar(lastchar, ptr);

	if (PassNo != 0) {
	    if (fontptr->font_file_id != NO_FILE) { /* ignore unavail fonts */
		if (len <= 2) EMITC('(');	/* just the first time */
		switch (lastchar) {
		    /* need to escape the following three chars */
		    case '(':
		    case ')':
		    case '\\':
			EMITC('\\');
			EMITC(lastchar);
			break;
		    default:
			if (lastchar < ' ' || lastchar >= 0177)
			    EMITO(lastchar);
			else
			    EMITC(lastchar);
		    }	/* end switch */
		hh += ptr->tfmw;
		}		/* end !NO_FILE */
	    h += ptr->tfmw;		/* update this even if no fontfile */
	    }		/* end PassNo != 0 */
	} /* end for */

    /* backup one char */
    len --;
    fseek(dvifp, -1L, 1);

    /* handle the last char */
    /* mostly duplicated code --- yuck !! */

    ptr = &(fontptr->ch[lastchar]);
    if ( !(ptr->where.isloaded) ) LoadAChar(lastchar, ptr);

    if (PassNo != 0) {
        if (fontptr->font_file_id != NO_FILE) {
	    if (len == 1) {
		EMITN(firstch);
		EMITS("c\n");
		}
	    else {		/* len != 1 */
		switch (lastchar) {
		    /* need to escape the following three chars */
		    case '(':
		    case ')':
		    case '\\':
			EMITC('\\');
			EMITC(lastchar);
			break;
		    default:
			    if (lastchar < ' ' || lastchar >= 0177)
				EMITO(lastchar);
			    else
				EMITC(lastchar);
		    }	/* end switch */
		EMITS(") s\n");		/* end of string */
		}	/* end len != 1 */
	    hh += ptr->tfmw;
	    }			/* end !NO_FILE */
	h += ptr->tfmw;		/* update even if no fontfile */
	}		/* end PassNo != 0 */

#ifdef STATS
    if (PassNo != 0) {
	Stnc += len;
	fontptr->ncts += len;
    }
#endif STATS

}	/* end SetString */

/*-->SignExtend */
/********************************************************************* */
/****************************  SignExtend  *************************** */
/********************************************************************* */

int
SignExtend(fp, n)   /* return n byte quantity from file fd */
register FILE *fp;  /* file pointer */
register int n;     /* number of bytes */

{
    int n1;         /* number of bytes	 */
    register int x; /* number being constructed */

    x = getc(fp);   /* get first (high-order) byte */
    n1 = n--;
    while (n--)  {
	x <<= 8;
	x |= getc(fp);
    }

    /* NOTE: This code assumes that the right-shift is an arithmetic, rather
    than logical, shift which will propagate the sign bit right.   According
    to Kernighan and Ritchie, this is compiler dependent! */

    x<<=32-8*n1;
    x>>=32-8*n1;  /* sign extend */

#ifdef DEBUG
    if (Debug)
    {
	fprintf(stderr,"\tSignExtend(fp,%d)=%X\n",n1,x);
    }
#endif
    return(x);
}


/*-->SkipFontDef */
/********************************************************************* */
/****************************  SkipFontDef  ************************** */
/********************************************************************* */

void
SkipFontDef(k)
int k;
{
    int a, l;
    char n[STRSIZE];

    NoSignExtend(dvifp, 4);
    NoSignExtend(dvifp, 4);
    NoSignExtend(dvifp, 4);
    a = NoSignExtend(dvifp, 1);
    l = NoSignExtend(dvifp, 1);
    GetBytes(dvifp, n, a+l);
}


/*-->Warning */
/********************************************************************* */
/*****************************  Warning  ***************************** */
/********************************************************************* */

void
Warning(fmt, a, b, c)  /* issue a warning */
char *fmt;	/* format */
char *a, *b, *c;	/* arguments */
{
    if (G_logging == 0)
    {
        if (G_logfile) /* if messages are going to a log file */
	        G_logfp=fopen(G_Logname,"w+");
        else {
                G_logfp=stderr;
                if( G_nowarn ) return;
                }
	G_logging = 1;
	if (G_logfp == NULL) G_logging = -1;
    }

    G_errenc = TRUE;
    if (G_logging == 1)
    {
	fprintf(G_logfp, fmt, a, b, c);
	fprintf(G_logfp,"\n");
    }
}


#ifndef USEPXL
void bad_gf(n)
	int n;
{
	Fatal("Bad gf file, case %d\n",n);	/* See gf.c */
}
#endif ~USEPXL

#ifdef FONTPATHS
/******
** SeparatePaths -- takes a CHPATHSEP separated path list sbPathVar, replaces
**	the CHPATHSEPs with NULLs and sets rgsbPaths[i] to the beginning of
**	the ith path in sbPathVar.  The number of paths is returned.
******/
  
SeparatePaths(sbPathVar, rgsbPaths)
char	*sbPathVar, *rgsbPaths[];
{
    char	*pch;
    int		csbPaths = 0;

    rgsbPaths[csbPaths++] = sbPathVar;
    while (sbPathVar = index(sbPathVar, CHPATHSEP)) {
	*sbPathVar++ = NULL;
        rgsbPaths[csbPaths++] = sbPathVar;
    }
    return(csbPaths);
}
#endif FONTPATHS
