/*****************************************************************************

 @(#) $RCSfile: strsyms.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/03/09 22:56:47 $

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

 Last Modified $Date: 2004/03/09 22:56:47 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strsyms.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/03/09 22:56:47 $"

static char const ident[] = "$RCSfile: strsyms.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/03/09 22:56:47 $";

#define __NO_VERSION__
#define EXPORT_SYMTAB

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

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
#include "strhead.h"			/* for str_minfo */
#include "strsysctl.h"
#include "strfifo.h"
#include "strattach.h"

EXPORT_SYMBOL_NOVERS(adjmsg);		/* stream.h */
EXPORT_SYMBOL_NOVERS(allocb);		/* stream.h */
EXPORT_SYMBOL_NOVERS(backq);		/* stream.h */
EXPORT_SYMBOL_NOVERS(bcanputnext);	/* stream.h */
EXPORT_SYMBOL_NOVERS(bcanput);		/* stream.h */
EXPORT_SYMBOL_NOVERS(bcopy);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(bufcall);		/* stream.h */
EXPORT_SYMBOL_NOVERS(bzero);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(canenable);	/* stream.h */
EXPORT_SYMBOL_NOVERS(canputnext);	/* stream.h */
EXPORT_SYMBOL_NOVERS(canput);		/* stream.h */
EXPORT_SYMBOL_NOVERS(cmn_err);		/* cmn_err.h */
EXPORT_SYMBOL_NOVERS(copyb);		/* stream.h */
EXPORT_SYMBOL_NOVERS(copyin);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(copymsg);		/* stream.h */
EXPORT_SYMBOL_NOVERS(copyout);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(datamsg);		/* stream.h */
EXPORT_SYMBOL_NOVERS(delay);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(drv_getparm);	/* ddi.h */
EXPORT_SYMBOL_NOVERS(drv_hztomsec);	/* ddi.h */
EXPORT_SYMBOL_NOVERS(drv_hztousec);	/* ddi.h */
EXPORT_SYMBOL_NOVERS(drv_msectohz);	/* ddi.h */
EXPORT_SYMBOL_NOVERS(drv_priv);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(drv_usectohz);	/* ddi.h */
EXPORT_SYMBOL_NOVERS(drv_usecwait);	/* ddi.h */
EXPORT_SYMBOL_NOVERS(dupb);		/* stream.h */
EXPORT_SYMBOL_NOVERS(dupmsg);		/* stream.h */
EXPORT_SYMBOL_NOVERS(enableok);		/* stream.h */
EXPORT_SYMBOL_NOVERS(enableq);		/* stream.h */
EXPORT_SYMBOL_NOVERS(esballoc);		/* stream.h */
EXPORT_SYMBOL_NOVERS(flushband);	/* stream.h */
EXPORT_SYMBOL_NOVERS(flushq);		/* stream.h */
EXPORT_SYMBOL_NOVERS(freeb);		/* stream.h */
EXPORT_SYMBOL_NOVERS(freemsg);		/* stream.h */
EXPORT_SYMBOL_NOVERS(freezestr);	/* stream.h */
EXPORT_SYMBOL_NOVERS(getadmin);		/* stream.h */
EXPORT_SYMBOL_NOVERS(getmajor);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(getmid);		/* stream.h */
EXPORT_SYMBOL_NOVERS(getminor);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(getq);		/* stream.h */
EXPORT_SYMBOL_NOVERS(insq);		/* stream.h */
EXPORT_SYMBOL_NOVERS(kmem_alloc);	/* kmem.h */
EXPORT_SYMBOL_NOVERS(kmem_free);	/* kmem.h */
EXPORT_SYMBOL_NOVERS(linkb);		/* stream.h */
EXPORT_SYMBOL_NOVERS(makedevice);	/* ddi.h */
EXPORT_SYMBOL_NOVERS(max);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(min);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(msgdsize);		/* stream.h */
EXPORT_SYMBOL_NOVERS(msgpullup);	/* stream.h */
EXPORT_SYMBOL_NOVERS(noenable);		/* stream.h */
EXPORT_SYMBOL_NOVERS(OTHERQ);		/* stream.h */
EXPORT_SYMBOL_NOVERS(pcmsg);		/* stream.h */
EXPORT_SYMBOL_NOVERS(pullupmsg);	/* stream.h */
EXPORT_SYMBOL_NOVERS(putbq);		/* stream.h */
EXPORT_SYMBOL_NOVERS(putctl1);		/* stream.h */
EXPORT_SYMBOL_NOVERS(putctl);		/* stream.h */
EXPORT_SYMBOL_NOVERS(putnextctl1);	/* stream.h */
EXPORT_SYMBOL_NOVERS(putnextctl);	/* stream.h */
EXPORT_SYMBOL_NOVERS(putnext);		/* stream.h */
EXPORT_SYMBOL_NOVERS(putq);		/* stream.h */
EXPORT_SYMBOL_NOVERS(put);		/* stream.h */
EXPORT_SYMBOL_NOVERS(qenable);		/* stream.h */
EXPORT_SYMBOL_NOVERS(qprocsoff);	/* stream.h */
EXPORT_SYMBOL_NOVERS(qprocson);		/* stream.h */
EXPORT_SYMBOL_NOVERS(qreply);		/* stream.h */
EXPORT_SYMBOL_NOVERS(qsize);		/* stream.h */
EXPORT_SYMBOL_NOVERS(RD);		/* stream.h */
EXPORT_SYMBOL_NOVERS(rmvb);		/* stream.h */
EXPORT_SYMBOL_NOVERS(rmvq);		/* stream.h */
EXPORT_SYMBOL_NOVERS(SAMESTR);		/* stream.h */
EXPORT_SYMBOL_NOVERS(strlog);		/* stream.h */
EXPORT_SYMBOL_NOVERS(strqget);		/* stream.h */
EXPORT_SYMBOL_NOVERS(strqset);		/* stream.h */
EXPORT_SYMBOL_NOVERS(testb);		/* stream.h */
EXPORT_SYMBOL_NOVERS(timeout);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(unbufcall);	/* stream.h */
EXPORT_SYMBOL_NOVERS(unfreezestr);	/* stream.h */
EXPORT_SYMBOL_NOVERS(unlinkb);		/* stream.h */
EXPORT_SYMBOL_NOVERS(untimeout);	/* ddi.h */
EXPORT_SYMBOL_NOVERS(WR);		/* stream.h */

