/*****************************************************************************

 @(#) $RCSfile: cd.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/07/11 12:32:07 $

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

 Last Modified $Date: 2006/07/11 12:32:07 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: cd.c,v $
 Revision 0.9.2.2  2006/07/11 12:32:07  brian
 - added ISO and other implementations to distribution

 Revision 0.9.2.1  2006/04/12 20:36:03  brian
 - added some experimental drivers

 *****************************************************************************/

#ident "@(#) $RCSfile: cd.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/07/11 12:32:07 $"

static char const ident[] =
    "$RCSfile: cd.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/07/11 12:32:07 $";

/*
 *  This module is a master device driver for Communications Device Streams presending the
 *  Communications Device Interface (CDI) Service Interface at the upper boundary.  It collects a
 *  wide range of CDI drivers into a single device hierarchy as well as provide some exported
 *  functions that can be used by registering drivers.
 */

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>

#define d_tst_bit(nr,addr)	test_bit(nr,addr)
#define d_set_bit(nr,addr)	__set_bit(nr,addr)
#define d_clr_bit(nr,addr)	__clear_bit(nr,addr)

#include <linux/interrupt.h>

#endif				/* LINUX */

#include <sys/cdi.h>

#define CD_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define CD_EXTRA	"Part of the OpenSS7 stack for Linux Fast-STREAMS"
#define CD_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define CD_REVISION	"OpenSS7 $RCSfile: cd.c,v $ $Name:  $ ($Revision: 0.9.2.2 $) $Date: 2006/07/11 12:32:07 $"
#define CD_DEVICE	"SVR 4.2 STREAMS CDI OSI Communications Device Provider"
#define CD_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define CD_LICENSE	"GPL"
#define CD_BANNER	CD_DESCRIP	"\n" \
			CD_EXTRA	"\n" \
			CD_COPYRIGHT	"\n" \
			CD_DEVICE	"\n" \
			CD_CONTACT
#define CD_SPLASH	CD_DESCRIP	"\n" \
			CD_REVISION

