/*****************************************************************************

 @(#) $RCSfile: matrix.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-11-26 13:13:55 $

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

 Last Modified $Date: 2008-11-26 13:13:55 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: matrix.c,v $
 Revision 0.9.2.1  2008-11-26 13:13:55  brian
 - sync with working copy

 *****************************************************************************/

#ident "@(#) $RCSfile: matrix.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-11-26 13:13:55 $"

static char const ident[] = "$RCSfile: matrix.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-11-26 13:13:55 $";

/*
 * This is the MATRIX multiplexing driver.  Its purpose is to allow a single
 * device, /dev/streams/matrix, to represent all multiplex and channel device
 * streams in the system.  The Matrix SNMP (mxMIB) configuration script opens
 * the appropriate device drivers (MX and CH) and links (I_PLINKs) them under
 * this multiplexer to form a single view of the device drivers.  It is then
 * possible to open a single MX or CH upper streams on this multiplexer and
 * either use it directly or link it, with appropriate modules pushed, under
 * another multiplexing device driver.  Examples for MX streams would be
 * linking them under the MG multiplexer or ZAPTEL multiplexer.  Examples for CH
 * stream could be the X.25 PLP with HLDC and LAPB modules pushed, or the MTP
 * driver with SDT and SL moudles pushed.
 *
 * Using MX or CH pipe moudles, it is possible to open a STREAMS based pipe,
 * push the mx-pmod or ch-pmod module on one end of the pipe and then link both
 * ends benath this multiplexer.  This permits testing of the multiplexer as
 * well as providing an internal switching matrix capability.
 *
 * The driver also provides pseudo-digital cross-connect capabilities within
 * lower MX streams, between lower MX streams, between lower CH streams, or
 * between CH streams and MX streams.  This cross-conect can be performe on a
 * dynamic basis (switching), a semi-permanent basis (DCCS), or a permanent
 * basis (channel bank).
 *
 * Lower MX and CH streams are always treated as Style 1 Streams.  That is, if
 * the lower Stream is in fact a Style 2 Stream, it must be attached to a PPA
 * before it is linked beneath the multiplexing driver.  Clone upper CH and MX
 * Streams are always treated as Style 2 Streams.  PPA addressing for the upper
 * CH and MX Streams is performed by using a globally assigned PPA or CLEI that
 * is assigned to a lower Stream once it is linked beneath the multiplexing
 * driver.  Adddresses for CH Streams are derived from the PPA and CLEI for
 * lower Streams and the slot map used in the enable and connect primitives is
 * used to select the channel(s) from the lower MX Stream.  Monitoring can be
 * accomplished, even for pseudo-digital cross-connect channels, by enabling and
 * connecting the upper CH stream in monitoring mode.
 *
 * Whether an upper stream is a CH stream or an MX stream is selected using the
 * minor device number of the opened Stream.  Upper CH and MX streams can be
 * clone streams or non-clone streams.  Clone streams are Style 2 streams that
 * can be attache to any available PPA.  Non-clone streams have a minor device
 * number that maps to a global PPA or CLEI of a lower Stream (the mapping is
 * assigned when the Stream is I_PLINKED beneath the multiplexing driver).
 * These are Style 1 MX or CH streams.  Only individual channel or full span CH
 * streams are available as Style 1 streams.  Fractional CH streams must utilize
 * the clone CH device number and attach to the appropriate fraction of the
 * span.
 *
 * The primary reason that the MX and CH portion of the switching matrix are
 * combined into one multiplexing device driver is to support the switching of
 * multi-rate ISDN and ISUP calls, where multiple 64 kbps channels are ganged
 * together to provide the mulitrate connection.
 *
 * Note that the only time that it is necessary to pass channel data to and from
 * the driver using an upper CH Stream is for data or voice connections that are
 * terminated on the host.  For example, X.25 connections that are terminated on
 * this host open a CH stream and push appropriate HDLC and LAPB modules and
 * link the resulting stream under the X.25 PLP driver.  Similarly, for SS7
 * MTP2, this host opens a CH stream and pushes an SDT and SL module and links
 * the resulting stream under the MTP driver.  A voice example is where a CH
 * stream is opened and attached to a channel and an application plays an
 * announcement or one into the connection.
 *
 * Note that this switching matrix is not capable of performing channel
 * conversion that is not provided by the lower CH or MX streams themselves.
 * All pseudo-digital cross-connect is performed clear channel and consists of
 * simple full or half duplex connections.  At a later date, one exception to
 * this might be the non-intensive mu-law to A-law conversion.
 *
 * To peferom dynamic media conversion, conferencing, or other media conversion
 * functions, MX and CH upper streams on this driver (or their corresponding
 * underlying devices) should be linked beneath the MG driver.  However, static
 * media conversion can also be performed using a module or the device driver
 * directly.  For example, VT over IP or RTP streams can have conversion modules
 * pushed to provide an MX or CH interface that can then be linked directly
 * underneath this multiplexing driver.  For example, the RTP stream then can
 * convert between the codec in use on the RTP stream and either mu-law or A-law
 * PCM presented to this driver as a CH stream.  To facilitate this, the upper
 * management CH stream an pass whatever input-output controls it wishes to a
 * managed lower CH stream.  For example, this could be used to set the codec
 * used by a lower RTP stream before it is enabled.  The same is true for the MX
 * management stream.
 */

