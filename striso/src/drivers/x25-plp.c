/*****************************************************************************

 @(#) $RCSfile: x25-plp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/08/14 07:05:03 $

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

 Last Modified $Date: 2007/08/14 07:05:03 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: x25-plp.c,v $
 Revision 0.9.2.2  2007/08/14 07:05:03  brian
 - GNUv3 header update

 Revision 0.9.2.1  2006/04/12 20:36:01  brian
 - added some experimental drivers

 *****************************************************************************/

#ident "@(#) $RCSfile: x25-plp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/08/14 07:05:03 $"

static char const ident[] =
    "$RCSfile: x25-plp.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/08/14 07:05:03 $";

/*
 * This is an X.25 PLP (LLC2, XOT, SLP and MLP) (CONS) driver per X.223.  It can be used with the
 * X.25 LAPB driver.
 */

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

#include <sys/os7/compat.h>

#include <sys/dlpi.h>
#include <sys/npi.h>
#include <sys/tihdr.h>

#define X25_PLP_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define X25_PLP_EXTRA		"Part of the OpenSS7 OSI Stack for Linux Fast-STREAMS"
#define X25_PLP_COPYRIGHT	"Copyright (c) 1997-2007  OpenSS7 Corporation.  All Rights Reserved."
#define X25_PLP_REVISION	"OpenSS7 $RCSfile$ $Name$($Revision$) $Date$"
#define X25_PLP_DEVICE		"SVR 4.2 STREAMS X.25 PLP (ISO 8208) Network Provider"
#define X25_PLP_CONTACT		"Brian BIdulock <bidulock@openss7.org>"
#define X25_PLP_LICENSE		"GPL"
#define X25_PLP_BANNER		X25_PLP_DESCRIP		"\n" \
				X25_PLP_EXTRA		"\n" \
				X25_PLP_COPYRIGHT	"\n" \
				X25_PLP_DEVICE		"\n" \
				X25_PLP_CONTACT
#define X25_PLP_SPLASH		X25_PLP_DESCRIP		" - " \
				X25_PLP_REVISION

#ifdef LINUX
MODULE_AUTHOR(X25_PLP_CONTACT);
MODULE_DESCRIPTION(X25_PLP_DESCRIP);
MODULE_SUPPORTED_DEVICE(X25_PLP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(X25_PLP_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODUL_ALIAS
MODULE_ALIAS("streams-x25-plp");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define X25_PLP_DRV_ID		CONFIG_STREAMS_X25_PLP_MODID
#define X25_PLP_DRV_NAME	CONFIG_STREAMS_X25_PLP_NAME
#define X25_PLP_CMAJORS		CONFIG_STREAMS_X25_PLP_NMAJORS
#define X25_PLP_CMAJOR_0	CONFIG_STREAMS_X25_PLP_MAJOR
#define X25_PLP_UNITS		CONFIG_STREAMS_X25_PLP_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_X25_PLP_MODID));
MODULE_ALIAS("streams-driver-x25-plp");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_X25_PLP_MAJOR));
MODULE_ALIAS("/dev/streams/x25-plp");
MODULE_ALIAS("/dev/streams/x25-plp/*");
MODULE_ALIAS("/dev/streams/clone/x25-plp");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(X25_PLP_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(X25_PLP_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(X25_PLP_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/x25-plp");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  =========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  =========================================================================
 */

#define DRV_ID		X25_PLP_DRV_ID
#define DRV_NAME	X25_PLP_DRV_NAME
#define CMAJORS		X25_PLP_CMAJORS
#define CMAJOR_0	X25_PLP_CMAJOR_0
#define UNITS		X25_PLP_UNITS

#ifdef MODULE
#define DRV_BANNER	X25_PLP_BANNER
#else				/* MODULE */
#define DRV_BANNER	X25_PLP_SPLASH
#endif				/* MODULE */

static struct module_info xp_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = 0,
	.mi_maxpsz = INFPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};
static struct module_stat xp_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat xp_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct module_info dl_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = 0,
	.mi_maxpsz = INFPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};
static struct module_stat dl_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat dl_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static const char *
tp_primname(t_uscalar_t prim)
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
		return ("T_????");
	}
}

static const char *
np_primname(np_ulong prim)
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
		return ("N_????");
	}
}

static const char *
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
		return ("DL_????");
	}
}

/*
 * ==========================================================================
 *
 * SREAMS Message Handling
 *
 * ==========================================================================
 */

/*
 * M_IOCTL Handling
 * --------------------------------------------------------------------------
 */

/*
 * Lower multiplex Link and Unlink routines, both interfaces.
 * ----------------------------------------------------------
 */

/**
 * plp_i_link: - perform I_LINK operation
 * @xp: PLP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * The driver supports I_LINK operations; however, any DL structure that is linked with an I_LINK
 * operation can only be managed by the control Stream linking the lower Stream and cannot be shared
 * across other upper Streams unless configured against a subsystem id.
 *
 * Note that if this is not the first DL linked and there are running Users, this DL will not be
 * available to them until it is configured and brought to the active state.  If this is the first
 * DL, there cannot be running users.
 */
static int
plp_i_link(queue_t *q, struct xp *xp, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	dl_info_req_t *p;
	struct plp *plp;
	struct dl *dl = NULL;
	mblk_t *rp = NULL;
	pl_t pl;
	int err;

	if (!(rp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		err = -ENOBUFS;
		goto error;
	}
	if (!(dl = plp_alloc_link(l->l_qtop, l->l_index, ioc->ioc_cr, 0))) {
		err = ENOMEM;
		goto error;
	}
	write_lock_irqsave(&plp_mux_lock, pl);
	if (!(plp = dl->plp.loc)) {
		write_unlock_irqrestore(&plp_mux_lock, pl);
		err = ENXIO;
		goto error;
	}
	if (!plp_trylock(q, xp)) {
		err = -EDEADLK;
		write_unlock_irqrestore(&plp_mux_lock, pl);
		goto error;
	}
	/* Note that there can only be one layer management Stream per PLP.  For temporary links,
	   that must be the same layer management Stream used to create the PLP. */
	if (plp->xp.lm != xp) {
		err = EPERM;
		plp_unlock(plp);
		write_unlock_irqrestore(&plp_mux_lock, pl);
		goto error;
	}
	if ((dl->plp.next = plp->dl.list))
		dl->plp.next->plp.prev = &dl->plp.next;
	dl->plp.prev = &plp->dl.list;
	dl->plp.plp = plp;
	plp->dl.list = dl;
	mi_attach(l->l_qtop, (caddr_t) dl);
	plp_unlock(plp);
	write_unlock_irqrestore(&plp_mux_lock, pl);
	mi_copy_done(q, mp, 0);
	DB_TYPE(rp) = M_PCPROTO;
	p = (typeof(p)) rp->b_rptr;
	p->dl_primitive = DL_INFO_REQ;
	rp->b_wptr = rp->b_rptr + sizeof(*p);
	putnext(dl->wq, rp);	/* immediate info request */
	return (0);
      error:
	if (rp)
		freeb(rp);
	if (dl)
		plp_free_link(dl);
	if (err >= 0) {
		mi_copy_done(q, mp, err);
		return (0);
	}
	return (err);
};
static int
plp_i_unlink(queue_t *q, struct xp *xp, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct dl *dl;

	dl = DL_PRIV(l->l_qtop);
	plp_free_link(dl);
	mi_copy_done(q, mp, 0);
	return (0);
};
static int
plp_i_plink(queue_t *q, struct xp *xp, mblk_t *mp)
{
	struct ioblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct dl *dl;
	int err = EPERM;

	if (ioc->ioc_cr->cr_uid == 0) {
		err = ENOMEM;
		if ((dl = plp_alloc_link(l->l_qbot, l->l_index, ioc->ioc_cr, 0)))
			err = 0;
	}
	mi_copy_done(q, mp, err);
	return (0);
}
static int
plp_i_punlink(queue_t *q, struct xp *xp, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct linkblk *l = (typeof(l)) mp->b_cont->b_rptr;
	struct dl *dl;
	int err = EPERM;

	if (ioc->ioc_cr->cr_uid == 0) {
		dl = DL_PRIV(l->l_qtop);
		plp_free_link(dl);
		err = 0;
	}
	mi_copy_done(q, mp, err);
	return (0);
}
static int
plp_i_ioctl(queue_t *q, struct xp *xp, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(I_LINK):
		return plp_i_link(q, xp, mp);
	case _IOC_NR(I_PLINK):
		return plp_i_plink(q, xp, mp);
	case _IOC_NR(I_UNLINK):
		return plp_i_unlink(q, xp, mp);
	case _IOC_NR(I_PUNLINK):
		return plp_i_punlink(q, xp, mp);
	}
	mi_copy_done(q, mp, EINVAL);
	return (0);
}

