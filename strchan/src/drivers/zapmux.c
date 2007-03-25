/*****************************************************************************

 @(#) $RCSfile: zapmux.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/03/25 19:00:52 $

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

 Last Modified $Date: 2007/03/25 19:00:52 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: zapmux.c,v $
 Revision 0.9.2.3  2007/03/25 19:00:52  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.2  2006/12/20 23:07:37  brian
 - updates for release and current development

 Revision 0.9.2.1  2006/12/06 11:23:23  brian
 - added new files

 *****************************************************************************/

#ident "@(#) $RCSfile: zapmux.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/03/25 19:00:52 $"

static char const ident[] = "$RCSfile: zapmux.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/03/25 19:00:52 $";

/*
 *  MX Primitives issued down to MX provider.
 *  -----------------------------------------
 */
/**
 * mx_info_req: issue MX_INFO_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 */
STATIC __unlikely int
mx_info_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_info_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		STRLOG(modid, PRIV(q)->sid, MXLOGTX, SL_TRACE, "MX_INFO_REQ ->");
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_optmgmt_req: issue MX_OPTMGMT_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 */
STATIC __unlikely int
mx_optmgmt_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_optmgmt_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OPTMGMT_REQ;
		mp->b_wptr += sizeof(*p);
		STRLOG(modid, PRIV(q)->sid, MXLOGTX, SL_TRACE, "MX_INFO_REQ ->");
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_attach_req: issue MX_ATTACH_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 * @flags: attach flags
 * @alen: address length
 * @aptr: address pointer
 */
STATIC __unlikely int
mx_attach_req(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, size_t alen, caddr_t aptr)
{
	struct MX_attach_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p) + alen, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_ATTACH_REQ;
		p->mx_addr_length = alen;
		p->mx_addr_offset = alen ? sizeof(*p) : 0;
		p->mx_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		STRLOG(modid, PRIV(q)->sid, MXLOGTX, SL_TRACE, "MX_INFO_REQ ->");
		mx_set_state(mx, MXS_WACK_AREQ);
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_enable_req: issue MX_ENABLE_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 */
STATIC __unlikely int
mx_enable_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_enable_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_ENABLE_REQ;
		mp->b_wptr += sizeof(*p);
		STRLOG(modid, PRIV(q)->sid, MXLOGTX, SL_TRACE, "MX_ENABLE_REQ ->");
		mx_set_state(mx, MXS_WACK_EREQ);
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_connect_req: issue MX_CONNECT_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 * @flags: connection flags
 * @slot: slot within multiplex
 */
