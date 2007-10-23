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
    "Header: /xtel/isode/isode/dsap/common/RCS/integer.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/dsap/common/RCS/integer.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: integer.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

/* LINTLIBRARY */

#include "quipu/util.h"
#include "quipu/attr.h"
#include "psap.h"

static PE
intenc(x)
	int *x;
{
	return (int2prim(*x));
}
static PE
enumenc(x)
	int *x;
{
	return enumint2prim((integer) * x);
}

static int *
intdec(pe)
	PE pe;
{
	int *x;

	if (!test_prim_pe(pe, PE_CLASS_UNIV, PE_PRIM_INT))
		return (0);

	x = (int *) smalloc(sizeof(int));
	*x = prim2num(pe);

	return x;

}

static int *
enumdec(pe)
	PE pe;
{
	int *x;

	if (!test_prim_pe(pe, PE_CLASS_UNIV, PE_PRIM_ENUM))
		return 0;
	x = (int *) smalloc(sizeof(int));
	*x = prim2enum(pe);

	return x;
}

/* ARGSUSED */
static
intprint(ps, x, format)
	PS ps;
	int *x, format;
{
	ps_printf(ps, "%d", *x);
}

#define enumprint intprint

static int *
intdup(x)
	int *x;
{
	int *y;

	y = (int *) smalloc(sizeof(int));
	*y = *x;

	return (y);
}

#define enumdup intdup

static
intcmp(x, y)
	int *x, *y;
{
	return (*x == *y ? 0 : (*x > *y ? 1 : -1));
}

#define enumcmp intcmp

static
intfree(x)
	int *x;
{
	free((char *) x);
}

#define enumfree intfree

static int *
intparse(str)
	char *str;
{
	int atoi();
	int *x;

	x = (int *) smalloc(sizeof(int));
	*x = atoi(str);

	return (x);
}

#define enumparse intparse

integer_syntax()
{
	(void) add_attribute_syntax("integer",
				    (IFP) intenc, (IFP) intdec,
				    (IFP) intparse, intprint,
				    (IFP) intdup, intcmp, intfree, NULLCP, NULLIFP, FALSE);

	(void) add_attribute_syntax("enumerated",
				    (IFP) enumenc, (IFP) enumdec,
				    (IFP) enumparse, enumprint,
				    (IFP) enumdup, enumcmp, enumfree, NULLCP, NULLIFP, FALSE);
}
