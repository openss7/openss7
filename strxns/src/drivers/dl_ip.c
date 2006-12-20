/*****************************************************************************

 @(#) $RCSfile: dl_ip.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2006/12/20 23:11:01 $

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

 Last Modified $Date: 2006/12/20 23:11:01 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dl_ip.c,v $
 Revision 0.9.2.4  2006/12/20 23:11:01  brian
 - current development

 Revision 0.9.2.3  2006/05/08 11:26:11  brian
 - post inc problem and working through test cases

 Revision 0.9.2.2  2006/04/18 18:00:45  brian
 - working up DL and NP drivers

 Revision 0.9.2.1  2006/04/13 18:32:50  brian
 - working up DL and NP drivers.

 *****************************************************************************/

#ident "@(#) $RCSfile: dl_ip.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2006/12/20 23:11:01 $"

static char const ident[] =
    "$RCSfile: dl_ip.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2006/12/20 23:11:01 $";

/*
 *  This is a DLPI driver for the IP subsystem.  The purpose of the driver is to directly access the
 *  packet layer beneath IP for the purposes of network interface taps and testing (primarily for
 *  testing SCTP and RTP).  It is effectively equivalent to a raw socket, except that it has the
 *  ability to intercept IP packets.
 */

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>

#define d_tst_bit(nr,addr)	test_bit(nr,addr)
#define d_set_bit(nr,addr)	__set_bit(nr,addr)
#define d_clr_bit(nr,addr)	__clear_bit(nr,addr)

#include <linux/interrupt.h>

#include <sys/dlpi.h>

#define DL_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define DL_EXTRA	"Part of the OpenSS7 stack for Linux Fast-STREAMS"
#define DL_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define DL_REVISION	"OpenSS7 $RCSfile: dl_ip.c,v $ $Name:  $ ($Revision: 0.9.2.4 $) $Date: 2006/12/20 23:11:01 $"
#define DL_DEVICE	"SVR 4.2 STREAMS DLPI DL_IP Data Link Provider"
#define DL_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define DL_LICENSE	"GPL"
#define DL_BANNER	DL_DESCRIP	"\n" \
			DL_EXTRA	"\n" \
			DL_COPYRIGHT	"\n" \
			DL_DEVICE	"\n" \
			DL_CONTACT
#define DL_SPLASH	DL_DESCRIP	"\n" \
			DL_REVISION

