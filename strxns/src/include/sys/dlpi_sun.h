/*****************************************************************************

 @(#) $Id: dlpi_sun.h,v 0.9.2.1 2008-06-18 16:43:16 brian Exp $

 -----------------------------------------------------------------------------

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

 -----------------------------------------------------------------------------

 Last Modified $Date: 2008-06-18 16:43:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dlpi_sun.h,v $
 Revision 0.9.2.1  2008-06-18 16:43:16  brian
 - added new files for NLI and DLPI

 *****************************************************************************/

#ifndef __SYS_DLPI_SUN_H__
#define __SYS_DLPI_SUN_H__

#ident "@(#) $RCSfile: dlpi_sun.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

#define DL_NOTIFY_REQ			0x100	/* Enable notifications */
#define DL_NOTIFY_ACK			0x101	/* Supported notifications */
#define DL_NOTIFY_IND			0x102	/* Notification from provider */
#define DL_AGGR_REQ			0x103	/* Enable link aggregation */
#define DL_AGGR_IND			0x104	/* Result from link aggregation */
#define DL_UNAGGR_REQ			0x105	/* Disable link aggregation */
#define DL_CAPABILITY_REQ		0x110	/* Capability request */
#define DL_CAPABILITY_ACK		0x111	/* Capability ack */
#define DL_CONTROL_REQ			0x112	/* Device specific control request */
#define DL_CONTROL_ACK			0x113	/* Device specific control ack */
#define DL_PASSIVE_REQ			0x114	/* Allow access to aggregated link */
#define DL_INTR_MODE_REQ		0x115	/* Request Rx processing in INTR mode */

#define DA_ICV_MAX_LEN 128 /* ??? */

/* Defines the form of a M_CTL message for passing to the driver. */
typedef struct da_ipsec {
	int da_type;			/* M_CTL message ident */
	int da_flag;
	uint32_t da_icv_len;		/* da_icv length in bytes */
	uint8_t da_icv[DA_ICV_MAX_LEN];	/* ICV for AH or ESP+auth */
} da_ipsec_t;

#define IPHADA_M_CTL	0xA1D53DE5U

/*
 * SUN additional dl_mac_type's for use with DL_INFO_ACK
 */
#define DL_IPV4				0x80000001UL	/* IPv4 Tunnel Link */
#define DL_IPV6				0x80000002UL	/* IPv6 Tunnel Link */
#define DL_VNI				0x80000003UL	/* Virtual network interface */
#define DL_WIFI				0x80000004UL	/* IEEE 802.11 */

/*
   DPLI notification codes for DL_NOTIFY primitives.  Bit-wise distinct since
   DL_NOTIFY_REQ and DL_NOTIFY_ACK carry multiple notification codes.
 */
#define DL_NOTE_PHYS_ADDR		(1UL<< 0)	/* Physical address change */
#define DL_NOTE_PROMISC_ON_PHYS		(1UL<< 1)	/* DL_PROMISC_PHYS set on ppa */
#define DL_NOTE_PROMISC_OFF_PHYS	(1UL<< 2)	/* DL_PROMISC_PHYS cleared on ppa */
#define DL_NOTE_LINK_DOWN		(1UL<< 3)	/* Link down */
#define DL_NOTE_LINK_UP			(1UL<< 4)	/* Link up */
#define DL_NOTE_AGGR_AVAIL		(1UL<< 5)	/* Link aggregation is available */
#define DL_NOTE_AGGR_UNAVAIL		(1UL<< 6)	/* Link aggregation is not available */
#define DL_NOTE_SDU_SIZE		(1UL<< 7)	/* New SDU size, global or per addr */
#define DL_NOTE_SPEED			(1UL<< 8)	/* Approximate link speed */
#define DL_NOTE_FASTPATH_FLUSH		(1UL<< 9)	/* Fast Path info changes */
#define DL_NOTE_CAPAB_RENEG		(1UL<<10)	/* Initiate capability renegotiation */

/* 
   The following are the capability types and structures used by the DL_CAPABILITY_REQ and
   DL_CAPABILITY_ACK primitives.

   These primitives are used both to determine the set of capabilities in the DLS provider and also
   to turn on and off specific capabiltiies.  The response is a DL_CAPABILITY_ACK or DL_ERROR_ACK.

   DL_CAPABILITY_REQ can either be empty (i.e., dl_sub_length is zero) which is a request for the
   driver to return all capabilities.  Otherwise, the DL_CAPABILITY_REQ contains the capabilities
   the DLS user wants to use and their settings.

   DL_CAPABILITY_ACK contains the capabilities that the DLS provider can support modified based on
   what was listed in the request.  If a capability was included in the request then the information
   returned in the ack might be modified based on the inofmration in the request.
 */

