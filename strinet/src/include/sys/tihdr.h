/*****************************************************************************

 @(#) $RCSfile: tihdr.h,v $ $Name:  $($Revision: 0.9 $) $Date: 2004/04/03 12:44:17 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free
 Software Foundation; either version 2.1 of the License, or (at your option)
 any later version.

 This library is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License
 along with this library; if not, write to the Free Software Foundation, Inc.,
 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/04/03 12:44:17 $ by $Author: brian $

 $Log: tihdr.h,v $
 Revision 0.9  2004/04/03 12:44:17  brian
 - Initial cut of new strinet package.

 *****************************************************************************/

#ifndef _SYS_TIHDR_H
#define _SYS_TIHDR_H

#ident "@(#) $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2004 OpenSS7 Corporation."

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

#define T_CURRENT_VERSION 5

#ifndef t_scalar_t
typedef int32_t t_scalar_t;
#define t_scalar_t t_scalar_t
typedef u_int32_t t_uscalar_t;
#define t_uscalar_t t_uscalar_t
#endif

/*
 *  Transport service error numbers
 *  (error codes used by TLI transport providers)
 */
#define TBADADDR	 1	/* Bad address format */
#define TBADOPT		 2	/* Bad options format */
#define TACCES		 3	/* Bad permissions */
#define TBADF		 4	/* Bad file descriptor */
#define TNOADDR		 5	/* Unable to allocate an address */
#define TOUTSTATE	 6	/* Would place interface out of state */
#define TBADSEQ		 7	/* Bad call sequence number */
#define TSYSERR		 8	/* System error */
#define TLOOK		 9	/* Has to t_look() for event			*/	/* not TLI */
#define TBADDATA	10	/* Bad amount of data */
#define TBUFOVFLW	11	/* Buffer was too small				*/	/* not TLI */
#define TFLOW		12	/* Would block due to flow control		*/	/* not TLI */
#define TNODATA		13	/* No data indication				*/	/* not TLI */
#define TNODIS		14	/* No disconnect indication			*/	/* not TLI */
#define TNOUDERR	15	/* No unitdata error indication			*/	/* not TLI */
#define TBADFLAG	16	/* Bad flags */
#define TNOREL		17	/* No orderly release indication		*/	/* not TLI */
#define TNOTSUPPORT	18	/* Not supported */
#define TSTATECHNG	19	/* State is currently changing			*/	/* not TLI */
#define TNOSTRUCTYPE	20	/* Structure type not supported			*/	/* not TLI */
#define TBADNAME	21	/* Bad transport provider name			*/	/* not TLI */
#define TBADQLEN	22	/* Listener queue length limit is zero		*/	/* not TLI */
#define TADDRBUSY	23	/* Address already in use			*/	/* not TLI */
#define TINDOUT		24	/* Outstanding connect indications		*/	/* not TLI */
#define TPROVMISMATCH	25	/* Not same transport provider			*/	/* not TLI */
#define TRESQLEN	26	/* Connection acceptor has qlen > 0		*/	/* not TLI */
#define TRESADDR	27	/* Conn. acceptor bound to different address	*/	/* not TLI */
#define TQFULL		28	/* Connection indicator queue is full		*/	/* not TLI */
#define TPROTO		29	/* Protocol error				*/	/* not TLI */

/*
 *  Transport service types
 */
#define T_COTS		1	/* Connection oriented transport service */
#define T_COTS_ORD	2	/* COTS with orderly release */
#define T_CLTS		3	/* Connectionless transport service */

/*
 *  Transport provider flags
 */
#define T_SNDZERO	1	/* Must match <sys/strops.h>: SNDZERO */
#define XPG4_1		2	/* This indicates that the transport provider conforms to XTI in
				   XPG4 and supports the new primitives T_ADDR_REQ and T_ADDR_ACK */

/*
 *  User level states (maintained internally by libnsl_s).
 */
#define T_UNINIT	0
#define T_UNBND		1
#define T_IDLE		2
#define T_OUTCON	3
#define T_INCON		4
#define T_DATAXFER	5
#define T_OUTREL	6
#define T_INREL		7
#define T_FAKE		8
#define T_HACK		12

