/*****************************************************************************

 @(#) $RCSfile: strsyms.c,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2004/05/05 23:10:10 $

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

 Last Modified $Date: 2004/05/05 23:10:10 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strsyms.c,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2004/05/05 23:10:10 $"

static char const ident[] =
    "$RCSfile: strsyms.c,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2004/05/05 23:10:10 $";

//#define __NO_VERSION__
//#define EXPORT_SYMTAB

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/modversions.h>

#ifndef __GENKSYMS__
#include <sys/modversions.h>
#endif

/* 
 *  This is my solution for those who don't want to inline GPL'ed functions or
 *  who don't use optimizations when compiling or specifies
 *  -fnoinline-functions or something of the like.  This file implements all
 *  of the extern inlines from the header files by just including the header
 *  files with the functions declared 'inline' instead of 'extern inline'.
 *
 *  There are implemented here in a separate object, out of the way of the
 *  modules that don't use them.
 */

#define __EXTERN_INLINE inline

#include <sys/kmem.h>
#include <sys/cmn_err.h>

#include <sys/strlog.h>
#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>

#if defined(CONFIG_STREAMS_COMPAT_SVR4)||defined(CONFIG_STREAMS_COMPAT_SVR4_MODULE)
#define _SVR4_SOURCE
#endif
#if defined(CONFIG_STREAMS_COMPAT_AIX)||defined(CONFIG_STREAMS_COMPAT_AIX_MODULE)
#define _AIX_SOURCE
#endif
#if defined(CONFIG_STREAMS_COMPAT_HPUX)||defined(CONFIG_STREAMS_COMPAT_HPUX_MODULE)
#define _HPUX_SOURCE
#endif
#if defined(CONFIG_STREAMS_COMPAT_OSF)||defined(CONFIG_STREAMS_COMPAT_OSF_MODULE)
#define _OSF_SOURCE
#endif
#if defined(CONFIG_STREAMS_COMPAT_SUN)||defined(CONFIG_STREAMS_COMPAT_SUN_MODULE)
#define _SUN_SOURCE
#endif
#if defined(CONFIG_STREAMS_COMPAT_UW7)||defined(CONFIG_STREAMS_COMPAT_UW7_MODULE)
#define _UW7_SOURCE
#endif
#if defined(CONFIG_STREAMS_COMPAT_LIS)||defined(CONFIG_STREAMS_COMPAT_LIS_MODULE)
#define _LIS_SOURCE
#endif

#undef STR
#include <sys/ddi.h>

#include "strdebug.h"
#include "strspecfs.h"
#include "strsched.h"
#include "strreg.h"
#include "sth.h"		/* for str_minfo */
#include "strsysctl.h"
#include "fifo.h"
#include "strattach.h"

