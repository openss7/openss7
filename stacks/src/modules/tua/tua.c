/*****************************************************************************

 @(#) $RCSfile: tua.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2005/03/31 06:53:18 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@dallas.net>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/03/31 06:53:18 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: tua.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2005/03/31 06:53:18 $"

static char const ident[] =
    "$RCSfile: tua.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2005/03/31 06:53:18 $";

#include "os7/compat.h"

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mtpi.h>
#include <ss7/mtpi_ioctl.h>
#include <ss7/sccpi.h>
#include <ss7/sccpi_ioctl.h>
#include <ss7/tcap.h>
#include <ss7/tcap_ioctl.h>

#include <sys/tihdr.h>
#include <sys/xti.h>
#include <sys/xti_mtp.h>
#include <sys/xti_sccp.h>
#include <sys/xti_tcap.h>

/*
 *  TUA MULTIPLEXOR
 *  -------------------------------------------------------------------------
 *  This is a multiplexing driver for TUA.  When TUA is opened by its control
 *  stream (typically a ua configuration daemon), it provides a control
 *  channel for configuration and routing.  When opened normally, by a user
 *  process, it provides an OpenSS7 TCAP Provider.  OpenSS7 TCAP Provider
 *  streams are I_LINKed and I_PLINKed under the multiplexor for Signalling
 *  Gateway (SG) Use by the TUA control stream or TUA user stream.  TCAP
 *  Provider streams can be opened for Application Server (AS) use.  Transport
 *  streams (typically SCTP) are I_LINKed or I_PLINKed under the multiplexor
 *  for both SG and AS use by the UA/LM control stream.
 */

#define TUA_DESCRIP	"TUA STREAMS MULTIPLEXING DRIVER."
#define TUA_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define TUA_REVISION	"OpenSS7 $RCSfile: tua.c,v $ $Name:  $ ($Revision: 0.9.2.5 $) $Date: 2005/03/31 06:53:18 $"
#define TUA_COPYRIGHT	"Copyright (c) 1997-2004 OpenSS7 Corporation.  All Rights Reserved."
#define TUA_DEVICE	"Supports OpenSS7 TCAP TCI/TRI Interface Pseudo-Device Drivers."
#define TUA_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TUA_LICENSE	"GPL"
#define TUA_BANNER	TUA_DESCRIP	"\n" \
			TUA_EXTRA	"\n" \
			TUA_REVISION	"\n" \
			TUA_COPYRIGHT	"\n" \
			TUA_DEVICE	"\n" \
			TUA_CONTACT
#define TUA_SPLASH	TUA_DESCRIP	"\n" \
			TUA_REVISION

#ifdef LINUX
MODULE_AUTHOR(TUA_CONTACT);
MODULE_DESCRIPTION(TUA_DESCRIP);
MODULE_SUPPORTED_DEVICE(TUA_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TUA_LICENSE);
#endif				/* MODULE_LICENSE */
#endif				/* LINUX */

#ifdef LFS
#define TUA_DRV_ID	CONFIG_STREAMS_TUA_MODID
#define TUA_DRV_NAME	CONFIG_STREAMS_TUA_NAME
#define TUA_CMAJORS	CONFIG_STREAMS_TUA_NMAJORS
#define TUA_CMAJOR_0	CONFIG_STREAMS_TUA_MAJOR
#define TUA_UNITS	CONFIG_STREAMS_TUA_NMINORS
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define DRV_ID		TUA_DRV_ID
#define DRV_NAME	TUA_DRV_NAME
#define CMAJORS		TUA_CMAJORS
#define CMAJOR_0	TUA_CMAJOR_0
#define UNITS		TUA_UNITS
#ifdef MODULE
#define DRV_BANNER	TUA_BANNER
#else				/* MODULE */
#define DRV_BANNER	TUA_SPLASH
#endif				/* MODULE */

static struct module_info tua_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 1,			/* Min packet size accepted *//* XXX */
	.mi_maxpsz = 512,		/* Max packet size accepted *//* XXX */
	.mi_hiwat = 8 * 512,		/* Hi water mark *//* XXX */
	.mi_lowat = 1 * 512,		/* Lo water mark *//* XXX */
};

STATIC struct module_stat tua_mstat = { 0, };