/*
 *  Kernel level states of a transport end point.
 */
#define TS_UNBND	0	/* unbound */
#define TS_WACK_BREQ	1	/* waiting for T_BIND_REQ ack */
#define TS_WACK_UREQ	2	/* waiting for T_UNBIND_REQ ack */
#define TS_IDLE		3	/* idle */
#define TS_WACK_OPTREQ	4	/* waiting for T_OPTMGMT_REQ ack */
#define TS_WACK_CREQ	5	/* waiting for T_CONN_REQ ack */
#define TS_WCON_CREQ	6	/* waiting for T_CONN_REQ confirmation */
#define TS_WRES_CIND	7	/* waiting for T_CONN_IND */
#define TS_WACK_CRES	8	/* waiting for T_CONN_RES ack */
#define TS_DATA_XFER	9	/* data transfer */
#define TS_WIND_ORDREL	10	/* releasing read but not write */
#define TS_WREQ_ORDREL	11	/* wait to release write but not read */
#define TS_WACK_DREQ6	12	/* waiting for T_DISCON_REQ ack */
#define TS_WACK_DREQ7	13	/* waiting for T_DISCON_REQ ack */
#define TS_WACK_DREQ9	14	/* waiting for T_DISCON_REQ ack */
#define TS_WACK_DREQ10	15	/* waiting for T_DISCON_REQ ack */
#define TS_WACK_DREQ11	16	/* waiting for T_DISCON_REQ ack */
#define TS_NOSTATES	17

/*
 *  Messages used by "timod".
 */
#define T_CONN_REQ	0
#define T_CONN_RES	1
#define T_DISCON_REQ	2
#define T_DATA_REQ	3
#define T_EXDATA_REQ	4
#define T_INFO_REQ	5
#define T_BIND_REQ	6
#define T_UNBIND_REQ	7
#define T_UNITDATA_REQ	8
#define T_OPTMGMT_REQ	9
#define T_ORDREL_REQ	10
#define T_OPTDATA_REQ	24	/* data with options request */
#define T_ADDR_REQ	25	/* address request */
#define T_CAPABILITY_REQ 28

#define T_CONN_IND	11
#define T_CONN_CON	12
#define T_DISCON_IND	13
#define T_DATA_IND	14
#define T_EXDATA_IND	15
#define T_INFO_ACK	16
#define T_BIND_ACK	17
#define T_ERROR_ACK	18
#define T_OK_ACK	19
#define T_UNITDATA_IND	20
#define T_UDERROR_IND	21
#define T_OPTMGMT_ACK	22
#define T_ORDREL_IND	23
#define T_OPTDATA_IND	26	/* data with options indication */
#define T_ADDR_ACK	27	/* address acknowledgement */
#define T_CAPABILITY_ACK 29

#ifndef T_MORE
/*
 *  Flags used from user level library routines.
 */
#define T_MORE		0x0001
#define T_EXPEDITED	0x0002
#define T_NEGOTIATE	0x0004	/* Negotiate options */
#define T_CHECK		0x0008	/* Check options */
#define T_DEFAULT	0x0010	/* Get default options */
#define T_SUCCESS	0x0020	/* Success */
#define T_FAILURE	0x0040	/* Failure */
#define T_CURRENT	0x0080	/* Get current options */
#define T_PARTSUCCESS	0x0100	/* Partial success */
#define T_READONLY	0x0200	/* Option is read only */
#define T_NOTSUPPORT	0x0400	/* Option is not supported */
#endif

#define T_ALLOPT	0	/* All options at a level */

#define T_ODF_MORE	T_MORE
#define T_ODF_EX	T_EXPEDITED

/*
 *  T_INFO_REQ, M_PCPROTO
 *
 *  This primitive requests the transport provider to return the sizes of all
 *  relevant protocol parameters, plus the current state of the provider[2].
 */
struct T_info_req {
	t_scalar_t PRIM_type;		/* always T_INFO_REQ */
};

