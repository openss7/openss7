/*****************************************************************************

 @(#) $RCSfile: slpmod.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/08/15 05:20:19 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/15 05:20:19 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: slpmod.c,v $
 Revision 0.9.2.8  2007/08/15 05:20:19  brian
 - GPLv3 updates

 Revision 0.9.2.7  2007/08/12 16:20:30  brian
 - new PPA handling

 Revision 0.9.2.6  2007/08/06 04:43:57  brian
 - rework of pipe-based emulation modules

 Revision 0.9.2.5  2007/07/14 01:35:06  brian
 - make license explicit, add documentation

 Revision 0.9.2.4  2007/05/18 00:01:17  brian
 - check for nf_reset

 Revision 0.9.2.3  2007/03/25 19:00:15  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.2  2006/12/29 12:18:14  brian
 - old rpms hate nested ifs, release updates

 Revision 0.9.2.1  2006/12/27 16:35:55  brian
 - added slpmod module and fixups for make check target

 *****************************************************************************/

#ident "@(#) $RCSfile: slpmod.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/08/15 05:20:19 $"

static char const ident[] =
    "$RCSfile: slpmod.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/08/15 05:20:19 $";

/*
 *  This is SLPMOD, an SL module that is pushed over a pipe end to form an internal
 *  pseudo-signalling link.  Service primitives on the SL interface at one end of the pipe are
 *  translated into Service primitives on the SL interface at the other end of the pipe.  Although
 *  the primaly purpose of such as link is testing, it also has the abilty to provide internal
 *  signalling links for building virtual SS7 networks within a host.
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
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

#define SL_DESCRIP	"Signalling Link (SL) Pipe Module (SLPMOD) STREAMS MODULE."
#define SL_REVISION	"OpenSS7 $RCSfile: slpmod.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2007/08/15 05:20:19 $"
#define SL_COPYRIGHT	"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define SL_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SL_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SL_LICENSE	"GPL"
#define SL_BANNER	SL_DESCRIP	"\n" \
			SL_REVISION	"\n" \
			SL_COPYRIGHT	"\n" \
			SL_DEVICE	"\n" \
			SL_CONTACT	"\n"
#define SL_SPLASH	SL_DESCRIP	" - " \
			SL_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(SL_CONTACT);
MODULE_DESCRIPTION(SL_DESCRIP);
MODULE_SUPPORTED_DEVICE(SL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SL_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-slpmod");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define SLPMOD_MOD_ID		CONFIG_STREAMS_SLPMOD_MODID
#define SLPMOD_MOD_NAME		CONFIG_STREAMS_SLPMOD_NAME
#endif				/* LFS */

#ifndef SLPMOD_MOD_NAME
#define SLPMOD_MOD_NAME "sl-pmod"
#endif				/* SLPMOD_MOD_NAME */

#ifndef SLPMOD_MOD_ID
#define SLPMOD_MOD_ID 0
#endif				/* SLPMOD_MOD_ID */

/*
 *  =========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  =========================================================================
 */

#define MOD_ID		SLPMOD_MOD_ID
#define MOD_NAME	SLPMOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SL_BANNER
#else				/* MODULE */
#define MOD_BANNER	SL_SPLASH
#endif				/* MODULE */

static struct module_info sl_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat sl_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sl_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

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

struct sl_pair;

struct sl {
	struct sl_pair *pair;		/* thread pointer to pair */
	struct sl *other;		/* other private structure in pair */
	queue_t *oq;			/* output queue for this side */
	uint max_sdu;			/* maximum SDU size */
	uint min_sdu;			/* minimum SDU size */
	uint header_len;		/* size of Level 1 header */
	uint ppa_style;			/* PPA style */
	struct st state, oldstate;
	mblk_t *t1;
	bufq_t tb;
	bufq_t rb;
	struct lmi_option option;	/* LMI protocol and variant options */
	struct {
		struct sdl_config config;	/* SDL configuration options */
		struct sdl_notify notify;	/* SDL notification options */
		struct sdl_statem statem;	/* SDL state machine variables */
		struct sdl_stats statsp;	/* SDL statistics periods */
		struct sdl_stats stats;	/* SDL statistics */
	} sdl;
	struct {
		struct sdt_config config;	/* SDT configuration */
		struct sdt_notify notify;	/* SDT events */
		struct sdt_statem statem;	/* SDT state machine */
		struct sdt_stats statsp;	/* SDT statistics periods */
		struct sdt_stats stats;	/* SDT statistics */
	} sdt;
	struct {
		struct sl_config config;	/* SL configuration */
		struct sl_notify notify;	/* SL events */
		struct sl_statem statem;	/* SL state machine */
		struct sl_stats statsp;	/* SL statistics periods */
		struct sl_stats stats;	/* SL statistics */
	} sl;
};

struct sl_pair {
	struct sl r_priv;
	struct sl w_priv;
};

#define PRIV(q)		((struct sl_pair *)(q)->q_ptr)
#define SL_PRIV(q)	(((q)->q_flag & QREADR) ? &PRIV(q)->r_priv : &PRIV(q)->w_priv)

#define STRLOGNO	0	/* log Stream errors */
#define STRLOGIO	1	/* log Stream input-output */
#define STRLOGST	2	/* log Stream state transitions */
#define STRLOGTO	3	/* log Stream timeouts */
#define STRLOGRX	4	/* log Stream primitives received */
#define STRLOGTX	5	/* log Stream primitives issued */
#define STRLOGTE	6	/* log Stream timer events */
#define STRLOGDA	7	/* log Stream data */

/**
 * sdt_iocname: display SDT ioctl command name
 * @cmd: ioctl command
 */
