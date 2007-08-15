/*****************************************************************************

 @(#) $RCSfile: sl_dl.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2007/08/15 05:14:12 $

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

 Last Modified $Date: 2007/08/15 05:14:12 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sl_dl.c,v $
 Revision 0.9.2.5  2007/08/15 05:14:12  brian
 - GPLv3 updates

 Revision 0.9.2.4  2007/08/14 08:34:11  brian
 - GPLv3 header update

 Revision 0.9.2.3  2007/07/14 01:33:45  brian
 - make license explicit, add documentation

 Revision 0.9.2.2  2007/03/25 18:59:07  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.1  2006/10/27 22:50:40  brian
 - working up modules and testsuite

 *****************************************************************************/

#ident "@(#) $RCSfile: sl_dl.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2007/08/15 05:14:12 $"

static char const ident[] =
    "$RCSfile: sl_dl.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2007/08/15 05:14:12 $";

/*
 * This is a simple SL to DL conversion module that converst between the
 * OpenSS7 sli(7) (Signalling link Interface) and the dlpi(7) (Data Link
 * Provder Interface) in case someone is more interested in using the later.
 */

#define _MPS_SOURCE
#define _LFS_SOURCE

#include <sys/os7/compat.h>

#define SL_DL_DESCRIP		"SIGNALLING LINK (SL) DATA LINK (DL) STREAMS MODULE"
#define SL_DL_REVISION		"OpenSS7 $RCSfile: sl_dl.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2007/08/15 05:14:12 $"

#define SL_DL_COPYRIGHT		"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define SL_DL_DEVICE		"Part of the OpenSS7 Stack for Linux Fast STREAMS"
#define SL_DL_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define SL_DL_LICENSE		"GPL"
#define SL_DL_BANNER		SL_DL_DESCRIP		"\n" \
				SL_DL_REVISION	"\n" \
				SL_DL_COPYRIGHT	"\n" \
				SL_DL_DEVICE		"\n" \
				SL_DL_CONTACT		"\n"
#define SL_DL_SPLASH		SL_DL_DEVICE		" - " \
				SL_DL_REVISION	"\n"

#ifdef LINUX
MODULE_DESCRIPTION(SL_DL_DESCRIP);
MODULE_AUTHOR(SL_DL_CONTACT);
MODULE_SUPPORTED_DEVICE(SL_DL_DEVICE);
MODULE_LICENSE(SL_DL_LICENSE);
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-sl_dl");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define SL_DL_MOD_ID	CONFIG_STREAMS_SL_DL_MODID
#define SL_DL_MOD_NAME	CONFIG_STREAMS_SL_DL_NAME
#endif

#define MOD_ID		SL_DL_MOD_ID
#define MOD_NAME	SL_DL_MOD_NAME
#ifdef MODULE
#define MOD_SPLASH	SL_DL_BANNER
#else				/* MODULE */
#define MOD_SPLASH	SL_DL_SPLASH
#endif				/* MODULE */

/*
 *  Private Structure
 */
struct sl {
	lmi_info_ack_t info;		/* information */
	lmi_ulong ppa;
};

struct dl {
	dl_info_ack_t info;		/* information */
	dl_qos_conn_ss7_t qos;
	dl_qos_range_ss7_t qor;
} dl;

struct priv {
	queue_t *rq;			/* read queue */
	queue_t *wq;			/* write queue */
	spinlock_t lock;		/* structure lock */
	struct sl sl;			/* signalling link structure */
	struct dl dl;			/* data link structure */
};

STATIC streams_fastcall mblk_t *
sl_allocb(queue_t *q, size_t size, int pri)
{
	mblk_t *mp;

	if (likely(mp = allocb(size, pri)))
		return (mp);
	mi_bufcall(q, size, pri);
	return (NULL);
}

#define dl_allocb(q,size,pri) sl_allocb(q,size,pri)

#define DLF_UNATTACHED		(1<<DL_UNATTACHED)
#define DLF_ATTACH_PENDING	(1<<DL_ATTACH_PENDING)
#define DLF_DETACH_PENDING	(1<<DL_DETACH_PENDING)
#define DLF_UNBOUND		(1<<DL_UNBOUND)
#define DLF_BIND_PENDING	(1<<DL_BIND_PENDING)
#define DLF_UNBIND_PENDING	(1<<DL_UNBIND_PENDING)
#define DLF_IDLE		(1<<DL_IDLE)
#define DLF_UDQOS_PENDING	(1<<DL_UDQOS_PENDING)
#define DLF_OUTCON_PENDING	(1<<DL_OUTCON_PENDING)
#define DLF_INCON_PENDING	(1<<DL_INCON_PENDING)
#define DLF_CONN_RES_PENDING	(1<<DL_CONN_RES_PENDING)
#define DLF_DATAXFER		(1<<DL_DATAXFER)
#define DLF_USER_RESET_PENDING	(1<<DL_USER_RESET_PENDING)
#define DLF_PROV_RESET_PENDING	(1<<DL_PROV_RESET_PENDING)
#define DLF_RESET_RES_PENDING	(1<<DL_RESET_RES_PENDING)
#define DLF_DISCON8_PENDING	(1<<DL_DISCON8_PENDING)
#define DLF_DISCON9_PENDING	(1<<DL_DISCON9_PENDING)
#define DLF_DISCON11_PENDING	(1<<DL_DISCON11_PENDING)
#define DLF_DISCON12_PENDING	(1<<DL_DISCON12_PENDING)
#define DLF_DISCON13_PENDING	(1<<DL_DISCON13_PENDING)
#define DLF_SUBS_BIND_PND	(1<<DL_SUBS_BIND_PND)
#define DLF_SUBS_UNBIND_PND	(1<<DL_SUBS_UNBIND_PND)
#define DLM_NOTPENDING		(DLF_UNATTACHED |\
				 DLF_UNBOUND |\
				 DLF_IDLE |\
				 DLF_DATAXFER)

/* Data Link state functions. */
STATIC streams_fastcall streams_inline dl_ulong
dl_get_state(struct priv *dl)
{
	return dl->dl.info.dl_current_state;
}
STATIC streams_fastcall streams_inline dl_ulong
dl_get_statef(struct priv *dl)
{
	return (1 << dl_get_state(dl));
}
STATIC streams_fastcall streams_inline void
dl_set_state(struct priv *dl, dl_ulong newstate)
{
	dl->dl.info.dl_current_state = newstate;
}
STATIC streams_fastcall streams_inline void
dl_state_lock(struct priv *dl)
{
	spin_lock_bh(&dl->lock);
}
STATIC streams_fastcall streams_inline void
dl_state_unlock(struct priv *dl)
{
	spin_unlock_bh(&dl->lock);
}
STATIC streams_fastcall streams_inline int
dl_lock_state(struct priv *dl, dl_ulong mask)
{
	dl_state_lock(dl);
	if (dl_get_statef(dl) & mask)
		return (true);
	dl_state_unlock(dl);
	return (false);
}
STATIC streams_fastcall streams_inline void
dl_unlock_state(struct priv *dl, dl_ulong newstate)
{
	dl_set_state(dl, newstate);
	dl_state_unlock(dl);
}

/* Signalling Link state functions. */
STATIC streams_fastcall streams_inline lmi_ulong
sl_get_state(struct priv *dl)
{
	return dl->sl.info.lmi_current_state;
}
STATIC streams_fastcall streams_inline lmi_ulong
sl_get_statef(struct priv * dl)
{
	return (1 << sl_get_state(dl));
}
STATIC streams_fastcall streams_inline void
sl_set_state(struct priv *dl, lmi_ulong newstate)
{
	dl->sl.info.lmi_current_state = newstate;
}
STATIC streams_fastcall streams_inline void
sl_state_lock(struct priv *dl)
{
	spin_lock_bh(&dl->lock);
}
STATIC streams_fastcall streams_inline void
sl_state_unlock(struct priv *dl)
{
	spin_unlock_bh(&dl->lock);
}
STATIC streams_fastcall streams_inline int
sl_lock_state(struct priv *dl, lmi_ulong mask)
{
	sl_state_lock(dl);
	if (sl_get_statef(dl) & mask)
		return (true);
	sl_state_unlock(dl);
	return (false);
}
STATIC streams_fastcall streams_inline void
sl_unlock_state(struct priv *dl, lmi_ulong newstate)
{
	sl_set_state(dl, newstate);
	sl_state_unlock(dl);
}

/*
 * DLPI Provider Messages.
 *
 * These are messages that are sent upstream to the DLPI user.
 */