/*
 *  T_BIND_REQ, M_PROTO
 *
 *  This primitive request that the transport provider bind a protocol address
 *  to the stream, negotiate the number of connection indications allowed to
 *  be outstanding by the transport provider for the specified protocol
 *  address, and activate[3] the stream associated with the protocol address.
 */
struct T_bind_req {
	t_scalar_t PRIM_type;		/* always T_BIND_REQ */
	t_scalar_t ADDR_length;		/* length of address */
	t_scalar_t ADDR_offset;		/* offset of address */
	t_uscalar_t CONIND_number;
	/*
	   requested number of connect indications to be queued 
	 */
};

/*
 *  T_UNBIND_REQ, M_PROTO
 *
 *  This primitive requests that the transport provider unbind the protocol
 *  address associated with the stream and deactivate the stream.  The format
 */
struct T_unbind_req {
	t_scalar_t PRIM_type;		/* always T_UNBIND_REQ */
};

/*
 *  T_OPTMGMT_REQ, M_PROTO
 *
 *  This primitive allows the transport user to manage the options associated
 *  with the stream.
 *
 *      MGMT_flags:
 *          T_NEGOTIATE - negotiate and set the options with the transport provider
 *          T_CHECK     - check the validity of the specified options
 *          T_CURRENT   - return the options currently in effect
 *          T_DEFAULT   - return the default options
 */
struct T_optmgmt_req {
	t_scalar_t PRIM_type;		/* always T_OPTMGMT_REQ */
	t_scalar_t OPT_length;		/* options length */
	t_scalar_t OPT_offset;		/* options offset */
	t_scalar_t MGMT_flags;		/* flags */
};

/*
 *  T_ADDR_REQ, M_PROTO
 *
 *  This primitive requests that the transport provider return the local
 *  protocol address that is bound to the stream and the address of the remote
 *  transport entity if a connection has been established.
 */
struct T_addr_req {
	t_scalar_t PRIM_type;		/* always T_ADDR_REQ */
};

/*
   information acknowledgment 
 */

/*
 *  T_INFO_ACK, M_PCPROTO
 *
 *  This primitive indicates to the transport user any relevant protocol-
 *  dependent parameters.  It should be initiated in response to the
 *  T_INFO_REQ primitives described above.
 */
struct T_info_ack {
	t_scalar_t PRIM_type;		/* always T_INFO_ACK */
	t_scalar_t TSDU_size;		/* max TSDU size */
	t_scalar_t ETSDU_size;		/* max ETSDU size */
	t_scalar_t CDATA_size;		/* Connect data size */
	t_scalar_t DDATA_size;		/* Discon data size */
	t_scalar_t ADDR_size;		/* TSAP size */
	t_scalar_t OPT_size;		/* options size */
	t_scalar_t TIDU_size;		/* TIDU size */
	t_scalar_t SERV_type;		/* service type */
	t_scalar_t CURRENT_state;	/* current state */
	t_scalar_t PROVIDER_flag;	/* provider flags */
};

/*
 *  T_BIND_ACK, M_PCPROTO
 *
 *  This primitive indicates to the transport user that the sepcified protocol
 *  address has been bound to the stream, that the specified number of connect
 *  indications are allowed to be queued by the transport provider for the
 *  specified protocol address, and that the stream associated with the
 *  specified protocol address has been activated.
 */
struct T_bind_ack {
	t_scalar_t PRIM_type;		/* always T_BIND_ACK */
	t_scalar_t ADDR_length;		/* length of address - see note in sec. 1.4 */
	t_scalar_t ADDR_offset;		/* offset of address */
	t_uscalar_t CONIND_number;	/* connect indications to be queued */
};

/*
 *  T_OPTMGMT_ACK, M_PCPROTO
 *
 *  This indicates to the transport user that the options management request
 *  has completed.
 */
struct T_optmgmt_ack {
	t_scalar_t PRIM_type;		/* always T_OPTMGMT_ACK */
	t_scalar_t OPT_length;		/* options length - see note in sec. 1.4 */
	t_scalar_t OPT_offset;		/* options offset */
	t_scalar_t MGMT_flags;		/* flags */
};

