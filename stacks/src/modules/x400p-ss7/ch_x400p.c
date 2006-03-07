/*****************************************************************************

 @(#) $RCSfile: ch_x400p.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/03/07 01:14:29 $

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

 Last Modified $Date: 2006/03/07 01:14:29 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ch_x400p.c,v $
 Revision 0.9.2.13  2006/03/07 01:14:29  brian
 - binary compatible callouts

 Revision 0.9.2.12  2006/03/04 13:00:20  brian
 - FC4 x86_64 gcc 4.0.4 2.6.15 changes

 *****************************************************************************/

#ident "@(#) $RCSfile: ch_x400p.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/03/07 01:14:29 $"

static char const ident[] =
    "$RCSfile: ch_x400p.c,v $ $Name:  $($Revision: 0.9.2.13 $) $Date: 2006/03/07 01:14:29 $";

#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/chi.h>
#include <ss7/chi_ioctl.h>

#define CH_SDL_DESCRIP		"X400P-SS7 CHANNEL (CH) STREAMS MODULE."
#define CH_SDL_EXTRA		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define CH_SDL_REVISION		"OpenSS7 $RCSfile: ch_x400p.c,v $ $Name:  $ ($Revision: 0.9.2.13 $) $Date: 2006/03/07 01:14:29 $"
#define CH_SDL_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define CH_SDL_DEVICE		"Supports SDLI pseudo-device drivers."
#define CH_SDL_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define CH_SDL_LICENSE		"GPL"
#define CH_SDL_BANNER		CH_SDL_DESCRIP		"\n" \
				CH_SDL_EXTRA		"\n" \
				CH_SDL_REVISION		"\n" \
				CH_SDL_COPYRIGHT	"\n" \
				CH_SDL_DEVICE		"\n" \
				CH_SDL_CONTACT
#define CH_SDL_SPLASH		CH_SDL_DEVICE		"\n" \
				CH_SDL_REVISION

