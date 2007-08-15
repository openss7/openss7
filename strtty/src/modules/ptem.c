/*****************************************************************************

 @(#) $RCSfile: ptem.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/08/15 05:34:51 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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

 Last Modified $Date: 2007/08/15 05:34:51 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ptem.c,v $
 Revision 0.9.2.8  2007/08/15 05:34:51  brian
 - GPLv3 updates

 Revision 0.9.2.7  2007/08/14 03:04:36  brian
 - GPLv3 header update

 Revision 0.9.2.6  2007/07/14 01:36:53  brian
 - make license explicit, add documentation

 Revision 0.9.2.5  2007/03/25 19:02:25  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.4  2007/03/25 06:00:57  brian
 - flush corrections

 Revision 0.9.2.3  2006/10/12 09:37:42  brian
 - completed much of the strtty package

 Revision 0.9.2.2  2006/10/02 11:32:14  brian
 - changes to get master builds working for RPM and DEB
 - added outside licenses to package documentation
 - added LICENSE automated release file
 - copy MANUAL to source directory
 - add and remove devices in -dev debian subpackages
 - get debian rules working better
 - release library version files
 - added notes to debian changelog
 - corrections for cooked manual pages in spec files
 - added release documentation to spec and rules files
 - copyright header updates
 - moved controlling tty checks in stream head
 - missing some defines for LiS build in various source files
 - added OSI headers to striso package
 - added includes and manual page paths to acincludes for various packages
 - added sunrpc, uidlpi, uinpi and uitpi licenses to documentation and release
   files
 - moved pragma weak statements ahead of declarations
 - changes for master build of RPMS and DEBS with LiS

 Revision 0.9.2.1  2006/09/29 11:40:07  brian
 - new files for strtty package and manual pages

 *****************************************************************************/

#ident "@(#) $RCSfile: ptem.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/08/15 05:34:51 $"

static char const ident[] =
    "$RCSfile: ptem.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/08/15 05:34:51 $";

/*
 * PTEM - Pseudo Terminal Emulation Module.
 *
 * Pushed over LDTERM to provide a pseudo terminal on the slave side.
 */

/*
   ptem -- STREAMS pseudo-terminal emulation module 

Description

   ptem  is  a  STREAMS  module that when used in conjunction with a line
   discipline  and  pseudo  terminal  driver  emulates  a  terminal.  See
   [6]pseudo(1).

   The  ptem  module must be pushed [see I_PUSH, [7]streamio(7)] onto the
   slave  side  of  a pseudo terminal STREAM, before the ldterm module is
   pushed.

   On  the  write-side,  the  TCSETA,  TCSETAF,  TCSETAW, TCGETA, TCSETS,
   TCSETSW, TCSETSF, TCGETS, TCSBRK, JWINSIZE, TIOCGWINSZ, and TIOCSWINSZ
   termio  [8]ioctl(2) messages are processed and acknowledged. A hang up
   (such  as  stty  0)  is  converted to a zero length M_DATA message and
   passed downstream. termio cflags window row and column information are
   stored  locally  one  per  stream. M_DELAY messages are discarded. All
   other messages are passed downstream unmodified.

   On  the read-side all messages are passed upstream unmodified with the
   following  exceptions.  All  M_READ  and M_DELAY messages are freed in
   both  directions. An ioctl TCSBRK is converted to an [9]M_BREAK(D7str)
   message  and  passed  upstream  and  an  acknowledgement  is  returned
   downstream.    An    ioctl    TIOCSIGNAL    is   converted   into   an
   [10]M_PCSIG(D7str) message, and passed upstream and an acknowledgement
   is  returned downstream. Finally an ioctl TIOCREMOTE is converted into
   an [11]M_CTL(D7str) message, acknowledged, and passed upstream.

References

   [12]crash(1M),      [13]ioctl(2),      [14]ldterm(7),     [15]pckt(7),
   [16]pseudo(1), [17]pty(7), [18]streamio(7), [19]stty(1), [20]termio(7)
     _________________________________________________________________

   [21]© 2002 Caldera International, Inc. All rights reserved.
   UnixWare 7 Release 7.1.3 - 30 October 2002
*/

#include <sys/os7/compat.h>

#include <sys/strtty.h>
#include <ttcompat.h>