#ifdef LINUX
MODULE_AUTHOR(CD_CONTACT);
MODULE_DESCRIPTION(CD_DESCRIP);
MODULE_SUPPORTED_DEVICE(CD_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(CD_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-cd");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define CD_DRV_ID	CONFIG_STREAMS_CD_MODID
#define CD_DRV_NAME	CONFIG_STREAMS_CD_NAME
#define CD_CMAJORS	CONFIG_STREAMS_CD_NMAJOR
#define CD_CMAJOR_0	CONFIG_STREAMS_CD_MAJOR
#define CD_UNITS	CONFIG_STREAMS_CD_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_CD_MODID));
MODULE_ALIAS("streams-driver-cd");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_CD_MAJOR));
MODULE_ALIAS("/dev/streams/cd");
MODULE_ALIAS("/dev/streams/cd/*");
MODULE_ALIAS("/dev/streams/clone/cd");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(DL_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(DL_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(DL_CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(DL_CMAJOR_0) "-" __stringify(DL_CMINOR));
MODULE_ALIAS("/dev/cd");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  Logging.
 */
#ifndef LOG_EMERG

#define LOG_EMERG	0
#define LOG_ALERT	1
#define LOG_CRIT	2
#define LOG_ERR		3
#define LOG_WARNING	4
#define LOG_NOTICE	5
#define LOG_INFO	6
#define LOG_DEBUG	7

#define LOG_KERN    (0<<3)
#define LOG_USER    (1<<3)

#endif				/* LOG_EMERG */

/*
 *  STREAMS Definitions
 *  ===================
 */

#define DRV_ID		CD_DRV_ID
#define DRV_NAME	CD_DRV_NAME
#define CMAJORS		CD_CMAJORS
#define CMAJOR_0	CD_CMAJOR_0
#define UNITS		CD_UNITS
#ifdef MODULE
#define DRV_BANNER	CD_BANNER
#else				/* MODULE */
#define DRV_BANNER	CD_SPLASH
#endif				/* MODULE */

STATIC struct module_info cd_minfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module name */
	.mi_minpsz = 0,		/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,	/* Max packet size acceptd */
	.mi_hiwat = 1 << 15,	/* Hi water mark */
	.mi_hiwat = 1 << 10,	/* Lo water mark */
};

/* Upper multiplex is a CD provider following the CDI. */

STATIC streamscall int cd_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int cd_qclose(queue_t *, int, cred_t *);

STATIC struct qinit cd_rinit = {
	.qi_qopen = &cd_qopen,	/* Each open */
	.qi_qclose = &cd_qclose,	/* Last close */
	.qi_minfo = &cd_minfo,	/* Module information */
};

STATIC struct qinit cd_winit = {
	.qi_minfo = &cd_minfo,	/* Module information */
};

STATIC struct streamtab cd_info = {
	.st_rdinit = &cd_rinit,	/* Upper read queue */
	.st_wrinit = &cd_winit,	/* Upper write queue */
};

#define MAX_MINORS 16

STATIC const char *cd_names[MAX_MINORS + 1] = {
	[0] = "cd",
	[1] = "cd_hdlc",
	[2] = "cd_bisync",
	[3] = "cd_lan",
	[4] = "cd_nodev",
	[5] = "cd",
	[6] = "cd",
	[7] = "cd",
	[8] = "cd",
	[9] = "cd",
	[10] = "cd",
	[11] = "cd",
	[12] = "cd",
	[13] = "cd",
	[14] = "cd",
	[15] = "cd",
	[16] = "cd",
};

struct cd_device {
	struct streamtab *cd_str;
	major_t cd_major;
};

STATIC rwlock_t cd_devices_lock = RW_LOCK_UNLOCKED;
STATIC struct cd_device cd_devices[MAX_MINORS + 1] = { };

int
cd_register_device(struct streamtab *str, major_t major, minor_t minor)
{
	int err;

	write_lock(&cd_devices_lock);
	if (minor < 0 || minor > MAX_MINORS || str == NULL)
		goto einval;
	if (cd_devices[minor].cd_str != str && cd_devices[minor].cd_str != NULL)
		goto eacces;
	if (cd_devices[minor].cd_str == str)
		goto ealready;

	cd_devices[minor].cd_str = str;
	cd_devices[minor].cd_major = major;

	/* For LFS, register a minor device node. */

	write_unlock(&cd_devices_lock);
	return (0);

      einval:
	err = -EINVAL;
	goto error;
      eacces:
	err = -EACCES;
	goto error;
      ealready:
	err = -EALREADY;
	goto error;
      error:
	write_unlock(&cd_devices_lock);
	return (err);
}

EXPORT_SYMBOL(cd_register_device);

int
cd_unregister_device(struct streamtab *str, major_t major, minor_t minor)
{
	int err;

	write_lock(&cd_devices_lock);
	if (minor < 0 || minor > MAX_MINORS || str == NULL)
		goto einval;
	if (cd_devices[minor].cd_str != str)
		goto eacces;
	if (cd_devices[minor].cd_str == NULL)
		goto ealready;

	/* For LFS, unregister a minor device node. */

	cd_devices[minor].cd_str = NULL;
	cd_devices[minor].cd_major = 0;

	write_unlock(&cd_devices_lock);
	return (0);

      einval:
	err = -EINVAL;
	goto error;
      eacces:
	err = -EACCES;
	goto error;
      ealready:
	err = -EALREADY;
	goto error;
      error:
	write_unlock(&cd_devices_lock);
	return (err);
}

EXPORT_SYMBOL(cd_unregister_device);

/*
 *  STREAMS Open and Close
 *  ======================
 */
STATIC streamscall int
cd_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	int rtn;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE, "cannot push as module");
		return (ENXIO);
	}
	/* cannot open anything but defined clone minors */
#ifdef LIS
	if (cmajor != CMAJOR_0 || cminor > MAX_MINORS) {
		strlog(DRV_ID, cminor, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "attempt to open device number %d:%d directly", cmajor, cminor);
		return (ENXIO);
	}
