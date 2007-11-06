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

/* cstrings.c - */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/cstrings.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/common/RCS/cstrings.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: cstrings.c,v
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

#include "psap.h"

static char arg_error[1024];
static char arg_flag[100];
int chase_flag = 2;
extern char *result_sequence;

reset_arg()
{
	arg_error[0] = 0;
	arg_flag[0] = 0;
	chase_flag = 2;
	if (result_sequence)
		free(result_sequence);
	result_sequence = NULLCP;
}

print_arg_error(opt)
	PS opt;
{
	if (arg_error[0] != 0) {
		ps_printf(opt, "'%s' ambiguous, specify\n%s", arg_flag, arg_error);
		return (OK);
	} else
		return (NOTOK);
}

int
test_arg(x, y, c)
	char *x;
	char *y;
	int c;
{
	int count = 0;
	char *top, *topx;

	top = y;
	topx = x;

	if (*y == '-')
		count--;

	for (; (*y != 0) || (*x != 0); y++) {
		if (*x == 0)
			if (count >= c)
				return (1);
			else {
				(void) strcat(arg_error, top);
				(void) strcat(arg_error, "\n");
				(void) strcpy(arg_flag, topx);
				return (0);
			}
		if (chrcnv[*x] != chrcnv[*y])
			return (0);

		count++;
		x++;
	}

	if (count >= c)
		return (1);
	else {
		(void) strcat(arg_error, top);
		(void) strcat(arg_error, "\n");
		(void) strcpy(arg_flag, topx);
		return (0);
	}
}