const char *
plp_iocname(int cmd)
{
	switch (_IOC_NR(cmd)) {
	case _IOC_NR(X25_IOCGOPTION):
		return ("X25_IOCGOPTION");
	case _IOC_NR(X25_IOCSOPTION):
		return ("X25_IOCSOPTION");
	case _IOC_NR(X25_IOCSCONFIG):
		return ("X25_IOCSCONFIG");
	case _IOC_NR(X25_IOCGCONFIG):
		return ("X25_IOCGCONFIG");
	case _IOC_NR(X25_IOCTCONFIG):
		return ("X25_IOCTCONFIG");
	case _IOC_NR(X25_IOCCCONFIG):
		return ("X25_IOCCCONFIG");
#ifdef X25_IOCLCONFIG
	case _IOC_NR(X25_IOCLCONFIG):
		return ("X25_IOCLCONFIG");
#endif				/* X25_IOCLCONFIG */
	case _IOC_NR(X25_IOCGSTATEM):
		return ("X25_IOCGSTATEM");
	case _IOC_NR(X25_IOCCMRESET):
		return ("X25_IOCCMRESET");
	case _IOC_NR(X25_IOCGSTATSP):
		return ("X25_IOCGSTATSP");
	case _IOC_NR(X25_IOCSSTATSP):
		return ("X25_IOCSSTATSP");
	case _IOC_NR(X25_IOCGSTATS):
		return ("X25_IOCGSTATS");
	case _IOC_NR(X25_IOCCSTATS):
		return ("X25_IOCCSTATS");
	case _IOC_NR(X25_IOCGNOTIFY):
		return ("X25_IOCGNOTIFY");
	case _IOC_NR(X25_IOCSNOTIFY):
		return ("X25_IOCSNOTIFY");
	case _IOC_NR(X25_IOCCNOTIFY):
		return ("X25_IOCCNOTIFY");
	case _IOC_NR(X25_IOCCMGMT):
		return ("X25_IOCCMGMT");
	case _IOC_NR(X25_IOCCPASS):
		return ("X25_IOCCPASS");
	default:
		return ("X25_????");
	}
}

/**
 * plp_ioctl: - process PLP M_IOCTL message
 * @xp: PLP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the input-output control operation.  Step 1 consists of copying in the
 * necessary prefix structure that identifies the object type and id being managed.
 */
static int
plp_ioctl(queue_t *q, struct xp *xp, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int size = -1;
	int err = 0;

	mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(%s)", plp_iocname(ioc->ioc_cmd));

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(X25_IOCGOPTION):
		size = sizeof(struct x25_option);
		break;
	case _IOC_NR(X25_IOCSOPTION):
		size = sizeof(struct x25_option);
		break;
	case _IOC_NR(X25_IOCSCONFIG):
		size = sizeof(struct x25_config);
		break;
	case _IOC_NR(X25_IOCGCONFIG):
		size = sizeof(struct x25_config);
		break;
	case _IOC_NR(X25_IOCTCONFIG):
		size = sizeof(struct x25_config);
		break;
	case _IOC_NR(X25_IOCCCONFIG):
		size = sizeof(struct x25_config);
		break;
#ifdef X25_IOCLCONFIG
	case _IOC_NR(X25_IOCLCONFIG):
		size = sizeof(struct x25_config);
		break;
#endif				/* X25_IOCLCONFIG */
	case _IOC_NR(X25_IOCGSTATEM):
		size = sizeof(struct x25_statem);
		break;
	case _IOC_NR(X25_IOCCMRESET):
		size = sizeof(struct x25_statem);
		break;
	case _IOC_NR(X25_IOCGSTATSP):
		size = sizeof(struct x25_stats);
		break;
	case _IOC_NR(X25_IOCSSTATSP):
		size = sizeof(struct x25_stats);
		break;
	case _IOC_NR(X25_IOCGSTATS):
		size = sizeof(struct x25_stats);
		break;
	case _IOC_NR(X25_IOCCSTATS):
		size = sizeof(struct x25_stats);
		break;
	case _IOC_NR(X25_IOCGNOTIFY):
		size = sizeof(struct x25_notify);
		break;
	case _IOC_NR(X25_IOCSNOTIFY):
		size = sizeof(struct x25_notify);
		break;
	case _IOC_NR(X25_IOCCNOTIFY):
		size = sizeof(struct x25_notify);
		break;
	case _IOC_NR(X25_IOCCMGMT):
		size = sizeof(struct x25_mgmt);
		break;
	case _IOC_NR(X25_IOCCPASS):
		size = sizeof(struct x25_pass);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_don(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
}

/* this structure is just to determine the maximum size of an ioctl */

union x25_ioctls {
	struct {
		struct x25_option option;
		union x25_option_obj obj;
	} opt_conf;
	struct {
		struct x25_option option;
		union x25_conf_obj obj;
	} conf;
	struct {
		struct x25_option option;
		union x25_statem_obj obj;
	} statem;
	struct {
		struct x25_option option;
		union x25_stats_obj obj;
	} stats;
	struct {
		struct x25_option option;
		union x25_notify_obj obj;
	} notify;
	struct x25_mgmt mgmt;
	struct x25_pass pass;
};

/**
 * plp_copyin: - process PLP M_IOCDATA message
 * @xp: PLP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is step 2 in the PLP input-output control operation.  Step 2 consists of copying out for GET
 * operations, and processing an additional copy in operation of object specific information for SET
 * operations.
 */
static int
plp_copyin(queue_t *q, struct xp *xp, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	mblk_t *bp = NULL;
	int size = -1;
	int err = 0;

	mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(%s)", plp_iocname(cp->cp_cmd));

	switch (_IOC_NR(cp->cp_cmd)) {
		int len;

	case _IOC_NR(X25_IOCGOPTION):
	case _IOC_NR(X25_IOCGCONFIG):
	case _IOC_NR(X25_IOCGSTATEM):
	case _IOC_NR(X25_IOCGSTATSP):
	case _IOC_NR(X25_IOCGSTATS):
	case _IOC_NR(X25_IOCCSTATS):
	case _IOC_NR(X25_IOCGNOTIFY):
		len = sizeof(union x25_ioctls);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, min(dp->b_wptr - dp->b_rptr, len));
		break;
#ifdef X25_IOCLCONFIG
	case _IOC_NR(X25_IOCLCONFIG):
	{
		struct x25_config *p = (typeof(p)) dp->b_rptr;

		len = (p->cmd + 1) * sizeof(*p);
		if (!(bp = mi_copyout_alloc(q, mp, NULL, len, false)))
			goto enobufs;
		bcopy(dp->b_rptr, bp->b_rptr, min(dp->b_wptr - dp->b_rptr, len));
		break;
	}
#endif				/* X25_IOCLCONFIG */
	}
	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(X25_IOCGOPTION):
		if (bp == NULL)
			goto enobufs;
		err = x25_get_options(bp);
		break;
	case _IOC_NR(X25_IOCSOPTION):
		err = x25_size_opt_confg(dp->b_rptr, &size);
		break;
	case _IOC_NR(X25_IOCGCONFIG):
		if (bp == NULL)
			goto enobufs;
		err = x25_get_conf(bp);
		break;
	case _IOC_NR(X25_IOCSCONFIG):
		err = x25_size_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(X25_IOCTCONFIG):
		err = x25_size_conf(dp->b_rptr, &size);
		break;
	case _IOC_NR(X25_IOCCCONFIG):
		err = x25_size_conf(dp->b_rptr, &size);
		break;
#ifdef X25_IOCLCONFIG
	case _IOC_NR(X25_IOCLCONFIG):
		if (bp == NULL)
			goto enobufs;
		err = x25_lst_conf(bp);
		break;
#endif				/* X25_IOCLCONFIG */
	case _IOC_NR(X25_IOCGSTATEM):
		if (bp == NULL)
			goto enobufs;
		err = x25_get_statem(bp);
		break;
	case _IOC_NR(X25_IOCCMRESET):
		err = EOPNOTSUPP;	/* later */
		break;
	case _IOC_NR(X25_IOCGSTATSP):
		if (bp == NULL)
			goto enobufs;
		err = x25_do_statsp(bp, X25_GET);
		break;
	case _IOC_NR(X25_IOCSSTATSP):
		err = x25_size_stats(dp->b_rptr, &size);
		break;
	case _IOC_NR(X25_IOCGSTATS):
		if (bp == NULL)
			goto enobufs;
		err = x25_get_stats(bp, false);
		break;
	case _IOC_NR(X25_IOCCSTATS):
		if (bp == NULL)
			goto enobufs;
		err = x25_get_stats(bp, true);
		break;
	case _IOC_NR(X25_IOCGNOTIFY):
		if (bp == NULL)
			goto enobufs;
		err = x25_do_notify(bp, 0, X25_GET);
		break;
	case _IOC_NR(X25_IOCSNOTIFY):
		err = x25_size_notify(dp->b_rptr, &size);
		break;
	case _IOC_NR(X25_IOCCNOTIFY):
		err = x25_size_notify(dp->b_rptr, &size);
		break;
	case _IOC_NR(X25_IOCCMGMT):
		err = x25_action(q, dp);
		break;
	case _IOC_NR(X25_IOCCPASS):
		break;
	default:
		err = EOPNOTSUPP;
		break;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin_n(q, mp, 0, size);
	}
	return (0);
      enobufs:
	err = ENOBUFS;
	goto done;
}

