/*****************************************************************************

 @(#) $RCSfile: strsyms.c,v $ $Name:  $($Revision: 0.9.2.29 $) $Date: 2005/03/08 19:31:40 $

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

 Last Modified $Date: 2005/03/08 19:31:40 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strsyms.c,v $ $Name:  $($Revision: 0.9.2.29 $) $Date: 2005/03/08 19:31:40 $"

static char const ident[] =
    "$RCSfile: strsyms.c,v $ $Name:  $($Revision: 0.9.2.29 $) $Date: 2005/03/08 19:31:40 $";

//#define __NO_VERSION__
//#define EXPORT_SYMTAB

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

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

#include "sys/config.h"
#include "strsched.h"
#include "strreg.h"
#include "src/modules/sth.h"		/* for str_minfo */
#include "strsysctl.h"
#include "src/drivers/fifo.h"
#include "strattach.h"

//EXPORT_SYMBOL(adjmsg);                /* stream.h */
//EXPORT_SYMBOL(allocb);                /* stream.h */
EXPORT_SYMBOL(backq);		/* stream.h */
//EXPORT_SYMBOL(bcanputnext);   /* stream.h */
//EXPORT_SYMBOL(bcanput);               /* stream.h */
//EXPORT_SYMBOL(bcopy);         /* ddi.h */
//EXPORT_SYMBOL(bufcall);               /* stream.h */
//EXPORT_SYMBOL(bzero);         /* ddi.h */
EXPORT_SYMBOL(canenable);	/* stream.h */
//EXPORT_SYMBOL(canputnext);    /* stream.h */
//EXPORT_SYMBOL(canput);                /* stream.h */
//EXPORT_SYMBOL(cmn_err);               /* cmn_err.h */
//EXPORT_SYMBOL(copyb);         /* stream.h */
EXPORT_SYMBOL(copyin);		/* ddi.h */
//EXPORT_SYMBOL(copymsg);               /* stream.h */
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
//EXPORT_SYMBOL(dupb);          /* stream.h */
//EXPORT_SYMBOL(dupmsg);                /* stream.h */
EXPORT_SYMBOL(enableok);	/* stream.h */
//EXPORT_SYMBOL(enableq);               /* stream.h */
//EXPORT_SYMBOL(esballoc);      /* stream.h */
//EXPORT_SYMBOL(flushband);     /* stream.h */
//EXPORT_SYMBOL(flushq);                /* stream.h */
//EXPORT_SYMBOL(freeb);         /* stream.h */
EXPORT_SYMBOL(freemsg);		/* stream.h */
//EXPORT_SYMBOL(freezestr);     /* stream.h */
//EXPORT_SYMBOL(getadmin);      /* stream.h, strreg.c */
EXPORT_SYMBOL(getmajor);	/* ddi.h */
//EXPORT_SYMBOL(getmid);                /* stream.h, strreg.c */
EXPORT_SYMBOL(getminor);	/* ddi.h */
//EXPORT_SYMBOL(getq);          /* stream.h */
//EXPORT_SYMBOL(insq);          /* stream.h */
EXPORT_SYMBOL(kmem_alloc);	/* kmem.h */
//EXPORT_SYMBOL(kmem_free);     /* kmem.h */
EXPORT_SYMBOL(linkb);		/* stream.h */
EXPORT_SYMBOL(makedevice);	/* ddi.h */
//EXPORT_SYMBOL(max);           /* ddi.h */
//EXPORT_SYMBOL(min);           /* ddi.h */
EXPORT_SYMBOL(msgdsize);	/* stream.h */
//EXPORT_SYMBOL(msgpullup);     /* stream.h */
EXPORT_SYMBOL(noenable);	/* stream.h */
EXPORT_SYMBOL(OTHERQ);		/* stream.h */
EXPORT_SYMBOL(pcmsg);		/* stream.h */
//EXPORT_SYMBOL(pullupmsg);     /* stream.h */
//EXPORT_SYMBOL(putbq);         /* stream.h */
EXPORT_SYMBOL(putctl1);		/* stream.h */
EXPORT_SYMBOL(putctl);		/* stream.h */
EXPORT_SYMBOL(putnextctl1);	/* stream.h */
EXPORT_SYMBOL(putnextctl);	/* stream.h */
//EXPORT_SYMBOL(putnext);               /* stream.h */
//EXPORT_SYMBOL(putq);          /* stream.h */
//EXPORT_SYMBOL(put);           /* stream.h */
//EXPORT_SYMBOL(qenable);               /* stream.h */
//EXPORT_SYMBOL(qprocsoff);     /* stream.h */
//EXPORT_SYMBOL(qprocson);      /* stream.h */
EXPORT_SYMBOL(qreply);		/* stream.h */
EXPORT_SYMBOL(qsize);		/* stream.h */
EXPORT_SYMBOL(RD);		/* stream.h */
EXPORT_SYMBOL(rmvb);		/* stream.h */
//EXPORT_SYMBOL(rmvq);          /* stream.h */
EXPORT_SYMBOL(SAMESTR);		/* stream.h */
//EXPORT_SYMBOL(strlog);                /* stream.h */
//EXPORT_SYMBOL(strqget);               /* stream.h */
//EXPORT_SYMBOL(strqset);               /* stream.h */
EXPORT_SYMBOL(testb);		/* stream.h */
//EXPORT_SYMBOL(timeout);               /* ddi.h */
//EXPORT_SYMBOL(unbufcall);     /* stream.h */
//EXPORT_SYMBOL(unfreezestr);   /* stream.h */
EXPORT_SYMBOL(unlinkb);		/* stream.h */
//EXPORT_SYMBOL(untimeout);     /* ddi.h */
EXPORT_SYMBOL(WR);		/* stream.h */

