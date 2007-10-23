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

/* interface.c - photo bit manipulation utility routines */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/photo_util.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/common/RCS/photo_util.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: photo_util.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
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

/* This file contains utility routines used by both the                 */
/* encoding and decoding programs.                                      */
/* The routines are concerned with getting and setting bits of          */
/* a bit string.                                                        */

/* All these routine work in basically the same way.
/* a mask is used to get at each individual bit within
/* a byte.  Each time the next bit is required, the
/* mask is shifted right, when the mask is zero, the byte is either
/* written to the file, or the next byte read in depending upon the
/* routine.
*/

int PIC_LINESIZE, STOP, NUMLINES;

/* ROUTINE:     Get_bit                                                 */
/*                                                                      */
/* SYNOPSIS:    Gets the next bit from the input.                       */
/*              Returns 0 if it is a zero.                              */
/*              Returns 1 if it is a one                                */
char
get_bit(lineptr)

	bit_string *lineptr;		/* the line to get the bit from */

{
	unsigned char result;

	/* Anding the mask and the data gives a 0 if the bit masked is 0, 1 otherwis e */
	result = lineptr->mask & lineptr->pos;

	lineptr->mask >>= 1;

	if (lineptr->mask == 0) {
		lineptr->pos = *lineptr->dbuf++;
		lineptr->mask = BIT_MASK;
	}

	if (result != 0)	/* may not be 1, may be 0001000 for example */
		result = 1;

	return ((char) result);
}

/* ROUTINE:   Set_bit                                                   */
/*                                                                      */
/* SYNOPSIS:  Sets the next bit of the bit string pointed to by         */
/*            lineptr to a one.                                         */

set_bit(lineptr)

	bit_string *lineptr;

{
	/* This sets the masked bit */
	lineptr->pos |= lineptr->mask;

	lineptr->mask >>= 1;

	if (lineptr->mask == 0) {
		*lineptr->dbuf++ = lineptr->pos;
		lineptr->mask = BIT_MASK;
	}

}

/* ROUTINE:   Clr_bit                                                   */
/*                                                                      */
/* SYNOPSIS:  clears the next bit of the bit string pointed to by       */
/*            lineptr.  i.e set it to zero.                             */

clr_bit(lineptr)

	bit_string *lineptr;

{
	/* clear the masked bit */
	lineptr->pos &= ~(lineptr->mask);

	lineptr->mask >>= 1;	/* right shift the mask */

	if (lineptr->mask == 0) {	/* may need to move on to the next byte */
		*lineptr->dbuf++ = lineptr->pos;
		lineptr->mask = BIT_MASK;
	}

}
