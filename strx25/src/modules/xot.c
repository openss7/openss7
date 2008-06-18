/*****************************************************************************

 @(#) $RCSfile: xot.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-06-18 16:45:27 $

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

 Last Modified $Date: 2008-06-18 16:45:27 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xot.c,v $
 Revision 0.9.2.2  2008-06-18 16:45:27  brian
 - widespread updates

 Revision 0.9.2.1  2008-05-03 21:22:41  brian
 - updates for release

 *****************************************************************************/

#ident "@(#) $RCSfile: xot.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-06-18 16:45:27 $"

static char const ident[] = "$RCSfile: xot.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-06-18 16:45:27 $";


/*
 * This is a XOT [RFC 1613] module.  It pushes over a TCP TPI stream and
 * provides a DLPI interface for use by the X25-PLP module.  Some nuances are
 * required for XOT and pure DLPI behaviour is not sufficient: therefore, when
 * this Stream is subsequently linked under the X25-PLP multiplexing driver,
 * it must be identified as a XOT data link and some additional XOT
 * characteristics (e.g. whether it represents a PVC) must be configured
 * against the link.
 *
 * The upper layer service interface is the Data Link Provider Interface
 * (DLPI) as interpreted for XOT, and the lower layer service interface is the
 * Transport Provider Interface (TPI) as intepreted for TCP.
 */


#include <sys/os7/compat.h>
#include <sys/dlpi.h>
#include <sys/tihdr.h>
#include <sys/tpi_ip.h>
#include <sys/tpi_tcp.h>

#define XOT_DESCRIPT	"X.25 OVER TCP MODULE FOR LINUX FAST-STREAMS"
#define XOT_EXTRA	"Part fo the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define XOT_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define XOT_REVISION	"OpenSS7 $RCSfile: xot.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-06-18 16:45:27 $"
#define XOT_DEVICE	"SVR 4.2MP X.25 over TCP Module (XOT) for X.25 CONS"
#define XOT_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define XOT_LICENSE	"GPL"
#define XOT_BANNER	XOT_DESCRIP	"\n" \
			XOT_EXTRA	"\n" \
			XOT_COPYRIGHT	"\n" \
			XOT_REVISION	"\n" \
			XOT_DEVICE	"\n" \
			XOT_CONTACT	"\n"
#define XOT_SPLASH	XOT_DESCRIP	" - " \
			XOT_REVISION

#ifndef CONFIG_STREAMS_XOT_NAME
#error "CONFIG_STREAMS_XOT_NAME must be defined."
#endif				/* CONFIG_STREAMS_XOT_NAME */
#ifndef CONFIG_STREAMS_XOT_MODID
#error "CONFIG_STREAMS_XOT_MODID must be defined."
#endif				/* CONFIG_STREAMS_XOT_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(XOT_CONTACT);
MODULE_DESCRIPTION(XOT_DESCRIP);
MODULE_SUPPORTED_DEVICE(XOT_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(XOT_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-xot");
MODULE_ALIAS("streams-module-xot");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_XOT_MODID));
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#endif				/* LINUX */

#ifndef XOT_MOD_NAME
#define XOT_MOD_NAME		CONFIG_STREAMS_XOT_NAME
#endif				/* XOT_MOD_NAME */
#ifndef XOT_MOD_MOD_ID
#define XOT_MOD_MOD_ID		CONFIG_STREAMS_XOT_MODID
#endif				/* XOT_MOD_MOD_ID */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define MOD_ID		XOT_MOD_ID
#define MOD_NAME	XOT_MOD_NAME
#ifdef MODULE
#define MODULE		XOT_BANNER
#else				/* MODULE */
#define MODULE		XOT_SPLASH
#endif				/* MODULE */

static struct module_info xot_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat xot_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat xot_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 * --------------------------------------------------------------------------
 *
 * PRIVATE STRUCTURE
 *
 * --------------------------------------------------------------------------
 */

struct dl;
struct tp;
struct priv;

/* Upper interface structure. */
struct dl {
	struct priv *priv;
	struct tp *tp;
	queue_t *oq;
	dl_ulong state;
	dl_ulong oldstate;
	dl_ulong coninds;
	dl_ulong oldconinds;
	struct dl *al;			/* accepting link */
	ushort add_len;
	struct {
		dl_info_ack_t info;
		unsigned char add[20];
	} proto;
	ushort loc_len;
	unsigned char loc[20];
	ushort rem_len;
	unsigned char rem[20];
};

/* Lower interface structure. */
struct tp {
	struct priv *priv;
	struct dl *dl;
	queue_t *oq;
	t_uscalar_t state;
	t_uscalar_t oldstate;
	t_uscalar_t coninds;
	t_uscalar_t oldconinds;
	struct tp *ap;			/* accepting provider */
	struct {
		struct T_info_ack info;
		struct T_addr_ack addr;
	} proto;
};

struct priv {
	atomic_int_t refs;
	struct dl w_priv;
	struct tp r_priv;
};

#define PRIV(q) ((struct priv *)q->q_ptr)
#define DL_PRIV(q) (&PRIV(q)->w_priv)
#define TP_PRIV(q) (&PRIV(q)->r_priv)

static void
priv_get(struct priv *priv)
{
	ATOMIC_INT_INCR(&priv->refs);
}
static void
priv_put(struct priv *priv)
{
	if (ATOMIC_INT_DECR(&priv->refs))
		mi_close_free((caddr_t) priv);
}
static struct dl *
dl_get(struct dl *dl)
{
	if (dl)
		priv_get(dl->priv);
	return (dl);
}
static void
dl_put(struct dl *dl)
{
	if (dl)
		priv_put(dl->priv);
}

static struct tp *
tp_get(struct tp *tp)
{
	if (tp)
		priv_get(tp->priv);
	return (tp);
}
static void
tp_put(struct tp *tp)
{
	if (tp)
		priv_put(tp->priv);
}

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

