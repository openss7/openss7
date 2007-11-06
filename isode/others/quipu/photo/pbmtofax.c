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

/* pbmtofax.c - pbm to FAX filter */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/photo/RCS/pbmtofax.c,v 9.0 1992/06/16 12:43:35 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/others/quipu/photo/RCS/pbmtofax.c,v 9.0 1992/06/16 12:43:35 isode Rel
 *
 *
 * Log: pbmtofax.c,v
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
#include "pbm.h"
#include "general.h"

/*    DATA */

int PIC_LINESIZE, STOP, NUMLINES;

extern int optlen;

char *encode_t4();

/*
 *   G3-Fax nonbasic parameters.
 */

extern int twoDimensional;
extern int fineResolution;
extern int unlimitedLength;
extern int b4Length;
extern int a3Width;
extern int b4Width;
extern int uncompressed;
extern int nopreamble;
extern int oldformat;
extern int standardwidth;

/*    MAIN */

/* ARGSUSED */

main(argc, argv, envp)
	int argc;
	char **argv, **envp;
{
	int bitcount;
	bit *bitrow;
	register bit *bP;
	unsigned char *byteP;
	register int i;
	register int j;
	int cols, format, rows, skip;
	char *cp, *data, *file, *optbuf;
	bit black;
	FILE *fp;

	black = PBM_BLACK;

	file = NULL;
	fp = stdin;

	for (argv++; cp = *argv; argv++) {
		if (*cp == '-') {
			if (cp[1] == NULL)
				continue;

			else if (strcmp(cp, "-reversebits") == 0)
				black = PBM_WHITE;

			else if (strcmp(cp, "-2d") == 0)
				twoDimensional = 1;

			else if (strcmp(cp, "-cr") == 0)
				fineResolution = 0;

			else if (strcmp(cp, "-ul") == 0)
				unlimitedLength = 1;

			else if (strcmp(cp, "-b4l") == 0)
				b4Length = 1;

			else if (strcmp(cp, "-a3w") == 0)
				a3Width = 1;

			else if (strcmp(cp, "-b4w") == 0)
				b4Width = 1;

			else if (strcmp(cp, "-uc") == 0)
				uncompressed = 1;

			else if (strcmp(cp, "-sw") == 0)
				standardwidth = 1;

			else if (strcmp(cp, "-old") == 0) {
				oldformat = 1;
				twoDimensional = 1;
			}

			else if (strcmp(cp, "-nopreamble") == 0)
				nopreamble = 1;

			else
				goto usage;
		} else if (file) {
		      usage:;
			(void)
			    fputs("usage: pbmtofax [-2d] [-cr] [-ul] [-b4l] [-a3w] [-b4w] [-uc]\n",
				  stderr);
			(void) fputs("                [-old] [-nopreamble] [file]\n", stderr);
			(void) fputc('\n', stderr);
			(void) fputs("  -2d  select two dimensional encoding mode\n", stderr);
			(void) fputs("  -cr  set coarse resolution indication\n", stderr);
			(void) fputs("  -ul  set unlimited image length indication\n", stderr);
			(void) fputs("  -b4l set B4 length indication\n", stderr);
			(void) fputs("  -a3w set A3 width indication\n", stderr);
			(void) fputs("  -b4w set B4 width indication\n", stderr);
			(void) fputs("  -sw  force standard width (1728 pels)\n", stderr);
			(void) fputs("  -uc  set uncompressed mode indication\n", stderr);
			(void) fputs("  -old encode fax using old photo format\n", stderr);
			(void) fputs("  -nopreamble   encode fax without any preamble info\n",
				     stderr);
			(void) fputs("  -reversebits  create an inverse image\n", stderr);
			exit(1);
		} else
			file = cp;
	}

	if (file) {
		fp = pm_openr(file);
		if (fp == NULL) {
			perror(file);
			exit(1);
		}
	} else
		file = "<stdin>";

	pbm_readpbminit(fp, &cols, &rows, &format);
	bitrow = pbm_allocrow(cols);

	data = malloc((unsigned) (cols * rows));
	byteP = (unsigned char *) data;

	for (i = rows; i-- > 0;) {
		pbm_readpbmrow(fp, bP = bitrow, cols, format);
		*byteP = NULL, bitcount = 7;

		for (j = cols; j-- > 0;) {
			unsigned char mask = 1 << bitcount;

			if (*bP++ == black)
				*byteP |= mask;
			else
				*byteP &= ~mask;
			if (--bitcount < 0)
				*++byteP = NULL, bitcount = 7;
		}
		if (bitcount != 7)
			byteP++;
	}

	pm_close(fp);

	STOP = (PIC_LINESIZE = cols) + 1;
	NUMLINES = rows;
	if ((skip = 8 - (PIC_LINESIZE % 8)) == 8)
		skip = 0;

	optbuf = encode_t4(twoDimensional ? 4 : 1, data, skip);

	(void) fwrite(optbuf, optlen, sizeof *optbuf, stdout);

	exit(0);
}