static const char *
sl_iocname(int cmd)
{
	switch (_IOC_TYPE(cmd)) {
#if 1
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
#if 1
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
 * sl_primname: display SL primitive name
 * @prim: the primtitive to display
 */
static const char *
sl_primname(sl_ulong prim)
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
#if 0
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
#if 1
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
 * sl_l_state_name: display LMI state name
 * @state: the state value to display
 */
static const char *
sl_l_state_name(int state)
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

static const char *
sl_i_state_name(int state)
{
	switch (state) {
	case SLS_POWER_OFF:
		return ("SLS_POWER_OFF");
	case SLS_OUT_OF_SERVICE:
		return ("SLS_OUT_OF_SERVICE");
	case SLS_NOT_ALIGNED:
		return ("SLS_NOT_ALIGNED");
	case SLS_INITIAL_ALIGNMENT:
		return ("SLS_INITIAL_ALIGNMENT");
	case SLS_PROVING:
		return ("SLS_PROVING");
	case SLS_ALIGNED_READY:
		return ("SLS_ALIGNED_READY");
	case SLS_ALIGNED_NOT_READY:
		return ("SLS_ALIGNED_NOT_READY");
	case SLS_IN_SERVICE:
		return ("SLS_IN_SERVICE");
	case SLS_PROCESSOR_OUTAGE:
		return ("SLS_PROCESSOR_OUTAGE");
	default:
		return ("SL_????");
	}
}

static const char *
sl_flags_set(int mask)
{
	switch (mask & (SLF_LOC_PROC_OUT | SLF_REM_PROC_OUT | SLF_EMERGENCY)) {
	case 0:
		return ("");
	case SLF_LOC_PROC_OUT:
		return ("(LOC)");
	case SLF_REM_PROC_OUT:
		return ("(REM)");
	case SLF_EMERGENCY:
		return ("(EMERG)");
	case SLF_LOC_PROC_OUT | SLF_REM_PROC_OUT:
		return ("(LOC|REM)");
	case SLF_LOC_PROC_OUT | SLF_EMERGENCY:
		return ("(LOC|EMERG)");
	case SLF_LOC_PROC_OUT | SLF_REM_PROC_OUT | SLF_EMERGENCY:
		return ("(LOC|REM|EMERG)");
	default:
		return ("(error)");
	}
}

/*
 * ==========================================================================
 * STATE TRANSITIONS
 * ==========================================================================
 */

/**
 * sl_get_l_state: - get management state for private structure
 * @sl: private structure
 */
static int
sl_get_l_state(struct sl *sl)
{
	return (sl->state.l_state);
}

/**
 * sl_set_l_state: - set management state for private structure
 * @sl: private structure
 * @newstate: new state
 */
static int
sl_set_l_state(struct sl *sl, int newstate)
{
	int oldstate = sl->state.l_state;

	if (newstate != oldstate) {
		sl->state.l_state = newstate;
		mi_strlog(sl->oq, STRLOGST, SL_TRACE, "%s <- %s", sl_l_state_name(newstate),
			  sl_l_state_name(oldstate));
	}
	return (newstate);
}

static int
sl_save_l_state(struct sl *sl)
{
	return ((sl->oldstate.l_state = sl_get_l_state(sl)));
}

static int
sl_restore_l_state(struct sl *sl)
{
	return sl_set_l_state(sl, sl->oldstate.l_state);
}

/**
 * sl_get_i_state: - get interface state for private structure
 * @sl: private structure
 */
static int
sl_get_i_state(struct sl *sl)
{
	return (sl->state.i_state);
}

/**
 * sl_set_i_state: - set interface state for private structure
 * @sl: private structure
 * @newstate: new state
 */
static int
sl_set_i_state(struct sl *sl, int newstate)
{
	int oldstate = sl_get_i_state(sl);

	if (newstate != oldstate) {
		sl->state.i_state = newstate;
		mi_strlog(sl->oq, STRLOGST, SL_TRACE, "%s <- %s", sl_i_state_name(newstate),
			  sl_i_state_name(oldstate));
	}
	return (newstate);
}

static int
sl_save_i_state(struct sl *sl)
{
	return ((sl->oldstate.l_state = sl_get_i_state(sl)));
}

static int
sl_restore_i_state(struct sl *sl)
{
	return sl_set_i_state(sl, sl->oldstate.l_state);
}

/**
 * sl_save_state: - reset state for private structure
 * @sl: private structure
 */
static void
sl_save_state(struct sl *sl)
{
	sl_save_l_state(sl);
	sl_save_i_state(sl);
}

/**
 * sl_restore_state: - reset state for private structure
 * @sl: private structure
 */
static int
sl_restore_state(struct sl *sl)
{
	sl_restore_i_state(sl);
	return sl_restore_l_state(sl);
}

/**
 * sl_get_flags: - get flags for private structure
 * @sl: private structure
 */
static int
sl_get_flags(struct sl *sl)
{
	return (sl->state.i_flags);
}

/**
 * sl_set_flags: - set flags for private structure
 * @sl: private structure
 * @newflags: new flags
 */
static int
sl_set_flags(struct sl *sl, int mask)
{
	int oldflags = sl_get_flags(sl), newflags = oldflags |= mask;

	if (oldflags != newflags)
		mi_strlog(sl->oq, STRLOGST, SL_TRACE, "%s",
			  sl_flags_set((newflags ^ oldflags) & mask));
	return (sl->state.i_flags = newflags);
}

static inline int
sl_or_flags(struct sl *sl, int orflags)
{
	return (sl->state.i_flags |= orflags);
}

static inline int
sl_nand_flags(struct sl *sl, int nandflags)
{
	return (sl->state.i_flags &= ~nandflags);
}

/**
 * sl_save_flags: - reset flags for private structure
 * @sl: private structure
 */
static void
sl_save_flags(struct sl *sl)
{
	sl->oldstate.i_flags = sl->state.i_flags;
}

/**
 * sl_restore_flags: - reset flags for private structure
 * @sl: private structure
 */
static int
sl_restore_flags(struct sl *sl)
{
	return sl_set_flags(sl, sl->oldstate.i_flags);
}

static void
sl_save_total_state(struct sl *sl)
{
	sl_save_flags(sl->other);
	sl_save_state(sl->other);
	sl_save_flags(sl);
	sl_save_state(sl);
}

static int
sl_restore_total_state(struct sl *sl)
{
	sl_restore_flags(sl->other);
	sl_restore_state(sl->other);
	sl_restore_flags(sl);
	return sl_restore_state(sl);
}

static inline int
sl_clr_flags(struct sl *sl, int mask)
{
	int oldflags = sl_get_flags(sl), newflags = oldflags &= ~mask;

	if (oldflags != newflags)
		mi_strlog(sl->oq, STRLOGST, SL_TRACE, "~%s",
			  sl_flags_set((newflags ^ oldflags) & mask));
	return (sl->state.i_flags = newflags);
}

static void
sl_timer_start(struct sl *sl, int timer, unsigned long value)
{
	switch (timer) {
	case 1:
		mi_timer(sl->oq, sl->t1, value);
		break;
	default:
		break;
	}
}
static void
sl_timer_stop(struct sl *sl, int timer)
{
	switch (timer) {
	case 1:
		mi_timer_cancel(sl->t1);
		break;
	default:
		break;
	}
}

/*
 *  =========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  SL Provider -> SL User primitives.
 *
 *  =========================================================================
 */
#if 0
/**
 * m_error: - issue M_ERROR for stream
 * @s: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @err: error to indicate
 */
static int
m_error(struct sl *s, queue_t *q, mblk_t *msg, int err)
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
lmi_info_ack(struct sl *s, queue_t *q, mblk_t *msg)
{
	lmi_info_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = LMI_CURRENT_VERSION;
		p->lmi_state = sl_get_l_state(s);
		p->lmi_max_sdu = s->max_sdu;
		p->lmi_min_sdu = s->min_sdu;
		p->lmi_header_len = s->header_len;
		p->lmi_ppa_style = LMI_STYLE1;
		p->lmi_ppa_length = 0;
		p->lmi_ppa_offset = sizeof(*p);
		p->lmi_prov_flags = s->state.i_flags;	/* FIXME maintain provider flags */
		p->lmi_prov_state = s->state.i_state;	/* FIXME maintain provider state */
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
static inline int
lmi_ok_ack(struct sl *s, queue_t *q, mblk_t *msg, int prim)
{
	lmi_ok_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (sl_get_l_state(s)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = sl_set_l_state(s, LMI_DISABLED);
			break;
		case LMI_DETACH_PENDING:
			p->lmi_state = sl_set_l_state(s, LMI_UNATTACHED);
			break;
		default:
			/* FIXME: log error */
			p->lmi_state = sl_get_l_state(s);
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
lmi_error_ack(struct sl *s, queue_t *q, mblk_t *msg, int prim, int error)
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
		p->lmi_state = sl_restore_l_state(s);
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
static inline int
lmi_error_reply(struct sl *s, queue_t *q, mblk_t *msg, int prim, int error)
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
lmi_enable_con(struct sl *s, queue_t *q, mblk_t *msg)
{
	lmi_enable_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = sl_set_l_state(s, LMI_ENABLED);
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
lmi_disable_con(struct sl *s, queue_t *q, mblk_t *msg)
{
	lmi_disable_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = sl_set_l_state(s, LMI_DISABLED);
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
lmi_optmgmt_ack(struct sl *s, queue_t *q, mblk_t *msg, caddr_t opt_ptr, size_t opt_len, uint flags)
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
lmi_error_ind(struct sl *s, queue_t *q, mblk_t *msg, int error, uint state)
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
			p->lmi_state = sl_set_l_state(s, state);
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
 */
static inline int
lmi_stats_ind(struct sl *s, queue_t *q, mblk_t *msg, lmi_ulong itvl)
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
lmi_event_ind(struct sl *s, queue_t *q, mblk_t *msg, uint oid, uint severity, caddr_t inf_ptr,
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

/**
 * sl_pdu_ind: - issue SL_PDU_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data
 */
static inline int
sl_pdu_ind(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong pri, mblk_t *dp)
{
	sl_pdu_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(sl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_PDU_IND;
			p->sl_mp = pri;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_PDU_IND");
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_link_congested_ind: - issue SL_LINK_CONGESTED_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
sl_link_congested_ind(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong cong, sl_ulong disc)
{
	sl_link_cong_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (bcanputnext(sl->oq, 1)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LINK_CONGESTED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			p->sl_cong_status = cong;
			p->sl_disc_status = disc;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_LINK_CONGESTED_IND");
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_linK_congestion_ceased_ind: - issue SL_LINK_CONGESTION_CEASED_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
sl_link_congestion_ceased_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_link_cong_ceased_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (bcanputnext(sl->oq, 1)) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->sl_primitive = SL_LINK_CONGESTION_CEASED_IND;
			p->sl_timestamp = drv_hztomsec(jiffies);
			p->sl_cong_status = 0;
			p->sl_disc_status = 0;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_LINK_CONGESTION_CEASED_IND");
			putnext(sl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieved_message_ind: - issue SL_RETRIEVED_MESSAGE_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_retrieved_message_ind(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong pri, mblk_t *dp)
{
	sl_retrieved_msg_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVED_MESSAGE_IND;
		p->sl_mp = pri;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		if (msg && msg->b_cont == dp)
			msg->b_cont = NULL;
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_RETRIEVED_MESSAGE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieval_complete_ind: - issue SL_RETRIEVAL_COMPLETE_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_retrieval_complete_ind(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong pri, mblk_t *dp)
{
	sl_retrieval_comp_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVAL_COMPLETE_IND;
		p->sl_mp = pri;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_RETRIEVAL_COMPLETE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_rb_cleared_ind: - issue SL_RB_CLEARED_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_rb_cleared_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_rb_cleared_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RB_CLEARED_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_RB_CLEARED_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_bsnt_ind: - issue SL_BSNT_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
sl_bsnt_ind(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong bsnt)
{
	sl_bsnt_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_BSNT_IND;
		p->sl_bsnt = bsnt;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_BSNT_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_in_service_ind: - issue SL_IN_SERVICE_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_in_service_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_in_service_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_IN_SERVICE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_IN_SERVICE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_out_of_service_ind: - issue SL_OUT_OF_SERVICE_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_out_of_service_ind(struct sl *sl, queue_t *q, mblk_t *msg, sl_ulong reason)
{
	sl_out_of_service_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_OUT_OF_SERVICE_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		p->sl_reason = reason;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_OUT_OF_SERVICE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_remote_processor_outage_ind: - issue SL_REMOTE_PROCESSOR_OUTAGE_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_remote_processor_outage_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_rem_proc_out_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_REMOTE_PROCESSOR_OUTAGE_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_REMOTE_PROCESSOR_OUTAGE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_remote_processor_recovered_ind: - issue SL_REMOTE_PROCESSOR_RECOVERED_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_remote_processor_recovered_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_rem_proc_recovered_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_REMOTE_PROCESSOR_RECOVERED_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_REMOTE_PROCESSOR_RECOVERED_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_rtb_cleared_ind: - issue SL_RTB_CLEARED_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_rtb_cleared_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_rtb_cleared_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RTB_CLEARED_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_RTB_CLEARED_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_retrieval_not_possible_ind: - issue SL_RETRIEVAL_NOT_POSSIBLE_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_retrieval_not_possible_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_retrieval_not_poss_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVAL_NOT_POSSIBLE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_RETRIEVAL_NOT_POSSIBLE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_bsnt_not_retrievable_ind: - issue SL_BSNT_NOT_RETRIEVABLE_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_bsnt_not_retrievable_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_bsnt_not_retr_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_BSNT_NOT_RETRIEVABLE_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_BSNT_NOT_RETRIEVABLE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#if 0
/**
 * sl_optmgmt_ack: - issue SL_OPTMGMT_ACK primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_optmgmt_ack(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_optmgmt_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_OPTMGMT_ACK;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_OPTMGMT_ACK");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_notify_ind: - issue SL_NOTIFY_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static int
sl_notify_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_notify_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_NOTIFY_IND;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_NOTIFY_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif
/**
 * sl_local_processor_outage_ind: - issue SL_LOCAL_PROCESSOR_OUTAGE_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
sl_local_processor_outage_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_loc_proc_out_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_LOCAL_PROCESSOR_OUTAGE_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_LOCAL_PROCESSOR_OUTAGE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * sl_local_processor_recovered_ind: - issue SL_LOCAL_PROCESSOR_RECOVERED_IND primitive
 * @sl: output signalling link
 * @q: active queue
 * @msg: message to free upon success
 */
static inline int
sl_local_processor_recovered_ind(struct sl *sl, queue_t *q, mblk_t *msg)
{
	sl_loc_proc_recovered_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_LOCAL_PROCESSOR_RECOVERED_IND;
		p->sl_timestamp = drv_hztomsec(jiffies);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_LOCAL_PROCESSOR_RECOVERED_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  =========================================================================
 *
 *  SL User -> SL Provider Primitives
 *
 *  =========================================================================
 */

/**
 * lmi_info_req: - process LMI_INFO_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the LMI_INFO_REQ primitive
 */
static int
lmi_info_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return lmi_info_ack(sl, q, mp);
}

/**
 * lmi_attach_req: - process LMI_ATTACH_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the LMI_ATTACH_REQ primitive
 *
 * SL-PMOD is always an LMI_STYLE1 module.  LMI_ATTACH_REQ is not supported.
 */
static int
lmi_attach_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return lmi_error_ack(sl, q, mp, LMI_ATTACH_REQ, LMI_NOTSUPP);
}

/**
 * lmi_detach_req: - process LMI_DETACH_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the LMI_DETACH_REQ primitive
 *
 * SL-PMOD is always an LMI_STYLE1 module. LMI_DETACH_REQ is not supported.
 */
static int
lmi_detach_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return lmi_error_ack(sl, q, mp, LMI_DETACH_REQ, LMI_NOTSUPP);
}

/**
 * lmi_enable_req: - process LMI_ENABLE_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the LMI_ENABLE_REQ primitive
 *
 * When requested, enable the local signalling link.
 */
static int
lmi_enable_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(sl) != LMI_DISABLED)
		goto outstate;
	sl_set_l_state(sl, LMI_ENABLE_PENDING);
	return lmi_enable_con(sl, q, mp);
      outstate:
	return lmi_error_ack(sl, q, mp, LMI_ENABLE_REQ, LMI_OUTSTATE);
}

/**
 * lmi_disable_req: - process LMI_DISABLE_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the LMI_DISABLE_REQ primitive
 *
 * When requested, disable the local signalling link.
 */
static int
lmi_disable_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	sl_set_l_state(sl, LMI_DISABLE_PENDING);
	return lmi_disable_con(sl, q, mp);
      outstate:
	return lmi_error_ack(sl, q, mp, LMI_DISABLE_REQ, LMI_OUTSTATE);
}

/**
 * lmi_optmgmt_req: - process LMI_OPTMGMT_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the LMI_OPTMGMT_REQ primitive
 *
 * Not supported for now.
 */
static int
lmi_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	return lmi_error_ack(sl, q, mp, LMI_OPTMGMT_REQ, LMI_NOTSUPP);
}

/**
 * sl_pdu_req: - process SDL_PDU_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SDL_PDU_REQ primitive
 *
 * Whenever the signalling link is not in service, simply put the message back on the queue, disable
 * the queue, and wait for some other event to happend.
 */
static int
sl_pdu_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct sl *so = sl->other;

	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	if (sl_get_i_state(sl) != SLS_IN_SERVICE) {
		bufq_queue(&sl->tb, mp);
		return (0);
	}
	if (bcanputnext(so->oq, mp->b_band)) {
		*(sl_ulong *) mp->b_rptr = SL_PDU_IND;
		putnext(so->oq, mp);
		return (0);
	}
	return (-EBUSY);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_PDU_REQ, LMI_OUTSTATE);
}

/**
 * sl_emergency_req: - process SL_EMERGENCY_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_EMERGENCY_REQ primitive
 */
static int
sl_emergency_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	sl_set_flags(sl, SLF_EMERGENCY);
      outstate:
	freemsg(mp);
	return (0);
}

/**
 * sl_emergency_ceases_req: - process SL_EMERGENCY_CEASES_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_EMERGENCY_CEASES_REQ primitive
 */
static int
sl_emergency_ceases_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	sl_clr_flags(sl, SLF_EMERGENCY);
      outstate:
	freemsg(mp);
	return (0);
}

/**
 * sl_start_req: - process SL_START_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_START_REQ primitive
 */
static int
sl_start_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct sl *so = sl->other;
	int err;

	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	switch (sl_get_i_state(sl)) {
	case SLS_POWER_OFF:
		goto outstate;
	case SLS_OUT_OF_SERVICE:
		sl_set_i_state(sl, SLS_INITIAL_ALIGNMENT);
		sl_timer_start(sl, 1, 20000);
		switch (sl_get_i_state(so)) {
		case SLS_POWER_OFF:
			break;
		case SLS_INITIAL_ALIGNMENT:
			sl_timer_stop(sl, 1);
			if (sl_get_flags(sl) & (SLF_LOC_PROC_OUT | SLF_REM_PROC_OUT)) {
				sl_set_i_state(sl, SLS_ALIGNED_NOT_READY);
				if ((err = sl_remote_processor_outage_ind(so, q, mp)))
					return (err);
			} else {
				sl_set_i_state(sl, SLS_ALIGNED_READY);
				if ((err = sl_in_service_ind(so, q, mp)))
					return (err);
			}
			if (sl_get_flags(so) & (SLF_LOC_PROC_OUT | SLF_REM_PROC_OUT)) {
				sl_set_i_state(so, SLS_ALIGNED_NOT_READY);
				if ((err = sl_remote_processor_outage_ind(sl, q, mp)))
					return (err);
			} else {
				sl_set_i_state(so, SLS_ALIGNED_READY);
				if ((err = sl_in_service_ind(sl, q, mp)))
					return (err);
			}
			break;
		case SLS_ALIGNED_READY:
		case SLS_ALIGNED_NOT_READY:
		case SLS_IN_SERVICE:
		default:
			goto outstate;
		}
		break;
	case SLS_INITIAL_ALIGNMENT:
	case SLS_ALIGNED_READY:
	case SLS_ALIGNED_NOT_READY:
	case SLS_IN_SERVICE:
		goto discard;
	default:
		goto outstate;
	}
      discard:
      outstate:
	freemsg(mp);
	return (0);
}

/**
 * sl_stop_req: - process SL_STOP_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_STOP_REQ primitive
 */
static int
sl_stop_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct sl *so = sl->other;
	int err;

	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	switch (sl_get_i_state(sl)) {
	case SLS_POWER_OFF:
		goto discard;
	case SLS_OUT_OF_SERVICE:
		goto discard;
	case SLS_INITIAL_ALIGNMENT:
	case SLS_ALIGNED_READY:
	case SLS_ALIGNED_NOT_READY:
	case SLS_IN_SERVICE:
	default:
		sl_timer_stop(sl, 1);
		sl_set_i_state(sl, SLS_OUT_OF_SERVICE);
		switch (sl_get_i_state(so)) {
		case SLS_POWER_OFF:
		case SLS_OUT_OF_SERVICE:
			break;
		case SLS_INITIAL_ALIGNMENT:
		case SLS_ALIGNED_READY:
		case SLS_ALIGNED_NOT_READY:
		case SLS_IN_SERVICE:
		default:
			sl_timer_stop(so, 1);
			sl_set_i_state(so, SLS_OUT_OF_SERVICE);
			if ((err = sl_out_of_service_ind(so, q, mp, 0)))
				return (err);
			break;
		}
		break;
	}
      discard:
      outstate:
	freemsg(mp);
	return (0);
}

/**
 * sl_retrieve_bsnt_req: - process SL_RETRIEVE_BSNT_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_RETRIEVE_BSNT_REQ primitive
 */
static int
sl_retrieve_bsnt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	return sl_bsnt_not_retrievable_ind(sl, q, mp);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_RETRIEVE_BSNT_REQ, LMI_OUTSTATE);
}

/**
 * sl_retrieval_request_and_fsnc_req: - process SL_RETRIEVAL_REQUEST_AND_FSNC_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_RETRIEVAL_REQUEST_AND_FSNC_REQ primitive
 *
 * Because there is no retranmission buffer in this implemetnation, the FSNC
 * can be ignored.  We do, however, have a transmission buffer, the contents
 * of which can be retrieved.
 */
static int
sl_retrieval_request_and_fsnc_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	mblk_t *bp;
	int err;

	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	if ((bp = bufq_head(&sl->tb))) {
		while ((bp = bufq_dequeue(&sl->tb))) {
			sl_pdu_req_t *p = (typeof(p)) bp->b_rptr;

			if ((err = sl_retrieved_message_ind(sl, q, bp, p->sl_mp, bp->b_cont))) {
				bufq_queue_head(&sl->tb, bp);
				return (err);
			}
		}
		return sl_retrieval_complete_ind(sl, q, mp, 0, NULL);
	}
	return sl_retrieval_not_possible_ind(sl, q, mp);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_RETRIEVAL_REQUEST_AND_FSNC_REQ, LMI_OUTSTATE);
}

/**
 * sl_clear_buffers_req: - process SL_CLEAR_BUFFERS_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_CLEAR_BUFFERS_REQ primitive
 */
static int
sl_clear_buffers_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	bufq_purge(&sl->tb);	/* purge the transmission buffer */
	bufq_purge(&sl->rb);	/* purge the receive buffer */
	return sl_rb_cleared_ind(sl, q, mp);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_CLEAR_BUFFERS_REQ, LMI_OUTSTATE);
}

