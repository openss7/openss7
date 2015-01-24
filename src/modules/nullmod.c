/*****************************************************************************

 @(#) File: src/modules/nullmod.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

 *****************************************************************************/

static char const ident[] = "src/modules/nullmod.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/*
 *  This is NULLMOD a STREAMS null module that performs no actions other than acting as a STREAMS
 *  module.  Its purpose is primarily for testing and for serviing as an example of the skeleton of
 *  a STREAMS module.
 *
 *  This is an absurdly simple module.
 *
 *  In addition, this module provide a specialized set of input-output controls for testing and
 *  verifying various internal STREAMS functions.
 */

#ifdef NEED_LINUX_AUTOCONF_H
#include NEED_LINUX_AUTOCONF_H
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#define NULLMOD_DESCRIP		"Null Module (NULLMOD) STREAMS Module"
#define NULLMOD_EXTRA		"Part of UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define NULLMOD_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define NULLMOD_REVISION	"OpenSS7 src/modules/nullmod.c (" PACKAGE_ENVR ") " PACKAGE_DATE
#define NULLMOD_DEVICE		"SVR 4.2 Null Module (NULLMOD) for STREAMS"
#define NULLMOD_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define NULLMOD_LICENSE		"GPL"
#define NULLMOD_BANNER		NULLMOD_DESCRIP		"\n" \
				NULLMOD_EXTRA		"\n" \
				NULLMOD_COPYRIGHT	"\n" \
				NULLMOD_REVISION	"\n" \
				NULLMOD_DEVICE		"\n" \
				NULLMOD_CONTACT		"\n"
#define NULLMOD_SPLASH		NULLMOD_DEVICE		" - " \
				NULLMOD_REVISION	"\n"

