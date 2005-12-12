/*****************************************************************************

 @(#) $RCSfile: testmod.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2005/12/12 12:26:27 $

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

 Last Modified $Date: 2005/12/12 12:26:27 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: testmod.c,v $
 Revision 1.1.2.2  2005/12/12 12:26:27  brian
 - changes for warningless compile on gcc 4.0.2.

 Revision 1.1.2.1  2005/10/23 05:01:26  brian
 - test programs and modules for POSIX testing

 Revision 0.9.2.6  2005/10/14 12:26:49  brian
 - SC module and scls utility tested

 Revision 0.9.2.5  2005/10/13 10:58:44  brian
 - working up testing of sad(4) and sc(4)

 Revision 0.9.2.4  2005/10/07 09:34:24  brian
 - more testing and corrections

 Revision 0.9.2.3  2005/10/05 09:25:31  brian
 - poll tests, some noxious problem still with poll

 Revision 0.9.2.2  2005/09/27 23:34:25  brian
 - added test cases, tweaked straccess()

 Revision 0.9.2.1  2005/09/25 22:52:11  brian
 - added test module and continuing with testing

 *****************************************************************************/

#ident "@(#) $RCSfile: testmod.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2005/12/12 12:26:27 $"

static char const ident[] = "$RCSfile: testmod.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2005/12/12 12:26:27 $";

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
#include <sys/ddi.h>

#include <sys/testmod.h>

#define TESTMOD_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define TESTMOD_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define TESTMOD_REVISION	"LfS $RCSfile: testmod.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2005/12/12 12:26:27 $"
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

#if defined LIS && defined MODULE
#define CONFIG_STREAMS_TESTMOD_MODULE MODULE
#endif

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
#define CONFIG_STREAMS_TESTMOD_NAME TESTMOD__MOD_NAME
//#error "CONFIG_STREAMS_TESTMOD_NAME must be defined."
#endif
#ifndef CONFIG_STREAMS_TESTMOD_MODID
#define CONFIG_STREAMS_TESTMOD_MODID TESTMOD__ID
//#error "CONFIG_STREAMS_TESTMOD_MODID must be defined."
#endif

modID_t modid = CONFIG_STREAMS_TESTMOD_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for TESTMOD.");

#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_TESTMOD_MODID));
MODULE_ALIAS("streams-module-testmod");
#endif
#endif

#define STRMINPSZ   0
#define STRMAXPSZ   4096
#define STRHIGH	    5120
#define STRLOW	    1024

