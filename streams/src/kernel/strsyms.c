/*****************************************************************************

 @(#) $RCSfile: strsyms.c,v $ $Name:  $($Revision: 0.9.2.42 $) $Date: 2005/12/22 10:28:44 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/12/22 10:28:44 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: strsyms.c,v $ $Name:  $($Revision: 0.9.2.42 $) $Date: 2005/12/22 10:28:44 $"

static char const ident[] =
    "$RCSfile: strsyms.c,v $ $Name:  $($Revision: 0.9.2.42 $) $Date: 2005/12/22 10:28:44 $";

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

#include "sys/strdebug.h"

#include <sys/kmem.h>
#include <sys/cmn_err.h>

#include <sys/strlog.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>

#undef STR
#include <sys/ddi.h>

#include "sys/config.h"
#include "strsched.h"
#include "strreg.h"
#include "src/modules/sth.h"	/* for str_minfo */
#include "strsysctl.h"
#include "src/drivers/fifo.h"
#include "strattach.h"

//EXPORT_SYMBOL_NOVERS(adjmsg);        /* stream.h */
//EXPORT_SYMBOL_NOVERS(allocb);        /* stream.h */
//EXPORT_SYMBOL_NOVERS(backq);         /* stream.h */
//EXPORT_SYMBOL_NOVERS(bcanputnext);   /* stream.h */
//EXPORT_SYMBOL_NOVERS(bcanput);       /* stream.h */
//EXPORT_SYMBOL_NOVERS(bcopy);         /* ddi.h */
//EXPORT_SYMBOL_NOVERS(bufcall);       /* stream.h */
//EXPORT_SYMBOL_NOVERS(bzero);         /* ddi.h */
//EXPORT_SYMBOL_NOVERS(canenable);     /* stream.h */
//EXPORT_SYMBOL_NOVERS(canputnext);    /* stream.h */
//EXPORT_SYMBOL_NOVERS(canput);        /* stream.h */
//EXPORT_SYMBOL_NOVERS(cmn_err);       /* cmn_err.h */
//EXPORT_SYMBOL_NOVERS(copyb);         /* stream.h */
//EXPORT_SYMBOL_NOVERS(copyin);        /* ddi.h */
//EXPORT_SYMBOL_NOVERS(copymsg);       /* stream.h */
//EXPORT_SYMBOL_NOVERS(copyout);       /* ddi.h */
//EXPORT_SYMBOL_NOVERS(datamsg);       /* stream.h */
//EXPORT_SYMBOL_NOVERS(delay);         /* ddi.h */
//EXPORT_SYMBOL_NOVERS(drv_getparm);   /* ddi.h */
//EXPORT_SYMBOL_NOVERS(drv_hztomsec);  /* ddi.h */
//EXPORT_SYMBOL_NOVERS(drv_hztousec);  /* ddi.h */
//EXPORT_SYMBOL_NOVERS(drv_msectohz);  /* ddi.h */
//EXPORT_SYMBOL_NOVERS(drv_priv);      /* ddi.h */
//EXPORT_SYMBOL_NOVERS(drv_usectohz);  /* ddi.h */
//EXPORT_SYMBOL_NOVERS(drv_usecwait);  /* ddi.h */
//EXPORT_SYMBOL_NOVERS(dupb);          /* stream.h */
//EXPORT_SYMBOL_NOVERS(dupmsg);        /* stream.h */
//EXPORT_SYMBOL_NOVERS(enableok);      /* stream.h */
//EXPORT_SYMBOL_NOVERS(enableq);       /* stream.h */
//EXPORT_SYMBOL_NOVERS(esballoc);      /* stream.h */
//EXPORT_SYMBOL_NOVERS(flushband);     /* stream.h */
//EXPORT_SYMBOL_NOVERS(flushq);        /* stream.h */
//EXPORT_SYMBOL_NOVERS(freeb);         /* stream.h */
//EXPORT_SYMBOL_NOVERS(freemsg);       /* stream.h */
//EXPORT_SYMBOL_NOVERS(freezestr);     /* stream.h */
//EXPORT_SYMBOL_NOVERS(getadmin);      /* stream.h, strreg.c */
//EXPORT_SYMBOL_NOVERS(getmajor);      /* ddi.h */
//EXPORT_SYMBOL_NOVERS(getmid);        /* stream.h, strreg.c */
//EXPORT_SYMBOL_NOVERS(getminor);      /* ddi.h */
//EXPORT_SYMBOL_NOVERS(getq);          /* stream.h */
//EXPORT_SYMBOL_NOVERS(insq);          /* stream.h */
//EXPORT_SYMBOL_NOVERS(kmem_alloc);    /* kmem.h */
//EXPORT_SYMBOL_NOVERS(kmem_free);     /* kmem.h */
//EXPORT_SYMBOL_NOVERS(linkb);         /* stream.h */
//EXPORT_SYMBOL_NOVERS(makedevice);    /* ddi.h */
//EXPORT_SYMBOL_NOVERS(max);           /* ddi.h */
//EXPORT_SYMBOL_NOVERS(min);           /* ddi.h */
//EXPORT_SYMBOL_NOVERS(msgdsize);      /* stream.h */
//EXPORT_SYMBOL_NOVERS(msgpullup);     /* stream.h */
//EXPORT_SYMBOL_NOVERS(noenable);      /* stream.h */
//EXPORT_SYMBOL_NOVERS(OTHERQ);        /* stream.h */
//EXPORT_SYMBOL_NOVERS(pcmsg);         /* stream.h */
//EXPORT_SYMBOL_NOVERS(pullupmsg);     /* stream.h */
//EXPORT_SYMBOL_NOVERS(putbq);         /* stream.h */
//EXPORT_SYMBOL_NOVERS(putctl1);       /* stream.h */
//EXPORT_SYMBOL_NOVERS(putctl);        /* stream.h */
//EXPORT_SYMBOL_NOVERS(putnextctl1);   /* stream.h */
//EXPORT_SYMBOL_NOVERS(putnextctl);    /* stream.h */
//EXPORT_SYMBOL_NOVERS(putnext);       /* stream.h */
//EXPORT_SYMBOL_NOVERS(putq);          /* stream.h */
//EXPORT_SYMBOL_NOVERS(put);           /* stream.h */
//EXPORT_SYMBOL_NOVERS(qenable);       /* stream.h */
//EXPORT_SYMBOL_NOVERS(qprocsoff);     /* stream.h */
//EXPORT_SYMBOL_NOVERS(qprocson);      /* stream.h */
//EXPORT_SYMBOL_NOVERS(qreply);        /* stream.h */
//EXPORT_SYMBOL_NOVERS(qsize);         /* stream.h */
//EXPORT_SYMBOL_NOVERS(RD);            /* stream.h */
//EXPORT_SYMBOL_NOVERS(rmvb);          /* stream.h */
//EXPORT_SYMBOL_NOVERS(rmvq);          /* stream.h */
//EXPORT_SYMBOL_NOVERS(SAMESTR);       /* stream.h */
//EXPORT_SYMBOL_NOVERS(strlog);        /* stream.h */
//EXPORT_SYMBOL_NOVERS(strqget);       /* stream.h */
//EXPORT_SYMBOL_NOVERS(strqset);       /* stream.h */
//EXPORT_SYMBOL_NOVERS(testb);         /* stream.h */
//EXPORT_SYMBOL_NOVERS(timeout);       /* ddi.h */
//EXPORT_SYMBOL_NOVERS(unbufcall);     /* stream.h */
//EXPORT_SYMBOL_NOVERS(unfreezestr);   /* stream.h */
//EXPORT_SYMBOL_NOVERS(unlinkb);       /* stream.h */
//EXPORT_SYMBOL_NOVERS(untimeout);     /* ddi.h */
//EXPORT_SYMBOL_NOVERS(WR);            /* stream.h */

