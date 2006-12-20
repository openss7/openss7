/*****************************************************************************

 @(#) $RCSfile: hdlc.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/12/20 23:07:07 $

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

 Last Modified $Date: 2006/12/20 23:07:07 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: hdlc.c,v $
 Revision 0.9.2.1  2006/12/20 23:07:07  brian
 - new working files

 *****************************************************************************/

#ident "@(#) $RCSfile: hdlc.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/12/20 23:07:07 $"

static char const ident[] = "$RCSfile: hdlc.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/12/20 23:07:07 $";

/*
 *  This is a multiplexing driver used to support linking CD HDLC streams.  When CD streams are
 *  linked under the driver and configured using input-output controls, the HDLC stream appears as a
 *  HDLC device to Linux.  CD streams can also be opened on the upper multiplex and attached to
 *  Linux raw HDLC devices.  This permits CD drivers to be exported to Linux and CD users to also
 *  use Linux devices not directly supported with a STREAMS driver.
 *
 *  To manage mapping of upper multiplex CD streams to Linux devices, a specialized input-output
 *  control and CD primitive is used.  These take as an argument the ascii name of the device and
 *  return a PPA (minor device number) that can be used either to open the device directly or can be
 *  used to attach a style 2 CD stream to the device.  To manage mapping of lower multiplex CD
 *  streams to Linux devices, a specialised intput-output control is used.  This input-output
 *  control takes as an argument the multiplex id and the ascii device name.  When successful, the
 *  lower multiplex streams is mapped to the Linux device of the same name.  Devices mapped in this
 *  way begin life as a raw-HDLC device but can be configured using the sethdlc utility for Linux.
 */

#define _DEBUG 1
// #undef _DEBUG

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#include <sys/os7/compat.h>

#include <stdbool.h>
#include <sys/cdi.h>

#define HDLC_DESCRIP	"HDLC STREAMS MULTIPLEXING DRIVER."
#define HDLC_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define HDLC_REVISION	"OpenSS7 $RCSfile: hdlc.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/12/20 23:07:07 $"
#define HDLC_COPYRIGHT	"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define HDLC_DEVICE	"Supports Linux HDLC devices."
#define HDLC_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define HDLC_LICENSE	"GPL"
#define HDLC_BANNER	HDLC_DESCRIP	"\n" \
			HDLC_EXTRA	"\n" \
			HDLC_REVISION	"\n" \
			HDLC_COPYRIGHT	"\n" \
			HDLC_DEVICE	"\n" \
			HDLC_CONTACT	"\n"
#define HDLC_SPLASH	HDLC_DESCRIP	" - " \
			HDLC_REVISION

#ifdef LINUX
MODULE_AUTHOR(HDLC_CONTACT);
MODULE_DESCRIPTION(HDLC_DESCRIP);
MODULE_SUPPORTED_DEVICE(HDLC_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(HDLC_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-hldc");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define HDLC_DRV_ID		CONFIG_STREAMS_HDLC_MODID
#define HDLC_DRV_NAME		CONFIG_STREAMS_HDLC_NAME
#define HDLC_CMAJORS		CONFIG_STREAMS_HDLC_NMAJORS
#define HDLC_CMAJOR_0		CONFIG_STREAMS_HDLC_MAJOR
#define HDLC_UNITS		CONFIG_STREAMS_HDLC_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_MXMUX_MODID));
MODULE_ALIAS("streams-driver-hdlc");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_MXMUX_MAJOR));
MODULE_ALIAS("/dev/streams/hdlc");
MODULE_ALIAS("/dev/streams/hdlc/*");
MODULE_ALIAS("/dev/streams/clone/hdlc");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(HDLC_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(HDLC_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(HDLC_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/hdlc");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

struct cd {
	int mid;
	int sid;
	union {
		dev_t dev;
		int index;
	};
	queue_t *oq;
	queue_t *iq;
	cd_ulong ppa;
	struct CD_info_ack info;
};

static struct module_info cd_minfo = {
	.mi_idnum = DRV_ID,		/* Driver module id number */
	.mi_idname = DRV_NAME,		/* Driver name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat cd_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit cd_rinit = {
	.qi_putp = &cd_rput,		/* Read put (message from below) */
	.qi_qopen = &cd_qopen,		/* Each open */
	.qi_qclose = &cd_qclose,	/* Last close */
	.qi_minfo = &cd_minfo,		/* Information */
	.qi_mstat = &cd_mstat,		/* Statistics */
};

static struct qinit cd_winit = {
	.qi_putp = &cd_wput,		/* Write put (message from above) */
	.qi_srvp = &cd_wsrv,		/* Write queue service */
	.qi_minfo = &cd_minfo,		/* Information */
	.qi_mstat = &cd_mstat,		/* Statistics */
};

static struct module_info hd_minfo = {
	.mi_idnum = DRV_ID,		/* Driver module id number */
	.mi_idname = DRV_NAME,		/* Driver name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat hd_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit hd_rinit = {
	.qi_putp = &hd_rput,		/* Read put (message from below) */
	.qi_srvp = &hd_rsrv,		/* Read queue service */
	.qi_minfo = &hd_minfo,		/* Information */
	.qi_mstat = &hd_mstat,		/* Statistics */
};

static struct qinit hd_winit = {
	.qi_putp = &hd_wput,		/* Write put (message from above) */
	.qi_minfo = &hd_minfo,		/* Information */
	.qi_mstat = &hd_mstat,		/* Statistics */
};

static struct streamtab hd_info = {
	.st_rdinit = &cd_rinit,		/* Upper multiplex read queue */
	.st_wrinit = &cd_winit,		/* Upper multiplex write queue */
	.st_muxrinit = &hd_rinit,	/* Lower multiplex read queue */
	.st_muxwinit = &hd_winit,	/* Lower multiplex write queue */
};

static struct cdevsw cd_cdev = {
	.d_name = DRV_NAME,		/* Driver name */
	.d_str = &hd_info,		/* Driver streamtab */
	.d_flag = D_MP,			/* Driver flags */
	.d_fop = NULL,			/* Driver operations (NULL for stream head) */
	.d_mode = S_IFCHR,		/* Device type */
	.d_kmod = THIS_MODULE,		/* Module owner */
};
static __init int
hdlcinit(void)
{
	int err;

	if ((err = register_strdev(&cd_cdev, major)) < 0) {
		strlog(modid, 0, 0, SL_CONSOLE | SL_FATAL,
		       "could not register STREAMS device, err = %d", err);
		return (err);
	}
	if (major == 0)
		major = err;
	return (0);
}
static __exit void
hdlcexit(void)
{
	int err;

	if ((err = unregsiter_strdev(&cd_cdev, major)) < 0)
		strlog(modid, 0, 0, SL_CONSOLE | SL_FATAL,
		       "could not register STREAMS device, err = %d", err);
	return;
}

module_init(hdlcinit);
module_exit(hdlcexit);
