/*****************************************************************************

 @(#) $RCSfile: strmain.c,v $ $Name:  $($Revision: 0.9.2.37 $) $Date: 2008-04-28 12:54:05 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2008-04-28 12:54:05 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strmain.c,v $
 Revision 0.9.2.37  2008-04-28 12:54:05  brian
 - update file headers for release

 Revision 0.9.2.36  2007/12/15 20:19:55  brian
 - updates

 Revision 0.9.2.35  2007/08/15 05:33:23  brian
 - GPLv3 updates

 Revision 0.9.2.34  2007/08/13 22:46:17  brian
 - GPLv3 header updates

 Revision 0.9.2.33  2007/07/14 01:35:54  brian
 - make license explicit, add documentation

 Revision 0.9.2.32  2006/12/18 10:08:59  brian
 - updated headers for release

 *****************************************************************************/

#ident "@(#) $RCSfile: strmain.c,v $ $Name:  $($Revision: 0.9.2.37 $) $Date: 2008-04-28 12:54:05 $"

static char const ident[] =
    "$RCSfile: strmain.c,v $ $Name:  $($Revision: 0.9.2.37 $) $Date: 2008-04-28 12:54:05 $";

#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/compiler.h>
#include <linux/kernel.h>

#include "sys/config.h"

#define STREAMS_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define STREAMS_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define STREAMS_REVISION	"LfS $RCSfile: strmain.c,v $ $Name:  $($Revision: 0.9.2.37 $) $Date: 2008-04-28 12:54:05 $"
#define STREAMS_DEVICE		"SVR 4.2 STREAMS Subsystem"
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

/* 
 *  We put all our heavily used globals handy.  Hopefully by placing these all
 *  together we keep them in the same cache slot or two.
 */
#ifdef CONFIG_STREAMS_CLONE
extern int clone_init(void);
extern void clone_exit(void);
#endif
#ifdef CONFIG_STREAMS_FIFO
extern int fifo_init(void);
extern void fifo_exit(void);
#endif
#ifdef CONFIG_STREAMS_LOOP
extern int loop_init(void);
extern void loop_exit(void);
#endif
#ifdef CONFIG_STREAMS_SAD
extern int sad_init(void);
extern void sad_exit(void);
#endif
#ifdef CONFIG_STREAMS_NSDEV
extern int nsdev_init(void);
extern void nsdev_exit(void);
#endif
#ifdef CONFIG_STREAMS_ECHO
extern int echo_init(void);
extern void echo_exit(void);
#endif
#ifdef CONFIG_STREAMS_NULS
extern int nuls_init(void);
extern void nuls_exit(void);
#endif
#ifdef CONFIG_STREAMS_PIPE
extern int pipe_init(void);
extern void pipe_exit(void);
#endif
#ifdef CONFIG_STREAMS_SOCKSYS
extern int socksys_init(void);
extern void socksys_exit(void);
#endif
#ifdef CONFIG_STREAMS_UNIX
extern int unix_init(void);
extern void unix_exit(void);
#endif
#ifdef CONFIG_STREAMS_XNET
extern int xnet_init(void);
extern void xnet_exit(void);
#endif
#ifdef CONFIG_STREAMS_LOG
extern int log_init(void);
extern void log_exit(void);
#endif
#ifdef CONFIG_STREAMS_INET
extern int inet_init(void);
extern void inet_exit(void);
#endif
#ifdef CONFIG_STREAMS_STH
extern int sth_init(void);
extern void sth_exit(void);
#endif
#ifdef CONFIG_STREAMS_PIPEMOD
extern int pipemod_init(void);
extern void pipemod_exit(void);
#endif
#ifdef CONFIG_STREAMS_CONNLD
extern int connld_init(void);
extern void connld_exit(void);
#endif
#ifdef CONFIG_STREAMS_TIMOD
extern int timod_init(void);
extern void timod_exit(void);
#endif
#ifdef CONFIG_STREAMS_TIRDWR
extern int tirdwr_init(void);
extern void tirdwr_exit(void);
#endif
#ifdef CONFIG_STREAMS_SOCKMOD
extern int sockmod_init(void);
extern void sockmod_exit(void);
#endif
#ifdef CONFIG_STREAMS_SC
extern int sc_init(void);
extern void sc_exit(void);
#endif