EXPORT_SYMBOL(adjmsg);		/* stream.h */
EXPORT_SYMBOL(allocb);		/* stream.h */
EXPORT_SYMBOL(backq);		/* stream.h */
EXPORT_SYMBOL(bcanputnext);	/* stream.h */
EXPORT_SYMBOL(bcanput);		/* stream.h */
EXPORT_SYMBOL(bcopy);		/* ddi.h */
EXPORT_SYMBOL(bufcall);		/* stream.h */
EXPORT_SYMBOL(bzero);		/* ddi.h */
EXPORT_SYMBOL(canenable);	/* stream.h */
EXPORT_SYMBOL(canputnext);	/* stream.h */
EXPORT_SYMBOL(canput);		/* stream.h */
EXPORT_SYMBOL(cmn_err);		/* cmn_err.h */
EXPORT_SYMBOL(copyb);		/* stream.h */
EXPORT_SYMBOL(copyin);		/* ddi.h */
EXPORT_SYMBOL(copymsg);		/* stream.h */
EXPORT_SYMBOL(copyout);		/* ddi.h */
EXPORT_SYMBOL(datamsg);		/* stream.h */
EXPORT_SYMBOL(delay);		/* ddi.h */
EXPORT_SYMBOL(drv_getparm);	/* ddi.h */
EXPORT_SYMBOL(drv_hztomsec);	/* ddi.h */
EXPORT_SYMBOL(drv_hztousec);	/* ddi.h */
EXPORT_SYMBOL(drv_msectohz);	/* ddi.h */
EXPORT_SYMBOL(drv_priv);	/* ddi.h */
EXPORT_SYMBOL(drv_usectohz);	/* ddi.h */
EXPORT_SYMBOL(drv_usecwait);	/* ddi.h */
EXPORT_SYMBOL(dupb);		/* stream.h */
EXPORT_SYMBOL(dupmsg);		/* stream.h */
EXPORT_SYMBOL(enableok);	/* stream.h */
EXPORT_SYMBOL(enableq);		/* stream.h */
EXPORT_SYMBOL(esballoc);	/* stream.h */
EXPORT_SYMBOL(flushband);	/* stream.h */
EXPORT_SYMBOL(flushq);		/* stream.h */
EXPORT_SYMBOL(freeb);		/* stream.h */
EXPORT_SYMBOL(freemsg);		/* stream.h */
EXPORT_SYMBOL(freezestr);	/* stream.h */
EXPORT_SYMBOL(getadmin);	/* stream.h, strreg.c */
EXPORT_SYMBOL(getmajor);	/* ddi.h */
EXPORT_SYMBOL(getmid);		/* stream.h, strreg.c */
EXPORT_SYMBOL(getminor);	/* ddi.h */
EXPORT_SYMBOL(getq);		/* stream.h */
EXPORT_SYMBOL(insq);		/* stream.h */
EXPORT_SYMBOL(kmem_alloc);	/* kmem.h */
EXPORT_SYMBOL(kmem_free);	/* kmem.h */
EXPORT_SYMBOL(linkb);		/* stream.h */
EXPORT_SYMBOL(makedevice);	/* ddi.h */
//EXPORT_SYMBOL(max);           /* ddi.h */
//EXPORT_SYMBOL(min);           /* ddi.h */
EXPORT_SYMBOL(msgdsize);	/* stream.h */
EXPORT_SYMBOL(msgpullup);	/* stream.h */
EXPORT_SYMBOL(noenable);	/* stream.h */
EXPORT_SYMBOL(OTHERQ);		/* stream.h */
EXPORT_SYMBOL(pcmsg);		/* stream.h */
EXPORT_SYMBOL(pullupmsg);	/* stream.h */
EXPORT_SYMBOL(putbq);		/* stream.h */
EXPORT_SYMBOL(putctl1);		/* stream.h */
EXPORT_SYMBOL(putctl);		/* stream.h */
EXPORT_SYMBOL(putnextctl1);	/* stream.h */
EXPORT_SYMBOL(putnextctl);	/* stream.h */
EXPORT_SYMBOL(putnext);		/* stream.h */
EXPORT_SYMBOL(putq);		/* stream.h */
EXPORT_SYMBOL(put);		/* stream.h */
EXPORT_SYMBOL(qenable);		/* stream.h */
EXPORT_SYMBOL(qprocsoff);	/* stream.h */
EXPORT_SYMBOL(qprocson);	/* stream.h */
EXPORT_SYMBOL(qreply);		/* stream.h */
EXPORT_SYMBOL(qsize);		/* stream.h */
EXPORT_SYMBOL(RD);		/* stream.h */
EXPORT_SYMBOL(rmvb);		/* stream.h */
EXPORT_SYMBOL(rmvq);		/* stream.h */
EXPORT_SYMBOL(SAMESTR);		/* stream.h */
EXPORT_SYMBOL(strlog);		/* stream.h */
EXPORT_SYMBOL(strqget);		/* stream.h */
EXPORT_SYMBOL(strqset);		/* stream.h */
EXPORT_SYMBOL(testb);		/* stream.h */
EXPORT_SYMBOL(timeout);		/* ddi.h */
EXPORT_SYMBOL(unbufcall);	/* stream.h */
EXPORT_SYMBOL(unfreezestr);	/* stream.h */
EXPORT_SYMBOL(unlinkb);		/* stream.h */
EXPORT_SYMBOL(untimeout);	/* ddi.h */
EXPORT_SYMBOL(WR);		/* stream.h */