//EXPORT_SYMBOL_NOVERS(putctl2);       /* stream.h D3str+ */
//EXPORT_SYMBOL_NOVERS(putnextctl2);   /* stream.h D3str+ */
//EXPORT_SYMBOL_NOVERS(unweldq);       /* stream.h D3str+ */
//EXPORT_SYMBOL_NOVERS(weldq);         /* stream.h D3str+ */

//EXPORT_SYMBOL_NOVERS(kmem_zalloc);   /* kmem.h */
//EXPORT_SYMBOL_NOVERS(kmem_alloc_node);       /* kmem.h */
//EXPORT_SYMBOL_NOVERS(kmem_zalloc_node);      /* kmem.h */

//EXPORT_SYMBOL_NOVERS(allocq);        /* stream.h LfS (internal) */
//EXPORT_SYMBOL_NOVERS(bcanget);       /* stream.h LfS (internal) */
//EXPORT_SYMBOL_NOVERS(canget);        /* stream.h LfS (internal) */
//EXPORT_SYMBOL_NOVERS(freeq); /* stream.h LfS (internal) */
//EXPORT_SYMBOL_NOVERS(qattach);       /* stream.h LfS (internal) */
//EXPORT_SYMBOL_NOVERS(qclose);        /* stream.h LfS (internal) */
//EXPORT_SYMBOL_NOVERS(qdetach);       /* stream.h LfS (internal) */
//EXPORT_SYMBOL_NOVERS(qopen); /* stream.h LfS (internal) */
//EXPORT_SYMBOL_NOVERS(setq);  /* stream.h LfS (internal) */
//EXPORT_SYMBOL_NOVERS(setqsched);     /* stream.h LfS (internal) */

