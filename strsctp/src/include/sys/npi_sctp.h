/*****************************************************************************

 @(#) $Id: npi_sctp.h,v 0.9.2.7 2007/08/14 06:22:29 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

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

 Last Modified $Date: 2007/08/14 06:22:29 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: npi_sctp.h,v $
 Revision 0.9.2.7  2007/08/14 06:22:29  brian
 - GPLv3 header update

 Revision 0.9.2.6  2006/10/19 12:48:31  brian
 - corrections to ETSI SACK frequency

 Revision 0.9.2.5  2006/10/19 11:52:45  brian
 - added support for ETSI SACK frequency

 *****************************************************************************/

#ifndef _SYS_NPI_SCTP_H
#define _SYS_NPI_SCTP_H

#ident "@(#) $Name:  $($Revision: 0.9.2.7 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

/*
 *  LiS npi.h is version 1
 */
#ifndef N_VERSION_2

#define N_CURRENT_VERSION   0x02	/** Current version of NPI. */
#define N_VERSION_2	    0x02	/** Version of NPI: December 16, 1991. */

/*
 * Information acknowledgement
 */
typedef struct {
	np_ulong PRIM_type;		/** Always N_INFO_ACK. */
	np_ulong NSDU_size;		/** Maximum NSDU size. */
	np_ulong ENSDU_size;		/** Maximum ENSDU size. */
	np_ulong CDATA_size;		/** Connect data size. */
	np_ulong DDATA_size;		/** Discon data size. */
	np_ulong ADDR_size;		/** Address size. */
	np_ulong ADDR_length;		/** Address length. */
	np_ulong ADDR_offset;		/** Address offset. */
	np_ulong QOS_length;		/** QOS values length. */
	np_ulong QOS_offset;		/** QOS values offset. */
	np_ulong QOS_range_length;	/** Length of QOS values' range. */
	np_ulong QOS_range_offset;	/** Offset of QOS values' range. */
	np_ulong OPTIONS_flags;		/** Bit masking for options supported. */
	np_ulong NIDU_size;		/** Network i/f data unit size. */
	long SERV_type;			/** Service type. */
	np_ulong CURRENT_state;		/** Current state. */
	np_ulong PROVIDER_type;		/** Type of NS provider. */
	np_ulong NODU_size;		/** Optimal NSDU size. */
	np_ulong PROTOID_length;	/** Length of bound protocol identifiers. */
	np_ulong PROTOID_offset;	/** Offset of bound protocol identifiers. */
	np_ulong NPI_version;		/** Version # of npi that is supported. */
} __N_info_ack_t;

#define N_info_ack_t __N_info_ack_t

/*
 * Bind acknowledgement
 */
typedef struct {
	np_ulong PRIM_type;		/** Always N_BIND_ACK. */
	np_ulong ADDR_length;		/** Address length. */
	np_ulong ADDR_offset;		/** Offset of address. */
	np_ulong CONIND_number;		/** Connection indications. */
	np_ulong TOKEN_value;		/** Value of "token" assigned to stream. */
	np_ulong PROTOID_length;	/** Length of bound protocol identifiers. */
	np_ulong PROTOID_offset;	/** Offset of bound protocol identifiers. */
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
	np_ulong n_qos_type;		/** Always N_QOS_SEL_CONN_SCTP. */
	np_ulong i_streams;		/** Maximum number of input streams. */
	np_ulong o_streams;		/** Requested number of output streams. */
} N_qos_sel_conn_sctp_t;

#define N_QOS_SEL_DATA_SCTP	2
typedef struct {
	np_ulong n_qos_type;		/** Always N_QOS_SEL_DATA_SCTP. */
	np_ulong ppi;			/** Protocol Payload Identifier. */
	np_ulong sid;			/** Stream identifier. */
	np_ulong ssn;			/** Stream sequence number. */
	np_ulong tsn;			/** Trasnsmission sequence number. */
	np_ulong more;			/** More data in NSDU. */
} N_qos_sel_data_sctp_t;

#define N_QOS_SEL_INFO_SCTP	3
typedef struct {
	np_ulong n_qos_type;		/** Always N_QOS_SEL_INFO_SCTP. */
	np_ulong i_streams;		/** Maximum number of input streams. */
	np_ulong o_streams;		/** Requested number of output streams. */
	np_ulong ppi;			/** Protocol Payload Identifier. */
	np_ulong sid;			/** Stream identifier. */
	np_ulong max_inits;		/** Maximum number of INIT retries. */
	np_ulong max_retrans;		/** Maximum number of retransmissions per association. */
	np_ulong ck_life;		/** Cookie lifetime. */
	np_ulong ck_inc;		/** Cookie preservative time increment. */
	np_ulong hmac;			/** Message Authentication Code algorithm. */
	np_ulong throttle;		/** INIT/COOKIE-ECHO throttle interval. */
	np_ulong max_sack;		/** Maximum SACK delay. */
	np_ulong rto_ini;		/** Retransmission timeout value initial. */
	np_ulong rto_min;		/** Retransmission timeout value minimum. */
	np_ulong rto_max;		/** Retransmission timeout value maximum. */
	np_ulong rtx_path;		/** Maximum number of retransmission per path (destination). */
	np_ulong hb_itvl;		/** Heartbeat interval. */
	np_ulong options;		/** Options flags. */
} N_qos_sel_info_sctp_t;

/* Additional selection type added to support ETSI SACK Frequency settings.  Note that this
 * structure must be compatibile with the original version above except in the last field. */
#define N_QOS_SEL_INFO_SCTP2	5
typedef struct {
	np_ulong n_qos_type;		/** Always N_QOS_SEL_INFO_SCTP2. */
	np_ulong i_streams;		/** Maximum number of input streams. */
	np_ulong o_streams;		/** Requested number of output streams. */
	np_ulong ppi;			/** Protocol Payload Identifier. */
	np_ulong sid;			/** Stream identifier. */
	np_ulong max_inits;		/** Maximum number of INIT retries. */
	np_ulong max_retrans;		/** Maximum number of retransmissions per association. */
	np_ulong ck_life;		/** Cookie lifetime. */
	np_ulong ck_inc;		/** Cookie preservative time increment. */
	np_ulong hmac;			/** Message Authentication Code algorithm. */
	np_ulong throttle;		/** INIT/COOKIE-ECHO throttle interval. */
	np_ulong max_sack;		/** Maximum SACK delay. */
	np_ulong rto_ini;		/** Retransmission timeout value initial. */
	np_ulong rto_min;		/** Retransmission timeout value minimum. */
	np_ulong rto_max;		/** Retransmission timeout value maximum. */
	np_ulong rtx_path;		/** Maximum number of retransmission per path (destination). */
	np_ulong hb_itvl;		/** Heartbeat interval. */
	np_ulong options;		/** Options flags. */
	np_ulong sack_freq;		/** Sack frequency. */
} N_qos_sel_info_sctp2_t;

#define N_QOS_RANGE_INFO_SCTP	4
typedef struct {
	np_ulong n_qos_type;		/** Always N_QOS_RANGE_INFO_SCTP. */
} N_qos_range_info_sctp_t;

typedef union N_qos_sctp {
	np_ulong n_qos_type;
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