EXPORT_SYMBOL(putctl2);		/* stream.h D3str+ */
EXPORT_SYMBOL(putnextctl2);	/* stream.h D3str+ */
//EXPORT_SYMBOL(unweldq);		/* stream.h D3str+ */
//EXPORT_SYMBOL(weldq);		/* stream.h D3str+ */

EXPORT_SYMBOL(kmem_zalloc);	/* kmem.h */
EXPORT_SYMBOL(kmem_alloc_node);	/* kmem.h */
EXPORT_SYMBOL(kmem_zalloc_node);	/* kmem.h */

//EXPORT_SYMBOL_GPL(allocq);		/* stream.h LfS (internal) */
//EXPORT_SYMBOL_GPL(bcanget);		/* stream.h LfS (internal) */
//EXPORT_SYMBOL_GPL(canget);		/* stream.h LfS (internal) */
//EXPORT_SYMBOL_GPL(freeq);		/* stream.h LfS (internal) */
//EXPORT_SYMBOL_GPL(qattach);		/* stream.h LfS (internal) */
//EXPORT_SYMBOL_GPL(qclose);		/* stream.h LfS (internal) */
//EXPORT_SYMBOL_GPL(qdetach);		/* stream.h LfS (internal) */
//EXPORT_SYMBOL_GPL(qopen);		/* stream.h LfS (internal) */
//EXPORT_SYMBOL_GPL(setq);		/* stream.h LfS (internal) */
//EXPORT_SYMBOL_GPL(setqsched);	/* stream.h LfS (internal) */

//EXPORT_SYMBOL_GPL(appq);		/* stream.h LiS */
EXPORT_SYMBOL_GPL(esbbcall);	/* stream.h LiS */
EXPORT_SYMBOL_GPL(isdatablk);	/* stream.h LiS */
EXPORT_SYMBOL_GPL(isdatamsg);	/* stream.h LiS */
EXPORT_SYMBOL_GPL(linkmsg);		/* stream.h LfS */
EXPORT_SYMBOL_GPL(msgsize);		/* stream.h LiS */
//EXPORT_SYMBOL_GPL(qcountstrm);	/* stream.h LiS */
EXPORT_SYMBOL_GPL(xmsgsize);	/* stream.h LiS */

#ifdef _AIX_SOURCE
//EXPORT_SYMBOL(mi_bufcall);	/* aixddi.h */
#endif

#ifdef _SUN_SOURCE
//EXPORT_SYMBOL(qbufcall);	/* sunddi.h */
//EXPORT_SYMBOL(qunbufcall);	/* sunddi.h */
//EXPORT_SYMBOL(qtimeout);	/* sunddi.h */
//EXPORT_SYMBOL(quntimeout);	/* sunddi.h */
#endif

//EXPORT_SYMBOL_GPL(register_cmajor);	/* strconf.h */
//EXPORT_SYMBOL_GPL(register_strdev);	/* strconf.h */
//EXPORT_SYMBOL_GPL(register_strdrv);	/* strconf.h */
//EXPORT_SYMBOL_GPL(register_strmod);	/* strconf.h */
//EXPORT_SYMBOL_GPL(unregister_cmajor);	/* strconf.h */
//EXPORT_SYMBOL_GPL(unregister_strdev);	/* strconf.h */
//EXPORT_SYMBOL_GPL(unregister_strdrv);	/* strconf.h */
//EXPORT_SYMBOL_GPL(unregister_strmod);	/* strconf.h */
//EXPORT_SYMBOL_GPL(autopush_add);	/* strconf.h */
//EXPORT_SYMBOL_GPL(autopush_del);	/* strconf.h */
//EXPORT_SYMBOL_GPL(autopush_vml);	/* strconf.h */
//EXPORT_SYMBOL_GPL(autopush_find);	/* strconf.h */

//EXPORT_SYMBOL_GPL(sysctl_str_strmsgsz);	/* strsysctl.h */
//EXPORT_SYMBOL_GPL(sysctl_str_strctlsz);	/* strsysctl.h */

//EXPORT_SYMBOL_GPL(specfs_mnt);	/* strspecfs.h */
//EXPORT_SYMBOL_GPL(spec_open);	/* strspecfs.h */

