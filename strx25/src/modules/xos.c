/*****************************************************************************

 @(#) $RCSfile: xos.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:57 $

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

 Last Modified $Date: 2008-06-13 06:43:57 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xos.c,v $
 Revision 0.9.2.1  2008-06-13 06:43:57  brian
 - added files

 *****************************************************************************/

#ident "@(#) $RCSfile: xos.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:57 $"

static char const ident[] = "$RCSfile: xos.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:57 $";

/*
 * This is a XOS [draft-bidulock-tsvwg-xos-00] module.  It pushes over a TCP
 * TPI stream and provides DLPI interface for use by the X25-PLP module.  This
 * datalink has behaviour very close to that of a LAPB data link and can be
 * conifugured accordingly.
 *
 * The upper layer service interface is the Data Link Provider Interface
 * (DLPI) as interpreted for XOS, and the lower layer service interface is the
 * Transport Provider Interface (TPI) as intepreted for SCTP.
 */

#include <sys/os7/compat.h>
#include <sys/dlpi.h>
#include <sys/npi.h>
#include <sys/npi_sctp.h>
#include <sys/tihdr.h>
#include <sys/tpi_sctp.h>

#define XOS_DESCRIPT	"X.25 OVER SCTP MODULE FOR LINUX FAST-STREAMS"
#define XOS_EXTRA	"Part fo the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define XOS_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define XOS_REVISION	"OpenSS7 $RCSfile: xos.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:57 $"
#define XOS_DEVICE	"SVR 4.2MP X.25 over SCTP Module (XOS) for X.25 CONS"
#define XOS_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define XOS_LICENSE	"GPL"
#define XOS_BANNER	XOS_DESCRIP	"\n" \
			XOS_EXTRA	"\n" \
			XOS_COPYRIGHT	"\n" \
			XOS_REVISION	"\n" \
			XOS_DEVICE	"\n" \
			XOS_CONTACT	"\n"
#define XOS_SPLASH	XOS_DESCRIP	" - " \
			XOS_REVISION

#ifndef CONFIG_STREAMS_XOS_NAME
#error "CONFIG_STREAMS_XOS_NAME must be defined."
#endif				/* CONFIG_STREAMS_XOS_NAME */
#ifndef CONFIG_STREAMS_XOS_MODID
#error "CONFIG_STREAMS_XOS_MODID must be defined."
#endif				/* CONFIG_STREAMS_XOS_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(XOS_CONTACT);
MODULE_DESCRIPTION(XOS_DESCRIP);
MODULE_SUPPORTED_DEVICE(XOS_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(XOS_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-xos");
MODULE_ALIAS("streams-module-xos");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_XOS_MODID));
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#endif				/* LINUX */

#ifndef XOS_MOD_NAME
#define XOS_MOD_NAME		CONFIG_STREAMS_XOS_NAME
#endif				/* XOS_MOD_NAME */
#ifndef XOS_MOD_MOD_ID
#define XOS_MOD_MOD_ID		CONFIG_STREAMS_XOS_MODID
#endif				/* XOS_MOD_MOD_ID */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define MOD_ID		XOS_MOD_ID
#define MOD_NAME	XOS_MOD_NAME
#ifdef MODULE
#define MODULE		XOS_BANNER
#else				/* MODULE */
#define MODULE		XOS_SPLASH
#endif				/* MODULE */

static struct module_info xos_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat xos_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat xos_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 * --------------------------------------------------------------------------
 *
 * PRIVATE STRUCTURE
 *
 * --------------------------------------------------------------------------
 */

struct dl;
struct tp;
struct np;
struct priv;

