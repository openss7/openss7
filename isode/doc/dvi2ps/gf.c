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

/*
 *			G F . C 
 *
 * $Revision: 1.2 $
 *
 * Log:	gf.c,v
 * Revision 1.2  86/09/04  16:25:17  dorab
 * Added distinctive RCS header
 * 
 */
#ifndef lint
static char RCSid[] = "@(#)Header: gf.c,v 1.2 86/09/04 16:25:17 dorab UCLA (UCLA)";
#endif

#ifndef USEPXL
#include	<stdio.h>

#define false	0
#define true	1

/* This file contains routines for reading character descriptions from
   GF files (the output of Metafont).  The following functions are defined:
   gettochar()	finds the next character, returns its code (reduced modulo
		256 if necessary), and sets global variables min_m, max_m,
		min_n, max_n.  A result of -1 indicates that there are no
		more characters and the postamble has been found.
   readbits()	After a character is found, this routine fills array bits[]
		with the character image.  The image is represented in
		Postscript format: the bits are packed into bytes
		(most significant bit first), and the bytes scan the image
		left-to-right, bottom-to-top.
		Set globals num_cols, num_rows, x_offset, y_offset
		(the latter give the offset of the origin from the upper
		bottom corner of the image, up being a positive y_offset).
   readpost()	After the postamble is found, this routine reads it and
		sets the remaining global variables declared below.
   seekpost()	rapidly finds the postamble by random accessing the file.
   seekchar(c)	uses fseek() to find a character with code c (modulo 256),
		returning a nonzero result if successful.  Global variables
		are set as they are by gettochar.  GF files may contain
		many characters with the same code, in which case subsequent
		calls to seekchar(c) with the same c will find the other
		versions.  This routine should only be called after readpost().
   No random access is attempted if seekpost() and seekchar() are not used.
*/

#define int32	long		/* used when at least 32 bits are required */

int min_m, max_m, min_n, max_n;		/* bounding box for character */
int32 charfam;				/* high order bytes of character code */

int font_min_m, font_max_m, font_min_n, font_max_n;	/* overall bounds */
int32 designsize;			/* font size in points scaled by 1<<20 */
int32 checksum;				/* should match TFM file and DVI file */
int32 hppp, vppp;			/* horizontal and vertical pixels/point scaled 1<<16 */
int char_wd[256];			/* character width in pixels, rounded if necessary */
int32 tfm_wd[256];			/* width as a fraction of designsize scaled 1<<20 */
char char_exists[256];			/* nonzero entries indicate presence of a char_loc */
int gf_bc, gf_ec;			/* first and last nonzero char_exists entries */

int32 char_pointer[256];		/* used by seekchar() */
int32 backpointer;			/* set by gettochar() for use by seekchar() */

void gf_beginc();			/* prepare to paint white at (min_m,max_n) */
void gf_paint();			/* paint d pixels and switch colors */
void gf_skip();				/* finish a row and skip d rows */
void gf_endc();				/* finish the last row */

extern void bad_gf();			/* aborts the program if the gf file is invalid */
FILE *gfin = stdin;			/* change this externally if necessary */

/* The following macros describe gf file format */

#define paint_0		0
#define last_paint	63
#define paint1		64
#define paint2		65
#define paint3		66
#define boc		67
#define boc1		68
#define eoc		69
#define skip0		70
#define skip1		71
#define skip2		72
#define skip3		73
#define new_row_0	74
#define last_new_row	238
#define xxx1		239
#define xxx2		240
#define xxx3		241
#define xxx4		242
#define yyy		243
#define no_op		244
#define char_loc	245
#define char_loc0	246
#define pre		247
#define post		248
#define postpost	249
#define undefined_cases	250: case 251: case 252: case 253: case 254: case 255
#define gf_version	131

/* All gf file input goes through the following routines */

#define getbyte()	(getc(gfin)&0xff)
/* int getbyte()
{
	int b;
	b=getc(gfin)&0xff;
	printf("Getting %d\n",b);
	return b;
} */

#define skip1byte()	getc(gfin)

int
get2bytes()
{
	register int ans;

	ans = getbyte() << 8;
	ans += getbyte();
	return ans;
}

int32
get3bytes()
{
	register int32 ans;

	ans = getbyte() << 16;
	ans += getbyte() << 8;
	ans += getbyte();
	return ans;
}

int32
get4bytes()
{
	register int32 ans;

	ans = getbyte() << 24;
	ans += getbyte() << 16;
	ans += getbyte() << 8;
	ans += getbyte();
	return ans;
}

void
skip_bytes(n)
	int n;
{
	while (n-- > 0)
		skip1byte();
}

/* In the unlikely event of a character code outside of the range 0..255,
   the high order bytes are placed in the global variable charfam.
   If no boc command is encountered, the result is -1 and the last byte read
   is guaranteed to be the post command.
*/
int
gettochar()
{
	int32 c;			/* the character code to be returned */
	register int x;			/* temporary */

	for (;;)
		switch (getbyte()) {
		case yyy:
			skip1byte();	/* intended to fall through 3 times */
		case paint3:
		case skip3:
			skip1byte();
		case paint2:
		case skip2:
			skip1byte();
		case paint1:
		case skip1:
			skip1byte();
			continue;

		case boc:
			c = get4bytes();
			backpointer = get4bytes();
			min_m = get4bytes();
			max_m = get4bytes();
			min_n = get4bytes();
			max_n = get4bytes();
			charfam = c < 0 ? -((-c) >> 8) : c >> 8;
			return c & 0377;

		case boc1:
			c = getbyte();
			backpointer = -1;
			x = getbyte();	/* del_m */
			max_m = getbyte();
			min_m = max_m - x;
			x = getbyte();	/* del_n */
			max_n = getbyte();
			min_n = max_n - x;
			return c;

		case pre:
			if (getbyte() != gf_version)
				bad_gf(1);
			skip_bytes(getbyte());
			continue;

		case xxx1:
			skip_bytes(getbyte());
			continue;
		case xxx2:
			skip_bytes(get2bytes());
			continue;
		case xxx3:
			skip_bytes((int) get3bytes());
			continue;
		case xxx4:
			skip_bytes((int) get4bytes());
			continue;

		case post:
			return -1;

		case char_loc:
		case char_loc0:
		case postpost:
		case undefined_cases:
			bad_gf(2);

		default:	/* do nothing */ ;
		}
}

