/*****************************************************************************

 @(#) $Id: tpi.h,v 0.9 2004/01/17 08:08:42 brian Exp $

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/01/17 08:08:42 $ by $Author: brian $

 *****************************************************************************/

#ifndef _SYS_TPI_H
#define _SYS_TPI_H

#ident "@(#) $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

#ifdef __cplusplus
#pragma interface
#endif

#define T_CURRENT_VERSION 5

#ifndef t_scalar_t
typedef long t_scalar_t;
typedef unsigned long t_uscalar_t;
#endif

#if 0
struct t_info {
	long addr;
	long options;
	long tsdu;
	long etsdu;
	long connect;
	long discon;
	long servtype;
};
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
#define XPG4_1		2	/* This indicates that the transport provider conforms to XTI in XPG4 and
				   supports the new primitives T_ADDR_REQ and T_ADDR_ACK */

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
	long PRIM_type;			/* always T_INFO_REQ */
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
	long PRIM_type;			/* always T_BIND_REQ */
	long ADDR_length;		/* length of address */
	long ADDR_offset;		/* offset of address */
	t_uscalar_t CONIND_number;
	/* requested number of connect indications to be queued */
};

/*
 *  T_UNBIND_REQ, M_PROTO
 *
 *  This primitive requests that the transport provider unbind the protocol
 *  address associated with the stream and deactivate the stream.  The format
 */
struct T_unbind_req {
	long PRIM_type;			/* always T_UNBIND_REQ */
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
	long PRIM_type;			/* always T_OPTMGMT_REQ */
	long OPT_length;		/* options length */
	long OPT_offset;		/* options offset */
	long MGMT_flags;		/* flags */
};

/*
 *  T_ADDR_REQ, M_PROTO
 *
 *  This primitive requests that the transport provider return the local
 *  protocol address that is bound to the stream and the address of the remote
 *  transport entity if a connection has been established.
 */
struct T_addr_req {
	long PRIM_type;			/* always T_ADDR_REQ */
};

/* information acknowledgment */

/*
 *  T_INFO_ACK, M_PCPROTO
 *
 *  This primitive indicates to the transport user any relevant protocol-
 *  dependent parameters.  It should be initiated in response to the
 *  T_INFO_REQ primitives described above.
 */
struct T_info_ack {
	long PRIM_type;			/* always T_INFO_ACK */
	long TSDU_size;			/* max TSDU size */
	long ETSDU_size;		/* max ETSDU size */
	long CDATA_size;		/* Connect data size */
	long DDATA_size;		/* Discon data size */
	long ADDR_size;			/* TSAP size */
	long OPT_size;			/* options size */
	long TIDU_size;			/* TIDU size */
	long SERV_type;			/* service type */
	long CURRENT_state;		/* current state */
	long PROVIDER_flag;		/* provider flags */
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
	long PRIM_type;			/* always T_BIND_ACK */
	long ADDR_length;		/* length of address - see note in sec. 1.4 */
	long ADDR_offset;		/* offset of address */
	t_uscalar_t CONIND_number;	/* connect indications to be queued */
};

/*
 *  T_OPTMGMT_ACK, M_PCPROTO
 *
 *  This indicates to the transport user that the options management request
 *  has completed.
 */
struct T_optmgmt_ack {
	long PRIM_type;			/* always T_OPTMGMT_ACK */
	long OPT_length;		/* options length - see note in sec. 1.4 */
	long OPT_offset;		/* options offset */
	long MGMT_flags;		/* flags */
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
	long PRIM_type;			/* always T_ERROR_ACK */
	long ERROR_prim;		/* primitive in error */
	long TLI_error;			/* TLI error code - see not in sec. 1.4 */
	long UNIX_error;		/* UNIX error code - see not in sec. 1.4 */
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
	long PRIM_type;			/* always T_OK_ACK */
	long CORRECT_prim;		/* primitive */
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
	long PRIM_type;			/* always T_ADDR_ACK */
	long LOCADDR_length;		/* length of local address - see not in sec. 1.4 */
	long LOCADDR_offset;		/* offset of local address */
	long REMADDR_length;		/* length of remote address - see not in sec. 1.4 */
	long REMADDR_offset;		/* offset of remote address */
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
	long PRIM_type;			/* always T_CONN_REQ */
	long DEST_length;		/* dest addr length */
	long DEST_offset;		/* dest addr offset */
	long OPT_length;		/* options length */
	long OPT_offset;		/* options offset */
};

/*
 *  T_CONN_RES, M_PROTO (followed by 0 or more M_DATA)
 *
 *  This primitive requests that the transport provider accept a previous
 *  connect request on the specified response queue.
 */
struct T_conn_res {
	long PRIM_type;			/* always T_CONN_RES */
	long ACCEPTOR_id;		/* reponse queue ptr */
	long OPT_length;		/* options length */
	long OPT_offset;		/* options offset */
	long SEQ_number;		/* sequence number */
};

/*
 *  T_DSICON_REQ, M_PROTO (followed by 0 or more M_DATA)
 *
 *  This primitive request that the transport provider deny a request for
 *  connection, or disconnect an existing connection.
 */