STATIC __unlikely int
mx_connect_req(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot)
{
	struct MX_connect_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_CONNECT_REQ;
		p->mx_conn_flags = flags;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		STRLOG(modid, PRIV(q)->sid, MXLOGTX, SL_TRACE, "MX_CONNECT_REQ ->");
		mx_set_state(mx, MXS_WACK_CREQ);
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_data_req: issue MX_DATA_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success (also data).
 * @slot: slot for data
 */
STATIC __unlikely int
mx_data_req(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong slot)
{
	struct MX_data_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DATA_REQ;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = XCHG(&msg->b_cont, NULL);
		STRLOG(modid, PRIV(q)->sid, MXLOGDA, SL_TRACE, "MX_DATA_REQ ->");
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_disconnect_req: issue MX_DISCONNECT_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 */
STATIC __unlikely int
mx_disconnect_req(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot)
{
	struct MX_disconnect_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISCONNECT_REQ;
		p->mx_conn_flags = flags;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		STRLOG(modid, PRIV(q)->sid, MXLOGTX, SL_TRACE, "MX_DISCONNECT_REQ ->");
		mx_set_state(mx, MXS_WACK_DREQ);
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_disable_req: issue MX_DISABLE_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 */
STATIC __unlikely int
mx_disable_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_disable_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISABLE_REQ;
		mp->b_wptr += sizeof(*p);
		STRLOG(modid, PRIV(q)->sid, MXLOGTX, SL_TRACE, "MX_DISABLE_REQ ->");
		mx_set_state(mx, MXS_WACK_RREQ);
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_detach_req: issue MX_DETACH_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 */
STATIC __unlikely int
mx_detach_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_detach_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DETACH_REQ;
		mp->b_wptr += sizeof(*p);
		STRLOG(modid, PRIV(q)->sid, MXLOGTX, SL_TRACE, "MX_DETACH_REQ ->");
		mx_set_state(mx, MXS_WACK_UREQ);
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  State machine and callbacks.
 *  ----------------------------
 */

static int
zm_strdoioctl_str(struct mx *mx, int cmd, const void __user *arg, size_t len)
{
	union ioctypes *ioc;
	mblk_t *mp, *dp;
	long timeo;
	int err= 0;

#ifdef CONFIG_STREAMS_LIS_BCM
	static cred_t creds; 
	cred_t *crp = &creds;

	crp->cr_uid = current->euid;
	crp->cr_gid = current->egid;
	crp->cr_ruid = current->uid;
	crp->cr_rgid = current->gid;
#else
	cred_t *crp = current_creds;
#endif

	if (len < 0 || len > sysctl_str_strmsgsz)
		return (-EINVAL);
	if (!access_ok(VERIFY_WRITE, arg, len))
		return (-EFAULT);
	while (!(mp = allocb(sizeof(*ioc), BPRI_WAITOK))) ;
	DB_TYPE(mp) = M_IOCTL;
	ioc = (typeof(ioc)) mp->b_rptr;
	mp->b_wptr = mp->b_rptr + sizeof(*ioc);
	bzero(ioc, sizeof(*ioc));
	ioc->iocblk.ioc_cmd = cmd;
	ioc->iocblk.ioc_cr = crp;
	ioc->iocblk.ioc_count = len;
	ioc->iocblk.ioc_flag = IOC_NATIVE;
	while (!(dp = allocb(len, BPRI_WAITOK))) ;
	mp->b_cont = dp;
	dp->b_wptr += len;
	if (len && (err = copyin(arg, dp->b_rptr, len)) < 0) {
		freemsg(mp);
		return (err);
	}
	timeo = 5 * HZ;

}

static int
zm_wait_for_ack(struct mx *mx)
{
	long timeo = drv_msectohz(5000);
#if defined HAVE_KFUNC_PREPARE_TO_WAIT
	DEFINE_WAIT(wait);
#else
	DECLARE_WAITQUEUE(wait, current);
#endif

#if !defined HAVE_KFUNC_PREPARE_TO_WAIT
	add_wait_queue(&mx->mx_waitq, &wait);
#endif
	for (;;) {
#if defined HAVE_KFUNC_PREPARE_TO_WAIT
		prepare_to_wait(&mx->mx_waitq, &wait, TASK_INTERRUPTIBLE);
#else
		set_current_state(TASK_INTERRUPTIBLE);
#endif
		if (timeo = 0)
			break;
		if (mx->error != 1)
			break;
		timeo = schedule_timeout(timeo);
	}
#if defined HAVE_KFUNC_FINISH_WAIT
	finish_wait(&mx->mx_wait, &wait);
#else
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&mx->mx_wait, &wait);
#endif
	return (mx->error == 1 ? -ETIME : mx->error);
}

/**
 * zm_setchunksize: - zaptel callback, set chunk size
 * @span: the span for which to set chunk size
 * @chunksize: the chunk size to set
 *
 * Required: Set the requested chunk size.  This is the unit in which you must
 * report results for conferencing, etc
 */
static int
zm_setchunksize(struct zt_span *span, int chunksize)
{
	if (chunksize != ZT_CHUNKSIZE)
		return (-1);
	return (0);
}

/**
 * zm_spanconfig: - zaptel callback, configure span
 * @span: the span to configure
 * @lc: the configuration information
 *
 * Configure the span (if appropriate).  lc has three significant fields: lbo
 * (line build-out), lineconfig (line configuration parameters), sync (level
 * of synchronization source).
 */
static int
zm_spanconfig(struct zt_span *span, struct zt_lineconfig *lc)
{
	int err;
	mblk_t *mp, *dp;
	struct iocblk *ioc;
	struct mx *mx = (struct mx *) span->prv;

	/* Generate equivalent IO control command downstream. */
	if (!(mp = allocb(sizeof(union ioctypes), BPRI_MED)))
		return (-ENOSR);
	if (!(dp = allocb(sizeof(struct mx_config), BPRI_MED))) {
		freemsg(mp);
		return (-ENOSR);
	}
	dp->b_wptr += sizeof(struct mx_config);
	mp->b_datap->db_type = M_IOCTL;
	ioc = (typeof(ioc)) mp->b_wptr;
	mp->b_wptr += sizeof(union ioctypes);
	mp->b_cont = dp;

	ioc->ioc_cmd = MX_IOCSCONFIG;
	ioc->ioc_cr = mx->cred;
	ioc->ioc_id = atomic_add_return(zm_ioc_count, 1);
	ioc->ioc_count = sizeof(struct mx_config);
	ioc->ioc_error = 0;
	ioc->ioc_rval = 0;
	ioc->ioc_flag = IOC_NATIVE;

	/* FIXME: fill out mx_config structure */

	mx->error = 1;

	putnext(mx->wq, mp);
	return zm_wait_for_ack(mx);
}

/**
 * zm_startup: - start the span
 * @span: the span to start
 *
 * Start the span.  The span is started with an MX_ENABLE_REQ that must
 * ultimately wait for an MX_ERROR_ACK or an MX_ENABLE_CON.
 *
 */
static int
zm_startup(struct zt_span *span)
{
	struct mx *mx = (struct mx *) span->prv;

	mx->error = 1;

	if (mx_enable_req(mx, mx->wq, NULL) != 0)
		return (-ENOSR);
	return zm_wait_for_ack(mx);
}

/**
 * zm_shutdown: - zaptel callback, shutdown span
 * @span: span to shut down
 */
static int
zm_shutdown(struct zt_span *span)
{
	struct mx *mx = (struct mx *) span->prv;

	mx->error = 1;

	if (mx_disable_req(mx, mx->wq, NULL) != 0)
		return (-ENOSR);
	return zm_wait_for_ack(mx);
}

/**
 * zm_maint: - zaptel callback, perform maintenance on span
 * @span: span to maintain
 * @mode: maintainenacne mode
 */
static int
zm_maint(struct zt_span *span, int mode)
{
	struct mx *mx = (struct mx *) span->prv;

	switch (mode) {
	case ZT_MAINT_NONE:
		/* normal mode */
	case ZT_MAINT_LOCALLOOP:
		/* local loopback */
	case ZT_MAINT_REMOTELOOP:
		/* remote loopback */
	case ZT_MAINT_LOOPUP:
		/* send loopup code */
	case ZT_MAINT_LOOPDOWN:
		/* send loopdown code */
	case ZT_MAINT_LOOPSTOP:
		/* stop sending loop codes */
	}
}

/**
 * zm_chanconfig: zaptell callback, channel configuration
 * @chan: channel to configure
 * @sigtype: signalling type
 */
static int
zm_chanconfig(struct zt_chan *chan, int sigtype)
{
	struct mx *mx = (struct mx *) chan->prv;
}

/**
 * zm_open: zaptell callback, open a channel
 * @chan: channel to open
 *
 * Prepare a channel for I/O.
 */
static int
zm_open(struct zt_chan *chan)
{
	struct mx *mx = (struct mx *) chan->prv;
	uint32_t cmask;
	int i;

	for (cmask = 0, i = chan->chanpos;
	     i < chan->span->channels && chan->span->chans[i].nextslave; cmask |= (1 << i), i++) ;

	mx->error = 1;

	if (mx_connect_req(mx, mx->wq, NULL, 0, cmask) != 0)
		return (-ENOSR);
	return zm_wait_for_ack(mx);
}

/**
 * zm_close: zaptell callback, close a channel
 * @chan: channel to close
 *
 * Close channel for I/O.
 */
static int
zm_close(struct zt_chan *chan)
{
	struct mx *mx = (struct mx *) chan->prv;
	uint32_t cmask;
	int i;

	for (cmask = 0, i = chan->chanpos;
	     i < chan->span->channels && chan->span->chans[i].nextslave; cmask |= (1 << i), i++) ;

	mx->error = 1;
	if (mx_disconnect_req(mx, mx->rq, NULL, 0, cmask) != 0)
		return (-ENOSR);
	return zm_wait_for_ack(mx);
}

static int
zm_ioctl(struct zt_chan *chan, unsigned int cmd, unsigned long data)
{
	struct mx *mx = (struct mx *) chan->prv;
	struct iocblk *ioc;
	mblk_t *mp, *dp;

	if (!(mp = allocb(sizeof(union ioctypes), BPRI_MED)))
		return (-ENOSR);
	if (!(dp = allobc(sizeof(data), BPRI_MED))) {
		freeb(mp);
		return (-ENOSR);
	}
	dp->b_wptr += sizeof(unsigned long);
	*(unsigned long *)dp->b_rptr = data;
	DB_TYPE(mp) = M_IOCTL;
	ioc = (typeof(ioc)) mp->b_rptr;
	mp->b_wptr += sizeof(union ioctypes);
	mp->b_cont = dp;

	ioc->ioc_cmd = cmd;
	ioc->ioc_cr = make_creds();
	ioc->ioc_id = atomic_add_return(zm_ioc_count, 1);
	ioc->ioc_count = TRANSPARENT;
	ioc->ioc_error = 0;
	ioc->ioc_rval = 0;
	ioc->ioc_flag = IOC_NATIVE;

	mx->error = 1;
	putnext(mx->wq, mp);
	return zm_wait_for_ack(mx);
}

/**
 * zm_echocan: - native echo cancellation
 * @chan: channel to echo cancel
 * @ecval: 0 off, non-zero on, 32-256, number of filter taps.
 * 
 * Native echo cancellation
 */
static int
zm_echocan(struct zt_chan *chan, int ecval)
{
	if (ecval != 0)
		return (-ENOTTY);
	return (0);
}

	/* Okay, now we get to the signalling.  You have several options: */

	/* Option 1: If you're a T1 like interface, you can just provide a rbsbits function and
	   we'll assert robbed bits for you.  Be sure to set the ZT_FLAG_RBS in this case.  */

	/* Opt: If the span uses A/B bits, set them here */
static int
zm_rbsbits(struct zt_chan *chan, int bits)
{
	struct mx *mx = (struct mx *) chan->prv;
}

	/* Option 2: If you don't know about sig bits, but do have their equivalents (i.e. you can
	   disconnect battery, detect off hook, generate ring, etc directly) then you can just
	   specify a sethook function, and we'll call you with appropriate hook states to set.
	   Still set the ZT_FLAG_RBS in this case as well */
static int
zm_hooksig(struct zt_chan *chan, zt_txsig_t hookstate)
{
	struct mx *mx = (struct mx *) chan->prv;
}

	/* Option 3: If you can't use sig bits, you can write a function which handles the
	   individual hook states */
static int
zm_sethook(struct zt_chan *chan, int hookstate)
{
	struct mx *mx = (struct mx *) chan->prv;
}

	/* Opt: Dacs the contents of chan2 into chan1 if possible */
static int
zm_dacs(struct zt_chan *chan1, struct zt_chan *chan2)
{
}

/*
 *  MX primitives received from MX Provider below.
 *  ----------------------------------------------
 */

/**
 * mx_info_ack: process MX_INFO_ACK message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_INFO_ACK message
 */
STATIC noinline __unlikely int
mx_info_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_info_ack *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr > mp->b_rptr + sizeof(*p));

	mx->info.mx_prov_flags = p->mx_prov_flags;
	mx->info.mx_prov_class = p->mx_prov_class;
	mx->info.mx_style = p->mx_style;
	mx->info.mx_version = p->mx_version;
	mx_set_state(mx, p->mx_state);

	if (p->mx_addr_length <= sizeof(mx->addrs)) {
		bcopy(mp->b_rptr + p->mx_addr_offset, &mx->addrs, p->mx_addr_length);
		mx->info.mx_addr_length = p->mx_addr_length;
		mx->info.mx_addr_offset = sizeof(mx->info);
	}
	if (p->mx_parm_length == sizeof(mx->parms)
	    && *(mx_ulong *) (mp->b_rptr + p->mx_parm_offset) == mx->parms.mp_type) {
		bcopy(mp->b_rptr + p->mx_parm_offset, &mx->parms, p->mx_parm_length);
		mx->info.mx_parm_length = p->mx_parm_length;
		mx->info.mx_parm_offset = sizeof(mx->info) + mx->info.mx_addr_length;
	}

	if (mx->info.mx_primitive == 0) {
		struct ch *ch = MXCH(mx);

		mx->info.mx_primitive = MX_INFO_ACK;
		/* First MX_INFO_ACK: the first MX_INFO_ACK gives us information about the MX
		   stream over which we have been pushed.  If the MX Stream is a style II Stream
		   and the Stream has not yet been attached, the parameters information (above)
		   will be unknown.  In that case, we must issue another information request after
		   the MX Stream has been attached. */
		switch (mx_get_state(mx)) {
		case MXS_DETACHED:
		case MXS_ENABLED:
			MXCH(mx)->info.ch_style = mx->info.mx_style;
			break;
		case MXS_ATTACHED:
			MXCH(mx)->info.ch_style = CH_STYLE1;
			break;
		}
		ch->info.ch_prov_flags = mx->info.mx_prov_flags;
		ch->info.ch_prov_class = mx->info.mx_prov_class;
		/* TODO: check provider class: it must be MX_CIRCUIT */
	}
	{
		/* Subsequent MX_INFO_ACK: a subsequent request is necessary immediately after an
		   attach if the Stream was not attached during the last request.  Or, at any time
		   that we need to synchronize state or other parameters, a subsequent request can
		   be issued. */
		switch (mx_get_state(mx)) {
		case MXS_DETACHED:
			ch_set_state(MXCH(mx), CHS_DETACHED);
			break;
		case MXS_ATTACHED:
			ch_set_state(MXCH(mx), CHS_ATTACHED);
			/* addr and parms should be valid */
			break;
		case MXS_ENABLED:
			ch_set_state(MXCH(mx), CHS_ENABLED);
			/* addr and parms should be valid */
			break;
		case MXS_CONNECTED:
			ch_set_state(MXCH(mx), CHS_CONNECTED);
			/* addr and parms should be valid */
			break;
		default:
			break;
		}
		switch (mx_get_state(mx)) {
		case MXS_ATTACHED:
		case MXS_ENABLED:
			/* addr and parms should be valid in this state */
			if (mx->info.mx_parm_length = sizeof(mx->parms)) {
				struct ch *ch = MXCH(mx);

				ch->parms.cp_type = CH_PARMS_CIRCUIT;
				ch->parms.cp_encoding = mx->parms.mp_encoding;
				ch->parms.cp_block_size = ((mp->parms.mp_sample_size + 7) & 0x7)
				    * mx->parms.mp_samples;
				ch->parms.cp_samples = mx->parms.mp_samples;
				ch->parms.cp_sample_size = mx->parms.mp_sample_size;
				ch->parms.cp_rate = mx->parms.mp_rx_channels * mx->parms.mp_rate;
				ch->parms.cp_tx_channels = mx->parms.mp_rx_channels ? 1 : 0;
				ch->parms.cp_rx_channels = mx->parms.mp_tx_channels ? 1 : 0;
				/* should record in options flags whether these are bit reversed or 
				   not */
				ch->parms.cp_opt_flags = mx->parms.mx_opt_flags;
				/* Need a channel sample map providing the offset of each sample
				   for each channel within a block.  We do not do this yet, and
				   just assume the V40XP channel mapping. */
				/* Pre-calculate some values used by the RX/TX procedures. */
				mx->offset = mx->info.mp_spans * mx->info.mp_slots;
				mx->mapsize = mx->info.mp_samples * mx->offset;
			}
			break;
		case MXS_DETACHED:
			/* addr and parms are not valid in this state */
			break;
		}
	}
	freemsg(mp);
	return (0);
}

