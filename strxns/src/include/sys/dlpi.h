/*****************************************************************************

 @(#) $Id: dlpi.h,v 0.9.2.11 2008-07-06 14:58:20 brian Exp $

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

 Last Modified $Date: 2008-07-06 14:58:20 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dlpi.h,v $
 Revision 0.9.2.11  2008-07-06 14:58:20  brian
 - improvements

 Revision 0.9.2.10  2008-06-18 16:45:39  brian
 - widespread updates

 Revision 0.9.2.9  2008-05-25 12:49:27  brian
 - updated manual pages and DLPI header

 Revision 0.9.2.8  2008-04-25 11:39:32  brian
 - updates to AGPLv3

 Revision 0.9.2.7  2007/08/14 03:31:14  brian
 - GPLv3 header update

 Revision 0.9.2.6  2006/09/18 13:52:47  brian
 - added doxygen markers to sources

 Revision 0.9.2.5  2006/03/03 11:27:48  brian
 - 32/64-bit compatibility

 *****************************************************************************/

#ifndef _SYS_DLPI_H
#define _SYS_DLPI_H

#ident "@(#) $RCSfile: dlpi.h,v $ $Name:  $($Revision: 0.9.2.11 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

/// @file
/// @brief This header file provides the Data Link Provider Interface (DLPI).

#ifdef __LP64__
typedef int32_t dl_long;
typedef u_int32_t dl_ulong;
typedef u_int16_t dl_ushort;
#else				/* __LP64__ */
typedef long dl_long;
typedef ulong dl_ulong;
typedef ushort dl_ushort;
#endif				/* __LP64__ */

/* 
   DLPI revision definition history
 */
#define DL_CURRENT_VERSION		0x02	/* current version of DLPI */
#define DL_VERSION_2			0x02	/* DLPI March 12, 1991 */

/* 
   Primitives for Local Management Services
 */
#define DL_INFO_REQ			0x00	/* Information Req */
#define DL_INFO_ACK			0x03	/* Information Ack */
#define DL_ATTACH_REQ			0x0b	/* Attach a PPA */
#define DL_DETACH_REQ			0x0c	/* Detach a PPA */
#define DL_BIND_REQ			0x01	/* Bind dlsap address */
#define DL_BIND_ACK			0x04	/* Dlsap address bound */
#define DL_UNBIND_REQ			0x02	/* Unbind dlsap address */
#define DL_OK_ACK			0x06	/* Success acknowledgment */
#define DL_ERROR_ACK			0x05	/* Error acknowledgment */
#define DL_SUBS_BIND_REQ		0x1b	/* Bind Subsequent DLSAP address */
#define DL_SUBS_BIND_ACK		0x1c	/* Subsequent DLSAP address bound */
#define DL_SUBS_UNBIND_REQ		0x15	/* Subsequent unbind */
#define DL_ENABMULTI_REQ		0x1d	/* Enable multicast addresses */
#define DL_DISABMULTI_REQ		0x1e	/* Disable multicast addresses */
#define DL_PROMISCON_REQ		0x1f	/* Turn on promiscuous mode */
#define DL_PROMISCOFF_REQ		0x20	/* Turn off promiscuous mode */

/* 
   Primitives used for Connectionless Service
 */
#define DL_UNITDATA_REQ			0x07	/* datagram send request */
#define DL_UNITDATA_IND			0x08	/* datagram receive indication */
#define DL_UDERROR_IND			0x09	/* datagram error indication */
#define DL_UDQOS_REQ			0x0a	/* set QOS for subsequent datagrams */

/* 
   Primitives used for Connection-Oriented Service
 */
#define DL_CONNECT_REQ			0x0d	/* Connect request */
#define DL_CONNECT_IND			0x0e	/* Incoming connect indication */
#define DL_CONNECT_RES			0x0f	/* Accept previous connect indication */
#define DL_CONNECT_CON			0x10	/* Connection established */
#define DL_TOKEN_REQ			0x11	/* Passoff token request */
#define DL_TOKEN_ACK			0x12	/* Passoff token ack */
#define DL_DISCONNECT_REQ		0x13	/* Disconnect request */
#define DL_DISCONNECT_IND		0x14	/* Disconnect indication */
#define DL_RESET_REQ			0x17	/* Reset service request */
#define DL_RESET_IND			0x18	/* Incoming reset indication */
#define DL_RESET_RES			0x19	/* Complete reset processing */
#define DL_RESET_CON			0x1a	/* Reset processing complete */

/* 
   Primitives used for Acknowledged Connectionless Service
 */
#define DL_DATA_ACK_REQ			0x21	/* data unit transmission request */
#define DL_DATA_ACK_IND			0x22	/* Arrival of a command PDU */
#define DL_DATA_ACK_STATUS_IND		0x23	/* Status indication of DATA_ACK_REQ */
#define DL_REPLY_REQ			0x24	/* Request a DLSDU from the remote */
#define DL_REPLY_IND			0x25	/* Arrival of a command PDU */
#define DL_REPLY_STATUS_IND		0x26	/* Status indication of REPLY_REQ */
#define DL_REPLY_UPDATE_REQ		0x27	/* Hold a DLSDU for transmission */
#define DL_REPLY_UPDATE_STATUS_IND	0x28	/* Status of REPLY_UPDATE req */

/* 
   Primitives used for XID and TEST operations
 */
#define DL_XID_REQ			0x29	/* Request to send an XID PDU */
#define DL_XID_IND			0x2a	/* Arrival of an XID PDU */
#define DL_XID_RES			0x2b	/* request to send a response XID PDU */
#define DL_XID_CON			0x2c	/* Arrival of a response XID PDU */
#define DL_TEST_REQ			0x2d	/* TEST command request */
#define DL_TEST_IND			0x2e	/* TEST response indication */
#define DL_TEST_RES			0x2f	/* TEST response */
#define DL_TEST_CON			0x30	/* TEST Confirmation */

/* 
   Primitives to get and set the physical address
 */
#define DL_PHYS_ADDR_REQ		0x31	/* Request to get physical addr */
#define DL_PHYS_ADDR_ACK		0x32	/* Return physical addr */
#define DL_SET_PHYS_ADDR_REQ		0x33	/* set physical addr */

/* 
   Primitives to get statistics
 */
#define DL_GET_STATISTICS_REQ		0x34	/* Request to get statistics */
#define DL_GET_STATISTICS_ACK		0x35	/* Return statistics */

/* 
   Invalid primitive
 */
#define DL_PRIM_INVAL			0xffff	/* Invalid DL primitive value */

/* 
   DLPI interface states
 */
