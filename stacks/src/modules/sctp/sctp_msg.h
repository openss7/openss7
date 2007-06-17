/*****************************************************************************

 @(#) $Id: sctp_msg.h,v 0.9.2.4 2007/06/17 01:56:20 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2007/06/17 01:56:20 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sctp_msg.h,v $
 Revision 0.9.2.4  2007/06/17 01:56:20  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __SCTP_MSG_H__
#define __SCTP_MSG_H__

#ident "@(#) $RCSfile: sctp_msg.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 *  =========================================================================
 *
 *  SCTP Provider --> SCTP User Interface Primitives
 *
 *  =========================================================================
 *
 *  There must be defined by the specific SCTP User and linked.
 *
 *  CONN_IND:   provides indication of a incoming connection from the peer and
 *              provides a pointer to the connection indication structure
 *              which contains information from the verified cookie in the
 *              COOKIE-ECHO message.  The interface should indicate the
 *              connection to its user and buffer the indication as required.
 *              A return value of non-zero indicates that the interface could
 *              not buffer the connection indication.  Any data received in the
 *              COOKIE-ECHO message will be separately indicated with the
 *              DATA_IND primitive.
 *
 *  CONN_CON:   provides confirmation that the connection has been
 *              established.This is called when a COOKIE-ACK has been
 *              successfully received.  Any data received with the SCTP
 *              message that contained the COOKIE-ACK will be separately
 *              indicated with the data indication primitive.
 *
 *  DATA_IND:   provides indication of data that was received in sequence for
 *              the specified message control block parameters.  The message
 *              control block parameters of interest are PPI, SID, and SSN.
 *              Also, the MORE argument indicates that there are further data
 *              indications that make up the same data record.  The M_DATA
 *              block provided contains a message control block and the data
 *              payload.
 *
 *  DATACK_IND: provides indication that the message with the given message
 *              control block parameters was received in entirety and
 *              positively acknowledged by the peer.  Message control block
 *              parameters of interest include SID and SSN.  Also of interest
 *              might be the per chunk statistics present in the control block
 *              (how many times sent, delay) This only applies to ordered data
 *              unless a token was provided on write.  The M_DATA block
 *              provided contains only a message control block and no data.
 *
 *  RESET_IND:  provids indication that the association has been reset as a
 *              result of an association restart condition.  It also means
 *              that any unacknowledged transmit data will be discarded and
 *              that stream sequence numbers are being reset.
 *
 *  DISCON_IND: provides indication that either an outstanding connection
 *              indication or an existing connection has been abotively
 *              disconnected for the provided reason.  When the CP argument is
 *              included, it identifies the connection indication being
 *              disconnected.  When the CP argument is NULL, the stream to
 *              which the DISCON_IND is sent is the one being disconnected.
 *              In SCTP no data is ever associated with an DISCON_IND.
 *
 *  ORDREL_IND: provides indication that an existing connection has received a
 *              SHUTDOWN or SHUTDOWN-ACK from the peer.
 *
 *  RETR_IND:   provides indication of a retrieved message with the given
 *              message control block parameters.  Message control block
 *              parameters of interest are SID and SSN, STATUS and statistics
 *              associated with the data.  The M_DATA block provided contains
 *              a message control block and the data payload.
 *
 *  RETR_CON:   provides confirmation that all unsent and unacked data has
 *              been retrieved.
 *
 */

struct sctp_ifops {
	int (*sctp_conn_ind) (sctp_t * sp, mblk_t *cp);
	int (*sctp_conn_con) (sctp_t * sp);
	int (*sctp_data_ind) (sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn, uint32_t tsn,
			      uint ord, uint more, mblk_t *dp);
	int (*sctp_datack_ind) (sctp_t * sp, uint32_t ppi, uint16_t sid, uint16_t ssn,
				uint32_t tsn);
	int (*sctp_reset_ind) (sctp_t * sp, ulong orig, ulong reason, mblk_t *cp);
	int (*sctp_reset_con) (sctp_t * sp);
	int (*sctp_discon_ind) (sctp_t * sp, ulong orig, long reason, mblk_t *cp);
	int (*sctp_ordrel_ind) (sctp_t * sp);
	int (*sctp_retr_ind) (sctp_t * sp, mblk_t *dp);
	int (*sctp_retr_con) (sctp_t * sp);
};