/**
 * plp_copyin2: - process PLP M_IOCDATA message
 * @xp: PLP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * Step 3 of the input-output control operation is an optional step that is used for SET operations.
 * After the second copying we now have the object specific structure that was passed by the user
 * and can complete the SET operation.  All Set operations also include a last copyout step that
 * copies out the information actually set (e.g. the assigned id on X25_ADD operations).
 */
static int
plp_copyin2(queue_t *q, struct xp *xp, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int err = 0;
	mblk_t *bp;

	mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(%s)", plp_iocname(cp->cp_cmd));

	if (!(bp = mi_copyout_alloc(q, mp, NULL, dp->b_wptr - dp->b_rptr, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, dp->b_wptr - dp->b_rptr);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(X25_IOCGOPTION):
		err = EPROTO;
		break;
	case _IOC_NR(X25_IOCSOPTION):
		err = x25_set_options(bp);
		break;
	case _IOC_NR(X25_IOCGCONFIG):
		err = EPROTO;
		break;
	case _IOC_NR(X25_IOCSCONFIG):
		err = x25_set_conf(bp);
		break;
	case _IOC_NR(X25_IOCTCONFIG):
		err = x25_test_conf(bp);
		break;
	case _IOC_NR(X25_IOCCCONFIG):
		err = x25_commit_conf(bp);
		break;
#ifdef X25_IOCLCONFIG
	case _IOC_NR(X25_IOCLCONFIG):
		err = EPROTO;
		break;
#endif				/* X25_IOCLCONFIG */
	case _IOC_NR(X25_IOCGSTATEM):
		err = EPROTO;
		break;
	case _IOC_NR(X25_IOCCMRESET):
		err = EPROTO;
		break;
	case _IOC_NR(X25_IOCGSTATSP):
		err = EPROTO;
		break;
	case _IOC_NR(X25_IOCSSTATSP):
		err = x25_do_statsp(bp, X25_CHA);
		break;
	case _IOC_NR(X25_IOCGSTATS):
		err = EPROTO;
		break;
	case _IOC_NR(X25_IOCCSTATS):
		err = EPROTO;
		break;
	case _IOC_NR(X25_IOCGNOTIFY):
		err = EPROTO;
		break;
	case _IOC_NR(X25_IOCSNOTIFY):
		err = x25_do_notify(bp, 0, X25_ADD);
		break;
	case _IOC_NR(X25_IOCCNOTIFY):
		err = x25_do_notify(bp, 0, X25_DEL);
		break;
	case _IOC_NR(X25_IOCCMGMT):
		err = EPROTO;
		break;
	case _IOC_NR(X25_IOCCPASS):
	{
		struct x25_pass *p = (typeof(p)) dp->b_rptr;

		break;
	}
	default:
		err = EOPNOTSUPP;
		break;
	}
      done:
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
      enobufs:
	err = ENOBUFS;
	goto done;

}

/**
 * plp_copyout: - process X25 M_IOCDATA message
 * @xp: PLP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * Step 4 and the final step of the input-output control operation is a final copyout step.
 */
static int
plp_copyout(queue_t *q, struct xp *xp, mblk_t *mp, mblk_t *dp)
{
	mi_copyout(q, mp);
	return (0);
}

/**
 * plp_w_ioctl: - process M_IOCTL message
 * @q: active queue (upper write queue)
 * @mp: the M_IOCTL message
 */
static int
plp_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct xp *xp;
	int err = 0;

	if (!mp->b_cont) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	if (!(xp = xp_acquire(q)))
		return (-EAGAIN);

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case __SID:
		err = plp_i_ioctl(q, xp, mp);
		break;
	case X25_IOC_MAGIC:
		err = plp_ioctl(q, xp, mp);
		break;
	default:
		mi_copy_done(q, mp, EINVAL);
		break;
	}
	xp_release(xp);
	return (err);
}

/**
 * plp_w_iocdata: - process M_IOCDATA message
 * @q: active queue (upper write queue)
 * @mp: the M_IOCDATA message
 */
static int
plp_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct xp *xp;
	int err = 0;
	mblk_t *dp;

	if (!(xp = xp_acquire(q)))
		return (-EAGAIN);
	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case X25_IOC_MAGIC:
			err = plp_copyin(q, xp, mp, dp);
			break;
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		break;
	case MI_COPY_CASE(MI_COPY_IN, 2):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case X25_IOC_MAGIC:
			err = plp_copyin2(q, xp, mp, dp);
			break;
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case X25_IOC_MAGIC:
			err = plp_copyout(q, xp, mp, dp);
			break;
		default:
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		break;
	default:
	}
	xp_release(xp);
	return (err);
}