struct T_discon_req {
	long PRIM_type;			/* always T_DISCON_REQ */
	long SEQ_number;		/* sequence number */
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
	long PRIM_type;			/* always T_DATA_REQ */
	long MORE_flag;			/* indicates more data in TSDU */
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
	long PRIM_type;			/* T_EXDATA_REQ */
	long MORE_flag;
};

/*
 *  T_ORDREL_REQ, M_PROTO
 *
 *  This primitive indicates to the transport provider that the user is
 *  finished sending data.  This primitive is only supported by the transport
 *  provider if it is of type T_COTS_ORD.
 */
struct T_ordrel_req {
	long PRIM_type;			/* always T_ORDREL_REQ */
};

/*
 *  T_CONN_IND, M_PROTO
 *
 *  This primnitive indicates to the transport user that a connect request to
 *  the user has been made by the user at the specified source address.  The
 */
struct T_conn_ind {
	long PRIM_type;			/* always T_CONN_IND */
	long SRC_length;		/* source addr length = see note in sec. 1.4 */
	long SRC_offset;		/* source addr offset */
	long OPT_length;		/* options length = see note in sec. 1.4 */
	long OPT_offset;		/* options offset */
	long SEQ_number;		/* sequence number - see not in sec. 1.4 */
};

/*
 *  T_CONN_CON, M_PROTO, (followed by 0 or more M_DATA)
 *
 *  This primitive indicates to the user that a connect request has been
 *  confirmed on the specified responding address.
 */
struct T_conn_con {
	long PRIM_type;			/* T_CONN_CON */
	long RES_length;		/* responding addr length - see note in sec. 1.4 */
	long RES_offset;		/* responding addr offset */
	long OPT_length;		/* options length - see note in sec. 1.4 */
	long OPT_offset;		/* options offset */
};

/*
 *  T_DISCON_IND, M_PROTO (followed by 0 or more M_DATA)
 *
 *  This primitive indicates to the user that either a request for connection
 *  has been denied or and existing connection has been disconnected.
 */
struct T_discon_ind {
	long PRIM_type;			/* T_DISCON_IND */
	long DISCON_reason;		/* disconnect reason - see note in sec. 1.4 */
	long SEQ_number;		/* sequence number - see note in sec. 1.4 */
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
	long PRIM_type;			/* always T_DATA_IND */
	long MORE_flag;			/* indicates more data in TSDU */
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
	long PRIM_type;			/* always T_EXDATA_IND */
	long MORE_flag;			/* indicates more data in ETSDU */
};

/*
 *  T_ORDREL_IND, M_PROTO
 *
 *  This primitive indicates to the transport user that the other side of the
 *  connection is finished sending data.  This primitive is only supported by
 *  the transport provider if it is of type T_COTS_ORD.
 */
struct T_ordrel_ind {
	long PRIM_type;			/* always T_ORDREL_IND */
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
	long PRIM_type;			/* always T_UNITDATA_REQ */
	long DEST_length;		/* dest addr length */
	long DEST_offset;		/* dest addr offset */
	long OPT_length;		/* options length */
	long OPT_offset;		/* options offset */
};

/*
 *  T_UNITDATA_IND, M_PROTO
 *
 *  This primitive indicates to the transport user that a datagram has been
 *  received from the specified source address.
 */
struct T_unitdata_ind {
	long PRIM_type;			/* T_UNITDATA_IND */
	long SRC_length;		/* source addr length - see note in sec. 1.4 */
	long SRC_offset;		/* source addr offset */
	long OPT_length;		/* options length - see note in sec. 1.4 */
	long OPT_offset;		/* options offset */
};

/*
 *  T_UDERROR_IND, M_PROTO
 *
 *  This primitive indicates to the transport user that a datagram with the
 *  specified destination address and options produced an error.
 */
struct T_uderror_ind {
	long PRIM_type;			/* T_UDERROR_IND */
	long DEST_length;		/* dest addr length - see note in sec. 1.4 */
	long DEST_offset;		/* dest addr offset */
	long OPT_length;		/* options length - see note in sec. 1.4 */
	long OPT_offset;		/* options offset */
	long ERROR_type;		/* error type */
};

struct T_optdata_req {
	long PRIM_type;			/* T_OPTDATA_REQ */
	long DATA_flag;			/* flag bits associated with data */
	long OPT_length;		/* options length */
	long OPT_offset;		/* options offset */
};

struct T_optdata_ind {
	long PRIM_type;			/* T_OPTDATA_REQ */
	long DATA_flag;			/* flag bits associated with data */
	long OPT_length;		/* options length */
	long OPT_offset;		/* options offset */
};

union T_primitives {
	long type;
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

};

#ifndef T_OPT_FIRSTHDR
/* The t_opthdr structure defines the layout of options in a T_OPTMGMT_*
 * data buffer. This is specified in the X/Open specs but does not
 * appear to exist in SCO 3.2.x, SCO OS5, Interactive SVR4 or UnixWare 1.x.
 * There are programs that make options request however.
 * The older TLI uses struct opthdr which is different and incompatible
 * (see below).
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
	((char *)(((unsigned long)(p) - (o) + _T_ALIGN_SIZE - 1)	\
		    & ~(_T_ALIGN_SIZE - 1)) + (o))

#define _T_ALIGN_OFFSET(p)						\
	((unsigned long)(p) & (_T_ALIGN_SIZE - 1))

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

#endif				/* _SYS_TPI_H */
