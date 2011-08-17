/*****************************************************************************

 @(#) $RCSfile: tdm_ip.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2010-11-28 14:22:07 $

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

 Last Modified $Date: 2010-11-28 14:22:07 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tdm_ip.c,v $
 Revision 1.1.2.3  2010-11-28 14:22:07  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.2  2009-06-29 07:35:46  brian
 - SVR 4.2 => SVR 4.2 MP

 Revision 1.1.2.1  2009-06-21 11:40:36  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: tdm_ip.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2010-11-28 14:22:07 $";

/*
 *  This is a Y.1453 TDM-IP module.  It pushes over a UDP Stream that provides connectivity to the
 *  peer TDM-IP system.  The upper boundary service interface is the MXI interface.  The lower
 *  boundary service interface is the UDP-TPI interface.
 *
 *  In general, UDP Stream must be opened, options configured, bound to a local IP address and port
 *  number, and connected to a remote IP address and port number.  This module can then be pushed.
 *  Pushing the module will flush the Stream and any data messages received on the Stream will be
 *  discarded until the Stream is configured, enabled and connected.
 *
 *  Once the module is pushed, the MXI Stream can be linked under the MATRIX multiplexing driver and
 *  the channels available in the multiplex facility will be made available to the switching matrix.
 */

#include <sys/os8/compat.h>

#include <sys/tihdr.h>

#include <sys/xti.h>
#include <sys/xti_inet.h>

#include <sys/mxi.h>
#include <sys/mxi_ioctl.h>

#define TDM_IP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define TDM_IP_EXTRA		"Part of the OpenSS7 VoIP Stack for Linux Fast-STREAMS"
#define TDM_IP_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define TDM_IP_REVISION		"OpenSS7 $RCSfile: tdm_ip.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2010-11-28 14:22:07 $"
#define TDM_IP_DEVICE		"SVR 4.2 MP STREAMS TDM-IP MODULE"
#define TDM_IP_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define TDM_IP_LICENSE		"GPL"
#define TDM_IP_BANNER		TDM_IP_DESCRIP		"\n" \
				TDM_IP_EXTRA		"\n" \
				TDM_IP_REVISION		"\n" \
				TDM_IP_COPYRIGHT	"\n" \
				TDM_IP_DEVICE		"\n" \
				TDM_IP_CONTACT
#define TDM_IP_SPLASH		TDM_IP_DESCRIP		" - " \
				TDM_IP_REVISION

#if defined LINUX
MODULE_AUTHOR(TDM_IP_CONTACT);
MODULE_DESCRIPTION(TDM_IP_DESCRIP);
MODULE_SUPPORTED_DEVICE(TDM_IP_DEVICE);
#if defined MODULE_LICENSE
MODULE_LICENSE(TDM_IP_LICENSE);
#endif				/* defined MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-tdm-ip");
#endif				/* defined MODULE_ALIAS */
#if defined MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* defined MODULE_VERSION */
#endif				/* defined LINUX */

#define TDM_IP_MOD_ID		CONFIG_STREAMS_TDM_IP_MODID
#define TDM_IP_MOD_NAME		CONFIG_STREAMS_TDM_IP_NAME

#if defined LINUX
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(TDM_IP_MOD_ID));
MODULE_ALIAS("streams-module-tdm-ip");
#endif				/* defined MODULE_ALIAS */
#endif				/* defined LINUX */

#define MOD_ID		TDM_IP_MOD_ID
#define MOD_NAME	TDM_IP_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	TDM_IP_BANNER
#else				/* MODULE */
#define MOD_BANNER	TDM_IP_SPLASH
#endif				/* MODULE */