#define DL_UNATTACHED			0x04	/* PPA not attached */
#define DL_ATTACH_PENDING		0x05	/* Waiting ack of DL_ATTACH_REQ */
#define DL_DETACH_PENDING		0x06	/* Waiting ack of DL_DETACH_REQ */
#define DL_UNBOUND			0x00	/* PPA attached */
#define DL_BIND_PENDING			0x01	/* Waiting ack of DL_BIND_REQ */
#define DL_UNBIND_PENDING		0x02	/* Waiting ack of DL_UNBIND_REQ */
#define DL_IDLE				0x03	/* dlsap bound, awaiting use */
#define DL_UDQOS_PENDING		0x07	/* Waiting ack of DL_UDQOS_REQ */
#define DL_OUTCON_PENDING		0x08	/* awaiting DL_CONN_CON */
#define DL_INCON_PENDING		0x09	/* awaiting DL_CONN_RES */
#define DL_CONN_RES_PENDING		0x0a	/* Waiting ack of DL_CONNECT_RES */
#define DL_DATAXFER			0x0b	/* connection-oriented data transfer */
#define DL_USER_RESET_PENDING		0x0c	/* awaiting DL_RESET_CON */
#define DL_PROV_RESET_PENDING		0x0d	/* awaiting DL_RESET_RES */
#define DL_RESET_RES_PENDING		0x0e	/* Waiting ack of DL_RESET_RES */
#define DL_DISCON8_PENDING		0x0f	/* Waiting ack of DL_DISC_REQ */
#define DL_DISCON9_PENDING		0x10	/* Waiting ack of DL_DISC_REQ */
#define DL_DISCON11_PENDING		0x11	/* Waiting ack of DL_DISC_REQ */
#define DL_DISCON12_PENDING		0x12	/* Waiting ack of DL_DISC_REQ */
#define DL_DISCON13_PENDING		0x13	/* Waiting ack of DL_DISC_REQ */
#define DL_SUBS_BIND_PND		0x14	/* Waiting ack of DL_SUBS_BIND_REQ */
#define DL_SUBS_UNBIND_PND		0x15	/* Waiting ack of DL_SUBS_UNBIND_REQ */

/* 
   DL_ERROR_ACK error return values
 */
#define DL_ACCESS			0x02	/* Improper permissions for request */
#define DL_BADADDR			0x01	/* DLSAP addr in improper format or invalid */
#define DL_BADCORR			0x05	/* Seq number not from outstand DL_CONN_IND */
#define DL_BADDATA			0x06	/* User data exceeded provider limit */
#define DL_BADPPA			0x08	/* Specified PPA was invalid */
#define DL_BADPRIM			0x09	/* Primitive received not known by provider */
#define DL_BADQOSPARAM			0x0a	/* QOS parameters contained invalid values */
#define DL_BADQOSTYPE			0x0b	/* QOS structure type is unknown/unsupported */
#define DL_BADSAP			0x00	/* Bad LSAP selector */
#define DL_BADTOKEN			0x0c	/* Token used not an active stream */
#define DL_BOUND			0x0d	/* Attempted second bind with dl_max_conind */
#define DL_INITFAILED			0x0e	/* Physical Link initialization failed */
#define DL_NOADDR			0x0f	/* Provider couldn't allocate alt. address */
#define DL_NOTINIT			0x10	/* Physical Link not initialized */
#define DL_OUTSTATE			0x03	/* Primitive issued in improper state */
#define DL_SYSERR			0x04	/* UNIX system error occurred */
#define DL_UNSUPPORTED			0x07	/* Requested serv. not supplied by provider */
#define DL_UNDELIVERABLE		0x11	/* Previous data unit could not be delivered */
#define DL_NOTSUPPORTED			0x12	/* Primitive is known but not supported */
#define DL_TOOMANY			0x13	/* Limit exceeded */
#define DL_NOTENAB			0x14	/* Promiscuous mode not enabled */
#define DL_BUSY				0x15	/* Other streams for PPA in post-attached state */
#define DL_NOAUTO			0x16	/* Automatic handling of XID and TEST response not
						   supported.  */
#define DL_NOXIDAUTO			0x17	/* Automatic handling of XID not supported */
#define DL_NOTESTAUTO			0x18	/* Automatic handling of TEST not supported */
#define DL_XIDAUTO			0x19	/* Automatic handling of XID response */
#define DL_TESTAUTO			0x1a	/* Automatic handling of TEST response */
#define DL_PENDING			0x1b	/* pending outstanding connect indications */

/* 
   NOTE: The range of error codes from 0x80 - 0xff is reserved for implementation specific error
   codes.  This reserved range of error codes will be defined by the DLS Provider.
 */

/* 
   DLPI media types supported
 */
#define DL_CSMACD			0x00	/* IEEE 802.3 CSMA/CD network */
#define DL_TPB				0x01	/* IEEE 802.4 Token Passing Bus */
#define DL_TPR				0x02	/* IEEE 802.5 Token Passing Ring */
#define DL_METRO			0x03	/* IEEE 802.6 Metro Net */
#define DL_ETHER			0x04	/* Ethernet Bus */
#define DL_HDLC				0x05	/* ISO HDLC protocol support */
#define DL_CHAR				0x06	/* Character Synchronous protocol support */
#define DL_CTCA				0x07	/* IBM Channel-to-Channel Adapter */
#define DL_FDDI				0x08	/* Fiber Distributed data interface */
#define DL_FC				0x10	/* Fibre Channel interface */
#define DL_ATM				0x11	/* ATM */
#define DL_IPATM			0x12	/* ATM Classical IP interface */
#define DL_X25				0x13	/* X.25 LAPB interface */
#define DL_ISDN				0x14	/* ISDN interface */
#define DL_HIPPI			0x15	/* HIPPI interface */
#define DL_100VG			0x16	/* 100 Based VG Ethernet */
#define DL_100VGTPR			0x17	/* 100 Based VG Token Ring */
#define DL_ETH_CSMA			0x18	/* ISO 8802/3 and Ethernet */
#define DL_100BT			0x19	/* 100 Base T */
#define DL_IB				0x1a	/* Infiniband */
#define DL_FRAME			0x0a	/* Frame Relay LAPF */
#define DL_MPFRAME			0x0b	/* Multi-protocol over Frame Relay */
#define DL_ASYNC			0x0c	/* Character Asynchronous Protocol */
#define DL_IPX25			0x0d	/* X.25 Classical IP interface */
#define DL_LOOP				0x0e	/* software loopback */
#define DL_OTHER			0x09	/* Any other medium not listed above */

/* 
   DLPI provider service supported.

   These must be allowed to be bitwise-OR for dl_service_mode in DL_INFO_ACK.
 */
#define DL_CODLS			0x01	/* connection-oriented service */
#define DL_CLDLS			0x02	/* connectionless data link service */
#define DL_ACLDLS			0x04	/* acknowledged connectionless service */
#ifdef _HPUX_SOURCE
#define DL_HP_RAWDLS			0x08	/* raw data link service */
#endif				/* _HPUX_SOURCE */

/* 
   DLPI provider style.

   The DLPI provider style which determines whether a provider requires a DL_ATTACH_REQ to inform
   the provider which PPA user messages should be sent/received on.
 */
#define DL_STYLE1			0x0500	/* PPA is implicitly bound by open(2) */
#define DL_STYLE2			0x0501	/* PPA must be expl. bound via DL_ATTACH_REQ */

