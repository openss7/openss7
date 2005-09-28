/*****************************************************************************

 @(#) $RCSfile: testmod.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/09/27 23:34:25 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/09/27 23:34:25 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: testmod.c,v $
 Revision 0.9.2.2  2005/09/27 23:34:25  brian
 - added test cases, tweaked straccess()

 Revision 0.9.2.1  2005/09/25 22:52:11  brian
 - added test module and continuing with testing

 *****************************************************************************/

#ident "@(#) $RCSfile: testmod.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/09/27 23:34:25 $"

static char const ident[] = "$RCSfile: testmod.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/09/27 23:34:25 $";

/*
 * This is TESTMOD a STREAMS test module that provides some specialized input-output controls meant
 * for testing purposes only.  It is pushed onto another driver (such as the nuls or echo driver) to
 * perform some more specialized functions.
 */

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include <sys/testmod.h>

#include "sys/config.h"

#define TESTMOD_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define TESTMOD_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define TESTMOD_REVISION	"LfS $RCSfile: testmod.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2005/09/27 23:34:25 $"
#define TESTMOD_DEVICE		"SVR 4.2 Test Module for STREAMS"
#define TESTMOD_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define TESTMOD_LICENSE		"GPL"
#define TESTMOD_BANNER		TESTMOD_DESCRIP		"\n" \
				TESTMOD_COPYRIGHT	"\n" \
				TESTMOD_REVISION	"\n" \
				TESTMOD_DEVICE		"\n" \
				TESTMOD_CONTACT		"\n"
#define TESTMOD_SPLASH		TESTMOD_DEVICE		" - " \
				TESTMOD_REVISION	"\n"

