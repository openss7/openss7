/*****************************************************************************

 @(#) $RCSfile: x25-llc.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:56 $

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

 Last Modified $Date: 2008-06-13 06:43:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: x25-llc.c,v $
 Revision 0.9.2.1  2008-06-13 06:43:56  brian
 - added files

 Revision 0.9.2.1  2008-05-03 21:22:40  brian
 - updates for release

 *****************************************************************************/

#ident "@(#) $RCSfile: x25-llc.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:56 $"

static char const ident[] =
    "$RCSfile: x25-llc.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:56 $";

/*
 * This is a driver for the IEEE 802.2 Logical Link Contrl (LLC).  It presents
 * a DLPI interface to its users.  As Linux performs MAC, LLC, SNAP and LLC
 * SNAP internal to the kernel, these elements are internal to the driver.
 * For each MAC device (LAN interface) there is a MAC device, associated LLC
 * station, and an associated SNAP LLC station.  Each driver stream represents
 * an LLC user.  The LLC user stream can be LLC Type 1, LLC Type 2 and LLC
 * Type 3.  LLC Type 3 is not supported yet.  When the driver loads, it
 * interrogates the system for MAC devices and adds LLC and LLC SNAP stations
 * to each MAC device.  When a driver stream attaches to a PPA, it attaches to
 * the MAC device, the LLC station, or the LLC SNAP station, depending on some
 * extra bits in the PPA.  Each PPA is otherwise a Linux interface index.
 *
 * When a driver stream is bound to a PPA for DL_CLDLS service, it is the only
 * stream that will receive DL_CLDLS (MAC or LLC Type 1) packets for the
 * MAC, LLC station, or LLC SNAP station, for the bound MAC and LSAP
 * addresses.  If also bound for DL_CODLS, then the stream may form
 * connections or accept connections for the stream.  All streams can perform
 * XID and TEST operations.
 */

#define _MPS_SOURCE

#include <sys/os7/compat.h>
#include <sys/strsun.h>
#include <sys/dlpi.h>

#define LLC_DESCRIP	"SVR 4.2 DLPI LLC DRIVER FOR LINUX FAST-STREAMS"
#define LLC_EXTRA	"Part of the Openss7 X.25 Stack for Linux Fast-STREAMS"
#define LLC_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define LLC_REVISION	"OpenSS7 $RCSfile: x25-llc.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:56 $"
#define LLC_DEVICE	"SVR 4.2MP DLPI Driver (DLPI) for IEEE 802.2 LLC"
#define LLC_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define LLC_LICENSE	"GPL"
#define LLC_BANNER	LLC_DESCRIP	"\n" \
			LLC_EXTRA	"\n" \
			LLC_COPYRIGHT	"\n" \
			LLC_REVISION	"\n" \
			LLC_DEVICE	"\n" \
			LLC_CONTACT	"\n"
#define LLC_SPLASH	LLC_DESCRIP	" - " \
			LLC_REVISION

#ifndef CONFIG_STREAMS_LLC_NAME
#error "CONFIG_STREAMS_LLC_NAME must be defined."
#endif				/* CONFIG_STREAMS_LLC_NAME */
#ifndef CONFIG_STREAMS_LLC_MODID
#error "CONFIG_STREAMS_LLC_MODID must be defined."
#endif				/* CONFIG_STREAMS_LLC_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(LLC_CONTACT);
MODULE_DESCRIPTION(LLC_DESCRIP);
MODULE_SUPPORTED_DEVICE(LLC_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(LLC_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_LLC_MODID));
MODULE_ALIAS("streams-driver-llc");
MODULE_ALIAS("streams-module-llc");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_LLC_MAJOR));
MODULE_ALIAS("/dev/streams/llc");
MODULE_ALIAS("/dev/streams/llc/*");
MODULE_ALIAS("/dev/streams/clone/llc");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0));
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0) "-" __stringify(LLC_CMINOR));
MODULE_ALIAS("/dev/llc");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#endif				/* LINUX */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define DRV_ID	    LLC_DRV_ID
#define DRV_NAME    LLC_DRV_NAME
#define CMAJORS	    LLC_CMAJORS
#define CMAJOR_0    LLC_CMAJOR_0
#define UNITS	    LLC_UNITS
#ifdef MODULE
#define DRV_BANNER  LLC_BANNER
#else				/* MODULE */
#define DRV_BANNER  LLC_SPLASH
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
 * --------------------------------------------------------------------------
 *
 * PRIVATE STRUCTURE
 *
 * --------------------------------------------------------------------------
 */

struct dl {
	queue_t *oq;
	dl_ulong state;
	dl_ulong oldstate;
	struct {
		dl_info_ack_t info;
		struct {
			dl_qos_co_sel1_t qos;
			dl_qos_co_range1_t qor;
		} co;
		struct {
			dl_qos_cl_sel1_t qos;
			dl_qos_cl_range1_t qor;
		} cl;
	} proto;
	dl_ulong lsap;
};

#define DL_PRIV(q) ((struct dl *)q->q_ptr)

#define STRLOGIO    0		/* input-output controls */
#define STRLOGST    1		/* state transitions */
#define STRLOGTO    2		/* timeouts */
#define STRLOGRX    3		/* primitives received */
#define STRLOGTX    4		/* primitives issued */
#define STRLOGTE    5		/* timer events */
#define STRLOGDA    6		/* data */

static inline const char *
dl_iocname(int cmd)
{
	switch (cmd) {
	default:
		return ("(unknown)");
	}
}

static inline const char *
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
		return ("(unknown)");
	}
}

static inline const char *
dl_statename(dl_ulong state)
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
		return ("(unknown)");
	}
}