/*
 *  T_ERROR_ACK, M_PRPROTO
 *
 *  This primitive indicates to the transport user that a non-fatal[9] error
 *  has occured in the last transport-user-originated primitive.  This may
 *  only be initiated as an acknowledgement for those primitives that require
 *  one.  It also indicates to the user that no action was taken on the
 *  primitive that cause the error.
 */
struct T_error_ack {
	t_scalar_t PRIM_type;		/* always T_ERROR_ACK */
	t_scalar_t ERROR_prim;		/* primitive in error */
	t_scalar_t TLI_error;		/* TLI error code - see not in sec. 1.4 */
	t_scalar_t UNIX_error;		/* UNIX error code - see not in sec. 1.4 */
};

/*
 *  T_OK_ACK, M_PCPROTO
 *
 *  This primtiive indicates to the transport user that the previous
 *  transport-user-originated primitive was received successfully by the
 *  transport provider.  It does not indicate to the transport user any
 *  transport protocol action taken due to the issuance of the last primitive.
 *  This may only be initiated as an acknowledgement for those primitives that
 *  require one.
 */
struct T_ok_ack {
	t_scalar_t PRIM_type;		/* always T_OK_ACK */
	t_scalar_t CORRECT_prim;	/* primitive */
};

/*
 *  T_ADDR_ACK, M_PCPROTO
 *
 *  This primitive indicates to the transport user the addresses fo the local
 *  and remote transport entities.  The local address is the protocol address
 *  that has been bound to the stream.  If a connection has been established,
 *  the remote address is the protocol address of the remote transport entity.
 */
struct T_addr_ack {
	t_scalar_t PRIM_type;		/* always T_ADDR_ACK */
	t_scalar_t LOCADDR_length;	/* length of local address - see not in sec. 1.4 */
	t_scalar_t LOCADDR_offset;	/* offset of local address */
	t_scalar_t REMADDR_length;	/* length of remote address - see not in sec. 1.4 */
	t_scalar_t REMADDR_offset;	/* offset of remote address */
};

/*
 *  Connection Oriented Transport Primitives
 */

/*
 *  T_CONN_REQ, M_PROTO
 *
 *  This primitive requests that the transport provider make a connection to
 *  the specified destination.
 */
struct T_conn_req {
	t_scalar_t PRIM_type;		/* always T_CONN_REQ */
	t_scalar_t DEST_length;		/* dest addr length */
	t_scalar_t DEST_offset;		/* dest addr offset */
	t_scalar_t OPT_length;		/* options length */
	t_scalar_t OPT_offset;		/* options offset */
};

/*
 *  T_CONN_RES, M_PROTO (followed by 0 or more M_DATA)
 *
 *  This primitive requests that the transport provider accept a previous
 *  connect request on the specified response queue.
 */
struct T_conn_res {
	t_scalar_t PRIM_type;		/* always T_CONN_RES */
	t_scalar_t ACCEPTOR_id;		/* reponse queue ptr */
	t_scalar_t OPT_length;		/* options length */
	t_scalar_t OPT_offset;		/* options offset */
	t_scalar_t SEQ_number;		/* sequence number */
};

/*
 *  T_DSICON_REQ, M_PROTO (followed by 0 or more M_DATA)
 *
 *  This primitive request that the transport provider deny a request for
 *  connection, or disconnect an existing connection.
 */
struct T_discon_req {
	t_scalar_t PRIM_type;		/* always T_DISCON_REQ */
	t_scalar_t SEQ_number;		/* sequence number */
};

/*
 *  T_DATA_REQ, (opt M_PROTO) (followed by 0 or more M_DATA)
 *
 *  This primitive indicates to the transport provider that this message
 *  contains a transport interface data unit.  One or more transport interface
 *  data units form a transport service data unit (TSDU)[11].  This primitive
 *  has a mechanism which indicates the beginning and end of a transport
 *  service data unit.  However, not all transport providers support the
 *  concept of a transport service data unit, as noted in section 2.1.2.1.
 */