/*
 *  There are four possible upper TCAP interfaces: LMI, TCI, TRI and TPI
 */

STATIC int tc_rdprim(queue_t *, mblk_t *);
STATIC int tc_wrprim(queue_t *, mblk_t *);

STATIC int tr_rdprim(queue_t *, mblk_t *);
STATIC int tr_wrprim(queue_t *, mblk_t *);

STATIC int tp_rdprim(queue_t *, mblk_t *);
STATIC int tp_wrprim(queue_t *, mblk_t *);

STATIC int lm_rdprim(queue_t *, mblk_t *);
STATIC int lm_wrprim(queue_t *, mblk_t *);

static struct qinit tua_rdinit = {
	.qi_putp = ua_rput,		/* Read put (msg from below) */
	.qi_srvp = ua_rsrv,		/* Read queue service */
	.qi_qopen = ua_open,		/* Each open */
	.qi_qclose = ua_close,		/* Last close */
	.qi_qadmin = NULL,		/* Admin (not used) */
	.qi_minfo = &tua_minfo,		/* Information */
	.qi_mstat = &tua_mstat,		/* Statistics */
};

static struct qinit tua_wrinit = {
	.qi_putp = ua_wput,		/* Write put (msg from above) */
	.qi_srvp = ua_wsrv,		/* Write queue service */
	.qi_qopen = NULL,		/* Each open */
	.qi_qclose = NULL,		/* Last close */
	.qi_qadmin = NULL,		/* Admin (not used) */
	.qi_minfo = &tua_minfo,		/* Information */
	.qi_mstat = &tua_mstat,		/* Statistics */
};

/*
 *  There are six possible lower interfaces: TCAP TCI, TCAP TRI, TCAP TPI, SCTP
 *  NPI ASP, SCTP NPI SGP, SCTP NPI SPP
 */

STATIC int tc_muxrprim(queue_t *, mblk_t *);
STATIC int tc_muxwprim(queue_t *, mblk_t *);

STATIC int tr_muxrprim(queue_t *, mblk_t *);
STATIC int tr_muxwprim(queue_t *, mblk_t *);

STATIC int tp_muxrprim(queue_t *, mblk_t *);
STATIC int tp_muxwprim(queue_t *, mblk_t *);

STATIC int as_muxrprim(queue_t *, mblk_t *);
STATIC int as_muxwprim(queue_t *, mblk_t *);

STATIC int sg_muxrprim(queue_t *, mblk_t *);
STATIC int sg_muxwprim(queue_t *, mblk_t *);

STATIC int sp_muxrprim(queue_t *, mblk_t *);
STATIC int sp_muxwprim(queue_t *, mblk_t *);

static struct qinit tua_muxrinit = {
	.qi_putp = ss7_iput,		/* Read put (msg from below) */
	.qi_srvp = ss7_isrv,		/* Read queue service */
	.qi_qopen = NULL,		/* Each open */
	.qi_qclose = NULL,		/* Last close */
	.qi_qadmin = NULL,		/* Admin (not used) */
	.qi_minfo = &tua_minfo,		/* Information */
	.qi_mstat = &tua_mstat,		/* Statistics */
};

static struct qinit tua_muxwinit = {
	.qi_putp = ss7_oput,		/* Write put (msg from above) */
	.qi_srvp = ss7_osrv,		/* Write queue service */
	.qi_qopen = NULL,		/* Each open */
	.qi_qclose = NULL,		/* Last close */
	.qi_qadmin = NULL,		/* Admin (not used) */
	.qi_minfo = &tua_minfo,		/* Information */
	.qi_mstat = &tua_mstat,		/* Statistics */
};

MODULE_STATIC struct streamtab tuainfo = {
	.st_rdinit = &tua_rdinit,	/* Upper read queue */
	.st_wrinit = &tua_wrinit,	/* Upper write queue */
	.st_muxrinit = &tua_muxrinit,	/* Lower read queue */
	.st_muxwinit = &tua_muxwinit,	/* Lower write queue */
};

/*
 *  =========================================================================
 *
 *  Private Structures
 *
 *  =========================================================================
 */

/* TCAP User */
struct tua {
	STR_DECLARATION (struct tua);
};
STATIC struct tua *tua_alloc_priv();
STATIC void tua_free_priv();

