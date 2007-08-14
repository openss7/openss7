/*****************************************************************************

 @(#) $RCSfile: tc_mod.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/08/14 12:18:55 $

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

 Last Modified $Date: 2007/08/14 12:18:55 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tc_mod.c,v $
 Revision 0.9.2.2  2007/08/14 12:18:55  brian
 - GPLv3 header updates

 Revision 0.9.2.1  2007/08/03 13:02:54  brian
 - added documentation and minimal modules

 *****************************************************************************/

#ident "@(#) $RCSfile: tc_mod.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/08/14 12:18:55 $"

static char const ident[] =
    "$RCSfile: tc_mod.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/08/14 12:18:55 $";

/*
 * This is TC-MOD.  It is a simplified Transaction Component Interface (TCI) module for TCAP that
 * can be pushed as a pushable STREAMS module over a TCAP TR Stream.  Either the TCAP TR Stream can
 * be bound before pushing or after.  This is the simple case where all transactions are delivered
 * to the same Stream.  When the TC User is bound to an application context, only transactions for
 * that application context will be delivered to the TC User.  Dialogues for Application contexts
 * other than the bound context will rejected.  When the TC User is wildcard bound, all transactions
 * on the TR Stream will be delivered to the TC User.
 *
 * In the more complex cases, where a TC-MUX is used, a TR Provider Stream is linked under the
 * TC-MUX and then TC User Streams are opened and bound to different Application Contexts.  This
 * allows different Application Contexts for the same SCCP Subsystem to be delivered to the
 * different TC User Streams.
 *
 * This TC-MOD module presents a Transaction Component Interface (TCI) to its user.  To present a
 * Transaction Interface (TRI) instead, do not push the TC-MOD module and use the TR-MOD module
 * directly instead.
 *
 * Usage:
 *
 *  - Open an SCCP Stream.
 *  - Push the TR-MOD module.
 *  - Push the TC-MOD module.
 *  - Bind the Stream.
 *  - Exchange transactions and components.
 *  - Unbind the Stream.
 *  - Pop the TC-MOD module.
 *  - Pop the TR-MOD module.
 *  - Close the SCCP Stream.
 */

#define _LFS_SOURCE 1
#define _MPS_SOURCE 1
#define _SVR4_SOURCE 1
#define _SUN_SOURCE 1

#include <sys/os7/compat.h>
#include <linux/socket.h>

#include <sys/npi.h>
#include <sys/npi_sccp.h>
#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>
#include <ss7/sccpi.h>
#include <ss7/sccpi_ioctl.h>
#include <sys/tihdr.h>
#include <sys/tpi.h>
#include <sys/tpi_tcap.h>
#include <ss7/tr.h>
#include <ss7/tc.h>
#include <ss7/tcap.h>
#include <ss7/tcap_ioctl.h>
#include <sys/xti.h>
#include <sys/xti_mtp.h>
#include <sys/xti_sccp.h>
#include <sys/xti_tcap.h>

#define TC_DESCRIP	"SS7/TCAP-TC (TCAP Component Handling) STREAMS MODULE."
#define TC_REVISION	"OpenSS7 $RCSfile: tc_mod.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/08/14 12:18:55 $"
#define TC_COPYRIGHT	"Copyright (c) 1997-2007 OpenSS7 Corporation.  All Rights Reserved."
#define TC_DEVICE	"Provides OpenSS7 TCAP-TC module."
#define TC_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TC_LICENSE	"GPL v2"
#define TC_BANNER	TC_DESCRIP	"\n" \
			TC_REVISION	"\n" \
			TC_COPYRIGHT	"\n" \
			TC_DEVICE	"\n" \
			TC_CONTACT	"\n"
#define TC_SPLASH	TC_DEVICE	" - " \
			TC_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(TC_CONTACT);