static inline const char *
mx_primname(const mx_ulong prim)
{
	switch (__builtin_expect(prim, MX_DATA_REQ)) {
	case MX_INFO_REQ:
		return ("MX_INFO_REQ");
	case MX_OPTMGMT_REQ:
		return ("MX_OPTMGMT_REQ");
	case MX_ATTACH_REQ:
		return ("MX_ATTACH_REQ");
	case MX_ENABLE_REQ:
		return ("MX_ENABLE_REQ");
	case MX_CONNECT_REQ:
		return ("MX_CONNECT_REQ");
	case MX_DATA_REQ:
		return ("MX_DATA_REQ");
	case MX_DISCONNECT_REQ:
		return ("MX_DISCONNECT_REQ");
	case MX_DISABLE_REQ:
		return ("MX_DISABLE_REQ");
	case MX_DETACH_REQ:
		return ("MX_DETACH_REQ");
	case MX_INFO_ACK:
		return ("MX_INFO_ACK");
	case MX_OPTMGMT_ACK:
		return ("MX_OPTMGMT_ACK");
	case MX_OK_ACK:
		return ("MX_OK_ACK");
	case MX_ERROR_ACK:
		return ("MX_ERROR_ACK");
	case MX_ENABLE_CON:
		return ("MX_ENABLE_CON");
	case MX_CONNECT_CON:
		return ("MX_CONNECT_CON");
	case MX_DATA_IND:
		return ("MX_DATA_IND");
	case MX_DISCONNECT_IND:
		return ("MX_DISCONNECT_IND");
	case MX_DISCONNECT_CON:
		return ("MX_DISCONNECT_CON");
	case MX_DISABLE_IND:
		return ("MX_DISABLE_IND");
	case MX_DISABLE_CON:
		return ("MX_DISABLE_CON");
	case MX_EVENT_IND:
		return ("MX_EVENT_IND");
	default:
		return ("(unknown)");
	}
}
static inline const char *
tp_primname(const t_uscalar_t prim)
{
	switch (__builtin_expect(prim, T_OPTDATA_IND)) {
	case T_CONN_REQ:
		return ("T_CONN_REQ");
	case T_CONN_RES:
		return ("T_CONN_RES");
	case T_DISCON_REQ:
		return ("T_DISCON_REQ");
	case T_DATA_REQ:
		return ("T_DATA_REQ");
	case T_EXDATA_REQ:
		return ("T_EXDATA_REQ");
	case T_INFO_REQ:
		return ("T_INFO_REQ");
	case T_BIND_REQ:
		return ("T_BIND_REQ");
	case T_UNBIND_REQ:
		return ("T_UNBIND_REQ");
	case T_UNITDATA_REQ:
		return ("T_UNITDATA_REQ");
	case T_OPTMGMT_REQ:
		return ("T_OPTMGMT_REQ");
	case T_ORDREL_REQ:
		return ("T_ORDREL_REQ");
	case T_OPTDATA_REQ:
		return ("T_OPTDATA_REQ");
	case T_ADDR_REQ:
		return ("T_ADDR_REQ");
	case T_CAPABILITY_REQ:
		return ("T_CAPABILITY_REQ");
	case T_CONN_IND:
		return ("T_CONN_IND");
	case T_CONN_CON:
		return ("T_CONN_CON");
	case T_DISCON_IND:
		return ("T_DISCON_IND");
	case T_DATA_IND:
		return ("T_DATA_IND");
	case T_EXDATA_IND:
		return ("T_EXDATA_IND");
	case T_INFO_ACK:
		return ("T_INFO_ACK");
	case T_BIND_ACK:
		return ("T_BIND_ACK");
	case T_ERROR_ACK:
		return ("T_ERROR_ACK");
	case T_OK_ACK:
		return ("T_OK_ACK");
	case T_UNITDATA_IND:
		return ("T_UNITDATA_IND");
	case T_UDERROR_IND:
		return ("T_UDERROR_IND");
	case T_OPTMGMT_ACK:
		return ("T_OPTMGMT_ACK");
	case T_ORDREL_IND:
		return ("T_ORDREL_IND");
	case T_OPTDATA_IND:
		return ("T_OPTDATA_IND");
	case T_ADDR_ACK:
		return ("T_ADDR_ACK");
	case T_CAPABILITY_ACK:
		return ("T_CAPABILITY_ACK");
	default:
		return ("(unknown)");
	}
}

static inline fastcall const char *
mx_statename(mx_ulong state)
{
	switch (__builtin_expect(state, MXS_CONNECTED)) {
	case MXS_UNINIT:
		return ("MXS_UNINIT");
	case MXS_UNUSABLE:
		return ("MXS_UNUSABLE");
	case MXS_DETACHED:
		return ("MXS_DETACHED");
	case MXS_WACK_AREQ:
		return ("MXS_WACK_AREQ");
	case MXS_WACK_UREQ:
		return ("MXS_WACK_UREQ");
	case MXS_ATTACHED:
		return ("MXS_ATTACHED");
	case MXS_WACK_EREQ:
		return ("MXS_WACK_EREQ");
	case MXS_WCON_EREQ:
		return ("MXS_WCON_EREQ");
	case MXS_WACK_RREQ:
		return ("MXS_WACK_RREQ");
	case MXS_WCON_RREQ:
		return ("MXS_WCON_RREQ");
	case MXS_ENABLED:
		return ("MXS_ENABLED");
	case MXS_WACK_CREQ:
		return ("MXS_WACK_CREQ");
	case MXS_WCON_CREQ:
		return ("MXS_WCON_CREQ");
	case MXS_WACK_DREQ:
		return ("MXS_WACK_DREQ");
	case MXS_WCON_DREQ:
		return ("MXS_WCON_DREQ");
	case MXS_CONNECTED:
		return ("MXS_CONNECTED");
	default:
		return ("(unknown)");
	}
}
static inline fastcall const char *
tp_statename(t_uscalar_t state)
{
	switch (__builtin_expect(state, TS_DATA_XFER)) {
	case TS_UNBND:
		return ("TS_UNBND");
	case TS_WACK_BREQ:
		return ("TS_WACK_BREQ");
	case TS_WACK_UREQ:
		return ("TS_WACK_UREQ");
	case TS_IDLE:
		return ("TS_IDLE");
	case TS_WACK_OPTREQ:
		return ("TS_WACK_OPTREQ");
	case TS_WACK_CREQ:
		return ("TS_WACK_CREQ");
	case TS_WCON_CREQ:
		return ("TS_WCON_CREQ");
	case TS_WRES_CIND:
		return ("TS_WRES_CIND");
	case TS_WACK_CRES:
		return ("TS_WACK_CRES");
	case TS_DATA_XFER:
		return ("TS_DATA_XFER");
	case TS_WIND_ORDREL:
		return ("TS_WIND_ORDREL");
	case TS_WREQ_ORDREL:
		return ("TS_WREQ_ORDREL");
	case TS_WACK_DREQ6:
		return ("TS_WACK_DREQ6");
	case TS_WACK_DREQ7:
		return ("TS_WACK_DREQ7");
	case TS_WACK_DREQ9:
		return ("TS_WACK_DREQ9");
	case TS_WACK_DREQ10:
		return ("TS_WACK_DREQ10");
	case TS_WACK_DREQ11:
		return ("TS_WACK_DREQ11");
	case TS_NOSTATES:
		return ("TS_NOSTATES");
	default:
		return ("(unknown)");
	}
}