#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/chi.h>
#include <ss7/chi_ioctl.h>
#include <ss7/mxi.h>
#include <ss7/mxi_ioctl.h>

#define MATRIX_DESCRIP		"MATRIX (MX) STREAMS MULTIPLEXING DRIVER."
#define MATRIX_REVISION		"OpenSS7 $RCSfile: matrix.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-11-26 13:13:55 $"
#define MATRIX_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define MATRIX_DEVICE		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define MATRIX_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define MATRIX_LICENSE		"GPL"
#define MATRIX_BANNER		MATRIX_DESCRIP		"\n" \
				MATRIX_REVISION		"\n" \
				MATRIX_COPYRIGHT	"\n" \
				MATRIX_DEVICE		"\n" \
				MATRIX_CONTACT		"\n"
#define MATRIX_SPLASH		MATRIX_DESCRIP		" - " \
				MATRIX_REVISION

#if defined LINUX

MODULE_AUTHOR(MATRIX_CONTACT);
MODULE_DESCRIPTION(MATRIX_DESCRIP);
MODULE_SUPPORTED_DEVICE(MATRIX_DEVICE);
#if defined MODULE_LICENSE
MODULE_LICENSE(MATRIX_LICENSE);
#endif				/* defined MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-matrix");
#endif				/* defined MODULE_ALIAS */
#if defined MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* defined MODULE_VERSION */

#endif				/* defined LINUX */

#if defined LFS
#define MATRIX_DRV_ID		CONFIG_STREAMS_MATRIX_MODID
#define MATRIX_DRV_NAME		CONFIG_STREAMS_MATRIX_NAME
#define MATRIX_CMAJORS		CONFIG_STREAMS_MATRIX_NMAJORS
#define MATRIX_CMAJOR_0		CONFIG_STREAMS_MATRIX_MAJOR
#define MATRIX_UNITS		CONFIG_STREAMS_MATRIX_NMINORS
#endif				/* defined LFS */

modID_t modid = MATRIX_DRV_ID;
major_t major = MATRIX_CMAJOR_0;

#if defined LINUX

#if defined module_param
module_param(modid, ushort, 0444);
module_param(major, uint, 0444);
#else				/* defined module_param */
MODULE_PARM(modid, "h");
MODULE_PARM(major, "d");
#endif				/* defined module_param */
MODULE_PARM_DESC(modid, "Module id number for MATRIX driver (0 for allocation).");
MODULE_PARM_DESC(major, "Major device number for MATRIX driver (0 for allocation).");

