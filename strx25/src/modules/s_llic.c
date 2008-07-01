/*****************************************************************************

 @(#) $RCSfile: s_llic.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 12:16:24 $

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

 $Log: s_llic.c,v $
 Revision 0.9.2.1  2008-07-01 12:16:24  brian
 - added manual pages, specs and conversion modules

 *****************************************************************************/

#ident "@(#) $RCSfile: s_llic.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 12:16:24 $"

static char const ident[] = "$RCSfile: s_llic.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 12:16:24 $";

/*
 * This is s_llic, a pushable STREAMS module that interprets the SpiderX25
 * input-output controls and converts them to DLPI input-output controls.
 * Primitives are not converted and are simply passed through.  The purpose of
 * this module is to permit SpiderX25 compatible management programs to be
 * invoked on DLPI Streams.
 */

#define _MPS_SOURCE

#include <sys/os7/compat.h>
#include <sys/dlpi.h>
#include <sys/dlpi_ioctl.h>
#include <sys/snet/ll_proto.h>
#include <sys/snet/ll_control.h>

#define S_LLIC_DESCRIP		"SPIDER LLI ADMIN COMPATIBILITY MODULE FOR LINUX FAST-STREAMS"
#define S_LLIC_EXTRA		"Part of the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define S_LLIC_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define S_LLIC_REVISION		"OpenSS7 $RCSfile: s_llic.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 12:16:24 $"
#define S_LLIC_DEVICE		"SVR 4.2MP DLPI Devices"
#define S_LLIC_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define S_LLIC_LICENSE		"GPL"
#define S_LLIC_BANNER		S_LLIC_DESCRIP		"\n" \
				S_LLIC_EXTRA		"\n" \
				S_LLIC_COPYRIGHT	"\n" \
				S_LLIC_REVISION		"\n" \
				S_LLIC_DEVICE		"\n" \
				S_LLIC_CONTACT		"\n"
#define S_LLIC_SPLASH		S_LLIC_DESCRIP		" - " \
				S_LLIC_REVISION

#ifndef CONFIG_STREAMS_S_LLIC_NAME
#error "CONFIG_STREAMS_S_LLIC_NAME must be defined."
#endif				/* CONFIG_STREAMS_S_LLIC_NAME */
#ifndef CONFIG_STREAMS_S_LLIC_MODID
#error "CONFIG_STREAMS_S_LLIC_MODID must be defined."
#endif				/* CONFIG_STREAMS_S_LLIC_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(S_LLIC_CONTACT);
MODULE_DESCRIPTION(S_LLIC_DESCRIP);
MODULE_SUPPORTED_DEVICE(S_LLIC_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(S_LLIC_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-s_llic");
MODULE_ALIAS("streams-module-s_llic");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_S_LLIC_MODID));
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#endif				/* LINUX */

#ifndef S_LLIC_MOD_NAME
#define S_LLIC_MOD_NAME		CONFIG_STREAMS_S_LLIC_NAME
#endif				/* S_LLIC_MOD_NAME */
#ifndef S_LLIC_MOD_ID
#define S_LLIC_MOD_ID		CONFIG_STREAMS_S_LLIC_MODID
#endif				/* S_LLIC_MOD_ID */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define MOD_ID		S_LLIC_MOD_ID
#define MOD_NAME	S_LLIC_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	S_LLIC_BANNER
#else				/* MODULE */
#define MOD_BANNER	S_LLIC_SPLASH
#endif				/* MODULE */

static struct module_info ll_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat ll_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat ll_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));



/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * --------------------------------------------------------------------------
 */

