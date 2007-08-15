/*****************************************************************************

 @(#) $RCSfile: ch.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/08/15 05:32:54 $

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

 Last Modified $Date: 2007/08/15 05:32:54 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ch.c,v $
 Revision 0.9.2.6  2007/08/15 05:32:54  brian
 - GPLv3 updates

 Revision 0.9.2.5  2007/08/14 06:47:28  brian
 - GPLv3 header update

 Revision 0.9.2.4  2007/07/21 20:43:47  brian
 - added manual pages, corrections

 Revision 0.9.2.3  2007/07/14 01:35:32  brian
 - make license explicit, add documentation

 Revision 0.9.2.2  2007/03/25 19:00:51  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.1  2006/12/20 23:07:07  brian
 - new working files

 *****************************************************************************/

#ident "@(#) $RCSfile: ch.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/08/15 05:32:54 $"

static char const ident[] =
    "$RCSfile: ch.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/08/15 05:32:54 $";

/*
 * This is a CH multiplexing driver.  MX stream are linked beneath the lower multiplex and CH
 * streams are opened at the upper multiplex.  This driver provides a unified approach to opening CH
 * streams and allows device drivers to implement only the MX interface yet support the CH interface
 * via this driver.  Modules are then pushed on CH streams to provide HDLC or SS7 and LAPB, LAPF,
 * LAPD or LAPM procedures.
 */

#ifndef HAVE_KTYPE_BOOL
#include <stdbool.h>
#endif

#define _DEBUG 1
// #undef _DEBUG

#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#include <sys/os7/compat.h>

#include <sys/chi.h>
#include <sys/chi_ioctl.h>
#include <sys/mxi.h>
#include <sys/mxi_ioctl.h>

#define CHMUX_DESCRIP	"CH (Channel) STREAMS MULTIPLEXING DRIVER."
#define CHMUX_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define CHMUX_REVISION	"OpenSS7 $RCSfile: ch.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/08/15 05:32:54 $"
#define CHMUX_COPYRIGHT	"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define CHMUX_DEVICE	"Supports MX devices."
#define CHMUX_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define CHMUX_LICENSE	"GPL"
#define CHMUX_BANNER	CHMUX_DESCRIP	"\n" \
			CHMUX_EXTRA	"\n" \
			CHMUX_REVISION	"\n" \
			CHMUX_COPYRIGHT	"\n" \
			CHMUX_DEVICE	"\n" \
			CHMUX_CONTACT	"\n"
#define CHMUX_SPLASH	CHMUX_DESCRIP	" - " \
			CHMUX_REVISION

