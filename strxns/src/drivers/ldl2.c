/*****************************************************************************

 @(#) $RCSfile: ldl2.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2008-05-25 12:46:56 $

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

 Last Modified $Date: 2008-05-25 12:46:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ldl2.c,v $
 Revision 1.1.2.1  2008-05-25 12:46:56  brian
 - added manual pages, libraries, utilities and drivers

 *****************************************************************************/

#ident "@(#) $RCSfile: ldl2.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2008-05-25 12:46:56 $"

static char const ident[] = "$RCSfile: ldl2.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2008-05-25 12:46:56 $";

/*
 * This is a complete rewrite (reimplementation) of the Linux Data Link driver.  There are a
 * number of deficiencies with the older data link driver, not the least of which the old LDL
 * driver does not support connection-oriented data link service (DL_CODLS), such as for SDLC,
 * HDLC, LAPB, LAPF, LAPD, LAPM, LAPS, LLC2, etc.  This driver, however, does not yet support
 * acknowledged connection-less data link service (DL_ACLDLS), but something needs to be left
 * for later.
 *
 * Another difference, this driver permits the linking of CDI device streams beneath this
 * driver.  The CDI device streams can be CD_HDLC, CD_BISYNC or CD_LAN devices.  For CD_HDLC,
 * the device is one that uses HDLC synchronous framing only (it does not actually perform the
 * upper level HDLC functions such as ABM, but merely the framing, bit transparency, checksum
 * functions.  The CD_LAN device is a raw MAC device in that all frames delivered to the MAC
 * address are indicated, and all frames sent are sent directly from the MAC device.  Upper
 * layer LLC functions (regardless of type) are not performed.  There may be differnt forms of
 * CD_LAN device: some may only deliver and send encapsulated ISO 8022 LLC frames, some may
 * only deliver and send SNAP encapsulated ISO 8022 LLC frames.
 */

#define _MPS_SOURCE		/* For MPS compatibility and utility functions. */

#include <sys/os7/compat.h>	/* For STREAMS compatibility */
#include <sys/strsun.h>		/* For some SUN convenience macros. */
#include <sys/dlip.h>		/* For DLPI Revision 2.0.0 interface definitions */

#define LDL_DESCRIP	"SVR 4.2 DATA LINK DRIVER FOR LINUX FAST-STREAMS"
#define LDL_EXTRA	"Part of the OpenSS7 X/Open Network Services for Linux Fast-STREAMS."
#define LDL_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define LDL_REVISION	"OpenSS7 $RCSfile: ldl2.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2008-05-25 12:46:56 $"
#define LDL_DEVICE	"SVR 4.2MP Data Link Driver (DL) for Linux Fast-STREAMS"
#define LDL_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define LDL_LICENSE	"GPL"
#define LDL_BANNER	LDL_DESCRIP	"\n" \
			LDL_EXTRA	"\n" \
			LDL_REVISION	"\n" \
			LDL_COPYRIGHT	"\n" \
			LDL_DEVICE	"\n" \
			LDL_CONTACT
#define LDL_SPLASH	LDL_DEVICE	"\n" \
			LDL_REVISION

