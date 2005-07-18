/*****************************************************************************

 @(#) $Id: npi_sctp.h,v 0.9.2.3 2005/07/18 12:53:09 brian Exp $

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

 Last Modified $Date: 2005/07/18 12:53:09 $ by $Author: brian $

 *****************************************************************************/

#ifndef _SYS_NPI_SCTP_H
#define _SYS_NPI_SCTP_H

#ident "@(#) $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

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

typedef struct sctp_addr {
	uint16_t port __attribute__ ((packed));
	uint32_t addr[0] __attribute__ ((packed));
} sctp_addr_t;

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

#define N_QOS_SEL_CONN_SCTP	1
typedef struct {
	ulong n_qos_type;
	ulong i_streams;
	ulong o_streams;
} N_qos_sel_conn_sctp_t;

#define N_QOS_SEL_DATA_SCTP	2
typedef struct {
	ulong n_qos_type;
	ulong ppi;
	ulong sid;
	ulong ssn;
	ulong tsn;
	ulong more;
} N_qos_sel_data_sctp_t;

#define N_QOS_SEL_INFO_SCTP	3
typedef struct {
	ulong n_qos_type;
	ulong i_streams;
	ulong o_streams;
	ulong ppi;
	ulong sid;
	ulong max_inits;
	ulong max_retrans;
	ulong ck_life;
	ulong ck_inc;
	ulong hmac;
	ulong throttle;
	ulong max_sack;
	ulong rto_ini;
	ulong rto_min;
	ulong rto_max;
	ulong rtx_path;
	ulong hb_itvl;
	ulong options;
} N_qos_sel_info_sctp_t;

#define N_QOS_RANGE_INFO_SCTP	4
typedef struct {
	ulong n_qos_type;
} N_qos_range_info_sctp_t;

typedef union N_qos_sctp {
	ulong n_qos_type;
	N_qos_sel_conn_sctp_t n_qos_conn;
	N_qos_sel_data_sctp_t n_qos_data;
	N_qos_sel_info_sctp_t n_qos_info;
	N_qos_range_info_sctp_t n_qos_range;
} N_qos_sctp_t;

#ifndef SCTP_OPTION_DROPPING
#define SCTP_OPTION_DROPPING	0x01	/* stream will drop packets */
#define SCTP_OPTION_BREAK	0x02	/* stream will break dest #1 */
#define SCTP_OPTION_DBREAK	0x04	/* stream will break dest both ways */
#define SCTP_OPTION_RANDOM	0x08	/* stream will drop packets at random */
#endif

#ifndef SCTP_HMAC_NONE
#define SCTP_HMAC_NONE      0
#define SCTP_HMAC_SHA_1     1
#define SCTP_HMAC_MD5       2
#endif

#endif				/* _SYS_NPI_SCTP_H */
