/*****************************************************************************

 @(#) $RCSfile: dl-llc.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:56 $

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

 Last Modified $Date: 2008-06-13 06:43:56 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dl-llc.c,v $
 Revision 0.9.2.1  2008-06-13 06:43:56  brian
 - added files

 *****************************************************************************/

#ident "@(#) $RCSfile: dl-llc.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:56 $"

static char const ident[] = "$RCSfile: dl-llc.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:56 $";

/*
 * This is a DLPI interface LLC driver for Linux.  What it does is provides DLPI access to the Linux
 * interface devices, such as Ethernet or FDDI.  There are several types of encapsulation that can
 * be performed and this driver supports a number of them.
 *
 * When a DLPI stream is opened on this driver it is an unattached Style 2 stream.  The Stream must
 * be attached to a PPA (Physical Point of Appearance) before it can be bound and messages
 * exchanged.  The information reported in DL_INFO_ACK will be meaningless until the Stream is
 * attached to a PPA.  This driver uses a specialized PPA addressing scheme which is based on the
 * level of access to the MAC device that is desired (in terms of framing and station or combined
 * station level), and the Linux device index number.  An input-output control method is provided
 * for translating between interface names (e.g., "eth0") and Linux interface index numbers.
 *
 * To make the PPA also compatible with other systems, in that the PPA is simply an index number,
 * this driver provides clone minor device numbers that provide framing specific interfaces and also
 * supports some of the AIX intput-output controls that will adjust the framing.
 *
 * The purpose of this driver is to provide IEEE 802.2 LLC access for SNA, X.25 and OSI CONS and
 * CLNS.  The driver is a non-multiplexing pseudo-device driver.  If you have DLPI MAC drivers for
 * specific interface cards, uses the drivers from the strxns package to first make them available
 * to Linux.
 *
 * This driver also supports some of the AIX mechanisms, input-output controls and specifics from
 * the GDLC based drivers DLC8023, DLCETHER, DLCTOKEN, DLCFDDI, but not DLCSDLC or DLCQLLC.  The
 * latter are provided by a separate hdlc driver.
 */

#define _MPS_SOURCE

#include <sys/os7/compat.h>	/* STREAMS compatibility header */
#include <sys/strsun.h>		/* some SUN goodies */
#include <sys/dlpi.h>		/* the DLPI Revision 2.0.0 interface */

#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <net/protocol.h>

#define LLC_DESCRIP	"UNIX SVR 4.2 LLC DRIVER FOR LINUX FAST-STREAMS"
#define LLC_EXTRA	"Part of the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define LLC_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define LLC_REVISION	"OpenSS7 $RCSfile: dl-llc.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-13 06:43:56 $"
#define LLC_DEVICE	"SVR 4.2MP IEEE 802.2 LLC Driver (LLC)"
#define LLC_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define LLC_LICENSE	"GPL"
#define LLC_BANNER	LLC_DESCRIP	"\n" \
			LLC_EXTRA	"\n" \
			LLC_COPYRIGHT	"\n" \
			LLC_REVISION	"\n" \
			LLC_DEVICE	"\n" \
			LLC_CONTACT	"\n"
#define LLC_SPLASH	LLC_DESCRIP	" - " \
			LLC_REVISION

#ifndef CONFIG_STREAMS_LLC_NAME
#error "CONFIG_STREAMS_LLC_NAME must be defined."
#endif				/* CONFIG_STREAMS_LLC_NAME */
#ifndef CONFIG_STREAMS_LLC_MODID
#error "CONFIG_STREAMS_LLC_MODID must be defined."
#endif				/* CONFIG_STREAMS_LLC_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(LLC_CONTACT);
MODULE_DESCRIPTION(LLC_DESCRIP);
MODULE_SUPPORTED_DEVICE(LLC_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(LLC_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_LLC_MODID));
MODULE_ALIAS("streams-driver-llc");
MODULE_ALIAS("streams-module-llc");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_LLC_MAJOR));
MODULE_ALIAS("/dev/streams/llc");
MODULE_ALIAS("/dev/streams/llc/*");
MODULE_ALIAS("/dev/streams/llc/eth");
MODULE_ALIAS("/dev/streams/llc/llc");
MODULE_ALIAS("/dev/streams/llc/tpb");
MODULE_ALIAS("/dev/streams/llc/tpr");
MODULE_ALIAS("/dev/streams/llc/fddi");
MODULE_ALIAS("/dev/streams/clone/llc");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0));
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0) "-1");
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0) "-2");
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0) "-3");
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0) "-4");
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0) "-5");
MODULE_ALIAS("char-major-" __stringify(LLC_MAJOR_0) "-" __stringify(LLC_CMINOR));
MODULE_ALIAS("/dev/eth");
MODULE_ALIAS("/dev/llc");
MODULE_ALIAS("/dev/tpb");
MODULE_ALIAS("/dev/tpr");
MODULE_ALIAS("/dev/fddi");
#endif				/* MODULE_ALIAS */
#endif				/* MODULE */
#endif				/* LINUX */

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS DEFINITIONS
 *
 * --------------------------------------------------------------------------
 */

#define DRV_ID	    LLC_DRV_ID
#define DRV_NAME    LLC_DRV_NAME
#define CMAJORS	    LLC_CMAJORS
#define CMAJOR_0    LLC_CMAJOR_0
#define UNITS	    LLC_UNITS
#ifdef MODULE
#define DRV_BANNER  LLC_BANNER
#else				/* MODULE */
#define DRV_BANNER  LLC_SPLASH
#endif				/* MODULE */

static struct module_info dl_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat dl_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat dl_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/*
 * --------------------------------------------------------------------------
 *
 * PRIVATE STRUCTURE
 *
 * --------------------------------------------------------------------------
 */

struct dl {
	queue_t *oq;
	dl_ulong state;
	dl_ulong oldstate;
	struct {
		dl_info_ack_t info;
		struct {
			dl_qos_co_sel1_t qos;
			dl_qos_co_range1_t qor;
		} co;
		struct {
			dl_qos_cl_sel1_t qos;
			dl_qos_cl_range1_t qor;
		} cl;
	} proto;
	dl_ulong lsap;
};

#define DL_PRIV(q) ((struct dl *)q->q_ptr)

#define STRLOGIO    0		/* input-output controls */
#define STRLOGST    1		/* state transitions */
#define STRLOGTO    2		/* timeouts */
#define STRLOGRX    3		/* primitives received */
#define STRLOGTX    4		/* primitives issued */
#define STRLOGTE    5		/* timer events */
#define STRLOGDA    6		/* data */

static inline const char *
dl_iocname(int cmd)
{
	switch (cmd) {
	default:
		return ("(unknown)");
	}
}

static inline const char *
dl_primname(dl_ulong prim)
{
	switch (prim) {
	case DL_INFO_REQ:
		return ("DL_INFO_REQ");
	case DL_INFO_ACK:
		return ("DL_INFO_ACK");
	case DL_ATTACH_REQ:
		return ("DL_ATTACH_REQ");
	case DL_DETACH_REQ:
		return ("DL_DETACH_REQ");
	case DL_BIND_REQ:
		return ("DL_BIND_REQ");
	case DL_BIND_ACK:
		return ("DL_BIND_ACK");
	case DL_UNBIND_REQ:
		return ("DL_UNBIND_REQ");
	case DL_OK_ACK:
		return ("DL_OK_ACK");
	case DL_ERROR_ACK:
		return ("DL_ERROR_ACK");
	case DL_SUBS_BIND_REQ:
		return ("DL_SUBS_BIND_REQ");
	case DL_SUBS_BIND_ACK:
		return ("DL_SUBS_BIND_ACK");
	case DL_SUBS_UNBIND_REQ:
		return ("DL_SUBS_UNBIND_REQ");
	case DL_ENABMULTI_REQ:
		return ("DL_ENABMULTI_REQ");
	case DL_DISABMULTI_REQ:
		return ("DL_DISABMULTI_REQ");
	case DL_PROMISCON_REQ:
		return ("DL_PROMISCON_REQ");
	case DL_PROMISCOFF_REQ:
		return ("DL_PROMISCOFF_REQ");
	case DL_UNITDATA_REQ:
		return ("DL_UNITDATA_REQ");
	case DL_UNITDATA_IND:
		return ("DL_UNITDATA_IND");
	case DL_UDERROR_IND:
		return ("DL_UDERROR_IND");
	case DL_UDQOS_REQ:
		return ("DL_UDQOS_REQ");
	case DL_CONNECT_REQ:
		return ("DL_CONNECT_REQ");
	case DL_CONNECT_IND:
		return ("DL_CONNECT_IND");
	case DL_CONNECT_RES:
		return ("DL_CONNECT_RES");
	case DL_CONNECT_CON:
		return ("DL_CONNECT_CON");
	case DL_TOKEN_REQ:
		return ("DL_TOKEN_REQ");
	case DL_TOKEN_ACK:
		return ("DL_TOKEN_ACK");
	case DL_DISCONNECT_REQ:
		return ("DL_DISCONNECT_REQ");
	case DL_DISCONNECT_IND:
		return ("DL_DISCONNECT_IND");
	case DL_RESET_REQ:
		return ("DL_RESET_REQ");
	case DL_RESET_IND:
		return ("DL_RESET_IND");
	case DL_RESET_RES:
		return ("DL_RESET_RES");
	case DL_RESET_CON:
		return ("DL_RESET_CON");
	case DL_DATA_ACK_REQ:
		return ("DL_DATA_ACK_REQ");
	case DL_DATA_ACK_IND:
		return ("DL_DATA_ACK_IND");
	case DL_DATA_ACK_STATUS_IND:
		return ("DL_DATA_ACK_STATUS_IND");
	case DL_REPLY_REQ:
		return ("DL_REPLY_REQ");
	case DL_REPLY_IND:
		return ("DL_REPLY_IND");
	case DL_REPLY_STATUS_IND:
		return ("DL_REPLY_STATUS_IND");
	case DL_REPLY_UPDATE_REQ:
		return ("DL_REPLY_UPDATE_REQ");
	case DL_REPLY_UPDATE_STATUS_IND:
		return ("DL_REPLY_UPDATE_STATUS_IND");
	case DL_XID_REQ:
		return ("DL_XID_REQ");
	case DL_XID_IND:
		return ("DL_XID_IND");
	case DL_XID_RES:
		return ("DL_XID_RES");
	case DL_XID_CON:
		return ("DL_XID_CON");
	case DL_TEST_REQ:
		return ("DL_TEST_REQ");
	case DL_TEST_IND:
		return ("DL_TEST_IND");
	case DL_TEST_RES:
		return ("DL_TEST_RES");
	case DL_TEST_CON:
		return ("DL_TEST_CON");
	case DL_PHYS_ADDR_REQ:
		return ("DL_PHYS_ADDR_REQ");
	case DL_PHYS_ADDR_ACK:
		return ("DL_PHYS_ADDR_ACK");
	case DL_SET_PHYS_ADDR_REQ:
		return ("DL_SET_PHYS_ADDR_REQ");
	case DL_GET_STATISTICS_REQ:
		return ("DL_GET_STATISTICS_REQ");
	case DL_GET_STATISTICS_ACK:
		return ("DL_GET_STATISTICS_ACK");
	default:
		return ("(unknown)");
	}
}

static inline const char *
dl_statename(dl_ulong state)
{
	switch (state) {
	case DL_UNATTACHED:
		return ("DL_UNATTACHED");
	case DL_ATTACH_PENDING:
		return ("DL_ATTACH_PENDING");
	case DL_DETACH_PENDING:
		return ("DL_DETACH_PENDING");
	case DL_UNBOUND:
		return ("DL_UNBOUND");
	case DL_BIND_PENDING:
		return ("DL_BIND_PENDING");
	case DL_UNBIND_PENDING:
		return ("DL_UNBIND_PENDING");
	case DL_IDLE:
		return ("DL_IDLE");
	case DL_UDQOS_PENDING:
		return ("DL_UDQOS_PENDING");
	case DL_OUTCON_PENDING:
		return ("DL_OUTCON_PENDING");
	case DL_INCON_PENDING:
		return ("DL_INCON_PENDING");
	case DL_CONN_RES_PENDING:
		return ("DL_CONN_RES_PENDING");
	case DL_DATAXFER:
		return ("DL_DATAXFER");
	case DL_USER_RESET_PENDING:
		return ("DL_USER_RESET_PENDING");
	case DL_PROV_RESET_PENDING:
		return ("DL_PROV_RESET_PENDING");
	case DL_RESET_RES_PENDING:
		return ("DL_RESET_RES_PENDING");
	case DL_DISCON8_PENDING:
		return ("DL_DISCON8_PENDING");
	case DL_DISCON9_PENDING:
		return ("DL_DISCON9_PENDING");
	case DL_DISCON11_PENDING:
		return ("DL_DISCON11_PENDING");
	case DL_DISCON12_PENDING:
		return ("DL_DISCON12_PENDING");
	case DL_DISCON13_PENDING:
		return ("DL_DISCON13_PENDING");
	case DL_SUBS_BIND_PND:
		return ("DL_SUBS_BIND_PND");
	case DL_SUBS_UNBIND_PND:
		return ("DL_SUBS_UNBIND_PND");
	default:
		return ("(unknown)");
	}
}

