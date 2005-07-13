/*****************************************************************************

 @(#) $RCSfile: sdl.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2005/07/13 12:01:38 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

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

 Last Modified $Date: 2005/07/13 12:01:38 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sdl.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2005/07/13 12:01:38 $"

static char const ident[] =
    "$RCSfile: sdl.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2005/07/13 12:01:38 $";

/*
 *  This is an SDL (Signalling Data Link) kernel module which provides the
 *  capabilities of the SDL to any pipe.  Its principal purpose is testing.
 */
#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>

#define SDL_DESCRIP	"SS7/SDL: (Signalling Data Link) STREAMS MODULE."
#define SDL_REVISION	"OpenSS7 $RCSfile: sdl.c,v $ $Name:  $($Revision: 0.9.2.10 $) $Date: 2005/07/13 12:01:38 $"
#define SDL_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define SDL_DEVICE	"Supports STREAMS pipes."
#define SDL_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SDL_LICENSE	"GPL"
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
MODULE_ALIAS("streams-sdl");
#endif
#endif				/* LINUX */

#ifdef LFS
#define SDL_MOD_ID	CONFIG_STREAMS_SDL_MODID
#define SDL_MOD_NAME	CONFIG_STREAMS_SDL_NAME
#endif

/*
 *  =======================================================================
 *
 *  STREAMS Definitions
 *
 *  =======================================================================
 */

#define MOD_ID		SDL_MOD_ID
#define MOD_NAME	SDL_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SDL_BANNER
#else				/* MODULE */
#define MOD_BANNER	SDL_SPLASH
#endif				/* MODULE */

STATIC struct module_info sdl_rinfo = {
	mi_idnum:MOD_ID,		/* Module ID number */
	mi_idname:MOD_NAME,		/* Module name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:128,			/* Max packet size accepted */
	mi_hiwat:1,			/* Hi water mark */
	mi_lowat:0,			/* Lo water mark */
};

STATIC struct module_info sdl_winfo = {
	mi_idnum:MOD_ID,		/* Module ID number */
	mi_idname:MOD_NAME,		/* Module name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:280,			/* Max packet size accepted */
	mi_hiwat:1,			/* Hi water mark */
	mi_lowat:0			/* Lo water mark */
};

STATIC int sdl_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int sdl_close(queue_t *, int, cred_t *);

STATIC struct qinit sdl_rinit = {
	qi_putp:ss7_oput,		/* Read put (message from below) */
	qi_qopen:sdl_open,		/* Each open */
	qi_qclose:sdl_close,		/* Last close */
	qi_minfo:&sdl_rinfo,		/* Information */
};

STATIC struct qinit sdl_winit = {
	qi_putp:ss7_iput,		/* Write put (message from above) */
	qi_minfo:&sdl_winfo,		/* Information */
};

STATIC struct streamtab sdlinfo = {
	st_rdinit:&sdl_rinit,		/* Upper read queue */
	st_wrinit:&sdl_winit,		/* Upper write queue */
};

/*
 *  ========================================================================
 *
 *  Private structure
 *
 *  ========================================================================
 */
typedef struct sdl {
	STR_DECLARATION (struct sdl);	/* stream declaration */
	unsigned long timestamp;	/* next tick timestamp */
	unsigned long bytecount;	/* bytes send within tick */
	unsigned long tickbytes;	/* bytes per tick */
	lmi_option_t option;		/* LMI protocol and variant options */
	sdl_timers_t timers;		/* SDL protocol timers */
	sdl_statem_t statem;		/* SDL state machine variables */
	sdl_config_t config;		/* SDL configuration options */
	sdl_notify_t notify;		/* SDL notification options */
	sdl_stats_t stats;		/* SDL statistics */
	sdl_stats_t stamp;		/* SDL statistics timestamps */
	lmi_sta_t statsp;		/* SDL statistics periods */
} sdl_t;
#define SDL_PRIV(__q) ((struct sdl *)(__q)->q_ptr)

struct sdl *sdl_opens = NULL;

STATIC struct sdl *sdl_alloc_priv(queue_t *, struct sdl **, dev_t *, cred_t *);
STATIC struct sdl *sdl_get(struct sdl *);
STATIC void sdl_put(struct sdl *);
STATIC void sdl_free_priv(queue_t *);

struct lmi_option lmi_default = {
	pvar:SS7_PVAR_ITUT_96,
	popt:0,
};

struct sdl_config sdl_default = {
	ifflags:0,
	iftype:SDL_TYPE_NONE,
	ifrate:64000,
	ifgtype:SDL_GTYPE_NONE,
	ifgrate:0,
	ifmode:SDL_MODE_PEER,
	ifgmode:SDL_MODE_NONE,
	ifgcrc:SDL_GCRC_NONE,
	ifclock:SDL_CLOCK_SHAPER,
	ifcoding:SDL_CODING_NONE,
	ifframing:SDL_FRAMING_NONE,
	ifleads:0,
	ifalarms:0,
	ifrxlevel:0,
	iftxlevel:0,
	ifsync:0,
	ifsyncsrc:{0, 0, 0, 0},
};

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
 *  Primitives sent upstream
 *
 *  ------------------------------------------------------------------------
 */
