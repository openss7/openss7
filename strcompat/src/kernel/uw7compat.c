/*****************************************************************************

 @(#) $RCSfile: uw7compat.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2006/07/07 20:49:55 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2006/07/07 20:49:55 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: uw7compat.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2006/07/07 20:49:55 $"

static char const ident[] =
    "$RCSfile: uw7compat.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2006/07/07 20:49:55 $";

/* 
 *  This is my solution for those who don't want to inline GPL'ed functions or
 *  who don't use optimizations when compiling or specifies
 *  -fnoinline-functions or something of the like.  This file implements all
 *  of the extern inlines from the header files by just including the header
 *  files with the functions declared 'inline' instead of 'extern inline'.
 *
 *  There are implemented here in a separate object, out of the way of the
 *  modules that don't use them.
 */

#define __UW7_EXTERN_INLINE __inline__ streamscall __unlikely

#define _UW7_SOURCE

#include "sys/os7/compat.h"

#define UW7COMP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define UW7COMP_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define UW7COMP_REVISION	"LfS $RCSfile: uw7compat.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2006/07/07 20:49:55 $"
#define UW7COMP_DEVICE		"UnixWare(R) 7.1.3 Compatibility"
#define UW7COMP_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define UW7COMP_LICENSE		"GPL"
#define UW7COMP_BANNER		UW7COMP_DESCRIP		"\n" \
				UW7COMP_COPYRIGHT	"\n" \
				UW7COMP_REVISION	"\n" \
				UW7COMP_DEVICE		"\n" \
				UW7COMP_CONTACT		"\n"
#define UW7COMP_SPLASH		UW7COMP_DEVICE		" - " \
				UW7COMP_REVISION	"\n"

#ifdef CONFIG_STREAMS_COMPAT_UW7_MODULE
MODULE_AUTHOR(UW7COMP_CONTACT);
MODULE_DESCRIPTION(UW7COMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(UW7COMP_DEVICE);
MODULE_LICENSE(UW7COMP_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-uw7compat");
#endif
#endif

/* don't use these - these are fakes */
#undef allocb_physreq
/**
 *  allocb_physreq:	- allocate a message block with physical requirements
 *  @size:		number of bytes to allocate
 *  @priority:		priority of the allocation
 *  @physreq_ptr:	physical requirements of the message block
 */
mblk_t *
allocb_physreq(size_t size, uint priority, physreq_t * prp)
{
#ifdef LIS
	return lis_allocb_physreq(size, priority, prp, __FILE__, __LINE__);
#endif
#ifdef LFS
	if (prp->phys_align > 8)
		return (NULL);
	if (prp->phys_boundary != 0)
		return (NULL);
	if (prp->phys_dmasize != 0)
		return (NULL);
	if (prp->phys_flags & PREQ_PHYSCONTIG)
		return (NULL);
	return (allocb(size, priority));
#endif
}

EXPORT_SYMBOL_NOVERS(allocb_physreq);	/* uw7/ddi.h */
mblk_t *
msgphysreq(mblk_t *mp, physreq_t * prp)
{
	if (prp->phys_align > 8)
		return (NULL);
	if (prp->phys_boundary != 0)
		return (NULL);
	if (prp->phys_dmasize != 0)
		return (NULL);
	if (prp->phys_flags & PREQ_PHYSCONTIG)
		return (NULL);
	return (mp);
}

EXPORT_SYMBOL_NOVERS(msgphysreq);	/* uw7/ddi.h */
mblk_t *
msgpullup_physreq(mblk_t *mp, size_t len, physreq_t * prp)
{
	if (prp->phys_align > 8)
		return (NULL);
	if (prp->phys_boundary != 0)
		return (NULL);
	if (prp->phys_dmasize != 0)
		return (NULL);
	if (prp->phys_flags & PREQ_PHYSCONTIG)
		return (NULL);
	return msgpullup(mp, len);
}

EXPORT_SYMBOL_NOVERS(msgpullup_physreq);	/* uw7/ddi.h */
mblk_t *
msgscgth(mblk_t *mp, physreq_t * prp, scgth_t * sgp)
{
	return (NULL);
}

EXPORT_SYMBOL_NOVERS(msgscgth);	/* uw7/ddi.h */

int printf_UW7(char *fmt, ...) __attribute__ ((format(printf, 1, 2)));
int
printf_UW7(char *fmt, ...)
{
	va_list args;
	int n;
	char printf_buf[1024];

	va_start(args, fmt);
	n = vsnprintf(printf_buf, sizeof(printf_buf), fmt, args);
	va_end(args);
	printk("%s", printf_buf);
	return (n);
}

EXPORT_SYMBOL_NOVERS(printf_UW7);	/* uw7/ddi.h */

__UW7_EXTERN_INLINE void ATOMIC_INT_ADD(atomic_int_t * counter, int value);

EXPORT_SYMBOL_NOVERS(ATOMIC_INT_ADD);	/* uw7/ddi.h */
__UW7_EXTERN_INLINE atomic_int_t *ATOMIC_INT_ALLOC(int flag);

EXPORT_SYMBOL_NOVERS(ATOMIC_INT_ALLOC);	/* uw7/ddi.h */
__UW7_EXTERN_INLINE void ATOMIC_INT_DEALLOC(atomic_int_t * counter);

EXPORT_SYMBOL_NOVERS(ATOMIC_INT_DEALLOC);	/* uw7/ddi.h */
__UW7_EXTERN_INLINE int ATOMIC_INT_DECR(atomic_int_t * counter);

EXPORT_SYMBOL_NOVERS(ATOMIC_INT_DECR);	/* uw7/ddi.h */
__UW7_EXTERN_INLINE void ATOMIC_INT_INCR(atomic_int_t * counter);

EXPORT_SYMBOL_NOVERS(ATOMIC_INT_INCR);	/* uw7/ddi.h */
__UW7_EXTERN_INLINE void ATOMIC_INT_INIT(atomic_int_t * counter, int value);

EXPORT_SYMBOL_NOVERS(ATOMIC_INT_INIT);	/* uw7/ddi.h */
__UW7_EXTERN_INLINE int ATOMIC_INT_READ(atomic_int_t * counter);

EXPORT_SYMBOL_NOVERS(ATOMIC_INT_READ);	/* uw7/ddi.h */
__UW7_EXTERN_INLINE void ATOMIC_INT_SUB(atomic_int_t * counter, int value);

EXPORT_SYMBOL_NOVERS(ATOMIC_INT_SUB);	/* uw7/ddi.h */
__UW7_EXTERN_INLINE void ATOMIC_INT_WRITE(atomic_int_t * counter, int value);

EXPORT_SYMBOL_NOVERS(ATOMIC_INT_WRITE);	/* uw7/ddi.h */

#ifdef CONFIG_STREAMS_COMPAT_UW7_MODULE
static
#endif
int __init
uw7comp_init(void)
{
#ifdef CONFIG_STREAMS_COMPAT_UW7_MODULE
	printk(KERN_INFO UW7COMP_BANNER);
#else
	printk(KERN_INFO UW7COMP_SPLASH);
#endif
	return (0);
}

#ifdef CONFIG_STREAMS_COMPAT_UW7_MODULE
static
#endif
void __exit
uw7comp_exit(void)
{
	return;
}

#ifdef CONFIG_STREAMS_COMPAT_UW7_MODULE
module_init(uw7comp_init);
module_exit(uw7comp_exit);
#endif
