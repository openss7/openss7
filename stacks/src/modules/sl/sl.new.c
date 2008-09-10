/*****************************************************************************

 @(#) $RCSfile: sl.new.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-09-10 03:49:33 $

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

 Last Modified $Date: 2008-09-10 03:49:33 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sl.new.c,v $
 Revision 0.9.2.3  2008-09-10 03:49:33  brian
 - changes to accomodate FC9, SUSE 11.0 and Ubuntu 8.04

 Revision 0.9.2.2  2008-04-29 07:11:11  brian
 - updating headers for release

 Revision 0.9.2.1  2007/08/18 03:53:27  brian
 - working up configuration files

 Revision 0.9.2.21  2007/07/14 01:35:06  brian
 - make license explicit, add documentation

 Revision 0.9.2.20  2007/03/25 19:00:15  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.19  2007/03/25 02:22:58  brian
 - add D_MP and D_MTPERQ flags

 Revision 0.9.2.18  2007/03/25 00:52:07  brian
 - synchronization updates

 Revision 0.9.2.17  2006/12/27 16:35:55  brian
 - added slpmod module and fixups for make check target

 Revision 0.9.2.16  2006/05/08 11:01:10  brian
 - new compilers mishandle postincrement of cast pointers

 Revision 0.9.2.15  2006/04/24 05:01:02  brian
 - call interface corrections

 Revision 0.9.2.14  2006/03/07 01:11:46  brian
 - updated headers

 *****************************************************************************/

#ident "@(#) $RCSfile: sl.new.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-09-10 03:49:33 $"

static char const ident[] =
    "$RCSfile: sl.new.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-09-10 03:49:33 $";

/*
 *  This is an SL (Signalling Link) module which can be pushed over an SDT
 *  (Signalling Data Terminal) module to effect an OpenSS7 Signalling Link.
 *  Having the SL state machines separate permits live upgrade and allows this
 *  state machine to be rigorously conformance tested only once.
 */
#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

#define SL_DESCRIP	"SS7/IP SIGNALLING LINK (SL) STREAMS MODULE."
#define SL_REVISION	"OpenSS7 $RCSfile: sl.new.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-09-10 03:49:33 $"
#define SL_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define SL_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SL_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SL_LICENSE	"GPL"
#define SL_BANNER	SL_DESCRIP	"\n" \
			SL_REVISION	"\n" \
			SL_COPYRIGHT	"\n" \
			SL_DEVICE	"\n" \
			SL_CONTACT	"\n"
#define SL_SPLASH	SL_DESCRIP	"\n" \
			SL_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(SL_CONTACT);
MODULE_DESCRIPTION(SL_DESCRIP);
MODULE_SUPPORTED_DEVICE(SL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SL_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sl");
#endif
#endif				/* LINUX */

#ifdef LFS
#define SL_MOD_ID	CONFIG_STREAMS_SL_MODID
#define SL_MOD_NAME	CONFIG_STREAMS_SL_NAME
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define MOD_ID		SL_MOD_ID
#define MOD_NAME	SL_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SL_BANNER
#else				/* MODULE */
#define MOD_BANNER	SL_SPLASH
#endif				/* MODULE */

static struct module_info sl_minfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 1,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_lowat = 1 << 10,		/* Lo water mark */
};

/*
 *  =========================================================================
 *
 *  SL Private Structure
 *
 *  =========================================================================
 */

typedef struct sl {
	STR_DECLARATION (struct sl);	/* stream declaration */
	bufq_t rb;			/* receive buffer */
	bufq_t tb;			/* transmission buffer */
	bufq_t rtb;			/* retransmission buffer */
	lmi_option_t option;		/* LMI options */
	sl_timers_t timers;		/* SL timers */
	sl_config_t config;		/* SL configuration */
	sl_statem_t statem;		/* SL state machine */
	sl_notify_t notify;		/* SL notification options */
	sl_stats_t stats;		/* SL statistics */
	sl_stats_t stamp;		/* SL statistics timestamps */
	lmi_sta_t statsp;		/* SL statistics periods */
} sl_t;

static struct sl *sl_alloc_priv(queue_t *, struct sl **, dev_t *, cred_t *);
static struct sl *sl_get(struct sl *);
static void sl_put(struct sl *);
static void sl_free_priv(queue_t *);

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

struct lmi_option lmi_default = {
	pvar:SS7_PVAR_ITUT_96,
	popt:0,
};
struct sl_config sl_default = {
	t1:45 * HZ,
	t2:5 * HZ,
	t2l:20 * HZ,
	t2h:100 * HZ,
	t3:1 * HZ,
	t4n:8 * HZ,
	t4e:500 * HZ / 1000,
	t5:100 * HZ / 1000,
	t6:4 * HZ,
	t7:1 * HZ,
	rb_abate:3,
	rb_accept:6,
	rb_discard:9,
	tb_abate_1:128 * 272,
	tb_onset_1:256 * 272,
	tb_discd_1:384 * 272,
	tb_abate_2:512 * 272,
	tb_onset_2:640 * 272,
	tb_discd_2:768 * 272,
	tb_abate_3:896 * 272,
	tb_onset_3:1024 * 272,
	tb_discd_3:1152 * 272,
	N1:127,
	N2:8192,
	M:5,
};

/**
 * sl_iocname - display SL ioctl command name
 * @cmd: ioctl command
 */
static const char *
sl_iocname(int cmd)
{
	switch (_IOC_TYPE(cmd)) {
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
		case _IOC_NR(LMI_IOCGCONFIG):
			return ("LMI_IOCGCONFIG");
		case _IOC_NR(LMI_IOCSCONFIG):
			return ("LMI_IOCSCONFIG");
		case _IOC_NR(LMI_IOCTCONFIG):
			return ("LMI_IOCTCONFIG");
		case _IOC_NR(LMI_IOCCCONFIG):
			return ("LMI_IOCCCONFIG");
		case _IOC_NR(LMI_IOCGSTATEM):
			return ("LMI_IOCGSTATEM");
		case _IOC_NR(LMI_IOCCMRESET):
			return ("LMI_IOCCMRESET");
		case _IOC_NR(LMI_IOCGSTATSP):
			return ("LMI_IOCGSTATSP");
		case _IOC_NR(LMI_IOCSSTATSP):
			return ("LMI_IOCSSTATSP");
		case _IOC_NR(LMI_IOCGSTATS):
			return ("LMI_IOCGSTATS");
		case _IOC_NR(LMI_IOCCSTATS):
			return ("LMI_IOCCSTATS");
		case _IOC_NR(LMI_IOCGNOTIFY):
			return ("LMI_IOCGNOTIFY");
		case _IOC_NR(LMI_IOCSNOTIFY):
			return ("LMI_IOCSNOTIFY");
		case _IOC_NR(LMI_IOCCNOTIFY):
			return ("LMI_IOCCNOTIFY");
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
	default:
		return ("(unknown primitive)");
	}
}

/**
 * sl_i_state_name: display LMI state name
 * @state: the state value to display
 */
static const char *
sl_i_state_name(int state)
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

#define SLS_POWER_OFF		0
#define SLS_OUT_OF_SERVICE	1
#define SLS_ALIGNMENT		2
#define SLS_ALIGNED_READY	3
#define SLS_ALIGNED_NOT_READY	4
#define SLS_IN_SERVICE		5

static const char *
sl_l_state_name(int state)
{
	switch (state) {
	case SLS_POWER_OFF:
		return ("SLS_POWER_OFF");
	case SLS_OUT_OF_SERVICE:
		return ("SLS_OUT_OF_SERVICE");
	case SLS_ALIGNMENT:
		return ("SLS_ALIGNMENT");
	case SLS_ALIGNED_READY:
		return ("SLS_ALIGNED_READY");
	case SLS_ALIGNED_NOT_READY:
		return ("SLS_ALIGNED_NOT_READY");
	case SLS_IN_SERVICE:
		return ("SLS_IN_SERVICE");
	default:
		return ("SL_????");
	}
}

#define SLF_LOC_PROC_OUT	(1<<0)
#define SLF_REM_PROC_OUT	(1<<1)
#define SLF_EMERGENCY		(1<<2)
#define SLF_CONG_DISCARD	(1<<3)
#define SLF_CONG_ACCEPT		(1<<4)

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
 * sdt_get_l_state: - get management state for private structure
 * @sdt: private structure
 */