static inline const char *
tp_iocname(int cmd)
{
	switch (cmd) {
	default:
		return ("(unknown)");
	}
}
static inline const char *
tp_primname(t_uscalar_t prim)
{
	switch (cmd) {
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
static inline const char *
tp_statename(t_uscalar_t state)
{
	switch (cmd) {
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
static inline const char *
tp_errname(t_scalar_t error)
{
	if (error < 0)
		return ("TSYSERR");
	switch (cmd) {
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
	default:
		return ("(unknown)");
	}
}
static inline const char *
tp_strerror(t_scalar_t error)
{
	if (error < 0)
		("System error.");
	switch (cmd) {
	case TBADADDR:
		("Bad address format.");
	case TBADOPT:
		("Bad options format.");
	case TACCES:
		("Bad permissions.");
	case TBADF:
		("Bad file descriptor.");
	case TNOADDR:
		("Unable to allocate an address.");
	case TOUTSTATE:
		("Would place interface out of state.");
	case TBADSEQ:
		("Bad call sequence number.");
	case TSYSERR:
		("System error.");
	case TLOOK:
		("Has to t_look() for event (not TPI).");
	case TBADDATA:
		("Bad amount of data.");
	case TBUFOVFLW:
		("Buffer was too small (not TPI).");
	case TFLOW:
		("Would block due to flow control (not TPI).");
	case TNODATA:
		("No data indication (not TPI).");
	case TNODIS:
		("No disconnect indication (not TPI).");
	case TNOUDERR:
		("No unitdata error indication (not TPI).");
	case TBADFLAG:
		("Bad flags.");
	case TNOREL:
		("No orderly release indication (not TPI).");
	case TNOTSUPPORT:
		("Not supported.");
	case TSTATECHNG:
		("State is currently changing (not TPI).");
	case TNOSTRUCTYPE:
		("Structure type not supported (not TPI).");
	case TBADNAME:
		("Bad transport provider name (not TPI).");
	case TBADQLEN:
		("Listener queue length limit is zero (not TPI).");
	case TADDRBUSY:
		("Address already in use (not TPI).");
	case TINDOUT:
		("Outstanding connect indications (not TPI).");
	case TPROVMISMATCH:
		("Not same transport provider (not TPI).");
	case TRESQLEN:
		("Connection acceptor has qlen > 0 (not TPI).");
	case TRESADDR:
		("Conn. acceptor bound to different address (not TPI).");
	case TQFULL:
		("Connection indicator queue is full (not TPI).");
	case TPROTO:
		("Protocol error (not TPI).");
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
	dl->oldconinds = dl->coninds;
	return ((dl->oldstate = dl_get_state(dl)));
}

static dl_ulong
dl_restore_state(struct dl *dl)
{
	dl->coninds = dl->oldconinds;
	return (dl_set_state(dl, dl->oldstate));
}

static t_uscalar_t
tp_get_state(struct tp *tp)
{
	return (tp->state);
}

static t_uscalar_t
tp_set_state(struct tp *tp, t_uscalar_t newstate)
{
	t_uscalar_t oldstate = tp->state;

	if (newstate != oldstate) {
		tp->state = newstate;
		mi_strlog(tp->oq, STRLOGST, SL_TRACE, "%s <- %s", tp_statename(newstate),
			  tp_statename(oldstate));
	}
	return (newstate);
}

static t_uscalar_t
tp_save_state(struct tp *tp)
{
	tp->oldconinds = tp->coninds;
	return ((tp->oldstate = tp_get_state(tp)));
}

static t_uscalar_t
tp_restore_state(struct tp *tp)
{
	tp->coninds = tp->oldconinds;
	return (tp_set_state(tp, tp->oldstate));
}

/*
 * --------------------------------------------------------------------------
 *
 * DLPI PROVIDER TO DLPI USER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 * These are primtives issued to the X.25 DLPI user above this module.
 */

static fastcall noinline __unlikely int
m_error(struct dl *dl, queue_t *q, mblk_t *msg, int rerr, int werr)
{
}

/**
 * m_error_reply: reply with M_ERROR message when necessary
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @error: negative UNIX error, positive DLPI error
 *
 * This function is like m_error() except that it passes through error codes
 * that the caller should simply return without generating an M_ERROR, and
 * also, it translates positive DLPI errors into EPROTO.
 */
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

static fastcall noinline __unlikely int
m_hangup(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_info_ack: - issue DL_INFO_ACK to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_info_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_bind_ack: - issue DL_BIND_ACK to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_bind_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong sap, struct sockaddr_in *add, dl_ulong
	    conind, dl_ulong flags)
{
	dl_bind_ack_t *p;
	mblk_t *mp;
	size_t add_len = add ? sizeof(add->sin_addr) + sizeof(add->sin_port) : 0;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_BIND_ACK;
		p->dl_sap = sap;
		p->dl_addr_length = add_len;
		p->dl_addr_offset = sizeof(*p);
		p->dl_max_conind = conind;
		p->dl_xidtest_flg = flags;
		mp->b_wptr += sizeof(*p);
		if (add) {
			bcopy(&add->sin_addr, mp->b_wptr, sizeof(add->sin_addr));
			mp->b_wptr += sizeof(add->sin_addr);
			bcopy(&add->sin_port, mp->b_wptr, sizeof(add->sin_port));
			mp->b_wptr += sizeof(add->sin_port);
		}
		freemsg(msg);
		dl_set_state(dl, DL_IDLE);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_BIND_ACK");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_ok_ack: - issue DL_OK_ACK to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_ok_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim)
{
	dl_ok_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_OK_ACK;
		p->dl_correct_primitive = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		switch (dl_get_state(dl)) {
		case DL_ATTACH_PENDING:
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_DETACH_PENDING:
			dl_set_state(dl, DL_UNATTACHED);
			break;
		case DL_UNBIND_PENDING:
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_UDQOS_PENDING:
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_CONN_RES_PENDING:
			dl_set_state(dl, DL_INCON_PENDING);
			break;
		case DL_RESET_RES_PENDING:
			dl->coninds--;
			dl_set_state(dl, (dl->coninds > 0) ? DL_INCON_PENDING : DL_IDLE);
			dl_set_state(dl->al, DL_DATAXFER);
			dl_put(dl->al);
			break;
		case DL_DISCON8_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON9_PENDING:
			dl->coninds--;
			dl_set_state(dl, (dl->coninds > 0) ? DL_INCON_PENDING : DL_IDLE);
			break;
		case DL_DISCON11_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON12_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON13_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_SUBS_BIND_PND:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_SUBS_UNBIND_PND:
			dl_set_state(dl, DL_IDLE);
			break;
		default:
			break;
		}
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_OK_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_error_ack: - issue DL_ERROR_ACK to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_error_ack(struct dl *dl, queue_t *q, mblk_t *msg)
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
		switch (dl_get_state(dl)) {
		case DL_ATTACH_PENDING:
			dl_set_state(dl, DL_UNATTACHED);
			break;
		case DL_DETACH_PENDING:
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_BIND_PENDING:
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_UNBIND_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_UDQOS_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_OUTCON_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_CONN_RES_PENDING:
			dl_set_state(dl, DL_INCON_PENDING);
			break;
		case DL_USER_RESET_PENDING:
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_RESET_RES_PENDING:
			dl_set_state(dl, DL_PROV_RESET_PENDING);
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
		case DL_SUBS_BIND_PND:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_SUBS_UNBIND_PND:
			dl_set_state(dl, DL_IDLE);
			break;
		}
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_ERROR_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_error_reply: - reply when necessary to error condition
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @error: negative UNIX error, positive DLPI error
 *
 * This is like dl_error_ack(), except that error codes that should simply be
 * returned by the called are passed through without generating a DL_ERROR_ACK
 * to the DLS-user.
 */
static fastcall noinline __unlikely int
dl_error_reply(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, dl_long error)
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
	return dl_error_ack(dl, q, msg, prim, error);
}

/**
 * dl_subs_bind_ack: - issue DL_SUBS_BIND_ACK to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_subs_bind_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_data_ind: - issue DL_DATA_IND to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_data_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong more, mblk_t *dp)
{
	if (likely(canputnext(dl->oq))) {
		dp->b_band = 0;
		if (more)
			dp->b_flag &= ~MSGDELIM;
		else
			dp->b_flag |= MSGDELIM;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGDA, SL_TRACE, "<- M_DATA");
		putnext(dl->oq, dp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * dl_unitdata_ind: - issue DL_UNITDATA_IND to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_unitdata_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_uderror_ind: - issue DL_UDERROR_IND to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_uderror_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_connect_ind: - issue DL_CONNECT_IND to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_connect_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong correlation,
	       struct sockaddr_in *cld, struct sockaddr_in *clg, dl_qos_co_range1_t * qos, mblk_t
	       *dp)
{
	dl_connect_ind_t *p;
	mblk_t *mp;
	size_t qos_len = qos ? sizeof(*qos) : 0;
	size_t cld_len = cld ? sizeof(cld->sin_addr) + sizeof(cld->sin_port) : 0;
	size_t clg_len = clg ? sizeof(clg->sin_addr) + sizeof(clg->sin_port) : 0;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + cld_len + clg_len + qos_len, BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
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
			if (cld) {
				bcopy(&cld->sin_addr, mp->b_wptr, sizeof(cld->sin_addr));
				mp->b_wptr += sizeof(cld->sin_addr);
				bcopy(&cld->sin_port, mp->b_wptr, sizeof(cld->sin_port));
				mp->b_wptr += sizeof(cld->sin_port);
			}
			if (clg) {
				bcopy(&clg->sin_addr, mp->b_wptr, sizeof(clg->sin_addr));
				mp->b_wptr += sizeof(clg->sin_addr);
				bcopy(&clg->sin_port, mp->b_wptr, sizeof(clg->sin_port));
				mp->b_wptr += sizeof(clg->sin_port);
			}
			if (qos) {
				bcopy(qos, mp->b_wptr, qos_len);
				mp->b_wptr += qos_len;
			}
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl->coninds++;
			dl_set_state(dl, DL_INCON_PENDING);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_CONNECT_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_connect_con: - issue DL_CONNECT_CON to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_connect_con(struct dl *dl, queue_t *q, mblk_t *msg, struct sockaddr_in *res, dl_qos_co_sel1_t
	       * qos, mblk_t *dp)
{
	dl_connect_con_t *p;
	mblk_t *mp;
	size_t qos_len = qos ? sizeof(*qos) : 0;
	size_t res_len = res ? sizeof(res->sin_addr) + sizeof(res->sin_port) : 0;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + res_len + qos_len, BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_CONNECT_CON;
			p->dl_resp_addr_length = res_len;
			p->dl_resp_addr_offset = sizeof(*p);
			p->dl_qos_length = qos_len;
			p->dl_qos_offset = sizeof(*p) + res_len;
			p->dl_growth = 0;
			mp->b_wptr += sizeof(*p);
			if (res) {
				bcopy(&res->sin_addr, mp->b_wptr, sizeof(res->sin_addr));
				mp->b_wptr += sizeof(res->sin_addr);
				bcopy(&res->sin_port, mp->b_wptr, sizeof(res->sin_port));
				mp->b_wptr += sizeof(res->sin_port);
			}
			if (qos) {
				bcopy(qos, mp->b_wptr, qos_len);
				mp->b_wptr += qos_len;
			}
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, DL_DATAXFER);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_CONNECT_CON");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_token_ack: - issue DL_TOKEN_ACK to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_token_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	struct dl_token_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_TOKEN_ACK;
		p->dl_token = dl->token;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_TOKEN_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * dl_disconnect_ind: - issue DL_DISCONNECT_IND to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_disconnect_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong originator,
		  dl_ulong reason, dl_ulong correlation, mblk_t *dp)
{
	dl_disconnect_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_DISCONNECT_IND;
			p->dl_originator = originator;
			p->dl_reason = reason;
			p->dl_correlation = correlation;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			if (correlation)
				dl->coninds--;
			dl_set_state(dl, (dl->coninds > 0) ? DL_INCON_PENDING : DL_IDLE);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_DISCONNECT_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_reset_ind: - issue DL_RESET_IND to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_reset_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_reset_con: - issue DL_RESET_CON to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_reset_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_data_ack_ind: - issue DL_DATA_ACK_IND to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_data_ack_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_data_ack_status_ind: - issue DL_DATA_ACK_STATUS_IND to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_data_ack_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_reply_ind: - issue DL_REPLY_IND to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_reply_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_reply_status_ind: - issue DL_REPLY_STATUS_IND to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_reply_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_reply_update_status_ind: - issue DL_UPDATE_STATUS_IND to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_reply_update_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_xid_ind: - issue DL_XID_IND to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_xid_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_xid_con: - issue DL_XID_CON to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_xid_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_test_ind: - issue DL_TEST_IND to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_test_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_test_con: - issue DL_TEST_CON to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_test_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_phys_addr_ack: - issue DL_PHYS_ADDR_ACK to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_phys_addr_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/**
 * dl_get_statistics_ack: - issue DL_GET_STATISTICS_ACK to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_get_statistics_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
}

/*
 * --------------------------------------------------------------------------
 *
 * TPI USER TO TPI PROVIDER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 * These are primitives issued to the TCP TPI provider beneath this module.
 */

/**
 * tp_conn_req: - issue T_CONN_REQ to the TPI provider
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_conn_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
}

/**
 * tp_conn_res: - issue T_CONN_RES to the TPI provider
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @ap: accepting stream
 * @seq: connection indication sequence number
 * @dp: user data (if any)
 * 
 * Note: eventually we should use the selected Data Link QOS parameters to
 * determine which options to feed to TCP on the connection response.
 */
static fastcall noinline __unlikely int
tp_conn_res(struct tp *tp, queue_t *q, mblk_t *msg, struct tp *ap, t_uscalar_t seq, mblk_t *dp)
{
	struct T_conn_res *p;
	mblk_t *mp;
	size_t opt_len = 0;		/* for now */

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED)))) {
		if (likely(canputnext(tp->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_RES;
			p->ACCEPTOR_id = ap->token;
			p->OPT_length = opt_len;
			p->OPT_offset = sizeof(*p);
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			/* FIXME: build options */
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			tp_set_state(tp, TS_WACK_CRES);
			mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "T_CONN_RES ->");
			putnext(tp->oq, mp);
			return (0);
		}
		freeb(mp);
		tp_put(ap);
		return (-EBUSY);
	}
	tp_put(ap);
	return (-ENOBUFS);
}

/**
 * tp_discon_req: - issue T_DISCON_REQ to the TPI provider
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_discon_req(struct tp *tp, queue_t *q, mblk_t *msg, t_uscalar_t seq, mblk_t *dp)
{
	struct T_discon_req *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(tp->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_DISCON_REQ;
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			tp_set_state(tp, TS_IDLE);
			mi_strlog(tp->oq, STRLOGTX, SL_TRACE, "T_DISCON_REQ ->");
			putnext(tp->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);

}

/**
 * tp_data_req: - issue T_DATA_REQ to the TPI provider
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_data_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
}

/**
 * tp_exdata_req: - issue T_EXDATA_REQ to the TPI provider
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_exdata_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
}

/**
 * tp_info_req: - issue T_INFO_REQ to the TPI provider
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_info_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
}

/**
 * tp_bind_req: - issue T_BIND_REQ to the TPI provider
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_bind_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
}

/**
 * tp_unbind_req: - issue T_UNBIND_REQ to the TPI provider
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_unbind_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
}

/**
 * tp_unitdata_req: - issue T_UNITDATA_REQ to the TPI provider
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_unitdata_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
}

/**
 * tp_optmgmt_req: - issue T_OPTMGMT_REQ to the TPI provider
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_optmgmt_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
}

/**
 * tp_ordrel_req: - issue T_ORDREL_REQ to the TPI provider
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_ordrel_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
}

/**
 * tp_optdata_req: - issue T_OPTDATA_REQ to the TPI provider
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_optdata_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
}

/**
 * tp_addr_req: - issue T_ADDR_REQ to the TPI provider
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_addr_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
}

/**
 * tp_capability_req: - issue T_CAPABILITY_REQ to the TPI provider
 * @tp: transport provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
tp_capability_req(struct tp *tp, queue_t *q, mblk_t *msg)
{
}

/*
 * --------------------------------------------------------------------------
 *
 * DLPI USER TO DLPI PROVIDER RECEIVED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 * These are primitives received from the X.25 DLPI user above this module.
 */

/**
 * dl_info_req: - process received DL_INFO_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_INFO_REQ message
 */
static fastcall noinline __unlikely int
dl_info_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_attach_req: - process received DL_ATTACH_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_ATTACH_REQ message
 */
static fastcall noinline __unlikely int
dl_attach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_detach_req: - process received DL_DETACH_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_DETACH_REQ message
 */
static fastcall noinline __unlikely int
dl_detach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_bind_req: - process received DL_BIND_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_BIND_REQ message
 */
static fastcall noinline __unlikely int
dl_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_unbind_req: - process received DL_UNBIND_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_UNBIND_REQ message
 */
static fastcall noinline __unlikely int
dl_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_subs_bind_req: - process received DL_SUBS_BIND_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_SUBS_BIND_REQ message
 */
static fastcall noinline __unlikely int
dl_subs_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_subs_unbind_req: - process received DL_SUBS_UNBIND_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_SUBS_UNBIND_REQ message
 */
static fastcall noinline __unlikely int
dl_subs_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_enabmulti_req: - process received DL_ENABMULTI_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_ENABMULTI_REQ message
 */
static fastcall noinline __unlikely int
dl_enabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_disabmulti_req: - process received DL_DISABMULTI_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_DISABMULTI_REQ message
 */
static fastcall noinline __unlikely int
dl_disabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_promiscon_req: - process received DL_PROMISCON_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_PROMISCON_REQ message
 */
static fastcall noinline __unlikely int
dl_promiscon_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_promiscoff_req: - process received DL_PROMISCOFF_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_PROMISCOFF_REQ message
 */
static fastcall noinline __unlikely int
dl_promiscoff_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_data_req: - process received DL_DATA_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_DATA_REQ message
 */
static fastcall noinline __unlikely int
dl_data_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_unitdata_req: - process received DL_UNITDATA_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_UNITDATA_REQ message
 */
static fastcall noinline __unlikely int
dl_unitdata_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_udqos_req: - process received DL_UDQOS_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_UDQOS_REQ message
 */
static fastcall noinline __unlikely int
dl_udqos_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_connect_req: - process received DL_CONNECT_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_CONNECT_REQ message
 */
static fastcall noinline __unlikely int
dl_connect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

static struct dl *
dl_lookup(struct dl *dl, dl_ulong token)
{
	struct dl *dl = NULL;
	caddr_t i;
	pl_t pl;

	if (token == 0)
		return (dl_get(dl));
	pl = RW_RDLOCK(&xot_rwlock, plstr);
	for (i = xot_opens; i; i = mi_next_ptr(i))
		if (((struct priv *) i)->w_priv.token == token)
			break;
	if (i) {
		priv_get((struct priv *) i);
		dl = &((struct priv *) i)->w_priv;
	}
	RW_UNLOCK(&xot_rwlock, pl);
	return (dl);
}

/**
 * dl_connect_res: - process received DL_CONNECT_RES primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_CONNECT_RES message
 */
static fastcall noinline __unlikely int
dl_connect_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_res_t *p = (typeof(p)) mp->b_rptr;
	struct dl *al;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqosparam;
	if (dl_get_state(dl) != DL_INCON_PENDING)
		goto outstate;
	if (p->dl_qos_length >= sizeof(dl->qos.dl_qos_type)) {
		bcopy(mp->b_rptr + p->dl_qos_offset, &dl->qos, sizeof(dl_ulong));
		if (dl->qos.dl_qos_type != DL_QOS_CO_SEL1)
			goto badqostype;
		if (p->dl_qos_length < sizeof(dl->qos))
			goto badqosparam;
		bcopy(mp->b_rptr + p->dl_qos_offset, &dl->qos, sizeof(dl->qos));
	} else {
		dl->qos.dl_qos_type = DL_QOS_CO_SEL1;
		dl->qos.dl_rcv_throughput = DL_QOS_DONT_CARE;
		dl->qos.dl_rcv_trans_delay = DL_QOS_DONT_CARE;
		dl->qos.dl_xmt_throughput = DL_QOS_DONT_CARE;
		dl->qos.dl_xmt_trans_delay = DL_QOS_DONT_CARE;
		dl->qos.dl_priority = DL_QOS_DONT_CARE;
		dl->qos.dl_protection = DL_QOS_DONT_CARE;
		dl->qos.dl_residual_error = DL_QOS_DONT_CARE;
		dl->qos.dl_resilience.dl_disc_prob = DL_QOS_DONT_CARE;
		dl->qos.dl_resilience.dl_reset_prob = DL_QOS_DONT_CARE;
	}
	if (dl->qos.dl_rcv_throughput == DL_QOS_DONT_CARE) {
		dl->qos.dl_rcv_throughput = dl->qor.dl_rcv_throughput.dl_accept_value;
	}
	if (dl->qos.dl_rcv_throughput == DL_UNKNOWN) {
		dl->qos.dl_rcv_throughput = dl->qor.dl_rcv_throughput.dl_target_value;
	}
	if (dl->qos.dl_rcv_trans_delay == DL_QOS_DONT_CARE) {
		dl->qos.dl_rcv_trans_delay = dl->qor.dl_rcv_trans_delay.dl_accept_value;
	}
	if (dl->qos.dl_rcv_trans_delay == DL_UNKNOWN) {
		dl->qos.dl_rcv_trans_delay = dl->qor.dl_rcv_trans_delay.dl_target_value;
	}
	if (dl->qos.dl_xmt_throughput == DL_QOS_DONT_CARE) {
		dl->qos.dl_xmt_throughput = dl->qor.dl_xmt_throughput.dl_accept_value;
	}
	if (dl->qos.dl_xmt_throughput == DL_UNKNOWN) {
		dl->qos.dl_xmt_throughput = dl->qor.dl_xmt_throughput.dl_target_value;
	}
	if (dl->qos.dl_xmt_trans_delay == DL_QOS_DONT_CARE) {
		dl->qos.dl_xmt_trans_delay = dl->qor.dl_xmt_trans_delay.dl_accept_value;
	}
	if (dl->qos.dl_xmt_trans_delay == DL_UNKNOWN) {
		dl->qos.dl_xmt_trans_delay = dl->qor.dl_xmt_trans_delay.dl_target_value;
	}
	if (dl->qos.dl_priority == DL_QOS_DONT_CARE) {
		dl->qos.dl_priority = dl->qor.dl_priority.dl_min;
	}
	if (dl->qos.dl_priority == DL_UNKNOWN) {
		dl->qos.dl_priority = dl->qor.dl_priority.dl_max;
	}
	if (dl->qos.dl_protection == DL_QOS_DONT_CARE) {
		dl->qos.dl_protection = dl->qor.dl_protection.dl_min;
	}
	if (dl->qos.dl_protection == DL_UNKNOWN) {
		dl->qos.dl_protection = dl->qor.dl_protection.dl_max;
	}
	if (dl->qos.dl_residual_error == DL_QOS_DONT_CARE) {
		dl->qos.dl_residual_error = dl->qor.dl_residual_error;
	}
	if (dl->qos.dl_residual_error == DL_UNKNOWN) {
		dl->qos.dl_residual_error = dl->qor.dl_residual_error;
	}
	if (dl->qos.dl_resilience.dl_disc_prob == DL_QOS_DONT_CARE) {
		dl->qos.dl_resilience.dl_disc_prob = dl->qor.dl_resilience.dl_disc_prob;
	}
	if (dl->qos.dl_resilience.dl_disc_prob == DL_UNKNOWN) {
		dl->qos.dl_resilience.dl_disc_prob = dl->qor.dl_resilience.dl_disc_prob;
	}
	if (dl->qos.dl_resilience.dl_reset_prob == DL_QOS_DONT_CARE) {
		dl->qos.dl_resilience.dl_reset_prob = dl->qor.dl_resilience.dl_reset_prob;
	}
	if (dl->qos.dl_resilience.dl_reset_prob == DL_UNKNOWN) {
		dl->qos.dl_resilience.dl_reset_prob = dl->qor.dl_resilience.dl_reset_prob;
	}
	/* Otherwise we trust the correlation value because we use TPI's
	   connection identifier for this purpose.  If the value is bad, TPI
	   will tell us later. */
	if (p->dl_correlation == 0)
		goto badcor;
	/* We use our own token values because, although we could just use
	   TPI's, that means that a DLPI user could erroneously accept on a TPI 
	   stream for which no XOT module has been pushed. */
	if ((al = dl_lookup(dl, p->dl_token)) == NULL)
		goto badtok;
	dl_set_state(dl, DL_CONN_RES_PENDING);
	return tp_conn_res(dl->tp, q, mp, al->tp, p->dl_correlation, mp->b_cont);
      badtok:
	err = DL_BADTOKEN;
	goto error;
      badcor:
	err = DL_BADCORR;
	goto error;
      badqostype:
	err = DL_BADQOSTYPE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badqosparam:
	err = DL_BADQOSPARAM;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	dl_error_reply(dl, q, mp, DL_CONNECT_RES, err);
}

/**
 * dl_token_req: - process received DL_TOKEN_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_TOKEN_REQ message
 */
static fastcall noinline __unlikely int
dl_token_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_token_ack(dl, q, mp);
}

/**
 * dl_disconnect_req: - process received DL_DISCONNECT_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_DISCONNECT_REQ message
 */
static fastcall noinline __unlikely int
dl_disconnect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_reset_req: - process received DL_RESET_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_RESET_REQ message
 * 
 * We don't really have reset capability with TCP, so just disconnect.  This
 * entails sending a T_DISCON_REQ and issuing a DL_DISCONNECT_IND up to the
 * DLS-user.
 */
static fastcall noinline __unlikely int
dl_reset_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	struct dl_reset_req_t *p = (typeof(p)) mp->b_rptr;
	dl_ulong reason = DL_DISC_TRANSIENT_CONDITION;
	dl_ulong orig = DL_USER;
	int err;

	if (!(dl->info.dl_service_mode & DL_CODLS))
		goto notsupport;
	if (dl_get_state(dl) != DL_DATAXFER)
		goto outstate;
	dl_set_state(dl, DL_USER_RESET_PENDING);
	/* Note that if we come through here twice due to, say, failure to
	   allocate a buffer in dl_disconnect_ind(), sending the T_DISCON_REQ
	   again will have no negative effects as TPI requires that a
	   T_DISCON_REQ received in the TS_IDLE state simply be discarded. */
	if ((err = tp_discon_req(dl->tp, q, NULL, 0, NULL)))
		goto error;
	return dl_disconnect_ind(dl, q, mp, orig, reason, 0, NULL);
      notsupport:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_RESET_REQ, err);
}

