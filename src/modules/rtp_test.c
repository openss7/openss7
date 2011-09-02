/*****************************************************************************

 @(#) $RCSfile: rtp_test.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:54 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2011-09-02 08:46:54 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: rtp_test.c,v $
 Revision 1.1.2.2  2011-09-02 08:46:54  brian
 - sync up lots of repo and build changes from git

 Revision 1.1.2.1  2011-01-11 23:40:11  brian
 - added new files

 *****************************************************************************/

static char const ident[] = "$RCSfile: rtp_test.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:54 $";

/** @file
  * This is the RTP-TEST module.  This is a pushable STREAMS module that can be
  * pushed over a UDP multiplex Stream to provide for testing of large scale RTP
  * media connections.
  */

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

#include <sys/os7/compat.h>

#include <sys/npi.h>
#include <sys/npi_ip.h>
#include <sys/npi_udp.h>

#define RTPT_DESCRIP	"RTP-TEST STREAMS MODULE."
#define RTPT_REVISION	"OpenSS7 $RCSfile: rtp_test.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:54 $"
#define RTPT_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limitied.  All Rights Reserved."
#define RTPT_DEVICE	"Provides OpenSS7 RTP Test driver."
#define RTPT_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define RTPT_LICENSE	"GPL"
#define RTPT_BANNER	RTPT_DESCRIP	"\n" \
			RTPT_REVISION	"\n" \
			RTPT_COPYRIGHT	"\n" \
			RTPT_DEVICE	"\n" \
			RTPT_CONTACT	"\n"
#define RTPT_SPLASH	RTPT_DEVICE	" - " \
			RTPT_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(RTPT_CONTACT);
