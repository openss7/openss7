/*****************************************************************************

 @(#) $RCSfile: zaptel.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/14 06:37:29 $

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

 Last Modified $Date: 2006/10/14 06:37:29 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: zaptel.c,v $
 Revision 0.9.2.1  2006/10/14 06:37:29  brian
 - added manpages, module, drivers, headers from strss7 package

 Revision 0.9.2.1  2006/03/20 12:23:30  brian
 - first cut of zaptel driver

 *****************************************************************************/

#ident "@(#) $RCSfile: zaptel.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/14 06:37:29 $"

static char const ident[] = "$RCSfile: zaptel.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/14 06:37:29 $";

/*
 *  This module pushes over an OpenSS7 MX Stream and provides a zaptel drvier
 *  interface to such a Stream.  This permits the approach of making the
 *  /dev/zap device an MX multiplexer device and simply autopushing this zaptel
 *  module over the MX multiplexer device Stream using autopush.  It will then
 *  be tranparent to the zaptel application (e.g. OpenPBX, Asterisk) as to
 *  whether the device opened was a Streams driver or the archaic character
 *  device driver that zaptel previously provided.
 *
 *  The other approoach is to simply provide a direct zaptel interface to the MX
 *  multiplexer.
 *
 *  Note that this module does not allow zaptel devices to be registered with
 *  the driver as that is the purpose of a driver.  There could be a zaptel-mx
 *  driver that allows zaptel devices to register with the zaptel-mx driver and
 *  then provides an MX interface that can then be linked under the MX
 *  multiplexer, or the MX multiplexer could be provided with the capability of
 *  registering zaptel device drivers.
 *
 *  So, this module is a bit of an experiment.
 */
#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/mxi.h>
#include <ss7/mxi_ioctl.h>
#include <ss7/zap.h>
#include <ss7/zap_ioctl.h>

#define ZAP_MX_DESCRIP	"ZAPTEL MULTIPLEX (ZAP-MX) STREAMS MODULE."
#define ZAP_MX_REVISION	"LfS $RCSfile: zaptel.c,v $ $Name:  $ ($Revision: 0.9.2.1 $) $Date: 2006/10/14 06:37:29 $"
#define ZAP_MX_COPYRIGHT    "Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define ZAP_MX_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS"
#define ZAP_MX_CONTACT	"Brian Biduock <bidulock@openss7.org>"
#define ZAP_MX_LICENCE	"GPL"
#define ZAP_MX_BANNER	ZAP_MX_DESCRIP		"\n" \
			ZAP_MX_REVISION		"\n" \
			ZAP_MX_COPYRIGHT	"\n" \
			ZAP_MX_DEVICE		"\n" \
			ZAP_MX_CONTACT		"\n"
#define ZAP_MX_SPLASH	ZAP_MX_DESCRIP		" - " \
			ZAP_MX_REVISION		"\n"

#ifdef LINUX
MODULE_AUTHOR(ZAP_MX_CONTACT);
MODULE_DESCRIPTION(ZAP_MX_DESCRIP);
MODULE_SUPPORTED_DEVICE(ZAP_MX_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(ZAP_MX_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-zap_mx");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define ZAP_MX_MOD_ID		CONFIG_STREAMS_ZAP_MX_MODID
#define ZAP_MX_MOD_NAME		CONFIG_STREAMS_ZAP_MX_NAME
#endif				/* LFS */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define MOD_ID		ZAP_MX_MOD_ID
#define MOD_NAME	ZAP_MX_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	ZAP_MX_BANNER
#else				/* MODULE */
#define MOD_BANNER	ZAP_MX_SPLASH
#endif				/* MODULE */

static struct module_info zap_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module ID name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1,			/* Hi water mark */
	.mi_lowat = 0,			/* Lo water mark */
};

STATIC streamscall int zap_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int zap_close(queue_t *, int, cred_t *);

STATIC struct qinit zap_rinit = {
	.qi_putp = ss7_oput,		/* Read put (message from below) */
	.qi_qopen = zap_open,		/* Each open */
	.qi_qclose = zap_close,		/* Last close */
	.qi_minfo = &zap_minfo,		/* Information */
};

STATIC struct qinit zap_winit = {
	.qi_putp = ss7_iput,		/* Write put (message from above) */
	.qi_minfo = &zap_minfo,		/* Information */
};

STATIC struct streamtab zap_mxinfo = {
	.st_rdinit = &zap_rinit,	/* Upper read queue */
	.st_wrinit = &zap_winit,	/* Upper write queue */
};

/*
 *  =========================================================================
 *
 *  Private Data Structures
 *
 *  =========================================================================
 */

/*
 * zaptel structure
 */
typedef struct zap {
	STR_DECLARATION (struct zap);	/* stream declaration */
	struct zap_config config;	/* configuration */
	struct zap_stats statsp;	/* stats periods */
	struct zap_stats stats;		/* statistics */
	struct zap_notify notify;	/* notifications */
} zap_t;

#define ZAP_PRIV(__q) ((struct zap *)(__q)->q_ptr)

struct zap_config zap_default = {
	.block_size = 64,		/* 64 bits per block */
	.encoding = MX_ENCODING_NONE,	/* sample block encoding */
	.sample_size = 8,		/* sample size */
	.rate = 8000,			/* clock rate */
	.tx_channels = 1,		/* tx channels */
	.rx_channels = 1,		/* rx channels */
	.opt_flags = MX_PARM_OPT_CLRCH,	/* option flags */
};

STATIC struct zap *zap_opens = NULL;

STATIC struct zap *zap_alloc_priv(queue_t *, struct zap **, dev_t *, cred_t *);
STATIC struct zap *zap_get(struct zap *);
STATIC void zap_put(struct zap *);
STATIC void zap_free_priv(queue_t *);

/*
 *  =========================================================================
 *
 *  PRIMITIVES
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives sent upstream
 *
 *  -------------------------------------------------------------------------
 *  Because we a emulating a legacy character device, we do not send primitives
 *  upstream.
 */

/**
 *  m_error: - send an M_ERROR message upstream if necessary
 *  @q: a queue in the stream
 *  @zap: stream private structure
 *  @error: error number (if any)
 */
STATIC int
m_error(queue_t *q, struct zap *zap, int error)
{
	mblk_t *mp;
	int hangup = 0;

	if (error < 0)
		error = -error;
	switch (error) {
	case EBUSY:
	case ENOBUFS:
	case ENOMEM:
	case EAGAIN:
		return (-error);
	case EPIPE:
	case ENETDOWN:
	case EHOSTUNREACH:
		hangup = 1;
	}
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		if (hangup) {
			mp->b_datap->db_type = M_HANGUP;
			zap->state = MXS_UNUSABLE;
			printd(("%s: %p: <- M_HANGUP\n", MOD_NAME, zap));
			putnext(zap->oq, mp);
			return (-error);
		} else {
			mp->b_datap->db_type = M_ERROR;
			*mp->bwptr++ = (error < 0) ? -error : error;
			*mp->bwptr++ = (error < 0) ? -error : error;
			zap->state = MXS_UNUSABLE;
			printd(("%s: %p: <- M_ERROR\n", MOD_NAME, zap));
			putnext(zap->oq, mp);
			return (QR_DONE);
		}
	}
	rare();
	return (-ENOBUFS);
};

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitive sent downstream
 *
 *  -------------------------------------------------------------------------
 *  Because we a emulating a legacy character device we do not send primitives
 *  downstream.
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives from below
 *
 *  -------------------------------------------------------------------------
 */