/* 
   DLPI Originator for Disconnect and Resets
 */
#define DL_PROVIDER			0x0700
#define DL_USER				0x0701

/* 
   DLPI Disconnect Reasons
 */
#define DL_CONREJ_DEST_UNKNOWN			0x0800
#define DL_CONREJ_DEST_UNREACH_PERMANENT	0x0801
#define DL_CONREJ_DEST_UNREACH_TRANSIENT	0x0802
#define DL_CONREJ_QOS_UNAVAIL_PERMANENT		0x0803
#define DL_CONREJ_QOS_UNAVAIL_TRANSIENT		0x0804
#define DL_CONREJ_PERMANENT_COND		0x0805
#define DL_CONREJ_TRANSIENT_COND		0x0806
#define DL_DISC_ABNORMAL_CONDITION		0x0807
#define DL_DISC_NORMAL_CONDITION		0x0808
#define DL_DISC_PERMANENT_CONDITION		0x0809
#define DL_DISC_TRANSIENT_CONDITION		0x080a
#define DL_DISC_UNSPECIFIED			0x080b

/* 
   DLPI Reset Reasons
 */
#define DL_RESET_FLOW_CONTROL		0x0900
#define DL_RESET_LINK_ERROR		0x0901
#define DL_RESET_RESYNCH		0x0902

/* 
   DLPI status values for acknowledged connectionless data transfer
 */
#define DL_CMD_MASK			0x0f	/* mask for command portion of status */
#define DL_CMD_OK			0x00	/* Command Accepted */
#define DL_CMD_RS			0x01	/* Unimplemented or inactivated service */
#define DL_CMD_UE			0x05	/* Data Link User interface error */
#define DL_CMD_PE			0x06	/* Protocol error */
#define DL_CMD_IP			0x07	/* Permanent implementation dependent error */
#define DL_CMD_UN			0x09	/* Resources temporarily unavailable */
#define DL_CMD_IT			0x0f	/* Temporary implementation dependent error */

#define DL_RSP_MASK			0xf0	/* mask for response portion of status */
#define DL_RSP_OK			0x00	/* Response DLSDU present */
#define DL_RSP_RS			0x10	/* Unimplemented or inactivated service */
#define DL_RSP_NE			0x30	/* Response DLSDU never submitted */
#define DL_RSP_NR			0x40	/* Response DLSDU not requested */
#define DL_RSP_UE			0x50	/* Data Link User interface error */
#define DL_RSP_IP			0x70	/* Permanent implementation dependent error */
#define DL_RSP_UN			0x90	/* Resources temporarily unavailable */
#define DL_RSP_IT			0xf0	/* Temporary implementation dependent error */

/* 
   Service Class values for acknowledged connectionless data transfer
 */
#define DL_RQST_RSP			0x01	/* Use acknowledge capability in MAC sublayer */
#define DL_RQST_NORSP			0x02	/* No acknowledgement service requested */

/* 
   DLPI address type definition
 */
#define DL_FACT_PHYS_ADDR		0x01	/* factory physical address */
#define DL_CURR_PHYS_ADDR		0x02	/* current physical address */
#define DL_IPV6_TOKEN			0x03	/* IPv6 interface token */
#define DL_IPV6_LINK_LAYER_ADDR		0x04	/* Neighbor Discovery format */

/* 
   DLPI flag definitions
 */
#define DL_POLL_FINAL			0x01	/* poll/final bit for TEST/XID */

/* 
   XID and TEST responses supported by the provider
 */
#define DL_AUTO_XID			0x01	/* provider will respond to XID */
#define DL_AUTO_TEST			0x02	/* provider will respond to TEST */

/* 
   Subsequent bind types
 */
#define DL_PEER_BIND			0x01	/* subsequent bind on a peer addr */
#define DL_HIERARCHICAL_BIND		0x02	/* subs-bind on a hierarchical addr */

/* 
   DLPI promiscuous mode definitions
 */
#define DL_PROMISC_PHYS			0x01	/* promiscuous mode at phys level */
#define DL_PROMISC_SAP			0x02	/* promiscous mode at sap level */
#define DL_PROMISC_MULTI		0x03	/* promiscuous mode for multicast */

/* 
   DLPI Quality Of Service definition for use in QOS structure definitions. The QOS structures are
   used in connection establishment, DL_INFO_ACK, and setting connectionless QOS values.
 */

/* 
   Throughput

   This parameter is specified for both directions.
 */
typedef struct {
	dl_long dl_target_value;	/* bits/second desired */
	dl_long dl_accept_value;	/* min. ok bits/second */
} dl_through_t;

/* 
   transit delay specification

   This parameter is specified for both directions. expressed in milliseconds assuming a DLSDU size
   of 128 octets. The scaling of the value to the current DLSDU size is provider dependent.
 */
typedef struct {
	dl_long dl_target_value;	/* desired value of service */
	dl_long dl_accept_value;	/* min. ok value of service */
} dl_transdelay_t;

/* 
   priority specification

   priority range is 0-100, with 0 being highest value.
 */
typedef struct {
	dl_long dl_min;
	dl_long dl_max;
} dl_priority_t;

/* 
   protection specification
 */
#define DL_NONE				0x0B01	/* no protection supplied */
#define DL_MONITOR			0x0B02	/* prot. from passive monit. */
#define DL_MAXIMUM			0x0B03	/* prot. from modification, replay, addition, or
						   deletion */

typedef struct {
	dl_long dl_min;
	dl_long dl_max;
} dl_protect_t;

/* 
   Resilience specification

   probabilities are scaled by a factor of 10,000 with a time interval of 10,000 seconds.
 */
typedef struct {
	dl_long dl_disc_prob;		/* prob. of provider init DISC */
	dl_long dl_reset_prob;		/* prob. of provider init RESET */
} dl_resilience_t;

/* 
   QOS type definition to be used for negotiation with the remote end of a connection, or a
   connectionless unitdata request. There are two type definitions to handle the negotiation
   process at connection establishment. The typedef dl_qos_range_t is used to present a range for
   parameters. This is used in the DL_CONNECT_REQ and DL_CONNECT_IND messages. The typedef
   dl_qos_sel_t is used to select a specific value for the QOS parameters. This is used in the
   DL_CONNECT_RES, DL_CONNECT_CON, and DL_INFO_ACK messages to define the selected QOS parameters
   for a connection.

   NOTE: A DataLink provider which has unknown values for any of the fields will use a value of
   DL_UNKNOWN for all values in the fields.

   NOTE: A QOS parameter value of DL_QOS_DONT_CARE informs the DLS provider the user requesting
   this value doesn't care what the QOS parameter is set to. This value becomes the least possible
   value in the range of QOS parameters. The order of the QOS parameter range is then:

   DL_QOS_DONT_CARE < 0 < MAXIMUM QOS VALUE
 */
#define DL_UNKNOWN		-1
#define DL_QOS_DONT_CARE	-2

