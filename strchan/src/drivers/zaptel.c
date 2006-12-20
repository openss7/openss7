/*****************************************************************************

 @(#) $RCSfile: zaptel.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/12/20 23:07:07 $

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

 Last Modified $Date: 2006/12/20 23:07:07 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: zaptel.c,v $
 Revision 0.9.2.1  2006/12/20 23:07:07  brian
 - new working files

 *****************************************************************************/

#ident "@(#) $RCSfile: zaptel.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/12/20 23:07:07 $"

static char const ident[] = "$RCSfile: zaptel.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/12/20 23:07:07 $";

/*
 *  This is a STREAMS multiplexing driver that links MX streams at the lower multiplex and provides
 *  zaptel streams at the upper interface.  There are two approaches that can be taken with this
 *  driver.  The initial approach is to register spans with the existing zaptel infrastructure once
 *  MX streams are linked beneath the multiplexing driver (zapmux driver).  The second approach
 *  (this zaptel driver) is to directly replace the zaptel infrastructure with a STREAMS-based one
 *  and provide the legacy zaptel character device driver interface directly from the upper
 *  multiplex.  In either case, the driver requires the presence of zaptel header files which have
 *  been included in the package but which match the version of any existing zaptel infrastructure.
 */

static int
zap_m_ioctl(struct zt_chan *chan, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *dp = mp->b_cont;

	switch (ioc->ioc_cmd) {
	case ZT_GET_BLOCKSIZE:
	case ZT_SET_BLOCKSIZE:
	case ZT_FLUSH:
	case ZT_SYNC:
	case ZT_GET_PARAMS:
	case ZT_SET_PARAMS:
	case ZT_HOOK:
	case ZT_SPANSTAT:
	case ZT_MAINT:
	case ZT_GETCONF:
	case ZT_SETCONF:
	case ZT_CONFLINK:
	case ZT_CONFDIAG:
	case ZT_GETGAINS:
	case ZT_SETGAINS:
	case ZT_SPANCONFIG:
	case ZT_CHANCONFIG:
	case ZT_CONFMUTE:
	case ZT_SENDTONE:
	case ZT_SETTONEZONE:
	case ZT_GETTONEZONE:
	case ZT_DEFAULTZONE:
	case ZT_LOADZONE:
	case ZT_FREEZONE:
	case ZT_SET_BUFINFO:
	case ZT_GET_BUFINFO:
	case ZT_GET_DIALPARAMS:
	case ZT_SET_DIALPARAMS:
	case ZT_DIAL:
	case ZT_AUDIOMODE:
	case ZT_ECHOCANCEL:
	case ZT_CHANNO:
	case ZT_DIALING:
	case ZT_HDLCRAWMODE:
	case ZT_HDLCFCSMODE:
	case ZT_SPECIFY:
	{
		int channo = *(int *) dp->b_rptr;

		if ((channo < 1) || (channo > ZT_MAX_CHANNELS))
			miocnak(q, mp, 0, EINVAL);
		if ((err = zt_specchan_open(NULL, qstream(q)->sd_file, channo, 0)) == 0) {
			RD(q)->q_ptr = WR(q)->q_ptr = &chans[channo];
			miocack(q, mp, 0, 0);
		} else {
			miocnak(q, mp, 0, (err < 0 ? -err : err));
		}
		return (0);
	}
	case ZT_SETLAW:
	case ZT_SETLINEAR:
	case ZT_HDLCPPP:
	case ZT_SETCADENCE:
	case ZT_SETTXBITS:
	case ZT_CHANDIAG:
	case ZT_GETRXBITS:
	case ZT_SFCONFIG:
	case ZT_TIMERCONFIG:
	case ZT_TIMERACK:
	case ZT_GETCONFMUTE:
	case ZT_ECHOTRAIN:
	case ZT_ONHOOKTRANSFER:
	case ZT_TIMERPING:
	case ZT_TIMERPONG:
	case ZT_SIGFREEZE:
	case ZT_GETSIGFREEZE:
		mi_copyout(q, mp, NULL, sizeof(int));
		return (0);
	case ZT_INDIRECT:
	case ZT_DYNAMIC_CREATE:
	case ZT_DYNAMIC_DESTROY:
	case ZT_TONEDETECT:
	case ZT_SETPOLARITY:
	case ZT_STARTUP:
	case ZT_SHUTDOWN:
	}
}

