/*****************************************************************************

 @(#) $RCSfile: sc.c,v $ $Name:  $($Revision: 0.9.2.33 $) $Date: 2006/09/29 11:51:14 $

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

 Last Modified $Date: 2006/09/29 11:51:14 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sc.c,v $
 Revision 0.9.2.33  2006/09/29 11:51:14  brian
 - libtool library tweaks in Makefile.am
 - better rpm spec handling in *.spec.in
 - added AC_LIBTOOL_DLOPEN to configure.ac
 - updated some copyright headers
 - rationalized item in two packages
 - added manual pages, drivers and modules to new strtty package

 Revision 0.9.2.32  2006/08/23 11:06:42  brian
 - corrections for compile

 Revision 0.9.2.31  2006/03/10 07:24:14  brian
 - rationalized streams and strutil package sources

 *****************************************************************************/

#ident "@(#) $RCSfile: sc.c,v $ $Name:  $($Revision: 0.9.2.33 $) $Date: 2006/09/29 11:51:14 $"

static char const ident[] =
    "$RCSfile: sc.c,v $ $Name:  $($Revision: 0.9.2.33 $) $Date: 2006/09/29 11:51:14 $";

/* 
 *  This is SC, a STREAMS Configuration module for Linux Fast-STREAMS.  This
 *  is an auxilliary module to the SAD (STREAMS Administrative Driver) and can
 *  be pushed over that module or over the NULS (Null STREAM) driver.
 */

#define _LFS_SOURCE

#include <sys/os7/compat.h>

#include "sys/sc.h"

#ifdef LIS
#define CONFIG_STREAMS_SC_MODID		SC_MOD_ID
#define CONFIG_STREAMS_SC_NAME		SC_MOD_NAME
#endif

#define SC_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SC_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define SC_REVISION	"LfS $RCSfile: sc.c,v $ $Name:  $($Revision: 0.9.2.33 $) $Date: 2006/09/29 11:51:14 $"
#define SC_DEVICE	"SVR 4.2 STREAMS STREAMS Configuration Module (SC)"
#define SC_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SC_LICENSE	"GPL"
#define SC_BANNER	SC_DESCRIP	"\n" \
			SC_COPYRIGHT	"\n" \
			SC_REVISION	"\n" \
			SC_DEVICE	"\n" \
			SC_CONTACT	"\n"
#define SC_SPLASH	SC_DEVICE	" - " \
			SC_REVISION

#ifdef CONFIG_STREAMS_SC_MODULE
MODULE_AUTHOR(SC_CONTACT);
MODULE_DESCRIPTION(SC_DESCRIP);
MODULE_SUPPORTED_DEVICE(SC_DEVICE);
MODULE_LICENSE(SC_LICENSE);
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sc");
#endif
#endif

#ifndef CONFIG_STREAMS_SC_NAME
#error CONFIG_STREAMS_SC_NAME must be defined.
#endif
#ifndef CONFIG_STREAMS_SC_MODID
#error CONFIG_STREAMS_SC_MODID must be defined.
#endif

modID_t modid = CONFIG_STREAMS_SC_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for SC.");

#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_SC_MODID));
MODULE_ALIAS("streams-module-sc");
#endif
#endif

