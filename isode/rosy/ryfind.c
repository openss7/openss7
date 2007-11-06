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

/* ryfind.c - ROSY: find operations and errors by numbers and names */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/rosy/RCS/ryfind.c,v 9.0 1992/06/16 12:37:29 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/rosy/RCS/ryfind.c,v 9.0 1992/06/16 12:37:29 isode Rel
 *
 *
 * Log: ryfind.c,v
 * Revision 9.0  1992/06/16  12:37:29  isode
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

/* LINTLIBRARY */

#include <stdio.h>
#include "rosy.h"

/*  */

struct RyOperation *
findopbyop(ryo, op)
	register struct RyOperation *ryo;
	int op;
{
	if (!ryo)
		return NULL;

	for (; ryo->ryo_name; ryo++)
		if (ryo->ryo_op == op)
			return ryo;

	return NULL;
}

struct RyOperation *
findopbyname(ryo, name)
	register struct RyOperation *ryo;
	char *name;
{
	if (!ryo)
		return NULL;

	for (; ryo->ryo_name; ryo++)
		if (strcmp(ryo->ryo_name, name) == 0)
			return ryo;

	return NULL;
}

struct RyError *
finderrbyerr(rye, err)
	register struct RyError *rye;
	int err;
{
	if (!rye)
		return NULL;

	for (; rye->rye_name; rye++)
		if (rye->rye_err == err)
			return rye;

	return NULL;
}

struct RyError *
finderrbyname(rye, name)
	register struct RyError *rye;
	char *name;
{
	if (!rye)
		return NULL;

	for (; rye->rye_name; rye++)
		if (strcmp(rye->rye_name, name) == 0)
			return rye;

	return NULL;
}