/**
 * dl_reset_res: - process received DL_RESET_RES primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_RESET_RES message
 *
 * We do have a provider reset capability (that is SCTP-RESTART condition).
 * Now I wish I have used the NPI-SCTP interface.  Perhaps it is not too
 * late...
 */
static fastcall noinline __unlikely int
dl_reset_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_data_ack_req: - process received DL_DATA_ACK_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_DATA_ACK_REQ message
 */
static fastcall noinline __unlikely int
dl_data_ack_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_reply_req: - process received DL_REPLY_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_REPLY_REQ message
 */
static fastcall noinline __unlikely int
dl_reply_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_reply_update_req: - process received DL_REPLY_UDPATE_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_REPLY_UDPATE_REQ message
 */
static fastcall noinline __unlikely int
dl_reply_update_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_xid_req: - process received DL_XID_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_XID_REQ message
 */
static fastcall noinline __unlikely int
dl_xid_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_xid_res: - process received DL_XID_RES primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_XID_RES message
 */
static fastcall noinline __unlikely int
dl_xid_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_test_req: - process received DL_TEST_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_TEST_REQ message
 */
static fastcall noinline __unlikely int
dl_test_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_test_res: - process received DL_TEST_RES primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_TEST_RES message
 */
static fastcall noinline __unlikely int
dl_test_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_phys_addr_req: - process received DL_PHYS_ADDR_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_PHYS_ADDR_REQ message
 */
