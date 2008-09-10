/*****************************************************************************

 @(#) $RCSfile: mx_x400p.c,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2008-09-10 03:49:36 $

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

 Last Modified $Date: 2008-09-10 03:49:36 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mx_x400p.c,v $
 Revision 0.9.2.24  2008-09-10 03:49:36  brian
 - changes to accomodate FC9, SUSE 11.0 and Ubuntu 8.04

 Revision 0.9.2.23  2008-04-29 07:11:18  brian
 - updating headers for release

 Revision 0.9.2.22  2007/08/15 05:20:51  brian
 - GPLv3 updates

 Revision 0.9.2.21  2007/08/14 12:19:00  brian
 - GPLv3 header updates

 Revision 0.9.2.20  2007/07/14 01:35:21  brian
 - make license explicit, add documentation

 Revision 0.9.2.19  2007/03/25 19:00:38  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.18  2007/03/25 02:23:09  brian
 - add D_MP and D_MTPERQ flags

 Revision 0.9.2.17  2007/03/25 00:52:19  brian
 - synchronization updates

 Revision 0.9.2.16  2006/05/08 11:01:14  brian
 - new compilers mishandle postincrement of cast pointers

 Revision 0.9.2.15  2006/03/07 01:14:33  brian
 - binary compatible callouts

 Revision 0.9.2.14  2006/03/04 13:00:21  brian
 - FC4 x86_64 gcc 4.0.4 2.6.15 changes

 *****************************************************************************/

#ident "@(#) $RCSfile: mx_x400p.c,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2008-09-10 03:49:36 $"

static char const ident[] =
    "$RCSfile: mx_x400p.c,v $ $Name:  $($Revision: 0.9.2.24 $) $Date: 2008-09-10 03:49:36 $";

#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/mxi.h>
#include <ss7/mxi_ioctl.h>

#define MX_SDL_DESCRIP		"X400P-SS7 MULTIPLEX (MX) STREAMS MODULE."
#define MX_SDL_EXTRA		"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define MX_SDL_REVISION		"OpenSS7 $RCSfile: mx_x400p.c,v $ $Name:  $ ($Revision: 0.9.2.24 $) $Date: 2008-09-10 03:49:36 $"
#define MX_SDL_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define MX_SDL_DEVICE		"Supports SDLI pseudo-device drivers."
#define MX_SDL_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define MX_SDL_LICENSE		"GPL"
#define MX_SDL_BANNER		MX_SDL_DESCRIP		"\n" \
				MX_SDL_EXTRA		"\n" \
				MX_SDL_REVISION		"\n" \
				MX_SDL_COPYRIGHT	"\n" \
				MX_SDL_DEVICE		"\n" \
				MX_SDL_CONTACT
#define MX_SDL_SPLASH		MX_SDL_DESCRIP		"\n" \
				MX_SDL_REVISION

#ifdef LINUX
MODULE_AUTHOR(MX_SDL_CONTACT);
MODULE_DESCRIPTION(MX_SDL_DESCRIP);
MODULE_SUPPORTED_DEVICE(MX_SDL_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MX_SDL_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-mx_x400p");
#endif
#endif				/* LINUX */

#ifdef LFS
#define MX_SDL_MOD_ID	CONFIG_STREAMS_MX_SDL_MODID
#define MX_SDL_MOD_NAME	CONFIG_STREAMS_MX_SDL_NAME
#endif				/* LFS */

/*
 *  This module converts and SDL interface provided by (for example) the
 *  X400P-SS7 driver to a MX (Multiplex) channel interface for use by the MG
 *  (Media Gateway) driver.  This module permits SDL channel on an SS7 driver
 *  to be used as bearer channels by the Media Gateway driver.
 *
 *  The module is simply pushed over an freshly opened SDL stream.  The PPA
 *  address which is used in the MX_ATTACH_REQ primitive should be the same
 *  PPA used by the underlying SDL module.  This module only supports single
 *  channel (slot) operation, where each SDL stream represents one DS0 or DS0a
 *  bearer channel.
 */

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define MOD_ID		MX_SDL_MOD_ID
#define MOD_NAME	MX_SDL_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	MX_SDL_BANNER
#else				/* MODULE */
#define MOD_BANNER	MX_SDL_SPLASH
#endif				/* MODULE */

STATIC struct module_info mx_minfo = {
	mi_idnum:MOD_ID,		/* Module ID number */
	mi_idname:MOD_NAME,		/* Module ID name */
	mi_minpsz:1,			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:1,			/* Hi water mark */
	mi_lowat:0,			/* Lo water mark */
};

STATIC streamscall int mx_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int mx_close(queue_t *, int, cred_t *);

STATIC struct qinit mx_rinit = {
	qi_putp:ss7_oput,		/* Read put (message from below) */
	qi_qopen:mx_open,		/* Each open */
	qi_qclose:mx_close,		/* Last close */
	qi_minfo:&mx_minfo,		/* Information */
};

STATIC struct qinit mx_winit = {
	qi_putp:ss7_iput,		/* Write put (message from above) */
	qi_minfo:&mx_minfo,		/* Information */
};

MODULE_STATIC struct streamtab mx_sdlinfo = {
	st_rdinit:&mx_rinit,		/* Upper read queue */
	st_wrinit:&mx_winit,		/* Upper write queue */
};

/*
 *  =========================================================================
 *
 *  Private Data Structures
 *
 *  =========================================================================
 */

/* multiplex structure */
typedef struct mx {
	STR_DECLARATION (struct mx);	/* stream declaration */
	struct mx_config config;	/* configuration */
	struct mx_stats statsp;		/* stats periods */
	struct mx_stats stats;		/* statistics */
	struct mx_notify notify;	/* notifications */
	uchar add_ptr[32];		/* attached address */
	size_t add_len;			/* attached address length */
	uchar rem_ptr[32];		/* remote address */
	size_t rem_len;			/* remote address length */
} mx_t;

#define MX_PRIV(__q) ((struct mx *)(__q)->q_ptr)

struct mx_config mx_default = {
	block_size:64,			/* 64 bits per block */
	encoding:MX_ENCODING_NONE,	/* sample block encoding */
	sample_size:8,			/* sample size */
	rate:8000,			/* clock rate */
	tx_channels:1,			/* tx channels */
	rx_channels:1,			/* tx channels */
	opt_flags:MX_PARM_OPT_CLRCH,	/* option flags */
};

STATIC struct mx *mx_opens = NULL;

STATIC struct mx *mx_alloc_priv(queue_t *, struct mx **, dev_t *, cred_t *);
STATIC struct mx *mx_get(struct mx *);
STATIC void mx_put(struct mx *);
STATIC void mx_free_priv(queue_t *);

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
m_error(queue_t *q, struct mx *mx, int error)
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
			mx->state = MXS_UNUSABLE;
			printd(("%s: %p: <- M_HANGUP\n", MOD_NAME, mx));
			putnext(mx->oq, mp);
			return (-error);
		} else {
			mp->b_datap->db_type = M_ERROR;
			*mp->b_wptr++ = error < 0 ? -error : error;
			*mp->b_wptr++ = error < 0 ? -error : error;
			mx->state = MXS_UNUSABLE;
			printd(("%s: %p: <- M_ERROR\n", MOD_NAME, mx));
			putnext(mx->oq, mp);
			return (QR_DONE);
		}
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MX_INFO_ACK
 *  -----------------------------------
 *  Indicates to the multiplex user information concerning the multiplex provider and the
 *  attached multiplex (if any).
 */
