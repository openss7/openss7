/*****************************************************************************

 @(#) $RCSfile: cd.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/12/18 08:59:35 $

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

 Last Modified $Date: 2006/12/18 08:59:35 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: cd.c,v $
 Revision 0.9.2.2  2006/12/18 08:59:35  brian
 - working up strchan package

 Revision 0.9.2.1  2006/11/30 13:01:05  brian
 - added working files

 *****************************************************************************/

#ident "@(#) $RCSfile: cd.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/12/18 08:59:35 $"

static char const ident[] = "$RCSfile: cd.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/12/18 08:59:35 $";

/*
 *  This is a pushable STREAMS module that provides the High-Level Data Link
 *  Control.  It presents a Communication Device Interface (CDI) at the upper
 *  service interface and uses a Channel Interface (CHI) at the lower service
 *  interface.  This module is for raw channel devices and support HDLC in
 *  software.  Some devices might support HDLC in device firmware, in which
 *  case, the device driver itself will present the CDI interface.  The CDI
 *  interface presented supports the lapb(4), lapf(4), lapd(4), lapm(4) data
 *  link modules and the sl(4) SS7 link module.
 *
 *  This module is originally intended for use with the V401P card, but can be
 *  used with any raw channel device.
 *
 *  This module can also present a pseudo-device driver interface to provide
 *  access to HDLC device driver implementations in the Linux kernel.  Also,
 *  raw CHI streams can be linked under the pseudo-device driver and then
 *  addressed by interface (multiplex) identifier from the upper multiplex.
 */

/*
 *  This is CD driver than implements HDLC and SS7 DAEDR/DAEDT AERM/SUERM/IEM
 *  using the OpenSS7 Soft-HDLC approach that can be pushed over any CH
 *  Stream to provide a channel, fractional E1/T1/J1, full E1/T1/J1 or
 *  multiple E1/T1/J1 interface for X.25, FR, GSM and SS7 applications.
 *
 *  For SS7 the CH interface replaces the SDL interface (and is compatible
 *  with that interface) and the CD interface replaces the SDT interface (but
 *  is also compatible with that interface).
 */

#define LFS_SOURCE

#include <sys/cdi.h>		/* Upper CD interface. */
#include <sys/chi.h>		/* Lower CH interface. */

#include "tab.h"		/* HDLC tables */

#define CD_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMD FOR LINUX"
#define CD_COPYRIGHT	"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define CD_REVISION	"OpenSS7 $RCSfile: cd.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/12/18 08:59:35 $"
#define CD_DEVICE	"SVR 4.2 STREAMS Communications Device (CD)"
#define CD_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define CD_LICENSE	"GPL"
#define CD_BANNER	CD_DESCRIP	"\n" \
			CD_COPYRIGHT	"\n" \
			CD_REVISION	"\n" \
			CD_DEVICE	"\n" \
			CD_CONTACT
#define CD_SPLASH	CD_DEVICE	" - " \
			CD_REVISION