static int
sdt_get_l_state(struct sdt *sdt)
{
	return (sdt->state.l_state);
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

/**
 * sdt_set_l_state: - set management state for private structure
 * @sdt: private structure
 * @newstate: new state
 */
static int
sdt_set_l_state(struct sdt *sdt, int newstate)
{
	int oldstate = sdt->state.l_state;

	if (newstate != oldstate) {
		sdt->state.l_state = newstate;
		mi_strlog(sdt->oq, STRLOGST, SL_TRACE, "%s <- %s", sdt_l_state_name(newstate),
			  sdt_l_state_name(oldstate));
	}
	return (newstate);
}

static int
sl_save_l_state(struct sl *sl)
{
	return ((sl->oldstate.l_state = sl_get_l_state(sl)));
}

static int
sdt_save_l_state(struct sdt *sdt)
{
	return ((sdt->oldstate.l_state = sdt_get_l_state(sdt)));
}

static int
sl_restore_l_state(struct sl *sl)
{
	return sl_set_l_state(sl, sl->oldstate.l_state);
}

static int
sdt_restore_l_state(struct sdt *sdt)
{
	return sdt_set_l_state(sdt, sdt->oldstate.l_state);
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
 * sdt_get_i_state: - get interface state for private structure
 * @sdt: private structure
 */
static int
sdt_get_i_state(struct sdt *sdt)
{
	return (sdt->state.i_state);
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

/**
 * sdt_set_i_state: - set interface state for private structure
 * @sdt: private structure
 * @newstate: new state
 */
static int
sdt_set_i_state(struct sdt *sdt, int newstate)
{
	int oldstate = sdt_get_i_state(sdt);

	if (newstate != oldstate) {
		sdt->state.i_state = newstate;
		mi_strlog(sdt->oq, STRLOGST, SL_TRACE, "%s <- %s", sdt_i_state_name(newstate),
			  sdt_i_state_name(oldstate));
	}
	return (newstate);
}

static int
sl_save_i_state(struct sl *sl)
{
	return ((sl->oldstate.l_state = sl_get_i_state(sl)));
}

static int
sdt_save_i_state(struct sdt *sdt)
{
	return ((sdt->oldstate.l_state = sdt_get_i_state(sdt)));
}

static int
sl_restore_i_state(struct sl *sl)
{
	return sl_set_i_state(sl, sl->oldstate.l_state);
}

static int
sdt_restore_i_state(struct sdt *sdt)
{
	return sdt_set_i_state(sdt, sdt->oldstate.l_state);
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
 * sdt_save_state: - reset state for private structure
 * @sdt: private structure
 */
static void
sdt_save_state(struct sdt *sdt)
{
	sdt_save_l_state(sdt);
	sdt_save_i_state(sdt);
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
 * sdt_restore_state: - reset state for private structure
 * @sdt: private structure
 */
static int
sdt_restore_state(struct sdt *sdt)
{
	sdt_restore_i_state(sdt);
	return sdt_restore_l_state(sdt);
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
 * sdt_get_flags: - get flags for private structure
 * @sdt: private structure
 */
static int
sdt_get_flags(struct sdt *sdt)
{
	return (sdt->state.i_flags);
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

/**
 * sdt_set_flags: - set flags for private structure
 * @sdt: private structure
 * @newflags: new flags
 */
static int
sdt_set_flags(struct sdt *sdt, int mask)
{
	int oldflags = sdt_get_flags(sdt), newflags = oldflags |= mask;

	if (oldflags != newflags)
		mi_strlog(sdt->oq, STRLOGST, SL_TRACE, "%s",
			  sdt_flags_set((newflags ^ oldflags) & mask));
	return (sdt->state.i_flags = newflags);
}

static inline int
sl_or_flags(struct sl *sl, int orflags)
{
	return (sl->state.i_flags |= orflags);
}

static inline int
sdt_or_flags(struct sdt *sdt, int orflags)
{
	return (sdt->state.i_flags |= orflags);
}

static inline int
sl_nand_flags(struct sl *sl, int nandflags)
{
	return (sl->state.i_flags &= ~nandflags);
}

static inline int
sdt_nand_flags(struct sdt *sdt, int nandflags)
{
	return (sdt->state.i_flags &= ~nandflags);
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
 * sdt_save_flags: - reset flags for private structure
 * @sdt: private structure
 */
static void
sdt_save_flags(struct sdt *sdt)
{
	sdt->oldstate.i_flags = sdt->state.i_flags;
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

/**
 * sdt_restore_flags: - reset flags for private structure
 * @sdt: private structure
 */
static int
sdt_restore_flags(struct sdt *sdt)
{
	return sdt_set_flags(sdt, sdt->oldstate.i_flags);
}

static void
sl_save_total_state(struct sl *sl)
{
	sdt_save_flags(sl->sdt);
	sdt_save_state(sl->sdt);
	sl_save_flags(sl);
	sl_save_state(sl);
}

static void
sdt_save_total_state(struct sdt *sdt)
{
	sl_save_flags(sdt->sl);
	sl_save_state(sdt->sl);
	sdt_save_flags(sdt);
	sdt_save_state(sdt);
}

static int
sl_restore_total_state(struct sl *sl)
{
	sdt_restore_flags(sl->sdt);
	sdt_restore_state(sl->sdt);
	sl_restore_flags(sl);
	return sl_restore_state(sl);
}

static int
sdt_restore_total_state(struct sdt *sdt)
{
	sl_restore_flags(sdt->sl);
	sl_restore_state(sdt->sl);
	sdt_restore_flags(sdt);
	return sdt_restore_state(sdt);
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

static inline int
sdt_clr_flags(struct sdt *sdt, int mask)
{
	int oldflags = sdt_get_flags(sdt), newflags = oldflags &= ~mask;

	if (oldflags != newflags)
		mi_strlog(sdt->oq, STRLOGST, SL_TRACE, "~%s",
			  sdt_flags_set((newflags ^ oldflags) & mask));
	return (sdt->state.i_flags = newflags);
}

/*
 *  ========================================================================
 *
 *  PRIMITIVES
 *
 *  ========================================================================
 */
/*
 *  ------------------------------------------------------------------------
 *
 *  Primitive sent upstream
 *
 *  ------------------------------------------------------------------------
 */
/*
 *  M_ERROR
 *  -----------------------------------
 */
static inline fastcall int
m_error(struct sl *sl, queue_t *q, int err)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		sl_set_i_state(sl, LMI_UNUSABLE);
		sdt_set_i_state(sl->sdt, LMI_UNUSABLE);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- M_ERROR");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#if 0
/*
 *  M_HANGUP
 *  -----------------------------------
 *  This primitive is not generated, but is merely passed from below.
 */
static inline fastcall int
m_hangup(struct sl *sl, queue_t *q, int err)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		DB_TYPE(mp) = M_HANGUP;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		sl_set_i_state(sl, LMI_UNUSABLE);
		sdt_set_i_state(sl->sdt, LMI_UNUSABLE);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- M_HANGUP");
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_INFO_ACK
 *  -----------------------------------
 *  This primitive is not generated, but is merely passed from below.
 */
static inline fastcall int
lmi_info_ack(struct sl *sl, queue_t *q, caddr_t ppa_ptr, size_t ppa_len)
{
	lmi_info_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = sl_get_i_state(sl);
		p->lmi_max_sdu = 272 + 1 + ((sl->option.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_min_sdu = ((sl->option.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE2;
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		printd(("%s: %p: <- LMI_INFO_ACK\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_OK_ACK
 *  -----------------------------------
 *  This primitive is not generated, but is merely passed from below.
 */
static inline fastcall int
lmi_ok_ack(struct sl *sl, queue_t *q, long prim)
{
	lmi_ok_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = ((typeof(p)) mp->b_wptr)++;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (sl_get_i_state(sl)) {
		case LMI_ATTACH_PENDING:
			sl_set_i_state(sl, LMI_DISABLED);
			break;
		case LMI_DETACH_PENDING:
			sl_set_i_state(sl, LMI_UNATTACHED);
			break;
		default:
			break;
		}
		p->lmi_state = sl_get_i_state(sl);
		printd(("%s: %p: <- LMI_OK_ACK\n", MOD_NAME, sl));
		putnext(sl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  LMI_ERROR_ACK
 *  -----------------------------------
 */
static inline fastcall int
lmi_error_ack(struct sl *sl, queue_t *q, mblk_t *msg, long prim, ulong reason, ulong errno)
{
	lmi_error_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_error_primitive = prim;
		switch (sl->i_state) {
		case LMI_ATTACH_PENDING:
			sl->i_state = LMI_UNATTACHED;
			break;
		case LMI_DETACH_PENDING:
			sl->i_state = LMI_DISABLED;
			break;
		case LMI_ENABLE_PENDING:
			sl->i_state = LMI_DISABLED;
			break;
		case LMI_DISABLE_PENDING:
			sl->i_state = LMI_ENABLED;
			break;
		default:
			break;
		}
		p->lmi_state = sl->i_state;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- LMI_ERROR_ACK");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#if 0
/*
 *  LMI_ENABLE_CON
 *  -----------------------------------
 *  This primitive is not generated, but is merely passed from below.
 */
static inline fastcall int
lmi_enable_con(struct sl *sl, queue_t *q)
{
	lmi_enable_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ENABLE_CON;
		p->lmi_state = sl_set_i_state(sl, LMI_ENABLED);
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- LMI_ENABLE_CON");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_CON
 *  -----------------------------------
 *  This primitive is not generated, but is merely passed from below.
 */
static inline fastcall int
lmi_disable_con(struct sl *sl, queue_t *q)
{
	lmi_disable_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_DISABLE_CON;
		p->lmi_state = sl_set_i_state(sl, LMI_ENABLED);
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- LMI_DISABLE_CON");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_OPTMGMT_ACK
 *  -----------------------------------
 *  This primitive is not generated, but is merely passed from below.
 */
static inline fastcall int
lmi_optmgmt_ack(struct sl *sl, queue_t *q, ulong flags, caddr_t opt_ptr, size_t opt_len)
{
	lmi_optmgmt_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = sizeof(*p);
		p->lmi_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- LMI_OPTMGMT_ACK");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  LMI_ERROR_IND
 *  -----------------------------------
 *  This primitive is not generated, but is merely passed from below.
 */
static inline fastcall int
lmi_error_ind(struct sl *sl, queue_t *q, ulong errno, ulong reason)
{
	lmi_error_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_state = sl_get_i_state(sl);
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- LMI_ERROR_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  LMI_STATS_IND
 *  -----------------------------------
 *  TODO: Statistics indications need to be provided in the implementation.
 */
static inline fastcall int
lmi_stats_ind(struct sl *sl, queue_t *q, ulong interval, mblk_t *dp)
{
	lmi_stats_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_STATS_IND;
		p->lmi_interval = interval;
		p->lmi_timestamp = jiffies;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- LMI_STATS_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  LMI_EVENT_IND
 *  -----------------------------------
 *  TODO: Event indications need to be provided in the implementation.
 */
static inline fastcall int
lmi_event_ind(struct sl *sl, queue_t *q, ulong oid, ulong level)
{
	mblk_t *mp;
	lmi_event_ind_t *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_EVENT_IND;
		p->lmi_objectid = oid;
		p->lmi_timestamp = jiffies;
		p->lmi_severity = level;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- LMI_EVENT_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  SL_PDU_IND
 *  -----------------------------------
 *  We don't actually use SL_PDU_INDs, we pass along M_DATA messages.
 */
static inline fastcall int
sl_pdu_ind(struct sl *sl, queue_t *q, mblk_t *dp)
{
	sl_pdu_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_PDU_IND;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
#ifndef _OPTIMIZE_SPEED
		mi_strlog(sl->oq, STRLOGDA, SL_TRACE, "<- SL_PDU_IND");
#endif				/* _OPTIMIZE_SPEED */
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  SL_LINK_CONGESTED_IND
 *  -----------------------------------
 */
static inline fastcall int
sl_link_congested_ind(struct sl *sl, queue_t *q, ulong cong, ulong disc)
{
	sl_link_cong_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_LINK_CONGESTED_IND;
		p->sl_cong_status = cong;
		p->sl_disc_status = disc;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_LINK_CONGESTED_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_LINK_CONGESTION_CEASED_IND
 *  -----------------------------------
 */
static inline fastcall int
sl_link_congestion_ceased_ind(struct sl *sl, queue_t *q, ulong cong, ulong disc)
{
	sl_link_cong_ceased_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_LINK_CONGESTION_CEASED_IND;
		p->sl_timestamp = jiffies;
		p->sl_cong_status = cong;
		p->sl_disc_status = disc;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_LINK_CONGESTION_CEASED_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVED_MESSAGE_IND
 *  -----------------------------------
 */
static inline fastcall int
sl_retrieved_message_ind(struct sl *sl, queue_t *q, mblk_t *dp)
{
	sl_retrieved_msg_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVED_MESSAGE_IND;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_RETRIEVED_MESSAGE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RETRIEVAL_COMPLETE_IND
 *  -----------------------------------
 */
static inline fastcall int
sl_retrieval_complete_ind(struct sl *sl, queue_t *q)
{
	sl_retrieval_comp_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVAL_COMPLETE_IND;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_RETRIEVAL_COMPLETE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RB_CLEARED_IND
 *  -----------------------------------
 */
static inline fastcall int
sl_rb_cleared_ind(struct sl *sl, queue_t *q)
{
	sl_rb_cleared_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RB_CLEARED_IND;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_RB_CLEARED_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_BSNT_IND
 *  -----------------------------------
 */
static inline fastcall int
sl_bsnt_ind(struct sl *sl, queue_t *q, ulong bsnt)
{
	sl_bsnt_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_BSNT_IND;
		p->sl_bsnt = bsnt;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_BSNT_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_IN_SERVICE_IND
 *  -----------------------------------
 */
static inline fastcall int
sl_in_service_ind(struct sl *sl, queue_t *q)
{
	sl_in_service_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_IN_SERVICE_IND;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_IN_SERVICE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_OUT_OF_SERVICE_IND
 *  -----------------------------------
 */
static inline fastcall int
sl_out_of_service_ind(struct sl *sl, queue_t *q, ulong reason)
{
	sl_out_of_service_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_OUT_OF_SERVICE_IND;
		p->sl_timestamp = jiffies;
		p->sl_reason = reason;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_OUT_OF_SERVICE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_REMOTE_PROCESSOR_OUTAGE_IND
 *  -----------------------------------
 */
static inline fastcall int
sl_remote_processor_outage_ind(struct sl *sl, queue_t *q)
{
	sl_rem_proc_out_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_REMOTE_PROCESSOR_OUTAGE_IND;
		p->sl_timestamp = jiffies;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_REMOTE_PROCESSOR_OUTAGE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_REMOTE_PROCESSOR_RECOVERED_IND
 *  -----------------------------------
 */
static inline fastcall int
sl_remote_processor_recovered_ind(struct sl *sl, queue_t *q)
{
	sl_rem_proc_recovered_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_REMOTE_PROCESSOR_RECOVERED_IND;
		p->sl_timestamp = jiffies;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_REMOTE_PROCESSOR_RECOVERED_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_RTB_CLEARED_IND
 *  -----------------------------------
 */
static inline fastcall int
sl_rtb_cleared_ind(struct sl *sl, queue_t *q)
{
	sl_rtb_cleared_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RTB_CLEARED_IND;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_RTB_CLEARED_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#if 0
/*
 *  SL_RETRIEVAL_NOT_POSSIBLE_IND
 *  -----------------------------------
 *  This primitive is not necessary in the current implementation.  Retrieval
 *  is always possible.
 */
static inline fastcall int
sl_retrieval_not_possible_ind(struct sl *sl, queue_t *q)
{
	sl_retrieval_not_poss_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_RETRIEVAL_NOT_POSSIBLE_IND;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_RETRIEVAL_NOT_POSSIBLE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SL_BSNT_NOT_RETRIEVABLE_IND
 *  -----------------------------------
 *  This primitive is not necessary in the current implementation.  Retrieval
 *  of BSNT is always possible.
 */
static inline fastcall int
sl_bsnt_not_retrievable_ind(struct sl *sl, queue_t *q, ulong bsnt)
{
	sl_bsnt_not_retr_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_BSNT_NOT_RETRIEVABLE_IND;
		p->sl_bsnt = bsnt;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_BSNT_NOT_RETRIEVABLE_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  SL_OPTMGMT_ACK
 *  -----------------------------------
 *  Option management from the local management interface is not yet
 *  implemented.
 */
static inline fastcall int
sl_optmgmt_ack(struct sl *sl, queue_t *q, caddr_t opt_ptr, size_t opt_len, ulong flags)
{
	sl_optmgmt_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_OPTMGMT_ACK;
		p->opt_length = opt_len;
		p->opt_offset = sizeof(*p);
		p->mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_OPTMGMT_ACK");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

#if 0
/*
 *  SL_NOTIFY_IND
 *  -----------------------------------
 *  TODO: Notification indications need to be included in the current
 *  implementation.  Notifications should be provided using LMI_EVENT_IND and
 *  not SL_NOTIFY_IND ???
 */
static inline fastcall int
sl_notify_ind(struct sl *sl, queue_t *q, ulong oid, ulong level)
{
	sl_notify_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sl_primitive = SL_NOTIFY_IND;
		p->sl_objectid = oid;
		p->sl_timestamp = drv_hztomsec(jiffies);
		p->sl_severity = level;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sl->oq, STRLOGTX, SL_TRACE, "<- SL_NOTIFY_IND");
		putnext(sl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}
#endif

/*
 *  ------------------------------------------------------------------------
 *
 *  Primitive sent downstream
 *
 *  ------------------------------------------------------------------------
 */

/*
 *  LMI_INFO_REQ
 *  -----------------------------------
 *  Generates the initial information request to synchronize the SL with the
 *  underlying SDT layer.  This must be successful or the stream will be
 *  rendered unusable.
 */
static inline fastcall int
sdt_info_req(struct sdt *sdt, queue_t *q)
{
	lmi_info_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "LMI_INFO_REQ ->");
		putnext(sdt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

#if 0
/*
 *  SDT_DAEDT_TRANSMISSION_REQ
 *  -----------------------------------
 *  We don't use transmission requests, we pass just M_DATA blocks instead.
 */
static inline fastcall int
sdt_daedt_transmission_req(struct sdt *sdt, queue_t *q, mblk_t *dp)
{
	sdt_daedt_transmission_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_DAEDT_TRANSMISSION_REQ;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = dp;
#ifndef _OPTIMIZE_SPEED
		mi_strlog(sdt->oq, STRLOGDA, SL_TRACE, "SDT_DEADT_TRANSMISSION_REQ ->");
#endif				/* _OPTIMIZE_SPEED */
		putnext(sdt->oq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  SDT_DAEDT_START_REQ
 *  -----------------------------------
 */
static inline fastcall int
sdt_daedt_start_req(struct sdt *sdt, queue_t *q)
{
	sdt_daedt_start_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_DAEDT_START_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "SDT_DAEDT_START_REQ ->");
		putnext(sdt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SDT_DAEDR_START_REQ
 *  -----------------------------------
 */
static inline fastcall int
sdt_daedr_start_req(struct sdt *sdt, queue_t *q)
{
	sdt_daedr_start_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_DAEDR_START_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "SDT_DAEDR_START_REQ ->");
		putnext(sdt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SDT_AERM_START_REQ
 *  -----------------------------------
 */
static inline fastcall int
sdt_aerm_start_req(struct sdt *sdt, queue_t *q)
{
	sdt_aerm_start_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_AERM_START_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "SDT_AERM_START_REQ ->");
		putnext(sdt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SDT_AERM_STOP_REQ
 *  -----------------------------------
 */
static inline fastcall int
sdt_aerm_stop_req(struct sdt *sdt, queue_t *q)
{
	sdt_aerm_stop_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_AERM_STOP_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "SDT_AERM_STOP_REQ ->");
		putnext(sdt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SDT_AERM_SET_TI_TO_TIN_REQ
 *  -----------------------------------
 */
static inline fastcall int
sdt_aerm_set_ti_to_tin_req(struct sdt *sdt, queue_t *q)
{
	sdt_aerm_set_ti_to_tin_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_AERM_SET_TI_TO_TIN_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "SDT_AERM_SET_TI_TO_TIN_REQ ->");
		putnext(sdt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SDT_AERM_SET_TI_TO_TIE_REQ
 *  -----------------------------------
 */
static inline fastcall int
sdt_aerm_set_ti_to_tie_req(struct sdt *sdt, queue_t *q)
{
	sdt_aerm_set_ti_to_tie_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_AERM_SET_TI_TO_TIE_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "SDT_AERM_SET_TI_TO_TIE_REQ ->");
		putnext(sdt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SDT_SUERM_START_REQ
 *  -----------------------------------
 */
static inline fastcall int
sdt_suerm_start_req(struct sdt *sdt, queue_t *q)
{
	sdt_suerm_start_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_SUERM_START_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "SDT_SUERM_START_REQ ->");
		putnext(sdt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  SDT_SUERM_STOP_REQ
 *  -----------------------------------
 */
static inline fastcall int
sdt_suerm_stop_req(struct sdt *sdt, queue_t *q)
{
	sdt_suerm_stop_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->sdt_primitive = SDT_SUERM_STOP_REQ;
		mp->b_wptr += sizeof(*p);
		mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "SDT_SUERM_STOP_REQ ->");
		putnext(sdt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  =========================================================================
 *
 *  PROTOCOL STATE MACHINE FUNCTIONS
 *
 *  =========================================================================
 */
/*
 *  ------------------------------------------------------------------------
 *
 *  Timers
 *
 *  ------------------------------------------------------------------------
 */
enum { tall, t1, t2, t3, t4, t5, t6, t7 };

static inline fastcall void
sl_timer_start(struct sl *sl, const int timer)
{
	switch (timer) {
	case t1:
		mi_timer(sl->sdt->oq, sl->timers.t1, sl->config.t1);
		break;
	case t2:
		mi_timer(sl->sdt->oq, sl->timers.t2, sl->config.t2);
		break;
	case t3:
		mi_timer(sl->sdt->oq, sl->timers.t3, sl->config.t3);
		break;
	case t4:
		mi_timer(sl->sdt->oq, sl->timers.t4, sl->statem.t4v);
		break;
	case t5:
		mi_timer(sl->sdt->oq, sl->timers.t5, sl->config.t5);
		break;
	case t6:
		mi_timer(sl->sdt->oq, sl->timers.t6, sl->config.t6);
		break;
	case t7:
		mi_timer(sl->sdt->oq, sl->timers.t7, sl->config.t7);
		break;
	default:
		mi_strlog(sl->oq, 0, SL_ERROR | SL_TRACE, "%s: invalid timer number", __FUNCTION__);
		break;
	}
}

static inline fastcall void
sl_timer_stop(struct sl *sl, const int timer)
{
	int single = 1;

	switch (timer) {
	case tall:
		single = 0;
		/* fall through */
	case t1:
		mi_timer_cancel(sl->timers.t1);
		if (single)
			break;
		/* fall through */
	case t2:
		mi_timer_cancel(sl->timers.t2);
		if (single)
			break;
		/* fall through */
	case t3:
		mi_timer_cancel(sl->timers.t3);
		if (single)
			break;
		/* fall through */
	case t4:
		mi_timer_cancel(sl->timers.t4);
		if (single)
			break;
		/* fall through */
	case t5:
		mi_timer_cancel(sl->timers.t5);
		if (single)
			break;
		/* fall through */
	case t6:
		mi_timer_cancel(sl->timers.t6);
		if (single)
			break;
		/* fall through */
	case t7:
		mi_timer_cancel(sl->timers.t7);
		if (single)
			break;
		/* fall through */
		break;
	default:
		mi_strlog(sl->oq, 0, SL_ERROR | SL_TRACE, "%s: invalid timer number", __FUNCTION__);
		break;
	}
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Duration Statistics
 *
 *  -------------------------------------------------------------------------
 */
#if 0
static void
sl_is_stats(queue_t *q, struct sl *sl)
{
	if (sl->stamp.sl_dur_unavail)
		sl->stats.sl_dur_unavail += jiffies - xchg(&sl->stamp.sl_dur_unavail, 0);
	if (sl->stamp.sl_dur_unavail_rpo)
		sl->stats.sl_dur_unavail_rpo += jiffies - xchg(&sl->stamp.sl_dur_unavail_rpo, 0);
	if (sl->stamp.sl_dur_unavail_failed)
		sl->stats.sl_dur_unavail_failed +=
		    jiffies - xchg(&sl->stamp.sl_dur_unavail_failed, 0);
	sl->stamp.sl_dur_in_service = jiffies;
}
static void
sl_oos_stats(queue_t *q, struct sl *sl)
{
	if (sl->stamp.sl_dur_in_service)
		sl->stats.sl_dur_in_service += jiffies - xchg(&sl->stamp.sl_dur_in_service, 0);
	if (sl->stamp.sl_dur_unavail_rpo)
		sl->stats.sl_dur_unavail_rpo += jiffies - xchg(&sl->stamp.sl_dur_unavail_rpo, 0);
	if (sl->stamp.sl_dur_unavail_failed)
		sl->stats.sl_dur_unavail_failed +=
		    jiffies - xchg(&sl->stamp.sl_dur_unavail_failed, 0);
	sl->stamp.sl_dur_unavail = jiffies;
}
static void
sl_rpo_stats(queue_t *q, struct sl *sl)
{
	if (sl->stamp.sl_dur_unavail_rpo)
		sl->stats.sl_dur_unavail_rpo += jiffies - xchg(&sl->stamp.sl_dur_unavail_rpo, 0);
}
static void
sl_rpr_stats(queue_t *q, struct sl *sl)
{
	if (sl->stamp.sl_dur_unavail_rpo)
		sl->stats.sl_dur_unavail_rpo += jiffies - xchg(&sl->stamp.sl_dur_unavail_rpo, 0);
}
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  SL State Machines
 *
 *  -------------------------------------------------------------------------
 */
#define SN_OUTSIDE(lower,middle,upper) \
	(  ( (lower) <= (upper) ) \
	   ? ( ( (middle) < (lower) ) || ( (middle) > (upper) ) ) \
	   : ( ( (middle) < (lower) ) && ( (middle) > (upper) ) ) \
	   )

/*
 *  -----------------------------------------------------------------------
 *
 *  STATE MACHINES:- The order of the state machine primitives below may seem
 *  somewhat disorganized at first glance; however, they have been ordered by
 *  dependency because they are all inline functions.  You see, the L2 state
 *  machine does not required multiple threading because there is never a
 *  requirement to invoke the individual state machines concurrently.  This
 *  works out good for the driver, because a primitive action expands inline
 *  to the necessary procedure, while the source still takes the appearance of
 *  the SDL diagrams in the SS7 specification for inspection and debugging.
 *
 *  -----------------------------------------------------------------------
 */

#define sl_cc_stop sl_cc_normal
static inline fastcall void
sl_cc_normal(struct sl *sl)
{
	sl_timer_stop(sl, t5);
	sl->statem.cc_state = SL_STATE_IDLE;
}

static inline fastcall void
sl_rc_stop(struct sl *sl)
{
	sl_cc_normal(sl);
	sl->statem.rc_state = SL_STATE_IDLE;
}

static inline fastcall int
sl_aerm_stop(struct sl *sl, queue_t *q)
{
	return sdt_aerm_stop_req(sl->sdt, q);
}

static inline fastcall int
sl_iac_stop(struct sl *sl, queue_t *q)
{
	int err;

	if (sl->statem.iac_state != SL_STATE_IDLE) {
		sl_timer_stop(sl, t3);
		sl_timer_stop(sl, t2);
		sl_timer_stop(sl, t4);
		if ((err = sl_aerm_stop(sl, q)))
			return (err);
		sl->statem.emergency = 0;
		sl->statem.iac_state = SL_STATE_IDLE;
	}
	return (0);
}

static fastcall void sl_tx_wakeup(struct sl *sl);

static inline fastcall void
sl_daedt_transmitter_wakeup(struct sl *sl)
{
	if (sl->statem.txc_state == SL_STATE_SLEEPING)
		sl->statem.txc_state = SL_STATE_IN_SERVICE;
	sl_tx_wakeup(sl);
}

static inline fastcall void
sl_txc_send_sios(struct sl *sl)
{
	sl_timer_stop(sl, t7);
	if (sl->option.pvar == SS7_PVAR_ANSI_92)
		sl_timer_stop(sl, t6);
	sl->statem.lssu_available = 1;
	sl->statem.tx.sio = LSSU_SIOS;
	sl_daedt_transmitter_wakeup(sl);
}

static inline fastcall void
sl_poc_stop(struct sl *sl)
{
	sl->statem.poc_state = SL_STATE_IDLE;
}

static inline fastcall int
sl_suerm_stop(struct sl *sl, queue_t *q)
{
	return sdt_suerm_stop_req(sl->sdt, q);
}

static inline fastcall int
sl_lsc_link_failure(struct sl *sl, queue_t *q, ulong reason)
{
	int err;

	if (sl->statem.lsc_state != SL_STATE_OUT_OF_SERVICE) {
		if ((err = sl_out_of_service_ind(sl, q, reason)))
			return (err);
		sl->statem.failure_reason = reason;
		if ((err = sl_iac_stop(sl, q)))	/* ok if not aligning */
			return (err);
		sl_timer_stop(sl, t1);	/* ok if not running */
		if ((err = sl_suerm_stop(sl, q)))	/* ok if not running */
			return (err);
		sl_rc_stop(sl);
		sl_txc_send_sios(sl);
		sl_poc_stop(sl);	/* ok if not ITUT */
		sl->statem.emergency = 0;
		sl->statem.local_processor_outage = 0;
		sl->statem.remote_processor_outage = 0;	/* ok if not ANSI */
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	}
	return (0);
}

static inline fastcall void
sl_txc_send_sib(struct sl *sl)
{
	sl->statem.tx.sio = LSSU_SIB;
	sl->statem.lssu_available = 1;
	sl_daedt_transmitter_wakeup(sl);
}

static inline fastcall void
sl_txc_send_sipo(struct sl *sl)
{
	sl_timer_stop(sl, t7);
	if (sl->option.pvar == SS7_PVAR_ANSI_92)
		sl_timer_stop(sl, t6);
	sl->statem.tx.sio = LSSU_SIPO;
	sl->statem.lssu_available = 1;
	sl_daedt_transmitter_wakeup(sl);
}

static inline fastcall void
sl_txc_send_sio(struct sl *sl)
{
	sl->statem.tx.sio = LSSU_SIO;
	sl->statem.lssu_available = 1;
	sl_daedt_transmitter_wakeup(sl);
}

static inline fastcall void
sl_txc_send_sin(struct sl *sl)
{
	sl->statem.tx.sio = LSSU_SIN;
	sl->statem.lssu_available = 1;
	sl_daedt_transmitter_wakeup(sl);
}

static inline fastcall void
sl_txc_send_sie(struct sl *sl)
{
	sl->statem.tx.sio = LSSU_SIE;
	sl->statem.lssu_available = 1;
	sl_daedt_transmitter_wakeup(sl);
}

static inline fastcall void
sl_txc_send_msu(struct sl *sl)
{
	if (sl->rtb.q_count)
		sl_timer_start(sl, t7);
	sl->statem.msu_inhibited = 0;
	sl->statem.lssu_available = 0;
	sl_daedt_transmitter_wakeup(sl);
}

static inline fastcall void
sl_txc_send_fisu(struct sl *sl)
{
	sl_timer_stop(sl, t7);
	if (sl->option.pvar == SS7_PVAR_ANSI_92 && !(sl->option.popt & SS7_POPT_PCR))
		sl_timer_stop(sl, t6);
	sl->statem.msu_inhibited = 1;
	sl->statem.lssu_available = 0;
	sl_daedt_transmitter_wakeup(sl);
}

static inline fastcall void
sl_txc_fsnx_value(struct sl *sl)
{
	if (sl->statem.tx.X.fsn != sl->statem.rx.X.fsn) {
		sl->statem.tx.X.fsn = sl->statem.rx.X.fsn;
		sl_daedt_transmitter_wakeup(sl);
	}
}

static inline fastcall void
sl_txc_nack_to_be_sent(struct sl *sl)
{
	int pcr = sl->option.popt & SS7_POPT_PCR;

	(void) pcr;
	assure(!pcr);
	sl->statem.tx.N.bib = sl->statem.tx.N.bib ? 0 : sl->statem.ib_mask;
	ptrace(("%s: %p: [%x/%x] %d\n", MOD_NAME, sl, sl->statem.tx.N.bib | sl->statem.tx.N.bsn,
		sl->statem.tx.N.fib | sl->statem.tx.N.fsn, pcr));
	sl_daedt_transmitter_wakeup(sl);
}

static inline fastcall int
sl_lsc_rtb_cleared(struct sl *sl, queue_t *q)
{
	int err;

	if (sl->statem.lsc_state == SL_STATE_PROCESSOR_OUTAGE) {
		sl->statem.remote_processor_outage = 0;
		if (sl->statem.local_processor_outage)
			break;
		if ((err = sl_remote_processor_recovered_ind(sl, q)))
			return (err);
		if ((err = sl_rtb_cleared_ind(sl, q)))
			return (err);
		sl_txc_send_msu(sl);
		sl->statem.lsc_state = SL_STATE_IN_SERVICE;
	}
	return (0);
}

static int sl_check_congestion(struct sl *sl, queue_t *q);

static inline fastcall int
sl_txc_bsnr_and_bibr(struct sl *sl, queue_t *q)
{
	int err;
	int pcr = sl->option.popt & SS7_POPT_PCR;

	sl->statem.tx.R.bsn = sl->statem.rx.R.bsn;
	sl->statem.tx.R.bib = sl->statem.rx.R.bib;
	printd(("%s: %p: [%x/%x] %d\n", MOD_NAME, sl, sl->statem.tx.N.bib | sl->statem.tx.N.bsn,
		sl->statem.tx.N.fib | sl->statem.tx.N.fsn, pcr));
	if (sl->statem.clear_rtb) {
		bufq_purge(&sl->rtb);
		sl->statem.Ct = 0;
		if ((err = sl_check_congestion(sl, q)))
			return (err);
		sl->statem.tx.F.fsn = (sl->statem.tx.R.bsn + 1) & sl->statem.sn_mask;
		sl->statem.tx.L.fsn = sl->statem.tx.R.bsn;
		sl->statem.tx.N.fsn = sl->statem.tx.R.bsn;
		trace();
		sl->statem.tx.N.fib = sl->statem.tx.R.bib;	/* for PCR too? */
		sl->statem.Z = (sl->statem.tx.R.bsn + 1) & sl->statem.sn_mask;
		sl->statem.z_ptr = NULL;
		/* FIXME: handle error return */
		if ((err = sl_lsc_rtb_cleared(sl, q)))
			return (err);
		sl->statem.clear_rtb = 0;
		sl->statem.rtb_full = 0;
		return (0);
	}
	printd(("%s: %p: F.fsn = %x, R.bsn = %x, mask = %x\n", MOD_NAME, sl, sl->statem.tx.F.fsn,
		sl->statem.tx.R.bsn, sl->statem.sn_mask));
	if (sl->statem.tx.F.fsn != ((sl->statem.tx.R.bsn + 1) & sl->statem.sn_mask)) {
		if (sl->statem.sib_received) {
			sl->statem.sib_received = 0;
			sl_timer_stop(sl, t6);
		}
		do {
			freemsg(bufq_dequeue(&sl->rtb));
			sl->statem.Ct--;
			sl->statem.tx.F.fsn = (sl->statem.tx.F.fsn + 1) & sl->statem.sn_mask;
		} while (sl->statem.tx.F.fsn != ((sl->statem.tx.R.bsn + 1) & sl->statem.sn_mask));
		if ((err = sl_check_congestion(sl, q)))
			return (err);
		sl_daedt_transmitter_wakeup(sl);
		if (sl->rtb.q_count == 0) {
			sl_timer_stop(sl, t7);
		} else {
			sl_timer_start(sl, t7);
		}
		if (!pcr || (sl->rtb.q_msgs < sl->config.N1 && sl->rtb.q_count < sl->config.N2)) {
			sl->statem.rtb_full = 0;
		}
		if (SN_OUTSIDE(sl->statem.tx.F.fsn, sl->statem.Z, sl->statem.tx.L.fsn)
		    || !sl->rtb.q_count) {
			sl->statem.Z = sl->statem.tx.F.fsn;
			sl->statem.z_ptr = sl->rtb.q_head;
		}
	}
	if (!pcr && sl->statem.tx.N.fib != sl->statem.tx.R.bib) {
		if (sl->statem.sib_received) {
			sl->statem.sib_received = 0;
			sl_timer_stop(sl, t6);
		}
		sl->statem.tx.N.fib = sl->statem.tx.R.bib;
		sl->statem.tx.N.fsn = (sl->statem.tx.F.fsn - 1) & sl->statem.sn_mask;
		if ((sl->statem.z_ptr = sl->rtb.q_head) != NULL) {
			sl->statem.retrans_cycle = 1;
		}
		sl_daedt_transmitter_wakeup(sl);
	}
	return (0);
}

static inline fastcall void
sl_txc_sib_received(struct sl *sl)
{
	/* 
	   FIXME: consider these variations for all */
	if (sl->option.pvar == SS7_PVAR_ANSI_92 && sl->statem.lssu_available)
		if (sl->statem.tx.sio != LSSU_SIB)
			return;
	if (sl->option.pvar != SS7_PVAR_ITUT_93 && !sl->rtb.q_count)
		return;
	if (!sl->statem.sib_received) {
		sl_timer_start(sl, t6);
		sl->statem.sib_received = 1;
	}
	sl_timer_start(sl, t7);
}

static inline fastcall int
sl_txc_clear_rtb(struct sl *sl, queue_t *q)
{
	bufq_purge(&sl->rtb);
	sl->statem.Ct = 0;
	sl->statem.clear_rtb = 1;
	sl->statem.rtb_full = 0;	/* added */
	/* 
	 * FIXME: should probably follow more of the ITUT flush_buffers stuff
	 * like reseting Z and FSNF, FSNL, FSNT.
	 */
	return sl_check_congestion(sl, q);
}

static inline fastcall int
sl_txc_clear_tb(struct sl *sl, queue_t *q)
{
	int err;

	bufq_purge(&sl->tb);
	flushq(sl->iq, FLUSHDATA);
	sl->statem.Cm = 0;
	return sl_check_congestion(sl, q);
}

static inline fastcall void
sl_txc_flush_buffers(struct sl *sl)
{
	bufq_purge(&sl->rtb);
	sl->statem.rtb_full = 0;
	sl->statem.Ct = 0;
	bufq_purge(&sl->tb);
	flushq(sl->iq, FLUSHDATA);
	sl->statem.Cm = 0;
	sl->statem.Z = 0;
	sl->statem.z_ptr = NULL;
	/* 
	   Z =0 error in ITUT 93 and ANSI */
	sl->statem.Z = sl->statem.tx.F.fsn = (sl->statem.tx.R.bsn + 1) & sl->statem.sn_mask;
	sl->statem.tx.L.fsn = sl->statem.rx.R.bsn;
	sl->statem.rx.T.fsn = sl->statem.rx.R.bsn;
	sl_timer_stop(sl, t7);
}

static inline fastcall void
sl_rc_fsnt_value(struct sl *sl)
{
	sl->statem.rx.T.fsn = sl->statem.tx.N.fsn;
}

static inline fastcall int
sl_txc_retrieval_request_and_fsnc(struct sl *sl, queue_t *q, sl_ulong fsnc)
{
	mblk_t *mp;
	int err;

	sl->statem.tx.C.fsn = fsnc & (sl->statem.sn_mask);
	/* 
	 *  FIXME: Q.704/5.7.2 states:
	 *
	 *  5.7.2   If a changeover order or acknowledgement containing an
	 *  unreasonable value of the forward sequence number is received, no
	 *  buffer updating or retrieval is performed, and new traffic is
	 *  started on the alternative signalling link(s).
	 *
	 *  It will be necessary to check FSNC for "reasonableness" here and
	 *  flush RTB and TB and return retrieval-complete indication with a
	 *  return code of "unreasonable FSNC".
	 *
	 *  (Tell the SIGTRAN working group and M2UA guys about this!)
	 */
	while (sl->rtb.q_count && sl->statem.tx.F.fsn != ((fsnc + 1) & sl->statem.sn_mask)) {
		freemsg(bufq_dequeue(&sl->rtb));
		sl->statem.Ct--;
		sl->statem.tx.F.fsn = (sl->statem.tx.F.fsn + 1) & sl->statem.sn_mask;
	}
	while ((mp = bufq_dequeue(&sl->tb))) {
		sl->statem.Cm--;
		bufq_queue(&sl->rtb, mp);
		sl->statem.Ct++;
		if (!sl->statem.Cm)
			qenable(sl->iq);	/* back enable */
	}
	sl->statem.Z = sl->statem.tx.F.fsn = (sl->statem.tx.C.fsn + 1) & sl->statem.sn_mask;
	while ((mp = bufq_dequeue(&sl->rtb))) {
		sl->statem.Ct--;
		if ((err = sl_retrieved_message_ind(sl, q, mp)))
			return (err);
	}
	sl->statem.rtb_full = 0;
	if ((err = sl_retrieval_complete_ind(sl, q)))
		return (err);
	sl->statem.Cm = 0;
	sl->statem.Ct = 0;
	sl->statem.tx.N.fsn = sl->statem.tx.L.fsn = sl->statem.tx.C.fsn;
	return (0);
}

static inline fastcall void
sl_daedt_fisu(struct sl *sl, mblk_t *mp)
{
	if (sl->option.popt & SS7_POPT_XSN) {
		*(sl_ushort *) mp->b_wptr = htons(sl->statem.tx.N.bsn | sl->statem.tx.N.bib);
		mp->b_wptr += sizeof(sl_ushort);
		*(sl_ushort *) mp->b_wptr = htons(sl->statem.tx.N.fsn | sl->statem.tx.N.fib);
		mp->b_wptr += sizeof(sl_ushort);
		*(sl_ushort *) mp->b_wptr = 0;
		mp->b_wptr += sizeof(sl_ushort);
	} else {
		*(sl_uchar *) mp->b_wptr = (sl->statem.tx.N.bsn | sl->statem.tx.N.bib);
		mp->b_wptr += sizeof(sl_uchar);
		*(sl_uchar *) mp->b_wptr = (sl->statem.tx.N.fsn | sl->statem.tx.N.fib);
		mp->b_wptr += sizeof(sl_uchar);
		*(sl_uchar *) mp->b_wptr = 0;
		mp->b_wptr += sizeof(sl_uchar);
	}
	sl->statem.txc_state = SL_STATE_SLEEPING;
	printd(("%s: %p: FISU [%x/%x] %ld ->\n", MOD_NAME, sl,
		sl->statem.tx.N.bib | sl->statem.tx.N.bsn,
		sl->statem.tx.N.fib | sl->statem.tx.N.fsn, sl->option.popt & SS7_POPT_PCR));
}

static inline fastcall void
sl_daedt_lssu(struct sl *sl, mblk_t *mp)
{
	if (sl->option.popt & SS7_POPT_XSN) {
		*(sl_ushort *) mp->b_wptr = htons(sl->statem.tx.N.bsn | sl->statem.tx.N.bib);
		mp->b_wptr += sizeof(sl_ushort);
		*(sl_ushort *) mp->b_wptr = htons(sl->statem.tx.N.fsn | sl->statem.tx.N.fib);
		mp->b_wptr += sizeof(sl_ushort);
		*(sl_ushort *) mp->b_wptr = htons(1);
		mp->b_wptr += sizeof(sl_ushort);
	} else {
		*(sl_uchar *) mp->b_wptr = (sl->statem.tx.N.bsn | sl->statem.tx.N.bib);
		mp->b_wptr += sizeof(sl_uchar);
		*(sl_uchar *) mp->b_wptr = (sl->statem.tx.N.fsn | sl->statem.tx.N.fib);
		mp->b_wptr += sizeof(sl_uchar);
		*(sl_uchar *) mp->b_wptr = 1;
		mp->b_wptr += sizeof(sl_uchar);
	}
	*(sl_uchar *) mp->b_wptr = (sl->statem.tx.sio);
	mp->b_wptr += sizeof(sl_uchar);
	if (sl->statem.tx.sio != LSSU_SIB)
		sl->statem.txc_state = SL_STATE_SLEEPING;
	switch (sl->statem.tx.sio) {
	case LSSU_SIO:
		printd(("%s: %p: SIO ->\n", MOD_NAME, sl));
		break;
	case LSSU_SIN:
		printd(("%s: %p: SIN ->\n", MOD_NAME, sl));
		break;
	case LSSU_SIE:
		printd(("%s: %p: SIE ->\n", MOD_NAME, sl));
		break;
	case LSSU_SIOS:
		printd(("%s: %p: SIOS ->\n", MOD_NAME, sl));
		break;
	case LSSU_SIPO:
		printd(("%s: %p: SIPO ->\n", MOD_NAME, sl));
		break;
	case LSSU_SIB:
		printd(("%s: %p: SIB ->\n", MOD_NAME, sl));
		break;
	}
}

static inline fastcall void
sl_daedt_msu(struct sl *sl, mblk_t *mp)
{
	int len = msgdsize(mp);

	if (sl->option.popt & SS7_POPT_XSN) {
		((sl_ushort *) mp->b_rptr)[0] = htons(sl->statem.tx.N.bsn | sl->statem.tx.N.bib);
		((sl_ushort *) mp->b_rptr)[1] = htons(sl->statem.tx.N.fsn | sl->statem.tx.N.fib);
		((sl_ushort *) mp->b_rptr)[2] = htons(len - 6 < 512 ? len - 6 : 511);
	} else {
		((sl_uchar *) mp->b_rptr)[0] = (sl->statem.tx.N.bsn | sl->statem.tx.N.bib);
		((sl_uchar *) mp->b_rptr)[1] = (sl->statem.tx.N.fsn | sl->statem.tx.N.fib);
		((sl_uchar *) mp->b_rptr)[2] = (len - 3 < 64 ? len - 3 : 63);
	}
	printd(("%s: %p: MSU [%d] ->\n", MOD_NAME, sl, len));
}

static inline mblk_t *
sl_txc_transmission_request(struct sl *sl)
{
	mblk_t *mp = NULL;
	int pcr;

	if (sl->statem.txc_state != SL_STATE_IN_SERVICE)
		return (mp);
	pcr = sl->option.popt & SS7_POPT_PCR;
	if (sl->statem.lssu_available) {
		if ((mp = allocb(7, BPRI_HI))) {
			if (sl->statem.tx.sio == LSSU_SIB)
				sl->statem.lssu_available = 0;
			sl->statem.tx.N.fsn = sl->statem.tx.L.fsn;
			// sl->statem.tx.N.bib = sl->statem.tx.N.bib;
			sl->statem.tx.N.bsn = (sl->statem.tx.X.fsn - 1) & sl->statem.sn_mask;
			// sl->statem.tx.N.fib = sl->statem.tx.N.fib;
			sl_daedt_lssu(sl, mp);
		}
		return (mp);
	}
	if (sl->statem.msu_inhibited) {
		if ((mp = allocb(6, BPRI_HI))) {
			sl->statem.tx.N.fsn = sl->statem.tx.L.fsn;
			// sl->statem.tx.N.bib = sl->statem.tx.N.bib;
			sl->statem.tx.N.bsn = (sl->statem.tx.X.fsn - 1) & sl->statem.sn_mask;
			// sl->statem.tx.N.fib = sl->statem.tx.N.fib;
			sl_daedt_fisu(sl, mp);
		}
		return (mp);
	}
	if (pcr) {
		if ((sl->rtb.q_msgs < sl->config.N1)
		    && (sl->rtb.q_count < sl->config.N2)) {
			sl->statem.forced_retransmission = 0;
			sl->statem.rtb_full = 0;
		}
		if (sl->tb.q_count && sl->statem.rtb_full) {
			sl->statem.forced_retransmission = 1;
		}
	}
	if ((!pcr && sl->statem.retrans_cycle)
	    || (pcr && (sl->statem.forced_retransmission || (!sl->tb.q_count && sl->rtb.q_count)))) {
		mblk_t *bp = sl->statem.z_ptr;

		if (bp && !(mp = dupmsg(bp)))
			return (mp);
		if (!bp && pcr) {
			bp = sl->rtb.q_head;
			if (bp && !(mp = dupmsg(bp)))
				return (mp);
			sl->statem.z_ptr = bp;
			sl->statem.Z = sl->statem.tx.F.fsn;
		}
		if (mp) {
			sl->statem.z_ptr = bp->b_next;
			if (pcr) {
				sl->statem.tx.N.fsn = sl->statem.Z;
				// sl->statem.tx.N.fib = sl->statem.tx.N.fib;
				sl->statem.tx.N.bsn =
				    (sl->statem.tx.X.fsn - 1) & sl->statem.sn_mask;
				// sl->statem.tx.N.bib = sl->statem.tx.N.bib;
				sl->statem.Z = (sl->statem.Z + 1) & sl->statem.sn_mask;
			} else {
				sl->statem.tx.N.fsn =
				    (sl->statem.tx.N.fsn + 1) & sl->statem.sn_mask;
				// sl->statem.tx.N.fib = sl->statem.tx.N.fib;
				sl->statem.tx.N.bsn =
				    (sl->statem.tx.X.fsn - 1) & sl->statem.sn_mask;
				// sl->statem.tx.N.bib = sl->statem.tx.N.bib;
			}
			sl_daedt_msu(sl, mp);
			if (sl->statem.tx.N.fsn == sl->statem.tx.L.fsn || sl->statem.z_ptr == NULL)
				sl->statem.retrans_cycle = 0;
			return (mp);
		}
	}
	if ((!pcr && (!sl->tb.q_count || sl->statem.rtb_full))
	    || (pcr && (!sl->tb.q_count && !sl->rtb.q_count))) {
		if ((mp = allocb(6, BPRI_HI))) {
			sl->statem.tx.N.fsn = sl->statem.tx.L.fsn;
			// sl->statem.tx.N.bib = sl->statem.tx.N.bib;
			sl->statem.tx.N.bsn = (sl->statem.tx.X.fsn - 1) & sl->statem.sn_mask;
			// sl->statem.tx.N.fib = sl->statem.tx.N.fib;
			sl_daedt_fisu(sl, mp);
		}
		return (mp);
	} else {
		spin_lock(&sl->tb.q_lock);
		if ((mp = bufq_head(&sl->tb)) && (mp = dupmsg(mp))) {
			mblk_t *bp = bufq_dequeue(&sl->tb);

			sl->statem.Cm--;
			if (!sl->statem.Cm)
				qenable(sl->iq);	/* back enable */
			sl->statem.tx.L.fsn = (sl->statem.tx.L.fsn + 1) & sl->statem.sn_mask;
			sl->statem.tx.N.fsn = sl->statem.tx.L.fsn;
			if (!sl->rtb.q_count)
				sl_timer_start(sl, t7);
			bufq_queue(&sl->rtb, bp);
			sl->statem.Ct++;
			sl_rc_fsnt_value(sl);
			if (pcr) {
				if ((sl->rtb.q_msgs >= sl->config.N1)
				    || (sl->rtb.q_count >= sl->config.N2)) {
					sl->statem.rtb_full = 1;
					sl->statem.forced_retransmission = 1;
				}
			} else {
				if ((sl->rtb.q_msgs >= sl->config.N1)
				    || (sl->rtb.q_count >= sl->config.N2)
				    || (sl->statem.tx.L.fsn ==
					((sl->statem.tx.F.fsn - 2) & sl->statem.sn_mask)))
					sl->statem.rtb_full = 1;
			}
			// sl->statem.tx.N.bib = sl->statem.tx.N.bib;
			sl->statem.tx.N.bsn = (sl->statem.tx.X.fsn - 1) & sl->statem.sn_mask;
			// sl->statem.tx.N.fib = sl->statem.tx.N.fib;
			sl_daedt_msu(sl, mp);
		}
		spin_unlock(&sl->tb.q_lock);
		return (mp);
	}
}

static inline fastcall int
sl_daedr_start(struct sl *sl, queue_t *q)
{
	return sdt_daedr_start_req(sl->sdt, q);
}

static inline fastcall int
sl_rc_start(struct sl *sl, queue_t *q)
{
	int err;

	if (sl->statem.rc_state == SL_STATE_IDLE) {
		sl->statem.rx.X.fsn = 0;
		sl->statem.rx.X.fib = sl->statem.ib_mask;
		sl->statem.rx.F.fsn = 0;
		sl->statem.rx.T.fsn = sl->statem.sn_mask;
		sl->statem.rtr = 0;	/* Basic only (note 1). */
		if (sl->option.pvar == SS7_PVAR_ANSI_92)
			sl->statem.msu_fisu_accepted = 1;
		else
			sl->statem.msu_fisu_accepted = 0;
		sl->statem.abnormal_bsnr = 0;
		sl->statem.abnormal_fibr = 0;	/* Basic only (note 1). */
		sl->statem.congestion_discard = 0;
		sl->statem.congestion_accept = 0;
		if ((err = sl_daedr_start(sl, q)))
			return (err);
		sl->statem.rc_state = SL_STATE_IN_SERVICE;
		/* 
		 *   Note 1 - Although rtr and abnormal_fibr are only
		 *   applicable to the Basic procedure (and not PCR), these
		 *   state machine variables are never examined by PCR
		 *   routines, so PCR and basic can share the same start
		 *   procedures.
		 */
	}
	return (0);
}

static inline fastcall void
sl_rc_reject_msu_fisu(struct sl *sl)
{
	sl->statem.msu_fisu_accepted = 0;
}

static inline fastcall void
sl_rc_accept_msu_fisu(struct sl *sl)
{
	sl->statem.msu_fisu_accepted = 1;
}

static inline fastcall void
sl_rc_retrieve_fsnx(struct sl *sl)
{
	sl_txc_fsnx_value(sl);	/* error in 93 spec */
	sl->statem.congestion_discard = 0;
	sl->statem.congestion_accept = 0;
	sl_cc_normal(sl);
	sl->statem.rtr = 0;	/* basic only */
}

static inline fastcall void
sl_rc_align_fsnx(struct sl *sl)
{
	sl_txc_fsnx_value(sl);
}

static inline fastcall int
sl_rc_clear_rb(struct sl *sl, queue_t *q)
{
	bufq_purge(&sl->rb);
	// flushq(sl->oq, FLUSHDATA);
	sl->statem.Cr = 0;
	return sl_rb_cleared_ind(sl, q);
}

static inline fastcall int
sl_rc_retrieve_bsnt(struct sl *sl, queue_t *q)
{
	sl->statem.rx.T.bsn = (sl->statem.rx.X.fsn - 1) & 0x7F;
	return sl_bsnt_ind(sl, q, sl->statem.rx.T.bsn);
}

static inline fastcall void
sl_cc_busy(struct sl *sl)
{
	if (sl->statem.cc_state == SL_STATE_NORMAL) {
		sl_txc_send_sib(sl);
		sl_timer_start(sl, t5);
		sl->statem.cc_state = SL_STATE_BUSY;
	}
}

static inline fastcall void
sl_rc_congestion_discard(struct sl *sl)
{
	sl->statem.congestion_discard = 1;
	sl_cc_busy(sl);
}

static inline fastcall void
sl_rc_congestion_accept(struct sl *sl)
{
	sl->statem.congestion_accept = 1;
	sl_cc_busy(sl);
}

static inline fastcall void
sl_rc_no_congestion(struct sl *sl)
{
	sl->statem.congestion_discard = 0;
	sl->statem.congestion_accept = 0;
	sl_cc_normal(sl);
	sl_txc_fsnx_value(sl);
	if (sl->statem.rtr == 1) {	/* rtr never set for PCR */
		ctrace(sl_txc_nack_to_be_sent(sl));
		sl->statem.rx.X.fib = sl->statem.rx.X.fib ? 0 : sl->statem.ib_mask;
	}
}

static inline fastcall void
sl_lsc_congestion_discard(struct sl *sl)
{
	sl_rc_congestion_discard(sl);
	sl->statem.l3_congestion_detect = 1;
}

static inline fastcall void
sl_lsc_congestion_accept(struct sl *sl)
{
	sl_rc_congestion_accept(sl);
	sl->statem.l3_congestion_detect = 1;
}

static inline fastcall int
sl_lsc_no_congestion(struct sl *sl)
{
	sl_rc_no_congestion(sl);
	sl->statem.l3_congestion_detect = 0;
}

static inline fastcall int
sl_lsc_sio(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl->statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
	case SL_STATE_INITIAL_ALIGNMENT:
		break;
	default:
		sl_timer_stop(sl, t1);	/* ok if not running */
		if ((err = sl_out_of_service_ind(sl, q, SL_FAIL_RECEIVED_SIO)))
			return (err);
		sl->statem.failure_reason = SL_FAIL_RECEIVED_SIO;
		sl_rc_stop(sl);
		if ((err = sl_suerm_stop(sl, q)))
			return (err);
		sl_poc_stop(sl);	/* ok if ANSI */
		sl_txc_send_sios(sl);
		sl->statem.emergency = 0;
		/* FIXME: reinspect */
		sl->statem.local_processor_outage = 0;
		sl->statem.remote_processor_outage = 0;	/* ok if ITUT */
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
	return (0);
}

static inline fastcall int
sl_lsc_alignment_not_possible(struct sl *sl, queue_t *q)
{
	int err;

	if ((err = sl_out_of_service_ind(sl, q, SL_FAIL_ALIGNMENT_NOT_POSSIBLE)))
		return (err);
	sl->statem.failure_reason = SL_FAIL_ALIGNMENT_NOT_POSSIBLE;
	sl_rc_stop(sl);
	sl_txc_send_sios(sl);
	sl->statem.local_processor_outage = 0;
	sl->statem.emergency = 0;
	sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	return (0);
}

static inline fastcall int
sl_iac_sio(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl->statem.iac_state) {
	case SL_STATE_NOT_ALIGNED:
		sl_timer_stop(sl, t2);
		if (sl->statem.emergency) {
			sl->statem.t4v = sl->config.t4e;
			printd(("%s: Sending SIE at %lu\n", MOD_NAME, jiffies));
			sl_txc_send_sie(sl);
		} else {
			sl->statem.t4v = sl->config.t4n;
			sl_txc_send_sin(sl);
		}
		sl_timer_start(sl, t3);
		sl->statem.iac_state = SL_STATE_ALIGNED;
		break;
	case SL_STATE_PROVING:
		sl_timer_stop(sl, t4);
		if ((err = sl_aerm_stop(sl, q)))
			return (err);
		sl_timer_start(sl, t3);
		sl->statem.iac_state = SL_STATE_ALIGNED;
		break;
	}
	return (0);
}

static inline fastcall int
sl_iac_sios(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl->statem.iac_state) {
	case SL_STATE_ALIGNED:
	case SL_STATE_PROVING:
		sl_timer_stop(sl, t4);	/* ok if not running */
		if ((err = sl_lsc_alignment_not_possible(sl, q)))
			return (err);
		if ((err = sl_aerm_stop(sl, q)))	/* ok if not running */
			return (err);
		sl_timer_stop(sl, t3);	/* ok if not running */
		sl->statem.emergency = 0;
		sl->statem.iac_state = SL_STATE_IDLE;
		break;
	}
	return (0);
}

static inline fastcall int
sl_lsc_sios(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl->statem.lsc_state) {
	case SL_STATE_ALIGNED_READY:
	case SL_STATE_ALIGNED_NOT_READY:
		sl_timer_stop(sl, t1);	/* ok to stop if not running */
	case SL_STATE_IN_SERVICE:
	case SL_STATE_PROCESSOR_OUTAGE:
		sl_out_of_service_ind(sl, q, SL_FAIL_RECEIVED_SIOS);
		sl->statem.failure_reason = SL_FAIL_RECEIVED_SIOS;
		if ((err = sl_suerm_stop(sl, q)))	/* ok to stop if not running */
			return (err);
		sl_rc_stop(sl);
		sl_poc_stop(sl);	/* ok if ANSI */
		sl_txc_send_sios(sl);
		sl->statem.emergency = 0;
		sl->statem.local_processor_outage = 0;
		sl->statem.remote_processor_outage = 0;	/* ok if ITU */
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
	return (0);
}

static inline fastcall void
sl_lsc_no_processor_outage(struct sl *sl)
{
	if (sl->statem.lsc_state == SL_STATE_PROCESSOR_OUTAGE) {
		sl->statem.processor_outage = 0;
		if (!sl->statem.l3_indication_received)
			return;
		sl->statem.l3_indication_received = 0;
		sl_txc_send_msu(sl);
		sl->statem.local_processor_outage = 0;
		sl_rc_accept_msu_fisu(sl);
		sl->statem.lsc_state = SL_STATE_IN_SERVICE;
	}
}

static inline fastcall void
sl_poc_remote_processor_recovered(struct sl *sl)
{
	switch (sl->statem.poc_state) {
	case SL_STATE_REMOTE_PROCESSOR_OUTAGE:
		sl_lsc_no_processor_outage(sl);
		sl->statem.poc_state = SL_STATE_IDLE;
		return;
	case SL_STATE_BOTH_PROCESSORS_OUT:
		sl->statem.poc_state = SL_STATE_LOCAL_PROCESSOR_OUTAGE;
		return;
	}
}

static inline fastcall int
sl_lsc_fisu_msu_received(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl->statem.lsc_state) {
	case SL_STATE_ALIGNED_READY:
		if ((err = sl_in_service_ind(sl, q)))
			return (err);
		if (sl->option.pvar == SS7_PVAR_ITUT_93)
			sl_rc_accept_msu_fisu(sl);	/* unnecessary */
		sl_timer_stop(sl, t1);
		sl_txc_send_msu(sl);
		sl->statem.lsc_state = SL_STATE_IN_SERVICE;
		break;
	case SL_STATE_ALIGNED_NOT_READY:
		if ((err = sl_in_service_ind(sl, q)))
			return (err);
		sl_timer_stop(sl, t1);
		sl->statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) {
			sl_poc_remote_processor_recovered(sl);
			if ((err = sl_remote_processor_recovered_ind(sl, q)))
				return (err);
		} else {
#if 0
			sl->statem.remote_processor_outage = 0;
			if ((err = sl_remote_processor_recovered_ind(sl, q)))
				return (err);
#else
			/* 
			 *  A deviation from the SDLs has been placed here to
			 *  limit the number of remote processor recovered
			 *  indications which are delivered to L3.  One
			 *  indication is sufficient.
			 */
			if (sl->statem.remote_processor_outage) {
				sl->statem.remote_processor_outage = 0;
				if ((err = sl_remote_processor_recovered_ind(sl, q)))
					return (err);
			}
#endif
		}
		break;
	}
	return (0);
}

static inline fastcall void
sl_poc_remote_processor_outage(struct sl *sl)
{
	switch (sl->statem.poc_state) {
	case SL_STATE_IDLE:
		sl->statem.poc_state = SL_STATE_REMOTE_PROCESSOR_OUTAGE;
		return;
	case SL_STATE_LOCAL_PROCESSOR_OUTAGE:
		sl->statem.poc_state = SL_STATE_BOTH_PROCESSORS_OUT;
		return;
	}
}

static inline fastcall void
sl_lsc_sib(struct sl *sl)
{
	switch (sl->statem.lsc_state) {
	case SL_STATE_IN_SERVICE:
	case SL_STATE_PROCESSOR_OUTAGE:
		sl_txc_sib_received(sl);
		break;
	}
}

static inline fastcall int
sl_lsc_sipo(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl->statem.lsc_state) {
	case SL_STATE_ALIGNED_READY:
		if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) {
			sl_timer_stop(sl, t1);
			if ((err = sl_remote_processor_outage_ind(sl, q)))
				return (err);
			sl_poc_remote_processor_outage(sl);
			sl->statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		} else {
			sl_timer_stop(sl, t1);
			if ((err = sl_remote_processor_outage_ind(sl, q)))
				return (err);
			sl->statem.remote_processor_outage = 1;
			sl->statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		}
		break;
	case SL_STATE_ALIGNED_NOT_READY:
		if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) {
			if ((err = sl_remote_processor_outage_ind(sl, q)))
				return (err);
			sl_poc_remote_processor_outage(sl);
			sl_timer_stop(sl, t1);
			sl->statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		} else {
			if ((err = sl_remote_processor_outage_ind(sl, q)))
				return (err);
			sl->statem.remote_processor_outage = 1;
			sl_timer_stop(sl, t1);
			sl->statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		}
		break;
	case SL_STATE_IN_SERVICE:
		if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) {
			sl_txc_send_fisu(sl);
			if ((err = sl_remote_processor_outage_ind(sl, q)))
				return (err);
			sl_poc_remote_processor_outage(sl);
			sl->statem.processor_outage = 1;	/* remote? */
			sl->statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		} else {
			sl_txc_send_fisu(sl);
			if ((err = sl_remote_processor_outage_ind(sl, q)))
				return (err);
			sl->statem.remote_processor_outage = 1;
			sl_rc_align_fsnx(sl);
			sl_cc_stop(sl);
			sl->statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		}
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) {
			if ((err = sl_remote_processor_outage_ind(sl, q)))
				return (err);
			sl_poc_remote_processor_outage(sl);
		} else {
#if 0
			sl->statem.remote_processor_outage = 1;
			return sl_remote_processor_outage_ind(sl, q);
#else
			/* 
			 *  A deviation from the SDLs has been placed here to
			 *  limit the number of remote processor outage
			 *  indications which are delivered to L3.  One
			 *  indication is sufficient.
			 */
			if (!sl->statem.remote_processor_outage) {
				sl->statem.remote_processor_outage = 1;
				return sl_remote_processor_outage_ind(sl, q);
			}
#endif
		}
		return (0);
	}
	return (0);
}

static inline fastcall void
sl_poc_local_processor_outage(struct sl *sl)
{
	switch (sl->statem.poc_state) {
	case SL_STATE_IDLE:
		sl->statem.poc_state = SL_STATE_LOCAL_PROCESSOR_OUTAGE;
		return;
	case SL_STATE_REMOTE_PROCESSOR_OUTAGE:
		sl->statem.poc_state = SL_STATE_BOTH_PROCESSORS_OUT;
		return;
	}
}

static inline fastcall int
sl_suerm_start(struct sl *sl, queue_t *q)
{
	return sdt_suerm_start_req(sl->sdt, q);
}

static inline fastcall int
sl_lsc_alignment_complete(struct sl *sl, queue_t *q)
{
	int err;

	if (sl->statem.lsc_state == SL_STATE_INITIAL_ALIGNMENT) {
		if ((err = sl_suerm_start(sl, q)))
			return (err);
		sl_timer_start(sl, t1);
		if (sl->statem.local_processor_outage) {
			if (sl->option.pvar != SS7_PVAR_ANSI_92)
				sl_poc_local_processor_outage(sl);
			sl_txc_send_sipo(sl);
			if (sl->option.pvar != SS7_PVAR_ITUT_93)	/* possible error */
				sl_rc_reject_msu_fisu(sl);
			sl->statem.lsc_state = SL_STATE_ALIGNED_NOT_READY;
		} else {
			sl_txc_send_msu(sl);	/* changed from send_fisu for Q.781 */
			sl_rc_accept_msu_fisu(sl);	/* error in ANSI spec? */
			sl->statem.lsc_state = SL_STATE_ALIGNED_READY;
		}
	}
	return (0);
}

static inline fastcall int
sl_lsc_sin(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl->statem.lsc_state) {
	case SL_STATE_IN_SERVICE:
		if ((err = sl_out_of_service_ind(sl, q, SL_FAIL_RECEIVED_SIN)))
			return (err);
		sl->statem.failure_reason = SL_FAIL_RECEIVED_SIN;
		if ((err = sl_suerm_stop(sl, q)))
			return (err);
		sl_rc_stop(sl);
		sl_txc_send_sios(sl);
		sl->statem.emergency = 0;
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		if ((err = sl_out_of_service_ind(sl, q, SL_FAIL_RECEIVED_SIN)))
			return (err);
		sl->statem.failure_reason = SL_FAIL_RECEIVED_SIN;
		if ((err = sl_suerm_stop(sl, q)))
			return (err);
		sl_rc_stop(sl);
		sl_poc_stop(sl);	/* ok if not ITUT */
		sl_txc_send_sios(sl);
		sl->statem.emergency = 0;
		sl->statem.local_processor_outage = 0;
		sl->statem.remote_processor_outage = 0;	/* ok if not ANSI */
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
	return (0);
}

static inline fastcall int
sl_aerm_set_ti_to_tie(struct sl *sl, queue_t *q)
{
	return sdt_aerm_set_ti_to_tie_req(sl->sdt, q);
}

static inline fastcall int
sl_aerm_start(struct sl *sl, queue_t *q)
{
	return sdt_aerm_start_req(sl->sdt, q);
}

static inline fastcall int
sl_iac_sin(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl->statem.iac_state) {
	case SL_STATE_NOT_ALIGNED:
		sl_timer_stop(sl, t2);
		if (sl->statem.emergency) {
			sl->statem.t4v = sl->config.t4e;
			sl_txc_send_sie(sl);
		} else {
			sl->statem.t4v = sl->config.t4n;
			sl_txc_send_sin(sl);
		}
		sl_timer_start(sl, t3);
		sl->statem.iac_state = SL_STATE_ALIGNED;
		break;
	case SL_STATE_ALIGNED:
		sl_timer_stop(sl, t3);
		if (sl->statem.t4v == sl->config.t4e)
			if ((err = sl_aerm_set_ti_to_tie(sl, q)))
				return (err);
		if ((err = sl_aerm_start(sl, q)))
			return (err);
		sl_timer_start(sl, t4);
		sl->statem.further_proving = 0;
		sl->statem.Cp = 0;
		sl->statem.iac_state = SL_STATE_PROVING;
		break;
	}
	return (0);
}

static inline fastcall int
sl_lsc_sie(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl->statem.lsc_state) {
	case SL_STATE_IN_SERVICE:
		sl_out_of_service_ind(sl, q, SL_FAIL_RECEIVED_SIE);
		sl->statem.failure_reason = SL_FAIL_RECEIVED_SIE;
		if ((err = sl_suerm_stop(sl, q)))
			return (err);
		sl_rc_stop(sl);
		sl_txc_send_sios(sl);
		sl->statem.emergency = 0;
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		sl_out_of_service_ind(sl, q, SL_FAIL_RECEIVED_SIE);
		sl->statem.failure_reason = SL_FAIL_RECEIVED_SIE;
		if ((err = sl_suerm_stop(sl, q)))
			return (err);
		sl_rc_stop(sl);
		sl_poc_stop(sl);	/* ok if not ITUT */
		sl_txc_send_sios(sl);
		sl->statem.emergency = 0;
		sl->statem.local_processor_outage = 0;
		sl->statem.remote_processor_outage = 0;	/* ok if not ANSI */
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
	return (0);
}

static inline fastcall int
sl_iac_sie(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl->statem.iac_state) {
	case SL_STATE_NOT_ALIGNED:
		sl_timer_stop(sl, t2);
		if (sl->statem.emergency) {
			sl->statem.t4v = sl->config.t4e;
			sl_txc_send_sie(sl);
		} else {
			sl->statem.t4v = sl->config.t4e;	/* yes e */
			sl_txc_send_sin(sl);
		}
		sl_timer_start(sl, t3);
		sl->statem.iac_state = SL_STATE_ALIGNED;
		break;
	case SL_STATE_ALIGNED:
		printd(("%s: Receiving SIE at %lu\n", MOD_NAME, jiffies));
		sl->statem.t4v = sl->config.t4e;
		sl_timer_stop(sl, t3);
		if ((err = sl_aerm_set_ti_to_tie(sl, q)))
			return (err);
		if ((err = sl_aerm_start(sl, q)))
			return (err);
		sl_timer_start(sl, t4);
		sl->statem.further_proving = 0;
		sl->statem.Cp = 0;
		sl->statem.iac_state = SL_STATE_PROVING;
		break;
	case SL_STATE_PROVING:
		if (sl->statem.t4v == sl->config.t4e)
			break;
		sl_timer_stop(sl, t4);
		sl->statem.t4v = sl->config.t4e;
		if ((err = sl_aerm_stop(sl, q)))
			return (err);
		if ((err = sl_aerm_set_ti_to_tie(sl, q)))
			return (err);
		if ((err = sl_aerm_start(sl, q)))
			return (err);
		sl_timer_start(sl, t4);
		sl->statem.further_proving = 0;
		break;
	}
	return (0);
}

/*
 *  --------------------------------------------------------------------------
 *
 *  These congestion functions are implementation dependent.  We should define
 *  a congestion onset level and set congestion accept at that point.  We
 *  should also define a second congestion onset level and set congestion
 *  discard at that point.  For STREAMS, the upstream congestion can be
 *  detected in two ways: 1) canputnext(): is the upstream module flow
 *  controlled; and, 2) canput(): are we flow controlled.  If the upstream
 *  module is flow controlled, then we can accept MSUs and place them on our
 *  own read queue.  If we are flow contolled, then we have no choice but to
 *  discard the message.  In addition, and because upstream message processing
 *  times are likely more sensitive to the number of backlogged messages than
 *  they are to the number of backlogged message octets, we have some
 *  configurable thresholds of backlogging and keep track of backlogged
 *  messages.
 *
 *  --------------------------------------------------------------------------
 */

static inline fastcall void
sl_rb_congestion_function(struct sl *sl)
{
	if (!sl->statem.l3_congestion_detect) {
		if (sl->statem.l2_congestion_detect) {
			if (sl->statem.Cr <= sl->config.rb_abate && canputnext(sl->oq)) {
				sl_rc_no_congestion(sl);
				sl->statem.l2_congestion_detect = 0;
			}
		} else {
			if (sl->statem.Cr >= sl->config.rb_discard || !canput(sl->oq)) {
				sl_rc_congestion_discard(sl);
				sl->statem.l2_congestion_detect = 1;
			} else if (sl->statem.Cr >= sl->config.rb_accept || !canputnext(sl->oq)) {
				sl_rc_congestion_accept(sl);
				sl->statem.l2_congestion_detect = 1;
			}
		}
	}
}

static inline fastcall int
sl_rc_signal_unit(struct sl *sl, queue_t *q, mblk_t *dp)
{
	int err;
	int pcr = sl->option.popt & SS7_POPT_PCR;

	if (sl->statem.rc_state != SL_STATE_IN_SERVICE)
		goto discard;
	/* 
	 *  Note: the SDT driver must check that the length of the frame is
	 *  within appropriate bounds as specified by the DAEDR in Q.703.  If
	 *  the length of the frame is incorrect, it should indicate
	 *  daedr_error_frame rather than daedr_received_frame.
	 */
	if (sl->option.popt & SS7_POPT_XSN) {
		sl->statem.rx.R.bsn = ntohs(*(sl_ushort *) dp->b_rptr) & 0x0fff;
		sl->statem.rx.R.bib = ntohs(*(sl_ushort *) dp->b_rptr++) & 0x8000;
		sl->statem.rx.R.fsn = ntohs(*(sl_ushort *) dp->b_rptr) & 0x0fff;
		sl->statem.rx.R.fib = ntohs(*(sl_ushort *) dp->b_rptr++) & 0x8000;
		sl->statem.rx.len = ntohs(*(sl_ushort *) dp->b_rptr++) & 0x01ff;
	} else {
		sl->statem.rx.R.bsn = *dp->b_rptr & 0x7f;
		sl->statem.rx.R.bib = *dp->b_rptr++ & 0x80;
		sl->statem.rx.R.fsn = *dp->b_rptr & 0x7f;
		sl->statem.rx.R.fib = *dp->b_rptr++ & 0x80;
		sl->statem.rx.len = *dp->b_rptr++ & 0x3f;
	}
	printd(("%s: %p: [%x/%x] SU [%d] li = %d <-\n", MOD_NAME, sl,
		sl->statem.rx.R.bsn | sl->statem.rx.R.bib,
		sl->statem.rx.R.fsn | sl->statem.rx.R.fib, msgdsize(dp), sl->statem.rx.len));
	if (sl->statem.rx.len == 1) {
		sl->statem.rx.sio = *dp->b_rptr++ & 0x07;
	}
	if (sl->statem.rx.len == 2) {
		sl->statem.rx.sio = *dp->b_rptr++ & 0x07;
		sl->statem.rx.sio = *dp->b_rptr++ & 0x07;
	}
	if (((sl->statem.rx.len) == 1) || ((sl->statem.rx.len) == 2)) {
		switch (sl->statem.rx.sio) {
		case LSSU_SIO:{
			printd(("%s: %p: SIO <-\n", MOD_NAME, sl));
			if ((err = sl_lsc_sio(sl, q)))
				return (err);
			if ((err = sl_iac_sio(sl, q)))
				return (err);
			break;
		}
		case LSSU_SIN:{
			printd(("%s: %p: SIN <-\n", MOD_NAME, sl));
			if ((err = sl_lsc_sin(sl, q)))
				return (err);
			if ((err = sl_iac_sin(sl, q)))
				return (err);
			break;
		}
		case LSSU_SIE:{
			printd(("%s: %p: SIE <-\n", MOD_NAME, sl));
			if ((err = sl_lsc_sie(sl, q)))
				return (err);
			if ((err = sl_iac_sie(sl, q)))
				return (err);
			break;
		}
		case LSSU_SIOS:{
			printd(("%s: %p: SIOS <-\n", MOD_NAME, sl));
			if ((err = sl_lsc_sios(sl, q)))
				return (err);
			if ((err = sl_iac_sios(sl, q)))
				return (err);
			break;
		}
		case LSSU_SIPO:{
			printd(("%s: %p: SIPO <-\n", MOD_NAME, sl));
			if ((err = sl_lsc_sipo(sl, q)))
				return (err);
			break;
		}
		case LSSU_SIB:{
			printd(("%s: %p: SIB <-\n", MOD_NAME, sl));
			sl_lsc_sib(sl);
			break;
		}
		}
		goto discard;
	}
	printd(("%s: %p: [%x/%x] FISU or MSU [%d] li = %d <-\n", MOD_NAME, sl,
		sl->statem.rx.R.bib | sl->statem.rx.R.bsn,
		sl->statem.rx.R.fib | sl->statem.rx.R.fsn, msgdsize(dp), sl->statem.rx.len));
	if (SN_OUTSIDE(((sl->statem.rx.F.fsn - 1) & sl->statem.sn_mask), sl->statem.rx.R.bsn,
		       sl->statem.rx.T.fsn)) {
		if (sl->statem.abnormal_bsnr) {
			if ((err = sl_lsc_link_failure(sl, q, SL_FAIL_ABNORMAL_BSNR)))
				return (err);
			sl->statem.rc_state = SL_STATE_IDLE;
			goto discard;
		} else {
			sl->statem.abnormal_bsnr = 1;
			sl->statem.unb = 0;
			goto discard;
		}
	}
	if (sl->statem.abnormal_bsnr) {
		if (sl->statem.unb == 1) {
			sl->statem.abnormal_bsnr = 0;
		} else {
			sl->statem.unb = 1;
			goto discard;
		}
	}
	if (pcr) {
		if ((err = sl_lsc_fisu_msu_received(sl, q)))
			return (err);
		if ((err = sl_txc_bsnr_and_bibr(sl, q)))
			return (err);
		sl->statem.rx.F.fsn = (sl->statem.rx.R.bsn + 1) & sl->statem.sn_mask;
		if (!sl->statem.msu_fisu_accepted)
			goto discard;
		sl_rb_congestion_function(sl);
		if (sl->statem.congestion_discard) {
			sl_cc_busy(sl);
			goto discard;
		}
		if ((sl->statem.rx.R.fsn == sl->statem.rx.X.fsn)
		    && (sl->statem.rx.len > 2)) {
			sl->statem.rx.X.fsn = (sl->statem.rx.X.fsn + 1) & sl->statem.sn_mask;
			printd(("%s: %p: MSU [%d] <-\n", MOD_NAME, sl, msgdsize(dp)));
			bufq_queue(&sl->rb, dp);
			sl->statem.Cr++;
			if (sl->statem.congestion_accept)
				sl_cc_busy(sl);
			else
				sl_txc_fsnx_value(sl);
			goto absorbed;
		}
		goto discard;
	}
	if (sl->statem.rx.R.fib == sl->statem.rx.X.fib) {
		if (sl->statem.abnormal_fibr) {
			if (sl->statem.unf == 1) {
				sl->statem.abnormal_fibr = 0;
			} else {
				sl->statem.unf = 1;
				goto discard;
			}
		}
		if ((err = sl_lsc_fisu_msu_received(sl, q)))
			return (err);
		if ((err = sl_txc_bsnr_and_bibr(sl, q)))
			return (err);
		sl->statem.rx.F.fsn = (sl->statem.rx.R.bsn + 1) & sl->statem.sn_mask;
		if (!sl->statem.msu_fisu_accepted)
			goto discard;
		sl_rb_congestion_function(sl);
		if (sl->statem.congestion_discard) {
			sl->statem.rtr = 1;
			sl_cc_busy(sl);
			goto discard;
		}
		if ((sl->statem.rx.R.fsn == sl->statem.rx.X.fsn)
		    && (sl->statem.rx.len > 2)) {
			sl->statem.rx.X.fsn = (sl->statem.rx.X.fsn + 1) & sl->statem.sn_mask;
			sl->statem.rtr = 0;
			printd(("%s: %p: MSU [%d] <-\n", MOD_NAME, sl, msgdsize(dp)));
			bufq_queue(&sl->rb, dp);
			sl->statem.Cr++;
			if (sl->statem.congestion_accept)
				sl_cc_busy(sl);
			else
				sl_txc_fsnx_value(sl);
			goto absorbed;
		}
		if ((sl->statem.rx.R.fsn == ((sl->statem.rx.X.fsn - 1) & sl->statem.sn_mask)))
			goto discard;
		else {
			if (sl->statem.congestion_accept) {
				sl->statem.rtr = 1;
				sl_cc_busy(sl);	/* not required? */
				goto discard;
			} else {
				ctrace(sl_txc_nack_to_be_sent(sl));
				sl->statem.rtr = 1;
				sl->statem.rx.X.fib = sl->statem.rx.X.fib ? 0 : sl->statem.ib_mask;
				goto discard;
			}
		}
	} else {
		if (sl->statem.abnormal_fibr) {
			if ((err = sl_lsc_link_failure(sl, q, SL_FAIL_ABNORMAL_FIBR)))
				return (err);
			goto discard;
		}
		if (sl->statem.rtr == 1) {
			if ((err = sl_txc_bsnr_and_bibr(sl, q)))
				return (err);
			sl->statem.rx.F.fsn = (sl->statem.rx.R.bsn + 1) & sl->statem.sn_mask;
			goto discard;
		}
		sl->statem.abnormal_fibr = 1;
		sl->statem.unf = 0;
		goto discard;
	}
      discard:
	freemsg(dp);
      absorbed:
	return (0);
}

static inline fastcall int
sl_lsc_stop(struct sl *sl, queue_t *q)
{
	int err;

	if (sl->statem.lsc_state != SL_STATE_OUT_OF_SERVICE) {
		if ((err = sl_iac_stop(sl, q)))	/* ok if not running */
			return (err);
		sl_timer_stop(sl, t1);	/* ok if not running */
		sl_rc_stop(sl);
		if ((err = sl_suerm_stop(sl, q)))	/* ok if not running */
			return (err);
		sl_poc_stop(sl);	/* ok if not running or not ITUT */
		sl_txc_send_sios(sl);
		sl->statem.emergency = 0;
		sl->statem.local_processor_outage = 0;
		sl->statem.remote_processor_outage = 0;	/* ok of not ANSI */
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	}
	return (0);
}

static inline fastcall int
sl_lsc_clear_rtb(struct sl *sl, queue_t *q)
{
	int err;

	if (sl->statem.lsc_state == SL_STATE_PROCESSOR_OUTAGE) {
		sl->statem.local_processor_outage = 0;
		sl_txc_send_fisu(sl);
		if ((err = sl_txc_clear_rtb(sl, q)))
			return (err);
	}
	return (0);
}

static inline fastcall int
sl_iac_correct_su(struct sl *sl, queue_t *q)
{
	int err;

	if (sl->statem.iac_state == SL_STATE_PROVING) {
		if (sl->statem.further_proving) {
			sl_timer_stop(sl, t4);
			if ((err = sl_aerm_start(sl, q)))
				return (err);
			sl->statem.further_proving = 0;
			sl_timer_start(sl, t4);
		}
	}
	return (0);
}

static inline fastcall int
sl_iac_abort_proving(struct sl *sl, queue_t *q)
{
	int err;

	if (sl->statem.iac_state == SL_STATE_PROVING) {
		sl->statem.Cp++;
		if (sl->statem.Cp == sl->config.M) {
			if ((err = sl_lsc_alignment_not_possible(sl, q)))
				return (err);
			sl_timer_stop(sl, t4);
			if ((err = sl_aerm_stop(sl, q)))
				return (err);
			sl->statem.emergency = 0;
			sl->statem.iac_state = SL_STATE_IDLE;
			return (0);
		}
		sl->statem.further_proving = 1;
	}
	return (0);
}

#define sl_lsc_flush_buffers sl_lsc_clear_buffers
static inline fastcall int
sl_lsc_clear_buffers(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl->statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
		if ((sl->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) {
			if ((err = sl_rtb_cleared_ind(sl, q)))
				return (err);
			sl->statem.local_processor_outage = 0;	/* ??? */
		}
		break;
	case SL_STATE_INITIAL_ALIGNMENT:
		if ((sl->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) {
			if ((err = sl_rtb_cleared_ind(sl, q)))
				return (err);
			sl->statem.local_processor_outage = 0;
		}
		break;
	case SL_STATE_ALIGNED_NOT_READY:
		if ((sl->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) {
			if ((err = sl_rtb_cleared_ind(sl, q)))
				return (err);
			sl->statem.local_processor_outage = 0;
			sl_txc_send_fisu(sl);
			sl->statem.lsc_state = SL_STATE_ALIGNED_READY;
		}
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) {
			sl_txc_flush_buffers(sl);
			sl->statem.l3_indication_received = 1;
			if (sl->statem.processor_outage)
				break;
			sl->statem.l3_indication_received = 0;
			sl_txc_send_msu(sl);
			sl->statem.local_processor_outage = 0;
			sl_rc_accept_msu_fisu(sl);
			sl->statem.lsc_state = SL_STATE_IN_SERVICE;
		} else {
			sl->statem.local_processor_outage = 0;
			if ((err = sl_rc_clear_rb(sl, q)))
				return (err);
			sl_rc_accept_msu_fisu(sl);
			sl_txc_send_fisu(sl);
			if ((err = sl_txc_clear_tb(sl, q)))
				return (err);
			if ((err = sl_txc_clear_rtb(sl, q)))
				return (err);
		}
		break;
	}
	return (0);
}

#if 0
static inline fastcall void
sl_lsc_continue(struct sl *sl)
{
	if (sl->statem.lsc_state == SL_STATE_PROCESSOR_OUTAGE) {
		if (sl->statem.processor_outage)
			return;
		sl->statem.l3_indication_received = 0;
		sl_txc_send_msu(sl);
		sl->statem.local_processor_outage = 0;
		sl_rc_accept_msu_fisu(sl);
		sl->statem.lsc_state = SL_STATE_IN_SERVICE;
	}
}
#endif

static inline fastcall void
sl_poc_local_processor_recovered(struct sl *sl)
{
	switch (sl->statem.poc_state) {
	case SL_STATE_LOCAL_PROCESSOR_OUTAGE:
		sl_lsc_no_processor_outage(sl);
		sl->statem.poc_state = SL_STATE_IDLE;
		return;
	case SL_STATE_BOTH_PROCESSORS_OUT:
		sl->statem.poc_state = SL_STATE_REMOTE_PROCESSOR_OUTAGE;
		return;
	}
}

#define sl_lsc_resume sl_lsc_local_processor_recovered
static inline fastcall int
sl_lsc_local_processor_recovered(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl->statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
		sl->statem.local_processor_outage = 0;
		break;
	case SL_STATE_INITIAL_ALIGNMENT:
		sl->statem.local_processor_outage = 0;
		break;
	case SL_STATE_ALIGNED_READY:
		break;
	case SL_STATE_ALIGNED_NOT_READY:
		if (sl->option.pvar != SS7_PVAR_ANSI_92)
			sl_poc_local_processor_recovered(sl);
		sl->statem.local_processor_outage = 0;
		sl_txc_send_fisu(sl);
		if (sl->option.pvar == SS7_PVAR_ITUT_96)
			sl_rc_accept_msu_fisu(sl);
		sl->statem.lsc_state = SL_STATE_ALIGNED_READY;
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI) {
			sl_poc_local_processor_recovered(sl);
			sl_rc_retrieve_fsnx(sl);
			sl_txc_send_fisu(sl);	/* note 3: in fisu BSN <= FSNX-1 */
			sl->statem.lsc_state = SL_STATE_IN_SERVICE;
		} else {
			sl->statem.local_processor_outage = 0;
			sl_rc_accept_msu_fisu(sl);
			if (sl->statem.remote_processor_outage) {
				sl_txc_send_fisu(sl);
				if ((err = sl_remote_processor_outage_ind(sl, q)))
					return (err);
			} else {
				sl_txc_send_msu(sl);
				sl->statem.lsc_state = SL_STATE_IN_SERVICE;
			}
		}
	}
	return (0);
}

#define sl_lsc_level_3_failure sl_lsc_local_processor_outage
static inline fastcall int
sl_lsc_local_processor_outage(struct sl *sl, queue_t *q)
{
	switch (sl->statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
	case SL_STATE_INITIAL_ALIGNMENT:
		sl->statem.local_processor_outage = 1;
		break;
	case SL_STATE_ALIGNED_READY:
		if (sl->option.pvar == SS7_PVAR_ANSI_92)
			sl->statem.local_processor_outage = 1;
		else
			sl_poc_local_processor_outage(sl);
		sl_txc_send_sipo(sl);
		if (sl->option.pvar != SS7_PVAR_ITUT_93)	/* possible error 93 specs */
			sl_rc_reject_msu_fisu(sl);
		sl->statem.lsc_state = SL_STATE_ALIGNED_NOT_READY;
		break;
	case SL_STATE_IN_SERVICE:
		if (sl->option.pvar == SS7_PVAR_ANSI_92) {
			sl->statem.local_processor_outage = 1;
		} else {
			sl_poc_local_processor_outage(sl);
			sl->statem.processor_outage = 1;
		}
		sl_txc_send_sipo(sl);
		sl_rc_reject_msu_fisu(sl);
		if (sl->option.pvar == SS7_PVAR_ANSI_92) {
			sl_rc_align_fsnx(sl);
			sl_cc_stop(sl);
		}
		sl->statem.lsc_state = SL_STATE_PROCESSOR_OUTAGE;
		break;
	case SL_STATE_PROCESSOR_OUTAGE:
		if (sl->option.pvar == SS7_PVAR_ANSI_92)
			sl->statem.local_processor_outage = 1;
		else
			sl_poc_local_processor_outage(sl);
		sl_txc_send_sipo(sl);
		break;
	}
	return (0);
}

static inline fastcall int
sl_iac_emergency(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl->statem.iac_state) {
	case SL_STATE_PROVING:
		sl_txc_send_sie(sl);
		sl_timer_stop(sl, t4);
		sl->statem.t4v = sl->config.t4e;
		if ((err = sl_aerm_stop(sl, q)))
			return (err);
		if ((err = sl_aerm_set_ti_to_tie(sl, q)))
			return (err);
		if ((err = sl_aerm_start(sl, q)))
			return (err);
		sl_timer_start(sl, t4);
		sl->statem.further_proving = 0;
		break;
	case SL_STATE_ALIGNED:
		sl_txc_send_sie(sl);
		sl->statem.t4v = sl->config.t4e;
		break;
	case SL_STATE_IDLE:
	case SL_STATE_NOT_ALIGNED:
		sl->statem.emergency = 1;
		break;
	}
	return (0);
}

static inline fastcall int
sl_lsc_emergency(struct sl *sl, queue_t *q)
{
	int err;

	if ((err = sl_iac_emergency(sl, q)))	/* added to pass Q.781/Test 1.20 */
		return (err);
	sl->statem.emergency = 1;
	return (0);
}

static inline fastcall void
sl_lsc_emergency_ceases(struct sl *sl)
{
	sl->statem.emergency = 0;
}

static inline fastcall void
sl_iac_start(struct sl *sl)
{
	if (sl->statem.iac_state == SL_STATE_IDLE) {
		sl_txc_send_sio(sl);
		sl_timer_start(sl, t2);
		sl->statem.iac_state = SL_STATE_NOT_ALIGNED;
	}
}

static inline fastcall int
sl_daedt_start(struct sl *sl, queue_t *q)
{
	return sdt_daedt_start_req(sl->sdt, q);
}

static inline fastcall int
sl_txc_start(struct sl *sl, queue_t *q)
{
	sl->statem.forced_retransmission = 0;	/* ok if basic */
	sl->statem.sib_received = 0;
	sl->statem.Ct = 0;
	sl->statem.rtb_full = 0;
	sl->statem.clear_rtb = 0;	/* ok if ITU */
	if ((sl->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_ANSI) {
		sl->statem.tx.sio = LSSU_SIOS;
		sl->statem.lssu_available = 1;
	}
	sl->statem.msu_inhibited = 0;
	if (sl->option.popt & SS7_POPT_XSN) {
		sl->statem.sn_mask = 0x7fff;
		sl->statem.ib_mask = 0x8000;
	} else {
		sl->statem.sn_mask = 0x7f;
		sl->statem.ib_mask = 0x80;
	}
	sl->statem.tx.L.fsn = sl->statem.tx.N.fsn = sl->statem.sn_mask;
	sl->statem.tx.X.fsn = 0;
	sl->statem.tx.N.fib = sl->statem.tx.N.bib = sl->statem.ib_mask;
	sl->statem.tx.F.fsn = 0;
	ptrace(("%s: %p: [%x/%x] %ld\n", MOD_NAME, sl, sl->statem.tx.N.bib | sl->statem.tx.N.bsn,
		sl->statem.tx.N.fib | sl->statem.tx.N.fsn, sl->option.popt & SS7_POPT_PCR));
	sl->statem.Cm = 0;
	sl->statem.Z = 0;
	sl->statem.z_ptr = NULL;	/* ok if basic */
	if (sl->statem.txc_state == SL_STATE_IDLE) {
		int err;

		if ((sl->option.pvar & SS7_PVAR_MASK) != SS7_PVAR_ANSI)
			sl->statem.lssu_available = 0;
		if ((err = sl_daedt_start(sl, q)))
			return (err);
	}
	sl->statem.txc_state = SL_STATE_SLEEPING;
	return (0);
}

static inline fastcall int
sl_lsc_start(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl->statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
		if ((err = sl_rc_start(sl, q)))
			return (err);
		if ((err = sl_txc_start(sl, q)))	/* Note 2 */
			return (err);
		if (sl->statem.emergency)
			if ((err = sl_iac_emergency(sl, q)))
				return (err);
		sl_iac_start(sl);
		sl->statem.lsc_state = SL_STATE_INITIAL_ALIGNMENT;
	}
	return (0);
}

/*
 *  Note 2: There is a difference here between ANSI_92 and ITUT_93/96 in that
 *  the transmitters in the ANSI_92 case may transmit one or two SIOSs before
 *  transmitting the first SIO of the initial alignment procedure.  ITUT will
 *  continue idling FISU or LSSU as before the start, then transmit the first
 *  SIO.  These are equivalent.  Because the LSC is in the OUT OF SERVICE
 *  state, the transmitters should be idling SIOS anyway.
 */

static inline fastcall int
sl_lsc_retrieve_bsnt(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl->statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
	case SL_STATE_PROCESSOR_OUTAGE:
		if ((err = sl_rc_retrieve_bsnt(sl, q)))
			return (err);
		break;
	}
	return (0);
}

static inline fastcall int
sl_lsc_retrieval_request_and_fsnc(struct sl *sl, queue_t *q, sl_ulong fsnc)
{
	int err;

	switch (sl->statem.lsc_state) {
	case SL_STATE_OUT_OF_SERVICE:
	case SL_STATE_PROCESSOR_OUTAGE:
		if ((err = sl_txc_retrieval_request_and_fsnc(sl, q, fsnc)))
			return (err);
		break;
	}
	return (0);
}

static inline fastcall int
sl_aerm_set_ti_to_tin(struct sl *sl, queue_t *q)
{
	return sdt_aerm_set_ti_to_tin_req(sl->sdt, q);
}

/*
 *  This power-on sequence should only be performed once, regardless of how
 *  many times the device driver is opened or closed.  This initializes the
 *  transmitters to send SIOS and should never be changed hence.
 */

static inline fastcall int
sl_lsc_power_on(struct sl *sl, queue_t *q)
{
	int err;

	switch (sl->statem.lsc_state) {
	case SL_STATE_POWER_OFF:
		if ((err = sl_txc_start(sl, q)))	/* Note 3 */
			return (err);
		sl_txc_send_sios(sl);	/* not necessary for ANSI */
		if ((err = sl_aerm_set_ti_to_tin(sl, q)))
			return (err);
		sl->statem.local_processor_outage = 0;
		sl->statem.emergency = 0;
		sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
		break;
	}
	return (0);
}

/*
 *  Note 3: There is a difference here between ANSI_92 and ITUT_93/96 in that
 *  the transmitters in the ITUT case may transmit one or two FISUs before
 *  transmitting SIOS on initial power-up.  ANSI will send SIOS on power-up.
 *  ANSI is the correct procedure as transmitters should always idle SIOS on
 *  power-up.
 */

/*
 *  The transmit congestion algorithm is an implementation dependent algorithm
 *  but is suggested as being based on TB and/or RTB buffer occupancy.  With
 *  STREAMS we can use octet count buffer occupancy over message count
 *  occupancy, because congestion in transmission is more related to octet
 *  count (because it determines transmission latency).
 *
 *  We check the total buffer occupancy and apply the necessary congestion
 *  control signal as per configured abatement, onset and discard thresholds.
 */
static int
sl_check_congestion(struct sl *sl, queue_t *q)
{
	unsigned int occupancy = sl->iq->q_count + sl->tb.q_count + sl->rtb.q_count;
	int old_cong_status = sl->statem.cong_status;
	int old_disc_status = sl->statem.disc_status;
	int multi = sl->option.popt & SS7_POPT_MPLEV;

	switch (sl->statem.cong_status) {
	case 0:
		if (occupancy >= sl->config.tb_onset_1) {
			sl->statem.cong_status = 1;
			if (occupancy >= sl->config.tb_discd_1) {
				sl->statem.disc_status = 1;
				if (!multi)
					break;
				if (occupancy >= sl->config.tb_onset_2) {
					sl->statem.cong_status = 2;
					if (occupancy >= sl->config.tb_discd_2) {
						sl->statem.disc_status = 2;
						if (occupancy >= sl->config.tb_onset_3) {
							sl->statem.cong_status = 3;
							if (occupancy >= sl->config.tb_discd_3) {
								sl->statem.disc_status = 3;
							}
						}
					}
				}
			}
		}
		break;
	case 1:
		if (occupancy < sl->config.tb_abate_1) {
			sl->statem.cong_status = 0;
			sl->statem.disc_status = 0;
		} else {
			if (!multi)
				break;
			if (occupancy >= sl->config.tb_onset_2) {
				sl->statem.cong_status = 2;
				if (occupancy >= sl->config.tb_discd_2) {
					sl->statem.disc_status = 2;
					if (occupancy >= sl->config.tb_onset_3) {
						sl->statem.cong_status = 3;
						if (occupancy >= sl->config.tb_discd_3) {
							sl->statem.disc_status = 3;
						}
					}
				}
			}
		}
		break;
	case 2:
		if (!multi) {
			sl->statem.cong_status = 1;
			sl->statem.disc_status = 1;
			break;
		}
		if (occupancy < sl->config.tb_abate_2) {
			sl->statem.cong_status = 1;
			sl->statem.disc_status = 1;
			if (occupancy < sl->config.tb_abate_1) {
				sl->statem.cong_status = 0;
				sl->statem.disc_status = 0;
			}
		} else if (occupancy >= sl->config.tb_onset_3) {
			sl->statem.cong_status = 3;
			if (occupancy >= sl->config.tb_discd_3) {
				sl->statem.disc_status = 3;
			}
		}
		break;
	case 3:
		if (!multi) {
			sl->statem.cong_status = 1;
			sl->statem.disc_status = 1;
			break;
		}
		if (occupancy < sl->config.tb_abate_3) {
			sl->statem.cong_status = 2;
			sl->statem.disc_status = 2;
			if (occupancy < sl->config.tb_abate_2) {
				sl->statem.cong_status = 1;
				sl->statem.disc_status = 1;
				if (occupancy < sl->config.tb_abate_1) {
					sl->statem.cong_status = 0;
					sl->statem.disc_status = 0;
				}
			}
		}
		break;
	}
	if (sl->statem.cong_status != old_cong_status || sl->statem.disc_status != old_disc_status) {
		if (sl->statem.cong_status < old_cong_status)
			return sl_link_congestion_ceased_ind(sl, q, sl->statem.cong_status,
							     sl->statem.disc_status);
		else {
			if (sl->statem.cong_status > old_cong_status) {
				if (sl->notify.events & SL_EVT_CONGEST_ONSET_IND
				    && !sl->stats.sl_cong_onset_ind[sl->statem.cong_status]++) {
					return sl_link_congested_ind(sl, q, sl->statem.cong_status,
								     sl->statem.disc_status);
				}
			} else {
				if (sl->notify.events & SL_EVT_CONGEST_DISCD_IND
				    && !sl->stats.sl_cong_discd_ind[sl->statem.disc_status]++) {
					return sl_link_congested_ind(sl, q, sl->statem.cong_status,
								     sl->statem.disc_status);
				}
			}
			return sl_link_congested_ind(sl, q, sl->statem.cong_status,
						     sl->statem.disc_status);
		}
	}
	return (0);
}

static inline fastcall int
sl_txc_message_for_transmission(struct sl *sl, queue_t *q, mblk_t *dp)
{
	int err;

	bufq_queue(&sl->tb, dp);
	sl->statem.Cm++;
	if (!(err = sl_check_congestion(sl, q))) {
		sl_daedt_transmitter_wakeup(sl);
		return (0);
	}
	bufq_unlink(&sl->tb, dp);
	sl->statem.Cm--;
	return (err);
}

static inline fastcall int
sl_lsc_pdu(queue_t *q, struct sl *sl, mblk_t *dp, ulong priority)
{
	mblk_t *mp;
	int hlen = (sl->option.popt & SS7_POPT_XSN) ? 6 : 3;

	if (MBLKHEAD(dp) >= hlen) {
		mp = dp;
		mp->b_rptr -= hlen;
	} else if ((mp = mi_allocb(q, hlen, BPRI_MED))) {
		mp->b_cont = dp;
		mp->b_wptr = mp->b_rptr + hlen;
	} else {
		return (-ENOBUFS);
	}
	bzero(mp->b_rptr, hlen);
	if ((sl->option.pvar & SS7_PVAR_MASK) == SS7_PVAR_JTTC)
		mp->b_rptr[hlen - 1] = (priority << 6);
	return sl_txc_message_for_transmission(sl, q, mp);
}

/*
 *  ========================================================================
 *
 *  EVENTS
 *
 *  ========================================================================
 */

/*
 *  TX WAKEUP
 *  -----------------------------------
 */
static fastcall void
sl_tx_wakeup(struct sl *sl)
{
	struct sdt *sdt = sl->sdt;
	mblk_t *mp;

	while ((sl->statem.txc_state == SL_STATE_IN_SERVICE) && canputnext(sdt->oq)
	       && (mp = sl_txc_transmission_request(sl))) {
		printd(("%s: %p: M_DATA [%d] ->\n", MOD_NAME, sl, msgdsize(mp)));
		putnext(sdt->oq, mp);
	}
	return;
}

/*
 *  RX WAKEUP
 *  -----------------------------------
 */
static streamscall void
sl_rx_wakeup(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);

	if (sl->rb.q_msgs && canputnext(sl->oq)) {
		switch (sl->statem.lsc_state) {
		case SL_STATE_INITIAL_ALIGNMENT:
		case SL_STATE_ALIGNED_READY:
		case SL_STATE_ALIGNED_NOT_READY:
			/* 
			   we shouldn't have receive buffers around in these states */
			swerr();
			bufq_purge(&sl->rb);
			break;
		case SL_STATE_OUT_OF_SERVICE:
			/* 
			   we keep receive buffers hanging around in these states */
			return;
		case SL_STATE_PROCESSOR_OUTAGE:
			if (sl->statem.local_processor_outage)
				/* 
				   we can't deliver */
				break;
			/* 
			   fall thru */
		case SL_STATE_IN_SERVICE:
			/* 
			   when in service we deliver as many buffers as we can */
			do {
				mblk_t *mp;

				mp = bufq_dequeue(&sl->rb);
				sl->statem.Cr--;
				printd(("%s: %p: <- MSU [%d]\n", MOD_NAME, sl, msgdsize(mp)));
				putnext(sl->oq, mp);
			} while (sl->rb.q_msgs && canputnext(sl->oq));
		}
	}
	return;
}

/*
 *  =========================================================================
 *
 *  TIMER EVENTS
 *
 *  =========================================================================
 */
/**
 * sl_t1_timeout: - timer T1 timeout
 * @sl: private structure
 * @q: active queue
 * @tp: timer message
 */
static int
sl_t1_timeout(struct sl *sl, queue_t *q, mblk_t *tp)
{
	int err;

	if ((err = sl_out_of_service_ind(sl, q, SL_FAIL_T1_TIMEOUT)))
		return (err);
	sl->statem.failure_reason = SL_FAIL_T1_TIMEOUT;
	sl_rc_stop(sl);
	if ((err = sl_suerm_stop(sl, q)))
		return (err);
	sl_txc_send_sios(sl);
	sl->statem.emergency = 0;
	if (sl->statem.lsc_state == SL_STATE_ALIGNED_NOT_READY) {
		sl_poc_stop(sl);	/* ok if ANSI */
		sl->statem.local_processor_outage = 0;
	}
	sl->statem.lsc_state = SL_STATE_OUT_OF_SERVICE;
	return (0);
}

/**
 * sl_t2_timeout: - timer T2 timeout
 * @sl: private structure
 * @q: active queue
 * @tp: timer message
 */
static int
sl_t2_timeout(struct sl *sl, queue_t *q, mblk_t *tp)
{
	int err;

	if (sl->statem.iac_state == SL_STATE_NOT_ALIGNED) {
		if ((err = sl_lsc_alignment_not_possible(sl, q)))
			return (err);
		sl->statem.emergency = 0;
		sl->statem.iac_state = SL_STATE_IDLE;
	}
	return (0);
}

/**
 * sl_t3_timeout: - timer T3 timeout
 * @sl: private structure
 * @q: active queue
 * @tp: timer message
 */
static int
sl_t3_timeout(struct sl *sl, queue_t *q, mblk_t *tp)
{
	int err;

	if (sl->statem.iac_state == SL_STATE_ALIGNED) {
		if ((err = sl_lsc_alignment_not_possible(sl, q)))
			return (err);
		sl->statem.emergency = 0;
		sl->statem.iac_state = SL_STATE_IDLE;
	}
	return (0);
}

/**
 * sl_t4_timeout: - timer T4 timeout
 * @sl: private structure
 * @q: active queue
 * @tp: timer message
 */
static int
sl_t4_timeout(struct sl *sl, queue_t *q, mblk_t *tp)
{
	int err;

	if (sl->statem.iac_state == SL_STATE_PROVING) {
		if (sl->statem.further_proving) {
			if ((err = sl_aerm_start(sl, q)))
				return (err);
			sl_timer_start(sl, t4);
			sl->statem.further_proving = 0;
		} else {
			if ((err = sl_lsc_alignment_complete(sl, q)))
				return (err);
			if ((err = sl_aerm_stop(sl, q)))
				return (err);
			sl->statem.emergency = 0;
			sl->statem.iac_state = SL_STATE_IDLE;
		}
	}
	return (0);
}

/**
 * sl_t5_timeout: - timer T5 timeout
 * @sl: private structure
 * @q: active queue
 * @tp: timer message
 */
static int
sl_t5_timeout(struct sl *sl, queue_t *q, mblk_t *tp)
{
	if (sl->statem.cc_state == SL_STATE_BUSY) {
		sl_txc_send_sib(sl);
		sl_timer_start(sl, t5);
	}
	return (0);
}

/**
 * sl_t6_timeout: - timer T6 timeout
 * @sl: private structure
 * @q: active queue
 * @tp: timer message
 */
static int
sl_t6_timeout(struct sl *sl, queue_t *q, mblk_t *tp)
{
	int err;

	if ((err = sl_lsc_link_failure(sl, q, SL_FAIL_CONG_TIMEOUT)))
		return (err);
	sl->statem.sib_received = 0;
	sl_timer_stop(sl, t7);
	return (0);
}

/**
 * sl_t7_timeout: - timer T7 timeout
 * @sl: private structure
 * @q: active queue
 * @tp: timer message
 */
static int
sl_t7_timeout(struct sl *sl, queue_t *q, mblk_t *tp)
{
	int err;

	if ((err = sl_lsc_link_failure(sl, q, SL_FAIL_ACK_TIMEOUT)))
		return (err);
	sl_timer_stop(sl, t6);
	if (sl->option.pvar == SS7_PVAR_ITUT_96)
		sl->statem.sib_received = 0;
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  LM User -> LM Provider Primitives
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  LMI_INFO_REQ
 *  -----------------------------------
 *  We just pass info requests along and service them on reply from the lower
 *  level.
 */
static int
lmi_info_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_info_req_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	return (QR_PASSFLOW);
      emsgsize:
	return lmi_error_ack(sl, q, mp, LMI_INFO_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 *  We do not peform attaches at this level.  That is the responsibility of
 *  the SDT driver.  After minor checking, we pass these requests along.
 */
static int
lmi_attach_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state == LMI_UNUSABLE)
		goto eagain;
	if (sl->i_style != LMI_STYLE2)
		goto eopnotsupp;
	if (sl->i_state != LMI_UNATTACHED)
		goto outstate;
	if (unlikely(!MBLKIN(mp, 0, sizeof(*p) + 2)))
		goto badppa;
	sl->i_state = LMI_ATTACH_PENDING;
	return (QR_PASSFLOW);
      badppa:
	ptrace(("%s: PROTO: bad ppa (too short)\n", MOD_NAME));
	return lmi_error_ack(sl, q, mp, LMI_ATTACH_REQ, LMI_BADPPA, EMSGSIZE);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return lmi_error_ack(sl, q, mp, LMI_ATTACH_REQ, LMI_OUTSTATE, EPROTO);
      eopnotsupp:
	ptrace(("%s: PROTO: primitive not support for style\n", MOD_NAME));
	return lmi_error_ack(sl, q, mp, LMI_ATTACH_REQ, LMI_NOTSUPP, EOPNOTSUPP);
      eagain:
	/* 
	   wait for stream to become usable */
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return lmi_error_ack(sl, q, mp, LMI_ATTACH_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_DETACH_REQ
 *  -----------------------------------
 *  We do not perform detaches.  SDT does.  We do some simple checks and pass
 *  the primitive along.
 */
static int
lmi_detach_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state == LMI_UNUSABLE)
		goto eagain;
	if (sl->i_style != LMI_STYLE2)
		goto eopnotsupp;
	if (sl->i_state != LMI_DISABLED)
		goto outstate;
	sl->i_state = LMI_DETACH_PENDING;
	return (QR_PASSALONG);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return lmi_error_ack(sl, q, mp, LMI_DETACH_REQ, LMI_OUTSTATE, EPROTO);
      eopnotsupp:
	ptrace(("%s: PROTO: primitive not support for style\n", MOD_NAME));
	return lmi_error_ack(sl, q, mp, LMI_ATTACH_REQ, LMI_NOTSUPP, EOPNOTSUPP);
      eagain:
	/* 
	   wait for stream to become usable */
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return lmi_error_ack(sl, q, mp, LMI_DETACH_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 *  We must allow SDT a crack at enabling before we fully enable.  We commit
 *  the enable when the LMI_ENABLE_CON is returned from below.
 */
static int
lmi_enable_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state == LMI_UNUSABLE)
		goto eagain;
	if (sl->i_state != LMI_DISABLED)
		goto outstate;
	sl->i_state = LMI_ENABLE_PENDING;
	return (QR_PASSALONG);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return lmi_error_ack(sl, q, mp, LMI_ENABLE_REQ, LMI_OUTSTATE, EPROTO);
      eagain:
	/* 
	   wait for stream to become usable */
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return lmi_error_ack(sl, q, mp, LMI_ENABLE_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 *  We must allow SDT a crack at disable before we fully disable.  We commit
 *  the enable when the LMI_DISABLE_CON is returned from below.
 */
static int
lmi_disable_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state == LMI_UNUSABLE)
		goto eagain;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	sl->i_state = LMI_DISABLE_PENDING;
	return (QR_PASSALONG);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return lmi_error_ack(sl, q, mp, LMI_DISABLE_REQ, LMI_OUTSTATE, EPROTO);
      eagain:
	/* 
	   wait for stream to become available */
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return lmi_error_ack(sl, q, mp, LMI_DISABLE_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 */
static int
lmi_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	return (QR_PASSALONG);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return lmi_error_ack(sl, q, mp, LMI_OPTMGMT_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SL User -> SL Provider Primitives
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_DATA
 *  -----------------------------------
 */
static inline fastcall int
sl_data_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct sl *sl = SL_PRIV(q);

	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	return sl_lsc_pdu(q, sl, mp, 0);
      outstate:
	swerr();
	return (-EPROTO);
}

/*
 *  SL_PDU_REQ
 *  -----------------------------------
 */
static int
sl_pdu_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_pdu_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	if ((err = sl_lsc_pdu(q, sl, mp->b_cont, p->sl_mp)))
		return (err);
	freeb(mp);
	return (0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_EMERGENCY_REQ
 *  -----------------------------------
 */
static int
sl_emergency_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_emergency_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	if ((err = sl_lsc_emergency(sl, q)))
		return (err);
	freemsg(mp);
	return (0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_EMERGENCY_CEASES_REQ
 *  -----------------------------------
 */
static int
sl_emergency_ceases_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_emergency_ceases_req_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	sl_lsc_emergency_ceases(sl);
	freemsg(mp);
	return (0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_START_REQ
 *  -----------------------------------
 */
static int
sl_start_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_start_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	if ((err = sl_lsc_start(sl, q)))
		return (err);
	freemsg(mp);
	return (0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_STOP_REQ
 *  -----------------------------------
 */
static int
sl_stop_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_stop_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	if ((err = sl_lsc_stop(sl, q)))
		return (err);
	freemsg(mp);
	return (0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_RETRIEVE_BSNT_REQ
 *  -----------------------------------
 */
static int
sl_retrieve_bsnt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieve_bsnt_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	if ((err = sl_lsc_retrieve_bsnt(sl, q)))
		return (err);
	freemsg(mp);
	return (0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_RETRIEVAL_REQUEST_AND_FSNC_REQ
 *  -----------------------------------
 */
static int
sl_retrieval_request_and_fsnc_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_retrieval_req_and_fsnc_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	if ((err = sl_lsc_retrieval_request_and_fsnc(sl, q, p->sl_fsnc)))
		return (err);
	freemsg(mp);
	return (0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_RESUME_REQ
 *  -----------------------------------
 */
static int
sl_resume_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_resume_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	if ((err = sl_lsc_resume(sl, q)))
		return (err);
	freemsg(mp);
	return (0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_CLEAR_BUFFERS_REQ
 *  -----------------------------------
 */
static int
sl_clear_buffers_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clear_buffers_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	if ((err = sl_lsc_clear_buffers(sl, q)))
		return (err);
	freemsg(mp);
	return (0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_CLEAR_RTB_REQ
 *  -----------------------------------
 */
static int
sl_clear_rtb_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_clear_rtb_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	if ((err = sl_lsc_clear_rtb(sl, q)))
		return (err);
	freemsg(mp);
	return (0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_LOCAL_PROCESSOR_OUTAGE_REQ
 *  -----------------------------------
 */
static int
sl_local_processor_outage_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_local_proc_outage_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	if ((err = sl_lsc_local_processor_outage(sl, q)))
		return (err);
	freemsg(mp);
	return (0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_CONGESTION_DISCARD_REQ
 *  -----------------------------------
 */
static int
sl_congestion_discard_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_cong_discard_req_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	sl_lsc_congestion_discard(sl);
	freemsg(mp);
	return (0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_CONGESTION_ACCEPT_REQ
 *  -----------------------------------
 */
static int
sl_congestion_accept_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_cong_accept_req_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	sl_lsc_congestion_accept(sl);
	freemsg(mp);
	return (0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_NO_CONGESTION_REQ
 *  -----------------------------------
 */
static int
sl_no_congestion_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_no_cong_req_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	sl_lsc_no_congestion(sl);
	freemsg(mp);
	return (0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_POWER_ON_REQ
 *  -----------------------------------
 */
static int
sl_power_on_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	sl_power_on_req_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sl->i_state != LMI_ENABLED)
		goto outstate;
	if ((err = sl_lsc_power_on(sl, q)))
		return (err);
	freemsg(mp);
	return (0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return (-EPROTO);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return (-EMSGSIZE);
}

/*
 *  SL_OPTMGMT_REQ
 *  -----------------------------------
 */
static int
sl_optmgmt_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/*
 *  SL_NOTIFY_REQ
 *  -----------------------------------
 */
static int
sl_notify_req(struct sl *sl, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/*
 *  ------------------------------------------------------------------------
 *
 *  SDT-Provider -> SDT-User Primitives
 *
 *  ------------------------------------------------------------------------
 */

/**
 * sdt_info_ack: - process received LMI_INFO_ACK primitive
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the LMI_INFO_ACK primitive
 *
 * There are two situations in which we get such a reply: first is when we are
 * initially pushed as a module over an SDT (in state LMI_UNUSABLE); second is
 * whenever we need to service an LMI_INFO_REQ.
 */
static int
sdt_info_ack(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	lmi_info_ack_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	{
		int hlen = (sdt->option.popt & SS7_POPT_XSN) ? 6 : 3;
		ulong state = sdt->i_state;

		sdt->i_state = p->lmi_state;
		sdt->i_style = p->lmi_ppa_style;
		switch (state) {
		case LMI_UNUSABLE:
			if (sdt->i_state == LMI_UNUSABLE)
				goto merror;
			return (QR_DONE);
		case LMI_UNATTACHED:
		case LMI_ATTACH_PENDING:
		case LMI_DETACH_PENDING:
		case LMI_DISABLED:
		case LMI_ENABLE_PENDING:
		case LMI_DISABLE_PENDING:
		case LMI_ENABLED:
			p->lmi_version = sdt->i_version;
			p->lmi_state = sdt->i_state;
			p->lmi_max_sdu = p->lmi_max_sdu > hlen ? p->lmi_max_sdu - hlen : 0;
			p->lmi_min_sdu = p->lmi_min_sdu > hlen ? p->lmi_min_sdu - hlen : 0;
			p->lmi_header_len += hlen;
			break;
		default:
			goto outstate;
		}
	}
	return (QR_PASSFLOW);
      outstate:
	swerr();
	return (-EPROTO);
      merror:
	swerr();
	return m_error(sdt->sl, q, -EFAULT);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/**
 * sdt_ok_ack: - process received LMI_OK_ACK primitive
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the LMI_OK_ACK primitive
 *
 * Just try to track the SDT state and then forward the message on.
 */
static int
sdt_ok_ack(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	lmi_ok_ack_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	switch (p->lmi_correct_primitive) {
	case LMI_ATTACH_REQ:
		sdt_set_i_state(sdt, LMI_DISABLED);
		sl_set_i_state(sdt->sl, LMI_DISABLED);
		break;
	case LMI_DETACH_REQ:
		sdt_set_i_state(sdt, LMI_UNATTACHED);
		sl_set_i_state(sdt->sl, LMI_UNATTACHED);
		break;
	default:
		switch (sdt_get_i_state(sdt)) {
		case LMI_ATTACH_PENDING:
			sdt_set_i_state(sdt, LMI_DISABLED);
			sl_set_i_state(sdt->sl, LMI_DISABLED);
			break;
		case LMI_DETACH_PENDING:
			sdt_set_i_state(sdt, LMI_UNATTACHED);
			sl_set_i_state(sdt->sl, LMI_UNATTACHED);
			break;
		default:
			mi_strlog(q, STRLOGNO, SL_ERROR | SL_TRACE,
				  "Received unexpected LMI_OK_ACK");
			sdt_set_i_state(sdt, LMI_UNUSABLE);
			sl_set_i_state(sdt->sl, LMI_UNUSABLE);
			break;
		}
	}
      pass:
	putnext(q, mp);
	return (0);
      emsgsize:
	mi_strlog(q, STRLOGNO, SL_ERROR | SL_TRACE, "Received badly formatted LMI_OK_ACK");
	goto pass;

}

/**
 * sdt_ok_ack: - process received LMI_OK_ACK primitive
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the LMI_OK_ACK primitive
 *
 * Just try to track the SDT state and then forward the message on.
 */
static int
sdt_error_ack(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	lmi_error_ack_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "Received badly formatted LMI_ERROR_ACK");
	switch (sdt_get_i_state(sdt)) {
	case LMI_UNUSABLE:
		break;
	case LMI_ATTACH_PENDING:
		sdt_set_i_state(sdt, LMI_UNATTACHED);
		sl_set_i_state(sdt->sl, LMI_UNATTACHED);
		break;
	case LMI_DETACH_PENDING:
		sdt_set_i_state(sdt, LMI_DISABLED);
		sl_set_i_state(sdt->sl, LMI_DISABLED);
		break;
	case LMI_ENABLE_PENDING:
		sdt_set_i_state(sdt, LMI_DISABLED);
		sl_set_i_state(sdt->sl, LMI_DISABLED);
		break;
	case LMI_DISABLE_PENDING:
		sdt_set_i_state(sdt, LMI_ENABLED);
		sl_set_i_state(sdt->sl, LMI_ENABLED);
		break;
	default:
		mi_strlog(q, STRLOGNO, SL_ERROR | SL_TRACE, "Received unexpected LMI_ERROR_ACK");
		break;
	}
	putnext(q, mp);
	return (0);
}

/**
 * sdt_enable_con: - process received LMI_ENABLE_CON primitive
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the LMI_ENABLE_CON primitive
 */
static int
sdt_enable_con(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	lmi_enable_con_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "Received badly formatted LMI_ENABLE_CON");
	sdt_set_i_state(sdt, LMI_ENABLED);
	sl_set_i_state(sdt->sl, LMI_ENABLED);
	putnext(q, mp);
	return (0);
}

/**
 * sdt_disable_con: - process received LMI_DISABLE_CON primitive
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the LMI_DISABLE_CON primitive
 */
static int
sdt_disable_con(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	lmi_disable_con_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		mi_strlog(q, 0, SL_ERROR | SL_TRACE, "Received badly formatted LMI_DISABLE_CON");
	sdt_set_i_state(sdt, LMI_DISABLED);
	sl_set_i_state(sdt->sl, LMI_DISABLED);
	putnext(q, mp);
	return (0);
}

/*
 *  LMI_OPTMGMT_ACK:
 *  -----------------------------------
 */
static int
sdt_optmgmt_ack(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_ack_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sdt->i_state == LMI_UNUSABLE)
		goto discard;
	if (sdt->i_state != LMI_ENABLED)
		goto outstate;
	return (QR_PASSFLOW);
      outstate:
	swerr();;
	return (-EPROTO);
      discard:
	swerr();
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/**
 * sdt_error_ind: - process received LMI_ERROR_IND primitive
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the LMI_ERROR_IND primiitve
 */
static int
sdt_error_ind(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	lmi_error_ind_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto badprim;
	switch (p->lmi_state) {
	case LMI_UNUSABLE:
	case LMI_UNATTACHED:
	case LMI_ATTACH_PENDING:
	case LMI_DETACH_PENDING:
	case LMI_DISABLED:
	case LMI_ENABLE_PENDING:
	case LMI_DISABLE_PENDING:
	case LMI_ENABLED:
		sdt_set_i_state(sdt, p->lmi_state);
		sl_set_i_state(sdt->sl, p->lmi_state);
		break;
	default:
		goto badprim;
	}
      pass:
	putnext(q, mp);
	return (0);
      badprim:
	mi_strlog(q, 0, SL_ERROR | SL_TRACE, "Received badly formatted LMI_ERROR_IND.");
	goto pass;
}

/*
 *  LMI_STATS_IND:
 *  -----------------------------------
 */
static int
sdt_stats_ind(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	lmi_stats_ind_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sdt->i_state == LMI_UNUSABLE)
		goto discard;
	if (sdt->i_state != LMI_ENABLED)
		goto outstate;
	return (QR_PASSFLOW);
      outstate:
	swerr();;
	return (-EPROTO);
      discard:
	swerr();
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/*
 *  LMI_EVENT_IND:
 *  -----------------------------------
 */
static int
sdt_event_ind(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	lmi_event_ind_t *p = (typeof(p)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto emsgsize;
	if (sdt->i_state == LMI_UNUSABLE)
		goto discard;
	if (sdt->i_state != LMI_ENABLED)
		goto outstate;
	return (QR_PASSFLOW);
      outstate:
	swerr();;
	return (-EPROTO);
      discard:
	swerr();
	return (QR_DONE);
      emsgsize:
	swerr();
	return (-EMSGSIZE);
}

/**
 * sdt_data_ind: - process M_DATA indication
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the M_DATA message
 */
static inline fastcall int
sdt_data_ind(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	return sl_rc_signal_unit(sdt->sl, q, mp);
}

/**
 * sdt_rc_signal_unit_ind: - process received SDT_RC_SIGNAL_UNIT_IND primitive
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the SDT_RC_SIGNAL_UNIT_IND primitive
 */
static inline fastcall int
sdt_rc_signal_unit_ind(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	int err;

	if ((err = sl_rc_signal_unit(sdt->sl, q, mp->b_cont)))
		return (err);
	freeb(mp);
	return (0);
}

/**
 * sdt_rc_congestion_accept_ind: - process received SDT_RC_CONGESTION_ACCEPT_IND primitive
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the * SDT_RC_CONGESTION_ACCEPT_IND primitive
 */
static int
sdt_rc_congestion_accept_ind(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	sl_rc_congestion_accept(sdt->sl);
	freemsg(mp);
	return (0);
}

/**
 * sdt_rc_congestion_discard_ind: - process received SDT_RC_CONGESTION_DISCARD_IND primitive
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the * SDT_RC_CONGESTION_DISCARD_IND primitive
 */
static int
sdt_rc_congestion_discard_ind(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	sl_rc_congestion_discard(sdt->sl);
	freemsg(mp);
	return (0);
}

/**
 * sdt_rc_no_congestion_ind: - process received SDT_RC_NO_CONGESTION_IND primitive
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the * SDT_RC_NO_CONGESTION_IND primitive
 */
static int
sdt_rc_no_congestion_ind(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	sl_rc_no_congestion(sdt->sl);
	freemsg(mp);
	return (0);
}

/**
 * sdt_iac_correct_su_ind: - process received SDT_IAC_CORRECT_SU_IND primitive
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the * SDT_IAC_CORRECT_SU_IND primitive
 */
static inline fastcall int
sdt_iac_correct_su_ind(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	int err;

	if ((err = sl_iac_correct_su(sdt->sl, q)))
		return (err);
	freemsg(mp);
	return (0);
}

/**
 * sdt_iac_abort_proving_ind: - process received SDT_IAC_ABORT_PROVING_IND primitive
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the * SDT_IAC_ABORT_PROVING_IND primitive
 */
static int
sdt_iac_abort_proving_ind(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	int err;

	if ((err = sl_iac_abort_proving(sdt->sl, q)))
		return (err);
	freemsg(mp);
	return (0);
}

/**
 * sdt_lsc_link_failure_ind: - process received SDT_LSC_LINK_FAILURE_IND primitive
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the * SDT_LSC_LINK_FAILURE_IND primitive
 */
static int
sdt_lsc_link_failure_ind(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	int err;

	if ((err = sl_lsc_link_failure(sdt->sl, q, SL_FAIL_SUERM_EIM)))
		return (err);
	freemsg(mp);
	return (0);
}

/**
 * sdt_txc_transmission_request_ind: - process received SDT_TXC_TRANSMISSION_REQUEST_IND primitive
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the SDT_TXC_TRANSMISSION_REQUEST_IND primitive
 */
static int
sdt_txc_transmission_request_ind(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	sl_tx_wakeup(sdt->sl);
	freemsg(mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  INPUT-OUTPUT CONTROLS
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
 * This is step 1 of the LMI input-output control operation.  Step 1 consists
 * of a copyin operation for SET operations and a copyout operation for GET
 * operations.
 */
static noinline fastcall __unlikely int
lmi_ioctl(struct sl *sl, queue_t *q, mblk_t *mp)
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
 * This is step number 2 for SET operations.  This is the result of the
 * implicit or explicit copyin operation.  We can now perform sets and
 * commits.  All SET operations also include a last copyout step that copies
 * out the information actually set.
 */
static noinline fastcall __unlikely int
lmi_copyin(struct sl *sl, queue_t *q, mblk_t *mp, mblk_t *dp)
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
static noinline fastcall __unlikely int
lmi_copyout(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sl_iocname(cp->cp_cmd));
	mi_copyout(q, mp);
	return (0);
}

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
static noinline fastcall __unlikely int
sl_ioctl(struct sl *sl, queue_t *q, mblk_t *mp)
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
static noinline fastcall __unlikely int
sl_copyin(struct sl *sl, queue_t *q, mblk_t *mp, mblk_t *dp)
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
static noinline fastcall __unlikely int
sl_copyout(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	mi_strlog(s->oq, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sl_iocname(cp->cp_cmd));
	mi_copyout(q, mp);
	return (0);
}

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
static noinline __unlikely int
sl_do_ioctl(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*ioc))) || unlikely(!mp->b_cont))
		return (EFAULT);
	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case LMI_IOC_MAGIC:
		return lmi_ioctl(sl, q, mp);
	case SL_IOC_MAGIC:
		return sl_ioctl(sl, q, mp);
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
static noinline __unlikely int
sl_do_copyin(struct sl *sl, queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*cp))) || unlikely(!mp->b_cont))
		return (EFAULT);
	switch (_IOC_TYPE(cp->cp_cmd)) {
	case LMI_IOC_MAGIC:
		return lmi_copyin(sl, q, mp, dp);
	case SL_IOC_MAGIC:
		return sl_copyin(sl, q, mp, dp);
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
static noinline __unlikely int
sl_do_copyout(struct sl *sl, queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*cp))) || unlikely(!mp->b_cont))
		return (EFAULT);
	switch (_IOC_TYPE(cp->cp_cmd)) {
	case LMI_IOC_MAGIC:
		return lmi_copyout(sl, q, mp);
	case SL_IOC_MAGIC:
		return sl_copyout(sl, q, mp);
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

static inline fastcall int
__sl_m_data(struct sl *sl, queue_t *q, mblk_t *mp)
{
#ifndef _OPTMIZE_SIZE
	mi_strlog(sl->oq, STRLOGDA, SL_TRACE, "-> M_DATA");
#endif				/* _OPTMIZE_SIZE */
	return sl_data_req(sl, q, mp);
}
static inline fastcall int
sl_m_data(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int rtn = -EAGAIN;

	if (likely(!!(priv = mi_trylock(q)))) {
		rtn = __sl_m_data(SL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (rtn);
}

/**
 * __sl_m_proto_slow: - process M_(PC)PROTO message, slow
 * @sl: (locked) private structure
 * @q: active queue (write queue)
 * @mp: the M_(PC)PROTO message
 * @prim: the primitive type
 */
static noinline fastcall int
__sl_m_proto_slow(struct sl *sl, queue_t *q, mblk_t *mp, lmi_ulong prim)
{
	int rtn;

	switch (prim) {
	case SL_PDU_REQ:
#ifndef _OPTIMIZE_SPEED
		mi_strlog(sl->oq, STRLOGDA, SL_TRACE, "-> SL_PDU_REQ");
#endif				/* _OPTIMIZE_SPEED */
		break;
	default:
		mi_strlog(sl->oq, STRLOGRX, SL_TRACE, "-> %s", sl_primname(prim));
		break;
	}

	sl_save_total_state(sl);

	switch (prim) {
	case LMI_INFO_REQ:
		rtn = lmi_info_req(sl, q, mp);
		break;
	case LMI_ATTACH_REQ:
		rtn = lmi_attach_req(sl, q, mp);
		break;
	case LMI_DETACH_REQ:
		rtn = lmi_detach_req(sl, q, mp);
		break;
	case LMI_ENABLE_REQ:
		rtn = lmi_enable_req(sl, q, mp);
		break;
	case LMI_DISABLE_REQ:
		rtn = lmi_disable_req(sl, q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		rtn = lmi_optmgmt_req(sl, q, mp);
		break;
	case SL_PDU_REQ:
		rtn = sl_pdu_req(sl, q, mp);
		break;
	case SL_EMERGENCY_REQ:
		rtn = sl_emergency_req(sl, q, mp);
		break;
	case SL_EMERGENCY_CEASES_REQ:
		rtn = sl_emergency_ceases_req(sl, q, mp);
		break;
	case SL_START_REQ:
		rtn = sl_start_req(sl, q, mp);
		break;
	case SL_STOP_REQ:
		rtn = sl_stop_req(sl, q, mp);
		break;
	case SL_RETRIEVE_BSNT_REQ:
		rtn = sl_retrieve_bsnt_req(sl, q, mp);
		break;
	case SL_RETRIEVAL_REQUEST_AND_FSNC_REQ:
		rtn = sl_retrieval_request_and_fsnc_req(sl, q, mp);
		break;
	case SL_CLEAR_BUFFERS_REQ:
		rtn = sl_clear_buffers_req(sl, q, mp);
		break;
	case SL_CLEAR_RTB_REQ:
		rtn = sl_clear_rtb_req(sl, q, mp);
		break;
	case SL_CONTINUE_REQ:
		rtn = sl_continue_req(sl, q, mp);
		break;
	case SL_LOCAL_PROCESSOR_OUTAGE_REQ:
		rtn = sl_local_processor_outage_req(sl, q, mp);
		break;
	case SL_RESUME_REQ:
		rtn = sl_resume_req(sl, q, mp);
		break;
	case SL_CONGESTION_DISCARD_REQ:
		rtn = sl_congestion_discard_req(sl, q, mp);
		break;
	case SL_CONGESTION_ACCEPT_REQ:
		rtn = sl_congestion_accept_req(sl, q, mp);
		break;
	case SL_NO_CONGESTION_REQ:
		rtn = sl_no_congestion_req(sl, q, mp);
		break;
	case SL_POWER_ON_REQ:
		rtn = sl_power_on_req(sl, q, mp);
		break;
	case SL_OPTMGMT_REQ:
		rtn = sl_optmgmt_req(sl, q, mp);
		break;
	case SL_NOTIFY_REQ:
		rtn = sl_notify_req(sl, q, mp);
		break;
	default:
		rtn = lmi_other_req(sl, q, mp);
		break;
	}
	if (unlikely(rtn))
		sl_restore_total_state(sl);
	return (rtn);
}

/**
 * __sl_m_proto: - process M_(PC)PROTO message, fast
 * @sl: (locked) private structure
 * @q: active queue (write queue)
 * @mp: the M_(PC)PROTO message
 */
static inline fastcall int
__sl_m_proto(struct sl *sl, queue_t *q, mblk_t *mp)
{
	lmi_ulong prim;
	int rtn;

	if (likely(MBLKIN(mp, 0, sizeof(prim)))) {
		prim = *(typeof(prim) *) mp->b_rptr;
		if (likely(prim == SL_PDU_REQ)) {
#ifndef _OPTIMIZE_SPEED
			mi_strlog(sdt->oq, STRLOGDA, SL_TRACE, "-> SL_PDU_REQ");
#endif				/* _OPTIMIZE_SPEED */
			rtn = sl_pdu_req(sl, q, mp);
		} else {
			rtn = __sl_m_proto_slow(sl, q, mp, prim);
		}
	} else {
		freemsg(mp);
		rtn = 0;
	}
	return (rtn);
}

/**
 * sl_m_proto: - process M_(PC)PROTO message
 * @q: active queue (write queue)
 * @mp: the M_(PC)PROTO message
 */
static inline fastcall int
sl_m_proto(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int rtn = -EAGAIN;

	if (likely(!!(priv = mi_trylock(q)))) {
		rtn = __sl_m_proto(SL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (rtn);
}

/**
 * __sl_m_sig: - process M_(PC)SIG message
 * @sl: (locked) private structure
 * @q: active queue (write queue)
 * @mp: the M_(PC)SIG message
 */
static inline fastcall int
__sl_m_sig(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int rtn = 0;
	uchar timer;

	if (!mi_timer_valid(mp))
		return (0);

	sl_save_total_state(sl);

	timer = *(int *) mp->b_rptr;

	mi_strlog(sl->oq, STRLOGTO, SL_TRACE, "-> T%c TIMEOUT", (char) ('1' + timer));

	switch (timer) {
	case t1:
		rtn = sl_t1_timeout(sl, q, mp);
		break;
	case t2:
		rtn = sl_t2_timeout(sl, q, mp);
		break;
	case t3:
		rtn = sl_t3_timeout(sl, q, mp);
		break;
	case t4:
		rtn = sl_t4_timeout(sl, q, mp);
		break;
	case t5:
		rtn = sl_t5_timeout(sl, q, mp);
		break;
	case t6:
		rtn = sl_t6_timeout(sl, q, mp);
		break;
	case t7:
		rtn = sl_t7_timeout(sl, q, mp);
		break;
	default:
		rtn = 0;
		break;
	}
	if (rtn != 0) {
		sl_restore_total_state(sl);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	return (rtn);
}

/**
 * sl_m_sig: - process M_(PC)SIG message
 * @q: active queue (write queue)
 * @mp: the M_(PC)SIG message
 */
static inline fastcall int
sl_m_sig(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int rtn = -EAGAIN;

	if (likely(!!(priv = mi_trylock(q)))) {
		rtn = __sl_m_sig(SL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (rtn);
}

/**
 * __sl_m_ioctl: - process M_IOCTL message
 * @sl: (locked) private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCTL message
 */
static inline fastcall int
__sl_m_ioctl(struct sl *sl, queue_t *q, mblk_t *mp)
{
	int err;

	err = sl_do_ioctl(sl, q, mp);
	if (err <= 0)
		return (err);
	mi_copy_done(q, mp, err);
	return (0);
}

/**
 * sl_m_ioctl: - process M_IOCTL message
 * @q: active queue (write queue)
 * @mp: the M_IOCTL message
 */
static inline fastcall int
sl_m_ioctl(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int rtn = -EAGAIN;

	if (likely(!!(priv = mi_trylock(q)))) {
		rtn = __sl_m_ioctl(SL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (rtn);
}

/**
 * __sl_m_iocdata: - process M_IOCDATA message
 * @sl: (locked) private structure
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 */
static inline fastcall int
__sl_m_iocdata(struct sl *sl, queue_t *q, mblk_t *mp)
{
	mblk_t *dp;
	int err;

	switch (mi_copy_state(q, mp, &dp)) {
	case -1:
		err = 0;
		break;
	case MI_COPY_CASE(MI_COPY_IN, 1):
		err = sl_do_copyin(sl, q, mp, dp);
		break;
	case MI_COPY_CASE(MI_COPY_OUT, 1):
		err = sl_do_copyout(sl, q, mp);
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
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 */
static inline fastcall int
sl_m_iocdata(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int rtn = -EAGAIN;

	if (likely(!!(priv = mi_trylock(q)))) {
		rtn = __sl_m_iocdata(SL_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (rtn);
}

/**
 * sl_m_flush: - process M_FLUSH
 * @q: active queue (write queue)
 * @mp: the M_FLUSH message
 */
static inline fastcall int
sl_m_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}

/**
 * sl_m_other: - process other STREAMS message
 * @q: active queue (write queue)
 * @mp: other STREAMS message
 *
 * Simply pass unrecognized messages along.
 */
static noinline fastcall __unlikely int
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
 * @q: active queue (write queue)
 * @mp: the STREAMS message
 */
static noinline fastcall int
sl_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return sl_m_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return sl_m_sig(q, mp);
	case M_IOCTL:
		return sl_m_ioctl(q, mp);
	case M_IOCDATA:
		return sl_m_iocdata(q, mp);
	case M_FLUSH:
		return sl_m_flush(q, mp);
	default:
		return sl_m_other(q, mp);
	case M_DATA:
		return sl_m_data(q, mp);
	}
}

/**
 * sl_msg: - process STREAMS message, fast
 * @q: active queue (write queue)
 * @mp: the STREAMS message
 */
static inline fastcall __hot_write int
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
 * @sl: (locked) private structure
 * @q: active queue (write queue)
 * @mp: the STREAMS message
 */
static noinline fastcall int
__sl_msg_slow(struct sl *sl, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __sl_m_proto(sl, q, mp);
	case M_SIG:
	case M_PCSIG:
		return __sl_m_sig(sl, q, mp);
	case M_IOCTL:
		return __sl_m_ioctl(sl, q, mp);
	case M_IOCDATA:
		return __sl_m_iocdata(sl, q, mp);
	case M_FLUSH:
		return sl_m_flush(q, mp);
	default:
		return sl_m_other(q, mp);
	case M_DATA:
		return __sl_m_data(sl, q, mp);
	}
}

/**
 * __sl_msg: - process STREAMS message, fast
 * @sl: (locked) private structure
 * @q: active queue (write queue)
 * @mp: the STREAMS message
 */
static inline fastcall __hot_out int
__sl_msg(struct sl *sl, queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return __sl_m_data(sl, q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return __sl_m_proto(sl, q, mp);
	return __sl_msg_slow(sl, q, mp);
}

/**
 * __sdt_m_data: - process M_DATA message, fast
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the M_DATA message
 */
static inline fastcall int
__sdt_m_data(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	return sdt_data_ind(sdt, q, mp);
}

/**
 * sdt_m_data: - process M_DATA message
 * @q: active queue (read queue)
 * @mp: the M_DATA message
 */
static inline fastcall int
sdt_m_data(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int rtn = -EAGAIN;

	if (likely(!!(priv = mi_trylock(q)))) {
		rtn = __sdt_m_data(SDT_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (rtn);
}

/**
 * __sdt_m_proto_slow: - proces M_(PC)PROTO message, slow
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the M_(PC)PROTO message
 * @prim: the primitive type
 */
static noinline fastcall int
__sdt_m_proto_slow(struct sdt *sdt, queue_t *q, mblk_t *mp, lmi_ulong prim)
{
	int rtn;

	switch (prim) {
	case SDT_RC_SIGNAL_UNIT_IND:
#ifndef _OPTIMIZE_SPEED
		mi_strlog(sdt->oq, STRLOGDA, SL_TRACE, "SDT_RC_SIGNAL_UNIT_IND <-");
#endif				/* _OPTIMIZE_SPEED */
		break;
	default:
		mi_strlog(sdt->oq, STRLOGRX, SL_TRACE, "%s <-", sdt_primname(prim));
		break;
	}

	sdt_save_total_state(sdt);

	switch (prim) {
	case LMI_INFO_ACK:
		rtn = sdt_info_ack(sdt, q, mp);
		break;
	case LMI_OK_ACK:
		rtn = sdt_ok_ack(sdt, q, mp);
		break;
	case LMI_ERROR_ACK:
		rtn = sdt_error_ack(sdt, q, mp);
		break;
	case LMI_ENABLE_CON:
		rtn = sdt_enable_con(sdt, q, mp);
		break;
	case LMI_DISABLE_CON:
		rtn = sdt_disable_con(sdt, q, mp);
		break;
	case LMI_OPTMGMT_ACK:
		rtn = sdt_optmgmt_ack(sdt, q, mp);
		break;
	case LMI_ERROR_IND:
		rtn = sdt_error_ind(sdt, q, mp);
		break;
	case LMI_STATS_IND:
		rtn = sdt_stats_ind(sdt, q, mp);
		break;
	case LMI_EVENT_IND:
		rtn = sdt_event_ind(sdt, q, mp);
		break;
	case SDT_RC_SIGNAL_UNIT_IND:
		rtn = sdt_rc_signal_unit_ind(sdt, q, mp);
		break;
	case SDT_RC_CONGESTION_ACCEPT_IND:
		rtn = sdt_rc_congestion_accept_ind(sdt, q, mp);
		break;
	case SDT_RC_CONGESTION_DISCARD_IND:
		rtn = sdt_rc_congestion_discard_ind(sdt, q, mp);
		break;
	case SDT_RC_NO_CONGESTION_IND:
		rtn = sdt_rc_no_congestion_ind(sdt, q, mp);
		break;
	case SDT_IAC_CORRECT_SU_IND:
		rtn = sdt_iac_correct_su_ind(sdt, q, mp);
		break;
	case SDT_IAC_ABORT_PROVING_IND:
		rtn = sdt_iac_abort_proving_ind(sdt, q, mp);
		break;
	case SDT_LSC_LINK_FAILURE_IND:
		rtn = sdt_lsc_link_failure_ind(sdt, q, mp);
		break;
	case SDT_TXC_TRANSMISSION_REQUEST_IND:
		rtn = sdt_txc_transmission_request_ind(sdt, q, mp);
		break;
	default:
		rtn = lmi_other_ind(sdt, q, mp);
		break;
	}
	if (rtn)
		sdt_restore_total_state(sdt);
	return (rtn);
}

/**
 * __sdt_m_proto: - proces M_(PC)PROTO message, fast
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the M_(PC)PROTO message
 */
static inline fastcall int
__sdt_m_proto(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	lmi_ulong prim;
	int rtn;

	if (likely(MBLKIN(mp, 0, sizeof(prim)))) {
		prim = *(typeof(prim) *) mp->b_rptr;
		if (likely(prim == SDT_RC_SIGNAL_UNIT_IND)) {
#ifndef _OPTIMIZE_SPEED
			mi_strlog(sdt->oq, STRLOGDA, SL_TRACE, "SDT_RC_SIGNAL_UNIT_IND <-");
#endif				/* _OPTIMIZE_SPEED */
			rtn = sdt_rc_signal_unit_ind(sdt, q, mp);
		} else {
			rtn = __sdt_m_proto_slow(sdt, q, mp, prim);
		}
	} else {
		freemsg(mp);
		rtn = 0;
	}
	return (rtn);
}

/**
 * sdt_m_proto: - proces M_(PC)PROTO message
 * @q: active queue (read queue)
 * @mp: the M_(PC)PROTO message
 */
static inline fastcall int
sdt_m_proto(queue_t *q, mblk_t *mp)
{
	caddr_t priv;
	int rtn = -EAGAIN;

	if (likely(!!(priv = mi_trylock(q)))) {
		rtn = __sdt_m_proto(SDT_PRIV(q), q, mp);
		mi_unlock(priv);
	}
	return (rtn);
}

/**
 * sdt_m_flush: - process M_FLUSH
 * @q: active queue (write queue)
 * @mp: the M_FLUSH message
 */
static inline fastcall int
sdt_m_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
	}
	putnext(q, mp);
	return (0);
}

/**
 * sl_m_other: - process other STREAMS message
 * @q: active queue (read queue)
 * @mp: other STREAMS message
 *
 * Simply pass unrecognized messages along.
 */
static noinline fastcall __unlikely int
sdt_m_other(queue_t *q, mblk_t *mp)
{
	if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

/**
 * __sdt_msg_slow: - process STREAMS message, slow
 * @sdt: (locked) private structure
 * @q: active queue
 * @mp: the STREAMS message
 */
static noinline fastcall int
__sdt_msg_slow(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return __sdt_m_proto(sdt, q, mp);
	case M_FLUSH:
		return sdt_m_flush(q, mp);
	default:
		return sdt_m_other(q, mp);
	case M_DATA:
		return __sdt_m_data(sdt, q, mp);
	}
}

/**
 * __sdt_msg: - process STREAMS message, fast
 * @sdt: (locked) private structure
 * @q: active queue (read queue)
 * @mp: the STREAMS message
 */
static inline fastcall __hot_read int
__sdt_msg(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return __sdt_m_data(sdt, q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return __sdt_m_proto(sdt, q, mp);
	return __sdt_msg_slow(sdt, q, mp);
}

/**
 * sdt_msg_slow: - process STREAMS message, slow
 * @q: active queue (read queue)
 * @mp: the STREAMS message
 */
static noinline fastcall int
sdt_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (__builtin_expect(DB_TYPE(mp), M_PROTO)) {
	case M_PROTO:
	case M_PCPROTO:
		return sdt_m_proto(q, mp);
	case M_FLUSH:
		return sdt_m_flush(q, mp);
	default:
		return sdt_m_other(q, mp);
	case M_DATA:
		return sdt_m_data(q, mp);
	}
}

/**
 * sdt_msg: - process STREAMS message, fast
 * @q: active queue (read queue)
 * @mp: the STREAMS message
 */
static inline fastcall __hot_in int
sdt_msg(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return sdt_m_data(q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return sdt_m_proto(q, mp);
	return sdt_msg_slow(q, mp);
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
 * @q: active queue (upper write queue)
 * @mp: message to put
 *
 * Quick canonical put procedure.
 */
static streamscall __hot_write int
sl_putp(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || q->q_flag & QSVCBUSY)) || sl_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * sl_srvp: - service procedure
 * @q: queue to service (upper write queue)
 *
 * Quick canonical batch mode service procedure.  This si a little quicker for
 * processing bulked messages because it takes the lock once for the entire
 * group of M_DATA messages, instead of once for each message.
 */
static streamscall __hot_out int
sl_srvp(queue_t *q)
{
	mblk_t *mp;

	if (likely(!!(mp = getq(q)))) {
		caddr_t priv;

		if (likely(!!(priv = mi_trylock(q)))) {
			do {
				if (unlikely(__sl_msg(SL_PRIV(q), q, mp)))
					break;
			} while (likely(!!(mp = getq(q))));
			mi_unlock(priv);
		}
		if (unlikely(!!mp))
			switch (DB_TYPE(mp)) {
			case M_SIG:
			case M_PCSIG:
				if (!mi_timer_requeue(mp)) {
					qenable(q);
					break;
				}
				/* fall through */
			default:
				putbq(q, mp);
				break;
			}
	}
	return (0);
}

/**
 * sl_srvp: - service procedure
 * @q: queue to service (lower read queue)
 *
 * Quick canonical batch mode service procedure.  This si a little quicker for
 * processing bulked messages because it takes the lock once for the entire
 * group of M_DATA messages, instead of once for each message.
 */
static streamscall __hot_read int
sdt_srvp(queue_t *q)
{
	mblk_t *mp;

	if (likely(!!(mp = getq(q)))) {
		caddr_t priv;

		if (likely(!!(priv = mi_trylock(q)))) {
			do {
				if (unlikely(__sdt_msg(SDT_PRIV(q), q, mp)))
					break;
			} while (likely(!!(mp = getq(q))));
			mi_unlock(priv);
		}
		if (unlikely(!!mp))
			switch (DB_TYPE(mp)) {
			case M_SIG:
			case M_PCSIG:
				if (!mi_timer_requeue(mp)) {
					qenable(q);
					break;
				}
				/* fall through */
			default:
				putbq(q, mp);
				break;
			}
	}
	return (0);
}

/**
 * sdt_putp: - put procedure
 * @q: active queue (lower read queue)
 * @mp: message to put
 *
 * Quick canonical put procedure.
 */
static streamscall __hot_in int
sdt_putp(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || q->q_flag & QSVCBUSY)) || sdt_msg(q, mp))
		putq(q, mp);
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

static streamscall int sl_qopen(queue_t *, dev_t *, int, int, cred_t *);
static streamscall int sl_qclose(queue_t *, int, cred_t *);

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
	mblk_t *rp;
	int err;

	if (q->q_ptr)
		return (0);

	if ((err = mi_open_comm_cache(&sl_opens, sl_priv_cachep, q, devp, oflags, sflag, crp))) {
		return (err);
	}
	p = PRIV(q);
	bzero(p, sizeof(*p));

	p->r_priv.priv = p;
	p->r_priv.sl = &p->w_priv;
	p->r_priv.oq = WR(q);
	p->r_priv.state.l_state = LMI_UNUSABLE;
	p->r_priv.oldstate.l_state = LMI_UNUSABLE;
	p->r_priv.state.i_state = 0;
	p->r_priv.oldstate.i_state = 0;
	p->r_priv.state.i_flags = 0;
	p->r_priv.oldstate.i_flags = 0;

	p->w_priv.priv = p;
	p->w_priv.sdt = &p->r_priv;
	p->w_priv.oq = q;
	p->w_priv.state.l_state = LMI_UNUSABLE;
	p->w_priv.oldstate.l_state = LMI_UNUSABLE;
	p->w_priv.state.i_state = 0;
	p->w_priv.oldstate.i_state = 0;
	p->w_priv.state.i_flags = 0;
	p->w_priv.oldstate.i_flags = 0;

	qprocson(q);

	/* issue immediate info request */
	if ((err = sdt_info_req(&p->r_priv, q))) {
		sl_qclose(q, oflags, crp);
		return (-err);
	}

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

	(void) p;
	qprocsoff(q);
	mi_close_comm_cache(&sl_opens, sl_priv_cachep, q);
	return (0);
}

/*
 *  =========================================================================
 *
 *  PRIVATE STRUCTURE ALLOCATION, DEALLOCATION AND CACHE
 *
 *  =========================================================================
 */
static kmem_cachep_t sl_priv_cachep = NULL;
static int
sl_init_caches(void)
{
	if (!sl_priv_cachep
	    && !(sl_priv_cachep =
		 kmem_create_cache("sl_priv_cachep", sizeof(struct sl), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate sl_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	} else
		printd(("%s: initialized module private structure cace\n", MOD_NAME));
	return (0);
}
static int
sl_term_caches(void)
{
	if (sl_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(sl_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sl_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			printd(("%s: destroyed sl_priv_cachep\n", MOD_NAME));
#else
		kmem_cache_destroy(sl_priv_cachep);
#endif
	}
	return (0);
}
static struct sl *
sl_alloc_priv(queue_t *q, struct sl **slp, dev_t *devp, cred_t *crp)
{
	struct sl *sl;

	if ((sl = kmem_cache_alloc(sl_priv_cachep, GFP_ATOMIC))) {
		printd(("%s: allocated module private structure\n", MOD_NAME));
		bzero(sl, sizeof(*sl));
		sl_get(sl);	/* first get */
		sl->u.dev.cmajor = getmajor(*devp);
		sl->u.dev.cminor = getminor(*devp);
		sl->cred = *crp;
		(sl->oq = RD(q))->q_ptr = sl_get(sl);
		(sl->iq = WR(q))->q_ptr = sl_get(sl);
		spin_lock_init(&sl->qlock);	/* "sl-queue-lock" */
		sl->o_wakeup = &sl_rx_wakeup;
		sl->i_wakeup = &sl_tx_wakeup;
		sl->i_state = LMI_UNUSABLE;
		sl->i_style = LMI_STYLE1;
		sl->i_version = 1;
		spin_lock_init(&sl->lock);	/* "sl-priv-lock" */
		if ((sl->next = *slp))
			sl->next->prev = &sl->next;
		sl->prev = slp;
		*slp = sl_get(sl);
		printd(("%s: linked module private structure\n", MOD_NAME));
		bufq_init(&sl->rb);
		bufq_init(&sl->tb);
		bufq_init(&sl->rtb);
		/* 
		   configuration defaults */
		sl->option = lmi_default;
		sl->config = sl_default;
		printd(("%s: setting module private structure defaults\n", MOD_NAME));
	} else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", MOD_NAME));
	return (sl);
}
static void
sl_free_priv(queue_t *q)
{
	struct sl *sl = SL_PRIV(q);
	psw_t flags;

	ensure(sl, return);
	spin_lock_irqsave(&sl->lock, flags);
	{
		ss7_unbufcall((str_t *) sl);
		sl_timer_stop(sl, tall);
		bufq_purge(&sl->rb);
		bufq_purge(&sl->tb);
		bufq_purge(&sl->rtb);
		if ((*sl->prev = sl->next))
			sl->next->prev = sl->prev;
		sl->next = NULL;
		sl->prev = &sl->next;
		sl_put(sl);
		sl->oq->q_ptr = NULL;
		flushq(sl->oq, FLUSHALL);
		sl->oq = NULL;
		sl_put(sl);
		sl->iq->q_ptr = NULL;
		flushq(sl->iq, FLUSHALL);
		sl->iq = NULL;
		sl_put(sl);
	}
	spin_unlock_irqrestore(&sl->lock, flags);
	sl_put(sl);		/* final put */
}
static struct sl *
sl_get(struct sl *sl)
{
	atomic_inc(&sl->refcnt);
	return (sl);
}
static void
sl_put(struct sl *sl)
{
	if (atomic_dec_and_test(&sl->refcnt)) {
		kmem_cache_free(sl_priv_cachep, sl);
		printd(("%s: %p: freed sl private structure\n", MOD_NAME, sl));
	}
}

/*
 *  =========================================================================
 *
 *  REGISTRATION AND INITIALIZATION
 *
 *  =========================================================================
 */

static struct qinit sl_rinit = {
	.qi_putp = sdt_putp,		/* Read put (message from below) */
	.qi_srvp = sdt_srvp,		/* Read queue service */
	.qi_qopen = sl_qopen,		/* Each open */
	.qi_qclose = sl_qclose,		/* Last close */
	.qi_minfo = &sl_minfo,		/* Information */
	.qi_mstat = &sl_rstat,		/* Statistics */
};

static struct qinit sl_winit = {
	.qi_putp = sl_putp,		/* Write put (message from above) */
	.qi_srvp = sl_srvp,		/* Write queue service */
	.qi_minfo = &sl_minfo,		/* Information */
	.qi_mstat = &sl_wstat,		/* Statistics */
};

static struct streamtab slinfo = {
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
MODULE_PARM_DESC(modid, "Module ID for SL module.  (0 for allocation.)");

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static struct fmodsw sl_fmod = {
	.f_name = MOD_NAME,
	.f_str = &slinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

/**
 * slinit: - initialize SL
 */
MODULE_STATIC int __init
slinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = sl_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = register_strmod(&sl_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d, err = %d\n", MOD_NAME,
			(int) modid, err);
		sl_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

/**
 * slterminate: - terminate SL module
 */
MODULE_STATIC void __exit
slterminate(void)
{
	int err;

	if ((err = unregister_strmod(&sl_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module, err = %d", MOD_NAME, err);
		return;
	}
	if ((err = sl_term_caches())) {
		cmn_err(CE_WARN, "%s: could not terminate caches, err = %d", MOD_NAME, err);
		return;
	}
	return;
}

module_init(slinit);
module_exit(slterminate);

#endif				/* LINUX */
