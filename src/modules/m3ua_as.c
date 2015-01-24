/*****************************************************************************

 @(#) File: src/modules/m3ua_as.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

 *****************************************************************************/

static char const ident[] = "src/modules/m3ua_as.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/*
 *  This is the AS side of M3UA implemented as a pushable module that pushes over an SCTP NPI
 *  stream.
 *
 *  The SCTP NPI stream can be a freshly opened stream (i.e. in the NS_UNBND state), or can be bound
 *  by the opener before pushing the M3UA-AS module, or can be connected by the opener before
 *  pushing the M3UA-AS module.  When the M3UA-AS module is pushed, it interrogates the state of the
 *  SCTP NPI stream beneath it and flushes the read queue (so that no messages get past the module).
 *
 *  When an MTP bind is performed, the SCTP stream will be bound (if it is not already bound).  If
 *  the SCTP stream is already connected, the bind operation initiates ASP Up procedure with the
 *  bound ASPID.
 *
 *  When an MTP bind is performed, the SCTP stream will be connected (if it is not already
 *  connected).  If the SCTP stream is already connected, and the ASP Up operation has completed, an
 *  ASP Active for the bound RC is initiated.  Because MTP streams are bound by address
 *  specification, the default behaviour of the AS is to register the address with the peer and
 *  obtain a Routing Context if one is not provided in the address.  An M3UA address differs
 *  slightly from that of MTP in that an NA and RC value are also provided in the address.  Once UP
 *  and registered, the routing context is activated.  All this occurs during the bind opertion for
 *  MTP.  (Note that if the SI value is pseudo-connection-oriented, such as ISUP, then an additional
 *  MTP connect operation is required before a routing key can be registered, a routing context
 *  obtained, and the AS activated.)
 *
 *  Once the interface is enabled, the M3UA-AS module audits the state of the MTP for the bound
 *  address by using the DAUD query.
 *
 *  At this point, MTP-primitmives can be issued by the MTP User that will be translated into M3UA
 *  messages for the activated RC.  M3UA messages received for the activated RC will be translated
 *  into MTP-primitives and sent upstream.
 *
 *  Unlike M2UA-AS, instead of a pushable module, M3UA-AS is better implemented as a multiplexing
 *  driver.  I will probably abandon this module.
 */

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#define _DEBUG 1
//#undef _DEBUG

#include <sys/os7/compat.h>
#include <sys/strsun.h>

#undef mi_timer
#define mi_timer mi_timer_MAC

#include <linux/socket.h>
#include <net/ip.h>

#include <sys/npi.h>
#include <sys/npi_sctp.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>

#include <sys/tihdr.h>
#include <sys/tpi_mtp.h>

#define MTPLOGST	1	/* log MTP state transitions */
#define MTPLOGTO	2	/* log MTP timeouts */
#define MTPLOGRX	3	/* log MTP primitives received */
#define MTPLOGTX	4	/* log MTP primitives issued */
#define MTPLOGTE	5	/* log MTP timer events */
#define MTPLOGDA	6	/* log MTP data */

/* ======================= */

#define M3UA_AS_DESCRIP		"M3UA/SCTP Message Transfer Part (MTP) STREAMS Module"
#define M3UA_AS_EXTRA		"Part of the OpenSS7 SS7 Stack for Linux Fast-STREAMS"
#define M3UA_AS_REVISION	"OpenSS7 src/modules/m3ua_as.c (" PACKAGE_ENVR ") " PACKAGE_DATE
#define M3UA_AS_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define M3UA_AS_DEVICE		"Part of the OpenSS7 Stack for Linux Fast STREAMS."
#define M3UA_AS_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define M3UA_AS_LICENSE		"GPL"
#define M3UA_AS_BANNER		M3UA_AS_DESCRIP		"\n" \
				M3UA_AS_EXTRA		"\n" \
				M3UA_AS_REVISION	"\n" \
				M3UA_AS_COPYRIGHT	"\n" \
				M3UA_AS_DEVICE		"\n" \
				M3UA_AS_CONTACT		"\n"
#define M3UA_AS_SPLASH		M3UA_AS_DEVICE		" - " \
				M3UA_AS_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(M3UA_AS_CONTACT);
MODULE_DESCRIPTION(M3UA_AS_DESCRIP);
MODULE_SUPPORTED_DEVICE(M3UA_AS_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(M3UA_AS_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-m3ua_as");
#endif
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif
#endif				/* LINUX */

#define M3UA_AS_MOD_ID		CONFIG_STREAMS_M3UA_AS_MODID
#define M3UA_AS_MOD_NAME	CONFIG_STREAMS_M3UA_AS_NAME

#define MOD_ID	    M3UA_AS_MOD_ID
#define MOD_NAME    M3UA_AS_MOD_NAME
#ifdef MODULE
#define MOD_SPLASH  M3UA_AS_BANNER
#else				/* MODULE */
#define MOD_SPLASH  M3UA_AS_SPLASH
#endif				/* MODULE */

static struct module_info mtp_minfo = {
	.mi_idnum = MOD_ID,	/* Module ID number */
	.mi_idname = MOD_NAME,	/* Module name */
	.mi_minpsz = STRMINPSZ,	/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,	/* Max packet size accepted */
	.mi_hiwat = STRHIGH,	/* Hi water mark */
	.mi_lowat = STRLOW,	/* Lo water mark */
};

static struct module_stat mtp_wstat __attribute__ ((aligned(SMP_CACHE_BYTES)));
static struct module_stat mtp_rstat __attribute__ ((aligned(SMP_CACHE_BYTES)));

struct mtp_state {
	int l_state;			/* LMI state */
	int n_state;			/* NPI state */
	int m_state;			/* MTP state */
	int a_state;			/* ASP state */
};

/*
 *  M3UA-AS Private Structure
 */
struct mtp {
	queue_t *rq;
	queue_t *wq;
	struct mtp_state curr, save;	/* current and saved state */
	uint32_t cong;
	uint32_t disc;
	struct mtp_addr orig;
	struct mtp_addr dest;
	mblk_t *rbuf;
	mblk_t *aspup_tack;
	mblk_t *aspac_tack;
	mblk_t *aspdn_tack;
	mblk_t *aspia_tack;
	mblk_t *hbeat_tack;
	mblk_t *rreq_tack;
	mblk_t *dreq_tack;
	int audit;
	uint32_t tm;			/* traffic mode */
	uint32_t rc;			/* routing context */
	uint32_t aspid;			/* ASP identifier */
	int coninds;			/* npi connection indications */
	struct {
		lmi_info_ack_t lmi;	/* LMI information */
		struct MTP_info_ack mtp;	/* MTP information */
		N_info_ack_t npi;	/* NPI information */
	} info;
	size_t loc_len;			/* local address length */
	size_t rem_len;			/* remote address length */
	struct sockaddr_in loc[8];	/* up to 8 local addresses */
	struct sockaddr_in rem[8];	/* up to 8 remote addresses */
};

#define MTP_PRIV(__q) ((struct mtp *)(__q)->q_ptr)

#if 0
#define NSF_UNBND	(1<<NS_UNBND)
#define NSF_WACK_BREQ	(1<<NS_WACK_BREQ)
#define NSF_WACK_UREQ	(1<<NS_WACK_UREQ)
#define NSF_IDLE	(1<<NS_IDLE)
#define NSF_WACK_OPTREQ	(1<<NS_WACK_OPTREQ)
#define NSF_WACK_RRES	(1<<NS_WACK_RRES)
#define NSF_WCON_CREQ	(1<<NS_WCON_CREQ)
#define NSF_WRES_CIND	(1<<NS_WRES_CIND)
#define NSF_WACK_CRES	(1<<NS_WACK_CRES)
#define NSF_DATA_XFER	(1<<NS_DATA_XFER)
#define NSF_WCON_RREQ	(1<<NS_WCON_RREQ)
#define NSF_WRES_RIND	(1<<NS_WRES_RIND)
#define NSF_WACK_DREQ6	(1<<NS_WACK_DREQ6)
#define NSF_WACK_DREQ7	(1<<NS_WACK_DREQ7)
#define NSF_WACK_DREQ9	(1<<NS_WACK_DREQ9)
#define NSF_WACK_DREQ10	(1<<NS_WACK_DREQ10)
#define NSF_WACK_DREQ11	(1<<NS_WACK_DREQ11)
#endif

/*
 * DEBUGGING
 * --------------------------------------------------------------------------
 */

#ifdef _OPTIMIZE_SPEED
#define STRLOG(priv, level, flags, fmt, ...) \
	do { } while (0)
#else
#define STRLOG(priv, level, flags, fmt, ...) \
	mi_strlog(priv->rq, level, flags, fmt, ##__VA_ARGS__)
#endif

#define STRLOGERR	0	/* log error information */
#define STRLOGNO	0	/* log notice information */
#define STRLOGST	1	/* log state transitions */
#define STRLOGTO	2	/* log timeouts */
#define STRLOGRX	3	/* log primitives received */
#define STRLOGTX	4	/* log primitives issued */
#define STRLOGTE	5	/* log timer events */
#define STRLOGIO	6	/* log additional data */
#define STRLOGDA	7	/* log data */

#if 1
#define LOGERR(priv, fmt, ...) STRLOG(priv, STRLOGERR, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGNO(priv, fmt, ...) STRLOG(priv, STRLOGNO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGST(priv, fmt, ...) STRLOG(priv, STRLOGST, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTO(priv, fmt, ...) STRLOG(priv, STRLOGTO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGRX(priv, fmt, ...) STRLOG(priv, STRLOGRX, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTX(priv, fmt, ...) STRLOG(priv, STRLOGTX, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGTE(priv, fmt, ...) STRLOG(priv, STRLOGTE, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGIO(priv, fmt, ...) STRLOG(priv, STRLOGIO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGDA(priv, fmt, ...) STRLOG(priv, STRLOGDA, SL_TRACE, fmt, ##__VA_ARGS__)
#else
#define LOGERR(priv, fmt, ...) STRLOG(priv, STRLOGERR, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGNO(priv, fmt, ...) STRLOG(priv, STRLOGNO, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGST(priv, fmt, ...) STRLOG(priv, STRLOGST, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTO(priv, fmt, ...) STRLOG(priv, STRLOGTO, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGRX(priv, fmt, ...) STRLOG(priv, STRLOGRX, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTX(priv, fmt, ...) STRLOG(priv, STRLOGTX, SL_TRACE | SL_ERROR | SL_CONSOLE, fmt, ##__VA_ARGS__)
#define LOGTE(priv, fmt, ...) STRLOG(priv, STRLOGTE, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGIO(priv, fmt, ...) STRLOG(priv, STRLOGIO, SL_TRACE, fmt, ##__VA_ARGS__)
#define LOGDA(priv, fmt, ...) STRLOG(priv, STRLOGDA, SL_TRACE, fmt, ##__VA_ARGS__)
#endif

#ifndef _OPTIMIZE_SPEED
static const char *
unix_errname(t_scalar_t err)
{
	switch (err) {
	case 0:
		return ("(success)");
	case EPROTO:
		return ("EPROTO");
	case EMSGSIZE:
		return ("EMSGSIZE");
	case EINVAL:
		return ("EINVAL");
	case EOPNOTSUPP:
		return ("EOPNOTSUPP");
	case EPERM:
		return ("EPERM");
	}
	return ("(unknown)");
}
#endif					/* _OPTIMIZE_SPEED */
static const char *
unix_errstring(t_scalar_t err)
{
	switch (err) {
	case 0:
		return ("No error");
	case EPROTO:
		return ("Protocol violation");
	case EMSGSIZE:
		return ("Bad message size");
	case EINVAL:
		return ("Invalid argument");
	case EOPNOTSUPP:
		return ("Operation not supported");
	case EPERM:
		return ("No permission");
	}
	return ("Unknown error code.");
}

#ifndef _OPTIMIZE_SPEED
static const char *
m_statename(mtp_long state)
{
	switch (state) {
	case MTPS_UNBND:
		return ("MTPS_UNBND");
	case MTPS_WACK_BREQ:
		return ("MTPS_WACK_BREQ");
	case MTPS_IDLE:
		return ("MTPS_IDLE");
	case MTPS_WACK_CREQ:
		return ("MTPS_WACK_CREQ");
	case MTPS_WCON_CREQ:
		return ("MTPS_WCON_CREQ");
	case MTPS_CONNECTED:
		return ("MTPS_CONNECTED");
	case MTPS_WACK_UREQ:
		return ("MTPS_WACK_UREQ");
	case MTPS_WACK_DREQ6:
		return ("MTPS_WACK_DREQ6");
	case MTPS_WACK_DREQ9:
		return ("MTPS_WACK_DREQ9");
	case MTPS_WACK_OPTREQ:
		return ("MTPS_WACK_OPTREQ");
	case MTPS_WREQ_ORDREL:
		return ("MTPS_WREQ_ORDREL");
	case MTPS_WIND_ORDREL:
		return ("MTPS_WIND_ORDREL");
	case MTPS_WRES_CIND:
		return ("MTPS_WRES_CIND");
	case MTPS_UNUSABLE:
		return ("MTPS_UNUSABLE");
	}
	return ("(unknown)");
}
static const char *
mtp_primname(mtp_long prim)
{
	switch (prim) {
	case MTP_BIND_REQ:
		return ("MTP_BIND_REQ");
	case MTP_UNBIND_REQ:
		return ("MTP_UNBIND_REQ");
	case MTP_CONN_REQ:
		return ("MTP_CONN_REQ");
	case MTP_DISCON_REQ:
		return ("MTP_DISCON_REQ");
	case MTP_ADDR_REQ:
		return ("MTP_ADDR_REQ");
	case MTP_INFO_REQ:
		return ("MTP_INFO_REQ");
	case MTP_OPTMGMT_REQ:
		return ("MTP_OPTMGMT_REQ");
	case MTP_STATUS_REQ:
		return ("MTP_STATUS_REQ");
	case MTP_TRANSFER_REQ:
		return ("MTP_TRANSFER_REQ");
	case MTP_OK_ACK:
		return ("MTP_OK_ACK");
	case MTP_ERROR_ACK:
		return ("MTP_ERROR_ACK");
	case MTP_BIND_ACK:
		return ("MTP_BIND_ACK");
	case MTP_ADDR_ACK:
		return ("MTP_ADDR_ACK");
	case MTP_INFO_ACK:
		return ("MTP_INFO_ACK");
	case MTP_OPTMGMT_ACK:
		return ("MTP_OPTMGMT_ACK");
	case MTP_TRANSFER_IND:
		return ("MTP_TRANSFER_IND");
	case MTP_PAUSE_IND:
		return ("MTP_PAUSE_IND");
	case MTP_RESUME_IND:
		return ("MTP_RESUME_IND");
	case MTP_STATUS_IND:
		return ("MTP_STATUS_IND");
	case MTP_RESTART_BEGINS_IND:
		return ("MTP_RESTART_BEGINS_IND");
	case MTP_RESTART_COMPLETE_IND:
		return ("MTP_RESTART_COMPLETE_IND");
	}
	return ("(unknown)");
}
static const char *
m_errname(mtp_long err)
{
	if (err < 0)
		return unix_errname(-err);
	switch (err) {
	case MSYSERR:
		return ("MSYSERR");
	case MACCESS:
		return ("MACCESS");
	case MBADADDR:
		return ("MBADADDR");
	case MNOADDR:
		return ("MNOADDR");
	case MBADPRIM:
		return ("MBADPRIM");
	case MOUTSTATE:
		return ("MOUTSTATE");
	case MNOTSUPP:
		return ("MNOTSUPP");
	case MBADFLAG:
		return ("MBADFLAG");
	case MBADOPT:
		return ("MBADOPT");
	}
	return ("(unknown)");
}
#endif					/* _OPTIMIZE_SPEED */
const char *
m_errstring(mtp_long err)
{
	if (err < 0)
		return unix_errstring(err);
	switch (err) {
	case MSYSERR:
		return ("UNIX system error occurred");
	case MACCESS:
		return ("User did not have proper permissions");
	case MBADADDR:
		return ("Incorrect address format/illegal address information");
	case MNOADDR:
		return ("MTP provider could not allocate address");
	case MBADPRIM:
		return ("Primitive type not supported by MTP provider");
	case MOUTSTATE:
		return ("Primitive was issued in wrong sequence");
	case MNOTSUPP:
		return ("Not supported by MTP provider");
	case MBADFLAG:
		return ("Flags specified were illegal or incorrect");
	case MBADOPT:
		return ("Incorrect option format/illegal option information");
	}
	return ("Unknown error code");
}

#ifndef _OPTIMIZE_SPEED
static const char *
n_statename(np_long state)
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
	}
	return ("(unknown)");
}
static const char *
n_primname(np_long prim)
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
	}
	return ("(unknown)");
}
static const char *
n_errname(np_long err)
{
	if (err < 0)
		return unix_errname(-err);
	switch (err) {
	case NBADADDR:
		return ("NBADADDR");
	case NBADOPT:
		return ("NBADOPT");
	case NACCESS:
		return ("NACCESS");
	case NNOADDR:
		return ("NNOADDR");
	case NOUTSTATE:
		return ("NOUTSTATE");
	case NBADSEQ:
		return ("NBADSEQ");
	case NSYSERR:
		return ("NSYSERR");
	case NBADDATA:
		return ("NBADDATA");
	case NBADFLAG:
		return ("NBADFLAG");
	case NNOTSUPPORT:
		return ("NNOTSUPPORT");
	case NBOUND:
		return ("NBOUND");
	case NBADQOSPARAM:
		return ("NBADQOSPARAM");
	case NBADQOSTYPE:
		return ("NBADQOSTYPE");
	case NBADTOKEN:
		return ("NBADTOKEN");
	case NNOPROTOID:
		return ("NNOPROTOID");
	}
	return ("(unknown)");
}
#endif					/* _OPTIMIZE_SPEED */
const char *
n_errstring(np_long err)
{
	if (err < 0)
		return unix_errstring(-err);
	switch (err) {
	case NBADADDR:
		return ("Incorrect address format/illegal address information");
	case NBADOPT:
		return ("Options in incorrect format or contain illegal information");
	case NACCESS:
		return ("User did not have proper permissions");
	case NNOADDR:
		return ("NS Provider could not allocate address");
	case NOUTSTATE:
		return ("Primitive was issues in wrong sequence");
	case NBADSEQ:
		return ("Sequence number in primitive was incorrect/illegal");
	case NSYSERR:
		return ("UNIX system error occurred");
	case NBADDATA:
		return ("User data spec. outside range supported by NS provider");
	case NBADFLAG:
		return ("Flags specified in primitive were illegal/incorrect");
	case NNOTSUPPORT:
		return ("Primitive type not supported by the NS provider");
	case NBOUND:
		return ("Illegal second attempt to bind listener or default listener");
	case NBADQOSPARAM:
		return ("QOS values specified are outside the range supported by the NS provider");
	case NBADQOSTYPE:
		return ("QOS structure type specified is not supported by the NS provider");
	case NBADTOKEN:
		return ("Token used is not associated with an open stream");
	case NNOPROTOID:
		return ("Protocol id could not be allocated");
	}
	return ("Unknown error code");
}
 
