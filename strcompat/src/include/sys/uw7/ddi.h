/*****************************************************************************

 @(#) $Id: ddi.h,v 0.9.2.5 2005/04/23 07:31:52 brian Exp $

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

 Last Modified $Date: 2005/04/23 07:31:52 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_UW7DDI_H__
#define __SYS_UW7DDI_H__

#ident "@(#) $RCSfile: ddi.h,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2005/04/23 07:31:52 $"

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __UW7_EXTERN_INLINE
#define __UW7_EXTERN_INLINE extern __inline__
#endif				/* __UW7_EXTERN_INLINE */

#include <sys/strconf.h>

#ifndef _UW7_SOURCE
#warning "_UW7_SOURCE not defined but uw7ddi.h,v included"
#endif

#if defined(CONFIG_STREAMS_COMPAT_UW7) || defined(CONFIG_STREAMS_COMPAT_UW7_MODULE)

#ifndef _SVR4_SOURCE
#define _SVR4_SOURCE
#endif
#include <sys/svr4ddi.h>

typedef unsigned long paddr_t;
typedef struct physreq {
	paddr_t phys_align;
	paddr_t phys_boundary;
	unsigned char phys_dmasize;
	/* 0 - DMA not used */
	/* 24 - ISA device */
	/* 32 - PCI, EISA, MCA device */
	/* 64 - 64-bit device */
	unsigned char phys_max_scgth;
	unsigned char phys_flags;
#define PREQ_PHYSCONTIG	    0x01
} physreq_t;

typedef struct {
	uint32_t sg_base;
	uint32_t sg_size;
} scgth_el32_t;

typedef struct {
	uint32_t sg_base[2];
	uint32_t sg_size;
	uint32_t _sg_reserved;
} scgth_el64_t;

typedef struct {
	union {
		scgth_el32_t el32;
		scgth_el64_t el64;
	} sg_elem;
	union {
		scgth_el32_t el32;
		scgth_el64_t el64;
	} sg_el_addr;
	unsigned char sg_nelem;
	unsigned char sg_format;
} scgth_t;

extern mblk_t *allocb_physreq(size_t size, uint priority, physreq_t * prp);
extern mblk_t *msgphysreq(mblk_t *mp, physreq_t * prp);
extern mblk_t *msgpullup_physreq(mblk_t *mp, size_t len, physreq_t * prp);
extern mblk_t *msgscgth(mblk_t *mp, physreq_t * prp, scgth_t * sgp);

int strioccall(int (*func) (void *), void *arg, uint iocid, queue_t *q);

__UW7_EXTERN_INLINE major_t getemajor(dev_t dev)
{
	return (getmajor(dev) + MAJOR(getminor(dev)));
}
__UW7_EXTERN_INLINE minor_t geteminor(dev_t dev)
{
	return (MINOR(getminor(dev)));
}
__UW7_EXTERN_INLINE major_t emajor(dev_t dev)
{
	return (getmajor(dev) + MAJOR(getminor(dev)));
}
__UW7_EXTERN_INLINE minor_t eminor(dev_t dev)
{
	return (MINOR(getminor(dev)));
}

extern int etoimajor(major_t emajor);
extern int itoemajor(major_t imajor, int prevemaj);

int printf_UW7(char *fmt, ...) __attribute__ ((format(printf, 1, 2)));

typedef atomic_t atomic_int_t;

__UW7_EXTERN_INLINE void ATOMIC_INT_ADD(atomic_int_t * counter, int value)
{
	atomic_add(value, counter);
}
__UW7_EXTERN_INLINE atomic_int_t *ATOMIC_INT_ALLOC(int flag)
{
	atomic_int_t *counter;
	if ((counter = kmem_alloc(sizeof(*counter), flag)))
		*counter = (atomic_int_t) ATOMIC_INIT(0);
	return (counter);
}
__UW7_EXTERN_INLINE void ATOMIC_INT_DEALLOC(atomic_int_t * counter)
{
	kmem_free(counter, sizeof(*counter));
}
__UW7_EXTERN_INLINE int ATOMIC_INT_DECR(atomic_int_t * counter)
{
	return atomic_dec_and_test(counter);
}
__UW7_EXTERN_INLINE void ATOMIC_INT_INCR(atomic_int_t * counter)
{
	atomic_inc(counter);
}
__UW7_EXTERN_INLINE void ATOMIC_INT_INIT(atomic_int_t * counter, int value)
{
	atomic_set(counter, value);
}
__UW7_EXTERN_INLINE int ATOMIC_INT_READ(atomic_int_t * counter)
{
	return atomic_read(counter);
}
__UW7_EXTERN_INLINE void ATOMIC_INT_SUB(atomic_int_t * counter, int value)
{
	atomic_sub(value, counter);
}
__UW7_EXTERN_INLINE void ATOMIC_INT_WRITE(atomic_int_t * counter, int value)
{
	atomic_set(counter, value);
}

#elif defined(_UW7_SOURCE)
#warning "_UW7_SOURCE defined but not CONFIG_STREAMS_COMPAT_UW7"
#endif				/* CONFIG_STREAMS_COMPAT_UW7 */

#endif				/* __SYS_UW7DDI_H__ */
