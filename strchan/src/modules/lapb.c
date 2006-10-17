/*****************************************************************************

 @(#) $RCSfile: lapb.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:56:00 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2006/10/17 11:56:00 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: lapb.c,v $
 Revision 0.9.2.1  2006/10/17 11:56:00  brian
 - copied files into new packages from strss7 package

 *****************************************************************************/

#ident "@(#) $RCSfile: lapb.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:56:00 $"

static char const ident[] = "$RCSfile: lapb.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:56:00 $";

/*
 *  This is a pushable STREAMS module that provides the Link Access Procedure
 *  (Balanced) for X.25.  It presents a Data Link Provider Interface (DLPI) at
 *  the upper service interface and uses a Communications Device Interface
 *  (CDI) at the lower service interface.  This module is for CDI devices and
 *  supports LAPB in software.  Some devices might support LAPB in device
 *  firmware, in which case, the device driver itself will present the DLPI
 *  interface.  The DLPI interface presented supports X.25 PLP modules.
 *
 *  This module is originally intended for use with the HDLC module, but can
 *  be used with any appropriate CDI device.
 *
 *  THis module can also present a pseudo-device driver interface to provide
 *  access to LAPB device driver implementations in the Linux kernel.  Also,
 *  CDI HDLC streams can be linked under the pseudo-device driver and then
 *  addressed by interface (multiplex) identifier from the upper multiplex.
 */

#include <sys/os8/compat.h>

#include <sys/cdi.h>		/* Lower boundary. */
#include <sys/dlpi.h>		/* Upper boundary. */

#define LAPB_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define LAPB_COPYRIGHT	"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define LAPB_REVISION	"OpenSS7 $RCSfile: lapb.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:56:00 $"
#define LAPB_DEVICE	"SVR 4.2 STREAMS Link Access Procedure Balanced (LAPB)"
#define LAPB_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define LAPB_LICENSE	"GPL"
#define LAPB_BANNER	LAPB_DESCRIP	"\n" \
			LAPB_COPYRIGHT	"\n" \
			LAPB_REVISION	"\n" \
			LAPB_DEVICE	"\n" \
			LAPB_CONTACT
#define LAPB_SPLASH	LAPB_DEVICE	" - " \
			LAPB_REVISION

