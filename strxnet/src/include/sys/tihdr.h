/*****************************************************************************

 @(#) $Id: tihdr.h,v 0.9.2.9 2006/09/25 12:04:43 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2006/09/25 12:04:43 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tihdr.h,v $
 Revision 0.9.2.9  2006/09/25 12:04:43  brian
 - updated headers, moved xnsl

 Revision 0.9.2.8  2006/09/24 21:57:22  brian
 - documentation and library updates

 Revision 0.9.2.7  2006/09/22 20:59:27  brian
 - prepared header file for use with doxygen, touching many lines

 Revision 0.9.2.6  2006/09/18 13:52:45  brian
 - added doxygen markers to sources

 Revision 0.9.2.5  2005/07/18 12:45:03  brian
 - standard indentation

 Revision 0.9.2.4  2005/05/14 08:28:29  brian
 - copyright header correction

 Revision 0.9.2.3  2004/05/16 04:12:33  brian
 - Updating strxnet release.

 Revision 1.4  2004/05/14 08:00:02  brian
 - Updated xns, tli, inet, xnet and documentation.

 *****************************************************************************/

#ifndef _SYS_TIHDR_H
#define _SYS_TIHDR_H

#ident "@(#) $RCSfile: tihdr.h,v $ $Name:  $($Revision: 0.9.2.9 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup tpi
  * @{ */

/** @file
  * Transport Interface header file.  */

/*
 * Transport Interface Header File.
 */

#define T_CURRENT_VERSION 5

#ifndef t_scalar_t
typedef int32_t t_scalar_t;

#define t_scalar_t t_scalar_t
#endif

#ifndef t_uscalar_t
typedef u_int32_t t_uscalar_t;

#define t_uscalar_t t_uscalar_t
#endif

#include <sys/tpi.h>		/**< Common TLI, XTI, TI definitions */

/** 
  * @name TPI States
  * Kernel level states of a transport end point.
  * @{ */
#define TS_UNBND	0	/**< Unbound. */
#define TS_WACK_BREQ	1	/**< Waiting for T_BIND_REQ ack. */
#define TS_WACK_UREQ	2	/**< Waiting for T_UNBIND_REQ ack. */
#define TS_IDLE		3	/**< Idle. */
#define TS_WACK_OPTREQ	4	/**< Waiting for T_OPTMGMT_REQ ack. */
#define TS_WACK_CREQ	5	/**< Waiting for T_CONN_REQ ack. */
#define TS_WCON_CREQ	6	/**< Waiting for T_CONN_REQ confirmation. */
#define TS_WRES_CIND	7	/**< Waiting for T_CONN_IND. */
#define TS_WACK_CRES	8	/**< Waiting for T_CONN_RES ack. */
#define TS_DATA_XFER	9	/**< Data transfer. */
#define TS_WIND_ORDREL	10	/**< Releasing read but not write. */
#define TS_WREQ_ORDREL	11	/**< Wait to release write but not read. */
#define TS_WACK_DREQ6	12	/**< Waiting for T_DISCON_REQ ack. */
#define TS_WACK_DREQ7	13	/**< Waiting for T_DISCON_REQ ack. */
#define TS_WACK_DREQ9	14	/**< Waiting for T_DISCON_REQ ack. */
#define TS_WACK_DREQ10	15	/**< Waiting for T_DISCON_REQ ack. */
#define TS_WACK_DREQ11	16	/**< Waiting for T_DISCON_REQ ack. */
#define TS_NOSTATES	17
/** @} */

/** 
  * @name TPI Primitive Types
  * Messages used by "timod".
  * @{ */
#define T_CONN_REQ	0	/**< TC request. */
#define T_CONN_RES	1	/**< TC response. */
#define T_DISCON_REQ	2	/**< Disconnect TC. */
#define T_DATA_REQ	3	/**< Connection-oriented data request. */
#define T_EXDATA_REQ	4	/**< Expedited data request. */
#define T_INFO_REQ	5	/**< Request TP information. */
#define T_BIND_REQ	6	/**< Bind to a transport address. */
#define T_UNBIND_REQ	7	/**< Unbind from a transport address. */
#define T_UNITDATA_REQ	8	/**< Connectionless unit data request. */
#define T_OPTMGMT_REQ	9	/**< Options management. */
#define T_ORDREL_REQ	10	/**< Request orderly release of TC. */
#define T_OPTDATA_REQ	24	/**< Data with options request. */
#define T_ADDR_REQ	25	/**< Address request. */
#define T_CAPABILITY_REQ 28	/**< Request TP capabilities. */

#define T_CONN_IND	11	/**< TC indication. */
#define T_CONN_CON	12	/**< TC confirmation. */
#define T_DISCON_IND	13	/**< TC disconnected. */
#define T_DATA_IND	14	/**< Receive connection-oriented data. */
#define T_EXDATA_IND	15	/**< Receive expedited data on TC. */
#define T_INFO_ACK	16	/**< TP information. */
#define T_BIND_ACK	17	/**< Bound to transport address. */
#define T_ERROR_ACK	18	/**< Last primitive encountered error. */
#define T_OK_ACK	19	/**< Last primitive successful. */
#define T_UNITDATA_IND	20	/**< Receive a unit of data. */
#define T_UDERROR_IND	21	/**< Error in sent unit data. */
#define T_OPTMGMT_ACK	22	/**< Options management result. */
#define T_ORDREL_IND	23	/**< Indication of orderly release of TC. */
#define T_OPTDATA_IND	26	/**< Data with options indication. */
#define T_ADDR_ACK	27	/**< Address acknowledgement. */
#define T_CAPABILITY_ACK 29
/** @} */

/** 
  * @name T_OPTDATA Flags
  * Flags for use in DATA_flag field of T_OPTDATA.
  * @{ */
#define T_ODF_MORE	T_MORE
#define T_ODF_EX	T_EXPEDITED
/** @} */

/** 
  * @name Local Management Primitives
  * @{
  */

/** 
  * T_INFO_REQ, M_PCPROTO.
  *
  * This primitive requests the transport provider to return the sizes of all
  * relevant protocol parameters, plus the current state of the provider[2].  */
struct T_info_req {
	t_scalar_t PRIM_type;		/**< Always T_INFO_REQ. */
};

/** 
  * T_BIND_REQ, M_PROTO.
  *
  * This primitive request that the transport provider bind a protocol address
  * to the stream, negotiate the number of connection indications allowed to
  * be outstanding by the transport provider for the specified protocol
  * address, and activate[3] the stream associated with the protocol address.
  */
struct T_bind_req {
	t_scalar_t PRIM_type;		/**< Always T_BIND_REQ. */
	t_scalar_t ADDR_length;		/**< Length of address. */
	t_scalar_t ADDR_offset;		/**< Offset of address. */
	t_uscalar_t CONIND_number;	/**< Requested number of connect
					     indications to be queued. */
};

/** 
  * T_UNBIND_REQ, M_PROTO.
  *
  * This primitive requests that the transport provider unbind the protocol
  * address associated with the stream and deactivate the stream.  The format
  */
struct T_unbind_req {
	t_scalar_t PRIM_type;		/**< Always T_UNBIND_REQ. */
};

/**
  * T_OPTMGMT_REQ, M_PROTO.
  *
  * This primitive allows the transport user to manage the options associated
  * with the stream.
  */
/*       MGMT_flags:
  *          T_NEGOTIATE - negotiate and set the options with the transport provider
  *          T_CHECK     - check the validity of the specified options
  *          T_CURRENT   - return the options currently in effect
  *          T_DEFAULT   - return the default options
  */
struct T_optmgmt_req {
	t_scalar_t PRIM_type;		/**< Always T_OPTMGMT_REQ. */
	t_scalar_t OPT_length;		/**< Options length. */
	t_scalar_t OPT_offset;		/**< Options offset. */
	t_scalar_t MGMT_flags;		/**< Flags. */
};

/** 
  * T_ADDR_REQ, M_PROTO.
  *
  * This primitive requests that the transport provider return the local
  * protocol address that is bound to the stream and the address of the remote
  * transport entity if a connection has been established.
  */
struct T_addr_req {
	t_scalar_t PRIM_type;		/**< Always T_ADDR_REQ. */
};

/** 
  * T_INFO_ACK, M_PCPROTO.
  *
  * This primitive indicates to the transport user any relevant protocol-
  * dependent parameters.  It should be initiated in response to the
  * T_INFO_REQ primitives described above.
  */
struct T_info_ack {
	t_scalar_t PRIM_type;		/**< Always T_INFO_ACK. */
	t_scalar_t TSDU_size;		/**< Max TSDU size. */
	t_scalar_t ETSDU_size;		/**< Max ETSDU size. */
	t_scalar_t CDATA_size;		/**< Connect data size. */
	t_scalar_t DDATA_size;		/**< Discon data size. */
	t_scalar_t ADDR_size;		/**< TSAP size. */
	t_scalar_t OPT_size;		/**< Options size. */
	t_scalar_t TIDU_size;		/**< TIDU size. */
	t_scalar_t SERV_type;		/**< Service type. */
	t_scalar_t CURRENT_state;	/**< Current state. */
	t_scalar_t PROVIDER_flag;	/**< Provider flags. */
};

/** 
  * T_BIND_ACK, M_PCPROTO.
  *
  * This primitive indicates to the transport user that the sepcified protocol
  * address has been bound to the stream, that the specified number of connect
  * indications are allowed to be queued by the transport provider for the
  * specified protocol address, and that the stream associated with the
  * specified protocol address has been activated.
  */
struct T_bind_ack {
	t_scalar_t PRIM_type;		/**< Always T_BIND_ACK. */
	t_scalar_t ADDR_length;		/**< Length of address - see note in sec. 1.4. */
	t_scalar_t ADDR_offset;		/**< Offset of address. */
	t_uscalar_t CONIND_number;	/**< Connect indications to be queued. */
};

/**
  * T_OPTMGMT_ACK, M_PCPROTO.
  *
  * This indicates to the transport user that the options management request
  * has completed.
  */
struct T_optmgmt_ack {
	t_scalar_t PRIM_type;		/**< Always T_OPTMGMT_ACK. */
	t_scalar_t OPT_length;		/**< Options length - see note in sec. 1.4. */
	t_scalar_t OPT_offset;		/**< Options offset. */
	t_scalar_t MGMT_flags;		/**< Flags. */
};

/** 
  * T_ERROR_ACK, M_PRPROTO
  *
  * This primitive indicates to the transport user that a non-fatal[9] error
  * has occured in the last transport-user-originated primitive.  This may
  * only be initiated as an acknowledgement for those primitives that require
  * one.  It also indicates to the user that no action was taken on the
  * primitive that cause the error.
  */
struct T_error_ack {
	t_scalar_t PRIM_type;		/**< Always T_ERROR_ACK. */
	t_scalar_t ERROR_prim;		/**< Primitive in error. */
	t_scalar_t TLI_error;		/**< TLI error code - see not in sec. 1.4. */
	t_scalar_t UNIX_error;		/**< UNIX error code - see not in sec. 1.4. */
};

/** 
  * T_OK_ACK, M_PCPROTO.
  *
  * This primitive indicates to the transport user that the previous
  * transport-user-originated primitive was received successfully by the
  * transport provider.  It does not indicate to the transport user any
  * transport protocol action taken due to the issuance of the last primitive.
  * This may only be initiated as an acknowledgement for those primitives that
  * require one.
  */
struct T_ok_ack {
	t_scalar_t PRIM_type;		/**< Always T_OK_ACK. */
	t_scalar_t CORRECT_prim;	/**< Primitive. */
};

/** 
  * T_ADDR_ACK, M_PCPROTO.
  *
  * This primitive indicates to the transport user the addresses fo the local
  * and remote transport entities.  The local address is the protocol address
  * that has been bound to the stream.  If a connection has been established,
  * the remote address is the protocol address of the remote transport entity.
  */
struct T_addr_ack {
	t_scalar_t PRIM_type;		/**< Always T_ADDR_ACK. */
	t_scalar_t LOCADDR_length;	/**< Length of local address - see not in sec. 1.4. */
	t_scalar_t LOCADDR_offset;	/**< Offset of local address. */
	t_scalar_t REMADDR_length;	/**< Length of remote address - see not in sec. 1.4. */
	t_scalar_t REMADDR_offset;	/**< Offset of remote address. */
};

/** @} */

/** 
  * @name Connection Oriented Transport Primitives
  * @{
  */

/** 
  * T_CONN_REQ, M_PROTO.
  *
  * This primitive requests that the transport provider make a connection to
  * the specified destination.
  */
struct T_conn_req {
	t_scalar_t PRIM_type;		/**< Always T_CONN_REQ. */
	t_scalar_t DEST_length;		/**< Dest addr length. */
	t_scalar_t DEST_offset;		/**< Dest addr offset. */
	t_scalar_t OPT_length;		/**< Options length. */
	t_scalar_t OPT_offset;		/**< Options offset. */
};

/** 
  * T_CONN_RES, M_PROTO (followed by 0 or more M_DATA).
  *
  * This primitive requests that the transport provider accept a previous
  * connect request on the specified response queue.
  */
struct T_conn_res {
	t_scalar_t PRIM_type;		/**< Always T_CONN_RES. */
	t_scalar_t ACCEPTOR_id;		/**< Reponse queue ptr. */
	t_scalar_t OPT_length;		/**< Options length. */
	t_scalar_t OPT_offset;		/**< Options offset. */
	t_scalar_t SEQ_number;		/**< Sequence number. */
};

/** 
  * T_DISCON_REQ, M_PROTO (followed by 0 or more M_DATA).
  *
  * This primitive request that the transport provider deny a request for
  * connection, or disconnect an existing connection.
  */
struct T_discon_req {
	t_scalar_t PRIM_type;		/**< Always T_DISCON_REQ. */
	t_scalar_t SEQ_number;		/**< Sequence number. */
};

/** 
  * T_DATA_REQ, (opt M_PROTO) (followed by 0 or more M_DATA).
  *
  * This primitive indicates to the transport provider that this message
  * contains a transport interface data unit.  One or more transport interface
  * data units form a transport service data unit (TSDU)[11].  This primitive
  * has a mechanism which indicates the beginning and end of a transport
  * service data unit.  However, not all transport providers support the
  * concept of a transport service data unit, as noted in section 2.1.2.1.
  */
struct T_data_req {
	t_scalar_t PRIM_type;		/**< Always T_DATA_REQ. */
	t_scalar_t MORE_flag;		/**< Indicates more data in TSDU. */
};

/** 
  * T_EXDATA_REQ, M_PROTO (followed by 1 or more M_DATA).
  *
  * This primitive indicates to the transport provider that this message
  * contains an expedited transport interface data unit.  One or more
  * expedited transport interface data units form an expedited transport
  * service data unit[12].  This primitive has a mechanism which indicates the
  * beginning and end of an expedited tansport service data unit.  However,
  * not all transport providers support the concept of an expedited transport
  * service data unit, as noted in section 2.1.2.1.
  */
struct T_exdata_req {
	t_scalar_t PRIM_type;		/**< Always T_EXDATA_REQ. */
	t_scalar_t MORE_flag;
};

/** 
  * T_ORDREL_REQ, M_PROTO.
  *
  * This primitive indicates to the transport provider that the user is
  * finished sending data.  This primitive is only supported by the transport
  * provider if it is of type T_COTS_ORD.
  */
struct T_ordrel_req {
	t_scalar_t PRIM_type;		/**< Always T_ORDREL_REQ. */
};

/** 
  * T_CONN_IND, M_PROTO.
  *
  * This primnitive indicates to the transport user that a connect request to
  * the user has been made by the user at the specified source address.  The
  */
struct T_conn_ind {
	t_scalar_t PRIM_type;		/**< Always T_CONN_IND. */
	t_scalar_t SRC_length;		/**< Source addr length = see note in sec. 1.4. */
	t_scalar_t SRC_offset;		/**< Source addr offset. */
	t_scalar_t OPT_length;		/**< Options length = see note in sec. 1.4. */
	t_scalar_t OPT_offset;		/**< Options offset. */
	t_scalar_t SEQ_number;		/**< Sequence number - see not in sec. 1.4. */
};

/** 
  * T_CONN_CON, M_PROTO, (followed by 0 or more M_DATA).
  *
  * This primitive indicates to the user that a connect request has been
  * confirmed on the specified responding address.
  */
struct T_conn_con {
	t_scalar_t PRIM_type;		/**< T_CONN_CON. */
	t_scalar_t RES_length;		/**< Responding addr length - see note in sec. 1.4. */
	t_scalar_t RES_offset;		/**< Responding addr offset. */
	t_scalar_t OPT_length;		/**< Options length - see note in sec. 1.4. */
	t_scalar_t OPT_offset;		/**< Options offset. */
};

/** 
  * T_DISCON_IND, M_PROTO (followed by 0 or more M_DATA).
  *
  * This primitive indicates to the user that either a request for connection
  * has been denied or and existing connection has been disconnected.
  */
struct T_discon_ind {
	t_scalar_t PRIM_type;		/**< T_DISCON_IND. */
	t_scalar_t DISCON_reason;	/**< Disconnect reason - see note in sec. 1.4. */
	t_scalar_t SEQ_number;		/**< Sequence number - see note in sec. 1.4. */
};

/** 
  * T_DATA_IND, (opt M_PROTO) (followed by 0 or more M_DATA).
  *
  * This primitive indicates to the transport user that this message contains
  * a transport interface data unit.  One or more transport interface data
  * units form a transport servie data unit.  This primitive has a mechanism
  * which indicates the beginning and end of a transport service data unit.
  * However, not all transport provider support the concept of a transport
  * service data unit, as noted in section 2.1.2.1.
  */
struct T_data_ind {
	t_scalar_t PRIM_type;		/**< Always T_DATA_IND. */
	t_scalar_t MORE_flag;		/**< Indicates more data in TSDU. */
};

/** 
  * T_EXDATA_IND, M_PROTO.
  *
  * This primitive indicates to the transport user that this message contains
  * an expedited transport interface data unit.  One or more expedited
  * transport interface data units form an expedited transport service data
  * unit.  This primitive has a mechanism which indicates the beginning and
  * end of an expedited transport service data unit.  However, not all
  * transport providers support the concept of an expedited transport service
  * data unit, as noted in section 2.1.2.1.
  */
struct T_exdata_ind {
	t_scalar_t PRIM_type;		/**< Always T_EXDATA_IND. */
	t_scalar_t MORE_flag;		/**< Indicates more data in ETSDU. */
};

/** 
  * T_ORDREL_IND, M_PROTO.
  *
  * This primitive indicates to the transport user that the other side of the
  * connection is finished sending data.  This primitive is only supported by
  * the transport provider if it is of type T_COTS_ORD.
  */
struct T_ordrel_ind {
	t_scalar_t PRIM_type;		/**< Always T_ORDREL_IND. */
};

/** @} */

/** 
  * @name Connectionless-Mode Transport Primitives
  * @{
  */

/** 
  * T_UNITDATA_REQ, M_PROTO.
  *
  * This primitive request that the transport provider send the specified
  * datagram to the specified destination.
  */
struct T_unitdata_req {
	t_scalar_t PRIM_type;		/**< Always T_UNITDATA_REQ. */
	t_scalar_t DEST_length;		/**< Dest addr length. */
	t_scalar_t DEST_offset;		/**< Dest addr offset. */
	t_scalar_t OPT_length;		/**< Options length. */
	t_scalar_t OPT_offset;		/**< Options offset. */
};

/** 
  * T_UNITDATA_IND, M_PROTO.
  *
  * This primitive indicates to the transport user that a datagram has been
  * received from the specified source address.
  */
struct T_unitdata_ind {
	t_scalar_t PRIM_type;		/**< T_UNITDATA_IND. */
	t_scalar_t SRC_length;		/**< Source addr length - see note in sec. 1.4. */
	t_scalar_t SRC_offset;		/**< Source addr offset. */
	t_scalar_t OPT_length;		/**< Options length - see note in sec. 1.4. */
	t_scalar_t OPT_offset;		/**< Options offset. */
};

/** 
  * T_UDERROR_IND, M_PROTO.
  *
  * This primitive indicates to the transport user that a datagram with the
  * specified destination address and options produced an error.
  */
struct T_uderror_ind {
	t_scalar_t PRIM_type;		/**< T_UDERROR_IND. */
	t_scalar_t DEST_length;		/**< Dest addr length - see note in sec. 1.4. */
	t_scalar_t DEST_offset;		/**< Dest addr offset. */
	t_scalar_t OPT_length;		/**< Options length - see note in sec. 1.4. */
	t_scalar_t OPT_offset;		/**< Options offset. */
	t_scalar_t ERROR_type;		/**< Error type. */
};

/** @} */

/** 
  * T_OPTDATA_REQ, M_PROTO.
  */
struct T_optdata_req {
	t_scalar_t PRIM_type;		/**< T_OPTDATA_REQ. */
	t_scalar_t DATA_flag;		/**< Flag bits associated with data. */
	t_scalar_t OPT_length;		/**< Options length. */
	t_scalar_t OPT_offset;		/**< Options offset. */
};

/** 
  * T_OPTDATA_IND, M_PROTO.
  */
struct T_optdata_ind {
	t_scalar_t PRIM_type;		/**< T_OPTDATA_REQ. */
	t_scalar_t DATA_flag;		/**< Flag bits associated with data. */
	t_scalar_t OPT_length;		/**< Options length. */
	t_scalar_t OPT_offset;		/**< Options offset. */
};

/** 
  * T_CAPABILITY_REQ, M_(PC)PROTO.
  */
struct T_capability_req {
	t_scalar_t PRIM_type;		/**< T_CAPABILITY_REQ. */
	t_uscalar_t CAP_bits1;		/**< Capability bits 1. */
};

/** 
  * T_CAPABILITY_ACK, M_PCPROTO.
  */
struct T_capability_ack {
	t_scalar_t PRIM_type;		/**< T_CAPABILITY_ACK. */
	t_uscalar_t CAP_bits1;		/**< Capability bits #1. */
	struct T_info_ack INFO_ack;	/**< Info acknowledgement. */
	t_uscalar_t ACCEPTOR_id;	/**< Accepting endpoint id. */
};

/** 
  * @name T_CAPABILITY Flags.
  * Flags for use with T_CAPABILITY_REQ and T_CAPABILITY_ACK primitives.
  * @{
  */
#define TC1_INFO	(1<<0)		/**< Contains T_info_ack. */
#define TC1_ACCEPTOR_ID	(1<<1)		/**< Contains acceptor id. */
#define TCI_CAP_BITS2	(1<<31)		/**< Contains extensions (unused). */
/** @} */

/** 
  * Union of all primitive formats.
  */
union T_primitives {
	t_scalar_t type;			/**< Overlapping type field */
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

#endif				/* _SYS_TIHDR_H */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
