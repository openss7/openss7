/*****************************************************************************

 @(#) $Id: debug.h,v 0.9.2.11 2006/06/14 10:37:17 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>

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

 Last Modified $Date: 2006/06/14 10:37:17 $ by $Author: brian $

 *****************************************************************************/

#ifndef __OS7_DEBUG_H__
#define __OS7_DEBUG_H__

#ident "@(#) $RCSfile: debug.h,v $ $Name:  $($Revision: 0.9.2.11 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#if defined LFS

#ifdef _DEBUG
#define CONFIG_STREAMS_DEBUG 1
#else
#ifdef _TEST
#define CONFIG_STREAMS_TEST 1
#else
#ifdef _SAFE
#define CONFIG_STREAMS_SAFE 1
#else
#define CONFIG_STREAMS_NONE 1
#endif
#endif
#endif

#include <sys/strdebug.h>
#include <sys/debug.h>

#else				/* defined LFS */

#if 1
#define DDTRACE          do { printk("%s [%s %d] trace\n",__FUNCTION__,__FILE__, __LINE__); } while(0)
#define DDPRINT(n,args)  do { printk args; } while(0)

#if	defined _DEBUG
#define DTRACE           do { if ( DEBUG_LEVEL ) { printk("%s [%s %d] trace\n",__FUNCTION__,__FILE__, __LINE__); } } while(0)
#define DPRINT(n,args)   do { if (DEBUG_LEVEL>n) { printk args; } } while(0)
#define DBLOCK(n,block)     { if (DEBUG_LEVEL>n) { block } }
#else				/* defined _DEBUG */
#define DTRACE
#define DPRINT(n,args)
#define DBLOCK(n,block)
#endif				/* defined _DEBUG */
#endif				/* 1 */

#undef  __never
#define __never() \
do { panic("%s: never() at "__FILE__ " +%d\n", __FUNCTION__, __LINE__); } while(0)

#undef  __rare
#define __rare() \
do { printk(KERN_NOTICE "%s: rare() at "__FILE__ " +%d\n", __FUNCTION__, __LINE__); } while(0)

#undef  __seldom
#define __seldom() \
do { printk(KERN_NOTICE "%s: seldom() at "__FILE__ " +%d\n", __FUNCTION__, __LINE__); } while(0)