#ifdef LINUX
MODULE_AUTHOR(LAPB_CONTACT);
MODULE_DESCRIPTION(LAPB_DESCRIP);
MODULE_SUPPORTED_DEVICE(LAPB_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(LAPB_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-lapb");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifndef LAPB_MOD_NAME
#define LAPB_MOD_NAME		"lapb"
#endif

#ifndef LAPB_MOD_ID
#define LAPB_MOD_ID		0
#endif

/*
 *  STREAMS Definitions
 */

#define MOD_ID		LAPB_MOD_ID
#define MOD_NAME	LAPB_MOD_NAME
#define DRV_ID		LAPB_MOD_ID
#define DRV_NAME	LAPB_MOD_NAME

#ifdef MODULE
#define MOD_BANNER	LAPB_BANNER
#define DRV_BANNER	LAPB_BANNER
#else				/* MODULE */
#define MOD_BANNER	LAPB_SPLASH
#define DRV_BANNER	LAPB_SPLASH
#endif				/* MODULE */

static struct module_info lapb_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMAXPSZ,
	.mi_minpsz = STRMINPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

/*
 *  PRIVATE STRUCTURE.
 *  ==================
 */

struct dl {
	struct dl *next;
	union {
		struct {
			major_t cmajor;
			minor_t cminor;
		} dev;
		long index;
	} u;
	uint i_state;
	uint i_oldstate;
	dl_info_ack_t info;
};

#define DL_PRIV(__q) ((struct dl *)((__q)->q_ptr))

#define LAPB_FRAMING_SYNCHRONOUS    0x01	/* 5-bit bit stuffing */
#define LAPB_FRAMING_START_STOP	    0x02	/* escape character with start/stop bits */

#define LAPB_ESCAPE_CHAR_DEFAULT    0xBE	/* escape character */
#define LAPB_FLAG_CHAR_DEFAULT	    0x7E	/* flag character */

/*
 *  MESSAGE HANDLING.
 *  =================
 */
static int
cd_data_req(queue_t *q, mblk_t *dp)
{
	struct dl *dl = DL_PRIV(q);
	queue_t *oq;

	if ((oq = dl->wq->q_next) || (oq = (dl->hdlc ? dl->hdlc->wq : NULL))) {
		if (canputnext(oq)) {
			putnext(oq, dp);
			return (0);
		}
		return (-EBUSY);
	}
	return (-EFAULT);
}

static streamscall void
dl_bufcall(long arg)
{
	queue_t *q = (queue_t *) arg;
	struct dl *dl = DL_PRIV(q);

	if (q == dl->rq) {
		if (unlikely(xchg(&dl->rbid, 0) == 0))
			return;
		qenable(q);
		return;
	}
	if (q == dl->wq) {
		if (unlikely(xchg(&dl->wbid, 0) == 0))
			return;
		qenable(q);
		return;
	}
}

static mblk_t *
dl_allocb(queue_t *q, size_t size, int pri, uchar addr, mblk_t *dp)
{
	mblk_t *mp;

	if ((mp = allocb(size, pri))) {
		*mp->b_wptr++ = addr;
		mp->b_cont = dp;
		return (mp);
	} else {
		struct dl *dl = DL_PRIV(q);
		bcid_t bid;

		if (q == dl->rq) {
			if ((bid = xchg(&dl->rbid, bufcall(size, pri, &dl_bufcall, (long) q))))
				unbufcall(bid);
			return (NULL);
		}
		if (q == dl->wq) {
			if ((bid = xchg(&dl->wbid, bufcall(size, pri, &dl_bufcall, (long) q))))
				unbufcall(bid);
			return (NULL);
		}
	}
}

#define DL_MTYPE_RR	0x80
#define DL_MTYPE_RNR	0xA0
#define DL_MTYPE_REJ	0x90
#define DL_MTYPE_I	0x00
#define DL_MTYPE_SABM	0xF4
#define DL_MTYPE_SABME	0xF6
#define DL_MTYPE_DISC	0xC2
#define DL_MTYPE_DM	0xF0
#define DL_MTYPE_UA	0xC0
#define DL_MTYPE_FRMR	0xE1

static int
dl_send_i_frame(queue_t *q, mblk_t *dp, uchar pf, uchar nr, uchar ns)
{
	struct dl *dl = DL_PRIV(q);
	mblk_t *mp;
	int err;

	if ((mp = dl_allocb(q, FASTBUF, BPRI_MED, dl->caddr, dp))) {
		if (dl->extended) {
			*mp->b_wptr++ = ns;
			*mp->b_wptr++ = (pf << 7) | nr;
		} else {
			*mp->b_wptr++ = (ns << 4) | (pf << 3) | nr;
		}
		if ((err = cd_data_req(q, mp))) {
			freeb(mp);
			return (err);
		}
	}
	return (-ENOBUFS);
}

static int
dl_send_s_frame(queue_t *q, uchar cmd, uchar pf, uchar nr, uchar mtype)
{
	struct dl *dl = DL_PRIV(q);
	mblk_t *mp;
	int err;

	if ((mp = dl_allocb(q, FASTBUF, BPRI_MED, cmd ? dl->caddr : dl->raddr, NULL))) {
		if (dl->extended) {
			*mp->b_wptr++ = mtype;
			*mp->b_wptr++ = (pf << 7) | nr;
		} else {
			*mp->b_wptr++ = mtype | (pf << 3) | nr;
		}
		if ((err = cd_data_req(q, mp))) {
			freeb(mp);
			return (err);
		}
	}
	return (-ENOBUFS);
}

static int
dl_send_u_frame(queue_t *q, uchar cmd, uchar pf, uchar mtype)
{
	struct dl *dl = DL_PRIV(q);
	mblk_t *mp;
	int err;

	if ((mp = dl_allocb(q, FASTBUF, BPRI_MED, cmd ? dl->caddr : dl->raddr, NULL))) {
		*mp->b_wptr++ = mtype | (pf << 3);
		if ((err = cd_data_req(q, mp))) {
			freeb(mp);
			return (err);
		}
	}
	return (-ENOBUFS);
}

/* RR - Receive Ready */
static int
dl_send_rr(queue_t *q, uchar cmd, uchar pf, uchar nr)
{
	return dl_send_s_frame(q, cmd, pf, nr, DL_MTYPE_RR);
}

/* RNR - Receive Not Ready */
static int
dl_send_rnr(queue_t *q, uchar cmd, uchar pf, uchar nr)
{
	return dl_send_s_frame(q, cmd, pf, nr, DL_MTYPE_RNR);
}

/* REJ - Reject */
static int
dl_send_rej(queue_t *q, uchar cmd, uchar pf, uchar nr)
{
	return dl_send_s_frame(q, cmd, pf, nr, DL_MTYPE_REJ);
}

/* SABM - Set Asynchronous Balanced Mode */
static int
dl_send_sabm(queue_t *q, uchar pf)
{
	return dl_send_u_frame(q, 1, pf, DL_MTYPE_SABM);
}

/* SABME - Set Asynchronous Balanced Mode Extended */
static int
dl_send_sabme(queue_t *q, uchar pf)
{
	return dl_send_u_frame(q, 1, pf, DL_MTYPE_SABME);
}

/* DISC - Disconnect */
static int
dl_send_disc(queue_t *q, uchar pf)
{
	return dl_send_u_frame(q, 1, pf, DL_MTYPE_DISC);
}

/* DM - Disconnect Mode */
static int
dl_send_dm(queue_t *q, uchar pf)
{
	return dl_send_u_frame(q, 0, pf, DL_MTYPE_DM);
}

/* UA - Unnumbered Acknowledgement */
static int
dl_send_ua(queue_t *q, uchar pf)
{
	return dl_send_u_frame(q, 0, pf, DL_MTYPE_UA);
}

/* FRMR - Frame Reject */
static int
dl_send_frmr(queue_t *q, ushort rfcf, uchar pf, uchar vs, uchar cr, uchar vr, uchar flags)
{
	struct dl *dl = DL_PRIV(q);
	mblk_t *mp;
	int err;

	if ((mp = dl_allocb(q, FASTBUF, BPRI_MED, dl->raddr, NULL))) {
		*mp->b_wptr++ = DL_MTYPE_FRMR | (pf << 3);
		if (dl->extended) {
			if (rfcf & 0xff00) {
				*mp->b_wptr++ = (rfcf >> 8);
				*mp->b_wptr++ = rfcf & 0xff;
			} else {
				*mp->b_wptr++ = rfcf;
				*mp->b_wptr++ = 0;
			}
		} else {
			*mp->b_wptr++ = rfcf;
		}
		*mp->b_wptr++ = (vs << 4) | (cr << 3) | (vr << 0);
		*mp->b_wptr++ = flags;
		if ((err = cd_data_req(q, mp))) {
			freeb(mp);
			return (err);
		}
	}
	return (-ENOBUFS);
}

/* I - Information */
static int
dl_send_i(queue_t *q, mblk_t *dp, uchar pf, uchar nr, uchar ns)
{
	return dl_send_i_frame(q, dp, pf, nr, ns);
}

/* RR - Receive Ready */
/*
 * Receiving acknowlegement: When correctly receiving an I frame or a supervisory frame (RR, RNR, or
 * REJ), even in the busy condition, the DCE will consider the N(R) contained in this frame as an
 * acknowledgement for all I frames it has transmitted with an N(S) up to and including the received
 * N(R)-1.  The DCE will stop timer T1 when it correctly receives an I frame or a supervisory frame
 * with N(R) higher than the last received N(R) (actually acknowledging some I frames), or a REJ frame
 * with an N(R) equal to the last received N(R).  If timer T1 has been stopped by the receipt of an I,
 * RR or RNR frame, and if there are outstanding I frames still unacknowledged, the DCE will restart
 * timer T1.  If timer T1 then runs out, the DCE will follow the recovery procedure with respect to
 * unacknowledged I frames.  If timer T1 has been stopped by the receipt of a REJ frame, the DCE will
 * follow the retransmission procedures. */
static int
dl_recv_rr(queue_t *q, mblk_t *mp, uchar cmd, uchar pf, uchar nr)
{
}

/* RNR - Receive Not Ready */
/*
 * Receiving an RNR frame: After receiving an RNR frame whose N(R) acknowledges all frames previously
 * transmitted, the DCE will stop timer T1 and may then transmit an I frame, with the P bit set to 0,
 * whose send sequence number is equal to the N(R) indicated in the RNR frame, restarting timer T1 as
 * it does.  After receiving an RNR frame whose N(R) indicates a previously transmitted frame, the DCE
 * will not transmit ore retransmit any I frame, timer T1 being already running.  In either case, if
 * timer T1 runs out before receipt of a busy clearance indication, the DCE will folllow the procedure
 * described below.  In any case, the DCE will not transmit any other I frames before receiving an RR
 * or REJ frame, or before the completion of a link resetting procedure.  Alternatively, after
 * receiving an RNR frame, the DCE may wait for a period of time (e.g. the length of the timer T1) and
 * then transmit a supervisory command frame (RR, RNR or REJ) with the P bit set to 1, and start timer
 * T1, in order to determine if there is any change in the receive status of the DTE.  The DTE shall
 * respond to the P bit set to 1 with a supervisory frame (RR, RNR or REJ) with the F bit set to 1
 * indicating either continuance of the busy condition (RNR) or clearance of the busy condition (RR or
 * REJ).  Upon receipt of the DTE response, timer T1 is stopped.  1) If the response is the RR or REJ
 * response, the busy condition is cleared and the DCE may transmit I frames beginning with the I
 * frame identified by the N(R) in the received reponse frame.  2) If the response is the RNR
 * response, the busy condition still exists, and the DCE will after a period of time (e.g. the length
 * of timer T1) repeat the enquiry of the DTE receive status.  If timer T1 runs out before a status
 * reponse is received, the enquiry process above is repeated.  If N2 attempts to get a status
 * response faile (i.e. timer T1 runs out N2 times), the DCE will initiate a data link set-up
 * procedure and enter the disconnected phase.  If at any time during the enquiry process, an
 * unsolicited RR or REJ frame is received from the DTE, it will be considered to be an indication of
 * clearance of the busy condition.  Should the unsolicited RR or REJ frame be a command frame with
 * the P bit set to 1, the appropriate response frame with the F bit set to 1 must be transmitted
 * before the DCE may resume transmission of I frames.  If timer T1 is running, the DCE will wait for
 * the non-busy response with the F bit set to 1 or will wait for timer T1 to run out and then either
 * may reinitiate the enquiry process in order to realize a successful P/F bit exchange or may resume
 * transmission of I frames beginning with the I frame identified by the N(R) in the received RR or
 * REJ frame. */
static int
dl_recv_rnr(queue_t *q, mblk_t *mp, uchar cmd, uchar pf, uchar nr)
{
}

/* REJ - Reject */
/*
 * Receiving a REJ frame: When receiving a REJ frame, the DCE will set its send state varilable V(S)
 * to N(R) received in the REJ control field.  It will transmit the corresponding I frame as soon as
 * it is available or retransmit it in accordance with the transmission procedures.  (Re)transmission
 * will conform to the following procedure: i) if the DCE is transmitting a supervisory command or
 * response when it receives the REJ frame, it will complete that transmission before commencing
 * transmission of the requested I frame; ii) if the DCE is transmitting an unnumbered command or
 * response when it receives the REJ frame, it will ignore the request for retransmission; iii) if the
 * DCE is tranmitting an I frame when the REJ frame is received, it may abort the I frame and commence
 * transmission of the requested I frame immediately after abortion; iv) if the DCE is not
 * transmitting any frame when the REJ frame is received, it will commence transmission of the
 * requested I frame immediately.  In all cases, of other unacknowleged I frames had already been
 * tranmitted following the one in the REJ frame, the those I frames will be retransmitted by the DCE
 * following the retransmission of the requested I frame.  Other I frames not yet transmitted may be
 * transmitted folowing the retransmitted I frames.  If the REJ frame was received from the DTE as a
 * command with the P bit set to 1, the DCE will transmit an RR, RNR or REJ response with the F bit
 * set to 1 before transmitting or retransmitting the corresponding I frame. */
static int
dl_recv_rej(queue_t *q, mblk_t *mp, uchar cmd, uchar pf, uchar nr)
{
}

/* SABM - Set Asynchronous Balanced Mode */
static int
dl_recv_sabm(queue_t *q, mblk_t *mp, uchar cmd, uchar pf, uchar nr)
{
}

/* SABME - Set Asynchronous Balanced Mode Extended */
static int
dl_recv_sabme(queue_t *q, mblk_t *mp, uchar cmd, uchar pf, uchar nr)
{
}

/* DISC - Disconnect */
static int
dl_recv_disc(queue_t *q, mblk_t *mp, uchar cmd, uchar pf, uchar nr)
{
}

/* DM - Disconnect Mode */
static int
dl_recv_dm(queue_t *q, mblk_t *mp, uchar cmd, uchar pf, uchar nr)
{
}

/* UA - Unnumbered Acknowledgement */
static int
dl_recv_ua(queue_t *q, mblk_t *mp, uchar cmd, uchar pf, uchar nr)
{
}

/* FRMR - Frame Reject */
/*
 * Sent in response to the receipt of a command or response field that is unidentified or not
 * implemented; the receipt of an I ream with an information field which exceeds the maximum
 * established length; receipt of an invalid N(R); or, receipt of a frame with an information field
 * which is not permitted or receipt of a supervisory or unnumbered frame with an incorrect length. */
static int
dl_recv_frmr(queue_t *q, mblk_t *mp, uchar cmd, uchar pf, uchar nr)
{
}

/* I - Information */
/*
 * Busy condition: The busy condition results when the DCE or DTE is temporarily unable to continue to
 * receive I frames due to internal constraints, e.g. receive buffering limitations.  In this case, an
 * RNR frame is transmitted from the busy DCE or DTE.  I frames pending transmission may be
 * transmitted from the busy DCE or DTE prior to or following the RNR frame.  An indication that the
 * busy condition has cleared in communicated by the tranmission of a UA (only in response to a
 * SABM/SABME command), RR, REJ or SABM/SABME (module 8/module 128) frame. */
/*
 * N(S) sequence error condition: The infromation field of all I frames received whose N(S) does not
 * equal the receive state variable V(R) will be discarded.  An N(S) sequence error exception
 * condition occurs in the receiver when an I frame received contains an N(S) which is not equal to
 * the receive state variable V(R) at the receiver.  The receiver does not acknowledge (increment its
 * receive state varibale) the I frame causing the sequence error, or any I frame which may follow,
 * until an I frame with the correct N(S) is received.  A DCE or DTE which receives one or more valid
 * I frames having sequence errors or subsequent supervisory frames (RR, RNR, and REJ) shall accept
 * the control information contained in the N(R) field and the P or F bit to perform data link control
 * functions, e.g. to receive acknowledgement of previously transmitted I frames and to cause the DCE
 * or DTE to response (P bit set to 1). */
/*
 * REJ recovery: The REJ frame is used by a receiving DCE or DTE to intiate a recovery
 * (retransmission) following the detection of an N(S) sequence error.  With respect to each direction
 * of transission on the data link, only one "sent REJ" exception condition from a DCE or DTE, to a
 * DTE or DCE, is established at a time.  A "send REJ" exception condition is cleared when the
 * requested I frame is received.  A DCE or DTE receiving a REJ frame initiates sequential
 * (re-)transmission of I frames starting with the I frame indicated by the N(R) contained in the REJ
 * frame.  The retransmitted frames may contain an N(R) and a P bit that are updated from and
 * therefore different from, the ones contained in the originally transmitted I frames.
 */
/*
 * Time-out recovery: If a DCE or DTE, due to transmission error, does not receive (or receives and
 * discards) a single I frame or the last I frame(s) in a sequence of I frames, it will not detect an
 * N(S) sequence error condtion and, therefore, will not transmit a REJ frame.  The DTE or DCE which
 * transmitted the unacknowledged I frame(s) shall, following the completion of a system specified
 * time-out period, take appropriate recovery action to determine at which I frame retransmission must
 * begin.  The retransmitted frame(s) may contain an N(R) and a P bit that is updated from, and
 * therefore different from, the ones contained in the originally transmitted frame(s).
 */
/*
 * Receiving an I frame:  When the DCE is not in a busy condition and receives a valid I frame whose
 * send sequence number N(S) is equal to the DCE receive state variable V(R), the DCE will accept the
 * information field of this frame, increment by one its receive state variable V(R), and act as
 * follows:  a) If the DCE is still not in a busy condition: i) If an I frame is available for
 * transmission by the DCE, it may acknowledge the received I frame by setting N(R) in the control
 * field of the next transmitted I frame to the value of the DCE receive state variable V(R).
 * Alternatively, the DCE may acknowledge the received I frame by transmitting an RR frame with the
 * N(R) equal to the value of the DCE receive state variable V(R).  ii) If no I frame is available for
 * tranmissoin by the DCE, it will transmit an RR frame with N(R) equal to the value of the DCE
 * receive state variable V(R). b) If the DCE is now in a busy condition, it will transmit an RNR
 * frame with N(R) equal to the value of the DCE receive state variable V(R).  When the DCE is in a
 * busy condition, it may ignore the information field contained in any received I frame. */
/*
 * Reception of out of sequence I frames: When the DCE receives a valid I frame whose send sequence
 * number N(S) is incorrect, i.e. not equal to the current DCE receive state variable V(R), it will
 * discard the information field of the I frame and transmit a REJ frame with the N(R) set to one
 * higher than the N(S) of the last correctly received I frame.  The REJ frame will be a command frame
 * with the P bit set to 1 if an acknowledged transfer of the retransmission request is required;
 * otherwise, the REJ frame may be either a command or a response frame.  The DCE will then discard
 * the information field of all I frames received until the expected I frame is correctly received.
 * When receiving the expected I frame, the DCE will then acknowledge the I frame as normal. */

static int
dl_recv_i(queue_t *q, mblk_t *mp, uchar cmd, uchar pf, uchar ns, uchar nr)
{
}

static streams_inline streams_fastcall __hot_read int
dl_recv_msg_slow(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	uchar pf, ns, nr, ad, ml, ca, ra, de;
	int ext = !!dl->extended;
	int num = ((mp->b_rptr[1] & 0xC0) != 0xC0);
	int inf = ((mp->b_rptr[1] & 0x80) == 0x00);
	int size = msgdsize(mp);
	int cmd = 0;

	if (size < 2)
		goto discard;

	if ((num || inf) && size < 2 + ext)
		goto badsize;

	ad = mp->b_rptr[0];
	nr = num ? (ext ? (mp->b_rptr[2] & 0x7f) : (mp->b_rptr[1] & 0x07)) : 0;
	ns = inf ? (ext ? (mp->b_rptr[1] & 0x7f) : ((mp->b_rptr[1] & 0x70) >> 4)) : 0;
	pf = ((num && ext) ? (mp->b_rptr[2] >> 7) : (mp->b_rptr[1] >> 3)) & 0x01;
	ml = dl->mlp ? 0x02 : 0x01;
	de = dl->dce ? 0x08 : 0x04;

	/*- Addresses:
	 *  SLP:    A 0xC0  DCE->DTE command  DCE<-DTE response
	 *	    B 0x80  DTE->DCE command  DTE<-DCE response
	 *  MLP:    C 0xF0  DCE->DTE command  DCE<-DTE response
	 *	    D 0xE0  DTE->DCE command  DTE<-DCE response */

	if ((ad & 0x0f) != 0)
		goto discard;	/* bad address */

	switch ((ad | ml)) {
	case 0x89:		/* dce slp */
	case 0xC9:		/* dce slp */
		ca = 0x80;	/* address on which I receive commands */
		ra = 0xC0;	/* address on which I receive responses */
		break;
	case 0x85:		/* dte slp */
	case 0xC5:		/* dte slp */
		ca = 0xC0;	/* address on which I receive commands */
		ra = 0x80;	/* address on which I receive responses */
		break;
	case 0xEA:		/* dce mlp */
	case 0xFA:		/* dce mlp */
		ca = 0xE0;	/* address on which I receive commands */
		ra = 0xF0;	/* address on which I receive responses */
		break;
	case 0xE6:		/* dte mlp */
	case 0xF6:		/* dte mlp */
		ca = 0xF0;	/* address on which I receive commands */
		ra = 0xE0;	/* address on which I receive responses */
		break;
	default:
		goto discard;
	}

	switch (mp->b_rptr[1] & 0xC0) {
	case 0x80:		/* Supervisory */
		switch (mp->b_rptr[1] & 0x30) {
		case 0x00:	/* RR *//* command, reponse */
			cmd = (ad == ca) ? 1 : 0;
			if (size != 2 + ext)
				goto badsize;
			return dl_recv_rr(q, mp, cmd, pf, nr);
		case 0x20:	/* RNR *//* command, reponse */
			cmd = (ad == ca) ? 1 : 0;
			if (size != 2 + ext)
				goto badsize;
			return dl_recv_rnr(q, mp, cmd, pf, nr);
		case 0x10:	/* REJ *//* command, reponse */
			cmd = (ad == ca) ? 1 : 0;
			if (size != 2 + ext)
				goto badsize;
			return dl_recv_rej(q, mp, cmd, pf, nr);
		default:	/* error */
			goto unrecog;
		}
	case 0xC0:		/* Unnumbered */
		switch (mp->b_rptr[1] & 0x37) {
		case 0x34:	/* SABM *//* command */
			if (!(cmd = (ad == ca) ? 1 : 0))
				goto discard;
			if (size != 2)
				goto badsize;
			return dl_recv_sabm(q, mp, cmd, pf, nr);
		case 0x36:	/* SABME *//* command */
			if (!(cmd = (ad == ca) ? 1 : 0))
				goto discard;
			if (size != 2)
				goto badsize;
			return dl_recv_sabme(q, mp, cmd, pf, nr);
		case 0x02:	/* DISC *//* command */
			if (!(cmd = (ad == ca) ? 1 : 0))
				goto discard;
			if (size != 2)
				goto badsize;
			return dl_recv_disc(q, mp, cmd, pf, nr);
		case 0x30:	/* DM *//* response */
			if ((cmd = (ad == ca) ? 1 : 0))
				goto discard;
			if (size != 2)
				goto badsize;
			return dl_recv_dm(q, mp, cmd, pf, nr);
		case 0x06:	/* UA *//* response */
			if ((cmd = (ad == ca) ? 1 : 0))
				goto discard;
			if (size != 2)
				goto badsize;
			return dl_recv_ua(q, mp, cmd, pf, nr);
		case 0x21:	/* FRMR *//* response */
			if ((cmd = (ad == ca) ? 1 : 0))
				goto discard;
			if (size != 5 + ext) {
				goto badsize;
			}
			return dl_recv_frmr(q, mp, cmd, pf, nr);
		default:	/* error */
			goto unrecog;
		}
	default:		/* Information *//* command */
		if (!(cmd = (ad == ca) ? 1 : 0))
			goto discard;
		if (size > 2 + ext + dl->info.dl_max_sdu)
			goto toolong;
		return dl_recv_i(q, mp, cmd, pf, ns, nr);
	}
	/* Frame rejection condition: A frame rejection condition is established upon the receipt of
	 * an error-free frame with one of the conditions listed below.  At the DCE or DTE, this frame
	 * rejection exception condition is reported by an FRMR response for approprite DTE or DCE
	 * action, respectively.  Once a DCE has established such an exception condition, no
	 * additional I frames are accepted until the condition is reset by the DTE, except for
	 * examination of the P bit.  The FRMR response may be repeated at each opportunity, until
	 * recovery is effected by the DTE, or until the DCE initiates its own recovery in case the
	 * DTE does not respond. */
	{
		uchar flags;

	      badnr:
#define DL_REJ_FLAG_BADNR	0x10
		/* send a frmr for bad N(R) */
		flags = DL_REJ_FLAG_BADNR;
		goto error;
	      toolong:
		/* send a frmr for too long info frame */
#define DL_REJ_FLAG_TOOLONG	0x20
		flags = DL_REJ_FLAG_TOOLONG;
		goto error;
	      badsize:
		/* send a frmr for bad size has info or wrong size */
#define DL_REJ_FLAG_BADSIZE	0xC0
		flags = DL_REJ_FLAG_BADSIZE;
		goto error;
	      unrecog:
		/* send a frmr for unrecognized */
#define DL_REJ_FLAG_UNDEF	0x80
		flags = DL_REJ_FLAG_UNDEF;
		goto error;
	      error:
		/* send an frmr */
		return dl_send_frmr();
	}
      discard:
	/* Invalid frame condition: Any frame that is invalid will be discarded, and no action taken
	 * as a result of that frame.  An invalid frame is defined as one which: a) is not properly
	 * bounded by two flags; b) in basic (modulo 8) operation, contains fewer than 32 bits between
	 * flags; in extended (modulo 128) operation, contains fewer than 40 bits between flags of
	 * frames that contain sequence numbers or 32 bits between flags of frames that do not contain
	 * sequence numbers; c) or start/stop transmission, in addition to conditions listed in b),
	 * contains an octet-framing violoation (i.e. a 0 bit occurs where a stop bit is expected); d)
	 * contains a Frame Check Sequence (FCS) error; e) contains an address other than A or B (for
	 * single link operation) or other than C or D (for multilink operation); or, f) frame
	 * aborted: in synchronous transmission, a frame is aborted when it contains at least seven
	 * contiguous 1 bits (with no inserted 0 bit); in start/stop transmission, a frame is aborted
	 * when it contains the two-octet sequence composed fo the control escape octet followed by a
	 * closing flag.  For those networks that are octet aligned, a detection of non-octet
	 * alignment may be made at the Data Link Layer by adding a frame validity check that requires
	 * the number of bits between the opening flag and the closing flag, excluding inserted bits
	 * (for transparency or for transmission timing for start/stop transmission), to be an
	 * integral number of octets in length, or the frame is considered invalid. */

	freemsg(mp);
	return (0);
}

/*
 *  DLPI PRIMITIVE HANDLING
 *  =======================
 */
static void
bufqenable(queue_t *q, size_t size, int priority)
{
}

static streams_noinline streams_fastcall int
dl_data_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	size_t size;

	if ((size = msgdsize(mp)) < 1 || size < dl->info.dl_min_sdu || size > dl->info.dl_max_sdu) {
		/* error */
		putnextctl2(OTHERQ(q), M_ERROR, EPROTO, EPROTO);
		freemsg(mp);
		return (0);
	}
	if (dl->i_state == DL_IDLE || dl->i_state == DL_PROV_RESET_PENDING) {
		/* silent discard */
		freemsg(mp);
		return (0);
	}
	if (dl->i_state != DL_DATAXFER) {
		/* error */
		putnextctl2(OTHERQ(q), M_ERROR, EPROTO, EPROTO);
		freemsg(mp);
		return (0);
	}
}
static streams_noinline streams_fastcall int
dl_info_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_attach_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_detach_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_bind_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_unbind_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_subs_bind_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_subs_unbind_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_enabmulti_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_disabmulti_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_promiscon_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_promiscoff_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_connect_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_token_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_disconnect_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_reset_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_reset_res(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_unitdata_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_udqos_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_test_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_test_res(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_xid_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_xid_res(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_data_ack_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_reply_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_reply_update_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_phys_addr_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_set_phys_addr_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_get_statistics_req(queue_t *q, mblk_t *mp)
{
}
static streams_noinline streams_fastcall int
dl_other_req(queue_t *q, mblk_t *mp)
{
	return dl_error_ack(q, DL_PRIV(q), *(uint32_t *) mp->b_rptr, DL_BADPRIM, EOPNOTSUPP);
}

static streams_noinline streams_fastcall int
lapb_w_data(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	int rtn;

	dl->i_oldstate = dl->i_state;
	if (!msgdsize(mp)) {
		__swerr();
		freemsg(mp);
		return (0);
	}
	printd(("%s: %p: -> DL_DATA_REQ\n", DRV_NAME, dl));
	rtn = dl_data_req(q, mp);
	if (rtn)
		dl->i_state = dl->i_oldstate;
	return (rtn);
}

static streams_noinline streams_fastcall int
lapb_w_proto(queue_t *q, mblk_t *mp)
{
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(uint32_t))) {
		__swerr();
		freemsg(mp);
		return (0);
	}
	switch (*(uint32_t *) mp->b_rptr) {
	case DL_INFO_REQ:
		printd(("%s: %p: -> DL_INFO_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_info_req(q, mp);
	case DL_ATTACH_REQ:
		printd(("%s: %p: -> DL_ATTACH_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_attach_req(q, mp);
	case DL_DETACH_REQ:
		printd(("%s: %p: -> DL_DETACH_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_detach_req(q, mp);
	case DL_BIND_REQ:
		printd(("%s: %p: -> DL_BIND_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_bind_req(q, mp);
	case DL_UNBIND_REQ:
		printd(("%s: %p: -> DL_UNBIND_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_unbind_req(q, mp);
	case DL_SUBS_BIND_REQ:
		printd(("%s: %p: -> DL_SUBS_BIND_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_subs_bind_req(q, mp);
	case DL_SUBS_UNBIND_REQ:
		printd(("%s: %p: -> DL_SUBS_UNBIND_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_subs_unbind_req(q, mp);
	case DL_ENABMULTI_REQ:
		printd(("%s: %p: -> DL_ENABMULTI_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_enabmulti_req(q, mp);
	case DL_DISABMULTI_REQ:
		printd(("%s: %p: -> DL_DISABMULTI_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_disabmulti_req(q, mp);
	case DL_PROMISCON_REQ:
		printd(("%s: %p: -> DL_PROMISCON_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_promiscon_req(q, mp);
	case DL_PROMISCOFF_REQ:
		printd(("%s: %p: -> DL_PROMISCOFF_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_promiscoff_req(q, mp);
	case DL_CONNECT_REQ:
		printd(("%s: %p: -> DL_CONNECT_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_connect_req(q, mp);
	case DL_TOKEN_REQ:
		printd(("%s: %p: -> DL_TOKEN_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_token_req(q, mp);
	case DL_DISCONNECT_REQ:
		printd(("%s: %p: -> DL_DISCONNECT_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_disconnect_req(q, mp);
	case DL_RESET_REQ:
		printd(("%s: %p: -> DL_RESET_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_reset_req(q, mp);
	case DL_RESET_RES:
		printd(("%s: %p: -> DL_RESET_RES\n", DRV_NAME, DL_PRIV(q)));
		return dl_reset_res(q, mp);
	case DL_UNITDATA_REQ:
		printd(("%s: %p: -> DL_UNITDATA_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_unitdata_req(q, mp);
	case DL_UDQOS_REQ:
		printd(("%s: %p: -> DL_UDQOS_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_udqos_req(q, mp);
	case DL_TEST_REQ:
		printd(("%s: %p: -> DL_TEST_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_test_req(q, mp);
	case DL_TEST_RES:
		printd(("%s: %p: -> DL_TEST_RES\n", DRV_NAME, DL_PRIV(q)));
		return dl_test_res(q, mp);
	case DL_XID_REQ:
		printd(("%s: %p: -> DL_XID_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_xid_req(q, mp);
	case DL_XID_RES:
		printd(("%s: %p: -> DL_XID_RES\n", DRV_NAME, DL_PRIV(q)));
		return dl_xid_res(q, mp);
	case DL_DATA_ACK_REQ:
		printd(("%s: %p: -> DL_DATA_ACK_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_data_ack_req(q, mp);
	case DL_REPLY_REQ:
		printd(("%s: %p: -> DL_REPLY_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_reply_req(q, mp);
	case DL_REPLY_UPDATE_REQ:
		printd(("%s: %p: -> DL_REPLY_UPDATE_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_reply_update_req(q, mp);
	case DL_PHYS_ADDR_REQ:
		printd(("%s: %p: -> DL_PHYS_ADDR_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_phys_addr_req(q, mp);
	case DL_SET_PHYS_ADDR_REQ:
		printd(("%s: %p: -> DL_SET_PHYS_ADDR_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_set_phys_addr_req(q, mp);
	case DL_GET_STATISTICS_REQ:
		printd(("%s: %p: -> DL_GET_STATISTICS_REQ\n", DRV_NAME, DL_PRIV(q)));
		return dl_get_statistics_req(q, mp);
	default:
		printd(("%s: %p: -> DL_?? %u ??\n", DRV_NAME, DL_PRIV(q),
			*(uint32_t *) mp->b_rptr));
		return dl_other_req(q, mp);
	}
}

static streams_noinline streams_fastcall __unlikely int
lapb_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHALL);
		else
			flushq(q, FLUSHALL);
	}
	{
		struct dl *dl = DL_PRIV(q);
		queue_t *oq;

		if ((oq = q->q_next) || (oq = dl->hdlc ? dl->hdlc->wq : NULL)) {
			put(oq, mp);
			return (0);
		}
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(RD(q), mp->b_rptr[1], FLUSHALL);
		else
			flushq(RD(q), FLUSHALL);
		mp->b_rptr[0] &= ~FLUSHW;
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

static streams_noinline streams_fastcall __unlikely int
lapb_wmsg_slow(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return lapb_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return lapb_w_proto(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return lapb_w_ioctl(q, mp);
	case M_FLUSH:
		return lapb_w_flush(q, mp);
	default:
		return lapb_w_other(q, mp);
	}
}

static streams_inline streams_fastcall __hot_write int
lapb_wmsg(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	uchar type;

	/* optimized for data */
	if ((type = mp->b_datap->db_type) == M_DATA) {
		queue_t *oq;

		/* just pass it */
		if ((oq = q->q_next) || (oq = dl->hdlc ? dl->hdlc->wq : NULL)) {
			if (bcanput(oq, mp->b_band)) {
				put(oq, mp);
				return (0);
			}
			return (-EBUSY);
		}
		/* in the wrong state, go slow */
	}
	if (type == M_PROTO || type = M_PCPROTO) {
		union DL_primitives *dl = (typeof(dl)) mp->b_rptr;

		dassert(mp->b_wptr >= mp->b_rptr + sizeof(dl->dl_primitive));
		if (dl->dl_primitive == DL_UNITDATA_REQ && mp->b_cont && msgdsize(mp->b_cont)) {
			queue_t *oq;

			/* just pass the data */
			if ((oq = q->q_next) || (oq = dl->hdlc ? dl->hdlc->wq : NULL)) {
				uchar band = mp->b_band;

				if (bcanput(oq, band)) {
					/* strip the M_(PC)PROTO */
					freeb(XCHG(&mp, mp->b_cont));
					mp->b_band = band;
					putnext(q, mp);
					return (0);
				}
				return (-EBUSY);
			}
			/* in the wrong state, go slow */
		}
	}
	return lapb_wmsg_slow(q, mp);
}

/*
 *  CDI PRIMITIVE HANDLING
 *  ======================
 */
static streams_noinline streams_fastcall __unlikely int
lapb_rmsg_slow(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return lapb_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return lapb_r_proto(q, mp);
	case M_FLUSH:
		return lapb_r_flush(q, mp);
	case M_HANGUP:
		return lapb_r_hangup(q, mp);
	case M_ERROR:
		return lapb_r_error(q, mp);
	case M_SETOPTS:
		return labp_r_setopts(q, mp);
	default:
		return lapb_r_other(q, mp);
	}
}

static streams_inline streams_fastcall __hot_read int
lapb_rmsg(queue_t *q, mblk_t *mp)
{
	uchar type;

	/* optimized for data */
	if ((type = mp->b_datap->db_type) == M_DATA) {
		/* just pass it */
		if (bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
	if (type == M_PROTO || type == M_PCPROTO) {
		union CD_primitives *cd = (typeof(cd)) mp->b_rptr;

		dassert(mp->b_wptr >= mp->b_rptr + sizeof(cd->cd_primitive));
		if (cd->cd_primitive == CD_UNITDATA_IND && mp->b_cont && msgdsize(mp->b_cont)) {
			uchar band = mp->b_band;

			/* just pass the data */
			if (bcanputnext(q, band)) {
				/* strip the M_(PC)PROTO */
				freeb(XCHG(&mp, mp->b_cont));
				mp->b_band = band;
				putnext(q, mp);
				return (0);
			}
			return (-EBUSY);
		}
	}
	return lapb_rmsg_slow(q, mp);
}

/*
 *  PUT and SERVICE procedures.
 *  ===========================
 */
STATIC streamscall int
lapb_rput(queue_t *q, mblk_t *mp)
{
	/* This must only be called if we are pushed as a module */
	assert(backq(q) != NULL);

	/* note that an M_FLUSH will pass through to lapb_rmsg() */
	if ((mp->b_datap->db_type < QPCTL && (q->q_flag & QSVCBUSY))
	    || lapb_rmsg(q, mp)) {
		if (putq(q, mp))
			return (0);
		__swerr();
		freemsg(mp);
	}
	return (0);
}

STATIC streamscall int
lapb_rsrv(queue_t *q)
{
	mblk_t *mp;

	/* this part runs if we were pushed as a module */
	while ((mp = getq(q))) {
		if (lapb_rmsg(q, mp)) {
			dassert(mp->b_datap->db_type < QPCTL);
			if (putbq(q, mp))
				break;
			__swerr();
			freemsg(mp);
			continue;
		}
	}
	/* this part runs if we are opened as a driver */
	if (mp == NULL) {
		/* processed all messages on queue, back-enable across the mux */
		struct dl *dl = DL_PRIV(q);

		if (dl->hdlc && (q = dl->hdlc->rq)) {
			enableok(q);
			qenable(q);
		}
	}
	return (0);
}

STATIC streamscall int
lapb_wput(queue_t *q, mblk_t *mp)
{
	/* note that an M_FLUSH will pass through to lapb_wmsg(), all others will queue if
	   necessary. */
	if ((mp->b_datap->db_type != M_FLUSH && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || lapb_wmsg(q, mp)) {
		if (putq(q, mp))
			return (0);
		__swerr();
		freemsg(mp);
	}
	return (0);
}

STATIC streamscall int
lapb_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (lapb_wmsg(q, mp)) {
			dassert(mp->b_datap->db_type < QPCTL);
			if (putbq(q, mp))
				break;
			__swerr();
			freemsg(mp);
		}
	}
	return (0);
}

STATIC streamscall int
hdlc_rput(queue_t *q, mblk_t *mp)
{
	/* note that M_FLUSH will pass through to hdlc_rmsg(), all others will queue if necessary. */
	if ((mp->b_datap->db_type != M_FLUSH && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || hdlc_rmsg(q, mp)) {
		if (putq(q, mp))
			return (0);
		__swerr();
		freemsg(mp);
	}
	return (0);
}

STATIC streamscall int
hdlc_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (hdlc_rmsg(q, mp)) {
			dassert(mp->b_datap->db_type < QPCTL);
			if (putbq(q, mp))
				break;
			__swerr();
			freemsg(mp);
		}
	}
	return (0);
}

#if 0
/* Pass everything along, we never write to this queue anyway. */
STATIC streamscall int
hdlc_wput(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}
#endif

/* Only gets invoked when back-enabling. */
STATIC streamscall int
hdlc_wsrv(queue_t *q)
{
	struct cd *cd = CD_PRIV(q);

	if (cd->lapd && (q = cd->lapd->wq)) {
		/* back-enable across the mux */
		enableok(q);
		qenable(q);
	}
}


/*
 *  OPEN and CLOSE routines.
 *  ========================
 */
STATIC streamscall int
lapb_open(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct dl *dl, **dlp;
	unsigned long flags;
	mblk_t *mp;
	struct stroptions *so;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	if ((mp = allocb(sizeof(*so), BPRI_MED)) == NULL)
		return (ENOBUFS);
	if (sflag == MODOPEN || WR(q)->q_next) {
		/* pushed as module */
		dlp = &master.mods.list;

		write_lock_str(&master.lock, flags);
		dl = lapb_alloc_priv(q, dlp, devp, crp);
		write_unlock_str(&master.lock, flags);
	} else {
		/* opened as driver */
		/* Linux Fast-STREAMS always passes internal major device number (module id).  Note 
		   also, however, that strconf-sh attempts to allocate module ids that are
		   identical to the abse major device number anyway. */
		dlp = &master.lapb.list;
#ifdef LIS
		/* sorry, cannot open by minor device number */
		if (cmajor != CMAJOR_0 || cminor != 0) {
			ptrace(("%s: ERROR: cannot open specific minor\n", DRV_NAME));
			freeb(mp);
			return (ENXIO);
		}
#endif				/* LIS */
#ifdef LFS
		/* sorry, cannot open by minor device number */
		if (cmajor != DRV_ID || cminor != 0) {
			ptrace(("%s: ERROR: cannot open specific minor\n", DRV_NAME));
			freeb(mp);
			return (ENXIO);
		}
#endif				/* LFS */
		cminor = 1;
		write_lock_str(&master.lock, flags);
		for (; *dlp; dlp = &(*dlp)->next) {
			if (cmajor != (*dlp)->u.dev.cmajor)
				break;
			if (cmajor == (*dlp)->u.dev.cmajor) {
				if (cminor < (*dlp)->u.dev.cminor) {
					if (++cminor >= NMINORS) {
						if (++mindex >= CMAJORS
						    || !(cmajor = lapb_majors[mindex]))
							break;
						cminor = 0;
					}
					continue;
				}
			}
		}
		if (mindex >= CMAJORS || !cmajor) {
			ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
			write_unlock_str(&master.lock, flags);
			freeb(mp);
			return (ENXIO);
		}
		_printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
		*devp = makedevice(cmajor, cminor);
		dl = lapb_alloc_priv(q, dlp, devp, crp);
		write_unlock_str(&master.lock, flags);
	}
	if (dl == NULL) {
		ptrace(("%s: ERROR: no memory\n", DRV_NAME));
		freeb(mp);
		return (ENOMEM);
	}
	so = (typeof(so)) mp->b_wptr;
#ifdef LFS
	so->so_flags |= SO_SKBUFF;
#endif				/* LFS */
	so->so_flags |= SO_WROFF;
	so->so_wroff = 12;
	/* Not really necessary for modules. */
	so->so_flags |= SO_MINPSZ;
	so->so_minpsz = lapb_minfo.mi_minpsz;
	so->so_flags |= SO_MAXPSZ;
	so->so_maxpsz = lapb_minfo.mi_maxpsz;
	so->so_flags |= SO_HIWAT;
	so->so_hiwat = lapb_minfo.mi_hiwat;
	so->so_flags |= SO_LOWAT;
	so->so_lowat = lapb_minfo.mi_lowat;
	mp->b_wptr += sizeof(*so);
	mp->b_datap->db_type = M_SETOPTS;
	putnext(q, mp);
	qprocson(q);
	return (0);
}
STATIC streamscall int
lapb_close(queue_t *q, dev_t *devp, int oflags)
{
	struct dl *dl = DL_PRIV(q);

	_printd(("%s: closing character device %d:%d\n", DRV_NAME, dl->u.dev.cmajor,
		 dl->u.dev.cminor));
#ifdef LIS
	/* protect agains LIS bugs */
	if (q->q_ptr == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS double-close bug detected.", DRV_NAME, __FUNCTION__);
		return (0);
	}
	if (q->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			DRV_NAME, __FUNCTION__);
	}
	/* make sure procedures are off */
	qprocsoff(q);
	lpab_free_priv(q);	/* unlink and free structure */
	return (0);
}

/*
 *  STREAMS Definitions
 *  ===================
 *
 *  Upper Multiplex.
 *  ----------------
 *  Provides a DLPI interface at the upper multiplex.  This can also be pushed
 *  as a module that presents a DLPI upper service interface.
 */

static struct module_stat lapb_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat lapb_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit lapb_rinit = {
	.qi_putp = lapb_rput,
	.qi_srvp = lapb_rsrv,
	.qi_qopen = lapb_open,
	.qi_qclose = lapb_close,
	.qi_minfo = &lapb_minfo,
	.qi_mstat = &lapb_rstat,
};
static struct qinit lapb_winit = {
	.qi_putp = lapb_wput,
	.qi_srvp = lapb_wsrv,
	.qi_minfo = &lapb_minfo,
	.qi_mstat = &lapb_wstat,
};

/*
 *  Lower Multiplex.
 *  ----------------
 *  Uses a CDI Interface at the lower multiplex.  This can also be pushed as a
 *  module that uses the CDI lower service interface.
 */

static struct module_stat hdlc_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat hdlc_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit hdlc_rinit = {
	.qi_putp = hdlc_rput,
	.qi_srvp = hdlc_rsrv,
	.qi_minfo = &lapb_minfo,
	.qi_mstat = &hdlc_rstat,
};
static struct qinit hdlc_winit = {
#if 0
	.qi_putp = hdlc_wput,		/* nulled out to cause a crash if it is invoked */
#endif
	.qi_srvp = hdlc_wsrv,
	.qi_minfo = &lapb_minfo,
	.qi_mstat = &hdlc_wstat,
};

MODULE_STATIC struct streamtab lapbinfo = {
	.st_rdinit = &lapb_rinit,
	.st_wrinit = &labp_winit,
	.st_muxrinit = &hdlc_rinit,
	.st_muxwinit = &hdlc_winit,
};

/*
 *  STREAMS Registration.
 *  =====================
 */
#ifdef LIS
#define fmodsw _fmodsw
#endif

modID_t modid = MOD_ID;
major_t major = CMAJOR_0 STATIC struct fmodsw lapb_fmod = {
	.f_name = "lapbmod",
	.f_str = &lapbinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

STATIC struct cdevsw lapb_cdev = {
	.d_name = DRV_NAME,
	.d_str = &lapbinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

static int lapb_module_registered = 0;

STATIC __unlikely int
lapb_register_strmod(void)
{
	int err;

	if (lapb_module_registered == 0) {
		if ((err = register_strmod(&lapbfmod)) < 0)
			return (err);
		lapb_module_registered = 1;
	}
	return (0);
}

STATIC __unlikely int
lapb_unregister_strmod(void)
{
	int err;

	if (lapb_module_registered == 1) {
		if ((err = unregister_strmod(&lapbfmod)) < 0)
			return (err);
		lapb_module_registered = 0;
	}
	return (0);
}

STATIC __unlikely int
lapb_register_strdev(major_t major)
{
	int err;

	if ((err = regsiter_strdev(&lapbcdev, major)) < 0)
		return (err);
	return (0);
}

STATIC __unlikely int
lapb_unregsiter_strdev(major_t major)
{
	int err;

	if (major && (err = unregister_strdev(&lapbcdev, major)) < 0)
		return (err);
	return (0);
}

/*
 *  Linux Registration.
 *  ===================
 */
#ifdef module_param
module_param(modid, ushort, 0);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for the LAPB driver. (0 for allocation.)");

#ifdef module_param
module_param(major, uint, 0);
#else				/* module_param */
MODULE_PARM(major, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Device number for the LAPB driver. (0 for allocation.)");

MODULE_STATIC void __exit
lapbterminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (lapb_majors[mindex]) {
			if ((err = lapb_unregsiter_strdev(lapb_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregsiter major %d", DRV_NAME,
					lapb_majors[mindex]);
			if (mindex)
				lapb_majors[mindex] = 0;
		}
	}
	if ((err = lapb_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", DRV_NAME);
	if ((err = lapb_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	lapb_term_hashes();
	return;
}

MODULE_STATIC int __init
lapbinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	lapb_init_hashes();
	if ((err = lapb_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		lapbterminate();
		return (err);
	}
	if ((err = lapb_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", DRV_NAME, err);
		lapbterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = lapb_register_strdev(lapb_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					lapb_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				lapbterminate();
				return (err);
			}
		}
		if (lapb_majors[mindex] == 0)
			lapb_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(lapb_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = lapb_majors[0];
	}
	return (0);
}

module_init(lapbinit);
module_exit(lapbterminate);