#ifdef LINUX
MODULE_AUTHOR(CD_CONTACT);
MODULE_DESCRIPTION(CD_DESCRIP);
MODULE_SUPPORTED_DEVICE(CD_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(CD_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-cd");
MODULE_ALIAS("streams-cdmod");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifndef CD_MOD_NAME
#define CD_MOD_NAME		"cd"
#endif

#ifndef CD_MOD_ID
#define CD_MOD_ID		0
#endif

#define MOD_ID		CD_MOD_ID
#define MOD_NAME	CD_MOD_NAME

#ifdef MODULE
#define MOD_BANNER	CD_BANNER
#define DRV_BANNER	CD_BANNER
#else				/* MODULE */
#define MOD_BANNER	CD_SPLASH
#define DRV_BANNER	CD_SPLASH
#endif				/* MODULE */

/*
 *  STREAMS Declarations.
 *  =====================
 */

STATIC struct module_info hdlc_minfo = {
	.mi_idnum = HDLC_MOD_ID,
	.mi_idname = HDLC_MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_lowat = STRLOW,
	.mi_hiwat = STRHIGH,
};

STATIC struct module_info daed_minfo = {
	.mi_idnum = DAED_MOD_ID,
	.mi_idname = DAED_MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_lowat = STRLOW,
	.mi_hiwat = STRHIGH,
};

STATIC struct module_info xray_minfo = {
	.mi_idnum = XRAY_MOD_ID,
	.mi_idname = XRAY_MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_lowat = STRLOW,
	.mi_hiwat = STRHIGH,
};

STATIC struct module_stat cd_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat cd_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 *  Private Structure.
 *  ==================
 */

/* This path structures is used once for the rx side and once for the tx side. */
struct cd_path {
	uint residue;			/* residue bits */
	ushort rbits;			/* number of residue bits */
	ushort bcc;			/* crc for message */
	ushort state;			/* current state machine state */
	ushort mode;			/* path mode */
	uint bytes;			/* number of whole bytes */
	mblk_t *msg;			/* message pointer */
	mblk_t *nxt;			/* current message pointer */
	mblk_t *cmp;			/* compression/repeat pointer */
	uint repeat;			/* compression repeat count */
	uint octets;			/* octets counted */
};

/* cd_path.mode for the rx state machine */
#define RX_MODE_IDLE	0	/* line is idle */
#define RX_MODE_HUNT	1	/* hunting for flags */
#define RX_MODE_SYNC	2	/* between frames synced on flags */
#define RX_MODE_MOF	3	/* middle of frame */

/* cd_path.mode for the tx state machine */
#define TX_MODE_IDLE	0	/* generating mark idle */
#define TX_MODE_FLAG	1	/* generating flag idle */
#define TX_MODE_BOF	2	/* generating bof flag */
#define TX_MODE_MOF	3	/* generating frame */
#define TX_MODE_BCC	4	/* generating FCS bytes */
#define TX_MODE_ABORT	5	/* abort current frame */

/* This configuration structure contains (read-only during state machine processing) information
 * used by both the transmit side and the receive side of the soft-hdlc engine. */
struct cd_conf {
	ushort bits;			/* number of bits significant in octet (7 or 8) */
	ushort flags;			/* how many and what type of flags between frames */
	const struct rx_entry *rx_table;	/* which rx table to use: rx_table7 or rx_table8 */
	ushort sdu_min;			/* minimum number of octets in frame 3 or 6 */
	ushort sdu_max;			/* maximum number of octets in frame sdu_min + 1 + "M" */
	ushort li_mask;			/* SS7 length indicator mask (zero for no LI check) */
	ushort octets_per_su;		/* SS7 number octets bad SU "N" (zero for no octet
					   counting) */
};

/* Constants for the cd_conf flags member. */
#define TX_FLAGS_ONE	0	/* one flag 01111110 */
#define TX_FLAGS_SHARED	1	/* shared flag 011111101111110 */
#define TX_FLAGS_TWO	2	/* two flags 0111111001111110 */
#define TX_FLAGS_THREE	3	/* three flags 011111100111111001111110 */

struct cd;
struct ch;

struct cd {
	struct ch *ch;
	cd_info_ack_t info;
	cd_ulong ppa;
};

struct ch {
	struct cd *cd;
	struct CH_info_ack info;
	ch_ulong addr;
	struct CH_parms_circuit parms;
};

struct priv {
	struct priv *next;
	struct priv **prev;
	struct cd cd;
	struct ch ch;
};

#define PRIV(__q) ((struct priv *)q->q_ptr)
#define CD_PRIV(__q) (&PRIV(_q)->cd)
#define CH_PRIV(__q) (&PRIV(_q)->ch)

/*
 *  CD Primitives issued up to the CD user.
 *  ---------------------------------------
 */

/**
 * cd_info_ack: issue CD_INFO_ACK message
 * @cd: private structure
 * @q: active queue
 * @msg: message to free on success
 *
 * - cd_state is the current state of the interface.
 * - cd_max_sdu is the maximum sized sdu
 * - cd_min_sdu is the minimum sized sdu
 * - cd_class is the class of the device and is one of CD_HDLC, CD_BISYNC, CD_LAN, CD_DAED or
 *   CD_NODEV; always CD_HDLC, CD_DAED or CD_NODEV for this module.
 * - cd_duplex one of CD_FULLDUPLEX or CD_HALFDUPLEX; always CD_FULLDUPLEX for this module.
 */
STATIC __unlikely int
cd_info_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_info_ack_t *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_INFO_ACK;
		p->cd_state = cd->info.cd_state;
		p->cd_max_sdu = cd->info.cd_max_sdu;
		p->cd_min_sdu = cd->info.cd_min_sdu;
		p->cd_class = cd->info.cd_class;
		p->cd_duplex = cd->info.cd_duplex;
		p->cd_output_style = cd->info.cd_output_style;
		p->cd_features = cd->info.cd_features;
		p->cd_addr_length = cd->info.cd_addr_length;
		p->cd_ppa_style = cd->info.cd_ppa_style;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- CD_INFO_ACK\n", CD_MOD_NAME, cd));
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_ok_ack: issue CD_OK_ACK message
 * @cd: private structure
 * @q: active queue
 * @msg: message to free on success
 * @state: resulting state
 * @prim: correct primitive
 */
STATIC __unlikely int
cd_ok_ack(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong state, cd_ulong prim)
{
	cd_ok_ack_t *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_OK_ACK;
		p->cd_state = cd_set_state(cd, state);
		p->cd_correct_primitive = prim;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- CD_OK_ACK\n", CD_MOD_NAME, cd));
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_error_ack: issue CD_ERROR_ACK message
 * @cd: private structure
 * @q: active queue
 * @msg: message to free on success
 * @prim: primitive in error
 * @error: error number (positive CD, negative unix)
 */
STATIC __unlikely int
cd_error_ack(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong prim, cd_long error)
{
	cd_error_ack_t *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ERROR_ACK;
		if (error == CD_FATALERR)
			cd_set_state(cd, CD_UNUSABLE);
		else
			cd_set_state(cd, cd->i_oldstate);
		p->cd_state = cd_get_state(cd);
		p->cd_error_primitive = prim;
		p->cd_errno = error < 0 ? CD_SYSERR : error;
		p->cd_explanation = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- CD_ERROR_ACK\n", CD_MOD_NAME, cd));
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_enable_con: issue CD_ENABLE_CON message
 * @cd: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC __unlikely int
cd_enable_con(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_enable_con_t *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ENABLE_CON;
		p->cd_state = cd_set_state(cd, CD_INPUT_ALLOWED);
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- CD_ENABLE_CON\n", CD_MOD_NAME, cd));
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_disable_con: issue CD_DISABLE_CON message
 * @cd: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC __unlikely int
cd_disable_con(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_disable_con_t *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_DISABLE_CON;
		p->cd_state = cd_set_state(cd, CD_DISABLED);
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- CD_DISABLE_CON\n", CD_MOD_NAME, cd));
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_error_ind: issue CD_ERROR_IND message
 * @cd: private structure
 * @q: active queue
 * @msg: message to free on success
 * @error: error number
 * @reason: reason code
 */
STATIC __unlikely int
cd_error_ind(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong error, cd_ulong reason)
{
	cd_error_ind_t *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_ERROR_IND;
		p->cd_state = cd_get_state(cd);
		p->cd_errno = error;
		p->cd_explanation = reason;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- CD_ERROR_IND\n", CD_MOD_NAME, cd));
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_unitdata_ack: issue CD_UNITDATA_ACK message
 * @cd: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC __unlikely int
cd_unitdata_ack(struct cd *cd, queue_t *q, mblk_t *msg)
{
	cd_unitdata_ack_t *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		mp->b_band = 2;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_UNITDATA_ACK;
		p->cd_state = cd_get_state(cd);
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- CD_UNITDATA_ACK\n", CD_MOD_NAME, cd));
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_unitdata_ind: issue CD_UNITDATA_IND message
 * @cd: private structure
 * @q: active queue
 * @msg: message to free on success (and data)
 */
STATIC __unlikely int
cd_unitdata_ind(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong type, cd_ulong pri, size_t slen,
		caddr_t sptr, size_t dlen, caddr_t dptr)
{
	cd_unitdata_ind_t *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p) + slen + dlen, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_UNITDATA_IND;
		p->cd_state = cd->info.cd_state;
		p->cd_src_addr_length = slen;
		p->cd_src_addr_offset = slen ? sizeof(*p) : 0;
		p->cd_addr_type = type;
		p->cd_priority = pri;
		p->cd_dest_addr_length = dlen;
		p->cd_dest_addr_offset = dlen ? sizeof(*p) + slen : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(sptr, mp->b_wptr, slen);
		mp->b_wptr += slen;
		bcopy(dptr, mp->b_wptr, dlen);
		mp->b_wptr += dlen;
		mp->b_cont = XCHG(&msg->b_cont, NULL);
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_bad_frame_ind: issue CD_BAD_FRAME_IND message
 * @cd: private structure
 * @q: active queue
 * @msg: message to free on success
 * @error: error number
 */
STATIC __unlikely int
cd_bad_frame_ind(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong error)
{
	cd_bad_frame_ind_t *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_BAD_FRAME_IND;
		p->cd_state = cd_get_state(cd);
		p->cd_error = error;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- CD_BAD_FRAME_IND\n", CD_MOD_NAME, cd));
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * cd_modem_sig_ind: issue CD_MODEM_SIG_IND message
 * @cd: private structure
 * @q: active queue
 * @msg: message to free on success
 * @sigs: modem signals
 */
STATIC __unlikely int
cd_modem_sig_ind(struct cd *cd, queue_t *q, mblk_t *msg, cd_ulong sigs)
{
	cd_modem_sig_ind_t *p;
	mblk_t *mp;

	if ((mp = cd_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		mp->b_band = 1;
		p = (typeof(p)) mp->b_wptr;
		p->cd_primitive = CD_MODEM_SIG_IND;
		p->cd_sigs = sigs;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: <- CD_MODEM_SIG_IND\n", CD_MOD_NAME, cd));
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  CH Primitives issued down to the CH provider.
 *  ---------------------------------------------
 */

/**
 * ch_info_req: issue CH_INFO_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC __unlikely int
ch_info_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_info_req *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: CH_INFO_REQ ->\n", CD_MOD_NAME, ch));
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_optmgmt_req: issue CH_OPTMGMT_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free on success
 * @flags: management flags
 * @olen: options length
 * @optr: pointer to options
 */
STATIC __unlikely int
ch_optmgmt_req(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, size_t olen, caddr_t optr)
{
	struct CH_optmgmt_req *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p) + olen, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OPTMGMT_REQ;
		p->ch_opt_length = olen;
		p->ch_opt_offset = olen ? sizeof(*p) : 0;
		p->ch_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		printd(("%s: %p: CH_OPTMGMT_REQ ->\n", CD_MOD_NAME, ch));
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_attach_req: issue CH_ATTACH_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free on success
 * @flags: attach flags
 * @alen: address length
 * @aptr: pointer to address
 */
STATIC __unlikely int
ch_attach_req(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, size_t alen, caddr_t aptr)
{
	struct CH_attach_req *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p) + alen, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_ATTACH_REQ;
		p->ch_addr_length = alen;
		p->ch_addr_offset = alen ? sizeof(*p) : 0;
		p->ch_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		ch_set_state(ch, CHS_WACK_AREQ);
		printd(("%s: %p: CH_ATTACH_REQ ->\n", CH_MOD_NAME, ch));
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_enable_req: issue CH_ENABLE_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC inline fastcall __unlikely int
ch_enable_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_enable_req *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_ENABLE_REQ;
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, CHS_WACK_EREQ);
		printd(("%s: %p: CH_ENABLE_REQ ->\n", CD_MOD_NAME, ch));
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_connect_req: issue CH_CONNECT_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free on success
 * @flags: connect flags
 * @slot: slot in channel
 */
STATIC __unlikely int
ch_connect_req(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_connect_req *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_CONNECT_REQ;
		p->ch_conn_flags = flags;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		printd(("%s: %p: CH_CONNECT_REQ ->\n", MOD_NAME, ch));
		ch->info.ch_state = CHS_WACK_CREQ;
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_data_req: issue CH_DATA_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free on success (and data)
 * @slot: slot in channel
 */
STATIC __unlikely int
ch_data_req(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong slot)
{
	struct CH_data_req *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DATA_REQ;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = XCHG(&msg->b_cont, NULL);
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disconnect_req: issue CH_DISCONNECT_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free on success
 * @flags: connect flags
 * @slot: slot in channel
 */
STATIC __unlikely int
ch_disconnect_req(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_disconnect_req *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISCONNECT_REQ;
		p->ch_conn_flags = flags;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disable_req: issue CH_DISABLE_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC __unlikely int
ch_disable_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_disable_req *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISABLE_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_detach_req: issue CH_DETACH_REQ primitive
 * @ch: private structure
 * @q: active queue
 * @msg: message to free on success
 */
STATIC __unlikely int
ch_detach_req(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_detach_req *p;
	mblk_t *mp;

	if ((mp = ch_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DETACH_REQ;
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, CHS_WACK_UREQ);
		printd(("%s: %p: CH_DETACH_REQ ->\n", CD_MOD_NAME, ch));
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  CD/CH State Machine
 *  -------------------
 */

/* Events for CD_EVENT_IND for SS7. */
#define CD_RC_CONGESTION_ACCEPT		(CD_DEVERR + 0x100 + 1)
#define CD_RC_CONGESTION_DISCARD	(CD_DEVERR + 0x100 + 2)
#define CD_RC_NO_CONGESTION		(CD_DEVERR + 0x100 + 3)
#define CD_IAC_CORRECT_SU		(CD_DEVERR + 0x100 + 4)
#define CD_IAC_ABORT_PROVING		(CD_DEVERR + 0x100 + 5)		/* CD_INITFAILED */
#define CD_LSC_LINK_FAILURE		(CD_DEVERR + 0x100 + 6)		/* CD_INITFAILED */

/* Controls (M_CTL) for SS7. */
#define DAEDT_AERM_START		1
#define DAEDT_AERM_STOP			2
#define DAEDT_AERM_SET_NORMAL		3   /* argument is value of tin */
#define DAEDT_AERM_SET_EMERGENCY	4   /* argument is value of tie */
#define DAEDT_SUERM_START		5
#define DAEDT_SUERM_STOP		6


/*
 *  SDT State Machine.
 *  ------------------
 */
static inline fastcall void
sdt_aerm_start(struct cd *cd, queue_t *q)
{
	cd->sdt.statem.Ca = 0;
	cd->sdt.statem.aborted_proving = 0;
	cd->sdt.statem.aerm_state = SDT_STATE_MONITORING;
}
static inline fastcall void
sdt_aerm_stop(struct cd *cd, queue_t *q)
{
	cd->sdt.statem.aerm_state = SDT_STATE_IDLE;
	cd->sdt.statem.Ti = cd->sdt.config.Tin;
}
static inline fastcall void
sdt_aerm_set_ti_to_tin(struct cd *cd, queue_t *q)
{
	if (cd->std.statem.aerm_state == SDT_STATE_IDLE)
		cd->sdt.statem.Ti = cd->sdt.config.Tin;
}
static inline fastcall void
sdt_aerm_set_ti_to_tie(struct cd *cd, queue_t *q)
{
	if (cd->std.statem.aerm_state == SDT_STATE_IDLE)
		cd->sdt.statem.Ti = cd->sdt.config.Tie;
}
static inline fastcall void
sdt_aerm_su_in_error(struct cd *cd, queue_t *q)
{
	if (cd->sdt.statem.aerm_state == SDT_STATE_MONITORING) {
		cd->sdt.statem.Ca++;
		if (cd->sdt.statem.Ca == cd->sdt.statem.Ti) {
			cd->sdt.statem.aborted_proving = 1;
			sl_iac_abort_proving(cd, q);
			cd->sdt.statem.Ca--;
			cd->sdt.statem.aerm_state = SDT_STATE_IDLE;
		}
	}
}
static inline fastcall void
sdt_aerm_correct_su(struct cd *cd, queue_t *q)
{
	if (cd->sdt.statem.aerm_state == SDT_STATE_IDLE) {
		if (cd->sdt.statem.aborted_proving) {
			sl_iac_correct_su(cd, q);
			cd->sdt.statem.aborted_proving = 0;
		}
	}
}

static inline fastcall void
sdt_eim_stop(struct cd *cd, queue_t *q)
{
	cd->sdt.statem.eim_state = SDT_STATE_IDLE;
	cd_timer_stop(cd, t8);
}
static inline fastcall void
sdt_eim_start(struct cd *cd, queue_t *q)
{
	cd->sdt.statem.Ce = 0;
	cd->sdt.statem.interval_error = 0;
	cd->sdt.statem.su_received = 0;
	cd_timer_start(cd, t8);
	cd->sdt.statem.eim_state = SDT_STATE_MONITORING;
}
static inline fastcall void
sdt_eim_su_in_error(struct cd *cd, queue_t *q)
{
	if (cd->sdt.statem.eim_state == SDT_STATE_MONITORING)
		cd->sdt.statem.interval_error = 1;
}
static inline fastcall void
sdt_eim_correct_su(struct cd *cd, queue_t *q)
{
	if (cd->sdt.statem.eim_state == SDT_STATE_MONITORING)
		cd->sdt.statem.su_received = 1;
}

static inline fastcall void
sdt_suerm_stop(struct cd *cd, queue_t *q)
{
	sdt_eim_stop(cd, q);
	cd->sdt.statem.suerm_state = SDT_STATE_IDLE;
}
static inline fastcall void
sdt_suerm_start(struct cd *cd, queue_t *q)
{
	if (cd->option.popt & SS7_POPT_HSL)
		sl_eim_start(cd, q);
	else {
		cd->sdt.statem.Cs = 0;
		cd->sdt.statem.Ns = 0;
		cd->sdt.statem.suerm_state = SDT_STATE_IN_SERVICE;
	}
}
static inline fastcall void
sdt_suerm_su_in_error(struct cd *cd, queue_t *q)
{
	if (cd->sdt.statem.suerm_state == SDT_STATE_IN_SERVICE) {
		cd->sdt.statem.Cs++;
		if (cd->sdt.statem.Cs >= cd->sdt.config.T) {
			sl_lsc_link_failure(cd, q, SL_FAIL_SUERM_EIM);
			cd->sdt.statem.Ca--;
			cd->sdt.statem.suerm_state = SDT_STATE_IDLE;
			return;
		}
		cd->sdt.statem.Ns++;
		if (cd->sdt.statem.Ns >= cd->sdt.config.D) {
			cd->sdt.statem.Ns = 0;
			if (cd->sdt.statem.Cs)
				cd->sdt.statem.Cs--;
		}
	}
}
static inline fastcall void
sdt_suerm_correct_su(struct cd *cd, queue_t *q)
{
	if (cd->sdt.statem.suerm_state == SDT_STATE_IN_SERVICE) {
		cd->sdt.statem.Ns++;
		if (cd->sdt.statem.Ns >= cd->sdt.config.D) {
			cd->sdt.statem.Ns = 0;
			if (cd->sdt.statem.Cs)
				cd->sdt.statem.Cs--;
		}
	}
}

static inline fastcall void
sdt_daedr_start(struct cd *cd, queue_t *q)
{
	cd->sdt.statem.daedr_state = SDT_STATE_IN_SERVICE;
	cd->info.cd_state = CD_INPUT_ALLOWED;
}
static inline fastcall void
sdt_daedr_correct_su(struct cd *cd, queue_t *q)
{
	sdt_eim_correct_su(cd, q);
	sdt_suerm_correct_su(cd, q);
	sdt_aerm_correct_su(cd, q);
}
static inline fastcall void
sdt_daedr_su_in_error(struct cd *cd, queue_t *q)
{
	sdt_eim_su_in_error(cd, q);
	sdt_suerm_su_in_error(cd, q);
	sdt_aerm_su_in_error(cd, q);
}
static inline fastcall void
sdt_daedt_start(struct cd *cd, queue_t *q)
{
	cd->sdt.statem.daedt_state = SDT_STATE_IN_SERVICE;
	// cd->info.cd_state = CD_ENABLED;
}
static inline fastcall void
sdt_daedt_transmission_request(struct cd *cd, queue_t *q)
{
	sl_txc_transmission_request(cd, q);
}

static inline fastcall register unsigned char cd_rev(register unsigned char byte) __attribute__((const));
/**
 * cd_rev: reverse bits in a byte
 * @byte: byte in which to reverse bits
 *
 * An attempt at the fastest bit reversal possible without a machine instruction.
 */
static inline fastcall register unsigned char
cd_rev(register unsigned char byte)
{
	byte = (byte << 4) | (byte >> 4);
	byte = ((byte << 2) & 0xcc) | ((byte >> 2) & 0x33);
	byte = ((byte << 1) & 0xaa) | ((byte >> 1) & 0x55);
	return byte;
}

static noinline fastcall void
sdt_daedr_signal_unit(struct cd *cd, queue_t *q, cd_path_t * rx, sdt_stats_t * stats)
{
	/* For normal operation (with an SS7 signalling link module pushed over this one), we do
	   not want to compress SUs.  The reason for this is that the read put procedure of the
	   upper layer module can process them immediately without deferral and should be easily
	   able to handle being passed all SUs.  Because we have already allocated a message block
	   to hold the FISU or LSSU, all we will do by compressing it is save a call to put(). */
	/* Only sed cmp_max > sdu_min if you want to compress SS7. */
	if (rx->cmp || rx->bytes <= config->cmp_max) {
		if (unlikely(!rx->cmp))
			goto no_comparison;
		if (unlikely(rx->bytes > config->cmp_max))
			goto no_match;
		if (unlikely(rx->bytes != rx->cmp_length))
			goto no_match;
		{
			int i;

			for (i = 0; i < rx->bytes; i++)
				if (unlikely(mp->b_rptr[i] != rx->cmp->b_rptr[i]))
					goto no_match;
		}
#if 0
		if (rx->repeat >= 50)
			goto no_match;
#endif
		rx->repeat++;
		stats->rx_sus_compressed++;
		return;
	      no_match:
		if (rx->repeat) {
			/* deliver up repeat frame and repeat count */
			if (sl_rc_signal_unit(cd, q, rx->cmp, rx->repeat))
				stats->rx_buffer_overflows++;
			else
				rx->cmp = NULL;
		}
		freemsg(XCHG(&rx->cmp, NULL));
		rx->cmp_length = 0;
		rx->repeat = 0;
		if (compressable) {
		      no_comparison:
			/* setup new comparison buffer */
			/* just don't compress if cannot get buffer */
			if ((rx->cmp = allocb(FASTBUF, BPRI_HI))) {
				bcopy(mp->b_rptr, rx->cmp->b_rptr, rx->bytes);
				rx->cmp->b_wptr = rx->cmp->b_rptr + rx->bytes;
				rx->cmp_length = rx->bytes;
			}
			rx->repeat = 0;
		}
	}
	if (sl_rc_signal_unit(cd, q, mp, 1)) {
		stats->rx_buffer_overflows++;
		freemsg(XCHG(&mp, NULL));
	}
	return;
}

/*
 *  Soft-HDLC/DAED
 *  ==============
 */

static inline fastcall const struct tx_entry *
tx_lookup(ushort state, uchar byte)
{
	return &tx_table[(state << 8) | byte];
}

/**
 * cd_tx_getq: - go looking for an M_DATA block on the write queue.
 * @cd: private structure
 * @q: active queue
 */
static inline fastcall mblk_t *
cd_tx_get(struct cd *cd, queue_t *q)
{
}

/**
 * cd_tx_block: generate a transmit bit block
 * @cd: private structure
 * @q: active queue
 * @mp: message frame to transmit
 *
 * Begins or continues transmission of the message @mp.  While transmitting @mp, the function
 * returns a bit block suitable to be passed to the channel provider.  When it is completed
 * transmitting @mp, NULL is returned.
 */
static inline fastcall mblk_t *
cd_tx_block(struct cd *cd, queue_t *q, mblk_t *mp)
{
	register struct cd_path *tx = &cd->tx;
	register struct tx_entry *t;
	register const struct cd_conf *cf = &cd->cf;
	struct sdt_stats *stats = &cd->sdt.stats;
	mblk_t *bp, *mb;

	if (mp != tx->msg) {
		if (tx->mode == TX_MODE_MOF)
			tx->mode == TX_MODE_ABORT;
		if ((tx->msg = tx->nxt = mp))
			if (tx->mode == TX_MODE_FLAG || tx->mode == TX_MODE_IDLE)
				tx->mode = TX_MODE_BOF;
	}

	mp = tx->msg;		/* current message being sent */
	mb = tx->nxt;		/* current message block in message being sent */
	bp = tx->blk;		/* current bit block pointer */

	while (likely(!!bp || !!(bp = tx->blk = cd_allocb(q, cf->bsize, BPRI_HI)))) {
		/* check if transmission block complete */
		while (bp->b_wptr < bp->b_datap->db_lim) {
			/* drain residue bits, if necessary */
		      check_rbits:
			if (likely(tx->rbits >= 8)) {
			      drain_rbits:
				*bp->b_wptr++ = tx->residue >> (tx->rbits - 8);
				rbits -= cf->bits;
				continue;
			}
			switch (tx->mode) {
			case TX_MODE_MOF:
				if (likely(mb->b_rptr < mb->b_wptr)
				    || unlikely((mb = tx->nxt = mb->b_cont))) {
					uint byte;

					byte = *mb->b_rptr++;
					/* continuing in message */
					tx->bcc =
					    (tx->bcc >> 8) ^ bc_table[(tx->bcc ^ byte) & 0x00ff];
					t = tx_lookup(tx->state, byte);
					tx->state = t->state;
					prefetch(tx_lookup(tx->state, mb->b_rptr));
					tx->residue <<= 8 + t->bit_length;
					tx->residue |= (t->bit_string << (8 - cf->bits));
					tx->rbits += 8 + t->bit_length;
					stats->tx_bytes++;
				} else {
					/* finished message */
					freemsg(XCHG(&mp, NULL));
					tx->msg = NULL;
					/* add 1st bcc byte */
					tx->bcc = ~(tx->bcc);
					t = tx_lookup(tx->state, tx->bcc);
					tx->state = t->state;
					prefetch(tx_lookup(tx->state, tx->bcc >> 8));
					tx->residue <<= 8 + t->bit_length;
					tx->residue |= (t->bit_string << (8 - cf->bits));
					tx->rbits += 8 + t->bit_length;
					tx->mode = TX_MODE_BCC;
				}
				goto drain_rbits;
			case TX_MOD_BOF:
			      beginning_of_frame:
				/* add opening flag (also closing flag) */
				switch (cf->flags) {
				default:
				case TX_FLAGS_ONE:
					tx->residue <<= 8;
					tx->residue |= (0x7e << (8 - cf->bits));
					tx->rbits += 8;
					break;
				case TX_FLAGS_SHARED:
					tx->residue <<= 15;
					tx->residue |= (0x3f7e << (8 - cf->bits));
					tx->rbits += 15;
					break;
				case TX_FLAGS_TWO:
					tx->residue <<= 16;
					tx->residue |= (0x7e7e << (8 - cf->bits));
					tx->rbits += 16;
					break;
				case TX_FLAGS_THREE:
					tx->residue <<= 24;
					tx->residue |= (0x7e7e7e << (8 - cf->bits));
					tx->rbits += 24;
					break;
				}
				tx->state = 0;
				tx->bcc = 0xffff;
				tx->mode = TX_MODE_MOF;
				goto drain_rbits;
			case TX_MODE_BCC:
				/* add 2nd bcc byte */
				t = tx_lookup(tx->state, tx->bcc >> 8);
				tx->state = t->state;
				/* no prefetch available */
				tx->residue <<= 8 + t->bit_length;
				tx->residue |= (t->bit_string << (8 - cf->bits));
				tx->rbits += 8 + t->bit_length;
				if (likely(!mb && !(mb = tx->nxt = mp)))
					break;
				tx->mode = TX_MODE_BOF;
				goto drain_rbits;
			case TX_MODE_ABORT:
				/* abort current message */
				tx->residue <<= 7;
				tx->residue |= (0x7f << (8 - cf->bits));
				tx->rbits += 7;
				if (unlikely(!mb && !(mb = tx->nxt = mp)))
					break;
				tx->mode = TX_MODE_BOF;
				goto check_rbits;
			case TX_MODE_FLAG:
				if (likely(!mb && !(mb = tx->nxt = mp)))
					break;
				tx->mode = TX_MODE_BOF;
				goto beginning_of_frame;
			case TX_MODE_IDLE:
				if (likely(!mb && !(mb = tx->nxt = mp)))
					break;
				tx->mode = TX_MODE_BOF;
				goto beginning_of_frame;
			}
			/* one more time for exceptions */
			switch (tx->mode) {
			case TX_MODE_ABORT:
				/* aborted frame */
			case TX_MODE_BCC:
				/* completed frame */
				tx->mode = TX_MODE_FLAG;
				return (0);	/* stop here */
			case TX_MODE_FLAG:
				/* idle flags */
				tx->residue <<= 8;
				tx->residue |= (0x07e << (8 - cf->bits));
				tx->rbits += 8;
				tx->mode = TX_MODE_FLAG;
				/* continue filling out 1 bit block */
				goto drain_rbits;
			case TX_MODE_IDLE:
				/* mark idle */
				tx->residue <<= 8;
				tx->residue |= (0x0ff << (8 - cf->bits));
				tx->rbits += 8;
				tx->mode = TX_MODE_IDLE;
				/* continue filling out 1 bit block */
				goto drain_rbits;
			default:
				swerr();
				return (-EFAULT);
			}
		}
		putnext(WR(q), XCHG(&bp, NULL));
		if (!bcanputnext(WR(q), 1))
			return (-EBUSY);
		/* do not fill out more than one block of these */
		if (unlikely(tx->mode == TX_MODE_IDLE || tx->mode == TX_MODE_FLAG))
			return (0);	/* stop here */
	}
	return (-ENOBUFS);
}

static inline fastcall int
cd_tx_process(struct cd *cd, queue_t *q, mblk_t *mp)
{
	mblk_t *bp;

	for (;;) {
		/* send transmit bit blocks one fastbuf at a time for speed */
		if (!bcanputnext(WR(q), 1))
			return (-EBUSY);
		if (!(bp = cd_tx_block(cd, q, mp)))
			break;
		putnext(WR(q), bp);
	}
	for (;;) {
		if (!bcanputnext(WR(q), 1))
			return (-EBUSY);
		if (!(bp = cd_tx_block(cd, q, NULL)))
			break;
		putnext(WR(q), bp);
	}
}

/**
 * ch_rx_block: process a received bit block
 * @ch: private structre (for channel)
 * @q: active queue (read queue)
 * @bp: message block containing received bits
 *
 * Message block is always consumed completely.  If message block cannot be allocated to complete
 * the received frame, the frame will be discarded and appropriate error counts incremented.  These
 * are, however, a software error rather than a hardware transmission error and it is up to the
 * upper layer state machine to figure that out.
 */
static inline fastcall void
ch_rx_block(struct ch *ch, queue_t *q, mblk_t *bp)
{
	struct cd *cd = ch->cd;
	register struct cd_path *rx = &ch->rx;
	register struct rx_entry *r;
	register const struct cd_conf *cf = &cd->cf;
	struct sdt_stats *stats = &cd->sdt.stats;

	mblk_t *mp = rx->msg;		/* message frame being built */
	mblk_t *mb = rx->nxt;		/* current message block in message frame */

	while (bp->b_rptr < bp->b_wptr) {
		r = cf->rx_table[(rx->state << 8) | *bp->b_rptr++];
		rx->state = r->state;
		prefetch(&cf->rx_table[(rx->state << 8) | bp->b_rptr]);
		switch (rx->mode) {
		case RX_MODE_MOF:
			/* middle of frame */
			if (!r->sync && r->bit_length) {
				rx->residue |= r->bit_string << rx->rbits;
				rx->rbits += r->bit_length;
			}
			if (!r->flag) {
				if (r->hunt || r->idle)
					goto aborted;
				while (rx->rbits > 16) {
					if (mb && mb->b_wptr >= mb->b_datap->db_lim)
						mp = linkmsg(mp, XCHG(&mb, NULL));
					if (!mb && !(mb = ch_allocb(q, cf->msize, BPRI_HI)))
						goto buffer_overflow;
					rx->bcc = (rx->bcc >> 8)
					    ^ bc_table[(rc->bcc ^ rx->residue) & 0x00ff];
					*mb->b_wptr++ = rx->residue;
					stats->rx_bytes++;
					rx->residue >>= 8;
					rx->rbits -= 8;
					rx->bytes++;
					if (rx->bytes > cf->sdu_max)
						goto frame_too_long;
				}
			} else {
				if (unlikely(rx->rbits != 16))
					goto residue_error;
				if (unlikely((~rx->bcc & 0xffff) != (rx->residue & 0xffff)))
					goto crc_error;
				if (unlikely(rx->bytes < cf->sdu_min))
					goto frame_too_short;
				mp = linkmsg(mp, XCHG(&mb, NULL));
				if (cf->li_mask) {
					unsigned char *buf = mp->b_rptr;
					uint len = rx->bytes - cf->sdu_min;
					uint li = (((uint) buf[cf->sdu_min - 1] << 8) |
						   buf[cf->sdu_min - 2]) & cf->li_mask;

					if (unlikely(len != li && (len < li || li != cf->li_mask)))
						goto length_error;
					sdt_daedr_correct_su(cd, q);
				}
				stats->rx_sus++;
				rdt_daedr_signal_unit(cd, q, rx, stats);
			      new_frame:
				rx->mode = RX_MODE_SYNC;
				rx->residue = 0;
				rx->rbits = 0;
				if (r->sync) {
				      begin_frame:
					if (r->bit_length) {
						rx->mode = RX_MODE_MOF;
						rx->residue = r->bit_string;
						rx->rbits = r->bit_length;
					}
				}
				rx->bytes = 0;
				rx->bcc = 0xffff;
			}
			break;
		case RX_MODE_SYNC:
			if (!r->hunt && !r->idle)
				goto begin_frame;
		      start_hunt:
			if (r->idle) {
				rx->mode = RX_MODE_IDLE;
				cd_error_ind(cd, q, NULL, CD_EVENT, CD_HDLC_IDLE);
			} else
				rx->mode = RX_MODE_HUNT;
			stats->rx_sync_transitions++;
			rx->octets = 0;
			break;
		case RX_MODE_IDLE:
			if (!r->idle) {
				rx->mode = RX_MODE_HUNT;
				cd_error_ind(cd, q, NULL, CD_EVENT, CD_HDLC_NOTIDLE);
			}
			/* fall through */
		case RX_MODE_HUNT:
			if (!r->flag) {
				if (cf->octets_per_su) {
					if ((++rx->octets) >= cf->octets_per_su) {
						stats->rx_sus_in_error++;
						sdt_daedr_su_in_error(cd, q);
						rx->octets -= cf->octets_per_su;
					}
					stats->rx_bits_octet_counted += 8;
				}
				if (r->idle && rx->mode != RX_MODE_IDLE) {
					rx->mode = RX_MODE_IDLE;
					cd_error_ind(cd, q, NULL, CD_EVENT, CD_HDLC_IDLE);
				}
				break;
			}
			stats->rx_sync_transitions++;
			goto new_frame;
		      frame_too_long:
			stats->rx_frame_too_long++;
			stats->rx_frame_errors++;
			cd_error_ind(cd, q, NULL, CD_EVENT, CD_FORMAT);
			goto abort_frame;
		      buffer_overflow:
			stats->rx_buffer_overflows++;
			cd_error_ind(cd, q, NULL, CD_EVENT, CD_OVERRUN);
			goto abort_frame;
		      aborted:
			stats->rx_aborts++;
			stats->rx_frame_errors++;
			cd_error_ind(cd, q, NULL, CD_EVENT, CD_HDLC_ABORT);
			goto abort_frame;
		      length_error:
			stats->rx_length_error++;
			cd_error_ind(cd, q, NULL, CD_EVENT, CD_FORMAT);
			goto abort_frame;
		      frame_too_short:
			stats->rx_frame_too_short++;
			stats->rx_frame_errors++;
			cd_error_ind(cd, q, NULL, CD_EVENT, CD_TOOSHORT);
			goto abort_frame;
		      crc_error:
			stats->rx_crc_errors++;
			cd_error_ind(cd, q, NULL, CD_EVENT, CD_CRCERR);
			goto abort_frame;
		      residue_error:
			stats->rx_residue_errors++;
			stats->rx_frame_errors++;
			cd_error_ind(cd, q, NULL, CD_EVENT, CD_INCOMPLETE);
			goto abort_frame;
		      abort_frame:
			freemsg(XCHG(&mb, NULL));
			freemsg(XCHG(&mp, NULL));
			stats->rx_sus_in_error++;
			sdt_daedr_su_in_error(cd, q);
			if (r->flag)
				goto new_frame;
			goto start_hunt;
		default:
			swerr();
			goto abort_frame;
		}
	}
	rx->msg = mp;
	rx->nxt = mb;
}

static inline fastcall int
ch_rx_process(struct ch *ch, queue_t *q, mblk_t *mp)
{
	if (!ch->rx.nxt && !(ch->rx.nxt = allocb(q, ch->cd->cf.msize, BPRI_HI)))
		return (-ENOBUFS);
	if (!bcanputnext(RD(q), 0))
		return (-EBUSY);
	ch_rx_block(ch, q, mp);
	return (0);
}





/*
 *  CD Primitives received from CD user above.
 *  ------------------------------------------
 */
/**
 * cd_info_req: process CD_INFO_REQ primitive
 * @cd: private structure
 * @q: active queue (write queue)
 * @mp: the CD_INFO_REQ primitive
 */
STATIC noinline __unlikely int
cd_info_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_info_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	return cd_info_ack(cd, q, mp);
      emsgsize:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_ack(cd, q, mp, CD_DETACH_REQ, err);
}

/**
 * cd_attach_req: process CD_ATTACH_REQ primitive
 * @cd: private structure
 * @q: active queue (write queue)
 * @mp: the CD_ATTACH_REQ primitive
 *
 * The PPA is as follows: 8 bits type, 8 bits card, 8 bits span, 8 bits time slot.  The type is one
 * of the possible CD classes CD_HDLC or CD_DAED.  The card number identifies the card upon which the
 * PPA resides.  The span is 0-15 for a specific span or 0xff to indicate all spans on the card.
 * The time slot is 1-31 for a specific time slot in each span and 0xff to indicate all time slots
 * of each span.
 *
 * If the channel indicates on the initial CH_INFO_REQ that the channel is STYLE2 and in the
 * unattached state, then the style of the CD device is STYLE2.  Otherwise, the CD device is STYLE1
 * and this primitive is not supported.  This primitive is only valid in the CD_UNATTACHED state.
 *
 * The CD_ATTACH_REQ is passed to the CH provider as a CH_ATTACH_REQ with the PPA as the address.
 * The PPA is not checked here but is checked by the CH provider.  When the CH_OK_ACK or
 * CH_ERROR_ACK is received, it is passed to the CD user as a CD_OK_ACK or CD_ERROR_ACK primitive.
 */
STATIC noinline __unlikely int
cd_attach_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_attach_req_t *p = (typeof(p)) mp->b_rptr;
	int err, class;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (cd->info.cd_ppa_style != CD_STYLE2)
		goto enotsupp;
	if (cd->info.cd_state != CD_UNATTACHED)
		goto outstate;
	cd->ppa = p->cd_ppa;
	cd->info.cd_state = CD_ATTACH_PENDING;
	return ch_attach_req(cd->ch, q, mp, 0, sizeof(cd->ppa), (caddr_t) &cd->ppa);
      emsgsize:
	err = CD_PROTOSHORT;
	goto error;
      enotsupp:
	err = CD_NOTSUPP;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      error:
	return cd_error_ack(cd, q, mp, CD_ATTACH_REQ, err);
}

/**
 * cd_detach_req: process CD_DETACH_REQ primitive
 * @cd: private structure
 * @q: active queue (write queue)
 * @mp: the CD_DETACH_REQ primitive
 */
STATIC noinline __unlikely int
cd_detach_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_detach_req_t *p = (typeof(p)) mp->b_rptr;
	int err

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (cd->info.cd_ppa_style != CD_STYLE2)
		goto enotsupp;
	if (cd->info.cd_state != CD_DISABLED)
		goto outstate;
	cd->info.cd_state = CD_DETACH_PENDING;
	return ch_detach_req(cd->ch, q, mp);
      outstate:
	err = CD_OUTSTATE;
	goto error;
      enotsupp:
	err = CD_OUTSTATE;
	goto error;
      emsgsize:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_ack(cd, q, mp, CD_DETACH_REQ, err);
}

/**
 * cd_enable_req: process CD_ENABLE_REQ primitive
 * @cd: private structure
 * @q: active queue (write queue)
 * @mp: the CD_ENABLE_REQ primitive
 */
STATIC noinline __unlikely int
cd_enable_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_enable_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (cd->info.cd_state != CD_DISABLED)
		goto outstate;
	if (p->cd_dial_type != CD_NODIAL)
		goto baddialtype;
	if (p->cd_dial_length != 0)
		goto baddial;
	cd->info.cd_state = CD_ENABLE_PENDING;
	return ch_enable_req(cd->ch, q, mp);
      baddial:
	err = CD_BADDIAL;
	goto error;
      baddialtype:
	err = CD_BADDIALTYPE;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      emsgsize:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_ack(cd, q, mp, CD_DETACH_REQ, err);
}

/**
 * cd_disable_req: process CD_DISABLE_REQ primitive
 * @cd: private structure
 * @q: active queue (write queue)
 * @mp: the CD_DISABLE_REQ primitive
 */
STATIC noinline __unlikely int
cd_disable_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_disable_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (cd->info.cd_state != CD_ENABLED && cd->info.cd_state != CD_INPUT_ALLOWED)
		goto outstate;
	cd->info.cd_state = CD_DISABLE_PENDING;
	return ch_disable_req(cd->ch, q, mp);
      outstate:
	err = CD_OUTSTATE;
	goto error;
      emsgsize:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_ack(cd, q, mp, CD_DETACH_REQ, err);
}

/**
 * cd_allow_input_req: process CD_ALLOW_INPUT_REQ primitive
 * @cd: private structure
 * @q: active queue (write queue)
 * @mp: the CD_ALLOW_INPUT_REQ primitive
 */
STATIC noinline __unlikely int
cd_allow_input_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_allow_input_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	if (cd->info.cd_state != CD_ENABLED && cd->info.cd_state != CD_INPUT_ALLOWED)
		goto outstate;
	return cd_ok_ack(cd, q, mp, CD_INPUT_ALLOWED, CD_ALLOW_INPUT_REQ);
      outstate:
	err = CD_OUTSTATE;
	goto error;
      emsgsize:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_ack(cd, q, mp, CD_DETACH_REQ, err);
}

/**
 * cd_read_req: process CD_READ_REQ primitive
 * @cd: private structure
 * @q: active queue (write queue)
 * @mp: the CD_READ_REQ primitive
 */
STATIC noinline __unlikely int
cd_read_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_read_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	err = CD_NOTSUPP;
	goto error;
      emsgsize:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_ack(cd, q, mp, CD_DETACH_REQ, err);
}

/**
 * cd_unitdata_req: process CD_UNITDATA_REQ primitive
 * @cd: private structure
 * @q: active queue (write queue)
 * @mp: the CD_UNITDATA_REQ primitive
 */
STATIC noinline __unlikely int
cd_unitdata_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;
	size_t dlen = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (cd->info.ch_state) {
	case CD_ENABLED:
	case CD_INPUT_ALLOWED:
	case CD_OUTPUT_ACTIVE:
	case CD_READ_ACTIVE:
		break;
	default:
		goto outstate;
	}
	switch (p->cd_addr_type) {
	case CD_IMPLICIT:
		break;
	case CD_SPECIFIC:
	case CD_BROADCAST:
	default:
		goto badaddrtype;
	}
	/* check message sizes */
	if (!(mp->b_flag & 0x80)) {
		if (mp->b_cont)
			dlen = msgdsize(mp->b_cont);
		if (cd->info.cd_max_sdu < dlen || dlen < cd->info.cd_min_sdu)
			goto baddata;
		if ((err = cd_transmit_data(cd, q, mp->b_cont)))
			return (err);
		mp->b_cont = NULL;
		mp->b_flag |= 0x80;	/* to avoid retransmission if ack fails */
	}
	/* yes we do acked output for unitdata requests */
	return cd_unitdata_ack(cd, q, mp);
      baddata:
	err = CD_WRITEFAIL;
	goto error;
      badaddrtype:
	err = CD_BADADDRTYPE;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      emsgsize:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_ack(cd, q, mp, CD_DETACH_REQ, err);
}

/**
 * cd_write_read_req: process CD_WRITE_READ_REQ primitive
 * @cd: private structure
 * @q: active queue (write queue)
 * @mp: the CD_WRITE_READ_REQ primitive
 */
STATIC noinline __unlikely int
cd_write_read_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_write_read_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	err = CD_NOTSUPP;
	goto error;
      emsgsize:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_ack(cd, q, mp, CD_DETACH_REQ, err);
}

/**
 * cd_halt_input_req: process CD_HALT_INPUT_REQ primitive
 * @cd: private structure
 * @q: active queue (write queue)
 * @mp: the CD_HALT_INPUT_REQ primitive
 */
STATIC noinline __unlikely int
cd_halt_input_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_halt_input_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (cd->info.cd_state) {
	case CD_ENABLED:
	case CD_INPUT_ALLOWED:
	case CD_READ_ACTIVE:
		break;
	default:
		goto outstate;
	}
	switch (p->cd_disposal) {
	case CD_FLUSH:
	{
		mblk_t *fp;

		/* What we are supposed to do in this case is flush all input data that has not yet
		   been delivered.  Use an M_FLUSH message for that.  It does not matter if we flush
		   the read side multiple times. */
		if (!(fp = ch_allocb(q, 2, BPRI_MED)))
			return (-ENOBUFS);
		fp->b_datap->db_type = M_FLUSH;
		*fp->b_wptr++ = FLUSHR;
		qreply(q, fp);
		break;
	}
	case CD_WAIT:
	case CD_DELIVER:
		/* What we should do in this case is wait for all data in queue to be delivered
		   before sending the ack primitive (which is M_PCPROTO).  This could possibly be
		   accomplished by sending an M_IOCTL message upstream and waiting for an M_IOCNAK.
		   This works against the Stream head and hopefull upstream multiplexers could be
		   coded to do the same.  For now we just skip it.  Some data may arrive after the
		   ack. */
		break;
	default:
		goto baddisposal;
	}
	return cd_ok_ack(cd, q, mp, CD_ENABLED, CD_HALT_INPUT_REQ);
      baddisposal:
	err = CD_BADDISPOSAL;
	goto error;
      outstate:
	err = CD_OUTSTATE;
	goto error;
      emsgsize:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_ack(cd, q, mp, CD_DETACH_REQ, err);
}

/**
 * cd_abort_output_req: process CD_ABORT_OUTPUT_REQ primitive
 * @cd: private structure
 * @q: active queue (write queue)
 * @mp: the CD_ABORT_OUTPUT_REQ primitive
 */
STATIC noinline __unlikely int
cd_abort_output_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_abort_output_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	switch (cd->info.cd_state) {
	case CD_ENABLED:
		cd_abort_output(cd);
		return cd_ok_ack(cd, q, mp, CD_ENABLED, CD_ABORT_OUTPUT_REQ);
	case CD_INPUT_ALLOWED:
	case CD_OUTPUT_ACTIVE:
		cd_abort_output(cd);
		return cd_ok_ack(cd, q, mp, CD_INPUT_ALLOWED, CD_ABORT_OUTPUT_REQ);
	default:
		goto outstate;
	}
      outstate:
	err = CD_OUTSTATE;
	goto error;
      emsgsize:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_ack(cd, q, mp, CD_DETACH_REQ, err);
}

/**
 * cd_mux_name_req: process CD_MUX_NAME_REQ primitive
 * @cd: private structure
 * @q: active queue (write queue)
 * @mp: the CD_MUX_NAME_REQ primitive
 */
STATIC noinline __unlikely int
cd_mux_name_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_mux_name_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	err = CD_NOTSUPP;
	goto error;
      emsgsize:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_ack(cd, q, mp, CD_DETACH_REQ, err);
}

/**
 * cd_modem_sig_req: process CD_MODEM_SIG_REQ primitive
 * @cd: private structure
 * @q: active queue (write queue)
 * @mp: the CD_MODEM_SIG_REQ primitive
 */
STATIC noinline __unlikely int
cd_modem_sig_req(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_modem_sig_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	err = CD_NOTSUPP;
	goto error;
      emsgsize:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_ack(cd, q, mp, CD_DETACH_REQ, err);
}

/**
 * cd_modem_sig_poll: process CD_MODEM_SIG_POLL primitive
 * @cd: private structure
 * @q: active queue (write queue)
 * @mp: the CD_MODEM_SIG_POLL primitive
 */
STATIC noinline __unlikely int
cd_modem_sig_poll(struct cd *cd, queue_t *q, mblk_t *mp)
{
	cd_modem_sig_poll_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto emsgsize;
	err = CD_NOTSUPP;
	goto error;
      emsgsize:
	err = CD_PROTOSHORT;
	goto error;
      error:
	return cd_error_ack(cd, q, mp, CD_DETACH_REQ, err);
}

/**
 * cd_w_proto: process a CD primitive
 * @cd: private structure
 * @q: active queue (write queue)
 * @mp: the CD primitive
 */
STATIC noinline __unlikely int
cd_w_proto(struct cd *cd, queue_t *q, mblk_t *mp)
{
	int err = 0;

	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(cd_ulong))) {
		fixme(("return an error primitive"));
		freemsg(msg);
	} else {
		cd_ulong oldstate = cd->info.cd_state;

		switch (*(cd_ulong *) mp->b_rptr) {
		case CD_INFO_REQ:
			err = cd_info_req(cd, q, mp);
			break;
		case CD_ATTACH_REQ:
			err = cd_attach_req(cd, q, mp);
			break;
		case CD_DETACH_REQ:
			err = cd_detach_req(cd, q, mp);
			break;
		case CD_ENABLE_REQ:
			err = cd_enable_req(cd, q, mp);
			break;
		case CD_DISABLE_REQ:
			err = cd_disable_req(cd, q, mp);
			break;
		case CD_ALLOW_INPUT_REQ:
			err = cd_allow_input_req(cd, q, mp);
			break;
		case CD_READ_REQ:
			err = cd_read_req(cd, q, mp);
			break;
		case CD_UNITDATA_REQ:
			err = cd_unitdata_req(cd, q, mp);
			break;
		case CD_WRITE_READ_REQ:
			err = cd_write_read_req(cd, q, mp);
			break;
		case CD_HALT_INPUT_REQ:
			err = cd_halt_input_req(cd, q, mp);
			break;
		case CD_ABORT_OUTPUT_REQ:
			err = cd_abort_output_req(cd, q, mp);
			break;
		case CD_MUX_NAME_REQ:
			err = cd_mux_name_req(cd, q, mp);
			break;
		case CD_MODEM_SIG_REQ:
			err = cd_modem_sig_req(cd, q, mp);
			break;
		case CD_MODEM_SIG_POLL:
			err = cd_modem_sig_poll(cd, q, mp);
			break;
		default:
			fixme(("return an error primitive"));
			freemsg(msg);
			break;
		}
		if (err)
			cd->info.cd_state = oldstate;
	}
	return (err);
}

/*
 *  CH Primitives received from CH provider below.
 *  -----------------------------------------------
 */

/**
 * ch_info_ack: process CH_INFO_ACK primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_INFO_ACK primitive
 */
STATIC noinline __unlikely int
ch_info_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_info_ack *p = (typeof(p)) mp->b_rptr;
	struct cd *cd = ch->cd;

	ch->info.ch_prov_flags = p->ch_prov_flags;
	ch->info.ch_style = p->ch_style;
	ch->info.ch_version = p->ch_version;
	ch->info.ch_state = p->ch_state;

	if (p->ch_addr_length <= sizeof(ch->addr)) {
		bcopy(mp->b_rptr + p->ch_addr_offset, &ch->addr, p->ch_addr_length);
		ch->info.ch_addr_length = p->ch_addr_length;
		ch->info.ch_addr_offset = sizeof(ch->info);
	}
	if (p->ch_parm_length == sizeof(ch->parms)
	    && *(ch_ulong *) (mp->b_rptr + p->ch_parm_offset) == ch->parms.cp_type) {
		bcopy(mp->b_rptr + p->ch_parm_offset, ch->parms, sizeof(ch->parms));
		ch->info.ch_parm_length = p->ch_parm_length;
		ch->info.ch_parm_offset = sizeof(ch->info) + ch->info.ch_addr_length;
	}

	if (cd->info.cd_primitive == 0) {
		ch->info.ch_primitive = CH_INFO_ACK;
		/* First CH_INFO_ACK: the first CH_INFO_ACK gives us information about the CH
		   stream over which we have been pushed.  If the CH Stream is a style II Stream
		   and the Stream has not yet been attached, the parameters information (above)
		   will be unknown.  In that case, we must issue another information request after
		   the CH Stream has been attached. */

		switch (ch->info.ch_state) {
		case CHS_DETACHED:
		case CHS_ENABLED:
			cd->info.cd_ppa_style = p->ch_style;
			break;
		case CHS_ATTACHED:
			cd->info.cd_ppa_style = CD_STYLE1;
			break;
		}
	}
	{
		/* Subsequent CH_INFO_ACK: a subsequent request is necessary immediately after an
		   attach if the Stream was not attached during the last request.  Or, at any time
		   that we need to synchronize state or other parameters, a subsequent request can
		   be issued. */
		switch (ch->info.ch_state) {
		case CHS_DETACHED:
			cd->info.cd_state = CD_UNATTACHED;
			break;
		case CHS_ATTACHED:
			cd->info.cd_state = CD_DISABLED;
			/* addr and parms should be valid */
			break;
		case CHS_ENABLED:
			cd->info.cd_state = CD_ENABLED;
			/* addr and parms should be valid */
			break;
		case CHS_CONNECTED:
			cd->info.cd_state = CD_INPUT_ALLOWED;
			/* addr and parms should be valid */
			break;
		default:
			cd->info.cd_state = CD_UNUSABLE;
			break;
		}
		switch (ch->info.ch_state) {
		case CHS_ATTACHED:
		case CHS_ENABLED:
			/* addr and parms should be valid */
			if (ch->parms.cp_type == CH_PARMS_CIRCUIT) {
				switch (cd->info.cd_class) {
				case CD_HDLC:
					cd->info.cd_max_sdu = 256;
					cd->info.cd_min_sdu = 1;
					break;
				case CD_DAED:
					if (ch->parms.cp_rate >= 8000) {
						/* talking high speed links, 12-bit SNs, 9-bit LI */
						cd->info.cd_max_sdu = 6 + 1 + 272;
						cd->info.cd_min_sdu = 6;
					} else {
						/* normal links, 7 bit SNs, 6 bit LI */
						cd->info.cd_max_sdu = 3 + 1 + 272;
						cd->info.cd_min_sdu = 3;
					}
					break;
				case CD_NODEV:
					/* These do not really matter.  One cannot transmit on an
					   xray Stream. */
					cd->info.cd_max_sdu = 0;
					cd->info.cd_min_sdu = 0;
					break;
				}
			}
			break;
		case CHS_DETACHED:
			/* addr and parms are not valid in this state */
			break;
		}
	}
	freemsg(mp);
	return (0);
}

/**
 * ch_optmgmt_ack: process CH_OPTMGMT_ACK primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_OPTMGMT_ACK primitive
 */
STATIC noinline __unlikely int
ch_optmgmt_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_optmgmt_ack *p = (typeof(p)) mp->b_rptr;
}

