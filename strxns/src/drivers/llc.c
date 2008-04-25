/*****************************************************************************

 @(#) $RCSfile: llc.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-25 11:39:32 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2008-04-25 11:39:32 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: llc.c,v $
 Revision 0.9.2.4  2008-04-25 11:39:32  brian
 - updates to AGPLv3

 Revision 0.9.2.3  2007/08/19 12:03:19  brian
 - move stdbool.h to compat.h

 Revision 0.9.2.2  2007/08/14 03:31:09  brian
 - GPLv3 header update

 Revision 0.9.2.1  2007/08/12 15:20:26  brian
 - added new files

 *****************************************************************************/

#ident "@(#) $RCSfile: llc.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-25 11:39:32 $"

static char const ident[] =
    "$RCSfile: llc.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-25 11:39:32 $";

/*
 * This is a DL driver for LLC1, LLC2 and LLC3 connections over 802.2.  It
 * hooks into the Linux networking layer and has access to all 802.2 devices
 * from there.
 */

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

#include <ss7/os8/compat.h>

#include <sys/dlpi.h>

#define DL_DESCRIP	"Data Link (DL) for IEEE 802.2 LLC STREAMS DRIVER."
#define DL_REVISION	"OpenSS7 $RCSfile: llc.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-25 11:39:32 $"
#define DL_COPYRIGHT	"Copyright (c) 1997-2007  OpenSS7 Corporation.  All Rights Reserved."
#define DL_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define DL_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define DL_LICENSE	"GPL"
#define DL_BANNER	DL_DESCRIP	"\n" \
			DL_REVISION	"\n" \
			DL_COPYRIGHT	"\n" \
			DL_DEVICE	"\n" \
			DL_CONTACT	"\n"
#define DL_SPLASH	DL_DESCRIP	" - " \
			DL_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(DL_CONTACT);
