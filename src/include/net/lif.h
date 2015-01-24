/*****************************************************************************

 @(#) src/include/net/lif.h

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

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

#ifndef _NET_LIF_H
#define _NET_LIF_H

#ifdef __KERNEL__
#include <linux/if.h>
#else
#include <net/if.h>
#endif

#ifndef LIFNAMSIZ

#ifndef IFF_UP
#define IFF_UP		    (1<< 0)	/* address is up */
#endif
#ifndef IFF_BROADCAST
#define IFF_BROADCAST	    (1<< 1)	/* broadcast address valid */
#endif
#ifndef IFF_DEBUG
#define IFF_DEBUG	    (1<< 2)	/* turn on debugging */
#endif
#ifndef IFF_LOOPBACK
#define IFF_LOOPBACK	    (1<< 3)	/* is a loopback net */
#endif
#ifndef IFF_POINTTOPOINT
#define IFF_POINTTOPOINT    (1<< 4)	/* point-to-point link */
#endif
#ifndef IFF_NOTRAILERS
#define IFF_NOTRAILERS	    (1<< 5)	/* avoid use of trailers */
#endif
#ifndef IFF_RUNNING
#define IFF_RUNNING	    (1<< 6)	/* resources allocated */
#endif
#ifndef IFF_NOARP
#define IFF_NOARP	    (1<< 7)	/* no address resolution protocol */
#endif
#ifndef IFF_PROMISC
#define IFF_PROMISC	    (1<< 8)	/* receive all packets */
#endif
#ifndef IFF_ALLMULTI
#define IFF_ALLMULTI	    (1<< 9)	/* receive all multicast packets */
#endif
#define IFF_INTELLIGENT	    (1<<10)	/* protocol code on board */
#ifndef IFF_MULTICAST
/* note that under linux this is (1<<12) */
#define IFF_MULTICAST	    (1<<11)	/* supports multicast */
#endif
#define IFF_MULTI_BCAST	    (1<<12)	/* multicast using brdcst address */
#define IFF_UNNUMBERED	    (1<<13)	/* non-unique address */
#define IFF_DHCPRUNNING	    (1<<14)	/* DHCP controls this interface */
#define IFF_PRIVATE	    (1<<15)	/* do not advertise */
#define IFF_NOXMIT	    (1<<16)	/* do not transmit packets */
#define IFF_NOLOCAL	    (1<<17)	/* no address - just on-link subnet */
#define IFF_DEPRECATED	    (1<<18)	/* address is deprecated */
#define IFF_ADDRCONF	    (1<<19)	/* address from stateless addrconf */
#define IFF_ROUTER	    (1<<20)	/* router on this interface */
#define IFF_NONUD	    (1<<21)	/* no NUD on this interface */
#define IFF_ANYCAST	    (1<<22)	/* anycast address */
#define IFF_NORTEXCH	    (1<<23)	/* do not exchange routing info */
#define IFF_IPV4	    (1<<24)	/* IPv4 interface */
#define IFF_IPV6	    (1<<25)	/* IPv6 interface */
#define IFF_NOACCEPT	    (1<<26)	/* no-accept mode VRRP ill */
#define IFF_NOFAILOVER	    (1<<27)	/* in.mpathd(1M) test address */
#define IFF_FAILED	    (1<<28)	/* interface has failed */
#define IFF_STANDBY	    (1<<29)	/* interface is a hot-spare */
#define IFF_INACTIVE	    (1<<30)	/* fuction but not used for data */
#define IFF_OFFLINE	    (1<<31)	/* interface is offline */
#define IFF_XRESOLV	    (1<<32)	/* IPv6 external resolver */
#define IFF_COS_ENABLED	    (1<<33)	/* iface supports CoS marking */
#define IFF_PREFERRED	    (1<<34)	/* prefer as source address */
#define IFF_TEMPORARY	    (1<<35)	/* RFC3041 */
#define IFF_FIXEDMTU	    (1<<36)	/* MTU manually set with SIOCSLIFMTU */
#define IFF_VIRTUAL	    (1<<37)	/* does not send or receive packets */
#define IFF_DUPLICATE	    (1<<38)	/* local address already in use */
#define IFF_IPMP	    (1<<39)	/* IPMP IP interface */
#define IFF_VRRP	    (1<<40)	/* managed by VRRP */
#define IFF_NOLINELOCAL	    (1<<41)	/* no default linklocal */
#define IFF_L3PROTECT	    (1<<42)	/* layer-3 protection enforced */