/*
 * M_CTL, M_PCCTL Handling
 * --------------------------------------------------------------------------
 * M_CTL and M_PCTL messages are inter-module messages that serve the same
 * function as M_IOCTL and related messages, but are generated by the adjacent
 * module instead of by the Stream head.  There are no copy requests or
 * responses: all data can be access directly, there are only the equivalent
 * of M_IOCTL, M_IOCACK and M_IOCNAK.  Each message contains an struct iocblk
 * formatted the same as a I_STR M_IOCTL.  Rather than performing a copyin or
 * copyout operation, the area in mp->b_cont can be used directly.
 */

/**
 * x25_ctl: - process M_CTL, M_PCCTL from above
 * @xp: X.25 PLP private structure
 * @q: active queue (upper write queue)
 * @mp: the M_CTL, M_PCCTL message
 *
 * These are controls comming down from an upper module.  Because they are not
 * generated by the Stream head they can be trusted more than M_IOCTL
 * messages.  The action is to be performed and a response given.  The
 * mp->b_cont buffer may need to be replaced as it might not be of sufficient
 * size.  ioc_rval should be set explicitly if a return value is to be given.
 * It should be set to -1 and an ioc_error value set when an error is
 * detected.
 *
 * Return zero if the buffer has been disposed of.  Return a positive error
 * number if an error response is to be issued (outside locks).  Return a
 * negative error number if the buffer is to be requeued.
 *
 * The objects are as follows:
 *
 *	X25_OBJECT_DF - default object
 *	X25_OBJECT_MG - management stream object
 *	X25_OBJECT_CN - connection object (CO Stream)
 *	X25_OBJECT_RT - route object (static, CI or RI entry)
 *	X25_OBJECT_AJ - adjacent object (SNARE, L1 IS, L2 IS, IDRP GW)
 *	X25_OBJECT_ES - destination object
 *	X25_OBJECT_ES - End System object
 *	X25_OBJECT_DL - Datalink object
 */
static int
x25_ctl(struct xp *xp, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int err = 0;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case X25_IOCGOPTION:
	{
		struct x25_option *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object and get its options */
	}
	case X25_IOCSOPTION:
	{
		struct x25_option *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object and set its options */
	}
	case X25_IOCGCONFIG:
	{
		struct x25_config *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object and get its configuration */
	}
	case X25_IOCSCONFIG:
	{
		struct x25_config *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object and set its configuration */
	}
	case X25_IOCTCONFIG:
	{
		struct x25_config *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object and test its configuration */
	}
	case X25_IOCCCONFIG:
	{
		struct x25_config *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object and commit its configuration */
	}
	case X25_IOCLCONFIG:
	{
		struct x25_config *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object and list their subordinate configuration */
	}
	case X25_IOCGSTATEM:
	{
		struct x25_statem *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object and retrieve its state */
	}
	case X25_IOCCMRESET:
	{
		struct x25_statem *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object and reset its state */
	}
	case X25_IOCGSTATSP:
	{
		struct x25_statsp *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object and set statistics periods */
	}
	case X25_IOCSSTATSP:
	{
		struct x25_statsp *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object an set statistics periods */
	}
	case X25_IOCGSTATS:
	{
		struct x25_stats *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object and get stats without clearing */
	}
	case X25_IOCCSTATS:
	{
		struct x25_stats *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object and return stats, clear stats */
	}
	case X25_IOCGNOTIFY:
	{
		struct x25_notify *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object and get notifications */
	}
	case X25_IOCSNOTIFY:
	{
		struct x25_notify *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object and set notifications */
	}
	case X25_IOCCNOTIFY:
	{
		struct x25_notify *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object and clear notifications */
	}
	case X25_IOCCMGMT:
	{
		struct x25_mgmt *p = (typeof(p)) mp->b_cont->b_rptr;
		/* find the object and perform management on it */
	}
	case X25_IOCCPASS:
	{
		struct x25_pass *p = (typeof(p)) mp->b_cont->b_rptr;
	}
	default:
		err = EINVAL;
		break;
	}
	return (err);
}
static int
__xp_w_ctl(struct xp *xp, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case X25_IOC_MAGIC:
		err = x25_ctl(xp, q, mp);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	}
	return (err);
}
static int
xp_w_ctl(queue_t *q, mblk_t *mp)
{
	struct xp *xp;
	int err = -EAGAIN;
	struct iocblk *ioc;

	if (likely((xp = xp_acquire(q)) != NULL)) {
		err = __xp_w_ctl(xp, q, mp);
		xp_release(xp);
	}
	if (err <= 0)
		return (err);
	/* return errors outside locks */
	ioc = (typeof(ioc)) mp->b_rptr;
	ioc->ioc_error = err;
	ioc->ioc_rval = -1;
	qreply(q, mp);
	return (0);
}

/**
 * labp_ctl: - process M_CTL or M_PCCTL from below
 * @dl: Datalink private structure
 * @q: active queue (lower read queue)
 * @mp: the M_CTL or M_PCCTL message
 *
 * This is an M_CTL or M_PCCTL message from below which is a response to a
 * M_CTL or M_PCCTL message that was issued by us.  The ioc_rval will contain
 * -1 and ioc_error will be non-zero if there was an error.  ioc_rval will
 * otherwise contain a return value.  The mp->b_cont buffer will contain the
 * results as though an I_STR M_IOCTL input-output control was issued.
 *
 * Return zero (0) from this function when the buffer has been disposed of.
 * Return a positive error number if the buffer is to be freed (outside
 * locks).  Return a negative error number if the buffer is to be requeued.
 */
static int
lapb_ctl(struct dl *dl, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int err = 0;

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case X25_IOCGOPTION:
	case X25_IOCSOPTION:
	case X25_IOCGCONFIG:
	case X25_IOCSCONFIG:
	case X25_IOCTCONFIG:
	case X25_IOCCCONFIG:
	case X25_IOCLCONFIG:
	case X25_IOCGSTATEM:
	case X25_IOCCMRESET:
	case X25_IOCGSTATSP:
	case X25_IOCSSTATSP:
	case X25_IOCGSTATS:
	case X25_IOCCSTATS:
	case X25_IOCGNOTIFY:
	case X25_IOCSNOTIFY:
	case X25_IOCCNOTIFY:
	case X25_IOCCMGMT:
	case X25_IOCCPASS:
	default:
		err = EINVAL;
		break;
	}
	return (err);
}
static int
__dl_r_ctl(struct dl *dl, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	int err = 0;

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case X25_IOC_MAGIC:
		err = lapb_ctl(dp, q, mp);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	}
	return (err);
}
static int
dl_r_ctl(queue_t *q, mblk_t *mp)
{
	struct dl *dl;
	int err = -EAGAIN;
	if (likely((dl = dl_acquire(q)) != NULL)) {
		err = __dl_r_ctl(xp, q, mp);
		dl_release(dl);
	}
	if (err <= 0)
		return (err);
	freemsg(mp);
	return (0);
}

/*
 * M_PROTO, M_PCPROTO Handling
 * --------------------------------------------------------------------------
 */