static inline const char *
dl_errname(dl_long error)
{
	if (error < 0)
		return ("DL_SYSERR");
	switch (error) {
	case DL_ACCESS:
		return ("DL_ACCESS");
	case DL_BADADDR:
		return ("DL_BADADDR");
	case DL_BADCORR:
		return ("DL_BADCORR");
	case DL_BADDATA:
		return ("DL_BADDATA");
	case DL_BADPPA:
		return ("DL_BADPPA");
	case DL_BADPRIM:
		return ("DL_BADPRIM");
	case DL_BADQOSPARAM:
		return ("DL_BADQOSPARAM");
	case DL_BADQOSTYPE:
		return ("DL_BADQOSTYPE");
	case DL_BADSAP:
		return ("DL_BADSAP");
	case DL_BADTOKEN:
		return ("DL_BADTOKEN");
	case DL_BOUND:
		return ("DL_BOUND");
	case DL_INITFAILED:
		return ("DL_INITFAILED");
	case DL_NOADDR:
		return ("DL_NOADDR");
	case DL_NOTINIT:
		return ("DL_NOTINIT");
	case DL_OUTSTATE:
		return ("DL_OUTSTATE");
	case DL_SYSERR:
		return ("DL_SYSERR");
	case DL_UNSUPPORTED:
		return ("DL_UNSUPPORTED");
	case DL_UNDELIVERABLE:
		return ("DL_UNDELIVERABLE");
	case DL_NOTSUPPORTED:
		return ("DL_NOTSUPPORTED");
	case DL_TOOMANY:
		return ("DL_TOOMANY");
	case DL_NOTENAB:
		return ("DL_NOTENAB");
	case DL_BUSY:
		return ("DL_BUSY");
	case DL_NOAUTO:
		return ("DL_NOAUTO");
	case DL_NOXIDAUTO:
		return ("DL_NOXIDAUTO");
	case DL_NOTESTAUTO:
		return ("DL_NOTESTAUTO");
	case DL_XIDAUTO:
		return ("DL_XIDAUTO");
	case DL_TESTAUTO:
		return ("DL_TESTAUTO");
	case DL_PENDING:
		return ("DL_PENDING");
	default:
		return ("(unknown)");
	}
}

static inline const char *
dl_strerror(dl_long error)
{
	if (error < 0)
		return ("UNIX system error occurred.");
	switch (error) {
	case DL_ACCESS:
		return ("Improper permissions for request.");
	case DL_BADADDR:
		return ("DLSAP addr in improper format or invalid.");
	case DL_BADCORR:
		return ("Seq number not from outstand DL_CONN_IND.");
	case DL_BADDATA:
		return ("User data exceeded provider limit.");
	case DL_BADPPA:
		return ("Specified PPA was invalid.");
	case DL_BADPRIM:
		return ("Primitive received not known by provider.");
	case DL_BADQOSPARAM:
		return ("QOS parameters contained invalid values.");
	case DL_BADQOSTYPE:
		return ("QOS structure type is unknown/unsupported.");
	case DL_BADSAP:
		return ("Bad LSAP selector.");
	case DL_BADTOKEN:
		return ("Token used not an active stream.");
	case DL_BOUND:
		return ("Attempted second bind with dl_max_conind.");
	case DL_INITFAILED:
		return ("Physical Link initialization failed.");
	case DL_NOADDR:
		return ("Provider couldn't allocate alt. address.");
	case DL_NOTINIT:
		return ("Physical Link not initialized.");
	case DL_OUTSTATE:
		return ("Primitive issued in improper state.");
	case DL_SYSERR:
		return ("UNIX system error occurred.");
	case DL_UNSUPPORTED:
		return ("Requested serv. not supplied by provider.");
	case DL_UNDELIVERABLE:
		return ("Previous data unit could not be delivered.");
	case DL_NOTSUPPORTED:
		return ("Primitive is known but not supported.");
	case DL_TOOMANY:
		return ("Limit exceeded.");
	case DL_NOTENAB:
		return ("Promiscuous mode not enabled.");
	case DL_BUSY:
		return ("Other streams for PPA in post-attached state.");
	case DL_NOAUTO:
		return ("Automatic handling of XID and TEST response not supported. .");
	case DL_NOXIDAUTO:
		return ("Automatic handling of XID not supported.");
	case DL_NOTESTAUTO:
		return ("Automatic handling of TEST not supported.");
	case DL_XIDAUTO:
		return ("Automatic handling of XID response.");
	case DL_TESTAUTO:
		return ("Automatic handling of TEST response.");
	case DL_PENDING:
		return ("pending outstanding connect indications.");
	default:
		return ("(unknown)");
	}
}

static dl_ulong
dl_get_state(struct dl *dl)
{
	return (dl->state);
}

static dl_ulong
dl_set_state(struct dl *dl, dl_ulong newstate)
{
	dl_ulong oldstate = dl->state;

	if (newstate != oldstate) {
		dl->state = newstate;
		dl->proto.info.dl_current_state = newstate;
		mi_strlog(dl->oq, STRLOGST, SL_TRACE, "%s <- %s", dl_statename(newstate),
			  dl_statename(oldstate));
	}
	return (newstate);
}

static dl_ulong
dl_save_state(struct dl *dl)
{
	return ((dl->oldstate = dl_get_state(dl)));
}

static dl_ulong
dl_restore_state(struct dl *dl)
{
	return (dl_set_state(dl, dl->oldstate));
}

/*
 * --------------------------------------------------------------------------
 *
 * MAC AND LLC SENT MESSAGES
 *
 * --------------------------------------------------------------------------
 */

#define LLC_MT_RR	0x01	/* 0000 0001 */
#define LLC_MT_RNR	0x05	/* 0000 0101 */
#define LLC_MT_REJ	0x09	/* 0000 1001 */
#define LLC_MT_I	0x00	/* 0000 0000 */
#define LLC_MT_UI	0x03	/* 0000 0011 */
#define LLC_MT_SABM	0x2f	/* 0010 1111 */
#define LLC_MT_SABME	0x6f	/* 0110 1111 */
#define LLC_MT_SM	0xc3	/* 1100 0011 */
#define LLC_MT_DISC	0x43	/* 0100 0011 */
#define LLC_MT_DM	0x0f	/* 0000 1111 */
#define LLC_MT_UA	0x63	/* 0110 0011 */
#define LLC_MT_FRMR	0x87	/* 1000 0111 */
#define LLC_MT_XID	0xaf	/* 1010 1111 */
#define LLC_MT_TEST	0xe3	/* 1110 0011 */

