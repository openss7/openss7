/*****************************************************************************

 @(#) File: src/kernel/strmain.c

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

static char const ident[] = "src/kernel/strmain.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

#ifdef NEED_LINUX_AUTOCONF_H
#include NEED_LINUX_AUTOCONF_H
#endif
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/compiler.h>
#include <linux/kernel.h>

#include "sys/config.h"

#define STREAMS_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define STREAMS_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define STREAMS_REVISION	"OpenSS7 File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE
#define STREAMS_DEVICE		"SVR 4.2 MP STREAMS Subsystem"
#define STREAMS_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define STREAMS_LICENSE		"GPL"
#define STREAMS_BANNER		STREAMS_DESCRIP		"\n" \
				STREAMS_COPYRIGHT	"\n" \
				STREAMS_REVISION	"\n" \
				STREAMS_DEVICE		"\n" \
				STREAMS_CONTACT		"\n"
#define STREAMS_SPLASH		STREAMS_DEVICE		" - " \
				STREAMS_REVISION	"\n"

#ifdef CONFIG_STREAMS_MODULE
MODULE_AUTHOR(STREAMS_CONTACT);
MODULE_DESCRIPTION(STREAMS_DESCRIP);
MODULE_SUPPORTED_DEVICE(STREAMS_DEVICE);
MODULE_LICENSE(STREAMS_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams");
#endif
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif
#endif
#if defined HAVE_KINC_LINUX_SECURITY_H
#include <linux/security.h>	/* avoid ptrace conflict */
#endif

#include "sys/strdebug.h"

#include <sys/stream.h>
#include <sys/strsubr.h>
#include <sys/strconf.h>
#include <sys/ddi.h>

#include "strprocfs.h"
#include "src/kernel/strspecfs.h"	/* for specfs_mount and specfs_umount */
#include "src/kernel/strsysctl.h"
#include "src/kernel/strsched.h"
#include "src/kernel/strreg.h"
#include "src/modules/sth.h"	/* for str_minfo */

#include "src/kernel/strmain.h"
#include "sys/modconf.h"

/* 
 *  We put all our heavily used globals handy.  Hopefully by placing these all
 *  together we keep them in the same cache slot or two.
 */

static int
modules_init(void)
{
	module_config_t *m;
	driver_config_t *d;
	int i, err;

	for (i = 0, m = &lfs_module_config[0]; i < sizeof(lfs_module_config)/sizeof(lfs_module_config[0]); i++, m++)
		if (m->cnf_init != NULL)
			if ((err = (*m->cnf_init)())) {
				for (i--, m--; i >= 0; i--, m--)
					if (m->cnf_term != NULL)
						(*m->cnf_term)();
				return (err);
			}
	for (i = 0, d = &lfs_driver_config[0]; i < sizeof(lfs_driver_config)/sizeof(lfs_driver_config[0]); i++, d++)
		if (d->cnf_init != NULL)
			if ((err = (*d->cnf_init)())) {
				for (i--, d--; i >= 0; i--, d--)
					if (d->cnf_term != NULL)
						(*d->cnf_term)();
				for (i = sizeof(lfs_module_config)/sizeof(lfs_module_config[0]), m = &lfs_module_config[i-1]; i > 0; i--, m--)
					if (m->cnf_term != NULL)
						(*m->cnf_term)();
				return (err);
			}
	return (0);
}

static void
modules_exit(void)
{
	driver_config_t *d;
	module_config_t *m;
	int i;

	for (i = sizeof(lfs_driver_config)/sizeof(lfs_driver_config[0]), d = &lfs_driver_config[i-1]; i > 0; i--, d--)
		if (d->cnf_term != NULL)
			(*d->cnf_term)();
	for (i = sizeof(lfs_module_config)/sizeof(lfs_module_config[0]), m = &lfs_module_config[i-1]; i > 0; i--, m--)
		if (m->cnf_term != NULL)
			(*m->cnf_term)();
}

#ifdef CONFIG_STREAMS_MODULE
static
#endif
int __init
streams_init(void)
{
	int result = -EINVAL;

#ifdef CONFIG_STREAMS_MODULE
	printk(KERN_INFO STREAMS_BANNER);	/* log message */
#else
	printk(KERN_INFO STREAMS_SPLASH);	/* console splash */
#endif
	if (!specfs_mount())
		goto no_specfs;
	if ((result = strprocfs_init()))
		goto no_procfs;
	if ((result = strsysctl_init()))
		goto no_strsysctl;
	if ((result = strsched_init()))
		goto no_strsched;
	if ((result = modules_init()))
		goto no_modules;
	return (0);
      no_modules:
	strsched_exit();
      no_strsched:
	strsysctl_exit();
      no_strsysctl:
	strprocfs_exit();
      no_procfs:
	specfs_umount();
      no_specfs:
	return (result);
}

#ifdef CONFIG_STREAMS_MODULE
static
#endif
void __exit
streams_exit(void)
{
	modules_exit();
	strsched_exit();
	strsysctl_exit();
	strprocfs_exit();
	specfs_umount();
}

#ifdef CONFIG_STREAMS_MODULE
module_init(streams_init);
module_exit(streams_exit);
#endif
