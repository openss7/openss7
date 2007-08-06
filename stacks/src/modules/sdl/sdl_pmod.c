/*****************************************************************************

 @(#) $RCSfile: sdl_pmod.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/08/06 04:43:53 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2007/08/06 04:43:53 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sdl_pmod.c,v $
 Revision 0.9.2.4  2007/08/06 04:43:53  brian
 - rework of pipe-based emulation modules

 Revision 0.9.2.3  2007/08/03 13:35:39  brian
 - manual updates, put ss7 modules in public release

 Revision 0.9.2.2  2007/07/21 20:09:51  brian
 - added pass structure

 Revision 0.9.2.1  2007/07/14 01:13:34  brian
 - added new files

 *****************************************************************************/

#ident "@(#) $RCSfile: sdl_pmod.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/08/06 04:43:53 $"

static char const ident[] =
    "$RCSfile: sdl_pmod.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/08/06 04:43:53 $";

/*
 *  This is a module that can be pushed over one end of a STREAMS-based pipe to form a simulation of
 *  a pair of connected signalling data links.  Each end of the pipes presents the SDLI (Signalling
 *  Data Link Interface) and each end appears to be a completely independent signalling link
 *  termination.
 *
 *  There are two primary purposes for this module: 1) testing of the signalling data terminal
 *  module that can be pushed onto one end of the pipe and the normal SS7 signalling test case
 *  executable approach can be performed to validate the signalling link module; and 2) to provide a
 *  pseudo signalling link that can be used to form internal connections between signalling points
 *  for testing MTP level 3 and forming virtual SS7 networks.
 *
 *  This is an SDL pipemod driver for testing and use with pipes.  This module is pushed on one side
 *  of the pipe (after pipemod, or alone) to make a pipe appear as a directly connected pair of SDL
 *  drivers.
 */

#define _LFS_SOURCE 1
#define _SVR4_SOURCE 1
#define _MPS_SOURCE 1
#define _SUN_SOURCE 1

#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>

/* don't really want the SUN version of these */
#undef freezestr
#undef unfreezestr

#define SDL_DESCRIP	"SS7/SDL: (Signalling Data Link) STREAMS PIPE MODULE."
#define SDL_REVISION	"OpenSS7 $RCSfile: sdl_pmod.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/08/06 04:43:53 $"
#define SDL_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define SDL_DEVICE	"Provides OpenSS7 SDL pipe driver."
#define SDL_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SDL_LICENSE	"GPL v2"
#define SDL_BANNER	SDL_DESCRIP	"\n" \
			SDL_REVISION	"\n" \
			SDL_COPYRIGHT	"\n" \
			SDL_DEVICE	"\n" \
			SDL_CONTACT	"\n"
#define SDL_SPLASH	SDL_DEVICE	" - " \
			SDL_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(SDL_CONTACT);
MODULE_DESCRIPTION(SDL_DESCRIP);
MODULE_SUPPORTED_DEVICE(SDL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SDL_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sdl-pmod");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define SDL_PMOD_MOD_ID		CONFIG_STREAMS_SDL_PMOD_MODID
#define SDL_PMOD_MOD_NAME	CONFIG_STREAMS_SDL_PMOD_NAME
#endif				/* LFS */

#ifndef SDL_PMOD_MOD_NAME
#define SDL_PMOD_MOD_NAME "sdl-pmod"
#endif				/* SDL_PMOD_MOD_NAME */

#ifndef SDL_PMOD_MOD_ID
#define SDL_PMOD_MOD_ID	0
#endif				/* SDL_PMOD_MOD_ID */

/*
 *  =========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  =========================================================================
 */

#define MOD_ID		SDL_PMOD_MOD_ID
#define MOD_NAME	SDL_PMOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SDL_BANNER
#else				/* MODULE */
#define MOD_BANNER	SDL_SPLASH
#endif				/* MODULE */

static struct module_info sdl_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat sdl_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sdl_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  =========================================================================
 *
 *  PRIVATE STRUCTURE
 *
 *  =========================================================================
 */

struct st {
	int l_state;			/* local management state */
	int i_state;			/* interface state */
	int i_flags;			/* interface flags */
};

struct sdl_pair;

struct sdl {
	struct sdl_pair *pair;		/* thread pointer to pair */
	struct sdl *other;		/* other private structure in pair */
	queue_t *oq;			/* output queue for this side */
	uint max_sdu;			/* maximum SDU size */
	uint min_sdu;			/* minimum SDU size */
	uint header_len;		/* size of Level 1 header */
	uint ppa_style;			/* PPA style */
	struct st state, oldstate;
	uint hlen;			/* level 2 header length */
	uint mlen;			/* mask for length indicator */
	mblk_t *tick;
	unsigned long timestamp;	/* tick interval timestamp */
	unsigned long bytecount;	/* count of bytes so far */
	unsigned long threshold;	/* threshould count for interval */
	toid_t ticktimer;		/* tick timer */
	struct lmi_option option;	/* LMI protocol and variant options */
	struct {
		struct sdl_config config;	/* SDL configuration options */
		struct sdl_notify notify;	/* SDL notification options */
		struct sdl_statem statem;	/* SDL state machine variables */
		struct sdl_stats statsp;	/* SDL statistics periods */
		struct sdl_stats stats;	/* SDL statistics */
	} sdl;
};

struct sdl_pair {
	struct sdl r_priv;
	struct sdl w_priv;
};

#define PRIV(q)		((struct sdl_pair *)(q)->q_ptr)
#define SDL_PRIV(q)	(((q)->q_flag & QREADR) ? &PRIV(q)->r_priv : &PRIV(q)->w_priv)

#define STRLOGNO	0	/* log Stream errors */
#define STRLOGIO	1	/* log Stream input-output */
#define STRLOGST	2	/* log Stream state transitions */
#define STRLOGTO	3	/* log Stream timeouts */
#define STRLOGRX	4	/* log Stream primitives received */
#define STRLOGTX	5	/* log Stream primitives issued */
#define STRLOGTE	6	/* log Stream timer events */
#define STRLOGDA	7	/* log Stream data */

/**
 * sdl_iocname: display SDT ioctl command name
 * @cmd: ioctl command
 */
static const char *
sdl_iocname(int cmd)
{
	switch (_IOC_TYPE(cmd)) {
#if 0
	case SL_IOC_MAGIC:
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(SL_IOCGCONFIG):
			return ("SL_IOCGCONFIG");
		case _IOC_NR(SL_IOCSCONFIG):
			return ("SL_IOCSCONFIG");
		case _IOC_NR(SL_IOCTCONFIG):
			return ("SL_IOCTCONFIG");
		case _IOC_NR(SL_IOCCCONFIG):
			return ("SL_IOCCCONFIG");
		case _IOC_NR(SL_IOCGSTATEM):
			return ("SL_IOCGSTATEM");
		case _IOC_NR(SL_IOCCMRESET):
			return ("SL_IOCCMRESET");
		case _IOC_NR(SL_IOCGSTATSP):
			return ("SL_IOCGSTATSP");
		case _IOC_NR(SL_IOCSSTATSP):
			return ("SL_IOCSSTATSP");
		case _IOC_NR(SL_IOCGSTATS):
			return ("SL_IOCGSTATS");
		case _IOC_NR(SL_IOCCSTATS):
			return ("SL_IOCCSTATS");
		case _IOC_NR(SL_IOCGNOTIFY):
			return ("SL_IOCGNOTIFY");
		case _IOC_NR(SL_IOCSNOTIFY):
			return ("SL_IOCSNOTIFY");
		case _IOC_NR(SL_IOCCNOTIFY):
			return ("SL_IOCCNOTIFY");
		default:
			return ("(unknown ioctl)");
		}
#endif
#if 0
	case SDT_IOC_MAGIC:
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(SDT_IOCGCONFIG):
			return ("SDT_IOCGCONFIG");
		case _IOC_NR(SDT_IOCSCONFIG):
			return ("SDT_IOCSCONFIG");
		case _IOC_NR(SDT_IOCTCONFIG):
			return ("SDT_IOCTCONFIG");
		case _IOC_NR(SDT_IOCCCONFIG):
			return ("SDT_IOCCCONFIG");
		case _IOC_NR(SDT_IOCGSTATEM):
			return ("SDT_IOCGSTATEM");
		case _IOC_NR(SDT_IOCCMRESET):
			return ("SDT_IOCCMRESET");
		case _IOC_NR(SDT_IOCGSTATSP):
			return ("SDT_IOCGSTATSP");
		case _IOC_NR(SDT_IOCSSTATSP):
			return ("SDT_IOCSSTATSP");
		case _IOC_NR(SDT_IOCGSTATS):
			return ("SDT_IOCGSTATS");
		case _IOC_NR(SDT_IOCCSTATS):
			return ("SDT_IOCCSTATS");
		case _IOC_NR(SDT_IOCGNOTIFY):
			return ("SDT_IOCGNOTIFY");
		case _IOC_NR(SDT_IOCSNOTIFY):
			return ("SDT_IOCSNOTIFY");
		case _IOC_NR(SDT_IOCCNOTIFY):
			return ("SDT_IOCCNOTIFY");
		default:
			return ("(unknown ioctl)");
		}
#endif
	case SDL_IOC_MAGIC:
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(SDL_IOCGCONFIG):
			return ("SDL_IOCGCONFIG");
		case _IOC_NR(SDL_IOCSCONFIG):
			return ("SDL_IOCSCONFIG");
		case _IOC_NR(SDL_IOCTCONFIG):
			return ("SDL_IOCTCONFIG");
		case _IOC_NR(SDL_IOCCCONFIG):
			return ("SDL_IOCCCONFIG");
		case _IOC_NR(SDL_IOCGSTATEM):
			return ("SDL_IOCGSTATEM");
		case _IOC_NR(SDL_IOCCMRESET):
			return ("SDL_IOCCMRESET");
		case _IOC_NR(SDL_IOCGSTATSP):
			return ("SDL_IOCGSTATSP");
		case _IOC_NR(SDL_IOCSSTATSP):
			return ("SDL_IOCSSTATSP");
		case _IOC_NR(SDL_IOCGSTATS):
			return ("SDL_IOCGSTATS");
		case _IOC_NR(SDL_IOCCSTATS):
			return ("SDL_IOCCSTATS");
		case _IOC_NR(SDL_IOCGNOTIFY):
			return ("SDL_IOCGNOTIFY");
		case _IOC_NR(SDL_IOCSNOTIFY):
			return ("SDL_IOCSNOTIFY");
		case _IOC_NR(SDL_IOCCNOTIFY):
			return ("SDL_IOCCNOTIFY");
		default:
			return ("(unknown ioctl)");
		}
	case LMI_IOC_MAGIC:
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(LMI_IOCGOPTIONS):
			return ("LMI_IOCGOPTIONS");
		case _IOC_NR(LMI_IOCSOPTIONS):
			return ("LMI_IOCSOPTIONS");
		default:
			return ("(unknown ioctl)");
		}
	default:
		return ("(unknown ioctl)");
	}
}

/**
 * sdl_primname: display SDT primitive name
 * @prim: the primtitive to display
 */
