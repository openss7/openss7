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


#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/sequence.c,v 9.0 1992/06/16 12:44:54 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/sequence.c,v 9.0 1992/06/16 12:44:54 isode Rel
 *
 *
 * Log: sequence.c,v
 * Revision 9.0  1992/06/16  12:44:54  isode
 * Release 8.0
 *
 */

#include "general.h"
#include <sys/types.h>
#include "sequence.h"

void
add_seq(seq, str)
	str_seq *seq;
	char *str;
{
	str_seq curr;

	if (*seq) {
		for (curr = *seq; curr->next; curr = curr->next) ;
		curr->next = (struct str_seq *) malloc(sizeof(struct str_seq));
		curr = curr->next;
	} else {
		curr = (struct str_seq *) malloc(sizeof(struct str_seq));
		*seq = curr;
	}

	curr->dname = (char *) malloc((unsigned int) (strlen(str) + 1));
	(void) strcpy(curr->dname, str);

	curr->next = 0;
}

char *
get_from_seq(seq_num, seq_ptr)
	int seq_num;
	str_seq seq_ptr;
{
	for (; seq_num > 1 && seq_ptr; seq_ptr = seq_ptr->next, seq_num--) {
	}
	if (seq_ptr)
		return seq_ptr->dname;
	else
		return 0;
}

void
free_seq(seq_ptr)
	str_seq seq_ptr;
{
	str_seq next_seq;

	while (seq_ptr) {
		free((char *) (seq_ptr->dname));
		next_seq = seq_ptr->next;
		free((char *) seq_ptr);
		seq_ptr = next_seq;
	}
}