MODULE_SUPPORTED_DEVICE(DL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(DL_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_LLC_MODID));
MODULE_ALIAS("streams-driver-llc");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_LLC_MAJOR));
MODULE_ALIAS("/dev/streams/llc");
MODULE_ALIAS("/dev/streams/llc/*");
MODULE_ALIAS("/dev/streams/clone/llc");
#endif				/* LFS */
MODULE_ALIAS("streams-llc");
MODULE_ALIAS("char-major-" __stringify(LLC_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(LLC_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(LLC_CMAJOR_0) "-0");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define LLC_DRV_ID		CONFIG_STREAMS_LLC_MODID
#define LLC_DRV_NAME		CONFIG_STREAMS_LLC_NAME
#define LLC_CMAJORS		CONFIG_STREAMS_LLC_NMAJORS
#define LLC_CMAJOR_0		CONFIG_STREAMS_LLC_MAJOR
#define LLC_UNITS		CONFIG_STREAMS_LLC_NMINORS
#endif				/* LFS */

#ifndef LLC_DRV_NAME
#define LLC_DRV_NAME	"llc"
#endif				/* LLC_DRV_NAME */

#ifndef LLC_DRV_ID
#define LLC_DRV_ID	0
#endif				/* LLC_DRV_ID */

/*
 *  =========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  =========================================================================
 */

#define DRV_ID		LLC_DRV_ID
#define DRV_NAME	LLC_DRV_NAME
#ifdef MODULE
#define DRV_BANNER	DL_BANNER
#else				/* MODULE */
#define DRV_BANNER	DL_SPLASH
#endif				/* MODULE */

static struct module_info dl_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat dl_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat dl_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  =========================================================================
 *
 *  PRIVATE STRUCTURE
 *
 *  =========================================================================
 */

struct st {
	int l_state;			/* link state */
	int i_state;			/* interface state */
	int i_flags;			/* interface flags */
};

struct dl {
	queue_t *wq;
	queue_t *rq;
	struct net_device *ndev;
	struct st state, oldstate;
	dl_info_ack_t prot;
	union {
		dl_qos_cl_sel1_t cl;
		dl_qos_co_sel1_t co;
	} qos;
	union {
		dl_qos_cl_range1_t cl;
		dl_qos_co_range1_t co;
	} qor;
	uchar add[MAX_ADDR_LENGTH];
	uchar bca[MAX_ADDR_LENGTH];
};

#define PRIV(q)	    ((struct dl *)(q)->q_ptr)
#define DL_PRIV(q)  PRIV(q)

static kmem_cachep_t dl_priv_cachep = NULL;

#define STRLOGNO	0	/* log Streams errors */
#define STRLOGIO	1	/* log Streams input-ouput */
#define STRLOGST	2	/* log Streams state transitions */
#define STRLOGTO	3	/* log Streams timeouts */
#define STRLOGRX	4	/* log Streams primitives received */
#define STRLOGTX	5	/* log Streams primitives issued */
#define STRLOGTE	6	/* log Streams timer events */
#define STRLOGDA	7	/* log Streams data */

static rwlock_t dl_drv_lock = RWLOCK_UNLOCKED;

/**
 * dl_iocname: display DL ioctl command name
 * @cmd: ioctl command
 */
static const char *
dl_iocname(int cmd)
{
	switch (_IOC_NR(cmd)) {
	case _IOC_NR(DL_IOCGOPTIONS):
		return ("DL_IOCGOPTIONS");
	case _IOC_NR(DL_IOCSOPTIONS):
		return ("DL_IOCSOPTIONS");
	case _IOC_NR(DL_IOCGCONFIG):
		return ("DL_IOCGCONFIG");
	case _IOC_NR(DL_IOCSCONFIG):
		return ("DL_IOCSCONFIG");
	case _IOC_NR(DL_IOCTCONFIG):
		return ("DL_IOCTCONFIG");
	case _IOC_NR(DL_IOCCCONFIG):
		return ("DL_IOCCCONFIG");
	case _IOC_NR(DL_IOCGSTATEM):
		return ("DL_IOCGSTATEM");
	case _IOC_NR(DL_IOCCMRESET):
		return ("DL_IOCCMRESET");
	case _IOC_NR(DL_IOCGSTATSP):
		return ("DL_IOCGSTATSP");
	case _IOC_NR(DL_IOCSSTATSP):
		return ("DL_IOCSSTATSP");
	case _IOC_NR(DL_IOCGSTATS):
		return ("DL_IOCGSTATS");
	case _IOC_NR(DL_IOCCSTATS):
		return ("DL_IOCCSTATS");
	case _IOC_NR(DL_IOCGNOTIFY):
		return ("DL_IOCGNOTIFY");
	case _IOC_NR(DL_IOCSNOTIFY):
		return ("DL_IOCSNOTIFY");
	case _IOC_NR(DL_IOCCNOTIFY):
		return ("DL_IOCCNOTIFY");
	default:
		return ("(unknown ioctl)");
	}

}

/**
 * dl_primname: - display DL primitive name
 * @prim: the primitive to display
 */
static const char *
dl_primname(dl_ulong prim)
{
	switch (prim) {
	case DL_INFO_REQ:
		return ("DL_INFO_REQ");
	case DL_INFO_ACK:
		return ("DL_INFO_ACK");
	case DL_ATTACH_REQ:
		return ("DL_ATTACH_REQ");
	case DL_DETACH_REQ:
		return ("DL_DETACH_REQ");
	case DL_BIND_REQ:
		return ("DL_BIND_REQ");
	case DL_BIND_ACK:
		return ("DL_BIND_ACK");
	case DL_UNBIND_REQ:
		return ("DL_UNBIND_REQ");
	case DL_OK_ACK:
		return ("DL_OK_ACK");
	case DL_ERROR_ACK:
		return ("DL_ERROR_ACK");
	case DL_SUBS_BIND_REQ:
		return ("DL_SUBS_BIND_REQ");
	case DL_SUBS_BIND_ACK:
		return ("DL_SUBS_BIND_ACK");
	case DL_SUBS_UNBIND_REQ:
		return ("DL_SUBS_UNBIND_REQ");
	case DL_ENABMULTI_REQ:
		return ("DL_ENABMULTI_REQ");
	case DL_DISABMULTI_REQ:
		return ("DL_DISABMULTI_REQ");
	case DL_PROMISCON_REQ:
		return ("DL_PROMISCON_REQ");
	case DL_PROMISCOFF_REQ:
		return ("DL_PROMISCOFF_REQ");
	case DL_UNITDATA_REQ:
		return ("DL_UNITDATA_REQ");
	case DL_UNITDATA_IND:
		return ("DL_UNITDATA_IND");
	case DL_UDERROR_IND:
		return ("DL_UDERROR_IND");
	case DL_UDQOS_REQ:
		return ("DL_UDQOS_REQ");
	case DL_CONNECT_REQ:
		return ("DL_CONNECT_REQ");
	case DL_CONNECT_IND:
		return ("DL_CONNECT_IND");
	case DL_CONNECT_RES:
		return ("DL_CONNECT_RES");
	case DL_CONNECT_CON:
		return ("DL_CONNECT_CON");
	case DL_TOKEN_REQ:
		return ("DL_TOKEN_REQ");
	case DL_TOKEN_ACK:
		return ("DL_TOKEN_ACK");
	case DL_DISCONNECT_REQ:
		return ("DL_DISCONNECT_REQ");
	case DL_DISCONNECT_IND:
		return ("DL_DISCONNECT_IND");
	case DL_RESET_REQ:
		return ("DL_RESET_REQ");
	case DL_RESET_IND:
		return ("DL_RESET_IND");
	case DL_RESET_RES:
		return ("DL_RESET_RES");
	case DL_RESET_CON:
		return ("DL_RESET_CON");
	case DL_DATA_ACK_REQ:
		return ("DL_DATA_ACK_REQ");
	case DL_DATA_ACK_IND:
		return ("DL_DATA_ACK_IND");
	case DL_DATA_ACK_STATUS_IND:
		return ("DL_DATA_ACK_STATUS_IND");
	case DL_REPLY_REQ:
		return ("DL_REPLY_REQ");
	case DL_REPLY_IND:
		return ("DL_REPLY_IND");
	case DL_REPLY_STATUS_IND:
		return ("DL_REPLY_STATUS_IND");
	case DL_REPLY_UPDATE_REQ:
		return ("DL_REPLY_UPDATE_REQ");
	case DL_REPLY_UPDATE_STATUS_IND:
		return ("DL_REPLY_UPDATE_STATUS_IND");
	case DL_XID_REQ:
		return ("DL_XID_REQ");
	case DL_XID_IND:
		return ("DL_XID_IND");
	case DL_XID_RES:
		return ("DL_XID_RES");
	case DL_XID_CON:
		return ("DL_XID_CON");
	case DL_TEST_REQ:
		return ("DL_TEST_REQ");
	case DL_TEST_IND:
		return ("DL_TEST_IND");
	case DL_TEST_RES:
		return ("DL_TEST_RES");
	case DL_TEST_CON:
		return ("DL_TEST_CON");
	case DL_PHYS_ADDR_REQ:
		return ("DL_PHYS_ADDR_REQ");
	case DL_PHYS_ADDR_ACK:
		return ("DL_PHYS_ADDR_ACK");
	case DL_SET_PHYS_ADDR_REQ:
		return ("DL_SET_PHYS_ADDR_REQ");
	case DL_GET_STATISTICS_REQ:
		return ("DL_GET_STATISTICS_REQ");
	case DL_GET_STATISTICS_ACK:
		return ("DL_GET_STATISTICS_ACK");
	default:
		return ("(unknown primitive)");
	}
}

static const char *
dl_i_state_name(dl_ulong state)
{
	switch (state) {
	case DL_UNATTACHED:
		return ("DL_UNATTACHED");
	case DL_ATTACH_PENDING:
		return ("DL_ATTACH_PENDING");
	case DL_DETACH_PENDING:
		return ("DL_DETACH_PENDING");
	case DL_UNBOUND:
		return ("DL_UNBOUND");
	case DL_BIND_PENDING:
		return ("DL_BIND_PENDING");
	case DL_UNBIND_PENDING:
		return ("DL_UNBIND_PENDING");
	case DL_IDLE:
		return ("DL_IDLE");
	case DL_UDQOS_PENDING:
		return ("DL_UDQOS_PENDING");
	case DL_OUTCON_PENDING:
		return ("DL_OUTCON_PENDING");
	case DL_INCON_PENDING:
		return ("DL_INCON_PENDING");
	case DL_CONN_RES_PENDING:
		return ("DL_CONN_RES_PENDING");
	case DL_DATAXFER:
		return ("DL_DATAXFER");
	case DL_USER_RESET_PENDING:
		return ("DL_USER_RESET_PENDING");
	case DL_PROV_RESET_PENDING:
		return ("DL_PROV_RESET_PENDING");
	case DL_RESET_RES_PENDING:
		return ("DL_RESET_RES_PENDING");
	case DL_DISCON8_PENDING:
		return ("DL_DISCON8_PENDING");
	case DL_DISCON9_PENDING:
		return ("DL_DISCON9_PENDING");
	case DL_DISCON11_PENDING:
		return ("DL_DISCON11_PENDING");
	case DL_DISCON12_PENDING:
		return ("DL_DISCON12_PENDING");
	case DL_DISCON13_PENDING:
		return ("DL_DISCON13_PENDING");
	case DL_SUBS_BIND_PND:
		return ("DL_SUBS_BIND_PND");
	case DL_SUBS_UNBIND_PND:
		return ("DL_SUBS_UNBIND_PND");
	default:
		return ("(unknown state)");
	}
}

/*
 *  =========================================================================
 *
 *  STATE TRANSITIONS
 *
 *  =========================================================================
 */

static inline fastcall dl_ulong
dl_get_i_state(struct dl *dl)
{
	return (dl->state.i_state);
}

static inline fastcall dl_ulong
dl_set_i_state(struct dl *dl, dl_ulong newstate)
{
	dl_ulong oldstate = dl_get_i_state(dl);

	if (newstate != oldstate) {
		dl->state.i_state = newstate;
		mi_strlog(dl->wq, STRLOGST, SL_TRACE, "%s <- %s", dl_i_state_name(newstate),
			  dl_i_state_name(oldstate));
	}
	return (newstate)
}

static inline fastcall dl_ulong
dl_save_i_state(struct dl *dl)
{
	return ((dl->oldstate.i_state = dl_get_i_state(dl)));
}

static inline fastcall dl_ulong
dl_restore_i_state(struct dl *dl)
{
	return (dl_set_i_state(dl, dl->oldstate.i_state));
}

/*
 *  =========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  DL Provider -> DL User primitives.
 *
 *  =========================================================================
 */

static inline fastcall
m_error(struct dl *dl, queue_t *q, mblk_t *msg, int err)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, 2, BPRI_MED)))) {
		DB_TYPE(mp) = M_ERROR;
		*mp->b_wptr++ = err < 0 ? -err : err;
		*mp->b_wptr++ = err < 0 ? -err : err;
		freemsg(msg);
		mi_strlog(dl->rq, 0, SL_ERROR | SL_TRACE, "<- M_ERROR %d", err < 0 ? -err : err);
		dl_set_i_state(dl, -1);
		putnext(dl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_info_ack: - issue DL_INFO_ACK primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_info_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_info_ack_t *p;
	mblk_t *mp;

	size_t msg_len =
	    sizeof(*p) + dl->prot.dl_qos_length + dl->prot.dl_qos_range_length +
	    dl->prot.dl_addr_length, dl->prot.dl_brdcst_addr_length;

	if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_INFO_ACK;
		p->dl_min_sdu = dl->prot.dl_min_sdu;
		p->dl_max_sdu = dl->prot.dl_max_sdu;
		p->dl_reserved = 0;
		p->dl_current_state = dl_get_i_state(dl);
		p->dl_sap_length = -dl->prot.dl_sap_length;
		p->dl_service_mode = dl->prot.dl_service_mode;
		p->dl_provider_style = DL_STYLE2;
		p->dl_version = DL_CURRENT_VERSION;
		p->dl_growth = 0;
		p->dl_qos_length = dl->prot.dl_qos_length;
		p->dl_qos_offset = sizeof(*p);
		p->dl_qos_range_length = dl->prot.dl_qos_range_length;
		p->dl_qos_range_offset = sizeof(*p) + dl->prot.dl_qos_length;
		p->dl_addr_length = dl->prot.dl_addr_length;
		p->dl_addr_offset =
		    sizeof(*p) + dl->prot.dl_qos_length + dl->prot.dl_qos_range_length;
		p->dl_brdcst_addr_length = dl->prot.dl_brdcst_addr_length;
		p->dl_brdcst_addr_offset =
		    sizeof(*p) + dl->prot.dl_qos_length + dl->prot.dl_qos_range_length +
		    dl->prot.dl_addr_length;
		p->dl_mac_type = dl->mac_type;
		mp->b_wptr += sizeof(*p);
		bcopy(&dl->qos, mp->b_wptr, dl->prot.dl_qos_length);
		mp->b_wptr += dl->prot.dl_qos_length;
		bcopy(&dl->qor, mp->b_wptr, dl->prot.dl_qos_range_length);
		mp->b_wptr += dl->prot.dl_qos_range_length;
		bcopy(&dl->add, mp->b_wptr, dl->prot.dl_addr_length);
		mp->b_wptr += dl->prot.dl_addr_length;
		bcopy(&dl->bca, mp->b_wptr, dl->prot.dl_brdcst_addr_length);
		mp->b_wptr += dl->prot.dl_brdcst_addr_length;
		freemsg(msg);
		mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_INFO_ACK");
		putnext(dl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_bind_ack: - issue DL_BIND_ACK primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_bind_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_bind_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_BIND_ACK;
		p->dl_sap =;
		p->dl_addr_length = dl->proto.dl_addr_length;
		p->dl_addr_offset = sizeof(*p);
		p->dl_max_conind = dl->conind;
		p->dl_xidtest_flg =;
		mp->b_wptr += sizeof(*p);
		bcopy(dl->add, mp->b_wptr, dl->proto.dl_addr_length);
		mp->b_wptr += dl->proto.dl_addr_length;
		freemsg(msg);
		mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_BIND_ACK");
		putnext(dl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_ok_ack: - issue DL_OK_ACK primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static inline fastcall int
dl_ok_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim)
{
	dl_ok_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_OK_ACK;
		p->dl_correct_primitive = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_OK_ACK");
		putnext(dl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_error_ack: - issue DL_ERROR_ACK primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @error: error value
 */
static inline fastcall int
dl_error_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, dl_long error)
{
	dl_error_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_ERROR_ACK;
		p->dl_error_primitive = prim;
		p->dl_errno = error < 0 ? DL_SYSERR : error;
		p->dl_unix_errno = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_ERROR_ACK");
		putnext(dl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_subs_bind_ack: - issue DL_SUBS_BIND_ACK primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_subs_bind_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_subs_bind_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_SUBS_BIND_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_SUBS_BIND_ACK");
		putnext(dl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_unitdata_ind: - issue DL_UNITDATA_IND primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_unitdata_ind(struct dl *dl, queue_t *q, mblk_t *msg,
		caddr_t dst_ptr, size_t dst_len, caddr_t src_ptr, size_t src_len,
		dl_ulong group, mblk_t *dp)
{
	dl_unitdata_ind_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p) + dst_len + src_len;

	if (likely(canputnext(dl->rq))) {
		if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_UNITDATA_IND;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = sizeof(*p);
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = sizeof(*p) + dst_len;
			p->dl_group_address = group;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_UNITDATA_IND");
			putnext(dl->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_uderror_ind: - issue DL_UDERROR_IND primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_uderror_ind(struct dl *dl, queue_t *q, mblk_t *msg,
	       caddr_t dst_ptr, size_t dst_len, dl_long error, mblk_t *dp)
{
	dl_uderror_ind_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p) + dst_len;

	if (likely(bcanputnext(dl->rq, 1))) {
		if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PCPROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_UDERROR_IND;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = sizeof(*p);
			p->dl_unix_errno = error < 0 ? -error : 0;
			p->dl_errno = error < 0 ? DL_SYSERR : error;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_UDERROR_IND");
			putnext(dl->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_connect_ind: - issue DL_CONNECT_IND primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_connect_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong correlation,
	       caddr_t cld_ptr, size_t cld_len,
	       caddr_t clg_ptr, size_t clg_len, caddr_t qos_ptr, size_t qos_len, mblk_t *dp)
{
	dl_connect_ind_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p) + cld_len + clg_len + qos_len;

	if (likely(canputnext(dl->rq))) {
		if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_CONNECT_IND;
			p->dl_correlation = correlation;
			p->dl_called_addr_length = cld_len;
			p->dl_called_addr_offset = sizeof(*p);
			p->dl_calling_addr_length = clg_len;
			p->dl_calling_addr_offset = sizeof(*p) + cld_len;
			p->dl_qos_length = qos_len;
			p->dl_qos_offset = sizeof(*p) + cld_len + clg_len;
			p->dl_growth = 0;
			mp->b_wptr += sizeof(*p);
			bcopy(cld_ptr, mp->b_wptr, cld_len);
			mp->b_wptr += cld_len;
			bcopy(clg_ptr, mp->b_wptr, clg_len);
			mp->b_wptr += clg_len;
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_CONNECT_IND");
			putnext(dl->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_connect_con: - issue DL_CONNECT_CON primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_connect_con(struct dl *dl, queue_t *q, mblk_t *msg,
	       caddr_t rsp_ptr, size_t rsp_len, caddr_t qos_ptr, size_t qos_len, mblk_t *dp)
{
	dl_connect_con_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p) + rsp_len + qos_len;

	if (likely(canputnext(dl->rq))) {
		if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_CONNECT_CON;
			p->dl_resp_addr_length = rsp_len;
			p->dl_resp_addr_offset = sizeof(*p);
			p->dl_qos_length = qos_len;
			p->dl_qos_offset = sizeof(*p) + rsp_len;
			p->dl_growth = 0;
			mp->b_wptr += sizeof(*p);
			bcopy(rsp_ptr, mp->b_wptr, rsp_len);
			mp->b_wptr += rsp_len;
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_CONNECT_CON");
			putnext(dl->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_token_ack: - issue DL_TOKEN_ACK primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_token_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong token)
{
	dl_token_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_TOKEN_ACK;
		p->dl_token = token;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_TOKEN_ACK");
		putnext(dl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_disconnect_ind: - issue DL_DISCONNECT_IND primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_disconnect_ind(struct dl *dl, queue_t *q, mblk_t *msg,
		  dl_ulong orig, dl_ulong reason, dl_ulong correlation)
{
	dl_disconnect_ind_t *p;
	mblk_t *mp;

	if (likely(canputnext(dl->rq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_DISCONNECT_IND;
			p->dl_originator = orig;
			p->dl_reason = reason;
			p->dl_correlation = correlation;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_DISCONNECT_IND");
			putnext(dl->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_reset_ind: - issue DL_RESET_IND primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_reset_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong orig, dl_ulong reason)
{
	dl_reset_ind_t *p;
	mblk_t *mp;

	if (likely(canputnext(dl->rq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_RESET_IND;
			p->dl_originator = orig;
			p->dl_reason = reason;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_RESET_IND");
			putnext(dl->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_reset_con: - issue DL_RESET_CON primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_reset_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reset_con_t *p;
	mblk_t *mp;

	if (likely(canputnext(dl->rq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_RESET_CON;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_RESET_CON");
			putnext(dl->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_data_ack_ind: - issue DL_DATA_ACK_IND primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_data_ack_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong priority, dl_ulong class,
		caddr_t dst_ptr, size_t dst_len, caddr_t src_ptr, size_t src_len, mblk_t *dp)
{
	dl_data_ack_ind_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p) + dst_len + src_len;

	if (likely(canputnext(dl->rq))) {
		if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_DATA_ACK_IND;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = sizeof(*p);
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = sizeof(*p) + dst_len;
			p->dl_priority = priority;
			p->dl_service_class = class;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont == NULL;
			freemsg(msg);
			mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_DATA_ACK_IND");
			putnext(dl->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_data_ack_status: - issue DL_DATA_ACK_STATUS_IND primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_data_ack_status_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong correlation,
		       dl_ulong status)
{
	dl_data_ack_status_ind_t *p;
	mblk_t *mp;

	if (likely(canputnext(dl->rq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_DATA_ACK_STATUS_IND;
			p->dl_correlation = correlation;
			p->dl_status = status;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_DATA_ACK_STATUS_IND");
			putnext(dl->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_reply_ind: - issue DL_REPLY_IND primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_reply_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong priority, dl_ulong class,
	     caddr_t dst_ptr, size_t dst_len, caddr_t src_ptr, size_t src_len, mblk_t *dp)
{
	dl_reply_ind_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p) + dst_len + src_len;

	if (likely(canputnext(dl->rq))) {
		if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_REPLY_IND;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = sizeof(*p);
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = sizeof(*p) + dst_len;
			p->dl_priority = priority;
			p->dl_service_class = class;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_REPLY_IND");
			putnext(dl->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_reply_status_ind: - issue DL_REPLY_STATUS_IND primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_reply_status_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong correlation, dl_ulong status,
		    mblk_t *dp)
{
	dl_reply_status_ind_t *p;
	mblk_t *mp;

	if (likely(canputnext(dl->rq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_REPLY_STATUS_IND;
			p->dl_correlation = correlation;
			p->dl_status = status;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_REPLY_STATUS_IND");
			putnext(dl->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_reply_update_status_ind: - issue DL_REPLY_UPDATE_STATUS_IND primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_reply_update_status_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong correlation,
			   dl_ulong status)
{
	dl_reply_update_status_ind_t *p;
	mblk_t *mp;

	if (likely(canputnext(dl->rq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_REPLY_UPDATE_STATUS_IND;
			p->dl_correlation = correlation;
			p->dl_status = status;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_REPLY_UPDATE_STATUS_IND");
			putnext(dl->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_xid_ind: - issue DL_XID_IND primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_xid_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong flag,
	   caddr_t dst_ptr, size_t dst_len, caddr_t src_ptr, size_t src_len, mblk_t *dp)
{
	dl_xid_ind_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p) + dst_len + src_len;

	if (likely(canpunext(dl->rq))) {
		if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_XID_IND;
			p->dl_flag = flag;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = sizeof(*p);
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = sizeof(*p) + dst_len;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_XID_IND");
			putnext(dl->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_xid_con: - issue DL_XID_CON primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_xid_con(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong flag,
	   caddr_t dst_ptr, size_t dst_len, caddr_t src_ptr, size_t src_len, mblk_t *dp)
{
	dl_xid_con_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p) + dst_len + src_len;

	if (likely(canputnext(dl->rq))) {
		if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_XID_CON;
			p->dl_flag = flag;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = sizeof(*p);
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = sizeof(*p) + dst_len;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_XID_CON");
			putnext(dl->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_test_ind: - issue DL_TEST_IND primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_test_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong flag,
	    caddr_t dst_ptr, size_t dst_len, caddr_t src_ptr, size_t src_len, mblk_t *dp)
{
	dl_test_ind_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p) + dst_len + src_len;

	if (likely(canputnext(dl->rq))) {
		if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_TEST_IND;
			p->dl_flag = flag;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = sizeof(*p);
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = sizeof(*p) + dst_len;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_TEST_IND");
			putnext(dl->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_test_con: - issue DL_TEST_CON primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_test_con(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong flag,
	    caddr_t dst_ptr, size_t dst_len, caddr_t src_ptr, size_t src_len, mblk_t *dp)
{
	dl_test_con_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p) + dst_len + src_len;

	if (likely(canputnext(dl->rq))) {
		if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_TEST_CON;
			p->dl_flag = flag;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = sizeof(*p);
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = sizeof(*p) + dst_len;
			mp->b_wptr += sizeof(*p);
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_TEST_CON");
			putnext(dl->rq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * dl_phys_addr_ack: - issue DL_PHYS_ADDR_ACK primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_phys_addr_ack(struct dl *dl, queue_t *q, mblk_t *msg, caddr_t add_ptr, size_t add_len)
{
	dl_phys_addr_ack_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p) + add_len;

	if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_PHYS_ADDR_ACK;
		p->dl_addr_length = add_len;
		p->dl_addr_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		freemsg(msg);
		mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_PHYS_ADDR_ACK");
		putnext(dl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_get_statistics_ack: - issue DL_GET_STATISTICS_ACK primitive
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
dl_get_statistics_ack(struct dl *dl, queue_t *q, mblk_t *msg, caddr_t sta_ptr, size_t sta_len)
{
	dl_get_statistics_ack_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p) + sta_len;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_GET_STATISTICS_ACK;
		p->dl_stat_length = sta_len;
		p->dl_stat_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		bcopy(sta_ptr, mp->b_wptr, sta_len);
		mp->b_wptr += sta_len;
		freemsg(msg);
		mi_strlog(dl->rq, STRLOGTX, SL_TRACE, "<- DL_GET_STATISTICS_ACK");
		putnext(dl->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_data_ind: - issue DL_DATA_IND primitive (M_DATA)
 * @dl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data
 */
static inline fastcall int
dl_data_ind(struct dl *dl, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	if (likely(canputnext(dl->rq))) {
		dp->b_band = 0;
		freemsg(msg);
#ifndef _OPTIMIZE_SPEED
		mi_strlog(dl->rq, STRLOGDA, SL_TRACE, "<- DL_DATA_IND");
#endif				/* _OPTIMIZE_SPEED */
		putnext(dl->rq, dp);
		return (0);
	}
	return (-EBUSY);
}

/*
 *  =========================================================================
 *
 *  PROTOCOL STATE MACHINE
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  RECEIVED PRIMITIVES
 *
 *  DL User -> DL Provider Primitives
 *
 *  =========================================================================
 */

/**
 * dl_info_req: - process DL_INFO_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_INFO_REQ primitive
 */
static noinline fastcall int
dl_info_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_info_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	return dl_info_ack(dl, q, mp);
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_INFO_REQ, err);
}

/**
 * dl_attach_req: - process DL_ATTACH_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_ATTACH_REQ primitive
 *
 * This driver is somewhat different than the old ldl driver (which still exists).  This driver
 * codes the Ethertype or MPOxx (NPID) code in the upper 16 bits of the PPA and the (Linux)
 * interface identifier in the lower 16 bits.  When the interface identifier is non-zero, it
 * specifies a particular Linux device.  When the interface identifier is zero, the attachment will
 * be to all devices.
 *
 * For receiving packets, we do not care whether they are native, or whether they contain a snap
 * header, or whether they contain an LLC1 header.  For transmitting packets, however, we need to
 * know whether to include a SNAP header and, for DL_CLDLS, whether to include an LLC1 header (an
 * LLC2, X25 LAPB, Frame Relay LAPF, ISDN LAPD, header is always included for DL_CODLS).  It is
 * independent whether we include a SNAP header and whether we include an LLC1 header: that is,
 * their may be no header, a SNAP header, and LLC1 header, or a SNAP and an LLC1 header.  When the S
 * bit is set in the PPA, it indicates that a SNAP header must be included on output, when the L bit
 * is set, it indicates that an LLC1 header must be included on DL_CLDLS output.
 *
 * physical point of attachment
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |    EtherType or MPOxx Code    |S:L:        Interface Id       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
static noinline fastcall int
dl_attach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_attach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_UNATTACHED)
		goto outstate;
	if (dl->prot.dl_provider_style == DL_STYLE1)
		goto notsupp;
	switch ((dl->framing = (p->dl_ppa & DL_FRAME_MASK) >> DL_FRAME_SHIFT)) {
	case DL_FRAME_ETHER:
	case DL_FRAME_802_2:
	case DL_FRAME_SNAP:
	case DL_FRAME_802_3:
	case DL_FRAME_RAW_LLC:
	case LD_FRAME_LLC2:
		break;
	default:
		goto badppa;
	}
	dl->ppa = (p->dl_ppa & DL_PPA_MASK) >> DL_PPA_SHIFT;
	/* FIXME: find the device from the ppa */
	switch (dev->type) {
	case ARPHRD_ETHER:
	case ARPHRD_LOOPBACK:
		switch (dl->framing) {
		case DL_FRAME_ETHER:
			break;
		case DL_FRAME_802_2:
			break;
		case DL_FRAME_SNAP:
			break;
		case DL_FRAME_802_3:
			break;
		case DL_FRAME_RAW_LLC:
			break;
		case DL_FRAME_LLC2:
			break;
		default:
			goto badppa;
		}
		break;
	case ARPHRD_FDDI:
		switch (dl->framing) {
		case DL_FRAME_802_2:
			break;
		case DL_FRAME_SNAP:
			break;
		case DL_FRAME_RAW_LLC:
			break;
		case DL_FRAME_LLC2:
			break;
		default:
			goto badppa;
		}
		break;
	case ARPHRD_IEEE802:
	case ARPHRD_IEEE802_TR:
		switch (dl->framing) {
		case DL_FRAME_802_2:
			break;
		case DL_FRAME_SNAP:
			break;
		case DL_FRAME_RAW_LLC:
			break;
		case DL_FRAME_LLC2:
			break;
		default:
			goto badppa;
		}
		break;
	case ARPHRD_HDLC:
		switch (dl->framing) {
		case DL_FRAME_RAW_LLC:
			break;
		case DL_FRAME_LLC2:
			break;
		default:
			goto badppa;
		}
		break;
	default:
		gtoo badppa;
	}
	switch (dev->type) {
		/* ETH_P_SNAP */
	case ARPHRD_IEEE802:
		/* ETH_P_8021Q */
		/* ETH_P_802_3 */
		dl->prot.dl_mac_type = DL_CSMACD;	/* IEEE 802.3 CSMA/CD network */
		dl->prot.dl_mac_type = DL_TPB;	/* IEEE 802.4 Token Passing Bus */
		dl->prot.dl_mac_type = DL_ETH_CSMA;	/* ISO 8802/3 and Ethernet */
		break;
	case ARPHRD_IEEE802_TR:
		/* ETH_P_TR_802_2 */
		dl->prot.dl_mac_type = DL_100VGTPR;	/* 100 Based VG Token Ring */
		dl->prot.dl_mac_type = DL_TPR;	/* IEEE 802.5 Token Passing Ring */
		break;
	case ARPHRD_ETHER:
		dl->prot.dl_mac_type = DL_100VG;	/* 100 Based VG Ethernet */
		dl->prot.dl_mac_type = DL_100BT;	/* 100 Base T */
		dl->prot.dl_mac_type = DL_ETHER;	/* Ethernet Bus */
		break;
	case ARPHRD_HDLC:
	case ARPHRD_RAWHDLC:
		/* ETH_P_HDLC */
		dl->prot.dl_mac_type = DL_HDLC;	/* ISO HDLC protocol support */
		break;
	case ARPHRD_FDDI:
		dl->prot.dl_mac_type = DL_FDDI;	/* Fiber Distributed data interface */
		break;
	case ARPHRD_FCPP:
	case ARPHRD_FCAL:
	case ARPHRD_FCPL:
	case ARPHRD_FCFABRIC:
		dl->prot.dl_mac_type = DL_FC;	/* Fibre Channel interface */
		break;
	case ARPHRD_ATM:
		/* ETH_P_ATMMPOA */
		dl->prot.dl_mac_type = DL_IPATM;	/* ATM Classical IP interface */
		/* ETH_P_ATMFATE */
		dl->prot.dl_mac_type = DL_ATM;	/* ATM */
		break;
	case ARPHRD_LAPB:
	case ARPHRD_AX25:
		/* ETH_P_X25 */
		/* ETH_P_AX25 */
		dl->prot.dl_mac_type = DL_X25;	/* X.25 LAPB interface */
		break;
	case ARPHRD_HIPPI:
		dl->prot.dl_mac_type = DL_HIPPI;	/* HIPPI interface */
		break;
	case ARPHRD_DLCI:
		dl->prot.dl_mac_type = DL_FRAME;	/* Frame Relay LAPF */
		break;
	case ARPHRD_FRAD:
		dl->prot.dl_mac_type = DL_MPFRAME;	/* Multi-protocol over Frame Relay */
		break;
	case ARPHRD_X25:
		dl->prot.dl_mac_type = DL_IPX25;	/* X.25 Classical IP interface */
		break;
	case ARPHRD_LOOPBACK:
		/* ETH_P_LOOP */
		dl->prot.dl_mac_type = DL_LOOP;	/* software loopback */
		break;
	default:
		dl->prot.dl_mac_type = DL_ISDN;	/* ISDN interface */
		dl->prot.dl_mac_type = DL_METRO;	/* IEEE 802.6 Metro Net */
		dl->prot.dl_mac_type = DL_CHAR;	/* Character Synchronous protocol support */
		dl->prot.dl_mac_type = DL_ASYNC;	/* Character Asynchronous Protocol */
		dl->prot.dl_mac_type = DL_CTCA;	/* IBM Channel-to-Channel Adapter */
		dl->prot.dl_mac_type = DL_OTHER;	/* Any other medium not listed above */
		break;
	}
	dl_set_i_state(dl, DL_ATTACH_PENDING);
	return dl_ok_ack(dl, q, mp, DL_ATTACH_REQ);
      badppa:
	err = DL_BADPPA;
	goto error;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_ATTACH_REQ, err);
}

/**
 * dl_detach_req: - process DL_DETACH_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_DETACH_REQ primitive
 */
static noinline fastcall int
dl_detach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_detach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_UNBOUND)
		goto outstate;
	if (dl->prot.dl_provider_style == DL_STYLE1)
		goto notsupp;
	dl_set_i_state(dl, DL_DETACH_PENDING);
	if ((err = dl_ok_ack(dl, q, mp, DL_DETACH_REQ)))
		return (err);
	ndev_release(dl);
	return (0);
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      oustate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_DETACH_REQ, err);
}

/**
 * dl_bind_req: - process DL_BIND_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_BIND_REQ primitive
 */
static noinline fastcall int
dl_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_UNBOUND)
		goto outstate;
	switch (p->dl_service_mode) {
	case DL_CLDLS:
	case DL_CODLS:
	case DL_ACLDLS:
		break;
	default:
		goto notsupp;
	}
	if (p->dl_xidtest_flg)
		goto noauto;
	dl_set_i_state(dl, DL_BIND_PENDING);
	goto notsupp;
      noauto:
	err = DL_NOAUTO;
	goto error;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      oustate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_BIND_REQ, err);
}

/**
 * dl_unbind_req: - process DL_UNBIND_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_UNBIND_REQ primitive
 */
static noinline fastcall int
dl_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_i_state(dl, DL_UNBIND_PENDING);
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_UNBIND_REQ, err);
}

/**
 * dl_subs_bind_req: - process DL_SUBS_BIND_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_SUBS_BIND_REQ primitive
 */
static noinline fastcall int
dl_subs_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_i_state(dl, DL_SUBS_BIND_PND);
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_SUBS_BIND_REQ, err);
}

/**
 * dl_subs_unbind_req: - process DL_SUBS_UNBIND_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_SUBS_UNBIND_REQ primitive
 */
static noinline fastcall int
dl_subs_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_i_state(dl, DL_SUBS_UNBIND_PND);
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_SUBS_UNBIND_REQ, err);
}

/**
 * dl_enabmulti_req: - process DL_ENABMULTI_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_ENABMULTI_REQ primitive
 */
static noinline fastcall int
dl_enabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_enabmulti_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) == DL_UNATTACHED || dl_get_i_state(dl) == DL_ATTACH_PENDING ||
	    dl_get_i_state(dl) == DL_DETACH_PENDING)
		goto outstate;
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_ENABMULTI_REQ, err);
}

/**
 * dl_disabmulti_req: - process DL_DISABMULTI_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_DISABMULTI_REQ primitive
 */
static noinline fastcall int
dl_disabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_disabmulti_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) == DL_UNATTACHED || dl_get_i_state(dl) == DL_ATTACH_PENDING ||
	    dl_get_i_state(dl) == DL_DETACH_PENDING)
		goto outstate;
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_DISABMULTI_REQ, err);
}

/**
 * dl_promiscon_req: - process DL_PROMISCON_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_PROMISCON_REQ primitive
 */
static noinline fastcall int
dl_promiscon_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_promiscon_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) == DL_UNATTACHED || dl_get_i_state(dl) == DL_ATTACH_PENDING ||
	    dl_get_i_state(dl) == DL_DETACH_PENDING)
		goto outstate;
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_PROMISCON_REQ, err);
}

/**
 * dl_promiscoff_req: - process DL_PROMISCOFF_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_PROMISCOFF_REQ primitive
 */
static noinline fastcall int
dl_promiscoff_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_promiscoff_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) == DL_UNATTACHED || dl_get_i_state(dl) == DL_ATTACH_PENDING ||
	    dl_get_i_state(dl) == DL_DETACH_PENDING)
		goto outstate;
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_PROMISCOFF_REQ, err);
}

/**
 * dl_unitdata_req: - process DL_UNITDATA_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_UNITDATA_REQ primitive
 */
static noinline fastcall int
dl_unitdata_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_IDLE)
		goto outstate;
	if (dl->prot.dl_service_mode != DL_CLDLS)
		goto notsupp;
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_UNITDATA_REQ, err);
}

/**
 * dl_udqos_req: - process DL_UDQOS_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_UDQOS_REQ primitive
 */
static noinline fastcall int
dl_udqos_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_udqos_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_IDLE)
		goto outstate;
	if (dl->prot.dl_service_mode != DL_CLDLS)
		goto notsupp;
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_UDQOS_REQ, err);
}

/**
 * dl_connect_req: - process DL_CONNECT_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_CONNECT_REQ primitive
 */
static noinline fastcall int
dl_connect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_IDLE)
		goto outstate;
	if (dl->prot.dl_service_mode != DL_CODLS)
		goto notsupp;
	dl_set_i_state(dl, DL_OUTCON_PENDING);
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_CONNECT_REQ, err);
}

/**
 * dl_connect_res: - process DL_CONNECT_RES primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_CONNECT_RES primitive
 */
static noinline fastcall int
dl_connect_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_INCON_PENDING)
		goto outstate;
	if (dl->prot.dl_service_mode != DL_CODLS)
		goto notsupp;
	dl_set_i_state(dl, DL_CONN_RES_PENDING);
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_CONNECT_RES, err);
}

/**
 * dl_token_req: - process DL_TOKEN_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_TOKEN_REQ primitive
 */
static noinline fastcall int
dl_token_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_token_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	return dl_token_ack(dl, q, mp, (dl_ulong) (long) dl->rq);
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_TOKEN_REQ, err);
}

/**
 * dl_disconnect_req: - process DL_DISCONNECT_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_DISCONNECT_REQ primitive
 */
static noinline fastcall int
dl_disconnect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_disconnect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl->prov.dl_service_mode != DL_CODLS)
		goto notsupp;
	switch (dl_get_i_state(dl)) {
	case DL_OUTCON_PENDING:
		dl_set_i_state(dl, DL_DISCON8_PENDING);
		break;
	case DL_INCON_PENDING:
		dl_set_i_state(dl, DL_DISCON9_PENDING);
		break;
	case DL_DATAXFER:
		dl_set_i_state(dl, DL_DISCON11_PENDING);
		break;
	case DL_USER_RESET_PENDING:
		dl_set_i_state(dl, DL_DISCON12_PENDING);
		break;
	case DL_PROV_RESET_PENDING:
		dl_set_i_state(dl, DL_DISCON13_PENDING);
		break;
	default:
		goto oustate;
	}
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_DISCONNECT_REQ, err);
}

/**
 * dl_reset_req: - process DL_RESET_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_RESET_REQ primitive
 */
static noinline fastcall int
dl_reset_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reset_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_DATAXFER)
		goto outstate;
	if (dl->prot.dl_service_mode != DL_CODLS)
		goto notsupp;
	dl_set_i_state(dl, DL_USER_RESET_PENDING);
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_RESET_REQ, err);
}