STATIC INLINE int
mx_info_ack(queue_t *q, struct mx *mx)
{
	mblk_t *mp;
	struct MX_info_ack *p;
	struct MX_parms_circuit *o;
	size_t pad_len = (mx->add_len + (sizeof(ulong) - 1)) & ~(sizeof(ulong) - 1);

	if ((mp = ss7_allocb(q, sizeof(*p) + pad_len + sizeof(*o), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mx_primitive = MX_INFO_ACK;
		p->mx_addr_length = mx->add_len;
		p->mx_addr_offset = mx->add_len ? sizeof(*p) : 0;
		p->mx_parm_length = sizeof(*o);
		p->mx_parm_offset = sizeof(*p) + pad_len;
		p->mx_prov_flags = 0;
		p->mx_style = MX_STYLE2;
		p->mx_version = MX_VERSION_1_0;
		if (mx->add_len) {
			bcopy(mx->add_ptr, mp->b_wptr, mx->add_len);
			mp->b_wptr += mx->add_len;
		}
		o = (typeof(o)) mp->b_wptr;
		mp->b_wptr += sizeof(*o);
		o->cp_type = MX_PARMS_CIRCUIT;
		o->cp_block_size = mx->config.block_size;
		o->cp_encoding = mx->config.encoding;
		o->cp_sample_size = mx->config.sample_size;
		o->cp_rate = mx->config.rate;
		o->cp_tx_channels = mx->config.tx_channels;
		o->cp_rx_channels = mx->config.rx_channels;
		o->cp_opt_flags = mx->config.opt_flags;
		printd(("%s: %p: <- MX_INFO_ACK\n", MOD_NAME, mx));
		putnext(mx->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MX_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC INLINE int
mx_optmgmt_ack(queue_t *q, struct mx *mx, uchar *opt_ptr, size_t opt_len, ulong flags)
{
	mblk_t *mp;
	struct MX_optmgmt_ack *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mx_primitive = MX_OPTMGMT_ACK;
		p->mx_opt_length = opt_len;
		p->mx_opt_offset = opt_len ? sizeof(*p) : 0;
		p->mx_mgmt_flags = flags;
		if (opt_len) {
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
		}
		printd(("%s: %p: <- MX_OPTMGMT_ACK\n", MOD_NAME, mx));
		putnext(mx->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MX_OK_ACK
 *  -----------------------------------
 *  Indicates to the multiplex user that the last operation requiring
 *  acknowledgement was completed successfully.  (There are only two
 *  operations requiring acknowledgement: MX_ATTACH_REQ and MX_DETACH_REQ.)
 */
STATIC INLINE int
mx_ok_ack(queue_t *q, struct mx *mx)
{
	mblk_t *mp;
	struct MX_ok_ack *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mx_primitive = MX_OK_ACK;
		switch (mx->state) {
		case MXS_WACK_AREQ:
			p->mx_correct_prim = MX_ATTACH_REQ;
			p->mx_state = mx->state = MXS_ATTACHED;
			break;
		case MXS_WACK_UREQ:
			p->mx_correct_prim = MX_DETACH_REQ;
			p->mx_state = mx->state = MXS_DETACHED;
			break;
		case MXS_UNUSABLE:
			p->mx_correct_prim = MX_DETACH_REQ;
			p->mx_state = mx->state = MXS_UNUSABLE;
			break;
		default:
			swerr();
			freemsg(mp);
			return (-EFAULT);
		}
		printd(("%s: %p: <- MX_OK_ACK\n", MOD_NAME, mx));
		putnext(mx->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MX_ERROR_ACK
 *  -----------------------------------
 *  Indicates to the multiplex user that the last operation requiring
 *  acknowledgement or confirmation did not complete and was unsuccessful
 *  (suffered a non-fatal error).  There are six operations requiring
 *  acknowledgement or confirmation.  In addition, this error is returned when
 *  unrecognized primitives are sent.
 */
STATIC INLINE int
mx_error_ack(queue_t *q, struct mx *mx, ulong prim, long error)
{
	mblk_t *mp;
	struct MX_error_ack *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mx_primitive = MX_ERROR_ACK;
		p->mx_error_primitive = prim;
		p->mx_error_type = error > 0 ? error : MXSYSERR;
		p->mx_unix_error = error < 0 ? -error : 0;
		switch (mx->state) {
		case MXS_WACK_AREQ:
			if (prim == MX_ATTACH_REQ)
				p->mx_state = mx->state = MXS_DETACHED;
			break;
		case MXS_WACK_UREQ:
			if (prim == MX_DETACH_REQ)
				p->mx_state = mx->state = MXS_ATTACHED;
			break;
		case MXS_WACK_EREQ:
		case MXS_WCON_EREQ:
			if (prim == MX_ENABLE_REQ)
				p->mx_state = mx->state = MXS_ATTACHED;
			break;
		case MXS_WACK_RREQ:
		case MXS_WCON_RREQ:
			if (prim == MX_DISABLE_REQ)
				p->mx_state = mx->state = MXS_ENABLED;
			break;
		case MXS_WACK_CREQ:
		case MXS_WCON_CREQ:
			if (prim == MX_CONNECT_REQ) {
				if (mx->flags & MXF_BOTH_DIR)
					p->mx_state = mx->state = MXS_CONNECTED;
				else
					p->mx_state = mx->state = MXS_ENABLED;
			}
			break;
		case MXS_WACK_DREQ:
		case MXS_WCON_DREQ:
			if (prim == MX_DISCONNECT_REQ)
				p->mx_state = mx->state = MXS_CONNECTED;
			break;
		default:
			/* default is don't change state */
			p->mx_state = mx->state;
			break;
		}
		printd(("%s: %p: <- MX_ERROR_ACK\n", MOD_NAME, mx));
		putnext(mx->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MX_ENABLE_CON
 *  -----------------------------------
 *  Confirms to the multiplex user that the attached multiplex was enabled as
 *  requested.
 */
STATIC INLINE int
mx_enable_con(queue_t *q, struct mx *mx)
{
	mblk_t *mp;
	struct MX_enable_con *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mx_primitive = MX_ENABLE_CON;
		switch (mx->state) {
		case MXS_WACK_EREQ:
		case MXS_WCON_EREQ:
			mx->state = MXS_ENABLED;
			break;
		default:
			swerr();
			freemsg(mp);
			return (-EFAULT);
		}
		printd(("%s: %p: <- MX_ENABLE_CON\n", MOD_NAME, mx));
		putnext(mx->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MX_CONNECT_CON
 *  -----------------------------------
 *  Confirms to the multiplex user that the enabled or connected multiplex was
 *  connected in the requested direction.
 */
STATIC INLINE int
mx_connect_con(queue_t *q, struct mx *mx, ulong flags, ulong slot)
{
	mblk_t *mp;
	struct MX_connect_con *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mx_primitive = MX_CONNECT_CON;
		p->mx_conn_flags = flags;
		p->mx_slot = slot;
		switch (mx->state) {
		case MXS_WCON_CREQ:
			mx->flags |= (flags & MXF_BOTH_DIR);
			mx->state = MXS_CONNECTED;
			break;
		default:
			swerr();
			freemsg(mp);
			return (-EFAULT);
		}
		printd(("%s: %p: <- MX_CONNECT_CON\n", MOD_NAME, mx));
		putnext(mx->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MX_DATA_IND
 *  -----------------------------------
 *  Indicates to the multiplex user data which was received on the multiplex.
 *  This is the non-preferred way of sending data to the multiplex user.  We
 *  should normally just send M_DATA blocks.
 */
STATIC INLINE int
mx_data_ind(queue_t *q, struct mx *mx)
{
	mblk_t *mp;
	struct MX_data_ind *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mx_primitive = MX_DATA_IND;
		p->mx_slot = 0;
		printd(("%s: %p: <- MX_DATA_IND\n", MOD_NAME, mx));
		putnext(mx->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MX_DISCONNECT_IND
 *  -----------------------------------
 *  Indicates to the multiplex user that an autonomous disconnection occured,
 *  the directions of the disconnection and the cause of the disconnection.
 *  This is normally used to indicate that both directions were disconnected
 *  due to hardware or transmission system failure (loss of carrier or carrier
 *  alarms).  This indication is rather optional, as carrier alarms are also
 *  indicated to management streams for the underlying devices.
 */
STATIC INLINE int
mx_disconnect_ind(queue_t *q, struct mx *mx, ulong flags, ulong slot)
{
	mblk_t *mp;
	struct MX_disconnect_ind *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mx_primitive = MX_DISCONNECT_IND;
		p->mx_conn_flags = flags;
		p->mx_slot = slot;
		p->mx_cause = 0;	/* FIXME */
		switch (mx->state) {
		case MXS_CONNECTED:
		case MXS_WACK_CREQ:
		case MXS_WCON_CREQ:
			mx->flags &= ~MXF_BOTH_DIR;
			mx->state = MXS_ENABLED;
			break;
		default:
			swerr();
			freemsg(mp);
			return (-EFAULT);
		}
		printd(("%s: %p: <- MX_DISCONNECT_IND\n", MOD_NAME, mx));
		putnext(mx->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MX_DISCONNECT_CON
 *  -----------------------------------
 *  Confirms to the multiplex user that the requested directions were
 *  disconnected as requested.
 */
STATIC INLINE int
mx_disconnect_con(queue_t *q, struct mx *mx, ulong flags, ulong slot)
{
	mblk_t *mp;
	struct MX_disconnect_con *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mx_primitive = MX_DISCONNECT_CON;
		p->mx_conn_flags = flags;
		p->mx_slot = slot;
		switch (mx->state) {
		case MXS_WCON_DREQ:
			mx->flags &= ~(flags & MXF_BOTH_DIR);
			if (mx->flags & MXF_BOTH_DIR)
				mx->state = MXS_CONNECTED;
			else
				mx->state = MXS_ENABLED;
			break;
		case MXS_UNUSABLE:
			mx->state = MXS_UNUSABLE;
			break;
		default:
			swerr();
			freemsg(mp);
			return (-EFAULT);
		}
		printd(("%s: %p: <- MX_DISCONNECT_CON\n", MOD_NAME, mx));
		putnext(mx->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MX_DISABLE_IND
 *  -----------------------------------
 *  Indicates to the multiplex user that an autonomous disabling of the
 *  multiplex has occured.  This is normally used to indicate a fatal error on
 *  the underlying stream, but is optional.  The multiplex user should detach
 *  and reattach the stream.
 */
STATIC INLINE int
mx_disable_ind(queue_t *q, struct mx *mx, long cause)
{
	mblk_t *mp;
	struct MX_disable_ind *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mx_primitive = MX_DISABLE_IND;
		p->mx_cause = cause;
		mx->state = MXS_UNUSABLE;
		printd(("%s: %p: <- MX_DISABLE_IND\n", MOD_NAME, mx));
		putnext(mx->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  MX_DISABLE_CON
 *  -----------------------------------
 */
STATIC INLINE int
mx_disable_con(queue_t *q, struct mx *mx)
{
	mblk_t *mp;
	struct MX_disable_con *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->mx_primitive = MX_DISABLE_CON;
		switch (mx->state) {
		case MXS_WACK_RREQ:
		case MXS_WCON_RREQ:
			mx->state = MXS_ATTACHED;
			mx->flags &= ~MXF_BOTH_DIR;
			break;
		case MXS_UNUSABLE:
			mx->state = MXS_UNUSABLE;
			mx->flags &= ~MXF_BOTH_DIR;
			break;
		default:
			swerr();
			freemsg(mp);
			return (-EFAULT);
		}
		printd(("%s: %p: <- MX_DISABLE_CON\n", MOD_NAME, mx));
		putnext(mx->oq, mp);
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
lmi_info_req(queue_t *q, struct mx *mx)
{
	mblk_t *mp;
	lmi_info_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_INFO_REQ;
		printd(("%s: %p: LMI_INFO_REQ ->\n", MOD_NAME, mx));
		putnext(mx->iq, mp);
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
lmi_attach_req(queue_t *q, struct mx *mx, uchar *ppa_ptr, size_t ppa_len)
{
	mblk_t *mp;
	lmi_attach_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ATTACH_REQ;
		if (ppa_len) {
			bcopy(ppa_ptr, mp->b_wptr, ppa_len);
			mp->b_wptr += ppa_len;
		}
		mx->state = MXS_WACK_AREQ;
		printd(("%s: %p: LMI_ATTACH_REQ ->\n", MOD_NAME, mx));
		putnext(mx->iq, mp);
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
lmi_detach_req(queue_t *q, struct mx *mx)
{
	mblk_t *mp;
	lmi_detach_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_DETACH_REQ;
		mx->state = MXS_WACK_UREQ;
		printd(("%s: %p: LMI_DETACH_REQ ->\n", MOD_NAME, mx));
		putnext(mx->iq, mp);
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
lmi_enable_req(queue_t *q, struct mx *mx)
{
	mblk_t *mp;
	lmi_enable_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + mx->rem_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ENABLE_REQ;
		if (mx->rem_len) {
			bcopy(mx->rem_ptr, mp->b_wptr, mx->rem_len);
			mp->b_wptr += mx->rem_len;
		}
		mx->state = MXS_WCON_EREQ;
		printd(("%s: %p: LMI_ENABLE_REQ ->\n", MOD_NAME, mx));
		putnext(mx->iq, mp);
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
lmi_disable_req(queue_t *q, struct mx *mx)
{
	mblk_t *mp;
	lmi_disable_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_DISABLE_REQ;
		mx->state = MXS_WCON_RREQ;
		printd(("%s: %p: LMI_DISABLE_REQ ->\n", MOD_NAME, mx));
		putnext(mx->iq, mp);
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
lmi_optmgmt_req(queue_t *q, struct mx *mx, uchar *opt_ptr, size_t opt_len, ulong flags)
{
	mblk_t *mp;
	lmi_optmgmt_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OPTMGMT_REQ;
		p->lmi_opt_length = opt_len;
		p->lmi_opt_offset = opt_len ? sizeof(*p) : 0;
		p->lmi_mgmt_flags = flags;
		if (opt_len) {
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
		}
		printd(("%s: %p: LMI_OPTMGMT_REQ ->\n", MOD_NAME, mx));
		putnext(mx->iq, mp);
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
sdl_bits_for_transmission_req(queue_t *q, struct mx *mx, mblk_t *dp)
{
	mblk_t *mp;
	sdl_bits_for_transmission_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdl_primitive = SDL_BITS_FOR_TRANSMISSION_REQ;
		mp->b_cont = dp;
		printd(("%s: %p: SDL_BITS_FOR_TRANSMISSION_REQ ->\n", MOD_NAME, mx));
		putnext(mx->iq, mp);
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
sdl_connect_req(queue_t *q, struct mx *mx, ulong flags)
{
	mblk_t *mp;
	sdl_connect_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdl_primitive = SDL_CONNECT_REQ;
		p->sdl_flags = flags;
		mx->state = MXS_WCON_CREQ;
		printd(("%s: %p: SDL_CONNECT_REQ ->\n", MOD_NAME, mx));
		putnext(mx->iq, mp);
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
sdl_disconnect_req(queue_t *q, struct mx *mx, ulong flags)
{
	mblk_t *mp;
	sdl_disconnect_req_t *p;

	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdl_primitive = SDL_DISCONNECT_REQ;
		p->sdl_flags = flags;
		mx->state = MXS_WCON_DREQ;
		printd(("%s: %p: SDL_DISCONNECT_REQ ->\n", MOD_NAME, mx));
		putnext(mx->iq, mp);
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
	struct mx *mx = MX_PRIV(q);

	return mx_ok_ack(q, mx);
}

/*
 *  LMI_ERROR_ACK
 *  -----------------------------------
 */
STATIC int
lmi_error_ack(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	long prim, error = -EFAULT;
	lmi_error_ack_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (p->lmi_reason) {
	case LMI_BADADDRTYPE:
	case LMI_BADADDRESS:
	case LMI_BADPPA:
		error = MXBADADDR;
		break;
	case LMI_BADPRIM:
		error = MXBADPRIM;
		break;
	case LMI_NOTSUPP:
		error = MXNOTSUPP;
		break;
	case LMI_OUTSTATE:
		error = MXOUTSTATE;
		break;
	case LMI_SYSERR:
		error = -p->lmi_errno;
		break;
	}
	switch (mx->state) {
	case MXS_WACK_AREQ:
		prim = MX_ATTACH_REQ;
		break;
	case MXS_WACK_UREQ:
		prim = MX_DETACH_REQ;
		break;
	case MXS_WACK_EREQ:
	case MXS_WCON_EREQ:
		prim = MX_ENABLE_REQ;
		break;
	case MXS_WACK_RREQ:
	case MXS_WCON_RREQ:
		prim = MX_DISABLE_REQ;
		break;
	case MXS_WACK_CREQ:
	case MXS_WCON_CREQ:
		prim = MX_CONNECT_REQ;
		break;
	case MXS_WACK_DREQ:
	case MXS_WCON_DREQ:
		prim = MX_DISCONNECT_REQ;
		break;
	default:
		/* not expecting primitive */
		swerr();
		return (-EFAULT);
	}
	return mx_error_ack(q, mx, prim, error);
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
	struct mx *mx = MX_PRIV(q);

	return mx_enable_con(q, mx);
}

/*
 *  LMI_DISABLE_CON
 *  -----------------------------------
 */
STATIC int
lmi_disable_con(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);

	return mx_disable_con(q, mx);
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
mx_read(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);

	if (!mp || !msgdsize(mp))
		goto eproto;
	switch (mx->state) {
	case MXS_UNUSABLE:
	case MXS_ENABLED:
		goto discard;
	case MXS_WACK_CREQ:
	case MXS_WCON_CREQ:
	case MXS_WACK_DREQ:
	case MXS_WCON_DREQ:
	case MXS_CONNECTED:
		if (!(mx->flags & MXF_RX_DIR))
			goto discard;
		putnext(mx->oq, mp);
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
	if ((err = mx_read(q, mp->b_cont)) == QR_ABSORBED)
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
	struct mx *mx = MX_PRIV(q);
	sdl_disconnect_ind_t *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mx->state != MXS_CONNECTED && mx->state != MXS_WACK_CREQ && mx->state != MXS_WCON_CREQ)
		goto outstate;
	return mx_disconnect_ind(q, mx, MXF_BOTH_DIR, 0);
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
 *  Requests that the multiplex provider place the data contained in the M_DATA
 *  block onto the transmit stream of the multiplex.  This is the preferred
 *  method of sending data to the multiplex.
 */
STATIC int
mx_write(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);

	if (!mp || !msgdsize(mp))
		goto eproto;
	switch (mx->state) {
	case MXS_UNUSABLE:
	case MXS_ENABLED:
		goto discard;
	case MXS_WACK_CREQ:
	case MXS_WCON_CREQ:
	case MXS_WACK_DREQ:
	case MXS_WCON_DREQ:
	case MXS_CONNECTED:
		if (!(mx->flags & MXF_TX_DIR))
			goto discard;
		putnext(mx->iq, mp);
		return (QR_ABSORBED);
	}
	goto eproto;
      eproto:
	return m_error(q, mx, -EPROTO);
      discard:
	rare();
	return (QR_DONE);
}

/*
 *  MX_INFO_REQ
 *  -----------------------------------
 *  Requests that the multiplex provider return information about the provider
 *  and the attached multiplex (if any).
 */
STATIC int
mx_info_req(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct MX_info_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mx->state == MXS_UNUSABLE)
		goto outstate;
	return mx_info_ack(q, mx);
      outstate:
	return mx_error_ack(q, mx, p->mx_primitive, MXOUTSTATE);
      emsgsize:
	return mx_error_ack(q, mx, p->mx_primitive, -EMSGSIZE);
}

/*
 *  MX_OPTMGMT_REQ
 *  -----------------------------------
 *  Requests that the multiplex provider set, get or negotiate provider or
 *  attached multiplex options.
 */
STATIC int
mx_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct MX_optmgmt_req *p = (typeof(p)) mp->b_rptr;
	union MX_parms *o;

	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr > mp->b_rptr + p->mx_opt_offset + p->mx_opt_length)
		goto emsgsize;
	switch (mx->state) {
	case MXS_DETACHED:
	case MXS_UNUSABLE:
	case MXS_WACK_AREQ:
	case MXS_WACK_UREQ:
		break;
	default:
		goto outstate;
	}
	if (p->mx_opt_length) {
		if (p->mx_opt_length < sizeof(o->cp_type))
			goto badopt;
		o = (typeof(o)) (mp->b_rptr + p->mx_opt_offset);
		switch (o->cp_type) {
		case MX_PARMS_CIRCUIT:
			if (p->mx_opt_length < sizeof(o->circuit))
				goto badparm;
			switch (p->mx_mgmt_flags) {
			case 0:
			case MX_NEGOTIATE:
				/* negotiate current */
			case MX_SET_OPT:
				/* set current */
				mx->config.block_size = o->circuit.cp_block_size;
				mx->config.encoding = o->circuit.cp_encoding;
				mx->config.sample_size = o->circuit.cp_sample_size;
				mx->config.rate = o->circuit.cp_rate;
				mx->config.tx_channels = o->circuit.cp_tx_channels;
				mx->config.rx_channels = o->circuit.cp_rx_channels;
				mx->config.opt_flags = o->circuit.cp_opt_flags;
				return mx_optmgmt_ack(q, mx, (uchar *) o, sizeof(o->circuit),
						      p->mx_mgmt_flags);
			case MX_DEFAULT:
				/* set default */
				mx_default.block_size = o->circuit.cp_block_size;
				mx_default.encoding = o->circuit.cp_encoding;
				mx_default.sample_size = o->circuit.cp_sample_size;
				mx_default.rate = o->circuit.cp_rate;
				mx_default.tx_channels = o->circuit.cp_tx_channels;
				mx_default.rx_channels = o->circuit.cp_rx_channels;
				mx_default.opt_flags = o->circuit.cp_opt_flags;
				return mx_optmgmt_ack(q, mx, (uchar *) o, sizeof(o->circuit),
						      p->mx_mgmt_flags);
			}
		}
		goto badparmtype;
	} else {
		union MX_parms parms;

		o = &parms;
		/* no options */
		switch (p->mx_mgmt_flags) {
		case MX_GET_OPT:
			/* get current */
			o->cp_type = MX_PARMS_CIRCUIT;
			o->circuit.cp_block_size = mx->config.block_size;
			o->circuit.cp_encoding = mx->config.encoding;
			o->circuit.cp_sample_size = mx->config.sample_size;
			o->circuit.cp_rate = mx->config.rate;
			o->circuit.cp_tx_channels = mx->config.tx_channels;
			o->circuit.cp_rx_channels = mx->config.rx_channels;
			o->circuit.cp_opt_flags = mx->config.opt_flags;
			return mx_optmgmt_ack(q, mx, (uchar *) o, sizeof(o->circuit),
					      p->mx_mgmt_flags);
		case 0:
		case MX_DEFAULT:
			/* get default */
			o->cp_type = MX_PARMS_CIRCUIT;
			o->circuit.cp_block_size = mx_default.block_size;
			o->circuit.cp_encoding = mx_default.encoding;
			o->circuit.cp_sample_size = mx_default.sample_size;
			o->circuit.cp_rate = mx_default.rate;
			o->circuit.cp_tx_channels = mx_default.tx_channels;
			o->circuit.cp_rx_channels = mx_default.rx_channels;
			o->circuit.cp_opt_flags = mx_default.opt_flags;
			return mx_optmgmt_ack(q, mx, (uchar *) o, sizeof(o->circuit),
					      p->mx_mgmt_flags);
		}
	}
	goto badflag;
      badflag:
	return mx_error_ack(q, mx, p->mx_primitive, MXBADFLAG);
      badparmtype:
	return mx_error_ack(q, mx, p->mx_primitive, MXBADPARMTYPE);
      badparm:
	return mx_error_ack(q, mx, p->mx_primitive, MXBADPARM);
      badopt:
	return mx_error_ack(q, mx, p->mx_primitive, MXBADOPT);
      outstate:
	return mx_error_ack(q, mx, p->mx_primitive, MXOUTSTATE);
      emsgsize:
	return mx_error_ack(q, mx, p->mx_primitive, -EMSGSIZE);
}

/*
 *  MX_ATTACH_REQ
 *  -----------------------------------
 *  Requests that the multiplex provider attached the requesting stream to the
 *  specified multiplex (specified in the multiplex address).
 */
STATIC int
mx_attach_req(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct MX_attach_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mp->b_wptr > mp->b_rptr + p->mx_addr_offset + p->mx_addr_length)
		goto emsgsize;
	if (mx->state != MXS_DETACHED)
		goto outstate;
	return lmi_attach_req(q, mx, mp->b_rptr + p->mx_addr_offset, p->mx_addr_length);
      outstate:
	return mx_error_ack(q, mx, p->mx_primitive, MXOUTSTATE);
      emsgsize:
	return mx_error_ack(q, mx, p->mx_primitive, -EMSGSIZE);
}

/*
 *  MX_ENABLE_REQ
 *  -----------------------------------
 *  Requests that the multiplex provider enable the attached multiplex.
 */
STATIC int
mx_enable_req(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct MX_enable_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (mx->state != MXS_ATTACHED)
		goto outstate;
	return lmi_enable_req(q, mx);
      outstate:
	return mx_error_ack(q, mx, p->mx_primitive, MXOUTSTATE);
      emsgsize:
	return mx_error_ack(q, mx, p->mx_primitive, -EMSGSIZE);
}

/*
 *  MX_CONNECT_REQ
 *  -----------------------------------
 *  Requests that the multiplex provider connect the specified direction on the
 *  attached and enabled multiplex.
 */
STATIC int
mx_connect_req(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct MX_connect_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (mx->state) {
	case MXS_ENABLED:
	case MXS_CONNECTED:
		if (p->mx_slot != 0)
			goto badslot;
		if (!(p->mx_conn_flags & MXF_BOTH_DIR))
			goto badflag;
		if (((mx->flags & p->mx_conn_flags) & MXF_BOTH_DIR))
			goto badflag;
		if ((err = sdl_connect_req(q, mx, p->mx_conn_flags & MXF_BOTH_DIR)))
			return (err);
		/* fall through */
	case MXS_WCON_CREQ:
		/* automatic confirmation */
		return mx_connect_con(q, mx, p->mx_conn_flags & MXF_BOTH_DIR, p->mx_slot);
	}
	goto outstate;
      badslot:
	return mx_error_ack(q, mx, p->mx_primitive, MXBADSLOT);
      badflag:
	return mx_error_ack(q, mx, p->mx_primitive, MXBADFLAG);
      outstate:
	return mx_error_ack(q, mx, p->mx_primitive, MXOUTSTATE);
      emsgsize:
	return mx_error_ack(q, mx, p->mx_primitive, -EMSGSIZE);
}

/*
 *  MX_DATA_REQ
 *  -----------------------------------
 *  Requests that the provider send data on the connected multiplex on the
 *  specified mux slot.  This module only provides one slot.
 */
STATIC int
mx_data_req(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct MX_data_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eproto;
	if (p->mx_slot != 0)
		goto eproto;
	if ((err = mx_write(q, mp->b_cont)) == QR_ABSORBED)
		return (QR_TRIMMED);
	return (err);
      eproto:
	return m_error(q, mx, -EPROTO);
}

/*
 *  MX_DISCONNECT_REQ
 *  -----------------------------------
 *  Requests that the provider disconnect the specified direction for the
 *  attached, enabled and connected multiplex.
 */
STATIC int
mx_disconnect_req(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct MX_disconnect_req *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (mx->state) {
	case MXS_CONNECTED:
		if (p->mx_slot != 0)
			goto badslot;
		if (!((mx->flags & p->mx_conn_flags) & MXF_BOTH_DIR))
			goto badflag;
		if ((err = sdl_disconnect_req(q, mx, p->mx_conn_flags & MXF_BOTH_DIR)))
			return (err);
		/* fall through */
	case MXS_WCON_DREQ:
	case MXS_UNUSABLE:
		/* automatic confirmation */
		return mx_disconnect_con(q, mx, p->mx_conn_flags & MXF_BOTH_DIR, p->mx_slot);
	}
	goto outstate;
      badslot:
	return mx_error_ack(q, mx, p->mx_primitive, MXBADSLOT);
      badflag:
	return mx_error_ack(q, mx, p->mx_primitive, MXBADFLAG);
      outstate:
	return mx_error_ack(q, mx, p->mx_primitive, MXOUTSTATE);
      emsgsize:
	return mx_error_ack(q, mx, p->mx_primitive, -EMSGSIZE);
}

/*
 *  MX_DISABLE_REQ
 *  -----------------------------------
 *  Requests that the provider disable the attached and enabled multiplex.
 */
STATIC int
mx_disable_req(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct MX_disable_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (mx->state) {
	case MXS_ENABLED:
		return lmi_disable_req(q, mx);
	case MXS_UNUSABLE:
		return mx_disable_con(q, mx);
	}
	goto outstate;
      outstate:
	return mx_error_ack(q, mx, p->mx_primitive, MXOUTSTATE);
      emsgsize:
	return mx_error_ack(q, mx, p->mx_primitive, -EMSGSIZE);
}

/*
 *  MX_DETACH_REQ
 *  -----------------------------------
 *  Requests that the provider detach the attached multiplex.
 */
STATIC int
mx_detach_req(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct MX_detach_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (mx->state) {
	case MXS_ATTACHED:
		return lmi_detach_req(q, mx);
	case MXS_UNUSABLE:
		return mx_ok_ack(q, mx);
	}
	goto outstate;
      outstate:
	return mx_error_ack(q, mx, p->mx_primitive, MXOUTSTATE);
      emsgsize:
	return mx_error_ack(q, mx, p->mx_primitive, -EMSGSIZE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  IO CONTROLS
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  MX_IOCGCONFIG
 *  -----------------------------------
 */
STATIC int
mx_iocgconfig(struct mx *mx, struct mx_config *p)
{
	*p = mx->config;
	return (0);
}

/*
 *  MX_IOCSCONFIG
 *  -----------------------------------
 */
STATIC int mx_ioctconfig(struct mx *, struct mx_config *);
STATIC int
mx_iocsconfig(struct mx *mx, struct mx_config *p)
{
	int err;

	if ((err = mx_ioctconfig(mx, p)))
		return (err);
	mx->config = *p;
	return (0);
}

/*
 *  MX_IOCTCONFIG
 *  -----------------------------------
 */
STATIC int
mx_ioctconfig(struct mx *mx, struct mx_config *p)
{
	if (p->block_size <= 0 || p->block_size & 0x7)
		goto einval;
	switch (p->encoding) {
	case MX_ENCODING_NONE:
	case MX_ENCODING_G711_PCM_A:
	case MX_ENCODING_G711_PCM_U:
	case MX_ENCODING_S8:
	case MX_ENCODING_U8:
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
 *  MX_IOCCCONFIG
 *  -----------------------------------
 */
STATIC int
mx_ioccconfig(struct mx *mx, struct mx_config *p)
{
	*p = mx->config = mx_default;
	return (0);
}

/*
 *  MX_IOCGSTATEM
 *  -----------------------------------
 */
STATIC int
mx_iocgstatem(struct mx *mx, struct mx_statem *p)
{
	p->state = mx->state;
	p->flags = mx->flags;
	return (0);
}

/*
 *  MX_IOCCMRESET
 *  -----------------------------------
 */
STATIC int
mx_ioccmreset(struct mx *mx, struct mx_statem *p)
{
	mx->state = p->state;
	mx->flags = p->flags;
	return (0);
}

/*
 *  MX_IOCGSTATSP
 *  -----------------------------------
 */
STATIC int
mx_iocgstatsp(struct mx *mx, struct mx_stats *p)
{
	*p = mx->statsp;
	return (0);
}

/*
 *  MX_IOCSSTATSP
 *  -----------------------------------
 */
STATIC int
mx_iocsstatsp(struct mx *mx, struct mx_stats *p)
{
	mx->statsp = *p;
	return (0);
}

/*
 *  MX_IOCGSTATS
 *  -----------------------------------
 */
STATIC int
mx_iocgstats(struct mx *mx, struct mx_stats *p)
{
	*p = mx->stats;
	return (0);
}

/*
 *  MX_IOCCSTATS
 *  -----------------------------------
 */
STATIC int
mx_ioccstats(struct mx *mx, struct mx_stats *p)
{
	bzero(&mx->stats, sizeof(mx->stats));
	return (0);
}

/*
 *  MX_IOCGNOTIFY
 *  -----------------------------------
 */
STATIC int
mx_iocgnotify(struct mx *mx, struct mx_notify *p)
{
	*p = mx->notify;
	return (0);
}

/*
 *  MX_IOCSNOTIFY
 *  -----------------------------------
 */
STATIC int
mx_iocsnotify(struct mx *mx, struct mx_notify *p)
{
	mx->notify.events |= p->events;
	return (0);
}

/*
 *  MX_IOCCNOTIFY
 *  -----------------------------------
 */
STATIC int
mx_ioccnotify(struct mx *mx, struct mx_notify *p)
{
	mx->notify.events &= ~p->events;
	return (0);
}

/*
 *  SDL_IOCGCONFIG
 *  -----------------------------------
 */
STATIC int
sdl_iocgconfig_req(queue_t *q)
{
	struct mx *mx = MX_PRIV(q);
	mblk_t *mp, *dp;
	struct iocblk *iocp;

	if ((mp = ss7_allocb(q, sizeof(*iocp), BPRI_MED))) {
		mp->b_datap->db_type = M_IOCTL;
		bzero(mp->b_wptr, sizeof(*iocp));
		iocp = (typeof(iocp)) mp->b_wptr;
		mp->b_wptr += sizeof(*iocp);
		iocp->ioc_cmd = SDL_IOCGCONFIG;
		iocp->ioc_id = (uint) (long) mx;	/* XXX */
		iocp->ioc_count = sizeof(sdl_config_t);
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
		if ((dp = ss7_allocb(q, sizeof(sdl_config_t), BPRI_HI))) {
			dp->b_datap->db_type = M_DATA;
			bzero(dp->b_wptr, sizeof(sdl_config_t));
			dp->b_wptr += sizeof(sdl_config_t);
			mp->b_cont = dp;
			putnext(mx->iq, mp);
			return (QR_DONE);
		}
		freemsg(mp);
	}
	rare();
	return (-ENOBUFS);
}
STATIC int
sdl_iocgconfig_ack(struct mx *mx, sdl_config_t * p)
{
	/* ignore */
	switch (mx->state) {
	case MXS_UNINIT:
		switch (p->iftype) {
		case SDL_TYPE_DS0:
			mx->config.sample_size = 8;
			mx->config.rate = 8000;
			break;
		case SDL_TYPE_DS0A:
			mx->config.sample_size = 7;
			mx->config.rate = 8000;
			break;
		default:
			goto einval;
		}
		switch (p->ifgtype) {
		case SDL_GTYPE_E1:
			mx->config.encoding = MX_ENCODING_G711_PCM_A;
			break;
		case SDL_GTYPE_T1:
			mx->config.encoding = MX_ENCODING_G711_PCM_U;
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
		mx->state = MXS_DETACHED;
		return (QR_DONE);
	default:
		swerr();
		return (-EFAULT);
	}
      einval:
	swerr();
	mx->state = MXS_UNUSABLE;
	return m_error(mx->oq, mx, -EFAULT);
}
STATIC int
sdl_iocgconfig_nak(struct mx *mx, sdl_config_t * p)
{
	swerr();
	return m_error(mx->oq, mx, -EFAULT);
}

/*
 *  SDL_IOCSSTATSP
 *  -----------------------------------
 */
#if 0
STATIC int
sdl_iocsstatsp_req(queue_t *q, sdl_stats_t * p)
{
	struct mx *mx = MX_PRIV(q);
	mblk_t *mp, *dp;
	struct iocblk *iocp;

	if ((mp = ss7_allocb(q, sizeof(*iocp), BPRI_MED))) {
		mp->b_datap->db_type = M_IOCTL;
		bzero(mp->b_wptr, sizeof(*iocp));
		iocp = (typeof(iocp)) mp->b_wptr;
		mp->b_wptr += sizeof(*iocp);
		iocp->ioc_cmd = SDL_IOCSSTATSP;
		iocp->ioc_id = (uint) (long) mx;	/* XXX */
		iocp->ioc_count = sizeof(sdl_stats_t);
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
		if ((dp = ss7_allocb(q, sizeof(sdl_stats_t), BPRI_HI))) {
			dp->b_datap->db_type = M_DATA;
			bcopy(p, dp->b_wptr, sizeof(sdl_stats_t));
			dp->b_wptr += sizeof(sdl_stats_t);
			mp->b_cont = dp;
			putnext(mx->iq, mp);
			return (QR_DONE);
		}
		freemsg(mp);
	}
	rare();
	return (-ENOBUFS);
}
#endif
STATIC int
sdl_iocsstatsp_ack(struct mx *mx, sdl_stats_t * p)
{
	/* ignore */
	return (QR_DONE);
}
STATIC int
sdl_iocsstatsp_nak(struct mx *mx, sdl_stats_t * p)
{
	swerr();
	return m_error(mx->oq, mx, -EFAULT);
}

/*
 *  SDL_IOCGSTATS
 *  -----------------------------------
 */
#if 0
STATIC int
sdl_iocgstats_req(queue_t *q)
{
	struct mx *mx = MX_PRIV(q);
	mblk_t *mp, *dp;
	struct iocblk *iocp;

	if ((mp = ss7_allocb(q, sizeof(*iocp), BPRI_MED))) {
		mp->b_datap->db_type = M_IOCTL;
		bzero(mp->b_wptr, sizeof(*iocp));
		iocp = (typeof(iocp)) mp->b_wptr;
		mp->b_wptr += sizeof(*iocp);
		iocp->ioc_cmd = SDL_IOCGSTATS;
		iocp->ioc_id = (uint) (long) mx;	/* XXX */
		iocp->ioc_count = sizeof(sdl_stats_t);
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
		if ((dp = ss7_allocb(q, sizeof(sdl_stats_t), BPRI_HI))) {
			dp->b_datap->db_type = M_DATA;
			bzero(dp->b_wptr, sizeof(sdl_stats_t));
			dp->b_wptr += sizeof(sdl_stats_t);
			mp->b_cont = dp;
			putnext(mx->iq, mp);
			return (QR_DONE);
		}
		freemsg(mp);
	}
	rare();
	return (-ENOBUFS);
}
#endif
STATIC int
sdl_iocgstats_ack(struct mx *mx, sdl_stats_t * p)
{
	/* ignore */
	return (QR_DONE);
}
STATIC int
sdl_iocgstats_nak(struct mx *mx, sdl_stats_t * p)
{
	swerr();
	return m_error(mx->oq, mx, -EFAULT);
}

/*
 *  SDL_IOCCSTATS
 *  -----------------------------------
 */
#if 0
STATIC int
sdl_ioccstats_req(queue_t *q)
{
	struct mx *mx = MX_PRIV(q);
	mblk_t *mp, *dp;
	struct iocblk *iocp;

	if ((mp = ss7_allocb(q, sizeof(*iocp), BPRI_MED))) {
		mp->b_datap->db_type = M_IOCTL;
		bzero(mp->b_wptr, sizeof(*iocp));
		iocp = (typeof(iocp)) mp->b_wptr;
		mp->b_wptr += sizeof(*iocp);
		iocp->ioc_cmd = SDL_IOCCSTATS;
		iocp->ioc_id = (uint) (long) mx;	/* XXX */
		iocp->ioc_count = sizeof(sdl_stats_t);
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
		if ((dp = ss7_allocb(q, sizeof(sdl_stats_t), BPRI_HI))) {
			dp->b_datap->db_type = M_DATA;
			bzero(dp->b_wptr, sizeof(sdl_stats_t));
			dp->b_wptr += sizeof(sdl_stats_t);
			mp->b_cont = dp;
			putnext(mx->iq, mp);
			return (QR_DONE);
		}
		freemsg(mp);
	}
	rare();
	return (-ENOBUFS);
}
#endif
STATIC int
sdl_ioccstats_ack(struct mx *mx, sdl_stats_t * p)
{
	/* ignore */
	return (QR_DONE);
}
STATIC int
sdl_ioccstats_nak(struct mx *mx, sdl_stats_t * p)
{
	swerr();
	return m_error(mx->oq, mx, -EFAULT);
}

/*
 *  SDL_IOCSNOTIFY
 *  -----------------------------------
 */
#if 0
STATIC int
sdl_iocsnotify_req(queue_t *q, sdl_notify_t * p)
{
	struct mx *mx = MX_PRIV(q);
	mblk_t *mp, *dp;
	struct iocblk *iocp;

	if ((mp = ss7_allocb(q, sizeof(*iocp), BPRI_MED))) {
		mp->b_datap->db_type = M_IOCTL;
		bzero(mp->b_wptr, sizeof(*iocp));
		iocp = (typeof(iocp)) mp->b_wptr;
		mp->b_wptr += sizeof(*iocp);
		iocp->ioc_cmd = SDL_IOCSNOTIFY;
		iocp->ioc_id = (uint) (long) mx;	/* XXX */
		iocp->ioc_count = sizeof(sdl_notify_t);
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
		if ((dp = ss7_allocb(q, sizeof(sdl_notify_t), BPRI_HI))) {
			dp->b_datap->db_type = M_DATA;
			bcopy(p, dp->b_wptr, sizeof(sdl_notify_t));
			dp->b_wptr += sizeof(sdl_notify_t);
			mp->b_cont = dp;
			putnext(mx->iq, mp);
			return (QR_DONE);
		}
		freemsg(mp);
	}
	rare();
	return (-ENOBUFS);
}
#endif
STATIC int
sdl_iocsnotify_ack(struct mx *mx, sdl_notify_t * p)
{
	/* ignore */
	return (QR_DONE);
}
STATIC int
sdl_iocsnotify_nak(struct mx *mx, sdl_notify_t * p)
{
	swerr();
	return m_error(mx->oq, mx, -EFAULT);
}

/*
 *  SDL_IOCCNOTIFY
 *  -----------------------------------
 */
#if 0
STATIC int
sdl_ioccnotify_req(queue_t *q, sdl_notify_t * p)
{
	struct mx *mx = MX_PRIV(q);
	mblk_t *mp, *dp;
	struct iocblk *iocp;

	if ((mp = ss7_allocb(q, sizeof(*iocp), BPRI_MED))) {
		mp->b_datap->db_type = M_IOCTL;
		bzero(mp->b_wptr, sizeof(*iocp));
		iocp = (typeof(iocp)) mp->b_wptr;
		mp->b_wptr += sizeof(*iocp);
		iocp->ioc_cmd = SDL_IOCCNOTIFY;
		iocp->ioc_id = (uint) (long) mx;	/* XXX */
		iocp->ioc_count = sizeof(sdl_notify_t);
		iocp->ioc_error = 0;
		iocp->ioc_rval = 0;
		if ((dp = ss7_allocb(q, sizeof(sdl_notify_t), BPRI_HI))) {
			dp->b_datap->db_type = M_DATA;
			bcopy(p, dp->b_wptr, sizeof(sdl_notify_t));
			dp->b_wptr += sizeof(sdl_notify_t);
			mp->b_cont = dp;
			putnext(mx->iq, mp);
			return (QR_DONE);
		}
		freemsg(mp);
	}
	rare();
	return (-ENOBUFS);
}
#endif
STATIC int
sdl_ioccnotify_ack(struct mx *mx, sdl_notify_t * p)
{
	/* ignore */
	return (QR_DONE);
}
STATIC int
sdl_ioccnotify_nak(struct mx *mx, sdl_notify_t * p)
{
	swerr();
	return m_error(mx->oq, mx, -EFAULT);
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
mx_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int ret = 0;

	switch (type) {
	case MX_IOC_MAGIC:
	{
		/* These are MX IOCTLs. */
		if (count < size || !arg) {
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(MX_IOCGCONFIG):
			ret = mx_iocgconfig(mx, arg);
			break;
		case _IOC_NR(MX_IOCSCONFIG):
			ret = mx_iocsconfig(mx, arg);
			break;
		case _IOC_NR(MX_IOCTCONFIG):
			ret = mx_ioctconfig(mx, arg);
			break;
		case _IOC_NR(MX_IOCCCONFIG):
			ret = mx_ioccconfig(mx, arg);
			break;
		case _IOC_NR(MX_IOCGSTATEM):
			ret = mx_iocgstatem(mx, arg);
			break;
		case _IOC_NR(MX_IOCCMRESET):
			ret = mx_ioccmreset(mx, arg);
			break;
		case _IOC_NR(MX_IOCGSTATSP):
			ret = mx_iocgstatsp(mx, arg);
			break;
		case _IOC_NR(MX_IOCSSTATSP):
			ret = mx_iocsstatsp(mx, arg);
			break;
		case _IOC_NR(MX_IOCGSTATS):
			ret = mx_iocgstats(mx, arg);
			break;
		case _IOC_NR(MX_IOCCSTATS):
			ret = mx_ioccstats(mx, arg);
			break;
		case _IOC_NR(MX_IOCGNOTIFY):
			ret = mx_iocgnotify(mx, arg);
			break;
		case _IOC_NR(MX_IOCSNOTIFY):
			ret = mx_iocsnotify(mx, arg);
			break;
		case _IOC_NR(MX_IOCCNOTIFY):
			ret = mx_ioccnotify(mx, arg);
			break;
		default:
			ptrace(("%s: ERROR: Unsupported MX ioctl %d\n", MOD_NAME, nr));
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
mx_r_iocack(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);

	if (iocp->ioc_id != (uint) (long) mx)
		/* XXX */
		/* we didn't send it */
		return (QR_PASSALONG);
	switch (type) {
	case SDL_IOC_MAGIC:
		if (count < size || !arg)
			goto efault;
		switch (nr) {
		case _IOC_NR(SDL_IOCGCONFIG):
			return sdl_iocgconfig_ack(mx, arg);
		case _IOC_NR(SDL_IOCSSTATSP):
			return sdl_iocsstatsp_ack(mx, arg);
		case _IOC_NR(SDL_IOCGSTATS):
			return sdl_iocgstats_ack(mx, arg);
		case _IOC_NR(SDL_IOCCSTATS):
			return sdl_ioccstats_ack(mx, arg);
		case _IOC_NR(SDL_IOCSNOTIFY):
			return sdl_iocsnotify_ack(mx, arg);
		case _IOC_NR(SDL_IOCCNOTIFY):
			return sdl_ioccnotify_ack(mx, arg);
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
mx_r_iocnak(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);

	if (iocp->ioc_id != (uint) (long) mx)
		/* XXX */
		/* we didn't send it */
		return (QR_PASSALONG);
	switch (type) {
	case SDL_IOC_MAGIC:
		if (count < size || !arg)
			goto efault;
		switch (nr) {
		case _IOC_NR(SDL_IOCGCONFIG):
			return sdl_iocgconfig_nak(mx, arg);
		case _IOC_NR(SDL_IOCSSTATSP):
			return sdl_iocsstatsp_nak(mx, arg);
		case _IOC_NR(SDL_IOCGSTATS):
			return sdl_iocgstats_nak(mx, arg);
		case _IOC_NR(SDL_IOCCSTATS):
			return sdl_ioccstats_nak(mx, arg);
		case _IOC_NR(SDL_IOCSNOTIFY):
			return sdl_iocsnotify_nak(mx, arg);
		case _IOC_NR(SDL_IOCCNOTIFY):
			return sdl_ioccnotify_nak(mx, arg);
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
 *  Primitives from MG to MX.
 *  -----------------------------------
 */
STATIC int
mx_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct mx *mx = MX_PRIV(q);
	ulong prim;

	(void) mx;
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case MX_INFO_REQ:
		printd(("%s: %p: -> MX_INFO_REQ\n", MOD_NAME, mx));
		rtn = mx_info_req(q, mp);
		break;
	case MX_OPTMGMT_REQ:
		printd(("%s: %p: -> MX_OPTMGMT_REQ\n", MOD_NAME, mx));
		rtn = mx_optmgmt_req(q, mp);
		break;
	case MX_ATTACH_REQ:
		printd(("%s: %p: -> MX_ATTACH_REQ\n", MOD_NAME, mx));
		rtn = mx_attach_req(q, mp);
		break;
	case MX_ENABLE_REQ:
		printd(("%s: %p: -> MX_ENABLE_REQ\n", MOD_NAME, mx));
		rtn = mx_enable_req(q, mp);
		break;
	case MX_CONNECT_REQ:
		printd(("%s: %p: -> MX_CONNECT_REQ\n", MOD_NAME, mx));
		rtn = mx_connect_req(q, mp);
		break;
	case MX_DATA_REQ:
		printd(("%s: %p: -> MX_DATA_REQ\n", MOD_NAME, mx));
		rtn = mx_data_req(q, mp);
		break;
	case MX_DISCONNECT_REQ:
		printd(("%s: %p: -> MX_DISCONNECT_REQ\n", MOD_NAME, mx));
		rtn = mx_disconnect_req(q, mp);
		break;
	case MX_DISABLE_REQ:
		printd(("%s: %p: -> MX_DISABLE_REQ\n", MOD_NAME, mx));
		rtn = mx_disable_req(q, mp);
		break;
	case MX_DETACH_REQ:
		printd(("%s: %p: -> MX_DETACH_REQ\n", MOD_NAME, mx));
		rtn = mx_detach_req(q, mp);
		break;
	default:
		printd(("%s: %p: -> MX_????\n", MOD_NAME, mx));
		rtn = mx_error_ack(q, mx, prim, MXNOTSUPP);
		break;
	}
	return (rtn);
}

/*
 *  Primitives from SDL to MX.
 *  -----------------------------------
 */
STATIC int
mx_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	struct mx *mx = MX_PRIV(q);

	(void) mx;
	switch (*((ulong *) mp->b_rptr)) {
	case LMI_INFO_ACK:
		printd(("%s: %p: LMI_INFO_ACK <-\n", MOD_NAME, mx));
		rtn = lmi_info_ack(q, mp);
		break;
	case LMI_OK_ACK:
		printd(("%s: %p: LMI_OK_ACK <-\n", MOD_NAME, mx));
		rtn = lmi_ok_ack(q, mp);
		break;
	case LMI_ERROR_ACK:
		printd(("%s: %p: LMI_ERROR_ACK <-\n", MOD_NAME, mx));
		rtn = lmi_error_ack(q, mp);
		break;
	case LMI_ENABLE_CON:
		printd(("%s: %p: LMI_ENABLE_CON <-\n", MOD_NAME, mx));
		rtn = lmi_enable_con(q, mp);
		break;
	case LMI_DISABLE_CON:
		printd(("%s: %p: LMI_DISABLE_CON <-\n", MOD_NAME, mx));
		rtn = lmi_disable_con(q, mp);
		break;
	case LMI_OPTMGMT_ACK:
		printd(("%s: %p: LMI_OPTMGMT_ACK <-\n", MOD_NAME, mx));
		rtn = lmi_optmgmt_ack(q, mp);
		break;
	case LMI_ERROR_IND:
		printd(("%s: %p: LMI_ERROR_IND <-\n", MOD_NAME, mx));
		rtn = lmi_error_ind(q, mp);
		break;
	case LMI_STATS_IND:
		printd(("%s: %p: LMI_STATS_IND <-\n", MOD_NAME, mx));
		rtn = lmi_stats_ind(q, mp);
		break;
	case LMI_EVENT_IND:
		printd(("%s: %p: LMI_EVENT_IND <-\n", MOD_NAME, mx));
		rtn = lmi_event_ind(q, mp);
		break;
	case SDL_RECEIVED_BITS_IND:
		printd(("%s: %p: SDL_RECEIVED_BITS_IND <-\n", MOD_NAME, mx));
		rtn = sdl_received_bits_ind(q, mp);
		break;
	case SDL_DISCONNECT_IND:
		printd(("%s: %p: SDL_DISCONNECT_IND <-\n", MOD_NAME, mx));
		rtn = sdl_disconnect_ind(q, mp);
		break;
	default:
		printd(("%s: %p: ???? %lu <-\n", MOD_NAME, mx, *(ulong *) mp->b_rptr));
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
mx_w_data(queue_t *q, mblk_t *mp)
{
	/* data from above */
	printd(("%s: %p: -> M_DATA\n", MOD_NAME, MX_PRIV(q)));
	return mx_write(q, mp);
}
STATIC int
mx_r_data(queue_t *q, mblk_t *mp)
{
	/* data from below */
	printd(("%s: %p: M_DATA <-\n", MOD_NAME, MX_PRIV(q)));
	return mx_read(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR, M_HANGUP Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
mx_r_error(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	int err;

	switch (mx->state) {
	default:
		/* warn user before we error out the stream */
		if ((err = mx_disable_ind(q, mx, -mp->b_rptr[0])))
			return (err);
		/* fall through */
	case MXS_UNUSABLE:
		return (QR_PASSALONG);
	}
}
STATIC int
mx_r_hangup(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	int err;

	switch (mx->state) {
	default:
		/* warn user before we error out the stream */
		if ((err = mx_disable_ind(q, mx, -EPIPE)))
			return (err);
		/* fall through */
	case MXS_UNUSABLE:
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
mx_w_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return mx_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return mx_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mx_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return mx_w_ioctl(q, mp);
	}
	return (QR_PASSALONG);
}
STATIC INLINE int
mx_r_prim(queue_t *q, mblk_t *mp)
{
	/* Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return mx_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return mx_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mx_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	case M_ERROR:
		return mx_r_error(q, mp);
	case M_HANGUP:
		return mx_r_hangup(q, mp);
	case M_IOCACK:
		return mx_r_iocack(q, mp);
	case M_IOCNAK:
		return mx_r_iocnak(q, mp);
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
mx_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int err;

	MOD_INC_USE_COUNT;	/* keep module from unloading */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		int cmajor = getmajor(*devp);
		int cminor = getminor(*devp);
		struct mx *mx;

		for (mx = mx_opens; mx; mx = mx->next) {
			if (mx->u.dev.cmajor == cmajor && mx->u.dev.cminor == cminor) {
				MOD_DEC_USE_COUNT;
				return (ENXIO);
			}
		}
		if (!mx_alloc_priv(q, &mx_opens, devp, crp)) {
			MOD_DEC_USE_COUNT;
			return (ENOMEM);
		}
		/* generate immediate information request */
		if ((err = sdl_iocgconfig_req(q)) < 0) {
			mx_free_priv(q);
			MOD_DEC_USE_COUNT;
			return (-err);
		}
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
mx_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	mx_free_priv(q);
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
STATIC kmem_cachep_t mx_priv_cachep = NULL;
STATIC int
mx_init_caches(void)
{
	if (!mx_priv_cachep
	    && !(mx_priv_cachep =
		 kmem_create_cache("mx_priv_cachep", sizeof(struct mx), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate mx_priv_cachep", MOD_NAME);
		return (-ENOMEM);
	} else
		printd(("%s: initialized mx private structure cache\n", MOD_NAME));
	return (0);
}
STATIC int
mx_term_caches(void)
{
	if (mx_priv_cachep) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(mx_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy mx_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			printd(("%s: destroyed mx_priv_cachep\n", MOD_NAME));
#else
		kmem_cache_destroy(mx_priv_cachep);
#endif
	}
	return (0);
}

/*
 *  MX allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC struct mx *
mx_alloc_priv(queue_t *q, struct mx **chp, dev_t *devp, cred_t *crp)
{
	struct mx *mx;

	if ((mx = kmem_cache_alloc(mx_priv_cachep, GFP_ATOMIC))) {
		printd(("%s: %p: allocated mx private structure\n", MOD_NAME, mx));
		bzero(mx, sizeof(*mx));
		mx->u.dev.cmajor = getmajor(*devp);
		mx->u.dev.cminor = getminor(*devp);
		mx->cred = *crp;
		(mx->oq = RD(q))->q_ptr = mx_get(mx);
		(mx->iq = WR(q))->q_ptr = mx_get(mx);
		spin_lock_init(&mx->qlock);	/* "mx-queue-lock" */
		mx->o_prim = &mx_r_prim;
		mx->i_prim = &mx_w_prim;
		mx->o_wakeup = NULL;
		mx->i_wakeup = NULL;
		mx->state = MXS_UNINIT;	/* unitialized */
		spin_lock_init(&mx->lock);	/* "mx-priv-lock" */
		if ((mx->next = *chp))
			mx->next->prev = &mx->next;
		mx->prev = chp;
		*chp = mx_get(mx);
		printd(("%s: %p: linked mx private structure\n", MOD_NAME, mx));
		mx->config = mx_default;
	} else
		ptrace(("%s: ERROR: Could not allocate mx private structure\n", MOD_NAME));
	return (mx);
}
STATIC void
mx_free_priv(queue_t *q)
{
	struct mx *mx = MX_PRIV(q);
	psw_t flags = 0;

	ensure(mx, return);
	spin_lock_irqsave(&mx->lock, flags);
	{
		ss7_unbufcall((str_t *) mx);
		if ((*mx->prev = mx->next))
			mx->next->prev = mx->prev;
		mx->next = NULL;
		mx->prev = &mx->next;
		mx_put(mx);
		mx->oq->q_ptr = NULL;
		flushq(mx->oq, FLUSHALL);
		mx->oq = NULL;
		mx_put(mx);
		mx->iq->q_ptr = NULL;
		flushq(mx->iq, FLUSHALL);
		mx->iq = NULL;
	}
	spin_unlock_irqrestore(&mx->lock, flags);
	mx_put(mx);		/* final put */
	return;
}
STATIC struct mx *
mx_get(struct mx *mx)
{
	atomic_inc(&mx->refcnt);
	return (mx);
}
STATIC void
mx_put(struct mx *mx)
{
	if (atomic_dec_and_test(&mx->refcnt)) {
		kmem_cache_free(mx_priv_cachep, mx);
		printd(("%s: %p: freed mx private structure\n", MOD_NAME, mx));
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
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the SSCOP module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw mx_fmod = {
	.f_name = MOD_NAME,
	.f_str = &mx_sdlinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

STATIC int
mx_register_strmod(void)
{
	int err;

	if ((err = register_strmod(&mx_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
mx_unregister_strmod(void)
{
	int err;

	if ((err = unregister_strmod(&mx_fmod)) < 0)
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
mx_register_strmod(void)
{
	int err;

	if ((err = lis_register_strmod(&mx_sdlinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	if ((err = lis_register_module_qlock_option(err, LIS_QLOCK_NONE)) < 0) {
		lis_unregister_strmod(&mx_sdlinfo);
		return (err);
	}
	return (0);
}

STATIC int
mx_unregister_strmod(void)
{
	int err;

	if ((err = lis_unregister_strmod(&mx_sdlinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
mx_sdlinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = mx_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = mx_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		mx_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
mx_sdlterminate(void)
{
	int err;

	if ((err = mx_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = mx_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(mx_sdlinit);
module_exit(mx_sdlterminate);

#endif				/* LINUX */
