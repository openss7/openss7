/*****************************************************************************

 @(#) $RCSfile: m2pa_dl.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/08/14 08:34:10 $

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

 Last Modified $Date: 2007/08/14 08:34:10 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m2pa_dl.c,v $
 Revision 0.9.2.4  2007/08/14 08:34:10  brian
 - GPLv3 header update

 Revision 0.9.2.3  2007/07/14 01:33:43  brian
 - make license explicit, add documentation

 Revision 0.9.2.2  2007/03/25 18:59:06  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.1  2006/10/27 22:50:40  brian
 - working up modules and testsuite

 *****************************************************************************/

#ident "@(#) $RCSfile: m2pa_dl.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/08/14 08:34:10 $"

static char const ident[] =
    "$RCSfile: m2pa_dl.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/08/14 08:34:10 $";

#include <sys/os7/compat.h>

#define M2PA_DL_DESCRIP		"M2PA/SCTP SS7 DATA LINK (DL) STREAMS MODULE"
#define M2PA_DL_REVISION	"OpenSS7 $RCSfile: m2pa_dl.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/08/14 08:34:10 $"
#define M2PA_DL_COPYRIGHT	"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define M2PA_DL_DEVICE		"Part of the OpenSS7 Stack for Linux Fast STREAMS"
#define M2PA_DL_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define M2PA_DL_LICENSE		"GPL v2"
#define M2PA_DL_BANNER		M2PA_DL_DESCRIP		"\n" \
				M2PA_DL_REVISION	"\n" \
				M2PA_DL_COPYRIGHT	"\n" \
				M2PA_DL_DEVICE		"\n" \
				M2PA_DL_CONTACT		"\n"
#define M2PA_DL_SPLASH		M2PA_DL_DEVICE		" - " \
				M2PA_DL_REVISION	"\n"

#ifdef LINUX
MODULE_DESCRIPTION(M2PA_DL_DESCRIP);
MODULE_AUTHOR(M2PA_DL_CONTACT);
MODULE_SUPPORTED_DEVICE(M2PA_DL_DEVICE);
MODULE_LICENSE(M2PA_DL_LICENSE);
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-m2pa_dl");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define M2PA_DL_MOD_ID		CONFIG_STREAMS_M2PA_DL_MODID
#define M2PA_DL_MOD_NAME	CONFIG_STREAMS_M2PA_DL_NAME
#endif

#define MOD_ID		M2PA_DL_MOD_ID
#define MOD_NAME	M2PA_DL_MOD_NAME
#ifdef MODULE
#define MOD_SPLASH	M2PA_DL_BANNER
#else				/* MODULE */
#define MOD_SPLASH	M2PA_DL_SPLASH
#endif				/* MODULE */

/*
 *  Private Structure
 */
struct dl {
	STR_DECLARATION (struct dl);	/* stream declaration */
	uint rmsu;			/* received unacked msus */
	uint tmsu;			/* transmitted unacked msus */
	uint rack;			/* received acked msus */
	uint tack;			/* transmitted acked msus */
	uint fsnr;			/* received msu seq number */
	uint bsnr;			/* received msu seq number */
	uint fsnx;			/* expected msu seq number */
	uint fsnt;			/* transmitted msu seq number */
	uint back;			/* bad acks */
	uint bbsn;			/* bad bsn */
	uint bfsn;			/* bad fsn */
	lmi_option_t option;		/* protocol and variant options */
	bufq_t rb;			/* receive buffer */
	bufq_t tb;			/* transmission buffer */
	bufq_t rtb;			/* retransmission buffer */
	struct {
		sl_timers_t timers;	/* SL protocol timers */
		sl_config_t config;	/* SL configuration options */
		sl_statem_t statem;	/* state machine variables */
		sl_notify_t notify;	/* SL notification options */
		sl_stats_t stats;	/* SL statistics */
		sl_stats_t stamp;	/* SL statistics timestamps */
		sl_stats_t statsp;	/* SL statistics periods */
	} sl;
	struct {
		sdt_timers_t timers;	/* SDT timers */
		sdt_config_t config;	/* SDT configuration options */
	} sdt;
	struct {
		sdl_timers_t timers;	/* SDL timers */
		sdl_config_t config;	/* SDL configuration options */
	} sdl;
};