static fastcall noinline __unlikely int
dl_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_set_phys_addr_req: - process received DL_SET_PHYS_ADDR_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_SET_PHYS_ADDR_REQ message
 */
static fastcall noinline __unlikely int
dl_set_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/**
 * dl_get_statistics_req: - process received DL_GET_STATISTICS_REQ primitive
 * @dl: data link private structure
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_GET_STATISTICS_REQ message
 */
static fastcall noinline __unlikely int
dl_get_statistics_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
}

/*
 * --------------------------------------------------------------------------
 *
 * TPI PROVIDER TO TPI USER RECEIVED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 * These are primitives received from the TCP TPI driver beneath this module.
 */

/**
 * tp_conn_ind: - process received T_CONN_IND primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the T_CONN_IND primitive
 */
static fastcall noinline __unlikely int
tp_conn_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_ind *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_in add, *clg = NULL, *cld;
	int err;

#ifdef XOT_SAFETY
	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->SRC_offset, p->SRC_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
#endif
	if (p->SRC_length) {
		bcopy(mp->b_rptr + p->SRC_offset, &add, sizeof(add));
		clg = &add;
	}
	cld = &tp->loc;
	/* FIXME: process options */
	tp_set_state(tp, TS_WRES_CIND);
	return dl_connect_ind(tp->dl, q, mp, cld, clg, NULL, mp->b_cont);
}

/**
 * tp_conn_con: - process received T_CONN_RES primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the T_CONN_RES primitive
 */
static fastcall noinline __unlikely int
tp_conn_con(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_conn_con *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_in add, *res = NULL;

#ifdef XOT_SAFETY
	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->RES_offset, p->RES_length))
		goto badaddr;
	if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
		goto badopt;
