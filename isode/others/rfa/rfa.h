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

#ifndef __RFA_RFA_H__
#define __RFA_RFA_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * RFA - Remote File Access
 *
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * rfa.h : common definitions for RFA
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * Header: /xtel/isode/isode/others/rfa/RCS/rfa.h,v 9.0 1992/06/16 12:47:25 isode Rel
 *
 * Log: rfa.h,v
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

/*
 *                              NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include "config.h"

#ifndef OK
#define OK 		0
#define NOTOK		1
#endif
#define NOTOK_LOCAL_LOCK		2
#define NOTOK_LOCAL_ERROR		3
#define NOTOK_REMOTE_ERROR		4
#define NOTOK_LOCKED			5
#define NOTOK_FILEACCESS		6
#define NOTOK_NOTREGULAR		7
#define NOTOK_GETMASTER			8
#define NOTOK_OUTOFSUBTREE		9
#define NOTOK_NOTLOCKED			10
#define NOTOK_ALREADY_SLAVE		11
#define NOTOK_ALREADY_MASTER		12
#define NOTOK_ALREADY_UNREG		13
#define NOTOK_REMOTE_NOT_MASTER		14
#define NOTOK_REMOTE_MASTER_OLDER	15
#define NOTOK_UNREG_LOCAL_FILE		16
#define NOTOK_IS_SLAVE			17
#define NOTOK_SYS			18
#define NOTOK_INCONSISTENCY		19
#define NOTOK_NOT_ALLOWED		20

extern char *makeFN2();
extern char *makeFN();
extern char *basename();
extern char *dirname();
extern char *expandSymLinks();
extern char *realPath(), *realPath3();

extern char rfaErrStr[];
extern char *errMsg();

/*--- tailor variables ---*/
extern int default_transfer;
extern int doChown;
extern int doChgrp;
extern int doChmod;
extern int doClearSUID;
extern int doRmWidows;
extern int doRfaExec;
extern int timeSlave;
extern int compLimit;
extern char *passwd;
extern char *user;
extern char *host;
extern int backup;

#endif				/* __RFA_RFA_H__ */