#ifndef _OPTIMIZE_SPEED
static const char *
l_statename(lmi_long state)
{
	switch (state) {
	case LMI_UNATTACHED:
		return ("LMI_UNATTACHED");
	case LMI_ATTACH_PENDING:
		return ("LMI_ATTACH_PENDING");
	case LMI_UNUSABLE:
		return ("LMI_UNUSABLE");
	case LMI_DISABLED:
		return ("LMI_DISABLED");
	case LMI_ENABLE_PENDING:
		return ("LMI_ENABLE_PENDING");
	case LMI_ENABLED:
		return ("LMI_ENABLED");
	case LMI_DISABLE_PENDING:
		return ("LMI_DISABLE_PENDING");
	case LMI_DETACH_PENDING:
		return ("LMI_DETACH_PENDING");
	}
	return ("(unknown)");
}
#endif					/* _OPTIMIZE_SPEED */

static np_long
n_set_state(struct mtp *mtp, np_long newstate)
{
	np_long oldstate = mtp->curr.n_state;

	if (newstate != oldstate) {
		LOGST(mtp, "%s <- %s", n_statename(newstate), n_statename(oldstate));
		mtp->curr.n_state = mtp->info.npi.CURRENT_state = newstate;
	}
	return (oldstate);
}
static inline np_long
n_get_state(struct mtp *mtp)
{
	return (mtp->curr.n_state);
}
static np_long
n_save_state(struct mtp *mtp)
{
	return (mtp->save.n_state = n_get_state(mtp));
}
static np_long
n_restore_state(struct mtp *mtp)
{
	return n_set_state(mtp, mtp->save.n_state);
}
static inline int
n_get_statef(struct mtp *mtp)
{
	return ((1 << n_get_state(mtp)));
}
static inline int
n_chk_state(struct mtp *mtp, int mask)
{
	return (n_get_statef(mtp) & mask);
}
static inline int
n_not_state(struct mtp *mtp, int mask)
{
	return (n_chk_state(mtp, ~mask));
}

#define MTPSF_UNBND		(1<<MTPS_UNBND)
#define MTPSF_WACK_BREQ		(1<<MTPS_WACK_BREQ)
#define MTPSF_IDLE		(1<<MTPS_IDLE)
#define MTPSF_WACK_CREQ		(1<<MTPS_WACK_CREQ)
#define MTPSF_WCON_CREQ		(1<<MTPS_WCON_CREQ)
#define MTPSF_CONNECTED		(1<<MTPS_CONNECTED)
#define MTPSF_WACK_UREQ		(1<<MTPS_WACK_UREQ)
#define MTPSF_WACK_DREQ6	(1<<MTPS_WACK_DREQ6)
#define MTPSF_WACK_DREQ9	(1<<MTPS_WACK_DREQ9)
#define MTPSF_WACK_OPTREQ	(1<<MTPS_WACK_OPTREQ)
#define MTPSF_WREQ_ORDREL	(1<<MTPS_WREQ_ORDREL)
#define MTPSF_WIND_ORDREL	(1<<MTPS_WIND_ORDREL)
#define MTPSF_WRES_CIND		(1<<MTPS_WRES_CIND)
#define MTPSF_UNUSABLE		(1<<MTPS_UNUSABLE)

static mtp_long
m_set_state(struct mtp *mtp, mtp_long newstate)
{
	mtp_long oldstate = mtp->curr.m_state;

	if (newstate != oldstate) {
		LOGST(mtp, "%s <- %s", m_statename(newstate), m_statename(oldstate));
		mtp->curr.m_state = mtp->info.mtp.mtp_current_state = newstate;
	}
	return (oldstate);
}
static inline mtp_long
m_get_state(struct mtp *mtp)
{
	return mtp->curr.m_state;
}
static mtp_long
m_save_state(struct mtp *mtp)
{
	return (mtp->save.m_state = m_get_state(mtp));
}
static mtp_long
m_restore_state(struct mtp *mtp)
{
	return m_set_state(mtp, mtp->save.m_state);
}
static inline int
m_get_statef(struct mtp *mtp)
{
	return (1 << m_get_state(mtp));
}
static inline int
m_chk_state(struct mtp *mtp, int mask)
{
	return (m_get_statef(mtp) & mask);
}
static inline int
m_not_state(struct mtp *mtp, int mask)
{
	return (m_chk_state(mtp, ~mask));
}

#define ASP_DOWN	0
#define ASP_WACK_ASPUP	1
#define ASP_WACK_ASPDN	2
#define ASP_UP		3
#define ASP_WRSP_RREQ	4
#define ASP_WRSP_DREQ   5
#define ASP_INACTIVE	6
#define ASP_WACK_ASPAC	7
#define ASP_WACK_ASPIA	8
#define ASP_ACTIVE	9

const char *
a_statename(int state)
{
	switch (state) {
	case ASP_DOWN:
		return ("ASP_DOWN");
	case ASP_WACK_ASPUP:
		return ("ASP_WACK_ASPUP");
	case ASP_WACK_ASPDN:
		return ("ASP_WACK_ASPDN");
	case ASP_UP:
		return ("ASP_UP");
	case ASP_WRSP_RREQ:
		return ("ASP_WRSP_RREQ");
	case ASP_WRSP_DREQ:
		return ("ASP_WRSP_DREQ");
	case ASP_INACTIVE:
		return ("ASP_INACTIVE");
	case ASP_WACK_ASPAC:
		return ("ASP_WACK_ASPAC");
	case ASP_WACK_ASPIA:
		return ("ASP_WACK_ASPIA");
	case ASP_ACTIVE:
		return ("ASP_ACTIVE");
	default:
		return ("ASP_????");
	}
}
static inline int
a_set_state(struct mtp *mtp, int newstate)
{
	int oldstate = mtp->curr.a_state;

	if (newstate != oldstate) {
		LOGST(mtp, "%s <- %s", a_statename(newstate), a_statename(oldstate));
		mtp->curr.a_state = newstate;
	}
	return (oldstate);
}
static inline int
a_get_state(struct mtp *mtp)
{
	return (mtp->curr.a_state);
}
static int
a_save_state(struct mtp *mtp)
{
	return (mtp->save.a_state = a_get_state(mtp));
}
static int
a_restore_state(struct mtp *mtp)
{
	return a_set_state(mtp, mtp->save.a_state);
}
static inline int
a_get_statef(struct mtp *mtp)
{
	return ((1 << a_get_state(mtp)));
}
static inline int
a_chk_state(struct mtp *mtp, int mask)
{
	return (a_get_statef(mtp) & mask);
}
static inline int
a_not_state(struct mtp *mtp, int mask)
{
	return (a_chk_state(mtp, ~mask));
}

#define LMF_UNATTACHED		(1<<LMI_UNATTACHED)
#define LMF_ATTACH_PENDING	(1<<LMI_ATTACH_PENDING)
#define LMF_UNUSABLE		(1<<LMI_UNUSABLE)
#define LMF_DISABLED		(1<<LMI_DISABLED)
#define LMF_ENABLE_PENDING	(1<<LMI_ENABLE_PENDING)
#define LMF_ENABLED		(1<<LMI_ENABLED)
#define LMF_DISABLE_PENDING	(1<<LMI_DISABLE_PENDING)
#define LMF_DETACH_PENDING	(1<<LMI_DETACH_PENDING)

static lmi_long
l_set_state(struct mtp *mtp, lmi_long newstate)
{
	lmi_long oldstate = mtp->curr.l_state;

	if (newstate != oldstate) {
		LOGST(mtp, "%s <- %s", l_statename(newstate), l_statename(oldstate));
		mtp->curr.l_state = mtp->info.lmi.lmi_state = newstate;
	}
	return (oldstate);
}
static inline lmi_long
l_get_state(struct mtp *mtp)
{
	return (mtp->curr.l_state);
}
static lmi_long
l_save_state(struct mtp *mtp)
{
	return (mtp->save.l_state = l_get_state(mtp));
}
static lmi_long
l_restore_state(struct mtp *mtp)
{
	return l_set_state(mtp, mtp->save.l_state);
}
static inline int
l_get_statef(struct mtp *mtp)
{
	return ((1 << l_get_state(mtp)));
}
static inline int
l_chk_state(struct mtp *mtp, int mask)
{
	return (l_get_statef(mtp) & mask);
}
static inline int
l_not_state(struct mtp *mtp, int mask)
{
	return (l_chk_state(mtp, ~mask));
}

static inline mtp_long
p_save_state(struct mtp *mtp)
{
	l_save_state(mtp);
	n_save_state(mtp);
	a_save_state(mtp);
	return m_save_state(mtp);
}
static inline mtp_long
p_restore_state(struct mtp *mtp)
{
	l_restore_state(mtp);
	n_restore_state(mtp);
	a_restore_state(mtp);
	return m_restore_state(mtp);
}

/*
 *  M3UA Message Definitions
 */

#define M3UA_PPI    3

#define UA_VERSION  1
#define UA_PAD4(__len) (((__len)+3)&~0x3)
#define UA_MHDR(__version, __spare, __class, __type) \
	(__constant_htonl(((__version)<<24)|((__spare)<<16)|((__class)<<8)|(__type)))

#define UA_MSG_VERS(__hdr) ((ntohl(__hdr)>>24)&0xff)
#define UA_MSG_CLAS(__hdr) ((ntohl(__hdr)>> 8)&0xff)
#define UA_MSG_TYPE(__hdr) ((ntohl(__hdr)>> 0)&0xff)

/*
 *  MESSAGE CLASSES:-
 */
#define UA_CLASS_MGMT	0x00	/* UA Management (MGMT) Message */
#define UA_CLASS_XFER	0x01	/* M3UA Data transfer message */
#define UA_CLASS_SNMM	0x02	/* Signalling Network Mgmt (SNM) Messages */
#define UA_CLASS_ASPS	0x03	/* ASP State Maintenance (ASPSM) Messages */
#define UA_CLASS_ASPT	0x04	/* ASP Traffic Maintenance (ASPTM) Messages */
#define UA_CLASS_Q921	0x05	/* Q.931 User Part Messages */
#define UA_CLASS_MAUP	0x06	/* M2UA Messages */
#define UA_CLASS_CNLS	0x07	/* SUA Connectionless Messages */
#define UA_CLASS_CONS	0x08	/* SUA Connection Oriented Messages */
#define UA_CLASS_RKMM	0x09	/* Routing Key Management Messages */
#define UA_CLASS_TDHM	0x0a	/* TUA Dialog Handling Mesages */
#define UA_CLASS_TCHM	0x0b	/* TUA Component Handling Messages */

/*
 *  MESSAGES DEFINED IN EACH CLASS:-
 */
#define UA_MGMT_ERR		UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x00)
#define UA_MGMT_NTFY		UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x01)
#define UA_MGMT_LAST		0x01

#define UA_XFER_DATA		UA_MHDR(UA_VERSION, 0, UA_CLASS_XFER, 0x01)

#define UA_SNMM_DUNA		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x01)
#define UA_SNMM_DAVA		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x02)
#define UA_SNMM_DAUD		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x03)
#define UA_SNMM_SCON		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x04)
#define UA_SNMM_DUPU		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x05)
#define UA_SNMM_DRST		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x06)
#define UA_SNMM_LAST		0x06

#define UA_ASPS_ASPUP_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x01)
#define UA_ASPS_ASPDN_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x02)
#define UA_ASPS_HBEAT_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x03)
#define UA_ASPS_ASPUP_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x04)
#define UA_ASPS_ASPDN_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x05)
#define UA_ASPS_HBEAT_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x06)
#define UA_ASPS_LAST		0x06

#define UA_ASPT_ASPAC_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x01)
#define UA_ASPT_ASPIA_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x02)
#define UA_ASPT_ASPAC_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x03)
#define UA_ASPT_ASPIA_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x04)
#define UA_ASPT_LAST		0x04

#define UA_RKMM_REG_REQ		UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x01)
#define UA_RKMM_REG_RSP		UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x02)
#define UA_RKMM_DEREG_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x03)
#define UA_RKMM_DEREG_RSP	UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x04)
#define UA_RKMM_LAST		0x04

#define UA_MHDR_SIZE (sizeof(uint32_t)*2)
#define UA_PHDR_SIZE (sizeof(uint32_t))
#define UA_MAUP_SIZE (UA_MHDR_SIZE + UA_PHDR_SIZE + sizeof(uint32_t))

#define UA_TAG_MASK		(__constant_htonl(0xffff0000))
#define UA_PTAG(__phdr)		((htonl(__phdr)>>16)&0xffff)
#define UA_PLEN(__phdr)		(htonl(__phdr)&0xffff)
#define UA_SIZE(__phdr)		(__constant_htonl(__phdr)&0xffff)
#define UA_TAG(__phdr)		((__constant_htonl(__phdr)>>16)&0xffff)
#define UA_PHDR(__phdr, __length) \
	(htonl(((__phdr)<<16)|((__length)+sizeof(uint32_t))))
#define UA_CONST_PHDR(__phdr, __length) \
	(__constant_htonl(((__phdr)<<16)|((__length)+sizeof(uint32_t))))

/*
 *  COMMON PARAMETERS:-
 *
 *  Common parameters per draft-ietf-sigtran-m2ua-10.txt
 *  Common parameters per draft-ietf-sigtran-m3ua-08.txt
 *  Common parameters per draft-ietf-sigtran-sua-07.txt
 *  Common parameters per rfc3057.txt
 *  -------------------------------------------------------------------
 */
#define UA_PARM_RESERVED	UA_CONST_PHDR(0x0000,0)
#define UA_PARM_IID		UA_CONST_PHDR(0x0001,sizeof(uint32_t))
#define UA_PARM_IID_RANGE	UA_CONST_PHDR(0x0002,0)	/* m2ua-10 */
#define UA_PARM_DATA		UA_CONST_PHDR(0x0003,0)	/* sua-07 */
#define UA_PARM_IID_TEXT	UA_CONST_PHDR(0x0003,0)
#define UA_PARM_INFO		UA_CONST_PHDR(0x0004,0)
#define UA_PARM_APC		UA_CONST_PHDR(0x0005,sizeof(uint32_t))	/* sua-07 */
#define UA_PARM_DLCI		UA_CONST_PHDR(0x0005,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_RC		UA_CONST_PHDR(0x0006,sizeof(uint32_t))
#define UA_PARM_DIAG		UA_CONST_PHDR(0x0007,0)
#define UA_PARM_IID_RANGE1	UA_CONST_PHDR(0x0008,0)	/* rfc3057 */
#define UA_PARM_HBDATA		UA_CONST_PHDR(0x0009,0)
#define UA_PARM_REASON		UA_CONST_PHDR(0x000a,sizeof(uint32_t))
#define UA_PARM_TMODE		UA_CONST_PHDR(0x000b,sizeof(uint32_t))
#define UA_PARM_ECODE		UA_CONST_PHDR(0x000c,sizeof(uint32_t))
#define UA_PARM_STATUS		UA_CONST_PHDR(0x000d,sizeof(uint32_t))
#define UA_PARM_ASPID		UA_CONST_PHDR(0x000e,sizeof(uint32_t))
#define UA_PARM_PROT_DATA	UA_CONST_PHDR(0x000e,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_CONG_LEVEL	UA_CONST_PHDR(0x000f,sizeof(uint32_t))	/* sua-07 */
#define UA_PARM_REL_REASON	UA_CONST_PHDR(0x000f,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_TEI_STATUS	UA_CONST_PHDR(0x0010,sizeof(uint32_t))	/* rfc3057 */

/*
 *  Somewhat common field values:
 */
#define   UA_ECODE_INVALID_VERSION		(0x01)
#define   UA_ECODE_INVALID_IID			(0x02)
#define   UA_ECODE_UNSUPPORTED_MESSAGE_CLASS	(0x03)
#define   UA_ECODE_UNSUPPORTED_MESSAGE_TYPE	(0x04)
#define   UA_ECODE_UNSUPPORTED_TRAFFIC_MODE	(0x05)
#define   UA_ECODE_UNEXPECTED_MESSAGE		(0x06)
#define   UA_ECODE_PROTOCOL_ERROR		(0x07)
#define   UA_ECODE_UNSUPPORTED_IID_TYPE		(0x08)
#define   UA_ECODE_INVALID_STREAM_IDENTIFIER	(0x09)
#define  IUA_ECODE_UNASSIGNED_TEI		(0x0a)
#define  IUA_ECODE_UNRECOGNIZED_SAPI		(0x0b)
#define  IUA_ECODE_INVALID_TEI_SAPI_COMBINATION	(0x0c)
#define   UA_ECODE_REFUSED_MANAGEMENT_BLOCKING	(0x0d)
#define   UA_ECODE_ASPID_REQUIRED		(0x0e)
#define   UA_ECODE_INVALID_ASPID		(0x0f)
#define M2UA_ECODE_ASP_ACTIVE_FOR_IIDS		(0x10)
#define   UA_ECODE_INVALID_PARAMETER_VALUE	(0x11)
#define   UA_ECODE_PARAMETER_FIELD_ERROR	(0x12)
#define   UA_ECODE_UNEXPECTED_PARAMETER		(0x13)
#define   UA_ECODE_DESTINATION_STATUS_UNKNOWN	(0x14)
#define   UA_ECODE_INVALID_NETWORK_APPEARANCE	(0x15)
#define   UA_ECODE_MISSING_PARAMETER		(0x16)
#define   UA_ECODE_ROUTING_KEY_CHANGE_REFUSED	(0x17)
#define   UA_ECODE_INVALID_ROUTING_CONTEXT	(0x19)
#define   UA_ECODE_NO_CONFIGURED_AS_FOR_ASP	(0x1a)
#define   UA_ECODE_SUBSYSTEM_STATUS_UNKNOWN	(0x1b)

#define UA_STATUS_AS_DOWN			(0x00010001)
#define UA_STATUS_AS_INACTIVE			(0x00010002)
#define UA_STATUS_AS_ACTIVE			(0x00010003)
#define UA_STATUS_AS_PENDING			(0x00010004)
#define UA_STATUS_AS_INSUFFICIENT_ASPS		(0x00020001)
#define UA_STATUS_ALTERNATE_ASP_ACTIVE		(0x00020002)
#define UA_STATUS_ASP_FAILURE			(0x00020003)
#define UA_STATUS_AS_MINIMUM_ASPS		(0x00020004)

#define UA_TMODE_OVERRIDE			(0x1)
#define UA_TMODE_LOADSHARE			(0x2)
#define UA_TMODE_BROADCAST			(0x3)
#define UA_TMODE_SB_OVERRIDE			(0x4)
#define UA_TMODE_SB_LOADSHARE			(0x5)
#define UA_TMODE_SB_BROADCAST			(0x6)

#define UA_RESULT_SUCCESS			(0x00)
#define UA_RESULT_FAILURE			(0x01)

/* M3UA Specific parameters. */

