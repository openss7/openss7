/*****************************************************************************

 @(#) $RCSfile: sdl_ch.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 07:11:09 $

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

 Last Modified $Date: 2008-04-29 07:11:09 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sdl_ch.c,v $
 Revision 0.9.2.4  2008-04-29 07:11:09  brian
 - updating headers for release

 Revision 0.9.2.3  2007/08/15 05:20:11  brian
 - GPLv3 updates

 Revision 0.9.2.2  2007/08/12 16:20:25  brian
 - new PPA handling

 Revision 0.9.2.1  2007/07/21 20:22:00  brian
 - added channel modules

 *****************************************************************************/

#ident "@(#) $RCSfile: sdl_ch.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 07:11:09 $"

static char const ident[] = "$RCSfile: sdl_ch.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 07:11:09 $";

#define _MPS_SOURCE 1
#define _LFS_SOURCE 1

/*
 * `This is the SDL-CH STREAMS pushable module.  Its purpose is to convert from a general purpose
 * Channel Interface (CHI) Stream to an SS7-specific Signalling Data Link (SDL) Stream.  For the
 * most part this is a one-to-one conversion as both interfaces are an abstraction of the same
 * thing.  This is maybe not the most efficient way of going about doing things.  The conversion
 * step might be costly (but then again it might not: in the main data flow it could be as little as
 * the overhead of one function call to the module's put procedure).
 *
 * Another more direct approach is to have a SDT module that pushes directly over a CH Stream
 * instead of over an SDL Stream.  Nevertheless, this module can be useful for testing.
 */
#define _LFS_SOURCE	1
#define _SUN_SOURCE	1

#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <sys/chi.h>
#include <sys/chi_ioctl.h>

#define SDL_DESCRIP	"SS7/SDL: (Signalling Data Link) STREAMS MODULE."
#define SDL_REVISION	"OpenSS7 $RCSfile: sdl_ch.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 07:11:09 $"
#define SDL_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define SDL_DEVICE	"Provides OpenSS7 SDL-CH module."
#define SDL_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SDL_LICENSE	"GPL"
#define SDL_BANNER	SDL_DESCRIP	"\n" \
			SDL_REVISION	"\n" \
			SDL_COPYRIGHT	"\n" \
			SDL_DEVICE	"\n" \
			SDL_CONTACT	"\n"
#define SDL_SPLASH	SDL_DEVICE	" - " \
			SDL_REVISIN	"\n"

#ifdef LINUX
MODULE_AUTHOR(SDL_CONTACT);
MODULE_DESCRIPTION(SDL_DESCRIP);
MODULE_SUPPORTED_DEVICE(SDL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SDL_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-sdl");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define SDL_MOD_ID	CONFIG_STREAMS_SDL_MODID
#define SDL_MOD_NAME	CONFIG_STREAMS_SDL_NAME
#endif				/* LFS */

#ifndef SDL_MOD_NAME
#define SDL_MOD_NAME	"sdl"
#endif				/* SDL_MOD_NAME */

#ifndef SDL_MOD_ID
#define SDL_MOD_ID	0
#endif				/* SDL_MOD_ID */

/*
 *  ========================================================================
 *
 *  STREAMS DEFINITIONS
 *
 *  ========================================================================
 */

#define MOD_ID		SDL_MOD_ID
#define MOD_NAME	SDL_MOD_NAME
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
 *  ========================================================================
 *
 *  PRIVATE STRUCTURE
 *
 *  ========================================================================
 */

struct sdl_pair;
struct sdl;
struct ch;

struct sdl {
	struct sdl_pair *pair;
	struct ch *ch;
	queue_t *q;
	uint max_sdu;
	uint min_sdu;
	uint header_len;
	uint ppa_style;
	uint flags;
	uint m_state;			/* transient state */
	uint m_oldstate;		/* original state */
	uint m_chkstate;		/* checkpoint state */
	struct lmi_option option;
	struct lmi_notify notify;
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

struct ch {
	struct sdl_pair *pair;
	struct sdl *sdl;
	queue_t *q;
	uint flags;
	uint m_state;			/* transient state */
	uint m_oldstate;		/* original state */
	uint m_chkstate;		/* checkpoint state */
	struct {
		struct ch_notify notify;
		struct ch_config config;
		struct ch_statem statem;
		struct ch_stats statsp;
		struct ch_stats stats;
	} ch;
};

struct sdl_pair {
	struct sdl r_priv;
	struct ch w_priv;
};

#define SDL_PRIV(q) (&((struct sdl_pair *)(q)->q_ptr)->r_priv)
#define  CH_PRIV(q) (&((struct sdl_pair *)(q)->q_ptr)->w_priv)

#define STRLOGER	0	/* log Stream errors */
#define STRLOGST	1	/* log Stream state transitions */
#define STRLOGTO	2	/* log Stream timeouts */
#define STRLOGRX	3	/* log Stream primitives received */
#define STRLOGTX	4	/* log Stream primitives issued */
#define STRLOGTE	5	/* log Stream timer events */
#define STRLOGDA	6	/* log Stream data */

static inline const char *
sdl_iocname(int cmd)
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
	default:
		return ("(Unknown IOC type.)");
	}
}

static inline const char *
sdl_primname(sdl_ulong prim)
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
	default:
		return ("(Unknown Primitive)");
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
 * sdl_get_m_state: - get management state for private structure
 * @sdl: private structure
 */
static int
sdl_get_m_state(struct sdl *sdl)
{
	return (sdl->m_state);
}

/**
 * sdl_set_m_state: - set management state for private structure
 * @sdl: private structure
 * @newstate: new state
 */
static int
sdl_set_m_state(struct sdl *sdl, int newstate)
{
	int oldstate = sdl->m_state;

	if (newstate != oldstate) {
		sdl->m_state = newstate;
		mi_strlog(sdl->q, STRLOGST, SL_TRACE, "%s <- %s", sdl_statename(newstate),
			  sdl_statename(oldstate));
	}
	return (newstate);
}

static int
sdl_save_m_state(struct sdl *sdl)
{
	return ((sdl->m_oldstate = sdl_get_m_state(sdl)));
}

static int
sdl_restore_m_state(struct sdl *sdl)
{
	return sdl_set_m_state(sdl, sdl->m_oldstate);
}

static int
sdl_checkpoint_m_state(struct sdl *sdl, int newstate)
{
	sdl->m_chkstate = sdl_get_m_state(sdl);
	return sdl_set_m_state(sdl, newstate);
}

static int
sdl_fallback_m_state(struct sdl *sdl)
{
	return sdl_set_m_state(sdl, sdl->m_chkstate);
}

static inline const char *
ch_primname(ch_ulong prim)
{
	switch (prim) {
	case CH_INFO_REQ:
		return ("CH_INFO_REQ");
	case CH_OPTMGMT_REQ:
		return ("CH_OPTMGMT_REQ");
	case CH_ATTACH_REQ:
		return ("CH_ATTACH_REQ");
	case CH_ENABLE_REQ:
		return ("CH_ENABLE_REQ");
	case CH_CONNECT_REQ:
		return ("CH_CONNECT_REQ");
	case CH_DATA_REQ:
		return ("CH_DATA_REQ");
	case CH_DISCONNECT_REQ:
		return ("CH_DISCONNECT_REQ");
	case CH_DISABLE_REQ:
		return ("CH_DISABLE_REQ");
	case CH_DETACH_REQ:
		return ("CH_DETACH_REQ");
	case CH_INFO_ACK:
		return ("CH_INFO_ACK");
	case CH_OPTMGMT_ACK:
		return ("CH_OPTMGMT_ACK");
	case CH_OK_ACK:
		return ("CH_OK_ACK");
	case CH_ERROR_ACK:
		return ("CH_ERROR_ACK");
	case CH_ENABLE_CON:
		return ("CH_ENABLE_CON");
	case CH_CONNECT_CON:
		return ("CH_CONNECT_CON");
	case CH_DATA_IND:
		return ("CH_DATA_IND");
	case CH_DISCONNECT_IND:
		return ("CH_DISCONNECT_IND");
	case CH_DISCONNECT_CON:
		return ("CH_DISCONNECT_CON");
	case CH_DISABLE_IND:
		return ("CH_DISABLE_IND");
	case CH_DISABLE_CON:
		return ("CH_DISABLE_CON");
	case CH_EVENT_IND:
		return ("CH_EVENT_IND");
	default:
		return ("(Unknown Primitive)");
	}
}

static inline const char *
ch_statename(int state)
{
	switch (state) {
	case CHS_UNINIT:
		return ("CHS_UNINIT");
	case CHS_UNUSABLE:
		return ("CHS_UNUSABLE");
	case CHS_DETACHED:
		return ("CHS_DETACHED");
	case CHS_WACK_AREQ:
		return ("CHS_WACK_AREQ");
	case CHS_WACK_UREQ:
		return ("CHS_WACK_UREQ");
	case CHS_ATTACHED:
		return ("CHS_ATTACHED");
	case CHS_WACK_EREQ:
		return ("CHS_WACK_EREQ");
	case CHS_WCON_EREQ:
		return ("CHS_WCON_EREQ");
	case CHS_WACK_RREQ:
		return ("CHS_WACK_RREQ");
	case CHS_WCON_RREQ:
		return ("CHS_WCON_RREQ");
	case CHS_ENABLED:
		return ("CHS_ENABLED");
	case CHS_WACK_CREQ:
		return ("CHS_WACK_CREQ");
	case CHS_WCON_CREQ:
		return ("CHS_WCON_CREQ");
	case CHS_WACK_DREQ:
		return ("CHS_WACK_DREQ");
	case CHS_WCON_DREQ:
		return ("CHS_WCON_DREQ");
	case CHS_CONNECTED:
		return ("CHS_CONNECTED");
	default:
		return ("(Unknown State)");
	}
}

/**
 * ch_get_m_state: - get management state for private structure
 * @ch: private structure
 */
static int
ch_get_m_state(struct ch *ch)
{
	return (ch->m_state);
}

/**
 * ch_set_m_state: - set management state for private structure
 * @ch: private structure
 * @newstate: new state
 */
static int
ch_set_m_state(struct ch *ch, int newstate)
{
	int oldstate = ch->m_state;

	if (newstate != oldstate) {
		ch->m_state = newstate;
		mi_strlog(ch->q, STRLOGST, SL_TRACE, "%s <- %s", ch_statename(newstate),
			  ch_statename(oldstate));
	}
	return (newstate);
}