/**
 * dl_reset_res: - process DL_RESET_RES primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_RESET_RES primitive
 */
static noinline fastcall int
dl_reset_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reset_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_PROV_RESET_PENDING)
		goto outstate;
	if (dl->prot.dl_service_mode != DL_CODLS)
		goto notsupp;
	dl_set_i_state(dl, DL_RESET_RES_PENDING);
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_RESET_RES, err);
}

/**
 * dl_data_ack_req: - process DL_DATA_ACK_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_DATA_ACK_REQ primitive
 */
static noinline fastcall int
dl_data_ack_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_data_ack_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_IDLE)
		goto outstate;
	if (dl->prot.dl_service_mode != DL_ACLDLS)
		goto notsupp;
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_DATA_ACK_REQ, err);
}

/**
 * dl_reply_req: - process DL_REPLY_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_REPLY_REQ primitive
 */
static noinline fastcall int
dl_reply_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reply_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_IDLE)
		goto outstate;
	if (dl->prot.dl_service_mode != DL_ACLDLS)
		goto notsupp;
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_REPLY_REQ, err);
}

/**
 * dl_reply_update_req: - process DL_REPLY_UPDATE_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_REPLY_UPDATE_REQ primitive
 */
static noinline fastcall int
dl_reply_update_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reply_update_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_IDLE)
		goto outstate;
	if (dl->prot.dl_service_mode != DL_ACLDLS)
		goto notsupp;
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_REPLY_UPDATE_REQ, err);
}