/* Upper interface structure. */
struct dl {
	struct priv *priv;
	struct tp *tp;
	struct np *np;
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
		dl_qos_co_sel1_t qos;
		dl_qos_co_range1_t qor;
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

/* Lower interface structure. */
struct np {
	struct priv *priv;
	struct dl *dl;
	queue_t *oq;
	np_ulong state;
	np_ulong oldstate;
	np_ulong coninds;
	np_ulong oldconinds;
	struct np *ap;			/* accepting provider */
	struct {
		N_info_ack_t info;
	};
};

struct priv {
	atomic_int_t refs;
	struct dl dlpi;
	struct tp tpi;
	struct np npi;
};

#define PRIV(q) ((struct priv *)q->q_ptr)
#define DL_PRIV(q) (&PRIV(q)->dlpi)
#define TP_PRIV(q) (&PRIV(q)->tpi)

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

static struct np *
np_get(struct np *np)
{
	if (np)
		priv_get(np->priv);
	return (np);
}
static void
np_put(struct np *np)
{
	if (np)
		priv_put(np->priv);
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

static inline const char *
np_iocname(int cmd)
{
	switch (cmd) {
	default:
		return ("(unknown)");
	}
}
static inline const char *
np_primname(np_ulong prim)
{
	switch (cmd) {
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
static inline const char *
np_statename(np_ulong state)
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
static inline const char *
np_errname(np_long error)
{
	if (error < 0)
		return ("NSYSERR");
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
static inline const char *
np_strerror(np_long error)
{
	if (error < 0)
		return ("System error.");
	switch (error) {
	case NBADADDR:
		return ("Incorrect address format/illegal address information.");
	case NBADOPT:
		return ("Options in incorrect format or contain illegal information.");
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
		return ("Illegal second attempt to bind listener or default listener.");
	case NBADQOSPARAM:
		return ("QOS values specified are outside the range supported by the NS provider.");
	case NBADQOSTYPE:
		return ("QOS structure type specified is not supported by the NS provider.");
	case NBADTOKEN:
		return ("Token used is not associated with an open stream.");
	case NNOPROTOID:
		return ("Protocol id could not be allocated.");
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
		tp->proto.caps.INFO_ack.CURRENT_state = newstate;
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

static np_ulong
np_get_state(struct np *np)
{
	return (np->state);
}

static np_ulong
np_set_state(struct np *np, np_ulong newstate)
{
	np_ulong oldstate = np->state;

	if (newstate != oldstate) {
		np->state = newstate;
		np->proto.info.CURRENT_state = newstate;
		mi_strlog(np->oq, STRLOGST, SL_TRACE, "%s <- %s", np_statename(newstate),
			  np_statename(oldstate));
	}
	return (newstate);
}

static np_ulong
np_save_state(struct np *np)
{
	np->oldconinds = np->coninds;
	return ((np->oldstate = np_get_state(np)));
}

static np_ulong
np_restore_state(struct np *np)
{
	np->coninds = np->oldconinds;
	return (np_set_state(np, np->oldstate));
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
	dl_info_ack_t *p;
	mblk_t *mp;
	size_t add_len = dl->proto.info.dl_addr_length;
	size_t qos_len = dl->proto.info.dl_qos_length;
	size_t qor_len = dl->proto.info.dl_qos_range_length;
	size_t brd_len = dl->proto.info.dl_brdcst_addr_length;
	size_t msg_len = sizeof(*p) + add_len + qos_len + qor_len + brd_len;

	if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_INFO_ACK;
		p->dl_max_sdu = dl->proto.info.dl_max_sdu;
		p->dl_min_sdu = dl->proto.info.dl_min_sdu;
		p->dl_addr_length = dl->proto.info.dl_addr_length;
		p->dl_addr_offset = sizeof(*p);
		p->dl_mac_type = dl->proto.info.dl_mac_type;
		p->dl_reserved = dl->proto.info.dl_reserved;
		p->dl_current_state = dl->proto.info.dl_current_state;
		p->dl_sap_length = dl->proto.info.dl_sap_length;
		p->dl_service_mode = dl->proto.info.dl_service_mode;
		p->dl_qos_length = dl->proto.info.dl_qos_length;
		p->dl_qos_offset = p->dl_addr_offset + p->dl_addr_length;
		p->dl_qos_range_length = dl->proto.info.dl_qos_range_length;
		p->dl_qos_range_offset = p->dl_qos_offset + p->dl_qos_length;
		p->dl_provider_style = dl->proto.info.dl_provider_style;
		p->dl_version = dl->proto.info.dl_version;
		p->dl_brdcst_addr_length = dl->proto.info.dl_brdcst_addr_length;
		p->dl_brdcst_addr_offset = p->dl_qos_range_offset + p->dl_qos_range_length;
		p->dl_growth = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(&dl->proto.add, mp->b_wptr, p->dl_addr_length);
		mp->b_wptr += p->dl_addr_length;
		bcopy(&dl->proto.qos, mp->b_wptr, p->dl_qos_length);
		mp->b_wptr += p->dl_qos_length;
		bcopy(&dl->proto.qor, mp->b_wptr, p->dl_qos_range_length);
		mp->b_wptr += p->dl_qos_range_length;
		bcopy(&dl->proto.brd, mp->b_wptr, p->dl_brdcst_addr_length);
		mp->b_wptr += p->dl_brdcst_addr_length;
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_INFO_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
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
dl_unitdata_ind(struct dl *dl, queue_t *q, mblk_t *msg, struct sockaddr_in
		*dst, struct sockaddr_in *src, dl_ulong group, mblk_t *dp)
{
	dl_unitdata_ind_t *p;
	mblk_t *mp;
	size_t dst_len = dst ? sizeof(dst->sin_addr) + sizeof(dst->sin_port) : 0;
	size_t src_len = src ? sizeof(src->sin_addr) + sizeof(src->sin_port) : 0;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + dst_len + src_len, BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_UNITDATA_IND;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = sizeof(*p);
			p->dl_src_addr_length = src_len;
			p->dl_src_addr_offset = sizeof(*p) + dst_len;
			p->dl_group_address = group;
			mp->b_wptr += sizeof(*p);
			if (dst) {
				bcopy(&dst->sin_addr, mp->b_wptr, sizeof(dst->sin_addr));
				mp->b_wptr += sizeof(dst->sin_addr);
				bcopy(&dst->sin_port, mp->b_wptr, sizeof(dst->sin_port));
				mp->b_wptr += sizeof(dst->sin_port);
			}
			if (src) {
				bcopy(&src->sin_addr, mp->b_wptr, sizeof(src->sin_addr));
				mp->b_wptr += sizeof(src->sin_addr);
				bcopy(&src->sin_port, mp->b_wptr, sizeof(src->sin_port));
				mp->b_wptr += sizeof(src->sin_port);
			}
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(dl->oq, STRLOGDA, SL_TRACE, "<- DL_UNITDATA_IND");
			putnext(dl->oq, mp);
			return (0);

		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * dl_uderror_ind: - issue DL_UDERROR_IND to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_uderror_ind(struct dl *dl, queue_t *q, mblk_t *msg, struct sockaddr_in
	       *dst, dl_long error, mblk_t *dp)
{
	dl_uderror_ind_t *p;
	mblk_t *p;
	size_t dst_len = dst ? sizeof(dst->sin_addr) + sizeof(dst->sin_port) : 0;
	unsigned char mtype = (error < 0x80) ? M_PCPROTO : M_PROTO;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + dst_len, BPRI_MED)))) {
		if (likely(pcmsg(mtype) || bcanputnext(dl->oq, 1))) {
			DB_TYPE(mp) = mtype;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_UDERROR_IND;
			p->dl_dest_addr_length = dst_len;
			p->dl_dest_addr_offset = sizeof(*p);
			p->dl_unix_errno = (error < 0) ? -error : 0;
			p->dl_errno = (error < 0) ? DL_SYSERR : error;
			mp->b_wptr += sizeof(*p);
			if (dst) {
				bcopy(&dst->sin_addr, mp->b_wptr, sizeof(dst->sin_addr));
				mp->b_wptr += sizeof(dst->sin_addr);
				bcopy(&dst->sin_port, mp->b_wptr, sizeof(dst->sin_port));
				mp->b_wptr += sizeof(dst->sin_port);
			}
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(dl->oq, STRLOGDA, SL_TRACE, "<- DL_UDERROR_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);

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
dl_reset_ind(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong orig, dl_ulong
	     reason)
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
			dl_set_state(dl, DL_PROV_RESET_PENDING);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_RESET_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
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
	dl_reset_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_RESET_CON;
			mp->b_wptr += sizeof(*p);
			dl_set_state(dl, DL_DATAXFER);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_RESET_CON");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
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
	return dl_error_reply(dl, q, msg, DL_DATA_ACK_IND, DL_NOTSUPPORTED);
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
	return dl_error_reply(dl, q, msg, DL_DATA_ACK_STATUS_IND, DL_NOTSUPPORTED);
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
	return dl_error_reply(dl, q, msg, DL_REPLY_IND, DL_NOTSUPPORTED);
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
	return dl_error_reply(dl, q, msg, DL_REPLY_STATUS_IND, DL_NOTSUPPORTED);
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
	return dl_error_reply(dl, q, msg, DL_REPLY_UPDATE_STATUS_IND, DL_NOTSUPPORTED);
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
	return dl_error_reply(dl, q, msg, DL_XID_RES, DL_NOTSUPPORTED);
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
	return dl_error_reply(dl, q, msg, DL_XID_REQ, DL_NOTSUPPORTED);
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
	return dl_error_reply(dl, q, msg, DL_TEST_RES, DL_NOTSUPPORTED);
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
	return dl_error_reply(dl, q, msg, DL_TEST_REQ, DL_NOTSUPPORTED);
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
	return dl_error_reply(dl, q, msg, DL_PHYS_ADDR_REQ, DL_NOTSUPPORTED);
}

/**
 * dl_get_statistics_ack: - issue DL_GET_STATISTICS_ACK to the DLPI user
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall noinline __unlikely int
dl_get_statistics_ack(struct dl *dl, queue_t *q, mblk_t *msg, caddr_t sta_ptr, size_t sta_len)
{
	dl_get_statistics_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + sta_len, BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_GET_STATISTICS_ACK;
			p->dl_stat_length = sta_len;
			p->dl_stat_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(sta_ptr, mp->b_wptr, sta_len);
			mp->b_wptr += sta_len;
			freemsg(msg);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_GET_STATISTICS_ACK");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * TPI USER TO TPI PROVIDER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 * These are primitives issued to the SCTP TPI provider beneath this module.
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
 * determine which options to feed to SCTP on the connection response.
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
 * NPI USER TO NPI PROVIDER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 * These are primitives issued to the SCTP NPI provider beneath this module.
 */

/**
 * np_conn_req: - issue N_CONN_REQ to the NPI provider
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @dst: destination socket address
 * @flags: connection flags
 * @qos: SCTP quality of service parameter structure
 * @dp: user data
 */
static fastcall noinline __unlikely int
np_conn_req(struct np *np, queue_t *q, mblk_t *msg, struct sockaddr_in *dst,
	    size_t dst_len, np_ulong flags, np_ulong i_streams, np_ulong
	    o_streams, mblk_t *dp)
{
	N_conn_req_t *p;
	mblk_t *mp;
	N_qos_sel_conn_sctp_t *qos;

	if (likely(!!(mp = mi_allocb(q, sizeof(*mp) + dst_len + sizeof(*qos))))) {
		if (likely(canputnext(np->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_CONN_REQ;
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p);
			p->CONN_flags = flags;
			p->QOS_length = sizeof(*qos);
			p->QOS_offset = sizeof(*p) + dst_len;
			mp->b_wptr += sizeof(*p);
			bcopy(dst, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			qos = (typeof(qos)) mp->b_wptr;
			qos->n_qos_type = N_QOS_SEL_CONN_SCTP;
			qos->i_streams = i_streams;
			qos->o_streams = o_streams;
			mp->b_wptr += sizeof(*qos);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			np_set_state(np, NS_WCON_CREQ);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "NP_CONN_REQ ->");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_conn_res: - issue N_CONN_RES to the NPI provider
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_conn_res(struct np *np, queue_t *q, mblk_t *msg, struct np *ap, struct
	    sockaddr_in *res, np_ulong seq, np_ulong flags, N_qos_sctp_conn_t * qos, mblk_t *dp)
{
	N_conn_res_t *p;
	mblk_t *mp;
	size_t res_len = res ? sizeof(res->sin_addr) + sizeof(res->sin_port) : 0;
	size_t qos_len = qos ? sizeof(*qos) : 0;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + res_len + opt_len, BPRI_MED)))) {
		if (likely(canputnext(np->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = T_CONN_RES;
			p->TOKEN_value = ap->token;
			p->RES_length = res_len;
			p->RES_offset = sizeof(*p);
			p->SEQ_number = seq;
			p->CONN_flags = flags;
			p->QOS_length = qos_len;
			p->QOS_offset = sizeof(*p) + res_len;
			mp->b_wptr += sizeof(*p);
			if (res) {
				bcopy(&res->sin_addr, mp->b_wptr, sizeof(res->sin_addr));
				mp->b_wptr += sizeof(res->sin_addr);
				bcopy(&res->sin_port, mp->b_wptr, sizeof(res->sin_port));
				mp->b_wptr += sizeof(res->sin_port);
			}
			if (qos) {
				bcopy(qos, mp->b_wptr, qos_len);
				mp->b_qptr += qos_len;
			}
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			np_set_state(np, NS_WACK_CRES);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "N_CONN_RES ->");
			putnext(np->oq, mp);
			return (0);
		}
		np_put(ap);
		freeb(mp);
		return (-EBUSY);
	}
	np_put(ap);
	return (-ENOBUFS);
}

/**
 * np_discon_req: - issue N_DISCON_REQ to the NPI provider
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_discon_req(struct np *np, queue_t *q, mblk_t *msg, struct sockaddr_in *res,
	      size_t res_len, np_ulong reason, np_ulong seq, mblk_t *dp)
{
	N_discon_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + res_len, BPRI_MED)))) {
		if (likely(canputnext(np->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wtpr;
			p->PRIM_type = N_DISCON_REQ;
			p->DISCON_reason = reason;
			p->RES_length = res_len;
			p->RES_offset = sizeof(*p);
			p->SEQ_number = seq;
			mp->b_wptr += sizeof(*p);
			bcopy(res, mp->b_wptr, res_len);
			mp->b_wptr += res_len;
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			switch (np_get_state(np)) {
			case NS_WCON_CREQ:
				np_set_state(np, NS_WACK_DREQ6);
				break;
			case NS_WRES_CIND:
				np_set_state(np, NS_WACK_DREQ7);
				break;
			case NS_DATA_XFER:
				np_set_state(np, NS_WACK_DREQ9);
				break;
			case NS_WCON_RREQ:
				np_set_state(np, NS_WACK_DREQ10);
				break;
			case NS_WRES_RIND:
				np_set_state(np, NS_WACK_DREQ11);
				break;
			}
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "N_CONN_REQ ->");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		reutrn(-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_data_req: - issue N_DATA_REQ to the NPI provider
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_data_req(struct np *np, queue_t *q, mblk_t *msg, np_ulong more, np_ulong
	    sid, mblk_t *dp)
{
	N_qos_sel_data_sctp_t *qos;
	N_data_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + sizeof(*qos), BPRI_MED)))) {
		if (likely(canputnext(np->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_REQ;
			p->DATA_xfer_flags = more ? N_MORE_DATA_FLAG : 0;
			mp->b_wptr += sizeof(*p);
			qos = (typeof(qos)) mp->b_wptr;
			qos->n_qos_type = N_QOS_SEL_DATA_SCTP;
			qos->ppi = 0;
			qos->sid = sid;
			qos->ssn = DL_QOS_DONT_CARE;
			qos->tsn = DL_QOS_DONT_CARE;
			qos->more = more ? 1 : 0;
			mp->b_wptr += sizeof(*qos);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGDA, SL_TRACE, "N_DATA_REQ ->");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_exdata_req: - issue N_EXDATA_REQ to the NPI provider
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_exdata_req(struct np *np, queue_t *q, mblk_t *msg)
{
}

/**
 * np_info_req: - issue N_INFO_REQ to the NPI provider
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_info_req(struct np *np, queue_t *q, mblk_t *msg)
{
}

/**
 * np_bind_req: - issue N_BIND_REQ to the NPI provider
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_bind_req(struct np *np, queue_t *q, mblk_t *msg, struct sockaddr_in *add,
	    np_ulong conind, np_ulong flags)
{
	N_bind_req_t *p;
	mblk_t *mp;
	size_t add_len = add ? sizeof(*add) : 0;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED)))) {
		if (likely(canputnext(np->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_BIND_REQ;
			p->ADDR_length = add_len;
			p->ADDR_offset = sizeof(*p);
			p->CONIND_number = conind;
			p->BIND_flags = flags;
			p->PROTOID_length = 0;
			p->PROTOID_offset = 0;
			mp->b_wptr += sizeof(*p);
			bcopy(add, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
			freemsg(msg);
			np_set_state(np, NS_WACK_BREQ);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "N_BIND_REQ ->");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_unbind_req: - issue N_UNBIND_REQ to the NPI provider
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_unbind_req(struct np *np, queue_t *q, mblk_t *msg)
{
	N_unbind_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(np->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNBIND_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			np_set_state(np, NS_WACK_UREQ);
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "N_UNBIND_REQ ->");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_unitdata_req: - issue N_UNITDATA_REQ to the NPI provider
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_unitdata_req(struct np *np, queue_t *q, mblk_t *msg, struct sockaddr_in
		*dst, np_ulong sid, mblk_t *dp)
{
	N_qos_sel_data_sctp_t *qos;
	N_unitdata_req_t *p;
	mblk_t *mp;
	size_t dst_len = dst ? sizeof(*dst) : 0;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + sizeof(*qos) + dst_len, BPRI_MED)))) {
		if (likely(canpunext(np->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNITDATA_REQ;
			p->DEST_length = dst_len;
			p->DEST_offset = sizeof(*p) + sizeof(*qos);
			p->RESERVED_field[0] = sizeof(*qos);
			p->RESERVED_field[1] = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			qos = (typeof(qos)) mp->b_wptr;
			qos->n_qos_type = N_QOS_SEL_DATA_SCTP;
			qos->ppi = 0;
			qos->sid = sid;
			qos->ssn = DL_QOS_DONT_CARE;
			qos->tsn = DL_QOS_DONT_CARE;
			qos->more = 0;
			mp->b_wptr += sizeof(*qos);
			bcopy(dst, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(np->oq, STRLOGDA, SL_TRACE, "N_UNITDATA_REQ ->");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * np_optmgmt_req: - issue N_OPTMGMT_REQ to the NPI provider
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_optmgmt_req(struct np *np, queue_t *q, mblk_t *msg, np_ulong flags, caddr_t
	       qos_ptr, size_t qos_len)
{
	N_optmgmt_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + qos_len, BPRI_MED)))) {
		if (likely(canputnext(np->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_OPTMGMT_REQ;
			p->QOS_length = qos_len;
			p->QOS_offset = sizeof(*p);
			p->OPTMGMT_flags = flags;
			mp->b_wptr += sizeof(*p);
			bcopy(qos_ptr, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			mi_strlog(np->oq, STRLOGTX, SL_TRACE, "N_OPTMGMT_REQ ->");
			putnext(np->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-ENOBUFS);
	}
	return (-ENOBUFS);
}

/**
 * np_datack_req: - issue N_DATACK_REQ to the NPI provider
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_datack_req(struct np *np, queue_t *q, mblk_t *msg)
{
}

/**
 * np_reset_req: - issue N_RESET_REQ to the NPI provider
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_reset_req(struct np *np, queue_t *q, mblk_t *msg)
{
}

/**
 * np_reset_res: - issue N_RESET_RES to the NPI provider
 * @np: network provider private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
static fastcall noinline __unlikely int
np_reset_res(struct np *np, queue_t *q, mblk_t *msg)
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
	return dl_info_ack(dl, q, mp);
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
	int err;

	/* Always in wrong state. */
	goto oustate;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_ATTACH_REQ, err);
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
	int err;

	if (dl_get_state(dl) != DL_UNBOUND)
		goto outstate;
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_DETACH_REQ, err);
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
	dl_bind_req_t *p = (typeof(p)) mp->b_rptr;
	np_ulong conind, flags;
	int err;

	if (dl_get_state(dl) != DL_UNBOUND)
		goto outstate;
	if (!(p->dl_service_mode & DL_CODLS))
		goto unsupported;
	if (p->dl_service_mode & DL_ACLDLS)
		goto unsupported;
	if (p->dl_max_conind > 0 && !(p->dl_service_mode & DL_CODLS))
		goto unsupported;
	if (p->dl_sap & 0xffff0000)
		goto badsap;
	dl->add.sin_family = AF_INET;
	dl->add.sin_port = p->dl_sap;
	dl->add.sin_addr.in_addr = INADDR_ANY;
	conind = p->dl_max_conind;
	flags = p->dl_conn_mgmt ? DEFAULT_LISTENER : 0;
	dl_set_state(dl, DL_BIND_PENDING);
	return np_bind_req(dl->np, q, mp, &dl->add, conind, flags);
      badsap:
	err = DL_BADSAP;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_BIND_REQ, err);
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
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_state(dl, DL_UNBIND_PENDING);
	return np_unbind_req(dl->np, q, mp);
      oustate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_UNBIND_REQ, err);
}

/**
 * dl_subs_bind_req: - process received DL_SUBS_BIND_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_SUBS_BIND_REQ message
 *
 * No point in doing a subsequent bind, unless we just want to bind to a
 * specific IP address.  To accomplish that would take dropping the original
 * bind and binding to the requested address, which might fail.  Just skip it
 * for now.
 */
static fastcall noinline __unlikely int
dl_subs_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_SUBS_BIND_REQ, DL_NOTSUPPORTED);
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
	return dl_error_reply(dl, q, mp, DL_SUBS_UNBIND_REQ, DL_NOTSUPPORTED);
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
	return dl_error_reply(dl, q, mp, DL_ENABMULTI_REQ, DL_NOTSUPPORTED);
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
	return dl_error_reply(dl, q, mp, DL_ENABMULTI_REQ, DL_NOTSUPPORTED);
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
	return dl_error_reply(dl, q, mp, DL_PROMISCON_REQ, DL_NOTSUPPORTED);
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
	return dl_error_reply(dl, q, mp, DL_PROMISCOFF_REQ, DL_NOTSUPPORTED);
}

/**
 * dl_data_req: - process received M_DATA primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the M_DATA message
 */
static fastcall noinline __unlikely int
dl_data_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	np_ulong more = !(mp->b_flag & MSGDELIM);
	np_ulong sid = 1;

	/* FIXME: determine sid from X.25 message */
	return np_data_req(np, dl, NULL, more, sid, mp);
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
	dl_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_in *dst = NULL;
	np_ulong sid = 1;

	if (p->dl_dest_addr_length)
		dst = (struct sockaddr_in *) (mp->b_rptr + p->dl_set_addr_offset);
	/* Note: priority is ignored. */
	/* FIXME: determine sid from X.25 message */
	return np_unitdata_req(np, dl, mp, dst, sid, mp->b_cont);
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
	dl_udqos_req_t *p = (typeof(p)) mp->b_rptr;
	dl_qos_cl_sel1_t qos;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (p->dl_qos_length) {
		qos = (typeof(qos)) (mp->b_rptr + p->dl_qos_offset);
		if (p->dl_qos_length < sizeof(qos.dl_qos_type))
			goto badqos;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qos, sizeof(qos.dl_qos_type));
		if (qos.dl_qos_type != DL_QOS_CL_SEL1)
			goto badqostype;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qos, sizeof(qos));
		if (qos->dl_trans_delay != DL_QOS_DONT_CARE)
			dl->udq.dl_trans_delay = qos->dl_trans_delay;
		if (qos->dl_priority != DL_QOS_DONT_CARE)
			dl->udq.dl_priority = qos->dl_priority;
		if (qos->dl_protection != DL_QOS_DONT_CARE)
			dl->udq.dl_protection = qos->dl_protection;
		if (qos->dl_residual_error != DL_QOS_DONT_CARE)
			dl->udq.dl_residual_error = qos->dl_residual_error;
	}
	dl_set_state(dl, DL_UDQOS_PENDING);
	return dl_ok_ack(dl, q, mp, DL_UDQOS_REQ);
      badqostype:
	err = DL_BADQOSTYPE;
	goto error;
      toosmall:
	err = DL_BADPRIM;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_UDQOS_REQ, err);
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
	dl_connect_req_t *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_in *dst;
	size_t dst_len;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (p->dl_dest_addr_length == 0)
		goto noaddr;
	if (p->dl_dest_addr_length % sizeof(*dst) != 0)
		goto badaddr2;
	dst = (typeof(dst)) (mp->b_rptr + p->dl_dest_addr_offset);
	dst_len = p->dl_dest_addr_length;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	if (p->dl_qos_length) {
		dl_qos_co_range1_t qos;

		if (p->dl_qos_length < sizeof(qos.dl_qos_type))
			goto badqos;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qos, sizeof(qos.dl_qos_type));
		if (qos.dl_qos_type != DL_QOS_CO_RANGE1)
			goto badqostype;
		if (p->dl_qos_length < sizeof(qos))
			goto badqos;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qos, sizeof(qos));
		if (qos->dl_rcv_throughput.dl_target_value != DL_QOS_DONT_CARE)
			dl->proto.qor.dl_rcv_throughput.dl_target_value =
			    qos->dl_rcv_throughput.dl_target_value;
		if (qos->dl_rcv_throughput.dl_accept_value != DL_QOS_DONT_CARE)
			dl->proto.qor.dl_rcv_throughput.dl_accept_value =
			    qos->dl_rcv_throughput.dl_accept_value;
		if (qos->dl_rcv_trans_delay.dl_target_value != DL_QOS_DONT_CARE)
			dl->proto.qor.dl_rcv_trans_delay.dl_target_value =
			    qos->dl_rcv_trans_delay.dl_target_value;
		if (qos->dl_rcv_trans_delay.dl_accept_value != DL_QOS_DONT_CARE)
			dl->proto.qor.dl_rcv_trans_delay.dl_accept_value =
			    qos->dl_rcv_trans_delay.dl_accept_value;
		if (qos->dl_xmt_throughput.dl_target_value != DL_QOS_DONT_CARE)
			dl->proto.qor.dl_xmt_throughput.dl_target_value =
			    qos->dl_xmt_throughput.dl_target_value;
		if (qos->dl_xmt_throughput.dl_accept_value != DL_QOS_DONT_CARE)
			dl->proto.qor.dl_xmt_throughput.dl_accept_value =
			    qos->dl_xmt_throughput.dl_accept_value;
		if (qos->dl_xmt_trans_delay.dl_target_value != DL_QOS_DONT_CARE)
			dl->proto.qor.dl_xmt_trans_delay.dl_target_value =
			    qos->dl_xmt_trans_delay.dl_target_value;
		if (qos->dl_xmt_trans_delay.dl_accept_value != DL_QOS_DONT_CARE)
			dl->proto.qor.dl_xmt_trans_delay.dl_accept_value =
			    qos->dl_xmt_trans_delay.dl_accept_value;
		if (qos->dl_priority.dl_min != DL_QOS_DONT_CARE)
			dl->proto.qor.dl_priority.dl_min = qos->dl_priority.dl_min;
		if (qos->dl_priority.dl_max != DL_QOS_DONT_CARE)
			dl->proto.qor.dl_priority.dl_max = qos->dl_priority.dl_max;
		if (qos->dl_protection.dl_min != DL_QOS_DONT_CARE)
			dl->proto.qor.dl_protection.dl_min = qos->dl_protection.dl_min;
		if (qos->dl_protection.dl_max != DL_QOS_DONT_CARE)
			dl->proto.qor.dl_protection.dl_max = qos->dl_protection.dl_max;
		if (qos->dl_resilience.dl_disc_prob != DL_QOS_DONT_CARE)
			dl->proto.qor.dl_resilience.dl_disc_prob = qos->dl_resilience.dl_disc_prob;
		if (qos->dl_resilience.dl_reset_prob != DL_QOS_DONT_CARE)
			dl->proto.qor.dl_resilience.dl_reset_prob =
			    qos->dl_resilience.dl_reset_prob;
	}
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_state(dl, DL_OUTCON_PENDING);
	return np_conn_req(dl->np, q, mp, dst, dst_len, EX_DATA_OPT, 4096, 4096, mp->b_cont);
      badqostype:
	err = DL_BADQOSTYPE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      badaddr2:
	err = DL_BADADDR;
	goto error;
      noaddr:
	err = DL_NOADDR;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      toosmall:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_CONNECT_REQ, err);
}