/**
 * ch_ok_ack: process CH_OK_ACK primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_OK_ACK primitive
 */
STATIC noinline __unlikely int
ch_ok_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_ok_ack *p = (typeof(p)) mp->b_rptr;
	int err;

	switch (p->ch_correct_primitive) {
	case CH_ATTACH_REQ:
		/* issue secondary CH_INFO_REQ once attached */
		if ((err = ch_info_req(ch, q, NULL)))
			break;
		ch->info.ch_state = CHS_ATTACHED;
		return cd_ok_ack(ch->cd, q, mp, CD_DISABLED, CD_ATTACH_REQ);
	case CH_DETACH_REQ:
		ch->info.ch_state = CHS_DETACHED;
		return cd_ok_ack(ch->cd, q, mp, CD_UNATTACHED, CD_DETACH_REQ);
	case CH_ENABLE_REQ:
		ch->info.ch_state = CHS_WCON_EREQ;
		freemsg(mp);
		return (0);
	case CH_DISABLE_REQ:
		ch->info.ch_state = CHS_WCON_RREQ;
		freemsg(mp);
		return (0);
	default:
		swerr();
		freemsg(mp);
		return (0);
	}
}

/**
 * ch_error_ack: process CH_ERROR_ACK primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_ERROR_ACK primitive
 */