#ifdef LINUX
MODULE_AUTHOR(CH_SDL_CONTACT);
MODULE_DESCRIPTION(CH_SDL_DESCRIP);
MODULE_SUPPORTED_DEVICE(CH_SDL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(CH_SDL_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-ch_x400p");
#endif
#endif				/* LINUX */

#ifdef LFS
#define CH_SDL_MOD_ID		CONFIG_STREAMS_CH_SDL_MODID
#define CH_SDL_MOD_NAME		CONFIG_STREAMS_CH_SDL_NAME
#endif
/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define MOD_ID		CH_SDL_MOD_ID
#define MOD_NAME	CH_SDL_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	CH_SDL_BANNER
#else				/* MODULE */
#define MOD_BANNER	CH_SDL_SPLASH
#endif				/* MODULE */

STATIC struct module_info ch_minfo = {
	mi_idnum:MOD_ID,		/* Module ID number */
	mi_idname:MOD_NAME,		/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1,			/* Hi water mark */
	mi_lowat:0,			/* Lo water mark */
};

STATIC streamscall int ch_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int ch_close(queue_t *, int, cred_t *);

STATIC struct qinit ch_rinit = {
	qi_putp:ss7_oput,		/* Read put (message from below) */
	qi_qopen:ch_open,		/* Each open */
	qi_qclose:ch_close,		/* Last close */
	qi_minfo:&ch_minfo,		/* Information */
};

STATIC struct qinit ch_winit = {
	qi_putp:ss7_iput,		/* Write put (message from above) */
	qi_minfo:&ch_minfo,		/* Information */
};

STATIC struct streamtab ch_sdlinfo = {
	st_rdinit:&ch_rinit,		/* Upper read queue */
	st_wrinit:&ch_winit,		/* Upper write queue */
};

/*
 *  =========================================================================
 *
 *  Private Data Structures
 *
 *  =========================================================================
 */

/* channel structure */
typedef struct ch {
	STR_DECLARATION (struct ch);	/* stream declaration */
	struct ch_config config;	/* configuration */
	struct ch_stats statsp;		/* stats periods */
	struct ch_stats stats;		/* statistics */
	struct ch_notify notify;	/* notifications */
	uchar add_ptr[32];		/* attached address */
	size_t add_len;			/* attached address length */
	uchar rem_ptr[32];		/* remote address */
	size_t rem_len;			/* remote address length */
} ch_t;
#define CH_PRIV(__q) ((struct ch *)(__q)->q_ptr)

struct ch_config ch_default = {
	block_size:64,			/* 64 bits per block */
	encoding:CH_ENCODING_NONE,	/* sample block encoding */
	sample_size:8,			/* sample size */
	rate:8000,			/* clock rate */
	tx_channels:1,			/* tx channels */
	rx_channels:1,			/* tx channels */
	opt_flags:CH_PARM_OPT_CLRCH,	/* option flags */
};

STATIC struct ch *ch_opens = NULL;

STATIC struct ch *ch_alloc_priv(queue_t *, struct ch **, dev_t *, cred_t *);
STATIC struct ch *ch_get(struct ch *);
STATIC void ch_put(struct ch *);
STATIC void ch_free_priv(queue_t *);

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
 */

/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC int
m_error(queue_t *q, struct ch *ch, int error)
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
			ch->i_state = CHS_UNUSABLE;
			printd(("%s: %p: <- M_HANGUP\n", MOD_NAME, ch));
			putnext(ch->oq, mp);
			return (-error);
		} else {
			mp->b_datap->db_type = M_ERROR;
			*mp->b_wptr++ = error < 0 ? -error : error;
			*mp->b_wptr++ = error < 0 ? -error : error;
			ch->i_state = CHS_UNUSABLE;
			printd(("%s: %p: <- M_ERROR\n", MOD_NAME, ch));
			putnext(ch->oq, mp);
			return (QR_DONE);
		}
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_INFO_ACK
 *  -----------------------------------
 *  Indicates to the channel user information concerning the channel provider and the
 *  attached channel (if any).
 */
STATIC INLINE int
ch_info_ack(queue_t *q, struct ch *ch)
{
	mblk_t *mp;
	struct CH_info_ack *p;
	struct CH_parms_circuit *o;
	size_t pad_len = (ch->add_len + (sizeof(ulong) - 1)) & ~(sizeof(ulong) - 1);
	if ((mp = ss7_allocb(q, sizeof(*p) + pad_len + sizeof(*o), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->ch_primitive = CH_INFO_ACK;
		p->ch_addr_length = ch->add_len;
		p->ch_addr_offset = ch->add_len ? sizeof(*p) : 0;
		p->ch_parm_length = sizeof(*o);
		p->ch_parm_offset = sizeof(*p) + pad_len;
		p->ch_prov_flags = 0;
		p->ch_style = 0x1;
		p->ch_version = CH_VERSION_1_0;
		if (ch->add_len) {
			bcopy(ch->add_ptr, mp->b_wptr, ch->add_len);
			mp->b_wptr += ch->add_len;
		}
		o = (typeof(o)) mp->b_wptr++;
		o->cp_type = CH_PARMS_CIRCUIT;
		o->cp_block_size = ch->config.block_size;
		o->cp_encoding = ch->config.encoding;
		o->cp_sample_size = ch->config.sample_size;
		o->cp_rate = ch->config.rate;
		o->cp_tx_channels = ch->config.tx_channels;
		o->cp_rx_channels = ch->config.rx_channels;
		o->cp_opt_flags = ch->config.opt_flags;
		printd(("%s: %p: <- CH_INFO_ACK\n", MOD_NAME, ch));
		putnext(ch->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC INLINE int
ch_optmgmt_ack(queue_t *q, struct ch *ch, uchar *opt_ptr, size_t opt_len, ulong flags)
{
	mblk_t *mp;
	struct CH_optmgmt_ack *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->ch_primitive = CH_OPTMGMT_ACK;
		p->ch_opt_length = opt_len;
		p->ch_opt_offset = opt_len ? sizeof(*p) : 0;
		p->ch_mgmt_flags = flags;
		if (opt_len) {
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
		}
		printd(("%s: %p: <- CH_OPTMGMT_ACK\n", MOD_NAME, ch));
		putnext(ch->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_OK_ACK
 *  -----------------------------------
 *  Indicates to the channel user that the last operation requiring
 *  acknowledgement was completed successfully.  (There are only two
 *  operations requiring acknowledgement: CH_ATTACH_REQ and CH_DETACH_REQ.)
 */
STATIC INLINE int
ch_ok_ack(queue_t *q, struct ch *ch)
{
	mblk_t *mp;
	struct CH_ok_ack *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->ch_primitive = CH_OK_ACK;
		switch (ch->i_state) {
		case CHS_WACK_AREQ:
			p->ch_correct_prim = CH_ATTACH_REQ;
			p->ch_state = ch->i_state = CHS_ATTACHED;
			break;
		case CHS_WACK_UREQ:
			p->ch_correct_prim = CH_DETACH_REQ;
			p->ch_state = ch->i_state = CHS_DETACHED;
			break;
		case CHS_UNUSABLE:
			p->ch_correct_prim = CH_DETACH_REQ;
			p->ch_state = ch->i_state = CHS_UNUSABLE;
			break;
		default:
			swerr();
			freemsg(mp);
			return (-EFAULT);
		}
		printd(("%s: %p: <- CH_OK_ACK\n", MOD_NAME, ch));
		putnext(ch->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_ERROR_ACK
 *  -----------------------------------
 *  Indicates to the channel user that the last operation requiring
 *  acknowledgement or confirmation did not complete and was unsuccessful
 *  (suffered a non-fatal error).  There are six operations requiring
 *  acknowledgement or confirmation.  In addition, this error is returned when
 *  unrecognized primitives are sent.
 */
STATIC INLINE int
ch_error_ack(queue_t *q, struct ch *ch, ulong prim, long error)
{
	mblk_t *mp;
	struct CH_error_ack *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->ch_primitive = CH_ERROR_ACK;
		p->ch_error_type = error > 0 ? error : CHSYSERR;
		p->ch_unix_error = error < 0 ? -error : 0;
		p->ch_error_primitive = prim;
		switch (ch->i_state) {
		case CHS_WACK_AREQ:
			if (prim == CH_ATTACH_REQ)
				p->ch_state = ch->i_state = CHS_DETACHED;
			break;
		case CHS_WACK_UREQ:
			if (prim == CH_DETACH_REQ)
				p->ch_state = ch->i_state = CHS_ATTACHED;
			break;
		case CHS_WACK_EREQ:
		case CHS_WCON_EREQ:
			if (prim == CH_ENABLE_REQ)
				p->ch_state = ch->i_state = CHS_ATTACHED;
			break;
		case CHS_WACK_RREQ:
		case CHS_WCON_RREQ:
			if (prim == CH_DISABLE_REQ)
				p->ch_state = ch->i_state = CHS_ENABLED;
			break;
		case CHS_WACK_CREQ:
		case CHS_WCON_CREQ:
			if (prim == CH_CONNECT_REQ) {
				if (ch->flags & CHF_BOTH_DIR)
					p->ch_state = ch->i_state = CHS_CONNECTED;
				else
					p->ch_state = ch->i_state = CHS_ENABLED;
			}
			break;
		case CHS_WACK_DREQ:
		case CHS_WCON_DREQ:
			if (prim == CH_DISCONNECT_REQ)
				p->ch_state = ch->i_state = CHS_CONNECTED;
			break;
		default:
			/* default is don't change state */
			p->ch_state = ch->i_state;
			break;
		}
		printd(("%s: %p: <- CH_ERROR_ACK\n", MOD_NAME, ch));
		putnext(ch->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_ENABLE_CON
 *  -----------------------------------
 *  Confirms to the channel user that the attached channel was enabled as
 *  requested.
 */
STATIC INLINE int
ch_enable_con(queue_t *q, struct ch *ch)
{
	mblk_t *mp;
	struct CH_enable_con *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->ch_primitive = CH_ENABLE_CON;
		switch (ch->i_state) {
		case CHS_WACK_EREQ:
		case CHS_WCON_EREQ:
			ch->i_state = CHS_ENABLED;
			break;
		default:
			swerr();
			freemsg(mp);
			return (-EFAULT);
		}
		printd(("%s: %p: <- CH_ENABLE_CON\n", MOD_NAME, ch));
		putnext(ch->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_CONNECT_CON
 *  -----------------------------------
 *  Confirms to the channel user that the enabled or connected channel was
 *  connected in the requested direction.
 */
STATIC INLINE int
ch_connect_con(queue_t *q, struct ch *ch, ulong flags)
{
	mblk_t *mp;
	struct CH_connect_con *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->ch_primitive = CH_CONNECT_CON;
		p->ch_conn_flags = flags;
		switch (ch->i_state) {
		case CHS_WCON_CREQ:
			ch->flags |= (flags & CHF_BOTH_DIR);
			ch->i_state = CHS_CONNECTED;
			break;
		default:
			swerr();
			freemsg(mp);
			return (-EFAULT);
		}
		printd(("%s: %p: <- CH_CONNECT_CON\n", MOD_NAME, ch));
		putnext(ch->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_DATA_IND
 *  -----------------------------------
 *  Indicates to the channel user data which was received on the channel.
 *  This is the non-preferred way of sending data to the channel user.  We
 *  should normally just send M_DATA blocks.
 */
STATIC INLINE int
ch_data_ind(queue_t *q, struct ch *ch)
{
	mblk_t *mp;
	struct CH_data_ind *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->ch_primitive = CH_DATA_IND;
		printd(("%s: %p: <- CH_DATA_IND\n", MOD_NAME, ch));
		putnext(ch->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_DISCONNECT_IND
 *  -----------------------------------
 *  Indicates to the channel user that an autonomous disconnection occured,
 *  the directions of the disconnection and the cause of the disconnection.
 *  This is normally used to indicate that both directions were disconnected
 *  due to hardware or transmission system failure (loss of carrier or carrier
 *  alarms).  This indication is rather optional, as carrier alarms are also
 *  indicated to management streams for the underlying devices.
 */
STATIC INLINE int
ch_disconnect_ind(queue_t *q, struct ch *ch, ulong flags)
{
	mblk_t *mp;
	struct CH_disconnect_ind *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->ch_primitive = CH_DISCONNECT_IND;
		p->ch_conn_flags = flags;
		p->ch_cause = 0;	/* FIXME */
		switch (ch->i_state) {
		case CHS_CONNECTED:
		case CHS_WACK_CREQ:
		case CHS_WCON_CREQ:
			ch->flags &= ~CHF_BOTH_DIR;
			ch->i_state = CHS_ENABLED;
			break;
		default:
			swerr();
			freemsg(mp);
			return (-EFAULT);
		}
		printd(("%s: %p: <- CH_DISCONNECT_IND\n", MOD_NAME, ch));
		putnext(ch->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_DISCONNECT_CON
 *  -----------------------------------
 *  Confirms to the channel user that the requested directions were
 *  disconnected as requested.
 */
STATIC INLINE int
ch_disconnect_con(queue_t *q, struct ch *ch, ulong flags)
{
	mblk_t *mp;
	struct CH_disconnect_con *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->ch_primitive = CH_DISCONNECT_CON;
		p->ch_conn_flags = flags;
		switch (ch->i_state) {
		case CHS_WCON_DREQ:
			ch->flags &= ~(flags & CHF_BOTH_DIR);
			if (ch->flags & CHF_BOTH_DIR)
				ch->i_state = CHS_CONNECTED;
			else
				ch->i_state = CHS_ENABLED;
			break;
		case CHS_UNUSABLE:
			ch->i_state = CHS_UNUSABLE;
			break;
		default:
			swerr();
			freemsg(mp);
			return (-EFAULT);
		}
		printd(("%s: %p: <- CH_DISCONNECT_CON\n", MOD_NAME, ch));
		putnext(ch->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_DISABLE_IND
 *  -----------------------------------
 *  Indicates to the channel user that an autonomous disabling of the channel
 *  has occured.  This is normally used to indicate a fatal error on the
 *  underlying stream, but is optional.  The channel user should detach and
 *  reattach the stream.
 */
STATIC INLINE int
ch_disable_ind(queue_t *q, struct ch *ch, long cause)
{
	mblk_t *mp;
	struct CH_disable_ind *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->ch_primitive = CH_DISABLE_IND;
		p->ch_cause = cause;
		ch->i_state = CHS_UNUSABLE;
		printd(("%s: %p: <- CH_DISABLE_IND\n", MOD_NAME, ch));
		putnext(ch->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CH_DISABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
ch_disable_con(queue_t *q, struct ch *ch)
{
	mblk_t *mp;
	struct CH_disable_con *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->ch_primitive = CH_DISABLE_CON;
		switch (ch->i_state) {
		case CHS_WACK_RREQ:
		case CHS_WCON_RREQ:
			ch->i_state = CHS_ATTACHED;
			ch->flags &= ~CHF_BOTH_DIR;
			break;
		case CHS_UNUSABLE:
			ch->i_state = CHS_UNUSABLE;
			ch->flags &= ~CHF_BOTH_DIR;
			break;
		default:
			swerr();
			freemsg(mp);
			return (-EFAULT);
		}
		printd(("%s: %p: <- CH_DISABLE_CON\n", MOD_NAME, ch));
		putnext(ch->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitive sent downstream
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  LMI_INFO_REQ
 *  -----------------------------------
 */
STATIC INLINE int
lmi_info_req(queue_t *q, struct ch *ch)
{
	mblk_t *mp;
	lmi_info_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->lmi_primitive = LMI_INFO_REQ;
		printd(("%s: %p: LMI_INFO_REQ ->\n", MOD_NAME, ch));
		putnext(ch->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 *  Requests that the provider attach the requesting stream to the specified
 *  PPA.  This is only valid for STYLE 2 devices.
 */
STATIC INLINE int
lmi_attach_req(queue_t *q, struct ch *ch, uchar *ppa_ptr, size_t ppa_len)
{
	mblk_t *mp;
	lmi_attach_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->lmi_primitive = LMI_ATTACH_REQ;
		if (ppa_len) {
			bcopy(ppa_ptr, mp->b_wptr, ppa_len);
			mp->b_wptr += ppa_len;
		}
		ch->i_state = CHS_WACK_AREQ;
		printd(("%s: %p: LMI_ATTACH_REQ ->\n", MOD_NAME, ch));
		putnext(ch->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_DETACH_REQ
 *  -----------------------------------
 *  Requests that the provider detach the requesting stream from the attached
 *  PPA.  This is only valid for STYLE 2 devices.
 */
STATIC INLINE int
lmi_detach_req(queue_t *q, struct ch *ch)
{
	mblk_t *mp;
	lmi_detach_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->lmi_primitive = LMI_DETACH_REQ;
		ch->i_state = CHS_WACK_UREQ;
		printd(("%s: %p: LMI_DETACH_REQ ->\n", MOD_NAME, ch));
		putnext(ch->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 *  Requests that the provider enable the attached stream to the specified
 *  remote address (if required).  Typically we do not have remote addresses
 *  for channels.
 */
STATIC INLINE int
lmi_enable_req(queue_t *q, struct ch *ch)
{
	mblk_t *mp;
	lmi_enable_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + ch->rem_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->lmi_primitive = LMI_ENABLE_REQ;
		if (ch->rem_len) {
			bcopy(ch->rem_ptr, mp->b_wptr, ch->rem_len);
			mp->b_wptr += ch->rem_len;
		}
		ch->i_state = CHS_WCON_EREQ;
		printd(("%s: %p: LMI_ENABLE_REQ ->\n", MOD_NAME, ch));
		putnext(ch->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 *  Requests that the provider disable the attached stream for the enabled
 *  remote address.
 */
STATIC INLINE int
lmi_disable_req(queue_t *q, struct ch *ch)
{
	mblk_t *mp;
	lmi_disable_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->lmi_primitive = LMI_DISABLE_REQ;
		ch->i_state = CHS_WCON_RREQ;
		printd(("%s: %p: LMI_DISABLE_REQ ->\n", MOD_NAME, ch));
		putnext(ch->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 *  Requests that the provider get, set or negotiate the specified options.
 */
STATIC INLINE int
lmi_optmgmt_req(queue_t *q, struct ch *ch, uchar *opt_ptr, size_t opt_len, ulong flags)
{
	mblk_t *mp;
	lmi_optmgmt_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->lmi_primitive = LMI_OPTMGMT_REQ;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		if (opt_len) {
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
		}
		printd(("%s: %p: LMI_OPTMGMT_REQ ->\n", MOD_NAME, ch));
		putnext(ch->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDL_BITS_FOR_TRANSMISSION_REQ
 *  -----------------------------------
 *  Requests transmission of the attached M_DATA blocks.  This is a
 *  non-preferred method for sending data to the lower level and is not used
 *  by this module.
 */
STATIC INLINE int
sdl_bits_for_transmission_req(queue_t *q, struct ch *ch, mblk_t *dp)
{
	mblk_t *mp;
	sdl_bits_for_transmission_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->sdl_primitive = SDL_BITS_FOR_TRANSMISSION_REQ;
		mp->b_cont = dp;
		printd(("%s: %p: SDL_BITS_FOR_TRANSMISSION_REQ ->\n", MOD_NAME, ch));
		putnext(ch->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDL_CONNECT_REQ
 *  -----------------------------------
 *  Requests that the provider connect the stream in the specified directions.
 */
STATIC INLINE int
sdl_connect_req(queue_t *q, struct ch *ch, ulong flags)
{
	mblk_t *mp;
	sdl_connect_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->sdl_primitive = SDL_CONNECT_REQ;
		p->sdl_flags = flags;
		ch->i_state = CHS_WCON_CREQ;
		printd(("%s: %p: SDL_CONNECT_REQ ->\n", MOD_NAME, ch));
		putnext(ch->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDL_DISCONNECT_REQ
 *  -----------------------------------
 */
STATIC INLINE int
sdl_disconnect_req(queue_t *q, struct ch *ch, ulong flags)
{
	mblk_t *mp;
	sdl_disconnect_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->sdl_primitive = SDL_DISCONNECT_REQ;
		p->sdl_flags = flags;
		ch->i_state = CHS_WCON_DREQ;
		printd(("%s: %p: SDL_DISCONNECT_REQ ->\n", MOD_NAME, ch));
		putnext(ch->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives from below
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  LMI_INFO_ACK
 *  -----------------------------------
 */
STATIC int
lmi_info_ack(queue_t *q, mblk_t *mp)
{
	/* discard */
	return (QR_DONE);
}

/*
 *  LMI_OK_ACK
 *  -----------------------------------
 */
STATIC int
lmi_ok_ack(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	return ch_ok_ack(q, ch);
}

/*
 *  LMI_ERROR_ACK
 *  -----------------------------------
 */
STATIC int
lmi_error_ack(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	long prim, error = -EFAULT;
	lmi_error_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (p->lmi_reason) {
	case LMI_BADADDRTYPE:
	case LMI_BADADDRESS:
	case LMI_BADPPA:
		error = CHBADADDR;
		break;
	case LMI_BADPRIM:
		error = CHBADPRIM;
		break;
	case LMI_NOTSUPP:
		error = CHNOTSUPP;
		break;
	case LMI_OUTSTATE:
		error = CHOUTSTATE;
		break;
	case LMI_SYSERR:
		error = -p->lmi_errno;
		break;
	}
	switch (ch->i_state) {
	case CHS_WACK_AREQ:
		prim = CH_ATTACH_REQ;
		break;
	case CHS_WACK_UREQ:
		prim = CH_DETACH_REQ;
		break;
	case CHS_WACK_EREQ:
	case CHS_WCON_EREQ:
		prim = CH_ENABLE_REQ;
		break;
	case CHS_WACK_RREQ:
	case CHS_WCON_RREQ:
		prim = CH_DISABLE_REQ;
		break;
	case CHS_WACK_CREQ:
	case CHS_WCON_CREQ:
		prim = CH_CONNECT_REQ;
		break;
	case CHS_WACK_DREQ:
	case CHS_WCON_DREQ:
		prim = CH_DISCONNECT_REQ;
		break;
	default:
		/* not expecting primitive */
		swerr();
		return (-EFAULT);
	}
	return ch_error_ack(q, ch, prim, error);
      emsgsize:
	swerr();
	return (-EFAULT);
}

/*
 *  LMI_ENABLE_CON
 *  -----------------------------------
 */
STATIC int
lmi_enable_con(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	return ch_enable_con(q, ch);
}

/*
 *  LMI_DISABLE_CON
 *  -----------------------------------
 */
STATIC int
lmi_disable_con(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	return ch_disable_con(q, ch);
}

/*
 *  LMI_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC int
lmi_optmgmt_ack(queue_t *q, mblk_t *mp)
{
	/* not expecting these */
	swerr();
	return (-EFAULT);
}

/*
 *  LMI_ERROR_IND
 *  -----------------------------------
 *  TODO: We need either an event indication or an error indication which
 *  indicates when a Red Alarm condition occurs so that we can notify of
 *  hardware failure.
 */
STATIC int
lmi_error_ind(queue_t *q, mblk_t *mp)
{
	/* not expecting these */
	swerr();
	return (-EFAULT);
}

/*
 *  LMI_STATS_IND
 *  -----------------------------------
 */
STATIC int
lmi_stats_ind(queue_t *q, mblk_t *mp)
{
	/* not expecting these */
	swerr();
	return (-EFAULT);
}

/*
 *  LMI_EVENT_IND
 *  -----------------------------------
 *  TODO: We need either an event indication or an error indication which
 *  indicates when a Red Alarm condition occurs so that we can notify of
 *  hardware failure.
 */
STATIC int
lmi_event_ind(queue_t *q, mblk_t *mp)
{
	/* not expecting these */
	swerr();
	return (-EFAULT);
}

/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int
ch_read(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	if (!mp || !msgdsize(mp))
		goto eproto;
	switch (ch->i_state) {
	case CHS_UNUSABLE:
	case CHS_ENABLED:
		goto discard;
	case CHS_WACK_CREQ:
	case CHS_WCON_CREQ:
	case CHS_WACK_DREQ:
	case CHS_WCON_DREQ:
	case CHS_CONNECTED:
		if (!(ch->flags & CHF_RX_DIR))
			goto discard;
		putnext(ch->oq, mp);
		return (QR_ABSORBED);
	}
	goto eproto;
      eproto:
	swerr();
	return (-EPROTO);
      discard:
	rare();
	return (QR_DONE);
}

/*
 *  SDL_RECEIVED_BITS_IND
 *  -----------------------------------
 */
STATIC int
sdl_received_bits_ind(queue_t *q, mblk_t *mp)
{
	sdl_received_bits_ind_t *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if ((err = ch_read(q, mp->b_cont)) == QR_ABSORBED)
		return (QR_TRIMMED);
	return (err);
      emsgsize:
	swerr();
	return (-EFAULT);
}

/*
 *  SDL_DISCONNECT_IND
 *  -----------------------------------
 */
STATIC int
sdl_disconnect_ind(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	sdl_disconnect_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (ch->i_state != CHS_CONNECTED && ch->i_state != CHS_WACK_CREQ
	    && ch->i_state != CHS_WCON_CREQ)
		goto outstate;
	return ch_disconnect_ind(q, ch, CHF_BOTH_DIR);
      emsgsize:
	swerr();
	return (-EFAULT);
      outstate:
	swerr();
	return (-EPROTO);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitives from above
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_DATA
 *  -----------------------------------
 *  Requests that the channel provider place the data contained in the M_DATA
 *  block onto the transmit stream of the channel.  This is the preferred
 *  method of sending data to the channel.
 */
STATIC int
ch_write(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	if (!mp || !msgdsize(mp))
		goto eproto;
	switch (ch->i_state) {
	case CHS_UNUSABLE:
	case CHS_ENABLED:
		goto discard;
	case CHS_WACK_CREQ:
	case CHS_WCON_CREQ:
	case CHS_WACK_DREQ:
	case CHS_WCON_DREQ:
	case CHS_CONNECTED:
		if (!(ch->flags & CHF_TX_DIR))
			goto discard;
		putnext(ch->iq, mp);
		return (QR_ABSORBED);
	}
	goto eproto;
      eproto:
	return m_error(q, ch, -EPROTO);
      discard:
	rare();
	return (QR_DONE);
}

/*
 *  CH_INFO_REQ
 *  -----------------------------------
 *  Requests that the channel provider return information about the provider
 *  and the attached channel (if any).
 */
STATIC int
ch_info_req(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	struct CH_info_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (ch->i_state == CHS_UNUSABLE)
		goto outstate;
	return ch_info_ack(q, ch);
      outstate:
	return ch_error_ack(q, ch, p->ch_primitive, CHOUTSTATE);
      emsgsize:
	return ch_error_ack(q, ch, p->ch_primitive, -EMSGSIZE);
}

/*
 *  CH_OPTMGMT_REQ
 *  -----------------------------------
 *  Requests that the channel provider set, get or negotiate provider or
 *  attached channel options.
 */
STATIC int
ch_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	struct CH_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	union CH_parms *o;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr > mp->b_rptr + p->ch_opt_offset + p->ch_opt_length)
		goto emsgsize;
	switch (ch->i_state) {
	case CHS_DETACHED:
	case CHS_UNUSABLE:
	case CHS_WACK_AREQ:
	case CHS_WACK_UREQ:
		break;
	default:
		goto outstate;
	}
	if (p->ch_opt_length) {
		if (p->ch_opt_length < sizeof(o->cp_type))
			goto badopt;
		o = (typeof(o)) (mp->b_rptr + p->ch_opt_offset);
		switch (o->cp_type) {
		case CH_PARMS_CIRCUIT:
			if (p->ch_opt_length < sizeof(o->circuit))
				goto badparm;
			switch (p->ch_mgmt_flags) {
			case 0:
			case CH_NEGOTIATE:
				/* negotiate current */
			case CH_SET:
				/* set current */
				ch->config.block_size = o->circuit.cp_block_size;
				ch->config.encoding = o->circuit.cp_encoding;
				ch->config.sample_size = o->circuit.cp_sample_size;
				ch->config.rate = o->circuit.cp_rate;
				ch->config.tx_channels = o->circuit.cp_tx_channels;
				ch->config.rx_channels = o->circuit.cp_rx_channels;
				ch->config.opt_flags = o->circuit.cp_opt_flags;
				return ch_optmgmt_ack(q, ch, (uchar *) o, sizeof(o->circuit),
						      p->ch_mgmt_flags);
			case CH_DEFAULT:
				/* set default */
				ch_default.block_size = o->circuit.cp_block_size;
				ch_default.encoding = o->circuit.cp_encoding;
				ch_default.sample_size = o->circuit.cp_sample_size;
				ch_default.rate = o->circuit.cp_rate;
				ch_default.tx_channels = o->circuit.cp_tx_channels;
				ch_default.rx_channels = o->circuit.cp_rx_channels;
				ch_default.opt_flags = o->circuit.cp_opt_flags;
				return ch_optmgmt_ack(q, ch, (uchar *) o, sizeof(o->circuit),
						      p->ch_mgmt_flags);
			}
		}
		goto badparmtype;
	} else {
		union CH_parms parms;
		o = &parms;
		/* no options */
		switch (p->ch_mgmt_flags) {
		case CH_GET:
			/* get current */
			o->cp_type = CH_PARMS_CIRCUIT;
			o->circuit.cp_block_size = ch->config.block_size;
			o->circuit.cp_encoding = ch->config.encoding;
			o->circuit.cp_sample_size = ch->config.sample_size;
			o->circuit.cp_rate = ch->config.rate;
			o->circuit.cp_tx_channels = ch->config.tx_channels;
			o->circuit.cp_rx_channels = ch->config.rx_channels;
			o->circuit.cp_opt_flags = ch->config.opt_flags;
			return ch_optmgmt_ack(q, ch, (uchar *) o, sizeof(o->circuit),
					      p->ch_mgmt_flags);
		case 0:
		case CH_DEFAULT:
			/* get default */
			o->cp_type = CH_PARMS_CIRCUIT;
			o->circuit.cp_block_size = ch_default.block_size;
			o->circuit.cp_encoding = ch_default.encoding;
			o->circuit.cp_sample_size = ch_default.sample_size;
			o->circuit.cp_rate = ch_default.rate;
			o->circuit.cp_tx_channels = ch_default.tx_channels;
			o->circuit.cp_rx_channels = ch_default.rx_channels;
			o->circuit.cp_opt_flags = ch_default.opt_flags;
			return ch_optmgmt_ack(q, ch, (uchar *) o, sizeof(o->circuit),
					      p->ch_mgmt_flags);
		}
	}
	goto badflag;
      badflag:
	return ch_error_ack(q, ch, p->ch_primitive, CHBADFLAG);
      badparmtype:
	return ch_error_ack(q, ch, p->ch_primitive, CHBADPARMTYPE);
      badparm:
	return ch_error_ack(q, ch, p->ch_primitive, CHBADPARM);
      badopt:
	return ch_error_ack(q, ch, p->ch_primitive, CHBADOPT);
      outstate:
	return ch_error_ack(q, ch, p->ch_primitive, CHOUTSTATE);
      emsgsize:
	return ch_error_ack(q, ch, p->ch_primitive, -EMSGSIZE);
}

/*
 *  CH_ATTACH_REQ
 *  -----------------------------------
 *  Requests that the channel provider attached the requesting stream to the
 *  specified channel (specified in the channel address).
 */
STATIC int
ch_attach_req(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	struct CH_attach_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr > mp->b_rptr + p->ch_addr_offset + p->ch_addr_length)
		goto emsgsize;
	if (ch->i_state != CHS_DETACHED)
		goto outstate;
	return lmi_attach_req(q, ch, mp->b_rptr + p->ch_addr_offset, p->ch_addr_length);
      outstate:
	return ch_error_ack(q, ch, p->ch_primitive, CHOUTSTATE);
      emsgsize:
	return ch_error_ack(q, ch, p->ch_primitive, -EMSGSIZE);
}

/*
 *  CH_ENABLE_REQ
 *  -----------------------------------
 *  Requests that the channel provider enable the attached channel.
 */
STATIC int
ch_enable_req(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	struct CH_enable_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (ch->i_state != CHS_ATTACHED)
		goto outstate;
	return lmi_enable_req(q, ch);
      outstate:
	return ch_error_ack(q, ch, p->ch_primitive, CHOUTSTATE);
      emsgsize:
	return ch_error_ack(q, ch, p->ch_primitive, -EMSGSIZE);
}

/*
 *  CH_CONNECT_REQ
 *  -----------------------------------
 *  Requests that the channel provider connect the specified direction on the
 *  attached and enabled channel.
 */
STATIC int
ch_connect_req(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	struct CH_connect_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (ch->i_state) {
	case CHS_ENABLED:
	case CHS_CONNECTED:
		if (!(p->ch_conn_flags & CHF_BOTH_DIR))
			goto badflag;
		if (((ch->flags & p->ch_conn_flags) & CHF_BOTH_DIR))
			goto badflag;
		if ((err = sdl_connect_req(q, ch, p->ch_conn_flags & CHF_BOTH_DIR)))
			return (err);
		/* fall through */
	case CHS_WCON_CREQ:
		/* automatic confirmation */
		return ch_connect_con(q, ch, p->ch_conn_flags & CHF_BOTH_DIR);
	}
	goto outstate;
      badflag:
	return ch_error_ack(q, ch, p->ch_primitive, CHBADFLAG);
      outstate:
	return ch_error_ack(q, ch, p->ch_primitive, CHOUTSTATE);
      emsgsize:
	return ch_error_ack(q, ch, p->ch_primitive, -EMSGSIZE);
}

/*
 *  CH_DATA_REQ
 *  -----------------------------------
 *  Requests that the provider send data on the connected channel.
 */
STATIC int
ch_data_req(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	struct CH_data_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eproto;
	if ((err = ch_write(q, mp->b_cont)) == QR_ABSORBED)
		return (QR_TRIMMED);
	return (err);
      eproto:
	return m_error(q, ch, -EPROTO);
}

/*
 *  CH_DISCONNECT_REQ
 *  -----------------------------------
 *  Requests that the provider disconnect the specified direction for the
 *  attached, enabled and connected channel.
 */
STATIC int
ch_disconnect_req(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	struct CH_disconnect_req *p = (typeof(p)) mp->b_rptr;
	int err;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (ch->i_state) {
	case CHS_CONNECTED:
		if (!((ch->flags & p->ch_conn_flags) & CHF_BOTH_DIR))
			goto badflag;
		if ((err = sdl_disconnect_req(q, ch, p->ch_conn_flags & CHF_BOTH_DIR)))
			return (err);
		/* fall through */
	case CHS_WCON_DREQ:
	case CHS_UNUSABLE:
		/* automatic confirmation */
		return ch_disconnect_con(q, ch, p->ch_conn_flags & CHF_BOTH_DIR);
	}
	goto outstate;
      badflag:
	return ch_error_ack(q, ch, p->ch_primitive, CHBADFLAG);
      outstate:
	return ch_error_ack(q, ch, p->ch_primitive, CHOUTSTATE);
      emsgsize:
	return ch_error_ack(q, ch, p->ch_primitive, -EMSGSIZE);
}

/*
 *  CH_DISABLE_REQ
 *  -----------------------------------
 *  Requests that the provider disable the attached and enabled channel.
 */
STATIC int
ch_disable_req(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	struct CH_disable_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (ch->i_state) {
	case CHS_ENABLED:
		return lmi_disable_req(q, ch);
	case CHS_UNUSABLE:
		return ch_disable_con(q, ch);
	}
	goto outstate;
      outstate:
	return ch_error_ack(q, ch, p->ch_primitive, CHOUTSTATE);
      emsgsize:
	return ch_error_ack(q, ch, p->ch_primitive, -EMSGSIZE);
}

/*
 *  CH_DETACH_REQ
 *  -----------------------------------
 *  Requests that the provider detach the attached channel.
 */
STATIC int
ch_detach_req(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	struct CH_detach_req *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (ch->i_state) {
	case CHS_ATTACHED:
		return lmi_detach_req(q, ch);
	case CHS_UNUSABLE:
		return ch_ok_ack(q, ch);
	}
	goto outstate;
      outstate:
	return ch_error_ack(q, ch, p->ch_primitive, CHOUTSTATE);
      emsgsize:
	return ch_error_ack(q, ch, p->ch_primitive, -EMSGSIZE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  IO CONTROLS
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  CH_IOCGCONFIG
 *  -----------------------------------
 */
STATIC int
ch_iocgconfig(struct ch *ch, struct ch_config *p)
{
	*p = ch->config;
	return (0);
}

/*
 *  CH_IOCSCONFIG
 *  -----------------------------------
 */
STATIC int ch_ioctconfig(struct ch *, struct ch_config *);
STATIC int
ch_iocsconfig(struct ch *ch, struct ch_config *p)
{
	int err;
	if ((err = ch_ioctconfig(ch, p)))
		return (err);
	ch->config = *p;
	return (0);
}

/*
 *  CH_IOCTCONFIG
 *  -----------------------------------
 */
STATIC int
ch_ioctconfig(struct ch *ch, struct ch_config *p)
{
	if (p->block_size <= 0 || p->block_size & 0x7)
		goto einval;
	switch (p->encoding) {
	case CH_ENCODING_NONE:
	case CH_ENCODING_G711_PCM_A:
	case CH_ENCODING_G711_PCM_U:
	case CH_ENCODING_S8:
	case CH_ENCODING_U8:
		break;
	default:
		goto einval;
	}
	if (p->sample_size <= 0 || p->sample_size > 8)
		goto einval;
	if (p->rate != 8000)
		goto einval;
	if (p->tx_channels != 1)
		goto einval;
	if (p->rx_channels != 1)
		goto einval;
	return (0);
      einval:
	return (-EINVAL);
}

/*
 *  CH_IOCCCONFIG
 *  -----------------------------------
 */
STATIC int
ch_ioccconfig(struct ch *ch, struct ch_config *p)
{
	*p = ch->config = ch_default;
	return (0);
}

/*
 *  CH_IOCGSTATEM
 *  -----------------------------------
 */
STATIC int
ch_iocgstatem(struct ch *ch, struct ch_statem *p)
{
	p->state = ch->i_state;
	p->flags = ch->flags;
	return (0);
}

/*
 *  CH_IOCCMRESET
 *  -----------------------------------
 */
STATIC int
ch_ioccmreset(struct ch *ch, struct ch_statem *p)
{
	ch->i_state = p->state;
	ch->flags = p->flags;
	return (0);
}

/*
 *  CH_IOCGSTATSP
 *  -----------------------------------
 */
STATIC int
ch_iocgstatsp(struct ch *ch, struct ch_stats *p)
{
	*p = ch->statsp;
	return (0);
}

/*
 *  CH_IOCSSTATSP
 *  -----------------------------------
 */
STATIC int
ch_iocsstatsp(struct ch *ch, struct ch_stats *p)
{
	ch->statsp = *p;
	return (0);
}

/*
 *  CH_IOCGSTATS
 *  -----------------------------------
 */
STATIC int
ch_iocgstats(struct ch *ch, struct ch_stats *p)
{
	*p = ch->stats;
	return (0);
}

/*
 *  CH_IOCCSTATS
 *  -----------------------------------
 */
STATIC int
ch_ioccstats(struct ch *ch, struct ch_stats *p)
{
	bzero(&ch->stats, sizeof(ch->stats));
	return (0);
}

/*
 *  CH_IOCGNOTIFY
 *  -----------------------------------
 */
STATIC int
ch_iocgnotify(struct ch *ch, struct ch_notify *p)
{
	*p = ch->notify;
	return (0);
}

/*
 *  CH_IOCSNOTIFY
 *  -----------------------------------
 */
STATIC int
ch_iocsnotify(struct ch *ch, struct ch_notify *p)
{
	ch->notify.events |= p->events;
	return (0);
}

/*
 *  CH_IOCCNOTIFY
 *  -----------------------------------
 */
STATIC int
ch_ioccnotify(struct ch *ch, struct ch_notify *p)
{
	ch->notify.events &= ~p->events;
	return (0);
}

/*
 *  SDL_IOCGCONFIG
 *  -----------------------------------
 */
STATIC int
sdl_iocgconfig_req(queue_t *q)
{
	struct ch *ch = CH_PRIV(q);
	mblk_t *mp, *dp;
	struct iocblk *iocp;
	if ((mp = ss7_allocb(q, sizeof(*iocp), BPRI_MED))) {
		mp->b_datap->db_type = M_IOCTL;
		bzero(mp->b_wptr, sizeof(*iocp));
		iocp = (typeof(iocp)) mp->b_wptr++;
		iocp->ioc_cmd = SDL_IOCGCONFIG;
		iocp->ioc_id = (uint) (long) ch; /* XXX */
		iocp->ioc_count = sizeof(sdl_config_t);
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
		if ((dp = ss7_allocb(q, sizeof(sdl_config_t), BPRI_HI))) {
			dp->b_datap->db_type = M_DATA;
			bzero(dp->b_wptr, sizeof(sdl_config_t));
			dp->b_wptr += sizeof(sdl_config_t);
			mp->b_cont = dp;
			putnext(ch->iq, mp);
			return (QR_DONE);
		}
		freemsg(mp);
	}
	rare();
	return (-ENOBUFS);
}
STATIC int
sdl_iocgconfig_ack(struct ch *ch, sdl_config_t * p)
{
	/* ignore */
	switch (ch->i_state) {
	case CHS_UNINIT:
		switch (p->iftype) {
		case SDL_TYPE_DS0:
			ch->config.sample_size = 8;
			ch->config.rate = 8000;
			break;
		case SDL_TYPE_DS0A:
			ch->config.sample_size = 7;
			ch->config.rate = 8000;
			break;
		default:
			goto einval;
		}
		switch (p->ifgtype) {
		case SDL_GTYPE_E1:
			ch->config.encoding = CH_ENCODING_G711_PCM_A;
			break;
		case SDL_GTYPE_T1:
			ch->config.encoding = CH_ENCODING_G711_PCM_U;
			break;
		default:
			goto einval;
		}
		switch (p->ifmode) {
		case SDL_MODE_NONE:
		case SDL_MODE_PEER:
			break;
		default:
			goto einval;
		}
		switch (p->ifgmode) {
		case SDL_GMODE_NONE:
			break;
		default:
			goto einval;
		}
		switch (p->ifcoding) {
		case SDL_CODING_AMI:
		case SDL_CODING_HDB3:
		case SDL_CODING_B8ZS:
			break;
		default:
			goto einval;
		}
		switch (p->ifframing) {
		case SDL_FRAMING_CCS:
		case SDL_FRAMING_CAS:
		case SDL_FRAMING_SF:
		case SDL_FRAMING_ESF:
			break;
		default:
			goto einval;
		}
		if (p->ifalarms & SDL_ALARM_RED) {
		}
		ch->i_state = CHS_DETACHED;
		return (QR_DONE);
	default:
		swerr();
		return (-EFAULT);
	}
      einval:
	swerr();
	ch->i_state = CHS_UNUSABLE;
	return m_error(ch->oq, ch, -EFAULT);
}
STATIC int
sdl_iocgconfig_nak(struct ch *ch, sdl_config_t * p)
{
	swerr();
	return m_error(ch->oq, ch, -EFAULT);
}

/*
 *  SDL_IOCSSTATSP
 *  -----------------------------------
 */
#if 0
STATIC int
sdl_iocsstatsp_req(queue_t *q, sdl_stats_t * p)
{
	struct ch *ch = CH_PRIV(q);
	mblk_t *mp, *dp;
	struct iocblk *iocp;
	if ((mp = ss7_allocb(q, sizeof(*iocp), BPRI_MED))) {
		mp->b_datap->db_type = M_IOCTL;
		bzero(mp->b_wptr, sizeof(*iocp));
		iocp = (typeof(iocp)) mp->b_wptr++;
		iocp->ioc_cmd = SDL_IOCSSTATSP;
		iocp->ioc_id = (uint) (long) ch; /* XXX */
		iocp->ioc_count = sizeof(sdl_stats_t);
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
		if ((dp = ss7_allocb(q, sizeof(sdl_stats_t), BPRI_HI))) {
			dp->b_datap->db_type = M_DATA;
			bcopy(p, dp->b_wptr, sizeof(sdl_stats_t));
			dp->b_wptr += sizeof(sdl_stats_t);
			mp->b_cont = dp;
			putnext(ch->iq, mp);
			return (QR_DONE);
		}
		freemsg(mp);
	}
	rare();
	return (-ENOBUFS);
}
#endif
STATIC int
sdl_iocsstatsp_ack(struct ch *ch, sdl_stats_t * p)
{
	/* ignore */
	return (QR_DONE);
}
STATIC int
sdl_iocsstatsp_nak(struct ch *ch, sdl_stats_t * p)
{
	swerr();
	return m_error(ch->oq, ch, -EFAULT);
}

/*
 *  SDL_IOCGSTATS
 *  -----------------------------------
 */
#if 0
STATIC int
sdl_iocgstats_req(queue_t *q)
{
	struct ch *ch = CH_PRIV(q);
	mblk_t *mp, *dp;
	struct iocblk *iocp;
	if ((mp = ss7_allocb(q, sizeof(*iocp), BPRI_MED))) {
		mp->b_datap->db_type = M_IOCTL;
		bzero(mp->b_wptr, sizeof(*iocp));
		iocp = (typeof(iocp)) mp->b_wptr++;
		iocp->ioc_cmd = SDL_IOCGSTATS;
		iocp->ioc_id = (uint) (long) ch; /* XXX */
		iocp->ioc_count = sizeof(sdl_stats_t);
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
		if ((dp = ss7_allocb(q, sizeof(sdl_stats_t), BPRI_HI))) {
			dp->b_datap->db_type = M_DATA;
			bzero(dp->b_wptr, sizeof(sdl_stats_t));
			dp->b_wptr += sizeof(sdl_stats_t);
			mp->b_cont = dp;
			putnext(ch->iq, mp);
			return (QR_DONE);
		}
		freemsg(mp);
	}
	rare();
	return (-ENOBUFS);
}
#endif
STATIC int
sdl_iocgstats_ack(struct ch *ch, sdl_stats_t * p)
{
	/* ignore */
	return (QR_DONE);
}
STATIC int
sdl_iocgstats_nak(struct ch *ch, sdl_stats_t * p)
{
	swerr();
	return m_error(ch->oq, ch, -EFAULT);
}

/*
 *  SDL_IOCCSTATS
 *  -----------------------------------
 */
#if 0
STATIC int
sdl_ioccstats_req(queue_t *q)
{
	struct ch *ch = CH_PRIV(q);
	mblk_t *mp, *dp;
	struct iocblk *iocp;
	if ((mp = ss7_allocb(q, sizeof(*iocp), BPRI_MED))) {
		mp->b_datap->db_type = M_IOCTL;
		bzero(mp->b_wptr, sizeof(*iocp));
		iocp = (typeof(iocp)) mp->b_wptr++;
		iocp->ioc_cmd = SDL_IOCCSTATS;
		iocp->ioc_id = (uint) (long) ch; /* XXX */
		iocp->ioc_count = sizeof(sdl_stats_t);
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
		if ((dp = ss7_allocb(q, sizeof(sdl_stats_t), BPRI_HI))) {
			dp->b_datap->db_type = M_DATA;
			bzero(dp->b_wptr, sizeof(sdl_stats_t));
			dp->b_wptr += sizeof(sdl_stats_t);
			mp->b_cont = dp;
			putnext(ch->iq, mp);
			return (QR_DONE);
		}
		freemsg(mp);
	}
	rare();
	return (-ENOBUFS);
}
#endif
STATIC int
sdl_ioccstats_ack(struct ch *ch, sdl_stats_t * p)
{
	/* ignore */
	return (QR_DONE);
}
STATIC int
sdl_ioccstats_nak(struct ch *ch, sdl_stats_t * p)
{
	swerr();
	return m_error(ch->oq, ch, -EFAULT);
}

/*
 *  SDL_IOCSNOTIFY
 *  -----------------------------------
 */
#if 0
STATIC int
sdl_iocsnotify_req(queue_t *q, sdl_notify_t * p)
{
	struct ch *ch = CH_PRIV(q);
	mblk_t *mp, *dp;
	struct iocblk *iocp;
	if ((mp = ss7_allocb(q, sizeof(*iocp), BPRI_MED))) {
		mp->b_datap->db_type = M_IOCTL;
		bzero(mp->b_wptr, sizeof(*iocp));
		iocp = (typeof(iocp)) mp->b_wptr++;
		iocp->ioc_cmd = SDL_IOCSNOTIFY;
		iocp->ioc_id = (uint) (long) ch; /* XXX */
		iocp->ioc_count = sizeof(sdl_notify_t);
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
		if ((dp = ss7_allocb(q, sizeof(sdl_notify_t), BPRI_HI))) {
			dp->b_datap->db_type = M_DATA;
			bcopy(p, dp->b_wptr, sizeof(sdl_notify_t));
			dp->b_wptr += sizeof(sdl_notify_t);
			mp->b_cont = dp;
			putnext(ch->iq, mp);
			return (QR_DONE);
		}
		freemsg(mp);
	}
	rare();
	return (-ENOBUFS);
}
#endif
STATIC int
sdl_iocsnotify_ack(struct ch *ch, sdl_notify_t * p)
{
	/* ignore */
	return (QR_DONE);
}
STATIC int
sdl_iocsnotify_nak(struct ch *ch, sdl_notify_t * p)
{
	swerr();
	return m_error(ch->oq, ch, -EFAULT);
}

/*
 *  SDL_IOCCNOTIFY
 *  -----------------------------------
 */
#if 0
STATIC int
sdl_ioccnotify_req(queue_t *q, sdl_notify_t * p)
{
	struct ch *ch = CH_PRIV(q);
	mblk_t *mp, *dp;
	struct iocblk *iocp;
	if ((mp = ss7_allocb(q, sizeof(*iocp), BPRI_MED))) {
		mp->b_datap->db_type = M_IOCTL;
		bzero(mp->b_wptr, sizeof(*iocp));
		iocp = (typeof(iocp)) mp->b_wptr++;
		iocp->ioc_cmd = SDL_IOCCNOTIFY;
		iocp->ioc_id = (uint) (long) ch; /* XXX */
		iocp->ioc_count = sizeof(sdl_notify_t);
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
		if ((dp = ss7_allocb(q, sizeof(sdl_notify_t), BPRI_HI))) {
			dp->b_datap->db_type = M_DATA;
			bcopy(p, dp->b_wptr, sizeof(sdl_notify_t));
			dp->b_wptr += sizeof(sdl_notify_t);
			mp->b_cont = dp;
			putnext(ch->iq, mp);
			return (QR_DONE);
		}
		freemsg(mp);
	}
	rare();
	return (-ENOBUFS);
}
#endif
STATIC int
sdl_ioccnotify_ack(struct ch *ch, sdl_notify_t * p)
{
	/* ignore */
	return (QR_DONE);
}
STATIC int
sdl_ioccnotify_nak(struct ch *ch, sdl_notify_t * p)
{
	swerr();
	return m_error(ch->oq, ch, -EFAULT);
}

/*
 *  =========================================================================
 *
 *  STREAMS Message Handling
 *
 *  =========================================================================
 *
 *  M_IOCTL Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
ch_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int ret = 0;
	switch (type) {
	case CH_IOC_MAGIC:
	{
		/* These are CH IOCTLs. */
		if (count < size || !arg) {
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(CH_IOCGCONFIG):
			ret = ch_iocgconfig(ch, arg);
			break;
		case _IOC_NR(CH_IOCSCONFIG):
			ret = ch_iocsconfig(ch, arg);
			break;
		case _IOC_NR(CH_IOCTCONFIG):
			ret = ch_ioctconfig(ch, arg);
			break;
		case _IOC_NR(CH_IOCCCONFIG):
			ret = ch_ioccconfig(ch, arg);
			break;
		case _IOC_NR(CH_IOCGSTATEM):
			ret = ch_iocgstatem(ch, arg);
			break;
		case _IOC_NR(CH_IOCCMRESET):
			ret = ch_ioccmreset(ch, arg);
			break;
		case _IOC_NR(CH_IOCGSTATSP):
			ret = ch_iocgstatsp(ch, arg);
			break;
		case _IOC_NR(CH_IOCSSTATSP):
			ret = ch_iocsstatsp(ch, arg);
			break;
		case _IOC_NR(CH_IOCGSTATS):
			ret = ch_iocgstats(ch, arg);
			break;
		case _IOC_NR(CH_IOCCSTATS):
			ret = ch_ioccstats(ch, arg);
			break;
		case _IOC_NR(CH_IOCGNOTIFY):
			ret = ch_iocgnotify(ch, arg);
			break;
		case _IOC_NR(CH_IOCSNOTIFY):
			ret = ch_iocsnotify(ch, arg);
			break;
		case _IOC_NR(CH_IOCCNOTIFY):
			ret = ch_ioccnotify(ch, arg);
			break;
		default:
			ptrace(("%s: ERROR: Unsupported CH ioctl %d\n", MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	default:
		/* pass down what we don't understand */
		return (QR_PASSALONG);
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
 *  -------------------------------------------------------------------------
 *
 *  M_IOCACK, M_IOCNAK Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
ch_r_iocack(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	if (iocp->ioc_id != (uint) (long) ch) /* XXX */
		/* we didn't send it */
		return (QR_PASSALONG);
	switch (type) {
	case SDL_IOC_MAGIC:
		if (count < size || !arg)
			goto efault;
		switch (nr) {
		case _IOC_NR(SDL_IOCGCONFIG):
			return sdl_iocgconfig_ack(ch, arg);
		case _IOC_NR(SDL_IOCSSTATSP):
			return sdl_iocsstatsp_ack(ch, arg);
		case _IOC_NR(SDL_IOCGSTATS):
			return sdl_iocgstats_ack(ch, arg);
		case _IOC_NR(SDL_IOCCSTATS):
			return sdl_ioccstats_ack(ch, arg);
		case _IOC_NR(SDL_IOCSNOTIFY):
			return sdl_iocsnotify_ack(ch, arg);
		case _IOC_NR(SDL_IOCCNOTIFY):
			return sdl_ioccnotify_ack(ch, arg);
		case _IOC_NR(SDL_IOCSCONFIG):
		case _IOC_NR(SDL_IOCTCONFIG):
		case _IOC_NR(SDL_IOCCCONFIG):
		case _IOC_NR(SDL_IOCGSTATEM):
		case _IOC_NR(SDL_IOCCMRESET):
		case _IOC_NR(SDL_IOCGSTATSP):
		case _IOC_NR(SDL_IOCGNOTIFY):
			break;
		}
		goto efault;
	}
      efault:
	swerr();
	return (-EFAULT);
}
STATIC int
ch_r_iocnak(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	if (iocp->ioc_id != (uint) (long) ch) /* XXX */
		/* we didn't send it */
		return (QR_PASSALONG);
	switch (type) {
	case SDL_IOC_MAGIC:
		if (count < size || !arg)
			goto efault;
		switch (nr) {
		case _IOC_NR(SDL_IOCGCONFIG):
			return sdl_iocgconfig_nak(ch, arg);
		case _IOC_NR(SDL_IOCSSTATSP):
			return sdl_iocsstatsp_nak(ch, arg);
		case _IOC_NR(SDL_IOCGSTATS):
			return sdl_iocgstats_nak(ch, arg);
		case _IOC_NR(SDL_IOCCSTATS):
			return sdl_ioccstats_nak(ch, arg);
		case _IOC_NR(SDL_IOCSNOTIFY):
			return sdl_iocsnotify_nak(ch, arg);
		case _IOC_NR(SDL_IOCCNOTIFY):
			return sdl_ioccnotify_nak(ch, arg);
		case _IOC_NR(SDL_IOCSCONFIG):
		case _IOC_NR(SDL_IOCTCONFIG):
		case _IOC_NR(SDL_IOCCCONFIG):
		case _IOC_NR(SDL_IOCGSTATEM):
		case _IOC_NR(SDL_IOCCMRESET):
		case _IOC_NR(SDL_IOCGSTATSP):
		case _IOC_NR(SDL_IOCGNOTIFY):
			break;
		}
		goto efault;
	}
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  Primitives from MG to CH.
 *  -----------------------------------
 */
STATIC int
ch_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct ch *ch = CH_PRIV(q);
	ulong prim;
	(void) ch;
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case CH_INFO_REQ:
		printd(("%s: %p: -> CH_INFO_REQ\n", MOD_NAME, ch));
		rtn = ch_info_req(q, mp);
		break;
	case CH_OPTMGMT_REQ:
		printd(("%s: %p: -> CH_OPTMGMT_REQ\n", MOD_NAME, ch));
		rtn = ch_optmgmt_req(q, mp);
		break;
	case CH_ATTACH_REQ:
		printd(("%s: %p: -> CH_ATTACH_REQ\n", MOD_NAME, ch));
		rtn = ch_attach_req(q, mp);
		break;
	case CH_ENABLE_REQ:
		printd(("%s: %p: -> CH_ENABLE_REQ\n", MOD_NAME, ch));
		rtn = ch_enable_req(q, mp);
		break;
	case CH_CONNECT_REQ:
		printd(("%s: %p: -> CH_CONNECT_REQ\n", MOD_NAME, ch));
		rtn = ch_connect_req(q, mp);
		break;
	case CH_DATA_REQ:
		printd(("%s: %p: -> CH_DATA_REQ\n", MOD_NAME, ch));
		rtn = ch_data_req(q, mp);
		break;
	case CH_DISCONNECT_REQ:
		printd(("%s: %p: -> CH_DISCONNECT_REQ\n", MOD_NAME, ch));
		rtn = ch_disconnect_req(q, mp);
		break;
	case CH_DISABLE_REQ:
		printd(("%s: %p: -> CH_DISABLE_REQ\n", MOD_NAME, ch));
		rtn = ch_disable_req(q, mp);
		break;
	case CH_DETACH_REQ:
		printd(("%s: %p: -> CH_DETACH_REQ\n", MOD_NAME, ch));
		rtn = ch_detach_req(q, mp);
		break;
	default:
		printd(("%s: %p: -> CH_????\n", MOD_NAME, ch));
		rtn = ch_error_ack(q, ch, prim, CHNOTSUPP);
		break;
	}
	return (rtn);
}

/*
 *  Primitives from SDL to CH.
 *  -----------------------------------
 */
STATIC int
ch_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct ch *ch = CH_PRIV(q);
	(void) ch;
	switch (*((ulong *) mp->b_rptr)) {
	case LMI_INFO_ACK:
		printd(("%s: %p: LMI_INFO_ACK <-\n", MOD_NAME, ch));
		rtn = lmi_info_ack(q, mp);
		break;
	case LMI_OK_ACK:
		printd(("%s: %p: LMI_OK_ACK <-\n", MOD_NAME, ch));
		rtn = lmi_ok_ack(q, mp);
		break;
	case LMI_ERROR_ACK:
		printd(("%s: %p: LMI_ERROR_ACK <-\n", MOD_NAME, ch));
		rtn = lmi_error_ack(q, mp);
		break;
	case LMI_ENABLE_CON:
		printd(("%s: %p: LMI_ENABLE_CON <-\n", MOD_NAME, ch));
		rtn = lmi_enable_con(q, mp);
		break;
	case LMI_DISABLE_CON:
		printd(("%s: %p: LMI_DISABLE_CON <-\n", MOD_NAME, ch));
		rtn = lmi_disable_con(q, mp);
		break;
	case LMI_OPTMGMT_ACK:
		printd(("%s: %p: LMI_OPTMGMT_ACK <-\n", MOD_NAME, ch));
		rtn = lmi_optmgmt_ack(q, mp);
		break;
	case LMI_ERROR_IND:
		printd(("%s: %p: LMI_ERROR_IND <-\n", MOD_NAME, ch));
		rtn = lmi_error_ind(q, mp);
		break;
	case LMI_STATS_IND:
		printd(("%s: %p: LMI_STATS_IND <-\n", MOD_NAME, ch));
		rtn = lmi_stats_ind(q, mp);
		break;
	case LMI_EVENT_IND:
		printd(("%s: %p: LMI_EVENT_IND <-\n", MOD_NAME, ch));
		rtn = lmi_event_ind(q, mp);
		break;
	case SDL_RECEIVED_BITS_IND:
		printd(("%s: %p: SDL_RECEIVED_BITS_IND <-\n", MOD_NAME, ch));
		rtn = sdl_received_bits_ind(q, mp);
		break;
	case SDL_DISCONNECT_IND:
		printd(("%s: %p: SDL_DISCONNECT_IND <-\n", MOD_NAME, ch));
		rtn = sdl_disconnect_ind(q, mp);
		break;
	default:
		printd(("%s: %p: ???? %lu <-\n", MOD_NAME, ch, *(ulong *) mp->b_rptr));
		rtn = -EFAULT;
		break;
	}
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
ch_w_data(queue_t *q, mblk_t *mp)
{
	/* data from above */
	printd(("%s: %p: -> M_DATA\n", MOD_NAME, CH_PRIV(q)));
	return ch_write(q, mp);
}
STATIC int
ch_r_data(queue_t *q, mblk_t *mp)
{
	/* data from below */
	printd(("%s: %p: M_DATA <-\n", MOD_NAME, CH_PRIV(q)));
	return ch_read(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR, M_HANGUP Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
ch_r_error(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	int err;
	switch (ch->i_state) {
	default:
		/* warn user before we error out the stream */
		if ((err = ch_disable_ind(q, ch, -mp->b_rptr[0])))
			return (err);
		/* fall through */
	case CHS_UNUSABLE:
		return (QR_PASSALONG);
	}
}
STATIC int
ch_r_hangup(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	int err;
	switch (ch->i_state) {
	default:
		/* warn user before we error out the stream */
		if ((err = ch_disable_ind(q, ch, -EPIPE)))
			return (err);
		/* fall through */
	case CHS_UNUSABLE:
		return (QR_PASSALONG);
	}
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE int
ch_w_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return ch_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return ch_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return ch_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return ch_w_ioctl(q, mp);
	}
	return (QR_PASSALONG);
}
STATIC INLINE int
ch_r_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return ch_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return ch_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return ch_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	case M_ERROR:
		return ch_r_error(q, mp);
	case M_HANGUP:
		return ch_r_hangup(q, mp);
	case M_IOCACK:
		return ch_r_iocack(q, mp);
	case M_IOCNAK:
		return ch_r_iocnak(q, mp);
	}
	return (QR_PASSALONG);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 */
/*
 *  OPEN
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
ch_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	MOD_INC_USE_COUNT;	/* keep module from unloading */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		int cmajor = getmajor(*devp);
		int cminor = getminor(*devp);
		struct ch *ch;
		for (ch = ch_opens; ch; ch = ch->next) {
			if (ch->u.dev.cmajor == cmajor && ch->u.dev.cminor == cminor) {
				MOD_DEC_USE_COUNT;
				return (ENXIO);
			}
		}
		if (!ch_alloc_priv(q, &ch_opens, devp, crp)) {
			MOD_DEC_USE_COUNT;
			return (ENOMEM);
		}
		/* generate immediate information request */
		sdl_iocgconfig_req(q);
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return (EIO);
}

/*
 *  CLOSE
 *  -------------------------------------------------------------------------
 */
STATIC streamscall int
ch_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	ch_free_priv(q);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  =========================================================================
 *
 *  Private Structure allocation, deallocation and cache
 *
 *  =========================================================================
 */
STATIC kmem_cache_t *ch_priv_cachep = NULL;
STATIC int
ch_init_caches(void)
{
	if (!ch_priv_cachep &&
	    !(ch_priv_cachep =
	      kmem_cache_create("ch_priv_cachep", sizeof(struct ch), 0, SLAB_HWCACHE_ALIGN, NULL,
				NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate ch_priv_cachep", MOD_NAME);
		return (-ENOMEM);
	} else
		printd(("%s: initialized ch private structure cache\n", MOD_NAME));
	return (0);
}
STATIC int
ch_term_caches(void)
{
	if (ch_priv_cachep) {
		if (kmem_cache_destroy(ch_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ch_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			printd(("%s: destroyed ch_priv_cachep\n", MOD_NAME));
	}
	return (0);
}

/*
 *  CH allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct ch *
ch_alloc_priv(queue_t *q, struct ch **chp, dev_t *devp, cred_t *crp)
{
	struct ch *ch;
	if ((ch = kmem_cache_alloc(ch_priv_cachep, SLAB_ATOMIC))) {
		printd(("%s: %p: allocated ch private structure\n", MOD_NAME, ch));
		bzero(ch, sizeof(*ch));
		ch_get(ch);	/* first get */
		ch->u.dev.cmajor = getmajor(*devp);
		ch->u.dev.cminor = getminor(*devp);
		ch->cred = *crp;
		ch->o_prim = &ch_r_prim;
		ch->i_prim = &ch_w_prim;
		ch->o_wakeup = NULL;
		ch->i_wakeup = NULL;
		(ch->oq = RD(q))->q_ptr = ch_get(ch);
		(ch->iq = WR(q))->q_ptr = ch_get(ch);
		spin_lock_init(&ch->qlock);	/* "ch-queue-lock" */
		ch->i_state = CHS_UNINIT;	/* unitialized */
		ch->i_version = 1;
		ch->i_style = 0;
		spin_lock_init(&ch->lock);	/* "ch-priv-lock" */
		if ((ch->next = *chp))
			ch->next->prev = &ch->next;
		ch->prev = chp;
		*chp = ch_get(ch);
		printd(("%s: %p: linked ch private structure\n", MOD_NAME, ch));
		ch->config = ch_default;
	} else
		ptrace(("%s: ERROR: Could not allocate ch private structure\n", MOD_NAME));
	return (ch);
}
STATIC void
ch_free_priv(queue_t *q)
{
	struct ch *ch = CH_PRIV(q);
	psw_t flags = 0;
	ensure(ch, return);
	spin_lock_irqsave(&ch->lock, flags);
	{
		ss7_unbufcall((str_t *) ch);
		if ((*ch->prev = ch->next))
			ch->next->prev = ch->prev;
		ch->next = NULL;
		ch->prev = &ch->next;
		ch_put(ch);
		ch->oq->q_ptr = NULL;
		flushq(ch->oq, FLUSHALL);
		ch->oq = NULL;
		ch_put(ch);
		ch->iq->q_ptr = NULL;
		flushq(ch->iq, FLUSHALL);
		ch->iq = NULL;
		ch_put(ch);
	}
	spin_unlock_irqrestore(&ch->lock, flags);
	ch_put(ch);		/* final put */
	return;
}
STATIC struct ch *
ch_get(struct ch *ch)
{
	atomic_inc(&ch->refcnt);
	return (ch);
}
STATIC void
ch_put(struct ch *ch)
{
	if (atomic_dec_and_test(&ch->refcnt)) {
		kmem_cache_free(ch_priv_cachep, ch);
		printd(("%s: %p: freed ch private structure\n", MOD_NAME, ch));
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
MODULE_PARM_DESC(modid, "Module ID for the CH-SDL module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw ch_fmod = {
	.f_name = MOD_NAME,
	.f_str = &ch_sdlinfo,
	.f_flag = 0,
	.f_kmod = THIS_MODULE,
};

STATIC int
ch_register_strmod(void)
{
	int err;
	if ((err = register_strmod(&ch_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
ch_unregister_strmod(void)
{
	int err;
	if ((err = unregister_strmod(&ch_fmod)) < 0)
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
ch_register_strmod(void)
{
	int err;
	if ((err = lis_register_strmod(&ch_sdlinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	return (0);
}

STATIC int
ch_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&ch_sdlinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
ch_sdlinit(void)
{
	int err;
	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = ch_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = ch_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		ch_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
ch_sdlterminate(void)
{
	int err;
	if ((err = ch_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = ch_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(ch_sdlinit);
module_exit(ch_sdlterminate);

#endif				/* LINUX */