static const char *
sdl_primname(lmi_ulong prim)
{
	switch (prim) {
	case LMI_INFO_REQ:
		return ("LMI_INFO_REQ");
	case LMI_ATTACH_REQ:
		return ("LMI_ATTACH_REQ");
	case LMI_DETACH_REQ:
		return ("LMI_DETACH_REQ");
	case LMI_ENABLE_REQ:
		return ("LMI_ENABLE_REQ");
	case LMI_DISABLE_REQ:
		return ("LMI_DISABLE_REQ");
	case LMI_OPTMGMT_REQ:
		return ("LMI_OPTMGMT_REQ");
	case LMI_INFO_ACK:
		return ("LMI_INFO_ACK");
	case LMI_OK_ACK:
		return ("LMI_OK_ACK");
	case LMI_ERROR_ACK:
		return ("LMI_ERROR_ACK");
	case LMI_ENABLE_CON:
		return ("LMI_ENABLE_CON");
	case LMI_DISABLE_CON:
		return ("LMI_DISABLE_CON");
	case LMI_OPTMGMT_ACK:
		return ("LMI_OPTMGMT_ACK");
	case LMI_ERROR_IND:
		return ("LMI_ERROR_IND");
	case LMI_STATS_IND:
		return ("LMI_STATS_IND");
	case LMI_EVENT_IND:
		return ("LMI_EVENT_IND");
#if 1
	case SDL_BITS_FOR_TRANSMISSION_REQ:
		return ("SDL_BITS_FOR_TRANSMISSION_REQ");
	case SDL_CONNECT_REQ:
		return ("SDL_CONNECT_REQ");
	case SDL_DISCONNECT_REQ:
		return ("SDL_DISCONNECT_REQ");
	case SDL_RECEIVED_BITS_IND:
		return ("SDL_RECEIVED_BITS_IND");
	case SDL_DISCONNECT_IND:
		return ("SDL_DISCONNECT_IND");
#endif
#if 0
	case SDT_DAEDT_TRANSMISSION_REQ:
		return ("SDT_DAEDT_TRANSMISSION_REQ");
	case SDT_DAEDT_START_REQ:
		return ("SDT_DAEDT_START_REQ");
	case SDT_DAEDR_START_REQ:
		return ("SDT_DAEDR_START_REQ");
	case SDT_AERM_START_REQ:
		return ("SDT_AERM_START_REQ");
	case SDT_AERM_STOP_REQ:
		return ("SDT_AERM_STOP_REQ");
	case SDT_AERM_SET_TI_TO_TIN_REQ:
		return ("SDT_AERM_SET_TI_TO_TIN_REQ");
	case SDT_AERM_SET_TI_TO_TIE_REQ:
		return ("SDT_AERM_SET_TI_TO_TIE_REQ");
	case SDT_SUERM_START_REQ:
		return ("SDT_SUERM_START_REQ");
	case SDT_SUERM_STOP_REQ:
		return ("SDT_SUERM_STOP_REQ");
	case SDT_RC_SIGNAL_UNIT_IND:
		return ("SDT_RC_SIGNAL_UNIT_IND");
	case SDT_RC_CONGESTION_ACCEPT_IND:
		return ("SDT_RC_CONGESTION_ACCEPT_IND");
	case SDT_RC_CONGESTION_DISCARD_IND:
		return ("SDT_RC_CONGESTION_DISCARD_IND");
	case SDT_RC_NO_CONGESTION_IND:
		return ("SDT_RC_NO_CONGESTION_IND");
	case SDT_IAC_CORRECT_SU_IND:
		return ("SDT_IAC_CORRECT_SU_IND");
	case SDT_IAC_ABORT_PROVING_IND:
		return ("SDT_IAC_ABORT_PROVING_IND");
	case SDT_LSC_LINK_FAILURE_IND:
		return ("SDT_LSC_LINK_FAILURE_IND");
	case SDT_TXC_TRANSMISSION_REQUEST_IND:
		return ("SDT_TXC_TRANSMISSION_REQUEST_IND");
#endif
#if 0
	case SL_PDU_REQ:
		return ("SL_PDU_REQ");
	case SL_EMERGENCY_REQ:
		return ("SL_EMERGENCY_REQ");
	case SL_EMERGENCY_CEASES_REQ:
		return ("SL_EMERGENCY_CEASES_REQ");
	case SL_START_REQ:
		return ("SL_START_REQ");
	case SL_STOP_REQ:
		return ("SL_STOP_REQ");
	case SL_RETRIEVE_BSNT_REQ:
		return ("SL_RETRIEVE_BSNT_REQ");
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		return ("SL_RETRIEVAL_REQUEST_AND_FSNC_REQ");
	case SL_CLEAR_BUFFERS_REQ:
		return ("SL_CLEAR_BUFFERS_REQ");
	case SL_CLEAR_RTB_REQ:
		return ("SL_CLEAR_RTB_REQ");
	case SL_CONTINUE_REQ:
		return ("SL_CONTINUE_REQ");
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		return ("SL_LOCAL_PROCESSOR_OUTAGE_REQ");
	case SL_RESUME_REQ:
		return ("SL_RESUME_REQ");
	case SL_CONGESTION_DISCARD_REQ:
		return ("SL_CONGESTION_DISCARD_REQ");
	case SL_CONGESTION_ACCEPT_REQ:
		return ("SL_CONGESTION_ACCEPT_REQ");
	case SL_NO_CONGESTION_REQ:
		return ("SL_NO_CONGESTION_REQ");
	case SL_POWER_ON_REQ:
		return ("SL_POWER_ON_REQ");
	case SL_OPTMGMT_REQ:
		return ("SL_OPTMGMT_REQ");
	case SL_NOTIFY_REQ:
		return ("SL_NOTIFY_REQ");
	case SL_PDU_IND:
		return ("SL_PDU_IND");
	case SL_LINK_CONGESTED_IND:
		return ("SL_LINK_CONGESTED_IND");
	case SL_LINK_CONGESTION_CEASED_IND:
		return ("SL_LINK_CONGESTION_CEASED_IND");
	case SL_RETRIEVED_MESSAGE_IND:
		return ("SL_RETRIEVED_MESSAGE_IND");
	case SL_RETRIEVAL_COMPLETE_IND:
		return ("SL_RETRIEVAL_COMPLETE_IND");
	case SL_RB_CLEARED_IND:
		return ("SL_RB_CLEARED_IND");
	case SL_BSNT_IND:
		return ("SL_BSNT_IND");
	case SL_IN_SERVICE_IND:
		return ("SL_IN_SERVICE_IND");
	case SL_OUT_OF_SERVICE_IND:
		return ("SL_OUT_OF_SERVICE_IND");
	case SL_REMOTE_PROCESSOR_OUTAGE_IND:
		return ("SL_REMOTE_PROCESSOR_OUTAGE_IND");
	case SL_REMOTE_PROCESSOR_RECOVERED_IND:
		return ("SL_REMOTE_PROCESSOR_RECOVERED_IND");
	case SL_RTB_CLEARED_IND:
		return ("SL_RTB_CLEARED_IND");
	case SL_RETRIEVAL_NOT_POSSIBLE_IND:
		return ("SL_RETRIEVAL_NOT_POSSIBLE_IND");
	case SL_BSNT_NOT_RETRIEVABLE_IND:
		return ("SL_BSNT_NOT_RETRIEVABLE_IND");
	case SL_OPTMGMT_ACK:
		return ("SL_OPTMGMT_ACK");
	case SL_NOTIFY_IND:
		return ("SL_NOTIFY_IND");
	case SL_LOCAL_PROCESSOR_OUTAGE_IND:
		return ("SL_LOCAL_PROCESSOR_OUTAGE_IND");
	case SL_LOCAL_PROCESSOR_RECOVERED_IND:
		return ("SL_LOCAL_PROCESSOR_RECOVERED_IND");
#endif
	default:
		return ("(unknown primitive)");
	}
}

/**
 * sdl_statename: display LMI state name
 * @state: the state value to display
 */
static const char *
sdl_statename(int state)
{
	switch (state) {
	case LMI_UNATTACHED:
		return ("LMI_UNATTACHED");
	case LMI_ATTACH_PENDING:
		return ("LMI_ATTACH_PENDING");
	case LMI_UNUSABLE:
		return ("LMI_UNUSABLE");
	case LMI_DISABLED:
		return ("LMI_DISABLED");
	case LMI_ENABLE_PENDING:
		return ("LMI_ENABLE_PENDING");
	case LMI_ENABLED:
		return ("LMI_ENABLED");
	case LMI_DISABLE_PENDING:
		return ("LMI_DISABLE_PENDING");
	case LMI_DETACH_PENDING:
		return ("LMI_DETACH_PENDING");
	default:
		return ("LMI_????");
	}
}

/**
 * sdl_get_l_state: - get management state for private structure
 * @sdl: private structure
 */
static int
sdl_get_l_state(struct sdl *sdl)
{
	return (sdl->state.l_state);
}

/**
 * sdl_set_l_state: - set management state for private structure
 * @sdl: private structure
 * @newstate: new state
 */
static int
sdl_set_l_state(struct sdl *sdl, int newstate)
{
	int oldstate = sdl->state.l_state;

	if (newstate != oldstate) {
		sdl->state.l_state = newstate;
		mi_strlog(sdl->oq, STRLOGST, SL_TRACE, "%s <- %s", sdl_statename(newstate),
			  sdl_statename(oldstate));
	}
	return (newstate);
}

static int
sdl_save_l_state(struct sdl *sdl)
{
	return ((sdl->oldstate.l_state = sdl_get_l_state(sdl)));
}

static int
sdl_restore_l_state(struct sdl *sdl)
{
	return sdl_set_l_state(sdl, sdl->oldstate.l_state);
}

/**
 * sdl_get_i_state: - get interface state for private structure
 * @sdl: private structure
 */
static int
sdl_get_i_state(struct sdl *sdl)
{
	return (sdl->state.i_state);
}

/**
 * sdl_set_i_state: - set interface state for private structure
 * @sdl: private structure
 * @newstate: new state
 */
static int
sdl_set_i_state(struct sdl *sdl, int newstate)
{
	int oldstate = sdl->state.i_state;

	if (newstate != oldstate) {
		sdl->state.i_state = newstate;
		mi_strlog(sdl->oq, STRLOGST, SL_TRACE, "%s <- %s", sdl_statename(newstate),
			  sdl_statename(oldstate));
	}
	return (newstate);
}

static int
sdl_save_i_state(struct sdl *sdl)
{
	return ((sdl->oldstate.i_state = sdl_get_i_state(sdl)));
}

static int
sdl_restore_i_state(struct sdl *sdl)
{
	return sdl_set_i_state(sdl, sdl->oldstate.i_state);
}

/**
 * sdl_save_state: - reset state for private structure
 * @sdl: private structure
 */
static void
sdl_save_state(struct sdl *sdl)
{
	sdl_save_l_state(sdl);
	sdl_save_i_state(sdl);
}

/**
 * sdl_restore_state: - reset state for private structure
 * @sdl: private structure
 */
static int
sdl_restore_state(struct sdl *sdl)
{
	sdl_restore_i_state(sdl);
	return sdl_restore_l_state(sdl);
}

/**
 * sdl_get_flags: - get flags for private structure
 * @sdl: private structure
 */
static inline int
sdl_get_flags(struct sdl *sdl)
{
	return sdl->state.i_flags;
}

/**
 * sdl_set_flags: - set flags for private structure
 * @sdl: private structure
 * @newflags: new flags
 */
static int
sdl_set_flags(struct sdl *sdl, int newflags)
{
	int oldflags = sdl->state.i_flags;

	if (newflags != oldflags) {
		sdl->state.i_flags = newflags;
	}
	return (newflags);
}

static inline int
sdl_or_flags(struct sdl *sdl, int orflags)
{
	return (sdl->state.i_flags |= orflags);
}

static inline int
sdl_nand_flags(struct sdl *sdl, int nandflags)
{
	return (sdl->state.i_flags &= ~nandflags);
}

/**
 * sdl_save_flags: - reset flags for private structure
 * @sdl: private structure
 */
static void
sdl_save_flags(struct sdl *sdl)
{
	sdl->oldstate.i_flags = sdl->state.i_flags;
}

/**
 * sdl_restore_flags: - reset flags for private structure
 * @sdl: private structure
 */
static int
sdl_restore_flags(struct sdl *sdl)
{
	return sdl_set_flags(sdl, sdl->oldstate.i_flags);
}

static void
sdl_save_total_state(struct sdl *sdl)
{
	sdl_save_flags(sdl->other);
	sdl_save_state(sdl->other);
	sdl_save_flags(sdl);
	sdl_save_state(sdl);
}

static int
sdl_restore_total_state(struct sdl *sdl)
{
	sdl_restore_flags(sdl->other);
	sdl_restore_state(sdl->other);
	sdl_restore_flags(sdl);
	return sdl_restore_state(sdl);
}