/*
 * --------------------------------------------------------------------------
 *
 * DLPI PROVIDER TO DLPI USER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
m_error(struct dl *dl, queue_t *q, mblk_t *msg, int rerr, int werr)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, 2, BPRI_MED)))) {
		DB_TYPE(mp) = M_ERROR;
		*mp->b_wptr++ = rerr;
		*mp->b_wptr++ = werr;
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- M_ERROR");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
m_error_reply(struct dl *dl, queue_t *q, mblk_t *msg, int error)
{
	switch (error) {
	case 0:
	case -ENOBUFS:
	case -EAGAIN:
	case -EDEADLK:
	case -EBUSY:
	case -ENOMEM:
		return (error);
	}
	if (error > 0)
		return m_error(dl, q, msg, EPROTO, EPROTO);
	return m_error(dl, q, msg, -error, -error);
}

static fastcall inline __hot_read int
dl_data_ind(struct dl *dl, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	if (pcmsg(DB_TYPE(dp)) || bcanputnext(dl->oq, dp->b_band)) {
		mi_strlog(dl->oq, STRLOGDA, SL_TRACE, "<- M_DATA");
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		putnext(dl->oq, dp);
		return (0);
	}
	return (-EBUSY);
}

static fastcall noinline __unlikely int
dl_info_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_info_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_INFO_ACK;
		p->dl_max_sdu = dl->proto.info.dl_max_sdu;
		p->dl_addr_length = dl->proto.info.dl_addr_length;
		p->dl_mac_type = dl->proto.info.dl_mac_type;
		p->dl_reserved = dl->proto.info.dl_reserved;
		p->dl_current_state = dl->proto.info.dl_current_state;
		p->dl_sap_length = dl->proto.info.dl_sap_length;
		p->dl_service_mode = dl->proto.info.dl_service_mode;
		p->dl_qos_length = dl->proto.info.dl_qos_length;
		p->dl_qos_offset = dl->proto.info.dl_qos_offset;
		p->dl_qos_range_length = dl->proto.info.dl_qos_range_length;
		p->dl_qos_range_offset = dl->proto.info.dl_qos_range_offset;
		p->dl_provider_style = dl->proto.info.dl_provider_style;
		p->dl_addr_offset = dl->proto.info.dl_addr_offset;
		p->dl_version = dl->proto.info.dl_version;
		p->dl_brdcst_addr_length = dl->proto.info.dl_brdcst_addr_length;
		p->dl_brdcst_addr_offset = dl->proto.info.dl_brdcst_addr_offset;
		p->dl_growth = dl->proto.info.dl_growth;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_INFO_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_bind_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_bind_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_BIND_ACK;
		p->dl_sap = dl->sap;
		p->dl_addr_length = 0;
		p->dl_addr_offset = sizeof(*p);
		p->dl_max_conind = 0;
		p->dl_xidtest_flg = 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		dl_set_state(dl, DL_IDLE);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_BIND_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
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
		switch (dl_get_state(dl)) {
		case DL_UNBIND_PENDING:
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_RESET_RES_PENDING:
			dl_set_state(dl, DL_DATAXFER);
			break;
		}
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_OK_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_error_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, dl_long error)
{
	dl_error_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_ERROR_ACK;
		p->dl_error_primitive = prim;
		p->dl_errno = error < 0 ? DL_SYSERR : 0;
		p->dl_unix_errno = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		switch (dl_get_state(dl)) {
		case DL_BIND_PENDING:
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_UNBIND_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_OUTCON_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
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
		case DL_RESET_RES_PENDING:
			dl_set_state(dl, DL_PROV_RESET_PENDING);
			break;
		case DL_USER_RESET_PENDING:
			dl_set_state(dl, DL_DATAXFER);
			break;
		}
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_ERROR_ACK [%s,%s] %s",
			  dl_primname(prim), dl_errname(error), dl_strerror(error));
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_error_reply(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, dl_long error)
{
	switch (error) {
	case 0:
	case -ENOBUFS:
	case -EBUSY:
	case -EDEADLK:
	case -ENOMEM:
	case -EAGAIN:
	case -ENOSR:
		return (error);
	}
	return dl_error_ack(dl, q, msg, prim, error);
}

static fastcall noinline __unlikely int
dl_connect_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_connect_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_CONNECT_CON;
			p->dl_resp_addr_length = 0;
			p->dl_resp_addr_offset = sizeof(*p);
			p->dl_qos_length = 0;
			p->dl_qos_offset = sizeof(*p);
			p->dl_growth = 0;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, DL_DATAXFER);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_CONNECT_CON");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_token_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_token_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_TOKEN_ACK;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_TOKEN_ACK");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_disconnect_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong orig, dl_ulong reason)
{
	dl_disconnect_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_DISCONNECT_IND;
			p->dl_originator = orig;
			p->dl_reason = reason;
			p->dl_correlation = 0;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, DL_IDLE);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_DISCONNECT_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_reset_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong orig, dl_ulong reason)
{
	dl_reset_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_RESET_IND;
			p->dl_originator = orig;
			p->dl_reason = reason;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, DL_PROV_RESET_PENDING);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_RESET_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_reset_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reset_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_RESET_CON;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, DL_DATAXFER);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_RESET_CON");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_get_statistics_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_get_statistics_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_GET_STATISTICS_ACK;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_GET_STATISTICS_ACK");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * LAPB TIMERS
 *
 * --------------------------------------------------------------------------
 */