static inline const char *
dl_errname(dl_long error)
{
	if (error < 0)
		return ("DL_SYSERR");
	switch (error) {
	case DL_ACCESS:
		return ("DL_ACCESS");
	case DL_BADADDR:
		return ("DL_BADADDR");
	case DL_BADCORR:
		return ("DL_BADCORR");
	case DL_BADDATA:
		return ("DL_BADDATA");
	case DL_BADPPA:
		return ("DL_BADPPA");
	case DL_BADPRIM:
		return ("DL_BADPRIM");
	case DL_BADQOSPARAM:
		return ("DL_BADQOSPARAM");
	case DL_BADQOSTYPE:
		return ("DL_BADQOSTYPE");
	case DL_BADSAP:
		return ("DL_BADSAP");
	case DL_BADTOKEN:
		return ("DL_BADTOKEN");
	case DL_BOUND:
		return ("DL_BOUND");
	case DL_INITFAILED:
		return ("DL_INITFAILED");
	case DL_NOADDR:
		return ("DL_NOADDR");
	case DL_NOTINIT:
		return ("DL_NOTINIT");
	case DL_OUTSTATE:
		return ("DL_OUTSTATE");
	case DL_SYSERR:
		return ("DL_SYSERR");
	case DL_UNSUPPORTED:
		return ("DL_UNSUPPORTED");
	case DL_UNDELIVERABLE:
		return ("DL_UNDELIVERABLE");
	case DL_NOTSUPPORTED:
		return ("DL_NOTSUPPORTED");
	case DL_TOOMANY:
		return ("DL_TOOMANY");
	case DL_NOTENAB:
		return ("DL_NOTENAB");
	case DL_BUSY:
		return ("DL_BUSY");
	case DL_NOAUTO:
		return ("DL_NOAUTO");
	case DL_NOXIDAUTO:
		return ("DL_NOXIDAUTO");
	case DL_NOTESTAUTO:
		return ("DL_NOTESTAUTO");
	case DL_XIDAUTO:
		return ("DL_XIDAUTO");
	case DL_TESTAUTO:
		return ("DL_TESTAUTO");
	case DL_PENDING:
		return ("DL_PENDING");
	default:
		return ("(unknown)");
	}
}

static inline const char *
dl_strerror(dl_long error)
{
	if (error < 0)
		return ("UNIX system error occurred.");
	switch (error) {
	case DL_ACCESS:
		return ("Improper permissions for request.");
	case DL_BADADDR:
		return ("DLSAP addr in improper format or invalid.");
	case DL_BADCORR:
		return ("Seq number not from outstand DL_CONN_IND.");
	case DL_BADDATA:
		return ("User data exceeded provider limit.");
	case DL_BADPPA:
		return ("Specified PPA was invalid.");
	case DL_BADPRIM:
		return ("Primitive received not known by provider.");
	case DL_BADQOSPARAM:
		return ("QOS parameters contained invalid values.");
	case DL_BADQOSTYPE:
		return ("QOS structure type is unknown/unsupported.");
	case DL_BADSAP:
		return ("Bad LSAP selector.");
	case DL_BADTOKEN:
		return ("Token used not an active stream.");
	case DL_BOUND:
		return ("Attempted second bind with dl_max_conind.");
	case DL_INITFAILED:
		return ("Physical Link initialization failed.");
	case DL_NOADDR:
		return ("Provider couldn't allocate alt. address.");
	case DL_NOTINIT:
		return ("Physical Link not initialized.");
	case DL_OUTSTATE:
		return ("Primitive issued in improper state.");
	case DL_SYSERR:
		return ("UNIX system error occurred.");
	case DL_UNSUPPORTED:
		return ("Requested serv. not supplied by provider.");
	case DL_UNDELIVERABLE:
		return ("Previous data unit could not be delivered.");
	case DL_NOTSUPPORTED:
		return ("Primitive is known but not supported.");
	case DL_TOOMANY:
		return ("Limit exceeded.");
	case DL_NOTENAB:
		return ("Promiscuous mode not enabled.");
	case DL_BUSY:
		return ("Other streams for PPA in post-attached state.");
	case DL_NOAUTO:
		return ("Automatic handling of XID and TEST response not supported. .");
	case DL_NOXIDAUTO:
		return ("Automatic handling of XID not supported.");
	case DL_NOTESTAUTO:
		return ("Automatic handling of TEST not supported.");
	case DL_XIDAUTO:
		return ("Automatic handling of XID response.");
	case DL_TESTAUTO:
		return ("Automatic handling of TEST response.");
	case DL_PENDING:
		return ("pending outstanding connect indications.");
	default:
		return ("(unknown)");
	}
}

static dl_ulong
dl_get_state(struct dl *dl)
{
	return (dl->state);
}

static dl_ulong
dl_set_state(struct dl *dl, dl_ulong newstate)
{
	dl_ulong oldstate = dl->state;

	if (newstate != oldstate) {
		dl->state = newstate;
		dl->proto.info.dl_current_state = newstate;
		mi_strlog(dl->oq, STRLOGST, SL_TRACE, "%s <- %s", dl_statename(newstate),
			  dl_statename(oldstate));
	}
	return (newstate);
}

static dl_ulong
dl_save_state(struct dl *dl)
{
	return ((dl->oldstate = dl_get_state(dl)));
}

static dl_ulong
dl_restore_state(struct dl *dl)
{
	return (dl_set_state(dl, dl->oldstate));
}

/*
 * --------------------------------------------------------------------------
 *
 * MAC AND LLC SENT MESSAGES
 *
 * --------------------------------------------------------------------------
 */

#define LLC_MT_RR	0x01	/* 0000 0001 */
#define LLC_MT_RNR	0x05	/* 0000 0101 */
#define LLC_MT_REJ	0x09	/* 0000 1001 */
#define LLC_MT_I	0x00	/* 0000 0000 */
#define LLC_MT_UI	0x03	/* 0000 0011 */
#define LLC_MT_SABM	0x2f	/* 0010 1111 */
#define LLC_MT_SABME	0x6f	/* 0110 1111 */
#define LLC_MT_SM	0xc3	/* 1100 0011 */
#define LLC_MT_DISC	0x43	/* 0100 0011 */
#define LLC_MT_DM	0x0f	/* 0000 1111 */
#define LLC_MT_UA	0x63	/* 0110 0011 */
#define LLC_MT_FRMR	0x87	/* 1000 0111 */
#define LLC_MT_XID	0xaf	/* 1010 1111 */
#define LLC_MT_TEST	0xe3	/* 1110 0011 */

#undef skbuff_head_cache
#ifdef HAVE_SKBUFF_HEAD_CACHE_ADDR
#define skbuff_head_cache (*((kmem_cachep_t *) HAVE_SKBUFF_HEAD_CACHE_ADDR))
#endif

/* NOTE -- IEEE 802.2 LLC only really supports SABME (extended, modulo 128) mode */

static fastcall noinline __unlikely struct sk_buff *
llc_alloc_skb_slow(struct llcdev *dev, mblk_t *mp, uint headroom, int gfp)
{
	struct sk_buff *skb;
	uint dlen = mp ? msgsize(mp) : 0;

	if (likely(!!(skb = alloc_skb(headroom + dlen, GFP_ATOMIC)) != NULL)) {
		skb_reserve(skb, headroom);
		{
			uchar *data;
			mblk_t *b;
			int blen;

			data = skb_put(skb, dlen);
			for (b = mp; b; b = b->b_cont) {
				if ((blen = MBLKL(b)) > 0) {
					bcopy(b->b_rptr, data, blen);
					data += blen;
				}
			}
		}
		freemsg(mp);
	}
	return (skb);
}

#ifdef LFS
static fastcall noinline __unlikely struct sk_buff *
llc_alloc_skb_old(struct llcdev *dev, mblk_t *mp, uint headroom, int gfp)
{
	struct sk_buff *skb;
	uchar *beg, *end;

	/* must not be a fastbuf */
	if (unlikely(DB_SIZE(mp) <= FASTBUF))
		goto go_slow;
	/* must not be esballoc'ed */
	if (unlikely(DB_FRTNP(mp) != NULL))
		goto go_slow;
	/* must be only one with reference (for now) */
	if (unlikely(DB_REF(mp) > 1))
		goto go_slow;

	/* First, check if there is enough head room on the data block */
	beg = mp->b_rptr - headroom;
	if (unlikely(beg < DB_BASE(mp)))
		goto go_frag;
	end = (uchar *) (((ulong) mp->b_wptr + (SMP_CACHE_BYTES - 1)) & ~(SMP_CACHE_BYTES - 1));
	if (unlikely(end + sizeof(struct skb_shared_info) > DB_LIM(mp)))
		goto go_frag;

	/* Last, allocate a socket buffer header and point it to the payload data. */
	skb = kmem_cache_alloc(skbuff_head_cache, gfp);
	if (unlikely(skb == NULL))
		goto no_skb;

	memset(skb, 0, offsetof(struct sk_buff, truesize));
	skb->truesize = end - beg + sizeof(struct sk_buff);
	atomic_set(&skb->users, 1);
	skb->head = DB_BASE(mp);
	skb->data = mp->b_rptr;
	skb_set_tail_pointer(skb, mp->b_wptr - mp->b_rptr);
#ifdef NET_SKBUFF_DATA_USES_OFFSET
	skb->end = end - skb->head;
#else				/* NET_SKBUFF_DATA_USES_OFFSET */
	skb->end = end;
#endif				/* NET_SKBUFF_DATA_USES_OFFSET */
	skb->len = MBLKL(mp);
	skb->cloned = 0;
	skb->data_len = 0;
	/* intialize shared data structure */
	memset(skb_shinfo(skb), 0, sizeof(struct skb_shared_info));
	atomic_set(&(skb_shinfo(skb)->dataref), 1);
	/* need to release message block and data block without releasing buffer */
	/* point into internal buffer */
	DB_FRTNP(mp) = (struct free_rtn *)
	    ((struct mdbblock *) ((struct mbinfo *) mp->b_datap - 1))->databuf;
	/* override with dummy free routine */
	DB_FRTNP(mp)->free_func = NULL;
	DB_FRTNP(mp)->free_arg = NULL;
	freemsg(mp);		/* give it up */
      no_skb:
	return (skb);
      go_frag:			/* for now */
      go_slow:
	return llc_alloc_skb_slow(dev, mp, headroom, gfp);
}

static inline fastcall __hot_out struct sk_buff *
llc_alloc_skb(struct llcdev *dev, mblk_t *mp, uint headroom, int gfp)
{
	struct sk_buff *skb;

	if (unlikely(mp == NULL))
		goto go_slow;
	if (unlikely((DB_FLAG(mp) & (DB_SKBUFF)) == 0))
		goto old_way;
	if (unlikely(MBLKHEAD(mp) < headroom))
		goto go_slow;
	if (unlikely(!(skb = (typeof(skb)) DB_FRTNP(mp)->free_arg)))
		goto go_slow;
	skb_get(skb);
	skb_reserve(skb, mp->b_rptr - skb->data);
	skb_put(skb, MBLKL(mp));
	freemsg(mp);
	return (skb);
      old_way:
	return llc_alloc_skb_old(dev, mp, headroom, gfp);
      go_slow:
	return llc_alloc_skb_slow(dev, mp, headroom, gfp);
}
#else				/* LFS */
static inline fastcall __hot_out struct sk_buff *
llc_alloc_skb(struct llcdev *dev, mblk_t *mp, uint headroom, int gfp)
{
	return llc_alloc_skb_slow(dev, mp, headroom, gfp);
}
#endif				/* LFS */