EXPORT_SYMBOL(linkmsg);		/* stream.h LfS */
EXPORT_SYMBOL(putctl2);		/* stream.h D3str+ */
EXPORT_SYMBOL(putnextctl2);	/* stream.h D3str+ */
EXPORT_SYMBOL(unweldq);		/* stream.h D3str+ */
EXPORT_SYMBOL(weldq);		/* stream.h D3str+ */

EXPORT_SYMBOL(allocq);		/* stream.h LfS (internal) */
EXPORT_SYMBOL(bcanget);		/* stream.h LfS (internal) */
EXPORT_SYMBOL(canget);		/* stream.h LfS (internal) */
EXPORT_SYMBOL(freeq);		/* stream.h LfS (internal) */
EXPORT_SYMBOL(qattach);		/* stream.h LfS (internal) */
EXPORT_SYMBOL(qclose);		/* stream.h LfS (internal) */
EXPORT_SYMBOL(qdetach);		/* stream.h LfS (internal) */
EXPORT_SYMBOL(qopen);		/* stream.h LfS (internal) */
EXPORT_SYMBOL(setq);		/* stream.h LfS (internal) */
EXPORT_SYMBOL(setqsched);	/* stream.h LfS (internal) */

EXPORT_SYMBOL(appq);		/* stream.h LiS */
EXPORT_SYMBOL(esbbcall);	/* stream.h LiS */
EXPORT_SYMBOL(isdatablk);	/* stream.h LiS */
EXPORT_SYMBOL(isdatamsg);	/* stream.h LiS */
EXPORT_SYMBOL(kmem_zalloc);	/* kmem.h */
EXPORT_SYMBOL(kmem_zalloc_node);	/* kmem.h */
EXPORT_SYMBOL(msgsize);		/* stream.h LiS */
EXPORT_SYMBOL(qcountstrm);	/* stream.h LiS */
EXPORT_SYMBOL(xmsgsize);	/* stream.h LiS */

#ifdef _AIX_SOURCE
EXPORT_SYMBOL(mi_bufcall);	/* aixddi.h */
#endif

#ifdef _SUN_SOURCE
EXPORT_SYMBOL(qbufcall);	/* sunddi.h */
EXPORT_SYMBOL(qunbufcall);	/* sunddi.h */
EXPORT_SYMBOL(qtimeout);	/* sunddi.h */
EXPORT_SYMBOL(quntimeout);	/* sunddi.h */
#endif

EXPORT_SYMBOL_GPL(register_cmajor);	/* strconf.h */
//EXPORT_SYMBOL_GPL(register_strdev);	/* strconf.h */
EXPORT_SYMBOL_GPL(register_strdrv);	/* strconf.h */
EXPORT_SYMBOL_GPL(register_strmod);	/* strconf.h */
EXPORT_SYMBOL_GPL(unregister_cmajor);	/* strconf.h */
//EXPORT_SYMBOL_GPL(unregister_strdev);	/* strconf.h */
EXPORT_SYMBOL_GPL(unregister_strdrv);	/* strconf.h */
EXPORT_SYMBOL_GPL(unregister_strmod);	/* strconf.h */
//EXPORT_SYMBOL_GPL(autopush_add);	/* strconf.h */
//EXPORT_SYMBOL_GPL(autopush_del);	/* strconf.h */
//EXPORT_SYMBOL_GPL(autopush_vml);	/* strconf.h */
//EXPORT_SYMBOL_GPL(autopush_find);	/* strconf.h */

EXPORT_SYMBOL_GPL(sysctl_str_strmsgsz);	/* strsysctl.h */
EXPORT_SYMBOL_GPL(sysctl_str_strctlsz);	/* strsysctl.h */

EXPORT_SYMBOL_GPL(specfs_mnt);	/* strspecfs.h */
//EXPORT_SYMBOL_GPL(strm_open);	/* strspecfs.h */

#if	defined(CONFIG_STREAMS_NSDEV_MODULE) || \
	defined(CONFIG_STREAMS_SC_MODULE)
//EXPORT_SYMBOL(fmodsw_list);	/* strreg.h */
//EXPORT_SYMBOL(cdevsw_list);	/* strreg.h */
//EXPORT_SYMBOL(fmod_count);	/* strreg.h */
//EXPORT_SYMBOL(cdev_count);	/* strreg.h */
#endif

