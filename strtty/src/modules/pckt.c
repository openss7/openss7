/*****************************************************************************

 @(#) $RCSfile: pckt.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/08/14 03:04:36 $

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

 Last Modified $Date: 2007/08/14 03:04:36 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: pckt.c,v $
 Revision 0.9.2.7  2007/08/14 03:04:36  brian
 - GPLv3 header update

 Revision 0.9.2.6  2007/07/14 01:36:53  brian
 - make license explicit, add documentation

 Revision 0.9.2.5  2007/03/25 19:02:25  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.4  2006/10/19 10:39:02  brian
 - corrections for compile on SuSE 10.0

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

#ident "@(#) $RCSfile: pckt.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/08/14 03:04:36 $"

static char const ident[] =
    "$RCSfile: pckt.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/08/14 03:04:36 $";

/*
 * This is the pckt(4) STREAMS module, a Packet Mode module to be pushed on the
 * master side of a pseudo-terminal device, pty(4).  It is a simple device that
 * performs the following:
 *
 * pckt is a STREAMS(9) module that is pushed over the control end of a
 * STREAMS-based master pseudo-terminal device, pty(4), to packetize messages
 * sent by the slave side of a pseudo-terminal that would otherwise be processed
 * by the Stream head. This allows the user process on the master side of the
 * pseudo-terminal to be informed of state changes.
 *
 * To enable the packet mode on the master side of a pseudo-terminal, pty(4),
 * the master side user process must push the "pckt" module using the I_PUSH(7)
 * STREAMS input-output control command.
 *
 * Once packet mode has been enabled on the Stream by pushing the pckt module,
 * all all write-side, write(2s), putmsg(2), putpmsg(2s) operations, are
 * unaltered. When the user process writes or puts a message to the Stream, it
 * is passed to the slave by the pseudo-terminal device, pty(4), as normal.
 *
 * In the packet mode, read-side, read(2s), getmsg(2), getpmsg(2s) operations,
 * are altered. The pckt module intercepts certain STREAMS(9) messages that
 * would otherwise be processed by a real tty(4) driver, and converts them into
 * M_PROTO(9) messages that can be subsequently read by the user process using
 * read(2s), getmsg(2), or getpmsg(2s). (Normally, without the pckt module
 * pushed, these intercepted messages would be processed by the Stream head.)
 *
 * Messages are encapsulated (packetized) by transforming them into M_DATA(9)
 * messages and prepending an M_PROTO(9) message block that contains the
 * original value of the message type (before it was overwritten with M_DATA(9))
 * in the M_PROTO(9) message block.
 *
 * Message types that are transformed in this fashion are: M_DATA(9),
 * M_FLUSH(9), M_IOCTL(9), M_PCPROTO(9), M_PROTO(9), M_READ(9), M_START(9),
 * M_STARTI(9), M_STOP(9) and M_STOPI(9). Other messages are passed to the
 * Stream head unchanged. Of the messages packetized, most are simply packetized
 * and passed to the Stream head to be read by the user process. Several receive
 * special treatment as follows:
 *
 * M_FLUSH(9)	The pseudo-terminal device, pty(4), slave side reverses the send
 *		of the M_FLUSH(9) flush bits so that they can be used directly
 *		by the master side Stream head. This is similar to the
 *		pipemod(4) module. To provide an encapsulated message that
 *		contains flush bits that are exactly as they were issued by the
 *		user of the slave side of the pty(4), pckt reverses the FLUSHR
 *		and FLUSHW bits before packetizing the M_FLUSH(9) message. Also,
 *		because every Stream must respond to M_FLUSH(9) by flushing
 *		queues, pckt also passes the M_FLUSH(9) message to the Stream
 *		head. However, to preserve packetized messages that may be
 *		sitting on the Stream head read queue, the read side is not
 *		flushed and the FLUSHR bit in any M_FLUSH(9) message passed to
 *		the Stream head will be cleared.
 *
 *		The result is as follows, depending on the value of the
 *		M_FLUSH(9) bits:
 *		
 *		FLUSHR	    The bits are set to FLUSHW and the message is
 *			    packetized. No M_FLUSH(9) message is sent to the
 *			    Stream head.
 *
 *		FLUSHW	    The bits are set to FLUSHR and the message is
 *			    packetized. An M_FLUSH(9) message is sent to the
 *			    Stream head containing the FLUSHW flag.
 *
 *		FLUSHRW	    The bits are set to FLUSHRW and the message is
 *			    packetized. An M_FLUSH(9) message is sent to the
 *			    Stream head containing only the FLUSHW flag.
 *
 * M_IOCTL(9)	The M_IOCTL(9) message is packetized as normal on 32-bit
 *		systems. On 64-bit systems, where the user process that pushed
 *		the pckt module on the master side of the pseudo-terminal,
 *		pty(4), device is a 32-bit process, the iocblk(9) structure
 *		contained in the message block is transformed by the pckt module
 *		into a 32-bit representation of the iocblk(9) structure (struct
 *		iocblk32) before being packetized.
 *
 * M_READ(9)	The M_READ(9) message is packetized as normal on 32-bit systems.
 *		On 64-bit systems, where the user process that pushed the pckt
 *		module on the master side of the pseudo-terminal, pty(4), device
 *		is a 32-bit process, the size_t count contained in the message
 *		block is transformed by the pckt module into a 32-bit
 *		representation of the size_t (size32_t) before being packetized.
 *
 *
 * The user process reading the master side of the pseudo-terminal device is
 * responsible for any other 64-bit to 32-bit conversions that might be required
 * on data structures contained in other messages of which the pckt module is
 * unaware.
 */