static int
ch_save_m_state(struct ch *ch)
{
	return ((ch->m_oldstate = ch_get_m_state(ch)));
}

static int
ch_restore_m_state(struct ch *ch)
{
	return ch_set_m_state(ch, ch->m_oldstate);
}

static int
ch_checkpoint_m_state(struct ch *ch, int newstate)
{
	ch->m_chkstate = ch_get_m_state(ch);
	return ch_set_m_state(ch, newstate);
}

static inline int
ch_fallback_m_state(struct ch *ch)
{
	return ch_set_m_state(ch, ch->m_chkstate);
}

/*
 *  ========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  SDL Provider -> SDL User primitives.
 *
 *  ========================================================================
 */

static noinline __unlikely int
m_error(struct sdl *sdl, queue_t *q, mblk_t *msg, int err)
{
	mblk_t *mp;

	if ((mp = mi_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*mp->b_wptr++ = err;
		*mp->b_wptr++ = err;
		freemsg(msg);
		mi_strlog(q, STRLOGER, SL_TRACE, "<- M_ERROR");
		sdl_set_m_state(sdl, LMI_UNUSABLE);
		putnext(sdl->q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_info_ack:- issue LMI_INFO_ACK primitive
 * @sdl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline __unlikely int
lmi_info_ack(struct sdl *sdl, queue_t *q, mblk_t *msg)
{
	lmi_info_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + sdl->ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_band = 0;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = LMI_CURRENT_VERSION;
		p->lmi_state = sdl_get_m_state(sdl);
		p->lmi_max_sdu = sdl->max_sdu;
		p->lmi_min_sdu = sdl->min_sdu;
		p->lmi_header_len = sdl->header_len;
		p->lmi_ppa_style = sdl->ppa_style;
		p->lmi_ppa_length = sdl->ppa_len;
		p->lmi_ppa_offset = sizeof(*p);
		p->lmi_prov_flags = sdl->flags & (SDL_RX_DIRECTION | SDL_TX_DIRECTION);
		p->lmi_prov_state = p->lmi_prov_flags ? SDL_CONNECTED : SDL_DISCONNECTED;
		mp->b_wptr += sizeof(*p);
		bcopy(sdl->ppa, mp->b_wptr, sdl->ppa_len);
		mp->b_wptr += sdl->ppa_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_INFO_ACK");
		putnext(sdl->q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_ok_ack:- issue LMI_OK_ACK primitive
 * @sdl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: correct primitive
 */
static noinline __unlikely int
lmi_ok_ack(struct sdl *sdl, queue_t *q, mblk_t *msg, sdl_long prim)
{
	lmi_ok_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_band = 0;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (prim) {
		case LMI_ATTACH_REQ:
			p->lmi_state = sdl_set_m_state(sdl, LMI_DISABLED);
			break;
		case LMI_DETACH_REQ:
			p->lmi_state = sdl_set_m_state(sdl, LMI_UNATTACHED);
			break;
		default:
			mi_strlog(q, STRLOGER, SL_ERROR, "LMI_OK_ACK for wrong primitive");
			p->lmi_state = sdl_get_m_state(sdl);
			break;
		}
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_OK_ACK");
		putnext(sdl->q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_error_ack:- issue LMI_ERROR_ACK primitive
 * @sdl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @prim: primitive in error
 * @errno: the error number
 */
static noinline __unlikely int
lmi_error_ack(struct sdl *sdl, queue_t *q, mblk_t *msg, sdl_long prim, sdl_long errno)
{
	lmi_error_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_band = 0;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = errno > 0 ? errno : LMI_SYSERR;
		p->lmi_reason = errno > 0 ? 0 : -errno;
		p->lmi_error_primitive = prim;
		p->lmi_state = sdl_fallback_m_state(sdl);
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_ERROR_ACK");
		putnext(sdl->q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_enable_con:- issue LMI_ENABLE_CON primitive
 * @sdl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline __unlikely int
lmi_enable_con(struct sdl *sdl, queue_t *q, mblk_t *msg)
{
	lmi_enable_con_t *p;
	mblk_t *mp;

	if (bcanputnext(sdl->q, 1)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_ENABLE_CON;
			p->lmi_state = sdl_set_m_state(sdl, LMI_ENABLED);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_ENABLE_CON");
			putnext(sdl->q, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * lmi_disable_con:- issue LMI_DISABLE_CON primitive
 * @sdl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline __unlikely int
lmi_disable_con(struct sdl *sdl, queue_t *q, mblk_t *msg)
{
	lmi_disable_con_t *p;
	mblk_t *mp;

	if (bcanputnext(sdl->q, 1)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_DISABLE_CON;
			p->lmi_state = sdl_set_m_state(sdl, LMI_DISABLED);
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_DISABLE_CON");
			putnext(sdl->q, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * lmi_optmgmt_ack:- issue LMI_OPGMGMT_ACK primitive
 * @sdl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @flags: option flags
 */
static inline __unlikely int
lmi_optmgmt_ack(struct sdl *sdl, queue_t *q, mblk_t *msg, caddr_t opt_ptr, size_t opt_len,
		uint flags)
{
	lmi_optmgmt_ack_t *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_band = 0;
		p = (typeof(p)) mp->b_wptr;
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_OPTMGMT_ACK");
		putnext(sdl->q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * lmi_error_ind:- issue LMI_ERROR_IND primitive
 * @sdl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @errno: the error number
 */
static noinline __unlikely int
lmi_error_ind(struct sdl *sdl, queue_t *q, mblk_t *msg, sdl_long errno)
{
	lmi_error_ind_t *p;
	mblk_t *mp;

	if (bcanputnext(sdl->q, 1)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_ERROR_IND;
			p->lmi_errno = errno > 0 ? errno : LMI_SYSERR;
			p->lmi_reason = errno > 0 ? 0 : -errno;
			p->lmi_state = sdl_get_m_state(sdl);
			mp->b_wptr += sizeof(*p);
			if (msg) {
				/* if there is data, the user can have that too */
				mp->b_cont = msg->b_cont;
				freeb(msg);
			}
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_ERROR_IND");
			putnext(sdl->q, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * lmi_stats_ind:- issue LMI_STATS_IND primitive
 * @sdl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline __unlikely int
lmi_stats_ind(struct sdl *sdl, queue_t *q, mblk_t *msg)
{
	lmi_stats_ind_t *p;
	mblk_t *mp;

	if (bcanputnext(sdl->q, 0)) {
		if ((mp = mi_allocb(q, sizeof(*p) + sizeof(sdl->sdl.stats), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 0;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_STATS_IND;
			p->lmi_interval = 0;
			drv_getparm(TIME, &p->lmi_timestamp);
			mp->b_wptr += sizeof(*p);
			*(struct sdl_stats *) mp->b_wptr = sdl->sdl.stats;
			mp->b_wptr += sizeof(sdl->sdl.stats);
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_STATS_IND");
			putnext(sdl->q, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * lmi_event_ind:- issue LMI_EVENT_IND primitive
 * @sdl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @oid: object identifier
 * @level: severity
 * @inf_ptr: information pointer
 * @inf_len: information length
 */
static inline __unlikely int
lmi_event_ind(struct sdl *sdl, queue_t *q, mblk_t *msg, sdl_ulong oid, sdl_ulong level,
	      caddr_t inf_ptr, size_t inf_len)
{
	lmi_event_ind_t *p;
	mblk_t *mp;

	if (bcanputnext(sdl->q, 0)) {
		if ((mp = mi_allocb(q, sizeof(*p) + inf_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 0;
			p = (typeof(p)) mp->b_wptr;
			p->lmi_primitive = LMI_EVENT_IND;
			p->lmi_objectid = oid;
			drv_getparm(TIME, &p->lmi_timestamp);
			p->lmi_severity = level;
			mp->b_wptr += sizeof(*p);
			bcopy(inf_ptr, mp->b_wptr, inf_len);
			mp->b_wptr += inf_len;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- LMI_EVENT_IND");
			putnext(sdl->q, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sdl_received_bits_ind:- issue SDL_RECEIVED_BITS_IND primitive
 * @sdl: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @dp: user data
 *
 * This function should not be used.  Data should be transferred as M_DATA
 * blocks only.
 */
static inline __hot int
sdl_received_bits_ind(struct sdl *sdl, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	sdl_received_bits_ind_t *p;
	mblk_t *mp;

	if (canputnext(sdl->q)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->sdl_primitive = SDL_RECEIVED_BITS_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- SDL_RECEIVED_BITS_IND");
			putnext(sdl->q, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * sdl_disconnect_ind:- issue SDL_DISCONNECT_IND primitive
 * @sdl: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline __unlikely int
sdl_disconnect_ind(struct sdl *sdl, queue_t *q, mblk_t *msg, uint flags)
{
	sdl_disconnect_ind_t *p;
	mblk_t *mp;

	if (bcanputnext(sdl->q, 1)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			p->sdl_primitive = SDL_DISCONNECT_IND;
			mp->b_wptr += sizeof(*p);
			sdl->flags &= ~flags;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "<- SDL_DISCONNECT_IND");
			putnext(sdl->q, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  ========================================================================
 *
 *  ISSUED PRIMITIVES
 *
 *  CH User -> CH Provider primitives.
 *
 *  ========================================================================
 */

/**
 * ch_info_req: - issue CH_INFO_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static inline __unlikely int
ch_info_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_info_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_band = 0;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "CH_INFO_REQ ->");
		putnext(ch->q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_optmgmt_req: - issue CH_OPTMGMT_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @opt_ptr: options pointer
 * @opt_len: options length
 * @flags: management flags
 */
static inline __unlikely int
ch_optmgmt_req(struct ch *ch, queue_t *q, mblk_t *msg, caddr_t opt_ptr, size_t opt_len, uint flags)
{
	struct CH_optmgmt_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_band = 0;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OPTMGMT_REQ;
		p->ch_opt_length = opt_len;
		p->ch_opt_offset = opt_len ? sizeof(*p) : 0;
		p->ch_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "CH_OPTMGMT_REQ ->");
		putnext(ch->q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_attach_req: - issue CH_ATTACH_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @add_ptr: address pointer
 * @add_len: address length
 * @flags: attach flags
 */
static noinline __unlikely int
ch_attach_req(struct ch *ch, queue_t *q, mblk_t *msg, caddr_t add_ptr, size_t add_len, uint flags)
{
	struct CH_attach_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_band = 0;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_ATTACH_REQ;
		p->ch_addr_length = add_len;
		p->ch_addr_offset = add_len ? sizeof(*p) : 0;
		p->ch_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "CH_ATTACH_REQ ->");
		ch_checkpoint_m_state(ch, CHS_WACK_AREQ);
		putnext(ch->q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_enable_req: - issue CH_ENABLE_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline __unlikely int
ch_enable_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_enable_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_band = 0;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_ENABLE_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "CH_ENABLE_REQ ->");
		ch_checkpoint_m_state(ch, CHS_WACK_EREQ);
		putnext(ch->q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_connect_req: - issue CH_CONNECT_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: connection flags
 * @slot: connection slot
 */
static noinline __unlikely int
ch_connect_req(struct ch *ch, queue_t *q, mblk_t *msg, uint flags, uint slot)
{
	struct CH_connect_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_band = 0;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_CONNECT_REQ;
		p->ch_conn_flags = flags;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "CH_CONNECT_REQ ->");
		ch_checkpoint_m_state(ch, CHS_WACK_CREQ);
		putnext(ch->q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_data_req: - issue CH_DATA_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @slot: slot
 * @dp: user data
 */
static inline __hot int
ch_data_req(struct ch *ch, queue_t *q, mblk_t *msg, uint slot, mblk_t *dp)
{
	struct CH_data_req *p;
	mblk_t *mp;

	if (bcanputnext(ch->q, 0)) {
		if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 0;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DATA_REQ;
			p->ch_slot = slot;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			mi_strlog(q, STRLOGTX, SL_TRACE, "CH_DATA_REQ ->");
			putnext(ch->q, mp);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/**
 * ch_disconnect_req: - issue CH_DISCONNECT_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 * @flags: connection flags
 * @slot: slot
 */
static noinline __unlikely int
ch_disconnect_req(struct ch *ch, queue_t *q, mblk_t *msg, uint flags, uint slot)
{
	struct CH_disconnect_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		mp->b_band = 0;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISCONNECT_REQ;
		p->ch_conn_flags = flags;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "CH_DISCONNECT_REQ ->");
		ch_checkpoint_m_state(ch, CHS_WACK_DREQ);
		putnext(ch->q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disable_req: - issue CH_DISABLE_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline __unlikely int
ch_disable_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_disable_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_band = 0;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISABLE_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "CH_DISABLE_REQ ->");
		ch_checkpoint_m_state(ch, CHS_WACK_RREQ);
		putnext(ch->q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_detach_req: - issue CH_DETACH_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free upon success
 */
static noinline __unlikely int
ch_detach_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_detach_req *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_band = 0;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DETACH_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, STRLOGTX, SL_TRACE, "CH_DETACH_REQ ->");
		ch_checkpoint_m_state(ch, CHS_WACK_UREQ);
		putnext(ch->q, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  ========================================================================
 *
 *  PROTOCOL STATE MACHINE
 *
 *  ========================================================================
 */

/*
 *  ========================================================================
 *
 *  RECEIVED PRIMITIVES
 *
 *  SDL User -> SDL Provider primitives.
 *
 *  ========================================================================
 */

/**
 * lmi_info_req: - process LMI_INFO_REQ primitive
 * @sdl: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_INFO_REQ primitive
 */
static noinline __unlikely int
lmi_info_req(struct sdl *sdl, queue_t *q, mblk_t *mp)
{
	lmi_info_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdl_get_m_state(sdl) == LMI_UNUSABLE)
		goto fatalerr;
	return lmi_info_ack(sdl, q, mp);
      fatalerr:
	err = LMI_FATALERR;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_ack(sdl, q, mp, LMI_INFO_REQ, err);
}

/**
 * lmi_attach_req: - process LMI_ATTACH_REQ primitive
 * @sdl: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_ATTACH_REQ primitive
 */
static noinline __unlikely int
lmi_attach_req(struct sdl *sdl, queue_t *q, mblk_t *mp)
{
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	caddr_t ppa_ptr;
	size_t ppa_len;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->lmi_ppa_offset, p->lmi_ppa_length))
		goto badppa;
	if (sdl_get_m_state(sdl) == LMI_UNUSABLE)
		goto fatalerr;
	if (sdl_get_m_state(sdl) != LMI_UNATTACHED)
		goto outstate;
	if (sdl->ppa_style != LMI_STYLE2)
		goto badprim;
	ppa_ptr = (caddr_t) (mp->b_rptr + p->lmi_ppa_offset);
	ppa_len = p->lmi_ppa_length;
	sdl_checkpoint_m_state(sdl, LMI_ATTACH_PENDING);
	return ch_attach_req(sdl->ch, q, mp, ppa_ptr, ppa_len, 0);
      badprim:
	err = LMI_BADPRIM;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      fatalerr:
	err = LMI_FATALERR;
	goto error;
      badppa:
	err = LMI_BADPPA;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_ack(sdl, q, mp, LMI_ATTACH_REQ, err);
}

/**
 * lmi_detach_req: - process LMI_DETACH_REQ primitive
 * @sdl: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_DETACH_REQ primitive
 */
static noinline __unlikely int
lmi_detach_req(struct sdl *sdl, queue_t *q, mblk_t *mp)
{
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdl_get_m_state(sdl) == LMI_UNUSABLE)
		goto fatalerr;
	if (sdl_get_m_state(sdl) != LMI_DISABLED)
		goto outstate;
	if (sdl->ppa_style != LMI_STYLE2)
		goto badprim;
	sdl_checkpoint_m_state(sdl, LMI_DETACH_PENDING);
	return ch_detach_req(sdl->ch, q, mp);
badprim:
	err = LMI_BADPRIM;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      fatalerr:
	err = LMI_FATALERR;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_ack(sdl, q, mp, LMI_DETACH_REQ, err);
}

/**
 * lmi_enable_req: - process LMI_ENABLE_REQ primitive
 * @sdl: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_ENABLE_REQ primitive
 */
static noinline __unlikely int
lmi_enable_req(struct sdl *sdl, queue_t *q, mblk_t *mp)
{
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (sdl_get_m_state(sdl) == LMI_UNUSABLE)
		goto fatalerr;
	if (sdl_get_m_state(sdl) != LMI_DISABLED)
		goto outstate;
	if (!MBLKIN(mp, p->lmi_rem_offset, p->lmi_rem_length))
		goto badaddr;
	sdl_checkpoint_m_state(sdl, LMI_ENABLE_PENDING);
	return ch_enable_req(sdl->ch, q, mp);
      badaddr:
	err = LMI_BADADDRESS;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      fatalerr:
	err = LMI_FATALERR;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_ack(sdl, q, mp, LMI_ENABLE_REQ, err);
}

/**
 * lmi_disable_req: - process LMI_DISABLE_REQ primitive
 * @sdl: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_DISABLE_REQ primitive
 */
static noinline __unlikely int
lmi_disable_req(struct sdl *sdl, queue_t *q, mblk_t *mp)
{
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdl_get_m_state(sdl) == LMI_UNUSABLE)
		goto fatalerr;
	if (sdl_get_m_state(sdl) != LMI_ENABLED)
		goto outstate;
	sdl_checkpoint_m_state(sdl, LMI_DISABLE_PENDING);
	return ch_disable_req(sdl->ch, q, mp);
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      fatalerr:
	err = LMI_FATALERR;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_ack(sdl, q, mp, LMI_DISABLE_REQ, err);
}

/**
 * lmi_optmgmt_req: - process LMI_OPTMGMT_REQ primitive
 * @sdl: private structure
 * @q: active queue (write queue)
 * @mp: the LMI_OPTMGMT_REQ primitive
 */
static noinline __unlikely int
lmi_optmgmt_req(struct sdl *sdl, queue_t *q, mblk_t *mp)
{
	lmi_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdl_get_m_state(sdl) == LMI_UNUSABLE)
		goto fatalerr;
	goto notsupp;
      notsupp:
	err = LMI_NOTSUPP;
	goto error;
      fatalerr:
	err = LMI_FATALERR;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_ack(sdl, q, mp, LMI_OPTMGMT_REQ, err);
}

/**
 * sdl_bits_for_transmission_req: - process SDL_BITS_FOR_TRANSMISSION_REQ primitive
 * @sdl: private structure
 * @q: active queue (write queue)
 * @mp: the SDL_BITS_FOR_TRANSMISSION_REQ primitive
 */
static inline __hot int
sdl_bits_for_transmission_req(struct sdl *sdl, queue_t *q, mblk_t *mp)
{
	sdl_bits_for_transmission_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdl_get_m_state(sdl) == LMI_UNUSABLE)
		goto fatalerr;
	if (sdl_get_m_state(sdl) != LMI_ENABLED)
		goto outstate;
	switch (ch_get_m_state(sdl->ch)) {
	case CHS_WACK_CREQ:
	case CHS_WCON_CREQ:
		noenable(q);
		return (-EAGAIN);
	}
	if (!(sdl->flags & SDL_TX_DIRECTION))
		goto disc;
	return ch_data_req(sdl->ch, q, mp, 0, mp->b_cont);
      disc:
	err = LMI_DISC;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      fatalerr:
	err = LMI_FATALERR;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_ind(sdl, q, mp, err);
}

/**
 * sdl_connect_req: - process SDL_CONNECT_REQ primitive
 * @sdl: private structure
 * @q: active queue (write queue)
 * @mp: the SDL_CONNECT_REQ primitive
 */
static noinline __unlikely int
sdl_connect_req(struct sdl *sdl, queue_t *q, mblk_t *mp)
{
	sdl_connect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdl_get_m_state(sdl) == LMI_UNUSABLE)
		goto fatalerr;
	if (sdl_get_m_state(sdl) != LMI_ENABLED)
		goto outstate;
	if (p->sdl_flags & ~(SDL_TX_DIRECTION | SDL_RX_DIRECTION))
		goto badflag;
	if (!(p->sdl_flags & (SDL_TX_DIRECTION | SDL_RX_DIRECTION)))
		goto badflag;
	if (((sdl->flags ^ p->sdl_flags) & p->sdl_flags) == 0)
		goto badflag;
	return ch_connect_req(sdl->ch, q, mp, p->sdl_flags, 0);
      badflag:
	err = -EINVAL;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      fatalerr:
	err = LMI_FATALERR;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_ack(sdl, q, mp, SDL_CONNECT_REQ, err);
}

/**
 * sdl_disconnect_req: - process SDL_DISCONNECT_REQ primitive
 * @sdl: private structure
 * @q: active queue (write queue)
 * @mp: the SDL_DISCONNECT_REQ primitive
 */
static noinline __unlikely int
sdl_disconnect_req(struct sdl *sdl, queue_t *q, mblk_t *mp)
{
	sdl_disconnect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto tooshort;
	if (sdl_get_m_state(sdl) == LMI_UNUSABLE)
		goto fatalerr;
	if (sdl_get_m_state(sdl) != LMI_ENABLED)
		goto outstate;
	if (p->sdl_flags & ~(SDL_TX_DIRECTION | SDL_RX_DIRECTION))
		goto badflag;
	if (!(p->sdl_flags & (SDL_TX_DIRECTION | SDL_RX_DIRECTION)))
		goto badflag;
	if (((sdl->flags ^ ~p->sdl_flags) & p->sdl_flags) == 0)
		goto badflag;
	return ch_disconnect_req(sdl->ch, q, mp, p->sdl_flags, 0);
      badflag:
	err = -EINVAL;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      fatalerr:
	err = LMI_FATALERR;
	goto error;
      tooshort:
	err = LMI_TOOSHORT;
	goto error;
      error:
	return lmi_error_ack(sdl, q, mp, SDL_DISCONNECT_REQ, err);
}

static noinline __unlikely int
sdl_other_req(struct sdl *sdl, queue_t *q, mblk_t *mp)
{
	return lmi_error_ack(sdl, q, mp, *(lmi_long *)mp->b_rptr, LMI_NOTSUPP);
}

/*
 *  ========================================================================
 *
 *  RECEIVED PRIMITIVES
 *
 *  CH Provider -> CH User primitives.
 *
 *  ========================================================================
 */

/**
 * ch_info_ack: - process CH_INFO_ACK primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_INFO_ACK primitive
 */
static noinline __unlikely int
ch_info_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_info_ack *p = (typeof(p)) mp->b_rptr;
	struct sdl *sdl = SDL_PRIV(q);

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->ch_addr_length + p->ch_addr_offset)
		goto badprim;
	if (mp->b_wptr < mp->b_rptr + p->ch_parm_length + p->ch_parm_offset)
		goto badprim;
	switch (p->ch_style) {
	case CH_STYLE1:
		sdl->ppa_style = LMI_STYLE1;
		break;
	case CH_STYLE2:
		sdl->ppa_style = LMI_STYLE2;
		break;
	}
	ch_set_m_state(ch, p->ch_state);
	sdl->ppa_len = p->ch_addr_length;
	if (sdl->ppa_len > 48)
		sdl->ppa_len = 48;
	bcopy(mp->b_rptr + p->ch_addr_offset, sdl->ppa, sdl->ppa_len);
	if (p->ch_parm_length >= sizeof(struct CH_parms_circuit)) {
		struct CH_parms_circuit *cp = (typeof(cp)) (mp->b_rptr + p->ch_parm_offset);

		if (cp->cp_type == CH_PARMS_CIRCUIT) {
			sdl->sdl.config.ifrate = cp->cp_rate * cp->cp_sample_size;
			sdl->sdl.config.ifmode = SDL_MODE_NONE;
			sdl->sdl.config.ifgmode = SDL_GMODE_NONE;
			sdl->sdl.config.ifgcrc = SDL_GCRC_NONE;
			sdl->sdl.config.ifclock = SDL_CLOCK_NONE;
			sdl->sdl.config.ifcoding = SDL_CODING_NONE;
			sdl->sdl.config.ifframing = SDL_FRAMING_NONE;
			sdl->sdl.config.ifleads = 0x1f;
			sdl->sdl.config.ifalarms = 0;
			sdl->sdl.config.ifrxlevel = 0;
			sdl->sdl.config.iftxlevel = 0;
			sdl->sdl.config.ifsync = 0;
			switch (cp->cp_rate) {
			case CH_RATE_VARIABLE:
			case CH_RATE_8000:	/* DS0/DS0A */
				sdl->sdl.config.iftype = SDL_TYPE_NONE;
				if (cp->cp_sample_size == 7)
					sdl->sdl.config.iftype = SDL_TYPE_DS0A;
				if (cp->cp_sample_size == 8)
					sdl->sdl.config.iftype = SDL_TYPE_DS0;
				sdl->sdl.config.ifgtype = SDL_GTYPE_NONE;
				sdl->sdl.config.ifgrate = SDL_GRATE_NONE;
				break;
			case CH_RATE_11025:
			case CH_RATE_16000:
			case CH_RATE_22050:
			case CH_RATE_44100:
			case CH_RATE_90000:
				sdl->sdl.config.iftype = SDL_TYPE_NONE;
				sdl->sdl.config.ifgtype = SDL_GTYPE_NONE;
				sdl->sdl.config.ifgrate = SDL_GRATE_NONE;
				break;
			case CH_RATE_184000:	/* 23B */
				sdl->sdl.config.iftype = SDL_TYPE_NONE;
				sdl->sdl.config.ifgtype = SDL_GTYPE_T1;
				sdl->sdl.config.ifgrate = SDL_GRATE_T1;
				break;
			case CH_RATE_192000:	/* T1 */
				sdl->sdl.config.iftype = SDL_TYPE_T1;
				sdl->sdl.config.ifgtype = SDL_GTYPE_T1;
				sdl->sdl.config.ifgrate = SDL_GRATE_T1;
				break;
			case CH_RATE_240000:	/* 30B */
				sdl->sdl.config.iftype = SDL_TYPE_NONE;
				sdl->sdl.config.ifgtype = SDL_GTYPE_E1;
				sdl->sdl.config.ifgrate = SDL_GRATE_E1;
				break;
			case CH_RATE_248000:	/* E1 */
				sdl->sdl.config.iftype = SDL_TYPE_E1;
				sdl->sdl.config.ifgtype = SDL_GTYPE_E1;
				sdl->sdl.config.ifgrate = SDL_GRATE_E1;
				break;
			case CH_RATE_768000:	/* T2 */
				sdl->sdl.config.iftype = SDL_TYPE_T1;
				sdl->sdl.config.ifgtype = SDL_GTYPE_T2;
				sdl->sdl.config.ifgrate = SDL_GRATE_T2;
				break;
			case CH_RATE_992000:	/* E2 */
				sdl->sdl.config.iftype = SDL_TYPE_E1;
				sdl->sdl.config.ifgtype = SDL_GTYPE_E2;
				sdl->sdl.config.ifgrate = SDL_GRATE_E2;
				break;
			case CH_RATE_3968000:	/* E3 */
				sdl->sdl.config.iftype = SDL_TYPE_E1;
				sdl->sdl.config.ifgtype = SDL_GTYPE_E3;
				sdl->sdl.config.ifgrate = SDL_GRATE_E3;
				break;
			case CH_RATE_5376000:	/* T3 */
				sdl->sdl.config.iftype = SDL_TYPE_T1;
				sdl->sdl.config.ifgtype = SDL_GTYPE_T3;
				sdl->sdl.config.ifgrate = SDL_GRATE_T3;
				break;
			}
		}
	}
	freemsg(mp);
	return (0);
      badprim:
	mi_strlog(q, STRLOGER, SL_ERROR, "%s: invalid primitive on read queue", __FUNCTION__);
	return m_error(ch->sdl, q, mp, EFAULT);
}

/**
 * ch_optmgmt_ack: - process CH_OPTMGMT_ACK primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_OPTMGMT_ACK primitive
 */
static noinline __unlikely int
ch_optmgmt_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

/**
 * ch_ok_ack: - process CH_OK_ACK primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_OK_ACK primitive
 */
static noinline __unlikely int
ch_ok_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_ok_ack *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (p->ch_correct_prim) {
	case CH_ATTACH_REQ:
		ch_set_m_state(ch, CHS_ATTACHED);
		if (sdl_get_m_state(ch->sdl) == LMI_ATTACH_PENDING)
			return lmi_ok_ack(ch->sdl, q, mp, LMI_ATTACH_REQ);
		break;
	case CH_DETACH_REQ:
		ch_set_m_state(ch, CHS_DETACHED);
		if (sdl_get_m_state(ch->sdl) == LMI_DETACH_PENDING)
			return lmi_ok_ack(ch->sdl, q, mp, LMI_DETACH_REQ);
		break;
	case CH_ENABLE_REQ:
		ch_set_m_state(ch, CHS_WCON_EREQ);
		break;
	case CH_DISABLE_REQ:
		ch_set_m_state(ch, CHS_WCON_RREQ);
		break;
	case CH_CONNECT_REQ:
		ch_set_m_state(ch, CHS_WCON_CREQ);
		break;
	case CH_DISCONNECT_REQ:
		ch_set_m_state(ch, CHS_WCON_DREQ);
		break;
	default:
		switch (ch_get_m_state(ch)) {
		case CHS_WACK_AREQ:
			ch_set_m_state(ch, CHS_ATTACHED);
			if (sdl_get_m_state(ch->sdl) == LMI_ATTACH_PENDING)
				return lmi_ok_ack(ch->sdl, q, mp, LMI_ATTACH_REQ);
			break;
		case CHS_WACK_UREQ:
			ch_set_m_state(ch, CHS_DETACHED);
			if (sdl_get_m_state(ch->sdl) == LMI_DETACH_PENDING)
				return lmi_ok_ack(ch->sdl, q, mp, LMI_DETACH_REQ);
			break;
		case CHS_WACK_EREQ:
		case CHS_WACK_RREQ:
		case CHS_WACK_CREQ:
		case CHS_WACK_DREQ:
			ch_set_m_state(ch, p->ch_state);
			enableok(ch->q);
			qenable(ch->q);
			break;
		default:
			mi_strlog(q, STRLOGER, SL_ERROR, "CH_OK_ACK in wrong state.");
			break;
		}
		break;
	}
	freemsg(mp);
	return (0);
      badprim:
	mi_strlog(q, STRLOGER, SL_ERROR, "%s: invalid primitive on read queue", __FUNCTION__);
	return m_error(ch->sdl, q, mp, EFAULT);
}

/**
 * ch_error_ack: - process CH_ERROR_ACK primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_ERROR_ACK primitive
 */
static noinline __unlikely int
ch_error_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_error_ack *p = (typeof(p)) mp->b_rptr;
	sdl_long errno, prim;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	ch_set_m_state(ch, p->ch_state);
	/* translate the error number */
	switch (p->ch_error_type) {
	case CHSYSERR:
		errno = -p->ch_unix_error;
		break;
	case CHBADADDR:
		errno = LMI_BADADDRESS;
		break;
	case CHOUTSTATE:
		errno = LMI_OUTSTATE;
		break;
	case CHBADOPT:
		errno = -EOPNOTSUPP;
		break;
	case CHBADPARM:
		errno = -EINVAL;
		break;
	case CHBADPARMTYPE:
		errno = -EINVAL;
		break;
	case CHBADFLAG:
		errno = -EINVAL;
		break;
	case CHBADPRIM:
		errno = LMI_BADPRIM;
		break;
	case CHNOTSUPP:
		errno = LMI_NOTSUPP;
		break;
	case CHBADSLOT:
		errno = LMI_INITFAILED;
		break;
	default:
		errno = LMI_UNSPEC;
		break;
	}
	/* translate the primitive */
	switch (p->ch_error_primitive) {
	case CH_INFO_REQ:
		prim = LMI_INFO_REQ;
		break;
	case CH_OPTMGMT_REQ:
		prim = LMI_OPTMGMT_REQ;
		break;
	case CH_ATTACH_REQ:
		prim = LMI_ATTACH_REQ;
		break;
	case CH_ENABLE_REQ:
		prim = LMI_ENABLE_REQ;
		break;
	case CH_CONNECT_REQ:
		prim = SDL_CONNECT_REQ;
		break;
	case CH_DATA_REQ:
		prim = SDL_BITS_FOR_TRANSMISSION_REQ;
		break;
	case CH_DISCONNECT_REQ:
		prim = SDL_DISCONNECT_REQ;
		break;
	case CH_DISABLE_REQ:
		prim = LMI_DISABLE_REQ;
		break;
	case CH_DETACH_REQ:
		prim = LMI_DETACH_REQ;
		break;
	default:
		freemsg(mp);
		return (0);
	}
	return lmi_error_ack(ch->sdl, q, mp, prim, errno);
      badprim:
	mi_strlog(q, STRLOGER, SL_ERROR, "%s: invalid primitive on read queue", __FUNCTION__);
	return m_error(ch->sdl, q, mp, EFAULT);
}

/**
 * ch_enable_con: - process CH_ENABLE_CON primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_ENABLE_CON primitive
 */
static noinline __unlikely int
ch_enable_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_enable_con *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	ch_set_m_state(ch, CHS_ENABLED);
	if (sdl_get_m_state(ch->sdl) == LMI_ENABLE_PENDING)
		return lmi_enable_con(ch->sdl, q, mp);
	freemsg(mp);
	return (0);
      badprim:
	mi_strlog(q, STRLOGER, SL_ERROR, "%s: invalid primitive on read queue", __FUNCTION__);
	return m_error(ch->sdl, q, mp, EFAULT);
}

/**
 * ch_connect_con: - process CH_CONNECT_CON primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_CONNECT_CON primitive
 *
 * Connects are not confirmed in SDL.  However, we may be holding data on the
 * write queue awaiting connection.
 */
static noinline __unlikely int
ch_connect_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_connect_con *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	ch->flags |= p->ch_conn_flags;
	ch->sdl->flags |= p->ch_conn_flags;
	ch_set_m_state(ch, CHS_CONNECTED);
	freemsg(mp);
	enableok(ch->q);
	qenable(ch->q);
	return (0);
      badprim:
	mi_strlog(q, STRLOGER, SL_ERROR, "%s: invalid primitive on read queue", __FUNCTION__);
	return m_error(ch->sdl, q, mp, EFAULT);
}

/**
 * ch_data_ind: - process CH_DATA_IND primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_DATA_IND primitive
 */
static inline __hot int
ch_data_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_data_ind *p = (typeof(p)) mp->b_rptr;

	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto badprim;
	if (unlikely(!(ch->flags & CHF_RX_DIR)))
		goto discard;
	return sdl_received_bits_ind(ch->sdl, q, mp, mp->b_cont);
      discard:
	freemsg(mp);
	return (0);
      badprim:
	mi_strlog(q, STRLOGER, SL_ERROR, "%s: invalid primitive on read queue", __FUNCTION__);
	return m_error(ch->sdl, q, mp, EFAULT);
}

/**
 * ch_disconnect_ind: - process CH_DISCONNECT_IND primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_DISCONNECT_IND primitive
 */
static noinline __unlikely int
ch_disconnect_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disconnect_ind *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (sdl_get_m_state(ch->sdl)) {
	case LMI_ENABLED:
	case LMI_DISABLE_PENDING:
		return sdl_disconnect_ind(ch->sdl, q, mp, p->ch_conn_flags);
	}
	freemsg(mp);
	return (0);
      badprim:
	mi_strlog(q, STRLOGER, SL_ERROR, "%s: invalid primitive on read queue", __FUNCTION__);
	return m_error(ch->sdl, q, mp, EFAULT);
}

/**
 * ch_disconnect_con: - process CH_DISCONNECT_CON primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_DISCONNECT_CON primitive
 *
 * Diconnects are not confirmed in SDL.
 */
static noinline __unlikely int
ch_disconnect_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disconnect_con *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	ch->flags &= ~p->ch_conn_flags;
	ch->sdl->flags &= ~p->ch_conn_flags;
	if (!(ch->flags & CHF_BOTH_DIR))
		ch_set_m_state(ch, CHS_ENABLED);
	freemsg(mp);
	return (0);
      badprim:
	mi_strlog(q, STRLOGER, SL_ERROR, "%s: invalid primitive on read queue", __FUNCTION__);
	return m_error(ch->sdl, q, mp, EFAULT);
}

/**
 * ch_disable_ind: - process CH_DISABLE_IND primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_DISABLE_IND primitive
 */
static noinline __unlikely int
ch_disable_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disable_ind *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	ch_set_m_state(ch, CHS_ATTACHED);
	sdl_set_m_state(ch->sdl, LMI_UNUSABLE);
	return lmi_error_ind(ch->sdl, q, mp, LMI_DISC);
      badprim:
	mi_strlog(q, STRLOGER, SL_ERROR, "%s: invalid primitive on read queue", __FUNCTION__);
	return m_error(ch->sdl, q, mp, EFAULT);
}

/**
 * ch_disable_con: - process CH_DISABLE_CON primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_DISABLE_CON primitive
 */
static noinline __unlikely int
ch_disable_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disable_con *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	ch_set_m_state(ch, CHS_ATTACHED);
	if (sdl_get_m_state(ch->sdl) == LMI_DISABLE_PENDING)
		return lmi_disable_con(ch->sdl, q, mp);
	freemsg(mp);
	return (0);
      badprim:
	mi_strlog(q, STRLOGER, SL_ERROR, "%s: invalid primitive on read queue", __FUNCTION__);
	return m_error(ch->sdl, q, mp, EFAULT);
}

static noinline __unlikely int
ch_other_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	mi_strlog(q, STRLOGER, SL_ERROR, "%s: invalid primitive on read queue", __FUNCTION__);
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

static noinline fastcall __unlikely int
lmi_testoptions(struct sdl *sdl, struct lmi_option *arg)
{
	/* FIXME: actually test the argument */
	return (0);
}
static inline fastcall __unlikely int
lmi_testconfig(struct sdl *sdl, struct lmi_config *arg)
{
	/* FIXME: actually test the argument */
	return (0);
}
static noinline fastcall __unlikely int
lmi_setnotify(struct sdl *sdl, struct lmi_notify *arg)
{
	/* FIXME: actually test the argument */
	sdl->notify.events |= arg->events;
	return (0);
}
static noinline fastcall __unlikely int
lmi_clrnotify(struct sdl *sdl, struct lmi_notify *arg)
{
	/* FIXME: actually test the argument */
	sdl->notify.events &= ~arg->events;
	return (0);
}

/**
 * lmi_ioctl: - process LMI M_IOCTL message
 * @q: active queue (write queue)
 * @mp: the M_IOCTL mesage
 */
static noinline fastcall __unlikely int
lmi_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct sdl *sdl = SDL_PRIV(q);
	caddr_t priv;
	mblk_t *bp;
	int size = -1, err = 0;

	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);

	mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(%s)", sdl_iocname(ioc->ioc_cmd));

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(LMI_IOCGOPTIONS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdl->option), false)))
			goto enobufs;
		bcopy(&sdl->option, bp->b_rptr, sizeof(sdl->option));
		break;
	case _IOC_NR(LMI_IOCSOPTIONS):
		size = sizeof(sdl->option);
		break;
	case _IOC_NR(LMI_IOCGNOTIFY):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdl->notify), false)))
			goto enobufs;
		bcopy(&sdl->notify, bp->b_rptr, sizeof(sdl->notify));
		break;
	case _IOC_NR(LMI_IOCSNOTIFY):
		size = sizeof(sdl->notify);
		break;
	case _IOC_NR(LMI_IOCCNOTIFY):
		size = sizeof(sdl->notify);
		break;
	default:
		err = EOPNOTSUPP;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}

	mi_unlock(priv);

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

/**
 * lmi_copyin: - process LMI M_IOCDATA message
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 * @dp: copied in data
 */
static noinline fastcall __unlikely int
lmi_copyin(queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct sdl *sdl = SDL_PRIV(q);
	caddr_t priv;
	mblk_t *bp;
	int err = 0;

	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);

	mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(%s)", sdl_iocname(cp->cp_cmd));

	if (!(bp = mi_copyout_alloc(q, mp, NULL, dp->b_wptr - dp->b_rptr, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, dp->b_wptr - dp->b_rptr);

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(LMI_IOCSOPTIONS):
		if ((err = lmi_testoptions(sdl, (struct lmi_option *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdl->option, sizeof(sdl->option));
		break;
	case _IOC_NR(LMI_IOCSNOTIFY):
		if ((err = lmi_setnotify(sdl, (struct lmi_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdl->notify, sizeof(sdl->notify));
		break;
	case _IOC_NR(LMI_IOCCNOTIFY):
		if ((err = lmi_clrnotify(sdl, (struct lmi_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdl->notify, sizeof(sdl->notify));
		break;
	default:
		err = EPROTO;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	}

	mi_unlock(priv);

	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
}

/**
 * lmi_copyout: - process LMI M_IOCDATA message
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 */
static noinline fastcall __unlikely int
lmi_copyout(queue_t *q, mblk_t *mp)
{
	mi_copyout(q, mp);
	return (0);
}

static noinline fastcall __unlikely int
sdl_testoptions(struct sdl *sdl, struct lmi_option *arg)
{
	/* FIXME: actually test the argument */
	return (0);
}
static noinline fastcall __unlikely int
sdl_testconfig(struct sdl *sdl, struct sdl_config *arg)
{
	/* FIXME: actually test the argument */
	return (0);
}
static noinline fastcall __unlikely int
sdl_setnotify(struct sdl *sdl, struct sdl_notify *arg)
{
	/* FIXME: actually test the argument */
	sdl->sdl.notify.events |= arg->events;
	return (0);
}
static noinline fastcall __unlikely int
sdl_clrnotify(struct sdl *sdl, struct sdl_notify *arg)
{
	/* FIXME: actually test the argument */
	sdl->sdl.notify.events &= ~arg->events;
	return (0);
}

/**
 * sdl_ioctl: - process SDL M_IOCTL message
 * @q: active queue (write queue)
 * @mp: the M_IOCTL mesage
 */
static noinline fastcall __unlikely int
sdl_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct sdl *sdl = SDL_PRIV(q);
	caddr_t priv;
	mblk_t *bp;
	int size = -1, err = 0;

	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);

	mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCTL(%s)", sdl_iocname(ioc->ioc_cmd));

	switch (_IOC_NR(ioc->ioc_cmd)) {
	case _IOC_NR(SDL_IOCGOPTIONS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdl->option), false)))
			goto enobufs;
		bcopy(&sdl->option, bp->b_rptr, sizeof(sdl->option));
		break;
	case _IOC_NR(SDL_IOCSOPTIONS):
		size = sizeof(sdl->option);
		break;
	case _IOC_NR(SDL_IOCGCONFIG):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdl->sdl.config), false)))
			goto enobufs;
		bcopy(&sdl->sdl.config, bp->b_rptr, sizeof(sdl->sdl.config));
		break;
	case _IOC_NR(SDL_IOCSCONFIG):
		size = sizeof(sdl->sdl.config);
		break;
	case _IOC_NR(SDL_IOCTCONFIG):
		size = sizeof(sdl->sdl.config);
		break;
	case _IOC_NR(SDL_IOCCCONFIG):
		size = sizeof(sdl->sdl.config);
		break;
	case _IOC_NR(SDL_IOCGSTATEM):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdl->sdl.statem), false)))
			goto enobufs;
		bcopy(&sdl->sdl.statem, bp->b_rptr, sizeof(sdl->sdl.statem));
		break;
	case _IOC_NR(SDL_IOCCMRESET):
		/* FIXME: reset the state machine. */
		break;
	case _IOC_NR(SDL_IOCGSTATSP):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdl->sdl.statsp), false)))
			goto enobufs;
		bcopy(&sdl->sdl.statsp, bp->b_rptr, sizeof(sdl->sdl.statsp));
		break;
	case _IOC_NR(SDL_IOCSSTATSP):
		size = sizeof(sdl->sdl.statsp);
		break;
	case _IOC_NR(SDL_IOCGSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdl->sdl.stats), false)))
			goto enobufs;
		bcopy(&sdl->sdl.stats, bp->b_rptr, sizeof(sdl->sdl.stats));
		break;
	case _IOC_NR(SDL_IOCCSTATS):
		size = sizeof(sdl->sdl.stats);
		break;
	case _IOC_NR(SDL_IOCGNOTIFY):
		if (!(bp = mi_copyout_alloc(q, mp, NULL, sizeof(sdl->sdl.notify), false)))
			goto enobufs;
		bcopy(&sdl->sdl.notify, bp->b_rptr, sizeof(sdl->sdl.notify));
		break;
	case _IOC_NR(SDL_IOCSNOTIFY):
		size = sizeof(sdl->sdl.notify);
		break;
	case _IOC_NR(SDL_IOCCNOTIFY):
		size = sizeof(sdl->sdl.notify);
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

	mi_unlock(priv);
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

static noinline fastcall __unlikely int
sdl_copyin_pass(queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct sdl *sdl = SDL_PRIV(q);
	caddr_t priv;
	int size = -1, err = 0;
	mblk_t *bp;

	struct sdl_pass *arg = (typeof(arg)) dp->b_rptr;

	if ((priv = mi_trylock(q)) == 0)
		return (-EDEADLK);

	if (arg->index != 0)
		goto einval;

	mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCPASS(%s))", sdl_iocname(arg->cmd));

	switch (_IOC_NR(arg->cmd)) {
	case _IOC_NR(SDL_IOCGOPTIONS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL,
					    sizeof(*arg) + sizeof(sdl->option), false)))
			goto enobufs;
		bcopy(arg, bp->b_rptr, sizeof(*arg));
		bcopy(&sdl->option, bp->b_rptr + sizeof(*arg), sizeof(sdl->option));
		break;
	case _IOC_NR(SDL_IOCSOPTIONS):
		size = sizeof(*arg) + sizeof(sdl->option);
		break;
	case _IOC_NR(SDL_IOCGCONFIG):
		if (!(bp = mi_copyout_alloc(q, mp, NULL,
					    sizeof(*arg) + sizeof(sdl->sdl.config), false)))
			goto enobufs;
		bcopy(arg, bp->b_rptr, sizeof(*arg));
		bcopy(&sdl->sdl.config, bp->b_rptr + sizeof(*arg), sizeof(sdl->sdl.config));
		break;
	case _IOC_NR(SDL_IOCSCONFIG):
		size = sizeof(*arg) + sizeof(sdl->sdl.config);
		break;
	case _IOC_NR(SDL_IOCTCONFIG):
		size = sizeof(*arg) + sizeof(sdl->sdl.config);
		break;
	case _IOC_NR(SDL_IOCCCONFIG):
		size = sizeof(*arg) + sizeof(sdl->sdl.config);
		break;
	case _IOC_NR(SDL_IOCGSTATEM):
		if (!(bp = mi_copyout_alloc(q, mp, NULL,
					    sizeof(*arg) + sizeof(sdl->sdl.statem), false)))
			goto enobufs;
		bcopy(arg, bp->b_rptr, sizeof(*arg));
		bcopy(&sdl->sdl.statem, bp->b_rptr + sizeof(*arg), sizeof(sdl->sdl.statem));
		break;
	case _IOC_NR(SDL_IOCCMRESET):
		/* FIXME: perform reset */
		break;
	case _IOC_NR(SDL_IOCGSTATSP):
		if (!(bp = mi_copyout_alloc(q, mp, NULL,
					    sizeof(*arg) + sizeof(sdl->sdl.statsp), false)))
			goto enobufs;
		bcopy(arg, bp->b_rptr, sizeof(*arg));
		bcopy(&sdl->sdl.statsp, bp->b_rptr + sizeof(*arg), sizeof(sdl->sdl.statsp));
		break;
	case _IOC_NR(SDL_IOCSSTATSP):
		break;
	case _IOC_NR(SDL_IOCGSTATS):
		if (!(bp = mi_copyout_alloc(q, mp, NULL,
					    sizeof(*arg) + sizeof(sdl->sdl.stats), false)))
			goto enobufs;
		bcopy(arg, bp->b_rptr, sizeof(*arg));
		bcopy(&sdl->sdl.stats, bp->b_rptr + sizeof(*arg), sizeof(sdl->sdl.stats));
		break;
	case _IOC_NR(SDL_IOCCSTATS):
		break;
	case _IOC_NR(SDL_IOCGNOTIFY):
		if (!(bp = mi_copyout_alloc(q, mp, NULL,
					    sizeof(*arg) + sizeof(sdl->sdl.notify), false)))
			goto enobufs;
		bcopy(arg, bp->b_rptr, sizeof(*arg));
		bcopy(&sdl->sdl.notify, bp->b_rptr + sizeof(*arg), sizeof(sdl->sdl.notify));
		break;
	case _IOC_NR(SDL_IOCSNOTIFY):
		size = sizeof(*arg) + sizeof(sdl->sdl.notify);
		break;
	case _IOC_NR(SDL_IOCCNOTIFY):
		size = sizeof(*arg) + sizeof(sdl->sdl.notify);
		break;
	case _IOC_NR(SDL_IOCCPASS):
		err = EINVAL;
		break;
	default:
		err = EPROTO;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	      einval:
		err = EINVAL;
		break;
	}
	mi_unlock(priv);
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

/**
 * sdl_copyin: - process SDL M_IOCDATA message
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 * @dp: copied in data
 */
static noinline fastcall __unlikely int
sdl_copyin(queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct sdl *sdl = SDL_PRIV(q);
	caddr_t priv;
	int err = 0;
	mblk_t *bp;

	if (_IOC_NR(cp->cp_cmd) == _IOC_NR(SDL_IOCCPASS))
		return sdl_copyin_pass(q, mp, dp);

	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);

	if (!(bp = mi_copyout_alloc(q, mp, NULL, dp->b_wptr - dp->b_rptr, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, dp->b_wptr - dp->b_rptr);

	mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(%s)", sdl_iocname(cp->cp_cmd));

	switch (_IOC_NR(cp->cp_cmd)) {
	case _IOC_NR(SDL_IOCSOPTIONS):
		if ((err = sdl_testoptions(sdl, (struct lmi_option *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdl->option, sizeof(sdl->option));
		break;
	case _IOC_NR(SDL_IOCSCONFIG):
		if ((err = sdl_testconfig(sdl, (struct sdl_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdl->sdl.config, sizeof(sdl->sdl.config));
		break;
	case _IOC_NR(SDL_IOCTCONFIG):
		err = sdl_testconfig(sdl, (struct sdl_config *) bp->b_rptr);
		break;
	case _IOC_NR(SDL_IOCCCONFIG):
		if ((err = sdl_testconfig(sdl, (struct sdl_config *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdl->sdl.config, sizeof(sdl->sdl.config));
		break;
	case _IOC_NR(SDL_IOCSSTATSP):
		bcopy(bp->b_rptr, &sdl->sdl.statsp, sizeof(sdl->sdl.statsp));
		break;
	case _IOC_NR(SDL_IOCSNOTIFY):
		if ((err = sdl_setnotify(sdl, (struct sdl_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdl->sdl.notify, sizeof(sdl->sdl.notify));
		break;
	case _IOC_NR(SDL_IOCCNOTIFY):
		if ((err = sdl_clrnotify(sdl, (struct sdl_notify *) bp->b_rptr)) == 0)
			bcopy(bp->b_rptr, &sdl->sdl.notify, sizeof(sdl->sdl.notify));
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
	mi_unlock(priv);
	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
}

static noinline fastcall __unlikely int
sdl_copyin2_pass(queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;
	struct sdl *sdl = SDL_PRIV(q);
	caddr_t priv;
	int err = 0;
	mblk_t *bp;
	struct sdl_pass *arg = (typeof(arg)) dp->b_rptr;

	if ((priv = mi_trylock(q)) == NULL)
		return (-EDEADLK);

	if (arg->index != 0)
		goto einval;

	mi_strlog(q, STRLOGRX, SL_TRACE, "-> M_IOCDATA(SDL_IOCPASS(%s))", sdl_iocname(arg->cmd));

	if (!(bp = mi_copyout_alloc(q, mp, NULL, dp->b_wptr - dp->b_rptr, false)))
		goto enobufs;
	bcopy(dp->b_rptr, bp->b_rptr, dp->b_wptr - dp->b_rptr);
	arg = (typeof(arg)) bp->b_rptr;

	switch (_IOC_NR(arg->cmd)) {
	case _IOC_NR(SDL_IOCSOPTIONS):
		if ((err = sdl_testoptions(sdl, (struct lmi_option *) (arg + 1))) == 0)
			bcopy((arg + 1), &sdl->option, sizeof(sdl->option));
		break;
	case _IOC_NR(SDL_IOCSCONFIG):
		if ((err = sdl_testconfig(sdl, (struct sdl_config *) (arg + 1))) == 0)
			bcopy((arg + 1), &sdl->sdl.config, sizeof(sdl->sdl.config));
		break;
	case _IOC_NR(SDL_IOCTCONFIG):
		err = sdl_testconfig(sdl, (struct sdl_config *) (arg + 1));
		break;
	case _IOC_NR(SDL_IOCCCONFIG):
		if ((err = sdl_testconfig(sdl, (struct sdl_config *) (arg + 1))) == 0)
			bcopy((arg + 1), &sdl->sdl.config, sizeof(sdl->sdl.config));
		break;
	case _IOC_NR(SDL_IOCSSTATSP):
		bcopy((arg + 1), &sdl->sdl.statsp, sizeof(sdl->sdl.statsp));
		break;
	case _IOC_NR(SDL_IOCSNOTIFY):
		if ((err = sdl_setnotify(sdl, (struct sdl_notify *) (arg + 1))) == 0)
			bcopy((arg + 1), &sdl->sdl.notify, sizeof(sdl->sdl.notify));
		break;
	case _IOC_NR(SDL_IOCCNOTIFY):
		if ((err = sdl_clrnotify(sdl, (struct sdl_notify *) (arg + 1))) == 0)
			bcopy((arg + 1), &sdl->sdl.notify, sizeof(sdl->sdl.notify));
		break;
	default:
		err = EPROTO;
		break;
	      enobufs:
		err = ENOBUFS;
		break;
	      einval:
		err = EINVAL;
		break;
	}

	mi_unlock(priv);

	if (err < 0)
		return (err);
	if (err > 0)
		mi_copy_done(q, mp, err);
	if (err == 0)
		mi_copyout(q, mp);
	return (0);
}

/**
 * sdl_copyin2: - process SDL M_IOCDATA message
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 * @dp: copied in data
 */
static noinline fastcall __unlikely int
sdl_copyin2(queue_t *q, mblk_t *mp, mblk_t *dp)
{
	struct copyresp *cp = (typeof(cp)) mp->b_rptr;

	if (_IOC_NR(cp->cp_cmd) == _IOC_NR(SDL_IOCCPASS))
		return sdl_copyin2_pass(q, mp, dp);

	mi_copy_done(q, mp, EPROTO);
	return (0);
}

/**
 * sdl_copyout: - process SDL M_IOCDATA message
 * @q: active queue (write queue)
 * @mp: the M_IOCDATA message
 */
static noinline fastcall __unlikely int
sdl_copyout(queue_t *q, mblk_t *mp)
{
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

static inline fastcall int
sdl_m_data(queue_t *q, mblk_t *mp)
{
	struct sdl *sdl = SDL_PRIV(q);
	caddr_t priv;
	int err;

	if (unlikely((priv = mi_trylock(q)) == NULL))
		return (-EDEADLK);
	if (unlikely(!bcanputnext(sdl->ch->q, mp->b_band))) {
		mi_unlock(priv);
		return (-EBUSY);
	}
	if (unlikely(sdl_get_m_state(sdl) == LMI_UNUSABLE))
		goto fatalerr;
	if (unlikely(sdl_get_m_state(sdl) != LMI_ENABLED))
		goto outstate;
	if (unlikely(ch_get_m_state(sdl->ch) != CHS_CONNECTED)) {
		noenable(q);
		mi_unlock(priv);
		return (-EAGAIN);
	}
	if (unlikely(!(sdl->flags & SDL_TX_DIRECTION)))
		goto disc;
	putnext(sdl->ch->q, mp);
	mi_unlock(priv);
	return (0);
      fatalerr:
	err = LMI_FATALERR;
	goto error;
      outstate:
	err = LMI_OUTSTATE;
	goto error;
      disc:
	err = LMI_DISC;
	goto error;
      error:
	mi_unlock(priv);
	return lmi_error_ind(sdl, q, mp, err);
}

static noinline fastcall __unlikely int
sdl_m_data_slow(queue_t *q, mblk_t *mp)
{
	return sdl_m_data(q, mp);
}

static int
sdl_m_proto(queue_t *q, mblk_t *mp)
{
	struct sdl *sdl = SDL_PRIV(q);
	caddr_t priv;
	sdl_ulong prim;
	int err = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim))
		goto discard;

	if ((priv = mi_trylock(q)) == NULL)
		goto edeadlk;

	sdl_save_m_state(sdl);
	ch_save_m_state(sdl->ch);

	prim = *(sdl_ulong *) mp->b_rptr;

	if (prim == SDL_BITS_FOR_TRANSMISSION_REQ)
		mi_strlog(q, STRLOGDA, SL_TRACE, "<- SDL_BITS_FOR_TRANSMISSION_REQ");
	else
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- %s", sdl_primname(prim));

	switch (prim) {
	case LMI_INFO_REQ:
		err = lmi_info_req(sdl, q, mp);
		break;
	case LMI_ATTACH_REQ:
		err = lmi_attach_req(sdl, q, mp);
		break;
	case LMI_DETACH_REQ:
		err = lmi_detach_req(sdl, q, mp);
		break;
	case LMI_ENABLE_REQ:
		err = lmi_enable_req(sdl, q, mp);
		break;
	case LMI_DISABLE_REQ:
		err = lmi_disable_req(sdl, q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		err = lmi_optmgmt_req(sdl, q, mp);
		break;
	case SDL_BITS_FOR_TRANSMISSION_REQ:
		err = sdl_bits_for_transmission_req(sdl, q, mp);
		break;
	case SDL_CONNECT_REQ:
		err = sdl_connect_req(sdl, q, mp);
		break;
	case SDL_DISCONNECT_REQ:
		err = sdl_disconnect_req(sdl, q, mp);
		break;
	default:
		err = sdl_other_req(sdl, q, mp);
		break;
	}
	if (err) {
		ch_restore_m_state(sdl->ch);
		sdl_restore_m_state(sdl);
	}
      unlock_done:
	mi_unlock(priv);
      done:
	return (err);
      discard:
	mi_strlog(q, STRLOGER, SL_ERROR, "M_(PC)PROTO block too short.");
	freemsg(mp);
	goto done;
      edeadlk:
	mi_strlog(q, STRLOGER, SL_TRACE, "Hit locks.");
	err = -EDEADLK;
	goto unlock_done;
}

static noinline fastcall __unlikely int
sdl_m_flush(queue_t *q, mblk_t *mp)
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

static noinline fastcall __unlikely int
sdl_m_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case LMI_IOC_MAGIC:
		return lmi_ioctl(q, mp);
	case SDL_IOC_MAGIC:
		return sdl_ioctl(q, mp);
	default:
		if (bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

static noinline fastcall __unlikely int
sdl_m_iocdata(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *dp;

	switch (_IOC_TYPE(ioc->ioc_cmd)) {
	case LMI_IOC_MAGIC:
		switch (mi_copy_state(q, mp, &dp)) {
		case MI_COPY_CASE(MI_COPY_IN, 1):
			return lmi_copyin(q, mp, dp);
		case MI_COPY_CASE(MI_COPY_OUT, 1):
			return lmi_copyout(q, mp);
		default:
			mi_copy_done(q, mp, EPROTO);
		case -1:
			break;
		}
		break;
	case SDL_IOC_MAGIC:
		switch (mi_copy_state(q, mp, &dp)) {
		case MI_COPY_CASE(MI_COPY_IN, 1):
			return sdl_copyin(q, mp, dp);
		case MI_COPY_CASE(MI_COPY_IN, 2):
			return sdl_copyin2(q, mp, dp);
		case MI_COPY_CASE(MI_COPY_OUT, 1):
			return sdl_copyout(q, mp);
		default:
			mi_copy_done(q, mp, EPROTO);
		case -1:
			break;
		}
		break;
	default:
		putnext(q, mp);
		break;
	}
	return (0);
}

static inline fastcall int
sdl_m_other(queue_t *q, mblk_t *mp)
{
	if (likely(mp->b_datap->db_type >= QPCTL || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static noinline fastcall int
sdl_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sdl_m_data_slow(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sdl_m_proto(q, mp);
	case M_FLUSH:
		return sdl_m_flush(q, mp);
	case M_IOCTL:
		return sdl_m_ioctl(q, mp);
	case M_IOCDATA:
		return sdl_m_iocdata(q, mp);
	default:
		return sdl_m_other(q, mp);
	}
}

static inline fastcall int
ch_m_data(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	caddr_t priv;
	int err = 0;

	if (unlikely((priv = mi_trylock(q)) == NULL))
		return (-EDEADLK);

	if (unlikely(!(ch->flags & CHF_RX_DIR))) {
		mi_unlock(priv);
		freemsg(mp);
		return (0);
	}
	if (unlikely(!bcanputnext(ch->sdl->q, mp->b_band))) {
		mi_unlock(priv);
		return (-EBUSY);
	}
	putnext(ch->sdl->q, mp);
	mi_unlock(priv);
	return (err);
}

static noinline fastcall __unlikely int
ch_m_data_slow(queue_t *q, mblk_t *mp)
{
	return ch_m_data(q, mp);
}

static int
ch_m_proto(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	caddr_t priv;
	ch_ulong prim;
	int err = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(prim))
		goto discard;

	if ((priv = mi_trylock(q)) == NULL)
		goto edeadlk;

	ch_save_m_state(ch);
	sdl_save_m_state(ch->sdl);

	prim = *(ch_ulong *) mp->b_rptr;

	if (prim == CH_DATA_IND)
		mi_strlog(q, STRLOGDA, SL_TRACE, "<- CH_DATA_IND");
	else
		mi_strlog(q, STRLOGRX, SL_TRACE, "<- %s", ch_primname(prim));

	switch (prim) {
	case CH_INFO_ACK:
		err = ch_info_ack(ch, q, mp);
		break;
	case CH_OPTMGMT_ACK:
		err = ch_optmgmt_ack(ch, q, mp);
		break;
	case CH_OK_ACK:
		err = ch_ok_ack(ch, q, mp);
		break;
	case CH_ERROR_ACK:
		err = ch_error_ack(ch, q, mp);
		break;
	case CH_ENABLE_CON:
		err = ch_enable_con(ch, q, mp);
		break;
	case CH_CONNECT_CON:
		err = ch_connect_con(ch, q, mp);
		break;
	case CH_DATA_IND:
		err = ch_data_ind(ch, q, mp);
		break;
	case CH_DISCONNECT_IND:
		err = ch_disconnect_ind(ch, q, mp);
		break;
	case CH_DISCONNECT_CON:
		err = ch_disconnect_con(ch, q, mp);
		break;
	case CH_DISABLE_IND:
		err = ch_disable_ind(ch, q, mp);
		break;
	case CH_DISABLE_CON:
		err = ch_disable_con(ch, q, mp);
		break;
	default:
		err = ch_other_ind(ch, q, mp);
		break;
	}
	if (err) {
		sdl_restore_m_state(ch->sdl);
		ch_restore_m_state(ch);
	}
      unlock_done:
	mi_unlock(priv);
      done:
	return (err);
      discard:
	mi_strlog(q, STRLOGER, SL_ERROR, "M_(PC)PROTO block too short.");
	freemsg(mp);
	goto done;
      edeadlk:
	mi_strlog(q, STRLOGER, SL_TRACE, "Hit locks.");
	err = -EDEADLK;
	goto unlock_done;
}

static noinline fastcall __unlikely int
ch_m_flush(queue_t *q, mblk_t *mp)
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

static inline fastcall int
ch_m_other(queue_t *q, mblk_t *mp)
{
	if (likely(mp->b_datap->db_type >= QPCTL || bcanputnext(q, mp->b_band))) {
		putnext(q, mp);
		return (0);
	}
	return (-EBUSY);
}

static noinline fastcall int
ch_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return ch_m_data_slow(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return ch_m_proto(q, mp);
	case M_FLUSH:
		return ch_m_flush(q, mp);
	default:
		return ch_m_other(q, mp);
	}
}

/*
 *  =========================================================================
 *
 *  PUT AND SERVICE PROCEDURES
 *
 *  =========================================================================
 */

/**
 * sdl_msg: - hand SDL message
 * @q: active queue (write queue)
 * @mp: the message
 */
static inline fastcall int
sdl_msg(queue_t *q, mblk_t *mp)
{
	if (likely(mp->b_datap->db_type == M_DATA))
		return sdl_m_data(q, mp);
	return sdl_msg_slow(q, mp);
}

/**
 * sdl_wput: - canonical put procedure
 * @q: active queue (write queue)
 * @mp: message to put
 */
static streamscall __hot_write int
sdl_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(mp->b_datap->db_type)
	     && (q->q_first || (q->q_flag & QSVCBUSY))) || sdl_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * sdl_wsrv: - canonical service procedure
 * @q: active queue (write queue)
 */
static streamscall __hot_out int
sdl_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (sdl_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

/**
 * ch_msg: - hand CH message
 * @q: active queue (read queue)
 * @mp: the message
 */
static inline fastcall int
ch_msg(queue_t *q, mblk_t *mp)
{
	if (likely(mp->b_datap->db_type == M_DATA))
		return ch_m_data(q, mp);
	return ch_msg_slow(q, mp);
}

/**
 * sdl_rput: - canonical put procedure
 * @q: active queue (read queue)
 * @mp: message to put
 */
static streamscall __hot_in int
sdl_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(mp->b_datap->db_type)
	     && (q->q_first || (q->q_flag & QSVCBUSY))) || ch_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * sdl_rsrv: - canonical service procedure
 * @q: active queue (read queue)
 */
static streamscall __hot_read int
sdl_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (ch_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  OPEN AND CLOSE
 *
 *  =========================================================================
 */

static caddr_t sdl_opens = NULL;

static streamscall int
sdl_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct sdl_pair *sp;
	int err;

	struct CH_info_req *p;
	mblk_t *mp;

	if (q->q_ptr)
		return (0);	/* already open */
	if (!(mp = allocb(sizeof(*p), BPRI_WAITOK)))
		return (ENOBUFS);
	if ((err = mi_open_comm(&sdl_opens, sizeof(*sp), q, devp, oflags, sflag, crp)))
		return (err);
	sp = (struct sdl_pair *) q->q_ptr;
	bzero(sp, sizeof(*sp));

	/* initialize the structure */
	sp->r_priv.pair = sp;
	sp->r_priv.ch = &sp->w_priv;
	sp->r_priv.q = q;
	sp->r_priv.max_sdu = 8 * 31;
	sp->r_priv.min_sdu = 8;
	sp->r_priv.header_len = 0;
	sp->r_priv.ppa_style = LMI_STYLE1;
	sp->r_priv.flags = 0;
	sp->r_priv.m_state = LMI_UNUSABLE;
	sp->r_priv.m_oldstate = LMI_UNUSABLE;
	sp->r_priv.m_chkstate = LMI_UNUSABLE;
	sp->r_priv.option.pvar = SS7_PVAR_ITUT_00;
	sp->r_priv.option.popt = 0;
	sp->r_priv.notify.events = 0;
	sp->r_priv.sdl.notify.events = 0;
	sp->r_priv.sdl.config.ifname = NULL;
	sp->r_priv.sdl.config.iftype = SDL_TYPE_NONE;
	sp->r_priv.sdl.config.ifrate = SDL_RATE_NONE;
	sp->r_priv.sdl.config.ifgtype = SDL_GTYPE_NONE;
	sp->r_priv.sdl.config.ifgrate = SDL_GRATE_NONE;
	sp->r_priv.sdl.config.ifmode = SDL_MODE_NONE;
	sp->r_priv.sdl.config.ifgmode = SDL_GMODE_NONE;
	sp->r_priv.sdl.config.ifgcrc = SDL_GCRC_NONE;
	sp->r_priv.sdl.config.ifclock = SDL_CLOCK_NONE;
	sp->r_priv.sdl.config.ifcoding = SDL_CODING_NONE;
	sp->r_priv.sdl.config.ifframing = SDL_FRAMING_NONE;
	sp->r_priv.sdl.config.ifleads = 0x1f;
	sp->r_priv.sdl.config.ifalarms = 0;
	sp->r_priv.sdl.config.ifrxlevel = 0;
	sp->r_priv.sdl.config.iftxlevel = 0;
	sp->r_priv.sdl.config.ifsync = 0;

	/* FIXME: etc */

	sp->w_priv.pair = sp;
	sp->w_priv.sdl = &sp->r_priv;
	sp->w_priv.q = WR(q);
	sp->w_priv.flags = 0;
	sp->w_priv.m_state = CHS_UNINIT;
	sp->w_priv.m_oldstate = CHS_UNINIT;
	sp->w_priv.m_chkstate = CHS_UNINIT;
	sp->w_priv.ch.notify.events = 0;
	sp->w_priv.ch.config.type = 0;
	sp->w_priv.ch.config.encoding = CH_ENCODING_NONE;
	sp->w_priv.ch.config.block_size = 8;
	sp->w_priv.ch.config.samples = 8;
	sp->w_priv.ch.config.rate = 8000;
	sp->w_priv.ch.config.tx_channels = 1;
	sp->w_priv.ch.config.rx_channels = 1;
	sp->w_priv.ch.config.opt_flags = 0;
	sp->w_priv.ch.statem.state = CHS_UNINIT;
	sp->w_priv.ch.statem.flags = 0;

	/* FIXME: etc */

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->ch_primitive = CH_INFO_REQ;
	mp->b_wptr += sizeof(*p);

	qprocson(q);

	qreply(q, mp);

	return (0);
}

static streamscall int
sdl_qclose(queue_t *q, int oflags, cred_t *crp)
{
	qprocsoff(q);
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

#ifdef LINUX

unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for SDL module. (0 for allocation.)");

STATIC struct qinit sdl_rinit = {
	.qi_putp = sdl_rput,		/* Read put (message from below) */
	.qi_srvp = sdl_rsrv,		/* Read queue service */
	.qi_qopen = sdl_qopen,		/* Each open */
	.qi_qclose = sdl_qclose,	/* Last close */
	.qi_minfo = &sdl_minfo,		/* Information */
	.qi_mstat = &sdl_rstat,		/* Statistics */
};

STATIC struct qinit sdl_winit = {
	.qi_putp = sdl_wput,		/* Write put (message from above) */
	.qi_srvp = sdl_wsrv,		/* Write queue service */
	.qi_minfo = &sdl_minfo,		/* Information */
	.qi_mstat = &sdl_wstat,		/* Statistics */
};

STATIC struct streamtab sdlinfo = {
	.st_rdinit = &sdl_rinit,	/* Upper read queue */
	.st_wrinit = &sdl_winit,	/* Upper write queue */
};

#ifdef LIS
#define fmodsw _fmodsw
#endif				/* LIS */

static struct fmodsw sdl_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sdlinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static __init int
sdlinit(void)
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

static __exit void
sdlexit(void)
{
	int err;

	if ((err = unregister_strmod(&sdl_fmod)) < 0)
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	return;
}

module_init(sdlinit);
module_exit(sdlexit);

#endif				/* LINUX */
