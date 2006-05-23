/*****************************************************************************

 @(#) $Id: ddi.h,v 0.9.2.11 2006/05/23 10:44:04 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>

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

 Last Modified $Date: 2006/05/23 10:44:04 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ddi.h,v $
 Revision 0.9.2.11  2006/05/23 10:44:04  brian
 - mark normal inline functions for unlikely text section

 Revision 0.9.2.10  2005/12/28 09:51:48  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.9  2005/07/18 12:25:40  brian
 - standard indentation

 Revision 0.9.2.8  2005/07/15 23:09:10  brian
 - checking in for sync

 Revision 0.9.2.7  2005/07/14 22:03:52  brian
 - updates for check pass and header splitting

 Revision 0.9.2.6  2005/07/12 19:15:47  brian
 - updates and check passes

 Revision 0.9.2.5  2005/07/12 13:54:43  brian
 - changes for os7 compatibility and check pass

 Revision 0.9.2.4  2005/07/12 08:42:42  brian
 - changes for check pass

 Revision 0.9.2.3  2005/07/05 22:46:05  brian
 - change for strcompat package

 Revision 0.9.2.2  2005/07/04 19:29:12  brian
 - first cut at streams compatibility package

 Revision 0.9.2.1  2005/07/03 17:41:12  brian
 - separated out MPS compatibility module

 Revision 0.9.2.1  2005/07/01 20:16:40  brian
 - added and updated manpages for some Mentat compatibility

 *****************************************************************************/

#ifndef __SYS_MPS_DDI_H__
#define __SYS_MPS_DDI_H__

#ident "@(#) $RCSfile: ddi.h,v $ $Name:  $($Revision: 0.9.2.11 $) Copyright (c) 2001-2005 OpenSS7 Corporation."

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __MPS_EXTERN_INLINE
#define __MPS_EXTERN_INLINE extern __inline__
#endif				/* __AIX_EXTERN_INLINE */

#ifndef _MPS_SOURCE
#warning "_MPS_SOURCE not defined but MPS ddi.h included"
#endif

#ifndef _SVR4_SOURCE
#define _SVR4_SOURCE
#endif
#include <sys/svr4/ddi.h>	/* for pl_t */

#if defined(CONFIG_STREAMS_COMPAT_MPS) || defined(CONFIG_STREAMS_COMPAT_MPS_MODULE)

__MPS_EXTERN_INLINE __unlikely int
mi_bcmp(const void *s1, const void *s2, size_t len)
{
	return bcmp(s1, s2, len);
}

/*
 *  Memory allocation functions.
 */
__MPS_EXTERN_INLINE __unlikely void *
mi_alloc(size_t size, unsigned int pri)
{
	size_t *sp;

	if ((sp = kmem_alloc(size, KM_NOSLEEP)))
		*sp++ = size;
	return (sp);
}

__MPS_EXTERN_INLINE __unlikely void *
mi_alloc_sleep(size_t size, unsigned int pri)
{
	size_t *sp;

	if ((sp = kmem_alloc(size, KM_SLEEP)))
		*sp++ = size;
	return (sp);
}
__MPS_EXTERN_INLINE __unlikely caddr_t
mi_zalloc(size_t size)
{
	caddr_t memp;

	if ((memp = mi_alloc(size, 0 /* BPRI_LO */ )))
		bzero(memp, size);
	return (memp);
}
__MPS_EXTERN_INLINE __unlikely caddr_t
mi_zalloc_sleep(size_t size)
{
	caddr_t memp;

	if ((memp = mi_alloc_sleep(size, 0 /* BPRI_LO */ )))
		bzero(memp, size);
	return (memp);
}
__MPS_EXTERN_INLINE __unlikely void
mi_free(void *ptr)
{
	size_t *sp;

	if ((sp = ptr)) {
		size_t size = *sp--;

		(void) size;	/* LiS ignores size */
		kmem_free(sp, size);
	}
}

/*
 *  System wrapper functions.
 */
extern int mi_sprintf(char *buf, char *fmt, ...) __attribute__ ((format(printf, 2, 3)));

#if 0
/* not implemented yet */
extern int mi_sprintf_putc(char *cookie, int ch);
#endif

extern int mi_strcmp(const caddr_t cp1, const caddr_t cp2);
extern int mi_strlen(const caddr_t str);
extern long mi_strtol(const caddr_t str, caddr_t *ptr, int base);

/*
 *  Some internals showing.
 */
extern void mps_intr_disable(pl_t * plp);
extern void mps_intr_enable(pl_t pl);

#else
#ifdef _MPS_SOURCE
#warning "_MPS_SOURCE defined but not CONFIG_STREAMS_COMPAT_MPS"
#endif
#endif

#endif				/* __SYS_MPS_DDI_H__ */
