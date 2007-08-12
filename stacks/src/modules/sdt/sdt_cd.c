/*****************************************************************************

 @(#) $RCSfile: sdt_cd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/12 15:20:14 $

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

 Last Modified $Date: 2007/08/12 15:20:14 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sdt_cd.c,v $
 Revision 0.9.2.1  2007/08/12 15:20:14  brian
 - added new files

 *****************************************************************************/

#ident "@(#) $RCSfile: sdt_cd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/12 15:20:14 $"

static char const ident[] = "$RCSfile: sdt_cd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/12 15:20:14 $";

/*
 *  This is the SDT-CD STREAMS pushable module.  It's purpose is to convert from a general purpose
 *  raw Communications Device Interface (CDI) Stream to an SS7-specific Signalling Data Terminal
 *  (SDT) Stream.  For the most part this is a one-to-one conversion as both interfaces are an
 *  abstraction of the same thing.  This is maybe not the most efficient way of going about doing
 *  things.  The conversion step might be costly (but then again, it might not: in the main data
 *  flow is could be as little as the overhead of one function call to the modules's put procedure.)
 *
 *  Another more direct approach is to have an SL module that pushes directly over a CD Stream
 *  instead of over an SDT Stream.  Nevertheless, this module can be useful for testing.
 *
 *  Ok, rethink this.  This is not a straigth one to one conversion.  What we and is the CD stream
 *  beneath to be a rather pure HDLC implementation, with FISU/LSSU compression, that is a
 *  DAEDR/DAEDT function.  On top of that we want to place the SDT state machine for AERM/SUERM/EIM
 *  to complete the SDT interface.
 */

#define _LSF_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

#include <ss7/os7/compat.h>

#include <sys/cdi.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>

#define SDT_DESCRIP	"SS7/SDT: (Signalling Data Terminal) STREAMS MODULE."
#define SDT_REVISION	"OpenSS7 $RCSfile: sdt_cd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/12 15:20:14 $"
#define SDT_COPYRIGHT	"Copyright (c) 1997-2007  OpenSS7 Corpopration.  All Rights Reserved."
#define SDT_DEVICE	"Provides OpenSS7 SDT-CD module."
#define SDT_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SDT_LICESNE	"GPL"
#define SDT_BANNER	SDT_DESCRIP	"\n" \
			SDT_REVISION	"\n" \
			SDT_COPYRIGHT	"\n" \
			SDT_DEVICE	"\n" \
			SDT_CONTACT	"\n"
#define SDT_SPLASH	SDT_DESCRIP	" - " \
			SDT_REVISION

#ifdef LINUX
MODULE_AUTHOR(SDT_CONTACT);
MODULE_DESCRIPTION(SDT_DESCRIP);
MODULE_SUPPORTED_DEVICE(SDT_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SDT_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-sdt-cd");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define SDT_CD_MOD_ID		CONFIG_STREAMS_SDT_CD_MODID
#define SDT_CD_MON_NAME		CONFIG_STREAMS_SDT_CD_NAME
#endif

#ifndef SDT_CD_MOD_ID
#define SDT_CD_MOD_ID		"sdt-cd"
#endif				/* SDT_CD_MOD_ID */

#ifndef SDT_CD_MOD_ID
#define SDT_CD_MOD_ID		0
#endif				/* SDT_CD_MOD_ID */

/*
 *  =========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  =========================================================================
 */

#define MOD_ID		SDT_CD_MOD_ID
#define MOD_NAME	SDT_CD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SDT_BANNER
#else				/* MODULE */
#define MOD_BANNER	SDT_SPLASH
#endif				/* MODULE */

static struct module_info sdt_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat sdt_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sdt_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  =========================================================================
 *
 *  PRIVATE STRUCTURE
 *
 *  =========================================================================
 */

struct sdt_pair;
struct sdt;
struct cd;
struct st {
	uint flags;
	uint state;
};

struct sdt {
	struct sdt_pair *pair;
	struct cd *cd;
	queue_t *oq;
	lmi_info_ack_t proto;
	struct st state, oldstate, chkstate;
	struct lmi_option option;
	struct lmi_notify notify;
	struct {
		struct sdt_notify notify;
		struct sdt_config config;
		struct sdt_statem statem;
		struct sdt_stats statsp;
		struct sdt_stats stats;
	} sdt;
	struct {
		struct sdl_notify notify;
		struct sdl_config config;
		struct sdl_statem statem;
		struct sdl_stats statsp;
		struct sdl_stats stats;
	} sdl;
	char ppa[48];
	uint ppa_len;
};

struct cd {
	struct sdt_pair *pair;
	struct sdt *sdt;
	queue_t *oq;
	lmi_info_ack_t proto;
	struct st state, oldstate, chkstate;
	struct {
		struct cd_notify notify;
		struct cd_config config;
		struct cd_statem statem;
		struct cd_statsp statsp;
		struct cd_stats stats;
	} cd;
} struct sdt_pair {
	struct sdt r_priv;
	struct cd w_priv;
};

#define SDT_PRIV(q) (&PRIV(q)->r_priv)
#define  CD_PRIV(q) (&PRIV(q)->w_priv)

#define STRLOGNO	0
#define STRLOGIO	1
#define STRLOGST	2
#define STRLOGTO	3
#define STRLOGRX	4
#define STRLOGTX	5
#define STRLOGTE	6
#define STRLOGDA	7

static inline const char *
sdt_iocname(int cmd)
{
	switch (_IOC_TYPE(cmd)) {
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
		case _IOC_NR(LMI_IOCCPASS):
			return ("LMI_IOCCPASS");
		default:
			return ("(Unknown LMI_IOC number.)");
		}
	case SDL_IOC_MAGIC:
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(SDL_IOCGOPTIONS):
			return ("SDL_IOCGOPTIONS");
		case _IOC_NR(SDL_IOCSOPTIONS):
			return ("SDL_IOCSOPTIONS");
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
		case _IOC_NR(SDL_IOCCPASS):
			return ("SDL_IOCCPASS");
		default:
			return ("(Unknown SDL_IOC number.)");
		}
	case SDT_IOC_MAGIC:
		switch (_IOC_NR(cmd)) {
		case _IOC_NR(SDT_IOCGOPTIONS):
			return ("SDT_IOCGOPTIONS");
		case _IOC_NR(SDT_IOCSOPTIONS):
			return ("SDT_IOCSOPTIONS");
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
		case _IOC_NR(SDT_IOCCPASS):
			return ("SDT_IOCCPASS");
		default:
			return ("(Unknown SDT_IOC number.)");
		}
	default:
		return ("(Unknown IOC type.)");
	}
}

static inline const char *
sdt_primname(sdt_ulong prim)
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
	default:
		return ("(Unknown Primitive)");
	}
}

/**
 * sdt_statename: display LMI state name
 * @state: the state value to display
 */
static const char *
sdt_statename(int state)
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
 * sdt_get_m_state: - get management state for private structure
 * @sdt: private structure
 */
static int
sdt_get_m_state(struct sdt *sdt)
{
	return (sdt->state.state);
}

/**
 * sdt_set_m_state: - set management state for private structure
 * @sdt: private structure
 * @newstate: new state
 */
static int
sdt_set_m_state(struct sdt *sdt, int newstate)
{
	int oldstate = sdt->state.state;

	if (newstate != oldstate) {
		sdt->state.state = newstate;
		mi_strlog(sdt->q, STRLOGST, SL_TRACE, "%s <- %s", sdt_statename(newstate),
			  sdt_statename(oldstate));
	}
	return (newstate);
}

static int
sdt_save_m_state(struct sdt *sdt)
{
	return ((sdt->oldstate.state = sdt_get_m_state(sdt)));
}

static int
sdt_restore_m_state(struct sdt *sdt)
{
	return sdt_set_m_state(sdt, sdt->oldstate.state);
}

static int
sdt_checkpoint_m_state(struct sdt *sdt, int newstate)
{
	sdt->chkstate.state = sdt_get_m_state(sdt);
	return sdt_set_m_state(sdt, newstate);
}

static int
sdt_fallback_m_state(struct sdt *sdt)
{
	return sdt_set_m_state(sdt, sdt->chkstate.state);
}