#endif
	if (p->RES_length) {
		bcopy(mp->b_rptr + p->RES_offset, &add, sizeof(add));
		res = &add;
	}
	/* FIXME: process options */
	tp_set_state(tp, TS_DATA_XFER);
	return dl_connect_con(tp->dl, q, mp, res, NULL, mp->b_cont);
}

#define SCTP_CAUSE_INVALID_STR		 1
#define SCTP_CAUSE_MISSING_PARM		 2
#define SCTP_CAUSE_STALE_COOKIE		 3
#define SCTP_CAUSE_NO_RESOURCE		 4
#define SCTP_CAUSE_BAD_ADDRESS		 5
#define SCTP_CAUSE_BAD_CHUNK_TYPE	 6
#define SCTP_CAUSE_INVALID_PARM		 7
#define SCTP_CAUSE_BAD_PARM		 8
#define SCTP_CAUSE_NO_DATA		 9
#define SCTP_CAUSE_SHUTDOWN		10
#define SCTP_CAUSE_NEW_ADDR		11
#define SCTP_CAUSE_USER_INITIATED	12
#define SCTP_CAUSE_PROTO_VIOLATION	13
#define SCTP_CAUSE_LAST_ADDR		0x100
#define SCTP_CAUSE_RES_SHORTAGE		0x101
#define SCTP_CAUSE_SOURCE_ADDR		0x102
#define SCTP_CAUSE_ILLEGAL_ASCONF	0x103

/**
 * tp_discon_ind: - process received T_DISCON_IND primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the T_DISCON_IND primitive
 *
 * Note that all negative disconnect reasons are UNIX error codes whereas all positive reasons are
 * SCTP cause values.
 */
static fastcall noinline __unlikely int
tp_discon_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_discon_ind *p = (typeof(p)) mp->b_rptr;
	dl_ulong originator = 0;
	dl_ulong reason = DL_UNSPECIFIED;

#ifdef XOT_SAFETY
	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
#endif
	if (p->SEQ_number == (t_uscalar_t) (-1))
		p->SEQ_number = 0;
	switch (tp_get_state(tp)) {
	case TS_WRES_CIND:
	case TS_DATA_XFER:
	case TS_WREQ_ORDREL:
	case TS_WIND_ORDREL:
		/* disconnect */
		switch ((int) p->DISCON_reason) {
		case SCTP_CAUSE_INVALID_STR:
		case SCTP_CAUSE_STALE_COOKIE:
		case SCTP_CAUSE_NO_RESOURCE:
		case SCTP_CAUSE_NO_DATA:
		case SCTP_CAUSE_SHUTDOWN:
		case SCTP_CAUSE_NEW_ADDR:
		case SCTP_CAUSE_LAST_ADDR:
		case SCTP_CAUSE_RES_SHORTAGE:
		case SCTP_CAUSE_SOURCE_ADDR:
		case SCTP_CAUSE_ILLEGAL_ASCONF:
		case -EHOSTUNREACH:
		case -ETIMEDOUT:
		case -ECONNRESET:
		case -EPIPE:
		case -EPROTO:
		case -EMSGSIZE:
		case -EADDRNOTAVAIL:
		case -EADDRINUSE:
		case -EFAULT:
		case -ERESTART:
		case -EISCONN:
		case -ENOTCONN:
		case -ENETDOWN:
		case -ENOMEM:
		case -EBUSY:
		case -EAGAIN:
		case -EDEADLK:
		case -ENOBUFS:
			originator = DL_PROVIDER;
			reason = DL_DISC_TRANSIENT_CONDITION;
			break;
		case SCTP_CAUSE_MISSING_PARM:
		case SCTP_CAUSE_BAD_ADDRESS:
		case SCTP_CAUSE_BAD_CHUNK_TYPE:
		case SCTP_CAUSE_INVALID_PARM:
		case SCTP_CAUSE_BAD_PARM:
		case SCTP_CAUSE_PROTO_VIOLATION:
		case -EINVAL:
		case -EDESTADDRREQ:
		case -EAFNOSUPPORT:
		case -EACCES:
		case -EOPNOTSUPP:
			originator = DL_PROVIDER;
			reason = DL_DISC_PERMANENT_CONDITION;
			break;
		case SCTP_CAUSE_USER_INITIATED:
			originator = DL_USER;
			reason = DL_DISC_ABNORMAL_CONDITION;
			break;
		case -ECONNABORTED:
		case -ECONNREFUSED:
			originator = DL_PROVIDER;
			reason = DL_DISC_UNSPECIFIED;
			break;
		default:
			/* unknown */
			originator = DL_UNKNOWN;
			reason = DL_UNKNOWN;
			break;
		}
		break;
	case TS_WCON_CREQ:
		/* connection rejection */
		switch ((int) p->DISCON_reason) {
		case SCTP_CAUSE_INVALID_STR:
		case SCTP_CAUSE_STALE_COOKIE:
		case SCTP_CAUSE_NO_RESOURCE:
		case SCTP_CAUSE_NO_DATA:
		case SCTP_CAUSE_SHUTDOWN:
		case SCTP_CAUSE_USER_INITIATED:
		case SCTP_CAUSE_LAST_ADDR:
		case SCTP_CAUSE_RES_SHORTAGE:
		case SCTP_CAUSE_SOURCE_ADDR:
		case SCTP_CAUSE_ILLEGAL_ASCONF:
		case -ETIMEDOUT:
		case -ECONNRESET:
		case -EPIPE:
		case -EPROTO:
		case -EINVAL:
		case -EMSGSIZE:
		case -EFAULT:
		case -ERESTART:
		case -EACCES:
		case -EISCONN:
		case -ENOTCONN:
		case -ENOMEM:
		case -EBUSY:
		case -EAGAIN:
		case -EDEADLK:
		case -ENOBUFS:
		case -ECONNREFUSED:
			originator = DL_PROVIDER;
			reason = DL_CONREJ_TRANSIENT_COND;
			break;
		case SCTP_CAUSE_MISSING_PARM:
		case SCTP_CAUSE_BAD_CHUNK_TYPE:
		case SCTP_CAUSE_INVALID_PARM:
		case SCTP_CAUSE_BAD_PARM:
		case SCTP_CAUSE_PROTO_VIOLATION:
		case -EOPNOTSUPP:
			originator = DL_PROVIDER;
			reason = DL_CONREJ_PERMANENT_COND;
			break;
		case SCTP_CAUSE_BAD_ADDRESS:
			originator = DL_PROVIDER;
			reason = DL_CONREJ_DEST_UNREACH_PERMANENT;
			break;
		case SCTP_CAUSE_NEW_ADDR:
		case -EHOSTUNREACH:
		case -EADDRNOTAVAIL:
		case -EADDRINUSE:
		case -ENETDOWN:
			originator = DL_PROVIDER;
			reason = DL_CONREJ_DEST_UNREACH_TRANSIENT;
			break;
		case -ECONNABORTED:
			originator = DL_PROVIDER;
			reason = DL_DISC_UNSPECIFIED;
			break;
		case -EDESTADDRREQ:
		case -EAFNOSUPPORT:
			originator = DL_PROVIDER;
			reason = DL_CONREJ_DEST_UNKNOWN;
			break;
		default:
			/* unknown */
			originator = DL_UNKNOWN;
			reason = DL_UNKNOWN;
			break;
		}
		break;
	default:
		/* unknown */
		originator = DL_UNKNOWN;
		reason = DL_UNKNOWN;
		break;
	}
	/* FIXME: need to map TPI disconnect reason to DLPI disconnect reason */
	return dl_disconnect_ind(tp->dl, q, mp, originator, reason, p->SEQ_number, mp->b_cont);
}

/**
 * tp_data_ind: - process received T_DATA_IND primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the T_DATA_IND primitive
 *
 * Simple, just strip the M_PROTO and fire it up.
 */
static fastcall noinline __unlikely int
tp_data_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_data_ind *p = (typeof(p)) mp->b_rptr;

	return dl_data_ind(tp->dl, q, mp, (p->MORE_flag & T_MORE), mp->b_cont);
}

/**
 * tp_exdata_ind: - process received T_EXDATA_IND primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the T_EXDATA_IND primitive
 *
 * Simple, just strip the M_PROTO and fire it up.
 */
static fastcall noinline __unlikely int
tp_exdata_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_exdata_ind *p = (typeof(p)) mp->b_rptr;

	return dl_data_ind(tp->dl, q, mp, (p->MORE_flag & T_MORE), mp->b - cont);
}

/**
 * tp_info_ack: - process received T_INFO_ACK primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the T_INFO_ACK primitive
 */
static fastcall noinline __unlikely int
tp_info_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_info_ack *p = (typeof(p)) mp->b_rptr;

#ifdef XOT_SAFETY
	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
#endif
	tp->proto.caps.INFO_ack = *p;
	if (tp->proto.caps.INFO_ack.CURRENT_state != tp->state) {
		/* Whooops. */
		tp->state = tp->oldstate = tp->proto.caps.info.INFO_ack.CURRENT_state;
	}
}

/**
 * tp_bind_ack: - process received T_BIND_ACK primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the T_BIND_ACK primitive
 */
