/*****************************************************************************

 @(#) $RCSfile: m3ua.h,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:42 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@dallas.net>

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

 Last Modified $Date: 2006/10/17 11:55:42 $ by $Author: brian $

 *****************************************************************************/

#ifndef __M3UA_H__
#define __M3UA_H__

#ident "@(#) $RCSfile: m3ua.h,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:42 $"

#include "../ua/ua.h"
#include "../../include/ss7/mtp.h"
#include "../../include/ss7/mtp_ioctl.h"

#ifndef M3UA_CMAJOR
#define M3UA_CMAJOR 240
#endif
#define M3UA_NMINOR 255

#define M3UA_MODULE_ID ('U'<<8|MTP_IOC_MAGIC)

extern ops_t m3ua_lmq_u_ops;
extern ops_t m3ua_ss7_u_ops;

#endif				/* __M3UA_H__ */