/*
 * COMPATIBILITY
 *	pckt is compatible with SVR 4.2[1], and implementations based on SVR
 *	4.2[1], such as Solaris®[2], OSF/1®[3], and UnixWare®[4], with the
 *	following portability considerations:
 *
 *	--  Solaris®[2] and OSF/1®[3] document the reversal of the M_FLUSH(9)
 *	    flush bits. OSF/1®[3] has an error in its documentation with regard
 *	    to the handling of the FLUSHRW bits for packetization.
 *
 *	--  UnixWare®[4] documents that the original message type is contained in
 *	    the first four (4) bytes of the M_PROTO(9) message block delivered
 *	    to the Stream head in packet mode. (This is presumably to maintain
 *	    structure alignment on the data part of the message if read from the
 *	    Stream using the control-data mode of read(2s). Solaris®[2] places
 *	    the original message type in the first (and only) byte of the
 *	    M_PROTO(9) message block. OSF/1® does not document the number of
 *	    message bytes contained in the M_PROTO(9) message block.
 *
 *	    Portable programs will use getmsg(2) or getpmsg(2) to retrieve the
 *	    messages and will examine the length of the control part of the
 *	    message before attempting to interpret the bytes within the control
 *	    part of the message.
 *
 *	--  UnixWare® does not include M_START(9) is the list of processed
 *	    messages in its documentation[4]. As M_START(9) is a message
 *	    commonly sent by ldterm(4) to a terminal device, tty(4), this is
 *	    probably a documentation error.
 *
 *	See STREAMS(9) for more compatibility information.
 *
 * CONFORMANCE
 *	SVR 4.2[1], SVID[5], XID[6], SUSv2[7], SUSv3[8].
 *
 * HISTORY
 *	pckt first appeared in SVR 3[9].
 *
 * REFERENCES
 *	[1] SVR 4.2, UNIX® System V Release 4.2 Programmer's Manual, 1992,
 *	    (Englewood Cliffs, New Jersey), AT&T UNIX System Laboratories, Inc.,
 *	    Prentice Hall.
 *
 *	[2] Solaris® 8, Solaris 8 Docmentation, 2001, (Santa Clara, California),
 *	    Sun Microsystems, Inc., Sun.  <http://docs.sun.com/>
 *
 *	[3] Digital® UNIX (OSF/1.2), Digital UNIX Documentation Library, 2003,
 *	    (Palo Alto, California), Digital Equipment Corporation,
 *	    Hewlett-Packard Company.  <http://www.true64unix.compaq.com/docs/>
 *
 *	[4] UnixWare® 7.1.3, UnixWare 7.1.3 (OpenUnix 8) Documentation, 2002,
 *	    (Lindon, Utah), Caldera International, Inc., Caldera.
 *	    <http://uw713doc.sco.com/>
 *
 *	[5] SVID, System V, Interface Definition, No. Fourth Edition.
 *
 *	[6] XBD Issue 5, X/Open System Interface Definitions, Open Group, Open
 *	    Group Publication.  <http://www.opengroup.org/onlinepubs/>
 *
 *	[7] SUS Version 2, Single UNIX Specification, Open Group, Open Group
 *	    Publication.  <http://www.opengroup.org/onlinepubs/>
 *
 *	[8] SUS Version 3, Single UNIX Specification, Open Group, Open Group
 *	    Publication.  <http://www.opengroup.org/onlinepubs/>
 *
 *	[9] SVR 3, UNIX® System V Release 3 Programmer's Manual, (Englewood
 *	Cliffs, New Jersey), AT&T UNIX System Laboratories, Inc., Prentice Hall.
 */