/*
 * --------------------------------------------------------------------------
 *
 * DLPI USER TO DLPI PROVIDER RECEIVED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

static fastcall inline __hot_write int
dl_data_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return cd_unitdata_req(dl->cd, q, NULL, mp);
}

static fastcall noinline __unlikely int
dl_info_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_info_ack(dl, q, mp);
}

/**
 * dl_attach_req: - process DL_ATTACH_REQ primitive
 * @dl: data link private structure
 * @q: active queue (write queue)
 * @mp: the DL_ATTACH_REQ primitive
 *
 * This driver is somewhat different thatn the old ldl driver (which still
 * exists in the strxns package).  This driver codes the Ethertype of MPOxx
 * (NPID) code in the upper 16 bits of the PPA and the (Linux) interface
 * identifier in the lower 16 bits.  When the interface identifier is
 * non-zero, it specifies a particular Linux device.  When the interface
 * identifier is zero, the attachment will be to all devices.
 *
 * For receiving packets, we do not care whether they are native, or whether
 * they contain a snap header, or whether they contain an LLC1 header.  For
 * transmitting packets, however, we need to know whether to include a SNAP
 * header and, for DL_CODLS, whether to include an LLC1 header (an LLC2, X25
 * LAPB, Frame Relay LAPB, ISDN LAPD, header is always included for DL_CODLS).
 * It is independent whether we include a SNAP header or whether we include an
 * LLC1 header: that is, there may no header, a SNAP header, an LLC1 header,
 * or a SNAP and an LLC1 header.  When the S bit is set in the PPA, it
 * indicates that a SNAP header must be included on output, when the L bit is
 * set in the PPA, it indicates that an LLC1 header must be included on
 * DL_CLDS output.
 *
 * Physical Point of Attachment
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |    EtherType or MPOxx Code    |S:L:        Interface Id       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
static fastcall noinline __unlikely int
dl_attach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_attach_req_t *p = (typeof(p)) mp->b_rptr;
	struct netdev *dev;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (dl_get_state(dl) != DL_UNATTACHED)
		goto outstate;
	if (dl->proto.info.dl_provider_style != DL_STYLE2)
		goto notsupported;
	switch ((dl->framing = (p->dl_ppa & DL_FRAME_MASK) >> DL_FRAME_SHIFT)) {
	case DL_FRAME_ETHER:
	case DL_FRAME_802_2:
	case DL_FRAME_SNAP:
	case DL_FRAME_802_3:
	case DL_FRAME_RAW_LLC:
	case DL_FRAME_LLC2:
		break;
	default:
		goto badppa;
	}
	dl->ppa = (p->dl_ppa & DL_PPA_MASK) >> DL_PPA_SHIFT;
	/* lock device map */
	if (!(dev = find_dev(dl->ppa))) {
		/* unlock device map */
		goto badppa;
	}
	dl->type = dev->type;
	/* unlock device map */
	switch (dl->type) {
	case ARPHDR_ETHER:
	case ARPHDR_LOOPBACK:
		break;
	case ARPHDR_FDDI:
		switch (dl->framing) {
		case DL_FRAME_ETHER:
		case DL_FRAME_802_3:
			goto badppa;
		default:
			break;
		}
		break;
	case APRHDR_IEEE802:
	case ARPHDR_IEEE802_TR:
		switch (dl->framing) {
		case DL_FRAME_ETHER:
		case DL_FRAME_802_3:
			goto badppa;
		default:
			break;
		}
		break;
	case ARPHDR_HDLC:
		switch (dl->framing) {
		case DL_FRAME_RAW_LLC:
		case DL_FRAME_LLC2:
			break;
		default:
			goto badppa;
		}
		break;
	default:
		goto badppa;
	}
	switch (dl->type) {
		/* ETH_P_SNAP */
	case ARPHDR_IEEE802:
		/* ETH_P_8021Q */
		/* ETH_P_802_3 */
		dl->proto.info.dl_mac_type = DL_CSMACD;
		dl->proto.info.dl_mac_type = DL_TPB;
		dl->proto.info.dl_mac_type = DL_ETH_CSMA;
		break;
	case ARPHDR_IEEE802_TR:
		/* ETH_P_TR_802_2 */
		dl->proto.info.dl_mac_type = DL_100VGTPR;
		dl->proto.info.dl_mac_type = DL_TPR;
		break;
	case ARPHDR_ETHER:
		dl->proto.info.dl_mac_type = DL_100VG;
		dl->proto.info.dl_mac_type = DL_100BT;
		dl->proto.info.dl_mac_type = DL_ETHER;
		break;
	case ARPHDR_HDLC:
	case ARPHDR_RAWHDLC:
		/* ETH_P_HDLC */
		dl->proto.info.dl_mac_type = DL_HDLC;
		break;
	case ARPHDR_FDDI:
		dl->proto.info.dl_mac_type = DL_FDDI;
		break;
	case ARPHDR_FCPP:
	case ARPHDR_FCAL:
	case ARPHDR_FCPL:
	case ARPHDR_FCFABRIC:
		dl->proto.info.dl_mac_type = DL_FC;
		break;
	case ARPHDR_ATM:
		/* ETH_P_ATMMPOA */
		dl->proto.info.dl_mac_type = DL_IPATM;
		/* ETH_P_ATMFATE */
		dl->proto.info.dl_mac_type = DL_ATM;
		break;
	case ARPHDR_LAPB:
	case ARPHDR_AX25:
		/* ETH_P_X25 */
		/* ETH_P_AX25 */
		dl->prot.dl_mac_type = DL_X25;
		break;
	case ARPHDR_HIPPI:
		dl->prot.dl_mac_type = DL_HIPPI;
		break;
	case ARPHDR_DLCI:
		dl->prot.dl_mac_type = DL_FRAME;
		break;
	case ARPHDR_FRAD:
		dl->prot.dl_mac_type = DL_MPFRAME;
		break;
	case ARPHDR_X25:
		dl->prot.dl_mac_type = DL_IPX25;
		break;
	case ARPHDR_LOOPBACK:
		/* ETH_P_LOOP */
		dl->prot.dl_mac_type = DL_LOOP;
		break;
	default:
		dl->prot.dl_mac_type = DL_ISDN;
		dl->prot.dl_mac_type = DL_METRO;
		dl->prot.dl_mac_type = DL_CHAR;
		dl->prot.dl_mac_type = DL_ASYNC;
		dl->prot.dl_mac_type = DL_CTCA;
		dl->prot.dl_mac_type = DL_OTHER;
		break;
	}
	dl_set_state(dl, DL_ATTACH_PENDING);
	if ((err = dl_ok_ack(dl, q, mp, DL_ATTACH_REQ)))
		goto error;
	return (0);
      badppa:
	err = DL_BADPPA;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_ATTACH_REQ, err);
}

static fastcall noinline __unlikely int
dl_detach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (dl_get_state(dl) != DL_UNBOUND)
		goto outstate;
	if (dl->proto.info.dl_provider_style != DL_STYLE2)
		goto notsupported;
	dl_set_state(dl, DL_DETACH_PENDING);
	if ((err = dl_ok_ack(dl, q, mp, DL_DETACH_REQ)))
		goto error;
	return (0);
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_DETACH_REQ, err);
}

/**
 * dl_bind_req: - precess DL_BIND_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_BIND_REQ primitive
 *
 * SAP addresses are coded different depending on the framing.  If we are
 * using ethernet framing, the SAP is the 16-bit EtherType II code.  If we are
 * using an LLC1 header, the SAP is the LSAP.  It we are using SNAP header,
 * the SAP is the LSAP.  If we are using X.25 framing, the SAP is the A, B, C
 * or D address.  If we are using Frame Relay framing, the SAP is the DLCI.
 */
static fastcall noinline __unlikely int
dl_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (dl_get_state(dl) != DL_UNBOUND)
		goto outstate;
	if (p->dl_service_mode & DL_ACLDLS)
		goto unsupported;
	if (!(p->dl_service_mode & (DL_CODLS | DL_CLDLS)))
		goto unsupported;
	dl->proto.info.dl_service_mode = p->dl_service_mode;
	dl->xidtest = (p->dl_xidtest_flg & (DL_AUTO_XID | DL_AUTO_TEST));
	if ((dl->proto.info.dl_sap_length = check_sap(dl, p->dl_sap)) == 0)
		goto badsap;
	dl->proto.info.dl_sap = p->dl_sap;
	dl_set_state(dl, DL_BIND_PENDING);
	if ((err = dl_bind_ack(dl, q, mp)))
		goto error;
	return (0);
      badsap:
	err = DL_BADSAP;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_BIND_REQ, err);
}

