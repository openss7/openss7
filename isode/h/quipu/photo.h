/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

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

#ifndef __ISODE_QUIPU_PHOTO_H__
#define __ISODE_QUIPU_PHOTO_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* photo.h - */

/* 
 * Header: /xtel/isode/isode/h/quipu/RCS/photo.h,v 9.0 1992/06/16 12:23:11 isode Rel
 *
 *
 * Log: photo.h,v
 * Revision 9.0  1992/06/16  12:23:11  isode
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

typedef struct {
	char length;			/* number of bits in the pattern */
	short pattern;			/* the bit pattern (starting at bit 13) */
} code_word;

typedef struct {
	code_word make,			/* make up code */
	    /* if make.length = 0 then there is no make up code */
	 term;				/* terminal code */
} full_code;

typedef struct {
	unsigned char pos;		/* pointer into the string */
	int *run_top;			/* run length pointer */
	int *run_pos;			/* pointer to above */
	unsigned char mask;		/* mask to get at one bit of the string */
	char *dbuf_top;
	char *dbuf;
} bit_string;

typedef struct {
	char r_type;			/* descibes type of run */
	int run_length;			/* a run_length */
} run_type;

typedef struct nd {
	char n_type;			/* describe the type of node */
	struct nd *one;			/* one branch */
	struct nd *zero;		/* zero branch */
	int value;			/* value arrived at */
} node;

/* now some constant definitions */
#define MSB_MASK 010000		/* octal. Mask to get at first bit of code_word pattern */
#define BIT_MASK 0x80		/* i.e. set bit 8 of a char */

#define WHITE   0
#define WT_TERM 0
#define BLACK   1
#define BL_TERM 1
#define MAKE    2
#define EOLN    127
#define INTERNAL 7

#define P   1
#define H   2
#define V0  6
#define VR1 7
#define VR2 8
#define VR3 9
#define VL1 5
#define VL2 4
#define VL3 3

#define BUFLEN  75000
#define LINEBUF 4096

#define FIXED_OFFSET 6

full_code get_code();
char get_bit();
node *get_node();
run_type next_run();

#endif				/* __ISODE_QUIPU_PHOTO_H__ */