#undef  __usual
#define __usual(__exp) \
do { if (unlikely(!(__exp))) printk(KERN_WARNING "%s: usual(%s) failed at " __FILE__ " +%d\n",__FUNCTION__, #__exp, __LINE__); } while(0)

#undef  __normal
#define __normal(__exp) \
do { if (unlikely(!(__exp))) printk(KERN_WARNING "%s: normal(%s) failed at " __FILE__ " +%d\n",__FUNCTION__, #__exp, __LINE__); } while(0)

#undef  __assert
#define __assert(__exp) \
do { if (unlikely(!(__exp))) { printk(KERN_EMERG "%s: assert(%s) failed at " __FILE__ " +%d\n",__FUNCTION__, #__exp, __LINE__); *(int *)0 = 0; } } while(0)

#undef  __assure
#define __assure(__exp) \
do { if (unlikely(!(__exp))) printk(KERN_WARNING "%s: assure(%s) failed at " __FILE__ " +%d\n",__FUNCTION__, #__exp, __LINE__); } while(0)

#undef  __ensure
#define __ensure(__exp,__sta) \
do { if (unlikely(!(__exp))) { printk(KERN_WARNING "%s: ensure(%s) failed at " __FILE__ " +%d\n",__FUNCTION__, #__exp, __LINE__); __sta; } } while(0)

#undef  __unless
#define __unless(__exp,__sta) \
__ensure(!(__exp),__sta)

#undef  __trace
#define __trace() \
do { printk(KERN_INFO "%s: trace() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); } while(0)

#undef  __ptrace
#define __ptrace(__pkspec) \
do { printk(KERN_INFO "%s: ptrace() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); printk __pkspec; } while(0)

#undef  __fixme
#define __fixme(__pkspec) \
do { printk(KERN_INFO "%s: fixme() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); printk __pkspec; } while(0)

#undef  __todo
#define __todo(__pkspec) \
do { printk(KERN_INFO "%s: todo() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); printk __pkspec; } while(0)

#undef  __ctrace
#define __ctrace(__fnc) \
({ printk(KERN_INFO "%s: calling " #__fnc " at " __FILE__ "+%d\n", __FUNCTION__, __LINE__); __fnc; })

#undef  __printd
#define __printd(__pkspec) \
do { printk __pkspec; } while(0)

#undef  __swerr
#define __swerr() \
do { printk(KERN_WARNING "%s: swerr() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); } while(0)

#undef  __pswerr
#define __pswerr(__pkspec) \
do { printk(KERN_WARNING "%s: pswerr() at " __FILE__ " +%d\n", __FUNCTION__, __LINE__); printk __pkspec; } while(0)

#define    _never()		do { } while(0)
#define     _rare()		do { } while(0)
#define   _seldom()		do { } while(0)
#define    _usual(__exp)	do { } while(0)
#define  _unusual(__exp)	do { } while(0)
#define   _normal(__exp)	do { } while(0)
#define _abnormal(__exp)	do { } while(0)
#define   _assert(__exp)	do { } while(0)
#define   _assure(__exp)	do { } while(0)
#define   _ensure(__exp,__sta)	do { if (unlikely(!(__exp))) { __sta; } } while(0)
#define   _unless(__exp,__sta)	do { if (unlikely( (__exp))) { __sta; } } while(0)
#define    _trace()		do { } while(0)
#define   _ptrace(__pks)	do { } while(0)
#define   _ctrace(__fnc)	(__fnc)
#define    _fixme(__pks)	do { } while(0)
#define     _todo(__pks)	do { } while(0)
#define   _printd(__pks)	do { } while(0)
#define    _swerr()		do { } while(0)
#define   _pswerr(__pks)	do { } while(0)

#if	defined _DEBUG

#define    never()		__never()
#define     rare()		__rare()
#define   seldom()		__seldom()
#define    usual(__exp)		__usual(__exp)
#define  unusual(__exp)		__usual(!(__exp))
#define   normal(__exp)		__normal(__exp)
#define abnormal(__exp)		__normal(!(__exp))
#define  dassert(__exp)		__assert(__exp)
#define   assert(__exp)		__assert(__exp)
#define   assure(__exp)		__assure(__exp)
#define   ensure(__exp,__sta)	__ensure(__exp,__sta)
#define   unless(__exp,__sta)	__ensure(!(__exp),__sta)
#define    trace()		__trace()
#define   ptrace(__pks)		__ptrace(__pks)
#define   ctrace(__fnc)		__ctrace(__fnc)
#define    fixme(__pks)		__fixme(__pks)
#define     todo(__pks)		__todo(__pks)
#define   printd(__pks)		__printd(__pks)
#define    swerr()		__swerr()
#define   pswerr(__pks)		__pswerr(__pks)

#undef STATIC
#define STATIC
#undef INLINE
#define INLINE

#else				/* defined _DEBUG */
#if	defined _TEST

#define    never()		__never()
#define     rare()		__rare()
#define   seldom()		__seldom()
#define    usual(__exp)		__usual(__exp)
#define  unusual(__exp)		__usual(!(__exp))
#define   normal(__exp)		__normal(__exp)
#define abnormal(__exp)		__normal(!(__exp))
#define  dassert(__exp)		__assert(__exp)
#define   assert(__exp)		__assert(__exp)
#define   assure(__exp)		__assure(__exp)
#define   ensure(__exp,__sta)	__ensure(__exp,__sta)
#define   unless(__exp,__sta)	__ensure(!(__exp),__sta)
#define    trace()		_trace()
#define   ptrace(__pks)		_ptrace(__pks)
#define   ctrace(__fnc)		_ctrace(__fnc)
#define    fixme(__pks)		__fixme(__pks)
#define     todo(__pks)		__todo(__pks)
#define   printd(__pks)		_printd(__pks)
#define    swerr()		__swerr()
#define   pswerr(__pks)		__pswerr(__pks)

#undef STATIC
#define STATIC static
#undef INLINE
#ifdef __inline
#define INLINE inline
#else
#ifdef __inline__
#define INLINE __inline__
#else
#define INLINE inline
#endif
#endif

#else				/* defined _TEST */
#if	defined _SAFE

#define    never()		do { *(int *)0 = 0; } while(0)
#define     rare()		_rare()
#define   seldom()		_seldom()
#define    usual(__exp)		_usual(__exp)
#define  unusual(__exp)		_unusual(__exp)
#define   normal(__exp)		_normal(__exp)
#define abnormal(__exp)		_abnormal(__exp)
#define  dassert(__exp)		_assert(__exp)
#define   assert(__exp)		{ if (!(__exp)) *(int *)0 = 0; } while(0)
#define   assure(__exp)		__assure(__exp)
#define   ensure(__exp,__sta)	__ensure(__exp,__sta)
#define   unless(__exp,__sta)	__ensure(!(__exp),__sta)
#define    trace()		_trace()
#define   ptrace(__pks)		_ptrace(__pks)
#define   ctrace(__fnc)		_ctrace(__fnc)
#define    fixme(__pks)		__fixme(__pks)
#define     todo(__pks)		__todo(__pks)
#define   printd(__pks)		_printd(__pks)
#define    swerr()		__swerr()
#define   pswerr(__pks)		__pswerr(__pks)

#undef STATIC
#define STATIC static
#undef INLINE
#ifdef __inline
#define INLINE __inline
#else
#ifdef __inline__
#define INLINE __inline__
#else
#define INLINE inline
#endif
#endif

#else				/* defined _SAFE */

#define    never()		_never()
#define     rare()		_rare()
#define   seldom()		_seldom()
#define    usual(__exp)		_usual(__exp)
#define  unusual(__exp)		_unusual(__exp)
#define   normal(__exp)		_normal(__exp)
#define abnormal(__exp)		_abnormal(__exp)
#define  dassert(__exp)		_assert(__exp)
#define   assert(__exp)		_assert(__exp)
#define   assure(__exp)		_assure(__exp)
#define   ensure(__exp,__sta)	_ensure(__exp,__sta)
#define   unless(__exp,__sta)	_unless(__exp,__sta)
#define    trace()		_trace()
#define   ptrace(__pks)		_ptrace(__pks)
#define   ctrace(__fnc)		_ctrace(__fnc)
#define    fixme(__pks)		_fixme(__pks)
#define     todo(__pks)		_todo(__pks)
#define   printd(__pks)		_printd(__pks)
#define    swerr()		_swerr()
#define   pswerr(__pks)		_pswerr(__pks)

#undef STATIC
#define STATIC static
#undef INLINE
#ifdef __inline
#define INLINE __inline
#else
#ifdef __inline__
#define INLINE __inline__
#else
#define INLINE inline
#endif
#endif

#endif				/* defined _SAFE */

#endif				/* defined _TEST */

#endif				/* defined _DEBUG */

#endif				/* defined LFS */

#endif				/* __OS7_DEBUG_H__ */