#ifdef LINUX
MODULE_AUTHOR(LDL_CONTACT);
MODULE_DESCRIPTION(LDL_DESCRIP);
MODULE_SUPPORTED_DEVICE(LDL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(LDL_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-ldl");
MODULE_ALIAS("streams-link-driver");
MODULE_ALIAS("streams-link-dri");
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_LDL_MODID));
MODULE_ALIAS("streams-driver-link-driver");
MODULE_ALIAS("streams-driver-link-dri");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_LDL_MAJOR));
MODULE_ALIAS("/dev/streams/link-dri");
MODULE_ALIAS("/dev/streams/link-dri/*");
MODULE_ALIAS("/dev/streams/clone/link-dri");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(LDL_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(LDL_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(LDL_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/ldl");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define LDL_DRV_ID	CONFIG_STREAMS_LDL_MODID
#define LDL_DRV_NAME	CONFIG_STREAMS_LDL_NAME
#define LDL_CMAJORS	CONFIG_STREAMS_LDL_NMAJORS
#define LDL_CMAJOR_0	CONFIG_STREAMS_LDL_MAJOR
#define LDL_UNITS	CONFIG_STREAMS_LDL_NMINORS
#endif				/* LFS */

#define DRV_ID		LDL_DRV_ID
#define DRV_NAME	LDL_DRV_NAME
#define CMAJORS		LDL_CMAJORS
#define CMAJOR_0	LDL_CMAJOR_0
#define UNITS		LDL_UNITS

#ifdef MODULE
#define DRV_BANNER	LDL_BANNER
#else				/* MODULE */
#define DRV_BANNER	LDL_SPLASH
#endif				/* MODULE */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

static struct module_info dl_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat dl_rstat __attribute__((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat dl_wstat __attribute__((__aligned__(SMP_CACHE_BYTES)));

/*
 * --------------------------------------------------------------------------
 *
 * QUEUE OPEN AND CLOSE ROUTINES
 *
 * --------------------------------------------------------------------------
 */
static caddr_t dl_opens = NULL;

static streamscall int
dl_qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	struct dl *dl;
	pl_t *pl;
	int err;
	minor_t bminor;
	mblk_t *timers[5];
	int i;

	if (q->q_ptr != NULL)
		return (0); /* already open */

	/* we alway clone open starting at LDL_CMINOR_FREE */
	bminor = getminor(*devp); /* original minor opened */
	if (bminor >= LDL_CMINOR_FREE)
		return (ENXIO);
	*devp = makedevice(getmajor(*devp), LDL_CMINOR_FREE);
	sflag = CLONEOPEN;

	/* allocate 5 timers */
	for (i = 0;i < 5; i++) {
		if (!(timers[i] = mi_timer_alloc(WR(q), sizeof(int)))) {
			while (--i)
				mi_timer_free(timers[i]);
			return (ENOBUFS);
		}
		*(int *)mp->b_wptr = i + 1;
		timers[i]->b_wptr += sizeof(int);
	}

	pl = RW_WRLOCK(&dl_lock, plstr);
	err = mi_open_comm(&dl_opens, sizeof(*dl), q, devp, oflag, sflag, crp);
	if (err) {
		RW_UNLOCK(&dl_lock, pl);
		for (i = 0; i < 5; i++)
			mi_timer_free(timers[i]);
		return (err);
	}

	bzero(dl, sizeof(*dl));
	dl->timers.t1 = timers[0];
	dl->timers.t2 = timers[1];
	dl->timers.t3 = timers[2];
	dl->timers.t4 = timers[3];
	dl->timers.t5 = timers[4];

	RW_UNLOCK(&dl_lock, pl);

	qprocson(q);
	return (0);
}

static streamscall int
dl_qclose(queue_t *q, int oflag, cred_t *crp)
{
	struct dl *dl;
	pl_t pl;
	int err;

	if (q->q_ptr == NULL)
		return (0);	/* already closed */

	qprocsoff(q);
	dl = DL_PRIV(q);
	mi_timer_free(dl->timers.t1);
	mi_timer_free(dl->timers.t2);
	mi_timer_free(dl->timers.t3);
	mi_timer_free(dl->timers.t4);
	mi_timer_free(dl->timers.t5);
	pl = RW_WRLOCK(&dl_lock, plstr);
	err = mi_close_comm(&dl_opens, q);
	RW_UNLOCK(&dl_lock, pl);
	return (err);
}


/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

static struct qinit dl_rdinit = {
	.qi_srvp = dl_rsrv,
	.qi_putp = dl_rput,
	.qi_qopen = dl_qopen,
	.qi_qclose = dl_qclose,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_rstat,
};

static struct qinit dl_wrinit = {
	.qi_srvp = dl_wsrv,
	.qi_putp = dl_wput,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_wstat,
};

static struct streamtab dl_info = {
	.st_rdinit = &dl_rdinit,
	.st_wrinit = &dl_wrinit,
};


