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

/* e_main.c - make the encoding routines into a stand alone program */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/photo/RCS/e_main.c,v 9.0 1992/06/16 12:43:35 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/others/quipu/photo/RCS/e_main.c,v 9.0 1992/06/16 12:43:35 isode Rel
 *
 *
 * Log: e_main.c,v
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
#include <pixrect/pixrect_hs.h>

struct pixrect *pr_load();

extern int PIC_LINESIZE;
extern int STOP;
extern int NUMLINES;
extern int optlen;

/* ROUTINE: main
/*
/* DESCRIPTION:  Interprets the command line parameters then calls the
/* encoding routine. The format of the command line is as follows:-
/*
/*     encode [-K] infile outfile
/*
/*     -K is the K parameter default 1
/*     if infile or outfile omitted then stdin or stdout is used.
*/
char *encode_t4();

main(argc, argv)
	int argc;
	char **argv;

{
	int k_param = 1;
	int length;
	char *inbuf;
	char *outbuf;
	FILE *fptr;
	struct pixrect *pix;
	struct mpr_data *src_mpr;
	int skip;

	argv++;

	if ((argc > 1) && (**argv == '-')) {
		switch (*++*argv) {
		case '1':
			k_param = 1;
			break;
		case '2':
			k_param = 2;
			break;
		case '4':
			k_param = 4;
			break;
		case 'n':
			k_param = 1;
			break;
		case 'l':
			k_param = 2;
			break;
		case 'h':
			k_param = 4;
			break;
		case 'v':
			k_param = 32767;
			break;
		default:
			(void) fprintf(stderr, "Usage: %s -[124nlhv] \n", argv[0]);
			exit(-1);
		}
		argv++;
		argc--;
	}

	if ((pix = pr_load(stdin, NULL)) == (struct pixrect *) NULL)
		(void) fprintf(stderr, "Not a pixrect.\n");

	PIC_LINESIZE = pix->pr_size.x;
	STOP = PIC_LINESIZE + 1;
	NUMLINES = pix->pr_size.y;

	src_mpr = (struct mpr_data *) (pix->pr_data);
	inbuf = (char *) src_mpr->md_image;

	skip = 16 - (PIC_LINESIZE % 16);
	if (skip == 16)
		skip = 0;

	outbuf = encode_t4(k_param, inbuf, skip);

	fwrite(outbuf, optlen, 1, stdout);

}
