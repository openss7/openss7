/*****************************************************************************

 @(#) $RCSfile: sdt_tpi.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:47 $

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

 Last Modified $Date: 2006/10/17 11:55:47 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sdt_tpi.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:47 $"

static char const ident[] =
    "$RCSfile: sdt_tpi.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:47 $";

/*
 *  This is an SDT (Signalling Data Terminal) module which can be pushed over
 *  a TLI transport to effect an OpenSS7 Signalling Data Terminal.  When used
 *  in conjunction with the OpenSS7 SL (Signalling Link) module, this forms a
 *  complete SS7 over IP SS7 Link solution.
 *
 *  Unlike the M2PA module, which depends upon SCTP as a transport, this
 *  module does not really care what transport has been provided.  The
 *  transport stream can be connection-oriented or connectionless and can
 *  either preserve message boundaries or ignore them (stream operation).
 *  OpenSS7 STREAMS transports available to be used by this module include
 *  RAWIP, UDP, TCP, SCTP.  Although the module works over SSCOP-MCE
 *  transport, this is rather pointless as the MTP3b signalling link provides
 *  a better mechanism for SSCOP-MCE operation.  Also, although this module
 *  can be used for SCTP transport, a standard way to use SCTP for SS7
 *  signallign link transport would be M2PA instead of this module.
 */
#include <sys/os7/compat.h>

#include <linux/socket.h>

#include <tihdr.h>
// #include <xti.h>
#include <xti_inet.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>

#define SDT_TPI_DESCRIP	"SS7/IP SIGNALLING DATA TERMINAL (SDT) STREAMS MODULE."
#define SDT_TPI_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corporation.  All Rights Reserved."
#define SDT_TPI_REVISION	"OpenSS7 $RCSfile: sdt_tpi.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:47 $"
#define SDT_TPI_DEVICE	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define SDT_TPI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SDT_TPI_LICENSE	"GPL"
#define SDT_TPI_BANNER	SDT_TPI_DESCRIP	"\n" \
			SDT_TPI_COPYRIGHT	"\n" \
			SDT_TPI_REVISION	"\n" \
			SDT_TPI_DEVICE	"\n" \
			SDT_TPI_CONTACT	"\n"
#define SDT_TPI_SPLASH	SDT_TPI_DESCRIP	" - " \
			SDT_TPI_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(SDT_TPI_CONTACT);
MODULE_DESCRIPTION(SDT_TPI_DESCRIP);
MODULE_SUPPORTED_DEVICE(SDT_TPI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SDT_TPI_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sdt_tpi");
#endif
#endif				/* LINUX */

#ifdef LFS
#define SDT_TPI_MOD_ID		CONFIG_STREAMS_SDT_TPI_MODID
#define SDT_TPI_MOD_NAME	CONFIG_STREAMS_SDT_TPI_NAME
#endif

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

#define MOD_ID		SDT_TPI_MOD_ID
#define MOD_NAME	SDT_TPI_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SDT_TPI_BANNER
#else				/* MODULE */
#define MOD_BANNER	SDT_TPI_SPLASH
#endif				/* MODULE */

STATIC struct module_info sdt_rinfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_lowat = 1 << 10,		/* Lo water mark */
};

STATIC struct module_info sdt_winfo = {
	.mi_idnum = MOD_ID,		/* Module ID number */
	.mi_idname = MOD_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size accepted */
	.mi_hiwat = 1,			/* Hi water mark */
	.mi_lowat = 0,			/* Lo water mark */
};

STATIC int streamscall sdt_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int streamscall sdt_close(queue_t *, int, cred_t *);

STATIC struct qinit sdt_rinit = {
	.qi_putp = ss7_oput,		/* Read put (msg from below) */
	.qi_qopen = sdt_open,		/* Each open */
	.qi_qclose = sdt_close,		/* Last close */
	.qi_minfo = &sdt_rinfo,		/* Information */
};

STATIC struct qinit sdt_winit = {
	.qi_putp = ss7_iput,		/* Write put (msg from above) */
	.qi_minfo = &sdt_winfo,		/* Information */
};

STATIC struct streamtab sdt_tpiinfo = {
	.st_rdinit = &sdt_rinit,	/* Upper read queue */
	.st_wrinit = &sdt_winit,	/* Upper write queue */
};

/*
 *  =========================================================================
 *
 *  SS7IP-SDT Private Structure
 *
 *  =========================================================================
 */

typedef struct sdt {
	STR_DECLARATION (struct sdt);	/* stream declaration */
	mblk_t *rbuf;			/* normal data reassembly */
	mblk_t *xbuf;			/* expedited data reassembly */
	lmi_option_t option;		/* LMI options */
	struct {
		mblk_t *tx_cmp;		/* tx compression buffer */
		mblk_t *rx_cmp;		/* rx compression buffer */
		uint tx_repeat;		/* tx compression count */
		uint rx_repeat;		/* rx compression count */
		sdt_timers_t timers;	/* SDT protocol timers */
		sdt_config_t config;	/* SDT configuration */
		sdt_statem_t statem;	/* SDT state machine */
		sdt_notify_t notify;	/* SDT notification options */
		sdt_stats_t stats;	/* SDT statistics */
		sdt_stats_t stamp;	/* SDT statistics timestamps */
		lmi_sta_t statsp;	/* SDT statistics periods */
	} sdt;
	struct {
		long tickbytes;		/* traffic bytes per tick */
		long bytecount;		/* traffic bytes this tick */
		long timestamp;		/* traffic last tick */
		sdl_timers_t timers;	/* SDL protocol timers */
		sdl_config_t config;	/* SDL configuration */
		sdl_statem_t statem;	/* SDL state machine variables */
		sdl_notify_t notify;	/* SDL notification options */
		sdl_stats_t stats;	/* SDL statistics */
		sdl_stats_t stamp;	/* SDL statistics timestamps */
		lmi_sta_t statsp;	/* SDL statistics periods */
	} sdl;
	struct {
		uint state;		/* transport state */
		uint serv_type;		/* transport service type */
		uint sequence;		/* transport conind sequence no */
		size_t pdu_size;	/* transport maximum pdu size */
		size_t opt_size;	/* transport maximum options size */
		size_t add_size;	/* transport addrress size */
		struct sockaddr loc;	/* transport local address */
		struct sockaddr rem;	/* transport remote address */
	} t;
} sdt_t;
#define SDT_PRIV(__q) ((struct sdt *)(__q)->q_ptr)

struct sdt *sdt_opens = NULL;

STATIC struct sdt *sdt_alloc_priv(queue_t *, struct sdt **, dev_t *, cred_t *);
STATIC struct sdt *sdt_get(struct sdt *);
STATIC void sdt_put(struct sdt *);
STATIC void sdt_free_priv(queue_t *);

struct lmi_option lmi_default = {
	pvar:SS7_PVAR_ITUT_96,
	popt:0,
};

struct sdl_config sdl_default = {
	ifflags:0,
	iftype:SDL_TYPE_PACKET,
	ifrate:64000,
	ifgtype:SDL_GTYPE_UDP,
	ifgrate:10000000,
	ifmode:SDL_MODE_PEER,
	ifgmode:SDL_GMODE_NONE,
	ifgcrc:SDL_GCRC_NONE,
	ifclock:SDL_CLOCK_NONE,
	ifcoding:SDL_CODING_NONE,
	ifframing:SDL_FRAMING_NONE,
	ifleads:0,
	ifalarms:0,
	ifrxlevel:0,
	iftxlevel:0,
	ifsync:0,
	ifsyncsrc:{0, 0, 0, 0},
};