#define M3UA_PARM_NTWK_APP	UA_CONST_PHDR(0x0200,sizeof(uint32_t))
#define M3UA_PARM_PROT_DATA1	UA_CONST_PHDR(0x0201,0)
#define M3UA_PARM_PROT_DATA2	UA_CONST_PHDR(0x0202,0)
#define M3UA_PARM_AFFECT_DEST	UA_CONST_PHDR(0x0203,sizeof(uint32_t))
#define M3UA_PARM_USER_CAUSE	UA_CONST_PHDR(0x0204,sizeof(uint32_t))
#define M3UA_PARM_CONG_IND	UA_CONST_PHDR(0x0205,sizeof(uint32_t))
#define M3UA_PARM_CONCERN_DEST	UA_CONST_PHDR(0x0206,sizeof(uint32_t))
#define M3UA_PARM_ROUTING_KEY	UA_CONST_PHDR(0x0207,0)
#define M3UA_PARM_REG_RESULT	UA_CONST_PHDR(0x0208,sizeof(uint32_t))
#define M3UA_PARM_DEREG_RESULT	UA_CONST_PHDR(0x0209,sizeof(uint32_t)*4)
#define M3UA_PARM_LOC_KEY_ID	UA_CONST_PHDR(0x020a,sizeof(uint32_t))
#define M3UA_PARM_DPC		UA_CONST_PHDR(0x020b,0)
#define M3UA_PARM_SI		UA_CONST_PHDR(0x020c,0)
#define M3UA_PARM_SSN		UA_CONST_PHDR(0x020d,0)
#define M3UA_PARM_OPC		UA_CONST_PHDR(0x020e,0)
#define M3UA_PARM_CIC		UA_CONST_PHDR(0x020f,0)
#define M3UA_PARM_PROT_DATA3	UA_CONST_PHDR(0x0210,0)	/* proposed */
#define M3UA_PARM_REG_STATUS	UA_CONST_PHDR(0x0212,sizeof(uint32_t))
#define M3UA_PARM_DEREG_STATUS	UA_CONST_PHDR(0x0213,sizeof(uint32_t))

/*
 *  Decode functions.
 */
struct ua_parm {
	union {
		uchar *cp;		/* pointer to parameter field */
		uint32_t *wp;		/* pointer to parameter field */
	};
	size_t len;			/* length of parameter field */
	uint32_t val;			/* value of first 4 bytes (host order) */
};

/*
 *  Extract a parameter from a message.  If the parameter does not exist in the message it returns
 *  false; otherwise true, and sets the parameter values if parm is non-NULL.
 */
static bool
ua_dec_parm(struct mtp *mtp, queue_t *q, mblk_t *mp, struct ua_parm *parm, uint32_t tag)
{
	uint32_t *wp;
	bool rtn = false;

	for (wp = (uint32_t *) mp->b_rptr + 2; (uchar *) (wp + 1) <= mp->b_wptr; wp += (UA_PLEN(*wp) + 3) >> 2) {
		if (UA_TAG(*wp) == UA_TAG(tag)) {
			rtn = true;
			if (parm) {
				parm->wp = (wp + 1);
				parm->len = UA_PLEN(*wp);
				parm->val = ntohl(wp[1]);
			}
			break;
		}
	}
	return (rtn);
}

/*
 *  =========================================================================
 *
 *  OUTPUT Events
 *
 *  =========================================================================
 */