static fastcall noinline __unlikely int
llc_snd_I_frame(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap,
		uchar cr, uchar pf, uint nr, uint ns, mblk_t *dp)
{
	struct sk_buff *skb;
	size_t hlen = ((dev->hard_header_len + 15) & ~15) + 2 + 2;
	uchar *ptr;

	if (likely(!(netif_queue_stopped(dev)))) {
		if (likely(!!(skb = llc_alloc_skb(dev, dp, hlen, GFP_ATOMIC)))) {
			/* find headers */
			__skb_push(skb, 2 + 2);
			skb_reset_network_header(skb);

			skb->dev = dev;
			skb->dst = NULL;	/* XXX */
			skb->priority = 0;	/* XXX */

			ptr = (typeof(ptr)) skb_network_header(skb);
			ptr[0] = (dsap << 1) | gr;
			ptr[1] = (ssap << 1) | cr;
			ptr[2] = (ns << 1);
			ptr[3] = (nr << 1) | pf;
			dev_queue_xmit(skb);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

static fastcall noinline __unlikely int
llc_snd_S_frame(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap, uchar cr,
		uchar pf, uint nr, uint mtype)
{
	struct sk_buff *skb;
	size_t hlen = ((dev->hard_header_len + 15) & ~15) + 2 + 2;
	uchar *ptr;

	if (likely(!(netif_queue_stopped(dev)))) {
		if (likely(!!(skb = llc_alloc_skb(dev, NULL, hlen, GFP_ATOMIC)))) {
			/* find headers */
			__skb_push(skb, 2 + 2);
			skb_reset_network_header(skb);

			skb->dev = dev;
			skb->dst = NULL;	/* XXX */
			skb->priority = 0;	/* XXX */

			ptr = (typeof(ptr)) skb_network_header(skb);
			ptr[0] = (dsap << 1) | gr;
			ptr[1] = (ssap << 1) | cr;
			ptr[2] = mtype;
			ptr[3] = (nr << 1) | pf;
			dev_queue_xmit(skb);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

static fastcall noinline __unlikely int
llc_snd_U_frame(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap,
		uchar cr, uchar pf, mblk_t *dp, uint mtype)
{
	struct sk_buff *skb;
	size_t hlen = ((dev->hard_header_len + 15) & ~15) + 2 + 1;
	uchar *ptr;

	if (likely(!(netif_queue_stopped(dev)))) {
		if (likely(!!(skb = llc_alloc_skb(dl, dp, hlen, GFP_ATOMIC)))) {
			/* find headers */
			__skb_push(skb, 2 + 1);
			skb_reset_network_header(skb);

			skb->dev = dev;
			skb->dst = NULL;	/* XXX */
			skb->priority = 0;	/* XXX */

			ptr = (typeof(ptr)) skb_network_header(skb);
			ptr[0] = (dsap << 1) | gr;
			ptr[1] = (ssap << 1) | cr;
			ptr[2] = mtype | (pf << 4);
			dev_queue_xmit(skb);
			return (0);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/* S-frames */

static fastcall noinline __unlikely int
llc_snd_RR(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap, uchar cr, uchar pf, uint nr)
{
	mi_strlog(WR(dl->oq), STRLOGTX, SL_TRACE, "RR ->");
	return llc_snd_S_frame(dev, gr, dsap, ssap, cr, pf, nr, LLC_MT_RR);
}

static fastcall noinline __unlikely int
llc_snd_RNR(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap, uchar cr, uchar pf, uint nr)
{
	mi_strlog(WR(dl->oq), STRLOGTX, SL_TRACE, "RNR ->");
	return llc_snd_S_frame(dev, gr, dsap, ssap, cr, pf, nr, LLC_MT_RNR);
}

static fastcall noinline __unlikely int
llc_snd_REJ(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap, uchar cr, uchar pf, uint nr)
{
	mi_strlog(WR(dl->oq), STRLOGTX, SL_TRACE, "REJ ->");
	return llc_snd_S_frame(dev, gr, dsap, ssap, cr, pf, nr, LLC_MT_REJ);
}

static fastcall noinline __unlikely int
llc_snd_SREJ(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap, uchar cr, uchar pf, uint nr)
{
	mi_strlog(WR(dl->oq), STRLOGTX, SL_TRACE, "SREJ ->");
	return llc_snd_S_frame(dev, gr, dsap, ssap, cr, pf, nr, LLC_MT_SREJ);
}

static fastcall noinline __unlikely int
llc_snd_UI(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap, uchar cr, uchar pf, mblk_t *dp)
{
	mi_strlog(WR(dl->oq), STRLOGDA, SL_TRACE, "UI ->");
	return llc_snd_U_frame(dev, gr, dsap, ssap, cr, pf, dp, LLC_MT_UI);
}

static fastcall noinline __unlikely int
llc_snd_SABM(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap, uchar p)
{
	mi_strlog(WR(dl->oq), STRLOGTX, SL_TRACE, "SABM ->");
	return llc_snd_U_frame(dev, gr, dsap, ssap, 0, p, NULL, LLC_MT_SABM);
}

static fastcall noinline __unlikely int
llc_snd_SABME(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap, uchar p)
{
	mi_strlog(WR(dl->oq), STRLOGTX, SL_TRACE, "SABME ->");
	return llc_snd_U_frame(dev, gr, dsap, ssap, 0, p, NULL, LLC_MT_SABME);
}

static fastcall noinline __unlikely int
llc_snd_SM(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap, uchar p)
{
	mi_strlog(WR(dl->oq), STRLOGTX, SL_TRACE, "SM ->");
	return llc_snd_U_frame(dev, gr, dsap, ssap, 0, p, NULL, LLC_MT_SM);
}

static fastcall noinline __unlikely int
llc_snd_SME(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap, uchar p)
{
	mi_strlog(WR(dl->oq), STRLOGTX, SL_TRACE, "SME ->");
	return llc_snd_U_frame(dev, gr, dsap, ssap, 0, p, NULL, LLC_MT_SME);
}

static fastcall noinline __unlikely int
llc_snd_DISC(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap, uint f)
{
	mi_strlog(WR(dl->oq), STRLOGTX, SL_TRACE, "DISC ->");
	return llc_snd_U_frame(dev, gr, dsap, ssap, 1, f, NULL, LLC_MT_DISC);
}

static fastcall noinline __unlikely int
llc_snd_DM(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap, uint f)
{
	mi_strlog(WR(dl->oq), STRLOGTX, SL_TRACE, "DM ->");
	return llc_snd_U_frame(dev, gr, dsap, ssap, 1, f, NULL, LLC_MT_DM);
}

static fastcall noinline __unlikely int
llc_snd_UA(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap, uint f)
{
	mi_strlog(WR(dl->oq), STRLOGTX, SL_TRACE, "UA ->");
	return llc_snd_U_frame(dev, gr, dsap, ssap, 1, f, NULL, LLC_MT_UA);
}

/* flags for use with FRMR */
#define LLC_FRMR_INVCF		0x01	/* invalid control field */
#define LLC_FRMR_INVLEN		0x02	/* invalid length */
#define LLC_FRMR_INVINF		0x04	/* information field too long */
#define LLC_FRMR_INVNR		0x08	/* invalid N(R) */

static fastcall noinline __unlikely int
llc_snd_FRMR(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap,
	     uint rfcf, uchar f, uint vs, uchar cr, uint vr, uchar flags)
{
	mblk_t *mp;
	int err;

	if (likely(!!(mp = mi_allocb(q, FASTBUF, BPRI_MED | BPRI_SKBUFF)))) {
		/* FRMR has an information field */
		*mp->b_wptr++ = rfcf;
		*mp->b_wptr++ = (rfcf >> 8);
		*mp->b_wptr++ = (vs << 1);
		*mp->b_wptr++ = (vr << 1) | cr;
		*mp->b_wptr++ = flags;
		mi_strlog(WR(dl->oq), STRLOGTX, SL_TRACE, "FRMR ->");
		if ((err = llc_snd_U_frame(dev, gr, dsap, ssap, 1, f, NULL, LLC_MT_FRMR))) {
			freeb(mp);
			return (err);
		}
		return (0);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
llc_snd_XID(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap, uchar cr, uchar pf, mblk_t *dp)
{
	mi_strlog(WR(dl->oq), STRLOGTX, SL_TRACE, "XID ->");
	return llc_snd_U_frame(dev, gr, dsap, ssap, cr, pf, dp, LLC_MT_XID);
}

static fastcall noinline __unlikely int
llc_snd_TEST(struct llcdev *dev, uchar gr, uchar dsap, uchar ssap, uchar cr, uchar pf, mblk_t *dp)
{
	mi_strlog(WR(dl->oq), STRLOGTX, SL_TRACE, "TEST ->");
	return llc_snd_U_frame(dev, gr, dsap, ssap, cr, pf, dp, LLC_MT_TEST);
}

/* NOTE -- An LLC can send both I command and response frames. */
static fastcall inline __hot_write int
llc_snd_I(struct llcdev *dev, uchar gr, uchar dspa, uchar ssap, uchar cr,
	  uchar pf, uint nr uint ns, mblk_t *dp)
{
	mi_strlog(WR(dl->oq), STRLOGDA, SL_TRACE, "I ->");
	return llc_snd_I_frame(dev, gr, dsap, ssap, cr, pf, nr, ns, dp);
}

/*
 * --------------------------------------------------------------------------
 *
 * DLPI PROVIDER TO DLPI USER ISSUED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
m_error(struct dl *dl, queue_t *q, mblk_t *msg)
{
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, 2, BPRI_MED)))) {
		DB_TYPE(mp) = M_ERROR;
		*mp->b_wptr++ = rerr;
		*mp->b_wptr++ = werr;
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- M_ERROR");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
};

/**
 * @dl: data link private structure
 * @q: active queue
 * @msg: message to free upon success (or NULL)
 * @error: negative UNIX error, positive DLPI error
 *
 * This function is like m_error() except that it passes through error codes that the caller
 * should simply return without generating an M_ERROR, and also, it translates positive DLPI
 * errors into EPROTO.
 */
static fastcall noinline __unlikely int
m_error_reply(struct dl *dl, queue_t *q, mblk_t *msg)
{
	switch (error) {
	case 0:
	case -ENOBUFS:
	case -EAGAIN:
	case -EDEADLK:
	case -EBUSY:
	case -ENOMEM:
		return (error);
	}
	if (error > 0)
		return m_error(dl, q, msg, EPROTO, EPROTO);
	return m_error(dl, q, msg, -error, -error);
};

static fastcall noinline __unlikely int
dl_data_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	if (likely(canpuntext(dl->oq))) {
		/* strip the I frame header */
		mp->b_rptr += dl->mode;
		if (msg && msg->b_cont == mp)
			msg->b_cont = NULL;
		freemsg(msg);
		/* Spec says not to indicate zero length frames. */
		if (msgdsize(mp) > 0) {
			putnext(dl->oq, mp);
			return (0);
		}
		freemsg(mp);
		return (0);
	}
	/* XXX: indicate busy */
	return (-EBUSY)
};

static fastcall noinline __unlikely int
dl_info_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_info_ack_t *p;
	mblk_t *mp;
	size_t add_len = dl->proto.info.add_len;
	size_t qos_len = sizeof(dl->proto.qos);
	size_t qor_len = sizeof(dl->proto.qor);
	size_t brd_len = dl->proto.info.brd_len;
	size_t msg_len = sizeof(*p) + qos_len + qor_len + add_len + brd_len;

	if (likely(!!(mp = mi_allocb(q, msg_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_INFO_ACK;
		p->dl_max_sdu = dl->proto.info.dl_max_sdu;
		p->dl_min_sdu = dl->proto.info.dl_min_sdu;
		p->dl_addr_length = add_len;
		p->dl_mac_type = dl->proto.info.dl_mac_type;
		p->dl_reserved = dl->proto.info.dl_reserved;
		p->dl_current_state = dl->proto.info.dl_current_state;
		p->dl_sap_length = dl->proto.info.dl_sap_length;
		p->dl_service_mode = dl->proto.info.dl_service_mode;
		p->dl_qos_length = qos_len;
		p->dl_qos_offset = sizeof(*p);
		p->dl_qos_range_length = qor_len;
		p->dl_qos_range_offset = p->dl_qos_offset + qos_len;
		p->dl_provider_style = dl->proto.info.dl_provider_style;
		p->dl_addr_offset = p->dl_qos_range_offset + qor_len;
		p->dl_version = dl->proto.info.dl_version;
		p->dl_brdcst_addr_length = brd_len;
		p->dl_brdcst_addr_offset = p->dl_addr_offset + add_len;
		p->dl_growth = 0;
		mp->b_wptr += sizeof(*p);
		bcopy(&dl->proto.qos, mp->b_wptr, qos_len);
		mp->b_wptr += qos_len;
		bcopy(&dl->proto.qor, mp->b_wptr, qor_len);
		mp->b_wptr += qor_len;
		bcopy(&dl->proto.add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		bcopy(&dl->proto.brd, mp->b_wptr, brd_len);
		mp->b_wptr += brd_len;
		freemsg(msg);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_INFO_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_bind_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_bind_ack_t *p;
	mblk_t *mp;
	size_t add_len = dl->proto.info.dl_addr_length;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + add_len, BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_BIND_ACK;
		p->dl_sap = dl->proto.info.dl_sap;
		p->dl_addr_length = add_len;
		p->dl_addr_offset = sizeof(*p);
		mp->b_wptr += sizeof(*p);
		bcopy(dl->proto.add, mp->b_wptr, add_len);
		mp->b_wptr += add_len;
		freemsg(msg);
		dl_set_state(dl, DL_IDLE);
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_BIND_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_ok_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim)
{
	dl_ok_ack_t *p;
	mblk_t *mp;
	int err;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_OK_ACK;
		p->dl_correct_primitive = prim;
		mp->b_wptr += sizeof(*p);
		switch (dl_get_state(dl)) {
		case DL_ATTACH_PENDING:
			freemsg(msg);
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_DETACH_PENDING:
			freemsg(msg);
			dl_set_state(dl, DL_UNATTACHED);
			break;
		case DL_UNBIND_PENDING:
			freemsg(msg);
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_UDQOS_PENDING:
			freemsg(msg);
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_CONN_RES_PENDING:
			switch (dl_get_pstate(dl)) {
			case DL_ST_CONN:
				if ((err = cd_snd_UA(dl->cd, q, msg, dl->f_flag)))
					goto error;
				dl->vs = 0;
				dl->vr = 0;
				dl->retry_count = 0;
				dl->p_flag = 0;
				dl->remote_busy = 0;
				dl_set_pstate(dl, DL_ST_NORMAL);
				break;
			default:
				err = -EPROTO;
				goto error;
			}
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_RESET_RES_PENDING:
			switch (dl_get_pstate(dl)) {
			case DL_ST_RESET_WAIT:
				/* local provider reset */
				if (dl->s_flag == 0) {
					if ((err = cd_snd_SM(dl->cd, q, msg, 1)))
						goto error;
					dl->p_flag = 1;
					dl->retry_count = 0;
					mi_timer(dl->t.ack, dl->t1);
					dl_set_pstate(dl, DL_ST_RESET);
				} else {
					if ((err = cd_snd_UA(dl->cd, q, msg, dl->f_flag)))
						goto error;
					dl->vs = 0;
					dl->vr = 0;
					dl->retry_count = 0;
					dl->p_flag = 0;
					dl->remote_busy = 0;
					dl_set_pstate(dl, DL_ST_NORMAL);
				}
				break;
			case DL_ST_RESET_CHECK:
				/* remote provider reset */
				if ((err = cd_snd_UA(dl->cq, q, msg, dl->f_flag)))
					goto error;
				dl->vs = 0;
				dl->vr = 0;
				dl->retry_count = 0;
				dl->p_flag = 0;
				dl->remote_busy = 0;
				dl_set_pstate(dl, DL_ST_NORMAL);
				break;
			default:
				err = -EPROTO;
				goto error;
			}
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_DISCON8_PENDING:
			switch (dl_get_pstate(dl)) {
			case DL_ST_SETUP:
				/* setup still in progress */
				/* this is not in IEEE 802.2 state machines! */
				if ((err = cd_snd_DM(dl->cd, q, msg, 1)))
					goto error;
				mi_timer_stop(dl->t.ack);
				dl_set_pstate(dl, DL_ST_ADM);
				break;
			default:
				err = -EPROTO;
				goto error;
			}
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON9_PENDING:
			switch (dl_get_pstate(dl)) {
			case DL_ST_CONN:
				if ((err = cd_snd_DM(dl->cd, q, msg, dl->f_flag)))
					goto error;
				dl_set_pstate(dl, DL_ST_ADM);
				break;
			default:
				err = -EPROTO;
				goto error;
			}
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON11_PENDING:
			switch (dl_get_pstate(dl)) {
			case DL_ST_NORMAL:
			case DL_ST_BUSY:
			case DL_ST_REJECT:
			case DL_ST_AWAIT:
			case DL_ST_AWAIT_BUSY:
			case DL_ST_AWAIT_REJECT:
				if ((err = cd_snd_DISC(dl->cd, q, msg, 1)))
					goto error;
				dl->p_flag = 1;
				dl->retry_count = 0;
				mi_timer_stop(dl->t.ack);
				mi_timer_stop(dl->t.p);
				mi_timer_stop(dl->t.rej);
				mi_timer_stop(dl->t.busy);
				mi_timer(dl->t.ack, dl->t1);
				dl_set_pstate(dl, DL_ST_D_CONN);
				break;
			default:
				err = -EPROTO;
				goto error;
			}
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON12_PENDING:
			/* FIXME: from what state? */
			/* send DISC command with P bit set */
			if ((err = cd_snd_DISC(dl->cd, q, msg, 1)))
				goto error;
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON13_PENDING:
			switch (dl_get_pstate(dl)) {
			case DL_ST_RESET_WAIT:
				/* local provider reset */
				if (dl->s_flag == 0) {
					if ((err = cd_snd_DISC(dl->cd, q, msg, 1)))
						goto error;
					dl->p_flag = 1;
					dl->retry_count = 0;
					mi_timer(dl->t.ack, dl->t1);
					dl_set_pstate(dl, DL_ST_D_CONN);
				} else {
					if ((err = cd_snd_DM(dl->cd, q, msg, dl->f_flag)))
						goto error;
					dl_set_pstate(dl, DL_ST_ADM);
				}
				break;
			case DL_ST_RESET_CHECK:
				/* remote provider reset */
				if ((err = cd_snd_DM(dl->cd, q, msg, dl->f_flag)))
					goto error;
				dl_set_pstate(dl, DL_ST_ADM);
				break;
			default:
				err = -EPROTO;
				goto error;
			}
			dl_set_state(dl, DL_IDLE);
			break;
		}
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_OK_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
      error:
	freeb(mp);
	return (err);
}

static fastcall noinline __unlikely int
dl_error_ack(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, dl_long error)
{
	dl_error_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		DB_TYPE(mp) = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->dl_primitive = DL_ERROR_ACK;
		p->dl_error_primitive = prim;
		p->dl_errno = error < 0 ? DL_SYSERR : 0;
		p->dl_unix_errno = error < 0 ? -error : 0;
		mp->b_wptr += sizeof(*p);
		freemsg(msg);
		switch (dl_get_state(dl)) {
		case DL_ATTACH_PENDING:
			dl_set_state(dl, DL_UNATTACHED);
			break;
		case DL_DETACH_PENDING:
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_BIND_PENDING:
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_UNBIND_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_UDQOS_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_OUTCON_PENDING:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_CONN_RES_PENDING:
			dl_set_state(dl, DL_INCON_PENDING);
			break;
		case DL_RESET_RES_PENDING:
			dl_set_state(dl, DL_PROV_RESET_PENDING);
			break;
		case DL_USER_RESET_PENDING:
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_DISCON8_PENDING:
			dl_set_state(dl, DL_OUTCON_PENDING);
			break;
		case DL_DISCON9_PENDING:
			dl_set_state(dl, DL_INCON_PENDING);
			break;
		case DL_DISCON11_PENDING:
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_DISCON12_PENDING:
			dl_set_state(dl, DL_USER_RESET_PENDING);
			break;
		case DL_DISCON13_PENDING:
			dl_set_state(dl, DL_PROV_RESET_PENDING);
			break;
		case DL_SUBS_BIND_PND:
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_SUBS_UNBIND_PND:
			dl_set_state(dl, DL_IDLE);
			break;
		}
		mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_ERROR_ACK");
		putnext(dl->oq, mp);
		return (0);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_error_reply(struct dl *dl, queue_t *q, mblk_t *msg, dl_ulong prim, dl_long error)
{
	switch (error) {
	case 0:
	case -ENOBUFS:
	case -EAGAIN:
	case -EDEADLK:
	case -EBUSY:
	case -ENOMEM:
		return (error);
	}
	return dl_error_ack(dl, q, msg, prim, error);
}

static fastcall noinline __unlikely int
dl_subs_bind_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_subs_bind_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_SUBS_BIND_ACK;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_SUBS_BIND_ACK");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_unitdata_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_unitdata_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_UNITDATA_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_UNITDATA_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_uderror_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_uderror_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_UDERROR_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_UDERROR_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_connect_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_connect_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_CONNECT_IND;
			p->dl_correlation = ++dl->correlation;
			p->dl_called_addr_length = 0;
			p->dl_called_addr_offset = 0;
			p->dl_calling_addr_length = 0;
			p->dl_calling_addr_offset = 0;
			p->dl_qos_length = 0;
			p->dl_qos_offset = 0;
			p->dl_growth = 0;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, DL_INCON_PENDING);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_CONNECT_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_connect_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_connect_con_t *p;
	mblk_t *mp;
	size_t qos_len = sizeof(dl->proto.info.qos);

	if (likely(!!(mp = mi_allocb(q, sizeof(*p) + qos_len, BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_CONNECT_CON;
			p->dl_resp_addr_length = 0;
			p->dl_resp_addr_offset = 0;
			p->dl_qos_length = qos_len;
			p->dl_qos_offset = sizeof(*p);
			p->dl_growth = 0;
			mp->b_wptr += sizeof(*p);
			bcopy(&dl->proto.info.qos, mp->b_wptr, qos_len);
			mp->b_wptr += qos_len;
			freemsg(msg);
			dl_set_state(dl, DL_DATAXFER);
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_CONNECT_CON");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_token_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_token_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_TOKEN_ACK;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_TOKEN_ACK");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_disconnect_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_disconnect_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_DISCONNECT_IND;
			p->dl_originator = orig;
			p->dl_reason = reason;
			p->dl_correlation = seq;
			mp->b_wptr += sizeof(*p);
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_DISCONNECT_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_reset_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reset_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_RESET_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_RESET_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_reset_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reset_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_RESET_CON;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_RESET_CON");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_data_ack_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_data_ack_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_DATA_ACK_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_DATA_ACK_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_data_ack_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_data_ack_status_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_DATA_ACK_STATUS_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_DATA_ACK_STATUS_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_reply_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reply_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_REPLY_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_REPLY_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_reply_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reply_status_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_REPLY_STATUS_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_REPLY_STATUS_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_reply_update_status_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_reply_update_status_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_REPLY_UPDATE_STATUS_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_REPLY_UPDATE_STATUS_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_xid_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_xid_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_XID_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_XID_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_xid_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_xid_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_XID_CON;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_XID_CON");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_test_ind(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_test_ind_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_TEST_IND;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_TEST_IND");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_test_con(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_test_con_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_TEST_CON;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_TEST_CON");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_phys_addr_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_phys_addr_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_PHYS_ADDR_ACK;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_PHYS_ADDR_ACK");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

static fastcall noinline __unlikely int
dl_get_statistics_ack(struct dl *dl, queue_t *q, mblk_t *msg)
{
	dl_get_statistics_ack_t *p;
	mblk_t *mp;

	if (likely(!!(mp = mi_allocb(q, sizeof(*p), BPRI_MED)))) {
		if (likely(canputnext(dl->oq))) {
			DB_TYPE(mp) = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->dl_primitive = DL_GET_STATISTICS_ACK;
			mp->b_wptr += sizeof(*p);
			mp->b_cont = dp;
			if (msg && dp && msg->b_cont == dp)
				msg->b_cont = NULL;
			freemsg(msg);
			dl_set_state(dl, dl_get_state(dl));
			mi_strlog(dl->oq, STRLOGTX, SL_TRACE, "<- DL_GET_STATISTICS_ACK");
			putnext(dl->oq, mp);
			return (0);
		}
		freeb(mp);
		return (-EBUSY);
	}
	return (-ENOBUFS);
}

/*
 * --------------------------------------------------------------------------
 *
 * MAC AND LLC RECEIVED MESSAGES
 *
 * --------------------------------------------------------------------------
 */
static fastcall noinline __unlikely int
llc_rcv_I(struct llcdev *dev, struct sk_buff *skb, uchar gr, uchar dsap, uchar ssap,
	  uchar cr, uchar pf, uint ns, uint nr)
{
};

static fastcall noinline __unlikely int
llc_rcv_RR(struct llcdev *dev, struct sk_buff *skb, uchar gr, uchar dsap, uchar ssap,
	   uchar cr, uchar pf, uint nr)
{
};

static fastcall noinline __unlikely int
llc_rcv_RNR(struct llcdev *dev, struct sk_buff *skb, uchar gr, uchar dsap, uchar ssap,
	    uchar cr, uchar pf, uint nr)
{
};

static fastcall noinline __unlikely int
llc_rcv_REJ(struct llcdev *dev, struct sk_buff *skb, uchar gr, uchar dsap, uchar ssap,
	    uchar cr, uchar pf, uint nr)
{
};

static fastcall noinline __unlikely int
llc_rcv_SREJ(struct llcdev *dev, struct sk_buff *skb, uchar gr, uchar dsap, uchar ssap,
	     uchar cr, uchar pf, uint nr)
{
};

static fastcall noinline __unlikely int
llc_rcv_UI(struct llcdev *dev, struct sk_buff *skb, uchar gr, uchar dsap, uchar ssap,
	   uchar cr, uchar pf)
{
};

static fastcall noinline __unlikely int
llc_rcv_SABM(struct llcdev *dev, struct sk_buff *skb, uchar gr, uchar dsap, uchar ssap,
	     uchar cr, uchar pf)
{
};

static fastcall noinline __unlikely int
llc_rcv_SABME(struct llcdev *dev, struct sk_buff *skb, uchar gr, uchar dsap, uchar ssap,
	      uchar cr, uchar pf)
{
};

static fastcall noinline __unlikely int
llc_rcv_SM(struct llcdev *dev, struct sk_buff *skb, uchar gr, uchar dsap, uchar ssap,
	   uchar cr, uchar pf)
{
};

static fastcall noinline __unlikely int
llc_rcv_SME(struct llcdev *dev, struct sk_buff *skb, uchar gr, uchar dsap, uchar ssap,
	    uchar cr, uchar pf)
{
};

static fastcall noinline __unlikely int
llc_rcv_DISC(struct llcdev *dev, struct sk_buff *skb, uchar gr, uchar dsap, uchar ssap,
	     uchar cr, uchar pf)
{
};

static fastcall noinline __unlikely int
llc_rcv_DM(struct llcdev *dev, struct sk_buff *skb, uchar gr, uchar dsap, uchar ssap,
	   uchar cr, uchar pf)
{
};

static fastcall noinline __unlikely int
llc_rcv_UA(struct llcdev *dev, struct sk_buff *skb, uchar gr, uchar dsap, uchar ssap,
	   uchar cr, uchar pf)
{
};

static fastcall noinline __unlikely int
llc_rcv_FRMR(struct llcdev *dev, struct sk_buff *skb, uchar gr, uchar dsap, uchar ssap,
	     uchar cr, uchar pf)
{
};

/**
 * llc_rcv_XID: - receive XID command or response
 * @dev: the device upon which the message was received
 * @skb: the XID command or response message
 * @gr: individual/group 0/1
 * @dsap: DSAP address
 * @ssap: SSAP address
 * @cr: command/response 0/1
 * @pr: Poll/Final bit
 *
 * IEEE 802.2:1998, 5.4.1.1.2 Exchange identification (XID) command.  The XID command PDU shall
 * be used to convey the types of LLC services supported (for all LLC services) and the receive
 * window size on a per data link connection basis ot the destination LLC, and to cause the
 * destination LLC to respond with the XID response PDU (see 5.4.1.2.1) at the earliest
 * opportunity.  The XID command PDU shall have no effect on any mode or sequence numbers
 * maintained by the remote LLC.  An XID command PDU shall have ether an individual, group,
 * global, or null address as the destination DSP address and a null address or the originator's
 * individual address as the SSAP address.
 *
 * The information field of an XID LLC basic format command PDU shall consist of an 8-bit XID
 * format identifier field push a 16-bit parameter field, as shown in figure 11.  The parameter
 * field is encoded to identify the LLC services supported plus the XID sender's receive window
 * size (RW).
 *
 * The XID receiver should set its tranmit window, k, to a value less than or equal to the
 * receive window of the XID sender, to avoid overrunning the XID sender.
 *
 * NOTE -- Other uses of the XID PDU are for further study.  In particular, the use of an
 * unsolicited XID response PDU to announce the presence of a new LLC will be examined.
 *
 *                |<------------XID information field------------>|
 *         -------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *                |X-X-X-X-X-X-X-X|Y-Y-Y-Y-Y Z-Z-Z|Z W-W-W-W-W-W-W|
 *         -------+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *         XXXXXXXX = 10000001
 *           XID format identifier: identifies LLC basic format
 *         YYYYY service support
 *           1xxxx - indicates support for Type 1
 *           x1xxx - indicates support for Type 2
 *           xx1xx - indicates support for Type 3
 *           If the null LSAP is used, the values of YYYYY are:
 *           10000 - Class   I LLC (Type 1)
 *           11000 - Class  II LLC (Type 1 and Type 2)
 *           10100 - Class III LLC (Type 1 and Type 3)
 *           11100 - Class  IV LLC (Type 1, Type 2 and Type 3)
 *           If a non-null LSAP is used, the values of YYY are:
 *           10000 - Type 1 LLC
 *           01000 - Type 2 LLC
 *           00100 - Type 3 LLC
 *           11000 - Type 1 and Type 2 LLCs
 *           10100 - Type 1 and Type 3 LLCs
 *           01100 - Type 2 and Type 3 LLCs
 *           11100 - Type 1 and Type 2 and Type 3 LLCs
 *         ZZZZ reserved (set to zero)
 *         WWWWWWWW receive window size
 *
 * IEEE 802.2:1998, 5.4.1.2.1 Exchange Identification (XID) response.  THe XID response PDU
 * shall be used to reply to an XID command PDU at the earliest opporunity.  The XID response
 * PDU shall identify the responding LLC and shall include an information field like that
 * defined for the XID command PDU (see 5.4.1.1.2), regardless of what information is present in
 * the information field of the received XID command PDU.  The XID response PDU shall use an
 * individual or null DSAP address and shall use an individual or null SSAP address.  The XID
 * response PDU shall have its F bit set to the state of the P bit in the XID command PDU.
 *
 * IEEE 802.2:1998, 6.6 Uses of the XID command PDU and response PDU.  While the response to an
 * XID command PDU shall be mandatory, the origination of an XID command PDU shall be optional.
 * It shall be possible for the XID capabilities to be used as a part of some network control
 * functions.  As such, an XID command PDU may be sent on direction from a higher layer
 * function, an administrative function having access to the data link layer, or an automatic
 * start-up function.  However, it shall also be possible for a more capable implementation of
 * LLC to incorporate the use of the XID function directly to make more efficient use of the
 * protocol.
 *
 * Some possible uses of the XID capabilities include:
 *
 * 1)	The XID command PDU with a null DSAP and a null SSAP is a way to solicit a response from
 *	any station (i.e., any DA).  As such it represents a basic "Are You There?" test
 *	capability.
 *
 * 2)	The XID command PDU with a group DA or group DSAP address can be used to determine the
 *	group membership.  In particular, the XID command PDU with a global DA address can
 *	identify all active stations.
 *
 * 3)	A duplicate address check can be made (see table 1).
 *
 * 4)	For Class II LLCs in ABM, an XID exchange can be used to identify the receive window
 *	size at each LLC for that data link connection.  NOTE -- The use of the XID exchange for
 *	this purpose is not valid in the asynchronous disconnected mode (ADM) state.
 *
 * 5)	An XID exchange with a null DSAP and a null SSAP can identify the class of each LLC.
 *
 * 6)	An XID exchange with a specific DSAP and a specific SSAP can identify the service types
 *	supported by those service access points.
 *
 * 7)	An LLC can announce its presence with a global DA address in an XID PDU.
 *
 *
 */
static fastcall noinline __unlikely int
llc_rcv_XID(struct llcdev *dev, struct sk_buff *skb, uchar gr, uchar dsap, uchar ssap,
	    uchar cr, uchar pf)
{
};

/**
 * llc_rcv_TEST: - receive TEST command or response
 * @dev: the device upon which the message was received
 * @skb: the TEST command or response message
 * @gr: individual/group 0/1
 * @dsap: DSAP address
 * @ssap: SSAP address
 * @cr: command/response 0/1
 * @pr: Poll/Final bit
 *
 * IEEE 802.2:1998, 5.4.1.1.3 Test (TEST) command.  The TEST command PDU shall be used to cause
 * the destination LLC to response with the TEST response PDU (see 5.4.1.2.2) at the earliest
 * opportunity, thus perromign a basic test of the LLC to LLC transmission path.  An information
 * field is optional with the TEST command PDU.  If present, however, the received information
 * field shall be returned, if possible, by the addressed LLC in the TEST response PDU.  The
 * TEST may be used with an individual, group, global, or null DSAP address with an individual
 * or null SSAP address, and with an individual group, or global, DA address.
 *
 * IEEE 802.2:1998, 5.4.1.2.2 Test (TEST) response.  The TEST response PDU shall be used to
 * reply to the TEST command PDU.  The TEST response PDU shall have its F bit set to the value
 * of the P bit in the TEST command PDU.  An information field, if present in the TEST command
 * PDU, shall be returned in the corresponding TEST response PDU.  If the LLC cannot accept an
 * information field(e.g., buffering limitation), a TEST response PDU without an information
 * field may eb returned.  Refer to 6.7 for specific details on TEST response usage.  The LLC
 * sends a TEST response PDU, using  athe SSAP address of the TEST command PDU as the DSAP
 * address of the TEST response PDU, and with an individual or null SSAP address selected by the
 * DAP address of the TEST command PDU.
 *
 * IEEE 802.2:1998, 6.7 Uses of the TEST command PDU and response PDU.  The TEST function
 * provides a facility to conduct loopback tests of the LLC to LLC transmission path.  The
 * initiation of the TEST function may be caused by an administration or management entity
 * within the data link layer.  Successful completion of the test consists of sending a TEST
 * command PDU with a particular information field provided by this adminstration or management
 * entity to the designated destination LLC address and receiving, in return, the identical
 * information field in a TEST response PDU.  Implementation of the TEST command PDU is optional
 * but every LLC must be able to respond to a received TEST command PDU with a TEST response
 * PDU.  The length of the information field is variable from 0 to the largest size specified
 * that each LLC on this local area network must support for normal data transfer.
 *
 * It shall also be possible to send larger informaiton fields with the following
 * interpretations.  If the receiving LLC can successfully receive and return the larger
 * information field, it will do so.  If it cannot receive the entire information field but the
 * MAC can detect a satisfactory FCS, the LLC shall discard the portion of the information field
 * received, and may return a TEST response PDU with no information field.  If the MAC
 * cannotproperly compute the FCS for the overlength information fields, the LLC shall discard
 * the protion of the information field received, and shall give no response.  Any TEST command
 * PDU received in error shall be discarded and no srponse PDU sent.  In the event of failure, it
 * shall be the responsibility of the administration or management entity that initiated the
 * TEST function to determine any future actions.
 */
static fastcall noinline __unlikely int
llc_rcv_TEST(struct llcdev *dev, struct sk_buff *skb, uchar gr, uchar dsap, uchar ssap,
	     uchar cr, uchar pf)
{
	int err;
	uint da, sa;

	sa = dsap;
	da = ssap;

	mp->b_rptr += 1;
	if ((err = llc_snd_TEST(dl, q, msg, mp, 1, da, sa, cr, pf))) {
		mp->b_rptr -= 1;
		return (err);
	}
	return (0);
};

static fastcall inline __hot_read int
llc_rcv_msg(struct llcdev *dev, struct sk_buff *skb)
{
	uint ns, pf, nr, cr = 0, da, sa, gr, mt;
	bool num;
	bool inf;
	size_t size;

	if ((size = msgdsize(mp)) < 2 + 1)
		goto discard;

	num = ((mp->b_rptr[2] & 0x03) != 0x03);	/* numbered frame? */
	inf = ((mp->b_rptr[2] & 0x01) != 0x00);	/* ??? */

	if (num && size < 2 + 2)
		goto badsize;

	gr = (mp->b_rptr[0] >> 0) & 0x01;
	da = (mp->b_rptr[0] >> 1) & 0x7f;
	cr = (mp->b_rptr[1] >> 0) & 0x01;
	sa = (mp->b_rptr[1] >> 1) & 0x7f;

	switch (mp->b_rptr[2] & 0x03) {
	case 0x00:
	case 0x02:		/* Information */
		ns = (mp->b_rptr[2] >> 1) & 0x7f;
		pf = (mp->b_rptr[3] >> 0) & 0x01;
		nr = (mp->b_rptr[3] >> 1) & 0x7f;
		return llc_rcv_I(dev, skb, gr, da, sa, cr, pf, ns, nr);
	case 0x01:		/* Supervisory */
		pf = (mp->b_rptr[3] >> 0) & 0x01;
		nr = (mp->b_rptr[3] >> 1) & 0x7f;
		mt = (mp->b_rptr[2] >> 0) & 0xff;
		switch ((mp->b_rptr[2] >> 2) & 0x03) {
		case 0x00:
			return llc_rcv_RR(dev, skb, gr, da, sa, cr, pf, nr);
		case 0x01:
			return llc_rcv_RNR(dev, skb, gr, da, sa, cr, pf, nr);
		case 0x02:
			return llc_rcv_REJ(dev, skb, gr, da, sa, cr, pf, nr);
		case 0x03:
			return llc_rcv_SREJ(dev, skb, gr, da, sa, cr, pf, nr);
		}
	case 0x03:		/* Unnumbered */
		pf = (mp->b_rptr[2] >> 4) & 0x01;
		switch ((mp->b_rptr[2] >> 2) & 0x3b) {
		case 0x00:
			return llc_rcv_UI(dev, skb, gr, da, sa, cr, pf);
		case 0x0b:
			return llc_rcv_SABM(dev, skb, gr, da, sa, cr, pf);
		case 0x1b:
			return llc_rcv_SABME(dev, skb, gr, da, sa, cr, pf);
		case 0x30:
			return llc_rcv_SM(dev, skb, gr, da, sa, cr, pf);
		case 0x3b:
			return llc_rcv_SME(dev, skb, gr, da, sa, cr, pf);
		case 0x10:
			return llc_rcv_DISC(dev, skb, gr, da, sa, cr, pf);
		case 0x03:
			return llc_rcv_DM(dev, skb, gr, da, sa, cr, pf);
		case 0x18:
			return llc_rcv_UA(dev, skb, gr, da, sa, cr, pf);
		case 0x21:
			return llc_rcv_FRMR(dev, skb, gr, da, sa, cr, pf);
		case 0x2b:
			return llc_rcv_XID(dev, skb, gr, da, sa, cr, pf);
		case 0x38:
			return llc_rcv_TEST(dev, skb, gr, da, sa, cr, pf);
		}
	}
}

/**
 * llc_rcv_msg: - receive a packet
 * @skb: the packet
 * @dev: the Linux network device
 * @pt: the packet type
 * @dev2: some other device
 *
 * Addressing:
 *
 * Addresses are one of the following:
 *
 * - 11111111: (Global) identifies all LLCs associated the the termination.
 * - 00000000: (Null) identifies the MAC entity.
 * - x1000000: identifies the LLC management entity.
 * - x0DDDDDD: identifier the LLC SAP entity.
 *
 * There are several actions that are taken here.
 *
 * - If the message is a Type 1 message (UI, XID or TEST) and is addressed to the global DSAP,
 *   the message is queued to each SAP with the same DA (unicast, multicast or broadcast).  The
 *   message is also processed by any active LS.
 *
 * - If the message is a Type 1 message (UI, XID or TEST) and is addressed to the null DSAP, the
 *   message is processed by any active LS associated with the DA (unicast, multicast or
 *   broadcast).  UI messages to the null DSAP are discarded: only XID and TEST are processed.
 *
 * - If the message is a Type 1 message (UI, XID or TEST) and is address to an individual DSAP,
 *   the message is queue to that LSAP if it exists.
 *
 * - If the message is a Type 2 message (SABME, DISC, DM, UA, I, RR, RNR, REJ, FRMR) and is
 *   addressed to a non-null SAP, non-group SAP, and a non-group DA, from a non-group SA, the
 *   DA/SA/DSAP/SSAP will be used to locate a Connection Component.  If a Connection Component
 *   exsits, the message will be queued to that Connection Component.  If a Connection Component
 *   does not exist and the message is a SABME, the DA/DSAP will be used to determine whether
 *   there is a listening LSAP.  If a listening LSAP exists, the message will be queued to the
 *   listening LSAP.
 *
 * - If the message is a Type 2 message (SABME) and is address to a non-null SAP, group SAP
 *   and/or group DA, the DA/SAP will be used to determine whether there is a listening LSAP.
 *   If a listening LSAP exists, the message wil be queued to the listening LSAP.
 */
static int
#ifdef HAVE_KMEMB_STRUCT_PACKET_TYPE_FUNC_4_ARGS
llc_rcv_msg(struct sk_buff *skb, struct llcdev *dev, struct packet_type *pt, struct llcdev *dev2)
#else					/* HAVE_KMEMB_STRUCT_PACKET_TYPE_FUNC_4_ARGS */
llc_rcv_msg(struct sk_buff *skb, struct llcdev *dev, struct packet_type *pt)
#endif					/* HAVE_KMEMB_STRUCT_PACKET_TYPE_FUNC_4_ARGS */
{
}

/*
 * --------------------------------------------------------------------------
 *
 * LLC TIMERS
 *
 * --------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
dl_timeout_t1(struct dl *dl, queue_t *q)
{
}

static fastcall noinline __unlikely int
dl_timeout_t2(struct dl *dl, queue_t *q)
{
}

static fastcall noinline __unlikely int
dl_timeout_t3(struct dl *dl, queue_t *q)
{
}

static fastcall noinline __unlikely int
dl_timeout_t4(struct dl *dl, queue_t *q)
{
}

static fastcall noinline __unlikely int
dl_timeout_t5(struct dl *dl, queue_t *q)
{
}

/*
 * --------------------------------------------------------------------------
 *
 * DLPI USER TO DLPI PROVIDER RECEIVED PRIMITIVES
 *
 * --------------------------------------------------------------------------
 */

static fastcall inline __hot_write int
dl_data_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return cd_unitdata_req(dl->cd, q, NULL, mp);
}

static fastcall noinline __unlikely int
dl_info_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_info_ack(dl, q, mp);
}

/**
 * dl_attach_req: - process DL_ATTACH_REQ primitive
 * @dl: data link private structure
 * @q: active queue (write queue)
 * @mp: the DL_ATTACH_REQ primitive
 *
 * This driver is somewhat different thatn the old ldl driver (which still
 * exists in the strxns package).  This driver codes the Ethertype of MPOxx
 * (NPID) code in the upper 16 bits of the PPA and the (Linux) interface
 * identifier in the lower 16 bits.  When the interface identifier is
 * non-zero, it specifies a particular Linux device.  When the interface
 * identifier is zero, the attachment will be to all devices.
 *
 * For receiving packets, we do not care whether they are native, or whether
 * they contain a snap header, or whether they contain an LLC1 header.  For
 * transmitting packets, however, we need to know whether to include a SNAP
 * header and, for DL_CODLS, whether to include an LLC1 header (an LLC2, X25
 * LAPB, Frame Relay LAPB, ISDN LAPD, header is always included for DL_CODLS).
 * It is independent whether we include a SNAP header or whether we include an
 * LLC1 header: that is, there may no header, a SNAP header, an LLC1 header,
 * or a SNAP and an LLC1 header.  When the S bit is set in the PPA, it
 * indicates that a SNAP header must be included on output, when the L bit is
 * set in the PPA, it indicates that an LLC1 header must be included on
 * DL_CLDS output.
 *
 * Physical Point of Attachment
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |    EtherType or MPOxx Code    |S:L:        Interface Id       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
static fastcall noinline __unlikely int
dl_attach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_attach_req_t *p = (typeof(p)) mp->b_rptr;
	struct netdev *dev;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (dl_get_state(dl) != DL_UNATTACHED)
		goto outstate;
	if (dl->proto.info.dl_provider_style != DL_STYLE2)
		goto notsupported;
	switch ((dl->framing = (p->dl_ppa & DL_FRAME_MASK) >> DL_FRAME_SHIFT)) {
	case DL_FRAME_ETHER:
	case DL_FRAME_802_2:
	case DL_FRAME_SNAP:
	case DL_FRAME_802_3:
	case DL_FRAME_RAW_LLC:
	case DL_FRAME_LLC2:
		break;
	default:
		goto badppa;
	}
	dl->ppa = (p->dl_ppa & DL_PPA_MASK) >> DL_PPA_SHIFT;
	/* lock device map */
	if (!(dev = find_dev(dl->ppa))) {
		/* unlock device map */
		goto badppa;
	}
	dl->type = dev->type;
	/* unlock device map */
	switch (dl->type) {
	case ARPHDR_ETHER:
	case ARPHDR_LOOPBACK:
		break;
	case ARPHDR_FDDI:
		switch (dl->framing) {
		case DL_FRAME_ETHER:
		case DL_FRAME_802_3:
			goto badppa;
		default:
			break;
		}
		break;
	case APRHDR_IEEE802:
	case ARPHDR_IEEE802_TR:
		switch (dl->framing) {
		case DL_FRAME_ETHER:
		case DL_FRAME_802_3:
			goto badppa;
		default:
			break;
		}
		break;
	case ARPHDR_HDLC:
		switch (dl->framing) {
		case DL_FRAME_RAW_LLC:
		case DL_FRAME_LLC2:
			break;
		default:
			goto badppa;
		}
		break;
	default:
		goto badppa;
	}
	switch (dl->type) {
		/* ETH_P_SNAP */
	case ARPHDR_IEEE802:
		/* ETH_P_8021Q */
		/* ETH_P_802_3 */
		dl->proto.info.dl_mac_type = DL_CSMACD;
		dl->proto.info.dl_mac_type = DL_TPB;
		dl->proto.info.dl_mac_type = DL_ETH_CSMA;
		break;
	case ARPHDR_IEEE802_TR:
		/* ETH_P_TR_802_2 */
		dl->proto.info.dl_mac_type = DL_100VGTPR;
		dl->proto.info.dl_mac_type = DL_TPR;
		break;
	case ARPHDR_ETHER:
		dl->proto.info.dl_mac_type = DL_100VG;
		dl->proto.info.dl_mac_type = DL_100BT;
		dl->proto.info.dl_mac_type = DL_ETHER;
		break;
	case ARPHDR_HDLC:
	case ARPHDR_RAWHDLC:
		/* ETH_P_HDLC */
		dl->proto.info.dl_mac_type = DL_HDLC;
		break;
	case ARPHDR_FDDI:
		dl->proto.info.dl_mac_type = DL_FDDI;
		break;
	case ARPHDR_FCPP:
	case ARPHDR_FCAL:
	case ARPHDR_FCPL:
	case ARPHDR_FCFABRIC:
		dl->proto.info.dl_mac_type = DL_FC;
		break;
	case ARPHDR_ATM:
		/* ETH_P_ATMMPOA */
		dl->proto.info.dl_mac_type = DL_IPATM;
		/* ETH_P_ATMFATE */
		dl->proto.info.dl_mac_type = DL_ATM;
		break;
	case ARPHDR_LAPB:
	case ARPHDR_AX25:
		/* ETH_P_X25 */
		/* ETH_P_AX25 */
		dl->prot.dl_mac_type = DL_X25;
		break;
	case ARPHDR_HIPPI:
		dl->prot.dl_mac_type = DL_HIPPI;
		break;
	case ARPHDR_DLCI:
		dl->prot.dl_mac_type = DL_FRAME;
		break;
	case ARPHDR_FRAD:
		dl->prot.dl_mac_type = DL_MPFRAME;
		break;
	case ARPHDR_X25:
		dl->prot.dl_mac_type = DL_IPX25;
		break;
	case ARPHDR_LOOPBACK:
		/* ETH_P_LOOP */
		dl->prot.dl_mac_type = DL_LOOP;
		break;
	default:
		dl->prot.dl_mac_type = DL_ISDN;
		dl->prot.dl_mac_type = DL_METRO;
		dl->prot.dl_mac_type = DL_CHAR;
		dl->prot.dl_mac_type = DL_ASYNC;
		dl->prot.dl_mac_type = DL_CTCA;
		dl->prot.dl_mac_type = DL_OTHER;
		break;
	}
	dl_set_state(dl, DL_ATTACH_PENDING);
	if ((err = dl_ok_ack(dl, q, mp, DL_ATTACH_REQ)))
		goto error;
	return (0);
      badppa:
	err = DL_BADPPA;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_ATTACH_REQ, err);
}

static fastcall noinline __unlikely int
dl_detach_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (dl_get_state(dl) != DL_UNBOUND)
		goto outstate;
	if (dl->proto.info.dl_provider_style != DL_STYLE2)
		goto notsupported;
	dl_set_state(dl, DL_DETACH_PENDING);
	if ((err = dl_ok_ack(dl, q, mp, DL_DETACH_REQ)))
		goto error;
	return (0);
      notsupported:
	err = DL_NOTSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_DETACH_REQ, err);
}

/**
 * dl_bind_req: - precess DL_BIND_REQ primitive
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_BIND_REQ primitive
 *
 * SAP addresses are coded different depending on the framing.  If we are
 * using ethernet framing, the SAP is the 16-bit EtherType II code.  If we are
 * using an LLC1 header, the SAP is the LSAP.  It we are using SNAP header,
 * the SAP is the LSAP.  If we are using X.25 framing, the SAP is the A, B, C
 * or D address.  If we are using Frame Relay framing, the SAP is the DLCI.
 */
static fastcall noinline __unlikely int
dl_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (dl_get_state(dl) != DL_UNBOUND)
		goto outstate;
	if (p->dl_service_mode & DL_ACLDLS)
		goto unsupported;
	if (!(p->dl_service_mode & (DL_CODLS | DL_CLDLS)))
		goto unsupported;
	dl->proto.info.dl_service_mode = p->dl_service_mode;
	dl->xidtest = (p->dl_xidtest_flg & (DL_AUTO_XID | DL_AUTO_TEST));
	if ((dl->proto.info.dl_sap_length = check_sap(dl, p->dl_sap)) == 0)
		goto badsap;
	dl->proto.info.dl_sap = p->dl_sap;
	dl_set_state(dl, DL_BIND_PENDING);
	if ((err = dl_bind_ack(dl, q, mp)))
		goto error;
	return (0);
      badsap:
	err = DL_BADSAP;
	goto error;
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_BIND_REQ, err);
}

static fastcall noinline __unlikely int
dl_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_state(dl, DL_UNBIND_PENDING);
	if ((err = dl_ok_ack(dl, q, mp, DL_UNBIND_REQ)))
		goto error;
	return (0);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_UNBIND_REQ, err);
}

static fastcall noinline __unlikely int
dl_subs_bind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_bind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!MBLKIN(mp, p->dl_subs_sap_offset, p->dl_subs_sap_length))
		goto fault;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	switch (p->dl_subs_bind_class) {
	case DL_PEER_BIND:
	case DL_HIERARCHICAL_BIND:
		break;
	default:
		goto badsap;
	}
	dl_set_state(dl, DL_SUBS_BIND_PND);
	if ((err = dl_subs_bind_ack(dl, q, mp)))
		goto error;
	return (0);
      badsap:
	err = DL_BADSAP;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_SUBS_BIND_REQ, err);
}

static fastcall noinline __unlikely int
dl_subs_unbind_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_subs_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!MBLKIN(mp, p->dl_subs_sap_offset, p->dl_subs_sap_length))
		goto fault;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_state(dl, DL_SUBS_UNBIND_PND);
	if ((err = dl_ok_ack(dl, q, mp, DL_SUBS_UNBIND_REQ)))
		goto error;
	return (0);
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_SUBS_UNBIND_REQ, err);
}