#ifdef LINUX
MODULE_AUTHOR(DL_CONTACT);
MODULE_DESCRIPTION(DL_DESCRIP);
MODULE_SUPPORTED_DEVICE(DL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(DL_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-dl_ip");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define DL_IP_DRV_ID	CONFIG_STREAMS_DL_IP_MODID
#define DL_IP_DRV_NAME	CONFIG_STREAMS_DL_IP_NAME
#define DL_IP_CMAJORS	CONFIG_STREAMS_DL_IP_NMAJOR
#define DL_IP_CMAJOR_0	CONFIG_STREAMS_DL_IP_MAJOR
#define DL_IP_UNITS	CONFIG_STREAMS_DL_IP_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_DL_MODID));
MODULE_ALIAS("streams-driver-dl_ip");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_DL_MAJOR));
MODULE_ALIAS("/dev/streams/dl_ip");
MODULE_ALIAS("/dev/streams/dl_ip/*");
MODULE_ALIAS("/dev/streams/clone/dl_ip");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(DL_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(DL_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(DL_CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(DL_CMAJOR_0) "-" __stringify(DL_CMINOR));
MODULE_ALIAS("/dev/dl_ip");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  STREAMS Definitions
 *  ===================
 */

#define DRV_ID		DL_IP_DRV_ID
#define CMAJORS		DL_IP_CMAJORS
#define CMAJOR_0	DL_IP_CMAJOR_0
#define UNITS		DL_IP_UNITS
#ifdef MODULE
#define DRV_BANNER	DL_IP_BANNER
#else				/* MODULE */
#define DRV_BANNER	DL_IP_SPLASH
#endif				/* MODULE */

STATIC struct module_info dl_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size acceptd */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_hiwat = 1 << 10,		/* Lo water mark */
};

STATIC struct module_stat dl_mstat = {
};

/* Upper multiplex is a DL provider following the DLPI. */

STATIC streamscall int dl_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int dl_qclose(queue_t *, int, cred_t *);

STATIC struct qinit dl_rinit = {
	.qi_putp = &ss7_oput,		/* Read put procedure (message from below) */
	.qi_srvp = &ss7_osrv,		/* Read service procedure */
	.qi_qopen = &dl_qopen,		/* Each open */
	.qi_qclose = &dl_qclose,	/* Last close */
	.qi_minfo = &dl_minfo,		/* Module information */
	.qi_mstat = &dl_mstat,		/* Module statistics */
};

STATIC struct qinit dl_winit = {
	.qi_putp = &ss7_iput,		/* Read put procedure (message from below) */
	.qi_srvp = &ss7_isrv,		/* Read service procedure */
	.qi_minfo = &dl_minfo,		/* Module information */
	.qi_mstat = &dl_mstat,		/* Module statistics */
};

STATIC struct streamtab dl_info = {
	.st_rdinit = &dl_rinit,		/* Upper read queue */
	.st_wrinit = &dl_winit,		/* Upper write queue */
};

/*
 *  Addressing:
 *
 *  PPAs are network interface device indexes.  PPA 0 is for all devices.
 *
 *  DLSAPs are IP Protocol numbers.  Initially it is only supporting IPv4.
 */

/*
 *  Some notes on logging.  This driver uses the strlog(9) capability so that the driver can be
 *  debugged and traced in a production environment.  Things that are trace logged are:
 *
 *  1. issued errors
 *  2. primitives issued from the DLS user
 *  3. primitives issued by the DLS provider
 *  4. DLS interface state transitions
 *  5. time set, clear and expiry
 *
 *  In addition errors are logged to the error logger.
 */
#define DL_LOG_DLS_ERROR    0
#define DL_LOG_DLSU_PRIM    1
#define DL_LOG_DLSP_PRIM    2
#define DL_LOG_DLS_STATE    3
#define DL_LOG_DLS_TIMER    4

STATIC const char *
dl_prim_name(dl_ulong prim)
{
	switch (prim) {
	/* *INDENT-OFF* */
	case DL_INFO_REQ:		return ("DL_INFO_REQ");
	case DL_INFO_ACK:		return ("DL_INFO_ACK");
	case DL_ATTACH_REQ:		return ("DL_ATTACH_REQ");
	case DL_DETACH_REQ:		return ("DL_DETACH_REQ");
	case DL_BIND_REQ:		return ("DL_BIND_REQ");
	case DL_BIND_ACK:		return ("DL_BIND_ACK");
	case DL_UNBIND_REQ:		return ("DL_UNBIND_REQ");
	case DL_OK_ACK:			return ("DL_OK_ACK");
	case DL_ERROR_ACK:		return ("DL_ERROR_ACK");
	case DL_SUBS_BIND_REQ:		return ("DL_SUBS_BIND_REQ");
	case DL_SUBS_BIND_ACK:		return ("DL_SUBS_BIND_ACK");
	case DL_SUBS_UNBIND_REQ:	return ("DL_SUBS_UNBIND_REQ");
	case DL_ENABMULTI_REQ:		return ("DL_ENABMULTI_REQ");
	case DL_DISABMULTI_REQ:		return ("DL_DISABMULTI_REQ");
	case DL_PROMISCON_REQ:		return ("DL_PROMISCON_REQ");
	case DL_PROMISCOFF_REQ:		return ("DL_PROMISCOFF_REQ");
	case DL_UNITDATA_REQ:		return ("DL_UNITDATA_REQ");
	case DL_UNITDATA_IND:		return ("DL_UNITDATA_IND");
	case DL_UDERROR_IND:		return ("DL_UDERROR_IND");
	case DL_UDQOS_REQ:		return ("DL_UDQOS_REQ");
	case DL_CONNECT_REQ:		return ("DL_CONNECT_REQ");
	case DL_CONNECT_IND:		return ("DL_CONNECT_IND");
	case DL_CONNECT_RES:		return ("DL_CONNECT_RES");
	case DL_CONNECT_CON:		return ("DL_CONNECT_CON");
	case DL_TOKEN_REQ:		return ("DL_TOKEN_REQ");
	case DL_TOKEN_ACK:		return ("DL_TOKEN_ACK");
	case DL_DISCONNECT_REQ:		return ("DL_DISCONNECT_REQ");
	case DL_DISCONNECT_IND:		return ("DL_DISCONNECT_IND");
	case DL_RESET_REQ:		return ("DL_RESET_REQ");
	case DL_RESET_IND:		return ("DL_RESET_IND");
	case DL_RESET_RES:		return ("DL_RESET_RES");
	case DL_RESET_CON:		return ("DL_RESET_CON");
	case DL_DATA_ACK_REQ:		return ("DL_DATA_ACK_REQ");
	case DL_DATA_ACK_IND:		return ("DL_DATA_ACK_IND");
	case DL_DATA_ACK_STATUS_IND:	return ("DL_DATA_ACK_STATUS_IND");
	case DL_REPLY_REQ:		return ("DL_REPLY_REQ");
	case DL_REPLY_IND:		return ("DL_REPLY_IND");
	case DL_REPLY_STATUS_IND:	return ("DL_REPLY_STATUS_IND");
	case DL_REPLY_UPDATE_REQ:	return ("DL_REPLY_UPDATE_REQ");
	case DL_REPLY_UPDATE_STATUS_IND:return ("DL_REPLY_UPDATE_STATUS_IND");
	case DL_XID_REQ:		return ("DL_XID_REQ");
	case DL_XID_IND:		return ("DL_XID_IND");
	case DL_XID_RES:		return ("DL_XID_RES");
	case DL_XID_CON:		return ("DL_XID_CON");
	case DL_TEST_REQ:		return ("DL_TEST_REQ");
	case DL_TEST_IND:		return ("DL_TEST_IND");
	case DL_TEST_RES:		return ("DL_TEST_RES");
	case DL_TEST_CON:		return ("DL_TEST_CON");
	case DL_PHYS_ADDR_REQ:		return ("DL_PHYS_ADDR_REQ");
	case DL_PHYS_ADDR_ACK:		return ("DL_PHYS_ADDR_ACK");
	case DL_SET_PHYS_ADDR_REQ:	return ("DL_SET_PHYS_ADDR_REQ");
	case DL_GET_STATISTICS_REQ:	return ("DL_GET_STATISTICS_REQ");
	case DL_GET_STATISTICS_ACK:	return ("DL_GET_STATISTICS_ACK");
	default:			return ("DL_???");
	/* *INDENT-ON* */
	}
}

/* state names */
STATIC const char *
dl_state_name(dl_ulong state)
{
	switch (state) {
	/* *INDENT-OFF* */
	case DL_UNATTACHED:		return ("DL_UNATTACHED");
	case DL_ATTACH_PENDING:		return ("DL_ATTACH_PENDING");
	case DL_DETACH_PENDING:		return ("DL_DETACH_PENDING");
	case DL_UNBOUND:		return ("DL_UNBOUND");
	case DL_BIND_PENDING:		return ("DL_BIND_PENDING");
	case DL_UNBIND_PENDING:		return ("DL_UNBIND_PENDING");
	case DL_IDLE:			return ("DL_IDLE");
	case DL_UDQOS_PENDING:		return ("DL_UDQOS_PENDING");
	case DL_OUTCON_PENDING:		return ("DL_OUTCON_PENDING");
	case DL_INCON_PENDING:		return ("DL_INCON_PENDING");
	case DL_CONN_RES_PENDING:	return ("DL_CONN_RES_PENDING");
	case DL_DATAXFER:		return ("DL_DATAXFER");
	case DL_USER_RESET_PENDING:	return ("DL_USER_RESET_PENDING");
	case DL_PROV_RESET_PENDING:	return ("DL_PROV_RESET_PENDING");
	case DL_RESET_RES_PENDING:	return ("DL_RESET_RES_PENDING");
	case DL_DISCON8_PENDING:	return ("DL_DISCON8_PENDING");
	case DL_DISCON9_PENDING:	return ("DL_DISCON9_PENDING");
	case DL_DISCON11_PENDING:	return ("DL_DISCON11_PENDING");
	case DL_DISCON12_PENDING:	return ("DL_DISCON12_PENDING");
	case DL_DISCON13_PENDING:	return ("DL_DISCON13_PENDING");
	case DL_SUBS_BIND_PND:		return ("DL_SUBS_BIND_PND");
	case DL_SUBS_UNBIND_PND:	return ("DL_SUBS_UNBIND_PND");
	default:			return ("DL_???");
	/* *INDENT-ON* */
	}
}

/* error names */
STATIC const char *
dl_error_name(dl_long error)
{
	switch (error) {
	/* *INDENT-OFF* */
	case DL_ACCESS:		return ("DL_ACCESS");
	case DL_BADADDR:	return ("DL_BADADDR");
	case DL_BADCORR:	return ("DL_BADCORR");
	case DL_BADDATA:	return ("DL_BADDATA");
	case DL_BADPPA:		return ("DL_BADPPA");
	case DL_BADPRIM:	return ("DL_BADPRIM");
	case DL_BADQOSPARAM:	return ("DL_BADQOSPARAM");
	case DL_BADQOSTYPE:	return ("DL_BADQOSTYPE");
	case DL_BADSAP:		return ("DL_BADSAP");
	case DL_BADTOKEN:	return ("DL_BADTOKEN");
	case DL_BOUND:		return ("DL_BOUND");
	case DL_INITFAILED:	return ("DL_INITFAILED");
	case DL_NOADDR:		return ("DL_NOADDR");
	case DL_NOTINIT:	return ("DL_NOTINIT");
	case DL_OUTSTATE:	return ("DL_OUTSTATE");
	case DL_SYSERR:		return ("DL_SYSERR");
	case DL_UNSUPPORTED:	return ("DL_UNSUPPORTED");
	case DL_UNDELIVERABLE:	return ("DL_UNDELIVERABLE");
	case DL_NOTSUPPORTED:	return ("DL_NOTSUPPORTED");
	case DL_TOOMANY:	return ("DL_TOOMANY");
	case DL_NOTENAB:	return ("DL_NOTENAB");
	case DL_BUSY:		return ("DL_BUSY");
	case DL_NOAUTO:		return ("DL_NOAUTO");
	case DL_NOXIDAUTO:	return ("DL_NOXIDAUTO");
	case DL_NOTESTAUTO:	return ("DL_NOTESTAUTO");
	case DL_XIDAUTO:	return ("DL_XIDAUTO");
	case DL_TESTAUTO:	return ("DL_TESTAUTO");
	case DL_PENDING:	return ("DL_PENDING");
	case -ENOBUFS:		return ("ENOBUFS");
	case -ENOMEM:		return ("ENOMEM");
	case -EBUSY:		return ("EBUSY");
	case -EAGAIN:		return ("EAGAIN");
	case -EOPNOTSUPP:	return ("EOPNOTSUPP");
	case -EINVAL:		return ("EINVAL");
	case -EFAULT:		return ("EFAULT");
	case -EPROTO:		return ("EPROTO");
	default:		return "(unknown)";
	/* *INDENT-ON* */
	}
}

/* error strings */
STATIC const char *
dl_error_string(dl_long error)
{
	switch (error) {
	/* *INDENT-OFF* */
	case DL_ACCESS:		return ("improper permissions for request");
	case DL_BADADDR:	return ("dlsap addr in improper format or invalid");
	case DL_BADCORR:	return ("seq number not from outstand dl_conn_ind");
	case DL_BADDATA:	return ("user data exceeded provider limit");
	case DL_BADPPA:		return ("specified ppa was invalid");
	case DL_BADPRIM:	return ("primitive received not known by provider");
	case DL_BADQOSPARAM:	return ("qos parameters contained invalid values");
	case DL_BADQOSTYPE:	return ("qos structure type is unknown/unsupported");
	case DL_BADSAP:		return ("bad lsap selector");
	case DL_BADTOKEN:	return ("token used not an active stream");
	case DL_BOUND:		return ("attempted second bind with dl_max_conind");
	case DL_INITFAILED:	return ("physical link initialization failed");
	case DL_NOADDR:		return ("provider couldn't allocate alt. address");
	case DL_NOTINIT:	return ("physical link not initialized");
	case DL_OUTSTATE:	return ("primitive issued in improper state");
	case DL_SYSERR:		return ("unix system error occurred");
	case DL_UNSUPPORTED:	return ("requested serv. not supplied by provider");
	case DL_UNDELIVERABLE:	return ("previous data unit could not be delivered");
	case DL_NOTSUPPORTED:	return ("primitive is known but not supported");
	case DL_TOOMANY:	return ("limit exceeded");
	case DL_NOTENAB:	return ("promiscuous mode not enabled");
	case DL_BUSY:		return ("other streams for ppa in post-attached state");
	case DL_NOAUTO:		return ("automatic handling of xid and test response not supported.");
	case DL_NOXIDAUTO:	return ("automatic handling of xid not supported");
	case DL_NOTESTAUTO:	return ("automatic handling of test not supported");
	case DL_XIDAUTO:	return ("automatic handling of xid response");
	case DL_TESTAUTO:	return ("automatic handling of test response");
	case DL_PENDING:	return ("pending outstanding connect indications");
	case -ENOBUFS:		return ("message block allocation would block");
	case -ENOMEM:		return ("memory allocation would block");
	case -EAGAIN:		return ("temporary failure, retry");
	case -EBUSY:		return ("flow control encountered");
	case -EOPNOTSUPP:	return ("primitive or operation not supported");
	case -EINVAL:		return ("invalid request");
	case -EFAULT:		return ("software error");
	case -EPROTO:		return ("protocol error");
	default:		return "(unknown)";
	/* *INDENT-ON* */
	}
}

/* State transitions */

STATIC INLINE fastcall dl_ulong
dl_get_state(struct dl *dl)
{
	return (dl->info.dl_current_state);
}

STATIC fastcall void
dl_set_state(struct dl *dl, dl_ulong state)
{
	strlog(DRV_ID, dl->u.dev.cminor, DL_LOG_DLS_STATE, SL_TRACE,
	       "state transition %s -> %s", dl_state_name(dl->info.dl_current_state),
	       dl_state_name(state));
	dl->info.dl_current_state = state;
}

/*
 *  DL-PRIMITIVES SENT UPSTREAM
 *  ==========================
 */
STATIC int
dl_m_error(queue_t *q, dl_ulong prim, int error)
{
	mblk_t *mp;
	int err;

	switch (error) {
	case QR_DONE:
	case -ENOBUFS:
	case -ENOMEM:
	case -EAGAIN:
	case -EBUSY:
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLS_ERROR, SL_TRACE,
		       "%s: %s: %s: %s", dl_prim_name(prim),
		       dl_state_name(dl_get_state(DL_PRIV(q))),
		       dl_error_name(error), dl_error_string(error));
		return (error);
	case -EFAULT:
	case -EPROTO:
	case -EMSGSIZE:
		err = EPROTO;
		break;
	default:
		err = error < 0 ? -error : error;
		break;
	}
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*mp->b_wptr++ = err;
		*mp->b_wptr++ = err;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLS_ERROR, SL_TRACE,
		       "%s: %s: %s: %s", dl_prim_name(prim),
		       dl_state_name(dl_get_state(DL_PRIV(q))),
		       dl_error_name(error), dl_error_string(error));
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