STATIC noinline __unlikely int
ch_error_ack(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_error_ack *p = (typeof(p)) mp->b_rptr;
	struct cd *cd = ch->cd;
	int error, cause;

	error = p->ch_errno;
	cause = p->ch_explanation;

	fixme(("Have to map CH errors to CD errors."));

	switch (p->ch_error_prmitive) {
	case CH_ATTACH_REQ:
		ch->info.ch_state = CHS_DETACHED;
		cd->info.cd_state = CD_UNATTACHED;
		return cd_error_ack(cd, q, mp, CD_ATTACH_REQ, error);
	case CH_DETACH_REQ:
		ch->info.ch_state = CHS_ATTACHED;
		cd->info.cd_state = CD_DISABLED;
		return cd_error_ack(cd, q, mp, CD_DETACH_REQ, error);
	case CH_ENABLE_REQ:
		ch->info.ch_state = CHS_ATTACHED;
		cd->info.cd_state = CD_DISABLED;
		return cd_error_ack(cd, q, mp, CD_ENABLE_REQ, error);
	case CH_DISABLE_REQ:
		ch->info.ch_state = CHS_ENABLED;
		cd->info.cd_state = CD_ENABLED;
		return cd_error_ack(cd, q, mp, CD_DISABLE_REQ, error);
	default:
		swerr();
		freemsg(mp);
		return (0);
	}
}

