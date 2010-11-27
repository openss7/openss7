/*****************************************************************************

 @(#) $RCSfile: testdrv_mps.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:20:56 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2009-06-21 11:20:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: testdrv_mps.c,v $
 Revision 1.1.2.1  2009-06-21 11:20:56  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: testdrv_mps.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:20:56 $";

/*
 * This is a test module for MPS compatability.  Its purpose is to test MPS compatability functions
 * in a module context and to serve as an example in the use of STREAMS and MPS compatability
 * functions.
 */

 /* Functions not defined by MPS are defined by the SVR4 compatability module.
    This define exposes the SVR4 functions.  */
#define _SVR4_SOURCE	1

 /* This exposes the MPS compatability functions.  */
#define _MPS_SOURCE	1

 /* This is the first header file included.  It provides all of the STREAMS
    header files in a compatabile way.  It uses the definitions above to
    determine which headers to include.  */
#include <sys/os7/compat.h>

 /* This is simply defines for use with Linux module functions. */
#define TD_MPS_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define TD_MPS_EXTRA		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define TD_MPS_COPYRIGHT	"Copyright (c) 2008-2010  Monavacon Limited.  All Rights Reserved."
#define TD_MPS_REVISION		"OpenSS7 $RCSfile: testdrv_mps.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:20:56 $"
#define TD_MPS_DEVICE		"MPS Compatability TEST Driver (TEST-DRV-MPS)"
#define TD_MPS_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define TD_MPS_LICENSE		"GPL"
#define TD_MPS_BANNER		TD_MPS_DESCRIP		"\n" \
				TD_MPS_EXTRA		"\n" \
				TD_MPS_REVISION		"\n" \
				TD_MPS_COPYRIGHT	"\n" \
				TD_MPS_DEVICE		"\n" \
				TD_MPS_CONTACT
#define TD_MPS_SPLASH		TD_MPS_DESCRIP		" - " \
				TD_MPS_REVISION

 /* The following provides the module definitions necessary to use the
    resulting binary as a loadable Linux kernel module. */
#if defined LINUX
MODULE_AUTHOR(TD_MPS_CONTACT);
MODULE_DESCRIPTION(TD_MPS_DESCRIP);
MODULE_SUPPORTED_DEVICES(TD_MPS_DEVICE);
#if defined MODULE_LICENSE
MODULE_LICENSE(TD_MPS_LICENSE);
#endif				/* defined MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-testdrv-mps");
#endif				/* defined MODULE_ALIAS */
#if defined MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* defined MODULE_VERSION */
#endif				/* defined LINUX */

#define TD_MPS_DRV_ID		CONFIG_STREAMS_TD_MPS_MAJOR
#define TD_MPS_DRV_NAME		CONFIG_STREAMS_TD_MPS_NAME
#define TD_MPS_CMAJORS		CONFIG_STREAMS_TD_MPS_NMAJORS
#define TD_MPS_CMAJOR_0		CONFIG_STREAMS_TD_MPS_MAJOR
#define TD_MPS_UNITS		CONFIG_STREAMS_TD_MPS_NMINORS

 /* Linux Fast-STREAMS supports a wide array of aliases. */
#if defined LINUX
#if defined MODULE_ALIAS
 /* autoload kernel module by driver id */
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_TD_MPS_MODID));
 /* autoload kernel module by driver name */
MODULE_ALIAS("streams-driver-testdrv-mps");
 /* autoload kernel module by specfs device node */
MODULE_ALIAS("/dev/streams/testdrv-mps");
 /* autoload kernel module by specfs clone device node */
MODULE_ALIAS("/dev/streams/clone/testdrv-mps");
 /* autoload kernel module by major device number */
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_TD_MPS_MAJOR));
 /* autoload kernel module by minor device number */
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_TD_MPS_MAJOR) "-*");
 /* autoload kernel module by clone device number */
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_TD_MPS_MAJOR) "-0");
 /* autoload kernel module by external device name */
MODULE_ALIAS("/dev/testdrv-mps");
#endif				/* defined MODULE_ALIAS */
#endif				/* defined LINUX */

 /* Shorten names some more. */
#define DRV_ID		TD_MPS_DRV_ID
#define DRV_NAME	TD_MPS_DRV_NAME
#define CMAJORS		TD_MPS_CMAJORS
#define CMAJOR_0	TD_MPS_CMAJOR_0
#define UNITS		TD_MPS_UNITS
 /* Display verbose banner for loadable kernel modules yet brief banner if
    statically linked with kernel. */
#if defined MODULE
#define DRV_BANNER	TD_MPS_BANNER
#else				/* !defined MODULE */
#define DRV_BANNER	TD_MPS_SPLASH
#endif				/* !defined MODULE */

 /* Standard STREAMS definitions. */

static struct module_info td_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = SHEADHIWAT,		/* Hi water mark */
	.mi_lowat = SHEADLOWAT,		/* Lo water mark */
};

 /* Under Linux Fast-STREAMS if you define a module statistics structure STREAMS
    will keep track of qopen, qclose, qadmin, qput and qsrv procedure entry
    statistics for you.  Align these on an SMP cache line for speed. */