static fastcall
zap_w_data_slow(queue_t *q, mblk_t *mp)
{
}
static inline fastcall
zap_w_data(queue_t *q, mblk_t *mp)
{
}
static fastcall
zap_w_proto(queue_t *q, mblk_t *mp)
{
}
static fastcall
zap_w_ioctl(queue_t *q, mblk_t *mp)
{
}
static fastcall
zap_w_ctl(queue_t *q, mblk_t *mp)
{
}
static fastcall
zap_w_flush(queue_t *q, mblk_t *mp)
{
	/* canonical flush for driver write queue */
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(_RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(_RD(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

static fastcall
zap_r_data_slow(queue_t *q, mblk_t *mp)
{
}
static inline fastcall
zap_r_data(queue_t *q, mblk_t *mp)
{
}
/**
 * zap_r_proto: - process M_(PC)PROTO message on lower read queue
 * @q: the lower read queue
 * @mp: the M_(PC)PROTO message to process
 */
static fastcall
zap_r_proto(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);

	switch (*(mx_ulong *) mp->b_rptr) {
	case MX_INFO_ACK:
		strlog(mx->modid, mx->sid, 0, SL_TRACE, "<- MX_INFO_ACK");
		rtn = mx_info_ack(mx, q, mp);
		break;
	case MX_OPTMGMT_ACK:
		strlog(mx->modid, mx->sid, 0, SL_TRACE, "<- MX_OPTMGMT_ACK");
		rtn = mx_optmgmt_ack(mx, q, mp);
		break;
	case MX_OK_ACK:
		strlog(mx->modid, mx->sid, 0, SL_TRACE, "<- MX_OK_ACK");
		rtn = mx_ok_ack(mx, q, mp);
		break;
	case MX_ERROR_ACK:
		strlog(mx->modid, mx->sid, 0, SL_TRACE, "<- MX_ERROR_ACK");
		rtn = mx_error_ack(mx, q, mp);
		break;
	case MX_ENABLE_CON:
		strlog(mx->modid, mx->sid, 0, SL_TRACE, "<- MX_ENABLE_CON");
		rtn = mx_enable_con(mx, q, mp);
		break;
	case MX_CONNECT_CON:
		strlog(mx->modid, mx->sid, 0, SL_TRACE, "<- MX_CONNECT_CON");
		rtn = mx_connect_con(mx, q, mp);
		break;
	case MX_DATA_IND:
		strlog(mx->modid, mx->sid, 0, SL_TRACE, "<- MX_DATA_IND");
		rtn = mx_data_ind(mx, q, mp);
		break;
	case MX_DISCONNECT_IND:
		strlog(mx->modid, mx->sid, 0, SL_TRACE, "<- MX_DISCONNECT_IND");
		rtn = mx_disconnect_ind(mx, q, mp);
		break;
	case MX_DISCONNECT_CON:
		strlog(mx->modid, mx->sid, 0, SL_TRACE, "<- MX_DISCONNECT_CON");
		rtn = mx_disconnect_con(mx, q, mp);
		break;
	case MX_DISABLE_IND:
		strlog(mx->modid, mx->sid, 0, SL_TRACE, "<- MX_DISABLE_IND");
		rtn = mx_disable_ind(mx, q, mp);
		break;
	case MX_DISABLE_CON:
		strlog(mx->modid, mx->sid, 0, SL_TRACE, "<- MX_DISABLE_CON");
		rtn = mx_disable_con(mx, q, mp);
		break;
	case MX_EVENT_IND:
		strlog(mx->modid, mx->sid, 0, SL_TRACE, "<- MX_EVENT_IND");
		rtn = mx_event_ind(mx, q, mp);
		break;
	}
}
static fastcall
zap_r_ioctl(queue_t *q, mblk_t *mp)
{
}
static fastcall
zap_r_flush(queue_t *q, mblk_t *mp)
{
	/* canonical flush for driver read queue */
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHR;
	}
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(_WR(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(_WR(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}
static fastcall
zap_r_ctl(queue_t *q, mblk_t *mp)
{
}
static fastcall
zap_r_sig(queue_t *q, mblk_t *mp)
{
}
static noinline fastcall
zap_w_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return zap_w_data_slow(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return zap_w_proto(q, mp);
	case M_FLUSH:
		return zap_w_flush(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return zap_w_ioctl(q, mp);
	case M_CTL:
	case M_PCCTL:
		return zap_w_ctl(q, mp);
	}
	freemsg(mp);
	return (0);
}
static noinline fastcall
zap_r_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return zap_r_data_slow(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return zap_r_proto(q, mp);
	case M_FLUSH:
		return zap_r_flush(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return zap_r_ioctl(q, mp);
	case M_CTL:
	case M_PCCTL:
		return zap_r_ctl(q, mp);
	case M_SIG:
	case M_PCSIG:
		return zap_r_sig(q, mp);
	}
	freemsg(mp);
	return (0);
}
static inline fastcall int
zap_w_msg(queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_DATA)
		return zap_w_data(q, mp);
	return zap_w_msg_slow(q, mp);
}
static inline fastcall int
zap_r_msg(queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_DATA)
		return zap_r_data(q, mp);
	return zap_r_msg_slow(q, mp);
}

static streamscall int
zap_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || zap_w_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int
zap_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (zap_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall int
zap_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || zap_r_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int
zap_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (zap_r_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static streamscall int
zap_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct zt_chan *chan;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == DRVOPEN && cmajor == CMAJOR_0) {
		if (cminor == 0) {
			/* explicit open of control device */
		} else if (cminor == 253) {
			/* special timining device */
		} else if (cminor == 254) {
		} else if (cminor == 255) {
		} else if (cminor < ZT_MAX_CHANNELS) {
			chan = &chans[cminor];
		} else
			goto enxio;
	} else if (sflag == CLONEOPEN && cmajor == CMAJOR_0) {
		if ((cminor == 255 && !(chan = zt_alloc_pseudo())) || cminor != 255)
			goto enxio;
	} else
		goto enxio;
	*devp = makedevice(cmajor, chan->channo);
	if ((err = zt_specchan_open(NULL, qstream(q)->sd_file, chan->channo, 1)))
		return (err < 0 ? -err : err);
	q->q_ptr = WR(q)->q_ptr = (void *) chan;
	return (0);
      enxio:
	return (ENXIO);
}

static streamscall int
zap_qclose(queue_t *q, int oflags, cred_t *crp)
{
	qprocsoff(q);
	q->q_ptr = WR(q)->q_ptr = NULL;
	return zt_specchan_release(sd->sd_inode, sd->sd_file);
}

static __init int
zapinit(void)
{
	int err;

	if ((err = register_strdev(&zap_cdev, major)) < 0) {
		strlog(modid, 0, 0, SL_CONSOLE | SL_FATAL,
		       "could not regsiter STREAMS device, err = %d", err);
		return (err);
	}
	if (major == 0)
		major = err;
	return (0);
}

static __exit void
zapexit(void)
{
	int err;

	if ((err = unregister_strdev(&zap_cdev, major)) < 0)
		strlog(modid, 0, 0, SL_CONSOLE | SL_FATAL,
		       "could not unregsiter STREAMS device, err = %d", err);
	return;
}

module_init(zapinit);
module_exit(zapexit);
