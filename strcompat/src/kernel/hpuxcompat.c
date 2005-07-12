/*****************************************************************************

 @(#) $RCSfile: hpuxcompat.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2005/07/12 13:54:45 $

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

 Last Modified $Date: 2005/07/12 13:54:45 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: hpuxcompat.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2005/07/12 13:54:45 $"

static char const ident[] =
    "$RCSfile: hpuxcompat.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2005/07/12 13:54:45 $";

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

#define __HPUX_EXTERN_INLINE inline

#define _HPUX_SOURCE

#include "sys/os7/compat.h"

#define HPUXCOMP_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define HPUXCOMP_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define HPUXCOMP_REVISION	"LfS $RCSfile: hpuxcompat.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2005/07/12 13:54:45 $"
#define HPUXCOMP_DEVICE		"HP-UX 11i v2 Compatibility"
#define HPUXCOMP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define HPUXCOMP_LICENSE	"GPL"
#define HPUXCOMP_BANNER		HPUXCOMP_DESCRIP	"\n" \
				HPUXCOMP_COPYRIGHT	"\n" \
				HPUXCOMP_REVISION	"\n" \
				HPUXCOMP_DEVICE		"\n" \
				HPUXCOMP_CONTACT	"\n"
#define HPUXCOMP_SPLASH		HPUXCOMP_DEVICE		" - " \
				HPUXCOMP_REVISION	"\n"

#ifdef CONFIG_STREAMS_COMPAT_HPUX_MODULE
MODULE_AUTHOR(HPUXCOMP_CONTACT);
MODULE_DESCRIPTION(HPUXCOMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(HPUXCOMP_DEVICE);
MODULE_LICENSE(HPUXCOMP_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-hpuxcompat");
#endif
#endif

static lock_t sleep_lock = SPIN_LOCK_UNLOCKED;
/**
 *  streams_get_sleep_lock: - acquire the global sleep lock
 *  @event:	the event which will be later passed to sleep
 *
 *  streams_get_sleep_lock() provides access to a global spinlock_t that may be used
 *  by all threads entering a wait queue to avoid race conditions between
 *  threads entering the wait queue.
 *
 *  Return Value:streams_get_sleep_lock() returns a pointer to the global sleep lock.
 */
lock_t *streams_get_sleep_lock(caddr_t event)
{
	(void) event;
	return &sleep_lock;
}
EXPORT_SYMBOL(streams_get_sleep_lock);	/* hpux/ddi.h */

lock_t *get_sleep_lock(caddr_t event) __attribute__((alias("streams_get_sleep_lock")));
EXPORT_SYMBOL(get_sleep_lock);	/* hpux/ddi.h */

#if LFS
__HPUX_EXTERN_INLINE void streams_put(streams_put_t func, queue_t *q, mblk_t *mp, void *priv);
EXPORT_SYMBOL(streams_put);	/* hpux/ddi.h */
#endif

#ifdef CONFIG_STREAMS_COMPAT_HPUX_MODULE
static
#endif
int __init hpuxcomp_init(void)
{
#ifdef CONFIG_STREAMS_COMPAT_HPUX_MODULE
	printk(KERN_INFO HPUXCOMP_BANNER);
#else
	printk(KERN_INFO HPUXCOMP_SPLASH);
#endif
	return (0);
}
#ifdef CONFIG_STREAMS_COMPAT_HPUX_MODULE
static
#endif
void __exit hpuxcomp_exit(void)
{
	return;
}

#ifdef CONFIG_STREAMS_COMPAT_HPUX_MODULE
module_init(hpuxcomp_init);
module_exit(hpuxcomp_exit);
#endif
