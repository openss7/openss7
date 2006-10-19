/*****************************************************************************

 @(#) $RCSfile: v401p.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/19 10:37:24 $

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

 Last Modified $Date: 2006/10/19 10:37:24 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: v401p.c,v $
 Revision 0.9.2.2  2006/10/19 10:37:24  brian
 - working up drivers and modules

 Revision 0.9.2.1  2006/10/14 06:37:27  brian
 - added manpages, module, drivers, headers from strss7 package

 *****************************************************************************/

#ident "@(#) $RCSfile: v401p.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/19 10:37:24 $"

static char const ident[] =
    "$RCSfile: v401p.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/19 10:37:24 $";

/*
 * This is an MX (Multiplex) STREAMS device driver for the Varion V401P card.
 * When Stream is opened on this driver it is a Multiplex (MX) Stream.
 * Multiplex (MX) Streams represent an entire span and provide timeslotting
 * of channels with in the data Stream.  MX Streams are bound to a specific
 * span.  The data stream is not demultiplexed by the driver and is
 * unswitched.  MX Streams are suitable for linking under an MX (Matrix) or
 * MG (Media Gateway) multiplexing driver.  The MX OR MG driver performs
 * Time-Space-Time switching as required.
 *
 * This is deviation from previous approaches for providing SS7 on the X400P
 * cards.  When an MX Stream for each span of a card is linked under an Matrix
 * driver, the user can then open a Channel (CH) Stream on the matrix, bind it
 * to any specific channel under the realm of the matrix switch, and have a
 * clear-channel data path for use in SS7, LAPB, LAPD, LAPF, whatever.  An SS7
 * SDL module is then pushed over the CH Stream to provide SS7 DAEDR/DAEDT
 * AERM/SUERM functions, and an SDT and SL module can be pushed from there to
 * form a complete MTP2 Signalling Link.
 *
 * With this approach, the Matrix switch can perform switching of channels
 * with codec (mu-law/A-law) conversion, digital padding, echo suppresion and
 * the like.  Switching is high performance regardless of it being done in
 * software.  We can also take the MX (Mulitplex) group blocks and pass them
 * over some network (PW3E, VLAN) without much manipulation at all.  Circuit
 * associated signalling information (if any) is passed with the MX blocks and
 * can be processed by MF or R2 controllers in the Matrix or beyond.
 *
 * A management Stream is provided on this device driver to perform management
 * functions.
 */