/**
 * dl_xid_req: - process DL_XID_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_XID_REQ primitive
 */
static noinline fastcall int
dl_xid_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_xid_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_IDLE && dl_get_i_state(dl) != DL_DATAXFER)
		goto outstate;
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_XID_REQ, err);
}

/**
 * dl_xid_res: - process DL_XID_RES primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_XID_RES primitive
 */
static noinline fastcall int
dl_xid_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_xid_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_IDLE && dl_get_i_state(dl) != DL_DATAXFER)
		goto outstate;
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_XID_RES, err);
}

/**
 * dl_test_req: - process DL_TEST_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_TEST_REQ primitive
 */
static noinline fastcall int
dl_test_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_test_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_IDLE && dl_get_i_state(dl) != DL_DATAXFER)
		goto outstate;
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_TEST_REQ, err);
}

/**
 * dl_test_res: - process DL_TEST_RES primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_TEST_RES primitive
 */
static noinline fastcall int
dl_test_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_test_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) != DL_IDLE && dl_get_i_state(dl) != DL_DATAXFER)
		goto outstate;
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_TEST_RES, err);
}

/**
 * dl_phys_addr_req: - process DL_PHYS_ADDR_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_PHYS_ADDR_REQ primitive
 */
static noinline fastcall int
dl_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_phys_addr_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) == DL_UNATTACHED || dl_get_i_state(dl) == DL_ATTACH_PENDING
	    || dl_get_i_state(dl) == DL_DETACH_PENDING)
		goto outstate;
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_PHYS_ADDR_REQ, err);
}