#if defined MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(MATRIX_MODID));
MODULE_ALIAS("streams-driver-matrix");
MODULE_ALIAS("streams-major-" __stringify(MATRIX_CMAJOR_0));
MODULE_ALIAS("/dev/streams/matrix");
MODULE_ALIAS("/dev/streams/matrix/mx");
MODULE_ALIAS("/dev/streams/matrix/ch");
MODULE_ALIAS("/dev/streams/matrix/admin");
MODULE_ALIAS("/dev/stremas/clone/matrix");
MODULE_ALIAS("char-major-" __stringify(MATRIX_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(MATRIX_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(MATRIX_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/matrix");
#endif				/* defined MODULE_ALIAS */

#endif				/* defined LINUX */

#define DRV_ID		MATRIX_DRV_ID
#define DRV_NAME	MATRIX_DRV_NAME
#if defined MODULE
#define DRV_BANNER	MATRIX_BANNER
#else				/* defined MODULE */
#define DRV_BANNER	MATRIX_SPLASH
#endif				/* defined MODULE */

/*
 * QUEUE OPEN AND CLOSE ROUTINES
 */

static streamscall int
ma_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	minor_t bminor = cminor;
	unsigned long flags;
	union ma *ma;
	int err;

	switch (cminor) {
	case MATRIX_CLONE_CH_MINOR:
		bminor = cminor;
		sflag = CLONEOPEN;
		break;
	case MATRIX_CLONE_MX_MINOR:
		bminor = cminor;
		sflag = CLONEOPEN;
		break;
	}
	if (sflag == MODOPEN)
		/* cannot push as module */
		return (ENXIO);
	if (q->q_ptr) {
		if (sflag == CLONEOPEN)
			return (0);
		/* FIXME: This is not a clone device and yet it is already open. Check that the
		   opening process has sufficient credential to open the non-clone matrix device. */
		return (EPERM);
	}
	if (cminor >= MATRIX_CLONE_FREE_MINOR)
		/* Cannot open clone device numbers explicitly. */
		return (ENXIO);
	if (cminor > MATRIX_CLONE_MX_CMINOR)
		/* all others are non-clone devices */
		sflag = DRVOPEN;
	if (sflag == CLONEOPEN) {
		cminor = MATRIX_CLONE_FREE_MINOR;
		*devp = makedevice(cmajor, cminor);
	}
	write_lock_irqsave(&ma_lock, flags);
	if ((err = mi_open_comm(&ma_opens, sizeof(*ma), q, devp, oflags, sflag, crp))) {
		write_unlock_irqrestore(&ma_lock, flags);
		return (err);
	}
	ma = MA_PRIV(q);
	/* initialize matrix private structure */
	ma->common.oq = q;
	ma->common.iq = WR(q);
	/* FIXME: more... */
	write_unlock_irqrestore(&ma_lock, flags);
	qprocson(q);
	return (0);
}

static streamscall int
ma_qclose(queue_t *q, int oflags, cred_t *crp)
{
	unsigned long flags;

	write_lock_irqsave(&ma_lock, flags);
	qprocsoff(q);
	mi_close_comm(&ma_opens, q);
	write_unlock_irqrestore(&ma_lock, flags);
	return (0);
}

/*
 * STREAMS DEFINITIONS
 */

static struct module_info ma_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_maxpsz = STRMAXPSZ,
	.mi_minpsz = STRMINPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat ma_rstat __attribute__((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat ma_wstat __attribute__((__aligned__(SMP_CACHE_BYTES)));

static struct qinit ma_rdinit = {
	.qi_srvp = ma_muxrsrv,
	.qi_qopen = ma_qopen,
	.qi_qclose = ma_qclose,
	.qi_minfo = &ma_minfo,
	.qi_mstat = &ma_rstat,
};
static struct qinit ma_wrinit = {
	.qi_putp = ma_wput,
	.qi_srvp = ma_wsrv,
	.qi_minfo = &ma_minfo,
	.qi_mstat = &ma_wstat,
};
static struct qinit ma_muxrinit = {
	.qi_putp = ma_rput,
	.qi_srvp = ma_rsrv,
	.qi_minfo = &ma_minfo,
	.qi_mstat = &ma_rstat,
};
static struct qinit ma_muxwinit = {
	.qi_srvp = ma_muxwsrv,
	.qi_minfo = &ma_minfo,
	.qi_mstat = &ma_wstat,
};

MODULE_STATIC struct streamtab matrixinfo = {
	.st_rdinit = &ma_rdinit,
	.st_wrinit = &ma_wrinit,
	.st_muxrinit = &ma_muxrinit,
	.st_muxwinit = &ma_muxwinit,
};

static struct cdevsw ma_cdev = {
	.d_name = DRV_NAME,
	.d_str = &matrixinfo,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

static __init int
matrixinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = register_strdev(&ma_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register STREAMS device %d, err = %d\n", DRV_NAME,
			(int) major, -err);
		return (err);
	}
	if (major == 0)
		major = err;
	return (0);
}

static int
matrixterminate(void)
{
	int err;

	if ((err = unregister_strdev(&ma_cdev, major)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister STREAMS device %d, err = %d\n", DRV_NAME,
			(int) major, -err);
		return (err);
	}
	major = 0;
	return (0);
}

static __exit void
matrixexit(void)
{
	matrixterminate();
}

module_init(matrixinit);
module_exit(matrixexit);