static struct module_info testmod_minfo = {
	.mi_idnum = CONFIG_STREAMS_TESTMOD_MODID,
	.mi_idname = CONFIG_STREAMS_TESTMOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
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

#ifdef LIS
union ioctypes {
	struct iocblk iocblk;
	struct copyreq copyreq;
	struct copyresp copyresp;
};

#define printd(__x) while (0) { }
#define ptrace(__x) while (0) { }

int
ctlmsg(unsigned char type)
{
	unsigned char mod = (type & ~QPCTL);

	return (((1 << mod) & ((1 << M_DATA) | (1 << M_PROTO) | (1 << (M_PCPROTO & ~QPCTL)))) == 0);
}

int
putnextctl2(queue_t *q, int type, int param1, int param2)
{
	mblk_t *mp;

	if (ctlmsg(type) && (mp = allocb(2, BPRI_HI))) {
		mp->b_datap->db_type = type;
		mp->b_wptr[0] = (unsigned char) param1;
		mp->b_wptr++;
		mp->b_wptr[1] = (unsigned char) param2;
		mp->b_wptr++;
		putnext(q, mp);
		return (1);
	}
	return (0);
}
#endif

static int
testmod_wput(queue_t *q, mblk_t *mp)
{
	union ioctypes *ioc = (typeof(ioc)) mp->b_rptr;
	int err = EINVAL;
	int rval = 0;

	/* we don't queue so we don't need to worry about M_FLUSH */
	switch (mp->b_datap->db_type) {
	case M_IOCTL:
		switch (ioc->iocblk.ioc_cmd) {
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
		case TM_IOC_PSIGNAL:
		{
			int signum;

			signum = *(long *) mp->b_cont->b_rptr;
			printd(("%s: signal number is %d\n", __FUNCTION__, signum));
			/* Synthesize an M_SIG message with a signal (equal to the arg) */
			if (putnextctl1(OTHERQ(q), M_PCSIG, signum))
				goto ack;
			err = ENOSR;
			goto nak;
		}
		case TM_IOC_NSIGNAL:
		{
			int signum;

			signum = *(long *) mp->b_cont->b_rptr;
			printd(("%s: signal number is %d\n", __FUNCTION__, signum));
			/* Synthesize an M_SIG message with a signal (equal to the arg) */
			if (putnextctl1(OTHERQ(q), M_SIG, signum))
				goto ack;
			err = ENOSR;
			goto nak;
		}
		case TM_IOC_COPYIN:
		{
			caddr_t uaddr;

			if (ioc->iocblk.ioc_count != TRANSPARENT || mp->b_cont == NULL
			    || mp->b_cont->b_datap->db_lim - mp->b_cont->b_datap->db_base <
			    FASTBUF) {
				ptrace(("Error path taken!\n"));
				err = EINVAL;
				goto nak;
			}
			uaddr = *(caddr_t *) mp->b_cont->b_rptr;
			mp->b_cont->b_wptr = mp->b_cont->b_rptr + FASTBUF;
			memset(mp->b_cont->b_rptr, 0xa5, FASTBUF);

			mp->b_datap->db_type = M_COPYIN;
			ioc->copyreq.cq_addr = uaddr;
			ioc->copyreq.cq_size = FASTBUF;
			ioc->copyreq.cq_flag = 0;
			ioc->copyreq.cq_private = NULL;
			goto reply;
		}
		case TM_IOC_COPYOUT:
		{
			caddr_t uaddr;

			if (ioc->iocblk.ioc_count != TRANSPARENT || mp->b_cont == NULL
			    || mp->b_cont->b_datap->db_lim - mp->b_cont->b_rptr < FASTBUF) {
				ptrace(("Error path taken!\n"));
				err = EINVAL;
				goto nak;
			}
			uaddr = *(caddr_t *) mp->b_cont->b_rptr;
			mp->b_cont->b_wptr = mp->b_cont->b_rptr + FASTBUF;
			memset(mp->b_cont->b_rptr, 0xa5, FASTBUF);

			mp->b_datap->db_type = M_COPYOUT;
			ioc->copyreq.cq_addr = uaddr;
			ioc->copyreq.cq_size = FASTBUF;
			ioc->copyreq.cq_flag = 0;
			ioc->copyreq.cq_private = NULL;
			goto reply;
		}
		case TM_IOC_IOCTL:
		{
			if (ioc->iocblk.ioc_count == TRANSPARENT) {
				ptrace(("Error path taken!\n"));
				err = EINVAL;
				goto nak;
			}
			if (mp->b_cont == NULL
			    || mp->b_cont->b_wptr - mp->b_cont->b_rptr != ioc->iocblk.ioc_count) {
				ptrace(("Error path taken!\n"));
				err = EINVAL;
				goto nak;
			}
			memset(mp->b_cont->b_rptr, 0xa5, ioc->iocblk.ioc_count);
			mp->b_datap->db_type = M_IOCACK;
			ioc->iocblk.ioc_rval = 0;
			ioc->iocblk.ioc_error = 0;
			goto reply;
		}
		case TM_IOC_COPYIO:
		{
			caddr_t uaddr;

			if (ioc->iocblk.ioc_count != TRANSPARENT || mp->b_cont == NULL
			    || mp->b_cont->b_datap->db_lim - mp->b_cont->b_datap->db_base <
			    FASTBUF) {
				ptrace(("Error path taken!\n"));
				err = EINVAL;
				goto nak;
			}
			uaddr = *(caddr_t *) mp->b_cont->b_rptr;
			mp->b_cont->b_wptr = mp->b_cont->b_rptr + FASTBUF;
			memset(mp->b_cont->b_rptr, 0xa5, FASTBUF);

			mp->b_datap->db_type = M_COPYIN;
			ioc->copyreq.cq_addr = uaddr;
			ioc->copyreq.cq_size = FASTBUF;
			ioc->copyreq.cq_flag = 0;
			ioc->copyreq.cq_private = (mblk_t *) uaddr;
			goto reply;
		}
		default:
			/* pass along anything we don't understand */
			break;
		}
		break;
	case M_IOCDATA:
		switch (ioc->copyresp.cp_cmd) {
		case TM_IOC_HANGUP:
		case TM_IOC_RDERR:
		case TM_IOC_WRERR:
		case TM_IOC_RWERR:
		case TM_IOC_PSIGNAL:
		case TM_IOC_NSIGNAL:
			if (ioc->copyresp.cp_rval != NULL)
				/* abort operations */
				goto free_it;
			break;
		case TM_IOC_COPYIN:
		{
			int i;

			if (ioc->copyresp.cp_rval != NULL)
				/* abort operations */
				goto free_it;
			if (!mp->b_cont || mp->b_cont->b_wptr - mp->b_cont->b_rptr != FASTBUF) {
				ptrace(("Error path taken!\n"));
				err = EINVAL;
				goto nak;
			}
			for (i = 0; i < FASTBUF; i++)
				if ((unsigned char) mp->b_cont->b_rptr[i] != (unsigned char) 0xa5)
					break;
			if (i == FASTBUF) {
				ptrace(("Error path taken!\n"));
				err = EINVAL;
				goto nak;
			}
			rval = 0;
			goto ack;
		}
		case TM_IOC_COPYOUT:
		{
			if (ioc->copyresp.cp_rval != NULL)
				/* abort operations */
				goto free_it;
			rval = 0;
			goto ack;
		}
		case TM_IOC_COPYIO:
		{
			int i;

			if (ioc->copyresp.cp_rval != NULL)
				/* abort operations */
				goto free_it;
			if (ioc->copyresp.cp_private != NULL) {
				if (!mp->b_cont
				    || mp->b_cont->b_wptr - mp->b_cont->b_rptr != FASTBUF) {
					ptrace(("Error path taken!\n"));
					err = EINVAL;
					goto nak;
				}
				for (i = 0; i < FASTBUF; i++)
					if ((unsigned char) mp->b_cont->b_rptr[i] !=
					    (unsigned char) 0xa5)
						break;
				if (i == FASTBUF) {
					ptrace(("Error path taken!\n"));
					err = EINVAL;
					goto nak;
				}
				memset(mp->b_cont->b_rptr, 0xa5, FASTBUF);

				mp->b_datap->db_type = M_COPYOUT;
				ioc->copyreq.cq_addr = (caddr_t) ioc->copyreq.cq_private;
				ioc->copyreq.cq_flag = 0;
				ioc->copyreq.cq_private = NULL;
				goto reply;
			}
			rval = 0;
			goto ack;
		}
		default:
			/* pass along anything we don't understand */
			break;
		}
		break;
	      ack:
		mp->b_datap->db_type = M_IOCACK;
		ioc->iocblk.ioc_count = 0;
		ioc->iocblk.ioc_rval = rval;
		ioc->iocblk.ioc_error = 0;
		goto reply;
	      nak:
		mp->b_datap->db_type = M_IOCNAK;
		ioc->iocblk.ioc_count = 0;
		ioc->iocblk.ioc_rval = -1;
		ioc->iocblk.ioc_error = err;
		goto reply;
	}
	putnext(q, mp);
	return (0);
      free_it:
	freemsg(mp);
	return (0);
      reply:
	qreply(q, mp);
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

#ifdef LFS
static struct fmodsw testmod_fmod = {
	.f_name = CONFIG_STREAMS_TESTMOD_NAME,
	.f_str = &testmod_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};
#endif

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
#ifdef LFS
	if ((err = register_strmod(&testmod_fmod)) < 0)
		return (err);
#endif
#ifdef LIS
	if ((err = lis_register_strmod(&testmod_info, CONFIG_STREAMS_TESTMOD_NAME)) < 0)
		return (err);
#endif
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

#ifdef LFS
	if ((err = unregister_strmod(&testmod_fmod)) < 0)
		return (void) (err);
#endif
#ifdef LIS
	if ((err = lis_unregister_strmod(&testmod_info)) < 0)
		return (void) (err);
#endif
	return (void) (0);
};

#ifdef CONFIG_STREAMS_TESTMOD_MODULE
module_init(testmod_init);
module_exit(testmod_exit);
#endif