static struct dl *
dl_lookup(struct dl *dl, dl_ulong token)
{
	struct dl *dl = NULL;
	caddr_t i;
	pl_t pl;

	if (token == 0)
		return (dl_get(dl));
	pl = RW_RDLOCK(&xos_rwlock, plstr);
	for (i = xos_opens; i; i = mi_next_ptr(i))
		if (((struct priv *) i)->dlpi.token == token)
			break;
	if (i) {
		priv_get((struct priv *) i);
		dl = &((struct priv *) i)->dlpi;
	}
	RW_UNLOCK(&xos_rwlock, pl);
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
	dl_qos_co_range1_t *qor = &dl->proto.qor;
	dl_qos_co_sel1_t *qos = &dl->proto.qos;
	struct dl *al;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqosparam;
	if (dl_get_state(dl) != DL_INCON_PENDING)
		goto outstate;
	if (p->dl_qos_length >= sizeof(qos->dl_qos_type)) {
		bcopy(mp->b_rptr + p->dl_qos_offset, qos, sizeof(dl_ulong));
		if (qos->dl_qos_type != DL_QOS_CO_SEL1)
			goto badqostype;
		if (p->dl_qos_length < sizeof(*qos))
			goto badqosparam;
		bcopy(mp->b_rptr + p->dl_qos_offset, qos, sizeof(*qos));
	} else {
		qos->dl_qos_type = DL_QOS_CO_SEL1;
		qos->dl_rcv_throughput = DL_QOS_DONT_CARE;
		qos->dl_rcv_trans_delay = DL_QOS_DONT_CARE;
		qos->dl_xmt_throughput = DL_QOS_DONT_CARE;
		qos->dl_xmt_trans_delay = DL_QOS_DONT_CARE;
		qos->dl_priority = DL_QOS_DONT_CARE;
		qos->dl_protection = DL_QOS_DONT_CARE;
		qos->dl_residual_error = DL_QOS_DONT_CARE;
		qos->dl_resilience.dl_disc_prob = DL_QOS_DONT_CARE;
		qos->dl_resilience.dl_reset_prob = DL_QOS_DONT_CARE;
	}
	if (qos->dl_rcv_throughput == DL_QOS_DONT_CARE)
		qos->dl_rcv_throughput = qor->dl_rcv_throughput.dl_accept_value;
	if (qos->dl_rcv_throughput == DL_UNKNOWN)
		qos->dl_rcv_throughput = qor->dl_rcv_throughput.dl_target_value;
	if (qos->dl_rcv_trans_delay == DL_QOS_DONT_CARE)
		qos->dl_rcv_trans_delay = qor->dl_rcv_trans_delay.dl_accept_value;
	if (qos->dl_rcv_trans_delay == DL_UNKNOWN)
		qos->dl_rcv_trans_delay = qor->dl_rcv_trans_delay.dl_target_value;
	if (qos->dl_xmt_throughput == DL_QOS_DONT_CARE)
		qos->dl_xmt_throughput = qor->dl_xmt_throughput.dl_accept_value;
	if (qos->dl_xmt_throughput == DL_UNKNOWN)
		qos->dl_xmt_throughput = qor->dl_xmt_throughput.dl_target_value;
	if (qos->dl_xmt_trans_delay == DL_QOS_DONT_CARE)
		qos->dl_xmt_trans_delay = qor->dl_xmt_trans_delay.dl_accept_value;
	if (qos->dl_xmt_trans_delay == DL_UNKNOWN)
		qos->dl_xmt_trans_delay = qor->dl_xmt_trans_delay.dl_target_value;
	if (qos->dl_priority == DL_QOS_DONT_CARE)
		qos->dl_priority = qor->dl_priority.dl_min;
	if (qos->dl_priority == DL_UNKNOWN)
		qos->dl_priority = qor->dl_priority.dl_max;
	if (qos->dl_protection == DL_QOS_DONT_CARE)
		qos->dl_protection = qor->dl_protection.dl_min;
	if (qos->dl_protection == DL_UNKNOWN)
		qos->dl_protection = qor->dl_protection.dl_max;
	if (qos->dl_residual_error == DL_QOS_DONT_CARE)
		qos->dl_residual_error = qor->dl_residual_error;
	if (qos->dl_residual_error == DL_UNKNOWN)
		qos->dl_residual_error = qor->dl_residual_error;
	if (qos->dl_resilience.dl_disc_prob == DL_QOS_DONT_CARE)
		qos->dl_resilience.dl_disc_prob = qor->dl_resilience.dl_disc_prob;
	if (qos->dl_resilience.dl_disc_prob == DL_UNKNOWN)
		qos->dl_resilience.dl_disc_prob = qor->dl_resilience.dl_disc_prob;
	if (qos->dl_resilience.dl_reset_prob == DL_QOS_DONT_CARE)
		qos->dl_resilience.dl_reset_prob = qor->dl_resilience.dl_reset_prob;
	if (qos->dl_resilience.dl_reset_prob == DL_UNKNOWN)
		qos->dl_resilience.dl_reset_prob = qor->dl_resilience.dl_reset_prob;
	/* Otherwise we trust the correlation value because we use TPI's
	   connection identifier for this purpose.  If the value is bad, TPI
	   will tell us later. */
	if (p->dl_correlation == 0)
		goto badcor;
	/* We use our own token values because, although we could just use
	   TPI's, that means that a DLPI user could erroneously accept on a TPI 
	   stream for which no XOS module has been pushed. */
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
	dl_disconnect_req_t *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_in *res = NULL;
	size_t res_len = 0;
	np_ulong seq = 0, reason = N_UNDEFINED;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	/* FIXME: Translate DLPI reason to NPI SCTP reason. */
	switch (dl->dl_reason) {
	case DL_CONREJ_DEST_UNKNOWN:
		break;
	case DL_CONREJ_DEST_UNREACH_PERMANENT:
		break;
	case DL_CONREJ_DEST_UNREACH_TRANSIENT:
		break;
	case DL_CONREJ_QOS_UNAVAIL_PERMANENT:
		break;
	case DL_CONREJ_QOS_UNAVAIL_TRANSIENT:
		break;
	case DL_CONREJ_PERMANENT_COND:
		break;
	case DL_CONREJ_TRANSIENT_COND:
		break;
	case DL_DISC_ABNORMAL_CONDITION:
		break;
	case DL_DISC_NORMAL_CONDITION:
		break;
	case DL_DISC_PERMANENT_CONDITION:
		break;
	case DL_DISC_TRANSIENT_CONDITION:
		break;
	case DL_DISC_UNSPECIFIED:
		break;
	}
	switch (dl_get_state(dl)) {
	case DL_IDLE:
		goto discard;
	case DL_OUTCON_PENDING:
		dl_set_state(dl, DL_DISCON8_PENDING);
		break;
	case DL_INCON_PENDING:
		seq = p->dl_correlation;
		dl_set_state(dl, DL_DISCON9_PENDING);
		break;
	case DL_DATAXFER:
		dl_set_state(dl, DL_DISCON11_PENDING);
		break;
	case DL_PROV_RESET_PENDING:
		dl_set_state(dl, DL_DISCON12_PENDING);
		break;
	case DL_USER_RESET_PENDING:
		dl_set_State(dl, DL_DISCON13_PENDING);
		break;
	default:
		goto outstate;
	}
	return np_discon_req(dl->np, q, mp, res, res_len, reason, seq, mp->b_cont);
      oustate:
	err = DL_OUTSTATE;
	goto error;
      tooshort:
	err = DL_BADPRIM;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_DISCONNECT_REQ, err);
      discard:
	freemsg(mp);
	return (0);

}