/**
 * ch_enable_con: process CH_ENABLE_CON primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_ENABLE_CON primitive
 */
STATIC noinline __unlikely int
ch_enable_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch->info.state = CH_ENABLED;
	return cd_enable_con(ch->cd, q, mp);
}

/**
 * ch_connect_con: process CH_CONNECT_CON primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_CONNECT_CON primitive
 */
STATIC noinline __unlikely int
ch_connect_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * ch_data_ind: process CH_DATA_IND primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_DATA_IND primitive
 */
STATIC noinline __unlikely int
ch_data_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_data_ind *p = (typeof(p)) mp->b_rptr;
}

/**
 * ch_disconnect_ind: process CH_DISCONNECT_IND primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_DISCONNECT_IND primitive
 */
STATIC noinline __unlikely int
ch_disconnect_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * ch_disconnect_con: process CH_DISCONNECT_CON primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_DISCONNECT_CON primitive
 */
STATIC noinline __unlikely int
ch_disconnect_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * ch_disable_ind: process CH_DISABLE_IND primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_DISABLE_IND primitive
 */
STATIC noinline __unlikely int
ch_disable_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * ch_disable_con: process CH_DISABLE_CON primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_DISABLE_CON primitive
 */
STATIC noinline __unlikely int
ch_disable_con(struct ch *ch, queue_t *q, mblk_t *mp)
{
	ch->info.state = CH_DISABLED;
	return cd_disable_con(ch->cd, q, mp);
}

