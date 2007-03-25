/*****************************************************************************

 @(#) $RCSfile: cdi_mod.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2007/03/25 19:00:10 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2007/03/25 19:00:10 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: cdi_mod.c,v $
 Revision 0.9.2.10  2007/03/25 19:00:10  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.9  2007/03/25 00:52:05  brian
 - synchronization updates

 Revision 0.9.2.8  2006/03/07 01:11:39  brian
 - updated headers

 *****************************************************************************/

#ident "@(#) $RCSfile: cdi_mod.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2007/03/25 19:00:10 $"

static char const ident[] = "$RCSfile: cdi_mod.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2007/03/25 19:00:10 $";

#include <sys/os7/compat.h>

#include <sys/cdi.h>

#define CDI_DESCRIP	"CDI SIGNALLING DATA LINK (SDL) STREAMS MODULE."
#define CDI_REVISION	"OpenSS7 $RCSfile: cdi_mod.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2007/03/25 19:00:10 $"
#define CDI_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define CDI_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define CDI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define CDI_LICENSE	"GPL"
#define CDI_BANNER	CDI_DESCRIP	"\n" \
			CDI_REVISION	"\n" \
			CDI_COPYRIGHT	"\n" \
			CDI_DEVICE	"\n" \
			CDI_CONTACT	"\n"
#define CDI_SPLASH	CDI_DEVICE	" - " \
			CDI_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(CDI_CONTACT);
MODULE_DESCRIPTION(CDI_DESCRIP);
MODULE_SUPPORTED_DEVICE(CDI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(CDI_LICENSE);
#endif
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-cdi_mod");
#endif
#endif				/* LINUX */

/*
 *  =======================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =======================================================================
 */
STATIC kmem_cachep_t cd_cachep = NULL;
STATIC INLINE void
cd_init_caches(void)
{
	if (!cd_cachep)
		if (!(cd_cachep = kmem_find_general_cachep(sizeof(cdi_t), GFP_ATOMIC)))
			if (!
			    (cd_cachep =
			     kmem_cache_create("cd_cachep", sizeof(cdi_t), 0, SLAB_HWCACHE_ALIGN,
					       NULL, NULL)))
				panic("%s: Cannot allocate cd_cache\n", __FUNCTION__);
	return;
}
STATIC INLINE void
cd_term_caches(void)
{
	return;
}
extern cdi_t *
cd_alloc_priv(queue_t *q)
{
	cdi_t *cd;
	if ((cd = kmem_cache_alloc(cd_cachep, GFP_ATOMIC))) {
		MOD_INC_USE_COUNT;
		bzero(cd, sizeof(*cd));
		RD(q)->q_ptr = WR(q)->q_ptr = cd;
		cd->rq = RD(q);
		cd->wq = WR(q);
		cd->version = C_VERSION;
		cd->i_state = CS_IDLE;
		cd_init_lock(q);
	}
	return (cd);
}
extern void
cd_free_priv(queue_t *q)
{
	cdi_t *cd = CD_PRIV(q);
	MOD_DEC_USE_COUNT;
	if (cd->rbid)
		unbufcall(xchg(&cd->rbid, 0));
	if (cd->wbid)
		unbufcall(xchg(&cd->wbid, 0));
	kmem_cache_free(cd_cachep, cd);
	return;
}

/*
 *  =======================================================================
 *
 *  LiS Module Initialization
 *
 *  =======================================================================
 */
STATIC INLINE void
cd_init(void)
{
	int modnum;
	unless(cd_minfo.mi_idnum, return);
	cmn_err(CE_NOTE, SDL_BANNER);	/* console splash */
	cd_init_caches();
	if (!(modnum = lis_register_strmod(&cd_info, cd_minfo.mi_idname))) {
		cd_minfo.mi_idnum = 0;
		cmn_err(CE_NOTE, "cdi-sdl: couldn't register module\n");
		return;
	}
	if (lis_register_module_qlock_option(modnum, LIS_QLOCK_NONE) < 0) {
		lis_unregister_strmod(&cd_info);
		cd_minfo.mi_idnum = 0;
		cmn_err(CE_NOTE, "cdi-sdl: couldn't register module\n");
		return;
	}
	cd_minfo.mi_idnum = modnum;
	return;
}
STATIC INLINE void
cd_terminate(void)
{
	ensure(cd_minfo.mi_idnum, return);
	if ((cd_minfo.mi_idnum = lis_unregister_strmod(&cd_info)))
		cmn_err(CE_WARN, "cdi-sdl: couldn't unregister as module\n");
	cd_term_caches();
	return;
}

/*
 *  =========================================================================
 *
 *  Linux Kernel Module Initialization
 *
 *  =========================================================================
 */

int
init_module(void)
{
	cd_init();
	return (0);
}

void
cleanup_module(void)
{
	cd_terminate();
	return;
}