static fastcall noinline __unlikely int
dl_enabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_enabmulti_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!MBLKIN(mp, p->dl_addr_offset, p->dl_addr_length))
		goto fault;
	switch (dl_get_state(dl)) {
	case DL_UNBOUND:
	case DL_IDLE:
	case DL_OUTCON_PENDING:
	case DL_INCON_PENDING:
	case DL_DATAXFER:
	case DL_USER_RESET_PENDING:
	case DL_PROV_RESET_PENDING:
		break;
	default:
		goto outstate;
	}
	if (p->dl_addr_length != dl->proto.info.dl_addr_length)
		goto badaddr;
	/* we actually need to attempt to add the multicast address to the interface */
	if ((err = dl_ok_ack(dl, q, mp, DL_ENABMULTI_REQ)))
		goto error;
	return (0);
      badaddr:
	err = DL_BADADDR;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_ENABMULTI_REQ, err);
}

static fastcall noinline __unlikely int
dl_disabmulti_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_disabmulti_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!MBLKIN(mp, p->dl_addr_offset, p->dl_addr_length))
		goto fault;
	switch (dl_get_state(dl)) {
	case DL_UNBOUND:
	case DL_IDLE:
	case DL_OUTCON_PENDING:
	case DL_INCON_PENDING:
	case DL_DATAXFER:
	case DL_USER_RESET_PENDING:
	case DL_PROV_RESET_PENDING:
		break;
	default:
		goto outstate;
	}
	if (p->dl_addr_length != dl->proto.info.dl_addr_length)
		goto badaddr;
	/* we actually need to attempt to remove the multicast address from the interface */
	if ((err = dl_ok_ack(dl, q, mp, DL_DISABMULTI_REQ)))
		goto error;
	return (0);
      badaddr:
	err = DL_BADADDR;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_DISABMULTI_REQ, err);
}

