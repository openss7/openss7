/*****************************************************************************

 @(#) $RCSfile: sccp_tpi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-11-17 14:04:35 $

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

 Last Modified $Date: 2008-11-17 14:04:35 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sccp_tpi.c,v $
 Revision 0.9.2.1  2008-11-17 14:04:35  brian
 - added documentation and conversion modules

 *****************************************************************************/

#ident "@(#) $RCSfile: sccp_tpi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-11-17 14:04:35 $"

static char const ident[] = "$RCSfile: sccp_tpi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-11-17 14:04:35 $";

/*
 * This is a TPI module that can be pushed over an SCCPI (Signalling Connection Control Part
 * Interface) stream to effect a TPI interface to the SCCP suitable for use with XTI.  This module
 * is intended to be used by applications programs or by upper modules that expect a TPI
 * connection-oriented or connectionless service provider.
 */

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

#include <sys/os7/compat.h>
#include <sys/strsun.h>

#include <linux/socket.h>

#include <sys/npi.h>
#include <sys/npi_ss7.h>
#include <sys/npi_sccp.h>
#include <sys/xti.h>
#include <sys/tihdr.h>
#include <sys/xti.h>
#include <sys/xti_ss7.h>
#include <sys/xti_sccp.h>

#define SCCP_TPI_DESCRIP	"SS7 Signalling Connection Control Part (SCCP) TPI STREAMS MODULE."
#define SCCP_TPI_REVISION	"OpenSS7 $RCSfile: sccp_tpi.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-11-17 14:04:35 $"
#define SCCP_TPI_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define SCCP_TPI_DEVICE		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SCCP_TPI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SCCP_TPI_LICENSE	"GPL"
#define SCCP_TPI_BANNER		SCCP_TPI_DESCRIP	"\n" \
				SCCP_TPI_REVISION	"\n" \
				SCCP_TPI_COPYRIGHT	"\n" \
				SCCP_TPI_DEVICE		"\n" \
				SCCP_TPI_CONTACT	"\n"
#define SCCP_TPI_SPLASH		SCCP_TPI_DESCRIP	"\n" \
				SCCP_TPI_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(SCCP_TPI_CONTACT);
MODULE_DESCRIPTION(SCCP_TPI_DESCRIP);
MODULE_SUPPORTED_DEVICE(SCCP_TPI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SCCP_TPI_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-sccp_tpi");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#ifdef LFS
#define SCCP_TPI_MOD_ID		CONFIG_STREAMS_SCCP_TPI_MODID
#define SCCP_TPI_MOD_NAME	CONFIG_STREAMS_SCCP_TPI_NAME
#endif				/* LFS */

#define MOD_ID		SCCP_TPI_MOD_ID
#define MOD_NAME	SCCP_TPI_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SCCP_TPI_BANNER
#else				/* MODULE */
#define MOD_BANNER	SCCP_TPI_SPLASH
#endif				/* MODULE */

#define STRLOGNO	0
#define STRLOGST	1
#define STRLOGTO	2
#define STRLOGRX	3
#define STRLOGTX	4
#define STRLOGTE	5
#define STRLOGDA	6

/*
 * PRIVATE STRUCTURE
 */

struct sccp {
	queue_t *rq;
	queue_t *wq;
	uint coninds;
	struct {
		N_info_ack_t info;
	} n;
	struct {
		struct T_capability_ack info;
		struct strbuf laddr;	/* local address */
		struct strbuf raddr;	/* remote address */
	} t;
};

#define SCCP_PRIV(__q) ((struct sccp *)(__q)->q_ptr)

static const char *
tp_primname(t_scalar_t prim)
{
	switch (prim) {
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
static const char *
np_primname(np_long prim)
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
		return ("(unknown)");
	}
}
static const char *
tp_statename(t_scalar_t state)
{
	switch (state) {
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
static const char *
np_statename(np_long state)
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
		return ("(unknown)");
	}
}
static const char *
tp_errname(t_scalar_t error)
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
		return ("TNOSTRUCTURE");
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
	default:
		return ("(unknown)");
	}
}
static const char *
np_errname(np_long error)
{
	switch (error) {
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
	default:
		return ("(unknown)");
	}
}

static const char *
tp_errdesc(t_scalar_t error)
{
	switch (error) {
	case TBADADDR:
		return ("Bad address format.");
	case TBADOPT:
		return ("Bad options format.");
	case TACCES:
		return ("Bad permissions.");
	case TBADF:
		return ("Bad file descriptor.");
	case TNOADDR:
		return ("Unable to allocate an address.");
	case TOUTSTATE:
		return ("Would place interface out of state.");
	case TBADSEQ:
		return ("Bad call sequence number.");
	case TSYSERR:
		return ("System error.");
	case TLOOK:
		return ("Has to t_look() for event.");
	case TBADDATA:
		return ("Bad amount of data.");
	case TBUFOVFLW:
		return ("Buffer was too small.");
	case TFLOW:
		return ("Would block due to flow control.");
	case TNODATA:
		return ("No data indication.");
	case TNODIS:
		return ("No disconnect indication.");
	case TNOUDERR:
		return ("No unitdata error indication.");
	case TBADFLAG:
		return ("Bad flags.");
	case TNOREL:
		return ("No orderly release indication.");
	case TNOTSUPPORT:
		return ("Not supported.");
	case TSTATECHNG:
		return ("State is currently changing.");
	case TNOSTRUCTYPE:
		return ("Structure type not supported.");
	case TBADNAME:
		return ("Bad transport provider name.");
	case TBADQLEN:
		return ("Listener queue length limit is zero.");
	case TADDRBUSY:
		return ("Address already in use.");
	case TINDOUT:
		return ("Outstanding connect indications.");
	case TPROVMISMATCH:
		return ("Not same transport provider.");
	case TRESQLEN:
		return ("Connection acceptor has qlen > 0.");
	case TRESADDR:
		return ("Conn. acceptor bound to different address.");
	case TQFULL:
		return ("Connection indicator queue is full.");
	case TPROTO:
		return ("Protocol error.");
	default:
		return ("(unknown)");
	}
}
static const char *
np_errdesc(np_long error)
{
	switch (error) {
	case NBADADDR:
		return ("Incorrect address format/illegal address information.");
	case NBADOPT:
		return
		    ("Options in incorrect format or contain illegal information.");
	case NACCESS:
		return ("User did not have proper permissions.");
	case NNOADDR:
		return ("NS Provider could not allocate address.");
	case NOUTSTATE:
		return ("Primitive was issues in wrong sequence.");
	case NBADSEQ:
		return ("Sequence number in primitive was incorrect/illegal.");
	case NSYSERR:
		return ("UNIX system error occurred.");
	case NBADDATA:
		return ("User data spec. outside range supported by NS provider.");
	case NBADFLAG:
		return ("Flags specified in primitive were illegal/incorrect.");
	case NNOTSUPPORT:
		return ("Primitive type not supported by the NS provider.");
	case NBOUND:
		return
		    ("Illegal second attempt to bind listener or default listener.");
	case NBADQOSPARAM:
		return
		    ("QOS values specified are outside the range supported by the NS provider.");
	case NBADQOSTYPE:
		return
		    ("QOS structure type specified is not supported by the NS provider.");
	case NBADTOKEN:
		return ("Token used is not associated with an open stream.");
	case NNOPROTOID:
		return ("Protocol id could not be allocated.");
	default:
		return ("(unknown)");
	}
}

/**
 * stripmsg: - strip leading blocks of a message
 * @mp: message to strip blocks from
 * @dp: message block to stop at
 *
 * Returns true if @dp was found in the message and false otherwise.  When @dp is null this function
 * has the same effect as freemsg(9) and returns true.
 */
noinline fastcall int
stripmsg(mblk_t *mp, mblk_t *dp)
{
	mblk_t *b, *b_next = mp;

	while ((b = b_next) && b != dp) {
		b_next = b->b_next;
		freeb(b);
	}
	return (b == dp);
}

/**
 * tp_rxprim: - trace log sent primitive
 * @sccp: private structure (locked)
 * @mp: the TPI primitive being received
 */
static int
tp_rxprim(struct sccp *sccp, mblk_t *mp)
{
	t_scalar_t prim = *(t_scalar_t *) mp->b_rptr;
	int priority;

	switch (prim) {
	case T_DATA_REQ:
	case T_EXDATA_REQ:
	case T_OPTDATA_REQ:
	case T_UNITDATA_REQ:
		priority = STRLOGDA;
		break;
	default:
		priority = STRLOGRX;
		break;
	}
	return mi_strlog(sccp->wq, priority, SL_TRACE, "-> %s", tp_primname(prim));
}

/**
 * np_rxprim: - trace log sent primitive
 * @sccp: private structure (locked)
 * @mp: the NPI primitive being received
 */
static int
np_rxprim(struct sccp *sccp, mblk_t *mp)
{
	np_long prim = *(np_long *) mp->b_rptr;
	int priority;

	switch (prim) {
	case N_DATA_IND:
	case N_EXDATA_IND:
	case N_DATACK_IND:
	case N_UDERROR_IND:
	case N_UNITDATA_IND:
		priority = STRLOGDA;
		break;
	default:
		priority = STRLOGRX;
		break;
	}
	return mi_strlog(sccp->rq, priority, SL_TRACE, "%s <-", np_primname(prim));
}

/**
 * tp_txprim: - trace log sent primitive
 * @sccp: private structure (locked)
 * @msg: message to strip upon success
 * @mp: the TPI primitive being sent
 */
static int
tp_txprim(struct sccp *sccp, mblk_t *msg, mblk_t *mp)
{
	t_scalar_t prim = *(t_scalar_t *) mp->b_rptr;
	int priority, rtn;

	switch (prim) {
	case T_DATA_IND:
	case T_EXDATA_IND:
	case T_OPTDATA_IND:
	case T_UDERROR_IND:
	case T_UNITDATA_IND:
		priority = STRLOGDA;
		break;
	default:
		priority = STRLOGRX;
		break;
	}
	rtn = mi_strlog(sccp->rq, priority, SL_TRACE, "<- %s", tp_primname(prim));
	stripmsg(msg, mp->b_cont);
	putnext(sccp->rq, mp);
	return (rtn);
}

/**
 * np_txprim: - trace log sent primitive
 * @sccp: private structure (locked)
 * @msg: message to strip upon success
 * @mp: the NPI primitive being sent
 */
static int
np_txprim(struct sccp *sccp, mblk_t *msg, mblk_t *mp)
{
	np_long prim = *(np_long *) mp->b_rptr;
	int priority, rtn;

	switch (prim) {
	case N_DATA_REQ:
	case N_EXDATA_REQ:
	case N_DATACK_REQ:
	case N_UNITDATA_REQ:
		priority = STRLOGDA;
		break;
	default:
		priority = STRLOGRX;
		break;
	}
	rtn = mi_strlog(sccp->rq, priority, SL_TRACE, "%s ->", np_primname(prim));
	stripmsg(msg, mp->b_cont);
	putnext(sccp->wq, mp);
	return (rtn);
}

/**
 * tp_error: - trace log error condition
 * @sccp: private structure (locked)
 * @error: TPI error type
 * @func: function in which error was detected
 */