/**
 * ch_event_ind: process CH_EVENT_IND primitive
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the CH_EVENT_IND primitive
 */
STATIC noinline __unlikely int
ch_event_ind(struct ch *ch, queue_t *q, mblk_t *mp)
{
	return cd_error_ind(ch->cd, q, mp, CD_EVENT, CD_DEVERR + p->cd_errno);
}

/**
 * ch_r_proto: process a CH primitive
 * @ch: private structure
 * @q: active queue (write queue)
 * @mp: the CD primitive
 */
STATIC noinline __unlikely int
ch_r_proto(struct ch *ch, queue_t *q, mblk_t *mp)
{
	int err = 0;

	if (mp->b_wptr < mp->b_rptr + sizeof(ch_ulong)) {
		fixme(("return an error primitive"));
		freemsg(msg);
	} else {
		ch_ulong ch_oldstate = ch->info.ch_state;
		cd_ulong cd_oldstate = ch->cd->info.cd_state;

		switch (*(ch_ulong *) mp->b_rptr) {
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
		case CH_EVENT_IND:
			err = ch_event_ind(ch, q, mp);
			break;
		default:
			fixme(("return an error primitive"));
			freemsg(msg);
			break;
		}
		if (err) {
			ch->info.ch_state = ch_oldstate;
			ch->cd->info.cd_state = cd_oldstate;
		}
	}
	return (err);
}

