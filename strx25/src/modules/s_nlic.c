/*****************************************************************************

 @(#) $RCSfile: s_nlic.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 12:16:24 $

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

 $Log: s_nlic.c,v $
 Revision 0.9.2.1  2008-07-01 12:16:24  brian
 - added manual pages, specs and conversion modules

 *****************************************************************************/

#ident "@(#) $RCSfile: s_nlic.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 12:16:24 $"

static char const ident[] = "$RCSfile: s_nlic.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 12:16:24 $";

/*
 * This is s_nlic, a pushable STREAMS module that interprets the SpiderX25
 * input-output controls and converts them to NPI input-output controls.
 * Primitives are not converted and are simply passed through.  The purpose of
 * this module is to permit SpiderX25 compatible management programs to be
 * invoked on NPI Streams.
 */

#define _MPS_SOURCE

#include <sys/os7/compat.h>
#include <sys/npi.h>
#include <sys/npi_ioctl.h>
#include <sys/snet/x25_proto.h>
#include <sys/snet/x25_control.h>

#define S_NLIC_DESCRIP		"SPIDER X25 ADMIN COMPATIBILITY MODULE FOR LINUX FAST-STREAMS"
#define S_NLIC_EXTRA		"Part of the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define S_NLIC_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define S_NLIC_REVISION		"OpenSS7 $RCSfile: s_nlic.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-07-01 12:16:24 $"
#define S_NLIC_DEVICE		"SVR 4.2MP CDI CD_HDLC Devices"
#define S_NLIC_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define S_NLIC_LICENSE		"GPL"
#define S_NLIC_BANNER		S_NLIC_DESCRIP		"\n" \
				S_NLIC_EXTRA		"\n" \
				S_NLIC_COPYRIGHT	"\n" \
				S_NLIC_REVISION		"\n" \
				S_NLIC_DEVICE		"\n" \
				S_NLIC_CONTACT		"\n"
#define S_NLIC_SPLASH		S_NLIC_DESCRIP		" - " \
				S_NLIC_REVISION

#ifndef CONFIG_STREAMS_S_NLIC_NAME
#error "CONFIG_STREAMS_S_NLIC_NAME must be defined."
#endif				/* CONFIG_STREAMS_S_NLIC_NAME */
#ifndef CONFIG_STREAMS_S_NLIC_MODID
#error "CONFIG_STREAMS_S_NLIC_MODID must be defined."
#endif				/* CONFIG_STREAMS_S_NLIC_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(S_NLIC_CONTACT);
MODULE_DESCRIPTION(S_NLIC_DESCRIP);
MODULE_SUPPORTED_DEVICE(S_NLIC_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(S_NLIC_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-s_nlic");
MODULE_ALIAS("streams-module-s_nlic");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_S_NLIC_MODID));
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#endif				/* LINUX */

#ifndef S_NLIC_MOD_NAME
#define S_NLIC_MOD_NAME		CONFIG_STREAMS_S_NLIC_NAME
#endif				/* S_NLIC_MOD_NAME */
#ifndef S_NLIC_MOD_ID
#define S_NLIC_MOD_ID		CONFIG_STREAMS_S_NLIC_MODID
#endif				/* S_NLIC_MOD_ID */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define MOD_ID		S_NLIC_MOD_ID
#define MOD_NAME	S_NLIC_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	S_NLIC_BANNER
#else				/* MODULE */
#define MOD_BANNER	S_NLIC_SPLASH
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
 * INPUT-OUTPUT CONTROL HANDLING
 *
 * --------------------------------------------------------------------------
 */