static inline int
m_error(struct mtp *mtp, queue_t *q, mblk_t *msg, int err)
{
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, 2, BPRI_HI)))) {
		DB_TYPE(mp) = M_ERROR;
		mp->b_wptr[0] = err;
		mp->b_wptr[1] = err;
		mp->b_wptr += 2;
		freemsg(msg);
		LOGTX(mtp, "<- M_ERROR");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

static inline int
m_flush(struct mtp *mtp, queue_t *q, mblk_t *msg, int flags, int band)
{
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, 2, BPRI_HI)))) {
		DB_TYPE(mp) = M_FLUSH;
		mp->b_wptr[0] = flags;
		mp->b_wptr[1] = band;
		mp->b_wptr += 2;
		freemsg(msg);
		LOGTX(mtp, "<- M_FLUSH");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * LMI Provider -> LMI User Primitives
 * --------------------------------------------------------------------------
 */
static inline int
lmi_info_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t ppa_ptr, size_t ppa_len)
{
	lmi_info_ack_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = LMI_CURRENT_VERSION;
		p->lmi_state = l_get_state(mtp);
		p->lmi_max_sdu = 272;
		p->lmi_min_sdu = 5;
		p->lmi_header_len = 16;
		p->lmi_ppa_style = LMI_STYLE2;
		p->lmi_ppa_offset = ppa_len ? sizeof(*p) : 0;
		p->lmi_ppa_length = ppa_len;
		p->lmi_prov_flags = 0;
		p->lmi_prov_state = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		freemsg(msg);
		LOGTX(mtp, "<- LMI_INFO_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
lmi_ok_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, lmi_long prim)
{
	lmi_ok_ack_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (prim) {
		case LMI_ATTACH_REQ:
			l_set_state(mtp, LMI_DISABLED);
			n_set_state(mtp, NS_IDLE);
			break;
		case LMI_DETACH_REQ:
			l_set_state(mtp, LMI_UNATTACHED);
			n_set_state(mtp, NS_UNBND);
			break;
		}
		p->lmi_state = l_get_state(mtp);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		LOGTX(mtp, "<- LMI_OK_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
lmi_error_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, lmi_long prim, lmi_long error)
{
	lmi_error_ack_t *p;
	mblk_t *mp;
	lmi_long state = l_get_state(mtp);

	switch (state) {
	case LMI_ATTACH_PENDING:
		state = LMI_UNATTACHED;
		break;
	case LMI_DETACH_PENDING:
		state = LMI_DISABLED;
		break;
	case LMI_ENABLE_PENDING:
		state = LMI_DISABLED;
		break;
	case LMI_DISABLE_PENDING:
		state = LMI_ENABLED;
		break;
	}
	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = error < 0 ? -error : 0;
		p->lmi_reason = error < 0 ? LMI_SYSERR : error;
		p->lmi_error_primitive = prim;
		p->lmi_state = state;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		p_restore_state(mtp);
		LOGTX(mtp, "<- LMI_ERROR_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
noinline int
lmi_error_reply(struct mtp *mtp, queue_t *q, mblk_t *msg, lmi_long prim, lmi_long error)
{
	switch (-error) {
	case EAGAIN:
	case EBUSY:
	case ENOMEM:
	case EDEADLK:
	case ENOBUFS:
		return (error);
	}
	return lmi_error_ack(mtp, q, msg, prim, error);
}
static inline int
lmi_enable_con(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	lmi_enable_con_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = LMI_ENABLED;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		l_set_state(mtp, LMI_ENABLED);
		m_set_state(mtp, MTPS_UNBND);
		LOGTX(mtp, "<- LMI_ENABLE_CON");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
lmi_disable_con(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	lmi_disable_con_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = LMI_DISABLED;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		l_set_state(mtp, LMI_DISABLED);
		n_set_state(mtp, NS_IDLE);
		LOGTX(mtp, "<- LMI_DISABLE_CON");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
lmi_optmgmt_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t opt_ptr, size_t opt_len, lmi_ulong flags)
{
	lmi_optmgmt_ack_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		freemsg(msg);
		LOGTX(mtp, "<- LMI_OPTMGMT_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
lmi_error_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, lmi_long error, int state, mblk_t *dp)
{
	lmi_error_ind_t *p;
	mblk_t *mp;
	int err;

	/* always flush queues when indicating an error */
	if ((err = m_flush(mtp, q, NULL, FLUSHW | FLUSHR, 0)))
		return (err);
	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = error < 0 ? -error : 0;
		p->lmi_reason = error < 0 ? LMI_SYSERR : error;
		p->lmi_state = state;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		if (dp && msg->b_cont == dp)
			msg->b_cont = NULL;
		switch (state) {
		case LMI_DISABLED:
			l_set_state(mtp, LMI_DISABLED);
			a_set_state(mtp, ASP_DOWN);
			/* always wake postponed write queue on an ASP state change */
			if (mtp->wq->q_first)
				qenable(mtp->wq);
			m_set_state(mtp, MTPS_UNUSABLE);	/* XXX */
			break;
		case LMI_ENABLED:
			l_set_state(mtp, LMI_ENABLED);
			a_set_state(mtp, ASP_INACTIVE);
			break;
		default:
			l_set_state(mtp, state);
			break;
		}
		freemsg(msg);
		LOGTX(mtp, "<- LMI_ERROR_IND");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
lmi_stats_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, lmi_ulong interval, mblk_t *dp)
{
	lmi_stats_ind_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_STATS_IND;
		p->lmi_interval = interval;
		p->lmi_timestamp = jiffies;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		if (dp && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		LOGTX(mtp, "<- LMI_STATS_IND");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline int
lmi_event_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, lmi_ulong objectid, lmi_ulong severity, mblk_t *dp)
{
	lmi_event_ind_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_EVENT_IND;
		p->lmi_objectid = objectid;
		p->lmi_timestamp = jiffies;
		p->lmi_severity = severity;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		if (dp && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		LOGTX(mtp, "<- LMI_EVENT_INT");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  MTP Provider (M3UA) -> MTP User Primitives
 *
 *  -------------------------------------------------------------------------
 */
/**
 * mtp_ok_ack: - issue an MTP_OK_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static inline int
mtp_ok_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, mtp_ulong prim)
{
	struct MTP_ok_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_OK_ACK;
		p->mtp_correct_prim = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		LOGTX(mtp, "<- MTP_OK_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_error_ack: - issue an MTP_ERROR_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @err: error (MTPI error positive; UNIX error negative)
 */
static int
mtp_error_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, mtp_ulong prim, mtp_long err)
{
	struct MTP_error_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_ERROR_ACK;
		p->mtp_error_primitive = prim;
		p->mtp_mtpi_error = err < 0 ? MSYSERR : err;
		p->mtp_unix_error = err < 0 ? -err : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		LOGTX(mtp, "<- MTP_ERROR_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static int
mtp_error_reply(struct mtp *mtp, queue_t *q, mblk_t *msg, mtp_long prim, mtp_long err)
{
	switch (-err) {
	case EAGAIN:
	case ENOBUFS:
	case ENOMEM:
	case EBUSY:
	case EDEADLK:
		return (err);
	}
	return mtp_error_ack(mtp, q, msg, prim, err);
}

/**
 * mtp_bind_ack: - issue an MTP_BIND_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aptr: address pointer
 * @alen: length of address
 */
static int
mtp_bind_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen)
{
	struct MTP_bind_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + alen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_BIND_ACK;
		p->mtp_addr_length = alen;
		p->mtp_addr_offset = alen ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(mtp, "<- MTP_BIND_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_addr_ack: - issue an MTP_ADDR_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @lptr: local address pointer
 * @llen: local address length
 * @rptr: remote address pointer
 * @rlen: remote address length
 */
static inline int
mtp_addr_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t lptr, size_t llen, caddr_t rptr, size_t rlen)
{
	struct MTP_addr_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + llen + rlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_ADDR_ACK;
		p->mtp_loc_length = llen;
		p->mtp_loc_offset = llen ? sizeof(*p) : 0;
		p->mtp_rem_length = rlen;
		p->mtp_rem_offset = rlen ? sizeof(*p) + llen : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(lptr, mp->b_wptr, llen);
		mp->b_wptr += llen;
		bcopy(rptr, mp->b_wptr, rlen);
		mp->b_wptr += rlen;
		freemsg(msg);
		LOGTX(mtp, "<- MTP_ADDR_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_info_ack: - issue an MTP_INFO_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
mtp_info_ack(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	struct MTP_info_ack *p;
	mblk_t *mp;
	size_t alen = mtp->info.mtp.mtp_addr_length;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + alen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_INFO_ACK;
		p->mtp_msu_size = mtp->info.mtp.mtp_msu_size;
		p->mtp_addr_size = mtp->info.mtp.mtp_addr_size;
		p->mtp_addr_length = alen;
		p->mtp_addr_offset = alen ? sizeof(*p) : 0;
		p->mtp_current_state = m_get_state(mtp);
		p->mtp_serv_type = mtp->info.mtp.mtp_serv_type;
		p->mtp_version = mtp->info.mtp.mtp_version;
		mp->b_wptr += sizeof(*p);
		bcopy(&mtp->orig, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(mtp, "<- MTP_INFO_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_optmgmt_ack: - issue an MTP_OPTMGMT_ACK primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @optr: options pointer
 * @olen: options length
 * @flags: options flags
 */
static inline int
mtp_optmgmt_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t optr, size_t olen, mtp_ulong flags)
{
	struct MTP_optmgmt_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + olen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_OPTMGMT_ACK;
		p->mtp_opt_length = olen;
		p->mtp_opt_offset = olen ? sizeof(*p) : 0;
		p->mtp_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		freemsg(msg);
		LOGTX(mtp, "<- MTP_OPTMGMT_ACK");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_transfer_ind: - issue an MTP_TRANSFER_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @sptr: source address pointer
 * @slen: source address length
 * @pri: message priority
 * @sls: signalling link selection
 * @dp: user data
 */
static int
mtp_transfer_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t sptr, size_t slen, mtp_ulong pri, mtp_ulong sls, mblk_t *dp)
{
	struct MTP_transfer_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + slen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_TRANSFER_IND;
		p->mtp_srce_length = slen;
		p->mtp_srce_offset = slen ? sizeof(*p) : 0;
		p->mtp_mp = pri;
		p->mtp_sls = sls;
		mp->b_wptr += sizeof(*p);
		bcopy(sptr, mp->b_wptr, slen);
		mp->b_wptr += slen;
		mp->b_cont = dp;
		freemsg(msg);
		LOGDA(mtp, "<- MTP_TRANSFER_IND");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_pause_ind: - issue an MTP_PAUSE_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aptr: address pointer
 * @alen: addres length
 */
static int
mtp_pause_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen)
{
	struct MTP_pause_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + alen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 2;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_PAUSE_IND;
		p->mtp_addr_length = alen;
		p->mtp_addr_offset = alen ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(mtp, "<- MTP_PAUSE_IND");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_resume_ind: - issue an MTP_RESUME_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aptr: address pointer
 * @alen: address length
 */
static int
mtp_resume_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen)
{
	struct MTP_resume_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + alen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 2;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_RESUME_IND;
		p->mtp_addr_length = alen;
		p->mtp_addr_offset = alen ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(mtp, "<- MTP_RESUME_IND");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_status_ind: - issue an MTP_STATUS_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aptr: address pointer
 * @alen: address length
 * @type: status type
 * @status: status
 */
static int
mtp_status_ind(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen, mtp_ulong type, mtp_ulong status)
{
	struct MTP_status_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + alen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 2;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_STATUS_IND;
		p->mtp_addr_length = alen;
		p->mtp_addr_offset = alen ? sizeof(*p) : 0;
		p->mtp_type = type;
		p->mtp_status = status;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		freemsg(msg);
		LOGTX(mtp, "<- MTP_STATUS_IND");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_restart_begins_ind: - issue an MTP_RESTART_BEGINS_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
mtp_restart_begins_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	struct MTP_restart_begins_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 2;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_RESTART_BEGINS_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		LOGTX(mtp, "<- MTP_RESTART_BEGINS_IND");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mtp_restart_complete_ind: - issue an MTP_RESTART_COMPLETE_IND primitive upstream
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
mtp_restart_complete_ind(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	struct MTP_restart_complete_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 2;
		p = (typeof(p)) mp->b_wptr;
		p->mtp_primitive = MTP_RESTART_COMPLETE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		LOGTX(mtp, "<- MTP_RESTART_COMPLETE_IND");
		putnext(mtp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  NPI User (M3UA) -> NPI Provider (SCTP) Primitives
 *
 *  -------------------------------------------------------------------------
 */

/**
 * n_bind_req: - issue a bind request to fullfill an attach request
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @add_ptr: pointer to address to bind
 * @add_len: length of bind address
 */
static inline int
n_bind_req(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t add_ptr, size_t add_len)
{
	N_bind_req_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_REQ;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = 0;
		p->BIND_flags = TOKEN_REQUEST;
		p->PROTOID_length = 0;
		p->PROTOID_offset = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		l_set_state(mtp, LMI_ATTACH_PENDING);
		n_set_state(mtp, NS_WACK_BREQ);
		freemsg(msg);
		LOGTX(mtp, "N_BIND_REQ ->");
		putnext(mtp->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_unbind_req: - issue an unbind request to fullfull a detach request
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
n_unbind_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	N_unbind_req_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
		l_set_state(mtp, LMI_DETACH_PENDING);
		n_set_state(mtp, NS_WACK_UREQ);
		freemsg(msg);
		LOGTX(mtp, "N_UNBIND_REQ ->");
		putnext(mtp->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_conn_req: - issue a connection request to fulfull an enable request
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dst_ptr: destination address
 * @dst_len: destination adresss length
 */
static inline int
n_conn_req(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t dst_ptr, size_t dst_len)
{
	N_qos_sel_conn_sctp_t *n;
	N_conn_req_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p) + dst_len + sizeof(*n), BPRI_MED)) != NULL)) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_CONN_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->CONN_flags = REC_CONF_OPT | EX_DATA_OPT;
		p->QOS_length = sizeof(*n);
		p->QOS_offset = sizeof(*p) + dst_len;
		mp->b_wptr += sizeof(*p);
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		n = (typeof(n)) mp->b_wptr;
		n->n_qos_type = N_QOS_SEL_CONN_SCTP;
		n->i_streams = 33;
		n->o_streams = 33;
		mp->b_wptr += sizeof(*n);
		l_set_state(mtp, LMI_ENABLE_PENDING);
		n_set_state(mtp, NS_WCON_CREQ);
		freemsg(msg);
		LOGTX(mtp, "N_CONN_REQ ->");
		putnext(mtp->wq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_discon_req: - issue a disconnection request to fulfill a disable request
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @reason: disconnect reason
 */
static inline int
n_discon_req(struct mtp *mtp, queue_t *q, mblk_t *msg, np_ulong reason)
{
	N_discon_req_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(mtp->wq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DISCON_REQ;
			p->DISCON_reason = reason;
			p->RES_length = 0;
			p->RES_offset = 0;
			p->SEQ_number = 0;
			mp->b_wptr += sizeof(*p);
			l_set_state(mtp, LMI_DISABLE_PENDING);
			switch (n_get_state(mtp)) {
			case NS_WCON_CREQ:
				n_set_state(mtp, NS_WACK_DREQ6);
				break;
			case NS_WRES_CIND:
				n_set_state(mtp, NS_WACK_DREQ7);
				break;
			case NS_DATA_XFER:
				n_set_state(mtp, NS_WACK_DREQ9);
				break;
			case NS_WCON_RREQ:
				n_set_state(mtp, NS_WACK_DREQ10);
				break;
			case NS_WRES_RIND:
				n_set_state(mtp, NS_WACK_DREQ11);
				break;
			default:
				n_set_state(mtp, NS_IDLE);
				break;
			}
			freemsg(msg);
			LOGTX(mtp, "N_DISCON_REQ ->");
			putnext(mtp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_data_req: - issue a data transfer request
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: data transfer flags
 * @sid: SCTP stream on which to send
 * @dp: user data to transfer
 */
static inline fastcall __hot_out int
n_data_req(struct mtp *mtp, queue_t *q, mblk_t *msg, np_ulong flags, np_ulong sid, mblk_t *dp)
{
	N_qos_sel_data_sctp_t *n;
	N_data_req_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p) + sizeof(*n), BPRI_MED)) != NULL)) {
		if (likely(bcanputnext(mtp->wq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_REQ;
			p->DATA_xfer_flags = flags;
			mp->b_wptr += sizeof(*p);
			n = (typeof(n)) mp->b_wptr;
			n->n_qos_type = N_QOS_SEL_DATA_SCTP;
			n->ppi = M3UA_PPI;
			n->sid = sid;
			n->ssn = 0;
			n->tsn = 0;
			n->more = flags;
			mp->b_wptr += sizeof(*n);
			mp->b_cont = dp;
			freemsg(msg);
			LOGDA(mtp, "N_DATA_REQ ->");
			putnext(mtp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_exdata_req: - issue an expedited data transfer request
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: data transfer flags
 * @sid: SCTP stream on which to send
 * @dp: user data to transfer
 */
static inline fastcall int
n_exdata_req(struct mtp *mtp, queue_t *q, mblk_t *msg, np_ulong flags, np_ulong sid, mblk_t *dp)
{
	N_qos_sel_data_sctp_t *n;
	N_exdata_req_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p) + sizeof(*n), BPRI_MED)) != NULL)) {
		mp->b_band = 1;	/* expedite */
		if (likely(bcanputnext(mtp->wq, mp->b_band))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_EXDATA_REQ;
			mp->b_wptr += sizeof(*p);
			n = (typeof(n)) mp->b_wptr;
			n->n_qos_type = N_QOS_SEL_DATA_SCTP;
			n->ppi = M3UA_PPI;
			n->sid = sid;
			n->ssn = 0;
			n->tsn = 0;
			n->more = flags;
			mp->b_wptr += sizeof(*n);
			mp->b_cont = dp;
			freemsg(msg);
			LOGDA(mtp, "N_EXDATA_REQ ->");
			putnext(mtp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * n_optmgmt_req: - issue an option management request
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @qos_ptr: pointer to QOS
 * @qos_len: length of QOS
 * @flags: options management flags
 */
static inline fastcall int
n_optmgmt_req(struct mtp *mtp, queue_t *q, mblk_t *msg, caddr_t qos_ptr, size_t qos_len, np_ulong flags)
{
	N_optmgmt_req_t *p;
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, sizeof(*p) + qos_len, BPRI_MED)) != NULL)) {
		if (likely(canputnext(mtp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_OPTMGMT_REQ;
			p->QOS_length = qos_len;
			p->QOS_offset = qos_len ? sizeof(*p) : 0;
			p->OPTMGMT_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			freemsg(msg);
			if (n_get_state(mtp) == NS_IDLE)
				n_set_state(mtp, NS_WACK_OPTREQ);
			LOGTX(mtp, "N_OPTMGMT_REQ ->");
			putnext(mtp->wq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 *  --------------------------------------------------------------------------
 *
 *  MTP AS -> MTP SG (M3UA) Sent Messages
 *
 *  --------------------------------------------------------------------------
 */
/**
 * mtp_send_mgmt_err: - send MGMT ERR message
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @ecode: error code
 * @dia_ptr: pointer to diagnostics
 * @dia_len: length of diagnostics
 */
static inline __unlikely int
mtp_send_mgmt_err(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t ecode, caddr_t dia_ptr, size_t dia_len)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_ECODE) + dia_len ? UA_SIZE(UA_PARM_DIAG) + UA_PAD4(dia_len) : 0;

	if (likely((mp = mi_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_MGMT_ERR;
		p[1] = htonl(mlen);
		p[2] = UA_PARM_ECODE;
		p[3] = htonl(ecode);
		p += 4;
		if (dia_len) {
			*p++ = UA_PHDR(UA_PARM_DIAG, dia_len);
			bcopy(dia_ptr, p, dia_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(dia_len);
		/* sent unordered and expedited on management stream */
		if (unlikely((err = n_exdata_req(mtp, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);

	}
	return (-ENOBUFS);
}

/**
 * mtp_send_asps_aspup_req: - send ASP Up
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aspid: ASP identifier if not NULL
 * @inf_ptr: Info pointer
 * @inf_len: Info length
 */
static int
mtp_send_asps_aspup_req(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t *aspid, caddr_t inf_ptr, size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if (likely((mp = mi_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_ASPUP_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(*aspid);
			p += 2;
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		a_set_state(mtp, ASP_WACK_ASPUP);
		mi_timer(mtp->aspup_tack, 2000);
		/* send unordered and expedited on stream 0 */
		if (unlikely((err = n_exdata_req(mtp, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_asps_aspdn_req: - send ASP Down
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @aspid: ASP identifier if not NULL
 * @inf_ptr: Info pointer
 * @inf_len: Info length
 */
int
mtp_send_asps_aspdn_req(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t *aspid, caddr_t inf_ptr, size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + aspid ? UA_SIZE(UA_PARM_ASPID) : 0 + inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if (likely((mp = mi_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_ASPDN_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (aspid) {
			p[0] = UA_PARM_ASPID;
			p[1] = htonl(*aspid);
			p += 2;
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		a_set_state(mtp, ASP_WACK_ASPDN);
		mi_timer(mtp->aspdn_tack, 2000);
		if (unlikely((err = n_exdata_req(mtp, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_asps_hbeat_req: - send a BEAT message
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @sid: SCTP stream identifier
 * @hbt_ptr: heartbeat info pointer
 * @hbt_len: heartbeat info length
 */
static inline int
mtp_send_asps_hbeat_req(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t sid, caddr_t hbt_ptr, size_t hbt_len)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + hbt_len ? UA_PHDR_SIZE + UA_PAD4(hbt_len) : 0;

	if (likely((mp = mi_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (hbt_len) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hbt_len);
			bcopy(hbt_ptr, p, hbt_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hbt_len);
		mi_timer(mtp->hbeat_tack, 2000);
		/* send ordered on specified stream */
		if (unlikely((err = n_data_req(mtp, q, msg, 0, sid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_asps_hbeat_ack: - send a BEAT message
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @sid: SCTP stream identifier
 * @hbt_ptr: heartbeat info pointer
 * @hbt_len: heartbeat info length
 */
static int
mtp_send_asps_hbeat_ack(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t sid, caddr_t hbt_ptr, size_t hbt_len)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + hbt_len ? UA_PHDR_SIZE + UA_PAD4(hbt_len) : 0;

	if (likely((mp = mi_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPS_HBEAT_ACK;
		p[1] = htonl(mlen);
		p += 2;
		if (hbt_len) {
			*p++ = UA_PHDR(UA_PARM_HBDATA, hbt_len);
			bcopy(hbt_ptr, p, hbt_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(hbt_len);
		/* send ordered on specified stream */
		if (unlikely((err = n_data_req(mtp, q, msg, 0, sid, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_aspt_aspac_req: - send ASP Active
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @tmode: traffic mode
 * @rc: Routing Context
 * @num_rc: number of routing contexts
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
static int
mtp_send_aspt_aspac_req(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t tmode, uint32_t *rc, uint32_t num_rc, caddr_t inf_ptr, size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_TMODE) + num_rc ? UA_PHDR_SIZE + num_rc * sizeof(uint32_t) : 0 + inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if (likely((mp = mi_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPAC_REQ;
		p[1] = htonl(mlen);
		p[2] = UA_PARM_TMODE;
		p[3] = htonl(tmode);
		p += 4;
		if (num_rc) {
			uint32_t *ip = rc;

			*p++ = UA_PHDR(UA_PARM_RC, num_rc * sizeof(uint32_t));
			while (num_rc--)
				*p++ = htonl(*ip++);
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		a_set_state(mtp, ASP_WACK_ASPAC);
		mi_timer(mtp->aspac_tack, 2000);
		/* always sent on same stream as data */
		if (unlikely((err = n_data_req(mtp, q, msg, 0, rc ? *rc : 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/**
 * mtp_send_aspt_aspia_req: - send ASP Inactive
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @rc: Routing Context
 * @num_rc: number of routing contexts
 * @inf_ptr: INFO pointer
 * @inf_len: INFO length
 */
static int
mtp_send_aspt_aspia_req(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t *rc, uint32_t num_rc, caddr_t inf_ptr, size_t inf_len)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + num_rc ? UA_PHDR_SIZE + num_rc * sizeof(uint32_t) : 0 + inf_len ? UA_PHDR_SIZE + UA_PAD4(inf_len) : 0;

	if (likely((mp = mi_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_ASPT_ASPIA_REQ;
		p[1] = htonl(mlen);
		p += 2;
		if (num_rc) {
			uint32_t *ip = rc;

			*p++ = UA_PHDR(UA_PARM_RC, num_rc * sizeof(uint32_t));
			while (num_rc--)
				*p++ = htonl(*ip++);
		}
		if (inf_len) {
			*p++ = UA_PHDR(UA_PARM_INFO, inf_len);
			bcopy(inf_ptr, p, inf_len);
		}
		mp->b_wptr = (unsigned char *) p + UA_PAD4(inf_len);
		a_set_state(mtp, ASP_WACK_ASPIA);
		mi_timer(mtp->aspia_tack, 2000);
		if (unlikely((err = n_data_req(mtp, q, msg, 0, rc ? *rc : 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

static int
mtp_send_rkmm_reg_req(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t id, struct mtp_addr *orig, struct mtp_addr *dest)
{
	freemsg(msg);
	return (0);
}
static int
mtp_send_rkmm_dereg_req(struct mtp *mtp, queue_t *q, mblk_t *msg)
{
	freemsg(msg);
	return (0);
}

#if 0
/**
 * mtp_send_rkmm_reg_req: - send REG REQ
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @id: request identifier
 * @sdti: Signalling Data Terminal Identifier
 * @sdli: Signalling Data Link Identifier
 */
static int
mtp_send_rkmm_reg_req(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t id, uint32_t sdti, uint32_t sdli)
{
	int err;
	mblk_t *mp;
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(M2UA_PARM_LINK_KEY);

	if (likely((mp = mi_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_RKMM_REG_REQ;
		p[1] = __constant_htonl(mlen);
		p[2] = M2UA_PARM_LINK_KEY;
		p[3] = M2UA_PARM_LOC_KEY_ID;
		p[4] = htonl(id);
		p[5] = M2UA_PARM_SDTI;
		p[6] = htonl(sdti);
		p[7] = M2UA_PARM_SDLI;
		p[8] = htonl(sdli);
		p += 9;
		mp->b_wptr = (unsigned char *) p;

		if (unlikely((err = n_exdata_req(mtp, q, msg, 0, 0, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}
#endif

/**
 * mtp_send_xfer_data: - send XFER DATA
 * @mtp: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @rc: routing context
 * @orig: originating address
 * @dest: destination address
 * @pri: message priority
 * @sls: signalling link selection
 * @dp: user data
 */
static inline fastcall __hot_write int
mtp_send_xfer_data(struct mtp *mtp, queue_t *q, mblk_t *msg, uint32_t rc, struct mtp_addr *orig, struct mtp_addr *dest, uint8_t pri, uint8_t sls, mblk_t *dp)
{
	int err;
	mblk_t *mp;
	size_t dlen = msgdsize(dp) + 3 * sizeof(uint32_t);
	size_t mlen = UA_MHDR_SIZE + UA_SIZE(UA_PARM_RC) + UA_PHDR_SIZE;

	if (likely((mp = mi_allocb(q, mlen, BPRI_MED)) != NULL)) {
		register uint32_t *p = (typeof(p)) mp->b_wptr;

		p[0] = UA_XFER_DATA;
		p[1] = htonl(mlen + dlen);
		p[2] = UA_PARM_RC;
		p[3] = htonl(rc);
		p[4] = UA_PHDR(M3UA_PARM_PROT_DATA3, dlen);
		p[5] = htonl(orig->pc);
		p[6] = htonl(dest->pc);
		p[7] = htonl(((uint32_t) dest->si << 24) | ((uint32_t) dest->ni << 16) | ((uint32_t) pri << 8) | ((uint32_t) sls << 0));
		mp->b_wptr = (unsigned char *) &p[8];
		mp->b_cont = dp;
		if (unlikely((err = n_data_req(mtp, q, msg, 0, rc, mp))))
			freeb(mp);
		return (err);
	}
	return (-ENOBUFS);
}

/*
 *  --------------------------------------------------------------------------
 *
 *  MTP SG -> MTP AS (M3UA) Received Messages
 *
 *  --------------------------------------------------------------------------
 */

/**
 * mtp_recv_mgmt_err: - process MGMT ERR message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_mgmt_err(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	/* FIXME: check the current state, if we are in a WACK state then we probably have to deal with the error.  If
	   we are not in a WACK state, the error might be able to be ignored, but also might require bringing the ASP
	   down. */
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_mgmt_ntfy: - process MGMT NTFY message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_mgmt_ntfy(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	/* FIXME: Mostly to do with traffic modes.  If we are in override and we came up as a standby, we might now be
	   active. */
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_asps_hbeat_req: - process ASPS BEAT Req message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_asps_hbeat_req(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	caddr_t hptr = NULL;		/* pointer to heartbeat data */
	size_t hlen = 0;		/* length of heartbeat data */
	int sid = 0;			/* stream id for ack message */

	/* FIXME: need to dig HBEAT DATA out of message and possibly derive the stream id from the IID. */

	return mtp_send_asps_hbeat_ack(mtp, q, mp, sid, hptr, hlen);
}

/**
 * mtp_recv_asps_aspup_ack: - process ASPS ASPUP Ack message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 *
 * We only expect an ASPUP Ack as part of the enabling process and when in the
 * LMI_ENABLE_PENDING state.  In that case, confirm the enable.
 */
static int
mtp_recv_asps_aspup_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	if (a_get_state(mtp) != ASP_WACK_ASPUP) {
		if (a_get_state(mtp) != ASP_UP)
			goto unexpected;
		goto discard;
	}
	mi_timer_cancel(mtp->aspup_tack);
	a_set_state(mtp, ASP_UP);
	switch (l_get_state(mtp)) {
	case LMI_ENABLE_PENDING:
		return lmi_enable_con(mtp, q, mp);
	}
      unexpected:
	/* TODO: send unexpected message */
      discard:
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_asps_aspdn_ack: - process ASPS ASPDN Ack message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 *
 * Either we are expecting an ASPDN Ack or it is unsolicited and represents the
 * failure of the M3UA, disabling the interface.  We actually might expect one if
 * we attempt an orderly shutdown.
 */
static int
mtp_recv_asps_aspdn_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	switch (a_get_state(mtp)) {
	case ASP_WACK_ASPDN:
		mi_timer_cancel(mtp->aspdn_tack);
		a_set_state(mtp, ASP_DOWN);
		/* Only if we try an orderly disable.  In that case the next step is to disconnect the interface. */
		if (l_get_state(mtp) == LMI_DISABLE_PENDING)
			return n_discon_req(mtp, q, mp, N_DISC_NORMAL);
		/* fall through */
	case ASP_DOWN:
		/* already down, don't care */
		freemsg(mp);
		return (0);
	case ASP_WACK_ASPUP:
	case ASP_UP:
	case ASP_WRSP_RREQ:
	case ASP_WRSP_DREQ:
	case ASP_INACTIVE:
	case ASP_WACK_ASPAC:
	case ASP_WACK_ASPIA:
	case ASP_ACTIVE:
	default:
		/* really bad in any other state */
		return lmi_error_ind(mtp, q, mp, LMI_DISC, LMI_DISABLED, NULL);
	}
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_asps_hbeat_ack: - process ASPS BEAT Ack message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_asps_hbeat_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_aspt_aspac_ack: - process ASPT ASPAC Ack message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 *
 * We only expect ASPAC Ack as part of the binding or connecting process when in
 * the MTPS__WACK_BREQ or MTPS_WACK_CREQ states.  In that case acknowledge the
 * bind or confirm the connection.
 */
static int
mtp_recv_aspt_aspac_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	if (a_get_state(mtp) != ASP_WACK_ASPAC) {
		if (a_get_state(mtp) != ASP_ACTIVE)
			goto unexpected;
		goto discard;
	}
	mi_timer_cancel(mtp->aspac_tack);
	a_set_state(mtp, ASP_ACTIVE);
	/* always wake postponed write queue on an ASP state change */
	if (mtp->wq->q_first)
		qenable(mtp->wq);
	switch (m_get_state(mtp)) {
	case MTPS_WACK_BREQ:
		if (mtp->info.mtp.mtp_serv_type == M_CLMS)
			return mtp_ok_ack(mtp, q, mp, MTP_BIND_REQ);
		break;
	case MTPS_WACK_CREQ:
		return mtp_ok_ack(mtp, q, mp, MTP_CONN_REQ);
	}
	freemsg(mp);
	return (0);
      unexpected:
	/* TODO: send unexpected message */
      discard:
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_aspt_aspia_ack: - process ASPT ASPIA Ack message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 *
 * When deactivation is complete, this complets a disconnect or unbind operation.
 * Acknowledge the operation.  Note that we do not deregister any dynamically
 * registered routing keys at this point, because an MTP_BIND_REQ with no bind
 * address must activate for the same AS.  Only if an MTP_BIND_REQ is requested
 * with a routing key that is different from a previous bind will this routing key
 * be deregistered.
 */
static int
mtp_recv_aspt_aspia_ack(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	switch (a_get_state(mtp)) {
	case ASP_WACK_ASPIA:
		mi_timer_cancel(mtp->aspia_tack);
		a_set_state(mtp, ASP_INACTIVE);
		/* always wake postponed write queue on an ASP state change */
		if (mtp->wq->q_first)
			qenable(mtp->wq);
		switch (m_get_state(mtp)) {
		case MTPS_WACK_UREQ:
			return mtp_ok_ack(mtp, q, mp, MTP_UNBIND_REQ);
		case MTPS_WACK_DREQ6:
		case MTPS_WACK_DREQ9:
			return mtp_ok_ack(mtp, q, mp, MTP_DISCON_REQ);
		}
		break;
	case ASP_INACTIVE:
	case ASP_UP:
		/* ignore in these states, might be repeat */
		break;
	case ASP_DOWN:
		/* not expected states: discard */
		break;
	case ASP_WRSP_RREQ:
	case ASP_WRSP_DREQ:
	case ASP_WACK_ASPUP:
	case ASP_WACK_ASPDN:
	case ASP_WACK_ASPAC:
		/* not expected states: discard, timers will do the job */
		break;
	case ASP_ACTIVE:
	default:
		/* Not good: unsolicited. */
		a_set_state(mtp, ASP_INACTIVE);
		/* always wake postponed write queue on an ASP state change */
		if (mtp->wq->q_first)
			qenable(mtp->wq);
	}
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_rkmm_reg_rsp: - process RKMM REG Resp message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_rkmm_reg_rsp(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	int err;

	switch (a_get_state(mtp)) {
	case ASP_WRSP_RREQ:
		mi_timer_cancel(mtp->rreq_tack);
		a_set_state(mtp, ASP_INACTIVE);
		/* always wake postponed write queue on an ASP state change */
		if (mtp->wq->q_first)
			qenable(mtp->wq);
		switch (m_get_state(mtp)) {
			uint32_t *rc;
			uint32_t rc_num;

		case MTPS_WACK_BREQ:
		case MTPS_WACK_CREQ:
			rc = mtp->rc ? &mtp->rc : NULL;
			rc_num = rc ? 1 : 0;
			if ((err = mtp_send_aspt_aspac_req(mtp, q, NULL, mtp->tm, rc, rc_num, NULL, 0)))
				return (err);
		}
		switch (m_get_state(mtp)) {
		case MTPS_WACK_BREQ:
			return mtp_bind_ack(mtp, q, mp, (caddr_t)&mtp->orig, sizeof(mtp->orig));
		case MTPS_WACK_CREQ:
			return mtp_ok_ack(mtp, q, mp, MTP_CONN_REQ);
		}
		break;
	}
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_rkmm_dereg_rsp: - process RKMM DEREG Resp message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 *
 * When deregistration is complete, this completes a deregistration operation that
 * was part of a rebind or reconnect to a new routing key (atypical).  Just ignore
 * the repsonse for now.  Deregistation of an old routing key is performed in
 * parallel to the registration of a new routing key, so cannot affect state.
 */
static int
mtp_recv_rkmm_dereg_rsp(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	switch (a_get_state(mtp)) {
	case ASP_WRSP_DREQ:
		mi_timer_cancel(mtp->dreq_tack);
		a_set_state(mtp, ASP_UP);
		switch (m_get_state(mtp)) {
		case MTPS_WACK_UREQ:
			return mtp_ok_ack(mtp, q, NULL, MTP_UNBIND_REQ);
		}
		break;
	}
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_xfer_data: - process XFER DATA message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 *
 * This is simple, just pass the message up in an MTP_TRANSFER_IND provided that
 * the MTP interface is an a data accepting state.
 */
static int
mtp_recv_xfer_data(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct ua_parm data;
	struct mtp_addr orig, dest;
	mtp_ulong pri, sls;
	mblk_t *dp;
	int err;

	if (m_not_state(mtp, ((1 << MTPS_IDLE) | (1 << MTPS_CONNECTED) | (1 << MTPS_WACK_DREQ6) | (1 << MTPS_WACK_DREQ9))))
		goto outstate;

	if (!(dp = dupmsg(mp)))
		goto nobufs;

	if (!ua_dec_parm(mtp, q, dp, &data, M3UA_PARM_PROT_DATA3))
		goto missing;

	orig.pc = htonl(data.wp[1]);
	if (mtp->orig.pc && mtp->orig.pc != orig.pc)
		goto invalid;

	dest.pc = htonl(data.wp[2]);
	dest.si = (htonl(data.wp[3]) >> 24) & 0x00ff;
	dest.ni = (htonl(data.wp[3]) >> 16) & 0x00ff;

	pri = (htonl(data.wp[3]) >> 8) & 0x00ff;
	sls = (htonl(data.wp[3]) >> 0) & 0x00ff;

	dp->b_rptr = (unsigned char *) &data.wp[4];

	/* trim tail to just the data */
	adjmsg(dp, (data.len - 4 * sizeof(uint32_t)) - msgsize(dp));

	err = mtp_transfer_ind(mtp, q, mp, (caddr_t) &dest, sizeof(dest), pri, sls, dp);
	if (err)
		freemsg(dp);
	return (err);
      missing:
	freemsg(dp);
	return (-ENXIO);	/* missing mandatory parameter */
      invalid:
	freemsg(dp);
	return (-EINVAL);	/* invalid parameter */
      nobufs:
	mi_bufcall(q, msgsize(mp), BPRI_MED);
	return (-ENOBUFS);
      outstate:
	/* just discard it */
	freemsg(mp);
	return (0);
}

/**
 * mtp_recv_snmm_duna: - process SNMM DUNA message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_snmm_duna(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct mtp_addr addr;
	struct ua_parm apc;

	if (!ua_dec_parm(mtp, q, mp, &apc, UA_PARM_APC))
		goto missing;

	addr.ni = mtp->orig.ni;
	addr.si = mtp->orig.si;
	addr.pc = apc.val;

	return mtp_pause_ind(mtp, q, mp, (caddr_t) &addr, sizeof(addr));
      missing:
	return (-ENXIO);	/* missing mandatory parameter */
}

/**
 * mtp_recv_snmm_dava: - process SNMM DAVA message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_snmm_dava(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct mtp_addr addr;
	struct ua_parm apc;

	if (!ua_dec_parm(mtp, q, mp, &apc, UA_PARM_APC))
		goto missing;

	addr.ni = mtp->orig.ni;
	addr.si = mtp->orig.si;
	addr.pc = apc.val;

	return mtp_resume_ind(mtp, q, mp, (caddr_t) &addr, sizeof(addr));
      missing:
	return (-ENXIO);	/* missing mandatory parameter */
}

/**
 * mtp_recv_snmm_scon: - process SNMM SCON message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_snmm_scon(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct mtp_addr addr;
	struct ua_parm apc, cong;

	if (!ua_dec_parm(mtp, q, mp, &apc, UA_PARM_APC))
		goto missing;
	if (!ua_dec_parm(mtp, q, mp, &cong, M3UA_PARM_CONG_IND))
		cong.val = MTP_STATUS_CONGESTION;

	addr.ni = mtp->orig.ni;
	addr.si = mtp->orig.si;
	addr.pc = apc.val;

	switch (cong.val) {
	case 0:
	case 1:
	case 2:
	case 3:
		break;
	default:
		goto invalid;
	}

	return mtp_status_ind(mtp, q, mp, (caddr_t) &addr, sizeof(addr), MTP_STATUS_TYPE_CONG, cong.val);
      missing:
	return (-ENXIO);	/* missing mandatory parameter */
      invalid:
	return (-EINVAL);	/* invalid parameter */
}

/**
 * mtp_recv_snmm_dupu: - process SNMM DUPU message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_snmm_dupu(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct mtp_addr addr;
	struct ua_parm apc, uc;
	mtp_ulong user, cause;

	if (!ua_dec_parm(mtp, q, mp, &apc, UA_PARM_APC))
		goto missing;
	if (!ua_dec_parm(mtp, q, mp, &uc, M3UA_PARM_USER_CAUSE))
		goto missing;

	user = uc.val & 0x0000ffff;
	if (mtp->orig.si && user != mtp->orig.si)
		goto invalid;

	cause = (uc.val >> 16) & 0x0000ffff;
	switch (cause) {
	case 0:
	case 1:
	case 2:
		break;
	default:
		goto invalid;
	}

	addr.ni = mtp->orig.ni;
	addr.si = user;
	addr.pc = apc.val;

	return mtp_status_ind(mtp, q, mp, (caddr_t) &addr, sizeof(addr), MTP_STATUS_TYPE_UPU, cause);
      missing:
	return (-ENXIO);	/* missing mandatory parameter */
      invalid:
	return (-EINVAL);	/* invalid parameter */
}

/**
 * mtp_recv_snmm_drst: - process SNMM DRST message
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 */
static int
mtp_recv_snmm_drst(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	struct mtp_addr addr;
	struct ua_parm apc;

	if (!ua_dec_parm(mtp, q, mp, &apc, UA_PARM_APC))
		goto missing;

	addr.ni = mtp->orig.ni;
	addr.si = mtp->orig.si;
	addr.pc = apc.val;

	return mtp_status_ind(mtp, q, mp, (caddr_t) &addr, sizeof(addr), MTP_STATUS_TYPE_RSTR, 0);
      missing:
	return (-ENXIO);	/* missing mandatory parameter */
}

/**
 * mtp_recv_err: - process error for received message
 * @mtp: private structure
 * @q: active queue (read queue)
 * @mp: the message (M3UA part only)
 * @err: error number
 */
static noinline fastcall int
mtp_recv_err(struct mtp *mtp, queue_t *q, mblk_t *mp, int err)
{
	switch (err) {
	case 0:
	case -EBUSY:
	case -ENOMEM:
	case -ENOBUFS:
	case -ENOSR:
	case -EAGAIN:
	case -EDEADLK:
		break;
	case -EINVAL:		/* unexpected message */
		err = UA_ECODE_UNEXPECTED_MESSAGE;
		goto error;
	case -EMSGSIZE:	/* syntax error */
		err = UA_ECODE_PARAMETER_FIELD_ERROR;
		goto error;
	case -EOPNOTSUPP:	/* unrecongized message type */
		err = UA_ECODE_UNSUPPORTED_MESSAGE_TYPE;
		goto error;
	case -ENOPROTOOPT:	/* unrecognized message class */
		err = UA_ECODE_UNSUPPORTED_MESSAGE_CLASS;
		goto error;
	case -EPROTO:		/* protocol error */
		err = UA_ECODE_PROTOCOL_ERROR;
		goto error;
	default:
		if (err < 0)
			err = UA_ECODE_PROTOCOL_ERROR;
	      error:
		return mtp_send_mgmt_err(mtp, q, mp, err, (caddr_t) mp->b_rptr, mp->b_wptr - mp->b_rptr);
	}
	return (err);
}

/**
 * mtp_recv_msg_slow: - process message received from M3UA peer (SG)
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (just the M3UA part)
 */
static noinline fastcall int
mtp_recv_msg_slow(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	register uint32_t *p = (typeof(p)) mp->b_rptr;
	int err = -EMSGSIZE;

	if (!MBLKIN(mp, 0, 2 * sizeof(*p)))
		goto error;
	if (!MBLKIN(mp, 0, ntohl(p[1])))
		goto error;
	switch (UA_MSG_CLAS(p[0])) {
	case UA_CLASS_MGMT:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_MGMT_ERR:
			LOGRX(mtp, "ERR <-");
			err = mtp_recv_mgmt_err(mtp, q, mp);
			break;
		case UA_MGMT_NTFY:
			LOGRX(mtp, "NTFY <-");
			err = mtp_recv_mgmt_ntfy(mtp, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	case UA_CLASS_ASPS:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_ASPS_HBEAT_REQ:
			LOGRX(mtp, "BEAT <-");
			err = mtp_recv_asps_hbeat_req(mtp, q, mp);
			break;
		case UA_ASPS_ASPUP_ACK:
			LOGRX(mtp, "ASPUP Ack <-");
			err = mtp_recv_asps_aspup_ack(mtp, q, mp);
			break;
		case UA_ASPS_ASPDN_ACK:
			LOGRX(mtp, "ASPDN Ack <-");
			err = mtp_recv_asps_aspdn_ack(mtp, q, mp);
			break;
		case UA_ASPS_HBEAT_ACK:
			LOGRX(mtp, "BEAT Ack <-");
			err = mtp_recv_asps_hbeat_ack(mtp, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	case UA_CLASS_ASPT:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_ASPT_ASPAC_ACK:
			LOGRX(mtp, "ASPAC Ack <-");
			err = mtp_recv_aspt_aspac_ack(mtp, q, mp);
			break;
		case UA_ASPT_ASPIA_ACK:
			LOGRX(mtp, "ASPIA Ack <-");
			err = mtp_recv_aspt_aspia_ack(mtp, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	case UA_CLASS_RKMM:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_RKMM_REG_RSP:
			LOGRX(mtp, "REG Rsp <-");
			err = mtp_recv_rkmm_reg_rsp(mtp, q, mp);
			break;
		case UA_RKMM_DEREG_RSP:
			LOGRX(mtp, "DEREG Rsp <-");
			err = mtp_recv_rkmm_dereg_rsp(mtp, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	case UA_CLASS_XFER:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_XFER_DATA:
			LOGDA(mtp, "DATA <-");
			err = mtp_recv_xfer_data(mtp, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	case UA_CLASS_SNMM:
		switch (UA_MSG_TYPE(p[0])) {
		case UA_SNMM_DUNA:
			LOGRX(mtp, "DUNA <-");
			err = mtp_recv_snmm_duna(mtp, q, mp);
			break;
		case UA_SNMM_DAVA:
			LOGRX(mtp, "DAVA <-");
			err = mtp_recv_snmm_dava(mtp, q, mp);
			break;
		case UA_SNMM_SCON:
			LOGRX(mtp, "SCON <-");
			err = mtp_recv_snmm_scon(mtp, q, mp);
			break;
		case UA_SNMM_DUPU:
			LOGRX(mtp, "DUPU <-");
			err = mtp_recv_snmm_dupu(mtp, q, mp);
			break;
		case UA_SNMM_DRST:
			LOGRX(mtp, "DRST <-");
			err = mtp_recv_snmm_drst(mtp, q, mp);
			break;
		default:
			err = (-EOPNOTSUPP);
			break;
		}
		break;
	default:
		err = (-ENOPROTOOPT);
		break;
	}
	if (err == 0)
		return (0);
      error:
	return mtp_recv_err(mtp, q, mp, err);
}

/**
 * mtp_recv_msg: - process message received from M3UA peer (SG)
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message (just the M3UA part)
 */
static inline fastcall int
mtp_recv_msg(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	uint32_t *p = (typeof(p)) mp->b_rptr;
	int err;

	/* fast path for data */
	if (a_chk_state(mtp, ((1 << ASP_ACTIVE) | (1 << ASP_WACK_ASPAC) | (1 << ASP_WACK_ASPIA)))
	    && !MBLKIN(mp, 0, 2 * sizeof(*p))
	    && !MBLKIN(mp, 0, ntohl(p[1]))
	    && UA_MSG_CLAS(p[0]) == UA_CLASS_XFER && UA_MSG_TYPE(p[0]) == UA_XFER_DATA) {
		if ((err = mtp_recv_xfer_data(mtp, q, mp)) == 0)
			return (0);
		return mtp_recv_err(mtp, q, mp, err);
	}
	return mtp_recv_msg_slow(mtp, q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  MTP User -> MTP Provider Primitives
 *
 *  -------------------------------------------------------------------------
 */
/**
 * lmi_info_req: - process LMI_INFO_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the message
 * @p: the primitive
 */
static int
lmi_info_req(struct mtp *mtp, queue_t *q, mblk_t *mp, lmi_info_req_t *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	return lmi_info_ack(mtp, q, mp, NULL, 0);
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(mtp, q, mp, LMI_INFO_REQ, err);
}

/**
 * lmi_attach_req: - process LMI_ATTACH_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the message
 * @p: the primitive
 *
 * LMI_ATTACH_REQ can be used to bind the SCTP interface.  It is simply translated into an
 * N_BIND_REQ.  Note that we don not acknowledge the attach until SCTP acknowledges the bind.
 */
static int
lmi_attach_req(struct mtp *mtp, queue_t *q, mblk_t *mp, lmi_attach_req_t *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	if (l_get_state(mtp) != LMI_UNATTACHED)
		goto outstate;
	if (!MBLKIN(mp, p->lmi_ppa_offset, p->lmi_ppa_length))
		goto badppa;
	return n_bind_req(mtp, q, mp, (caddr_t) p + p->lmi_ppa_offset, p->lmi_ppa_length);
      badppa:
	err = LMI_BADPPA;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(mtp, q, mp, LMI_ATTACH_REQ, err);
}

/**
 * lmi_detach_req: - process LMI_DETACH_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the message
 * @p: the primitive
 *
 * LMI_DETACH_REQ can be used to unbind the SCTP interface.  It is simply translated int an
 * N_UNBIND_REQ.  Note that we do not acknowledge the detach until SCTP acknowledges the unbind.
 */
static int
lmi_detach_req(struct mtp *mtp, queue_t *q, mblk_t *mp, lmi_detach_req_t *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	if (l_get_state(mtp) != LMI_DISABLED)
		goto outstate;
	l_set_state(mtp, LMI_DETACH_PENDING);
	return n_unbind_req(mtp, q, mp);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(mtp, q, mp, LMI_DETACH_REQ, err);
}

/**
 * lmi_enable_req: - process LMI_ENABLE_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the message
 * @p: the primitive
 *
 * LMI_ENABLE_REQ can be used to connect the SCTP interface.  It is translated into an N_CONN_REQ,
 * however, it is not confirmed until the N_CONN_CON is received and the module successfully
 * exchanges an ASP UP and ASP UP ack with the peer.  If you need an ASP Id to be sent, it must be
 * set with input-output controls before enabling the interface.
 */
static int
lmi_enable_req(struct mtp *mtp, queue_t *q, mblk_t *mp, lmi_enable_req_t *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	if (l_get_state(mtp) != LMI_DISABLED)
		goto outstate;
	if (!MBLKIN(mp, p->lmi_rem_offset, p->lmi_rem_length))
		goto badaddress;
	return n_conn_req(mtp, q, mp, (caddr_t) p + p->lmi_rem_offset, p->lmi_rem_length);
      badaddress:
	err = LMI_BADADDRESS;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(mtp, q, mp, LMI_ENABLE_REQ, err);
}

/**
 * lmi_disable_req: - process LMI_DISABLE_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the message
 * @p: the primitive
 *
 * LMI_DISABLE_REQ can be used to disconnect the SCTP interface.  It is simply translated into an
 * N_DISCON_REQ.  We do no deregister or deactivate under M3UA.  We simply disconnect the SCTP
 * association.  The disable is not confirmed until the N_OK_ACK is received.
 */
static int
lmi_disable_req(struct mtp *mtp, queue_t *q, mblk_t *mp, lmi_disable_req_t *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	if (l_not_state(mtp, (LMF_ENABLED | LMF_ENABLE_PENDING)))
		goto outstate;
	return n_discon_req(mtp, q, mp, N_DISC_NORMAL);
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(mtp, q, mp, LMI_DISABLE_REQ, err);
}

/**
 * lmi_optmgmt_req: - process LMI_OPTMGMT_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the message
 * @p: the primitive
 *
 * LMI_OPTMGMT_REQ can be used to set options on the SCTP interface.  It is simply translated into a
 * N_OPTMGMT_REQ.
 */
static int
lmi_optmgmt_req(struct mtp *mtp, queue_t *q, mblk_t *mp, lmi_optmgmt_req_t * p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto protoshort;
	if (!MBLKIN(mp, p->lmi_opt_offset, p->lmi_opt_length))
		goto protoshort;
	if (p->lmi_mgmt_flags != LMI_NEGOTIATE)
		goto notsupp;
	return n_optmgmt_req(mtp, q, mp, (caddr_t) p + p->lmi_opt_offset, p->lmi_opt_length, 0);
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(mtp, q, mp, LMI_OPTMGMT_REQ, err);
}

/**
 * mtp_bind_req: - process MTP_BIND_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the message
 * @p: the primitive
 *
 * Note an MTP_BIND_REQ that does not contain any address is an attempt to rebind
 * to a previously bound address.  Also, if there is an MTP bind flag set
 * indicating that this is a pseudo-connection oriented bind, then we wait for an
 * MTP_CONN_REQ before we do anything with regards to M3UA regsitration or
 * activation.
 */
static int
mtp_bind_req(struct mtp *mtp, queue_t *q, mblk_t *mp, struct MTP_bind_req *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (m_get_state(mtp) != MTPS_UNBND)
		goto outstate;
	if (!MBLKIN(mp, p->mtp_addr_offset, p->mtp_addr_length))
		goto badaddr;
	if (p->mtp_addr_length == 0) {
		/* Note that an MTP_BIND_REQ that does not contain any address is an attempt to rebind to a previously
		   bound address. */
		if (mtp->info.mtp.mtp_addr_length < sizeof(mtp->orig))
			goto noaddr;
	} else if (p->mtp_addr_length < sizeof(mtp->orig)) {
		goto badaddr;
	} else {
		bcopy((caddr_t) p + p->mtp_addr_offset, &mtp->orig, sizeof(mtp->orig));
		/* check the address */
		if (mtp->orig.family != AF_MTP && mtp->orig.family != 0)
			goto badaddr;
		/* no management or invalid SIs */
		if (mtp->orig.si == 1 || mtp->orig.si == 2 || mtp->orig.si > 15)
			goto badaddr;
		/* ok, set */
		mtp->info.mtp.mtp_addr_length = sizeof(mtp->orig);
		mtp->info.mtp.mtp_addr_offset = sizeof(mtp->info.mtp);
	}
	m_set_state(mtp, MTPS_WACK_BREQ);
	if (p->mtp_bind_flags) {
		/* If there is an MTP bind flag set, it indicates that this is a pseudo-connection oriented bind.  In
		   this case we wait for an MTP_CONN_REQ before we do anything with regard to M3UA registration or
		   activation. */
		mtp->info.mtp.mtp_serv_type = M_COMS;
		return (mtp_bind_ack(mtp, q, mp, (caddr_t) &mtp->orig, sizeof(mtp->orig)));
	}
	mtp->info.mtp.mtp_serv_type = M_CLMS;
	if (mtp->rc) {
		/* If we have a set routing context then we do not need to register dynamically, just activate the AS. */
		return mtp_send_aspt_aspac_req(mtp, q, mp, mtp->tm, &mtp->rc, 1, NULL, 0);
	}
	/* FIXME: set this up properly. */
	return mtp_send_rkmm_reg_req(mtp, q, mp, 1, NULL, NULL);
      noaddr:
	err = MNOADDR;
	goto error;
      badaddr:
	err = MBADADDR;
	goto error;
      outstate:
	err = MOUTSTATE;
	goto error;
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_reply(mtp, q, mp, MTP_BIND_REQ, err);
}

/**
 * mtp_unbind_req: - process MTP_UNBIND_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the message
 * @p: the primitive
 */
static int
mtp_unbind_req(struct mtp *mtp, queue_t *q, mblk_t *mp, struct MTP_unbind_req *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (m_get_state(mtp) != MTPS_IDLE)
		goto outstate;
	m_set_state(mtp, MTPS_WACK_UREQ);
	if (mtp->info.mtp.mtp_serv_type == M_CLMS) {
		uint32_t *rc;
		uint32_t rc_num;

		/* For connectionless service we can acknowledge the unbind immediately but initiate a deactivate
		   procedure if not already initiated. */
		rc = mtp->rc ? &mtp->rc : NULL;
		rc_num = rc ? 1 : 0;
		if (a_get_state(mtp) == ASP_ACTIVE)
			if ((err = mtp_send_aspt_aspia_req(mtp, q, NULL, rc, rc_num, NULL, 0)))
				return (err);
	}
	return mtp_ok_ack(mtp, q, mp, MTP_UNBIND_REQ);
outstate:
	err = MOUTSTATE;
	goto error;
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_reply(mtp, q, mp, MTP_UNBIND_REQ, err);
}

/**
 * mtp_conn_req: - process MTP_CONN_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the message
 * @p: the primitive
 */
static int
mtp_conn_req(struct mtp *mtp, queue_t *q, mblk_t *mp, struct MTP_conn_req *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (mtp->info.mtp.mtp_serv_type != M_COMS)
		goto notsupp;
	if (m_get_state(mtp) != MTPS_IDLE)
		goto outstate;
	if (!MBLKIN(mp, p->mtp_addr_offset, p->mtp_addr_length))
		goto badaddr;
	if (p->mtp_addr_length == 0) {
		/* Note that an MTP_CONN_REQ that does not contain any address is an attempt to reconnect to a
		   previously connected address. */
		if (mtp->info.mtp.mtp_addr_length < sizeof(mtp->orig) + sizeof(mtp->dest))
			goto noaddr;
	} else if (p->mtp_addr_length < sizeof(mtp->dest)) {
		goto badaddr;
	} else {
		bcopy((caddr_t) p + p->mtp_addr_offset, &mtp->dest, sizeof(mtp->dest));
		/* check the address */
		if (mtp->dest.family != AF_MTP && mtp->dest.family != 0)
			goto badaddr;
		/* no management or invalid SIs */
		if (mtp->dest.si == 1 || mtp->dest.si == 2 || mtp->dest.si > 15)
			goto badaddr;
		/* ok, set */
		mtp->info.mtp.mtp_addr_length = sizeof(mtp->orig) + sizeof(mtp->dest);
		mtp->info.mtp.mtp_addr_offset = sizeof(mtp->info.mtp);
	}
	m_set_state(mtp, MTPS_WACK_CREQ);
	if (mtp->rc) {
		/* If we have a set routing context then we do not need to regsiter dynamically, just activate the AS. */
		return mtp_send_aspt_aspac_req(mtp, q, mp, mtp->tm, &mtp->rc, 1, NULL, 0);
	}
	/* FIXME: set this up properly */
	return mtp_send_rkmm_reg_req(mtp, q, mp, 1, NULL, NULL);
      noaddr:
	err = MNOADDR;
	goto error;
      badaddr:
	err = MBADADDR;
	goto error;
      notsupp:
	err = MNOTSUPP;
	goto error;
      outstate:
	err = MOUTSTATE;
	goto error;
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_reply(mtp, q, mp, MTP_CONN_REQ, err);
}

/**
 * mtp_discon_req: - process MTP_DISCON_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the message
 * @p: the primitive
 */
static int
mtp_discon_req(struct mtp *mtp, queue_t *q, mblk_t *mp, struct MTP_discon_req *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (mtp->info.mtp.mtp_serv_type != M_COMS)
		goto notsupp;
	if (m_get_state(mtp) != MTPS_CONNECTED)
		goto outstate;
	m_set_state(mtp, MTPS_WACK_DREQ9);
	{
		uint32_t *rc;
		uint32_t rc_num;

		/* Acknowledge the disconnect immediately, but initiate a deactivate procedure if not already
		   initiated. */
		rc = mtp->rc ? &mtp->rc : NULL;
		rc_num = rc ? 1 : 0;
		if (a_get_state(mtp) == ASP_ACTIVE)
			if ((err = mtp_send_aspt_aspia_req(mtp, q, NULL, rc, rc_num, NULL, 0)))
				return (err);
	}
	return mtp_ok_ack(mtp, q, mp, MTP_DISCON_REQ);
      outstate:
	err = MOUTSTATE;
	goto error;
      notsupp:
	err = MNOTSUPP;
	goto error;
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_reply(mtp, q, mp, MTP_DISCON_REQ, err);
}

/**
 * mtp_addr_req: - process MTP_ADDR_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the message
 * @p: the primitive
 */
static int
mtp_addr_req(struct mtp *mtp, queue_t *q, mblk_t *mp, struct MTP_addr_req *p)
{
	int err;
	caddr_t loc_ptr = NULL, rem_ptr = NULL;
	size_t loc_len = 0, rem_len = 0;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	/* Note that we offer up addresses when they are assigned regardless of the current state of the interface. If
	   the interface has been previously bound, there will always be a local address reported.  If the interface
	   has been previously connected, there will always be a remote address. */
	if (mtp->info.mtp.mtp_addr_length >= sizeof(mtp->orig)) {
		loc_ptr = (caddr_t) &mtp->orig;
		loc_len = sizeof(mtp->orig);
	}
	if (mtp->info.mtp.mtp_addr_length >= sizeof(mtp->orig) + sizeof(mtp->dest)) {
		rem_ptr = (caddr_t) &mtp->dest;
		rem_len = sizeof(mtp->dest);
	}
	return mtp_addr_ack(mtp, q, mp, loc_ptr, loc_len, rem_ptr, rem_len);
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_reply(mtp, q, mp, MTP_ADDR_REQ, err);
}

/**
 * mtp_info_req: - process MTP_INFO_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the message
 * @p: the primitive
 */
static int
mtp_info_req(struct mtp *mtp, queue_t *q, mblk_t *mp, struct MTP_info_req *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	return mtp_info_ack(mtp, q, mp);
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_reply(mtp, q, mp, MTP_INFO_REQ, err);
}

/**
 * mtp_optmgmt_req: - process MTP_OPTMGMT_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the message
 * @p: the primitive
 */
static int
mtp_optmgmt_req(struct mtp *mtp, queue_t *q, mblk_t *mp, struct MTP_optmgmt_req *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (!MBLKIN(mp, p->mtp_opt_offset, p->mtp_opt_length))
		goto badopt;
	goto notsupp;
      notsupp:
	err = MNOTSUPP;
	goto error;
      badopt:
	err = MBADOPT;
	goto error;
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_reply(mtp, q, mp, MTP_OPTMGMT_REQ, err);
}

static int
mtp_status_req(struct mtp *mtp, queue_t *q, mblk_t *mp, struct MTP_status_req *p)
{
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	goto notsupp;
      notsupp:
	err = MNOTSUPP;
	goto error;
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_reply(mtp, q, mp, MTP_STATUS_REQ, err);
}

/**
 * mtp_transfer_req: - process MTP_TRANSFER_REQ primitive
 * @mtp: MTP private structure
 * @q: active queue (write queue)
 * @mp: the message
 * @p: the primitive
 */
static int
mtp_transfer_req(struct mtp *mtp, queue_t *q, mblk_t *mp, struct MTP_transfer_req *p)
{
	struct mtp_addr dst, *dest = &mtp->dest;
	mblk_t *dp;
	size_t dlen;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto badprim;
	if (!MBLKIN(mp, p->mtp_dest_offset, p->mtp_dest_length))
		goto badaddr;
	if (mtp->info.mtp.mtp_serv_type == M_COMS) {
		switch (m_get_state(mtp)) {
		case MTPS_CONNECTED:
			break;
		case MTPS_IDLE:
			goto discard;
		default:
			goto outstate;
		}
		if (p->mtp_dest_length > 0 && p->mtp_dest_length < sizeof(dst))
			goto badaddr;
		if (p->mtp_dest_length > 0) {
			dest = &dst;
			bcopy((caddr_t) p + p->mtp_dest_offset, dest, sizeof(dst));
			/* default si value */
			if (dst.si == 0)
				dst.si = mtp->dest.si;
		}
	} else {
		if (m_get_state(mtp) != MTPS_IDLE)
			goto outstate;
		if (p->mtp_dest_length < sizeof(dst))
			goto badaddr;
		dest = &dst;
		bcopy((caddr_t) p + p->mtp_dest_offset, dest, sizeof(dst));
	}
	if ((dp = mp->b_cont) == NULL || (dlen = msgdsize(dp)) < mtp->info.lmi.lmi_min_sdu || dlen > mtp->info.lmi.lmi_max_sdu)
		goto baddata;
	if (dest != &mtp->dest) {
		/* check provided address */
		if (dst.family != AF_MTP && dst.family != 0)
			goto badaddr;
		/* default si value */
		if (dst.si == 0)
			dst.si = mtp->orig.si;
		/* no management or invalid SIs */
		if (dst.si < 3 || dst.si > 15)
			goto badaddr;
	}
	/* Ok, just before transferring the data check the ASP state.  If we have a parallel process activating the ASP 
	   and it has not activated yet, we want to postpone data transfers until the process completes. */
	if (a_get_state(mtp) == ASP_WACK_ASPAC)
		return (-EAGAIN);	/* wait for activation */
	err = mtp_send_xfer_data(mtp, q, mp, mtp->rc, &mtp->orig, dest, p->mtp_mp, p->mtp_sls, mp->b_cont);
	if (err == 0)
		freeb(mp);
	return (err);
      baddata:
#ifdef MBADDATA
	err = MBADDATA;
#else
	err = MBADOPT;
#endif
	goto error;
      discard:
	freemsg(mp);
	return (0);
      outstate:
	err = MOUTSTATE;
	goto error;
      badaddr:
	err = MBADADDR;
	goto error;
      badprim:
	err = MBADPRIM;
	goto error;
      error:
	return mtp_error_reply(mtp, q, mp, MTP_TRANSFER_REQ, err);
}

/**
 * mtp_other_req: - process unrecognized MTP primitives
 * @mtp: MTP private structure
 * @q: active queue (write queu)
 * @mp: the message
 *
 * Because the numbering of MTP messages is substantially different from NPI messages, it is
 * posisble to pass through SCTP NPI messages from the upper queue, intercepting some information on
 * the way by. This permits the initial establishment of an SCTP association with bound and
 * connected addresses retained.  This is perhaps not the best approach, but it is here.  Another
 * approach is to configure the M3UA module to map MTP addresses to SCTP addresses on the SG and
 * potentially routing contexts for the SG using input-output controls.  Another approach yet is to
 * query the bound and connected addresses when the module is pushed.  The last approach is probably
 * best.
 */
static int
mtp_other_req(struct mtp *mtp, queue_t *q, mblk_t *mp, mtp_long *p)
{
	freemsg(mp);
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Timeouts
 *
 *  -------------------------------------------------------------------------
 */
/**
 * mtp_aspup_tack_timeout: - timeout awaiting ASP Up Ack
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 *
 * If the SG is not going to respond, do not resend it.
 */
static int
mtp_aspup_tack_timeout(struct mtp *mtp, queue_t *q)
{
	switch (__builtin_expect(a_get_state(mtp), ASP_WACK_ASPUP)) {
	case ASP_WACK_ASPUP:
		/* we still want one */
		a_set_state(mtp, ASP_DOWN);
		if (l_get_state(mtp) == LMI_ENABLE_PENDING) {
			return lmi_error_ind(mtp, q, NULL, LMI_INITFAILED, LMI_DISABLED, NULL);
		}

		break;
	}
	return (0);
}

/**
 * mtp_aspac_tack_timeout: - timeout awaiting ASP Active Ack
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 */
static int
mtp_aspac_tack_timeout(struct mtp *mtp, queue_t *q)
{
	switch (__builtin_expect(a_get_state(mtp), ASP_WACK_ASPAC)) {
	case ASP_WACK_ASPAC:
		/* we still want one */
		a_set_state(mtp, ASP_INACTIVE);
		/* always wake postponed write queue on an ASP state change */
		if (mtp->wq->q_first)
			qenable(mtp->wq);
		switch (m_get_state(mtp)) {
		case MTPS_WACK_BREQ:
			return mtp_error_ack(mtp, q, NULL, MTP_BIND_REQ, -ETIMEDOUT);
		case MTPS_WACK_CREQ:
			return mtp_error_ack(mtp, q, NULL, MTP_CONN_REQ, -ETIMEDOUT);
		}
		return lmi_error_ind(mtp, q, NULL, LMI_DISC, LMI_ENABLED, NULL);
	}
	return (0);
}

/**
 * mtp_aspdn_tack_timeout: - timeout awaiting ASP Down Ack
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 */
static int
mtp_aspdn_tack_timeout(struct mtp *mtp, queue_t *q)
{
	switch (__builtin_expect(a_get_state(mtp), ASP_WACK_ASPDN)) {
	case ASP_WACK_ASPDN:
		/* we still want one, act like we got one */
		a_set_state(mtp, ASP_DOWN);
		switch (l_get_state(mtp)) {
		case LMI_DISABLE_PENDING:
			return n_discon_req(mtp, q, NULL, N_DISC_NORMAL);
		}
		break;
	}
	return (0);
}

/**
 * mtp_aspia_tack_timeout: - timeout awaiting ASP Inactive Ack
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 */
static int
mtp_aspia_tack_timeout(struct mtp *mtp, queue_t *q)
{
	switch (__builtin_expect(a_get_state(mtp), ASP_WACK_ASPIA)) {
	case ASP_WACK_ASPIA:
		/* we still want one, act like we got one */
		a_set_state(mtp, ASP_INACTIVE);
		/* always wake postponed write queue on an ASP state change */
		if (mtp->wq->q_first)
			qenable(mtp->wq);
		switch (m_get_state(mtp)) {
		case MTPS_WACK_UREQ:
			if (!mtp->rc)
				return mtp_send_rkmm_dereg_req(mtp, q, NULL);
			return mtp_ok_ack(mtp, q, NULL, MTP_UNBIND_REQ);
		}
		break;
	}
	return (0);
}

/**
 * mtp_hbeat_tack_timeout: - timeout awaiting Heartbeat Ack
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 */
static int
mtp_hbeat_tack_timeout(struct mtp *mtp, queue_t *q)
{
	/* We don't send heartbeats for the moment. */
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  N Provider -> N User Primitives
 *
 *  -------------------------------------------------------------------------
 */
/**
 * n_conn_ind: - process N_CONN_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @p: the primitive
 *
 * M3UA-AS never binds as a responder: therefore we do not expect to receive these.
 */
static int
n_conn_ind(struct mtp *mtp, queue_t *q, mblk_t *mp, N_conn_ind_t * p)
{
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	goto notsupport;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * n_conn_con: - process N_CONN_CON primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @p: the primitive
 *
 * When we get a valid connection confirmation it is a confirmation of our attempt to connect that
 * was part of the LMI_ENABLE_REQ operation.  Respond with an LMI_ENABLE_CON primitive.  We do not
 * save the responding address.
 * 
 * The question is whether to send ASP UP at this point first.  It is probably a good idea.  We can
 * pend sending the LMI_ENABLE_CON util the ASP UP ACK is received, an error is received, or the
 * operation times out.
 */
static int
n_conn_con(struct mtp *mtp, queue_t *q, mblk_t *mp, N_conn_con_t * p)
{
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (l_get_state(mtp) != LMI_ENABLE_PENDING)
		goto outstate;
	n_set_state(mtp, NS_DATA_XFER);
	{
		uint32_t *aspid;

		aspid = mtp->aspid ? &mtp->aspid : NULL;
		return mtp_send_asps_aspup_req(mtp, q, mp, aspid, NULL, 0);
	}
      outstate:
	err = NOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * n_discon_ind: - process N_DISCON_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @p: the primitive
 *
 * Procedures for disconnect are somewhat different depending upon what state we are in.  If there
 * is an enable pending, we need to error acknowledge the enable.  If there is a disable pending, we
 * are fine.  If we are in an enabled state, indicate an error and move to the disabled state.  If
 * MTP is bound it is implicitly unbound by the disable.
 *
 * Note that this makes M3UA somewhat different to use than a pure MTP stream.  M3UA is more like a
 * signalling link set anyway (i.e. one that can fail and restart).  This interface is intended on
 * being used from within the MTP multiplexing driver that indeed treats M3UA-AS streams as link
 * sets.  When an M3UA stream my be used directly, the user should respond appropriately to LMI
 * primitives, especially LMI_ERROR_INDs.  If a disabling LMI_ERROR_IND is indicated, an
 * LMI_ENABLE_REQ without a remote address will attempt to restablish to the same destination and a
 * MTP_BIND_REQ without an address will attempt to rebind to the same MTP address as was previously
 * bound.  What timers or other procedures are employed are up to the user of the stream.  In the
 * case of MTP, forced rerouting procedures will be used when the link fails, probationary measures
 * will to taken to avoid oscillations and possible MTP restart procedures will also apply.
 *
 * TODO: careful about flushing when sending N_DISCON_REQ.  When we receive a flush from below, we
 * should really not pass it to the Stream Head but should loop it back around.  Also when we
 * received a flush from the Stream Head, we should not pass it down, but loop it back to the Stream
 * head, both as though we were a multiplexing driver instead of a module.
 */
static int
n_discon_ind(struct mtp *mtp, queue_t *q, mblk_t *mp, N_discon_ind_t * p)
{
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	n_set_state(mtp, NS_IDLE);
	switch (l_get_state(mtp)) {
	case LMI_ENABLE_PENDING:
		/* TODO: we could examine the p->DISCON_reason and provide more fine grained error codes here. */
		n_set_state(mtp, NS_IDLE);
		return lmi_error_ind(mtp, q, mp, LMI_INITFAILED, LMI_DISABLED, NULL);
	case LMI_ENABLED:
		/* TODO: we could examine the p->DISCON_reason and provide more fine grained error codes here. */
		n_set_state(mtp, NS_IDLE);
		return lmi_error_ind(mtp, q, mp, LMI_DISC, LMI_DISABLED, NULL);
	case LMI_DISABLE_PENDING:
		/* Confirm it now: we might not get an N_OK_ACK for our disconnect request, or we might get an
		   N_ERROR_ACK for out of state momentarily.  Or our N_DISCON_REQ might have been flushed. */
		return lmi_disable_con(mtp, q, mp);
	case LMI_DISABLED:
	case LMI_UNUSABLE:
		/* ignore it, we are already disabled */
		goto discard;
	default:
		goto outstate;
	}
	goto outstate;
      discard:
	freemsg(mp);
	return (0);
      outstate:
	err = NOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * n_data_ind_slow: - slow path processing for N_DATA_IND
 * @mtp: private structure
 * @q: active queue (read queue)
 * @mp: the N_DATA_IND primitive
 *
 * Normally we receive data unfragmented and in a single M_DATA message block.  This slower routine
 * handles the circumstance where we receive fragmented data or data that is chained together in
 * multiple M_DATA message blocks.  It also handles various slow path error conditions.
 */
noinline fastcall __hot_in int
n_data_ind_slow(struct mtp *mtp, queue_t *q, mblk_t *mp, N_data_ind_t * p)
{
	mblk_t *dp, *bp, *newp = NULL;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (l_get_state(mtp) != LMI_ENABLED)
		goto outstate;
	if ((dp = mp->b_cont) == NULL)
		goto baddata;
	if (dp->b_cont != NULL) {
		/* We have chained M_DATA blocks: pull them up. */
		if (!pullupmsg(dp, -1)) {
			/* normally only fail because of dup'ed blocks */
			if (!(newp = msgpullup(dp, -1))) {
				/* normally only fail because of no buffer */
				mi_bufcall(q, xmsgsize(dp), BPRI_MED);
				return (-ENOBUFS);
			}
			dp = newp;
		}
	}
	if (p->DATA_xfer_flags & N_MORE_DATA_FLAG) {
		/* We have a partial delivery.  Chain normal message together.  We might have a problem with messages
		   split over multiple streams? Treat normal and expedited separately. */
		mtp->rbuf = linkmsg(mtp->rbuf, dp);
	} else {
		bp = linkmsg(XCHG(&mtp->rbuf, NULL), dp);
		(void) bp;
		if ((err = mtp_recv_msg(mtp, q, mp))) {
			if (err < 0) {
				mtp->rbuf = unlinkmsg(mtp->rbuf, dp);
				if (newp)
					freemsg(newp);
			}
			return (err);
		}
	}
	if (newp == NULL)
		mp->b_cont = NULL;
	freemsg(mp);
	return (0);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      baddata:
	err = NBADDATA;
	goto error;
      error:
	return (err);
}

/**
 * n_data_ind: - process N_DATA_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @p: the primitive
 *
 */
static inline fastcall __hot_in int
n_data_ind(struct mtp *mtp, queue_t *q, mblk_t *mp, N_data_ind_t * p)
{
	mblk_t *dp;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto go_slow;
	if (unlikely(l_get_state(mtp) != LMI_ENABLED))
		goto go_slow;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto go_slow;
	if (unlikely(dp->b_cont != NULL))
		goto go_slow;
	if (unlikely(p->DATA_xfer_flags & N_MORE_DATA_FLAG))
		goto go_slow;
	if (unlikely(mtp->rbuf != NULL))
		goto go_slow;
	if ((err = mtp_recv_msg(mtp, q, dp)) == 0)
		freeb(mp);
	return (err);
      go_slow:
	return n_data_ind_slow(mtp, q, mp, p);
}

/**
 * n_exdata_ind_slow: - process N_EXDATA_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @p: the primitive
 *
 * Normaly we receive data unfragmented and in a single M_DATA message block.  This slower routine
 * handles the circumstance where we receive fragmented data or data that is chained together in
 * multiple M_DATA message blocks.  It also handles other slow path error conditions.
 */
static int
n_exdata_ind_slow(struct mtp *mtp, queue_t *q, mblk_t *mp, N_exdata_ind_t * p)
{
	mblk_t *dp, *newp = NULL;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (n_not_state(mtp, (NSF_DATA_XFER | NSF_WRES_RIND | NSF_WACK_RRES)))
		goto outstate;
	if (l_not_state(mtp, (LMF_ENABLED | LMF_DISABLE_PENDING)))
		goto outstate;
	if ((dp = mp->b_cont) == NULL)
		goto baddata;
	if (dp->b_cont != NULL) {
		/* We have chained M_DATA blocks: pull them up. */
		if (!pullupmsg(dp, -1)) {
			/* normally only fails because of buffer allocation failure */
			if (!(newp = msgpullup(dp, -1))) {
				mi_bufcall(q, xmsgsize(dp), BPRI_MED);
				return (-ENOBUFS);
			}
			dp = newp;
		}
	}
	if ((err = mtp_recv_msg(mtp, q, dp))) {
		if (newp)
			freemsg(newp);
		return (err);
	}
	if (newp == NULL)
		mp->b_cont = NULL;
	freemsg(mp);
	return (0);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      baddata:
	err = NBADDATA;
	goto error;
      error:
	return (err);
}

/**
 * n_exdata_ind: - process N_EXDATA_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @p: the primitive
 */
static int
n_exdata_ind(struct mtp *mtp, queue_t *q, mblk_t *mp, N_exdata_ind_t * p)
{
	mblk_t *dp;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto go_slow;
	if (unlikely(n_not_state(mtp, (NSF_DATA_XFER | NSF_WRES_RIND | NSF_WACK_RRES))))
		goto go_slow;
	if (unlikely(l_not_state(mtp, (LMF_ENABLED | LMF_DISABLE_PENDING))))
		goto go_slow;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto go_slow;
	if (unlikely(dp->b_cont != NULL))
		goto go_slow;
	if ((err = mtp_recv_msg(mtp, q, dp)) == 0)
		freeb(mp);
	return (err);
go_slow:
	return n_exdata_ind_slow(mtp, q, mp, p);
}

/**
 * n_info_ack: - process N_INFO_ACK primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @p: the primitive
 */
static int
n_info_ack(struct mtp *mtp, queue_t *q, mblk_t *mp, N_info_ack_t * p)
{
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	freemsg(mp);
	return (0);
}

/**
 * n_bind_ack: - process N_BIND_ACK primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @p: the primitive
 *
 * An N_BIND_ACK is returned as a result of an LMI_ATTACH_REQ operation.
 */
static int
n_bind_ack(struct mtp *mtp, queue_t *q, mblk_t *mp, N_bind_ack_t * p)
{
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (l_get_state(mtp) != LMI_ATTACH_PENDING)
		goto outstate;
	/* it is not really necessary to copy the bound address */
	return lmi_ok_ack(mtp, q, mp, LMI_ATTACH_REQ);
      outstate:
	err = NOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * n_error_ack: - process N_ERROR_ACK primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @p: the primitive
 */
static int
n_error_ack(struct mtp *mtp, queue_t *q, mblk_t *mp, N_error_ack_t * p)
{
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (p->ERROR_prim) {
	case N_BIND_REQ:
		if (l_get_state(mtp) != LMI_ATTACH_PENDING)
			goto outstate;
		return lmi_error_ack(mtp, q, mp, LMI_ATTACH_REQ, -EFAULT);
	case N_UNBIND_REQ:
		if (l_get_state(mtp) != LMI_DETACH_PENDING)
			goto outstate;
		return lmi_error_ack(mtp, q, mp, LMI_DETACH_REQ, -EFAULT);
	case N_CONN_REQ:
		if (l_get_state(mtp) != LMI_ENABLE_PENDING)
			goto outstate;
		return lmi_error_ack(mtp, q, mp, LMI_ENABLE_REQ, -EFAULT);
	case N_DISCON_REQ:
		if (l_get_state(mtp) != LMI_DISABLE_PENDING)
			goto outstate;

		return lmi_error_ack(mtp, q, mp, LMI_DISABLE_REQ, -EFAULT);
	case N_OPTMGMT_REQ:
		return lmi_error_ack(mtp, q, mp, LMI_OPTMGMT_REQ, -EFAULT);
	case N_INFO_REQ:
		return (-EFAULT);
	default:
		goto einval;
	}
	return (0);
      einval:
	err = -EINVAL;
	goto error;
      outstate:
	err = NOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * n_ok_ack: - process N_OK_ACK primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @p: the primitive
 */
static int
n_ok_ack(struct mtp *mtp, queue_t *q, mblk_t *mp, N_ok_ack_t * p)
{
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (p->CORRECT_prim) {
	case N_UNBIND_REQ:
		/* An ack of a bind request complets an LMI_DETACH_REQ operation. */
		if (l_get_state(mtp) != LMI_DETACH_PENDING)
			goto outstate;
		return lmi_ok_ack(mtp, q, mp, LMI_DETACH_REQ);
	case N_DISCON_REQ:
		/* An ack of a disconnect request completes an LMI_DISABLE_REQ operation */
		if (l_get_state(mtp) != LMI_DISABLE_PENDING)
			goto outstate;
		return lmi_disable_con(mtp, q, mp);
	case N_OPTMGMT_REQ:
		/* An ack of options management completes an LMI_OPTMGMT_REQ operation */
		return lmi_optmgmt_ack(mtp, q, mp, NULL, 0, LMI_SUCCESS);
	case N_RESET_RES:
		/* If a reset (SCTP restart) occurs, we ack it internally, handle the state change. */
		n_set_state(mtp, NS_DATA_XFER);
		break;
	case N_CONN_RES:
		/* We do not respond to connection indications. */
	default:
		break;
	}
	freemsg(mp);
	return (0);
      outstate:
	err = NOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * n_unitdata_ind: - process N_UNITDATA_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @p: the primitive
 */
static int
n_unitdata_ind(struct mtp *mtp, queue_t *q, mblk_t *mp, N_unitdata_ind_t * p)
{
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	goto notsupport;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * n_uderror_ind: - process N_UDERROR_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @p: the primitive
 */
static int
n_uderror_ind(struct mtp *mtp, queue_t *q, mblk_t *mp, N_uderror_ind_t * p)
{
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	goto notsupport;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * n_datack_ind: - process N_DATACK_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @p: the primitive
 */
static int
n_datack_ind(struct mtp *mtp, queue_t *q, mblk_t *mp, N_datack_ind_t * p)
{
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	goto notsupport;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * n_reset_ind: - process N_RESET_IND primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @p: the primitive
 *
 * NPI SCTP only sends N_RESET_IND when an association has restarted.  In that case, the other end
 * might have reset the M3UA state or might muddle on from a previous state.  If it is continuing,
 * all is well.  Otherwise, M3UA protocol semantics and timers should bring the AS down if there is
 * a problem.  So, we will ignore this situation for the moment.
 */
static int
n_reset_ind(struct mtp *mtp, queue_t *q, mblk_t *mp, N_reset_ind_t * p)
{
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (l_not_state(mtp, (LMF_ENABLED | LMF_UNUSABLE)))
		goto outstate;
	/* just ignore it */
	freemsg(mp);
	return (0);
      outstate:
	err = NOUTSTATE;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
error:
	return (err);
}

/**
 * n_reset_con: - process N_RESET_CON primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @p: the primitive
 *
 * SCTP NPI does not accept N_RESET_REQ so N_RESET_CON is not expected.
 */
static int
n_reset_con(struct mtp *mtp, queue_t *q, mblk_t *mp, N_reset_con_t * p)
{
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	goto notsupport;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * n_other_ind: - process unknown primitive
 * @mtp: MTP private structure
 * @q: active queue (read queue)
 * @mp: the message
 * @p: the primitive
 */
static int
n_other_ind(struct mtp *mtp, queue_t *q, mblk_t *mp, np_long *p)
{
	return (NNOTSUPPORT);
}

/*
 * STREAMS MESSAGE HANDLING
 * --------------------------------------------------------------------------
 */

/*
 * M_SIG and M_PCSIG HANDLING
 * --------------------------------------------------------------------------
 */

static int
__mtp_r_sig(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	int rtn;

	if (unlikely(!mi_timer_valid(mp)))
		return (0);

	switch (*(int *) mp->b_rptr) {
	case 1:
		LOGTO(mtp, "-> ASP Up TIMEOUT <-");
		rtn = mtp_aspup_tack_timeout(mtp, q);
		break;
	case 2:
		LOGTO(mtp, "-> ASP Active TIMEOUT <-");
		rtn = mtp_aspac_tack_timeout(mtp, q);
		break;
	case 3:
		LOGTO(mtp, "-> ASP Down TIMEOUT <-");
		rtn = mtp_aspdn_tack_timeout(mtp, q);
		break;
	case 4:
		LOGTO(mtp, "-> ASP Inactive TIMEOUT <-");
		rtn = mtp_aspia_tack_timeout(mtp, q);
		break;
	case 5:
		LOGTO(mtp, "-> Heartbeat TIMEOUT <-");
		rtn = mtp_hbeat_tack_timeout(mtp, q);
		break;
	default:
		LOGTO(mtp, "-> ??? TIMEOUT <-");
		rtn = (0);
		break;
	}
	if (rtn && !mi_timer_requeue(mp))
		rtn = 0;
	return (rtn);
}

noinline fastcall int
mtp_r_sig(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp;
	int err;

	if (likely(!!(mtp = (typeof(mtp)) mi_trylock(q)))) {
		err = __mtp_r_sig(mtp, q, mp);
		mi_unlock((caddr_t) mtp);
	} else
		err = -EDEADLK;
	return (err);
}

/*
 * M_PROTO and M_PCPROTO HANDLING
 * --------------------------------------------------------------------------
 */
noinline fastcall __unlikely int
mtp_w_proto_return(struct mtp *mtp, queue_t *q, mblk_t *mp, int err)
{
	if (err < 0)
		p_restore_state(mtp);
	switch (__builtin_expect(-err, EAGAIN)) {
	case EBUSY:
	case EAGAIN:
	case ENOMEM:
	case ENOBUFS:
	case EDEADLK:
		return (err);
	default:
		freemsg(mp);
		LOGNO(mtp, "Primitive <%s> on upper write queue generated error [%s]", mtp_primname(*(mtp_long *)mp->b_rptr), m_errname(err));
		/* fall through */
	case 0:
		return (0);
	case EPROTO:
	case EFAULT:
		return m_error(mtp, q, mp, -err);
	}
}
noinline fastcall __unlikely int
mtp_r_proto_return(struct mtp *mtp, queue_t *q, mblk_t *mp, int err)
{
	if (err < 0)
		p_restore_state(mtp);
	switch (__builtin_expect(-err, EAGAIN)) {
	case EBUSY:
	case EAGAIN:
	case ENOMEM:
	case ENOBUFS:
	case EDEADLK:
		return (err);
	default:
		freemsg(mp);
		LOGERR(mtp, "Primitive <%s> on lower read queue generated error [%s]", n_primname(*(np_long *)mp->b_rptr), n_errname(err));
		/* fall through */
	case 0:
		return (0);
	case EPROTO:
	case EFAULT:
		return m_error(mtp, q, mp, -err);
	}
}
static fastcall __hot_out int
__mtp_w_proto_slow(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	union MTP_primitives *p = (typeof(p)) mp->b_rptr;
	union LMI_primitives *l = (typeof(l)) mp->b_rptr;
	register mtp_long type = p->mtp_primitive;
	int err;

	p_save_state(mtp);
	if (unlikely(!MBLKIN(mp, 0, sizeof(type))))
		goto emsgsize;
	LOGRX(mtp, "-> %s", mtp_primname(type));
	switch (type) {
	case LMI_INFO_REQ:
		err = lmi_info_req(mtp, q, mp, &l->info_req);
		break;
	case LMI_ATTACH_REQ:
		err = lmi_attach_req(mtp, q, mp, &l->attach_req);
		break;
	case LMI_DETACH_REQ:
		err = lmi_detach_req(mtp, q, mp, &l->detach_req);
		break;
	case LMI_ENABLE_REQ:
		err = lmi_enable_req(mtp, q, mp, &l->enable_req);
		break;
	case LMI_DISABLE_REQ:
		err = lmi_disable_req(mtp, q, mp, &l->disable_req);
		break;
	case LMI_OPTMGMT_REQ:
		err = lmi_optmgmt_req(mtp, q, mp, &l->optmgmt_req);
		break;
	case MTP_BIND_REQ:
		err = mtp_bind_req(mtp, q, mp, &p->bind_req);
		break;
	case MTP_UNBIND_REQ:
		err = mtp_unbind_req(mtp, q, mp, &p->unbind_req);
		break;
	case MTP_CONN_REQ:
		err = mtp_conn_req(mtp, q, mp, &p->conn_req);
		break;
	case MTP_DISCON_REQ:
		err = mtp_discon_req(mtp, q, mp, &p->discon_req);
		break;
	case MTP_ADDR_REQ:
		err = mtp_addr_req(mtp, q, mp, &p->addr_req);
		break;
	case MTP_INFO_REQ:
		err = mtp_info_req(mtp, q, mp, &p->info_req);
		break;
	case MTP_OPTMGMT_REQ:
		err = mtp_optmgmt_req(mtp, q, mp, &p->optmgmt_req);
		break;
	case MTP_STATUS_REQ:
		err = mtp_status_req(mtp, q, mp, &p->status_req);
		break;
	case MTP_TRANSFER_REQ:
		err = mtp_transfer_req(mtp, q, mp, &p->transfer_req);
		break;
	default:
		err = mtp_other_req(mtp, q, mp, &p->mtp_primitive);
		break;
	}
	if (err)
		goto error;
	return (0);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	if (err < 0)
		p_restore_state(mtp);
	return (err);
}
static fastcall __hot_get int
__mtp_r_proto_slow(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	union N_primitives *p = (typeof(p)) mp->b_rptr;
	register np_long type = p->type;
	int err;

	p_save_state(mtp);
	if (unlikely(!MBLKIN(mp, 0, sizeof(type))))
		goto emsgsize;
	LOGRX(mtp, "%s <-", n_primname(type));
	switch (type) {
	case N_CONN_IND:
		err = n_conn_ind(mtp, q, mp, &p->conn_ind);
		break;
	case N_CONN_CON:
		err = n_conn_con(mtp, q, mp, &p->conn_con);
		break;
	case N_DISCON_IND:
		err = n_discon_ind(mtp, q, mp, &p->discon_ind);
		break;
	case N_DATA_IND:
		err = n_data_ind(mtp, q, mp, &p->data_ind);
		break;
	case N_EXDATA_IND:
		err = n_exdata_ind(mtp, q, mp, &p->exdata_ind);
		break;
	case N_INFO_ACK:
		err = n_info_ack(mtp, q, mp, &p->info_ack);
		break;
	case N_BIND_ACK:
		err = n_bind_ack(mtp, q, mp, &p->bind_ack);
		break;
	case N_ERROR_ACK:
		err = n_error_ack(mtp, q, mp, &p->error_ack);
		break;
	case N_OK_ACK:
		err = n_ok_ack(mtp, q, mp, &p->ok_ack);
		break;
	case N_UNITDATA_IND:
		err = n_unitdata_ind(mtp, q, mp, &p->unitdata_ind);
		break;
	case N_UDERROR_IND:
		err = n_uderror_ind(mtp, q, mp, &p->uderror_ind);
		break;
	case N_DATACK_IND:
		err = n_datack_ind(mtp, q, mp, &p->datack_ind);
		break;
	case N_RESET_IND:
		err = n_reset_ind(mtp, q, mp, &p->reset_ind);
		break;
	case N_RESET_CON:
		err = n_reset_con(mtp, q, mp, &p->reset_con);
		break;
	default:
		err = n_other_ind(mtp, q, mp, &p->type);
		break;
	}
	if (err)
		goto error;
	return (0);
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	if (err < 0)
		p_restore_state(mtp);
	return (err);
}
static inline fastcall __hot_out int
__mtp_w_proto(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	union MTP_primitives *p = (typeof(p)) mp->b_rptr;
	register mtp_long type = p->mtp_primitive;
	register int err;

	if (likely(type == MTP_TRANSFER_REQ)) {
		if (unlikely((err = mtp_transfer_req(mtp, q, mp, &p->transfer_req))))
			goto error;
	} else if (unlikely((err = __mtp_w_proto_slow(mtp, q, mp))))
		goto error;
	return (0);
      error:
	return mtp_w_proto_return(mtp, q, mp, err);
}
static inline fastcall __hot_get int
__mtp_r_proto(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	union N_primitives *p = (typeof(p)) mp->b_rptr;
	register np_long type = p->type;
	register int err;

	if ((1 << type) & ((1 << N_EXDATA_IND) | (1 << N_DATA_IND))) {
		/* fast paths for data */
		switch (__builtin_expect(type, N_DATA_IND)) {
		case N_DATA_IND:
			if (unlikely((err = n_data_ind(mtp, q, mp, &p->data_ind))))
				goto error;
			break;
		case N_EXDATA_IND:
			if (unlikely((err = n_exdata_ind(mtp, q, mp, &p->exdata_ind))))
				goto error;
			break;
		}
	} else if (unlikely((err = __mtp_r_proto_slow(mtp, q, mp))))
		goto error;
	return (0);
      error:
	return mtp_r_proto_return(mtp, q, mp, err);
}
noinline fastcall int
mtp_w_proto_now(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp;
	int err;

	if (likely(!!(mtp = (typeof(mtp)) mi_trylock(q)))) {
		err = __mtp_w_proto_slow(mtp, q, mp);
		mi_unlock((caddr_t) mtp);
	} else
		err = -EDEADLK;
	return (err);
}
noinline fastcall int
mtp_r_proto_now(queue_t *q, mblk_t *mp)
{
	struct mtp *mtp;
	int err;

	if (likely(!!(mtp = (typeof(mtp)) mi_trylock(q)))) {
		err = __mtp_r_proto_slow(mtp, q, mp);
		mi_unlock((caddr_t) mtp);
	} else
		err = -EDEADLK;
	return (err);
}

/**
 * mtp_w_proto: process M_PROTO or M_PCPROTO from put procedure
 * @q: active queue (upper write queue)
 * @mp: message to put
 *
 * Always placing normal (fast path) data on the queue improves performance from two perspectives:
 * it reduces lock contention by processing out of the service procedure; and processing from the
 * service procedure keeps data processing paths hot.
 */
static inline fastcall __hot_put int
mtp_w_proto(queue_t *q, mblk_t *mp)
{
	/* only process high priority messages immediately */
	if (unlikely(pcmsg(DB_TYPE(mp))))
		return mtp_w_proto_now(q, mp);
	/* always queue normal data for speed */
	return (-EAGAIN);
}

/**
 * mtp_r_proto: process M_PROTO or M_PCPROTO from put procedure
 * @q: active queue (lower read queue)
 * @mp: message to put
 *
 * Always placing normal (fast path) data on the queue improves performance from two perspectives:
 * it reduces lock contention by processing out of the service procedure; and processing from the
 * service procedure keeps data processing paths hot.
 */
static inline fastcall __hot_in int
mtp_r_proto(queue_t *q, mblk_t *mp)
{
	/* only process high priority messages immediately */
	if (unlikely(pcmsg(DB_TYPE(mp))))
		return mtp_r_proto_now(q, mp);
	/* always queue normal data for speed */
	return (-EAGAIN);
}

/*
 * M_DATA AND M_HPDATA HANDLING
 * --------------------------------------------------------------------------
 */
static inline fastcall __hot_out int
__mtp_w_data(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}
static inline fastcall __hot_read int
__mtp_r_data(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return(0);
}
static inline fastcall __hot_write int
mtp_w_data(queue_t *q, mblk_t *mp)
{
	/* only process high priority data immediately */
	if (unlikely(pcmsg(DB_TYPE(mp)))) {
		struct mtp *mtp;
		int err = -EDEADLK;

		if (likely(!!(mtp = (typeof(mtp)) mi_trylock(q)))) {
			err = __mtp_w_data(mtp, q, mp);
			mi_unlock((caddr_t) mtp);
		}
		return (err);
	}
	/* always queue normal data for speed */
	return (-EAGAIN);
}
static inline fastcall __hot_in int
mtp_r_data(queue_t *q, mblk_t *mp)
{
	/* only process high priority data immediately */
	if (unlikely(pcmsg(DB_TYPE(mp)))) {
		struct mtp *mtp;
		int err = -EDEADLK;

		if (likely(!!(mtp = (typeof(mtp)) mi_trylock(q)))) {
			err = __mtp_r_data(mtp, q, mp);
			mi_unlock((caddr_t) mtp);
		}
		return (err);
	}
	/* always queue normal data for speed */
	return (-EAGAIN);
}

/*
 * M_FLUSH HANDLING
 * --------------------------------------------------------------------------
 * Flush handling differs from basic modules in that there is a separation between
 * the lower NPI interface and the upper LMI/MTPI interface.  Therefore, we flush
 * like a multiplexing driver instead.
 */
noinline fastcall __unlikely int
mtp_w_flush(queue_t *q, mblk_t *mp)
{
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
noinline fastcall __unlikely int
mtp_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHR;
	}
	/* behave like the stream head */
	if ((mp->b_rptr[0] & FLUSHW) && !(mp->b_flag & MSGNOLOOP)) {
		mp->b_flag |= MSGNOLOOP;
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/*
 * OTHER STREAMS MESSAGE HANDLING
 * --------------------------------------------------------------------------
 * Canonical module unprocessed message handling.
 */
noinline fastcall int
mtp_w_other(queue_t *q, mblk_t *mp)
{
	/* pass unprocessed message types along */
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}
noinline fastcall int
mtp_r_other(queue_t *q, mblk_t *mp)
{
	/* pass unprocessed message types along */
	/* note that this includes M_HANGUP and M_ERROR message from below */
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 * STREAMS MESSAGE DISCRIMINATION
 * --------------------------------------------------------------------------
 */
static fastcall __hot_write int
mtp_wput_msg(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return mtp_w_proto(q, mp);
	case M_DATA:
	case M_HPDATA:
		return mtp_w_data(q, mp);
	case M_FLUSH:
		return mtp_w_flush(q, mp);
	default:
		return mtp_w_other(q, mp);
	}
}
static fastcall __hot_out int
mtp_wsrv_msg(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __mtp_w_proto(mtp, q, mp);
	case M_DATA:
	case M_HPDATA:
		return __mtp_w_data(mtp, q, mp);
	default:
		return mtp_w_other(q, mp);
	}
}
static fastcall __hot_read int
mtp_rsrv_msg(struct mtp *mtp, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __mtp_r_proto(mtp, q, mp);
	case M_DATA:
	case M_HPDATA:
		return __mtp_r_data(mtp, q, mp);
	case M_SIG:
	case M_PCSIG:
		return __mtp_r_sig(mtp, q, mp);
	default:
		return mtp_r_other(q, mp);
	}
}
static fastcall __hot_in int
mtp_rput_msg(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return mtp_r_proto(q, mp);
	case M_DATA:
	case M_HPDATA:
		return mtp_r_data(q, mp);
	case M_SIG:
	case M_PCSIG:
		return mtp_r_sig(q, mp);
	case M_FLUSH:
		return mtp_r_flush(q, mp);
	default:
		return mtp_r_other(q, mp);
	}
}

/*
 * QUEUE PUT AND SERVICE PROCEDURES
 * --------------------------------------------------------------------------
 */

/**
 * mtp_wput: - canonical high performance put procedure
 * @q: active queue (upper write queue)
 * @mp: message to put
 */
static streamscall __hot_write int
mtp_wput(queue_t *q, mblk_t *mp)
{
	if (unlikely(!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || unlikely(mtp_wput_msg(q, mp))) {
		mtp_wstat.ms_acnt++;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);	/* cannot fail */
		}
	}
	return (0);
}

/**
 * mtp_wsrv: - canonical high performance service procedure with locking
 * @q: active queue (upper write queue)
 */
static streamscall __hot_out int
mtp_wsrv(queue_t *q)
{
	struct mtp *mtp;
	mblk_t *mp;

	if (likely((!!(mtp = (typeof(mtp)) mi_trylock(q))))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(mtp_wsrv_msg(mtp, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;	/* must succeed */
					putbq(q, mp);
				}
				LOGTX(mtp, "write queue stalled");
				break;
			}
		}
		mi_unlock((caddr_t) mtp);
	}
	return (0);
}

/**
 * mtp_rsrv: - canonical high performance service procedure with locking
 * @q: active queue (lower read queue)
 */
static streamscall __hot_read int
mtp_rsrv(queue_t *q)
{
	struct mtp *mtp;
	mblk_t *mp;

	if (likely(!!(mtp = (typeof(mtp)) mi_trylock(q)))) {
		while (likely(!!(mp = getq(q)))) {
			if (unlikely(mtp_rsrv_msg(mtp, q, mp))) {
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;	/* must succeed */
					putbq(q, mp);
				}
				LOGTX(mtp, "read queue stalled");
				break;
			}
		}
		mi_unlock((caddr_t) mtp);
	}
	return (0);
}

/**
 * mtp_rput: - canonical high performance put procedure
 * @q: active queue (lower read queue)
 * @mp: message to put
 */
static streamscall __hot_in int
mtp_rput(queue_t *q, mblk_t *mp)
{
	if (unlikely(!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || unlikely(mtp_rput_msg(q, mp))) {
		mtp_rstat.ms_acnt++;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);	/* cannot fail */
		}
	}
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Open and Close Routines
 *
 *  -------------------------------------------------------------------------
 */
static caddr_t mtp_opens = NULL;

static streamscall __unlikely int
mtp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct mtp *mtp;
	mblk_t *mp, *tp;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */

	while (!(mp = allocb(sizeof(N_info_req_t), BPRI_WAITOK))) ;

	if ((err = mi_open_comm(&mtp_opens, sizeof(*mtp), q, devp, oflags, sflag, crp)))
		return (err);

	mtp = MTP_PRIV(q);
	/* initialize the structure */
	bzero(mtp, sizeof(*mtp));

	/* allocate timers */
	if (!(tp = mtp->aspup_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 1;
	if (!(tp = mtp->aspac_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 2;
	if (!(tp = mtp->aspdn_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 3;
	if (!(tp = mtp->aspia_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 4;
	if (!(tp = mtp->hbeat_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 5;
	if (!(tp = mtp->rreq_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 6;
	if (!(tp = mtp->dreq_tack = mi_timer_alloc_MAC(q, sizeof(int))))
		goto enobufs;
	*(int *) tp->b_rptr = 7;

	mtp->rq = RD(q);
	mtp->wq = WR(q);

	mtp->info.lmi.lmi_primitive = LMI_INFO_ACK;
	mtp->info.lmi.lmi_version = LMI_VERSION_2;
	mtp->info.lmi.lmi_state = LMI_UNATTACHED;
	mtp->info.lmi.lmi_max_sdu = 272;
	mtp->info.lmi.lmi_min_sdu = 5;
	mtp->info.lmi.lmi_header_len = 0;
	mtp->info.lmi.lmi_ppa_style = LMI_STYLE2;
	mtp->info.lmi.lmi_ppa_length = 0;
	mtp->info.lmi.lmi_ppa_offset = 0;
	mtp->info.lmi.lmi_prov_flags = 0;
	mtp->info.lmi.lmi_prov_state = 0;
	mtp->curr.l_state = LMI_UNATTACHED;
	mtp->save.l_state = LMI_UNATTACHED;

	mtp->info.mtp.mtp_primitive = MTP_INFO_ACK;
	mtp->info.mtp.mtp_msu_size = 272;
	mtp->info.mtp.mtp_addr_size = sizeof(mtp->orig);
	mtp->info.mtp.mtp_addr_length = 0;
	mtp->info.mtp.mtp_addr_offset = 0;
	mtp->info.mtp.mtp_current_state = MTPS_UNUSABLE;
	mtp->info.mtp.mtp_serv_type = M_CLMS;
	mtp->info.mtp.mtp_version = MTP_CURRENT_VERSION;
	mtp->curr.m_state = MTPS_UNUSABLE;
	mtp->save.m_state = MTPS_UNUSABLE;

	mtp->info.npi.PRIM_type = N_INFO_ACK;
	mtp->info.npi.NSDU_size = 512;
	mtp->info.npi.ENSDU_size = 512;
	mtp->info.npi.CDATA_size = -1;
	mtp->info.npi.DDATA_size = -1;
	mtp->info.npi.ADDR_size = sizeof(struct sockaddr_in);
	mtp->info.npi.ADDR_length = 0;
	mtp->info.npi.ADDR_offset = 0;
	mtp->info.npi.QOS_length = 0;
	mtp->info.npi.QOS_offset = 0;
	mtp->info.npi.QOS_range_length = 0;
	mtp->info.npi.QOS_range_offset = 0;
	mtp->info.npi.OPTIONS_flags = 0;
	mtp->info.npi.NIDU_size = 512;
	mtp->info.npi.SERV_type = N_CONS;
	mtp->info.npi.CURRENT_state = NS_UNBND;
	mtp->info.npi.PROVIDER_type = N_SUBNET;
	mtp->info.npi.PROTOID_length = 0;
	mtp->info.npi.PROTOID_offset = 0;
	mtp->info.npi.NPI_version = N_CURRENT_VERSION;
	mtp->curr.n_state = NS_UNBND;
	mtp->save.n_state = NS_UNBND;

	mtp->curr.a_state = ASP_DOWN;
	mtp->save.a_state = ASP_DOWN;

	/* issue an immediate information request */
	DB_TYPE(mp) = M_PCPROTO;
	((N_info_req_t *) mp->b_wptr)->PRIM_type = N_INFO_REQ;
	mp->b_wptr += sizeof(N_info_req_t);
	qprocson(q);
	putnext(q, mp);
	return (0);
      enobufs:
	err = ENOBUFS;
	goto error;
      error:
	freemsg(mp);
	/* cancel all timers */
	if ((tp = xchg(&mtp->aspup_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->aspdn_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->aspac_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->aspia_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->hbeat_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->rreq_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->dreq_tack, NULL)))
		mi_timer_free(tp);
	mi_close_comm(&mtp_opens, q);
	return (err);
}

static streamscall __unlikely int
mtp_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct mtp *mtp = MTP_PRIV(q);
	mblk_t *tp;

	qprocsoff(q);

	/* cancel all timers */
	if ((tp = xchg(&mtp->aspup_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->aspdn_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->aspac_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->aspia_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->hbeat_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->rreq_tack, NULL)))
		mi_timer_free(tp);
	if ((tp = xchg(&mtp->dreq_tack, NULL)))
		mi_timer_free(tp);

	mi_close_comm(&mtp_opens, q);
	return (0);
}

unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the M3UA-AS module. (0 for allocation.)");

static struct qinit mtp_rinit = {
	.qi_putp = mtp_rput,	/* Read put (message from below) */
	.qi_srvp = mtp_rsrv,	/* Read queue service */
	.qi_qclose = mtp_qclose,	/* Each open */
	.qi_qopen = mtp_qopen,	/* Last close */
	.qi_minfo = &mtp_minfo,	/* Information */
	.qi_mstat = &mtp_rstat,	/* Statistics */
};

static struct qinit mtp_winit = {
	.qi_putp = mtp_wput,	/* Write put (message from above) */
	.qi_srvp = mtp_wsrv,	/* Write queue service */
	.qi_minfo = &mtp_minfo,	/* Information */
	.qi_mstat = &mtp_wstat,	/* Statistics */
};

static struct streamtab m3ua_asinfo = {
	.st_rdinit = &mtp_rinit,	/* Upper read queue */
	.st_wrinit = &mtp_winit,	/* Upper write queue */
};

static struct fmodsw m3_fmod = {
	.f_name = MOD_NAME,
	.f_str = &m3ua_asinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
m3ua_asinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_SPLASH);
	if ((err = register_strmod(&m3_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module id %d\n", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
m3ua_asexit(void)
{
	int err;

	if ((err = unregister_strmod(&m3_fmod)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d\n", MOD_NAME, err);
	return;
}

module_init(m3ua_asinit);
module_exit(m3ua_asexit);