#if	defined(CONFIG_STREAMS_COMPAT_AIX_MODULE)
EXPORT_SYMBOL(fmod_str);	/* strreg.h */
#endif

#if	defined(CONFIG_STREAMS_COMPAT_UW7_MODULE)
EXPORT_SYMBOL(strthreads);	/* strsched.h */
#endif

//EXPORT_SYMBOL(cdrv_get);	/* strreg.h */
//EXPORT_SYMBOL(cdrv_put);	/* strreg.h */

//EXPORT_SYMBOL(ap_get);		/* strsched.h */
//EXPORT_SYMBOL(ap_grab);		/* strsched.h */
//EXPORT_SYMBOL(ap_put);		/* strsched.h */

//EXPORT_SYMBOL(alloclk);		/* strsched.h */
//EXPORT_SYMBOL(freelk);		/* strsched.h */

EXPORT_SYMBOL(qpop);		/* stream.h */
EXPORT_SYMBOL(qpush);		/* stream.h */

#if	defined(CONFIG_STREAMS_COMPAT_LIS_MODULE) || \
	defined(CONFIG_STREAMS_FIFO_MODULE) || \
	defined(CONFIG_STREAMS_PIPE_MODULE)
//EXPORT_SYMBOL(allocsd);		/* strsched.h */
//EXPORT_SYMBOL(autopush);	/* sth.h */
//EXPORT_SYMBOL(freesd);		/* strsched.h */
//EXPORT_SYMBOL(sd_get);		/* strsched.h */
//EXPORT_SYMBOL(sd_put);		/* strsched.h */
//EXPORT_SYMBOL(strclose);	/* sth.h */
//EXPORT_SYMBOL(strfasync);	/* sth.h */
//EXPORT_SYMBOL(strflush);	/* sth.h */
//EXPORT_SYMBOL(strioctl);	/* sth.h */
//EXPORT_SYMBOL(strllseek);	/* sth.h */
//EXPORT_SYMBOL(strmmap);		/* sth.h */
//EXPORT_SYMBOL(stropen);		/* sth.h */
//EXPORT_SYMBOL(strpoll);		/* sth.h */
//EXPORT_SYMBOL(strread);		/* sth.h */
//EXPORT_SYMBOL(strreadv);	/* sth.h */
//EXPORT_SYMBOL(strsendpage);	/* sth.h */
//EXPORT_SYMBOL(strwrite);	/* sth.h */
//EXPORT_SYMBOL(strwritev);	/* sth.h */
//EXPORT_SYMBOL(strgetpmsg);	/* sth.h */
//EXPORT_SYMBOL(strputpmsg);	/* sth.h */
#ifdef CONFIG_STREAMS_COMPAT_LIS_MODULE
EXPORT_SYMBOL(apush_get);	/* strconf.h */
EXPORT_SYMBOL(apush_set);	/* strconf.h */
EXPORT_SYMBOL(apush_vml);	/* strconf.h */
//EXPORT_SYMBOL(cdev_get);	/* strsubr.h, strreg.c */
//EXPORT_SYMBOL(cdev_put);	/* strsubr.h, strreg.c */
//EXPORT_SYMBOL(cdev_find);	/* strsubr.h, strreg.c */
EXPORT_SYMBOL(fmod_get);	/* strsubr.h, strreg.c */
EXPORT_SYMBOL(fmod_put);	/* strsubr.h, strreg.c */
EXPORT_SYMBOL(fmod_find);	/* strsubr.h, strreg.c */
EXPORT_SYMBOL(node_find);	/* strsubr.h, strreg.c */
EXPORT_SYMBOL(vcmn_err);	/* cmn_err.h */
#ifdef HAVE_KERNEL_FATTACH_SUPPORT
//EXPORT_SYMBOL(do_fattach);	/* strattach.h */
//EXPORT_SYMBOL(do_fdetach);	/* strattach.h */
#endif
#ifdef HAVE_KERNEL_PIPE_SUPPORT
//EXPORT_SYMBOL(do_spipe);	/* strspecfs.h */
#endif
#endif
#endif