/* flags that cannot be changed by user on any interface */
#define IFF_CANTCHANGE \
	(IFF_BROADCAST | IFF_POINTTOPOINT | IFF_RUNNING | IFF_PROMISC | \
	 IFF_MULTICAST | IFF_MULTI_BCAST | IFF_UNNUMBERED | IFF_IPV4 | \
	 IFF_IPV6 | IFF_IPMP | IFF_FIXEDMTU | IFF_VIRTUAL | \
	 IFF_LOOPBACK | IFF_ALLMULTI | IFF_DUPLICATE | IFF_COS_ENABLED | \
	 IFF_VRRP | IFF_NOLINELOCAL | IFF_L3PROTECT)

/* flags that cannot be changed by user on IPMP interface */
#define IFF_IPMP_CANTCHANGE	IF_FAILED

/* flags that can never be set on IPMP interace */
#define IFF_IPMP_INVALID \
	(IFF_STANDBY | IFF_INACTIVE | IFF_OFFLINE | \
	 IFF_NOFAILOVER | IFF_NOARP | IFF_NONUD | IFF_XRESOLV | \
	 IFF_NOACCEPT)

#define ND_MAX_HDW_LEN 64
typedef struct lif_nd_req {
	struct sockaddr_storage lnr_addr;
	uint8_t lnr_state_create;	/* when creating */
	uint8_t lnr_state_same_lla;	/* update same addr */
	uint8_t lnr_state_diff_lla;	/* update w/ diff. */
	int lnr_hdw_len;
	int lnr_flags;
	int lnr_pad0;
	char lnr_hdw_addr[ND_MAX_HDW_LEN];
} lif_nd_req_t;

/* Neighbor reachability states. */
#define ND_UNCHANGED	0	/* for ioctls that do not modify state */
#define ND_INCOMPLETE	1	/* addr resolution in progress */
#define ND_REACHABLE	2	/* have recently been reachable */
#define ND_STALE	3	/* may be unreachable, do not act */
#define ND_DELAY	4	/* wait for upper layer hint */
#define ND_PROBE	5	/* send probes */
#define ND_UNREACHABLE	6	/* delete this route */
#define ND_INITIAL	7	/* ipv4: arp resolution has not been sent yet */

#define ND_STATE_VALID_MTU  0
#define ND_STATE_VALID_MAX  7

/* lnr_flags */
#define NDF_ISROUTER_ON		(1<<0)
#define NDF_ISROUTER_OFF	(1<<1)
#define NDF_ANYCAST_ON		(1<<2)
#define NDF_ANYCAST_OFF		(1<<3)
#define NDF_PROXY_ON		(1<<4)
#define NDF_PROXY_OFF		(1<<5)
#define NDF_STATIC		(1<<6)

typedef struct lif_ifinfo_req {
	uint8_t lir_maxhops;
	uint32_t lir_reachtime;		/* reachable time in msec */
	uint32_t lir_reachretrans;	/* retranmission timer msec */
	uint32_t lir_maxmtu;
} lif_ifinfo_req_t;

#define _LIFNAMSIZ 32

#define LIFNAMSIZ _LIFNAMSIZ
#define LIFGRNAMSIZ LIFNAMSIZ

#ifndef __id_t_defined
typedef int id_t;
#define __id_t_defined
#endif
typedef id_t zoneid_t;