static int
__tp_w_proto(struct xp *xp, queue_t *q, mblk_t *mp)
{
	t_uscalar_t prim;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		return (-EINVAL);
	prim = *(t_uscalar_t *) mp->b_rptr;
	switch (prim) {
	case T_CONN_REQ:
		return tp_conn_req(xp, q, mp);
	case T_CONN_RES:
		return tp_conn_res(xp, q, mp);
	case T_DISCON_REQ:
		return tp_discon_req(xp, q, mp);
	case T_DATA_REQ:
		return tp_data_req(xp, q, mp);
	case T_EXDATA_REQ:
		return tp_exdata_req(xp, q, mp);
	case T_INFO_REQ:
		return tp_info_req(xp, q, mp);
	case T_BIND_REQ:
		return tp_bind_req(xp, q, mp);
	case T_UNBIND_REQ:
		return tp_unbind_req(xp, q, mp);
	case T_UNITDATA_REQ:
		return tp_unitdata_req(xp, q, mp);
	case T_OPTMGMT_REQ:
		return tp_optmgmt_req(xp, q, mp);
	case T_ORDREL_REQ:
		return tp_ordrel_req(xp, q, mp);
	case T_OPTDATA_REQ:
		return tp_optdata_req(xp, q, mp);
	case T_ADDR_REQ:
		return tp_addr_req(xp, q, mp);
	case T_CAPABILITY_REQ:
		return tp_capability_req(xp, q, mp);
	default:
		return tp_other_req(xp, q, mp);
	}
}

static int
__np_w_proto(struct xp *xp, queue_t *q, mblk_t *mp)
{
	np_ulong prim;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		return (-EINVAL);
	prim = *(np_ulong *) mp->b_rptr;
	switch (prim) {
	case N_CONN_REQ:
		return np_conn_req(xp, q, mp);
	case N_CONN_RES:
		return np_conn_res(xp, q, mp);
	case N_DISCON_REQ:
		return np_discon_req(xp, q, mp);
	case N_DATA_REQ:
		return np_data_req(xp, q, mp);
	case N_EXDATA_REQ:
		return np_exdata_req(xp, q, mp);
	case N_INFO_REQ:
		return np_info_req(xp, q, mp);
	case N_BIND_REQ:
		return np_bind_req(xp, q, mp);
	case N_UNBIND_REQ:
		return np_unbind_req(xp, q, mp);
	case N_UNITDATA_REQ:
		return np_unitdata_req(xp, q, mp);
	case N_OPTMGMT_REQ:
		return np_optmgmt_req(xp, q, mp);
	case N_DATACK_REQ:
		return np_datack_req(xp, q, mp);
	case N_RESET_REQ:
		return np_reset_req(xp, q, mp);
	case N_RESET_RES:
		return np_reset_res(xp, q, mp);
	default:
		return np_other_req(xp, q, mp);
	}
}

static int
__xp_w_proto(struct xp *xp, queue_t *q, mblk_t *mp)
{
	switch (xp->type) {
	default:
	case X25_STYPE_MGMT:
	case X25_STYLE_NPI:
		return __np_w_proto(xp, q, mp);
	case X25_STYLE_TPI:
		return __tp_w_proto(xp, q, mp);
	}
}

static int
xp_w_proto(queue_t *q, mblk_t *mp)
{
	struct xp *xp;
	int err = -EAGAIN;

	if ((xp = xp_acquire(q))) {
		uint oldstate = xp_get_state(xp);

		if ((err = __xp_w_proto(xp, q, mp)))
			xp_set_state(q, xp, oldstate);
		xp_release(xp);
	}
	return (err < 0 ? err : 0);
}

static int
__dl_r_proto(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_ulong prim;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		return (-EINVAL);

	prim = *(dl_ulong *) mp->b_rptr;
	switch (prim) {
	case DL_UNITDATA_IND:
	case DL_UDERROR_IND:
	case DL_DATA_ACK_IND:
	case DL_DATA_ACK_STATUS_IND:
	case DL_REPLY_IND:
	case DL_REPLY_STATUS_IND:
		mi_strlog(q, STRLOGDA, SL_TRACE, "%s [%u] <-", dl_primname(prim),
			  (int) msgdsize(mp->b_cont));
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "%s <-", dl_primname(prim));
	}

	switch (*(dl_ulong *) mp->b_rptr) {
	case DL_INFO_ACK:
		return dl_info_ack(dl, q, mp);
	case DL_BIND_ACK:
		return dl_bind_ack(dl, q, mp);
	case DL_OK_ACK:
		return dl_ok_ack(dl, q, mp);
	case DL_ERROR_ACK:
		return dl_error_ack(dl, q, mp);
	case DL_SUBS_BIND_ACK:
		return dl_subs_bind_ack(dl, q, mp);
	case DL_UNITDATA_IND:
		return dl_unitdata_ind(dl, q, mp);
	case DL_UDERROR_IND:
		return dl_uderror_ind(dl, q, mp);
	case DL_CONNECT_IND:
		return dl_connect_ind(dl, q, mp);
	case DL_CONNECT_CON:
		return dl_connect_con(dl, q, mp);
	case DL_TOKEN_ACK:
		return dl_token_ack(dl, q, mp);
	case DL_DISCONNECT_IND:
		return dl_disconnect_ind(dl, q, mp);
	case DL_RESET_IND:
		return dl_reset_ind(dl, q, mp);
	case DL_RESET_CON:
		return dl_reset_cont(dl, q, mp);
	case DL_DATA_ACK_IND:
		return dl_data_ack_ind(dl, q, mp);
	case DL_DATA_ACK_STATUS_IND:
		return dl_data_ack_status_ind(dl, q, mp);
	case DL_REPLY_IND:
		return dl_reply_ind(dl, q, mp);
	case DL_REPLY_STATUS_IND:
		return dl_reply_status_ind(dl, q, mp);
	case DL_REPLY_UPDATE_STATUS_IND:
		return dl_reply_update_status_ind(dl, q, mp);
	case DL_XID_IND:
		return dl_xid_ind(dl, q, mp);
	case DL_XID_CON:
		return dl_xid_con(dl, q, mp);
	case DL_TEST_IND:
		return dl_test_ind(dl, q, mp);
	case DL_TEST_CON:
		return dl_test_con(dl, q, mp);
	case DL_PHYS_ADDR_ACK:
		return dl_phys_addr_ack(dl, q, mp);
	case DL_GET_STATISTICS_ACK:
		return dl_get_statistics_ack(dl, q, mp);
	default:
		return dl_other_ind(dl, q, mp);
	}
}

static int
dl_r_proto(queue_t *q, mblk_t *mp)
{
	struct dl *dl;
	int err = -EAGAIN;

	if ((dl = dl_acquire(q))) {
		uint oldstate = dl_get_state(dl);

		if ((err = __dl_r_proto(dl, q, mp)))
			dl_set_state(q, dl, oldstate);
		dl_release(dl);
	}
	return (err < 0 ? err : 0);
}

/*
 * M_DATA Handling
 * --------------------------------------------------------------------------
 */
static int
np_write(struct xp *xp, queue_t *q, mblk_t *mp)
{
}
static int
tp_write(struct xp *xp, queue_t *q, mblk_t *mp)
{
}
static int
__xp_w_data(struct xp *xp, queue_t *q, mblk_t *mp)
{
	switch (xp->type) {
	default:
	case X25_STYLE_MGMT:
	case X25_STYLE_NPI:
		return np_write(xp, q, mp);
	case X25_STYLE_TPI:
		return tp_write(xp, q, mp);
	}
}

static inline fastcall int
xp_w_data(queue_t *q, mblk_t *mp)
{
	struct xp *xp;
	int err = -EAGAIN;

	if (likely((xp = xp_acquire(q)) != NULL)) {
		err = __xp_w_data(xp, q, mp);
		xp_release(xp);
	}
	return (err);
}

static int
dl_read(struct dl *dl, queue_t *q, mblk_t *mp)
{
}
static int
dl_r_data(queue_t *q, mblk_t *mp)
{
	struct dl *dl;
	int err = -EAGAIN;

	if (likely(!!(dl = dl_acquire(q)))) {
		err = dl_read(dl, q, mp);
		dl_release(dl);
	}
	return (err);
}

/*
 * M_SIG, M_PCSIG Handling
 * --------------------------------------------------------------------------
 */
