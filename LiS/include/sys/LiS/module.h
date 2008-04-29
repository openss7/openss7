/*****************************************************************************

 @(#) $RCSfile: module.h,v $ $Name:  $($Revision: 1.1.1.1.4.6 $) $Date: 2008-04-29 08:33:15 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2008-04-29 08:33:15 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: module.h,v $
 Revision 1.1.1.1.4.6  2008-04-29 08:33:15  brian
 - update headers for Affero release

 Revision 1.1.1.1.4.5  2007/08/14 10:47:10  brian
 - GPLv3 header update

 *****************************************************************************/

#ifndef LIS_MODULE_H
#define LIS_MODULE_H

#ident "@(#) $RCSfile: module.h,v $ $Name:  $($Revision: 1.1.1.1.4.6 $) $Date: 2008-04-29 08:33:15 $"

/************************************************************************
*                        LiS Module Header File                         *
*************************************************************************
*									*
* A STREAMS driver that is capable of being a separately loadable	*
* module can include this file at the very beginning.  This header file	*
* will sense the MODULE and MODVERSIONS kernel parameters and include	*
* the proper kernel header files in the proper order.			*
* 									*
* There is a difference between 2.4 and 2.6 in this respect.  In 2.4	*
* the kernel defines "module_info", a standard STREAMS symbol.  If we	*
* include <sys/stream.h> BEFORE <linux/module.h> then the kernel's	*
* definition clashes with ours.						*
* 									*
* In 2.6 the kernel declares "dev_t" in such a way that it interferes	*
* with the LiS definition.  We need to define it away before including	*
* any kernel includes.							*
* 									*
* The bottom line is that the order of includes makes a difference	*
* depending upon whether it is a 2.4 or 2.6 kernel.  So this file exists*
* in order to make it easy for the driver write to get around this.	*
*									*
************************************************************************/

#if defined(LINUX) && defined(__KERNEL__)

#include <sys/LiS/linux-mdep.h>

#if defined(KERNEL_2_4)
#undef module_info
#endif

#ifdef MODULE
# if defined(LINUX) && defined(__KERNEL__)
#  ifdef MODVERSIONS
#   ifdef LISMODVERS
#    include <sys/modversions.h>	/* /usr/src/LiS/include/sys */
#   else
#    include <linux/modversions.h>
#   endif
#  endif
#  include <linux/module.h>
# else
#  error This can only be a module in the Linux kernel environment
# endif
#endif

#else				/* defined(LINUX) && defined(__KERNEL__) */
#ifdef USER

#include <sys/LiS/user-mdep.h>

#endif
#endif				/* defined(LINUX) && defined(__KERNEL__) */

#include <sys/LiS/modcnt.h>	/* MODGET(), MODPUT() */

#endif				/* LIS_MODULE_H */