MODULE_DESCRIPTION(RTPT_DESCRIP);
MODULE_SUPPORTED_DEVICE(RTPT_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(RTPT_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-rtp-test");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#define RTPT_MOD_ID	CONFIG_STREAMS_RTPT_MODID
#define RPTP_MOD_NAME	CONFIG_STREAMS_RTPT_NAME

#ifndef RTPT_MOD_NAME
#define RPTP_MOD_NAME	"rtp-test"
#endif				/* RTPT_MOD_NAME */

#ifndef RTPT_MOD_ID
#define RTPT_MOD_ID	0
#endif				/* RTPT_MOD_ID */

/*
 *  STREAMS DEFINITIONS
 */

#define MOD_ID		RTPT_MOD_ID
#define MOD_NAME	RTPT_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	RTPT_BANNER
#else				/* MODULE */
#define MOD_BANNER	RTPT_SPLASH
#endif				/* MODULE */

static struct module_info rtpt_minfo = {
	.mi_idnum = MOD_ID,	/* Module ID number */
	.mi_idname = MOD_NAME,	/* Module name */
	.mi_minpsz = STRMINPSZ,	/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,	/* Max packet size accepted */
	.mi_hiwat = STRHIGH,	/* Hi water mark */
	.mi_lowat = STRLOW,	/* Lo water mark */
};

static struct module_stat rtpt_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat rtpt_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 * PRIVATE STRUCTURE
 */

struct st {
	int i_state;			/* interface state */
	void *token;			/* token for lower level */
};

struct rtp_pair;

struct rtp {
	struct rtp_pair *pair;
	struct rtp *other;
	queue_t *oq;
	struct st state, oldstate;
	uint reqflags;
	mblk_t *tick;
	ulong interval;
	struct {
		struct rtp_config config;
		struct rtp_notify notify;
		struct rtp_statem statem;
		struct rtp_stats statsp;
		struct rtp_stats stats;
	} rtp;
};

struct rtp_pair {
	struct rtp r_priv;
	struct rtp w_priv;
};

#define PRIV(q)		((struct rtp_pair *)(q)->q_ptr)
#define MX_PRIV(q)	(((q)->q_flag & QREADR) ? &PRIV(q)->r_priv : &PRIV(q)->w_priv)

#define STRLOGERR	0	/* log error information */
#define STRLOGNO	0	/* log notice information */
#define STRLOGST	1	/* log state transitions */
#define STRLOGTO	2	/* log timeouts */
#define STRLOGRX	3	/* log primitives received */
#define STRLOGTX	4	/* log primitives issued */
#define STRLOGTE	5	/* log timer events */
#define STRLOGIO	6	/* log additional data */
#define STRLOGDA	7	/* log data */

/** rtpt_iocname: display RTP-TEST ioctl command name
  * @cmd: ioctl command
  */
static const char *
rtpt_iocname(int cmd)
{
	switch (_IOC_NR(cmd)) {
	case _IOC_NR(RTPT_IOCGCONFIG):
		return ("RTPT_IOCGCONFIG");
	case _IOC_NR(RTPT_IOCSCONFIG):
		return ("RTPT_IOCSCONFIG");
	case _IOC_NR(RTPT_IOCTCONFIG):
		return ("RTPT_IOCTCONFIG");
	case _IOC_NR(RTPT_IOCCCONFIG):
		return ("RTPT_IOCCCONFIG");
	case _IOC_NR(RTPT_IOCGSTATEM):
		return ("RTPT_IOCGSTATEM");
	case _IOC_NR(RTPT_IOCCMRESET):
		return ("RTPT_IOCCMRESET");
	case _IOC_NR(RTPT_IOCGSTATSP):
		return ("RTPT_IOCGSTATSP");
	case _IOC_NR(RTPT_IOCSSTATSP):
		return ("RTPT_IOCSSTATSP");
	case _IOC_NR(RTPT_IOCGSTATS):
		return ("RTPT_IOCGSTATS");
	case _IOC_NR(RTPT_IOCSSTATS):
		return ("RTPT_IOCSSTATS");
	case _IOC_NR(RTPT_IOCGNOTIFY):
		return ("RTPT_IOCGNOTIFY");
	case _IOC_NR(RTPT_IOCSNOTIFY):
		return ("RTPT_IOCSNOTIFY");
	case _IOC_NR(RTPT_IOCCNOTIFY):
		return ("RTPT_IOCCNOTIFY");
	case _IOC_NR(RTPT_IOCCMGMT):
		return ("RTPT_IOCCMGMT");
	default:
		return ("(unknown ioctl)");
	}
}

/** np_primname: display NPI primitive name
  * @prim: the primitive to display
  */
static const char *
np_primname(np_ulong prim)
{
	switch (prim) {
	case N_CONN_REQ:
		return ("N_CONN_REQ");
	case N_CONN_RES:
		return ("N_CONN_RES");
	case N_DISCON_REQ:
		return ("N_DISCON_REQ");
	case N_DATA_REQ:
		return ("N_DATA_REQ");
	case N_EXDATA_REQ:
		return ("N_EXDATA_REQ");
	case N_INFO_REQ:
		return ("N_INFO_REQ");
	case N_BIND_REQ:
		return ("N_BIND_REQ");
	case N_UNBIND_REQ:
		return ("N_UNBIND_REQ");
	case N_UNITDATA_REQ:
		return ("N_UNITDATA_REQ");
	case N_OPTMGMT_REQ:
		return ("N_OPTMGMT_REQ");
	case N_CONN_IND:
		return ("N_CONN_IND");
	case N_CONN_CON:
		return ("N_CONN_CON");
	case N_DISCON_IND:
		return ("N_DISCON_IND");
	case N_DATA_IND:
		return ("N_DATA_IND");
	case N_EXDATA_IND:
		return ("N_EXDATA_IND");
	case N_INFO_ACK:
		return ("N_INFO_ACK");
	case N_BIND_ACK:
		return ("N_BIND_ACK");
	case N_ERROR_ACK:
		return ("N_ERROR_ACK");
	case N_OK_ACK:
		return ("N_OK_ACK");
	case N_UNITDATA_IND:
		return ("N_UNITDATA_IND");
	case N_UDERROR_IND:
		return ("N_UDERROR_IND");
	case N_DATACK_REQ:
		return ("N_DATACK_REQ");
	case N_DATACK_IND:
		return ("N_DATACK_IND");
	case N_RESET_REQ:
		return ("N_RESET_REQ");
	case N_RESET_IND:
		return ("N_RESET_IND");
	case N_RESET_RES:
		return ("N_RESET_RES");
	case N_RESET_CON:
		return ("N_RESET_CON");
	default:
		return ("(unknown primitive)");
	}
}

/** np_statename: display NPI state name
  * @state: the state value to display
  */
static const char *
np_statename(long state)
{
	switch (state) {
	case NS_UNBND:
		return ("NS_UNBND");
	case NS_WACK_BREQ:
		return ("NS_WACK_BREQ");
	case NS_WACK_UREQ:
		return ("NS_WACK_UREQ");
	case NS_IDLE:
		return ("NS_IDLE");
	case NS_WACK_OPTREQ:
		return ("NS_WACK_OPTREQ");
	case NS_WACK_RRES:
		return ("NS_WACK_RRES");
	case NS_WCON_CREQ:
		return ("NS_WCON_CREQ");
	case NS_WRES_CIND:
		return ("NS_WRES_CIND");
	case NS_WACK_CRES:
		return ("NS_WACK_CRES");
	case NS_DATA_XFER:
		return ("NS_DATA_XFER");
	case NS_WCON_RREQ:
		return ("NS_WCON_RREQ");
	case NS_WRES_RIND:
		return ("NS_WRES_RIND");
	case NS_WACK_DREQ6:
		return ("NS_WACK_DREQ6");
	case NS_WACK_DREQ7:
		return ("NS_WACK_DREQ7");
	case NS_WACK_DREQ9:
		return ("NS_WACK_DREQ9");
	case NS_WACK_DREQ10:
		return ("NS_WACK_DREQ10");
	case NS_WACK_DREQ11:
		return ("NS_WACK_DREQ11");
	case NS_NOSTATES:
		return ("NS_NOSTATES");
	default:
		return ("NS_????");
	}
}

/** rtpt_get_i_state: - get state for private structure
  * @priv: private structure
  * @t: token value
  */
static int
rtpt_get_i_state(struct rtpt *priv, struct rtp_tag *t)
{
	t = rtpt_lookup_priv(priv, t);
	if (t == NULL)
		return NS_UNBND;
	return t->i_state;
}

/** rtpt_set_i_state: - set state for private structure
  * @priv: private structure
  * @t: token value
  * @newstate: new state
  */
static int
rtpt_set_i_state(struct rtpt *priv, struct rtp_tag *t, int newstate)
{
	int oldstate;

	t = rtpt_lookup_priv(priv, t);
	if (t != NULL) {
		if (newstate != (oldstate = t->i_state)) {
			t->i_state = newstate;
			mi_strlog(priv->oq, STRLOGST, SL_TRACE, "%s <- %s",
				  rtpt_statename(newstate), rtpt_statename(oldstate));
		}
	} else {
		newstate = NS_UNBND;
	}
	return (newstate);
}

/** rtpt_save_i_state: - 
  */
static int
rtpt_save_i_state(struct rtpt *priv, struct rtp_tag *t)
{
	t = rtpt_lookup_priv(priv, t);
	if (t != NULL) {
		priv->oldstate.i_state = t->i_state;
	} else {
		priv->oldstate.i_state = NS_UNBND;
	}
	return priv->oldstate.i_state;
}

/** rtpt_restore_i_state: - 
  */
static int
rtpt_restore_i_state(struct rtpt *priv, struct rtp_tag *t)
{
	t = rtpt_lookup_priv(priv, t);
	if (t != NULL) {
		t->i_state = priv->oldstate.i_state;
	}
	return priv->oldstate.i_state;
}

/** rtpt_error_ack: - issue N_ERROR_ACK primitive
  * @priv: private structure
  * @t: connection tag
  * @q: active queue
  * @msg: message to free upon success
  * @prim: primitive in error
  * @error: error number
  *
  * Only called by rtpt_error_reply(), but then, it can be invoked by the
  * handling procedure for just about any N-primitive passed down through the
  * module.  State is restored by falling back to the last checkpoint state.
  */
static int
rtpt_error_ack(struct rtpt *priv, struct rtp_tag *t, queue_t *q, mblk_t *msg, int prim, int error)
{
	N_error_ack_t *p;
	mblk_t *p;

	if (likely(! !(mp = mi_allocb(q, sizeof(t) + sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		*(struct rtp_tag **) mp->b_wptr = t;
		mp->b_wptr += sizeof(t);
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_ERROR_ACK;
		p->ERROR_prim = prim;
		p->NPI_error = error < 0 ? NSYSERR : error;
		p->UNIX_error = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(priv->oq, STRLOGTX, SL_TRACE, "<- N_ERROR_ACK");
		putnext(priv->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/** rtpt_error_reply: - reply with an error
  * @priv: private structure
  * @t: connection tag
  * @q: active queue
  * @msg: message to free upon success
  * @prim: primitive in error
  * @error: error number
  *
  * This is essentially the same as rtpt_error_ack() except that typical queue
  * return codes are filtered and returned directly.  rtpt_error_reply() and an
  * rtpt_error_ack() can be invoked by any N-primitive being issued in the
  * downward direction.
  */
static int
rtpt_error_reply(struct rtpt *priv, struct rtp_tag *t, queue_t *q, mblk_t *msg, int prim, int error)
{
	if (unlikely(error <= 0)) {
		switch (error) {
		case 0:
		case -EBUSY:
		case -ENOBUFS:
		case -ENOMEM:
		case -EAGAIN:
		case -EDEADLK:
			return (error);
		default:
			break;
		}
	}
	return rtpt_error_ack(priv, t, q, msg, prim, error);
}

/** rtpt_error_discard: - discard in error
  * @priv: private structure
  * @t: connection tag
  * @q: active queue
  * @msg: message to free upon success
  * @prim: primitive in error
  * @error: error number
  */
static int
rtpt_error_discard(struct rtpt *priv, struct rtp_tag *t, queue_t *q, mblk_t *msg, int prim,
		   int error)
{
	if (unlikely(error <= 0)) {
		switch (error) {
		case 0:
		case -EBUSY:
		case -ENOBUFS:
		case -ENOMEM:
		case -EAGAIN:
		case -EDEADLK:
			return (error);
		default:
			break;
		}
	}
	freemsg(msg);
	mi_strlog(priv->oq, STRLOGIO, SL_TRACE, "Discard %s, error %d", np_primname(prim), error);
	return (0);
}

/** rtpt_conn_req: - process N_CONN_REQ primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  *
  * When a connection request goes by, simply change state.  There is no need to
  * remember the destination address because it will be associated with the
  * connection.
  */
static int
rtpt_conn_req(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_conn_req_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (rtpt_get_i_state(priv, t) != NS_IDLE)
		goto outstate;
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto ebusy;
	rtpt_set_i_state(priv, t, NS_WCON_CREQ);
	putnext(q, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      error:
	return rtpt_error_reply(priv, q, mp, N_CONN_REQ, err);
}

/** rtpt_conn_res: - process N_CONN_RES primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  *
  * When a connection response goes by, simply change state.  There is no need
  * to remember the responding address because it will be associatedw tih the
  * connection.
  */
static int
rtpt_conn_res(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_conn_res_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (rtpt_get_i_state(priv, t) != NS_WRES_CIND)
		goto outstate;
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto ebusy;
	rtpt_set_i_state(priv, t, NS_WACK_CRES);
	putnext(q, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      error:
	return rtpt_error_reply(priv, q, mp, N_CONN_RES, err);
}

/** rtpt_discon_req: - process N_DISCON_REQ primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_discon_req(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_discon_req_t *p;
	struct rtp_tag *t;
	int err, state;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	switch (state = rtpt_get_i_state(priv, t)) {
	case NS_WCON_CREQ:
	case NS_WACK_CRES:
	case NS_DATA_XFER:
	case NS_WCON_RREQ:
	case NS_WRES_RIND:
		break;
	default:
		goto outstate;
	}
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto ebusy;
	switch (state) {
	case NS_WCON_CREQ:
		state = NS_WACK_DREQ6;
		break;
	case NS_WACK_CRES:
		state = NS_WACK_DREQ7;
		break;
	case NS_DATA_XFER:
		state = NS_WACK_DREQ9;
		break;
	case NS_WCON_RREQ:
		state = NS_WACK_DREQ10;
		break;
	case NS_WRES_RIND:
		state = NS_WACK_DREQ11;
		break;
	}
	rtpt_set_i_state(priv, t, state);
	putnext(q, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      error:
	return rtpt_error_reply(priv, q, mp, N_DISCON_REQ, err);
}

/** rtpt_data_req: - process N_DATA_REQ primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  *
  * In fitting with the purpose of this module (testing many RTP media streams),
  * there are two forms of data request:
  *
  * 1. The RTP media stream tag is specified and corresponds to a existing RTP
  *    stream in the data transfer state.
  *
  * 2. The RTP media stream tag is NULL.  In this case, the RTP packet is to be
  *    replicated across all RTP media streams in the data transfer state.
  */
static int
rtpt_data_req(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_data_req_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (t != NULL) {
		if (rtpt_get_i_state(priv, t) != NS_DATA_XFER)
			goto outstate;
		if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
			goto ebusy;
		*(void **) mp->b_rptr = t->token;
		putnext(q, mp);
		return (0);
	} else {
		t = priv->last_stream;
		if (t == NULL)
			t = priv->first_stream;
		for (t = priv->last_stream; t != NULL; t = t->next) {
			if ((dp = dupmsg(mp)) == NULL)
				goto enobufs;
			if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, dp->b_band)) {
				freemsg(dp);
				goto ebusy;
			}
			*(void **) dp->b_rptr = t->token;
			putnext(q, dp);
		}
		freemsg(mp);
		return (0);
	}
      enobufs:
	err = -ENOBUFS;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      error:
	return rtpt_error_reply(priv, q, mp, N_DATA_REQ, err);
}

/** rtpt_exdata_req: - process N_EXDATA_REQ primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_exdata_req(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_exdata_req_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (unlikely((t = rtpt_lookup_priv(priv, t)) == NULL))
		goto einval;
	if (rtpt_get_i_state(priv, t) != NS_DATA_XFER)
		goto outstate;
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto ebusy;
	*(void **) mp->b_rptr = t->token;
	putnext(q, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	return rtpt_error_reply(priv, q, mp, N_EXDATA_REQ, err);
}

/** rtpt_info_req: - process N_INFO_REQ primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_info_req(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_info_req_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto ebusy;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (unlikely((t = rtpt_lookup_priv(priv, t)) == NULL))
		goto einval;
	*(void **) mp->b_rptr = t->token;
	putnext(q, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      error:
	return rtpt_error_reply(priv, q, mp, N_INFO_REQ, err);
}

/** rtpt_bind_req: - process N_BIND_REQ primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_bind_req(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_bind_req_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (unlikely((t = rtpt_lookup_priv(priv, t)) == NULL))
		goto einval;
	if (rtpt_get_i_state(priv, t) != NS_UNBND)
		goto outstate;
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto ebusy;
	rtpt_set_i_state(priv, t, NS_WACK_BREQ);
	*(void **) mp->b_rptr = t->token;
	putnext(q, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      error:
	return rtpt_error_reply(priv, q, mp, N_BIND_REQ, err);
}

/** rtpt_unbind_req: - process N_UNBIND_REQ primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_unbind_req(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_unbind_req_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (rtpt_get_i_state(priv, t) != NS_IDLE)
		goto outstate;
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto ebusy;
	rtpt_set_i_state(priv, t, NS_WACK_UREQ);
	*(void **) mp->b_rptr = t->token;
	putnext(q, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      error:
	return rtpt_error_reply(priv, q, mp, N_UNBIND_REQ, err);
}

/** rtpt_unitdata_req: - process N_UNITDATA_REQ primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  *
  * The N_UNIDATA_REQ is not the normal way of using this module, so we pass
  * them through transparently.
  */
static int
rtpt_unitdata_req(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_unitdata_req_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (rtpt_get_i_state(priv, t) != NS_IDLE)
		goto outstate;
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto ebusy;
	*(void **) mp->b_rptr = t->token;
	putnext(q, mp);
	return (0);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	return rtpt_error_reply(priv, q, mp, N_UNITDATA_REQ, err);
}

/** rtpt_optmgmt_req: - process N_OPTMGMT_REQ primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  *
  * Just pass option management requests through.
  */
static int
rtpt_optmgmt_req(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_optmgmt_req_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto ebusy;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (rtpt_get_i_state(priv, t) == NS_IDLE)
		rtpt_set_i_state(priv, t, NS_WACK_OPTREQ);
	*(void **) mp->b_rptr = t->token;
	putnext(q, mp);
	return (0);
      einval:
	err = -EINVAL;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	return rtpt_error_reply(priv, q, mp, N_OPTMGMT_REQ, err);
}

/** rtpt_conn_ind: - process N_CONN_IND primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  *
  * We do not really expect to receive these.  However, in pseudo-connection-
  * oriented mode we might create a connection indication on an idle (bound)
  * connection that was bound with a number of connection indications greater
  * than zero.  Just track the state and pass them through.
  */
static int
rtpt_conn_ind(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_conn_ind_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	switch (rtpt_get_i_state(priv, t)) {
	case NS_IDLE:
	case NS_WRES_CIND:
		if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
			goto ebusy;
		/* FIXME: */
		rtpt_set_i_state(priv, t, NS_WRES_CIND);
		*(void **) mp->b_rptr = t->user;
		putnext(q, mp);
		return (0);
	case NS_WACK_CRES:
		/* put it back */
		goto eagain;
	default:
		goto oustate;
	}
      einval:
	err = -EINVAL;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      eagain:
	err = -EAGAIN;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return rtpt_error_discard(priv, q, mp, N_CONN_IND, err);
}

/** rtpt_conn_con: - process N_CONN_CON primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  *
  * Use these to track connection state.  There is no need to save any
  * information other than current state.
  */
static int
rtpt_conn_con(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_conn_con_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (rtpt_get_i_state(priv, t) != NS_WCON_CREQ)
		goto outstate;
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto ebusy;
	rtpt_set_i_state(priv, t, NS_DATA_XFER);
	*(void **) mp->b_rptr = t->user;
	putnext(q, mp);
	return (0);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	return rtpt_error_discard(priv, q, mp, N_CONN_CON, err);
}

/** rtpt_discon_ind: - process N_DISCON_IND primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  *
  * Use these to track connection state.  We are not really expecting to receive
  * these.  The other end does not disconnect on UDP.  Errors are indicated as
  * reset indications.  Nevertheless, make the motions.
  */
static int
rtpt_discon_ind(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_discon_ind_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto ebusy;
	switch (rtpt_get_i_state(priv, t)) {
	case NS_WCON_CREQ:
	case NS_WACK_RRES:
	case NS_WRES_RIND:
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		rtpt_set_i_state(priv, t, NS_IDLE);
		break;
	default:
		goto outstate;
	}
	*(void **) mp->b_rptr = t->user;
	putnext(q, mp);
	return (0);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	return rtpt_error_discard(priv, q, mp, N_DISCON_IND, err);
}

/** rtpt_data_ind: - process N_DATA_IND primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_data_ind(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	switch (rtpt_get_i_state(priv, t))
	{
	case NS_DATA_XFER:
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
	case NS_WRES_RIND:
	case NS_WACK_RRES:
	case NS_WCON_RREQ:
		break;
	default:
		goto outstate;
	}
	t = rtpt_lookup_priv(priv, t);
	if (t != NULL)
		t->rx_packets++;
	freemsg(mp);
	return (0);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return rtpt_error_discard(priv, q, mp, N_DATA_IND, err);
}

/** rtpt_exdata_ind: - process N_EXDATA_IND primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_exdata_ind(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_exdata_ind_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (rtpt_get_i_state(priv, t) != NS_DATA_XFER)
		goto outstate;
	/* Discard these */
	freemsg(mp);
	return (0);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return rtpt_error_discard(priv, q, mp, N_EXDATA_IND, err);
}

/** rtpt_info_ack: - process N_INFO_ACK primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_info_ack(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_info_ack_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto ebusy;
	*(void **) mp->b_rptr = t->user;
	putnext(q, mp);
	return (0);
      einval:
	err = -EINVAL;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	return rtpt_error_discard(priv, q, mp, N_INFO_ACK, err);
}

/** rtpt_bind_ack: - process N_BIND_ACK primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_bind_ack(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_bind_ack_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (rtpt_get_i_state(priv, t) != NS_WACK_BREQ)
		goto outstate;
	rtpt_set_i_state(priv, t, NS_IDLE);
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto ebusy;
	/* FIXME: copy the bound address from the response into the state structure for the RTP
	   media stream. */
	*(void **) mp->b_rptr = t->user;
	putnext(q, mp);
	return (0);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      ebusy:
	err = -EBUSY;
	goto error;
      error:
	return rtpt_error_discard(priv, q, mp, N_BIND_ACK, err);
}

/** rtpt_error_ack: - process N_ERROR_ACK primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_error_ack(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_error_ack_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	switch (rtpt_get_i_state(priv, t)) {
	case NS_WACK_BREQ:
		rtpt_set_i_state(priv, t, NS_UNBND);
		break;
	case NS_WACK_UREQ:
		rtpt_set_i_state(priv, t, NS_IDLE);
		break;
	case NS_WACK_OPTREQ:
		rtpt_set_i_state(priv, t, NS_IDLE);
		break;
	case NS_WACK_CRES:
		rtpt_set_i_state(priv, t, NS_WRES_CIND);
		break;
	case NS_WCON_CREQ:
		rtpt_set_i_state(priv, t, NS_IDLE);
		break;
	case NS_WACK_DREQ6:
		rtpt_set_i_state(priv, t, NS_WCON_CREQ);
		break;
	case NS_WACK_DREQ7:
		rtpt_set_i_state(priv, t, NS_WRES_CIND);
		break;
	case NS_WACK_DREQ9:
		rtpt_set_i_state(priv, t, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ10:
		rtpt_set_i_state(priv, t, NS_WCON_RREQ);
		break;
	case NS_WACK_DREQ11:
		rtpt_set_i_state(priv, t, NS_WRES_RIND);
		break;
	}
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto ebusy;
	*(void **) mp->b_rptr = t->user;
	putnext(q, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      error:
	return rtpt_error_discard(priv, q, mp, N_ERROR_ACK, err);
}

/** rtpt_ok_ack: - process N_OK_ACK primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_ok_ack(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_ok_ack_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	switch (rtpt_get_i_state(priv, t)) {
	case NS_WACK_BREQ:
		rtpt_set_i_state(priv, t, NS_IDLE);
		break;
	case NS_WACK_UREQ:
		rtpt_set_i_state(priv, t, NS_UNBND);
		break;
	case NS_WACK_OPTREQ:
		rtpt_set_i_state(priv, t, NS_IDLE);
		break;
	case NS_WACK_CRES:
		rtpt_set_i_state(priv, t, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		rtpt_set_i_state(priv, t, NS_IDLE);
		break;
	}
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto ebusy;
	*(void **) mp->b_rptr = t->user;
	putnext(q, mp);
	return (0);
      ebusy:
	err = -EBUSY;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      error:
	return rtpt_error_discard(priv, q, mp, N_OK_ACK, err);
}

/** rtpt_unitdata_ind: - process N_UNITDATA_IND primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_unitdata_ind(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_unitdata_ind_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (rtpt_get_i_state(priv, t) != NS_DATA_XFER)
		goto outstate;
	/* The application does NOT want to see these.  FIXME: provide statistics collection,
	   though. */
	freemsg(mp);
	return (0);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return rtpt_error_discard(priv, q, mp, N_UNITDATA_IND, err);
}

/** rtpt_uderror_ind: - process N_UDERROR_IND primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_uderror_ind(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_uderror_ind_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (rtpt_get_i_state(priv, t) != NS_DATA_XFER)
		goto outstate;
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto busy;
	/* The application might want to see these, particularly if there is a configuration
	   problem and we are sending packets and receiving ICMP errors for every packet. */
	*(void **) mp->b_rptr = t->user;
	putnext(q, mp);
	return (0);
      busy:
	err = -EBUSY;
	goto error;
      oustate:
	err = NOUTSTATE;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      error:
	return rtpt_error_discard(priv, q, mp, N_UDERROR_IND, err);
}

/** rtpt_datack_req: - process N_DATACK_REQ primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_datack_req(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_datack_req_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	goto notsupp;
      notsupp:
	err = NNOTSUPPORT;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      error:
	return rtpt_reply_error(priv, q, mp, N_DATACK_REQ, err);
}

/** rtpt_datack_ind: - process N_DATACK_IND primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_datack_ind(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	/* Just plain discard these. */
	freemsg(mp);
	return (0);
}

/** rtpt_reset_req: - process N_RESET_REQ primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_reset_req(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_reset_req_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (rtpt_get_i_state(priv, t) != NS_DATA_XFER)
		goto outstate;
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto busy;
	rtpt_set_i_state(priv, t, NS_WCON_RREQ);
	*(void **) mp->b_rptr = t->token;
	putnext(q, mp);
	return (0);
      busy:
	err = -EBUSY;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return rtpt_error_reply(priv, q, mp, N_RESET_REQ, err);
}

/** rtpt_reset_ind: - process N_RESET_IND primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_reset_ind(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_reset_ind_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (rtpt_get_i_state(priv, t) != NS_DATA_XFER)
		goto outstate;
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto busy;
	rtpt_set_i_state(priv, t, NS_WRES_RIND);
	/* FIXME: we actually don't want to pass these along (if we ever do get them), but respond
	   immediately with the reset response.  This is because the upper interface does not want
	   to handle data nor reset indications. */
	*(void **) mp->b_rptr = t->user;
	putnext(q, mp);
	return (0);
      busy:
	err = -EBUSY;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return rtpt_error_discard(priv, q, mp, N_RESET_IND, err);
}

/** rtpt_reset_res: - process N_RESET_RES primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_reset_res(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_reset_res_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (rtpt_get_i_state(priv, t) != NS_WRES_RIND)
		goto outstate;
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto busy;
	rtpt_set_i_state(priv, t, NS_WACK_RRES);
	*(void **) mp->b_rptr = t->token;
	putnext(q, mp);
	return (0);
      busy:
	err = -EBUSY;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return rtpt_error_reply(priv, q, mp, N_RESET_RES, err);
}

/** rtpt_reset_con: - process N_RESET_CON primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_reset_con(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	N_reset_con_t *p;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	t = *(struct rtp_tag **) mp->b_rptr;
	if (rtpt_get_i_state(priv, t) != NS_WCON_RREQ)
		goto outstate;
	if (!pcmsg(DB_TYPE(mp)) && !bcanputnext(q, mp->b_band))
		goto ebusy;
	rtpt_set_i_state(priv, t, NS_DATA_XFER);
	*(void **) mp->b_rptr = t->user;
	putnext(q, mp);
	return (0);
      busy:
	err = -EBUSY;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      error:
	return rtpt_error_discard(priv, q, mp, N_RESET_CON, err);
}

/** rtpt_other_req: - process N_????_REQ primitive
  * @priv: private structure
  * @q: active queue
  * @mp: the primitive
  */
static int
rtpt_other_req(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	np_ulong *p = (typeof(p)) mp->b_rptr;
	struct rtp_tag *t;
	int err;

	if (!MBLKIN(mp, sizeof(t), sizeof(*p)))
		goto einval;
	goto notsupp;
      notsupp:
	err = NNOTSUPPORT;
	goto error;
      einval:
	err = -EINVAL;
	goto error;
      error:
	return rtpt_error_reply(priv, q, mp, *p, err);
}

/*
 *  =========================================================================
 *
 *  INPUT-OUTPUT CONTROLS
 *
 *  =========================================================================
 */

/** rtpt_testconfig: - test RTP-TEST configuration for validity
  * @priv: (locked) private structure
  * @arg: configuration to test
  */
static int
rtpt_testconfig(struct rtpt *priv, struct rtpt_config *arg)
{
	int err = 0;

	/* FIXME: test arg */
	return (err);
}

#define RTPT_EVT_ALL	    0

/** rtpt_setnotify: - set RTP-TEST notification bits
  * @priv: (locked) private structure
  * @arg: notification arguments
  */
static int
rtpt_setnotify(struct rtpt *priv, struct rtpt_notify *arg)
{
	if (arg->events & ~(RTPT_EVT_ALL))
		return (EINVAL);
	arg->events = arg->events | priv->rtpt.notify.events;
	return (0);
}

/** rtpt_clrnotify: - clear RTP-TEST notification bits
  * @priv: (locked) private structure
  * @arg: notification arguments
  */
static int
rtpt_clrnotify(struct rtpt *priv, struct rtpt_notify *arg)
{
	if (arg->events & ~(RTPT_EVT_ALL))
		return (EINVAL);
	arg->events = ~arg->events & priv->rtpt.notify.events;
	return (0);
}

/** rtpt_manage: - perform RTP-TEST management action
  * @priv: (locked) private structure
  * @arg: management arguments
  */
static int
rtpt_manage(struct rtpt *priv, struct rtpt_mgmt *arg)
{
	switch (arg->cmd) {
	case RTPT_MGMT_RESET:
		/* FIXME: reset */
		break;
	default:
		return (EINVAL);
	}
	return (0);
}

/** rtpt_ioctl: - process RTP-TEST M_IOCTL message
  * @priv: (locked) private structure
  * @q: active queue
  * @mp: the M_IOCTL message
  *
  * This is step 1 of the RTP-TEST input-output control operation.  Step 1
  * consists of a copyin operation for SET operations and a copyout operation
  * for GET operations.
  */
static int
rtpt_ioctl(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *bp;
	int size = -1;
	int err = 0;

	mi_strlog(priv->oq, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", rtpt_iocname(ioc->ioc_cmd));

	rtpt_save_total_state(priv);

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(RTPT_IOCGCONFIG):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(priv->rtpt.config), false)))
			goto enobufs;
		bcopy(&priv->rtpt.config, bp->b_rptr, sizeof(priv->rtpt.config));
		break;
	case _IOC_NR(RTPT_IOCSCONFIG):
		size = sizeof(priv->rtpt.config);
		break;
	case _IOC_NR(RTPT_IOCTCONFIG):
		size = sizeof(priv->rtpt.config);
		break;
	case _IOC_NR(RTPT_IOCCCONFIG):
		size = sizeof(priv->rtpt.config);
		break;
	case _IOC_NR(RTPT_IOCGSTATEM):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(priv->rtpt.state), false)))
			goto enobufs;
		bcopy(&priv->rtpt.statem, bp->b_rptr, sizeof(priv->rtpt.statem));
		break;
	case _IOC_NR(RTPT_IOCCMRESET):
		/* FIXME: reset the state machine */
		break;
	case _IOC_NR(RTPT_IOCGSTATSP):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(priv->rtpt.statsp), false)))
			goto enobufs;
		bcopy(&priv->rtpt.statsp, bp->b_rptr, sizeof(priv->rtpt.statsp));
		break;
	case _IOC_NR(RTPT_IOCSSTATSP):
		size = sizeof(priv - rtpt.statsp);
		break;
	case _IOC_NR(RTPT_IOCGSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(priv->rtpt.stats), false)))
			goto enobufs;
		bcopy(&priv->rtpt.stats, bp->b_rptr, sizeof(priv->rtpt.stats));
		break;
	case _IOC_NR(RTPT_IOCCSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(priv->rtpt.stats), false)))
			goto enobufs;
		bcopy(&priv->rtpt.stats, bp->b_rptr, sizeof(priv->rtpt.stats));
		bzero(&priv->rtpt.stats, sizeof(priv->rtpt.stats));
		break;
	case _IOC_NR(RTPT_IOCGNOTIFY):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(priv->rtpt.notify), false)))
			goto enobufs;
		bcopy(&priv->rtpt.notify, bp->b_rptr, sizeof(priv->rtpt.notify));
		break;
	case _IOC_NR(RTPT_IOCSNOTIFY):
		size = sizeof(priv->rtpt.notify);
		break;
	case _IOC_NR(RTPT_IOCCNOTIFY):
		size = sizeof(priv->rtpt.notify);
		break;
	case _IOC_NR(RTPT_IOCCMGMT):
		size = sizeof(struct rtpt_mgmt);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0) {
		rtpt_restore_total_state(priv);
		return (err);
	}
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == 0)
			mi_copy_done(q, mp, 0);
		else if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
}

