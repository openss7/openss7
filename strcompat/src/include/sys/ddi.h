/*****************************************************************************

 @(#) $Id: ddi.h,v 0.9.2.16 2006/11/03 10:39:14 brian Exp $

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

 Last Modified $Date: 2006/11/03 10:39:14 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ddi.h,v $
 Revision 0.9.2.16  2006/11/03 10:39:14  brian
 - updated headers, correction to mi_timer_expiry type

 Revision 0.9.2.15  2005/07/15 23:08:18  brian
 - checking in for sync

 Revision 0.9.2.14  2005/07/12 13:54:40  brian
 - changes for os7 compatibility and check pass

 *****************************************************************************/

#ifndef __SYS_DDI_H__
#define __SYS_DDI_H__

#ident "@(#) $RCSfile: ddi.h,v $ $Name:  $($Revision: 0.9.2.16 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifdef LIS
#include <sys/LiS/ddi.h>
#endif
#ifdef LFS
#include <sys/streams/ddi.h>
#endif

#ifdef _SVR3_SOURCE
#include <sys/svr3/ddi.h>
#endif
#ifdef _SVR4_SOURCE
#include <sys/svr4/ddi.h>
#endif
#ifdef _LFS_SOURCE
#ifdef LIS
#include <sys/lfs/ddi.h>
#endif
#endif				/* _LFS_SOURCE */
#ifdef _LIS_SOURCE
#ifdef LFS
#include <sys/lis/ddi.h>
#endif
#endif				/* _LIS_SOURCE */
#ifdef _MPS_SOURCE
#include <sys/mps/ddi.h>
#endif
#ifdef _OSF_SOURCE
#include <sys/osf/ddi.h>
#endif
#ifdef _AIX_SOURCE
#include <sys/aix/ddi.h>
#endif
#ifdef _HPUX_SOURCE
#include <sys/hpux/ddi.h>
#endif
#ifdef _UW7_SOURCE
#include <sys/uw7/ddi.h>
#endif
#ifdef _SUN_SOURCE
#include <sys/sunddi.h>
#include <sys/strsun.h>
#endif
#ifdef _MAC_SOURCE
#include <sys/mac/ddi.h>
#endif
#ifdef _IRIX_SOURCE
#include <sys/irix/ddi.h>
#endif
#ifdef _OS7_SOURCE
#include <sys/os7/ddi.h>
#endif

#endif				/* __SYS_DDI_H__ */