/**
 * dl_set_phys_addr_req: - process DL_SET_PHYS_ADDR_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_SET_PHYS_ADDR_REQ primitive
 */
static noinline fastcall int
dl_set_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_set_phys_addr_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (dl_get_i_state(dl) == DL_UNATTACHED || dl_get_i_state(dl) == DL_ATTACH_PENDING
	    || dl_get_i_state(dl) == DL_DETACH_PENDING)
		goto outstate;
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_SET_PHYS_ADDR_REQ, err);
}

/**
 * dl_get_statistics_req: - process DL_GET_STATISTICS_REQ primitive
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_GET_STATISTICS_REQ primitive
 */
static noinline fastcall int
dl_get_statistics_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_get_statistics_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	goto notsupp;
      notsupp:
	err = DL_NOTSUPPORTED;
	goto error;
      badprim:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_ack(dl, q, mp, DL_GET_STATISTICS_REQ, err);
}

/**
 * dl_data_req: - process DL_DATA_REQ primitive (M_DATA)
 * @dl: private structure
 * @q: active queue (write queue)
 * @mp: the DL_DATA_REQ primitive (M_DATA)
 */
static inline fastcall int
dl_data_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	size_t msg_size = msgdsize(mp);
	int err;

	if (msg_size < dl->prot.dl_min_sdu)
		goto eproto;
	if (msg_size > dl->prot.dl_max_sdu)
		goto eproto;
	switch (dl_get_i_state(dl)) {
	case DL_DATAXFER:
		break;
	case DL_IDLE:
	case DL_PROV_RESET_PENDING:
		goto discard;
	default:
		goto eproto;
	}
	goto discard;
      eproto:
	if ((err = m_error(dl, q, mp, EPROTO)))
		return (err);
	dl->state.i_flags |= DLF_ERROR;
	return (0);
      discard:
	freemsg(mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS MESSAGE HANDLING
 *
 *  =========================================================================
 */

static inline fastcall int
__dl_w_data(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_data_req(dl, q, mp);
}
static inline fastcall int
dl_w_data(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int rtn = -EAGAIN;

	if (likely(!!(priv = mi_trylock(q)))) {
		rtn = __dl_w_data(DL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (rtn);
}
static inline fastcall int
__dl_w_proto_slow(struct dl *dl, queue_t *q, mblk_t *mp, dl_ulong prim)
{
	int rtn;

	switch (prim) {
	case DL_UNITDATA_REQ:
	case DL_UDQOS_REQ:
	case DL_DATA_ACK_REQ:
	case DL_REPLY_REQ:
	case DL_REPLY_UPDATE_REQ:
#ifndef _OPTIMIZE_SPEED
		mi_strlog(q, STRLOGDA, SL_TRACE, "-> %s", dl_primname(prim));
		break;
#endif				/* _OPTIMIZE_SPEED */
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> %s", dl_primname(prim));
		break;
	}

	dl_save_i_state(dl);

	switch (prim) {
	case DL_INFO_REQ:
		rtn = dl_info_req(dl, q, mp);
		break;
	case DL_ATTACH_REQ:
		rtn = dl_attach_req(dl, q, mp);
		break;
	case DL_DETACH_REQ:
		rtn = dl_detach_req(dl, q, mp);
		break;
	case DL_BIND_REQ:
		rtn = dl_bind_req(dl, q, mp);
		break;
	case DL_UNBIND_REQ:
		rtn = dl_unbind_req(dl, q, mp);
		break;
	case DL_SUBS_BIND_REQ:
		rtn = dl_subs_bind_req(dl, q, mp);
		break;
	case DL_SUBS_UNBIND_REQ:
		rtn = dl_subs_unbind_req(dl, q, mp);
		break;
	case DL_ENABMULTI_REQ:
		rtn = dl_enabmulti_req(dl, q, mp);
		break;
	case DL_DISABMULTI_REQ:
		rtn = dl_disabmulti_req(dl, q, mp);
		break;
	case DL_PROMISCON_REQ:
		rtn = dl_promiscon_req(dl, q, mp);
		break;
	case DL_PROMISCOFF_REQ:
		rtn = dl_promiscoff_req(dl, q, mp);
		break;
	case DL_UNITDATA_REQ:
		rtn = dl_unitdata_req(dl, q, mp);
		break;
	case DL_UDQOS_REQ:
		rtn = dl_udqos_req(dl, q, mp);
		break;
	case DL_CONNECT_REQ:
		rtn = dl_connect_req(dl, q, mp);
		break;
	case DL_CONNECT_RES:
		rtn = dl_connect_res(dl, q, mp);
		break;
	case DL_TOKEN_REQ:
		rtn = dl_token_req(dl, q, mp);
		break;
	case DL_DISCONNECT_REQ:
		rtn = dl_disconnect_req(dl, q, mp);
		break;
	case DL_RESET_REQ:
		rtn = dl_reset_req(dl, q, mp);
		break;
	case DL_RESET_RES:
		rtn = dl_reset_res(dl, q, mp);
		break;
	case DL_DATA_ACK_REQ:
		rtn = dl_data_ack_req(dl, q, mp);
		break;
	case DL_REPLY_REQ:
		rtn = dl_reply_req(dl, q, mp);
		break;
	case DL_REPLY_UPDATE_REQ:
		rtn = dl_reply_update_req(dl, q, mp);
		break;
	case DL_XID_REQ:
		rtn = dl_xid_req(dl, q, mp);
		break;
	case DL_XID_RES:
		rtn = dl_xid_res(dl, q, mp);
		break;
	case DL_TEST_REQ:
		rtn = dl_test_req(dl, q, mp);
		break;
	case DL_TEST_RES:
		rtn = dl_test_res(dl, q, mp);
		break;
	case DL_PHYS_ADDR_REQ:
		rtn = dl_phys_addr_req(dl, q, mp);
		break;
	case DL_SET_PHYS_ADDR_REQ:
		rtn = dl_set_phys_addr_req(dl, q, mp);
		break;
	case DL_GET_STATISTICS_REQ:
		rtn = dl_get_statistics_req(dl, q, mp);
		break;
	default:
		rtn = dl_other_req(dl, q, mp);
		break;
	}
	if (rtn)
		dl_restore_i_state(dl);
	return (rtn);
}
static inline fastcall int
__dl_w_proto(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_ulong prim;
	int rtn;

	if (likely(MBLKIN(mp, 0, sizeof(prim)))) {
		prim = *(typeof(prim) *) mp->b_rptr;
		if (prim = DL_UNITDATA_REQ) {
#ifndef _OPTIMIZE_SPEED
			mi_strlog(dl->wq, STRLOGRX, SL_TRACE, "-> DL_UNITDATA_REQ");
#endif				/* _OPTIMIZE_SPEED */
			rtn = dl_unitdata_req(dl, q, mp);
		} else {
			rtn = __dl_w_proto_slow(dl, q, mp, prim);
		}
	} else {
		mi_strlog(q, STRLOGNO, SL_ERROR | SL_TRACE, "%s: bad primitive (too short)",
			  __FUNCTION__);
		freemsg(mp);
		rtn = 0;
	}
	return (rtn);
}
static inline fastcall int
dl_w_proto(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int rtn = -EAGAIN;

	if (likely(!!(priv = mi_trylock(q)))) {
		rtn = __dl_w_proto(DL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (rtn);
}
static inline fastcall int
__dl_m_sig(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int rtn;

	if (unlikely(!mi_timer_valid(mp)))
		return (0);

	dl_save_i_flags(dl);

	switch (*(int *) mp->b_rptr) {
	case 1:
		rtn = dl_t1_timeout(dl, q, mp);
		break;
	case 2:
		rtn = dl_t2_timeout(dl, q, mp);
		break;
	case 3:
		rtn = dl_t3_timeout(dl, q, mp);
		break;
	case 4:
		rtn = dl_t4_timeout(dl, q, mp);
		break;
	case 5:
		rtn = dl_t5_timeout(dl, q, mp);
		break;
	case 6:
		rtn = dl_t6_timeout(dl, q, mp);
		break;
	default:
		mi_strlog(q, STRLOGNO, SL_ERROR | SL_TRACE, "%s: invalid timer %d",
			  __FUNCTION__, *(int *) mp->b_rptr);
		rtn = 0;
		break;
	}

	if (rtn) {
		rtn = mi_timer_requeue(mp) ? rtn : 0;
		dl_restore_flags(dl);
	}
	return (rtn);
}
static inline fastcall int
dl_m_sig(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int rtn = -EAGAIN;

	if (likely(!!(priv = mi_trylock(q)))) {
		rtn = __dl_m_sig(DL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (rtn);
}
static inline fastcall int
dl_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] & ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(RD(q), FLUSHDATA);
		putnext(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}
static inline fastcall int
__dl_w_ioctl(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static inline fastcall int
dl_w_ioctl(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int rtn = -EAGAIN;

	if (likely(!!(priv = mi_trylock(q)))) {
		rtn = __dl_w_ioctl(DL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (rtn);
}
static inline fastcall int
__dl_w_iocdata(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static inline fastcall int
dl_w_iocdata(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int rtn = -EAGAIN;

	if (likely(!!(priv = mi_trylock(q)))) {
		rtn = __dl_w_iocdata(DL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (rtn);
}
static inline fastcall int
dl_w_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGNO, SL_ERROR | SL_TRACE, "%s: unexpected STREAMS message %d",
		  __FUNCTION__, (int) DB_TYPE(mp));
	freemsg(mp);
	return (0);
}

static noinline fastcall int
__dl_w_msg_slow(struct dl *dl, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return __dl_w_data(dl, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return __dl_w_proto(dl, q, mp);
	case M_SIG:
	case M_PCSIG:
		return __dl_m_sig(dl, q, mp);
	case M_FLUSH:
		return dl_w_flush(q, mp);
	case M_IOCTL:
		return __dl_w_ioctl(dl, q, mp);
	case M_IOCDATA:
		return __dl_w_iocdata(dl, q, mp);
	default:
		return dl_w_other(q, mp);
	}
}
static noinline fastcall int
__dl_w_msg(struct dl *dl, queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) = M_DATA)
		return __dl_w_data(dl, q, mp);
	if (DB_TYPE(mp) = M_PROTO)
		return __dl_w_proto(dl, q, mp);
	return __dl_w_msg_slow(dl, q, mp);
}
static noinline fastcall int
dl_w_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return dl_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_w_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return dl_m_sig(q, mp);
	case M_FLUSH:
		return dl_w_flush(q, mp);
	case M_IOCTL:
		return dl_w_ioctl(q, mp);
	case M_IOCDATA:
		return dl_w_iocdata(q, mp);
	default:
		return dl_w_other(q, mp);
	}
}
static noinline fastcall int
dl_w_msg(queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) = M_DATA)
		return dl_w_data(q, mp);
	if (DB_TYPE(mp) = M_PROTO)
		return dl_w_proto(q, mp);
	return dl_w_msg_slow(q, mp);
}

static inline fastcall int
__dl_r_data(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_data_ind(dl, q, mp);
}
static inline fastcall int
dl_r_data(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int rtn = -EAGAIN;

	if (likely(!!(priv = mi_trylock(q)))) {
		rtn = __dl_r_data(DL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (rtn);
}
static inline fastcall int
dl_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}
static inline fastcall int
dl_r_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGNO, SL_ERROR | SL_TRACE,
		  "%s: unexpected STREAMS message on read queue, %d", __FUNCTION__,
		  (int) DB_TYPE(mp));
	freemsg(mp);
	return (0);
}

static noinline fastcall int
__dl_r_msg_slow(struct dl *dl, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_SIG:
	case M_PCSIG:
		return __dl_m_sig(dl, q, mp);
	case M_DATA:
		return __dl_r_data(dl, q, mp);
	case M_FLUSH:
		return dl_r_flush(q, mp);
	default:
		return dl_r_other(q, mp);
	}
}
static noinline fastcall int
__dl_r_msg(struct dl *dl, queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_DATA)
		return __dl_r_data(dl, q, mp);
	return __dl_r_msg_slow(dl, q, mp);
}
static noinline fastcall int
dl_r_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_SIG:
	case M_PCSIG:
		return dl_m_sig(q, mp);
	case M_DATA:
		return dl_r_data(q, mp);
	case M_FLUSH:
		return dl_r_flush(q, mp);
	default:
		return dl_r_other(q, mp);
	}
}
static noinline fastcall int
dl_r_msg(queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_DATA)
		return dl_r_data(q, mp);
	return dl_r_msg_slow(q, mp);
}