/**
 * mx_optmgmt_ack: process MX_OPTMGMT_ACK message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_OPTMGMT_ACK message
 */
STATIC noinline __unlikely int
mx_optmgmt_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * mx_ok_ack: process MX_OK_ACK message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_OK_ACK message
 */
STATIC noinline __unlikely int
mx_ok_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_ok_ack *p = (typeof(p)) mp->b_rptr;
	mx_ulong state;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));

	state = mx_get_state(mx);
	mx_set_state(mx, p->mx_state);

	switch (state) {
	case MXS_WACK_AREQ:
		return ch_ok_ack(MXCH(mx), q, mp, CH_ATTACH_REQ, CHS_ATTACHED);
	case MXS_WACK_UREQ:
		return ch_ok_ack(MXCH(mx), q, mp, CH_DETACH_REQ, CHS_DETACHED);
	case MXS_WACK_EREQ:
		return ch_ok_ack(MXCH(mx), q, mp, CH_ENABLE_REQ, CHS_WCON_EREQ);
	case MXS_WACK_RREQ:
		return ch_ok_ack(MXCH(mx), q, mp, CH_DISABLE_REQ, CHS_WCON_RREQ);
	case MXS_WACK_CREQ:
		return ch_ok_ack(MXCH(mx), q, mp, CH_CONNECT_REQ, CHS_WCON_CREQ);
	case MXS_WACK_DREQ:
		return ch_ok_ack(MXCH(mx), q, mp, CH_DISCONNECT_REQ, CHS_WCON_DREQ);
	default:
		swerr();
		freemsg(mp);
		return (0);
	}
}

