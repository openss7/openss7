/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/*
 *  This driver provides a sockfs(5) based approach to providing sockets
 *  interface to TPI streams.
 */

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>
#include <linux/interrupt.h>

#include <sys/socksys.h>

#define SSYS_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SSYS_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define SSYS_COPYRIGHT	"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define SSYS_REVISION	"OpenSS7 $RCSfile$ $Name$($Revision$) $Date$"
#define SSYS_DEVICE	"SVR 4.2 STREAMS SOCKSYS Driver"
#define SSYS_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SSYS_LICENSE	"GPL"
#define SSYS_BANNER	SSYS_DESCRIP		"\n" \
			SSYS_EXTRA		"\n" \
			SSYS_REVISION		"\n" \
			SSYS_COPYRIGHT		"\n" \
			SSYS_DEVICE		"\n" \
			SSYS_CONTACT		"\n"
#define SSYS_SPLASH	SSYS_DESCRIP		" - " \
			SSYS_REVISION

#ifdef LFS
#define SSYS_DRV_ID	CONFIG_STREAMS_SSYS_MODID
#define SSYS_DRV_NAME	CONFIG_STREAMS_SSYS_NAME
#define SSYS_CMAJORS	CONFIG_STREAMS_SSYS_NMAJORS
#define SSYS_CMAJOR_0	CONFIG_STREAMS_SSYS_MAJOR
#define SSYS_UNITS	CONFIG_STREAMS_SSYS_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_SSYS_MODID));
MODULE_ALIAS("streams-driver-socksys");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_SSYS_MAJOR));
MODULE_ALIAS("/dev/streams/socksys");
MODULE_ALIAS("/dev/streams/socksys/*");
MODULE_ALIAS("/dev/streams/clone/socksys");
#endif
MODULE_ALIAS("char-major-" __stringify(SSYS_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(SSYS_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(SSYS_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/socksys");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  STREAMS Definitions
 *  ==========================================================================
 */

#define DRV_ID		SSYS_DRV_ID
#define DRV_NAME	SSYS_DRV_NAME
#define CMAJORS		SSYS_CMAJORS
#define CMAJOR_0	SSYS_CMAJOR_0
#define UNITS		SSYS_UNITS
#ifdef MODULE
#define DRV_BANNER	SSYS_BANNER
#else				/* MODULE */
#define DRV_BANNER	SSYS_SPLASH
#endif				/* MODULE */

STATIC struct module_info ssys_minfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module name */
	.mi_minpsz = 0,		/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,	/* Max packet size accepted */
	.mi_hiwat = (1 << 18),	/* Hi water mark */
	.mi_lowat = 0,		/* Lo water mark */
};

STATIC struct module_stat ssys_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat ssys_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

STATIC streamscall int ssys_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int ssys_qclose(queue_t *, int, cred_t *);

streamscall int ssys_rput(queue_t *, mblk_t *);
streamscall int ssys_wput(queue_t *, mblk_t *);

STATIC struct qinit ssys_rinit = {
	.qi_putp = ssys_rput,	/* Read put procedure (messgae from below) */
	.qi_qopen = ssys_qopen,	/* Each open */
	.qi_qclose = ssys_qclose,	/* Last close */
	.qi_minfo = &ssys_minfo,	/* Module information */
	.qi_mstat = &ssys_rstat,	/* Module statistics */
};

STATIC struct qinit ssys_winit = {
	.qi_putp = ssys_wput,	/* Write put procedure (message from above) */
	.qi_minfo = &ssys_minfo,	/* Module information */
	.qi_mstat = &ssys_wstat,	/* Module statistics */
};

MODULE_STATIC struct streamtab ssys_info = {
	.st_rdinit = &ssys_rinit,	/* Upper read queue */
	.st_wrinit = &ssys_winit,	/* Upper write queue */
};

/*
 *  Primary data structures.
 */

/*
 *  Registration and initialization
 */
#ifdef LINUX
/*
 *  Linux registration
 */
unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the SOCKSYS driver. (0 for allocation.)");

major_t major = CMAJOR_0;
MODULE_PARM(major, "h");
#else
MODULE_PARM(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the SOCKSYS driver. (0 for allocation.)");

#ifdef LFS
/*
 *  Linux Fast-STREAMS Registration
 */
STATIC struct cdevsw ssys_cdev = {
	.d_name = DRV_NAME,
	.d_str = &ssys_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC __unlikely int
ssys_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&ssys_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC __unlikely int
ssys_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&ssys_cdev, major)) < 0)
		return (err);
	return (0);
}
#endif				/* LFS */

#ifdef LIS
/*
 *  Linux STREAMS Registration
 */
STATIC __unlikely int
ssys_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &ssys_info, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC __unlikely int
ssys_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}
#endif				/* LIS */

MODULE_STATIC void __exit
ssysterminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (ssys_majors[mindex]) {
			if ((err = ssys_unregister_strdev(ssys_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					ssys_majors[mindex]);
			if (mindex)
				ssys_majors[mindex] = 0;
		}
	}
	if ((err = ssys_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
ssysinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = ssys_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		ssysterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = ssys_register_strdev(ssys_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					ssys_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				ssysterminate();
				return (err);
			}
		}
		if (ssys_majors[mindex] == 0)
			ssys_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(ssys_majors[index]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = ssys_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(ssysinit);
module_exit(ssysterminate);

#endif				/* LINUX */
