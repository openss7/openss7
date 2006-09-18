/*****************************************************************************

 @(#) $Id: npi_mtp.h,v 0.9.2.3 2006/09/18 13:52:37 brian Exp $

 -----------------------------------------------------------------------------

     Copyright (C) 1997-2002 OpenSS7 Corporation.  All Rights Reserved.

                                  PUBLIC LICENSE

     This license is provided without fee, provided that the above copy-
     right notice and this public license must be retained on all copies,
     extracts, compilations and derivative works.  Use or distribution of
     this work in a manner that restricts its use except as provided here
     will render this license void.

     The author(s) hereby waive any and all other restrictions in respect
     of their copyright in this software and its associated documentation.
     The authors(s) of this software place in the public domain any novel
     methods or processes which are embodied in this software.

     The author(s) undertook to write it for the sake of the advancement
     of the Arts and Sciences, but it is provided as is, and the author(s)
     will not take any responsibility in it.

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

 Last Modified $Date: 2006/09/18 13:52:37 $ by $Author: brian $

 *****************************************************************************/

#ifndef _SYS_NPI_MTP_H
#define _SYS_NPI_MTP_H

#ident "@(#) $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/*
 *  LiS npi.h is version 1
 */
#ifndef N_VERSION_2

#define N_CURRENT_VERSION   0x02	/* current version of NPI */
#define N_VERSION_2	    0x02	/* version of npi, December 16, 1991 */

/*
 * Information acknowledgement
 */
typedef struct {
	np_ulong PRIM_type;		/* always N_INFO_ACK */
	np_ulong NSDU_size;		/* maximum NSDU size */
	np_ulong ENSDU_size;		/* maximum ENSDU size */
	np_ulong CDATA_size;		/* connect data size */
	np_ulong DDATA_size;		/* discon data size */
	np_ulong ADDR_size;		/* address size */
	np_ulong ADDR_length;		/* address length */
	np_ulong ADDR_offset;		/* address offset */
	np_ulong QOS_length;		/* QOS values length */
	np_ulong QOS_offset;		/* QOS values offset */
	np_ulong QOS_range_length;	/* length of QOS values' range */
	np_ulong QOS_range_offset;	/* offset of QOS values' range */
	np_ulong OPTIONS_flags;		/* bit masking for options supported */
	np_ulong NIDU_size;		/* network i/f data unit size */
	long SERV_type;			/* service type */
	np_ulong CURRENT_state;		/* current state */
	np_ulong PROVIDER_type;		/* type of NS provider */
	np_ulong NODU_size;		/* optimal NSDU size */
	np_ulong PROTOID_length;	/* length of bound protocol ids */
	np_ulong PROTOID_offset;	/* offset of bound protocol ids */
	np_ulong NPI_version;		/* version # of npi that is supported */
} __N_info_ack_t;
#define N_info_ack_t __N_info_ack_t

/*
 * Bind acknowledgement
 */
typedef struct {
	np_ulong PRIM_type;		/* always N_BIND_ACK */
	np_ulong ADDR_length;		/* address length */
	np_ulong ADDR_offset;		/* offset of address */
	np_ulong CONIND_number;		/* connection indications */
	np_ulong TOKEN_value;		/* value of "token" assigned to stream */
	np_ulong PROTOID_length;	/* length of bound protocol ids */
	np_ulong PROTOID_offset;	/* offset of bound protocol ids */
} __N_bind_ack_t;
#define N_bind_ack_t __N_bind_ack_t

#endif				/* N_VERSION_2 */

#ifndef NSF_UNBND
#define NSF_UNBND	(1<<NS_UNBND	  )
#define NSF_WACK_BREQ	(1<<NS_WACK_BREQ  )
#define NSF_WACK_UREQ	(1<<NS_WACK_UREQ  )
#define NSF_IDLE	(1<<NS_IDLE	  )
#define NSF_WACK_OPTREQ	(1<<NS_WACK_OPTREQ)
#define NSF_WACK_RRES	(1<<NS_WACK_RRES  )
#define NSF_WCON_CREQ	(1<<NS_WCON_CREQ  )
#define NSF_WRES_CIND	(1<<NS_WRES_CIND  )
#define NSF_WACK_CRES	(1<<NS_WACK_CRES  )
#define NSF_DATA_XFER	(1<<NS_DATA_XFER  )
#define NSF_WCON_RREQ	(1<<NS_WCON_RREQ  )
#define NSF_WRES_RIND	(1<<NS_WRES_RIND  )
#define NSF_WACK_DREQ6	(1<<NS_WACK_DREQ6 )
#define NSF_WACK_DREQ7	(1<<NS_WACK_DREQ7 )
#define NSF_WACK_DREQ9	(1<<NS_WACK_DREQ9 )
#define NSF_WACK_DREQ10	(1<<NS_WACK_DREQ10)
#define NSF_WACK_DREQ11	(1<<NS_WACK_DREQ11)
#endif				/* NSF_UNBND */