static fastcall inline int
ll_w_msg_put(queue_t *q, mblk_t *mp)
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
		return ll_m_flush(q, mp);
	default:
		return ll_m_other(q, mp);
	}
}
static fastcall noinline int
ll_w_msg_srv(queue_t *q, mblk_t *mp)
{
	struct ll *ll = LL_W_PRIV(q);

	if (likely(DB_TYPE(mp) == M_DATA))
		return ll_m_data(ll, q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return ll_m_proto(ll, q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return ll_m_data(ll, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return ll_m_proto(ll, q, mp);
	case M_CTL:
	case M_PCCTL:
		return ll_m_ctl(ll, q, mp);
	case M_SIG:
	case M_PCSIG:
		return ll_m_sig(q, mp);
	case M_RSE:
	case M_PCRSE:
		return ll_m_rse(q, mp);
	case M_FLUSH:
		return ll_m_flush(q, mp);
	default:
		return ll_m_other(q, mp);
	}
}
static fastcall noinline int
ll_r_msg_srv(queue_t *q, mblk_t *mp)
{
	struct ll *ll = LL_R_PRIV(q);

	if (likely(DB_TYPE(mp) == M_PROTO))
		return ll_m_proto(ll, q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return ll_m_data(ll, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return ll_m_proto(ll, q, mp);
	case M_SIG:
	case M_PCSIG:
		return ll_m_sig(q, mp);
	case M_CTL:
	case M_PCCTL:
		return ll_m_ctl(ll, q, mp);
	case M_RSE:
	case M_PCRSE:
		return ll_m_rse(q, mp);
	case M_FLUSH:
		return ll_m_flush(q, mp);
	default:
		return ll_m_other(q, mp);
	}
}
static fastcall inline int
ll_r_msg_put(queue_t *q, mblk_t *mp)
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
		return ll_m_flush(q, mp);
	default:
		return ll_m_other(q, mp);
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
ll_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || ll_w_msg_put(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int
ll_wsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (ll_w_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}
static streamscall int
ll_rsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (ll_r_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}
static streamscall int
ll_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || ll_r_msg_put(q, mp))
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

caddr_t ll_opens = NULL;

static streamscall int
ll_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	struct ll *llw;
	struct ll *llr;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if ((err = mi_open_comm(&ll_opens, sizeof(*p), q, devp, oflags, sflag, crp)))
		return (err);

	p = PRIV(q);
	llw = &p->w_priv;
	llr = &p->r_priv;

	/* initialize private structure */
	llw->priv = p;
	llw->ll = llr;
	llw->oq = q;
	llw->state = CD_DISABLED;
	llw->oldstate = CD_DISABLED;

	llw->proto.info.cd_primitive = CD_INFO_ACK;
	llw->proto.info.cd_state = CD_DISABLED;
	llw->proto.info.cd_max_sdu = STRMAXPSZ;
	llw->proto.info.cd_min_sdu = STRMINPSZ;
	llw->proto.info.cd_class = CD_HDLC;
	llw->proto.info.cd_duplex = CD_FULLDUPLEX;
	llw->proto.info.cd_output_style = CD_UNACKEDOUTPUT;
	llw->proto.info.cd_features = (CD_CANREAD | CD_AUTOALLOW);
	llw->proto.info.cd_addr_length = 0;
	llw->proto.info.cd_ppa_style = CD_STYLE1;

	llw->sigs = CD_DSR;
	llw->alen = 0;

	llr->priv = p;
	llr->ll = llw;
	llr->oq = WR(q);
	llr->state = CD_DISABLED;
	llr->oldstate = CD_DISABLED;

	llr->proto.info.cd_primitive = CD_INFO_ACK;
	llr->proto.info.cd_state = CD_DISABLED;
	llr->proto.info.cd_max_sdu = STRMAXPSZ;
	llr->proto.info.cd_min_sdu = STRMINPSZ;
	llr->proto.info.cd_class = CD_HDLC;
	llr->proto.info.cd_duplex = CD_FULLDUPLEX;
	llr->proto.info.cd_output_style = CD_UNACKEDOUTPUT;
	llr->proto.info.cd_features = (CD_CANREAD | CD_AUTOALLOW);
	llr->proto.info.cd_addr_length = 0;
	llr->proto.info.cd_ppa_style = CD_STYLE1;

	llr->sigs = CD_DSR;
	llr->alen = 0;

	qprocson(q);
	return (0);
}

static streamscall int
ll_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	if (p == NULL)
		return (0);
	qprocsoff(q);
	mi_close_comm(&ll_opens, q);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

static struct qinit ll_rinit = {
	.qi_putp = &ll_rput,
	.qi_srvp = &ll_rsrv,
	.qi_qopen = &ll_qopen,
	.qi_qclose = &ll_qclose,
	.qi_minfo = &ll_minfo,
	.qi_mstat = &ll_rstat,
};

static struct qinit ll_winit = {
	.qi_putp = &ll_wput,
	.qi_srvp = &ll_wsrv,
	.qi_minfo = &ll_minfo,
	.qi_mstat = &ll_wstat,
};

struct streamtab s_llic_info = {
	.st_rdinit = &ll_rinit,
	.st_wrinit = &ll_winit,
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
MODULE_PARM_DESC(modid, "Module ID for S_LLIC.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

struct fmodsw s_llic_fmod = {
	.f_name = MOD_NAME,
	.f_str = &s_llic_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
s_llic_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&s_llic_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
s_llic_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&s_llic_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(s_llic_modinit);
module_exit(s_llic_modexit);

#endif				/* LINUX */