#define DL_PRIV(__q)	((struct dl *)(__q)->q_ptr)

STATIC struct dl *dl_alloc_priv(queue_t *q, struct sl **slp, dev_t *devp, cred_t *crp);
STATIC void dl_free_priv(queue_t *q);
STATIC struct dl *dl_get(struct sl *dl);
STATIC void dl_put(struct sl *dl);

#define M2PA_VERSION_DRAFT3	0x30
#define M2PA_VERSION_DRAFT3_1	0x31
#define M2PA_VERSION_DRAFT4	0x40
#define M2PA_VERSION_DRAFT4_1	0x41
#define M2PA_VERSION_DRAFT4_9	0x49
#define M2PA_VERSION_DRAFT5	0x50
#define M2PA_VERSION_DRAFT5_1	0x51
#define M2PA_VERSION_DRAFT6	0x60
#define M2PA_VERSION_DRAFT6_1	0x61
#define M2PA_VERSION_DRAFT6_9	0x69
#define M2PA_VERSION_DRAFT10	0xa0
#define M2PA_VERSION_DRAFT11	0xb0
#define M2PA_VERISON_RFC4165	0xc1
#define M2PA_VERSION_DEFAULT	M2PA_VERSION_RFC4165

/*
 *  M2PA PDU Message Definitions
 */
#define M2PA_PPI		5
#define M2PA_MESSAGE_CLASS	11
#define M2PA_VERSION		1
#define M2PA_MTYPE_DATA		1
#define M2PA_MTYPE_STATUS	2
#define M2PA_MTYPE_PROVING	3
#define M2PA_MTYPE_ACK		4
#define M2PA_DATA_MESSAGE	__constant_htonl((M2PA_VERSION<<24)|(M2PA_MESSAGE_CLASS<<8)|M2PA_MTYPE_DATA)
#define M2PA_STATUS_MESSAGE	__constant_htonl((M2PA_VERSION<<24)|(M2PA_MESSAGE_CLASS<<8)|M2PA_MTYPE_STATUS)
#define M2PA_PROVING_MESSAGE	__constant_htonl((M2PA_VERSION<<24)|(M2PA_MESSAGE_CLASS<<8)|M2PA_MTYPE_PROVING)
#define M2PA_ACK_MESSAGE	__constant_htonl((M2PA_VERSION<<24)|(M2PA_MESSAGE_CLASS<<8)|M2PA_MTYPE_ACK)
#define M2PA_STATUS_STREAM	0
#define M2PA_DATA_STREAM	1
#define M2PA_STATUS_ALIGNMENT			(__constant_htonl(1))
#define M2PA_STATUS_PROVING_NORMAL		(__constant_htonl(2))
#define M2PA_STATUS_PROVING_EMERGENCY		(__constant_htonl(3))
#define M2PA_STATUS_IN_SERVICE			(__constant_htonl(4))
#define M2PA_STATUS_PROCESSOR_OUTAGE		(__constant_htonl(5))
#define M2PA_STATUS_PROCESSOR_OUTAGE_ENDED	(__constant_htonl(6))
#define M2PA_STATUS_BUSY			(__constant_htonl(7))
#define M2PA_STATUS_BUSY_ENDED			(__constant_htonl(8))
#define	M2PA_STATUS_OUT_OF_SERVICE		(__constant_htonl(9))
#define M2PA_STATUS_ACK				(__constant_htonl(10))

#ifndef PAD4
#define PAD4(__x) (((__x)+0x3)&~0x3)
#endif

/*
 * Message block allocation.
 */
/**
 * dl_allocb: - allocate a message block reliably
 * @dl: private structure
 * @q: active queue
 * @size: size of data block
 * @pri: allocation priority
 */