/*
 *  =========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  SDL Provider -> SDL User primitives.
 *
 *  =========================================================================
 */
#if 0
/**
 * m_error: - issue M_ERROR for stream
 * @sdl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @err: error to indicate
 */
static int
m_error(struct sdl *sdl, queue_t *q, mblk_t *msg, int err)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		freemsg(msg);
		mi_strlog(s->oq, 0, SL_ERROR, "<- M_ERROR %d", err);
		putnext(s->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/**
 * lmi_info_ack: - issue LMI_INFO_ACK primitive
 * @s: private structure
 * @q: active queue (write queue)
 * @msg: message to free upon success
 */
static int
lmi_info_ack(struct sdl *s, queue_t *q, mblk_t *msg)
{
	lmi_info_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = sdl_get_l_state(s);
		p->lmi_max_sdu = s->max_sdu;
		p->lmi_min_sdu = s->min_sdu;
		p->lmi_header_len = s->header_len;
		p->lmi_ppa_style = LMI_STYLE1;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- LMI_INFO_ACK");
		putnext(s->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_ok_ack: - issue LMI_OK_ACK primitive
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static int
lmi_ok_ack(struct sdl *s, queue_t *q, mblk_t *msg, int prim)
{
	lmi_ok_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (sdl_get_l_state(s)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = sdl_set_l_state(s, LMI_DISABLED);
			break;
		case LMI_DETACH_PENDING:
			p->lmi_state = sdl_set_l_state(s, LMI_UNATTACHED);
			break;
		default:
			/* FIXME: log error */
			p->lmi_state = sdl_get_l_state(s);
			break;
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- LMI_OK_ACK");
		putnext(s->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_error_ack: - issue LMI_ERROR_ACK primitive
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @error: error number
 */
static noinline fastcall __unlikely int
lmi_error_ack(struct sdl *s, queue_t *q, mblk_t *msg, int prim, int error)
{
	lmi_error_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = error < 0 ? LMI_SYSERR : error;
		p->lmi_reason = error < 0 ? -error : 0;
		p->lmi_error_primitive = prim;
		p->lmi_state = sdl_restore_l_state(s);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- LMI_ERROR_ACK");
		putnext(s->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_error_reply: - generate error acknolwegement as required
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @error: error to return (negative, put message back) (positive, error ack)
 */
static int
lmi_error_reply(struct sdl *s, queue_t *q, mblk_t *msg, int prim, int error)
{
	if (error == 0)
		freemsg(msg);
	else if (error > 0)
		return lmi_error_ack(s, q, msg, prim, error);
	return (error);
}

/**
 * lmi_enable_con: - issue LMI_ENABLE_CON primitive
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
lmi_enable_con(struct sdl *s, queue_t *q, mblk_t *msg)
{
	lmi_enable_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = sdl_set_l_state(s, LMI_ENABLED);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- LMI_ENABLE_CON");
		putnext(s->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_disable_con: - issue LMI_DISABLE_CON primitive
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static int
lmi_disable_con(struct sdl *s, queue_t *q, mblk_t *msg)
{
	lmi_disable_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = sdl_set_l_state(s, LMI_DISABLED);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- LMI_DISABLE_CON");
		putnext(s->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_optmgmt_ack: - issue LMI_OPTMGMT_ACK primitive
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @flags: management flags
 */
static inline int
lmi_optmgmt_ack(struct sdl *s, queue_t *q, mblk_t *msg, caddr_t opt_ptr, size_t opt_len, uint flags)
{
	lmi_optmgmt_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		freemsg(msg);
		mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- LMI_OPTMGMT_ACK");
		putnext(s->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_error_ind: - issue LMI_ERROR_IND primitive
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @error: error to indicate
 * @state: state after error indication
 */
static inline int
lmi_error_ind(struct sdl *s, queue_t *q, mblk_t *msg, int error, uint state)
{
	lmi_error_ind_t *p;
	mblk_t *mp;

	if (canputnext(s->oq)) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_ERROR_IND;
			p->lmi_errno = error < 0 ? LMI_SYSERR : error;
			p->lmi_reason = error < 0 ? -error : 0;
			p->lmi_state = sdl_set_l_state(s, state);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- LMI_ERROR_IND");
			putnext(s->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * lmi_stats_ind: - issue LMI_STATS_IND primitive
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @intvl: interval
 */
static inline int
lmi_stats_ind(struct sdl *s, queue_t *q, mblk_t *msg, lmi_ulong itvl)
{
	lmi_stats_ind_t *p;
	mblk_t *mp;

	if (bcanputnext(s->oq, 1)) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_STATS_IND;
			p->lmi_interval = itvl;
			p->lmi_timestamp = drv_hztomsec(jiffies);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- LMI_STATS_IND");
			putnext(s->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * lmi_event_ind: - issue LMI_EVENT_IND primitive
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @oid: event object id
 * @level: severity
 * @inf_ptr: information pointer
 * @inf_len: information length
 */
static inline int
lmi_event_ind(struct sdl *s, queue_t *q, mblk_t *msg, uint oid, uint severity, caddr_t inf_ptr,
	      size_t inf_len)
{
	lmi_event_ind_t *p;
	mblk_t *mp;

	if (bcanputnext(s->oq, 1)) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p) + inf_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_EVENT_IND;
			p->lmi_objectid = oid;
			p->lmi_timestamp = drv_hztomsec(jiffies);
			p->lmi_severity = severity;
			mp->b_wptr += sizeof(*p);
			bcopy(inf_ptr, mp->b_wptr, inf_len);
			mp->b_wptr += inf_len;
			freemsg(msg);
			mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- LMI_EVENT_IND");
			putnext(s->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

#if 0
/**
 * sdl_received_bits_ind: - issue SDL_RECEIVED_BITS_IND upstream
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data (bits)
 */
static int
sdl_received_bits_ind(struct sdl *s, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	if (bcanputnext(q, dp->b_band)) {
		sdl_received_bits_ind_t *p;
		mblk_t *mp;

		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sdl_primitive = SDL_RECEIVED_BITS_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mp->b_cont = dp;
			mi_strlog(s->oq, STRLOGDA, SL_TRACE, "<- SDL_RECEIVED_BITS_IND");
			putnext(q, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}
#endif

#if 0
/**
 * sdl_disconnect_ind: - issue SDL_DISCONNECT_IND primitive
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 *
 * SDL_DISCONNECT_IND is generated as a result of a SDL_DISCONNECT_REQ at the
 * other end of the pipe.  This primitive is issued along the same queue as
 * the SDL_DISCONNECT_REQ was received on.
 */
static int
sdl_disconnect_ind(struct sdl *s, queue_t *q, mblk_t *msg)
{
	sdl_disconnect_ind_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdl_primitive = SDL_DISCONNECT_IND;
		freemsg(msg);
		s->sdl.statem.rx_state = SDL_STATE_IDLE;
		s->sdl.statem.tx_state = SDL_STATE_IDLE;
		s->sdl.config.ifflags &= ~(SDL_IF_UP | SDL_IF_RX_RUNNING | SDL_IF_TX_RUNNING);
		mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- SDT_DISCONNECT_IND");
		putnext(q, mp);	/* pass to other side */
		return (0);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/**
 * sdt_rc_signal_unit_ind: - generate SDT_RC_SIGNAL_UNIT_IND primitive
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data (su)
 */
static int
sdt_rc_signal_unit_ind(struct sdt *s, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	if (bcanputnext(q, dp->b_band)) {
		sdt_rc_signal_unit_ind_t *p;
		mblk_t *mp;

		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sdt_primitive = SDT_RC_SIGNAL_UNIT_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mp->b_cont = dp;
			mi_strlog(s->oq, STRLOGDA, SL_TRACE, "<- SDT_RC_SIGNAL_UNIT_IND");
			putnext(q, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sdt_rc_congestion_accept_ind: - generate SDT_RC_CONGESTION_ACCEPT_IND primitive
 * @sdt: private structure
 * @q: active queue
 * @oq: output queue
 * @msg: message to free upon success
 */
static int
sdt_rc_congestion_accept_ind(struct sdt *sdt, queue_t *q, queue_t *oq, mblk_t *msg)
{
	sdt_rc_congestion_accept_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_RC_CONGESTION_ACCEPT_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- SDT_RC_CONGESTION_ACCEPT_IND");
		putnext(oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sdt_rc_congestion_discard_ind: - generate SDT_RC_CONGESTION_DISCARD_IND primitive
 * @sdt: private structure
 * @q: active queue
 * @oq: output queue
 * @msg: message to free upon success
 */
static int
sdt_rc_congestion_discard_ind(struct sdt *sdt, queue_t *q, queue_t *oq, mblk_t *msg)
{
	sdt_rc_congestion_discard_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_RC_CONGESTION_DISCARD_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- SDT_RC_CONGESTION_DISCARD_IND");
		putnext(oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sdt_rc_no_congestion_ind: - generate SDT_RC_NO_CONGESTION_IND primitive
 * @sdt: private structure
 * @q: active queue
 * @oq: output queue
 * @msg: message to free upon success
 */
static int
sdt_rc_no_congestion_ind(struct sdt *sdt, queue_t *q, queue_t *oq, mblk_t *msg)
{
	sdt_rc_no_congestion_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_RC_NO_CONGESTION_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- SDT_RC_NO_CONGESTION_IND");
		putnext(oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/**
 * sdt_iac_correct_su_ind: - generate SDT_IAC_CORRECT_SU_IND primitive
 * @sdt: private structure
 * @q: active queue
 */
static int
sdt_iac_correct_su_ind(struct sdt *sdt, queue_t *q)
{
	sdt_iac_correct_su_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_IAC_CORRECT_SU_IND;
		mp->b_wptr += sizeof(*p);
		mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- SDT_IAC_CORRECT_SU_IND");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sdt_iac_abort_proving_ind: - generate SDT_IAC_ABORT_PROVING_IND primitive
 * @sdt: private structure
 * @q: active queue
 */
static int
sdt_iac_abort_proving_ind(struct sdt *sdt, queue_t *q)
{
	sdt_iac_abort_proving_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_IAC_ABORT_PROVING_IND;
		mp->b_wptr += sizeof(*p);
		mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- SDT_IAC_ABORT_PROVING_IND");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sdt_lsc_link_failure_ind: - generate SDT_LSC_LINK_FAILURE_IND primitive
 * @sdt: private structure
 * @q: active queue
 */
static int
sdt_lsc_link_failure_ind(struct sdt *sdt, queue_t *q)
{
	sdt_lsc_link_failure_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_LSC_LINK_FAILURE_IND;
		mp->b_wptr += sizeof(*p);
		mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- SDT_LSC_LINK_FAILURE_IND");
		putnext(q, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/**
 * sdt_txc_transmission_request_ind: - generate SDT_TXC_TRANSMISSION_REQUEST_IND primitive
 * @sdt: private structure
 * @q: active queue
 * @oq: output queue
 * @msg: message to free upon success
 */
static int
sdt_txc_transmission_request_ind(struct sdt *sdt, queue_t *q, queue_t *oq, mblk_t *msg)
{
	sdt_txc_transmission_request_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_TXC_TRANSMISSION_REQUEST_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(s->oq, STRLOGTX, SL_TRACE, "<- SDT_TXC_TRANSMISSION_REQUEST_IND");
		putnext(oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  =========================================================================
 *
 *  PROTOCOL STATE MACHINE
 *
 *  =========================================================================
 */

static inline int
sdl_attach(struct sdl *s, queue_t *q, mblk_t *msg)
{
	int err;

	sdl_set_l_state(s, LMI_ATTACH_PENDING);
	err = lmi_ok_ack(s, q, msg, LMI_ATTACH_REQ);
	return (err);
}

static inline int
sdl_enable(struct sdl *s, queue_t *q, mblk_t *msg)
{
	int err;

	sdl_set_l_state(s, LMI_ENABLE_PENDING);
	err = lmi_enable_con(s, q, msg);
	return (err);
}

static inline int
sdl_connect(struct sdl *s, queue_t *q, mblk_t *msg, int flags)
{
	s->state.i_flags |= flags;
	/* Under CHI, connect requests are confirmed.  Not under SDL. */
	freemsg(msg);
	return (0);
}

#if 1
static streamscall void
sdl_tickdone(caddr_t arg)
{
	struct sdl *sdl = (typeof(sdl)) arg;

	if (xchg(&sdl->ticktimer, 0))
		qenable(sdl->oq);
}

static inline int
sdl_layer1_bits_for_transmission(struct sdl *sdl, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	size_t bytecount;

	/* Here we do throttling. */
	/* First, check if the clock ticked over.  If the clock ticked over then clear the byte
	   count. */
	if (sdl->timestamp != jiffies) {
		sdl->timestamp = jiffies;
		sdl->bytecount = 0;
	}
	bytecount = msgdsize(dp);
	if (sdl->bytecount == 0 || sdl->bytecount + bytecount > sdl->threshold) {
		if (sdl->ticktimer)
			return (-EBUSY);
		if ((sdl->ticktimer = timeout(&sdl_tickdone, (caddr_t) sdl, 1)) != 0)
			return (-EBUSY);
	}
	if ((sdl->state.i_flags & SDL_TX_DIRECTION) && (sdl->other->state.i_flags & SDL_RX_DIRECTION)) {
		if (!bcanputnext(q, dp->b_band))
			return (-EBUSY);
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		putnext(q, dp);
		return (0);
	}
	if (msg && msg->b_cont == dp)
		msg->b_cont = NULL;
	freemsg(msg);
	freemsg(dp);
	return (0);
}
#endif

static inline int
sdl_disconnect(struct sdl *s, queue_t *q, mblk_t *msg, int flags)
{
	s->state.i_flags &= ~flags;
	/* Under CHI, disconnect requests are confirmed.  Not under SDL. */
	freemsg(msg);
	return (0);
}

static inline int
sdl_disable(struct sdl *s, queue_t *q, mblk_t *msg)
{
	int err;

	sdl_set_l_state(s, LMI_DISABLE_PENDING);
	err = lmi_disable_con(s, q, msg);
	return (err);
}

static inline int
sdl_detach(struct sdl *s, queue_t *q, mblk_t *msg)
{
	int err;

	sdl_set_l_state(s, LMI_DETACH_PENDING);
	err = lmi_ok_ack(s, q, msg, LMI_DETACH_REQ);
	return (err);
}

#if 0
/*
 *  M_DATA Handling
 *  -------------------------------------------------------------------------
 *  We need to throttle these to avoid locking up the processor.  The throttle
 *  is for 10 blocks every 10ms maximum which is precisely 64kbps.
 */
static void streamscall
sdl_w_timeout(caddr_t data)
{
	queue_t *q = (queue_t *) data;
	struct sdl *s = SDL_PRIV(q);

	if (!xchg(&s->wtim, 0))
		return;
	enableok(q);
	qenable(q);
	return;
}
static void streamscall
sdl_r_timeout(caddr_t data)
{
	queue_t *q = (queue_t *) data;
	struct sdl *s = SDL_PRIV(q);

	if (!xchg(&s->rtim, 0))
		return;
	enableok(q);
	qenable(q);
	return;
}
static INLINE int
sdl_r_data(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);

	if (s->rts == jiffies) {
		if (s->rno++ >= 10) {
			/* come back in a tick */
			s->rtim = timeout(&sdl_r_timeout, (caddr_t) q, 1);
			// printd(("sdl: rx: blocking\n"));
			return (QR_DISABLE);
		}
	} else {
		s->rno = 1;
		s->rts = jiffies;
	}
	return (QR_PASSFLOW);
}
#endif

/*
 *  =========================================================================
 *
 *  PROCESSING RECEIVED PRIMITIVES
 *
 *  SDT User -> SDT Provider primitives.
 *
 *  =========================================================================
 */

/**
 * lmi_info_req: - process LMI_INFO_REQ primitive
 * @s: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
lmi_info_req(struct sdl *s, queue_t *q, mblk_t *mp)
{
	lmi_info_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if ((err = lmi_info_ack(s, q, mp)) != 0)
		goto error;
	return (0);
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, LMI_INFO_REQ, err);
}

/**
 * lmi_attach_req: - process LMI_ATTACH_REQ primitive
 * @s: private structure
 * @q: active queue
 * @mp: the primitive
 *
 * SDT-PMOD is a Style 1 driver.  When the pipe is created and the module pushed, the
 * communications medium and physical point of attachment is implied by the stream.
 */
static int
lmi_attach_req(struct sdl *s, queue_t *q, mblk_t *mp)
{
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sdl_get_l_state(s) != LMI_UNATTACHED
	    && (sdl_get_l_state(s) != LMI_DISABLED || MBLKL(mp) == sizeof(*p)))
		goto outstate;
	/* Note that we do not need to support LMI_ATTACH_REQ because this is a style 1 drier only,
	   but if the address is null, simply move to the appropriate state. */
	if ((err = sdl_attach(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, LMI_ATTACH_REQ, err);
}

/**
 * lmi_detach_req: - process LMI_DETACH_REQ primitive
 * @s: private structure
 * @q: active queue
 * @mp: the primitive
 *
 * SDT-PMOD is a Style 1 driver.  When the pipe is created and the module pushed, the
 * communications medium and physical point of attachment is implied by the stream.
 */
static int
lmi_detach_req(struct sdl *s, queue_t *q, mblk_t *mp)
{
	lmi_detach_req_t *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sdl_get_l_state(s) != LMI_UNATTACHED && sdl_get_l_state(s) != LMI_DISABLED)
		goto outstate;
	if ((err = sdl_detach(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, LMI_DETACH_REQ, err);
}

/**
 * lmi_enable_req: - process LMI_ENABLE_REQ primitive
 * @s: private structure
 * @q: active queue
 * @mp: the primitive
 *
 * Enabling a SDT-PMOD stream is simply marking it a enabled on the appropriate side of the
 * connection and returning a confirmation primitive.
 */
static int
lmi_enable_req(struct sdl *s, queue_t *q, mblk_t *mp)
{
	lmi_enable_req_t *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sdl_get_l_state(s) != LMI_DISABLED && sdl_get_l_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdl_enable(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, LMI_ENABLE_REQ, err);
}

/**
 * lmi_disable_req: - process LMI_DISABLE_REQ primitive
 * @s: private structure
 * @q: active queue
 * @mp: the primitive
 *
 * Disabling an SDT-PMOD stream is simply marking it disabled on the appropriate side of the
 * connection and returning a confirmation primitive.
 */
static int
lmi_disable_req(struct sdl *s, queue_t *q, mblk_t *mp)
{
	lmi_disable_req_t *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sdl_get_l_state(s) != LMI_DISABLED && sdl_get_l_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdl_disable(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, LMI_DISABLE_REQ, err);
}

/**
 * lmi_optmgmt_req: - process LMI_OPTMGMT_REQ primitive
 * @s: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
lmi_optmgmt_req(struct sdl *s, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_req_t *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, LMI_OPTMGMT_REQ, err);
}

#if 1
/**
 * sdl_bits_for_transmission_req: - transmit bits
 * @s: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sdl_bits_for_transmission_req(struct sdl *s, queue_t *q, mblk_t *mp)
{
	sdl_bits_for_transmission_req_t *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sdl_get_l_state(s) != LMI_ENABLED)
		goto outstate;
	if (s->sdl.statem.tx_state == SDL_STATE_IDLE)
		goto discard;
	if ((err = sdl_layer1_bits_for_transmission(s, q, mp, mp->b_cont)) == 0) {
		freeb(mp);
		return (0);
	}
	goto error;
      discard:
	err = 0;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDL_BITS_FOR_TRANSMISSION_REQ, err);
}

/**
 * sdl_connect_req: - process SDL_CONNECT_REQ primitive
 * @s: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sdl_connect_req(struct sdl *s, queue_t *q, mblk_t *mp)
{
	sdl_connect_req_t *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sdl_get_l_state(s) != LMI_ENABLED)
		goto outstate;
	p = (typeof(p)) mp->b_rptr;
	if ((p->sdl_flags & ~(SDL_TX_DIRECTION | SDL_RX_DIRECTION)) ||
	    ((p->sdl_flags & (SDL_TX_DIRECTION | SDL_RX_DIRECTION)) == 0))
		goto badflag;
	if (((s->state.i_flags ^ p->sdl_flags) & p->sdl_flags) == 0)
		goto outstate;
	if ((err = sdl_connect(s, q, mp, p->sdl_flags)) != 0)
		goto error;
	return (0);
      badflag:
	err = LMI_UNSPEC;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDL_CONNECT_REQ, err);
}

/**
 * sdl_disconnect_req: - process SDL_DISCONNECT_REQ primitive
 * @s: private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
static int
sdl_disconnect_req(struct sdl *s, queue_t *q, mblk_t *mp)
{
	sdl_disconnect_req_t *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sdl_get_l_state(s) != LMI_ENABLED)
		goto outstate;
	p = (typeof(p)) mp->b_rptr;
	if ((p->sdl_flags & ~(SDL_TX_DIRECTION | SDL_RX_DIRECTION)) &&
	    ((p->sdl_flags & (SDL_RX_DIRECTION | SDL_RX_DIRECTION)) == 0))
		goto badflag;
	if (((s->state.i_flags ^ ~p->sdl_flags) & p->sdl_flags) == 0)
		goto outstate;
	if ((err = sdl_disconnect(s, q, mp, p->sdl_flags)) != 0)
		goto error;
	return (0);
      badflag:
	err = LMI_UNSPEC;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDL_DISCONNECT_REQ, err);
}
#endif

#if 0
/**
 * sdt_daedt_trasnmission_req: - process SDT_DAEDT_TRANSMISSION_REQ primitive
 * @s: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
sdt_daedt_transmission_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	union SDT_primitives *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(p->daedt_transmission_req)))
		goto tooshort;
	if (sdt_get_l_state(s) != LMI_ENABLED)
		goto outstate;
	if (s->sdt.statem.daedt_state == SDT_STATE_IDLE)
		goto discard;
	if ((err = sdt_daedt_bits_for_transmission(s, q, mp, mp->b_cont)) != 0)
		goto error;
	return (0);
      discard:
	err = 0;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_DAEDT_TRANSMISSION_REQ, err);
}

/**
 * sdt_daedt_start_req: - process SDT_DAEDT_START_REQ primitive
 * @s: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
sdt_daedt_start_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	sdt_daedt_start_req_t *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sdt_get_l_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdt_daedt_start(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_DAEDT_START_REQ, err);
}

/**
 * sdt_daedr_start_req: - process SDT_DAEDR_START_REQ primitive
 * @s: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
sdt_daedr_start_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	sdt_daedr_start_req_t *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sdt_get_l_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdt_daedr_start(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_DAEDR_START_REQ, err);
}

/**
 * sdt_aerm_start_req: - process SDT_AERM_START_REQ primitive
 * @s: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
sdt_aerm_start_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	sdt_aerm_start_req_t *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sdt_get_l_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdt_aerm_start(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_AERM_START_REQ, err);
}

/**
 * sdt_aerm_stop_req: - process SDT_AERM_STOP_REQ primitive
 * @s: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
sdt_aerm_stop_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	sdt_aerm_stop_req_t *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sdt_get_l_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdt_aerm_stop(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_AERM_STOP_REQ, err);
}

/**
 * sdt_aerm_set_ti_to_tin_req: - process SDT_AERM_SET_TI_TO_TIN_REQ primitive
 * @s: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
sdt_aerm_set_ti_to_tin_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	sdt_aerm_set_ti_to_tin_req_t *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sdt_get_l_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdt_aerm_set_ti_to_tin(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_AERM_SET_TI_TO_TIN_REQ, err);
}

/**
 * sdt_aerm_set_ti_to_tie_req: - process SDT_AERM_SET_TI_TO_TIE_REQ primitive
 * @s: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
sdt_aerm_set_ti_to_tie_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	sdt_aerm_set_ti_to_tie_req_t *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sdt_get_l_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdt_aerm_set_ti_to_tie(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_AERM_SET_TI_TO_TIE_REQ, err);
}

/**
 * sdt_suerm_start_req: - process SDT_SUERM_START_REQ primitive
 * @s: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
sdt_suerm_start_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	sdt_suerm_start_req_t *p;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sdt_get_l_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdt_suerm_start(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_SUERM_START_REQ, err);
}

/**
 * sdt_suerm_stop_req: - process SDT_SUERM_STOP_REQ primitive
 * @s: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
sdt_suerm_stop_req(struct sdt *s, queue_t *q, mblk_t *mp)
{
	sdt_suerm_stop_req_t *p;
	int err = 0;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sdt_get_l_state(s) != LMI_ENABLED)
		goto outstate;
	if ((err = sdt_suerm_stop(s, q, mp)) != 0)
		goto error;
	return (0);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, SDT_SUERM_STOP_REQ, err);
}
#endif

/**
 * lmi_other_req: - process unknown primitive 
 * @s: private structure
 * @q: active queue
 * @mp: the primitive
 */
static int
lmi_other_req(struct sdl *s, queue_t *q, mblk_t *mp)
{
	lmi_ulong *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	goto notsupp;
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_reply(s, q, mp, *p, err);
}

/*
 *  =========================================================================
 *
 *  INPUT OUTPUT CONTROLS
 *
 *  =========================================================================
 */

/**
 * lmi_testoption: - test LMI options for validity
 * @s: (locked) private structure
 * @arg: options to test
 */
static int
lmi_testoption(struct sdl *s, struct lmi_option *arg)
{
	int err = 0;

	/* FIXME: check options */
	return (err);
}

/**
 * lmi_ioctl: - process LMI M_IOCTL message
 * @s: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCTL message
 * @dp: data part
 *
 * This is step 1 of the LMI input-output control operation.  Step 1 consists of a copyin operation
 * for SET operations and a copyout operation for GET operations.
 */
static int
lmi_ioctl(struct sdl *s, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *bp;
	int size = -1;
	int err = 0;

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", sdl_iocname(ioc->ioc_cmd));

	sdl_save_total_state(s);

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(LMI_IOCGOPTIONS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->option), false)))
			goto enobufs;
		bcopy(&s->option, bp->b_rptr, sizeof(s->option));
		break;
	case _IOC_NR(LMI_IOCSOPTIONS):
		size = sizeof(s->option);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0) {
		sdl_restore_total_state(s);
		return (err);
	}
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
}

/**
 * lmi_copyin: - process LMI M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is step number 2 for SET operations.  This is the result of the implicit or explicit copyin
 * operation.  We can now perform sets and commits.  All SET operations also include a last copyout
 * step that copies out the information actually set.
 */
static int
lmi_copyin(struct sdl *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, MBLKL(dp), false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, MBLKL(dp));

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sdl_iocname(cp->cp_cmd));

	sdl_save_total_state(s);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(LMI_IOCSOPTIONS):
		if ((err = lmi_testoption(s, (struct lmi_option *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->option, sizeof(s->option));
		break;
	default:
		err = EPROTO;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0) {
		sdl_restore_total_state(s);
		return (err);
	}
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
}

/**
 * lmi_copyout: - process LMI M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is the final step which is a simple copy done operation.
 */
static int
lmi_copyout(struct sdl *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sdl_iocname(cp->cp_cmd));
	mi_copyout(q, mp);
	return (0);
}

/**
 * sdl_testconfig: - test SDL configuration for validity
 * @s: private structure
 * @arg: configuration to test
 */
static int
sdl_testconfig(struct sdl *s, struct sdl_config *arg)
{
	int err = 0;

	if (arg->ifflags)
		err = EINVAL;
	if (arg->iftype != SDL_TYPE_NONE)
		err = EINVAL;
	if (arg->ifmode != SDL_MODE_NONE)
		err = EINVAL;
	if (arg->ifgmode != SDL_GMODE_NONE)
		err = EINVAL;
	if (arg->ifgtype != SDL_GMODE_NONE)
		err = EINVAL;
	if (arg->ifgcrc != SDL_GCRC_NONE)
		err = EINVAL;
	if (arg->ifclock != SDL_CLOCK_NONE)
		err = EINVAL;
	if (arg->ifcoding != SDL_CODING_NONE)
		err = EINVAL;
	if (arg->ifframing != SDL_FRAMING_NONE)
		err = EINVAL;
	if (arg->iftxlevel == 0)
		arg->iftxlevel = s->sdl.config.iftxlevel;
	else if (arg->iftxlevel <= 16)
		arg->iftxlevel--;
	else
		err = EINVAL;
	return (err);
}

#ifndef SDL_EVT_ALL
#define SDL_EVT_ALL \
	( 0 \
	  | SDL_EVT_LOST_SYNC \
	  | SDL_EVT_SU_ERROR \
	  | SDL_EVT_TX_FAIL \
	  | SDL_EVT_RX_FAIL \
	)
#endif				/* SDL_EVT_ALL */

/**
 * sdl_setnotify: - set SDL notification bits
 * @s: private structure
 * @arg: notification arguments
 */
static int
sdl_setnotify(struct sdl *s, struct sdl_notify *arg)
{
	if (arg->events & ~(SDL_EVT_ALL))
		return (EINVAL);
	arg->events = arg->events | s->sdl.notify.events;
	return (0);
}

/**
 * sdl_clrnotify: - clear SDL notification bits
 * @s: private structure
 * @arg: notification arguments
 */
static int
sdl_clrnotify(struct sdl *s, struct sdl_notify *arg)
{
	if (arg->events & ~(SDL_EVT_ALL))
		return (EINVAL);
	arg->events = ~arg->events & s->sdl.notify.events;
	return (0);
}

/**
 * sdl_ioctl: - process SDL M_IOCTL message
 * @s: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCTL message
 * @dp: data part
 *
 * This is step 1 of the SDL input-output control operation.  Step 1 consists of a copyin operation
 * for SET operations and a copyout operation for GET operations.
 */
static int
sdl_ioctl(struct sdl *s, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *bp;
	int size = -1;
	int err = 0;

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", sdl_iocname(ioc->ioc_cmd));

	sdl_save_total_state(s);

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SDL_IOCSCONFIG):
		size = sizeof(s->option);
		break;
	case _IOC_NR(SDL_IOCGCONFIG):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdl.config), false)))
			goto enobufs;
		bcopy(&s->sdl.config, bp->b_rptr, sizeof(s->sdl.config));
		break;
	case _IOC_NR(SDL_IOCTCONFIG):
		size = sizeof(s->sdl.config);
		break;
	case _IOC_NR(SDL_IOCCCONFIG):
		size = sizeof(s->sdl.config);
		break;
	case _IOC_NR(SDL_IOCGSTATEM):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdl.statem), false)))
			goto enobufs;
		bcopy(&s->sdl.statem, bp->b_rptr, sizeof(s->sdl.statem));
		break;
	case _IOC_NR(SDL_IOCCMRESET):
		/* FIXME reset the state machine. */
		break;
	case _IOC_NR(SDL_IOCGSTATSP):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdl.statsp), false)))
			goto enobufs;
		bcopy(&s->sdl.statsp, bp->b_rptr, sizeof(s->sdl.statsp));
		break;
	case _IOC_NR(SDL_IOCSSTATSP):
		size = sizeof(s->sdl.statsp);
		break;
	case _IOC_NR(SDL_IOCGSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdl.stats), false)))
			goto enobufs;
		bcopy(&s->sdl.stats, bp->b_rptr, sizeof(s->sdl.stats));
		break;
	case _IOC_NR(SDL_IOCCSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdl.stats), false)))
			goto enobufs;
		bcopy(&s->sdl.stats, bp->b_rptr, sizeof(s->sdl.stats));
		bzero(&s->sdl.stats, sizeof(s->sdl.stats));
		break;
	case _IOC_NR(SDL_IOCGNOTIFY):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdl.notify), false)))
			goto enobufs;
		bcopy(&s->sdl.notify, bp->b_rptr, sizeof(s->sdl.notify));
		break;
	case _IOC_NR(SDL_IOCSNOTIFY):
		size = sizeof(s->sdl.notify);
		break;
	case _IOC_NR(SDL_IOCCNOTIFY):
		size = sizeof(s->sdl.notify);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0) {
		sdl_restore_total_state(s);
		return (err);
	}
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
}

/**
 * sdl_copyin: - process SDL M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is step number 2 for SET operations.  This is the result of the implicit or explicit copyin
 * operation.  We can now perform sets and commits.  All SET operations also include a last copyout
 * step that copies out the information actually set.
 */
static int
sdl_copyin(struct sdl *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, MBLKL(dp), false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, MBLKL(dp));

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sdl_iocname(cp->cp_cmd));

	sdl_save_total_state(s);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SDL_IOCSCONFIG):
		if ((err = sdl_testconfig(s, (struct sdl_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sdl.config, sizeof(s->sdl.config));
		break;
	case _IOC_NR(SDL_IOCTCONFIG):
		err = sdl_testconfig(s, (struct sdl_config *) bp->b_rptr);
		break;
	case _IOC_NR(SDL_IOCCCONFIG):
		if ((err = sdl_testconfig(s, (struct sdl_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sdl.config, sizeof(s->sdl.config));
		break;
	case _IOC_NR(SDL_IOCSSTATSP):
		bcopy(bp->b_rptr, &s->sdl.statsp, sizeof(s->sdl.statsp));
		break;
	case _IOC_NR(SDL_IOCSNOTIFY):
		if ((err = sdl_setnotify(s, (struct sdl_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sdl.notify, sizeof(s->sdl.notify));
		break;
	case _IOC_NR(SDL_IOCCNOTIFY):
		if ((err = sdl_clrnotify(s, (struct sdl_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sdl.notify, sizeof(s->sdl.notify));
		break;
	default:
		err = EPROTO;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0) {
		sdl_restore_total_state(s);
		return (err);
	}
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
}

/**
 * sdl_copyout: - process SDL M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is the final step which is a simple copy done operation.
 */
static int
sdl_copyout(struct sdl *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sdl_iocname(cp->cp_cmd));
	mi_copyout(q, mp);
	return (0);
}

#if 0
/**
 * sdt_testconfig: - test SDT configuration
 * @s: (locked) private structure
 * @arg: configuration argument
 */
static int
sdt_testconfig(struct sdt *s, struct sdt_config *arg)
{
	int err = 0;

	/* FIXME: check configuration */
	return (err);
}

#ifndef SDT_EVT_ALL
#define SDT_EVT_ALL \
	( 0 \
	    | SDT_EVT_LOST_SYNC \
	    | SDT_EVT_SU_ERROR \
	    | SDT_EVT_TX_FAIL \
	    | SDT_EVT_RX_FAIL \
	    | SDT_EVT_CARRIER \
	)
#endif				/* SDT_EVT_ALL */

/**
 * sdt_setnotify: - set SDT notifications bits
 * @s: (locked) private structure
 * @arg: notification argument
 */
static int
sdt_setnotify(struct sdt *s, struct sdt_notify *arg)
{
	if (arg->events & ~(SDT_EVT_ALL))
		return (EINVAL);
	arg->events = arg->events | s->sdt.notify.events;
	return (0);
}

/**
 * sdt_clrnotify: - clear SDT notifications bits
 * @s: (locked) private structure
 * @arg: notification argument
 */
static int
sdt_clrnotify(struct sdt *s, struct sdt_notify *arg)
{
	if (arg->events & ~(SDT_EVT_ALL))
		return (EINVAL);
	arg->events = ~arg->events & s->sdt.notify.events;
	return (0);
}

/**
 * sdt_ioctl: - process SDT M_IOCTL message
 * @s: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the SDT input-output control operation.  Step 1 consists
 * of a copyin operation for SET operations and a copyout operation for GET
 * operations.
 */
static int
sdt_ioctl(struct sdt *s, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *bp;
	int size = -1;
	int err = 0;

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", sdt_iocname(ioc->ioc_cmd));

	sdt_save_total_state(s);
	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SDT_IOCGCONFIG):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdt.config), false)))
			goto enobufs;
		bcopy(&s->sdt.config, bp->b_rptr, sizeof(s->sdt.config));
		break;
	case _IOC_NR(SDT_IOCSCONFIG):
		size = sizeof(s->option);
		break;
	case _IOC_NR(SDT_IOCTCONFIG):
		size = sizeof(s->sdt.config);
		break;
	case _IOC_NR(SDT_IOCCCONFIG):
		size = sizeof(s->sdt.config);
		break;
	case _IOC_NR(SDT_IOCGSTATEM):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdt.statem), false)))
			goto enobufs;
		bcopy(&s->sdt.statem, bp->b_rptr, sizeof(s->sdt.statem));
		break;
	case _IOC_NR(SDT_IOCCMRESET):
		/* FIXME reset the state machine. */
		break;
	case _IOC_NR(SDT_IOCGSTATSP):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdt.statsp), false)))
			goto enobufs;
		bcopy(&s->sdt.statsp, bp->b_rptr, sizeof(s->sdt.statsp));
		break;
	case _IOC_NR(SDT_IOCSSTATSP):
		size = sizeof(s->sdt.statsp);
		break;
	case _IOC_NR(SDT_IOCGSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdt.stats), false)))
			goto enobufs;
		bcopy(&s->sdt.stats, bp->b_rptr, sizeof(s->sdt.stats));
		break;
	case _IOC_NR(SDT_IOCCSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdt.stats), false)))
			goto enobufs;
		bcopy(&s->sdt.stats, bp->b_rptr, sizeof(s->sdt.stats));
		bzero(&s->sdt.stats, sizeof(s->sdt.stats));
		break;
	case _IOC_NR(SDT_IOCGNOTIFY):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sdt.notify), false)))
			goto enobufs;
		bcopy(&s->sdt.notify, bp->b_rptr, sizeof(s->sdt.notify));
		break;
	case _IOC_NR(SDT_IOCSNOTIFY):
		size = sizeof(s->sdt.notify);
		break;
	case _IOC_NR(SDT_IOCCNOTIFY):
		size = sizeof(s->sdt.notify);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0) {
		sdt_restore_total_state(s);
		return (err);
	}
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == 0)
			mi_copy_done(q, mp, 0);
		else if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
}