/* 
   Every QOS structure has the first 4 bytes containing a type field, denoting the definition of
   the rest of the structure. This is used in the same manner has the dl_primitive variable is in
   messages.

   The following list is the defined QOS structure type values and structures.
 */
#define DL_QOS_CO_RANGE1		0x0101	/* CO QOS range struct. */
#define DL_QOS_CO_SEL1			0x0102	/* CO QOS selection structure */
#define DL_QOS_CL_RANGE1		0x0103	/* CL QOS range struct. */
#define DL_QOS_CL_SEL1			0x0104	/* CL QOS selection */

typedef struct {
	dl_ulong dl_qos_type;
	dl_through_t dl_rcv_throughput;	/* desired and accep. */
	dl_transdelay_t dl_rcv_trans_delay;	/* desired and accep. */
	dl_through_t dl_xmt_throughput;
	dl_transdelay_t dl_xmt_trans_delay;
	dl_priority_t dl_priority;	/* min and max values */
	dl_protect_t dl_protection;	/* min and max values */
	dl_long dl_residual_error;
	dl_resilience_t dl_resilience;
} dl_qos_co_range1_t;

typedef struct {
	dl_ulong dl_qos_type;
	dl_long dl_rcv_throughput;
	dl_long dl_rcv_trans_delay;
	dl_long dl_xmt_throughput;
	dl_long dl_xmt_trans_delay;
	dl_long dl_priority;
	dl_long dl_protection;
	dl_long dl_residual_error;
	dl_resilience_t dl_resilience;
} dl_qos_co_sel1_t;

typedef struct {
	dl_ulong dl_qos_type;
	dl_transdelay_t dl_trans_delay;
	dl_priority_t dl_priority;
	dl_protect_t dl_protection;
	dl_long dl_residual_error;
} dl_qos_cl_range1_t;

typedef struct {
	dl_ulong dl_qos_type;
	dl_long dl_trans_delay;
	dl_long dl_priority;
	dl_long dl_protection;
	dl_long dl_residual_error;
} dl_qos_cl_sel1_t;

union DL_qos_types {
	dl_ulong dl_qos_type;
	dl_qos_co_range1_t qos_co_range1;
	dl_qos_co_sel1_t qos_co_sel1;
	dl_qos_cl_range1_t qos_cl_range1;
	dl_qos_cl_sel1_t qos_cl_sel1;
};

/* 
   DLPI interface primitive definitions.

   Each primitive is sent as a stream message.  It is possible that the messages may be viewed as a
   sequence of bytes that have the following form without any padding. The structure definition of
   the following messages may have to change depending on the underlying hardware architecture and
   crossing of a hardware boundary with a different hardware architecture.

   Fields in the primitives having a name of the form dl_reserved cannot be used and have the value
   of binary zero, no bits turned on.

   Each message has the name defined followed by the stream message type (M_PROTO, M_PCPROTO,
   M_DATA)
 */

/* 
   LOCAL MANAGEMENT SERVICE PRIMITIVES
 */

/* 
   DL_INFO_REQ, M_PCPROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* set to DL_INFO_REQ */
} dl_info_req_t;

/* 
   DL_INFO_ACK, M_PCPROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* set to DL_INFO_ACK */
	dl_ulong dl_max_sdu;		/* Max bytes in a DLSDU */
	dl_ulong dl_min_sdu;		/* Min bytes in a DLSDU */
	dl_ulong dl_addr_length;	/* length of DLSAP address */
	dl_ulong dl_mac_type;		/* type of medium supported */
	dl_ulong dl_reserved;		/* value set to zero */
	dl_ulong dl_current_state;	/* state of DLPI interface */
	dl_long dl_sap_length;		/* length of dlsap SAP part */
	dl_ulong dl_service_mode;	/* CO, CL or ACL */
	dl_ulong dl_qos_length;		/* length of qos values */
	dl_ulong dl_qos_offset;		/* offset from start of block */
	dl_ulong dl_qos_range_length;	/* available range of qos */
	dl_ulong dl_qos_range_offset;	/* offset from start of block */
	dl_ulong dl_provider_style;	/* style1 or style2 */
	dl_ulong dl_addr_offset;	/* offset of the dlsap addr */
	dl_ulong dl_version;		/* version number */
	dl_ulong dl_brdcst_addr_length;	/* length of broadcast addr */
	dl_ulong dl_brdcst_addr_offset;	/* offset from start of block */
	dl_ulong dl_growth;		/* set to zero */
} dl_info_ack_t;

/* 
   DL_ATTACH_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* set to DL_ATTACH_REQ */
	dl_ulong dl_ppa;		/* id of the PPA */
} dl_attach_req_t;

/* 
   DL_DETACH_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* set to DL_DETACH_REQ */
} dl_detach_req_t;

/* 
   DL_BIND_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* set to DL_BIND_REQ */
	dl_ulong dl_sap;		/* info to identify dlsap addr */
	dl_ulong dl_max_conind;		/* max # of outstanding con_ind */
	dl_ushort dl_service_mode;	/* CO, CL or ACL */
	dl_ushort dl_conn_mgmt;		/* if non-zero, is con-mgmt stream */
	dl_ulong dl_xidtest_flg;	/* auto init. of test and xid */
} dl_bind_req_t;

/* 
   DL_BIND_ACK, M_PCPROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_BIND_ACK */
	dl_ulong dl_sap;		/* DLSAP addr info */
	dl_ulong dl_addr_length;	/* length of complete DLSAP addr */
	dl_ulong dl_addr_offset;	/* offset from start of M_PCPROTO */
	dl_ulong dl_max_conind;		/* allowed max. # of con-ind */
	dl_ulong dl_xidtest_flg;	/* responses supported by provider */
} dl_bind_ack_t;

/* 
   DL_SUBS_BIND_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_SUBS_BIND_REQ */
	dl_ulong dl_subs_sap_offset;	/* offset of subs_sap */
	dl_ulong dl_subs_sap_length;	/* length of subs_sap */
	dl_ulong dl_subs_bind_class;	/* peer or hierarchical */
} dl_subs_bind_req_t;

#define dl_subs_sap_len dl_subs_sap_length	/* SCO compatibility */

/* 
   DL_SUBS_BIND_ACK, M_PCPROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_SUBS_BIND_ACK */
	dl_ulong dl_subs_sap_offset;	/* offset of subs_sap */
	dl_ulong dl_subs_sap_length;	/* length of subs_sap */
} dl_subs_bind_ack_t;

/* 
   DL_UNBIND_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_UNBIND_REQ */
} dl_unbind_req_t;

/* 
   DL_SUBS_UNBIND_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_SUBS_UNBIND_REQ */
	dl_ulong dl_subs_sap_offset;	/* offset of subs_sap */
	dl_ulong dl_subs_sap_length;	/* length of subs_sap */
} dl_subs_unbind_req_t;

/* 
   DL_OK_ACK, M_PCPROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_OK_ACK */
	dl_ulong dl_correct_primitive;	/* primitive acknowledged */
} dl_ok_ack_t;

