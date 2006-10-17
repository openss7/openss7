/*****************************************************************************

 @(#) $RCSfile: tali.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:43 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@dallas.net>

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

 Last Modified $Date: 2006/10/17 11:55:43 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: tali.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:43 $"

static char const ident[] =
    "$RCSfile: tali.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:43 $";

#include <sys/os7/compat.h>

#include "tali.h"
#include "tali_data.h"

/*
 *  TALI MULTIPLEXING DRIVER
 *  -------------------------------------------------------------------------
 *  This is a multiplexing driver to TALI.  When TALI is opened by its control
 *  stream (typically a tali configuration daemon), it provides a control
 *  channel for configuration and routing.  When TALI is opened by one of its
 *  user streams, it provides either a normal OpenSS7 MTP, ISUP, or SCCP stream.
 */

#define TALI_DESCRIP	"TALI STREAMS MULTIPLEXING DRIVER." "\n" \
			"Part of the OpenSS7 stack for Linux Fast-STREAMS"
#define TALI_REVISION	"OpenSS7 $RCSfile: tali.c,v $ $Name:  $ ($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:43 $"
#define TALI_COPYRIGHT	"Copyright (c) 1997-2004 OpenSS7 Corporation.  All Rights Reserved."
#define TALI_DEVICE	"Part of the OpenSS7 Stack for Linux Fast STREAMS."
#define TALI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TALI_LICENSE	"GPL"
#define TALI_BANNER	TALI_DESCRIP	"\n" \
			TALI_REVISION	"\n" \
			TALI_COPYRIGHT	"\n" \
			TALI_DEVICE	"\n" \
			TALI_CONTACT	"\n"
#define TALI_SPLASH	TALI_DEVICE	" - " \
			TALI_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(TALI_CONTACT);
MODULE_DESCRIPTION(TALI_DESCRIP);
MODULE_SUPPORTED_DEVICE(TALI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TALI_LICENSE);
#endif
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-tali");
#endif
#endif				/* LINUX */

#ifdef LFS
#define TALI_DRV_ID	CONFIG_STREAMS_TALI_MODID
#define TALI_DRV_NAME	CONFIG_STREAMS_TALI_NAME
#define TALI_CMAJORS	CONFIG_STREAMS_TALI_NMAJORS
#define TALI_CMAJOR_0	CONFIG_STREAMS_TALI_MAJOR
#define TALI_UNITS	CONFIG_STREAMS_TALI_UNITS
#endif

#define TALI_MTP_CMINOR		1
#define TALI_SCCP_CMINOR	2
#define TALI_ISUP_CMINOR	3
#define TALI_FREE_CMINOR	4

/*
 *  =========================================================================
 *
 *  STREAM Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		TALI_DRV_ID
#define DRV_NAME	TALI_DRV_NAME
#define CMAJORS		TALI_CMAJORS
#define CMAJOR_0	TALI_CMAJOR_0
#define UNITS		TALI_UNITS
#ifdef MODULE
#define DRV_BANNER	TALI_BANNER
#else				/* MODULE */
#define DRV_BANNER	TALI_SPLASH
#endif				/* MODULE */

STATIC struct module_info tali_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1,			/* Hi water mark */
	.mi_lowat = 0,			/* Lo water mark */
};

STATIC struct module_stat tali_mstat = { 0, };

STATIC int tali_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int tali_close(queue_t *, int, cred_t *);

STATIC int tali_rdprim(queue_t *, mblk_t *);
STATIC int tali_wrprim(queue_t *, mblk_t *);

STATIC int mtp_rdprim(queue_t *, mblk_t *);
STATIC int mtp_wrprim(queue_t *, mblk_t *);

STATIC int sccp_rdprim(queue_t *, mblk_t *);
STATIC int sccp_wrprim(queue_t *, mblk_t *);

STATIC int isup_rdprim(queue_t *, mblk_t *);
STATIC int isup_wrprim(queue_t *, mblk_t *);

STATIC struct qinit tali_rdinit {
	.qi_putp = ss7_oput,		/* Read put (msg from below) */
	.qi_srvp = ss7_osrv,		/* Read queue service */
	.qi_qopen = tali_open,		/* Each open */
	.qi_qclose = tali_close,	/* Last close */
	.qi_qadmin = NULL,		/* Admin (not used) */
	.qi_minfo = &tali_minfo,	/* Information */
	.qi_mstat = &tali_mstat,	/* Statistics */
};

STATIC struct qinit tali_wrinit {
	.qi_putp = ss7_iput,		/* Write put (msg from above) */
	.qi_srvp = ss7_isrv,		/* Write queue service */
	.qi_qopen = NULL,		/* Each open */
	.qi_qclose = NULL,		/* Last close */
	.qi_qadmin = NULL,		/* Admin (not used) */
	.qi_minfo = &tali_minfo,	/* Information */
	.qi_mstat = &tali_mstat,	/* Statistics */
};