EXPORT_SYMBOL_NOVERS(linkmsg);		/* stream.h LfS */
EXPORT_SYMBOL_NOVERS(putctl2);		/* stream.h D3str+ */
EXPORT_SYMBOL_NOVERS(putnextctl2);	/* stream.h D3str+ */
EXPORT_SYMBOL_NOVERS(unweldq);		/* stream.h D3str+ */
EXPORT_SYMBOL_NOVERS(weldq);		/* stream.h D3str+ */

EXPORT_SYMBOL_NOVERS(allocq);		/* stream.h LfS (internal) */
EXPORT_SYMBOL_NOVERS(bcanget);		/* stream.h LfS (internal) */
EXPORT_SYMBOL_NOVERS(canget);		/* stream.h LfS (internal) */
EXPORT_SYMBOL_NOVERS(freeq);		/* stream.h LfS (internal) */
EXPORT_SYMBOL_NOVERS(qattach);		/* stream.h LfS (internal) */
EXPORT_SYMBOL_NOVERS(qclose);		/* stream.h LfS (internal) */
EXPORT_SYMBOL_NOVERS(qdetach);		/* stream.h LfS (internal) */
EXPORT_SYMBOL_NOVERS(qopen);		/* stream.h LfS (internal) */
EXPORT_SYMBOL_NOVERS(setq);		/* stream.h LfS (internal) */
EXPORT_SYMBOL_NOVERS(setqsched);	/* stream.h LfS (internal) */

EXPORT_SYMBOL_NOVERS(appq);		/* stream.h LiS */
EXPORT_SYMBOL_NOVERS(esbbcall);		/* stream.h LiS */
EXPORT_SYMBOL_NOVERS(isdatablk);	/* stream.h LiS */
EXPORT_SYMBOL_NOVERS(isdatamsg);	/* stream.h LiS */
EXPORT_SYMBOL_NOVERS(kmem_zalloc);	/* kmem.h */
EXPORT_SYMBOL_NOVERS(kmem_zalloc_node);	/* kmem.h */
EXPORT_SYMBOL_NOVERS(msgsize);		/* stream.h LiS */
EXPORT_SYMBOL_NOVERS(qcountstrm);	/* stream.h LiS */
EXPORT_SYMBOL_NOVERS(xmsgsize);		/* stream.h LiS */

#ifdef _AIX_STREAMS_SOURCE
EXPORT_SYMBOL_NOVERS(mi_bufcall);	/* aixddi.h */
#endif

#ifdef _SUN_SOURCE
EXPORT_SYMBOL_NOVERS(qbufcall);		/* sunddi.h */
EXPORT_SYMBOL_NOVERS(qunbufcall);	/* sunddi.h */
EXPORT_SYMBOL_NOVERS(qtimeout);		/* sunddi.h */
EXPORT_SYMBOL_NOVERS(quntimeout);	/* sunddi.h */
#endif

EXPORT_SYMBOL_GPL(register_inode);		/* strconf.h */
EXPORT_SYMBOL_GPL(register_inode_major);	/* strconf.h */
//EXPORT_SYMBOL_GPL(register_inode_minor);	/* strconf.h */
EXPORT_SYMBOL_GPL(register_strdev);		/* strconf.h */
EXPORT_SYMBOL_GPL(register_strdev_major);	/* strconf.h */
//EXPORT_SYMBOL_GPL(register_strdev_minor);	/* strconf.h */
EXPORT_SYMBOL_GPL(register_strmod);		/* strconf.h */
EXPORT_SYMBOL_GPL(unregister_inode);		/* strconf.h */
EXPORT_SYMBOL_GPL(unregister_inode_major);	/* strconf.h */
//EXPORT_SYMBOL_GPL(unregister_inode_minor);	/* strconf.h */
EXPORT_SYMBOL_GPL(unregister_strdev);		/* strconf.h */
EXPORT_SYMBOL_GPL(unregister_strdev_major);	/* strconf.h */
//EXPORT_SYMBOL_GPL(unregister_strdev_minor);	/* strconf.h */
EXPORT_SYMBOL_GPL(unregister_strmod);		/* strconf.h */
EXPORT_SYMBOL_GPL(autopush_add);		/* strconf.h */
EXPORT_SYMBOL_GPL(autopush_del);		/* strconf.h */
EXPORT_SYMBOL_GPL(autopush_vml);		/* strconf.h */
EXPORT_SYMBOL_GPL(autopush_find);		/* strconf.h */