/**
 * sdt_copyin: - process SDT M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is step number 2 for SET operations.  This is the result of the
 * implicit or explicit copyin operation.  We can now perform sets and
 * commits.  All SET operations also include a last copyout step that copies
 * out the information actually set.
 */
static int
sdt_copyin(struct sdt *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, MBLKL(dp), false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, MBLKL(dp));

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sdt_iocname(cp->cp_cmd));

	sdt_save_total_state(s);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SDT_IOCSCONFIG):
		if ((err = sdt_testconfig(s, (struct sdt_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sdt.config, sizeof(s->sdt.config));
		break;
	case _IOC_NR(SDT_IOCTCONFIG):
		err = sdt_testconfig(s, (struct sdt_config *) bp->b_rptr);
		break;
	case _IOC_NR(SDT_IOCCCONFIG):
		if ((err = sdt_testconfig(s, (struct sdt_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sdt.config, sizeof(s->sdt.config));
		break;
	case _IOC_NR(SDT_IOCSSTATSP):
		bcopy(bp->b_rptr, &s->sdt.statsp, sizeof(s->sdt.statsp));
		break;
	case _IOC_NR(SDT_IOCSNOTIFY):
		if ((err = sdt_setnotify(s, (struct sdt_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sdt.notify, sizeof(s->sdt.notify));
		break;
	case _IOC_NR(SDT_IOCCNOTIFY):
		if ((err = sdt_clrnotify(s, (struct sdt_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sdt.notify, sizeof(s->sdt.notify));
		break;
	default:
		err = EPROTO;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0) {
		sdt_restore_total_state(s);
		return (err);
	}
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
}

/**
 * sdt_copyout: - process SDT M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is the final step which is a simple copy done operation.
 */
static int
sdt_copyout(struct sdt *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sdt_iocname(cp->cp_cmd));
	mi_copyout(q, mp);
	return (0);
}
#endif

#if 0
/**
 * sl_testconfig: - test SL configuration
 * @s: (locked) private structure
 * @arg: configuration argument
 */
static int
sl_testconfig(struct sl *s, struct sl_config *arg)
{
	int err = 0;

	/* FIXME: check configuration */
	return (err);
}

#ifndef SL_EVT_ALL
#define SL_EVT_ALL \
	( 0 \
	    | SL_EVT_LOST_SYNC \
	    | SL_EVT_SU_ERROR \
	    | SL_EVT_TX_FAIL \
	    | SL_EVT_RX_FAIL \
	    | SL_EVT_CARRIER \
	)
#endif				/* SL_EVT_ALL */

/**
 * sl_setnotify: - set SL notifications bits
 * @s: (locked) private structure
 * @arg: notification argument
 */
static int
sl_setnotify(struct sl *s, struct sl_notify *arg)
{
	if (arg->events & ~(SL_EVT_ALL))
		return (EINVAL);
	arg->events = arg->events | s->sl.notify.events;
	return (0);
}

/**
 * sl_clrnotify: - clear SL notifications bits
 * @s: (locked) private structure
 * @arg: notification argument
 */
static int
sl_clrnotify(struct sl *s, struct sl_notify *arg)
{
	if (arg->events & ~(SL_EVT_ALL))
		return (EINVAL);
	arg->events = ~arg->events & s->sl.notify.events;
	return (0);
}

/**
 * sl_ioctl: - process SL M_IOCTL message
 * @s: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the SL input-output control operation.  Step 1 consists
 * of a copyin operation for SET operations and a copyout operation for GET
 * operations.
 */
static int
sl_ioctl(struct sl *s, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *bp;
	int size = -1;
	int err = 0;

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", sl_iocname(ioc->ioc_cmd));

	sl_save_total_state(s);
	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SL_IOCGCONFIG):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sl.config), false)))
			goto enobufs;
		bcopy(&s->sl.config, bp->b_rptr, sizeof(s->sl.config));
		break;
	case _IOC_NR(SL_IOCSCONFIG):
		size = sizeof(s->option);
		break;
	case _IOC_NR(SL_IOCTCONFIG):
		size = sizeof(s->sl.config);
		break;
	case _IOC_NR(SL_IOCCCONFIG):
		size = sizeof(s->sl.config);
		break;
	case _IOC_NR(SL_IOCGSTATEM):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sl.statem), false)))
			goto enobufs;
		bcopy(&s->sl.statem, bp->b_rptr, sizeof(s->sl.statem));
		break;
	case _IOC_NR(SL_IOCCMRESET):
		/* FIXME reset the state machine. */
		break;
	case _IOC_NR(SL_IOCGSTATSP):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sl.statsp), false)))
			goto enobufs;
		bcopy(&s->sl.statsp, bp->b_rptr, sizeof(s->sl.statsp));
		break;
	case _IOC_NR(SL_IOCSSTATSP):
		size = sizeof(s->sl.statsp);
		break;
	case _IOC_NR(SL_IOCGSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sl.stats), false)))
			goto enobufs;
		bcopy(&s->sl.stats, bp->b_rptr, sizeof(s->sl.stats));
		break;
	case _IOC_NR(SL_IOCCSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sl.stats), false)))
			goto enobufs;
		bcopy(&s->sl.stats, bp->b_rptr, sizeof(s->sl.stats));
		bzero(&s->sl.stats, sizeof(s->sl.stats));
		break;
	case _IOC_NR(SL_IOCGNOTIFY):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(s->sl.notify), false)))
			goto enobufs;
		bcopy(&s->sl.notify, bp->b_rptr, sizeof(s->sl.notify));
		break;
	case _IOC_NR(SL_IOCSNOTIFY):
		size = sizeof(s->sl.notify);
		break;
	case _IOC_NR(SL_IOCCNOTIFY):
		size = sizeof(s->sl.notify);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0) {
		sl_restore_total_state(s);
		return (err);
	}
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == 0)
			mi_copy_done(q, mp, 0);
		else if (size == -1)
			mi_copyout(q, mp);
		else
			mi_copyin(q, mp, NULL, size);
	}
	return (0);
}