/**
 * dl_reset_req: - process received DL_RESET_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_RESET_REQ message
 * 
 * We don't really have reset capability with SCTP, so just disconnect.  This
 * entails sending a T_DISCON_REQ and issuing a DL_DISCONNECT_IND up to the
 * DLS-user.
 */
static fastcall noinline __unlikely int
dl_reset_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_reset_req_t *p = (typeof(p)) mp->b_rptr;
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
	return dl_error_reply(dl, q, mp, DL_DATA_ACK_REQ, DL_UNSUPPORTED);
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
	return dl_error_reply(dl, q, mp, DL_REPLY_REQ, DL_UNSUPPORTED);
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
	return dl_error_reply(dl, q, mp, DL_REPLY_UPDATE_REQ, DL_UNSUPPORTED);
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
	dl_xid_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
	case DL_DATAXFER:
		break;
	default:
		goto outstate;
	}
	if (dl->xidflags & DL_AUTO_XID)
		goto xidauto;
	goto notsupported;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      xidauto:
	err = DL_XIDAUTO;
	goto error;
      oustate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      toosmall:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_XID_REQ, err);
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
	dl_xid_res_t *p = (typeof(*p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
	case DL_DATAXFER:
		break;
	default:
		goto outstate;
	}
	if (dl->xidflags & DL_AUTO_XID)
		goto xidauto;
	goto notsupported;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      xidauto:
	err = DL_XIDAUTO;
	goto error;
      oustate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      toosmall:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_XID_RES, err);
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
	dl_test_req_t *p = (typeof(*p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
	case DL_DATAXFER:
		break;
	default:
		goto outstate;
	}
	if (dl->xidflags & DL_AUTO_TEST)
		goto testauto;
	goto notsupported;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      testauto:
	err = DL_TESTAUTO;
	goto error;
      oustate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      toosmall:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_TEST_REQ, err);
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
	dl_test_res_t *p = (typeof(*p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto toosmall;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
	case DL_DATAXFER:
		break;
	default:
		goto outstate;
	}
	if (dl->xidflags & DL_AUTO_TEST)
		goto testauto;
	goto notsupported;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      testauto:
	err = DL_TESTAUTO;
	goto error;
      oustate:
	err = DL_OUTSTATE;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      toosmall:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_TEST_RES, err);
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
	return dl_error_reply(dl, q, mp, DL_PHYS_ADDR_REQ, DL_NOTSUPPORTED);
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
	return dl_error_reply(dl, q, mp, DL_SET_PHYS_ADDR_REQ, DL_NOTSUPPORTED);
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
	/* FIXME: what statistics structure? */
	return dl_get_statistics_ack(dl, q, mp, NULL, 0);
}