static fastcall noinline __unlikely int
dl_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_state(dl, DL_UNBIND_PENDING);
	if ((err = dl_ok_ack(dl, q, mp, DL_UNBIND_REQ)))
		goto error;
	return (0);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_UNBIND_REQ, err);
}

static fastcall noinline __unlikely int
dl_subs_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!MBLKIN(mp, p->dl_subs_sap_offset, p->dl_subs_sap_length))
		goto fault;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	switch (p->dl_subs_bind_class) {
	case DL_PEER_BIND:
	case DL_HIERARCHICAL_BIND:
		break;
	default:
		goto badsap;
	}
	dl_set_state(dl, DL_SUBS_BIND_PND);
	if ((err = dl_subs_bind_ack(dl, q, mp)))
		goto error;
	return (0);
      badsap:
	err = DL_BADSAP;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_SUBS_BIND_REQ, err);
}

static fastcall noinline __unlikely int
dl_subs_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!MBLKIN(mp, p->dl_subs_sap_offset, p->dl_subs_sap_length))
		goto fault;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_state(dl, DL_SUBS_UNBIND_PND);
	if ((err = dl_ok_ack(dl, q, mp, DL_SUBS_UNBIND_REQ)))
		goto error;
	return (0);
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_SUBS_UNBIND_REQ, err);
}

static fastcall noinline __unlikely int
dl_enabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_enabmulti_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!MBLKIN(mp, p->dl_addr_offset, p->dl_addr_length))
		goto fault;
	switch (dl_get_state(dl)) {
	case DL_UNBOUND:
	case DL_IDLE:
	case DL_OUTCON_PENDING:
	case DL_INCON_PENDING:
	case DL_DATAXFER:
	case DL_USER_RESET_PENDING:
	case DL_PROV_RESET_PENDING:
		break;
	default:
		goto outstate;
	}
	if (p->dl_addr_length != dl->proto.info.dl_addr_length)
		goto badaddr;
	/* we actually need to attempt to add the multicast address to the interface */
	if ((err = dl_ok_ack(dl, q, mp, DL_ENABMULTI_REQ)))
		goto error;
	return (0);
      badaddr:
	err = DL_BADADDR;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_ENABMULTI_REQ, err);
}

static fastcall noinline __unlikely int
dl_disabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_disabmulti_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!MBLKIN(mp, p->dl_addr_offset, p->dl_addr_length))
		goto fault;
	switch (dl_get_state(dl)) {
	case DL_UNBOUND:
	case DL_IDLE:
	case DL_OUTCON_PENDING:
	case DL_INCON_PENDING:
	case DL_DATAXFER:
	case DL_USER_RESET_PENDING:
	case DL_PROV_RESET_PENDING:
		break;
	default:
		goto outstate;
	}
	if (p->dl_addr_length != dl->proto.info.dl_addr_length)
		goto badaddr;
	/* we actually need to attempt to remove the multicast address from the interface */
	if ((err = dl_ok_ack(dl, q, mp, DL_DISABMULTI_REQ)))
		goto error;
	return (0);
      badaddr:
	err = DL_BADADDR;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_DISABMULTI_REQ, err);
}

static fastcall noinline __unlikely int
dl_promiscon_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_promiscon_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	switch (dl_get_state(dl)) {
	case DL_UNBOUND:
	case DL_IDLE:
	case DL_OUTCON_PENDING:
	case DL_INCON_PENDING:
	case DL_DATAXFER:
	case DL_USER_RESET_PENDING:
	case DL_PROV_RESET_PENDING:
		break;
	default:
		goto outstate;
	}
	switch (p->dl_level) {
	case DL_PROMISC_PHYS:	/* at MAC address or raw X.25 level */
		/* Indicates that all frames should be delivered from the PPA regardless of the
		   physical address. */
	case DL_PROMISC_SAP:	/* at LSAP address or X.25 level */
		/* Indicates that all frames should be delivered from the PPA regardless of the SAP 
		   address. */
	case DL_PROMISC_MULTI:	/* at MAC multicast address level */
		/* Indicates that all frames should be delivered to the PPA when sent to any
		   multicast address. */
		break;
	default:
		goto unsupported;
	}
	if ((err = dl_ok_ack(dl, q, mp, DL_PROMISCON_REQ)))
		goto error;
	return (0);
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_PROMISCON_REQ, err);
}

static fastcall noinline __unlikely int
dl_promiscoff_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_promiscoff_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	switch (dl_get_state(dl)) {
	case DL_UNBOUND:
	case DL_IDLE:
	case DL_OUTCON_PENDING:
	case DL_INCON_PENDING:
	case DL_DATAXFER:
	case DL_USER_RESET_PENDING:
	case DL_PROV_RESET_PENDING:
		break;
	default:
		goto outstate;
	}
	switch (p->dl_level) {
	case DL_PROMISC_PHYS:
	case DL_PROMISC_SAP:
	case DL_PROMISC_MULTI:
		break;
	default:
		goto unsupported;
	}
	if ((err = dl_ok_ack(dl, q, mp, DL_PROMISCOFF_REQ)))
		goto error;
	return (0);
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_PROMISCOFF_REQ, err);
}

static fastcall noinline __unlikely int
dl_unitdata_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto fault;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	switch (dl->proto.info.dl_mac_type) {
	case DL_X25:
		if (p->dl_dest_addr_length != 0)
			goto badaddr;
		break;
	default:
		if (p->dl_dest_addr_length != dl->proto.info.dl_addr_length)
			goto badaddr;
		break;
	}
	/* send (and absorb) the message */
	return (0);
      badaddr:
	err = DL_BADADDR;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_uderror_reply(dl, q, mp, DL_UNITDATA_REQ, err);
}

static fastcall noinline __unlikely int
dl_udqos_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_UDQOS_REQ, DL_UNSUPPORTED);
}