STATIC streams_fastcall mblk_t *
dl_allocb(struct dl *dl, queue_t *q, size_t size, int pri)
{
	mblk_t *mp;

	if (likely(mp = allocb(size, pri)))
		return (mp);

#if 0
	if (q == dl->oq)
		unbufcall(xchg(&dl->obid, bufcall()));
	if (q == dl->iq)
		unbufcall(xchg(&dl->ibid, bufcall()));
#else
	mi_bufcall(q, size, pri);
#endif

	return (NULL);
}

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
#define DLM_PENDING		(DLF_ATTACH_PENDING |\
				 DLF_DETACH_PENDING |\
				 DLF_BIND_PENDING |\
				 DLF_UNBIND_PENDING |\
				 DLF_UDQOS_PENDING |\
				 DLF_OUTCON_PENDING |\
				 DLF_INCON_PENDING |\
				 DLF_CONN_RES_PENDING |\
				 DLF_USER_RESET_PENDING |\
				 DLF_PROV_RESET_PENDING |\
				 DLF_RESET_RES_PENDING |\
				 DLF_DISCON8_PENDING |\
				 DLF_DISCON9_PENDING |\
				 DLF_DISCON11_PENDING |\
				 DLF_DISCON12_PENDING |\
				 DLF_DISCON13_PENDING |\
				 DLF_SUBS_BIND_PND |\
				 DLF_SUBS_UNBIND_PND)
