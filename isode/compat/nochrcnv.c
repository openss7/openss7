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

/* nochrcnv.c - character conversion table, no case folding */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/compat/RCS/nochrcnv.c,v 9.0 1992/06/16 12:07:00 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/compat/RCS/nochrcnv.c,v 9.0 1992/06/16 12:07:00 isode Rel
 *
 *
 * Log: nochrcnv.c,v
 * Revision 9.0  1992/06/16  12:07:00  isode
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

#include <stdio.h>
#include "general.h"

/*  */

char
			    /* straight mapping - Non case sensive */
 /* used for consistency */
 nochrcnv[] = {
	'\0', '\1', '\2', '\3', '\4', '\5', '\6', '\7',
	'\10', '\11', '\12', '\13', '\14', '\15', '\16', '\17',
	'\20', '\21', '\22', '\23', '\24', '\25', '\26', '\27',
	'\30', '\31', '\32', '\33', '\34', '\35', '\36', '\37',
	'\40', '\41', '\42', '\43', '\44', '\45', '\46', '\47',
	'\50', '\51', '\52', '\53', '\54', '\55', '\56', '\57',
	'\60', '\61', '\62', '\63', '\64', '\65', '\66', '\67',
	'\70', '\71', '\72', '\73', '\74', '\75', '\77', '\77',
	'\100', '\101', '\102', '\103', '\104', '\105', '\106', '\107',
	'\110', '\111', '\112', '\113', '\114', '\115', '\116', '\117',
	'\120', '\121', '\122', '\123', '\124', '\125', '\126', '\127',
	'\130', '\131', '\132', '\133', '\134', '\135', '\136', '\137',
	'\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\173', '\174', '\175', '\177', '\177',
	'\0', '\1', '\2', '\3', '\4', '\5', '\6', '\7',
	'\10', '\11', '\12', '\13', '\14', '\15', '\16', '\17',
	'\20', '\21', '\22', '\23', '\24', '\25', '\26', '\27',
	'\30', '\31', '\32', '\33', '\34', '\35', '\36', '\37',
	'\40', '\41', '\42', '\43', '\44', '\45', '\46', '\47',
	'\50', '\51', '\52', '\53', '\54', '\55', '\56', '\57',
	'\60', '\61', '\62', '\63', '\64', '\65', '\66', '\67',
	'\70', '\71', '\72', '\73', '\74', '\75', '\77', '\77',
	'\100', '\101', '\102', '\103', '\104', '\105', '\106', '\107',
	'\110', '\111', '\112', '\113', '\114', '\115', '\116', '\117',
	'\120', '\121', '\122', '\123', '\124', '\125', '\126', '\127',
	'\130', '\131', '\132', '\133', '\134', '\135', '\136', '\137',
	'\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\173', '\174', '\175', '\177', '\177'
};