/*
 * Considerations for performance:  For performance purposes we do not want to
 * move bytes about in a received TDM frame.  We want to work with it exactly
 * as the hardware delivers it to us and in-place in the elastic buffers.
 *
 * Also, on a TDM barrier, we want to perform processing to collect all
 * timeslots for the next frame transmission.
 *
 * To accomplish this efficiently we need a pull architecutre instead of a
 * push architecture.  A pull architecture is one where TDM frames are
 * "pulled" from the transmitter rather than being "pushed" toward it.  Or a
 * more synchronous fashion.
 *
 * MX streams running in sycnrhonous fashion deliver an allocb'ed and
 * possibly duped mblk that references the transmit buffer upstream to
 * producers that fill out their portion of the transmit block and then free
 * the buffer.  When all references to the buffer are freed, the block is
 * complete and ready for transmission.  On the receive side, an allocb'ed
 * and possibly duped bmblk that references the receive block is sent
 * upstream to the consumers that read their portion of the receive block and
 * then free the buffer.  Once the last reference is released, the block is
 * freed.  The receive direction is loose in the sense that late processes
 * have their receive information still available.  The transmit side is tight
 * in the sense that all processes must complete their portion of the block
 * before the block can be sent and the transmitter must either wait for the
 * last producer to complete its portion of the block before transmission or
 * the trasnmitter must underrun for late producers.  The transmit side could
 * be loosened by leading slow producers by a number of time frames.  The
 * trasnmit side could consider delay and jitter characteristics of producers.
 *
 * This transmit side manipulation could be accomplished outside of the
 * driver.  An MX stream that simply produces single receive frames and
 * accepts single transmit frames could suffice.
 *
 * Another possibility is timestamping trasnmit blocks with the tick clock
 * time at which the block will be trasnmitted.  Then producers can detect
 * their own trasnmit underruns by comparing the timing mark on the trasnmit
 * block with the current time and dropping trasnmit blocks upon which it is
 * late and thus delaying itself without affecting other producers.  That is
 * the bets model I think.  So what the transmitter does is create N frames
 * from the current frame ahead in time and timestamps them all.  N times the
 * frame delay is maximum delay that can be accomodated by the system.
 *
 * Received trasnmit blocks are queued.  In the producer, when a receive block
 * is received, the different between the timing mark and the current time
 * gives an indication of the delay on the receive side downstream.  The
 * producer can either queue the receive frame (if processing is compute
 * intensive) or process it immediately.  After deferring or processing k
 * receive blocks, the transmit section runs with the objective of producing
 * k transmit blocks.  (k can be 1).  Any queued transmit blocks that have
 * expired when the transmit section runs represent a transmitter underrun and
 * the producer can act on the underrun in any fashion defined by the media
 * (e.g. for HDLC, the current frame can be aborted and retranmission of the
 * current frame can begin).  If insufficient trasnmit blocks bave been
 * queued; that is, there are less than k transmit blocks queued, the
 * trasnmitter is not producing enough transmit blocks for the delay and the
 * producer can send a tx block request downstream requesting that N be
 * increased by (k - n) where n is the number of blocks available.  The
 * producer can consider this as an underrun condition and leave the n
 * unexpired transmit blocks queued.  If there are k trasnmit blocks avilable,
 * the transmit section completes the k blocks and then frees them.  The
 * transmit section can keep track of the number of blocks that it has
 * underrun, say 'm'.  This number, 'm' times the block delay is the delay on
 * the transmit side.  The delay on the receive side above plus this delay on
 * the transmit side is the overrall delay.
 *
 * The driver generates N transmit blocks and places a timing mark in them
 * and passes them upstream.  Whenever a transmit block is transmitted, the
 * number of underruns can be detected by checking the reference count on the
 * mblk.  If the reference count is 1, no underrun occured.  If no underruns
 * occured, the transmit block can be recycled upstream to keep N outstanding
 * transmit blocks.  If an underrun occured, the transmit block can be queued
 * for later recycling and a new transmit block must be acquired.  The
 * recycling queue can be checked for a recylable transmit block (the one at
 * the head of the recycling queue has a db_ref reference count of 1) and that
 * block used, or a new block must be allocated with allocb().
 *
 * If the driver receives a request for additional transmit blocks, it expands
 * the number N by the request and furhishes that many transmit blocks, either
 * from the recycle queue or by allocating fresh blocks.
 *
 * A similary flexible buffer allocation strategy can be used on the receive
 * side.  Whenever a block is received a new block must be made available for
 * the next cycle.  Blocks can be allocated an duped sending one copy upstream
 * and holding a copy in a local recycling queue.  If the block at the head of
 * the queue has a reference count (db_ref) of 1, then all receivers have
 * disposed of the block and the block can be considered available for the
 * next cycle.  Otherwise, a fresh block must be allocated and placed at the
 * head of the queue.  When the receiver goes to receive a block it simply
 * uses the block at the head of the queue and runs the cycle again.  After a
 * short training period, the recycle queue will be long enough that the
 * receiver simply takes one off the head of the recycle queue, fills it and
 * then places it at the end of the recycle queue.  Queueing is not really
 * necessary, this can simply be a descriptor ring.
 *
 * Probably the best for performance is to provide CD raw, hdlc and ss7
 * device streams directly in the driver and MX device streams as well.  The
 * CD streams will drop and insert from within the MX (span) streams.  MX
 * streams can be linked under an MX or MG driver for switchiing, or under a
 * Zaptel driver for zaptel.
 */

#define _LFS_SOURCE
#define _MPS_SOURCE

#include <sys/os8/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitopts.h>

#define vp_tst_bit(nr, addr) test_bit(nr,addr)
#define vp_set_bit(nr, addr) __set_bit(nr,addr)
#define vp_clr_bit(nr, addr) __clear_bit(nr,addr)

#endif				/* LINUX */

#include <sys/cdi.h>
#include <sys/mxi.h>

#define VP_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define VP_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define VP_COPYRIGHT	"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define VP_REVISION	"OpenSS7 $RCSfile: v401p.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/19 10:37:24 $"
#define VP_DEVICE	"SVR 4.2 STREAMS V401P Driver"
#define VP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define VP_LICENSE	"GPL"
#define VP_BANNER	VP_DESCRIP	"\n" \
			VP_EXTRA	"\n" \
			VP_REVISION	"\n" \
			VP_COPYRIGHT	"\n" \
			VP_DEVCIE	"\n" \
			VP_CONTACT	"\n"
#define VP_SPLASH	VP_DESCRIP	" - " \
			VP_REVISION