static fastcall noinline __unlikely int
dl_promiscon_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_promiscon_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	switch (dl_get_state(dl)) {
	case DL_UNBOUND:
	case DL_IDLE:
	case DL_OUTCON_PENDING:
	case DL_INCON_PENDING:
	case DL_DATAXFER:
	case DL_USER_RESET_PENDING:
	case DL_PROV_RESET_PENDING:
		break;
	default:
		goto outstate;
	}
	switch (p->dl_level) {
	case DL_PROMISC_PHYS:	/* at MAC address or raw X.25 level */
		/* Indicates that all frames should be delivered from the PPA regardless of the
		   physical address. */
	case DL_PROMISC_SAP:	/* at LSAP address or X.25 level */
		/* Indicates that all frames should be delivered from the PPA regardless of the SAP 
		   address. */
	case DL_PROMISC_MULTI:	/* at MAC multicast address level */
		/* Indicates that all frames should be delivered to the PPA when sent to any
		   multicast address. */
		break;
	default:
		goto unsupported;
	}
	if ((err = dl_ok_ack(dl, q, mp, DL_PROMISCON_REQ)))
		goto error;
	return (0);
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_PROMISCON_REQ, err);
}

static fastcall noinline __unlikely int
dl_promiscoff_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_promiscoff_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	switch (dl_get_state(dl)) {
	case DL_UNBOUND:
	case DL_IDLE:
	case DL_OUTCON_PENDING:
	case DL_INCON_PENDING:
	case DL_DATAXFER:
	case DL_USER_RESET_PENDING:
	case DL_PROV_RESET_PENDING:
		break;
	default:
		goto outstate;
	}
	switch (p->dl_level) {
	case DL_PROMISC_PHYS:
	case DL_PROMISC_SAP:
	case DL_PROMISC_MULTI:
		break;
	default:
		goto unsupported;
	}
	if ((err = dl_ok_ack(dl, q, mp, DL_PROMISCOFF_REQ)))
		goto error;
	return (0);
      unsupported:
	err = DL_UNSUPPORTED;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_PROMISCOFF_REQ, err);
}