static fastcall noinline __unlikely int
tp_bind_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_bind_ack *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_in loc, *add = NULL;

#ifdef XOT_SAFETY
	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->ADDR_offset, p->ADDR_length))
		goto badaddr;
#endif
	if (p->ADDR_length) {
		add = &loc;
		bcopy(mp->b_rptr + p->ADDR_offset, add, sizeof(*add));
	}
	tp_set_state(tp, TS_IDLE);
	return dl_bind_ack(tp->dl, q, mp, add, p->CONIND_number, (DL_AUTO_XID | DL_AUTO_TEST));
}

/**
 * tp_error_ack: - process received T_ERROR_ACK primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the T_ERROR_ACK primitive
 */
static fastcall noinline __unlikely int
tp_error_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_error_ack *p = (typeof(p)) mp->b_rptr;
	dl_long error;
	dl_ulong prim;

#ifdef XOT_SAFETY
	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
#endif
	switch (p->ERROR_prim) {
	case T_CONN_REQ:
		prim = DL_CONNECT_REQ;
		break;
	case T_CONN_RES:
		prim = DL_CONNECT_RES;
		break;
	case T_DISCON_REQ:
		prim = DL_DISCONNECT_REQ;
		break;
	case T_BIND_REQ:
		/* FIXME */
		prim = DL_BIND_REQ;
		prim = DL_SUBS_BIND_REQ;
		break;
	case T_UNBIND_REQ:
		/* FIXME */
		prim = DL_UNBIND_REQ;
		prim = DL_SUBS_UNBIND_REQ;
		break;
	case T_UNITDATA_REQ:
		prim = DL_UNITDATA_REQ;
		break;
	case T_OPTMGMT_REQ:
		prim = DL_UDQOS_REQ;
		break;
	default:
		freemsg(mp);
		mi_strlog(tp->oq, 0, SL_ERROR, "%s: T_ERROR_ACK for primitive %s", __FUNCTION__,
			  tp_primname(p->ERROR_prim));
		return (0);
	}
	switch (p->TLI_error) {
	case TBADADDR:
		error = DL_BADADDR;
		break;
	case TBADOPT:
		error = DL_BADQOSTYPE;
		break;
	case TACCES:
		error = DL_ACCESS;
		break;
	case TBADF:
		error = DL_BADTOKEN;
		break;
	case TNOADDR:
		error = DL_NOADDR;
		break;
	case TOUTSTATE:
		error = DL_OUTSTATE;
		break;
	case TBADSEQ:
		error = DL_BADCORR;
		break;
	case TSYSERR:
		error = -p->UNIX_error;
		break;
	case TBADDATA:
		error = DL_BADDATA;
		break;
	case TBADFLAG:
		error = DL_BADQOSPARAM;
		break;
	case TNOTSUPPORT:
		error = DL_NOTSUPPORTED;
		break;
	case TADDRBUSY:
		/* FIXME */
		error = DL_BOUND;
		error = DL_BUSY;
		break;
	case TINDOUT:
		error = DL_PENDING;
		break;
	case TPROVMISMATCH:
		error = DL_BADTOKEN;
		break;
	case TRESQLEN:
		error = DL_PENDING;
		break;
	case TRESADDR:
		error = DL_BADTOKEN;
		break;
	default:
		error = -EFAULT;
		break;
	}
	switch (tp_get_state(tp)) {
	case TS_WACK_BREQ:
		tp_set_state(tp, TS_UNBND);
		break;
	case TS_WACK_UREQ:
		tp_set_state(tp, TS_IDLE);
		break;
	case TS_WACK_OPTREQ:
		tp_set_state(tp, TS_IDLE);
		break;
	case TS_WACK_CREQ:
		tp_set_state(tp, (tp->coninds > 0) ? TS_WRES_CIND : TS_IDLE);
		break;
	case TS_WACK_CRES:
		tp_set_state(tp, (tp->coninds > 0) ? TS_WRES_CIND : TS_IDLE);
		break;
	case TS_WACK_DREQ6:
		tp_set_state(tp, TS_WCON_CREQ);
		break;
	case TS_WACK_DREQ7:
		tp_set_state(tp, TS_WRES_CIND);
		break;
	case TS_WACK_DREQ9:
		tp_set_state(tp, TS_DATA_XFER);
		break;
	case TS_WACK_DREQ10:
		tp_set_state(tp, TS_WIND_ORDREL);
		break;
	case TS_WACK_DREQ11:
		tp_set_state(tp, TS_WREQ_ORDREL);
		break;
	default:
		break;
	}
	return dl_error_ack(tp->dl, q, mp, prim, error);
}

/**
 * tp_ok_ack: - process received T_OK_ACK primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the T_OK_ACK primitive
 */
static fastcall noinline __unlikely int
tp_ok_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
	struct T_ok_ack *p = (typeof(p)) mp->b_rptr;
	dl_ulong prim;

	switch (p->CORRECT_prim) {
	case T_CONN_REQ:
		/* Note that TPI sends T_OK_ACK in response to T_CONN_REQ,
		   whereas NPI and DLPI do not.  They only send N_ERROR_ACK or
		   DL_ERROR_ACK if there is a problem. */
		prim = DL_CONNECT_REQ;
		break;
	case T_CONN_RES:
		prim = DL_CONNECT_RES;
		break;
	case T_DISCON_REQ:
		prim = DL_DISCONNECT_REQ;
		break;
	case T_UNBIND_REQ:
		/* FIXME */
		prim = DL_UNBIND_REQ;
		prim = DL_SUBS_UNBIND_REQ;
		break;
	case T_OPTMGMT_REQ:
		prim = DL_UDQOS_REQ;
		break;
	default:
		freemsg(mp);
		mi_strlog(tp->oq, 0, SL_ERROR, "%s: T_ERROR_ACK for primitive %s", __FUNCTION__,
			  tp_primname(p->ERROR_prim));
		return (0);
	}
	switch (tp_get_state(tp)) {
	case TS_WACK_BREQ:
		tp_set_state(tp, TS_IDLE);
		break;
	case TS_WACK_UREQ:
		tp_set_state(tp, TS_UNBND);
		break;
	case TS_WACK_OPTREQ:
		tp_set_state(tp, TS_IDLE);
		break;
	case TS_WACK_CREQ:
		/* Note that DLPI does not wait for DL_OK_ACK having issued a
		   DL_CONNECT_REQ, but moves directly tot he DL_OUTCON_PENDING
		   state and awaits either DL_ERROR_ACK, DL_DISCONNECT_IND or
		   DL_CONNECT_CON.  Free this excess primitive here. */
		tp_set_state(tp, TS_WCON_CREQ);
		freemsg(mp);
		return (0);
	case TS_WACK_CRES:
		/* If we accepted on the same stream, the we need to move to
		   the TS_DATA_XFER state instead of the TS_IDLE state.
		   Otherwise it is the accepting stream that moves to
		   TS_DATA_XFER. */
		tp->coninds--;
		tp_set_state(tp, (tp->coninds > 0) ? TS_WRES_CIND : TS_IDLE);
		tp_set_state(tp->ap, TS_DATA_XFER);
		break;
	case TS_WACK_DREQ6:
		tp_set_state(tp, TS_IDLE);
		break;
	case TS_WACK_DREQ7:
		tp->coninds--;
		tp_set_state(tp, (tp->coninds > 0) ? TS_WRES_CIND : TS_IDLE);
		break;
	case TS_WACK_DREQ9:
		tp_set_state(tp, TS_IDLE);
		break;
	case TS_WACK_DREQ10:
		tp_set_state(tp, TS_IDLE);
		break;
	case TS_WACK_DREQ11:
		tp_set_state(tp, TS_IDLE);
		break;
	default:
		break;
	}
	return dl_ok_ack(tp->dl, q, mp, prim);
}

/**
 * tp_unitdata_ind: - process received T_UNITDATA_IND primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the T_UNITDATA_IND primitive
 */
static fastcall noinline __unlikely int
tp_unitdata_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_uderror_ind: - process received T_UDERROR_IND primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the T_UDERROR_IND primitive
 */
static fastcall noinline __unlikely int
tp_uderror_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_optmgmt_ack: - process received T_OPTMGMT_ACK primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the T_OPTMGMT_ACK primitive
 */
static fastcall noinline __unlikely int
tp_optmgmt_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_ordrel_ind: - process received T_ORDREL_IND primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the T_ORDREL_IND primitive
 */
static fastcall noinline __unlikely int
tp_ordrel_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_optdata_ind: - process received T_OPTDATA_IND primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the T_OPTDATA_IND primitive
 */
static fastcall noinline __unlikely int
tp_optdata_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_addr_ack: - process received T_ADDR_ACK primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the T_ADDR_ACK primitive
 */
static fastcall noinline __unlikely int
tp_addr_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/**
 * tp_capability_ack: - process received T_CAPABILITY_ACK primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the T_CAPABILITY_ACK primitive
 */
static fastcall noinline __unlikely int
tp_capability_ack(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

int
dummy(void)
{
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
	case DL_DATA_REQ:
		rtn = dl_data_req(dl, q, mp);
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
	}
}