/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC INLINE int
m_error(queue_t *q, struct sdl *s, int err)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		s->i_state = LMI_UNUSABLE;
		printd(("%s: %p: <- M_ERROR\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  M_HANGUP
 *  -----------------------------------
 */
STATIC INLINE int
m_hangup(queue_t *q, struct sdl *s, int err)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_HANGUP;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		s->i_state = LMI_UNUSABLE;
		printd(("%s: %p: <- M_HANGUP\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_INFO_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_info_ack(queue_t *q, struct sdl *s)
{
	mblk_t *mp;
	lmi_info_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = s->i_state;
		p->lmi_max_sdu = 64;
		p->lmi_min_sdu = 8;
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE1;	/* only STYLE1 for modules */
		printd(("%s: %p: <- LMI_INFO_ACK\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_OK_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_ok_ack(queue_t *q, struct sdl *s, long prim)
{
	mblk_t *mp;
	lmi_ok_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (s->i_state) {
		case LMI_ATTACH_PENDING:
			s->i_state = LMI_DISABLED;
			break;
		case LMI_DETACH_PENDING:
			s->i_state = LMI_UNATTACHED;
			break;
		}
		p->lmi_state = s->i_state;
		printd(("%s: %p: <- LMI_OK_ACK\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ERROR_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_error_ack(queue_t *q, struct sdl *s, long prim, ulong reason, ulong errno)
{
	mblk_t *mp;
	lmi_error_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ERROR_ACK;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_error_primitive = prim;
		switch (s->i_state) {
		case LMI_ATTACH_PENDING:
			s->i_state = LMI_UNATTACHED;
			break;
		case LMI_DETACH_PENDING:
			s->i_state = LMI_DISABLED;
			break;
		case LMI_ENABLE_PENDING:
			s->i_state = LMI_DISABLED;
			break;
		case LMI_DISABLE_PENDING:
			s->i_state = LMI_ENABLED;
			break;
		default:
			break;
		}
		p->lmi_state = s->i_state;
		printd(("%s: %p: <- LMI_ERROR_ACK\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ENABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
lmi_enable_con(queue_t *q, struct sdl *s)
{
	mblk_t *mp;
	lmi_enable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ENABLE_CON;
		switch (s->i_state) {
		case LMI_ENABLE_PENDING:
			s->i_state = LMI_ENABLED;
			s->timestamp = jiffies;
			s->bytecount = 0;
			break;
		default:
			swerr();
			break;
		}
		p->lmi_state = s->i_state;
		printd(("%s: %p: <- LMI_ENABLE_CON\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
lmi_disable_con(queue_t *q, struct sdl *s)
{
	mblk_t *mp;
	lmi_disable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_DISABLE_CON;
		switch (s->i_state) {
		case LMI_DISABLE_PENDING:
			s->i_state = LMI_DISABLED;
			break;
		default:
			swerr();
			break;
		}
		p->lmi_state = s->i_state;
		printd(("%s: %p: <- LMI_DISABLE_CON\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_optmgmt_ack(queue_t *q, struct sdl *s, ulong flags, caddr_t opt_ptr, size_t opt_len)
{
	mblk_t *mp;
	lmi_optmgmt_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OPTMGMT_ACK;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		printd(("%s: %p: <- LMI_OPTMGMT_ACK\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ERROR_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_error_ind(queue_t *q, struct sdl *s, long error, long reason)
{
	mblk_t *mp;
	lmi_error_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = error;
		p->lmi_reason = reason;
		p->lmi_state = s->i_state;
		printd(("%s: %p: <- LMI_ERROR_IND\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_STATS_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_stats_ind(queue_t *q, struct sdl *s)
{
	mblk_t *mp;
	lmi_stats_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_STATS_IND;
		p->lmi_interval = 0;
		p->lmi_timestamp = jiffies;
		printd(("%s: %p: <- LMI_STATS_IND\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_EVENT_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_event_ind(queue_t *q, struct sdl *s, ulong oid, ulong level, caddr_t inf_ptr, size_t inf_len)
{
	mblk_t *mp;
	lmi_event_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + inf_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_EVENT_IND;
		p->lmi_objectid = oid;
		p->lmi_timestamp = jiffies;
		p->lmi_severity = level;
		bcopy(inf_ptr, mp->b_wptr, inf_len);
		mp->b_wptr += inf_len;
		printd(("%s: %p: <- LMI_EVENT_IND\n", MOD_NAME, s));
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDL_RECEIVED_BITS_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdl_received_bits_ind(queue_t *q, struct sdl *s, mblk_t *dp)
{
	mblk_t *mp;
	sdl_received_bits_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdl_primitive = SDL_RECEIVED_BITS_IND;
		mp->b_cont = dp;
		putnext(s->oq, mp);
		return (QR_ABSORBED);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDL_DISCONNECT_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdl_disconnect_ind(queue_t *q, struct sdl *s)
{
	mblk_t *mp;
	sdl_disconnect_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdl_primitive = SDL_DISCONNECT_IND;
		putnext(s->oq, mp);
		return (QR_DONE);
	}
	rare();
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
 *  -------------------------------------------------------------------------
 *
 *  Timers
 *
 *  -------------------------------------------------------------------------
 */
enum { tall, t9 };

STATIC int sdl_t9_timeout(struct sdl *);

STATIC void
sdl_t9_expiry(caddr_t data)
{
	ss7_do_timeout(data, "t9", "sdl", &((struct sdl *) data)->timers.t9,
		       (int (*)(struct head *)) &sdl_t9_timeout, &sdl_t9_expiry);
}

STATIC void
sdl_stop_timer_t9(struct sdl *s)
{
	ss7_stop_timer((struct head *) s, "t9", "sdl", &s->timers.t9);
}

STATIC void
sdl_start_timer_t9(struct sdl *s)
{
	ss7_start_timer((struct head *) s, "t9", "sdl", &s->timers.t9, &sdl_t9_expiry,
			s->timestamp - jiffies);
};

STATIC INLINE void
__sdl_timer_stop(struct sdl *s, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case t9:
		sdl_stop_timer_t9(s);
		if (single)
			break;
		/* 
		   fall through */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
sdl_timer_stop(struct sdl *s, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&s->lock, flags);
	{
		__sdl_timer_stop(s, t);
	}
	spin_unlock_irqrestore(&s->lock, flags);
}
STATIC INLINE void
sdl_timer_start(struct sdl *s, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&s->lock, flags);
	{
		__sdl_timer_stop(s, t);
		switch (t) {
		case t9:
			sdl_start_timer_t9(s);
			break;
		default:
			swerr();
			break;
		}
	}
	spin_unlock_irqrestore(&s->lock, flags);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  State Machine
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  ========================================================================
 *
 *  EVENTS
 *
 *  ========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Timer Events
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  T10 EXPIRY
 *  -----------------------------------
 */
STATIC int
sdl_t9_timeout(struct sdl *s)
{
	/* 
	   wakeup the write queue */
	qenable(s->iq);
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  LM User -> LM Provider Primitives
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  LMI_INFO_REQ:
 *  -----------------------------------
 */
STATIC int
lmi_info_req(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	lmi_info_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	return lmi_info_ack(q, s);
      emsgsize:
	return lmi_error_ack(q, s, LMI_INFO_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_ATTACH_REQ:
 *  -----------------------------------
 */
STATIC int
lmi_attach_req(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state == LMI_UNUSABLE)
		goto eagain;
	if (s->i_style != LMI_STYLE2)
		goto eopnotsupp;
	if (s->i_state != LMI_UNATTACHED)
		goto outstate;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p) + 2)
		goto badppa;
	s->i_state = LMI_ATTACH_PENDING;
	return lmi_ok_ack(q, s, LMI_ATTACH_REQ);
      badppa:
	ptrace(("%s: %p: PROTO: bad ppa (too short)\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_ATTACH_REQ, LMI_BADPPA, EMSGSIZE);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_ATTACH_REQ, LMI_OUTSTATE, EPROTO);
      eopnotsupp:
	ptrace(("%s: %p: PROTO: primitive not supported for style\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_ATTACH_REQ, LMI_NOTSUPP, EOPNOTSUPP);
      eagain:
	ptrace(("%s: %p: INFO: waiting for streams to become usable\n", MOD_NAME, s));
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_ATTACH_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_DETACH_REQ:
 *  -----------------------------------
 */
STATIC int
lmi_detach_req(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state == LMI_UNUSABLE)
		goto eagain;
	if (s->i_style != LMI_STYLE2)
		goto eopnotsupp;
	if (s->i_state != LMI_DISABLED)
		goto outstate;
	s->i_state = LMI_DETACH_PENDING;
	return lmi_ok_ack(q, s, LMI_DETACH_REQ);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_DETACH_REQ, LMI_OUTSTATE, EPROTO);
      eopnotsupp:
	ptrace(("%s: %p: PROTO: primitive not supported for style\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_DETACH_REQ, LMI_NOTSUPP, EOPNOTSUPP);
      eagain:
	ptrace(("%s: %p: INFO: waiting for streams to become usable\n", MOD_NAME, s));
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_DETACH_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_ENABLE_REQ:
 *  -----------------------------------
 */
STATIC int
lmi_enable_req(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state == LMI_UNUSABLE)
		goto eagain;
	if (s->i_state != LMI_DISABLED)
		goto outstate;
	s->i_state = LMI_ENABLE_PENDING;
	return lmi_enable_con(q, s);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_ENABLE_REQ, LMI_OUTSTATE, EPROTO);
      eagain:
	ptrace(("%s: %p: INFO: waiting for streams to become usable\n", MOD_NAME, s));
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_ENABLE_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_DISABLE_REQ:
 *  -----------------------------------
 */
STATIC int
lmi_disable_req(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state == LMI_UNUSABLE)
		goto eagain;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	s->i_state = LMI_DISABLE_PENDING;
	return lmi_disable_con(q, s);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_DISABLE_REQ, LMI_OUTSTATE, EPROTO);
      eagain:
	ptrace(("%s: %p: INFO: waiting for streams to become usable\n", MOD_NAME, s));
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_DISABLE_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_OPTMGMT_REQ:
 *  -----------------------------------
 */
STATIC int
lmi_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	lmi_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	goto eopnotsupp;
      eopnotsupp:
	ptrace(("%s: %p: PROTO: primitive not supported\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_OPTMGMT_REQ, LMI_NOTSUPP, EOPNOTSUPP);
      emsgsize:
	(void) s;
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return lmi_error_ack(q, s, LMI_OPTMGMT_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDL User -> SDL Provider Primitives
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int
sdl_send_data(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	int size;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	if (s->timestamp > jiffies) {
		printd(("%s: %p: %s sleeping for %ld ms\n", MOD_NAME, s, __FUNCTION__,
			s->timestamp - jiffies));
		sdl_timer_start(s, t9);
		return (-EAGAIN);
	}
	if (s->timestamp < jiffies) {
		s->bytecount = 0;
		s->timestamp = jiffies;
		s->stats.tx_underruns++;
	}
	if (s->statem.tx_state != SDL_STATE_IDLE) {
		size = msgdsize(mp);
	} else {
		/* 
		   idle when transmitter shut down */
		mblk_t *dp;
		for (size = 0, dp = mp; dp; dp = dp->b_cont) {
			if (dp->b_wptr > dp->b_rptr) {
				int len = dp->b_wptr - dp->b_rptr;
				size += len;
				memset(dp->b_rptr, 0xff, len);
			}
		}
	}
	putnext(s->iq, mp);
	s->stats.tx_octets += size;
	s->bytecount += size;
	ensure(s->tickbytes > 0, s->tickbytes = 1);
	while (s->bytecount >= s->tickbytes) {
		s->bytecount -= s->tickbytes;
		s->timestamp++;
	}
	return (QR_ABSORBED);
      outstate:
	return (-EPROTO);	/* ignore */
}

/*
 *  SDL_BITS_FOR_TRANSMISSION_REQ
 *  -----------------------------------
 *  This is the non-preferred way of sending data.  It is preferred that
 *  M_DATA blocks are used (above).  We just strip off the M_PROTO and requeue
 *  only the M_DATA blocks.
 */
STATIC int
sdl_bits_for_transmission_req(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	sdl_bits_for_transmission_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state == LMI_UNUSABLE)
		goto eagain;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	if (!mp->b_cont || mp->b_cont->b_datap->db_type != M_DATA)
		goto eproto;
	trace();
	return (QR_STRIP);
      eproto:
	ptrace(("%s: %p: ERROR: bad message\n", MOD_NAME, s));
	return m_error(q, s, EPROTO);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return m_error(q, s, EPROTO);
      eagain:
	ptrace(("%s: %p: INFO: waiting for streams to become usable\n", MOD_NAME, s));
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return m_error(q, s, EPROTO);
}

/*
 *  SDL_CONNECT_REQ:
 *  -----------------------------------
 */
STATIC int
sdl_connect_req(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	sdl_connect_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state == LMI_UNUSABLE)
		goto eagain;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	s->statem.tx_state = SDL_STATE_IN_SERVICE;
	s->statem.rx_state = SDL_STATE_IN_SERVICE;
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return m_error(q, s, EPROTO);
      eagain:
	ptrace(("%s: %p: INFO: waiting for streams to become usable\n", MOD_NAME, s));
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return m_error(q, s, EPROTO);
}

/*
 *  SDL_DISCONNECT_REQ:
 *  -----------------------------------
 */
STATIC int
sdl_disconnect_req(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	sdl_connect_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (s->i_state == LMI_UNUSABLE)
		goto eagain;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	s->statem.tx_state = SDL_STATE_IDLE;
	s->statem.rx_state = SDL_STATE_IDLE;
	return (QR_DONE);
      outstate:
	ptrace(("%s: %p: PROTO: out of state\n", MOD_NAME, s));
	return m_error(q, s, EPROTO);
      eagain:
	ptrace(("%s: %p: INFO: waiting for streams to become usable\n", MOD_NAME, s));
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: %p: PROTO: M_PROTO block too short\n", MOD_NAME, s));
	return m_error(q, s, EPROTO);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDL-Provider -> SDL-User Primitives
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int
sdl_recv_data(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	int size;
	if (s->i_state != LMI_ENABLED)
		goto outstate;
	if (s->statem.rx_state != SDL_STATE_IDLE) {
		size = msgdsize(mp);
	} else {
		/* 
		   idle when receiver shut down */
		mblk_t *dp;
		for (size = 0, dp = mp; dp; dp = dp->b_cont) {
			if (dp->b_wptr > dp->b_rptr) {
				int len = dp->b_wptr - dp->b_rptr;
				size += len;
				memset(dp->b_rptr, 0xff, len);
			}
		}
	}
	s->stats.rx_octets += size;
	putnext(s->oq, mp);
	return (QR_ABSORBED);
      outstate:
	return (QR_DONE);	/* ignore */
}

/*
 *  =========================================================================
 *
 *  IO Controls
 *
 *  =========================================================================
 *
 *  LM IO Controls
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
lmi_test_config(struct sdl *sdl, lmi_config_t * arg)
{
	return (-EOPNOTSUPP);
}
STATIC int
lmi_commit_config(struct sdl *sdl, lmi_config_t * arg)
{
	return (-EOPNOTSUPP);
}
STATIC int
lmi_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		int ret = 0;
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			*arg = s->option;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		int ret = 0;
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->option = *arg;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		psw_t flags;
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			arg->version = s->i_version;
			arg->style = s->i_style;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		psw_t flags;
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->i_version = arg->version;
			s->i_style = arg->style;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return lmi_test_config(s, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return lmi_commit_config(s, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		psw_t flags;
		lmi_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			arg->state = s->i_state;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccmreset(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		lmi_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->i_state = LMI_UNUSABLE;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&s->lock, flags);
		{
			*arg = s->statsp;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret = 0;
		psw_t flags;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->statsp = *arg;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		lmi_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret;
		psw_t flags;
		spin_lock_irqsave(&s->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccstats(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	int ret;
	psw_t flags;
	(void) mp;
	spin_lock_irqsave(&s->lock, flags);
	{
		ret = -EOPNOTSUPP;
	}
	spin_unlock_irqrestore(&s->lock, flags);
	return (ret);
}
STATIC int
lmi_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret;
		psw_t flags;
		spin_lock_irqsave(&s->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret;
		psw_t flags;
		spin_lock_irqsave(&s->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret;
		psw_t flags;
		spin_lock_irqsave(&s->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDL IO Controls
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdl_test_config(struct sdl *s, sdl_config_t * arg)
{
	(void) s;
	(void) arg;
	// fixme(("%s: FIXME: write this function\n", MOD_NAME));
	return (0);
}
STATIC void
sdl_commit_config(struct sdl *s, sdl_config_t * arg)
{
	s->config = *arg;
	/* 
	   reshape traffic */
	if (s->timestamp > jiffies) {
		sdl_timer_stop(s, t9);
		s->bytecount += s->tickbytes * (s->timestamp - jiffies);
	}
	if (s->timestamp < jiffies) {
		s->bytecount = 0;
		s->stats.tx_underruns++;
	}
	s->timestamp = jiffies;
	s->tickbytes = s->config.ifrate / HZ / 8;
	ensure(s->tickbytes > 0, s->tickbytes = 1);
	while (s->bytecount >= s->tickbytes) {
		s->bytecount -= s->tickbytes;
		s->timestamp++;
	}
	if (s->timestamp > jiffies) {
		printd(("%s: %p: %s sleeping for %ld ms\n", MOD_NAME, s, __FUNCTION__,
			s->timestamp - jiffies));
		sdl_timer_start(s, t9);
	}
}
STATIC int
sdl_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			*arg = s->option;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->option = *arg;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		psw_t flags;
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			*arg = s->config;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		int ret;
		psw_t flags;
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			if (!(ret = sdl_test_config(s, arg)))
				sdl_commit_config(s, arg);
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		int ret;
		psw_t flags;
		spin_lock_irqsave(&s->lock, flags);
		{
			ret = sdl_test_config(s, arg);
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		spin_lock_irqsave(&s->lock, flags);
		{
			sdl_commit_config(s, arg);
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		psw_t flags;
		sdl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			*arg = s->statem;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccmreset(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	(void) s;
	(void) mp;
	// fixme(("%s: FIXME: Support master reset\n", MOD_NAME));
	return (-EOPNOTSUPP);
}
STATIC int
sdl_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		psw_t flags;
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			*arg = s->statsp;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		psw_t flags;
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->statsp = *arg;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		psw_t flags;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			*arg = s->stats;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccstats(queue_t *q, mblk_t *mp)
{
	psw_t flags;
	struct sdl *s = SDL_PRIV(q);
	(void) mp;
	spin_lock_irqsave(&s->lock, flags);
	{
		bzero(&s->stats, sizeof(s->stats));
	}
	spin_unlock_irqrestore(&s->lock, flags);
	return (0);
}
STATIC int
sdl_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		psw_t flags;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			*arg = s->notify;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		psw_t flags;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->notify = *arg;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdl *s = SDL_PRIV(q);
		psw_t flags;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&s->lock, flags);
		{
			s->notify.events &= ~arg->events;
		}
		spin_unlock_irqrestore(&s->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccdisctx(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	psw_t flags;
	spin_lock_irqsave(&s->lock, flags);
	{
		s->config.ifflags &= ~SDL_IF_TX_RUNNING;
		s->statem.tx_state = SDL_STATE_IDLE;
	}
	spin_unlock_irqrestore(&s->lock, flags);
	return (0);
}
STATIC int
sdl_ioccconntx(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	psw_t flags;
	spin_lock_irqsave(&s->lock, flags);
	{
		s->config.ifflags |= SDL_IF_TX_RUNNING;
		s->statem.tx_state = SDL_STATE_IN_SERVICE;
	}
	spin_unlock_irqrestore(&s->lock, flags);
	return (0);
}

/*
 *  ========================================================================
 *
 *  STREAMS Message Handling
 *
 *  ========================================================================
 *
 *  M_IOCTL Handling
 *  -----------------------------------------------------------------------
 */
STATIC int
sdl_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	struct linkblk *lp = (struct linkblk *) arg;
	int ret = 0;
	switch (type) {
	case __SID:
	{
		switch (nr) {
		case _IOC_NR(I_STR):
		case _IOC_NR(I_LINK):
		case _IOC_NR(I_PLINK):
		case _IOC_NR(I_UNLINK):
		case _IOC_NR(I_PUNLINK):
			(void) lp;
			ptrace(("%s: %p: ERROR: Unsupported STREAMS ioctl %d\n", MOD_NAME, s, nr));
			ret = (-EINVAL);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unsupported STREAMS ioctl %d\n", MOD_NAME, s, nr));
			ret = (-EOPNOTSUPP);
			break;
		}
		break;
	}
	case LMI_IOC_MAGIC:
	{
		if (count < size || s->i_state == LMI_UNATTACHED) {
			ret = (-EINVAL);
			break;
		}
		switch (nr) {
		case _IOC_NR(LMI_IOCGOPTIONS):	/* lmi_option_t */
			ret = lmi_iocgoptions(q, mp);
			break;
		case _IOC_NR(LMI_IOCSOPTIONS):	/* lmi_option_t */
			ret = lmi_iocsoptions(q, mp);
			break;
		case _IOC_NR(LMI_IOCGCONFIG):	/* lmi_config_t */
			ret = lmi_iocgconfig(q, mp);
			break;
		case _IOC_NR(LMI_IOCSCONFIG):	/* lmi_config_t */
			ret = lmi_iocsconfig(q, mp);
			break;
		case _IOC_NR(LMI_IOCTCONFIG):	/* lmi_config_t */
			ret = lmi_ioctconfig(q, mp);
			break;
		case _IOC_NR(LMI_IOCCCONFIG):	/* lmi_config_t */
			ret = lmi_ioccconfig(q, mp);
			break;
		case _IOC_NR(LMI_IOCGSTATEM):	/* lmi_statem_t */
			ret = lmi_iocgstatem(q, mp);
			break;
		case _IOC_NR(LMI_IOCCMRESET):	/* lmi_statem_t */
			ret = lmi_ioccmreset(q, mp);
			break;
		case _IOC_NR(LMI_IOCGSTATSP):	/* lmi_sta_t */
			ret = lmi_iocgstatsp(q, mp);
			break;
		case _IOC_NR(LMI_IOCSSTATSP):	/* lmi_sta_t */
			ret = lmi_iocsstatsp(q, mp);
			break;
		case _IOC_NR(LMI_IOCGSTATS):	/* lmi_stats_t */
			ret = lmi_iocgstats(q, mp);
			break;
		case _IOC_NR(LMI_IOCCSTATS):	/* lmi_stats_t */
			ret = lmi_ioccstats(q, mp);
			break;
		case _IOC_NR(LMI_IOCGNOTIFY):	/* lmi_notify_t */
			ret = lmi_iocgnotify(q, mp);
			break;
		case _IOC_NR(LMI_IOCSNOTIFY):	/* lmi_notify_t */
			ret = lmi_iocsnotify(q, mp);
			break;
		case _IOC_NR(LMI_IOCCNOTIFY):	/* lmi_notify_t */
			ret = lmi_ioccnotify(q, mp);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unsupported LMI ioctl %d\n", MOD_NAME, s, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SDL_IOC_MAGIC:
	{
		if (count < size || s->i_state == LMI_UNATTACHED) {
			ret = (-EINVAL);
			break;
		}
		switch (nr) {
		case _IOC_NR(SDL_IOCGOPTIONS):	/* lmi_option_t */
			ret = sdl_iocgoptions(q, mp);
			break;
		case _IOC_NR(SDL_IOCSOPTIONS):	/* lmi_option_t */
			ret = sdl_iocsoptions(q, mp);
			break;
		case _IOC_NR(SDL_IOCGCONFIG):	/* sdl_config_t */
			ret = sdl_iocgconfig(q, mp);
			break;
		case _IOC_NR(SDL_IOCSCONFIG):	/* sdl_config_t */
			ret = sdl_iocsconfig(q, mp);
			break;
		case _IOC_NR(SDL_IOCTCONFIG):	/* sdl_config_t */
			ret = sdl_ioctconfig(q, mp);
			break;
		case _IOC_NR(SDL_IOCCCONFIG):	/* sdl_config_t */
			ret = sdl_ioccconfig(q, mp);
			break;
		case _IOC_NR(SDL_IOCGSTATEM):	/* sdl_statem_t */
			ret = sdl_iocgstatem(q, mp);
			break;
		case _IOC_NR(SDL_IOCCMRESET):	/* sdl_statem_t */
			ret = sdl_ioccmreset(q, mp);
			break;
		case _IOC_NR(SDL_IOCGSTATSP):	/* lmi_sta_t */
			ret = sdl_iocgstatsp(q, mp);
			break;
		case _IOC_NR(SDL_IOCSSTATSP):	/* lmi_sta_t */
			ret = sdl_iocsstatsp(q, mp);
			break;
		case _IOC_NR(SDL_IOCGSTATS):	/* sdl_stats_t */
			ret = sdl_iocgstats(q, mp);
			break;
		case _IOC_NR(SDL_IOCCSTATS):	/* sdl_stats_t */
			ret = sdl_ioccstats(q, mp);
			break;
		case _IOC_NR(SDL_IOCGNOTIFY):	/* sdl_notify_t */
			ret = sdl_iocgnotify(q, mp);
			break;
		case _IOC_NR(SDL_IOCSNOTIFY):	/* sdl_notify_t */
			ret = sdl_iocsnotify(q, mp);
			break;
		case _IOC_NR(SDL_IOCCNOTIFY):	/* sdl_notify_t */
			ret = sdl_ioccnotify(q, mp);
			break;
		case _IOC_NR(SDL_IOCCDISCTX):	/* */
			ret = sdl_ioccdisctx(q, mp);
			break;
		case _IOC_NR(SDL_IOCCCONNTX):	/* */
			ret = sdl_ioccconntx(q, mp);
			break;
		default:
			ptrace(("%s: ERROR: Unsupported SDL ioctl %d\n", MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	default:
		ret = (QR_PASSFLOW);
		break;
	}
	if (ret > 0) {
		return (ret);
	} else if (ret == 0) {
		mp->b_datap->db_type = M_IOCACK;
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
	} else {
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_error = -ret;
		iocp->ioc_rval = -1;
	}
	qreply(q, mp);
	return (QR_ABSORBED);
}

/*
 *  -----------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -----------------------------------------------------------------------
 */
/*
 *  Primitives from User to SDL.
 *  -----------------------------------
 */
STATIC int
sdl_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct sdl *s = SDL_PRIV(q);
	ulong oldstate = s->i_state;
	if ((prim = *(ulong *) mp->b_rptr) == SDL_BITS_FOR_TRANSMISSION_REQ) {
		printd(("%s: %p: -> SDL_BITS_FOR_TRANSMISSION_REQ [%d]\n", MOD_NAME, s,
			msgdsize(mp->b_cont)));
		if ((rtn = sdl_bits_for_transmission_req(q, mp)) < 0)
			s->i_state = oldstate;
		return (rtn);
	}
	switch (prim) {
	case SDL_BITS_FOR_TRANSMISSION_REQ:
		printd(("%s: %p: -> SDL_BITS_FOR_TRANSMISSION_REQ [%d]\n", MOD_NAME, s,
			msgdsize(mp->b_cont)));
		rtn = sdl_bits_for_transmission_req(q, mp);
		break;
	case SDL_CONNECT_REQ:
		printd(("%s: %p: -> SDL_CONNECT_REQ\n", MOD_NAME, s));
		rtn = sdl_connect_req(q, mp);
		break;
	case SDL_DISCONNECT_REQ:
		printd(("%s: %p: -> SDL_DISCONNECT_REQ\n", MOD_NAME, s));
		rtn = sdl_disconnect_req(q, mp);
		break;
	case LMI_INFO_REQ:
		printd(("%s: %p: -> LMI_INFO_REQ\n", MOD_NAME, s));
		rtn = lmi_info_req(q, mp);
		break;
	case LMI_ATTACH_REQ:
		printd(("%s: %p: -> LMI_ATTACH_REQ\n", MOD_NAME, s));
		rtn = lmi_attach_req(q, mp);
		break;
	case LMI_DETACH_REQ:
		printd(("%s: %p: -> LMI_DETACH_REQ\n", MOD_NAME, s));
		rtn = lmi_detach_req(q, mp);
		break;
	case LMI_ENABLE_REQ:
		printd(("%s: %p: -> LMI_ENABLE_REQ\n", MOD_NAME, s));
		rtn = lmi_enable_req(q, mp);
		break;
	case LMI_DISABLE_REQ:
		printd(("%s: %p: -> LMI_DISABLE_REQ\n", MOD_NAME, s));
		rtn = lmi_disable_req(q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		printd(("%s: %p: -> LMI_OPTMGMT_REQ\n", MOD_NAME, s));
		rtn = lmi_optmgmt_req(q, mp);
		break;
	default:
		/* 
		   discard anything we don't recognize */
		swerr();
		rtn = (-EOPNOTSUPP);
		break;
	}
	if (rtn < 0)
		s->i_state = oldstate;
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
sdl_w_data(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	(void) s;
	printd(("%s: %p: -> M_DATA [%d]\n", MOD_NAME, s, msgdsize(mp)));
	return sdl_send_data(q, mp);
}
STATIC INLINE int
sdl_r_data(queue_t *q, mblk_t *mp)
{
	struct sdl *s = SDL_PRIV(q);
	(void) s;
	printd(("%s: %p: M_DATA [%d] <-\n", MOD_NAME, s, msgdsize(mp)));
	return sdl_recv_data(q, mp);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int
sdl_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return sdl_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sdl_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sdl_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return sdl_w_ioctl(q, mp);
	}
	return (QR_PASSALONG);
}
STATIC INLINE int
sdl_r_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return sdl_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sdl_r_data(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSALONG);
}

/*
 *  =======================================================================
 *
 *  OPEN and CLOSE
 *
 *  =======================================================================
 */
STATIC int
sdl_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		int cmajor = getmajor(*devp);
		int cminor = getminor(*devp);
		struct sdl *s;
		/* 
		   test for multiple push */
		for (s = sdl_opens; s; s = s->next) {
			if (s->u.dev.cmajor == cmajor && s->u.dev.cminor == cminor) {
				MOD_DEC_USE_COUNT;
				return (ENXIO);
			}
		}
		if (!(sdl_alloc_priv(q, &sdl_opens, devp, crp))) {
			MOD_DEC_USE_COUNT;
			return (ENOMEM);
		}
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return (EIO);
}
STATIC int
sdl_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	sdl_free_priv(q);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  ========================================================================
 *
 *  PRIVATE struct allocation, deallocation and cache
 *
 *  ========================================================================
 */
STATIC kmem_cache_t *sdl_priv_cachep = NULL;
STATIC int
sdl_init_caches(void)
{
	if (!sdl_priv_cachep
	    && !(sdl_priv_cachep =
		 kmem_cache_create("sdl_priv_cachep", sizeof(struct sdl), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate sdl_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	}
	printd(("%s: Allocated/selected private structure cache\n", MOD_NAME));
	return (0);
}
STATIC int
sdl_term_caches(void)
{
	if (sdl_priv_cachep) {
		if (kmem_cache_destroy(sdl_priv_cachep)) {
			cmn_err(CE_WARN, "%s: id not destroy sdl_priv_cachep.", __FUNCTION__);
			return (-EBUSY);
		} else
			printd(("%s: destroyed sdl_priv_cachep\n", MOD_NAME));
	}
	return (0);
}
STATIC struct sdl *
sdl_alloc_priv(queue_t *q, struct sdl **sp, dev_t *devp, cred_t *crp)
{
	struct sdl *s;
	if ((s = kmem_cache_alloc(sdl_priv_cachep, SLAB_ATOMIC))) {
		printd(("%s: %p: allocated module private structure\n", MOD_NAME, s));
		bzero(s, sizeof(*s));
		sdl_get(s);	/* first get */
		s->u.dev.cmajor = getmajor(*devp);
		s->u.dev.cminor = getminor(*devp);
		s->cred = *crp;
		(s->oq = RD(q))->q_ptr = sdl_get(s);
		(s->iq = WR(q))->q_ptr = sdl_get(s);
		spin_lock_init(&s->qlock);	/* "sdl-queue-lock" */
		s->o_prim = &sdl_r_prim;
		s->i_prim = &sdl_w_prim;
		s->o_wakeup = NULL;	// &sdl_rx_wakeup;
		s->i_wakeup = NULL;	// &sdl_tx_wakeup;
		s->i_state = LMI_DISABLED;
		s->i_style = LMI_STYLE1;
		s->i_version = 1;
		spin_lock_init(&s->qlock);	/* "sdl-priv_lock" */
		if ((s->next = *sp))
			s->next->prev = &s->next;
		s->prev = sp;
		*sp = sdl_get(s);
		printd(("%s: %p: linked module private structure\n", MOD_NAME, s));
		s->timestamp = jiffies;
		s->tickbytes = s->config.ifrate / HZ / 8;
		s->bytecount = 0;
		/* 
		   configuration defaults */
		s->option = lmi_default;
		s->config = sdl_default;
		printd(("%s: %p: setting module private structure defaults\n", MOD_NAME, s));
	} else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", MOD_NAME));
	return (s);
}
STATIC void
sdl_free_priv(queue_t *q)
{
	struct sdl *s = SDL_PRIV(q);
	psw_t flags;
	ensure(s, return);
	spin_lock_irqsave(&s->lock, flags);
	{
		ss7_unbufcall((str_t *) s);
		sdl_timer_stop(s, tall);
		if ((*(s->prev) = s->next))
			s->next->prev = s->prev;
		s->next = NULL;
		s->prev = &s->next;
		sdl_put(s);
		s->oq->q_ptr = NULL;
		flushq(s->oq, FLUSHALL);
		s->oq = NULL;
		sdl_put(s);
		s->iq->q_ptr = NULL;
		flushq(s->iq, FLUSHALL);
		s->iq = NULL;
		sdl_put(s);
	}
	spin_unlock_irqrestore(&s->lock, flags);
	sdl_put(s);		/* final put */
}
STATIC struct sdl *
sdl_get(struct sdl *s)
{
	atomic_inc(&s->refcnt);
	return (s);
}
STATIC void
sdl_put(struct sdl *s)
{
	if (atomic_dec_and_test(&s->refcnt)) {
		kmem_cache_free(sdl_priv_cachep, s);
		printd(("%s: %p: freed sdl private structure\n", MOD_NAME, s));
	}
}

/*
 *  =========================================================================
 *
 *  Registration and initialization
 *
 *  =========================================================================
 */
#ifdef LINUX
/*
 *  Linux Registration
 *  -------------------------------------------------------------------------
 */

unsigned short modid = MOD_ID;
#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the SDL module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw sdl_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sdlinfo,
	.f_flag = 0,
	.f_kmod = THIS_MODULE,
};

STATIC int
sdl_register_strmod(void)
{
	int err;
	if ((err = register_strmod(&sdl_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
sdl_unregister_strmod(void)
{
	int err;
	if ((err = unregister_strmod(&sdl_fmod)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

/*
 *  Linux STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LIS

STATIC int
sdl_register_strmod(void)
{
	int err;
	if ((err = lis_register_strmod(&sdlinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	return (0);
}

STATIC int
sdl_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&sdlinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
sdlinit(void)
{
	int err;
	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = sdl_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = sdl_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		sdl_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
sdlterminate(void)
{
	int err;
	if ((err = sdl_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = sdl_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(sdlinit);
module_exit(sdlterminate);

#endif				/* LINUX */