/**
 * mx_error_ack: process MX_ERROR_ACK message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_ERROR_ACK message
 */
STATIC noinline __unlikely int
mx_error_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_ok_ack *p = (typeof(p)) mp->b_rptr;
	mx_ulong state;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));

	state = mx_get_state(mx);
	mx_set_state(mx, p->mx_state);

	switch (state) {
	case MXS_WACK_AREQ:
		return ch_error_ack(MXCH(mx), q, mp, CH_ATTACH_REQ, CHS_DETACHED,
				    p->mx_error_type, p->mx_unix_error);
	case MXS_WACK_UREQ:
		return ch_error_ack(MXCH(mx), q, mp, CH_DETACH_REQ, CHS_ATTACHED,
				    p->mx_error_type, p->mx_unix_error);
	case MXS_WACK_EREQ:
		return ch_error_ack(MXCH(mx), q, mp, CH_ENABLE_REQ, CHS_ATTACHED,
				    p->mx_error_type, p->mx_unix_error);
	case MXS_WACK_RREQ:
		return ch_error_ack(MXCH(mx), q, mp, CH_DISABLE_REQ, CHS_ENABLED,
				    p->mx_error_type, p->mx_unix_error);
	case MXS_WACK_CREQ:
		return ch_error_ack(MXCH(mx), q, mp, CH_CONNECT_REQ, CHS_ENABLED,
				    p->mx_error_type, p->mx_unix_error);
	case MXS_WACK_DREQ:
		return ch_error_ack(MXCH(mx), q, mp, CH_DISCONNECT_REQ, CHS_CONNECTED,
				    p->mx_error_type, p->mx_unix_error);
	}
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * mx_enable_con: process MX_ENABLE_CON message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_ENABLE_CON message
 */