struct T_data_req {
	t_scalar_t PRIM_type;		/* always T_DATA_REQ */
	t_scalar_t MORE_flag;		/* indicates more data in TSDU */
};

/*
 *  T_EXDATA_REQ, M_PROTO (followed by 1 or more M_DATA)
 *
 *  This primitive indicates to the transport provider that this message
 *  contains an expedited transport interface data unit.  One or more
 *  expedited transport interface data units form an expedited transport
 *  service data unit[12].  This primitive has a mechanism which indicates the
 *  beginning and end of an expedited tansport service data unit.  However,
 *  not all transport providers support the concept of an expedited transport
 *  service data unit, as noted in section 2.1.2.1.
 */
struct T_exdata_req {
	t_scalar_t PRIM_type;		/* T_EXDATA_REQ */
	t_scalar_t MORE_flag;
};

/*
 *  T_ORDREL_REQ, M_PROTO
 *
 *  This primitive indicates to the transport provider that the user is
 *  finished sending data.  This primitive is only supported by the transport
 *  provider if it is of type T_COTS_ORD.
 */
struct T_ordrel_req {
	t_scalar_t PRIM_type;		/* always T_ORDREL_REQ */
};

/*
 *  T_CONN_IND, M_PROTO
 *
 *  This primnitive indicates to the transport user that a connect request to
 *  the user has been made by the user at the specified source address.  The
 */
struct T_conn_ind {
	t_scalar_t PRIM_type;		/* always T_CONN_IND */
	t_scalar_t SRC_length;		/* source addr length = see note in sec. 1.4 */
	t_scalar_t SRC_offset;		/* source addr offset */
	t_scalar_t OPT_length;		/* options length = see note in sec. 1.4 */
	t_scalar_t OPT_offset;		/* options offset */
	t_scalar_t SEQ_number;		/* sequence number - see not in sec. 1.4 */
};

/*
 *  T_CONN_CON, M_PROTO, (followed by 0 or more M_DATA)
 *
 *  This primitive indicates to the user that a connect request has been
 *  confirmed on the specified responding address.
 */
struct T_conn_con {
	t_scalar_t PRIM_type;		/* T_CONN_CON */
	t_scalar_t RES_length;		/* responding addr length - see note in sec. 1.4 */
	t_scalar_t RES_offset;		/* responding addr offset */
	t_scalar_t OPT_length;		/* options length - see note in sec. 1.4 */
	t_scalar_t OPT_offset;		/* options offset */
};

/*
 *  T_DISCON_IND, M_PROTO (followed by 0 or more M_DATA)
 *
 *  This primitive indicates to the user that either a request for connection
 *  has been denied or and existing connection has been disconnected.
 */
struct T_discon_ind {
	t_scalar_t PRIM_type;		/* T_DISCON_IND */
	t_scalar_t DISCON_reason;	/* disconnect reason - see note in sec. 1.4 */
	t_scalar_t SEQ_number;		/* sequence number - see note in sec. 1.4 */
};

/*
 *  T_DATA_IND, (opt M_PROTO) (followed by 0 or more M_DATA)
 *
 *  This primitive indicates to the transport user that this message contains
 *  a transport interface data unit.  One or more transport interface data
 *  units form a transport servie data unit.  This primitive has a mechanism
 *  which indicates the beginning and end of a transport service data unit.
 *  However, not all transport provider support the concept of a transport
 *  service data unit, as noted in section 2.1.2.1.
 */
struct T_data_ind {
	t_scalar_t PRIM_type;		/* always T_DATA_IND */
	t_scalar_t MORE_flag;		/* indicates more data in TSDU */
};

/*
 *  T_EXDATA_IND, M_PROTO
 *
 *  This primitive indicates to the transport user that this message contains
 *  an expedited transport interface data unit.  One or more expedited
 *  transport interface data units form an expedited transport service data
 *  unit.  This primitive has a mechanism which indicates the beginning and
 *  end of an expedited transport service data unit.  However, not all
 *  transport providers support the concept of an expedited transport service
 *  data unit, as noted in section 2.1.2.1.
 */
