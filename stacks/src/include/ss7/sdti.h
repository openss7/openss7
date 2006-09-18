/*****************************************************************************

 @(#) $Id: sdti.h,v 0.9.2.3 2006/09/18 13:52:34 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

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

 Last Modified $Date: 2006/09/18 13:52:34 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SS7_SDTI_H__
#define __SS7_SDTI_H__

#ident "@(#) $RCSfile: sdti.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2004  OpenSS7 Corporation"

/* This file can be processed by doxygen(1). */

 /*
  * The purpose of the SDT interface is to provide a separation between
  * the SL (Signalling Link) interface which provides SS7 Level 2 (LINK)
  * state machine services and the underlying driver which provides
  * essentially HDLC capablities.  In SS7 the entity providing HDLC
  * services is called the Signalling Data Terminal (SDT).  An SDTI
  * implements the AERM/SUERM/EIM and DAEDR/DAEDT capabilities and
  * communicates upstream to the Signalling Link using the primitives
  * provided here.
  *
  * The SDT interface also recognizes Local Management Interface (LMI)
  * primitives defined elsewhere <sys/ss7/lmi.h>.
  */

typedef lmi_long sdt_long;
typedef lmi_ulong sdt_ulong;
typedef lmi_ushort sdt_ushort;
typedef lmi_uchar sdt_uchar;

#define SDT_PROTO_BASE				 48L

#define SDT_DSTR_FIRST				( 1L + SDT_PROTO_BASE)
#define SDT_DAEDT_TRANSMISSION_REQ		( 1L + SDT_PROTO_BASE)
#define SDT_DAEDT_START_REQ			( 2L + SDT_PROTO_BASE)
#define SDT_DAEDR_START_REQ			( 3L + SDT_PROTO_BASE)
#define SDT_AERM_START_REQ			( 4L + SDT_PROTO_BASE)
#define SDT_AERM_STOP_REQ			( 5L + SDT_PROTO_BASE)
#define SDT_AERM_SET_TI_TO_TIN_REQ		( 6L + SDT_PROTO_BASE)
#define SDT_AERM_SET_TI_TO_TIE_REQ		( 7L + SDT_PROTO_BASE)
#define SDT_SUERM_START_REQ			( 8L + SDT_PROTO_BASE)
#define SDT_SUERM_STOP_REQ			( 9L + SDT_PROTO_BASE)
#define SDT_DSTR_LAST				( 9L + SDT_PROTO_BASE)

#define SDT_USTR_LAST				(-1L - SDT_PROTO_BASE)
#define SDT_RC_SIGNAL_UNIT_IND			(-1L - SDT_PROTO_BASE)
#define SDT_RC_CONGESTION_ACCEPT_IND		(-2L - SDT_PROTO_BASE)
#define SDT_RC_CONGESTION_DISCARD_IND		(-3L - SDT_PROTO_BASE)
#define SDT_RC_NO_CONGESTION_IND		(-4L - SDT_PROTO_BASE)
#define SDT_IAC_CORRECT_SU_IND			(-5L - SDT_PROTO_BASE)
#define SDT_IAC_ABORT_PROVING_IND		(-6L - SDT_PROTO_BASE)
#define SDT_LSC_LINK_FAILURE_IND		(-7L - SDT_PROTO_BASE)
#define SDT_TXC_TRANSMISSION_REQUEST_IND	(-8L - SDT_PROTO_BASE)
#define SDT_USTR_FIRST				(-8L - SDT_PROTO_BASE)

/*
 *  SDT_RC_SIGNAL_UNIT_IND, M_DATA or M_PROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_RC_SIGNAL_UNIT_IND */
	sdt_ulong sdt_count;
} sdt_rc_signal_unit_ind_t;

/*
 *  SDT_DAEDT_TRANSMISSION_REQ, M_DATA or M_PROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_DAEDT_TRANSMISSION_REQ */
} sdt_daedt_transmission_req_t;

/*
 *  SDT_DAEDT_START_REQ, M_PROTO or M_PCPROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_DAEDT_START_REQ */
} sdt_daedt_start_req_t;

/*
 *  SDT_DAEDR_START_REQ, M_PROTO or M_PCPROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_DAEDR_START_REQ */
} sdt_daedr_start_req_t;

/*
 *  SDT_IAC_CORRECT_SU_IND, M_PROTO or M_PCPROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_IAC_CORRECT_SU_IND */
} sdt_iac_correct_su_ind_t;

/*
 *  SDT_AERM_START_REQ, M_PROTO or M_PCPROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_AERM_START_REQ */
} sdt_aerm_start_req_t;

/*
 *  SDT_AERM_STOP_REQ, M_PROTO or M_PCPROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_AERM_STOP_REQ */
} sdt_aerm_stop_req_t;

/*
 *  SDT_AERM_SET_TI_TO_TIN_REQ, M_PROTO or M_PCPROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_AERM_SET_TI_TO_TIN_REQ */
} sdt_aerm_set_ti_to_tin_req_t;

/*
 *  SDT_AERM_SET_TI_TO_TIE_REQ, M_PROTO or M_PCPROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_AERM_SET_TI_TO_TIE_REQ */
} sdt_aerm_set_ti_to_tie_req_t;