/* Application Server Process */
struct asp {
	HEAD_DECLARATION (struct asp);
};
STATIC struct asp *tua_alloc_asp();
STATIC void tua_free_asp();

/* Application Server */
struct as {
	HEAD_DECLARATION (struct as);
};
STATIC struct as *tua_alloc_as();
STATIC void tua_free_as();

/* Signalling Gateway */
struct sg {
	HEAD_DECLARATION (struct sg);
};
STATIC struct sg *tua_alloc_sg();
STATIC void tua_free_sg();

/* Signalling Gateway Process */
struct sgp {
	HEAD_DECLARATION (struct sgp);
};
STATIC struct sgp *tua_alloc_sgp();
STATIC void tua_free_sgp();

/* SCTP Transport Provider */
struct xp {
	HEAD_DECLARATION (struct xp);
};
STATIC struct xp *tua_alloc_xp();
STATIC void tua_free_xp();

/* TCAP Provider */
struct tcap {
	HEAD_DECLARATION (struct tcap);
};
STATIC struct tcap *tua_alloc_tcap();
STATIC void tua_free_tcap();

/* Default */
struct df {
	spinlock_t lock;
	SLIST_HEAD (tua, tua);
	SLIST_HEAD (asp, asp);
	SLIST_HEAD (as, as);
	SLIST_HEAD (sg, sg);
	SLIST_HEAD (sgp, sgp);
	SLIST_HEAD (xp, xp);
	SLIST_HEAD (tcap, tcap);
};

STATIC struct df master = { SPIN_LOCK_UNLOCKED, };

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 */

/*
 *  M_IOCTL Handling
 *  -----------------------------------
 */
