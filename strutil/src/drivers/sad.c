/*****************************************************************************

 @(#) $RCSfile: sad.c,v $ $Name:  $($Revision: 0.9.2.34 $) $Date: 2005/12/28 10:01:21 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2005/12/28 10:01:21 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sad.c,v $ $Name:  $($Revision: 0.9.2.34 $) $Date: 2005/12/28 10:01:21 $"

static char const ident[] =
    "$RCSfile: sad.c,v $ $Name:  $($Revision: 0.9.2.34 $) $Date: 2005/12/28 10:01:21 $";

#define _LFS_SOURCE
#include <sys/os7/compat.h>

#ifdef LIS
#define CONFIG_STREAMS_SAD_MODID	SAD_DRV_ID
#define CONFIG_STREAMS_SAD_NAME		SAD_DRV_NAME
#define CONFIG_STREAMS_SAD_MAJOR	SAD_CMAJOR_0
#endif

#define SAD_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SAD_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
#define SAD_REVISION	"LfS $RCSfile: sad.c,v $ $Name:  $($Revision: 0.9.2.34 $) $Date: 2005/12/28 10:01:21 $"
#define SAD_DEVICE	"SVR 4.2 STREAMS Administrative Driver (SAD)"
#define SAD_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SAD_LICENSE	"GPL"
#define SAD_BANNER	SAD_DESCRIP	"\n" \
			SAD_COPYRIGHT	"\n" \
			SAD_REVISION	"\n" \
			SAD_DEVICE	"\n" \
			SAD_CONTACT	"\n"
#define SAD_SPLASH	SAD_DEVICE	" - " \
			SAD_REVISION	"\n"

#ifdef CONFIG_STREAMS_SAD_MODULE
MODULE_AUTHOR(SAD_CONTACT);
MODULE_DESCRIPTION(SAD_DESCRIP);
MODULE_SUPPORTED_DEVICE(SAD_DEVICE);
MODULE_LICENSE(SAD_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sad");
#endif
#endif

#ifndef CONFIG_STREAMS_SAD_NAME
#error CONFIG_STREAMS_SAD_NAME must be defined.
#endif
#ifndef CONFIG_STREAMS_SAD_MAJOR
#error CONFIG_STREAMS_SAD_MAJOR must be defined.
#endif
#ifndef CONFIG_STREAMS_SAD_MODID
#error CONFIG_STREAMS_SAD_MODID must be defined.
#endif

modID_t modid = CONFIG_STREAMS_SAD_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module id number for STREAMS-administrative driver.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_SAD_MODID));
MODULE_ALIAS("streams-driver-sad");
#endif

major_t major = CONFIG_STREAMS_SAD_MAJOR;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Major device number for STREAMS-administrative driver.");

#ifdef MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_SAD_MAJOR) "-*");
MODULE_ALIAS("/dev/sad");
MODULE_ALIAS("/dev/sad/admin");
MODULE_ALIAS("/dev/sad/user");
#ifdef LFS
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_SAD_MAJOR));
MODULE_ALIAS("/dev/streams/sad");
MODULE_ALIAS("/dev/streams/sad/admin");
MODULE_ALIAS("/dev/streams/sad/user");
#endif
#endif

static struct module_info sad_minfo = {
	mi_idnum:CONFIG_STREAMS_SAD_MODID,
	mi_idname:CONFIG_STREAMS_SAD_NAME,
	mi_minpsz:0,
	mi_maxpsz:INFPSZ,
	mi_hiwat:STRHIGH,
	mi_lowat:STRLOW,
};

/* private structures */
struct sad {
	int assigned;
	int iocstate;
	int nmods;
} sads[2];