static fastcall noinline __unlikely int
dl_unitdata_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto fault;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	switch (dl->proto.info.dl_mac_type) {
	case DL_X25:
		if (p->dl_dest_addr_length != 0)
			goto badaddr;
		break;
	default:
		if (p->dl_dest_addr_length != dl->proto.info.dl_addr_length)
			goto badaddr;
		break;
	}
	/* send (and absorb) the message */
	return (0);
      badaddr:
	err = DL_BADADDR;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_uderror_reply(dl, q, mp, DL_UNITDATA_REQ, err);
}

static fastcall noinline __unlikely int
dl_udqos_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_UDQOS_REQ, DL_UNSUPPORTED);
}

static fastcall noinline __unlikely int
dl_connect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_req_t *p = (typeof(p)) mp->b_rptr;
	dl_qos_co_range1_t qor;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	if (!MBLKIN(mp, p->dl_dest_addr_offset, p->dl_dest_addr_length))
		goto badaddr;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto badqos;
	switch (dl->proto.info.dl_mac_type) {
	case DL_X25:
		if (p->dl_dest_addr_length != 0)
			goto badaddr;
		break;
	default:
		if (p->dl_dest_addr_length != dl->proto.info.dl_addr_length)
			goto badaddr;
		break;
	}
	if (p->dl_qos_length) {
		if (p->dl_qos_length < sizeof(qor.dl_qos_type))
			goto fault;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qor.dl_qos_type, sizeof(qor.dl_qos_type));
		if (qor.dl_qos_type != DL_QOS_CO_RANGE1)
			goto badqostype;
		if (p->dl_qos_length < sizeof(qor))
			goto fault;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qor, sizeof(qor));
		if (qor.dl_rcv_throughput.dl_accept_value != DL_QOS_DONT_CARE)
			dl->proto.info.co.qor.dl_rcv_throughput.dl_accept_value =
			    qor.dl_rcv_throughput.dl_accept_value;
		if (qor.dl_rcv_throughput.dl_target_value != DL_QOS_DONT_CARE)
			dl->proto.info.co.qor.dl_rcv_throughput.dl_target_value =
			    qor.dl_rcv_throughput.dl_target_value;
		else
			dl->proto.info.co.qor.dl_rcv_throughput.dl_target_value =
			    qor.dl_rcv_throughput.dl_accept_value;
		if (qor.dl_rcv_trans_delay.dl_accept_value != DL_QOS_DONT_CARE)
			dl->proto.info.co.qor.dl_rcv_trans_delay.dl_accept_value =
			    qor.dl_rcv_trans_delay.dl_accept_value;
		if (qor.dl_rcv_trans_delay.dl_target_value != DL_QOS_DONT_CARE)
			dl->proto.info.co.qor.dl_rcv_trans_delay.dl_target_value =
			    qor.dl_rcv_trans_delay.dl_target_value;
		else
			dl->proto.info.co.qor.dl_rcv_trans_delay.dl_target_value =
			    qor.dl_rcv_trans_delay.dl_accept_value;
		if (qor.dl_xmt_throughput.dl_accept_value != DL_QOS_DONT_CARE)
			dl->proto.info.co.qor.dl_xmt_throughput.dl_accept_value =
			    qor.dl_xmt_throughput.dl_accept_value;
		if (qor.dl_xmt_throughput.dl_target_value != DL_QOS_DONT_CARE)
			dl->proto.info.co.qor.dl_xmt_throughput.dl_target_value =
			    qor.dl_xmt_throughput.dl_target_value;
		else
			dl->proto.info.co.qor.dl_xmt_throughput.dl_target_value =
			    qor.dl_xmt_throughput.dl_accept_value;
		if (qor.dl_xmt_trans_delay.dl_accept_value != DL_QOS_DONT_CARE)
			dl->proto.info.co.qor.dl_xmt_trans_delay.dl_accept_value =
			    qor.dl_xmt_trans_delay.dl_accept_value;
		if (qor.dl_xmt_trans_delay.dl_target_value != DL_QOS_DONT_CARE)
			dl->proto.info.co.qor.dl_xmt_trans_delay.dl_target_value =
			    qor.dl_xmt_trans_delay.dl_target_value;
		else
			dl->proto.info.co.qor.dl_xmt_trans_delay.dl_target_value =
			    qor.dl_xmt_trans_delay.dl_accept_value;
		/* FIXME: do some more */
	}
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	dl_set_state(dl, DL_OUTCON_PENDING);
	/* FIXME: send SABM/SABME/SM and absorb message */
	/* Note: before sending SABM/SABME/SM, the data link should be tested for an active channel 
	   state.  This can be accomplished using the TEST procedures for LLC and others.  Also, an 
	   XID exchange should be made to negotiate parameters before initiating the connection (if 
	   DL_AUTO_XID is specified).  Only if the XID procedure fails should default or configured 
	   values be used (unless DL_AUTO_XID is not specified).  Note that the TEST procedure can
	   also be used to determine whether oversized frames can be accepted by the remote LLC
	   SAP. */
	return (0);
      badqostype:
	err = DL_BADQOSTYPE;
	goto error;
      badqos:
	err = DL_BADQOSPARAM;
	goto error;
      badaddr:
	err = DL_BADADDR;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_CONNECT_REQ, err);
}

