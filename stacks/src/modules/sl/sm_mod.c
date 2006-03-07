/*****************************************************************************

 @(#) $RCSfile: sm_mod.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/03/07 01:12:04 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2006/03/07 01:12:04 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sm_mod.c,v $
 Revision 0.9.2.13  2006/03/07 01:12:04  brian
 - updated headers

 *****************************************************************************/

#ident "@(#) $RCSfile: sm_mod.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/03/07 01:12:04 $"

static char const ident[] =
    "$RCSfile: sm_mod.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/03/07 01:12:04 $";

#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/mtpi.h>

#define SM_MOD_DESCRIP		"SIMPLE SINGLE LINK MTP."
#define SM_MOD_REVISION		"LfS $RCSfile: sm_mod.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/03/07 01:12:04 $"
#define SM_MOD_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define SM_MOD_DEVICE		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SM_MOD_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define SM_MOD_LICENSE		"GPL"
#define SM_MOD_BANNER		SM_MOD_DESCRIP		"\n" \
				SM_MOD_REVISION		"\n" \
				SM_MOD_COPYRIGHT	"\n" \
				SM_MOD_DEVICE		"\n" \
				SM_MOD_CONTACT		"\n"
#define SM_MOD_SPLASH		SM_MOD_DESCRIP		"\n" \
				SM_MOD_REVISION		"\n"