static int
do_timeout(queue_t *q, mblk_t *mp)
{
	struct x25_timer *t = (typeof(t)) mp->b_rptr;

	mi_strlog(q, STRLOGTO, SL_TRACE, "%s expiry at %lu", xp_timername(t->timer), jiffies);

	switch (t->timer) {
	case t1:
		return xp_t1_timeout(q, t->arg);
	default:
		return (0);
	}
}

static int
__xp_w_sig(struct xp *xp, queue_t *q, mblk_t *mp)
{
	uint oldstate = xp_get_state(xp);
	int err = 0;

	if (likely(mi_timer_valid(mp))) {
		if ((err = do_timeout(q, mp))) {
			xp_set_state(xp, oldstate);
			err = mi_timer_requeue(mp) ? err : 0;
		}
	}
	return (err < 0 ? err : 0);
}

static int
xp_w_sig(queue_t *q, mblk_t *mp)
{
	struct xp *xp;
	int err = 0;

	if (!(xp = xp_acquire(q)))
		return (mi_timer_requeue(mp) ? -EAGAIN : 0);
	err = __xp_w_sig(xp, q, mp);
	xp_release(xp);
	return (err);
}

static int
__dl_r_sig(struct dl *dl, queue_t *q, mblk_t *mp)
{
	uint oldstate = dl_get_state(dl);
	int err = 0;

	if (likely(mi_timer_valid(mp))) {
		if ((err = do_timeout(q, mp))) {
			dl_set_state(dl, oldstate);
			err = mi_timer_requeue(mp) ? err : 0;
		}
	}
	return (err < 0 ? err : 0);
}

static int
dl_r_sig(queue_t *q, mblk_t *mp)
{
	struct dl *dl;
	int err = 0;

	if (!(dl = dl_acquire(q)))
		return (mi_timer_requeue(mp) ? -EAGAIN : 0);
	err = __dl_r_sig(xp, q, mp);
	dl_release(dl);
	return (err);
}

/*
 * M_RSE, M_PCRSE Handling
 * --------------------------------------------------------------------------
 */
static int
xp_w_rse(queue_t *q, mblk_t *mp)
{
	return (EPROTO);
}
static int
dl_r_rse(queue_t *q, mblk_t *mp)
{
	return (EPROTO);
}

/*
 * Unknown STREAMS Message Handling
 * --------------------------------------------------------------------------
 */
static int
xp_w_other(queue_t *q, mblk_t *mp)
{
	return (EOPNOTSUPP);
}
static int
dl_r_other(queue_t *q, mblk_t *mp)
{
	return (EOPNOTSUPP);
}

/*
 * M_FLUSH Handling
 * --------------------------------------------------------------------------
 */
static int
xp_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_prtr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(RD(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

static int
dl_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHR;
	}
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(WR(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(WR(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/*
 * M_ERROR Handling
 * --------------------------------------------------------------------------
 */
static int
dl_r_error(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);

	dl_set_state(dl, LMI_UNUSABLE);
	fixme(("Complete this function\n"));
	return (-EFAULT);
}

/*
 * M_HANGUP Handling
 * --------------------------------------------------------------------------
 */
static int
dl_r_hangup(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);

	dl_set_state(dl, LMI_UNUSABLE);
	fixme(("Complete this function\n"));
	return (-EFAULT);
}

/*
 *  =========================================================================
 *
 *  NPI Interface
 *
 *  =========================================================================
 */
/*
 * Message and Primitive Handling for the XX25 Interface.
 * ------------------------------------------------------
 */

static noinline fastcall int
xp_w_msg_put_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return xp_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return xp_w_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return xp_w_sig(q, mp);
	case M_RSE:
	case M_PCRSE:
		return xp_w_rse(q, mp);
	case M_CTL:
	case M_PCCTL:
		return xp_w_ctl(q, mp);
	case M_IOCTL:
		return xp_w_ioctl(q, mp);
	case M_IOCDATA:
		return xp_w_iocdata(q, mp);
	case M_FLUSH:
		return xp_w_flush(q, mp);
	default:
		return xp_w_other(q, mp);
	}
}
static noinline fastcall int
xp_w_msg_srv_slow(struct xp *xp, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return __xp_w_data(xp, q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return __xp_w_proto(xp, q, mp);
	case M_SIG:
	case M_PCSIG:
		return __xp_w_sig(xp, q, mp);
	case M_RSE:
	case M_PCRSE:
		return __xp_w_rse(xp, q, mp);
	case M_CTL:
	case M_PCCTL:
		return __xp_w_ctl(xp, q, mp);
	case M_IOCTL:
		return __xp_w_ioctl(xp, q, mp);
	case M_IOCDATA:
		return __xp_w_iocdata(xp, q, mp);
	case M_FLUSH:
		return xp_w_flush(q, mp);
	default:
		return xp_w_other(q, mp);
	}
}

static inline fastcall int __hot_put
xp_w_msg_put(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return xp_w_data(q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO)) {
		if (likely(MBLKIN(mp, 0, sizeof(np_ulong)))) {
			if (likely(*(np_ulong *) mp->b_rptr == N_DATA_REQ))
				return np_data_req(q, mp);
			if (likely(*(np_ulong *) mp->b_rptr == N_UNITDATA_REQ))
				return np_unitdata_req(q, mp);
		}
	}
	return xp_w_msg_put_slow(q, mp);
};
static inline fastcall int __hot_read
xp_w_msg_srv(struct xp *xp, queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return __xp_w_data(xp, q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO)) {
		if (likely(MBLKIN(mp, 0, sizeof(np_ulong)))) {
			if (likely(*(np_ulong *) mp->b_rptr == N_DATA_REQ))
				return __np_data_req(xp, q, mp);
			if (likely(*(np_ulong *)mp->b_rptr == N_UNITDATA_REQ))
				return __np_unitdata_req(xp, q, mp);
		}
	}
	return xp_w_msg_srv_slow(xp, q, mp);
};
static int
xp_r_msg_srv(struct xp *xp, queue_t *q, mblk_t *mp)
{
};
static int
xp_r_msg_put(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return xp_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return xp_w_proto(q, mp);
	case M_FLUSH:
		return xp_w_flush(q, mp);
	case M_IOCTL:
		return xp_w_ioctl(q, mp);
	case M_IOCDATA:
		return xp_w_iocdata(q, mp);
	default:
		return xp_w_other(q, mp;);
	}
};

/*
 *  =========================================================================
 *
 *  DLPI Interface
 *
 *  =========================================================================
 */
static int
dl_r_msg_slow(queue_t *q, mblk_t *mp)
{
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
	case M_RSE:
	case M_PCRSE:
		return dl_r_rse(q, mp);
	case M_CTL:
	case M_PCCTL:
		return dl_r_ctl(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
	default:
		return dl_r_other(q, mp);
	case M_FLUSH:
		return dl_r_flush(q, mp);
	case M_ERROR:
		return dl_r_error(q, mp);
	case M_HANGUP:
		return dl_r_hangup(q, mp);
	}
};

static inline fastcall int __hot_in
dl_r_msg(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return dl_r_data(q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		if (likely(MBLKIN(mp, 0, sizeof(dl_ulong))))
			if (likely(*(np_ulong *) mp->b_rptr == DL_UNITDATA_IND))
				return dl_unitdata_ind(q, mp);
	return dl_r_msg_slow(q, mp);
}

/*
 * ==========================================================================
 *
 * PUT AND SERVICE PROCEDURES
 *
 * ==========================================================================
 */

/*
 * Put and Service procedures for the XX25 interface.
 * --------------------------------------------------------------------------
 */

/**
 * xp_wput: upper multiplex write queue put procedure
 * @q: upper multiplex write queue
 * @mp: the message to put
 *
 * Canonical STREAMS put procedure.
 */
static int
xp_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || xp_w_msg_put(q, mp))
		putq(q, mp);
	return (0);
};