/*
 *  =========================================================================
 *
 *  STREAMS QUEUE PUT AND SERVICE PROCEDURES
 *
 *  =========================================================================
 */

static streamscall int __hot_write
dl_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || dl_w_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int __hot_out
dl_wsrv(queue_t *q)
{
	mblk_t *mp;

	if (likely(!!(mp = getq(q)))) {
		caddr_t priv;

		if (likely(!!(priv = mi_trylock(q)))) {
			do {
				if (unlikely(__dl_w_msg(DL_PRIV(q), q, mp)))
					break;
			} while (likely(!!(mp = getq(q))));
			mi_unlock(priv);
		}
		if (unlikely(!!mp))
			putbq(q, mp);
	}
	return (0);
}
static streamscall int __hot_read
dl_rsrv(queue_t *q)
{
	mblk_t *mp;

	if (likely(!!(mp = getq(q)))) {
		caddr_t priv;

		if (likely(!!(priv = mi_trylock(q)))) {
			do {
				if (unlikely(__dl_r_msg(DL_PRIV(q), q, mp)))
					break;
			} while (likely(!!(mp = getq(q))));
			mi_unlock(priv);
		}
		if (unlikely(!!mp))
			putbq(q, mp);
	}
	return (0);
}
static streamscall int __hot_in
dl_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || dl_r_msg(q, mp))
		putq(q, mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS OPEN AND CLOSE ROUTINES
 *
 *  =========================================================================
 */

static streamscall int
dl_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	unsigned long flags;
	struct dl *dl;
	int err;

	if (q->q_ptr)
		return (0);
	if (unlikely(!(dl = (struct dl *) mi_open_alloc_cache(dl_priv_cachep, GFP_KERNEL))))
		return (ENOMEM);
	write_lock_irqsave(&dl_drv_lock, flags);
	err = mi_open_link(&dl_opens, (caddr_t) dl, devp, oflags, sflag, crp);
	write_unlock_irqrestore(&dl_drv_lock, flags);
	if (err) {
		mi_close_free_cache(dl_priv_cachep, (caddr_t) dl);
		return (err);
	}
	mi_attach(q, (caddr_t) dl);
	bzero(dl, sizeof(*dl));

	/* intialize private structure */
	dl->rq = q;
	dl->wq = WR(q);
	dl->state.l_state = 0;
	dl->state.i_state = DL_UNATTACHED;
	dl->state.i_flags = 0;
	dl->oldstate.l_state = 0;
	dl->oldstate.i_state = DL_UNATTACHED;
	dl->oldstate.i_flags = 0;
	dl->prot.dl_primitive = DL_INFO_ACK;
	dl->prot.dl_min_sdu = 0;
	dl->prot.dl_max_sdu = 4096;
	dl->prot.dl_addr_length = 0;
	dl->prot.dl_mac_type = DL_OTHER;
	dl->prot.dl_reserved = 0;
	dl->prot.dl_current_state = DL_UNATTACHED;
	dl->prot.dl_sap_length = 0;
	dl->prot.dl_service_mode = DL_CLDLS | DL_CODLS | DL_ACLDLS;
	dl->prot.dl_qos_length = 0;
	dl->prot.dl_qos_offset = sizeof(dl->prot);
	dl->prot.dl_qos_range_length = 0;
	dl->prot.dl_qos_offset = sizeof(dl->prot);
	dl->prot.dl_provider_style = DL_STYLE2;
	dl->prot.dl_addr_offset = sizeof(dl->prot);
	dl->prot.dl_version = DL_CURRENT_VERSION;
	dl->prot.dl_brdcst_addr_length = 0;
	dl->prot.dl_brdcst_addr_offset = sizeof(dl->prot);
	dl->prot.dl_growth = 0;
	dl->qos.cl.dl_qos_type = DL_QOS_CL_SEL1;
	dl->qos.cl.dl_trans_delay = DL_UNKNOWN;
	dl->qos.cl.dl_priority = dl->priority;
	dl->qos.cl.dl_protection = DL_NONE;
	dl->qos.cl.dl_residual_error = DL_UNKNOWN;
	dl->qor.cl.dl_qos_type = DL_QOS_CL_RANGE1;
	dl->qor.cl.dl_trans_delay.dl_target_value = DL_UNKNOWN;
	dl->qor.cl.dl_trans_delay.dl_accept_value = DL_UNKNOWN;
	dl->qor.cl.dl_priority.dl_min = 100;
	dl->qor.cl.dl_priority.dl_max = 0;
	dl->qor.cl.dl_protection.dl_min = DL_NONE;
	dl->qor.cl.dl_protection.dl_max = DL_NONE;
	dl->qor.cl.dl_residual_error = DL_UNKNOWN;
	dl->qos.co.dl_qos_type = DL_QOS_CO_SEL1;
	dl->qos.co.dl_rcv_throughput = DL_UNKNOWN;
	dl->qos.co.dl_rcv_trans_delay = DL_UNKNOWN;
	dl->qos.co.dl_xmt_throughput = DL_UNKNOWN;
	dl->qos.co.dl_xmt_trans_delay = DL_UNKNOWN;
	dl->qos.co.dl_priority = dl->priority;
	dl->qos.co.dl_protection = DL_NONE;
	dl->qos.co.dl_residual_error = DL_UNKNOWN;
	dl->qor.co.dl_qos_type = DL_QOS_CO_RANGE1;
	dl->qor.co.dl_rcv_throughput.dl_target_value = DL_UNKNOWN;
	dl->qor.co.dl_rcv_throughput.dl_accept_value = DL_UNKNOWN;
	dl->qor.co.dl_rcv_trans_delay.dl_target_value = DL_UNKNOWN;
	dl->qor.co.dl_rcv_trans_delay.dl_accept_value = DL_UNKNOWN;
	dl->qor.co.dl_xmt_throughput.dl_target_value = DL_UNKNOWN;
	dl->qor.co.dl_xmt_throughput.dl_accept_value = DL_UNKNOWN;
	dl->qor.co.dl_xmt_trans_delay.dl_target_value = DL_UNKNOWN;
	dl->qor.co.dl_xmt_trans_delay.dl_accept_value = DL_UNKNOWN;
	dl->qor.co.dl_priority.dl_min = 100;
	dl->qor.co.dl_priority.dl_max = 0;
	dl->qor.co.dl_protection.dl_min = DL_NONE;
	dl->qor.co.dl_protection.dl_max = DL_NONE;
	dl->qor.co.dl_residual_error = DL_UNKNOWN;
	dl->qor.co.dl_resilience.dl_disc_prob = DL_UNKNOWN;
	dl->qor.co.dl_resilience.dl_reset_prob = DL_UNKNOWN;

	qprocson(q);
	return (0);
}