MODULE_DESCRIPTION(TC_DESCRIP);
MODULE_SUPPORTED_DEVICE(TC_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TC_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-tc-mod");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define TC_MOD_MOD_ID		CONFIG_STREAMS_TC_MOD_MODID
#define TC_MOD_MOD_NAME		CONFIG_STREAMS_TC_MOD_NAME
#endif				/* LFS */

#ifndef TC_MOD_MOD_NAME
#define TC_MOD_MOD_NAME		"tc-mod"
#endif				/* TC_MOD_MOD_NAME */

#ifndef TC_MOD_MOD_ID
#define TC_MOD_MOD_ID		0
#endif				/* TC_MOD_MOD_ID */

/*
 *  =========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  =========================================================================
 */

#define MOD_ID		TC_MOD_MOD_ID
#define MOD_NAME	TC_MOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	TC_BANNER
#else				/* MODULE */
#define MOD_BANNER	TC_SPLASH
#endif				/* MODULE */

static struct module_info tc_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat tc_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat tc_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  =========================================================================
 *
 *  OPTIONS
 *
 *  =========================================================================
 */

struct tc_var {
	size_t len;
	uchar *ptr;
};

struct tc_opts {
	uint flags;			/* success flags */
	t_uscalar_t *ver;		/* TCAP version */
	struct tc_var ctx;		/* TCAP application context */
	struct tc_var inf;		/* TCAP user information */
	struct tc_var sec;		/* TCAP security context */
	t_uscalar_t *pcl;		/* SCCP protocol class */
	t_uscalar_t *ret;		/* SCCP return option */
	t_uscalar_t *imp;		/* SCCP importance */
	t_uscalar_t *seq;		/* SCCP sequence control */
	t_uscalar_t *pri;		/* SCCP priority */
	t_uscalar_t *sls;		/* MTP signalling link selection */
	t_uscalar_t *mp;		/* MTP message priority */
};

#define TCAP_BUF_MAXLEN 256

struct tc_val {
	uint len;
	uchar buf[TCAP_BUF_MAXLEN];
};

struct tc_opts_vals {
	uint flags;			/* success flags */
	t_uscalar_t ver;		/* TCAP version */
	struct tc_val ctx;		/* TCAP application context */
	struct tc_val inf;		/* TCAP user information */
	struct tc_val sec;		/* TCAP security context */
	t_uscalar_t pcl;		/* SCCP protocol class */
	t_uscalar_t ret;		/* SCCP return option */
	t_uscalar_t imp;		/* SCCP importance */
	t_uscalar_t seq;		/* SCCP sequence control */
	t_uscalar_t pri;		/* SCCP priority */
	t_uscalar_t sls;		/* MTP signalling link selection */
	t_uscalar_t mp;			/* MTP message priority */
} opt_defaults;

enum {
	T_OPT_VER = 0, T_OPT_APP, T_OPT_INF, T_OPT_SEC, T_OPT_PCL,
	T_OPT_RET, T_OPT_IMP, T_OPT_SEQ, T_OPT_PRI, T_OPT_SLS, T_OPT_MP,
};

#define TF_OPT_VER	(1<<T_OPT_VER)	/* version */
#define TF_OPT_APP	(1<<T_OPT_APP)	/* application context */
#define TF_OPT_INF	(1<<T_OPT_INF)	/* use information */
#define TF_OPT_SEC	(1<<T_OPT_SEC)	/* security context */
#define TF_OPT_PCL	(1<<T_OPT_PCL)	/* protocol class */
#define TF_OPT_RET	(1<<T_OPT_RET)	/* return option */
#define TF_OPT_IMP	(1<<T_OPT_IMP)	/* importance */
#define TF_OPT_SEQ	(1<<T_OPT_SEQ)	/* sequence control */
#define TF_OPT_PRI	(1<<T_OPT_PRI)	/* priority */
#define TF_OPT_SLS	(1<<T_OPT_SLS)	/* signalling link selection */
#define TF_OPT_MP	(1<<T_OPT_MP)	/* message priority */

/*
 *  =========================================================================
 *
 *  PRIVATE STRUCTURE
 *
 *  =========================================================================
 */

struct priv;
struct tc;
struct tr;
struct xa;

struct xa {
	struct tr *tr;			/* TRI stream */
	struct tc *tc;			/* TCI stream */
	uint flags;
	uint state;
	uint oldstate;
	struct {
		t_uscalar_t id;		/* transaction id */
		struct sccp_addr add;	/* address */
	} orig, dest;
};

struct tc {
	struct priv *priv;
	struct tr *tr;
	queue_t *oq;
	t_uscalar_t state;
	t_uscalar_t oldstate;
	struct tc_opts_vals options;
	uint xacts;
	uint token;
	struct tcap_opt_conf_tc config;
	struct tcap_statem_tc statem;
	struct tcap_stats_tc statsp;
	struct tcap_stats_tc stats;
	struct tcap_notify_tc notify;
};

struct tr {
	struct priv *priv;
	struct tc *tc;
	queue_t *oq;
	t_uscalar_t state;
	t_uscalar_t oldstate;
	struct sccp_addr loc;
	struct sccp_addr rem;
	struct TR_info_ack info;
	uint xacts;
	uint token;
	struct {
		struct tcap_opt_conf_sp config;
		struct tcap_statem_sp statem;
		struct tcap_stats_sp statsp;
		struct tcap_stats_sp stats;
		struct tcap_notify_sp notify;
	} sp;
};

struct {
	struct tcap_opt_conf_df config;
	struct tcap_statem_df statem;
	struct tcap_stats_df statsp;
	struct tcap_stats_df stats;
	struct tcap_notify_df notify;
} df;

struct priv {
	struct tr r_priv;		/* TCAP TR structure on RD side */
	struct tc w_priv;		/* TCAP TC structure on WR side */
};

#define PRIV(q)	    ((struct priv *)(q)->q_ptr)

#define TC_PRIV(q)  (&PRIV(q)->w_priv)
#define TR_PRIV(q)  (&PRIV(q)->r_priv)

#define STRLOGIO	0	/* log Stream input-output controls */
#define STRLOGST	1	/* log Stream state transitions */
#define STRLOGTO	2	/* log Stream timeouts */
#define STRLOGRX	3	/* log Stream primitives received */
#define STRLOGTX	4	/* log Stream primitives issued */
#define STRLOGTE	5	/* log Stream timer events */
#define STRLOGDA	6	/* log Stream data */

static inline const char *
tc_iocname(int cmd)
{
	switch (cmd) {
	case TCAP_IOCGOPTION:
		return ("TCAP_IOCGOPTION");
	case TCAP_IOCSOPTION:
		return ("TCAP_IOCSOPTION");
	case TCAP_IOCGCONFIG:
		return ("TCAP_IOCGCONFIG");
	case TCAP_IOCSCONFIG:
		return ("TCAP_IOCSCONFIG");
	case TCAP_IOCTCONFIG:
		return ("TCAP_IOCTCONFIG");
	case TCAP_IOCCCONFIG:
		return ("TCAP_IOCCCONFIG");
	case TCAP_IOCGSTATEM:
		return ("TCAP_IOCGSTATEM");
	case TCAP_IOCCMRESET:
		return ("TCAP_IOCCMRESET");
	case TCAP_IOCGSTATSP:
		return ("TCAP_IOCGSTATSP");
	case TCAP_IOCSSTATSP:
		return ("TCAP_IOCSSTATSP");
	case TCAP_IOCGSTATS:
		return ("TCAP_IOCGSTATS");
	case TCAP_IOCCSTATS:
		return ("TCAP_IOCCSTATS");
	case TCAP_IOCGNOTIFY:
		return ("TCAP_IOCGNOTIFY");
	case TCAP_IOCSNOTIFY:
		return ("TCAP_IOCSNOTIFY");
	case TCAP_IOCCNOTIFY:
		return ("TCAP_IOCCNOTIFY");
	case TCAP_IOCCMGMT:
		return ("TCAP_IOCCMGMT");
	case TCAP_IOCCPASS:
		return ("TCAP_IOCCPASS");
	default:
		return ("(unknown)");
	}
}
static inline const char *
tc_primname(uint prim)
{
	switch (prim) {
	case TC_INFO_REQ:
		return ("TC_INFO_REQ");
	case TC_BIND_REQ:
		return ("TC_BIND_REQ");
	case TC_UNBIND_REQ:
		return ("TC_UNBIND_REQ");
	case TC_SUBS_BIND_REQ:
		return ("TC_SUBS_BIND_REQ");
	case TC_SUBS_UNBIND_REQ:
		return ("TC_SUBS_UNBIND_REQ");
	case TC_OPTMGMT_REQ:
		return ("TC_OPTMGMT_REQ");
	case TC_UNI_REQ:
		return ("TC_UNI_REQ");
	case TC_BEGIN_REQ:
		return ("TC_BEGIN_REQ");
	case TC_BEGIN_RES:
		return ("TC_BEGIN_RES");
	case TC_CONT_REQ:
		return ("TC_CONT_REQ");
	case TC_END_REQ:
		return ("TC_END_REQ");
	case TC_ABORT_REQ:
		return ("TC_ABORT_REQ");
	case TC_INFO_ACK:
		return ("TC_INFO_ACK");
	case TC_BIND_ACK:
		return ("TC_BIND_ACK");
	case TC_SUBS_BIND_ACK:
		return ("TC_SUBS_BIND_ACK");
	case TC_OK_ACK:
		return ("TC_OK_ACK");
	case TC_ERROR_ACK:
		return ("TC_ERROR_ACK");
	case TC_OPTMGMT_ACK:
		return ("TC_OPTMGMT_ACK");
	case TC_UNI_IND:
		return ("TC_UNI_IND");
	case TC_BEGIN_IND:
		return ("TC_BEGIN_IND");
	case TC_BEGIN_CON:
		return ("TC_BEGIN_CON");
	case TC_CONT_IND:
		return ("TC_CONT_IND");
	case TC_END_IND:
		return ("TC_END_IND");
	case TC_ABORT_IND:
		return ("TC_ABORT_IND");
	case TC_NOTICE_IND:
		return ("TC_NOTICE_IND");
	case TC_INVOKE_REQ:
		return ("TC_INVOKE_REQ");
	case TC_RESULT_REQ:
		return ("TC_RESULT_REQ");
	case TC_ERROR_REQ:
		return ("TC_ERROR_REQ");
	case TC_CANCEL_REQ:
		return ("TC_CANCEL_REQ");
	case TC_REJECT_REQ:
		return ("TC_REJECT_REQ");
	case TC_INVOKE_IND:
		return ("TC_INVOKE_IND");
	case TC_RESULT_IND:
		return ("TC_RESULT_IND");
	case TC_ERROR_IND:
		return ("TC_ERROR_IND");
	case TC_CANCEL_IND:
		return ("TC_CANCEL_IND");
	case TC_REJECT_IND:
		return ("TC_REJECT_IND");
	default:
		return ("(unknown)");
	}
}
static inline const char *
tc_statename(uint state)
{
	switch (state) {
	case TCS_UNBND:
		return ("TCS_UNBND");
	case TCS_WACK_BREQ:
		return ("TCS_WACK_BREQ");
	case TCS_WACK_UREQ:
		return ("TCS_WACK_UREQ");
	case TCS_IDLE:
		return ("TCS_IDLE");
	case TCS_WACK_OPTREQ:
		return ("TCS_WACK_OPTREQ");
	case TCS_WACK_RRES:
		return ("TCS_WACK_RRES");
	case TCS_WCON_CREQ:
		return ("TCS_WCON_CREQ");
	case TCS_WRES_CIND:
		return ("TCS_WRES_CIND");
	case TCS_WACK_CRES:
		return ("TCS_WACK_CRES");
	case TCS_DATA_XFER:
		return ("TCS_DATA_XFER");
	case TCS_WCON_RREQ:
		return ("TCS_WCON_RREQ");
	case TCS_WRES_RIND:
		return ("TCS_WRES_RIND");
	case TCS_WACK_DREQ6:
		return ("TCS_WACK_DREQ6");
	case TCS_WACK_DREQ7:
		return ("TCS_WACK_DREQ7");
	case TCS_WACK_DREQ9:
		return ("TCS_WACK_DREQ9");
	case TCS_WACK_DREQ10:
		return ("TCS_WACK_DREQ10");
	case TCS_WACK_DREQ11:
		return ("TCS_WACK_DREQ11");
	case TCS_NOSTATES:
		return ("TCS_NOSTATES");
	default:
		return ("(unknown)");
	}
}

static uint
tc_get_state(struct tc *tc)
{
	return (tc->state);
}

static uint
tc_set_state(struct tc *tc, uint newstate)
{
	uint oldstate = tc->state;

	if (newstate != oldstate) {
		tc->state = newstate;
		mi_strlog(tc->oq, STRLOGST, SL_TRACE, "%s <- %s", tc_statename(newstate),
			  tc_statename(oldstate));
	}
	return (newstate);
}

static uint
tc_save_state(struct tc *tc)
{
	return ((tc->oldstate = tc_get_state(tc)));
}

static uint
tc_restore_state(struct tc *tc)
{
	return (tc_set_state(tc, tc->oldstate));
}

static inline const char *
tr_primname(uint prim)
{
	switch (prim) {
	case TR_INFO_REQ:
		return ("TR_INFO_REQ");
	case TR_BIND_REQ:
		return ("TR_BIND_REQ");
	case TR_UNBIND_REQ:
		return ("TR_UNBIND_REQ");
	case TR_OPTMGMT_REQ:
		return ("TR_OPTMGMT_REQ");
	case TR_UNI_REQ:
		return ("TR_UNI_REQ");
	case TR_BEGIN_REQ:
		return ("TR_BEGIN_REQ");
	case TR_BEGIN_RES:
		return ("TR_BEGIN_RES");
	case TR_CONT_REQ:
		return ("TR_CONT_REQ");
	case TR_END_REQ:
		return ("TR_END_REQ");
	case TR_ABORT_REQ:
		return ("TR_ABORT_REQ");
	case TR_INFO_ACK:
		return ("TR_INFO_ACK");
	case TR_BIND_ACK:
		return ("TR_BIND_ACK");
	case TR_OK_ACK:
		return ("TR_OK_ACK");
	case TR_ERROR_ACK:
		return ("TR_ERROR_ACK");
	case TR_OPTMGMT_ACK:
		return ("TR_OPTMGMT_ACK");
	case TR_UNI_IND:
		return ("TR_UNI_IND");
	case TR_BEGIN_IND:
		return ("TR_BEGIN_IND");
	case TR_BEGIN_CON:
		return ("TR_BEGIN_CON");
	case TR_CONT_IND:
		return ("TR_CONT_IND");
	case TR_END_IND:
		return ("TR_END_IND");
	case TR_ABORT_IND:
		return ("TR_ABORT_IND");
	case TR_NOTICE_IND:
		return ("TR_NOTICE_IND");
	default:
		return ("(unknown)");
	}
}
static inline const char *
tr_statename(uint state)
{
	switch (state) {
	case TRS_UNBND:
		return ("TRS_UNBND");
	case TRS_WACK_BREQ:
		return ("TRS_WACK_BREQ");
	case TRS_WACK_UREQ:
		return ("TRS_WACK_UREQ");
	case TRS_IDLE:
		return ("TRS_IDLE");
	case TRS_WACK_OPTREQ:
		return ("TRS_WACK_OPTREQ");
	case TRS_WACK_RRES:
		return ("TRS_WACK_RRES");
	case TRS_WCON_CREQ:
		return ("TRS_WCON_CREQ");
	case TRS_WRES_CIND:
		return ("TRS_WRES_CIND");
	case TRS_WACK_CRES:
		return ("TRS_WACK_CRES");
	case TRS_DATA_XFER:
		return ("TRS_DATA_XFER");
	case TRS_WCON_RREQ:
		return ("TRS_WCON_RREQ");
	case TRS_WRES_RIND:
		return ("TRS_WRES_RIND");
	case TRS_WACK_DREQ6:
		return ("TRS_WACK_DREQ6");
	case TRS_WACK_DREQ7:
		return ("TRS_WACK_DREQ7");
	case TRS_WACK_DREQ9:
		return ("TRS_WACK_DREQ9");
	case TRS_WACK_DREQ10:
		return ("TRS_WACK_DREQ10");
	case TRS_WACK_DREQ11:
		return ("TRS_WACK_DREQ11");
	case TRS_NOSTATES:
		return ("TRS_NOSTATES");
	default:
		return ("(unknown)");
	}
}

static uint
tr_get_state(struct tr *tr)
{
	return (tr->state);
}

static uint
tr_set_state(struct tr *tr, uint newstate)
{
	uint oldstate = tr->state;

	if (newstate != oldstate) {
		tr->state = newstate;
		mi_strlog(tr->oq, STRLOGST, SL_TRACE, "%s <- %s", tr_statename(newstate),
			  tr_statename(oldstate));
	}
	return (newstate);
}

static uint
tr_save_state(struct tr *tr)
{
	return ((tr->oldstate = tr_get_state(tr)));
}

static uint
tr_restore_state(struct tr *tr)
{
	return (tr_set_state(tr, tr->oldstate));
}

static uint
xa_get_state(struct xa *xa)
{
	return (xa->state);
}

static uint
xa_set_state(struct xa *xa, uint newstate)
{
	uint oldstate = xa->state;

	if (newstate != oldstate) {
		xa->state = newstate;
		mi_strlog(xa->tr->oq, STRLOGST, SL_TRACE, "%s <- %s", tc_statename(newstate),
			  tc_statename(oldstate));
	}
	return (newstate);
}

static inline uint
xa_save_state(struct xa *xa)
{
	return ((xa->oldstate = xa_get_state(xa)));
}

static inline uint
xa_restore_state(struct xa *xa)
{
	return (xa_set_state(xa, xa->oldstate));
}

/*
 *  =========================================================================
 *
 *  OPTIONS HANDLING
 *
 *  =========================================================================
 */
#define _T_ALIGN_SIZEOF(s) \
	((sizeof((s)) + _T_ALIGN_SIZE - 1) & ~(_T_ALIGN_SIZE - 1))

static inline size_t
tc_opts_size(struct tc *tc, struct tc_opts *ops)
{
	const size_t hlen = sizeof(struct t_opthdr);	/* 32 bytes */
	size_t len = 0;

	if (ops == NULL)
		goto done;
	if (ops->ver)
		len += hlen + _T_ALIGN_SIZEOF(*(ops->ver));
	if (ops->ctx.ptr)
		len += hlen + T_ALIGN(ops->ctx.len);
	if (ops->inf.ptr)
		len += hlen + T_ALIGN(ops->inf.len);
	if (ops->sec.ptr)
		len += hlen + T_ALIGN(ops->sec.len);
	if (ops->pcl)
		len += hlen + _T_ALIGN_SIZEOF(*(ops->pcl));
	if (ops->ret)
		len += hlen + _T_ALIGN_SIZEOF(*(ops->ret));
	if (ops->imp)
		len += hlen + _T_ALIGN_SIZEOF(*(ops->imp));
	if (ops->seq)
		len += hlen + _T_ALIGN_SIZEOF(*(ops->seq));
	if (ops->pri)
		len += hlen + _T_ALIGN_SIZEOF(*(ops->pri));
	if (ops->sls)
		len += hlen + _T_ALIGN_SIZEOF(*(ops->sls));
	if (ops->mp)
		len += hlen + _T_ALIGN_SIZEOF(*(ops->mp));
      done:
	return (len);
}

static inline void
tc_build_opts(struct tc *tc, struct tc_opts *ops, uchar *p)
{
	const size_t hlen = sizeof(struct t_opthdr);	/* 32 bytes */
	struct t_opthdr *oh;

	if (ops == NULL)
		goto done;
	if (ops->ver) {
		oh = (typeof(oh)) p;
		oh->len = hlen + sizeof(*(ops->ver));
		oh->level = T_SS7_TCAP;
		oh->name = T_TCAP_VERSION;
		oh->status = (ops->flags & TF_OPT_VER) ? T_SUCCESS : T_FAILURE;
		p += sizeof(*oh);
		*((typeof(ops->ver)) p) = *(ops->ver);
		p += _T_ALIGN_SIZEOF(*ops->ver);
	}
	if (ops->ctx.ptr) {
		oh = (typeof(oh)) p;
		oh->len = hlen + ops->ctx.len;
		oh->level = T_SS7_TCAP;
		oh->name = T_TCAP_APP_CTX;
		oh->status = (ops->flags & TF_OPT_APP) ? T_SUCCESS : T_FAILURE;
		p += sizeof(*oh);
		bcopy(ops->ctx.ptr, p, ops->ctx.len);
		p += T_ALIGN(ops->ctx.len);
	}
	if (ops->inf.ptr) {
		oh = (typeof(oh)) p;
		oh->len = hlen + ops->inf.len;
		oh->level = T_SS7_TCAP;
		oh->name = T_TCAP_USER_INFO;
		oh->status = (ops->flags & TF_OPT_INF) ? T_SUCCESS : T_FAILURE;
		p += sizeof(*oh);
		bcopy(ops->inf.ptr, p, ops->inf.len);
		p += T_ALIGN(ops->inf.len);
	}
	if (ops->sec.ptr) {
		oh = (typeof(oh)) p;
		oh->len = hlen + ops->sec.len;
		oh->level = T_SS7_TCAP;
		oh->name = T_TCAP_SEC_CTX;
		oh->status = (ops->flags & TF_OPT_SEC) ? T_SUCCESS : T_FAILURE;
		p += sizeof(*oh);
		bcopy(ops->sec.ptr, p, ops->sec.len);
		p += T_ALIGN(ops->sec.len);
	}
	if (ops->pcl) {
		oh = (typeof(oh)) p;
		oh->len = hlen + sizeof(*(ops->pcl));
		oh->level = T_SS7_SCCP;
		oh->name = T_SCCP_PCLASS;
		oh->status = (ops->flags & TF_OPT_PCL) ? T_SUCCESS : T_FAILURE;
		p += sizeof(*oh);
		*((typeof(ops->pcl)) p) = *(ops->pcl);
		p += _T_ALIGN_SIZEOF(*ops->pcl);
	}
	if (ops->ret) {
		oh = (typeof(oh)) p;
		oh->len = hlen + sizeof(*(ops->ret));
		oh->level = T_SS7_SCCP;
		oh->name = T_SCCP_RET_ERROR;
		oh->status = (ops->flags & TF_OPT_RET) ? T_SUCCESS : T_FAILURE;
		p += sizeof(*oh);
		*((typeof(ops->ret)) p) = *(ops->ret);
		p += _T_ALIGN_SIZEOF(*ops->ret);
	}
	if (ops->imp) {
		oh = (typeof(oh)) p;
		oh->len = hlen + sizeof(*(ops->imp));
		oh->level = T_SS7_SCCP;
		oh->name = T_SCCP_IMPORTANCE;
		oh->status = (ops->flags & TF_OPT_IMP) ? T_SUCCESS : T_FAILURE;
		p += sizeof(*oh);
		*((typeof(ops->imp)) p) = *(ops->imp);
		p += _T_ALIGN_SIZEOF(*ops->imp);
	}
	if (ops->seq) {
		oh = (typeof(oh)) p;
		oh->len = hlen + sizeof(*(ops->seq));
		oh->level = T_SS7_SCCP;
		oh->name = T_SCCP_SEQ_CTRL;
		oh->status = (ops->flags & TF_OPT_SEQ) ? T_SUCCESS : T_FAILURE;
		p += sizeof(*oh);
		*((typeof(ops->seq)) p) = *(ops->seq);
		p += _T_ALIGN_SIZEOF(*ops->seq);
	}
	if (ops->pri) {
		oh = (typeof(oh)) p;
		oh->len = hlen + sizeof(*(ops->pri));
		oh->level = T_SS7_SCCP;
		oh->name = T_SCCP_PRIORITY;
		oh->status = (ops->flags & TF_OPT_PRI) ? T_SUCCESS : T_FAILURE;
		p += sizeof(*oh);
		*((typeof(ops->pri)) p) = *(ops->pri);
		p += _T_ALIGN_SIZEOF(*ops->pri);
	}
	if (ops->sls) {
		oh = (typeof(oh)) p;
		oh->len = hlen + sizeof(*(ops->sls));
		oh->level = T_SS7_MTP;
		oh->name = T_MTP_SLS;
		oh->status = (ops->flags & TF_OPT_SLS) ? T_SUCCESS : T_FAILURE;
		p += sizeof(*oh);
		*((typeof(ops->sls)) p) = *(ops->sls);
		p += _T_ALIGN_SIZEOF(*ops->sls);
	}
	if (ops->mp) {
		oh = (typeof(oh)) p;
		oh->len = hlen + sizeof(*(ops->mp));
		oh->level = T_SS7_MTP;
		oh->name = T_MTP_MP;
		oh->status = (ops->flags & TF_OPT_MP) ? T_SUCCESS : T_FAILURE;
		p += sizeof(*oh);
		*((typeof(ops->mp)) p) = *(ops->mp);
		p += _T_ALIGN_SIZEOF(*ops->mp);
	}
      done:
	return;
}

static int
tc_parse_opts(struct tc *tc, struct tc_opts *ops, uchar *op, size_t len)
{
	struct t_opthdr *oh;

	for (oh = _T_OPT_FIRSTHDR_OFS(op, len, 0); oh; oh = _T_OPT_NEXTHDR_OFS(op, len, oh, 0)) {
		switch (oh->level) {
		case T_SS7_TCAP:
			switch (oh->name) {
			case T_TCAP_VERSION:
				ops->ver = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_VER;
				continue;
			case T_TCAP_APP_CTX:
				ops->ctx.ptr = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->ctx.len = oh->len - sizeof(*oh);
				ops->flags |= TF_OPT_APP;
				continue;
			case T_TCAP_USER_INFO:
				ops->inf.ptr = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->inf.len = oh->len - sizeof(*oh);
				ops->flags |= TF_OPT_INF;
				continue;
			case T_TCAP_SEC_CTX:
				ops->sec.ptr = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->sec.len = oh->len - sizeof(*oh);
				ops->flags |= TF_OPT_SEC;
				continue;
			}
			break;
		case T_SS7_SCCP:
			switch (oh->name) {
			case T_SCCP_PCLASS:
				ops->pcl = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_PCL;
				continue;
			case T_SCCP_RET_ERROR:
				ops->ret = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_RET;
				continue;
			case T_SCCP_IMPORTANCE:
				ops->imp = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_IMP;
				continue;
			case T_SCCP_SEQ_CTRL:
				ops->seq = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_SEQ;
				continue;
			case T_SCCP_PRIORITY:
				ops->pri = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_PRI;
				continue;
			}
			break;
		case T_SS7_MTP:
			switch (oh->name) {
			case T_MTP_SLS:
				ops->sls = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_SLS;
				continue;
			case T_MTP_MP:
				ops->mp = (void *) _T_OPT_DATA_OFS(oh, 0);
				ops->flags |= TF_OPT_MP;
				continue;
			}
			break;
		default:
			break;
		}
	}
	if (oh)
		return (TCBADOPT);
	return (0);
}

static inline int
tc_opt_check(struct tc *tc, struct tc_opts *ops)
{
	if (ops && ops->flags) {
		ops->flags = 0;
		if (ops->ver)
			ops->flags |= TF_OPT_VER;
		if (ops->ctx.ptr && ops->ctx.len <= TCAP_BUF_MAXLEN)
			ops->flags |= TF_OPT_APP;
		if (ops->inf.ptr && ops->inf.len <= TCAP_BUF_MAXLEN)
			ops->flags |= TF_OPT_INF;
		if (ops->sec.ptr && ops->sec.len <= TCAP_BUF_MAXLEN)
			ops->flags |= TF_OPT_SEC;
		if (ops->pcl)
			ops->flags |= TF_OPT_PCL;
		if (ops->ret)
			ops->flags |= TF_OPT_RET;
		if (ops->imp)
			ops->flags |= TF_OPT_IMP;
		if (ops->seq)
			ops->flags |= TF_OPT_SEQ;
		if (ops->pri)
			ops->flags |= TF_OPT_PRI;
		if (ops->sls)
			ops->flags |= TF_OPT_SLS;
		if (ops->mp)
			ops->flags |= TF_OPT_MP;
	}
	return (0);
}
static inline int
tc_opt_default(struct tc *tc, struct tc_opts *ops)
{
	if (ops) {
		int flags = ops->flags;

		ops->flags = 0;
		if (!flags || ops->ver) {
			ops->ver = &opt_defaults.ver;
			ops->flags |= TF_OPT_VER;
		}
		if (!flags || ops->ctx.ptr) {
			ops->ctx.ptr = opt_defaults.ctx.buf;
			ops->ctx.len = opt_defaults.ctx.len;
			ops->flags |= TF_OPT_APP;
		}
		if (!flags || ops->inf.ptr) {
			ops->inf.ptr = opt_defaults.inf.buf;
			ops->inf.len = opt_defaults.inf.len;
			ops->flags |= TF_OPT_INF;
		}
		if (!flags || ops->sec.ptr) {
			ops->sec.ptr = opt_defaults.sec.buf;
			ops->sec.len = opt_defaults.sec.len;
			ops->flags |= TF_OPT_SEC;
		}
		if (!flags || ops->pcl) {
			ops->pcl = &opt_defaults.pcl;
			ops->flags |= TF_OPT_PCL;
		}
		if (!flags || ops->ret) {
			ops->ret = &opt_defaults.ret;
			ops->flags |= TF_OPT_RET;
		}
		if (!flags || ops->imp) {
			ops->imp = &opt_defaults.imp;
			ops->flags |= TF_OPT_IMP;
		}
		if (!flags || ops->seq) {
			ops->seq = &opt_defaults.seq;
			ops->flags |= TF_OPT_SEQ;
		}
		if (!flags || ops->pri) {
			ops->pri = &opt_defaults.pri;
			ops->flags |= TF_OPT_PRI;
		}
		if (!flags || ops->sls) {
			ops->sls = &opt_defaults.sls;
			ops->flags |= TF_OPT_SLS;
		}
		if (!flags || ops->mp) {
			ops->mp = &opt_defaults.mp;
			ops->flags |= TF_OPT_MP;
		}
		return (0);
	}
	swerr();
	return (-EFAULT);
}
static inline int
tc_opt_current(struct tc *tc, struct tc_opts *ops)
{
	int flags = ops->flags;

	ops->flags = 0;
	if (!flags || ops->ver) {
		ops->ver = &tc->options.ver;
		ops->flags |= TF_OPT_VER;
	}
	if (!flags || ops->ctx.ptr) {
		ops->ctx.ptr = tc->options.ctx.buf;
		ops->ctx.len = tc->options.ctx.len;
		ops->flags |= TF_OPT_APP;
	}
	if (!flags || ops->inf.ptr) {
		ops->inf.ptr = tc->options.inf.buf;
		ops->inf.len = tc->options.inf.len;
		ops->flags |= TF_OPT_INF;
	}
	if (!flags || ops->sec.ptr) {
		ops->sec.ptr = tc->options.sec.buf;
		ops->sec.len = tc->options.sec.len;
		ops->flags |= TF_OPT_SEC;
	}
	if (!flags || ops->pcl) {
		ops->pcl = &tc->options.pcl;
		ops->flags |= TF_OPT_PCL;
	}
	if (!flags || ops->ret) {
		ops->ret = &tc->options.ret;
		ops->flags |= TF_OPT_RET;
	}
	if (!flags || ops->imp) {
		ops->imp = &tc->options.imp;
		ops->flags |= TF_OPT_IMP;
	}
	if (!flags || ops->seq) {
		ops->seq = &tc->options.seq;
		ops->flags |= TF_OPT_SEQ;
	}
	if (!flags || ops->pri) {
		ops->pri = &tc->options.pri;
		ops->flags |= TF_OPT_PRI;
	}
	if (!flags || ops->sls) {
		ops->sls = &tc->options.sls;
		ops->flags |= TF_OPT_SLS;
	}
	if (!flags || ops->mp) {
		ops->mp = &tc->options.mp;
		ops->flags |= TF_OPT_MP;
	}
	return (0);
}
static inline int
tc_opt_negotiate(struct tc *tc, struct tc_opts *ops)
{
	if (ops->flags) {
		ops->flags = 0;
		if (ops->ver) {
			tc->options.ver = *ops->ver;
			ops->ver = &tc->options.ver;
			ops->flags |= TF_OPT_VER;
		}
		if (ops->ctx.ptr && ops->ctx.len <= TCAP_BUF_MAXLEN) {
			bcopy(ops->ctx.ptr, tc->options.ctx.buf, ops->ctx.len);
			tc->options.ctx.len = ops->ctx.len;
			ops->ctx.ptr = tc->options.ctx.buf;
			ops->flags |= TF_OPT_APP;
		}
		if (ops->inf.ptr && ops->inf.len <= TCAP_BUF_MAXLEN) {
			bcopy(ops->inf.ptr, tc->options.inf.buf, ops->inf.len);
			tc->options.inf.len = ops->inf.len;
			ops->inf.ptr = tc->options.inf.buf;
			ops->flags |= TF_OPT_INF;
		}
		if (ops->sec.ptr && ops->sec.len <= TCAP_BUF_MAXLEN) {
			bcopy(ops->sec.ptr, tc->options.sec.buf, ops->sec.len);
			tc->options.sec.len = ops->sec.len;
			ops->sec.ptr = tc->options.sec.buf;
			ops->flags |= TF_OPT_SEC;
		}
		if (ops->pcl) {
			tc->options.pcl = *ops->pcl;
			ops->pcl = &tc->options.pcl;
			ops->flags |= TF_OPT_PCL;
		}
		if (ops->ret) {
			tc->options.ret = *ops->ret;
			ops->ret = &tc->options.ret;
			ops->flags |= TF_OPT_RET;
		}
		if (ops->imp) {
			tc->options.imp = *ops->imp;
			ops->imp = &tc->options.imp;
			ops->flags |= TF_OPT_IMP;
		}
		if (ops->seq) {
			tc->options.seq = *ops->seq;
			ops->seq = &tc->options.seq;
			ops->flags |= TF_OPT_SEQ;
		}
		if (ops->pri) {
			tc->options.pri = *ops->pri;
			ops->pri = &tc->options.pri;
			ops->flags |= TF_OPT_PRI;
		}
		if (ops->sls) {
			tc->options.sls = *ops->sls;
			ops->sls = &tc->options.sls;
			ops->flags |= TF_OPT_SLS;
		}
		if (ops->mp) {
			tc->options.mp = *ops->mp;
			ops->mp = &tc->options.mp;
			ops->flags |= TF_OPT_MP;
		}
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  TC Provider -> TC User primitives.
 *
 *  =========================================================================
 */

static int
m_error(struct tc *tc, queue_t *q, mblk_t *msg, int err)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_ERROR;
		*mp->b_wptr++ = err < 0 ? -err : err;
		*mp->b_wptr++ = err < 0 ? -err : err;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- M_ERROR");
		putnext(tc->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#define FIXME (-1)

/**
 * tc_info_ack: - issue TC_INFO_ACK primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
tc_info_ack(struct tc *tc, queue_t *q, mblk_t *msg)
{
	struct TC_info_ack *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INFO_ACK;
	p->TSDU_size = FIXME;
	p->ETSDU_size = FIXME;
	p->CDATA_size = FIXME;
	p->DDATA_size = FIXME;
	p->ADDR_size = FIXME;
	p->OPT_size = FIXME;
	p->TIDU_size = FIXME;
	p->SERV_type = FIXME;
	p->CURRENT_state = tc_get_state(tc);
	p->PROVIDER_flag = FIXME;
	p->TCI_version = FIXME;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(q, STRLOGTX, SL_TRACE, "<- TC_INFO_ACK");
	putnext(tc->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * tc_bind_ack: - issue TC_BIND_ACK primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @xacts: maximum number of outstanding dialogue indications
 * @add_len: address length
 * @add_ptr: address pointer
 */
static int
tc_bind_ack(struct tc *tc, queue_t *q, mblk_t *msg,
	    t_uscalar_t xacts, size_t add_len, caddr_t add_ptr)
{
	struct TC_bind_ack *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_BIND_ACK;
	p->ADDR_length = add_len;
	p->ADDR_offset = add_len ? sizeof(*p) : 0;
	p->XACT_number = xacts;
	p->TOKEN_value = (t_uscalar_t) (ulong) tc->oq;
	mp->b_wptr += sizeof(*p);
	bcopy(add_ptr, mp->b_wptr, add_len);
	mp->b_wptr += add_len;
	freemsg(msg);
	mi_strlog(q, STRLOGTX, SL_TRACE, "<- TC_BIND_ACK");
	putnext(tc->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * tc_subs_bind_ack: - issue TC_SUBS_BIND_ACK primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
tc_subs_bind_ack(struct tc *tc, queue_t *q, mblk_t *msg)
{
	struct TC_subs_bind_ack *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_SUBS_BIND_ACK;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(q, STRLOGTX, SL_TRACE, "<- TC_SUBS_BIND_ACK");
	putnext(tc->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * tc_ok_ack: - issue TC_OK_ACK primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static int
tc_ok_ack(struct tc *tc, queue_t *q, mblk_t *msg, t_uscalar_t prim)
{
	struct TC_ok_ack *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_OK_ACK;
	p->CORRECT_prim = prim;
	mp->b_wptr += sizeof(*p);
	switch (tc_get_state(tc)) {
	case TCS_WACK_UREQ:
		tc_set_state(tc, TCS_UNBND);
		break;
	case TCS_WACK_DREQ6:
	case TCS_WACK_DREQ7:
	case TCS_WACK_DREQ9:
	case TCS_WACK_DREQ10:
	case TCS_WACK_DREQ11:
		tc_set_state(tc, TCS_IDLE);
		break;
	}
	freemsg(msg);
	mi_strlog(q, STRLOGTX, SL_TRACE, "<- TC_OK_ACK");
	putnext(tc->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * tc_error_ack: - issue TC_ERROR_ACK primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @error: error number
 * @did: dialog id
 * @iid: invoke id
 */
static int
tc_error_ack(struct tc *tc, queue_t *q, mblk_t *msg,
	     t_uscalar_t prim, t_scalar_t error, t_uscalar_t did, t_uscalar_t iid)
{
	struct TC_error_ack *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_ERROR_ACK;
	p->ERROR_prim = prim;
	p->TRPI_error = error < 0 ? TCSYSERR : error;
	p->UNIX_error = error < 0 ? -error : 0;
	p->DIALOG_id = did;
	p->INVOKE_id = iid;
	mp->b_wptr += sizeof(*p);
	tc_restore_state(tc);
	freemsg(msg);
	mi_strlog(q, STRLOGTX, SL_TRACE, "<- TC_ERROR_ACK");
	putnext(tc->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * tc_optmgmt_ack: - issue TC_OPTMGMT_ACK primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: management flags
 * @opt_len: options length
 * @opt_ptr: options pointer
 */
static int
tc_optmgmt_ack(struct tc *tc, queue_t *q, mblk_t *msg,
	       t_uscalar_t flags, size_t opt_len, caddr_t opt_ptr)
{
	struct TC_optmgmt_ack *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_OPTMGMT_ACK;
	p->OPT_length = opt_len;
	p->OPT_offset = sizeof(*p);
	p->MGMT_flags = flags;
	mp->b_wptr += sizeof(*p);
	bcopy(opt_ptr, mp->b_wptr, opt_len);
	mp->b_wptr += opt_len;
	freemsg(msg);
	mi_strlog(q, STRLOGTX, SL_TRACE, "<- TC_OPTMGMT_ACK");
	putnext(tc->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * tc_uni_ind: - issue TC_UNI_IND primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @did: dialog id
 * @src_len: source address length
 * @src_ptr: source address pointer
 * @dst_len: destination address length
 * @dst_ptr: destination address pointer
 * @opt_len: options length
 * @opt_ptr: options pointer
 * @dp: user info
 * @comp: component primitives
 */
static inline int
tc_uni_ind(struct tc *tc, queue_t *q, mblk_t *msg,
	   t_uscalar_t did, size_t src_len, caddr_t src_ptr,
	   size_t dst_len, caddr_t dst_ptr, size_t opt_len, caddr_t opt_ptr, mblk_t *dp,
	   mblk_t *comp)
{
	struct TC_uni_ind *p;
	mblk_t *mp;

	if (unlikely(!canputnext(tc->oq)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + src_len + dst_len + opt_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_UNI_IND;
	p->SRC_length = src_len;
	p->SRC_offset = sizeof(*p);
	p->DEST_length = dst_len;
	p->DEST_offset = sizeof(*p) + src_len;
	p->OPT_length = opt_len;
	p->OPT_offset = sizeof(*p) + src_len + dst_len;
	p->DIALOG_id = did;
	p->COMP_flags = (comp != NULL);
	mp->b_wptr += sizeof(*p);
	bcopy(src_ptr, mp->b_wptr, src_len);
	mp->b_wptr += src_len;
	bcopy(dst_ptr, mp->b_wptr, dst_len);
	mp->b_wptr += dst_len;
	bcopy(opt_ptr, mp->b_wptr, opt_len);
	mp->b_wptr += opt_len;
	mp->b_cont = dp;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(q, STRLOGDA, SL_TRACE, "<- TC_UNI_IND");
	for (mp->b_next = comp; mp; mp = comp) {
		comp = XCHG(&mp->b_next, NULL);
		putnext(tc->oq, mp);
	}
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
}

/**
 * tc_begin_ind: - issue TC_BEGIN_IND primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @did: dialog id
 * @src_len: source address length
 * @src_ptr: source address pointer
 * @dst_len: destination address length
 * @dst_ptr: destination address pointer
 * @opt_len: options length
 * @opt_ptr: options pointer
 * @dp: user info
 * @comp: component primitives
 */
static inline int
tc_begin_ind(struct tc *tc, queue_t *q, mblk_t *msg,
	     t_uscalar_t did, size_t src_len, caddr_t src_ptr,
	     size_t dst_len, caddr_t dst_ptr, size_t opt_len, caddr_t opt_ptr, mblk_t *dp,
	     mblk_t *comp)
{
	struct TC_begin_ind *p;
	mblk_t *mp;

	if (unlikely(!canputnext(tc->oq)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + src_len + dst_len + opt_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_BEGIN_IND;
	p->SRC_length = src_len;
	p->SRC_offset = sizeof(*p);
	p->DEST_length = dst_len;
	p->DEST_offset = p->SRC_offset + src_len;
	p->OPT_length = opt_len;
	p->OPT_offset = p->DEST_offset + dst_len;
	p->DIALOG_id = did;
	p->COMP_flags = (comp != NULL);
	mp->b_wptr += sizeof(*p);
	bcopy(src_ptr, mp->b_wptr, src_len);
	mp->b_wptr += src_len;
	bcopy(dst_ptr, mp->b_wptr, dst_len);
	mp->b_wptr += dst_len;
	bcopy(opt_ptr, mp->b_wptr, opt_len);
	mp->b_wptr += opt_len;
	mp->b_cont = dp;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(q, STRLOGDA, SL_TRACE, "<- TC_BEGIN_IND");
	for (mp->b_next = comp; mp; mp = comp) {
		comp = XCHG(&mp->b_next, NULL);
		putnext(tc->oq, mp);
	}
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
}

/**
 * tc_begin_con: - issue TC_BEGIN_CON primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @did: dialog id
 * @opt_len: options length
 * @opt_ptr: options pointer
 * @dp: user info
 * @comp: component primitives
 */
static inline int
tc_begin_con(struct tc *tc, queue_t *q, mblk_t *msg,
	     t_uscalar_t did, size_t opt_len, caddr_t opt_ptr, mblk_t *dp, mblk_t *comp)
{
	struct TC_begin_con *p;
	mblk_t *mp;

	if (unlikely(!canputnext(tc->oq)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_BEGIN_CON;
	p->OPT_length = opt_len;
	p->OPT_offset = sizeof(*p);
	p->DIALOG_id = did;
	p->COMP_flags = (comp != NULL);
	mp->b_wptr += sizeof(*p);
	bcopy(opt_ptr, mp->b_wptr, opt_len);
	mp->b_wptr += opt_len;
	mp->b_cont = dp;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(q, STRLOGDA, SL_TRACE, "<- TC_BEGIN_CON");
	for (mp->b_next = comp; mp; mp = comp) {
		comp = XCHG(&mp->b_next, NULL);
		putnext(tc->oq, mp);
	}
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
}

/**
 * tc_cont_ind: - issue TC_CONT_IND primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @did: dialog id
 * @opt_len: options length
 * @opt_ptr: options pointer
 * @dp: user info
 * @comp: component primitives
 */
static inline int
tc_cont_ind(struct tc *tc, queue_t *q, mblk_t *msg,
	    t_uscalar_t did, size_t opt_len, caddr_t opt_ptr, mblk_t *dp, mblk_t *comp)
{
	struct TC_cont_ind *p;
	mblk_t *mp;

	if (unlikely(!canputnext(tc->oq)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_CONT_IND;
	p->OPT_length = opt_len;
	p->OPT_offset = sizeof(*p);
	p->DIALOG_id = did;
	p->COMP_flags = (comp != NULL);
	mp->b_wptr += sizeof(*p);
	bcopy(opt_ptr, mp->b_wptr, opt_len);
	mp->b_wptr += opt_len;
	mp->b_cont = dp;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(q, STRLOGDA, SL_TRACE, "<- TC_CONT_IND");
	for (mp->b_next = comp; mp; mp = comp) {
		comp = XCHG(&mp->b_next, NULL);
		putnext(tc->oq, mp);
	}
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
}

/**
 * tc_end_ind: - issue TC_END_IND primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @did: dialog id
 * @opt_len: options length
 * @opt_ptr: options pointer
 * @dp: user info
 * @comp: component primitives
 */
static inline int
tc_end_ind(struct tc *tc, queue_t *q, mblk_t *msg,
	   t_uscalar_t did, size_t opt_len, caddr_t opt_ptr, mblk_t *dp, mblk_t *comp)
{
	struct TC_end_ind *p;
	mblk_t *mp;

	if (unlikely(!canputnext(tc->oq)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_END_IND;
	p->OPT_length = opt_len;
	p->OPT_offset = sizeof(*p);
	p->DIALOG_id = did;
	p->COMP_flags = (comp != NULL);
	mp->b_wptr += sizeof(*p);
	bcopy(opt_ptr, mp->b_wptr, opt_len);
	mp->b_wptr += opt_len;
	mp->b_cont = dp;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(q, STRLOGDA, SL_TRACE, "<- TC_END_IND");
	for (mp->b_next = comp; mp; mp = comp) {
		comp = XCHG(&mp->b_next, NULL);
		putnext(tc->oq, mp);
	}
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
}

/**
 * tc_abort_ind: - issue TC_ABORT_IND primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @did: dialog id
 * @reason: abort reason
 * @orig: originator
 * @opt_len: options length
 * @opt_ptr: options pointer
 * @dp: user info
 */
static inline int
tc_abort_ind(struct tc *tc, queue_t *q, mblk_t *msg,
	     t_uscalar_t did, t_uscalar_t reason, t_uscalar_t orig,
	     size_t opt_len, caddr_t opt_ptr, mblk_t *dp)
{
	struct TC_abort_ind *p;
	mblk_t *mp;

	if (unlikely(!canputnext(tc->oq)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_ABORT_IND;
	p->OPT_length = opt_len;
	p->OPT_offset = sizeof(*p);
	p->DIALOG_id = did;
	p->ABORT_reason = reason;
	p->ORIGINATOR = orig;
	mp->b_wptr += sizeof(*p);
	bcopy(opt_ptr, mp->b_wptr, opt_len);
	mp->b_wptr += opt_len;
	mp->b_cont = dp;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(q, STRLOGDA, SL_TRACE, "<- TC_ABORT_IND");
	putnext(tc->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
}

/**
 * tc_notice_ind: - issue TC_NOTICE_IND primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @did: dialog id
 * @cause: report cause
 * @dp: user data
 */
static inline int
tc_notice_ind(struct tc *tc, queue_t *q, mblk_t *msg,
	      t_uscalar_t did, t_uscalar_t cause, mblk_t *dp)
{
	struct TC_notice_ind *p;
	mblk_t *mp;

	if (unlikely(!canputnext(tc->oq)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_NOTICE_IND;
	p->DIALOG_id = did;
	p->REPORT_cause = cause;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(q, STRLOGTX, SL_TRACE, "<- TC_NOTICE_IND");
	putnext(tc->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
}

/**
 * tc_invoke_ind: - issue TC_INVOKE_IND primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @did: dialog id
 * @cls: operations class
 * @iid: invoke id
 * @lid: linked id
 * @opn: operation
 * @more: more flag
 * @dp: parameters
 */
static inline mblk_t *
tc_invoke_ind(struct tc *tc, queue_t *q, mblk_t *msg, t_uscalar_t did,
	      t_uscalar_t cls, t_uscalar_t iid, t_uscalar_t lid, t_uscalar_t opn, t_uscalar_t more,
	      mblk_t *dp)
{
	struct TC_invoke_ind *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_INVOKE_IND;
	p->DIALOG_id = did;
	p->OP_class = cls;
	p->INVOKE_id = iid;
	p->LINKED_id = lid;
	p->OPERATION = opn;
	p->MORE_flag = more;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(q, STRLOGDA, SL_TRACE, "<- TC_INVOKE_IND");
	return (mp);
      enobufs:
	return (NULL);
}

/**
 * tc_result_ind: - issue TC_RESULT_IND primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @did: dialog id
 * @iid: invoke id
 * @opn: operation
 * @more: more flag
 * @dp: parameters
 */
static inline mblk_t *
tc_result_ind(struct tc *tc, queue_t *q, mblk_t *msg, t_uscalar_t did, t_uscalar_t iid,
	      t_uscalar_t opn, t_uscalar_t more, mblk_t *dp)
{
	struct TC_result_ind *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_RESULT_IND;
	p->DIALOG_id = did;
	p->INVOKE_id = iid;
	p->OPERATION = opn;
	p->MORE_flag = more;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(q, STRLOGDA, SL_TRACE, "<- TC_RESULT_IND");
	return (mp);
      enobufs:
	return (NULL);
}

/**
 * tc_error_ind: - issue TC_ERROR_IND primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @did: dialog id
 * @iid: invoke id
 * @code: error code
 * @dp: parameters
 */
static inline mblk_t *
tc_error_ind(struct tc *tc, queue_t *q, mblk_t *msg, t_uscalar_t did, t_uscalar_t iid,
	     t_uscalar_t code, mblk_t *dp)
{
	struct TC_error_ind *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_ERROR_IND;
	p->DIALOG_id = did;
	p->INVOKE_id = iid;
	p->ERROR_code = code;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(q, STRLOGDA, SL_TRACE, "<- TC_ERROR_IND");
	return (mp);
      enobufs:
	return (NULL);
}

/**
 * tc_cancel_ind: - issue TC_CANCEL_IND primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @did: dialog id
 * @iid: invoke id
 */
static inline mblk_t *
tc_cancel_ind(struct tc *tc, queue_t *q, mblk_t *msg, t_uscalar_t did, t_uscalar_t iid)
{
	struct TC_cancel_ind *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_CANCEL_IND;
	p->DIALOG_id = did;
	p->INVOKE_id = iid;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(q, STRLOGDA, SL_TRACE, "<- TC_CANCEL_IND");
	return (mp);
      enobufs:
	return (NULL);
}

/**
 * tc_reject_ind: - issue TC_REJECT_IND primitive
 * @tc: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @did: dialog id
 * @iid: invoke id
 */
static inline mblk_t *
tc_reject_ind(struct tc *tc, queue_t *q, mblk_t *msg, t_uscalar_t did, t_uscalar_t iid)
{
	struct TC_cancel_ind *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TC_CANCEL_IND;
	p->DIALOG_id = did;
	p->INVOKE_id = iid;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(q, STRLOGDA, SL_TRACE, "<- TC_CANCEL_IND");
	return (mp);
      enobufs:
	return (NULL);
}

/*
 *  =========================================================================
 *
 *  TR User -> TR Provider primitives.
 *
 *  =========================================================================
 */

/**
 * tr_info_req: - issue TR_INFO_REQ primitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
tr_info_req(struct tr *tr, queue_t *q, mblk_t *msg)
{
	struct TR_info_req *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TR_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(q, STRLOGTX, SL_TRACE, "TR_INFO_REQ ->");
	putnext(tr->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * tr_addr_req: - issue a TR_ADDR_REQ primitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upone success
 */
static inline int
tr_addr_req(struct tr *tr, queue_t *q, mblk_t *msg)
{
	struct TR_addr_req *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TR_ADDR_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	mi_strlog(q, STRLOGTX, SL_TRACE, "TR_ADDR_REQ ->");
	putnext(tr->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * tr_bind_req: - issue TR_BIND_REQ primitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @xacts:
 * @flags:
 * @add_len:
 * @add_ptr:
 */
static inline int
tr_bind_req(struct tr *tr, queue_t *q, mblk_t *msg,
	    t_uscalar_t xacts, t_uscalar_t flags, size_t add_len, caddr_t add_ptr)
{
	struct TR_bind_req *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TR_BIND_REQ;
	p->ADDR_length = add_len;
	p->ADDR_offset = sizeof(*p);
	p->XACT_number = xacts;
	p->BIND_flags = flags;
	mp->b_wptr += sizeof(*p);
	bcopy(add_ptr, mp->b_wptr, add_len);
	mp->b_wptr += add_len;
	freemsg(msg);
	tr_set_state(tr, TRS_WACK_BREQ);
	mi_strlog(q, STRLOGTX, SL_TRACE, "TR_BIND_REQ ->");
	putnext(tr->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * tr_unbind_req: - issue TR_UNBIND_REQ primitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
tr_unbind_req(struct tr *tr, queue_t *q, mblk_t *msg)
{
	struct TR_unbind_req *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TR_UNBIND_REQ;
	mp->b_wptr += sizeof(*p);
	freemsg(msg);
	tr_set_state(tr, TRS_WACK_UREQ);
	mi_strlog(q, STRLOGTX, SL_TRACE, "TR_UNBIND_REQ ->");
	putnext(tr->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * tr_optmgmt_req: - issue TR_OPTMGMT_REQ primitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: management flags
 * @opt_len: options length
 * @opt_ptr: options pointer
 */
static inline int
tr_optmgmt_req(struct tr *tr, queue_t *q, mblk_t *msg,
	       t_uscalar_t flags, size_t opt_len, caddr_t opt_ptr)
{
	struct TR_optmgmt_req *p;
	mblk_t *mp;

	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TR_OPTMGMT_REQ;
	p->OPT_length = opt_len;
	p->OPT_offset = sizeof(*p);
	p->MGMT_flags = flags;
	mp->b_wptr += sizeof(*p);
	bcopy(opt_ptr, mp->b_wptr, opt_len);
	mp->b_wptr += opt_len;
	freemsg(msg);
	mi_strlog(q, STRLOGTX, SL_TRACE, "TR_OPTMGMT_REQ ->");
	putnext(tr->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/**
 * tr_uni_req: - issue TR_UNIT_REQ primitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data (dialog portion and component portion)
 * @dst_len: destination address length
 * @dst_ptr: destination address pointer
 * @org_len: originating address length
 * @org_ptr: originating address pointer
 * @opt_len: options length
 * @opt_ptr: options pointer
 * @dp: user data (optional dialogue, mandatory components)
 */
static inline int
tr_uni_req(struct tr *tr, queue_t *q, mblk_t *msg,
	   size_t dst_len, caddr_t dst_ptr,
	   size_t org_len, caddr_t org_ptr, size_t opt_len, caddr_t opt_ptr, mblk_t *dp)
{
	struct TR_uni_req *p;
	mblk_t *mp;

	if (unlikely(!canputnext(tr->oq)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + dst_len + org_len + opt_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TR_UNI_REQ;
	p->DEST_length = dst_len;
	p->DEST_offset = sizeof(*p);
	p->ORIG_length = org_len;
	p->ORIG_length = p->DEST_offset + p->DEST_length;
	p->OPT_length = opt_len;
	p->OPT_offset = p->ORIG_offset + p->ORIG_length;
	mp->b_wptr += sizeof(*p);
	bcopy(dst_ptr, mp->b_wptr, dst_len);
	mp->b_wptr += dst_len;
	bcopy(org_ptr, mp->b_wptr, org_len);
	mp->b_wptr += org_len;
	bcopy(opt_ptr, mp->b_wptr, opt_len);
	mp->b_wptr += opt_len;
	mp->b_cont = dp;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(q, STRLOGTX, SL_TRACE, "TR_UNI_REQ ->");
	putnext(tr->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
}

/**
 * tr_begin_req: - issue TR_BEGIN_REQ primitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @cid: correlation id
 * @flags: association flags
 * @dst_len: destination address length
 * @dst_ptr: destination address pointer
 * @org_len: originating address length
 * @org_ptr: originating address pointer
 * @opt_len: options length
 * @opt_ptr: options pointer
 * @dp: user data (optional dialogue, mandatory components)
 */
static inline int
tr_begin_req(struct tr *tr, queue_t *q, mblk_t *msg, t_uscalar_t cid, t_uscalar_t flags,
	     size_t dst_len, caddr_t dst_ptr,
	     size_t org_len, caddr_t org_ptr, size_t opt_len, caddr_t opt_ptr, mblk_t *dp)
{
	struct TR_begin_req *p;
	mblk_t *mp;

	if (unlikely(!canputnext(tr->oq)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + dst_len + org_len + opt_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TR_BEGIN_REQ;
	p->CORR_id = cid;
	p->ASSOC_flags = flags;
	p->DEST_length = dst_len;
	p->DEST_offset = sizeof(*p);
	p->ORIG_length = org_len;
	p->ORIG_offset = p->DEST_offset + p->DEST_length;
	p->OPT_length = opt_len;
	p->ORIG_offset = p->ORIG_offset + p->ORIG_length;
	mp->b_wptr += sizeof(*p);
	bcopy(dst_ptr, mp->b_wptr, dst_len);
	mp->b_wptr += dst_len;
	bcopy(org_ptr, mp->b_wptr, org_len);
	mp->b_wptr += org_len;
	bcopy(opt_ptr, mp->b_wptr, opt_len);
	mp->b_wptr += opt_len;
	mp->b_cont = dp;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(q, STRLOGTX, SL_TRACE, "TR_BEGIN_REQ ->");
	putnext(tr->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
}

/**
 * tr_begin_res: - issue TR_BEGIN_RES primitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @tid: transaction id
 * @flags: association flags
 * @org_len: responding address length
 * @org_ptr: responding address pointer
 * @opt_len: options length
 * @opt_prt: options pointer
 * @dp: user data (dialogue portion and component portion)
 */
static inline int
tr_begin_res(struct tr *tr, queue_t *q, mblk_t *msg, struct xa *xa,
	     t_uscalar_t tid, t_uscalar_t flags, size_t org_len, caddr_t org_ptr, size_t opt_len,
	     caddr_t opt_ptr, mblk_t *dp)
{
	struct TR_begin_res *p;
	mblk_t *mp;

	if (unlikely(!canputnext(tr->oq)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + org_len + opt_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TR_BEGIN_RES;
	p->TRANS_id = tid;
	p->ASSOC_flags = flags;
	p->ORIG_length = org_len;
	p->ORIG_offset = sizeof(*p);
	p->OPT_length = opt_len;
	p->OPT_offset = p->ORIG_offset + p->ORIG_length;
	mp->b_wptr += sizeof(*p);
	bcopy(org_ptr, mp->b_wptr, org_len);
	mp->b_wptr += org_len;
	bcopy(opt_ptr, mp->b_wptr, opt_len);
	mp->b_wptr += opt_len;
	mp->b_cont = dp;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	xa_set_state(xa, TRS_DATA_XFER);
	mi_strlog(q, STRLOGTX, SL_TRACE, "TR_BEGIN_RES ->");
	putnext(tr->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
}

/**
 * tr_cont_req: - issue TR_CONT_REQ primitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @tid: transaction id
 */
static inline int
tr_cont_req(struct tr *tr, queue_t *q, mblk_t *msg, struct xa *xa,
	    t_uscalar_t tid, t_uscalar_t flags, size_t opt_len, caddr_t opt_ptr, mblk_t *dp)
{
	struct TR_cont_req *p;
	mblk_t *mp;

	if (unlikely(!canputnext(tr->oq)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TR_CONT_REQ;
	p->TRANS_id = tid;
	p->ASSOC_flags = flags;
	p->OPT_length = opt_len;
	p->OPT_offset = sizeof(*p);
	mp->b_wptr += sizeof(*p);
	bcopy(opt_ptr, mp->b_wptr, opt_len);
	mp->b_wptr += opt_len;
	mp->b_cont = dp;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(q, STRLOGTX, SL_TRACE, "TR_CONT_REQ ->");
	putnext(tr->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
}

/**
 * tr_end_req: - issue TR_END_REQ primitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @tid: transaction id
 * @term: termination scenario
 * @opt_len: options length
 * @opt_ptr: options pointer
 * @dp: user data (dialog and component portions)
 */
static inline int
tr_end_req(struct tr *tr, queue_t *q, mblk_t *msg, struct xa *xa,
	   t_uscalar_t tid, t_uscalar_t term, size_t opt_len, caddr_t opt_ptr, mblk_t *dp)
{
	struct TR_end_req *p;
	mblk_t *mp;

	if (unlikely(!canputnext(tr->oq)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TR_END_REQ;
	p->TRANS_id = tid;
	p->TERM_scenario = term;
	p->OPT_length = opt_len;
	p->OPT_offset = sizeof(*p);
	mp->b_wptr += sizeof(*p);
	bcopy(opt_ptr, mp->b_wptr, opt_len);
	mp->b_wptr += opt_len;
	mp->b_cont = dp;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(q, STRLOGDA, SL_TRACE, "TR_END_REQ ->");
	putnext(tr->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
}

/**
 * tr_abort_req: - issue TR_ABORT_REQ primitive
 * @tr: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @tid: transaction id
 * @cause: abort cause
 * @opt_len: options length
 * @opt_ptr: options pointer
 * @dp: user data (dialog and component portions)
 */
static inline int
tr_abort_req(struct tr *tr, queue_t *q, mblk_t *msg, struct xa *xa,
	     t_uscalar_t tid, t_uscalar_t cause, size_t opt_len, caddr_t opt_ptr, mblk_t *dp)
{
	struct TR_abort_req *p;
	mblk_t *mp;

	if (unlikely(!canputnext(tr->oq)))
		goto ebusy;
	if (unlikely(!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))))
		goto enobufs;
	DB_TYPE(mp) = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = TR_ABORT_REQ;
	p->TRANS_id = tid;
	p->ABORT_cause = cause;
	p->OPT_length = opt_len;
	p->OPT_offset = sizeof(*p);
	mp->b_wptr += sizeof(*p);
	bcopy(opt_ptr, mp->b_wptr, opt_len);
	mp->b_wptr += opt_len;
	mp->b_cont = dp;
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	mi_strlog(q, STRLOGTX, SL_TRACE, "TR_ABORT_REQ ->");
	putnext(tr->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
      ebusy:
	return (-EBUSY);
}

/*
 *  =========================================================================
 *
 *  PROTOCOL STATE MACHINE
 *
 *  =========================================================================
 */

/* Message Types */
#define	TCAP_MT_UNI		1	/* Unidirectional */
#define	TCAP_MT_QWP		2	/* Query w/ permission */
#define	TCAP_MT_QWOP		3	/* Query w/o permission */
#define	TCAP_MT_CWP		4	/* Conversation w/ permission */
#define	TCAP_MT_CWOP		5	/* Conversation w/o permission */
#define	TCAP_MT_RESP		6	/* Response */
#define	TCAP_MT_ABORT		7	/* Abort */

/* Component Types */
#define	TCAP_CT_INVOKE_L	1	/* Invoke (Last) */
#define	TCAP_CT_INVOKE_NL	2	/* Invoke (Not Last) */
#define	TCAP_CT_RESULT_L	3	/* Return Result (Last) */
#define	TCAP_CT_RESULT_NL	4	/* Return Result (Not Last) */
#define	TCAP_CT_REJECT		5	/* Reject */
#define	TCAP_CT_ERROR		6	/* Error */

#define TCAP_TAG_UNIV_INT	2	/* UNIV Integer */
#define TCAP_TAG_UNIV_OSTR	4	/* UNIV Octet String */
#define TCAP_TAG_UNIV_OID	6	/* UNIV Object Id */
#define	TCAP_TAG_UNIV_PSEQ	16	/* UNIV Parameter Sequence */
#define TCAP_TAG_UNIV_UTC	17	/* UNIV Universal Time */
#define TCAP_TAG_UNIV_SEQ	48	/* UNIV Sequence */

#define	TCAP_TAG_TCAP_ACG	 1	/* TCAP ACG Indicators */
#define	TCAP_TAG_TCAP_STA	 2	/* TCAP Standard Announcement */
#define TCAP_TAG_TCAP_CUA	 3	/* TCAP Customized Announcment */
#define	TCAP_TAG_TCAP_TDIG	 4	/* TCAP Digits */
#define	TCAP_TAG_TCAP_SUEC	 5	/* TCAP Standard User Error Code */
#define	TCAP_TAG_TCAP_PDTA	 6	/* TCAP Problem Data */
#define	TCAP_TAG_TCAP_TCGPA	 7	/* TCAP SCCP Calling Party Address */
#define	TCAP_TAG_TCAP_TRSID	 8	/* TCAP Transaction ID */
#define	TCAP_TAG_TCAP_PCTY	 9	/* TCAP Package Type */
#define	TCAP_TAG_TCAP_SKEY	10	/* TCAP Service Key (Constructor) */
#define	TCAP_TAG_TCAP_BISTAT	11	/* TCAP Busy/Idle Status */
#define	TCAP_TAG_TCAP_CFSTAT	12	/* TCAP Call Forwarding Status */
#define	TCAP_TAG_TCAP_ORIGR	13	/* TCAP Origination Restrictions */
#define	TCAP_TAG_TCAP_TERMR	14	/* TCAP Terminating Restrictions */
#define	TCAP_TAG_TCAP_DNMAP	15	/* TCAP DN to Line Service TYpe Mapping */
#define	TCAP_TAG_TCAP_DURTN	16	/* TCAP Duration */
#define	TCAP_TAG_TCAP_RETD	17	/* TCAP Return Data (Constructor) */
#define	TCAP_TAG_TCAP_BCRQ	18	/* TCAP Bearer Capability Requested */
#define	TCAP_TAG_TCAP_BCSUP	19	/* TCAP Bearer Capability Supported */
#define	TCAP_TAG_TCAP_REFID	20	/* TCAP Reference Id */
#define	TCAP_TAG_TCAP_BGROUP	21	/* TCAP Business Group */
#define	TCAP_TAG_TCAP_SNI	22	/* TCAP Signalling Networks Identifier */
#define	TCAP_TAG_TCAP_MWIT	23	/* TCAP Message Waiting Indicator Type */

#define	TCAP_TAG_PRIV_UNI	 1	/* ANSI Unidirectional */
#define	TCAP_TAG_PRIV_QWP	 2	/* ANSI Query w/ permission */
#define	TCAP_TAG_PRIV_QWOP	 3	/* ANSI Query w/o permission */
#define	TCAP_TAG_PRIV_RESP	 4	/* ANSI Response */
#define	TCAP_TAG_PRIV_CWP	 5	/* ANSI Conversaion w/ permission */
#define	TCAP_TAG_PRIV_CWOP	 6	/* ANSI Conversaion w/o permission */
#define	TCAP_TAG_PRIV_TRSID	 7	/* ANSI Transaction Id */
#define	TCAP_TAG_PRIV_CSEQ	 8	/* ANSI Component Sequence */
#define	TCAP_TAG_PRIV_INKL	 9	/* ANSI Invoke (Last) */
#define	TCAP_TAG_PRIV_RRL	10	/* ANSI Return Result (Last) */
#define	TCAP_TAG_PRIV_RER	11	/* ANSI Return Error */
#define	TCAP_TAG_PRIV_REJ	12	/* ANSI Reject */
#define	TCAP_TAG_PRIV_INK	13	/* ANSI Invoke (Not Last) */
#define	TCAP_TAG_PRIV_RR	14	/* ANSI Result (Not Last) */
#define	TCAP_TAG_PRIV_CORID	15	/* ANSI Correlation Id(s) */
#define	TCAP_TAG_PRIV_NOPCO	16	/* ANSI National Operation Code */
#define	TCAP_TAG_PRIV_POPCO	17	/* ANSI Private Operation Code */
#define	TCAP_TAG_PRIV_PSET	18	/* ANSI Parameter Set */
#define TCAP_TAG_PRIV_NECODE	19	/* ANSI National Error Code */
#define TCAP_TAG_PRIV_PECODE	20	/* ANSI Private Error Code */
#define	TCAP_TAG_PRIV_PBCODE	21	/* ANSI Reject Problem Code */
#define	TCAP_TAG_PRIV_PSEQ	21	/* ANSI Parameter Sequence */
#define	TCAP_TAG_PRIV_ABORT	22	/* ANSI Abort */
#define	TCAP_TAG_PRIV_PCAUSE	23	/* ANSI P-Abort Cause */
#define	TCAP_TAG_PRIV_U_ABORT	24	/* ANSI User Abort Information */
#define	TCAP_TAG_PRIV_DLGP	25	/* ANSI Dialog Portion */
#define	TCAP_TAG_PRIV_VERSION	26	/* ANSI Protocol Version */
#define	TCAP_TAG_PRIV_CONTEXT	27	/* ANSI Integer Application Context */
#define	TCAP_TAG_PRIV_CTX_OID	28	/* ANSI OID Application Context */
#define	TCAP_TAG_PRIV_UINFO	29	/* ANSI User Information */

#define TCAP_TAG_APPL_UNI	1	/* ITUT Unidirectional */
#define TCAP_TAG_APPL_BEGIN	2	/* ITUT Begin Transaction */
#define TCAP_TAG_APPL_END	4	/* ITUT End Transaction */
#define TCAP_TAG_APPL_CONT	5	/* ITUT Continue Transaction */
#define TCAP_TAG_APPL_ABORT	7	/* ITUT Abort Transaction */
#define TCAP_TAG_APPL_ORIGID	8	/* ITUT Origination Transaction Id */
#define TCAP_TAG_APPL_DESTID	9	/* ITUT Destination Transaction Id */
#define TCAP_TAG_APPL_PCAUSE	10	/* ITUT P-Abort Cause */
#define TCAP_TAG_APPL_DLGP	11	/* ITUT Dialog Portion */
#define TCAP_TAG_APPL_CSEQ	12	/* ITUT Component Portion */

#define TCAP_TAG_APPL_AUDT_PDU	0	/* ITUT AUDT APDU */
#define TCAP_TAG_APPL_AARQ_PDU	0	/* ITUT AARQ APDU */
#define TCAP_TAG_APPL_AARE_PDU	1	/* ITUT AARE APDU */
#define TCAP_TAG_APPL_RLRQ_PDU	2	/* ITUT RLRQ APDU */
#define TCAP_TAG_APPL_RLRE_PDU	3	/* ITUT RLRE APDU */
#define TCAP_TAG_APPL_ABRT_PDU	4	/* ITUT ABRT APDU */

#define TCAP_TAG_CNTX_LID	0	/* Linked Id */
#define TCAP_TAG_CNTX_INK	1	/* Invoke */
#define TCAP_TAG_CNTX_RRL	2	/* Return Result (Last) */
#define TCAP_TAG_CNTX_RER	3	/* Return Error */
#define TCAP_TAG_CNTX_REJ	4	/* Reject */
#define TCAP_TAG_CNTX_RR	7	/* Return Result (Not Last) */

/*
 *  =========================================================================
 *
 *  TCAP SEND MESSAGE FUNCTIONS
 *
 *  =========================================================================
 */

static inline int
xa_send_uni_req(struct xa *xa, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}
static inline int
xa_send_begin_req(struct xa *xa, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}
static inline int
xa_send_begin_res(struct xa *xa, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}
static inline int
xa_send_cont_req(struct xa *xa, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}
static inline int
xa_send_end_req(struct xa *xa, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}
static inline int
xa_send_abort_req(struct xa *xa, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/* ---------------------------------- */

/**
 * tc_send_uni_req: - process received TC_UNI_REQ primitive
 * @tc: private structure
 * @q: active queue (write queue)
 * @mp: the TC_UNI_REQ primitive
 */
static inline int
tc_send_uni_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/**
 * tc_send_begin_req: - process received TC_BEGIN_REQ primitive
 * @tc: private structure
 * @q: active queue (write queue)
 * @mp: the TC_BEGIN_REQ primitive
 */
static inline int
tc_send_begin_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/**
 * tc_send_begin_res: - process received TC_BEGIN_RES primitive
 * @tc: private structure
 * @q: active queue (write queue)
 * @mp: the TC_BEGIN_RES primitive
 */
static inline int
tc_send_begin_res(struct tc *tc, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/**
 * tc_send_cont_req: - process received TC_CONT_REQ primitive
 * @tc: private structure
 * @q: active queue (write queue)
 * @mp: the TC_CONT_REQ primitive
 */
static inline int
tc_send_cont_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/**
 * tc_send_end_req: - process received TC_END_REQ primitive
 * @tc: private structure
 * @q: active queue (write queue)
 * @mp: the TC_END_REQ primitive
 */
static inline int
tc_send_end_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/**
 * tc_send_abort_req: - process received TC_ABORT_REQ primitive
 * @tc: private structure
 * @q: active queue (write queue)
 * @mp: the TC_ABORT_REQ primitive
 */
static inline int
tc_send_abort_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  TCAP RECEIVE MESSAGE FUNCTIONS
 *
 *  =========================================================================
 */

static inline int
xa_recv_uni_ind(struct xa *xa, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}
static inline int
xa_recv_begin_ind(struct xa *xa, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}
static inline int
xa_recv_begin_con(struct xa *xa, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}
static inline int
xa_recv_cont_ind(struct xa *xa, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}
static inline int
xa_recv_end_ind(struct xa *xa, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}
static inline int
xa_recv_abort_ind(struct xa *xa, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/* ---------------------------------- */

#define TF_VAR_TRP	(1<<2)
#define TF_VAR_IDS	(1<<3)
#define TF_VAR_OID	(1<<0)
#define TF_VAR_TID	(1<<1)
#define TF_VAR_DLG	(1<<4)
#define TF_VAR_CMP	(1<<5)
#define TF_VAR_ABT	(1<<6)
#define TF_VAR_INF	(1<<7)

struct tr_msg {
	uint flags;
	struct tc_var trp;		/* trasnsaction portion */
	struct tc_var ids;		/* transaction ids */
	struct tc_var oid;		/* originating transaction id */
	struct tc_var did;		/* terminating transaction id */
	struct tc_var dlg;		/* dialog portion */
	struct tc_var cmp;		/* component portion */
	struct tc_var abt;		/* P-Abort Cause */
	struct tc_var inf;		/* User Information */
};

static noinline fastcall __unlikely int
tc_unpack_tag_extended(uchar **p, uchar **e, uint * tag)
{
	uchar ptag;

	*tag = 0;
	if (*p >= *e)
		goto emsgsize;
	ptag = **p;
	(*p)++;
	*tag |= (ptag & 0x7f);
	if (ptag & 0x80) {
		if (*p >= *e)
			goto emsgsize;
		*tag <<= 7;
		ptag = **p;
		(*p)++;
		*tag |= (ptag & 0x7f);
		if (ptag & 0x80) {
			if (*p >= *e)
				goto emsgsize;
			*tag <<= 7;
			ptag = **p;
			(*p)++;
			*tag |= (ptag & 0x7f);
			if (ptag & 0x80) {
				if (*p >= *e)
					goto emsgsize;
				*tag <<= 7;
				ptag = **p;
				(*p)++;
				*tag |= (ptag & 0x7f);
				if (ptag & 0x80)
					goto eproto;
			}
		}
	}
	return (0);
      eproto:
	return (-EPROTO);
      emsgsize:
	return (-EMSGSIZE);
}

static noinline fastcall __unlikely int
tc_unpack_len_extended(uchar **p, uchar **e, uint * len)
{
	uint plen = *len & 0x7f;

	if (plen > 4 || plen == 0)
		goto eproto;
	*len = 0;
	while (plen--) {
		if (*p >= *e)
			goto emsgsize;
		*len |= **p;
		(*p)++;
		*len <<= 8;
	}
	return (0);
      emsgsize:
	return (-EMSGSIZE);
      eproto:
	return (-EPROTO);
}

static noinline fastcall __hot_in int
tc_unpack_taglen(uchar **p, uchar **e, uint * tag, uint * cls)
{
	uint len, tac;
	int err;

	if (unlikely(*p >= *e))
		goto emsgsize;
	if (unlikely((*tag = ((tac = *(*p)++) & 0x1f)) == 0x1f)
	    && unlikely((err = tc_unpack_tag_extended(p, e, tag))))
		goto error;
	*cls = (tac & 0xe0);
	if (unlikely(*p >= *e))
		goto emsgsize;
	if (unlikely(((len = *(*p)++) & 0x80) == 0x80)
	    && unlikely((err = tc_unpack_len_extended(p, e, &len))))
		goto error;
	if (*p + len > *e)
		goto einval;
	*e = *p + len;
	return (0);
      einval:
	err = -EINVAL;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return (err);
}

/**
 * tc_unpack_comps: - unpack the dialog and component portion of the message
 * @tc: private structure
 * @q: active queue (read queue)
 * @mp: the TR-Primitive
 * @m: the representation of the message
 *
 * This function unpacks the transaction portion.  Note that we leave the constructor for the
 * transaction portion in the transaction portion so that we can determine whether the message is an
 * application-wide or private structure.
 */
static int
tc_unpack_comps(struct tc *tc, queue_t *q, mblk_t *mp, struct tr_msg *m)
{
	uchar *s, *l, *p, *e;
	uint tag, cls;
	int err;

	/* unpack first primitive or constructor */
	s = p = m->trp.ptr;
	l = e = s + m->trp.len;
	for (; s < l; s = p = e, e = l) {
		if ((err = tc_unpack_taglen(&p, &e, &tag, &cls)))
			goto error;
		switch (cls) {
		case 0x00:	/* universal primitive - class 0 form 0 */
			break;
		case 0x20:	/* universal constructor - class 0 form 1 */
			break;
		case 0x80:	/* application-wide primitive - class 1 form 0 */
			break;
		case 0xa0:	/* application-wide constructor - class 1 form 1 */
			break;
		case 0x40:	/* context-specific primitive - class 2 form 0 */
			break;
		case 0x60:	/* context-specific constructor - class 2 form 1 */
			break;
		case 0xc0:	/* private primitive - class 3 form 0 */
			switch (tag) {
			case TCAP_TAG_PRIV_TRSID:	/* transaction ids */
				switch (e - p) {
				case 0:
				case 4:
				case 8:
				default:
					err = EPROTO;
					goto error;
				}
				break;
			}
			break;
		case 0xe0:	/* private constructor - class 3 form 1 */
			switch (tag) {
			case TCAP_TAG_PRIV_DLGP:	/* dialog portion */
			case TCAP_TAG_PRIV_CSEQ:	/* component sequence */
				break;
			}
			break;
		}
	}
      error:
	fixme(("Deal with error."));
	freemsg(mp);
	return (0);
}

/**
 * tc_recv_uni_ind: - process received TR_UNI_IND
 * @tc: private structure
 * @q: active queue (read queue)
 * @mp: the TR_UNI_IND primitive
 *
 * We have received a unidirectional indication (TR_UNI_IND) and it has been checked.
 */
static int
tc_recv_uni_ind(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct tr_msg m = { 0U, };
	mblk_t *dp = mp->b_cont;
	int err;

	m.trp.ptr = dp->b_rptr;
	m.trp.len = dp->b_wptr - dp->b_rptr;
	m.flags |= TF_VAR_TRP;

	if ((err = tc_unpack_comps(tc, q, mp, &m)))
		return (err);
	return (0);
}

/**
 * tc_recv_begin_ind: - process received TR_BEGIN_IND
 * @tc: private structure
 * @q: active queue (read queue)
 * @mp: the TR_BEGIN_IND primitive
 *
 * Ok, we have a begin indication.  Tear apart the trasaction portion.  The transaction portion
 * consists of an originating trasnaction id, possibly dialog portion and possibly a component
 * portion.  Create a trasnaction object and complete the originating transaction id and originating
 * address information.
 */
static int
tc_recv_begin_ind(struct tc *tc, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/**
 * tc_recv_begin_con: - process received TR_BEGIN_CON
 * @tc: private structure
 * @q: active queue (read queue)
 * @mp: the TR_BEGIN_CON primitive
 */
static int
tc_recv_begin_con(struct tc *tc, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/**
 * tc_recv_cont_ind: - process received TR_CONT_IND
 * @tc: private structure
 * @q: active queue (read queue)
 * @mp: the TR_CONT_IND primitive
 */
static int
tc_recv_cont_ind(struct tc *tc, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/**
 * tc_recv_end_ind: - process received TR_END_IND
 * @tc: private structure
 * @q: active queue (read queue)
 * @mp: the TR_END_IND primitive
 */
static int
tc_recv_end_ind(struct tc *tc, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

/**
 * tc_recv_abort_ind: - process received TR_ABORT_IND
 * @tc: private structure
 * @q: active queue (read queue)
 * @mp: the TR_ABORT_IND primitive
 */
static int
tc_recv_abort_ind(struct tc *tc, queue_t *q, mblk_t *mp)
{
	/* FIXME */
	freemsg(mp);
	return (0);
}

static inline int
tc_recv_msg(struct tc *tc, queue_t *q, mblk_t *mp)
{
	int err = 0;

	switch (tc_get_state(tc)) {
	case TCS_UNBND:
		goto discard;
	case TCS_WACK_BREQ:
	case TCS_WACK_UREQ:
		goto eagain;
	case TCS_IDLE:
		switch (*(t_uscalar_t *) mp->b_rptr) {
		case TR_UNI_IND:
			return tc_recv_uni_ind(tc, q, mp);
		case TR_BEGIN_IND:
			return tc_recv_begin_ind(tc, q, mp);
		case TR_BEGIN_CON:
			return tc_recv_begin_con(tc, q, mp);
		case TR_CONT_IND:
			return tc_recv_cont_ind(tc, q, mp);
		case TR_END_IND:
			return tc_recv_end_ind(tc, q, mp);
		case TR_ABORT_IND:
			return tc_recv_abort_ind(tc, q, mp);
		}
		goto badprim;
	}
	goto outstate;
      outstate:
	mi_strlog(q, STRLOGRX, SL_TRACE, "Message received in state %s.",
		  tc_statename(tc_get_state(tc)));
	tc_set_state(tc, TCS_NOSTATES);
	if ((err = tr_info_req(tc->tr, q, NULL)) == 0)
		goto eagain;
	goto error;
      eagain:
	mi_strlog(q, STRLOGST, SL_TRACE, "Waiting for state synchronization.");
	noenable(q);
	err = -EAGAIN;
	goto error;
      discard:
	mi_strlog(q, STRLOGRX, SL_TRACE, "Discarding message in unbound state.");
	goto done;
      badprim:
	mi_strlog(q, STRLOGRX, SL_TRACE, "Incorrect primitive received %s.",
		  tr_primname(*(t_uscalar_t *) mp->b_rptr));
	goto done;
      done:
	freemsg(mp);
	err = 0;
	goto error;
      error:
	return (err);
}

/*
 *  =========================================================================
 *
 *  RECEIVED PRIMITIVES
 *
 *  =========================================================================
 */
/*
 *  =========================================================================
 *
 *  TC User -> TC Provider primitives.
 *
 *  =========================================================================
 */

/**
 * tc_info_req: - process TC_INFO_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_INFO_REQ primitive
 */
static int
tc_info_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_info_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_bind_req: - process TC_BIND_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_BIND_REQ primitive
 */
static int
tc_bind_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_bind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_unbind_req: - process TC_UNBIND_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_UNBIND_REQ primitive
 */
static int
tc_unbind_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_unbind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_subs_bind_req: - process TC_SUBS_BIND_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_SUBS_BIND_REQ primitive
 */
static int
tc_subs_bind_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_subs_bind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_subs_unbind_req: - process TC_SUBS_UNBIND_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_SUBS_UNBIND_REQ primitive
 */
static int
tc_subs_unbind_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_subs_unbind_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_optmgmt_req: - process TC_OPTMGMT_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_OPTMGMT_REQ primitive
 */
static int
tc_optmgmt_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	struct tc_opts opts = { 0U, };
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (p->OPT_length) {
		if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
			goto badprim;
		if (tc_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
      badopt:
	err = TCBADOPT;
	goto error;
      badprim:
	err = -EINVAL;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_uni_req: - process TC_UNI_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_UNI_REQ primitive
 */
static int
tc_uni_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_uni_req *p = (typeof(p)) mp->b_rptr;
	struct tc_opts opts = { 0U, };
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (p->OPT_length) {
		if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
			goto badprim;
		if (tc_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
      badopt:
	err = TCBADOPT;
	goto error;
      badprim:
	err = -EINVAL;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_begin_req: - process TC_BEGIN_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_BEGIN_REQ primitive
 */
static int
tc_begin_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_begin_req *p = (typeof(p)) mp->b_rptr;
	struct tc_opts opts = { 0U, };
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (p->OPT_length) {
		if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
			goto badprim;
		if (tc_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
      badopt:
	err = TCBADOPT;
	goto error;
      badprim:
	err = -EINVAL;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_begin_res: - process TC_BEGIN_RES primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_BEGIN_RES primitive
 */
static int
tc_begin_res(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_begin_req *p = (typeof(p)) mp->b_rptr;
	struct tc_opts opts = { 0U, };
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto emsgsize;
	if (p->OPT_length) {
		if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
			goto badprim;
		if (tc_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
      badopt:
	err = TCBADOPT;
	goto error;
      badprim:
	err = -EINVAL;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_cont_req: - process TC_CONT_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_CONT_REQ primitive
 */
static int
tc_cont_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_cont_req *p = (typeof(p)) mp->b_rptr;
	struct tc_opts opts = { 0U, };
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (p->OPT_length) {
		if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
			goto badprim;
		if (tc_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
      badopt:
	err = TCBADOPT;
	goto error;
      badprim:
	err = -EINVAL;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_end_req: - process TC_END_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_END_REQ primitive
 */
static int
tc_end_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_end_req *p = (typeof(p)) mp->b_rptr;
	struct tc_opts opts = { 0U, };
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (p->OPT_length) {
		if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
			goto badprim;
		if (tc_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
      badopt:
	err = TCBADOPT;
	goto error;
      badprim:
	err = -EINVAL;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_abort_req: - process TC_ABORT_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_ABORT_REQ primitive
 */
static int
tc_abort_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_abort_req *p = (typeof(p)) mp->b_rptr;
	struct tc_opts opts = { 0U, };
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (p->OPT_length) {
		if (!MBLKIN(mp, p->OPT_offset, p->OPT_length))
			goto badprim;
		if (tc_parse_opts(tc, &opts, mp->b_rptr + p->OPT_offset, p->OPT_length))
			goto badopt;
	}
      badopt:
	err = TCBADOPT;
	goto error;
      badprim:
	err = -EINVAL;
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_invoke_req: - process TC_INVOKE_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_INVOKE_REQ primitive
 */
static int
tc_invoke_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_invoke_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_result_req: - process TC_RESULT_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_RESULT_REQ primitive
 */
static int
tc_result_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_result_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_error_req: - process TC_ERROR_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_ERROR_REQ primitive
 */
static int
tc_error_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_error_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_cancel_req: - process TC_CANCEL_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_CANCEL_REQ primitive
 */
static int
tc_cancel_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_cancel_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_reject_req: - process TC_REJECT_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_REJECT_REQ primitive
 */
static int
tc_reject_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct TC_reject_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, p->PRIM_type, err, 0, 0);
}

/**
 * tc_other_req: - process TC_????_REQ primitive
 * @tr: private structure
 * @q: active queue (write queue)
 * @mp: the TC_????_REQ primitive
 */
static int
tc_other_req(struct tc *tc, queue_t *q, mblk_t *mp)
{
	t_uscalar_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = -EMSGSIZE;
	goto error;
      error:
	return tc_error_ack(tc, q, mp, *p, err, 0, 0);
}

/*
 *  =========================================================================
 *
 *  TR Provider -> TR User primitives.
 *
 *  =========================================================================
 */

/**
 * tr_info_ack: - process TR_INFO_ACK primitive
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the TR_INFO_ACK primitive
 *
 * The TR_INFO_ACK is invoked when the module is first pushed or when synchronization is a good
 * idea.  Receipt of this primitive results in updating the transaction provider information and
 * syncrhonizing the transaction provider state.  If the previous state was TRS_NOSTATES, it means
 * that we have not syncrhonized for the first time and we need to perform an enableok() to start
 * regular traffic flowing.  We perform a noenable() when the module is first pushed to keep data
 * transactions from being delivered upstream until the TR_ADDR_ACK and TR_INFO_ACK arrive.
 */
static int
tr_info_ack(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_info_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	tr->info = *p;
	if (tr_get_state(tr) == TRS_NOSTATES)
		enableok(q);
	tr_set_state(tr, p->CURRENT_state);
	freemsg(mp);
	return (0);
      emsgsize:
	err = EFAULT;
	goto error;
      error:
	return m_error(tr->tc, q, mp, err);
}

/**
 * tr_addr_ack: - process TR_ADDR_ACK primitmive
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the TR_ADDR_ACK primitive
 *
 * We issue a TR_ADDR_REQ message to illicit a TR_ADDR_ACK message when we first push the module.
 * The proper order is to request addresses first and then request information second.  Therefore,
 * we do not enable the interface when we receive the TR_ADDR_ACK, only when we receive the
 * TR_INFO_ACK.
 */
static int
tr_addr_ack(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_addr_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (p->LOCADDR_length) {
		if (mp->b_wptr < mp->b_rptr + p->LOCADDR_offset + p->LOCADDR_length)
			goto emsgsize;
		if (p->LOCADDR_length != sizeof(tr->loc))
			goto ebadaddr;
		bcopy(mp->b_rptr + p->LOCADDR_offset, &tr->loc, p->LOCADDR_length);
	}
	if (p->REMADDR_length) {
		if (mp->b_wptr < mp->b_rptr + p->REMADDR_offset + p->REMADDR_length)
			goto emsgsize;
		if (p->REMADDR_length != sizeof(tr->rem))
			goto ebadaddr;
		bcopy(mp->b_rptr + p->REMADDR_offset, &tr->rem, p->REMADDR_length);
	}
	freemsg(mp);
	return (0);
      ebadaddr:
	err = EFAULT;
	goto error;
      emsgsize:
	err = EFAULT;
	goto error;
      error:
	return m_error(tr->tc, q, mp, err);
}

/**
 * tr_bind_ack: - process TR_BIND_ACK primitive
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the TR_BIND_ACK primitive
 *
 * This primitive should only appear in the proper state.  If it appears otherwise, then the module
 * has been pushed over a Stream that is binding.  Always update the bound address and move the
 * interface to the TRS_IDLE state.
 */
static int
tr_bind_ack(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_bind_ack *p = (typeof(p)) mp->b_rptr;
	struct tc *tc = tr->tc;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (p->ADDR_length) {
		if (mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length)
			goto emsgsize;
		if (p->ADDR_length != sizeof(tr->loc))
			goto badaddr;
		bcopy(mp->b_rptr + p->ADDR_offset, &tr->loc, p->ADDR_length);
	}
	tr->xacts = p->XACT_number;
	tr->token = p->TOKEN_value;
	if (tr_get_state(tr) != TRS_WACK_BREQ)
		goto outstate;
	tr_set_state(tr, TRS_IDLE);
	if (tc_get_state(tc) == TCS_WACK_BREQ)
		return tc_bind_ack(tc, q, mp,
				   p->XACT_number, p->ADDR_length, mp->b_rptr + p->ADDR_offset);
	if (tc_get_state(tc) != TCS_IDLE)
		goto outstate;
	freemsg(mp);
	return (0);
      outstate:
	noenable(q);
	tr_set_state(tr, TRS_NOSTATES);
	tc_set_state(tc, TCS_NOSTATES);
	return tc_info_req(tc, q, mp);	/* try to sync state */
      badaddr:
	err = EFAULT;
	goto error;
      emsgsize:
	err = EFAULT;
	goto error;
      error:
	return m_error(tc, q, mp, err);
}

/**
 * tr_ok_ack: - process TR_OK_ACK primitive
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the TR_OK_ACK primitive
 *
 * The only time that we receive a TR_OK_ACK is in response to a TR_UNBIND_REQ.  The TR_UNBIND_REQ
 * is issued in response to a TC_UNBIND_REQ.  The ack needs to be propagated up to the TC User.
 */
static int
tr_ok_ack(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_ok_ack *p = (typeof(p)) mp->b_rptr;
	struct tc *tc = tr->tc;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (tr_get_state(tr) != TRS_WACK_UREQ && p->CORRECT_prim != TR_UNBIND_REQ)
		goto outstate;
	tr_set_state(tr, TRS_UNBND);
	if (tc_get_state(tc) == TCS_WACK_UREQ)
		return tc_ok_ack(tc, q, mp, TC_UNBIND_REQ);
	if (tc_get_state(tc) != TCS_UNBND)
		goto outstate;
	freemsg(mp);
	return (0);
      outstate:
	/* Attempt to syncrhonize state. */
	noenable(q);
	return tr_info_req(tr, q, mp);
      emsgsize:
	err = EFAULT;
	goto error;
      error:
	return m_error(tc, q, mp, err);
}

/**
 * tr_error_ack: - process TR_ERROR_ACK primitive
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the TR_ERROR_ACK primitive
 */
static int
tr_error_ack(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_error_ack *p = (typeof(p)) mp->b_rptr;
	struct tc *tc = tr->tc;
	t_scalar_t error;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	error = p->TRPI_error != TRSYSERR ? p->TRPI_error : -p->UNIX_error;
	switch (tr_get_state(tr)) {
	case TRS_WACK_BREQ:
		tr_set_state(tr, TRS_UNBND);
		if (tc_get_state(tc) == TCS_WACK_BREQ)
			return tc_error_ack(tc, q, mp, TC_BIND_REQ, error, 0, 0);
		if (tc_get_state(tc) != TCS_UNBND)
			goto outstate;
		break;
	case TRS_WACK_UREQ:
		tr_set_state(tr, TRS_IDLE);
		if (tc_get_state(tc) == TCS_WACK_UREQ)
			return tc_error_ack(tc, q, mp, TC_UNBIND_REQ, error, 0, 0);
		if (tc_get_state(tc) != TCS_IDLE)
			goto outstate;
		break;
	default:
		goto outstate;
	}
	freemsg(mp);
	return (0);
      outstate:
	noenable(q);
	tr_set_state(tr, TRS_NOSTATES);
	tc_set_state(tc, TRS_NOSTATES);
	return tr_info_req(tr, q, mp);	/* try to sync state */
      emsgsize:
	err = EFAULT;
	goto error;
      error:
	return m_error(tc, q, mp, err);
}

/**
 * tr_optmgmt_ack: - process TR_OPTMGMT_ACK primitive
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the TR_OPTMGMT_ACK primitive
 *
 * Options management is passed directly through.
 */
static int
tr_optmgmt_ack(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_optmgmt_ack *p = (typeof(p)) mp->b_rptr;
	struct tc *tc = tr->tc;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (p->OPT_length) {
		if (mp->b_wptr < mp->b_rptr + p->OPT_offset + p->OPT_length)
			goto badopt;
	}
	/* FIXME: there might be more to do here, such as respond to the TC only option portion. */
	return tc_optmgmt_ack(tc, q, mp, p->MGMT_flags, p->OPT_length, mp->b_rptr + p->OPT_offset);
      badopt:
	err = EFAULT;
	goto error;
      emsgsize:
	err = EFAULT;
	goto error;
      error:
	return m_error(tr->tc, q, mp, err);
}

/**
 * tr_uni_ind: - process TR_UNI_IND primitive
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the TR_UNI_IND primitive
 *
 * TR_UNI_IND primitives are passed directly through as long as the TC interface is in the correct
 * state.  Any TR_UNI_IND primitives not in the idle state will simply be discarded, however, if we
 * are in an uninitialized state then we might as well noenable() the queue, put the message back on
 * the queue and issue a syncrhonization request.
 */
static int
tr_uni_ind(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_uni_ind *p = (typeof(p)) mp->b_rptr;
	struct tc *tc = tr->tc;
	mblk_t *dp;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if ((dp = mp->b_cont) == NULL)
		goto discard;
	if (!pullupmsg(dp, -1))
		goto noresource;
	switch (tr_get_state(tr)) {
	case TRS_UNBND:
		goto discard;
	case TRS_WACK_BREQ:
	case TRS_WACK_UREQ:
		goto eagain;
	case TRS_IDLE:
		switch (tc_get_state(tc)) {
		case TCS_UNBND:
			goto discard;
		case TCS_WACK_BREQ:
		case TCS_WACK_UREQ:
			goto eagain;
		case TCS_IDLE:
			/* FIXME: Ok, we have a valid deliverable transaction.  Create a
			   transaction object (not necessary). Check the dialog and component
			   portion for proper formatting.  Create on invoke object (unnecessary)
			   for each invoke in the component portion.  Create a TC_INVOKE_IND
			   primitive for each invoke component.  Create a TC_UNI_IND primitive and
			   attach the dialog portion and pass it the chain of TC_INVOKE_IND
			   primitives. */
			return tc_recv_uni_ind(tc, q, mp);
		default:
		case TCS_NOSTATES:
			goto outstate;
		}
	default:
	case TRS_NOSTATES:
		goto outstate;
	}
      discard:
	freemsg(mp);
	return (0);
      noresource:
	/* we do not respond to unidirectionals */
	freemsg(mp);
	return (0);
      outstate:
	noenable(q);
	if ((err = tc_info_req(tc, q, NULL)) == 0)
		return (-EAGAIN);
	return (err);
      eagain:
	noenable(q);
	return (-EAGAIN);
      emsgsize:
	err = EFAULT;
	goto error;
      error:
	return m_error(tr->tc, q, mp, err);
}

/**
 * tr_begin_ind: - process TR_BEGIN_IND primitive
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the TR_BEGIN_IND primitive
 *
 * When we get a TR_BEGIN_IND, examine the dialog portion.  If the dialog portion contains an
 * acceptable application context, create a dialog and assign it the transaction id as the dialogue
 * id and deliver a TC_BEGIN_IND followed by components, one for each component present.
 *
 * For the module, we pass TR_BEGIN_IND almost straight through.  For the multiplexer, we would
 * examine the dialog portion and determine which upper TC User stream to deliver the transaction
 * to.
 */
static int
tr_begin_ind(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_begin_ind *p = (typeof(p)) mp->b_rptr;
	mblk_t *dp;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if ((dp = mp->b_cont) == NULL)
		goto discard;
	if (!pullupmsg(dp, -1))
		goto noresource;
	switch (tr_get_state(tr)) {
	case TRS_UNBND:
		goto discard;
	case TRS_WACK_BREQ:
	case TRS_WACK_UREQ:
		goto eagain;
	case TRS_IDLE:
		switch (tr_get_state(tr)) {
		case TRS_UNBND:
			break;
		}
	}
      discard:
	freemsg(mp);
	return (0);
      noresource:
	err = ENOMEM;
	goto error;
      eagain:
	return (-EAGAIN);
      emsgsize:
	err = EFAULT;
	goto error;
      error:
	return m_error(tr->tc, q, mp, err);
}

/**
 * tr_begin_con: - process TR_BEGIN_CON primitive
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the TR_BEGIN_CON primitive
 */
static int
tr_begin_con(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_begin_con *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = EFAULT;
	goto error;
      error:
	return m_error(tr->tc, q, mp, err);
}

/**
 * tr_cont_ind: - process TR_CONT_IND primitive
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the TR_CONT_IND primitive
 */
static int
tr_cont_ind(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_cont_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = EFAULT;
	goto error;
      error:
	return m_error(tr->tc, q, mp, err);
}

/**
 * tr_end_ind: - process TR_END_IND primitive
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the TR_END_IND primitive
 */
static int
tr_end_ind(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_end_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = EFAULT;
	goto error;
      error:
	return m_error(tr->tc, q, mp, err);
}

/**
 * tr_abort_ind: - process TR_ABORT_IND primitive
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the TR_ABORT_IND primitive
 */
static int
tr_abort_ind(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_abort_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = EFAULT;
	goto error;
      error:
	return m_error(tr->tc, q, mp, err);
}

/**
 * tr_notice_ind: - process TR_NOTICE_IND primitive
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the TR_NOTICE_IND primitive
 */
static int
tr_notice_ind(struct tr *tr, queue_t *q, mblk_t *mp)
{
	struct TR_notice_ind *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = EFAULT;
	goto error;
      error:
	return m_error(tr->tc, q, mp, err);
}

/**
 * tr_other_ind: - process TR_????_IND primitive
 * @tr: private structure
 * @q: active queue (read queue)
 * @mp: the TR_????_IND primitive
 */
static int
tr_other_ind(struct tr *tr, queue_t *q, mblk_t *mp)
{
	t_uscalar_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
      emsgsize:
	err = EFAULT;
	goto error;
      error:
	return m_error(tr->tc, q, mp, err);
}

/*
 *  =========================================================================
 *
 *  TCAP INPUT-OUTPUT CONTROLS
 *
 *  =========================================================================
 */

static int
tc_testoption(struct tc *tc, struct tcap_option *arg)
{
	/* FIXME: check options */
	return (0);
}
static int
tc_setoption(struct tc *tc, struct tcap_option *arg)
{
	int err;

	if ((err = tc_testoption(tc, arg)) != 0)
		return (err);
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		df.config = arg->options[0].df;
		break;
	case TCAP_OBJ_TYPE_SP:
		tc->tr->sp.config = arg->options[0].sp;
		break;
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		err = EINVAL;
		break;
	}
	return (0);
}
static int
tc_testconfig(struct tc *tc, struct tcap_config *arg)
{
	/* FIXME: check configuration */
	return (0);
}
static int
tc_setconfig(struct tc *tc, struct tcap_config *arg)
{
	int err;

	if ((err = tc_testconfig(tc, arg)) != 0)
		return (err);
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		// df.config = arg->config[0].df;
		break;
	case TCAP_OBJ_TYPE_SP:
		// tc->sp.config = arg->config[0].sp;
		break;
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (EINVAL);
	}
	return (0);
}
static int
tc_setstatsp(struct tc *tc, struct tcap_stats *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		df.statsp = arg->stats[0].df;
		break;
	case TCAP_OBJ_TYPE_SP:
		tc->tr->sp.statsp = arg->stats[0].sp;
		break;
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (EINVAL);
	}
	return (0);
}
static int
tc_setnotify(struct tc *tc, struct tcap_notify *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		df.notify.events |= arg->notify[0].df.events;
		arg->notify[0].df.events = df.notify.events;
		break;
	case TCAP_OBJ_TYPE_SP:
		tc->tr->sp.notify.events |= arg->notify[0].sp.events;
		arg->notify[0].sp.events = tc->tr->sp.notify.events;
		break;
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (EINVAL);
	}
	return (0);
}
static int
tc_clrnotify(struct tc *tc, struct tcap_notify *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		df.notify.events &= ~arg->notify[0].df.events;
		arg->notify[0].df.events = df.notify.events;
		break;
	case TCAP_OBJ_TYPE_SP:
		tc->tr->sp.notify.events &= ~arg->notify[0].sp.events;
		arg->notify[0].sp.events = tc->tr->sp.notify.events;
		break;
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (EINVAL);
	}
	return (0);
}

static int
tc_size_options(struct tc *tc, struct tcap_option *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (sizeof(df.config));
	case TCAP_OBJ_TYPE_SP:
		return (sizeof(tc->tr->sp.config));
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (0);
	}
}
static int
tc_size_config(struct tc *tc, struct tcap_config *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (sizeof(df.config));
	case TCAP_OBJ_TYPE_SP:
		return (sizeof(tc->tr->sp.config));
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (0);
	}
}
static int
tc_size_statem(struct tc *tc, struct tcap_statem *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (sizeof(df.statem));
	case TCAP_OBJ_TYPE_SP:
		return (sizeof(tc->tr->sp.statem));
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (0);
	}
}
static int
tc_size_statsp(struct tc *tc, struct tcap_stats *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (sizeof(df.statsp));
	case TCAP_OBJ_TYPE_SP:
		return (sizeof(tc->tr->sp.statsp));
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (0);
	}
}
static int
tc_size_stats(struct tc *tc, struct tcap_stats *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (sizeof(df.stats));
	case TCAP_OBJ_TYPE_SP:
		return (sizeof(tc->tr->sp.stats));
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (0);
	}
}
static int
tc_size_notify(struct tc *tc, struct tcap_notify *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (sizeof(df.notify));
	case TCAP_OBJ_TYPE_SP:
		return (sizeof(tc->tr->sp.notify));
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (0);
	}
}

static void *
tc_ptr_options(struct tc *tc, struct tcap_option *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (&df.config);
	case TCAP_OBJ_TYPE_SP:
		return (&tc->tr->sp.config);
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (NULL);
	}
}
static void *
tc_ptr_config(struct tc *tc, struct tcap_config *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (&df.config);
	case TCAP_OBJ_TYPE_SP:
		return (&tc->tr->sp.config);
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (NULL);
	}
}
static void *
tc_ptr_statem(struct tc *tc, struct tcap_statem *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (&df.statem);
	case TCAP_OBJ_TYPE_SP:
		return (&tc->tr->sp.statem);
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (NULL);
	}
}
static void *
tc_ptr_statsp(struct tc *tc, struct tcap_stats *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (&df.statsp);
	case TCAP_OBJ_TYPE_SP:
		return (&tc->tr->sp.statsp);
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (NULL);
	}
}
static void *
tc_ptr_stats(struct tc *tc, struct tcap_stats *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (&df.stats);
	case TCAP_OBJ_TYPE_SP:
		return (&tc->tr->sp.stats);
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (NULL);
	}
}
static void *
tc_ptr_notify(struct tc *tc, struct tcap_notify *arg)
{
	switch (arg->type) {
	case TCAP_OBJ_TYPE_DF:
		return (&df.notify);
	case TCAP_OBJ_TYPE_SP:
		return (&tc->tr->sp.notify);
	default:
	case TCAP_OBJ_TYPE_SC:
	case TCAP_OBJ_TYPE_TC:
	case TCAP_OBJ_TYPE_IV:
	case TCAP_OBJ_TYPE_DG:
	case TCAP_OBJ_TYPE_TR:
		return (NULL);
	}
}

/**
 * tc_ioctl: - process M_IOCTL message
 * @tc: private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the TCAP input-output control operation.  Step 1 consists always consists of a
 * copyin operation to determine the object type and id.
 */
static __unlikely int
tc_ioctl(struct tc *tc, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", tc_iocname(ioc->ioc_cmd));

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(TCAP_IOCGOPTION):
	case _IOC_NR(TCAP_IOCSOPTION):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_option));
		break;
	case _IOC_NR(TCAP_IOCGCONFIG):
	case _IOC_NR(TCAP_IOCSCONFIG):
	case _IOC_NR(TCAP_IOCTCONFIG):
	case _IOC_NR(TCAP_IOCCCONFIG):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_config));
		break;
	case _IOC_NR(TCAP_IOCGSTATEM):
	case _IOC_NR(TCAP_IOCCMRESET):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_statem));
		break;
	case _IOC_NR(TCAP_IOCGSTATSP):
	case _IOC_NR(TCAP_IOCSSTATSP):
	case _IOC_NR(TCAP_IOCGSTATS):
	case _IOC_NR(TCAP_IOCCSTATS):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_stats));
		break;
	case _IOC_NR(TCAP_IOCGNOTIFY):
	case _IOC_NR(TCAP_IOCSNOTIFY):
	case _IOC_NR(TCAP_IOCCNOTIFY):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_notify));
		break;
	case _IOC_NR(TCAP_IOCCMGMT):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_mgmt));
		break;
	case _IOC_NR(TCAP_IOCCPASS):
		mi_copyin(q, mp, NULL, sizeof(struct tcap_pass));
		break;
	default:
		mi_copy_done(q, mp, EINVAL);
		break;
	}
	return (0);
}

/**
 * tc_copyin1: - process M_IOCDATA message
 * @tc: private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 * @dp: the copyied in data
 *
 * This is step 2 of the TCAP input-output control operationl.  Step 2 consists of identifying the
 * object and performing a copyout for GET operations and an additional copyin for SET operations.
 */
static __unlikely int
tc_copyin1(struct tc *tc, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int alen, len, size = -1;
	int err = 0;
	mblk_t *bp;
	void *ptr;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", tc_iocname(cp->cp_cmd));

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(TCAP_IOCGOPTION):
		alen = sizeof(struct tcap_option);
		len = tc_size_options(tc, (struct tcap_option *) dp->b_rptr);
		ptr = tc_ptr_options(tc, (struct tcap_option *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(TCAP_IOCSOPTION):
		alen = sizeof(struct tcap_option);
		len = tc_size_options(tc, (struct tcap_option *) dp->b_rptr);
		goto copyin;
	case _IOC_NR(TCAP_IOCGCONFIG):
		alen = sizeof(struct tcap_config);
		len = tc_size_config(tc, (struct tcap_config *) dp->b_rptr);
		ptr = tc_ptr_config(tc, (struct tcap_config *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(TCAP_IOCSCONFIG):
	case _IOC_NR(TCAP_IOCTCONFIG):
	case _IOC_NR(TCAP_IOCCCONFIG):
		alen = sizeof(struct tcap_config);
		len = tc_size_config(tc, (struct tcap_config *) dp->b_rptr);
		goto copyin;
	case _IOC_NR(TCAP_IOCGSTATEM):
		alen = sizeof(struct tcap_statem);
		len = tc_size_statem(tc, (struct tcap_statem *) dp->b_rptr);
		ptr = tc_ptr_statem(tc, (struct tcap_statem *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(TCAP_IOCCMRESET):
		size = 0;
		switch (((struct tcap_statem *) dp->b_rptr)->type) {
		case TCAP_OBJ_TYPE_DF:
		case TCAP_OBJ_TYPE_SP:
		case TCAP_OBJ_TYPE_SC:
			/* FIXME: reset state machine */
			break;
		default:
		case TCAP_OBJ_TYPE_TC:
		case TCAP_OBJ_TYPE_IV:
		case TCAP_OBJ_TYPE_DG:
		case TCAP_OBJ_TYPE_TR:
			err = EINVAL;
			break;
		}
		break;
	case _IOC_NR(TCAP_IOCGSTATSP):
		alen = sizeof(struct tcap_stats);
		len = tc_size_statsp(tc, (struct tcap_stats *) dp->b_rptr);
		ptr = tc_ptr_statsp(tc, (struct tcap_stats *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(TCAP_IOCSSTATSP):
		alen = sizeof(struct tcap_stats);
		len = tc_size_statsp(tc, (struct tcap_stats *) dp->b_rptr);
		goto copyin;
	case _IOC_NR(TCAP_IOCGSTATS):
		alen = sizeof(struct tcap_stats);
		len = tc_size_stats(tc, (struct tcap_stats *) dp->b_rptr);
		ptr = tc_ptr_stats(tc, (struct tcap_stats *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(TCAP_IOCCSTATS):
		alen = sizeof(struct tcap_stats);
		len = tc_size_stats(tc, (struct tcap_stats *) dp->b_rptr);
		ptr = tc_ptr_stats(tc, (struct tcap_stats *) dp->b_rptr);
		goto copyout_clear;
	case _IOC_NR(TCAP_IOCGNOTIFY):
		alen = sizeof(struct tcap_notify);
		len = tc_size_notify(tc, (struct tcap_notify *) dp->b_rptr);
		ptr = tc_ptr_notify(tc, (struct tcap_notify *) dp->b_rptr);
		goto copyout;
	case _IOC_NR(TCAP_IOCSNOTIFY):
	case _IOC_NR(TCAP_IOCCNOTIFY):
		alen = sizeof(struct tcap_notify);
		len = tc_size_notify(tc, (struct tcap_notify *) dp->b_rptr);
		goto copyin;
	case _IOC_NR(TCAP_IOCCMGMT):
		size = 0;
		switch (((struct tcap_mgmt *) dp->b_rptr)->type) {
		case TCAP_OBJ_TYPE_DF:
		case TCAP_OBJ_TYPE_SP:
		case TCAP_OBJ_TYPE_SC:
			/* FIXME: perform command */
			break;
		default:
		case TCAP_OBJ_TYPE_TC:
		case TCAP_OBJ_TYPE_IV:
		case TCAP_OBJ_TYPE_DG:
		case TCAP_OBJ_TYPE_TR:
			err = EINVAL;
			break;
		}
		break;
	case _IOC_NR(TCAP_IOCCPASS):
		err = EOPNOTSUPP;
		break;
	default:
		err = EINVAL;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	      copyin:
		if (len == 0) {
			err = EINVAL;
			break;
		}
		size = alen + len;
		break;
	      copyout:
		if (len == 0 || ptr == NULL) {
			err = EINVAL;
			break;
		}
		if (!(bp = mi_copyout_alloc(q, mp, NULL, alen + len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, alen);
		bcopy(ptr, bp->b_rptr + alen, len);
		break;
	      copyout_clear:
		if (len == 0 || ptr == NULL) {
			err = EINVAL;
			break;
		}
		if (!(bp = mi_copyout_alloc(q, mp, NULL, alen + len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, alen);
		bcopy(ptr, bp->b_rptr + alen, len);
		bzero(ptr, len);
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == 0)
			mi_copy_done(q, mp, 0);
		else if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
}

/**
 * tc_copyin2: - process M_IOCDATA message
 * @tc: private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 * @dp: the copyied in data
 *
 * This is step 3 for SET operations.  This is the result of the implicit or explicit copyin
 * operation for the object specific structure.  We can now perform sets and commits.  All SET
 * operations also include a last copyout step that copies out the information actually set.
 */
static __unlikely int
tc_copyin2(struct tc *tc, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int len, err = 0;
	mblk_t *bp;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", tc_iocname(cp->cp_cmd));

	len = dp->b_wptr - dp->b_rptr;
	if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, len);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(TCAP_IOCSOPTION):
		err = tc_setoption(tc, (struct tcap_option *) bp->b_rptr);
		break;
	case _IOC_NR(TCAP_IOCSCONFIG):
		err = tc_setconfig(tc, (struct tcap_config *) bp->b_rptr);
		break;
	case _IOC_NR(TCAP_IOCTCONFIG):
		err = tc_testconfig(tc, (struct tcap_config *) bp->b_rptr);
		break;
	case _IOC_NR(TCAP_IOCCCONFIG):
		err = tc_setconfig(tc, (struct tcap_config *) bp->b_rptr);
		break;
	case _IOC_NR(TCAP_IOCSSTATSP):
		err = tc_setstatsp(tc, (struct tcap_stats *) bp->b_rptr);
		break;
	case _IOC_NR(TCAP_IOCSNOTIFY):
		err = tc_setnotify(tc, (struct tcap_notify *) dp->b_rptr);
		break;
	case _IOC_NR(TCAP_IOCCNOTIFY):
		err = tc_clrnotify(tc, (struct tcap_notify *) dp->b_rptr);
		break;
	default:
		err = EPROTO;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
}

/**
 * tc_copyout: - process M_IOCDATA message
 * @tc: private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 *
 * This is the final step which is a simple copy done operation.
 */
static __unlikely int
tc_copyout(struct tc *tc, queue_t *q, mblk_t *mp)
{
	mi_copyout(q, mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS MESSAGE HANDLING
 *
 *  =========================================================================
 */

/**
 * tc_m_data: - process M_DATA message
 * @q: active queue (write queue)
 * @mp: the M_DATA message
 */
static fastcall int
tc_m_data(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * tc_m_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (write queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
static fastcall int
tc_m_proto(queue_t *q, mblk_t *mp)
{
	t_uscalar_t prim;
	struct tc *tc;
	caddr_t priv;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding primitive too short",
			  __FUNCTION__);
		freemsg(mp);
		return (0);
	}
	prim = *(t_uscalar_t *) mp->b_rptr;
	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);
	tc = TC_PRIV(q);
	tc_save_state(tc);
	tr_save_state(tc->tr);
	switch (prim) {
	case TC_BEGIN_REQ:
	case TC_BEGIN_RES:
	case TC_CONT_REQ:
	case TC_END_REQ:
	case TC_ABORT_REQ:
	case TC_INVOKE_REQ:
	case TC_RESULT_REQ:
	case TC_CANCEL_REQ:
	case TC_REJECT_REQ:
		mi_strlog(q, STRLOGDA, SL_TRACE, "<- %s", tc_primname(prim));
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- %s", tc_primname(prim));
		break;
	}

	switch (prim) {
	case TC_INFO_REQ:
		rtn = tc_info_req(tc, q, mp);
		break;
	case TC_BIND_REQ:
		rtn = tc_bind_req(tc, q, mp);
		break;
	case TC_UNBIND_REQ:
		rtn = tc_unbind_req(tc, q, mp);
		break;
	case TC_SUBS_BIND_REQ:
		rtn = tc_subs_bind_req(tc, q, mp);
		break;
	case TC_SUBS_UNBIND_REQ:
		rtn = tc_subs_unbind_req(tc, q, mp);
		break;
	case TC_OPTMGMT_REQ:
		rtn = tc_optmgmt_req(tc, q, mp);
		break;
	case TC_UNI_REQ:
		rtn = tc_uni_req(tc, q, mp);
		break;
	case TC_BEGIN_REQ:
		rtn = tc_begin_req(tc, q, mp);
		break;
	case TC_BEGIN_RES:
		rtn = tc_begin_res(tc, q, mp);
		break;
	case TC_CONT_REQ:
		rtn = tc_cont_req(tc, q, mp);
		break;
	case TC_END_REQ:
		rtn = tc_end_req(tc, q, mp);
		break;
	case TC_ABORT_REQ:
		rtn = tc_abort_req(tc, q, mp);
		break;
	case TC_INVOKE_REQ:
		rtn = tc_invoke_req(tc, q, mp);
		break;
	case TC_RESULT_REQ:
		rtn = tc_result_req(tc, q, mp);
		break;
	case TC_ERROR_REQ:
		rtn = tc_error_req(tc, q, mp);
		break;
	case TC_CANCEL_REQ:
		rtn = tc_cancel_req(tc, q, mp);
		break;
	case TC_REJECT_REQ:
		rtn = tc_reject_req(tc, q, mp);
		break;
	default:
		rtn = tc_other_req(tc, q, mp);
		break;
	}
	if (rtn) {
		tr_restore_state(tc->tr);
		tc_restore_state(tc);
	}
	mi_unlock(priv);
	return (rtn);

}

/**
 * tc_m_sig: - process M_SIG or M_PCSIG message
 * @q: active queue (write queue)
 * @mp: the M_SIG or M_PCSIG message
 */
static fastcall int
tc_m_sig(queue_t *q, mblk_t *mp)
{
	struct tc *tc;
	caddr_t priv;
	int rtn;

	if ((priv = mi_trylock(q)) == NULL)
		return (mi_timer_requeue(mp) ? -EDEADLK : 0);

	if (!mi_timer_valid(mp))
		return (0);

	tc = TC_PRIV(q);

	tc_save_state(tc);

	switch (*(int *) mp->b_rptr) {
#if 0
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = tc_t1_timeout(tc, q, mp);
		break;
#endif
	default:
		mi_strlog(q, 0, SL_ERROR, "%s: discarding undefined timeout %d", __FUNCTION__,
			  *(int *) mp->b_rptr);
		rtn = 0;
		break;
	}
	if (rtn) {
		tc_restore_state(tc);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * tc_m_ioctl: - process M_IOCTL message
 * @q: active queue (write queue)
 * @mp: the M_IOCTL message
 */
static fastcall int
tc_m_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct tc *tc;
	caddr_t priv;
	int err;

	if (_IOC_TYPE(ioc->ioc_cmd) != TCAP_IOC_MAGIC) {
		/* Let driver deal with these. */
		putnext(q, mp);
		return (0);
	}
	if (!mp->b_cont) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);
	tc = TC_PRIV(q);
	err = tc_ioctl(tc, q, mp);
	mi_unlock(priv);
	return (err);
}

/**
 * tc_m_iocdata: - process M_IOCDATA message
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 */
static fastcall int
tc_m_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct tc *tc;
	caddr_t priv;
	mblk_t *dp;
	int err;

	if (_IOC_TYPE(cp->cp_cmd) != TCAP_IOC_MAGIC) {
		/* Let driver deal with these. */
		putnext(q, mp);
		return (0);
	}
	if (!mp->b_cont) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);
	tc = TC_PRIV(q);
	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		err = 0;
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		err = tc_copyin1(tc, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_IN, 2):
		err = tc_copyin2(tc, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		err = tc_copyout(tc, q, mp);
		break;
	default:
		mi_copy_done(q, mp, EPROTO);
		err = 0;
		break;
	}
	mi_unlock(priv);
	return (err);
}

/**
 * tc_m_flush: - process M_FLUSH message
 * @q: active queue (write queue)
 * @mp: the M_FLUSH message
 */
static fastcall int
tc_m_flush(queue_t *q, mblk_t *mp)
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
 * tc_m_other: - process unrecognized STREAMS message
 * @q: active queue (write queue)
 * @mp: the unrecognized STREAMS message
 */
static fastcall int
tc_m_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static inline fastcall int
tc_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return tc_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tc_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return tc_m_sig(q, mp);
	case M_IOCTL:
		return tc_m_ioctl(q, mp);
	case M_IOCDATA:
		return tc_m_iocdata(q, mp);
	case M_FLUSH:
		return tc_m_flush(q, mp);
	default:
		return tc_m_other(q, mp);
	}
}

/**
 * tr_m_data: - process M_DATA message
 * @q: active queue (read queue)
 * @mp: the M_DATA message
 */
static fastcall int
tr_m_data(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * tr_m_proto: - process M_PROTO or M_PCPROTO message
 * @q: active queue (read queue)
 * @mp: the M_PROTO or M_PCPROTO message
 */
static fastcall int
tr_m_proto(queue_t *q, mblk_t *mp)
{
	t_uscalar_t prim;
	struct tr *tr;
	caddr_t priv;
	int rtn;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim)) {
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "%s: discarding primitive too short",
			  __FUNCTION__);
		freemsg(mp);
		return (0);
	}
	prim = *(t_uscalar_t *) mp->b_rptr;
	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);
	tr = TR_PRIV(q);
	tr_save_state(tr);
	tc_save_state(tr->tc);
	switch (prim) {
	case TR_BEGIN_IND:
	case TR_BEGIN_CON:
	case TR_CONT_IND:
	case TR_END_IND:
	case TR_ABORT_IND:
		mi_strlog(q, STRLOGDA, SL_TRACE, "%s <-", tr_primname(prim));
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "%s <-", tr_primname(prim));
		break;
	}
	switch (prim) {
	case TR_INFO_ACK:
		rtn = tr_info_ack(tr, q, mp);
		break;
	case TR_ADDR_ACK:
		rtn = tr_addr_ack(tr, q, mp);
		break;
	case TR_BIND_ACK:
		rtn = tr_bind_ack(tr, q, mp);
		break;
	case TR_OK_ACK:
		rtn = tr_ok_ack(tr, q, mp);
		break;
	case TR_ERROR_ACK:
		rtn = tr_error_ack(tr, q, mp);
		break;
	case TR_OPTMGMT_ACK:
		rtn = tr_optmgmt_ack(tr, q, mp);
		break;
	case TR_UNI_IND:
		rtn = tr_uni_ind(tr, q, mp);
		break;
	case TR_BEGIN_IND:
		rtn = tr_begin_ind(tr, q, mp);
		break;
	case TR_BEGIN_CON:
		rtn = tr_begin_con(tr, q, mp);
		break;
	case TR_CONT_IND:
		rtn = tr_cont_ind(tr, q, mp);
		break;
	case TR_END_IND:
		rtn = tr_end_ind(tr, q, mp);
		break;
	case TR_ABORT_IND:
		rtn = tr_abort_ind(tr, q, mp);
		break;
	case TR_NOTICE_IND:
		rtn = tr_notice_ind(tr, q, mp);
		break;
	default:
		rtn = tr_other_ind(tr, q, mp);
		break;
	}
	if (rtn) {
		tc_restore_state(tr->tc);
		tr_restore_state(tr);
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * tr_m_sig: - process M_SIG or M_PCSIG message
 * @q: active queue (read queue)
 * @mp: the M_SIG or M_PCSIG message
 */
static fastcall int
tr_m_sig(queue_t *q, mblk_t *mp)
{
	struct tr *tr;
	caddr_t priv;
	int rtn;

	if ((priv = mi_trylock(q)) == NULL)
		return (mi_timer_requeue(mp) ? -EDEADLK : 0);

	if (!mi_timer_valid(mp))
		return (0);

	tr = TR_PRIV(q);

	tr_save_state(tr);

	switch (*(int *) mp->b_rptr) {
#if 0
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = tr_t1_timeout(tr, q, mp);
		break;
#endif
	default:
		mi_strlog(q, 0, SL_ERROR, "%s: discarding undefined timeout %d", __FUNCTION__,
			  *(int *) mp->b_rptr);
		rtn = 0;
		break;
	}
	if (rtn) {
		tr_restore_state(tr);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	mi_unlock(priv);
	return (rtn);
}

/**
 * tr_m_copy: - process M_COPYIN or M_COPYOUT message
 * @q: active queue (read queue)
 * @mp: the M_COPYIN or M_COPYOUT message
 */
static fastcall int
tr_m_copy(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * tr_m_iocack: - process M_IOCACK or M_IOCNAK message
 * @q: active queue (read queue)
 * @mp: the M_IOCACK or M_IOCNAK message
 */
static fastcall int
tr_m_iocack(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * tr_m_flush: - process M_FLUSH message
 * @q: active queue (read queue)
 * @mp: the M_FLUSH message
 */
static fastcall int
tr_m_flush(queue_t *q, mblk_t *mp)
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
 * tr_m_other: - process unrecognized STREAMS message
 * @q: active queue (read queue)
 * @mp: the unrecognized STREAMS message
 */
static fastcall int
tr_m_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static inline fastcall int
tr_msg(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return tr_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return tr_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return tr_m_sig(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return tr_m_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return tr_m_iocack(q, mp);
	case M_FLUSH:
		return tr_m_flush(q, mp);
	default:
		return tr_m_other(q, mp);
	}
}

/*
 *  =========================================================================
 *
 *  QUEUE PUT AND SERVICE PROCEDURES
 *
 *  =========================================================================
 */
static streamscall __hot_write int
tc_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || tc_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall __hot_out int
tc_srv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (tc_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_read int
tr_srv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (tr_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall __hot_in int
tr_put(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || tr_msg(q, mp))
		putq(q, mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  QUEUE OPEN AND CLOSE ROUTINES
 *
 *  =========================================================================
 */

static caddr_t tc_opens = NULL;

static streamscall int
tc_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	mblk_t *r1p, *r2p;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if (!(r1p = allocb(sizeof(struct TR_addr_req), BPRI_WAITOK)))
		return (ENOBUFS);
	if (!(r2p = allocb(sizeof(struct TR_info_req), BPRI_WAITOK))) {
		freeb(r1p);
		return (ENOBUFS);
	}
	if ((err = mi_open_comm(&tc_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		freeb(r1p);
		freeb(r2p);
		return (err);
	}
	p = PRIV(q);

	/* initialize private structure */
	p->r_priv.priv = p;
	p->r_priv.tc = &p->w_priv;
	p->r_priv.oq = WR(q);
	p->r_priv.state = TRS_NOSTATES;
	p->r_priv.oldstate = TRS_NOSTATES;
	/* FIXME initialize some more */

	p->w_priv.priv = p;
	p->w_priv.tr = &p->r_priv;
	p->w_priv.oq = q;
	p->w_priv.state = TCS_NOSTATES;
	p->w_priv.oldstate = TCS_NOSTATES;
	/* FIXME initialize some more */

	/* issue an immediate address request */
	DB_TYPE(r1p) = M_PCPROTO;
	((struct TR_addr_req *) r1p->b_wptr)->PRIM_type = TR_ADDR_REQ;
	r1p->b_wptr += sizeof(struct TR_addr_req);

	/* issue an immediate information request */
	DB_TYPE(r2p) = M_PCPROTO;
	((struct TR_info_req *) r2p->b_wptr)->PRIM_type = TR_INFO_REQ;
	r2p->b_wptr += sizeof(struct TR_info_req);

	qprocson(q);
	noenable(q);
	putnext(q, r1p);
	putnext(q, r2p);
	return (0);
}

static streamscall int
tc_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	(void) p;
	qprocsoff(q);
	/* FIXME free timers and other things */
	mi_close_comm(&tc_opens, q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS INITIALIZATION
 *
 *  =========================================================================
 */

static struct qinit tc_rinit = {
	.qi_putp = tr_put,
	.qi_srvp = tr_srv,
	.qi_qopen = tc_qopen,
	.qi_qclose = tc_qclose,
	.qi_minfo = &tc_minfo,
	.qi_mstat = &tc_rstat,
};
static struct qinit tc_winit = {
	.qi_putp = tc_put,
	.qi_srvp = tc_srv,
	.qi_minfo = &tc_minfo,
	.qi_mstat = &tc_wstat,
};
static struct streamtab tc_modinfo = {
	.st_rdinit = &tc_rinit,
	.st_wrinit = &tc_winit,
};

static unsigned short modid = MOD_ID;

/*
 *  =========================================================================
 *
 *  KERNEL MODULE INITIALIZATION
 *
 *  =========================================================================
 */

#ifdef module_param
module_param(modid, short, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module id for TC-MOD module.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static struct fmodsw tc_fmod = {
	.f_name = MOD_NAME,
	.f_str = &tc_modinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
tc_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&tc_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
tc_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&tc_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(tc_modinit);
module_exit(tc_modexit);