/**
 * sl_clear_rtb_req: - process SL_CLEAR_RTB_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_CLEAR_RTB_REQ primitive
 */
static int
sl_clear_rtb_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	/* do not have a retransmission buffer */
	return sl_rtb_cleared_ind(sl, q, mp);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_CLEAR_RTB_REQ, LMI_OUTSTATE);
}

/**
 * sl_continue_req: - process SL_CONTINUE_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_CONTINUE_REQ primitive
 */
static int
sl_continue_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct sl *so = sl->other;

	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	if (!(sl_get_flags(sl) & SLF_LOC_PROC_OUT))
		goto discard;
	sl_clr_flags(sl, SLF_LOC_PROC_OUT);
	if (sl_get_i_state(sl) == SLS_ALIGNED_NOT_READY) {
		sl_set_i_state(sl, SLS_IN_SERVICE);
		if (sl_get_i_state(so) == SLS_ALIGNED_READY) {
			sl_set_i_state(so, SLS_IN_SERVICE);
			return sl_in_service_ind(so, q, mp);
		}
	}
	return sl_remote_processor_recovered_ind(so, q, mp);
      discard:
	freemsg(mp);
	return (0);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_CONTINUE_REQ, LMI_OUTSTATE);
}

/**
 * sl_local_processor_outage_req: - process SL_LOCAL_PROCESSOR_OUTAGE_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_LOCAL_PROCESSOR_OUTAGE_REQ primitive
 */