/**
 * dl_info_ack: - send a DL_INFO_ACK upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_info_ack(struct priv *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl->info) + sizeof(dl->qos) + sizeof(dl->qor);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		/* copy directly from private structure */
		bcopy(&dl->info, mp->b_wptr, size);
		mp->b_wptr += size;
		printd(("%s: %p: <- DL_INFO_ACK\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_bind_ack: - send a DL_BIND_ACK upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_bind_ack(struct priv *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_bind_ack_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_bind_ack_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_BIND_ACK;
		p->dl_sap = 0;
		p->dl_addr_length = 0;
		p->dl_addr_offset = 0;
		p->dl_max_conind = 0;
		p->dl_xidtest_flg = 0;
		printd(("%s: %p: <- DL_BIND_ACK\n", MOD_NAME, dl));
		dl_set_state(dl, DL_IDLE);
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_ok_ack: - send a DL_OK_ACK upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 * @dl_correct_primitive: primitive to ack
 */
STATIC streams_fastcall int
dl_ok_ack(struct priv *dl, queue_t *q, mblk_t *msg, dl_ulong dl_correct_primitive)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_ok_ack_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_ok_ack_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PCPROTO;
		p->dl_primitive = DL_OK_ACK;
		p->dl_correct_primitive = dl_correct_primitive;
		switch (dl_correct_primitive) {
		case DL_ATTACH_REQ:
		case DL_UNBIND_REQ:
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_DETACH_REQ:
			dl_set_state(dl, DL_UNATTACHED);
			break;
		case DL_CONNECT_RES:
			dl_set_state(dl, DL_DATAXFER);	/* XXX */
			break;
		case DL_RESET_RES:
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_DISCONNECT_REQ:
			switch (dl_get_state(dl)) {
			case DL_OUTCON_PENDING:
				dl_set_state(dl, DL_DISCON8_PENDING);
				break;
			case DL_INCON_PENDING:
				dl_set_state(dl, DL_DISCON9_PENDING);
				break;
			case DL_DATAXFER:
				dl_set_state(dl, DL_DISCON11_PENDING);
				break;
			case DL_USER_RESET_PENDING:
				dl_set_state(dl, DL_DISCON12_PENDING);
				break;
			case DL_PROV_RESET_PENDING:
				dl_set_state(dl, DL_DISCON13_PENDING);
				break;
			}
			break;
		case DL_SUBS_UNBIND_REQ:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_PROMISCON_REQ:
		case DL_ENABMULTI_REQ:
		case DL_DISABMULTI_REQ:
		case DL_PROMISCOFF_REQ:
			break;
		default:
			swerr();
			break;
		}
		printd(("%s: %p: <- DL_OK_ACK\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_error_ack: - send a DL_ERROR_ACK upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_error_ack(struct priv *dl, queue_t *q, mblk_t *msg, dl_ulong dl_error_primitive,
	     dl_ulong dl_errno, dl_ulong dl_unix_errno)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_error_ack_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_error_ack_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PCPROTO;
		p->dl_primitive = DL_ERROR_ACK;
		p->dl_error_primitive = dl_error_primitive;
		p->dl_errno = dl_errno;
		p->dl_unix_errno = dl_unix_errno;
		switch (dl_error_primitive) {
		case DL_ATTACH_REQ:
			if (dl_get_state(dl) == DL_ATTACH_PENDING)
				dl_set_state(dl, DL_UNATTACHED);
			break;
		case DL_UNBIND_REQ:
			if (dl_get_state(dl) == DL_UNBIND_PENDING)
				dl_set_state(dl, DL_IDLE);
			break;
		case DL_DETACH_REQ:
			if (dl_get_state(dl) == DL_DETACH_PENDING)
				dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_CONNECT_RES:
			if (dl_get_state(dl) == DL_CONN_RES_PENDING)
				dl_set_state(dl, DL_INCON_PENDING);
			break;
		case DL_RESET_RES:
			if (dl_get_state(dl) = DL_RESET_RES_PENDING)
				dl_set_state(dl, DL_PROV_RESET_PENDING);
			break;
		case DL_DISCONNECT_REQ:
			switch (dl_get_state(dl)) {
			case DL_DISCON8_PENDING:
				dl_set_state(dl, DL_OUTCON_PENDING);
				break;
			case DL_DISCON9_PENDING:
				dl_set_state(dl, DL_INCON_PENDING);
				break;
			case DL_DISCON11_PENDING:
				dl_set_state(dl, DL_DATAXFER);
				break;
			case DL_DISCON12_PENDING:
				dl_set_state(dl, DL_USER_RESET_PENDING);
				break;
			case DL_DISCON13_PENDING:
				dl_set_state(dl, DL_PROV_RESET_PENDING);
				break;
			default:
				break;
			}
			break;
		case DL_SUBS_UNBIND_REQ:
			if (dl_get_state(dl) == DL_SUBS_UNBIND_PND)
				dl_set_state(dl, DL_IDLE);
			break;
		case DL_PROMISCON_REQ:
		case DL_ENABMULTI_REQ:
		case DL_PROMISCOFF_REQ:
			break;
		case DL_BIND_REQ:
			if (dl_get_state(dl) == DL_BIND_PENDING)
				dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_SUBS_BIND_REQ:
			if (dl_get_state(dl) == DL_SUBS_BIND_PND)
				dl_set_state(dl, DL_IDLE);
			break;
		case DL_CONNECT_REQ:
			if (dl_get_state(dl) == DL_OUTCON_PENDING)
				dl_set_state(dl, DL_IDLE);
			break;
		case DL_RESET_REQ:
			if (dl_get_state(dl) == DL_USER_RESET_PENDING)
				dl_set_state(dl, DL_DATAXFER);
			break;
		default:
			swerr();
			break;
		}
		printd(("%s: %p: <- DL_ERROR_ACK\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_subs_bind_ack: - send a DL_SUBS_BIND_ACK upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_subs_bind_ack(struct priv *dl, queue_t *q, mblk_t *mp, caddr_t ptr, size_t len)
{
	mblk_t *mp;
	size_t size = sizeof(dl_subs_bind_ack_t) + len;

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_subs_bind_ack_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_SUBS_BIND_ACK;
		bcopy(ptr, mp->b_wptr, len);
		mp->b_wptr += len;
		printd(("%s: %p: <- DL_SUBS_BIND_ACK\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_data_ind: - send a DL_DATA_IND (M_DATA) upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 * @dp: data portion of message
 */
STATIC streams_fastcall streams_inline __hot_read int
dl_data_ind(struct priv *dl, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	if (bcanputnext(dl->rq, msg->b_band)) {
		putnext(dl->rq, dp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * dl_unitdata_ind: - send a DL_UNITDATA_IND upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_unitdata_ind(struct priv *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_unitdata_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_unitdata_ind_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_UNITDATA_IND;
		printd(("%s: %p: <- DL_UNITDATA_IND\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_uderror_ind: - send a DL_UDERROR_IND upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 * @aptr: a pointer to the destination address of the failed unitdata
 * @alen: the length of the destination address
 * @error: the DLPI error number
 * @errno: the UNIX error number
 */
STATIC streams_fastcall int
dl_uderror_ind(struct priv *dl, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen,
	       dl_ulong error, dl_long errno)
{
	mblk_t *mp;
	size_t size = sizeof(dl_uderror_ind_t) + alen;

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_uderror_ind_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_UDERROR_IND;
		p->dl_dest_addr_length = alen;
		p->dl_dest_addr_offset = alen ? sizeof(*p) : 0;
		p->dl_unix_errno = errno < 0 ? -errno : errno;
		p->dl_errno = error;
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		printd(("%s: %p: <- DL_UDERROR_IND\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_connect_ind: - send a DL_CONNECT_IND upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_connect_ind(struct priv *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_connect_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_connect_ind_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_CONNECT_IND;
		printd(("%s: %p: <- DL_CONNECT_IND\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_connect_con: - send a DL_CONNECT_CON upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_connect_con(struct priv *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_connect_con_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_connect_con_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_CONNECT_CON;
		p->dl_resp_addr_offset = 0;
		p->dl_resp_addr_length = 0;
		p->dl_qos_offset = 0;
		p->dl_qos_length = 0;
		p->dl_growth = 0;
		printd(("%s: %p: <- DL_CONNECT_CON\n", MOD_NAME, dl));
		dl_set_state(dl, DL_DATAXFER);
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_token_ack: - send a DL_TOKEN_ACK upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_token_ack(struct priv *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_token_ack_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_token_ack_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_TOKEN_ACK;
		p->dl_token = dl->u.index;
		printd(("%s: %p: <- DL_TOKEN_ACK\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_disconnect_ind: - send a DL_DISCONNECT_IND upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_disconnect_ind(struct priv *dl, queue_t *q, mblk_t *msg, dl_ulong origin, dl_ulong reason)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_disconnect_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_disconnect_ind_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_DISCONNECT_IND;
		p->dl_originator = origin;
		p->dl_reason = reason;
		p->dl_correlation = 0;
		printd(("%s: %p: <- DL_DISCONNECT_IND\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_reset_ind: - send a DL_RESET_IND upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 * @origin: origin of the reset (DL_USER, DL_PROVIDER)
 * @reason: reason for the reset (DL_RESET_FLOW_CONTROL, DL_RESET_LINK_ERROR, DL_RESET_RESYNC)
 *
 * Implements SL_LINK_CONGESTED_IND and SL_LINK_CONGESTION_CEASED_IND.
 */
STATIC streams_fastcall int
dl_reset_ind(struct priv *dl, queue_t *q, mblk_t *msg, dl_ulong origin, dl_ulong reason)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_reset_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_reset_ind_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_RESET_IND;
		p->dl_originator = origin;
		p->dl_reason = reason;
		printd(("%s: %p: <- DL_RESET_IND\n", MOD_NAME, dl));
		dl_set_state(dl, DL_PROV_RESET_PENDING);
		atomic_inc(&dl->resinds);
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_reset_con: - send a DL_RESET_CON upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 *
 * Implements SL_RB_CLEARED_IND.
 */
STATIC streams_fastcall int
dl_reset_con(struct priv *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_reset_con_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_reset_con_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_RESET_CON;
		dl_set_state(dl, DL_DATAXFER);
		printd(("%s: %p: <- DL_RESET_CON\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_data_ack_ind: - send a DL_DATA_ACK_IND upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 * @pri: message priority
 * @dp: data portion
 */
STATIC streams_fastcall int
dl_data_ack_ind(struct priv *dl, queue_t *q, mblk_t *msg, dl_ulong pri, mblk_t *dp)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_data_ack_ind_t);

	if (bcanputnext(dl->rq, 3)) {
		if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
			dl_data_ack_ind_t *p = (typeof(p)) mp->b_wptr++;

			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 3;
			p->dl_primitive = DL_DATA_ACK_IND;
			p->dl_dest_addr_length = 0;
			p->dl_dest_addr_offset = 0;
			p->dl_src_addr_length = 0;
			p->dl_src_addr_offset = 0;
			p->dl_priority = pri;
			p->dl_service_class = DL_RQST_NORSP;
			printd(("%s: %p: <- DL_DATA_ACK_IND\n", MOD_NAME, dl));
			mp->b_cont = dp;
			putnext(dl->rq, mp);
			freemsg(msg);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_data_ack_status_ind: - send a DL_DATA_ACK_STATUS_IND upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_data_ack_status_ind(struct priv *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_data_ack_status_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_data_ack_status_ind_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_DATA_ACK_STATUS_IND;
		printd(("%s: %p: <- DL_DATA_ACK_STATUS_IND\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_reply_ind: - send a DL_REPLY_IND upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_reply_ind(struct priv *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_reply_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_reply_ind_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_REPLY_IND;
		printd(("%s: %p: <- DL_REPLY_IND\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_reply_status_ind: - send a DL_REPLY_STATUS_IND upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_reply_status_ind(struct priv *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_reply_status_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_reply_status_ind_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_REPLY_STATUS_IND;
		printd(("%s: %p: <- DL_REPLY_STATUS_IND\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_reply_update_status_ind: - send a DL_REPLY_UPDATE_STATUS_IND upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_reply_update_status_ind(struct priv *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_reply_update_status_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_reply_update_status_ind_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_REPLY_UPDATE_STATUS_IND;
		printd(("%s: %p: <- DL_REPLY_UPDATE_STATUS_IND\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_xid_ind: - send a DL_XID_IND upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_xid_ind(struct priv *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_xid_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_xid_ind_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_XID_IND;
		printd(("%s: %p: <- DL_XID_IND\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_xid_con: - send a DL_XID_CON upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_xid_con(struct priv *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_xid_con_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_xid_con_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_XID_CON;
		printd(("%s: %p: <- DL_XID_CON\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_test_ind: - send a DL_TEST_IND upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_test_ind(struct priv *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_test_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_test_ind_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_TEST_IND;
		printd(("%s: %p: <- DL_TEST_IND\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_test_con: - send a DL_TEST_CON upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_test_con(struct priv *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_test_con_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_test_con_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->dl_primitive = DL_TEST_CON;
		printd(("%s: %p: <- DL_TEST_CON\n", MOD_NAME, dl));
		putnext(dl->rq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  LM User Primitives.
 *
 *  These primitives are send downstream to the SL provider.
 */
/**
 * lmi_info_req: - send an LMI_INFO_REQ downstream
 * @lmi: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
lm_info_req(struct priv *lmi, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(lmi_info_req_t);

	if ((mp = sl_allocb(lmi, q, size, BPRI_MED))) {
		lmi_info_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->lmi_primitive = LMI_INFO_REQ;
		printd(("%s: %p: LMI_INFO_REQ ->\n", MOD_NAME, lmi));
		putnext(lmi->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lm_attach_req: - send an LMI_ATTACH_REQ downstream
 * @lmi: private structure
 * @q: active queue
 * @msg: message to free on success
 * @ppa: PPA to which to attach
 */
STATIC streams_fastcall int
lm_attach_req(struct priv *lmi, queue_t *q, mblk_t *msg, dl_ulong ppa)
{
	mblk_t *mp;
	const size_t size = sizeof(lmi_attach_req_t) + sizeof(ppa);

	if ((mp = sl_allocb(lmi, q, size, BPRI_MED))) {
		lmi_attach_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->lmi_primitive = LMI_ATTACH_REQ;
		p->lmi_ppa[0] = ppa;
		printd(("%s: %p: LMI_ATTACH_REQ ->\n", MOD_NAME, lmi));
		putnext(lmi->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_detach_req: - send an LMI_DETACH_REQ downstream
 * @lmi: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
lm_detach_req(struct priv *lmi, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(lmi_detach_req_t);

	if ((mp = sl_allocb(lmi, q, size, BPRI_MED))) {
		lmi_detach_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->lmi_primitive = LMI_DETACH_REQ;
		printd(("%s: %p: LMI_DETACH_REQ ->\n", MOD_NAME, lmi));
		putnext(lmi->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_enable_req: - send an LMI_ENABLE_REQ downstream
 * @lmi: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
lm_enable_req(struct priv *lmi, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(lmi_enable_req_t);

	if ((mp = sl_allocb(lmi, q, size, BPRI_MED))) {
		lmi_enable_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->lmi_primitive = LMI_ENABLE_REQ;
		printd(("%s: %p: LMI_ENABLE_REQ ->\n", MOD_NAME, lmi));
		putnext(lmi->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_disable_req: - send an LMI_DISABLE_REQ downstream
 * @lmi: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
lm_disable_req(struct priv *lmi, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(lmi_disable_req_t);

	if ((mp = sl_allocb(lmi, q, size, BPRI_MED))) {
		lmi_disable_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->lmi_primitive = LMI_DISABLE_REQ;
		printd(("%s: %p: LMI_DISABLE_REQ ->\n", MOD_NAME, lmi));
		putnext(lmi->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_optmgmt_req: - send an LMI_OPTMGMT_REQ downstream
 * @lmi: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
lm_optmgmt_req(struct priv *lmi, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(lmi_optmgmt_req_t);

	if ((mp = sl_allocb(lmi, q, size, BPRI_MED))) {
		lmi_optmgmt_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->lmi_primitive = LMI_OPTMGMT_REQ;
		printd(("%s: %p: LMI_OPTMGMT_REQ ->\n", MOD_NAME, lmi));
		putnext(lmi->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL User Primitives.
 *
 *  These primitives are send downstream to the SL provider.
 */
/**
 * sl_pdu_req: - send an SL_PDU_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_pdu_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_pdu_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_pdu_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_PDU_REQ;
		printd(("%s: %p: SL_PDU_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_emergency_req: - send an SL_EMERGENCY_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_emergency_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_emergency_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_emergency_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_EMERGENCY_REQ;
		printd(("%s: %p: SL_EMERGENCY_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_emergency_ceases_req: - send an SL_EMERGENCY_CEASES_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_emergency_ceases_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_emergency_ceases_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_emergency_ceases_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_EMERGENCY_CEASES_REQ;
		printd(("%s: %p: SL_EMERGENCY_CEASES_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_start_req: - send an SL_START_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_start_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_start_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_start_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_START_REQ;
		printd(("%s: %p: SL_START_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_stop_req: - send an SL_STOP_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_stop_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_stop_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_stop_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_STOP_REQ;
		printd(("%s: %p: SL_STOP_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieve_bsnt_req: - send an SL_RETRIEVE_BSNT_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_retrieve_bsnt_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_retrieve_bsnt_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_retrieve_bsnt_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_RETRIEVE_BSNT_REQ;
		printd(("%s: %p: SL_RETRIEVE_BSNT_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieval_request_and_fsnc_req: - send an SL_RETRIEVAL_REQUEST_AND_FSNC_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_retrieval_request_and_fsnc_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_retrieval_and_fsnc_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_retrieval_and_fsnc_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_RETRIEVAL_REQUEST_AND_FSNC_REQ;
		printd(("%s: %p: SL_RETRIEVAL_REQUEST_AND_FSNC_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_clear_buffers_req: - send an SL_CLEAR_BUFFERS_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_clear_buffers_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_clear_buffers_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_clear_buffers_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_CLEAR_BUFFERS_REQ;
		printd(("%s: %p: SL_CLEAR_BUFFERS_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_clear_rtb_req: - send an SL_CLEAR_RTB_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_clear_rtb_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_clear_rtb_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_clear_rtb_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_CLEAR_RTB_REQ;
		printd(("%s: %p: SL_CLEAR_RTB_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_continue_req: - send an SL_CONTINUE_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_continue_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_continue_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_continue_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_CONTINUE_REQ;
		printd(("%s: %p: SL_CONTINUE_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_local_processor_outage_req: - send an SL_LOCAL_PROCESSOR_OUTAGE_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_local_processor_outage_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_local_proc_outage_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_local_proc_outage_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_REQ;
		printd(("%s: %p: SL_LOCAL_PROCESSOR_OUTAGE_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_resume_req: - send an SL_RESUME_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_resume_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_resume_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_resume_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_RESUME_REQ;
		printd(("%s: %p: SL_RESUME_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_congestion_discard_req: - send an SL_CONGESTION_DISCARD_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_congestion_discard_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_cong_discard_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_cong_discard_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_CONGESTION_DISCARD_REQ;
		printd(("%s: %p: SL_CONGESTION_DISCARD_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_congestion_accept_req: - send an SL_CONGESTION_ACCEPT_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_congestion_accept_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_cong_accept_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_cong_accept_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_CONGESTION_ACCEPT_REQ;
		printd(("%s: %p: SL_CONGESTION_ACCEPT_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_no_congestion_req: - send an SL_NO_CONGESTION_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_no_congestion_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_no_cong_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_no_cong_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_NO_CONGESTION_REQ;
		printd(("%s: %p: SL_NO_CONGESTION_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_power_on_req: - send an SL_POWER_ON_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_power_on_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_power_on_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_power_on_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_POWER_ON_REQ;
		printd(("%s: %p: SL_POWER_ON_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_optmgmt_req: - send an SL_OPTMGMT_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_optmgmt_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_optmgmt_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_optmgmt_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_OPTMGMT_REQ;
		printd(("%s: %p: SL_OPTMGMT_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_notify_req: - send an SL_NOTIFY_REQ downstream
 * @sl: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC streams_fastcall int
sl_notify_req(struct priv *sl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(sl_notify_req_t);

	if ((mp = sl_allocb(sl, q, size, BPRI_MED))) {
		sl_notify_req_t *p = (typeof(p)) mp->b_wptr++;

		mp->b_datap->db_type = M_PROTO;
		p->sl_primitive = SL_NOTIFY_REQ;
		printd(("%s: %p: SL_NOTIFY_REQ ->\n", MOD_NAME, sl));
		putnext(sl->wq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_info_req: - process DL_INFO_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_info_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_info_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return dl_info_ack(dl, q, mp);
      badprim:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_attach_req: - process DL_ATTACH_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * Currently this is implemented as a pushable module.  At a later date we
 * might make this also a multiplexing driver and link SCTP NPI streams under
 * the multiplexing driver.  Then when a DL stream is opened on the
 * multiplexing driver, it will need to attach to an SCTP NPI stream beneath.
 * Linked streams will be assigned a PPA and the dl_ppa in this primitive
 * identifies the stream to which to link.  For now, there are no streams in
 * the DL_UNATTACHED state and this primitive will always return DL_OUTSTATE.
 */
STATIC streams_fastcall int
dl_attach_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_attach_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (dl->dl.info.dl_provider_style != DL_STYLE2)
		goto notsupported;
	if (!dl_lock_state(dl, DLF_UNATTACHED))
		goto outstate;
	dl_set_state(dl, DL_ATTACH_PENDING);
	dl_unlock_state(dl);
	/* get SL to do the attach */
	return lm_attach_req(dl, q, mp, p->dl_ppa);
      oustate:
	return dl_error_ack(dl, q, mp, DL_ATTACH_REQ, DL_OUTSTATE, EPROTO);
      notsupported:
	return dl_error_ack(dl, q, mp, DL_ATTACH_REQ, DL_NOTSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(dl, q, mp, DL_ATTACH_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_bind_req: - process DL_BIND_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_bind_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_bind_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!dl_lock_state(dl, DLF_UNBOUND))
		goto outstate;
	dl_set_state(dl, DL_BIND_PENDING);
	dl_unlock_state(dl);
	/* ignore dl_sap */
	/* ignore dl_max_conind */
	if (p->dl_service_mode != DL_CODLS)
		goto unsupported;
	/* modules cannot have connection management streams (we are it) */
	if (p->dl_conn_mgmt != 0)
		goto bound;
	if ((p->dl_xidtest_flg & (DL_AUTO_XID | DL_AUTO_TEST)) == (DL_AUTO_XID | DL_AUTO_TEST))
		goto noauto;
	if ((p->dl_xidtest_flg & DL_AUTO_XID))
		goto noxidauto;
	if ((p->dl_xidtest_flg & DL_AUTO_TEST))
		goto notestauto;
	return lm_enable_req(dl, q, mp);
      notestauto:
	return dl_error_ack(dl, q, mp, DL_BIND_REQ, DL_NOTESTAUTO, EOPNOTSUPP);
      noxidauto:
	return dl_error_ack(dl, q, mp, DL_BIND_REQ, DL_NOXIDAUTO, EOPNOTSUPP);
      noauto:
	return dl_error_ack(dl, q, mp, DL_BIND_REQ, DL_NOAUTO, EOPNOTSUPP);
      bound:
	return dl_error_ack(dl, q, mp, DL_BIND_REQ, DL_BOUND, EADDRBUSY);
      unsupported:
	return dl_error_ack(dl, q, mp, DL_BIND_REQ, DL_UNSUPPORTED, EOPNOTSUPP);
      oustate:
	return dl_error_ack(dl, q, mp, DL_BIND_REQ, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(dl, q, mp, DL_BIND_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_unbind_req: - process DL_UNBIND_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * Implements SL_POWER_OFF_REQ (which does not exist in SS7).
 */
STATIC streams_fastcall int
dl_unbind_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_unbind_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!dl_lock_state(dl, DLF_IDLE))
		goto outstate;
	dl_set_state(dl, DL_UNBIND_PENDING);
	dl_unlock_state(dl);
	return lm_disable_req(dl, q, mp);
      outstate:
	return dl_error_ack(dl, q, mp, DL_UNBIND_REQ, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(dl, q, mp, DL_UNBIND_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_subs_bind_req: - process DL_SUBS_BIND_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_subs_bind_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_bind_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->dl_subs_sap_offset + p->dl_subs_sap_length)
		goto badprim;
	if (!dl_lock_state(dl, DLF_IDLE))
		goto outstate;
	dl_set_state(dl, DL_SUBS_BIND_PND);
	dl_unlock_state(dl);
	if (p->dl_subs_bind_class != DL_PEER_BIND)
		goto unsupported;
	/* a no-op */
	return dl_subs_bind_ack(dl, q, mp, mp->b_rptr + p->dl_subs_sap_offset,
				p->dl_subs_sap_length);
      unsupported:
	return dl_error_ack(dl, q, mp, DL_SUBS_BIND_REQ, DL_UNSUPPORTED, EOPNOTSUPP);
      outstate:
	return dl_error_ack(dl, q, mp, DL_SUBS_BIND_REQ, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(dl, q, mp, DL_SUBS_BIND_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_subs_unbind_req: - process DL_SUBS_UNBIND_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_subs_unbind_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_unbind_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->dl_subs_sap_offset + p->dl_subs_sap_length)
		goto badprim;
	if (!dl_lock_state(dl, DLF_IDLE))
		goto outstate;
	dl_set_state(dl, DL_SUBS_UNBIND_PND);
	dl_unlock_state(dl);
	/* a no-op */
	return dl_ok_ack(dl, q, mp, DL_SUBS_UNBIND_REQ);
      outstate:
	return dl_error_ack(dl, q, mp, DL_SUBS_UNBIND_REQ, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(dl, q, mp, DL_SUBS_UNBIND_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_enabmulti_req: - process DL_ENABMULTI_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_enabmulti_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_enabmulti_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->dl_addr_offset + p->dl_addr_length)
		goto badprim;
	if (dl_chk_state(dl, (DLF_UNBOUND | DLF_IDLE | DLF_DATAXFER)))
		goto outstate;
	goto notsupported;
      notsupported:
	return dl_error_ack(dl, q, mp, DL_ENABMULTI_REQ, DL_NOTSUPPORTED, EOPNOTSUPP);
      outstate:
	return dl_error_ack(dl, q, mp, DL_ENABMULTI_REQ, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(dl, q, mp, DL_ENABMULTI_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_disabmulti_req: - process DL_DISABMULTI_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_disabmulti_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_disabmulti_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->dl_addr_offset + p->dl_addr_length)
		goto badprim;
	if (dl_chk_state(dl, (DLF_UNBOUND | DLF_IDLE | DLF_DATAXFER)))
		goto outstate;
	goto notsupported;
      notsupported:
	return dl_error_ack(dl, q, mp, DL_DISABMULTI_REQ, DL_NOTSUPPORTED, EOPNOTSUPP);
      outstate:
	return dl_error_ack(dl, q, mp, DL_DISABMULTI_REQ, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(dl, q, mp, DL_DISABMULTI_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_promiscon_req: - process DL_PROMISCON_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_promiscon_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_promiscon_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (p->dl_level) {
	case DL_PROMISC_PHYS:
	case DL_PROMISC_SAP:
	case DL_PROMISC_MULTI:
		break;
	default:
		goto unsupported;
	}
	goto notsupported;
      notsupported:
	return dl_error_ack(dl, q, mp, DL_PROMISCON_REQ, DL_NOTSUPPORTED, EOPNOTSUPP);
      unsupported:
	return dl_error_ack(dl, q, mp, DL_PROMISCON_REQ, DL_UNSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(dl, q, mp, DL_PROMISCON_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_promiscoff_req: - process DL_PROMISCOFF_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_promiscoff_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_promiscoff_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (p->dl_level) {
	case DL_PROMISC_PHYS:
	case DL_PROMISC_SAP:
	case DL_PROMISC_MULTI:
		break;
	default:
		goto unsupported;
	}
	goto notsupported;
      notsupported:
	return dl_error_ack(dl, q, mp, DL_PROMISCOFF_REQ, DL_NOTSUPPORTED, EOPNOTSUPP);
      unsupported:
	return dl_error_ack(dl, q, mp, DL_PROMISCOFF_REQ, DL_UNSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(dl, q, mp, DL_PROMISCOFF_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_unitdata_req: - process DL_UNITDATA_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_unitdata_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	size_t dlen;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->dl_dest_addr_offset + p->dl_dest_addr_length)
		goto badprim;
	if (dl_not_state(dl, DLF_IDLE))
		goto outstate;
	if (!mp->b_cont || (dlen = msgdsize(mp->b_cont)) < dl->info.dl_min_sdu
	    || dlen > dl->info.dl_max_sdu)
		goto baddata;
	goto notsupported;
      notsupported:
	return dl_uderror_ind(dl, q, mp, mp->b_rptr + p->dl_dest_addr_offset,
			      p->dl_dest_addr_length, DL_NOTSUPPORTED, EOPNOTSUPP);
      baddata:
	return dl_uderror_ind(dl, q, mp, mp->b_rptr + p->dl_dest_addr_offset,
			      p->dl_dest_addr_length, DL_BADDATA, EMSGSIZE);
      outstate:
	return dl_uderror_ind(dl, q, mp, mp->b_rptr + p->dl_dest_addr_offset,
			      p->dl_dest_addr_length, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_uderror_ind(dl, q, mp, NULL, 0, DL_BADPRIM, EINVAL);
}

/**
 * dl_udqos_req: - process DL_UDQOS_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_udqos_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_udqos_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->dl_qos_offset + p->dl_qos_length)
		goto badprim;
	if (dl_not_state(dl, DLF_IDLE))
		goto outstate;
	goto notsupported;
      notsupported:
	return dl_error_ack(dl, q, mp, DL_UDQOS_REQ, DL_NOTSUPPORTED, EOPNOTSUPP);
      outstate:
	return dl_error_ack(dl, q, mp, DL_UDQOS_REQ, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(dl, q, mp, DL_UDQOS_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_connect_req: - process DL_CONNECT_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * Implements SL_EMERGENCY_REQ, SL_EMERGENCY_CEASES_REQ, SL_START_REQ.  Also
 * implements SL_RESUME_REQ, SL_CLEAR_BUFFERS_REQ.
 */
STATIC streams_fastcall int
dl_connect_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_req_t *p = (typeof(p)) mp->b_rptr;
	dl_qos_conn_ss7_t qos = { DL_QOS_CONN_SS7, 0, 0, };

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->dl_dest_addr_offset + p->dl_dest_addr_length)
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->dl_qos_length + p->dl_qos_offset)
		goto badprim;
	if (!dl_lock_state(dl, DLF_IDLE))
		goto outstate;
	dl_set_state(dl, DL_OUTCON_PENDING);
	dl_unlock_state(dl);
	if (p->dl_dest_addr_length != 0)
		goto badaddr;
	if (p->dl_qos_length != 0) {
		if (p->dl_qos_length < sizeof(dl_ulong))
			goto badprim;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qos, sizeof(qos.dl_qos_type));
		if (qos.dl_qos_type != DL_QOS_CONN_SS7)
			goto badqostype;
		if (p->dl_qos_length != sizeof(qos))
			goto badqosparam;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qos, sizeof(qos));
		if (qos.flush & ~FLUSHRW)
			goto badqosparam;
	}
	/* FIXME: start the SS7 link */
	freemsg(mp);
	return (0);
      badaddr:
	return dl_error_ack(dl, q, mp, DL_CONNECT_REQ, DL_BADADDR, EINVAL);
      outstate:
	return dl_error_ack(dl, q, mp, DL_CONNECT_REQ, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(dl, q, mp, DL_CONNECT_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_connect_res: - process DL_CONNECT_RES primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_connect_res(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_res_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->dl_qos_offset + p->dl_qos_length)
		goto badprim;
	if (!dl_lock_state(dl, DLF_INCON_PENDING))
		goto outstate;
	dl_set_state(dl, DL_CONN_RES_PENDING);
	dl_unlock_state(dl);
	goto notsupported;
      notsupported:
	return dl_error_ack(dl, q, mp, DL_CONNECT_RES, DL_NOTSUPPORTED, EOPNOTSUPP);
      outstate:
	return dl_error_ack(dl, q, mp, DL_CONNECT_RES, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(dl, q, mp, DL_CONNECT_RES, DL_BADPRIM, EINVAL);
}

/**
 * dl_token_req: - process DL_TOKEN_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_token_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_token_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return dl_token_ack(dl, q, mp);
      badprim:
	return dl_error_ack(dl, q, mp, DL_TOKEN_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_disconnect_req: - process DL_DISCONNECT_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * Implements SL_STOP_REQ, SL_LOCAL_PROCESSOR_OUTAGE_REQ.
 */
STATIC streams_fastcall int
dl_disconnect_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_disconnect_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!dl_lock_state(dl, (DLF_DATAXFER | DLF_INCON_PENDING | DLF_OUTCON_PENDING
				| DLF_PROV_RESET_PENDING | DLF_USER_RESET_PENDING)))
		goto outstate;
	switch (dl_get_state(dl)) {
	case DL_OUTCON_PENDING:
		dl_set_state(dl, DL_DISCON8_PENDING);
		break;
	case DL_INCON_PENDING:
		dl_set_state(dl, DL_DISCON9_PENDING);
		break;
	case DL_DATAXFER:
		dl_set_state(dl, DL_DISCON11_PENDING);
		break;
	case DL_USER_RESET_PENDING:
		dl_set_state(dl, DL_DISCON12_PENDING);
		break;
	case DL_PROV_RESET_PENDING:
		dl_set_state(dl, DL_DISCON13_PENDING);
		break;
	default:
		never();
		break;
	}
	dl_unlock_state(dl);
	if (p->dl_correlation != 0)
		goto badcorr;
	switch (p->dl_reason) {
	case DL_DISC_NORMAL_CONDITION:
		/* SL_STOP_REQ */
	case DL_DISC_ABNORMAL_CONDITION:
		/* SL_LOCAL_PROCESSOR_OUTAGE_REQ */
	case DL_DISC_UNSPECIFIED:
		/* SL_STOP_REQ */
		break;
	case DL_CONREJ_PERMANENT_COND:
	case DL_CONREJ_TRANSIENT_COND:
		goto unsupported;
	default:
		goto unsupported;
	}
	/* FIXME: issue an SS7 link stop signal */
	freemsg(mp);
	return (0);
      unsupported:
	return dl_error_ack(dl, q, mp, DL_DISCONNECT_REQ, DL_UNSUPPORTED, EINVAL);
      badcorr:
	return dl_error_ack(dl, q, mp, DL_DISCONNECT_REQ, DL_BADCORR, EINVAL);
      oustate:
	return dl_error_ack(dl, q, mp, DL_DISCONNECT_REQ, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(dl, q, mp, DL_DISCONNECT_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_reset_req: - process DL_RESET_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * This primitive implements the SL_CLEAR_BUFFERS_REQ primitive.
 */
STATIC streams_fastcall int
dl_reset_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_reset_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!dl_lock_state(dl, DLF_DATAXFER))
		goto outstate;
	dl_set_state(dl, DL_USER_RESET_PENDING);
	dl_unlock_state(dl);
	/* XXX: clear buffers */
	return dl_reset_con(dl, q, mp);
      outstate:
	return dl_error_ack(dl, q, mp, DL_RESET_REQ, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(dl, q, mp, DL_RESET_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_reset_res: - process DL_RESET_RES primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_reset_res(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_reset_res_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!dl_lock_state(dl, DLF_PROV_RESET_PENDING))
		goto outstate;
	dl_set_state(dl, DL_RESET_RES_PENDING);
	dl_unlock_state(dl);
	/* no-op */
	return dl_ok_ack(dl, q, mp, DL_RESET_RES);
      badprim:
	return dl_error_ack(dl, q, mp, DL_RESET_RES, DL_BADPRIM, EINVAL);
}

/**
 * dl_data_ack_req: - process DL_DATA_ACK_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_data_ack_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_data_ack_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupported;
      notsupported:
	return dl_error_ack(dl, q, mp, DL_DATA_ACK_REQ, DL_NOTSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(dl, q, mp, DL_DATA_ACK_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_reply_req: - process DL_REPLY_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_reply_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_reply_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupported;
      notsupported:
	return dl_error_ack(dl, q, mp, DL_REPLY_REQ, DL_NOTSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(dl, q, mp, DL_REPLY_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_reply_update_req: - process DL_REPLY_UPDATE_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_reply_update_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_reply_update_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupported;
      notsupported:
	return dl_error_ack(dl, q, mp, DL_REPLY_UPDATE_REQ, DL_NOTSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(dl, q, mp, DL_REPLY_UPDATE_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_xid_req: - process DL_XID_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_xid_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_xid_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupported;
      notsupported:
	return dl_error_ack(dl, q, mp, DL_XID_REQ, DL_NOTSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(dl, q, mp, DL_XID_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_xid_res: - process DL_XID_RES primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_xid_res(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_xid_res_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupported;
      notsupported:
	return dl_error_ack(dl, q, mp, DL_XID_RES, DL_NOTSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(dl, q, mp, DL_XID_RES, DL_BADPRIM, EINVAL);
}

/**
 * dl_test_req: - process DL_TEST_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_test_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_test_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupported;
      notsupported:
	return dl_error_ack(dl, q, mp, DL_TEST_REQ, DL_NOTSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(dl, q, mp, DL_TEST_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_test_res: - process DL_TEST_RES primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_test_res(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_test_res_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupported;
      notsupported:
	return dl_error_ack(dl, q, mp, DL_TEST_RES, DL_NOTSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(dl, q, mp, DL_TEST_RES, DL_BADPRIM, EINVAL);
}

/**
 * dl_phys_addr_req: - process DL_PHYS_ADDR_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_phys_addr_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_phys_addr_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupported;
      notsupported:
	return dl_error_ack(dl, q, mp, DL_PHYS_ADDR_REQ, DL_NOTSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(dl, q, mp, DL_PHYS_ADDR_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_set_phys_addr_req: - process DL_SET_PHYS_ADDR_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_set_phys_addr_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_set_phys_addr_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupported;
      notsupported:
	return dl_error_ack(dl, q, mp, DL_SET_PHYS_ADDR_REQ, DL_NOTSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(dl, q, mp, DL_SET_PHYS_ADDR_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_get_statistics_req: - process DL_GET_STATISTICS_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_get_statistics_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_get_statistics_req_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupported;
      notsupported:
	return dl_error_ack(dl, q, mp, DL_GET_STATISTICS_REQ, DL_NOTSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(dl, q, mp, DL_GET_STATISTICS_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_notsupported_req: - process unsupported primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_notsupported(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_ulong prim = *(dl_ulong *) mp->b_rptr;

	return dl_error_ack(dl, q, mp, prim, DL_NOTSUPPORTED, EOPNOTSUPP);
}

/**
 * dl_unknown_req: - process DL_UNKNOWN_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC streams_fastcall int
dl_unknown_req(struct priv *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_ack(dl, q, mp, DL_UNKNOWN_REQ, DL_BADPRIM, EINVAL);
}

/**
 * lmi_info_ack: process LMI_INFO_ACK primitive
 * @priv: private structure
 * @q: active queue
 * @mp: the LMI_INFO_ACK primitive
 *
 * When the module is first pushed, DL-SL issues an LMI_INFO_REQ and expect this response to fill
 * out the information structures.  Always fill out both the sl and dl information structures when
 * this response arrives.  Another reason for issuing an LMI_INFO_REQ is if the module suspects that
 * it is out of sync with the underlying SL provider.
 */
STATIC streams_fastcall int
lm_info_ack(struct priv *priv, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	priv->sl.info.lmi_version = p->lmi_version;
	switch ((priv->sl.info.lmi_state = p->lmi_state)) {
	case LMI_UNATTACHED:
		dl_set_state(priv, DL_UNATTACHED);
		break;
	case LMI_ATTACH_PENDING:
		dl_set_state(priv, DL_ATTACH_PENDING);
		break;
	case LMI_UNUSABLE:
		dl_set_state(priv, -1);
		break;
	case LMI_DISABLED:
		dl_set_state(priv, DL_UNBOUND);
		break;
	case LMI_ENABLE_PENDING:
		dl_set_state(priv, DL_BIND_PENDING);
		break;
	case LMI_ENABLED:
		dl_set_state(priv, DL_IDLE);
		break;
	case LMI_DISABLE_PENDING:
		dl_set_state(priv, DL_UNBIND_PENDING);
		break;
	case LMI_DETACH_PENDING:
		dl_set_state(priv, DL_DETACH_PENDING);
		break;
	}
	priv->dl.info.dl_min_sdu = p->lmi_min_sdu;
	priv->sl.info.lmi_min_sdu = p->lmi_min_sdu;
	priv->dl.info.dl_max_sdu = p->lmi_max_sdu;
	priv->sl.info.lmi_max_sdu = p->lmi_max_sdu;
	switch ((priv->sl.info.lmi_ppa_style = p->lmi_ppa_style)) {
	case LMI_STYLE1:
		priv->dl.info.dl_provider_style = DL_STYLE1;
		break;
	case LMI_STYLE2:
		priv->dl.info.dl_provider_style = DL_STYLE2;
		break;
	default:
		swerr();
		break;
	}
	priv->sl.info.lmi_ppa_addr[0] = priv->sl.info.lmi_ppa_addr[0];
	freemsg(mp);
	return (0);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * lmi_info_ack: process LMI_INFO_ACK primitive
 * @lm: private structure
 * @q: active queue
 * @mp: the LMI_INFO_ACK primitive
 *
 * Much of the coordination work is done here.  This is where most responses to primitives that
 * issued downstream result.
 */
STATIC streams_fastcall int
lm_ok_ack(struct priv *lm, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (p->lmi_correct_primitive) {
	case LMI_ATTACH_REQ:
		if (dl_get_state(lm) == DL_ATTACH_PENDING)
			return dl_ok_ack(lm, q, mp, DL_ATTACH_REQ);
		goto error;
	case LMI_DETACH_REQ:
		if (dl_get_state(lm) == DL_DETACH_PENDING)
			return dl_ok_ack(lm, q, mp, DL_DETACH_REQ);
		goto error;
	case SL_PDU_REQ:
	case SL_EMERGENCY_REQ:
	case SL_EMERGENCY_CEASES_REQ:
	case SL_START_REQ:
	case SL_STOP_REQ:
	case SL_RETRIEVE_BSNT_REQ:
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
	case SL_CLEAR_BUFFERS_REQ:
	case SL_CLEAR_RTB_REQ:
	case SL_CONTINUE_REQ:
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
	case SL_RESUME_REQ:
	case SL_CONGESTION_DISCARD_REQ:
	case SL_CONGESTION_ACCEPT_REQ:
	case SL_NO_CONGESTION_REQ:
	case SL_POWER_ON_REQ:
	case SL_OPTMGMT_REQ:
	case SL_NOTIFY_REQ:
		/* None of these expect an LMI_OK_ACK. */
		goto error;
	case LMI_INFO_REQ:
	case LMI_ENABLE_REQ:
	case LMI_DISABLE_REQ:
	case LMI_OPTMGMT_REQ:
		/* These must be acknowledged with a different primitive. */
		goto error;
	default:
		goto error;
	}
	freemsg(mp);
	return (0);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * lmi_info_ack: process LMI_INFO_ACK primitive
 * @lmi: private structure
 * @q: active queue
 * @mp: the LMI_INFO_ACK primitive
 */
STATIC streams_fastcall int
lm_error_ack(struct priv *lmi, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (p->lmi_error_primitive) {
	case LMI_INFO_REQ:
	case LMI_ATTACH_REQ:
	case LMI_DETACH_REQ:
	case LMI_ENABLE_REQ:
	case LMI_DISABLE_REQ:
	case LMI_OPTMGMT_REQ:
	case SL_PDU_REQ:
	case SL_EMERGENCY_REQ:
		return dl_error_ack(lmi, q, mp, DL_CONNECT_REQ, XXX, XXX);
	case SL_EMERGENCY_CEASES_REQ:
		return dl_error_ack(lmi, q, mp, DL_CONNECT_REQ, XXX, XXX);
	case SL_START_REQ:
		return dl_error_ack(lmi, q, mp, DL_CONNECT_REQ, XXX, XXX);
	case SL_STOP_REQ:
		return dl_error_ack(lmi, q, mp, DL_DISCONNECT_REQ, XXX, XXX);
	case SL_RETRIEVE_BSNT_REQ:
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
	case SL_CLEAR_BUFFERS_REQ:
		return dl_error_ack(lmi, q, mp, DL_RESET_REQ, XXX, XXX);
	case SL_CLEAR_RTB_REQ:
		return dl_error_ack(lmi, q, mp, DL_RESET_REQ, XXX, XXX);
	case SL_CONTINUE_REQ:
		return dl_error_ack(lmi, q, mp, DL_RESET_RES, XXX, XXX);
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
	case SL_RESUME_REQ:
	case SL_CONGESTION_DISCARD_REQ:
	case SL_CONGESTION_ACCEPT_REQ:
	case SL_NO_CONGESTION_REQ:
	case SL_POWER_ON_REQ:
	case SL_OPTMGMT_REQ:
	case SL_NOTIFY_REQ:
		break;
	}
	freemsg(mp);
	return (0);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * lmi_info_ack: process LMI_INFO_ACK primitive
 * @lmi: private structure
 * @q: active queue
 * @mp: the LMI_INFO_ACK primitive
 */
STATIC streams_fastcall int
lm_enable_con(struct priv *lmi, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return dl_bind_ack(lm, q, mp);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * lmi_info_ack: process LMI_INFO_ACK primitive
 * @lmi: private structure
 * @q: active queue
 * @mp: the LMI_INFO_ACK primitive
 */
STATIC streams_fastcall int
lm_disable_con(struct priv *lmi, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return dl_ok_ack(lm, q, mp, DL_UNBIND_REQ);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * lmi_info_ack: process LMI_INFO_ACK primitive
 * @lmi: private structure
 * @q: active queue
 * @mp: the LMI_INFO_ACK primitive
 */
STATIC streams_fastcall int
lm_optmgmt_ack(struct priv *lmi, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	freemsg(mp);
	return (0);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * lmi_info_ack: process LMI_INFO_ACK primitive
 * @lmi: private structure
 * @q: active queue
 * @mp: the LMI_INFO_ACK primitive
 */
STATIC streams_fastcall int
lm_error_ind(struct priv *lmi, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	freemsg(mp);
	return (0);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * lmi_info_ack: process LMI_INFO_ACK primitive
 * @lmi: private structure
 * @q: active queue
 * @mp: the LMI_INFO_ACK primitive
 */
STATIC streams_fastcall int
lm_stats_ind(struct priv *lmi, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	freemsg(mp);
	return (0);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * lmi_info_ack: process LMI_INFO_ACK primitive
 * @lmi: private structure
 * @q: active queue
 * @mp: the LMI_INFO_ACK primitive
 */
STATIC streams_fastcall int
lm_event_ind(struct priv *lmi, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	freemsg(mp);
	return (0);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * sl_pdu_ind: process SL_PDU_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_PDU_IND primitive
 *
 * SL_PDU_IND maps to DL_DATA_IND (M_DATA).
 */
STATIC streams_fastcall int
sl_pdu_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_pdu_ind_t *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	dp = XCHG(&mp->b_cont, NULL);
	if ((err = dl_data_ind(sl, q, mp, dp))) {
		mp->b_cont = dp;
		return (err);
	}
	return (0);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * sl_link_congested_ind: process SL_LINK_CONGESTED_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_LINK_CONGESTED_IND primitive
 *
 * SL_LINK_CONGESTED_IND translates into DL_RESET_IND.
 */
STATIC streams_fastcall int
sl_link_congested_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_link_cong_ind_t *p = (typeof(p)) mp->b_rptr;
	dl_ulong reason;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	p = (typeof(p)) mp->b_rptr;
	reason = DL_RESET_FLOW_CONTROL;
	reason |= ((p->sl_cong_status & 0x0f) << 12);
	reason |= ((p->sl_disc_status & 0x0f) << 8);
	return dl_reset_ind(sl, q, mp, DL_PROVIDER, reason);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * sl_link_congestion_ceased_ind: process SL_LINK_CONGESTION_CEASED_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_LINK_CONGESTION_CEASED_IND primitive
 *
 * SL_LINK_CONGESTION_CEASED_IND translates into DL_RESET_IND.
 */
STATIC streams_fastcall int
sl_link_congestion_ceased_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_link_cong_ceased_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	p = (typeof(p)) mp->b_rptr;
	reason = DL_RESET_FLOW_CONTROL;
	reason |= ((p->sl_cong_status & 0x0f) << 12);
	reason |= ((p->sl_disc_status & 0x0f) << 8);
	return dl_reset_ind(sl, q, mp, DL_PROVIDER, reason);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * sl_retrieved_message_ind: process SL_RETRIEVED_MESSAGE_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_RETRIEVED_MESSAGE_IND primitive
 *
 * SL_RETRIEVED_MESSAGE_IND translates into DL_DATA_ACK_IND.
 */
STATIC streams_fastcall int
sl_retrieved_message_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieved_msg_ind_t *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	dp = XCHG(&mp->b_cont, NULL);
	if ((err = dl_data_ack_ind(sl, q, mp, p->sl_mp, dp))) {
		mp->b_cont = dp;
		return (err);
	}
	return (0);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * sl_retrieval_complete_ind: process SL_RETRIEVAL_COMPLETE_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_RETRIEVAL_COMPLETE_IND primitive
 *
 * SL_RETRIEVED_MESSAGE_IND translates into DL_DATA_ACK_IND (with no data).
 */
STATIC streams_fastcall int
sl_retrieval_complete_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieval_comp_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return dl_data_ack_ind(sl, q, mp, 0, NULL);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * sl_rb_cleared_ind: process SL_RB_CLEARED_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_RB_CLEARED_IND primitive
 *
 * SL_RB_CLEARED_IND translates to DL_RESET_CON.
 */
STATIC streams_fastcall int
sl_rb_cleared_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_rb_cleared_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return dl_reset_con(sl, q, mp);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * sl_bsnt_ind: process SL_BSNT_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_BSNT_IND primitive
 */
STATIC streams_fastcall int
sl_bsnt_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_bsnt_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	freemsg(mp);
	return (0);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * sl_in_service_ind: process SL_IN_SERVICE_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_IN_SERVICE_IND primitive
 *
 * SL_IN_SERVICE_IND maps to DL_CONNECT_CON.
 */
STATIC streams_fastcall int
sl_in_service_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_in_service_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return dl_connect_con(sl, q, mp);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * sl_out_of_service_ind: process SL_OUT_OF_SERVICE_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_OUT_OF_SERVICE_IND primitive
 *
 * SL_OUT_OF_SERVICE_IND maps to DL_DISCONNECT_IND.
 */
STATIC streams_fastcall int
sl_out_of_service_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_out_of_service_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return dl_disconnect_ind(sl, q, mp, DL_PROVIDER, reason);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * sl_remote_processor_outage_ind: process SL_REMOTE_PROCESSOR_OUTAGE_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_REMOTE_PROCESSOR_OUTAGE_IND primitive
 *
 * SL_REMOTE_PROCESSOR_OUTAGE maps to DL_RESET_IND.
 */
STATIC streams_fastcall int
sl_remote_processor_outage_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_rem_proc_out_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return dl_reset_ind(sl, q, mp, DL_PROVIDER, 0xff00 | DL_RESET_FLOW_CONTROL);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * sl_remote_processor_recovered_ind: process SL_REMOTE_PROCESSOR_RECOVERED_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_REMOTE_PROCESSOR_RECOVERED_IND primitive
 *
 * SL_REMOTE_PROCESSOR_RECOVERED_IND maps to DL_RESET_IND.
 */
STATIC streams_fastcall int
sl_remote_processor_recovered_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_rem_proc_recovered_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return dl_reset_ind(sl, q, mp, DL_PROVIDER, 0x0000 | DL_RESET_FLOW_CONTROL);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * sl_rtb_cleared_ind: process SL_RTB_CLEARED_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_RTB_CLEARED_IND primitive
 *
 * SL_RTB_CLEARED_IND maps to DL_RESET_CON.
 */
STATIC streams_fastcall int
sl_rtb_cleared_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_rtb_cleared_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return dl_reset_con(sl, q, mp);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * sl_retrieval_not_possible_ind: process SL_RETRIEVAL_NOT_POSSIBLE_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_RETRIEVAL_NOT_POSSIBLE_IND primitive
 *
 * SL_RETRIEVAL_NOT_POSSIBLE maps into DL_DATA_ACK_IND (w/ no data).
 */
STATIC streams_fastcall int
sl_retrieval_not_possible_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieval_not_poss_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	return dl_data_ack_ind(sl, q, mp, 0, NULL);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * sl_bsnt_not_retrievable_ind: process SL_BSNT_NOT_RETRIEVABLE_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_BSNT_NOT_RETRIEVABLE_IND primitive
 */
STATIC streams_fastcall int
sl_bsnt_not_retrievable_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_bsnt_not_retr_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	freemsg(mp);
	return (0);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * sl_optmgmt_ack: process SL_OPTMGMT_ACK primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_OPTMGMT_ACK primitive
 */
STATIC streams_fastcall int
sl_optmgmt_ack(struct priv *sl, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	freemsg(mp);
	return (0);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

#if 0
/**
 * sl_notify_ind: process SL_NOTIFY_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_NOTIFY_IND primitive
 */
STATIC streams_fastcall int
sl_notify_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_notify_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	freemsg(mp);
	return (0);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}
#endif

/**
 * sl_local_processor_outage_ind: process SL_LOCAL_PROCESSOR_OUTAGE_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_LOCAL_PROCESSOR_OUTAGE_IND primitive
 */
STATIC streams_fastcall int
sl_local_processor_outage_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_loc_proc_out_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	freemsg(mp);
	return (0);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * sl_local_processor_recovered_ind: process SL_LOCAL_PROCESSOR_RECOVERED_IND primitive
 * @sl: private structure
 * @q: active queue
 * @mp: the SL_LOCAL_PROCESSOR_RECOVERED_IND primitive
 */
STATIC streams_fastcall int
sl_local_processor_recovered_ind(struct priv *sl, queue_t *q, mblk_t *mp)
{
	sl_loc_proc_recovered_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	freemsg(mp);
	return (0);
      badprim:
	goto error;
      error:
	swerr();
	freemsg(mp);
	return (0);
}

STATIC streams_fastcall int
dl_w_proto(struct priv *dl, queue_t *q, mblk_t *mp)
{
	dl_ulong prim;

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		switch ((prim = *(dl_ulong *) mp->b_rptr)) {
		case DL_INFO_REQ:
			return dl_info_req(dl, q, mp);
		case DL_ATTACH_REQ:
			return dl_attach_req(dl, q, mp);
		case DL_BIND_REQ:
			return dl_bind_req(dl, q, mp);
		case DL_UNBIND_REQ:
			return dl_unbind_req(dl, q, mp);
		case DL_SUBS_BIND_REQ:
			return dl_subs_bind_req(dl, q, mp);
		case DL_SUBS_UNBIND_REQ:
			return dl_subs_unbind_req(dl, q, mp);
		case DL_ENABMULTI_REQ:
			return dl_enabmulti_req(dl, q, mp);
		case DL_DISABMULTI_REQ:
			return dl_disabmulti_req(dl, q, mp);
		case DL_PROMISCON_REQ:
			return dl_promiscon_req(dl, q, mp);
		case DL_PROMISCOFF_REQ:
			return dl_promiscoff_req(dl, q, mp);
		case DL_UNITDATA_REQ:
			return dl_unitdata_req(dl, q, mp);
		case DL_UDQOS_REQ:
			return dl_udqos_req(dl, q, mp);
		case DL_CONNECT_REQ:
			return dl_connect_req(dl, q, mp);
		case DL_CONNECT_RES:
			return dl_connect_res(dl, q, mp);
		case DL_TOKEN_REQ:
			return dl_token_req(dl, q, mp);
		case DL_DISCONNECT_REQ:
			return dl_disconnect_req(dl, q, mp);
		case DL_RESET_REQ:
			return dl_reset_req(dl, q, mp);
		case DL_RESET_RES:
			return dl_reset_res(dl, q, mp);
		case DL_DATA_ACK_REQ:
			return dl_data_ack_req(dl, q, mp);
		case DL_REPLY_REQ:
			return dl_reply_req(dl, q, mp);
		case DL_REPLY_UPDATE_REQ:
			return dl_reply_update_req(dl, q, mp);
		case DL_XID_REQ:
			return dl_xid_req(dl, q, mp);
		case DL_XID_RES:
			return dl_xid_res(dl, q, mp);
		case DL_TEST_REQ:
			return dl_test_req(dl, q, mp);
		case DL_TEST_RES:
			return dl_test_res(dl, q, mp);
		case DL_PHYS_ADDR_REQ:
			return dl_phys_addr_req(dl, q, mp);
		case DL_SET_PHYS_ADDR_REQ:
			return dl_set_phys_addr_req(dl, q, mp);
		case DL_GET_STATISTICS_REQ:
			return dl_get_statistics_req(dl, q, mp);
		default:
			return dl_unknown_req(dl, q, mp);
		}
	}
	putnextctl2(dl->rq, M_ERROR, EPROTO, EPROTO);
	freemsg(mp);
	return (0);
}

STATIC streams_fastcall int
sl_r_proto(struct priv *sl, queue_t *q, mblk_t *mp)
{
	np_ulong prim;

	switch ((prim = *(np_ulong *) mp->b_rptr)) {
	case LMI_INFO_ACK:
		return lm_info_ack(sl, q, mp);
	case LMI_OK_ACK:
		return lm_ok_ack(sl, q, mp);
	case LMI_ERROR_ACK:
		return lm_error_ack(sl, q, mp);
	case LMI_ENABLE_CON:
		return lm_enable_con(sl, q, mp);
	case LMI_DISABLE_CON:
		return lm_disable_con(sl, q, mp);
	case LMI_OPTMGMT_ACK:
		return lmi_optmgmt_ack(sl, q, mp);
	case LMI_ERROR_IND:
		return lmi_error_ind(sl, q, mp);
	case LMI_STATS_IND:
		return lmi_stats_ind(sl, q, mp);
	case LMI_EVENT_IND:
		return lmi_event_ind(sl, q, mp);
	case SL_PDU_IND:
		return sl_pdu_ind(sl, q, mp);
	case SL_LINK_CONGESTED_IND:
		return sl_link_congested_ind(sl, q, mp);
	case SL_LINK_CONGESTION_CEASED_IND:
		return sl_link_congestion_ceased_ind(sl, q, mp);
	case SL_RETRIEVED_MESSAGE_IND:
		return sl_retrieved_message_ind(sl, q, mp);
	case SL_RETRIEVAL_COMPLETE_IND:
		return sl_retrieval_complete_ind(sl, q, mp);
	case SL_RB_CLEARED_IND:
		return sl_rb_cleared_ind(sl, q, mp);
	case SL_BSNT_IND:
		return sl_bsnt_ind(sl, q, mp);
	case SL_IN_SERVICE_IND:
		return sl_in_service_ind(sl, q, mp);
	case SL_OUT_OF_SERVICE_IND:
		return sl_out_of_service_ind(sl, q, mp);
	case SL_REMOTE_PROCESSOR_OUTAGE_IND:
		return sl_remote_processor_outage_ind(sl, q, mp);
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		return sl_remote_processor_recovered_ind(sl, q, mp);
	case SL_RTB_CLEARED_IND:
		return sl_rtb_cleared_ind(sl, q, mp);
	case SL_RETRIEVAL_NOT_POSSIBLE_IND:
		return sl_retrieval_not_possible_ind(sl, q, mp);
	case SL_BSNT_NOT_RETRIEVABLE_IND:
		return sl_bsnt_not_retrievable_ind(sl, q, mp);
	case SL_OPTMGMT_ACK:
		return sl_optmgmt_ack(sl, q, mp);
#if 0
	case SL_NOTIFY_IND:
		return sl_notify_ind(sl, q, mp);
#endif
	case SL_LOCAL_PROCESSOR_OUTAGE_IND:
		return sl_local_processor_outage_ind(sl, q, mp);
	case SL_LOCAL_PROCESSOR_RECOVERED_IND:
		return sl_local_processor_recovered_ind(sl, q, mp);
	default:
		return sl_unknown_ind(sl, q, mp);
	}
	freemsg(mp);
	return (0);
}

STATIC streams_fastcall int
dl_wmsg(queue_t *q, mblk_t *mp)
{
	struct priv *dl = DL_PRIV(q);

	switch (mp->b_datap->db_type) {
	case M_IOCTL:
	case M_IOCDATA:
		return dl_w_ioctl(dl, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_w_proto(dl, q, mp);
	case M_DATA:
		return dl_w_data(dl, q, mp);
	case M_CTL:
		return dl_w_ctl(dl, q, mp);
	}
	if (!pcmsg(mp->b_datap->db_type) && !bcanputnext(q, mp->b_band))
		return (1);
	putnext(q, mp);
	return (0);
}

STATIC streams_fastcall int
sl_rmsg(queue_t *q, mblk_t *mp)
{
	struct priv *sl = DL_PRIV(q);

	switch (mp->b_datap->db_type) {
	case M_COPYIN:
	case M_COPYOUT:
	case M_IOCACK:
		return sl_r_ioctl(sl, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sl_r_proto(sl, q, mp);
	case M_DATA:
		return sl_r_data(sl, q, mp);
	case M_CTL:
		return sl_r_ctl(sl, q, mp);
	}
	if (!pcmsg(mp->b_datap->db_type) && !bcanputnext(q, mp->b_band))
		return (1);
	freemsg(mp);
	return (0);
}

/*
 *  PUT and SERVICE procedures.
 */
STATIC streamscall int
dl_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(mp->b_datap->db_type) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || dl_wmsg(q, mp)) {
		if (putq(q, mp))
			return (0);
		swerr();
		freemsg(mp);
	}
	return (0);
}

STATIC streamscall int
dl_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (dl_wmsg(q, mp)) {
			if (putbq(q, mp))
				return (0);
			swerr();
			freemsg(mp);
		}
	}
	return (0);
}

STATIC streamscall int
sl_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (sl_rmsg(q, mp)) {
			if (putbq(q, mp))
				return (0);
			swerr();
			freemsg(mp);
		}
	}
	return (0);
}

STATIC streamscall int
sl_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(mp->b_datap->db_type) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || sl_rmsg(q, mp)) {
		if (putq(q, mp))
			return (0);
		swerr();
		freemsg(mp);
	}
	return (0);
}

/*
 *  OPEN and CLOSE routines.
 */
static caddr_t dl_opens = NULL;

STATIC streamscall int
dl_open(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *priv;
	lmi_info_req_t *p;
	mblk_t *mp;
	int err;

	if (q->q_ptr)
		return (0);
	if ((mp = allocb(sizeof(lmi_info_req_t), BPRI_WAITOK)) == NULL)
		return (ENOMEM);
	if (getminor(*devp) < 5)
		sflag = CLONEOPEN;
	if ((err = mi_open_comm(&dl_opens, sizeof(*priv), q, devp, oflags, sflag, crp))) {
		freemsg(mp);
		return (err);
	}
	priv = (typeof(priv)) q->q_ptr;
	bzero(priv, sizeof(*priv));
	priv->rq = q;
	priv->wq = WR(q);
	spin_lock_init(&priv->lock);
	priv->sl.info.lmi_primitive = LMI_INFO_ACK;
	priv->sl.info.lmi_version = LMI_VERSION_1;
	priv->sl.info.lmi_state = LMI_UNATTACHED;
	priv->sl.info.lmi_max_sdu = 272;	/* initially */
	priv->sl.info.lmi_min_sdu = 0;
	priv->sl.info.lmi_ppa_style = LMI_STYLE2;
	priv->sl.info.lmi_ppa_addr[0] = 0;
	priv->dl.info.dl_primitive = DL_INFO_ACK;
	priv->dl.info.dl_max_sdu = 272;	/* initially */
	priv->dl.info.dl_min_sdu = 0;
	priv->dl.info.dl_addr_length = 0;
	priv->dl.info.dl_mac_type = DL_SS7;
	priv->dl.info.dl_reserved = 0;
	priv->dl.info.dl_current_state = DL_UNATTACHED;	/* initially */
	priv->dl.info.dl_sap_length = 0;
	priv->dl.info.dl_service_mode = DL_CODLS;
	priv->dl.info.dl_qos_length = sizeof(priv->dl.qos);
	priv->dl.info.dl_qos_offset = sizeof(priv->dl.info);
	priv->dl.info.dl_qos_range_length = sizeof(priv->dl.qor);
	priv->dl.info.dl_qos_range_offset = sizeof(priv->dl.info) + sizeof(priv->dl.qos);
	priv->dl.info.dl_provider_style = DL_STYLE2;
	priv->dl.info.dl_addr_offset = 0;
	priv->dl.info.dl_version = DL_VERSION_2;
	priv->dl.info.dl_brdcst_addr_length =;
	priv->dl.info.dl_brdcst_addr_offset =;
	priv->dl.info.dl_growth = 0;
	priv->dl.qos.emergency = 0;
	priv->dl.qos.flush = 0;
	priv->dl.qor.emergency = 1;
	priv->dl.qor.flush = FLUSHRW;
	p = (typeof(p)) mp->b_wptr++;
	p->lmi_primitive = LMI_INFO_REQ;
	qprocson(q);		/* before qreply() so we don't miss the response */
	qreply(q, mp);
	return (0);
}

STATIC streamscall int
dl_close(queue_t *q, int oflags, cred_t *crp)
{
	return mi_close_comm(&dl_opens, q);
}

/*
 *  STREAMS datastructures.
 */

STATIC struct module_info dl_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

STATIC struct module_stat sl_mstat = __attribute__ ((__aligned__(SMB_CACHE_BYTES)));
STATIC struct module_stat dl_mstat = __attribute__ ((__aligned__(SMB_CACHE_BYTES)));

STATIC struct qinit sl_qinit = {
	.qi_putp = &sl_rput,
	.qi_srvp = &sl_rsrv,
	.qi_qopen = &dl_open,
	.qi_qclose = &dl_close,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &sl_mstat,
};

STATIC struct qinit dl_qinit = {
	.qi_putp = &dl_wput,
	.qi_srvp = &dl_wsrv,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_mstat,
};

STATIC struct streamtab dl_slinfo = {
	.st_rdinit = &sl_qinit,
	.st_wrinit = &dl_qinit,
};

/*
 *  Regstration and initialization.
 */

modID_t modid = MOD_ID;

#ifdef LINUX

#ifdef module_param
module_param(modid, modID_t, 0444);
#else				/* module_param */
MODULE_PARAM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID fo the DL-SL module. (0 for allocation).");

#ifdef LIS
#define fmodsw _fmodsw
#endif
STATIC struct fmodsw dl_fmod = {
	.f_name = MOD_NAME,
	.f_str = &dl_slinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

MODULE_STATIC int __init
dl_slinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_SPLASH);	/* console splash */
	if ((err = register_strmod(&dl_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, -err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
dl_slterminate(void)
{
	if ((err = unregister_strmod(&dl_fmod)))
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, -err);
	return;
}

/*
 *  Linux kernel module initialization.
 */
module_init(dl_slinit);
module_exit(dl_slterminate);
#endif				/* LINUX */