/*
 *  SDT_IAC_ABORT_PROVING_IND, M_PROTO or M_PCPROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_IAC_ABORT_PROVING_IND */
} sdt_iac_abort_proving_ind_t;

/*
 *  SDT_SUERM_START_REQ, M_PROTO or M_PCPROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_SUERM_START_REQ */
} sdt_suerm_start_req_t;

/*
 *  SDT_SUERM_STOP_REQ, M_PROTO or M_PCPROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_SUERM_STOP_REQ */
} sdt_suerm_stop_req_t;

/*
 *  SDT_LSC_LINK_FAILURE_IND, M_PROTO or M_PCPROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_LSC_LINK_FAILURE_IND */
} sdt_lsc_link_failure_ind_t;

/*
 *  SDT_RC_CONGESTION_ACCEPT_IND, M_PROTO or M_PCPROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_RC_CONGESTION_ACCEPT_IND */
} sdt_rc_congestion_accept_ind_t;

/*
 *  SDT_RC_CONGESTION_DISCARD_IND, M_PROTO or M_PCPROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_RC_CONGESTION_DISCARD_IND */
} sdt_rc_congestion_discard_ind_t;

/*
 *  SDT_RC_NO_CONGESTION_IND, M_PROTO or M_PCPROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_RC_NO_CONGESTION_IND */
} sdt_rc_no_congestion_ind_t;

/*
 *  SDT_TXC_TRANSMISSION_REQUEST_IND, M_PROTO or M_PCPROTO
 */
typedef struct {
	sdt_long sdt_primitive;		/* SDT_TXC_TRANSMISSION_REQUEST_IND */
} sdt_txc_transmission_request_ind_t;

union SDT_primitives {
	sdt_long sdt_primitive;
	sdt_daedt_transmission_req_t daedt_transmission_req;
	sdt_daedt_start_req_t daedt_start_req;
	sdt_daedr_start_req_t daedr_start_req;
	sdt_aerm_start_req_t aerm_start_req;
	sdt_aerm_stop_req_t aerm_stop_req;
	sdt_aerm_set_ti_to_tin_req_t aerm_set_ti_to_tin_req;
	sdt_aerm_set_ti_to_tie_req_t aerm_set_ti_to_tie_req;
	sdt_suerm_start_req_t suerm_start_req;
	sdt_suerm_stop_req_t suerm_stop_req;
	sdt_rc_signal_unit_ind_t rc_signal_unit_ind;
	sdt_rc_congestion_accept_ind_t rc_congestion_accept_ind;
	sdt_rc_congestion_discard_ind_t rc_congestion_discard_ind;
	sdt_rc_no_congestion_ind_t rc_no_congestion_ind;
	sdt_iac_correct_su_ind_t iac_correct_su_ind;
	sdt_iac_abort_proving_ind_t iac_abort_proving_ind;
	sdt_lsc_link_failure_ind_t lsc_link_failure_ind;
	sdt_txc_transmission_request_ind_t txc_transmission_request_ind;
};

#define SDT_DAEDT_TRANSMISSION_REQ_SIZE		sizeof(sdt_daedt_transmission_req_t)
#define SDT_DAEDR_START_REQ_SIZE		sizeof(sdt_daedr_start_req_t)
#define SDT_DAEDT_START_REQ_SIZE		sizeof(sdt_daedt_start_req_t)
#define SDT_AERM_START_REQ_SIZE			sizeof(sdt_aerm_start_req_t)
#define SDT_AERM_STOP_REQ_SIZE			sizeof(sdt_aerm_stop_req_t)
#define SDT_AERM_SET_TI_TO_TIN_REQ_SIZE		sizeof(sdt_aerm_set_ti_to_tin_req_t)
#define SDT_AERM_SET_TI_TO_TIE_REQ_SIZE		sizeof(sdt_aerm_set_ti_to_tie_req_t)
#define SDT_SUERM_START_REQ_SIZE		sizeof(sdt_suerm_start_req_t)
#define SDT_SUERM_STOP_REQ_SIZE			sizeof(sdt_suerm_stop_req_t)
#define SDT_RC_SIGNAL_UNIT_IND_SIZE		sizeof(sdt_rc_signal_unit_ind_t)
#define SDT_RC_CONGESTION_ACCEPT_IND_SIZE	sizeof(sdt_rc_congestion_accept_ind_t)
#define SDT_RC_CONGESTION_DISCARD_IND_SIZE	sizeof(sdt_rc_congestion_discard_ind_t)
#define SDT_RC_NO_CONGESTION_IND_SIZE		sizeof(sdt_rc_no_congestion_ind_t)
#define SDT_IAC_CORRECT_SU_IND_SIZE		sizeof(sdt_iac_correct_su_ind_t)
#define SDT_IAC_ABORT_PROVING_IND_SIZE		sizeof(sdt_iac_abort_proving_ind_t)
#define SDT_LSC_LINK_FAILURE_IND_SIZE		sizeof(sdt_lsc_link_failure_ind_t)
#define SDT_TXC_TRANSMISSION_REQUEST_IND_SIZE	sizeof(sdt_txc_transmission_request_ind_t)

#endif				/* __SS7_SDTI_H__ */