/*
 *  =========================================================================
 *
 *  SCTP User Interface --> SCTP Provider Primitives
 *
 *  =========================================================================
 *
 *  BIND_REQ:   requests that the provider bind the stream to an SCTP port and address
 *              list and set the requested number of connection indications.  The provider
 *              allocates any necessary resources for binding the stream.
 *
 *  CONN_REQ:   requests that the provider initiate an association with the provided data
 *              (if any).  This will initiate an association by starting the
 *              INIT/INIT-ACK/COOKIE-ECHO/COOKIE-ACK procedure.  The user will either
 *              receive and error number in return, or will receive indication via the
 *              DISCON_IND or CONN_CON callbacks that the connection has failed or was
 *              successful.
 *
 *  CONN_RES:   responds to a previous connection indication from the SCTP provider.  The
 *              connection response includes the conndication indication that was earlier
 *              provided and a pointer to the accepting stream  the accepting stream may
 *              be the same as the responding stream.  Any data provided will be bundled
 *              with the COOKIE-ACK.
 *
 *  DATA_REQ:   requests that the provided data with the provide message control block
 *              parameters be sent on the connection.  Message control block parameters of
 *              interest are PPI, SID, ordered flag, receipt confirmation flag.  The
 *              M_DATA block provided contains a message control block and the data
 *              payload.
 *
 *  DATACK_REQ: requests that the provider acknowledge receipt of a previous DATA_IND with
 *              the given transmit sequence number.  (This will also acknowledge any
 *              previous data fragments that made up the data record that ended with this
 *              TSN.)
 *
 *  RESET_CON:  used to acknowledge a previous reset indication.  This sends a COOKIE ACK
 *              in an SCTP restart scenario.
 *
 *  DISCON_REQ: requests that either the the requesting stream be aborted or that the
 *              connection indication provided by rejected.  This results in an ABORT
 *              being sent.
 *
 *  ORDREL_REQ: request that the SCTP provider accept no more data for transmission on the
 *              connection and inform the other side that it is finished sending data.
 *              This results in a SHUTDOWN or SHUTDOWN-ACK being sent.
 *
 *  UNBIND_REQ: requests that the provider unbind the stream in preparation for having the
 *              stream closed.  The provider frees any and all resources associated with
 *              the stream regardless of what s_state the stream is in.
 *
 */
extern int sctp_bind_req(sctp_t * sp, uint16_t sport, uint32_t *sptr, size_t snum, ulong cons);
extern int sctp_conn_req(sctp_t * sp, uint16_t dport, uint32_t *dptr, size_t dnum, mblk_t *dp);
extern int sctp_conn_res(sctp_t * sp, mblk_t *cp, sctp_t * ap, mblk_t *dp);
extern int sctp_data_req(sctp_t * sp, uint32_t ppi, uint16_t sid, uint ord, uint more, uint rctp,
			 mblk_t *dp);
extern int sctp_reset_req(sctp_t * sp);
extern int sctp_reset_res(sctp_t * sp);
extern int sctp_discon_req(sctp_t * sp, mblk_t *cp);
extern int sctp_ordrel_req(sctp_t * sp);
extern int sctp_unbind_req(sctp_t * sp);

/*
 *  =========================================================================
 *
 *  SCTP Peer --> SCTP Primitives (Receive Messages)
 *
 *  =========================================================================
 */

extern void sctp_cleanup_read(sctp_t * sp);

extern int sctp_recv_msg(sctp_t * sp, mblk_t *mp);
extern int sctp_recv_err(sctp_t * sp, mblk_t *mp);

/*
 *  ==========================================================================
 *
 *  SCTP --> SCTP Peer Messages (Send Messages)
 *
 *  ==========================================================================
 */

extern void sctp_transmit_wakeup(sctp_t * sp);

extern void sctp_send_abort_ootb(uint32_t daddr, uint32_t saddr, struct sctphdr *sh);
extern void sctp_send_shutdown_complete_ootb(uint32_t daddr, uint32_t saddr, struct sctphdr *sh);

#endif				/* __SCTP_MSG_H__ */
