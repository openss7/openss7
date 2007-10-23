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

...int OP1(), ..., OPn();

/* OPERATIONS are numbered APDU_OPx, where each is a unique integer.  Further,
   APDU_UNKNOWN is used as a tag different than any valid operation.

   ERRORS are numbered ERROR_xyz, where each is a unique integer.
   ERROR_MISTYPED is used to signal an argument error to an operation.
   Further, ERROR_UNKNOWN is used as a tag to indicate that the operation
   succeeded.

   Finally, note that rox -> rox_args is updated in place by these routines.
   If the routine returns ERROR_UNKNOWN, then rox_args contains the results
   of the operation.  If the routine returns ERROR_MISTYPED, then rox_args is
   untouched.  Otherwise, if the routine returns any other value, then
   rox_args contains the parameters of the error which occurred.  Obviously,
   each routine calls ROXFREE prior to setting rox_args to a new value.
 */

static struct dispatch {
	int ds_operation;
	IFP ds_vector;
} dispatches[] = {
APDU_OP1, OP1,...APDU_OPn, OPn, APDU_UNKNOWN};

...