#include <sys/os7/compat.h>

#define PCKT_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define PCKT_COPYRIGHT		"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define PCKT_REVISION		"OpenSS7 $RCSfile: pckt.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/08/14 03:04:36 $"
#define PCKT_DEVICE		"SVR 4.2 STREAMS Packet Mode Module (PCKT)"
#define PCKT_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define PCKT_LICENSE		"GPL v2"
#define PCKT_BANNER		PCKT_DESCRIP	"\n" \
				PCKT_COPYRIGHT	"\n" \
				PCKT_REVISION	"\n" \
				PCKT_DEVICE	"\n" \
				PCKT_CONTACT
#define PCKT_SPLASH		PCKT_DEVICE	" - " \
				PCKT_REVISION
#ifdef LINUX
MODULE_AUTHOR(PCKT_CONTACT);
MODULE_DESCRIPTION(PCKT_DESCRIP);
MODULE_SUPPORTED_DEVICE(PCKT_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(PCKT_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-pckt");
#endif
#endif				/* LINUX */

#ifndef CONFIG_STREAMS_PCKT_NAME
#error CONFIG_STREAMS_PCKT_NAME must be defined.
#endif
#ifndef CONFIG_STREAMS_PCKT_MODID
#error CONFIG_STREAMS_PCKT_MODID must be defined.
#endif

modID_t modid = CONFIG_STREAMS_PCKT_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for SC.");

#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_PCKT_MODID));
MODULE_ALIAS("streams-module-pckt");
#endif
#endif

static struct module_info pckt_minfo = {
	.mi_idnum = CONFIG_STREAMS_PCKT_MODID,
	.mi_idname = CONFIG_STREAMS_PCKT_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat pckt_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat pckt_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

#if defined __LP64__ && defined LFS
#  undef WITH_32BIT_CONVERSION
#  define WITH_32BIT_CONVERSION 1
#endif

/*
 *  Private structure.
 */

struct pckt {
	int flags;			/* saved file flags */
	bcid_t bufcall;			/* buffer callback */
};

#define PCKT_PRIV(__q) ((struct pckt *)((__q)->q_ptr))

/* 
 *  PUTP and SRVP routines
 */
static streamscall void
pckt_enable(long arg)
{
	queue_t *q = (queue_t *) arg;
	bcid_t bc;
	struct pckt *p;

	if ((p = PCKT_PRIV(q)) && (bc = xchg(&p->bufcall, 0))) {
		qenable(q);
		enableok(q);
	}
}

struct iocblk32 {
	int ioc_cmd32;
	uint32_t ioc_cr32;
	uint ioc_id32;
	uint32_t ioc_count32;
	int ioc_error32;
	int ioc_rval32;
	uint32_t ioc_filler32[4];
};

static int
pckt_r_msg(queue_t *q, mblk_t *mp)
{
	mblk_t *bp, *fp;
	struct pckt *p = PCKT_PRIV(q);

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		/* The pseudo-terminal device, pty(4), slave side reverses the sense of the
		   M_FLUSH(9) flush bits so that they can be used directly by the master side
		   Stream head. This is similar to the pipemod(4) module. To provide an
		   encapsulated message that contains flush bits that are exactly as they were
		   issued by the user of the slave side of the pty(4), pckt reverses the FLUSHR and 
		   FLUSHW bits before packetizing the M_FLUSH(9) message. Also, because every
		   Stream must respond to M_FLUSH(9) by flushing queues, pckt also passes the
		   M_FLUSH(9) message to the Stream head. However, to preserve packetized messages
		   that may be sitting on the Stream head read queue, the read side is not flushed
		   and the FLUSHR bit in any M_FLUSH(9) message passed to the Stream head will be
		   cleared. The result is as follows, depending on the value of the M_FLUSH(9)
		   bits: FLUSHR: The bits are set to FLUSHW and the message is packetized. No
		   M_FLUSH(9) message is sent to the Stream head. FLUSHW: The bits are set to
		   FLUSHR and the message is packetized. An M_FLUSH(9) message is sent to the
		   Stream head containing the FLUSHW flag. FLUSHRW: The bits are set to FLUSHRW and 
		   the message is packetized. An M_FLUSH(9) message is sent to the Stream head
		   containing only the FLUSHW flag.  */
		if (!(bp = allocb(1, BPRI_MED)))
			goto bufcall;
		if ((mp->b_rptr[0] & FLUSHW)) {
			if (!(fp = copyb(mp))) {
				freeb(bp);
				goto bufcall;
			}
			fp->b_rptr[0] &= ~FLUSHR;
			putnext(q, fp);
		} else {
			fp = NULL;
		}
		switch (mp->b_rptr[0] & (FLUSHR | FLUSHW)) {
		case FLUSHR:
			mp->b_rptr[0] &= ~FLUSHR;
			mp->b_rptr[0] |= FLUSHW;
			break;
		case FLUSHW:
			mp->b_rptr[0] &= ~FLUSHW;
			mp->b_rptr[0] |= FLUSHR;
			break;
		}
		goto finish_it;
	case M_IOCTL:
		/* The M_IOCTL(9) message is packetized as normal on 32-bit systems. On 64-bit
		   systems, where the user process that pushed the pckt module on the master side
		   of the pseudo-terminal, pty(4), device is a 32-bit process, the iocblk(9)
		   structure contained in the message block is transformed by the pckt module into
		   a 32-bit representation of the iocblk(9) structure (struct iocblk32) before
		   being packetized.  */
		if ((p->flags & FILP32)) {
			struct iocblk *ioc;
			struct iocblk32 ioc32 = { 0, };

			/* Need to convert from native to ILP32. */
			if ((bp = allocb(1, BPRI_MED)) == NULL)
				goto bufcall;
			ioc = (typeof(ioc)) mp->b_rptr;
			ioc32.ioc_cmd32 = ioc->ioc_cmd;
			ioc32.ioc_cr32 = (uint32_t) (long) ioc->ioc_cr;
			ioc32.ioc_id32 = ioc->ioc_id;
			ioc32.ioc_count32 = ioc->ioc_count;
			ioc32.ioc_error32 = ioc->ioc_error;
			ioc32.ioc_rval32 = ioc->ioc_rval;
			ioc32.ioc_filler32[0] = ioc->ioc_filler[0];
			ioc32.ioc_filler32[1] = ioc->ioc_filler[1];
			ioc32.ioc_filler32[2] = ioc->ioc_filler[2];
			ioc32.ioc_filler32[3] = ioc->ioc_filler[3];
			mp->b_wptr = mp->b_wptr - sizeof(*ioc) + sizeof(ioc32);
			*(struct iocblk32 *) mp->b_rptr = ioc32;
			goto finish_it;
		}
		goto pass_it;
	case M_READ:
		/* The M_READ(9) message is packetized as normal on 32-bit systems.  On 64-bit
		   systems, where the user process that pushed the pckt module on the master side
		   of the pseudo-terminal, pty(4), device is a 32-bit process, the size_t count
		   contained in the message block is transformed by the pckt module into a 32-bit
		   representation of the size_t (size32_t) before being packetized.  */
		if ((p->flags & FILP32)) {
			uint32_t size32;

			/* Need to convert from native to ILP32. */
			if ((bp = allocb(1, BPRI_MED)) == NULL)
				goto bufcall;
			size32 = *(size_t *) mp->b_rptr;
			*(uint32_t *) mp->b_rptr = size32;
			mp->b_wptr = mp->b_wptr - sizeof(size_t) + sizeof(uint32_t);
			goto finish_it;
		}
		goto pass_it;
	case M_PROTO:
	case M_PCPROTO:
	case M_STOP:
	case M_STOPI:
	case M_START:
	case M_STARTI:
	case M_DATA:
	      pass_it:
		/* Problem: UnixWare says 4 bytes.  Solaris says 1 byte.  The user must determine
		   the size and alignment of the message type by the length of the control part.
		   We'll go with 1 byte.  On little endian it should line up. */
		if ((bp = allocb(1, BPRI_MED))) {
		      finish_it:
			bp->b_datap->db_type = M_PROTO;
			bp->b_wptr[0] = mp->b_datap->db_type;
			bp->b_wptr++;
			mp->b_datap->db_type = M_DATA;
			bp->b_cont = mp;
			putnext(q, bp);
			return (1);
		}
	      bufcall:
		noenable(q);
		if (!(p->bufcall = bufcall(1, BPRI_MED, pckt_enable, (long) q)))
			qenable(q);	/* spin through service procedure */
		return (0);
	default:
		putnext(q, mp);
		return (1);
	}
}

static streamscall int
pckt_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (mp->b_datap->db_type < QPCTL) {
			if (!bcanputnext(q, mp->b_band))
				goto put_back;
		}
		if (pckt_r_msg(q, mp))
			continue;
	      put_back:
		if (!putbq(q, mp))
			freemsg(mp);
		break;
	}
	return (0);
}