/*
 *  PUT and SERVICE procedures.
 *  ===========================
 */

/*
 *  Write side message handling.
 */
STATIC noinline fastcall int
cd_w_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_PROTO:
	case M_PCPROTO:
		return cd_w_proto(CD_PRIV(q), q, mp);
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
		putnext(q, mp);
		return (0);
	default:
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

/**
 * cd_w_msg: process message from write queue
 * @cd: private communications device pointer
 * @q: write queue
 * @mp: message to process
 *
 * The put procedure marks every message with MSGDELIM on the way in.  The mark is cleared when we
 * begin processing the message.  If the message is not marked upon entry into this procedure, we
 * have already performed beginning-of-frame processing for the message.
 */
STATIC inline fastcall int
cd_w_msg(struct cd *cd, queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_DATA) {
		int err;

		/* transmit cycle - generate multiple transmit blocks per call until linked message 
		   consumed or flow control encountered.  */
		while (!(err = cd_tx_process(cd, q, mp))) ;
		/* kick receivers */
		qenable(RD(q));
		return (err);
	}
	return cd_w_msg_slow(q, mp);
}

/**
 * cd_wput: write put procedure
 * @q: write queue to which to put message
 * @mp: message to put
 *
 * Basic optimized queue pair serializing queueing put procedure, that marks all messages.
 */
STATIC streamscall __hot_out int
cd_wput(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	bool locked;

	if (unlikely(datamsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || unlikely(!(locked = test_and_set_bit(0, &cd->flags)))
	    || cd_w_msg(cd, w, wp))
		if (putq(q, mp)) {
			swerr();
			freemsg(mp);
		}
	if (locked)
		clear_bit(0, &cd->flags);
	return (0);
}

/**
 * cd_wsrv: write service procedure
 * @q: write queue to service
 *
 * Basic optimized queue pair serializing draining service procedure.
 */
STATIC streamscall __hot_out int
cd_wsrv(queue_t *q)
{
	struct cd *cd = CD_PRIV(q);
	mblk_t *mp;

	if (likely(test_and_set_bit(0, &cd->flags))) {
		if (likely(!!(mp = getq(q))))
			do {
				if (cd_w_msg(cd, q, mp)) {
					putbq(q, mp);
					break;
				}
			} while (unlikely(!!(mp = getq(q))));
		clear_bit(0, &cd->flags);
	} else
		qenable(q);	/* come back again later */
	return (0);
}

/*
 *  Read side message handling.
 */
STATIC noinline fastcall int
ch_r_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_PROTO:
	case M_PCPROTO:
		return ch_r_proto(CH_PRIV(q), q, mp);
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHBAND);
		}
		putnext(q, mp);
		return (0);
	default:
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

