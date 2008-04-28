/*****************************************************************************

 @(#) $RCSfile: maccompat.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2008-04-28 16:47:13 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 Last Modified $Date: 2008-04-28 16:47:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: maccompat.c,v $
 Revision 0.9.2.22  2008-04-28 16:47:13  brian
 - updates for release

 Revision 0.9.2.21  2007/08/15 05:33:09  brian
 - GPLv3 updates

 Revision 0.9.2.20  2007/08/12 15:51:19  brian
 - header and extern updates, GPLv3, 3 new lock functions

 Revision 0.9.2.19  2007/07/14 01:35:41  brian
 - make license explicit, add documentation

 Revision 0.9.2.18  2007/03/02 10:04:06  brian
 - updates to common build process and versions for all exported symbols

 Revision 0.9.2.17  2006/11/03 10:39:28  brian
 - updated headers, correction to mi_timer_expiry type

 *****************************************************************************/

#ident "@(#) $RCSfile: maccompat.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2008-04-28 16:47:13 $"

static char const ident[] =
    "$RCSfile: maccompat.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2008-04-28 16:47:13 $";

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

#define __MAC_EXTERN_INLINE __inline__ streamscall __unlikely
#define __MAX_EXTERN streamscall

#define _MAC_SOURCE

#include "sys/os7/compat.h"

#define MACCOMP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define MACCOMP_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define MACCOMP_REVISION	"LfS $RCSfile: maccompat.c,v $ $Name:  $($Revision: 0.9.2.22 $) $Date: 2008-04-28 16:47:13 $"
#define MACCOMP_DEVICE		"Mac OpenTransport Version 1.5r2 Compatibility"
#define MACCOMP_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define MACCOMP_LICENSE		"GPL"
#define MACCOMP_BANNER		MACCOMP_DESCRIP		"\n" \
				MACCOMP_COPYRIGHT	"\n" \
				MACCOMP_REVISION	"\n" \
				MACCOMP_DEVICE		"\n" \
				MACCOMP_CONTACT		"\n"
#define MACCOMP_SPLASH		MACCOMP_DEVICE		" - " \
				MACCOMP_REVISION	"\n"