/*
 * --------------------------------------------------------------------------
 *
 * TPI PROVIDER TO TPI USER RECEIVED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 * These are primitives received from the SCTP TPI driver beneath this module.
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
	dl_ulong seq;

#ifdef XOS_SAFETY
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
	seq = p->SEQ_number;
	/* FIXME: process options */
	tp_set_state(tp, TS_WRES_CIND);
	return dl_connect_ind(tp->dl, q, mp, seq, cld, clg, NULL, mp->b_cont);
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

#ifdef XOS_SAFETY
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

#ifdef XOS_SAFETY
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

#ifdef XOS_SAFETY
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

#ifdef XOS_SAFETY
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

#ifdef XOS_SAFETY
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
			  tp_primname(p->CORRECT_prim));
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

/**
 * tp_other_ind: - process received other primitive
 * @tp: transport provider private structure
 * @q: active queue (lower read queue)
 * @mp: the other primitive
 */
static fastcall noinline __unlikely int
tp_other_ind(struct tp *tp, queue_t *q, mblk_t *mp)
{
}

/*
 * --------------------------------------------------------------------------
 *
 * NPI PROVIDER TO NPI USER RECEIVED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 * These are primitives received from the SCTP NPI driver beneath this module.
 */

/**
 * np_conn_ind:- process received N_CONN_IND primitive
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the N_CONN_IND primitive
 */
