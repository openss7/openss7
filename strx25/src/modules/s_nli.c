/*****************************************************************************

 @(#) $RCSfile: s_nli.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 12:16:24 $

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

 Last Modified $Date: 2008-07-01 12:16:24 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: s_nli.c,v $
 Revision 0.9.2.1  2008-07-01 12:16:24  brian
 - added manual pages, specs and conversion modules

 *****************************************************************************/

#ident "@(#) $RCSfile: s_nli.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 12:16:24 $"

static char const ident[] = "$RCSfile: s_nli.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 12:16:24 $";

/*
 * This is s_nli, a pushable STREAMS module that interprets the SpiderX25
 * input-output controls and converts them to NPI input-output controls.
 * Primtives are also converted from NLI primitives to NPI primitives.  The
 * purpose of this module is to permit SpiderX25 compatible programs, drivers
 * and modules to be invoked or attached to NPI Streams.
 */

#define _MPS_SOURCE

#include <sys/os7/compat.h>
#include <sys/npi.h>
#include <sys/npi_ioctl.h>
#include <sys/snet/x25_proto.h>
#include <sys/snet/x25_control.h>

#define S_NLI_DESCRIP	"SPIDER X25 COMPATIBILITY MODULE FOR LINUX FAST-STREAMS"
#define S_NLI_EXTRA	"Part of the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define S_NLI_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define S_NLI_REVISION	"OpenSS7 $RCSfile: s_nli.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 12:16:24 $"
#define S_NLI_DEVICE	"SVR 4.2MP CDI CD_HDLC Devices"
#define S_NLI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define S_NLI_LICENSE	"GPL"
#define S_NLI_BANNER	S_NLI_DESCRIP	"\n" \
			S_NLI_EXTRA	"\n" \
			S_NLI_COPYRIGHT	"\n" \
			S_NLI_REVISION	"\n" \
			S_NLI_DEVICE	"\n" \
			S_NLI_CONTACT	"\n"
#define S_NLI_SPLASH	S_NLI_DESCRIP	" - " \
			S_NLI_REVISION

#ifndef CONFIG_STREAMS_S_NLI_NAME
#error "CONFIG_STREAMS_S_NLI_NAME must be defined."
#endif				/* CONFIG_STREAMS_S_NLI_NAME */
#ifndef CONFIG_STREAMS_S_NLI_MODID
#error "CONFIG_STREAMS_S_NLI_MODID must be defined."
#endif				/* CONFIG_STREAMS_S_NLI_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(S_NLI_CONTACT);
MODULE_DESCRIPTION(S_NLI_DESCRIP);
MODULE_SUPPORTED_DEVICE(S_NLI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(S_NLI_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-s_nli");
MODULE_ALIAS("streams-module-s_nli");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_S_NLI_MODID));
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#endif				/* LINUX */

#ifndef S_NLI_MOD_NAME
#define S_NLI_MOD_NAME		CONFIG_STREAMS_S_NLI_NAME
#endif				/* S_NLI_MOD_NAME */
#ifndef S_NLI_MOD_ID
#define S_NLI_MOD_ID		CONFIG_STREAMS_S_NLI_MODID
#endif				/* S_NLI_MOD_ID */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define MOD_ID		S_NLI_MOD_ID
#define MOD_NAME	S_NLI_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	S_NLI_BANNER
#else				/* MODULE */
#define MOD_BANNER	S_NLI_SPLASH
#endif				/* MODULE */

static struct module_info nl_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat nl_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat nl_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));



/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * --------------------------------------------------------------------------
 */