//EXPORT_SYMBOL_NOVERS(appq);  /* stream.h LiS */
//EXPORT_SYMBOL_NOVERS(esbbcall);      /* stream.h LiS */
//EXPORT_SYMBOL_NOVERS(isdatablk);     /* stream.h LiS */
//EXPORT_SYMBOL_NOVERS(isdatamsg);     /* stream.h LiS */
//EXPORT_SYMBOL_NOVERS(linkmsg);       /* stream.h LfS */
//EXPORT_SYMBOL_NOVERS(msgsize);       /* stream.h LiS */
//EXPORT_SYMBOL_NOVERS(qcountstrm);/* stream.h LiS */
//EXPORT_SYMBOL_NOVERS(xmsgsize);      /* stream.h LiS */

#ifdef _AIX_SOURCE
//EXPORT_SYMBOL_NOVERS(mi_bufcall);    /* aixddi.h */
#endif

#ifdef _SUN_SOURCE
//EXPORT_SYMBOL_NOVERS(qbufcall);      /* sunddi.h */
//EXPORT_SYMBOL_NOVERS(qunbufcall);    /* sunddi.h */
//EXPORT_SYMBOL_NOVERS(qtimeout);      /* sunddi.h */
//EXPORT_SYMBOL_NOVERS(quntimeout);    /* sunddi.h */
#endif

//EXPORT_SYMBOL_NOVERS(register_cmajor);       /* strconf.h */
//EXPORT_SYMBOL_NOVERS(register_strdev);       /* strconf.h */
//EXPORT_SYMBOL_NOVERS(register_strdrv);       /* strconf.h */
//EXPORT_SYMBOL_NOVERS(register_strmod);       /* strconf.h */
//EXPORT_SYMBOL_NOVERS(unregister_cmajor);     /* strconf.h */
//EXPORT_SYMBOL_NOVERS(unregister_strdev);     /* strconf.h */
//EXPORT_SYMBOL_NOVERS(unregister_strdrv);     /* strconf.h */
//EXPORT_SYMBOL_NOVERS(unregister_strmod);     /* strconf.h */
//EXPORT_SYMBOL_NOVERS(autopush_add);  /* strconf.h */
//EXPORT_SYMBOL_NOVERS(autopush_del);  /* strconf.h */
//EXPORT_SYMBOL_NOVERS(autopush_vml);  /* strconf.h */
//EXPORT_SYMBOL_NOVERS(autopush_find); /* strconf.h */

//EXPORT_SYMBOL_NOVERS(sysctl_str_strmsgsz);   /* strsysctl.h */
//EXPORT_SYMBOL_NOVERS(sysctl_str_strctlsz);   /* strsysctl.h */

//EXPORT_SYMBOL_NOVERS(specfs_mnt);/* strspecfs.h */
//EXPORT_SYMBOL_NOVERS(spec_open);     /* strspecfs.h */
//EXPORT_SYMBOL_NOVERS(spec_reparent);     /* strspecfs.h */

#if	defined(CONFIG_STREAMS_NSDEV_MODULE) || \
	defined(CONFIG_STREAMS_SC_MODULE)