static int
sad_put(queue_t *q, mblk_t *mp)
{
	struct sad *sad = q->q_ptr;
	union ioctypes *ioc;
	int err = 0, rval = 0;
	mblk_t *dp = mp->b_cont;

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHALL);
			else
				flushq(q, FLUSHALL);
			mp->b_rptr[0] &= ~FLUSHW;
		}
		if (mp->b_rptr[0] & FLUSHR) {
			queue_t *rq = RD(q);

			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(rq, mp->b_rptr[1], FLUSHALL);
			else
				flushq(rq, FLUSHALL);
			qreply(q, mp);
			return (0);
		}
		break;
	case M_IOCTL:
		ioc = (typeof(ioc)) mp->b_rptr;
		switch (ioc->iocblk.ioc_cmd) {
		case SAD_SAP:
			err = -EPERM;
			if (ioc->iocblk.ioc_uid != 0)
				goto nak;
			if (ioc->iocblk.ioc_count == TRANSPARENT) {
				mp->b_datap->db_type = M_COPYIN;
				ioc->copyreq.cq_addr = *(void **) dp->b_rptr;
				ioc->copyreq.cq_size = sizeof(struct strapush);
				ioc->copyreq.cq_flag = 0;
				sad->iocstate = 1;
				qreply(q, mp);
				return (0);
			}
			err = -EFAULT;
			if (!dp || dp->b_wptr < dp->b_rptr + sizeof(struct strapush))
				goto nak;
			sad->iocstate = 1;
			goto sad_sap_state1;
		case SAD_GAP:
			if (ioc->iocblk.ioc_count == TRANSPARENT) {
				mp->b_datap->db_type = M_COPYIN;
				ioc->copyreq.cq_addr = *(void **) dp->b_rptr;
				ioc->copyreq.cq_size = sizeof(struct strapush);
				ioc->copyreq.cq_flag = 0;
				sad->iocstate = 1;
				qreply(q, mp);
				return (0);
			}
			err = -EFAULT;
			if (!dp || dp->b_wptr < dp->b_rptr + sizeof(struct strapush))
				goto nak;
			sad->iocstate = 1;
			goto sad_gap_state1;
		case SAD_VML:
			if (ioc->iocblk.ioc_count == TRANSPARENT) {
				mp->b_datap->db_type = M_COPYIN;
				ioc->copyreq.cq_addr = *(void **) dp->b_rptr;
				ioc->copyreq.cq_size = sizeof(struct str_list);
				ioc->copyreq.cq_flag = 0;
				sad->iocstate = 1;
				qreply(q, mp);
				return (0);
			}
			err = -EFAULT;
			if (!dp || dp->b_wptr < dp->b_rptr + sizeof(struct str_list))
				goto nak;
			sad->iocstate = 1;
			goto sad_vml_state1;
		}
		err = -EOPNOTSUPP;
		goto nak;
	case M_IOCDATA:
		ioc = (typeof(ioc)) mp->b_rptr;
		switch (ioc->copyresp.cp_cmd) {
		case SAD_SAP:
			err = -(long) ioc->copyresp.cp_rval;
			if (err == 0) {
				struct strapush *sap;

				switch (sad->iocstate) {
				case 1:
				      sad_sap_state1:
					err = -EFAULT;
					if (!dp || dp->b_wptr < dp->b_rptr + sizeof(*sap))
						goto nak;
					sap = (typeof(sap)) dp->b_rptr;
					switch (sap->sap_cmd) {
					case SAP_CLEAR:
#if 0
						err = -EINVAL;
						if (0 > sap->sap_major ||
						    sap->sap_major >= MAX_CHRDEV)
							goto nak;
#endif
						err = autopush_del(sap);
						if (err)
							goto nak;
						goto ack;
					case SAP_ONE:	/* just a range of 1 */
						sap->sap_lastminor = sap->sap_minor;
						goto range;
					case SAP_ALL:	/* just a range of all */
						sap->sap_lastminor = 255;
						sap->sap_minor = 0;
						goto range;
					case SAP_RANGE:
					      range:
						err = -EINVAL;
						if (sap->sap_lastminor >= sap->sap_minor)
							goto nak;
#if 0
						err = -EINVAL;
						if (0 > sap->sap_major ||
						    sap->sap_major >= MAX_CHRDEV)
							goto nak;
#endif
						err = autopush_add(sap);
						if (err)
							goto nak;
						goto ack;
					}
					err = -EINVAL;
					break;
				}
			}
			goto nak;
		case SAD_GAP:
			err = -(long) ioc->copyresp.cp_rval;
			if (err == 0) {
				struct strapush *sap;

				switch (sad->iocstate) {
				case 1:
				      sad_gap_state1:
					sap = (typeof(sap)) dp->b_rptr;
					err = -ENODEV;
					if (!(sap = autopush_find(makedevice(sap->sap_major,
									     sap->sap_minor))))
						goto nak;
					bcopy(sap, dp->b_rptr, sizeof(*sap));
					mp->b_datap->db_type = M_COPYOUT;
					sad->iocstate = 2;
					qreply(q, mp);
					return (0);
				case 2:
					/* done */
					sad->iocstate = 0;
					goto ack;
				}
				err = -EIO;
			}
			goto nak;
		case SAD_VML:
			err = -(long) ioc->copyresp.cp_rval;
			if (err == 0) {
				struct str_list *slp;
				struct str_mlist *slm;

				switch (sad->iocstate) {
				case 1:
				      sad_vml_state1:
					slp = (typeof(slp)) dp->b_rptr;
					err = -EINVAL;
					if ((sad->nmods = slp->sl_nmods) < 1)
						goto nak;
					mp->b_datap->db_type = M_COPYIN;
					ioc->copyreq.cq_addr = (void *) slp->sl_modlist;
					ioc->copyreq.cq_size =
					    sad->nmods * sizeof(struct str_mlist);
					ioc->copyreq.cq_flag = 0;
					sad->iocstate = 2;
					qreply(q, mp);
					return (0);
				case 2:
					slm = (typeof(slm)) dp->b_rptr;
					if ((err = autopush_vml(slm, sad->nmods)) < 0)
						goto nak;
					rval = err;
					goto ack;
				}
			}
			goto nak;
		}
		return (0);
	}
	freemsg(mp);
	return (0);
      nak:
	sad->iocstate = 0;
	mp->b_datap->db_type = M_IOCNAK;
	ioc->iocblk.ioc_rval = -1;
	ioc->iocblk.ioc_error = -err;
	qreply(q, mp);
	return (0);
      ack:
	sad->iocstate = 0;
	mp->b_datap->db_type = M_IOCACK;
	ioc->iocblk.ioc_rval = rval;
	ioc->iocblk.ioc_error = 0;
	qreply(q, mp);
	return (0);
}

 /* There are two minors 0 is the /dev/sad/admin driver and 1 is the /dev/sad/user driver.
    Permission for access to the /dev/sad/admin minor is performed by filesystem permission on the
    character device and a check on open. */
