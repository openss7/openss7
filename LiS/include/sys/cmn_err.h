/*****************************************************************************

 @(#) $RCSfile: cmn_err.h,v $ $Name:  $($Revision: 1.1.1.2.4.3 $) $Date: 2005/04/12 22:45:21 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2005/04/12 22:45:21 $ by $Author: brian $

 *****************************************************************************/

/*                               -*- Mode: C -*- 
 * cmd_err.h --- 
 * Author          : Francisco J. Ballesteros, David Grothe
 * Created On      : Tue May 31 21:40:37 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: cmn_err.h,v 1.1.1.2.4.3 2005/04/12 22:45:21 brian Exp $
 *
 * Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 */

#ifndef _CMD_ERR_H
#define _CMD_ERR_H 1

#ident "@(#) $RCSfile: cmn_err.h,v $ $Name:  $($Revision: 1.1.1.2.4.3 $) $Date: 2005/04/12 22:45:21 $"

#include <sys/LiS/genconf.h>

#define	CE_CONT		0	/* continue printing */
#define	CE_NOTE		1	/* NOTICE */
#define	CE_WARN		2	/* WARNING */
#define	CE_PANIC	3	/* PANIC */

#ifdef __KERNEL__

#ifdef __LIS_INTERNAL__
void lis_cmn_err_init(void);		/* not exported */
#endif
void lis_vcmn_err(int err_lvl, const char *fmt, va_list args);
void lis_cmn_err(int err_lvl, const char *fmt, ...) __attribute__ ((format(printf, 2, 3)));

#define	cmn_err		lis_cmn_err
#define vcmn_err	lis_vcmn_err

#endif				/* __KERNEL__ */

#endif				/* !_CMD_ERR_H */

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
