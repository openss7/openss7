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

/* move.c - */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/dish/RCS/move.c,v 9.0 1992/06/16 12:35:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/quipu/dish/RCS/move.c,v 9.0 1992/06/16 12:35:39 isode Rel
 *
 *
 * Log: move.c,v
 * Revision 9.0  1992/06/16  12:35:39  isode
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

#include "quipu/util.h"
#include "quipu/name.h"

extern DN dn;

#define	OPT	(!frompipe || rps -> ps_byteno == 0 ? opt : rps)
#define	RPS	(!frompipe || opt -> ps_byteno == 0 ? rps : opt)
extern char frompipe;
extern PS opt, rps;

extern char move_flag;

DN fixed_pos = NULLDN;

call_moveto(argc, argv)
	int argc;
	char **argv;
{
	char pwd_flag = FALSE;
	char check_move = TRUE;
	int x;

	if (argc == 1) {
		Usage(argv[0]);
		return;
	}

	move_flag = FALSE;

	for (x = 1; x < argc; x++) {
		if (test_arg(argv[x], "-pwd", 1))
			pwd_flag = TRUE;
		else if (test_arg(argv[x], "-nopwd", 3))
			pwd_flag = FALSE;
		else if (test_arg(argv[x], "-check", 1))
			check_move = TRUE;
		else if (test_arg(argv[x], "-nocheck", 3))
			check_move = FALSE;
		else if (test_arg(argv[x], "-sequence", 3)) {
			if (x + 1 == argc) {
				ps_printf(OPT, "We need a sequence name.\n");
				return;
			} else {
				shuffle_up(argc--, argv, x);
				set_sequence(argv[x]);
			}
		} else if (move(argv[x]) == OK) {
			if (move_flag == TRUE) {
				ps_print(RPS, "Too many parameters !\n");
				Usage(argv[0]);
				return;
			}
			move_flag = TRUE;
		} else {
			move_flag = FALSE;
			if (*argv[x] != '-')
				ps_printf(OPT, "Unknown entity '%s'\n", argv[x]);
			else
				Usage(argv[0]);
			return;
		}
	}

	if (check_move)
		if (test_move_dn() != TRUE) {
			move_flag = FALSE;
			return;
		}

	if (move_flag == TRUE)
		consolidate_move();

	if (pwd_flag) {
		dn_print(RPS, fixed_pos, EDBOUT);
		ps_print(RPS, "\n");
	}

}

consolidate_move()
{
	if (move_flag) {
		move_flag = FALSE;
		dn_free(fixed_pos);
		fixed_pos = dn_cpy(dn);
	}
}

set_current_pos()
{
	move_flag = FALSE;
	dn_free(dn);
	dn = dn_cpy(fixed_pos);
}

move(arg)
	char *arg;
{
	extern int print_parse_errors;

	DN user_down();
	DN str2dn_aux();
	DN sequence_dn();
	DN tmpdn;
	char *ptr;
	char alias = FALSE;
	int len;

	print_parse_errors = FALSE;

	if (*arg == '-') {
		print_parse_errors = TRUE;
		return (NOTOK);	/* can't be a move if it starts with a minus */
	}

	if (*arg == '+')
		arg++;		/* old style call */

	ptr = arg;
	while (*ptr != 0) {
		if (!isdigit(*ptr))
			break;
		ptr++;
	}

	if (*ptr == 0) {
		/* sequence move */
		dn_free(dn);
		dn = dn_cpy(sequence_dn(atoi(arg)));
		print_parse_errors = TRUE;
		if (dn == NULLDN) {
			ps_printf(OPT, "Invalid sequence number %s\n", arg);
			return (NOTOK);
		} else
			return (OK);
	}

	len = strlen(arg);
	if ((arg[0] == '<') && (arg[len - 1] == '>')) {
		dn_free(dn);
		dn = str2dn(arg);
		return (OK);
	}

	if ((ptr = index(arg, '@')) != NULLCP) {
		*ptr = 0;

		if (*arg == 0) {
			/* from root */
			dn_free(dn);
			dn = NULLDN;
			*ptr = '@';
			arg = ++ptr;
			if (*arg == 0) {
				print_parse_errors = TRUE;
				return (OK);	/* @ -> move to root */
			}
			if ((ptr = index(arg, '@')) != NULLCP)
				*ptr = 0;
		}
	} else {
		dn_free(dn);
		dn = dn_cpy(fixed_pos);
	}

	if (strcmp(arg, "..") == 0) {
		do {
			DN dnptr;
			DN trail;

			if (dn == NULLDN) {
				print_parse_errors = TRUE;
				ps_print(OPT, "Can't go back past root\n");
				return (NOTOK);
			}
			if (dn->dn_parent == NULLDN) {
				dn_free(dn);
				dn = NULLDN;
			} else {
				for (dnptr = dn; dnptr->dn_parent != NULLDN;
				     dnptr = dnptr->dn_parent)
					trail = dnptr;

				dn_comp_free(dnptr);
				trail->dn_parent = NULLDN;
			}
			if (ptr == NULLCP) {
				print_parse_errors = TRUE;
				return (OK);
			}
			arg = ++ptr;
			if ((ptr = index(arg, '@')) != NULLCP)
				*ptr = 0;
		} while (strcmp(arg, "..") == 0);
	}

	if (ptr != NULL)
		*ptr = '@';

	if ((tmpdn = str2dn_aux(arg, &alias)) != NULLDN) {
		if (dn == NULLDN)
			dn = tmpdn;
		else {
			if (alias) {
				dn_free(dn);
				dn = tmpdn;
			} else
				dn_append(dn, tmpdn);
		}
		print_parse_errors = TRUE;
		return (OK);
	} else {
		print_parse_errors = TRUE;
		return (NOTOK);
	}

}

test_move_dn()
{
	char *name = "moveto";

	/* Might do something else here... */
	/* current policy is to read the entry and cache it ! */

	if (dn == NULLDN)
		return (TRUE);	/* assume root exists - read will fail */

	return (read_cache(1, &name));
}