static inline const char *
cd_primname(cd_ulong prim)
{
	switch (prim) {
	case CD_INFO_REQ:
		return ("CD_INFO_REQ");
	case CD_INFO_ACK:
		return ("CD_INFO_ACK");
	case CD_ATTACH_REQ:
		return ("CD_ATTACH_REQ");
	case CD_DETACH_REQ:
		return ("CD_DETACH_REQ");
	case CD_ENABLE_REQ:
		return ("CD_ENABLE_REQ");
	case CD_DISABLE_REQ:
		return ("CD_DISABLE_REQ");
	case CD_OK_ACK:
		return ("CD_OK_ACK");
	case CD_ERROR_ACK:
		return ("CD_ERROR_ACK");
	case CD_ENABLE_CON:
		return ("CD_ENABLE_CON");
	case CD_DISABLE_CON:
		return ("CD_DISABLE_CON");
	case CD_ERROR_IND:
		return ("CD_ERROR_IND");
	case CD_ALLOW_INPUT_REQ:
		return ("CD_ALLOW_INPUT_REQ");
	case CD_READ_REQ:
		return ("CD_READ_REQ");
	case CD_UNITDATA_REQ:
		return ("CD_UNITDATA_REQ");
	case CD_WRITE_READ_REQ:
		return ("CD_WRITE_READ_REQ");
	case CD_UNITDATA_ACK:
		return ("CD_UNITDATA_ACK");
	case CD_UNITDATA_IND:
		return ("CD_UNITDATA_IND");
	case CD_HALT_INPUT_REQ:
		return ("CD_HALT_INPUT_REQ");
	case CD_ABORT_OUTPUT_REQ:
		return ("CD_ABORT_OUTPUT_REQ");
	case CD_MUX_NAME_REQ:
		return ("CD_MUX_NAME_REQ");
	case CD_BAD_FRAME_IND:
		return ("CD_BAD_FRAME_IND");
	case CD_MODEM_SIG_REQ:
		return ("CD_MODEM_SIG_REQ");
	case CD_MODEM_SIG_IND:
		return ("CD_MODEM_SIG_IND");
	case CD_MODEM_SIG_POLL:
		return ("CD_MODEM_SIG_POLL");
	default:
		return ("(Unknown Primitive)");
	}
}

static inline const char *
cd_statename(int state)
{
	switch (state) {
	case CD_UNATTACHED:
		return ("CD_UNATTACHED");
	case CD_UNUSABLE:
		return ("CD_UNUSABLE");
	case CD_DISABLED:
		return ("CD_DISABLED");
	case CD_ENABLE_PENDING:
		return ("CD_ENABLE_PENDING");
	case CD_ENABLED:
		return ("CD_ENABLED");
	case CD_READ_ACTIVE:
		return ("CD_READ_ACTIVE");
	case CD_INPUT_ALLOWED:
		return ("CD_INPUT_ALLOWED");
	case CD_DISABLE_PENDING:
		return ("CD_DISABLE_PENDING");
	case CD_OUTPUT_ACTIVE:
		return ("CD_OUTPUT_ACTIVE");
	case CD_XRAY:
		return ("CD_XRAY");
	case CD_NOT_AUTH:
		return ("CD_NOT_AUTH");
	default:
		return ("(Unknown State)");
	}
}

/*
 *  =========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  SDT Provider to SDT User primitives.
 *
 *  -------------------------------------------------------------------------
 */