static noinline int
nl_ioctl_snident(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocdata_snident(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocack_snident(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_snmode(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocdata_snmode(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocack_snmode(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_snconfig(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocdata_snconfig(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocack_snconfig(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_snread(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocdata_snread(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocack_snread(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_getstats(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocdata_getstats(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocack_getstats(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_zerostats(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocdata_zerostats(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocack_zerostats(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_putpvcmap(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copyin(q, mp, NULL, sizeof(struct pvcconff));
	return (0);
}
static noinline int
nl_iocdata_putpvcmap(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mblk_t *db;

	switch (mi_copy_state(q, mp, &db)) {
	case -1:
		return (0);
	case MI_COPY_CASE(MI_COPYIN, 1):
		/* db contains the copied in pvcconff structure */
		/* fill out an N_IOCSCONFIG input-output control for an N_PVCMAP type object and
		   send it to the driver. */
		nl->ib = mp;
		return (0);
	}
	mi_copy_done(q, mp, EFAULT);
	return (0);
}
static noinline int
nl_iocack_putpvcmap(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	mblk_t *ib;
	if ((ib = nl->ib) != NULL) {
		nl->ib = NULL;
		freemsg(msg);
		mi_copy_done(q, mp, 0);
		return (0);
	}
	putnext(q, mp);
	return (0);
}
static noinline int
nl_ioctl_getpvcmap(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocdata_getpvcmap(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocack_getpvcmap(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_getvcstatus(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocdata_getvcstatus(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocack_getvcstatus(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_getnliversion(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mblk_t *db;
	struct nliformat *nf;

	if ((db = mi_copyout_alloc(q, mp, 0, sizeof(*nf)))) {
		nf = (typeof(nf)) db->b_rptr;
		/* the version returned is always version 3 */
		nf->version = 3;
		mi_copyout(q, mp);
		return (0);
	}
	return (0);
}
static noinline int
nl_iocdata_getnliversion(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mblk_t *db;

	switch (mi_copy_state(q, mp, &db)) {
	case -1:
		return (0);
	case MI_COPY_STATE(MI_COPY_OUT, 1):
		mi_copy_done(q, mp, 0);
		return (0);
	}
	mi_copy_done(q, mp, EFAULT);
	break;
}
static noinline int
nl_iocack_getnliversion(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_traceon(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline int
nl_iocdata_traceon(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline int
nl_iocack_traceon(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_traceoff(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline int
nl_iocdata_traceoff(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline int
nl_iocack_traceoff(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_nuimsg(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline int
nl_iocdata_nuimsg(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline int
nl_iocack_nuimsg(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_nuiput(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline int
nl_iocdata_nuiput(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline int
nl_iocack_nuiput(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_nuidel(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline int
nl_iocdata_nuidel(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline int
nl_iocack_nuidel(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_nuiget(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline int
nl_iocdata_nuiget(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline int
nl_iocack_nuiget(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_nuimget(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline int
nl_iocdata_nuimget(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline int
nl_iocack_nuimget(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_nuireset(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline int
nl_iocdata_nuireset(struct nl *nl, queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}
static noinline int
nl_iocack_nuireset(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_zerovcstats(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocdata_zerovcstats(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocack_zerovcstats(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_putx32map(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocdata_putx32map(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocack_putx32map(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_getx32map(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocdata_getx32map(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocack_getx32map(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_getsnidstats(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocdata_getsnidstats(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocack_getsnidstats(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_zerosnidstats(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocdata_zerosnidstats(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocack_zerosnidstats(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_setqosdatpri(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocdata_setqosdatpri(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocack_setqosdatpri(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}
static noinline int
nl_ioctl_resetqosdatpri(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocdata_resetqosdatpri(struct nl *nl, queue_t *q, mblk_t *mp)
{
}
static noinline int
nl_iocack_resetqosdatpri(struct nl *nl, queue_t *q, mblk_t *msg, mblk_t *mp)
{
	nl->ib = NULL;
	freemsg(msg);
	mi_copy_done(q, mp, 0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * --------------------------------------------------------------------------
 */
/**
 * nl_passflow - pass message along with flow control
 * @q: active queue
 * @mp: the message
 *
 * For the most part, we pass messages along with flow control.  The only
 * messages that we are interested in are M_FLUSH messages and messages from the
 * input-output control group: M_IOCTL, M_IOCDATA, M_IOCACK, M_IOCNAK, M_COPYIN
 * and M_COPYOUT.
 */
static fastcall inline int
nl_passflow(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static fastcall noinline __unlikely int
nl_w_ioctl(struct nl *nl, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case _IOC_TYPE(N_snident):
		switch (_IOC_NR(ioc->ioc_cmd)) {
		case _IOC_NR(N_snident):
			return nl_ioctl_snident(nl, q, mp);
		case _IOC_NR(N_snmode):
			return nl_ioctl_snmode(nl, q, mp);
		case _IOC_NR(N_snconfig):
			return nl_ioctl_snconfig(nl, q, mp);
		case _IOC_NR(N_snread):
			return nl_ioctl_snread(nl, q, mp);
		case _IOC_NR(N_getstats):
			return nl_ioctl_getstats(nl, q, mp);
		case _IOC_NR(N_zerostats):
			return nl_ioctl_zerostats(nl, q, mp);
		case _IOC_NR(N_putpvcmap):
			return nl_ioctl_putpvcmap(nl, q, mp);
		case _IOC_NR(N_getpvcmap):
			return nl_ioctl_getpvcmap(nl, q, mp);
		case _IOC_NR(N_getVCstatus):
			return nl_ioctl_getvcstatus(nl, q, mp);
		case _IOC_NR(N_getnliversion):
			return nl_ioctl_getnliversion(nl, q, mp);
		case _IOC_NR(N_traceon):
			return nl_ioctl_traceon(nl, q, mp);
		case _IOC_NR(N_traceoff):
			return nl_ioctl_traceoff(nl, q, mp);
		case _IOC_NR(N_nuimsg):
			return nl_ioctl_nuimsg(nl, q, mp);
		case _IOC_NR(N_nuiput):
			return nl_ioctl_nuiput(nl, q, mp);
		case _IOC_NR(N_nuidel):
			return nl_ioctl_nuidel(nl, q, mp);
		case _IOC_NR(N_nuiget):
			return nl_ioctl_nuiget(nl, q, mp);
		case _IOC_NR(N_nuimget):
			return nl_ioctl_nuimget(nl, q, mp);
		case _IOC_NR(N_nuireset):
			return nl_ioctl_nuireset(nl, q, mp);
		case _IOC_NR(N_zeroVCstats):
			return nl_ioctl_zerovcstats(nl, q, mp);
		case _IOC_NR(N_putx32map):
			return nl_ioctl_putx32map(nl, q, mp);
		case _IOC_NR(N_getx32map):
			return nl_ioctl_getx32map(nl, q, mp);
		case _IOC_NR(N_getSNIDstats):
			return nl_ioctl_getsnidstats(nl, q, mp);
		case _IOC_NR(N_zeroSNIDstats):
			return nl_ioctl_zerosnidstats(nl, q, mp);
		case _IOC_NR(N_setQOSDATPRI):
			return nl_ioctl_setqosdatpri(nl, q, mp);
		case _IOC_NR(N_resetQOSDATPRI):
			return nl_ioctl_resetqosdatpri(nl, q, mp);
		}
	default:
		break;
	}
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
nl_w_iocdata(struct nl *nl, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	switch (_IOC_TYPE(cp->cp_cmd)) {
	case _IOC_TYPE(N_snident):
		switch (_IOC_NR(cp->cp_cmd)) {
		case _IOC_NR(N_snident):
			return nl_iocdata_snident(nl, q, mp);
		case _IOC_NR(N_snmode):
			return nl_iocdata_snmode(nl, q, mp);
		case _IOC_NR(N_snconfig):
			return nl_iocdata_snconfig(nl, q, mp);
		case _IOC_NR(N_snread):
			return nl_iocdata_snread(nl, q, mp);
		case _IOC_NR(N_getstats):
			return nl_iocdata_getstats(nl, q, mp);
		case _IOC_NR(N_zerostats):
			return nl_iocdata_zerostats(nl, q, mp);
		case _IOC_NR(N_putpvcmap):
			return nl_iocdata_putpvcmap(nl, q, mp);
		case _IOC_NR(N_getpvcmap):
			return nl_iocdata_getpvcmap(nl, q, mp);
		case _IOC_NR(N_getVCstatus):
			return nl_iocdata_getvcstatus(nl, q, mp);
		case _IOC_NR(N_getnliversion):
			return nl_iocdata_getnliversion(nl, q, mp);
		case _IOC_NR(N_traceon):
			return nl_iocdata_traceon(nl, q, mp);
		case _IOC_NR(N_traceoff):
			return nl_iocdata_traceoff(nl, q, mp);
		case _IOC_NR(N_nuimsg):
			return nl_iocdata_nuimsg(nl, q, mp);
		case _IOC_NR(N_nuiput):
			return nl_iocdata_nuiput(nl, q, mp);
		case _IOC_NR(N_nuidel):
			return nl_iocdata_nuidel(nl, q, mp);
		case _IOC_NR(N_nuiget):
			return nl_iocdata_nuiget(nl, q, mp);
		case _IOC_NR(N_nuimget):
			return nl_iocdata_nuimget(nl, q, mp);
		case _IOC_NR(N_nuireset):
			return nl_iocdata_nuireset(nl, q, mp);
		case _IOC_NR(N_zeroVCstats):
			return nl_iocdata_zerovcstats(nl, q, mp);
		case _IOC_NR(N_putx32map):
			return nl_iocdata_putx32map(nl, q, mp);
		case _IOC_NR(N_getx32map):
			return nl_iocdata_getx32map(nl, q, mp);
		case _IOC_NR(N_getSNIDstats):
			return nl_iocdata_getsnidstats(nl, q, mp);
		case _IOC_NR(N_zeroSNIDstats):
			return nl_iocdata_zerosnidstats(nl, q, mp);
		case _IOC_NR(N_setQOSDATPRI):
			return nl_iocdata_setqosdatpri(nl, q, mp);
		case _IOC_NR(N_resetQOSDATPRI):
			return nl_iocdata_resetqosdatpri(nl, q, mp);
		}
	default:
		break;
	}
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
nl_r_iocack(struct nl *nl, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc;
	mblk_t *ib = nl->ib;

	if (ib == NULL)
		return nl_passflow(q, mp);

	ioc = (typeof(ioc)) ib->b_rptr;

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case _IOC_TYPE(N_snident):
		switch (_IOC_NR(ioc->ioc_cmd)) {
		case _IOC_NR(N_snident):
			return nl_iocack_snident(nl, q, mp, ib);
		case _IOC_NR(N_snmode):
			return nl_iocack_snmode(nl, q, mp, ib);
		case _IOC_NR(N_snconfig):
			return nl_iocack_snconfig(nl, q, mp, ib);
		case _IOC_NR(N_snread):
			return nl_iocack_snread(nl, q, mp, imp, ib);
		case _IOC_NR(N_getstats):
			return nl_iocack_getstats(nl, q, mp, ib);
		case _IOC_NR(N_zerostats):
			return nl_iocack_zerostats(nl, q, mp, ib);
		case _IOC_NR(N_putpvcmap):
			return nl_iocack_putpvcmap(nl, q, mp, ib);
		case _IOC_NR(N_getpvcmap):
			return nl_iocack_getpvcmap(nl, q, mp, ib);
		case _IOC_NR(N_getVCstatus):
			return nl_iocack_getvcstatus(nl, q, mp, ib);
		case _IOC_NR(N_getnliversion):
			return nl_iocack_getnliversion(nl, q, mp, ib);
		case _IOC_NR(N_traceon):
			return nl_iocack_traceon(nl, q, mp, ib);
		case _IOC_NR(N_traceoff):
			return nl_iocack_traceoff(nl, q, mp, ib);
		case _IOC_NR(N_nuimsg):
			return nl_iocack_nuimsg(nl, q, mp, ib);
		case _IOC_NR(N_nuiput):
			return nl_iocack_nuiput(nl, q, mp, ib);
		case _IOC_NR(N_nuidel):
			return nl_iocack_nuidel(nl, q, mp, ib);
		case _IOC_NR(N_nuiget):
			return nl_iocack_nuiget(nl, q, mp, ib);
		case _IOC_NR(N_nuimget):
			return nl_iocack_nuimget(nl, q, mp, ib);
		case _IOC_NR(N_nuireset):
			return nl_iocack_nuireset(nl, q, mp, ib);
		case _IOC_NR(N_zeroVCstats):
			return nl_iocack_zerovcstats(nl, q, mp, ib);
		case _IOC_NR(N_putx32map):
			return nl_iocack_putx32map(nl, q, mp, ib);
		case _IOC_NR(N_getx32map):
			return nl_iocack_getx32map(nl, q, mp, ib);
		case _IOC_NR(N_getSNIDstats):
			return nl_iocack_getsnidstats(nl, q, mp, ib);
		case _IOC_NR(N_zeroSNIDstats):
			return nl_iocack_zerosnidstats(nl, q, mp, ib);
		case _IOC_NR(N_setQOSDATPRI):
			return nl_iocack_setqosdatpri(nl, q, mp, ib);
		case _IOC_NR(N_resetQOSDATPRI):
			return nl_iocack_resetqosdatpri(nl, q, mp, ib);
		}
	default:
		break;
	}
	putnext(q, mp);
	return (0);
}
static fastcall noinline __unlikely int
nl_r_iocnak(struct nl *nl, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc, *ioc2;
	mblk_t *ib;

	if ((ib = nl->ib) != NULL) {
		ioc = (typeof(ioc)) ib->b_rptr;
		switch (_IOC_TYPE(ioc->ioc_cmd)) {
		case _IOC_TYPE(N_snident):
			switch (_IOC_NR(ioc->ioc_cmd)) {
			case _IOC_NR(N_snident):
			case _IOC_NR(N_snmode):
			case _IOC_NR(N_snconfig):
			case _IOC_NR(N_snread):
			case _IOC_NR(N_getstats):
			case _IOC_NR(N_zerostats):
			case _IOC_NR(N_putpvcmap):
			case _IOC_NR(N_getpvcmap):
			case _IOC_NR(N_getVCstatus):
			case _IOC_NR(N_getnliversion):
			case _IOC_NR(N_traceon):
			case _IOC_NR(N_traceoff):
			case _IOC_NR(N_nuimsg):
			case _IOC_NR(N_nuiput):
			case _IOC_NR(N_nuidel):
			case _IOC_NR(N_nuiget):
			case _IOC_NR(N_nuimget):
			case _IOC_NR(N_nuireset):
			case _IOC_NR(N_zeroVCstats):
			case _IOC_NR(N_putx32map):
			case _IOC_NR(N_getx32map):
			case _IOC_NR(N_getSNIDstats):
			case _IOC_NR(N_zeroSNIDstats):
			case _IOC_NR(N_setQOSDATPRI):
			case _IOC_NR(N_resetQOSDATPRI):
				ioc2 = (typeof(ioc2)) msg->b_rptr;
				nl->ib = NULL;
				freemsg(msg);
				mi_copy_done(q, ib, ioc2->ioc_error);
				return (0);
			}
		default:
			break;
		}
	}
	putnext(q, mp);
	return (0);
}

/**
 * nl_w_flush - cannonical flushing of the module write queue
 * @q: write queue
 * @mp: M_FLUSH message
 */
static fastcall noinline __unlikely int
nl_w_flush(queue_t *q, mblk_t *mp)
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
 * nl_w_flush - cannonical flushing of the module read queue
 * @q: read queue
 * @mp: M_FLUSH message
 */
static fastcall noinline __unlikely int
nl_r_flush(queue_t *q, mblk_t *mp)
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

static fastcall inline int
nl_w_msg_put(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		goto done;	/* fast path for data */
	if (likely(DB_TYPE(mp) == M_PROTO))
		goto done;	/* fast path for data */
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
		break;
	case M_IOCTL:
	case M_IOCDATA:
		return (-EAGAIN);
	case M_FLUSH:
		return nl_w_flush(q, mp);
	default:
		break;
	}
      done:
	return nl_passflow(q, mp);
}
static fastcall noinline int
nl_w_msg_srv(queue_t *q, mblk_t *mp)
{
	struct nl *nl = NL_W_PRIV(q);

	if (likely(DB_TYPE(mp) == M_DATA))
		goto done;	/* fast path for data */
	if (likely(DB_TYPE(mp) == M_PROTO))
		goto done;	/* fast path for data */
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
		break;
	case M_IOCTL:
		return nl_w_ioctl(nl, q, mp);
	case M_IOCDATA:
		return nl_w_iocdata(nl, q, mp);
	case M_FLUSH:
		return nl_w_flush(q, mp);
	default:
		break;
	}
      done:
	return nl_passflow(q, mp);
}
static fastcall noinline int
nl_r_msg_srv(queue_t *q, mblk_t *mp)
{
	struct nl *nl = NL_R_PRIV(q);

	if (likely(DB_TYPE(mp) == M_PROTO))
		goto done;	/* fast path for data */
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
	case M_COPYIN:
	case M_COPYOUT:
		break;
	case M_FLUSH:
		return nl_r_flush(q, mp);
	case M_IOCACK:
		return nl_r_iocack(q, mp);
	case M_IOCNAK:
		return nl_r_iocnak(q, mp);
	default:
		break;
	}
      done:
	return nl_passflow(q, mp);
}
static fastcall inline int
nl_r_msg_put(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_PROTO))
		goto done;	/* fast path for data */
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
		break;
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		return (-EAGAIN);	/* handle from service procedure */
	case M_FLUSH:
		return nl_r_flush(q, mp);
	default:
		break;
	}
      done:
	return nl_passflow(q, mp);
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

struct streamtab s_nlic_info = {
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
MODULE_PARM_DESC(modid, "Module ID for S_NLIC.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

struct fmodsw s_nlic_fmod = {
	.f_name = MOD_NAME,
	.f_str = &s_nlic_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
s_nlic_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&s_nlic_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d", MOD_NAME, (int) modid);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static __exit void
s_nlic_modexit(void)
{
	int err;

	if ((err = unregister_strmod(&s_nlic_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(s_nlic_modinit);
module_exit(s_nlic_modexit);

#endif				/* LINUX */