static streamscall int
pckt_rput(queue_t *q, mblk_t *mp)
{
	if (mp->b_datap->db_type < QPCTL) {
		if (q->q_first)
			goto queue_it;
		if (q->q_flag & QSVCBUSY)
			goto queue_it;
		if (!bcanputnext(q, mp->b_band))
			goto queue_it;
	}
	if (pckt_r_msg(q, mp))
		return (0);
      queue_it:
	if (!putq(q, mp))
		freemsg(mp);
	return (0);
}

/**
 * pckt_wput - write-side put procedure
 * @q: write queue
 * @mp: message to put
 *
 * When the user process writes data, the pckt module passes the message on to
 * the next module or driver unchanged.
 */
static streamscall int
pckt_wput(queue_t *q, mblk_t *mp)
{
	/* No write-side service procedure, no need to check for flow control, pass the message
	   along. */
	putnext(q, mp);
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
static streamscall int
pckt_qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	if (q->q_ptr)
		return (0);	/* already open */
	if (sflag == MODOPEN && WR(q)->q_next != NULL) {
		struct pckt *p;

		if ((p = kmem_alloc(sizeof(*p), KM_SLEEP))) {
			bzero(p, sizeof(*p));
			p->flags = oflag;	/* to test later for data model */
			q->q_ptr = WR(q)->q_ptr = (void *) p;
			qprocson(q);
			return (0);
		}
		return (OPENFAIL);
	}
	return (OPENFAIL);	/* can't be opened as driver */
}