#define PTEM_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define PTEM_COPYRIGHT		"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define PTEM_REVISION		"OpenSS7 $RCSfile: ptem.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/08/15 05:34:51 $"
#define PTEM_DEVICE		"SVR 4.2 STREAMS Pseudo-Terminal Emulation Module (PTEM)"
#define PTEM_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define PTEM_LICENSE		"GPL"
#define PTEM_BANNER		PTEM_DESCRIP	"\n" \
				PTEM_COPYRIGHT	"\n" \
				PTEM_REVISION	"\n" \
				PTEM_DEVICE	"\n" \
				PTEM_CONTACT
#define PTEM_SPLASH		PTEM_DEVICE	" - " \
				PTEM_REVISION

#ifdef LINUX
#ifdef CONFIG_STREAMS_PTEM_MODULE
MODULE_AUTHOR(PTEM_CONTACT);
MODULE_DESCRIPTION(PTEM_DESCRIP);
MODULE_SUPPORTED_DEVICE(PTEM_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(PTEM_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-ptem");
#endif				/* MODULE_ALIAS */
#endif				/* CONFIG_STREAMS_PTEM_MODULE */
#endif				/* LINUX */

#ifndef CONFIG_STREAMS_PTEM_NAME
#error CONFIG_STREAMS_PTEM_NAME must be defined.
#endif
#ifndef CONFIG_STREAMS_PTEM_MODID
#error CONFIG_STREAMS_PTEM_MODID must be defined.
#endif

modID_t modid = CONFIG_STREAMS_PTEM_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for PTEM (0 for allocation).");

#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_PTEM_MODID));
MODULE_ALIAS("streams-module-ptem");
#endif				/* LFS */
#endif				/* MODULE_ALIAS */

static struct module_info ptem_minfo = {
	.mi_idnum = CONFIG_STREAMS_PTEM_MODID,
	.mi_idname = CONFIG_STREAMS_PCKT_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat ptem_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat ptem_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

#if defined __LP64__ && defined LFS
#   undef WITH_32BIT_CONVERSION
#   define WITH_32BIT_CONVERSION 1
#endif

/*
 * Private Structure.
 */

struct ptem {
	int flags;			/* private flags */
	mblk_t *zero;			/* zero-length message block */
	struct termios c;		/* termios structure */
	struct winsize ws;		/* winsize structure */
};

#define PTEM_HAVE_WINSIZE   (1<<0)
#define PTEM_REMOTE_MODE    (1<<1)
#define PTEM_OUTPUT_STOPPED (1<<2)

/*
 * Use a message block to allocated the private structure.  This is a little
 * more portable that memory caches.  Also, if our data structure is small
 * enough, it will be contained in a FASTBUF that will be in a memory cache
 * anyway.
 */
#define PTEM_PRIV(__q)	    ((struct ptem *)((__q)->q_ptr))

/* 
 *  Write-Side Processing
 */

/**
 * ptem_w_msg - process a message on the write side
 * @q: write queue
 * @mp: message to process
 *
 * Returns 1 when the caller (putp or srvp) needs to queue or requeue the
 * message.  Returns 0 when the message has been disposed and the caller must
 * release its reference to mp.
 *
 * Keep this function out of the way of the fastpath.
 */
static streams_noinline int
ptem_w_msg(queue_t *q, mblk_t *mp)
{
	struct ptem *p = PTEM_PRIV(q);

	/* fast path */
	if (likely(mp->b_datap->db_type == M_DATA)) {
	      m_data:
		if ((p->flags & PTEM_OUTPUT_STOPPED)
		    || (q->q_first != NULL)
		    || (q->q_flag & QSVCBUSY)
		    || (!bcanputnext(q, mp->b_band)))
			return (1);
		putnext(q, mp);
		return (0);
	}

	switch (mp->b_datap->db_type) {
	case M_DATA:
		goto m_data;
	case M_IOCTL:
	{
		struct iocblk *ioc = (struct iocblk *) mp->b_rptr;
		int error = EINVAL;
		int rval = 0;
		int count = 0;
		mblk_t *bp, *cp;

		/* The Stream head is set to recognized all transparent terminal input-output
		   controls and pass them downstream as though they were I_STR input-output
		   controls.  There is also the opportunity to register input-output controls with
		   the Stream head using the TIOC_REPLY message. */
		if (ioc->ioc_count == TRANSPARENT) {
			__swerr();
			goto nak;
		}

		if ((bp = mp->b_cont) == NULL)
			goto nak;

		switch (ioc->ioc_cmd) {
		case TCSETAF:
			/* Note, if properly handled the M_FLUSH message will never be queued and
			   upon successful return from this function, we have already processed the
			   read-side flush along the entire Stream. */
			if (!putnextctl1(q, M_FLUSH, FLUSHR)) {
				error = EAGAIN;
				goto nak;
			}
			/* fall through */
		case TCSETAW:
			/* Note, output should have already drained. */
			/* fall through */
		case TCSETA:
		{
			struct termio *c;
			mblk_t *zp;

			if (!pullupmsg(bp, sizeof(struct termio)))
				goto nak;

			c = (typeof(c)) bp->b_rptr;

			if ((c->c_cflag & CBAUD) == B0) {
				/* slave hangup */
				if ((zp = xchg(&p->zero, NULL)))
					putnext(q, zp);
			} else {
				if (!(cp = copymsg(mp))) {
					error = EAGAIN;
					goto nak;
				}

				p->c.c_iflag = (p->c.c_iflag & 0xffff0000) | c->c_iflag;
				p->c.c_oflag = (p->c.c_oflag & 0xffff0000) | c->c_oflag;
				p->c.c_cflag = (p->c.c_cflag & 0xffff0000) | c->c_cflag;
				p->c.c_lflag = (p->c.c_lflag & 0xffff0000) | c->c_lflag;
				p->c.c_line = c->c_line;
				bcopy(c->c_cc, p->c.c_cc, NCC);

				putnext(q, cp);
			}
			goto ack;
		}

		case TCSETSF:
			/* Note, if properly handled the M_FLUSH message will never be queued and
			   upon successful return from this function, we have already processed the
			   read-side flush along the entire Stream. */
			if (!putnextctl1(q, M_FLUSH, FLUSHR)) {
				error = EAGAIN;
				goto nak;
			}
			/* fall through */
		case TCSETSW:
			/* Note, output should have already drained. */
			/* fall through */
		case TCSETS:
		{
			struct termios *c;
			mblk_t *zp;

			if (!pullupmsg(bp, sizeof(struct termios)))
				goto nak;

			c = (typeof(c)) bp->b_rptr;

			if ((c->c_cflag & CBAUD) == B0) {
				/* slave hangup */
				if ((zp = xchg(&p->zero, NULL)))
					putnext(q, zp);
			} else {
				if (!(cp = copymsg(mp))) {
					error = EAGAIN;
					goto nak;
				}

				p->c = *c;

				putnext(q, cp);
			}
			goto ack;
		}

		case TCGETA:
		{
			struct termio *c;
			extern void __struct_termio_is_too_large_for_fastbuf(void);

			if (FASTBUF < sizeof(struct termio))
				__struct_termio_is_too_large_for_fastbuf();
			count = sizeof(*c);
			bp->b_rptr = bp->b_datap->db_base;
			bp->b_wptr = bp->b_rptr + count;
			c = (typeof(c)) bp->b_rptr;

			c->c_iflag = p->c.c_iflag;
			c->c_oflag = p->c.c_oflag;
			c->c_cflag = p->c.c_cflag;
			c->c_lflag = p->c.c_lflag;
			c->c_line = p->c.c_line;
			bcopy(p->c.c_cc, p->c.c_cc, NCC);

			goto ack;
		}
		case TCGETS:
		{
			extern void __struct_termios_is_too_large_for_fastbuf(void);

			if (FASTBUF < sizeof(struct termios))
				__struct_termios_is_too_large_for_fastbuf();
			count = sizeof(p->c);
			bp->b_rptr = bp->b_datap->db_base;
			bp->b_wptr = bp->b_rptr + count;
			*((struct termios *) bp->b_rptr) = p->c;
			goto ack;
		}
		case TIOCGWINSZ:
		{
			extern void __struct_winsize_is_too_large_for_fastbuf(void);

			if (!(p->flags & PTEM_HAVE_WINSIZE))
				goto nak;
			if (FASTBUF < sizeof(struct winsize))
				__struct_winsize_is_too_large_for_fastbuf();
			count = sizeof(p->ws);
			bp->b_rptr = bp->b_datap->db_base;
			bp->b_wptr = bp->b_rptr + count;
			*((struct winsize *) bp->b_rptr) = p->ws;
			goto ack;
		}
#ifdef JWINSIZE
		case JWINSIZE:
		{
			struct jwinsize *jws;
			extern void __struct_jwinsize_is_too_large_for_fastbuf(void);

			if (!(p->flags & PTEM_HAVE_WINSIZE))
				goto nak;
			if (FASTBUF < sizeof(struct jwinsize))
				__struct_jwinsize_is_too_large_for_fastbuf();
			/* always have room in a fastbuf */
			count = sizeof(*jws);
			bp->b_rptr = bp->b_datap->db_base;
			bp->b_wptr = bp->b_rptr + count;
			jws = (typeof(jws)) bp->b_rptr;

			jws->bytesx = p->ws.ws_col;
			jws->bytesy = p->ws.ws_row;
			jws->bitsx = p->ws.ws_xpixel;
			jws->bitsy = p->ws.ws_ypixel;

			goto ack;
		}
#endif				/* JWINSIZE */
		case TIOCSWINSZ:
		{
			struct winsize *ws;
			int changed = 0;
			int zeroed = !(p->flags & PTEM_HAVE_WINSIZE);
			mblk_t *mb;

			if (!pullupmsg(bp, sizeof(*ws)))
				goto nak;
			if (!(cp = copymsg(mp))) {
				error = EAGAIN;
				goto nak;
			}
			if (!(mb = allocb(1, BPRI_MED))) {
				freemsg(cp);
				error = EAGAIN;
				goto nak;
			}
			ws = (typeof(ws)) bp->b_rptr;
			if (ws->ws_col != p->ws.ws_col) {
				if ((p->ws.ws_col = ws->ws_col))
					zeroed = 0;
				changed = 1;
			}
			if (ws->ws_row != p->ws.ws_row) {
				if ((p->ws.ws_row = ws->ws_row))
					zeroed = 0;
				changed = 1;
			}
			if (ws->ws_xpixel != p->ws.ws_xpixel) {
				if ((p->ws.ws_xpixel = ws->ws_xpixel))
					zeroed = 0;
				changed = 1;
			}
			if (ws->ws_ypixel != p->ws.ws_ypixel) {
				if ((p->ws.ws_ypixel = ws->ws_ypixel))
					zeroed = 0;
				changed = 1;
			}
			if (zeroed)
				p->flags &= ~PTEM_HAVE_WINSIZE;
			else
				p->flags |= PTEM_HAVE_WINSIZE;
			if (changed) {
				mb->b_datap->db_type = M_SIG;
				*mb->b_wptr++ = SIGWINCH;
				qreply(q, mb);
			} else
				freeb(mb);
			putnext(q, cp);	/* copy for pctk(4) */
			count = 0;
			goto ack;
		}
		case TCSBRK:
			if (!(cp = copymsg(mp))) {
				error = EAGAIN;
				goto nak;
			}
			putnext(q, cp);
			count = 0;
			goto ack;
		default:
			goto nak;
		}
		break;
	      ack:
		mp->b_datap->db_type = M_IOCACK;
		ioc->ioc_error = 0;
		ioc->ioc_rval = rval;
		ioc->ioc_count = count;
		goto reply;
	      nak:
		mp->b_datap->db_type = M_IOCNAK;
		ioc->ioc_error = error;
		ioc->ioc_rval = -1;
		ioc->ioc_count = 0;
	      reply:
		qreply(q, mp);
		break;
	}
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
		putnext(q, mp);
		break;
	default:
		if (mp->b_datap->db_type < QPCTL) {
			if ((q->q_first != NULL)
			    || (q->q_flag & QSVCBUSY)
			    || (!bcanputnext(q, mp->b_band)))
				return (1);	/* (re)queue */
		}
		putnext(q, mp);
		break;
	}
	return (0);
}

static streamscall int
ptem_wsrv(queue_t *q)
{
	mblk_t *mp;

	/* Simply drain and process them all. */
	while ((mp = getq(q))) {
		if (ptem_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static streamscall __hot_put int
ptem_wput(queue_t *q, mblk_t *mp)
{
	struct ptem *p = PTEM_PRIV(q);

	/* fast path */
	if (likely(mp->b_datap->db_type == M_DATA)) {
	      m_data:
		/* free zero-length messages */
		if (msgdsize(mp) != 0) {
			if ((p->flags & PTEM_OUTPUT_STOPPED)
			    || (q->q_first != NULL)
			    || (q->q_flag & QSVCBUSY)
			    || (!bcanputnext(q, mp->b_band))) {
				/* Note, the only reason for failinng putq() is the lack of a queue 
				   band, in which case the band is empty and no loss of order will
				   result from putting it to the next queue. */
				if (putq(q, mp))
					return (0);
			}
			putnext(q, mp);
			return (0);
		}
		freemsg(mp);
		return (0);
	}

	switch (mp->b_datap->db_type) {
	case M_DATA:
		goto m_data;
	case M_IOCTL:
	{
		struct iocblk *ioc = (struct iocblk *) mp->b_rptr;

		/* The Stream head is set to recognized all transparent terminal input-output
		   controls and pass them downstream as though they were I_STR input-output
		   controls.  There is also the opportunity to register input-output controls with
		   the Stream head using the TIOC_REPLY message. */
		if (unlikely(ioc->ioc_count == TRANSPARENT))
			goto do_it;

		switch (ioc->ioc_cmd) {
		case TCSETAW:
		case TCSETAF:
		case TCSETSW:
		case TCSETSF:
		case TCSBRK:
			/* These need to wait for the output to drain before being processed, queue 
			   them. */
			putq(q, mp);
			break;
		default:
			/* Process others immediately, regardless of whether there is any data or
			   other messages in queue. */
			goto do_it;
		}
		break;
	}
	case M_DELAY:
	case M_READ:
		freemsg(mp);
		break;
	case M_STOP:
		if (canenable(q)) {
			noenable(q);
			p->flags |= PTEM_OUTPUT_STOPPED;
		}
		putnext(q, mp);
		break;
	case M_START:
		if (!canenable(q)) {
			p->flags &= ~PTEM_OUTPUT_STOPPED;
			enableok(q);
			qenable(q);
		}
		putnext(q, mp);
		break;
	case M_STOPI:
	case M_STARTI:
		/* We have no read side queue so we cannot queue in this direction.  Tell master so 
		   that pckt(4) can tell master not to send anything more. */
		putnext(q, mp);
		break;
	default:
	      do_it:
		if (ptem_w_msg(q, mp) && !putq(q, mp))
			freemsg(mp);
		break;
	}
	return (0);
}

/*
 *  Read-Side Processing
 */

/**
 * ptem_r_msg - process a message on the read side
 * @q: read queue
 * @mp: the message to process
 *
 * Returns 1 when the caller (putp or srvp) needs to queue or requeue the
 * message.  Returns 0 when the message has been disposed and the caller must
 * release its reference to mp.
 *
 * Keep this away from the fast path.
 */
static streams_noinline __unlikely void
ptem_r_msg(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int error = EINVAL;
	int count = 0;
	int rval = 0;
	mblk_t *bp;

	/* The Stream head is set to recognized all transparent terminal input-output controls and
	   pass them downstream as though they were I_STR input-output controls.  There is also the 
	   opportunity to register input-output controls with the Stream head using the TIOC_REPLY
	   message. */
	if (unlikely(ioc->ioc_count == TRANSPARENT)) {
		__swerr();
		goto nak;
	}
	if (!(bp = mp->b_cont))
		goto nak;

	switch (ioc->ioc_cmd) {
	case TCSBRK:
		/* When the ptem module receives an M_IOCTL message of type TCSBRK on its read-side 
		   queue, it sends an M_IOCACK message downstream and an M_BREAK message upstream. */
		if (!pullupmsg(bp, sizeof(int)))
			goto nak;
		if (!putnextctl(q, M_BREAK)) {
			error = EAGAIN;
			goto nak;
		}
		goto ack;
	case TIOCGWINSZ:
	{
		/* Keeps track of the informaiton needed for teh TIOCSWINSZ, TIOCGWINSZ, JWINSIZE,
		   input-output control commands. */
		struct ptem *p = PTEM_PRIV(q);
		extern void __struct_winsize_is_too_large_for_fastbuf(void);

		if (!(p->flags & PTEM_HAVE_WINSIZE))
			goto nak;
		if (FASTBUF < sizeof(struct winsize)) {
			__struct_winsize_is_too_large_for_fastbuf();
		}
		/* always have room in a fastbuf */
		count = sizeof(p->ws);
		bp->b_rptr = bp->b_datap->db_base;
		bp->b_wptr = bp->b_rptr + count;
		*(struct winsize *) bp->b_rptr = p->ws;
		goto ack;
	}
#ifdef JWINSIZE
	case JWINSIZE:
		/* Keeps track of the informaiton needed for teh TIOCSWINSZ, TIOCGWINSZ, JWINSIZE,
		   input-output control commands. */
	{
		struct ptem *p = PTEM_PRIV(q);
		struct jwinsize *jws;

		if (!(p->flags & PTEM_HAVE_WINSIZE))
			goto nak;
		if (FASTBUF < sizeof(struct jwinsize))
			__undefined_call_makes_compile_fail();
		/* always have room in a fastbuf */
		count = sizeof(*jws);
		bp->b_rptr = bp->b_datap->db_base;
		bp->b_wptr = bp->b_rptr + count;
		jws = (typeof(jws)) bp->b_rptr;
		jws->bytesx = p->ws.ws_col;
		jws->bytesy = p->ws.ws_row;
		jws->bitsx = p->ws.ws_xpixel;
		jws->bitsy = p->ws.ws_ypixel;
		goto ack;
	}
#endif				/* JWINSIZE */
	case TIOCSWINSZ:
		/* Keeps track of the informaiton needed for teh TIOCSWINSZ, TIOCGWINSZ, JWINSIZE,
		   input-output control commands. */
	{
		struct ptem *p = PTEM_PRIV(q);
		struct winsize *ws;
		int changed = 0;
		int zeroed = !(p->flags & PTEM_HAVE_WINSIZE);
		mblk_t *mb;

		if (!pullupmsg(bp, sizeof(*ws)))
			goto nak;
		if (!(mb = allocb(1, BPRI_MED))) {
			error = EAGAIN;
			goto nak;
		}
		ws = (typeof(ws)) bp->b_rptr;
		if (ws->ws_col != p->ws.ws_col) {
			if ((p->ws.ws_col = ws->ws_col))
				zeroed = 0;
			changed = 1;
		}
		if (ws->ws_row != p->ws.ws_row) {
			if ((p->ws.ws_row = ws->ws_row))
				zeroed = 0;
			changed = 1;
		}
		if (ws->ws_xpixel != p->ws.ws_xpixel) {
			if ((p->ws.ws_xpixel = ws->ws_xpixel))
				zeroed = 0;
			changed = 1;
		}
		if (ws->ws_ypixel != p->ws.ws_ypixel) {
			if ((p->ws.ws_ypixel = ws->ws_ypixel))
				zeroed = 0;
			changed = 1;
		}
		if (zeroed)
			p->flags &= ~PTEM_HAVE_WINSIZE;
		else
			p->flags |= PTEM_HAVE_WINSIZE;
		if (changed) {
			mb->b_datap->db_type = M_SIG;
			*mb->b_wptr++ = SIGWINCH;
			putnext(q, mb);
		} else
			freeb(mb);
		count = 0;
		goto ack;
	}
#ifdef TIOCSIGNAL
	case TIOCSIGNAL:
#endif				/* TIOCSIGNAL */
#ifdef O_TIOCSIGNAL
	case O_TIOCSIGNAL:
#endif				/* O_TIOCSIGNAL */
	{
		uint s;

		if (!pullupmsg(bp, sizeof(s)))
			goto nak;
		if ((s = *(uint *) bp->b_rptr) > _NSIG || s == 0)
			goto nak;
		if (!putnextctl1(q, M_PCSIG, s)) {
			error = EAGAIN;
			goto nak;
		}
		count = 0;
		goto ack;
	}
#ifdef TIOCREMOTE
	case TIOCREMOTE:
#endif				/* TIOCREMOTE */
#ifdef O_TIOCREMOTE
	case O_TIOCREMOTE:
#endif				/* O_TIOCREMOTE */
	{
		struct ptem *p = PTEM_PRIV(q);
		struct iocblk *ctl;
		mblk_t *mb;

		if (!pullupmsg(bp, sizeof(uint)))
			goto nak;
		if (!(mb = allocb(sizeof(*ctl), BPRI_MED))) {
			error = EAGAIN;
			goto nak;
		}
		mb->b_datap->db_type = M_CTL;
		ctl = (typeof(ctl)) mb->b_rptr;
		mb->b_wptr += sizeof(*ctl);
		bzero(ctl, sizeof(ctl));
		if (*(uint *) bp->b_rptr) {
			ctl->ioc_cmd = MC_NO_CANON;
			p->flags |= PTEM_REMOTE_MODE;
		} else {
			ctl->ioc_cmd = MC_DO_CANON;
			p->flags &= ~PTEM_REMOTE_MODE;
		}
		putnext(q, mb);
		count = 0;
		goto ack;
	}
	default:
		break;
	}
	putnext(q, mp);
	return;

      ack:
	mp->b_datap->db_type = M_IOCNAK;
	ioc->ioc_error = error;
	ioc->ioc_rval = -1;
	ioc->ioc_count = 0;
      reply:
	qreply(q, mp);
	return;
      nak:
	mp->b_datap->db_type = M_IOCACK;
	ioc->ioc_error = error;
	ioc->ioc_rval = rval;
	ioc->ioc_count = count;
	goto reply;
}

/**
 * ptem_rput - read put procedure
 * @q: read queue
 * @mp: message to put
 */
static streamscall __hot_get int
ptem_rput(queue_t *q, mblk_t *mp)
{
	/* fast path for data */
	if (likely(mp->b_datap->db_type == M_DATA)) {
	      m_data:
		/* no read side service procedure, do not check for flow control. */
		putnext(q, mp);
		return (0);
	}
	switch (mp->b_datap->db_type) {
	case M_DATA:
		goto m_data;
	case M_IOCDATA:
	case M_IOCACK:
		/* should not happen */
		swerr();
	case M_READ:
	case M_DELAY:
	case M_IOCNAK:
		freemsg(mp);
		break;
	default:
		swerr();
	case M_HANGUP:
	case M_FLUSH:		/* we do not queue on the read side */
		putnext(q, mp);
		break;
	case M_IOCTL:
		ptem_r_msg(q, mp);
		break;
	}
	return (0);
}

/*
 * ---------------------------------------------------------------------------
 *
 * OPEN and CLOSE Routines.
 *
 * ---------------------------------------------------------------------------
 */

/**
 * ptem_qopen - STREAMS module open routine
 * @q: read queue
 * @devp: number of device
 * @oflag: flags to open(2s) call
 * @sflag: STREAMS flag
 * @crp: credentials pointer
 *
 * When ptem is pushed successfully, it assigns this Stream as a controlling
 * terminal if one has not already been assigend by sending a M_SETOPTS
 * message to the Stream head with the SO_ISTTY flag set.  (The Stream head
 * determines whether the process already has a controlling tty.)  If,
 * however, the O_NOCTTY flag was set to open (and appears in oflag as
 * FNOCTTY), then no controlling tty should be assigned.  It is the job of the
 * Stream head to detect this too.
 *
 * When the module is pushed, if we are using old TTY semantics we need to
 * send SO_NDELON to the Stream head in an M_SETOPTS.  It is typical to change
 * the Stream head water marks and packet sizes using SO_MINPSZ, SO_MAXPSZ,
 * SO_HIWAT and SO_LOWAT in the M_SETOPTS.
 *
 * As we don't know (and can't query) the state of the Stream head, we need to
 * set several options that should be correct if the Stream head started in
 * the default state (which it might not have).  SO_MREADOFF should be set as
 * we default to canonical mode and do not want read notifications.  Also
 * RMSGN mode to SO_RDOPT for canonical mode (RNORM for non-canonical).
 * SO_NODELIM should probably be set, but see the discussion under
 * M_SETOPTS(9).  SO_WROFF should be set to zero.  We may or may not want to
 * set the SO_STRHOLD feature.  Do not set it for now and see if we can get a
 * performance gain from it.
 */
static streamscall int
ptem_qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	if (q->q_ptr)
		return (0);	/* already open */
	if (sflag == MODOPEN && WR(q)->q_next != NULL) {
		struct stroptions *so;
		struct ptem *p;
		mblk_t *mp, *hp;

		if (!(hp = allocb(0, BPRI_WAITOK)))
			goto openfail;

		/* zero-length message block is good as it is */

		if (!(mp = allocb(sizeof(*so), BPRI_WAITOK))) {
			freeb(hp);
			goto openfail;
		}

		/* I don't really know what the point of this is, ldterm() will do this again.
		   Everything except the read options and SO_ISTTY the water marks and SO_READOPT
		   are defaults. */
		mp->b_datap->db_type = M_SETOPTS;
		so = (struct stroptions *) mp->b_rptr;
		mp->b_wptr += sizeof(*so);
		so->so_flags = SO_ISTTY;

		if (!(p = kmem_alloc(sizeof(*p), KM_SLEEP))) {
			freeb(mp);
			freeb(hp);
			goto openfail;
		}

		bzero(p, sizeof(*p));
		p->zero = hp;
		p->c.c_iflag = BRKINT | ICRNL | IXON | ISTRIP | IXANY;
		p->c.c_oflag = OPOST | ONLCR | TAB3;
		p->c.c_cflag = 0;
		p->c.c_lflag = ISIG | ICANON | ECHO | ECHOK;

		q->q_ptr = WR(q)->q_ptr = (void *) p;
		qprocson(q);

		putnext(q, mp);
		return (0);
	}
      openfail:
	return (OPENFAIL);
}

static streamscall int
ptem_qclose(queue_t *q, int oflag, cred_t *crp)
{
	struct ptem *p;

	if ((p = PTEM_PRIV(q))) {
		mblk_t *mp;

		/* didn't hang up, do it now */
		if ((mp = xchg(&p->zero, NULL)))
			putnext(WR(q), mp);

		qprocsoff(q);
		q->q_ptr = WR(q)->q_ptr = NULL;
		kmem_free(p, sizeof(*p));
		return (0);
	}
	return (EIO);
}

/*
 * ---------------------------------------------------------------------------
 *
 * Registration and Initialization.
 *
 * ---------------------------------------------------------------------------
 */

static struct qinit ptem_rinit = {
	.qi_putp = ptem_rput,		/* Read-side put procedure. */
	.qi_qopen = ptem_qopen,		/* Each open. */
	.qi_qclose = ptem_qclose,	/* Last close. */
	.qi_minfo = &ptem_minfo,	/* Module information. */
	.qi_mstat = &ptem_rstat,	/* Module statistics. */
};

static struct qinit ptem_winit = {
	.qi_putp = ptem_wput,		/* Write-side put procedure. */
	.qi_srvp = ptem_wsrv,		/* Write-side service procedure. */
	.qi_minfo = &ptem_minfo,	/* Module information. */
	.qi_mstat = &ptem_wstat,	/* Module statistics. */
};

static struct streamtab ptem_info = {
	.st_rdinit = &ptem_rinit,
	.st_wrinit = &ptem_winit,
};

#ifdef LIS
#define fmodsw _fmodsw
#endif
static struct fmodsw ptem_fmod = {
	.f_name = CONFIG_STREAMS_PTEM_NAME,
	.f_str = &ptem_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_PTEM_MODULE
static
#endif
int __init
ptem_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_PTEM_MODULE
	cmn_err(CE_NOTE, PTEM_BANNER);
#else
	cmn_err(CE_NOTE, PTEM_SPLASH);
#endif
	ptem_minfo.mi_idnum = modid;
	if ((err = register_strmod(&ptem_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
};

#ifdef CONFIG_STREAMS_PTEM_MODULE
static
#endif
void __exit
ptem_exit(void)
{
	unregister_strmod(&ptem_fmod);
};

/*
 * ---------------------------------------------------------------------------
 *
 * Linux Registration.
 *
 * ---------------------------------------------------------------------------
 */

#ifdef CONFIG_STREAMS_PTEM_MODULE
module_init(ptem_init);
module_exit(ptem_exit);
#endif