struct sdt_config sdt_default = {
	Tin:4,				/* AERM normal proving threshold */
	Tie:1,				/* AERM emergency proving threshold */
	T:64,				/* SUERM error threshold */
	D:256,				/* SUERM error rate parameter */
	t8:100 * HZ / 1000,		/* T8 timeout */
	Te:577169,			/* EIM error threshold */
	De:9308000,			/* EIM correct decrement */
	Ue:144292000,			/* EIM error increment */
	N:16,				/* octets per su in OCM */
	m:272,				/* maximum SIF size */
	b:8,				/* transmit block size */
	f:SDT_FLAGS_ONE,		/* one flag between frames */
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
 *  Primitive sent upstream
 *
 *  ------------------------------------------------------------------------
 */
#if 0
/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC INLINE int
m_error(queue_t *q, struct sdt *sdt, int err)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		sdt->t.state = TS_NOSTATES;
		sdt->i_state = LMI_UNUSABLE;
		printd(("%s: %p: <- M_ERROR\n", MOD_NAME, sdt));
		putnext(sdt->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  M_HANGUP
 *  -----------------------------------
 */
STATIC INLINE int
m_hangup(queue_t *q, struct sdt *sdt, int err)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_HANGUP;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		*(mp->b_wptr)++ = err < 0 ? -err : err;
		sdt->t.state = TS_NOSTATES;
		sdt->i_state = LMI_UNUSABLE;
		printd(("%s: %p: <- M_HANGUP\n", MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
lmi_info_ack(queue_t *q, struct sdt *sdt, caddr_t ppa_ptr, size_t ppa_len)
{
	mblk_t *mp;
	lmi_info_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + ppa_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_INFO_ACK;
		p->lmi_version = 1;
		p->lmi_state = sdt->i_state;
		p->lmi_max_sdu =
		    sdt->sdt.config.m + 1 + ((sdt->option.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_min_sdu = ((sdt->option.popt & SS7_POPT_XSN) ? 6 : 3);
		p->lmi_header_len = 0;
		p->lmi_ppa_style = LMI_STYLE2;
		bcopy(ppa_ptr, mp->b_wptr, ppa_len);
		mp->b_wptr += ppa_len;
		printd(("%s: %p: <- LMI_INFO_ACK\n", MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
lmi_ok_ack(queue_t *q, struct sdt *sdt, long prim)
{
	mblk_t *mp;
	lmi_ok_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_OK_ACK;
		p->lmi_correct_primitive = prim;
		switch (sdt->i_state) {
		case LMI_ATTACH_PENDING:
			sdt->i_state = LMI_DISABLED;
			break;
		case LMI_DETACH_PENDING:
			sdt->i_state = LMI_UNATTACHED;
			break;
		default:
			break;
		}
		p->lmi_state = sdt->i_state;
		printd(("%s: %p: <- LMI_OK_ACK\n", MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
lmi_error_ack(queue_t *q, struct sdt *sdt, long prim, ulong reason, ulong errno)
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
		switch (sdt->i_state) {
		case LMI_ATTACH_PENDING:
			sdt->i_state = LMI_UNATTACHED;
			break;
		case LMI_DETACH_PENDING:
			sdt->i_state = LMI_DISABLED;
			break;
		case LMI_ENABLE_PENDING:
			sdt->i_state = LMI_DISABLED;
			break;
		case LMI_DISABLE_PENDING:
			sdt->i_state = LMI_ENABLED;
			break;
		default:
			break;
		}
		p->lmi_state = sdt->i_state;
		printd(("%s: %p: <- LMI_ERROR_ACK\n", MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
lmi_enable_con(queue_t *q, struct sdt *sdt)
{
	mblk_t *mp;
	lmi_enable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ENABLE_CON;
		assure(sdt->i_state == LMI_ENABLE_PENDING);
		sdt->i_state = LMI_ENABLED;
		p->lmi_state = sdt->i_state;
		printd(("%s: %p: <- LMI_ENABLE_CON\n", MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
lmi_disable_con(queue_t *q, struct sdt *sdt)
{
	mblk_t *mp;
	lmi_disable_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_DISABLE_CON;
		assure(sdt->i_state == LMI_DISABLE_PENDING);
		sdt->i_state = LMI_DISABLED;
		p->lmi_state = sdt->i_state;
		printd(("%s: %p: <- LMI_DISABLE_CON\n", MOD_NAME, sdt));
		putnext(sdt->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
 *  LMI_OPTMGMT_ACK
 *  -----------------------------------
 */
STATIC INLINE int
lmi_optmgmt_ack(queue_t *q, struct sdt *sdt, ulong flags, caddr_t opt_ptr, size_t opt_len)
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
		printd(("%s: %p: <- LMI_OPTMGMT_ACK\n", MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
lmi_error_ind(queue_t *q, struct sdt *sdt, ulong errno, ulong reason)
{
	mblk_t *mp;
	lmi_error_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->lmi_primitive = LMI_ERROR_IND;
		p->lmi_errno = errno;
		p->lmi_reason = reason;
		p->lmi_state = sdt->i_state;
		printd(("%s: %p: <- LMI_ERROR_IND\n", MOD_NAME, sdt));
		putnext(sdt->oq, mp);
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
lmi_stats_ind(queue_t *q, struct sdt *sdt, ulong interval)
{
	if (canputnext(sdt->oq)) {
		mblk_t *mp;
		lmi_stats_ind_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_STATS_IND;
			p->lmi_interval = interval;
			p->lmi_timestamp = jiffies;
			printd(("%s: %p: <- LMI_STATS_IND\n", MOD_NAME, sdt));
			putnext(sdt->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  LMI_EVENT_IND
 *  -----------------------------------
 */
STATIC INLINE int
lmi_event_ind(queue_t *q, struct sdt *sdt, ulong oid, ulong level)
{
	if (canputnext(sdt->oq)) {
		mblk_t *mp;
		lmi_event_ind_t *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->lmi_primitive = LMI_EVENT_IND;
			p->lmi_objectid = oid;
			p->lmi_timestamp = jiffies;
			p->lmi_severity = level;
			printd(("%s: %p: <- LMI_EVENT_IND\n", MOD_NAME, sdt));
			putnext(sdt->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}
#endif

/*
 *  SDT_RC_SIGNAL_UNIT_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_signal_unit_ind(queue_t *q, struct sdt *sdt, mblk_t *dp, ulong count)
{
	printd(("%s: %p: <- SDT_RC_SIGNAL_UNIT_IND [%x/%x:%d x %lu]\n", MOD_NAME, sdt,
		dp->b_rptr[0], dp->b_rptr[1], msgdsize(dp), count));
	if (count) {
		if (canputnext(sdt->oq)) {
			if (count > 1) {
				mblk_t *mp;
				sdt_rc_signal_unit_ind_t *p;
				if ((mp = allocb(sizeof(*p), BPRI_MED))) {
					mp->b_datap->db_type = M_PROTO;
					p = (typeof(p)) mp->b_wptr;
					mp->b_wptr += sizeof(*p);
					p->sdt_primitive = SDT_RC_SIGNAL_UNIT_IND;
					p->sdt_count = count;
					mp->b_cont = dp;
					putnext(sdt->oq, mp);
					return (QR_ABSORBED);
				}
				rare();
				return (-ENOBUFS);
			} else {
				putnext(sdt->oq, dp);
				return (QR_ABSORBED);
			}
		}
		rare();
		return (-EBUSY);
	}
	swerr();
	return (-EFAULT);
}

#if 0
/*
 *  SDT_RC_CONGESTION_ACCEPT_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_congestion_accept_ind(queue_t *q, struct sdt *sdt)
{
	mblk_t *mp;
	sdt_rc_congestion_accept_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_RC_CONGESTION_ACCEPT_IND;
		printd(("%s: %p: <- SDT_RC_CONGESTION_ACCEPT_IND\n", MOD_NAME, sdt));
		putnext(sdt->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_RC_CONGESTION_DISCARD_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_congestion_discard_ind(queue_t *q, struct sdt *sdt)
{
	mblk_t *mp;
	sdt_rc_congestion_discard_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_RC_CONGESTION_DISCARD_IND;
		printd(("%s: %p: <- SDT_RC_CONGESTION_DISCARD_IND\n", MOD_NAME, sdt));
		putnext(sdt->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_RC_NO_CONGESTION_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_rc_no_congestion_ind(queue_t *q, struct sdt *sdt)
{
	mblk_t *mp;
	sdt_rc_no_congestion_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_RC_NO_CONGESTION_IND;
		printd(("%s: %p: <- SDT_RC_NO_CONGESTION_IND\n", MOD_NAME, sdt));
		putnext(sdt->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  SDT_IAC_CORRECT_SU_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_iac_correct_su_ind(queue_t *q, struct sdt *sdt)
{
	if (canputnext(sdt->oq)) {
		mblk_t *mp;
		sdt_iac_correct_su_ind_t *p;
		if ((mp = allocb(sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->sdt_primitive = SDT_IAC_CORRECT_SU_IND;
			printd(("%s: %p: <- SDT_IAC_CORRECT_SU_IND\n", MOD_NAME, sdt));
			putnext(sdt->oq, mp);
			return (QR_DONE);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  SDT_IAC_ABORT_PROVING_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_iac_abort_proving_ind(queue_t *q, struct sdt *sdt)
{
	mblk_t *mp;
	sdt_iac_abort_proving_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_IAC_ABORT_PROVING_IND;
		printd(("%s: %p: <- SDT_IAC_ABORT_PROVING_IND\n", MOD_NAME, sdt));
		putnext(sdt->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  SDT_LSC_LINK_FAILURE_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_lsc_link_failure_ind(queue_t *q, struct sdt *sdt)
{
	mblk_t *mp;
	sdt_lsc_link_failure_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_LSC_LINK_FAILURE_IND;
		printd(("%s: %p: <- SDT_LSC_LINK_FAILURE_IND\n", MOD_NAME, sdt));
		putnext(sdt->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
 *  SDT_TXC_TRANSMISSION_REQUEST_IND
 *  -----------------------------------
 */
STATIC INLINE int
sdt_txc_transmission_request_ind(queue_t *q, struct sdt *sdt)
{
	mblk_t *mp;
	sdt_txc_transmission_request_ind_t *p;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->sdt_primitive = SDT_TXC_TRANSMISSION_REQUEST_IND;
		printd(("%s: %p: <- SDT_TXC_TRANSMISSION_REQUEST_IND\n", MOD_NAME, sdt));
		putnext(sdt->oq, mp);
		return (QR_DONE);
	}
	rare();
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
 *  T_CONN_REQ
 *  ------------------------------------------------------------------------
 *  Generate a connection request as part of an LMI_ENABLE_REQ on a connection
 *  oriented client-side transport.
 */
STATIC INLINE int
t_conn_req(queue_t *q, struct sdt *sdt)
{
	caddr_t dst_ptr = (caddr_t) &sdt->t.rem, opt_ptr = NULL;
	size_t dst_len = sdt->t.add_size, opt_len = 0;
	mblk_t *mp, *dp = NULL;
	struct T_conn_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->PRIM_type = T_CONN_REQ;
		p->DEST_length = dst_len;
		p->DEST_offset = dst_len ? sizeof(*p) : 0;
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
		bcopy(dst_ptr, mp->b_wptr, dst_len);
		mp->b_wptr += dst_len;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		mp->b_cont = dp;
		assure(sdt->t.state == TS_IDLE);
		sdt->t.state = TS_WACK_CREQ;
		printd(("%s: %p: T_CONN_REQ ->\n", MOD_NAME, sdt));
		putnext(sdt->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  T_CONN_RES
 *  ------------------------------------------------------------------------
 *  Respond to a T_CONN_IND when in the LMI_ENABLED state and we are in server
 *  mode on a connection-oriented transport.
 */
STATIC INLINE int
t_conn_res(queue_t *q, struct sdt *sdt, long seq)
{
	caddr_t opt_ptr = NULL;
	size_t opt_len = 0;
	mblk_t *mp, *dp = NULL;
	struct T_conn_res *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->PRIM_type = T_CONN_RES;
		p->ACCEPTOR_id = (t_uscalar_t) (long) RD(sdt->iq->q_next);	/* accept on indicating
									   queue */
		p->OPT_length = opt_len;
		p->OPT_offset = opt_len ? sizeof(*p) : 0;
		p->SEQ_number = seq;
		sdt->t.sequence = seq;
		bcopy(opt_ptr, mp->b_wptr, opt_len);
		mp->b_wptr += opt_len;
		mp->b_cont = dp;
		assure(sdt->t.state == TS_WRES_CIND);
		sdt->t.state = TS_WACK_CRES;
		printd(("%s: %p: T_CONN_RES ->\n", MOD_NAME, sdt));
		putnext(sdt->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  T_DISCON_REQ
 *  ------------------------------------------------------------------------
 *  Perform a disconnect when performing an LMI_DISABLE_REQ on a
 *  connection-oriented transport which is not orderly release capable.
 */
STATIC INLINE int
t_discon_req(queue_t *q, struct sdt *sdt, ulong seq)
{
	mblk_t *mp, *dp = NULL;
	struct T_discon_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->PRIM_type = T_DISCON_REQ;
		p->SEQ_number = seq;
		mp->b_cont = dp;
		switch (sdt->t.state) {
		case TS_WCON_CREQ:
			sdt->t.state = TS_WACK_DREQ6;
			break;
		case TS_WRES_CIND:
			sdt->t.state = TS_WACK_DREQ7;
			break;
		case TS_DATA_XFER:
			sdt->t.state = TS_WACK_DREQ9;
			break;
		case TS_WIND_ORDREL:
			sdt->t.state = TS_WACK_DREQ10;
			break;
		case TS_WREQ_ORDREL:
			sdt->t.state = TS_WACK_DREQ11;
			break;
		default:
			swerr();
			sdt->t.state = TS_NOSTATES;
			break;
		}
		printd(("%s: %p: T_DISCON_REQ ->\n", MOD_NAME, sdt));
		putnext(sdt->iq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  T_DATA_REQ
 *  ------------------------------------------------------------------------
 *  Send normal data on a connection oriented transport.
 */
STATIC INLINE mblk_t *
t_data_req(queue_t *q, struct sdt *sdt)
{
	mblk_t *mp = NULL;
	if (canputnext(sdt->iq)) {
		struct T_data_req *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_DATA_REQ;
			p->MORE_flag = 0;
		}
	}
	return (mp);
}

#if 0
/*
 *  T_EXDATA_REQ
 *  ------------------------------------------------------------------------
 *  Send expedited data on a connection oriented transport.  (We never do
 *  this.)
 */
STATIC INLINE mblk_t *
t_exdata_req(queue_t *q, struct sdt *sdt)
{
	mblk_t *mp = NULL;
	if (bcanputnext(sdt->iq, 1)) {
		struct T_exdata_req *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = 1;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_EXDATA_REQ;
			p->MORE_flag = 0;
		}
	}
	return (mp);
}
#endif

/*
 *  T_INFO_REQ
 *  ------------------------------------------------------------------------
 *  Request information about the provider.  This can be performed at the time
 *  that the module is pushed, particularly to discover if we have been pushed
 *  over a connection-oriented transport, whether the transport supports
 *  orderly release and whether the transport is connectionless.  Also to
 *  discover the connection state of the transport to determine whether we
 *  need to attach or not.
 */
STATIC INLINE int
t_info_req(queue_t *q, struct sdt *sdt)
{
	mblk_t *mp;
	struct T_info_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->PRIM_type = T_INFO_REQ;
		assure(sdt->t.state == TS_NOSTATES);
		printd(("%s: %p: T_INFO_REQ ->\n", MOD_NAME, sdt));
		putnext(sdt->iq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  T_BIND_REQ
 *  ------------------------------------------------------------------------
 *  Binding is performed as part of the LMI_ENABLE_REQ on both connectionless
 *  and connection oriented transports.  The address is taken directly from
 *  the local address provided in the PPA.  The bind will be a listening
 *  stream if the transport is connection oriented and in server mode.
 */
STATIC INLINE int
t_bind_req(queue_t *q, struct sdt *sdt)
{
	caddr_t add_ptr = (caddr_t) &sdt->t.loc;
	size_t add_len = sdt->t.add_size;
#if 0
	ulong conind = ((sdt->t.serv_type == T_COTS || sdt->t.serv_type == T_COTS_ORD)
			&& (sdt->sdl.ifmode == SDL_MODE_SERVER)) ? 1 : 0;
#else
	/* 
	   always listen on COTS */
	ulong conind = (sdt->t.serv_type == T_COTS || sdt->t.serv_type == T_COTS_ORD) ? 1 : 0;
#endif
	mblk_t *mp;
	struct T_bind_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->PRIM_type = T_BIND_REQ;
		p->ADDR_length = add_len;
		p->ADDR_offset = add_len ? sizeof(*p) : 0;
		p->CONIND_number = conind;
		bcopy(add_ptr, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		assure(sdt->t.state == TS_UNBND);
		sdt->t.state = TS_WACK_BREQ;
		printd(("%s: %p: T_BIND_REQ ->\n", MOD_NAME, sdt));
		putnext(sdt->iq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  T_UNBIND_REQ
 *  ------------------------------------------------------------------------
 *  Unbinding is performed as part of the LMI_DISABLE_REQ on both
 *  connectionless and connection-oriented transports.
 */
STATIC INLINE int
t_unbind_req(queue_t *q, struct sdt *sdt)
{
	mblk_t *mp;
	struct T_unbind_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->PRIM_type = T_UNBIND_REQ;
		assure(sdt->t.state == TS_IDLE);
		sdt->t.state = TS_WACK_UREQ;
		printd(("%s: %p: T_UNBIND_REQ ->\n", MOD_NAME, sdt));
		putnext(sdt->iq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  T_UNITDATA_REQ
 *  ------------------------------------------------------------------------
 *  Unidata are sent on connectionless transports when in the LMI_ENABLED
 *  state.
 */
STATIC INLINE mblk_t *
t_unitdata_req(queue_t *q, struct sdt *sdt)
{
	mblk_t *mp = NULL;
	if (canputnext(sdt->iq)) {
		caddr_t dst_ptr = (caddr_t) &sdt->t.rem, opt_ptr = NULL;
		size_t dst_len = sdt->t.add_size, opt_len = 0;
		struct T_unitdata_req *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + dst_len + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_UNITDATA_REQ;
			p->DEST_length = dst_len;
			p->DEST_offset = dst_len ? sizeof(*p) : 0;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) + dst_len : 0;
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
		}
	}
	return (mp);
}

#if 0
/*
 *  T_OPTMGMT_REQ
 *  ------------------------------------------------------------------------
 *  Options may be requested for particular transport types once the module is
 *  pushed over the transport.
 */
STATIC INLINE int
t_optmgmt_req(queue_t *q, struct sdt *sdt, caddr_t opt_ptr, size_t opt_len, ulong flags)
{
	if (canputnext(sdt->iq)) {
		mblk_t *mp;
		struct T_optmgmt_req *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_OPTMGMT_REQ;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			p->MGMT_flags = flags;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			printd(("%s: %p: T_OPTMGMT_REQ ->\n", MOD_NAME, sdt));
			putnext(sdt->iq, mp);
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}
#endif

/*
 *  T_ORDREL_REQ
 *  ------------------------------------------------------------------------
 *  Orderly release is performed as part of the LMI_DISABLE_REQ on capable
 *  connection oriented transports.
 */
STATIC INLINE int
t_ordrel_req(queue_t *q, struct sdt *sdt)
{
	if (canputnext(sdt->iq)) {
		mblk_t *mp;
		struct T_ordrel_req *p;
		if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_ORDREL_REQ;
			assure(sdt->t.state == TS_DATA_XFER);
			printd(("%s: %p: T_ORDREL_REQ ->\n", MOD_NAME, sdt));
			putnext(sdt->iq, mp);
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

#if 0
/*
 *  T_OPTDATA_REQ
 *  ------------------------------------------------------------------------
 *  Option data rather than data or exdata may be sent on some connection
 *  oriented transports where control over data options is necessary.
 */
STATIC INLINE int
t_optdata_req(queue_t *q, struct sdt *sdt, mblk_t *dp)
{
	if (canputnext(sdt->iq)) {
		caddr_t opt_ptr = NULL;
		size_t opt_len = 0;
		int flags = 0;
		mblk_t *mp;
		struct T_optdata_req *p;
		if ((mp = ss7_allocb(q, sizeof(*p) + opt_len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = (flags & T_ODF_EX) ? 1 : 0;
			p = (typeof(p)) mp->b_wptr;
			mp->b_wptr += sizeof(*p);
			p->PRIM_type = T_OPTDATA_REQ;
			p->DATA_flag = flags;
			p->OPT_length = opt_len;
			p->OPT_offset = opt_len ? sizeof(*p) : 0;
			bcopy(opt_ptr, mp->b_wptr, opt_len);
			mp->b_wptr += opt_len;
			mp->b_cont = dp;
			assure(sdt->t.state == TS_IDLE);
			printd(("%s: %p: T_OPTDATA_REQ ->\n", MOD_NAME, sdt));
			putnext(sdt->iq, mp);
			return (0);
		}
		rare();
		return (-ENOBUFS);
	}
	rare();
	return (-EBUSY);
}

/*
 *  T_ADDR_REQ
 *  ------------------------------------------------------------------------
 *  Not useful.
 */
STATIC INLINE int
t_addr_req(queue_t *q, struct sdt *sdt)
{
	mblk_t *mp;
	struct T_addr_req *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		mp->b_wptr += sizeof(*p);
		p->PRIM_type = T_ADDR_REQ;
		printd(("%s: %p: T_ADDR_REQ ->\n", MOD_NAME, sdt));
		putnext(sdt->iq, mp);
		return (0);
	}
	rare();
	return (-ENOBUFS);
}
#endif

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
enum { tall, t8, t9 };

STATIC int sdt_t8_timeout(struct sdt *);
STATIC void streamscall
sdt_t8_expiry(caddr_t data)
{
	ss7_do_timeout(data, "t8", "sdt-tpi", &((struct sdt *) data)->sdt.timers.t8,
		       (int (*)(struct head *)) &sdt_t8_timeout, &sdt_t8_expiry);
}
STATIC void
sdt_stop_timer_t8(struct sdt *sdt)
{
	ss7_stop_timer((struct head *) sdt, "t8", "sdt-tpi", &sdt->sdt.timers.t8);
}
STATIC void
sdt_start_timer_t8(struct sdt *sdt)
{
	ss7_start_timer((struct head *) sdt, "t8", "sdt-tpi", &sdt->sdt.timers.t8, &sdt_t8_expiry,
			sdt->sdt.config.t8);
};
STATIC int sdt_t9_timeout(struct sdt *);
STATIC void streamscall
sdt_t9_expiry(caddr_t data)
{
	ss7_do_timeout(data, "t9", "sdt-tpi", &((struct sdt *) data)->sdl.timers.t9,
		       (int (*)(struct head *)) &sdt_t9_timeout, &sdt_t9_expiry);
}
STATIC void
sdt_stop_timer_t9(struct sdt *sdt)
{
	ss7_stop_timer((struct head *) sdt, "t9", "sdt-tpi", &sdt->sdl.timers.t9);
}
STATIC void
sdt_start_timer_t9(struct sdt *sdt)
{
	ss7_start_timer((struct head *) sdt, "t9", "sdt-tpi", &sdt->sdl.timers.t9, &sdt_t9_expiry,
			sdt->sdl.timestamp - jiffies);
};

STATIC INLINE void
__sdt_timer_stop(struct sdt *s, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/* 
		   fall through */
	case t8:
		sdt_stop_timer_t8(s);
		if (single)
			break;
		/* 
		   fall through */
	case t9:
		sdt_stop_timer_t9(s);
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
sdt_timer_stop(struct sdt *s, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&s->lock, flags);
	{
		__sdt_timer_stop(s, t);
	}
	spin_unlock_irqrestore(&s->lock, flags);
}
STATIC INLINE void
sdt_timer_start(struct sdt *s, const uint t)
{
	psw_t flags;
	spin_lock_irqsave(&s->lock, flags);
	{
		__sdt_timer_stop(s, t);
		switch (t) {
		case t8:
			sdt_start_timer_t8(s);
			break;
		case t9:
			sdt_start_timer_t9(s);
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
 *  SDT State Machines
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void
sdt_aerm_stop(queue_t *q, struct sdt *sdt)
{
	sdt->sdt.statem.aerm_state = SDT_STATE_IDLE;
	sdt->sdt.statem.Ti = sdt->sdt.config.Tin;
}

STATIC INLINE void
sdt_eim_stop(queue_t *q, struct sdt *sdt)
{
	sdt->sdt.statem.eim_state = SDT_STATE_IDLE;
	sdt_timer_stop(sdt, t8);
}

STATIC INLINE void
sdt_suerm_stop(queue_t *q, struct sdt *sdt)
{
	sdt_eim_stop(q, sdt);
	sdt->sdt.statem.suerm_state = SDT_STATE_IDLE;
}

STATIC INLINE int
sdt_lsc_link_failure(queue_t *q, struct sdt *sdt)
{
	return sdt_lsc_link_failure_ind(q, sdt);
}

STATIC INLINE void
sdt_daedr_start(queue_t *q, struct sdt *sdt)
{
	sdt->sdt.statem.daedr_state = SDT_STATE_IN_SERVICE;
	sdt->sdl.statem.rx_state = SDL_STATE_IN_SERVICE;
	sdt->sdl.config.ifflags |= (SDL_IF_UP | SDL_IF_RX_RUNNING);
}

STATIC INLINE void
sdt_eim_start(queue_t *q, struct sdt *sdt)
{
	sdt->sdt.statem.Ce = 0;
	sdt->sdt.statem.interval_error = 0;
	sdt->sdt.statem.su_received = 0;
	sdt_timer_start(sdt, t8);
	sdt->sdt.statem.eim_state = SDT_STATE_MONITORING;
}

STATIC INLINE void
sdt_suerm_start(queue_t *q, struct sdt *sdt)
{
	if (sdt->option.popt & SS7_POPT_HSL)
		sdt_eim_start(q, sdt);
	else {
		sdt->sdt.statem.Cs = 0;
		sdt->sdt.statem.Ns = 0;
		sdt->sdt.statem.suerm_state = SDT_STATE_IN_SERVICE;
	}
}

STATIC INLINE void
sdt_aerm_set_ti_to_tie(queue_t *q, struct sdt *sdt)
{
	if (sdt->sdt.statem.aerm_state == SDT_STATE_IDLE)
		sdt->sdt.statem.Ti = sdt->sdt.config.Tie;
}

STATIC INLINE void
sdt_aerm_start(queue_t *q, struct sdt *sdt)
{
	sdt->sdt.statem.Ca = 0;
	sdt->sdt.statem.aborted_proving = 0;
	sdt->sdt.statem.aerm_state = SDT_STATE_MONITORING;
}

STATIC INLINE int
sdt_iac_correct_su(queue_t *q, struct sdt *sdt)
{
	return sdt_iac_correct_su_ind(q, sdt);
}

STATIC INLINE int
sdt_iac_abort_proving(queue_t *q, struct sdt *sdt)
{
	return sdt_iac_abort_proving_ind(q, sdt);
}

STATIC INLINE void
sdt_daedt_start(queue_t *q, struct sdt *sdt)
{
	sdt->sdt.statem.daedt_state = SDT_STATE_IN_SERVICE;
	sdt->sdl.statem.tx_state = SDL_STATE_IN_SERVICE;
	sdt->sdl.config.ifflags |= (SDL_IF_UP | SDL_IF_TX_RUNNING);
}

STATIC INLINE void
sdt_aerm_set_ti_to_tin(queue_t *q, struct sdt *sdt)
{
	if (sdt->sdt.statem.aerm_state == SDT_STATE_IDLE)
		sdt->sdt.statem.Ti = sdt->sdt.config.Tin;
}

STATIC INLINE int
sdt_aerm_su_in_error(queue_t *q, struct sdt *sdt)
{
	int err;
	if (sdt->sdt.statem.aerm_state == SDT_STATE_MONITORING) {
		sdt->sdt.statem.Ca++;
		if (sdt->sdt.statem.Ca == sdt->sdt.statem.Ti) {
			if ((err = sdt_iac_abort_proving(q, sdt))) {
				sdt->sdt.statem.Ca--;
				return (err);
			}
			sdt->sdt.statem.aborted_proving = 1;
			sdt->sdt.statem.aerm_state = SDT_STATE_IDLE;
		}
	}
	return (QR_DONE);
}

STATIC INLINE int
sdt_aerm_correct_su(queue_t *q, struct sdt *sdt)
{
	int err;
	if (sdt->sdt.statem.aerm_state == SDT_STATE_IDLE) {
		if (sdt->sdt.statem.aborted_proving) {
			if ((err = sdt_iac_correct_su(q, sdt)))
				return (err);
			sdt->sdt.statem.aborted_proving = 0;
		}
	}
	return (QR_DONE);
}

STATIC INLINE int
sdt_suerm_su_in_error(queue_t *q, struct sdt *sdt)
{
	int err;
	if (sdt->sdt.statem.suerm_state == SDT_STATE_IN_SERVICE) {
		sdt->sdt.statem.Cs++;
		if (sdt->sdt.statem.Cs >= sdt->sdt.config.T) {
			if ((err = sdt_lsc_link_failure(q, sdt))) {
				sdt->sdt.statem.Cs--;
				return (err);
			}
			sdt->sdt.statem.Ca--;
			sdt->sdt.statem.suerm_state = SDT_STATE_IDLE;
		} else {
			sdt->sdt.statem.Ns++;
			if (sdt->sdt.statem.Ns >= sdt->sdt.config.D) {
				sdt->sdt.statem.Ns = 0;
				if (sdt->sdt.statem.Cs)
					sdt->sdt.statem.Cs--;
			}
		}
	}
	return (QR_DONE);
}

STATIC INLINE void
sdt_eim_su_in_error(queue_t *q, struct sdt *sdt)
{
	if (sdt->sdt.statem.eim_state == SDT_STATE_MONITORING)
		sdt->sdt.statem.interval_error = 1;
}

STATIC INLINE void
sdt_suerm_correct_su(queue_t *q, struct sdt *sdt)
{
	if (sdt->sdt.statem.suerm_state == SDT_STATE_IN_SERVICE) {
		sdt->sdt.statem.Ns++;
		if (sdt->sdt.statem.Ns >= sdt->sdt.config.D) {
			sdt->sdt.statem.Ns = 0;
			if (sdt->sdt.statem.Cs)
				sdt->sdt.statem.Cs--;
		}
	}
}

STATIC INLINE void
sdt_eim_correct_su(queue_t *q, struct sdt *sdt)
{
	if (sdt->sdt.statem.eim_state == SDT_STATE_MONITORING)
		sdt->sdt.statem.su_received = 1;
}

STATIC INLINE int
sdt_daedr_correct_su(queue_t *q, struct sdt *sdt)
{
	sdt_eim_correct_su(q, sdt);
	sdt_suerm_correct_su(q, sdt);
	return sdt_aerm_correct_su(q, sdt);
}

/*
 *  Hooks to underlying driver
 *  -----------------------------------
 */
STATIC INLINE int
sdt_daedr_su_in_error(queue_t *q, struct sdt *sdt)
{
	int err;
	if (sdt->sdt.statem.daedr_state != SDT_STATE_IDLE) {
		/* 
		   cancel compression */
		if (sdt->sdt.rx_cmp) {
			printd(("SU in error\n"));
			freemsg(xchg(&sdt->sdt.rx_cmp, NULL));
		}
		sdt_eim_su_in_error(q, sdt);
		if ((err = sdt_suerm_su_in_error(q, sdt)))
			return (err);
		if ((err = sdt_aerm_su_in_error(q, sdt)))
			return (err);
		sdt->sdt.stats.rx_sus_in_error++;
	}
	return (QR_DONE);
}

/*
 *  NOTES:  Some notes on receive SU compression.
 *
 *  1) When doing receive compression, it is extremely important that the first TWO copies be
 *     delivered to the upper layer.  This is for several reasons.  Some parts of the state
 *     machine requires this double action such as: SIN/SIE when no SIO received in not aligned
 *     state: the first SIN moves the state machine to aligned and the second SIN/SIE moves the
 *     state machine to proving.  Also, when there are abnormal sequence numbers or indicator
 *     bits FISUs, it is necessary that two of the FISUs be delivered to move the state machine
 *     to out of service.  Therefore, the compression algorithm delivers the first two copies
 *     of each repetition.
 * 
 *  2) If we compressed a number of copies, deliver the copies with the number to the upper
 *     layer.
 *
 *  3) We always compress LSSUs, even if they are SIBs or have an invalid SIO field.  This is
 *     because of network outages which have been experienced in the past from processing each
 *     LSSU under error conditions (when they are just discarded by the upper layer).
 */
STATIC INLINE int
sdt_daedr_received_bits(queue_t *q, struct sdt *sdt, mblk_t *mp)
{
	int err, i, len, mlen, min_len, max_len, lmax, li, sif, xsn;
	if (sdt->sdt.statem.daedr_state == SDT_STATE_IDLE)
		goto discard;
	xsn = sdt->option.popt & SS7_POPT_XSN;
	mlen = xsn ? 8 : 5;
	min_len = mlen - 2;
	max_len = min_len + sdt->sdt.config.m + 1;
	printd(("%s: %p: SDT_DAEDR_RECEIVED_BITS [%x/%x:%d]<-\n", MOD_NAME, sdt, mp->b_rptr[0],
		mp->b_rptr[1], msgdsize(mp)));
	ensure(mp, return (-EFAULT));
	len = msgdsize(mp);
	if (len > max_len || min_len > len) {
		if (len > max_len) {
			if ((err = sdt_daedr_su_in_error(q, sdt)))
				goto error;
			sdt->sdt.stats.rx_frame_too_long++;
		}
		if (len < min_len) {
			if ((err = sdt_daedr_su_in_error(q, sdt)))
				goto error;
			sdt->sdt.stats.rx_frame_too_short++;
		}
		goto error_su;
	}
	lmax = xsn ? 0x1ff : 0x3f;
	li = xsn ? mp->b_rptr[4] | (mp->b_rptr[5] << 8) : mp->b_rptr[2];
	sif = len - min_len;
	if ((li &= lmax) == sif || (li == lmax && sif > lmax))
		goto good_su;
	if ((err = sdt_daedr_su_in_error(q, sdt)))
		goto error;
	sdt->sdt.stats.rx_length_error++;
      error_su:
	if (sdt->sdt.rx_cmp)
		freemsg(xchg(&sdt->sdt.rx_cmp, NULL));
	goto done;
      good_su:
	if (sdt->sdt.rx_cmp) {
		if (len > mlen || len < min_len || len != msgdsize(sdt->sdt.rx_cmp))
			goto no_match;
		for (i = 0; i < len; i++)
			if (mp->b_rptr[i] != sdt->sdt.rx_cmp->b_rptr[i])
				goto no_match;
		if (sdt->sdt.rx_repeat++) {
			if ((err = sdt_daedr_correct_su(q, sdt))) {
				sdt->sdt.rx_repeat--;
				goto error;
			}
			printd(("%s: %p: Compressing (%x/%x:%d)\n", MOD_NAME, sdt,
				mp->b_rptr[0], mp->b_rptr[1], msgdsize(mp)));
			sdt->sdt.stats.rx_sus_compressed++;
			goto done;
		}
		goto deliver;
	      no_match:
		if (sdt->sdt.rx_repeat > 1) {
			if ((err =
			     sdt_rc_signal_unit_ind(q, sdt, sdt->sdt.rx_cmp,
						    sdt->sdt.rx_repeat)) == QR_ABSORBED)
				sdt->sdt.rx_cmp = NULL;
			else {
				sdt->sdt.stats.rx_buffer_overflows++;
				goto error;
			}
		}
		sdt->sdt.rx_repeat = 0;
	}
	if (len <= mlen) {
		if (sdt->sdt.rx_cmp || (sdt->sdt.rx_cmp = allocb(mlen, BPRI_HI))) {
			bcopy(mp->b_rptr, sdt->sdt.rx_cmp->b_rptr, len);
			sdt->sdt.rx_cmp->b_wptr = sdt->sdt.rx_cmp->b_rptr + len;
			sdt->sdt.rx_repeat = 0;
		}
	}
      deliver:
	if ((err = sdt_daedr_correct_su(q, sdt)))
		goto error;
	printd(("%s: %p: Delivering (%x/%x:%d)\n", MOD_NAME, sdt, mp->b_rptr[0], mp->b_rptr[1],
		msgdsize(mp)));
	if ((err = sdt_rc_signal_unit_ind(q, sdt, mp, 1)) == QR_ABSORBED)
		goto absorbed;
	rare();
	sdt->sdt.stats.rx_buffer_overflows++;
      error:
	return (err);
      discard:
	rare();
      done:
	return (QR_DONE);
      absorbed:
	return (QR_ABSORBED);
}

/*
 *  ========================================================================
 *
 *  EVENTS
 *
 *  ========================================================================
 */
/*
 *  RX WAKEUP
 *  -----------------------------------
 */
STATIC int
sdt_rx_wakeup(queue_t *q)
{
	(void) q;
	return (0);
}
STATIC streamscall void
sdt_r_wakeup(queue_t *q)
{
	sdt_rx_wakeup(q);
}

/*
 *  TX WAKEUP
 *  -----------------------------------
 */
STATIC int sdt_send_data(queue_t *q, mblk_t *dp, int norepeat);
STATIC int
sdt_tx_wakeup(queue_t *q)
{
	struct sdt *sdt = SDT_PRIV(q);
	mblk_t *mp;
	int err;
	if (sdt->sdt.statem.daedt_state == SDT_STATE_IDLE)
		goto discard;
	if (sdt->sdl.config.ifclock == SDL_CLOCK_NONE)
		goto done;
	if (sdt->iq->q_count)
		goto eagain;
	if (!sdt->sdt.tx_cmp)
		goto overflow;
	if (sdt->sdl.config.ifclock == SDL_CLOCK_NONE)
		goto done;
	while (canputnext(sdt->iq)) {
		if (!(mp = dupmsg(sdt->sdt.tx_cmp)))
			goto enobufs;
		if ((err = sdt_send_data(sdt->iq, mp, 1)) != QR_ABSORBED) {
			freemsg(mp);
			return (err);
		}
		switch (sdt->sdl.config.ifclock) {
		case SDL_CLOCK_TICK:
			if (!(sdt->option.popt & SS7_POPT_PCR) && !sdt->iq->q_count) {
				printd(("%s: %p: %s sleeping for 30 ms\n", MOD_NAME, sdt,
					__FUNCTION__));
				sdt->sdl.timestamp = jiffies + 3;
				sdt_timer_start(sdt, t9);
				goto eagain;
			}
		}
	}
	rare();
	return (-EBUSY);
      enobufs:
	rare();
	sdt->sdt.stats.tx_buffer_overflows++;
	return (-ENOBUFS);
      overflow:
	seldom();
	sdt->sdt.stats.tx_buffer_overflows++;
	return (QR_DONE);
      eagain:
	rare();
	return (-EAGAIN);
      done:
	return (QR_DONE);
      discard:
	seldom();
	return (QR_DONE);
}
STATIC streamscall void
sdt_w_wakeup(queue_t *q)
{
	sdt_tx_wakeup(q);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Timer Events
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  T8 TIMEOUT
 *  -----------------------------------
 */
STATIC int
sdt_t8_timeout(struct sdt *sdt)
{
	queue_t *q = NULL;
	int err;
	if (sdt->sdt.statem.eim_state == SDT_STATE_MONITORING) {
		sdt_timer_start(sdt, t8);
		if (sdt->sdt.statem.su_received) {
			sdt->sdt.statem.su_received = 0;
			if (!sdt->sdt.statem.interval_error) {
				if ((sdt->sdt.statem.Ce -= sdt->sdt.config.De) < 0)
					sdt->sdt.statem.Ce = 0;
				return (QR_DONE);
			}
		}
		sdt->sdt.statem.Ce += sdt->sdt.config.Ue;
		if (sdt->sdt.statem.Ce > sdt->sdt.config.Te) {
			if ((err = sdt_lsc_link_failure(q, sdt))) {
				sdt->sdt.statem.Ce -= sdt->sdt.config.Ue;
				return (err);
			}
			sdt->sdt.statem.eim_state = SDT_STATE_IDLE;
		}
		sdt->sdt.statem.interval_error = 0;
	}
	return (QR_DONE);
}

/*
 *  T9 EXPIRY
 *  -----------------------------------
 */
STATIC int
sdt_t9_timeout(struct sdt *sdt)
{
	qenable(sdt->iq);
	return (QR_DONE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  LM User -> LM Provider (SS7IP) Primitives
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  LMI_INFO_REQ
 *  -----------------------------------
 */
STATIC int
lmi_info_req(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	lmi_info_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	return lmi_info_ack(q, sdt, NULL, 0);
      emsgsize:
	return lmi_error_ack(q, sdt, LMI_INFO_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_ATTACH_REQ
 *  -----------------------------------
 *  Attachment requires a local address as a PPA (Physical Point of Attachment).  The local
 *  address is necessary for T_BIND on both connection oriented and connectionless transports.
 *  For style 2 transports, we copy the PPA and bind the transport with T_BIND.  Style 1
 *  transports are already bound and this primitive is invalid for style 1 transports.
 */
STATIC int
lmi_attach_req(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	lmi_attach_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sdt->i_state == LMI_UNUSABLE)
		goto eagain;
	if (sdt->i_style != LMI_STYLE2)
		goto eopnotsupp;
	if (sdt->i_state != LMI_UNATTACHED)
		goto outstate;
	sdt->i_state = LMI_ATTACH_PENDING;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p) + sdt->t.add_size)
		goto badppa;
	bcopy(p->lmi_ppa, &sdt->t.loc, sdt->t.add_size);
	/* 
	   start bind in motion */
	return t_bind_req(q, sdt);
      badppa:
	ptrace(("%s: PROTO: bad ppa (too short)\n", MOD_NAME));
	return lmi_error_ack(q, sdt, LMI_ATTACH_REQ, LMI_BADPPA, EMSGSIZE);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return lmi_error_ack(q, sdt, LMI_ATTACH_REQ, LMI_OUTSTATE, EPROTO);
      eopnotsupp:
	ptrace(("%s: PROTO: primitive not support for style\n", MOD_NAME));
	return lmi_error_ack(q, sdt, LMI_ATTACH_REQ, LMI_NOTSUPP, EOPNOTSUPP);
      eagain:
	/* 
	   wait for stream to become usable */
	rare();
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return lmi_error_ack(q, sdt, LMI_ATTACH_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_DETACH_REQ
 *  -----------------------------------
 *  Detaching requires that the stream be detached from the local address.  This results in a
 *  T_UNBIND connection oriented and connectionless transports.  For style 2 transports, we 
 *  perform the unbinding operation.  For style 1 transports, we were already bound and this
 *  primitive is invalid for style 1 transports.
 */
STATIC int
lmi_detach_req(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	lmi_detach_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sdt->i_state == LMI_UNUSABLE)
		goto eagain;
	if (sdt->i_style != LMI_STYLE2)
		goto eopnotsupp;
	if (sdt->i_state != LMI_DISABLED)
		goto outstate;
	sdt->i_state = LMI_DETACH_PENDING;
	bzero(&sdt->t.loc, sdt->t.add_size);
	/* 
	   start unbind in motion */
	return t_unbind_req(q, sdt);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return lmi_error_ack(q, sdt, LMI_DETACH_REQ, LMI_OUTSTATE, EPROTO);
      eopnotsupp:
	ptrace(("%s: PROTO: primitive not support for style\n", MOD_NAME));
	return lmi_error_ack(q, sdt, LMI_ATTACH_REQ, LMI_NOTSUPP, EOPNOTSUPP);
      eagain:
	/* 
	   wait for stream to become usable */
	rare();
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return lmi_error_ack(q, sdt, LMI_DETACH_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_ENABLE_REQ
 *  -----------------------------------
 *  For style 1 connection oriented transports, we are completely connected and can simply
 *  acknowledge the enable.  For style 1 connectionless transports we need to know the remote
 *  address of the peer and require a remote address as part of the enable.  For style 2
 *  connection oriented transports we need to know the remote address for connection or if no
 *  remote address is provided, simply stay listening.  (Note: style 2 connection oriented
 *  transports are quite insecure for this reason.  Style 1 is preferrable for connection
 *  oriented transports for a number of reasons).
 */
STATIC int
lmi_enable_req(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	lmi_enable_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sdt->i_state == LMI_UNUSABLE)
		goto eagain;
	if (sdt->i_state != LMI_DISABLED)
		goto outstate;
	sdt->i_state = LMI_ENABLE_PENDING;
	switch (sdt->t.serv_type) {
	case T_CLTS:
		assure(sdt->t.state == TS_IDLE);
		if (mp->b_wptr < mp->b_rptr + sizeof(*p) + sdt->t.add_size)
			goto badppa;
		bcopy(p->lmi_rem, &sdt->t.rem, sdt->t.add_size);
		return lmi_enable_con(q, sdt);
	case T_COTS:
	case T_COTS_ORD:
		switch (sdt->i_style) {
		case LMI_STYLE1:
			assure(sdt->t.state == TS_DATA_XFER);
			return lmi_enable_con(q, sdt);
		case LMI_STYLE2:
			assure(sdt->t.state == TS_IDLE);
			if (mp->b_wptr == mp->b_rptr + sizeof(*p))	/* wait for T_CONN_IND */
				return (QR_DONE);
			if (mp->b_wptr < mp->b_rptr + sizeof(*p) + sdt->t.add_size)
				goto badppa;
			bcopy(p->lmi_rem, &sdt->t.rem, sdt->t.add_size);
			/* 
			   start connection in motion */
			return t_conn_req(q, sdt);
		}
	}
      badppa:
	ptrace(("%s: PROTO: bad ppa (too short)\n", MOD_NAME));
	return lmi_error_ack(q, sdt, LMI_ATTACH_REQ, LMI_BADPPA, EMSGSIZE);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return lmi_error_ack(q, sdt, LMI_ENABLE_REQ, LMI_OUTSTATE, EPROTO);
      eagain:
	/* 
	   wait for stream to become usable */
	rare();
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return lmi_error_ack(q, sdt, LMI_ENABLE_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_DISABLE_REQ
 *  -----------------------------------
 *  For style 1 connection oriented transports, we stay completely connected and can simply
 *  acknowledge the disable.  For style 1 connectionless transports we may also simply
 *  acknowledge the disable.  Fort style 2 connection oriented transports we need to
 *  disconnect the connection.
 */
STATIC int
lmi_disable_req(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	lmi_disable_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (sdt->i_state == LMI_UNUSABLE)
		goto eagain;
	if (sdt->i_state != LMI_ENABLED)
		goto outstate;
	sdt->i_state = LMI_DISABLE_PENDING;
	if (sdt->i_style == LMI_STYLE1 || sdt->t.serv_type == T_CLTS)
		return lmi_disable_con(q, sdt);
	/* 
	   start disconnect in motion */
	if (sdt->t.serv_type == T_COTS_ORD)
		return t_ordrel_req(q, sdt);
	return t_discon_req(q, sdt, 0);
      outstate:
	ptrace(("%s: PROTO: out of state\n", MOD_NAME));
	return lmi_error_ack(q, sdt, LMI_DISABLE_REQ, LMI_OUTSTATE, EPROTO);
      eagain:
	/* 
	   wait for stream to become available */
	rare();
	return (-EAGAIN);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return lmi_error_ack(q, sdt, LMI_DISABLE_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  LMI_OPTMGMT_REQ
 *  -----------------------------------
 */
STATIC int
lmi_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	lmi_optmgmt_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	goto eopnotsupp;
      eopnotsupp:
	ptrace(("%s: PROTO: Primitive is not supported\n", MOD_NAME));
	return lmi_error_ack(q, sdt, LMI_OPTMGMT_REQ, LMI_NOTSUPP, EOPNOTSUPP);
      emsgsize:
	ptrace(("%s: PROTO: M_PROTO block too short\n", MOD_NAME));
	return lmi_error_ack(q, sdt, LMI_OPTMGMT_REQ, LMI_PROTOSHORT, EMSGSIZE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDT User -> SDT Provider (SS7IP) Primitives
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_DATA
 *  -----------------------------------
 */
STATIC int
sdt_send_data(queue_t *q, mblk_t *dp, int norepeat)
{
	struct sdt *sdt = SDT_PRIV(q);
	if (sdt->i_state != LMI_ENABLED)
		goto discard;
	if (sdt->sdt.statem.daedt_state == SDT_STATE_IDLE)
		goto discard;
	if (!canputnext(sdt->iq))
		goto ebusy;
	{
		mblk_t *mp, *cp = NULL;
		int len = msgdsize(dp);
		int hlen = (sdt->option.popt & SS7_POPT_XSN) ? 6 : 3;
		int mlen = hlen + 2;
		int li, sio;
		switch (sdt->sdl.config.ifclock) {
		case SDL_CLOCK_TICK:
		case SDL_CLOCK_SHAPER:
			if (sdt->sdl.timestamp > jiffies) {
				printd(("%s: %p: %s sleeping for %ld ms\n", MOD_NAME, sdt,
					__FUNCTION__, sdt->sdl.timestamp - jiffies));
				sdt_timer_start(sdt, t9);
				goto eagain;
			}
			if (sdt->sdl.timestamp < jiffies) {
				sdt->sdl.bytecount = 0;
				sdt->sdl.timestamp = jiffies;
			}
		}
		if (sdt->t.serv_type == T_CLTS) {
			if (!(mp = t_unitdata_req(q, sdt)))
				goto enobufs;
		} else {
			if (!(mp = t_data_req(q, sdt)))
				goto enobufs;
		}
		if (norepeat || len < hlen || len > mlen)
			goto dont_repeat;
		if (sdt->option.popt & SS7_POPT_XSN) {
			li = ((dp->b_rptr[5] << 8) | dp->b_rptr[4]) & 0x1ff;
			sio = 6;
		} else {
			li = dp->b_rptr[2] & 0x3f;
			sio = 3;
		}
		if (li != mlen - hlen)
			goto dont_repeat;
		if (len > hlen) {
			switch (dp->b_rptr[sio] & 0x7) {
			case 0x0:	/* SIO */
			case 0x1:	/* SIN */
			case 0x2:	/* SIE */
			case 0x3:	/* SIOS */
			case 0x4:	/* SIPO */
				break;
			case 0x5:	/* SIB */
			default:
				goto dont_repeat;
			}
		}
		if (!(cp = copymsg(dp))) {
			freemsg(mp);
			goto enobufs;
		}
		/* 
		   always correct length indicator */
		if (sdt->option.popt && SS7_POPT_XSN) {
			cp->b_rptr[4] &= 0x00;
			cp->b_rptr[5] &= 0xfe;
			cp->b_rptr[4] += (len - hlen);
		} else {
			cp->b_rptr[2] &= 0xc0;
			cp->b_rptr[2] += (len - hlen);
		}
	      dont_repeat:
		if (sdt->sdt.tx_cmp) {
			freemsg(xchg(&sdt->sdt.tx_cmp, NULL));
			sdt->sdt.tx_repeat = 0;
		}
		sdt->sdt.tx_cmp = cp;
		linkb(mp, dp);
		printd(("%s: %p: T_UNITDATA_REQ [%d] ->\n", MOD_NAME, sdt, len));
		putnext(sdt->iq, mp);
		sdt->sdt.stats.tx_bytes += len;
		sdt->sdt.stats.tx_sus++;
		switch (sdt->sdl.config.ifclock) {
		case SDL_CLOCK_TICK:
		case SDL_CLOCK_SHAPER:
			sdt->sdl.bytecount += len;
			ensure(sdt->sdl.tickbytes > 0, sdt->sdl.tickbytes = 1);
			while (sdt->sdl.bytecount >= sdt->sdl.tickbytes) {
				sdt->sdl.bytecount -= sdt->sdl.tickbytes;
				sdt->sdl.timestamp++;
			}
		}
		return (QR_ABSORBED);
	}
      enobufs:
	rare();
	return (-ENOBUFS);
      eagain:
	rare();
	return (-EAGAIN);
      ebusy:
	rare();
	return (-EBUSY);
      discard:
	rare();
	return (QR_DONE);
}

/*
 *  SDT_DAEDT_TRANSMISSION_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_daedt_transmission_req(queue_t *q, mblk_t *mp)
{
	int err;
	if ((err = sdt_send_data(q, mp->b_cont, 0)) == QR_ABSORBED)
		return (QR_TRIMMED);
	return (err);
}

/*
 *  SDT_DAEDT_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_daedt_start_req(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	if (sdt->i_state == LMI_ENABLED)
		sdt_daedt_start(q, sdt);
	return (QR_DONE);
}

/*
 *  SDT_DAEDR_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_daedr_start_req(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	if (sdt->i_state == LMI_ENABLED)
		sdt_daedr_start(q, sdt);
	return (QR_DONE);
}

/*
 *  SDT_AERM_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_start_req(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	if (sdt->i_state == LMI_ENABLED)
		sdt_aerm_start(q, sdt);
	return (QR_DONE);
}

/*
 *  SDT_AERM_STOP_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_stop_req(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	if (sdt->i_state == LMI_ENABLED)
		sdt_aerm_stop(q, sdt);
	return (QR_DONE);
}

/*
 *  SDT_AERM_SET_TI_TO_TIN_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_set_ti_to_tin_req(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	if (sdt->i_state == LMI_ENABLED)
		sdt_aerm_set_ti_to_tin(q, sdt);
	return (QR_DONE);
}

/*
 *  SDT_AERM_SET_TI_TO_TIE_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_aerm_set_ti_to_tie_req(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	if (sdt->i_state == LMI_ENABLED)
		sdt_aerm_set_ti_to_tie(q, sdt);
	return (QR_DONE);
}

/*
 *  SDT_SUERM_START_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_suerm_start_req(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	if (sdt->i_state == LMI_ENABLED)
		sdt_suerm_start(q, sdt);
	return (QR_DONE);
}

/*
 *  SDT_SUERM_STOP_REQ:
 *  -----------------------------------
 */
STATIC int
sdt_suerm_stop_req(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	if (sdt->i_state == LMI_ENABLED)
		sdt_suerm_stop(q, sdt);
	return (QR_DONE);
}

/*
 *  ------------------------------------------------------------------------
 *
 *  TPI-Provider -> TPI-User (SS7IP) Primitives
 *
 *  ------------------------------------------------------------------------
 */
STATIC int
sdt_recv_data(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	return sdt_daedr_received_bits(q, sdt, mp);
}

/*
 *  T_CONN_IND
 *  ------------------------------------------------------------------------
 */
STATIC int
t_conn_ind(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	struct T_conn_ind *p = (typeof(p)) mp->b_rptr;
	assure(sdt->t.state == TS_IDLE);
	sdt->t.state = TS_WRES_CIND;
	if ((sdt->i_state != LMI_UNUSABLE) && (sdt->i_style == LMI_STYLE2)
	    && (sdt->i_state == LMI_ENABLE_PENDING)) {
		if (p->SRC_length)
			bcopy((char *) p + p->SRC_offset, &sdt->t.rem, p->SRC_length);
		return t_conn_res(q, sdt, p->SEQ_number);
	}
	/* 
	   refuse connection */
	return t_discon_req(q, sdt, p->SEQ_number);
}

/*
 *  T_CONN_CON
 *  ------------------------------------------------------------------------
 *  For style 2 transports, we have succeeded in enabling the transport.
 */
STATIC int
t_conn_con(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	if (sdt->i_state != LMI_UNUSABLE) {
		assure(sdt->t.state == TS_WCON_CREQ);
		sdt->t.state = TS_DATA_XFER;
		if ((sdt->i_style == LMI_STYLE2) && (sdt->i_state == LMI_ENABLE_PENDING))
			return lmi_enable_con(q, sdt);
	}
	return (QR_DONE);
}

/*
 *  T_DISCON_IND
 *  ------------------------------------------------------------------------
 */
STATIC int
t_discon_ind(queue_t *q, mblk_t *mp)
{
	int err;
	struct sdt *sdt = SDT_PRIV(q);
	struct T_discon_ind *p = (typeof(p)) mp->b_rptr;
	if (sdt->i_state != LMI_UNUSABLE) {
		if (sdt->t.state != TS_DATA_XFER) {
			sdt->t.state = TS_IDLE;
			if (sdt->i_style == LMI_STYLE2) {
				if (sdt->i_state == LMI_DISABLE_PENDING)
					return lmi_disable_con(q, sdt);
				if (sdt->i_state == LMI_ENABLE_PENDING)
					return lmi_error_ack(q, sdt, LMI_ENABLE_REQ, LMI_SYSERR,
							     p->DISCON_reason);
			}
			return (QR_DONE);
		}
		if (sdt->t.state == TS_DATA_XFER) {
			sdt->t.state = TS_IDLE;
			if (sdt->i_state == LMI_ENABLED) {
				if ((err = sdt_lsc_link_failure(q, sdt)))
					return (err);
				ptrace(("%s: Link Failed: SUERM/EIM\n", MOD_NAME));
			}
			return m_hangup(q, sdt, EPIPE);
		}
	}
	return (QR_DONE);
}

/*
 *  T_DATA_IND
 *  ------------------------------------------------------------------------
 */
STATIC int
t_data_ind_slow(queue_t *q, struct sdt *sdt, mblk_t *mp, int more)
{
	/* 
	 *  Normally we receive data unfragmented and in a single M_DATA
	 *  block.  This slower routine handles the circumstances where we
	 *  receive fragmented data or data that is chained together in
	 *  multiple M_DATA blocks.
	 */
	mblk_t *newp = NULL, *dp = mp->b_cont;
	struct T_data_ind *p = (typeof(p)) mp->b_rptr;
	int err;
	seldom();
	/* 
	 *  We have a chained M_DATA blocks: pull them up.
	 */
	if (dp->b_cont && (err = ss7_pullupmsg(q, dp, -1)) < 0)
		return (err);
	if (more) {
		/* 
		 *  We have a partial delivery.  Chain normal message
		 *  together.  We might have a problem with messages split
		 *  over multiple streams?  Treat normal and expedited
		 *  separately.
		 */
		if (sdt->rbuf)
			linkb(sdt->rbuf, dp);
		else
			sdt->rbuf = dp;
	} else {
		int err;
		if (sdt->rbuf) {
			linkb(sdt->rbuf, dp);
			dp = xchg(&sdt->rbuf, NULL);
		}
		if ((err = sdt_recv_data(q, mp)) != QR_ABSORBED) {
			if (newp)
				freemsg(newp);
			return (err);
		}
	}
	return (newp ? QR_DONE : QR_TRIMMED);
}
STATIC int
t_data_ind(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	if (sdt->i_state == LMI_ENABLED && sdt->sdt.statem.daedr_state != SDT_STATE_IDLE) {
		mblk_t *dp = mp->b_cont;
		struct T_data_ind *p = (typeof(p)) mp->b_rptr;
		if (!p->MORE_flag && !dp->b_cont) {
			int err;
			if ((err = sdt_recv_data(q, dp)) != QR_ABSORBED)
				return (err);
			return (QR_TRIMMED);	/* absorbed data */
		} else {
			return t_data_ind_slow(q, sdt, mp, p->MORE_flag);
		}
	}
	/* 
	   ignore data in other states */
	return (QR_DONE);
}

/*
 *  T_EXDATA_IND
 *  ------------------------------------------------------------------------
 */
STATIC int
t_exdata_ind_slow(queue_t *q, struct sdt *sdt, mblk_t *mp, int more)
{
	/* 
	 *  Normally we receive data unfragmented and in a single M_DATA
	 *  block.  This slower routine handles the circumstances where we
	 *  receive fragmented data or data that is chained together in
	 *  multiple M_DATA blocks.
	 */
	mblk_t *newp = NULL, *dp = mp->b_cont;
	struct T_exdata_ind *p = (typeof(p)) mp->b_rptr;
	int err;
	seldom();
	/* 
	   We have a chaned M_DATA blocks: pull them up. */
	if (dp->b_cont && (err = ss7_pullupmsg(q, dp, -1)) < 0)
		return (err);
	if (more) {
		/* 
		 *  We have a partial delivery.  Chain normal message
		 *  together.  We might have a problem with messages split
		 *  over multiple streams?  Treat normal and expedited
		 *  separately.
		 */
		if (sdt->xbuf)
			linkb(sdt->xbuf, dp);
		else
			sdt->xbuf = dp;
	} else {
		int err;
		if (sdt->xbuf) {
			linkb(sdt->xbuf, dp);
			dp = xchg(&sdt->xbuf, NULL);
		}
		if ((err = sdt_recv_data(q, dp)) != QR_ABSORBED) {
			if (newp)
				freemsg(newp);
			return (err);
		}
	}
	return (newp ? QR_DONE : QR_TRIMMED);
}
STATIC int
t_exdata_ind(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	if (sdt->i_state == LMI_ENABLED && sdt->sdt.statem.daedr_state != SDT_STATE_IDLE) {
		mblk_t *dp = mp->b_cont;
		struct T_exdata_ind *p = (typeof(p)) mp->b_rptr;
		if (!p->MORE_flag && !dp->b_cont) {
			int err;
			if ((err = sdt_recv_data(q, dp)) != QR_ABSORBED)
				return (err);
			return (QR_TRIMMED);	/* absorbed data */
		} else
			return t_exdata_ind_slow(q, sdt, mp, p->MORE_flag);
	}
	/* 
	   ignore data in other states */
	return (QR_DONE);
}

/*
 *  T_INFO_ACK
 *  ------------------------------------------------------------------------
 *  This is a response to our request for information about the transport
 *  provider.  We must first get this response before doing anything with the
 *  transport.  The transport state is set to TS_NOSTATES until we know the
 *  state of the transport.
 */
STATIC int
t_info_ack(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	struct T_info_ack *p = ((typeof(p)) mp->b_rptr);
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		sdt->t.pdu_size = p->TSDU_size;
		if (p->TIDU_size && p->TIDU_size < p->TSDU_size)
			sdt->t.pdu_size = p->TIDU_size;
		if ((sdt->sdt.config.m =
		     sdt->t.pdu_size - 1 - ((sdt->option.popt & SS7_POPT_XSN) ? 6 : 3)) < 272)
			cmn_err(CE_WARN, "%s: transport provider TDU_size is too small %ld",
				MOD_NAME, sdt->sdt.config.m);
		if ((sdt->t.add_size = p->ADDR_size) > sizeof(struct sockaddr))
			cmn_err(CE_WARN, "%s: transport provider ADDR_size is too large %ld",
				MOD_NAME, (unsigned long) p->ADDR_size);
		sdt->t.opt_size = p->OPT_size;
		sdt->t.state = p->CURRENT_state;
		sdt->t.serv_type = p->SERV_type;
		switch (sdt->t.serv_type) {
		case T_COTS:
		case T_COTS_ORD:
			if (sdt->t.state == TS_DATA_XFER) {
				/* 
				   no attachment required */
				sdt->i_state = LMI_DISABLED;
				sdt->i_style = LMI_STYLE1;
				return (QR_DONE);
			}
			if (sdt->t.state == TS_UNBND) {
				sdt->i_state = LMI_UNATTACHED;
				sdt->i_style = LMI_STYLE2;
				return (QR_DONE);
			}
			break;
		case T_CLTS:
			if (sdt->t.state == TS_IDLE) {
				/* 
				   no attachment required */
				sdt->i_state = LMI_DISABLED;
				sdt->i_style = LMI_STYLE1;
				return (QR_DONE);
			}
			if (sdt->t.state == TS_UNBND) {
				sdt->i_state = LMI_UNATTACHED;
				sdt->i_style = LMI_STYLE2;
				return (QR_DONE);
			}
			break;
		}
		swerr();
		return (-EFAULT);
	}
	return (-EMSGSIZE);
}

/*
 *  T_BIND_ACK
 *  ------------------------------------------------------------------------
 *  We should only get bind acks when we are attaching for a Style 2 transport.  Otherwise, we
 *  should just echo the state change and otherwise ignore the ack.
 */
STATIC int
t_bind_ack(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	if (sdt->i_state != LMI_UNUSABLE) {
		assure(sdt->t.state == TS_WACK_BREQ);
		sdt->t.state = TS_IDLE;
		if ((sdt->i_style == LMI_STYLE2) && (sdt->i_state == LMI_ATTACH_PENDING))
			return lmi_ok_ack(q, sdt, LMI_ATTACH_REQ);
	}
	return (QR_DONE);
}

/*
 *  T_ERROR_ACK
 *  ------------------------------------------------------------------------
 */
STATIC int
t_error_ack(queue_t *q, mblk_t *mp)
{
	struct T_error_ack *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		struct sdt *sdt = SDT_PRIV(q);
		switch (sdt->t.state) {
		case TS_WACK_OPTREQ:
			assure(p->ERROR_prim == T_OPTMGMT_REQ);
			swerr();
			return (-EFAULT);
		case TS_WACK_BREQ:
			assure(p->ERROR_prim == T_BIND_REQ);
			if (sdt->i_state != LMI_UNUSABLE) {
				sdt->t.state = TS_UNBND;
				if ((sdt->i_style == LMI_STYLE2)
				    && (sdt->i_state == LMI_ATTACH_PENDING)) {
					ptrace(("%s: ERROR: got T_ERROR_ACK from TPI\n", MOD_NAME));
					return lmi_error_ack(q, sdt, LMI_ATTACH_REQ, LMI_BADPPA,
							     p->UNIX_error);
				}
			}
			return (QR_DONE);
		case TS_WACK_UREQ:
			assure(p->ERROR_prim == T_UNBIND_REQ);
			if (sdt->i_state != LMI_UNUSABLE) {
				sdt->t.state = TS_IDLE;
				if ((sdt->i_style == LMI_STYLE2)
				    && (sdt->i_state == LMI_DETACH_PENDING)) {
					ptrace(("%s: ERROR: got T_ERROR_ACK from TPI\n", MOD_NAME));
					return lmi_error_ack(q, sdt, LMI_ATTACH_REQ, LMI_SYSERR,
							     p->UNIX_error);
				}
			}
			return (QR_DONE);
		case TS_WACK_CREQ:
			assure(p->ERROR_prim == T_CONN_REQ);
			if (sdt->i_state != LMI_UNUSABLE) {
				sdt->t.state = TS_IDLE;
				if ((sdt->i_style == LMI_STYLE2)
				    && (sdt->i_state == LMI_ENABLE_PENDING)) {
					ptrace(("%s: ERROR: got T_ERROR_ACK from TPI\n", MOD_NAME));
					return lmi_error_ack(q, sdt, LMI_ENABLE_REQ, LMI_SYSERR,
							     p->UNIX_error);
				}
			}
			return (QR_DONE);
		case TS_WACK_CRES:
			assure(p->ERROR_prim == T_CONN_RES);
			if (sdt->i_state != LMI_UNUSABLE) {
				sdt->t.state = TS_WRES_CIND;
				if ((sdt->i_style == LMI_STYLE2)
				    && (sdt->i_state == LMI_ENABLE_PENDING)) {
					ptrace(("%s: ERROR: got T_ERROR_ACK from TPI\n", MOD_NAME));
					return lmi_error_ack(q, sdt, LMI_ENABLE_REQ, LMI_SYSERR,
							     p->UNIX_error);
				}
			}
			/* 
			   try refusing the connection */
			return t_discon_req(q, sdt, sdt->t.sequence);
		case TS_WACK_DREQ6:
		case TS_WACK_DREQ7:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			assure(p->ERROR_prim == T_DISCON_REQ);
			return (QR_DONE);
		case TS_WACK_DREQ9:
			assure(p->ERROR_prim == T_DISCON_REQ);
			return (-EFAULT);
		case TS_WIND_ORDREL:
			assure(p->ERROR_prim == T_ORDREL_REQ);
			sdt->t.state = TS_DATA_XFER;
			if (sdt->i_state == LMI_ENABLE_PENDING
			    || sdt->i_state == LMI_DISABLE_PENDING)
				return t_discon_req(q, sdt, 0);
			return (QR_DONE);
		default:
		case TS_UNBND:
		case TS_IDLE:
		case TS_WCON_CREQ:
		case TS_WRES_CIND:
		case TS_DATA_XFER:
		case TS_WREQ_ORDREL:
		case TS_NOSTATES:
			swerr();
			return (-EFAULT);
		}
	}
	swerr();
	return (-EMSGSIZE);
}

/*
 *  T_OK_ACK
 *  ------------------------------------------------------------------------
 */
STATIC int
t_ok_ack(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	struct T_ok_ack *p = (typeof(p)) mp->b_rptr;
	if ((mp->b_wptr - mp->b_rptr) >= sizeof(*p)) {
		switch (sdt->t.state) {
		case TS_WACK_UREQ:
			assure(p->CORRECT_prim == T_UNBIND_REQ);
			if (sdt->i_state != LMI_UNUSABLE) {
				sdt->t.state = TS_UNBND;
				if ((sdt->i_style == LMI_STYLE2)
				    && (sdt->i_state == LMI_DETACH_PENDING))
					return lmi_ok_ack(q, sdt, LMI_DETACH_REQ);
			}
			return (QR_DONE);
		case TS_WACK_CREQ:
			assure(p->CORRECT_prim == T_CONN_REQ);
			if (sdt->i_state != LMI_UNUSABLE)
				sdt->t.state = TS_WCON_CREQ;
			/* 
			   wait for T_CONN_CON */
			return (QR_DONE);
		case TS_WACK_CRES:
			assure(p->CORRECT_prim == T_CONN_RES);
			if (sdt->i_state != LMI_UNUSABLE) {
				sdt->t.state = TS_DATA_XFER;
				if ((sdt->i_style = LMI_STYLE2)
				    && (sdt->i_state == LMI_ENABLE_PENDING))
					return lmi_enable_con(q, sdt);
			}
			return (QR_DONE);
		case TS_WACK_DREQ7:
		case TS_WACK_DREQ6:
		case TS_WACK_DREQ10:
		case TS_WACK_DREQ11:
			assure(p->CORRECT_prim == T_DISCON_REQ);
			return (QR_DONE);
		case TS_WACK_DREQ9:
			assure(p->CORRECT_prim == T_DISCON_REQ);
			if (sdt->i_state != LMI_UNUSABLE) {
				sdt->t.state = TS_IDLE;
				if ((sdt->i_style == LMI_STYLE2)
				    && (sdt->i_state == LMI_DISABLE_PENDING))
					return lmi_disable_con(q, sdt);
			}
			return (QR_DONE);
		default:
		case TS_WACK_OPTREQ:
		case TS_WACK_BREQ:
		case TS_UNBND:
		case TS_IDLE:
		case TS_WCON_CREQ:
		case TS_WRES_CIND:
		case TS_DATA_XFER:
		case TS_WREQ_ORDREL:
		case TS_WIND_ORDREL:
		case TS_NOSTATES:
			swerr();
			return (-EFAULT);
		}
	}
	swerr();
	return (-EMSGSIZE);
}

/*
 *  T_UNITDATA_IND
 *  -------------------------------------------------------------------------
 */
STATIC int
t_unitdata_ind(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	if (sdt->i_state == LMI_ENABLED && sdt->sdt.statem.daedr_state != SDT_STATE_IDLE) {
		mblk_t *dp = mp->b_cont;
#if 0
		struct T_unitdata_ind *p = (typeof(p)) mp->b_rptr;
		/* 
		   check source of packet */
		if (p->SRC_length
		    && !memcmp(mp->b_rptr + p->SRC_offset, &sdt->t.rem, p->SRC_length)) {
#endif
			int err;
			if ((err = sdt_recv_data(q, dp)) == QR_ABSORBED)
				return (QR_TRIMMED);
			return (err);
#if 0
		}
#endif
		/* 
		   ignore packets not from remote address */
	}
	/* 
	   ignore data in other states */
	return (QR_DONE);
}

/*
 *  T_UDERROR_IND
 *  -------------------------------------------------------------------------
 */
STATIC int
t_uderror_ind(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	return sdt_daedr_su_in_error(q, sdt);
}

/*
 *  T_OPTMGMT_ACK
 *  ------------------------------------------------------------------------
 */
STATIC int
t_optmgmt_ack(queue_t *q, mblk_t *mp)
{
	/* 
	   ignore */
	return (QR_DONE);
}

/*
 *  T_ORDREL_IND
 *  -------------------------------------------------------------------------
 */
STATIC int
t_ordrel_ind(queue_t *q, mblk_t *mp)
{
	int err;
	struct sdt *sdt = SDT_PRIV(q);
	if (sdt->i_state != LMI_UNUSABLE) {
		if (sdt->t.state == TS_WIND_ORDREL) {
			sdt->t.state = TS_IDLE;
			if ((sdt->i_style == LMI_STYLE2) && (sdt->i_state == LMI_DISABLE_PENDING))
				return lmi_disable_con(q, sdt);
			return (QR_DONE);
		}
		if (sdt->t.state == TS_DATA_XFER) {
			sdt->t.state = TS_WREQ_ORDREL;
			if (sdt->i_state == LMI_ENABLED) {
				if ((err = sdt_lsc_link_failure(q, sdt)))
					return (err);
				ptrace(("%s: Link Failed: SUERM/EIM\n", MOD_NAME));
			}
			if ((err = t_ordrel_req(q, sdt)))
				return (err);
			return m_hangup(q, sdt, EPIPE);
		}
	}
	return (QR_DONE);
}

/*
 *  T_OPTDATA_IND
 *  -------------------------------------------------------------------------
 */
STATIC int
t_optdata_ind(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	if (sdt->i_state == LMI_ENABLED && sdt->sdt.statem.daedr_state != SDT_STATE_IDLE) {
		mblk_t *dp = mp->b_cont;
		struct T_optdata_ind *p = (typeof(p)) mp->b_rptr;
		if (!(p->DATA_flag & T_ODF_MORE) && !dp->b_cont) {
			int err;
			if ((err = sdt_recv_data(q, dp)) == QR_ABSORBED)
				return (QR_TRIMMED);	/* absorbed data */
			return (err);
		} else {
			if (p->DATA_flag & T_ODF_EX)
				return t_exdata_ind_slow(q, sdt, mp, (p->DATA_flag & T_ODF_MORE));
			else
				return t_data_ind_slow(q, sdt, mp, (p->DATA_flag & T_ODF_MORE));
		}
	}
	/* 
	   ignore data in other states */
	return (QR_DONE);
}

/*
 *  T_ADDR_ACK
 *  ------------------------------------------------------------------------
 */
STATIC int
t_addr_ack(queue_t *q, mblk_t *mp)
{
	/* 
	   ignore */
	return (QR_DONE);
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
lmi_test_config(struct sdt *sdt, lmi_config_t * arg)
{
	return (-EOPNOTSUPP);
}
STATIC int
lmi_commit_config(struct sdt *sdt, lmi_config_t * arg)
{
	return (-EOPNOTSUPP);
}
STATIC int
lmi_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		int ret = 0;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			*arg = sdt->option;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		int ret = 0;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			sdt->option = *arg;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			arg->version = sdt->i_version;
			arg->style = sdt->i_style;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			sdt->i_version = arg->version;
			sdt->i_style = arg->style;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return lmi_test_config(sdt, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		lmi_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return lmi_commit_config(sdt, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			arg->state = sdt->i_state;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccmreset(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		lmi_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret = 0;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			sdt->i_state = LMI_UNUSABLE;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret = 0;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			*arg = sdt->sdt.statsp;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret = 0;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			sdt->sdt.statsp = *arg;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		lmi_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccstats(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	psw_t flags;
	int ret;
	(void) mp;
	spin_lock_irqsave(&sdt->lock, flags);
	{
		ret = -EOPNOTSUPP;
	}
	spin_unlock_irqrestore(&sdt->lock, flags);
	return (ret);
}
STATIC int
lmi_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
lmi_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		lmi_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			ret = -EOPNOTSUPP;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDT IO Controls
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdt_test_config(struct sdt *sdt, sdt_config_t * arg)
{
	int ret = 0;
	psw_t flags;
	spin_lock_irqsave(&sdt->lock, flags);
	do {
		if (!arg->t8)
			arg->t8 = sdt->sdt.config.t8;
		if (!arg->Tin)
			arg->Tin = sdt->sdt.config.Tin;
		if (!arg->Tie)
			arg->Tie = sdt->sdt.config.Tie;
		if (!arg->T)
			arg->T = sdt->sdt.config.T;
		if (!arg->D)
			arg->D = sdt->sdt.config.D;
		if (!arg->Te)
			arg->Te = sdt->sdt.config.Te;
		if (!arg->De)
			arg->De = sdt->sdt.config.De;
		if (!arg->Ue)
			arg->Ue = sdt->sdt.config.Ue;
		if (!arg->N)
			arg->N = sdt->sdt.config.N;
		if (!arg->m)
			arg->m = sdt->sdt.config.m;
		if (!arg->b)
			arg->b = sdt->sdt.config.b;
		else if (arg->b != sdt->sdt.config.b) {
			ret = -EINVAL;
			break;
		}
	} while (0);
	spin_unlock_irqrestore(&sdt->lock, flags);
	return (ret);
}
STATIC int
sdt_commit_config(struct sdt *sdt, sdt_config_t * arg)
{
	psw_t flags;
	spin_lock_irqsave(&sdt->lock, flags);
	{
		sdt_test_config(sdt, arg);
		sdt->sdt.config = *arg;
	}
	spin_unlock_irqrestore(&sdt->lock, flags);
	return (0);
}
STATIC int
sdt_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			*arg = sdt->option;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			sdt->option = *arg;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			*arg = sdt->sdt.config;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			sdt->sdt.config = *arg;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return sdt_test_config(sdt, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		sdt_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		return sdt_commit_config(sdt, arg);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		sdt_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			*arg = sdt->sdt.statem;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccmreset(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	(void) sdt;
	(void) mp;
	fixme(("%s: Master reset\n", MOD_NAME));
	return (-EOPNOTSUPP);
}
STATIC int
sdt_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			*arg = sdt->sdt.statsp;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			sdt->sdt.statsp = *arg;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		sdt_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			*arg = sdt->sdt.stats;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccstats(queue_t *q, mblk_t *mp)
{
	psw_t flags;
	struct sdt *sdt = SDT_PRIV(q);
	(void) mp;
	spin_lock_irqsave(&sdt->lock, flags);
	{
		bzero(&sdt->sdt.stats, sizeof(sdt->sdt.stats));
	}
	spin_unlock_irqrestore(&sdt->lock, flags);
	return (0);
}
STATIC int
sdt_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			*arg = sdt->sdt.notify;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			sdt->sdt.notify = *arg;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		sdt_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			sdt->sdt.notify.events &= ~arg->events;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdt_ioccabort(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	psw_t flags;
	int ret = 0;
	(void) mp;
	spin_lock_irqsave(&sdt->lock, flags);
	{
		ret = -EOPNOTSUPP;
	}
	spin_unlock_irqrestore(&sdt->lock, flags);
	return (ret);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SDL IO Controls
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdl_test_config(struct sdt *sdt, sdl_config_t * arg)
{
	(void) sdt;
	(void) arg;
	// fixme(("%s: FIXME: write this function\n", MOD_NAME));
	return (0);
}
STATIC void
sdl_commit_config(struct sdt *sdt, sdl_config_t * arg)
{
	long tdiff;
	sdt->sdl.config = *arg;
	switch (sdt->sdl.config.ifclock) {
	case SDL_CLOCK_TICK:
	case SDL_CLOCK_SHAPER:
		/* 
		   reshape traffic */
		if ((tdiff = sdt->sdl.timestamp - jiffies) > 0)
			sdt->sdl.bytecount += sdt->sdl.tickbytes * tdiff;
		else
			sdt->sdl.bytecount = 0;
		sdt->sdl.timestamp = jiffies;
		sdt->sdl.tickbytes = sdt->sdl.config.ifrate / HZ / 8;
		if (sdt->sdl.tickbytes < 1)
			sdt->sdl.tickbytes = 1;
		ensure(sdt->sdl.tickbytes > 0, sdt->sdl.tickbytes = 1);
		while (sdt->sdl.bytecount >= sdt->sdl.tickbytes) {
			sdt->sdl.bytecount -= sdt->sdl.tickbytes;
			sdt->sdl.timestamp++;
		}
	}
}
STATIC int
sdl_iocgoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			*arg = sdt->option;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsoptions(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_option_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			sdt->option = *arg;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			*arg = sdt->sdl.config;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret = 0;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			if (!(ret = sdl_test_config(sdt, arg)))
				sdl_commit_config(sdt, arg);
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioctconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		int ret = 0;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			ret = sdl_test_config(sdt, arg);
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (ret);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccconfig(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		sdl_config_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		psw_t flags;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			sdl_commit_config(sdt, arg);
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgstatem(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		sdl_statem_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			*arg = sdt->sdl.statem;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccmreset(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	(void) sdt;
	(void) arg;
	fixme(("%s: FIXME: Support master reset\n", MOD_NAME));
	return (-EOPNOTSUPP);
}
STATIC int
sdl_iocgstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			*arg = sdt->sdl.statsp;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsstatsp(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		lmi_sta_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		fixme(("%s: FIXME: check these settings\n", MOD_NAME));
		spin_lock_irqsave(&sdt->lock, flags);
		{
			sdt->sdl.statsp = *arg;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocgstats(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		sdl_stats_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			*arg = sdt->sdl.stats;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccstats(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	psw_t flags;
	(void) mp;
	spin_lock_irqsave(&sdt->lock, flags);
	{
		bzero(&sdt->sdl.stats, sizeof(sdt->sdl.stats));
	}
	spin_unlock_irqrestore(&sdt->lock, flags);
	return (0);
}
STATIC int
sdl_iocgnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			*arg = sdt->sdl.notify;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_iocsnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			sdt->sdl.notify.events |= arg->events;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccnotify(queue_t *q, mblk_t *mp)
{
	if (mp->b_cont) {
		struct sdt *sdt = SDT_PRIV(q);
		psw_t flags;
		sdl_notify_t *arg = (typeof(arg)) mp->b_cont->b_rptr;
		spin_lock_irqsave(&sdt->lock, flags);
		{
			sdt->sdl.notify.events &= ~arg->events;
		}
		spin_unlock_irqrestore(&sdt->lock, flags);
		return (0);
	}
	rare();
	return (-EINVAL);
}
STATIC int
sdl_ioccdisctx(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	psw_t flags;
	(void) mp;
	spin_lock_irqsave(&sdt->lock, flags);
	{
		sdt->sdl.config.ifflags &= ~SDL_IF_TX_RUNNING;
	}
	spin_unlock_irqrestore(&sdt->lock, flags);
	return (0);
}
STATIC int
sdl_ioccconntx(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	psw_t flags;
	(void) mp;
	spin_lock_irqsave(&sdt->lock, flags);
	{
		sdt->sdl.config.ifflags |= SDL_IF_TX_RUNNING;
	}
	spin_unlock_irqrestore(&sdt->lock, flags);
	return (0);
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
sdt_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
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
			ptrace(("%s: ERROR: Unsupported STREAMS ioctl %d\n", MOD_NAME, nr));
			ret = -EINVAL;
			break;
		default:
			ptrace(("%s: ERROR: Unsupported STREAMS ioctl %d\n", MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case LMI_IOC_MAGIC:
	{
		if (count < size || SDT_PRIV(q)->state == LMI_UNATTACHED) {
			ret = -EINVAL;
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
			ptrace(("%s: ERROR: Unsupported SDT ioctl %d\n", MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SDT_IOC_MAGIC:
	{
		if (count < size || SDT_PRIV(q)->state == LMI_UNATTACHED) {
			ret = -EINVAL;
			break;
		}
		switch (nr) {
		case _IOC_NR(SDT_IOCGOPTIONS):	/* lmi_option_t */
			ret = sdt_iocgoptions(q, mp);
			break;
		case _IOC_NR(SDT_IOCSOPTIONS):	/* lmi_option_t */
			ret = sdt_iocsoptions(q, mp);
			break;
		case _IOC_NR(SDT_IOCGCONFIG):	/* sdt_config_t */
			ret = sdt_iocgconfig(q, mp);
			break;
		case _IOC_NR(SDT_IOCSCONFIG):	/* sdt_config_t */
			ret = sdt_iocsconfig(q, mp);
			break;
		case _IOC_NR(SDT_IOCTCONFIG):	/* sdt_config_t */
			ret = sdt_ioctconfig(q, mp);
			break;
		case _IOC_NR(SDT_IOCCCONFIG):	/* sdt_config_t */
			ret = sdt_ioccconfig(q, mp);
			break;
		case _IOC_NR(SDT_IOCGSTATEM):	/* sdt_statem_t */
			ret = sdt_iocgstatem(q, mp);
			break;
		case _IOC_NR(SDT_IOCCMRESET):	/* sdt_statem_t */
			ret = sdt_ioccmreset(q, mp);
			break;
		case _IOC_NR(SDT_IOCGSTATSP):	/* lmi_sta_t */
			ret = sdt_iocgstatsp(q, mp);
			break;
		case _IOC_NR(SDT_IOCSSTATSP):	/* lmi_sta_t */
			ret = sdt_iocsstatsp(q, mp);
			break;
		case _IOC_NR(SDT_IOCGSTATS):	/* sdt_stats_t */
			ret = sdt_iocgstats(q, mp);
			break;
		case _IOC_NR(SDT_IOCCSTATS):	/* sdt_stats_t */
			ret = sdt_ioccstats(q, mp);
			break;
		case _IOC_NR(SDT_IOCGNOTIFY):	/* sdt_notify_t */
			ret = sdt_iocgnotify(q, mp);
			break;
		case _IOC_NR(SDT_IOCSNOTIFY):	/* sdt_notify_t */
			ret = sdt_iocsnotify(q, mp);
			break;
		case _IOC_NR(SDT_IOCCNOTIFY):	/* sdt_notify_t */
			ret = sdt_ioccnotify(q, mp);
			break;
		case _IOC_NR(SDT_IOCCABORT):	/* */
			ret = sdt_ioccabort(q, mp);
			break;
		default:
			ptrace(("%s: ERROR: Unsupported SDT ioctl %d\n", MOD_NAME, nr));
			ret = -EOPNOTSUPP;
			break;
		}
		break;
	}
	case SDL_IOC_MAGIC:
	{
		if (count < size || sdt->i_state == LMI_UNATTACHED) {
			ptrace(("%s: ERROR: ioctl count = %d, size = %d, state = %lu\n",
				MOD_NAME, count, size, sdt->i_state));
			ret = -EINVAL;
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
		case _IOC_NR(SDL_IOCGSTATSP):	/* sdl_stats_t */
			ret = sdl_iocgstatsp(q, mp);
			break;
		case _IOC_NR(SDL_IOCSSTATSP):	/* sdl_stats_t */
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
		ret = (QR_PASSALONG);
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
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
sdt_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct sdt *sdt = SDT_PRIV(q);
	ulong oldstate = sdt->i_state;
	/* 
	   Fast Path */
	if ((prim = *(ulong *) mp->b_rptr) == SDT_DAEDT_TRANSMISSION_REQ) {
		printd(("%s: %p: -> SDT_DAEDT_TRANSMISSION_REQ\n", MOD_NAME, sdt));
		if ((rtn = sdt_daedt_transmission_req(q, mp)) < 0)
			sdt->i_state = oldstate;
		return (rtn);
	}
	switch (prim) {
	case LMI_INFO_REQ:
		printd(("%s: %p: -> LMI_INFO_REQ\n", MOD_NAME, sdt));
		rtn = lmi_info_req(q, mp);
		break;
	case LMI_ATTACH_REQ:
		printd(("%s: %p: -> LMI_ATTACH_REQ\n", MOD_NAME, sdt));
		rtn = lmi_attach_req(q, mp);
		break;
	case LMI_DETACH_REQ:
		printd(("%s: %p: -> LMI_DETACH_REQ\n", MOD_NAME, sdt));
		rtn = lmi_detach_req(q, mp);
		break;
	case LMI_ENABLE_REQ:
		printd(("%s: %p: -> LMI_ENABLE_REQ\n", MOD_NAME, sdt));
		rtn = lmi_enable_req(q, mp);
		break;
	case LMI_DISABLE_REQ:
		printd(("%s: %p: -> LMI_DISABLE_REQ\n", MOD_NAME, sdt));
		rtn = lmi_disable_req(q, mp);
		break;
	case LMI_OPTMGMT_REQ:
		printd(("%s: %p: -> LMI_OPTMGMT_REQ\n", MOD_NAME, sdt));
		rtn = lmi_optmgmt_req(q, mp);
		break;
	case SDT_DAEDT_TRANSMISSION_REQ:
		printd(("%s: %p: -> SDT_DAEDT_TRANSMISSION_REQ\n", MOD_NAME, sdt));
		rtn = sdt_daedt_transmission_req(q, mp);
		break;
	case SDT_DAEDT_START_REQ:
		printd(("%s: %p: -> SDT_DAEDT_START_REQ\n", MOD_NAME, sdt));
		rtn = sdt_daedt_start_req(q, mp);
		break;
	case SDT_DAEDR_START_REQ:
		printd(("%s: %p: -> SDT_DAEDR_START_REQ\n", MOD_NAME, sdt));
		rtn = sdt_daedr_start_req(q, mp);
		break;
	case SDT_AERM_START_REQ:
		printd(("%s: %p: -> SDT_AERM_START_REQ\n", MOD_NAME, sdt));
		rtn = sdt_aerm_start_req(q, mp);
		break;
	case SDT_AERM_STOP_REQ:
		printd(("%s: %p: -> SDT_AERM_STOP_REQ\n", MOD_NAME, sdt));
		rtn = sdt_aerm_stop_req(q, mp);
		break;
	case SDT_AERM_SET_TI_TO_TIN_REQ:
		printd(("%s: %p: -> SDT_AERM_SET_TI_TO_TIN_REQ\n", MOD_NAME, sdt));
		rtn = sdt_aerm_set_ti_to_tin_req(q, mp);
		break;
	case SDT_AERM_SET_TI_TO_TIE_REQ:
		printd(("%s: %p: -> SDT_AERM_SET_TI_TO_TIE_REQ\n", MOD_NAME, sdt));
		rtn = sdt_aerm_set_ti_to_tie_req(q, mp);
		break;
	case SDT_SUERM_START_REQ:
		printd(("%s: %p: -> SDT_SUERM_START_REQ\n", MOD_NAME, sdt));
		rtn = sdt_suerm_start_req(q, mp);
		break;
	case SDT_SUERM_STOP_REQ:
		printd(("%s: %p: -> SDT_SUERM_STOP_REQ\n", MOD_NAME, sdt));
		rtn = sdt_suerm_stop_req(q, mp);
		break;
	default:
		rtn = -EOPNOTSUPP;
		break;
	}
	if (rtn < 0)
		sdt->i_state = oldstate;
	return (rtn);
}

STATIC int
sdt_r_proto(queue_t *q, mblk_t *mp)
{
	int rtn;
	ulong prim;
	struct sdt *sdt = SDT_PRIV(q);
	ulong oldstate = sdt->t.state;
	/* 
	   Fast Path */
	if ((prim = *((ulong *) mp->b_rptr)) == T_UNITDATA_IND) {
		printd(("%s: %p: T_UNITDATA_IND [%d] <-\n", MOD_NAME, sdt, msgdsize(mp->b_cont)));
		if ((rtn = t_unitdata_ind(q, mp)) < 0)
			sdt->t.state = oldstate;
		return (rtn);
	}
	switch (prim) {
	case T_CONN_IND:
		printd(("%s: %p: T_CONN_IND <-\n", MOD_NAME, sdt));
		rtn = t_conn_ind(q, mp);
		break;
	case T_CONN_CON:
		printd(("%s: %p: T_CONN_CON <-\n", MOD_NAME, sdt));
		rtn = t_conn_con(q, mp);
		break;
	case T_DISCON_IND:
		printd(("%s: %p: T_DISCON_IND <-\n", MOD_NAME, sdt));
		rtn = t_discon_ind(q, mp);
		break;
	case T_DATA_IND:
		printd(("%s: %p: T_DATA_IND <-\n", MOD_NAME, sdt));
		rtn = t_data_ind(q, mp);
		break;
	case T_EXDATA_IND:
		printd(("%s: %p: T_EXDATA_IND <-\n", MOD_NAME, sdt));
		rtn = t_exdata_ind(q, mp);
		break;
	case T_INFO_ACK:
		printd(("%s: %p: T_INFO_ACK <-\n", MOD_NAME, sdt));
		rtn = t_info_ack(q, mp);
		break;
	case T_BIND_ACK:
		printd(("%s: %p: T_BIND_ACK <-\n", MOD_NAME, sdt));
		rtn = t_bind_ack(q, mp);
		break;
	case T_ERROR_ACK:
		printd(("%s: %p: T_ERROR_ACK <-\n", MOD_NAME, sdt));
		rtn = t_error_ack(q, mp);
		break;
	case T_OK_ACK:
		printd(("%s: %p: T_OK_ACK <-\n", MOD_NAME, sdt));
		rtn = t_ok_ack(q, mp);
		break;
	case T_UNITDATA_IND:
		printd(("%s: %p: T_UNITDATA_IND [%d] <-\n", MOD_NAME, sdt, msgdsize(mp->b_cont)));
		rtn = t_unitdata_ind(q, mp);
		break;
	case T_UDERROR_IND:
		printd(("%s: %p: T_UDERROR_IND <-\n", MOD_NAME, sdt));
		rtn = t_uderror_ind(q, mp);
		break;
	case T_OPTMGMT_ACK:
		printd(("%s: %p: T_OPTMGMT_ACK <-\n", MOD_NAME, sdt));
		rtn = t_optmgmt_ack(q, mp);
		break;
	case T_ORDREL_IND:
		printd(("%s: %p: T_ORDREL_IND <-\n", MOD_NAME, sdt));
		rtn = t_ordrel_ind(q, mp);
		break;
	case T_OPTDATA_IND:
		printd(("%s: %p: T_OPTDATA_IND <-\n", MOD_NAME, sdt));
		rtn = t_optdata_ind(q, mp);
		break;
	case T_ADDR_ACK:
		printd(("%s: %p: T_ADDR_ACK <-\n", MOD_NAME, sdt));
		rtn = t_addr_ack(q, mp);
		break;
	default:
		swerr();
		rtn = (-EOPNOTSUPP);
		break;
	}
	if (rtn < 0)
		sdt->t.state = oldstate;
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
sdt_w_data(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	(void) sdt;
	printd(("%s: %p: -> M_DATA [%d]\n", MOD_NAME, sdt, msgdsize(mp)));
	return sdt_send_data(q, mp, 0);
}
STATIC int
sdt_r_data(queue_t *q, mblk_t *mp)
{
	struct sdt *sdt = SDT_PRIV(q);
	(void) sdt;
	printd(("%s: %p: M_DATA [%d] <-\n", MOD_NAME, sdt, msgdsize(mp)));
	return sdt_recv_data(q, mp);
}

/*
 *  =========================================================================
 *
 *  PUT and SRV
 *
 *  =========================================================================
 */
STATIC INLINE streamscall int
sdt_w_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return sdt_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sdt_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sdt_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return sdt_w_ioctl(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC INLINE streamscall int
sdt_r_prim(queue_t *q, mblk_t *mp)
{
	/* 
	   Fast Path */
	if (mp->b_datap->db_type == M_DATA)
		return sdt_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return sdt_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return sdt_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}

/*
 *  =========================================================================
 *
 *  OPEN and CLOSE
 *
 *  =========================================================================
 *  Open is called on the first open of a character special device stream
 *  head; close is called on the last close of the same device.
 */
STATIC streamscall int
sdt_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	int err;
	MOD_INC_USE_COUNT;	/* keep module from unloading in our face */
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next != NULL) {
		int cmajor = getmajor(*devp);
		int cminor = getminor(*devp);
		struct sdt *sdt;
		/* 
		   test for multiple push */
		for (sdt = sdt_opens; sdt; sdt = sdt->next) {
			if (sdt->u.dev.cmajor == cmajor && sdt->u.dev.cminor == cminor) {
				MOD_DEC_USE_COUNT;
				return (ENXIO);
			}
		}
		if (!(sdt_alloc_priv(q, &sdt_opens, devp, crp))) {
			MOD_DEC_USE_COUNT;
			return (ENOMEM);
		}
		/* 
		   generate immediate information request */
		if ((err = t_info_req(q, sdt)) < 0) {
			sdt_free_priv(q);
			MOD_DEC_USE_COUNT;
			return (-err);
		}
		return (0);
	}
	MOD_DEC_USE_COUNT;
	return (EIO);
}
STATIC streamscall int
sdt_close(queue_t *q, int flag, cred_t *crp)
{
	(void) flag;
	(void) crp;
	sdt_free_priv(q);
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
STATIC kmem_cache_t *sdt_priv_cachep = NULL;
STATIC int
sdt_init_caches(void)
{
	if (!sdt_priv_cachep
	    && !(sdt_priv_cachep =
		 kmem_cache_create("sdt_priv_cachep", sizeof(struct sdt), 0, SLAB_HWCACHE_ALIGN,
				   NULL, NULL))) {
		cmn_err(CE_PANIC, "%s: Cannot allocate sdt_priv_cachep", __FUNCTION__);
		return (-ENOMEM);
	} else
		printd(("%s: initialized module private structure cace\n", MOD_NAME));
	return (0);
}
STATIC int
sdt_term_caches(void)
{
	/* 
	   don't shrink */
	if (sdt_priv_cachep) {
		if (kmem_cache_destroy(sdt_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy sdt_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		} else
			printd(("%s: destroyed sdt_priv_cachep\n", MOD_NAME));
	}
	return (0);
}
STATIC struct sdt *
sdt_alloc_priv(queue_t *q, struct sdt **sdtp, dev_t *devp, cred_t *crp)
{
	struct sdt *sdt;
	if ((sdt = kmem_cache_alloc(sdt_priv_cachep, SLAB_ATOMIC))) {
		printd(("%s: allocated module private structure\n", MOD_NAME));
		bzero(sdt, sizeof(*sdt));
		sdt_get(sdt);	/* first get */
		sdt->u.dev.cmajor = getmajor(*devp);
		sdt->u.dev.cminor = getminor(*devp);
		(sdt->oq = RD(q))->q_ptr = sdt_get(sdt);
		(sdt->iq = WR(q))->q_ptr = sdt_get(sdt);
		spin_lock_init(&sdt->qlock);	/* "sdt-queue-lock" */
		sdt->i_state = LMI_UNUSABLE;
		sdt->i_style = LMI_STYLE1;
		sdt->i_version = 1;
		sdt->o_prim = sdt_r_prim;
		sdt->o_wakeup = sdt_r_wakeup;
		sdt->i_prim = sdt_w_prim;
		sdt->i_wakeup = sdt_w_wakeup;
		spin_lock_init(&sdt->lock);	/* "sdt-priv-lock" */
		if ((sdt->next = *sdtp))
			sdt->next->prev = &sdt->next;
		sdt->prev = sdtp;
		*sdtp = sdt_get(sdt);
		printd(("%s: %p: linked module private structure\n", MOD_NAME, sdt));
		/* 
		   TPI configuration defaults */
		sdt->t.state = TS_NOSTATES;
		sdt->t.serv_type = T_CLTS;
		sdt->t.sequence = 0;
		sdt->t.pdu_size = 272 + 1 + 3;
		sdt->t.opt_size = -1UL;
		sdt->t.add_size = sizeof(struct sockaddr);
		/* 
		   LMI configuration defaults */
		sdt->option = lmi_default;
		/* 
		   SDL configuration defaults */
		sdt->sdl.config = sdl_default;
		sdt->sdl.timestamp = jiffies;
		sdt->sdl.tickbytes = sdt->sdl.config.ifrate / HZ / 8;
		sdt->sdl.bytecount = 0;
		/* 
		   SDT configuration defaults */
		sdt->sdt.config = sdt_default;
		printd(("%s: setting module private structure defaults\n", MOD_NAME));
	} else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", MOD_NAME));
	return (sdt);
}
STATIC void
sdt_free_priv(queue_t *q)
{
	struct sdt *sdt = SDT_PRIV(q);
	psw_t flags;
	ensure(sdt, return);
	spin_lock_irqsave(&sdt->lock, flags);
	{
		if (sdt->sdt.rx_cmp)
			freemsg(xchg(&sdt->sdt.rx_cmp, NULL));
		if (sdt->sdt.tx_cmp)
			freemsg(xchg(&sdt->sdt.tx_cmp, NULL));
		if (sdt->rbuf)
			freemsg(xchg(&sdt->rbuf, NULL));
		if (sdt->xbuf)
			freemsg(xchg(&sdt->xbuf, NULL));
		ss7_unbufcall((str_t *) sdt);
		sdt_timer_stop(sdt, tall);
		if ((*sdt->prev = sdt->next))
			sdt->next->prev = sdt->prev;
		sdt->next = NULL;
		sdt->prev = &sdt->next;
		sdt_put(sdt);
		sdt->oq->q_ptr = NULL;
		flushq(sdt->oq, FLUSHALL);
		sdt->oq = NULL;
		sdt_put(sdt);
		sdt->iq->q_ptr = NULL;
		flushq(sdt->iq, FLUSHALL);
		sdt->iq = NULL;
		sdt_put(sdt);
	}
	spin_unlock_irqrestore(&sdt->lock, flags);
	sdt_put(sdt);		/* final put */
}
STATIC struct sdt *
sdt_get(struct sdt *sdt)
{
	atomic_inc(&sdt->refcnt);
	return (sdt);
}
STATIC void
sdt_put(struct sdt *sdt)
{
	if (atomic_dec_and_test(&sdt->refcnt)) {
		kmem_cache_free(sdt_priv_cachep, sdt);
		printd(("%s: %p: freed sdt private structure\n", MOD_NAME, sdt));
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
MODULE_PARM_DESC(modid, "Module ID for the SDT module. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifdef LFS

STATIC struct fmodsw sdt_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sdt_tpiinfo,
	.f_flag = 0,
	.f_kmod = THIS_MODULE,
};

STATIC int
sdt_register_strmod(void)
{
	int err;
	if ((err = register_strmod(&sdt_fmod)) < 0)
		return (err);
	return (0);
}

STATIC int
sdt_unregister_strmod(void)
{
	int err;
	if ((err = unregister_strmod(&sdt_fmod)) < 0)
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
sdt_register_strmod(void)
{
	int err;
	if ((err = lis_register_strmod(&sdt_tpiinfo, MOD_NAME)) == LIS_NULL_MID)
		return (-EIO);
	return (0);
}

STATIC int
sdt_unregister_strmod(void)
{
	int err;
	if ((err = lis_unregister_strmod(&sdt_tpiinfo)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

MODULE_STATIC int __init
sdt_tpiinit(void)
{
	int err;
	cmn_err(CE_NOTE, MOD_BANNER);	/* banner message */
	if ((err = sdt_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", MOD_NAME, err);
		return (err);
	}
	if ((err = sdt_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		sdt_term_caches();
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

MODULE_STATIC void __exit
sdt_tpiterminate(void)
{
	int err;
	if ((err = sdt_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	if ((err = sdt_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", MOD_NAME);
	return;
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(sdt_tpiinit);
module_exit(sdt_tpiterminate);

#endif				/* LINUX */