int
dummy(void)
{
	switch (prim) {
	case T_CONN_IND:
		rtn = tp_conn_ind(tp, q, mp);
		break;
	case T_CONN_CON:
		rtn = tp_conn_con(tp, q, mp);
		break;
	case T_DISCON_IND:
		rtn = tp_discon_ind(tp, q, mp);
		break;
	case T_DATA_IND:
		rtn = tp_data_ind(tp, q, mp);
		break;
	case T_EXDATA_IND:
		rtn = tp_exdata_ind(tp, q, mp);
		break;
	case T_INFO_ACK:
		rtn = tp_info_ack(tp, q, mp);
		break;
	case T_BIND_ACK:
		rtn = tp_bind_ack(tp, q, mp);
		break;
	case T_ERROR_ACK:
		rtn = tp_error_ack(tp, q, mp);
		break;
	case T_OK_ACK:
		rtn = tp_ok_ack(tp, q, mp);
		break;
	case T_UNITDATA_IND:
		rtn = tp_unitdata_ind(tp, q, mp);
		break;
	case T_UDERROR_IND:
		rtn = tp_uderror_ind(tp, q, mp);
		break;
	case T_OPTMGMT_ACK:
		rtn = tp_optmgmt_ack(tp, q, mp);
		break;
	case T_ORDREL_IND:
		rtn = tp_ordrel_ind(tp, q, mp);
		break;
	case T_OPTDATA_IND:
		rtn = tp_optdata_ind(tp, q, mp);
		break;
	case T_ADDR_ACK:
		rtn = tp_addr_ack(tp, q, mp);
		break;
	case T_CAPABILITY_ACK:
		rtn = tp_capability_ack(tp, q, mp);
		break;
	}
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * --------------------------------------------------------------------------
 */

/**
 * dl_m_data: - process received M_DATA message
 * @q: active queue (upper write queue)
 * @mp: the M_DATA message
 */
static fastcall noinline __unlikely int
dl_m_data(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);	/* already locked */

	return dl_data_req(dl, q, mp);
}

/**
 * dl_m_proto: - process received M_PROTO or M_PCPROTO message
 * @q: active queue (upper write queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
static fastcall noinline __unlikely int
dl_m_proto(queue_t *q, mblk_t *mp)
{
	struct dl *dl;
	dl_ulong prim;
	int rtn = -EAGAIN;

	dl = DL_PRIV(q);	/* already locked */
	dl_save_state(dl);
	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	prim = *(dl_ulong *) mp->b_rptr;
	/* fast paths for data */
	if (likely(prim == DL_UNITDATA_REQ)) {
		rtn = dl_unitdata_req(dl, q, mp);
		goto done;
	}
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
	}
      done:
	if (rtn)
		dl_restore_state(dl);
	return (rtn);
}

/**
 * dl_m_ctl: - process received M_CTL or M_PCCTL message
 * @q: active queue (upper write queue)
 * @mp: the M_CTL or M_PCCTL message
 *
 * Eventually provide a trusted intermodule interface.  For now, treat the intermodule interface as
 * untrusted.
 */
static fastcall noinline __unlikely int
dl_m_ctl(queue_t *q, mblk_t *mp)
{
	return dl_m_proto(q, mp);
}

/**
 * dl_m_sig: - process received M_SIG or M_PCSIG message
 * @q: active queue (upper write queue)
 * @mp: the M_SIG or M_PCSIG message
 */
static fastcall noinline __unlikely int
dl_m_sig(queue_t *q, mblk_t *mp)
{
	freemsg(mp);		/* handle when we have timers */
	return (0);
}

/**
 * dl_m_rse: - process received M_RSE or M_PCRSE message
 * @q: active queue (upper write queue)
 * @mp: the M_RSE or M_PCRSE message
 *
 * Ugggh.  Free them.
 */
static fastcall noinline __unlikely int
dl_m_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * dl_m_flush: - process received M_FLUSH message
 * @q: active queue (upper write queue)
 * @mp: the M_FLUSH message
 *
 * Canonical flush handling for a STREAMS module.
 */
static fastcall noinline __unlikely int
dl_m_flush(queue_t *q, mblk_t *mp)
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

/**
 * dl_m_ioctl: - process received M_IOCTL message
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * We don't have an input-output controls of our own so pass them to the driver.
 */
