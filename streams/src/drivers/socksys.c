/*****************************************************************************

 @(#) $RCSfile: socksys.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/05/24 04:16:30 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/05/24 04:16:30 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: socksys.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/05/24 04:16:30 $"

static char const ident[] =
    "$RCSfile: socksys.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/05/24 04:16:30 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/modversions.h>
#include <linux/init.h>

#include <linux/smp_lock.h>
#include <linux/slab.h>

#ifndef __GENKSYMS__
#include <sys/streams/modversions.h>
#endif

#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include <tihdr.h>

#include "sys/config.h"
#include "strdebug.h"
//#include "strsocksys.h"               /* extern verification */
#include "strreg.h"

#define SOCKSYS_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SOCKSYS_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define SOCKSYS_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.5 $) $Date: 2004/05/24 04:16:30 $"
#define SOCKSYS_DEVICE		"SVR 4.2 STREAMS Sockets Support"
#define SOCKSYS_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define SOCKSYS_LICENSE		"GPL"
#define SOCKSYS_BANNER		SOCKSYS_DESCRIP		"\n" \
				SOCKSYS_COPYRIGHT	"\n" \
				SOCKSYS_REVISION	"\n" \
				SOCKSYS_DEVICE		"\n" \
				SOCKSYS_CONTACT		"\n"
#define SOCKSYS_SPLASH		SOCKSYS_DEVICE		" - " \
				SOCKSYS_REVISION	"\n"

#ifdef CONFIG_STREAMS_SOCKSYS_MODULE
MODULE_AUTHOR(SOCKSYS_CONTACT);
MODULE_DESCRIPTION(SOCKSYS_DESCRIP);
MODULE_SUPPORTED_DEVICE(SOCKSYS_DEVICE);
MODULE_LICENSE(SOCKSYS_LICENSE);
#endif

#ifndef CONFIG_STREAMS_SOCKSYS_NAME
#define CONFIG_STREAMS_SOCKSYS_NAME "socksys"
//#error "CONFIG_STREAMS_SOCKSYS_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_SOCKSYS_MAJOR
#define CONFIG_STREAMS_SOCKSYS_MAJOR 0
//#error "CONFIG_STREAMS_SOCKSYS_MAJOR must be defined."
#endif

static unsigned short major = CONFIG_STREAMS_SOCKSYS_MAJOR;
MODULE_PARM(major, "b");
MODULE_PARM_DESC(major, "Major device number for SOCKSYS driver (0 for allocation).");

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUTP and SRVP
 *
 *  -------------------------------------------------------------------------
 */
static int socksys_rsrv(queue_t *q)
{
	return (-EFAULT);
}
static int socksys_wsrv(queue_t *q)
{
	return (-EFAULT);
}
static int socksys_wput(queue_t *q, mblk_t *mp)
{
	return (-EFAULT);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
static int socksys_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	return (-EFAULT);
}
static int socksys_close(queue_t *q, int oflag, cred_t *crp)
{
	return (-EFAULT);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  INITIALIZATION
 *
 *  -------------------------------------------------------------------------
 */
static struct qinit socksys_rinit = {
	qi_putp:putq,
	qi_srvp:socksys_rsrv,
	qi_qopen:socksys_open,
	qi_qclose:socksys_close,
	qi_minfo:&socksys_minfo,
};

static struct qinit socksys_winit = {
	qi_putp:socksys_wput,
	qi_srvp:socksys_wsrv,
	qi_minfo:&socksys_minfo,
};

static struct streamtab socksys_info = {
	st_rdinit:&socksys_rinit,
	st_wrinit:&socksys_winit,
};

static struct cdevsw socksys_cdev = {
	d_name:CONFIG_STREAMS_SOCKSYS_NAME,
	d_str:&socksys_info,
	d_flag:D_CLONE,
	d_fop:NULL,
	d_mode:S_IFCHR,
	d_kmod:THIS_MODULE,
};

static int __init socksys_init(void)
{
	int err;
#ifdef CONFIG_STREAMS_SOCKSYS_MODULE
	printk(KERN_INFO SOCKSYS_BANNER);
#else
	printk(KERN_INFO SOCKSYS_SPLASH);
#endif
	if ((err = register_strdev(&socksys_cdev, major)) < 0)
		return (err);
	if (err > 0)
		major = err;
	return (0);
}
static void __exit socksys_exit(void)
{
	unregister_strdev(&socksys_cdev, major);
}

#ifdef CONFIG_STREAMS_SOCKSYS_MODULE
module_init(socksys_init);
module_exit(socksys_exit);
#endif