static int
tp_error(struct sccp *sccp, t_scalar_t error, const char *func)
{
	return mi_strlog(sccp->wq, STRLOGNO, SL_TRACE, "ERROR: %s: %s: %s", func,
			 tp_errname(error), tp_errdesc(error));
}

/**
 * np_error: - trace log error condition
 * @sccp: private structure (locked)
 * @error: NPI error type
 * @func: function in which error was detected
 */
static int
np_error(struct sccp *sccp, np_long error, const char *func)
{
	return mi_strlog(sccp->rq, STRLOGNO, SL_TRACE, "ERROR: %s: %s: %s", func,
			 np_errname(error), np_errdesc(error));
}

/**
 * np_t_prim: - translate NPI primitive to TPI primitive
 * @prim: the primitive type to translate
 */
static t_scalar_t
np_t_prim(np_ulong prim)
{
	switch (prim) {
	case N_CONN_REQ:
		return (T_CONN_REQ);
	case N_CONN_RES:
		return (T_CONN_RES);
	case N_DISCON_REQ:
		return (T_DISCON_REQ);
	case N_DATA_REQ:
		return (T_DATA_REQ);
	case N_EXDATA_REQ:
		return (T_EXDATA_REQ);
	case N_INFO_REQ:
		return (T_INFO_REQ);
	case N_BIND_REQ:
		return (T_BIND_REQ);
	case N_UNBIND_REQ:
		return (T_UNBIND_REQ);
	case N_UNITDATA_REQ:
		return (T_UNITDATA_REQ);
	case N_OPTMGMT_REQ:
		return (T_OPTMGMT_REQ);
	case N_CONN_IND:
		return (T_CONN_IND);
	case N_CONN_CON:
		return (T_CONN_CON);
	case N_DISCON_IND:
		return (T_DISCON_IND);
	case N_DATA_IND:
		return (T_DATA_IND);
	case N_EXDATA_IND:
		return (T_EXDATA_IND);
	case N_INFO_ACK:
		return (T_INFO_ACK);
	case N_BIND_ACK:
		return (T_BIND_ACK);
	case N_ERROR_ACK:
		return (T_ERROR_ACK);
	case N_OK_ACK:
		return (T_OK_ACK);
	case N_UNITDATA_IND:
		return (T_UNITDATA_IND);
	case N_UDERROR_IND:
		return (T_UDERROR_IND);
	case N_DATACK_REQ:
		return (T_OPTMGMT_REQ);
	case N_DATACK_IND:
		return (T_OPTDATA_IND);
	case N_RESET_REQ:
		return (T_OPTMGMT_REQ);
	case N_RESET_IND:
		return (T_OPTDATA_IND);
	case N_RESET_RES:
		return (T_OPTMGMT_REQ);
	case N_RESET_CON:
		return (T_OPTDATA_IND);
	default:
		return (-1);
	}
}

/**
 * np_t_error: - translate NPI error to TPI error
 * @error: error type to translate
 */
static t_scalar_t
np_t_error(np_ulong error, np_ulong errno)
{
	switch (error) {
	case NBADADDR:
		return (TBADADDR);
	case NBADOPT:
		return (TBADOPT);
	case NACCESS:
		return (TACCES);
	case NNOADDR:
		return (TNOADDR);
	case NOUTSTATE:
		return (TOUTSTATE);
	case NBADSEQ:
		return (TBADSEQ);
	case NSYSERR:
		return (-errno);
	case NBADDATA:
		return (TBADDATA);
	case NBADFLAG:
		return (TBADFLAG);
	case NNOTSUPPORT:
		return (TNOTSUPPORT);
	case NBOUND:
		return (TADDRBUSY);
	case NBADQOSPARAM:
		return (TBADOPT);
	case NBADQOSTYPE:
		return (TBADOPT);
	case NBADTOKEN:
		return (TBADF);
	case NNOPROTOID:
		return (TNOADDR);
	default:
		return (error);
	}
}

/*
 * TPI MESSAGES ISSUED UPSTREAM
 */

/**
 * t_conn_ind: - issue T_CONN_IND upstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @sptr: source address pointer
 * @slen: source address length
 * @optr: option pointer
 * @olen: option length
 * @seq: sequence number
 * @dp: connect data
 */