static int
sad_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	major_t major = getmajor(*devp);
	minor_t minor = getminor(*devp);

	if (q->q_ptr)
		return (0);	/* already open */
	switch (sflag) {
	case CLONEOPEN:
		minor = 1;
		if (sads[minor].assigned != 0)
			break;
	case DRVOPEN:
		if (minor != 0 && minor != 1)
			break;
		if (minor != 0 || crp->cr_uid == 0 || crp->cr_ruid == 0)
			return (-EACCES);
		*devp = makedevice(major, minor);
		sads[minor].assigned |= 1;
		q->q_ptr = WR(q)->q_ptr = &sads[minor];
		return (0);
	}
	return (-ENXIO);
}
static int
sad_close(queue_t *q, int oflag, cred_t *crp)
{
	struct sad *sad = q->q_ptr;

	q->q_ptr = WR(q)->q_ptr = NULL;
	sad->assigned = 0;
	sad->iocstate = 0;
	return (0);
}

static struct qinit sad_qinit = {
	qi_putp:sad_put,
	qi_qopen:sad_open,
	qi_qclose:sad_close,
	qi_minfo:&sad_minfo,
};

static struct streamtab sad_info = {
	st_rdinit:&sad_qinit,
	st_wrinit:&sad_qinit,
};

static struct cdevsw sad_cdev = {
	d_name:"sad",
	d_str:&sad_info,
	d_flag:0,
	d_fop:NULL,
	d_mode:S_IFCHR | S_IRUGO | S_IWUGO,
	d_kmod:THIS_MODULE,
};

static struct devnode sad_node_admin = {
	n_name:"admin",
	n_flag:0,
	n_mode:S_IFCHR | S_IRUSR | S_IWUSR,
};

static struct devnode sad_node_user = {
	n_name:"user",
	n_flag:0,
	n_mode:S_IFCHR | S_IRUGO | S_IWUGO,
};

#ifdef CONFIG_STREAMS_SAD_MODULE
static
#endif
int __init
sad_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_SAD_MODULE
	printk(KERN_INFO SAD_BANNER);
#else
	printk(KERN_INFO SAD_SPLASH);
#endif
	sad_minfo.mi_idnum = modid;
	if ((err = register_strdev(&sad_cdev, major)) < 0)
		return (err);
	if (major == 0 && err > 0)
		major = err;
	bzero(&sads, sizeof(sads));
	register_strnod(&sad_cdev, &sad_node_admin, 0);
	register_strnod(&sad_cdev, &sad_node_user, 1);
	return (0);
};

#ifdef CONFIG_STREAMS_SAD_MODULE
static
#endif
void __exit
sad_exit(void)
{
	unregister_strnod(&sad_cdev, 1);
	unregister_strnod(&sad_cdev, 0);
	unregister_strdev(&sad_cdev, major);
};

#ifdef CONFIG_STREAMS_SAD_MODULE
module_init(sad_init);
module_exit(sad_exit);
#endif