STATIC int
tua_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct tua *tua = TUA_PRIV(q);
	struct iocblk *iocp = (typeof(iocp)) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont : NULL;
	int cmd = iocp->ioc_cmnd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int ret = 0;
	switch (type) {
	case _IOCT_TYPE(__SID):
	{
		psw_t flags;
		struct tua *tua, **tuap;
		struct linkblk *lb;
		if (!(lb = arg)) {
			swerr();
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(I_PLINK):
			ptrace(("%s: %p: I_PLINK\n", DRV_NAME, tua));
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PLINK\n", DRV_NAME,
					tua));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_LINK):
			ptrace(("%s: %p: I_LINK\n", DRV_NAME, tua));
			MOD_INC_USE_COUNT;	/* keep module from unloading */
			spin_lock_irqsave(&master.lock, flags);
			{
				/* place in list in ascending index order */
				for (sccpp = &master.sccp.list;
				     *sccpp && (*sccpp)->u.mux.index < lb->l_index;
				     sccpp = &(*sccpp)->next) ;
				if ((sccp =
				     tua_alloc_sccp(lb->l_qbot, sccpp, lb->l_index,
						     iocp->ioc_cr))) {
					spin_unlock_irqrestore(&master.lock, flags);
					break;
				}
				MOD_DEC_USE_COUNT;
				ret = -ENOMEM;
			}
			spin_unlock_irqrestore(&master.lock, flags);
			break;
		case _IOC_NR(I_PUNLINK):
			ptrace(("%s: %p: I_PUNLINK\n", DRV_NAME, tua));
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PUNLINK\n", DRV_NAME,
					tua));
				ret = -EPERM;
				break;
			}
		case _IOC_NR(I_UNLINK):
			ptrace(("%s: %p: I_UNLINK\n", DRV_NAME, tua));
			spin_lock_irqsave(&master.lock, flags);
			{
				for (sccp = master.sccp.list; sccp; sccp = sccp->next)
					if (sccp->u.mux.index == lb->l_index)
						break;
				if (!sccp) {
					ret = -EINVAL;
					ptrace(("%s: %p: ERROR: Couldn't find I_UNLINK muxid\n",
						DRV_NAME, tua));
					spin_unlock_irqrestore(&master.lock, flags);
					break;
				}
				tua_free_sccp(sccp->iq);
				MOD_DEC_USE_COUNT;
			}
			spin_unlock_irqrestore(&master.lock, flags);
			break;
		default:
		case _IOC_NR(I_STR):
			ptrace(("%s: %p: ERROR: Unsupported STREAMS ioctl %c, %d\n", DRV_NAME, tua,
				(char) type, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case TUA_IOC_MAGIC:
	{
		if (count < size) {
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(TUA_IOCGOPTIONS):
			printd(("%s; %p: -> TUA_IOCGOPTIONS\n", DRV_NAME, tua));
			ret = tua_iocgoptions(q, tua, mp);
			break;
		case _IOC_NR(TUA_IOCSOPTIONS):
			printd(("%s; %p: -> TUA_IOCSOPTIONS\n", DRV_NAME, tua));
			ret = tua_iocsoptions(q, tua, mp);
			break;
		case _IOC_NR(TUA_IOCGCONFIG):
			printd(("%s; %p: -> TUA_IOCGCONFIG\n", DRV_NAME, tua));
			ret = tua_iocgconfig(q, tua, mp);
			break;
		case _IOC_NR(TUA_IOCSCONFIG):
			printd(("%s; %p: -> TUA_IOCSCONFIG\n", DRV_NAME, tua));
			ret = tua_iocsconfig(q, tua, mp);
			break;
		case _IOC_NR(TUA_IOCTCONFIG):
			printd(("%s; %p: -> TUA_IOCTCONFIG\n", DRV_NAME, tua));
			ret = tua_ioctconfig(q, tua, mp);
			break;
		case _IOC_NR(TUA_IOCCCONFIG):
			printd(("%s; %p: -> TUA_IOCCCONFIG\n", DRV_NAME, tua));
			ret = tua_ioccconfig(q, tua, mp);
			break;
		case _IOC_NR(TUA_IOCGSTATEM):
			printd(("%s; %p: -> TUA_IOCGSTATEM\n", DRV_NAME, tua));
			ret = tua_iocgstatem(q, tua, mp);
			break;
		case _IOC_NR(TUA_IOCCMRESET):
			printd(("%s; %p: -> TUA_IOCCMRESET\n", DRV_NAME, tua));
			ret = tua_ioccmreset(q, tua, mp);
			break;
		case _IOC_NR(TUA_IOCGSTATSP):
			printd(("%s; %p: -> TUA_IOCGSTATSP\n", DRV_NAME, tua));
			ret = tua_iocgstatsp(q, tua, mp);
			break;
		case _IOC_NR(TUA_IOCSSTATSP):
			printd(("%s; %p: -> TUA_IOCSSTATSP\n", DRV_NAME, tua));
			ret = tua_iocsstatsp(q, tua, mp);
			break;
		case _IOC_NR(TUA_IOCGSTATS):
			printd(("%s; %p: -> TUA_IOCGSTATS\n", DRV_NAME, tua));
			ret = tua_iocgstats(q, tua, mp);
			break;
		case _IOC_NR(TUA_IOCCSTATS):
			printd(("%s; %p: -> TUA_IOCCSTATS\n", DRV_NAME, tua));
			ret = tua_ioccstats(q, tua, mp);
			break;
		case _IOC_NR(TUA_IOCGNOTIFY):
			printd(("%s; %p: -> TUA_IOCGNOTIFY\n", DRV_NAME, tua));
			ret = tua_iocgnotify(q, tua, mp);
			break;
		case _IOC_NR(TUA_IOCSNOTIFY):
			printd(("%s; %p: -> TUA_IOCSNOTIFY\n", DRV_NAME, tua));
			ret = tua_iocsnotify(q, tua, mp);
			break;
		case _IOC_NR(TUA_IOCCNOTIFY):
			printd(("%s; %p: -> TUA_IOCCNOTIFY\n", DRV_NAME, tua));
			ret = tua_ioccnotify(q, tua, mp);
			break;
		case _IOC_NR(TUA_IOCCMGMT):
			printd(("%s; %p: -> TUA_IOCCMGMT\n", DRV_NAME, tua));
			ret = tua_ioccmgmt(q, tua, mp);
			break;
		case _IOC_NR(TUA_IOCCPASS):
			printd(("%s; %p: -> TUA_IOCCPASS\n", DRV_NAME, tua));
			ret = tua_ioccpass(q, tua, mp);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unsupported TUA ioctl %c, %d\n", DRV_NAME, tua,
				(char) type, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	default:
		ptrace(("%s: %p: ERROR: Unsupported ioctl %c, %d\n", DRV_NAME, tua, (char) type,
			nr));
		ret = -EOPNOTSUPP;
		break;
	}
	if (ret > 0) {
		return (ret);
	} else if (ret == 0) {
		mp->b_datap->db_type = M_IOCACK;
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
	} else {
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_error = -ret;
		iocp->ioc_rval = -1;
	}
	qreply(q, mp);
	return (QR_ABSORBED);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
STATIC major_t tua_majors[CMAJORS] = { CMAJOR_0, };

/*
 *  OPEN
 *  -----------------------------------
 */
STATIC int
tua_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	psw_t flags;
	int mindex = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	minor_t bminor = cminor;
	struct tua *tua, **tpp = &master.tua.list;
	MOD_INC_USE_COUNT;	/* keep module from unloading */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", DRV_NAME));
		MOD_DEC_USE_COUNT;
		return (EIO);
	}
	if (cmajor != CMAJOR_0 || cminor >= TUA_CMINOR_FREE) {
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	/* allocate a new device */
	cminor = TUA_CMINOR_FREE;
	spin_lock_irqsave(&master.lock, flags);
	for (; *tpp; tpp = &(*tpp)->next) {
		major_t dmajor = (*tpp)->u.dev.cmajor;
		if (cmajor != dmajor)
			break;
		if (cmajor == dmajor) {
			minor_t dminor = (*tpp)->u.dev.cminor;
			if (cminor < dminor)
				break;
			if (cminor > dminor)
				continue;
			if (cminor == dminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= CMAJORS || !(cmajor = tua_majors[mindex]))
						break;
					cminor = 0;
				}
				continue;
			}
		}
	}
	if (mindex >= CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		spin_unlock_irqrestore(&master.lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(tua = tua_alloc_priv(q, tpp, devp, crp, bminor))) {
		ptrace(("%s: ERROR: no memory\n", DRV_NAME));
		spin_unlock_irqrestore(&master.lock, flags);
		MOD_DEC_USE_COUNT;
		return (ENOMEM);
	}
	spin_unlock_irqrestore(&master.lock, flags);
	return (0);
}

/*
 *  CLOSE
 *  -----------------------------------
 */
STATIC int
tua_close(queue_t *q, int flag, cred_t *crp)
{
	struct tua *tua = TUA_PRIV(q);
	psw_t flags;
	(void) flag;
	(void) crp;
	(void) tua;
	printd(("%s: closing character device %d:%d\n", DRV_NAME, tua->u.dev.cmajor,
		tua->u.dev.cminor));
	spin_lock_irqsave(&master.lock, flags);
	tua_free_priv(tua);
	spin_unlock_irqrestore(&master.lock, flags);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  =========================================================================
 *
 *  Allocation and Deallocation
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  Registration and initialization
 *
 *  =========================================================================
 */
#ifdef LINUX
/*
 *  Linux Registration
 *  -------------------------------------------------------------------------
 */

unsigned short modid = DRV_ID;
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the TUA driver. (0 for allocation.)");

major_t major = CMAJOR_0;
#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Major device number for the TUA driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct cdevsw tua_cdev = {
	.d_name = DRV_NAME,
	.d_str = &tuainfo,
	.d_flag = 0,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC int
tua_register_strdev(major_t major)
{
	int err;
	if ((err = register_strdev(&tua_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC int
tua_unregister_strdev(major_t major)
{
	int err;
	if ((err = unregister_strdev(&tua_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

STATIC int
tua_register_strdev(major_t major)
{
	int err;
	if ((err = lis_register_strdev(major, &tuainfo, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC int
tua_unregister_strdev(major_t major)
{
	int err;
	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC void __exit
tuaterminate(void)
{
	int err, mindex;
	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (tua_majors[mindex]) {
			if ((err = tua_unregister_strdev(tua_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					tua_majors[mindex]);
			if (mindex)
				tua_majors[mindex] = 0;
		}
	}
	if ((err = tua_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
tuainit(void)
{
	int err, mindex = 0;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = tua_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		tuaterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = tua_register_strdev(tua_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					tua_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				tuaterminate();
				return (err);
			}
		}
		if (tua_majors[mindex] == 0)
			tua_majors[mindex] = err;
#ifdef LIS
		LIS_DEVFLAGS(tua_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = tua_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(tuainit);
module_exit(tuaterminate);

#endif				/* LINUX */