STATIC int
dl_m_hangup(queue_t *q, int error)
{
	mblk_t *mp;

	if ((mp = ss7_allocb(q, 0, BPRI_MED))) {
		mp->b_datap->db_type = M_HANGUP;
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

STATIC int
dl_info_ack(queue_t *q)
{
	dl_info_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_INFO_ACK;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_INFO_ACK");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * dl_bind_ack - bind an NS user acknowledgement
 * @q: write queue
 * @dl_sap: validated partial DLSAP 
 * @dl_addr: a pointer to the complete DLSAP address
 * @dl_addr_length: the length of the complete DLSAP address
 *
 * Complete DLSAP addresses have the following form:
 *
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   -reserved-  |  IP Protocol  |      Protocol Port Number     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                        IPv4 IP Address                        |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   \                                                               \
 *   /                              ...                              /
 *   \                                                               \
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |                        IPv4 IP Address                        |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * The protocol port number is provided in network byte order.  IPv4 addresses are provided in
 * network byte order.  The first 4 bytes are also present in the dl_sap member.
 *
 * Determining the IPv4 Addresses is performed as follows: all addresses for the assigned PPA (or
 * wildcard PPA) will provided.  For PPA 0 this is all IPv4 addresses assigned to the host.  For a
 * specific PPA `n', this is all the IPv4 addresses assigned to network interface `n'.  This could
 * be indicated by returning no IPv4 addresses or IPv4 address 0.0.0.0 when PPA is zero, and a
 * specific list when PPA is non-zero.  DL_SUBS_BIND_REQ can be used with a hierarchal bind to
 * further restrict the addresses if necessary.
 *
 * Another approach is to assign zero IP addresses and require a peer subsequent bind to
 * establish additional IP addresses.
 *
 * There are three ways to actually perform the bind to an IP protocol: provide a netfilter hook,
 * register the IP protocol with register_inet_protocol(), or register a packet handler.  Those
 * three approaches are discussed here:
 *
 * netfilter hooks: a netfilter hook can be provided that will cause IP datagrams to be passed
 * through the hook (filtering on incoming) and will permit the packet to be stolen by the stream.
 * One difficulty with this is that the netfilter is a configuration option and may not be
 * implemented on all kernels (e.g. no need for firewall).
 *
 * inet_protocol: a ip network protocol can be registered using reigster_inet_protocol and friends.
 * A difficulty with this approach is that, while one can register multiple protocol stacks against
 * a 2.4 kernel, a 2.6 kernel will burp.  Also, a 2.4 kernel registers behind the already installed
 * protocol instead of in front of it.  Some symbol ripping hacks are possible, but dangerous.
 *
 * packet handler: a packet handler for IP packets can be registered with dev_add_pack() and
 * friends.  The packet will be delivered, and on both 2.4 and 2.6 kernels, a shared sk_buff will be
 * provided that can be manipulated to "steal" the IP packet.  The problem here is that none of the
 * ip stack has run, so it is necessary to do one's own SAR, packet linearization, netfilter hooks,
 * and other policies (if desired).  Shared sk_buffs that are not altered to steal them will still
 * be processed through the stacks as usual.
 *
 * For the purposes of this device, the latter seems like the best policy.  When a packet arrives to
 * the packet handler, we perform the following:
 *
 * 1. if it is not ipv4, ignore the packet
 * 2. if the protocol is not in the bound protocols hash, ignore the packet
 * 3. if no non-zero port number is bound for the protocol, or if the port number (using UDP header
 *    layout) is not bound for the protocol, ignore the packet
 * 4. if the address does not match any bound to the port number, ignore the packet
 * 5. otherwise, munge the sk_buff so IP will not deliver it (set skb->pkg_type to
 *    PACKET_OTHERHOST) and deliver it up on the bound stream.
 *
 */
STATIC int
dl_bind_ack(queue_t *q, dl_ulong dl_sap, caddr_t dl_addr, size_t dl_addr_length)
{
	dl_bind_ack_t *p;
	mblk_t *mp = NULL;
	int err = -ENOBUFS;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_BIND_ACK;
		p->dl_sap = dl_sap;
		p->dl_addr_length = dl_addr_length;
		p->dl_addr_offset = dl_addr_length ? sizeof(*p) : 0;
		p->dl_max_conind = 0;
		p->dl_xidtest_flg = 0;
		if (dl_addr_length) {
			bcopy(dl_addr, mp->b_wptr, dl_addr_length);
			mp->b_wptr += dl_addr_length;
		}
		/* Okay, this is where we really need to bind.  About the only error that can crop
		   up here is the fact that the protocol and port is already bound for the
		   interface.  When binding for connectionless service, the effect of a successful
		   bind operation is that traffic begins being delivered immediately. */
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_BIND_ACK");
		dl_set_state(DL_PRIV(q), DL_IDLE);
		qreply(WR(q), mp);
		return (QR_DONE);
	}
      error:
	if (mp != NULL)
		freemsg(mp);
	return (err);
      bound:
	err = DL_BOUND;
	goto free_error;
      free_error:
	freemsg(mp);
	return (err);
}
STATIC int
dl_ok_ack(queue_t *q, const dl_ulong dl_correct_primitive, struct dl *ap, mblk_t *cp)
{
	struct dl *dl = DL_PRIV(dl);
	dl_ok_ack_t *p;
	mblk_t *mp;
	int err = -EFAULT;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_OK_ACK;
		p->dl_correct_primitive = dl_correct_primitive;
		switch (dl_get_state(dl)) {
		case DL_ATTACH_PENDING:	/* Waiting ack of DL_ATTACH_REQ */
			if ((err = dl_attach(dl, dl->dl_ppa)))
				goto free_error;
			dlpi_set_state(dl, DL_UNBOUND);
			break;
		case DL_DETACH_PENDING:	/* Waiting ack of DL_DETACH_REQ */
			if ((err = dl_detach(dl)))
				goto free_error;
			dlpi_set_state(dl, DL_UNATTACHED);
			break;
		case DL_BIND_PENDING:	/* Waiting ack of DL_BIND_REQ */
			swerr();	/* should be DL_BIND_ACK */
			break;
		case DL_UNBIND_PENDING:	/* Waiting ack of DL_UNBIND_REQ */
			dlpi_set_state(dl, DL_UNBOUND);
			break;
		case DL_UDQOS_PENDING:	/* Waiting ack of DL_UDQOS_REQ */
			dlpi_set_state(dl, DL_IDLE);
			break;
		case DL_OUTCON_PENDING:	/* awaiting DL_CONN_CON */
		case DL_INCON_PENDING:	/* awaiting DL_CONN_RES */
			swerr();
			break;
		case DL_CONN_RES_PENDING:	/* Waiting ack of DL_CONNECT_RES */
			if (--dl->dl_outstanding_coninds > 0)
				dlpi_set_state(dl, DL_INCON_PENDING);
			else
				dlpi_set_state(dl, DL_IDLE);
			break;
		case DL_USER_RESET_PENDING:	/* awaiting DL_RESET_CON */
		case DL_PROV_RESET_PENDING:	/* awaiting DL_RESET_RES */
			swerr();
			break;
		case DL_RESET_RES_PENDING:	/* Waiting ack of DL_RESET_RES */
			dlpi_set_state(dl, DL_DATAXFER);
			break;
		case DL_DISCON8_PENDING:	/* Waiting ack of DL_DISC_REQ from
						   DL_OUTCON_PENDING */
			dlpi_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON9_PENDING:	/* Waiting ack of DL_DISC_REQ from DL_INCON_PENDING 
						 */
			if (--dl->dl_outstanding_coninds > 0)
				dlpi_set_state(dl, DL_INCON_PENDING);
			else
				dlpi_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON11_PENDING:	/* Waiting ack of DL_DISC_REQ from DL_ DATAXFER */
			dlpi_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON12_PENDING:	/* Waiting ack of DL_DISC_REQ from
						   DL_USER_RESET_PENDING */
			dlpi_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON13_PENDING:	/* Waiting ack of DL_DISC_REQ from
						   DL_PROV_RESET_PENDING */
			dlpi_set_state(dl, DL_IDLE);
			break;
		case DL_SUBS_BIND_PND:	/* Waiting ack of DL_SUBS_BIND_REQ */
			swerr();	/* should be DL_SUBS_BIND_ACK */
			break;
		case DL_SUBS_UNBIND_PND:	/* Waiting ack of DL_SUBS_UNBIND_REQ */
			dlpi_set_state(dl, DL_IDLE);
			break;

		case DL_UNATTACHED:	/* PPA not attached */
		case DL_UNBOUND:	/* PPA attached */
		case DL_IDLE:	/* dlsap bound, awaiting use */
		case DL_DATAXFER:	/* connection-oriented data transfer */
			/* If not in a pending state, then this is probably an acknowledged
			   primitive that does not have a pending state (can be issued from any
			   state), such as DL_PROMISCON_REQ.  Just stay in the same state and
			   acknowledge the primitive. */
			break;
		}
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_OK_ACK");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_error_ack(queue_t *q, dl_ulong dl_error_primitive, dl_long error)
{
	struct dl *dl = DL_PRIV(q);
	dl_error_ack_t *p;
	mblk_t *mp;

	/* Note: don't ever call this with a positive queue return code other than QR_DONE (which
	   is really zero), because those codes overlap with provider specific error codes.  The
	   ones to really watch out for are QR_ABSORBED and QR_STRIP. */
	switch (err) {
	case QR_DONE:		/* this is zero and, therefore, cannot overlap with an error */
	case -ENOBUFS:
	case -ENOMEM:
	case -EAGAIN:
	case -EBUSY:
		/* These do not generate an error ack, but get returned directly */
		return (error);
	case -EPROTO:
	case -EMSGSIZE:
	case -EFAULT:
		break;
	}
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_ERROR_ACK;
		p->dl_error_primitive = dl_error_primitive;
		p->dl_errno = error > 0 ? error : 0;
		p->dl_unix_errno = error < 0 ? -error : 0;
		switch (dl_get_state(dl)) {
		case DL_UNATTACHED:	/* PPA not attached */
		case DL_ATTACH_PENDING:	/* Waiting ack of DL_ATTACH_REQ */
			dl_set_state(dl, DL_UNATTACHED);
			break;
		case DL_DETACH_PENDING:	/* Waiting ack of DL_DETACH_REQ */
		case DL_UNBOUND:	/* PPA attached */
		case DL_BIND_PENDING:	/* Waiting ack of DL_BIND_REQ */
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_UNBIND_PENDING:	/* Waiting ack of DL_UNBIND_REQ */
		case DL_SUBS_BIND_PND:	/* Waiting ack of DL_SUBS_BIND_REQ */
		case DL_SUBS_UNBIND_PND:	/* Waiting ack of DL_SUBS_UNBIND_REQ */
		case DL_IDLE:	/* dlsap bound, awaiting use */
		case DL_UDQOS_PENDING:	/* Waiting ack of DL_UDQOS_REQ */
		case DL_OUTCON_PENDING:	/* awaiting DL_CONN_CON */
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_INCON_PENDING:	/* awaiting DL_CONN_RES */
		case DL_CONN_RES_PENDING:	/* Waiting ack of DL_CONNECT_RES */
		case DL_DISCON9_PENDING:	/* Waiting ack of DL_DISC_REQ (DL_INCON_PENDING) */
			dl_set_state(dl, DL_INCON_PENDING);
			break;
		case DL_DISCON8_PENDING:	/* Waiting ack of DL_DISC_REQ (DL_OUTCON_PENDING) */
			dl_set_state(dl, DL_OUTCON_PENDING);
			break;
		case DL_DATAXFER:	/* connection-oriented data transfer */
		case DL_USER_RESET_PENDING:	/* awaiting DL_RESET_CON */
		case DL_DISCON11_PENDING:	/* Waiting ack of DL_DISC_REQ (DL_DATA_XFER) */
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_DISCON12_PENDING:	/* Waiting ack of DL_DISC_REQ
						   (DL_USER_RESET_PENDING) */
			dl_set_state(dl, DL_USER_RESET_PENDING);
			break;
		case DL_PROV_RESET_PENDING:	/* awaiting DL_RESET_RES */
		case DL_RESET_RES_PENDING:	/* Waiting ack of DL_RESET_RES */
		case DL_DISCON13_PENDING:	/* Waiting ack of DL_DISC_REQ
						   (DL_PROV_RESET_PENDING) */
			dl_set_state(dl, DL_PROV_RESET_PENDING);
			break;
		default:
			/* If the interface is not in a pending state, remain in the same state. */
			break;
		}
		strlog(DRV_ID, dl->u.dev.cminor, DL_LOG_DLS_ERROR, SL_TRACE,
		       "%s: %s: %s: %s",
		       dl_prim_name(dl_error_primitive),
		       dl_get_state(dl), dl_error_name(error), dl_error_string(error));
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_ERROR_ACK");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_subs_bind_ack(queue_t *q)
{
	dl_subs_bind_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_SUBS_BIND_ACK;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_SUBS_BIND_ACK");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_unitdata_ind(queue_t *q)
{
	dl_unitdata_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_UNITDATA_IND;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_UNITDATA_IND");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_uderror_ind(queue_t *q)
{
	dl_uderror_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_UDERROR_IND;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_UDERROR_IND");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_connect_ind(queue_t *q)
{
	dl_connect_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_CONNECT_IND;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_CONNECT_IND");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_connect_con(queue_t *q)
{
	dl_connect_con_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_CONNECT_CON;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_CONNECT_CON");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_token_ack(queue_t *q)
{
	dl_token_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_TOKEN_ACK;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_TOKEN_ACK");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_disconnect_ind(queue_t *q)
{
	dl_disconnect_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_DISCONNECT_IND;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_DISCONNECT_IND");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_reset_ind(queue_t *q)
{
	dl_reset_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_RESET_IND;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_RESET_IND");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_reset_con(queue_t *q)
{
	dl_reset_con_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_RESET_CON;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_RESET_CON");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_data_ack_ind(queue_t *q)
{
	dl_data_ack_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_DATA_ACK_IND;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_DATA_ACK_IND");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_data_ack_status_ind(queue_t *q)
{
	dl_data_ack_status_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_DATA_ACK_STATUS_IND;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_DATA_ACK_STATUS");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_reply_ind(queue_t *q)
{
	dl_reply_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_REPLY_IND;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_REPLY_IND");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_reply_status_ind(queue_t *q)
{
	dl_reply_status_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_REPLY_STATUS_IND;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_REPLY_STATUS_IND");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_reply_update_status_ind(queue_t *q)
{
	dl_reply_update_status_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_REPLY_UPDATE_STATUS_IND;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_REPLY_UPDATE_STATUS_IND");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_xid_ind(queue_t *q)
{
	dl_xid_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_XID_IND;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_XID_IND");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_xid_con(queue_t *q)
{
	dl_xid_con_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_XID_CON;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_XID_CON");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_test_ind(queue_t *q)
{
	dl_test_ind_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_TEST_IND;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_TEST_IND");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_test_con(queue_t *q)
{
	dl_test_con_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_TEST_CON;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_TEST_CON");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_phys_addr_ack(queue_t *q)
{
	dl_phys_addr_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_PHYS_ADDR_ACK;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_PHYS_ADDR_ACK");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}
STATIC int
dl_get_statistics_ack(queue_t *q)
{
	dl_get_statistics_ack_t *p;
	mblk_t *mp;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->dl_primitive = DL_GET_STATISTICS_ACK;
		strlog(DRV_ID, DL_PRIV(q)->u.dev.cminor, DL_LOG_DLSP_PRIM, SL_TRACE,
		       "<- DL_GET_STATISTICS_ACK");
		qreply(WR(q), mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/*
 *  DL-PRIMITIVES RECEIVED FROM UPSTREAM
 *  ===================================
 */
/**
 * dl_info_req - provide NS provider information
 * @q: write queue
 * @mp: the DL_INFO_REQ message
 *
 * For this provider, the mac type returned is always DL_IP.
 */
STATIC int
dl_info_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_info_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	{
		if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
			goto emsgsize;
		if (p->dl_primitive != DL_INFO_REQ)
			goto efault;
		if ((err = dl_info_ack(q)) != QR_DONE)
			goto error;
	}
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_INFO_REQ, err);
}

/**
 * dl_attach_req - attache to a Physical Point of Attachment
 * @q: write queue
 * @mp: the DL_ATTACH_REQ message
 *
 * All the streams provided by this device are Style2 streams.  The PPA (if non-zero) attaches the
 * stream to a specific network interface identifier.  Otherwise, it is effectively attached to all
 * network interfaces under all encapsulation techniques.
 */
STATIC int
dl_attach_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_attach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (dl_get_state(dl) != DL_ATTACH_PENDING) {
		if (dl_get_style(dl) != DL_STYLE2)
			goto outstate;
		if (dl_get_state(dl) != DL_UNATTACHED)
			goto outstate;
		if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
			goto emsgsize;
		if (p->dl_primitive != DL_ATTACH_REQ)
			goto efault;
		dl_set_state(dl, DL_ATTACH_PENDING);
	}
	return (dl_ok_ack(q, DL_ATTACH_REQ, NULL, NULL));
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_ATTACH_REQ, err);
}
STATIC int
dl_detach_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_detach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (dl_get_state(dl) != DL_DETACH_PENDING) {
		if (dl_get_style(dl) != DL_STYLE2)
			goto outstate;
		if (dl_get_state(dl) != DL_UNBOUND)
			goto outstate;
		if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
			goto emsgsize;
		if (p->dl_primitive != DL_DETACH_REQ)
			goto efault;
		dl_set_state(dl, DL_DETACH_PENDING);
	}
	return (dl_ok_ack(q, DL_DETACH_REQ, NULL, NULL));
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_DETACH_REQ, err);
}

/**
 * dl_bind_req - bind to a DLSAP with options
 * @q: write queue
 * @mp: the DL_BIND_REQ message
 *
 * The DLSAP is the 8-bit IP protocol number.  It might also be possible to support IP protocol
 * number 0 which would indicate a wildcard of all IP protocols, but that would be a little crazy as
 * it would likely shut down all linux networking, so we don't allow it for now.  Well, guess what,
 * zero is assigned!  Because we don't want to bind to an entire protocol unless desired, the DLSAP
 * will also contain a port number.  If the port number portion of the DLSAP is zero, then the
 * entire protocol will be bound initially.  If the PPA is non-zero, only the corresponding attached
 * network device is bound.  So the DLSAP is as follows:
 *
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   |   -reserved-  |  IP Protocol  |      Protocol Port Number     |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * To protect against unintended breakage, binding of TCP/UDP/SCTP to a wildcard port number will
 * return an [DL_BOUND] error.  The protocol port number is provided in network byte order.
 */
STATIC int
dl_bind_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	unsigned char protocol;
	unsigned short port;

	if (dl_get_state(dl) != DL_BIND_PENDING) {
		/* may of these things are particular to this provider */
		if (dl_get_state(dl) == DL_UNATTACHED)
			goto notinit;
		if (dl_get_state(dl) != DL_UNBOUND)
			goto outstate;
		if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
			goto emsgsize;
		if (p->dl_primitive != DL_BIND_REQ)
			goto efault;
		if (p->dl_service_mode != DL_CLDLS)
			goto unsupported;
		if ((p->dl_xidtest_flg & (DL_AUTO_XID | DL_AUTO_TEST)) ==
		    (DL_AUTO_XID | DL_AUTO_TEST))
			goto noauto;
		if (p->dl_xidtest_flg & DL_AUTO_XID)
			goto noxidauto;
		if (p->dl_xidtest_flg & DL_AUTO_TEST)
			goto notestauto;
		protocol = (uint) (p->dl_sap >> 16);
		port = (uint) (p->dl_sap);
		if (protocol > 254)
			goto badaddr;
		switch (protocol) {
		case IPPROTO_UDP:
		case IPPROTO_TCP:
		case IPPROTO_SCTP:
			if (port == 0)
				goto badaddr;
		}
		/* FIXME: we should check permissions for binding to ports beneath 1024 and all but
		   specific protocols. */
		dl_set_state(dl, DL_BIND_PENDING);
	}
	if ((err = dl_bind_ack(q, p->dl_sap, (caddr_t) &p->dl_sap, sizeof(p->dl_sap))) != QR_DONE)
		goto error;
	return (QR_DONE);
      badaddr:
	err = DL_BADADDR;
	goto error;
      notestauto:
	err = DL_NOAUTO;
	goto error;
      noxidauto:
	err = DL_NOXIDAUTO;
	goto error;
      noauto:
	err = DL_NOAUTO;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      notinit:
	err = DL_NOTINIT;
	goto error;
      error:
	return dl_error_ack(dl, DL_BIND_REQ, err);
}
STATIC int
dl_unbind_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_UNBIND_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_UNBIND_REQ, err);
}
STATIC int
dl_subs_bind_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_subs_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_SUBS_BIND_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_SUBS_BIND_REQ, err);
}
STATIC int
dl_subs_unbind_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_subs_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_SUBS_UNBIND_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_SUBS_UNBIND_REQ, err);
}
STATIC int
dl_enabmulti_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_enabmulti_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_ENABMULTI_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_ENABMULTI_REQ, err);
}
STATIC int
dl_disabmulti_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_disabmulti_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_DISABMULTI_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_DISABMULTI_REQ, err);
}
STATIC int
dl_promiscon_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_promiscon_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_PROMISCON_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_PROMISCON_REQ, err);
}
STATIC int
dl_promiscoff_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_promiscoff_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_PROMISCOFF_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_PROMISCOFF_REQ, err);
}
STATIC int
dl_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_UNITDATA_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ind(dl, DL_UNITDATA_REQ, err);
}
STATIC int
dl_udqos_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_udqos_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_UDQOS_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_UDQOS_REQ, err);
}
STATIC int
dl_connect_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_connect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_CONNECT_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_CONNECT_REQ, err);
}
STATIC int
dl_connect_res(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_connect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_CONNECT_RES)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_CONNECT_RES, err);
}
STATIC int
dl_token_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_token_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_TOKEN_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_TOKEN_REQ, err);
}
STATIC int
dl_disconnect_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_disconnect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_DISCONNECT_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_DISCONNECT_REQ, err);
}
STATIC int
dl_reset_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_reset_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_RESET_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_RESET_REQ, err);
}
STATIC int
dl_reset_res(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_reset_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_RESET_RES)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_RESET_RES, err);
}
STATIC int
dl_data_ack_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_data_ack_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_DATA_ACK_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_DATA_ACK_REQ, err);
}
STATIC int
dl_reply_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_reply_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_REPLY_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_REPLY_REQ, err);
}
STATIC int
dl_reply_update_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_reply_update_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_REPLY_UPDATE_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_REPLY_UPDATE_REQ, err);
}
STATIC int
dl_xid_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_xid_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_XID_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_XID_REQ, err);
}
STATIC int
dl_xid_res(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_xid_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_XID_RES)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_XID_RES, err);
}
STATIC int
dl_test_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_test_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_TEST_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_TEST_REQ, err);
}
STATIC int
dl_test_res(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_test_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_TEST_RES)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_TEST_RES, err);
}
STATIC int
dl_phys_addr_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_phys_addr_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_PHYS_ADDR_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_PHYS_ADDR_REQ, err);
}
STATIC int
dl_set_phys_addr_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_set_phys_addr_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_SET_PHYS_ADDR_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_SET_PHYS_ADDR_REQ, err);
}
STATIC int
dl_get_statistics_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_get_statistics_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if ((mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto emsgsize;
	if (p->dl_primitive != DL_GET_STATISTICS_REQ)
		goto efault;
	return (QR_DONE);
      efault:
	err = -EFAULT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_ack(dl, DL_GET_STATISTICS_REQ, err);
}

STATIC INLINE fastcall __hot_put int
dl_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	dl_long prim = 0;
	struct dl *dl = DL_PRIV(q);
	dl_long oldstate = dlpi_get_state(dl);

	if (mp->b_wptr >= mp->b_rptr + sizeof(prim)) {
		prim = *(dl_long *) mp->b_rptr;
		strlog(DRV_ID, dl->u.dev.cminor, DL_LOG_DLSU_PRIM, SL_TRACE, "%s ->",
		       dl_prim_name(prim));
		switch (prim) {
		case DL_INFO_REQ:	/* Information Req */
			rtn = dl_info_req(q, mp);
			break;
		case DL_ATTACH_REQ:	/* Attach a PPA */
			rtn = dl_attach_req(q, mp);
			break;
		case DL_DETACH_REQ:	/* Detach a PPA */
			rtn = dl_detach_req(q, mp);
			break;
		case DL_BIND_REQ:	/* Bind dlsap address */
			rtn = dl_bind_req(q, mp);
			break;
		case DL_UNBIND_REQ:	/* Unbind dlsap address */
			rtn = dl_unbind_req(q, mp);
			break;
		case DL_SUBS_BIND_REQ:	/* Bind Subsequent DLSAP address */
			rtn = dl_subs_bind_req(q, mp);
			break;
		case DL_SUBS_UNBIND_REQ:	/* Subsequent unbind */
			rtn = dl_subs_unbind_req(q, mp);
			break;
		case DL_ENABMULTI_REQ:	/* Enable multicast addresses */
			rtn = dl_enabmulti_req(q, mp);
			break;
		case DL_DISABMULTI_REQ:	/* Disable multicast addresses */
			rtn = dl_disabmulti_req(q, mp);
			break;
		case DL_PROMISCON_REQ:	/* Turn on promiscuous mode */
			rtn = dl_promiscon_req(q, mp);
			break;
		case DL_PROMISCOFF_REQ:	/* Turn off promiscuous mode */
			rtn = dl_promiscoff_req(q, mp);
			break;
		case DL_UNITDATA_REQ:	/* datagram send request */
			rtn = dl_unitdata_req(q, mp);
			break;
		case DL_UDQOS_REQ:	/* set QOS for subsequent datagrams */
			rtn = dl_udqos_req(q, mp);
			break;
		case DL_CONNECT_REQ:	/* Connect request */
			rtn = dl_connect_req(q, mp);
			break;
		case DL_CONNECT_RES:	/* Accept previous connect indication */
			rtn = dl_connect_res(q, mp);
			break;
		case DL_TOKEN_REQ:	/* Passoff token request */
			rtn = dl_token_req(q, mp);
			break;
		case DL_DISCONNECT_REQ:	/* Disconnect request */
			rtn = dl_disconnect_req(q, mp);
			break;
		case DL_RESET_REQ:	/* Reset service request */
			rtn = dl_reset_req(q, mp);
			break;
		case DL_RESET_RES:	/* Complete reset processing */
			rtn = dl_reset_res(q, mp);
			break;
		case DL_DATA_ACK_REQ:	/* data unit transmission request */
			rtn = dl_data_ack_req(q, mp);
			break;
		case DL_REPLY_REQ:	/* Request a DLSDU from the remote */
			rtn = dl_reply_req(q, mp);
			break;
		case DL_REPLY_UPDATE_REQ:	/* Hold a DLSDU for transmission */
			rtn = dl_reply_update_req(q, mp);
			break;
		case DL_XID_REQ:	/* Request to send an XID PDU */
			rtn = dl_xid_req(q, mp);
			break;
		case DL_XID_RES:	/* request to send a response XID PDU */
			rtn = dl_xid_res(q, mp);
			break;
		case DL_TEST_REQ:	/* TEST command request */
			rtn = dl_test_req(q, mp);
			break;
		case DL_TEST_RES:	/* TEST response */
			rtn = dl_test_res(q, mp);
			break;
		case DL_PHYS_ADDR_REQ:	/* Request to get physical addr */
			rtn = dl_phys_addr_req(q, mp);
			break;
		case DL_SET_PHYS_ADDR_REQ:	/* set physical addr */
			rtn = dl_set_phys_addr_req(q, mp);
			break;
		case DL_GET_STATISTICS_REQ:	/* Request to get statistics */
			rtn = dl_get_statistics_req(q, mp);
			break;

		case DL_INFO_ACK:	/* Information Ack */
		case DL_BIND_ACK:	/* Dlsap address bound */
		case DL_OK_ACK:	/* Success acknowledgment */
		case DL_ERROR_ACK:	/* Error acknowledgment */
		case DL_SUBS_BIND_ACK:	/* Subsequent DLSAP address bound */
		case DL_UNITDATA_IND:	/* datagram receive indication */
		case DL_UDERROR_IND:	/* datagram error indication */
		case DL_CONNECT_IND:	/* Incoming connect indication */
		case DL_CONNECT_CON:	/* Connection established */
		case DL_TOKEN_ACK:	/* Passoff token ack */
		case DL_DISCONNECT_IND:	/* Disconnect indication */
		case DL_RESET_IND:	/* Incoming reset indication */
		case DL_RESET_CON:	/* Reset processing complete */
		case DL_DATA_ACK_IND:	/* Arrival of a command PDU */
		case DL_DATA_ACK_STATUS_IND:	/* Status indication of DATA_ACK_REQ */
		case DL_REPLY_IND:	/* Arrival of a command PDU */
		case DL_REPLY_STATUS_IND:	/* Status indication of REPLY_REQ */
		case DL_REPLY_UPDATE_STATUS_IND:	/* Status of REPLY_UPDATE req */
		case DL_XID_IND:	/* Arrival of an XID PDU */
		case DL_XID_CON:	/* Arrival of a response XID PDU */
		case DL_TEST_IND:	/* TEST response indication */
		case DL_TEST_CON:	/* TEST Confirmation */
		case DL_PHYS_ADDR_ACK:	/* Return physical addr */
		case DL_GET_STATISTICS_ACK:	/* Return statistics */
			/* wrong direction */
			rtn = DL_NOTSUPPORTED;
			break;
		default:
			/* unrecognized primitive */
			rtn = DL_BADPRIM;
			break;
		}
	}
	if (rtn < 0) {
		seldom();
		dlpi_set_state(dl, oldstate);
		/* The put and service procedure do not recognize all errors. Sometimes we return
		   an error to here just to restore the previous state. */
		switch (rtn) {
		case -EBUSY:	/* flow controlled */
		case -EAGAIN:	/* try again */
		case -ENOMEM:	/* could not allocate memory */
		case -ENOBUFS:	/* could not allocate an mblk */
			/* these are handled by the put and service procedures */
			break;
		case DL_BADPRIM:	/* primitive not supported */
		case DL_NOTSUPPORTED:	/* primitive not supported */
			/* these need a DL_ERROR_ACK */
			return dl_error_ack(q, prim, rtn);
		case -EPROTO:
		case -EFAULT:
			/* these need an M_ERROR */
			return dl_m_error(q, prim, rtn);
		default:
			/* ignore all other errors */
			rtn = 0;
			break;
		}
	}
	return (rtn);
}
STATIC int
dl_w_data(queue_t *q, mblk_t *mp)
{
}
STATIC int
dl_w_ctl(queue_t *q, mblk_t *mp)
{
}
STATIC int
dl_w_ioctl(queue_t *q, mblk_t *mp)
{
}

unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the IP driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the IP driver. (0 for allocation.)");


static struct cdevsw dl_cdev = {
	.d_name = DRV_NAME,
	.d_str = &dl_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

#ifdef LFS
static struct devnode dl_node_ip = {
	.n_name = "ip",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode dl_node_ipco = {
	.n_name = "ipco",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

static struct devnode dl_node_ipcl = {
	.n_name = "ipcl",
	.n_flag = D_CLONE,
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};
#endif				/* LFS */

static __init int
dlipinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	dl_init_hashes();
	if ((err = register_strdev(&dl_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME, major);
		dl_term_hashes();
		return (err);
	}
	if (major == 0)
		major = err;

#ifdef LFS
	register_strnod(&dl_cdev, &dl_node_ip, IP_CMINOR);
	register_strnod(&dl_cdev, &dl_node_ipco, IPCO_CMINOR);
	register_strnod(&dl_cdev, &dl_node_ipcl, IPCL_CMINOR);
#endif				/* LFS */

	return (0);
}

static __exit void
dlipexit(void)
{
	int err;

#ifdef LFS
	unregister_strnod(&dl_cdev, IP_CMINOR);
	unregister_strnod(&dl_cdev, IPCO_CMINOR);
	unregister_strnod(&dl_cdev, IPCL_CMINOR);
#endif				/* LFS */

	if ((err = unregister_strdev(&dl_cdev, major)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister major %d", DRV_NAME, major);
	dl_term_hashes();
	return;
}

module_init(dlipinit);
module_init(dlipexit);