static fastcall noinline __unlikely int
dl_connect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_req_t *p = (typeof(p)) mp->b_rptr;
	dl_qos_co_range1_t qor;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	switch (dl->proto.info.dl_mac_type) {
	case DL_X25:
		if (p->dl_dest_addr_length != 0)
			goto badaddr;
		break;
	default:
		if (p->dl_dest_addr_length != dl->proto.info.dl_addr_length)
			goto badaddr;
		break;
	}
	if (p->dl_qos_length) {
		if (p->dl_qos_length < sizeof(qor.dl_qos_type))
			goto fault;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qor.dl_qos_type, sizeof(qor.dl_qos_type));
		if (qor.dl_qos_type != DL_QOS_CO_RANGE1)
			goto badqostype;
		if (p->dl_qos_length < sizeof(qor))
			goto fault;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qor, sizeof(qor));
		if (qor.dl_rcv_throughput.dl_accept_value != DL_QOS_DONT_CARE)
			dl->proto.info.co.qor.dl_rcv_throughput.dl_accept_value =
			    qor.dl_rcv_throughput.dl_accept_value;
		if (qor.dl_rcv_throughput.dl_target_value != DL_QOS_DONT_CARE)
			dl->proto.info.co.qor.dl_rcv_throughput.dl_target_value =
			    qor.dl_rcv_throughput.dl_target_value;
		else
			dl->proto.info.co.qor.dl_rcv_throughput.dl_target_value =
			    qor.dl_rcv_throughput.dl_accept_value;
		if (qor.dl_rcv_trans_delay.dl_accept_value != DL_QOS_DONT_CARE)
			dl->proto.info.co.qor.dl_rcv_trans_delay.dl_accept_value =
			    qor.dl_rcv_trans_delay.dl_accept_value;
		if (qor.dl_rcv_trans_delay.dl_target_value != DL_QOS_DONT_CARE)
			dl->proto.info.co.qor.dl_rcv_trans_delay.dl_target_value =
			    qor.dl_rcv_trans_delay.dl_target_value;
		else
			dl->proto.info.co.qor.dl_rcv_trans_delay.dl_target_value =
			    qor.dl_rcv_trans_delay.dl_accept_value;
		if (qor.dl_xmt_throughput.dl_accept_value != DL_QOS_DONT_CARE)
			dl->proto.info.co.qor.dl_xmt_throughput.dl_accept_value =
			    qor.dl_xmt_throughput.dl_accept_value;
		if (qor.dl_xmt_throughput.dl_target_value != DL_QOS_DONT_CARE)
			dl->proto.info.co.qor.dl_xmt_throughput.dl_target_value =
			    qor.dl_xmt_throughput.dl_target_value;
		else
			dl->proto.info.co.qor.dl_xmt_throughput.dl_target_value =
			    qor.dl_xmt_throughput.dl_accept_value;
		if (qor.dl_xmt_trans_delay.dl_accept_value != DL_QOS_DONT_CARE)
			dl->proto.info.co.qor.dl_xmt_trans_delay.dl_accept_value =
			    qor.dl_xmt_trans_delay.dl_accept_value;
		if (qor.dl_xmt_trans_delay.dl_target_value != DL_QOS_DONT_CARE)
			dl->proto.info.co.qor.dl_xmt_trans_delay.dl_target_value =
			    qor.dl_xmt_trans_delay.dl_target_value;
		else
			dl->proto.info.co.qor.dl_xmt_trans_delay.dl_target_value =
			    qor.dl_xmt_trans_delay.dl_accept_value;
		/* FIXME: do some more */
	}
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_state(dl, DL_OUTCON_PENDING);
	/* FIXME: send SABM/SABME/SM and absorb message */
	/* Note: before sending SABM/SABME/SM, the data link should be tested for an active channel 
	   state.  This can be accomplished using the TEST procedures for LLC and others.  Also, an 
	   XID exchange should be made to negotiate parameters before initiating the connection (if 
	   DL_AUTO_XID is specified).  Only if the XID procedure fails should default or configured 
	   values be used (unless DL_AUTO_XID is not specified).  Note that the TEST procedure can
	   also be used to determine whether oversized frames can be accepted by the remote LLC
	   SAP. */
	return (0);
      badqostype:
	err = DL_BADQOSTYPE;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_CONNECT_REQ, err);
}

/**
 * dl_connect_res: - process a DL_CONNECT_RES primitive from the DLS User
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_CONNECT_RES message
 *
 * This message is in response to a DL_CONNECT_IND that was issued due to the
 * receipt of an SABM/SABME/SM command frame in the disconnected phase.  The
 * response held awaiting either a rejection (DL_DISCONNECT_REQ) or an
 * acceptance (DL_CONNECT_RES) of the connection.
 */
static fastcall noinline __unlikely int
dl_connect_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto fault;
	if (dl_get_state(dl) != DL_INCON_PENDING)
		goto outstate;
	if (p->dl_qos_length) {
		dl_qos_co_sel1_t qos;

		/* The quality of service parameters selected must be a dl_qos_co_sel1_t structure. 
		 */
		if (p->dl_qos_length < sizeof(qos.dl_qos_type))
			goto fault;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qos.dl_qos_type, sizeof(qos.dl_qos_type));
		if (qos.dl_qos_type != DL_QOS_CO_SEL1)
			goto badqostype;
		if (p->dl_qos_length < sizeof(qos))
			goto fault;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qos, sizeof(qos));
		/* FIXME: process QOS selection. */
	}
      badqostype:
	err = DL_BADQOSTYPE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_CONNECT_RES, err);
}

static fastcall noinline __unlikely int
dl_token_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_token_ack(dl, q, mp);
}

static fastcall noinline __unlikely int
dl_disconnect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_disconnect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
		goto discard;
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
		goto outstate;
	}
	return cd_disable_req(dl->cd, q, mp, CD_FLUSH);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_TOKEN_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_reset_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_DATAXFER)
		goto outstate;
	dl_set_state(dl, DL_USER_RESET_PENDING);
	return cd_abort_output_req(dl->cd, q, mp);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_RESET_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_reset_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_PROV_RESET_PENDING)
		goto outstate;
	dl_set_state(dl, DL_RESET_RES_PENDING);
	return cd_abort_output_req(dl->cd, q, mp);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_RESET_RES, err);
      discard:
	freemsg(mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_data_ack_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_DATA_ACK_REQ, DL_UNSUPPORTED);
}

static fastcall noinline __unlikely int
dl_reply_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_REPLY_REQ, DL_UNSUPPORTED);
}

static fastcall noinline __unlikely int
dl_reply_update_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_REPLY_UPDATE_REQ, DL_UNSUPPORTED);
}

static fastcall noinline __unlikely int
dl_xid_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_XID_REQ, DL_NOTSUPPORTED);
}

static fastcall noinline __unlikely int
dl_xid_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_XID_RES, DL_NOTSUPPORTED);
}

static fastcall noinline __unlikely int
dl_test_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_TEST_REQ, DL_NOTSUPPORTED);
}

static fastcall noinline __unlikely int
dl_test_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_TEST_RES, DL_NOTSUPPORTED);
}

static fastcall noinline __unlikely int
dl_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_PHYS_ADDR_REQ, DL_NOTSUPPORTED);
}

static fastcall noinline __unlikely int
dl_set_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_SET_PHYS_ADDR_REQ, DL_NOTSUPPORTED);
}

static fastcall noinline __unlikely int
dl_get_statistics_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_get_statistics_ack(dl, q, mp);
}