static int
sl_local_processor_outage_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct sl *so = sl->other;

	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	if ((sl_get_flags(sl) & SLF_LOC_PROC_OUT))
		goto discard;
	sl_set_flags(sl, SLF_LOC_PROC_OUT);
	sl_set_flags(so, SLF_REM_PROC_OUT);
	return sl_remote_processor_outage_ind(so, q, mp);
      discard:
	freemsg(mp);
	return (0);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_LOCAL_PROCESSOR_OUTAGE_REQ, LMI_OUTSTATE);
}

/**
 * sl_resume_req: - process SL_RESUME_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_RESUME_REQ primitive
 */
static int
sl_resume_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct sl *so = sl->other;

	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	if (!(sl_get_flags(sl) & SLF_LOC_PROC_OUT))
		goto discard;
	sl_clr_flags(sl, SLF_LOC_PROC_OUT);
	if (sl_get_i_state(sl) == SLS_ALIGNED_NOT_READY) {
		sl_set_i_state(sl, SLS_IN_SERVICE);
		if (sl_get_i_state(so) == SLS_ALIGNED_READY) {
			sl_set_i_state(so, SLS_IN_SERVICE);
			return sl_in_service_ind(so, q, mp);
		}
	}
	return sl_remote_processor_recovered_ind(so, q, mp);
      discard:
	freemsg(mp);
	return (0);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_RESUME_REQ, LMI_OUTSTATE);
}