#define DLM_NOTPENDING		(DLF_UNATTACHED |\
				 DLF_UNBOUND |\
				 DLF_IDLE |\
				 DLF_DATAXFER)

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
dl_info_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl->info) + sizeof(dl->qos) + sizeof(dl->qor);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		/* copy directly from private structure */
		bcopy(&dl->info, mp->b_wptr, size);
		mp->b_wptr += size;
		printd(("%s: %p: <- DL_INFO_ACK\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
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
dl_bind_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_bind_ack_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_bind_ack_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_BIND_ACK;
		p->dl_sap = 0;
		p->dl_addr_length = 0;
		p->dl_addr_offset = 0;
		p->dl_max_conind = 0;
		p->dl_xidtest_flg = 0;
		printd(("%s: %p: <- DL_BIND_ACK\n", MOD_NAME, dl));
		dl_set_state(dl, DL_IDLE);
		putnext(dl->oq, mp);
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
dl_ok_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong dl_correct_primitive)
{
	mblk_t *mp;
	dl_ok_ack_t *p;

	if ((mp = dl_allocb(dl, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
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
		putnext(dl->oq, mp);
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
dl_error_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong dl_error_primitive, dl_ulong dl_errno,
	     dl_ulong dl_unix_errno)
{
	mblk_t *mp;
	dl_error_ack_t *p;

	if ((mp = dl_allocb(dl, q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
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
		putnext(dl->oq, mp);
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
dl_subs_bind_ack(struct dl *dl, queue_t *q, mblk_t *mp, caddr_t ptr, size_t len)
{
	mblk_t *mp;
	size_t size = sizeof(dl_subs_bind_ack_t) + len;

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_subs_bind_ack_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_SUBS_BIND_ACK;
		bcopy(ptr, mp->b_wptr, len);
		mp->b_wptr += len;
		printd(("%s: %p: <- DL_SUBS_BIND_ACK\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_unitdata_ind: - send a DL_UNITDATA_IND upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_unitdata_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_unitdata_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_unitdata_ind_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_UNITDATA_IND;
		printd(("%s: %p: <- DL_UNITDATA_IND\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
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
dl_uderror_ind(struct dl *dl, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen,
	       dl_ulong error, dl_long errno)
{
	mblk_t *mp;
	size_t size = sizeof(dl_uderror_ind_t) + alen;

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_uderror_ind_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_UDERROR_IND;
		p->dl_dest_addr_length = alen;
		p->dl_dest_addr_offset = alen ? sizeof(*p) : 0;
		p->dl_unix_errno = errno < 0 ? -errno : errno;
		p->dl_errno = error;
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		printd(("%s: %p: <- DL_UDERROR_IND\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
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
dl_connect_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_connect_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_connect_ind_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_CONNECT_IND;
		printd(("%s: %p: <- DL_CONNECT_IND\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
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
dl_connect_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_connect_con_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_connect_con_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_CONNECT_CON;
		printd(("%s: %p: <- DL_CONNECT_CON\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
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
dl_token_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_token_ack_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_token_ack_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_TOKEN_ACK;
		p->dl_token = dl->u.index;
		printd(("%s: %p: <- DL_TOKEN_ACK\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
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
dl_disconnect_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_disconnect_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_disconnect_ind_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_DISCONNECT_IND;
		printd(("%s: %p: <- DL_DISCONNECT_IND\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
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
dl_reset_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong origin, dl_ulong reason)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_reset_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_reset_ind_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_RESET_IND;
		p->dl_originator = origin;
		p->dl_reason = reason;
		printd(("%s: %p: <- DL_RESET_IND\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
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
dl_reset_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_reset_con_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_reset_con_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_RESET_CON;
		printd(("%s: %p: <- DL_RESET_CON\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
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
 */
STATIC streams_fastcall int
dl_data_ack_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_data_ack_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_data_ack_ind_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_DATA_ACK_IND;
		printd(("%s: %p: <- DL_DATA_ACK_IND\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_data_ack_status_ind: - send a DL_DATA_ACK_STATUS_IND upstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
dl_data_ack_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_data_ack_status_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_data_ack_status_ind_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_DATA_ACK_STATUS_IND;
		printd(("%s: %p: <- DL_DATA_ACK_STATUS_IND\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
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
dl_reply_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_reply_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_reply_ind_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_REPLY_IND;
		printd(("%s: %p: <- DL_REPLY_IND\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
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
dl_reply_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_reply_status_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_reply_status_ind_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_REPLY_STATUS_IND;
		printd(("%s: %p: <- DL_REPLY_STATUS_IND\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
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
dl_reply_update_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_reply_update_status_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_reply_update_status_ind_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_REPLY_UPDATE_STATUS_IND;
		printd(("%s: %p: <- DL_REPLY_UPDATE_STATUS_IND\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
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
dl_xid_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_xid_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_xid_ind_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_XID_IND;
		printd(("%s: %p: <- DL_XID_IND\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
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
dl_xid_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_xid_con_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_xid_con_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_XID_CON;
		printd(("%s: %p: <- DL_XID_CON\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
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
dl_test_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_test_ind_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_test_ind_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_TEST_IND;
		printd(("%s: %p: <- DL_TEST_IND\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
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
dl_test_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(dl_test_con_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		dl_test_con_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_TEST_CON;
		printd(("%s: %p: <- DL_TEST_CON\n", MOD_NAME, dl));
		putnext(dl->oq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * NPI User Messages.
 *
 * These are messages that are sent downstream to the SCTP NPI provider.
 */
/**
 * n_conn_req:- send a N_CONN_REQ downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
n_conn_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(N_conn_req_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		N_conn_req_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->PRIM_type = N_CONN_REQ;
		printd(("%s: %p: N_CONN_REQ ->\n", MOD_NAME, dl));
		putnext(dl->iq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_conn_res:- send a N_CONN_RES downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
n_conn_res(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(N_conn_res_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		N_conn_res_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->PRIM_type = N_CONN_RES;
		printd(("%s: %p: N_CONN_RES ->\n", MOD_NAME, dl));
		putnext(dl->iq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_discon_req:- send a N_DISCON_REQ downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
n_discon_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(N_discon_req_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		N_discon_req_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->PRIM_type = N_DISCON_REQ;
		printd(("%s: %p: N_DISCON_REQ ->\n", MOD_NAME, dl));
		putnext(dl->iq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_data_req:- send a N_DATA_REQ downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
n_data_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(N_data_req_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		N_data_req_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->PRIM_type = N_DATA_REQ;
		printd(("%s: %p: N_DATA_REQ ->\n", MOD_NAME, dl));
		putnext(dl->iq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_exdata_req:- send a N_EXDATA_REQ downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
n_exdata_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(N_exdata_req_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		N_exdata_req_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->PRIM_type = N_EXDATA_REQ;
		printd(("%s: %p: N_EXDATA_REQ ->\n", MOD_NAME, dl));
		putnext(dl->iq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_info_req:- send a N_INFO_REQ downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
n_info_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(N_info_req_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		N_info_req_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->PRIM_type = N_INFO_REQ;
		printd(("%s: %p: N_INFO_REQ ->\n", MOD_NAME, dl));
		putnext(dl->iq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_bind_req:- send a N_BIND_REQ downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
n_bind_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(N_bind_req_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		N_bind_req_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->PRIM_type = N_BIND_REQ;
		printd(("%s: %p: N_BIND_REQ ->\n", MOD_NAME, dl));
		putnext(dl->iq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_unbind_req:- send a N_UNBIND_REQ downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
n_unbind_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(N_unbind_req_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		N_unbind_req_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->PRIM_type = N_UNBIND_REQ;
		printd(("%s: %p: N_UNBIND_REQ ->\n", MOD_NAME, dl));
		putnext(dl->iq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_unitdata_req:- send a N_UNITDATA_REQ downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
n_unitdata_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(N_unitdata_req_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		N_unitdata_req_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->PRIM_type = N_UNITDATA_REQ;
		printd(("%s: %p: N_UNITDATA_REQ ->\n", MOD_NAME, dl));
		putnext(dl->iq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_optmgmt_req:- send a N_OPTMGMT_REQ downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
n_optmgmt_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(N_optmgmt_req_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		N_optmgmt_req_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->PRIM_type = N_OPTMGMT_REQ;
		printd(("%s: %p: N_OPTMGMT_REQ ->\n", MOD_NAME, dl));
		putnext(dl->iq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_datack_req:- send a N_DATACK_REQ downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
n_datack_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(N_datack_req_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		N_datack_req_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->PRIM_type = N_DATACK_REQ;
		printd(("%s: %p: N_DATACK_REQ ->\n", MOD_NAME, dl));
		putnext(dl->iq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_reset_req:- send a N_RESET_REQ downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
n_reset_req(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(N_reset_req_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		N_reset_req_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->PRIM_type = N_RESET_REQ;
		printd(("%s: %p: N_RESET_REQ ->\n", MOD_NAME, dl));
		putnext(dl->iq, mp);
		freemsg(msg);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_reset_res:- send a N_RESET_RES downstream
 * @dl: private structure
 * @q: active queue
 * @msg: message to free if successful
 */
STATIC streams_fastcall int
n_reset_res(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	const size_t size = sizeof(N_reset_res_t);

	if ((mp = dl_allocb(dl, q, size, BPRI_MED))) {
		N_reset_res_t *p;

		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->PRIM_type = N_RESET_RES;
		printd(("%s: %p: N_RESET_RES ->\n", MOD_NAME, dl));
		putnext(dl->iq, mp);
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
dl_info_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_info_req_t *p;

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
dl_attach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_attach_req_t *p;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!dl_lock_state(dl, DLF_UNATTACHED))
		goto outstate;
	dl_set_state(dl, DL_ATTACH_PENDING);
	dl_unlock_state(dl);
	/* have to actually do the attach */
	return dl_ok_ack(dl, q, mp, DL_ATTACH_REQ);
      oustate:
	return dl_error_ack(dl, q, mp, DL_ATTACH_REQ, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(dl, q, mp, DL_ATTACH_REQ, DL_BADPRIM, EINVAL);
}

/**
 * dl_bind_req: - process DL_BIND_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * Implements SL_POWER_ON_REQ, however, this is a no-op for M2PA.
 */
STATIC streams_fastcall int
dl_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_bind_req_t *p;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!dl_lock_state(dl, DLF_UNBOUND))
		goto outstate;
	dl_set_state(dl, DL_BIND_PENDING);
	dl_unlock_state(dl);
	/* actually do the bind */
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
	/* nothing to do */
	return dl_bind_ack(dl, q, mp);
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
dl_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_unbind_req_t *p;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (!dl_lock_state(dl, DLF_IDLE))
		goto outstate;
	dl_set_state(dl, DL_UNBIND_PENDING);
	dl_unlock_state(dl);
	return dl_ok_ack(dl, q, mp, DL_UNBIND_REQ);
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
dl_subs_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_bind_req_t *p;

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
dl_subs_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_unbind_req_t *p;

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
dl_enabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_enabmulti_req_t *p;

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
dl_disabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_disabmulti_req_t *p;

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
dl_promiscon_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_promiscon_req_t *p;

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
dl_promiscoff_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_promiscoff_req_t *p;

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
dl_unitdata_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_unitdata_req_t *p;
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
 *
 * M2PA might need to use this to support setting and clearing of emergency or
 * flushing of buffers.
 */
STATIC streams_fastcall int
dl_udqos_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_udqos_req_t *p;

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
dl_connect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_req_t *p;
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
 *
 * The M2PA DL provider never issues a DL_CONNECT_IND, so this primitive is
 * always in error and will result in DL_OUTSTATE.
 */
STATIC streams_fastcall int
dl_connect_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_res_t *p;

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
dl_token_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_token_req_t *p;

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
dl_disconnect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_disconnect_req_t *p;

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
dl_reset_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reset_req_t *p;

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
dl_reset_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reset_res_t *p;

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
dl_data_ack_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_data_ack_req_t *p;

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
dl_reply_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reply_req_t *p;

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
dl_reply_update_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reply_update_req_t *p;

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
dl_xid_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_xid_req_t *p;

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
dl_xid_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_xid_res_t *p;

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
dl_test_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_test_req_t *p;

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
dl_test_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_test_res_t *p;

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
dl_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_phys_addr_req_t *p;

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
dl_set_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_set_phys_addr_req_t *p;

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
dl_get_statistics_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_get_statistics_req_t *p;

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
dl_notsupported(struct dl *dl, queue_t *q, mblk_t *mp)
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
dl_unknown_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_ack(dl, q, mp, DL_UNKNOWN_REQ, DL_BADPRIM, EINVAL);
}

/**
 * n_conn_ind: - process N_CONN_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_IND primitive
 */
STATIC streams_fastcall int
n_conn_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	N_conn_ind_t *p;

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
 * n_conn_con: - process N_CONN_CON primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the N_CONN_CON primitive
 */
STATIC streams_fastcall int
n_conn_con(struct dl *dl, queue_t *q, mblk_t *mp)
{
	N_conn_con_t *p;

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
 * n_discon_ind: - process N_DISCON_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the N_DISCON_IND primitive
 */
STATIC streams_fastcall int
n_discon_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	N_discon_ind_t *p;

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
 * n_data_ind: - process N_DATA_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the N_DATA_IND primitive
 */
STATIC streams_fastcall int
n_data_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p;

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
 * n_exdata_ind: - process N_EXDATA_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the N_EXDATA_IND primitive
 */
STATIC streams_fastcall int
n_exdata_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	N_exdata_ind_t *p;

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
 * n_info_ack: - process N_INFO_ACK primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the N_INFO_ACK primitive
 */
STATIC streams_fastcall int
n_info_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	N_info_ack_t *p;

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
 * n_bind_ack: - process N_BIND_ACK primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the N_BIND_ACK primitive
 */
STATIC streams_fastcall int
n_bind_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	N_bind_ack_t *p;

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
 * n_error_ack: - process N_ERROR_ACK primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the N_ERROR_ACK primitive
 */
STATIC streams_fastcall int
n_error_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	N_error_ack_t *p;

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
 * n_ok_ack: - process N_OK_ACK primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the N_OK_ACK primitive
 */
STATIC streams_fastcall int
n_ok_ack(struct dl *dl, queue_t *q, mblk_t *mp)
{
	N_ok_ack_t *p;

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
 * n_unitdata_ind: - process N_UNITDATA_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the N_UNITDATA_IND primitive
 */
STATIC streams_fastcall int
n_unitdata_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	N_unitdata_ind_t *p;

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
 * n_uderror_ind: - process N_UDERROR_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the N_UDERROR_IND primitive
 */
STATIC streams_fastcall int
n_uderror_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	N_uderror_ind_t *p;

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
 * n_datack_ind: - process N_DATACK_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the N_DATACK_IND primitive
 */
STATIC streams_fastcall int
n_datack_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	N_datack_ind_t *p;

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
 * n_reset_ind: - process N_RESET_IND primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the N_RESET_IND primitive
 */
STATIC streams_fastcall int
n_reset_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	N_reset_ind_t *p;

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
 * n_reset_con: - process N_RESET_CON primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the N_RESET_CON primitive
 */
STATIC streams_fastcall int
n_reset_con(struct dl *dl, queue_t *q, mblk_t *mp)
{
	N_reset_con_t *p;

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
 * n_unknown_ind: - process unknown primitive
 * @dl: private structure
 * @q: active queue (read queue)
 * @mp: the unknown primitive
 */
STATIC streams_fastcall int
n_unknown_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	swerr();
	freemsg(mp);
	return (0);
}

STATIC streams_fastcall int
dl_w_proto(struct dl *dl, queue_t *q, mblk_t *mp)
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
	putnextctl2(dl->iq, M_ERROR, EPROTO, EPROTO);
	freemsg(mp);
	return (0);
}

STATIC streams_fastcall int
dl_r_proto(struct dl *dl, queue_t *q, mblk_t *mp)
{
	np_ulong prim;

	switch ((prim = *(np_ulong *) mp->b_rptr)) {
	case N_CONN_IND:
		return n_conn_ind(dl, q, mp);
	case N_CONN_CON:
		return n_conn_con(dl, q, mp);
	case N_DISCON_IND:
		return n_discon_ind(dl, q, mp);
	case N_DATA_IND:
		return n_data_ind(dl, q, mp);
	case N_EXDATA_IND:
		return n_exdata_ind(dl, q, mp);
	case N_INFO_ACK:
		return n_info_ack(dl, q, mp);
	case N_BIND_ACK:
		return n_bind_ack(dl, q, mp);
	case N_ERROR_ACK:
		return n_error_ack(dl, q, mp);
	case N_OK_ACK:
		return n_ok_ack(dl, q, mp);
	case N_UNITDATA_IND:
		return n_unitdata_ind(dl, q, mp);
	case N_UDERROR_IND:
		return n_uderror_ind(dl, q, mp);
	case N_DATACK_IND:
		return n_datack_ind(dl, q, mp);
	case N_RESET_IND:
		return n_reset_ind(dl, q, mp);
	case N_RESET_CON:
		return n_reset_con(dl, q, mp);
	default:
		return n_unknown_ind(dl, q, mp);
	}
	freemsg(mp);
	return (0);
}

STATIC streams_fastcall int
dl_wmsg(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);

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
dl_rmsg(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);

	switch (mp->b_datap->db_type) {
	case M_COPYIN:
	case M_COPYOUT:
	case M_IOCACK:
		return dl_r_ioctl(dl, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_r_proto(dl, q, mp);
	case M_DATA:
		return dl_r_data(dl, q, mp);
	case M_CTL:
		return dl_r_ctl(dl, q, mp);
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
dl_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (dl_rmsg(q, mp)) {
			if (putbq(q, mp))
				return (0);
			swerr();
			freemsg(mp);
		}
	}
	return (0);
}

STATIC streamscall int
dl_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(mp->b_datap->db_type) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || dl_rmsg(q, mp)) {
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

STATIC struct module_stat dl_rstat = __attribute__ ((__aligned__(SMB_CACHE_BYTES)));
STATIC struct module_stat dl_wstat = __attribute__ ((__aligned__(SMB_CACHE_BYTES)));

STATIC struct qinit dl_rinit = {
	.qi_putp = &dl_rput,
	.qi_srvp = &dl_rsrv,
	.qi_qopen = &dl_open,
	.qi_qclose = &dl_close,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_rstat,
};

STATIC struct qinit dl_winit = {
	.qi_putp = &dl_wput,
	.qi_srvp = &dl_wsrv,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_wstat,
};

STATIC struct streamtab m2pa_dlinfo = {
	.st_rdinit = &dl_rinit,
	.st_wrinit = &dl_winit,
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
MODULE_PARM_DESC(modid, "Module ID fo the M2PA-DL module. (0 for allocation).");

#ifdef LIS
#define fmodsw _fmodsw
#endif
STATIC struct fmodsw dl_fmod = {
	.f_name = MOD_NAME,
	.f_str = &m2pa_dlinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

MODULE_STATIC int __init
m2pa_dlinit(void)
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
m2pa_dlterminate(void)
{
	if ((err = unregister_strmod(&dl_fmod)))
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, -err);
	return;
}

/*
 *  Linux kernel module initialization.
 */
module_init(m2pa_dlinit);
module_exit(m2pa_dlterminate);
#endif				/* LINUX */
