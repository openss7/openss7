/*****************************************************************************

 @(#) $RCSfile: strmain.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/05/03 06:30:21 $

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

 Last Modified $Date: 2004/05/03 06:30:21 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strmain.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/05/03 06:30:21 $"

static char const ident[] =
    "$RCSfile: strmain.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/05/03 06:30:21 $";

#include <linux/config.h>
#include <linux/version.h>
#include <linux/modversions.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/compiler.h>
#include <linux/kernel.h>

#ifndef __GENKSYMS__
#include <sys/modversions.h>
#endif

#include "strdebug.h"

#define STREAMS_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define STREAMS_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define STREAMS_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/05/03 06:30:21 $"
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

MODULE_AUTHOR(STREAMS_CONTACT);
MODULE_DESCRIPTION(STREAMS_DESCRIP);
MODULE_SUPPORTED_DEVICE(STREAMS_DEVICE);
MODULE_LICENSE(STREAMS_LICENSE);

#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strsubr.h>
#include <sys/strconf.h>
#include <sys/ddi.h>

#include "strprocfs.h"
#include "strspecfs.h"
#include "strhead.h"		/* for str_minfo */
#include "strsysctl.h"
#include "strsched.h"
#include "strreg.h"

/* 
 *  We put all our heavily used globals handy.  Hopefully by placing these all
 *  together we keep them in the same cache slot or two.
 */

static int __init streams_init(void)
{
	int result;
#ifdef MODULE
	printk(KERN_INFO STREAMS_BANNER);	/* log message */
#else
	printk(KERN_INFO STREAMS_SPLASH);	/* console splash */
#endif
	if ((result = strprocfs_init()))
		goto no_procfs;
	if ((result = strspecfs_init()))
		goto no_specfs;
	if ((result = strsysctl_init()))
		goto no_strsysctl;
	if ((result = strsched_init()))
		goto no_strsched;
	if ((result = strreg_init()))
		goto no_strreg;
	return (0);
      no_strreg:
	strsched_exit();
      no_strsched:
	strsysctl_exit();
      no_strsysctl:
	strspecfs_exit();
      no_specfs:
	strprocfs_exit();
      no_procfs:
	return (result);
}

static void __exit streams_exit(void)
{
	strreg_exit();
	strsched_exit();
	strsysctl_exit();
	strspecfs_exit();
	strprocfs_exit();
}

module_init(streams_init);
module_exit(streams_exit);