/* 
   DL_ERROR_ACK, M_PCPROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_ERROR_ACK */
	dl_ulong dl_error_primitive;	/* primitive in error */
	dl_ulong dl_errno;		/* DLPI error code */
	dl_ulong dl_unix_errno;		/* UNIX system error code */
} dl_error_ack_t;

/* 
   DL_ENABMULTI_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_ENABMULTI_REQ */
	dl_ulong dl_addr_length;	/* length of multicast address */
	dl_ulong dl_addr_offset;	/* offset from start of M_PROTO block */
} dl_enabmulti_req_t;

/* 
   DL_DISABMULTI_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_DISABMULTI_REQ */
	dl_ulong dl_addr_length;	/* length of multicast address */
	dl_ulong dl_addr_offset;	/* offset from start of M_PROTO block */
} dl_disabmulti_req_t;

/* 
   DL_PROMISCON_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_PROMISCON_REQ */
	dl_ulong dl_level;		/* physical,SAP, or ALL multicast */
} dl_promiscon_req_t;

/* 
   DL_PROMISCOFF_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_PROMISCOFF_REQ */
	dl_ulong dl_level;		/* Physical,SAP, or ALL multicast */
} dl_promiscoff_req_t;

/* 
   Primitives to get and set the Physical address
 */

/* 
   DL_PHYS_ADDR_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_PHYS_ADDR_REQ */
	dl_ulong dl_addr_type;		/* factory or current physical addr */
} dl_phys_addr_req_t;

/* 
   DL_PHYS_ADDR_ACK, M_PCPROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_PHYS_ADDR_ACK */
	dl_ulong dl_addr_length;	/* length of the physical addr */
	dl_ulong dl_addr_offset;	/* offset from start of block */
} dl_phys_addr_ack_t;

/* 
   DL_SET_PHYS_ADDR_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_SET_PHYS_ADDR_REQ */
	dl_ulong dl_addr_length;	/* length of physical addr */
	dl_ulong dl_addr_offset;	/* offset from start of block */
} dl_set_phys_addr_req_t;

/* 
   Primitives to get statistics
 */

/* 
   DL_GET_STATISTICS_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_GET_STATISTICS_REQ */
} dl_get_statistics_req_t;

/* 
   DL_GET_STATISTICS_ACK, M_PCPROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_GET_STATISTICS_ACK */
	dl_ulong dl_stat_length;	/* length of statistics structure */
	dl_ulong dl_stat_offset;	/* offset from start of block */
} dl_get_statistics_ack_t;

/* 
   CONNECTION-ORIENTED SERVICE PRIMITIVES
 */

/* 
   DL_CONNECT_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_CONNECT_REQ */
	dl_ulong dl_dest_addr_length;	/* len. of dlsap addr */
	dl_ulong dl_dest_addr_offset;	/* offset */
	dl_ulong dl_qos_length;		/* len. of QOS parm val */
	dl_ulong dl_qos_offset;		/* offset */
	dl_ulong dl_growth;		/* set to zero */
} dl_connect_req_t;

/* 
   DL_CONNECT_IND, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_CONNECT_IND */
	dl_ulong dl_correlation;	/* provider's correl. token */
	dl_ulong dl_called_addr_length;	/* length of called address */
	dl_ulong dl_called_addr_offset;	/* offset from start of block */
	dl_ulong dl_calling_addr_length;	/* length of calling address */
	dl_ulong dl_calling_addr_offset;	/* offset from start of block */
	dl_ulong dl_qos_length;		/* length of qos structure */
	dl_ulong dl_qos_offset;		/* offset from start of block */
	dl_ulong dl_growth;		/* set to zero */
} dl_connect_ind_t;

/* 
   DL_CONNECT_RES, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_CONNECT_RES */
	dl_ulong dl_correlation;	/* provider's correlation token */
	dl_ulong dl_resp_token;		/* token of responding stream */
	dl_ulong dl_qos_length;		/* length of qos structure */
	dl_ulong dl_qos_offset;		/* offset from start of block */
	dl_ulong dl_growth;		/* set to zero */
} dl_connect_res_t;

/* 
   DL_CONNECT_CON, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_CONNECT_CON */
	dl_ulong dl_resp_addr_length;	/* responder's address len */
	dl_ulong dl_resp_addr_offset;	/* offset from start of block */
	dl_ulong dl_qos_length;		/* length of qos structure */
	dl_ulong dl_qos_offset;		/* offset from start of block */
	dl_ulong dl_growth;		/* set to zero */
} dl_connect_con_t;

/* 
   DL_TOKEN_REQ, M_PCPROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_TOKEN_REQ */
} dl_token_req_t;

/* 
   DL_TOKEN_ACK, M_PCPROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_TOKEN_ACK */
	dl_ulong dl_token;		/* Connection response token */
} dl_token_ack_t;

/* 
   DL_DISCONNECT_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_DISCONNECT_REQ */
	dl_ulong dl_reason;		/* norm., abnorm., perm. or trans. */
	dl_ulong dl_correlation;	/* association with connect_ind */
} dl_disconnect_req_t;

/* 
   DL_DISCONNECT_IND, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_DISCONNECT_IND */
	dl_ulong dl_originator;		/* USER or PROVIDER */
	dl_ulong dl_reason;		/* permanent or transient */
	dl_ulong dl_correlation;	/* association with connect_ind */
} dl_disconnect_ind_t;

/* 
   DL_RESET_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_RESET_REQ */
} dl_reset_req_t;

/* 
   DL_RESET_IND, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_RESET_IND */
	dl_ulong dl_originator;		/* Provider or User */
	dl_ulong dl_reason;		/* flow control, link error, resync */
} dl_reset_ind_t;

/* 
   DL_RESET_RES, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_RESET_RES */
} dl_reset_res_t;

/* 
   DL_RESET_CON, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_RESET_CON */
} dl_reset_con_t;

/* 
   CONNECTIONLESS SERVICE PRIMITIVES
 */

/* 
   DL_UNITDATA_REQ, M_PROTO type, with M_DATA block(s)
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_UNITDATA_REQ */
	dl_ulong dl_dest_addr_length;	/* DLSAP length of dest. user */
	dl_ulong dl_dest_addr_offset;	/* offset from start of block */
	dl_priority_t dl_priority;	/* priority value */
} dl_unitdata_req_t;

/* 
   DL_UNITDATA_IND, M_PROTO type, with M_DATA block(s)
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_UNITDATA_IND */
	dl_ulong dl_dest_addr_length;	/* DLSAP length of dest. user */
	dl_ulong dl_dest_addr_offset;	/* offset from start of block */
	dl_ulong dl_src_addr_length;	/* DLSAP addr length sender */
	dl_ulong dl_src_addr_offset;	/* offset from start of block */
	dl_ulong dl_group_address;	/* one if multicast/broadcast */
} dl_unitdata_ind_t;