/**
 * sl_congestion_discard_req: - process SL_CONGESTION_DISCARD_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_CONGESTION_DISCARD_REQ primitive
 */
static int
sl_congestion_discard_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	sl_set_flags(sl, SLF_CONG_DISCARD);
	freemsg(mp);
	return (0);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_CONGESTION_DISCARD_REQ, LMI_OUTSTATE);
}

/**
 * sl_congestion_accept_req: - process SL_CONGESTION_ACCEPT_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_CONGESTION_ACCEPT_REQ primitive
 */
static int
sl_congestion_accept_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	sl_set_flags(sl, SLF_CONG_ACCEPT);
	freemsg(mp);
	return (0);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_CONGESTION_ACCEPT_REQ, LMI_OUTSTATE);
}

/**
 * sl_no_congestion_req: - process SL_NO_CONGESTION_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_NO_CONGESTION_REQ primitive
 */
static int
sl_no_congestion_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct sl *so = sl->other;

	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	sl_clr_flags(sl, (SLF_CONG_DISCARD | SLF_CONG_ACCEPT));
	canenable(so->oq);
	qenable(so->oq);
	freemsg(mp);
	return (0);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_NO_CONGESTION_REQ, LMI_OUTSTATE);
}

/**
 * sl_power_on_req: - process SL_POWER_ON_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_POWER_ON_REQ primitive
 */
static int
sl_power_on_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	if (sl_get_i_state(sl) != SLS_POWER_OFF)
		goto outstate;
	sl_set_i_state(sl, SLS_OUT_OF_SERVICE);
	freemsg(mp);
	return (0);
      outstate:
	return lmi_error_ack(sl, q, mp, SL_POWER_ON_REQ, LMI_OUTSTATE);
}

/**
 * sl_optmgmt_req: - process SL_OPTMGMT_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_OPTMGMT_REQ primitive
 */
static int
sl_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * sl_notify_req: - process SL_NOTIFY_REQ primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the SL_NOTIFY_REQ primitive
 */
static int
sl_notify_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * lmi_other_req: - process unrecognized primitive
 * @sl: input signalling link
 * @q: active queue
 * @mp: the unrecognized primitive
 */
static int
lmi_other_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/*
 *  =========================================================================
 *
 *  TIMEOUTS
 *
 *  =========================================================================
 */

