/*****************************************************************************

 @(#) $RCSfile: testmod_mps.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:56 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2011-09-02 08:46:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: testmod_mps.c,v $
 Revision 1.1.2.3  2011-09-02 08:46:56  brian
 - sync up lots of repo and build changes from git

 Revision 1.1.2.2  2010-11-28 14:22:08  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:40:36  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: testmod_mps.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:56 $";

/*
 * This is a test driver for MPS compatability.  Its purpose is to test MPS compatability functions
 * in a driver context and to serve as an example in the use of STREAMS and MPS compatability
 * functions.
 */

 /* Functions not defined by MPS are defined by the SVR4 compatability module.
    This define exposes the SVR4 functions. */
#define _SVR4_SOURCE	1

 /* This exposes the MPS compatability functions.  */
#define _MPS_SOURCE	1

 /* This is the first header file included.  It provides all of the STREAMS
    header files in a compatabile way.  It uses the definitions above to
    determine which headers to include.  */
#include <sys/os7/compat.h>

 /* This is simply defines for use with Linux module functions. */
#define TM_MPS_DESCRIP		"MPS Compatibility Test Module (TEST-MOD-MPS) STREAMS Module"
#define TM_MPS_EXTRA		"Part of UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define TM_MPS_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define TM_MPS_REVISION		"OpenSS7 $RCSfile: testmod_mps.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-09-02 08:46:56 $"
#define TM_MPS_DEVICE		"MPS Compatability TEST Module (TEST-MOD-MPS)"
#define TM_MPS_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define TM_MPS_LICENSE		"GPL"
#define TM_MPS_BANNER		TM_MPS_DESCRIP		"\n" \
				TM_MPS_EXTRA		"\n" \
				TM_MPS_REVISION		"\n" \
				TM_MPS_COPYRIGHT	"\n" \
				TM_MPS_DEVICE		"\n" \
				TM_MPS_CONTACT
#define TM_MPS_SPLASH		TM_MPS_DESCRIP		" - " \
				TM_MPS_REVISION

 /* The following provides the module definitions necessary to use the
    resulting binary as a loadable Linux kernel module. */
#if defined LINUX
MODULE_AUTHOR(TM_MPS_CONTACT);
MODULE_DESCRIPTION(TM_MPS_DESCRIP);
MODULE_SUPPORTED_DEVICES(TM_MPS_DEVICE);
#if defined MODULE_LICENSE
MODULE_LICENSE(TM_MPS_LICENSE);
#endif				/* defined MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-testmod-mps");
#endif				/* defined MODULE_ALIAS */
#if defined MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* defined MODULE_VERSION */
#endif				/* defined LINUX */

#define TM_MPS_MOD_ID		CONFIG_STREAMS_TM_MPS_MODID
#define TM_MPS_MOD_NAME		CONFIG_STREAMS_TM_MPS_NAME

 /* Linux Fast-STREAMS supports a wide array of aliases. */
#if defined LINUX
#if defined MODULE_ALIAS
 /* autoload kernel module by module id */
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_TM_MPS_MODID));
 /* autoload kernel module by module name */
MODULE_ALIAS("streams-module-testmod-mps");
#endif				/* defined MODULE_ALIAS */
#endif				/* defined LINUX */

 /* Shorten names some more. */
#define MOD_ID		TM_MPS_MOD_ID
#define MOD_NAME	TM_MPS_MOD_NAME
 /* Display verbose banner for loadable kernel modules yet brief banner if
    statically linked with kernel. */
#if defined MODULE
#define MOD_BANNER	TM_MPS_BANNER
#else				/* !defined MODULE */
#define MOD_BANNER	TM_MPS_SPLASH
#endif				/* !defined MODULE */

 /* Standard STREAMS definitions. */

static struct module_info tm_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = SHEADHIWAT,		/* Hi water mark */
	.mi_lowat = SHEADLOWAT,		/* Lo water mark */
};

 /* Under Linux Fast-STREAMS if you define a module statistics structure STREAMS
    will keep track of qopen, qclose, qadmin, qput and qsrv procedure entry
    statistics for you.  Align these on an SMP cache line for speed. */
static struct module_stat tm_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat tm_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

 /* Standard STREAMS open and close procedures.  These are 'streamscall' so
    that they match the ABI of the STREAMS implementation. */
