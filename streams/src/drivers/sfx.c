/*****************************************************************************

 @(#) $RCSfile: sfx.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2004/05/29 08:28:14 $

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

 Last Modified $Date: 2004/05/29 08:28:14 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sfx.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2004/05/29 08:28:14 $"

static char const ident[] =
    "$RCSfile: sfx.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2004/05/29 08:28:14 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/modversions.h>
#include <linux/init.h>

#ifndef __GENKSYMS__
#include <sys/streams/modversions.h>
#endif

#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include "sys/config.h"
#include "strdebug.h"
#include "strspecfs.h"		/* for str_args */
#include "strsched.h"		/* for sd_get/sd_put */
#include "strsad.h"		/* for autopush */
#include "sth.h"
#include "fifo.h"		/* for fifo stuff */

#define SFX_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SFX_COPYRIGHT	"Copyright (c) 1997-2004 OpenSS7 Corporation.  All Rights Reserved."
#define SFX_REVISION	"LfS $RCSFile$ $Name:  $($Revision: 0.9.2.8 $) $Date: 2004/05/29 08:28:14 $"
#define SFX_DEVICE	"SVR 4.2 STREAMS-based FIFOs"
#define SFX_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SFX_LICENSE	"GPL"
#define SFX_BANNER	SFX_DESCRIP	"\n" \
			SFX_COPYRIGHT	"\n" \
			SFX_REVISION	"\n" \
			SFX_DEVICE	"\n" \
			SFX_CONTACT	"\n"
#define SFX_SPLASH	SFX_DESCRIP	" - " \
			SFX_COPYRIGHT	"\n"

#ifdef CONFIG_STREAMS_SFX_MODULE
MODULE_AUTHOR(SFX_CONTACT);
MODULE_DESCRIPTION(SFX_DESCRIP);
MODULE_SUPPORTED_DEVICE(SFX_DEVICE);
MODULE_LICENSE(SFX_LICENSE);
#endif

#ifndef CONFIG_STREAMS_SFX_NAME
//#define CONFIG_STREAMS_SFX_NAME "sfx"
#error "CONFIG_STREAMS_SFX_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_SFX_MODID
//#define CONFIG_STREAMS_SFX_MODID 9
#error "CONFIG_STREAMS_SFX_MODID must be defined."
#endif
#ifndef CONFIG_STREAMS_SFX_MAJOR
//#define CONFIG_STREAMS_SFX_MAJOR 0
#error "CONFIG_STREAMS_SFX_MAJOR must be defined."
#endif

modID_t modid = CONFIG_STREAMS_SFX_MODID;
MODULE_PARM(modid, "h");
MODULE_PARM_DESC(modid, "Module id number for STREAMS-based FIFOs (0 for allocation).");

major_t major = CONFIG_STREAMS_SFX_MAJOR;
MODULE_PARM(major, "h");
MODULE_PARM_DESC(major, "Major device number for STREAMS-based FIFOs (0 for allocation).");

unsigned short modid = CONFIG_STREAMS_SFX_MODID;
MODULE_PARM(modid, "h");
MODULE_PARM_DESC(modid, "Module identification number for STREAMS-based FIFOs");

static struct module_info sfx_minfo = {
	mi_idnum:CONFIG_STREAMS_SFX_MODID,
	mi_idname:CONFIG_STREAMS_SFX_NAME,
	mi_minpsz:0,
	mi_maxpsz:INFPSZ,
	mi_hiwat:STRHIGH,
	mi_lowat:STRLOW,
};

static struct qinit sfx_rinit = {
	qi_putp:strrput,
	qi_qopen:sfx_open,
	qi_qclose:sfx_close,
	qi_minfo:&sfx_minfo,
};

static struct qinit sfx_winit = {
	qi_srvp:strwsrv,
	qi_minfo:&sfx_minfo,
};

static struct streamtab sfx_info = {
	st_rdinit:&sfx_rinit,
	st_wrinit:&sfx_winit,
};

#define stri_lookup(__f) ((struct stdata *)(__f)->private_data)

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
static int sfx_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int err;
	MOD_INC_USE_COUNT;	/* keep module from unloading */
	if (q->q_ptr != NULL) {
		/* we walk down the queue chain calling open on each of the modules and the driver */
		queue_t *wq = WR(q), *wq_next;
		wq_next = SAMESTR(wq) ? wq->q_next : NULL;
		while ((wq = wq_next)) {
			/* all opens are module opens on fifos, there is no driver */
			wq_next = SAMESTR(wq) ? wq->q_next : NULL;
			if ((err = qopen(wq - 1, devp, oflag, MODOPEN, crp)))
				goto error;
		}
		MOD_DEC_USE_COUNT;	/* already open */
		return (0);
	}
	if (sflag == DRVOPEN || sflag == CLONEOPEN || WR(q)->q_next == NULL) {
		dev_t dev = *devp;
		struct stdata *sd;
		if ((sd = ((struct queinfo *) q)->qu_str)) {
			struct cdevsw *sdev = sd->sd_cdevsw;
			/* 1st step: attach the driver and call its open routine */
			/* we are the driver and this *is* the open routine */
			/* 2nd step: check for redirected return */
			/* we are the driver and this *is* the open routine and there is no
			   redirection. */
			/* 3rd step: autopush modules and call their open routines */
			if ((err = autopush(sd, sdev, &dev, oflag, MODOPEN, crp)))
				goto error;
			/* lastly, attach our privates and return */
			q->q_ptr = WR(q)->q_ptr = sd;
			return (0);
		}
	}
	err = -EIO;		/* can't be opened as module or clone */
      error:
	MOD_DEC_USE_COUNT;
	return (err <= 0 ? err : -err);
}
static int sfx_close(queue_t *q, int oflag, cred_t *crp)
{
	return (0);
}

static struct cdevsw sfx_cdev = {
	d_name:CONFIG_STREAMS_SFX_NAME,
	d_str:&sfx_info,
	d_flag:0,
	d_fop:&sfx_ops,
	d_mode:S_IFIFO,
	d_kmod:THIS_MODULE,
};

static int __init sfx_init(void)
{
	int err;
#ifdef CONFIG_STREAMS_SFX_MODULE
	printk(KERN_INFO SFX_BANNER);
#else
	printk(KERN_INFO SFX_SPLASH);
#endif
	sfx_minfo.mi_idnum = modid;
	if ((err = register_strdev(&sfx_cdev, major)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	return (0);
};
static void __exit sfx_exit(void)
{
	unregister_strdev(&sfx_cdev, major);
};

#ifdef CONFIG_STREAMS_SFX_MODULE
module_init(sfx_init);
module_exit(sfx_exit);
#endif
