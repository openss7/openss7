/*
 *			D V I 2 P S . H 
 *
 * $Revision: 1.4 $
 *
 * $Log:	dvi2ps.h,v $
 * Revision 1.4  86/10/01  18:16:30  dorab
 * made changes to allow multiple directories in TEXFONTS variable.
 * ifdeffed with FONTPATHS
 * 
 * Revision 1.3  86/04/29  23:00:18  dorab
 * first general release
 * 
 * Revision 1.2  86/04/29  13:26:17  dorab
 * Added distinctive RCS header
 * 
 */

/* this is dvi2ps.h */

#define VERSION "2.11"

typedef int BOOLEAN;
#define NEW(A) ((A *) malloc(sizeof(A)))
#define DEBUG   1			/* for massive printing of input */
					/* trace information; select by -d */
					/* option after filename: */
					/* dviview filename -d */
#define STATS		/* to enable statistics reporting via -s option */

#define BINARYOPEN fopen		/* byte-oriented host version */

#define ARITHRSHIFT 1                   /* define if ">>" operator is a */
                                        /* sign-propagating arithmetic  */
                                        /*   right shift */
#define USEGLOBALMAG 1			/* when defined, the dvi global */
   					/*   magnification is applied   */
      
/* We can leave USEGLOBALMAG undefined when we have a limited
   number of font magnifications (at 300dpi) available.  Otherwise, we
   will simply complain about missing PXL files
 */

/* #undef USEGLOBALMAG */

                        /* define for "optimal" relative postioning, rather
                           than absolute.  Relative can reduce size of postcript
                           output 20% (and reduce print time by almost as much */
#define USERELPOS 1 

#define FONTPATHS 1	/* turn on code for multiple paths in the
			   TEXFONTS env variable */

#define  DVIFORMAT        2
#define  TRUE             1
#define  FALSE            0
#define  FIRSTPXLCHAR     0
#define  LASTPXLCHAR    127

#ifndef FONTAREA
#define  FONTAREA         "/usr/lib/tex/fonts"
#endif

#ifndef HDRFILE
#define HDRFILE         "/usr/lib/tex.ps"
#endif


#ifdef apollo
#define  MAXOPEN         45  /* limit on number of open PXL files */
#else !apollo
#define  MAXOPEN         12  /* limit on number of open PXL files */
#endif

#define  NPXLCHARS      128
#define  PXLID         1001
#define  READ             4  /* for access() */
#define  LWresolution   300  /* only used for font file access */
/* convRESOLUTION is the number of PostScript units per inch that
tex.ps sets up the LW for. this number should equal "Resolution" in tex.ps */
#define  convRESOLUTION  300
/* there is a strong assumption that the horiz and vert resolution
is the same for use with TPIC - since otherwise circles will come
out like ellipses. if you do not define TPIC, then there is no problem */
#define  hconvRESOLUTION convRESOLUTION
#define  vconvRESOLUTION convRESOLUTION
#define  STACKSIZE      100
#define  STRSIZE        257
#define  NONEXISTANT     -1   /* offset for PXL files not found */
#define  NO_FILE        (FILE *)-1

#ifdef FONTPATHS
#define MAXFONTPATHS	16	/* max paths in TEXFONTS environment var */
#define	CHPATHSEP	':'	/* path seperator character */
#endif FONTPATHS

#define EQ(a,b) (strcmp(a,b)==0)

                        /* output a formatted string */
#define EMIT      fprintf
                        /* output a simple string */
#define EMITS(s)  fputs(s,outfp)
                        /* output an escaped octal number */
#define EMITO(c)  PutOct(c)
                        /* output a decimal integer */
#define EMITN(n)  PutInt(n)
                        /* output a byte value in Hex */
#define EMITH(h)  putc(*(digit+((h>>4)&0xF)),outfp),\
                  putc(*(digit+(h&0xF)),outfp)
                        /* output a single character */
#define EMITC(c)  putc(c,outfp)
                        /* output a scaled X dimension */
#define EMITX(x)  PutInt(PixRound(x,hconv))
                        /* output a scaled Y dimension */
#define EMITY(y)  PutInt(PixRound(y,vconv))