#ifdef LINUX
MODULE_AUTHOR(SM_MOD_CONTACT);
MODULE_DESCRIPTION(SM_MOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(SM_MOD_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SM_MOD_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sm_mod");
#endif
#endif				/* LINUX */

#ifdef LFS
#define SM_MOD_MOD_ID		CONFIG_STREAMS_SM_MOD_MODID
#define SM_MOD_MOD_NAME		CONFIG_STREAMS_SM_MOD_NAME
#endif

/*
 *  =======================================================================
 *
 *  STREAMS Definitions
 *
 *  =======================================================================
 */

#define MOD_ID		SM_MOD_MOD_ID
#define MOD_NAME	SM_MOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SM_MOD_BANNER
#else				/* MODULE */
#define MOD_BANNER	SM_MOD_SPLASH
#endif				/* MODULE */

STATIC struct module_info sm_minfo = {
	MOD_ID,				/* id */
	MOD_NAME,			/* name */
	0,				/* min packet size accepted */
	INFPSZ,				/* max packet size accepted */
	10240L,				/* high water mark */
	512L				/* low water mark */
};

STATIC int streamscall sm_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int streamscall sm_close(queue_t *, int, cred_t *);

STATIC int streamscall sm_wput(queue_t *, mblk_t *);
STATIC int streamscall sm_rput(queue_t *, mblk_t *);

STATIC struct qinit sm_rinit = {
	sm_rput,			/* put */
	NULL,				/* service */
	sm_open,			/* open */
	sm_close,			/* close */
	NULL,				/* admin */
	&sm_minfo,			/* info */
	NULL				/* stat */
};

STATIC struct qinit sm_winit = {
	sm_wput,			/* put */
	NULL,				/* service */
	NULL,				/* open */
	NULL,				/* close */
	NULL,				/* admin */
	&sm_minfo,			/* info */
	NULL				/* stat */
};

MODULE_STATIC struct streamtab sm_modinfo = {
	&sm_rinit,			/* read queue */
	&sm_winit,			/* write queue */
	NULL,				/* mux read queue */
	NULL				/* mux write queue */
};

struct priv {
	uint32_t opc;
	uint32_t dpc;
	uint32_t sio;
};

STATIC int
sm_init_caches(void)
{
	return (0);
}

STATIC int
sm_term_caches(void)
{
	return (0);
}

STATIC streamscall int
sm_rput(queue_t *q, mblk_t *dp)
{
	/* 
	   message from below, strip MTP header and add MTP-TRANSFER primitive */
	switch (dp->b_datap->db_type) {
	case M_DATA:
	{
		mblk_t *mp;
		struct MTP_transfer_ind *m;
		struct mtp_addr *a;
		uint sio, dpc, opc, sls, b;
		sio = *dp->b_rptr++;
		if ((sio & 0xf) != 5) {
			__ptrace(("Discarding bad message sio = %#02x\n", sio));
			freemsg(dp);
			return (0);
		}
		b = *dp->b_rptr++;
		dpc = ((b << 0) & 0xff);
		b = *dp->b_rptr++ & 0x00ff;
		dpc |= ((b << 8) & 0x3f00);
		opc = ((b >> 6) & 0x03);
		b = *dp->b_rptr++ & 0x00ff;
		opc |= ((b << 2) & 0x3fc);
		b = *dp->b_rptr++ & 0x00ff;
		opc |= ((b << 10) & 0x3c00);
		sls = (b >> 4) & 0x0f;
		mp = allocb(sizeof(*m) + sizeof(*a), BPRI_MED);
		mp->b_datap->db_type = M_PROTO;
		m = (typeof(m)) mp->b_wptr;
		mp->b_wptr += sizeof(*m);
		m->mtp_primitive = MTP_TRANSFER_IND;
		m->mtp_srce_length = sizeof(*a);
		m->mtp_srce_offset = sizeof(*m);
		m->mtp_mp = 0;
		m->mtp_sls = sls;
		a = (typeof(a)) mp->b_wptr;
		mp->b_wptr += sizeof(*a);
		a->ni = 0;
		a->pc = opc;
		a->si = sio & 0x0f;
		mp->b_cont = dp;
		putnext(q, mp);
		return (0);
	}
	}
	/* 
	   pass along unrecognized */
	putnext(q, dp);
	return (0);
}

STATIC streamscall int
sm_wput(queue_t *q, mblk_t *mp)
{
	/* 
	   message from above, strip MTP-TRANSFER primitive and add header */
	switch (mp->b_datap->db_type) {
	case M_PROTO:
	case M_PCPROTO:
	{
		mblk_t *dp;
		struct MTP_transfer_req *m = (typeof(m)) mp->b_rptr;
		struct priv *p = q->q_ptr;
		if (m->mtp_primitive != MTP_TRANSFER_REQ)
			break;
		dp = allocb(11, BPRI_MED);
		dp->b_wptr += 6;
		*dp->b_wptr++ = p->sio;
		*dp->b_wptr++ = p->dpc;
		*dp->b_wptr++ = ((p->dpc >> 8) & 0x3f) | ((p->opc & 0x3) << 6);
		*dp->b_wptr++ = ((p->opc >> 2) & 0x0ff);
		*dp->b_wptr++ = ((p->opc >> 10) & 0x0f) | ((m->mtp_sls & 0x0f) << 4);
		dp->b_cont = mp->b_cont;
		freeb(mp);
		pullupmsg(dp, -1);
		dp->b_rptr += 6;
		putnext(q, dp);
		return (0);
	}
	}
	/* 
	   pass along unrecognized */
	putnext(q, mp);
	return (0);
}

STATIC streamscall int
sm_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	struct priv *p;
	if (q->q_ptr)
		return (0);
	if (!(p = kmalloc(sizeof(*p), GFP_ATOMIC)))
		return (ENOMEM);
	bzero(p, sizeof(*p));
	p->opc = 0x11ae;	/* our point code */
	p->dpc = 0x11d8;	/* their point code */
	p->sio = 0x05;		/* isup */
	RD(q)->q_ptr = p;
	WR(q)->q_ptr = p;
	return (0);
}

STATIC streamscall int
sm_close(queue_t *q, int sflag, cred_t *crp)
{
	struct priv *p = q->q_ptr;
	RD(q)->q_ptr = NULL;
	WR(q)->q_ptr = NULL;
	kfree(p);
	return (0);
}

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

unsigned short modid = MOD_ID;
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the SM-MOD module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw sm_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sm_modinfo,
	.f_flag = 0,
	.f_kmod = THIS_MODULE,
};

STATIC int
sm_register_strmod(void)
{
	int err;
	if ((err = register_strmod(&sm_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
sm_unregister_strmod(void)
{
	int err;
	if ((err = unregister_strmod(&sm_fmod)) < 0)
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
sm_register_strmod(void)
{
	int err;
	if ((err = lis_register_strmod(&sm_modinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	return (0);
}

STATIC int
sm_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&sm_modinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
sm_modinit(void)
{
	int err;
	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = sm_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = sm_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		sm_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
sm_modterminate(void)
{
	int err;
	if ((err = sm_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = sm_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(sm_modinit);
module_exit(sm_modterminate);

#endif				/* LINUX */