STATIC noinline __unlikely int
mx_enable_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_enable_con *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	mx_set_state(mx, MXS_ENABLED);
	return ch_enable_con(MXCH(mx), q, mp);
}

/**
 * mx_connect_con: process MX_CONNECT_CON message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_CONNECT_CON message
 */
STATIC noinline __unlikely int
mx_connect_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_connect_con *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	mx_set_state(mx, MXS_CONNECTED);
	return ch_connect_con(MXCH(mx), q, mp, p->mx_conn_flags, p->mx_slot);
}

/**
 * mx_data_ind: process MX_DATA_IND message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_DATA_IND message
 */
STATIC noinline __unlikely int
mx_data_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	/* must be discarding */
	freemsg(mp);
	return (0);
}

/**
 * mx_disconnect_ind: process MX_DISCONNECT_IND message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_DISCONNECT_IND message
 */
STATIC noinline __unlikely int
mx_disconnect_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disconnect_ind *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	mx_set_state(mx, MXS_ENABLED);
	return ch_disconnect_ind(MXCH(mx), q, mp, p->mx_conn_flags, p->mx_slot, p->mx_cause);
}

/**
 * mx_disconnect_con: process MX_DISCONNECT_CON message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_DISCONNECT_CON message
 */
STATIC noinline __unlikely int
mx_disconnect_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disconnect_con *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	mx_set_state(mx, MXS_ENABLED);
	return ch_disconnect_con(MXCH(mx), q, mp, p->mx_conn_flags, p->mx_slot);
}

