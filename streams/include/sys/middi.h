/*****************************************************************************

 @(#) $Id: middi.h,v 0.9.2.1 2005/07/01 20:16:40 brian Exp $

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

 Last Modified $Date: 2005/07/01 20:16:40 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: middi.h,v $
 Revision 0.9.2.1  2005/07/01 20:16:40  brian
 - added and updated manpages for some Mentat compatibility

 *****************************************************************************/

#ifndef __SYS_MIDDI_H__
#define __SYS_MIDDI_H__

#ident "@(#) $RCSfile: middi.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2005 OpenSS7 Corporation."

#ifndef __KERNEL__
#error "Do not use kernel headers for user space programs"
#endif				/* __KERNEL__ */

#ifndef __MI_EXTERN_INLINE
#define __MI_EXTERN_INLINE extern __inline__
#endif				/* __AIX_EXTERN_INLINE */

#include <sys/strconf.h>

#ifndef _MI_SOURCE
#warning "_MI_SOURCE not defined but middi.h included"
#endif

#if defined(CONFIG_STREAMS_COMPAT_MI) || defined(CONFIG_STREAMS_COMPAT_MI_MODULE)

#ifndef dev_t
#define dev_t __streams_dev_t
#endif

__MI_EXTERN_INLINE void *mi_alloc(size_t size, unsigned int pri)
{
	size_t *sp;
	if ((sp = kmem_alloc(size, KM_NOSLEEP)))
		*sp++ = size;
	return (sp);
}

__MI_EXTERN_INLINE *mi_alloc_sleep(size_t size, unsigned int pri)
{
	size_t *sp;
	if ((sp = kmem_alloc(size, KM_SLEEP)))
		*sp++ = size;
	return (sp);
}
__MI_EXTERN_INLINE caddr_t mi_zalloc(size_t size)
{
	caddr_t memp;
	if ((memp = mi_alloc(size, BPRI_LO)))
		bzero(memp, size);
	return (memp);
}
__MI_EXTERN_INLINE caddr_t mi_zalloc_sleep(size_t size)
{
	caddr_t memp;
	if ((memp = mi_alloc_sleep(size, BPRI_LO)))
		bzero(memp, size);
	return (memp);
}
__MI_EXTERN_INLINE void mi_free(void *ptr)
{
	size_t *sp;
	if ((sp = ptr)) {
		size_t size = *sp--;
		kmem_free(sp, size);
	}
}

extern caddr_t mi_open_alloc(size_t size);
extern caddr_t mi_open_alloc_sleep(size_t size);

#if 0 /* tough ones for now... */
extern int mi_mpprintf(mblk_t *mp, char *fmt, ...) __attribute__ ((format(printf, 2, 3)));
extern int mi_mpprintf_nr(mblk_t *mp, char *fmt, ...) __attribute__ ((format(printf, 2, 3)));
#endif

extern int mi_open_link(void **mi_head, caddr_t ptr, dev_t *devp, int flag, int sflag, cred_t *credp);
extern void mi_close_unlink(void **mi_head, caddr_t ptr);
extern void mi_close_free(caddr_t ptr);

extern caddr_t mi_first_ptr(void **mi_head);
extern caddr_t mi_first_dev_ptr(void **mi_head);
extern caddr_t mi_next_dev_ptr(void **mi_head, caddr_t ptr);

extern int mi_sprintf_putc(char *cookie, int ch);

extern int mi_strcmp(const caddr_t cp1, const caddr_t cp2);
extern int mi_strlen(const caddr_t str);

extern int mi_strlog(queue_t *q, char level, ushort flags, char *fmt,
		     ...) __attribute___((format(printf, 4, 5)));

extern long mi_strol(const caddr_t str, caddr_t *ptr, int base);

extern void mi_timer_move(queue_t *q, mblk_t *mp);	/* also mi_timer_q_switch */
extern void mi_timer_stop(mblk_t *mp);	/* also called mi_timer_cancel */

/* ------------------------- */

extern void mi_copyin(queue_t *q, mblk_t *mp, caddr_t uaddr, size_t len);
extern void mi_copyin_n(queue_t *q, mblk_t *mp, size_t offset, size_t len);
extern void mi_copyout(queue_t *q, mblk_t *mp);
extern mblk_t *mi_copyout_alloc(queue_t *q, mblk_t *mp, caddr_t uaddr, size_t len,
				int free_on_error);
extern void mi_copy_done(queue_t *q, mblk_t *mp, int err);
extern int mi_copy_state(queue_t *q, mblk_t *mp, mblk_t **mpp);

extern int mi_open_comm(void **mi_head, size_t size, queue_t *q, dev_t *devp, int flag, int sflag,
			cred_t *credp);
extern int mi_close_comm(void **mi_head, queue_t *q);
extern caddr_t mi_next_ptr(void **mi_head, caddr_t ptr);

extern uint8_t *mi_offset_param(mblk_t *mp, size_t offset, size_t len);
extern uint8_t *mi_offset_paramc(mblk_t *mp, size_t offset, size_t len);

extern int mi_set_sth_hiwat(queue_t *q, size_t size);
extern int mi_set_sth_lowat(queue_t *q, size_t size);
extern int mi_set_sth_maxblk(queue_t *q, ssize_t size);
extern int mi_set_sth_copyopt(queue_t *q, int copyopt);
extern int mi_set_sth_wroff(queue_t *q, size_t size);

extern int mi_sprintf(char *buf, char *fmt, ...) __attribute__ ((format(printf, 2, 3)));

extern void mi_timer(queue_t *q, mblk_t *mp, clock_t tim);
extern mblk_t *mi_timer_alloc(queue_t *q, mblk_t *mp, clock_t tim);
extern void mi_timer_free(mblk_t *mp);
extern int mi_timer_valid(mblk_t *mp);

#elif defined(_MI_SOURCE)
#warning "_MI_SOURCE defined but not CONFIG_STREAMS_COMPAT_MI"
#endif

#endif				/* __SYS_MIDDI_H__ */