#endif				/* LIS */
#ifdef LFS
	/* Linux Fast-STREAMS always passes internal major dvice numbers (module ids) */
	if (cmajor != DRV_ID || cminor > MAX_MINORS) {
		strlog(DRV_ID, cminor, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "attempt to open device number %d:%d directly", cmajor, cminor);
		return (ENXIO);
	}
#endif				/* LFS */
	sflag = CLONEOPEN;
	read_lock(&cd_devices_lock);
	if (cd_devices[cminor].cd_str == NULL) {
		/* we could probably reattempt this in a number of ways, particularly under Linux
		   Fast-STREAMS there are are number of module aliases that we can use. */
		read_unlock(&cd_devices_lock);
		request_module(cd_names[cminor]);
		read_lock(&cd_devices_lock);
		if (cd_devices[cminor].cd_str == NULL) {
			read_unlock(&cd_devices_lock);
			return (ENXIO);
		}
	}
	strlog(DRV_ID, cminor, LOG_DEBUG, SL_TRACE,
	       "opening character device %d:%d", cmajor, cminor);
	*devp = makedevice(cd_devices[cminor].cd_major, 0);
	rtn = (*cd_devices[cminor].cd_str->st_rdinit->qi_qopen) (q, devp, oflags, sflag, crp);
	read_unlock(&cd_devices_lock);
	return (rtn);
}

STATIC streamscall int
cd_qclose(queue_t *q, int oflags, cred_t *crp)
{
#ifdef LIS
	/* protect against LiS bugs */
	if (q->q_ptr == NULL) {
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "LiS double-close bug detected");
		return (0);
	}
#endif				/* LIS */
	swerr();
	/* We should never get here.  One of the major reasons is that STREAMS should have
	   allocated the new streamtab because the registered driver should have opened with its
	   own major device numbers. */
	// mi_close_comm(&cd_device_list, q);
	return (0);
}

/*
 *  Registration and Deregistration
 *  ===============================
 */

#ifdef LINUX
unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID number for CD driver (0-for allocation).");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else				/* module_param */
module_param(major, uint, 0);
#endif				/* module_param */
MODULE_PARM_DESC(major, "Major device number for CD driver (0 for allocation).");

#endif				/* LINUX */

#ifdef LFS
STATIC struct cdevsw cd_cdev = {
	.d_name = DRV_NAME,
	.d_str = &cd_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

int __init
cd_init(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = register_strdev(&cd_cdev, major)) < 0) {
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "could not register major %d", major);
		return (err);
	}
	if (err > 0)
		major = err;
	return (0);
}

void __exit
cd_exit(void)
{
	unregister_strdev(&cd_cdev, major);
}

#ifdef MODULE
module_init(cd_init);
module_exit(cd_exit);
#endif				/* MODULE */

#endif				/* LFS */

#ifdef LIS
STATIC int cd_initalized = 0;
STATIC void
cd_init(void)
{
	int err;

	if (cd_initialized != 0)
		return;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = lis_register_strdev(major, &cd_info, UNITS, DRV_NAME)) < 0) {
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "cannot register major %d", major);
		cd_initialized = err;
		return;
	}
	cd_initialized = 1;
	if (major = 0 && err > 0) {
		major = err;
		cd_initialized = 2;
	}
	if ((err = lis_register_driver_qlock_option(major, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strdev(major);
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "cannot register major %d", major);
		cd_initialized = err;
		return;
	}
	return;
}
STATIC void
cd_terminate(void)
{
	int err;

	if (cd_initialized <= 0)
		return;
	if (major) {
		if ((err = lis_unregister_strdev(major)) < 0)
			strlog(DRV_ID, 0, LOG_CRIT, SL_FATAL | SL_CONSOLE,
			       "cannot unregister major %d", major);
		major = 0;
	}
	cd_initialized = 0;
	return;
}

#ifdef MODULE
int
init_module(void)
{
	cd_init();
	if (cd_initialized < 0)
		return cd_initialized;
	return (0);
}

void
cleanup_module(void)
{
	return cd_terminate();
}
#endif				/* MODULE */

#endif				/* LIS */