static inline fastcall const char *
mx_errname(const mx_long error)
{
	switch (error) {
	case MXSYSERR:
		return ("MXSYSERR");
	case MXBADADDR:
		return ("MXBADADDR");
	case MXOUTSTATE:
		return ("MXOUTSTATE");
	case MXBADOPT:
		return ("MXBADOPT");
	case MXBADPARM:
		return ("MXBADPARM");
	case MXBADPARMTYPE:
		return ("MXBADPARMTYPE");
	case MXBADFLAG:
		return ("MXBADFLAG");
	case MXBADPRIM:
		return ("MXBADPRIM");
	case MXNOTSUPP:
		return ("MXNOTSUPP");
	case MXBADSLOT:
		return ("MXBADSLOT");
	case -EMSGSIZE:
		return ("EMSGSIZE");
	case -EINVAL:
		return ("EINVAL");
	case -EPROTO:
		return ("EPROTO");
	case -EFAULT:
		return ("EFAULT");
	case -EAGAIN:
		return ("EAGAIN");
	case -ENOMEM:
		return ("ENOMEM");
	case -ENOBUFS:
		return ("ENOBUFS");
	case -EDEADLK:
		return ("EDEADLK");
	case -EBUSY:
		return ("EBUSY");
	default:
		return ("(unknown)");
	}
}

static inline fastcall const char *
tp_errname(const t_scalar_t error)
{
	switch (error) {
	case TBADADDR:
		return ("TBADADDR");
	case TBADOPT:
		return ("TBADOPT");
	case TACCES:
		return ("TACCES");
	case TBADF:
		return ("TBADF");
	case TNOADDR:
		return ("TNOADDR");
	case TOUTSTATE:
		return ("TOUTSTATE");
	case TBADSEQ:
		return ("TBADSEQ");
	case TSYSERR:
		return ("TSYSERR");
	case TLOOK:
		return ("TLOOK");
	case TBADDATA:
		return ("TBADDATA");
	case TBUFOVFLW:
		return ("TBUFOVFLW");
	case TFLOW:
		return ("TFLOW");
	case TNODATA:
		return ("TNODATA");
	case TNODIS:
		return ("TNODIS");
	case TNOUDERR:
		return ("TNOUDERR");
	case TBADFLAG:
		return ("TBADFLAG");
	case TNOREL:
		return ("TNOREL");
	case TNOTSUPPORT:
		return ("TNOTSUPPORT");
	case TSTATECHNG:
		return ("TSTATECHNG");
	case TNOSTRUCTYPE:
		return ("TNOSTRUCTYPE");
	case TBADNAME:
		return ("TBADNAME");
	case TBADQLEN:
		return ("TBADQLEN");
	case TADDRBUSY:
		return ("TADDRBUSY");
	case TINDOUT:
		return ("TINDOUT");
	case TPROVMISMATCH:
		return ("TPROVMISMATCH");
	case TRESQLEN:
		return ("TRESQLEN");
	case TRESADDR:
		return ("TRESADDR");
	case TQFULL:
		return ("TQFULL");
	case TPROTO:
		return ("TPROTO");
	case -EMSGSIZE:
		return ("EMSGSIZE");
	case -EINVAL:
		return ("EINVAL");
	case -EPROTO:
		return ("EPROTO");
	case -EFAULT:
		return ("EFAULT");
	case -EAGAIN:
		return ("EAGAIN");
	case -ENOMEM:
		return ("ENOMEM");
	case -ENOBUFS:
		return ("ENOBUFS");
	case -EDEADLK:
		return ("EDEADLK");
	case -EBUSY:
		return ("EBUSY");
	default:
		return ("(unknown)");
	}
}

static inline fastcall const char *
mx_strerror(const mx_long error)
{
	switch (error) {
	case MXSYSERR:
		return ("UNIX system error");
	case MXBADADDR:
		return ("bad address format or content");
	case MXOUTSTATE:
		return ("interface out of state");
	case MXBADOPT:
		return ("bad options format or content");
	case MXBADPARM:
		return ("bad parameter format or content");
	case MXBADPARMTYPE:
		return ("bad paramater structure type");
	case MXBADFLAG:
		return ("bad flag");
	case MXBADPRIM:
		return ("bad primitive");
	case MXNOTSUPP:
		return ("primitive not supported");
	case MXBADSLOT:
		return ("bad multplex slot");
	case -EMSGSIZE:
		return ("invalid message size");
	case -EINVAL:
		return ("invalid argument");
	case -EPROTO:
		return ("protocol error");
	case -EFAULT:
		return ("segmentation violation");
	case -EAGAIN:
		return ("would block");
	case -ENOMEM:
		return ("out of memory");
	case -ENOBUFS:
		return ("out of buffers");
	case -EDEADLK:
		return ("deadlock avoidance");
	case -EBUSY:
		return ("resource busy");
	default:
		return ("(unknown)");
	}
}