/**
 * dl_connect_res: - process a DL_CONNECT_RES primitive from the DLS User
 * @dl: data link private structure
 * @q: active queue (upper write queue)
 * @mp: the DL_CONNECT_RES message
 *
 * This message is in response to a DL_CONNECT_IND that was issued due to the
 * receipt of an SABM/SABME/SM command frame in the disconnected phase.  The
 * response held awaiting either a rejection (DL_DISCONNECT_REQ) or an
 * acceptance (DL_CONNECT_RES) of the connection.
 */
static fastcall noinline __unlikely int
dl_connect_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_connect_res_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto fault;
	if (!MBLKIN(mp, p->dl_qos_offset, p->dl_qos_length))
		goto fault;
	if (dl_get_state(dl) != DL_INCON_PENDING)
		goto outstate;
	if (p->dl_qos_length) {
		dl_qos_co_sel1_t qos;

		/* The quality of service parameters selected must be a dl_qos_co_sel1_t structure. 
		 */
		if (p->dl_qos_length < sizeof(qos.dl_qos_type))
			goto fault;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qos.dl_qos_type, sizeof(qos.dl_qos_type));
		if (qos.dl_qos_type != DL_QOS_CO_SEL1)
			goto badqostype;
		if (p->dl_qos_length < sizeof(qos))
			goto fault;
		bcopy(mp->b_rptr + p->dl_qos_offset, &qos, sizeof(qos));
		/* FIXME: process QOS selection. */
	}
      badqostype:
	err = DL_BADQOSTYPE;
	goto error;
      outstate:
	err = DL_OUTSTATE;
	goto error;
      fault:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_CONNECT_RES, err);
}

static fastcall noinline __unlikely int
dl_token_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_token_ack(dl, q, mp);
}

static fastcall noinline __unlikely int
dl_disconnect_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_disconnect_req_t *p = (typeof(p)) mp->b_rptr;
	int err;

	if (!MBLKIN(mp, 0, sizeof(*p)))
		goto tooshort;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
		goto discard;
	case DL_OUTCON_PENDING:
		dl_set_state(dl, DL_DISCON8_PENDING);
		break;
	case DL_INCON_PENDING:
		dl_set_state(dl, DL_DISCON9_PENDING);
		break;
	case DL_DATAXFER:
		dl_set_state(dl, DL_DISCON11_PENDING);
		break;
	case DL_USER_RESET_PENDING:
		dl_set_state(dl, DL_DISCON12_PENDING);
		break;
	case DL_PROV_RESET_PENDING:
		dl_set_state(dl, DL_DISCON13_PENDING);
		break;
	default:
		goto outstate;
	}
	return cd_disable_req(dl->cd, q, mp, CD_FLUSH);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      tooshort:
	err = -EFAULT;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_TOKEN_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_reset_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_DATAXFER)
		goto outstate;
	dl_set_state(dl, DL_USER_RESET_PENDING);
	return cd_abort_output_req(dl->cd, q, mp);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_RESET_REQ, err);
      discard:
	freemsg(mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_reset_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	int err;

	if (dl_get_state(dl) == DL_IDLE)
		goto discard;
	if (dl_get_state(dl) != DL_PROV_RESET_PENDING)
		goto outstate;
	dl_set_state(dl, DL_RESET_RES_PENDING);
	return cd_abort_output_req(dl->cd, q, mp);
      outstate:
	err = DL_OUTSTATE;
	goto error;
      error:
	return dl_error_reply(dl, q, mp, DL_RESET_RES, err);
      discard:
	freemsg(mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_data_ack_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_DATA_ACK_REQ, DL_UNSUPPORTED);
}

static fastcall noinline __unlikely int
dl_reply_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_REPLY_REQ, DL_UNSUPPORTED);
}

static fastcall noinline __unlikely int
dl_reply_update_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_REPLY_UPDATE_REQ, DL_UNSUPPORTED);
}

static fastcall noinline __unlikely int
dl_xid_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_XID_REQ, DL_NOTSUPPORTED);
}

static fastcall noinline __unlikely int
dl_xid_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_XID_RES, DL_NOTSUPPORTED);
}

static fastcall noinline __unlikely int
dl_test_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_TEST_REQ, DL_NOTSUPPORTED);
}

static fastcall noinline __unlikely int
dl_test_res(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_TEST_RES, DL_NOTSUPPORTED);
}

static fastcall noinline __unlikely int
dl_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_PHYS_ADDR_REQ, DL_NOTSUPPORTED);
}

static fastcall noinline __unlikely int
dl_set_phys_addr_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_error_reply(dl, q, mp, DL_SET_PHYS_ADDR_REQ, DL_NOTSUPPORTED);
}

static fastcall noinline __unlikely int
dl_get_statistics_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	return dl_get_statistics_ack(dl, q, mp);
}

static fastcall noinline __unlikely int
dl_wrongway_ind(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_ulong *p = (typeof(p)) mp->b_rptr;

	return dl_error_reply(dl, q, mp, *p, DL_BADPRIM);
}

static fastcall noinline __unlikely int
dl_other_req(struct dl *dl, queue_t *q, mblk_t *mp)
{
	dl_ulong *p = (typeof(p)) mp->b_rptr;

	return dl_error_reply(dl, q, mp, *p, DL_BADPRIM);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS MESSAGE HANDLING
 *
 * -------------------------------------------------------------------------
 */

static fastcall noinline __unlikely int
dl_w_data(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);	/* already locked */

	mi_strlog(q, STRLOGDA, SL_TRACE, "-> M_DATA");
	return dl_data_req(dl, q, mp);
}

static fastcall noinline __unlikely int
dl_w_proto(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_ulong prim;
	int rtn = -EFAULT;

	if (!MBLKIN(mp, 0, sizeof(prim)))
		goto done;
	prim = *(dl_ulong *) mp->b_rptr;
	dl_save_state(dl);
	mi_strlog(q, STRLOGRX, SL_TRACE, "-> %s", dl_primname(prim));
	switch (prim) {
	case DL_INFO_REQ:
		rtn = dl_info_req(dl, q, mp);
		break;
	case DL_ATTACH_REQ:
		rtn = dl_attach_req(dl, q, mp);
		break;
	case DL_DETACH_REQ:
		rtn = dl_detach_req(dl, q, mp);
		break;
	case DL_BIND_REQ:
		rtn = dl_bind_req(dl, q, mp);
		break;
	case DL_UNBIND_REQ:
		rtn = dl_unbind_req(dl, q, mp);
		break;
	case DL_SUBS_BIND_REQ:
		rtn = dl_subs_bind_req(dl, q, mp);
		break;
	case DL_SUBS_UNBIND_REQ:
		rtn = dl_subs_unbind_req(dl, q, mp);
		break;
	case DL_ENABMULTI_REQ:
		rtn = dl_enabmulti_req(dl, q, mp);
		break;
	case DL_DISABMULTI_REQ:
		rtn = dl_disabmulti_req(dl, q, mp);
		break;
	case DL_PROMISCON_REQ:
		rtn = dl_promiscon_req(dl, q, mp);
		break;
	case DL_PROMISCOFF_REQ:
		rtn = dl_promiscoff_req(dl, q, mp);
		break;
	case DL_UNITDATA_REQ:
		rtn = dl_unitdata_req(dl, q, mp);
		break;
	case DL_UDQOS_REQ:
		rtn = dl_udqos_req(dl, q, mp);
		break;
	case DL_CONNECT_REQ:
		rtn = dl_connect_req(dl, q, mp);
		break;
	case DL_CONNECT_RES:
		rtn = dl_connect_res(dl, q, mp);
		break;
	case DL_TOKEN_REQ:
		rtn = dl_token_req(dl, q, mp);
		break;
	case DL_DISCONNECT_REQ:
		rtn = dl_disconnect_req(dl, q, mp);
		break;
	case DL_RESET_REQ:
		rtn = dl_reset_req(dl, q, mp);
		break;
	case DL_RESET_RES:
		rtn = dl_reset_res(dl, q, mp);
		break;
	case DL_DATA_ACK_REQ:
		rtn = dl_data_ack_req(dl, q, mp);
		break;
	case DL_REPLY_REQ:
		rtn = dl_reply_req(dl, q, mp);
		break;
	case DL_REPLY_UPDATE_REQ:
		rtn = dl_reply_update_req(dl, q, mp);
		break;
	case DL_XID_REQ:
		rtn = dl_xid_req(dl, q, mp);
		break;
	case DL_XID_RES:
		rtn = dl_xid_res(dl, q, mp);
		break;
	case DL_TEST_REQ:
		rtn = dl_test_req(dl, q, mp);
		break;
	case DL_TEST_RES:
		rtn = dl_test_res(dl, q, mp);
		break;
	case DL_PHYS_ADDR_REQ:
		rtn = dl_phys_addr_req(dl, q, mp);
		break;
	case DL_SET_PHYS_ADDR_REQ:
		rtn = dl_set_phys_addr_req(dl, q, mp);
		break;
	case DL_GET_STATISTICS_REQ:
		rtn = dl_get_statistics_req(dl, q, mp);
		break;
	case DL_INFO_ACK:
	case DL_BIND_ACK:
	case DL_OK_ACK:
	case DL_ERROR_ACK:
	case DL_SUBS_BIND_ACK:
	case DL_TOKEN_ACK:
	case DL_PHYS_ADDR_ACK:
	case DL_GET_STATISTICS_ACK:
	case DL_UNITDATA_IND:
	case DL_UDERROR_IND:
	case DL_CONNECT_IND:
	case DL_CONNECT_CON:
	case DL_DISCONNECT_IND:
	case DL_RESET_IND:
	case DL_RESET_CON:
	case DL_DATA_ACK_IND:
	case DL_DATA_ACK_STATUS_IND:
	case DL_REPLY_IND:
	case DL_REPLY_STATUS_IND:
	case DL_REPLY_UPDATE_STATUS_IND:
	case DL_XID_IND:
	case DL_XID_CON:
	case DL_TEST_IND:
	case DL_TEST_CON:
		rtn = dl_wrongway_ind(dl, q, mp);
		break;
	default:
		rtn = dl_other_req(dl, q, mp);
		break;
	}
      done:
	if (rtn)
		dl_restore_state(dl);
	return (0);
}