STATIC int sg_muxrprim(queue_t *, mblk_t *);
STATIC int sg_muxwprim(queue_t *, mblk_t *);

STATIC int as_muxrprim(queue_t *, mblk_t *);
STATIC int as_muxwprim(queue_t *, mblk_t *);

STATIC int mtp_muxrprim(queue_t *, mblk_t *);
STATIC int mtp_muxwprim(queue_t *, mblk_t *);

STATIC int sccp_muxrprim(queue_t *, mblk_t *);
STATIC int sccp_muxwprim(queue_t *, mblk_t *);

STATIC int isup_muxrprim(queue_t *, mblk_t *);
STATIC int isup_muxwprim(queue_t *, mblk_t *);

STATIC struct qinit tali_muxrinit {
	.qi_putp = ss7_iput,		/* Read put (msg from below) */
	.qi_srvp = ss7_isrv,		/* Read queue service */
	.qi_qopen = NULL,		/* Each open */
	.qi_qclose = NULL,		/* Last close */
	.qi_qadmin = NULL,		/* Admin (not used) */
	.qi_minfo = &tali_minfo,	/* Information */
	.qi_mstat = &tali_mstat,	/* Statistics */
};

STATIC struct qinit tali_muxwinit {
	.qi_putp = ss7_oput,		/* Write put (msg from above) */
	.qi_srvp = ss7_osrv,		/* Write queue service */
	.qi_qopen = NULL,		/* Each open */
	.qi_qclose = NULL,		/* Last close */
	.qi_qadmin = NULL,		/* Admin (not used) */
	.qi_minfo = &tali_minfo,	/* Information */
	.qi_mstat = &tali_mstat,	/* Statistics */
};