static inline fastcall const char *
tp_strerror(const t_scalar_t error)
{
	switch (error) {
	case TBADADDR:
		return ("bad address format");
	case TBADOPT:
		return ("bad options format");
	case TACCES:
		return ("bad permissions");
	case TBADF:
		return ("bad file descriptor");
	case TNOADDR:
		return ("unable to allocate an address");
	case TOUTSTATE:
		return ("would place interface out of state");
	case TBADSEQ:
		return ("bad call sequence number");
	case TSYSERR:
		return ("system error");
	case TLOOK:
		return ("has to t_look() for event");
	case TBADDATA:
		return ("bad amount of data");
	case TBUFOVFLW:
		return ("buffer was too small");
	case TFLOW:
		return ("would block due to flow control");
	case TNODATA:
		return ("no data indication");
	case TNODIS:
		return ("no disconnect indication");
	case TNOUDERR:
		return ("no unitdata error indication");
	case TBADFLAG:
		return ("bad flags");
	case TNOREL:
		return ("no orderly release indication");
	case TNOTSUPPORT:
		return ("not supported");
	case TSTATECHNG:
		return ("state is currently changing");
	case TNOSTRUCTYPE:
		return ("structure type not supported");
	case TBADNAME:
		return ("bad transport provider name");
	case TBADQLEN:
		return ("listener queue length limit is zero");
	case TADDRBUSY:
		return ("address already in use");
	case TINDOUT:
		return ("outstanding connect indications");
	case TPROVMISMATCH:
		return ("not same transport provider");
	case TRESQLEN:
		return ("connection acceptor has qlen > 0");
	case TRESADDR:
		return ("conn. acceptor bound to different address");
	case TQFULL:
		return ("connection indicator queue is full");
	case TPROTO:
		return ("protocol error");
	case -EMSGSIZE:
		return ("invalid message size");
	case -EINVAL:
		return ("invalid argument");
	case -EPROTO:
		return ("protocol error");
	case -EFAULT:
		return ("segmentation violation");
	case -EAGAIN:
		return ("would block");
	case -ENOMEM:
		return ("out of memory");
	case -ENOBUFS:
		return ("out of buffers");
	case -EDEADLK:
		return ("deadlock avoidance");
	case -EBUSY:
		return ("resource busy");
	default:
		return ("(unknown)");
	}
}

/*
 * STATE CHANGES
 */

static inline fastcall mx_ulong
mx_get_state(struct tdm *tdm)
{
	return tdm->mx.state.state;
}
static inline fastcall mx_ulong
mx_set_state(struct tdm *tdm, const mx_ulong newstate)
{
	const mx_ulong oldstate = mx_get_state(tdm);

	if (newstate != oldstate) {
		mi_strlog(tdm->wq, STRLOGST, SL_TRACE, "%s <- %s", mx_statename(newstate),
			  mx_statename(oldstate));
		tdm->mx.state.state = newstate;
		tdm->mx.info.mx_state = newstate;
	}
	return (newstate);
}
static inline fastcall mx_ulong
mx_save_state(struct tdm *tdm)
{
	tdm->mx.oldstate = tdm->mx.state;
	return (tdm->mx.state.state);
}
static inline fastcall mx_ulong
mx_restore_state(struct tdm *tdm)
{
	tdm->mx.state = tdm->mx.oldstate;
	return (tdm->mx.info.mx_state = tdm->mx.state.state);
}

static inline fastcall t_uscalar_t
tp_get_state(struct tdm *tdm)
{
	return tdm->tp.state.state;
}
static inline fastcall t_uscalar_t
tp_set_state(struct tdm *tdm, const t_uscalar_t newstate)
{
	const t_uscalar_t oldstate = tp_get_state(tdm);

	if (newstate != oldstate) {
		mi_strlog(tdm->rq, STRLOGST, SL_TRACE, "%s <- %s", tp_statename(newstate),
			  tp_statename(oldstate));
		tdm->tp.state.state = newstate;
		tdm->tp.info.INFO_ack.CURRENT_state = newstate;
	}
	return (newstate);
}
static inline fastcall t_uscalar_t
tp_save_state(struct tdm *tdm)
{
	tdm->tp.oldstate = tdm->tp.state;
	return (tdm->tp.state.state);
}
static inline fastcall t_uscalar_t
tp_restore_state(struct tdm *tdm)
{
	tdm->tp.state = tdm->tp.oldstate;
	return (tdm->tp.info.INFO_ack.CURRENT_state = tdm->tp.state.state);
}

/*
 * MESSAGE LOGGING
 */

static inline fastcall void
tdm_stripmsg(mblk_t *msg, mblk_t *mp)
{
	mblk_t *b, *b_next = msg;

	while ((b = b_next) && b != mp->b_cont) {
		b_next = b->b_next;
		freeb(b);
	}
}