static fastcall noinline __unlikely int
np_conn_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_in *dst = NULL, *src = NULL;
	N_qos_sctp_conn_t *qos = NULL;
	dl_ulong seq, flags;

	if (p->DEST_length)
		dst = (struct sockaddr_in *) (mp->b_rptr + p->DEST_offset);
	if (p->SRC_length)
		src = (struct sockaddr_in *) (mp->b_rptr + p->SRC_offset);
	if (p->QOS_length)
		qos = (N_qos_sctp_conn_t *) (mp->b_rptr + p->QOS_offset);
	seq = p->SEQ_number;
	flags = p->CONN_flags;
	/* FIXME: process options */
	np->coninds++;
	np_set_state(np, TS_WRES_CIND);
	return dl_connect_ind(np->dl, q, mp, seq, dst, src, NULL, mp->b_cont);
}

/**
 * np_conn_con:- process received N_CONN_CON primitive
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the N_CONN_CON primitive
 */
static fastcall noinline __unlikely int
np_conn_con(struct np *np, queue_t *q, mblk_t *mp)
{
	N_conn_con_t *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_in *res = NULL;
	N_qos_sctp_conn_t *qos = NULL;
	np_ulong flags;
	int err;

	if (p->RES_length)
		res = (struct sockaddr_in *) (mp->b_rptr + p->RES_offset);
	if (p->QOS_length)
		qos = (N_qos_sctp_conn_t *) (mp->b_rptr + p->QOS_offset);
	flags = p->CONN_flags;
	/* FIXME: process options */
	np_set_state(np, NS_DATA_XFER);
	return dl_connect_con(np->dl, q, mp, res, NULL, mp->b_cont);
}

/**
 * np_discon_ind:- process received N_DISCON_IND primitive
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the N_DISCON_IND primitive
 */
static fastcall noinline __unlikely int
np_discon_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_discon_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_in *res = NULL;
	np_ulong orig, reason, seq;
	int err;

	switch (p->DISCON_orig) {
	case N_USER:
		orig = DL_USER;
		break;
	case N_PROVIDER:
		orig = DL_PROVIDER;
		break;
	case N_UNDEFINED:
	default:
		orig = DL_UNKNOWN;
		break;
	}
	/* FIXME: transform the reason code */
	reason = p->DISCON_reason;
	if (p->RES_length)
		res = (struct sockaddr_in *) (mp->b_rptr + p->RES_offset);
	seq = p->SEQ_number;
	np_set_state(np, NS_IDLE);
	return dl_disconnect_ind(np->dl, q, mp, orig, reason, seq, mp->b_cont);
}

/**
 * np_data_ind:- process received N_DATA_IND primitive
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the N_DATA_IND primitive
 */
static fastcall noinline __unlikely int
np_data_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_data_ind_t *p = (typeof(p)) mp->b_rptr;
	np_ulong more;

	more = ((p->DATA_xfer_flags & N_MORE_DATA_FLAG) != 0);

	return dl_data_ind(np->dl, q, mp, more, mp->b_cont);
}

/**
 * np_exdata_ind:- process received N_EXDATA_IND primitive
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the N_EXDATA_IND primitive
 */
static fastcall noinline __unlikely int
np_exdata_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	return dl_data_ind(np->dl, q, mp, 0, mp->b_cont);
}

/**
 * np_info_ack:- process received N_INFO_ACK primitive
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the N_INFO_ACK primitive
 */
static fastcall noinline __unlikely int
np_info_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	N_info_ack_t *p = (typeof(p)) mp->b_rptr;

	np->proto.info.PRIM_type = N_INFO_ACK;
	np->proto.info.NSDU_size = p->NSDU_size;
	np->proto.info.ENSDU_size = p->ENSDU_size;
	np->proto.info.CDATA_size = p->CDATA_size;
	np->proto.info.DDATA_size = p->DDATA_size;
	np->proto.info.ADDR_size = p->ADDR_size;
	np->proto.info.ADDR_length = p->ADDR_length;
	np->proto.info.ADDR_offset = sizeof(np->proto.info);
	np->proto.info.QOS_length = p->QOS_length;
	np->proto.info.QOS_offset = sizeof(np->proto.info) + sizeof(np->proto.addr);
	np->proto.info.QOS_range_length = p->QOS_range_length;
	np->proto.info.QOS_range_offset =
	    sizeof(np->proto.info) + sizeof(np->proto.addr) + sizeof(np->proto.qos);
	np->proto.info.OPTIONS_flags = p->OPTIONS_flags;
	np->proto.info.NIDU_size = p->NIDU_size;
	np->proto.info.SERV_type = p->SERV_type;
	np->proto.info.CURRENT_state = p->CURRENT_state;
	np->proto.info.PROVIDER_type = p->PROVIDER_type;
	np->proto.info.NODU_size = p->NODU_size;
	np->proto.info.PROTOID_length = p->PROTOID_length;
	np->proto.info.PROTOID_offset = sizeof(np->proto.info) +
	    sizeof(np->proto.addr) + sizeof(np->proto.qos) + sizeof(np->proto.qor);
	np->proto.info.NPI_version = p->NPI_version;
	bcopy(mp->b_rptr + p->ADDR_offset, &np->proto.addr, p->ADDR_length);
	bcopy(mp->b_rptr + p->QOS_offset, &np->proto.qos, p->QOS_length);
	bcopy(mp->b_rptr + p->QOS_range_offset, &np->proto.qor, p->QOS_range_length);
	bcopy(mp->b_rptr + p->PROTOID_offset, &np->proto.pro, p->PROTOID_length);

	/* There are two circumstances for receiving an N_INFO_ACK, one is that 
	   we have requested information from within the module.  The other is
	   that we need information to respond to a DL_INFO_REQ.  In the first
	   case we simply discard the message.  In the latter, we need to
	   formulate the DL_INFO_ACK.  In the end, it appears the athe latter
	   case is not necessary. */
	freemsg(mp);
	return (0);
}

/**
 * np_bind_ack:- process received N_BIND_ACK primitive
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the N_BIND_ACK primitive
 */
static fastcall noinline __unlikely int
np_bind_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	N_bind_ack_t *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_in *add = NULL;
	uint8_t *pro = NULL;
	np_ulong conind;
	dl_ulong flags = (DL_AUTO_XID | DL_AUTO_TEST);
	size_t pro_len;

	if (p->ADDR_length)
		add = (struct sockaddr_in *) (mp->b_rptr + p->ADDR_offset);
	conind = p->CONIND_number;
	np->token = p->TOKEN_value;
	if ((pro_len = p->PROTOID_length))
		pro = (uint8_t *) (mp->b_rptr + p->PROTOID_offset);
	np_set_state(np, NS_IDLE);
	return dl_bind_ack(np->dl, q, mp, add, conind, flags);
}

/**
 * np_error_ack:- process received N_ERROR_ACK primitive
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the N_ERROR_ACK primitive
 */
