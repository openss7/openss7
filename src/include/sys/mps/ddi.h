/*****************************************************************************

 @(#) $Id: ddi.h,v 1.1.2.1 2009-06-21 11:26:47 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2009-06-21 11:26:47 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ddi.h,v $
 Revision 1.1.2.1  2009-06-21 11:26:47  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SYS_MPS_DDI_H__
#define __SYS_MPS_DDI_H__

#ident "@(#) $RCSfile: ddi.h,v $ $Name:  $($Revision: 1.1.2.1 $) Copyright (c) 2008-2009 Monavacon Limited."

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __MPS_EXTERN_INLINE
#define __MPS_EXTERN_INLINE __EXTERN_INLINE streamscall
#endif				/* __AIX_EXTERN_INLINE */

#ifndef __MPS_EXTERN
#define __MPS_EXTERN extern streamscall
#endif

#ifndef _MPS_SOURCE
#warning "_MPS_SOURCE not defined but MPS ddi.h included"
#endif

#ifndef _SVR4_SOURCE
#define _SVR4_SOURCE
#endif
#include <sys/svr4/ddi.h>	/* for pl_t */

#if defined(CONFIG_STREAMS_COMPAT_MPS) || defined(CONFIG_STREAMS_COMPAT_MPS_MODULE)

static __inline__ int
mi_bcmp(const void *s1, const void *s2, size_t len)
{
	return bcmp(s1, s2, len);
}

/*
 *  Memory allocation functions.
 */
__MPS_EXTERN_INLINE void *
mi_alloc(size_t size, unsigned int pri)
{
	size_t *sp;

	if ((sp = kmem_alloc(size, KM_NOSLEEP)))
		*sp++ = size;
	return (sp);
}

__MPS_EXTERN_INLINE void *
mi_alloc_sleep(size_t size, unsigned int pri)
{
	size_t *sp;

	if ((sp = kmem_alloc(size, KM_SLEEP)))
		*sp++ = size;
	return (sp);
}
static __inline__ caddr_t
mi_zalloc(size_t size)
{
	caddr_t memp;

	if ((memp = mi_alloc(size, 0 /* BPRI_LO */ )))
		bzero(memp, size);
	return (memp);
}
static __inline__ caddr_t
mi_zalloc_sleep(size_t size)
{
	caddr_t memp;

	if ((memp = mi_alloc_sleep(size, 0 /* BPRI_LO */ )))
		bzero(memp, size);
	return (memp);
}
__MPS_EXTERN_INLINE void
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
__MPS_EXTERN int mi_sprintf(char *buf, char *fmt, ...) __attribute__ ((format(printf, 2, 3)));

#if 0
/* not implemented yet */
__MPS_EXTERN int mi_sprintf_putc(char *cookie, int ch);
#endif

__MPS_EXTERN int mi_strcmp(const caddr_t cp1, const caddr_t cp2);
__MPS_EXTERN int mi_strlen(const caddr_t str);
__MPS_EXTERN long mi_strtol(const caddr_t str, caddr_t *ptr, int base);

/*
 *  Some internals showing.
 */
__MPS_EXTERN void mps_intr_disable(pl_t * plp);
__MPS_EXTERN void mps_intr_enable(pl_t pl);

#else
#ifdef _MPS_SOURCE
#warning "_MPS_SOURCE defined but not CONFIG_STREAMS_COMPAT_MPS"
#endif
#endif

#endif				/* __SYS_MPS_DDI_H__ */