/**
 * xp_wsrv: upper multiplex write queue service procedure
 * @q: the upper multiplex write queue to service
 *
 * Canonical STREAMS service procedure with external locking.
 */
static int
xp_wsrv(queue_t *q)
{
	mblk_t *mp;

	if (likely(!!(mp = getq(q)))) {
		struct plp *plp;

		if (likely(!!(plp = plp_acquire(q)))) {
			do {
				if (xp_w_msg_srv(plp, q, mp))
					break;
			} while (likely(!!(mp = getq(q))));
			plp_release(plp);
		}
	}
	if (mp != NULL)
		putbq(q, mp);
	return (0);
};

/**
 * xp_rsrv: upper multiplex read queue service procedure.
 * @q: upper multiplex read queue
 * 
 * Canonical STREAMS service procedure with external locking.
 */
static int
xp_rsrv(queue_t *q)
{
	mblk_t *mp;

	if (likely(!!(mp = getq(q)))) {
		struct plp *plp;

		if (likely(!!(plp = plp_acquire(q)))) {
			do {
				if (xp_r_msg_srv(plp, q, mp))
					break;
			} while (likely(!!(mp = getq(q))));
			plp_release(plp);
		}
	}
	if (mp != NULL)
		putbq(q, mp);
	return (0);
};

/**
 * xp_rput: upper multiplex read queue put procedure
 * @q: upper multiplex read queue
 * @mp: the message to put
 *
 * This is where the SNDCF (lower DL Stream) places messages that are destined
 * for this Stream.  They are formatted as lower multiplex DLPI
 * DL_UNITDATA_IND or DLPI DL_UDERROR_IND or DLP DATA_IND (M_DATA) message.
 * We use three bands for this purpose: band 0 for data, band 1 for expedited
 * data, and band 2 for error indications.  The feeding lower Stream need not
 * check flow control before calling this put() procedure, but needs to set
 * the band number of the message before calling put().
 */
static int
xp_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || xp_r_msg_put(q, mp)) {
		if (!bcanput(q, mp->b_band) || !putq(q, mp)) {
			mp->b_band = 0;
			putq(q, mp);
		}
	}
	return (0);
};

/*
 * Put and Service Procedures for the DL-Provider interface.
 * --------------------------------------------------------------------------
 */

/**
 * dl_wsrv: lower write qeuue service procedure
 * @q: lower write queue
 *
 * This function is only invoked when flow controlled downstream on the write
 * side and the flow control has subsided resulting in this queue being
 * back-enabled.  Downstream write side flow control only occurs when an lower
 * fails bcanpuntext() for the queue and places the message back on its own
 * write queue.  Therefore, when back-enabled we need to wake all the flow
 * controlled upper Streams.  This function tries to be intelligent by
 * disadvantaging noisy upper Streams to keep them from starving other NSAPs
 * on the same SNPA.  This is done by remembering which sender first
 * encountered the congestion (likely the noisy one).  The sender that first
 * encountered congestion (called the loud stream), is enabled last (in its
 * priority band) and will thus be last ot be serviced by the STREAMS
 * scheduler on this processor.  All other blocked streams go first, but in
 * order of priority.
 *
 * Note that messages are never placed on this queue: flow control is tested
 * downstream from this queue and if it fails, the invoking message is placed
 * back on the upper queue or an invoking timer is rescheduled.
 */
static streamscall int __hot_out
dl_wsrv(queue_t *q)
{
	struct plp *plp, *p = PLP_PRIV(q);

	if (!(p = plp_mux_acquire(q)))
		return;
	if (p->blocked) {
		for (plp = p->plp.list; plp; plp = plp->plp.next) {
			if (plp->blocked) {
				if (plp == p->loud)
					continue;
				plp->blocked = 0;
				qenable(plp->wq);
			}
		}
		if (p->loud != NULL) {
			qenable(p->loud->wq);
			p->loud = NULL;
		}
		p->blocked = 0;
	}
	plp_mux_release(p);
	return (0);
};

/**
 * dl_rsrv: lower multiplex read queue service procedure
 * @q: lower multiplex read queue
 *
 * Canonical STREAMS service procedure.
 */
static streamscall int __hot_read
dl_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q)))
		if (dl_r_msg(q, mp))
			break;
	if (mp != NULL)
		putbq(q, mp);
	return (0);
};

/**
 * dl_rput: lower multiplex read queue put procedure
 * @q: lower multiplex read queue
 * @mp: the message to put
 *
 * Canonical put procedure with interrupt protection.
 *
 * Whe check whether we are in an interrupt to keep from running plp_n_msg()
 * in interrupt context.  This protects the entire driver from interrupt
 * context processing and makes all locks simple spin locks that do not need
 * BH or IRQ protection.
 */