static fastcall noinline __unlikely int
np_error_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	N_error_ack_t *p = (typeof(p)) mp->b_rptr;
	np_long error;
	np_ulong prim;

	switch (p->ERROR_prim) {
	case N_CONN_REQ:
		prim = DL_CONNECT_REQ;
		break;
	case N_CONN_RES:
		prim = DL_CONNECT_RES;
		break;
	case N_DISCON_REQ:
		prim = DL_DISCONNECT_REQ;
		break;
	case N_INFO_REQ:
		prim = DL_INFO_REQ;
		break;
	case N_BIND_REQ:
		/* FIXME: choose one */
		prim = DL_BIND_REQ;
		prim = DL_SUBS_BIND_REQ;
		break;
	case N_UNBIND_REQ:
		/* FIXME: choose one */
		prim = DL_UNBIND_REQ;
		prim = DL_SUBS_UNBIND_REQ;
		break;
	case N_UNITDATA_REQ:
		prim = DL_UNITDATA_REQ;
		break;
	case N_OPTMGMT_REQ:
		prim = DL_UDQOS_REQ;
		break;
	case N_RESET_REQ:
		prim = DL_RESET_REQ;
		break;
	case N_RESET_RES:
		prim = DL_RESET_RES;
		break;
	default:
		freemsg(mp);
		mi_strlog(np->oq, 0, SL_ERROR, "%s: bad prim %s",
			  __FUNCTION__, np_primname(p->ERROR_prim));
		return (0);
	}
	switch (p->NPI_error) {
	case NBADADDR:
		error = DL_BADADDR;
		break;
	case NBADOPT:
		error = DL_BADQOSPARAM;
		break;
	case NACCESS:
		error = DL_ACCESS;
		break;
	case NNOADDR:
		error = DL_NOADDR;
		break;
	case NOUTSTATE:
		error = DL_OUTSTATE;
		break;
	case NBADSEQ:
		error = DL_BADCORR;
		break;
	case NSYSERR:
		error = -p->UNIX_error;
		break;
	case NBADDATA:
		error = DL_BADDATA;
		break;
	case NBADFLAG:
		error = DL_BADQOSPARAM;
		break;
	case NNOTSUPPORT:
		error = DL_NOTSUPPORTED;
		break;
	case NBOUND:
		error = DL_BOUND;
		break;
	case NBADQOSPARAM:
		error = DL_BADQOSPARAM;
		break;
	case NBADQOSTYPE:
		error = DL_BADQOSTYPE;
		break;
	case NBADTOKEN:
		error = DL_BADTOKEN;
		break;
	case NNOPROTOID:
		error = DL_NOADDR;
		break;
	default:
		error = -EFAULT;
		break;
	}
	switch (np_get_state(tp)) {
	case NS_WACK_BREQ:
		np_set_state(np, NS_UNBND);
		break;
	case NS_WACK_UREQ:
		np_set_state(np, NS_IDLE);
		break;
	case NS_WACK_OPTREQ:
		np_set_state(np, NS_IDLE);
		break;
	case NS_WACK_RRES:
		np_set_state(np, NS_WRES_RIND);
		break;
	case NS_WCON_CREQ:
		np_set_state(np, NS_IDLE);
		break;
	case NS_WACK_CRES:
		np_set_state(np, NS_WRES_CIND);
		break;
	case NS_WCON_RREQ:
		np_set_state(np, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ6:
		np_set_state(np, NS_WCON_CREQ);
		break;
	case NS_WACK_DREQ7:
		np_set_state(np, NS_WRES_CIND);
		break;
	case NS_WACK_DREQ9:
		np_set_state(np, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ10:
		np_set_state(np, NS_WCON_RREQ);
		break;
	case NS_WACK_DREQ11:
		np_set_state(np, NS_WRES_RIND);
		break;
	default:
		break;
	}
	return dl_error_ack(np->dl, q, mp, prim, error);
}

/**
 * np_ok_ack:- process received N_OK_ACK primitive
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the N_OK_ACK primitive
 */
static fastcall noinline __unlikely int
np_ok_ack(struct np *np, queue_t *q, mblk_t *mp)
{
	N_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	dl_ulong prim;

	switch (p->CORRECT_prim) {
	case N_CONN_RES:
		prim = DL_CONNECT_RES;
		break;
	case N_DISCON_REQ:
		prim = DL_DISCONNECT_REQ;
		break;
	case N_UNBIND_REQ:
		/* FIXME: choose one */
		prim = DL_BIND_REQ;
		prim = DL_SUBS_BIND_REQ;
		break;
	case N_OPTMGMT_REQ:
		prim = DL_UDQOS_REQ;
		break;
	case N_RESET_REQ:
		prim = DL_RESET_REQ;
		break;
	case N_RESET_RES:
		prim = DL_RESET_RES;
		break;
	default:
		freemsg(mp);
		mi_strlog(np->oq, 0, SL_ERROR, "%s: bad prim %s", __FUNCTION__,
			  np_primname(p->CORRECT_prim));
		return (0);
	}
	switch (np_get_state(np)) {
	case NS_WACK_UREQ:
		np_set_state(np, NS_UNBND);
		break;
	case NS_WACK_OPTREQ:
		np_set_state(np, NS_IDLE);
		break;
	case NS_WACK_RRES:
		np_set_state(np, NS_DATA_XFER);
		break;
	case NS_WACK_CRES:
		/* If we accepted on the same stream, the we need to move to
		   the NS_DATA_XFER state instead of the NS_IDLE state.
		   Otherwise it is the accepting stream that moves to
		   NS_DATA_XFER. */
		np->coninds--;
		np_set_state(np, (np->coninds > 0) ? NS_WRES_CIND : NS_IDLE);
		np_set_state(np->ap, NS_DATA_XFER);
		break;
	case NS_WACK_DREQ7:
		np->coninds--;
		/* fall through */
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		np_set_state(np, (np->coninds > 0) ? NS_WRES_CIND : NS_IDLE);
		break;
	}
}

/**
 * np_unitdata_ind:- process received N_UNITDATA_IND primitive
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the N_UNITDATA_IND primitive
 */
static fastcall noinline __unlikely int
np_unitdata_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_in *dst = NULL, *src = NULL;
	dl_ulong group = 0;

	if (p->DEST_length)
		dst = (struct sockaddr_in *) (mp->b_rptr + p->DEST_offset);
	if (p->SRC_length)
		src = (struct sockaddr_in *) (mp->b_rptr + p->SRC_offset);

	return dl_unitdata_ind(np->dl, q, mp, dst, src, group, mp->b_cont);
}

/**
 * np_uderror_ind:- process received N_UDERROR_IND primitive
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the N_UDERROR_IND primitive
 */
static fastcall noinline __unlikely int
np_uderror_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_uderror_ind_t *p = (typeof(p)) mp->b_rptr;
	struct sockaddr_in *dst = NULL;
	np_long etype;

	if (p->DEST_length)
		dst = (struct sockaddr_in *) (mp->b_rptr + p->DEST_offset);
	etype = p->ERROR_type;
	/* FIXME: map NPI error type to DLPI error type */
	return dl_uderror_ind(np->dl, q, mp, dst, etype, mp->b_cont);
}

/**
 * np_datack_ind:- process received N_DATACK_IND primitive
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the N_DATACK_IND primitive
 */
static fastcall noinline __unlikely int
np_datack_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * np_reset_ind:- process received N_RESET_IND primitive
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the N_RESET_IND primitive
 */
static fastcall noinline __unlikely int
np_reset_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	N_reset_ind_t *p = (typeof(p)) mp->b_rptr;
	dl_ulong orig;
	dl_ulong reason;

	switch (p->RESET_orig) {
	case N_USER:
		orig = DL_USER;
		break;
	case N_PROVIDER:
		orig = DL_PROVIDER;
		break;
	case N_UNDEFINED:
	default:
		orig = DL_UNKNOWN;
		break;
	}
	/* FIXME: map NPI reset reason to DLPI reset resason */
	reason = p->RESET_reason;
	/* Almost always has to be set this way anyway */
	orig = DL_PROVIDER;
	reason = DL_RESET_RESYNCH;
	np_set_state(np, NS_WRES_RIND);
	return dl_reset_ind(np->dl, q, mp, orig, reason);
}

/**
 * np_reset_con:- process received N_RESET_CON primitive
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the N_RESET_CON primitive
 */
static fastcall noinline __unlikely int
np_reset_con(struct np *np, queue_t *q, mblk_t *mp)
{
	return dl_reset_con(np->dl, q, mp);
}

/**
 * np_other_ind:- process received other primitive
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the other primitive
 */
static fastcall noinline __unlikely int
np_other_ind(struct np *np, queue_t *q, mblk_t *mp)
{
	np_ulong prim = *(np_ulong *) mp->b_rptr;

	freemsg(mp);
	mi_strlog(q, 0, SL_ERROR, "%s: bad prim %s", __FUNCTION__, np_primname(prim));
	return (0);
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
 * @tp: tranport provider private structure
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
 * @tp: tranport provider private structure
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
	default:
		rtn = tp_other_ind(tp, q, mp);
		break;
	}
      done:
	if (rtn)
		tp_restore_state(tp);
	return (rtn);
}

/**
 * tp_m_sig: - process received M_SIG or M_PCSIG message
 * @tp: tranport provider private structure
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
 * @tp: tranport provider private structure
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
 * @tp: tranport provider private structure
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
 * @tp: tranport provider private structure
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
 * @tp: tranport provider private structure
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
 * @tp: tranport provider private structure
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
 * @tp: tranport provider private structure
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
 * @tp: tranport provider private structure
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
 * @tp: tranport provider private structure
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
 * @tp: tranport provider private structure
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

/**
 * np_m_data: - process received M_DATA or M_HPDATA message
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the M_DATA or M_HPDATA message
 *
 * Don't need to process the data, just pass it along.  Which means that we could just push tirdwr
 * module and let the data pass through.
 */