EXPORT_SYMBOL_GPL(sysctl_str_strmsgsz);	/* strsysctl.h */
EXPORT_SYMBOL_GPL(specfs_mnt);		/* strspecfs.h */


#if	defined(CONFIG_STREAMS_CLONE_MODULE) || \
	defined(CONFIG_STREAMS_NSDEV_MODULE) || \
	defined(CONFIG_STREAMS_SC_MODULE) || \
	defined(CONFIG_STREAMS_COMPAT_AIX_MODULE)
EXPORT_SYMBOL_NOVERS(fmodsw);		/* strreg.h */
EXPORT_SYMBOL_NOVERS(cdevsw);		/* strreg.h */
EXPORT_SYMBOL_NOVERS(strm_open);	/* strreg.h */
EXPORT_SYMBOL_NOVERS(sdev_open);	/* strreg.h */
EXPORT_SYMBOL_NOVERS(fmodsw_list);	/* strreg.h */
EXPORT_SYMBOL_NOVERS(cdevsw_list);	/* strreg.h */
EXPORT_SYMBOL_NOVERS(fmod_count);	/* strreg.h */
EXPORT_SYMBOL_NOVERS(cdev_count);	/* strreg.h */
#endif

#if	defined(CONFIG_STREAMS_COMPAT_UW7_MODULE)
EXPORT_SYMBOL_NOVERS(strthreads);	/* strsched.h */
#endif

#if	defined(CONFIG_STREAMS_COMPAT_LIS_MODULE) || \
	defined(CONFIG_STREAMS_FIFO_MODULE) || \
	defined(CONFIG_STREAMS_PIPE_MODULE)
EXPORT_SYMBOL_NOVERS(allocsd);		/* strsched.h */
EXPORT_SYMBOL_NOVERS(autopush);		/* strhead.h */
EXPORT_SYMBOL_NOVERS(freesd);		/* strsched.h */
EXPORT_SYMBOL_NOVERS(sd_get);		/* strsched.h */
EXPORT_SYMBOL_NOVERS(sd_put);		/* strsched.h */
EXPORT_SYMBOL_NOVERS(strclose);		/* strhead.h */
EXPORT_SYMBOL_NOVERS(strfasync);	/* strhead.h */
EXPORT_SYMBOL_NOVERS(strflush);		/* strhead.h */
EXPORT_SYMBOL_NOVERS(strgetpmsg);	/* strhead.h */
EXPORT_SYMBOL_NOVERS(strioctl);		/* strhead.h */
EXPORT_SYMBOL_NOVERS(strllseek);	/* strhead.h */
EXPORT_SYMBOL_NOVERS(strmmap);		/* strhead.h */
EXPORT_SYMBOL_NOVERS(stropen);		/* strhead.h */
EXPORT_SYMBOL_NOVERS(strpoll);		/* strhead.h */
EXPORT_SYMBOL_NOVERS(strputpmsg);	/* strhead.h */
EXPORT_SYMBOL_NOVERS(strread);		/* strhead.h */
EXPORT_SYMBOL_NOVERS(strreadv);		/* strhead.h */
EXPORT_SYMBOL_NOVERS(strsendpage);	/* strhead.h */
EXPORT_SYMBOL_NOVERS(strwrite);		/* strhead.h */
EXPORT_SYMBOL_NOVERS(strwritev);	/* strhead.h */
#ifdef CONFIG_STREAMS_COMPAT_LIS_MODULE
EXPORT_SYMBOL_NOVERS(apush_get);	/* strconf.h */
EXPORT_SYMBOL_NOVERS(apush_set);	/* strconf.h */
EXPORT_SYMBOL_NOVERS(apush_vml);	/* strconf.h */
EXPORT_SYMBOL_NOVERS(sdev_get);		/* strsubr.h */
EXPORT_SYMBOL_NOVERS(sdev_put);		/* strsubr.h */
EXPORT_SYMBOL_NOVERS(smod_get);		/* strsubr.h */
EXPORT_SYMBOL_NOVERS(smod_put);		/* strsubr.h */
EXPORT_SYMBOL_NOVERS(vcmn_err);		/* cmn_err.h */
#ifdef HAVE_KERNEL_FATTACH_SUPPORT
EXPORT_SYMBOL_NOVERS(do_fattach);	/* strattach.h */
EXPORT_SYMBOL_NOVERS(do_fdetach);	/* strattach.h */
#endif
#endif
#endif