static struct module_info sc_minfo = {
	.mi_idnum = CONFIG_STREAMS_SC_MODID,
	.mi_idname = CONFIG_STREAMS_SC_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat sc_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sc_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

#if defined __LP64__ && defined LFS
#  undef WITH_32BIT_CONVERSION
#  define WITH_32BIT_CONVERSION 1
#endif

static size_t
sc_mlist_copy(long major, struct streamtab *st, caddr_t _mlist, const int reset, const uint flag)
{
	struct module_info *info;
	struct module_stat *stat;

#ifdef WITH_32BIT_CONVERSION
	if (flag == IOC_ILP32) {
		struct sc_mlist32 *mlist = (typeof(mlist)) _mlist;

		if ((mlist->major = major) != -1) {
			if (st->st_wrinit && (info = st->st_wrinit->qi_minfo)) {
				strncpy(mlist->name, info->mi_idname, FMNAMESZ + 1);
				mlist->mi[0].index = 0x8;
				mlist->mi[0].mi_idnum = info->mi_idnum;
				strncpy(mlist->mi[0].mi_idname, info->mi_idname, FMNAMESZ + 1);
				mlist->mi[0].mi_minpsz = info->mi_minpsz;
				mlist->mi[0].mi_maxpsz = info->mi_maxpsz;
				mlist->mi[0].mi_hiwat = info->mi_hiwat;
				mlist->mi[0].mi_lowat = info->mi_lowat;
				if ((stat = st->st_wrinit->qi_mstat)) {
					mlist->ms[0].index = 0x8;
					mlist->ms[0].ms_pcnt = stat->ms_pcnt;
					mlist->ms[0].ms_scnt = stat->ms_scnt;
					mlist->ms[0].ms_ocnt = stat->ms_ocnt;
					mlist->ms[0].ms_ccnt = stat->ms_ccnt;
					mlist->ms[0].ms_acnt = stat->ms_acnt;
					mlist->ms[0].ms_flags = stat->ms_flags;
					mlist->ms[0].ms_xsize = stat->ms_xsize;
				}
			}
			if (st->st_rdinit && (info = st->st_rdinit->qi_minfo)) {
				if (info == st->st_wrinit->qi_minfo)
					mlist->mi[0].index |= 0x4;
				else
					mlist->mi[1].index = 0x4;
				mlist->mi[1].mi_idnum = info->mi_idnum;
				strncpy(mlist->mi[1].mi_idname, info->mi_idname, FMNAMESZ + 1);
				mlist->mi[1].mi_minpsz = info->mi_minpsz;
				mlist->mi[1].mi_maxpsz = info->mi_maxpsz;
				mlist->mi[1].mi_hiwat = info->mi_hiwat;
				mlist->mi[1].mi_lowat = info->mi_lowat;
				if ((stat = st->st_rdinit->qi_mstat)) {
					if (stat == st->st_wrinit->qi_mstat)
						mlist->ms[0].index |= 0x4;
					else
						mlist->ms[1].index = 0x4;
					mlist->ms[1].ms_pcnt = stat->ms_pcnt;
					mlist->ms[1].ms_scnt = stat->ms_scnt;
					mlist->ms[1].ms_ocnt = stat->ms_ocnt;
					mlist->ms[1].ms_ccnt = stat->ms_ccnt;
					mlist->ms[1].ms_acnt = stat->ms_acnt;
					mlist->ms[1].ms_flags = stat->ms_flags;
					mlist->ms[1].ms_xsize = stat->ms_xsize;
				}
			}
			if (st->st_muxwinit && (info = st->st_muxwinit->qi_minfo)) {
				if (info == st->st_wrinit->qi_minfo)
					mlist->mi[0].index |= 0x2;
				else if (info == st->st_rdinit->qi_minfo)
					mlist->mi[1].index |= 0x2;
				else
					mlist->mi[2].index = 0x2;
				mlist->mi[2].mi_idnum = info->mi_idnum;
				strncpy(mlist->mi[2].mi_idname, info->mi_idname, FMNAMESZ + 1);
				mlist->mi[2].mi_minpsz = info->mi_minpsz;
				mlist->mi[2].mi_maxpsz = info->mi_maxpsz;
				mlist->mi[2].mi_hiwat = info->mi_hiwat;
				mlist->mi[2].mi_lowat = info->mi_lowat;
				if ((stat = st->st_muxwinit->qi_mstat)) {
					if (stat == st->st_wrinit->qi_mstat)
						mlist->ms[0].index |= 0x2;
					else if (stat == st->st_rdinit->qi_mstat)
						mlist->ms[1].index |= 0x2;
					else
						mlist->ms[2].index = 0x2;
					mlist->ms[2].ms_pcnt = stat->ms_pcnt;
					mlist->ms[2].ms_scnt = stat->ms_scnt;
					mlist->ms[2].ms_ocnt = stat->ms_ocnt;
					mlist->ms[2].ms_ccnt = stat->ms_ccnt;
					mlist->ms[2].ms_acnt = stat->ms_acnt;
					mlist->ms[2].ms_flags = stat->ms_flags;
					mlist->ms[2].ms_xsize = stat->ms_xsize;
				}
			}
			if (st->st_muxrinit && (info = st->st_muxrinit->qi_minfo)) {
				if (info == st->st_wrinit->qi_minfo)
					mlist->mi[0].index |= 0x1;
				else if (info == st->st_rdinit->qi_minfo)
					mlist->mi[1].index |= 0x1;
				else if (info == st->st_muxwinit->qi_minfo)
					mlist->mi[2].index |= 0x1;
				else
					mlist->mi[3].index = 0x1;
				mlist->mi[3].mi_idnum = info->mi_idnum;
				strncpy(mlist->mi[3].mi_idname, info->mi_idname, FMNAMESZ + 1);
				mlist->mi[3].mi_minpsz = info->mi_minpsz;
				mlist->mi[3].mi_maxpsz = info->mi_maxpsz;
				mlist->mi[3].mi_hiwat = info->mi_hiwat;
				mlist->mi[3].mi_lowat = info->mi_lowat;
				if ((stat = st->st_muxrinit->qi_mstat)) {
					if (stat == st->st_wrinit->qi_mstat)
						mlist->ms[0].index |= 0x1;
					else if (stat == st->st_rdinit->qi_mstat)
						mlist->ms[1].index |= 0x1;
					else if (stat == st->st_muxwinit->qi_mstat)
						mlist->ms[2].index |= 0x1;
					else
						mlist->ms[3].index = 0x1;
					mlist->ms[3].ms_pcnt = stat->ms_pcnt;
					mlist->ms[3].ms_scnt = stat->ms_scnt;
					mlist->ms[3].ms_ocnt = stat->ms_ocnt;
					mlist->ms[3].ms_ccnt = stat->ms_ccnt;
					mlist->ms[3].ms_acnt = stat->ms_acnt;
					mlist->ms[3].ms_flags = stat->ms_flags;
					mlist->ms[3].ms_xsize = stat->ms_xsize;
				}
			}
			if (reset) {
				if (st->st_wrinit && (stat = st->st_wrinit->qi_mstat)) {
					stat->ms_pcnt = 0;
					stat->ms_scnt = 0;
					stat->ms_ocnt = 0;
					stat->ms_ccnt = 0;
					stat->ms_acnt = 0;
					stat->ms_flags = 0;
				}
				if (st->st_rdinit && (stat = st->st_rdinit->qi_mstat)) {
					stat->ms_pcnt = 0;
					stat->ms_scnt = 0;
					stat->ms_ocnt = 0;
					stat->ms_ccnt = 0;
					stat->ms_acnt = 0;
					stat->ms_flags = 0;
				}
				if (st->st_muxwinit && (stat = st->st_muxwinit->qi_mstat)) {
					stat->ms_pcnt = 0;
					stat->ms_scnt = 0;
					stat->ms_ocnt = 0;
					stat->ms_ccnt = 0;
					stat->ms_acnt = 0;
					stat->ms_flags = 0;
				}
				if (st->st_muxrinit && (stat = st->st_muxrinit->qi_mstat)) {
					stat->ms_pcnt = 0;
					stat->ms_scnt = 0;
					stat->ms_ocnt = 0;
					stat->ms_ccnt = 0;
					stat->ms_acnt = 0;
					stat->ms_flags = 0;
				}
			}
		}
		return sizeof(struct sc_mlist32);
	} else
#endif				/* WITH_32BIT_CONVERSION */
	{
		struct sc_mlist *mlist = (typeof(mlist)) _mlist;

		if ((mlist->major = major) != -1) {
			if (st->st_wrinit && (info = st->st_wrinit->qi_minfo)) {
				strncpy(mlist->name, info->mi_idname, FMNAMESZ + 1);
				mlist->mi[0].index = 0x8;
				mlist->mi[0].mi_idnum = info->mi_idnum;
				strncpy(mlist->mi[0].mi_idname, info->mi_idname, FMNAMESZ + 1);
				mlist->mi[0].mi_minpsz = info->mi_minpsz;
				mlist->mi[0].mi_maxpsz = info->mi_maxpsz;
				mlist->mi[0].mi_hiwat = info->mi_hiwat;
				mlist->mi[0].mi_lowat = info->mi_lowat;
				if ((stat = st->st_wrinit->qi_mstat)) {
					mlist->ms[0].index = 0x8;
					mlist->ms[0].ms_pcnt = stat->ms_pcnt;
					mlist->ms[0].ms_scnt = stat->ms_scnt;
					mlist->ms[0].ms_ocnt = stat->ms_ocnt;
					mlist->ms[0].ms_ccnt = stat->ms_ccnt;
					mlist->ms[0].ms_acnt = stat->ms_acnt;
					mlist->ms[0].ms_flags = stat->ms_flags;
					mlist->ms[0].ms_xsize = stat->ms_xsize;
				}
			}
			if (st->st_rdinit && (info = st->st_rdinit->qi_minfo)) {
				if (info == st->st_wrinit->qi_minfo)
					mlist->mi[0].index |= 0x4;
				else
					mlist->mi[1].index = 0x4;
				mlist->mi[1].mi_idnum = info->mi_idnum;
				strncpy(mlist->mi[1].mi_idname, info->mi_idname, FMNAMESZ + 1);
				mlist->mi[1].mi_minpsz = info->mi_minpsz;
				mlist->mi[1].mi_maxpsz = info->mi_maxpsz;
				mlist->mi[1].mi_hiwat = info->mi_hiwat;
				mlist->mi[1].mi_lowat = info->mi_lowat;
				if ((stat = st->st_rdinit->qi_mstat)) {
					if (stat == st->st_wrinit->qi_mstat)
						mlist->ms[0].index |= 0x4;
					else
						mlist->ms[1].index = 0x4;
					mlist->ms[1].ms_pcnt = stat->ms_pcnt;
					mlist->ms[1].ms_scnt = stat->ms_scnt;
					mlist->ms[1].ms_ocnt = stat->ms_ocnt;
					mlist->ms[1].ms_ccnt = stat->ms_ccnt;
					mlist->ms[1].ms_acnt = stat->ms_acnt;
					mlist->ms[1].ms_flags = stat->ms_flags;
					mlist->ms[1].ms_xsize = stat->ms_xsize;
				}
			}
			if (st->st_muxwinit && (info = st->st_muxwinit->qi_minfo)) {
				if (info == st->st_wrinit->qi_minfo)
					mlist->mi[0].index |= 0x2;
				else if (info == st->st_rdinit->qi_minfo)
					mlist->mi[1].index |= 0x2;
				else
					mlist->mi[2].index = 0x2;
				mlist->mi[2].mi_idnum = info->mi_idnum;
				strncpy(mlist->mi[2].mi_idname, info->mi_idname, FMNAMESZ + 1);
				mlist->mi[2].mi_minpsz = info->mi_minpsz;
				mlist->mi[2].mi_maxpsz = info->mi_maxpsz;
				mlist->mi[2].mi_hiwat = info->mi_hiwat;
				mlist->mi[2].mi_lowat = info->mi_lowat;
				if ((stat = st->st_muxwinit->qi_mstat)) {
					if (stat == st->st_wrinit->qi_mstat)
						mlist->ms[0].index |= 0x2;
					else if (stat == st->st_rdinit->qi_mstat)
						mlist->ms[1].index |= 0x2;
					else
						mlist->ms[2].index = 0x2;
					mlist->ms[2].ms_pcnt = stat->ms_pcnt;
					mlist->ms[2].ms_scnt = stat->ms_scnt;
					mlist->ms[2].ms_ocnt = stat->ms_ocnt;
					mlist->ms[2].ms_ccnt = stat->ms_ccnt;
					mlist->ms[2].ms_acnt = stat->ms_acnt;
					mlist->ms[2].ms_flags = stat->ms_flags;
					mlist->ms[2].ms_xsize = stat->ms_xsize;
				}
			}
			if (st->st_muxrinit && (info = st->st_muxrinit->qi_minfo)) {
				if (info == st->st_wrinit->qi_minfo)
					mlist->mi[0].index |= 0x1;
				else if (info == st->st_rdinit->qi_minfo)
					mlist->mi[1].index |= 0x1;
				else if (info == st->st_muxwinit->qi_minfo)
					mlist->mi[2].index |= 0x1;
				else
					mlist->mi[3].index = 0x1;
				mlist->mi[3].mi_idnum = info->mi_idnum;
				strncpy(mlist->mi[3].mi_idname, info->mi_idname, FMNAMESZ + 1);
				mlist->mi[3].mi_minpsz = info->mi_minpsz;
				mlist->mi[3].mi_maxpsz = info->mi_maxpsz;
				mlist->mi[3].mi_hiwat = info->mi_hiwat;
				mlist->mi[3].mi_lowat = info->mi_lowat;
				if ((stat = st->st_muxrinit->qi_mstat)) {
					if (stat == st->st_wrinit->qi_mstat)
						mlist->ms[0].index |= 0x1;
					else if (stat == st->st_rdinit->qi_mstat)
						mlist->ms[1].index |= 0x1;
					else if (stat == st->st_muxwinit->qi_mstat)
						mlist->ms[2].index |= 0x1;
					else
						mlist->ms[3].index = 0x1;
					mlist->ms[3].ms_pcnt = stat->ms_pcnt;
					mlist->ms[3].ms_scnt = stat->ms_scnt;
					mlist->ms[3].ms_ocnt = stat->ms_ocnt;
					mlist->ms[3].ms_ccnt = stat->ms_ccnt;
					mlist->ms[3].ms_acnt = stat->ms_acnt;
					mlist->ms[3].ms_flags = stat->ms_flags;
					mlist->ms[3].ms_xsize = stat->ms_xsize;
				}
			}
			if (reset) {
				if (st->st_wrinit && (stat = st->st_wrinit->qi_mstat)) {
					stat->ms_pcnt = 0;
					stat->ms_scnt = 0;
					stat->ms_ocnt = 0;
					stat->ms_ccnt = 0;
					stat->ms_acnt = 0;
					stat->ms_flags = 0;
				}
				if (st->st_rdinit && (stat = st->st_rdinit->qi_mstat)) {
					stat->ms_pcnt = 0;
					stat->ms_scnt = 0;
					stat->ms_ocnt = 0;
					stat->ms_ccnt = 0;
					stat->ms_acnt = 0;
					stat->ms_flags = 0;
				}
				if (st->st_muxwinit && (stat = st->st_muxwinit->qi_mstat)) {
					stat->ms_pcnt = 0;
					stat->ms_scnt = 0;
					stat->ms_ocnt = 0;
					stat->ms_ccnt = 0;
					stat->ms_acnt = 0;
					stat->ms_flags = 0;
				}
				if (st->st_muxrinit && (stat = st->st_muxrinit->qi_mstat)) {
					stat->ms_pcnt = 0;
					stat->ms_scnt = 0;
					stat->ms_ocnt = 0;
					stat->ms_ccnt = 0;
					stat->ms_acnt = 0;
					stat->ms_flags = 0;
				}
			}
		}
		return sizeof(struct sc_mlist);
	}
}

static size_t
str_mlist_count(void)
{
#ifdef LIS
	int i, cdev_count = 0, fmod_count = 0;

	for (i = 0; i < MAX_STRDEV; i++)
		if (lis_fstr_sw[i].f_str && lis_fstr_sw[i].f_count)
			cdev_count++;
	for (i = 1; i < MAX_STRMOD; i++)
		if ((lis_fmod_sw[i].f_state & LIS_MODSTATE_INITED)
		    && lis_fmod_sw[i].f_count)
			fmod_count++;
#endif
	return (cdev_count + fmod_count);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUT routine
 *  
 *  -------------------------------------------------------------------------
 */
static streamscall int
sc_wput(queue_t *q, mblk_t *mp)
{
	union ioctypes *ioc;
	int err = 0, rval = 0, reset = 0;
	mblk_t *dp = mp->b_cont;

	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		/* canonical flushing */
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
	{
		caddr_t uaddr;
		size_t usize;

		_trace();
		ioc = (typeof(ioc)) mp->b_rptr;

#ifdef WITH_32BIT_CONVERSION
		if (ioc->iocblk.ioc_flag == IOC_ILP32) {
			uaddr = (caddr_t) (unsigned long) (uint32_t) *(unsigned long *) dp->b_rptr;
			usize = sizeof(struct sc_list32);
		} else
#endif
		{
			uaddr = (caddr_t) *(unsigned long *) dp->b_rptr;
			usize = sizeof(struct sc_list);
		}
		switch (ioc->iocblk.ioc_cmd) {
		case SC_IOC_RESET:
			reset = 1;
		case SC_IOC_LIST:
			/* there is really no reason why a regular user cannot list modules and
			   related information. */
#if 0
			_trace();
			err = -EPERM;
			if (ioc->iocblk.ioc_uid != 0) {
				_ptrace(("Error path taken!\n"));
				goto nak;
			}
#endif
			_trace();
			if (ioc->iocblk.ioc_count == TRANSPARENT) {
				_trace();
				if (uaddr == NULL) {
					rval = str_mlist_count();
					goto ack;
				}
				mp->b_datap->db_type = M_COPYIN;
				ioc->copyreq.cq_addr = uaddr;
				ioc->copyreq.cq_size = usize;
				ioc->copyreq.cq_flag = 0;
				ioc->copyreq.cq_private = (mblk_t *) 0;
				qreply(q, mp);
				return (0);
			}
			_trace();
			/* doesn't support I_STR yet, just TRANSPARENT */
			err = -EINVAL;
			goto nak;
		}
		break;
	}
	case M_IOCDATA:
		ioc = (typeof(ioc)) mp->b_rptr;

		switch (ioc->copyresp.cp_cmd) {
		case SC_IOC_RESET:
			reset = 1;
		case SC_IOC_LIST:
			_trace();
			if (ioc->copyresp.cp_rval != 0) {
#ifdef LFS
				_ptrace(("Aborting ioctl!\n"));
				goto abort;
#endif
#ifdef LIS
				/* LiS has a bug here... */
				_ptrace(("Nacking failed ioctl!\n"));
				err = -(long) ioc->copyresp.cp_rval;
				goto nak;
#endif
			}
			_trace();
			if (ioc->copyresp.cp_private == (mblk_t *) 0) {
				int n, count;
				caddr_t uaddr;
				size_t usize;

				n = 0;
				if (!dp || dp->b_wptr == dp->b_rptr) {
					rval = str_mlist_count();
					goto ack;
				}
#ifdef WITH_32BIT_CONVERSION
				if (ioc->copyresp.cp_flag == IOC_ILP32) {
					if (dp->b_wptr < dp->b_rptr + sizeof(struct sc_list32)) {
						_ptrace(("Error path taken!\n"));
						err = -EFAULT;
						goto nak;
					} else {
						struct sc_list32 *sclp = (typeof(sclp)) dp->b_rptr;

						count = sclp->sc_nmods;
						uaddr = (caddr_t) (unsigned long) sclp->sc_mlist;
						usize = count * sizeof(struct sc_mlist32);
					}
				} else
#endif
				{
					if (dp->b_wptr < dp->b_rptr + sizeof(struct sc_list)) {
						_ptrace(("Error path taken!\n"));
						err = -EFAULT;
						goto nak;
					} else {
						struct sc_list *sclp = (typeof(sclp)) dp->b_rptr;

						count = sclp->sc_nmods;
						uaddr = (caddr_t) sclp->sc_mlist;
						usize = count * sizeof(struct sc_mlist);
					}
				}
				if (count < 0) {
					_ptrace(("Error path taken!\n"));
					err = -EINVAL;
					goto nak;
				}
				if (count > 100) {
					_ptrace(("Error path taken!\n"));
					err = -ERANGE;
					goto nak;
				}
				if (count == 0) {
					rval = str_mlist_count();
					goto ack;
				}
				if (!(dp = allocb(usize, BPRI_MED))) {
					_ptrace(("Error path taken!\n"));
					err = -ENOSR;
					goto nak;
				}
				dp->b_wptr = dp->b_rptr + usize;
				bzero(dp->b_rptr, usize);
				freemsg(mp->b_cont);
				mp->b_cont = dp;
#ifdef LIS
				{
					int i;
					uint flag = 0;
					caddr_t mlist = (typeof(mlist)) dp->b_rptr;

					_trace();
					if (n < count) {
						_trace();
						/* list all devices */
						for (i = 0; i < MAX_STRDEV; i++) {
							struct fmodsw *cdev;
							struct streamtab *st;

							cdev = &lis_fstr_sw[i];
							if (!cdev->f_str || !cdev->f_count)
								continue;
							if (n >= count)
								break;
							st = cdev->f_str;
							mlist +=
							    sc_mlist_copy(i, st, mlist, reset,
									  flag);
							n++;
						}
					}
					_trace();
					if (n < count) {
						/* list all modules */
						for (i = 1; i < MAX_STRMOD; i++) {
							struct fmodsw *fmod;
							struct streamtab *st;

							fmod = &lis_fmod_sw[i];
							if (!fmod->f_str || !fmod->f_count)
								continue;
							if (n >= count)
								break;
							st = fmod->f_str;
							mlist +=
							    sc_mlist_copy(0, st, mlist, reset,
									  flag);
							n++;
						}
					}
				}
#endif
#ifdef LFS
				{
					struct list_head *pos;
					uint flag = ioc->copyresp.cp_flag;
					caddr_t mlist = (typeof(mlist)) dp->b_rptr;

					_trace();
					if (n < count) {
						_trace();
						/* output all devices */
						read_lock(&cdevsw_lock);
						list_for_each(pos, &cdevsw_list) {
							struct cdevsw *cdev;
							struct streamtab *st;

							if (n >= count)
								break;
							cdev =
							    list_entry(pos, struct cdevsw, d_list);
							st = cdev->d_str;
							mlist +=
							    sc_mlist_copy(cdev->d_major, st,
									  mlist, reset, flag);
							n++;
						}
						read_unlock(&cdevsw_lock);
					}
					_trace();
					if (n < count) {
						_trace();
						/* output all modules */
						read_lock(&fmodsw_lock);
						list_for_each(pos, &fmodsw_list) {
							struct fmodsw *fmod;
							struct streamtab *st;

							if (n >= count)
								break;
							fmod =
							    list_entry(pos, struct fmodsw, f_list);
							st = fmod->f_str;
							mlist +=
							    sc_mlist_copy(0, st, mlist, reset,
									  flag);
							n++;
						}
						read_unlock(&fmodsw_lock);
					}
					_trace();
					/* zero all excess elements */
					for (; n < count; n++) {
						mlist +=
						    sc_mlist_copy(-1, NULL, mlist, reset, flag);
					}
				}
#endif
				_trace();
				mp->b_datap->db_type = M_COPYOUT;
				ioc->copyreq.cq_addr = uaddr;
				ioc->copyreq.cq_size = usize;
				ioc->copyreq.cq_flag = 0;
				ioc->copyreq.cq_private = (mblk_t *) (long) count;
				qreply(q, mp);
				return (0);
			} else {
				_trace();
				/* done */
				rval = (int) (long) ioc->copyresp.cp_private;
				goto ack;
			}
		}
	      nak:
		mp->b_datap->db_type = M_IOCNAK;
		ioc->iocblk.ioc_count = 0;
		ioc->iocblk.ioc_rval = -1;
		ioc->iocblk.ioc_error = -err;
		qreply(q, mp);
		return (0);
	      ack:
		mp->b_datap->db_type = M_IOCACK;
		ioc->iocblk.ioc_count = 0;
		ioc->iocblk.ioc_rval = rval;
		ioc->iocblk.ioc_error = 0;
		qreply(q, mp);
		return (0);
#ifdef LFS
	      abort:
		_ctrace(freemsg(mp));
		return (0);
#endif
	}
	putnext(q, mp);
	return (0);
}
static streamscall int
sc_rput(queue_t *q, mblk_t *mp)
{
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
static streamscall int
sc_open(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	if (q->q_ptr)
		return (0);	/* already open */
	if (sflag == MODOPEN && WR(q)->q_next != NULL) {
		/* don't need to be privileged to push the module, just to use it */
#if 0
		if (crp->cr_uid != 0 && crp->cr_ruid != 0)
			return (-EACCES);
#endif
		q->q_ptr = WR(q)->q_ptr = (void *) 1;
		qprocson(q);
		return (0);
	}
	return (-EIO);		/* can't be opened as driver */
}
static streamscall int
sc_close(queue_t *q, int oflag, cred_t *crp)
{
	qprocsoff(q);
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

static struct qinit sc_rqinit = {
	.qi_putp = sc_rput,
	.qi_qopen = sc_open,
	.qi_qclose = sc_close,
	.qi_minfo = &sc_minfo,
	.qi_mstat = &sc_rstat,
};

static struct qinit sc_wqinit = {
	.qi_putp = sc_wput,
	.qi_minfo = &sc_minfo,
	.qi_mstat = &sc_wstat,
};

static struct streamtab sc_info = {
	.st_rdinit = &sc_rqinit,
	.st_wrinit = &sc_wqinit,
};

#ifdef LIS
#define fmodsw _fmodsw
#endif
static struct fmodsw sc_fmod = {
	.f_name = CONFIG_STREAMS_SC_NAME,
	.f_str = &sc_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef LFS
static void *sc_opaque;
#endif

static void
sc_unregister_ioctl32(void)
{
#ifdef LFS
	if (sc_opaque != NULL)
		unregister_ioctl32(sc_opaque);
#endif
}

static int
sc_register_ioctl32(void)
{
#ifdef LFS
	if ((sc_opaque = register_ioctl32(SC_IOC_LIST)) == NULL)
		return (-ENOMEM);
#endif
	return (0);
}

#ifdef CONFIG_STREAMS_SC_MODULE
static
#endif
int __init
sc_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_SC_MODULE
	printk(KERN_INFO SC_BANNER);
#else
	printk(KERN_INFO SC_SPLASH);
#endif
	sc_minfo.mi_idnum = modid;
	if ((err = sc_register_ioctl32()) < 0)
		return (err);
	if ((err = register_strmod(&sc_fmod)) < 0) {
		sc_unregister_ioctl32();
		return (err);
	}
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
};

#ifdef CONFIG_STREAMS_SC_MODULE
static
#endif
void __exit
sc_exit(void)
{
	unregister_strmod(&sc_fmod);
	sc_unregister_ioctl32();
};

#ifdef CONFIG_STREAMS_SC_MODULE
module_init(sc_init);
module_exit(sc_exit);
#endif