/**
 * mx_disable_ind: process MX_DISABLE_IND message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_DISABLE_IND message
 */
STATIC noinline __unlikely int
mx_disable_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disable_ind *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	mx_set_state(mx, MXS_ATTACHED);
	return ch_disable_ind(MXCH(mx), q, mp, p->mx_cause);
}

/**
 * mx_disable_con: process MX_DISABLE_CON message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_DISABLE_CON message
 */
STATIC noinline __unlikely int
mx_disable_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disable_con *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	mx_set_state(mx, MXS_ATTACHED);
	return ch_disable_con(MXCH(mx), q, mp);
}

/**
 * mx_event_ind: process MX_EVENT_IND message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_EVENT_IND message
 *
 * There are two events of interest, alarm events and robbed signalling bit
 * events.  Alarm events have a span number in the slot assignment.  Robbed
 * signalling bits have a channel number in the slot assignment.
 */
STATIC noinline __unlikely int
mx_event_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_event_ind *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));

	span = ((p->ch_slot >> 8) & 0x0ff) - 1;
	chan = ((p->ch_slot >> 0) & 0x0ff) - 1;

	switch (p->ch_event) {
	case MX_EVT_YEL_ALARM:
	case MX_EVT_BLU_ALARM:
	case MX_EVT_RED_ALARM:
	case MX_EVT_NO_ALARM:
	case MX_EVT_ALARMS:
		mx->zt_span[span].alarms = p->mx_status;
		freemsg(mp);
		zt_alarm_notify(&mx->zt_span[span]);
		return (0);
	case MX_EVT_SIG_BITS:
		int status = p->mx_status;

		freemsg(mp);
		if (!(mx->zt_span[span].chans[chan].sig & ZT_SIG_CLEAR))
			zt_rbsbits(&mx->zt_span[span].chans[chan], p->mx_status);
		return (0);
	default:
		strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "unrecognized event in MX_EVENT_IND %d",
		       (int) p->mx_event);
		freemsg(mp);
		return (0);

	}
	return ch_event_ind(MXCH(mx), q, mp, p->mx_event, p->mx_slot);
}

static caddr_t mx_links = NULL;

static inline fastcall int
zm_w_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_IOCTL:
	{
		struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
		struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
		struct mx *mx;
		int err = EINVAL;
		mblk_t *bp;

		switch (ioc->ioc_cmd) {
		case I_PLINK:
			if (ioc->ioc_cr->cr_uid != 0) {
				err = EPERM;
				goto nak_it;
			}
			/* fall through */
		case I_LINK:
			if (!(bp = allocb(sizeof(mx_ulong), BPRI_MED))) {
				err = ENOSR;
				goto nak_it;
			}
			if (!(mx = (struct mx *) mi_open_detached(&mx_links, sizeof(*mx),
								  (dev_t *) &l->l_index))) {
				freeb(bp);
				err = ENOMEM;
				goto nak_it;
			}
			mi_attach(_RD(l->l_qbot), (caddr_t) mx);
			bzero(mx, sizeof(*mx));

			mx->mid = q->q_init->qi_minfo->mi_idnum;
			mx->sid = l->l_index;
			mx->wq = l->l_qbot;
			mx->rq = _RD(l->l_qbot);
			mx->rq->q_ptr = mx->wq->q_ptr = mx;
			mx->info.mx_state = MXS_UNUSABLE;
			/* XXX: initialize structure some more */
			DB_TYPE(bp) = M_PCPROTO;
			*(mx_ulong *) bp->b_wptr = MX_INFO_REQ;
			bp->b_wptr += sizeof(mx_ulong);
			putnext(mx->wq, bp);
			/* Because this was a priority message, we should already have a response,
			   or the message disappeared. */
			if (mx->info.mx_state != MXS_ATTACHED) {
				mi_close_comm(&mx_links, _RD(l->l_qbot));
				err = ENXIO;
				goto nak_it;
			}
			/* initialize the structure some more */
			if (zt_register(&mx->span, 0) == -1) {
				mi_close_comm(&mx_links, _RD(l->l_qbot));
				err = ENXIO;
				goto nak_it;
			}
			goto ack_it;
		case I_PUNLINK:
			if (ioc->ioc_cr->cr_uid != 0) {
				err = EPERM;
				goto nak_it;
			}
			/* fall through */
		case I_UNLINK:
			for (mx = (struct mx *) mi_first_ptr(&mx_links);
			     mx && mx->sid != l->l_index;
			     mx = mi_next_ptr(&mx_links, (caddr_t) mx)) ;
			if (!mx) {
				err = ENXIO;
				goto nak_it;
			}
			/* unregister */
			zt_unregister(&mx->span);
			flushq(mx->rq, FLUSHALL);
			mi_close_comm(&mx_links, mx->rq);
			goto ack_it;
		default:
		      nak_it:
			DB_TYPE(mp) = M_IOCNAK;
			ioc->ioc_rval = -1;
			ioc->ioc_error = err;
			qreply(q, mp);
			return (0);
		      ack_it:
			/* ack it */
			DB_TYPE(mp) = M_IOCACK;
			ioc->ioc_rval = 0;
			ioc->ioc_error = 0;
			qreply(q, mp);
			return (0);
		}
	}
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
			mp->b_rptr[0] &= ~FLUSHW;
		}
		if (mp->b_rptr[0] & FLUSHR) {
			qreply(q, mp);
			return (0);
		}
		freemsg(mp);
		return (0);
	}
}

