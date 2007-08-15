/*****************************************************************************

 @(#) $Id: mtdrv.h,v 1.1.1.1.12.5 2007/08/14 10:47:10 brian Exp $

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

 Last Modified $Date: 2007/08/14 10:47:10 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mtdrv.h,v $
 Revision 1.1.1.1.12.5  2007/08/14 10:47:10  brian
 - GPLv3 header update

 *****************************************************************************/

#ifndef __SYS_LIS_MTDRV_H__
#define __SYS_LIS_MTDRV_H__

#ident "@(#) $RCSfile: mtdrv.h,v $ $Name:  $($Revision: 1.1.1.1.12.5 $) $Date: 2007/08/14 10:47:10 $"

/* This file can be processed with doxygen(1). */

/** @addtogroup mtdrv
  * @{ */

/** @file
  * STREAMS Muti Thread Driver mtdrv(4) header file.
  * This file contains IOCTL definitions for mtdrv(4).  */

/** @name MT Driver Magic Number
  * @{ */
#define	MTDRV_IOCTL(n)	(('M' << 8) | (n))
/** @} */

/** @name MT Driver Input-Output Control Commands
  * @{ */
#define MTDRV_SET_CLONE_DEV	MTDRV_IOCTL(1)
#define MTDRV_SET_OPEN_SLEEP	MTDRV_IOCTL(2)
/** @} */

/** @} */

#endif				/* __SYS_LIS_MTDRV_H__ */

// vim: ft=cpp com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