static fastcall noinline __unlikely int
np_m_data(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * np_m_proto: - process received M_PROTO or M_PCPROTO message
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
static fastcall noinline __unlikely int
np_m_proto(queue_t *q, mblk_t *mp)
{
	struct np *np;
	np_ulong prim;
	int rtn = -EFAULT;

	np = NP_PRIV(q);	/* already locked */
	np_save_state(np);
	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	prim = *(np_ulong *) mp->b_rptr;
	/* fast paths for data */
	if (likely(prim = N_DATA_IND)) {
		rtn = np_data_ind(np, q, mp);
		goto done;
	}
	if (likely(prim = N_EXDATA_IND)) {
		rtn = np_exdata_ind(np, q, mp);
		goto done;
	}
	switch (prim) {
	case N_CONN_IND:
		rtn = np_conn_ind(np, q, mp);
		break;
	case N_CONN_CON:
		rtn = np_conn_con(np, q, mp);
		break;
	case N_DISCON_IND:
		rtn = np_discon_ind(np, q, mp);
		break;
	case N_DATA_IND:
		rtn = np_data_ind(np, q, mp);
		break;
	case N_EXDATA_IND:
		rtn = np_exdata_ind(np, q, mp);
		break;
	case N_INFO_ACK:
		rtn = np_info_ack(np, q, mp);
		break;
	case N_BIND_ACK:
		rtn = np_bind_ack(np, q, mp);
		break;
	case N_ERROR_ACK:
		rtn = np_error_ack(np, q, mp);
		break;
	case N_OK_ACK:
		rtn = np_ok_ack(np, q, mp);
		break;
	case N_UNITDATA_IND:
		rtn = np_unitdata_ind(np, q, mp);
		break;
	case N_UDERROR_IND:
		rtn = np_uderror_ind(np, q, mp);
		break;
	case N_DATACK_IND:
		rtn = np_datack_ind(np, q, mp);
		break;
	case N_RESET_IND:
		rtn = np_reset_ind(np, q, mp);
		break;
	case N_RESET_CON:
		rtn = np_reset_con(np, q, mp);
		break;
	default:
		rtn = np_other_ind(np, q, mp);
	}
      done:
	if (rtn)
		np_restore_state(np);
	return (rtn);
}

/**
 * np_m_sig: - process received M_SIG or M_PCSIG message
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the M_SIG or M_PCSIG message
 *
 * If these are short messages they are true M_SIG or M_PCSIG signals heading for the Stream head.
 * Just pass them along with flow control.
 */
static fastcall noinline __unlikely int
np_m_sig(queue_t *q, mblk_t *mp)
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
 * np_m_ctl: - process received M_CTL or M_PCCTL message
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the M_CTL or M_PCCTL message
 *
 * We always trust the lower interface, so treat them the same as M_PROTO or M_PCPROTO.
 */
static fastcall noinline __unlikely int
np_m_ctl(queue_t *q, mblk_t *mp)
{
	return tp_m_proto(q, mp);
}

/**
 * np_m_rse: - process received M_RSE or M_PCRSE message
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the M_RSE or M_PCRSE message
 *
 * Ugly, don't pass these around.
 */
static fastcall noinline __unlikely int
np_m_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * np_m_flush: - process received M_FLUSH message
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the M_FLUSH message
 *
 * Canonical flush handling for a STREAMS module
 */
static fastcall noinline __unlikely int
np_m_flush(queue_t *q, mblk_t *mp)
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
 * np_m_error: - process received M_ERROR message
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the M_ERROR message
 *
 * Whoah.  Bad.  The stream is errored out.  Simply pass it directly to the Stream head so that the
 * entire Stream can error out.  Flush our queues as well and go to a bad state.
 */
static fastcall noinline __unlikely int
np_m_error(queue_t *q, mblk_t *mp)
{
	tp_set_state(tp, -1);
	flushq(q, FLUSHALL);
	flushq(WR(q), FLUSHALL);
	putnext(q, mp);
	return (0);
}

/**
 * np_m_hangup: - process received M_HANGUP message
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the M_HANGUP message
 *
 * Hmmm.  Just pass it along directly.  It only affects the Stream head and there might be an
 * unhangup later.
 */
static fastcall noinline __unlikely int
np_m_hangup(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/**
 * np_m_setopts: - process received M_SETOPTS or M_PCSETOPTS message
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the M_SETOPTS or M_PCSETOPTS message
 *
 * We are really just converting between interfaces, so any write offsets or other stream head
 * thingies that need to be done are ok.  Just pass them along.
 */
static fastcall noinline __unlikely int
np_m_setopts(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/**
 * np_m_copy: - process received M_COPYIN or M_COPYOUT message
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the M_COPYIN or M_COPYOUT message
 *
 * We don't have any input output controls of our own, so pass along.
 */
static fastcall noinline __unlikely int
np_m_copy(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/**
 * np_m_iocack: - process received M_IOCACK or M_IOCNAK message
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the M_IOCACK or M_IOCNAK message
 *
 * We don't have any input output controls of our own, so pass along.
 */
static fastcall noinline __unlikely int
np_m_iocack(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

/**
 * np_m_other: - process received other message
 * @np: network provider private structure
 * @q: active queue (lower read queue)
 * @mp: the other message
 *
 * Modules should pass unrecognized messages along.
 */
static fastcall noinline __unlikely int
np_m_other(queue_t *q, mblk_t *mp)
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

static fastcall noinline int
np_msg_srv(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_PROTO))
		return np_m_proto(q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return np_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return np_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return np_m_sig(q, mp);
	case M_CTL:
	case M_PCCTL:
		return np_m_ctl(q, mp);
	case M_RSE:
	case M_PCRSE:
		return np_m_rse(q, mp);
	case M_FLUSH:
		return np_m_flush(q, mp);
	case M_ERROR:
		return np_m_error(q, mp);
	case M_HANGUP:
	case M_UNHANGUP:
		return np_m_hangup(q, mp);
	case M_SETOPTS:
	case M_PCSETOPTS:
		return np_m_setopts(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return np_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return np_m_iocack(q, mp);
	default:
		return np_m_other(q, mp);
	}
}
static fastcall inline int
np_msg_put(queue_t *q, mblk_t *mp)
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
		return np_m_flush(q, mp);
	case M_ERROR:
		return np_m_error(q, mp);
	case M_HANGUP:
	case M_UNHANGUP:
		return np_m_hangup(q, mp);
	case M_SETOPTS:
	case M_PCSETOPTS:
		return np_m_setopts(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return np_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return np_m_iocack(q, mp);
	default:
		return np_m_other(q, mp);
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

static streamscall int
np_rsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (np_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}
static streamscall int
np_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flags & QSVCBUSY))) || np_msg_put(q, mp))
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

caddr_t xos_opens = NULL;
rwlock_t xos_rwlock = RW_LOCK_UNLOCKED;

static int
xos_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	struct dl *dl;
	struct tp *tp;
	struct np *np;
	mblk_t *mp;
	int err;
	pl_t pl;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (!(mp = allocb(sizeof(struct T_capability_req), BPRI_WAITOK)))
		return (ENOBUFS);
	pl = RW_WRLOCK(&xos_rwlock, plstr);
	if ((err = mi_open_comm(&xos_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		RW_UNLOCK(&xos_rwlock, pl);
		freeb(mp);
		return (err);
	}
	RW_UNLOCK(&xos_rwlock, pl);

	p = PRIV(q);
	ATOMIC_INT_INIT(&p->refs, 1);
	dl = &p->dlpi;
	tp = &p->tpi;
	np = &p->npi;

	/* initialize private structure */
	dl->priv = p;
	dl->tp = tp;
	dl->np = np;
	dl->oq = q;
	dl->state = DL_UNBOUND;
	dl->oldstate = DL_UNBOUND;

	tp->priv = p;
	tp->dl = dl;
	tp->oq = WR(q);
	tp->state = TS_UNBND;
	tp->oldstate = TS_UNBND;

	np->priv = p;
	np->dl = dl;
	np->oq = WR(q);
	np->state = NS_UNBND;
	np->oldstate = NS_UNBND;

	/* FIXME: more initialization */

	DB_TYPE(mp) = M_PCPROTO;
	((struct T_capability_req *) mp->b_wptr)->PRIM_type = T_CAPABILITY_REQ;
	mp->b_wptr += sizeof(struct T_capability_req);

	qprocson(q);
	putnext(q, mp);
	return (0);
}

/**
 * xos_qclose: - STREAMS close procedure
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
xos_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	if (p == NULL)
		return (0);
	qprocsoff(q);
	/* FIXME: free timers and other things. */
	{
		pl_t pl = RW_WRLOCK(&xos_rwlock, plstr);

		/* detach private structure from queue pair */
		mi_detach(q, (caddr_t) p);
		/* remove structure from open list */
		mi_close_unlink(&xos_opens, (caddr_t) p);
		RW_UNLOCK(&xos_rwlock, pl);
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

static struct qinit xos_rinit = {
	.qi_putp = &tp_wput,
	.qi_srvp = &tp_wsrv,
	.qi_qopen = &xos_qopen,
	.qi_qclose = &xos_qclose,
	.qi_minfo = &xos_minfo,
	.qi_mstat = &xos_rstat,
};

static struct qinit xos_winit = {
	.qi_putp = &dl_wput,
	.qi_srvp = &dl_wsrv,
	.qi_minfo = &xos_minfo,
	.qi_mstat = &xos_wstat,
};

struct streamtab xos_info = {
	.st_rdinit = &xos_rinit,
	.st_wrinit = &xos_winit,
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
MODULE_PARM_DESC(modid, "Module ID for XOS.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static __init int
xos_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&xos_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
xos_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&xos_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(xos_modinit);
module_exit(xos_modexit);

#endif				/* LINUX */