static streamscall int
dl_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct dl *dl = DL_PRIV(q);
	unsigned long flags;

	(void) dl;
	qprocsoff(q);
	write_lock_irqsave(&dl_drv_lock, flags);
	mi_close_comm_cache(&dl_opens, dl_priv_cachep, q);
	write_unlock_irqrestore(&dl_drv_lock, flags);
	return (0);
}

/*
 *  =========================================================================
 *
 *  DEVICE NOTIFICATION
 *
 *  =========================================================================
 */

/**
 * dl_notify - network device notifier
 * @notifier: out notifier block
 * @event: the notification event
 * @ptr: a pointer to our ndev structure
 */
static int
dl_notify(struct notifier_block *notifier, unsigned long event, void *ptr)
{
	struct net_device *ndev = (typeof(ndev)) ptr;
	caddr_t priv;
	struct dl *dl;

	read_lock(&dl_drv_lock);
	for (priv = mi_first_ptr(&dl_opens); priv && ((struct dl *) priv)->ndev != ndev;
	     priv = mi_next_ptr(&dl_opens, priv)) ;
	dl = (struct dl *) priv;
	if (event == NETDEV_DOWN || event == NETDEV_UNREGISTER) {
		if (dl->state.i_flags & DLF_UNHANGUP) {
			dl->state.i_flags |= DLF_HANGUP;
			if (!m_hangup(dp, NULL, NULL))
				dl->state.i_flags &= ~DLF_UNHANGUP;
		}
	}
	if (event == NETDEV_UP || event == NETDEV_REGISTER) {
		if (dl->state.i_flags & DLF_HANGUP) {
			dl->state.i_flags |= DLF_UNHANGUP;
			if (!m_unhangup(dp, NULL, NULL))
				dl->state.i_flags &= ~DLF_HANGUP;
		}
	}
	read_unlock(&dl_drv_lock);
	return NOTIFY_DONE;
}