#define DL_CAPAB_ID_WRAPPR	0x00	/* Module ID wrapper: dl_data is dl_capab_id_t */
#define DL_CAPAB_HCKSUM		0x01	/* Checksum offload: dl_data is dl_capab_hcksum_t */
#define DL_CAPAB_IPSEC_AH	0x02	/* IPsec AH acceleration: dl_data is dl_capab_ipsec_t */
#define DL_CAPAB_IPSEC_ESP	0x03	/* IPsec ESP acceleration: dl_data is dl_capab_ipsec_t */
#define DL_CAPAB_MDT		0x04	/* Multidata Transmit: dl_data is dl_capab_mdt_t */
#define DL_CAPAB_ZEROCOPY	0x05	/* Zero-copy: dl_data is dl_capab_zerocopy_t */
#define DL_CAPAB_POLL		0x06	/* Polling: dl_data is dl_capab_dls_t */
#define DL_CAPAB_SOFT_RING	0x07	/* Soft ring: dl_data is dl_capab_dls_t */
#define DL_CAPAB_LSO		0x08	/* Large Send Offload: dl_data is dl_capab_lso_t */

typedef struct {
	dl_ulong dl_cap;		/* capability type */
	dl_ulong dl_length;		/* length of data following */
	char dl_data[0];		/* Followed by zero of more byte of dl_data */
} dl_capability_sub_t;

/* 
   Definitions and structures needed for DL_CONTROL_REQ and DL_CONTROL_ACK primitives.  Extensible
   message to send down control information to the DLS provider.  The response is a DL_CONTROL_ACK
   or DL_ERROR_ACK.

   Different types of control operations will define different formats for the key and data fields.
   ADD requires key and data fields; if the <type, key> matches an already existing entry a
   DL_ERROR_ACK will be returned, DELETE requires a key field; if the <type, key> does not exist, a
   DL_ERROR_ACK will be returned.  FLUSH requires neither a key nor data; it unconditionally removes
   all entries for the specified type.  GET requires a key field; the get operation returns the data
   for the <type, key>.  If <type, key> does not exist a DL_ERROR_ACK is returned.  UPDATE requires
   key and data fields; if <type, key> does not exist a DL_ERROR_ACK-is returned.
 */

/* 
   Control operations
 */
#define DL_CO_ADD	0x01	/* Add new entry matching for <type, key> */
#define DL_CO_DELETE	0x02	/* Delete the entry matching <type, key> */
#define DL_CO_FLUSH	0x03	/* Purge all entries of <type> */
#define DL_CO_GET	0x04	/* Get the data for the <type, key> */
#define DL_CO_UPDATE	0x05	/* Update the data for <type, key> */
#define DL_CO_SET	0x06	/* Add or update as appropriate */

/* 
   Control types (dl_type field of dl_control_req_t or dl_control_ack_t)
 */
#define DL_CT_IPSEC_AH	0x01	/* AH; key=spi,dest_addr; data=keyring material */
#define DL_CT_IPSEC_ESP	0x02	/* ESP; key=spi,dest_taddr; data=keyring material */

/*
   For DL_CT_IPSEC_AH and DL_CT_IPSEC_ESP, the optional dl_key data that
   follows the dl_control_req_t or dl_control_ack_t will be the IPsec SPI
   (Security Parameters Index) value and the destination address.  This is
   define as being unique per protocol.
 */

#define DL_CTL_IPSEC_ADDR_LEN	16	/* IP addree length in bytes */

typedef struct dl_ct_ipsec_key {
	uint32_t dl_key_spi;		/* Security Parameters Index value */
	uint8_t dl_key_dest_addr[DL_CTL_IPSEC_ADDR_LEN];	/* dest IP address */
	uint32_t dl_key_addr_family;	/* family of dest IP address (AF_INET or AF_INET6) */
} dl_ct_ipsec_key_t;

#define DL_CT_IPSEC_MAX_KEY_LEN	512	/* max key length in bytes */

/*
   Possible flags fro sadb_sa_flags.
 */
#define DL_CT_IPSEC_INBOUND	0x01	/* SA can be used for inbound pkts */
#define DL_CT_IPSEC_OUTBOUND	0x02	/* SA can be used for outbound pkts */

/*
 * minimal SADB entry content
 * fields are defined as per RFC 2367 and <net/pfkeyv2.h>
 * This defines the content and format of the dl_data portion of the
 * dl_control_req_t or dl_control_ack_t.
 */
