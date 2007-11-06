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

/* ps_alloc.c - allocate a presentation stream */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/psap/RCS/ps_alloc.c,v 9.0 1992/06/16 12:25:44 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/psap/RCS/ps_alloc.c,v 9.0 1992/06/16 12:25:44 isode Rel
 *
 *
 * Log: ps_alloc.c,v
 * Revision 9.0  1992/06/16  12:25:44  isode
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
#include "psap.h"

/* A Presentatation Stream (or PStream) is the second generation of
   "generic" I/O stream-based handling.  (For the first attempt,
   take a look at the prototype implementation of the TTI Trusted Mail
   Agent.)  The idea is to present a common, simple I/O paradigm (i.e.,
   the UNIX v7 philosophy) to protocol-translation entities regardless of
   the underlying medium (files, pipes, sockets, or strings).

   New streams are created by a call to ps_alloc().  It allocates memory
   and calls an open routine.  This routine fills in the dispatch vectors
   for read/write and (optionally) close.  It can also fill in any other
   part of the stream's structure it likes.

   Once created, I/O is done using the macros ps_read/ps_write.  These
   return either NOTOK or OK; depending on how things went. The read/write
   routines are invoked as:

	int	iofunc (ps, data, n, in_line)
	PS	ps;
	PElementData data;
	PElementLen  n;
	int	in_line;

   They should read/write upto len bytes, starting at data, and return the
   number of bytes processed, or NOTOK on error.  The routine ps_io() will
   make successive calls to fill/flush the data.  If the read/write routine
   returns NOTOK, it should set ps_errno as well.

   Streams are removed by a call to ps_free ().  It calls the close
   routine, if any, which should de-commission any parts of the stream's
   structure that are in use.  ps_free() will then free the allocated
   memory.
 */

/*  */

PS
ps_alloc(io)
	register IFP io;
{
	register PS ps;

	if ((ps = (PS) calloc(1, sizeof *ps)) == NULLPS)
		return NULLPS;

	if ((*io) (ps) == NOTOK) {
		ps_free(ps);
		return NULLPS;
	}

	return ps;
}