static inline fastcall int
mx_txprim(struct tdm *tdm, queue_t *q, mblk_t *msg, mblk_t *mp, mx_ulong prim)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(tdm->rq, mp->b_band))) {
#ifdef _DEBUG
		int level;

		switch (__builtin_expect(prim, MX_DATA_REQ)) {
		case MX_DATA_REQ:
			level = STRLOGDA;
			break;
		default:
			level = STRLOGTX;
			break;
		}
		mi_strlog(tdm->rq, level, SL_TRACE, "<- %s", mx_primname(prim));
#endif				/* _DEBUG */
		tdm_stripmsg(msg);
		putnext(tdm->rq, mp);
		return (0);
	}
	freeb(mp);
	return (-EBUSY);
}
static inline fastcall int
tp_txprim(struct tdm *tdm, queue_t *q, mblk_t *msg, mblk_t *mp, t_uscalar_t prim)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(tdm->wq, mp->b_band))) {
#ifdef _DEBUG
		int level;

		switch (__builtin_expect(prim, T_DATA_REQ)) {
		case T_DATA_REQ:
		case T_EXDATA_REQ:
		case T_OPTDATA_REQ:
		case T_UNITDATA_REQ:
			level = STRLOGDA;
			break;
		default:
			level = STRLOGTX;
			break;
		}
		mi_strlog(tdm->wq, level, SL_TRACE, "%s ->", tp_primname(prim));
#endif				/* _DEBUG */
		tdm_stripmsg(msg);
		putnext(tdm->wq, mp);
		return (0);
	}
	freeb(mp);
	return (-EBUSY);
}

static inline fastcall void
mx_rxprim(struct tdm *tdm, queue_t *q, mblk_t *mp, mx_ulong prim)
{
#ifdef _DEBUG
	int level;

	switch (__builtin_expect(prim, MX_DATA_IND)) {
	case MX_DATA_IND:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGRX;
		break;
	}
	mi_strlog(q, level, SL_TRACE, "-> %s", mx_primname(prim));
#endif				/* _DEBUG */
}
static inline fastcall void
tp_rxprim(struct tdm *tdm, queue_t *q, mblk_t *mp, t_uscalar_t prim)
{
#ifdef _DEBUG
	int level;

	switch (__builtin_expect(prim, T_DATA_IND)) {
	case T_DATA_IND:
	case T_EXDATA_IND:
	case T_OPTDATA_IND:
	case T_UNITDATA_IND:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGTX;
		break;
	}
	mi_strlog(q, level, SL_TRACE, "%s <-", tp_primname(prim));
#endif				/* _DEBUG */
}

/*
 * MXI PROVIDER TO MXI USER PRIMITIVES (TO ABOVE)
 */