typedef struct dl_ct_ipsec {
	uint8_t sadb_sa_auth;		/* Authentication algorithm */
	uint8_t sadb_sa_encrypt;	/* Encryption algorithm */
	uint32_t sadb_sa_flags;		/* SA flags. */
	uint16_t sadb_key_len_a;	/* auth key length in bytes */
	uint16_t sadb_key_bits_a;	/* auth key length in bits */
	uint16_t sadb_key_data_a[DL_CT_IPSEC_MAX_KEY_LEN];	/* auth key data */
	uint16_t sadb_key_len_e;	/* encr key length in bytes */
	uint16_t sadb_key_bits_e;	/* encr key length in bits */
	uint16_t sadb_key_data_e[DL_CT_IPSEC_MAX_KEY_LEN];	/* encr key data */
} dl_ct_ipsec_t;

/* 
   Module ID token to be included in new sub-capability structures.  Existing sub-capability lacking
   an identification token, e.g., IPSEC hardware acceleration, need to be encapsulated within the ID
   sub-capability.  Access to this structure must be done through dlcapab{set,check}qid().
 */
typedef struct {
	dl_ulong mid[4];		/* private fields */
} dl_mid_t;

/* 
   Module ID wrapper (follows dl_capability_sub_t)
 */
typedef struct {
	dl_mid_t id_mid;		/* module ID token */
	dl_capability_sub_t id_subcap;	/* sub-capability */
} dl_capab_id_t;

/* 
   Multidata Transmit sub-capability (follows dl_capability_sub_t)
 */
typedef struct {
	dl_ulong mdt_version;		/* interface version */
	dl_ulong mdt_flags;		/* flags */
	dl_ulong mdt_hdr_head;		/* minimum leading header space */
	dl_ulong mdt_hdr_tail;		/* minimum trailing header space */
	dl_ulong mdt_max_pld;		/* maximum doable payload buffers */
	dl_ulong mdt_span_limit;	/* scatter-gather descriptor limit */
	dl_mid_t mdt_mid;		/* module ID token */
} dl_capab_mdt_t;

/* 
   Multidata Transmit revision defintion history
 */
#define MDT_CURRENT_VERSION	0x02
#define MDT_VERSION_2		0x02

/* 
   mdt_flags values
 */
#define DL_CAPAB_MDT_ENABLE	0x01	/* enable Multidata transmit */

/* 
   DL_CAPAB_HCKSUM.  Used for negotiating different flavors of checksum offload capabilities.
 */
typedef struct {
	dl_ulong hcksum_version;	/* version of data following */
	dl_ulong hcksum_txflags;	/* capabilities on transmit */
	dl_mid_t hcksum_mid;		/* module ID */
} dl_capab_hcksum_t;

/* 
   DL_CAPAB_HCKSUM revision defintion history
 */
#define HCKSUM_CURRENT_VERSION	0x01
#define HCKSUM_VERSION_1	0x01

/* 
   Values for hcksum_txflags
 */
#define HCKSUM_ENABLE		0x01	/* Set to enable hardware checksum capability */
#define HCKSUM_INET_PARTIAL	0x02	/* Partial 1's complement checksum ability */
#define HCKSUM_INET_FULL_V4	0x04	/* Full 1's complement checksum ability for IPv4 packets. */
#define HCKSUM_INET_FULL_V6	0x08	/* Full 1's complement checkusm ability for IPv6 packets. */
#define HCKSUM_IPDHRCHKSUM	0x10	/* IPv4 Header checksum offload capability */

/*
   IPSec algorithms capabilities (cip_data in dl_capab_ipsec_t)
 */
typedef struct {
	dl_ulong alg_type;
	dl_ulong alg_prim;		/* algorithm primitive */
	dl_ulong alg_thruput;		/* approx throughput metric tin Mb/s */
	dl_ulong alg_flag;		/* flags */
	dl_ulong alg_minbits;		/* minimum key len in bits */
	dl_ulong alg_maxbits;		/* maximum key len in bits */
	dl_ulong alg_incrbits;		/* key len increment in bits */
} dl_capab_ipsec_alg_t;

/*
   IPSec sub-capability (follows dl_capability_sub_t)
 */
typedef struct {
	dl_ulong cip_version;		/* interface version */
	dl_ulong cip_nciphers;		/* number ciphers supported */
	dl_capab_ipsec_alg_t cip_data[1];	/* data */
} dl_capabl_ipsec_t;