/* readbits reads a raster description from the gf file and uses the external
   routines to actually process the raster information.
*/
void
readbits()
{
	register unsigned char b;

	gf_beginc();
	for (;;) {
		b = getbyte();
		if (b <= last_paint)
			gf_paint(b - paint_0);
		if (b < new_row_0)
			switch (b) {
			case paint1:
				gf_paint(getbyte());
				continue;
			case paint2:
				gf_paint(get2bytes());
				continue;
			case paint3:
				gf_paint((int) get3bytes());
				continue;
			case boc:
			case boc1:
				bad_gf(3);
			case eoc:
				goto finish;
			case skip0:
				gf_skip(0);
				continue;
			case skip1:
				gf_skip(getbyte());
				continue;
			case skip2:
				gf_skip(get2bytes());
				continue;
			case skip3:
				gf_skip((int) get3bytes());
				continue;
		} else if (b <= last_new_row) {
			gf_skip(0);
			gf_paint(b - new_row_0);
		} else
			switch (b) {
			case xxx1:
				skip_bytes(getbyte());
				continue;
			case xxx2:
				skip_bytes(get2bytes());
				continue;
			case xxx3:
				skip_bytes((int) get3bytes());
				continue;
			case xxx4:
				skip_bytes((int) get4bytes());
				continue;
			case yyy:
				get4bytes();
				continue;
			case no_op:
				continue;
			default:
				bad_gf(4);
			}
	}
      finish:gf_endc();
}

void
readpost()
{
	int i, b, c;

	get4bytes();		/* ignore back pointer to font-wide xxx commands */
	designsize = get4bytes();
	checksum = get4bytes();
	hppp = get4bytes();
	vppp = get4bytes();
	font_min_m = get4bytes();
	font_max_m = get4bytes();
	font_min_n = get4bytes();
	font_max_n = get4bytes();

	for (i = 0; i < 256; i++) {
		char_exists[i] = false;
		char_wd[i] = 0;
		tfm_wd[i] = 0;
		char_pointer[i] = -1;
	}
	gf_bc = 255;
	gf_ec = 0;
	for (;;) {
		b = getbyte();
		c = getbyte();
		if (b == char_loc0)
			char_wd[c] = getbyte();
		else if (b == char_loc) {
			char_wd[c] = (get4bytes() + 0100000) >> 16;
			get4bytes();	/* skip dy */
		} else
			break;
		tfm_wd[c] = get4bytes();
		char_pointer[c] = get4bytes();
		char_exists[c] = true;
		if (gf_bc > c)
			gf_bc = c;
		if (gf_ec < c)
			gf_ec = c;
	}
}

void
seekpost()
{
	int c;

	fseek(gfin, -5L, 2);	/* skip four 223's */
	do {
		c = getbyte();
		fseek(gfin, -2L, 1);
	} while (c == 223);
	if (c != gf_version)
		bad_gf(5);	/* check version number */
	fseek(gfin, -3L, 1);	/* back up to the pointer */
	if (fseek(gfin, (long) get4bytes(), 0) < 0)
		bad_gf(6);
	if (getbyte() != post)
		bad_gf(7);
}

int
seekchar(c)
	int c;
{
	if (c < 0 || c > 255 || char_pointer[c] < 0)
		return false;
	if (fseek(gfin, char_pointer[c], 0) < 0)
		bad_gf(8);
	if (gettochar() != c)
		bad_gf(9);
	char_pointer[c] = backpointer;
	return true;
}

#define BITBUFSIZE 20000
unsigned char bits[BITBUFSIZE];
int num_cols, num_rows, num_bytes, x_offset, y_offset;

#define row_start(n) (&(bits[(n-min_n)*rowbytes]))
#define m_byte(m) ((m-min_m)>>3)	/* byte within within a row containing bit m */
#define m_bit(m) (7-(m-min_m)&07)	/* bit within byte for a given m */

static int rowbytes, gf_m0, gf_m, gf_n, paint_switch;
static unsigned char *row_ptr;

void
gf_beginc()
{
	num_cols = max_m - min_m + 1;
	num_rows = max_n - min_n + 1;
	x_offset = -min_m;
	y_offset = -min_n;
	rowbytes = (num_cols + 7) / 8;
	num_bytes = num_rows * rowbytes;
	if (num_bytes >= BITBUFSIZE)
		bad_gf(10);	/* really, should allocate a bigger one */
	gf_m = gf_m0 = min_m;
	gf_n = max_n;
	row_ptr = row_start(max_n);
	bzero(bits, num_bytes);
	paint_switch = 0;
}

void
gf_paint(d)
	register int d;
{
	if (d > 0) {
		if (paint_switch)
			while (d-- > 0) {
				row_ptr[m_byte(gf_m)] |= 1 << m_bit(gf_m);
				gf_m++;
		} else
			gf_m += d;
	}
	paint_switch ^= 1;
}

void
gf_skip(d)
	int d;
{
	gf_n -= d + 1;
	gf_m = gf_m0;
	row_ptr = row_start(gf_n);
	paint_switch = 0;
}

void
gf_endc()
{				/* do nothing */
}
#endif