#if	defined(CONFIG_STREAMS_NSDEV_MODULE) || \
	defined(CONFIG_STREAMS_SC_MODULE)
//EXPORT_SYMBOL_GPL(fmodsw_list);	/* strreg.h */
//EXPORT_SYMBOL_GPL(cdevsw_list);	/* strreg.h */
//EXPORT_SYMBOL_GPL(fmod_count);	/* strreg.h */
//EXPORT_SYMBOL_GPL(cdev_count);	/* strreg.h */
#endif

#if	defined(CONFIG_STREAMS_COMPAT_AIX_MODULE)
//EXPORT_SYMBOL_GPL(fmod_str);	/* strreg.h */
#endif

#if	defined(CONFIG_STREAMS_COMPAT_UW7_MODULE)
//EXPORT_SYMBOL_GPL(strthreads);	/* strsched.h */
#endif

//EXPORT_SYMBOL_GPL(cdrv_get);	/* strreg.h */
//EXPORT_SYMBOL_GPL(cdrv_put);	/* strreg.h */

//EXPORT_SYMBOL_GPL(ap_alloc);		/* strsched.h */
//EXPORT_SYMBOL_GPL(ap_get);		/* strsched.h */
//EXPORT_SYMBOL_GPL(ap_put);		/* strsched.h */

//EXPORT_SYMBOL_GPL(alloclk);		/* strsched.h */
//EXPORT_SYMBOL_GPL(freelk);		/* strsched.h */

//EXPORT_SYMBOL_GPL(qpop);		/* stream.h */
//EXPORT_SYMBOL_GPL(qpush);		/* stream.h */

#if	defined(CONFIG_STREAMS_COMPAT_LIS_MODULE) || \
	defined(CONFIG_STREAMS_FIFO_MODULE) || \
	defined(CONFIG_STREAMS_PIPE_MODULE)
//EXPORT_SYMBOL_GPL(allocsd);		/* strsched.h */
//EXPORT_SYMBOL_GPL(autopush);	/* sth.h */
//EXPORT_SYMBOL_GPL(freesd);		/* strsched.h */
//EXPORT_SYMBOL_GPL(sd_get);		/* strsched.h */
//EXPORT_SYMBOL_GPL(sd_put);		/* strsched.h */
//EXPORT_SYMBOL_GPL(strclose);	/* sth.h */
//EXPORT_SYMBOL_GPL(strfasync);	/* sth.h */
//EXPORT_SYMBOL_GPL(strflush);	/* sth.h */
//EXPORT_SYMBOL_GPL(strioctl);	/* sth.h */
//EXPORT_SYMBOL_GPL(strllseek);	/* sth.h */
//EXPORT_SYMBOL_GPL(strmmap);		/* sth.h */
//EXPORT_SYMBOL_GPL(stropen);		/* sth.h */
//EXPORT_SYMBOL_GPL(strpoll);		/* sth.h */
//EXPORT_SYMBOL_GPL(strread);		/* sth.h */
//EXPORT_SYMBOL_GPL(strreadv);	/* sth.h */
//EXPORT_SYMBOL_GPL(strsendpage);	/* sth.h */
//EXPORT_SYMBOL_GPL(strwrite);	/* sth.h */
//EXPORT_SYMBOL_GPL(strwritev);	/* sth.h */
//EXPORT_SYMBOL_GPL(strgetpmsg);	/* sth.h */
//EXPORT_SYMBOL_GPL(strputpmsg);	/* sth.h */
#ifdef CONFIG_STREAMS_COMPAT_LIS_MODULE
//EXPORT_SYMBOL_GPL(apush_get);	/* strconf.h */
//EXPORT_SYMBOL_GPL(apush_set);	/* strconf.h */
//EXPORT_SYMBOL_GPL(apush_vml);	/* strconf.h */
//EXPORT_SYMBOL_GPL(cdev_get);	/* strsubr.h, strreg.c */
//EXPORT_SYMBOL_GPL(cdev_put);	/* strsubr.h, strreg.c */
//EXPORT_SYMBOL_GPL(cdev_find);	/* strsubr.h, strreg.c */
//EXPORT_SYMBOL_GPL(fmod_get);	/* strsubr.h, strreg.c */
//EXPORT_SYMBOL_GPL(fmod_put);	/* strsubr.h, strreg.c */
//EXPORT_SYMBOL_GPL(fmod_find);	/* strsubr.h, strreg.c */
//EXPORT_SYMBOL_GPL(cmin_find);	/* strsubr.h, strreg.c */
//EXPORT_SYMBOL_GPL(vcmn_err);	/* cmn_err.h */
#ifdef HAVE_KERNEL_FATTACH_SUPPORT
//EXPORT_SYMBOL_GPL(do_fattach);	/* strattach.h */
//EXPORT_SYMBOL_GPL(do_fdetach);	/* strattach.h */
#endif
#ifdef HAVE_KERNEL_PIPE_SUPPORT
//EXPORT_SYMBOL_GPL(do_spipe);	/* strspecfs.h */
#endif
#endif
#endif