static struct module_stat td_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat td_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

 /* Standard STREAMS open and close procedures.  These are 'streamscall' so
    that they match the ABI of the STREAMS implementation.  */
static streamscall int td_qopen(queue_t *, dev_t *, int, int, cred_t *);
static streamscall int td_qclose(queue_t *, int, cred_t *);

 /* Standard STREAMS read side put procedures and qinit structure. */
static streamscall int td_rput(queue_t *, mblk_t *);
static streamscall int td_rsrv(queue_t *);

static struct qinit td_rinit = {
	.qi_putp = td_rput,		/* Read put (message from below) */
	.qi_srvp = td_rsrv,		/* Read queue service */
	.qi_qopen = td_qopen,		/* Each open */
	.qi_qclose = td_qclose,		/* Last close */
	.qi_minfo = &td_minfo,		/* Information */
	.qi_mstat = &td_rstat,		/* Statistics */
};

 /* Standard STREAMS write side put procedures and qinit structure. */
static streamscall int td_wput(queue_t *, mblk_t *);
static streamscall int td_wsrv(queue_t *);

static struct qinit td_winit = {
	.qi_putp = td_wput,		/* Write put (message from above) */
	.qi_srvp = td_wsrv,		/* Write queue service */
	.qi_minfo = &td_minfo,		/* Information */
	.qi_mstat = &td_wstat,		/* Statistics */
};

 /* Standard STREAMS definition structure, the name is important if you want
    LiS to link and automatically register your module.  This must match the
    prefix defined in the configuration file concatenated with "info".  It must 
    be non-static for linkage but static for a loadable module. */
MODULE_STATIC struct streamtab td_mpsinfo = {
	.st_rdinit = &td_rinit,		/* Upper read queue */
	.st_wrinit = &td_winit,		/* Lower read queue */
};

 /* A private structure.  The members are whatever you need them to be. */
struct td_priv {
	int state;
	int flags;
};

 /* If you want to use memory caches for your private structures, you must
    initialize and terminate the memory cache yourself.  Use mi_open_size() to
    determine the size of the private structure with the additional hidden
    structure that the MPS functions use. */
static kmem_cachep_t td_priv_cachep = NULL;

static int
td_init_caches(void)
{
	if (!td_priv_cachep &&
	    !(td_priv_cachep =
	      kmem_create_cache("td_priv_cachep", mi_open_size(sizeof(struct td_priv)), 0,
				SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_WARN, "error: %s +%d: %s: cannot allocate td_priv_cachep",
			__FILE__, __LINE__, __FUNCTION__);
		return (-ENOMEM);
	}
	cmn_err(CE_DEBUG, "%s: initialized module private structure cache", DRV_NAME);
	return (0);
}

static int
td_term_caches(void)
{
	if (td_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(td_priv_cachep)) {
			cmn_err(CE_WARN, "error: %s +%d: %s: cannot destroy td_priv_cachep",
				__FILE__, __LINE__, __FUNCTION__);
			return (-EBUSY);
		}
#else
		kmem_cache_destroy(td_priv_cachep);
#endif
		cmn_err(CE_DEBUG, "%s: destroyed module private struture cache", DRV_NAME);
	}
	return (0);
}

 /* Linux kernel module registration and initialization. */

#if defined LINUX
#if !defined module_param
MODULE_PARM(modid, "h");
#else				/* defined module_param */
module_param(modid, ushort, 0444);
#endif				/* defined module_param */
MODULE_PARM_DESC(modid, "Module ID for TESTDRV-MPS driver. (0 for allocation.)");

#if !defined module_param
MODULE_PARM(major, "h");
#else				/* defined module_param */
module_param(major, int, 0444);
#endif				/* defined module_param */
MODULE_PARM_DESC(major, "Major device number for TESTDRV-MPS driver. (0 for allocation.)");

 /* Linux FAST-STREAMS registration. */

static struct cdevsw td_cdev = {
	.d_name = DRV_NAME,
	.d_str = &td_mpsinfo,
	.d_flag = D_MP | D_CLONE,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

 /* Linux kernel initialization. */
static __init int
td_init(void)
{
	int err;

	if ((err = register_strdev(&td_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register STREAMS device %d, err = %d", DRV_NAME,
			(int) major, -err);
		return (err);
	}
	if (major == 0)
		major = err;
	if ((err = td_init_caches())) {
		cmn_err(CE_WARN, "%s: could not initialize caches, err = %d", DRV_NAME, -err);
		unregister_strdev(&td_cdev, major);
		return (err);
	}
	return (0);
}

static __exit void
td_exit(void)
{
	int err;

	if ((err = td_term_caches))
		cmn_err(CE_WARN, "%s: could not terminate caches, err = %d", DRV_NAME, -err);
	if ((err = unregister_strdev(&td_cdev, major)))
		cmn_err(CE_WARN, "%s: could not unregister STREAMS device %d, err = %d", DRV_NAME,
			(int) major, -err);
	return;
}

#endif				/* defined LINUX */