/* 
   DL_UDERROR_IND, M_PROTO type (or M_PCPROTO type if LLI-based provider)
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_UDERROR_IND */
	dl_ulong dl_dest_addr_length;	/* Destination DLSAP */
	dl_ulong dl_dest_addr_offset;	/* Offset from start of block */
	dl_ulong dl_unix_errno;		/* unix system error code */
	dl_ulong dl_errno;		/* DLPI error code */
} dl_uderror_ind_t;

/* 
   DL_UDQOS_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_UDQOS_REQ */
	dl_ulong dl_qos_length;		/* requested qos byte length */
	dl_ulong dl_qos_offset;		/* offset from start of block */
} dl_udqos_req_t;

/* 
   Primitives to handle XID and TEST operations
 */

/* 
   DL_TEST_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_TEST_REQ */
	dl_ulong dl_flag;		/* poll/final */
	dl_ulong dl_dest_addr_length;	/* DLSAP length of dest. user */
	dl_ulong dl_dest_addr_offset;	/* offset from start of block */
} dl_test_req_t;

/* 
   DL_TEST_IND, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_TEST_IND */
	dl_ulong dl_flag;		/* poll/final */
	dl_ulong dl_dest_addr_length;	/* dlsap length of dest. user */
	dl_ulong dl_dest_addr_offset;	/* offset from start of block */
	dl_ulong dl_src_addr_length;	/* dlsap length of source */
	dl_ulong dl_src_addr_offset;	/* offset from start of block */
} dl_test_ind_t;

/* 
   DL_TEST_RES, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_TEST_RES */
	dl_ulong dl_flag;		/* poll/final */
	dl_ulong dl_dest_addr_length;	/* DLSAP length of dest. user */
	dl_ulong dl_dest_addr_offset;	/* offset from start of block */
} dl_test_res_t;

/* 
   DL_TEST_CON, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_TEST_CON */
	dl_ulong dl_flag;		/* poll/final */
	dl_ulong dl_dest_addr_length;	/* dlsap length of dest. user */
	dl_ulong dl_dest_addr_offset;	/* offset from start of block */
	dl_ulong dl_src_addr_length;	/* dlsap length of source */
	dl_ulong dl_src_addr_offset;	/* offset from start of block */
} dl_test_con_t;

/* 
   DL_XID_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_XID_REQ */
	dl_ulong dl_flag;		/* poll/final */
	dl_ulong dl_dest_addr_length;	/* dlsap length of dest. user */
	dl_ulong dl_dest_addr_offset;	/* offset from start of block */
} dl_xid_req_t;

/* 
   DL_XID_IND, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_XID_IND */
	dl_ulong dl_flag;		/* poll/final */
	dl_ulong dl_dest_addr_length;	/* dlsap length of dest. user */
	dl_ulong dl_dest_addr_offset;	/* offset from start of block */
	dl_ulong dl_src_addr_length;	/* dlsap length of source */
	dl_ulong dl_src_addr_offset;	/* offset from start of block */
} dl_xid_ind_t;

/* 
   DL_XID_RES, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_XID_RES */
	dl_ulong dl_flag;		/* poll/final */
	dl_ulong dl_dest_addr_length;	/* DLSAP length of dest. user */
	dl_ulong dl_dest_addr_offset;	/* offset from start of block */
} dl_xid_res_t;

/* 
   DL_XID_CON, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_XID_CON */
	dl_ulong dl_flag;		/* poll/final */
	dl_ulong dl_dest_addr_length;	/* dlsap length of dest. user */
	dl_ulong dl_dest_addr_offset;	/* offset from start of block */
	dl_ulong dl_src_addr_length;	/* dlsap length of source */
	dl_ulong dl_src_addr_offset;	/* offset from start of block */
} dl_xid_con_t;

/* 
   ACKNOWLEDGED CONNECTIONLESS SERVICE PRIMITIVES
 */

/* 
   DL_DATA_ACK_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_DATA_ACK_REQ */
	dl_ulong dl_correlation;	/* User's correlation token */
	dl_ulong dl_dest_addr_length;	/* length of destination addr */
	dl_ulong dl_dest_addr_offset;	/* offset from start of block */
	dl_ulong dl_src_addr_length;	/* length of source address */
	dl_ulong dl_src_addr_offset;	/* offset from start of block */
	dl_ulong dl_priority;		/* priority */
	dl_ulong dl_service_class;	/* DL_RQST_RSP|DL_RQST_NORSP */
} dl_data_ack_req_t;

/* 
   DL_DATA_ACK_IND, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_DATA_ACK_IND */
	dl_ulong dl_dest_addr_length;	/* length of destination addr */
	dl_ulong dl_dest_addr_offset;	/* offset from start of block */
	dl_ulong dl_src_addr_length;	/* length of source address */
	dl_ulong dl_src_addr_offset;	/* offset from start of block */
	dl_ulong dl_priority;		/* pri. for data unit transm. */
	dl_ulong dl_service_class;	/* DL_RQST_RSP|DL_RQST_NORSP */
} dl_data_ack_ind_t;

/* 
   DL_DATA_ACK_STATUS_IND, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_DATA_ACK_STATUS_IND */
	dl_ulong dl_correlation;	/* User's correlation token */
	dl_ulong dl_status;		/* success or failure of previous req */
} dl_data_ack_status_ind_t;

/* 
   DL_REPLY_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_REPLY_REQ */
	dl_ulong dl_correlation;	/* User's correlation token */
	dl_ulong dl_dest_addr_length;	/* destination address length */
	dl_ulong dl_dest_addr_offset;	/* offset from start of block */
	dl_ulong dl_src_addr_length;	/* source address length */
	dl_ulong dl_src_addr_offset;	/* offset from start of block */
	dl_ulong dl_priority;		/* pri for data unit trans. */
	dl_ulong dl_service_class;
} dl_reply_req_t;

/* 
   DL_REPLY_IND, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_REPLY_IND */
	dl_ulong dl_dest_addr_length;	/* destination address length */
	dl_ulong dl_dest_addr_offset;	/* offset from start of block */
	dl_ulong dl_src_addr_length;	/* length of source address */
	dl_ulong dl_src_addr_offset;	/* offset from start of block */
	dl_ulong dl_priority;		/* pri for data unit trans. */
	dl_ulong dl_service_class;	/* DL_RQST_RSP|DL_RQST_NORSP */
} dl_reply_ind_t;

/* 
   DL_REPLY_STATUS_IND, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_REPLY_STATUS_IND */
	dl_ulong dl_correlation;	/* User's correlation token */
	dl_ulong dl_status;		/* success or failure of previous req */
} dl_reply_status_ind_t;

/* 
   DL_REPLY_UPDATE_REQ, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_REPLY_UPDATE_REQ */
	dl_ulong dl_correlation;	/* user's correlation token */
	dl_ulong dl_src_addr_length;	/* length of source address */
	dl_ulong dl_src_addr_offset;	/* offset from start of block */
} dl_reply_update_req_t;

/* 
   DL_REPLY_UPDATE_STATUS_IND, M_PROTO type
 */