static inline fastcall int
t_conn_ind(struct sccp *sccp, queue_t *q, mblk_t *msg,
	   caddr_t sptr, size_t slen,
	   caddr_t optr, size_t olen, t_uscalar_t seq, mblk_t *dp)
{
	struct T_conn_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + slen + olen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_IND;
		p->SRC_length = slen;
		p->SRC_offset = slen ? sizeof(*p) : 0;
		p->OPT_length = olen;
		p->OPT_offset = olen ? sizeof(*p) + slen : 0;
		p->SEQ_number = seq;
		mp->b_wptr += sizeof(*p);
		bcopy(sptr, mp->b_wptr, slen);
		mp->b_wptr += slen;
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		sccp->coninds++;
		sccp_set_t_state(sccp, TS_WRES_CIND);
		tp_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_conn_con: - issue T_CONN_CON upstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @rptr: responding address pointer
 * @rlen: responding address length
 * @optr: options pointer
 * @olen: options length
 * @dp: connect data
 */
static inline fastcall int
t_conn_con(struct sccp *sccp, queue_t *q, mblk_t *msg,
	   caddr_t rptr, size_t rlen, caddr_t optr, size_t olen, mblk_t *dp)
{
	struct T_conn_con *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + rlen + olen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CONN_CON;
		p->RES_length = rlen;
		p->RES_offset = rlen ? sizeof(*p) : 0;
		p->OPT_length = olen;
		p->OPT_offset = olen ? sizeof(*p) + rlen : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(sptr, mp->b_wptr, rlen);
		mp->b_wptr += rlen;
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		sccp_set_t_state(sccp, TS_DATA_XFER);
		tp_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_discon_ind: - issue T_DISCON_IND upstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @reason: disconnect reason
 * @seq: sequence number
 * @dp: disconnect data
 */
static inline fastcall int
t_discon_ind(struct sccp *sccp, queue_t *q, mblk_t *msg, t_uscalar_t reason,
	     t_uscalar_t seq, mblk_t *dp)
{
	struct T_discon_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_DISCON_IND;
		p->DISCON_reason = reason;
		p->SEQ_number = seq;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		switch (sccp_get_t_state(sccp)) {
		case TS_WRES_CIND:
		case TS_WACK_CRES:
			if (--sccp->coninds)
				sccp_set_t_state(sccp, TS_WRES_CIND);
			else
				sccp_set_t_state(sccp, TS_IDLE);
			break;
		default:
			sccp_set_t_state(sccp, TS_IDLE);
			break;
		}
		sccp_set_t_state(sccp, TS_IDLE);
		tp_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_data_ind: - issue T_DATA_IND upstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @more: more data flag
 * @dp: user data
 */
static inline fastcall int
t_data_ind(struct sccp *sccp, queue_t *q, mblk_t *msg, t_scalar_t more, mblk_t *dp)
{
	struct T_data_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_DATA_IND;
		p->MORE_flag = more ? 1 : 0;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		tp_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_exdata_ind: - issue T_EXDATA_IND upstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @more: more data flag
 * @dp: user data
 */
static inline fastcall int
t_exdata_ind(struct sccp *sccp, queue_t *q, mblk_t *msg, t_scalar_t more, mblk_t *dp)
{
	struct T_exdata_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_EXDATA_IND;
		p->MORE_flag = more ? 1 : 0;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		tp_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_info_ack: - issue T_INFO_ACK upstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 */
static inline fastcall int
t_info_ack(struct sccp *sccp, queue_t *q, mblk_t *msg)
{
	struct T_info_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		*p = sccp->t.info;
		p->PRIM_type = T_INFO_ACK;
		mp->b_wptr += sizeof(*p);
		tp_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_bind_ack: - issue T_BIND_ACK upstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @aptr: address pointer
 * @alen: address length
 * @coninds: maximum number of outstanding connection indications
 */
static inline fastcall int
t_bind_ack(struct sccp *sccp, queue_t *q, mblk_t *msg,
	   caddr_t aptr, size_t alen, t_uscalar_t coninds)
{
	struct T_bind_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + alen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_BIND_ACK;
		p->ADDR_length = alen;
		p->ADDR_offset = alen ? sizeof(*p) : 0;
		p->CONIND_number = coninds;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		sccp_set_t_state(sccp, TS_IDLE);
		tp_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_error_ack: - issue T_ERROR_ACK upstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @error: positive TLI error or negative UNIX error
 */
static inline fastcall int
t_error_ack(struct sccp *sccp, queue_t *q, mblk_t *msg, t_scalar_t prim,
	    t_scalar_t error)
{
	struct T_error_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ERROR_ACK;
		p->ERROR_prim = prim;
		p->TLI_error = error > 0 ? error : TSYSERR;
		p->UNIX_error = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		switch (sccp_get_t_state(sccp)) {
		case TS_WACK_BREQ:
			sccp_set_t_state(sccp, TS_UNBND);
			break;
		case TS_WACK_UREQ:
			sccp_set_t_state(sccp, TS_IDLE);
			break;
		case TS_WACK_CREQ:
			sccp_set_t_state(sccp, TS_IDLE);
			break;
		case TS_WACK_CRES:
			if (sccp->coninds == 0) {
				sccp_set_t_state(sccp, TS_IDLE);
				break;
			}
			sccp_set_t_state(sccp, TS_WRES_CIND);
			break;
		case TS_WACK_DREQ7:
			if (sccp->coninds == 0) {
				sccp_set_t_state(sccp, TS_IDLE);
				break;
			}
			sccp_set_t_state(sccp, TS_WRES_CIND);
			break;
		case TS_WACK_DREQ9:
			sccp_set_t_state(sccp, TS_DATA_XFER);
			break;
		case TS_WACK_DREQ10:
			sccp_set_t_state(sccp, TS_WIND_ORDREL);
			break;
		case TS_WACK_DREQ11:
			sccp_set_t_state(sccp, TS_WREQ_ORDREL);
			break;
		case TS_WACK_OPTREQ:
			sccp_set_t_state(sccp, TS_IDLE);
			break;
		}
		tp_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

noinline fastcall int
m_error(struct sccp *sccp, queue_t *q, mblk_t *msg, unsigned char rerror,
	unsigned char werror)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(rerror) + sizeof(werror), BPRI_HI)))) {
		DB_TYPE(mp) = M_ERROR;
		mp->b_wptr[0] = rerror;
		mp->b_wptr[1] = werror;
		mp->b_wptr += 2;
		sccp_set_n_state(sccp, -1);
		sccp_set_t_state(sccp, -1);
		freemsg(msg);
		putnext(sccp->rq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_error_reply: - reply with an error
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @error: positive TPI error or negative UNIX error
 * @func: function in which error was detected
 */
noinline fastcall int
t_error_reply(struct sccp *sccp, queue_t *q, mblk_t *msg, t_scalar_t prim,
	      t_scalar_t error, const char *func)
{
	switch (error) {
	case -EAGAIN:
	case -ENOBUFS:
	case -ENOMEM:
	case -EDEADLK:
	case -EBUSY:
		return (err);
	case 0:
		/* discard situation */
		freemsg(msg);
		return (0);
	}
	switch (prim) {
	case T_DATA_REQ:
	case T_EXDATA_REQ:
	case T_UNITDATA_REQ:
	case T_ORDREL_REQ:
	case T_OPTDATA_REQ:
		/* If the primitive in error is one for which no T_ERROR_ACK is
		   possible, then the TPI user is given an M_ERROR message. */
		mi_strlog(q, STRLOGDA, SL_TRACE, "ERROR: %s: %s: %s", func,
			  tp_errname(error), tp_errdesc(error));
		goto merror;
	}
	switch (sccp_get_t_state(sccp)) {
	case TS_WACK_BREQ:
	case TS_WACK_UREQ:
	case TS_WACK_OPTREQ:
	case TS_WACK_CREQ:
	case TS_WACK_CRES:
	case TS_WACK_DREQ6:
	case TS_WACK_DREQ7:
	case TS_WACK_DREQ9:
	case TS_WACK_DREQ10:
	case TS_WACK_DREQ11:
		/* If the interface is in the waiting for acknowledgement state then 
		   the TPI user has violated the interface protocol and deserves an
		   M_ERROR message. */
		mi_strlog(q, STRLOGNO, SL_TRACE | SL_ERROR, "ERROR: %s: %s: %s",
			  func, tp_errname(error), tp_errdesc(error));
		goto merror;
	}
	mi_strlog(q, STRLOGNO, SL_TRACE, "ERROR: %s: %s: %s", func,
		  tp_errname(error), tp_errdesc(error));
	return t_error_ack(sccp, q, msg, prim, error);
      merror:
	return m_error(sccp, q, msg, EPROTO, EPROTO);
}

/**
 * t_ok_ack: - issue T_OK_ACK upstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @accept: accepting private structure (locked)
 * @prim: correct primitive
 */
static inline fastcall int
t_ok_ack(struct sccp *sccp, queue_t *q, mblk_t *msg, struct sccp *accept,
	 t_scalar_t prim)
{
	struct T_ok_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OK_ACK;
		p->CORRECT_prim = prim;
		mp->b_wptr += sizeof(*p);
		switch (sccp_get_t_state(sccp)) {
		case TS_WACK_BREQ:
			sccp_set_t_state(sccp, TS_IDLE);
			break;
		case TS_WACK_UREQ:
			sccp_set_t_state(sccp, TS_UNBND);
			break;
		case TS_WACK_CREQ:
			sccp_set_t_state(sccp, TS_WCON_CREQ);
			break;
		case TS_WACK_CRES:
			sccp_set_t_state(accept, TS_DATA_XFER);
			if (sccp != accept) {
				if (--sccp->coninds)
					sccp_set_t_state(sccp, TS_WRES_CIND);
				else
					sccp_set_t_state(sccp, TS_IDLE);
			}
			break;
		case TS_WACK_DREQ7:
			if (accept) {
				if (--sccp->coninds)
					sccp_set_t_state(sccp, TS_WRES_CIND);
				else
					sccp_set_t_state(sccp, TS_IDLE);
			}
			break;
		case TS_WACK_DREQ9:
			sccp_set_t_state(sccp, TS_IDLE);
			break;
		case TS_WACK_DREQ10:
			sccp_set_t_state(sccp, TS_IDLE);
			break;
		case TS_WACK_DREQ11:
			sccp_set_t_state(sccp, TS_IDLE);
			break;
		case TS_WACK_OPTREQ:
			sccp_set_t_state(sccp, TS_IDLE);
			break;
		}
		tp_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_unitdata_ind: - issue T_UNITDATA_IND upstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @sptr: source address pointer
 * @slen: source address length
 * @optr: options pointer
 * @olen: options length
 * @dp: user data
 */
static inline fastcall int
t_unitdata_ind(struct sccp *sccp, queue_t *q, mblk_t *msg,
	       caddr_t sptr, size_t slen, caddr_t optr, size_t olen, mblk_t *dp)
{
	struct T_unitdata_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + slen + olen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_UNITDATA_IND;
		p->SRC_length = slen;
		p->SRC_offset = slen ? sizeof(*p) : 0;
		p->OPT_length = olen;
		p->OPT_offset = olen ? sizeof(*p) + slen : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(sptr, mp->b_wptr, slen);
		mp->b_wptr += slen;
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		tp_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_uderror_ind: = issue T_UDERROR_IND upstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @dptr: destination address pointer
 * @dlen: destination address length
 * @optr: options pointer
 * @olen: options length
 * @type: error type
 * @dp: user data
 */
static inline fastcall int
t_uderror_ind(struct sccp *sccp, queue_t *q, mblk_t *msg,
	      caddr_t dptr, size_t dlen,
	      caddr_t optr, size_t olen, t_scalar_t type, mblk_t *dp)
{
	struct T_uderror_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + dlen + olen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_UDERROR_IND;
		p->DEST_length = dlen;
		p->DEST_offset = dlen ? sizeof(*p) : 0;
		p->OPT_length = olen;
		p->OPT_offset = olen ? sizeof(*p) + dlen : 0;
		p->ERROR_type = type;
		mp->b_wptr += sizeof(*p);
		bcopy(dptr, mp->b_wptr, dlen);
		mp->b_wptr += dlen;
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		tp_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_optmgmt_ack: - issue T_OPGMGMT_ACK upstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @optr: options pointer
 * @olen: options length
 * @flags: management flags
 */
static inline fastcall int
t_optmgmt_ack(struct sccp *sccp, queue_t *q, mblk_t *msg,
	      caddr_t optr, size_t olen, t_scalar_t flags)
{
	struct T_optmgmt_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + olen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OPTMGMT_ACK;
		p->OPT_length = olen;
		p->OPT_offset = olen ? sizeof(*p) : 0;
		p->MGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		if (sccp_get_t_state(sccp) == TS_WACK_OPTREQ)
			sccp_set_t_state(sccp, TS_IDLE);
		tp_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_ordrel_ind: - issue T_ORDREL_IND upstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @dp: release data
 */
static inline fastcall int
t_ordrel_ind(struct sccp *sccp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	struct T_ordrel_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ORDREL_IND;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		switch (sccp_get_t_state(sccp)) {
		case TS_WIND_ORDREL:
			sccp_set_t_state(sccp, TS_IDLE);
			break;
		case TS_DATA_XFER:
			sccp_set_t_state(sccp, TS_WREQ_ORDREL);
			break;
		case TS_WACK_DREQ9:
			sccp_set_t_state(sccp, TS_WACK_DREQ11);
			break;
		case TS_WACK_DREQ11:
			sccp_set_t_state(sccp, TS_IDLE);
			break;
		}
		tp_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_optdata_ind: - issue T_OPTDATA_IND upstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @flag: data flags
 * @optr: options pointer
 * @olen: options length
 * @dp: user data
 */
static inline fastcall int
t_optdata_ind(struct sccp *sccp, queue_t *q, mblk_t *msg, t_scalar_t flag,
	      caddr_t optr, size_t olen, mblk_t *dp)
{
	struct T_optdata_ind *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + olen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_OPTDATA_IND;
		p->DATA_flag = flag;
		p->OPT_length = olen;
		p->OPT_offset = olen ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		mp->b_cont = dp;
		tp_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * t_addr_ack: - issue T_ADDR_ACK upstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @laddr: local address buffer
 * @raddr: remote address buffer
 */
static inline fastcall int
t_addr_ack(struct sccp *sccp, queue_t *q, mblk_t *msg, struct strbuf *laddr,
	   struct strbuf *raddr)
{
	struct T_addr_ack *p;
	mblk_t *mp;

	const unsigned char *lptr = laddr ? laddr->buf : NULL;
	const unsigned char *rptr = raddr ? raddr->buf : NULL;
	const size_t llen = laddr ? laddr->len : 0;
	const size_t rlen = raddr ? raddr->len : 0;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + llen + rlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_ADDR_ACK;
		p->LOCADDR_length = llen;
		p->LOCADDR_offset = llen ? sizeof(*p) : 0;
		p->REMADDR_length = rlen;
		p->REMADDR_offset = rlen ? sizeof(*p) + llen : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(lptr, mp->b_wptr, llen);
		mp->b_wptr += llen;
		bcopy(rptr, mp->b_wptr, rlen);
		mp->b_wptr += rlen;
		tp_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline fastcall int
t_capability_ack(struct sccp *sccp, queue_t *q, mblk_t *msg)
{
	struct T_capability_ack *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = T_CAPABILITY_ACK;
		p->CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
		p->INFO_ack = sccp->t.info;
		p->INFO_ack.PRIM_type = T_INFO_ACK;
		p->ACCEPTOR_id = sccp->token;
		mp->b_wptr += sizeof(*p);
		tp_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * NPI MESSAGES ISSUED DOWNSTREAM
 */

/**
 * n_conn_req: - issue N_CONN_REQ message downstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @flags: connect flags
 * @dst: destination address
 * @qos: quality of service parameters
 * @dp: connect user data
 */
static inline fastcall int
n_conn_req(struct sccp *sccp, queue_t *q, mblk_t *msg, np_ulong flags,
	   const struct strbuf *dst, const struct strbuf *qos, const mblk_t *dp)
{
	N_conn_req_t *p;
	mblk_t *mp;
	const size_t dlen = dst->len;
	const size_t qlen = qos->len;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + dlen + qlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_CONN_REQ;
		p->DEST_length = dlen;
		p->DEST_offset = dlen ? sizeof(*p) : 0;
		p->CONN_flags = flags;
		p->QOS_length = qlen;
		p->QOS_offset = qlen ? sizeof(*p) + dlen : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(dst->buf, mp->b_wptr, dlen);
		mp->b_wptr += dlen;
		bcopy(qos->buf, mp->b_wptr, qlen);
		mp->b_wptr += qlen;
		mp->b_cond = dp;
		sccp_set_n_state(sccp, NS_WCON_CREQ);
		np_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_conn_res: - issue N_CONN_RES primitive downstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @token: accepting stream token
 * @flags: connect flags
 * @res: responding address
 * @qos: quality of service parameters
 * @seq: sequence number
 * @dp: connect user data
 */
static inline fastcall int
n_conn_res(struct sccp *sccp, queue_t *q, mblk_t *msg, np_ulong token,
	   np_ulong flags, struct strbuf *res, struct strbuf *qos, np_ulong seq,
	   mblk_t *dp)
{
	N_conn_res_t *p;
	mblk_t *mp;
	const size_t rlen = res->len;
	const size_t qlen = qos->len;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + rlen + qlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_CONN_RES;
		p->TOKEN_value = token;
		p->RES_length = rlen;
		p->RES_offset = rlen ? sizeof(*p) : 0;
		p->SEQ_number = seq;
		p->CONN_flags = flags;
		p->QOS_length = qlen;
		p->QOS_offset = qlen ? sizeof(*p) + rlen : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(res->buf, mp->b_wptr, rlen);
		mp->b_wptr += rlen;
		bcopy(qos->buf, mp->b_wptr, qlen);
		mp->b_wptr += qlen;
		mp->b_cont = dp;
		sccp_set_n_state(sccp, NS_WACK_CRES);
		np_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * n_discon_req: - issue N_DISCON_REQ primitive downstream
 * @sccp: private structure (locked)
 * @q: active queue
 * @msg: message to free upon success
 * @res: responding address
 * @seq: sequence number
 * @dp: disconnect user data
 */
static inline fastcall int
n_discon_req(struct sccp *sccp, queue_t *q, mblk_t *msg, np_ulong reason,
	     struct strbuf *res, np_ulong seq, mblk_t *dp)
{
	N_discon_req_t *p;
	mblk_t *mp;
	const size_t rlen = res->len;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + rlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_DISCON_REQ;
		p->DISCON_reason = reason;
		p->RES_length = rlen;
		p->RES_offset = rlen ? sizeof(*p) : 0;
		p->SEQ_number = seq;
		mp->b_wptr += sizeof(*p);
		bcopy(res->ptr, mp->b_wptr, rlen);
		mp->b_wptr += rlen;
		mp->b_cont = dp;
		switch (sccp_get_n_state(sccp)) {
		case NS_WCON_CREQ:
			sccp_set_n_state(sccp, NS_WACK_DREQ6);
			break;
		case NS_WRES_CIND:
			sccp_set_n_state(sccp, NS_WACK_DREQ7);
			break;
		case NS_DATA_XFER:
			sccp_set_n_state(sccp, NS_WACK_DREQ9);
			break;
		case NS_WCON_RREQ:
			sccp_set_n_state(sccp, NS_WACK_DREQ10);
			break;
		case NS_WRES_RIND:
			sccp_set_n_state(sccp, NS_WACK_DREQ11);
			break;
		}
		np_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_data_req(struct sccp *sccp, queue_t *q, mblk_t *msg, np_ulong flags, mblk_t *dp)
{
	N_data_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(sccp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_REQ;
			p->DATA_xfer_flags = flags;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			np_txprim(sccp, msg, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_exdata_req(struct sccp *sccp, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	N_exdata_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(bcanputnext(sccp->wq, 1))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_EXDATA_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			np_txprim(sccp, msg, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_info_req(struct sccp *sccp, queue_t *q, mblk_t *msg)
{
	N_info_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		np_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_bind_req(struct sccp *sccp, queue_t *q, mblk_t *msg,
	   caddr_t aptr, size_t alen,
	   np_ulong coninds, np_ulong flags, caddr_t pptr, size_t plen)
{
	N_bind_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + alen + plen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_BIND_REQ;
		p->ADDR_length = alen;
		p->ADDR_offset = alen ? sizeof(*p) : 0;
		p->CONIND_number = coninds;
		p->BIND_flags = flags;
		p->PROTOID_length = plen;
		p->PROTOID_offset = plen ? sizeof(*p) + alen : 0;
		mp->b_wptr += sizeof(*p);
		sccp_set_n_state(sccp, NS_WACK_BREQ);
		np_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_unbind_req(struct sccp *sccp, queue_t *q, mblk_t *msg)
{
	N_unbind_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_UNBIND_REQ;
		mp->b_wptr += sizeof(*p);
		sccp_set_n_state(sccp, NS_WACK_UREQ);
		np_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_unitdata_req(struct sccp *sccp, queue_t *q, mblk_t *msg,
	       caddr_t dptr, size_t dlen, mblk_t *dp)
{
	N_unitdata_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + dlen, BPRI_MED)))) {
		if (likely(canputnext(sccp->wq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNITDATA_REQ;
			p->DEST_length = dlen;
			p->DEST_offset = dlen ? sizeof(*p) : 0;
			p->RESERVED_field[0] = 0;
			p->RESERVED_field[1] = 0;
			mp->b_wptr += sizeof(*p);
			bcopy(dptr, mp->b_wptr, dlen);
			mp->b_wptr += dlen;
			mp->b_cond = dp;
			np_txprim(sccp, msg, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_optmgmt_req(struct sccp *sccp, queue_t *q, mblk_t *msg,
	      caddr_t qptr, size_t qlen, np_ulong flags)
{
	N_optmgmt_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + qlen, BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_OPTMGMT_REQ;
		p->QOS_length = qlen;
		p->QOS_offset = qlen ? sizeof(*p) : 0;
		p->OPTMGMT_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(qptr, mp->b_qptr, qlen);
		mp->b_wptr += qlen;
		if (sccp_get_n_state(sccp) == NS_IDLE)
			sccp_set_n_state(sccp, NS_WACK_OPTREQ);
		np_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_datack_req(struct sccp *sccp, queue_t *q, mblk_t *msg)
{
	N_datack_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_DATACK_REQ;
		mp->b_wptr += sizeof(*p);
		np_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_reset_req(struct sccp *sccp, queue_t *q, mblk_t *msg, np_ulong reason)
{
	N_reset_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_RESET_REQ;
		p->RESET_reason = reason;
		mp->b_wptr += sizeof(*p);
		sccp_set_n_state(sccp, NS_WCON_CREQ);
		np_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}
static inline fastcall int
n_reset_res(struct sccp *sccp, queue_t *q, mblk_t *msg)
{
	N_reset_res_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->PRIM_type = N_RESET_RES;
		mp->b_wptr += sizeof(*p);
		sccp_set_n_state(sccp, NS_WACK_RRES);
		np_txprim(sccp, msg, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * TPI MESSAGES FROM ABOVE
 */

/**
 * t_conn_req: - process T_CONN_REQ from upstream
 * @sccp: private structure (locked)
 * @q: active queue (write queue)
 * @mp: the T_CONN_REQ primitive
 *
 * There is a mismatch between TPI and NPI here.  TPI returns T_ERROR_ACK or T_OK_ACK and then
 * T_CONN_CON or T_DISCON_IND wherease NPI only returns N_ERROR_ACK or N_CONN_CON or N_DISCON_IND
 * (that is no N_OK_ACK).  There are two possibilities for this mismatch: postpone returning
 * T_OK_ACK until N_CONN_CON is received, or return T_OK_ACK immediately and issue T_DISCON_IND
 * if an N_ERROR_ACK arrives.  Another approach is a hybrid of the two: issue the N_CONN_REQ with
 * the private structure unlocked and then reaquire the lock and see whether an N_ERROR_ACK has
 * already been processed: if so, return, otherwise issue the T_OK_ACK and await N_CONN_CON or
 * N_DISCON_IND.  In the event that the N_ERROR_ACK was delayed, if an N_ERROR_ACK is received in
 * the TS_WCON_CREQ state, change it to a T_DISCON_IND.
 */
noinline fastcall int
t_conn_req(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_conn_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (unlikely(!MBLKIN(mp, p->DEST_offset, p->DEST_length)))
		goto badaddr;
	if (unlikely(!MBLKIN(mp, p->OPT_offset, p->OPT_length)))
		got badopt;

	if (unlikely(sccp_get_t_state(sccp) != TS_IDLE))
		goto outstate;
	sccp_set_t_state(sccp, TS_WACK_CREQ);
	/* FIXME: format options appropriately */
	{
		struct strbuf dst =
		    { -1, p->DEST_length, mp->b_rptr + p->DEST_offset };
		struct strbuf qos = { -1, 0, NULL };

		if ((err = n_conn_req(sccp, q, NULL, 0, &dst, &qos, mp->b_cont)))
			return (err);
	}
	if (sccp_get_t_state(sccp) == TS_WACK_CREQ) {
		if ((err = t_ok_ack(sccp, q, mp, T_CONN_REQ)))
			return (err);
	} else
		freemsg(mp);
	return (0);
      outstate:
	err = TOUTSTATE;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return t_error_reply(sccp, q, mp, T_CONN_REQ, err, __FUNCTION__);
}
noinline fastcall int
t_conn_res(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_conn_res *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (unlikely(!MBLKIN(mp, p->OPT_offset, p->OPT_length)))
		goto badopt;
	/* FIXME: format options into qos parameters */
	{
		struct strbuf res = { -1, 0, NULL };
		struct strbuf qos = { -1, 0, NULL };

		return n_conn_res(sccp, q, mp, p->ACCEPTOR_id, 0, &res, &qos,
				  p->SEQ_number, mp->b_cont);
	}
      badopt:
	err = TBADOPT;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return t_error_reply(sccp, q, mp, T_CONN_RES, err, __FUNCTION__);
}
noinline fastcall int
t_discon_req(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_discon_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	/* FIXME: set reason, fake out responding address */
	{
		struct strbuf res = { -1, 0, NULL };

		return n_discon_req(sccp, q, mp, 0, &res, p->SEQ_number, mp->b_cont);
	}
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return t_error_reply(sccp, q, mp, T_DISCON_REQ, err, __FUNCTION__);
}

/**
 * t_data_req_slow: - process T_DATA_REQ primitive with errors
 * @sccp: private structure (locked)
 * @q: active queue
 * @mp: the T_DATA_REQ message
 *
 * This is the slow path for a T_DATA_REQ primitive received with detected errors.  This is
 * performed in this separate function to keep the error handling code away from the fast path.
 */
noinline fastcall __unlikely int
t_data_req_slow(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_data_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	switch (__builtin_expect(sccp_get_t_state(sccp), TS_DATA_XFER)) {
	case TS_DATA_XFER:
	case TS_WREQ_ORDREL:
		break;
	case TS_IDLE:
		goto discard;
	default:
		goto oustate;
	}
	return n_data_req(sccp, q, mp, p->MORE_flag, mp->b_cont);
      outstate:
	err = TOUTSTATE;
	goto error;
      discard:
	err = 0;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return t_error_reply(sccp, q, mp, T_DATA_REQ, err, __FUNCTION__);
}

/**
 * t_data_req: - process T_DATA_REQ primitive
 * @sccp: private structure (locked)
 * @q: active queue
 * @mp: the T_DATA_REQ message
 *
 * This is the fast path for processing the T_DATA_REQ primitive.  If an error is detected, the
 * slow path is taken.
 */
noinline fastcall __hot_write int
t_data_req(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_data_req *p = (typeof(p)) mp->b_rptr;
	N_data_req_t *r = (typeof(r)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto goslow;
	switch (__builtin_expect(sccp_get_t_state(sccp), TS_DATA_XFER)) {
	case TS_DATA_XFER:
	case TS_WREQ_ORDREL:
		break;
	case TS_IDLE:
		goto goslow;
	default:
		goto goslow;
	}
	if (unlikely(!bcanputnext(sccp->wq, mp->b_band)))
		goto goslow;
	/* When everything checks out OK, the T_DATA_REQ primitive is transformed
	   in-place to an N_DATA_REQ primitive and the message is passed downstream. 
	   Note that T_DATA_REQ is indeed equal to N_DATA_REQ. */
#if (T_DATA_REQ != N_DATA_REQ)
	r->PRIM_type = N_DATA_REQ;
#endif
	r->DATA_xfer_flags = (p->MORE_flag ? N_MORE_DATA_FLAG : 0);
	mi_strlog(q, STRLOGDA, SL_TRACE, "N_DATA_REQ ->");
	putnext(sccp->wq, mp);
	return (0);
      goslow:
	/* keep error path away from the fast path */
	return t_data_req_slow(sccp, q, mp);
}

/**
 * t_exdata_req_slow: - process T_EXDATA_REQ primitive with errors
 * @sccp: private structure (locked)
 * @q: active queue
 * @mp: the T_EXDATA_REQ message
 *
 * This is the slow path for a T_EXDATA_REQ primitive received with detected errors.  This is
 * performed in this separate function to keep the error handling code away from the fast path.
 */
noinline fastcall __unlikely int
t_exdata_req_slow(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_exdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (unlikely(p->MORE_flag))
		goto badflag;
	return n_exdata_req(sccp, q, mp, mp->b_cont);
      badflag:
	err = TBADFLAG;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return t_error_reply(sccp, q, mp, T_EXDATA_REQ, err, __FUNCTION__);
}

/**
 * t_exdata_req: - process T_EXDATA_REQ primitive
 * @sccp: private structure (locked)
 * @q: active queue
 * @mp: the T_EXDATA_REQ message
 *
 * This is the fast path for processing the T_EXDATA_REQ primitive.  If an error is detected, the
 * slow path is taken.
 */
noinline fastcall __hot_write int
t_exdata_req(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_exdata_req *p = (typeof(p)) mp->b_rptr;
	N_data_req_t *r = (typeof(r)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto goslow;
	if (unlikely(p->MORE_flag))
		goto goslow;
	if (unlikely(!bcanputnext(sccp->wq, mp->b_band)))
		goto goslow;
	/* When everything checks out OK, the T_EXDATA_REQ primitive is transformed
	   in-place to an N_EXDATA_REQ primitive and the message is passed
	   downstream.  Note that T_EXDATA_REQ is indeed equal to N_EXDATA_REQ. */
#if (T_EXDATA_REQ != N_EXDATA_REQ)
	r->PRIM_type = N_EXDATA_REQ;
#endif
	mp->b_wptr = (unsigned char *) (r + 1);
	mi_strlog(q, STRLOGDA, SL_TRACE, "N_EXDATA_REQ ->");
	putnext(sccp->wq, mp);
	return (0);
      goslow:
	/* keep error handling away from the fast path */
	return t_exdata_req(sccp, q, mp);
}
noinline fastcall int
t_info_req(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_info_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	return n_info_req(sccp, q, mp);
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return t_error_reply(sccp, q, mp, T_INFO_REQ, err, __FUNCTION__);
}
noinline fastcall int
t_bind_req(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_bind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (unlikely(!MBLKIN(mp, p->ADDR_offset, p->ADDR_length)))
		goto badaddr;
	if (unlikely(sccp_get_t_state(sccp) != TS_UNBND))
		goto outstate;
	/* FIXME: handle flags and protocol ids */
	return n_bind_req(sccp, q, mp, (caddr_t) (mp->b_rptr + p->ADDR_offset),
			  p->ADDR_length, p->CONIND_number, 0, NULL, 0);
      outstate:
	err = TOUTSTATE;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return t_error_reply(sccp, q, mp, T_BIND_REQ, err, __FUNCTION__);
}
noinline fastcall int
t_unbind_req(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_unbind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (unlikely(sccp_get_t_state(sccp) != TS_IDLE))
		goto outstate;
	return n_unbind_req(sccp, q, mp);
      outstate:
	err = TOUTSTATE;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return t_error_reply(sccp, q, mp, T_UNBIND_REQ, err, __FUNCTION__);
}

/**
 * t_unitdata_req_slow: - process T_UNITDATA_REQ primitive with errors
 * @sccp: private structure (locked)
 * @q: active queue
 * @mp: the T_UNITDATA_REQ message
 *
 * This is the slow path for a T_UNITDATA_REQ primitive received with detected errors.  This is
 * performed in this separate function to keep the error handling code away from the fast path.
 */
noinline fastcall __unlikely int
t_unitdata_req_slow(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (likely(p->DEST_length != 0)) {
		if (unlikely(!MBLKIN(mp, p->DEST_offset, p->DEST_length)))
			goto badaddr;
		if (unlikely(p->DEST_offset < sizeof(*p)))
			goto badaddr;
	}
	if (unlikely(p->OPT_length != 0)) {
		if (unlikely(!MBLKIN(mp, p->OPT_offset, p->OPT_length)))
			goto badopt;
		if (unlikely(p->OPT_offset < sizeof(*p)))
			goto badaddr;
	}
	switch (sccp_get_t_state(sccp)) {
	case TS_IDLE:
		goto discard;
	case TS_DATA_XFER:
	case TS_WREQ_ORDREL:
		break;
	default:
		goto outstate;
	}
	/* XXX: options must be set separately by issuing M_PROTO type N_OPTMGMT_REQ 
	   primitives first. */
	return n_unitdata_req(sccp, q, mp, (caddr_t) (mp->b_rptr + p->DEST_offset),
			      p->DEST_length, mp->b_cont);
      outstate:
	err = TOUTSTATE;
	goto error;
      discard:
	err = 0;
	goto error;
      badopt:
	err = TBADOPT;
	goto error;
      badaddr:
	err = TBADADDR;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return t_error_reply(sccp, q, mp, T_UNITDATA_REQ, err, __FUNCTION__);
}

/**
 * t_unitdata_req: - process T_UNITDATA_REQ primitive
 * @sccp: private structure (locked)
 * @q: active queue
 * @mp: the T_UNITDATA_REQ message
 *
 * This is the fast path for processing the T_UNITDATA_REQ primitive.  If an error is detected,
 * the slow path is taken.
 */
noinline fastcall __hot_write int
t_unitdata_req(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_unitdata_req *p = (typeof(p)) mp->b_rptr;
	N_unitdata_req_t *r = (typeof(r)) mp->b_wptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto goslow;
	if (unlikely(p->DEST_length == 0))
		goto goslow;
	if (unlikely(!MBLKIN(mp, p->DEST_offset, p->DEST_length)))
		goto goslow;
	if (unlikely(p->DEST_offset < sizeof(*p)))
		goto goslow;
	if (unlikely(!MBLKIN(mp, p->OPT_offset, p->OPT_length)))
		goto goslow;
	if (unlikely(p->OPT_length != 0))
		goto goslow;
	switch (sccp_get_t_state(sccp)) {
	case TS_IDLE:
		goto goslow;
	case TS_DATA_XFER:
	case TS_WREQ_ORDREL:
		break;
	default:
		goto goslow;
	}
	if (unlikely(!bcanputnext(sccp->wq, mp->b_band)))
		goto goslow;
	/* When everything checks out OK, the T_UNITDATA_REQ primitive is
	   transformed in-place into an N_UNITDATA_REQ primitive and the message is
	   passed downstream.  Note that T_UNITDATA_REQ is indeed equal to N_UNITDATA_REQ. */
#if (T_UNITDATA_REQ != N_UNITDATA_REQ)
	r->PRIM_type = N_UNITDATA_REQ;
#endif
#if 0
	r->RESERVED_field[0] = 0;
	r->RESERVED_field[1] = 0;
#endif
	mi_strlog(q, STRLOGDA, SL_TRACE, "N_UNITDATA_REQ ->");
	putnext(sccp->wq, mp);
	return (0);
      goslow:
	/* keep error handling away from the fast path */
	return t_unitdata_req_slow(sccp, q, mp);
}
noinline fastcall int
t_optmgmt_req(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (unlikely(!MBLKIN(mp, p->OPT_offset, p->OPT_length)))
		goto badopt;
      badopt:
	err = TBADOPT;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return t_error_reply(sccp, q, mp, T_OPTMGMT_REQ, err, __FUNCTION__);
}
noinline fastcall int
t_ordrel_req(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_ordrel_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	goto notsupport;
      notsupport:
	err = TNOTSUPPORT;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return t_error_reply(sccp, q, mp, T_ORDREL_REQ, err, __FUNCTION__);
}
noinline fastcall int
t_optdata_req(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_optdata_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	if (unlikely(!MBLKIN(mp, p->OPT_offset, p->OPT_length)))
		goto badopt;
	/* XXX: options must be set separately by issuing M_PROTO type N_OPTMGMT_REQ 
	   primitives first. */
	if (p->DATA_flag & T_ODF_EX)
		return n_exdata_req(sccp, q, mp, mp->b_cont);
	return n_data_req(sccp, q, mp,
			  (p->DATA_flag & T_ODF_MORE) ? N_MORE_DATA_FLAG : 0,
			  mp->b_cont);
      badopt:
	err = TBADOPT;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return t_error_reply(sccp, q, mp, T_OPTDATA_REQ, err, __FUNCTION__);
}
noinline fastcall int
t_addr_req(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_addr_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	return t_addr_ack(sccp, q, mp, &sccp->t.laddr, &sccp->t.raddr);
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return t_error_reply(sccp, q, mp, T_ADDR_REQ, err, __FUNCTION__);
}
noinline fastcall int
t_capability_req(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	struct T_capability_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	return t_capability_ack(sccp, q, mp);
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return t_error_reply(sccp, q, mp, T_CAPABILITY_REQ, err, __FUNCTION__);
}
noinline fastcall int
t_other_req(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	t_scalar_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	err = -EPROTO;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return t_error_reply(sccp, q, mp, *p, err, __FUNCTION__);
}

/*
 * NPI MESSAGES FROM BELOW
 */

noinline fastcall __unlikely int
n_error_reply(struct sccp *sccp, queue_t *q, mblk_t *msg, np_ulong prim,
	      np_long error, const char *func)
{
	switch (error) {
	case -EAGAIN:
	case -ENOBUFS:
	case -ENOMEM:
	case -EDEADLK:
	case -EBUSY:
		return (err);
	case 0:		/* discard situation */
		freemsg(msg);
		return (0);
	}
	switch (prim) {
	case N_DATA_IND:
	case N_EXDATA_IND:
	case N_DATACK_IND:
	case N_UDERROR_IND:
	case N_UNITDATA_IND:
		/* If the primitive is a data primitive, log with appropriate error
		   level. */
		mi_strlog(q, STRLOGDA, SL_TRACE, "ERROR: %s: %s: %s", func,
			  np_errname(error), np_errdesc(error));
		goto merror;
	}
	mi_strlog(q, STRLOGNO, SL_TRACE | SL_ERROR, "ERROR: %s: %s: %s", func,
		  np_errname(error), np_errdesc(error));
	goto merror;
      merror:
	return m_error(sccp, q, msg, EPROTO, EPROTO);
}

noinline fastcall int
n_conn_ind(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_conn_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

#if 0
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, p->DEST_offset, p->DEST_length)))
		goto badaddr;
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, p->SRC_offset, p->SRC_length)))
		goto badaddr;
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, p->QOS_offset, p->QOS_length)))
		goto badqos;
#endif
	switch (sccp_get_n_state(sccp)) {
	case NS_IDLE:
	case NS_WRES_CIND:
		break;
	default:
		goto outstate;
	}
	sccp_set_n_state(sccp, NS_WRES_CIND);
	/* FIXME: convert connect flags and QOS parameters into options */
	return t_conn_ind(sccp, q, mp, (caddr_t) (mp->b_rptr + p->SRC_offset),
			  p->SRC_length, NULL, 0, p->SEQ_number, mp->b_cont);
      oustate:
	err = NOUTSTATE;
	goto error;
      badqos:
	err = NBADQOSPARAM;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
#if 0
      badprim:
	err = -EMSGSIZE;
	goto error;
#endif
      error:
	return n_error_reply(sccp, q, mp, N_CONN_IND, err, __FUNCTION__);
}
noinline fastcall int
n_conn_con(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_conn_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

#if 0
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, p->RES_offset, p->RES_length)))
		goto badaddr;
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, p->QOS_offset, p->QOS_length)))
		goto badqos;
#endif
	if (unlikely(sccp_get_n_state(sccp) != NS_WCON_CREQ))
		goto outstate;
	sccp_set_n_state(sccp, NS_DATA_XFER);
	/* FIXME: convert QOS parameters and connect flags into options */
	return t_conn_con(sccp, q, mp, (caddr_t) (mp->b_rptr + p->RES_offset),
			  p->RES_length, NULL, 0, mp->b_cont);
      oustate:
	err = NOUTSTATE;
	goto error;
#if 0
      badqos:
	err = NBADQOSPARAM;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
#endif
      error:
	return n_error_reply(sccp, q, mp, N_CONN_CON, err, __FUNCTION__);
}

/**
 * n_discon_ind: - process N_DISCON_IND received from downstream
 * @sccp: private structure (locked)
 * @q: active queue (read queue)
 * @mp: the N_DISCON_IND message
 */
noinline fastcall int
n_discon_ind(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_discon_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

#if 0
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, p->RES_offset, p->RES_length)))
		goto badaddr;
#endif
	sccp_set_n_state(sccp, NS_IDLE);
	/* A wrinkle of the impeadance mismatch between NPI and TPI is that if we
	   have not yet issued the T_OK_ACK in response to a T_CONN_REQ and now
	   receive an N_DISCON_IND, we need to issue the T_OK_ACK before issuing the 
	   T_DISCON_IND to meet the TPI user's expectations. */
	if (sccp_get_t_state(sccp) == TS_WACK_CREQ)
		if ((err = t_ok_ack(sccp, q, NULL, T_CONN_REQ)))
			return (err);
	/* FIXME: convert disconnet origin and reason */
	return t_discon_ind(sccp, q, mp, p->DISCON_reason, p->SEQ_number,
			    mp->b_cont);
#if 0
      badaddr:
	err = NBADADDR;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
#endif
      error:
	return n_error_reply(sccp, q, mp, N_DISCON_IND, err, __FUNCTION__);
}
noinline fastcall __unlikely int
n_data_ind_slow(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	switch (sccp_get_n_state(sccp)) {
	case NS_IDLE:
		goto discard;
	case NS_DATA_XFER:
	case NS_WRES_RIND:
	case NS_WCON_RREQ:
		break;
	default:
		goto outstate;
	}
	return t_data_ind(sccp, q, mp, p->DATA_xfer_flags & N_MORE_DATA_FLAG,
			  mp->b_cont);
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	err = 0;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_reply(sccp, q, mp, N_DATA_IND, err, __FUNCTION__);
}
noinline fastcall int
n_data_ind(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;
	struct T_data_ind *r = (typeof(r)) mp->b_rptr;

	switch (__builtin_expect(sccp_get_n_state(sccp), NS_DATA_XFER)) {
	case NS_DATA_XFER:
	case NS_WRES_RIND:
	case NS_WCON_RREQ:
		break;
	default:
		goto goslow;
	}
	if (unlikely(!bcanputnext(sccp->rq, mp->b_band)))
		goto goslow;
	/* When everything checks out OK simply pass the message upstream after
	   tranforming the N_DATA_IND message into a T_DATA_IND message.  Note that
	   N_DATA_IND is indeed equal to T_DATA_IND. */
#if (N_DATA_IND != T_DATA_IND)
	r->PRIM_type = T_DATA_IND;
#endif
	r->MORE_flag = (p->DATA_xfer_flags & N_MORE_DATA_FLAG) ? 1 : 0;
	mi_strlog(q, STRLOGDA, SL_TRACE, "<- T_DATA_IND");
	putnext(sccp->rq, mp);
	return (0);
      goslow:
	return n_data_ind_slow(sccp, q, mp);
}
noinline fastcall int
n_exdata_ind_slow(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_exdata_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	switch (sccp_get_n_state(sccp)) {
	case NS_IDLE:
		goto discard;
	case NS_DATA_XFER:
	case NS_WRES_RIND:
	case NS_WCON_RREQ:
		break;
	default:
		goto outstate;
	}
	return t_exdata_ind(sccp, q, mp, 0, mp->b_cont);
      outstate:
	err = NOUTSTATE;
	goto error;
      discard:
	err = 0;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_reply(sccp, q, mp, N_EXDATA_IND, err, __FUNCTION__);
}
noinline fastcall int
n_exdata_ind(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_exdata_ind_t *p = (typeof(p)) mp->b_rptr;
	struct T_exdata_ind *r = (typeof(r)) mp->b_rptr;

	switch (__builtin_expect(sccp_get_n_state(sccp), NS_DATA_XFER)) {
	case NS_DATA_XFER:
	case NS_WRES_RIND:
	case NS_WCON_RREQ:
		break;
	default:
		goto goslow;
	}
	if (unlikely(!bcanputnext(sccp->rq, mp->b_band)))
		goto goslow;
	/* When everything checks out OK simply pass the message upstream after
	   tranforming the N_EXDATA_IND message into a T_EXDATA_IND message.  Note
	   that N_EXDATA_IND is indeed equal to T_EXDATA_IND. */
#if (N_EXDATA_IND != T_EXDATA_IND)
	r->PRIM_type = T_EXDATA_IND;
#endif
	r->MORE_flag = 0;
	mp->b_wptr = (unsigned char *) (r + 1);
	mi_strlog(q, STRLOGDA, SL_TRACE, "<- T_EXDATA_IND");
	putnext(sccp->rq, mp);
	return (0);
      goslow:
	return n_exdata_ind(sccp, q, mp);
}
noinline fastcall t_uscalar_t
np_t_serv_type(np_ulong type)
{
	t_uscalar_t tp_type = 0;

	if (type & N_CONS)
		tp_type |= T_COTS;
	if (type & N_CLNS)
		tp_type |= T_CLTS;
	return (tp_type);
}
noinline fastcall t_scalar_t
np_t_state(np_ulong state)
{
	switch (state) {
	case NS_UNBND:
		return (TS_UNBND);
	case NS_WACK_BREQ:
		return (TS_WACK_BREQ);
	case NS_WACK_UREQ:
		return (TS_WACK_UREQ);
	case NS_IDLE:
		return (TS_IDLE);
	case NS_WACK_OPTREQ:
		return (TS_WACK_OPTREQ);
	case NS_WACK_RRES:
		return (TS_DATA_XFER);
	case NS_WCON_CREQ:
		return (TS_WCON_CREQ);
	case NS_WRES_CIND:
		return (TS_WRES_CIND);
	case NS_WACK_CRES:
		return (TS_WACK_CREQ);
	case NS_DATA_XFER:
		return (TS_DATA_XFER);
	case NS_WCON_RREQ:
		return (TS_DATA_XFER);
	case NS_WRES_RIND:
		return (TS_DATA_XFER);
	case NS_WACK_DREQ6:
		return (TS_WACK_DREQ6);
	case NS_WACK_DREQ7:
		return (TS_WACK_DREQ7);
	case NS_WACK_DREQ9:
		return (TS_WACK_DREQ9);
	case NS_WACK_DREQ10:
		return (TS_WACK_DREQ9);
	case NS_WACK_DREQ11:
		return (TS_WACK_DREQ9);
	default:
		return (-1);
	}
}
noinline fastcall int
n_info_ack(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_info_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

#if 0
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
#endif
	sccp->n.info.PRIM_type = p->PRIM_type;
	sccp->n.info.NSDU_size = p->NSDU_size;
	sccp->n.info.ENSDU_size = p->ENSDU_size;
	sccp->n.info.CDATA_size = p->CDATA_size;
	sccp->n.info.DDATA_size = p->DDATA_size;
	sccp->n.info.ADDR_size = p->ADDR_size;
	sccp->n.info.ADDR_length = p->ADDR_length;
	sccp->n.info.ADDR_offset = p->ADDR_offset;
	sccp->n.info.QOS_length = p->QOS_length;
	sccp->n.info.QOS_offset = p->QOS_offset;
	sccp->n.info.QOS_range_length = p->QOS_range_length;
	sccp->n.info.QOS_range_offset = p->QOS_range_offset;
	sccp->n.info.OPTIONS_flags = p->OPTIONS_flags;
	sccp->n.info.NIDU_size = p->NIDU_size;
	sccp->n.info.SERV_type = p->SERV_type;
	sccp->n.info.CURRENT_state = p->CURRENT_state;
	sccp->n.info.PROVIDER_type = p->PROVIDER_type;
	sccp->n.info.NODU_size = p->NODU_size;
	sccp->n.info.PROTOID_length = p->PROTOID_length;
	sccp->n.info.PROTOID_offset = p->PROTOID_offset;
	sccp->n.info.NPI_version = p->NPI_version;

	sccp->t.info.PRIM_type = T_CAPABILITY_ACK;
	sccp->t.info.CAP_bits1 |= TC1_INFO;
	sccp->t.info.INFO_ack.PRIM_type = T_INFO_ACK;
	sccp->t.info.INFO_ack.TSDU_size = p->NSDU_size;
	sccp->t.info.INFO_ack.ETSDU_size = p->ENSDU_size;
	sccp->t.info.INFO_ack.CDATA_size = p->CDATA_size;
	sccp->t.info.INFO_ack.DDATA_size = p->DDATA_size;
	sccp->t.info.INFO_ack.ADDR_size = p->ADDR_size;
	sccp->t.info.INFO_ack.OPT_size = -1;
	sccp->t.info.INFO_ack.TIDU_size = p->NIDU_size;
	sccp->t.info.INFO_ack.SERV_type = np_t_serv_type(p->SERV_type);
	sccp->t.info.INFO_ack.CURRENT_state = np_t_state(p->CURRENT_state);
	sccp->t.info.INFO_ack.PROVIDER_flag = 0;
	return t_info_ack(sccp, q, mp);
#if 0
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_reply(sccp, q, mp, N_INFO_ACK, err, __FUNCTION__);
#endif
}
noinline fastcall int
n_bind_ack(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_bind_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

#if 0
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, p->ADDR_offset, p->ADDR_length)))
		goto badaddr;
#endif
	if (unlikely(sccp_get_n_state(sccp) != NS_WACK_BREQ))
		goto outstate;
	sccp_set_n_state(sccp, NS_IDLE);
	return t_bind_ack(sccp, q, mp, (caddr_t) (mp->b_rptr = p->ADDR_offset),
			  p->ADDR_length, p->CONIND_number)
      oustate:
	err = NOUTSTATE;
	goto error;
#if 0
      badaddr:
	err = NBADADDR;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
#endif
      error:
	return n_error_reply(sccp, q, mp, N_BIND_ACK, err, __FUNCTION__);
}

/**
 * n_error_ack: - process N_ERROR_ACK recieved from downstream
 * @sccp: private structure (locked)
 * @q: active queue (read queue)
 * @mp: the N_ERROR_ACK message
 */
noinline fastcall int
n_error_ack(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_error_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

#if 0
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
#endif
	switch (sccp_get_n_state(sccp)) {
	case NS_WACK_BREQ:
		sccp_set_n_state(sccp, NS_UNBND);
		break;
	case NS_WACK_UREQ:
		sccp_set_n_state(sccp, NS_IDLE);
		break;
	case NS_WACK_OPTREQ:
		sccp_set_n_state(sccp, NS_IDLE);
		break;
	case NS_WACK_RRES:
		if (sccp->coninds)
			sccp_set_n_state(sccp, NS_WRES_CIND);
		else
			sccp_set_n_state(sccp, NS_IDLE);
		break;
	case NS_WCON_CREQ:
		sccp_set_n_state(sccp, NS_IDLE);
		if (sccp_get_t_state(sccp) != TS_WCON_CREQ)
			break;
		/* In this state we have already issued a T_OK_ACK back in response
		   to a received T_CONN_REQ and the upper module is not expecting a
		   T_ERROR_ACK. The event must be transformed into a T_DISCON_IND
		   and issued upstream. */
		return t_discon_ind(sccp, q, mp,
				    np_t_error(p->NPI_error, p->UNIX_error), 0,
				    NULL);
	case NS_WACK_CRES:
		if (sccp->coninds)
			sccp_set_n_state(sccp, NS_WRES_CIND);
		else
			sccp_set_n_state(sccp, NS_IDLE);
		break;
	case NS_WCON_RREQ:
		sccp_set_n_state(sccp, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ6:
		sccp_set_n_state(sccp, NS_WCON_CREQ);
		break;
	case NS_WACK_DREQ7:
		sccp_set_n_state(sccp, NS_WRES_CIND);
		break;
	case NS_WACK_DREQ9:
		sccp_set_n_state(sccp, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ10:
		sccp_set_n_state(sccp, NS_WCON_RREQ);
		break;
	case NS_WACK_DREQ11:
		sccp_set_n_state(sccp, NS_WRES_RIND);
		break;
	}
	return t_error_ack(sccp, q, mp, np_t_prim(p->ERROR_prim),
			   np_t_error(p->NPI_error, p->UNIX_error));
#if 0
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_reply(sccp, q, mp, N_ERROR_ACK, err, __FUNCTION__);
#endif
}
noinline fastcall int
n_ok_ack(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

#if 0
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
#endif
	switch (sccp_get_n_state(sccp)) {
	case NS_WACK_BREQ:
		sccp_set_n_state(sccp, NS_IDLE);
		break;
	case NS_WACK_UREQ:
		sccp_set_n_state(sccp, NS_UNBND);
		break;
	case NS_WACK_OPTREQ:
		sccp_set_n_state(sccp, NS_IDLE);
		break;
	case NS_WACK_RRES:
		if (sccp->coninds > 1)
			sccp_set_n_state(sccp, NS_WRES_CIND);
		else
			sccp_set_n_state(sccp, NS_IDLE);
		break;
	case NS_WACK_CRES:
		sccp_set_n_state(sccp, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ6:
		sccp_set_n_state(sccp, NS_IDLE);
		break;
	case NS_WACK_DREQ7:
		sccp_set_n_state(sccp, NS_IDLE);
		break;
	case NS_WACK_DREQ9:
		sccp_set_n_state(sccp, NS_IDLE);
		break;
	case NS_WACK_DREQ10:
		sccp_set_n_state(sccp, NS_IDLE);
		break;
	case NS_WACK_DREQ11:
		sccp_set_n_state(sccp, NS_IDLE);
		break;
	}
	/* FIXME: convert primitive */
	return t_ok_ack(sccp, q, mp, p->CORRECT_prim);
#if 0
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_reply(sccp, q, mp, N_OK_ACK, err, __FUNCTION__);
#endif
}

noinline fastcall int
n_unitdata_ind_slow(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, p->SRC_offset, p->SRC_length)))
		goto badaddr;
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, p->DEST_offset, p->DEST_length)))
		goto badaddr;
	if (unlikely(sccp_get_n_state(sccp) == NS_IDLE))
		goto discard;
	switch (sccp_get_n_state(sccp), NS_IDLE) {
	case NS_IDLE:
	case NS_WACK_UREQ:
		break;
	default:
		goto discard;
	}
	/* XXX: what about destination address and options? */
	return t_unitdata_ind(sccp, q, mp, (caddr_t) (mp->b_rptr + p->SRC_offset),
			      p->SRC_length, NULL, 0, mp->b_cont);
      discard:
	err = 0;
	goto error;
      badaddr:
	err = NBADADDR;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_reply(sccp, q, mp, N_UNITDATA_IND, err, __FUNCTION__);
}
/**
 * n_unitdata_ind: - process N_UNITDATA_IND from below
 * @sccp: private structure (locked)
 * @q: active queue (lower read queue)
 * @mp: the primitive
 */
noinline fastcall int
n_unitdata_ind(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;
	struct T_unitdata_ind *r = (typeof(r)) mp->b_rptr;

	switch (__builtin_expect(sccp_get_n_state(sccp), NS_IDLE)) {
	case NS_IDLE:
	case NS_WACK_UREQ:
		break;
	default:
		goto goslow;
	}
	/* When everything checks out OK, simply pass the message upstream after
	   transforming the N_UNITDATA_IND message into a T_UNITDATA_IND message.
	   Note that T_UNITDATA_IND is indeed equal to N_UNITDATA_IND. */
#if (N_UNITDATA_IND != T_UNITDATA_IND)
	r->PRIM_type = T_UNITDATA_IND;
#endif
	r->SRC_length = p->SRC_length;
	r->SRC_offset = p->SRC_offset;
	r->OPT_length = 0;
	r->OPT_offset = 0;
	mi_strlog(q, STRLOGDA, SL_TRACE, "<- T_UNITDATA_IND");
	putnext(sccp->rq, mp);
	return (0);
      goslow:
	return n_unitdata_ind_slow(sccp, q, mp);
}

noinline fastcall int
n_uderror_ind_slow(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_uderror_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, p->DEST_offset, p->DEST_length)))
		goto badaddr;
	/* FIXME: convert error types */
	return t_uderror_ind(sccp, s, mp, (caddr_t) (mp->b_rptr + p->DEST_offset),
			     p->DEST_length, NULL, 0, p->ERROR_type, mp->b_cont);
      badaddr:
	err = NBADADDR;
	goto error;
      badprim:
	err = -EMSGSIZE;
	goto error;
      error:
	return n_error_reply(sccp, q, mp, N_UDERROR_IND, err, __FUNCTION__);
}
noinline fastcall int
n_uderror_ind(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_uderror_ind_t *p = (typeof(p)) mp->b_rptr;
	struct T_uderror_ind *r = (typeof(r)) mp->b_rptr;

	switch (__builtin_expect(sccp_get_n_state(sccp), NS_IDLE)) {
	case NS_IDLE:
	case NS_WACK_UREQ:
		break;
	default:
		goto goslow;
	}
	if (unlikely(!bcanputnext(sccp->rq, mp->b_band)))
		goto goslow;

#if (N_UDERROR_IND != T_UDERROR_IND)
	r->PRIM_type = T_UDERROR_IND;
#endif
	DEST_length;
	DEST_offset;
	RESERVED_field;
	ERROR_type;


	/* FIXME: convert error types */
	return t_uderror_ind(sccp, s, mp, (caddr_t) (mp->b_rptr + p->DEST_offset),
			     p->DEST_length, NULL, 0, p->ERROR_type, mp->b_cont);
goslow:
	return n_uderror_ind(sccp, q, mp);
}
noinline fastcall int
n_datack_ind(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_datack_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

#if 0
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
#endif
	goto notsupport;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
#if 0
      badprim:
	err = -EMSGSIZE;
	goto error;
#endif
      error:
	return n_error_reply(sccp, q, mp, N_DATACK_IND, err, __FUNCTION__);
}
noinline fastcall int
n_reset_ind(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_reset_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

#if 0
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
#endif
	goto notsupport;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
#if 0
      badprim:
	err = -EMSGSIZE;
	goto error;
#endif
      error:
	return n_error_reply(sccp, q, mp, N_RESET_IND, err, __FUNCTION__);
}
noinline fastcall int
n_reset_con(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	N_reset_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

#if 0
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
#endif
	goto notsupport;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
#if 0
      badprim:
	err = -EMSGSIZE;
	goto error;
#endif
      error:
	return n_error_reply(sccp, q, mp, N_RESET_CON, err, __FUNCTION__);
}
noinline fastcall int
n_other_ind(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	np_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

#if 0
	/* We can assume that the lower level knows what it is sending us. */
	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
#endif
	goto notsupport;
      notsupport:
	err = NNOTSUPPORT;
	goto error;
#if 0
      badprim:
	err = -EMSGSIZE;
	goto error;
#endif
      error:
	return n_error_reply(sccp, q, mp, *p, err, __FUNCTION__);
}

/*
 * M_DATA HANDLING
 */

static inline fastcall int
sccp_w_data(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	return n_data_req(sccp, q, mp, mp->b_cont);
}

static inline fastcall int
sccp_r_data(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	return t_data_ind(sccp, q, mp, mp->b_cont);
}

/*
 * M_(PC)PROTO HANDLING
 */

static inline fastcall int
sccp_w_proto(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	t_scalar_t prim;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto badprim;
	prim = *(t_scalar_t *) mp->b_rptr;

	tp_rxprim(sccp, mp);

	sccp_save_t_state(sccp);

	switch (prim) {
	case T_CONN_REQ:
		err = t_conn_req(sccp, q, mp);
		break;
	case T_CONN_RES:
		err = t_conn_res(sccp, q, mp);
		break;
	case T_DISCON_REQ:
		err = t_discon_req(sccp, q, mp);
		break;
	case T_DATA_REQ:
		err = t_data_req(sccp, q, mp);
		break;
	case T_EXDATA_REQ:
		err = t_exdata_req(sccp, q, mp);
		break;
	case T_INFO_REQ:
		err = t_info_req(sccp, q, mp);
		break;
	case T_BIND_REQ:
		err = t_bind_req(sccp, q, mp);
		break;
	case T_UNBIND_REQ:
		err = t_unbind_req(sccp, q, mp);
		break;
	case T_UNITDATA_REQ:
		err = t_unitdata_req(sccp, q, mp);
		break;
	case T_OPTMGMT_REQ:
		err = t_optmgmt_req(sccp, q, mp);
		break;
	case T_ORDREL_REQ:
		err = t_ordrel_req(sccp, q, mp);
		break;
	case T_OPTDATA_REQ:
		err = t_optdata_req(sccp, q, mp);
		break;
	case T_ADDR_REQ:
		err = t_addr_req(sccp, q, mp);
		break;
	case T_CAPABILITY_REQ:
		err = t_capability_req(sccp, q, mp);
		break;
	default:
		err = t_other_req(sccp, q, mp);
		break;
	}

	if (unlikely(err)) {
		sccp_restore_t_state(sccp);
		/* Sometimes errors may be returned just to restore state. */
		switch (err) {
		case -EAGAIN:
		case -EDEADLK:
		case -EBUSY:
		case -ENOBUFS:
		case -ENOMEM:
			break;
		default:
			err = 0;
			break;
		}
	}
	return (err);
      badprim:
	return m_error(sccp, q, mp, EPROTO);
}

static inline fastcall int
sccp_r_proto(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	np_ulong prim;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		goto badprim;

	prim = *(np_ulong *) mp->b_rptr;

	np_rxprim(sccp, mp);

	sccp_save_n_state(sccp);

	switch (prim) {
	case N_CONN_IND:
		err = n_conn_ind(sccp, q, mp);
		break;
	case N_CONN_CON:
		err = n_conn_con(sccp, q, mp);
		break;
	case N_DISCON_IND:
		err = n_discon_ind(sccp, q, mp);
		break;
	case N_DATA_IND:
		err = n_data_ind(sccp, q, mp);
		break;
	case N_EXDATA_IND:
		err = n_exdata_ind(sccp, q, mp);
		break;
	case N_INFO_ACK:
		err = n_info_ack(sccp, q, mp);
		break;
	case N_BIND_ACK:
		err = n_bind_ack(sccp, q, mp);
		break;
	case N_ERROR_ACK:
		err = n_error_ack(sccp, q, mp);
		break;
	case N_OK_ACK:
		err = n_ok_ack(sccp, q, mp);
		break;
	case N_UNITDATA_IND:
		err = n_unitdata_ind(sccp, q, mp);
		break;
	case N_UDERROR_IND:
		err = n_uderror_ind(sccp, q, mp);
		break;
	case N_DATACK_IND:
		err = n_datack_ind(sccp, q, mp);
		break;
	case N_RESET_IND:
		err = n_reset_ind(sccp, q, mp);
		break;
	case N_RESET_CON:
		err = n_reset_con(sccp, q, mp);
		break;
	default:
		err = n_other_ind(sccp, q, mp);
		break;
	}

	if (unlikely(err)) {
		sccp_restore_n_state(sccp);
		switch (err) {
		case -EAGAIN:
		case -EDEADLK:
		case -ENOBUFS:
		case -ENOMEM:
		case -EBUSY:
			break;
		default:
			err = 0;
			break;
		}
	}
	return (err);
      badprim:
	return m_error(sccp, q, mp, EPROTO);
}

/*
 * M_FLUSH HANDLING
 */

noinline fastcall int
sccp_r_flush(queue_t *q, mblk_t *mp)
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

noinline fastcall int
sccp_w_flush(queue_t *q, mblk_t *mp)
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

/*
 * STREAMS MESSAGE HANDLING
 */

static fastcall int
sccp_wsrv_msg(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return sccp_w_data(sccp, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sccp_w_proto(sccp, q, mp);
	case M_FLUSH:
		return sccp_w_flush(q, mp);
	default:
		if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

static fastcall int
sccp_wput_msg(queue_t *q, mblk_t *mp)
{
	struct sccp *sccp;
	int rtn = -EDEADLK;

	if (likely(!!(sccp = (struct sccp *) mi_trylock(q)))) {
		rtn = sccp_wsrv_msg(sccp, q, mp);
		mi_unlock((caddr_t) sccp);
	}
	return (rtn);
}

static fastcall int
sccp_rsrv_msg(struct sccp *sccp, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return sccp_r_data(sccp, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sccp_r_proto(sccp, q, mp);
	case M_FLUSH:
		return sccp_r_flush(q, mp);
	default:
		if (likely(pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band))) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

static fastcall int
sccp_rput_msg(queue_t *q, mblk_t *mp)
{
	struct sccp *sccp;
	int rtn = -EDEADLK;

	if (likely(!!(sccp = (struct sccp *) mi_trylock(q)))) {
		rtn = sccp_rsrv_msg(sccp, q, mp);
		mi_unlock((caddr_t) sccp);
	}

}

/*
 * QUEUE OPEN AND CLOSE PROCEDURES
 */

/**
 * sccp_wput: - SCCP write put procedure
 * @q: queue to which to put
 * @mp: message to put to queue
 */
static streamscall __hot_put int
sccp_wput(queue_t *q, mblk_t *mp)
{
	if (!pcmsg(DB_TYPE(mp)) && sccp_wput_msg(q, mp))
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	return (0);
}

/**
 * sccp_wsrv: - SCCP write service procedure
 * @q: queue to service
 */
static streamscall __hot_out int
sccp_wsrv(queue_t *q)
{
	struct sccp *sccp;
	mblk_t *mp;

	if (likely(!!(sccp = (struct sccp *) mi_trylock(q)))) {
		if (likely(!!(mp = getq(q)))) {
			do {
				if (unlikely(sccp_wsrv_msg(sccp, q, mp))) {
					if (unlikely(!putbq(q, mp))) {
						mp->b_band = 0;
						putbq(q, mp);
					}
					break;
				}
			} while (unlikely(!!(mp = getq(q))));
		}
		mi_unlock((caddr_t) sccp);
	}
	return (0);
}

/**
 * sccp_rsrv: - SCCP read service procedure
 * @q: queue to service
 */
static streamscall __hot_get int
sccp_rsrv(queue_t *q)
{
	struct sccp *sccp;
	mblk_t *mp;

	if (likely(!!(sccp = (struct sccp *) mi_trylock(q)))) {
		if (likely(!!(mp = getq(q)))) {
			do {
				if (unlikely(sccp_rsrv_msg(sccp, q, mp))) {
					if (unlikely(!putbq(q, mp))) {
						mp->b_band = 0;
						putbq(q, mp);
					}
					break;
				}
			} while (unlikely(!!(mp = getq(q))));
		}
		mi_unlock((caddr_t) sccp);
	}
	return (0);
}

/**
 * sccp_rput: SCCP read put procedure
 * @q: queue to which to put
 * @mp: message to put to queue
 */
static streamscall __hot_in int
sccp_rput(queue_t *q, mblk_t *mp)
{
	if (!pcmsg(DB_TYPE(mp)) && sccp_rput_msg(q, mp))
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;
			putq(q, mp);
		}
	return (0);
}

/**
 * sccp_qopen: - open a stream
 * @q: opening queue pair read queue
 * @devp: device number pointer
 * @oflags: open flags
 * @sflag: STREAMS flag
 * @credp: credentials (of opener)
 */
static streamscall int
sccp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *credp)
{
	struct sccp *sccp;
	N_info_req_t *p;
	mblk_t *mp;
	int err;

	if (q->q_ptr != NULL)
		return (0);
	if (unlikely(!(mp = allocb(sizeof(*p), BPRI_WAITOK))))
		return (ENOSR);
	if (unlikely((err = mi_open_comm(&sccp_opens, sizeof(*sccp), q, devp,
					 oflags, sflag, credp)))) {
		freeb(mp);
		return (err);
	}
	sccp = (struct sccp *) q->q_ptr;
	bzero(sccp, sizeof(*sccp));
	sccp->rq = q;
	sccp->wq = WR(q);
	sccp->cred = *credp;
	sccp->info.npi.PRIM_type = 0;
	sccp->info.npi.NSDU_size = -1;
	sccp->info.npi.ENSDU_size = -2;
	sccp->info.npi.CDATA_size = -2;
	sccp->info.npi.DDATA_size = -2;
	sccp->info.npi.ADDR_size = sizeof(struct sccp_addr);
	sccp->info.npi.OPTIONS_flags = 0;
	sccp->info.npi.NIDU_size = -1;
	sccp->info.npi.SERV_type = N_CLNS | N_CONS;
	sccp->info.npi.CURRENT_state = NS_UNBND;
	sccp->info.npi.PROVIDER_type = N_SNICFP;
	sccp->info.npi.NODU_size = 279;
	sccp->info.npi.NPI_version = N_VERSION_2;
	sccp->info.tpi.PRIM_type = 0;
	sccp->info.tpi.TSDU_size = -1;
	sccp->info.tpi.ETSDU_size = -2;
	sccp->info.tpi.CDATA_size = -2;
	sccp->info.tpi.DDATA_size = -2;
	sccp->info.tpi.ADDR_size = sizeof(struct sccp_addr);
	sccp->info.tpi.OPT_size = -1;
	sccp->info.tpi.TIDU_size = -1;
	sccp->info.tpi.SERV_type = T_CLTS | T_COTS;
	sccp->info.tpi.CURRENT_state = TS_UNBND;
	sccp->info.tpi.PROVIDER_flag = 0;
	/* issue immediate inforation request */
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	qprocson(q);
	putnext(q, mp);
	/* Because messages are priority messages we should already have a response. 
	   If we don't it is simply because we must back off so wait on a STREAMS
	   event. */
	if ((sccp->info.npi.CURRENT_state == -1U && qwait_sig(q) == 0) ||
	    (sccp->info.npi.CURRENT_state != NS_UNBND)) {
		mi_close_comm(&sccp_opens, q);
		return (ENXIO);
	}
	return (0);
}

/**
 * sccp_qclose: - STREAMS close procedure
 * @q: closing queue pair read queue
 * @oflags: open flags
 * @credp: credentials of closer
 */
static streamscall int
sccp_qclose(queue_t *q, int oflags, cred_t *credp)
{
	struct sccp *sccp = (struct sccp *) q->q_ptr;

	qprocsoff(q);
	flushq(RD(q), FLUSHALL);
	flushq(WR(q), FLUSHALL);
	mi_close_comm(&sccp_opens, q);
	return (0);
}

/*
 * STREAMS DEFINITIONS
 */

static struct module_info sccp_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = 1,
	.mi_maxpsz = INFPSZ,
	.mi_hiwat = 1,
	.mi_lowat = 0,
};

static struct module_stat sccp_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sccp_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit sccp_rdinit = {
	.qi_putp = sccp_rput,
	.qi_srvp = sccp_rsrv,
	.qi_qopen = sccp_qopen,
	.qi_qclose = sccp_qclose,
	.qi_minfo = &sccp_minfo,
	.qi_mstat = &sccp_rstat,
};

static struct qinit sccp_wrinit = {
	.qi_putp = sccp_wput,
	.qi_srvp = sccp_wsrv,
	.qi_minfo = &sccp_minfo,
	.qi_mstat = &sccp_wstat,
};

static struct streamtab sccp_tpiinfo = {
	.st_rdinit = &sccp_rdinit,
	.st_wrinit = &sccp_wrinit,
};

static struct fmodsw sccp_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sccp_tpinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

unsigned short modid = MOD_ID;

#ifdef LINUX
#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_parm(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for the SCCP-TPI moduels.  (0 for allocation.)");
#endif				/* LINUX */

MODULE_STATIC int __init
sccp_tpiinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if (unlikely((err = register_strmod(&sccp_fmod)) < 0)) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME,
			err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
sccp_tpiterminate(void)
{
	int err;

	if (unlikely((err = unregister_strmod(&sccp_fmod)) < 0))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	return;
}

#ifdef LINUX
module_init(sccp_tpiinit);
module_exit(sccp_tpiterminate);
#endif				/* LINUX */