static struct notifier_block dl_notifier = { dl_notify, NULL, 0 };

/*
 *  =========================================================================
 *
 *  REGISTRATION AND INITIALIZATION
 *
 *  =========================================================================
 */

static struct qinit dl_rinit = {
	.qi_putp = dl_rput,
	.qi_srvp = dl_rsrv,
	.qi_qopen = dl_qopen,
	.qi_qclose = dl_qclose,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_rstat,
};

static struct qinit dl_winit = {
	.qi_putp = dl_wput,
	.qi_srvp = dl_wsrv,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_wstat,
};

static struct streamtab llcinfo = {
	.st_rdinit = &dl_rinit,
	.st_wrinit = &dl_winit,
};

#ifdef LINUX
/*
 *  =========================================================================
 *
 *  LINUX INITIALIZATION
 *
 *  =========================================================================
 */

unsigned short modid = DRV_ID;

#ifdef module_param
module_param(modid, ushort, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for LLC driver.  (0 for allocation.)");

unsigned short major = LLC_CMAJOR_0;

#ifdef module_param
module_param(major, ushort, 0444);
#else				/* module_param */
MODULE_PARM(major, "h");
#endif				/* module_param */
MODULE_PARM_DESC(major, "Major number for LLC driver.  (0 for allocation.)");

static struct cdevsw dl_cdev = {
	.d_name = DRV_NAME,
	.d_str = &llcinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

static __init int
llcinit(void)
{
	int err;

	if (!(dl_priv_cachep =
	      kmem_cache_create("dl_priv_cachep", mi_open_size(sizeof(struct dl)), 0,
				SLAB_HWCACHE_ALIGN, NULL, NULL))) {
		cmn_err(CE_WARN, "%s: Could not allocate dl_priv_cachep\n", DRV_NAME);
		return (-ENOMEM);
	}
	if ((err = register_netdevice_notifier(&dl_notifier))) {
		cmn_err(CE_WARN, "%s: Could not register netdevice notifier, err = %d\n", DRV_NAME,
			-err);
		kmem_cache_destroy(dl_priv_cachep);
		return (err);
	}
	if ((err = register_strdev(&dl_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: Could not register major %d, err = %d\n", DRV_NAME,
			(int) major, -err);
		unregister_netdevice_notifier(&dl_notifier);
		return (err);
	}
	if (major == 0)
		major = err;
	return (0);
}
static __exit void
llcterminate(void)
{
	int err = 0;

	if ((err = unregister_strdev(&dl_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: Could not unregister major %d, err = %d\n", DRV_NAME,
			(int) major, -err);
	}
	if ((err = unregister_netdevice_notifier(&dl_notifier))) {
		cmn_err(CE_WARN, "%s: Could not unregister netdevice notifier, err = %d\n",
			DRV_NAME, -err);
	}
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
	if (kmem_cache_destroy(dl_priv_cachep)) {
		cmn_err(CE_WARN, "%s: Could not destroy dl_priv_cachep\n", DRV_NAME);
	}
#else
	kmem_cache_destroy(dl_priv_cachep);
#endif
	return (void) (err);
}

module_init(llcinit);
module_exit(llcterminate);

#endif				/* LINUX */