static fastcall noinline __unlikely int
dl_m_ioctl(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/**
 * dl_m_iocdata: - process received M_IOCDATA message
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 *
 * We don't have an input-output controls of our own so pass them to the driver.
 */
static fastcall noinline __unlikely int
dl_m_iocdata(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/**
 * dl_m_read: - process received M_READ message
 * @q: active queue (upper write queue)
 * @mp: the M_READ message
 *
 * We don't process M_READ ourselves, so pass them to the driver.
 */
static fastcall noinline __unlikely int
dl_m_read(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/**
 * dl_m_other: - process received other message
 * @q: active queue (upper write queue)
 * @mp: the other message
 *
 * Pass along what we don't recognize (or process).
 */
static fastcall noinline __unlikely int
dl_m_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "%s; bad message type on write queue %d\n", __FUNCTION__,
		  (int) DB_TYPE(mp));
	putnext(q, mp);
	return (0);
}

/**
 * tp_m_data: - process received M_DATA message
 * @q: active queue (lower read queue)
 * @mp: the M_DATA message
 *
 * Don't need to process the data, just pass it along.  Which means that we could just push tirdwr
 * module and let the data pass through.
 */
static fastcall noinline __unlikely int
tp_m_data(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * tp_m_proto: - process received M_PROTO or M_PCPROTO message
 * @q: active queue (lower read queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
static fastcall noinline __unlikely int
tp_m_proto(queue_t *q, mblk_t *mp)
{
	struct tp *tp;
	t_uscalar_t prim;
	int rtn = -EFAULT;

	tp = TP_PRIV(q);	/* already locked */
	tp_save_state(tp);
	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	prim = *(t_uscalar_t *) mp->b_rptr;
	/* fast paths for data */
	if (likely(prim == T_OPTDATA_IND)) {
		rtn = tp_optdata_ind(tp, q, mp);
		goto done;
	}
	if (likely(prim == T_DATA_IND)) {
		rtn = tp_data_ind(tp, q, mp);
		goto done;
	}
	if (likely(prim = T_EXDATA_IND)) {
		rtn = tp_exdata_ind(tp, q, mp);
		goto done;
	}
	switch (prim) {
	case T_CONN_IND:
		rtn = tp_conn_ind(tp, q, mp);
		break;
	case T_CONN_CON:
		rtn = tp_conn_con(tp, q, mp);
		break;
	case T_DISCON_IND:
		rtn = tp_discon_ind(tp, q, mp);
		break;
	case T_DATA_IND:
		rtn = tp_data_ind(tp, q, mp);
		break;
	case T_EXDATA_IND:
		rtn = tp_exdata_ind(tp, q, mp);
		break;
	case T_INFO_ACK:
		rtn = tp_info_ack(tp, q, mp);
		break;
	case T_BIND_ACK:
		rtn = tp_bind_ack(tp, q, mp);
		break;
	case T_ERROR_ACK:
		rtn = tp_error_ack(tp, q, mp);
		break;
	case T_OK_ACK:
		rtn = tp_ok_ack(tp, q, mp);
		break;
	case T_UNITDATA_IND:
		rtn = tp_unitdata_ind(tp, q, mp);
		break;
	case T_UDERROR_IND:
		rtn = tp_uderror_ind(tp, q, mp);
		break;
	case T_OPTMGMT_ACK:
		rtn = tp_optmgmt_ack(tp, q, mp);
		break;
	case T_ORDREL_IND:
		rtn = tp_ordrel_ind(tp, q, mp);
		break;
	case T_OPTDATA_IND:
		rtn = tp_optdata_ind(tp, q, mp);
		break;
	case T_ADDR_ACK:
		rtn = tp_addr_ack(tp, q, mp);
		break;
	case T_CAPABILITY_ACK:
		rtn = tp_capability_ack(tp, q, mp);
		break;
	}
      done:
	if (rtn)
		tp_restore_state(tp);
	return (rtn);
}

/**
 * tp_m_sig: - process received M_SIG or M_PCSIG message
 * @q: active queue (lower read queue)
 * @mp: the M_SIG or M_PCSIG message
 *
 * If these are short messages they are true M_SIG or M_PCSIG signals heading for the Stream head.
 * Just pass them along with flow control.
 */
static fastcall noinline __unlikely int
tp_m_sig(queue_t *q, mblk_t *mp)
{
	if (!MBLKIN(mp, 0, sizeof(int))) {
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
	freemsg(mp);		/* process when we have timers */
	return (0);
}

/**
 * tp_m_ctl: - process received M_CTL or M_PCCTL message
 * @q: active queue (lower read queue)
 * @mp: the M_CTL or M_PCCTL message
 *
 * We always trust the lower interface, so treat them the same as M_PROTO or M_PCPROTO.
 */
static fastcall noinline __unlikely int
tp_m_ctl(queue_t *q, mblk_t *mp)
{
	return tp_m_proto(q, mp);
}

/**
 * tp_m_rse: - process received M_RSE or M_PCRSE message
 * @q: active queue (lower read queue)
 * @mp: the M_RSE or M_PCRSE message
 *
 * Ugly, don't pass these around.
 */
static fastcall noinline __unlikely int
tp_m_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * tp_m_flush: - process received M_FLUSH message
 * @q: active queue (lower read queue)
 * @mp: the M_FLUSH message
 *
 * Canonical flush handling for a STREAMS module
 */
static fastcall noinline __unlikely int
tp_m_flush(queue_t *q, mblk_t *mp)
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

/**
 * tp_m_error: - process received M_ERROR message
 * @q: active queue (lower read queue)
 * @mp: the M_ERROR message
 *
 * Whoah.  Bad.  The stream is errored out.  Simply pass it directly to the Stream head so that the
 * entire Stream can error out.  Flush our queues as well and go to a bad state.
 */
static fastcall noinline __unlikely int
tp_m_error(queue_t *q, mblk_t *mp)
{
	tp_set_state(tp, -1);
	flushq(q, FLUSHALL);
	flushq(WR(q), FLUSHALL);
	putnext(q, mp);
	return (0);
}

/**
 * tp_m_hangup: - process received M_HANGUP message
 * @q: active queue (lower read queue)
 * @mp: the M_HANGUP message
 *
 * Hmmm.  Just pass it along directly.  It only affects the Stream head and there might be an
 * unhangup later.
 */
static fastcall noinline __unlikely int
tp_m_hangup(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/**
 * tp_m_setopts: - process received M_SETOPTS or M_PCSETOPTS message
 * @q: active queue (lower read queue)
 * @mp: the M_SETOPTS or M_PCSETOPTS message
 *
 * We are really just converting between interfaces, so any write offsets or other stream head
 * thingies that need to be done are ok.  Just pass them along.
 */
static fastcall noinline __unlikely int
tp_m_setopts(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/**
 * tp_m_copy: - process received M_COPYIN or M_COPYOUT message
 * @q: active queue (lower read queue)
 * @mp: the M_COPYIN or M_COPYOUT message
 *
 * We don't have any input output controls of our own, so pass along.
 */
static fastcall noinline __unlikely int
tp_m_copy(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/**
 * tp_m_iocack: - process received M_IOCACK or M_IOCNAK message
 * @q: active queue (lower read queue)
 * @mp: the M_IOCACK or M_IOCNAK message
 *
 * We don't have any input output controls of our own, so pass along.
 */
static fastcall noinline __unlikely int
tp_m_iocack(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/**
 * tp_m_other: - process received other message
 * @q: active queue (lower read queue)
 * @mp: the other message
 *
 * Modules should pass unrecognized messages along.
 */
static fastcall noinline __unlikely int
tp_m_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "%s: unrecognized STREAMS message on read queue, %d",
		  __FUNCTION__, (int) DB_TYPE(mp));
	putnext(q, mp);
	return (0);
}

static fastcall inline int
dl_msg_put(queue_t *q, mblk_t *mp)
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
		return dl_m_flush(q, mp);
	case M_IOCTL:
		return dl_m_ioctl(q, mp);
	case M_IOCDATA:
		return dl_m_iocdata(q, mp);
	case M_READ:
		return dl_m_read(q, mp);
	default:
		return dl_m_other(q, mp);
	}
}
static fastcall noinline int
dl_msg_srv(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return dl_m_data(q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return dl_m_proto(q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return dl_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_m_proto(q, mp);
	case M_CTL:
	case M_PCCTL:
		return dl_m_ctl(q, mp);
	case M_SIG:
	case M_PCSIG:
		return dl_m_sig(q, mp);
	case M_RSE:
	case M_PCRSE:
		return dl_m_rse(q, mp);
	case M_FLUSH:
		return dl_m_flush(q, mp);
	case M_IOCTL:
		return dl_m_ioctl(q, mp);
	case M_IOCDATA:
		return dl_m_iocdata(q, mp);
	case M_READ:
		return dl_m_read(q, mp);
	default:
		return dl_m_other(q, mp);
	}
}
static fastcall noinline int
tp_msg_srv(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_PROTO))
		return tp_m_proto(q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return tp_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tp_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return tp_m_sig(q, mp);
	case M_CTL:
	case M_PCCTL:
		return tp_m_ctl(q, mp);
	case M_RSE:
	case M_PCRSE:
		return tp_m_rse(q, mp);
	case M_FLUSH:
		return tp_m_flush(q, mp);
	case M_ERROR:
		return tp_m_error(q, mp);
	case M_HANGUP:
	case M_UNHANGUP:
		return tp_m_hangup(q, mp);
	case M_SETOPTS:
	case M_PCSETOPTS:
		return tp_m_setopts(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return tp_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return tp_m_iocack(q, mp);
	default:
		return tp_m_other(q, mp);
	}
}
static fastcall inline int
tp_msg_put(queue_t *q, mblk_t *mp)
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
		return tp_m_flush(q, mp);
	case M_ERROR:
		return tp_m_error(q, mp);
	case M_HANGUP:
	case M_UNHANGUP:
		return tp_m_hangup(q, mp);
	case M_SETOPTS:
	case M_PCSETOPTS:
		return tp_m_setopts(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return tp_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return tp_m_iocack(q, mp);
	default:
		return tp_m_other(q, mp);
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
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flags & QSVCBUSY))) || dl_msg_put(q, mp))
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
			if (dl_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}
static streamscall int
tp_rsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (tp_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}
static streamscall int
tp_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flags & QSVCBUSY))) || tp_msg_put(q, mp))
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

caddr_t xot_opens = NULL;
rwlock_t xot_rwlock = RW_LOCK_UNLOCKED;

static int
xot_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	struct dl *dl;
	struct tp *tp;
	mblk_t *mp;
	int err;
	pl_t pl;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (!(mp = allocb(sizeof(struct T_capability_req), BPRI_WAITOK)))
		return (ENOBUFS);
	pl = RW_WRLOCK(&xot_rwlock, plstr);
	if ((err = mi_open_comm(&xot_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		RW_UNLOCK(&xot_rwlock, pl);
		freeb(mp);
		return (err);
	}
	RW_UNLOCK(&xot_rwlock, pl);

	p = PRIV(q);
	ATOMIC_INT_INIT(&p->refs, 1);
	dl = &p->w_priv;
	tp = &p->r_priv;

	/* initialize private structure */
	dl->priv = p;
	dl->tp = tp;
	dl->oq = q;
	dl->state = DL_UNBOUND;
	dl->oldstate = DL_UNBOUND;

	tp->priv = p;
	tp->dl = dl;
	tp->oq = WR(q);
	tp->state = TS_UNBND;
	tp->oldstate = TS_UNBND;

	/* FIXME: more initialization */

	DB_TYPE(mp) = M_PCPROTO;
	((struct T_capability_req *) mp->b_wptr)->PRIM_type = T_CAPABILITY_REQ;
	mp->b_wptr += sizeof(struct T_capability_req);

	qprocson(q);
	putnext(q, mp);
	return (0);
}

/**
 * xot_qclose: - STREAMS close procedure
 * @q: queue pair to close
 * @oflags: flags to open(2s)
 * @crp: credientials of closer
 *
 * Note that we do not simply use mi_close_comm.  This is because an
 * listening stream may reference this stream as an accepting stream, so we
 * must reference count.  The close procedure is broken into three steps:
 * mi_detach, detach the instance data from the queue pair; mi_close_unlink,
 * remove the instance data from the master list; and, dl_put free the
 * instance data only once the last reference count has been released.
 */
static int
xot_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	if (p == NULL)
		return (0);
	qprocsoff(q);
	/* FIXME: free timers and other things. */
	{
		pl_t pl = RW_WRLOCK(&xot_rwlock, plstr);

		/* detach private structure from queue pair */
		mi_detach(q, (caddr_t) p);
		/* remove structure from open list */
		mi_close_unlink(&xot_opens, (caddr_t) p);
		RW_UNLOCK(&xot_rwlock, pl);
	}
	/* This does not call mi_close_free until the last reference is
	   released. */
	priv_put(p);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

static struct qinit xot_rinit = {
	.qi_putp = &tp_wput,
	.qi_srvp = &tp_wsrv,
	.qi_qopen = &xot_qopen,
	.qi_qclose = &xot_qclose,
	.qi_minfo = &xot_minfo,
	.qi_mstat = &xot_rstat,
};

static struct qinit xot_winit = {
	.qi_putp = &dl_wput,
	.qi_srvp = &dl_wsrv,
	.qi_minfo = &xot_minfo,
	.qi_mstat = &xot_wstat,
};

struct streamtab xot_info = {
	.st_rdinit = &xot_rinit,
	.st_wrinit = &xot_winit,
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
MODULE_PARM_DESC(modid, "Module ID for XOT.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static __init int
xot_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&xot_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
xot_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&xot_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(xot_modinit);
module_exit(xot_modexit);

#endif				/* LINUX */
