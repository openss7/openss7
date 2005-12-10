/*****************************************************************************

 @(#) $Id: strdebug.h,v 0.9.2.27 2005/12/10 11:33:57 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2005/12/10 11:33:57 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_STREAMS_STRDEBUG_H__
#define __SYS_STREAMS_STRDEBUG_H__

#ident "@(#) $RCSfile: strdebug.h,v $ $Name:  $($Revision: 0.9.2.27 $) $Date: 2005/12/10 11:33:57 $"

#ifndef __SYS_STRDEBUG_H__
#warning "Do no include sys/streams/strdebug.h directly, include sys/strdebug.h instead."
#endif

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

/*
 *  First optimizations, then assertions.
 */

#if __GNUC__ < 3
#define __builtin_prefetch(addr, rw, level) ((void)(addr))
#endif

#if defined FASTCALL && !defined fastcall
#define fastcall FASTCALL()
#endif

#if !defined FASTCALL
#define FASTCALL(__x) __x
#define fastcall
#endif

#if !defined fastcall
#define fastcall FASTCALL()
#endif

#ifndef likely
#define likely(__exp) (__exp)
#endif

#ifndef unlikely
#define unlikely(__exp) (__exp)
#endif

#if defined(CONFIG_STREAMS_OPTIMIZE_NONE) || defined(CONFIG_STREAMS_DEBUG)

#undef prefetchw
#define prefetchw(__a) ((void)(__a))

#undef prefetch
#define prefetch(__a) ((void)(__a))

#undef likely
#define likely(__exp) (__exp)

#undef unlikely
#define unlikely(__exp) (__exp)

#define __hot
#define __hot_read
#define __hot_write
#define __hot_put
#define __hot_get
#define __hot_out
#define __hot_in
#define __unlikely

#undef STATIC
#define STATIC

#undef INLINE
#define INLINE

#undef STREAMS_FASTCALL
#define STREAMS_FASTCALL(__x) __x

#undef streams_fastcall
#define streams_fastcall STREAMS_FASTCALL()

#undef streams_inline
#define streams_inline

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE
#endif

#elif defined(CONFIG_STREAMS_OPTIMIZE_SIZE)

#undef prefetchw
#define prefetchw(__a) __builtin_prefetch((__a),1,3)

#undef prefetch
#define prefetch(__a) __builtin_prefetch((__a),0,3)

/* these don't affect size, just position */
#define __hot __attribute__((section(".text.hot")))
#define __hot_read  __attribute__((section(".text.hot.read")))
#define __hot_write __attribute__((section(".text.hot.write")))
#define __hot_put  __attribute__((section(".text.hot.put")))
#define __hot_get __attribute__((section(".text.hot.get")))
#define __unlikely __attribute__((section(".text.unlikely")))

#undef STATIC
#define STATIC static

#undef INLINE
#define INLINE

#undef STREAMS_FASTCALL
#define STREAMS_FASTCALL(__x) FASTCALL(__x)

#undef streams_fastcall
#define streams_fastcall STREAMS_FASTCALL()

#undef streams_inline
#define streams_inline

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern
#endif

#elif defined(CONFIG_STREAMS_OPTIMIZE_SPEED)

#undef prefetchw
#define prefetchw(__a) __builtin_prefetch((__a),1,3)

#undef prefetch
#define prefetch(__a) __builtin_prefetch((__a),0,3)

/* these don't affect size, just position */
#define __hot __attribute__((section(".text.hot")))
#define __hot_read  __attribute__((section(".text.hot.read")))
#define __hot_write __attribute__((section(".text.hot.write")))
#define __hot_put  __attribute__((section(".text.hot.put")))
#define __hot_get __attribute__((section(".text.hot.get")))
#define __unlikely __attribute__((section(".text.unlikely")))

#undef STATIC
#define STATIC static

#undef INLINE
#define INLINE inline

#undef STREAMS_FASTCALL
#define STREAMS_FASTCALL(__x) FASTCALL(__x)

#undef streams_fastcall
#define streams_fastcall STREAMS_FASTCALL()

#undef streams_inline
#define streams_inline inline

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE inline
#endif

#else /* defined(CONFIG_STREAMS_OPTIMIZE_NORMAL) */

#undef prefetchw
#define prefetchw(__a) __builtin_prefetch((__a),1,3)

#undef prefetch
#define prefetch(__a) __builtin_prefetch((__a),0,3)

/* these don't affect size, just position */
#define __hot __attribute__((section(".text.hot")))
#define __hot_read  __attribute__((section(".text.hot.read")))
#define __hot_write __attribute__((section(".text.hot.write")))
#define __hot_put  __attribute__((section(".text.hot.put")))
#define __hot_get __attribute__((section(".text.hot.get")))
#define __unlikely __attribute__((section(".text.unlikely")))

#undef STATIC
#define STATIC static

#undef INLINE
#define INLINE inline

#undef STREAMS_FASTCALL
#define STREAMS_FASTCALL(__x) FASTCALL(__x)

#undef streams_fastcall
#define streams_fastcall STREAMS_FASTCALL()

#undef streams_inline
#define streams_inline inline

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern inline
#endif

#endif

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

#if defined(CONFIG_STREAMS_DEBUG)

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

#elif defined(CONFIG_STREAMS_TEST)

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

#elif defined(CONFIG_STREAMS_SAFE)

#define    never()		do { *(int *)0 = 0; } while(0)
#define     rare()		_rare()
#define   seldom()		_seldom()
#define    usual(__exp)		_usual(__exp)
#define  unusual(__exp)		_unusual(__exp)
#define   normal(__exp)		_normal(__exp)
#define abnormal(__exp)		_abnormal(__exp)
#define  dassert(__exp)		_assert(__exp)
#define   assert(__exp)		{ if (unlikely(!(__exp))) *(int *)0 = 0; } while(0)
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

#else

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

#endif

#endif				/* __SYS_STREAMS_STRDEBUG_H__ */