static fastcall inline int
nl_w_msg_put(queue_t *q, mblk_t *mp)
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
		return nl_m_flush(q, mp);
	default:
		return nl_m_other(q, mp);
	}
}
static fastcall noinline int
nl_w_msg_srv(queue_t *q, mblk_t *mp)
{
	struct nl *nl = NL_W_PRIV(q);

	if (likely(DB_TYPE(mp) == M_DATA))
		return nl_m_data(nl, q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return nl_m_proto(nl, q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return nl_m_data(nl, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return nl_m_proto(nl, q, mp);
	case M_CTL:
	case M_PCCTL:
		return nl_m_ctl(nl, q, mp);
	case M_SIG:
	case M_PCSIG:
		return nl_m_sig(q, mp);
	case M_RSE:
	case M_PCRSE:
		return nl_m_rse(q, mp);
	case M_FLUSH:
		return nl_m_flush(q, mp);
	default:
		return nl_m_other(q, mp);
	}
}
static fastcall noinline int
nl_r_msg_srv(queue_t *q, mblk_t *mp)
{
	struct nl *nl = NL_R_PRIV(q);

	if (likely(DB_TYPE(mp) == M_PROTO))
		return nl_m_proto(nl, q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return nl_m_data(nl, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return nl_m_proto(nl, q, mp);
	case M_SIG:
	case M_PCSIG:
		return nl_m_sig(q, mp);
	case M_CTL:
	case M_PCCTL:
		return nl_m_ctl(nl, q, mp);
	case M_RSE:
	case M_PCRSE:
		return nl_m_rse(q, mp);
	case M_FLUSH:
		return nl_m_flush(q, mp);
	default:
		return nl_m_other(q, mp);
	}
}
static fastcall inline int
nl_r_msg_put(queue_t *q, mblk_t *mp)
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
		return nl_m_flush(q, mp);
	default:
		return nl_m_other(q, mp);
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
nl_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || nl_w_msg_put(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int
nl_wsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (nl_w_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}
static streamscall int
nl_rsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (nl_r_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}
static streamscall int
nl_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || nl_r_msg_put(q, mp))
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

caddr_t nl_opens = NULL;

static streamscall int
nl_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	struct nl *nlw;
	struct nl *nlr;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if ((err = mi_open_comm(&nl_opens, sizeof(*p), q, devp, oflags, sflag, crp)))
		return (err);

	p = PRIV(q);
	nlw = &p->w_priv;
	nlr = &p->r_priv;

	/* initialize private structure */
	nlw->priv = p;
	nlw->nl = nlr;
	nlw->oq = q;
	nlw->state = CD_DISABLED;
	nlw->oldstate = CD_DISABLED;

	nlw->proto.info.cd_primitive = CD_INFO_ACK;
	nlw->proto.info.cd_state = CD_DISABLED;
	nlw->proto.info.cd_max_sdu = STRMAXPSZ;
	nlw->proto.info.cd_min_sdu = STRMINPSZ;
	nlw->proto.info.cd_class = CD_HDLC;
	nlw->proto.info.cd_duplex = CD_FULLDUPLEX;
	nlw->proto.info.cd_output_style = CD_UNACKEDOUTPUT;
	nlw->proto.info.cd_features = (CD_CANREAD | CD_AUTOALLOW);
	nlw->proto.info.cd_addr_length = 0;
	nlw->proto.info.cd_ppa_style = CD_STYLE1;

	nlw->sigs = CD_DSR;
	nlw->alen = 0;

	nlr->priv = p;
	nlr->nl = nlw;
	nlr->oq = WR(q);
	nlr->state = CD_DISABLED;
	nlr->oldstate = CD_DISABLED;

	nlr->proto.info.cd_primitive = CD_INFO_ACK;
	nlr->proto.info.cd_state = CD_DISABLED;
	nlr->proto.info.cd_max_sdu = STRMAXPSZ;
	nlr->proto.info.cd_min_sdu = STRMINPSZ;
	nlr->proto.info.cd_class = CD_HDLC;
	nlr->proto.info.cd_duplex = CD_FULLDUPLEX;
	nlr->proto.info.cd_output_style = CD_UNACKEDOUTPUT;
	nlr->proto.info.cd_features = (CD_CANREAD | CD_AUTOALLOW);
	nlr->proto.info.cd_addr_length = 0;
	nlr->proto.info.cd_ppa_style = CD_STYLE1;

	nlr->sigs = CD_DSR;
	nlr->alen = 0;

	qprocson(q);
	return (0);
}

static streamscall int
nl_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	if (p == NULL)
		return (0);
	qprocsoff(q);
	mi_close_comm(&nl_opens, q);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

static struct qinit nl_rinit = {
	.qi_putp = &nl_rput,
	.qi_srvp = &nl_rsrv,
	.qi_qopen = &nl_qopen,
	.qi_qclose = &nl_qclose,
	.qi_minfo = &nl_minfo,
	.qi_mstat = &nl_rstat,
};

static struct qinit nl_winit = {
	.qi_putp = &nl_wput,
	.qi_srvp = &nl_wsrv,
	.qi_minfo = &nl_minfo,
	.qi_mstat = &nl_wstat,
};

struct streamtab s_nli_info = {
	.st_rdinit = &nl_rinit,
	.st_wrinit = &nl_winit,
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
MODULE_PARM_DESC(modid, "Module ID for S_NLI.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

struct fmodsw s_nli_fmod = {
	.f_name = MOD_NAME,
	.f_str = &s_nli_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
s_nli_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&s_nli_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
s_nli_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&s_nli_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(s_nli_modinit);
module_exit(s_nli_modexit);

#endif				/* LINUX */