static inline fastcall int
mx_info_ack(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct MX_info_ack *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(tdm, q, msg, mp, MX_INFO_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_optmgmt_ack(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct MX_info_ack *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(tdm, q, msg, mp, MX_INFO_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_ok_ack(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct MX_info_ack *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(tdm, q, msg, mp, MX_INFO_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_error_ack(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct MX_info_ack *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(tdm, q, msg, mp, MX_INFO_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_enable_con(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct MX_info_ack *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(tdm, q, msg, mp, MX_INFO_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_connect_con(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct MX_info_ack *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(tdm, q, msg, mp, MX_INFO_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_data_ind(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct MX_info_ack *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(tdm, q, msg, mp, MX_INFO_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_disconnect_ind(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct MX_info_ack *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(tdm, q, msg, mp, MX_INFO_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_disconnect_con(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct MX_info_ack *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(tdm, q, msg, mp, MX_INFO_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_disable_ind(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct MX_info_ack *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(tdm, q, msg, mp, MX_INFO_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_disable_con(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct MX_info_ack *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(tdm, q, msg, mp, MX_INFO_ACK);
	}
	return (-ENOBUFS);
}
static inline fastcall int
mx_event_ind(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct MX_info_ack *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		return mx_txprim(tdm, q, msg, mp, MX_INFO_ACK);
	}
	return (-ENOBUFS);
}

noinline fastcall __unlikely int
mx_error_reply(struct tdm *tdm, queue_t *q, mblk_t *msg, mx_ulong prim, mx_long error)
{
	int err;

	switch (error) {
	case 0:
		freemsg(msg);
		return (0);
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
	case -EDEADLK:
	case -EBUSY:
		return (error);
	}
	switch (prim) {
	case MX_DATA_REQ:
	default:
		goto m_error;
	case MX_INFO_REQ:
	case MX_OPTMGMT_REQ:
	case MX_ATTACH_REQ:
	case MX_ENABLE_REQ:
	case MX_CONNECT_REQ:
	case MX_DISCONNECT_REQ:
	case MX_DISABLE_REQ:
	case MX_DETACH_REQ:
		break;
	}
	mx_restore_state(tdm);
	if ((err = mx_error_ack(tdm, q, msg, prim, error)))
		return (err);
	mi_strlog(tdm->wq, STRLOGERR, SL_TRACE | SL_ERROR, "%s: error %s: %s",
		  mx_primname(prim), mx_errname(error), mx_strerror(error));
	return (0);
      m_error:
	mx_restore_state(tdm);
	if ((err = m_error(tdm, q, msg, EPROTO, EPROTO)))
		return (err);
	mx_set_state(tdm, MXS_UNUSABLE);
	mi_strlog(tdm->wq, STRLOGDA, SL_TRACE | SL_ERROR, "%s: error %s: %s",
		  mx_primname(prim), mx_errname(error), mx_strerror(error));
	return (err);
}

/*
 * TPI USER TO TPI PROVIDER PRIMITIVES (TO BELOW)
 */

static inline fastcall int
t_conn_req(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct T_conn_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_REQ;
		mp->b_wptr += sizeof(*p);
		return tp_txprim(tdm, q, msg, mp, T_CONN_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
t_conn_res(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct T_conn_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_REQ;
		mp->b_wptr += sizeof(*p);
		return tp_txprim(tdm, q, msg, mp, T_CONN_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
t_discon_req(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct T_conn_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_REQ;
		mp->b_wptr += sizeof(*p);
		return tp_txprim(tdm, q, msg, mp, T_CONN_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
t_data_req(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct T_conn_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_REQ;
		mp->b_wptr += sizeof(*p);
		return tp_txprim(tdm, q, msg, mp, T_CONN_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
t_exdata_req(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct T_conn_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_REQ;
		mp->b_wptr += sizeof(*p);
		return tp_txprim(tdm, q, msg, mp, T_CONN_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
t_info_req(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct T_conn_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_REQ;
		mp->b_wptr += sizeof(*p);
		return tp_txprim(tdm, q, msg, mp, T_CONN_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
t_bind_req(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct T_conn_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_REQ;
		mp->b_wptr += sizeof(*p);
		return tp_txprim(tdm, q, msg, mp, T_CONN_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
t_unbind_req(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct T_conn_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_REQ;
		mp->b_wptr += sizeof(*p);
		return tp_txprim(tdm, q, msg, mp, T_CONN_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
t_unitdata_req(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct T_conn_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_REQ;
		mp->b_wptr += sizeof(*p);
		return tp_txprim(tdm, q, msg, mp, T_CONN_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
t_optmgmt_req(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct T_conn_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_REQ;
		mp->b_wptr += sizeof(*p);
		return tp_txprim(tdm, q, msg, mp, T_CONN_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
t_ordrel_req(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct T_conn_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_REQ;
		mp->b_wptr += sizeof(*p);
		return tp_txprim(tdm, q, msg, mp, T_CONN_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
t_optdata_req(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct T_conn_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_REQ;
		mp->b_wptr += sizeof(*p);
		return tp_txprim(tdm, q, msg, mp, T_CONN_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
t_addr_req(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct T_conn_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_REQ;
		mp->b_wptr += sizeof(*p);
		return tp_txprim(tdm, q, msg, mp, T_CONN_REQ);
	}
	return (-ENOBUFS);
}
static inline fastcall int
t_capability_req(struct tdm *tdm, queue_t *q, mblk_t *msg)
{
	struct T_conn_req *p;
	mblk_t *mp;
	size_t mlen = sizeof(*p);

	if (likely(!!(mp = mi_allocb(q, mlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_REQ;
		mp->b_wptr += sizeof(*p);
		return tp_txprim(tdm, q, msg, mp, T_CONN_REQ);
	}
	return (-ENOBUFS);
}

noinline fastcall __unlikely int
tp_error_reply(struct tdm *tdm, queue_t *q, mblk_t *msg, t_uscalar_t prim, t_scalar_t error)
{
	int err, level;

	switch (error) {
	case 0:
		freemsg(msg);
		return (0);
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
	case -EDEADLK:
	case -EBUSY:
		return (error);
	}
	tp_restore_state(tdm);
	if ((err = m_error(tdm, q, msg, EPROTO, EPROTO)))
		return (err);
	switch (prim) {
	case T_DATA_REQ:
	case T_EXDATA_REQ:
	case T_OPTDATA_REQ:
	case T_UNITDATA_REQ:
		level = STRLOGDA;
		break;
	default:
		level = STRLOGERR;
		break;
	}
	mi_strlog(tdm->rq, level, SL_TRACE | SL_ERROR, "%s: error %s: %s",
		  tp_primname(prim), tp_errname(error), tp_strerror(error));
	tp_set_state(tdm, -1);
	return (0);
}


/*
 * MXI USER TO MXI PROVIDER PRIMITIVES (FROM ABOVE)
 */

noinline __unlikely int
mx_info_req(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct MX_info_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = MXBADPRIM;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
mx_optmgmt_req(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct MX_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = MXBADPRIM;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
mx_attach_req(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct MX_attach_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = MXBADPRIM;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
mx_enable_req(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct MX_enable_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = MXBADPRIM;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
mx_connect_req(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct MX_connect_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = MXBADPRIM;
	goto error;
      error:
	return (err);
}
static inline __hot_put int
mx_data_req(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct MX_data_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = MXBADPRIM;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
mx_disconnect_req(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct MX_disconnect_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = MXBADPRIM;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
mx_disable_req(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct MX_disable_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = MXBADPRIM;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
mx_detach_req(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct MX_detach_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = MXBADPRIM;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
mx_other_req(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	mx_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = MXNOTSUPP;
	goto error;
      tooshort:
	err = MXBADPRIM;
	goto error;
      error:
	return (err);
}

/*
 * TPI PROVIDER TO TPI USER PRIMITIVES (FROM BELOW)
 */

noinline __unlikely int
t_conn_ind(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct T_conn_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
t_conn_con(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct T_conn_con *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
t_discon_ind(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct T_discon_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static inline __hot_get int
t_data_ind(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct T_data_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static inline __hot_get int
t_exdata_ind(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct T_exdata_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
t_info_ack(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct T_info_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
t_bind_ack(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct T_bind_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
t_error_ack(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct T_error_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
t_ok_ack(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct T_ok_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static inline __hot_get int
t_unitdata_ind(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct T_unitdata_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
t_uderror_ind(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct T_uderror_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
t_optmgmt_ack(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct T_optmgmt_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
t_ordrel_ind(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct T_ordrel_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
static inline __hot_get int
t_optdata_ind(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct T_optdata_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
t_addr_ack(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct T_addr_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
t_capability_ack(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct T_capability_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}
noinline __unlikely int
t_other_ind(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	t_scalar_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto tooshort;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      tooshort:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

static int
tdm_w_data(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
}
static int
tdm_r_data(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
}

static int
tdm_w_proto(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	mx_ulong prim;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;
	prim = *(typeof(prim) *) mp->b_rtpr;
	mx_save_state(tdm);
	mx_rxprim(tdm, q, mp, prim);
	switch (__builtin_expect(prim, MX_DATA_REQ)) {
	case MX_INFO_REQ:
		err = mx_info_req(tdm, q, mp);
		break;
	case MX_OPTMGMT_REQ:
		err = mx_optmgmt_req(tdm, q, mp);
		break;
	case MX_ATTACH_REQ:
		err = mx_attach_req(tdm, q, mp);
		break;
	case MX_ENABLE_REQ:
		err = mx_enable_req(tdm, q, mp);
		break;
	case MX_CONNECT_REQ:
		err = mx_connect_req(tdm, q, mp);
		break;
	case MX_DATA_REQ:
		err = mx_data_req(tdm, q, mp);
		break;
	case MX_DISCONNECT_REQ:
		err = mx_disconnect_req(tdm, q, mp);
		break;
	case MX_DISABLE_REQ:
		err = mx_disable_req(tdm, q, mp);
		break;
	case MX_DETACH_REQ:
		err = mx_detach_req(tdm, q, mp);
		break;
	default:
		err = mx_other_req(tdm, q, mp);
		break;
	}
	if (likely(err == 0))
		return (0);
	mx_restore_state(tdm);
      out:
	return mx_error_reply(tdm, q, mp, prim, err);
      tooshort:
	err = -EMSGSIZE;
	goto out;
}
static int
tdm_r_proto(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	t_uscalar_t prim;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto tooshort;
	prim = *(typeof(prim) *) mp->b_rptr;
	tp_save_state(tdm);
	tp_rxprim(tdm, q, mp, prim);
	switch (__builtin_expect(prim, T_DATA_IND)) {
	case T_CONN_IND:
		err = t_conn_ind(tdm, q, mp);
		break;
	case T_CONN_CON:
		err = t_conn_con(tdm, q, mp);
		break;
	case T_DISCON_IND:
		err = t_discon_ind(tdm, q, mp);
		break;
	case T_DATA_IND:
		err = t_data_ind(tdm, q, mp);
		break;
	case T_EXDATA_IND:
		err = t_exdata_ind(tdm, q, mp);
		break;
	case T_INFO_ACK:
		err = t_info_ack(tdm, q, mp);
		break;
	case T_BIND_ACK:
		err = t_bind_ack(tdm, q, mp);
		break;
	case T_ERROR_ACK:
		err = t_error_ack(tdm, q, mp);
		break;
	case T_OK_ACK:
		err = t_ok_ack(tdm, q, mp);
		break;
	case T_UNITDATA_IND:
		err = t_unitdata_ind(tdm, q, mp);
		break;
	case T_UDERROR_IND:
		err = t_uderror_ind(tdm, q, mp);
		break;
	case T_OPTMGMT_ACK:
		err = t_optmgmt_ack(tdm, q, mp);
		break;
	case T_ORDREL_IND:
		err = t_ordrel_ind(tdm, q, mp);
		break;
	case T_OPTDATA_IND:
		err = t_optdata_ind(tdm, q, mp);
		break;
	case T_ADDR_ACK:
		err = t_addr_ack(tdm, q, mp);
		break;
	case T_CAPABILITY_ACK:
		err = t_capability_ack(tdm, q, mp);
		break;
	default:
		err = t_other_ind(tdm, q, mp);
		break;
	}
	if (likely(err == 0))
		return (0);
	tp_restore_state(tdm);
      out:
	return tp_error_reply(tdm, q, mp, prim, err);
      tooshort:
	err = -EMSGSIZE;
	goto out;
}

static int
tdm_w_ctl(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	if (MBLKIN(mp, 0, sizeof(*ioc)) && _IOC_TYPE(ioc->ioc_cmd) == TDM_IP_IOC_MAGIC) {
		/* FIXME: process these */
		freemsg(mp);
		return (0);
	}
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}
static int
tdm_r_ctl(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	/* FIXME: process ones we sent */
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static int
tdm_w_ioctl(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	if (MBLKIN(mp, 0, sizeof(*ioc)) && _IOC_TYPE(ioc->ioc_cmd) == TDM_IP_IOC_MAGIC) {
		/* FIXME: process these */
		freemsg(mp);
		return (0);
	}
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}
static int
tdm_r_ioctl(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

static int
m_w_flush(queue_t *q, mblk_t *mp)
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
static int
m_r_flush(queue_t *q, mblk_t *mp)
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

static int
m_r_error(queue_t *q, mblk_t *mp)
{
	/* FIXME process these */
	freemsg(mp);
	return (0);
}

static int
m_w_other(queue_t *q, mblk_t *mp)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}
static int
m_r_other(queue_t *q, mblk_t *mp)
{
	if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static int
tdm_wsrv_msg(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		return tdm_w_data(tdm, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tdm_w_proto(tdm, q, mp);
	case M_CTL:
	case M_PCCTL:
		return tdm_w_ctl(tdm, q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return tdm_w_ioctl(tdm, q, mp);
#if 0
	case M_FLUSH:
		return m_w_flush(q, mp);
#endif
	default:
		/* MUST NOT HAPPEN */
		return m_w_other(q, mp);
	}
}
static int
tdm_rsrv_msg(struct tdm *tdm, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_DATA)) {
	case M_DATA:
	case M_HPDATA:
		return tdm_r_data(tdm, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tdm_r_proto(tdm, q, mp);
	case M_CTL:
	case M_PCCTL:
		return tdm_r_ctl(tdm, q, mp);
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		return tdm_r_ioctl(tdm, q, mp);
#if 0
	case M_ERROR:
	case M_HANGUP:
		return m_r_error(q, mp);
	case M_FLUSH:
		return m_r_flush(q, mp);
#endif
	default:
		return m_r_other(q, mp);
	}
}

static int
tdm_wput_msg(queue_t *q, mblk_t *mp)
{
	unsigned char type;

	switch (__builtin_expect((type = DB_TYPE(mp)), M_DATA)) {
	case M_DATA:
	case M_PROTO:
	case M_CTL:
	case M_IOCTL:
		return (-EAGAIN);	/* for speed */
	case M_HPDATA:
	case M_PCPROTO:
	case M_PCCTL:
	case M_IOCDATA:
	{
		struct tdm *tdm;
		int err;

		if (unlikely(!(tdm = (typeof(tdm)) mi_trylock(q))))
			return (-EDEADLK);
		switch (type) {
		case M_HPDATA:
			err = tdm_w_data(tdm, q, mp);
			break;
		case M_PCPROTO:
			err = tdm_w_proto(tdm, q, mp);
			break;
		case M_PCCTL:
			err = tdm_w_ctl(tdm, q, mp);
			break;
		case M_IOCDATA:
			err = tdm_w_ioctl(tdm, q, mp);
			break;
		}
		mi_unlock((caddr_t) tdm);
		return (err);
	}
	case M_FLUSH:
		return m_w_flush(q, mp);
	default:
		return m_w_other(q, mp);
	}

}
static int
tdm_rput_msg(queue_t *q, mblk_t *mp)
{
	unsigned char type;

	switch (__builtin_expect((type = DB_TYPE(mp)), M_DATA)) {
	case M_DATA:
	case M_PROTO:
	case M_CTL:
		return (-EAGAIN);	/* for speed */
	case M_HPDATA:
	case M_PCPROTO:
	case M_PCCTL:
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
	{
		struct tdm *tdm;
		int err;

		if (unlikely(!(tdm = (typeof(tdm)) mi_trylock(q))))
			return (-EDEADLK);
		switch (type) {
		case M_HPDATA:
			err = tdm_r_data(tdm, q, mp);
			break;
		case M_PCPROTO:
			err = tdm_r_proto(tdm, q, mp);
			break;
		case M_PCCTL:
			err = tdm_r_ctl(tdm, q, mp);
			break;
		case M_IOCACK:
		case M_IOCNAK:
		case M_COPYIN:
		case M_COPYOUT:
			err = tdm_r_ioctl(tdm, q, mp);
			break;
		}
		mi_unlock((caddr_t) tdm);
		return (err);
	}
	case M_ERROR:
	case M_HANGUP:
		return m_r_error(q, mp);
	case M_FLUSH:
		return m_r_flush(q, mp);
	default:
		return m_r_other(q, mp);
	}
}

static streamscall int
tdm_wput(queue_t *q, mblk_t *mp)
{
}
static streamscall int
tdm_wsrv(queue_t *q)
{
}
static streamscall int
tdm_rsrv(queue_t *q)
{
}
static streamscall int
tdm_rput(queue_t *q, mblk_t *mp)
{
}

static streamscall int
tdm_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
}
static streamscall int
tdm_qclose(queue_t *q, int oflags, cred_t *crp)
{
}

static struct module_info tdm_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat tdm_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat tdm_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit tdm_rdinit = {
	.qi_putp = tdm_rput,
	.qi_srvp = tdm_rsrv,
	.qi_qopen = tdm_qopen,
	.qi_qclose = tdm_qclose,
	.qi_minfo = &tdm_minfo,
	.qi_mstat = &tdm_rstat,
};

static struct qinit tdm_wrinit = {
	.qi_putp = tdm_wput,
	.qi_srvp = tdm_wsrv,
	.qi_minfo = &tdm_minfo,
	.qi_mstat = &tdm_wstat,
};

static struct streamtab tdm_ipinfo = {
	.st_rdinit = &tdm_rdinit,
	.st_wrinit = &tdm_wrinit,
};

static struct fmodsw tdm_fmod = {
	.f_name = MOD_NAME,
	.f_str = &tdm_ipinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static int __init
tdm_ipinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&tdm_fmod, modid)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d, err = %d\n", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static int
tdm_ipterminate(void)
{
	int err;

	if ((err = unregister_strmod(&tdm_fmod, modid)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module %d, err = %d\n", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	modid = 0;
	return (0);
}

static void __exit
tdm_ipexit(void)
{
	tdm_ipterminate();
}