static noinline fastcall int
mx_r_msg(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);

	switch (DB_TYPE(mp)) {
	case M_PROTO:
	case M_PCPROTO:
		if (mp->b_wptr < mp->b_rptr + sizeof(mx_ulong)) {
			strlog(mx->mid, mx->sid, 0, SL_ERROR, "primitive too short");
			freemsg(mp);
			return (0);
		}
		switch (*(mx_ulong *) mp->b_rptr) {
		case MX_INFO_ACK:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "MX_INFO_ACK <-");
			return mx_info_ack(mx, q, mp);
		case MX_OPTMGMT_ACK:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "MX_OPTMGMT_ACK <-");
			return mx_optmgmt_ack(mx, q, mp);
		case MX_OK_ACK:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "MX_OK_ACK <-");
			return mx_ok_ack(mx, q, mp);
		case MX_ERROR_ACK:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "MX_ERROR_ACK <-");
			return mx_error_ack(mx, q, mp);
		case MX_ENABLE_CON:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "MX_ENABLE_CON <-");
			return mx_enable_con(mx, q, mp);
		case MX_CONNECT_CON:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "MX_CONNECT_CON <-");
			return mx_connect_con(mx, q, mp);
		case MX_DATA_IND:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "MX_DATA_IND <-");
			return mx_data_ind(mx, q, mp);
		case MX_DISCONNECT_IND:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "MX_DISCONNECT_IND <-");
			return mx_disconnect_ind(mx, q, mp);
		case MX_DISCONNECT_CON:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "MX_DISCONNECT_CON <-");
			return mx_disconnect_con(mx, q, mp);
		case MX_DISABLE_IND:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "MX_DISABLE_IND <-");
			return mx_disable_ind(mx, q, mp);
		case MX_DISABLE_CON:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "MX_DISABLE_CON <-");
			return mx_disable_con(mx, q, mp);
		case MX_EVENT_IND:
			strlog(mx->mid, mx->sid, MXLOGRX, SL_TRACE, "MX_EVENT_IND <-");
			return mx_event_ind(mx, q, mp);
		}
		strlog(mx->mid, mx->sid, 0, SL_ERROR, "unexpected primitive %d",
		       (int) *(mx_ulong *) mp->b_rptr);
		freemsg(mp);
		return (0);
	case M_FLUSH:
		/* canonical flush for lower mux stream */
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
			mp->b_rptr[0] &= ~FLUSHR;
		}
		if (!(mp->b_flag & MSGNOLOOP)) {
			if (mp->b_rptr[0] & FLUSHW) {
				mp->b_flag |= MSGNOLOOP;
				qreply(q, mp);
				return (0);
			}
		}
		freemsg(mp);
		return (0);
	}
	strlog(mx->mid, mx->sid, 0, SL_ERROR, "unexpected message %d on lower read queue",
	       (int) DB_TYPE(mp));
	freemsg(mp);
	return (0);
}

