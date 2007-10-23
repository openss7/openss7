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

/* t4014.c - display on tetronix 4014 terminals */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/photo/RCS/t4014.c,v 9.0 1992/06/16 12:43:35 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/others/quipu/photo/RCS/t4014.c,v 9.0 1992/06/16 12:43:35 isode Rel
 *
 *
 * Log: t4014.c,v
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
#include "manifest.h"
#include <signal.h>

extern int NUMLINES, PIC_LINESIZE;
extern unsigned position;

/* Any errors should be written to *stdout* */
/* if the process exits, with out giving an error message, quipu may hang */

#define SCALE 5
#define Y_OFFSET 2700
#define X_OFFSET 3000
#define X_SKIP 8

int y = Y_OFFSET;

SFD
photo_quit()
{
	putch(030);		/* Return to non-graphic mode */
	exit(0);
}

/* ARGSUSED */
photo_start(name)
	char *name;
{
	putch(035);		/* Enter graphic mode */

	openpl();
	erase();
	linemod("solid");

	(void) signal(SIGTERM, photo_quit);
	/* return 0 if sucessful -1 if not */

	return (0);
}

/* ARGSUSED */
photo_end(name)
	char *name;
{
	/* Decoding has finished - display the photo */
	move(0, Y_OFFSET - 100);
	closepl();

	(void) printf("\n");
	(void) fflush(stdout);
	(void) close(1);	/* this is needed for QUIPU */
	/* wait until signalled to Terminate */
	for (;;) ;
}

/* ARGSUSED */
photo_black(length)
	int length;
{
	;
}

photo_white(length)
	int length;
{
	line((position * SCALE) + X_OFFSET, y, ((length + position - 1) * SCALE) + X_OFFSET, y);
}

/* ARGSUSED */
photo_line_end(line)
	bit_string *line;
{
	y -= SCALE;
}
