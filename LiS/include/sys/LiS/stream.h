/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

/*                               -*- Mode: C -*- 
 * <stream.h> --- Linux STREAMS common declarations. 
 * Author          : gram & nemo
 * Created On      : Mon Oct 17 11:37:12 1994
 * RCS Id          ; $Id: stream.h,v 1.8 1996/01/27 00:40:01 dave Exp $
 * Last Modified By: David Grothe
 * Restrictions    : SHAREd items can be read/writen by usr
 *                 : EXPORTed items can only be read by usr
 *                 : PRIVATEd items cannot be read nor writen
 * Purpose         : provide basic Streams types and declarations
 *                 : for the modules/drivers developers.
 *
 *    Copyright (C) 1995  Graham Wheeler, Francisco J. Ballesteros
 *    Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *    You can reach us by email to any of
 *    gram@aztec.co.za, nemo@ordago.uc3m.es
 */

#ifndef __SYS_LIS_STREAM_H__
#define __SYS_LIS_STREAM_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

/*  -------------------------------------------------------------------  */
/*                               Dependencies                            */

/* This are std defines to allow you to include only `stream.h' and run
 * miles w/o including anything else from LiS. */

/* plain usr API */
#ifndef _STRPORT_H
#include <sys/strport.h>	/* porting structures */
#endif

#ifndef _LIS_CONFIG_H
#include <sys/strconfig.h>	/* streams config symbols are here */
#endif
#ifndef _STR_MSG_H
#include <sys/strmsg.h>		/* streams msgs */
#endif
#ifndef _SYS_STROPTS_H
#include <sys/stropts.h>	/* streams operations */
#endif
#ifndef	DKI_H
#include <sys/dki.h>		/* SVR4 compatible prototypes */
#endif
#ifndef	__SYS_LIS_DDI_H__
#include <sys/LiS/ddi.h>	/* SVR4 compatible prototypes */
#endif

/*include everything exported to usr-level from LiS internal modules */

#ifndef _SHARE_H
#include <sys/LiS/share.h>	/* some generic stuff */
#endif
#ifndef _MSG_H
#include <sys/LiS/msg.h>	/* streams msgs management */
#endif
#ifndef _MSGUTL_H
#include <sys/LiS/msgutl.h>	/* streams msg utilities */
#endif
#ifndef _MOD_H
#include <sys/LiS/mod.h>	/* streams modules */
#endif
#ifndef _QUEUE_H
#include <sys/LiS/queue.h>	/* streams queue symbols & types */
#endif
#ifndef _BUFFCALL_H
#include <sys/LiS/buffcall.h>	/* buffcalls & mem stuff */
#endif
#ifndef _EVENTS_H
#include <sys/LiS/events.h>	/* streams events */
#endif
#ifndef _STATS_H
#include <sys/LiS/stats.h>	/* streams statistics */
#endif
#ifndef _HEAD_H
#include <sys/LiS/head.h>	/* streams statistics */
#endif
#ifndef _WAIT_H
#include <sys/LiS/wait.h>	/* timers,waits,... */
#endif
#ifndef	_STRMDBG_H
#include <sys/LiS/strmdbg.h>	/* debugging stuff */
#endif

/*  -------------------------------------------------------------------  */
/*                               Entry points                            */

/* This are just definitions to the appropriate functions exported
 * from any of the <sys/LiS/...> stuff
 * We shouldn't have exported symbols starting w/ other than `lis' to
 * avoid kernel namespace pollution. 
 */

/* get ptr to the queue behind q. That queue with q_next == q.
 * returns NULL if no such queue
 */
#ifdef __KERNEL__
#define backq(q)	lis_backq_fcn((q),__FILE__,__LINE__)
#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */

/* test for flow cntl in band
 * returns STR_OK if msg can be put(), STR_ERR (0) if not.
 */
#ifdef __KERNEL__
#define bcanput		lis_bcanput
#define bcanputnext(q,b) lis_bcanput((q)->q_next,(b))
#define canput(q)       bcanput((q),0)
#define canputnext(q)   bcanputnext(q,0)
#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */
#ifdef __KERNEL__
#define  noenable    lis_noenable
#define  enableok    lis_enableok
#define  canenable   lis_canenable
#define  putnext     lis_putnext
#define  qreply      lis_qreply
#define	 OTHERQ      LIS_OTHERQ
#define	 OTHER       LIS_OTHERQ
#define	 RD          LIS_RD
#define	 WR          LIS_WR
#define  SAMESTR     LIS_SAMESTR
#define	 strqset	lis_strqset
#define	 strqget	lis_strqget
extern void _RP lis_safe_putmsg(queue_t *q, mblk_t *mp, char *f, int l);