typedef struct {
	dl_ulong dl_primitive;		/* DL_REPLY_UPDATE_STATUS_IND */
	dl_ulong dl_correlation;	/* User's correlation token */
	dl_ulong dl_status;		/* success or failure of previous req */
} dl_reply_update_status_ind_t;

#ifdef _SUN_SOURCE
#include <sys/dlpi_sun.h>
#endif				/* _SUN_SOURCE */

#ifdef _HPUX_SOURCE
#include <sys/dlpi_ext.h>
#endif				/* _HPUX_SOURCE */

union DL_primitives {
	dl_ulong dl_primitive;
	dl_info_req_t info_req;
	dl_info_ack_t info_ack;
	dl_attach_req_t attach_req;
	dl_detach_req_t detach_req;
	dl_bind_req_t bind_req;
	dl_bind_ack_t bind_ack;
	dl_unbind_req_t unbind_req;
	dl_subs_bind_req_t subs_bind_req;
	dl_subs_bind_ack_t subs_bind_ack;
	dl_subs_unbind_req_t subs_unbind_req;
	dl_ok_ack_t ok_ack;
	dl_error_ack_t error_ack;
	dl_connect_req_t connect_req;
	dl_connect_ind_t connect_ind;
	dl_connect_res_t connect_res;
	dl_connect_con_t connect_con;
	dl_token_req_t token_req;
	dl_token_ack_t token_ack;
	dl_disconnect_req_t disconnect_req;
	dl_disconnect_ind_t disconnect_ind;
	dl_reset_req_t reset_req;
	dl_reset_ind_t reset_ind;
	dl_reset_res_t reset_res;
	dl_reset_con_t reset_con;
	dl_unitdata_req_t unitdata_req;
	dl_unitdata_ind_t unitdata_ind;
	dl_uderror_ind_t uderror_ind;
	dl_udqos_req_t udqos_req;
	dl_enabmulti_req_t enabmulti_req;
	dl_disabmulti_req_t disabmulti_req;
	dl_promiscon_req_t promiscon_req;
	dl_promiscoff_req_t promiscoff_req;
	dl_phys_addr_req_t phys_addr_req;
	dl_phys_addr_ack_t phys_addr_ack;
	dl_set_phys_addr_req_t set_phys_addr_req;
	dl_get_statistics_req_t get_statistics_req;
	dl_get_statistics_ack_t get_statistics_ack;
	dl_test_req_t test_req;
	dl_test_ind_t test_ind;
	dl_test_res_t test_res;
	dl_test_con_t test_con;
	dl_xid_req_t xid_req;
	dl_xid_ind_t xid_ind;
	dl_xid_res_t xid_res;
	dl_xid_con_t xid_con;
	dl_data_ack_req_t data_ack_req;
	dl_data_ack_ind_t data_ack_ind;
	dl_data_ack_status_ind_t data_ack_status_ind;
	dl_reply_req_t reply_req;
	dl_reply_ind_t reply_ind;
	dl_reply_status_ind_t reply_status_ind;
	dl_reply_update_req_t reply_update_req;
	dl_reply_update_status_ind_t reply_update_status_ind;
#ifdef _SUN_SOURCE
	dl_notify_req_t notify_req;
	dl_notify_ack_t notify_ack;
	dl_notify_ind_t notify_ind;
	dl_aggr_req_t aggr_req;
	dl_aggr_ind_t aggr_ind;
	dl_unaggr_req_t unaggr_req;
	dl_capability_req_t capabiltiy_req;
	dl_capability_ack_t capabiltiy_ack;
	dl_control_req_t control_req;
	dl_control_ack_t control_ack;
	dl_passive_req_t passive_req;
	dl_intr_mode_req_t intr_mod_req;
#endif					/* _SUN_SOURCE */
#ifdef _HPUX_SOURCE
	dl_hp_ppa_req_t ppa_req;
	dl_hp_ppa_ack_t ppa_ack;
	dl_hp_multicast_list_req_t multicast_list_req;
	dl_hp_multicast_list_ack_t multicast_list_ack;
	dl_hp_rawdata_req_t rawdata_req;
	dl_hp_rawdata_ind_t rawdata_ind;
	dl_hp_hw_reset_req_t hw_reset_req;
	dl_hp_info_req_t hp_info_req;
	dl_hp_info_ack_t hp_info_ack;
	dl_hp_set_ack_to_req_t set_ack_to_req;
	dl_hp_set_p_to_req_t set_p_to_req;
	dl_hp_set_rej_to_req_t set_rej_to_req;
	dl_hp_set_busy_to_req_t set_busy_to_req;
	dl_hp_set_send_ack_to_req_t set_send_ack_to_req;
	dl_hp_set_max_retries_req_t set_max_retries_req;
	dl_hp_set_ack_threshold_req_t set_ack_threshold_req;
	dl_hp_set_local_win_req_t set_local_win_req;
	dl_hp_set_remote_win_req_t set_remote_win_req;
	dl_hp_clear_stats_req_t clear_stats_req;
	dl_hp_set_local_busy_req_t set_local_busy_req;
	dl_hp_clear_local_busy_req_t clear_local_busy_req;
#endif					/* _HPUX_SOURCE */
};