static fastcall noinline __unlikely int
dl_w_ctl(queue_t *q, mblk_t *mp)
{
	/* No safe path for now... */
	return dl_w_proto(q, mp);
}

static fastcall noinline __unlikely int
dl_w_sig(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	int rtn;

	if (!mi_timer_valid(mp))
		goto done;
	dl_save_state(dl);
	switch (*(int *) mp->b_rptr) {
	case 1:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T1 TIMEOUT <-");
		rtn = dl_timeout_t1(dl, q);
		break;
	case 2:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T2 TIMEOUT <-");
		rtn = dl_timeout_t2(dl, q);
		break;
	case 3:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T3 TIMEOUT <-");
		rtn = dl_timeout_t3(dl, q);
		break;
	case 4:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T4 TIMEOUT <-");
		rtn = dl_timeout_t4(dl, q);
		break;
	case 5:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> T5 TIMEOUT <-");
		rtn = dl_timeout_t5(dl, q);
		break;
	default:
		mi_strlog(q, STRLOGTO, SL_TRACE, "-> undefined TIMEOUT <-");
		goto discard;
	}
	if (rtn) {
		dl_restore_state(dl);
		rtn = mi_timer_requeue(mp) ? rtn : 0;
	}
	return (rtn);
      done:
	return (0);
      discard:
	freemsg(mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_w_rse(queue_t *q, mblk_t *mp)
{
	freemsg(mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_w_flush(queue_t *q, mblk_t *mp)
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

static fastcall noinline __unlikely int
dl_w_ioctl(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_w_iocdata(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_w_read(queue_t *q, mblk_t *mp)
{
	putnext(q, mp);
	return (0);
}

static fastcall noinline __unlikely int
dl_w_other(queue_t *q, mblk_t *mp)
{
	mi_strlog(q, 0, SL_ERROR, "%s; bad message type on write queue %d\n", __FUNCTION__,
		  (int) DB_TYPE(mp));
	putnext(q, mp);
	return (0);
}

static fastcall inline int
dl_w_msg_put(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		return (-EAGAIN);	/* fast path for data */
	if (likely(DB_TYPE(mp) == M_PROTO))
		return (-EAGAIN);	/* fast path for data */
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
	case M_PROTO:
	case M_PCPROTO:
	case M_CTL:
	case M_PCCTL:
	case M_SIG:
	case M_PCSIG:
	case M_RSE:
	case M_PCRSE:
		return (-EAGAIN);
	case M_FLUSH:
		return dl_w_flush(q, mp);
	case M_IOCTL:
		return dl_w_ioctl(q, mp);
	case M_IOCDATA:
		return dl_w_iocdata(q, mp);
	case M_READ:
		return dl_w_read(q, mp);
	default:
		return dl_w_other(q, mp);
	}
}

static fastcall noinline int
dl_w_msg_srv(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);

	if (unlikely(dl->proto.info.dl_primitive == 0))
		return (-EAGAIN);
	if (likely(DB_TYPE(mp) == M_DATA))
		return dl_w_data(q, mp);
	if (likely(DB_TYPE(mp) == M_PROTO))
		return dl_w_proto(q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return dl_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_w_proto(q, mp);
	case M_CTL:
	case M_PCCTL:
		return dl_w_ctl(q, mp);
	case M_SIG:
	case M_PCSIG:
		return dl_w_sig(q, mp);
	case M_RSE:
	case M_PCRSE:
		return dl_w_rse(q, mp);
	case M_FLUSH:
		return dl_w_flush(q, mp);
	case M_IOCTL:
		return dl_w_ioctl(q, mp);
	case M_IOCDATA:
		return dl_w_iocdata(q, mp);
	case M_READ:
		return dl_w_read(q, mp);
	default:
		return dl_w_other(q, mp);
	}
}

static fastcall noinline int
dl_r_msg_srv(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_PROTO))
		return dl_r_proto(q, mp);
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
		return dl_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_r_proto(q, mp);
	case M_SIG:
	case M_PCSIG:
		return dl_r_sig(q, mp);
	case M_CTL:
	case M_PCCTL:
		return dl_r_ctl(q, mp);
	case M_RSE:
	case M_PCRSE:
		return dl_r_rse(q, mp);
	case M_FLUSH:
		return dl_r_flush(q, mp);
	case M_ERROR:
		return dl_r_error(q, mp);
	case M_HANGUP:
	case M_UNHANGUP:
		return dl_r_hangup(q, mp);
	case M_SETOPTS:
	case M_PCSETOPTS:
		return dl_r_setopts(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return dl_r_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return dl_r_iocack(q, mp);
	default:
		return dl_r_other(q, mp);
	}
}

static fastcall inline int
dl_r_msg_put(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_PROTO))
		return (-EAGAIN);	/* fast path for data */
	switch (DB_TYPE(mp)) {
	case M_DATA:
	case M_HPDATA:
	case M_PROTO:
	case M_PCPROTO:
	case M_SIG:
	case M_PCSIG:
	case M_CTL:
	case M_PCCTL:
	case M_RSE:
	case M_PCRSE:
		return (-EAGAIN);	/* handle from service procedure */
	case M_FLUSH:
		return dl_r_flush(q, mp);
	case M_ERROR:
		return dl_r_error(q, mp);
	case M_HANGUP:
	case M_UNHANGUP:
		return dl_r_hangup(q, mp);
	case M_SETOPTS:
	case M_PCSETOPTS:
		return dl_r_setopts(q, mp);
	case M_COPYIN:
	case M_COPYOUT:
		return dl_r_copy(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
		return dl_r_iocack(q, mp);
	default:
		return dl_r_other(q, mp);
	}
}

/*
 * --------------------------------------------------------------------------
 *
 * PUT AND SERVICE PROCEDURES
 *
 * --------------------------------------------------------------------------
 */

static streamscall int
dl_wput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || dl_w_msg_put(q, mp))
		putq(q, mp);
	return (0);
}

static streamscall int
dl_wsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (dl_w_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}

static streamscall int
dl_rsrv(queue_t *q)
{
	caddr_t priv;

	if ((priv = mi_trylock(q)) != NULL) {
		mblk_t *mp;

		while ((mp = getq(q))) {
			if (dl_r_msg_srv(q, mp)) {
				putbq(q, mp);
				break;
			}
		}
		mi_unlock(priv);
	}
	return (0);
}

static streamscall int
dl_rput(queue_t *q, mblk_t *mp)
{
	if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || dl_r_msg_put(q, mp))
		putq(q, mp);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * OPEN AND CLOSE ROUTINES
 *
 * --------------------------------------------------------------------------
 */

caddr_t dl_opens = NULL;

static int
dl_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct dl *dl;
	int err;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if ((err = mi_open_comm(&dl_opens, sizeof(*p), q, devp, oflags, sflag, crp)))
		return (err);

	dl = DL_PRIV(q);
	bzero(dl, sizeof(*dl));

	/* initialize private structure */
	dl->oq = q;
	dl->state = DL_UNATTACHED;
	dl->oldstate = DL_UNATTACHED;

	dl->proto.info.dl_primitive = DL_INFO_ACK;
	dl->proto.info.dl_max_sdu = 128;
	dl->proto.info.dl_min_sdu = 1;
	dl->proto.info.dl_addr_length = 0;	/* will be 8: 6 octets MAC + SSAP + DSAP */
	dl->proto.info.dl_mac_type = DL_OTHER;	/* will be DL_ETH_CSMA or DL_CSMACD */
	dl->proto.info.dl_reserved = 0;
	dl->proto.info.dl_current_state = DL_UNATTACHED;
	dl->proto.info.dl_sap_length = 0;	/* until bound */
	dl->proto.info.dl_service_mode = (DL_CLDLS | DL_CODLS);
	dl->proto.info.dl_qos_length = 0;
	dl->proto.info.dl_qos_offset = 0;
	dl->proto.info.dl_qos_range_length = 0;
	dl->proto.info.dl_qos_range_offset = 0;
	dl->proto.info.dl_provider_style = DL_STYLE2;
	dl->proto.info.dl_addr_offset = 0;
	dl->proto.info.dl_version = DL_CURRENT_VERSION;
	dl->proto.info.dl_brdcst_addr_length = 0;
	dl->proto.info.dl_brdcst_addr_offset = 0;
	dl->proto.info.dl_growth = 0;

	dl->proto.info.co.qos.dl_qos_type = DL_QOS_CO_SEL1;
	dl->proto.info.co.qos.dl_rcv_throughput = DL_UNKNOWN;
	dl->proto.info.co.qos.dl_rcv_trans_delay = DL_UNKNOWN;
	dl->proto.info.co.qos.dl_xmt_throughput = DL_UNKNOWN;
	dl->proto.info.co.qos.dl_xmt_trans_delay = DL_UNKNOWN;
	dl->proto.info.co.qos.dl_priority = DL_UNKNOWN;
	dl->proto.info.co.qos.dl_protection = DL_UNKNOWN;
	dl->proto.info.co.qos.dl_residual_error = DL_UNKNOWN;
	dl->proto.info.co.qos.dl_resilience.dl_disc_prob = DL_UNKNOWN;
	dl->proto.info.co.qos.dl_resilience.dl_reset_prob = DL_UNKNOWN;

	dl->proto.info.co.qor.dl_qos_type = DL_QOS_CO_RANGE1;
	dl->proto.info.co.qor.dl_rcv_throughput.dl_target_value = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_rcv_throughput.dl_accept_value = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_rcv_trans_delay.dl_target_value = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_rcv_trans_delay.dl_accept_value = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_xmt_throughput.dl_target_value = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_xmt_throughput.dl_accept_value = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_xmt_trans_delay.dl_target_value = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_xmt_trans_delay.dl_accept_value = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_priority.dl_min = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_priority.dl_max = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_protection.dl_min = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_protection.dl_max = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_residual_error = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_resilience.dl_disc_prob = DL_UNKNOWN;
	dl->proto.info.co.qor.dl_resilience.dl_reset_prob = DL_UNKNOWN;

	dl->proto.info.cl.qos.dl_qos_type = DL_QOS_CL_SEL1;
	dl->proto.info.cl.qos.dl_trans_delay = DL_UNKNOWN;
	dl->proto.info.cl.qos.dl_priority = DL_UNKNOWN;
	dl->proto.info.cl.qos.dl_protection = DL_UNKNOWN;
	dl->proto.info.cl.qos.dl_residual_error = DL_UNKNOWN;

	dl->proto.info.cl.qor.dl_qos_type = DL_QOS_CL_RANGE1;
	dl->proto.info.cl.qor.dl_trans_delay.dl_target_value = DL_UNKNOWN;
	dl->proto.info.cl.qor.dl_trans_delay.dl_accept_value = DL_UNKNOWN;
	dl->proto.info.cl.qor.dl_priority.dl_min = DL_UNKNOWN;
	dl->proto.info.cl.qor.dl_priority.dl_max = DL_UNKNOWN;
	dl->proto.info.cl.qor.dl_protection.dl_min = DL_UNKNOWN;
	dl->proto.info.cl.qor.dl_protection.dl_max = DL_UNKNOWN;
	dl->proto.info.cl.qor.dl_residual_error = DL_UNKNOWN;

	qprocson(q);
	return (0);
}

static int
dl_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct priv *p = PRIV(q);

	if (p == NULL)
		return (0);
	qprocsoff(q);
	mi_close_comm(&dl_opens, q);
	return (0);
}

/*
 * --------------------------------------------------------------------------
 *
 * STREAMS INITIALIZATION
 *
 * --------------------------------------------------------------------------
 */

static struct qinit dl_rinit = {
	.qi_putp = &dl_rput,
	.qi_srvp = &dl_rsrv,
	.qi_qopen = &dl_qopen,
	.qi_qclose = &dl_qclose,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_rstat,
};

static struct qinit dl_winit = {
	.qi_putp = &dl_wput,
	.qi_srvp = &dl_wsrv,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_wstat,
};

struct streamtab llc_info = {
	.st_rdinit = &dl_rinit,
	.st_wrinit = &dl_winit,
};

static modID_t modid = MOD_ID;

/*
 * --------------------------------------------------------------------------
 *
 * LINUX FAST STREAMS REGISTRATION
 *
 * --------------------------------------------------------------------------
 */

#ifdef LINUX

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for LLC.  (0 for allocation)");

#ifdef LFS

struct cdevsw llc_cdev = {
	.d_str = &llc_info,
	.d_flag = D_MP | D_CLONE,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

static int
llc_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&llc_cdev, major)) < 0)
		return (err);
	return (0);
}

static int
llc_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&llc_cdev, major)) < 0)
		return (err);
	return (0);
}

#endif				/* LFS */

#ifdef LIS

static int
llc_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &llc_info, UNITS, DRV_NAME)) < 0)
		return (err);
	if (major == 0)
		major = err;
	if ((err = lis_register_driver_qlock_option(major, LIS_QLOCK_NAME)) < 0) {
		lis_unregister_strdev(major);
		return (err);
	}
	return (0);
}

static int
llc_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}

#endif				/* LIS */

static __init int
llc_modinit(void)
{
	int err;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = llc_register_strdev(major)) < 0) {
		cmn_err(CE_WARN, "%s: could not register driver major %d", DRV_NAME, (int) major);
		return (err);
	}
	return (0);
}

static __exit void
llc_modexit(void)
{
	int err;

	if ((err = llc_unregister_strdev(major)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister driver, err = %d", DRV_NAME, err);
		return;
	}
	return;
}

module_init(llc_modinit);
module_exit(llc_modexit);

#endif				/* LINUX */