/*
   Algorithm types (alg_type field of dl_capab_ipsec_alg_t)
 */
#define DL_CAPAB_IPSEC_ALG_AUTH		0x01	/* authentication alg. */
#define DL_CAPAB_IPSEC_ALG_ENCR		0x02	/* encryption alg. */

/* alg_prim ciphers */
#define DL_CAPAB_IPSEC_ENCR_DES		0x02
#define DL_CAPAB_IPSEC_ENCR_3DES	0x03
#define DL_CAPAB_IPSEC_ENCR_BLOWFISH	0x04
#define DL_CAPAB_IPSEC_ENCR_NULL	0x0b	/* no encryption */
#define DL_CAPAB_IPSEC_ENCR_AES		0x0c

/* alg_prim authentications */
#define DL_CAPAB_IPSEC_AUTH_NONE	0x00	/* no authentication */
#define DL_CAPAB_IPSEC_AUTH_MD5HMAC	0x02
#define DL_CAPAB_IPSEC_AUTH_SHA1HMAC	0x03

/* alg_flag values */
#define DL_CAPAB_ALG_ENABLE		0x01	/* enable this algorithm */

#ifdef _KERNEL

/*
 * This structure is used by DL_CAPAB_POLL and DL_CAPAB_SOFT_RING capabilities. It provides a
 * mechanism for IP to exchange function pointers with a gldv3-based driver to enable it to bypass
 * streams- data-paths. DL_CAPAB_POLL mechanism provides a way to blank interrupts. Note: True
 * polling support will be added in the future.  DL_CAPAB_SOFT_RING provides a mechanism to create
 * soft ring at the dls layer.
 */
typedef struct dl_capab_dls_s {
	dl_ulong dls_version;
	dl_ulong dls_flags;

	/* DLD provided information */
	uintptr_t dls_tx_handle;
	uintptr_t dls_tx;
	uintptr_t dls_ring_change_status;
	uintptr_t dls_ring_bind;
	uintptr_t dls_ring_unbind;

	/* IP provided information */
	uintptr_t dls_rx_handle;
	uintptr_t dls_ring_assign;
	uintptr_t dls_rx;
	uintptr_t dls_ring_add;
	dl_ulong dls_ring_cnt;

	dl_mid_t dls_mid;		/* module ID */
} dl_capab_dls_t;

#define	POLL_CURRENT_VERSION		0x01
#define	POLL_VERSION_1			0x01

#define	SOFT_RING_VERSION_1		0x01

/* 
   Values for poll_flags
 */
#define	POLL_ENABLE			0x01	/* Set to enable polling capability */
#define	POLL_CAPABLE			0x02	/* Polling ability exists */
#define	POLL_DISABLE			0x03	/* Disable Polling */

/* 
   Values for soft_ring_flags
 */
#define	SOFT_RING_ENABLE		0x04	/* Set to enable soft_ring capability */
#define	SOFT_RING_CAPABLE		0x05	/* Soft_Ring ability exists */
#define	SOFT_RING_DISABLE		0x06	/* Disable Soft_Ring */

/* 
   Soft_Ring fanout types (used by soft_ring_change_status)
 */
#define	SOFT_RING_NONE			0x00
#define	SOFT_RING_FANOUT		0x01

#endif				/* _KERNEL */

/* 
   Zero-copy sub-capability (follows dl_capability_sub_t)
 */
typedef struct {
	dl_ulong zerocopy_version;	/* interface version */
	dl_ulong zerocopy_flags;	/* capability flags */
	dl_ulong reserved[9];		/* reserved fields */
	dl_mid_t zerocopy_mid;		/* module ID */
} dl_capab_zerocopy_t;

/*
 * Zero-copy revision definition history
 */
#define	ZEROCOPY_CURRENT_VERSION	0x01
#define	ZEROCOPY_VERSION_1		0x01

/*
 * Currently supported values of zerocopy_flags
 */
#define	DL_CAPAB_VMSAFE_MEM		0x01	/* Driver is zero-copy safe wrt VM named buffers on 
						   transmit */

/*
 * Large Send Offload sub-capability (follows dl_capability_sub_t)
 */
typedef struct {
	dl_ulong lso_version;		/* interface version */
	dl_ulong lso_flags;		/* capability flags */
	dl_ulong lso_max;		/* maximum payload */
	dl_ulong reserved[1];		/* reserved fields */
	dl_mid_t lso_mid;		/* module ID */
} dl_capab_lso_t;

/*
 * Large Send Offload revision definition history
 */
#define	LSO_CURRENT_VERSION		0x01
#define	LSO_VERSION_1			0x01