#define DL_INFO_REQ_SIZE		sizeof(dl_info_req_t)
#define DL_INFO_ACK_SIZE		sizeof(dl_info_ack_t)
#define DL_ATTACH_REQ_SIZE		sizeof(dl_attach_req_t)
#define DL_DETACH_REQ_SIZE		sizeof(dl_detach_req_t)
#define DL_BIND_REQ_SIZE		sizeof(dl_bind_req_t)
#define DL_BIND_ACK_SIZE		sizeof(dl_bind_ack_t)
#define DL_UNBIND_REQ_SIZE		sizeof(dl_unbind_req_t)
#define DL_SUBS_BIND_REQ_SIZE		sizeof(dl_subs_bind_req_t)
#define DL_SUBS_BIND_ACK_SIZE		sizeof(dl_subs_bind_ack_t)
#define DL_SUBS_UNBIND_REQ_SIZE		sizeof(dl_subs_unbind_req_t)
#define DL_OK_ACK_SIZE			sizeof(dl_ok_ack_t)
#define DL_ERROR_ACK_SIZE		sizeof(dl_error_ack_t)
#define DL_CONNECT_REQ_SIZE		sizeof(dl_connect_req_t)
#define DL_CONNECT_IND_SIZE		sizeof(dl_connect_ind_t)
#define DL_CONNECT_RES_SIZE		sizeof(dl_connect_res_t)
#define DL_CONNECT_CON_SIZE		sizeof(dl_connect_con_t)
#define DL_TOKEN_REQ_SIZE		sizeof(dl_token_req_t)
#define DL_TOKEN_ACK_SIZE		sizeof(dl_token_ack_t)
#define DL_DISCONNECT_REQ_SIZE		sizeof(dl_disconnect_req_t)
#define DL_DISCONNECT_IND_SIZE		sizeof(dl_disconnect_ind_t)
#define DL_RESET_REQ_SIZE		sizeof(dl_reset_req_t)
#define DL_RESET_IND_SIZE		sizeof(dl_reset_ind_t)
#define DL_RESET_RES_SIZE		sizeof(dl_reset_res_t)
#define DL_RESET_CON_SIZE		sizeof(dl_reset_con_t)
#define DL_UNITDATA_REQ_SIZE		sizeof(dl_unitdata_req_t)
#define DL_UNITDATA_IND_SIZE		sizeof(dl_unitdata_ind_t)
#define DL_UDERROR_IND_SIZE		sizeof(dl_uderror_ind_t)
#define DL_UDQOS_REQ_SIZE		sizeof(dl_udqos_req_t)
#define DL_ENABMULTI_REQ_SIZE		sizeof(dl_enabmulti_req_t)
#define DL_DISABMULTI_REQ_SIZE		sizeof(dl_disabmulti_req_t)
#define DL_PROMISCON_REQ_SIZE		sizeof(dl_promiscon_req_t)
#define DL_PROMISCOFF_REQ_SIZE		sizeof(dl_promiscoff_req_t)
#define DL_PHYS_ADDR_REQ_SIZE		sizeof(dl_phys_addr_req_t)
#define DL_PHYS_ADDR_ACK_SIZE		sizeof(dl_phys_addr_ack_t)
#define DL_SET_PHYS_ADDR_REQ_SIZE	sizeof(dl_set_phys_addr_req_t)
#define DL_GET_STATISTICS_REQ_SIZE	sizeof(dl_get_statistics_req_t)
#define DL_GET_STATISTICS_ACK_SIZE	sizeof(dl_get_statistics_ack_t)
#define DL_XID_REQ_SIZE			sizeof(dl_xid_req_t)
#define DL_XID_IND_SIZE			sizeof(dl_xid_ind_t)
#define DL_XID_RES_SIZE			sizeof(dl_xid_res_t)
#define DL_XID_CON_SIZE			sizeof(dl_xid_con_t)
#define DL_TEST_REQ_SIZE		sizeof(dl_test_req_t)
#define DL_TEST_IND_SIZE		sizeof(dl_test_ind_t)
#define DL_TEST_RES_SIZE		sizeof(dl_test_res_t)
#define DL_TEST_CON_SIZE		sizeof(dl_test_con_t)
#define DL_DATA_ACK_REQ_SIZE		sizeof(dl_data_ack_req_t)
#define DL_DATA_ACK_IND_SIZE		sizeof(dl_data_ack_ind_t)
#define DL_DATA_ACK_STATUS_IND_SIZE	sizeof(dl_data_ack_status_ind_t)
#define DL_REPLY_REQ_SIZE		sizeof(dl_reply_req_t)
#define DL_REPLY_IND_SIZE		sizeof(dl_reply_ind_t)
#define DL_REPLY_STATUS_IND_SIZE	sizeof(dl_reply_status_ind_t)
#define DL_REPLY_UPDATE_REQ_SIZE	sizeof(dl_reply_update_req_t)
#define DL_REPLY_UPDATE_STATUS_IND_SIZE	sizeof(dl_reply_update_status_ind_t)

#ifdef _SUN_SOURCE

#define DL_NOTIFY_REQ_SIZE		sizeof(dl_notify_req_t)
#define DL_NOTIFY_ACK_SIZE		sizeof(dl_notify_ack_t)
#define DL_NOTIFY_IND_SIZE		sizeof(dl_notify_ind_t)
#define DL_AGGR_REQ_SIZE		sizeof(dl_aggr_req_t)
#define DL_AGGR_IND_SIZE		sizeof(dl_aggr_ind_t)
#define DL_UNAGGR_REQ_SIZE		sizeof(dl_unaggr_req_t)
#define DL_CAPABILITY_REQ_SIZE		sizeof(dl_capability_req_t)
#define DL_CAPABILITY_ACK_SIZE		sizeof(dl_capability_ack_t)
#define DL_CONTROL_REQ_SIZE		sizeof(dl_control_req_t)
#define DL_CONTROL_ACK_SIZE		sizeof(dl_control_ack_t)
#define DL_PASSIVE_REQ_SIZE		sizeof(dl_passive_req_t)
#define DL_INTR_MODE_REQ_SIZE		sizeof(dl_intr_mode_req_t)

#endif				/* _SUN_SOURCE */

#ifdef _HPUX_SOURCE

#define DL_HP_PPA_REQ_SIZE		sizeof(dl_hp_ppa_req_t)
#define DL_HP_PPA_ACK_SIZE		sizeof(dl_hp_ppa_ack_t)
#define DL_HP_MULTICAST_LIST_REQ_SIZE	sizeof(dl_hp_multicast_list_req_t)
#define DL_HP_MULTICAST_LIST_ACK_SIZE	sizeof(dl_hp_multicast_list_ack_t)
#define DL_HP_RAWDATA_REQ_SIZE		sizeof(dl_hp_rawdata_req_t)
#define DL_HP_RAWDATA_IND_SIZE		sizeof(dl_hp_rawdata_ind_t)
#define DL_HP_HW_RESET_REQ_SIZE		sizeof(dl_hp_hw_reset_req_t)
#define DL_HP_INFO_REQ_SIZE		sizeof(dl_hp_info_req_t)
#define DL_HP_INFO_ACK_SIZE		sizeof(dl_hp_info_ack_t)
#define DL_HP_SET_ACK_TO_REQ_SIZE	sizeof(dl_hp_set_ack_to_req_t)
#define DL_HP_SET_P_TO_REQ_SIZE		sizeof(dl_hp_set_p_to_req_t)
#define DL_HP_SET_REJ_TO_REQ_SIZE	sizeof(dl_hp_set_rej_to_req_t)
#define DL_HP_SET_BUSY_TO_REQ_SIZE	sizeof(dl_hp_set_busy_to_req_t)
#define DL_HP_SET_SEND_ACK_TO_REQ_SIZE	sizeof(dl_hp_set_send_ack_to_req_t)
#define DL_HP_SET_MAX_RETRIES_REQ_SIZE	sizeof(dl_hp_set_max_retries_req_t)
#define DL_HP_SET_ACK_THRESHOLD_REQ_SIZE sizeof(dl_hp_set_ack_threshold_req_t)
#define DL_HP_SET_LOCAL_WIN_REQ_SIZE	sizeof(dl_hp_set_local_win_req_t)
#define DL_HP_SET_REMOTE_WIN_REQ_SIZE	sizeof(dl_hp_set_remote_win_req_t)
#define DL_HP_CLEAR_STATS_REQ_SIZE	sizeof(dl_hp_clear_stats_req_t)
#define DL_HP_SET_LOCAL_BUSY_REQ_SIZE	sizeof(dl_hp_set_local_busy_req_t)
#define DL_HP_CLEAR_LOCAL_BUSY_REQ_SIZE	sizeof(dl_hp_clear_local_busy_req_t)

#endif				/* _HPUX_SOURCE */

#endif				/* _SYS_DLPI_H */