/**
 * lmi_info_ack: - issue LMI_INFO_ACK primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_info_ack(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	lmi_info_ack_t *p;
	mblk_t *mp;
	size_t msg_len = sizeof(*p) + sdt->proto.lmi_ppa_length;

	if (likely(canputnext(sdt->oq))) {
		if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_INFO_ACK;
			p->lmi_version = sdt->proto.lmi_version;
			p->lmi_state = sdt->proto.lmi_state;
			p->lmi_max_sdu = sdt->proto.lmi_max_sdu;
			p->lmi_min_sdu = sdt->proto.lmi_min_sdu;
			p->lmi_header_len = sdt->proto.lmi_header_len;
			p->lmi_ppa_style = sdt->proto.lmi_ppa_style;
			p->lmi_ppa_length = sdt->proto.lmi_ppa_length;
			p->lmi_ppa_offset = sizeof(*p);
			p->lmi_prov_flags = sdt->proto.lmi_prov_flags;
			p->lmi_prov_state = sdt->proto.lmi_prov_state;
			mp->b_wptr += sizeof(*p);
			bcopy(&sdt->ppa, mp->b_wptr, sdt->proto.lmi_ppa_length);
			mp->b_wptr += sdt->proto.lmi_ppa_length;
			freemsg(msg);
			mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "<- LMI_INFO_ACK");
			putnext(sdt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * lmi_ok_ack: - issue LMI_INFO_ACK primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_ok_ack(struct sdt *sdt, queue_t *q, mblk_t *msg, lmi_ulong prim)
{
	lmi_ok_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		switch (sdt_get_m_state(sdt)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = sdt_set_m_state(sdt, LMI_DISABLED);
			break;
		case LMI_DETACH_PENDING:
			p->lmi_state = sdt_set_m_state(sdt, LMI_UNATTACHED);
			break;
		default:
			p->lmi_state = sdt_get_m_state(sdt);
			break;
		}
		mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "<- LMI_OK_ACK");
		putnext(sdt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_error_ack: - issue LMI_ERROR_ACK primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_error_ack(struct sdt *sdt, queue_t *q, mblk_t *msg, lmi_ulong prim, lmi_long error)
{
	lmi_error_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_error_primitive = prim;
		p->lmi_reason = error < 0 ? LMI_SYSERR : error;
		p->lmi_errno = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		switch (sdt_get_m_state(sdt)) {
		case LMI_ATTACH_PENDING:
			p->lmi_state = sdt_set_m_state(sdt, LMI_UNATTACHED);
			break;
		case LMI_DETACH_PENDING:
			p->lmi_state = sdt_set_m_state(sdt, LMI_DISABLED);
			break;
		case LMI_ENABLE_PENDING:
			p->lmi_state = sdt_set_m_state(sdt, LMI_DISABLED);
			break;
		case LMI_DISABLE_PENDING:
			p->lmi_state = sdt_set_m_state(sdt, LMI_ENABLED);
			break;
		default:
			p->lmi_state = sdt_revert_m_state(sdt);
			break;
		}
		mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "<- LMI_ERROR_ACK");
		putnext(sdt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_enable_con: - issue LMI_ENABLE_CON primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_enable_con(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	lmi_enable_con_t *p;
	mblk_t *mp;

	if (likely(canputnext(sdt->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_ENABLE_CON;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			p->lmi_state = sdt_set_m_state(sdt, LMI_ENABLED);
			mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "<- LMI_ENABLE_CON");
			putnext(sdt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * lmi_disable_con: - issue LMI_DISABLE_CON primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_disable_con(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	lmi_disable_con_t *p;
	mblk_t *mp;

	if (likely(canputnext(sdt->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_DISABLE_CON;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			p->lmi_state = sdt_set_m_state(sdt, LMI_DISABLED);
			mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "<- LMI_DISABLE_CON");
			putnext(sdt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * lmi_optmgmt_ack: - issue LMI_OPTMGMT_ACK primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_optmgmt_ack(struct sdt *sdt, queue_t *q, mblk_t *msg, caddr_t opt_ptr, size_t opt_len,
		lmi_ulong flags)
{
	lmi_optmgmt_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_mgmt_flags = flags;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		freemsg(msg);
		mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "<- LMI_OPTMGMT_ACK");
		putnext(sdt->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_error_ind: - issue LMI_ERROR_IND primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_error_ind(struct sdt *sdt, queue_t *q, mblk_t *msg, lmi_long error, lmi_ulong state)
{
	lmi_error_ind_t *p;
	mblk_t *mp;

	if (likely(canputnext(sdt->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_ERROR_IND;
			p->lmi_reason = error < 0 ? LMI_SYSERR : error;
			p->lmi_errno = error < 0 ? -error : 0;
			p->lmi_state = state;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			sdt_set_m_state(sdt, state);
			mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "<- LMI_ERROR_IND");
			putnext(sdt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * lmi_stats_ind: - issue LMI_STATS_IND primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_stats_ind(struct sdt *sdt, queue_t *q, mblk_t *msg, caddr_t sta_ptr, size_t sta_len,
	      lmi_ulong interval)
{
	lmi_stats_ind_t *p;
	mblk_t *mp;

	if (likely(bcanputnext(sdt->oq, 1))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p) + sta_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_STATS_IND;
			p->lmi_interval = interval;
			p->lmi_timestamp = drv_hztomsec(jiffies);	/* FIXME: wall clock
									   instead */
			mp->b_wptr += sizeof(*p);
			bcopy(sta_ptr, mp->b_wptr, sta_len);
			mp->b_wptr += sta_len;
			freemsg(msg);
			mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "<- LMI_STATS_IND");
			putnext(sdt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * lmi_event_ind: - issue LMI_EVENT_IND primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
lmi_event_ind(struct sdt *sdt, queue_t *q, mblk_t *msg, lmi_ulong oid, lmi_ulong severity,
	      caddr_t inf_ptr, size_t inf_len)
{
	lmi_event_ind_t *p;
	mblk_t *mp;

	if (likely(canputnext(sdt->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p) + inf_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_EVENT_IND;
			p->lmi_objectid = oid;
			p->lmi_timestamp = drv_hztomsec(jiffies);	/* FIXME: wall clock
									   instead */
			p->lmi_severity = severity;
			mp->b_wptr += sizeof(*p);
			bcopy(inf_ptr, mp->b_wptr, inf_len);
			mp->b_wptr += inf_len;
			freemsg(msg);
			mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "<- LMI_EVENT_IND");
			putnext(sdt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sdt_rc_signal_unit_ind: - issue SDT_RC_SIGNAL_UNIT_IND primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sdt_rc_signal_unit_ind(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	sdt_rc_signal_unit_ind_t *p;
	mblk_t *mp;

	if (likely(canputnext(sdt->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sdt_primitive = SDT_RC_SIGNAL_UNIT_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "<- SDT_RC_SIGNAL_UNIT_IND");
			putnext(sdt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sdt_rc_congestion_accept_ind: - issue SDT_RC_CONGESTION_ACCEPT_IND primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sdt_rc_congestion_accept_ind(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	sdt_rc_congestion_accept_ind_t *p;
	mblk_t *mp;

	if (likely(canputnext(sdt->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sdt_primitive = SDT_RC_CONGESTION_ACCEPT_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "<- SDT_RC_CONGESTION_ACCEPT_IND");
			putnext(sdt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sdt_rc_congestion_discard_ind: - issue SDT_RC_CONGESTION_DISCARD_IND primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sdt_rc_congestion_discard_ind(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	sdt_rc_congestion_discard_ind_t *p;
	mblk_t *mp;

	if (likely(canputnext(sdt->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sdt_primitive = SDT_RC_CONGESTION_DISCARD_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "<- SDT_RC_CONGESTION_DISCARD_IND");
			putnext(sdt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sdt_rc_no_congestion_ind: - issue SDT_RC_NO_CONGESTION_IND primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sdt_rc_no_congestion_ind(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	sdt_rc_no_congestion_ind_t *p;
	mblk_t *mp;

	if (likely(canputnext(sdt->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sdt_primitive = SDT_RC_NO_CONGESTION_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "<- SDT_RC_NO_CONGESTION_IND");
			putnext(sdt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sdt_iac_correct_su_ind: - issue SDT_IAC_CORRECT_SU_IND primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sdt_iac_correct_su_ind(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	sdt_iac_correct_su_ind_t *p;
	mblk_t *mp;

	if (likely(canputnext(sdt->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sdt_primitive = SDT_IAC_CORRECT_SU_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "<- SDT_IAC_CORRECT_SU_IND");
			putnext(sdt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sdt_iac_abort_proving_ind: - issue SDT_IAC_ABORT_PROVING_IND primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sdt_iac_abort_proving_ind(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	sdt_iac_abort_proving_ind_t *p;
	mblk_t *mp;

	if (likely(canputnext(sdt->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sdt_primitive = SDT_IAC_ABORT_PROVING_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "<- SDT_IAC_ABORT_PROVING_IND");
			putnext(sdt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sdt_lsc_link_failure_ind: - issue SDT_LSC_LINK_FAILURE_IND primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sdt_lsc_link_failure_ind(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	sdt_lsc_link_failure_ind_t *p;
	mblk_t *mp;

	if (likely(canputnext(sdt->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sdt_primitive = SDT_LSC_LINK_FAILURE_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sdt->oq, STRLOGTX, SL_TRACE, "<- SDT_LSC_LINK_FAILURE_IND");
			putnext(sdt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sdt_txc_transmission_request_ind: - issue SDT_TXC_TRANSMISSION_REQUEST_IND primitive
 * @sdt: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
sdt_txc_transmission_request_ind(struct sdt *sdt, queue_t *q, mblk_t *msg)
{
	sdt_txc_transmission_request_ind_t *p;
	mblk_t *mp;

	if (likely(canputnext(sdt->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sdt_primitive = SDT_TXC_TRANSMISSION_REQUEST_IND;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(sdt->oq, STRLOGTX, SL_TRACE,
				  "<- SDT_TXC_TRANSMISSION_REQUEST_IND");
			putnext(sdt->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  CD User to CD Provider primitives.
 *
 *  -------------------------------------------------------------------------
 */
/**
 * cd_info_req: - issue CD_INFO_REQ primitive
 * @cd: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
cd_info_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_info_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_INFO_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_attach_req: - issue CD_ATTACH_REQ primitive
 * @cd: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
cd_attach_req(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong ppa)
{
	cd_attach_req_t *p;
	mblk_t *mp;

	if (likely(canputnext(cd->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_ATTACH_REQ;
			p->cd_ppa = ppa;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			cd_set_m_state(cd, CD_ATTACH_PENDING);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_ATTACH_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * cd_detach_req: - issue CD_DETACH_REQ primitive
 * @cd: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
cd_detach_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_detach_req_t *p;
	mblk_t *mp;

	if (likely(canputnext(cd->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_DETACH_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			cd_set_m_state(cd, CD_DETACH_PENDING);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_DETACH_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * cd_enable_req: - issue CD_ENABLE_REQ primitive
 * @cd: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
cd_enable_req(struct cd *cd, queue_t *q, mblk_t *msg, caddr_t dial_ptr, size_t dial_len)
{
	cd_enable_req_t *p;
	mblk_t *mp;

	if (likely(canputnext(cd->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p) + dial_len, BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_ENABLE_REQ;
			p->cd_dial_type = dial_len ? 0 : CD_NODIAL;
			p->cd_dial_length = dial_len;
			p->cd_dial_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			bcopy(dial_ptr, mp->b_wptr, dial_len);
			mp->b_wptr += dial_len;
			freemsg(msg);
			cd_set_m_state(cd, CD_ENABLE_PENDING);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_ENABLE_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * cd_disable_req: - issue CD_DISABLE_REQ primitive
 * @cd: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
cd_disable_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_disable_req_t *p;
	mblk_t *mp;

	if (likely(canputnext(cd->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_DISABLE_REQ;
			p->cd_disposal = CD_DELIVER;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			cd_set_m_state(cd, CD_DISABLE_PENDING);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_DISABLE_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * cd_allow_input_req: - issue CD_INPUT_REQ primitive
 * @cd: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
cd_allow_input_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_allow_input_req_t *p;
	mblk_t *mp;

	if (likely(canputnext(cd->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_INPUT_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_INPUT_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * cd_read_req: - issue CD_READ_REQ primitive
 * @cd: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
cd_read_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_read_req_t *p;
	mblk_t *mp;

	if (likely(canputnext(cd->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_READ_REQ;
			p->cd_msec = 1;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_READ_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * cd_unitdata_req: - issue CD_UNITDATA_REQ primitive
 * @cd: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
cd_unitdata_req(struct cd *cd, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	cd_unitdata_req_t *p;
	mblk_t *mp;

	if (likely(canputnext(cd->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_UNITDATA_REQ;
			p->cd_addr_type = CD_IMPLICIT;
			p->cd_priority = 0;
			p->cd_dest_addr_length = 0;
			p->cd_dest_addr_offset = sizeof(*p);
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_UNITDATA_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * cd_write_read_req: - issue CD_WRITE_READ_REQ primitive
 * @cd: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
cd_write_read_req(struct cd *cd, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	cd_write_read_req_t *p;
	mblk_t *mp;

	if (likely(canputnext(cd->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_WRITE_READ_REQ;
			p->cd_unitdata_req.cd_primitive = CD_UNITDATA_REQ;
			p->cd_unitdata_req.cd_addr_type = CD_IMPLICIT;
			p->cd_unitdata_req.cd_priority = 0;
			p->cd_unitdata_req.cd_dest_addr_length = 0;
			p->cd_unitdata_req.cd_dest_addr_offset = sizeof(*p);
			p->cd_read_req.cd_primitive = CD_READ_REQ;
			p->cd_read_req.cd_msec = 1;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_WRITE_READ_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * cd_halt_input_req: - issue CD_HALT_INPUT_REQ primitive
 * @cd: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
cd_halt_input_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_halt_input_req_t *p;
	mblk_t *mp;

	if (likely(canputnext(cd->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_HALT_INPUT_REQ;
			p->cd_disposal = CD_DELIVER;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			cd_set_m_state(cd, CD_ENABLED);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_HALT_INPUT_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * cd_abort_output_req: - issue CD_ABORT_OUTPUT_REQ primitive
 * @cd: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
cd_abort_output_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_abort_output_req_t *p;
	mblk_t *mp;

	if (likely(canputnext(cd->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_ABORT_OUTPUT_REQ;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			if (cd_get_m_state(cd) == CD_OUTPUT_ACTIVE)
				cd_set_m_state(cd, CD_INPUT_ALLOWED);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_ABORT_OUTPUT_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * cd_mux_name_req: - issue CD_MUX_NAME_REQ primitive
 * @cd: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
cd_mux_name_req(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_mux_name_req_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_MUX_NAME_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_MUX_NAME_REQ ->");
		putnext(cd->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_modem_sig_req: - issue CD_MODEM_SIG_REQ primitive
 * @cd: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
cd_modem_sig_req(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong sigs)
{
	cd_modem_sig_req_t *p;
	mblk_t *mp;

	if (likely(canputnext(cd->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PCPROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_MODEM_SIG_REQ;
			p->cd_sigs = sigs;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_MODEM_SIG_REQ ->");
			putnext(cd->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * cd_modem_sig_poll: - issue CD_MODEM_SIG_POLL primitive
 * @cd: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static fastcall int
cd_modem_sig_poll(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_modem_sig_poll_t *p;
	mblk_t *mp;

	if (likely(canputnext(cd->oq))) {
		if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->cd_primitive = CD_MODEM_SIG_POLL;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(cd->oq, STRLOGTX, SL_TRACE, "CD_MODEM_SIG_POLL ->");
			putnext(cd->oq, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  =========================================================================
 *
 *  PROTOCOL STATE MACHINE
 *
 *  =========================================================================
 */

/*
 *  =========================================================================
 *
 *  RECEIVED PRIMITIVES
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  SDT User to SDT Provider primitives.
 *
 *  -------------------------------------------------------------------------
 */

/**
 * lmi_info_req: - process LMI_INFO_REQ primitive
 * @sdt: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_INFO_REQ primitive
 */
static fastcall int
lmi_info_req(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	lmi_info_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	return cd_info_req(sdt->cd, q, mp);
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(sdt, q, mp, LMI_INFO_REQ, err);
}

/**
 * lmi_attach_req: - process LMI_ATTACH_REQ primitive
 * @sdt: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_ATTACH_REQ primitive
 */
static fastcall int
lmi_attach_req(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	cd_ulong ppa;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	if (sdt_get_m_state(sdt) != LMI_UNATTACHED)
		goto outstate;
	if (sdt->proto.lmi_ppa_style != LMI_STYLE2)
		goto notsupp;
	if (unlikely(!MBLKIN(mp, p->lmi_ppa_offset, p->lmi_ppa_length)))
		goto badppa;
	if (p->lmi_ppa_length != sizeof(ppa))
		goto badppa;
	bcopy(mp->b_rptr + p->lmi_ppa_offset, &ppa, sizeof(ppa));
	sdt_set_m_state(sdt, LMI_ATTACH_PENDING);
	return cd_attach_req(sdt->cd, q, mp, ppa);
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      badaddr:
	err = LMI_BADADDRESS;
	goto error;
      badppa:
	err = LMI_BADPPA;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(sdt, q, mp, LMI_ATTACH_REQ, err);
}

/**
 * lmi_detach_req: - process LMI_DETACH_REQ primitive
 * @sdt: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_DETACH_REQ primitive
 */
static fastcall int
lmi_detach_req(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	if (sdt_get_m_state(sdt) != LMI_DISABLED)
		goto outstate;
	if (sdt->proto.lmi_ppa_style != LMI_STYLE2)
		goto notsupp;
	sdt_set_m_state(sdt, LMI_DETACH_PENDING);
	return cd_detach_req(sdt->cd, q, mp);
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(sdt, q, mp, LMI_DETACH_REQ, err);
}

/**
 * lmi_enable_req: - process LMI_ENABLE_REQ primitive
 * @sdt: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_ENABLE_REQ primitive
 */
static fastcall int
lmi_enable_req(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	if (sdt_get_m_state(sdt) != LMI_DISABLED)
		goto outstate;
	if (unlikely(!MBLKIN(mp, p->lmi_rem_offset, p->lmi_rem_length)))
		goto badaddr;
	sdt_set_m_state(sdt, LMI_ENABLE_PENDING);
	return cd_enable_req(sdt->cd, q, mp, (caddr_t) (mp->b_rptr + p->lmi_rem_offset),
			     p->lmi_rem_length);
      badaddr:
	err = LMI_BADADDRESS;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(sdt, q, mp, LMI_ENABLE_REQ, err);
}

/**
 * lmi_disable_req: - process LMI_DISABLE_REQ primitive
 * @sdt: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_DISABLE_REQ primitive
 */
static fastcall int
lmi_disable_req(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	if (unlikely(sdt_get_m_state(sdt) != LMI_ENABLED))
		goto outstate;
	sdt_set_m_state(sdt, LMI_DISABLE_PENDING);
	return cd_disable_req(sdt->cd, q, mp);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(sdt, q, mp, LMI_DISABLE_REQ, err);
}

/**
 * lmi_optmgmt_req: - process LMI_OPTMGMT_REQ primitive
 * @sdt: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_OPTMGMT_REQ primitive
 */
static fastcall int
lmi_optmgmt_req(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	goto notsupp;
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(sdt, q, mp, LMI_OPTMGMT_REQ, err);
}

/**
 * sdt_daedt_transmission_req: - process SDT_XXX primitive
 * @sdt: private structure
 * @q: active queue (write queue)
 * @mp: the SDT_XXX primitive
 */
static fastcall int
sdt_daedt_transmission_req(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	sdt_daedt_transmission_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	if (unlikely(sdt_get_m_state(sdt) != LMI_ENABLED))
		goto outstate;
	if (likely(sdt_get_p_state(sdt) != SDT_XXX))
		goto discard;
	if (unlikely(!(sdt_get_p_flags(sdt) * SDT_XXX)))
		goto discard;
	if ((mp->b_cont == NULL) ||
	    (msgdsize(mp->b_cont) < sdt->proto.lmi_min_sdu) ||
	    (msgdsize(mp->b_cont) > sdt->proto.lmi_max_sdu))
		goto baddata;
	return cd_data_req(sdl->cd, q, mp, mp->b_cont);
      discard:
	freemsg(mp);
	return (0);
      baddata:
	err = EPROTO;
	goto error;
      outstate:
	err = EPROTO;
	goto error;
      protoshort:
	err = EPROTO;
	goto error;
      error:
	return m_error(sdt, q, mp, err);
}

/**
 * sdt_daedt_start_req: - process SDT_XXX primitive
 * @sdt: private structure
 * @q: active queue (write queue)
 * @mp: the SDT_XXX primitive
 */
static fastcall int
sdt_daedt_start_req(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	sdt_daedt_start_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	goto notsupp;
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      badaddr:
	err = LMI_BADADDRESS;
	goto error;
      badppa:
	err = LMI_BADPPA;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(sdt, q, mp, SDT_XXX, err);
}

/**
 * sdt_daedr_start_req: - process SDT_XXX primitive
 * @sdt: private structure
 * @q: active queue (write queue)
 * @mp: the SDT_XXX primitive
 */
static fastcall int
sdt_daedr_start_req(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	sdt_daedr_start_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	goto notsupp;
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      badaddr:
	err = LMI_BADADDRESS;
	goto error;
      badppa:
	err = LMI_BADPPA;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(sdt, q, mp, SDT_XXX, err);
}

/**
 * sdt_aerm_start_req: - process SDT_XXX primitive
 * @sdt: private structure
 * @q: active queue (write queue)
 * @mp: the SDT_XXX primitive
 */
static fastcall int
sdt_aerm_start_req(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	sdt_aerm_start_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	goto notsupp;
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      badaddr:
	err = LMI_BADADDRESS;
	goto error;
      badppa:
	err = LMI_BADPPA;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(sdt, q, mp, SDT_XXX, err);
}

/**
 * sdt_aerm_stop_req: - process SDT_XXX primitive
 * @sdt: private structure
 * @q: active queue (write queue)
 * @mp: the SDT_XXX primitive
 */
static fastcall int
sdt_aerm_stop_req(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	sdt_aerm_stop_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	goto notsupp;
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      badaddr:
	err = LMI_BADADDRESS;
	goto error;
      badppa:
	err = LMI_BADPPA;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(sdt, q, mp, SDT_XXX, err);
}

/**
 * sdt_aerm_set_ti_to_tin_req: - process SDT_XXX primitive
 * @sdt: private structure
 * @q: active queue (write queue)
 * @mp: the SDT_XXX primitive
 */
static fastcall int
sdt_aerm_set_ti_to_tin_req(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	sdt_aerm_set_ti_to_tin_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	goto notsupp;
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      badaddr:
	err = LMI_BADADDRESS;
	goto error;
      badppa:
	err = LMI_BADPPA;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(sdt, q, mp, SDT_XXX, err);
}

/**
 * sdt_aerm_set_ti_to_tie_req: - process SDT_XXX primitive
 * @sdt: private structure
 * @q: active queue (write queue)
 * @mp: the SDT_XXX primitive
 */
static fastcall int
sdt_aerm_set_ti_to_tie_req(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	sdt_aerm_set_ti_to_tie_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	goto notsupp;
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      badaddr:
	err = LMI_BADADDRESS;
	goto error;
      badppa:
	err = LMI_BADPPA;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(sdt, q, mp, SDT_XXX, err);
}

/**
 * sdt_suerm_start_req: - process SDT_XXX primitive
 * @sdt: private structure
 * @q: active queue (write queue)
 * @mp: the SDT_XXX primitive
 */
static fastcall int
sdt_suerm_start_req(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	sdt_suerm_start_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	if (unlikely(sdt_get_m_state(sdt) != LMI_ENABLED))
		goto outstate;
	goto notsupp;
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      badaddr:
	err = LMI_BADADDRESS;
	goto error;
      badppa:
	err = LMI_BADPPA;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(sdt, q, mp, SDT_XXX, err);
}

/**
 * sdt_suerm_stop_req: - process SDT_XXX primitive
 * @sdt: private structure
 * @q: active queue (write queue)
 * @mp: the SDT_XXX primitive
 */
static fastcall int
sdt_suerm_stop_req(struct sdt *sdt, queue_t *q, mblk_t *mp)
{
	sdt_suerm_stop_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	goto notsupp;
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      badaddr:
	err = LMI_BADADDRESS;
	goto error;
      badppa:
	err = LMI_BADPPA;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      protoshort:
	err = LMI_PROTOSHORT;
	goto error;
      error:
	return lmi_error_reply(sdt, q, mp, SDT_XXX, err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  CD Provider to CD User primitives.
 *
 *  -------------------------------------------------------------------------
 */

/**
 * cd_info_ack: - process CD_INFO_ACK primitive
 * @cd: private structure
 * @q: active queue (read queue)
 * @mp: the CD_INFO_ACK primitive
 */
static fastcall int
cd_info_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_info_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	goto notsupp;
      notsupp:
	err = CD_NOTSUPP;
	goto error;
      badaddr:
	err = CD_BADADDRESS;
	goto error;
      badppa:
	err = CD_BADPPA;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      protoshort:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_reply(cd, q, mp, CD_OK_ACK, err);
}

/**
 * cd_ok_ack: - process CD_OK_ACK primitive
 * @cd: private structure
 * @q: active queue (read queue)
 * @mp: the CD_OK_ACK primitive
 */
static fastcall int
cd_ok_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	switch (p->cd_correct_primitive) {
	case CD_ALLOW_INPUT_REQ:
		cd_set_m_state(cd, CD_INPUT_ALLOWED);
		break;
	case CD_ATTACH_REQ:
		cd_set_m_state(cd, CD_DISABLED);
		return lmi_ok_ack(cd->sdt, q, mp, LMI_ATTACH_REQ);
	case CD_DETACH_REQ:
		cd_set_m_state(cd, CD_UNATTACHED);
		return lmi_ok_ack(cd->sdt, q, mp, LMI_DETACH_REQ);
	case CD_HALT_INPUT_REQ:
		cd_set_m_state(cd, CD_ENABLED);
		break;
	case CD_ABORT_OUTPUT_REQ:
		cd_set_m_state(cd, p->cd_state);
		break;
	case CD_MODEM_SIG_REQ:
		cd_set_m_state(cd, p->cd_state);
		break;
	case CD_MUX_NAME_REQ:
		cd_set_m_state(cd, p->cd_state);
		break;
	}
	freemsg(mp);
	return (0);
      protoshort:
	err = EFAULT;
	goto error;
      error:
	return m_error(cd->sdt, q, mp, err);
}

/**
 * cd_error_ack: - process CD_ERROR_ACK primitive
 * @cd: private structure
 * @q: active queue (read queue)
 * @mp: the CD_ERROR_ACK primitive
 */
static fastcall int
cd_error_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_error_ack_t *p = (typeof(p)) mp->b_rptr;
	lmi_ulong prim;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	switch (p->cd_error_primitive) {
	case CD_ABORT_OUTPUT_REQ:
		prim = SDT_SUERM_STOP_REQ;
		break;
	case CD_ALLOW_INPUT_REQ:
		prim = SDT_SUERM_START_REQ;
		break;
	case CD_ATTACH_REQ:
		prim = LMI_ATTACH_REQ;
		break;
	case CD_DETACH_REQ:
		prim = LMI_DETACH_REQ;
		break;
	case CD_ENABLE_REQ:
		prim = LMI_ENABLE_REQ;
		break;
	case CD_HALT_INPUT_REQ:
		prim = SDT_SUERM_STOP_REQ;
		break;
	case CD_INFO_REQ:
		prim = LMI_INFO_REQ;
		break;
	case CD_MODEM_SIG_REQ:
	case CD_MUX_NAME_REQ:
	case CD_READ_REQ:
	case CD_UNITDATA_REQ:
	case CD_WRITE_READ_REQ:
		cd_set_m_state(cd, p->cd_state);
		goto discard;
	default:
		goto badprim;
	}
	cd_set_m_state(cd, p->cd_state);
	return lmi_error_ack(cd->sdt, q, mp, prim,
			     (p->cd_errno == CD_SYSERR) ? -p->cd_explanation : p->cd_errno);
      discard:
	freemsg(mp);
	return (0);
      badprim:
	err = EFAULT;
	goto error;
      protoshort:
	err = EFAULT;
	goto error;
      error:
	return m_error(cd->sdt, q, mp, err);
}

/**
 * cd_enable_con: - process CD_ENABLE_CON primitive
 * @cd: private structure
 * @q: active queue (read queue)
 * @mp: the CD_ENABLE_CON primitive
 */
static fastcall int
cd_enable_con(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_enable_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	cd_set_m_state(cd, CD_ENABLED);
	return lmi_enable_con(cd->sdt, q, mp);
      protoshort:
	err = EFAULT;
	goto error;
      error:
	return m_error(cd->sdt, q, mp, err);
}

/**
 * cd_disable_con: - process CD_DISABLE_CON primitive
 * @cd: private structure
 * @q: active queue (read queue)
 * @mp: the CD_DISABLE_CON primitive
 */
static fastcall int
cd_disable_con(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_disable_con_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	cd_set_state(cd, CD_DISABLED);
	return lmi_disable_con(cd->sdt, q, mp);
      protoshort:
	err = EFAULT;
	goto error;
      error:
	return m_error(cd->sdt, q, mp, err);
}

/**
 * cd_unitdata_ack: - process CD_UNITDATA_ACK primitive
 * @cd: private structure
 * @q: active queue (read queue)
 * @mp: the CD_UNITDATA_ACK primitive
 */
static fastcall int
cd_unitdata_ack(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_unitdata_ack_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	goto notsupp;
      notsupp:
	err = EFAULT;
	goto error;
      protoshort:
	err = EFAULT;
	goto error;
      error:
	return m_error(cd->sdt, q, mp, err);
}

/**
 * cd_error_ind: - process CD_ERROR_IND primitive
 * @cd: private structure
 * @q: active queue (read queue)
 * @mp: the CD_ERROR_IND primitive
 */
static fastcall int
cd_error_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_error_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	cd_set_m_state(cd, p->cd_state);
	if (p->cd_errno == CD_DISC)
	if (p->cd_errno == CD_EVENT) {
		switch(p->cd_explanation) {
		case CD_HDLC_IDLE:
			return sdt_txc_transmission_request_ind(cd->sdt, q, mp);
		case CD_DEVERR: /* congestion accept */
			return sdt_rc_congestion_accept_ind(cd->sdt, q, mp);
		case CD_DEVERR+1: /* congestion discard */
			return sdt_rc_congestion_discard_ind(cd->sdt, q, mp);
		case CD_DEVERR+2: /* no congestion */
			return sdt_rc_no_congestion_ind(cd->sdt, q, mp);
		case CD_DEVERR+3: /* correct su */
			return sdt_iac_correct_su_ind(cd->sdt, q, mp);
		case CD_DEVERR+4: /* abort proving */
			return sdt_iac_abort_proving_ind(cd->sdt, q, mp);
		}
	}
	freemsg(mp);
	return (0);
      protoshort:
	err = EFAULT;
	goto error;
      error:
	return m_error(cd->sdt, q, mp, err);
}

/**
 * cd_unitdata_ind: - process CD_UNITDATA_IND primitive
 * @cd: private structure
 * @q: active queue (read queue)
 * @mp: the CD_UNITDATA_IND primitive
 */
static fastcall int
cd_unitdata_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	goto notsupp;
      notsupp:
	err = EFAULT;
	goto error;
      protoshort:
	err = EFAULT;
	goto error;
      error:
	return m_error(cd->sdt, q, mp, err);
}

/**
 * cd_bad_frame_ind: - process CD_BAD_FRAME_IND primitive
 * @cd: private structure
 * @q: active queue (read queue)
 * @mp: the CD_BAD_FRAME_IND primitive
 */
static fastcall int
cd_bad_frame_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_bad_frame_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	cd_set_m_state(cd, p->cd_state);
	freemsg(mp);
	return (0);
      protoshort:
	err = EFAULT;
	goto error;
      error:
	return m_error(cd->sdt, q, mp, err);
}

/**
 * cd_modem_sig_ind: - process CD_MODEM_SIG_IND primitive
 * @cd: private structure
 * @q: active queue (read queue)
 * @mp: the CD_MODEM_SIG_IND primitive
 */
static fastcall int
cd_modem_sig_ind(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_modem_sig_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (unlikely(!MBLKIN(mp, 0, sizeof(*p))))
		goto protoshort;
	goto notsupp;
      notsupp:
	err = EFAULT;
	goto error;
      protoshort:
	err = EFAULT;
	goto error;
      error:
	return m_error(cd->sdt, q, mp, err);
}

/*
 *  =========================================================================
 *
 *  STREAMS INPUT-OUTPUT CONTROLS
 *
 *  =========================================================================
 */

static int
lmi_testoptions(struct sdt *sdt, struct lmi_option *arg)
{
	return (0);
}
static int
sdl_testconfig(struct sdt *sdt, struct sdl_config *arg)
{
	return (0);
}
static int
sdl_setnotify(struct sdt *sdt, struct sdl_notify *arg)
{
	return (0);
}
static int
sdl_clrnotify(struct sdt *sdt, struct sdl_notify *arg)
{
	return (0);
}
static int
sdt_testconfig(struct sdt *sdt, struct sdt_config *arg)
{
	return (0);
}
static int
sdt_setnotify(struct sdt *sdt, struct sdt_notify *arg)
{
	return (0);
}
static int
sdt_clrnotify(struct sdt *sdt, struct sdt_notify *arg)
{
	return (0);
}
static noinline fastcall int
lmi_ioctl(struct sdt *sdt, queue_t *q, mblk_t *mp, struct iocblk *ioc)
{
	int size = -1, err = 0;
	mblk_t *bp;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", sdt_iocname(ioc->ioc_cmd));
	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(LMI_IOCGOPTIONS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdt->option), false)))
			goto enobufs;
		bcopy(&sdt->option, bp->b_rptr, sizeof(sdt->option));
		break;
	case _IOC_NR(LMI_IOCSOPTIONS):
		size = sizeof(sdt->option);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0)
		return (err);
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
static noinline fastcall int
lmi_copyin(struct sdt *sdt, queue_t *q, mblk_t *mp, struct copyresp *cp, mblk_t *dp)
{
	int err = 0;
	mblk_t *bp;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, MBLKL(dp), false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, MBLKL(dp));
	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sdt_iocname(cp->cp_cmd));
	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(LMI_IOCSOPTIONS):
		if ((err = lmi_testoptions(sdt, (struct lmi_option *) mp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdt->option, sizeof(sdt->option));
		break;
	default:
		err = EPROTO;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
}
static noinline fastcall int
lmi_copyout(struct sdt *sdt, queue_t *q, mblk_t *mp, struct copyresp *cp)
{
	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sdt_iocname(cp->cp_cmd));
	mi_copyout(q, mp);
	return (0);
}

static noinline fastcall int
sdl_ioctl(struct sdt *sdt, queue_t *q, mblk_t *mp, struct iocblk *ioc)
{
	int size = -1, err = 0;
	mblk_t *bp;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", sdt_iocname(ioc->ioc_cmd));
	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SDL_IOCGOPTION):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdt->option), false)))
			goto enobufs;
		bcopy(&sdt->option, bp->b_rptr, sizeof(sdt->option));
		break;
	case _IOC_NR(SDL_IOCSOPTION):
		size = sizeof(sdt->option);
		break;
	case _IOC_NR(SDL_IOCGCONFIG):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdt->sdl.config), false)))
			goto enobufs;
		bcopy(&sdt->sdl.config, bp->b_rptr, sizeof(sdt->sdl.config));
		break;
	case _IOC_NR(SDL_IOCSCONFIG):
	case _IOC_NR(SDL_IOCTCONFIG):
	case _IOC_NR(SDL_IOCCCONFIG):
		size = sizeof(sdt->sdl.config);
		break;
	case _IOC_NR(SDL_IOCGSTATEM):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdt->sdl.statem), false)))
			goto enobufs;
		bcopy(&sdt->sdl.statem, bp->b_rptr, sizeof(sdt->sdl.statem));
		break;
	case _IOC_NR(SDL_IOCCMRESET):
		break;
	case _IOC_NR(SDL_IOCGSTATSP):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdt->sdl.statsp), false)))
			goto enobufs;
		bcopy(&sdt->sdl.statsp, bp->b_rptr, sizeof(sdt->sdl.statsp));
		break;
	case _IOC_NR(SDL_IOCSSTATSP):
		size = sizeof(sdt->sdl.statsp);
		break;
	case _IOC_NR(SDL_IOCGSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdt->sdl.stats), false)))
			goto enobufs;
		bcopy(&sdt->sdl.stats, bp->b_rptr, sizeof(sdt->sdl.stats));
		break;
	case _IOC_NR(SDL_IOCCSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdt->sdl.stats), false)))
			goto enobufs;
		bcopy(&sdt->sdl.stats, bp->b_rptr, sizeof(sdt->sdl.stats));
		bzero(&sdt->sdl.stats, sizeof(sdt->sdl.stats));
		break;
	case _IOC_NR(SDL_IOCGNOTIFY):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdt->sdl.notify), false)))
			goto enobufs;
		bcopy(&sdt->sdl.notify, bp->b_rptr, sizeof(sdt->sdl.notify));
		break;
	case _IOC_NR(SDL_IOCSNOTIFY):
		size = sizeof(sdt->sdl.notify);
		break;
	case _IOC_NR(SDL_IOCCNOTIFY):
		size = sizeof(sdt->sdl.notify);
		break;
	case _IOC_NR(SDL_IOCCMGMT):
		size = sizeof(struct sdl_mgmt);
		break;
	case _IOC_NR(SDL_IOCCPASS):
		size = sizeof(struct sdl_pass);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0)
		return (err);
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
static noinline fastcall int
sdt_copyin(struct sdt *sdt, queue_t *q, mblk_t *mp, struct copyresp *cp, mblk_t *dp)
{
	int err = 0, size = -1;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, MBLKL(dp), false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, MBLKL(dp));
	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sdt_iocname(cp->cp_cmd));
	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SDL_IOCSOPTIONS):
		if ((err = lmi_testoptions(sdt, (struct lmi_option *) mp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdt->option, sizeof(sdt->option));
		size = sizeof(sdt->option);
		break;
	case _IOC_NR(SDL_IOCSCONFIG):
	case _IOC_NR(SDL_IOCCCONFIG):
		if ((err = sdl_testconfig(sdt, (struct sdl_config *) mp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdt->sdl.config, sizeof(sdt->sdl.config));
		size = sizeof(sdt->sdl.config);
		break;
	case _IOC_NR(SDL_IOCTCONFIG):
		err = sdl_testconfig(sdt, (struct sdl_config *) mp->b_rptr);
		size = sizeof(sdt->sdl.config);
		break;
	case _IOC_NR(SDL_IOCSSTATSP):
		bcopy(bp->b_rptr, &sdt->sdl.statsp, sizeof(sdt->sdl.statsp));
		break;
	case _IOC_NR(SDL_IOCSNOTIFY):
		if ((err = sdl_setnotify(sdt, (struct sdl_notify *) mp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdt->sdl.notify, sizeof(sdt->sdl.notify));
		size = sizeof(sdt->sdl.notify);
		break;
	case _IOC_NR(SDL_IOCCNOTIFY):
		if ((err = sdl_clrnotify(sdt, (struct sdl_notify *) mp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdt->sdl.notify, sizeof(sdt->sdl.notify));
		size = sizeof(sdt->sdl.notify);
		break;
	case _IOC_NR(SDL_IOCCMGMT):
		break;
	case _IOC_NR(SDL_IOCCPASS):
		err = EFAULT;
		break;
	default:
		err = EPROTO;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copy_done(q, mp, 0);
		else
			mi_copyout(q, mp);
	}
	return (0);
}
static noinline fastcall int
sdt_copyout(struct sdt *sdt, queue_t *q, mblk_t *mp, struct copyresp *cp)
{
	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sdt_iocname(cp->cp_cmd));
	mi_copyout(q, mp);
	return (0);
}

static noinline fastcall int
sdt_ioctl(struct sdt *sdt, queue_t *q, mblk_t *mp, struct iocblk *ioc)
{
	int size = -1, err = 0;
	mblk_t *bp;

	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCTL(%s)", sdt_iocname(ioc->ioc_cmd));
	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SDT_IOCGOPTION):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdt->option), false)))
			goto enobufs;
		bcopy(&sdt->option, bp->b_rptr, sizeof(sdt->option));
		break;
	case _IOC_NR(SDT_IOCSOPTION):
		size = sizeof(sdt->option);
		break;
	case _IOC_NR(SDT_IOCGCONFIG):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdt->sdt.config), false)))
			goto enobufs;
		bcopy(&sdt->sdt.config, bp->b_rptr, sizeof(sdt->sdt.config));
		break;
	case _IOC_NR(SDT_IOCSCONFIG):
	case _IOC_NR(SDT_IOCTCONFIG):
	case _IOC_NR(SDT_IOCCCONFIG):
		size = sizeof(sdt->sdt.config);
		break;
	case _IOC_NR(SDT_IOCGSTATEM):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdt->sdt.statem), false)))
			goto enobufs;
		bcopy(&sdt->sdt.statem, bp->b_rptr, sizeof(sdt->sdt.statem));
		break;
	case _IOC_NR(SDT_IOCCMRESET):
		break;
	case _IOC_NR(SDT_IOCGSTATSP):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdt->sdt.statsp), false)))
			goto enobufs;
		bcopy(&sdt->sdt.statsp, bp->b_rptr, sizeof(sdt->sdt.statsp));
		break;
	case _IOC_NR(SDT_IOCSSTATSP):
		size = sizeof(sdt->sdt.statsp);
		break;
	case _IOC_NR(SDT_IOCGSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdt->sdt.stats), false)))
			goto enobufs;
		bcopy(&sdt->sdt.stats, bp->b_rptr, sizeof(sdt->sdt.stats));
		break;
	case _IOC_NR(SDT_IOCCSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdt->sdt.stats), false)))
			goto enobufs;
		bcopy(&sdt->sdt.stats, bp->b_rptr, sizeof(sdt->sdt.stats));
		bzero(&sdt->sdt.stats, sizeof(sdt->sdt.stats));
		break;
	case _IOC_NR(SDT_IOCGNOTIFY):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdt->sdt.notify), false)))
			goto enobufs;
		bcopy(&sdt->sdt.notify, bp->b_rptr, sizeof(sdt->sdt.notify));
		break;
	case _IOC_NR(SDT_IOCSNOTIFY):
		size = sizeof(sdt->sdt.notify);
		break;
	case _IOC_NR(SDT_IOCCNOTIFY):
		size = sizeof(sdt->sdt.notify);
		break;
	case _IOC_NR(SDT_IOCCMGMT):
		size = sizeof(struct sdt_mgmt);
		break;
	case _IOC_NR(SDT_IOCCPASS):
		size = sizeof(struct sdt_pass);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0)
		return (err);
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
static noinline fastcall int
sdt_copyin(struct sdt *sdt, queue_t *q, mblk_t *mp, struct copyresp *cp, mblk_t *dp)
{
	int err = 0, size = -1;

	if (!(bp = mi_copyout_alloc(q, mp, NULL, MBLKL(dp), false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, MBLKL(dp));
	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sdt_iocname(cp->cp_cmd));
	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SDT_IOCSOPTIONS):
		if ((err = lmi_testoptions(sdt, (struct lmi_option *) mp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdt->option, sizeof(sdt->option));
		size = sizeof(sdt->option);
		break;
	case _IOC_NR(SDT_IOCSCONFIG):
	case _IOC_NR(SDT_IOCCCONFIG):
		if ((err = sdt_testconfig(sdt, (struct sdt_config *) mp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdt->sdt.config, sizeof(sdt->sdt.config));
		size = sizeof(sdt->sdt.config);
		break;
	case _IOC_NR(SDT_IOCTCONFIG):
		err = sdt_testconfig(sdt, (struct sdt_config *) mp->b_rptr);
		size = sizeof(sdt->sdt.config);
		break;
	case _IOC_NR(SDT_IOCSSTATSP):
		bcopy(bp->b_rptr, &sdt->sdt.statsp, sizeof(sdt->sdt.statsp));
		break;
	case _IOC_NR(SDT_IOCSNOTIFY):
		if ((err = sdt_setnotify(sdt, (struct sdt_notify *) mp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdt->sdt.notify, sizeof(sdt->sdt.notify));
		size = sizeof(sdt->sdt.notify);
		break;
	case _IOC_NR(SDT_IOCCNOTIFY):
		if ((err = sdt_clrnotify(sdt, (struct sdt_notify *) mp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdt->sdt.notify, sizeof(sdt->sdt.notify));
		size = sizeof(sdt->sdt.notify);
		break;
	case _IOC_NR(SDT_IOCCMGMT):
		break;
	case _IOC_NR(SDT_IOCCPASS):
		err = EFAULT;
		break;
	default:
		err = EPROTO;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0) {
		if (size == -1)
			mi_copy_done(q, mp, 0);
		else
			mi_copyout(q, mp);
	}
	return (0);
}
static noinline fastcall int
sdt_copyout(struct sdt *sdt, queue_t *q, mblk_t *mp, struct copyresp *cp)
{
	mi_strlog(q, STRLOGIO, SL_TRACE, "-> M_IOCDATA(%s)", sdt_iocname(cp->cp_cmd));
	mi_copyout(q, mp);
	return (0);
}

/*
 *  =========================================================================
 *
 *  STREAMS MESSAGE HANDLING
 *
 *  =========================================================================
 */

static fastcall int
sdt_m_data(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt;
	int rtn;

	if (!(sdt = sdt_acquire(q)))
		return (-EAGAIN);
	rtn = sdt_data_ind(sdt, q, mp);
	sdt_release(sdt);
	return (rtn);
}
static fastcall int
sdt_m_proto_slow(struct sdt *sdt, queue_t *q, mblk_t *mp, sdt_ulong prim)
{
	switch (prim) {
	case SDT_DAEDT_TRANSMISSION_REQ:
		mi_strlog(q, STRLOGDA, SL_TRACE, "-> SDT_DAEDT_TRANSMISSION_REQ");
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> %s", sdt_primname(prim));
		break;
	}
	switch (prim) {
	case LMI_INFO_REQ:
		return lmi_info_req(sdt, q, mp);
	case LMI_ATTACH_REQ:
		return lmi_attach_req(sdt, q, mp);
	case LMI_DETACH_REQ:
		return lmi_detach_req(sdt, q, mp);
	case LMI_ENABLE_REQ:
		return lmi_enable_req(sdt, q, mp);
	case LMI_DISABLE_REQ:
		return lmi_disable_req(sdt, q, mp);
	case LMI_OPTMGMT_REQ:
		return lmi_optmgmt_req(sdt, q, mp);
	case SDT_DAEDT_TRANSMISSION_REQ:
		return sdt_daedt_transmission_req(sdt, q, mp);
	case SDT_DAEDT_START_REQ:
		return sdt_daedt_start_req(sdt, q, mp);
	case SDT_DAEDR_START_REQ:
		return sdt_daedr_start_req(sdt, q, mp);
	case SDT_AERM_START_REQ:
		return sdt_aerm_start_req(sdt, q, mp);
	case SDT_AERM_STOP_REQ:
		return sdt_aerm_stop_req(sdt, q, mp);
	case SDT_AERM_SET_TI_TO_TIN_REQ:
		return sdt_aerm_set_ti_to_tin_req(sdt, q, mp);
	case SDT_AERM_SET_TI_TO_TIE_REQ:
		return sdt_aerm_set_ti_to_tie_req(sdt, q, mp);
	case SDT_SUERM_START_REQ:
		return sdt_suerm_start_req(sdt, q, mp);
	case SDT_SUERM_STOP_REQ:
		return sdt_suerm_stop_req(sdt, q, mp);
	default:
		return sdt_other_req(sdt, q, mp, prim);
	}
}
static fastcall int
sdt_m_proto(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = sdt_acquire(q);
	sdt_ulong prim = *(typeof(prim) *) mp->b_rptr;
	int rtn;

	if (sdt == NULL)
		return (-EAGAIN);
	if (!MBLKIN(mp, 0, sizeof(prim))) {
		freemsg(mp);
		return (0);
	}
	if (prim == SDT_DAEDT_TRANSMISSION_REQ) {
#ifndef _OPTIMIZE_SPEED
		mi_strlog(q, STRLOGDA, SL_TRACE, "-> SDT_DAEDT_TRANSMISSION_REQ");
#endif				/* _OPTIMIZE_SPEED */
		rtn = sdt_daedt_transmission_req(sdt, q, mp);
	} else {
		rtn = sdt_m_proto_slow(sdt, q, mp, prim);
	}
	return sdt_release(sdt, rtn);
}
static fastcall int
sdt_m_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct sdt *sdt = sdt_acquire(q);
	int rtn = 0;

	if (sdt == NULL)
		return (-EAGAIN);
	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case LMI_IOC_MAGIC:
		rtn = lmi_ioctl(sdt, q, mp, ioc);
		break;
	case SDL_IOC_MAGIC:
		rtn = sdl_ioctl(sdt, q, mp, ioc);
		break;
	case SDT_IOC_MAGIC:
		rtn = sdt_ioctl(sdt, q, mp, ioc);
		break;
	default:
		if (canputnext(q))
			putnext(q, mp);
		else
			rtn = -EBUSY;
		break;
	}
	if (rtn > 0) {
		mi_copy_done(q, mp, rtn);
		rtn = 0;
	}
	return sdt_release(sdt, rtn);
}
static fastcall int
sdt_m_iocdata(queue_t *q, mblk_t *mp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct sdt *sdt = sdt_acquire(q);
	mblk_t *dp;
	int rtn;

	if (sdt == NULL)
		return (-EAGAIN);
	switch (mi_copy_case(q, mp, &dp)) {
	case -1:
		rtn = 0;
		break;
	case MI_COPY_CASE(M_COPYIN, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case LMI_IOC_MAGIC:
			rtn = lmi_copyin(sdt, q, mp, cp, dp);
			break;
		case SDL_IOC_MAGIC:
			rtn = sdl_copyin(sdt, q, mp, cp, dp);
			break;
		case SDT_IOC_MAGIC:
			rtn = sdt_copyin(sdt, q, mp, cp, dp);
			break;
		default:
			rtn = EPROTO;
			break;
		}
		break;
	case MI_COPY_CASE(M_COPYOUT, 1):
		switch (_IOC_TYPE(cp->cp_cmd)) {
		case LMI_IOC_MAGIC:
			rtn = lmi_copyout(sdt, q, mp, cp);
			break;
		case SDL_IOC_MAGIC:
			rtn = sdl_copyout(sdt, q, mp, cp);
			break;
		case SDT_IOC_MAGIC:
			rtn = sdt_copyout(sdt, q, mp, cp);
			break;
		default:
			rtn = EPROTO;
			break;
		}
		break;
	default:
		rtn = EINVAL;
		break;
	}
	if (rtn > 0) {
		mi_copy_done(q, mp, rtn);
		rtn = 0;
	}
	return sdt_release(sdt, rtn);
}
static fastcall int
sdt_m_flush(queue_t *q, mblk_t *mp)
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
static fastcall int
sdt_m_other(queue_t *q, mblk_t *mp, uchar type)
{
	if (pcmsg(type) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static fastcall int
sdt_msg_slow(queue_t *q, mblk_t *mp, uchar type)
{
	switch (type) {
	case M_DATA:
		return sdt_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sdt_m_proto(q, mp);
	case M_IOCTL:
		return sdt_m_ioctl(q, mp);
	case M_IOCDATA:
		return sdt_m_iocdata(q, mp);
	case M_FLUSH:
		return sdt_m_flush(q, mp);
	default:
		return sdt_m_other(q, mp, type);
	}
}
static fastcall int
sdt_msg(queue_t *q, mblk_t *mp)
{
	uchar type = DB_TYPE(mp);

	if (type == M_DATA)
		return sdt_m_data(q, mp);
	if (type == M_PROTO)
		return sdt_m_data(q, mp);
	return sdt_msg_slow(q, mp, type);
}

static fastcall int
cd_m_data(queue_t *q, mblk_t *mp)
{
	struct cd *cd;
	int rtn;

	if (!(cd = cd_acquire(q)))
		return (-EAGAIN);
	rtn = cd_data_ind(cd, q, mp);
	cd_release(cd);
	return (0);
}
static fastcall int
cd_m_proto_slow(struct cd *cd, queue_t *q, mblk_t *mp, cd_ulong prim)
{
	switch (prim) {
	case CD_UNITDATA_IND:
	case CD_UNITDATA_ACK:
		mi_strlog(q, STRLOGDA, SL_TRACE, "%s <-", cd_primname(prim));
		break;
	default:
		mi_strlog(q, STRLOGRX, SL_TRACE, "%s <-", cd_primname(prim));
		break;
	}
	switch (prim) {
	case CD_INFO_ACK:
		return cd_info_ack(cd, q, mp);
	case CD_OK_ACK:
		return cd_ok_ack(cd, q, mp);
	case CD_ERROR_ACK:
		return cd_error_ack(cd, q, mp);
	case CD_ENABLE_CON:
		return cd_enable_con(cd, q, mp);
	case CD_DISABLE_CON:
		return cd_disable_con(cd, q, mp);
	case CD_ERROR_IND:
		return cd_error_ind(cd, q, mp);
	case CD_UNITDATA_ACK:
		return cd_unitdata_ack(cd, q, mp);
	case CD_UNITDATA_IND:
		return cd_unitdata_ind(cd, q, mp);
	case CD_BAD_FRAME_IND:
		return cd_bad_frame_ind(cd, q, mp);
	case CD_MODEM_SIG_IND:
		return cd_modem_sig_ind(cd, q, mp);
	default:
		return cd_other_ind(cd, q, mp, prim);
	}
}
static fastcall int
cd_m_proto(queue_t *q, mblk_t *mp)
{
	struct cd *cd = cd_acquire(q);
	cd_ulong prim = *(typeof(prim) *) mp->b_rptr;
	int rtn;

	if (cd == NULL)
		return (-EAGAIN);
	if (!MBLKIN(mp, 0, sizeof(prim))) {
		freemsg(mp);
		return (0);
	}
	if (prim == CD_UNITDATA_IND) {
#ifndef _OPTIMIZE_SPEED
		mi_strlog(q, STRLOGDA, SL_TRACE, "CD_UNITDATA_IND <-");
#endif				/* _OPTIMIZE_SPEED */
		rtn = cd_unitdata_ind(cd, q, mp);
	} else if (prim = CD_UNITDATA_ACK) {
#ifndef _OPTIMIZE_SPEED
		mi_strlog(q, STRLOGDA, SL_TRACE, "CD_UNITDATA_ACK <-");
#endif				/* _OPTIMIZE_SPEED */
		rtn = cd_unitdata_ack(cd, q, mp);
	} else {
		rtn = cd_m_proto_slow(cd, q, mp, prim);
	}
	return cd_release(cd, rtn);
}
static fastcall int
cd_m_flush(queue_t *q, mblk_t *mp)
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
static fastcall int
cd_m_other(queue_t *q, mblk_t *mp, uchar type)
{
	if (pcmsg(type) || bcanputnext(q, mp->b_band)) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static fastcall int
cd_msg_slow(queue_t *q, mblk_t *mp, uchar type)
{
	switch (type) {
	case M_DATA:
		return cd_m_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cd_m_proto(q, mp);
	case M_FLUSH:
		return cd_m_flush(q, mp);
	default:
		return cd_m_other(q, mp, type);
	}
}
static fastcall int
cd_msg(queue_t *q, mblk_t *mp)
{
	uchar type = DB_TYPE(mp);

	if (type == M_DATA)
		return cd_m_data(q, mp);
	if (type == M_PROTO)
		return cd_m_proto(q, mp);
	return cd_msg_slow(q, mp, type);
}

/*
 *  =========================================================================
 *
 *  STREAMS PUT AND SERVICE PROCEDURES
 *
 *  =========================================================================
 */

static streamscall int
sdt_putp(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || sdt_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int
sdt_srvp(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q)))
		if (sdt_msg(q, mp))
			break;
	if (mp)
		putbq(q, mp);
	return (0);
}
static streamscall int
cd_srvp(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q)))
		if (cd_msg(q, mp))
			break;
	if (mp)
		putbq(q, mp);
	return (0);
}
static streamscall int
cd_putp(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || cd_msg(q, mp))
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

static streamscall int
sdt_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct sdt_pair *p;

	int err;

	if (!!q->q_ptr)
		return (0);
	if ((err = mi_open_comm(&sdt_opens, sizeof(*p), q, devp, oflags, sflag, crp)))
		return (err);
	p = PRIV(q);
	bzero(p, sizeof(*p));

	p->r_priv.pair = p;
	p->r_priv.cd = &p->w_priv;
	p->r_priv.oq = q;
	p->r_priv.state.flags = 0;
	p->r_priv.state.state = LMI_UNUSABLE;
	p->r_priv.oldstate.flags = 0;
	p->r_priv.oldstate.state = LMI_UNUSABLE;
	p->r_priv.chkstate.flags = 0;
	p->r_priv.chkstate.state = LMI_UNUSABLE;

	p->w_priv.pair = p;
	p->w_priv.sdt = &p->r_priv;
	p->w_priv.oq = WR(q);
	p->w_priv.state.flags = 0;
	p->w_priv.state.state = CD_UNUSABLE;
	p->w_priv.oldstate.flags = 0;
	p->w_priv.oldstate.state = CD_UNUSABLE;
	p->w_priv.chkstate.flags = 0;
	p->w_priv.chkstate.state = CD_UNUSABLE;

	return (0);
}
static streamscall int
sdt_qclose(queue_t *q, int oflags, cred_t *crp)
{
}

/*
 *  =========================================================================
 *
 *  STREAMS REGISTRATION INITIALIZATION
 *
 *  =========================================================================
 */

unsigned short modid = MOD_ID;

#ifdef LINUX
#ifdef module_param
module_param(modid, ushort, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for SDT module.  (0 for allocation.)");
#endif				/* LINUX */

static struct qinit sdt_rinit = {
	.qi_putp = cd_putp,
	.qi_srvp = cd_srvp,
	.qi_qopen = sdt_qopen,
	.qi_qclose = sdt_qclose,
	.qi_minfo = &sdt_minfo,
	.qi_mstat = &sdt_rstat,
};
static struct qinit sdt_winit = {
	.qi_putp = sdt_putp,
	.qi_srvp = sdt_srvp,
	.qi_minfo = &sdt_minfo,
	.qi_mstat = &sdt_rstat,
};

static struct streamtab sdt_cdinfo = {
	.st_rdinit = &sdt_rinit,
	.st_wrinit = &sdt_winit,
};

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static struct fmodsw *sdt_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sdt_cdinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
sdt_cdinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&sdt_fmod)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d, err = %d\n", MOD_NAME,
			(int) modid, err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}
static __exit void
sdt_cdterminate(void)
{
	int err;

	if ((err = unregister_strmod(&sdt_fmod)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister module %d, err = %d\n", MOD_NAME,
			(int) modid, err);
	return;
}

#ifdef LINUX
module_init(sdt_cdinit);
module_exit(sdt_cdterminate);
#endif				/* LINUX */