struct T_exdata_ind {
	t_scalar_t PRIM_type;		/* always T_EXDATA_IND */
	t_scalar_t MORE_flag;		/* indicates more data in ETSDU */
};

/*
 *  T_ORDREL_IND, M_PROTO
 *
 *  This primitive indicates to the transport user that the other side of the
 *  connection is finished sending data.  This primitive is only supported by
 *  the transport provider if it is of type T_COTS_ORD.
 */
struct T_ordrel_ind {
	t_scalar_t PRIM_type;		/* always T_ORDREL_IND */
};

/*
 *  Connectionless-Mode Transport Primitives
 */

/*
 *  T_UNITDATA_REQ, M_PROTO
 *
 *  This primitive request that the transport provider send the specified
 *  datagram to the specified destination.
 */
struct T_unitdata_req {
	t_scalar_t PRIM_type;		/* always T_UNITDATA_REQ */
	t_scalar_t DEST_length;		/* dest addr length */
	t_scalar_t DEST_offset;		/* dest addr offset */
	t_scalar_t OPT_length;		/* options length */
	t_scalar_t OPT_offset;		/* options offset */
};

/*
 *  T_UNITDATA_IND, M_PROTO
 *
 *  This primitive indicates to the transport user that a datagram has been
 *  received from the specified source address.
 */
struct T_unitdata_ind {
	t_scalar_t PRIM_type;		/* T_UNITDATA_IND */
	t_scalar_t SRC_length;		/* source addr length - see note in sec. 1.4 */
	t_scalar_t SRC_offset;		/* source addr offset */
	t_scalar_t OPT_length;		/* options length - see note in sec. 1.4 */
	t_scalar_t OPT_offset;		/* options offset */
};

/*
 *  T_UDERROR_IND, M_PROTO
 *
 *  This primitive indicates to the transport user that a datagram with the
 *  specified destination address and options produced an error.
 */
struct T_uderror_ind {
	t_scalar_t PRIM_type;		/* T_UDERROR_IND */
	t_scalar_t DEST_length;		/* dest addr length - see note in sec. 1.4 */
	t_scalar_t DEST_offset;		/* dest addr offset */
	t_scalar_t OPT_length;		/* options length - see note in sec. 1.4 */
	t_scalar_t OPT_offset;		/* options offset */
	t_scalar_t ERROR_type;		/* error type */
};

struct T_optdata_req {
	t_scalar_t PRIM_type;		/* T_OPTDATA_REQ */
	t_scalar_t DATA_flag;		/* flag bits associated with data */
	t_scalar_t OPT_length;		/* options length */
	t_scalar_t OPT_offset;		/* options offset */
};

struct T_optdata_ind {
	t_scalar_t PRIM_type;		/* T_OPTDATA_REQ */
	t_scalar_t DATA_flag;		/* flag bits associated with data */
	t_scalar_t OPT_length;		/* options length */
	t_scalar_t OPT_offset;		/* options offset */
};

struct T_capability_req {
	t_scalar_t PRIM_type;		/* T_CAPABILITY_REQ */
	t_uscalar_t CAP_bits1;		/* capability bits 1 */
};

struct T_capability_ack {
	t_scalar_t PRIM_type;		/* T_CAPABILITY_ACK */
	t_uscalar_t CAP_bits1;		/* capability bits #1 */
	struct T_info_ack INFO_ack;	/* info acknowledgement */
	t_uscalar_t ACCEPTOR_id;	/* accepting endpoint id */
};

#define TC1_INFO	(1<<0)
#define TC1_ACCEPTOR_ID	(1<<1)
#define TCI_CAP_BITS2	(1<<31)