/** rtpt_copyin: - process RTP-TEST M_IOCDATA message
  * @priv: (locked) private structure
  * @q: active queue
  * @mp: the M_IOCDATA message
  * @dp: data part
  *
  * This is step number 2 for SET operations.  This is the result of the
  * implicit or explicit copyin operation.  We can now perform sets and commits.
  * All SET operations also include a last copyout step that copies out the
  * information actually set.
  */
static int
rtpt_copyin(struct rtpt *priv, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, MBLKL(dp), false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, MBLKL(dp));
	mi_strlog(priv->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", rtpt_iocname(cp->cp_cmd));
	rtpt_save_total_state(priv);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(RTPT_IOCSCONFIG):
		if ((err = rtpt_testconfig(priv, (struct rtpt_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &priv->rptp.config, sizeof(priv->rtpt.config));
		break;
	case _IOC_NR(RTPT_IOCTCONFIG):
		err = rtpt_testconfig(priv, (struct rtpt_config *) bp->b_rptr);
		break;
	case _IOC_NR(RTPT_IOCCCONFIG):
		if ((err = rtpt_testconfig(priv, (struct rtpt_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &priv->rtpt.config, sizeof(priv->rtpt.config));
		break;
	case _IOC_NR(RTPT_IOCSSTATP):
		bcopy(bp->b_rptr, &priv->rtpt.statsp, sizeof(priv->rtpt.statsp));
		break;
	case _IOC_NR(RTPT_IOCSNOTIFY):
		if ((err = rtpt_setnotify(priv, (struct rtpt_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &priv->rtpt.notify, sizeof(priv->rtpt.notify));
		break;
	case _IOC_NR(RTPT_IOCCNOTIFY):
		err = rtpt_manage(priv, (struct rtpt_mgmt *) bp->b_rptr);
		break;
	case _IOC_NR(RTPT_IOCCMGMT):
		break;
	default:
		err = EPROTO;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0) {
		rtpt_restore_total_state(priv);
		return (err);
	}
	if (err < 0) {
		rtpt_restore_total_state(priv);
		return (err);
	}
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
}

/** rtpt_copyout: - process RTP-TEST M_IOCDATA message
  * @priv: (locked) private structure
  * @q: active queue
  * @mp: the M_IOCDATA message
  * @dp: data part
  *
  * This is the final step which is a simply copy done operation.
  */
static int
rtpt_copyout(struct rtpt *priv, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	mi_strlog(priv->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", rtpt_iocname(cp->cp_cmd));
	mi_copyout(q, mp);
	return (0);
}

/** rtpt_do_ioctl: - process M_IOCTL message
  * @priv: (locked) private structure
  * @q: active queue
  * @mp: the M_IOCTL message
  *
  * This is step 1 of the input-output control operation.  Step 1 consists of a
  * copyin operation for SET operations and a copyout operation for GET
  * oeprations.
  */
static int
rtpt_do_ioctl(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*ioc))) || unlikely(mp->b_cond == NULL)) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case RTPT_IOC_MAGIC:
		return rtpt_ioctl(priv, q, mp);
	default:
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

/** rtpt_do_copyin: - process M_IOCDATA message
  * @priv: (locked) private structure
  * @q: active queue
  * @mp: the M_IOCDATA message
  * @dp: data part
  *
  * This is step number 2 for SET operations.  This is the result of the
  * implicit or explicit copyin operation.  We can now perform sets and commits.
  * All Set operations also include a last copyout step that copies out the
  * information actually set.
  */
static int
rtpt_do_copyin(struct rtpt *priv, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*cp))) || unlikely(mp->b_cont == NULL)) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	switch (_IOC_TYPE(cp->cp_cmd)) {
	case RTPT_IOC_MAGIC:
		return rtpt_copyin(priv, q, mp, dp);
	default:
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

/*
 *  STREAMS MESSAGE HANDLING
 */

/** __rtpt_m_data: - process M_DATA message
  * @priv: (locked) private structure
  * @q: active queue
  * @mp: the M_DATA message
  */
static inline fastcall int
__rtpt_m_data(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	return rtpt_data(priv, q, NULL, 0, mp);
}

/** rtpt_m_data: - process M_DATA message
  * @q: active queue
  * @mp: the M_DATA message
  */
static inline fastcall int
rtpt_m_data(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if (likely((priv = mi_trylock(q)) != NULL)) {
		err = __rtpt_m_data(RTPT_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

/** rtpt_m_proto_slow: - process M_(PC)PROTO message
  * @priv: (locked) private structure
  * @q: active queue
  * @mp: the M_(PC)PROTO message
  * @prim: the primitive
  */
static noinline fastcall int
rtpt_m_proto_slow(struct rtpt *priv, queue_t *q, mblk_t *mp, np_ulong prim)
{
	int rtn;

	switch (prim) {
	case N_DATA_REQ:
		mi_strlog(priv->oq, STRLOGDA, SL_TRACE, "-> %s", np_primname(prim));
		break;
	default:
		mi_strlog(priv->oq, STRLOGRX, SL_TRACE, "-> %s", np_primname(prim));
		break;
	}

	rtpt_save_total_state(priv);

	switch (prim) {
	case N_CONN_REQ:
		rtn = rtpt_conn_req(priv, q, mp);
		break;
	case N_CONN_RES:
		rtn = rtpt_conn_res(priv, q, mp);
		break;
	case N_DISCON_REQ:
		rtn = rtpt_discon_req(priv, q, mp);
		break;
	case N_DATA_REQ:
		rtn = rtpt_data_req(priv, q, mp);
		break;
	case N_EXDATA_REQ:
		rtn = rtpt_exdata_req(priv, q, mp);
		break;
	case N_INFO_REQ:
		rtn = rtpt_info_req(priv, q, mp);
		break;
	case N_BIND_REQ:
		rtn = rtpt_bind_req(priv, q, mp);
		break;
	case N_UNBIND_REQ:
		rtn = rtpt_unbind_req(priv, q, mp);
		break;
	case N_UNITDATA_REQ:
		rtn = rtpt_unitdata_req(priv, q, mp);
		break;
	case N_OPTMGMT_REQ:
		rtn = rtpt_optmgmt_req(priv, q, mp);
		break;
	case N_CONN_IND:
		rtn = rtpt_conn_ind(priv, q, mp);
		break;
	case N_CONN_CON:
		rtn = rtpt_conn_con(priv, q, mp);
		break;
	case N_DISCON_IND:
		rtn = rtpt_discon_ind(priv, q, mp);
		break;
	case N_DATA_IND:
		rtn = rtpt_data_ind(priv, q, mp);
		break;
	case N_EXDATA_IND:
		rtn = rtpt_exdata_ind(priv, q, mp);
		break;
	case N_INFO_ACK:
		rtn = rtpt_info_ack(priv, q, mp);
		break;
	case N_BIND_ACK:
		rtn = rtpt_bind_ack(priv, q, mp);
		break;
	case N_ERROR_ACK:
		rtn = rtpt_error_ack(priv, q, mp);
		break;
	case N_OK_ACK:
		rtn = rtpt_ok_ack(priv, q, mp);
		break;
	case N_UNITDATA_IND:
		rtn = rtpt_unitdata_ind(priv, q, mp);
		break;
	case N_UDERROR_IND:
		rtn = rtpt_uderror_ind(priv, q, mp);
		break;
	case N_DATACK_REQ:
		rtn = rtpt_datack_req(priv, q, mp);
		break;
	case N_DATACK_IND:
		rtn = rtpt_datack_ind(priv, q, mp);
		break;
	case N_RESET_REQ:
		rtn = rtpt_reset_req(priv, q, mp);
		break;
	case N_RESET_IND:
		rtn = rtpt_reset_ind(priv, q, mp);
		break;
	case N_RESET_RES:
		rtn = rtpt_reset_res(priv, q, mp);
		break;
	case N_RESET_CON:
		rtn = rtpt_reset_con(priv, q, mp);
		break;
	default:
		rtn = rtpt_other_req(priv, q, mp);
		break;
	}
	if (rtn)
		rtpt_restore_total_state(priv);
	return (rtn);
}

/** __rtpt_m_proto: - process M_(PC)PROTO message
  * @priv: locked private structure
  * @q: active queue
  * @mp: the M_(PC)PROTO message
  */
static inline fastcall int
__rtpt_m_proto(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	np_ulong prim;
	int rtn;

	if (unlikely(MBLKIN(mp, sizeof(struct rtp_tag *), sizeof(prim)))) {
		prim = *(typeof(prim) *) (mp->b_rptr + sizeof(struct rtp_tag *));

		if (likely(prim == N_DATA_REQ)) {
#ifndef _OPTIMIZE_SPEED
			mi_strlog(priv->oq, STRLOGDA, SL_TRACE, "-> N_DATA_REQ");
#endif				/* _OPTIMIZE_SPEED */
			rtn = rtpt_data_req(priv, q, mp);
		} else {
			rtn = rtpt_m_proto_slow(priv, q, mp, prim);
		}
	} else {
		freemsg(mp);
		rtn = 0;
	}
	return (rtn);
}

/** rtpt_m_proto: - process M_(PC)PROTO message
  * @q: active queue
  * @mp: the M_(PC)PROTO message
  */
static inline fastcall int
rtpt_m_proto(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if (likely((priv = mi_trylock(q)) != NULL)) {
		err = __rtpt_m_proto(RTPT_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

/** __rtpt_m_sig: - process M_(PC)SIG message
  * @priv: (locked) private structure
  * @q: active queue
  * @mp: the M_(PC)SIG message
  *
  * This is the method for processing tick timers.  Under the tick approach,
  * each time that the tick timer fires we reset the timer.  Then we prepare as
  * many blocks as will fit in the interval and send them to the other side.
  */

/** rtpt_m_sig: - process M_(PC)SIG message
  * @q: active queue
  * @mp: the M_(PC)SIG message
  */
static inline fastcall int
rtpt_m_sig(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if (likely((priv = mi_trylock(q)) != NULL)) {
		err = __rtpt_m_sig(RTPT_PRIV(q), q, mp);
		mi_unlock(priv);
	} else
		err = mi_timer_requeue(mp) ? -EAGAIN : 0;
	return (err);
}

/** rtpt_m_flush: - process M_FLUSH message
  * @q: active queue
  * @mp: the M_FLUSH message
  *
  * Avoid having to push pipemod.  When we are the bottommost module over a pipe
  * twist then perform the actions of pipemod.  This means that the rtp-test
  * module must be pushed over the same side of a pipe as pipemod, if pipemod is
  * pushed at all.
  */
static fastcall int
rtpt_m_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHALL);
	}
	if (!SAMESTR(q)) {
		switch (mp->b_rptr[0] & FLUSHRW) {
		case FLUSHW:
			mp->b_rptr[0] &= ~FLUSHW;
			mp->b_rptr[0] |= FLUSHR;
			break;
		case FLUSHR:
			mp->b_rptr[0] &= ~FLUSHR;
			mp->b_rptr[0] |= FLUSHW;
			break;
		}
	}
	putnext(q, mp);
	return (0);
}

static fastcall int
__rtpt_m_ioctl(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	int err;

	err = rtpt_do_ioctl(priv, q, mp);
	if (err <= 0)
		return (err);
	mi_copy_done(q, mp, err);
	return (0);
}

/** rtpt_m_ioctl: - process M_IOCTL message
  * @q: active queue
  * @mp: the M_IOCTL message
  */
static fastcall int
rtpt_m_ioctl(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if ((priv = mi_trylock(q)) != NULL) {
		err = __rtpt_m_ioctl(RTPT_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

static fastcall int
__rtpt_m_iocdata(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	mblk_t *dp;
	int err;

	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		err = 0;
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		err = rtpt_do_copyin(priv, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		err = rtpt_do_copyout(priv, q, mp, dp);
		break;
	default:
		err = EPROTO;
		break;
	}
	if (err <= 0)
		return (err);
	mi_copy_done(q, mp, err);
	return (0);
}

/** rtpt_m_iocdata: - process M_IOCDATA message
  * @q: active queue
  * @mp: the M_IOCDATA message
  */
static fastcall int
rtpt_m_iocdata(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if ((priv = mi_trylock(q)) != NULL) {
		err = __rtpt_m_iocdata(RTPT_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

/** rtpt_m_rse: - process M_(PC)RSE message
  * @q: active queue
  * @mp: the M_(PC)RSE message
  */
static fastcall int
rtpt_m_rse(queue_t *q, mblk_t *mp)
{
	/* simply discard it */
	freemsg(mp);
	return (0);
}

/** rtpt_m_other: - process other STREAMS message
  * @q: active queue
  * @mp: other STREAMS message
  *
  * Simply pass unrecognized messages along.
  */
static fastcall int
rtpt_m_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/** rptp_msg_slow: - process STREAMS message, slow path
  * @q: active queue
  * @mp: the STREAMS message
  *
  * This is the slow version of the STREAMS message handling.  It is expected
  * that data is delivered in M_DATA message blocks intead of N_DATA_IND or
  * N_DATA_REQ message blocks.  Nevertheless, if this slower function gets
  * called, it is more likely because we have an M_PROTO message block
  * containing an N_DATA_REQ.
  */
static noinline fastcall int
rtpt_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return rtpt_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return rtpt_m_sig(q, mp);
	case M_IOCTL:
		return rtpt_m_ioctl(q, mp);
	case M_IOCDATA:
		return rtpt_m_iocdata(q, mp);
	case M_FLUSH:
		return rtpt_m_flush(q, mp);
	case M_RSE:
	case M_PCRSE:
		return rtpt_m_rse(q, mp);
	default:
		return rtpt_m_other(q, mp);
	case M_DATA:
		return rtpt_m_data(q, mp);
	}
}

/** rtpt_msg: - process STREAMS message
  * @q: active queue
  * @mp: the message
  *
  * This function returns zero when the message has been absorbed.  When it
  * returns non-zero, the message is to be placed (back) on the queue.
  */
static inline fastcall int
rtpt_msg(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return rtpt_m_data(q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return rtpt_m_proto(q, mp);
	return rtpt_msg_slow(q, mp);
}

/** __rtpt_msg_slow: - process STREAMS message, slow path
  * @priv: locked private structure
  * @q: active queue
  * @mp: the STREAMS message
  */
static noinline fastcall int
__rtpt_msg_slow(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __rtpt_m_proto(priv, q, mp);
	case M_SIG:
	case M_PCSIG:
		return __rtpt_m_sig(priv, q, mp);
	case M_IOCTL:
		return __rtpt_m_ioctl(priv, q, mp);
	case M_IOCDATA:
		return __rtpt_m_iocdata(priv, q, mp);
	case M_FLUSH:
		return __rtpt_m_flush(q, mp);
	case M_RSE:
	case M_PCRSE:
		return __rtpt_m_rse(q, mp);
	default:
		return __rtpt_m_other(q, mp);
	case M_DATA:
		return __rtpt_m_data(q, mp);
	}
}

/** __rtpt_msg: - process STREAMS message locked
  * @priv: locked private structure pointer
  * @q: active queue
  * @mp: the message
  */
static inline fastcall int
__rtpt_msg(struct rtpt *priv, queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return __rtpt_m_data(priv, q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return __rtpt_m_proto(priv, q, mp);
	return __rtpt_msg_slow(priv, q, mp);
}

/*
 *  STREAMS QUEUE PUT AND SERVICE PROCEDURES
 */

/** rtpt_putp: - canonical put procedure
  * @q: active queue
  * @mp: message to put
  *
  * Quick canoncial put procedure.
  */
static streamscall __hot_in int
rtpt_putp(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || rtpt_msg(q, mp)) {
		if (!putq(q, mp)) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
}

/** rtpt_srvp: canonical service procedure
  * @q: active queue
  *
  * Quick canonical service procedure.  This is a little quicker for processing
  * bulked messages because it takes the lock once for the entire group of
  * M_DATA messages, instead of once for each message.
  */
static streamscall __hot_read int
rtpt_srvp(queue_t *q)
{
	mblk_t *mp;

	if (likely((mp = getq(q)) != NULL)) {
		caddr_t priv;

		if (likely((priv = mi_trylock(q)) != NULL)) {
			do {
				if (unlikely(__rtpt_msg(RTPT_PRIV(q), q, mp) != 0))
					break;
			} while (likely((mp = getq(q)) != NULL));
			mi_unlock(priv);
		}
		if (unlikely(mp != NULL))
			putbq(q, mp);
	}
	return (0);
}

/*
 *  STREAMS QUEUE OPEN AND CLOSE ROUTINES
 */

static caddr_t rtpt_opens = NULL;

/** rtpt_qopen: - module queue open procedure
  * @q: read queue of queue pair
  * @devp: device number of driver
  * @oflags: flags to open(2) call
  * @sflag: STREAMS flag
  * @crp: credentials of opening process
  */
static streamscall int
rtpt_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct rtpt *priv;
	mblk_t *tick;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if ((tick = mi_timer_alloc(0)) == NULL)
		return (ENOBUFS);
	if ((err = mi_open_comm(&rtpt_opens, sizeof(*priv), q, devp, oflags, sflag, crp))) {
		mi_timer_free(tick);
		return (err);
	}
	priv = PRIV(q);
	bzero(priv, sizeof(*priv));

	/* initialize the structure */
	priv->r_priv.pair = priv;
	priv->r_priv.other = &priv->w_priv;
	priv->r_priv.oq = WR(q);
	priv->r_priv.state.i_state = NS_UNBND;
	priv->r_priv.oldstate.i_state = NS_UNBND;
	priv->r_priv.state.i_flags = 0;
	priv->r_priv.oldstate.i_flags = 0;

	priv->r_priv.reqflags = 0;
	priv->r_priv.tick = tick;
	priv->r_priv.interval = 10;	/* milliseconds */

	priv->w_priv.pair = priv;
	priv->w_priv.other = &priv->r_priv;
	priv->w_priv.oq = q;
	priv->w_priv.state.i_state = NS_UNBND;
	priv->w_priv.oldstate.i_state = NS_UNBND;
	priv->w_priv.state.i_flags = 0;
	priv->w_priv.oldstate.i_flags = 0;

	priv->w_priv.reqflags = 0;
	priv->w_priv.tick = tick;
	priv->w_priv.interval = 10;

	qprocson(q);
	return (0);
}

/** rtpt_qclose: - module queue close procedure
  * @q: queue pair
  * @oflags: flags to open(2) call
  * @crp: credentials of closing process
  */
static streamscall int
rtpt_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct rtpt *priv = PRIV(q);

	qprocsoff(q);
	mi_timer_free(rp->ticks);
	mi_close_comm(&rtpt_opens, q);
	return (0);
}

/*
 *  REGISTRATION AND INITIALIZATION
 */

static struct qinit rtpt_rinit = {
	.qi_putp = rtpt_putp,	/* Read put (message from below) */
	.qi_srvp = rtpt_srvp,	/* Read queue service */
	.qi_qopen = rtpt_qopen,	/* Each open */
	.qi_qclose = rtpt_qclose,	/* Last close */
	.qi_minfo = &rtpt_minfo,	/* Information */
	.qi_mstat = &rtpt_rstat,	/* Statistics */
};

static struct qinit rtpt_winit = {
	.qi_putp = rtpt_putp,	/* write put (message from above) */
	.qi_srvp = rtpt_srvp,	/* write queue service */
	.qi_minfo = &rtpt_minfo,	/* Information */
	.qi_mstat = &rtpt_wstat,	/* Statistics */
};

static struct streamtab rtptinfo = {
	.st_rdinit = &rtpt_rinit,	/* upper read queue */
	.st_wrinit = &rtpt_winit,	/* lower write queue */
};

#ifdef LINUX
/*
 *  LINUX INITIALIZATION
 */
unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for RTP-TEST module. (0 for allocation.)");

static struct fmodsw rtpt_fmod = {
	.f_name = MOD_NAME,
	.f_str = &rtptinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

/** rtptinit: - intialize RTP-TEST
  */
static __init int
rptpinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&rtpt_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

/** rtptexit: - terminate RTP-TEST
  */
static __exit void
rtptexit(void)
{
	int err;

	if ((err = unregister_strmod(&rtpt_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregster module", MOD_NAME);
		return;
	}
	return;
}

module_init(rtptinit);
module_exit(rtptexit);

#endif				/* LINUX */