static inline fastcall __hot_in int
mx_r_data(queue_t *q, mblk_t *mp)
{
	const unsigned char *iptr;
	unsigned char *optr;
	int sample, chan, span;
	uint32_t smask, cmask;

	/* Only process from service procedure.  If we split we slip, it's only zaptel.  If you
	   want rock solid data channels, use the ch module instead.  */
	for (iptr = mp->b_rptr, sample = 0; sample < ZT_CHUNKSIZE; sample++)
		for (chan = 0; chan < mx->channels; chan++)
			for (span = 0; span < SPANS_PER_CARD; span++) {
				mx->zt_span[span].chans[chan].readchunk[sample] = *iptr++;
				*optr++ = mx->zt_span[span].chans[chan].writechunk[sample];
			}
	qreply(q, mp);
	for (span = 0; span < SPANS_PER_CARD; span++) {
		if (mx->zt_span[span].flags & ZT_FLAG_RUNNING) {
			/* tormenta driver tries to close the analytic window a bit */
			for (chan = 0; chan < mx->zt_spans[span].channels; chan++) {
				zt_ec_chunk(&mx->zt_spans[span].chans[chan],
					    mx->zt_spans[span].chans[chan].readchunk,
					    mx->ec_chunk2[span][chan]);
				bcopy(mx->ec_chunk1[span][chan], mx->ec_chunk2[span][chan],
				      ZT_CHUNKSIZE);
				bcopy(mx->zt_span[span].chans[chan].writechunk,
				      mx->ec_chunk1[span][chan], ZT_CHUNKSIZE);
			}
			zt_receive(&mx->zt_span[span]);
		}
	}
	for (span = 0; span < SPANS_PER_CARD; span++)
		if (mx->zt_span[span].flags & ZT_FLAG_RUNNING)
			zt_transmit(&mx->zt_span[span]);
	return (0);
}

/*
 *  Put and service procedure.
 */
static streamscall __hot_write int
zm_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || zm_w_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_out int
zm_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (zm_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static streamscall __hot_in int
mx_rput(queue_t *q, mblk_t *mp)
{
	if ((DB_TYPE(mp) == M_DATA)
	    || (!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || mx_r_msg(q, mp))
		putq(q, mp);
	return (0);
}

static streamscall __hot_read int
mx_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if ((DB_TYPE(mp) == M_DATA && mx_r_data(q, mp))
		    || (DB_TYPE(mp) != M_DATA && mx_r_msg(q, mp))) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

/*
 *  Open and close.
 */
static caddr_t zm_head = NULL;

static streamscall __unlikely int
zm_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct zm *zm;
	int err;

	if ((err = mi_open_comm(&zm_head, q, devp, oflags, sflag, crp)))
		return (err);

	zm = ZM_PRIV(q);
	/* initialize zm structure */
	qprocson(q);
	return (0);
}

static streamscall __unlikely int
zm_qclose(queue_t *q, int oflags, cred_t *crp)
{
	qprocsoff(q);
	/* terminate zm structure */
	if ((err = mi_close_comm(&zm_head, q)))
		return (err);
	return (0);
}

static modID_t modid = DRV_ID;
static major_t major = CMAJOR_0;

#ifdef module_param
module_param(modid, modID_t, 0444);
module_param(major, major_t, 0444);
#else
MODULE_PARM(modid, "h");
MODULE_PARM(major, "h");
#endif

static struct module_info zm_info = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat zm_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat zm_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat mx_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat mx_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit zm_rinit = {
	.qi_putp = NULL,
	.qi_srvp = NULL,
	.qi_qopen = zm_qopen,
	.qi_qclose = zm_qclose,
	.qi_minfo = &zm_minfo,
	.qi_mstat = &zm_rstat,
};

static struct qinit zm_winit = {
	.qi_putp = zm_wput,
	.qi_srvp = zm_wsrv,
	.qi_minfo = &zm_minfo,
	.qi_mstat = &zm_wstat,
};

static struct qinit mx_rinit = {
	.qi_putp = mx_rput,
	.qi_srvp = mx_rsrv,
	.qi_qopen = mx_qopen,
	.qi_qclose = mx_qclose,
	.qi_minfo = &zm_minfo,
	.qi_mstat = &mx_rstat,
};

static struct qinit mx_winit = {
	.qi_putp = NULL,
	.qi_srvp = NULL,
	.qi_minfo = &zm_minfo,
	.qi_mstat = &mx_wstat,
};

static struct streamtab zapmuxinfo = {
	.st_rdinit = &zm_rinit,
	.st_wrinit = &zm_winit,
	.st_muxrinit = &mx_rinit,
	.st_muxwinit = &mx_winit,
};

static struct cdevsw zm_cdev = {
	.d_name = DRV_NAME,
	.d_str = zapmuxinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

static __init int
zapmuxinit(void)
{
	int err;

	if ((err = register_strdev(&zm_cdev, major)) < 0)
		return (err);
	if (major == 0)
		major = err;
	return (0);
}

static __exit void
zapmuxterminate(void)
{
	int err;

	if ((err = unregister_strdev(&zm_cdev, major)) < 0)
		strlog(modid, 0, 0, SL_CONSOLE | SL_FATAL, "cannot unregister major %d",
		       (int) major);
	return;
}

module_init(zapmuxinit);
module_exit(zapmuxterminate);