/*
 * Currently supported values of lso_flags
 */
#define	LSO_TX_ENABLE			0x01	/* to enable LSO */
#define	LSO_TX_BASIC_TCP_IPV4		0x02	/* TCP LSO capability */

/* 
   DL_NOTIFY_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_NOTIFY_REQ */
	uint32_t dl_notifications;	/* requested set of notifications */
	uint32_t dl_timelimit;		/* In milliseconds */
} dl_notify_req_t;

/* 
   DL_NOTIFY_ACK, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_NOTIFY_ACK */
	uint32_t dl_notifications;	/* supported set of notifications */
} dl_notify_ack_t;

/* 
   DL_NOTIFY_IND, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_NOTIFY_IND */
	uint32_t dl_notification;	/* Which notification? */
	uint32_t dl_data;		/* notification specific */
	dl_ulong dl_addr_length;	/* length of complete DLSAP addr */
	dl_ulong dl_addr_offset;	/* offset from start of M_PROTO */
} dl_notify_ind_t;

/* 
   DL_AGGR_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_AGGR_REQ */
	uint32_t dl_key;		/* Key identifying the group */
	uint32_t dl_port;		/* Identifying the NIC */
	dl_ulong dl_addr_length;	/* length of PHYS addr */
	dl_ulong dl_addr_offset;	/* offset from start of M_PROTO */
} dl_aggr_req_t;

/* 
   DL_AGGR_IND, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_AGGR_IND */
	uint32_t dl_key;		/* Key identifying the group */
	uint32_t dl_port;		/* Identifying the NIC */
	dl_ulong dl_addr_length;	/* length of PHYS addr */
	dl_ulong dl_addr_offset;	/* offset from start of M_PROTO */
} dl_aggr_ind_t;

/* 
   DL_UNAGGR_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_UNAGGR_REQ */
	uint32_t dl_key;		/* Key identifying the group */
	uint32_t dl_port;		/* Identifying the NIC */
	dl_ulong dl_addr_length;	/* length of PHYS addr */
	dl_ulong dl_addr_offset;	/* offset from start of M_PROTO */
} dl_unaggr_req_t;

/* 
   DL_CAPABILITY_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_CAPABILITY_REQ */
	dl_ulong dl_sub_offset;		/* options offset */
	dl_ulong dl_sub_length;		/* options length */
	/* Followed by a list of zero or more dl_capability_sub_t */
} dl_capability_req_t;

/* 
   DL_CAPABILITY_ACK, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_CAPABILITY_ACK */
	dl_ulong dl_sub_offset;		/* options offset */
	dl_ulong dl_sub_length;		/* options length */
	/* Followed by a list of zero or more dl_capability_sub_t */
} dl_capability_ack_t;

/* 
   DL_CONTROL_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_CONTROL_REQ */
	dl_ulong dl_operation;		/* add/delete/purge */
	dl_ulong dl_type;		/* e.g. AH/ESP/ .. */
	dl_ulong dl_key_offset;		/* offset of key */
	dl_ulong dl_key_length;		/* length of key */
	dl_ulong dl_data_offset;	/* offset of data */
	dl_ulong dl_data_length;	/* length of data */
} dl_control_req_t;

/* 
   DL_CONTROL_ACK, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_CONTROL_ACK */
	dl_ulong dl_operation;		/* add/delete/purge */
	dl_ulong dl_type;		/* e.g. AH/ESP/ .. */
	dl_ulong dl_key_offset;		/* offset of key */
	dl_ulong dl_key_length;		/* length of key */
	dl_ulong dl_data_offset;	/* offset of data */
	dl_ulong dl_data_length;	/* length of data */
} dl_control_ack_t;

/* 
   DL_PASSIVE_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_PASSIVE_REQ */
} dl_passive_req_t;

/* 
   DL_INTR_MOD_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_INTR_MODE_REQ */
	dl_ulong dl_sap;
	dl_ulong dl_imode;		/* intr mode: 0 off, 1 on */
} dl_intr_mode_req_t;

#define DLIOC			('D' << 8)
#define DLIOCRAW		(DLIOC|1)	/* M_DATA "raw" mode */
#define DLIOCNATIVE		(DLIOC|2)	/* Native traffic mode */
#define DLIOCMARGININFO		(DLIOC|3)	/* margin size info */
#define DLIOCHDRINFO		(DLIOC|10)	/* IP fast-path */

#define DL_IOC_HDR_INFO	DLIOCHDRINFO

#endif				/* __SYS_DLPI_SUN_H__ */
