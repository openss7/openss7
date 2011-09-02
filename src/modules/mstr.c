/*****************************************************************************

 @(#) $RCSfile: mstr.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:53 $

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

 Last Modified $Date: 2011-09-02 08:46:53 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mstr.c,v $
 Revision 1.1.2.2  2011-09-02 08:46:53  brian
 - sync up lots of repo and build changes from git

 Revision 1.1.2.1  2010-12-19 00:26:46  brian
 - added multi-stream conversion module

 *****************************************************************************/

static char const ident[] = "$RCSfile: mstr.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:53 $";

/*
 *  This is a single-stream to multi-stream conversion module.  What it does is accept a
 *  multi-stream M_PROTO/M_PCPROTO interface and converts it to a M_CTL/M_PCCTL interface.  It does
 *  this simply by converting M_PROTO messages to M_CTL and M_PCPROTO messages to M_PCCTL and visa
 *  versa.  The module has no service procedure on either side, but merely converts messages on the
 *  fly.  This is a very simplistic module.  M_PROTO/M_PCPROTO on the write queue are converted to
 *  M_CTL/M_PCCTL and M_CTL/M_PCCTL on the read queue are converted to M_PROTO/M_PCPROTO.  This
 *  allows user-land interface and testing of multi-stream modules that use the inter-module
 *  M_CTL/M_PCCTL interface.
 */

#include <sys/os7/compat.h>

#define MSTR_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define MSTR_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define MSTR_REVISION	"OpenSS7 $RCSfile: mstr.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:53 $"
#define MSTR_DEVICE	"SVR 4.2 MP STREAMS Multi-Stream Module"
#define MSTR_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define MSTR_LICENSE	"GPL"
#define MSTR_BANNER	MSTR_DESCRIP	"\n" \
			MSTR_COPYRIGHT	"\n" \
			MSTR_REVISION	"\n" \
			MSTR_DEVICE	"\n" \
			MSTR_CONTACT
#define MSTR_SPLASH	MSTR_DEVICE	" - " \
			MSTR_REVISION

#ifdef CONFIG_STREAMS_MSTR_MODULE
MODULE_AUTHOR(MSTR_CONTACT);
MODULE_DESCRIPTION(MSTR_DESCRIP);
MODULE_SUPPORTED_DEVICE(MSTR_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MSTR_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* CONFIG_STREAMS_MSTR_MODULE */

#ifndef CONFIG_STREAMS_MSTR_NAME
#error "CONFIG_STREAMS_MSTR_NAME must be defined."
#endif				/* CONFIG_STREAMS_MSTR_NAME */
#ifndef CONFIG_STREAMS_MSTR_MODID
#error "CONFIG_STREAMS_MSTR_MODID must be defined."
#endif				/* CONFIG_STREAMS_MSTR_MODID */

#ifdef MODULE
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-mstr");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */

/*
 *  STREAMS DEFINITIONS
 */

#define MOD_ID		CONFIG_STREAMS_MSTR_MODID
#define MOD_NAME	CONFIG_STREAMS_MSTR_NAME
#ifdef MODULE
#define MOD_BANNER	MSTR_BANNER
#else				/* MODULE */
#define MOD_BANNER	MSTR_SPLASH
#endif				/* MODULE */

static struct module_info mstr_minfo = {
	.mi_idnum = MOD_ID,	/* Module ID number */
	.mi_idname = MOD_NAME,	/* Module name */
	.mi_minpsz = 0,		/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,	/* Max packet size accepted */
	.mi_hiwat = 0,		/* Hi water mark */
	.mi_lowat = 0,		/* Lo water mark */
};

static struct module_stat mstr_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat mstr_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static streamscall int mstr_qopen(queue_t *, dev_t *, int, int, cred_t *);
static streamscall int mstr_qclose(queue_t *, int, cred_t *);

static streamscall int mstr_rput(queue_t *, mblk_t *);
static streamscall int mstr_wput(queue_t *, mblk_t *);

static struct qinit mstr_rinit = {
	.qi_putp = mstr_rput,
	.qi_qopen = mstr_qopen,
	.qi_qclose = mstr_qclose,
	.qi_minfo = &mstr_minfo,
	.qi_mstat = &mstr_rstat,
};

static struct qinit mstr_winit = {
	.qi_putp = mstr_wput,
	.qi_minfo = &mstr_minfo,
	.qi_mstat = &mstr_wstat,
};

MODULE_STATIC struct streamtab mstrinfo = {
	.st_rdinit = &mstr_rinit,
	.st_wrinit = &mstr_winit,
};

/*
 *  PUT PROCEDURES
 */

STATIC streamscall int
mstr_wput(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(mp->b_datap->db_type, M_PROTO)) {
	case M_PROTO:
		mp->b_datap->db_type = M_CTL;
		break;
	case M_PCPROTO:
		mp->b_datap->db_type = M_PCCTL;
		break;
	default:
		break;
	}
	putnext(q, mp);
	return (0);
}

STATIC streamscall int
mstr_rput(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(mp->b_datap->db_type, M_CTL)) {
	case M_CTL:
		mp->b_datap->db_type = M_PROTO;
		break;
	case M_PCCTL:
		mp->b_datap->db_type = M_PCPROTO;
		break;
	default:
		break;
	}
	putnext(q, mp);
	return (0);
}

/*
 *  OPEN AND CLOSE
 */

STATIC streamscall int
mstr_qopen(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag != MODOPEN || WR(q)->q_next == NULL)
		return (ENXIO);
	/* dummy private structure */
	q->q_ptr = WR(q)->q_ptr = (void *) 1;
	return (0);
}

STATIC streamscall int
mstr_qclose(queue_t *q, int oflag, cred_t *crp)
{
	qprocsoff(q);
	q->q_ptr = WR(q)->q_ptr = NULL;
	return (0);
}

/*
 *  REGISTRATION AND INITIALIZATION
 */

#ifdef LINUX
/*
 *  Linux Registration
 */

unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, short, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for MSTR module. (0 for allocation.)");

static struct fmodsw mstr_fmod = {
	.f_name = MOD_NAME,
	.f_str = &mstrinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

MODULE_STATIC int __init
mstrinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&mstr_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
mstrexit(void)
{
	int err;

	if ((err = unregister_strmod(&mstr_fmod)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
	return;
}

module_init(mstrinit);
module_exit(mstrexit);

#endif				/* LINUX */