STATIC inline fastcall __hot_in int
ch_rx_linkb(struct ch *ch, mblk_t *mp)
{
	int err;
	pl_t pl;

	/* M_DATA messages are fed to the receive procedure one at a time via the ch->rx.nxt
	   pointer.  It is necessary to lock the ch structure when attaching messages to this
	   pointer, or when taking them off to avoid races. */
	spin_lock_irqsave(&ch->lock, pl);
	{
		if (ch->rx.msg == NULL) {
			ch->rx.msg = mp;
			err = 0;
		} else {
			err = (-EBUSY);
		}
	}
	spin_unlock_irqrestore(&ch->lock, pl);
	return (err);
}

/**
 * ch_r_msg: process message from read queue
 * @ch: private channel pointer
 * @q: read queue
 * @mp: message to process
 *
 * This needs to be fast, but should not be inline because I do not want it inlined twice (once for
 * cd_rput and once for cd_rsrv.  Keep it closer to cd_rput.
 */
STATIC noinline fastcall __hot_in int
ch_r_msg(struct ch *ch, queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA)) {
		int err;
		/* link message block to receiver */
		if ((err = ch_rx_linkb(ch, mp)))
			return (err);
		/* receive cycle - absorbs mp if no error */
		if ((err = ch_rx_process(ch, q, mp)))
			return (err);
		/* kick transmitters */
		qenable(WR(q));
		return (0);
	}
	return ch_r_msg_slow(q, mp);
}

/**
 * cd_rput: read put procedure
 * @q: read queue to which to put message
 * @mp: message to put
 *
 * Basic optimized queue pair serializing queueing put procedure.
 */
STATIC streamscall __hot_in int
cd_rput(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	bool locked;

	if (unlikely(datamsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY)))
	    || unlikely(!(locked = test_and_set_bit(0, &cd->flags)))
	    || ch_r_msg(cd->ch, q, mp))
		if (putq(q, mp)) {
			swerr();
			freemsg(mp);
		}
	if (locked)
		clear_bit(0, &cd->flags);
	return (0);
}

/**
 * cd_rsrv: read service procedure
 * @q: read queue to service
 *
 * Basic optimized queue pair serializing draining service procedure.
 */
STATIC streamscall __hot_in int
cd_rsrv(queue_t *q)
{
	mblk_t *mp;

	if (likely(test_and_set_bit(0, &cd->flags))) {
		if (likely(!!(mp = getq(q))))
			do {
				if (ch_r_msg(cd->ch, q, mp)) {
					putbq(q, mp);
					break;
				}
			} while (unlikely(!!(mp = getq(q))));
		clear_bit(0, &cd->flags);
	} else
		qenable(q);	/* come back again later */
	return (0);
}

/*
 *  OPEN and CLOSE procedures.
 *  ==========================
 */

/**
 * cd_qopen: open a Communications Device (CD) stream
 * @q: read queue
 * @devp: device pointer (ignored)
 * @oflags: open flags (ignored)
 * @sflag: always MODOPEN
 * @crp: pointer to credentials of opening Stream
 */
STATIC streamscall __unlikely int
cd_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct priv *p;
	mblk_t *mp;

	if (q->q_ptr != NULL)
		return (0);	/* already open */

	dassert(sflag == MODOPEN);
	dassert(WR(q)->q_next != NULL);

	if (!(mp = allocb(sizeof(ch_ulong), BPRI_WAITOK)))
		return (ENOSR);
	mp->b_datap->db_type = M_PCPROTO;
	*(ch_ulong *) mp->b_wptr = CH_INFO_REQ;
	mp->b_wptr += sizeof(ch_ulong);

	if (!(p = kmem_alloc(sizeof(*p), KM_SLEEP))) {
		freeb(mp);
		return (ENXIO);
	}
	bzero(p, sizeof(*p));

	/* point halves to each other */
	p->cd.ch = &p->ch;
	p->ch.cd = &p->cd;

	p->cd.info.cd_primitive = CD_INFO_ACK;
	p->cd.info.cd_state = CD_UNUSABLE;	/* until channel state reported */
	p->cd.info.cd_max_sdu = 256;	/* until speed reported */
	p->cd.info.cd_min_sdu = 0;	/* until speed reported */
	if (q->q_qinfo->qi_minfo == &hdlc_minfo)
		p->cd.info.cd_class = CD_HDLC;
	if (q->q_qinfo->qi_minfo == &daed_minfo)
		p->cd.info.cd_class = CD_DAED;
	if (q->q_qinfo->qi_minfo == &xray_minfo)
		p->cd.info.cd_class = CD_NODEV;
	p->cd.info.cd_duplex = CD_FULLDUPLEX;	/* always */
	p->cd.info.cd_output_style = CD_PACEDOUTPUT;	/* always */
	p->cd.info.cd_features = CD_CANREAD | CD_AUTOALLOW;	/* always */
	p->cd.info.cd_addr_length = 0;	/* always */
	p->cd.info.cd_ppa_style = CD_STYLE2;	/* until channel type reported */

	spin_lock(&cd_lock);
	if ((p->next = cd_opens))
		p->next->prev = &p->next;
	p->prev = &cd_opens;
	cd_opens = p;
	spin_unlock(&cd_lock);

	qprocson(q);
	putnext(WR(q), mp);	/* issue immediate CH_INFO_REQ */
	return (0);
}

STATIC streamscall __unlikely int
cd_qclose(queue_t *q, dev_t *devp, int oflags)
{
	struct priv *p = PRIV(q);

	qprocsoff(q);

	unbufcall(p->rbid);
	unbufcall(p->wbid);

	q->q_ptr = WR(q)->q_ptr = NULL;
	kmem_free(p, sizeof(*p));
	return (0);
}

/*
 *  STREAMS Definitions.
 *  ====================
 */

STATIC struct qinit hdlc_winit = {
	.qi_putp = cd_wput,
	.qi_srvp = cd_wsrv,
	.qi_minfo = &hdlc_minfo,
	.qi_mstat = &cd_wstat,
};

STATIC struct qinit hdlc_rinit = {
	.qi_putp = cd_rput,
	.qi_srvp = cd_rsrv,
	.qi_qopen = cd_qopen,
	.qi_qclose = cd_qclose,
	.qi_minfo = &hdlc_minfo,
	.qi_mstat = &cd_rstat,
};

STATIC struct streamtab hdlcinfo = {
	.st_rdinit = &hdlc_rinit,
	.st_wrinit = &hdlc_winit,
};

STATIC struct qinit daed_winit = {
	.qi_putp = cd_wput,
	.qi_srvp = cd_wsrv,
	.qi_minfo = &daed_minfo,
	.qi_mstat = &cd_wstat,
};

STATIC struct qinit daed_rinit = {
	.qi_putp = cd_rput,
	.qi_srvp = cd_rsrv,
	.qi_qopen = cd_qopen,
	.qi_qclose = cd_qclose,
	.qi_minfo = &daed_minfo,
	.qi_mstat = &cd_rstat,
};

STATIC struct streamtab daedinfo = {
	.st_rdinit = &daed_rinit,
	.st_wrinit = &daed_winit,
};

STATIC struct qinit xray_winit = {
	.qi_putp = cd_wput,
	.qi_srvp = cd_wsrv,
	.qi_minfo = &xray_minfo,
	.qi_mstat = &cd_wstat,
};

STATIC struct qinit xray_rinit = {
	.qi_putp = cd_rput,
	.qi_srvp = cd_rsrv,
	.qi_qopen = cd_qopen,
	.qi_qclose = cd_qclose,
	.qi_minfo = &xray_minfo,
	.qi_mstat = &cd_rstat,
};

STATIC struct streamtab xrayinfo = {
	.st_rdinit = &xray_rinit,
	.st_wrinit = &xray_winit,
};

/*
 *  STREAMS Registration.
 *  =====================
 */
#ifdef LIS
#define fmodsw _fmodsw
#endif

STATIC struct fmodsw hdlc_fmod = {
	.f_name = HDLC_MOD_NAME,
	.f_str = &hdlcinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

STATIC struct fmodsw daed_fmod = {
	.f_name = DAED_MOD_NAME,
	.f_str = &daedinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

STATIC struct fmodsw xray_fmod = {
	.f_name = XRAY_MOD_NAME,
	.f_str = &xrayinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

STATIC __unlikely int
cd_unregister_strmod(void)
{
	int rtn, err = 0;

	if ((rtn = unregister_strmod(&xray_fmod)) < 0)
		err = err ? : rtn;
	if ((rtn = unregister_strmod(&daed_fmod)) < 0)
		err = err ? : rtn;
	if ((rtn = unregister_strmod(&hdlc_fmod)) < 0)
		err = err ? : rtn;

	return (err);
}
STATIC __unlikely int
cd_register_strmod(void)
{
	int err;

	if ((err = register_strmod(&hdlc_fmod)) < 0)
		return (err);
	if ((err = register_strmod(&daed_fmod)) < 0)
		return (err);
	if ((err = register_strmod(&xray_fmod)) < 0)
		return (err);
	return (0);
}

/*
 *  Linux Registration.
 *  ===================
 */

modID_t modid = MOD_ID;

#ifdef module_param
module_param(modid, ushort, 0);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for the CD module. (0 for allocation.)");

MODULE_STATIC void
cdterminate(void)
{
	int err;

	if ((err = cd_unregister_strmod()))
		cmn_err(CE_WARN, "%s: could not unregister module", MOD_NAME);
	return;
}

MODULE_STATIC int
cdinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);	/* console splash */
	if ((err = cd_register_strmod())) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", MOD_NAME, err);
		cd_unregister_strmod();
		return (err);
	}
	return (0);
}

module_init(cdinit);
module_exit(cdterminate);