#ifdef LINUX
MODULE_AUTHOR(CHMUX_CONTACT);
MODULE_DESCRIPTION(CHMUX_DESCRIP);
MODULE_SUPPORTED_DEVICE(CHMUX_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(CHMUX_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-ch");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define CHMUX_DRV_ID		CONFIG_STREAMS_CHMUX_MODID
#define CHMUX_DRV_NAME		CONFIG_STREAMS_CHMUX_NAME
#define CHMUX_CMAJORS		CONFIG_STREAMS_CHMUX_NMAJORS
#define CHMUX_CMAJOR_0		CONFIG_STREAMS_CHMUX_MAJOR
#define CHMUX_UNITS		CONFIG_STREAMS_CHMUX_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_MXMUX_MODID));
MODULE_ALIAS("streams-driver-ch");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_MXMUX_MAJOR));
MODULE_ALIAS("/dev/streams/ch");
MODULE_ALIAS("/dev/streams/ch/*");
MODULE_ALIAS("/dev/streams/clone/ch");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(CHMUX_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(CHMUX_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(CHMUX_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/ch");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  Private structures.
 */
struct ch;
struct mx;

struct ch {
	queue_t *oq;
	queue_t *iq;
	ch_ulong i_state;		/* interface state */
	uint32_t ppa;			/* phyical point of appearance */
	struct CH_info_ack info;	/* information */
	struct CH_parms_circuit parm;	/* parameters */
	struct ch_config config;	/* configuration */
	struct ch_stats stats;		/* statistics */
	struct ch_statem statem;	/* state machine */
	struct ch_notify notify;	/* event notification */

	int span;			/* first span */
	int spans;			/* number of spans in span mask */
	int channels;			/* number of channels in channel mask */

	struct mx *mx;
	uint32_t smask;			/* mask of spans */
	uint32_t mask;			/* mask of channels */
	uint32_t masks[8];		/* per-span mask of channels */
	struct {
		struct ch *next;
		struct ch **prev;
	} a;				/* attached ch linkage */
	struct {
		struct ch *next;
		struct ch **prev;
	} e;				/* enabled ch linkage */
	struct {
		struct ch *next;
		struct ch **prev;
	} c;				/* connected ch linkage */
};

struct mx {
	queue_t *oq;
	queue_t *iq;
	mx_ulong i_state;		/* interface state */
	uint32_t ppa;			/* phyical point of appearance */
	struct MX_info_ack info;	/* information */
	struct MX_parms_circuit parm;	/* parameters */
	struct mx_config config;	/* configuration */
	struct mx_stats stats;		/* statistics */
	struct mx_statem statem;	/* state machine */
	struct mx_notify notify;	/* event notification */

	int spans;			/* number of spans in MX */
	int channels;			/* number of channels per span in MX */
	struct {
		struct ch *list;
		uint32_t mask;
		uint32_t masks[8];
		int count;
	} a;				/* attached ch list */
	struct {
		struct ch *list;
		uint32_t mask;
		uint32_t masks[8];
		int count;
	} e;				/* enabled ch list */
	struct {
		struct ch *list;
		uint32_t mask;
		uint32_t masks[8];
		int count;
	} c;				/* connected ch list */
};

#define CH_PRIV(q) ((struct ch *)(q)->q_ptr)
#define MX_PRIV(q) ((struct mx *)(q)->q_ptr)

/**
 * ch_attach: - attach a channel to a ppa
 * @q: queue associated with CH stream
 * @ppa: phsyical point of appearance
 * @style: PPA style
 *
 * The PPA is coded as 32 bits total: 8 bits indicating card number; 8 bits providing a span mask of
 * up to 8 spans per card; 8 bits indicating the starting channel number; 8 bits indicating the
 * ending channel number.
 *
 * When a CH stream attaches to a ppa, we need to locate the MX associated with the ppa (which has
 * already been checked by calling routines) and attach the CH structure to the MX structure.  Also,
 * we need to map out the default channel map for the CH stream.
 *
 * Another time that a CH stream is attached to a PPA is when the CH stream is opened by minor
 * device number (equalling a compressed ppa).  In that case, ch_attach() is called from ch_qopen()
 * with style of CH_STYLE1.
 */
static __unlikely void
ch_attach(queue_t *q, ch_ulong ppa, ch_ulong style)
{
	struct ch *ch = CH_PRIV(q);
	struct mx *mx = ch->mx;		/* already assigned */
	int card = (ppa >> 24) & 0xff;	/* eight cards */
	int span = (ppa >> 16) & 0xff;	/* eight spans */
	int beg_chan = (ppa >> 8) & 0xff;	/* 31 channels */
	int num_chan = (ppa >> 0) & 0xff;	/* 31 channels */

	if (span == 0) {
		ch->smask = (1 << mx->spans) - 1;
		ch->span = span;
		ch->spans = mx->spans;
	} else {
		ch->smask = (1 << (span - 1));
		ch->span = span;
		ch->spans = 1;
	}
	/* fill out channel mask and count */
	if (beg_chan == 0) {
		ch->mask = mx->channels > 24 ? 0x7fffffff : 0x00ffffff;
		num_chan = mx->channels;
	} else {
		if (num_chan == 0)
			num_chan = (mx->channels - (beg_chan - 1));
		if (num_chan > mx->channels)
			num_chan = mx->channels;
		for (i = beg_chan, x = (1 << (beg_chan - 1)); i < beg_chan + num_chan; i++, x <<= 1)
			ch->mask |= x;
	}
	ch->channels = num_chan;

	for (i = ch->span; i < ch->spans; i++)
		ch->masks[i] |= ch->mask;

	/* save ppa */
	ch->ppa = ppa;

	/* link ch in to mx's attached CH list */
	if ((ch->a.next = mx->a.list))
		ch->a.next->a.prev = &ch->a.next;
	ch->a.prev = &mx->a.list;
	ch->mx = mx;
	mx->a.list = ch;
	mx->a.count++;
	mx->a.mask |= ch->mask;
	for (i = 0; i < 8; i++)
		mx->a.masks[i] |= ch->a.masks[i];

	/* set up parameters */
	ch->parm.cp_type = CH_PARMS_CIRCUIT;
	ch->parm.cp_encoding = CH_ENCODING_NONE;
	ch->parm.cp_block_size = 8 * ch->spans * ch->channels;
	ch->parm.cp_samples = 8;
	ch->parm.cp_sample_size = 8;
	ch->parm.cp_rate = 8000;
	ch->parm.cp_tx_channels = 1;
	ch->parm.cp_rx_channels = 1;
	ch->parm.cp_opt_flags = 0;

	/* set up information */
	ch->info.ch_addr_length = sizeof(ch->ppa);
	ch->info.ch_addr_offset = sizeof(ch->info);
	ch->info.ch_parm_length = sizeof(ch->parm);
	ch->info.ch_parm_offset = sizeof(ch->info) + sizeof(ch->ppa);
	ch->info.ch_prov_flags = 0;
	ch->info.ch_class = CH_CIRCUIT;
	ch->info.ch_style = style;
	ch->info.ch_version = CH_VERSION;
	ch_set_state(ch, CHS_ATTACHED);
}

/**
 * ch_enable: enable a CH stream
 * @ch: CH private structure
 *
 * Note that while a CH stream is attached to an MX, the CH stream can be used to alter
 * characteristics of the MX, provided that there are no other CH streams enabled for the MX.  Once
 * the first CH stream is enabled for the MX, the MX is enabled with the current configuration.
 * Disruptive configuration changes (such as switching the MX from E1 to T1 operation) will be
 * refused from this point until all CH streams disable for the MX.  When the last CH stream
 * disables for the MX, the MX is not disabled, but will accept disruptive configuration changes
 * once again.  Whether to lock distruptive configuration changes or not and when to bring up a MX
 * stream, is one of the primary purposes of the enable/disable operation.
 */
static __unlikely void
ch_enable(struct ch *ch)
{
	if ((ch->e.next = mx->e.list))
		ch->e.next->e.prev = &ch->e.next;
	ch->e.prev = &mx->e.list;
	mx->e.list = ch;
	mx->e.count++;
	mx_e_mask(mx);
	ch_set_state(ch, CHS_ENABLED);
}

static __unlikely void
ch_init_priv(queue_t *q, dev_t dev, cred_t *crp)
{
	struct ch *ch = CH_PRIV(q);

	bzero(ch, sizeof(*ch));

	ch->a.next = NULL;
	ch->a.prev = &ch->a.next;
	ch->e.next = NULL;
	ch->e.prev = &ch->e.next;
	ch->c.next = NULL;
	ch->c.prev = &ch->c.next;

	ch->info.ch_primitive = CH_INFO_ACK;
	ch->info.ch_addr_length = 0;
	ch->info.ch_addr_offset = 0;
	ch->info.ch_parm_length = 0;
	ch->info.ch_parm_offset = 0;
	ch->info.ch_prov_flags = 0;
	ch->info.ch_prov_class = CH_CIRCUIT;
	ch->info.ch_style = CH_STYLE2;
	ch->info.ch_version = CH_VERSION;
	ch->info.ch_state = CHS_DETACHED;

	ch->parm.cp_type = CH_PARMS_CIRCUIT;
	ch->parm.cp_encoding = CH_ENCODING_NONE;
	ch->parm.cp_block_size = 8;
	ch->parm.cp_samples = 8;
	ch->parm.cp_sample_size = 8;
	ch->parm.cp_rate = 8000;
	ch->parm.cp_tx_channels = 1;
	ch->parm.cp_rx_channels = 1;
	ch->parm.cp_opt_flags = 0;
}

static __unlikely void
ch_term_priv(queue_t *q)
{
	struct ch *ch = CH_PRIV(q);

	switch (ch_get_state(ch)) {
	default:
		/* don't know, run them anyway */
	case CHS_CONNECTED:
		ch_disconnect(ch, true);	/* forced */
	case CHS_ENABLED:
		ch_disable(ch, true);	/* forced */
	case CHS_ATTACHED:
		ch_detach(ch, true);	/* forced */
	case CHS_DETACHED:
		break;
	}
}

static __unlikely void
mx_init_priv(queue_t *q, int index, cred_t *crp)
{
	struct mx *mx = CH_PRIV(q);

	bzero(mx, sizeof(*mx));

	mx->info.mx_primitive = MX_INFO_ACK;
	mx->info.mx_addr_length = 0;
	mx->info.mx_addr_offset = 0;
	mx->info.mx_parm_length = 0;
	mx->info.mx_parm_offset = 0;
	mx->info.mx_prov_flags = 0;
	mx->info.mx_prov_class = MX_CIRCUIT;
	mx->info.mx_style = MX_STYLE2;
	mx->info.mx_version = MX_VERSION;
	mx->info.mx_state = MXS_DETACHED;

	mx->parm.mp_type = MX_PARMS_CIRCUIT;
	mx->parm.mp_encoding = MX_ENCODING_NONE;
	mx->parm.mp_block_size = 8192;
	mx->parm.mp_samples = 8;
	mx->parm.mp_sample_size = 8;
	mx->parm.mp_rate = 8000;
	mx->parm.mp_tx_channels = 31;
	mx->parm.mp_rx_channels = 31;
	mx->parm.mp_opt_flags = 0;
}

static __unlikely void
mx_term_priv(queue_t *q)
{
	struct mx *mx = MX_PRIV(q);

	switch (mx_get_state(mx)) {
	default:
		/* don't know, run them anyway */
	case MXS_CONNECTED:
		mx_disconnect(mx, true);	/* forced */
	case MXS_ENABLED:
		mx_disable(mx, true);	/* forced */
	case MXS_ATTACHED:
		mx_detach(mx, true);	/* forced */
	case MXS_DETACHED:
		break;
	}
}

/*
 *  CH State Processing functions.
 *  ==============================
 */
static void
mx_a_mask(struct mx *mx)
{
	struct ch *ch;
	int span;

	mx->a.mask = 0;
	for (span = 0; span < mx->spans; span++) {
		mx->a.masks[span] = 0;
		for (ch = mx->a.list; ch; ch = ch->a.next)
			mx->a.masks[span] |= ch->masks[span];
		mx->a.mask |= mx->a.masks[span];
	}
}
static void
mx_e_mask(struct mx *mx)
{
	struct ch *ch;
	int span;

	mx->e.mask = 0;
	for (span = 0; span < mx->spans; span++) {
		mx->e.masks[span] = 0;
		for (ch = mx->e.list; ch; ch = ch->e.next)
			mx->e.masks[span] |= ch->masks[span];
		mx->e.mask |= mx->e.masks[span];
	}
}
static void
mx_c_mask(struct mx *mx)
{
	struct ch *ch;
	int span;

	mx->c.mask = 0;
	for (span = 0; span < mx->spans; span++) {
		mx->c.masks[span] = 0;
		for (ch = mx->c.list; ch; ch = ch->c.next)
			mx->c.masks[span] |= ch->masks[span];
		mx->c.mask |= mx->c.masks[span];
	}
}

static void
ch_attach(struct ch *ch, ch_ulong ppa, ch_ulong style)
{
	unsigned long flags;
	struct mx *mx = ch->mx;
	int span;

	write_lock_irqsave(&mx->lock, flags);
	state = CHS_ATTACHED;
	if ((ch->a.next = mx->a.list))
		ch->a.next->a.prev = &ch->a.next;
	ch->a.prev = &mx->a.list;
	mx->a.list = ch;
	mx_a_mask(mx);
	write_unlock_irqrestore(&mx->lock, flags);
}
static int
ch_enable(struct ch *ch)
{
	unsigned long flags;
	struct mx *mx = ch->mx;
	int span;

	write_lock_irqsave(&mx->lock, flags);
	state = CHS_ENABLED;
	if ((ch->e.next = mx->e.list))
		ch->e.next->e.prev = &ch->e.next;
	ch->e.prev = &mx->e.list;
	mx->e.list = ch;
	mx_e_mask(mx);
	write_unlock_irqrestore(&mx->lock, flags);
	if (mx_get_state(mx) == MXS_ATTACHED) {
		/* TODO: need to ask MX to enable */
	}
	return (0);
}
static void
ch_connect(struct ch *ch, ch_ulong flag, ch_ulong slot)
{
	unsigned long flags;
	struct mx *mx = ch->mx;
	int span;

	write_lock_irqsave(&mx->lock, flags);
	state = CHS_CONNECTED;
	if ((ch->c.next = mx->c.list))
		ch->c.next->c.prev = &ch->c.next;
	ch->c.prev = &mx->c.list;
	mx->c.list = ch;
	mx_c_mask(mx);
	/* TODO: need to ask MX to connect channels */
	write_unlock_irqrestore(&mx->lock, flags);
}
static void
ch_disconnect(struct ch *ch, bool close)
{
	unsigned long flags;
	struct mx *mx = ch->mx;
	int span;

	write_lock_irqsave(&mx->lock, flags);
	state = CHS_ENABLED;
	if ((*ch->c.prev = ch->c.next))
		ch->c.next->c.prev = ch->c.prev;
	ch->c.next = NULL;
	ch->c.prev = &ch->c.next;
	mx_c_mask(mx);
	/* TODO: need to ask MX to disconnect channels */
	write_unlock_irqrestore(&mx->lock, flags);
}
static void
ch_disable(struct ch *ch, bool close)
{
	struct mx *mx = ch->mx;
	int span;

	if ((*ch->e.prev = ch->e.next))
		ch->e.next->e.prev = ch->e.prev;
	ch->e.next = NULL;
	ch->e.prev = &ch->e.next;
	mx_e_mask(mx);
	ch_set_state(ch, CHS_ATTACHED);
}
static void
ch_detach(struct ch *ch, bool close)
{
	unsigned long flags;
	struct mx *mx = ch->mx;
	int span;

	write_lock_irqsave(&mx->lock, flags);
	state = CHS_DETACHED;
	if ((*ch->a.prev = ch->a.next))
		ch->a.next->a.prev = ch->a.prev;
	ch->a.next = NULL;
	ch->a.prev = &ch->a.next;
	mx_a_mask(mx);
	write_unlock_irqrestore(&mx->lock, flags);
}

/*
 *  MX Primitives sent downstream.
 *  ==============================
 */

/***
 * mx_info_req: - send MX_INFO_REQ primitive downstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
STATIC __unlikely int
mx_info_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_info_req *p;

	if ((mp = ch_alloc(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, CHLOGTX, SL_TRACE, "MX_INFO_REQ ->");
		putnext(mx->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/***
 * mx_optmgmt_req: - send MX_OPTMGMT_REQ primitive downstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @optr: pointer to options to place in message
 * @olen: length of options to place in message
 * @flags: management flags
 */
STATIC __unlikely int
mx_optmgmt_req(struct mx *mx, queue_t *q, mblk_t *msg, caddr_t optr, size_t olen, mx_ulong flags)
{
	struct MX_optmgmt_req *p;

	if ((mp = mi_allocb(q, sizeof(*p) + olen, BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OPTMGMT_REQ;
		p->mx_opt_length = olen;
		p->mx_opt_offset = olen ? sizeof(*p) : 0;
		p->mx_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		freemsg(msg);
		mi_strlog(q, CHLOGTX, SL_TRACE, "MX_OPTMGMT_REQ ->");
		putnext(mx->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/***
 * mx_attach_req: - send MX_ATTACH_REQ primitive downstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @aptr: pointer to addresses to place in message
 * @alen: length of addresses to place in message
 */
STATIC __unlikely int
mx_attach_req(struct mx *mx, queue_t *q, mblk_t *msg, caddr_t aptr, size_t alen)
{
	struct MX_attach_req *p;

	if ((mp = mi_allocb(q, sizeof(*p) + alen, BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_ATTACH_REQ;
		p->mx_addr_length = alen;
		p->mx_addr_offset = alen ? sizeof(*p) : 0;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		mx_set_state(mx, MXS_WACK_AREQ);
		freemsg(msg);
		mi_strlog(q, CHLOGTX, SL_TRACE, "MX_ATTACH_REQ ->");
		putnext(mx->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/***
 * mx_enable_req: - send MX_ENABLE_REQ primitive downstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
STATIC __unlikely int
mx_enable_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_enable_req *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_ENABLE_REQ;
		mp->b_wptr += sizeof(*p);
		mx_set_state(mx, MXS_WACK_EREQ);
		freemsg(msg);
		mi_strlog(q, CHLOGTX, SL_TRACE, "MX_ENABLE_REQ ->");
		putnext(mx->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/***
 * mx_connect_req: - send MX_CONNECT_REQ primitive downstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: connection flags
 * @slot: connection slot (PPA range format)
 */
STATIC __unlikely int
mx_connect_req(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot)
{
	struct MX_connect_req *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_CONNECT_REQ;
		p->mx_conn_flags = flags;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		mx_set_state(mx, MXS_WACK_CREQ);
		freemsg(msg);
		mi_strlog(q, CHLOGTX, SL_TRACE, "MX_CONNECT_REQ ->");
		putnext(mx->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/***
 * mx_data_req: - send MX_DATA_REQ primitive downstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @dp: data to include in message
 *
 * The data pointer points to a data block that contains channel bytes laid out in multiplex
 * interface order.  Intermodule communication should use the M_DATA message instead of this M_PROTO
 * message.
 */
STATIC inline fastcall int
mx_data_req(struct mx *mx, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	struct MX_data_req *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(mx->oq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_DATA_REQ;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			putnext(mx->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/***
 * mx_sig_req: - send MX_SIG_REQ primitive downstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @dp: data to include in message
 *
 * The data pointer points to channel associated signalling data laid out in multiplex interface
 * order.  This is subject to flow control.  Intermodule communication should use the M_CTL message
 * instead of this M_PROTO messages.
 */
STATIC inline fastcall int
mx_sig_req(struct mx *mx, queue_t *q, mblk_t *msg, mblk_t *dp)
{
	struct MX_sig_req *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if (canputnext(mx->oq)) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->mx_primitive = MX_SIG_REQ;
			mp->b_wptr += sizeof(*p);
			if (msg && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			putnext(mx->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/***
 * mx_disconnect_req: - send MX_DISCONNECT_REQ primitive downstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @flags: connection flags
 * @slot: connection slot (PPA range format)
 */
STATIC __unlikely int
mx_disconnect_req(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot)
{
	struct MX_disconnect_req *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISCONNECT_REQ;
		p->mx_conn_flags = flags;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		mx_set_state(mx, MXS_WACK_DREQ);
		freemsg(msg);
		mi_strlog(q, CHLOGTX, SL_TRACE, "MX_DISCONNECT_REQ ->");
		putnext(mx->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/***
 * mx_disable_req: - send MX_DISABLE_REQ primitive downstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
STATIC __unlikely int
mx_disable_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_disable_req *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISABLE_REQ;
		mp->b_wptr += sizeof(*p);
		mx_set_state(mx, MXS_WACK_RREQ);
		freemsg(msg);
		mi_strlog(q, CHLOGTX, SL_TRACE, "MX_DISABLE_REQ ->");
		putnext(mx->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/***
 * mx_detach_req: - send MX_DETACH_REQ primitive downstream
 * @mx: private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 */
STATIC __unlikely int
mx_detach_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_detach_req *p;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DETACH_REQ;
		mp->b_wptr += sizeof(*p);
		mx_set_state(mx, MXS_WACK_UREQ);
		freemsg(msg);
		mi_strlog(q, CHLOGTX, SL_TRACE, "MX_DETACH_REQ ->");
		putnext(mx->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  CH Primitives sent upstream.
 *  ============================
 */
/**
 * ch_info_ack: - send an CH_INFO_ACK primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 */
STATIC __unlikely int
ch_info_ack(struct ch *ch, queue_t *q, mblk_t *msg)
{
	size_t alen = ch->info.ch_addr_length;
	size_t plen = ch->info.ch_parm_length;
	caddr_t aptr = (caddr_t) &ch->addr;
	caddr_t pptr = (caddr_t) &ch->parm;

	struct CH_info_ack *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + alen + plen, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		*p = ch->info;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		bcopy(pptr, mp->b_wptr, plen);
		mp->b_wptr += plen;
		freemsg(msg);
		mi_strlog(q, CHLOGTX, SL_TRACE, "<- CH_INFO_ACK");
		putnext(ch->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_optmgmt_ack: - send an CH_OPTMGMT_ACK primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @flags: management flags
 * @olen: options length
 * @optr: options pointer
 */
STATIC __unlikely int
ch_optmgmt_ack(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, size_t olen, caddr_t optr)
{
	struct CH_optmgmt_ack *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p) + olen, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OPTMGMT_ACK;
		p->ch_opt_length = olen;
		p->ch_opt_offset = olen ? sizeof(*p) : 0;
		p->ch_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		freemsg(msg);
		mi_strlog(q, CHLOGTX, SL_TRACE, "<- CH_OPTMGMT_ACK");
		putnext(ch->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_ok_ack: - send an CH_OK_ACK primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @prim: correct primitive
 * @state: resulting state
 */
STATIC __unlikely int
ch_ok_ack(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong prim, ch_ulong state)
{
	struct CH_ok_ack *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		struct mx *mx;

		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OK_ACK;
		p->ch_correct_prim = prim;
		p->ch_state = state;
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, state);
		freemsg(msg);
		mi_strlog(q, CHLOGTX, SL_TRACE, "<- CH_OK_ACK");
		putnext(ch->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_error_ack: - send an CH_ERROR_ACK primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @type: error type
 * @error: UNIX error
 */
STATIC __unlikely int
ch_error_ack(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong prim, ch_ulong type, ch_ulong error)
{
	struct CH_error_ack *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_ERROR_ACK;
		p->ch_error_primitive = prim;
		p->ch_error_type = type;
		p->ch_unix_error = error;
		p->ch_state = ch_get_state(ch);
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, state);
		freemsg(msg);
		mi_strlog(q, CHLOGTX, SL_TRACE, "<- CH_ERROR_ACK");
		putnext(ch->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_attach_ack: - send an CH_OK_ACK primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @ppa: PPA to which to attach
 */
STATIC __unlikely int
ch_attach_ack(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong ppa)
{
	struct CH_ok_ack *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OK_ACK;
		p->ch_correct_prim = CH_ATTACH_REQ;
		p->ch_state = CHS_ATTACHED;
		mp->b_wptr += sizeof(*p);
		ch_attach(ch, ppa, CH_STYLE2);
		freemsg(msg);
		mi_strlog(q, CHLOGTX, SL_TRACE, "<- CH_OK_ACK");
		putnext(ch->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_enable_ack_con: - send an CH_OK_ACK/CH_ENABLE_CON primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 */
STATIC __unlikely int
ch_enable_ack_con(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_enable_con *p;
	struct CH_ok_ack *p2;
	mblk_t *mp, *mp2;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = mi_allocb(q, sizeof(*p2), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_ENABLE_CON;
			mp->b_wptr += sizeof(*p);

			mp2->b_datap->db_type = M_PCPROTO;
			p2 = (typeof(p2)) mp2->b_wptr;
			p2->ch_primitive = CH_OK_ACK;
			p2->ch_correct_prim = CH_ENABLE_REQ;
			p2->ch_state = ch_set_state(CHS_WCON_EREQ);
			mp2->b_wptr += sizeof(*p2);

			freemsg(msg);
			mi_strlog(q, CHLOGTX, SL_TRACE, "<- CH_OK_ACK");
			putnext(ch->oq, mp2);
			ch_enable(ch);
			mi_strlog(q, CHLOGTX, SL_TRACE, "<- CH_ENABLE_CON");
			putnext(ch->oq, mp);
			return (0);
		}
		freeb(mp);
	}
	return (-ENOBUFS);
}

/**
 * ch_enable_con: - send a CH_ENABLE_CON primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 */
STATIC __unlikely int
ch_enable_con(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_enable_con *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_ENABLE_CON;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		ch_enable(ch);
		mi_strlog(q, CHLOGTX, SL_TRACE, "<- CH_ENABLE_CON");
		putnext(ch->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_data_ind: - send an CH_DATA_IND primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @slot: slot within multiplex
 */
STATIC __unlikely int
ch_data_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong slot)
{
	struct CH_data_ind *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DATA_IND;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = XCHG(&msg->b_cont, NULL);
		freemsg(msg);
		mi_strlog(q, CHLOGDA, SL_TRACE, "<- CH_DATA_IND");
		putnext(ch->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disconnect_ind: - send an CH_DISCONNECT_IND primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @flags: connect flags
 * @slot: connect slot
 * @cause: disconnect cause
 */
STATIC __unlikely int
ch_disconnect_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot,
		  ch_ulong cause)
{
	struct CH_disconnect_ind *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISCONNECT_IND;
		p->ch_conn_flags = flags;
		p->ch_slot = slot;
		p->ch_cause = cause;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, CHLOGTX, SL_TRACE, "<- CH_DISCONNECT_IND");
		putnext(ch->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disconnect_con: - send a CH_DISCONNECT_CON primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @flags: connect flags
 * @slot: connect slot
 */
STATIC __unlikely int
ch_disconnect_con(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_disconnect_con *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISCONNECT_CON;
		p->ch_conn_flags = flags;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		ch_disconnect(ch);
		mi_strlog(q, CHLOGTX, SL_TRACE, "<- CH_DISCONNECT_CON");
		putnext(ch->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disable_ind: - send an CH_DISABLE_IND primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @cause: disable cause
 */
STATIC __unlikely int
ch_disable_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong cause)
{
	struct CH_disable_ind *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISABLE_IND;
		p->ch_cause = cause;
		mp->b_wptr += sizeof(*p);
		ch_disable(ch);
		ch_set_state(ch, CHS_ATTACHED);
		freemsg(msg);
		mi_strlog(q, CHLOGTX, SL_TRACE, "<- CH_DISABLE_IND");
		putnext(ch->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disable_ack_con: - send an CH_OK_ACK/CH_DISABLE_CON primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 */
STATIC __unlikely int
ch_disable_ack_con(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_disable_con *p;
	struct CH_ok_ack *p2;
	mblk_t *mp, *mp2;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		if ((mp2 = mi_allocb(q, sizeof(*p2), BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->ch_primitive = CH_DISABLE_CON;
			mp->b_wptr += sizeof(*p);

			mp2->b_datap->db_type = M_PCPROTO;
			p2 = (typeof(p2)) mp2->b_wptr;
			p2->ch_primitive = CH_OK_ACK;
			p2->ch_correct_prim = CH_DISABLE_REQ;
			p2->ch_state = ch_set_state(CHS_WCON_RREQ);
			mp2->b_wptr += sizeof(*p2);

			freemsg(msg);
			mi_strlog(q, CHLOGTX, SL_TRACE, "<- CH_OK_ACK");
			putnext(ch->oq, mp2);
			ch_disable(ch, false);
			mi_strlog(q, CHLOGTX, SL_TRACE, "<- CH_DISABLE_CON");
			putnext(ch->oq, mp);
			return (0);
		}
		freeb(mp);
	}
	return (-ENOBUFS);
}

/**
 * ch_disable_con: = send a CH_DISABLE_CON primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 */
STATIC __unlikely int
ch_disable_con(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_disable_con *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		DB_TYPE(mp) = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISABLE_CON;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		ch_disable(ch);
		mi_strlog(q, CHLOGTX, SL_TRACE, "<- CH_DISABLE_CON");
		putnext(ch->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_detach_ack: - send an CH_OK_ACK primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 */
STATIC __unlikely int
ch_detach_ack(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_ok_ack *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OK_ACK;
		p->ch_correct_prim = CH_DETACH_REQ;
		p->ch_state = CHS_DETACHED;
		mp->b_wptr += sizeof(*p);

		freemsg(msg);
		ch_detach(ch, false);
		mi_strlog(q, CHLOGTX, SL_TRACE, "<- CH_OK_ACK");
		putnext(ch->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_event_ind: - send an CH_EVENT_IND primitive upstream
 * @ch: private structure
 * @q: active queue
 * @msg: message to free or consume if successful
 * @event: event indicated
 * @slot: slot for event
 */
STATIC __unlikely int
ch_event_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong event, ch_ulong slot)
{
	struct CH_event_ind *p;
	mblk_t *mp;

	if ((mp = mi_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_EVENT_IND;
		p->ch_event = event;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		mi_strlog(q, CHLOGTX, SL_TRACE, "<- CH_EVENT_IND");
		putnext(ch->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  CH Primitives received from upstream.
 *  =====================================
 */
/**
 * ch_info_req: - process CH_INFO_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
ch_info_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	return ch_info_ack(ch, q, mp);
}

/**
 * ch_optmgmt_req: - process CH_OPTMGMT_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
ch_optmgmt_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	return ch_error_ack(ch, q, mp, CH_OPTMGMT_REQ, CHNOTSUPP, EOPNOTSUPP);
}

/**
 * ch_attach_req: - process CH_ATTACH_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 *
 * Requests that the stream be attached to the specified card and span.  The specified card and span
 * must exist and be available.  (Each lower multiplex MX stream represents a card.  They are linked
 * under the multiplexing driver in order of ascending card number starting at 1.)
 */
STATIC noinline __unlikely int
ch_attach_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_attach_req *p = (typeof(p)) mp->b_rptr;
	uint32_t addr;
	int card, span, smask, cmask;
	struct mx *mx;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch->info.ch_style != CH_STYLE2)
		goto notsupport;
	if (ch_get_state(ch) != CHS_DETACHED)
		goto outstate;
	if (p->ch_addr_length != sizeof(addr))
		goto badaddr;
	if (mp->b_wptr < mp->b_rptr + p->ch_addr_length + p->ch_addr_offset)
		goto protoshort;
	/* avoid alignment problems */
	bcopy(mp->b_rptr, &addr, sizeof(addr));
	{
		int card = (addr >> 16) & 0x0ff;
		int span = (addr >> 8) & 0x0ff;
		int slot = (addr >> 0) & 0x0ff;

		if (!card || !(mx = ch_find_card(ch, card)))
			goto badaddr;
		if (span != 0 && (mx->spans + 1 < span || span < 1))
			goto badaddr;
		if (slot != 0 && (mx->channels + 1 < slot || slot < 1))
			goto badaddr;
	}
	return ch_attach_ack(ch, q, mp, addr);
      badaddr:
	return ch_error_ack(ch, q, mp, CH_ATTACH_REQ, CHBADADDR, EINVAL);
      notsupport:
	return ch_error_ack(ch, q, mp, CH_ATTACH_REQ, CHNOTSUPP, EOPNOTSUPP);
      outstate:
	return ch_error_ack(ch, q, mp, CH_ATTACH_REQ, CHOUTSTATE, EPROTO);
      badprim:
	return ch_error_ack(ch, q, mp, CH_ATTACH_REQ, CHBADPRIM, EMSGSIZE);
}

/**
 * ch_enable_req: - process CH_ENABLE_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
ch_enable_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_enable_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badrpim;
	if (ch_get_state(ch) != CHS_ATTACHED)
		goto outstate;
	ch_set_state(ch, CHS_WACK_EREQ);
	if (ch->mx->e.count > 0)
		return ch_enable_ack_con(ch, q, mp);
	/* enable multiplex if first channel to enable */
	return mx_enable_req(ch->mx, q, mp);
      outstate:
	return ch_error_ack(ch, q, mp, CH_ENABLE_REQ, CHOUTSTATE, EPROTO);
      badprim:
	return ch_error_ack(ch, q, mp, CH_ENABLE_REQ, CHBADPRIM, EMSGSIZE);
}

/**
 * ch_connect_req: - process CH_CONNECT_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
ch_connect_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_connect_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch_get_state(ch) != CHS_ENABLED)
		goto outstate;
	ch_set_state(ch, CHS_WACK_CREQ);
	return mx_connect_req(ch->mx, q, mp, MX_BOTH, ch->ppa);
      outstate:
	return ch_error_ack(ch, q, mp, CH_CONNECT_REQ, CHOUTSTATE, EPROTO);
      badprim:
	return ch_error_ack(ch, q, mp, CH_CONNECT_REQ, CHBADPRIM, EMSGSIZE);
}

/**
 * ch_data_req: - process CH_DATA_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
ch_data_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
}

/**
 * ch_disconnect_req: - process CH_DISCONNECT_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
ch_disconnect_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disconnect_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch_get_state(ch) != CHS_CONNECTED)
		goto outstate;
	ch_set_state(ch, CHS_WACK_DREQ);
	return mx_disconnect_req(ch->mx, q, mp, MX_BOTH, ch->ppa);
      outstate:
	return ch_error_ack(ch, q, mp, CH_DISCONNECT_REQ, CHOUTSTATE, EPROTO);
      badprim:
	return ch_error_ack(ch, q, mp, CH_DISCONNECT_REQ, CHBADPRIM, EMSGSIZE);
}

/**
 * ch_disable_req: - process CH_DISABLE_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
ch_disable_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disable_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch_get_state(ch) != CHS_ENABLED)
		goto outstate;
	ch_set_state(ch, CHS_WACK_RREQ);
	if (ch->mx->e.count > 1)
		return ch_disable_ack_con(ch, q, mp);
	return mx_disable_req(ch->mx, q, mp);
      outstate:
	return ch_error_ack(ch, q, mp, CH_DISABLE_REQ, CHOUTSTATE, EPROTO);
      badprim:
	return ch_error_ack(ch, q, mp, CH_DISABLE_REQ, CHBADPRIM, EMSGSIZE);
}

/**
 * ch_detach_req: - process CH_DETACH_REQ primitive
 * @ch: CH private structure
 * @q: active queue (write queue)
 * @mp: the primitive
 */
STATIC noinline __unlikely int
ch_detach_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_detach_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch_get_state(ch) != CHS_ATTACHED)
		goto oustate;
	ch_set_state(ch, CHS_WACK_UREQ);
	return ch_detach_ack(ch, q, mp);
      outstate:
	return ch_error_ack(ch, q, mp, CH_DETACH_REQ, CHOUTSTATE, EPROTO);
      badprim:
	return ch_error_ack(ch, q, mp, CH_DETACH_REQ, CHBADPRIM, EMSGSIZE);
}

/**
 * ch_w_data: - process M_DATA message block
 * @q: write queue
 * @mp: message
 *
 * The MX stream allocates a TX buffer of sufficient size to hold one TX block.  If the TX block has
 * not yet been written to for this channel, data is written directly to the TX block.  If the TX
 * block has been written, data is queued.  If the data block is too small it is discarded.
 */
static inline fastcall int
ch_w_data(queue_t *q, mblk_t *mp)
{
	struct ch *ch;
	struct mx *mx = ch->mx;
	int err;

	err = 0;
	if (mp->b_wptr < mp->b_rptr + ch->blocksize)
		goto error;

	err = -EDEADLK;
	if ((ch = (struct ch *) mi_trylock(q)) == NULL)
		goto error;

	err = 0;
	if (ch_get_state(ch) != CHS_CONNECTED)
		goto ch_unlock_error;

	err = 0;
	if (!(mx = ch->mx))
		goto ch_unlock_error;

#if 0
	err = -EDEADLK;
	if (!mi_trylock(q))
		goto ch_unlock_error;
#endif

	err = 0;
	if (mx_get_state(mx) != MXS_CONNECTED)
		goto mx_unlock_error;

	err = -EBUSY;
	if (ch->tx.block == mx->tx.block)
		goto mx_unlock_error;

	{
		unsigned char *p, *q, *r;
		int frame, span, chan;
		uint32_t mask;

		for (p = mx->tx.buf, frame = 0; frame < 8; p += mx->spans * mx->channels, frame++)
			for (span = ch->span, q = p; span < ch->spans; q++, span++)
				for (mask = 0x1, chan = 0, r = q;
				     ch->mask & ~(mask - 1); chan++, r += mx->spans, mask <<= 1)
					if (ch->mask & mask)
						*r = *mp->b_rptr++;
	}

	ch->tx.block = mx->tx.block;

	err = -EAGAIN;
	if (mp->b_wptr >= mp->b_rptr + ch->blocksize)
		/* put back remainder */
		goto mx_unlock_error;

	err = 0;
#if 0
      mx_unlock_error:
	mi_unlock(priv);
#endif
      ch_unlock_error:
	mi_unlock((caddr_t) ch);
      error:
	if (err == 0)
		freemsg(mp);
	return (err);
}

/**
 * ch_w_data_slow: - process M_DATA message block
 * @q: write queue
 * @mp: message
 */
static noinline fastcall __unlikely int
ch_w_data_slow(queue_t *q, mblk_t *mp)
{
	return ch_w_data(q, mp);
}

static fastcall int
ch_w_proto(queue_t *q, mblk_t *mp)
{
	struct ch *ch;
	int rtn = -EDEADLK;

	if ((ch = (struct ch *) mi_trylock(q))) {
		ch_ulong oldstate = ch_get_state(ch);

		switch (*(ch_ulong *) mp->b_rptr) {
		case CH_INFO_REQ:
			mi_strlog(q, 0, SL_TRACE, "-> CH_INFO_REQ");
			rtn = ch_info_req(ch, q, mp);
			break;
		case CH_OPTMGMT_REQ:
			mi_strlog(q, 0, SL_TRACE, "-> CH_OPTMGMT_REQ");
			rtn = ch_optmgmt_req(ch, q, mp);
			break;
		case CH_ATTACH_REQ:
			mi_strlog(q, 0, SL_TRACE, "-> CH_ATTACH_REQ");
			rtn = ch_attach_req(ch, q, mp);
			break;
		case CH_ENABLE_REQ:
			mi_strlog(q, 0, SL_TRACE, "-> CH_ENABLE_REQ");
			rtn = ch_enable_req(ch, q, mp);
			break;
		case CH_CONNECT_REQ:
			mi_strlog(q, 0, SL_TRACE, "-> CH_CONNECT_REQ");
			rtn = ch_connect_req(ch, q, mp);
			break;
		case CH_DATA_REQ:
			mi_strlog(q, 0, SL_TRACE, "-> CH_DATA_REQ");
			rtn = ch_data_req(ch, q, mp);
			break;
		case CH_DISCONNECT_REQ:
			mi_strlog(q, 0, SL_TRACE, "-> CH_DISCONNECT_REQ");
			rtn = ch_disconnect_req(ch, q, mp);
			break;
		case CH_DISABLE_REQ:
			mi_strlog(q, 0, SL_TRACE, "-> CH_DISABLE_REQ");
			rtn = ch_disable_req(ch, q, mp);
			break;
		case CH_DETACH_REQ:
			mi_strlog(q, 0, SL_TRACE, "-> CH_DETACH_REQ");
			rtn = ch_detach_req(ch, q, mp);
			break;
		default:
			mi_strlog(q, 0, SL_TRACE, "-> CH_????_???");
			rtn = ch_other_req(ch, q, mp);
			break;
		}
		if (rtn)
			ch_set_state(ch, oldstate);
		mi_unlock((caddr_t) ch);
	}
	return (rtn);
}
static fastcall int
ch_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	struct ch *ch = CH_PRIV(q);
	mblk_t *dp;

	switch (ioc->ioc_cmd) {
	case CH_IOCGCONFIG:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(CH_IOCGCONFIG)");
		if ((dp = mi_copyout_alloc(q, mp, 0, sizeof(ch->config)))) {
			bcopy(&ch->config, dp->b_rptr, sizeof(ch->config));
			mi_copyout(q, mp);
		}
		break;
	case CH_IOCSCONFIG:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(CH_IOCSCONFIG)");
		/* fall through */
	case CH_IOCTCONFIG:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(CH_IOCTCONFIG)");
		/* fall through */
	case CH_IOCCCONFIG:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(CH_IOCCCONFIG)");
		mi_copyin(q, mp, NULL, sizeof(ch->config));
		break;
	case CH_IOCGSTATEM:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(CH_IOCGSTATEM)");
		if ((dp = mi_copyout_alloc(q, mp, 0, sizeof(ch->statem)))) {
			bcopy(&ch->statem, dp->b_rptr, sizeof(ch->statem));
			mi_copyout(q, mp);
		}
		break;
	case CH_IOCCMRESET:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(CH_IOCCMRESET)");
		/* reset state machine */
		mi_copy_set_rval(mp, 0);
		mi_copy_done(q, mp, 0);
		break;
	case CH_IOCGSTATSP:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(CH_IOCGSTATSP)");
		if ((dp = mi_copyout_alloc(q, mp, 0, sizeof(ch->statsp)))) {
			bcopy(&ch->statsp, dp->b_rptr, sizeof(ch->statsp));
			mi_copyout(q, mp);
		}
		break;
	case CH_IOCSSTATSP:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(CH_IOCSSTATSP)");
		mi_copyin(q, mp, NULL, sizeof(ch->statsp));
		break;
	case CH_IOCGSTATS:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(CH_IOCGSTATS)");
		if ((dp = mi_copyout_alloc(q, mp, 0, sizeof(ch->stats)))) {
			bcopy(&ch->stats, dp->b_rptr, sizeof(ch->stats));
			mi_copyout(q, mp);
		}
		break;
	case CH_IOCCSTATS:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(CH_IOCCSTATS)");
		/* clear statistics */
		bzero(&ch->stats, sizeof(ch->stats));
		mi_copy_set_rval(mp, 0);
		mi_copy_done(q, mp, 0);
		break;
	case CH_IOCGNOTIFY:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(CH_IOCGNOTIFY)");
		if ((dp = mi_copyout_alloc(q, mp, 0, sizeof(ch->events)))) {
			bcopy(&ch->events, dp->b_rptr, sizeof(ch->events));
			mi_copyout(q, mp);
		}
		break;
	case CH_IOCSNOTIFY:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(CH_IOCSNOTIFY)");
		mi_copyin(q, mp, NULL, sizeof(ch->events));
		break;
	case CH_IOCCNOTIFY:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(CH_IOCCNOTIFY)");
		/* clear events */
		bzero(&ch->events, sizeof(ch->events));
		mi_copy_set_rval(mp, 0);
		mi_copy_done(q, mp, 0);
		break;
	case I_PLINK:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(I_PLINK)");
		if (!drv_priv(ioc->ioc_cr)) {
			mi_copy_done(q, mp, EPERM);
			break;
		}
		goto link;
	case I_LINK:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(I_LINK)");
	      link:
		if (!(mx = mi_open_alloc(sizeof(*mx)))) {
			mi_copy_done(q, mp, ENOMEM);
			break;
		}
		if ((err = mi_open_link(&mx_links, (caddr_t) mx, (dev_t *) &l->l_index, 0,
					DRVOPEN, NULL))) {
			mi_copy_done(q, mp, err);
			break;
		}
		mi_attach(RD(l->l_qbot), (caddr_t) mx);
		mx_link(mx);
		break;
	case I_PUNLINK:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(I_PUNLINK)");
		if (!drv_priv(ioc->ioc_cr)) {
			mi_copy_done(q, mp, EPERM);
			break;
		}
		goto unlink;
	case I_UNLINK:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCTL(I_UNLINK)");
	      unlink:
		for (mx = mi_first_ptr(&mx_links); mx && mx->sid != l->l_index;
		     mx = mi_next_ptr(&mx_links, (caddr_t) mx)) ;
		if (!mx) {
			mi_copy_done(q, mp, EINVAL);
			break;
		}
		mx_unlink(mx);
		mi_detach(RD(mx->oq), (caddr_t) mx);
		mi_close_unlink(&mx_links, (caddr_t) mx);
		mi_close_free((caddr_t) mx);
		mi_copy_done(q, mp, 0);
		break;
	}
	return (0);
}
static fastcall int
ch_w_iocdata(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;
	mblk_t *dp;

	switch (ioc->ioc_cmd) {
	case CH_IOCGCONFIG:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCDATA(CH_IOCGCONFIG)");
		goto get_done;
	case CH_IOCGSTATEM:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCDATA(CH_IOCGSTATEM)");
		goto get_done;
	case CH_IOCGSTATSP:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCDATA(CH_IOCGSTATSP)");
		goto get_done;
	case CH_IOCGSTATS:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCDATA(CH_IOCGSTATS)");
		goto get_done;
	case CH_IOCGNOTIFY:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCDATA(CH_IOCGNOTIFY)");
	      get_done:
		switch (mi_copy_state(q, mp, &dp)) {
		case -1:
			break;
		case M_COPY_STATE(MI_COPY_OUT, 1):
			mi_copy_set_rval(mp, 0);
			mi_copy_done(q, mp, 0);
			break;
		}
		break;
	case CH_IOCSCONFIG:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCDATA(CH_IOCSCONFIG)");
		switch (mi_copy_state(q, mp, &dp)) {
		case -1:
			break;
		case M_COPY_STATE(MI_COPY_IN, 1):
			struct ch *ch2;

			if (!(ch2 = ch_find_id(ch, ((struct ch_config *) dp->b_rptr)->id))) {
				mi_copy_done(q, mp, EINVAL);
				break;
			}
			/* must be privileged to set some other configuration */
			if (ch2 != ch && !drv_priv(ioc->ioc_cr)
			    && ioc->ioc_cr->euid != ch2->crp->euid) {
				mi_copy_done(q, mp, EPERM);
				break;
			}
			/* FIXME: check value */
			bcopy(dp->b_rptr, &ch2->config, sizeof(ch2->config));
			mi_copy_set_rval(mp, 0);
			mi_copy_done(q, mp, 0);
			break;
		}
		break;
	case CH_IOCTCONFIG:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCDATA(CH_IOCTCONFIG)");
		switch (mi_copy_state(q, mp, &dp)) {
		case -1:
			break;
		case M_COPY_STATE(MI_COPY_IN, 1):
			struct ch *ch2;

			if (!(ch2 = ch_find_id(ch, ((struct ch_config *) dp->b_rptr)->id))) {
				mi_copy_done(q, mp, EINVAL);
				break;
			}
			/* must be privileged to set some other configuration */
			if (ch2 != ch && !drv_priv(ioc->ioc_cr)
			    && ioc->ioc_cr->euid != ch2->crp->euid) {
				mi_copy_done(q, mp, EPERM);
				break;
			}
			/* FIXME: check value */
			bcopy(dp->b_rptr, &ch2->config, sizeof(ch2->config));
			mi_copy_set_rval(mp, 0);
			mi_copy_done(q, mp, 0);
			break;
		}
		break;
	case CH_IOCCCONFIG:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCDATA(CH_IOCCONFIG)");
		switch (mi_copy_state(q, mp, &dp)) {
		case -1:
			break;
		case M_COPY_STATE(MI_COPY_IN, 1):
			struct ch *ch2;

			if (!(ch2 = ch_find_id(ch, ((struct ch_config *) dp->b_rptr)->id))) {
				mi_copy_done(q, mp, EINVAL);
				break;
			}
			/* must be privileged to set some other configuration */
			if (ch2 != ch && !drv_priv(ioc->ioc_cr)
			    && ioc->ioc_cr->euid != ch2->crp->euid) {
				mi_copy_done(q, mp, EPERM);
				break;
			}
			/* FIXME: check value */
			bcopy(dp->b_rptr, &ch2->config, sizeof(ch2->config));
			mi_copy_set_rval(mp, 0);
			mi_copy_done(q, mp, 0);
			break;
		}
		break;
	case CH_IOCCMRESET:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCDATA(CH_IOCCMRESET)");
		/* should not happen */
		break;
	case CH_IOCSSTATSP:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCDATA(CH_IOCSSTATSP)");
		switch (mi_copy_state(q, mp, &dp)) {
		case -1:
			break;
		case M_COPY_STATE(MI_COPY_IN, 1):
			struct ch *ch2;

			if (!(ch2 = ch_find_id(ch, ((struct ch_config *) dp->b_rptr)->id))) {
				mi_copy_done(q, mp, EINVAL);
				break;
			}
			/* must be privileged to set some other configuration */
			if (ch2 != ch && !drv_priv(ioc->ioc_cr)
			    && ioc->ioc_cr->euid != ch2->crp->euid) {
				mi_copy_done(q, mp, EPERM);
				break;
			}
			/* FIXME: check value */
			bcopy(dp->b_rptr, &ch2->statsp, sizeof(ch2->statsp));
			mi_copy_set_rval(mp, 0);
			mi_copy_done(q, mp, 0);
			break;
		}
		break;
	case CH_IOCCSTATS:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCDATA(CH_IOCCSTATS)");
		/* should not happen */
		break;
	case CH_IOCSNOTIFY:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCDATA(CH_IOCSNOTIFY)");
		switch (mi_copy_state(q, mp, &dp)) {
		case -1:
			break;
		case M_COPY_STATE(MI_COPY_IN, 1):
			struct ch *ch2;

			if (!(ch2 = ch_find_id(ch, ((struct ch_config *) dp->b_rptr)->id))) {
				mi_copy_done(q, mp, EINVAL);
				break;
			}
			/* must be privileged to set some other configuration */
			if (ch2 != ch && !drv_priv(ioc->ioc_cr)
			    && ioc->ioc_cr->euid != ch2->crp->euid) {
				mi_copy_done(q, mp, EPERM);
				break;
			}
			/* FIXME: check value */
			bcopy(dp->b_rptr, &ch2->events, sizeof(ch2->events));
			mi_copy_set_rval(mp, 0);
			mi_copy_done(q, mp, 0);
			break;
		}
		break;
	case CH_IOCCNOTIFY:
		mi_strlog(q, CHLOGIO, SL_TRACE, "-> M_IOCDATA(CH_IOCCNOTIFY)");
		/* should not happen */
		break;
	}
	return (0);
}
static fastcall int
ch_w_ctl(queue_t *q, mblk_t *mp)
{
}
static fastcall int
ch_w_flush(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	queue_t *oq;

	if (mp->b_rptr[0] & FLUSHW) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		if (ch->other && (oq = ch->other->oq)) {
			if (oq->q_flag & QREADR) {
				/* need to reverse sense of flush */
				switch (mp->b_rptr[0] & FLUSHRW) {
				case FLUSHR:
					mp->b_rptr[0] &= ~FLUSHRW;
					mp->b_rptr[0] |= FLUSHW;
					break;
				case FLUSHW:
					mp->b_rptr[0] &= ~FLUSHRW;
					mp->b_rptr[0] |= FLUSHR;
					break;
				}
			}
			putnext(oq, mp);
			return (0);
		}
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (ch->other && (oq = ch->other->oq)) {
			if (oq->q_flag & QREADR) {
				/* need to reverse sense of flush */
				switch (mp->b_rptr[0] & FLUSHRW) {
				case FLUSHR:
					mp->b_rptr[0] &= ~FLUSHRW;
					mp->b_rptr[0] |= FLUSHW;
					break;
				case FLUSHW:
					mp->b_rptr[0] &= ~FLUSHRW;
					mp->b_rptr[0] |= FLUSHR;
					break;
				}
			}
			putnext(oq, mp);
			return (0);
		}
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(_RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(_RD(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}
static fastcall int
ch_w_other(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	queue_t *oq;

	/* pass along with flow control */
	if (ch->other && (oq = ch->other->oq)) {
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(oq, mp->b_band)) {
			putnext(oq, mp);
			return (0);
		}
		return (-EBUSY);
	}
	freemsg(mp);
	return (0);
}

/*
 * MX Primitives received from below.
 * ==================================
 */
/**
 * mx_info_ack: - process MX_INFO_ACK primitive
 * @mx: private structure
 * @q: lower read queue
 * @mp: MX_INFO_ACK message
 */
static __unlikely int
mx_info_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_info_ack *p = (typeof(p)) mp->b_rptr;

	mx->info.mx_prov_flags = p->mx_prov_flags;
	mx->info.mx_style = p->mx_style;
	mx->info.mx_version = p->mx_version;
	mx->info.mx_state = p->mx_state;

	if (p->mx_addr_length <= sizeof(mx->ppa)) {
		bcopy(mp->b_rptr + p->mx_addr_offset, &mx->ppa, p->mx_addr_length);
		mx->info.mx_addr_length = p->mx_addr_length;
		mx->info.mx_addr_offset = sizeof(mx->info);
	} else {
		mx->info.mx_addr_length = 0;
		mx->info.mx_addr_offset = 0;
	}
	if (p->mx_parm_length == sizeof(mx->parm)
	    && *(mx_ulong *) (mp->b_rptr + p->mx_parm_offset) == mx->parm.mx_type) {
		bcopy(mp->b_rptr + p->mx_parm_offset, &mx->parm, sizeof(mx->parm));
		mx->info.mx_parm_length = p->mx_parm_length;
		mx->info.mx_parm_offset = sizeof(mx->info) + mx->info.mx_addr_length;
	} else {
		mx->info.mx_parm_length = 0;
		mx->info.mx_parm_offset = 0;
	}
	if (mx->info.mx_primitive == 0) {
		mx->info.mx_primitive = MX_INFO_ACK;
		/* First MX_INFO_ACK: the first MX_INFO_ACK gives us information about the MX
		   stream that has been linked.  If the MX stream is a style II stream, and the
		   stream has not yet been attached, the parameters information (above) will be
		   unknown.  In that case; we must issue another information request after the MX
		   stream has been attached. */
	}
	{
		/* Subsequent MX_INFO_ACK: a subsequent request is necessary immediately after an
		   attach if the stream was not attached during the last request.  Or, at any time
		   that we need to syncrhonize state or other parameters, a subsequent request can
		   be issued. */
		switch (mx->info.mx_state) {
		case MXS_ATTACHED:
		case MXS_ENABLED:
		case MXS_CONNECTED:
			break;
		default:
		case MXS_DETACHED:
			mx->info.mx_state = MXS_UNUSABLE;
			break;
		}
		switch (mx->info.mx_state) {
		case MXS_ATTACHED:
		case MXS_ENABLED:
			/* addr and parms should be valid */
			if (mx->parms.mp_type == MX_PARMS_CIRCUIT) {
			}
		}
	}
	freemsg(mp);
	return (0);
}

/**
 * mx_optmgmt_ack: - process MX_OPTMGMT_ACK primitive
 * @mx: private structure
 * @q: lower read queue
 * @mp: MX_OPTMGMT_ACK message
 */
static __unlikely int
mx_optmgmt_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_optmgmt_ack *p = (typeof(p)) mp->b_rptr;
}

/**
 * mx_ok_ack: - process MX_OK_ACK primitive
 * @mx: private structure
 * @q: lower read queue
 * @mp: MX_OK_ACK message
 */
static __unlikely int
mx_ok_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_ok_ack *p = (typeof(p)) mp->b_rptr;
	mx_ulong prim, state;
	struct ch *ch;
	int err;

	prim = p->mx_primitive;
	state = p->mx_state;

	switch (prim) {
	case MX_ENABLE_REQ:
		for (ch = mx->a.list; ch; ch = ch->a.next)
			if (ch_get_state(ch) == MXS_WACK_EREQ)
				if ((err = ch_ok_ack(ch, q, NULL, CH_ENABLE_REQ, CHS_WCON_EREQ)))
					return (err);
		break;
	case MX_CONNECT_REQ:
		for (ch = mx->e.list; ch; ch = ch->e.next)
			if (ch_get_state(ch) == MXS_WACK_CREQ)
				if ((err = ch_ok_ack(ch, q, NULL, CH_CONNECT_REQ, CHS_WCON_CREQ)))
					return (err);
		break;
	case MX_DISCONNECT_REQ:
		for (ch = mx->c.list; ch; ch = ch->c.next)
			if (ch_get_state(ch) == MXS_WACK_RREQ)
				if ((err =
				     ch_ok_ack(ch, q, NULL, CH_DISCONNECT_REQ, CHS_WCON_DREQ)))
					return (err);
		break;
	case MX_DISABLE_REQ:
		for (ch = mx->e.list; ch; ch = ch->e.next)
			if (ch_get_state(ch) == MXS_WACK_DREQ)
				if ((err = ch_ok_ack(ch, q, NULL, CH_DISABLE_REQ, CHS_WCON_RREQ)))
					return (err);
		break;
	case MX_DETACH_REQ:
	case MX_ATTACH_REQ:
	default:
		swerr();
		break;
	}
	mx_set_state(mx, state);
	freemsg(mp);
	return (0);
}

/**
 * mx_error_ack: - process MX_ERROR_ACK primitive
 * @mx: private structure
 * @q: lower read queue
 * @mp: MX_ERROR_ACK message
 */
static __unlikely int
mx_error_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_error_ack *p = (typeof(p)) mp->b_rptr;
}

/**
 * mx_enable_con: - process MX_ENABLE_CON primitive
 * @mx: private structure
 * @q: lower read queue
 * @mp: MX_ENABLE_CON message
 */
static __unlikely int
mx_enable_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_enable_con *p = (typeof(p)) mp->b_rptr;
	struct ch *ch;
	int err;

	for (ch = mx->a.list; ch; ch = ch->a.next)
		if (ch_get_state(ch) == CHS_WCON_EREQ)
			if ((err = ch_enable_con(ch, q, NULL)))
				return (err);
	mx_set_state(mx, MXS_ENABLED);
	freemsg(mp);
	return (0);
}

/**
 * mx_connect_con: - process MX_CONNECT_CON primitive
 * @mx: private structure
 * @q: lower read queue
 * @mp: MX_CONNECT_CON message
 */
static __unlikely int
mx_connect_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_connect_con *p = (typeof(p)) mp->b_rptr;
	struct ch *ch;
	int err;

	for (ch = mx->e.list; ch; ch = ch->e.next)
		if (ch_get_state(ch) == CHS_WCON_CREQ)
			if ((err = ch_connect_con(ch, q, NULL, CH_BOTH, 0)))
				return (err);
	mx_set_state(mx, MXS_CONNECTED);
	freemsg(mp);
	return (0);
}

/**
 * mx_data_ind: - process MX_DATA_IND primitive
 * @mx: private structure
 * @q: lower read queue
 * @mp: MX_DATA_IND message
 */
static __unlikely int
mx_data_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_data_ind *p = (typeof(p)) mp->b_rptr;
}

/**
 * mx_sig_ind: - process MX_SIG_IND primitive
 * @mx: private structure
 * @q: lower read queue
 * @mp: MX_SIG_IND message
 */
static __unlikely int
mx_sig_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_sig_ind *p = (typeof(p)) mp->b_rptr;
}

/**
 * mx_disconnect_ind: - process MX_DISCONNECT_IND primitive
 * @mx: private structure
 * @q: lower read queue
 * @mp: MX_DISCONNECT_IND message
 */
static __unlikely int
mx_disconnect_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disconnect_ind *p = (typeof(p)) mp->b_rptr;
	struct ch *ch;
	int err;

	for (ch = mx->c.list; ch; ch = ch->c.next) {
		if (ch_get_state(ch) == CHS_CONNECTED)
			if ((err = ch_disconnect_ind(ch, q, NULL, CH_BOTH, 0)))
				return (err);
		if (ch_get_state(ch) == CHS_WCON_DREQ)
			if ((err = ch_disconnect_con(ch, q, NULL, CH_BOTH, 0)))
				return (err);
	}
	for (ch = mx->e.list; ch; ch = ch->e.next)
		if (ch_get_state(ch) == CHS_WCON_CREQ)
			if ((err = ch_disconnect_ind(ch, q, NULL, CH_BOTH, 0)))
				return (err);
	mx_set_state(mx, (mx->c.mask == 0) ? MXS_ENABLED : MXS_CONNECTED);
	freemsg(mp);
	return (0);
}

/**
 * mx_disconnect_con: - process MX_DISCONNECT_CON primitive
 * @mx: private structure
 * @q: lower read queue
 * @mp: MX_DISCONNECT_CON message
 */
static __unlikely int
mx_disconnect_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disconnect_con *p = (typeof(p)) mp->b_rptr;
	struct ch *ch;
	int err;

	for (ch = mx->c.list; ch; ch = ch->c.next)
		if (ch_get_state(ch) == CHS_WCON_DREQ)
			if ((err = ch_disconnect_con(ch, q, NULL, CH_BOTH, 0)))
				return (err);
	mx_set_state(mx, (mx->c.mask == 0) ? MXS_ENABLED : MXS_CONNECTED);
	freemsg(mp);
	return (0);
}

/**
 * mx_disable_ind: - process MX_DISABLE_IND primitive
 * @mx: private structure
 * @q: lower read queue
 * @mp: MX_DISABLE_IND message
 */
static __unlikely int
mx_disable_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disable_ind *p = (typeof(p)) mp->b_rptr;
	struct ch *ch;
	int err;

	for (ch = mx->e.list; ch; ch = ch->e.next) {
		if (ch_get_state(ch) == CHS_ENABLED)
			if ((err = ch_disable_ind(ch, q, NULL)))
				return (err);
		if (ch_get_state(ch) == CHS_WCON_RREQ)
			if ((err = ch_disable_con(ch, q, NULL)))
				return (err);
	}
	for (ch = mx->a.list; ch; ch = ch->a.next)
		if (ch_get_state(ch) == CHS_WCON_EREQ)
			if ((err = ch_disable_ind(ch, q, NULL)))
				return (err);
	mx_set_state(mx, MXS_ATTACHED);
	freemsg(mp);
	return (0);
}

/**
 * mx_disable_con: - process MX_DISABLE_CON primitive
 * @mx: private structure
 * @q: lower read queue
 * @mp: MX_DISABLE_CON message
 */
static __unlikely int
mx_disable_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disable_con *p = (typeof(p)) mp->b_rptr;
	struct ch *ch;
	int err;

	for (ch = mx->e.list; ch; ch = ch->e.next)
		if (ch_get_state(ch) == CHS_WCON_RREQ)
			if ((err = ch_disable_con(ch, q, NULL)))
				return (err);
	mx_set_state(mx, MXS_ATTACHED);
	freemsg(mp);
	return (0);
}

/**
 * mx_event_ind: - process MX_EVENT_IND primitive
 * @mx: private structure
 * @q: lower read queue
 * @mp: MX_EVENT_IND message
 */
static __unlikely int
mx_event_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_event_ind *p = (typeof(p)) mp->b_rptr;
}

/**
 * mx_other_ind: - process MX_OTHER_IND primitive
 * @mx: private structure
 * @q: lower read queue
 * @mp: MX_OTHER_IND message
 */
static __unlikely int
mx_other_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
}

/**
 * mx_r_data: - process M_DATA message block
 * @q: read queue
 * @mp: message
 *
 * When an RX block is received it should be queued.  It already contains all of the necessary data
 * and can be processed much later for data channels.  Channels are such that they must not miss
 * octets, but may delay them for significant periods.  The maximum delay is determined by the read
 * queue high water mark and service procedure latency.  From the service procedure, data is
 * transferred immediately out of the RX block into channel message blocks, which are passed
 * upstream.  Once the RX block has been emptied, data is copied from the TX block buffer to the
 * message block and it is passed downstream as transmit data.
 */
static inline fastcall int
mx_r_data(queue_t *q, mblk_t *mp)
{
	struct mx *mx;
	struct ch *ch;

	if (unlikely((mx = (struct mx *) mi_trylock(q)) == NULL))
		goto edeadlk;

	if (unlikely(mp->b_wptr < mp->b_rptr + mx->blocksize))
		goto discard;

	mx->rx.block++;

	/* trasnfer all data first */
	for (ch = mx->c.list; ch; ch = ch->c.next) {
		if ((ch->rx.nxt || (ch->rx.nxt = allocb(ch->blocksize, BPRI_MED)))
		    && canputnext(ch->oq)) {
			for (p = mp->b_rptr, frame = 0; frame < 8;
			     p += mx->spans * mx->channels, frame++)
				for (span = ch->span, q = p; span < ch->spans; q++, span++)
					for (mask = 0x1, chan = 0, r = q;
					     ch->mask & ~(mask - 1);
					     chan++, r += mx->spans, mask <<= 1)
						if (ch->mask & mask)
							*ch->rx.nxt->b_wptr++ = *r;
			if (DB_LIM(ch->rx.nxt) < ch->rx.nxt->b_wptr + ch->blocksize)
				putnext(ch->oq, XCHG(&ch->rx.nxt, NULL));
		} else {
			/* TODO: peg overruns */
		}
		ch->rx.block = mx->rx.block;
	}
	/* could likely be more efficient than this: i.e. transferring only necessary channels */
	bcopy(mp->b_rptr, mx->tx.buf, mx->blocksize);
	mi_unlock(mx);
	qreply(q, mp);
	return (0);

      discard:
	mi_unlock((caddr_t) mx);
	freemsg(mp);
	return (0);
      edeadlk:
	return (-EDEADLK);
      eagain:
	return (-EAGAIN);
}
static noinline fastcall __unlikely int
mx_r_data_slow(queue_t *q, mblk_t *mp)
{
	return mx_r_data(q, mp);
}
static fastcall int
mx_r_proto(queue_t *q, mblk_t *mp)
{
	struct mx *mx;
	int rtn = -EDEADLK;

	if ((mx = (struct mx *) mi_trylock(q)) != NULL) {
		mx_ulong oldstate = mx_get_state(mx);

		switch (*(mx_ulong *) mp->b_rptr) {
		case MX_INFO_ACK:
			mi_strlog(q, 0, SL_TRACE, "MX_INFO_ACK <-");
			rtn = mx_info_ack(mx, q, mp);
			break;
		case MX_OPTMGMT_ACK:
			mi_strlog(q, 0, SL_TRACE, "MX_OPTMGMT_ACK <-");
			rtn = mx_optmgmt_ack(mx, q, mp);
			break;
		case MX_OK_ACK:
			mi_strlog(q, 0, SL_TRACE, "MX_OK_ACK <-");
			rtn = mx_ok_ack(mx, q, mp);
			break;
		case MX_ERROR_ACK:
			mi_strlog(q, 0, SL_TRACE, "MX_ERROR_ACK <-");
			rtn = mx_error_ack(mx, q, mp);
			break;
		case MX_ENABLE_CON:
			mi_strlog(q, 0, SL_TRACE, "MX_ENABLE_CON <-");
			rtn = mx_enable_con(mx, q, mp);
			break;
		case MX_CONNECT_CON:
			mi_strlog(q, 0, SL_TRACE, "MX_CONNECT_CON <-");
			rtn = mx_connect_con(mx, q, mp);
			break;
		case MX_DATA_IND:
			mi_strlog(q, 0, SL_TRACE, "MX_DATA_IND <-");
			rtn = mx_data_ind(mx, q, mp);
			break;
		case MX_SIG_IND:
			mi_strlog(q, 0, SL_TRACE, "MX_SIG_IND <-");
			rtn = mx_sig_ind(mx, q, mp);
			break;
		case MX_DISCONNECT_IND:
			mi_strlog(q, 0, SL_TRACE, "MX_DISCONNECT_IND <-");
			rtn = mx_disconnect_ind(mx, q, mp);
			break;
		case MX_DISCONNECT_CON:
			mi_strlog(q, 0, SL_TRACE, "MX_DISCONNECT_CON <-");
			rtn = mx_disconnect_con(mx, q, mp);
			break;
		case MX_DISABLE_IND:
			mi_strlog(q, 0, SL_TRACE, "MX_DISABLE_IND <-");
			rtn = mx_disable_ind(mx, q, mp);
			break;
		case MX_DISABLE_CON:
			mi_strlog(q, 0, SL_TRACE, "MX_DISABLE_CON <-");
			rtn = mx_disable_con(mx, q, mp);
			break;
		case MX_EVENT_IND:
			mi_strlog(q, 0, SL_TRACE, "MX_EVENT_IND <-");
			rtn = mx_event_ind(mx, q, mp);
			break;
		default:
			mi_strlog(q, 0, SL_TRACE, "MX_????_??? <-");
			rtn = mx_other_ind(mx, q, mp);
			break;
		}
		if (rtn)
			mx_set_state(mx, oldstate);
		mi_unlock((caddr_t) mx);
	}
	return (rtn);
}
static fastcall int
mx_r_ioctl(queue_t *q, mblk_t *mp)
{
}
static fastcall int
mx_r_ctl(queue_t *q, mblk_t *mp)
{
}
static fastcall int
mx_r_sig(queue_t *q, mblk_t *mp)
{
}
static fastcall int
mx_r_flush(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	queue_t *oq;

	if (mp->b_rptr[0] & FLUSHR) {
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		if (mx->other && mx->other->oq) {
			if (!(oq->q_flag & QREADR)) {
				/* need to reverse sense of flush */
				switch (mp->b_rptr[0] & FLUSHRW) {
				case FLUSHR:
					mp->b_rptr[0] &= ~FLUSHRW;
					mp->b_rptr[0] |= FLUSHW;
					break;
				case FLUSHW:
					mp->b_rptr[0] &= ~FLUSHRW;
					mp->b_rptr[0] |= FLUSHR;
					break;
				}
			}
			putnext(oq, mp);
			return (0);
		}
		mp->b_rptr[0] &= ~FLUSHR;
	}
	if (mp->b_rptr[0] & FLUSHW) {
		if (mx->other && (oq = mx->other->oq)) {
			if (!(oq->q_flag & QREADR)) {
				/* need to reverse sense of flush */
				switch (mp->b_rptr[0] & FLUSHRW) {
				case FLUSHR:
					mp->b_rptr[0] &= ~FLUSHRW;
					mp->b_rptr[0] |= FLUSHW;
					break;
				case FLUSHW:
					mp->b_rptr[0] &= ~FLUSHRW;
					mp->b_rptr[0] |= FLUSHR;
					break;
				}
			}
			putnext(oq, mp);
			return (0);
		}
		if (mp->b_rptr[0] & FLUSHBAND)
			flushband(_WR(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(_WR(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}
static fastcall int
mx_r_other(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);
	queue_t *oq;

	/* pass along with flow control */
	if (mx->other && (oq = mx->other->oq)) {
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(oq, mp->b_band)) {
			putnext(oq, mp);
			return (0);
		}
		return (-EBUSY);
	}
	freemsg(mp);
	return (0);
}

static noinline fastcall int
ch_w_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return ch_w_data_slow(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return ch_w_proto(q, mp);
	case M_IOCTL:
		return ch_w_ioctl(q, mp);
	case M_IOCDATA:
		return ch_w_iocdata(q, mp);
	case M_CTL:
	case M_PCCTL:
		return ch_w_ctl(q, mp);
	case M_FLUSH:
		return ch_w_flush(q, mp);
	}
	return ch_w_other(q, mp);
}
static inline fastcall int
ch_w_msg(queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_DATA)
		return ch_w_data(q, mp);
	return ch_w_msg_slow(q, mp);
}

static inline fastcall int
mx_r_msg_slow(queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_DATA:
		return mx_r_data_slow(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return mx_r_proto(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
	case M_IOCACK:
	case M_IOCNAK:
		return mx_r_ioctl(q, mp);
	case M_CTL:
	case M_PCCTL:
		return mx_r_ctl(q, mp);
	case M_SIG:
	case M_PCSIG:
		return mx_r_sig(q, mp);
	case M_FLUSH:
		return mx_r_flush(q, mp);
	}
	return mx_r_other(q, mp);
}
static noinline fastcall int
mx_r_msg(queue_t *q, mblk_t *mp)
{
	if (DB_TYPE(mp) == M_DATA)
		return mx_r_data(q, mp);
	return mx_r_msg_slow(q, mp);
}

static streamscall int
ch_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || ch_w_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int
ch_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (ch_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}
static streamscall int
ch_rput(queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);

	mi_strlog(q, 0, SL_ERROR, "software error: %s", __FUNCTION__);
	putnext(q, mp);
	return (0);
}

static streamscall int
mx_wput(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);

	mi_strlog(q, 0, SL_ERROR, "software error: %s", __FUNCTION__);
	putnext(q, mp);
	return (0);
}
static streamscall int
mx_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || mx_r_msg(q, mp))
		putq(q, mp);
	return (0);
}
static streamscall int
mx_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q))) {
		if (mx_r_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	}
	return (0);
}

static caddr_t ch_opens = NULL;

static streamscall int
ch_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	minor_t cminor = getminor(*devp);
	struct ch *ch;
	int err;

	if (q->q_ptr)
		return (0);	/* already open */
	if (!cminor)
		sflag = CLONEOPEN;
	if (sflag == CLONEOPEN)
		/* start half way through minor device range for clone devices */
		cminor = (getminor(makedevice(0, 0xffffffff)) >> 1) + 1;
	if ((err = mi_open_comm(&ch_opens, sizeof(*ch), q, devp, oflags, sflag, crp)))
		return (err);
	ch = CH_PRIV(q);
	/* intialize ch structure */
	return (0);
}
static streamscall int
ch_qclose(queue_t *q, int oflags, cred_t *crp)
{
	qprocsoff(q);
	mi_close_comm(&ch_opens, q);
	return (0);
}

static struct module_info ch_minfo = {
	.mi_idnum = DRV_ID,		/* Driver module id number */
	.mi_idname = DRV_NAME,		/* Driver name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat ch_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit ch_rinit = {
	.qi_putp = &ch_rput,		/* Read put (message from below) */
	.qi_qopen = &ch_qopen,		/* Each open */
	.qi_qclose = &ch_qclose,	/* Last close */
	.qi_minfo = &ch_minfo,		/* Information */
	.qi_mstat = &ch_mstat,		/* Statistics */
};

static struct qinit ch_winit = {
	.qi_putp = &ch_wput,		/* Write put (message from above) */
	.qi_srvp = &ch_wsrv,		/* Write queue service */
	.qi_minfo = &ch_minfo,		/* Information */
	.qi_mstat = &ch_mstat,		/* Statistics */
};

static struct module_info mx_minfo = {
	.mi_idnum = DRV_ID,		/* Driver module id number */
	.mi_idname = DRV_NAME,		/* Driver name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

static struct module_stat mx_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit mx_rinit = {
	.qi_putp = &mx_rput,		/* Read put (message from below) */
	.qi_srvp = &mx_rsrv,		/* Read queue service */
	.qi_minfo = &mx_minfo,		/* Information */
	.qi_mstat = &mx_mstat,		/* Statistics */
};

static struct qinit mx_winit = {
	.qi_putp = &mx_wput,		/* Write put (message from above) */
	.qi_minfo = &mx_minfo,		/* Information */
	.qi_mstat = &mx_mstat,		/* Statistics */
};

static struct streamtab mx_info = {
	.st_rdinit = &ch_rinit,		/* Upper multiplex read queue */
	.st_wrinit = &ch_winit,		/* Upper multiplex write queue */
	.st_muxrinit = &mx_rinit,	/* Lower multiplex read queue */
	.st_muxwinit = &mx_winit,	/* Lower multiplex write queue */
};

static struct cdevsw ch_cdev = {
	.d_name = DRV_NAME,		/* Driver name */
	.d_str = &mx_info,		/* Driver streamtab */
	.d_flag = D_MP,			/* Driver flags */
	.d_fop = NULL,			/* Driver operations (NULL for stream head) */
	.d_mode = S_IFCHR,		/* Device type */
	.d_kmod = THIS_MODULE,		/* Module owner */
};
static __init int
chmuxinit(void)
{
	int err;

	if ((err = register_strdev(&ch_cdev, major)) < 0) {
		cmn_err(CE_WARN, "could not register STREAMS device, err = %d", err);
		return (err);
	}
	if (major == 0)
		major = err;
	return (0);
}
static __exit void
chmuxexit(void)
{
	int err;

	if ((err = unregsiter_strdev(&ch_cdev, major)) < 0)
		cmn_err(CE_WARN, "could not register STREAMS device, err = %d", err);
	return;
}

module_init(chmuxinit);
module_exit(chmuxexit);