MODULE_STATIC struct streamtab tali_info = {
	.st_rdinit = &tali_rdinit,	/* Upper read queue */
	.st_wrinit = &tali_wrinit,	/* Upper write queue */
	.st_muxrinit = &tali_muxrinit,	/* Lower read queue */
	.st_muxwinit = &tali_muxwinit,	/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  Private Structures
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  Cache initialization and termination.
 *
 *  =========================================================================
 */

extern kmem_cache_t *tali_pp_cachep = NULL;
extern kmem_cache_t *tali_xp_cachep = NULL;
extern kmem_cache_t *tali_gp_cachep = NULL;
extern kmem_cache_t *tali_as_cachep = NULL;
extern kmem_cache_t *tali_ap_cachep = NULL;
extern kmem_cache_t *tali_sp_cachep = NULL;
extern kmem_cache_t *tali_np_cachep = NULL;

STATIC tali_pp_cache_allocated = 0;
STATIC tali_xp_cache_allocated = 0;
STATIC tali_gp_cache_allocated = 0;
STATIC tali_as_cache_allocated = 0;
STATIC tali_ap_cache_allocated = 0;
STATIC tali_sp_cache_allocated = 0;
STATIC tali_np_cache_allocated = 0;

STATIC void
tali_term_caches(void)
{
	if (tali_pp_cachep) {
		if (tali_pp_cache_allocated)
			kmem_cache_destroy(tali_pp_cachep);
		tali_pp_cachep = NULL;
	}
	if (tali_xp_cachep) {
		if (tali_xp_cache_allocated)
			kmem_cache_destroy(tali_xp_cachep);
		tali_xp_cachep = NULL;
	}
	if (tali_gp_cachep) {
		if (tali_gp_cache_allocated)
			kmem_cache_destroy(tali_gp_cachep);
		tali_gp_cachep = NULL;
	}
	if (tali_sp_cachep) {
		if (tali_sp_cache_allocated)
			kmem_cache_destroy(tali_sp_cachep);
		tali_sp_cachep = NULL;
	}
	if (tali_as_cachep) {
		if (tali_as_cache_allocated)
			kmem_cache_destroy(tali_as_cachep);
		tali_as_cachep = NULL;
	}
	if (tali_ap_cachep) {
		if (tali_ap_cache_allocated)
			kmem_cache_destroy(tali_ap_cachep);
		tali_ap_cachep = NULL;
	}
	if (tali_np_cachep) {
		if (tali_np_cache_allocated)
			kmem_cache_destroy(tali_np_cachep);
		tali_np_cachep = NULL;
	}
	return;
}

STATIC int
tali_init_caches(void)
{
	if (!(tali_pp_cachep))
		if (!(tali_pp_cachep = kmem_cache_create("tali_pp_cachep", sizeof(pp_t),
							 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
			goto tali_init_caches_failed;
		else
			tali_pp_cache_allocated = 1;
	if (!(tali_xp_cachep))
		if (!(tali_xp_cachep = kmem_cache_create("tali_xp_cachep", sizeof(xp_t),
							 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
			goto tali_init_caches_failed;
		else
			tali_xp_cache_allocated = 1;
	if (!(tali_gp_cachep))
		if (!(tali_gp_cachep = kmem_cache_create("tali_gp_cachep", sizeof(gp_t),
							 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
			goto tali_init_caches_failed;
		else
			tali_gp_cache_allocated = 1;
	if (!(tali_sp_cachep))
		if (!(tali_sp_cachep = kmem_cache_create("tali_sp_cachep", sizeof(sp_t),
							 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
			goto tali_init_caches_failed;
		else
			tali_sp_cache_allocated = 1;
	if (!(tali_as_cachep))
		if (!(tali_as_cachep = kmem_cache_create("tali_as_cachep", sizeof(as_t),
							 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
			goto tali_init_caches_failed;
		else
			tali_as_cache_allocated = 1;
	if (!(tali_ap_cachep))
		if (!(tali_ap_cachep = kmem_cache_create("tali_ap_cachep", sizeof(ap_t),
							 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
			goto tali_init_caches_failed;
		else
			tali_ap_cache_allocated = 1;
	if (!(tali_np_cachep))
		if (!(tali_np_cachep = kmem_cache_create("tali_np_cachep", sizeof(np_t),
							 0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
			goto tali_init_caches_failed;
		else
			tali_np_cache_allocated = 1;
	return (0);

      tali_init_caches_failed:
	tali_term_caches();
	return (ENOMEM);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
STATIC major_t tali_majors[CMAJORS] = { CMAJOR_0, };

/*
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC int
tali_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	psw_t flags;
	int mindex = 0;
	major_t cmajor = bmajor = getmajor(*devp);
	minor_t cminor = bminor = getminor(*devp);
	struct tali *ta, **tpp;
	MOD_INC_USE_COUNT;
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q) - q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", DRV_NAME));
		MOD_DEC_USE_COUNT;
		return (EIO);
	}
	if (cmajor != CMAJOR_0 || cminor > TALI_ISUP_CMINOR) {
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	/* allocate a new device */
	cminor = TALI_FREE_CMINOR;
	spin_lock_irqsave(&master.lock, flags);
	for (tpp = &master.ta.list; *tpp; tpp = &(*tpp)->next) {
		major_t dmajor = (*tpp)->u.dev.cmajor;
		if (cmajor != dmajor) {
			minor_t dminor = (*tpp)->u.dev.cminor;
			if (cminor < dminor)
				break;
			if (cminor > dminor)
				continue;
			if (cminor == dminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= CMAJORS || !(cmajor = tali_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_irqrestore(&master.lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	printd(("%s: opened character device %hu:%hu\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(ta = tali_alloc_ta(q, tpp, cmajor, cminor, crp, bminor))) {
		ptrace(("%s: ERROR: no memory\n", DRV_NAME));
		spin_unlock_irqrestore(&master.lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENOMEM);
	}
	spin_unlock_irqrestore(&master.lock, flags);
	return (0);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC int
tali_close(queue_t *q, int flag, cred_t *crp)
{
	struct tali *ta = TA_PRIV(q);
	psw_t flags;
	(void) ta;
	printd(("%s: %p: closing character device %hu:%hu\n", DRV_NAME, ta, ta->u.dev.cmajor,
		ta->u.dev.cminor));
	spin_lock_irqsave(&master.lock, flags);
	{
		tali_free_ta(q);
	}
	spin_unlock_irqrestore(&master.lock, flags);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  =========================================================================
 *
 *  Registration and initialization
 *
 *  =========================================================================
 */
#ifdef LINUX
/*
 *  Linux Registration
 *  -------------------------------------------------------------------------
 */

unsigned short modid = DRV_ID;
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the TALI driver. (0 for allocation.)");

major_t major = CMAJOR_0;
#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the TALI driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw tali_cdev = {
	.d_name = DRV_NAME,
	.d_str = &taliinfo,
	.d_flag = 0,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int
tali_register_strdev(major_t major)
{
	int err;
	if ((err = register_strdev(&tali_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int
tali_unregister_strdev(major_t major)
{
	int err;
	if ((err = unregister_strdev(&tali_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

STATIC int
tali_register_strdev(major_t major)
{
	int err;
	if ((err = lis_register_strdev(major, &taliinfo, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC int
tali_unregister_strdev(major_t major)
{
	int err;
	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
taliterminate(void)
{
	int err, mindex;
	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (tali_majors[mindex]) {
			if ((err = tali_unregister_strdev(tali_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					tali_majors[mindex]);
			if (mindex)
				tali_majors[mindex] = 0;
		}
	}
	if ((err = tali_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
taliinit(void)
{
	int err, mindex = 0;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = tali_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		taliterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = tali_register_strdev(tali_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					tali_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				taliterminate();
				return (err);
			}
		}
		if (tali_majors[mindex] == 0)
			tali_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(tali_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = tali_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(taliinit);
module_exit(taliterminate);

#endif				/* LINUX */
