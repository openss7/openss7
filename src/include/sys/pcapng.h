/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This library is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License
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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef _SYS_PCAPNG_H
#define _SYS_PCAPNG_H

/* This file can be processed with doxygen(1). */

#ifndef _PCAPNG_H
#error "Do not include sys/pcapng.h directly!"
#endif

#define OPT_ENDOFOPT	 0	// len 0
#define OPT_COMMENT	 1	// variable

#define SHB_HARDWARE	 2	// variable
#define SHB_OS		 3	// variable
#define SHB_USERAPPL	 4	// variable
#define SHB_UUID	0x5572	// len 16

#define IDB_NAME	 2	// variable
#define IDB_DESCRIPTION	 3	// variable
#define IDB_IPV4ADDR	 4	// len 8 (address/netmask)
#define IDB_IPV6ADDR	 5	// len 17 (address/prefixlen)
#define IDB_MACADDR	 6	// len 6
#define IDB_EUIADDR	 7	// len 8
#define IDB_SPEED	 8	// len 8
#define IDB_TSRESOL	 9	// len 1
#define IDB_TZONE	10	// len 4
#define IDB_FILTER	11	// variable
#define IDB_OS		12	// variable
#define IDB_FCSLEN	13	// len 1
#define IDB_TSOFFSET	14	// len 8 (high/low)
#define IDB_PHYSADDR	0x5572	// variable
#define IDB_UUID	0x5573	// len 16
#define IDB_IFINDEX	0x5574	// len 4

#define EPB_FLAGS	 2	// len 4
#define EPB_HASH	 3	// variable
#define EPB_DROPCOUNT	 4	// len 8
#define EPB_COMPCOUNT	0x5572	// len 8

#define OPB_FLAGS	 2	// len 4
#define OPB_HASH	 3	// variable

#define ISB_STARTTIME	 2	// len 8 (high/low)
#define ISB_ENDTIME	 3	// len 8 (high/low)
#define ISB_IFRECV	 4	// len 8
#define ISB_IFDROP	 5	// len 8
#define ISB_FILTERACCEPT 6	// len 8
#define ISB_OSDROP	 7	// len 8
#define ISB_USRDELIV	 8	// len 8
#define ISB_IFCOMP	0x5572	// len 8

#define PCAPNG_SHB_TYPE	0x0a0d0d0a
#define PCAPNG_IDB_TYPE	0x00000001
#define PCAPNG_OPB_TYPE 0x00000002
#define PCAPNG_SPB_TYPE 0x00000003
#define PCAPNG_NRB_TYPE 0x00000004
#define PCAPNG_ISB_TYPE 0x00000005
#define PCAPNG_EPB_TYPE 0x00000006
#define PCAPNG_ITB_TYPE 0x00000007
#define PCAPNG_AEB_TYPE 0x00000008

#define PCAPNG_MAGIC	0x1a2b3c4d

struct var {
	char *var_value;
	int var_length;
};

struct opt {
	uint16_t opt_code;
	uint16_t opt_len;
	uint32_t opt_value[0];
};

struct shb_info {
	uint16_t shbi_major;		/* always 0x0001 */
	uint16_t shbi_minor;		/* always 0x0000 */
	uint64_t shbi_section_length;	/* 0xffffffffffffffff when unknown */
	char *shbi_comment;
	char *shbi_hardware;
	char *shbi_os;
	char *shbi_userappl;
	uint8_t *shbi_uuid;		/* len 16 */
};

struct idb_info {
	uint16_t idbi_linktype;
	uint32_t idbi_snaplen;
	char *idbi_comment;
	char *idbi_name;
	char *idbi_desc;
	uint8_t *idbi_ipv4addr;		/* len 8 */
	uint8_t *idbi_ipv6addr;		/* len 17 */
	uint8_t *idbi_macaddr;		/* len 6 */
	uint8_t *idbi_euiaddr;		/* len 8 */
	uint64_t idbi_speed;		/* len 8 */
	uint8_t idbi_tsresol;		/* len 1 */
	uint32_t idbi_tzone;		/* len 4 */
	struct var idbi_filter;
	char *idbi_os;
	uint8_t idbi_fcslen;		/* len 1 */
	uint32_t idbi_tsoffset[2];
	struct var idbi_physaddr;
	uint8_t *idbi_uuid;		/* len 16 */
	uint32_t idbi_ifindex;		/* len 4 */
};

struct epb_info {
	uint32_t epbi_interfaceid;
	uint32_t epbi_ts[2];
	uint32_t epbi_captlen;
	uint32_t epbi_packlen;
	struct var epbi_data;
	char *epbi_comment;
	uint32_t epbi_flags;		/* len 4 */
	struct var epbi_hash;
	uint64_t epbi_dropcount;
	uint64_t epbi_compcount;
};

struct spb_info {
	uint32_t spbi_packlen;
	struct var spbi_data;
};

struct opb_info {
	uint16_t opbi_interfaceid;
	uint16_t opbi_drops;
	uint32_t opbi_ts[2];
	uint32_t opbi_captlen;
	uint32_t opbi_packlen;
	struct var opbi_data;
	char *opbi_comment;
	uint32_t opbi_flags;		/* len 4 */
	struct var opbi_hash;
};

struct isb_info {
	uint32_t isbi_interfaceid;
	uint32_t isbi_ts[2];
	const char *isbi_comment;
	uint32_t isbi_starttime[2];
	uint32_t isbi_endtime[2];
	uint64_t isbi_ifrecv;		/* len 8 */
	uint64_t isbi_ifdrop;		/* len 8 */
	uint64_t isbi_filteraccept;	/* len 8 */
	uint64_t isbi_osdrop;		/* len 8 */
	uint64_t isbi_usrdeliv;		/* len 8 */
	uint64_t isbi_ifcomp;		/* len 8 */
};