/**
 * sl_copyin: - process SL M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is step number 2 for SET operations.  This is the result of the
 * implicit or explicit copyin operation.  We can now perform sets and
 * commits.  All SET operations also include a last copyout step that copies
 * out the information actually set.
 */
static int
sl_copyin(struct sl *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, MBLKL(dp), false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, MBLKL(dp));

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sl_iocname(cp->cp_cmd));

	sl_save_total_state(s);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SL_IOCSCONFIG):
		if ((err = sl_testconfig(s, (struct sl_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sl.config, sizeof(s->sl.config));
		break;
	case _IOC_NR(SL_IOCTCONFIG):
		err = sl_testconfig(s, (struct sl_config *) bp->b_rptr);
		break;
	case _IOC_NR(SL_IOCCCONFIG):
		if ((err = sl_testconfig(s, (struct sl_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sl.config, sizeof(s->sl.config));
		break;
	case _IOC_NR(SL_IOCSSTATSP):
		bcopy(bp->b_rptr, &s->sl.statsp, sizeof(s->sl.statsp));
		break;
	case _IOC_NR(SL_IOCSNOTIFY):
		if ((err = sl_setnotify(s, (struct sl_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sl.notify, sizeof(s->sl.notify));
		break;
	case _IOC_NR(SL_IOCCNOTIFY):
		if ((err = sl_clrnotify(s, (struct sl_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &s->sl.notify, sizeof(s->sl.notify));
		break;
	default:
		err = EPROTO;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0) {
		sl_restore_total_state(s);
		return (err);
	}
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
}

/**
 * sl_copyout: - process SL M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is the final step which is a simple copy done operation.
 */
static int
sl_copyout(struct sl *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sl_iocname(cp->cp_cmd));
	mi_copyout(q, mp);
	return (0);
}
#endif

/**
 * sdl_do_ioctl: - process M_IOCTL message
 * @ch: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the input-output control operation.  Step 1 consists
 * of a copyin operation for SET operations and a copyout operation for GET
 * operations.
 */
static int
sdl_do_ioctl(struct sdl *s, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*ioc))) || unlikely(mp->b_cont == NULL)) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case LMI_IOC_MAGIC:
		return lmi_ioctl(s, q, mp);
	case SDL_IOC_MAGIC:
		return sdl_ioctl(s, q, mp);
#if 0
	case SDT_IOC_MAGIC:
		return sdt_ioctl(s, q, mp);
#endif
#if 0
	case SL_IOC_MAGIC:
		return sl_ioctl(s, q, mp);
#endif
	default:
		if (bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

/**
 * sdl_do_copyin: - process M_IOCDATA message
 * @s: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is step number 2 for SET operations.  This is the result of the
 * implicit or explicit copyin operation.  We can now perform sets and
 * commits.  All SET operations also include a last copyout step that copies
 * out the information actually set.
 */
static int
sdl_do_copyin(struct sdl *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*cp))) || unlikely(mp->b_cont == NULL)) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	switch (_IOC_TYPE(cp->cp_cmd)) {
	case LMI_IOC_MAGIC:
		return lmi_copyin(s, q, mp, dp);
	case SDL_IOC_MAGIC:
		return sdl_copyin(s, q, mp, dp);
#if 0
	case SDT_IOC_MAGIC:
		return sdt_copyin(s, q, mp, dp);
#endif
#if 0
	case SL_IOC_MAGIC:
		return sl_copyin(s, q, mp, dp);
#endif
	default:
		if (bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

/**
 * sdl_do_copyout: - process M_IOCDATA message
 * @s: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is the final stop which is a simple copy done operation.
 */
static int
sdl_do_copyout(struct sdl *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*cp))) || unlikely(mp->b_cont == NULL)) {
		mi_copy_done(q, mp, EFAULT);
		return (0);
	}
	switch (_IOC_TYPE(cp->cp_cmd)) {
	case LMI_IOC_MAGIC:
		return lmi_copyout(s, q, mp, dp);
	case SDL_IOC_MAGIC:
		return sdl_copyout(s, q, mp, dp);
#if 0
	case SDT_IOC_MAGIC:
		return sdt_copyout(s, q, mp, dp);
#endif
#if 0
	case SL_IOC_MAGIC:
		return sl_copyout(s, q, mp, dp);
#endif
	default:
		if (bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

/*
 *  =========================================================================
 *
 *  STREAMS MESSAGE HANDLING
 *
 *  =========================================================================
 */

/**
 * __sdl_m_data: - process M_DATA message
 * @s: (locked) private structure
 * @q: active queue
 * @mp: the M_DATA message
 */
static fastcall int
__sdl_m_data(struct sdl *s, queue_t *q, mblk_t *mp)
{
	return sdl_layer1_bits_for_transmission(s->other, q, NULL, mp);
}

/**
 * sdl_m_data: - process M_DATA message
 * @q: active queue
 * @mp: the M_DATA message
 */
static inline fastcall int
sdl_m_data(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if (likely((priv = mi_trylock(q)) != NULL)) {
		err = __sdl_m_data(SDL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

/**
 * sdl_m_proto_slow: - process M_(PC)PROTO message
 * @s: private structure
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 * @prim: the primitive
 */
static fastcall int
sdl_m_proto_slow(struct sdl *s, queue_t *q, mblk_t *mp, lmi_ulong prim)
{
	int rtn;

	switch (prim) {
	case SDL_BITS_FOR_TRANSMISSION_REQ:
		mi_strlog(s->oq, STRLOGDA, SL_TRACE, "-> %s", sdl_primname(prim));
		break;
	default:
		mi_strlog(s->oq, STRLOGRX, SL_TRACE, "-> %s", sdl_primname(prim));
		break;
	}

	sdl_save_total_state(s);

	switch (prim) {
	case LMI_INFO_REQ:
		rtn = lmi_info_req(s, q, mp);
		break;
	case LMI_ATTACH_REQ:
		rtn = lmi_attach_req(s, q, mp);
		break;
	case LMI_DETACH_REQ:
		rtn = lmi_detach_req(s, q, mp);
		break;
	case LMI_ENABLE_REQ:
		rtn = lmi_enable_req(s, q, mp);
		break;
	case LMI_DISABLE_REQ:
		rtn = lmi_disable_req(s, q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		rtn = lmi_optmgmt_req(s, q, mp);
		break;
#if 1
	case SDL_BITS_FOR_TRANSMISSION_REQ:
		rtn = sdl_bits_for_transmission_req(s, q, mp);
		break;
	case SDL_CONNECT_REQ:
		rtn = sdl_connect_req(s, q, mp);
		break;
	case SDL_DISCONNECT_REQ:
		rtn = sdl_disconnect_req(s, q, mp);
		break;
#endif
#if 0
	case SDT_DAEDT_TRANSMISSION_REQ:
		rtn = sdt_daedt_transmission_req(s, q, mp);
		break;
	case SDT_DAEDT_START_REQ:
		rtn = sdt_daedt_start_req(s, q, mp);
		break;
	case SDT_DAEDR_START_REQ:
		rtn = sdt_daedr_start_req(s, q, mp);
		break;
	case SDT_AERM_START_REQ:
		rtn = sdt_aerm_start_req(s, q, mp);
		break;
	case SDT_AERM_STOP_REQ:
		rtn = sdt_aerm_stop_req(s, q, mp);
		break;
	case SDT_AERM_SET_TI_TO_TIN_REQ:
		rtn = sdt_aerm_set_ti_to_tin_req(s, q, mp);
		break;
	case SDT_AERM_SET_TI_TO_TIE_REQ:
		rtn = sdt_aerm_set_ti_to_tie_req(s, q, mp);
		break;
	case SDT_SUERM_START_REQ:
		rtn = sdt_suerm_start_req(s, q, mp);
		break;
	case SDT_SUERM_STOP_REQ:
		rtn = sdt_suerm_stop_req(s, q, mp);
		break;
#endif
#if 0
	case SL_PDU_REQ:
		rtn = sl_pdu_req(s, q, mp);
		break;
	case SL_EMERGENCY_REQ:
		rtn = sl_emergency_req(s, q, mp);
		break;
	case SL_EMERGENCY_CEASES_REQ:
		rtn = sl_emergency_ceases_req(s, q, mp);
		break;
	case SL_START_REQ:
		rtn = sl_start_req(s, q, mp);
		break;
	case SL_STOP_REQ:
		rtn = sl_stop_req(s, q, mp);
		break;
	case SL_RETRIEVE_BSNT_REQ:
		rtn = sl_retrieve_bsnt_req(s, q, mp);
		break;
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		rtn = sl_retrieval_request_and_fsnc_req(s, q, mp);
		break;
	case SL_CLEAR_BUFFERS_REQ:
		rtn = sl_clear_buffers_req(s, q, mp);
		break;
	case SL_CLEAR_RTB_REQ:
		rtn = sl_clear_rtb_req(s, q, mp);
		break;
	case SL_CONTINUE_REQ:
		rtn = sl_continue_req(s, q, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		rtn = sl_local_processor_outage_req(s, q, mp);
		break;
	case SL_RESUME_REQ:
		rtn = sl_resume_req(s, q, mp);
		break;
	case SL_CONGESTION_DISCARD_REQ:
		rtn = sl_congestion_discard_req(s, q, mp);
		break;
	case SL_CONGESTION_ACCEPT_REQ:
		rtn = sl_congestion_accept_req(s, q, mp);
		break;
	case SL_NO_CONGESTION_REQ:
		rtn = sl_no_congestion_req(s, q, mp);
		break;
	case SL_POWER_ON_REQ:
		rtn = sl_power_on_req(s, q, mp);
		break;
	case SL_OPTMGMT_REQ:
		rtn = sl_optmgmt_req(s, q, mp);
		break;
	case SL_NOTIFY_REQ:
		rtn = sl_notify_req(s, q, mp);
		break;
#endif
	default:
		rtn = lmi_other_req(s, q, mp);
		break;
	}
	if (rtn)
		sdl_restore_total_state(s);
	return (rtn);
}

/**
 * __sdl_m_proto: - process M_(PC)PROTO message
 * @s: locked private structure
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 */
static inline fastcall int
__sdl_m_proto(struct sdl *s, queue_t *q, mblk_t *mp)
{
	t_uscalar_t prim;
	int rtn;

	if (unlikely(MBLKIN(mp, 0, sizeof(prim)))) {
		sdl_ulong prim = *(typeof(prim) *) mp->b_rptr;

		if (likely(prim == SDL_BITS_FOR_TRANSMISSION_REQ)) {
#ifndef _OPTIMIZE_SPEED
			mi_strlog(s->oq, STRLOGDA, SL_TRACE, "-> SDL_BITS_FOR_TRANSMISSION_REQ");
#endif				/* _OPTIMIZE_SPEED */
			rtn = sdl_layer1_bits_for_transmission(s, q, mp, mp->b_cont);
		} else {
			rtn = sdl_m_proto_slow(s, q, mp, prim);
		}
	} else {
		freemsg(mp);
		rtn = 0;
	}
	return (rtn);
}

/**
 * sdl_m_proto: = process M_(PC)PROTO message
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 */
static inline fastcall int
sdl_m_proto(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if (likely((priv = mi_trylock(q)) != NULL)) {
		err = __sdl_m_proto(SDL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

/**
 * __sdl_m_sig: process M_(PC)SIG message
 * @s: (locked) private structure
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 *
 * This is the method for processing tick timers.  Under the tick approach
 * each time that the tick timer fires we reset the timer.  Then we prepare as
 * many blocks as will fit in the interval and send them to the other side.
 */
static inline fastcall int
__sdl_m_sig(struct sdl *s, queue_t *q, mblk_t *mp)
{
	int rtn = 0;

	if (!mi_timer_valid(mp))
		return (0);


	return (rtn);
}

/**
 * sdl_m_sig: process M_(PC)SIG message
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 */
static inline fastcall int
sdl_m_sig(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if (likely((priv = mi_trylock(q)) != NULL)) {
		err = __sdl_m_sig(SDL_PRIV(q), q, mp);
		mi_unlock(priv);
	} else 
		err = mi_timer_requeue(mp) ? -EAGAIN : 0;
	return (err);
}

/**
 * sdl_m_flush: - process M_FLUSH message
 * @q: active queue
 * @mp: the M_FLUSH message
 *
 * Avoid having to push pipemod.  If we are the bottommost module over a pipe
 * twist then perform the actions of pipemod.  This means that the sdl-pmod
 * module must be pushed over the same side of a pipe as pipemod, if pipemod
 * is pushed at all.
 */
static fastcall int
sdl_m_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	/* pipemod style flush bit reversal */
	if (!SAMESTR(q)) {
		switch (mp->b_rptr[0] & FLUSHRW) {
		case FLUSHW:
			mp->b_rptr[0] &= ~FLUSHW;
			mp->b_rptr[0] |= FLUSHR;
			break;
		case FLUSHR:
			mp->b_rptr[0] &= ~FLUSHR;
			mp->b_rptr[0] |= FLUSHW;
			break;
		}
	}
	putnext(q, mp);
	return (0);
}

static fastcall int
__sdl_m_ioctl(struct sdl *s, queue_t *q, mblk_t *mp)
{
	int err;

	err = sdl_do_ioctl(s, q, mp);
	if (err <= 0)
		return (err);
	mi_copy_done(q, mp, err);
	return (0);
}

/**
 * sdl_m_ioctl: - process M_IOCTL message
 * @q: active queue
 * @mp: the M_IOCTL message
 */
static fastcall int
sdl_m_ioctl(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if ((priv = mi_trylock(q)) != NULL) {
		err = __sdl_m_ioctl(SDL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

static fastcall int
__sdl_m_iocdata(struct sdl *s, queue_t *q, mblk_t *mp)
{

	mblk_t *dp;
	int err;

	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		err = 0;
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		err = sdl_do_copyin(s, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		err = sdl_do_copyout(s, q, mp, dp);
		break;
	default:
		err = EPROTO;
		break;
	}
	if (err <= 0)
		return (err);
	mi_copy_done(q, mp, err);
	return (0);
}

/**
 * sdl_m_iocdata: - process M_IOCDATA message
 * @q: active queue
 * @mp: the M_IOCDATA message
 */
static fastcall int
sdl_m_iocdata(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if ((priv = mi_trylock(q)) != NULL) {
		err = __sdl_m_iocdata(SDL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

/**
 * sdl_m_rse: - process M_(PC)RSE message
 * @q: active queue
 * @mp: the M_(PC)RSE message
 */
static fastcall int
sdl_m_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * sdl_m_other: - process other STREAMS message
 * @q: active queue
 * @mp: other STREAMS message
 *
 * Simply pass unrecognized messages along.
 */
static fastcall int
sdl_m_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * sdl_msg_slow: - process STREAMS message, slow
 * @q: active queue
 * @mp: the STREAMS message
 *
 * This is the slow version of the STREAMS message handling.  It is expected
 * that data is delivered in M_DATA message blocks instead of SDL_PDU_IND or
 * SDL_PDU_REQ message blocks.  Nevertheless, if this slower function gets
 * called, it is more likely because we have an M_PROTO message block
 * containing an SDL_PDU_REQ.
 */
static noinline fastcall int
sdl_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return sdl_m_proto(q, mp);
#if 1
	case M_SIG:
	case M_PCSIG:
		return sdl_m_sig(q, mp);
#endif
	case M_IOCTL:
		return sdl_m_ioctl(q, mp);
	case M_IOCDATA:
		return sdl_m_iocdata(q, mp);
	case M_FLUSH:
		return sdl_m_flush(q, mp);
	case M_RSE:
	case M_PCRSE:
		return sdl_m_rse(q, mp);
	default:
		return sdl_m_other(q, mp);
	case M_DATA:
		return sdl_m_data(q, mp);
	}
}

/**
 * sdl_msg: - process STREAMS message
 * @q: active queue
 * @mp: the message
 *
 * This function returns zero when the message has been absorbed.  When it returns non-zero, the
 * message is to be placed (back) on the queue.
 */
static inline fastcall int
sdl_msg(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return sdl_m_data(q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return sdl_m_proto(q, mp);
	return sdl_msg_slow(q, mp);
}

/**
 * __sdl_msg_slow: - process STREAMS message, slow
 * @s: locked private structure
 * @q: active queue
 * @mp: the STREAMS message
 */
static noinline fastcall int
__sdl_msg_slow(struct sdl *s, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __sdl_m_proto(s, q, mp);
	case M_SIG:
	case M_PCSIG:
		return __sdl_m_sig(s, q, mp);
	case M_IOCTL:
		return __sdl_m_ioctl(s, q, mp);
	case M_IOCDATA:
		return __sdl_m_iocdata(s, q, mp);
	case M_FLUSH:
		return sdl_m_flush(q, mp);
	case M_RSE:
	case M_PCRSE:
		return sdl_m_rse(q, mp);
	default:
		return sdl_m_other(q, mp);
	case M_DATA:
		return __sdl_m_data(s, q, mp);
	}
}

/**
 * __sdl_msg: - process STREAMS message locked
 * @s: locked private structure
 * @q: active queue
 * @mp: the message
 *
 * This function returns zero when the message has been absorbed.  When it returns non-zero, the
 * message is to be placed (back) on the queue.
 */
static inline fastcall int
__sdl_msg(struct sdl *s, queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return __sdl_m_data(s, q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return __sdl_m_proto(s, q, mp);
	return __sdl_msg_slow(s, q, mp);
}

/*
 *  =========================================================================
 *
 *  STREAMS QUEUE PUT AND SERVICE PROCEDURES
 *
 *  =========================================================================
 */

/**
 * sdl_putp: - put procedure
 * @q: active queue
 * @mp: message to put
 *
 * Quick canonical put procedure.
 */
static streamscall __hot_in int
sdl_putp(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sdl_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * sdl_srvp: - service procedure
 * @q: queue to service
 *
 * Quick canonical service procedure.  This is a  little quicker for
 * processing bulked messages because it takes the lock once for the entire
 * group of M_DATA messages, instead of once for each message.
 */
static streamscall __hot_read int
sdl_srvp(queue_t *q)
{
	mblk_t *mp;

	if (likely((mp = getq(q)) != NULL)) {
		caddr_t priv;

		if (likely((priv = mi_trylock(q)) != NULL)) {
			do {
				if (unlikely(__sdl_msg(SDL_PRIV(q), q, mp) != 0))
					break;
			} while (likely((mp = getq(q)) != NULL));
			mi_unlock(priv);
		}
		if (unlikely(mp != NULL))
			putbq(q, mp);
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS QUEUE OPEN AND CLOSE ROUTINES
 *
 *  =========================================================================
 */

static caddr_t sdl_opens = NULL;

/**
 * sdl_qopen: - STREAMS module queue open routine
 * @q: read queue of freshly allocated queue pair
 * @devp: device number of driver
 * @oflags: flags to open(2) call
 * @sflag: STREAMS flag
 * @crp: credentials of opening process
 */
static streamscall int
sdl_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct sdl_pair *p;
	mblk_t *tick;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if ((tick = mi_timer_alloc(0)) == NULL)
		return (ENOBUFS);
	if ((err = mi_open_comm(&sdl_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		mi_timer_free(tick);
		return (err);
	}

	p = PRIV(q);
	bzero(p, sizeof(*p));

	/* initialize the structure */
	p->r_priv.pair = p;
	p->r_priv.other = &p->w_priv;
	p->r_priv.oq = WR(q);
	p->r_priv.max_sdu = FASTBUF;
	p->r_priv.min_sdu = FASTBUF;
	p->r_priv.header_len = 0;
	p->r_priv.ppa_style = LMI_STYLE1;
	p->r_priv.state.l_state = LMI_DISABLED;
	p->r_priv.oldstate.l_state = LMI_DISABLED;
	p->r_priv.state.i_state = LMI_DISABLED;
	p->r_priv.oldstate.i_state = LMI_DISABLED;
	p->r_priv.state.i_flags = 0;
	p->r_priv.oldstate.i_flags = 0;

	p->r_priv.timestamp = jiffies;
	p->r_priv.bytecount = 0;
	p->r_priv.ticktimer = 0;
	p->r_priv.threshold = 8000 / HZ;

	p->r_priv.option.pvar = SS7_PVAR_ITUT_00;
	p->r_priv.option.popt = 0;

	p->r_priv.sdl.notify.events = 0;

	p->r_priv.sdl.config.ifname = NULL;
	p->r_priv.sdl.config.iftype = SDL_TYPE_PACKET;
	p->r_priv.sdl.config.ifrate = SDL_RATE_NONE;
	p->r_priv.sdl.config.ifgtype = SDL_GTYPE_NONE;
	p->r_priv.sdl.config.ifgrate = SDL_GRATE_NONE;
	p->r_priv.sdl.config.ifmode = SDL_MODE_PEER;
	p->r_priv.sdl.config.ifgmode = SDL_GMODE_NONE;
	p->r_priv.sdl.config.ifgcrc = SDL_GCRC_NONE;
	p->r_priv.sdl.config.ifclock = SDL_CLOCK_NONE;
	p->r_priv.sdl.config.ifcoding = SDL_CODING_NONE;
	p->r_priv.sdl.config.ifframing = SDL_FRAMING_NONE;
	p->r_priv.sdl.config.ifblksize = 8;
	p->r_priv.sdl.config.ifleads = 0;
	p->r_priv.sdl.config.ifbpv = 0;
	p->r_priv.sdl.config.ifalarms = 0;
	p->r_priv.sdl.config.ifrxlevel = 0;
	p->r_priv.sdl.config.iftxlevel = 1;
	p->r_priv.sdl.config.ifsync = 0;
	// p->r_priv.sdl.config.ifsyncsrc = { 0, 0, 0, 0};

	p->r_priv.sdl.statem.tx_state = SDL_STATE_IDLE;
	p->r_priv.sdl.statem.rx_state = SDL_STATE_IDLE;

	p->r_priv.sdl.stats.rx_octets = 0;
	p->r_priv.sdl.stats.tx_octets = 0;
	p->r_priv.sdl.stats.rx_overruns = 0;
	p->r_priv.sdl.stats.tx_underruns = 0;
	p->r_priv.sdl.stats.rx_buffer_overflows = 0;
	p->r_priv.sdl.stats.tx_buffer_overflows = 0;
	p->r_priv.sdl.stats.lead_cts_lost = 0;
	p->r_priv.sdl.stats.lead_dcd_lost = 0;
	p->r_priv.sdl.stats.carrier_lost = 0;

	p->w_priv.pair = p;
	p->w_priv.other = &p->r_priv;
	p->w_priv.oq = q;
	p->w_priv.max_sdu = FASTBUF;
	p->w_priv.min_sdu = FASTBUF;
	p->w_priv.header_len = 0;
	p->w_priv.ppa_style = LMI_STYLE1;
	p->w_priv.state.l_state = LMI_DISABLED;
	p->w_priv.oldstate.l_state = LMI_DISABLED;
	p->w_priv.state.i_state = LMI_DISABLED;
	p->w_priv.oldstate.i_state = LMI_DISABLED;
	p->w_priv.state.i_flags = 0;
	p->w_priv.oldstate.i_flags = 0;

	p->w_priv.timestamp = jiffies;
	p->w_priv.bytecount = 0;
	p->w_priv.ticktimer = 0;
	p->w_priv.threshold = 8000 / HZ;

	p->w_priv.option.pvar = SS7_PVAR_ITUT_00;
	p->w_priv.option.popt = 0;

	p->w_priv.sdl.notify.events = 0;

	p->w_priv.sdl.config.ifname = NULL;
	p->w_priv.sdl.config.iftype = SDL_TYPE_PACKET;
	p->w_priv.sdl.config.ifrate = SDL_RATE_NONE;
	p->w_priv.sdl.config.ifgtype = SDL_GTYPE_NONE;
	p->w_priv.sdl.config.ifgrate = SDL_GRATE_NONE;
	p->w_priv.sdl.config.ifmode = SDL_MODE_PEER;
	p->w_priv.sdl.config.ifgmode = SDL_GMODE_NONE;
	p->w_priv.sdl.config.ifgcrc = SDL_GCRC_NONE;
	p->w_priv.sdl.config.ifclock = SDL_CLOCK_NONE;
	p->w_priv.sdl.config.ifcoding = SDL_CODING_NONE;
	p->w_priv.sdl.config.ifframing = SDL_FRAMING_NONE;
	p->w_priv.sdl.config.ifblksize = 8;
	p->w_priv.sdl.config.ifleads = 0;
	p->w_priv.sdl.config.ifbpv = 0;
	p->w_priv.sdl.config.ifalarms = 0;
	p->w_priv.sdl.config.ifrxlevel = 0;
	p->w_priv.sdl.config.iftxlevel = 1;
	p->w_priv.sdl.config.ifsync = 0;
	// p->w_priv.sdl.config.ifsyncsrc = { 0, 0, 0, 0};

	p->w_priv.sdl.statem.tx_state = SDL_STATE_IDLE;
	p->w_priv.sdl.statem.rx_state = SDL_STATE_IDLE;

	p->w_priv.sdl.stats.rx_octets = 0;
	p->w_priv.sdl.stats.tx_octets = 0;
	p->w_priv.sdl.stats.rx_overruns = 0;
	p->w_priv.sdl.stats.tx_underruns = 0;
	p->w_priv.sdl.stats.rx_buffer_overflows = 0;
	p->w_priv.sdl.stats.tx_buffer_overflows = 0;
	p->w_priv.sdl.stats.lead_cts_lost = 0;
	p->w_priv.sdl.stats.lead_dcd_lost = 0;
	p->w_priv.sdl.stats.carrier_lost = 0;

	qprocson(q);
	return (0);
}

/**
 * sdl_qclose: - STREAMS module queue close routine
 * @q: read queue of queue pair
 * @oflags: flags to open(2) call
 * @crp: credentials of closing process
 */
static streamscall int
sdl_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct sdl *sdl = SDL_PRIV(q);

	qprocsoff(q);
	mi_timer_free(sdl->tick);
	mi_close_comm(&sdl_opens, q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  REGISTRATION AND INITIALIZATION
 *
 *  =========================================================================
 */

static struct qinit sdl_rinit = {
	.qi_putp = sdl_putp,		/* Read put (message from below) */
	.qi_srvp = sdl_srvp,		/* Read queue service */
	.qi_qopen = sdl_qopen,		/* Each open */
	.qi_qclose = sdl_qclose,	/* Last close */
	.qi_minfo = &sdl_minfo,		/* Information */
	.qi_mstat = &sdl_rstat,		/* Statistics */
};

static struct qinit sdl_winit = {
	.qi_putp = sdl_putp,		/* Write put (message from above) */
	.qi_srvp = sdl_srvp,		/* Write queue service */
	.qi_minfo = &sdl_minfo,		/* Information */
	.qi_mstat = &sdl_wstat,		/* Statistics */
};

static struct streamtab sdl_pmodinfo = {
	.st_rdinit = &sdl_rinit,	/* Upper read queue */
	.st_wrinit = &sdl_winit,	/* Upper write queue */
};

#ifdef LINUX
/*
 *  =========================================================================
 *
 *  LINUX INITIALIZATION
 *
 *  =========================================================================
 */

unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for SDL-PMOD module. (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static struct fmodsw sdl_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sdl_pmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

/**
 * sdl_pmodinit: - initialize SDL-PMOD
 */
static __init int
sdl_pmodinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&sdl_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

/**
 * sdl_pmodexit: - terminate SDL-PMOD
 */
static __exit void
sdl_pmodexit(void)
{
	int err;

	if ((err = unregister_strmod(&sdl_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
		return;
	}
	return;
}

module_init(sdl_pmodinit);
module_exit(sdl_pmodexit);

#endif				/* LINUX */