static streamscall int
pckt_qclose(queue_t *q, int oflag, cred_t *crp)
{
	struct pckt *p;

	qprocsoff(q);
	if ((p = (struct pckt *) q->q_ptr)) {
		bcid_t bc;

		/* atomic exchange for LiS's stupid sake */
		if ((bc = xchg(&p->bufcall, 0)))
			unbufcall(bc);
		kmem_free(p, sizeof(*p));
	}
	q->q_ptr = WR(q)->q_ptr = NULL;
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and initialization
 *
 *  -------------------------------------------------------------------------
 */

static struct qinit pckt_rqinit = {
	.qi_putp = pckt_rput,
	.qi_srvp = pckt_rsrv,
	.qi_qopen = pckt_qopen,
	.qi_qclose = pckt_qclose,
	.qi_minfo = &pckt_minfo,
	.qi_mstat = &pckt_rstat,
};

static struct qinit pckt_wqinit = {
	.qi_putp = pckt_wput,
	.qi_minfo = &pckt_minfo,
	.qi_mstat = &pckt_wstat,
};

static struct streamtab pckt_info = {
	.st_rdinit = &pckt_rqinit,
	.st_wrinit = &pckt_wqinit,
};

#ifdef LIS
#define fmodsw _fmodsw
#endif
static struct fmodsw pckt_fmod = {
	.f_name = CONFIG_STREAMS_PCKT_NAME,
	.f_str = &pckt_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_PCKT_MODULE
static
#endif
int __init
pckt_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_PCKT_MODULE
	cmn_err(CE_NOTE, PCKT_BANNER);
#else
	cmn_err(CE_NOTE, PCKT_SPLASH);
#endif
	pckt_minfo.mi_idnum = modid;
	if ((err = register_strmod(&pckt_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
};

#ifdef CONFIG_STREAMS_PCKT_MODULE
static
#endif
void __exit
pckt_exit(void)
{
	unregister_strmod(&pckt_fmod);
};

#ifdef CONFIG_STREAMS_PCKT_MODULE
module_init(pckt_init);
module_exit(pckt_exit);
#endif
