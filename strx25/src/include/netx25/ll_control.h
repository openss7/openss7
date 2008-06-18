/*****************************************************************************

 @(#) $Id: ll_control.h,v 0.9.2.1 2008-06-13 06:43:57 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-06-13 06:43:57 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ll_control.h,v $
 Revision 0.9.2.1  2008-06-13 06:43:57  brian
 - added files

 *****************************************************************************/

#ifndef __NETX25_LL_CONTROL_H__
#define __NETX25_LL_CONTROL_H__

#ident "@(#) $RCSfile: ll_control.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

#define LLIOC ('L'<<8)

#define L_SETSNID		(LLIOC|0x01)
#define L_GETSNID		(LLIOC|0x02)
#define L_SETTUNE		(LLIOC|0x03)
#define L_GETTUNE		(LLIOC|0x04)
#define L_GETSTATS		(LLIOC|0x05)
#define L_ZEROSTATS		(LLIOC|0x06)
#define L_TRACEON		(LLIOC|0x07)
#define L_TRACEOFF		(LLIOC|0x08)

#define L_GETGSTATS		(LLIOC|0x0a)
#define L_ZEROGSTATS		(LLIOC|0x0b)
#define L_LINKDISABLE		(LLIOC|0x0c)
#define L_LINKENABLE		(LLIOC|0x0d)

#define L_PUTX32MAP		(LLIOC|0x0f)
#define L_GETX32MAP		(LLIOC|0x10)

#endif				/* __NETX25_LL_CONTROL_H__ */

