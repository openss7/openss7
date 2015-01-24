/*****************************************************************************

 @(#) File: src/modules/connld.c

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

static char const ident[] = "src/modules/connld.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/* 
 *  This is CONNLD, a pipe module which generate new pipes for each open of an
 *  existing pipe-end.
 */

#ifdef NEED_LINUX_AUTOCONF_H
#include NEED_LINUX_AUTOCONF_H
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>

#include "sys/config.h"

#define CONNLD_DESCRIP		"SVR 4.2 Connection Line Discipline (CONNLD) STREAMS Module"
#define CONNLD_EXTRA		"Part of UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define CONNLD_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define CONNLD_REVISION	"OpenSS7 src/modules/connld.c (" PACKAGE_ENVR ") " PACKAGE_DATE
#define CONNLD_DEVICE		"SVR 4.2 CONNLD Module for STREAMS-based pipes"
#define CONNLD_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define CONNLD_LICENSE		"GPL"
#define CONNLD_BANNER		CONNLD_DESCRIP		"\n" \
				CONNLD_EXTRA		"\n" \
				CONNLD_COPYRIGHT	"\n" \
				CONNLD_REVISION		"\n" \
				CONNLD_DEVICE		"\n" \
				CONNLD_CONTACT		"\n"
#define CONNLD_SPLASH		CONNLD_DEVICE		" - " \
				CONNLD_REVISION		"\n"

#ifdef CONFIG_STREAMS_CONNLD_MODULE
MODULE_AUTHOR(CONNLD_CONTACT);
MODULE_DESCRIPTION(CONNLD_DESCRIP);
MODULE_SUPPORTED_DEVICE(CONNLD_DEVICE);
MODULE_LICENSE(CONNLD_LICENSE);
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* CONFIG_STREAMS_CONNLD_MODULE */

#ifndef CONFIG_STREAMS_CONNLD_NAME
//#define CONFIG_STREAMS_CONNLD_NAME "connld"
#error "CONFIG_STREAMS_CONNLD_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_CONNLD_MODID
//#define CONFIG_STREAMS_CONNLD_MODID 12
#error "CONFIG_STREAMS_CONNLD_MODID must be defined."
#endif

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-connld");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

#ifndef CONFIG_STREAMS_CONNLD_MODULE
static
#endif
modID_t modid = CONFIG_STREAMS_CONNLD_MODID;

#ifdef CONFIG_STREAMS_CONNLD_MODULE
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for CONNLD.");
#endif				/* CONFIG_STREAMS_CONNLD_MODULE */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_CONNLD_MODID));
MODULE_ALIAS("streams-module-connld");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

static struct module_info connld_minfo = {
	.mi_idnum = CONFIG_STREAMS_CONNLD_MODID,
	.mi_idname = CONFIG_STREAMS_CONNLD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat connld_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/* 
 *  -------------------------------------------------------------------------
 *
 *  Open and close.
 *
 *  -------------------------------------------------------------------------
 */
static streamscall int
connld_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *credp)
{
	return (ENXIO);
}
static streamscall int
connld_close(queue_t *q, int oflag, cred_t *credp)
{
	return (ENXIO);
}
static streamscall int
connld_putp(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and initialization
 *
 *  -------------------------------------------------------------------------
 */
static struct qinit connld_qinit = {
	.qi_putp = connld_putp,
	.qi_qopen = connld_open,
	.qi_qclose = connld_close,
	.qi_minfo = &connld_minfo,
	.qi_mstat = &connld_mstat,
};

#ifdef CONFIG_STREAMS_CONNLD_MODULE
static
#endif
struct streamtab connldinfo = {
	.st_rdinit = &connld_qinit,
	.st_wrinit = &connld_qinit,
};

static struct fmodsw connld_fmod = {
	.f_name = CONFIG_STREAMS_CONNLD_NAME,
	.f_str = &connldinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_CONNLD_MODULE
static
#endif
int __init
connldinit(void)
{
	int err;

#ifdef CONFIG_STREAMS_CONNLD_MODULE
	printk(KERN_INFO CONNLD_BANNER);
#else
	printk(KERN_INFO CONNLD_SPLASH);
#endif
	connld_minfo.mi_idnum = modid;
	if ((err = register_strmod(&connld_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
};

#ifdef CONFIG_STREAMS_CONNLD_MODULE
static
#endif
void __exit
connldexit(void)
{
	int err;

	if ((err = unregister_strmod(&connld_fmod)) < 0)
		return (void) (err);
	return (void) (0);
};

#ifdef CONFIG_STREAMS_CONNLD_MODULE
module_init(connldinit);
module_exit(connldexit);
#endif