static int
dl_rput(queue_t *q, mblk_t *mp)
{
	if (in_interrupt() || (!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || dl_r_msg(q, mp))
		putq(q, mp);
	return (0);
};

/*
 * ==========================================================================
 *
 * OPEN AND CLOSE ROUTINES
 *
 * ==========================================================================
 */
#ifndef NUM_PLP
#define NUM_PLP 1		/* we only have one PLP subsystem, but this could change */
#endif
static caddr_t plp_opens = NULL;
static struct xp *lm_ctrl = NULL;

DECLARE_WAIT_QUEUE_HEAD(plp_waitq);

/*
 * Open and Close Routines for the XX25 interface.
 * -----------------------------------------------
 */

/**
 * xp_qopen: - STREAMS driver open routine TPI interface
 * @q: read queue of newly created queue pair
 * @devp: pointer to device number associated with the Stream
 * @oflags: flags to the open(2s) call
 * @sflag: STREAMS flag
 * @crp: pointer to the credentials of the opening process
 *
 * When a Stream is opened on the driver it corresponds to a PLP subsystem associated with the
 * driver.  The PLP is determined from the minor device opened.  All minor devices corresponding to
 * PLP subsystems are clone or auto-clone devices.  There may be several DL lower Streams for each
 * PLP.  If a PLP structure has not been allocated for the corresponding minor device number, we
 * allocate one.  When a DL Stream is I_LINK'ed under the driver, it is associated with the PLP
 * structure.  A XP structure is allocated and associated with each upper Stream.
 *
 * This driver cannot be pushed as a module.
 *
 * (cminor == 0) && (sflag == DRVOPEN)
 *	When minor device number 0 is opened with DRVOPEN (non-clone), a control Stream is opened.
 *	If a control Stream has already been opened, the open is refused.  The @sflag is changed
 *	from %DRVOPEN to %CLONEOPEN and a new minor device number above NUM_PLP is assigned.  This
 *	uses the autocloning features of Linux Fast-STREAMS.  This corresponds to the
 *	/dev/streams/plp/lm minor device node.
 *
 * (cminor == 0) && (sflag == CLONEOPEN)
 *	This is a normal clone open using the clone(4) driver.  A disassociated user Stream is
 *	opened.  A new unique minor device number above NUM_PLP is assigned.  This corresponds to
 *	the /dev/streams/clone/plp clone device node.
 *
 * (1 <= cminor && cminr <= NUM_PLP)
 *	This is a normal non-clone open.  Where the minor device number is between 1 and NUM_PLP, an
 *	associated user Stream is opened.  If there is no PLP structure to associate, one is created
 *	with default values (and associated with default values).  A new minor device number above
 *	NUM_PLP is assigned.  This uses the autocloning features of Linux Fast-STREAMS.  This
 *	corresponds to the /dev/streams/plp/NNNN minor device node where NNNN is the minor device
 *	number.
 */
static streamscall int
xp_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct xp *xp;
	pl_t pl;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next) {
		mi_strlog(q, 0, SL_ERROR, "cannot push as module");
		return (ENXIO);
	}
	if (cminor > NUM_PLP) {
		mi_strlog(q, 0, SL_ERROR, "cannot open dynamic minor device number");
		return (ENXIO);
	}
	if (cmajor != T_CMAJOR_0 || cminor >= PLP_CMINOR_FREE) {
		mi_strlog(q, 0, SL_ERROR, "cannot open cloned minors");
		return (ENXIO);
	}
	if (!(xp = xp_alloc_priv(q, devp, crp, cminor)))
		return (ENOMEM);
	*devp = makedevice(cmajor, NUM_PLP + 1);
	/* start assigning minors at NUM_PLP + 1 */
	write_lock_irqsave(&plp_mux_lock, pl);
	if (cminor == 0) {
		/* When a zero minor device number was opened, the Stream is either a clone open or
		   an attempt to open the master control Stream.  The difference is whether the
		   @sflag was %DRVOPEN or %CLONEOPEN. */
		if (sflag == DRVOPEN) {
			/* When the master control Stream is opened, another master control Stream
			   must not yet exist.  If this is the only master control Stream then it
			   is created. */
			if (lm_ctrl != NULL) {
				write_unlock_irqrestore(&plp_mux_lock, pl);
				xp_free_priv(xp);
				return (ENXIO);
			}
		}
		if ((err = mi_open_link(&plp_opens, (caddr_t) xp, devp, oflags, CLONEOPEN, crp))) {
			write_unlock_irqrestore(&plp_mux_lock, pl);
			xp_free_priv(xp);
			return (err);
		}
		if (sflag == DRVOPEN)
			lm_ctrl = xp;
		/* Both master control Stream and clone user Streams are disassociated with any
		   specific PLP subsystem.  Master control Streams are never associated with a
		   specific PLP subsystem.  User Streams are associated with a PLP using the snid
		   in the PPA to the attach primitive, or when a DL Stream is temporarily linked
		   under the driver using the I_LINK input-output control. */
	} else {
		DECLARE_WAITQUEUE(wait, current);
		struct plp *plp;

		/* When a non-zero minor device number was opened, the Stream is automatically
		   associated with the PLP to which the minor device nuber corresponds.  It cannot
		   be disassociated except when it is closed. */
		if (!(plp = plp_lookup(cminor))) {
			write_unlock_irqrestore(&plp_mux_lock, pl);
			xp_free_priv(xp);
			return (ENXIO);
		}
		/* Locking: need to wait until a lock on the PLP structure can be acquired, or a
		   signal is received, or the PLP structure is deallocated.  If the lock can be
		   acquired, associate the User Stream with the PLP structure; in all other cases,
		   return an error.  Note that it is a likely event that the lock can be acquired
		   without waiting. */
		err = 0;
		add_wait_queue(&plp_waitq, &wat);
		spin_lock(&xp->sq.lock);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			if (signal_pending(current)) {
				err = EINTR;
				spin_unlock(&xp->sq.lock);
				break;
			}
			if (xp->sq.users != 0) {
				spin_unlock(&xp->sq.lock);
				write_unlock_irqrestore(&plp_mux_lock, pl);
				if (!(xp = xp_lookup(cminor))) {
					err = ENXIO;
					break;
				}
				spin_lock(&xp->sq.lock);
				continue;
			}
			err = mi_open_link(&plp_opens, (caddr_t) xp, devp, oflags, CLONEOPEN, crp);
			if (err == 0)
				xp_attach(plp, xp, 0);
			spin_unlock(&xp->sq.lock);
			break;
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&plp_waitq, &wait);
		if (err) {
			write_unlock_irqrestore(&plp_mux_lock, pl);
			xp_free_priv(xp);
			return (err);
		}
	}
	write_unlock_irqrestore(&plp_mux_lock, pl);
	mi_attach(q, (caddr_t) xp);
	qprocson(q);
	return (0);
}

/**
 * xp_qclose: - STREAMS driver close routine NPI interface
 * @q: queue pair
 * @oflags: flags to the open(2s) call
 * @crp: pointer to the credentials of the closing process
 */
static streamscall int
xp_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct xp *xp = XP_PRIV(q);
	pl_t pl;
	struct plp *plp;

	qprocsoff(q);
	mi_detach(q, (caddr_t) xp);
	write_lock_irqsave(&plp_mux_lock, pl);
	if ((plp = xp->plp.loc)) {
		DECLARE_WAITQUEUE(wait, current);

		/* Locking: need to wait until a PLP lock can be acquired, or the PLP structure is
		   deallocated.  If a lock can be acquired, the closing Stream is disassociated
		   with the PLP; otherwise, if the PLP structure is deallocated, there is no
		   further need to disassociate.  Note that it is a likely event that the lock can
		   be acquired without waiting. */
		add_wait_queue(&plp_waitq, &wait);
		spin_lock(&xp->sq.lock);
		for (;;) {
			set_current_state(TASK_INTERRUPTIBLE);
			if (sp->sq.users == 0) {
				xp_detach(xp);
				spin_unlock(&xp->sq.lock);
				break;
			}
			spin_unlock(&xp->sq.lock);
			write_unlock_irqrestore(&plp_mux_lock, pl);
			schedule();
			write_lock_irqsave(&plp_mux_lock, pl);
			if (!(plp = xp->plp.loc))
				break;
			spin_lock(&xp->sq.lock);
		}
		set_current_state(TASK_RUNNING);
		remove_wait_queue(&plp_waitq, &wait);
	}
	mi_close_unlink(&plp_opens, (caddr_t) xp);
	write_unlock_irqrestore(&plp_mux_lock, pl);
	xp_free_priv(xp);
	return (0);
}

/*
 * ==========================================================================
 *
 * REGISTRATION AND INITIALIZATION
 *
 * ==========================================================================
 */

static struct qinit plp_rdinit = {
	.qi_putp = xp_rput,
	.qi_srvp = xp_rsrv,
	.qi_qopen = xp_qopen,
	.qi_qclose = xp_qclose,
	.qi_minfo = &xp_minfo,
	.qi_mstat = &xp_rstat,
};
static struct qinit plp_wrinit = {
	.qi_putp = xp_wput,
	.qi_srvp = xp_wsrv,
	.qi_minfo = &xp_minfo,
	.qi_mstat = &xp_wstat,
};
static struct qinit plp_muxrinit = {
	.qi_putp = dl_rput,
	.qi_srvp = dl_rsrv,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_rstat,
};
static struct qinit plp_muxwinit = {
	.qi_putp = NULL,
	.qi_srvp = dl_wsrv,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_wstat,
};


struct streamtab plpinfo = {
	.st_rdinit = &plp_rdinit,
	.st_wrinit = &plp_wrinit,
	.st_muxrinit = &plp_muxrinit,
	.st_muxwinit = &plp_muxwinit,
};

/*
 * Registration and initialization
 * ==========================================================================
 */

#ifdef LINUX

unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the X25 PLP driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Device number for the X25 PLP driver. (0 for allocation.)");

static struct cdevsw plp_cdev = {
	.d_name = DRV_NAME,
	.d_str = &plpinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

MODULE_STATIC int __init
plpinit(void)
{
}

MODULE_STATIC void __exit
plpterminate(void)
{
}

module_init(plpinit);
module_exit(plpterminate);

#endif				/* LINUX */