struct lifreq {
	char lifr_name[LIFNAMSIZ];	/* if name */
	union {
		int lifru_addrlen;	/* for subnet/token etc */
		uint lifru_ppa;		/* SIOCLIFNAME */
	} lifr_lifru1;
#define lifr_addrlen	lifr_lifru1.lifru_addrlen
#define lifr_ppa	lifr_lifru1.lifru_ppa
	uint lifr_type;			/* IFR_ETHER, ... */
	union {
		struct sockaddr_storage lifru_addr;
		struct sockaddr_storage lifru_dstaddr;
		struct sockaddr_storage lifru_broadaddr;
		struct sockaddr_storage lifru_token;	/* with lifr_addrlen */
		struct sockaddr_storage lifru_subnet;	/* with lifr_addrlen */
		int lifru_index;	/* interface index */
		uint64_t lifru_flags;	/* flags for SIOC?LIFFLAGS */
		int lifru_metric;
		uint lifru_mtu;
		int lif_muxid[2];	/* mux ids for arp and ip */
		struct lif_nd_req lifru_nd_req;
		struct lif_ifinfo_req lifru_ifinfo_req;
		char lifru_groupname[LIFGRNAMSIZ];	/* SIOC(GS)LIFGROUPNAME */
		char lifru_binding[LIFNAMSIZ];	/* SIOCGLIFBINDING */
		zoneid_t lifru_zoneid;	/* SIOC(GS)LIFZONE */
		uint lifru_dadstate;	/* SIOCGLIFDATASTATE */
	} lifr_lifru;
#define lifr_addr	lifr_lifru.lifru_addr	/* address */
#define lifr_dstaddr	lifr_lifru.lifru_dstaddr	/* other end of p2p link */
#define lifr_broadaddr	lifr_lifru.lifru_broadaddr	/* broadcast addr */
#define lifr_token	lifr_lifru.lifru_token	/* address token */
#define lifr_subnet	lifr_lifru.lifru_subnet	/* subnet prefix */
#define lifr_index	lifr_lifru.lifru_index	/* interface index */
#define lifr_flags	lifr_lifru.lifru_flags	/* flags */
#define lifr_metric	lifr_lifru.lifru_metric	/* metric */
#define lifr_mtu	lifr_lifru.lifru_mtu	/* mtu */
#define lifr_ip_muxid	lifr_lifru.lifru_muxid[0]
#define lifr_arp_muxid	lifr_lifru.lifru_muxid[1]
#define lifr_nd		lifr_lifru.lifru_nd_req	/* SIOCLIF*ND */
#define lifr_ifinfo	lifr_lifru.lifru_ifinfo_req	/* SIOC[GS]LIFLNKINFO */
#define lifr_groupname	lifr_lifru.lifru_groupname
#define lifr_binding	lifr_lifru.lifru_binding
#define lifr_zoneid	lifr_lifru.lifru_zoneid
#define lifr_dadstate	lifr_lifru.lifru_dadstate
};

#define DAD_IN_PROGRESS		0x1
#define DAD_DONE		0x2

#ifndef __KERNEL__
typedef unsigned short int sa_family_t;
#endif

/* used by SIOCGLIFNUM */
struct lifnum {
	sa_family_t lifn_family;
	int lifn_flags;			/* request specific interfaces */
	int lifn_count;			/* result */
};

/* used by SIOCGLIFCONF */
struct lifconf {
	sa_family_t lifc_family;
	int lifc_flags;			/* request specific interfaces */
	int lifc_len;			/* size of associated buffer */
	union {
		caddr_t lifcu_buf;
		struct lifreq *lifcu_req;
	} lifc_lifcu;
#define lifc_buf	lifc_lifcu.lifcu_buf	/* buffer address */
#define lifc_req	lifc_lifcu.lifcu_req	/* array of structures returned */
};

/* flags */
#define LIFC_NOXMIT		(1<<0)	/* include IFF_NOXMIT interface */
#define LIFC_EXTERNAL_SOURCE	(1<<1)
#define LIFC_TEMPORARY		(1<<2)
#define LIFC_ALLZONES		(1<<3)
#define LIFC_UNDER_IPMP		(1<<4)
#define LIFC_ENABLED		(1<<5)

