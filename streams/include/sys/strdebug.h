/*****************************************************************************

 @(#) $Id: strdebug.h,v 0.9.2.6 2004/06/20 20:34:04 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/06/20 20:34:04 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_STRDEBUG_H__
#define __SYS_STRDEBUG_H__

#ident "@(#) $RCSfile: strdebug.h,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2004/06/20 20:34:04 $"

#define __never() \
do { panic("%s: never() at "__FILE__ " +%d\n", __FUNCTION__, __LINE__); } while(0)

#define __rare() \
do { printk(KERN_NOTICE "%s: rare() at "__FILE__ " +%d\n", __FUNCTION__, __LINE__); } while(0)

#define __seldom() \
do { printk(KERN_NOTICE "%s: seldom() at "__FILE__ " +%d\n", __FUNCTION__, __LINE__); } while(0)

#define __usual(__exp) \
do { if (!(__exp)) printk(KERN_WARNING "%s: usual(" #__exp ") failed at " __FILE__ " +%d\n",__FUNCTION__, __LINE__); } while(0)

#define __normal(__exp) \
do { if (!(__exp)) printk(KERN_WARNING "%s: normal(" #__exp ") failed at " __FILE__ " +%d\n",__FUNCTION__, __LINE__); } while(0)

#define __assert(__exp) \
do { if (!(__exp)) { printk(KERN_EMERG "%s: assert(" #__exp ") failed at " __FILE__ " +%d\n",__FUNCTION__, __LINE__); *(int *)0 = 0; } } while(0)

#define __assure(__exp) \
do { if (!(__exp)) printk(KERN_WARNING "%s: assure(" #__exp ") failed at " __FILE__ " +%d\n",__FUNCTION__, __LINE__); } while(0)

#define __ensure(__exp,__sta) \
do { if (!(__exp)) { printk(KERN_WARNING "%s: ensure(" #__exp ") failed at " __FILE__ " +%d\n",__FUNCTION__, __LINE__); __sta; } } while(0)

#define __unless(__exp,__sta) \
__ensure(!(__exp),__sta)

#define __trace() \
do { printk(KERN_INFO "%s: trace() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); } while(0)

#define __ptrace(__pkspec) \
do { printk(KERN_INFO "%s: ptrace() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); printk __pkspec; } while(0)

#define __fixme(__pkspec) \
do { printk(KERN_INFO "%s: fixme() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); printk __pkspec; } while(0)

#define __todo(__pkspec) \
do { printk(KERN_INFO "%s: todo() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); printk __pkspec; } while(0)

#define __ctrace(__fnc) \
({ printk(KERN_INFO "%s: calling " #__fnc " at " __FILE__ "+%d\n", __FUNCTION__, __LINE__); __fnc; })

#define __printd(__pkspec) \
do { printk __pkspec; } while(0)

#define __swerr() \
do { printk(KERN_WARNING "%s: swerr() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); } while(0)

#define __pswerr(__pkspec) \
do { printk(KERN_WARNING "%s: pswerr() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); printk __pkspec; } while(0)

#if defined(CONFIG_STREAMS_DEBUG)

#define STATIC
#define INLINE

#define   never()		__never()
#define    rare()		__rare()
#define  seldom()		__seldom()
#define   usual(__exp)		__usual(__exp)
#define unusual(__exp)		__usual(!(__exp))
#define  normal(__exp)		__normal(__exp)
#define abnormal(__exp)		__normal(!(__exp))
#define  assert(__exp)		__assert(__exp)
#define  assure(__exp)		__assure(__exp)
#define  ensure(__exp,__sta)	__ensure(__exp,__sta)
#define  unless(__exp,__sta)	__unless(__exp,__sta)
#define   trace()		__trace()
#define  ptrace(__pks)		__ptrace(__pks)
#define  ctrace(__fnc)		__ctrace(__fnc)
#define   fixme(__pks)		__fixme(__pks)
#define    todo(__pks)		__todo(__pks)
#define  printd(__pks)		__printd(__pks)
#define   swerr()		__swerr()
#define  pswerr(__pks)		__pswerr(__pks)

#elif defined(CONFIG_STREAMS_TEST)

#define STATIC static
#define INLINE inline

#define   never()		__never()
#define    rare()		__rare()
#define  seldom()		__seldom()
#define   usual(__exp)		__usual(__exp)
#define unusual(__exp)		__usual(!(__exp))
#define  normal(__exp)		__normal(__exp)
#define abnormal(__exp)		__noemal(!(__exp))
#define  assert(__exp)		__assert(__exp)
#define  assure(__exp)		__assure(__exp)
#define  ensure(__exp,__sta)	__ensure(__exp,__sta)
#define  unless(__exp,__sta)	__unless(__exp,__sta)
#define   trace()		do { } while(0)
#define  ptrace(__pks)		do { } while(0)
#define  ctrace(__fnc)		(__fnc)
#define   fixme(__pks)		__fixme(__pks)
#define    todo(__pks)		__todo(__pks)
#define  printd(__pks)		do { } while(0)
#define   swerr()		__swerr()
#define  pswerr(__pks)		__pswerr(__pks)

#elif defined(CONFIG_STREAMS_SAFE)

#define STATIC static
#define INLINE inline

#define   never()		__never()
#define    rare()		do { } while(0)
#define  seldom()		do { } while(0)
#define   usual(__exp)		do { } while(0)
#define unusual(__exp)		do { } while(0)
#define  normal(__exp)		do { } while(0)
#define abnormal(__exp)		do { } while(0)
#define  assert(__exp)		__assert(__exp)
#define  assure(__exp)		__assure(__exp)
#define  ensure(__exp,__sta)	__ensure(__exp,__sta)
#define  unless(__exp,__sta)	__unless(__exp,__sta)
#define   trace()		do { } while(0)
#define  ptrace(__pks)		do { } while(0)
#define  ctrace(__fnc)		(__fnc)
#define   fixme(__pks)		__fixme(__pks)
#define    todo(__pks)		__todo(__pks)
#define  printd(__pks)		do { } while(0)
#define   swerr()		__swerr()
#define  pswerr(__pks)		__swerr()

#else

#define STATIC static
#define INLINE inline

#define   never()		do { } while(0)
#define	   rare()		do { } while(0)
#define	 seldom()		do { } while(0)
#define   usual(__exp)		do { } while(0)
#define unusual(__exp)		do { } while(0)
#define  normal(__exp)		do { } while(0)
#define abnormal(__exp)		do { } while(0)
#define  assert(__exp)		do { } while(0)
#define  assure(__exp)		do { } while(0)
#define  ensure(__exp,__sta)	do { } while(0)
#define  unless(__exp,__sta)	do { } while(0)
#define  trace()		do { } while(0)
#define  ptrace(__pks)		do { } while(0)
#define  ctrace(__fnc)		(__fnc)
#define   fixme(__pks)		__fixme(__pks)
#define    todo(__pks)		do { } while(0)
#define  printd(__pks)		do { } while(0)
#define   swerr()		__swerr()
#define  pswerr(__pks)		__swerr()

#endif

#endif				/* __SYS_STRDEBUG_H__ */