static fastcall noinline __unlikely int
dl_wrongway_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_ulong *p = (typeof(p)) mp->b_rptr;

	return dl_error_reply(dl, q, mp, *p, DL_BADPRIM);
}

static fastcall noinline __unlikely int
dl_other_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_ulong *p = (typeof(p)) mp->b_rptr;

	return dl_error_reply(dl, q, mp, *p, DL_BADPRIM);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * -------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
dl_w_data(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);	/* already locked */

	mi_strlog(q, STRLOGDA, SL_TRACE, "-> M_DATA");
	return dl_data_req(dl, q, mp);
}

static fastcall noinline __unlikely int
dl_w_proto(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_ulong prim;
	int rtn = -EFAULT;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	prim = *(dl_ulong *) mp->b_rptr;
	dl_save_state(dl);
	mi_strlog(q, STRLOGRX, SL_TRACE, "-> %s", dl_primname(prim));
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
	case DL_INFO_ACK:
	case DL_BIND_ACK:
	case DL_OK_ACK:
	case DL_ERROR_ACK:
	case DL_SUBS_BIND_ACK:
	case DL_TOKEN_ACK:
	case DL_PHYS_ADDR_ACK:
	case DL_GET_STATISTICS_ACK:
	case DL_UNITDATA_IND:
	case DL_UDERROR_IND:
	case DL_CONNECT_IND:
	case DL_CONNECT_CON:
	case DL_DISCONNECT_IND:
	case DL_RESET_IND:
	case DL_RESET_CON:
	case DL_DATA_ACK_IND:
	case DL_DATA_ACK_STATUS_IND:
	case DL_REPLY_IND:
	case DL_REPLY_STATUS_IND:
	case DL_REPLY_UPDATE_STATUS_IND:
	case DL_XID_IND:
	case DL_XID_CON:
	case DL_TEST_IND:
	case DL_TEST_CON:
		rtn = dl_wrongway_ind(dl, q, mp);
		break;
	default:
		rtn = dl_other_req(dl, q, mp);
		break;
	}
      done:
	if (rtn)
		dl_restore_state(dl);
	return (0);
}

static fastcall noinline __unlikely int
dl_w_ctl(queue_t *q, mblk_t *mp)
{
	/* No safe path for now... */
	return dl_w_proto(q, mp);
}

static fastcall noinline __unlikely int
dl_w_sig(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_w_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_w_ioctl(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_w_iocdata(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_w_read(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_w_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "%s; bad message type on write queue %d\n", __FUNCTION__,
		  (int) DB_TYPE(mp));
	putnext(q, mp);
	return (0);
}

static fastcall inline int
dl_w_msg_put(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return (-EAGAIN);	/* fast path for data */
	if (likely(DB_TYPE(mp) == M_PROTO))
		return (-EAGAIN);	/* fast path for data */
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
	case M_PROTO:
	case M_PCPROTO:
	case M_CTL:
	case M_PCCTL:
	case M_SIG:
	case M_PCSIG:
	case M_RSE:
	case M_PCRSE:
		return (-EAGAIN);
	case M_FLUSH:
		return dl_w_flush(q, mp);
	case M_IOCTL:
		return dl_w_ioctl(q, mp);
	case M_IOCDATA:
		return dl_w_iocdata(q, mp);
	case M_READ:
		return dl_w_read(q, mp);
	default:
		return dl_w_other(q, mp);
	}
}

static fastcall noinline int
dl_w_msg_srv(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);

	if (unlikely(dl->proto.info.dl_primitive == 0))
		return (-EAGAIN);
	if (likely(DB_TYPE(mp) == M_DATA))
		return dl_w_data(q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return dl_w_proto(q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return dl_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_w_proto(q, mp);
	case M_CTL:
	case M_PCCTL:
		return dl_w_ctl(q, mp);
	case M_SIG:
	case M_PCSIG:
		return dl_w_sig(q, mp);
	case M_RSE:
	case M_PCRSE:
		return dl_w_rse(q, mp);
	case M_FLUSH:
		return dl_w_flush(q, mp);
	case M_IOCTL:
		return dl_w_ioctl(q, mp);
	case M_IOCDATA:
		return dl_w_iocdata(q, mp);
	case M_READ:
		return dl_w_read(q, mp);
	default:
		return dl_w_other(q, mp);
	}
}

static fastcall noinline int
dl_r_msg_srv(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_PROTO))
		return dl_r_proto(q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return dl_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_r_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return dl_r_sig(q, mp);
	case M_CTL:
	case M_PCCTL:
		return dl_r_ctl(q, mp);
	case M_RSE:
	case M_PCRSE:
		return dl_r_rse(q, mp);
	case M_FLUSH:
		return dl_r_flush(q, mp);
	case M_ERROR:
		return dl_r_error(q, mp);
	case M_HANGUP:
	case M_UNHANGUP:
		return dl_r_hangup(q, mp);
	case M_SETOPTS:
	case M_PCSETOPTS:
		return dl_r_setopts(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return dl_r_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return dl_r_iocack(q, mp);
	default:
		return dl_r_other(q, mp);
	}
}

static fastcall inline int
dl_r_msg_put(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_PROTO))
		return (-EAGAIN);	/* fast path for data */
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
	case M_PROTO:
	case M_PCPROTO:
	case M_SIG:
	case M_PCSIG:
	case M_CTL:
	case M_PCCTL:
	case M_RSE:
	case M_PCRSE:
		return (-EAGAIN);	/* handle from service procedure */
	case M_FLUSH:
		return dl_r_flush(q, mp);
	case M_ERROR:
		return dl_r_error(q, mp);
	case M_HANGUP:
	case M_UNHANGUP:
		return dl_r_hangup(q, mp);
	case M_SETOPTS:
	case M_PCSETOPTS:
		return dl_r_setopts(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return dl_r_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return dl_r_iocack(q, mp);
	default:
		return dl_r_other(q, mp);
	}
}

/*
 * --------------------------------------------------------------------------
 *
 * PUT AND SERVICE PROCEDURES
 *
 * --------------------------------------------------------------------------
 */

static streamscall int
dl_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || dl_w_msg_put(q, mp))
		putq(q, mp);
	return (0);
}

static streamscall int
dl_wsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (dl_w_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}

static streamscall int
dl_rsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (dl_r_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}

static streamscall int
dl_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || dl_r_msg_put(q, mp))
		putq(q, mp);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * OPEN AND CLOSE ROUTINES
 *
 * --------------------------------------------------------------------------
 */

caddr_t dl_opens = NULL;