#ifdef __KERNEL__
#ifdef __LP64__

struct lifconf32 {
	sa_family_t lifc_family;
	int32_t lifc_flags;
	int32_t lifc_len;
	union {
		uint32_t lifcu_buf;
		uint32_t lifcu_req;
	} lifc_lifcu;
};

#endif				/* __LP64__ */
#endif				/* __KERNEL__ */

#endif				/* LIFNAMSIZ */

#ifndef IFNAMSIZ

struct ifreq {
#define IFNAMSIZ
	char ifr_name[IFNAMSIZ];
	union {
		struct sockaddr ifru_addr;
		struct sockaddr ifru_dstaddr;
		char ifru_oname[IFNAMSIZ];	/* other if name */
		struct sockaddr ifru_broadaddr;
		int ifru_index;		/* interface index */
		uint ifru_mtu;
		short ifru_flags;
		int ifru_metric;
		char ifru_data[1];	/* interface dependent data */
		char ifru_enaddr[6];
		int if_muxid[2];	/* mux ids for arp and ip */

		/* struct for flags/ppa */
		struct ifr_ppaflags {
			short ifrup_flags;	/* space of ifru_flags */
			short ifrup_filler;
			uint ifrup_ppa;
		} ifru_ppaflags;

		/* struct for FDDI ioctls */
		struct ifr_dnld_reqs {
			uint32_t v_addr;
			uint32_t m_addr;
			uint32_t ex_addr;
			uint32_t size;
		} ifru_dnld_req;

		/* struct for FDDI stats */
		struct ifr_fddi_stats {
			uint32_t stat_size;
			uint32_t fddi_state;
		} ifru_fddi_stat;

		struct ifr_netmapents {
			uint32_t map_ent_size;	/* size of netmap struct */
			uint32_t entry_number;	/* index into netmap list */
			uint32_t fddi_map_ent;	/* pointer to user structure */
		} ifru_netmapent;

		/* field for generic ioctl for fddi */
		struct ifr_fddi_gen_structure {
			uint32_t ifru_fddi_gioctl;	/* field for gen ioctl */
			uint32_t ifru_fddi_gaddr;	/* generic ptr to a field */
		} ifru_fddi_gstruct;

	} ifr_ifru;
#define ifr_addr	ifr_ifru.ifru_addr	/* address */
#define ifr_dstaddr	ifr_ifru.ifru_dstaddr	/* other end of p2p link */
#define ifr_oname	ifr_ifru.ifru_oname	/* other if name */
#define ifr_broadaddr	ifr_ifru.ifru_broadaddr	/* broadcast address */
#define ifr_flags	ifr_ifru.ifru_flags	/* flags */
#define ifr_metric	ifr_ifru.ifru_metric	/* metric */
#define ifr_data	ifr_ifru.ifru_data	/* for use by interface */
#define ifr_enaddr	ifr_ifru.ifru_enaddr	/* ethernet address */
#define ifr_index	ifr_ifru.ifru_index	/* interface index */
#define ifr_mtu		ifr_ifru.ifru_mtu	/* mtu */
/* for setting ppa */
#define ifr_ppa		ifr_ifru.ifru_ppaflags.ifrup_ppa
/* FDDI specific */
#define ifr_dnld_req	ifr_ifru.ifru_dnld_req
#define ifr_fddi_stat	ifr_ifru.ifru_fddi_stat
#define ifr_fddi_gstruct ifr_ifru.ifru_fddi_gstruct

#define ifr_ip_muxid	ifr_ifru.if_muxid[0]
#define ifr_arp_muxid	ifr_ifru.if_muxid[1]
};

#endif				/* IFNAMSIZ */

#ifdef __KERNEL__
#ifdef __LP64__

struct ifconf32 {
	int32_t ifc_len;
	union {
		uint32_t ifcu_buf;
		uint32_t ifcu_req;
	} ifc_ifcu;
};

#endif				/* __LP64__ */
#endif				/* __KERNEL__ */

#endif				/* _NET_LIF_H */