struct pcapng_pkthdr {
	uint32_t pph_interfaceid;
	uint32_t pph_ts[2];
	uint32_t pph_captlen;
	uint32_t pph_packlen;
	uint32_t pph_datalen;
	uint32_t pph_flags;
	uint64_t pph_dropcount;
	uint64_t pph_compcount;
};

#define PCAPNG_FLAG_DIR_MASK		0x00000003
#define PCAPNG_FLAG_DIR_UNKNOWN		0x00000000
#define PCAPNG_FLAG_DIR_INBOUND		0x00000001
#define PCAPNG_FLAG_DIR_OUTBOUND	0x00000002
#define PCAPNG_FLAG_DIR_INVALID		0x00000003
#define PCAPNG_FLAG_DIR(flag)		((flag&PCAPNG_FLAG_DIR_MASK)>>0)

#define PCAPNG_FLAG_TYP_MASK		0x0000001c
#define PCAPNG_FLAG_TYP_UNSPEC		0x00000000
#define PCAPNG_FLAG_TYP_UNICAST		0x00000004
#define PCAPNG_FLAG_TYP_MULTICAST	0x00000008
#define PCAPNG_FLAG_TYP_BROADCAST	0x0000000c
#define PCAPNG_FLAG_TYP_PROMISCUOUS	0x00000010
#define PCAPNG_FLAG_TYP(flag)		((flag&PCAPNG_FLAG_TYP_MASK)>>2)

#define PCAPNG_FLAG_FCS_MASK		0x000001e0
#define PCAPNG_FLAG_FCS_UNAVAIL		0x00000000
#define PCAPNG_FLAG_FCS_VAL(val)	((val<<5)&PCAPNG_FLAG_FCS_MASK)
#define PCAPNG_FLAG_FCS(flag)		((flag&PCAPNG_FLAG_FCS_MASK)>>5)

#define PCAPNG_FLAG_ERR_MASK		0xff000000
#define PCAPNG_FLAG_ERR_SYMBOL		0x80000000
#define PCAPNG_FLAG_ERR_PREAMBLE	0x40000000
#define PCAPNG_FLAG_ERR_START		0x20000000
#define PCAPNG_FLAG_ERR_UNALIGNED	0x10000000
#define PCAPNG_FLAG_ERR_GAP		0x08000000
#define PCAPNG_FLAG_ERR_SHORT		0x04000000
#define PCAPNG_FLAG_ERR_LONG		0x02000000
#define PCAPNG_FLAG_ERR_CRC		0x01000000
#define PCAPNG_FLAG_ERR(flag)		((flag*PCAPNG_FLAG_ERR_MASK)>>24)

typedef struct pcapng_pcap {
	pcap_t *pcap;			/**< the original pcap structure */
	struct idb_info idbi;		/**< interface information */
	struct isb_info isbi;		/**< interface statistics */
} pcapng_pcap_t;

typedef struct pcapng {
	pthread_rwlock_t lock;		/**< lock for this structure */
	struct shb_info shbi;		/**< section header information */
	int swapped;			/**< file or stream being read is byte swapped */
	int savefile;			/**< file or stream being read is a savefile */
	int if_count;			/**< number of assigned interfaces */
	pcapng_pcap_t **interfaces;	/**< assigned interfaces */
} pcapng_t;

typedef struct pcapng_stats {
	struct pcapng_stats *ps_next;	/**< next stats structure in chain */
	struct isb_info ps_isbi;	/**< interface stats block information */
} pcapng_stats_t;

#define ps_interfaceid		ps_isbi.isbi_interfaceid
#define ps_ts			ps_isbi.isbi_ts
#define ps_comment		ps_isbi.isbi_comment
#define ps_starttime		ps_isbi.isbi_starttime
#define ps_endtime		ps_isbi.isbi_endtime
#define ps_ifrecv		ps_isbi.isbi_ifrecv
#define ps_ifdrop		ps_isbi.isbi_ifdrop
#define ps_filteraccept		ps_isbi.isbi_filteraccept
#define ps_osdrop		ps_isbi.isbi_osdrop
#define ps_usrdeliv		ps_isbi.isbi_usrdeliv
#define ps_ifcomp		ps_isbi.isbi_ifcomp

typedef struct pcapng_if {
	struct pcapng_if *if_next;	/**< next interface structure in chain */
	struct idb_info if_idbi;	/**< interface desc block information */
} pcapng_if_t;

#define if_linktype		if_isbi.idbi_linktype
#define if_snaplen		if_isbi.idbi_snaplen
#define if_comment		if_isbi.idbi_comment
#define if_name			if_isbi.idbi_name
#define if_desc			if_isbi.idbi_desc
#define if_ipv4addr		if_isbi.idbi_ipv4addr
#define if_ipv6addr		if_isbi.idbi_ipv6addr
#define if_macaddr		if_isbi.idbi_macaddr
#define if_euiaddr		if_isbi.idbi_euiaddr
#define if_speed		if_isbi.idbi_speed
#define if_tsresol		if_isbi.idbi_tsresol
#define if_tzone		if_isbi.idbi_tzone
#define if_filter		if_isbi.idbi_filter
#define if_os			if_isbi.idbi_os
#define if_fcslen		if_isbi.idbi_fcslen
#define if_tsoffset		if_isbi.idbi_tsoffset
#define if_physaddr		if_isbi.idbi_physaddr
#define if_uuid			if_isbi.idbi_uuid
#define if_ifindex		if_isbi.idbi_ifindex

typedef int (*pcapng_handler)(void *, const struct pcapng_pkthdr *, const u_char *);

#endif				/* _SYS_PCAPNG_H */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