static int
sl_t1_timeout(struct sl *sl, queue_t *q)
{
	if (sl_get_l_state(sl) != LMI_ENABLED)
		goto outstate;
	if (sl_get_i_state(sl) != SLS_INITIAL_ALIGNMENT)
		goto outstate;
	sl_set_i_state(sl, SLS_OUT_OF_SERVICE);
	return sl_out_of_service_ind(sl, q, NULL, 0);
      outstate:
	return (0);
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
lmi_testoption(struct sl *s, struct lmi_option *arg)
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
lmi_ioctl(struct sl *s, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *bp;
	int size = -1;
	int err = 0;

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", sl_iocname(ioc->ioc_cmd));

	sl_save_total_state(s);

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
		sl_restore_total_state(s);
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
lmi_copyin(struct sl *s, queue_t *q, mblk_t *mp, mblk_t *dp)
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
 * lmi_copyout: - process LMI M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is the final step which is a simple copy done operation.
 */
static int
lmi_copyout(struct sl *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sl_iocname(cp->cp_cmd));
	mi_copyout(q, mp);
	return (0);
}

/**
 * sdl_testconfig: - test SDL configuration for validity
 * @s: private structure
 * @arg: configuration to test
 */
static int
sdl_testconfig(struct sl *s, struct sdl_config *arg)
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
sdl_setnotify(struct sl *s, struct sdl_notify *arg)
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
sdl_clrnotify(struct sl *s, struct sdl_notify *arg)
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
sdl_ioctl(struct sl *s, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *bp;
	int size = -1;
	int err = 0;

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", sl_iocname(ioc->ioc_cmd));

	sl_save_total_state(s);

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
		sl_restore_total_state(s);
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
sdl_copyin(struct sl *s, queue_t *q, mblk_t *mp, mblk_t *dp)
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
 * sdl_copyout: - process SDL M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is the final step which is a simple copy done operation.
 */
static int
sdl_copyout(struct sl *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sl_iocname(cp->cp_cmd));
	mi_copyout(q, mp);
	return (0);
}

#if 1
/**
 * sdt_testconfig: - test SDT configuration
 * @s: (locked) private structure
 * @arg: configuration argument
 */
static int
sdt_testconfig(struct sl *s, struct sdt_config *arg)
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
sdt_setnotify(struct sl *s, struct sdt_notify *arg)
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
sdt_clrnotify(struct sl *s, struct sdt_notify *arg)
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
sdt_ioctl(struct sl *s, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *bp;
	int size = -1;
	int err = 0;

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", sl_iocname(ioc->ioc_cmd));

	sl_save_total_state(s);
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
sdt_copyin(struct sl *s, queue_t *q, mblk_t *mp, mblk_t *dp)
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
 * sdt_copyout: - process SDT M_IOCDATA message
 * @s: private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is the final step which is a simple copy done operation.
 */
static int
sdt_copyout(struct sl *s, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sl_iocname(cp->cp_cmd));
	mi_copyout(q, mp);
	return (0);
}
#endif

#if 1
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
	| SL_EVT_FAIL_UNSPECIFIED \
	| SL_EVT_FAIL_CONG_TIMEOUT \
	| SL_EVT_FAIL_ACK_TIMEOUT \
	| SL_EVT_FAIL_ABNORMAL_BSNR \
	| SL_EVT_FAIL_ABNORMAL_FIBR \
	| SL_EVT_FAIL_ABORMAL \
	| SL_EVT_FAIL_SUERM_EIM \
	| SL_EVT_FAIL_ALIGNMENT \
	| SL_EVT_FAIL_RECEIVED_SIO \
	| SL_EVT_FAIL_RECEIVED_SIN \
	| SL_EVT_FAIL_RECEIVED_SIE \
	| SL_EVT_FAIL_RECEIVED_SIOS \
	| SL_EVT_FAIL_T1_TIMEOUT \
	| SL_EVT_RPO_BEGIN \
	| SL_EVT_RPO_END \
	| SL_EVT_CONGEST_ONSET_IND \
	| SL_EVT_CONGEST_DISCD_IND \
	| SL_EVT_FAIL_ALL_REASONS \
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
 * sl_do_ioctl: - process M_IOCTL message
 * @ch: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCTL message
 *
 * This is step 1 of the input-output control operation.  Step 1 consists
 * of a copyin operation for SET operations and a copyout operation for GET
 * operations.
 */
static int
sl_do_ioctl(struct sl *s, queue_t *q, mblk_t *mp)
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
#if 1
	case SDT_IOC_MAGIC:
		return sdt_ioctl(s, q, mp);
#endif
#if 1
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
 * sl_do_copyin: - process M_IOCDATA message
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
sl_do_copyin(struct sl *s, queue_t *q, mblk_t *mp, mblk_t *dp)
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
#if 1
	case SDT_IOC_MAGIC:
		return sdt_copyin(s, q, mp, dp);
#endif
#if 1
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
 * sl_do_copyout: - process M_IOCDATA message
 * @s: (locked) private structure
 * @q: active queue
 * @mp: the M_IOCDATA message
 * @dp: data part
 *
 * This is the final stop which is a simple copy done operation.
 */
static int
sl_do_copyout(struct sl *s, queue_t *q, mblk_t *mp, mblk_t *dp)
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
#if 1
	case SDT_IOC_MAGIC:
		return sdt_copyout(s, q, mp, dp);
#endif
#if 1
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
 * __sl_m_data: - process M_DATA message
 * @s: (locked) private structure
 * @q: active queue
 * @mp: the M_DATA message
 */
static fastcall int
__sl_m_data(struct sl *s, queue_t *q, mblk_t *mp)
{
	return sl_pdu_ind(s->other, q, mp, 0, mp->b_cont);
}

/**
 * sl_m_data: - process M_DATA message
 * @q: active queue
 * @mp: the M_DATA message
 */
static inline fastcall int
sl_m_data(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if (likely((priv = mi_trylock(q)) != NULL)) {
		err = __sl_m_data(SL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

/**
 * sl_m_proto_slow: - process M_(PC)PROTO message
 * @s: private structure
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 * @prim: the primitive
 */
static fastcall int
sl_m_proto_slow(struct sl *s, queue_t *q, mblk_t *mp, lmi_ulong prim)
{
	int rtn;

	switch (prim) {
	case SL_PDU_REQ:
		mi_strlog(s->oq, STRLOGDA, SL_TRACE, "-> %s", sl_primname(prim));
		break;
	default:
		mi_strlog(s->oq, STRLOGRX, SL_TRACE, "-> %s", sl_primname(prim));
		break;
	}

	sl_save_total_state(s);

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
#if 0
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
#if 1
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
		sl_restore_total_state(s);
	return (rtn);
}

/**
 * __sl_m_proto: - process M_(PC)PROTO message
 * @s: locked private structure
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 */
static inline fastcall int
__sl_m_proto(struct sl *s, queue_t *q, mblk_t *mp)
{
	t_uscalar_t prim;
	int rtn;

	if (likely(MBLKIN(mp, 0, sizeof(prim)))) {
		sl_ulong prim = *(typeof(prim) *) mp->b_rptr;

		if (likely(prim == SL_PDU_REQ)) {
#ifndef _OPTIMIZE_SPEED
			mi_strlog(s->oq, STRLOGDA, SL_TRACE, "-> SL_PDU_REQ");
#endif				/* _OPTIMIZE_SPEED */
			rtn = sl_pdu_req(s, q, mp);
		} else {
			rtn = sl_m_proto_slow(s, q, mp, prim);
		}
	} else {
		freemsg(mp);
		rtn = 0;
	}
	return (rtn);
}

/**
 * sl_m_proto: = process M_(PC)PROTO message
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 */
static inline fastcall int
sl_m_proto(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if (likely((priv = mi_trylock(q)) != NULL)) {
		err = __sl_m_proto(SL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

/**
 * __sl_m_sig: process M_(PC)SIG message
 * @s: (locked) private structure
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 *
 * This is the method for processing tick timers.  Under the tick approach
 * each time that the tick timer fires we reset the timer.  Then we prepare as
 * many blocks as will fit in the interval and send them to the other side.
 */
static inline fastcall int
__sl_m_sig(struct sl *s, queue_t *q, mblk_t *mp)
{
	int rtn = 0;

	if (!mi_timer_valid(mp))
		return (0);

	sl_save_total_state(s);

	switch (*(int *) mp->b_rptr) {
	case 1:
		mi_strlog(s->oq, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = sl_t1_timeout(s, q);
		break;
	default:
		mi_strlog(s->oq, STRLOGTO, SL_TRACE, "-> ?? TIMEOUT <-");
		rtn = 0;
		break;
	}
	if (rtn != 0) {
		sl_restore_total_state(s);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}

	return (rtn);
}

/**
 * sl_m_sig: process M_(PC)SIG message
 * @q: active queue
 * @mp: the M_(PC)PROTO message
 */
static inline fastcall int
sl_m_sig(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if (likely((priv = mi_trylock(q)) != NULL)) {
		err = __sl_m_sig(SL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

/**
 * sl_m_flush: - process M_FLUSH message
 * @q: active queue
 * @mp: the M_FLUSH message
 *
 * Avoid having to push pipemod.  If we are the bottommost module over a pipe
 * twist then perform the actions of pipemod.  This means that the sl-pmod
 * module must be pushed over the same side of a pipe as pipemod, if pipemod
 * is pushed at all.
 */
static fastcall int
sl_m_flush(queue_t *q, mblk_t *mp)
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
__sl_m_ioctl(struct sl *s, queue_t *q, mblk_t *mp)
{
	int err;

	err = sl_do_ioctl(s, q, mp);
	if (err <= 0)
		return (err);
	mi_copy_done(q, mp, err);
	return (0);
}

/**
 * sl_m_ioctl: - process M_IOCTL message
 * @q: active queue
 * @mp: the M_IOCTL message
 */
static fastcall int
sl_m_ioctl(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if ((priv = mi_trylock(q)) != NULL) {
		err = __sl_m_ioctl(SL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

static fastcall int
__sl_m_iocdata(struct sl *s, queue_t *q, mblk_t *mp)
{

	mblk_t *dp;
	int err;

	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		err = 0;
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		err = sl_do_copyin(s, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		err = sl_do_copyout(s, q, mp, dp);
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
 * sl_m_iocdata: - process M_IOCDATA message
 * @q: active queue
 * @mp: the M_IOCDATA message
 */
static fastcall int
sl_m_iocdata(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int err = -EAGAIN;

	if ((priv = mi_trylock(q)) != NULL) {
		err = __sl_m_iocdata(SL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (err);
}

/**
 * sl_m_rse: - process M_(PC)RSE message
 * @q: active queue
 * @mp: the M_(PC)RSE message
 */
static fastcall int
sl_m_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * sl_m_other: - process other message
 * @q: active queue
 * @mp: other STREAMS message
 *
 * Simply pass unrecognized messages along.
 */
static fastcall int
sl_m_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * sl_msg_slow: - process STREAMS message, slow
 * @q: active queue
 * @mp: the STREAMS message
 *
 * This is the slow version of the STREAMS message handling.  It is expected
 * that data is delivered in M_DATA message blocks instead of SL_PDU_IND or
 * SL_PDU_REQ message blocks.  Nevertheless, if this slower function gets
 * called, it is more likely because we have an M_PROTO message block
 * containing an SL_PDU_REQ.
 */
static noinline fastcall int
sl_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return sl_m_proto(q, mp);
#if 1
	case M_SIG:
	case M_PCSIG:
		return sl_m_sig(q, mp);
#endif
	case M_IOCTL:
		return sl_m_ioctl(q, mp);
	case M_IOCDATA:
		return sl_m_iocdata(q, mp);
	case M_FLUSH:
		return sl_m_flush(q, mp);
	case M_RSE:
	case M_PCRSE:
		return sl_m_rse(q, mp);
	default:
		return sl_m_other(q, mp);
	case M_DATA:
		return sl_m_data(q, mp);
	}
}

/**
 * sl_msg: - process STREAMS message
 * @q: active queue
 * @mp: the message
 *
 * This function returns zero when the message has been absorbed.  When it returns non-zero, the
 * message is to be placed (back) on the queue.
 */
static inline fastcall int
sl_msg(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return sl_m_data(q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return sl_m_proto(q, mp);
	return sl_msg_slow(q, mp);
}

/**
 * __sl_msg_slow: - process STREAMS message, slow
 * @s: locked private structure
 * @q: active queue
 * @mp: the STREAMS message
 */
static noinline fastcall int
__sl_msg_slow(struct sl *s, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __sl_m_proto(s, q, mp);
	case M_SIG:
	case M_PCSIG:
		return __sl_m_sig(s, q, mp);
	case M_IOCTL:
		return __sl_m_ioctl(s, q, mp);
	case M_IOCDATA:
		return __sl_m_iocdata(s, q, mp);
	case M_FLUSH:
		return sl_m_flush(q, mp);
	case M_RSE:
	case M_PCRSE:
		return sl_m_rse(q, mp);
	default:
		return sl_m_other(q, mp);
	case M_DATA:
		return __sl_m_data(s, q, mp);
	}
}

/**
 * __sl_msg: - process STREAMS message locked
 * @s: locked private structure
 * @q: active queue
 * @mp: the message
 *
 * This function returns zero when the message has been absorbed.  When it returns non-zero, the
 * message is to be placed (back) on the queue.
 */
static inline fastcall int
__sl_msg(struct sl *s, queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return __sl_m_data(s, q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return __sl_m_proto(s, q, mp);
	return __sl_msg_slow(s, q, mp);
}

/*
 *  =========================================================================
 *
 *  STREAMS QUEUE PUT AND SERVICE PROCEDURES
 *
 *  =========================================================================
 */

/**
 * sl_putp: - put procedure
 * @q: active queue
 * @mp: message to put
 *
 * Quick canonical put procedure.
 */
static streamscall __hot_in int
sl_putp(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sl_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * sl_srvp: - service procedure
 * @q: queue to service
 *
 * Quick canonical service procedure.  This is a  little quicker for
 * processing bulked messages because it takes the lock once for the entire
 * group of M_DATA messages, instead of once for each message.
 */
static streamscall __hot_read int
sl_srvp(queue_t *q)
{
	mblk_t *mp;

	if (likely((mp = getq(q)) != NULL)) {
		caddr_t priv;

		if (likely((priv = mi_trylock(q)) != NULL)) {
			do {
				if (unlikely(__sl_msg(SL_PRIV(q), q, mp) != 0))
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
 *  STREAMS OPEN AND CLOSE ROUTINES
 *
 *  =========================================================================
 */

static caddr_t sl_opens = NULL;

/**
 * sl_qopen: - STREAMS module queue open routine
 * @q: read queue of freshly allocated queue pair
 * @devp: device number of driver
 * @oflags: flags to open(2s) call
 * @sflag: STREAMS flag
 * @crp: credentials of closing process
 */
static streamscall int
sl_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct sl_pair *p;
	mblk_t *loc_tp, *rem_tp;
	int err;

	if (q->q_ptr)
		return (0);
	if (!(loc_tp = mi_timer_alloc(sizeof(int))))
		return (ENOBUFS);
	*(int *) loc_tp->b_rptr = 1;
	if (!(rem_tp = mi_timer_alloc(sizeof(int)))) {
		mi_timer_free(loc_tp);
		return (ENOBUFS);
	}
	*(int *) rem_tp->b_rptr = 1;
	if ((err = mi_open_comm(&sl_opens, sizeof(*p), q, devp, oflags, sflag, crp))) {
		mi_timer_free(loc_tp);
		mi_timer_free(rem_tp);
		return (err);
	}

	p = PRIV(q);
	bzero(p, sizeof(*p));

	/* initialize the structure */
	p->r_priv.pair = p;
	p->r_priv.other = &p->w_priv;
	p->r_priv.oq = WR(q);
	p->r_priv.max_sdu = 272;
	p->r_priv.min_sdu = 3;
	p->r_priv.header_len = 0;
	p->r_priv.ppa_style = LMI_STYLE1;
	p->r_priv.state.l_state = LMI_DISABLED;
	p->r_priv.oldstate.l_state = LMI_DISABLED;
	p->r_priv.state.i_state = SLS_POWER_OFF;
	p->r_priv.oldstate.i_state = SLS_POWER_OFF;
	p->r_priv.state.i_flags = 0;
	p->r_priv.oldstate.i_flags = 0;

	p->r_priv.t1 = loc_tp;
	bufq_init(&p->r_priv.tb);
	bufq_init(&p->r_priv.rb);

	p->r_priv.option.pvar = SS7_PVAR_ITUT_00;
	p->r_priv.option.popt = 0;

	p->r_priv.sdt.notify.events = 0;

	p->r_priv.sdt.config.t8 = 100;
	p->r_priv.sdt.config.Tin = 4;
	p->r_priv.sdt.config.Tie = 1;
	p->r_priv.sdt.config.T = 64;
	p->r_priv.sdt.config.D = 256;
	p->r_priv.sdt.config.Te = 793544;	/* E1 */
	p->r_priv.sdt.config.De = 11328000;	/* E1 */
	p->r_priv.sdt.config.Ue = 198384000;	/* E1 */
	p->r_priv.sdt.config.N = 16;
	p->r_priv.sdt.config.m = 272;
	p->r_priv.sdt.config.b = 8;
	p->r_priv.sdt.config.f = SDT_FLAGS_ONE;

	p->r_priv.sdt.statem.aerm_state = SDT_STATE_IDLE;
	p->r_priv.sdt.statem.aborted_proving = false;
	p->r_priv.sdt.statem.Ca = 0;
	p->r_priv.sdt.statem.Ti = 0;
	p->r_priv.sdt.statem.suerm_state = SDT_STATE_IDLE;
	p->r_priv.sdt.statem.Cs = 0;
	p->r_priv.sdt.statem.Ns = 0;
	p->r_priv.sdt.statem.eim_state = SDT_STATE_IDLE;
	p->r_priv.sdt.statem.Ce = 0;
	p->r_priv.sdt.statem.su_received = false;
	p->r_priv.sdt.statem.interval_error = false;
	p->r_priv.sdt.statem.daedt_state = SDT_STATE_IDLE;
	p->r_priv.sdt.statem.daedr_state = SDT_STATE_IDLE;
	p->r_priv.sdt.statem.octet_counting_mode = false;

	p->r_priv.sdt.stats.tx_bytes = 0;
	p->r_priv.sdt.stats.tx_sus = 0;
	p->r_priv.sdt.stats.tx_sus_repeated = 0;
	p->r_priv.sdt.stats.tx_underruns = 0;
	p->r_priv.sdt.stats.tx_aborts = 0;
	p->r_priv.sdt.stats.tx_buffer_overflows = 0;
	p->r_priv.sdt.stats.tx_sus_in_error = 0;
	p->r_priv.sdt.stats.rx_bytes = 0;
	p->r_priv.sdt.stats.rx_sus = 0;
	p->r_priv.sdt.stats.rx_sus_compressed = 0;
	p->r_priv.sdt.stats.rx_overruns = 0;
	p->r_priv.sdt.stats.rx_aborts = 0;
	p->r_priv.sdt.stats.rx_buffer_overflows = 0;
	p->r_priv.sdt.stats.rx_sus_in_error = 0;
	p->r_priv.sdt.stats.rx_sync_transitions = 0;
	p->r_priv.sdt.stats.rx_crc_errors = 0;
	p->r_priv.sdt.stats.rx_frame_errors = 0;
	p->r_priv.sdt.stats.rx_frame_overflows = 0;
	p->r_priv.sdt.stats.rx_frame_too_long = 0;
	p->r_priv.sdt.stats.rx_frame_too_short = 0;
	p->r_priv.sdt.stats.rx_residue_errors = 0;
	p->r_priv.sdt.stats.rx_length_error = 0;
	p->r_priv.sdt.stats.carrier_cts_lost = 0;
	p->r_priv.sdt.stats.carrier_dcd_lost = 0;
	p->r_priv.sdt.stats.carrier_lost = 0;

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
	p->w_priv.max_sdu = 272;
	p->w_priv.min_sdu = 3;
	p->w_priv.header_len = 0;
	p->w_priv.ppa_style = LMI_STYLE1;
	p->w_priv.state.l_state = LMI_DISABLED;
	p->w_priv.oldstate.l_state = LMI_DISABLED;
	p->w_priv.state.i_state = SLS_POWER_OFF;
	p->w_priv.oldstate.i_state = SLS_POWER_OFF;
	p->w_priv.state.i_flags = 0;
	p->w_priv.oldstate.i_flags = 0;

	p->w_priv.t1 = rem_tp;
	bufq_init(&p->w_priv.tb);
	bufq_init(&p->w_priv.rb);

	p->w_priv.option.pvar = SS7_PVAR_ITUT_00;
	p->w_priv.option.popt = 0;

	p->w_priv.sdt.notify.events = 0;

	p->w_priv.sdt.config.t8 = 100;
	p->w_priv.sdt.config.Tin = 4;
	p->w_priv.sdt.config.Tie = 1;
	p->w_priv.sdt.config.T = 64;
	p->w_priv.sdt.config.D = 256;
	p->w_priv.sdt.config.Te = 793544;	/* E1 */
	p->w_priv.sdt.config.De = 11328000;	/* E1 */
	p->w_priv.sdt.config.Ue = 198384000;	/* E1 */
	p->w_priv.sdt.config.N = 16;
	p->w_priv.sdt.config.m = 272;
	p->w_priv.sdt.config.b = 8;
	p->w_priv.sdt.config.f = SDT_FLAGS_ONE;

	p->w_priv.sdt.statem.aerm_state = SDT_STATE_IDLE;
	p->w_priv.sdt.statem.aborted_proving = false;
	p->w_priv.sdt.statem.Ca = 0;
	p->w_priv.sdt.statem.Ti = 0;
	p->w_priv.sdt.statem.suerm_state = SDT_STATE_IDLE;
	p->w_priv.sdt.statem.Cs = 0;
	p->w_priv.sdt.statem.Ns = 0;
	p->w_priv.sdt.statem.eim_state = SDT_STATE_IDLE;
	p->w_priv.sdt.statem.Ce = 0;
	p->w_priv.sdt.statem.su_received = false;
	p->w_priv.sdt.statem.interval_error = false;
	p->w_priv.sdt.statem.daedt_state = SDT_STATE_IDLE;
	p->w_priv.sdt.statem.daedr_state = SDT_STATE_IDLE;
	p->w_priv.sdt.statem.octet_counting_mode = false;

	p->w_priv.sdt.stats.tx_bytes = 0;
	p->w_priv.sdt.stats.tx_sus = 0;
	p->w_priv.sdt.stats.tx_sus_repeated = 0;
	p->w_priv.sdt.stats.tx_underruns = 0;
	p->w_priv.sdt.stats.tx_aborts = 0;
	p->w_priv.sdt.stats.tx_buffer_overflows = 0;
	p->w_priv.sdt.stats.tx_sus_in_error = 0;

	p->w_priv.sdt.stats.rx_bytes = 0;
	p->w_priv.sdt.stats.rx_sus = 0;
	p->w_priv.sdt.stats.rx_sus_compressed = 0;
	p->w_priv.sdt.stats.rx_overruns = 0;
	p->w_priv.sdt.stats.rx_aborts = 0;
	p->w_priv.sdt.stats.rx_buffer_overflows = 0;
	p->w_priv.sdt.stats.rx_sus_in_error = 0;
	p->w_priv.sdt.stats.rx_sync_transitions = 0;
	p->w_priv.sdt.stats.rx_crc_errors = 0;
	p->w_priv.sdt.stats.rx_frame_errors = 0;
	p->w_priv.sdt.stats.rx_frame_overflows = 0;
	p->w_priv.sdt.stats.rx_frame_too_long = 0;
	p->w_priv.sdt.stats.rx_frame_too_short = 0;
	p->w_priv.sdt.stats.rx_residue_errors = 0;
	p->w_priv.sdt.stats.rx_length_error = 0;
	p->w_priv.sdt.stats.carrier_cts_lost = 0;
	p->w_priv.sdt.stats.carrier_dcd_lost = 0;
	p->w_priv.sdt.stats.carrier_lost = 0;

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
 * sl_qclose: - STREAMS module queue close routine
 * @q: read queue of closing queue pair
 * @oflags: flags to open(2s) call
 * @crp: credentials of closing process
 */
static streamscall int
sl_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct sl_pair *p = PRIV(q);

	qprocsoff(q);
	mi_timer_free(XCHG(&p->r_priv.t1, NULL));
	mi_timer_free(XCHG(&p->w_priv.t1, NULL));
	bufq_purge(&p->r_priv.tb);
	bufq_purge(&p->r_priv.rb);
	bufq_purge(&p->w_priv.tb);
	bufq_purge(&p->w_priv.rb);
	mi_close_comm(&sl_opens, q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  PRIVATE STRUCTURE ALLOCATION, DEALLOCATION AND CACHE
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  REGISTRATION AND INITIALIZATION
 *
 *  =========================================================================
 */

STATIC struct qinit sl_rinit = {
	.qi_putp = sl_putp,		/* Read put (message from below) */
	.qi_srvp = sl_srvp,		/* Read queue service */
	.qi_qopen = sl_qopen,		/* Each open */
	.qi_qclose = sl_qclose,		/* Last close */
	.qi_minfo = &sl_minfo,		/* Information */
	.qi_mstat = &sl_rstat,		/* Statistics */
};

STATIC struct qinit sl_winit = {
	.qi_putp = sl_putp,		/* Write put (message from above) */
	.qi_srvp = sl_srvp,		/* Write queue service */
	.qi_minfo = &sl_minfo,		/* Information */
	.qi_mstat = &sl_wstat,		/* Statistics */
};

STATIC struct streamtab slpmodinfo = {
	.st_rdinit = &sl_rinit,		/* Upper read queue */
	.st_wrinit = &sl_winit,		/* Upper write queue */
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
MODULE_PARM_DESC(modid, "Module ID for SLPMOD module.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

STATIC struct fmodsw sl_fmod = {
	.f_name = MOD_NAME,
	.f_str = &slpmodinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

/**
 * slpmodinit: - initialize SL-PMOD
 */
MODULE_STATIC int __init
slpmodinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&sl_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d, err = %d\n", MOD_NAME, (int) modid, err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

/**
 * slpmodterminate: - terminate SL-PMOD
 */
MODULE_STATIC void __exit
slpmodterminate(void)
{
	int err;

	if ((err = unregister_strmod(&sl_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(slpmodinit);
module_exit(slpmodterminate);

#endif				/* LINUX */