#ifdef CONFIG_STREAMS_TESTMOD_MODULE
MODULE_AUTHOR(TESTMOD_CONTACT);
MODULE_DESCRIPTION(TESTMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(TESTMOD_DEVICE);
MODULE_LICENSE(TESTMOD_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-testmod");
#endif
#endif

#ifndef CONFIG_STREAMS_TESTMOD_NAME
//#define CONFIG_STREAMS_TESTMOD_NAME "testmod"
#error "CONFIG_STREAMS_TESTMOD_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_TESTMOD_MODID
//#define CONFIG_STREAMS_TESTMOD_MODID 13
#error "CONFIG_STREAMS_TESTMOD_MODID must be defined."
#endif

modID_t modid = CONFIG_STREAMS_TESTMOD_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for TESTMOD.");

#ifdef MODULE_ALIAS
#if LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_TESTMOD_MODID));
MODULE_ALIAS("streams-module-testmod");
#endif
#endif

static struct module_info testmod_minfo = {
	.mi_idnum = CONFIG_STREAMS_TESTMOD_MODID,
	.mi_idname = CONFIG_STREAMS_TESTMOD_NAME,
	.mi_minpsz = 0,
	.mi_maxpsz = INFPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUT routines
 *
 *  -------------------------------------------------------------------------
 */

static int
testmod_rput(queue_t *q, mblk_t *mp)
{
	/* we don't queue so we don't need to worry about M_FLUSH */
	putnext(q, mp);
	return (0);
}

static int
testmod_wput(queue_t *q, mblk_t *mp)
{
	/* we don't queue so we don't need to worry about M_FLUSH */
	switch (mp->b_datap->db_type) {
	case M_IOCTL:
	{
		struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
		int err = EINVAL;
		int rval = 0;

		switch (ioc->ioc_cmd) {
		case TM_IOC_HANGUP:
			/* It is convenient to by able to synthesize an M_HANGUP message upstream
			   so that the test program can test the response of a hung up stream to
			   various system calls and input-output commands.  This ioctl accomplishes 
			   that. */
			if (putnextctl(OTHERQ(q), M_HANGUP))
				goto ack;
			err = ENOSR;
			goto nak;
		case TM_IOC_RDERR:
		{
			int rwerr;

			rwerr = *(long *) mp->b_cont->b_rptr;
			printd(("%s: error number is %d\n", __FUNCTION__, rwerr));
			/* Synthesize a M_ERROR message with a read error (equal to the arg) */
			if (putnextctl2(OTHERQ(q), M_ERROR, rwerr, NOERROR))
				goto ack;
			err = ENOSR;
			goto nak;
		}
		case TM_IOC_WRERR:
		{
			int rwerr;

			rwerr = *(long *) mp->b_cont->b_rptr;
			printd(("%s: error number is %d\n", __FUNCTION__, rwerr));
			/* Synthesize a M_ERROR message with a write error (equal to the arg) */
			if (putnextctl2(OTHERQ(q), M_ERROR, NOERROR, rwerr))
				goto ack;
			err = ENOSR;
			goto nak;
		}
		case TM_IOC_RWERR:
		{
			int rwerr;

			rwerr = *(long *) mp->b_cont->b_rptr;
			printd(("%s: error number is %d\n", __FUNCTION__, rwerr));
			/* Synthesize a M_ERROR message with an error (equal to the arg) */
			if (putnextctl1(OTHERQ(q), M_ERROR, rwerr))
				goto ack;
			err = ENOSR;
			goto nak;
		}
		default:
			/* pass along anything we don't understand */
			break;
		}
		break;
	      ack:
		mp->b_datap->db_type = M_IOCACK;
		ioc->ioc_count = 0;
		ioc->ioc_rval = rval;
		ioc->ioc_error = 0;
		qreply(q, mp);
		return (0);
	      nak:
		mp->b_datap->db_type = M_IOCNAK;
		ioc->ioc_count = 0;
		ioc->ioc_rval = -1;
		ioc->ioc_error = err;
		qreply(q, mp);
		return (0);
	}
	}
	putnext(q, mp);
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
static int
testmod_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	queue_t *wq;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	wq = WR(q);
	if (sflag == MODOPEN || wq->q_next != NULL) {
		/* inherit packet sizes of what we are pushed over */
		wq->q_minpsz = wq->q_next->q_minpsz;
		wq->q_maxpsz = wq->q_next->q_maxpsz;
		q->q_ptr = wq->q_ptr = q;	/* just set it to something */
		return (0);
	}
	return (EIO);		/* can't be opened as driver */
}
static int
testmod_close(queue_t *q, int oflag, cred_t *crp)
{
	(void) oflag;
	(void) crp;
	if (!q->q_ptr)
		return (ENXIO);
	q->q_ptr = WR(q)->q_ptr = NULL;
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and initialization
 *
 *  -------------------------------------------------------------------------
 */
static struct qinit testmod_rinit = {
	.qi_putp = testmod_rput,
	.qi_qopen = testmod_open,
	.qi_qclose = testmod_close,
	.qi_minfo = &testmod_minfo,
};

static struct qinit testmod_winit = {
	.qi_putp = testmod_wput,
	.qi_minfo = &testmod_minfo,
};

static struct streamtab testmod_info = {
	.st_rdinit = &testmod_rinit,
	.st_wrinit = &testmod_winit,
};

static struct fmodsw testmod_fmod = {
	.f_name = CONFIG_STREAMS_TESTMOD_NAME,
	.f_str = &testmod_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_TESTMOD_MODULE
static
#endif
int __init
testmod_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_TESTMOD_MODULE
	printk(KERN_INFO TESTMOD_BANNER);
#else
	printk(KERN_INFO TESTMOD_SPLASH);
#endif
	testmod_minfo.mi_idnum = modid;
	if ((err = register_strmod(&testmod_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
};

#ifdef CONFIG_STREAMS_TESTMOD_MODULE
static
#endif
void __exit
testmod_exit(void)
{
	int err;

	if ((err = unregister_strmod(&testmod_fmod)) < 0)
		return (void) (err);
	return (void) (0);
};

#ifdef CONFIG_STREAMS_TESTMOD_MODULE
module_init(testmod_init);
module_exit(testmod_exit);
#endif
