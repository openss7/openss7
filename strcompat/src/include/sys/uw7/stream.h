/*****************************************************************************

 @(#) $Id: stream.h,v 0.9.2.4 2006/06/22 13:11:31 brian Exp $

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

 Last Modified $Date: 2006/06/22 13:11:31 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: stream.h,v $
 Revision 0.9.2.4  2006/06/22 13:11:31  brian
 - more optmization tweaks and fixes

 Revision 0.9.2.3  2005/12/28 09:51:49  brian
 - remove warnings on FC4 compile

 Revision 0.9.2.2  2005/07/14 22:04:07  brian
 - updates for check pass and header splitting

 Revision 0.9.2.1  2005/07/12 13:54:45  brian
 - changes for os7 compatibility and check pass

 *****************************************************************************/

#ifndef __SYS_UW7_STREAM_H__
#define __SYS_UW7_STREAM_H__

#ident "@(#) $RCSfile: stream.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2005 OpenSS7 Corporation."

#ifndef __SYS_STREAM_H__
#warning "Do not include sys/uw7/stream.h directly, include sys/stream.h instead."
#endif

#ifndef __KERNEL__
#error "Do not include kernel header files in user space programs."
#endif

#ifndef __EXTERN_INLINE
#define __EXTERN_INLINE extern __inline__
#endif

#ifndef __UW7_EXTERN_INLINE
#define __UW7_EXTERN_INLINE __EXTERN_INLINE
#endif

#ifndef _UW7_SOURCE
#warning "_UW7_SOURCE not defined but UW7 stream.h included."
#endif

#include <sys/strcompat/config.h>

#if defined CONFIG_STREAMS_COMPAT_UW7 || defined CONFIG_STREAMS_COMPAT_UW7_MODULE

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

#ifdef LFS
/* already defined by LiS */
extern mblk_t *allocb_physreq(size_t size, uint priority, physreq_t * prp);
#endif
extern mblk_t *msgphysreq(mblk_t *mp, physreq_t * prp);
extern mblk_t *msgpullup_physreq(mblk_t *mp, size_t len, physreq_t * prp);
extern mblk_t *msgscgth(mblk_t *mp, physreq_t * prp, scgth_t * sgp);

#if 0
/* not implemented yet */
int strioccall(int (*func) (void *), void *arg, uint iocid, queue_t *q);
#endif

#else
#ifdef _UW7_SOURCE
#warning "_UW7_SOURCE defined by not CONFIG_STREAMS_COMPAT_UW7"
#endif
#endif

#endif				/* __SYS_UW7_STREAM_H__ */
