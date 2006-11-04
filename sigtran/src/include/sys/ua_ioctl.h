/*****************************************************************************

 @(#) $Id: ua_ioctl.h,v 0.9.2.1 2006/11/04 11:31:18 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2006/11/04 11:31:18 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ua_ioctl.h,v $
 Revision 0.9.2.1  2006/11/04 11:31:18  brian
 - new generic ua files

 *****************************************************************************/

#ifndef __SYS_UA_IOCTL_H__
#define __SYS_UA_IOCTL_H__

#ident "@(#) $RCSfile: ua_ioctl.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#define UA_IOC_MAGIC	'0'

#define UA_OBJ_TYPE_LM		 0	/* layer management */
#define UA_OBJ_TYPE_AS_U	 1	/* application server (user) */
#define UA_OBJ_TYPE_AS_P	 2	/* application server (provider) */
#define UA_OBJ_TYPE_SP		 3	/* signalling process */
#define UA_OBJ_TYPE_SG		 4	/* signalling gateway */
#define UA_OBJ_TYPE_SPP		 5	/* signalling process proxy */
#define UA_OBJ_TYPE_ASP		 6	/* signalling process proxy */
#define UA_OBJ_TYPE_SGP		 7	/* signalling process proxy */
#define UA_OBJ_TYPE_SS_U	 8	/* signalling service (user) */
#define UA_OBJ_TYPE_SS_P	 9	/* signalling service (provider) */
#define UA_OBJ_TYPE_XP_SCTP	10	/* transport provider */
#define UA_OBJ_TYPE_XP_TCP	11	/* transport provider */
#define UA_OBJ_TYPE_XP_SSCOP	12	/* transport provider */
#define UA_OBJ_TYPE_DF		13	/* default */

#endif				/* __SYS_UA_IOCTL_H__ */