#define put(q,m)	lis_safe_putmsg((q),(m),__FILE__,__LINE__)
#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */

#ifdef __KERNEL__

/*Remove a msg block (2nd arg) from the msg */
#define rmvb	lis_rmvb

/* Allocate a message */
#define allocb(s1,s2)	lis_allocb(s1,s2,"mblk " __FILE__,__LINE__)
#define esballoc(b,s,p,f) lis_esballoc(b,s,p,f,"mblk " __FILE__,__LINE__)
#define allocb_physreq(s1,s2,s3)	\
		       lis_allocb_physreq(s1,s2,s3,"physblk " __FILE__,__LINE__)

/* Check if a message can be allocated */
#define testb	lis_testb

/* Free a message block */
#define freeb	lis_freeb

/* Free a message list */
#define freemsg	lis_freemsg

/* Duplicate a message block */
#define dupb		lis_dupb

/* Duplicate a message */
#define dupmsg	lis_dupmsg

/* Copy a message block */
#define copyb	lis_copyb

/* Copy a message */
#define copymsg	lis_copymsg

/* Link message blocks */
#define linkb	lis_linkb

/* Unlink first block */
#define unlinkb	lis_unlinkb

/* Pull up the first N bytes */
#define pullupmsg	lis_pullupmsg
#define msgpullup	lis_msgpullup

/* Trim first/last(N<0) bytes */
#define adjmsg	lis_adjmsg

/* # data bytes */
#define msgdsize	lis_msgdsize

/* # bytes of first msg block type */
#define xmsgsize	lis_xmsgsize

/* # bytes in all blocks */
#define msgsize	lis_msgsize

/* Remove first msg from queue */
#define getq	lis_getq

/* Remove this msg from queue */
#define rmvq	lis_rmvq

/* Flush messages */
#define flushq	lis_flushq
#define	flushband	lis_flushband

/* Append message to queue */
#define putq	lis_putq
#define putqf	lis_putqf

/* Put message back onto queue */
#define putbq	lis_putbq
#define putbqf	lis_putbqf

/* Insert message(3) before message(2) or at end */
#define insq	lis_insq

/* Zero-byte ctl msg */
#define putctl(q, t)		lis_putctl((q),(t), __FILE__,__LINE__)
#define putnextctl(q, t)	lis_putnextctl((q),(t), __FILE__,__LINE__)

/* One-byte ctl msg */
#define putctl1(q, t, p)	lis_putctl1((q),(t),(p), __FILE__,__LINE__)
#define putnextctl1(q, t, p)	lis_putnextctl1((q),(t),(p), __FILE__,__LINE__)

/* Schedule a service procedure */
#define qenable		lis_qenable

/* Enable or disable q procs for MP version */
#define	qprocson	lis_qprocson
#define	qprocsoff	lis_qprocsoff

/* recover from allocb failure: call this when allocb() rets NULL
 * func will be called asap when a buffer for size bytes is available.
 * arg is the argument given to func
 * Returns STR_OK if ok and STR_ERR if couldn't install func.
 */
#define bufcall 	 lis_bufcall
#define esbbcall	 lis_esbbcall
#define unbufcall	 lis_unbufcall

/* Check is msg is a data msg
 */
#define datamsg	    lis_datamsg
#define isdatamsg   lis_isdatamsg
#define isdatablk   lis_isdatablk

#endif				/* __KERNEL__ */

/*  -------------------------------------------------------------------  */

#ifdef __KERNEL__
#ifdef __LIS_INTERNAL__
extern int lis_init_stream_subsystem(int memlimit);
#endif
/* void lis_scantmout_handler(unsigned long arg); */
/* void lis_runqueues(void) ; */
#endif				/* __KERNEL_ */

/*  -------------------------------------------------------------------  */

#endif				/* __SYS_LIS_STREAM_H__ */
/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