static streamscall int tm_qopen(queue_t *, dev_t *, int, int, cred_t *);
static streamscall int tm_qclose(queue_t *, int, cred_t *);

 /* Standard STREAMS read side put procedures and qinit structure. */
static streamscall int tm_rput(queue_t *, mblk_t *);
static streamscall int tm_rsrv(queue_t *);

static struct qinit tm_rinit = {
	.qi_putp = tm_rput,		/* Read put (message from below) */
	.qi_srvp = tm_rsrv,		/* Read queue service */
	.qi_qopen = tm_qopen,		/* Each open */
	.qi_qclose = tm_qclose,		/* Last close */
	.qi_minfo = &tm_minfo,		/* Information */
	.qi_mstat = &tm_rstat,		/* Statistics */
};

 /* Standard STREAMS write side put procedures and qinit structure. */
static streamscall int tm_wput(queue_t *, mblk_t *);
static streamscall int tm_wsrv(queue_t *);

static struct qinit tm_winit = {
	.qi_putp = tm_wput,		/* Write put (message from above) */
	.qi_srvp = tm_wsrv,		/* Write queue service */
	.qi_minfo = &tm_minfo,		/* Information */
	.qi_mstat = &tm_wstat,		/* Statistics */
};

 /* Standard STREAMS definition structure, the name is important if you want
    LiS to link and automatically register your module.  This must match the
    prefix defined in the configuration file concatenated with "info".  It must 
    be non-static for linkage but static for a loadable module. */
MODULE_STATIC struct streamtab tm_mpsinfo = {
	.st_rdinit = &tm_rinit,		/* Upper read queue */
	.st_wrinit = &tm_winit,		/* Lower read queue */
};

 /* A private structure.  The members are whatever you need them to be. */
struct tm_priv {
	int state;
	int flags;
};

 /* If you want to use memory caches for your private structures, you must
    initialize and terminate the memory cache yourself.  Use mi_open_size() to
    determine the size of the private structure with the additional hidden
    structure that the MPS functions use. */
static kmem_cachep_t tm_priv_cachep = NULL;

static int
tm_init_caches(void)
{
	if (!tm_priv_cachep &&
	    !(tm_priv_cachep =
	      kmem_create_cache("tm_priv_cachep", mi_open_size(sizeof(struct tm_priv)), 0,
				SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_WARN, "error: %s +%d: %s: cannot allocate tm_priv_cachep",
			__FILE__, __LINE__, __FUNCTION__);
		return (-ENOMEM);
	}
	cmn_err(CE_DEBUG, "%s: initialized module private structure cache", MOD_NAME);
	return (0);
}

static int
tm_term_caches(void)
{
	if (tm_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(tm_priv_cachep)) {
			cmn_err(CE_WARN, "error: %s +%d: %s: cannot destroy tm_priv_cachep",
				__FILE__, __LINE__, __FUNCTION__);
			return (-EBUSY);
		}
#else
		kmem_cache_destroy(tm_priv_cachep);
#endif
		cmn_err(CE_DEBUG, "%s: destroyed module private struture cache", MOD_NAME);
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
MODULE_PARM_DESC(modid, "Module ID for TESTMOD-MPS driver. (0 for allocation.)");

 /* Linux FAST-STREAMS registration. */

static struct fmodsw tm_fmod = {
	.f_name = MOD_NAME,
	.f_str = &tm_mpsinfo,
	.f_flag = F_MP,
	.f_kmod = THIS_MODULE,
};

 /* Linux kernel initialization. */
static __init int
tm_init(void)
{
	int err;

	if ((err = register_strmod(&tm_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register STREAMS module %d, err = %d", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	if ((err = tm_init_caches())) {
		cmn_err(CE_WARN, "%s: could not initialize caches, err = %d", MOD_NAME, -err);
		unregister_strmod(&tm_fmod);
		return (err);
	}
	return (0);
}

static __exit void
tm_exit(void)
{
	int err;

	if ((err = tm_term_caches))
		cmn_err(CE_WARN, "%s: could not terminate caches, err = %d", MOD_NAME, -err);
	if ((err = unregister_strmod(&tm_fmod)))
		cmn_err(CE_WARN, "%s: could not unregister STREAMS module %d, err = %d", MOD_NAME,
			(int) modid, -err);
	return;
}

#endif				/* defined LINUX */