static int
modules_init(void)
{
#ifdef CONFIG_STREAMS_CLONE
	clone_init();
#endif
#ifdef CONFIG_STREAMS_FIFO
	fifo_init();
#endif
#ifdef CONFIG_STREAMS_LOOP
	loop_init();
#endif
#ifdef CONFIG_STREAMS_SAD
	sad_init();
#endif
#ifdef CONFIG_STREAMS_NSDEV
	nsdev_init();
#endif
#ifdef CONFIG_STREAMS_ECHO
	echo_init();
#endif
#ifdef CONFIG_STREAMS_NULS
	nuls_init();
#endif
#ifdef CONFIG_STREAMS_PIPE
	pipe_init();
#endif
#ifdef CONFIG_STREAMS_SOCKSYS
	socksys_init();
#endif
#ifdef CONFIG_STREAMS_UNIX
	unix_init();
#endif
#ifdef CONFIG_STREAMS_XNET
	xnet_init();
#endif
#ifdef CONFIG_STREAMS_LOG
	log_init();
#endif
#ifdef CONFIG_STREAMS_INET
	inet_init();
#endif
#ifdef CONFIG_STREAMS_STH
	sth_init();
#endif
#ifdef CONFIG_STREAMS_PIPEMOD
	pipemod_init();
#endif
#ifdef CONFIG_STREAMS_CONNLD
	connld_init();
#endif
#ifdef CONFIG_STREAMS_TIMOD
	timod_init();
#endif
#ifdef CONFIG_STREAMS_TIRDWR
	tirdwr_init();
#endif
#ifdef CONFIG_STREAMS_SOCKMOD
	sockmod_init();
#endif
#ifdef CONFIG_STREAMS_SC
	sc_init();
#endif
	return (0);
}

static void
modules_exit(void)
{
#ifdef CONFIG_STREAMS_SC
	sc_exit();
#endif
#ifdef CONFIG_STREAMS_SOCKMOD
	sockmod_exit();
#endif
#ifdef CONFIG_STREAMS_TIRDWR
	tirdwr_exit();
#endif
#ifdef CONFIG_STREAMS_TIMOD
	timod_exit();
#endif
#ifdef CONFIG_STREAMS_CONNLD
	connld_exit();
#endif
#ifdef CONFIG_STREAMS_PIPEMOD
	pipemod_exit();
#endif
#ifdef CONFIG_STREAMS_STH
	sth_exit();
#endif
#ifdef CONFIG_STREAMS_INET
	inet_exit();
#endif
#ifdef CONFIG_STREAMS_LOG
	log_exit();
#endif
#ifdef CONFIG_STREAMS_XNET
	xnet_exit();
#endif
#ifdef CONFIG_STREAMS_UNIX
	unix_exit();
#endif
#ifdef CONFIG_STREAMS_SOCKSYS
	socksys_exit();
#endif
#ifdef CONFIG_STREAMS_PIPE
	pipe_exit();
#endif
#ifdef CONFIG_STREAMS_NULS
	nuls_exit();
#endif
#ifdef CONFIG_STREAMS_ECHO
	echo_exit();
#endif
#ifdef CONFIG_STREAMS_NSDEV
	nsdev_exit();
#endif
#ifdef CONFIG_STREAMS_SAD
	sad_exit();
#endif
#ifdef CONFIG_STREAMS_LOOP
	loop_exit();
#endif
#ifdef CONFIG_STREAMS_FIFO
	fifo_exit();
#endif
#ifdef CONFIG_STREAMS_CLONE
	clone_exit();
#endif
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
	modules_init();
	return (0);
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
