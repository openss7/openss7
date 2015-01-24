/*****************************************************************************

 @(#) File: srci/drivers/np_rtp.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 *****************************************************************************/

static char const ident[] = "src/drivers/np_rtp.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/*
 *  This driver is similar to the NPI-UDP driver in that it provides NPI access to UDP; however, it
 *  is tailored for large-scale RTP use.  That is, it supports 128,000 - 512,000 RTP channels. To
 *  accomplish this it uses a specialized bottom end to discriminate UDP messages, and has an
 *  extremely limited (in size) channel structure.  Also, the driver provides the new inter-module
 *  multi-stream interface so that all 512,000 channels can be accomodated by a single stream.  To
 *  permit proper flow control on the multi-stream, it uses data-acking up the Stream.  Another
 *  specialized feature is the ability to cross-connect two RTP legs, transition a leg from
 *  receive-only to bidirectional (connectionless to connection-oriented).  Also, the driver
 *  provides automatic allocation of ports (well, port-pairs) and selection of IP addresses.  The
 *  user can also bind payload types and limit the payloads that are cross-connected.
 *
 *  This driver provides the functionality of a UDP (User Datagram Protocol) hook similary to udp
 *  sockets, with the exception that the hook acts as a port bound intercept for UDP packets for the
 *  bound addresses.  This driver is used primarily by OpenSS7 protocol test module (e.g. for CLNS
 *  over UDP) and for applications where a single port for an existing addres must be intercepted
 *  (e.g. for RTP/RTCP).  This driver uses a hook into the Linux IP protocol tables and passes
 *  packets transparently on to the underlying protocol in which it is not interested (bound).  The
 *  driver uses the NPI (Network Provider Interface) API.
 *
 *  This driver has been further modified to provide RTP/RTCP services.  One of the challenges in
 *  providing RTP/RTCP media channels is the relatively large number of channels that are needed
 *  (64,000 to 256,000 for the Optranex 248 platform, 128,000 to 512,000 for the Optranex 10G
 *  platform).  Therefore, this driver now provides a multi-stream inter-module control interface
 *  that allows the multiplexing of multiple connections (virtual NPI interfaces) over a single NPI
 *  stream.
 */

#define _DEBUG 1
#undef	_DEBUG

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>

#define np_tst_bit(nr,addr)	    test_bit(nr,addr)
#define np_set_bit(nr,addr)	    __set_bit(nr,addr)
#define np_clr_bit(nr,addr)	    __clear_bit(nr,addr)

#include <linux/interrupt.h>

#ifdef HAVE_KINC_LINUX_BRLOCK_H
#include <linux/brlock.h>
#endif

#include <linux/udp.h>

#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/inet_ecn.h>
#include <net/snmp.h>

#include <net/udp.h>
#include <net/tcp.h>		/* for checksumming */

#ifdef HAVE_KINC_NET_DST_H
#include <net/dst.h>
#endif

#include <net/protocol.h>

#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#endif				/* LINUX */

#include "net_hooks.h"

#include <sys/npi.h>
#include <sys/npi_ip.h>
#include <sys/npi_udp.h>

#define NP_DESCRIP	"Real-Time Protocol (RTP) Network Provider (NP) STREAMS Driver"
#define NP_EXTRA	"Part of the OpenSS7 VoIP Stack for Linux Fast-STREAMS"
#define NP_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define NP_REVISION	"OpenSS7 File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE
#define NP_DEVICE	"SVR 4.2 MP STREAMS NPI NP_RTP Network Provider"
#define NP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define NP_LICENSE	"GPL"
#define NP_BANNER	NP_DESCRIP	"\n" \
			NP_EXTRA	"\n" \
			NP_REVISION	"\n" \
			NP_COPYRIGHT	"\n" \
			NP_DEVICE	"\n" \
			NP_CONTACT
#define NP_SPLASH	NP_DESCRIP	"\n" \
			NP_REVISION

#ifdef LINUX
MODULE_AUTHOR(NP_CONTACT);
MODULE_DESCRIPTION(NP_DESCRIP);
MODULE_SUPPORTED_DEVICE(NP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(NP_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-np_rtp");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif
#endif				/* LINUX */

#define NP_DRV_ID	CONFIG_STREAMS_NP_RTP_MODID
#define NP_DRV_NAME	CONFIG_STREAMS_NP_RTP_NAME
#define NP_CMAJORS	CONFIG_STREAMS_NP_RTP_NMAJORS
#define NP_CMAJOR_0	CONFIG_STREAMS_NP_RTP_MAJOR
#define NP_UNITS	CONFIG_STREAMS_NP_RTP_NMINORS

#ifdef LINUX
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_NP_RTP_MODID));
MODULE_ALIAS("streams-driver-np_rtp");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_NP_RTP_MAJOR));
MODULE_ALIAS("/dev/streams/np_rtp");
MODULE_ALIAS("/dev/streams/np_rtp/*");
MODULE_ALIAS("/dev/streams/clone/np_rtp");
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_CLONE_MAJOR) "-"
	     __stringify(CONFIG_STREAMS_NP_RTP_MAJOR));
MODULE_ALIAS("/dev/np_rtp");
//MODULE_ALIAS("devname:np_rtp");
#endif				/* defined MODULE_ALIAS */
#endif				/* defined LINUX */

/*
 *  ==========================================================================
 *
 *  STREAMS Definitions
 *
 *  ==========================================================================
 */

#define DRV_ID		NP_DRV_ID
#define DRV_NAME	NP_DRV_NAME
#define CMAJORS		NP_CMAJORS
#define CMAJOR_0	NP_CMAJOR_0
#define UNITS		NP_UNITS
#ifdef MODULE
#define DRV_BANNER	NP_BANNER
#else				/* MODULE */
#define DRV_BANNER	NP_SPLASH
#endif				/* MODULE */

STATIC struct module_info np_minfo = {
	.mi_idnum = DRV_ID,	/* Module ID number */
	.mi_idname = DRV_NAME,	/* Module name */
	.mi_minpsz = 0,		/* Min packet size accepted */
	.mi_maxpsz = (1 << 16),	/* Max packet size accepted */
	.mi_hiwat = (1 << 18),	/* Hi water mark */
	.mi_lowat = (1 << 16),	/* Lo water mark */
};

STATIC struct module_stat np_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
STATIC struct module_stat np_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

/* Upper multiplex is a N provider following the NPI. */

STATIC streamscall int np_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int np_qclose(queue_t *, int, cred_t *);

streamscall int np_rput(queue_t *, mblk_t *);
streamscall int np_rsrv(queue_t *);

STATIC struct qinit np_rinit = {
	.qi_putp = np_rput,	/* Read put procedure (message from below) */
	.qi_srvp = np_rsrv,	/* Read service procedure */
	.qi_qopen = np_qopen,	/* Each open */
	.qi_qclose = np_qclose,	/* Last close */
	.qi_minfo = &np_minfo,	/* Module information */
	.qi_mstat = &np_rstat,	/* Module statistics */
};

streamscall int np_wput(queue_t *, mblk_t *);
streamscall int np_wsrv(queue_t *);

STATIC struct qinit np_winit = {
	.qi_putp = np_wput,	/* Write put procedure (message from above) */
	.qi_srvp = np_wsrv,	/* Write service procedure */
	.qi_minfo = &np_minfo,	/* Module information */
	.qi_mstat = &np_wstat,	/* Module statistics */
};

MODULE_STATIC struct streamtab np_info = {
	.st_rdinit = &np_rinit,	/* Upper read queue */
	.st_wrinit = &np_winit,	/* Upper write queue */
};

#if !defined HAVE_KMEMB_STRUCT_SK_BUFF_TRANSPORT_HEADER
#if !defined HAVE_KFUNC_SKB_TRANSPORT_HEADER
static inline unsigned char *
skb_tail_pointer(const struct sk_buff *skb)
{
	return skb->tail;
}

static inline unsigned char *
skb_end_pointer(const struct sk_buff *skb)
{
	return skb->end;
}

static inline unsigned char *
skb_transport_header(const struct sk_buff *skb)
{
	return skb->h.raw;
}

static inline unsigned char *
skb_network_header(const struct sk_buff *skb)
{
	return skb->nh.raw;
}

static inline unsigned char *
skb_mac_header(const struct sk_buff *skb)
{
	return skb->mac.raw;
}

static inline void
skb_reset_tail_pointer(struct sk_buff *skb)
{
	skb->tail = skb->data;
}

static inline void
skb_reset_end_pointer(struct sk_buff *skb)
{
	skb->end = skb->data;
}

static inline void
skb_reset_transport_header(struct sk_buff *skb)
{
	skb->h.raw = skb->data;
}

static inline void
skb_reset_network_header(struct sk_buff *skb)
{
	skb->nh.raw = skb->data;
}

static inline void
skb_reset_mac_header(struct sk_buff *skb)
{
	skb->mac.raw = skb->data;
}

static inline void
skb_set_tail_pointer(struct sk_buff *skb, const int offset)
{
	skb_reset_tail_pointer(skb);
	skb->tail += offset;
}

static inline void
skb_set_transport_header(struct sk_buff *skb, const int offset)
{
	skb_reset_transport_header(skb);
	skb->h.raw += offset;
}

static inline void
skb_set_network_header(struct sk_buff *skb, const int offset)
{
	skb_reset_network_header(skb);
	skb->nh.raw += offset;
}

static inline void
skb_set_mac_header(struct sk_buff *skb, const int offset)
{
	skb_reset_mac_header(skb);
	skb->mac.raw += offset;
}
#endif				/* !defined HAVE_KFUNC_SKB_TRANSPORT_HEADER */
#endif				/* !defined HAVE_KMEMB_STRUCT_SK_BUFF_TRANSPORT_HEADER */

/*
 *  Primary data structures.
 */

/*
 *  ----------------------------------------------------------------------------------------------
 *  Network Bind Structure:
 *  ----------------------------------------------------------------------------------------------
 *  The network bind structure is a separate structure that is used for aging port numbers.  The
 *  reason for a separate structure from the network connection structure is so that we use a
 *  separate data cache from the the network connection cache.
 */

struct nb {
	struct nb *next;		/* linkage for free list */
	uint16_t port;			/* port number (host order) */
	unsigned long stamp;		/* timestamp when made free */
};

/*
 *  ----------------------------------------------------------------------------------------------
 *  Network Connection structure:
 *  ----------------------------------------------------------------------------------------------
 *  The network connection structure is separate from a Stream private structure and represents the
 *  state and options of a single interface.  A single base network connection structure is
 *  associated with a single-interface Stream.  Multiple network connection structures are
 *  associated with a multiple-interface Stream.  Network connection structures are memory cache
 *  allocated.  When they are bound (connectionless) or connected (connection-oriented), they are
 *  placed in the port map for the local address to which they are bound.  When they are bound but
 *  have no address assigned (disconnected connection-oriented stream), they do not yet exist in the
 *  port maps.  Network connections are linked into a linked list associated with the owning Stream
 *  private structure (np).
 *
 *  Streams of service type N_CLNS that are in the NS_IDLE state are included in the port maps and
 *  can receive packets.  Packets are delivered upstream (or cross-connected) as N_UNIDATA_IND
 *  primitives.  Streams of service type N_CONS that are in the NS_DATA_XFER state are included in
 *  the port maps and can receive packets.  Pacets are delivered upstream (or cross-connected) as
 *  N_DATA_IND primnitives.  When the @link member is not NULL, the NC is cross-connected with
 *  another NC.  Cross connections cause all received data of an appropriate payload type to be
 *  transferred to the other leg.  Appropriate types are payload types to which the other NC is
 *  bound.
 */
struct np;

struct nc {
	unsigned long tag;		/* client tag for this NC */
	struct np *np;			/* Stream for this NC */
	struct nc *next;		/* linkage for stream structure */
	struct nc **prev;		/* linkage for stream structure */
	struct nc *link;		/* linkage for cross-connected channel */
	struct nb *nb;			/* bind structure for this NC */

	/* Note that we should try to collapse these into a single 32-bit word to save space and increase
	   performance.  The NPI state is never more than 31 and can be represented in 5 bits.  BIND_flags
	   and CONN_flags are only a couple of bits.  Accessor functions should be used for these fields so
	   that we can move them around and compress them in various ways. */

	union {
		struct {
			np_ulong CURRENT_state:5;	/* current NPI state of the NC */
			np_ulong SERV_type:2;	/* service type for the NC */
			np_ulong BIND_flags:3;	/* bind flags for the NC: zero when unbound */
			np_ulong CONN_flags:1;	/* connection flags for the NC; zero when disconnected */
		} state;

		np_ulong fullstate;	/* combined state and flags */
	} u;

	int8_t protoids[8];		/* up to 8 payload types: unused entries are set to -1 */
	uint32_t saddr;			/* address of the nc or INADDR_ANY when not assigned */
	uint32_t daddr;			/* address of the peer or INADDR_ANY when not connected */
	uint16_t sport;			/* port assignment (host byte order), zero when unassigned */
	uint16_t dport;			/* port assignment (host byte order), zero when unassigned */
	uint16_t datinds;		/* number of outstanding data indications */
	uint16_t resinds;		/* number of outstanding reset indications */

#if 0
	/* These are only required if we are generating connection indications.  I don't think that this is
	   so useful. */
	np_ulong CONIND_number;		/* maximum number of oustanding connection indications */
	bufq_t conq;			/* list of outstanding connection indications */
#endif
};

/*
 *  The purpose of the following accessor functions is to keep the per-channel structure as
 *  compressed as possible.  This will increase performance.
 */

static inline fastcall np_ulong
nc_get_CURRENT_state(struct nc *nc)
{
	return (nc->u.state.CURRENT_state);
}

static inline fastcall np_ulong
nc_set_CURRENT_state(struct nc *nc, np_ulong state)
{
	np_ulong oldstate = nc_get_CURRENT_state(nc);

	nc->u.state.CURRENT_state = state;
	return (oldstate);
}

static inline fastcall np_ulong
nc_get_SERV_type(struct nc *nc)
{
	return (nc->u.state.SERV_type);
}

static inline fastcall np_ulong
nc_set_SERV_type(struct nc *nc, np_ulong servtype)
{
	int oldtype = nc_get_SERV_type(nc);

	nc->u.state.SERV_type = servtype;
	return (oldtype);
}

static inline fastcall np_ulong
nc_get_BIND_flags(struct nc *nc)
{
	return (nc->u.state.BIND_flags);
}

static inline fastcall np_ulong
nc_set_BIND_flags(struct nc *nc, np_ulong flags)
{
	np_ulong oldflags = nc_get_BIND_flags(nc);

	nc->u.state.BIND_flags = flags;
	return (oldflags);
}

static inline fastcall np_ulong
nc_get_CONN_flags(struct nc *nc)
{
	return (nc->u.state.CONN_flags);
}

static inline fastcall np_ulong
nc_set_CONN_flags(struct nc *nc, np_ulong flags)
{
	int oldflags = nc_get_CONN_flags(nc);

	nc->u.state.CONN_flags = flags;
	return (oldflags);
}

/*
 *  ----------------------------------------------------------------------------------------------
 *  Private structure:
 *  ----------------------------------------------------------------------------------------------
 *  The private structure contains one network connection structure that is used for
 *  single-interface Streams (M_PROTO/M_PCPROTO).  It also has a pointer to a list of allocated
 *  network connection structures that are used for multiple-interface Streams (M_CTL/M_PCCTL).
 *  Aside from that it contains pointers to the input and output queues as well as NS provider
 *  information and quality of service parameters.
 */
struct np {
	queue_t *oq;			/* output (read) queue */
	queue_t *iq;			/* input (write) queue */
	struct nc *nc;			/* network connections for multiple-interface streams */
	struct nc base;			/* network connection for single-interface streams */
	N_info_ack_t info;		/* network service provider information */
	N_qos_sel_info_rtp_t qos;	/* NS provider QOS */
};

#define PRIV(__q) (((__q)->q_ptr))
#define NP_PRIV(__q) ((struct np *)((__q)->q_ptr))

/*
 *  ----------------------------------------------------------------------------------------------
 *  Address Structure:
 *  ----------------------------------------------------------------------------------------------
 *  The address structure holds all of the information associated directly with a specific address.
 *  Each address has a network connection map structure that maps network connection pointers for
 *  each of the ports available for use under RTP for that address (port even and odd pairs from
 *  1024 to 65534).
 */

struct np_addr_map;

struct np_addr_info {
	struct np_addr_map *map;	/* address port map */
	uint16_t clns_lo_port;		/* lowest assignable connectionless port */
	uint16_t clns_hi_port;		/* highest assignable connectionless port */
	uint16_t cons_lo_port;		/* lowest assignable connection-oriented port */
	uint16_t cons_hi_port;		/* highest assignable connection-oriented port */
	uint16_t clns_port;		/* next assignment of connectionless port */
	uint16_t cons_port;		/* next assignment of connection-oriented port */
	struct nb *clns_head;		/* list of assigned connectionless bind structures */
	struct nb **clns_tail;		/* list of assigned connectionless bind structures */
	struct nb *cons_head;		/* list of assigned connection-oriented bind structures */
	struct nb **cons_tail;		/* list of assigned connection-oreinted bind structures */
	unsigned long clns_guard;	/* CLNS guard time in HZ.  Default 5 seconds. */
	unsigned long cons_guard;	/* CONS guard time in HZ.  Default 0 seconds. */
	N_qos_sel_info_rtp_t qos;	/* addresss-related QOS parameters (note that the daddr and dport
					   fields holds the assigned IPv4 address and lowest available port
					   number) */
};

struct np_addr_bucket {
	uint32_t addr;			/* 32-bit IPv4 address */
};

/* this is about 1 page of structures */
static int np_addr_num = 0;		/* number of allocated addresses 0 to 64. */
static struct np_addr_bucket np_addrs[64] __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct np_addr_info np_infos[64] __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static inline fastcall __hot_in void
np_addr_prefetch(uint8_t index)
{
	prefetch(&np_addrs[index]);
	prefetch(&np_infos[index]);
}

static inline fastcall __hot_in int
np_addr_get_bucket(uint32_t addr)
{
	int i;

	for (prefetch(&np_addrs[1]), i = 0; i < np_addr_num; prefetch(&np_addrs[i + 2]), i++)
		if (np_addrs[i].addr == addr) {
			prefetch(&np_infos[i]);
			return (i);
		}
	return (-1);
}

/** np_addr_lookup: looks up a network connection by address and port number
  * @addr: the 32-bit IPv4 address to look up
  * @port: the 16-bit UDP port number to look up
  *
  * This function returns a pointer to the network connection structure, or NULL when no network
  * connection structure is assigned to the address and port number.
  */
static inline fastcall __hot_in struct nc *
np_addr_lookup(uint32_t addr, uint16_t port)
{
	struct nc *nc = NULL;

	if (likely(addr != 0)) {
		int i;

		for (prefetch(&np_addrs[1], i = 0; i < 64; prefetch(&np_addrs[i + 2]), i++)) {
			uint32_t test = np_addrs[i];

			if (unlikely(test == INADDR_ANY))
				break;
			if (np_addrs[i].addr == addr) {
				struct nc **ncpp;

				nccp = (struct nc **) &np_infos[i].map + ((port >> 1) - 512);
				if (likely((nc = *ncpp) != NULL)) {
					nc = *ncpp;
					prefetch(nc);
					break;
				}

			}
		}
	}
	return (nc);
}

/** np_find_azddress - find an address in the address list
  * @addr: the 32-bit IPv4 address to find
  *
  * This function returns the (positive or zero) index of the address or a negative error number if
  * the search fails.
  */
static fastcall int
np_find_address(uint32_t addr)
{
	if (likely(addr != 0)) {
		int i;

		for (prefetch(&np_addrs[1], i = 0; i < 64; prefetch(&np_addrs[i + 2]), i++)) {
			uint32_t test = np_addrs[i];

			if (unlikely(test == INADDR_ANY))
				break;
			if (np_addrs[i].addr == addr) {
				prefetchw(np_infos[i].map);
				return (i);
			}
		}
	}
	return (-ESRCH);
}

/** np_add_address: add an address to the address list
  * @addr: the 32-bit IPv4 address to add
  *
  * This function allocated port mapping memory for the IPv4 address using __get_free_pages().  This
  * function returns the (positive or zero) index of the newly allocated address or a negative error
  * number if allocation failed.
  */
static fastcall int
np_add_address(uint32_t addr)
{
	int index, order;
	struct np_addr_map *map;
	struct np_addr_info *ni;
	struct np_addr_bucket *na;

	if (unlikely((index = np_addr_num) >= 64))
		return (-ENOSR);

	order = 15 + (sizeof(void *) == 4) ? 2 : 3;
	map = (struct np_addr_map *) __get_free_pages(GFP_ATOMIC, order);
	bzero(map, (1 << order));

	if (unlikely(map == NULL))
		return (-ENOMEM);

	ni = &np_infos[index];

	ni->map = map;
	ni->clns_head = NULL;
	ni->clns_tail = &ni->clns_head;
	ni->clns_guard = 5 * HZ;	/* should probably be in milliseconds */
	ni->clns_lo_port = 16536;
	ni->clns_hi_port = 32766;
	ni->clns_port = 32766;
	ni->cons_head = NULL;
	ni->cons_tail = &ni->cons_head;
	ni->cons_guard = 0 * HZ;	/* should probably be in milliseconds */
	ni->cons_lo_port = 1024;
	ni->cons_hi_port = 65534;
	ni->cons_port = 65534;
	ni->qos.n_qos_type = N_QOS_SEL_INFO_RTP;
	ni->qos.protocol = IPPROTO_UDP;
	ni->qos.priority = 0;
	ni->qos.ttl = 64;
	ni->qos.tos = 0;
	ni->qos.mtu = 536;
	ni->qos.checksum = 0;
	ni->qos.daddr = addr;
	ni->qos.dport = 0;

	na = &np_addrs[index];

	na->addr = addr;

	np_addr_num++;
	return (index);
}

/** np_del_address: delete address of specified index
  * @index: the index at which to delete the address
  *
  * This function collapses the list of IP addresses assigned and deletes the specified address.
  * Note that the lookup structures need to be write locked.  Also, any interfaces that are assigned
  * ports for the address must be removed.  Addresses cannot be deleted unless they are no longer
  * being used.  In fact, we should probably not delete them at all unless we are removing the
  * module.  This function frees the mapping memory for the IPv4 address using free_pages().
  */
static fastcall int
np_del_address(int index)
{
	struct np_addr_info *ni;
	struct np_addr_bucket *na;
	struct np_addr_map *map;
	struct nc *nc, *nc_next;
	int i, j, order;

	order = 15 + (sizeof(void *) == 4) ? 2 : 3;
	map = xchg(&np_infos[index].map, NULL);
	free_pages((unsigned long) map, order);

	nc_next = xchg(&np_infos[index].clns_head, NULL);
	while ((nc = nc_next) != NULL) {
		nc_next = nc->next;
		kmem_cache_free(nc_struct_cache, nc);
	}

	nc_next = xchg(&np_infos[index].cons_head, NULL);
	while ((nc = nc_next) != NULL) {
		nc_next = nc->next;
		kmem_cache_free(nc_struct_cache, nc);
	}

	for (i = index, j = i + 1; j < 64; i++, j++) {
		bcopy(&np_addrs[j], &np_addrs[i], sizeof(np_addrs[0]));
		bcopy(&np_infos[j], &np_infos[i], sizeof(np_infos[0]));
	}

	ni = &np_infos[j];

	ni->map = NULL;
	ni->clns_head = NULL;
	ni->clns_tail = &ni->clns_head;
	ni->clns_guard = 5 * HZ;	/* should probably be in milliseconds */
	ni->clns_lo_port = 16536;
	ni->clns_hi_port = 32766;
	ni->clns_port = 32766;
	ni->cons_head = NULL;
	ni->cons_tail = &ni->cons_head;
	ni->cons_guard = 0 * HZ;	/* should probably be in milliseconds */
	ni->cons_lo_port = 1024;
	ni->cons_hi_port = 65534;
	ni->cons_port = 65534;
	ni->qos.n_qos_type = N_QOS_SEL_INFO_RTP;
	ni->qos.protocol = IPPROTO_UDP;
	ni->qos.priority = 0;
	ni->qos.ttl = 64;
	ni->qos.tos = 0;
	ni->qos.mtu = 536;
	ni->qos.checksum = 0;
	ni->qos.daddr = addr;
	ni->qos.dport = 0;

	na = &np_addrs[j];

	na->addr = INADDR_ANY;

	np_addr_num--;

}

/*
 *  ----------------------------------------------------------------------------------------------
 *  Port allocation.
 *  ----------------------------------------------------------------------------------------------
 */

/** np_get_port - allocate a port
  * @index: address index
  * @SERV_type: service type
  *
  * I want to provide some degree of guard time between the last deallocation of a port and the next
  * allocation of the same port.  That is, I do not want to immediately allocate the same port that
  * was just deallocated.  The aging must begin when the port is released rather than being based on
  * when the port was siezed.  This makes a deterministic selection impossible.  The olp port must
  * be frozen for a period of time.  This is accomplished by keeping the NC structure around after
  * it has been released for a period of time before it is available for reallocation.  When it is
  * reallocated, the port number will be reused.
  *
  * I almost never deallocate NC structures (unless the module is unloaded, of course).  When a new
  * port is bound, the old structure could be resused, removing the burden of allocating and
  * deallocating NC structures during active switching.  The NC structures that have been released
  * can be placed on the back of the free list.  The port allocation procedures can then remove a
  * structure from the front of the free list provided that it has been on the free list (or
  * otherwise unavailable) for a sufficient guard time.  On problem with reusing the structure is
  * that we want to allocate cross-connected NC structures as pairs (one immediately after the
  * other) so that they are likely to be located closely together (on the same cache page) and
  * increase data cach efficiency.  Therefore, I reallocated and then deallocate the old structure.
  *
  * When there are port numbers available that hvae not yet been used, I allocate from the freed
  * port space first.  When allocating new port numbers, I start at the opt (maximum 65,534) and
  * wokr downward (to a minimum of 1,024).  When a port number is on the free list, I use that
  * instead of allocating a new port.  This is so that we use dthe minimum range of ports for each
  * IP address at the top end of the port range (to keep out of the way of Linux other uses for
  * UDP).  Once all of the port numbers have been allocated once, they can only be taken from the
  * free list, and only after a sufficient guard time.  There is a possible 32,256 port allocations
  * per address.  We need 128,000 port allocation to support 64,000 channels of cross-connect or
  * transcoding, or, or a minimum of 4 IP addresses.  With 32,256 availble channels per IP address,
  * with an average holding time of 120 seconds, there will be a new port becoming available 267
  * times a second.  With a guard time of 20 seconds, 5,376 ports will always be unavailable for
  * use.  With a guard time of 5 seconds, 1,344 ports will always be unavailable for use.  By
  * allocating twice as many IP addresses as necessary, the guard time could even be extended to
  * 120 seconds.  We will start by assuming a 5 second guard time as this seems means that only 4%
  * of the memory cache will be tied up with stagnant NC structures.
  *
  * Note that the guard time is not required when connection-oriented channel legs are allocated.
  * This is because the only unguarded connection is a connectionless one.  That is, a port is
  * allocated as connectionless immediately after being released and RTP packets arrive to the port
  * from a previoulsy conection that has not yet terminated.  For connection-oriented, there is no
  * need because the packets will be discarded unless they are from the self-same port number at the
  * remote end.
  *
  * Another advantage of this approach is that port allocations will quickly become stoichastic
  * rather than deterministic, making it impossible for an attacker to predict which port will be
  * allocated next.  Also, we only need to keep track of the highest unallocated port number.  A
  * slight variation of the approach is to not require a guard time, but to simply pic the oldest
  * port from the free list.  This has all the stoichastic and speed port selection advantages; and
  * still the guard time can be enforced by the user by not releasing the channels involved until
  * after the guard time has expired.  This relieves use of the need to maintain a timestamp in each
  * NC structure.
  *
  * Note that the default port range for 2.6.32 kernels is 32,768 to 61,000.  Unfortunately, the
  * 2.6.32 kerne also performs port number randomization within this range.  Perhaps the best thing
  * for us to do is to assign port numbers in the range from 16,384 to 32,766 for a total of 8,192
  * port assignments, and from 61,002 to 65,534 for 2,266 more.  This will provide a total of
  * 10,458 port assignments for connectionless and 21,798 port assignments for connection-oriented,
  * or a total of 32,256 port assignments.  Unfortunately, in a typical arrangement, some IP
  * addresses will likely be reserved for internal network used (and possibly be all
  * connectionless), and others will normally be used for external network (and possibly be all
  * connection-oriented).
  *
  * A conflict between Linux UDP and RTP only occurs when the port is bound connectionless to avoid
  * clipping early media.  When ports are for connection-oriented use only, the remote IP address
  * and port number will allow packets belonging to some Linux UDP socket to bypass correctly.  The
  * only problem is that, when bound connectionless and the packet cannot be distinguished from an
  * RTP packet, there can be a conflict.
  *
  * To avoid this, the user can take care that IP addreses used are used only for RTP.  Another
  * possibility is to never use connectionless and rely upon proper SIP messaging to establish a
  * full connection before media is passed.  I provide a separate port range that will be used by
  * each address: one range for connectionless and another for connection-oriented.  In this way, IP
  * addresses that require connectionless can have their connectionless port range set to 16,384 to
  * 32,766 and connection-oriented port range set to 1,024 to 65,534.  The connectionless port range
  * will be skipped when allocating connection-oriented ports.  For IP adresseses that do not
  * require connectionless, the connectionless port range can be set to 0 to 0, and the
  * connection-oriented port range of 1,024 to 65,534 will then use all available RTP ports.
  *
  * Using the approach detailed above, this function checks the address' free list for an available
  * port.  When a port is available on the free list, unlink it and use the port number.  The NC
  * structure should be freed and reallocated for data cache efficiency.  When no port is available
  * on the free list, see if there are still new usable ports in the port range.  If so, allocate
  * one and decrement the assigned port number (used_port).
  *
  * The lowest (even) unallocated port number is in lo_port.  When this value is zero (0) (or less
  * than 1024) then there are no port numbers available.  The highest (even) allocated port number
  * is in hi_port.  When this value is 65534
  *
  * RETURN: Returns the allocated (positive) port number or (negative) error number.  The only error
  * number that is returned is when there are no available ports left for the address.
  *
  */
static fastcall int
np_get_port(int index, struct nc *nc, np_ulong SERV_type)
{
	struct np_addr_bucket *na;
	struct np_addr_info *ni;
	struct nb *nb;
	int err = 0;

	ni = &np_infos[index];
	na = &np_addrs[index];

	switch (SERV_type & (N_CONS | N_CLNS)) {
	case 0:
	case N_CLNS:
	case (N_CONS | N_CLNS):
		if (likely((nb = ni->clns_head) != NULL && (jiffies - nb->stamp) >= ni->clns_guard)) {
			/* Check if there is a suitably aged port number on the free list.  If there is,
			   remove the structure from the free list and assign that port number.  Free the old 
			   structure. Free lists for CLNS and CONS are handled differently. */
			if ((ni->clns_head = nb->next) == NULL)
				ni->clns_tail = &ni->clns_head;
			nc->saddr = na->addr;
			nc->sport = htons(nb->port);
			nc->nb = nb;
			nb->next = NULL;
		} else if (likely(ni->clns_port > ni->clns_lo_port)) {
			/* When there is no available free port number, check whether we still have available 
			   port numbers in the range.  If we do, allocate the next one and decrement the
			   allocation mark. */
			if (likely((nb = kmem_cache_alloc(nb_struct_cache, GFP_ATOMIC)) != NULL)) {
				bzero(nb, sizeof(*nb));
				nb->port = ni->clns_port;
				nc->saddr = na->addr;
				nc->sport = htons(nb->port);
				ni->clns_port -= 2;
			} else {
				err = -ENOMEM;
			}
		} else {
			err = -ESRCH;
		}
		break;
	case N_CONS:
		if (likely((nb = ni->cons_head) != NULL && (jiffies - nb->stamp) >= ni->cons_guard)) {
			/* Check if there is a suitably aged port number on the free list.  If there is,
			   remove the structure from the free list and assign that port number.  Free the old 
			   structure. Free lists for CLNS and CONS are handled differently. */
			if ((ni->cons_head = nb->next) == NULL)
				ni->cons_tail = &ni->cons_head;
			nc->saddr = na->addr;
			nc->sport = htons(nb->port);
			nc->nb = nb;
			nb->next = NULL;
		} else if (likely(ni->cons_port > ni->cons_lo_port)) {
			/* When there is no available free port number, check whether we still have available 
			   port numbers in the range.  If we do, allocate the next one and decrement the
			   allocation mark. */
			if (likely((nb = kmem_cache_alloc(nb_struct_cache, GFP_ATOMIC)) != NULL)) {
				bzero(nb, sizeof(*nb));
				nb->port = ni->cons_port;
				nc->saddr = na->addr;
				nc->sport = htons(nb->port);
				ni->cons_port -= 2;
				/* When allocating for CONS, skip the port range reserved for CLNS. */
				if (ni->cons_port == ni->clns_hi_port)
					ni->clns_lo_port - 2;
			} else {
				err = -ENOMEM;
			}
		} else {
			err = -ESRCH;
		}
		break;
	}
	return (err);
}

/** np_put_port - release a port
  * @index: address index
  * @nc: the NC structure to release
  *
  * Release a port for use by an RTP channel by placing the bind structure on the address' free
  * list.  Note that the bind structure does not need to be cache line aligned because it is seldom
  * used.  The bind structure is about 20 bytes long, and so even when there is 32,256 ports
  * assigned per IPv4 address, that is 630k bytes of memory.  With a maximum allocation of 128,000
  * ports, that is 2.46M of memory.  If we allocate the bind structure when placing it on the list,
  * memory will only be used while idle.  When operating, only 100.8k of memory will be used
  * (assuming a 5 second guard time which translates to 4 percent of the ports waiting in guard).
  * When not using a guard time, or when connection-oriented is used predominately, memory
  * consumption for bind will be zero.
  *
  */
static fastcall void
np_put_port(int index, struct nc *nc)
{
	struct np_addr_info *ni;
	struct nb *nb;

	ni = &np_infos[index];

	dassert(nc->nb != NULL);
	if (likely((nb = xchg(&nc->nb, NULL)) != NULL)) {

		nb->next = NULL;	/* for safety */
		nb->stamp = jiffies;	/* timestamp it was freed */

		if (ni->clns_hi_port >= nb->port && ni->clns_lo_port <= nb->port) {
			/* When the port number is in the CLNS range, place the NC structure onto the free
			   list for CLNS. */
			*(ni->clns_tail) = nb;
			ni->clns_tail = &nb->next;
		} else {
			/* Otherwise, place the NC structure on the list for CONS. */
			*(ni->cons_tail) = nb;
			ni->cons_tail = &nb->next;
		}
	}
}

/*
 *  ----------------------------------------------------------------------------------------------
 *  Network Connection map:
 *  ----------------------------------------------------------------------------------------------
 *  The network connection map is a map established by IP address that provides a pointer to each
 *  port number allocated to the IP address.  This is laid out as a simply array of pointers, one
 *  array for each IP address.  There is an IP address map that provides the pointer into this
 *  array.  The array is offset by 512 locations (1024 ports) because the first 512 locations are
 *  unusable as IP ports for RTP.  The array has 32,768 (2^15) - 512, or 32,256 locations.  Each
 *  location contans a pointer to a network connection structure.
 */

struct nc;

struct np_addr_map {
	struct nc *ports[(1 << 15) - 512];
};

static inline fastcall __hot void
np_addr_prefetch(struct np_addr_map *addrmap, unsigned short port)
{
	prefetch(&addrmap->ports[(port >> 1) - 512]);
}

static inline fastcall __hot struct nc *
np_addr_fetch(struct np_addr_map *addrmap, unsigned short port)
{
	dassert(port >= 1024);
	return addrmap->ports[(port >> 1) - 512];
}

static inline fastcall struct nc *
np_addr_store(struct np_addr_map *addrmap, unsigned short port, struct nc *nc)
{
	return xchg(&addrmap->ports[(port >> 1) - 512], nc);
}

/*
 *  ----------------------------------------------------------------------------------------------
 *  Port map:
 *  ----------------------------------------------------------------------------------------------
 *  The port map is a 4k page of bits with the first 512 bits (64 bytes) unused for port mapping and
 *  usable for adminstrative storage (i.e. the number of bits mapped, the number of ports mapped).
 *  There is one port map structure for all IP address (for the entire driver).  It is page
 *  allocated to keep the map in a single memory page.
 */

struct np_portmap {
	union {
		struct {
			int bits_set;
			int ports_set;
		} data;
		int __bits[16];
	} u;
	int bits[1024 - 16];
};

static struct np_portmap *np_portmap = NULL;

/** np_portmap_prefetch: prefetch the port map
  *
  * This should be called (in np_v4_rcv) as soon as the np_v4_rcv is entered.
  */
static inline fastcall __hot void
np_portmap_prefetch()
{
	prefetch(np_portmap);
}

/** np_port_prefetch: prefetch bit map for given port
  *
  * This should be called (in np_v4_rcv) as soon as the port number can be obtained.  The port
  * number should be obtained as early as possible and before other checks are made to ensure that
  * the prefectch completes as soon as possible.
  */
static inline fastcall __hot void
np_port_prefetch(unsigned short port)
{
	prefetch(np_portmap + (port >> 6));
}

/** np_port_check: checks whether a port is assigned
  */
static inline fastcall __hot int
np_port_check(unsigned short port)
{
	if (unlikely(port < 1024))
		return (0);
	return test_bit((int *) nc_portmap, (port >> 1));
}

/** np_port_set: set a bit in the port map
  * @port: the port for which to set the bit
  *
  * This function does not necessarily require a write lock, not even a read lock because the
  * structure cannot go away unless the module is unloading.
  */
static void int
np_port_set(unsigned short port)
{
	dassert(port >= 1024);
	if (!test_and_set_bit((int *) np_portmap, (port >> 1)))
		np_portmap->bits_set++;
}

/** np_port_clr: clear a bit in the port map
  * @port: the port for which to clear the bit
  *
  * This function does not necessarily require a write lock, not even a read lock because the
  * structure cannot go away unless the module is unloading.
  */
static void int
np_port_clr(unsigned short port)
{
	dassert(port >= 1024);
	clear_bit((int *) np_portmap, (port >> 1));
	np_portmap->bits_set--;
}

/*
 *  ----------------------------------------------------------------------------------------------
 *  Utility functions for adding and removing address mappings.
 *  ----------------------------------------------------------------------------------------------
 */

/** nc_add - add a NC structure to the address/port maps
  * @addr: the 32-bit IPv4 address
  * @port: the 16-bit UDP port
  * @nc: the NC struture to add
  *
  * Adds the NC structure specified by @nc to the adddress and port mappings for use by the lookup
  * function.  This utility must be called with the address/port mapping lock held for write.  This
  * function returns the (positive or zero) index of the address, or a (negative) error number.
  * This function only returns an error when memory could not be allocated or the function was
  * passed an invalid address.
  */
static fastcall int
nc_add(uint32_t addr, uint16_t port, struct nc *nc)
{
	int index;

	dassert(addr != INADDR_ANY);
	if (unlikely((index = np_find_address(addr)) < 0))
		if (unlikely((index = np_add_address(addr)) < 0))
			return index;
	dassert(port > 1023 && port < 65536);
	dassert(np_addr_fetch(np_infos[index].map, port) == NULL);
	np_addr_store(np_infos[index].map, port, nc);
	np_port_set(port);
	return (index);
}

/** nc_del - delete a NC structure from the address/port maps
  * @addr: the 32-bit IPv4 address
  * @port: the 16-bit UDP port
  * @nc: the NC structure to remove
  *
  * Deletes the NC strcuture specified by @nc from the address and port mapping from use by tbe
  * lookup function.  This utility must be called with the address/port mapping lock held for write.
  * Note that the port bit-map does not have the bit cleared when there are mappings for other
  * addresses for that port that remain after the deletion.
  */
static fastcall void
nc_del(uint32_t addr, uint16_t port, struct nc *nc)
{
	int index;

	dassert(addr != INADDR_ANY);
	if (unlikely((index = np_find_address(addr)) < 0))
		return index;
	dassert(port > 1023 && port < 65536);
	dassert(np_addr_fetch(np_infos[index].map, port) == nc);
	np_addr_store(np_infos[index].map, port, NULL);
	for (index = 0; index < np_addr_num; index++)
		if (np_addr_fetch(np_infos[index].map, port) != NULL)
			return;
	np_port_clr(port);
	return;
}

/*
 * -----------------------------------------------------------------------------------------------
 * -----------------------------------------------------------------------------------------------
 * -----------------------------------------------------------------------------------------------
 * -----------------------------------------------------------------------------------------------
 */

struct nc_bhash_bucket;
struct nc_chash_bucket;

struct nc_daddr {
	uint32_t addr;			/* IP address this destination */
	uint32_t saddr;			/* current source address */
	unsigned char ttl;		/* time to live, this destination */
	unsigned char tos;		/* type of service, this destination */
	unsigned short mtu;		/* maximum transfer unit this destination */
	struct dst_entry *dst;		/* route for this destination */
	int oif;			/* current interface */
};

struct nc_saddr {
	uint32_t addr;			/* IP address this source */
};

struct nc_baddr {
	uint32_t addr;			/* IP address this bind */
};

struct np;

/* Network connection structure */
typedef struct nc {
	struct nc *nnext;		/* linkage for private structure */
	struct nc **nprev;		/* linkage for private structure */
	struct np *np;			/* linkage for private structure */
	struct nc *bnext;		/* linkage for bind/list hash */
	struct nc **bprev;		/* linkage for bind/list hash */
	struct nc_bhash_bucket *bhash;	/* linkage for bind/list hash */
	struct nc *cnext;		/* linkage for conn hash */
	struct nc **cprev;		/* linkage for conn hash */
	struct nc_chash_bucket *chash;	/* linkage for conn hash */
	np_ulong SERV_type;		/* service type */
	np_ulong CURRENT_state;		/* state */
	np_ulong BIND_flags;		/* bind flags */
	np_ulong CONN_flags;		/* connection flags */
	np_ulong CONIND_number;		/* maximum outstanding connection indications */
	bufq_t conq;			/* connection indication queue */
	bufq_t datq;			/* data indication queue */
	bufq_t resq;			/* reset indication queue */
	unsigned short pnum;		/* number of boudn protocol ids */
	uint8_t protoids[16];		/* bound protocol ids */
	unsigned short bnum;		/* number of bound addresses */
	unsigned short bport;		/* bound port number (network order) */
	struct nc_baddr baddrs[8];	/* bound addresses */
	unsigned short snum;		/* number of source (connected) addresses */
	unsigned short sport;		/* source (connected) port number (network order) */
	struct nc_saddr saddrs[8];	/* source (connected) addresses */
	unsigned short dnum;		/* number of destination (connected) addresses */
	unsigned short dport;		/* destination (connected) port number (network order) */
	struct nc_daddr daddrs[8];	/* destination (connected) addresses */
} nc_t;

/* Private structure */
struct np {
	STR_DECLARATION (struct np);	/* Stream declaration */
	queue_t *oq;			/* Output (read) queue */
	queue_t *iq;			/* Input (write) queue */
	struct nc *nc;			/* network connections */
	struct nc base;			/* network connection for real interface */
	N_info_ack_t info;		/* service provider information */
	struct N_qos_sel_info_udp qos;	/* network service provider quality of service */
	struct N_qos_range_info_udp qor;	/* network service provider quality of service range */
};

#define PRIV(__q) (((__q)->q_ptr))
#define NP_PRIV(__q) ((struct np *)((__q)->q_ptr))

typedef struct df {
	rwlock_t lock;			/* structure lock */
	SLIST_HEAD (np, np);		/* master list of np (open) structures */
} df_t;

static struct df master = {
#if	defined __RW_LOCK_UNLOCKED
	.lock = __RW_LOCK_UNLOCKED(master.lock),
#elif	defined RW_LOCK_UNLOCKED
	.lock = RW_LOCK_UNLOCKED,
#else
#error cannot initialize read-write locks
#endif
};

/*
 *  Bind buckets, caches and hashes.
 */
struct nc_bind_bucket {
	struct nc_bind_bucket *next;	/* linkage of bind buckets for hash slot */
	struct nc_bind_bucket **prev;	/* linkage of bind buckets for hash slot */
	unsigned char proto;		/* IP protocol identifier */
	unsigned short port;		/* port number (host order) */
	struct nc *owners;		/* list of owners of this protocol/port combination */
	struct nc *dflt;		/* default listeners/destinations for this protocol */
};
struct nc_conn_bucket {
	struct nc_conn_bucket *next;	/* linkage of conn buckets for hash slot */
	struct nc_conn_bucket **prev;	/* linkage of conn buckets for hash slot */
	unsigned char proto;		/* IP protocol identifier */
	unsigned short sport;		/* source port number (network order) */
	unsigned short dport;		/* destination port number (network order) */
	struct nc *owners;		/* list of owners of this protocol/sport/dport combination */
};

struct nc_bhash_bucket {
	rwlock_t lock;
	struct nc *list;
};
struct nc_chash_bucket {
	rwlock_t lock;
	struct nc *list;
};

STATIC struct nc_bhash_bucket *nc_bhash;
STATIC size_t nc_bhash_size = 0;
STATIC size_t nc_bhash_order = 0;

STATIC struct nc_chash_bucket *nc_chash;
STATIC size_t nc_chash_size = 0;
STATIC size_t nc_chash_order = 0;

STATIC INLINE fastcall __hot_in int
nc_bhashfn(unsigned char proto, unsigned short bport)
{
	return ((nc_bhash_size - 1) & (proto + bport));
}

STATIC INLINE fastcall __unlikely int
nc_chashfn(unsigned char proto, unsigned short sport, unsigned short dport)
{
	return ((nc_chash_size - 1) & (proto + sport + dport));
}

#if defined HAVE_KTYPE_STRUCT_NET_PROTOCOL
#define mynet_protocol net_protocol
#endif				/* defined HAVE_KTYPE_STRUCT_NET_PROTOCOL */
#if defined HAVE_KTYPE_STRUCT_INET_PROTOCOL
#define mynet_protocol inet_protocol
#endif				/* defined HAVE_KTYPE_STRUCT_INET_PROTOCOL */

struct ipnet_protocol {
	struct mynet_protocol proto;
	struct mynet_protocol *next;
	struct module *kmod;
};

struct nc_prot_bucket {
	unsigned char proto;		/* protocol number */
	int refs;			/* reference count */
	int corefs;			/* N_CONS references */
	int clrefs;			/* N_CLNS references */
	struct ipnet_protocol prot;	/* Linux registration structure */
};

#if	defined DEFINE_RWLOCK
STATIC DEFINE_RWLOCK(nc_prot_lock);
#elif	defined __RW_LOCK_UNLOCKED
STATIC rwlock_t nc_prot_lock = __RW_LOCK_UNLOCKED(nc_prot_lock);
#elif	defined RW_LOCK_UNLOCKED
STATIC rwlock_t nc_prot_lock = RW_LOCK_UNLOCKED;
#else
#error cannot initialize read-write locks
#endif
STATIC struct nc_prot_bucket *nc_prots[256];

STATIC kmem_cachep_t np_prot_cachep;	/* protocol structure memory cache */
STATIC kmem_cachep_t np_priv_cachep;	/* private structure memory cache */
STATIC kmem_cachep_t np_conn_cachep;	/* network connection memory cache */

static INLINE struct np *
np_get(struct np *np)
{
	dassert(np != NULL);
	atomic_inc(&np->refcnt);
	return (np);
}

static INLINE __hot void
np_put(struct np *np)
{
	dassert(np != NULL);
	if (atomic_dec_and_test(&np->refcnt)) {
		kmem_cache_free(np_priv_cachep, np);
	}
}

static INLINE fastcall __hot void
np_release(struct np **npp)
{
	struct np *np;

	dassert(npp != NULL);
	if (likely((np = XCHG(npp, NULL)) != NULL))
		np_put(np);
}

static INLINE struct np *
np_alloc(void)
{
	struct np *np;

	if ((np = kmem_cache_alloc(np_priv_cachep, GFP_ATOMIC))) {
		bzero(np, sizeof(*np));
		atomic_set(&np->refcnt, 1);
		spin_lock_init(&np->lock);	/* "np-lock" */
		np->priv_put = &np_put;
		np->priv_get = &np_get;
		// np->type = 0;
		// np->id = 0;
		// np->state = 0;
		// np->flags = 0;
	}
	return (np);
}

STATIC struct nc *
nc_alloc(struct np *np, void *tag, np_ulong SERV_type, np_ulong CONIND_number, np_ulong BIND_flags,
	 caddr_t ADDR_buffer, size_t ADDR_length, caddr_t PROTOID_buffer, size_t PROTOID_length)
{
	struct nc *nc;

	if (likely((nc = kmem_cache_alloc(np_conn_cachep, GFP_ATOMIC)) != NULL)) {
		struct sockaddr_in *add = (struct sockaddr_in *) ADDR_buffer;
		uint8_t *pro = (uint8_t *) PROTOID_buffer;
		int i;

		bzero(nc, sizeof(*nc));
		if ((nc->nnext = np->nc))
			nc->nnext->nprev = &nc->nnext;
		nc->nprev = &np->nc;
		nc->np = np;
		np->nc = nc;
		nc->bnext = NULL;
		nc->bprev = &nc->bnext;
		nc->bhash = NULL;
		nc->cnext = NULL;
		nc->cprev = &nc->cnext;
		nc->chash = NULL;
		nc->sndmblk = 0;
		nc->SERV_type = SERV_type;
		nc->CURRENT_state = NS_UNBND;
		nc->BIND_flags = BIND_flags;
		nc->CONN_flags = 0;
		nc->CONIND_number = CONIND_number;
		bufq_init(&nc->conq);
		bufq_init(&nc->datq);
		bufq_init(&nc->resq);
		if (PROTOID_length > sizeof(nc->protoids))
			PROTOID_length = sizeof(nc->protoids);
		nc->pnum = PROTOID_length;
		bcopy(PROTOID_buffer, nc->protoids, PROTOID_length);
		nc->bnum = ADDR_length / sizeof(add);
		if (nc->bnum > 8)
			nc->bnum = 8;
		nc->bport = add[0].sin_port;
		for (i = 0; i < nc->bnum; i++)
			nc->baddr.addr = add[i].sin_addr.s_addr;
		nc->snum = 0;
		nc->sport = 0;
		nc->dnum = 0;
		nc->dport = 0;
	}
	return (nc);
}

/*
 *  Locking
 */

/* Must always be bottom-half versions to avoid lock badness.  But give these
 * different names to avoid conflict with generic definitions.  */

//#if defined CONFIG_STREAMS_NOIRQ || defined _TEST
#if 1

#define spin_lock_str2(__lkp, __flags) \
	do { (void)__flags; spin_lock_bh(__lkp); } while (0)
#define spin_unlock_str2(__lkp, __flags) \
	do { (void)__flags; spin_unlock_bh(__lkp); } while (0)
#define write_lock_str2(__lkp, __flags) \
	do { (void)__flags; write_lock_bh(__lkp); } while (0)
#define write_unlock_str2(__lkp, __flags) \
	do { (void)__flags; write_unlock_bh(__lkp); } while (0)
#define read_lock_str2(__lkp, __flags) \
	do { (void)__flags; read_lock_bh(__lkp); } while (0)
#define read_unlock_str2(__lkp, __flags) \
	do { (void)__flags; read_unlock_bh(__lkp); } while (0)
#define local_save_str2(__flags) \
	do { (void)__flags; local_bh_disable(); } while (0)
#define local_restore_str2(__flags) \
	do { (void)__flags; local_bh_enable(); } while (0)

#else

#define spin_lock_str2(__lkp, __flags) \
	spin_lock_irqsave(__lkp, __flags)
#define spin_unlock_str2(__lkp, __flags) \
	spin_unlock_irqrestore(__lkp, __flags)
#define write_lock_str2(__lkp, __flags) \
	write_lock_irqsave(__lkp, __flags)
#define write_unlock_str2(__lkp, __flags) \
	write_unlock_irqrestore(__lkp, __flags)
#define read_lock_str2(__lkp, __flags) \
	read_lock_irqsave(__lkp, __flags)
#define read_unlock_str2(__lkp, __flags) \
	read_unlock_irqrestore(__lkp, __flags)
#define local_save_str2(__flags) \
	local_irq_save(__flags)
#define local_restore_str2(__flags) \
	local_irq_restore(__flags)

#endif

/*
 *  =========================================================================
 *
 *  State Changes
 *
 *  =========================================================================
 */

/* State flags */
#ifndef NSF_UNBND
#define NSF_UNBND	(1 << NS_UNBND		)
#define NSF_WACK_BREQ	(1 << NS_WACK_BREQ	)
#define NSF_WACK_UREQ	(1 << NS_WACK_UREQ	)
#define NSF_IDLE	(1 << NS_IDLE		)
#ifdef NS_WACK_OPTREQ
#define NSF_WACK_OPTREQ	(1 << NS_WACK_OPTREQ	)
#endif
#define NSF_WACK_RRES	(1 << NS_WACK_RRES	)
#define NSF_WCON_CREQ	(1 << NS_WCON_CREQ	)
#define NSF_WRES_CIND	(1 << NS_WRES_CIND	)
#define NSF_WACK_CRES	(1 << NS_WACK_CRES	)
#define NSF_DATA_XFER	(1 << NS_DATA_XFER	)
#define NSF_WCON_RREQ	(1 << NS_WCON_RREQ	)
#define NSF_WRES_RIND	(1 << NS_WRES_RIND	)
#define NSF_WACK_DREQ6	(1 << NS_WACK_DREQ6	)
#define NSF_WACK_DREQ7	(1 << NS_WACK_DREQ7	)
#define NSF_WACK_DREQ9	(1 << NS_WACK_DREQ9	)
#define NSF_WACK_DREQ10	(1 << NS_WACK_DREQ10	)
#define NSF_WACK_DREQ11	(1 << NS_WACK_DREQ11	)
#define NSF_NOSTATES	(1 << NS_NOSTATES	)
#endif

/* State masks */
#define NSM_ALLSTATES	(NSF_NOSTATES - 1)
#define NSM_WACK_DREQ	(NSF_WACK_DREQ6 \
			|NSF_WACK_DREQ7 \
			|NSF_WACK_DREQ9 \
			|NSF_WACK_DREQ10 \
			|NSF_WACK_DREQ11)
#define NSM_LISTEN	(NSF_IDLE \
			|NSF_WRES_CIND)
#define NSM_CONNECTED	(NSF_WCON_CREQ\
			|NSF_WRES_CIND\
			|NSF_DATA_XFER\
			|NSF_WCON_RREQ\
			|NSF_WRES_RIND)
#define NSM_DISCONN	(NSF_IDLE\
			|NSF_UNBND)
#define NSM_INDATA	(NSF_DATA_XFER\
			|NSF_WCON_RREQ)
#define NSM_OUTDATA	(NSF_DATA_XFER\
			|NSF_WRES_RIND)

#ifndef N_PROVIDER
#define N_PROVIDER  0
#define N_USER	    1
#endif

#ifdef _DEBUG
STATIC const char *
np_state_name(np_ulong state)
{
	switch (state) {
	case NS_UNBND:
		return ("NS_UNBND");
	case NS_WACK_BREQ:
		return ("NS_WACK_BREQ");
	case NS_WACK_UREQ:
		return ("NS_WACK_UREQ");
	case NS_IDLE:
		return ("NS_IDLE");
	case NS_WACK_OPTREQ:
		return ("NS_WACK_OPTREQ");
	case NS_WACK_RRES:
		return ("NS_WACK_RRES");
	case NS_WCON_CREQ:
		return ("NS_WCON_CREQ");
	case NS_WRES_CIND:
		return ("NS_WRES_CIND");
	case NS_WACK_CRES:
		return ("NS_WACK_CRES");
	case NS_DATA_XFER:
		return ("NS_DATA_XFER");
	case NS_WCON_RREQ:
		return ("NS_WCON_RREQ");
	case NS_WRES_RIND:
		return ("NS_WRES_RIND");
	case NS_WACK_DREQ6:
		return ("NS_WACK_DREQ6");
	case NS_WACK_DREQ7:
		return ("NS_WACK_DREQ7");
	case NS_WACK_DREQ9:
		return ("NS_WACK_DREQ9");
	case NS_WACK_DREQ10:
		return ("NS_WACK_DREQ10");
	case NS_WACK_DREQ11:
		return ("NS_WACK_DREQ11");
	case NS_NOSTATES:
		return ("NS_NOSTATES");
	default:
		return ("(unknown)");
	}
}
#endif				/* _DEBUG */

/* State functions */

STATIC INLINE fastcall np_ulong
nc_get_state(const struct nc *nc)
{
	return (nc_get_CURRENT_state(nc));
}

STATIC INLINE fastcall np_ulong
np_get_state(const struct np *np)
{
	return nc_get_state(&np->base);
}

STATIC INLINE fastcall void
nc_set_state(queue_t *q, struct nc *nc, const np_ulong state)
{
	mi_strlog(q, STRLOGST, SL_TRACE, "%s <- %s (%p)", np_state_name(state),
		  np_state_name(nc_get_state(nc)), (void *) nc);
	nc_set_CURRENT_state(nc, state);
}

STATIC INLINE fastcall void
np_set_state(struct np *np, const np_ulong state)
{
	nc_set_state(np->oq, &np->base, state);
}

STATIC INLINE fastcall np_ulong
nc_chk_state(const struct nc *nc, const np_ulong mask)
{
	return (((1 << nc_get_state(nc)) & (mask)) != 0);
}

STATIC INLINE fastcall np_ulong
np_chk_state(const struct np *np, const np_ulong mask)
{
	return nc_chk_state(&np->base, mask);
}

STATIC INLINE fastcall np_ulong
nc_not_state(const struct nc *nc, const np_ulong mask)
{
	return (((1 << nc_get_state(nc)) & (mask)) == 0);
}

STATIC INLINE fastcall np_ulong
np_not_state(const struct np *np, const np_ulong mask)
{
	return nc_not_state(&np->base, mask);
}

STATIC INLINE fastcall long
nc_get_statef(const struct nc *nc)
{
	return (1 << nc_get_state(nc));
}

STATIC INLINE fastcall long
np_get_statef(const struct np *np)
{
	return nc_get_state(&np->base);
}

/*
 *  =========================================================================
 *
 *  Quality of Service structures
 *
 *  =========================================================================
 */

union N_qos_types {
	np_ulong n_qos_type;
	union N_qos_ip_types ip;
	union N_qos_udp_types udp;
	union N_qos_rtp_types rtp;
};

/*
 *  =========================================================================
 *
 *  IP Local Management
 *
 *  =========================================================================
 */

STATIC int np_v4_rcv(struct sk_buff *skb);
STATIC void np_v4_err(struct sk_buff *skb, u32 info);

/*
 *  IP subsystem management
 */
#ifdef LINUX
/** np_v4_rcv_next - pass a socket buffer to the next handler
  * @skb: socket buffer to pass
  *
  * In the Linux packet handler, if the packet is not for us, pass it to the next handler.  If there
  * is no next handler, free the packet and return.  Note that we do not have to lock the hash
  * because we own it and are also holding a reference to any module owning the next handler.  This
  * function returns zero (0) if the packet has not or will not be seen by another packet handler,
  * and one (1) if the packet has or will be seen by another packet handler.  This return value is
  * used to determine whether to generate ICMP errors or not.
  *
  * Because our RTP port number assignments can easily conflict with underlying UDP assignements
  * (why does not RTP have its own protocol number?), there are several criteria that must be met to
  * determine that the packet is for us:
  *
  *	1. the packet is received on an even port number and contains a valid RTP header.
  *	2. the packet is received on an odd port number and contains a valid RTCP header.
  *	3. the destination address matches a bound address (if specified in the network connection).
  *	4. the source address matches a connected address (if specified in the network connection).
  *
  * When the criteria are not met, the packet is passed to the next handler.  Note that a connected
  * RTP/RTCP channel can coexist under this scheme with a normal Linux UDP socket for some other
  * protocol even using the same port numbers.  Also note that the first criteria are quick to check
  * as the socket buffer and payload should be hot coming into the receive function.
  */
STATIC INLINE fastcall __hot_in int
np_v4_rcv_next(struct sk_buff *skb)
{
	struct nc_prot_bucket *pb;
	struct mynet_protocol *pp;
	struct iphdr *iph;
	unsigned char proto;

	iph = (typeof(iph)) skb_network_header(skb);
	proto = iph->protocol;
	if ((pb = nc_prots[proto]) && (pp = pb->prot.next)) {
		pp->handler(skb);
		return (1);
	}
	kfree_skb(skb);
	return (0);
}

/**
 * np_v4_err_next - pass a socket buffer to the next error handler
 * @skb: socket buffer to pass
 *
 * In the Linux packet error handler, if the packet is not for us, pass it to the next error
 * handler.  If there is no next error handler, simply return.
 */
STATIC INLINE fastcall __hot_in void
np_v4_err_next(struct sk_buff *skb, __u32 info)
{
	struct nc_prot_bucket *pb;
	struct mynet_protocol *pp;
	unsigned char proto;

	proto = ((struct iphdr *) skb->data)->protocol;
	if ((pb = nc_prots[proto]) && (pp = pb->prot.next))
		pp->err_handler(skb, info);
	return;
}

/**
 * np_init_nproto - initialize network protocol override
 * @proto: the protocol to register or override
 *
 * This is the network protocol override function.
 *
 * This is complicated because we hack the inet protocol tables.  If no other protocol was
 * previously registered, this reduces to inet_add_protocol().  If there is a protocol previously
 * registered (which we expect for UDP), we take a reference on the kernel module owning the entry
 * (which we expect to be none because UDP is built into the kernel), if possible, and replace the
 * entry with our own, saving a pointer to the previous entry for passing sk_bufs along that we are
 * not interested in.  Taking a module reference is particularly for things like SCTP, where
 * unloading the module after protocol override would break things horribly.  Taking the reference
 * keeps the module from unloading (this works for OpenSS7 SCTP as well as lksctp).
 */
STATIC INLINE fastcall __unlikely struct nc_prot_bucket *
np_init_nproto(unsigned char proto, unsigned int type)
{
	struct nc_prot_bucket *pb;
	struct ipnet_protocol *pp;
	struct mynet_protocol **ppp;
	int hash = proto & (MAX_INET_PROTOS - 1);

	write_lock_bh(&nc_prot_lock);
	if ((pb = nc_prots[proto]) != NULL) {
		pb->refs++;
		switch (type) {
		case N_CONS:
			++pb->corefs;
			break;
		case N_CLNS:
			++pb->clrefs;
			break;
		default:
			swerr();
			break;
		}
	} else if ((pb = kmem_cache_alloc(np_prot_cachep, GFP_ATOMIC))) {
		bzero(pb, sizeof(*pb));
		pb->refs = 1;
		switch (type) {
		case N_CONS:
			pb->corefs = 1;
			break;
		case N_CLNS:
			pb->clrefs = 1;
			break;
		default:
			swerr();
			break;
		}
		pp = &pb->prot;
#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_PROTOCOL
		pp->proto.protocol = proto;
		pp->proto.name = "streams-np_rtp";
#endif
#if defined HAVE_KTYPE_STRUCT_NET_PROTOCOL_PROTO
		pp->proto.proto = proto;
#endif				/* defined HAVE_KTYPE_STRUCT_NET_PROTOCOL_PROTO */
#if defined HAVE_KMEMB_STRUCT_NET_PROTOCOL_NO_POLICY || defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_NO_POLICY
		pp->proto.no_policy = 1;
#endif
		pp->proto.handler = &np_v4_rcv;
		pp->proto.err_handler = &np_v4_err;
		ppp = &inet_protosp[hash];

		{
			net_protocol_lock();
#ifdef HAVE_OLD_STYLE_INET_PROTOCOL
			while (*ppp && (*ppp)->protocol != proto)
				ppp = &(*ppp)->next;
#endif				/* HAVE_OLD_STYLE_INET_PROTOCOL */
			if (*ppp != NULL) {
#ifdef HAVE_KMEMB_STRUCT_INET_PROTOCOL_COPY
				/* can only override last entry */
				if ((*ppp)->copy != 0) {
					__ptrace(("Cannot override copy entry\n"));
					net_protocol_unlock();
					write_unlock_bh(&nc_prot_lock);
					kmem_cache_free(np_prot_cachep, pb);
					return (NULL);
				}
#endif				/* HAVE_KMEMB_STRUCT_INET_PROTOCOL_COPY */
				if ((pp->kmod = streams_module_address((ulong) *ppp))
				    && pp->kmod != THIS_MODULE) {
					if (!try_module_get(pp->kmod)) {
						__ptrace(("Cannot acquire module\n"));
						net_protocol_unlock();
						write_unlock_bh(&nc_prot_lock);
						kmem_cache_free(np_prot_cachep, pb);
						return (NULL);
					}
				}
#if defined HAVE_KMEMB_STRUCT_NET_PROTOCOL_NEXT || defined HAVE_KMEMB_STRUCT_INET_PROTOCOL_NEXT
				pp->proto.next = (*ppp)->next;
#endif
			}
			pp->next = xchg(ppp, &pp->proto);
			net_protocol_unlock();
		}
		/* link into hash slot */
		nc_prots[proto] = pb;
	}
	write_unlock_bh(&nc_prot_lock);
	return (pb);
}

/** np_term_nproto - terminate network protocol override
  * @proto: network protocol to terminate
  *
  * This is the network protocol restoration function.
  *
  * This is complicated and brittle.  The module stuff here is just for ourselves (other kernel
  * modules pulling the same trick) as Linux IP protocols are normally kernel resident.  If a
  * protocol was previously registered, restore the protocol's entry and drop the reference to its
  * owning kernel module.  If there was no protocol previously registered, this reduces to
  * inet_del_protocol().
  */
STATIC INLINE fastcall __unlikely void
np_term_nproto(unsigned char proto, unsigned int type)
{
	struct nc_prot_bucket *pb;

	write_lock_bh(&nc_prot_lock);
	if ((pb = nc_prots[proto]) != NULL) {
		switch (type) {
		case N_CONS:
			assure(pb->corefs > 0);
			--pb->corefs;
			break;
		case N_CLNS:
			assure(pb->clrefs > 0);
			--pb->clrefs;
			break;
		default:
			swerr();
			break;
		}
		if (--pb->refs == 0) {
			struct ipnet_protocol *pp = &pb->prot;
			struct mynet_protocol **ppp;
			int hash = proto & (MAX_INET_PROTOS - 1);

			ppp = &inet_protosp[hash];
			{
				net_protocol_lock();
#ifdef HAVE_OLD_STYLE_INET_PROTOCOL
				while (*ppp && *ppp != &pp->proto)
					ppp = &(*ppp)->next;
				if (pp->next)
					pp->next->next = pp->proto.next;
#endif				/* HAVE_OLD_STYLE_INET_PROTOCOL */
				__assert(*ppp == &pp->proto);
				*ppp = pp->next;
				net_protocol_unlock();
			}
			if (pp->next != NULL && pp->kmod != NULL && pp->kmod != THIS_MODULE)
				module_put(pp->kmod);
			/* unlink from hash slot */
			nc_prots[proto] = NULL;

			kmem_cache_free(np_prot_cachep, pb);
		}
	}
	write_unlock_bh(&nc_prot_lock);
}
#endif				/* LINUX */

/**
 * np_bind_prot -  bind a protocol
 * @proto:	    protocol number to bind
 *
 * NOTICES: Notes about registration.  Older 2.4 kernels will allow you to register whatever inet
 * protocols you want on top of any existing protocol.  This is good.  2.6 kernels, on the other
 * hand, do not allow registration of inet protocols over existing inet protocols.  We rip symbols
 * on 2.6 and put special code in the handler to give us effectively the old 2.4 approach.
 * This is also detectable by the fact that inet_add_protocol() returns void on 2.4 and int on 2.6.
 *
 * Issues with the 2.4 approach to registration is that the ip_input function passes a cloned skb
 * to each protocol registered.  We don't want to do that.  If the message is for us, we want to
 * process it without passing it to others.
 *
 * Issues with the 2.6 approach to registration is that the ip_input function passes the skb to
 * only one function.  We don't want that either.  If the message is not for us, we want to pass it
 * to the next protocol module.
 */
STATIC INLINE fastcall __unlikely int
np_bind_prot(unsigned char proto, unsigned int type)
{
	struct nc_prot_bucket *pb;

	if ((pb = np_init_nproto(proto, type)))
		return (0);
	return (-ENOMEM);
}

/**
 * np_unbind_prot - unbind a protocol
 * @proto:	    protocol number to unbind
 */
STATIC INLINE fastcall __unlikely void
np_unbind_prot(unsigned char proto, unsigned int type)
{
	np_term_nproto(proto, type);
}

/**
 * np_bind - bind a Stream to an NSAP
 * @np: private structure
 * @PROTOID_buffer: protocol ids to bind
 * @PROTOID_length; length of protocol ids
 * @ADDR_buffer: addresses to bind
 * @ADDR_length: length of addresses
 * @CONIND_number: maximum number of connection indications
 * @BIND_flags: bind flags
 *
 * Bind to protocol ids and port and addresses.  We currently only bind the first protocol id
 * regardless of the number specified.  Binding supports wildcard addresses, both in port number and
 * IP address.  The bind hash contains bind buckets that list Streams that are bound to the same
 * protocol id and port number.
 *
 * Assign a port number and bind to it, or bind to the selected port.
 *
 * Linux RTP uses a specific port range for dynamic assignment of port numbers.  We take the same
 * approach here, however, we need to use a range that does not overlap with the Linux RTP range.
 *
 * TODO: also check Linux RTP bind hashes.  If the port is specifically in the Linux RTP dynamic
 * port assignment range, we should just refuse it.  This is because Linux RTP believes that it owns
 * this range and will assign within this range without the user having any control.  If outside
 * this range, it might be an idea to check the Linux RTP hashes to make sure that noone else is
 * using this port at the moment.  It does not keep Linux RTP from assigning it later, resulting in
 * a conflict.
 *
 */
STATIC INLINE fastcall __unlikely int
np_bind(struct np *np, unsigned char *PROTOID_buffer, size_t PROTOID_length, struct sockaddr_in *ADDR_buffer,
	const socklen_t ADDR_length, const np_ulong CONIND_number, const np_ulong BIND_flags)
{
	static unsigned short np_prev_port = 61000;
	static const unsigned short np_frst_port = 61000;
	static const unsigned short np_last_port = 65000;

	struct nc_bhash_bucket *hp;
	unsigned short bport = ADDR_buffer[0].sin_port;
	unsigned char proto = PROTOID_buffer[0];
	size_t anum = ADDR_length / sizeof(*ADDR_buffer);
	struct np *np2;
	int i, j, err;
	unsigned short num = 0;
	unsigned long flags;

	PROTOID_length = 1;
	if (bport == 0) {
		num = np_prev_port;	/* UNSAFE */
	      try_again:
		bport = htons(num);
	}
	hp = &nc_bhash[nc_bhashfn(proto, bport)];
	write_lock_str2(&hp->lock, flags);
	for (np2 = hp->list; np2; np2 = np2->bnext) {
		if (proto != np2->protoids[0])
			continue;
		if (bport != np2->bport)
			continue;
		/* Allowed to bind to each NSAP once as DEFAULT_DEST, once as DEFAULT_LISTENER and once as
		   neither. */
		if ((BIND_flags & (DEFAULT_DEST | DEFAULT_LISTENER)) !=
		    (np2->BIND_flags & (DEFAULT_DEST | DEFAULT_LISTENER)))
			continue;
		for (i = 0; i < np2->bnum; i++) {
			for (j = 0; j < anum; j++)
				if (np2->baddrs[i].addr == ADDR_buffer[j].sin_addr.s_addr)
					break;
			if (j < anum)
				break;
		}
		if (i < np2->bnum)
			break;
	}
	if (np2 != NULL) {
		if (num == 0) {
			/* specific port number requested */
			write_unlock_str2(&hp->lock, flags);
			/* There is a question as to which error should be returned when a protocol address
			   is already bound.

			   NPI 2.0.0 says that "[i]f the NS provider cannot bind the specified address, it
			   may assign another network address to the user.  It is the network user's
			   responsibility to check the network address returned in the N_BIND_ACK primitive
			   to see if it is the same as the one requested."

			   NPI 2.0.0 says "[o]nly one default listener Stream is allowed per occurrence of
			   NPI.  An attempt to bind a default listener Stream when one is already bound
			   should result in an error (of type NBOUND)" and "[o]nly one default destination
			   stream per NSAP is alloed per occurence of NPI. An attempt to bind a default
			   destination stream to an NSAP when one is already bound should result in an error
			   of type NBOUND." But aslo, "NBOUND: The NS user attempted to bind a second Stream
			   to a network address with the CONIND_number set to a non-zero value, or attempted
			   to bind a second Stream with the DEFAULT_LISTENER flag value set to non-zero."

			   However, we return NBOUND for a connectionless bind when an attempt is made to
			   bind a second address to the same NSAP where either both have the DEFAULT_DEST
			   flag set or both have the DEFAULT_DEST flag clear. */
			return (NBOUND);
		} else {
			write_unlock_str2(&hp->lock, flags);
			if (++num > np_last_port)
				num = np_frst_port;
			if (num != np_prev_port)
				goto try_again;
			return (NNOADDR);
		}
	}
	if ((err = np_bind_prot(proto, np->info.SERV_type))) {
		write_unlock_str2(&hp->lock, flags);
		return (err);
	}
	if (num != 0)
		np_prev_port = num;
	ADDR_buffer[0].sin_port = bport;
	if ((np_long) np->qos.protocol == QOS_UNKNOWN)
		np->qos.protocol = proto;
	if ((np->bnext = hp->list))
		np->bnext->bprev = &np->bnext;
	np->bprev = &hp->list;
	hp->list = np_get(np);
	np->bhash = hp;
	/* copy into private structure */
	np->CONIND_number = CONIND_number;
	np->BIND_flags = BIND_flags;
	np->pnum = PROTOID_length;
	for (i = 0; i < PROTOID_length; i++)
		np->protoids[i] = PROTOID_buffer[i];
	np->bnum = anum;
	np->bport = bport;
	ptrace(("%s: %s: bound to proto = %d, bport = %d\n", DRV_NAME, __FUNCTION__, (int) proto,
		(int) ntohs(bport)));
	for (i = 0; i < anum; i++)
		np->baddrs[i].addr = ADDR_buffer[i].sin_addr.s_addr;
	write_unlock_str2(&hp->lock, flags);
#if defined HAVE_KFUNC_SYNCHRONIZE_NET
#if defined HAVE_KFUNC_IN_ATOMIC || defined in_atomic
	if (!in_interrupt() && !in_atomic())
#else
	if (!in_interrupt())
#endif
		synchronize_net();	/* might sleep */
#endif				/* defined HAVE_KFUNC_SYNCHRONIZE_NET */
	return (0);
}

#if defined HAVE_KFUNC_DST_OUTPUT
STATIC INLINE __hot_out int
np_rtp_queue_xmit(struct sk_buff *skb)
{
	struct dst_entry *dst = skb_dst(skb);
	struct iphdr *iph = (typeof(iph)) skb_network_header(skb);

#if defined NETIF_F_TSO
#if defined HAVE_KFUNC_IP_SELECT_IDENT_MORE_SK_BUFF
	ip_select_ident_more(skb, dst, NULL, 0);
#else				/* !defined HAVE_KFUNC_IP_SELECT_IDENT_MORE_SK_BUFF */
	ip_select_ident_more(iph, dst, NULL, 0);
#endif				/* defined HAVE_KFUNC_IP_SELECT_IDENT_MORE_SK_BUFF */
#else				/* !defined NETIF_F_TSO */
	ip_select_ident(iph, dst, NULL);
#endif				/* defined NETIF_F_TSO */
	ip_send_check(iph);
#ifndef NF_IP_LOCAL_OUT
#define NF_IP_LOCAL_OUT NF_INET_LOCAL_OUT
#endif
#if defined HAVE_KFUNC_IP_DST_OUTPUT
	return NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dst->dev, ip_dst_output);
#else				/* !defined HAVE_KFUNC_IP_DST_OUTPUT */
	return NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dst->dev, dst_output);
#endif				/* defined HAVE_KFUNC_IP_DST_OUTPUT */
}
#else				/* !defined HAVE_KFUNC_DST_OUTPUT */
STATIC INLINE __hot_out int
np_rtp_queue_xmit(struct sk_buff *skb)
{
	struct dst_entry *dst = skb_dst(skb);
	struct iphdr *iph = skb->nh.iph;

	if (skb->len > dst_pmtu(dst)) {
		rare();
		return ip_fragment(skb, dst->output);
	} else {
		iph->frag_off |= __constant_htons(IP_DF);
		ip_send_check(iph);
		return dst->output(skb);
	}
}
#endif				/* defined HAVE_KFUNC_DST_OUTPUT */

/**
 * np_alloc_skb_slow - allocate a socket buffer from a message block
 * @np: private pointer
 * @mp: the message block
 * @headroom: header room for resulting sk_buff
 * @gfp: general fault protection
 *
 * This is the old slow way of allocating a socket buffer.  We simple allocate a socket buffer with
 * sufficient head room and copy the data from the message block(s) to the socket buffer.  This is
 * slow.  This is the only way that LiS can do things (because it has unworkable message block
 * allocation).
 */
noinline fastcall __unlikely struct sk_buff *
np_alloc_skb_slow(struct np *np, mblk_t *mp, unsigned int headroom, int gfp)
{
	struct sk_buff *skb;
	unsigned int dlen = msgsize(mp);

	if (likely((skb = alloc_skb(headroom + dlen, GFP_ATOMIC)) != NULL)) {
		skb_reserve(skb, headroom);
		{
			unsigned char *data;
			mblk_t *b;
			int blen;

			data = skb_put(skb, dlen);
			for (b = mp; b; b = b->b_cont) {
				if ((blen = b->b_wptr - b->b_rptr) > 0) {
					bcopy(b->b_rptr, data, blen);
					data += blen;
					__assert(data <= skb_tail_pointer(skb));
				} else
					rare();
			}
		}
		freemsg(mp);	/* must absorb */
	}
	return (skb);
}

#ifdef HAVE_SKBUFF_HEAD_CACHE_USABLE
extern kmem_cachep_t skbuff_head_cache;

/**
 * np_alloc_skb_old - allocate a socket buffer from a message block
 * @np: private pointer
 * @mp: the message block
 * @headroom: header room for resulting sk_buff
 * @gfp: general fault protection
 *
 * Description: this function is used for zero-copy allocation of a socket buffer from a message
 * block.  The socket buffer contains all of the data in the message block including any head or
 * tail room (db_base to db_lim).  The data portion of the socket buffer contains the data
 * referenced by the message block (b_rptr to b_wptr).  Because there is no socket buffer destructor
 * capable of freeing the message block, we steal the kmem_alloc'ed buffer from the message and
 * attach it tot he socket buffer header.  The reference to the message block is consumed unless the
 * function returns NULL.
 *
 * A problem exists in converting mblks to sk_buffs (although visa versa is easy):  sk_buffs put a
 * hidden shared buffer structure at the end of the buffer (where it is easily overwritten on buffer
 * overflows).  There is not necessarily enough room at the end of the mblk to add this structure.
 * There are several things that I added to the Stream head to help with this:
 *
 * 1. A SO_WRPAD option to M_SETOPTS that will specify how much room to leave after the last SMP
 *    cache line in the buffer.
 *
 * 2. Three flags, SO_NOCSUM, SO_CSUM, SO_CRC32C were added to the Stream head so that the stream
 *    can support partial checksum while copying from the user.
 *
 * 3. db_lim is now always set to the end of the actual allocation rather than the end of the
 *    requested allocation.  Linux kmalloc() allocates from 2^n size memory caches that are
 *    always SMP cache line aligned.
 *
 * With these options in play, the size of the buffer should have sufficient room for the shared
 * buffer structure.  If, however, the data block was not delivered by the Stream head (but an
 * intermediate module) or has been modified (by an intermediate module) the tail room might not be
 * available.  Instead of copying the entire buffer which would be quite memory intensive, in this
 * case we allocate a new buffer and copy only the portion of the original buffer necessary to make
 * room for the shared buffer structure.
 *
 * The same is true for the IP header portion.  Using SO_WROFF it is possible to reserve sufficient
 * room for the hardware header, IP header and RTP header.  Message blocks should normally already
 * contain this headroom.  However, again, it might be possible that the message block originated at
 * an intermediate module or was modified by an intermediate module unaware of this policy.  If
 * there is insufficient headroom, again we allocate a new message block large enough to contain the
 * header and make two sk_buffs, one for the header and one for the payload.
 *
 * As a result, we might wind up with three socket buffers: one containing the headroom for the hard
 * header, IP header and RTP header; one containing most of the data payload; and one containing the
 * last fragment of the payload smaller than or equal to sizeof(struct skb_shared_info).  All but
 * the initial socket buffer are placed in the frag_list of the first socket buffer.  Note that only
 * the header need be completed.  If checksum has not yet been performed, it is necessary to walk
 * through the data to generate the checksum.
 */
noinline fastcall __unlikely struct sk_buff *
np_alloc_skb_old(struct np *np, mblk_t *mp, unsigned int headroom, int gfp)
{
	struct sk_buff *skb;
	unsigned char *beg, *end;

	/* must not be a fastbuf */
	if (unlikely(mp->b_datap->db_size <= FASTBUF))
		goto go_slow;
	/* must not be esballoc'ed */
	if (unlikely(mp->b_datap->db_frtnp != NULL))
		goto go_slow;
	/* must be only reference (for now) */
	if (unlikely(mp->b_datap->db_ref > 1))
		goto go_slow;

	beg = mp->b_rptr - headroom;
	/* First, check if there is enough head room in the data block. */
	if (unlikely(beg < mp->b_datap->db_base)) {
		goto go_frag;
	}
	/* Next, check if there is enough tail room in the data block. */
	end =
	    (unsigned char *) (((unsigned long) mp->b_wptr + (SMP_CACHE_BYTES - 1)) & ~(SMP_CACHE_BYTES - 1));
	if (unlikely(end + sizeof(struct skb_shared_info) > mp->b_datap->db_lim)) {
		goto go_frag;
	}

	/* Last, allocate a socket buffer header and point it to the payload data. */
	skb = kmem_cache_alloc(skbuff_head_cache, gfp);
	if (unlikely(skb == NULL))
		goto no_skb;

	memset(skb, 0, offsetof(struct sk_buff, truesize));

	skb->truesize = end - beg + sizeof(struct sk_buff);
	atomic_set(&skb->users, 1);
	skb->head = mp->b_datap->db_base;
	skb->data = mp->b_rptr;
	skb_set_tail_pointer(skb, mp->b_wptr - mp->b_rptr);
#if defined NET_SKBUFF_DATA_USES_OFFSET
	skb->end = end - skb->head;
#else				/* defined NET_SKBUFF_DATA_USES_OFFSET */
	skb->end = end;
#endif				/* defined NET_SKBUFF_DATA_USES_OFFSET */
	skb->len = mp->b_wptr - mp->b_rptr;
	skb->cloned = 0;
	skb->data_len = 0;
	/* initialize shared data structure */
	memset(skb_shinfo(skb), 0, sizeof(struct skb_shared_info));
	atomic_set(&(skb_shinfo(skb)->dataref), 1);

	/* need to release message block and data block without releasing buffer */

	/* point into internal buffer */
	mp->b_datap->db_frtnp = (struct free_rtn *)
	    ((struct mdbblock *) ((struct mbinfo *) mp->b_datap - 1))->databuf;
	/* override with dummy free routine */
	mp->b_datap->db_frtnp->free_func = NULL;	/* tells freeb not to call */
	mp->b_datap->db_frtnp->free_arg = NULL;
	freemsg(mp);

      no_skb:
	return (skb);
	return (NULL);
      go_frag:			/* for now */
      go_slow:
	return np_alloc_skb_slow(np, mp, headroom, gfp);
}
#endif

STATIC INLINE fastcall __hot_out struct sk_buff *
np_alloc_skb(struct np *np, mblk_t *mp, unsigned int headroom, int gfp)
{
	struct sk_buff *skb;

	if (unlikely((mp->b_datap->db_flag & (DB_SKBUFF)) == 0))
		goto old_way;
	if (unlikely((mp->b_rptr < mp->b_datap->db_base + headroom)))
		goto go_slow;
	if (unlikely((skb = (typeof(skb)) mp->b_datap->db_frtnp->free_arg) == NULL))
		goto go_slow;
	skb_get(skb);
	skb_reserve(skb, mp->b_rptr - skb->data);
	skb_put(skb, mp->b_wptr - mp->b_rptr);
	freemsg(mp);
	return (skb);
      old_way:
#ifdef HAVE_SKBUFF_HEAD_CACHE_USABLE
	return np_alloc_skb_old(np, mp, headroom, gfp);
#endif
      go_slow:
	return np_alloc_skb_slow(np, mp, headroom, gfp);
}

noinline fastcall int
np_route_output_slow(struct np *np, const uint32_t daddr, struct rtable **rtp)
{
	int err;

	if (XCHG(rtp, NULL) != NULL)
		dst_release(XCHG(&np->daddrs[0].dst, NULL));
#if defined HAVE_KMEMB_STRUCT_RTABLE_RT_SRC
	if (likely((err = ip_route_output(rtp, daddr, np->qos.saddr, 0, 0)) == 0)) {
		dst_hold(rt_dst(*rtp));
		np->daddrs[0].dst = rt_dst(*rtp);
		np->daddrs[0].addr = daddr;
		np->daddrs[0].saddr = (*rtp)->rt_src;
		np->daddrs[0].oif = (*rtp)->rt_oif;
	}
#else				/* defined HAVE_KMEMB_STRUCT_RTABLE_RT_SRC */
	{
		struct flowi4 fl4;
		struct rtable *rt;

		flowi4_init_output(&fl4, 0, 0, 0, RT_SCOPE_UNIVERSE, 0, 0, daddr, np->qos.saddr, 0, 0);
		rt = __ip_route_output_key(&init_net, &fl4);
		if (IS_ERR(rt))
			return PTR_ERR(rt);
		np->daddrs[0].dst = rt_dst(rt);
		np->daddrs[0].addr = fl4.daddr;
		np->daddrs[0].saddr = fl4.saddr;
		np->daddrs[0].oif = fl4.flowi4_oif;
		np->daddrs[0].tos = fl4.flowi4_tos;
		if (rtp)
			*rtp = rt;
		err = 0;
	}
#endif				/* defined HAVE_KMEMB_STRUCT_RTABLE_RT_SRC */
	return (err);
}

STATIC INLINE fastcall __hot_out int
np_route_output(struct np *np, const uint32_t daddr, struct rtable **rtp)
{
	register struct rtable *rt;

	if (likely((rt = *rtp) != NULL)) {
		dst_hold(rt_dst(rt));
		return (0);
	}
	return np_route_output_slow(np, daddr, rtp);
}

#ifdef NETIF_F_NO_CSUM
#define DONT_CHECKSUM (NETIF_F_NO_CSUM|NETIF_F_HW_CSUM|NETIF_F_IP_CSUM|NETIF_F_LOOPBACK)
#else
#define DONT_CHECKSUM (NETIF_F_HW_CSUM|NETIF_F_IP_CSUM|NETIF_F_LOOPBACK)
#endif


/** np_senddata - process a unit data request
  * @np: Stream private structure
  * @protocol: IP protocol number for packet
  * @dport: destination port
  * @daddr: destination address
  * @mp: message payload
  */
STATIC INLINE fastcall __hot_out int
np_senddata(struct np *np, uint8_t protocol, const unsigned short dport, uint32_t daddr, mblk_t *mp)
{
	struct rtable *rt;
	int err;

	rt = (struct rtable *) np->daddrs[0].dst;
	prefetch(rt);

	if (likely((err = np_route_output(np, daddr, &rt)) == 0)) {
		struct sk_buff *skb;
		struct net_device *dev = rt_dst(rt)->dev;
		size_t hlen = ((dev->hard_header_len + 15) & ~15)
		    + sizeof(struct iphdr) + sizeof(struct udphdr);
		size_t dlen = msgsize(mp);
		size_t plen = dlen + sizeof(struct udphdr);
		size_t tlen = plen + sizeof(struct iphdr);

		_ptrace(("%s: %s: sending data message block %p\n", DRV_NAME, __FUNCTION__, mp));
		usual(hlen > sizeof(struct iphdr) + sizeof(struct udphdr));
		usual(dlen);

		if (likely((skb = np_alloc_skb(np, mp, hlen, GFP_ATOMIC)) != NULL)) {
			struct iphdr *iph;
			struct udphdr *uh;
			uint32_t saddr;

#ifdef HAVE_KMEMB_STRUCT_RTABLE_RT_SRC
			saddr = np->qos.saddr ? : rt->rt_src;
#else
			saddr = np->qos.saddr ? : np->daddrs[0].saddr;
#endif
			skb->ip_summed = CHECKSUM_UNNECESSARY;
			skb->csum = 0;
			if (unlikely(np->qos.checksum == 0))
				goto no_csum;
			if (likely(dev->features & DONT_CHECKSUM))
				goto no_csum;
			skb->ip_summed = 0;
			skb->csum = csum_tcpudp_nofold(saddr, daddr, skb->len, IPPROTO_UDP, 0);
			skb->csum = csum_fold(skb_checksum(skb, 0, skb->len, skb->csum));

		      no_csum:
			/* find headers */
			__skb_push(skb, sizeof(struct udphdr));
			skb_reset_transport_header(skb);
			__skb_push(skb, sizeof(struct iphdr));
			skb_reset_network_header(skb);

			skb_dst_set(skb, rt_dst(rt));
			skb->priority = 0;	// np->qos.priority;

			iph = (typeof(iph)) skb_network_header(skb);
			iph->version = 4;
			iph->ihl = 5;
			iph->tos = np->qos.tos ? : np->daddrs[0].tos;
			iph->frag_off = htons(IP_DF);	/* never frag */
			// iph->frag_off = 0; /* need qos bit */
			iph->ttl = np->qos.ttl ? : np->daddrs[0].ttl;
			iph->daddr = daddr;
			iph->saddr = saddr;
			iph->protocol = protocol;
			iph->tot_len = htons(tlen);

			uh = (typeof(uh)) skb_transport_header(skb);
			uh->dest = dport;
			uh->source = np->sport ? : np->bport;
			uh->len = htons(plen);
			uh->check = 0;

#ifndef HAVE_KFUNC_DST_OUTPUT
#ifdef HAVE_KFUNC___IP_SELECT_IDENT_2_ARGS
			__ip_select_ident(iph, rt_dst(rt));
#elif defined HAVE_KFUNC___IP_SELECT_IDENT_3_ARGS
			__ip_select_ident(iph, rt_dst(rt), 0);
#else
#error HAVE_KFUNC___IP_SELECT_IDENT_2_ARGS or HAVE_KFUNC___IP_SELECT_IDENT_3_ARGS must be defined.
#endif
#endif
			_printd(("sending message %p\n", skb));
#ifdef HAVE_KFUNC_DST_OUTPUT
			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, np_rtp_queue_xmit);
#else
			np_rtp_queue_xmit(skb);
#endif
			return (QR_ABSORBED);
		}
		_rare();
		return (-ENOBUFS);
	}
	_rare();
	return (err);
}

STATIC INLINE fastcall int
np_datack(queue_t *q)
{
	/* not supported */
	return (-EOPNOTSUPP);
}

/**
 * np_conn_check - check and enter into connection hashes
 * @np: private structure
 * @proto: protocol to which to connect
 */
STATIC fastcall int
np_conn_check(struct np *np, unsigned char proto)
{
	unsigned short sport = np->sport;
	unsigned short dport = np->dport;
	struct np *conflict = NULL;
	struct nc_chash_bucket *hp, *hp1, *hp2;
	unsigned long flags;

	hp1 = &nc_chash[nc_chashfn(proto, dport, sport)];
	hp2 = &nc_chash[nc_chashfn(proto, 0, 0)];

	write_lock_str2(&hp1->lock, flags);
	if (hp1 != hp2)
		write_lock(&hp2->lock);

	hp = hp1;
	do {
		register struct np *np2;

		for (np2 = hp->list; np2; np2 = np2->cnext) {
			int i, j;

			if (np_not_state(np, (NSF_DATA_XFER | NSF_WRES_RIND)))
				continue;
			if (np2->sport != sport)
				continue;
			if (np2->dport != dport)
				continue;
			for (i = 0; conflict == NULL && i < np2->snum; i++)
				for (j = 0; conflict == NULL && j < np->snum; j++)
					if (np2->saddrs[i].addr == np->saddrs[j].addr)
						conflict = np2;
			if (conflict == NULL)
				continue;
			conflict = NULL;
			for (i = 0; conflict == NULL && i < np2->dnum; i++)
				for (j = 0; conflict == NULL && j < np->dnum; j++)
					if (np2->daddrs[i].addr == np->daddrs[j].addr)
						conflict = np2;
			if (conflict == NULL)
				continue;
			break;
		}
	} while (conflict == NULL && hp != hp2 && (hp = hp2));
	if (conflict != NULL) {
		if (hp1 != hp2)
			write_unlock(&hp2->lock);
		write_unlock_str2(&hp1->lock, flags);
		/* how do we say already connected? (-EISCONN) */
		return (NBADADDR);
	}
	/* link into connection hash */
	if ((np->cnext = hp1->list))
		np->cnext->cprev = &np->cnext;
	np->cprev = &hp1->list;
	hp1->list = np_get(np);
	np->chash = hp1;
	if (hp1 != hp2)
		write_unlock(&hp2->lock);
	write_unlock_str2(&hp1->lock, flags);
	return (0);
}

/**
 * np_connect - form a connection
 * @np: private structure
 * @DEST_buffer: pointer to destination addresses
 * @DEST_length: length of destination addresses
 * @QOS_buffer: pointer to connection quality-of-service parameters
 * @CONN_flags: connection flags
 *
 * Connect to the specified port and address.  If the connection results in a conflict, TADDRBUSY is
 * returned, zero (0) on success.
 *
 * Destination addresses and port number as well as connection request quality of service parameters
 * should already be stored into the private structure.  Yes, this information will remain if there
 * is an error in the connection request.  When any primitive containing options fails and returns
 * and error, it is the caller's responsibility to set again the values of the options.
 */
STATIC fastcall int
np_connect(struct np *np, const struct sockaddr_in *DEST_buffer, socklen_t DEST_length,
	   struct N_qos_sel_conn_rtp *QOS_buffer, const np_ulong CONN_flags)
{
	size_t dnum = DEST_length / sizeof(*DEST_buffer);
	int err;
	int i;

	err = NBADQOSPARAM;
	/* first validate parameters */
	if (QOS_buffer->priority != QOS_UNKNOWN) {
		if ((np_long) QOS_buffer->priority < np->qor.priority.priority_min_value)
			goto error;
		if ((np_long) QOS_buffer->priority > np->qor.priority.priority_max_value)
			goto error;
	} else {
		QOS_buffer->priority = np->qos.priority;
	}
	if (QOS_buffer->protocol != QOS_UNKNOWN) {
		for (i = 0; i < np->pnum; i++)
			if (np->protoids[i] == QOS_buffer->protocol)
				break;
		if (i >= np->pnum)
			goto error;
	} else {
		QOS_buffer->protocol = np->qos.protocol;
	}
	if (QOS_buffer->ttl != QOS_UNKNOWN) {
		if ((np_long) QOS_buffer->ttl < np->qor.ttl.ttl_min_value)
			goto error;
		if ((np_long) QOS_buffer->ttl > np->qor.ttl.ttl_max_value)
			goto error;
	} else {
		QOS_buffer->ttl = np->qos.ttl;
	}
	if (QOS_buffer->tos != QOS_UNKNOWN) {
		if ((np_long) QOS_buffer->tos < np->qor.tos.tos_min_value)
			goto error;
		if ((np_long) QOS_buffer->tos > np->qor.tos.tos_max_value)
			goto error;
	} else {
		QOS_buffer->tos = np->qos.tos;
	}
	if (QOS_buffer->mtu != QOS_UNKNOWN) {
		if ((np_long) QOS_buffer->mtu < np->qor.mtu.mtu_min_value)
			goto error;
		if ((np_long) QOS_buffer->mtu > np->qor.mtu.mtu_max_value)
			goto error;
	} else {
		QOS_buffer->mtu = np->qos.mtu;
	}

	/* Need to determine source addressess from bound addresses before we can test the source address. If 
	   we are bound to specific addresses, then the source address list is simply the destination address 
	   list. If bound to a wildcard address, then the source address list could be determined from the
	   scope of the destination addresses and the available interfaces and their addresses. However, for
	   the moment it is probably easier to simply allow wildcard source addresses and let the user
	   specify any address when there is a wildcard source address. */

	np->sport = np->bport;
	np->snum = np->bnum;
	for (i = 0; i < np->bnum; i++)
		np->saddrs[i].addr = np->baddrs[i].addr;

	if (QOS_buffer->saddr != QOS_UNKNOWN) {
		if (QOS_buffer->saddr != 0) {
			for (i = 0; i < np->snum; i++) {
				if (np->saddrs[i].addr == INADDR_ANY)
					break;
				if (np->saddrs[i].addr == QOS_buffer->saddr)
					break;
			}
			if (i >= np->snum)
				goto recover;
		}
	} else {
		QOS_buffer->saddr = np->qos.saddr;
	}
	if (QOS_buffer->daddr != QOS_UNKNOWN) {
		/* Specified default destination address must be in the destination address list. */
		for (i = 0; i < dnum; i++)
			if (DEST_buffer[i].sin_addr.s_addr == QOS_buffer->daddr)
				break;
		if (i >= dnum)
			goto recover;
	} else {
		/* The default destination address is the first address in the list. */
		QOS_buffer->daddr = DEST_buffer[0].sin_addr.s_addr;
	}

	/* Destination addresses have been checked as follows: they have been aligned. There is at least 1
	   address and no more than 8 addresses.  The first address has an address family type of AF_INET or
	   zero (0).  No IP address in the list is INADDR_ANY.  Things that have not been checked are: there
	   might be duplicates in the list.  The user might not have the necessary privilege to use some of
	   the addresses.  Some addresses might be zeronet, broadcast or multicast addresses. The addresses
	   might be of disjoint scope.  There might not exist a route to some addresses.  The destination
	   port number might be zero. */

	np->dport = DEST_buffer[0].sin_port;

	err = NBADADDR;
	if (np->dport == 0 && (np->bport != 0 || np->sport != 0))
		goto recover;
	if (np->dport != 0 && np->sport == 0)
		/* TODO: really need to autobind the stream to a dynamically allocated source port number. */
		goto recover;

	for (i = 0; i < dnum; i++) {
		struct rtable *rt = NULL;

		if ((err = ip_route_output(&rt, DEST_buffer[i].sin_addr.s_addr, 0, 0, 0)))
			goto recover;
		np->daddrs[i].dst = rt_dst(rt);

		/* Note that we do not have to use the destination reference cached above.  It is enough that 
		   we hold a reference to it so that it remains in the routing caches so lookups to this
		   destination are fast.  They will be released upon disconnection. */

		np->daddrs[i].addr = DEST_buffer[i].sin_addr.s_addr;
		np->daddrs[i].ttl = QOS_buffer->ttl;
		np->daddrs[i].tos = QOS_buffer->tos;
		np->daddrs[i].mtu = dst_pmtu(np->daddrs[i].dst);
		if (np->daddrs[i].mtu < QOS_buffer->mtu)
			QOS_buffer->mtu = np->daddrs[i].mtu;
	}
	np->dnum = dnum;

	/* store negotiated values */
	np->qos.protocol = QOS_buffer->protocol;
	np->qos.priority = QOS_buffer->priority;
	np->qos.ttl = QOS_buffer->ttl;
	np->qos.tos = QOS_buffer->tos;
	np->qos.mtu = QOS_buffer->mtu;
	np->qos.saddr = QOS_buffer->saddr;
	np->qos.daddr = QOS_buffer->daddr;
	np->qos.checksum = QOS_buffer->checksum;
	/* note that on failure we are allowed to have partially negotiated some values */

	/* note that all these state changes are not seen by the read side until we are placed into the
	   hashes under hash lock. */

	/* try to place in connection hashes with conflict checks */
	if ((err = np_conn_check(np, QOS_buffer->protocol)) != 0)
		goto recover;

	return (0);
      recover:
	/* clear out source addresses */
	np->sport = 0;
	for (i = 0; i < np->snum; i++) {
		np->saddrs[i].addr = INADDR_ANY;
	}
	np->snum = 0;
	/* clear out destination addresses */
	np->dport = 0;
	for (i = 0; i < np->dnum; i++) {
		if (np->daddrs[i].dst)
			dst_release(XCHG(&np->daddrs[i].dst, NULL));
		np->daddrs[i].addr = INADDR_ANY;
		np->daddrs[i].ttl = 0;
		np->daddrs[i].tos = 0;
		np->daddrs[i].mtu = 0;
	}
	np->dnum = 0;
      error:
	return (err);
}

/**
 * np_reset_loc - perform a local reset
 * @np: Stream private structure
 * @RESET_orig: origin of reset
 * @RESET_reason: reason for reset
 * @dp: ICMP message payload
 *
 * When completing a local reset, it is necessary to send an ICMP message to the peer.  The attached
 * M_DATA message blocks contain the ICMP message payload.  The @RESET_reason parameter contains the
 * reset reason that translates to an ICMP error code.  The destination for the reset on a
 * multi-homed connection is the current default destination.
 */
STATIC int
np_reset_loc(struct np *np, np_ulong RESET_orig, np_ulong RESET_reason, mblk_t *dp)
{
	fixme(("Write this function.\n"));
	/* should send ICMP, but don't discard it because send function will not abosorb it. */
	return (QR_DONE);
}

/**
 * np_reset_rem - perform a remote reset
 * @np: Stream private structure
 * @RESET_orig: origin of reset
 * @RESET_reason: reason for reset
 *
 * When completing a remote reset, it is necessary to dequeue and free the earliest outstanding
 * reset indication.
 */
STATIC int
np_reset_rem(struct np *np, np_ulong RESET_orig, np_ulong RESET_reason)
{
	mblk_t *rp;

	/* free last one on list */
	if ((rp = bufq_tail(&np->resq)) != NULL) {
		bufq_unlink(&np->resq, rp);
		freemsg(rp);
	}
	return (0);
}

STATIC int
np_optmgmt(struct np *np, union N_qos_udp_types *QOS_buffer, np_ulong OPTMGMT_flags)
{
	int i;

	switch (QOS_buffer->n_qos_type) {
	case N_QOS_SEL_INFO_UDP:
		/* protocol must be one of the bound protocol ids */
		if ((np_long) QOS_buffer->n_qos_sel_info.protocol != QOS_UNKNOWN && np->pnum > 0) {
			if ((np_long) QOS_buffer->n_qos_sel_info.protocol < 0) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_info.protocol > 255) {
				return (NBADQOSPARAM);
			}
			for (i = 0; i < np->pnum; i++)
				if (np->protoids[i] == QOS_buffer->n_qos_sel_info.protocol)
					break;
			if (i >= np->pnum) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_info.priority != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_info.priority <
			    np->qor.priority.priority_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_info.priority >
			    np->qor.priority.priority_max_value) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_info.ttl != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_info.ttl < np->qor.ttl.ttl_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_info.ttl > np->qor.ttl.ttl_max_value) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_info.tos != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_info.tos < np->qor.tos.tos_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_info.tos > np->qor.tos.tos_max_value) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_info.mtu != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_info.mtu < np->qor.mtu.mtu_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_info.mtu > np->qor.mtu.mtu_max_value) {
				return (NBADQOSPARAM);
			}
		}
		/* source address should be one of the specified source addresses */
		if ((np_long) QOS_buffer->n_qos_sel_info.saddr != QOS_UNKNOWN && np->snum > 0) {
			if (QOS_buffer->n_qos_sel_info.saddr != 0) {
				for (i = 0; i < np->snum; i++) {
					if (np->saddrs[i].addr == INADDR_ANY)
						break;
					if (np->saddrs[i].addr == QOS_buffer->n_qos_sel_info.saddr)
						break;
				}
				if (i >= np->snum) {
					return (NBADQOSPARAM);
				}
			}
		}
		/* destination address must be one of the specified destination addresses */
		if ((np_long) QOS_buffer->n_qos_sel_info.daddr != QOS_UNKNOWN && np->dnum > 0) {
			for (i = 0; i < np->dnum; i++)
				if (np->daddrs[i].addr == QOS_buffer->n_qos_sel_info.daddr)
					break;
			if (i >= np->dnum) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_info.protocol != QOS_UNKNOWN)
			np->qos.protocol = QOS_buffer->n_qos_sel_info.protocol;
		if ((np_long) QOS_buffer->n_qos_sel_info.priority != QOS_UNKNOWN)
			np->qos.priority = QOS_buffer->n_qos_sel_info.priority;
		if ((np_long) QOS_buffer->n_qos_sel_info.ttl != QOS_UNKNOWN)
			np->qos.ttl = QOS_buffer->n_qos_sel_info.ttl;
		if ((np_long) QOS_buffer->n_qos_sel_info.tos != QOS_UNKNOWN)
			np->qos.tos = QOS_buffer->n_qos_sel_info.tos;
		if ((np_long) QOS_buffer->n_qos_sel_info.mtu != QOS_UNKNOWN)
			np->qos.mtu = QOS_buffer->n_qos_sel_info.mtu;
		if ((np_long) QOS_buffer->n_qos_sel_info.saddr != QOS_UNKNOWN)
			np->qos.saddr = QOS_buffer->n_qos_sel_info.saddr;
		if ((np_long) QOS_buffer->n_qos_sel_info.daddr != QOS_UNKNOWN)
			np->qos.daddr = QOS_buffer->n_qos_sel_info.daddr;
		break;
	case N_QOS_RANGE_INFO_UDP:
		return (NBADQOSTYPE);
	case N_QOS_SEL_CONN_UDP:
		if (!(np->info.SERV_type & N_CONS))
			return (NBADQOSTYPE);
		/* protocol must be one of the bound protocol ids */
		if ((np_long) QOS_buffer->n_qos_sel_conn.protocol != QOS_UNKNOWN && np->pnum > 0) {
			if ((np_long) QOS_buffer->n_qos_sel_conn.protocol < 0) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_conn.protocol > 255) {
				return (NBADQOSPARAM);
			}
			for (i = 0; i < np->pnum; i++)
				if (np->protoids[i] == QOS_buffer->n_qos_sel_conn.protocol)
					break;
			if (i >= np->pnum) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_conn.priority != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_conn.priority <
			    np->qor.priority.priority_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_conn.priority >
			    np->qor.priority.priority_max_value) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_conn.ttl != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_conn.ttl < np->qor.ttl.ttl_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_conn.ttl > np->qor.ttl.ttl_max_value) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_conn.tos != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_conn.tos < np->qor.tos.tos_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_conn.tos > np->qor.tos.tos_max_value) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_conn.mtu != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_conn.mtu < np->qor.mtu.mtu_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_conn.mtu > np->qor.mtu.mtu_max_value) {
				return (NBADQOSPARAM);
			}
		}
		/* source address should be one of the specified source addresses */
		if ((np_long) QOS_buffer->n_qos_sel_conn.saddr != QOS_UNKNOWN && np->snum > 0) {
			if (QOS_buffer->n_qos_sel_conn.saddr != 0) {
				for (i = 0; i < np->snum; i++) {
					if (np->saddrs[i].addr == INADDR_ANY)
						break;
					if (np->saddrs[i].addr == QOS_buffer->n_qos_sel_conn.saddr)
						break;
				}
				if (i >= np->snum) {
					return (NBADQOSPARAM);
				}
			}
		}
		/* destination address must be one of the specified destination addresses */
		if ((np_long) QOS_buffer->n_qos_sel_conn.daddr != QOS_UNKNOWN) {
			for (i = 0; i < np->dnum; i++)
				if (np->daddrs[i].addr == QOS_buffer->n_qos_sel_conn.daddr)
					break;
			if (i >= np->dnum) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_conn.protocol != QOS_UNKNOWN)
			np->qos.protocol = QOS_buffer->n_qos_sel_conn.protocol;
		if ((np_long) QOS_buffer->n_qos_sel_conn.priority != QOS_UNKNOWN)
			np->qos.priority = QOS_buffer->n_qos_sel_conn.priority;
		if ((np_long) QOS_buffer->n_qos_sel_conn.ttl != QOS_UNKNOWN)
			np->qos.ttl = QOS_buffer->n_qos_sel_conn.ttl;
		if ((np_long) QOS_buffer->n_qos_sel_conn.tos != QOS_UNKNOWN)
			np->qos.tos = QOS_buffer->n_qos_sel_conn.tos;
		if ((np_long) QOS_buffer->n_qos_sel_conn.mtu != QOS_UNKNOWN)
			np->qos.mtu = QOS_buffer->n_qos_sel_conn.mtu;
		if ((np_long) QOS_buffer->n_qos_sel_conn.saddr != QOS_UNKNOWN)
			np->qos.saddr = QOS_buffer->n_qos_sel_conn.saddr;
		if ((np_long) QOS_buffer->n_qos_sel_conn.daddr != QOS_UNKNOWN)
			np->qos.daddr = QOS_buffer->n_qos_sel_conn.daddr;
		np->info.SERV_type = N_CONS;
		break;
	case N_QOS_SEL_UD_UDP:
		if (!(np->info.SERV_type & N_CLNS))
			return (NBADQOSTYPE);
		/* protocol must be one of the bound protocol ids */
		if ((np_long) QOS_buffer->n_qos_sel_ud.protocol != QOS_UNKNOWN && np->pnum > 0) {
			if ((np_long) QOS_buffer->n_qos_sel_ud.protocol < 0) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_ud.protocol > 255) {
				return (NBADQOSPARAM);
			}
			for (i = 0; i < np->pnum; i++)
				if (np->protoids[i] == QOS_buffer->n_qos_sel_ud.protocol)
					break;
			if (i >= np->pnum) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_ud.priority != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_ud.priority < np->qor.priority.priority_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_ud.priority > np->qor.priority.priority_max_value) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_ud.ttl != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_ud.ttl < np->qor.ttl.ttl_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_ud.ttl > np->qor.ttl.ttl_max_value) {
				return (NBADQOSPARAM);
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_ud.tos != QOS_UNKNOWN) {
			if ((np_long) QOS_buffer->n_qos_sel_ud.tos < np->qor.tos.tos_min_value) {
				return (NBADQOSPARAM);
			}
			if ((np_long) QOS_buffer->n_qos_sel_ud.tos > np->qor.tos.tos_max_value) {
				return (NBADQOSPARAM);
			}
		}
		/* source address should be one of the specified source addresses */
		if ((np_long) QOS_buffer->n_qos_sel_ud.saddr != QOS_UNKNOWN && np->snum > 0) {
			if (QOS_buffer->n_qos_sel_ud.saddr != 0) {
				for (i = 0; i < np->snum; i++) {
					if (np->saddrs[i].addr == INADDR_ANY)
						break;
					if (np->saddrs[i].addr == QOS_buffer->n_qos_sel_ud.saddr)
						break;
				}
				if (i >= np->snum) {
					return (NBADQOSPARAM);
				}
			}
		}
		if ((np_long) QOS_buffer->n_qos_sel_ud.protocol != QOS_UNKNOWN)
			np->qos.protocol = QOS_buffer->n_qos_sel_ud.protocol;
		if ((np_long) QOS_buffer->n_qos_sel_ud.priority != QOS_UNKNOWN)
			np->qos.priority = QOS_buffer->n_qos_sel_ud.priority;
		if ((np_long) QOS_buffer->n_qos_sel_ud.ttl != QOS_UNKNOWN)
			np->qos.ttl = QOS_buffer->n_qos_sel_ud.ttl;
		if ((np_long) QOS_buffer->n_qos_sel_ud.tos != QOS_UNKNOWN)
			np->qos.tos = QOS_buffer->n_qos_sel_ud.tos;
		if ((np_long) QOS_buffer->n_qos_sel_ud.saddr != QOS_UNKNOWN)
			np->qos.saddr = QOS_buffer->n_qos_sel_ud.saddr;
		np->info.SERV_type = N_CLNS;
		break;
	default:
		return (NBADQOSTYPE);
	}
	return (0);
}

/**
 * np_unbind - unbind a Stream from an NSAP
 * @np: private structure
 *
 * Simply remove the Stream from the bind hashes and release a reference to the Stream.  This
 * function can be called whether the stream is bound or not (and is always called before the
 * private structure is freed.
 */
STATIC int
np_unbind(struct np *np)
{
	struct nc_bhash_bucket *hp;
	unsigned long flags;

	if ((hp = np->base.bhash)) {
		write_lock_str2(&hp->lock, flags);
		if ((*np->base.bprev = np->base.bnext))
			np->base.bnext->bprev = np->base.bprev;
		np->base.bnext = NULL;
		np->base.bprev = &np->base.bnext;
		np->base.bhash = NULL;
		np_unbind_prot(np->base.protoids[0], np->base.SERV_type);
		np->base.bport = np->base.sport = 0;
		np->base.bnum = np->base.snum = np->base.pnum = 0;
		np_set_state(np, NS_UNBND);
		np_put(np);
		write_unlock_str2(&hp->lock, flags);
#if defined HAVE_KFUNC_SYNCHRONIZE_NET
#if defined HAVE_KFUNC_IN_ATOMIC || defined in_atomic
		if (!in_interrupt() && !in_atomic())
#else
		if (!in_interrupt())
#endif
			synchronize_net();	/* might sleep */
#endif				/* defined HAVE_KFUNC_SYNCHRONIZE_NET */
		return (0);
	}
	return (-EALREADY);
}

/**
 * nc_unbind - unbind a virtual interface from an NSAP
 * @np: private structure
 * @nc: network connection
 *
 * Simply remove the network connection from the bind hashes.  This function can be called whether
 * the network connection is bound or not (and is always called before the private structure is
 * freed.
 */
STATIC int
nc_unbind(struct np *np, struct nc *nc)
{
	struct nc_bhash_bucket *hp;
	unsigned long flags;

	if ((hp = nc->bhash)) {
		write_lock_str2(&hp->lock, flags);
		if ((*nc->bprev = nc->bnext))
			nc->bnext->bprev = nc->bprev;
		nc->bnext = NULL;
		nc->bprev = &nc->bnext;
		nc->bhash = NULL;
		np_unbind_prot(nc->protoids[0], nc->SERV_type);
		nc->bport = nc->sport = 0;
		nc->bnum = nc->snum = nc->pnum = 0;
		nc_set_state(np->oq, nc, NS_UNBND);
		write_unlock_str2(&hp->lock, flags);
#if defined HAVE_KFUNC_SYNCHRONIZE_NET
#if defined HAVE_KFUNC_IN_ATOMIC || defined in_atomic
		if (!in_interrupt() && !in_atomic())
#else
		if (!in_interrupt())
#endif
			synchronize_net();	/* might sleep */
#endif				/* defined HAVE_KFUNC_SYNCHRONIZE_NET */
		return (0);
	}
	return (-EALREADY);
}

/**
 * np_passive - perform a passive connection
 * @np: private structure
 * @RES_buffer: responding addresses
 * @RES_length: length of responding addresses
 * @QOS_buffer: quality of service parameters
 * @SEQ_number: connection indication being accepted
 * @TOKEN_value: accepting Stream private structure
 * @CONN_flags: connection flags
 * @dp: user connect data
 */
noinline fastcall int
np_passive(struct np *np, struct sockaddr_in *RES_buffer, const socklen_t RES_length,
	   struct N_qos_sel_conn_rtp *QOS_buffer, mblk_t *SEQ_number, struct np *TOKEN_value,
	   const np_ulong CONN_flags, mblk_t *dp)
{
	size_t rnum = RES_length / sizeof(*RES_buffer);
	int err;
	struct iphdr *iph;
	struct udphdr *uh;
	int i, j;

	/* Get at the connection indication.  The packet is contained in the SEQ_number message block
	   starting with the IP header. */
	iph = (typeof(iph)) SEQ_number->b_rptr;
	uh = (typeof(uh)) (SEQ_number->b_rptr + (iph->ihl << 2));

	if (TOKEN_value != np) {
		err = NBADTOKEN;
		/* Accepting Stream must be bound to the same protocol as connection indication. */
		for (j = 0; j < TOKEN_value->pnum; j++)
			if (TOKEN_value->protoids[j] == iph->protocol)
				break;
		if (j >= TOKEN_value->pnum)
			/* Must be bound to the same protocol. */
			goto error;
		/* Accepting Stream must be bound to the same address (or wildcard) including destination
		   address in connection indication. */
		for (i = 0; i < TOKEN_value->bnum; i++)
			if (TOKEN_value->baddrs[i].addr == INADDR_ANY
			    || TOKEN_value->baddrs[i].addr == iph->daddr)
				break;
		if (i >= TOKEN_value->bnum)
			goto error;
	}

	/* validate parameters */
	err = NBADQOSPARAM;
	/* Cannot really validate parameters here.  One of the problems is that some of the information
	   against which we should be checking is contained in the connection indication packet, and other
	   information is associated with the destination addresses themselves, that are contained in the
	   responding address(es) for NPI-IP.  Therefore, QOS parameter checks must be performed in the
	   np_passive() function instead. */
	if (QOS_buffer->protocol != QOS_UNKNOWN) {
		/* Specified protocol probably needs to be the same as the indication, but since we only bind 
		   to one protocol id at the moment that is not a problem.  The connection indication
		   protocol was checked against the accepting Stream above. */
		for (i = 0; i < TOKEN_value->pnum; i++)
			if (TOKEN_value->protoids[i] == QOS_buffer->protocol)
				break;
		if (i >= TOKEN_value->pnum)
			goto error;
	} else {
		QOS_buffer->protocol = TOKEN_value->qos.protocol;
	}
	if (QOS_buffer->priority != QOS_UNKNOWN) {
		if ((np_long) QOS_buffer->priority < TOKEN_value->qor.priority.priority_min_value)
			goto error;
		if ((np_long) QOS_buffer->priority > TOKEN_value->qor.priority.priority_max_value)
			goto error;
	} else {
		QOS_buffer->priority = TOKEN_value->qos.priority;
	}
	if (QOS_buffer->ttl != QOS_UNKNOWN) {
		if ((np_long) QOS_buffer->ttl < TOKEN_value->qor.ttl.ttl_min_value)
			goto error;
		if ((np_long) QOS_buffer->ttl > TOKEN_value->qor.ttl.ttl_max_value)
			goto error;
	} else {
		QOS_buffer->ttl = TOKEN_value->qos.ttl;
	}
	if (QOS_buffer->tos != QOS_UNKNOWN) {
		if ((np_long) QOS_buffer->tos < TOKEN_value->qor.tos.tos_min_value)
			goto error;
		if ((np_long) QOS_buffer->tos > TOKEN_value->qor.tos.tos_max_value)
			goto error;
	} else {
		/* FIXME: TOS should be negotiated.  The TOS should be upgraded to whatever TOS the caller
		   wishes, but not downgraded. */
		QOS_buffer->tos = TOKEN_value->qos.tos;
	}
	if (QOS_buffer->mtu != QOS_UNKNOWN) {
		if ((np_long) QOS_buffer->mtu < TOKEN_value->qor.mtu.mtu_min_value)
			goto error;
		if ((np_long) QOS_buffer->mtu > TOKEN_value->qor.mtu.mtu_max_value)
			goto error;
		/* FIXME: MTU should be negotiated.  The MTU should be downgraded to the lesser value of what 
		   the connection requires or what was specified, but not upgraded. */
	} else {
		QOS_buffer->mtu = TOKEN_value->qos.mtu;
	}

	/* Need to determine source addressess from bound addresses before we can test the source address. If 
	   we are bound to specific addresses, then the source address list is simply the destination address 
	   list. If bound to a wildcard address, then the source address list could be determined from the
	   scope of the destination addresses and the available interfaces and their addresses. However, for
	   the moment it is probably easier to simply allow wildcard source addresses and let the user
	   specify any address when there is a wildcard source address.  Port number is a different
	   situation: either the Stream is bound to the port number in the received connection indication, or 
	   it was bound to a wildcard port number.  In either case, the local port number for the connection
	   is the port number to which the connection indication was sent. */

	TOKEN_value->sport = uh->dest;
	TOKEN_value->snum = TOKEN_value->bnum;
	for (i = 0; i < TOKEN_value->bnum; i++)
		TOKEN_value->saddrs[i].addr = TOKEN_value->baddrs[i].addr;

	if (QOS_buffer->saddr != QOS_UNKNOWN) {
		if (QOS_buffer->saddr != 0) {
			for (i = 0; i < TOKEN_value->snum; i++) {
				if (TOKEN_value->saddrs[i].addr == INADDR_ANY)
					break;
				if (TOKEN_value->saddrs[i].addr == QOS_buffer->saddr)
					break;
			}
			if (i >= TOKEN_value->snum)
				goto recover;
		}
	} else {
		QOS_buffer->saddr = TOKEN_value->qos.saddr;
	}

	/* Here's a problem: we don't realy have any destination addresses yet, so we can't check at this
	   point. */

	if (QOS_buffer->daddr != QOS_UNKNOWN) {
		if (rnum > 0) {
			/* Specified destination addresses must be in the responding address list. */
			for (i = 0; i < rnum; i++)
				if (RES_buffer[i].sin_addr.s_addr == QOS_buffer->daddr)
					break;
			if (i >= rnum)
				goto recover;
		} else {
			/* If no responding address list is provided (rnum == 0), the destination address
			   must be the source address of the connection indication. */
			if (QOS_buffer->daddr != iph->saddr)
				goto recover;
		}
	} else {
		QOS_buffer->daddr = rnum ? RES_buffer[0].sin_addr.s_addr : iph->saddr;
	}

	TOKEN_value->dport = rnum ? RES_buffer[0].sin_port : uh->source;

	err = NBADADDR;
	if (TOKEN_value->dport == 0 && (TOKEN_value->bport != 0 || TOKEN_value->sport != 0))
		goto recover;
	if (TOKEN_value->dport != 0 && TOKEN_value->sport == 0)
		/* TODO: really need to autobind the stream to a dynamically allocated source port number. */
		goto recover;

	if (rnum > 0) {
		for (i = 0; i < rnum; i++) {
			struct rtable *rt = NULL;

			if ((err = ip_route_output(&rt, RES_buffer[i].sin_addr.s_addr, 0, 0, 0)))
				goto recover;
			TOKEN_value->daddrs[i].dst = rt_dst(rt);

			/* Note that we do not have to use the destination reference cached above. It is
			   enough that we hold a reference to it so that it remains in the routing caches so
			   lookups to this destination are fast.  They will be released upon disconnection. */

			TOKEN_value->daddrs[i].addr = RES_buffer[i].sin_addr.s_addr;
			TOKEN_value->daddrs[i].ttl = QOS_buffer->ttl;
			TOKEN_value->daddrs[i].tos = QOS_buffer->tos;
			TOKEN_value->daddrs[i].mtu = dst_pmtu(TOKEN_value->daddrs[i].dst);
			if (TOKEN_value->daddrs[i].mtu < QOS_buffer->mtu)
				QOS_buffer->mtu = TOKEN_value->daddrs[i].mtu;
		}
		TOKEN_value->dnum = rnum;
	} else {
		struct rtable *rt = NULL;

		if ((err = ip_route_output(&rt, iph->saddr, 0, 0, 0)))
			goto recover;
		TOKEN_value->daddrs[0].dst = rt_dst(rt);

		/* Note that we do not have to use the destination reference cached above.  It is enough that 
		   we hold a reference to it so that it remains in the routing caches so lookups to this
		   destination are fast.  They will be released upon disconnection. */

		TOKEN_value->daddrs[0].addr = iph->saddr;
		TOKEN_value->daddrs[0].ttl = QOS_buffer->ttl;
		TOKEN_value->daddrs[0].tos = QOS_buffer->tos;
		TOKEN_value->daddrs[0].mtu = dst_pmtu(TOKEN_value->daddrs[0].dst);
		if (TOKEN_value->daddrs[0].mtu < QOS_buffer->mtu)
			QOS_buffer->mtu = TOKEN_value->daddrs[0].mtu;

		TOKEN_value->dnum = 1;
	}

	/* store negotiated qos values */
	TOKEN_value->qos.protocol = QOS_buffer->protocol;
	TOKEN_value->qos.priority = QOS_buffer->priority;
	TOKEN_value->qos.ttl = QOS_buffer->ttl;
	TOKEN_value->qos.tos = QOS_buffer->tos;
	TOKEN_value->qos.mtu = QOS_buffer->mtu;
	TOKEN_value->qos.saddr = QOS_buffer->saddr;
	TOKEN_value->qos.daddr = QOS_buffer->daddr;
	/* note: on failure allowed to have partially negotiated options */

	/* try to place in connection hashes with conflict checks */
	if ((err = np_conn_check(TOKEN_value, QOS_buffer->protocol)) != 0)
		goto recover;

	if (dp != NULL)
		if (unlikely((err = np_senddata(np, np->qos.protocol, np->dport, np->qos.daddr, dp)) != QR_ABSORBED))
			goto recover;
	if (SEQ_number != NULL) {
		bufq_unlink(&np->conq, SEQ_number);
		freeb(XCHG(&SEQ_number, SEQ_number->b_cont));
		/* queue any pending data */
		while (SEQ_number)
			put(TOKEN_value->oq, XCHG(&SEQ_number, SEQ_number->b_cont));
	}

	return (QR_ABSORBED);

      recover:
	/* clear out source addresses */
	TOKEN_value->sport = 0;
	for (i = 0; i < TOKEN_value->snum; i++) {
		TOKEN_value->saddrs[i].addr = INADDR_ANY;
	}
	TOKEN_value->snum = 0;
	/* clear out destination addresses */
	TOKEN_value->dport = 0;
	for (i = 0; i < TOKEN_value->dnum; i++) {
		if (TOKEN_value->daddrs[i].dst)
			dst_release(XCHG(&TOKEN_value->daddrs[i].dst, NULL));
		TOKEN_value->daddrs[i].addr = INADDR_ANY;
		TOKEN_value->daddrs[i].ttl = 0;
		TOKEN_value->daddrs[i].tos = 0;
		TOKEN_value->daddrs[i].mtu = 0;
	}
	TOKEN_value->dnum = 0;
      error:
	return (err);
}

/**
 * np_disconnect - disconnect a network connection
 * @np: private structure
 * @RES_buffer: responding address (unused)
 * @SEQ_number: connection indication being refused
 * @DISCON_reason: disconnect reason (unused)
 * @dp: user disconnect data
 *
 * Removes the network connection from the connection hashes and sets the state to NS_IDLE.
 * Note that this function always succeeds when not given data.
 */
STATIC int
np_disconnect(struct np *np, struct sockaddr_in *RES_buffer, mblk_t *SEQ_number, const np_ulong DISCON_reason,
	      mblk_t *dp)
{
	struct nc_chash_bucket *hp;
	int err = 0;
	unsigned long flags;

	if (dp != NULL) {
		if ((err = np_senddata(np, np->qos.protocol, np->dport, np->qos.daddr, dp)))
			goto error;
	}
	if (SEQ_number != NULL) {
		bufq_unlink(&np->base.conq, SEQ_number);
		freemsg(SEQ_number);
	}
	if ((hp = np->base.chash) != NULL) {
		write_lock_str2(&hp->lock, flags);
		if ((*np->base.cprev = np->base.cnext))
			np->base.cnext->cprev = np->base.cprev;
		np->base.cnext = NULL;
		np->base.cprev = &np->base.cnext;
		np->base.chash = NULL;
		np->base.dport = np->base.sport = 0;
		np->base.dnum = np->base.snum = 0;
		np_set_state(np, NS_IDLE);
		np_put(np);	/* XXX */
		write_unlock_str2(&hp->lock, flags);
	}
      error:
	return (err);
}

/** nc_disconnect - disconnect a virtual network connection
  * @np: private structure
  * @nc: network connection
  * @RES_buffer: responding address (unused)
  * @SEQ_number: connection indication being refused when non-NULL
  * @DISCON_reason: disconnect reason (unused)
  * @dp: user disconnect data
  *
  * Removes the virtual network connection from the connection hashes and sets the state to NS_IDLE.
  * Note that this function always succeeds when not given data.
  */
STATIC int
nc_disconnect(struct np *np, struct nc *nc, struct sockaddr_in *RES_buffer, mblk_t *SEQ_number,
	      const np_ulong DISCON_reason, mblk_t *dp)
{
	struct nc_chash_bucket *hp;
	int err = 0;
	unsigned long flags;

	if (dp != NULL) {
		/* FIXME: not right yet... */
		if ((err = nc_senddata(np, nc, np->qos.protocol, nc->dport, np->qos.daddr, dp)))
			goto error;
	}
	if (SEQ_number != NULL) {
		bufq_unlink(&nc->conq, SEQ_number);
		freemsg(SEQ_number);
	}
	if ((hp = nc->chash) != NULL) {
		write_lock_str2(&hp->lock, flags);
		if ((*nc->cprev = nc->cnext))
			nc->cnext->cprev = nc->cprev;
		nc->cnext = NULL;
		nc->cprev = &nc->cnext;
		nc->chash = NULL;
		nc->dport = nc->sport = 0;
		nc->dnum = nc->snum = 0;
		nc_set_state(np->oq, nc, NS_IDLE);
		write_unlock_str2(&np->lock, flags);
	}
      error:
	return (err);
}

/*
 *  Addressing:
 *
 *  NSAPs (Protocol IDs) are IP protocol numbers.  NSAP addresses consist of a port number and a
 *  list of IP addreses.  If the port number is zero, any port number is used.  Initially it is only
 *  supporting IPv4.
 *
 *  There are two types of providers: connectionless and connection oriented.
 *
 *  - Connectionless providers will start delivering packets after the bind.
 *
 *  - When the NS provider is bound to multiple protocol ids, or bound or connected to multiple
 *    addresses, data will be delivered as N_DATA_IND primitives that contain the protocol id index,
 *    destination address index, and source addresses index in the DATA_xfer_flags as the highest
 *    order byte, next highest order byte and so on.  An index of 0 indicates the first bound
 *    protocol id, source address or destination address.  In this way, the high order 3 bytes of
 *    the DATA_xfer_flags are coded all zeros in the non-multiple case.
 *
 *    The NS user is also permitted to send N_DATA_REQ primitives that contain the protocol id,
 *    destination address, and source addresses, similarly encoded in the DATA_xfer_flags.  Invalid
 *    indexes will error the stream.
 *
 *  - Connection oriented provider bound as listening will start delivering packets as connection
 *    indications after the bind.  The user can either reject these with a disconnect request or can
 *    accept them with a connection response, with a slight difference from normal NPI: the
 *    responding address in the connection response is the list of peer addresses to which to
 *    establish a connection connect rather than the local responding address.
 *
 *    If the connection is accepted on the listening stream, no further connection indications will
 *    be delivered.  If accepted on another stream, further connection indications (belonging to
 *    annother association) will be delivered.
 *
 *  - Connection oriented providers bound as non-listening will deliver packets only after a
 *    successful connection establishment operation.  Connection establishment does not doe anything
 *    except to provide a port and list of addresses from which the provider is to deliver packets.
 *    This can be useful for RTP and for establishing endpoint communication with SCTP.
 *
 *  - Before bind, the provider will report both N_CLNS and N_CONS.  When bound with bind flags
 *    equal to N_CLNS in the second least significant BIND_flags byte, the provider will be
 *    connectionless.  When bound with bind flags equal to N_CONS in the second least significant
 *    BIND_flags byte, the provider will be connection-oriented.
 */

/*
 *  TS Provider -> TS User (Indication, Confirmation and Ack) Primitives
 *  ====================================================================
 */

/**
 * m_flush: deliver an M_FLUSH message upstream
 * @q: a queue in the queue pair
 * @how: FLUSHBAND or FLUSHALL
 * @band: band to flush if how is FLUSHBAND
 */
noinline fastcall int
m_flush(queue_t *q, const int how, const int band)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp;

	if (unlikely((mp = mi_allocb(q, 2, BPRI_HI)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_FLUSH;
	*mp->b_wptr++ = how;
	*mp->b_wptr++ = band;
	putnext(np->oq, mp);
	return (QR_DONE);
      enobufs:
	return (-ENOBUFS);
}

/**
 * m_error: deliver an M_ERROR message upstream
 * @q: a queue in the queue pair (write queue)
 * @error: the error to deliver
 */
noinline fastcall __unlikely int
m_error(queue_t *q, const int error)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, 2, BPRI_HI)) != NULL)) {
		mp->b_datap->db_type = M_ERROR;
		mp->b_wptr[0] = mp->b_wptr[1] = error;
		mp->b_wptr += 2;
		/* make sure the stream is disconnected */
		np_disconnect(np, NULL, NULL, N_REASON_UNDEFINED, NULL);
		/* make sure the stream is unbound */
		np_unbind(np);
		_printd(("%s: %p: <- M_ERROR %d\n", DRV_NAME, np, error));
		qreply(q, mp);
		return (QR_DONE);
	}
	return (-ENOBUFS);
}

/**
 * m_hangup: deliver an M_HANGUP message upstream
 * @q: a queue in the queue pair (write queue)
 */
noinline fastcall __unlikely int
m_hangup(queue_t *q)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp;

	if (likely((mp = mi_allocb(q, 0, BPRI_HI)) != NULL)) {
		mp->b_datap->db_type = M_HANGUP;
		/* make sure the stream is disconnected */
		np_disconnect(np, NULL, NULL, N_REASON_UNDEFINED, NULL);
		/* make sure the stream is unbound */
		np_unbind(np);
		_printd(("%s: %p: <- M_HANGUP\n", DRV_NAME, np));
		qreply(q, mp);
		return (QR_DONE);

	}
	return (-ENOBUFS);
}

/**
 * np_error_fatal - reply to a message with an M_ERROR message
 * @q: active queue in queue pair (write queue)
 * @error: error number
 */
noinline fastcall __unlikely int
np_error_fatal(queue_t *q, const long error)
{
	switch (error) {
	case 0:
	case 1:
	case 2:
		__seldom();
		return (error);
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
	case -EDEADLK:
		return (error);
	case -EPIPE:
	case -ENETDOWN:
	case -EHOSTUNREACH:
		return m_hangup(q);
	default:
		return m_error(q, EPROTO);
	}
}

/**
 * np_info_ack - generate an N_INFO_ACK and pass it upstream
 * @q: active queue in queue pair (write queue)
 */
noinline fastcall int
np_info_ack(queue_t *q)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp;
	N_info_ack_t *p;
	struct sockaddr_in *ADDR_buffer;
	struct N_qos_sel_info_udp *QOS_buffer = &np->qos;
	struct N_qos_range_info_udp *QOS_range_buffer = &np->qor;
	unsigned char *PROTOID_buffer = np->protoids;
	size_t ADDR_length = np->snum * sizeof(*ADDR_buffer);
	size_t QOS_length = sizeof(*QOS_buffer);
	size_t QOS_range_length = sizeof(*QOS_range_buffer);
	size_t PROTOID_length = np->pnum;
	size_t size = sizeof(*p) + ADDR_length + QOS_length + QOS_range_length + PROTOID_length;

	if (unlikely((mp = mi_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_INFO_ACK;
	p->NSDU_size = np->info.NSDU_size;
	p->ENSDU_size = np->info.ENSDU_size;
	p->CDATA_size = np->info.CDATA_size;
	p->DDATA_size = np->info.DDATA_size;
	p->ADDR_size = np->info.ADDR_size;
	p->ADDR_length = ADDR_length;
	p->ADDR_offset = ADDR_length ? sizeof(*p) : 0;
	p->QOS_length = QOS_length;
	p->QOS_offset = QOS_length ? sizeof(*p) + ADDR_length : 0;
	p->QOS_range_length = QOS_range_length;
	p->QOS_range_offset = QOS_range_length ? sizeof(*p) + ADDR_length + QOS_length : 0;
	p->OPTIONS_flags = np->info.OPTIONS_flags;
	p->NIDU_size = np->info.NIDU_size;
	p->SERV_type = np->info.SERV_type ? np->info.SERV_type : (N_CONS | N_CLNS);
	p->CURRENT_state = np_get_state(np);
	p->PROVIDER_type = np->info.PROVIDER_type;
	p->NODU_size = 536;
	p->PROTOID_length = PROTOID_length;
	p->PROTOID_offset = PROTOID_length ? sizeof(*p) + ADDR_length + QOS_length + QOS_range_length : 0;
	p->NPI_version = np->info.NPI_version;
	mp->b_wptr += sizeof(*p);
	if (ADDR_length) {
		int i;

		for (i = 0; i < np->snum; i++) {
			ADDR_buffer = (struct sockaddr_in *) mp->b_wptr;
			ADDR_buffer->sin_family = AF_INET;
			ADDR_buffer->sin_port = np->sport;
			ADDR_buffer->sin_addr.s_addr = np->saddrs[i].addr;
			mp->b_wptr += sizeof(struct sockaddr_in);
		}
	}
	if (QOS_length) {
		bcopy(QOS_buffer, mp->b_wptr, QOS_length);
		mp->b_wptr += QOS_length;
	}
	if (QOS_range_length) {
		bcopy(QOS_range_buffer, mp->b_wptr, QOS_range_length);
		mp->b_wptr += QOS_range_length;
	}
	if (PROTOID_length) {
		bcopy(PROTOID_buffer, mp->b_wptr, PROTOID_length);
		mp->b_wptr += PROTOID_length;
	}
	_printd(("%s: %p: <- N_INFO_ACK\n", DRV_NAME, np));
	qreply(q, mp);
	return (QR_DONE);

      enobufs:
	return (-ENOBUFS);
}

/**
 * ne_bind_ack - NE_BIND_ACK event
 * @q: active queue in queue pair (write queue)
 * @PROTOID_buffer: protocol ids to bind
 * @PROTOID_length: length of protocol ids
 * @ADDR_buffer: addresses to bind
 * @ADDR_length: length of addresses
 * @CONIND_number: maximum number of connection indications
 * @BIND_flags: bind flags
 *
 * Generate an N_BIND_ACK and pass it upstream.
 */
noinline fastcall int
ne_bind_ack(queue_t *q, unsigned char *PROTOID_buffer, size_t PROTOID_length, struct sockaddr_in *ADDR_buffer,
	    socklen_t ADDR_length, np_ulong CONIND_number, np_ulong BIND_flags)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp = NULL;
	N_bind_ack_t *p;
	size_t size = sizeof(*p) + ADDR_length + PROTOID_length;
	int err;

	err = -EFAULT;
	if (unlikely(np_get_state(np) != NS_WACK_BREQ))
		goto error;

	err = -ENOBUFS;
	if (unlikely((mp = mi_allocb(q, size, BPRI_MED)) == NULL))
		goto error;

	err =
	    np_bind(np, PROTOID_buffer, PROTOID_length, ADDR_buffer, ADDR_length, CONIND_number, BIND_flags);
	if (unlikely(err != 0)) {
		freeb(mp);
		goto error;
	}

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_BIND_ACK;
	p->ADDR_length = ADDR_length;
	p->ADDR_offset = ADDR_length ? sizeof(*p) : 0;
	p->CONIND_number = CONIND_number;
	p->TOKEN_value = (BIND_flags & TOKEN_REQUEST) ? (long) RD(q) : 0;
	p->PROTOID_length = PROTOID_length;
	p->PROTOID_offset = PROTOID_length ? sizeof(*p) + ADDR_length : 0;
	mp->b_wptr += sizeof(*p);
	if (ADDR_length) {
		bcopy(ADDR_buffer, mp->b_wptr, ADDR_length);
		mp->b_wptr += ADDR_length;
	}
	if (PROTOID_length) {
		bcopy(PROTOID_buffer, mp->b_wptr, PROTOID_length);
		mp->b_wptr += PROTOID_length;
	}
	/* all ready, complete the bind */
	np_set_state(np, NS_IDLE);
	_printd(("%s: %p: <- N_BIND_ACK\n", DRV_NAME, np));
	qreply(q, mp);
	return (QR_DONE);

      error:
	return (err);
}

/** ne_error_ack - genereate an N_ERROR_ACK and pass it upstream
  * @q: active queue
  * @msg: message to free upon success
  * @np: private structure (locked)
  * @ERROR_prim: primitive in error
  * @error: error (postitive NPI_error, negative UNIX_error)
  */
noinline fastcall __unlikely int
ne_error_ack(queue_t *q, mblk_t *msg, struct np *np, np_ulong ERROR_prim, np_long error)
{
	N_error_ack_t *p;
	mblk_t *mp;
	int state;

	if (unlikely(!(mi_allocb(q, sizeof(*p), BPRI_MED))))
		goto enobufs;

	DB_TYPE(mp) = M_PCPROTO;

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_ERROR_ACK;
	p->ERROR_prim = ERROR_prim;
	p->NPI_error = (error < 0) ? NSYSERR : error;
	p->UNIX_error = (error < 0) ? -error : 0;
	mp->b_wptr += sizeof(*p);

	switch ((state = np_get_state(np))) {
	case NS_WACK_BREQ:
		state = NS_UNBND;
		break;
	case NS_WACK_UREQ:
		state = NS_IDLE;
		break;
	case NS_WACK_OPTREQ:
		state = NS_IDLE;
		break;
	case NS_WACK_RRES:
		state = NS_WRES_RIND;
		break;
	case NS_WCON_CREQ:
		state = NS_IDLE;
		break;
	case NS_WACK_CRES:
		state = NS_WRES_CIND;
		break;
	case NS_WCON_RREQ:
		state = NS_DATA_XFER;
		break;
	case NS_WACK_DREQ6:
		state = NS_WCON_CREQ;
		break;
	case NS_WACK_DREQ7:
		state = NS_WRES_CIND;
		break;
	case NS_WACK_DREQ9:
		state = NS_DATA_XFER;
		break;
	case NS_WACK_DREQ10:
		state = NS_WCON_RREQ;
		break;
	case NS_WACK_DREQ11:
		state = NS_WRES_RIND;
		break;
	}
	np_set_state(np, state);

	freemsg(msg);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_ERROR_ACK");
	putnext(np->oq, mp);
	return (0);

      enobufs:
	return (-ENOBUFS);
}

/**
 * np_error_reply - generate an N_ERROR_ACK and pass it upstream
 * @q: active queue in queue pair (write queue)
 * @msg: message to free upon success
 * @np: private structure (locked)
 * @ERROR_prim: primitive in error
 * @err: error (positive NPI_error, negative UNIX_error)
 *
 * Some errors are passed through because they are intercepted and used by the put or service
 * procedure on the write side queue.  Theses are 0 (no error), -EBUSY (canput failed), -EAGAIN
 * (lock failed), -ENOMEM (kmem_alloc failed), -ENOBUFS (allocb failed).  All of these pass-through
 * errors will cause the state to be restored to the state before the current write queue message
 * was processed and the current write queue message will be placed back on the write queue.
 *
 * Some BSD-specific UNIX errors are translated to equivalent NPI errors.
 *
 * This function will also place the NPI provider interface state in the correct state following
 * issuing the N_ERROR_ACK according to the Sequence of Primities of the Network Provider Interface
 * specification, Revision 2.0.0.
 */
noinline fastcall __unlikely int
np_error_reply(queue_t *q, mblk_t *msg, struct np *np, np_ulong ERROR_prim, np_long error)
{
	np_restore_total_state(np);	/* rollback to checkpoint */

	switch (__builtin_expect(error, 0)) {
	case 0:
		/* message must be discarded */
		freemsg(msg);
		/* fall through */
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
	case -EDEADLK:
		/* message is going back on the queue */
		goto error;
	case -EADDRINUSE:
		error = NBOUND;
		break;
	case -EADDRNOTAVAIL:
	case -EDESTADDRREQ:
		error = NNOADDR;
		break;
	case -EMSGSIZE:
		error = NBADDATA;
		break;
	case -EACCES:
		error = NACCESS;
		break;
	case -EOPNOTSUPP:
		error = NNOTSUPPORT;
		break;
	}
	return ne_error_ack(q, msg, np, ERROR_prim, error);
      error:
	return (error);
}

/**
 * np_ok_ack - generate an N_OK_ACK and pass it upstream
 * @q: active queue in queue pair (write queue)
 * @CORRECT_prim: correct primitive
 * @ADDR_buffer: destination or responding address
 * @ADDR_length: length of destination or responding addresses
 * @QOS_buffer: quality of service parameters
 * @SEQ_number: sequence number (i.e. connection/reset indication sequence number)
 * @TOKEN_value: token (i.e. connection response token)
 * @flags: mangement flags, connection flags, disconnect reason, etc.
 * @dp: user data
 */
STATIC int
np_ok_ack(queue_t *q, np_ulong CORRECT_prim, struct sockaddr_in *ADDR_buffer, socklen_t ADDR_length,
	  void *QOS_buffer, mblk_t *SEQ_number, struct np *TOKEN_value, np_ulong flags, mblk_t *dp)
{
	struct np *np = NP_PRIV(q);
	N_ok_ack_t *p;
	mblk_t *mp;
	const size_t size = sizeof(*p);
	int err = QR_DONE;
	pl_t pl;

	if (unlikely((mp = mi_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_OK_ACK;
	p->CORRECT_prim = CORRECT_prim;
	mp->b_wptr += sizeof(*p);
	switch (np_get_state(np)) {
	case NS_WACK_OPTREQ:
		err = np_optmgmt(np, QOS_buffer, flags);
		if (unlikely(err != 0))
			goto free_error;
		pl = bufq_lock(&np->conq);
		np_set_state(np, bufq_length(&np->conq) > 0 ? NS_WRES_CIND : NS_IDLE);
		bufq_unlock(&np->conq, pl);
		break;
	case NS_WACK_UREQ:
		err = np_unbind(np);
		if (unlikely(err != 0))
			goto free_error;
		/* NPI spec says that if the provider must flush both queues before responding with a
		   N_OK_ACK primitive when responding to a N_UNBIND_REQ. This is to flush queued data for
		   connectionless providers. */
		err = m_flush(q, FLUSHRW, 0);
		if (unlikely(err != 0))
			goto free_error;
		np_set_state(np, NS_UNBND);
		break;
	case NS_WACK_CRES:
		/* FIXME: needs to hold reference to and lock the accepting stream */
		if (np != TOKEN_value)
			TOKEN_value->i_oldstate = np_get_state(TOKEN_value);
		np_set_state(TOKEN_value, NS_DATA_XFER);
		err =
		    np_passive(np, ADDR_buffer, ADDR_length, QOS_buffer, SEQ_number, TOKEN_value, flags, dp);
		if (unlikely(err != QR_ABSORBED)) {
			np_set_state(TOKEN_value, TOKEN_value->i_oldstate);
			goto free_error;
		}
		if (np != TOKEN_value) {
			pl = bufq_lock(&np->conq);
			np_set_state(np, bufq_length(&np->conq) > 0 ? NS_WRES_CIND : NS_IDLE);
			bufq_unlock(&np->conq, pl);
		}
		break;
	case NS_WACK_RRES:
		err = np_reset_rem(np, N_USER, N_REASON_UNDEFINED);
		if (unlikely(err != 0))
			goto free_error;
		pl = bufq_lock(&np->resq);
		np_set_state(np, bufq_length(&np->resq) > 0 ? NS_WRES_RIND : NS_DATA_XFER);
		bufq_unlock(&np->resq, pl);
		break;
	case NS_WACK_DREQ6:
	case NS_WACK_DREQ7:
	case NS_WACK_DREQ9:
	case NS_WACK_DREQ10:
	case NS_WACK_DREQ11:
		err = np_disconnect(np, ADDR_buffer, SEQ_number, flags, dp);
		if (unlikely(err != QR_ABSORBED))
			goto free_error;
		pl = bufq_lock(&np->conq);
		np_set_state(np, bufq_length(&np->conq) > 0 ? NS_WRES_CIND : NS_IDLE);
		bufq_unlock(&np->conq, pl);
		break;
	default:
		/* Note: if we are not in a WACK state we simply do not change state.  This occurs normally
		   when we are responding to a N_OPTMGMT_REQ in other than the NS_IDLE state. */
		if (CORRECT_prim == N_OPTMGMT_REQ) {
			err = np_optmgmt(np, QOS_buffer, flags);
			if (unlikely(err != 0))
				goto free_error;
			break;
		}
		break;
	}
	_printd(("%s: %p: <- N_OK_ACK\n", DRV_NAME, np));
	qreply(q, mp);
	return (err);
      free_error:
	freemsg(mp);
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}

/**
 * np_conn_con: perform N_CONN_CON actions
 * @q: active queue in queue pair (write queue)
 * @RES_buffer: responding addresses
 * @RES_length: length of responding addresses
 * @QOS_buffer: connected quality of service
 * @CONN_flags: connected connection flags
 *
 * The NPI-RTP driver supports a pseudo-connection-oriented mode.  The responding address and
 * quality-of-service parameters returned in the N_CONN_CON do not represent a connection
 * establishment or negotiation with the remote NS provider, but are simply formed locally and
 * negotiated locally.  The N_CONN_CON is synthesized from the N_CONN_REQ and is returned
 * automatically and immediately in response to the N_CONN_REQ.
 *
 * Unlikely TPI, NPI does not return an N_OK_ACK in response to a N_CONN_REQ and does not have a
 * TS_WACK_CREQ state, only a NS_WCON_CREQ state.  This makes NPI more ammedable to this kind of
 * pseudo-connection-oriented mode.
 *
 * Unlikey the NPI-UDP driver, the NPI-RTP driver does not support multihoming.  However, when
 * connections are autobound (a null address was specified on N_BIND_REQ and one is only selected on
 * N_CONN_REQ), it would be convenient if the NS-provider would return the bound address so that the
 * NS-user does not have to issue a N_INFO_REQ to obtain the autobound value.  Therefore, the
 * primitive always provides an N_QOS_SEL_CONN_RTP structure and the structure always contains the
 * bound address.
 */
STATIC INLINE fastcall int
np_conn_con(queue_t *q, struct sockaddr_in *RES_buffer, socklen_t RES_length,
	    struct N_qos_sel_conn_rtp *QOS_buffer, np_ulong CONN_flags)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp = NULL;
	N_conn_con_t *p;
	int err;
	size_t QOS_length = sizeof(*QOS_buffer);
	const size_t plen = sizeof(*p);
	const size_t mlen = sizeof(*p) + RES_length + QOS_length;
	static const int band = 2;

	/* The M_PROTO is set to band 2 to keep expedited data on the same network connection from overtaking 
	   the connection confirmation. */

	np_set_state(np, NS_WCON_CREQ);

	if (unlikely((mp = mi_allocb(q, mlen, BPRI_MED)) == NULL))
		goto enobufs;

	if (unlikely(!bcanputnext(np->oq, band)))
		goto ebusy;

	err = np_connect(np, RES_buffer, RES_length, QOS_buffer, CONN_flags);
	if (unlikely(err != 0))
		goto free_error;

	DB_TYPE(mp) = M_PROTO;
	mp->b_band = band;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_CONN_CON;
	p->RES_length = RES_length;
	p->RES_offset = plen;
	p->CONN_flags = CONN_flags;
	p->QOS_length = QOS_length;
	p->QOS_offset = p->RES_offset + RES_length;
	mp->b_wptr += sizeof(*p);

	bcopy(RES_buffer, mp->b_wptr, RES_length);
	mp->b_wptr += RES_length;
	bcopy(QOS_buffer, mp->b_wptr, QOS_length);
	mp->b_wptr += QOS_length;

	mp->b_cont = dp;

	np_set_state(np, NS_DATA_XFER);

	if (msg) {
		if (msg == dp)
			msg = NULL;
		else if (msg->b_cont == dp)
			msg->b_cont = NULL;
	}
	freemsg(msg);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_CONN_CON");
	putnext(np->oq, mp);
	return (0);

      ebusy:
	err = -EBUSY;
	goto free_error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      free_error:
	freeb(mp);
      error:
	return (err);

}

/**
 * np_reset_con - generate a N_RESET_CON message
 * @q: active queue in queue pair (write queue)
 * @RESET_orig: origin of the reset
 * @RESET_reason: reason for the reset
 * @dp: message containing IP packet
 *
 * An N_RESET_CON message is sent only when the reset completes successfully.
 */
STATIC fastcall int
np_reset_con(queue_t *q, np_ulong RESET_orig, np_ulong RESET_reason, mblk_t *dp)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp = NULL;
	N_reset_con_t *p;
	size_t size = sizeof(*p);
	int err;
	pl_t pl;

	if (unlikely((mp = mi_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely((err = np_reset_loc(np, RESET_orig, RESET_reason, dp)) != 0))
		goto free_error;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_CON;
	mp->b_wptr += sizeof(*p);
	pl = bufq_lock(&np->resq);
	np_set_state(np, bufq_length(&np->resq) > 0 ? NS_WRES_RIND : NS_DATA_XFER);
	bufq_unlock(&np->resq, pl);
	_printd(("%s: <- N_RESET_CON\n", DRV_NAME));
	qreply(q, mp);
	return (QR_DONE);

      free_error:
	freeb(mp);
	goto error;
      enobufs:
	err = -ENOBUFS;
	goto error;
      error:
	return (err);
}

/**
 * np_conn_ind - generate a N_CONN_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 *
 * We generate connection indications to Streams that are bound as listening to an address including
 * the destination address of the IP packet, where no connection exists for the source address of
 * the IP packet.
 */
STATIC INLINE fastcall __hot_get int
np_conn_ind(queue_t *q, mblk_t *SEQ_number)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp, *cp;
	N_conn_ind_t *p;
	struct sockaddr_in *DEST_buffer, *SRC_buffer;
	struct N_qos_sel_conn_rtp *QOS_buffer;
	np_ulong DEST_length, SRC_length, QOS_length;
	size_t size;
	struct iphdr *iph = (struct iphdr *) SEQ_number->b_rptr;
	struct udphdr *uh = (struct udphdr *) (SEQ_number->b_rptr + (iph->ihl << 2));
	unsigned long flags;

	assure(SEQ_number->b_wptr >= SEQ_number->b_rptr + sizeof(*iph));
	assure(SEQ_number->b_wptr >= SEQ_number->b_rptr + (iph->ihl << 2));

	if (unlikely(np_not_state(np, (NSF_IDLE | NSF_WRES_CIND | NSF_WACK_CRES))))
		goto discard;

	/* Make sure we don't already have a connection indication */
	spin_lock_str2(&np->conq.q_lock, flags);
	for (cp = bufq_head(&np->conq); cp; cp = cp->b_next) {
		struct iphdr *iph2 = (struct iphdr *) cp->b_rptr;

		if (iph->protocol == iph2->protocol && iph->saddr == iph2->saddr && iph->daddr == iph2->daddr) {
			/* already have a connection indication, link the data */
			linkb(cp, SEQ_number);
			spin_unlock_str2(&np->conq.q_lock, flags);
			goto absorbed;
		}
	}
	spin_unlock_str2(&np->conq.q_lock, flags);

	if (unlikely(bufq_length(&np->conq) >= np->CONIND_number))
		/* If there are already too many connection indications outstanding, discard further
		   connection indications until some are accepted -- we might get fancy later and queue it
		   anyway.  Note that data for existing outstanding connection indications is preserved
		   above. */
		goto eagain;
	if (unlikely(np_not_state(np, (NSF_IDLE | NSF_WRES_CIND))))
		/* If there is already a connection accepted on the listening stream, discard further
		   connection indications until the current connection disconnects */
		goto eagain;

	np_set_state(np, NS_WRES_CIND);

	if (unlikely((cp = mi_dupmsg(q, SEQ_number)) == NULL))
		goto enobufs;

	DEST_length = sizeof(*DEST_buffer);
	SRC_length = sizeof(*SRC_buffer);
	QOS_length = sizeof(*QOS_buffer);
	size = sizeof(*p) + DEST_length + SRC_length + QOS_length;

	if (unlikely(!(mp = mi_allocb(q, size, BPRI_MED))))
		goto free_enobufs;

	if (unlikely(!canputnext(q)))
		goto ebusy;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 0;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += sizeof(*p);
	p->PRIM_type = N_CONN_IND;
	p->DEST_length = DEST_length;
	p->DEST_offset = DEST_length ? sizeof(*p) : 0;
	p->SRC_length = SRC_length;
	p->SRC_offset = SRC_length ? sizeof(*p) + DEST_length : 0;
	p->SEQ_number = (np_ulong) (long) SEQ_number;
	p->CONN_flags = 0;
	p->QOS_length = QOS_length;
	p->QOS_offset = QOS_length ? sizeof(*p) + DEST_length + SRC_length : 0;
	if (DEST_length) {
		DEST_buffer = (struct sockaddr_in *) mp->b_wptr;
		DEST_buffer->sin_family = AF_INET;
		DEST_buffer->sin_port = (np->bport != 0) ? uh->dest : 0;
		DEST_buffer->sin_addr.s_addr = iph->daddr;
		mp->b_wptr += sizeof(struct sockaddr_in);
	}
	if (SRC_length) {
		SRC_buffer = (struct sockaddr_in *) mp->b_wptr;
		SRC_buffer->sin_family = AF_INET;
		SRC_buffer->sin_port = (np->bport != 0) ? uh->source : 0;
		SRC_buffer->sin_addr.s_addr = iph->saddr;
		mp->b_wptr += sizeof(struct sockaddr_in);
	}
	if (QOS_length) {
		QOS_buffer = (struct N_qos_sel_conn_rtp *) mp->b_wptr;
		QOS_buffer->n_qos_type = N_QOS_SEL_CONN_RTP;
		/* FIXME: might be a problem here on 2.4 where we steal the packet by overwritting the
		   protocol id. */
		QOS_buffer->protocol = iph->protocol;
		QOS_buffer->priority = cp->b_band;
		QOS_buffer->ttl = iph->ttl;
		QOS_buffer->tos = iph->tos;
		QOS_buffer->mtu = QOS_UNKNOWN;	/* FIXME: determine route and get mtu from it */
		QOS_buffer->daddr = iph->daddr;
		QOS_buffer->saddr = iph->saddr;
		mp->b_wptr += QOS_length;
	}
	/* should we trim off the IP header and UDP header? */
	mp->b_cont = cp;
	/* sure, all the info is in the qos structure and addresses */
	adjmsg(cp, (iph->ihl << 2) + sizeof(struct udphdr));
	/* save original in connection indication list */
	bufq_queue(&np->conq, SEQ_number);
	_printd(("%s: %p: <- N_CONN_IND\n", DRV_NAME, np));
	putnext(q, mp);
      absorbed:
	return (QR_ABSORBED);

      ebusy:
	freeb(cp);
	freeb(mp);
	return (-EBUSY);
      free_enobufs:
	freemsg(cp);
      enobufs:
	return (-ENOBUFS);
      eagain:
	return (-EAGAIN);
      discard:
	return (QR_DONE);
}

/**
 * np_discon_ind - NE_DISCON_IND NC disconnected event
 * @q: active queue (read queue)
 * @dp: message containing ICMP packet
 *
 * The N_DISCON_IND is sent when we encounter an error on a connection oriented Stream, i.e. as a
 * result of receiving an ICMP error.  For multihomed hosts, we only do this if all destination
 * addresses have errors, otherwise, we just perform a reset for the affected destination.
 */
STATIC INLINE fastcall int
np_discon_ind(queue_t *q, struct sockaddr_in *RES_buffer, socklen_t RES_length, np_ulong RESERVED_field,
	      np_ulong DISCON_orig, np_ulong DISCON_reason, mblk_t *SEQ_number, mblk_t *dp)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp;
	N_discon_ind_t *p;
	size_t size = sizeof(*p) + RES_length;

	if (unlikely(np_not_state(np, (NSF_WRES_CIND | NSF_DATA_XFER | NSF_WRES_RIND | NSF_WCON_RREQ))))
		goto discard;

	if (unlikely((mp = mi_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 2;		/* expedite */
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DISCON_IND;
	p->DISCON_orig = DISCON_orig;
	p->DISCON_reason = DISCON_reason;
	p->RES_length = RES_length;
	p->RES_offset = RES_length ? sizeof(*p) : 0;
	p->SEQ_number = (np_ulong) (long) SEQ_number;
	mp->b_wptr += sizeof(*p);
	if (RES_length) {
		bcopy(RES_buffer, mp->b_wptr, RES_length);
		mp->b_wptr += RES_length;
	}
	mp->b_cont = dp;
	_printd(("%s: %p: <- N_DISCON_IND\n", DRV_NAME, np));
	putnext(q, mp);
	return (QR_ABSORBED);

      enobufs:
	return (-ENOBUFS);
      discard:
	return (QR_DONE);
}

/**
 * np_discon_ind_icmp - NE_DISCON_IND event resulting from ICMP message
 * @q: active queue in queue pair
 * @mp: the ICMP message
 */
STATIC INLINE fastcall int
np_discon_ind_icmp(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	struct iphdr *iph;
	struct icmphdr *icmp;
	struct udphdr *uh;
	struct sockaddr_in res_buf, *RES_buffer = &res_buf;
	np_ulong DISCON_reason;
	np_ulong RESERVED_field, DISCON_orig;
	mblk_t *rp;
	mblk_t *cp, *SEQ_number;
	ptrdiff_t hidden;
	int err;

	iph = (struct iphdr *) mp->b_rptr;	/* this is the ICMP message IP header */
	icmp = (struct icmphdr *) (mp->b_rptr + (iph->ihl << 2));
	iph = (struct iphdr *) (icmp + 1);	/* this is the encapsulated IP header */
	uh = (struct udphdr *) ((unsigned char *) iph + (iph->ihl << 2));
	if (mp->b_wptr < (unsigned char *) (uh + 1))
		uh = NULL;	/* don't have a full transport header */

	RES_buffer->sin_family = AF_INET;
	RES_buffer->sin_port = uh ? uh->source : 0;
	RES_buffer->sin_addr.s_addr = iph->saddr;

	switch (icmp->type) {
	case ICMP_DEST_UNREACH:
		switch (icmp->code) {
		case ICMP_NET_UNREACH:
		case ICMP_HOST_UNREACH:
		case ICMP_PROT_UNREACH:
		case ICMP_PORT_UNREACH:
			DISCON_orig = N_PROVIDER;
			DISCON_reason = N_REJ_NSAP_UNREACH_P;	// N_UD_ROUTE_UNAVAIL;
			RESERVED_field = 0;
			break;
		case ICMP_FRAG_NEEDED:
			DISCON_orig = N_PROVIDER;
			DISCON_reason = N_REJ_QOS_UNAVAIL_P;	// N_UD_SEG_REQUIRED;
			RESERVED_field = icmp->un.frag.mtu;
			break;
		case ICMP_NET_UNKNOWN:
		case ICMP_HOST_UNKNOWN:
		case ICMP_HOST_ISOLATED:
		case ICMP_NET_ANO:
		case ICMP_HOST_ANO:
		case ICMP_PKT_FILTERED:
		case ICMP_PREC_VIOLATION:
		case ICMP_PREC_CUTOFF:
			DISCON_orig = N_PROVIDER;
			DISCON_reason = N_REJ_NSAP_UNKNOWN;	// N_UD_ROUTE_UNAVAIL;
			RESERVED_field = 0;
			break;
		case ICMP_SR_FAILED:
		case ICMP_NET_UNR_TOS:
		case ICMP_HOST_UNR_TOS:
			DISCON_orig = N_PROVIDER;
			DISCON_reason = N_REJ_QOS_UNAVAIL_P;	// N_UD_QOS_UNAVAIL;
			RESERVED_field = 0;
			break;
		default:
			DISCON_orig = N_UNDEFINED;
			DISCON_reason = N_REASON_UNDEFINED;	// N_UD_UNDEFINED;
			RESERVED_field = 0;
			break;
		}
		break;
	case ICMP_SOURCE_QUENCH:
		/* Should not cause disconnect. */
		DISCON_orig = N_PROVIDER;
		DISCON_reason = N_CONGESTION;	// N_UD_CONGESTION;
		RESERVED_field = 0;
		break;
	case ICMP_TIME_EXCEEDED:
		switch (icmp->code) {
		case ICMP_EXC_TTL:
			DISCON_orig = N_PROVIDER;
			DISCON_reason = N_REJ_QOS_UNAVAIL_P;	// N_UD_LIFE_EXCEEDED;
			RESERVED_field = 0;
			break;
		case ICMP_EXC_FRAGTIME:
			DISCON_orig = N_PROVIDER;
			DISCON_reason = N_REJ_QOS_UNAVAIL_P;	// N_UD_TD_EXCEEDED;
			RESERVED_field = 0;
			break;
		default:
			DISCON_orig = N_UNDEFINED;
			DISCON_reason = N_REASON_UNDEFINED;	// N_UD_UNDEFINED;
			RESERVED_field = 0;
			break;
		}
		break;
	case ICMP_PARAMETERPROB:
		DISCON_orig = N_UNDEFINED;
		DISCON_reason = N_REASON_UNDEFINED;	// N_UD_UNDEFINED;
		RESERVED_field = 0;
		break;
	default:
		DISCON_orig = N_UNDEFINED;
		DISCON_reason = N_REASON_UNDEFINED;	// N_UD_UNDEFINED;
		RESERVED_field = 0;
		break;
	}

	/* check for outstanding reset indications for responding address */
	for (rp = bufq_head(&np->resq); rp; rp = rp->b_next) {
		struct iphdr *iph2 = (struct iphdr *) rp->b_rptr;

		if (iph->protocol == iph2->protocol && iph->saddr == iph2->saddr && iph->daddr == iph2->daddr)
			break;
	}

	/* check for outstanding connection indications for responding address */
	for (cp = bufq_head(&np->conq); cp; cp = cp->b_next) {
		struct iphdr *iph2 = (struct iphdr *) cp->b_rptr;

		if (iph->protocol == iph2->protocol && iph->saddr == iph2->saddr && iph->daddr == iph2->daddr)
			break;
	}
	SEQ_number = cp;

	/* hide ICMP header */
	hidden = (unsigned char *) iph - mp->b_rptr;
	mp->b_rptr = (unsigned char *) iph;
	if ((err =
	     np_discon_ind(q, RES_buffer, sizeof(*RES_buffer), RESERVED_field, DISCON_orig, DISCON_reason,
			   SEQ_number, mp)) < 0)
		mp->b_rptr -= hidden;
	else {
		if (cp != NULL) {
			bufq_unlink(&np->conq, cp);
			freemsg(cp);
		}
		if (rp != NULL) {
			bufq_unlink(&np->conq, rp);
			freemsg(rp);
		}
	}
	return (err);
}

/**
 * np_data_ind - generate a N_DATA_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 *
 * Very fast.  In fact, we could just pass the raw M_DATA blocks upstream.  We leave the IP header
 * in the block.
 */
STATIC INLINE fastcall __hot_get int
np_data_ind(queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	N_data_ind_t *p;
	const size_t size = sizeof(*p);

	if (unlikely((mp = mi_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	if (unlikely(!canputnext(q)))
		goto ebusy;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATA_IND;
	/* TODO: here we can set some info like ECN... */
	p->DATA_xfer_flags = 0;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	dp->b_datap->db_type = M_DATA;	/* just in case */
	_printd(("%s: %p: <- N_DATA_IND\n", DRV_NAME, NP_PRIV(q)));
	putnext(q, mp);
	return (QR_ABSORBED);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * np_exdata_ind - generate a N_EXDATA_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 */
STATIC INLINE fastcall __hot_get int
np_exdata_ind(queue_t *q, mblk_t *dp)
{
	mblk_t *mp;
	N_exdata_ind_t *p;

	if (unlikely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!bcanputnext(q, 1)))
		goto ebusy;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_EXDATA_IND;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = dp;
	dp->b_datap->db_type = M_DATA;	/* just in case */
	_printd(("%s: %p: <- N_EXDATA_IND\n", DRV_NAME, NP_PRIV(q)));
	putnext(q, mp);
	return (QR_ABSORBED);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/**
 * np_unitdata_ind - generate a N_UNITDATA_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing IP packet
 *
 * Note that on unit data indications the entire IP packet is delivered in the data part of
 * the message.  If the NS user is willing to extract information from the IP header, it is
 * welcome to discard the control part.
 */
STATIC INLINE fastcall __hot_get int
np_unitdata_ind(queue_t *q, mblk_t *dp)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp;
	N_unitdata_ind_t *p;
	struct sockaddr_in *SRC_buffer, *DEST_buffer;
	const np_ulong SRC_length = sizeof(*SRC_buffer);
	const np_ulong DEST_length = sizeof(*DEST_buffer);
	size_t size = sizeof(*p) + SRC_length + DEST_length;
	struct iphdr *iph;
	struct udphdr *uh;

	if (unlikely(np_get_state(np) != NS_IDLE))
		goto discard;
	if (unlikely((mp = mi_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(q)))
		goto ebusy;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UNITDATA_IND;
	p->SRC_length = SRC_length;
	p->SRC_offset = SRC_length ? sizeof(*p) : 0;
	p->DEST_length = DEST_length;
	p->DEST_offset = DEST_length ? sizeof(*p) + SRC_length : 0;
	p->ERROR_type = 0;
	mp->b_wptr += sizeof(*p);

	iph = (struct iphdr *) dp->b_rptr;
	uh = (struct udphdr *) (dp->b_rptr + (iph->ihl << 2));

	if (SRC_length) {
		SRC_buffer = (struct sockaddr_in *) mp->b_wptr;
		SRC_buffer->sin_family = AF_INET;
		SRC_buffer->sin_port = uh->source;
		SRC_buffer->sin_addr.s_addr = iph->saddr;
		mp->b_wptr += SRC_length;
	}
	if (DEST_length) {
		DEST_buffer = (struct sockaddr_in *) mp->b_wptr;
		DEST_buffer->sin_family = AF_INET;
		DEST_buffer->sin_port = uh->dest;
		DEST_buffer->sin_addr.s_addr = iph->daddr;
		mp->b_wptr += DEST_length;
	}
	/* pull IP header */
	dp->b_rptr = (unsigned char *) uh;
	mp->b_cont = dp;
	dp->b_datap->db_type = M_DATA;	/* just in case */
	_printd(("%s: %p: <- N_UNITDATA_IND\n", DRV_NAME, np));
	putnext(q, mp);
	return (QR_ABSORBED);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
      discard:
	return (QR_DONE);
}

/**
 * np_uderror_ind - generate a N_UDERROR_IND message
 * @q: active queue in queue pair (read or write queue)
 * @DEST_buffer: pointer to destination address
 * @RESERVED_field: MTU if error is N_UD_SEG_REQUIRED
 * @ERROR_type: error number
 * @dp: message containing (part of) errored packet
 *
 * Note that in the special case of N_UD_SEG_REQUIRED, we use the RESERVED_field to indicate that
 * the value of the MTU is for the destination, gleened from the ICMP message.  This is a sneaky
 * trick, because the field must be coded zero according to spec, so the presence of a non-zero
 * value indicates that the MTU value is present for supporting providers.  Users that wish to
 * ignore this field must use N_INFO_REQ and examine the NSDU_size parameter to determine the
 * current value of the MTU.
 */
noinline __unlikely int
np_uderror_ind(queue_t *q, struct sockaddr_in *DEST_buffer, np_ulong RESERVED_field, np_ulong ERROR_type,
	       mblk_t *dp)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp;
	N_uderror_ind_t *p;
	np_ulong DEST_length = sizeof(*DEST_buffer);
	size_t size = sizeof(*p) + (DEST_buffer ? DEST_length : 0);

	if (unlikely(np_get_state(np) != NS_IDLE))
		goto discard;
	if (unlikely((mp = mi_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(np->oq)))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 2;		/* XXX move ahead of data indications */
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UDERROR_IND;
	p->DEST_length = DEST_buffer ? DEST_length : 0;
	p->DEST_offset = DEST_buffer ? sizeof(*p) : 0;
	p->RESERVED_field = RESERVED_field;
	p->ERROR_type = ERROR_type;
	mp->b_wptr += sizeof(*p);
	if (DEST_buffer) {
		bcopy(DEST_buffer, mp->b_wptr, DEST_length);
		mp->b_wptr += DEST_length;
	}
	if (dp != NULL) {
		mp->b_cont = dp;
		dp->b_datap->db_type = M_DATA;	/* was M_ERROR in some cases */
	}
	_printd(("%s: %p: <- N_UDERROR_IND\n", DRV_NAME, np));
	putnext(np->oq, mp);
	return (QR_ABSORBED);
      ebusy:
	freeb(mp);
	ptrace(("%s: ERROR: Flow controlled\n", DRV_NAME));
	return (-EBUSY);
      enobufs:
	ptrace(("%s: ERROR: No buffers\n", DRV_NAME));
	return (-ENOBUFS);
      discard:
	return (QR_DONE);
}

/**
 * np_uderror_ind_icmp - generate an N_UDERROR_IND message from an ICMP packet
 * @q: active queue in pair (read queue)
 * @mp: message containing ICMP packet
 *
 * There is another reason for issuing an N_UDERROR_IND and that is Explicit Congestion
 * Notification, but there is no ICMP message associated with that and it has not yet been coded:
 * probably need an ne_uderror_ind_ecn() function.
 *
 * Note that the special case of N_UD_SEG_REQUIRED, we use the RESERVED_field to indicate that the
 * value of the MTU is for the destination, gleened from the ICMP message.  This is a sneaky trick,
 * because the field must be coded zero according to NPI spec, so the presence of a non-zero value
 * indicates the MTU value from a supporting NPI provider.
 */
noinline fastcall __unlikely int
np_uderror_ind_icmp(queue_t *q, mblk_t *mp)
{
	struct iphdr *iph;
	struct icmphdr *icmp;
	struct udphdr *uh;
	struct sockaddr_in dst_buf, *DEST_buffer = &dst_buf;
	np_ulong ERROR_type;
	np_ulong RESERVED_field;
	unsigned char *hidden;
	int err;

	iph = (struct iphdr *) mp->b_rptr;	/* this is the ICMP message IP header */
	icmp = (struct icmphdr *) (mp->b_rptr + (iph->ihl << 2));
	iph = (struct iphdr *) (icmp + 1);	/* this is the encapsulated IP header */
	uh = (struct udphdr *) ((unsigned char *) iph + (iph->ihl << 2));
	if (mp->b_wptr < (unsigned char *) (uh + 1))
		uh = NULL;	/* don't have a full transport header */
	DEST_buffer->sin_family = AF_INET;
	DEST_buffer->sin_port = uh ? uh->source : 0;
	DEST_buffer->sin_addr.s_addr = iph->saddr;
	switch (icmp->type) {
	case ICMP_DEST_UNREACH:
		switch (icmp->code) {
		case ICMP_NET_UNREACH:
		case ICMP_HOST_UNREACH:
		case ICMP_PROT_UNREACH:
		case ICMP_PORT_UNREACH:
			ERROR_type = N_UD_ROUTE_UNAVAIL;
			RESERVED_field = 0;
			break;
		case ICMP_FRAG_NEEDED:
			ERROR_type = N_UD_SEG_REQUIRED;
			RESERVED_field = icmp->un.frag.mtu;
			break;
		case ICMP_NET_UNKNOWN:
		case ICMP_HOST_UNKNOWN:
		case ICMP_HOST_ISOLATED:
		case ICMP_NET_ANO:
		case ICMP_HOST_ANO:
		case ICMP_PKT_FILTERED:
		case ICMP_PREC_VIOLATION:
		case ICMP_PREC_CUTOFF:
			ERROR_type = N_UD_ROUTE_UNAVAIL;
			RESERVED_field = 0;
			break;
		case ICMP_SR_FAILED:
		case ICMP_NET_UNR_TOS:
		case ICMP_HOST_UNR_TOS:
			ERROR_type = N_UD_QOS_UNAVAIL;
			RESERVED_field = 0;
			break;
		default:
			ERROR_type = N_UD_UNDEFINED;
			RESERVED_field = 0;
			break;
		}
		break;
	case ICMP_SOURCE_QUENCH:
		ERROR_type = N_UD_CONGESTION;
		RESERVED_field = 0;
		break;
	case ICMP_TIME_EXCEEDED:
		switch (icmp->code) {
		case ICMP_EXC_TTL:
			ERROR_type = N_UD_LIFE_EXCEEDED;
			RESERVED_field = 0;
			break;
		case ICMP_EXC_FRAGTIME:
			ERROR_type = N_UD_TD_EXCEEDED;
			RESERVED_field = 0;
			break;
		default:
			ERROR_type = N_UD_UNDEFINED;
			RESERVED_field = 0;
			break;
		}
		break;
	case ICMP_PARAMETERPROB:
		ERROR_type = N_UD_UNDEFINED;
		RESERVED_field = 0;
		break;
	default:
		ERROR_type = N_UD_UNDEFINED;
		RESERVED_field = 0;
		break;
	}
	/* hide ICMP header */
	hidden = mp->b_rptr;
	mp->b_rptr = (unsigned char *) iph;
	if ((err = np_uderror_ind(q, DEST_buffer, RESERVED_field, ERROR_type, mp)) < 0)
		mp->b_rptr = hidden;
	return (err);
}

noinline fastcall __unlikely int
np_uderror_reply(queue_t *q, struct sockaddr_in *DEST_buffer, np_ulong RESERVED_field, np_long ERROR_type,
		 mblk_t *db)
{
	switch (ERROR_type) {
	case -EBUSY:
	case -EAGAIN:
	case -ENOMEM:
	case -ENOBUFS:
		return (ERROR_type);
	case 0:
		return (ERROR_type);
	case -EADDRINUSE:
		ERROR_type = NBOUND;
		break;
	case -EADDRNOTAVAIL:
	case -EDESTADDRREQ:
		ERROR_type = NNOADDR;
		break;
	case -EACCES:
		ERROR_type = NACCESS;
		break;
	case -EOPNOTSUPP:
		ERROR_type = NNOTSUPPORT;
		break;
	case NBADADDR:
	case NBADDATA:
	case NBADOPT:
	case NBADQOSTYPE:
		break;
	default:
	case NOUTSTATE:
	case -EINVAL:
	case -EFAULT:
	case -EMSGSIZE:
		return np_error_fatal(q, -EPROTO);
	}
	return np_uderror_ind(q, DEST_buffer, RESERVED_field, ERROR_type, db);
}

/**
 * np_reset_ind - generate a N_RESET_IND message
 * @q: active queue in queue pair (read queue)
 * @dp: message containing ICMP packet
 *
 * We generate an extended reset indication that includes the resetting address, and the qos
 * parameters associated with the reset.  Reset reasons are ICMP errors.  The reset originator is
 * always the NS provider.
 *
 * Another reason for sending a N_RESET_IND is Explicity Congestion Notification (N_UD_CONGESTION)
 * but not as a result of an ICMP message.  This has yet to be coded.
 */
STATIC int
np_reset_ind(queue_t *q, mblk_t *dp)
{
	struct np *np = NP_PRIV(q);
	mblk_t *mp, *bp;
	N_reset_ind_t *p;
	const size_t size = sizeof(*p);
	struct iphdr *iph = (struct iphdr *) dp->b_rptr;
	struct icmphdr *icmp = (struct icmphdr *) (dp->b_rptr + (iph->ihl << 2));

	assure(dp->b_wptr >= dp->b_rptr + sizeof(*iph));
	assure(dp->b_wptr >= dp->b_rptr + (iph->ihl << 2));

	/* Make sure we don't already have a reset indication */
	for (bp = bufq_head(&np->resq); bp; bp = bp->b_next) {
		struct iphdr *iph2 = (struct iphdr *) bp->b_rptr;
		struct icmphdr *icmp2 = (struct icmphdr *) (bp->b_rptr + (iph2->ihl << 2));

		if (iph->protocol == iph2->protocol && iph->saddr == iph2->saddr && icmp->type == icmp2->type
		    && icmp->code == icmp2->code)
			/* duplicate, just discard it */
			goto discard;
	}

	if (unlikely((bp = mi_dupmsg(q, dp)) == NULL))
		goto enobufs;
	if (unlikely((mp = mi_allocb(q, size, BPRI_MED)) == NULL))
		goto enobufs;

	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 2;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_IND;
	p->RESET_orig = N_PROVIDER;
	mp->b_wptr += sizeof(*p);
	switch (icmp->type) {
	case ICMP_DEST_UNREACH:
		switch (icmp->code) {
		case ICMP_NET_UNREACH:
		case ICMP_HOST_UNREACH:
		case ICMP_PROT_UNREACH:
		case ICMP_PORT_UNREACH:
			p->RESET_reason = N_UD_ROUTE_UNAVAIL;
			break;
		case ICMP_FRAG_NEEDED:
			/* If the reason was fragmentation needed, then we sent a packet that was too large
			   and so we might need to adjust down our NSDU_size as well as the np->qos.mtu that
			   is being reported for the Stream.  When the user receives this error, it is their
			   responsibility to check sizes again with N_INFO_REQ. */
			if (np->qos.mtu > icmp->un.frag.mtu)
				np->qos.mtu = icmp->un.frag.mtu;
			if (np->info.NIDU_size + sizeof(struct iphdr) > np->qos.mtu)
				np->info.NIDU_size = np->qos.mtu - sizeof(struct iphdr);
			if (np->info.NSDU_size + sizeof(struct iphdr) > np->qos.mtu)
				np->info.NSDU_size = np->qos.mtu - sizeof(struct iphdr);
			if (np->info.ENSDU_size + sizeof(struct iphdr) > np->qos.mtu)
				np->info.ENSDU_size = np->qos.mtu - sizeof(struct iphdr);
			if (np->info.CDATA_size + sizeof(struct iphdr) > np->qos.mtu)
				np->info.CDATA_size = np->qos.mtu - sizeof(struct iphdr);
			if (np->info.DDATA_size + sizeof(struct iphdr) > np->qos.mtu)
				np->info.DDATA_size = np->qos.mtu - sizeof(struct iphdr);
			p->RESET_reason = N_UD_SEG_REQUIRED;
			break;
		case ICMP_NET_UNKNOWN:
		case ICMP_HOST_UNKNOWN:
		case ICMP_HOST_ISOLATED:
		case ICMP_NET_ANO:
		case ICMP_HOST_ANO:
		case ICMP_PKT_FILTERED:
		case ICMP_PREC_VIOLATION:
		case ICMP_PREC_CUTOFF:
			p->RESET_reason = N_UD_ROUTE_UNAVAIL;
			break;
		case ICMP_SR_FAILED:
		case ICMP_NET_UNR_TOS:
		case ICMP_HOST_UNR_TOS:
			p->RESET_reason = N_UD_QOS_UNAVAIL;
			break;
		default:
			p->RESET_reason = N_UD_UNDEFINED;
			break;
		}
		break;
	case ICMP_SOURCE_QUENCH:
		p->RESET_reason = N_UD_CONGESTION;
		break;
	case ICMP_TIME_EXCEEDED:
		switch (icmp->code) {
		case ICMP_EXC_TTL:
			p->RESET_reason = N_UD_LIFE_EXCEEDED;
			break;
		case ICMP_EXC_FRAGTIME:
			p->RESET_reason = N_UD_TD_EXCEEDED;
			break;
		default:
			p->RESET_reason = N_UD_UNDEFINED;
			break;
		}
		break;
	case ICMP_PARAMETERPROB:
		p->RESET_reason = N_UD_UNDEFINED;
		break;
	default:
		p->RESET_reason = N_UD_UNDEFINED;
		break;
	}
	/* save original in reset indication list */
	bufq_queue(&np->resq, dp);
	_printd(("%s: <- N_RESET_IND\n", DRV_NAME));
	putnext(q, mp);
      discard:
	return (QR_DONE);
      enobufs:
	if (bp)
		freemsg(bp);
	return (-ENOBUFS);
}

/**
 * np_datack_ind - NE_DATACK_IND event
 * @q: active queue in pair (read queue)
 */
STATIC INLINE fastcall int
np_datack_ind(queue_t *q)
{
	mblk_t *mp;
	N_datack_ind_t *p;

	if (unlikely((mp = mi_allocb(q, sizeof(*p), BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(q)))
		goto ebusy;

	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATACK_IND;
	mp->b_wptr += sizeof(*p);

	_printd(("%s: <- N_DATACK_IND\n", DRV_NAME));
	putnext(q, mp);
	return (QR_DONE);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  ===================================================================
 *
 *  NS-Provider -> NS-User Multi-Stream primitives
 *
 *  ===================================================================
 */

/** nc_conn_ind - generate an N_CONN_IND and pass it upstream
  * @q: active queue in queue pair
  * @msg: message to free upon success
  * @np: private structure (locked)
  * @nc: network connection
  * @SEQ_number: connection indication sequence number
  * @CONN_flags: connection flags
  * @DEST_buffer: destination address pointer
  * @DEST_length: destination address length
  * @SRC_buffer: source address pointer
  * @SRC_length: source address length
  * @QOS_buffer: quality of service parameters pointer
  * @QOS_length: quality of service parameters length
  * @dp: connection data
  */
STATIC int
nc_conn_ind(queue_t *q, mblk_t *msg, struct np *np, struct nc *nc, mblk_t *SEQ_number, np_ulong CONN_flags,
	    caddr_t DEST_buffer, size_t DEST_length, caddr_t SRC_buffer, size_t SRC_length,
	    caddr_t QOS_buffer, size_t QOS_length, mblk_t *dp)
{
	N_conn_ind_t *p;
	mblk_t *mp;
	const size_t plen = sizeof(*p) + (nc ? sizeof(nc) : 0);
	const size_t mlen = plen + DEST_length + SRC_length + QOS_length;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	if (unlikely(!canputnext(np->oq)))
		goto ebusy;

	if (likely(nc != NULL)) {
		DB_TYPE(mp) = M_CTL;

		*(typeof(nc->tag) *) mp->b_wptr = nc->tag;
		mp->b_wptr += sizeof(nc->tag);
	} else {
		DB_TYPE(mp) = M_PROTO;
	}

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_CONN_IND;
	p->DEST_offset = plen;
	p->DEST_length = DEST_length;
	p->SRC_offset = p->DEST_offset + DEST_length;
	p->SRC_length = SRC_length;
	p->SEQ_number = (np_ulong) (long) SEQ_number;
	p->CONN_flags = CONN_flags;
	p->QOS_offset = p->SRC_offset + SRC_length;
	p->QOS_length = QOS_length;
	mp->b_wptr += sizeof(*p);

	bcopy(DEST_buffer, mp->b_wptr, DEST_length);
	mp->b_wptr += DEST_length;
	bcopy(SRC_buffer, mp->b_wptr, SRC_length);
	mp->b_wptr += SRC_length;
	bcopy(QOS_buffer, mp->b_wptr, QOS_length);
	mp->b_wptr += QOS_length;

	mp->b_cont = dp;

	nc_set_state(np->oq, np, nc, NS_WRES_CIND);

	if (msg) {
		if (msg == dp)
			msg = NULL;
		else if (msg->b_cont == dp)
			msg->b_cont = NULL;
	}
	freemsg(msg);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_CONN_IND");
	putnext(np->oq, mp);
	return (0);
      ebusy:
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** nc_conn_con - generate an N_CONN_CON and pass it upstream
  * @q: active queue in queue pair
  * @msg: message to free upon success
  * @np: private structure (locked)
  * @nc: network connection
  * @CONN_flags: connection flags
  * @RES_buffer: responding address pointer
  * @RES_length: responding address length
  * @QOS_buffer: quality of service parameters pointer
  * @QOS_length: quality of service parameters length
  * @dp: connection data
  *
  */
STATIC int
nc_conn_con(queue_t *q, mblk_t *msg, struct np *np, struct nc *nc, np_ulong CONN_flags, caddr_t RES_buffer,
	    size_t RES_length, caddr_t QOS_buffer, size_t QOS_length, mblk_t *dp)
{
	N_conn_con_t *p;
	mblk_t *mp;
	const size_t plen = sizeof(nc) + sizeof(*p);
	const size_t mlen = plen + RES_length + QOS_length;
	static const int band = 2;

	/* The M_CTL/M_PROTO is set to band 2 to keep expedited data on the same network connection from
	   overtaking the connection confirmation. */

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	if (unlikely(!bcanputnext(np->oq, band)))
		goto ebusy;

	DB_TYPE(mp) = M_CTL;
	mp->b_band = band;

	*(typeof(nc->tag) *) mp->b_wptr = nc->tag;
	mp->b_wptr += sizeof(nc->tag);

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_CONN_CON;
	p->RES_offset = plen;
	p->RES_length = RES_length;
	p->CONN_flags = CONN_flags;
	p->QOS_offset = p->RES_offset + RES_length;
	p->QOS_length = QOS_length;
	mp->b_wptr += sizeof(*p);

	bcopy(RES_buffer, mp->b_wptr, RES_length);
	mp->b_wptr += RES_length;
	bcopy(QOS_buffer, mp->b_wptr, QOS_length);
	mp->b_wptr += QOS_length;

	mp->b_cont = dp;

	nc_set_state(np->oq, np, nc, NS_DATA_XFER);

	if (msg) {
		if (msg == dp)
			msg = NULL;
		else if (msg->b_cont == dp)
			msg->b_cont = NULL;
	}
	freemsg(msg);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_CONN_CON");
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/** nc_discon_ind - generate an N_DISCON_IND and pass it upstream
  * @q: active queue in queue pair
  * @msg: message to free upon success
  * @np: private structure (locked)
  * @nc: network connection
  * @DISCON_orig: disconnection origin
  * @DISCON_reason: disconnection reason
  * @RES_buffer: responding address pointer
  * @RES_length: responding address length
  * @SEQ_number: sequence number
  * @dp: data
  *
  * The N_DISCON_IND can be sent when we encounter an error on a connection oriented virtual
  * interface, i.e. as a result of receiving an ICMP error.  For multihomed hosts we only do this if
  * all destination addresses have errors, otherwise, we just perform a reset for the affected
  * destination.  Also, in the case of RTP/RTCP we always want the reset behaviour rather than the
  * disconnect behaviour.
  */
STATIC int
nc_discon_ind(queue_t *q, mblk_t *msg, struct np *np, struct nc *nc, np_ulong DISCON_orig,
	      np_ulong DISCON_reason, caddr_t RES_buffer, size_t RES_length, mblk_t *SEQ_number, mblk_t *dp)
{
	N_discon_ind_t *p;
	mblk_t *mp;
	const size_t plen = sizeof(*p) + (nc ? sizeof(nc) : 0);
	const size_t mlen = plen + RES_length;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	if (unlikely(!bcanputnext(np->oq, 2)))
		goto ebusy;

	if (likely(nc != NULL)) {
		DB_TYPE(mp) = M_CTL;
		mp->b_band = 2;	/* expedite */

		*(typeof(nc->tag) *) mp->b_wptr = nc->tag;
		mp->b_wptr += sizeof(nc->tag);
	} else {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 2;	/* expedite */
	}

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DISCON_IND;
	p->DISCON_orig = DISCON_orig;
	p->DISCON_reason = DISCON_reason;
	p->RES_offset = plen;
	p->RES_length = RES_length;
	p->SEQ_number = (np_ulong) (long) SEQ_number;
	mp->b_wptr += sizeof(*p);

	bcopy(RES_buffer, mp->b_wptr, RES_length);
	mp->b_wptr += RES_length;

	mp->b_cont = dp;

	nc_set_state(np->oq, np, nc, NS_IDLE);

	if (msg) {
		if (msg == dp)
			msg = NULL;
		if (msg->b_cont = dp)
			msg->b_cont = NULL;
	}
	freemsg(msg);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_DISCON_IND");
	putnext(np->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** nc_data_ind - generate an N_DATA_IND and pass it upstream
  * @q: active queue in queue pair
  * @msg: message to free upon success
  * @np: private structure (locked)
  * @nc: network connection
  * @dp: the data
  */
STATIC INLINE fastcall __hot_get int
nc_data_ind(queue_t *q, mblk_t *msg, struct np *np, struct nc *nc, mblk_t *dp)
{
	N_data_ind_t *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + (nc ? sizeof(nc) : 0);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	if (unlikely(!bcanputnext(np->oq, dp->b_band)))
		goto ebusy;

	if (likely(nc != NULL)) {
		DB_TYPE(mp) = M_CTL;
		mp->b_band = dp->b_band;

		*(typeof(nc->tag) *) mp->b_wptr = nc->tag;
		mp->b_wptr += sizeof(nc->tag);
	} else {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = dp->b_band;
	}

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATA_IND;
	p->DATA_xfer_flags = 0;
	mp->b_wptr += sizeof(*p);

	mp->b_cont = dp;

	if (msg) {
		if (msg == dp)
			msg = NULL;
		else if (msg->b_cont == dp)
			msg->b_cont = NULL;
	}
	freemsg(msg);
#ifndef _OPTIMIZE_SPEED
	mi_strlog(np->oq, STRLOGDA, SL_TRACE, "<- N_DATA_IND");
#endif
	putnext(np->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** nc_exdata_ind - generate an N_EXDATA_IND and pass it upstream
  * @q: active queue in queue pair
  * @msg: message to free upon success
  * @np: private structure (locked)
  * @nc: network connection
  * @dp: the data
  */
STATIC INLINE fastcall __hot_get int
nc_exdata_ind(queue_t *q, mblk_t *msg, struct np *np, struct nc *nc, mblk_t *dp)
{
	N_exdata_ind_t *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + (nc ? sizeof(nc) : 0);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	if (unlikely(!bcanputnext(np->oq, dp->b_band)))
		goto ebusy;

	if (likely(nc != NULL)) {
		DB_TYPE(mp) = M_CTL;
		mp->b_band = dp->b_band;

		*(typeof(nc->tag) *) mp->b_wptr = nc->tag;
		mp->b_wptr += sizeof(nc->tag);
	} else {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = dp->b_band;
	}

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_EXDATA_IND;
	mp->b_wptr += sizeof(*p);

	mp->b_cont = dp;

	if (msg) {
		if (msg == dp)
			msg = NULL;
		else if (msg->b_cont == dp)
			msg->b_cont = NULL;
	}
	freemsg(msg);
#ifndef _OPTIMIZE_SPEED
	mi_strlog(np->oq, STRLOGDA, SL_TRACE, "<- N_EXDATA_IND");
#endif				/* _OPTIMIZE_SPEED */
	putnext(np->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** nc_unitdata_ind - generate an N_UNITDATA_IND and pass it upstream
  * @q: active queue in queue pair
  * @msg: message to free upon success
  * @np: private structure (locked)
  * @nc: network connection
  * @SRC_buffer: source address pointer
  * @SRC_length: source address length
  * @DEST_buffer: destination address pointer
  * @DEST_length: destination address length
  * @dp: the data
  *
  * Note that on unit data indications the entire IP packet is delivered in the data part of the
  * message.  If the NS user is willing to extract information from the IP header, it is welcome to
  * discard the control part.
  */
STATIC int
nc_unitdata_ind(queue_t *q, mblk_t *msg, struct np *np, struct nc *nc, caddr_t SRC_buffer, size_t SRC_length,
		caddr_t DEST_buffer, size_t DEST_length, mblk_t *dp)
{
	N_unitdata_ind_t *p;
	mblk_t *mp;
	const size_t plen = sizeof(*p) + (nc ? sizeof(nc) : 0);
	const size_t mlen = plen + SRC_length + DEST_length;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	if (unlikely(!bcanputnext(np->oq, dp->b_band)))
		goto ebusy;

	if (likely(nc != NULL)) {
		DB_TYPE(mp) = M_CTL;
		mp->b_band = dp->b_band;

		*(typeof(nc->tag) *) mp->b_wptr = nc->tag;
		mp->b_wptr += sizeof(nc->tag);
	} else {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = dp->b_band;
	}

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UNITDATA_IND;
	p->SRC_offset = plen;
	p->SRC_length = SRC_length;
	p->DEST_offset = p->SRC_offset + SRC_length;
	p->DEST_length = DEST_length;
	p->ERROR_type = 0;
	mp->b_wptr += sizeof(*p);

	mp->b_cont = dp;

	if (msg) {
		if (msg == dp)
			msg = NULL;
		else if (msg->b_cont = dp)
			msg->b_cont = NULL;
	}
	freemsg(msg);
#ifndef _OPTIMIZE_SPEED
	mi_strlog(np->oq, STRLOGDA, SL_TRACE, "<- N_UNITDATA_IND");
#endif				/* _OPTIMIZE_SPEED */
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/** nc_info_ack - generate an N_INFO_ACK and pass it upstream
  * @q: active queue in queue pair
  * @msg: message to free upon success
  * @np: private structure (locked)
  * @nc: network connection
  * @ADDR_buffer: address pointer
  * @ADDR_length: address length
  * @QOS_buffer: quality of service parameter pointer
  * @QOS_length: quality of service parameter length
  * @QOS_range_buffer: quality of service parameter range pointer
  * @QOS_range_length: quality of service parameter range length
  * @PROTOID_buffer: protocol identifiers pointer
  * @PROTOID_length: protocol identifiers length
  */
STATIC int
nc_info_ack(queue_t *q, mblk_t *msg, struct np *np, struct nc *nc, caddr_t ADDR_buffer, size_t ADDR_length,
	    caddr_t QOS_buffer, size_t QOS_length, caddr_t QOS_range_buffer, size_t QOS_range_length,
	    caddr_t PROTOID_buffer, size_t PROTOID_length)
{
	N_info_ack_t *p;
	mblk_t *mp;
	const size_t plen = sizeof(*p) + (nc ? sizeof(nc) : 0);
	const size_t mlen = plen + ADDR_length + QOS_length + QOS_range_length + PROTOID_length;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	if (likely(nc != NULL)) {
		DB_TYPE(mp) = M_PCCTL;

		*(typeof(nc->tag) *) mp->b_wptr = nc->tag;
		mp->b_wptr += sizeof(nc->tag);
	} else {
		DB_TYPE(mp) = M_PCPROTO;
	}

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_INFO_ACK;
	p->NSDU_size = np->info.NSDU_size;
	p->ENSDU_size = np->info.ENSDU_size;
	p->CDATA_size = np->info.CDATA_size;
	p->DDATA_size = np->info.DDATA_size;
	p->ADDR_size = np->info.ADDR_size;
	p->ADDR_length = ADDR_length;
	p->ADDR_offset = plen;
	p->QOS_length = QOS_length;
	p->QOS_offset = p->ADDR_offset + ADDR_length;
	p->QOS_range_length = QOS_range_length;
	p->QOS_range_offset = p->QOS_offset + QOS_length;
	p->OPTIONS_flags = np->info.OPTIONS_flags;
	p->NIDU_size = np->info.NIDU_size;
	p->SERV_type = np->info.SERV_type ? np->info.SERV_type : (N_CONS | N_CLNS);
	p->CURRENT_state = nc_get_state(np, nc);
	p->PROVIDER_type = np->info.PROVIDER_type;
	p->NODU_size = 536;
	p->PROTOID_length = PROTOID_length;
	p->PROTOID_offset = p->QOS_range_offset + QOS_range_length;
	p->NPI_version = np->info.NPI_version;
	mp->b_wptr += sizeof(*p);

	bcopy(ADDR_buffer, mp->b_wptr, ADDR_length);
	mp->b_wptr += ADDR_length;
	bcopy(QOS_buffer, mp->b_wptr, QOS_length);
	mp->b_wptr += QOS_length;
	bcopy(QOS_range_buffer, mp->b_wptr, QOS_range_length);
	mp->b_wptr += QOS_range_length;
	bcopy(PROTOID_buffer, mp->b_wptr, PROTOID_buffer);
	mp->b_wptr += PROTOID_length;

	freemsg(msg);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_INFO_ACK");
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/** nc_bind_ack - generate an N_BIND_ACK and pass it upstream
  * @q: active queue in queue pair
  * @msg: message to free upon success
  * @np: private structure (locked)
  * @nc: network connection
  * @SERV_type: service type
  * @CONIND_number: confirmed number of connection indications
  * @TOKEN_value: token value for stream
  * @ADDR_buffer: bound address pointer
  * @ADDR_length: bound address length
  * @PROTOID_buffer: protocol identifiers pointer
  * @PROTOID_length: protocol identifiers length
  */
STATIC int
nc_bind_ack(queue_t *q, mblk_t *msg, struct np *np, struct nc *nc, np_ulong SERV_type, np_ulong CONIND_number,
	    np_ulong BIND_flags, caddr_t ADDR_buffer, size_t ADDR_length, caddr_t PROTOID_buffer,
	    size_t PROTOID_length)
{
	N_bind_ack_t *p;
	mblk_t *mp;
	const size_t plen = sizeof(*p) + (nc ? 2 * sizeof(nc) : 0);
	const size_t mlen = plen + ADDR_length + PROTOID_length;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	if (likely(nc != NULL)) {
		if (unlikely
		    (!(nc =
		       nc_alloc(np, *(typeof(nc->tag) *) mp->b_wptr, SERV_type, CONIND_number, BIND_flags,
				ADDR_buffer, ADDR_length, PROTOID_buffer, PROTOID_length))))
			goto enomem;

		DB_TYPE(mp) = M_PCCTL;

		*(typeof(nc->tag) *) mp->b_wptr = nc->tag;
		mp->b_wptr += sizeof(nc->tag);
	} else {
		DB_TYPE(mp) = M_PCPROTO;
	}

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_BIND_ACK;
	p->ADDR_offset = plen;
	p->ADDR_length = ADDR_length;
	p->CONIND_number = CONIND_number;
	p->TOKEN_value = (BIND_flags & TOKEN_REQUEST) ? (np_ulong) (long) np->oq : 0;
	p->PROTOID_offset = p->ADDR_offset + ADDR_length;
	p->PROTOID_length = PROTOID_length;
	mp->b_wptr += sizeof(*p);

	if (likely(nc != NULL)) {
		*(typeof(nc) *) mp->b_wptr = nc;
		mp->b_wptr += sizeof(nc);
	}

	bcopy(ADDR_buffer, mp->b_wptr, ADDR_length);
	mp->b_wptr += ADDR_length;
	bcopy(PROTOID_buffer, mp->b_wptr, PROTOID_length);
	mp->b_wptr += PROTOID_length;

	nc_set_state(np->oq, np, nc, NS_IDLE);

	freemsg(msg);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_BIND_ACK");
	putnext(np->oq, mp);
	return (0);
      enomem:
	freeb(mp);
	return (-ENOMEM);
      enobufs:
	return (-ENOBUFS);
}

/** nc_error_ack - generate an N_ERROR_ACK and pass it upstream
  * @q: active queue in queue pair
  * @msg: message to free upon success
  * @np: private structure (locked)
  * @nc: network connection
  * @ERROR_prim: primitive in error
  * @error: error (positive NPI_error, negative UNIX_error)
  */
STATIC int
nc_error_ack(queue_t *q, mblk_t *msg, struct np *np, struct nc *nc, np_ulong ERROR_prim, np_long error)
{
	N_error_ack_t *p;
	mblk_t *mp;
	int state;
	const size_t plen = sizeof(*p) + (nc ? sizeof(nc) : 0);
	const size_t mlen = plen;

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	if (likely(nc != NULL)) {
		DB_TYPE(mp) = M_PCCTL;

		*(typeof(nc->tag) *) mp->b_wptr = nc->tag;
		mp->b_wptr += sizeof(nc->tag);
	} else {
		DB_TYPE(mp) = M_PCPROTO;
	}

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_ERROR_ACK;
	p->ERROR_prim = ERROR_prim;
	p->NPI_error = error < 0 ? NSYSERR : error;
	p->UNIX_error = error < 0 ? -error : 0;
	mp->b_wptr += sizeof(*p);

	switch ((state = nc_get_state(np, nc))) {
	case NS_WACK_BREQ:
		state = NS_UNBND;
		break;
	case NS_WACK_UREQ:
		state = NS_IDLE;
		break;
	case NS_WACK_OPTREQ:
		state = NS_IDLE;
		break;
	case NS_WACK_RRES:
		state = NS_WRES_RIND;
		break;
	case NS_WCON_CREQ:
		state = NS_IDLE;
		break;
	case NS_WACK_CRES:
		state = NS_WRES_CIND;
		break;
	case NS_WCON_RREQ:
		state = NS_DATA_XFER;
		break;
	case NS_WACK_DREQ6:
		state = NS_WCON_CREQ;
		break;
	case NS_WACK_DREQ7:
		state = NS_WRES_CIND;
		break;
	case NS_WACK_DREQ9:
		state = NS_DATA_XFER;
		break;
	case NS_WACK_DREQ10:
		state = NS_WCON_RREQ;
		break;
	case NS_WACK_DREQ11:
		state = NS_WRES_RIND;
		break;
	}
	nc_set_state(np->oq, np, nc, state);

	freemsg(msg);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_ERROR_ACK");
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/** nc_ok_ack - generate an N_OK_ACK and pass it upstream
  * @q: active queue in queue pair
  * @msg: message to free upon success
  * @np: private structure (locked)
  * @nc: network connection
  * @CORRECT_prim: correct primitive
  */
STATIC int
nc_ok_ack(queue_t *q, mblk_t *msg, struct np *np, struct nc *nc, np_ulong CORRECT_prim)
{
	N_ok_ack_t *p;
	mblk_t *mp;
	int state;
	const size_t mlen = sizeof(*p) + (nc ? sizeof(nc) : 0);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	if (likely(nc != NULL)) {
		DB_TYPE(mp) = M_PCCTL;

		*(typeof(nc->tag) *) mp->b_wptr = nc->tag;
		mp->b_wptr += sizeof(nc->tag);
	} else {
		DB_TYPE(mp) = M_PCPROTO;
	}

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_OK_ACK;
	p->CORRECT_prim = CORRECT_prim;
	mp->b_wptr += sizeof(*p);

	switch ((state = nc_get_state(np, nc))) {
	case NS_WACK_BREQ:
		state = NS_IDLE;
		break;
	case NS_WACK_UREQ:
		state = NS_UNBND;
		break;
	case NS_WACK_OPTREQ:
		state = NS_IDLE;
		break;
	case NS_WACK_RRES:
		/* FIXME: only when no more resets outstanding */
		state = --nc->resinds ? NS_WRES_RIND : NS_DATA_XFER;
		break;
	case NS_WACK_CRES:
		/* FIXME: only when no more connection indications outstanding */
		state = --nc->coninds ? NS_WRES_CIND : NS_IDLE;
		break;
	case NS_WACK_DREQ6:
		state = NS_IDLE;
		break;
	case NS_WACK_DREQ7:
		state = NS_IDLE;
		break;
	case NS_WACK_DREQ9:
		state = NS_IDLE;
		break;
	case NS_WACK_DREQ10:
		state = NS_IDLE;
		break;
	case NS_WACK_DREQ11:
		state = NS_IDLE;
		break;
	}
	nc_set_state(np->oq, np, nc, state);

	freemsg(msg);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_OK_ACK");
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/** nc_uderror_ind - generate an N_UDERROR_IND and pass it upstream
  * @q: active queue in queue pair
  * @msg: message to free upon success
  * @np: private structure (locked)
  * @nc: network connection
  */
STATIC int
nc_uderror_ind(queue_t *q, mblk_t *msg, struct np *np, struct nc *nc)
{
	N_uderror_ind_t *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + (nc ? sizeof(nc) : 0);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	if (unlikely(!bcanputnext(np->oq, 1)))
		goto ebusy;

	if (likely(nc != NULL)) {
		DB_TYPE(mp) = M_CTL;
		mp->b_band = 1;	/* expedite */

		*(typeof(nc->tag) *) mp->b_wptr = nc->tag;
		mp->b_wptr += sizeof(nc->tag);
	} else {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 1;	/* expedite */
	}

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_UDERROR_IND;
	mp->b_wptr += sizeof(*p);

	nc_set_state(np->oq, np, nc, NS_WRES_CIND);

	freemsg(msg);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_UDERROR_IND");
	putnext(np->oq, mp);
	return (0);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** nc_datack_ind - generate an N_DATACK_IND and pass it upstream
  * @q: active queue in queue pair
  * @msg: message to free upon success
  * @np: private structure (locked)
  * @nc: network connection
  */
STATIC int
nc_datack_ind(queue_t *q, mblk_t *msg, struct np *np, struct nc *nc)
{
	N_datack_ind_t *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + (nc ? sizeof(nc) : 0);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	if (unlikely(!bcanputnext(np->oq, 1)))
		goto ebusy;

	if (likely(nc != NULL)) {
		DB_TYPE(mp) = M_CTL;
		mp->b_band = 1;	/* expedite */

		*(typeof(nc->tag) *) mp->b_wptr = nc->tag;
		mp->b_wptr += sizeof(nc->tag);
	} else {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 1;	/* expedite */
	}

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATACK_IND;
	mp->b_wptr += sizeof(*p);

	freemsg(msg);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_DATACK_IND");
	putnext(np->oq, mp);
	return (0);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** nc_reset_ind - generate an N_RESET_IND and pass it upstream
  * @q: active queue in queue pair
  * @msg: message to free upon success
  * @np: private structure (locked)
  * @nc: network connection
  */
STATIC int
nc_reset_ind(queue_t *q, mblk_t *msg, struct np *np, struct nc *nc)
{
	N_reset_ind_t *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + (nc ? sizeof(nc) : 0);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	if (unlikely(!bcanputnext(np->oq, 1)))
		goto enobufs;

	if (likely(nc != NULL)) {
		DB_TYPE(mp) = M_CTL;
		mp->b_band = 1;	/* expedite */

		*(typeof(nc->tag) *) mp->b_wptr = nc->tag;
		mp->b_wptr += sizeof(nc->tag);
	} else {
		DB_TYPE(mp) = M_PROTO;
		mp->b_band = 1;	/* expedite */
	}

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_IND;
	mp->b_wptr += sizeof(*p);

	nc_set_state(np->oq, np, nc, NS_WRES_RIND);

	freemsg(msg);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_RESET_IND");
	putnext(np->oq, mp);
	return (0);

      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}

/** nc_reset_con - generate an N_RESET_CON and pass it upstream
  * @q: active queue in queue pair
  * @msg: message to free upon success
  * @np: private structure (locked)
  * @nc: network connection
  */
STATIC int
nc_reset_con(queue_t *q, mblk_t *msg, struct np *np, struct nc *nc)
{
	N_reset_con_t *p;
	mblk_t *mp;
	const size_t mlen = sizeof(*p) + (nc ? sizeof(nc) : 0);

	if (unlikely(!(mp = mi_allocb(q, mlen, BPRI_MED))))
		goto enobufs;

	if (unlikely(!bcanputnext(np->oq, 0)))
		goto ebusy;

	if (likely(nc != NULL)) {
		DB_TYPE(mp) = M_CTL;

		*(typeof(nc->tag) *) mp->b_wptr = nc->tag;
		mp->b_wptr += sizeof(nc->tag);
	} else {
		DB_TYPE(mp) = M_PROTO;
	}

	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_CON;
	mp->b_wptr += sizeof(*p);

	nc_set_state(np->oq, np, nc, NS_WRES_CIND);

	freemsg(msg);
	mi_strlog(np->oq, STRLOGTX, SL_TRACE, "<- N_RESET_CON");
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  ===================================================================
 *  ===================================================================
 *  ===================================================================
 *  ===================================================================
 *  ===================================================================
 */
/*
 *  NS User -> NS Provider (Request and Response) primitives
 *  ========================================================
 */
/** np_info_req - NE_INFO_REQ information request event
  * @q: active queue in pair (write queue)
  * @mp: N_INFO_REQ message
  * @np: private structure (locked)
  */
noinline int
np_info_req(queue_t *q, mblk_t *mp, struct np *np)
{
	N_info_req_t *p;

	if (likely(mp->b_wptr >= mp->b_rptr + sizeof(*p))) {
		p = (typeof(p)) mp->b_rptr;
		dassert(p->PRIM_type == N_INFO_REQ);
		return np_info_ack(q);
	}
	return (-EINVAL);
}

/*
 *  ===================================================================
 *  ===================================================================
 *  ===================================================================
 *  ===================================================================
 *  ===================================================================
 */
/** np_bind_req - NE_BIND_REQ bind a NS user to network address event
  * @q: active queue in pair (write queue)
  * @mp: N_BIND_REQ message
  * @np: private structure (locked)
  *
  * For this RTP driver, it is not possible to bind more than one address to a stream.  When
  * multiple addresses are provided in the N_BIND_REQ pirmitive, they represent a selection of
  * addresses from which the provider can choose the best address.  They are listed in the order of
  * priority.  When the port number is not provided, one will be selected by the provider.  It is
  * possible to bind a virtual interface to a wildcard address.  In this case, no packets may be
  * sent or received until the virtual interface is connected.  The Linux routing tables will be
  * consulted to determine a local IP address that can reach the remote IP address specified in the
  * N_CONN_REQ.
  *
  * It is possible to bind more than one address to a stream.  Each address is formatted as a
  * sockaddr_in up to the size of sockaddr_storage (which is 8 addresses on Linux).  If the
  * DEFAULT_DEST flag is set, then the port number is insignificant (and should be coded zero) or an
  * address is not provided.  When DEFAULT_DEST is not specified, the port number is significant.
  * Once bound, all packets that arrive for the specified address(es) and port number
  * combindation(s) are delivered to the bound stream.  For DEFAULT_DEST streams, all packets that
  * arrive for the specified address(es) that have port numbers that are bound to no other address
  * will be delivered to the DEFAULT_DEST stream.  DEFAULT_DEST streams that are bound with no
  * address (or to a wildcard address) will have all packets delivered that are bound to no other
  * address or port number combination.
  *
  * Protocol ids must always be specified.  Currently we don't allow binding to more than one
  * protocol id, but, instead of generating an error, we simply bind to the first protocol id
  * specified and ignore the reset.  We will only return the first protocol id in the bind ack.
  */
noinline int
np_bind_req(queue_t *q, mblk_t *mp, struct np *np)
{
	N_bind_req_t *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_BIND_REQ);
	err = NOUTSTATE;
	if (unlikely(np_not_state(np, NSF_UNBND)))
		goto error;
	np_set_state(np, NS_WACK_BREQ);
	err = NBADFLAG;
	if (unlikely((p->BIND_flags & ~(TOKEN_REQUEST | DEFAULT_DEST | DEFAULT_LISTENER)) != 0))
		goto error;
	switch (p->BIND_flags & (DEFAULT_DEST | DEFAULT_LISTENER)) {
	case DEFAULT_LISTENER:
		/* If DEFAULT_LISTENER is specified, the Stream is always considered connection-oriented.
		   Each protocol id and port number combination (whether the port number is zero or not) is
		   permitted multiple DEFAULT_LISTENER Streams (one for each IP address, and one for the
		   wildcard IP address).  DEFAULT_LISTENER Streams are only considered when there is no match 
		   for any other listening Stream.  It is ok under NPI-IP to set the DEFAULT_LISTENER flag,
		   even though CONIND_number is set to zero, in which case, it merely specifies that the
		   Stream is to be a pseudo-connection-oriented stream. */
		np->info.SERV_type = N_CONS;
		np->CONIND_number = p->CONIND_number;
		break;
	case DEFAULT_DEST:
		/* If DEFAULT_DEST is specified, the Stream is always considered connectionless. Each
		   protocol id is permitted multiple DEFAULT_DEST Streams (one for each IP address, and one
		   for the wildcard IP address).  DEFAULT_DEST Streams are only considered when there is no
		   match for any other connectionless bound Stream that matches the destination address. */
		np->info.SERV_type = N_CLNS;
		np->CONIND_number = 0;
		break;
	case 0:
		/* If the number of connection indications is zero, the bind is performed and the service
		   type set to connectionless.  If the number of connection indications is non-zero, the bind 
		   is performed and the service type set to connection-oriented. If, however, the
		   TOKEN_REQUEST flag is set, then the bind is always considered connection-oriented, even if 
		   the number of connection indications is set to zero, as this is the typical case for an
		   accepting Stream. */
		if (p->CONIND_number > 0 || (p->BIND_flags & TOKEN_REQUEST)) {
			np->info.SERV_type = N_CONS;
			np->CONIND_number = p->CONIND_number;
		} else {
			np->info.SERV_type = N_CLNS;
			np->CONIND_number = 0;
		}
		break;
	case (DEFAULT_LISTENER | DEFAULT_DEST):
		/* can't specify both */
		goto error;
	}
	err = NBADADDR;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->ADDR_offset + p->ADDR_length))
		goto error;
	err = NNOPROTOID;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->PROTOID_offset + p->PROTOID_length))
		goto error;
	if (unlikely(p->PROTOID_length == 0))
		goto error;
	{
		struct sockaddr_in ADDR_buffer[8];
		size_t ADDR_length = p->ADDR_length;
		unsigned char *PROTOID_buffer = (typeof(PROTOID_buffer)) (mp->b_rptr + p->PROTOID_offset);

		if (ADDR_length) {
			err = NBADADDR;
			if (unlikely(ADDR_length < sizeof(struct sockaddr_in)))
				goto error;
			if (unlikely(ADDR_length > sizeof(ADDR_buffer)))
				goto error;
			if (unlikely(ADDR_length % sizeof(struct sockaddr_in) != 0))
				goto error;
			/* avoid alignment problems */
			bcopy(mp->b_rptr + p->ADDR_offset, ADDR_buffer, ADDR_length);
			if (unlikely(ADDR_buffer[0].sin_family != AF_INET))
				goto error;
		} else {
			ADDR_length = sizeof(struct sockaddr_in);
			ADDR_buffer[0].sin_family = AF_INET;
			ADDR_buffer[0].sin_port = 0;
			ADDR_buffer[0].sin_addr.s_addr = INADDR_ANY;
		}
		/* cannot specify address if default listener */
		if (unlikely((p->BIND_flags & DEFAULT_LISTENER)
			     && (ADDR_buffer[0].sin_port != 0
				 || ADDR_buffer[0].sin_addr.s_addr != INADDR_ANY)))
			goto error;
		/* cannot listen on wildcard port number */
		if (unlikely(p->CONIND_number > 0 && ADDR_buffer[0].sin_port == 0))
			goto error;
		/* cannot specify port if default destination */
		if (unlikely((p->BIND_flags & DEFAULT_DEST) && ADDR_buffer[0].sin_port != 0))
			goto error;
		ptrace(("%s: %s: proto = %d, bport = %d\n", DRV_NAME, __FUNCTION__, (int) PROTOID_buffer[0],
			(int) ntohs(ADDR_buffer[0].sin_port)));
		err =
		    ne_bind_ack(q, PROTOID_buffer, p->PROTOID_length, ADDR_buffer, ADDR_length,
				p->CONIND_number, p->BIND_flags);
		if (unlikely(err != 0))
			goto error;
		return (QR_DONE);
	}
      error:
	return np_error_reply(q, N_BIND_REQ, err);
}

/** np_unbind_req - NE_UNBIND_REQ unbind NS user from network address event
  * @q: active queue in pair (write queue)
  * @mp: N_UNBIND_REQ message
  * @np: private structure (locked)
  */
STATIC INLINE fastcall int
np_unbind_req(queue_t *q, mblk_t *mp, struct np *np)
{
	N_unbind_req_t *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_UNBIND_REQ);
	err = NOUTSTATE;
	if (unlikely(np_not_state(np, NSF_IDLE)))
		goto error;
	np_set_state(np, NS_WACK_UREQ);
	err = np_ok_ack(q, N_UNBIND_REQ, NULL, 0, NULL, NULL, NULL, 0, NULL);
	if (unlikely(err != 0))
		goto error;
	return (QR_DONE);
      error:
	return np_error_reply(q, N_UNBIND_REQ, err);
}

/** np_optmgmt_req - NE_OPTMGMT_REQ options management request
  * @q: active queue in pair (write queue)
  * @mp: N_OPTMGMT_REQ message
  * @np: private structure (locked)
  */
noinline int
np_optmgmt_req(queue_t *q, mblk_t *mp, struct np *np)
{
	N_optmgmt_req_t *p;
	union N_qos_types qos_buf, *QOS_buffer = NULL;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_OPTMGMT_REQ);
	if (unlikely(np_chk_state(np, NSF_IDLE)))
		np_set_state(np, NS_WACK_OPTREQ);
	err = NBADOPT;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length))
		goto error;
	if (p->QOS_length) {
		err = NBADOPT;
		if (p->QOS_length < sizeof(QOS_buffer->n_qos_type))
			goto error;
		if (p->QOS_length > sizeof(*QOS_buffer))
			goto error;
		err = NBADQOSTYPE;
		QOS_buffer = &qos_buf;
		bcopy(mp->b_rptr + p->QOS_offset, QOS_buffer, p->QOS_length);
		switch (QOS_buffer->n_qos_type) {
		case N_QOS_SEL_INFO_IP:
			if (p->QOS_length != sizeof(QOS_buffer->ip.n_qos_sel_info))
				goto error;
			break;
		case N_QOS_SEL_CONN_IP:
			if (p->QOS_length != sizeof(QOS_buffer->ip.n_qos_sel_conn))
				goto error;
			break;
		case N_QOS_SEL_UD_IP:
			if (p->QOS_length != sizeof(QOS_buffer->ip.n_qos_sel_ud))
				goto error;
			break;
		case N_QOS_RANGE_INFO_IP:
			if (p->QOS_length != sizeof(QOS_buffer->ip.n_qos_range_info))
				goto error;
			break;
		case N_QOS_SEL_INFO_UDP:
			if (p->QOS_length != sizeof(QOS_buffer->udp.n_qos_sel_info))
				goto error;
			break;
		case N_QOS_SEL_CONN_UDP:
			if (p->QOS_length != sizeof(QOS_buffer->udp.n_qos_sel_conn))
				goto error;
			break;
		case N_QOS_SEL_UD_UDP:
			if (p->QOS_length != sizeof(QOS_buffer->udp.n_qos_sel_ud))
				goto error;
			break;
		case N_QOS_RANGE_INFO_UDP:
			if (p->QOS_length != sizeof(QOS_buffer->udp.n_qos_range_info))
				goto error;
			break;
		case N_QOS_SEL_INFO_RTP:
			if (p->QOS_length != sizeof(QOS_buffer->rtp.n_qos_sel_info))
				goto error;
			break;
		case N_QOS_SEL_CONN_RTP:
			if (p->QOS_length != sizeof(QOS_buffer->rtp.n_qos_sel_conn))
				goto error;
			break;
		case N_QOS_SEL_UD_RTP:
			if (p->QOS_length != sizeof(QOS_buffer->rtp.n_qos_sel_ud))
				goto error;
			break;
		case N_QOS_RANGE_INFO_RTP:
			if (p->QOS_length != sizeof(QOS_buffer->rtp.n_qos_range_info))
				goto error;
			break;
		default:
			goto error;
		}
	}
	if (p->OPTMGMT_flags & DEFAULT_RC_SEL)
		np->i_flags |= IP_FLAG_DEFAULT_RC_SEL;
	else
		np->i_flags &= ~IP_FLAG_DEFAULT_RC_SEL;
	err = np_ok_ack(q, N_OPTMGMT_REQ, NULL, 0, QOS_buffer, NULL, NULL, p->OPTMGMT_flags, NULL);
	if (unlikely(err != 0))
		goto error;
	return (QR_DONE);
      error:
	return np_error_reply(q, N_OPTMGMT_REQ, err);
}

/** ne_unitdata_req_slow - NE_UNITDATA_REQ unit data request event
  * @q: active queue in pair (write queue)
  * @mp: N_UNITDATA_REQ message
  *
  * The destination address, DEST_length and DEST_offset, must be specified. Neither the IP address
  * nor the UDP port number can be wildcarded.  The attached M_DATA block begin with the UDP
  * payload.  The IP header information for the transmitted packet can be set with a N_QOS_SEL_UD_IP
  * structure to N_OPTMGMT_REQ and will be taken from the Stream private structure for all packets
  * sent.  The NPI-UDP provider will request that the Stream head provide an additional write offset
  * of 24 bytes to accomodate the UDP header.  Note that IP header attributes can only be set on a
  * per-Stream basis for now rather than on a per-Nc basis.
  */
noinline fastcall __unlikely int
ne_unitdata_req_slow(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	size_t dlen;
	N_unitdata_req_t *p;
	struct sockaddr_in dst_buf, *DEST_buffer = NULL;
	int err;
	mblk_t *dp = mp->b_cont;
	uint32_t daddr;
	uint16_t dport;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_UNITDATA_REQ);
	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CONS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CLNS))
		goto error;
	if (unlikely(np_get_state(np) != NS_IDLE))
		goto error;
	err = NNOADDR;
	if (unlikely(p->DEST_length == 0))
		goto error;
	err = NBADADDR;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length))
		goto error;
	if (unlikely(p->DEST_length != sizeof(struct sockaddr_in)))
		goto error;
	/* avoid alignment problems */
	DEST_buffer = &dst_buf;
	bcopy(mp->b_rptr + p->DEST_offset, DEST_buffer, p->DEST_length);
	if (unlikely(DEST_buffer->sin_family != AF_INET))
		goto error;
	if (unlikely((daddr = DEST_buffer->sin_addr.s_addr) == INADDR_ANY))
		goto error;
	if (unlikely((dport = DEST_buffer->sin_port) == 0))
		goto error;
	err = NBADDATA;
	if (unlikely(dp == NULL))
		goto error;
	if (unlikely((dlen = msgsize(dp)) <= 0 || dlen > np->info.NSDU_size))
		goto error;
	if (unlikely((err = np_senddata(np, np->qos.protocol, dport, daddr, dp)) != QR_ABSORBED))
		goto error;
	return (QR_TRIMMED);
      error:
	if ((err = np_uderror_reply(q, DEST_buffer, 0, err, dp)) == QR_ABSORBED)
		return (QR_TRIMMED);
	return (err);
}

/** np_unitdata_req - NE_UNITDATA_REQ unit data request event
  * @q: write queue
  * @mp: the primitive
  * @np: private structure (locked)
  *
  * Optimize fast path for no options and correct behaviour.  This should run much faster than the
  * old plodding way of doing things.
  */
STATIC INLINE fastcall __hot_put int
np_unitdata_req(queue_t *q, mblk_t *mp, struct np *np)
{
	N_unitdata_req_t *p;
	mblk_t *dp;
	int err;
	struct sockaddr_in dst_buf;
	uint32_t daddr;
	uint16_t dport;

	prefetch(np);
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto go_slow;
	p = (typeof(p)) mp->b_rptr;
	prefetch(p);
	dassert(p->PRIM_type == N_UNITDATA_REQ);
	if (unlikely(np->info.SERV_type == N_CONS))
		goto go_slow;
	if (unlikely(np->info.SERV_type != N_CLNS))
		goto go_slow;
	if (unlikely(np_get_state(np) != NS_IDLE))
		goto go_slow;
	if (unlikely(p->DEST_length == 0))
		goto go_slow;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length))
		goto go_slow;
	if (unlikely(p->DEST_length != sizeof(struct sockaddr_in)))
		goto go_slow;
	/* avoid alignment problems */
	bcopy(mp->b_rptr + p->DEST_offset, &dst_buf, p->DEST_length);
	if (unlikely(dst_buf.sin_family != AF_INET))
		goto go_slow;
	if (unlikely(dst_buf.sin_addr.s_addr == INADDR_ANY))
		goto go_slow;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto go_slow;
	prefetch(dp);
	{
		size_t dlen;

		if (unlikely((dlen = msgsize(dp)) <= 0 || dlen > np->info.NSDU_size))
			goto go_slow;
	}
	daddr = dst_buf.sin_addr.s_addr;
	dport = dst_buf.sin_port;
	if (unlikely((err = np_senddata(np, np->qos.protocol, dport, daddr, dp)) != QR_ABSORBED))
		goto error;
	return (QR_TRIMMED);
      error:
	err = np_uderror_reply(q, &dst_buf, 0, err, dp);
	if (likely(err == QR_ABSORBED))
		return (QR_TRIMMED);
	return (err);
      go_slow:
	return ne_unitdata_req_slow(q, mp);
}

/** np_conn_req - NE_CONN_REQ connect NS user event
  * @q: active queue (write queue)
  * @mp: N_CONN_REQ message
  * @np: private structure (locked)
  *
  * Unlike the NPI-UDP driver, this driver does not support multi-homing (because RTP/RTCP for
  * telephony does not require it and it would significantly increase the size of the channel
  * structure).  When multiple addresses are specified in the N_CONN_REQ primitive, the NS-provider
  * will select the address that is to be used on the virtual interface.  The priority of selection
  * is the order of addresses as provided with descending preference.  This, however, is not a very
  * useful feature and typically only one remote address will be provided.
  *
  * When the channel has been bound to a null address (wildcard bound), the NS-provider will select
  * a suitable local address by consulting the Linux routing tables and will select a suitable local
  * port number considering hash/cache efficiency.  Port numbers for RTP are in the range 1024-65534
  * and include both the (even) RTP port number and the (odd) RTCP port number that is one greater
  * than the RTP port number.  Note, however, that there is also a field in the N_QOS_SEL_CONN_RTP
  * structure that can be used to negotiate the bound address at connection time.
  *
  * Request that a connection be made to (possibly supplemental) destination addresses.  The
  * addresses are formatted as an array of sockaddr_in structures.
  *
  * Fields are as follows:
  *
  * - PRIM_type: always N_CONN_REQ
  * - DEST_length: destination address length
  * - DEST_offset: destination address offset from start of message block
  * - CONN_flags: REC_CONF_OPT or EX_DATA_OPT
  * - QOS_length: quality of service parameters length
  * - QOS_offset: quality of service parameters offset from start of message block
  *
  * To support multihoming, this event permits an array of destination addresses.  The first address
  * in the array will be the primary address, unless another primary is specified in the QOS
  * parameters.  The primary address is used for subsequent NE_DATA_REQ and NE_EXDATA_REQ events
  * until changed with a NE_OPTMGMT_REQ event.
  *
  */
noinline fastcall __hot_put int
np_conn_req(queue_t *q, mblk_t *mp, struct np *np)
{
	N_conn_req_t *p;
	struct N_qos_sel_conn_rtp qos_buf = { N_QOS_SEL_CONN_RTP, }, *QOS_buffer = &qos_buf;
	struct sockaddr_in dst_buf[8] = { {AF_INET,}, }, *DEST_buffer = NULL;
	int err;
	mblk_t *dp = mp->b_cont;
	size_t dlen;
	int i;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_CONN_REQ);
	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Connection requests are not allowed on a listening Stream.  Note that there is a conflict in the
	   NPI specifications here: under the description for N_BIND_REQ, NPI 2.0.0 says: "If a Stream is
	   bound as a listener Stream, it will not be able to initiate connect requests. If the NS user
	   attempts to send an N_CONN_REQ primitive down this Stream, an N_ERROR_ACK message will be sent to
	   the NS user by the NS provider with an error value of NACCESS." Then, under N_BIND_ACK, NPI 2.0.0
	   says: "A Stream with a negotiated CONIND_number greater than zero may generate connect requests or 
	   accept connect indications." */
	err = NACCESS;
	if (unlikely(np->CONIND_number > 0))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np_get_state(np) != NS_IDLE))
		goto error;
	err = NBADFLAG;
	if (unlikely(p->CONN_flags != 0))
		goto error;
	err = NNOADDR;
	if (unlikely(p->DEST_length == 0))
		goto error;
	err = NBADADDR;
	if (unlikely(p->DEST_length > np->info.ADDR_size))
		goto error;
	if (unlikely(mp->b_wptr < mp->b_rptr + p->DEST_offset + p->DEST_length))
		goto error;
	if (unlikely(p->DEST_length < sizeof(*DEST_buffer)))
		goto error;
	if (unlikely(p->DEST_length > (sizeof(*DEST_buffer) << 3)))
		goto error;
	if (unlikely(p->DEST_length % sizeof(*DEST_buffer) != 0))
		goto error;
	DEST_buffer = dst_buf;
	bcopy(mp->b_rptr + p->DEST_offset, DEST_buffer, p->DEST_length);
	if (unlikely(DEST_buffer[0].sin_family != AF_INET && DEST_buffer[0].sin_family != 0))
		goto error;
	for (i = 0; i < p->DEST_length / sizeof(*DEST_buffer); i++)
		if (unlikely(DEST_buffer[i].sin_addr.s_addr == INADDR_ANY))
			goto error;
	if (p->QOS_length != 0) {
		err = NBADOPT;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length))
			goto error;
		if (unlikely(p->QOS_length < sizeof(QOS_buffer->n_qos_type)))
			goto error;
		bcopy(mp->b_rptr + p->QOS_offset, QOS_buffer, sizeof(QOS_buffer->n_qos_type));
		err = NBADQOSTYPE;
		if (unlikely(QOS_buffer->n_qos_type != N_QOS_SEL_CONN_RTP))
			goto error;
		if (unlikely(p->QOS_length != sizeof(*QOS_buffer)))
			goto error;
		bcopy(mp->b_rptr + p->QOS_offset, QOS_buffer, p->QOS_length);
	} else {
		/* set default values in QOS buffer */
		QOS_buffer->n_qos_type = N_QOS_SEL_CONN_RTP;
		QOS_buffer->protocol = QOS_UNKNOWN;
		QOS_buffer->priority = QOS_UNKNOWN;
		QOS_buffer->ttl = QOS_UNKNOWN;
		QOS_buffer->tos = QOS_UNKNOWN;
		QOS_buffer->mtu = QOS_UNKNOWN;
		QOS_buffer->saddr = QOS_UNKNOWN;
		QOS_buffer->daddr = QOS_UNKNOWN;
	}
	if (dp != NULL) {
		err = NBADDATA;
		if (unlikely((dlen = msgsize(dp)) <= 0 || dlen > np->info.CDATA_size))
			goto error;
	}
	/* Ok, all checking done.  Now we need to connect the new address. */
	err = np_conn_con(q, DEST_buffer, p->DEST_length, QOS_buffer, p->CONN_flags);
	if (unlikely(err != 0))
		goto error;
	/* send data only after connection complete */
	if (dp == NULL)
		return (QR_DONE);
	if (np_senddata(np, np->qos.protocol, np->dport, np->qos.daddr, dp) != QR_ABSORBED) {
		pswerr(("Discarding data on N_CONN_REQ\n"));
		return (QR_DONE);	/* discard the data */
	}
	return (QR_TRIMMED);	/* np_senddata() consumed message blocks */
      error:
	return np_error_reply(q, N_CONN_REQ, err);
}

STATIC INLINE fastcall mblk_t *
n_seq_check(struct np *np, np_ulong SEQ_number)
{
	mblk_t *cp;

	for (cp = bufq_head(&np->conq); cp && (np_ulong) (long) cp != SEQ_number; cp = cp->b_next) ;
	usual(cp);
	return (cp);
}

STATIC INLINE fastcall struct np *
n_tok_check(np_ulong TOKEN_value)
{
	struct np *ap;
	unsigned long flags;

	read_lock_str2(&master.lock, flags);
	for (ap = master.np.list; ap && (np_ulong) (long) ap->oq != TOKEN_value; ap = ap->next) ;
	read_unlock_str2(&master.lock, flags);
	usual(ap);
	return (ap);
}

/** np_conn_res - NE_CONN_RES accept previous connection indication event
  * @q: active queue (write queue)
  * @mp: N_CONN_RES message
  * @np: private structure (locked)
  *
  * An NE_CONN_IND event is generated when an IP message arrives for an address bound to an N_CONS
  * Stream and there is not yet a connection for the source address in the message.  Outstanding
  * connection indications are queued against the Stream with a sequence number assigned (derived
  * from the address of the connecting message held in the connection indication list).  The
  * NE_CONN_RES event is generated by the user to accept a connection indication event from the
  * connection indication queue and to perform a passive connection (NE_PASS_CON event) on the
  * indicated Stream.  In addition, the user can set QOS parameters for the Stream to which a
  * passive connection is made and for any reponse message (data attached to the N_CONN_RES
  * message).
  *
  * There is a deviation here from the NPI specifications: the responding address(es) in the
  * N_CONN_RES primitive contains the list of destination address(es) to which to form the
  * connection.  If no responding addresses are provided, then the destination address is the source
  * address from the connection indication.
  */
noinline fastcall int
np_conn_res(queue_t *q, mblk_t *mp, struct np *np)
{
	struct np *TOKEN_value = np;
	N_conn_res_t *p;
	struct N_qos_sel_conn_rtp qos_buf = { N_QOS_SEL_CONN_RTP, }, *QOS_buffer = NULL;
	struct sockaddr_in res_buf[8] = { {AF_INET,}, }, *RES_buffer = NULL;
	mblk_t *dp, *SEQ_number;
	size_t dlen;
	int err;

	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_CONN_RES);
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	if (unlikely(np_not_state(np, NSF_WRES_CIND)))
		goto error;
	if (p->RES_length != 0) {
		err = NBADADDR;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->RES_offset + p->RES_length))
			goto error;
		if (unlikely(p->RES_length < sizeof(*RES_buffer)))
			goto error;
		if (unlikely(p->RES_length > (sizeof(*RES_buffer) << 3)))
			goto error;
		if (unlikely(p->RES_length % sizeof(*RES_buffer) != 0))
			goto error;
		/* Cannot be sure that the address is properly aligned, and to keep unscrupulous users from
		   DoS attacks, copy the information into an aligned buffer. */
		bcopy(mp->b_rptr + p->RES_offset, &res_buf, p->RES_length);
		if (unlikely(res_buf[0].sin_family != AF_INET && res_buf[0].sin_family != 0))
			goto error;
		if (unlikely(res_buf[0].sin_port == 0))
			goto error;
		RES_buffer = res_buf;
	}
	if (p->QOS_length != 0) {
		err = NBADOPT;
		if (unlikely(mp->b_wptr < mp->b_rptr + p->QOS_offset + p->QOS_length))
			goto error;
		if (unlikely(p->QOS_length < sizeof(qos_buf.n_qos_type)))
			goto error;
		if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(qos_buf.n_qos_type)))
			goto error;
		/* Cannot be sure that the quality of service parameters are properly aligned, and to keep
		   unscrupulous users from DoS attacks, copy the informatoin into an aligned buffer. */
		bcopy(mp->b_rptr + p->QOS_offset, &qos_buf, sizeof(qos_buf.n_qos_type));
		err = NBADQOSTYPE;
		if (unlikely(qos_buf.n_qos_type != N_QOS_SEL_CONN_RTP))
			goto error;
		if (unlikely(p->QOS_length != sizeof(qos_buf)))
			goto error;
		QOS_buffer = &qos_buf;
		bcopy(mp->b_rptr + p->QOS_offset, QOS_buffer, p->QOS_length);
	}
	err = NBADDATA;
	if ((dp = mp->b_cont) != NULL)
		if (unlikely((dlen = msgsize(dp)) == 0 || dlen > np->info.CDATA_size))
			goto error;
	err = NBADSEQ;
	if (unlikely(p->SEQ_number == 0))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np_not_state(np, NSF_WRES_CIND) || bufq_length(&np->conq) < 1))
		goto error;
	err = NBADSEQ;
	if (unlikely((SEQ_number = n_seq_check(np, p->SEQ_number)) == NULL))
		goto error;
	if (p->TOKEN_value != 0) {
		err = NBADTOKEN;
		if (unlikely((TOKEN_value = n_tok_check(p->TOKEN_value)) == NULL))
			goto error;
		err = NBADTOKEN;
		if (unlikely(TOKEN_value->info.SERV_type == N_CLNS))
			goto error;
		err = NBADTOKEN;
		if (unlikely(TOKEN_value->CONIND_number > 0))
			goto error;
		err = NOUTSTATE;
		if (np_get_state(TOKEN_value) != NS_IDLE)
			/* Later we could auto bind if in NS_UNBND state. Note that the Stream to which we do 
			   a passive connection could be already connected: we just are just adding another
			   address to a multihomed connection.  But this is not as useful as adding or
			   removing an address with N_OPTMGMT_REQ. */
			goto error;
		err = NBADTOKEN;
		if (TOKEN_value->info.SERV_type != N_CONS)
			/* Must be connection-oriented Stream. */
			goto error;
	} else {
		err = NOUTSTATE;
		if (bufq_length(&np->conq) > 1)
			goto error;
	}
	/* Ok, all checking done.  Now we need to connect the new address. */
	np_set_state(np, NS_WACK_CRES);
	err =
	    np_ok_ack(q, N_CONN_RES, RES_buffer, p->RES_length, QOS_buffer, SEQ_number, TOKEN_value,
		      p->CONN_flags, dp);
	if (unlikely(err != QR_ABSORBED))
		goto error;
	return (QR_TRIMMED);	/* user data is absorbed */
      error:
	return np_error_reply(q, N_CONN_RES, err);
}

/** np_discon_req - process N_DISCON_REQ message
  * @q: active queue in queue pair (write queue)
  * @mp: the N_DISCON_REQ message
  * @np: private structure (locked)
  */
noinline fastcall int
np_discon_req(queue_t *q, mblk_t *mp, struct np *np)
{
	N_discon_req_t *p;
	struct sockaddr_in *RES_buffer = NULL;
	size_t RES_length = 0;
	mblk_t *dp, *SEQ_number = NULL;
	size_t dlen;
	int err;
	np_long state;

	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_DISCON_REQ);
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np_not_state(np, NSM_CONNECTED)))
		goto error;
	if ((RES_length = p->RES_length)) {
		err = -EINVAL;
		if (mp->b_wptr < mp->b_rptr + p->RES_offset + RES_length)
			goto error;
		err = NBADADDR;
		if (RES_length % sizeof(*RES_buffer) != 0)
			goto error;
		RES_buffer = (struct sockaddr_in *) (mp->b_rptr + p->RES_offset);
	}
	err = NBADDATA;
	if ((dp = mp->b_cont) != NULL)
		if (unlikely((dlen = msgsize(dp)) <= 0 || dlen > np->info.DDATA_size))
			goto error;
	/* FIXME: hold conq bufq_lock to keep connection indication state from changing */
	state = np_get_state(np);
	err = NBADSEQ;
	if (p->SEQ_number != 0) {
		if (unlikely(state != NS_WRES_CIND))
			goto error;
		if (unlikely((SEQ_number = n_seq_check(np, p->SEQ_number)) == NULL))
			goto error;
	} else {
		if (unlikely(state == NS_WRES_CIND))
			goto error;
	}
	err = NOUTSTATE;
	switch (state) {
	case NS_WCON_CREQ:
		np_set_state(np, NS_WACK_DREQ6);
		break;
	case NS_WRES_CIND:
		np_set_state(np, NS_WACK_DREQ7);
		break;
	case NS_DATA_XFER:
		np_set_state(np, NS_WACK_DREQ9);
		break;
	case NS_WCON_RREQ:
		np_set_state(np, NS_WACK_DREQ10);
		break;
	case NS_WRES_RIND:
		np_set_state(np, NS_WACK_DREQ11);
		break;
	default:
		goto error;
	}
	/* Ok, all checking done.  Now we need to disconnect the address. */
	err =
	    np_ok_ack(q, N_DISCON_REQ, RES_buffer, RES_length, NULL, SEQ_number, NULL, p->DISCON_reason, dp);
	if (unlikely(err != QR_ABSORBED))
		goto error;
	return (QR_TRIMMED);	/* user data is absorbed */
      error:
	return np_error_reply(q, N_DISCON_REQ, err);
}

/**
 * ne_write_req - process M_DATA message
 * @q: write queue
 * @mp: the M_DATA message
 * @np: private structure (locked)
 *
 * TODO: We should check the MSGDELIM flag and see whether this was a complete write or not.  If
 * not, we should accumulate the M_DATA block in a buffer waiting for a delimited message or final
 * N_DATA_REQ.
 */
noinline fastcall __unlikely int
ne_write_req(queue_t *q, mblk_t *mp, struct np *np)
{
	size_t dlen;
	int err;

	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Note: If the interface is in the NS_IDLE or NS_WRES_RIND states when the provider receives the
	   N_DATA_REQ primitive, then the NS provider should discard the request without generating a fatal
	   error. */
	if (np_chk_state(np, (NSF_IDLE | NSF_WRES_RIND)))
		goto discard;
	/* For multihomed operation, we should not actually discard the N_DATA_REQ if the destination of the
	   request is an address that does not have an outstanding reset indication. */
	if (unlikely(np_not_state(np, NSM_OUTDATA)))
		goto error;
	/* If we are writing we must include the IP header, which is at least 20 bytes, and, if the Stream is 
	   bound to a port, at least the size of a UDP header.  The length of the entire NSDU must not exceed 
	   65535 bytes. */
	err = NBADDATA;
	if (unlikely((dlen = msgsize(mp)) == 0 || dlen > np->info.NIDU_size || dlen > np->info.NSDU_size))
		goto error;
	if (unlikely((err = np_senddata(np, np->qos.protocol, np->dport, np->qos.daddr, mp)) != QR_ABSORBED))
		goto error;
	return (QR_ABSORBED);	/* np_senddata() consumed message block */
      discard:
	return (QR_DONE);	/* np_senddata() did not consume message blocks */
      error:
	return np_error_fatal(q, -EPROTO);
}

/** np_data_req - process N_DATA_REQ message
  * @q: active queue in queue pair (write queue)
  * @mp: the N_DATA_REQ message
  * @np: private structure (locked)
  *
  * Unfortunately, there is no standard way of specifying destination and source addreses for
  * multihomed hosts.  We use N_OPTMGMT_REQ to change the primary destination address, source
  * address and QOS parameters.
  */
noinline fastcall __hot_put int
np_data_req(queue_t *q, mblk_t *mp, struct np *np)
{
	N_data_req_t *p;
	size_t dlen;
	mblk_t *dp;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_DATA_REQ);
	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Note: If the interface is in the NS_IDLE or NS_WRES_RIND states when the provider receives the
	   N_DATA_REQ primitive, then the NS provider should discard the request without generating a fatal
	   error. */
	if (np_chk_state(np, (NSF_IDLE | NSF_WRES_RIND)))
		/* For multihomed operation, we should not actually discard the N_DATA_REQ if the destination 
		   of the request is an address that does not have an outstanding reset indication. */
		goto discard;
	err = NOUTSTATE;
	if (unlikely(np_not_state(np, NSM_OUTDATA)))
		goto error;
	err = NBADFLAG;
	if (unlikely(p->DATA_xfer_flags & (N_MORE_DATA_FLAG | N_RC_FLAG)))
		/* TODO: We should check the N_MORE_DATA_FLAG and see whether this is a complete NSDU or not. 
		   If not, we should accumulate the M_DATA block in a buffer waiting for a final N_DATA_REQ
		   or delimited message.  */
		goto error;
	err = NBADDATA;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto error;
	if (unlikely((dlen = msgsize(dp)) == 0 || dlen > np->info.NIDU_size || dlen > np->info.NSDU_size))
		goto error;
	if (unlikely((err = np_senddata(np, np->qos.protocol, np->dport, np->qos.daddr, dp)) != QR_ABSORBED))
		goto error;
	return (QR_TRIMMED);	/* np_senddata() consumed message blocks */
      discard:
	return (QR_DONE);	/* np_senddata() does not consume message blocks */
      error:
	return np_error_fatal(q, -EPROTO);
}

/** np_exdata_req - process N_EXDATA_REQ message
  * @q: active queue in queue pair (write queue)
  * @mp: the N_EXDATA_REQ message
  * @np: private structure (locked)
  */
noinline fastcall int
np_exdata_req(queue_t *q, mblk_t *mp, struct np *np)
{
	N_exdata_req_t *p;
	size_t dlen;
	mblk_t *dp;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_EXDATA_REQ);
	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Note: If the interface is in the NS_IDLE or NS_WRES_RIND states when the provider receives the
	   N_EXDATA_REQ primitive, then the NS provider should discard the request without generating a fatal 
	   error. */
	if (np_chk_state(np, (NSF_IDLE | NSF_WRES_RIND)))
		goto discard;
	if (unlikely(np_not_state(np, NSM_OUTDATA)))
		goto error;
	err = NBADDATA;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto error;
	dlen = msgsize(dp);
	if (unlikely(dlen == 0 || dlen > np->info.NIDU_size || dlen > np->info.ENSDU_size))
		goto error;
	err = np_senddata(np, np->qos.protocol, np->dport, np->qos.daddr, dp);
	if (unlikely(err != QR_ABSORBED))
		goto error;
	return (QR_TRIMMED);
      discard:
	return (QR_DONE);	/* np_senddata() does not consume message blocks */
      error:
	return np_error_fatal(q, -EPROTO);
}

/** np_datack_req - process N_DATACK_REQ message
  * @q: active queue in queue pair (write queue)
  * @mp: the N_DATACK_REQ message
  * @np: private structure (locked)
  */
noinline fastcall int
np_datack_req(queue_t *q, mblk_t *mp, struct np *np)
{
	N_datack_req_t *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (typeof(p)) mp->b_rptr;
	dassert(p->PRIM_type == N_DATACK_REQ);
	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Note: If the interface is in the NS_IDLE state when the provider receives the N_DATACK_REQ
	   primitive, then the NS provider should discard the request without generating a fatal error. */
	if (np_get_state(np) == NS_IDLE)
		goto discard;
	if (unlikely(np_not_state(np, NSM_CONNECTED)))
		goto error;
	/* Note: If the NS provider had no knowledge of a previous N_DATA_IND with the receipt confirmation
	   flag set, then the NS provider should just ignore the request without generating a fatal error. */
	if (unlikely(bufq_length(&np->datq) <= 0))
		goto error;
	if (unlikely((err = np_datack(q)) < 0))
		goto error;
      discard:
	return (QR_DONE);
      error:
	return np_error_fatal(q, -EPROTO);
}

/** np_reset_req - process N_RESET_REQ message
  * @q: active queue in queue pair (write queue)
  * @mp: the N_RESET_REQ message
  * @np: private structure (locked)
  */
noinline fastcall int
np_reset_req(queue_t *q, mblk_t *mp, struct np *np)
{
	N_reset_req_t *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (N_reset_req_t *) mp->b_rptr;
	dassert(p->PRIM_type == N_RESET_REQ);
	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	/* Note: If the interface is in the NS_IDLE state when the provider receives the N_RESET_REQ
	   primitive, then the NS provider should discard the message without generating an error. */
	err = 0;
	if (np_get_state(np) == NS_IDLE)
		goto error;
	err = NOUTSTATE;
	if (unlikely(np_get_state(np) != NS_DATA_XFER))
		goto error;
	/* Ok, message checks out. */
	np_set_state(np, NS_WCON_RREQ);
	if ((err = np_reset_con(q, N_USER, p->RESET_reason, mp->b_cont)) != 0)
		goto error;
	return (QR_DONE);	/* user data message blocks are not absorbed */
      error:
	return np_error_reply(q, N_RESET_REQ, err);

}

/** np_reset_res - process N_RESET_RES message
  * @q: active queue in queue pair (write queue)
  * @mp: the N_RESET_RES message
  * @np: private structure (locked)
  *
  * When a reset indication is sent to the user, we expect a reset response which is acknowledged.
  * The reason is, only one oustanding reset indication per destination per RESET_reason is issued.
  * No additional reset indication will be issued until a response is received.  Because of this,
  * reset indications are expedited (band 2).
  */
noinline fastcall int
np_reset_res(queue_t *q, mblk_t *mp, struct np *np)
{
	N_reset_res_t *p;
	int err;

	err = -EINVAL;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(*p)))
		goto error;
	p = (N_reset_res_t *) mp->b_rptr;
	dassert(p->PRIM_type == N_RESET_RES);
	err = NNOTSUPPORT;
	if (unlikely(np->info.SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(np->info.SERV_type != N_CONS))
		goto error;
	if (unlikely(np_not_state(np, NSF_WRES_RIND)))
		goto error;
	/* Ok, parameters check out. */
	np_set_state(np, NS_WACK_RRES);
	err = np_ok_ack(q, N_RESET_RES, NULL, 0, NULL, NULL, NULL, 0, NULL);
	if (unlikely(err != 0))
		goto error;
	return (QR_DONE);
      error:
	return np_error_reply(q, N_RESET_RES, err);
}

/** np_other_prim - handle wrong primitive on queue
  * @q: active queue in queue pair (write queue)
  * @mp: the primitive
  * @np: private structure (locked)
  * @prim: the primitive type
  */
noinline fastcall __unlikely int
np_other_prim(queue_t *q, mblk_t *mp, struct np *np, np_ulong prim)
{
	freemsg(mp);

	switch (prim) {
	case N_CONN_IND:	/* Incoming connection indication */
	case N_CONN_CON:	/* Connection established */
	case N_DISCON_IND:	/* NC disconnected */
	case N_DATA_IND:	/* Incoming connection-mode data indication */
	case N_EXDATA_IND:	/* Incoming expedited data indication */
	case N_INFO_ACK:	/* Information Acknowledgement */
	case N_BIND_ACK:	/* NS User bound to network address */
	case N_ERROR_ACK:	/* Error Acknowledgement */
	case N_OK_ACK:		/* Success Acknowledgement */
	case N_UNITDATA_IND:	/* Connection-less data receive indication */
	case N_UDERROR_IND:	/* UNITDATA Error Indication */
	case N_DATACK_IND:	/* Data acknowledgement indication */
	case N_RESET_IND:	/* Incoming NC reset request indication */
	case N_RESET_CON:	/* Reset processing complete */
		/* wrong direction */
		mi_strlog(q, STRLOGERR, SL_ERROR, "%s in wrong direction", np_primname(prim));
		break;
	default:
		/* unrecognized primitive */
		mi_strlog(q, STRLOGERR, SL_ERROR, "Unrecognized primitive %d", (int) prim);
		break;
	}
	return (0);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  NS-User to NS-Provider messages (multi-Stream)
 *
 *  -------------------------------------------------------------------------
 */

/** nc_data_req - process N_DATA_REQ primitive
  * @q: active queue
  * @mp: the N_DATA_REQ
  * @np: private structure (locked)
  * @nc: network connection
  *
  * Unfortunately, there is no way of specifying destination and source addresses for multihomed
  * hosts.  We use N_OPTMGMT_REQ to change the primary destination address, source address and QOS
  * parameters.
  */
STATIC INLINE fastcall __hot_put int
nc_data_req(queue_t *q, mblk_t *mp, struct np *np, struct nc *nc)
{
	N_data_req_t *p;
	int err;
	size_t dlen;
	mblk_t *dp;

	err = -EMSGSIZE;
	if (unlikely(!MBLKIN(mp, sizeof(nc), sizeof(*p))))
		goto error;
	p = (typeof(p)) (mp->b_rptr + sizeof(nc));
	dassert(p->PRIM_type == N_DATA_REQ);
	err = NNOTSUPPORT;
	if (unlikely(nc->SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(nc->SERV_type != N_CONS))
		goto error;
	/* Note: when the interface is in the NS_IDLE or NS_WRES_RIND states when the provider receives the
	   N_DATA_REQ primitive, then the NS provider should discard the request without generating a fatal
	   error. */
	if (nc_chk_state(nc, (NSF_IDLE | NSF_WRES_RIND)))
		/* For multihomed operation, we should not actually discard the N_DATA_REQ if the destination 
		   of the request is the address that does not have an outstanding reset indication. */
		goto discard;
	err = NOUTSTATE;
	if (unlikely(np_not_state(np, NSM_OUTDATA)))
		goto error;
	err = NBADFLAG;
	if (unlikely(p->DATA_xfer_flags & (N_MORE_DATA_FLAG | N_RC_FLAG)))
		/* TODO: we should check the N_MORE_DATA_FLAG and see whether this is a complete NSDU or not. 
		   If not, we should accumulate the M_DATA block in a buffer waiting for a findal N_DATA_REQ
		   or delimited message. */
		goto error;
	err = NBADDATA;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto error;
	if (unlikely((dlen = msgsize(dp)) == 0 || dlen > np->info.NIDU_size || dlen > np->info.NSDU_size))
		goto error;
	return nc_senddata(q, mp, np, nc, NULL, dp);
      error:
	return nc_error_reply(q, mp, np, nc, N_DATA_REQ, -EPROTO);
      discard:
	freemsg(mp);
	return (0);
}

noinline fastcall int
 nc_unitdata_req_slow(queue_t *q, mblk_t *mp, struct np *np, struct nc *nc);

/** nc_unitdata_req - process N_DATA_REQ primitive
  * @q: active queue
  * @mp: the N_DATA_REQ
  * @np: private structure (locked)
  * @nc: network connection
  */
STATIC INLINE fastcall __hot_put int
nc_unitdata_req(queue_t *q, mblk_t *mp, struct np *np, struct nc *nc)
{
	N_unitdata_req_t *p;
	mblk_t *dp;
	size_t dlen;
	struct sockaddr_in dst;
	caddr_t dst_ptr = (caddr_t) &dst;
	size_t dst_len;

	prefetch(nc);
	if (unlikely(!MBLKIN(mp, sizeof(nc), sizeof(*p))))
		goto go_slow;
	p = (typeof(p)) (mp->b_rptr + sizeof(nc));
	dassert(p->PRIM_type == N_UNITDATA_REQ);
	prefetch(p);
	if (unlikely(nc->SERV_type == N_CONS))
		goto go_slow;
	if (unlikely(nc->SERV_type != N_CLNS))
		goto go_slow;
	if (unlikely(nc_get_state(np, nc) != NS_IDLE))
		goto go_slow;
	if (unlikely((dst_len = p->DEST_length) == 0))
		goto go_slow;
	if (unlikely(!MBLKIN(mp, p->DEST_offset, dst_len)))
		goto go_slow;
	if (unlikely(dst_len != sizeof(struct sockaddr_in)))
		goto go_slow;
	bcopy(mp->b_rptr + p->DEST_offset, dst_ptr, dst_len);
	if (unlikely(dst.sin_family != AF_INET && dst.sin_family != 0))
		goto go_slow;
	if (unlikely(dst_buf.sin_addr.s_addr == INADDR_ANY))
		goto go_slow;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto go_slow;
	prefetch(dp);
	if (unlikely((dlen = msgsize(dp)) <= 0 || dlen > np->info.NSDU_size))
		goto go_slow;
	return nc_senddata(q, mp, np, nc, &dst, dp);
      error:
	return nc_error_reply(q, mp, np, nc, N_UNITDATA_REQ, -EPROTO);
      go_slow:
	return nc_unitdata_req_slow(q, mp, np, nc);

}

/** nc_unitdata_req_slow: - process complex N_DATA_REQ primitive
  * @q: active queue
  * @mp: the N_UNITDATA_REQ
  * @np: private structure (locked)
  * @nc: network connection
  *
  * The destination address, DEST_length and DEST_offset, must be specified. Neither the IP address
  * nor the UDP port number can be wildcarded.  The attached M_DATA block begin with the UDP
  * payload.  The IP header information for the transmitted packet can be set with a N_QOS_SEL_UD_IP
  * structure to N_OPTMGMT_REQ and will be taken from the Stream private structure for all packets
  * sent.  The NPI-UDP provider will request that the Stream head provide an additional write offset
  * of 24 bytes to accomodate the UDP header.  Note that IP header attributes can only be set on a
  * per-Stream basis for now rather than on a per-Nc basis.
  */
noinline fastcall int
nc_unitdata_req_slow(queue_t *q, mblk_t *mp, struct np *np, struct nc *nc)
{
	N_unitdata_req_t *p;
	int err;
	mblk_t *dp;
	size_t dlen;
	struct sockaddr_in dst;
	caddr_t dst_ptr = (caddr_t) &dst;
	size_t dst_len;

	err = -EMSGSIZE;
	if (unlikely(!MBLKIN(mp, sizeof(nc), sizeof(*p))))
		goto error;
	p = (typeof(p)) (mp->b_rptr + sizeof(nc));
	dassert(p->PRIM_type == N_UNITDATA_REQ);
	err = NNOTSUPPORT;
	if (unlikely(nc->SERV_type == N_CONS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(nc->SERV_type != N_CLNS))
		goto error;
	err = NNOADDR;
	if (unlikely((dst_len = p->DEST_length) == 0))
		goto error;
	err = NBADADDR;
	if (unlikely(!MBLKIN(mp, p->DEST_offset, p->DEST_length)))
		goto error;
	if (unlikely(dst_len != sizeof(dst)))
		goto error;
	bcopy(mp->b_rptr + p->DEST_offset, dst_ptr, dst_len);
	if (unlikely(dst.sin_family != AF_INET || dst.sin_family != 0))
		goto error;
	if (unlikely(dst.sin_addr.s_addr == INADDR_ANY))
		goto error;
	if (unlikely(dst.sin_port == 0))
		goto error;
	err = NBADDATA;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto error;
	if (unlikely((dlen = msgsize(dp)) <= 0 || dlen > np->info.NSDU_size))
		goto error;
	return np_senddata(q, mp, np, nc, &dst, dp);
      error:
	return nc_error_reply(q, mp, np, nc, N_UNITDATA_REQ, -EPROTO);
}

/** nc_exdata_req - process N_DATA_REQ primitive
  * @q: active queue
  * @mp: the N_DATA_REQ
  * @np: private structure (locked)
  * @nc: network connection
  */
STATIC INLINE fastcall __hot_put int
nc_exdata_req(queue_t *q, mblk_t *mp, struct np *np, struct nc *nc)
{
	N_exdata_req_t *p;
	int err;
	size_t dlen;
	mblk_t *dp;

	err = -EMSGSIZE;
	if (unlikely(!MBLKIN(mp, sizeof(nc), sizeof(*p))))
		goto error;
	p = (typeof(p)) (mp->b_rptr + sizeof(nc));
	dassert(p->PRIM_type == N_EXDATA_REQ);
	err = NNOTSUPPORT;
	if (unlikely(nc->SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(nc->SERV_type != N_CONS))
		goto error;
	/* Note: when the interface is in the NS_IDLE or NS_WRES_RIND states when the provider receives the
	   N_EXDATA_REQ primitive, the NS provider should discard the requeswt without generating a fatal
	   error. */
	if (unlikely(nc_chK_state(nc, (NSF_IDLE | NSF_WRES_RIND))))
		goto error;
	if (unlikely(nc_not_state(nc, NSM_OUTDATA)))
		goto error;
	err = NBADDATA;
	if (unlikely((dp = mp->b_cont) == NULL))
		goto error;
	dlen = msgsize(dp);
	if (unlikely(dlen == 0 || dlen > np->info.NIDU_size || dlen > np->info.ENSDU_size))
		goto error;
	return nc_senddata(q, mp, np, nc, NULL, dp);
      error:
	return nc_error_reply(q, mp, np, nc, N_EXDATA_REQ, -EPROTO);
      discard:
	freemsg(mp);
	return (0);
}

/** nc_conn_req - process N_DATA_REQ primitive
  * @q: active queue
  * @mp: the N_DATA_REQ
  * @np: private structure (locked)
  * @nc: network connection
  */
static int
nc_conn_req(queue_t *q, mblk_t *mp, struct np *np, struct nc *nc)
{
	N_conn_req_t *p;
	int err, i;
	struct sockaddr dst;
	struct sockaddr_in *DEST_buffer = (struct sockaddr_in *) dst;
	caddr_t dst_ptr = (caddr_t) &dst;
	size_t dst_len;
	struct N_qos_sel_conn_rtp qos = { N_QOS_SEL_CONN_RTP, }, *QOS_buffer = &qos;
	caddr_t qos_ptr = (caddr_t) &qos;
	size_t qos_len, dlen;
	mblk_t dp;

	err = -EMSGSIZE;
	if (unlikely(!MBLKIN(mp, sizeof(nc), sizeof(*p))))
		goto error;
	p = (typeof(p)) (mp->b_rptr + sizeof(nc));
	dassert(p->PRIM_type == N_CONN_REQ);
	err = NNOTSUPPORT;
	if (unlikely(nc->SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(nc->SERV_type != N_CONS))
		goto error;
	/* Connection requests are not allowed on a listening Stream.  Note that there is a conflict in the
	   NPI specification here: under the description for N_BIND_REQ, NPI 2.0.0 says: "If a Stream is
	   bound as a listener Stream, it will not be able to initiate connect requests. If the NS user
	   attempts to send a N_CONN_REQ primitive down this Stream, an N_ERROR_ACK message will be sent to
	   the NS user by the NS provider with an error value of NACCESS." Then, under N_BIND_ACK, NPI 2.0.0
	   says: "A Stream with a negotiated CONIND_number greater than zero may generate connect requests or 
	   accept connect indications." */
	err = NACCESS;
	if (unlikely(nc->CONIND_number > 0))
		goto error;
	err = NOUTSTATE;
	if (unlikely(nc_get_state(np, nc) != NS_IDLE))
		goto error;
	err = NBADFLAG;
	if (unlikely(p->CONN_flags != 0))
		goto error;
	dst_len = p->DEST_length;
	err = NNOADDR;
	if (unlikely(dst_len == 0))
		goto error;
	err = NBADADDR;
	if (unlikely(dst_len > np->info.ADDR_size))
		goto error;
	if (unlikely(!MBLKIN(mp, p->DEST_offset, p->DEST_length)))
		goto error;
	if (unlikely(dst_len < sizeof(struct sockaddr_in)))
		goto error;
	if (unlikely(dst_len > sizeof(struct sockaddr)))
		goto error;
	if (unlikely(dst_len % sizeof(struct sockaddr_in) != 0))
		goto error;
	bcopy(mp->b_rtpr + p->DEST_offset, dst_ptr, dst_len);
	if (unlikely(DEST_buffer[0].sin_family != AF_INET && DEST_buffer[0].sin_family != 0))
		goto error;
	for (i = 0; i < dst_len / sizeof(*DEST_buffer); i++)
		if (unlikely(DEST_buffer[i].sin_addr.s_addr == INADDR_ANY))
			goto error;
	if ((qos_len = p->QOS_length)) {
		err = NBADOPT;
		if (unlikely(!MBLKIN(mp, p->QOS_offset, p->QOS_length)))
			goto error;
		if (unlikely(qos_len < sizeof(QOS_buffer->n_qos_type)))
			goto error;
		bcopy(mp->b_rptr + p->QOS_offset, qos_ptr, sizeof(QOS_buffer->n_qos_type));
		err = NBADQOSTYPE;
		if (unlikely(QOS_buffer->n_qos_type != N_QOS_SEL_CONN_RTP))
			goto error;
		if (unlikely(qos_len != sizeof(*QOS_buffer)))
			goto error;
		bcopy(mp->b_rptr + p->QOS_offset, qos_ptr, qos_len);
	} else {
		QOS_buffer->n_qos_type = N_QOS_SEL_CONN_RTP;
		QOS_buffer->protocol = QOS_UNKNOWN;
		QOS_buffer->priority = QOS_UNKNOWN;
		QOS_buffer->ttl = QOS_UNKNOWN;
		QOS_buffer->tos = QOS_UNKNOWN;
		QOS_buffer->mtu = QOS_UNKNOWN;
		QOS_buffer->saddr = QOS_UNKNOWN;
		QOS_buffer->daddr = QOS_UNKNOWN;
	}
	if ((dp = mp->b_cont) != NULL) {
		err = NBADDATA;
		if (unlikely((dlen = msgsize(dp)) <= 0 || dlen > np->info.CDATA_size))
			goto error;
	}
	return nc_conn_con(q, mp, np, nc, p->CONN_flags, dst_ptr, dst_len, qos_ptr, qos_len, NULL);
      error:
	return nc_error_reply(q, np, np, nc, N_CONN_REQ, err);

}

/**
 * nc_conn_res - process N_DATA_REQ primitive
 * @q: active queue
 * @mp: the N_DATA_REQ
 * @np: private structure (locked)
 * @nc: network connection
 *
 * An NE_CONN_IND event is generated when a UDP message arrives for an address to an N_CONS
 * Stream and there is not yet a connection for the source address in the message.  Outstanding
 * connection indications are queued against the Stream with a sequence number assigned (derived
 * from the address of the connecting message held in the connection indication list).  The
 * NE_CONN_RES event is generated by the user to accept a connection indication event from the
 * connection indication queue and to perform a passive connection (NE_PASS_CON event) on the
 * indicated Stream.  In addition, the user can set QOS parameters for the Stream to which a passive
 * connection is made and for any response message (data attached to the N_CONN_RES message). 
 *
 * There is a deviation here from the NPI specifications: the responding address(es) in the
 * N_CONN_RES primitive contains the list of destination address(es) to which to form the
 * connection.  If no responding addresses are provided, then the destination address is the source
 * address from the connection indication.
 */
static int
nc_conn_res(queue_t *q, mblk_t *mp, struct np *np, struct nc *nc)
{
	N_conn_res_t *p;
	struct sockaddr res;
	struct sockaddr_in *RES_buffer = (struct sockaddr_in *) res;
	caddr_t res_ptr = (caddr_t) &res;
	size_t res_len;
	struct N_qos_sel_conn_rtp qos, *QOS_buffer = &qos;
	caddr_t qos_ptr = (caddr_t) &qos;
	size_t qos_len;
	mblk_t *dp, *SEQ_number;
	size_t dlen;
	int err;

	err = NNOTSUPPORT;
	if (unlikely(nc->SERV_type == N_CLNS))
		goto error;
	err = -EMSGSIZE;
	if (unlikely(!MBLKIN(mp, sizeof(nc), sizeof(*p))))
		goto error;
	p = (typeof(p)) (mp->b_rptr + sizeof(nc));
	dassert(p->PRIM_type == N_CONN_RES);
	err = NOUTSTATE;
	if (unlikely(nc->SERV_type != N_CONS))
		goto error;
	if (unlikely(nc_get_state(np, nc) != NS_WRES_CIND))
		goto error;
	if ((res_len = p->RES_length)) {
		err = NBADADDR;
		if (unlikely(!MBLKIN(mp, p->RES_offset, p->RES_length)))
			goto error;
		if (unlikely(res_len < sizeof(*RES_buffer)))
			goto error;
		if (unlikely(res_len > sizeof(res)))
			goto error;
		if (unlikely(res_len % sizeof(*RES_buffer) != 0))
			goto error;
		/* Cannot be sure that the address is properly aligned, and to keep unscrupulous users from
		   DoS attacks, copy the information into an aligned buffer. */
		bcopy(mp->b_rptr + p->RES_offset, res_ptr, res_len);
		if (unlikely((RES_buffer[0].sin_family != AF_INET && RES_buffer[0].sin_family != 0)))
			goto error;
		if (unlikely(RES_buffer[0].sin_port == 0))
			goto error;
	}
	if ((qos_len = p->QOS_length)) {
		err = NBADOPT;
		if (unlikely(!MBLKIN(mp, p->QOS_offset, p->QOS_length)))
			goto error;
		if (unlikely(qos_len < sizeof(QOS_buffer->n_qos_type)))
			goto error;
		/* Cannot be sure that the quality of service parameters are properly aligned, and to keep
		   unscrupulous users from DoS attacks, copy the information into an aligned buffer. */
		bcopy(mp->b_rptr + p->QOS_offset, qos_ptr, sizeof(QOS_buffer->n_qos_type));
		err = NBADQOSTYPE;
		if (unlikely(QOS_buffer->n_qos_type != N_QOS_SEL_CONN_RTP))
			goto error;
		if (unlikely(qos_len != sizeof(*QOS_buffer)))
			goto error;
		bcopy(mp->b_rptr + p->QOS_offset, qos_ptr, qos_len);
	}
	if ((dp = mp->b_cont) != NULL) {
		err = NBADDATA;
		if (unlikely((dlen = msgsize(dp)) == 0 || dlen > np->info.CDATA_size))
			goto error;
	}
	err = NBADSEQ;
	if (unlikely(p->SEQ_number == 0))
		goto error;
	err = NOUTSTATE;
	if (unlikely(nc_get_state(np, nc) != NS_WRES_CIND || nc->conlist != NULL))
		goto error;
	err = NBADSEQ;
	if (unlikely((SEQ_number = nc_seq_check(np, nc, p->SEQ_number)) == NULL))
		goto error;
	if (p->TOKEN_value != 0) {
		err = NBADTOKEN;
		if (unlikely((TOKEN_value = nc_tok_check(np, nc, p->TOKEN_value)) == NULL))
			goto error;
		if (unlikely(TOKEN_value->SERV_type == N_CLNS))
			goto error;
		if (unlikely(TOKEN_value->CONIND_number > 0))
			goto error;
		err = NOUTSTATE;
		if (nc_get_state(np, TOKEN_value) != NS_IDLE)
			/* Later we could auto bind if in NS_UNBND state.  Note that the Stream to which se
			   do a passive connection could be already connected: we are just adding another
			   address to the multihomed connection.  But this is not as useful as adding or
			   removing an address with N_OPTMGMT_REQ. */
			goto error;
		err = NBADTOKEN;
		if (unlikely(TOKEN_value->SERV_type != N_CONS))
			/* Must be connection-oriented Stream. */
			goto error;
	} else {
		err = NOUTSTATE;
		if (nc->conq)
			goto error;
	}
	return nc_ok_ack(q, mp, np, nc, N_CONN_RES);
      error:
	return nc_error_reply(q, mp, np, nc, N_CONN_RES, err);
}

/**
 * nc_discon_req - process N_DATA_REQ primitive
 * @q: active queue
 * @mp: the N_DATA_REQ
 * @np: private structure (locked)
 * @nc: network connection
 */
static int
nc_discon_req(queue_t *q, mblk_t *mp, struct np *np, struct nc *nc)
{
	N_discon_req_t *p;
	int err;
	struct sockaddr res;
	struct sockaddr_in *RES_buffer = (struct sockaddr_in *) &res;
	caddr_t res_ptr = (caddr_t) &res;
	size_t res_len;
	mblk_t *dp;
	size_t dlen;
	np_ulong state;

	err = NNOTSUPPORT;
	if (unlikely(nc->SERV_type == N_CLNS))
		goto error;
	err = -EMSGSIZE;
	if (unlikely(!MBLKIN(mp, sizeof(nc), sizeof(*p))))
		goto error;
	p = (typeof(p)) mp->b_rtpr;
	dassert(p->PRIM_type == N_DISCON_REQ);
	err = NOUTSTATE;
	if (unlikely(np->SERV_type != N_CONS))
		goto error;
	if (unlikely(nc_not_state(nc, NSM_CONNECTED)))
		goto error;
	if ((res_len = p->RES_length)) {
		err = -EMSGSIZE;
		if (unlikely(!MBLKIN(mp, p->RES_offset, p->RES_length)))
			goto error;
		err = NBADADDR;
		if (unlikely(res_len < sizeof(*RES_buffer)))
			goto error;
		if (unlikely(res_len > sizeof(res)))
			goto error;
		if (unlikely(res_len % sizeof(*RES_buffer) != 0))
			goto error;
		bcopy(mp->b_rptr + p->RES_offset, res_ptr, res_len);
		if (unlikely((RES_buffer[0].sin_family != AF_INET && RES_buffer[0].sin_family != 0)))
			goto error;
		if (unlikely(RES_buffer[0].sin_port == 0))
			goto error;
	}
	if ((dp = mp->b_cont) != NULL) {
		err = NBADDATA;
		if (unlikely((dlen = msgsize(mp)) <= 0 || dlen > np->info.DDATA_size))
			goto error;
	}
	state = nc_get_state(np, nc);
	err = NBADSEQ;
	if (p->SEQ_number != 0) {
		if (unlikely(state != NS_WRES_CIND))
			goto error;
		if (unlikely((SEQ_number = nc_seq_check(np, p->SEQ_number)) == NULL))
			goto error;
	} else {
		if (unlikely(state == NS_WRES_CIND))
			goto error;
	}
	err = NOUTSTATE;
	switch (state) {
	case NS_WCON_CREQ:
		state = NS_WACK_DREQ6;
		break;
	case NS_WRES_CIND:
		state = NS_WACK_DREQ7;
		break;
	case NS_DATA_XFER:
		state = NS_WACK_DREQ9;
		break;
	case NS_WCON_RREQ:
		state = NS_WACK_DREQ10;
		break;
	case NS_WRES_RIND:
		state = NS_WACK_DREQ11;
		break;
	default:
		goto error;
	}
	return nc_ok_ack(q, mp, np, nc, N_DISCON_REQ);
      error:
	return nc_error_ack(q, mp, np, nc, N_DISCON_REQ, err);

}

/** nc_info_req - process N_DATA_REQ primitive
  * @q: active queue
  * @mp: the N_DATA_REQ
  * @np: private structure (locked)
  * @nc: network connection
  */
static int
nc_info_req(queue_t *q, mblk_t *mp, struct np *np, struct nc *nc)
{
	N_info_req_t *p;
	int err;

	err = -EMSGSIZE;
	if (unlikely(!MBLKIN(mp, sizeof(nc), sizeof(*p))))
		goto error;
	p = (typeof(p)) (mp->b_rptr + sizeof(nc));
	dassert(p->PRIM_type == N_INFO_REQ);
	return nc_info_ack(q, mp, np, nc);
      error:
	return nc_error_reply(q, mp, np, nc, N_INFO_ACK, err);
}

/** nc_bind_req - process N_DATA_REQ primitive
  * @q: active queue
  * @mp: the N_DATA_REQ
  * @np: private structure (locked)
  * @nc: network connection
  *
  * It is not possible to bind more than one address to a Stream.  Each address is formatted as a
  * sockaddr_in up to the size of sockaddr_storage (which is 8 addresses on Linux).  If the
  * DEFAULT_DEST flag is set, then the port number is insignificant (and should be coded zero) or an
  * address is not provided.  When DEFAULT_DEST is not specified, the port number is significant.
  * Once bound, all packets that arrive for the specified address(es) and port numebr combination(s)
  * are delivered to the bound stream.  For DEFAULT_DEST Streams, all packets that arrive for the
  * specified address(es) that have port numbers that are bound to no other address will be
  * delivered to the DEFAULT_DEST Stream.  DEFAULT_DEST Streams that are bound with no address (or
  * to a wilcard address) will have all packets delivered that are bound to no ther address or port
  * number combination.
  *
  * Protocol ids must always be specified.  Currently we don't allow binding to more that one
  * protocol id, but, instead of generating an error, we simply bind to the first protocol id
  * specified and ignore the reset.  We will only return the first protocol id in the bind ack.
  *
  * There are two ways of specifying the address: a specific user-assigned IP address and port
  * number, and a provider-assigned IP address and port number.  The user-assigned IP address and
  * port number is a single sockaddr_in structure with specified IP address and port number.  The
  * provider-assigned IP address and port number is a list (one or more) IP addresses in order of
  * preference with a zero port number.  The provider will assign the port number to the first
  * address that has one available and return the selected address from the list and the assigned
  * port number in the N_BIND_ACK.
  *
  * Whether the bind is connectionless or connection-oriented must be specified using the
  * BIND_flags.  The default for NPI-RTP is connection-oriented.  Connection oriented interfaces do
  * not accept packets after the bind when CONIND_number is zero.  To accept packets after the bind
  * for transferring early media, there are two approaches: set CONIND_number to non-zero and
  * process N_CONN_IND generated when packets arrive, or mark the interface as connectionless (using
  * the DEFAULT_DEST flag) and process N_UNITDATA_IND messages.  Both forms of interface can later
  * be connected using N_CONN_REQ or by accepting N_CONN_IND messages with N_CONN_RES.
  */
static int
nc_bind_req(queue_t *q, mblk_t *mp, struct np *np, struct nc *nc)
{
	N_bind_req_t *p;
	int err;

	struct sockaddr add;
	struct sockaddr_in *ADDR_buffer = (struct sockaddr_in *) &add;
	caddr_t add_ptr;
	size_t add_len;
	unsigned char *PROTOID_buffer;
	caddr_t pro_ptr;
	size_t pro_len;
	np_ulong SERV_type, CONIND_number;

	err = -EMSGSIZE;
	if (unlikely(!MBLKIN(mp, sizeof(nc), sizeof(*p))))
		goto error;
	p = (typeof(p)) (mp->b_rptr + sizeof(nc));
	dassert(p->PRIM_type == N_BIND_REQ);
	err = NOUTSTATE;
	if (unlikely(nc_get_state(np, nc) != NS_UNBND))
		goto error;
	np_set_state(np, NS_WACK_BREQ);
	/* The only valid bind flag is TOKEN_REQUEST. */
	err = NBADFLAG;
	if (unlikely((p->BIND_flags & ~(TOKEN_REQUEST | DEFAULT_DEST)) != 0))
		goto error;
	if ((p->BIND_flags & DEFAULT_DEST) || p->CONIND_number != 0) {
		/* When DEFAULT_DEST is specified, the interface is always considered connectionless.  Each
		   protocol id is permitted multiple DEFAULT_DEST interface (one for each IP address and
		   port, and one for the wildcard IP address and port). DEFAULT_DEST interfaces are only
		   considered when there is no match for any other connectionless bound interface that maches 
		   the destination adress. */
		SERV_type = N_CLNS;
		CONIND_number = 0;
	} else {
		SERV_type = N_CONS;
		CONIND_number = p->CONIND_number;
	}
	err = NNOPROTOID;
	if (unlikely(!MBLKIN(mp, p->PROTOID_offset, p->PROTOID_length)))
		goto error;
	if (unlikely(p->PROTOID_length = 0))
		goto error;
	PROTOID_buffer = (unsigned char *) (mp->b_rptr + p->PROTOID_offset);
	pro_ptr = (caddr_t) PROTOID_buffer;

	err = NBADADDR;
	if (unlikely(!MBLKIN(mp, p->ADDR_offset, p->ADDR_length)))
		goto error;
	if ((add_len = p->ADDR_length)) {
		int add_num;

		if (unlikely(add_len < sizeof(*ADDR_buffer)))
			goto error;
		if (unlikely(add_len > sizeof(add)))
			goto error;
		if (unlikely(add_len % sizeof(*ADDR_buffer) != 0))
			goto error;
		add_num = add_len / sizeof(*ADDR_buffer);
		/* avoid alignment problems */
		bcopy(mp->b_rptr + p->ADDR_offset, add_ptr, add_len);
		if (unlikely(ADDR_buffer[0].sin_family != AF_INET && ADDR_buffer[0].sin_family != 0))
			goto error;
		if (add_num == 1
		    && (ADDR_buffer[0].sin_addr.s_addr == INADDR_ANY || ADDR_buffer[0].sin_port == 0))
			SERV_type = N_CONS;
	} else {
		add_len = sizeof(struct sockaddr_in);
		ADDR_buffer[0].sin_familiy = AF_INET;
		ADDR_buffer[0].sin_addr.s_addr = INADDR_ANY;
		ADDR_buffer[0].sin_port = 0;
		SERV_type = N_CONS;
	}
	/* cannot specify address if default listener */
	if (unlikely((p->BIND_flags & DEFAULT_LISTENER)
		     && (ADDR_buffer[0].sin_port != 0 || ADDR_buffer[0].sin_addr.s_addr != INADDR_ANY)))
		goto error;
	/* cannot listen on wildcard port number */
	if (unlikely(p->CONIND_number > 0 && ADDR_buffer[0].sin_port == 0))
		goto error;
	/* cannot specify port if default destination */
	if (unlikely(p->BIND_flags & DEFAULT_DEST) && ADDR_buffer[0].sin_port != 0)
		goto error;
	return nc_bind_ack(q, mp, np, nc, SERV_type, CONIND_number, p->BIND_flags, add_ptr, add_len, pro_ptr,
			   pro_len);
      error:
	return nc_error_reply(q, mp, np, nc, N_BIND_REQ, err);
}

/** nc_unbind_req - process N_DATA_REQ primitive
  * @q: active queue
  * @mp: the N_DATA_REQ
  * @np: private structure (locked)
  * @nc: network connection
  */
static int
nc_unbind_req(queue_t *q, mblk_t *mp, struct np *np, struct nc *nc)
{
	N_unbind_req_t *p;
	int err;

	err = -EMSGSIZE;
	if (unlikely(!MBLKIN(mp, sizeof(nc), sizeof(*p))))
		goto error;
	p = (typeof(p)) (mp->b_rptr + sizeof(nc));
	dassert(p->PRIM_type == N_UNBIND_REQ);
	err = NOUTSTATE;
	if (unlikely(nc_get_state(np, nc) != NS_IDLE))
		goto error;
	nc_set_state(np->oq, np, nc, NS_WACK_UREQ);
	return nc_ok_ack(q, mp, np, nc, N_UNBIND_REQ);
      error:
	return nc_error_reply(q, mp, np, nc, N_UNBIND_REQ, err);
}

/** nc_optmgmt_req - process N_DATA_REQ primitive
  * @q: active queue
  * @mp: the N_DATA_REQ
  * @np: private structure (locked)
  * @nc: network connection
  */
static int
nc_optmgmt_req(queue_t *q, mblk_t *mp, struct np *np, struct nc *nc)
{
	N_optmgmt_req_t *p;
	int err;
	union N_qos_types qos, *QOS_buffer = &qos;
	caddr_t qos_ptr = (caddr_t) &qos;
	size_t qos_len;

	err = -EMSGSIZE;
	if (unlikely(!MBLKIN(mp, sizeof(nc), sizeof(*p))))
		goto error;
	p = (typeof(p)) (mp->b_rptr + sizeof(nc));
	dassert(p->PRIM_type == N_OPTMGMT_REQ);
	if (unlikely(nc_get_state(np, nc) == NS_IDLE))
		nc_set_state(np->oq, np, nc, NS_WACK_OPTREQ);
	err = NBADOPT;
	if (unlikely(!MBLKIN(mp, p->QOS_offset, p->QOS_length)))
		goto error;
	if ((qos_len = p->QOS_length)) {
		err = NBADOPT;
		if (qos_len < sizeof(QOS_buffer->n_qos_type))
			goto error;
		if (qos_len > sizeof(*QOS_buffer))
			goto error;
		err = NBADQOSTYPE;
		bcopy(mp->b_rptr + p->QOS_offset, qos_ptr, qos_len);
		switch (QOS_buffer->n_qos_type) {
		case N_QOS_SEL_INFO_IP:
			if (qos_len != sizeof(QOS_buffer->ip.n_qos_sel_info))
				goto error;
			break;
		case N_QOS_SEL_CONN_IP:
			if (qos_len != sizeof(QOS_buffer->ip.n_qos_sel_conn))
				goto error;
			break;
		case N_QOS_SEL_UD_IP:
			if (qos_len != sizeof(QOS_buffer->ip.n_qos_sel_ud))
				goto error;
			break;
		case N_QOS_RANGE_INFO_IP:
			if (qos_len != sizeof(QOS_buffer->ip.n_qos_range_info))
				goto error;
			break;
		case N_QOS_SEL_INFO_UDP:
			if (qos_len != sizeof(QOS_buffer->udp.n_qos_sel_info))
				goto error;
			break;
		case N_QOS_SEL_CONN_UDP:
			if (qos_len != sizeof(QOS_buffer->udp.n_qos_sel_conn))
				goto error;
			break;
		case N_QOS_SEL_UD_UDP:
			if (qos_len != sizeof(QOS_buffer->udp.n_qos_sel_ud))
				goto error;
			break;
		case N_QOS_RANGE_INFO_UDP:
			if (qos_len != sizeof(QOS_buffer->udp.n_qos_range_info))
				goto error;
			break;
		case N_QOS_SEL_INFO_RTP:
			if (qos_len != sizeof(QOS_buffer->rtp.n_qos_sel_info))
				goto error;
			break;
		case N_QOS_SEL_CONN_RTP:
			if (qos_len != sizeof(QOS_buffer->rtp.n_qos_sel_conn))
				goto error;
			break;
		case N_QOS_SEL_UD_RTP:
			if (qos_len != sizeof(QOS_buffer->rtp.n_qos_sel_ud))
				goto error;
			break;
		case N_QOS_RANGE_INFO_RTP:
			if (qos_len != sizeof(QOS_buffer->rtp.n_qos_range_info))
				goto error;
			break;
		default:
			goto error;
		}
	}
	if (p->OPTMGMT_flags & DEFAULT_RC_SEL)
		np->i_flags |= IP_FLAG_DEFAULT_RC_SEL;
	else
		np->i_flags &= ~IP_FLAG_DEFAULT_RC_SEL;
	return nc_ok_ack(q, mp, np, rc, N_OPTMGMT_REQ);
      error:
	return nc_error_reply(q, mp, np, rc, N_OPTMGMT_REQ, err);
}

/** nc_datack_req - process N_DATA_REQ primitive
  * @q: active queue
  * @mp: the N_DATA_REQ
  * @np: private structure (locked)
  * @nc: network connection
  *
  * Note that requesting the NS-User to provide data acknoweldgements using the N_DATACK_REQ is one
  * mechanism that can be used for better flow control at the hook end.  Because we have many
  * virtual interfaces on the same Stream, it is possible for the noisiest channel to overwhelm the
  * other channels and cause indefinite postponement.  This mechanism can be used instead to limit
  * the maximum number of unacknowleged N_DATA_INDs.
  */
static int
nc_datack_req(queue_t *q, mblk_t *mp, struct np *np, struct nc *nc)
{
	N_datack_req_t *p;
	int err;

	err = -EMSGSIZE;
	if (unlikely(!MBLKIN(mp, sizeof(nc), sizeof(*p))))
		goto error;
	p = (typeof(p)) (mp->b_rptr + sizeof(nc));
	dassert(p->PRIM_type == N_DATACK_REQ);
	err = NNOTSUPPORT;
	if (unlikely(nc->SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(nc->SERV_type != N_CONS))
		goto error;
	/* Note: If the interface is in the NS_IDLE state when the provider receives the N_DATACK_REQ
	   primitive, the the NS provider should discard the request without generating a fatal error. */
	err = 0;
	if (unlikely(nc_get_state(np, nc) == NS_IDLE))
		goto error;
	err = NOUTSTATE;
	if (unlikely(nc_not_state(nc, NSM_CONNECTED)))
		goto error;
	/* Note: If the NS provider had no knowleged of a previous N_DATA_IND with the receipt confirmation
	   flag set, then the NS provider should ignore the request without generating a fatal error. */
	if (unlikely(nc->datacks <= 0))
		goto discard;
	return nc_datack(q, mp, np, nc);
      error:
	return nc_error_reply(q, mp, np, nc, N_DATACK_REQ, err);
}

/** nc_reset_req - process N_DATA_REQ primitive
  * @q: active queue
  * @mp: the N_DATA_REQ
  * @np: private structure (locked)
  * @nc: network connection
  */
static int
nc_reset_req(queue_t *q, mblk_t *mp, struct np *np, struct nc *nc)
{
	N_reset_req_t *p;
	int err, state;

	err = -EMSGSIZE;
	if (unlikely(!MBLKIN(mp, sizeof(nc), sizeof(*p))))
		goto error;
	p = (typeof(p)) (mp->b_rptr + sizeof(nc));
	dassert(p->PRIM_type == N_RESET_REQ);
	err = NNOTSUPPORT;
	if (unlikely(nc->SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(nc->SERV_type != N_CONS))
		goto error;
	/* Note: when the interface is in the NS_IDLE state when the provider receives the N_RESET_REQ
	   primitive, then the NS provider should discard the message without generating and error. */
	err = 0;
	state = nc_get_state(np, nc);
	if (unlikely(state == NS_IDLE))
		goto error;
	err = NOUTSTATE;
	if (unlikely(state == NS_WCON_RREQ))
		goto error;
	if (unlikely(nc_not_state(nc, NSM_CONNECTED)))
		goto error;
	nc_set_state(np->oq, np, nc, NS_WCON_RREQ);
	/* FIXME: we should really reset the number of outstanding data acknowledgemeents. */
	return nc_reset_con(q, mp, np, nc);
      error:
	return nc_error_reply(q, mp, np, nc, N_RESET_REQ, err);

}

/** nc_reset_res - process N_DATA_REQ primitive
  * @q: active queue
  * @mp: the N_DATA_REQ
  * @np: private structure (locked)
  * @nc: network connection
  *
  * When a reset indication is sent to the user, we expect a reset response which is acknowledged.
  * The reason is, only one outstanding reset indication per destination per RESET_reason is issued.
  * No additional reset indication will be issued until a response is received.  Because of this,
  * reset indications are expedited (band 2).
  */
static int
nc_reset_res(queue_t *q, mblk_t *mp, struct np *np, struct nc *nc)
{
	N_reset_res_t *p;
	int err;

	err = -EMSGSIZE;
	if (unlikely(!MBLKIN(mp, sizeof(nc), sizeof(*p))))
		got error;

	p = (typeof(p)) (mp->b_rptr + sizeof(nc));
	dassert(p->PRIM_type == N_RESET_RES);
	err = NNOTSUPPORT;
	if (unlikely(nc->SERV_type == N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (unlikely(nc->SERV_type != N_CONS))
		goto error;
	if (unlikely(nc_get_state(np, nc) != NS_WRES_RIND))
		goto error;
	nc_set_state(np->oq, np, nc, NS_WACK_RRES);
	return nc_ok_ack(q, mp, np, nc, N_RESET_RES);
      error:
	return nc_error_reply(q, mp, np, nc, N_RESET_RES, err);
}

/** nc_other_prim - handle wrong primitive on queue
  * @q: active queue
  * @mp: the primitive
  * @np: private structure (locked)
  * @nc: network connection
  * @prim: the primitive type
  */
noinline fastcall __unlikely int
nc_other_prim(queue_t *q, mblk_t *mp, struct np *np, struct nc *nc, np_ulong prim)
{
	freemsg(mp);
	switch (prim) {
	case N_CONN_IND:	/* Incoming connection indication */
	case N_CONN_CON:	/* Connection established */
	case N_DISCON_IND:	/* NC disconnected */
	case N_DATA_IND:	/* Incoming connection-mode data indication */
	case N_EXDATA_IND:	/* Incoming expedited data indication */
	case N_INFO_ACK:	/* Information Acknowledgement */
	case N_BIND_ACK:	/* NS User bound to network address */
	case N_ERROR_ACK:	/* Error Acknowledgement */
	case N_OK_ACK:		/* Success Acknowledgement */
	case N_UNITDATA_IND:	/* Connection-less data receive indication */
	case N_UDERROR_IND:	/* UNITDATA Error Indication */
	case N_DATACK_IND:	/* Data acknowledgement indication */
	case N_RESET_IND:	/* Incoming NC reset request indication */
	case N_RESET_CON:	/* Reset processing complete */
		/* wrong direction */
		mi_strlog(q, STRLOGERR, SL_ERROR, "%s in wrong direction", np_primname(prim));
		break;
	default:
		/* unrecognized primitive */
		mi_strlog(q, STRLOGERR, SL_ERROR, "Unrecognized primitive %d", (int) prim);
		break;
	}
	return (0);
}

/*
 *  STREAMS MESSAGE HANDLING
 */

/**
 * np_w_proto - process an M_PROTO, M_PCPROTO message on the write queue
 * @q: active queue in queue pair (write queue)
 * @mp: the M_PROTO, M_PCPROTO message to process
 *
 * These are normal N-primitives written from the upper layer protocol.
 */
STATIC INLINE fastcall __hot_put int
np_w_proto(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	int err = -EPROTO;
	np_long prim = 0;

	if (unlikely(!MBLKIN(mp, 0, sizeof(prim))))
		return np_w_proto_err(q, mp, np);

	prim = *(typeof(prim) *) mp->b_rptr;

	np_save_total_state(np);	/* checkpoint */

	if (likely((1 << prim) & ((1 << N_DATA_REQ) | (1 << N_EXDATA_REQ) | (1 << N_UNITDATA_REQ)))) {
#ifndef _OPTIMIZE_SPEED
		mi_strlog(q, STRLOGDA, SL_TRACE, "-> %s", np_primname(prim));
#endif
	} else {
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> %s", np_primname(prim));
	}

	switch (__builtin_expect(prim, N_DATA_REQ)) {
	case N_DATA_REQ:	/* Connection-Mode data transfer request */
		err = np_data_req(q, mp, np);
		break;
	case N_UNITDATA_REQ:	/* Connection-less data send request */
		err = np_unitdata_req(q, mp, np);
		break;
	case N_EXDATA_REQ:	/* Expedited data request */
		err = np_exdata_req(q, mp, np);
		break;
	case N_CONN_REQ:	/* NC request */
		err = np_conn_req(q, mp, np);
		break;
	case N_CONN_RES:	/* Accept previous connection indication */
		err = np_conn_res(q, mp, np);
		break;
	case N_DISCON_REQ:	/* NC disconnection request */
		err = np_discon_req(q, mp, np);
		break;
	case N_INFO_REQ:	/* Information Request */
		err = np_info_req(q, mp, np);
		break;
	case N_BIND_REQ:	/* Bind a NS user to network address */
		err = np_bind_req(q, mp, np);
		break;
	case N_UNBIND_REQ:	/* Unbind NS user from network address */
		err = np_unbind_req(q, mp, np);
		break;
	case N_OPTMGMT_REQ:	/* Options Management request */
		err = np_optmgmt_req(q, mp, np);
		break;
	case N_DATACK_REQ:	/* Data acknowledgement request */
		err = np_datack_req(q, mp, np);
		break;
	case N_RESET_REQ:	/* NC reset request */
		err = np_reset_req(q, mp, np);
		break;
	case N_RESET_RES:	/* Reset processing accepted */
		err = np_reset_res(q, mp, np);
		break;
	default:
		err = np_other_prim(q, mp, np, prim);
		break;
	}
      error:
	if (err < 0) {
#ifndef _TEST
		/* not so seldom() during conformance suite testing */
		seldom();
#endif
		np_restore_total_state(np);
		/* The put and service procedure do not recognize all errors.  Sometimes we return an error
		   to here just to restore the previous state. */
		switch (err) {
		case -EBUSY:	/* flow controlled */
		case -EAGAIN:	/* try again */
		case -ENOMEM:	/* could not allocate memory */
		case -ENOBUFS:	/* could not allocate an mblk */
		case -EDEADLK:	/* could not lock private structure */
		case -EOPNOTSUPP:	/* primitive not supported */
			err = np_error_reply(q, prim, err);
			break;
		case -EPROTO:
			err = np_error_fatal(q, -EPROTO);
			break;
		default:
			/* ignore all other errors */
			err = 0;
			break;
		}
	}
	return (err);
}

/** np_w_ctl - process a M_CTL, M_PCCTL message on the write queue
  * @q: active queue in queue pair (write queue)
  * @mp: the M_CTL or M_PCCTL message to process
  * @np: private structure (locked)
  *
  * This is the inter-module interface that mimic the user (M_(PC)PROTO) interface; however, it uses
  * a virtual interface identifier (pointer) prefixed to every primitive and appended to some
  * (N_BIND_ACK).  These identifiers allow multiple virtual NPI interface instances to run over a
  * single Stream instance.  This greatly eases management and scalability in user-space of the
  * STREAMS framework.  It was invented for RTP/RTCP processing in the OpenSS7 stack.
  */
STATIC INLINE fastcall __hot_put int
np_w_ctl(queue_t *q, mblk_t *mp, struct np *np)
{
	struct nc *nc = NULL;
	np_long prim = 0;
	int err = 0;

	if (unlikely(!MBLKIN(mp, sizeof(nc), sizeof(prim))))
		return __np_w_ctl_err(q, mp, np);

	prim = *(typeof(prim) *) (mp->b_rptr + sizeof(nc));

	if (unlikely((nc = np_tag_lookup(q, mp, np, prim, &err)) == NULL && err != 0))
		return nc_error_reply(q, mp, np, nc, prim, NBADTOKEN);

	/* Note that if @nc is NULL, this is a no-op. */
	nc_save_total_state(np, nc);

	if (likely((1 << prim) & ((1 << N_DATA_REQ) | (1 << N_EXDATA_REQ) | (1 << N_UNITDATA_REQ)))) {
#ifndef _OPTIMIZE_SPEED
		mi_strlog(q, STRLOGDA, SL_TRACE, "-> %s", np_primname(prim));
#endif				/* _OPTIMIZE_SPEED */
	} else {
		mi_strlog(q, STRLOGRX, SL_TRACE, "-> %s", np_primname(prim));
	}

	switch (__builtin_expect(prim, N_DATA_REQ)) {
	case N_DATA_REQ:	/* Connection-Mode data transfer request */
		err = nc_data_req(q, mp, np, nc);
		break;
	case N_UNITDATA_REQ:	/* Connection-less data send request */
		err = nc_unitdata_req(q, mp, np, nc);
		break;
	case N_EXDATA_REQ:	/* Expedited data request */
		err = nc_exdata_req(q, mp, np, nc);
		break;
	case N_CONN_REQ:	/* NC request */
		err = nc_conn_req(q, mp, np, nc);
		break;
	case N_CONN_RES:	/* Accept previous connection indication */
		err = nc_conn_res(q, mp, np, nc);
		break;
	case N_DISCON_REQ:	/* NC disconnection request */
		err = nc_discon_req(q, mp, np, nc);
		break;
	case N_INFO_REQ:	/* Information request */
		err = nc_info_req(q, mp, np, nc);
		break;
	case N_BIND_REQ:	/* Bind a NS user to network address */
		err = nc_bind_req(q, mp, np, nc);
		break;
	case N_UNBIND_REQ:	/* Unbind NS user from network address */
		err = nc_unbind_req(q, mp, np, nc);
		break;
	case N_OPTMGMT_REQ:	/* Options Management request */
		err = nc_optmgmt_req(q, mp, np, nc);
		break;
	case N_DATACK_REQ:	/* Data acknowledgement request */
		err = nc_datack_req(q, mp, np, nc);
		break;
	case N_RESET_REQ:	/* NC reset request */
		err = nc_reset_req(q, mp, np, nc);
		break;
	case N_RESET_RES:	/* Reset processing accepted. */
		err = nc_reset_res(q, mp, np, nc);
		break;
	default:
		err = nc_other_prim(q, mp, np, nc);
		break;
	}
	if (err < 0)
		nc_restore_total_state(np, nc);
	return (err);
}

/** np_wp_data - process M_DATA message
  * @q: active queue in pair (write queue)
  * @mp: the M_DATA message
  * @np: private structure (locked)
  *
  * Note that data delivered in M_DATA message blocks only can only belong to the base Stream (real
  * NPI interface) and cannot be used for the inter-module multi-stream interfaces.  These
  * interfaces require that the M_CTL message with a N_DATA_REQ primitive be used.
  */
noinline fastcall __unlikely int
np_w_data(queue_t *q, mblk_t *mp, struct np *np)
{
	return ne_write_req(q, mp, np);
}

/** np_w_other - proces other message
  * @q: active queue in pair (write queue)
  * @mp: the message
  * @type: the message type
  */
noinline fastcall __unlikely int
np_w_other(queue_t *q, mblk_t *mp, int type)
{
	freemsg(mp);
	mi_strlog(q, STRLOGERR, SL_ERROR, "Unsupported block type %d", type);
	return (0);
}

/** np_w_ioctl - M_IOCTL handling
  * @q: active queue in queue pair (write queue)
  * @mp: the message
  *
  * This NPI-UDP provider does not support any input-output controls and, therefore, all
  * input-output controls are negatively acknowledged.
  */
noinline fastcall __unlikely int
np_w_ioctl(queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}

/** np_w_iocdata - M_IOCDATA handling
  * @q: active queue in queue pair (write queue)
  * @mp: the message
  *
  * This NPI-UDP provider does not support any input-output controls and, therefore, all
  * input-output controls are negatively acknowledged.
  */
noinline fastcall __unlikely int
np_w_iocdata(queue_t *q, mblk_t *mp)
{
	mi_copy_done(q, mp, EINVAL);
	return (0);
}

/** np_w_flush - M_FLUSH handling
  * @q: active queue in queue pair (write queue)
  * @mp: the M_FLUSH message
  *
  * This is a canonical driver flush procedure.
  */
noinline streamscall int
np_w_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHW) {
		if (unlikely(mp->b_rptr[0] & FLUSHBAND))
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHDATA);
		mp->b_rptr[0] &= ~FLUSHW;
	}
	if (mp->b_rptr[0] & FLUSHR) {
		if (unlikely(mp->b_rptr[0] & FLUSHBAND))
			flushband(RD(q), mp->b_rptr[1], FLUSHDATA);
		else
			flushq(RD(q), FLUSHDATA);
		qreply(q, mp);
		return (0);
	}
	freemsg(mp);
	return (0);
}

/**
 * np_r_other - process other message
 * @q: active queue in pair (read queue)
 * @mp: the message
 * @type: message type
 */
noinline fastcall __unlikely int
np_r_other(queue_t *q, mblk_t *mp, int type)
{
	freemsg(mp);
	mi_strlog(q, STRLOGERR, SL_ERROR, "Unsupported block type %d", type);
	return (0);
}

/**
 * np_r_data - process M_DATA message
 * @q: active queue in queue pair (read queue)
 * @mp: the M_DATA message
 *
 * M_DATA messages are placed to the read queue by the Linux IP np_v4_rcv() callback.  The message
 * contains a complete IP datagram starting with the IP header.  What needs to be done is to convert
 * this to an upper layer indication and deliver it upstream.
 */
STATIC INLINE fastcall __hot_in int
np_r_data(queue_t *q, mblk_t *mp, struct np *np)
{
	int rtn;

	switch (np->info.SERV_type) {
	case N_CLNS:
		switch (np_get_state(np)) {
		case NS_IDLE:
			rtn = np_unitdata_ind(q, mp);
			break;
		default:
			rtn = QR_DONE;
			break;
		}
		break;
	case N_CONS:
		switch (np_get_state(np)) {
		case NS_DATA_XFER:
		case NS_WCON_CREQ:
		case NS_WACK_RRES:
		case NS_WRES_RIND:
		case NS_WCON_RREQ:
		case NS_WACK_DREQ6:
		case NS_WACK_DREQ7:
		case NS_WACK_DREQ9:
		case NS_WACK_DREQ10:
		case NS_WACK_DREQ11:
			rtn = np_data_ind(q, mp);
			break;
		case NS_IDLE:
			if (np->CONIND_number == 0) {
				rtn = QR_DONE;
				break;
			}
			/* fall through */
		case NS_WRES_CIND:
		case NS_WACK_CRES:
			rtn = np_conn_ind(q, mp);
			break;
		default:
			/* ignore it now */
			rtn = QR_DONE;
			break;
		}
		break;
	default:
		swerr();
		rtn = QR_DONE;
		break;
	}
	return (rtn);
}

/**
 * np_r_ctl - process M_CTL message
 * @q: active queue in queue pair (read queue)
 * @mp: the M_CTL message
 *
 * M_CTL messages are placed to the read queue by the Linux IP np_v4_err() callback.  The message
 * contains a complete ICMP datagram starting with the IP header.  What needs to be done is to
 * convert this to an upper layer indication and deliver it upstream.
 */
STATIC INLINE fastcall __hot_get int
np_r_ctl(queue_t *q, mblk_t *mp, struct np *np)
{
	int rtn;

	switch (np->info.SERV_type) {
	case N_CLNS:
		switch (np_get_state(np)) {
		case NS_IDLE:
			rtn = np_uderror_ind_icmp(q, mp);
			break;
		default:
			rtn = QR_DONE;
			break;
		}
		break;
	case N_CONS:
		switch (np_get_state(np)) {
		case NS_DATA_XFER:
		case NS_WCON_CREQ:
		case NS_WACK_RRES:
		case NS_WCON_RREQ:
		case NS_WACK_DREQ6:
		case NS_WACK_DREQ7:
		case NS_WACK_DREQ9:
		case NS_WACK_DREQ10:
		case NS_WACK_DREQ11:
			rtn = np_reset_ind(q, mp);
			break;
		case NS_WRES_CIND:
		case NS_WACK_CRES:
			rtn = np_discon_ind_icmp(q, mp);
			break;
		default:
			/* ignore it now */
			rtn = QR_DONE;
			break;
		}
		break;
	default:
		swerr();
		rtn = QR_DONE;
		break;
	}
	return (rtn);
}

/** np_r_flush - process M_FLUSH message
  * @q: queue to flush
  * @mp: M_FLUSH message
  *
  * This is here for canonical reasons only.  We never put M_FLUSH messages onto the driver read queue.
  */
noinline streamscall int
np_r_flush(queue_t *q, mblk_t *mp)
{
	if (mp->b_rptr[0] & FLUSHR) {
		if (unlikely(mp->b_rptr[0] & FLUSHBAND))
			flushband(q, mp->b_rptr[1], FLUSHDATA);
		else
			flushq(q, FLUSHALL);
	}
	putnext(q, mp);
	return (0);
}

/*
 *  ====================================================================
 *  ====================================================================
 *  ====================================================================
 *  ====================================================================
 *  ====================================================================
 *  ====================================================================
 */

noinline fastcall int
np_w_put_locked(queue_t *q, mblk_t *mp, type)
{
	struct np *np;
	int rtn = -EDEADLK;

	if (likely((np = (typeof(np)) mi_trylock(q)) != NULL)) {
		switch (__builtin_expect(type, M_PCCTL)) {
		case M_PCCTL:
			rtn = np_w_ctl(q, mp, np);
			break;
		case M_PCPROTO:
			rtn = np_w_proto(q, mp, np);
			break;
		case M_IOCTL:
			rtn = np_w_ioctl(q, mp, np);
			break;
		case M_IOCDATA:
			rtn = np_w_iocdata(q, mp, np);
			break;
		case M_FLUSH:
			rtn = np_w_flush(q, mp);
			break;
		case M_PROTO:
		case M_CTL:
		case M_DATA:
			rtn = -EAGAIN;
			break;
		}
		mi_unlock((caddr_t) np);
	}
	return (rtn);
}

noinline fastcall int
np_w_put_slow(queue_t *q, mblk_t *mp, int type)
{
	switch (__builtin_expect(type, M_PCCTL)) {
	case M_PCPROTO:
	case M_PCCTL:
	case M_IOCTL:
	case M_IOCDATA:
		return np_w_put_locked(q, mp, type);
	case M_FLUSH:
		return np_w_flush(q, mp);
	case M_PROTO:
	case M_CTL:
	case M_DATA:
		return (-EAGAIN);
	default:
		return np_w_other(q, mp, type);
	}
}

STATIC INLINE fastcall __hot_put int
np_w_put(queue_t *q, mblk_t *mp)
{
	int type = DB_TYPE(mp);

	/* The only non-priority message that we expedite is M_IOCTL. */
	if (likely((1 << type) & ((1 << M_PROTO) | (1 << M_CTL) | (1 << M_DATA)))) {
		/* fast path */
		return (-EAGAIN);
	}
	/* slow path */
	return np_w_put_slow(q, mp, type);
}

noinline fastcall int
np_w_srv_slow(queue_t *q, mblk_t *mp, struct np *np, int type)
{
	switch (__builtin_expect(type, M_IOCTL)) {
	case M_PROTO:
	case M_PCPROTO:
		return np_w_proto(q, mp, np);
	case M_CTL:
	case M_PCCTL:
		return np_w_ctl(q, mp, np);
	case M_DATA:
	case M_HPDATA:
		return np_w_data(q, mp, np);
	case M_IOCTL:
		return np_w_ioctl(q, mp, np);
	case M_IOCDATA:
		return np_w_iocdata(q, mp, np);
	case M_FLUSH:
		return np_w_flush(q, mp);
	default:
		return np_w_other(q, mp, type);
	}
}

STATIC INLINE fastcall __hot_out int
np_w_srv(queue_t *q, mblk_t *mp, struct np *np)
{
	int type = DB_TYPE(mp);

	if (likely((1 << type) & ((1 << M_PROTO) | (1 << M_CTL) | (1 << M_DATA)))) {
		/* fast path */
		if (likely(type == M_CTL))
			return np_w_ctl(q, mp, np);
		if (likely(type == M_PROTO))
			return np_w_proto(q, mp, np);
		if (likely(type == M_DATA))
			return np_w_data(q, mp, np);
	}
	/* slow path */
	return np_w_srv_slow(q, mp, np, type);

}

noinline fastcall int
np_r_srv_slow(queue_t *q, mblk_t *mp, struct np *np, int type)
{
	switch (__builtin_expect(type, M_DATA)) {
	case M_DATA:
		return np_r_data(q, mp, np);
	case M_CTL:
		return np_r_ctl(q, mp, np);
	case M_FLUSH:
		return np_r_flush(q, mp);
	default:
		return np_r_other(q, mp, type);
	}
}

STATIC INLINE fastcall __hot_get int
np_r_srv(queue_t *q, mblk_t *mp, struct np *np)
{
	int type = DB_TYPE(mp);

	if (likely((1 << type) & ((1 << M_DATA) | (1 << M_CTL)))) {
		/* fast path */
		if (likely(type == M_DATA))
			return np_r_data(q, mp, np);
		if (likely(type == M_CTL))
			return np_r_ctl(q, mp, np);
	}
	/* slow path */
	return np_r_srv_slow(q, mp, np, type);
}

noinline fastcall int
np_r_put_slow(queue_t *q, mblk_t *mp, int type)
{
	switch (__builtin_expect(type, M_FLUSH)) {
	case M_DATA:
		return (-EAGAIN);
	case M_CTL:
		return (-EAGAIN);
	case M_FLUSH:
		return np_r_flush(q, mp);
	default:
		return np_r_other(q, mp, type);
	}
}

STATIC INLINE fastcall __hot_in int
np_r_put(queue_t *q, mblk_t *mp)
{
	int type = DB_TYPE(mp);

	if (likely((1 << type) & ((1 << M_DATA) | (1 << M_CTL)))) {
		/* fast path */
		return (-EAGAIN);
	}
	/* slow path */
	return np_r_put_slow(q, mp, type);
}

/** np_wput - process message from above
  * @q: the write queue
  * @mp: the message to put
  *
  * This is a canonical STREAMS put procedure.  It might be possible to expedite normal messages.
  * Priority messages must usually be expedited.  When the queue contains messages, normal messages
  * may not be expedited.  Messages that cannot be processed immediately must be placed back on the
  * queue.  Message that required an amount of processing (usually much more than a state change),
  * such as most data messages, should be processed out of the service procedure for performance.
  *
  * We want sheer capacity here and are not worried about the latency of low capacity Streams.
  */
streamscall __hot_put int
np_wput(queue_t *q, mblk_t *mp)
{
	if (likely(!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))
		   || np_w_put(q, mp) != 0)) {
		np_wstat.ms_acnt++;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;	/* must succeed */
			putq(q, mp);
		}
	}
	return (0);
}

/** np_wsrv - service the write queue
  * @q: the write queue to service
  *
  * This is a canonical STREAMS service procedure.  Messages are removed from the queue one at a
  * time with getq() and processed until a message cannot be processed (usually due to flow control)
  * or there are no messages left.  Any messages that cannot be processed are placed back on the
  * queue and the procedure returns.  Note that putbq() is guaranteed to succeed when it follows
  * getq() for the same queue and (unaltered) message.  putbq() is always guaranteed to succeed when
  * the band of the message is zero, because no allocation is required in that case.
  *
  * Note that for speed and to avoid lock contention, we acquire a lock on the private structure
  * before processing messages; that is, once for all messages instead of once for each message.  If
  * we fail to obtain the lock, this queue will be again scheduled when the lock holder releases the
  * lock.
  */
streamscall __hot_out int
np_wsrv(queue_t *q)
{
	struct np *np;

	prefetch(q->q_ptr);
	if (likely((np = (typeof(np)) mi_trylock(q)) != NULL)) {
		mblk_t *mp;

		while (likely((mp = getq(q)) != NULL)) {
			prefetch(mp);
			if (unlikely(np_w_srv(q, mp, np) != 0)) {
#ifndef _OPTIMIZE_SPEED
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;	/* must succeed */
#endif				/* _OPTIMIZE_SPEED */
					putbq(q, mp);
#ifndef _OPTIMIZE_SPEED
				}
#endif				/* _OPTIMIZE_SPEED */
				break;
			}
		}
		__np_w_wakeup(q, np);
		mi_unlock((caddr_t) np);
	}
	return (0);
}

/** np_rsrv - service the read queue
  * @q: the read queue to service
  *
  * This is a canonical STREAMS service procedure.  Messages are removed from the queue one at a
  * time with getq() and processed until a message cannot be processed (usually due to flow control)
  * or there are no messages left.  Any messages that cannot be processed are placed back on the
  * queue and the procedure returns.  Note that putbq() is guaranteed to succeed when it follows
  * getq() for the same queue and (unaltered) message.  putbq() is always guaranteed to succeed when
  * the band of the message is zero, because no allocation is required in that case.  It therefore
  * can only fail if we change up the band number and then place it back on the queue and there is
  * no qband structure for the selected band.  Of course, we never do anything so crazy in these
  * drivers, but the result would be a message buffer leak, so it is for safety.
  *
  * Note that for speed and to avoid lock contention, we acquire a lock on the private structure
  * before processing messages; that is, once for all messages instead of once for each message.  If
  * we fail to obtain the lock, this queue will be again scheduled when the lock holder releases the
  * lock.
  */
streamscall __hot_get int
np_rsrv(queue_t *q)
{
	struct np *np;

	prefetch(q->q_ptr);
	if (likely((np = (typeof(np)) mi_trylock(q)) != NULL)) {
		mblk_t *mp;

		while (likely((mp = getq(q)) != NULL)) {
			prefetch(mp);
			if (unlikely(np_r_srv(q, mp, np) != 0)) {
#ifndef _OPTIMIZE_SPEED
				if (unlikely(!putbq(q, mp))) {
					mp->b_band = 0;	/* must succeed */
#endif				/* _OPTIMIZE_SPEED */
					putbq(q, mp);
#ifndef _OPTIMIZE_SPEED
				}
#endif				/* _OPTIMIZE_SPEED */
				break;
			}
		}
		__np_r_wakeup(q, np);
		mi_unlock((caddr_t) np);
	}
	return (0);
}

/** np_rput - process message from below
  * @q: the read queue
  * @mp: the message to put
  *
  * This is a canonical STREAMS put procedure.  It might be possible to expedite normal messages.
  * Priority messages must usually be expedited.  When the queue contains messages, normal messages
  * may not be expedited.  Messages that cannot be processed immediately must be placed back on the
  * queue.  Message that required an amount of processing (usually much more than a state change),
  * such as most data messages, should be processed out of the service procedure for performance.
  *
  * We want sheer capacity here and are not worried about the latency of low capacity Streams.
  */
streamscall __hot_in int
np_rput(queue_t *q, mblk_t *mp)
{
	if (likely(!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))
		   || np_r_put(q, mp) != 0)) {
		np_rstat.ms_acnt++;
		if (unlikely(!putq(q, mp))) {
			mp->b_band = 0;	/* must succeed */
			putq(q, mp);
		}
	}
	return (0);
}

/*
 *  =========================================================================
 *
 *  Bottom end Linux IP hooks.
 *
 *  =========================================================================
 */
/**
 * nc_lookup_conn - lookup network connection in the connection hashes
 * @proto: protocol (of received packet)
 * @daddr: destination address (of received packet)
 * @dport: destination port (of received packet)
 * @saddr: source address (of received packet)
 * @sport: source port (of received packet)
 *
 * The start of the right kind of code shaping up here.  We provide connection buckets that have
 * IP protocol number, source port and destination port as hash parameters.  Attached to the
 * connection bucket are all network connections with the same protocol number, source port and
 * destination port combination (but possibly different IP adresses).  These network connections
 * that are "owners" of the connection bucket must be traversed and checked for address matches.
 */
noinline fastcall __unlikely struct nc *
nc_lookup_conn(unsigned char proto, uint32_t daddr, uint16_t dport, uint32_t saddr, uint16_t sport)
{
	struct nc *result = NULL;
	int hiscore = -1;
	struct nc_chash_bucket *hp, *hp1, *hp2;
	unsigned long flags;

	hp1 = &nc_chash[nc_chashfn(proto, sport, dport)];
	hp2 = &nc_chash[nc_chashfn(proto, 0, 0)];

	hp = hp1;
	do {
		read_lock_str2(&hp->lock, flags);
		{
			register struct nc *nc;

			for (nc = hp->list; nc; nc = nc->cnext) {
				register int score = 0;
				register int i;

				/* only Streams in close to the correct state */
				if (nc_not_state(nc, (NSF_DATA_XFER | NSF_WRES_RIND)))
					continue;
				/* must match a bound protocol id */
				for (i = 0; i < nc->pnum; i++) {
					if (nc->protoids[i] != proto)
						continue;
					break;
				}
				if (i >= nc->pnum)
					continue;
				if (nc->sport != 0) {
					if (nc->sport != sport)
						continue;
					score++;
				}
				if (nc->dport != 0) {
					if (nc->dport != dport)
						continue;
					score++;
				}
				for (i = 0; i < nc->snum; i++) {
					if (nc->saddrs[i].addr == 0)
						break;
					if (nc->saddrs[i].addr != daddr)
						continue;
					score++;
					break;
				}
				if (i >= nc->snum)
					continue;
				for (i = 0; i < nc->dnum; i++) {
					if (nc->daddrs[i].addr == 0)
						break;
					if (nc->daddrs[i].addr != saddr)
						continue;
					score++;
					break;
				}
				if (i >= nc->dnum)
					continue;
				if (score > hiscore) {
					hiscore = score;
					if (result != NULL)
						nc_put(result);
					result = nc_get(nc);
				}
				if (score == 4)
					/* perfect match */
					break;
			}
		}
		read_unlock_str2(&hp->lock, flags);
	} while (hiscore < 4 && hp != hp2 && (hp = hp2));
	usual(result);
	return (result);
}

/**
 * nc_lookup_bind - lookup Stream in the bind/listening hashes
 * @proto:	protocol from IP header
 * @daddr:	destination address from IP header
 * @dport:	destination port from UDP header
 *
 * Note that an N_CLNS Stream can bind to either a port number or port number zero. An N_CONS Stream
 * can only bind listening to a non-zero port number, but can bind normal to a zero port number.
 *
 * N_CLNS Streams are always entered into the bind hashes against port number zero.  N_CONS Streams
 * are entered against their bound port number (which can be zero only if they are not listening).
 * N_CONS Streams that are not listening will not be matched.  Only the DEFAULT_LISTENER can be
 * bound listening against a zero port (and it requires a wildcard address).
 *
 * NOTICES: There are two types of Streams in the bind hashes, N_CONS and N_CLNS.  When lookup up
 * a Stream for a received packet, we are interested in any N_CLNS Stream that matches or any N_CONS
 * stream that is in the listening state that matches.
 */
STATIC INLINE fastcall __hot_in struct nc *
nc_lookup_bind(unsigned char proto, uint32_t daddr, unsigned short dport)
{
	struct nc *result = NULL;
	int hiscore = -1;
	struct nc_bhash_bucket *hp, *hp1, *hp2;
	unsigned long flags;

	hp1 = &nc_bhash[nc_bhashfn(proto, dport)];
	hp2 = &nc_bhash[nc_bhashfn(proto, 0)];

	hp = hp1;
	_ptrace(("%s: %s: proto = %d, dport = %d\n", DRV_NAME, __FUNCTION__, (int) proto,
		 (int) ntohs(dport)));
	do {
		read_lock_str2(&hp->lock, flags);
		{
			register struct nc *nc;

			for (nc = hp->list; nc; nc = nc->bnext) {
				register int score = 0;
				register int i;

				/* only listening N_CONS Streams and N_CLNS Streams */
				if (nc->CONIND_number == 0 && nc->SERV_type != N_CLNS)
					continue;
				/* only Streams in close to the correct state */
				if (nc_not_state(nc, (NSF_IDLE | NSF_WCON_CREQ)))
					continue;
				for (i = 0; i < nc->pnum; i++) {
					if (nc->protoids[i] != proto)
						continue;
					break;
				}
				if (i >= nc->pnum)
					continue;
				if (nc->bport != 0) {
					if (nc->bport != dport)
						continue;
					score++;
				}
				if (!(nc->BIND_flags & (DEFAULT_DEST | DEFAULT_LISTENER)))
					score++;
				for (i = 0; i < nc->bnum; i++) {
					if (nc->baddrs[i].addr == 0)
						break;
					if (nc->baddrs[i].addr != daddr)
						continue;
					score++;
					break;
				}
				if (i >= nc->bnum)
					continue;
				if (score > hiscore) {
					hiscore = score;
					if (result != NULL)
						nc_put(result);
					result = nc_get(nc);
				}
				if (score == 3)
					/* perfect match */
					break;
			}
		}
		read_unlock_str2(&hp->lock, flags);
	} while (hiscore < 2 && hp != hp2 && (hp = hp2));
	usual(result);
	return (result);
}

noinline fastcall __unlikely struct nc *
nc_lookup_common_slow(struct nc_prot_bucket *pp, uint8_t proto, uint32_t daddr, uint16_t dport,
		      uint32_t saddr, uint16_t sport)
{
	struct nc *result = NULL;

	if (likely(pp != NULL)) {
		if (likely(pp->corefs != 0))
			result = nc_lookup_conn(proto, daddr, dport, saddr, sport);
		if (result == NULL && (pp->corefs != 0 || pp->clrefs != 0))
			result = nc_lookup_bind(proto, daddr, dport);
	}
	return (result);
}

STATIC INLINE fastcall __hot_in struct nc *
nc_lookup_common(uint8_t proto, uint32_t daddr, uint16_t dport, uint32_t saddr, uint16_t sport)
{
	struct nc_prot_bucket *pp, **ppp;
	register struct nc *result;
	unsigned long flags;

	ppp = &nc_prots[proto];

	read_lock_str2(&nc_prot_lock, flags);
	if (likely((pp = *ppp) != NULL)) {
		if (likely(pp->corefs == 0)) {
			if (likely(pp->clrefs > 0)) {
				result = nc_lookup_bind(proto, daddr, dport);
			      done:
				read_unlock_str2(&nc_prot_lock, flags);
				return (result);
			}
		}
	}
	result = nc_lookup_common_slow(pp, proto, daddr, dport, saddr, sport);
	goto done;
}

/**
 * nc_lookup - lookup Stream by protocol, address and port.
 * @iph: IP header
 * @uh: UDP header
 *
 * The UDP header here is just to get port numbers in the right place.  All connection-oriented IP
 * protocols use port numbers in the same layout as UDP.  This header is ignored for connectionless
 * lookups if the Stream is not bound to a non-zero port number.
 *
 * There are two places to look: connection hash and bind hash.  Connection hashes and bind hashes
 * are only searched for connection-oriented Streams if there is a connection oriented Stream bound
 * to the protocol id.  Bind hashes are only searched for connectionless Streams if there is a
 * connectionless Stream bound to the protocol id.
 */
STATIC INLINE fastcall __hot_in struct nc *
nc_lookup(struct iphdr *iph, struct udphdr *uh)
{
	return nc_lookup_common(iph->protocol, iph->daddr, uh->dest, iph->saddr, uh->source);
}

/** nc_lookup_icmp - a little different from nc_lookup(), for ICMP messages
  * @iph: returned IP header
  * @len: length of returned message
  *
  * This needs to do a reverse lookup (where destination address and port are compared to source
  * address and port, and visa versa).
  */
noinline fastcall __unlikely struct nc *
nc_lookup_icmp(struct iphdr *iph, unsigned int len)
{
	struct udphdr *uh = (struct udphdr *) ((unsigned char *) iph + (iph->ihl << 2));

	if (len < (iph->ihl << 2) + 4)
		/* too short: don't have port numbers - ignore it */
		return (NULL);

	return nc_lookup_common(iph->protocol, iph->saddr, uh->source, iph->daddr, uh->dest);
}

/**
 * np_free - message block free function for mblks esballoc'ed from sk_buffs
 * @data: client data (sk_buff pointer in this case)
 */
STATIC streamscall __hot_get void
np_free(caddr_t data)
{
	struct sk_buff *skb = (typeof(skb)) data;

	dassert(skb != NULL);
	kfree_skb(skb);
	return;
}

#ifndef CHECKSUM_HW
#define CHECKSUM_HW CHECKSUM_COMPLETE
#endif

/**
 * np_v4_rcv - process a received IP packet
 * @skb: socket buffer containing IP packet
 *
 * This function is a callback function called by the Linux IP code when a packet is delivered to an
 * IP protocol number to which a Stream is bound.  For NPI-RTP, this is IPPROTO_UDP, however, the
 * possibility is there to permit a specialized IP protocol number for RTP (although one has not
 * been assigned by IANA yet).
 *
 * The NPI-RTP interface does not support broadcast or multicast addresses yet.  These are simply
 * not processed and are passed on to Linux UDP for further processing.  When the destination
 * address is a unicast address, look up the receiving IP interface based on the protocol number, IP
 * address and UDP port number.  If no receiving interface exists for a unicast packet, or if the
 * packet is a broadcast or multicast packet, pass the packet along to the next handler if any.  If
 * there is no next handler and the packet was not sent to any interface, generate an appropriate
 * ICMP error.
 *
 * Because our RTP port number assignments can easily conflict with underlying UDP assignments (as
 * IPPROTO_UDP is used as protocol id), there are several criteria that must be met to determine
 * that the packet is for us:
 *
 *	1. the packet is received on an even port number and contains a valid RTP header.
 *	2. the packet is receive don an odd port number and contains a valid RTCP header.
 *	3. the destination address of the packet matches a local bound address.
 *	4. the source address of the packet matches a specified connected address.
 *	5. the network connection is in the apporpriate state for accepting messages.
 *
 * When the criteria is not met, the packet is passed to the next handler.  Note that a connected
 * RTP/RTCP channel can coexist under this scheme with a normal Linux UDP socket form som other
 * protocol even using the same port numbers (but not the same headers).  Also not that the first
 * criteria are quick to check as the socket buffer and payload should be hot coming into the
 * receive function.
 *
 * If the receiving Stream is flow controlled, simply discard its copy of the IP packet.  Otherwise,
 * generate an (internal) M_DATA message and pass it to the Stream.  To avoid dropping messages, use
 * the N_DATA_IND, N_DATACK_REQ mechanism for connected interfaces to permit at least
 * 3 outstanding data indications.  This means that the maximum backlog will be 3 times the number
 * of connected channels.  The maximum number of STREAMS buffers available for allocation should be
 * set to something larger than this value.  For connectionless channel for early media, use the
 * regular flow control mechanism.  This means that early media might stagger when the system is
 * heavily loaded and getting behind, or when the high water mark is set too low.  The STREAMS MIB
 * can be used to determine and tweak threshold on a running system.
 */
STATIC __hot_in int
np_v4_rcv(struct sk_buff *skb)
{
	struct nc *nc;
	struct iphdr *iph = (typeof(iph)) skb_network_header(skb);
	struct udphdr *uh = (struct udphdr *) (skb_network_header(skb) + (iph->ihl << 2));
	struct rtable *rt;
	ushort ulen;

#ifdef HAVE_KFUNC_NF_RESET
	nf_reset(skb);
#endif

//      IP_INC_STATS_BH(IpInDelivers);  /* should wait... */

	if (unlikely(!pskb_may_pull(skb, sizeof(struct udphdr))))
		goto too_small;
	if (unlikely(skb->pkt_type != PACKET_HOST))
		goto bad_pkt_type;
	rt = skb_rtable(skb);
	if (rt->rt_flags & (RTCF_BROADCAST | RTCF_MULTICAST))
		/* need to do something about broadcast and multicast */ ;

	_printd(("%s: %s: packet received %p\n", DRV_NAME, __FUNCTION__, skb));
//      UDP_INC_STATS_BH(UdpInDatagrams);
	uh = (typeof(uh)) skb_transport_header(skb);
	ulen = ntohs(uh->len);
	/* sanity check UDP length */
	if (unlikely(ulen > skb->len || ulen < sizeof(struct udphdr)))
		goto too_small;
	if (unlikely(pskb_trim(skb, ulen)))
		goto too_small;
	/* we do the lookup before the checksum */
	if (unlikely((nc = nc_lookup(iph, uh)) == NULL))
		goto no_stream;
	/* checksum initialization */
	if (likely(uh->check == 0))
		skb->ip_summed = CHECKSUM_UNNECESSARY;
	else {
		if (skb->ip_summed == CHECKSUM_HW)
			skb->ip_summed = CHECKSUM_UNNECESSARY;
		else if (skb->ip_summed != CHECKSUM_UNNECESSARY)
			skb->csum = csum_tcpudp_nofold(iph->saddr, iph->daddr, ulen, IPPROTO_UDP, 0);
	}
	if (unlikely(skb_is_nonlinear(skb))) {
		_ptrace(("Non-linear sk_buff encountered!\n"));
		goto linear_fail;
	}
	{
		mblk_t *mp;
		frtn_t fr = { &np_free, (caddr_t) skb };
		size_t plen = skb->len + (skb->data - skb_network_header(skb));
		queue_t *q;

		/* now allocate an mblk */
		if (unlikely((mp = esballoc(skb_network_header(skb), plen, BPRI_MED, &fr)) == NULL))
			goto no_buffers;
		/* tell others it is a socket buffer */
		mp->b_datap->db_flag |= DB_SKBUFF;
		_ptrace(("Allocated external buffer message block %p\n", mp));
		/* check flow control only after we have a buffer */
		if ((q = nc->np->oq) == NULL || !canput(q))
			goto flow_controlled;
		// mp->b_datap->db_type = M_DATA;
		mp->b_wptr += plen;
		put(q, mp);
//              UDP_INC_STATS_BH(UdpInDatagrams);
		/* release reference from lookup */
		nc_put(nc);
		return (0);
	      flow_controlled:
		np_rstat.ms_ccnt++;
		freeb(mp);	/* will take sk_buff with it */
		nc_put(nc);
		return (0);
	}
      bad_checksum:
//      UDP_INC_STATS_BH(UdpInErrors);
//      IP_INC_STATS_BH(IpInDiscards);
	/* decrement IpInDelivers ??? */
	// goto linear_fail;
      no_buffers:
      linear_fail:
	if (nc)
		nc_put(nc);
	kfree_skb(skb);
	return (0);
      no_stream:
	ptrace(("ERROR: No stream\n"));
	/* Note, in case there is nobody to pass it too, we have to complete the checksum check before
	   dropping it to handle stats correctly. */
	if (skb->ip_summed != CHECKSUM_UNNECESSARY
	    && (unsigned short) csum_fold(skb_checksum(skb, 0, skb->len, skb->csum)))
		goto bad_checksum;
//      UDP_INC_STATS_BH(UdpNoPorts);   /* should wait... */
      bad_pkt_type:
      too_small:
	if (np_v4_rcv_next(skb)) {
		/* TODO: want to generate an ICMP error here */
	}
	return (0);
}

/**
 * np_v4_err - process a received ICMP packet
 * @skb: socket buffer containing ICMP packet
 * @info: additional information (unused)
 *
 * This function is a network protocol callback that is invoked when transport specific ICMP errors
 * are received.  The function looks up the Stream and, if found, wraps the packet in an M_CTL
 * message and passes it to the read queue of the Stream.
 *
 * ICMP packet consists of ICMP IP header, ICMP header, IP header of returned packet, and IP payload
 * of returned packet (up to some number of bytes of total payload).  The passed in sk_buff has
 * skb->data pointing to the ICMP payload which is the beginning of the returned IP header.
 * However, we include the entire packet in the message.
 *
 * LOCKING: master.lock protects the master list and protects from open, close, link and unlink.
 * np->qlock protects the state of private structure.  np->refs protects the private structure
 * from being deallocated before locking.
 */
STATIC __unlikely void
np_v4_err(struct sk_buff *skb, u32 info)
{
	struct nc *nc;
	struct iphdr *iph = (struct iphdr *) skb->data;

	printd(("%s: %s: error packet received %p\n", DRV_NAME, __FUNCTION__, skb));
	/* Note: use returned IP header and possibly payload for lookup */
	if ((nc = np_lookup_icmp(iph, skb->len)) == NULL)
		goto no_stream;
	if (nc_get_state(nc) == NS_UNBND)
		goto closed;
	{
		mblk_t *mp;
		queue_t *q;
		size_t plen = skb->len + (skb->data - skb_network_header(skb));

		/* Create a queue a specialized M_CTL message to the Stream's read queue for further
		   processing.  The Stream will convert this message into a N_UDERROR_IND, N_RESET_IND or
		   N_DISCON_IND message and pass it along. */
		if ((mp = allocb(plen, BPRI_MED)) == NULL)
			goto no_buffers;
		/* check flow control only after we have a buffer */
		if ((q = nc->np->oq) == NULL || !bcanput(q, 1))
			goto flow_controlled;
		DB_TYPE(mp) = M_CTL;
		mp->b_band = 1;
		bcopy(skb_network_header(skb), mp->b_wptr, plen);
		mp->b_wptr += plen;
		put(q, mp);
		goto discard_put;
	      flow_controlled:
		np_rstat.ms_ccnt++;
		ptrace(("ERROR: stream is flow controlled\n"));
		freeb(mp);
		goto discard_put;
	}
      discard_put:
	/* release reference from lookup */
	nc_put(nc);
	np_v4_err_next(skb, info);	/* anyway */
	return;
      no_buffers:
	ptrace(("ERROR: could not allocate buffer\n"));
	goto discard_put;
      closed:
	ptrace(("ERROR: ICMP for closed stream\n"));
	goto discard_put;
      no_stream:
	ptrace(("ERROR: could not find stream for ICMP message\n"));
	np_v4_err_next(skb, info);
#ifdef HAVE_KINC_LINUX_SNMP_H
#ifdef HAVE_ICMP_INC_STATS_BH_2_ARGS
	ICMP_INC_STATS_BH(dev_net(skb->dev), ICMP_MIB_INERRORS);
#else
	ICMP_INC_STATS_BH(ICMP_MIB_INERRORS);
#endif
#else
	ICMP_INC_STATS_BH(IcmpInErrors);
#endif
	return;
}

/*
 *  Private structure allocation and deallocation.
 */

/**
 * np_alloc_priv - allocate a private structure for the open routine
 * @q: read queue of newly created Stream
 * @slp: pointer to place in list for insertion
 * @type: type of provider: 0, N_CONS, N_CLNS
 * @devp: pointer to device number
 * @crp: pointer to credentials
 *
 * Allocates a new private structure, initializes it to appropriate values, and then inserts it into
 * the private structure list.
 */
noinline struct np *
np_alloc_priv(queue_t *q, struct np **npp, int type, dev_t *devp, cred_t *crp)
{
	struct np *np;

	if ((np = np_alloc())) {
		/* np generic members */
		np->u.dev.cmajor = getmajor(*devp);
		np->u.dev.cminor = getminor(*devp);
		np->cred = *crp;
		(np->oq = q)->q_ptr = np_get(np);
		(np->iq = WR(q))->q_ptr = np_get(np);
		// np->i_prim = &np_w_prim;
		// np->o_prim = &np_r_prim;
		// np->i_wakeup = NULL;
		// np->o_wakeup = NULL;
		spin_lock_init(&np->qlock);	/* "np-queue-lock" */
		// np->ibid = 0;
		// np->obid = 0;
		// np->iwait = NULL;
		// np->owait = NULL;
		np->i_state = (-1);	// LMI_UNUSABLE;
		np->i_style = (1);	// LMI_STYLE1;
		np->i_version = 1;
		np->i_oldstate = (1);	// LMI_UNUSABLE;
		np->type = type;
		/* np specific members */
		np->info.PRIM_type = N_INFO_ACK;
		np->info.NSDU_size = 65535 - sizeof(struct iphdr);
		np->info.ENSDU_size = 0;
		np->info.CDATA_size = 65535 - sizeof(struct iphdr);
		np->info.DDATA_size = 65535 - sizeof(struct iphdr);
		np->info.ADDR_size = sizeof(struct sockaddr_storage);
		np->info.ADDR_length = 0;
		np->info.ADDR_offset = 0;
		np->info.QOS_length = 0;
		np->info.QOS_offset = 0;
		np->info.QOS_range_length = 0;
		np->info.QOS_range_offset = 0;
		np->info.OPTIONS_flags = 0;
		np->info.NIDU_size = 65535 - sizeof(struct iphdr);
		np->info.SERV_type = type ? : (N_CLNS | N_CONS);
		np->info.CURRENT_state = NS_UNBND;
		np->info.PROVIDER_type = N_SUBNET;
		np->info.NODU_size = 536;
		np->info.PROTOID_length = 0;
		np->info.PROTOID_offset = 0;
		np->info.NPI_version = N_CURRENT_VERSION;
		bufq_init(&np->conq);
		bufq_init(&np->datq);
		bufq_init(&np->resq);
		/* qos defaults */
		np->qos.n_qos_type = N_QOS_SEL_INFO_RTP;
		np->qos.protocol = QOS_UNKNOWN;
		np->qos.priority = 0;
		np->qos.ttl = 64;
		np->qos.tos = 0;
		np->qos.mtu = 65535;
		np->qos.saddr = 0;
		np->qos.daddr = QOS_UNKNOWN;
		np->qos.checksum = 0;
		/* qos range info */
		np->qor.n_qos_type = N_QOS_RANGE_INFO_RTP;
		np->qor.priority.priority_min_value = 0;
		np->qor.priority.priority_max_value = 255;
		np->qor.ttl.ttl_min_value = 1;
		np->qor.ttl.ttl_max_value = 255;
		np->qor.tos.tos_min_value = 0;
		np->qor.tos.tos_max_value = 255;
		np->qor.mtu.mtu_min_value = 536;
		np->qor.mtu.mtu_max_value = 65535;
		/* link into master list */
		np_get(np);
		if ((np->next = *npp))
			np->next->prev = &np->next;
		np->prev = npp;
		*npp = np;
	} else
		ptrace(("%s: ERROR: Could not allocate module private structure\n", DRV_NAME));
	return (np);
}

/**
  * np_free_priv - deallocate a private structure for the close routine
  * @q: read queue of closing Stream
  */
noinline void
np_free_priv(queue_t *q)
{
	struct np *np;

	ensure(q, return);
	np = NP_PRIV(q);
	ensure(np, return);
	printd(("%s: unlinking private structure, reference count = %d\n", DRV_NAME,
		atomic_read(&np->refcnt)));
	/* make sure the stream is disconnected */
	np_disconnect(np, NULL, NULL, N_REASON_UNDEFINED, NULL);
	/* make sure the stream is unbound */
	np_unbind(np);
	if (np->daddrs[0].dst != NULL) {
		dst_release(np->daddrs[0].dst);
		np->daddrs[0].dst = NULL;
	}
	bufq_purge(&np->conq);
	bufq_purge(&np->datq);
	bufq_purge(&np->resq);
	printd(("%s: removed bufcalls, reference count = %d\n", DRV_NAME, atomic_read(&np->refcnt)));
	/* remove from master list */
	if ((*np->prev = np->next))
		np->next->prev = np->prev;
	np->next = NULL;
	np->prev = &np->next;
	np_put(np);
	printd(("%s: unlinked, reference count = %d\n", DRV_NAME, atomic_read(&np->refcnt)));
	np_release((struct np **) &np->oq->q_ptr);
	np->oq = NULL;
	np_release((struct np **) &np->iq->q_ptr);
	np->iq = NULL;
	assure(atomic_read(&np->refcnt) == 1);
	np_release(&np);	/* should normally be final put */
	return;
}

/*
 *  Open and Close
 */
#define FIRST_CMINOR	0
#define  PROT_CMINOR	0	/* connection oriented or connectionless */
#define  CONS_CMINOR	N_CONS	/* connection oriented only */
#define  CLNS_CMINOR	N_CLNS	/* connectionless only */
#define  LAST_CMINOR	2
#define  FREE_CMINOR	3
STATIC int np_majors[CMAJORS] = { CMAJOR_0, };

caddr_t np_opens = NULL;

/**
 * np_qopen - NPI UDP driver STREAMS open routine
 * @q: read queue of newly created Stream
 * @devp: pointer to device number opened
 * @oflag: flags to the open call
 * @sflag: STREAMS flag: DRVOPEN, MODOPEN or CLONEOPEN
 * @crp: pointer to opener's credentials
 *
 * There are four clone minor device numebrs: PROT_CMINOR is a connection-less or
 * connection-oriented mode; CONS_CMINOR is a connection-oriented only mode; CLNS_CMINOR is a
 * connectionless only mode.  All modes support multiple streams using M_CTL/M_PCCTL messages.
 */
STATIC streamscall int
np_qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	minor_t bminor = cminor;
	unsigned long flags;
	struct np *np;
	int err, type = 0;
	mblk_t *mp;
	struct stroptions *so;
	struct nc *nc;

	if (sflag == MODOPEN)
		/* cannot push as module */
		return (ENXIO);
	switch (cminor) {
	case PROT_CMINOR:
	case CONS_CMINOR:
	case CLNS_CMINOR:
		type = cminor;
		bminor = cminor;
		sflag = CLONEOPEN;
		break;
	}
	if (q->q_ptr) {
		if (sflag == CLONEOPEN)
			return (0);
		/* FIXME: this is not a clone device and yet it is already open? Check that the opening
		   process has sufficient credentials to open the non-clone device. */
		return (EPERM);
	}
	if (cminor >= FREE_CMINOR)
		/* Cannot open clone device numbers explicitly. */
		return (ENXIO);
	if (cminor > LAST_CMINOR)
		/* All others a non-clone devices. */
		sflag = DRVOPEN;
	if (sflag == CLONEOPEN) {
		cminor = FREE_CMINOR;
		*devp = makedevice(cmajor, cminor);
	}
	if (unlikely(!(mp = allocb(sizeof(*so), BPRI_WAITOK))))
		return (ENOSR);

	/* np_lock protects the list of streams.  This is needed for accepting a connection indication from
	   one stream on another. */
	write_lock_irqsave(&np_lock, flags);
	if ((err = mi_open_comm_cache(&np_opens, np_priv_cachep, q, devp, oflags, sflag, crp))) {
		write_unlock_irqrestore(&np_lock, flags);
		freeb(mp);
		return (err);
	}

	/* initialize network provider private structure */
	np = NP_PRIV(q);
	bzero(np, sizeof(*np));
	np->oq = q;
	np->iq = WR(q);
	np->nc = NULL;

	np->base.nnext = NULL;
	np->base.nprev = &np->base.nnext;
	np->base.np = np;
	np->base.bnext = NULL;
	np->base.bprev = &np->base.bnext;
	np->base.bhash = NULL;
	np->base.cnext = NULL;
	np->base.cprev = &np->base.cnext;
	np->base.chash = NULL;
	np->base.SERV_type = type ? : (N_CLNS | N_CONS);
	np->base.CURRENT_state = NS_UNBND;
	np->base.BIND_flags = 0;
	np->base.CONN_flags = 0;
	np->base.CONIND_number = 0;
	bufq_init(&np->base.conq);
	bufq_init(&np->base.datq);
	bufq_init(&np->base.resq);
	np->base.pnum = 0;
	np->base.bnum = 0;
	np->base.bport = 0;
	np->base.snum = 0;
	np->base.sport = 0;
	np->base.dnum = 0;
	np->base.dport = 0;

	/* There are several groupings of information associated with RTP channels that we will use when
	   sending RTP packets. The base group is global defaults.  Another group is per-Stream.  To acheive
	   a different grouping of per-Stream parameters, open another Stream.  Another group is per-IP
	   address.  We only expect a limited number (16) IP addresses to be used.  The last grouping is
	   per-interface parameters.  Per-interface parameters are limited as much as possible to reduce the
	   size (and thus increase the performance) of the connection-mapping memory. */

	np->info.PRIM_type = N_INFO_ACK;
	np->info.NSDU_size = 65535 - sizeof(struct iphdr) - sizeof(struct udphdr);
	np->info.ENSDU_size = 0;
	np->info.CDATA_size = 65535 - sizeof(struct iphdr) - sizeof(struct udphdr);
	np->info.DDATA_size = 65535 - sizeof(struct ipdhr) - sizeof(struct udphdr);
	np->info.ADDR_size = sizeof(struct sockaddr_storage);
	np->info.ADDR_length = 0;
	np->info.ADDR_offset = 0;
	np->info.QOS_length = 0;
	np->info.QOS_offset = 0;
	np->info.QOS_range_length = 0;
	np->info.QOS_range_offset = 0;
	np->info.OPTIONS_flags = 0;
	np->info.NIDU_size = 65535 - sizeof(struct iphdr) - sizeof(struct udphdr);
	np->info.SERV_type = type ? : (N_CLNS | N_CONS);
	np->info.CURRENT_state = NS_UNBND;
	np->info.PROVIDER_type = N_SUBNET;
	np->info.NODU_size = 536 - sizeof(struct udphdr);
	np->info.PROTOID_length = 0;
	np->info.PROTOID_offset = 0;
	np->info.NPI_version = N_CURRENT_VERSION;

	/* qos defaults */
	np->qos.n_qos_type = N_QOS_SEL_INFO_UDP;
	np->qos.protocol = QOS_UNKNOWN;
	np->qos.priotity = 0;
	np->qos.ttl = 64;
	np->qos.tos = 0;
	np->qos.mtu = 65535;
	np->qos.saddr = 0;
	np->qos.daddr = QOS_UNKNOWN;
	np->qos.checksum = 0;
	/* qos range info */
	np->qor.n_qos_type = N_QOS_RANGE_INFO_UDP;
	np->qor.priority.priority_min_value = 0;
	np->qor.priority.priority_max_value = 255;
	np->qor.ttl.ttl_min_value = 1;
	np->qor.ttl.ttl_max_value = 255;
	np->qor.tos.tos_min_value = 0;
	np->qor.tos.tos_max_value = 255;
	np->qor.mtu.mtu_min_value = 536;
	np->qor.mtu.mtu_max_value = 65535;

	write_unlock_irqrestore(&np_lock, flags);

	/* We generate a message to the Stream head (that can be intercepted by other modules) that specifies 
	   that M_DATA message blocks should be allocated using socket buffers.  Also, set the write offset
	   in the M_DATA messages to accept MAX_HEADER bytes so that we can fill out the IP and hardware
	   header without reallocating socket buffers on the way out. Set the minimum and maximum packet size 
	   for outgoing messages so that the Stream head will do the checking for us where possible.  The
	   high and low water marks for the Stream head are also set, but this is preliminary as this can be
	   adjusted using the STREAMS MIB.  */

	DB_TYPE(mp) = M_PCSETOPTS;
	so = (typeof(so)) mp->b_wptr;
	bzero(so, sizeof(*so));
	so->so_flags |= SO_SKBUFF;
	/* want to set a write ofset of MAX_HEADER bytes */
	so->so_flags |= SO_WROFF;
	so->so_wroff = MAX_HEADER;	/* this is too big */
	so->so_flags |= SO_MINPSZ;
	so->so_minpsz = np_minfo.mi_minpsz;
	so->so_flags |= SO_MAXPSZ;
	so->so_maxpsz = np_minfo.mi_maxpsz;
	so->so_flags |= SO_HIWAT;
	so->so_hiwat = np_minfo.mi_hiwat;
	so->so_flags |= SO_LOWAT;
	so->so_lowat = np_minfo.mi_lowat;
	mp->b_wptr += sizeof(*so);
	/* It might be an idea to move this after the putnext(). */
	qprocson(q);
	putnext(q, mp);
	return (0);
}

/** np_qlcose - NPI-UDP driver STREAMS close routine
  * @q: read queue of closing stream
  * @oflag: flags to open(2) call
  * @crp: closer's credentials
  *
  * Note that a Stream can be closed regardless of the state of the Stream-based network connection
  * or any multi-channel connections associated with the Stream.  Each one of these network
  * connections needs to be disconnected or aborted in some way.  It may, for example, be necessary
  * to send an RTCP BYE message to the other end to indicate the disconnection of media.  It is not;
  * however, necessary to issue primitives upstream (because the Stream head is already closing and
  * they cannot be received anyway).
  */
STATIC streamscall int
np_qclose(queue_t *q, int oflag, cred_t *crp)
{
	struct np *np = NP_PRIV(q);
	struct nc *nc, *nc_nnext;
	unsigned long flags;

	(void) oflag;
	(void) crp;
	(void) np;
	_printd(("%s: closing character device %d:%d\n", DRV_NAME, np->u.dev.cmajor, np->u.dev.cminor));
	/* make sure procedures are off */
	qprocsoff(q);
	flushq(WR(q), FLUSHALL);
	flushq(RD(q), FLUSHALL);

	/* disconnect, unbind, and purge base connection */
	/* make sure the real interface is disconnected */
	np_disconnect(np, NULL, NULL, N_REASON_UNDEFINED, NULL);
	/* make sure the real interface is unbound */
	np_unbind(np);
	if (np->base.daddrs[0].dst != NULL) {
		dst_release(np->base.daddrs[0].dst);
		np->base.daddrs[0].dst = NULL;
	}
	bufq_purge(&np->base.conq);
	bufq_purge(&np->base.datq);
	bufq_purge(&np->base.resq);

	/* disconnect, unbind, purge and free all network connections */
	for (nc_nnext = np->nc; (nc = nc_nnext);) {
		nc_nnext = nc->nnext;
		/* make sure the virtual interface is disconnected */
		nc_disconnect(np, nc, NULL, NULL, N_REASON_UNDEFINED, NULL);
		/* make sure the virtual interface is unbound */
		nc_unbind(nc);
		if (nc->daddrs[0].dst != NULL) {
			dst_release(np->daddrs[0].dst);
			np->daddrs[0].dst = NULL;
		}
		bufq_purge(&nc->conq);
		bufq_purge(&nc->datq);
		bufq_purge(&nc->resq);
		if (*nc->nprev = nc_nnext)
			nc_nnext->nprev = nc->nprev;
		kmem_cache_free(np_conn_cache, nc);
	}
	write_lock_irqsave(&np_lock, flags);
	mi_close_comm_cache(np_opens, np_priv_cachep, q);
	write_unlock_irqrestore(&np_lock, flags);
	return (0);
}

/*
 *  Private structure reference counting, allocation, deallocation and cache
 */
STATIC __unlikely int
np_term_caches(void)
{
	if (np_conn_cachep != NULL) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_conn_cachep)) {
			cmn_err(CE_WARN, "%s: did not destory np_conn_cachep", __FUNCTION__);
			return (-EBUSY);
		}
#else				/* HAVE_KTYPE_KMEM_CACHE_T_P */
		kmem_cache_destroy(np_conn_cachep);
#endif				/* HAVE_KTYPE_KMEM_CACHE_T_P */
		_printd(("%s: destroyed np_conn_cachep\n", DRV_NAME));
		np_conn_cachep = NULL;
	}
	if (np_prot_cachep != NULL) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_prot_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_prot_cachep", __FUNCTION__);
			return (-EBUSY);
		}
#else				/* HAVE_KTYPE_KMEM_CACHE_T_P */
		kmem_cache_destroy(np_prot_cachep);
#endif				/* HAVE_KTYPE_KMEM_CACHE_T_P */
		_printd(("%s: destroyed np_prot_cachep\n", DRV_NAME));
		np_prot_cachep = NULL;
	}
	if (np_priv_cachep != NULL) {
#ifdef HAVE_KTYPE_KMEM_CACHE_T_P
		if (kmem_cache_destroy(np_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy np_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		}
#else				/* HAVE_KTYPE_KMEM_CACHE_T_P */
		kmem_cache_destroy(np_priv_cachep);
#endif				/* HAVE_KTYPE_KMEM_CACHE_T_P */
		_printd(("%s: destroyed np_priv_cachep\n", DRV_NAME));
		np_priv_cachep = NULL;
	}
	return (0);
}

STATIC __unlikely int
np_init_caches(void)
{
	if (np_priv_cachep == NULL) {
		np_priv_cachep =
		    kmem_create_cache("np_priv_cachep", mi_open_size(sizeof(struct np)), 0, SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (np_priv_cachep == NULL) {
			cmn_err(CE_WARN, "%s: Cannot allocate np_priv_cachep", __FUNCTION__);
			np_term_caches();
			return (-ENOMEM);
		}
		_printd(("%s: initialized driver private structure cache\n", DRV_NAME));
	}
	if (np_prot_cachep == NULL) {
		np_prot_cachep =
		    kmem_create_cache("np_prot_cachep", sizeof(struct nc_prot_bucket), 0, SLAB_HWCACHE_ALIGN,
				      NULL, NULL);
		if (np_prot_cachep == NULL) {
			cmn_err(CE_WARN, "%s: Cannot allocate np_prot_cachep", __FUNCTION__);
			np_term_caches();
			return (-ENOMEM);
		}
		_printd(("%s: initialized driver protocol structure cache\n", DRV_NAME));
	}
	if (np_conn_cachep == NULL) {
		np_conn_cachep =
		    kmem_create_cache("np_conn_cachep", sizeof(struct nc), 0, SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (np_conn_cachep == NULL) {
			cmn_err(CE_WARN, "%s: Cannot allocate np_conn_cachep", __FUNCTION__);
			np_term_caches();
			return (-ENOMEM);
		}
		_printd(("%s: intializaed driver network connection cache\n", DRV_NAME));
	}
	return (0);
}

STATIC __unlikely void
np_term_hashes(void)
{
	if (nc_bhash) {
		free_pages((unsigned long) nc_bhash, nc_bhash_order);
		nc_bhash = NULL;
		nc_bhash_size = 0;
		nc_bhash_order = 0;
	}
	if (nc_chash) {
		free_pages((unsigned long) nc_chash, nc_chash_order);
		nc_chash = NULL;
		nc_chash_size = 0;
		nc_chash_order = 0;
	}
}

STATIC __unlikely void
np_init_hashes(void)
{
	int i;

	/* Start with just one page for each. */
	if (nc_bhash == NULL) {
		nc_bhash_order = 0;
		if ((nc_bhash = (struct nc_bhash_bucket *) __get_free_pages(GFP_ATOMIC, nc_bhash_order))) {
			nc_bhash_size = (1 << (nc_bhash_order + PAGE_SHIFT)) / sizeof(struct nc_bhash_bucket);
			_printd(("%s: INFO: bind hash table configured size = %ld\n", DRV_NAME,
				 (long) nc_bhash_size));
			bzero(nc_bhash, nc_bhash_size * sizeof(struct nc_bhash_bucket));
			for (i = 0; i < nc_bhash_size; i++)
				rwlock_init(&nc_bhash[i].lock);
		} else {
			np_term_hashes();
			cmn_err(CE_PANIC, "%s: Failed to allocate bind hash table\n", __FUNCTION__);
			return;
		}
	}
	if (nc_chash == NULL) {
		nc_chash_order = 0;
		if ((nc_chash = (struct nc_chash_bucket *) __get_free_pages(GFP_ATOMIC, nc_chash_order))) {
			nc_chash_size = (1 << (nc_chash_order + PAGE_SHIFT)) / sizeof(struct nc_chash_bucket);
			_printd(("%s: INFO: conn hash table configured size = %ld\n", DRV_NAME,
				 (long) nc_chash_size));
			bzero(nc_chash, nc_chash_size * sizeof(struct nc_chash_bucket));
			for (i = 0; i < nc_chash_size; i++)
				rwlock_init(&nc_chash[i].lock);
		} else {
			np_term_hashes();
			cmn_err(CE_PANIC, "%s: Failed to allocate bind hash table\n", __FUNCTION__);
			return;
		}
	}
}

/*
 *  Registration and initialization
 */
#ifdef LINUX
/*
 *  Linux registration
 */

unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0444);
#endif
MODULE_PARM_DESC(modid, "Module ID for the UDP driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0444);
#endif
MODULE_PARM_DESC(major, "Device number for the UDP driver. (0 for allocation.)");

/*
 *  Linux Fast-STREAMS Registration
 */
STATIC struct cdevsw np_cdev = {
	.d_name = DRV_NAME,
	.d_str = &np_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

/*
 *  Minor device nodes for the NPI-RTP driver.
 *
 *  All three minor device nodes are clone minors that are available to anyone that can open the
 *  device.  The first minor device, "rtp",  is for plain RTP; the second, "rtpco", connect-oriented
 *  RTP; the third, "rtpcl", connectionless.  There is, in fact, little difference between these
 *  minor device nodes aside from the defaults of several settings.
 */

STATIC struct devnode np_node_prot = {
	.n_name = "rtp",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode np_node_cons = {
	.n_name = "rtpco",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC struct devnode np_node_clns = {
	.n_name = "rtpcl",
	.n_flag = D_CLONE,	/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

/** np_regsiter_strdev - register NPI-RTP STREAMS device
  *
  * Registers the STREAMS major and minor device nodes associated with the NPI-RTP pseudo-device
  * driver.  Note that once the registration f the STREAMS major device node has been successful,
  * registration of the minor device nodes is always successful.  Registration of "named" minor
  * device nodes is not critical anyway.
  */
STATIC __unlikely int
np_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&np_cdev, major)) < 0)
		return (err);
	register_strnod(&np_cdev, &np_node_prot, PROT_CMINOR);
	register_strnod(&np_cdev, &np_node_cons, CONS_CMINOR);
	register_strnod(&np_cdev, &np_node_clns, CLNS_CMINOR);
	return (0);
}

/** np_unregsiter_strdev - unregister NPI-RTP STREAMS device
  *
  * Unregisters the STREAMS minor and major device nodes associated with the NPI-RTP pseudo-device
  * driver.  Note that deregistration of the minor device nodes always succeeds.  Deregistration of
  * the major device node only fails when it was not successfully registered in the first place.
  */
STATIC __unlikely int
np_unregister_strdev(major_t major)
{
	int err;

	unregister_strnod(&np_cdev, PROT_CMINOR);
	unregister_strnod(&np_cdev, CONS_CMINOR);
	unregister_strnod(&np_cdev, CLNS_CMINOR);
	if ((err = unregister_strdev(&np_cdev, major)) < 0)
		return (err);
	return (0);
}

/** npterminate - terminate the NPI-RTP driver.
  *
  * Terminates the NPI-RTP driver and prepares for module unloading.  Note that this function is not
  * called unless there are no instances of the driver allocated.
  */
MODULE_STATIC void __exit
npterminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (np_majors[mindex]) {
			if ((err = np_unregister_strdev(np_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					np_majors[mindex]);
			if (mindex)
				np_majors[mindex] = 0;
		}
	}
	if ((err = np_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	np_term_hashes();
	return;
}

/** npinit - intialize the NPI-RTP driver.
  *
  * Initialize the NPI-RTP driver after module (or kernel) loading.  The function registers the
  * STREAMS driver and allocates caches, hashes and bitmaps.
  * RETURN: int - zero on success; non-zero error number, otherwise.
  */
MODULE_STATIC int __init
npinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	np_init_hashes();
	if ((err = np_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		npterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = np_register_strdev(np_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					np_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d", DRV_NAME, err);
				npterminate();
				return (err);
			}
		}
		if (np_majors[mindex] == 0)
			np_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(np_majors[mindex]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = np_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(npinit);
module_exit(npterminate);

#endif				/* LINUX */