union T_primitives {
	t_scalar_t type;
	struct T_addr_ack addr_ack;
	struct T_addr_req addr_req;
	struct T_bind_ack bind_ack;
	struct T_bind_req bind_req;
	struct T_conn_con conn_con;
	struct T_conn_ind conn_ind;
	struct T_conn_req conn_req;
	struct T_conn_res conn_res;
	struct T_data_ind data_ind;
	struct T_data_req data_req;
	struct T_discon_ind discon_ind;
	struct T_discon_req discon_req;
	struct T_error_ack error_ack;
	struct T_exdata_ind exdata_ind;
	struct T_exdata_req exdata_req;
	struct T_info_ack info_ack;
	struct T_info_req info_req;
	struct T_ok_ack ok_ack;
	struct T_optmgmt_ack optmgmt_ack;
	struct T_optmgmt_req optmgmt_req;
	struct T_ordrel_ind ordrel_ind;
	struct T_ordrel_req ordrel_req;
	struct T_uderror_ind uderror_ind;
	struct T_unbind_req unbind_req;
	struct T_unitdata_ind unitdata_ind;
	struct T_unitdata_req unitdata_req;
	struct T_optdata_req optdata_req;
	struct T_optdata_ind optdata_ind;
	struct T_capability_req capability_req;
	struct T_capability_ack capability_ack;
};

#ifndef T_OPT_FIRSTHDR
/*
   The t_opthdr structure defines the layout of options in a T_OPTMGMT_* data buffer. This is
   specified in the X/Open specs but does not appear to exist in SCO 3.2.x, SCO OS5, Interactive
   SVR4 or UnixWare 1.x. There are programs that make options request however. The older TLI uses
   struct opthdr which is different and incompatible (see below). 
 */
struct t_opthdr {
	t_uscalar_t len;		/* Option length, incl. header */
	t_uscalar_t level;		/* Option level */
	t_uscalar_t name;		/* Option name */
	t_uscalar_t status;		/* Negotiation result */
	char value[0];			/* and onwards...  */
};

struct opthdr {
	t_scalar_t level;		/* Option level */
	t_scalar_t name;		/* Option name */
	t_scalar_t len;			/* Length of option value */
	char value[0];			/* and onwards...  */
};

#define _T_ALIGN_SIZE sizeof(t_uscalar_t)

#define _T_ALIGN_OFS(p, o)						\
	((char *)(((t_uscalar_t)(p) - (o) + _T_ALIGN_SIZE - 1)	\
		    & ~(_T_ALIGN_SIZE - 1)) + (o))

#define _T_ALIGN_OFFSET(p)						\
	((t_uscalar_t)(p) & (_T_ALIGN_SIZE - 1))

#define _T_ALIGN_OFS_OFS(p, l, o) _T_ALIGN_OFS((char *)(p) + l, (o))

#define _T_OPT_FIRSTHDR_OFS(b, l, o)					\
	((struct t_opthdr *)(						\
	  (_T_ALIGN_OFS((b), (o)) + sizeof(struct t_opthdr)		\
	   <= (char *)(b) + (l)) ?					\
	   _T_ALIGN_OFS((b), (o)) : NULL))

#define _T_OPT_NEXTHDR_OFS(b, l, p, o)					\
	((struct t_opthdr *)(						\
	  (_T_ALIGN_OFS_OFS((p), (p)->len + sizeof(struct t_opthdr), (o)) \
	   <= ((char *)(b) + (l))) ?					\
	  _T_ALIGN_OFS_OFS((p), (p)->len, (o)) : NULL			\
	))

#define _T_OPT_DATA_OFS(p, o)						\
	((unsigned char *)(_T_ALIGN_OFS((struct t_opthdr *)(p)+1, (o))))

#define T_OPT_FIRSTHDR(b)	_T_OPT_FIRSTHDR_OFS((b)->buf, (b)->len, 0)
#define T_OPT_NEXTHDR(b, p)	_T_OPT_NEXTHDR_OFS((b)->buf, (b)->len, p, 0)
#define T_OPT_DATA(p)		_T_OPT_DATA_OFS((p), 0)
#endif

/*
 *  General purpose constants
 */
#ifndef T_YES
#define T_YES		1	/* Yes/on/true */
#define T_NO		0	/* No/off/false */
#endif
#ifndef T_INFINITE
#define T_INFINITE	-1	/* No limit */
#define T_INVALID	-2	/* No sense */
#endif
#ifndef T_SNDZERO
#define T_SNDZERO	1	/* Must match <sys/stropts.h>: SNDZERO */
#endif

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* _SYS_TIHDR_H */
