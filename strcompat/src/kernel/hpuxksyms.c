/*****************************************************************************

 @(#) hpuxksyms.c,v (1.1.2.3) 2003/10/21 21:50:08

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified 2003/10/21 21:50:08 by brian

 *****************************************************************************/

#ident "@(#) hpuxksyms.c,v (1.1.2.3) 2003/10/21 21:50:08"

static char const ident[] = "hpuxksyms.c,v (1.1.2.3) 2003/10/21 21:50:08";

#define __NO_VERSION__

#include <linux/config.h>
#include <linux/module.h>
#include <linux/stddef.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/poll.h>

#define _HPUX_SOURCE
#include <linux/stropts.h>
#include <linux/stream.h>
#include <linux/strconf.h>
#include <linux/strsubr.h>
#include <linux/ddi.h>

#include "strdebug.h"

/* these are HP-UX specific, use CONFIG_STREAMS_COMPAT_HPUX to get these */
EXPORT_SYMBOL_NOVERS(get_sleep_lock);	/* hpuxddi.h */
EXPORT_SYMBOL_NOVERS(streams_put);	/* hpuxddi.h */

#if 0
/* these are not so basic SVR 4 functions, use CONFIG_STREAMS_COMPAT_SVR4 to get these.  */
EXPORT_SYMBOL_NOVERS(itimeout);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(LOCK_ALLOC);	/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(LOCK);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(LOCK_DEALLOC);	/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(major);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(makedev);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(minor);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(sleep);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(spln);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(splstr);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(SV_ALLOC);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(SV_BROADCAST);	/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(SV_DEALLOC);	/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(SV_WAIT);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(SV_WAIT_SIG);	/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(TRYLOCK);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(UNLOCK);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(vtop);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(wakeup);		/* svr4ddi.h */
#endif

#if 0
/* there are common ddi functions, use CONFIG_STREAMS and you just get these */
EXPORT_SYMBOL_NOVERS(bcopy);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(bzero);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(cmn_err);		/* cmn_err.h */
EXPORT_SYMBOL_NOVERS(delay);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(drv_getparm);	/* ddi.h */
EXPORT_SYMBOL_NOVERS(drv_priv);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(getmajor);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(getminor);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(kmem_alloc);	/* kmem.h */
EXPORT_SYMBOL_NOVERS(kmem_free);	/* kmem.h */
EXPORT_SYMBOL_NOVERS(makedevice);	/* ddi.h */
EXPORT_SYMBOL_NOVERS(max);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(min);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(timeout);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(untimeout);	/* ddi.h */
#endif

#if 0
/* these are very common enhancements, use CONFIG_STREAMS and you just get these */
EXPORT_SYMBOL_NOVERS(putctl2);		/* stream.h D3str+ */
EXPORT_SYMBOL_NOVERS(putnextctl2);	/* stream.h D3str+ */
EXPORT_SYMBOL_NOVERS(unweldq);		/* stream.h D3str+ */
EXPORT_SYMBOL_NOVERS(weldq);		/* stream.h D3str+ */
#endif

#if 0
/* these are basic SVR 4 STREAMS functions, use CONFIG_STREAMS and you just get these */
EXPORT_SYMBOL_NOVERS(adjmsg);		/* stream.h */
EXPORT_SYMBOL_NOVERS(allocb);		/* stream.h */
EXPORT_SYMBOL_NOVERS(backq);		/* stream.h */
EXPORT_SYMBOL_NOVERS(bcanput);		/* stream.h */
EXPORT_SYMBOL_NOVERS(bcanputnext);	/* stream.h */
EXPORT_SYMBOL_NOVERS(bufcall);		/* stream.h */
EXPORT_SYMBOL_NOVERS(canenable);	/* stream.h */
EXPORT_SYMBOL_NOVERS(canput);		/* stream.h */
EXPORT_SYMBOL_NOVERS(canputnext);	/* stream.h */
EXPORT_SYMBOL_NOVERS(copyb);		/* stream.h */
EXPORT_SYMBOL_NOVERS(copymsg);		/* stream.h */
EXPORT_SYMBOL_NOVERS(datamsg);		/* stream.h */
EXPORT_SYMBOL_NOVERS(dupb);		/* stream.h */
EXPORT_SYMBOL_NOVERS(dupmsg);		/* stream.h */
EXPORT_SYMBOL_NOVERS(enableok);		/* stream.h */
EXPORT_SYMBOL_NOVERS(esballoc);		/* stream.h */
EXPORT_SYMBOL_NOVERS(esbbcall);		/* stream.h */
EXPORT_SYMBOL_NOVERS(flushband);	/* stream.h */
EXPORT_SYMBOL_NOVERS(flushq);		/* stream.h */
EXPORT_SYMBOL_NOVERS(freeb);		/* stream.h */
EXPORT_SYMBOL_NOVERS(freemsg);		/* stream.h */
EXPORT_SYMBOL_NOVERS(freezestr);	/* stream.h */
EXPORT_SYMBOL_NOVERS(getadmin);		/* stream.h */
EXPORT_SYMBOL_NOVERS(getmid);		/* stream.h */
EXPORT_SYMBOL_NOVERS(getq);		/* stream.h */
EXPORT_SYMBOL_NOVERS(insq);		/* stream.h */
EXPORT_SYMBOL_NOVERS(linkb);		/* stream.h */
EXPORT_SYMBOL_NOVERS(msgdsize);		/* stream.h */
EXPORT_SYMBOL_NOVERS(msgpullup);	/* stream.h */
EXPORT_SYMBOL_NOVERS(noenable);		/* stream.h */
EXPORT_SYMBOL_NOVERS(OTHERQ);		/* stream.h */
EXPORT_SYMBOL_NOVERS(pcmsg);		/* stream.h */
EXPORT_SYMBOL_NOVERS(pullupmsg);	/* stream.h */
EXPORT_SYMBOL_NOVERS(put);		/* stream.h */
EXPORT_SYMBOL_NOVERS(putbq);		/* stream.h */
EXPORT_SYMBOL_NOVERS(putctl);		/* stream.h */
EXPORT_SYMBOL_NOVERS(putctl1);		/* stream.h */
EXPORT_SYMBOL_NOVERS(putnext);		/* stream.h */
EXPORT_SYMBOL_NOVERS(putnextctl);	/* stream.h */
EXPORT_SYMBOL_NOVERS(putnextctl1);	/* stream.h */
EXPORT_SYMBOL_NOVERS(putq);		/* stream.h */
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
EXPORT_SYMBOL_NOVERS(unbufcall);	/* stream.h */
EXPORT_SYMBOL_NOVERS(unfreezestr);	/* stream.h */
EXPORT_SYMBOL_NOVERS(unlinkb);		/* stream.h */
EXPORT_SYMBOL_NOVERS(WR);		/* stream.h */
#endif