#ifdef CONFIG_STREAMS_COMPAT_MAC_MODULE
MODULE_AUTHOR(MACCOMP_CONTACT);
MODULE_DESCRIPTION(MACCOMP_DESCRIP);
MODULE_SUPPORTED_DEVICE(MACCOMP_DEVICE);
MODULE_LICENSE(MACCOMP_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-maccompat");
#endif
#endif

/*
 *  Note that mi_bufcall, mi_close_comm, mi_next_ptr, mi_open_comm and
 *  mi_prev_ptr are in aixcompat.
 */
/*
 *  ALLOCBI
 *  -------------------------------------------------------------------------
 */
/*
 *  ESBALLOCA
 *  -------------------------------------------------------------------------
 */
/*
 *  MPNOTIFY
 *  -------------------------------------------------------------------------
 */
/*
 *  PUTHERE
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_ALLOCQ
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_BCMP
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_BUFCALL
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_CLOSE_COMM
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_CLOSE_DETACHED
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_COPY_CASE
 *  -------------------------------------------------------------------------
 *  implemented in the mpscompat module, just a declaration here
 */
/*
 *  MI_COPY_DONE
 *  -------------------------------------------------------------------------
 *  implemented in the mpscompat module, just a declaration here
 */
/*
 *  MI_COPYIN
 *  -------------------------------------------------------------------------
 *  implemented in the mpscompat module, just a declaration here
 */
extern void mi_copyin(queue_t *q, mblk_t *mp, caddr_t uaddr, size_t len);

/*
 *  MI_COPYOUT
 *  -------------------------------------------------------------------------
 *  implemented in the mpscompat module, just a declaration here
 */
extern void mi_copyout(queue_t *q, mblk_t *mp);

/*
 *  MI_COPYOUT_ALLOC
 *  -------------------------------------------------------------------------
 *  implemented in the mpscompat module, just a declaration here
 */
extern mblk_t *mi_copyout_alloc(queue_t *q, mblk_t *mp, caddr_t uaddr, size_t len,
				int free_on_error);
/*
 *  MI_COPY_SET_RVAL
 *  -------------------------------------------------------------------------
 *  implemented in the mpscompat module, just a declaration here
 */
/*
 *  MI_COPY_STATE
 *  -------------------------------------------------------------------------
 *  implemented in the mpscompat module, just a declaration here
 */
/*
 *  MI_DETACH
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_FREEQ
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_NEXT_PTR
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_OFFSET_PARAM
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_OFFSET_PARAMC
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_OPEN_COMM
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_OPEN_DETACHED
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_PREV_PTR
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_REALLOCB
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_RESUE_PROTO
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_SET_STH_HIWAT
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_SET_STH_LOWAT
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_SET_STH_MAXBLK
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_SET_STH_WROFF
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_SPRINTF
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_TIMER
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_TIMER_ALLOC
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_TIMER_CANCEL
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_TIMER_FREE
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_TIMER_Q_SWITCH
 *  -------------------------------------------------------------------------
 */
/*
 *  MI_TIMER_VALID
 *  -------------------------------------------------------------------------
 */
/*
 *  MPS_BECOME_WRITER
 *  -------------------------------------------------------------------------
 */
/*
 *  MPS_INTR_DISABLE
 *  -------------------------------------------------------------------------
 */
/*
 *  MPS_INTR_ENABLE
 *  -------------------------------------------------------------------------
 */
/*
 *  MPNOTIFY
 *  -------------------------------------------------------------------------
 */
STATIC streams_inline streams_fastcall __hot_out struct mdbblock *
mb_to_mdb(register mblk_t *mb)
{
	return ((struct mdbblock *) mb);
}
STATIC streams_inline streams_fastcall __hot_in mblk_t *
db_to_mb(register dblk_t *db)
{
	return ((mblk_t *) ((struct mbinfo *) db - 1));
}
STATIC streams_inline streams_fastcall __hot_in unsigned char *
db_to_buf(register dblk_t *db)
{
	return (&mb_to_mdb(db_to_mb(db))->databuf[0]);
}

static void streamscall
mpnotify_func(caddr_t arg)
{
	dblk_t *db = (dblk_t *) arg;

	/* first call notification function */
	(*db->db_frtnp[1].free_func) (db->db_frtnp[1].free_arg);
	/* restore pointer */
	if (db->db_base != db_to_buf(db)) {
		db->db_frtnp = (frtn_t *) (db + 1);
		/* call free function */
		(*db->db_frtnp[0].free_func) (db->db_frtnp[0].free_arg);
	} else
		db->db_frtnp = NULL;
	return;
}

__MAC_EXTERN int
mpnotify(mblk_t *mp, mpnotify_func_t func, caddr_t arg)
{
	frtn_t *frtnp;

	if ((frtnp = kmem_alloc(2 * sizeof(frtn_t), KM_NOSLEEP))) {
		frtnp[0].free_func = &mpnotify_func;
		frtnp[0].free_arg = (void *) mp->b_datap;
		frtnp[1].free_func = func;
		frtnp[1].free_arg = arg;
		mp->b_datap->db_frtnp = frtnp;
		return (1);
	}
	return (0);
}

EXPORT_SYMBOL(mpnotify);

#ifdef CONFIG_STREAMS_COMPAT_MAC_MODULE
static
#endif
int __init
maccomp_init(void)
{
#ifdef CONFIG_STREAMS_COMPAT_MAC_MODULE
	printk(KERN_INFO MACCOMP_BANNER);
#else
	printk(KERN_INFO MACCOMP_SPLASH);
#endif
	return (0);
}

#ifdef CONFIG_STREAMS_COMPAT_MAC_MODULE
static
#endif
void __exit
maccomp_exit(void)
{
	return;
}

#ifdef CONFIG_STREAMS_COMPAT_MAC_MODULE
module_init(maccomp_init);
module_exit(maccomp_exit);
#endif