#ifndef __HAVE_MTP_ADDR
typedef struct mtp_addr {
	unsigned int family __attribute__ ((packed));
	unsigned short int ni __attribute__ ((packed));
	unsigned short int si __attribute__ ((packed));
	unsigned int pc __attribute__ ((packed));
} mtp_addr_t;
#define __HAVE_MTP_ADDR
#endif

#define N_QOS_SEL_DATA_MTP	0x0801
typedef struct {
	ulong n_qos_type;		/* always N_QOS_SEL_DATA_MTP */
	ulong sls;			/* signalling link selection */
	ulong mp;			/* message priority */
} N_qos_sel_data_mtp_t;

#define N_QOS_SEL_CONN_MTP	0x0802
typedef struct {
	ulong n_qos_type;		/* always N_QOS_SEL_CONN_MTP */
} N_qos_sel_conn_mtp_t;

#define N_QOS_SEL_INFO_MTP	0x0803
typedef struct {
	ulong n_qos_type;		/* always N_QOS_SEL_INFO_MTP */
	ulong pvar;			/* protocol variant */
	ulong popt;			/* protocol options */
	ulong sls;			/* signalling link selection */
	ulong mp;			/* message priority */
} N_qos_sel_info_mtp_t;

#define N_QOS_RANGE_INFO_MTP	0x0804
typedef struct {
	ulong n_qos_type;		/* always N_QOS_RANGE_INFO_MTP */
	ulong sls_range;		/* signalling link selection mask */
	ulong mp_range;			/* message priority mask */
} N_qos_range_info_mtp_t;

typedef union N_qos_mtp {
	ulong n_qos_type;
	N_qos_sel_conn_mtp_t n_qos_conn;
	N_qos_sel_data_mtp_t n_qos_data;
	N_qos_sel_info_mtp_t n_qos_info;
	N_qos_range_info_mtp_t n_qos_range;
} N_qos_mtp_t;

#define N_MTP_M_CLUSTER			(0x00000100)
#define N_MTP_M_MEMBER			(0x00001000)
#define N_MTP_AVAILABLE			(0x00000001)
#define N_MTP_PROHIBITED		(0x00000002)
#define N_MTP_CONGESTED(__s)		(0x00000003|((__s)<<16))
#define N_MTP_DEST_AVAILABLE		(N_MTP_AVAILABLE|N_MTP_M_MEMBER )
#define N_MTP_DEST_PROHIBITED		(N_MTP_PROHIBITED|N_MTP_M_MEMBER )
#define N_MTP_DEST_CONGESTED(__s)	(N_MTP_CONGESTED(__s)|N_MTP_M_MEMBER )
#define N_MTP_CLUS_AVAILABLE		(N_MTP_AVAILABLE|N_MTP_M_CLUSTER)
#define N_MTP_CLUS_PROHIBITED		(N_MTP_PROHIBITED|N_MTP_M_CLUSTER)
#define N_MTP_CLUS_CONGESTED(__s)	(N_MTP_CONGESTED(__s)|N_MTP_M_CLUSTER)
#define N_MTP_RESTARTING		(0x00001004)
#define N_MTP_RESTARTED			(0x00001005)
#define N_MTP_USER_PART_UNKNOWN		(0x00001006)
#define N_MTP_USER_PART_UNEQUIPPED	(0x00001007)
#define N_MTP_USER_PART_UNAVAILABLE	(0x00001008)
#define N_MTP_CONGESTION_STATUS(__x)	(((__x)>>16))

#endif				/* _SYS_NPI_MTP_H */