//EXPORT_SYMBOL_NOVERS(fmodsw_list);   /* strreg.h */
//EXPORT_SYMBOL_NOVERS(cdevsw_list);   /* strreg.h */
//EXPORT_SYMBOL_NOVERS(fmod_count);    /* strreg.h */
//EXPORT_SYMBOL_NOVERS(cdev_count);    /* strreg.h */
#endif

//EXPORT_SYMBOL_NOVERS(fmod_str);      /* strreg.h */

//EXPORT_SYMBOL_NOVERS(strthreads);/* strsched.h */

//EXPORT_SYMBOL_NOVERS(cdrv_get);      /* strreg.h */
//EXPORT_SYMBOL_NOVERS(cdrv_put);      /* strreg.h */

//EXPORT_SYMBOL_NOVERS(ap_alloc);      /* strsched.h */
//EXPORT_SYMBOL_NOVERS(ap_get);        /* strsched.h */
//EXPORT_SYMBOL_NOVERS(ap_put);        /* strsched.h */

//EXPORT_SYMBOL_NOVERS(alloclk);       /* strsched.h */
//EXPORT_SYMBOL_NOVERS(freelk);        /* strsched.h */

//EXPORT_SYMBOL_NOVERS(allocstr);      /* strsched.h */
//EXPORT_SYMBOL_NOVERS(autopush);      /* sth.h */
//EXPORT_SYMBOL_NOVERS(freestr);       /* strsched.h */
//EXPORT_SYMBOL_NOVERS(sd_get);        /* strsched.h */
//EXPORT_SYMBOL_NOVERS(sd_put);        /* strsched.h */
//EXPORT_SYMBOL_NOVERS(strclose);      /* sth.h */
//EXPORT_SYMBOL_NOVERS(strfasync);     /* sth.h */
//EXPORT_SYMBOL_NOVERS(strflush);      /* sth.h */
//EXPORT_SYMBOL_NOVERS(strioctl);      /* sth.h */
//EXPORT_SYMBOL_NOVERS(strllseek);     /* sth.h */
//EXPORT_SYMBOL_NOVERS(strmmap);       /* sth.h */
//EXPORT_SYMBOL_NOVERS(stropen);       /* sth.h */
//EXPORT_SYMBOL_NOVERS(strpoll);       /* sth.h */
//EXPORT_SYMBOL_NOVERS(strread);       /* sth.h */
//EXPORT_SYMBOL_NOVERS(strreadv);      /* sth.h */
//EXPORT_SYMBOL_NOVERS(strsendpage);/* sth.h */
//EXPORT_SYMBOL_NOVERS(strwrite);      /* sth.h */
//EXPORT_SYMBOL_NOVERS(strwritev);     /* sth.h */
//EXPORT_SYMBOL_NOVERS(strgetpmsg);/* sth.h */
//EXPORT_SYMBOL_NOVERS(strputpmsg);/* sth.h */
//EXPORT_SYMBOL_NOVERS(apush_get);     /* strconf.h */
//EXPORT_SYMBOL_NOVERS(apush_set);     /* strconf.h */
//EXPORT_SYMBOL_NOVERS(apush_vml);     /* strconf.h */
//EXPORT_SYMBOL_NOVERS(sdev_get);      /* strsubr.h, strreg.c */
//EXPORT_SYMBOL_NOVERS(sdev_put);      /* strsubr.h, strreg.c */
//EXPORT_SYMBOL_NOVERS(cdev_find);     /* strsubr.h, strreg.c */
//EXPORT_SYMBOL_NOVERS(fmod_get);      /* strsubr.h, strreg.c */
//EXPORT_SYMBOL_NOVERS(fmod_put);      /* strsubr.h, strreg.c */
//EXPORT_SYMBOL_NOVERS(fmod_find);     /* strsubr.h, strreg.c */
//EXPORT_SYMBOL_NOVERS(cmin_find);     /* strsubr.h, strreg.c */
//EXPORT_SYMBOL_NOVERS(vcmn_err);      /* cmn_err.h */
#ifdef HAVE_KERNEL_FATTACH_SUPPORT
//EXPORT_SYMBOL_NOVERS(do_fattach);/* strattach.h */
//EXPORT_SYMBOL_NOVERS(do_fdetach);/* strattach.h */
#endif
#ifdef HAVE_KERNEL_PIPE_SUPPORT
//EXPORT_SYMBOL_NOVERS(do_spipe);      /* strspecfs.h */
#endif