static int
dl_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct dl *dl;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if ((err = mi_open_comm(&dl_opens, sizeof(*p), q, devp, oflags, sflag, crp)))
		return (err);

	dl = DL_PRIV(q);
	bzero(dl, sizeof(*dl));

	/* initialize private structure */
	dl->oq = q;
	dl->state = DL_UNATTACHED;
	dl->oldstate = DL_UNATTACHED;

	dl->proto.info.dl_primitive = DL_INFO_ACK;
	dl->proto.info.dl_max_sdu = 128;
	dl->proto.info.dl_min_sdu = 1;
	dl->proto.info.dl_addr_length = 0;	/* will be 8: 6 octets MAC + SSAP + DSAP */
	dl->proto.info.dl_mac_type = DL_OTHER;	/* will be DL_ETH_CSMA or DL_CSMACD */
	dl->proto.info.dl_reserved = 0;
	dl->proto.info.dl_current_state = DL_UNATTACHED;
	dl->proto.info.dl_sap_length = 0;	/* until bound */
	dl->proto.info.dl_service_mode = (DL_CLDLS | DL_CODLS);
	dl->proto.info.dl_qos_length = 0;
	dl->proto.info.dl_qos_offset = 0;
	dl->proto.info.dl_qos_range_length = 0;
	dl->proto.info.dl_qos_range_offset = 0;
	dl->proto.info.dl_provider_style = DL_STYLE2;
	dl->proto.info.dl_addr_offset = 0;
	dl->proto.info.dl_version = DL_CURRENT_VERSION;
	dl->proto.info.dl_brdcst_addr_length = 0;
	dl->proto.info.dl_brdcst_addr_offset = 0;
	dl->proto.info.dl_growth = 0;

	dl->proto.info.co.qos.dl_qos_type = DL_QOS_CO_SEL1;
	dl->proto.info.co.qos.dl_rcv_throughput = DL_UNKNOWN;
	dl->proto.info.co.qos.dl_rcv_trans_delay = DL_UNKNOWN;
	dl->proto.info.co.qos.dl_xmt_throughput = DL_UNKNOWN;
	dl->proto.info.co.qos.dl_xmt_trans_delay = DL_UNKNOWN;
	dl->proto.info.co.qos.dl_priority = DL_UNKNOWN;
	dl->proto.info.co.qos.dl_protection = DL_UNKNOWN;
	dl->proto.info.co.qos.dl_residual_error = DL_UNKNOWN;
	dl->proto.info.co.qos.dl_resilience.dl_disc_prob = DL_UNKNOWN;
	dl->proto.info.co.qos.dl_resilience.dl_reset_prob = DL_UNKNOWN;

	dl->proto.info.co.qor.dl_qos_type = DL_QOS_CO_RANGE1;
	dl->proto.info.co.qor.dl_rcv_throughput.dl_target_value = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_rcv_throughput.dl_accept_value = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_rcv_trans_delay.dl_target_value = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_rcv_trans_delay.dl_accept_value = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_xmt_throughput.dl_target_value = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_xmt_throughput.dl_accept_value = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_xmt_trans_delay.dl_target_value = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_xmt_trans_delay.dl_accept_value = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_priority.dl_min = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_priority.dl_max = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_protection.dl_min = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_protection.dl_max = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_residual_error = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_resilience.dl_disc_prob = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_resilience.dl_reset_prob = DL_UNKNOWN;

	dl->proto.info.cl.qos.dl_qos_type = DL_QOS_CL_SEL1;
	dl->proto.info.cl.qos.dl_trans_delay = DL_UNKNOWN;
	dl->proto.info.cl.qos.dl_priority = DL_UNKNOWN;
	dl->proto.info.cl.qos.dl_protection = DL_UNKNOWN;
	dl->proto.info.cl.qos.dl_residual_error = DL_UNKNOWN;

	dl->proto.info.cl.qor.dl_qos_type = DL_QOS_CL_RANGE1;
	dl->proto.info.cl.qor.dl_trans_delay.dl_target_value = DL_UNKNOWN;
	dl->proto.info.cl.qor.dl_trans_delay.dl_accept_value = DL_UNKNOWN;
	dl->proto.info.cl.qor.dl_priority.dl_min = DL_UNKNOWN;
	dl->proto.info.cl.qor.dl_priority.dl_max = DL_UNKNOWN;
	dl->proto.info.cl.qor.dl_protection.dl_min = DL_UNKNOWN;
	dl->proto.info.cl.qor.dl_protection.dl_max = DL_UNKNOWN;
	dl->proto.info.cl.qor.dl_residual_error = DL_UNKNOWN;

	qprocson(q);
	return (0);
}

static int
dl_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	if (p == NULL)
		return (0);
	qprocsoff(q);
	mi_close_comm(&dl_opens, q);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

static struct qinit dl_rinit = {
	.qi_putp = &dl_rput,
	.qi_srvp = &dl_rsrv,
	.qi_qopen = &dl_qopen,
	.qi_qclose = &dl_qclose,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_rstat,
};

static struct qinit dl_winit = {
	.qi_putp = &dl_wput,
	.qi_srvp = &dl_wsrv,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_wstat,
};

struct streamtab llc_info = {
	.st_rdinit = &dl_rinit,
	.st_wrinit = &dl_winit,
};

static modID_t modid = MOD_ID;

/*
 * --------------------------------------------------------------------------
 *
 * LINUX FAST STREAMS REGISTRATION
 *
 * --------------------------------------------------------------------------
 */

#ifdef LINUX

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for LLC.  (0 for allocation)");

#ifdef LFS

struct cdevsw llc_cdev = {
	.d_str = &llc_info,
	.d_flag = D_MP | D_CLONE,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

static int
llc_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&llc_cdev, major)) < 0)
		return (err);
	return (0);
}

static int
llc_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&llc_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

#ifdef LIS

static int
llc_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &llc_info, UNITS, DRV_NAME)) < 0)
		return (err);
	if (major == 0)
		major = err;
	if ((err = lis_register_driver_qlock_option(major, LIS_QLOCK_NAME)) < 0) {
		lis_unregister_strdev(major);
		return (err);
	}
	return (0);
}

static int
llc_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

static __init int
llc_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = llc_register_strdev(major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register driver major %d", DRV_NAME, (int) major);
		return (err);
	}
	return (0);
}

static __exit void
llc_modexit(void)
{
	int err;

	if ((err = llc_unregister_strdev(major)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister driver, err = %d", DRV_NAME, err);
		return;
	}
	return;
}

module_init(llc_modinit);
module_exit(llc_modexit);

#endif				/* LINUX */