#ifdef LINUX
MODULE_AUTHOR(VP_CONTACT);
MODULE_DESCRIPTION(VP_DESCRIP);
MODULE_SUPPORTED_DEVICE(VP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(VP_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-v401p");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define VP_DRV_ID	CONFIG_STREAMS_V401P_MODID
#define VP_DRV_NAME	CONFIG_STREAMS_V401P_NAME
#define VP_CMAJORS	CONFIG_STREAMS_V401P_NMAJORS
#define VP_CMAJOR_0	CONFIG_STREAMS_V401P_MAJOR
#define VP_UNITS	CONFIG_STREAMS_V401P_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_V401P_MODID));
MODULE_ALIAS("streams-driver-v401p");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_V401P_MAJOR));
MODULE_ALIAS("/dev/streams/v401p");
MODULE_ALIAS("/dev/streams/v401p/*");
MODULE_ALIAS("/dev/streams/clone/v401p");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(VP_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(VP_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(VP_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/v401p");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#define DRV_ID		VP_DRV_ID
#define DRV_NAME	VP_DRV_NAME
#define CMAJORS		VP_CMAJORS
#define CMAJOR_0	VP_CMAJOR_0
#define UNITS		VP_UNITS
#ifdef MODULE
#define DRV_BANNER	VP_BANNER
#else				/* MODULE */
#define DRV_BANNER	VP_SPLASH
#endif				/* MODULE */

static struct module_info vp_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static streamscall void
vp_bufcall(long arg)
{
	enableok((queue_t *) arg);
	qenable((queue_t *) arg);
}

static streams_fastcall __hot mblk_t *
vp_allocb(struct vp *vp, queue_t *q, mblk_t *mp, size_t size, int pri)
{
	mblk_t *mp;

	if ((mp = allocb(size, pri)))
		return (mp);
	if (q == vp->rq) {
		unbufcall(XCHG(&vp->rbid, bufcall(size, pri, &vp_bufcall, (long) q)));
	}
	if (q == vp->wq) {
		unbufcall(XCHG(&vp->wbid, bufcall(size, pri, &vp_bufcall, (long) q)));
	}
	return (mp);
}

/*
 *  CD Primitives sent upstream
 */

/**
 * cd_error_ack: - issue a CD_ERROR_ACK primitive
 * @vp: private structure
 * @q: active queue
 * @msg: message to free on success
 * @prim: primitive in error
 * @error: CDI error number
 * @errno: UNIX error number
 */
static streams_fastcall streams_noinline __unlikely int
cd_error_ack(struct vp *vp, queue_t *q, mblk_t *msg, cd_ulong prim, cd_ulong error, cd_ulong errno)
{
	cd_error_ack_t *p;
	mblk_t *mp;

	if ((mp = vp_allocb(vp, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_ERROR_ACK;
		p->cd_state = vp->i_state;
		p->cd_error_primitive = prim;
		p->cd_errno = error;
		p->cd_explanation = errno;
		freemsg(msg);
		putnext(vp->rq, mp);
		return (0);
	}
	return (ENOBUFS);
}

/**
 * cd_ok_ack: - issue a CD_OK_ACK primitive
 * @vp: private structure
 * @q: active queue
 * @msg: the primitive to free on success
 */
static streams_fastcall int
cd_ok_ack(struct vp *vp, queue_t *q, mblk_t *msg)
{
	cd_ok_ack_t *p;
	mblk_t *mp;

	if ((mp = vp_allocb(vp, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_OK_ACK;
		switch ((p->cd_correct_primitive = *(cd_ulong *) msg->b_rptr)) {
		case CD_ATTACH_REQ:
			assert(cd_get_state(vp) == CD_UNUSABLE);
			p->cd_state = CD_DISABLED;
			cd_set_state(vp, p->cd_state);
			break;
		case CD_DETACH_REQ:
			assert(cd_get_state(vp) == CD_UNUSABLE);
			p->cd_state = CD_UNATTACHED;
			cd_set_state(vp, p->cd_state);
			break;
		case CD_ALLOW_INPUT_REQ:
			p->cd_state = CD_INPUT_ALLOWED;
			break;
		case CD_READ_REQ:
			p->cd_state = CD_INPUT_ALLOWED;
			break;
		case CD_HALT_INPUT_REQ:
			p->cd_state = CD_ENABLED;
			break;
		case CD_ABORT_OUTPUT_REQ:
			p->cd_state = cd_get_state(vp);
			break;
		default:
			never();
			p->cd_state = CD_UNUSABLE;
			cd_set_state(vp, p->cd_state);
			break;
		}
		freemsg(msg);
		putnext(vp->rq, mp);
		return (0);
	}
	return (ENOBUFS);
}

/**
 * cd_info_ack: - issue a CD_INFO_ACK primitive
 * @vp: private structure
 * @q: active queue
 * @msg: message to free on success
 */
static streams_fastcall streams_noinline int
cd_info_ack(struct vp *vp, queue_t *q, mblk_t *msg)
{
	cd_info_ack_t *p;
	mblk_t *mp;

	if ((mp = vp_allocb(vp, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		*p = vp->u.cd.info;
		freemsg(msg);
		putnext(vp->rq, mp);
		return (0);
	}
	return (ENOBUFS);
}

/**
 * cd_enable_con: - issue a CD_ENABLE_CON primitive
 * @vp: private structure
 * @q: active queue
 * @msg: message to free on success
 */
static streams_fastcall streams_inline
cd_enable_con(struct vp *vp, queue_t *q, mblk_t *msg)
{
	cd_enable_con_t *p;
	mblk_t *mp;

	if ((mp = vp_allocb(vp, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_ENABLE_CON;
		assert(cd_get_state(vp) == CD_ENABLE_PENDING);
		if (vp->u.cd.info.cd_features & CD_AUTOALLOW) {
			p->cd_state = CD_INPUT_ALLOWED;
			cd_set_state(vp, CD_INPUT_ALLOWED);
		} else {
			p->cd_state = CD_ENABLED;
			cd_set_state(vp, CD_ENABLED);
		}
		putnext(vp->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (ENOBUFS);
}

/**
 * cd_disable_con: - issue a CD_DISABLE_CON primitive
 * @vp: private structure
 * @q: active queue
 * @msg: message to free on success
 */
static streams_fastcall streams_inline
cd_disable_con(struct vp *vp, queue_t *q, mblk_t *msg)
{
	cd_disable_con_t *p;
	mblk_t *mp;

	if ((mp = vp_allocb(vp, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_DISABLE_CON;
		p->cd_state = CD_DISABLED;
		assert(cd_get_state(vp) == CD_DISABLE_PENDING);
		cd_set_state(vp, CD_DISABLED);
		putnext(vp->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (ENOBUFS);
}

/*
 *  CD Primitives
 */

/**
 * cd_info_req: - process CD_INFO_REQ primitive
 * @vp: private structure
 * @q: active queue
 * @mp: CD_INFO_REQ primitive
 */
static streams_fastcall streams_noinline __unlikely
cd_info_req(struct vp *vp, queue_t *q, mblk_t *mp)
{
	cd_info_req_t *p = (typeof(p)) mp->b_rptr;
	int error, errno;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	return cd_info_ack(vp, q, mp);
      protoshort:
	error = CD_PROTOSHORT;
	errno = EFAULT;
	goto error;
      error:
	return cd_error_ack(vp, q, mp, p->cd_primitive, error, errno);
}

/**
 * cd_attach_req: - process CD_ATTACH_REQ primitive
 * @vp: private structure
 * @q: active queue
 * @mp: CD_ATTACH_REQ primitive
 */
static streams_fastcall streams_noinline __unlikely
cd_attach_req(struct vp *vp, queue_t *q, mblk_t *mp)
{
	cd_attach_req_t *p = (typeof(p)) mp->b_rptr;
	int error, errno;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (vp->u.cd.info.cd_ppa_style != CD_STYLE2)
		goto notsupp;
	if (!cd_tas_state(vp, (CDF_UNATTACHED), CD_UNUSABLE))
		goto outstate;
	/* do it */
	return cd_ok_ack(vp, q, mp);
      badppa:
	error = CD_BADPPA;
	errno = EPROTO;
	goto error;
      notsupp:
	error = CD_NOTSUPP;
	errno = EOPNOTSUPP;
	goto error;
      outstate:
	error = CD_OUTSTATE;
	errno = EPROTO;
	goto error;
      protoshort:
	error = CD_PROTOSHORT;
	errno = EFAULT;
	goto error;
      error:
	return cd_error_ack(vp, q, mp, p->cd_primitive, error, errno);
}

/**
 * cd_detach_req: - process CD_DETACH_REQ primitive
 * @vp: private structure
 * @q: active queue
 * @mp: CD_DETACH_REQ primitive
 *
 * The driver might also support direct addressing where up to 124 minor
 * device numbers can be opened directly providing STYLE1 PPAs.  Watch out.
 */
static streams_fastcall streams_noinline __unlikely
cd_detach_req(struct vp *vp, queue_t *q, mblk_t *mp)
{
	cd_detach_req_t *p = (typeof(p)) mp->b_rptr;
	int error, errno;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (vp->u.cd.info.cd_ppa_style != CD_STYLE2)
		goto notsupp;
	if (!cd_tas_state(vp, (CDF_ENABLED), CD_UNUSABLE))
		goto outstate;
	/* do it */
	return cd_ok_ack(vp, q, mp);
      outstate:
	error = CD_OUTSTATE;
	errno = EPROTO;
	goto error;
      nosupp:
	error = CD_NOTSUPP;
	errno = EOPNOTSUPP;
	goto error;
      protoshort:
	error = CD_PROTOSHORT;
	errno = EFAULT;
	goto error;
      error:
	return cd_error_ack(vp, q, mp, p->cd_primitive, error, errno);
}

/**
 * cd_enable_req: - process CD_ENABLE_REQ primitive
 * @vp: private structure
 * @q: active queue
 * @mp: CD_ENABLE_REQ primitive
 */
static streams_fastcall streams_noinline __unlikely
cd_enable_req(struct vp *vp, queue_t *q, mblk_t *mp)
{
	cd_enable_req_t *p = (typeof(p)) mp->b_rptr;
	int error, errno;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!cd_tas_state(vp, (CDF_DISABLED), CD_ENABLE_PENDING))
		goto outstate;
	/* do it */
	return cd_enable_con(vp, q, mp);
      outstate:
	error = CD_OUTSTATE;
	errno = EPROTO;
	goto error;
      protoshort:
	error = CD_PROTOSHORT;
	errno = EFAULT;
	goto error;
      error:
	return cd_error_ack(vp, q, mp, p->cd_primitive, error, errno);
}

/**
 * cd_disable_req: - process CD_DISABLE_REQ primitive
 * @vp: private structure
 * @q: active queue
 * @mp: CD_DISABLE_REQ primitive
 */
static streams_fastcall streams_noinline __unlikely
cd_disable_req(struct vp *vp, queue_t *q, mblk_t *mp)
{
	cd_disable_req_t *p = (typeof(p)) mp->b_rptr;
	int error, errno;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!cd_tas_state(vp, (CDF_ENABLED | CDF_READ_ACTIVE | CDF_INPUT_ALLOWED |
			       CDF_OUTPUT_ACTIVE), CD_DISABLE_PENDING))
		goto outstate;	/* XXX: recheck valid states above */
	/* do it */
	return cd_disable_con(vp, q, mp);
      outstate:
	error = CD_OUTSTATE;
	errno = EPROTO;
	goto error;
      protoshort:
	error = CD_PROTOSHORT;
	errno = EFAULT;
	goto error;
      error:
	return cd_error_ack(vp, q, mp, p->cd_primitive, error, errno);
}

/**
 * cd_allow_input_req: - process CD_ALLOW_INPUT_REQ primitive
 * @vp: private structure
 * @q: active queue
 * @mp: CD_ALLOW_INPUT_REQ primitive
 */
static streams_fastcall streams_noinline __unlikely
cd_allow_input_req(struct vp *vp, queue_t *q, mblk_t *mp)
{
	cd_allow_input_req_t *p = (typeof(p)) mp->b_rptr;
	int error, errno;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!cd_tas_state(vp, (CDF_ENABLED | CDF_INPUT_ALLOWED), CD_INPUT_ALLOWED))
		goto outstate;
	/* do it */
	return cd_ok_ack(vp, q, mp);
      protoshort:
	error = CD_PROTOSHORT;
	errno = EFAULT;
	goto error;
      error:
	return cd_error_ack(vp, q, mp, p->cd_primitive, error, errno);
}

/**
 * cd_read_req: - process CD_READ_REQ primitive
 * @vp: private structure
 * @q: active queue
 * @mp: CD_READ_REQ primitive
 */
static streams_fastcall streams_noinline __unlikely
cd_read_req(struct vp *vp, queue_t *q, mblk_t *mp)
{
	cd_read_req_t *p = (typeof(p)) mp->b_rptr;
	int error, errno;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!cd_tas_state(vp, (CDF_ENABLED), CD_INPUT_ALLOWED))
		goto outstate;
	goto notsupp;
      notsupp:
	error = CD_NOTSUPP;
	errno = EOPNOTSUPP;
	goto error;
      protoshort:
	error = CD_PROTOSHORT;
	errno = EFAULT;
	goto error;
      error:
	return cd_error_ack(vp, q, mp, p->cd_primitive, error, errno);
}

/**
 * cd_unitdata_req: - process CD_UNITDATA_REQ primitive
 * @vp: private structure
 * @q: active queue
 * @mp: CD_UNITDATA_REQ primitive
 */
static streams_fastcall streams_noinline __unlikely
cd_unitdata_req(struct vp *vp, queue_t *q, mblk_t *mp)
{
	cd_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	int error, errno;
	size_t dlen;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!cd_chk_state(vp, (CDF_ENABLED | CDF_INPUT_ALLOWED | CDF_OUTPUT_ACTIVE |
			       CDF_READ_ACTIVE)))
		goto outstate;
	if (p->cd_addr_type != CD_IMPLICIT)
		goto baddrtype;
	if (p->cd_dest_addr_length || p->cd_dest_addr_offset)
		goto badaddress;
	if (mp->b_cont == NULL || (dlen = msgdsize(mp->b_cont)) < vp->u.cd.info.cd_min_sdu
	    || size > vp->u.cd.info.cd_max_sdu)
		goto badprim;
	/* do it */
	freemsg(mp);
	return (0);
      badaddress:
	error = CD_BADADDRESS;
	errno = EINVAL;
	goto error;
      badaddrtype:
	error = CD_BADADDRTYPE;
	errno = EINVAL;
	goto error;
      protoshort:
	error = CD_PROTOSHORT;
	errno = EFAULT;
	goto error;
      error:
	return cd_error_ack(vp, q, mp, p->cd_primitive, error, errno);
}

/**
 * cd_write_read_req: - process CD_WRITE_READ_REQ primitive
 * @vp: private structure
 * @q: active queue
 * @mp: CD_WRITE_READ_REQ primitive
 */
static streams_fastcall streams_noinline __unlikely
cd_write_read_req(struct vp *vp, queue_t *q, mblk_t *mp)
{
	cd_write_read_req_t *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp, *bp = mp->b_cont;
	int error, errno, err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!cd_tas_state(vp, (CDF_ENABLED), CD_INPUT_ALLOWED))
		goto outstate;
	/* make two primitives out of one */
	if (!(dp = dupb(mp)))
		goto enobufs;
	dp->b_rptr += sizeof(p->cd_primitive);
	dp->b_cont = bp;
	/* Try a CD_UNITDATA_REQ with duplicated block: if cd_unitdata_req() defers
	   cd_write_read_req() will be invoked later with the original primitive. */
	if ((err = cd_unitdata_req(vp, q, dp)))
		goto freeerr;
	mp->b_rptr = (unsigned char *) &p->cd_read_req;
	mp->b_cont = NULL;
	/* Permanently create a CD_READ_REQ out of the original block: if cd_read_req() defers,
	   cd_read_req() will be invoked later with the primitive. */
	return cd_read_req(vp, q, dp);
      freeerr:
	freeb(dp);
	return (err);
      enobufs:
	error = CD_SYSERR;
	errno = ENOBUFS;
	goto error;
      protoshort:
	error = CD_PROTOSHORT;
	errno = EFAULT;
	goto error;
      error:
	return cd_error_ack(vp, q, mp, p->cd_primitive, error, errno);

}

/**
 * cd_halt_input_req: - process CD_HALT_INPUT_REQ primitive
 * @vp: private structure
 * @q: active queue
 * @mp: CD_HALT_INPUT_REQ primitive
 */
static streams_fastcall streams_noinline __unlikely
cd_halt_input_req(struct vp *vp, queue_t *q, mblk_t *mp)
{
	cd_halt_input_req_t *p = (typeof(p)) mp->b_rptr;
	int error, errno;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (!cd_tas_state(vp, (CDF_ENABLED | CDF_INPUT_ALLOWED | CDF_READ_ACTIVE), CD_ENABLED))
		goto outstate;
	/* do it */
	return cd_ok_ack(vp, q, mp);
      outstate:
	error = CD_OUTSTATE;
	errno = EPROTO;
	goto error;
      protoshort:
	error = CD_PROTOSHORT;
	errno = EFAULT;
	goto error;
      error:
	return cd_error_ack(vp, q, mp, p->cd_primitive, error, errno);
}

/**
 * cd_abort_output_req: - process CD_ABORT_OUTPUT_REQ primitive
 * @vp: private structure
 * @q: active queue
 * @mp: CD_ABORT_OUTPUT_REQ primitive
 */
static streams_fastcall streams_noinline __unlikely
cd_abort_output_req(struct vp *vp, queue_t *q, mblk_t *mp)
{
	cd_abort_output_req_t *p = (typeof(p)) mp->b_rptr;
	cd_ulong newstate;
	int error, errno;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	if (cd_chk_state
	    (vp, (CDF_ENABLED | CDF_INPUT_ALLOWED | CDF_READ_ACTIVE | CDF_OUTPUT_ACTIVE)))
		goto outstate;
	newstate = (vp->flags & CD_FLAG_INPUT_ACTIVE) ? CD_INPUT_ALLOWED : CD_ENABLED;
	if (cd_tas_state(vp, (CDF_OUTPUT_ACTIVE), newstate)) {
		/* do it */
	}
	return cd_ok_ack(vp, q, mp);
      protoshort:
	error = CD_PROTOSHORT;
	errno = EFAULT;
	goto error;
      error:
	return cd_error_ack(vp, q, mp, p->cd_primitive, error, errno);
}

/**
 * cd_unknown_req: - process unknown primitive
 * @vp: private structure
 * @q: active queue
 * @mp: unknown primitive
 */
static streams_fastcall streams_noinline __unlikely
cd_unknown_req(struct vp *vp, queue_t *q, mblk_t *mp)
{
	cd_ulong *p = (typeof(p)) mp->b_rptr;
	int error, errno;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto protoshort;
	goto badprim;
	/* always bad */
      badprim:
	error = CD_BADPRIM;
	errno = EPROTO;
	goto error;
      protoshort:
	error = CD_PROTOSHORT;
	errno = EFAULT;
	goto error;
      error:
	return cd_error_ack(vp, q, mp, p->cd_primitive, error, errno);
}

static streams_fastcall streams_inline __hot_put int
cd_w_proto(struct vp *vp, queue_t *q, mblk_t *mp)
{
	cd_ulong prim;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
		swerr();
		freemsg(mp);
		return (0);
	}
	switch ((prim = *(cd_ulong *) mp->b_rptr)) {
	case CD_INFO_REQ:
		return cd_info_req(vp, q, mp);
	case CD_ATTACH_REQ:
		return cd_attach_req(vp, q, mp);
	case CD_DETACH_REQ:
		return cd_detach_req(vp, q, mp);
	case CD_ENABLE_REQ:
		return cd_enable_req(vp, q, mp);
	case CD_DISABLE_REQ:
		return cd_disable_req(vp, q, mp);
	case CD_ALLOW_INPUT_REQ:
		return cd_allow_input_req(vp, q, mp);
	case CD_READ_REQ:
		return cd_read_req(vp, q, mp);
	case CD_UNITDATA_REQ:
		return cd_unitdata_req(vp, q, mp);
	case CD_WRITE_READ_REQ:
		return cd_write_read_req(vp, q, mp);
	case CD_HALT_INPUT_REQ:
		return cd_halt_input_req(vp, q, mp);
	case CD_ABORT_OUTPUT_REQ:
		return cd_abort_output_req(vp, q, mp);
	default:
	case CD_MUX_NAME_REQ:
	case CD_MODEM_SIG_REQ:
		return cd_unknown_req(vp, q, mp);
	}
}

/*
 *  MX Primitives
 */

/*
 *  Management Primitives
 */

/*
 *  Message handling.
 */

static streamscall __hot_write int
cd_wmsg(struct vp *vp, queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return cd_w_data(vp, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cd_w_proto(vp, q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return cd_w_ioctl(vp, q, mp);
	case M_FLUSH:
		return cd_w_flush(vp, q, mp);
	default:
		return cd_w_other(vp, q, mp);
	}
}
static streamscall int
cd_rmsg(struct vp *vp, queue_t *q, mblk_t *mp)
{
}
static streamscall int
mx_wmsg(struct vp *vp, queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return mx_w_data(vp, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mx_w_proto(vp, q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return mx_w_ioctl(vp, q, mp);
	case M_FLUSH:
		return mx_w_flush(vp, q, mp);
	default:
		return mx_w_other(vp, q, mp);
	}
}
static streamscall int
mx_rmsg(struct vp *vp, queue_t *q, mblk_t *mp)
{
}
static streamscall int
mt_wmsg(struct vp *vp, queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return mt_w_data(vp, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mt_w_proto(vp, q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return mt_w_ioctl(vp, q, mp);
	case M_FLUSH:
		return mt_w_flush(vp, q, mp);
	default:
		return mt_w_other(vp, q, mp);
	}
}
static streamscall int
mt_rmsg(struct vp *vp, queue_t *q, mblk_t *mp)
{
}

/*
 *  PUT and SERVICE procedures.
 */
static streamscall int
vp_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(mp->b_datap->db_type) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || vp->wmsg(vp, q, mp)) {
		if (putq(q, mp))
			return (0);
		swerr();
		freemsg(mp);
	}
	return (0);
}

static streamscall int
vp_wsrv(queue_t *q)
{
	struct vp *vp = VP_PRIV(q);
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (vp->wmsg(vp, q, mp)) {
			dassert(!pcmsg(mp->b_datap->db_type));
			if (putbq(q, mp))
				break;
			swerr();
			freemsg(mp);
		}
	}
	return (0);
}

static streamscall int
vp_rsrv(queue_t *q)
{
	struct vp *vp = VP_PRIV(q);
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (vp->rmsg(vp, q, mp)) {
			dassert(!pcmsg(mp->b_datap->db_type));
			if (putbq(q, mp))
				break;
			swerr();
			freemsg(mp);
		}
	}
	return (0);
}

static streamscall int
vp_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(mp->b_datap->db_type) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || vp->rmsg(vp, q, mp)) {
		if (putq(q, mp))
			return (0);
		swerr();
		freemsg(mp);
	}
	return (0);
}

/*
 *  OPEN and CLOSE
 */
static caddr_t vp_head = NULL;

static streamscall int
vp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct vp *vp;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if (cmajor == CMAJOR_0 && cminor < 4)
		sflag = CLONEOPEN;
	if ((err = mi_open_comm(&vp_head, sizeof(*vp), q, devp, oflags, sflag, crp)))
		return (err);
	/* initialize private structure */
	vp = VP_PRIV(q);
	vp->rq = q;
	vp->wq = WR(q);
	switch ((vp->type = cminor)) {
	case 0:		/* CD raw stream (default) */
	case 1:		/* CD hdlc stream */
	case 2:		/* CD daed stream */
		cd->info.cd_class = CD_DAED;
		vp->rmsg = &cd_rmsg;
		vp->wmsg = &cd_wmsg;
		break;
	case 3:		/* MX stream */
		vp->rmsg = &mx_rmsg;
		vp->wmsg = &mx_wmsg;
		break;
	case 4:		/* V401P mgmt stream */
		vp->rmsg = &mt_rmsg;
		vp->wmsg = &mt_wmsg;
		break;
	}
	return (0);
}

static streamscall int
vp_qclose(queue_t *q, int oflags, cred_t *crp)
{
	qprocsoff(q);
	unbufcall(XCHG(&vp->rbid, 0));
	unbufcall(XCHG(&vp->wbid, 0));
	mi_close_comm(&vp_head, q);
	return (0);
}

static struct module_stat vp_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat vp_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit vp_rinit = {
	.qi_putp = vp_rput,
	.qi_srvp = vp_rsrv,
	.qi_qopen = vp_qopen,
	.qi_qclose = vp_qclose,
	.qi_minfo = &vp_minfo,
	.qi_mstat = &vp_rstat,
};

static struct qinit vp_winit = {
	.qi_putp = vp_wput,
	.qi_srvp = vp_wsrv,
	.qi_minfo = &vp_minfo,
	.qi_mstat = &vp_wstat,
};

static struct streamtab v401pinfo = {
	.st_rdinit = &vp_rinit,
	.st_wrinit = &vp_winit,
} modID_t modid = DRV_ID;
major_t major = CMAJOR_0;

#ifdef LINUX
/*
 *  Linux registration
 */
#ifdef module_param
module_param(modid, modID_t, 0);
module_param(major, major_t, 0);
#else				/* module_param */
MODULE_PARM(modid, "h");
MODULE_PARM(major, "d");
#endif				/* module_param */

MODULE_PARM_DESC(modid, "Module ID for the V401P driver. (0 for allocation.)");
MODULE_PARM_DESC(major, "Device number for the V401P driver. (0 for allocation.)");

static struct cdevsw vp_cdev = {
	.d_name = DRV_NAME,
	.d_str = &v401pinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

static __unlikely int
vp_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&vp_cdev, major)) < 0)
		return (err);
	return (0);
}

static __unlikely int
vp_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&vp_cdev, major)) < 0)
		return (err);
	return (0);
}

MODULE_STATIC void __ext
v401pterminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (vp_majors[mindex]) {
			if ((err = vp_unregister_strdev(vp_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					tp_majors[mindex]);
			if (mindex)
				vp_majors[mindex] = 0;
		}
	}
	if ((err = vp_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC void __init
v401pinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = vp_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		v401pterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = vp_register_strdev(vp_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					vp_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				udpterminate();
				return (err);
			}
		}
		if (vp_majors[mindex] == 0)
			vp_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(vp_majors[index]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = vp_majors[0];
	}
	return (0);
}

module_init(v401pinit);
module_exit(v401pterminate);

#endif				/* LINUX */