#ifdef CONFIG_STREAMS_NULLMOD_MODULE
MODULE_AUTHOR(NULLMOD_CONTACT);
MODULE_DESCRIPTION(NULLMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(NULLMOD_DEVICE);
MODULE_LICENSE(NULLMOD_LICENSE);
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* CONFIG_STREAMS_NULLMOD_MODULE */

#ifndef CONFIG_STREAMS_NULLMOD_NAME
#error "CONFIG_STREAMS_NULLMOD_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_NULLMOD_MODID
#error "CONFIG_STREAMS_NULLMOD_MODID must be defined."
#endif

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-nullmod");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

#ifndef CONFIG_STREAMS_NULLMOD_MODULE
static
#endif
modID_t modid = CONFIG_STREAMS_NULLMOD_MODID;

#ifdef CONFIG_STREAMS_NULLMOD_MODULE
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for NULLMOD.");
#endif				/* CONFIG_STREAMS_NULLMOD_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_NULLMOD_MODID));
MODULE_ALIAS("streams-module-nullmod");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

STATIC struct module_info nullmod_minfo = {
	.mi_idnum = CONFIG_STREAMS_NULLMOD_MODID,
	.mi_idname = CONFIG_STREAMS_NULLMOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

STATIC struct module_stat nullmod_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat nullmod_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/* 
 *  -------------------------------------------------------------------------
 *
 *  Test and Verification Cases
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  Test Case 1: allocb()  Test allocation and freeing of message blocks.
 */
STATIC int
testcase_1(union ioctypes *ioc, mblk_t *dp)
{
	ioc->iocblk.ioc_rval = 0;
	ioc->iocblk.ioc_error = 0;
	ioc->iocblk.ioc_count = 0;
	return (0);
}

STATIC int
testcase_2(union ioctypes *ioc, mblk_t *dp)
{
	ioc->iocblk.ioc_rval = 0;
	ioc->iocblk.ioc_error = 0;
	ioc->iocblk.ioc_count = 0;
	return (0);
}

STATIC int
testcase_3(union ioctypes *ioc, mblk_t *dp)
{
	ioc->iocblk.ioc_rval = 0;
	ioc->iocblk.ioc_error = 0;
	ioc->iocblk.ioc_count = 0;
	return (0);
}

STATIC int
testcase_4(union ioctypes *ioc, mblk_t *dp)
{
	ioc->iocblk.ioc_rval = 0;
	ioc->iocblk.ioc_error = 0;
	ioc->iocblk.ioc_count = 0;
	return (0);
}

STATIC int
testcase_5(union ioctypes *ioc, mblk_t *dp)
{
	ioc->iocblk.ioc_rval = 0;
	ioc->iocblk.ioc_error = 0;
	ioc->iocblk.ioc_count = 0;
	return (0);
}

STATIC int
testcase_6(union ioctypes *ioc, mblk_t *dp)
{
	ioc->iocblk.ioc_rval = 0;
	ioc->iocblk.ioc_error = 0;
	ioc->iocblk.ioc_count = 0;
	return (0);
}

STATIC int
testcase_7(union ioctypes *ioc, mblk_t *dp)
{
	ioc->iocblk.ioc_rval = 0;
	ioc->iocblk.ioc_error = 0;
	ioc->iocblk.ioc_count = 0;
	return (0);
}

STATIC int
testcase_8(union ioctypes *ioc, mblk_t *dp)
{
	ioc->iocblk.ioc_rval = 0;
	ioc->iocblk.ioc_error = 0;
	ioc->iocblk.ioc_count = 0;
	return (0);
}

STATIC int
testcase_9(union ioctypes *ioc, mblk_t *dp)
{
	ioc->iocblk.ioc_rval = 0;
	ioc->iocblk.ioc_error = 0;
	ioc->iocblk.ioc_count = 0;
	return (0);
}

STATIC int
testcase_10(union ioctypes *ioc, mblk_t *dp)
{
	ioc->iocblk.ioc_rval = 0;
	ioc->iocblk.ioc_error = 0;
	ioc->iocblk.ioc_count = 0;
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUT routines
 *
 *  -------------------------------------------------------------------------
 */

STATIC streamscall int
nullmod_wput(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_IOCTL:
	{
		int err = 0;
		union ioctypes *ioc;

		ioc = (typeof(ioc)) mp->b_rptr;
		if (_IOC_TYPE(ioc->iocblk.ioc_cmd) != 'V')
			break;
		switch (_IOC_NR(ioc->iocblk.ioc_cmd)) {
		case 1:
			err = testcase_1(ioc, mp->b_cont);
			break;
		case 2:
			err = testcase_2(ioc, mp->b_cont);
			break;
		case 3:
			err = testcase_3(ioc, mp->b_cont);
			break;
		case 4:
			err = testcase_4(ioc, mp->b_cont);
			break;
		case 5:
			err = testcase_5(ioc, mp->b_cont);
			break;
		case 6:
			err = testcase_6(ioc, mp->b_cont);
			break;
		case 7:
			err = testcase_7(ioc, mp->b_cont);
			break;
		case 8:
			err = testcase_8(ioc, mp->b_cont);
			break;
		case 9:
			err = testcase_9(ioc, mp->b_cont);
			break;
		case 10:
			err = testcase_10(ioc, mp->b_cont);
			break;
		default:
			err = -EINVAL;
			break;
		}
		if (err) {
			mp->b_datap->db_type = M_IOCNAK;
			ioc->iocblk.ioc_count = 0;
			ioc->iocblk.ioc_error = -err;
			ioc->iocblk.ioc_rval = -1;
		} else {
			mp->b_datap->db_type = M_IOCACK;
		}
		qreply(q, mp);
		return (0);
	}
	}
	putnext(q, mp);
	return (0);
}

STATIC streamscall int
nullmod_rput(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
nullmod_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	queue_t *wq;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	wq = WR(q);
	if (sflag == MODOPEN || wq->q_next != NULL) {
		/* inherit packet sizes of what we are pushed over */
		wq->q_minpsz = wq->q_next->q_minpsz;
		wq->q_maxpsz = wq->q_next->q_maxpsz;
		q->q_ptr = wq->q_ptr = q;	/* just set it to something */
		qprocson(q);
		return (0);
	}
	return (EIO);		/* can't be opened as driver */
}
STATIC streamscall int
nullmod_close(queue_t *q, int oflag, cred_t *crp)
{
	(void) oflag;
	(void) crp;
	if (!q->q_ptr)
		return (ENXIO);
	qprocsoff(q);
	q->q_ptr = WR(q)->q_ptr = NULL;
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and initialization
 *
 *  -------------------------------------------------------------------------
 */
STATIC struct qinit nullmod_rinit = {
	.qi_putp = nullmod_rput,
	.qi_qopen = nullmod_open,
	.qi_qclose = nullmod_close,
	.qi_minfo = &nullmod_minfo,
	.qi_mstat = &nullmod_rstat,
};

STATIC struct qinit nullmod_winit = {
	.qi_putp = nullmod_wput,
	.qi_minfo = &nullmod_minfo,
	.qi_mstat = &nullmod_wstat,
};

#ifdef CONFIG_STREAMS_NULLMOD_MODULE
STATIC
#endif
struct streamtab nullmodinfo = {
	.st_rdinit = &nullmod_rinit,
	.st_wrinit = &nullmod_winit,
};

STATIC struct fmodsw nullmod_fmod = {
	.f_name = CONFIG_STREAMS_NULLMOD_NAME,
	.f_str = &nullmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

struct nullmod_ioctl {
	unsigned int cmd;
	void *opaque;
};

static struct nullmod_ioctl nullmod_map[] = {
	{.cmd = (('V' << 8) | 1),}
	, {.cmd = (('V' << 8) | 2),}
	, {.cmd = (('V' << 8) | 3),}
	, {.cmd = (('V' << 8) | 4),}
	, {.cmd = (('V' << 8) | 5),}
	, {.cmd = (('V' << 8) | 6),}
	, {.cmd = (('V' << 8) | 7),}
	, {.cmd = (('V' << 8) | 8),}
	, {.cmd = (('V' << 8) | 9),}
	, {.cmd = (('V' << 8) | 10),}
	, {.cmd = 0,}
};

static void
nullmod_unregister_ioctl32(void)
{
	struct nullmod_ioctl *i;

	for (i = nullmod_map; i->cmd != 0; i++)
		if (i->opaque != NULL)
			unregister_ioctl32(i->opaque);
}

static int
nullmod_register_ioctl32(void)
{
	struct nullmod_ioctl *i;

	for (i = nullmod_map; i->cmd != 0; i++) {
		if ((i->opaque = register_ioctl32(i->cmd)) == NULL) {
			nullmod_unregister_ioctl32();
			return (-ENOMEM);
		}
	}
	return (0);
}

#ifdef CONFIG_STREAMS_NULLMOD_MODULE
STATIC
#endif
int __init
nullmodinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_NULLMOD_MODULE
	printk(KERN_INFO NULLMOD_BANNER);
#else
	printk(KERN_INFO NULLMOD_SPLASH);
#endif
	nullmod_minfo.mi_idnum = modid;
	if ((err = nullmod_register_ioctl32()) < 0)
		return (err);
	if ((err = register_strmod(&nullmod_fmod)) < 0) {
		nullmod_unregister_ioctl32();
		return (err);
	}
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
}

#ifdef CONFIG_STREAMS_NULLMOD_MODULE
STATIC
#endif
void __exit
nullmodexit(void)
{
	unregister_strmod(&nullmod_fmod);
	nullmod_unregister_ioctl32();
}

#ifdef CONFIG_STREAMS_NULLMOD_MODULE
module_init(nullmodinit);
module_exit(nullmodexit);
#endif
