/*****************************************************************************

 @(#) $Id: slsi.h,v 0.9.2.2 2005/05/14 08:30:46 brian Exp $

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

 Last Modified $Date: 2005/05/14 08:30:46 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SS7_SLSI_H__
#define __SS7_SLSI_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2004  OpenSS7 Corporation"

/* This file can be processed by doxygen(1). */

typedef lmi_long ls_long;
typedef lmi_ulong ls_ulong;
typedef lmi_ushort ls_ushort;
typedef lmi_uchar ls_uchar;

#define LS_PROTO_BASE				   96L

#define LS_DSTR_FIRST				(   1L + LS_PROTO_BASE )
#define LS_MESSAGE_FOR_ROUTING_REQ		(   1L + LS_PROTO_BASE )
#define LS_MESSAGE_FOR_DISTRIBUTION_REQ		(   2L + LS_PROTO_BASE )
#define LS_MESSAGE_FOR_REROUTING_REQ		(   3L + LS_PROTO_BASE )
#define LS_LINK_ACTIVATE_REQ			(   4L + LS_PROTO_BASE )
#define LS_LINK_DEACTIVATE_REQ			(   5L + LS_PROTO_BASE )
#define LS_LINK_INHIBIT_REQ			(   6L + LS_PROTO_BASE )
#define LS_LINK_UNINHIBIT_REQ			(   7L + LS_PROTO_BASE )
#define LS_ACTIVATE_REQ				(   8L + LS_PROTO_BASE )
#define LS_DEACTIVATE_REQ			(   9L + LS_PROTO_BASE )
#define LS_INHIBIT_REQ				(  10L + LS_PROTO_BASE )
#define LS_UNINHIBIT_REQ			(  11L + LS_PROTO_BASE )
#define LS_FORCE_UNINHIBIT_REQ			(  12L + LS_PROTO_BASE )
#define LS_ROUTING_OUTAGE_REQ			(  13L + LS_PROTO_BASE )
#define LS_ROUTING_RECOVERED_REQ		(  14L + LS_PROTO_BASE )
#define LS_CRITICAL_REQ				(  15L + LS_PROTO_BASE )
#define LS_NONCRITICAL_REQ			(  16L + LS_PROTO_BASE )
#define LS_EMERGENCY_REQ			(  17L + LS_PROTO_BASE )
#define LS_EMERGENCY_CEASES_REQ			(  18L + LS_PROTO_BASE )
#define LS_TRAFFIC_STARTS_REQ			(  19L + LS_PROTO_BASE )
#define LS_TRAFFIC_ENDS_REQ			(  20L + LS_PROTO_BASE )
#define LS_ADJACENT_SP_INACCESSIBLE_REQ		(  21L + LS_PROTO_BASE )
#define LS_ADJACENT_SP_ACCESSIBLE_REQ		(  22L + LS_PROTO_BASE )
#define LS_DSTR_LAST				(  22L + LS_PROTO_BASE )

#define LS_USTR_LAST				(  -1L - LS_PROTO_BASE )
#define LS_MESSAGE_FOR_ROUTING_IND		(  -1L - LS_PROTO_BASE )
#define LS_MESSAGE_FOR_DISCRIMINATION_IND	(  -2L - LS_PROTO_BASE )
#define LS_LINK_INHIBITED_IND			(  -3L - LS_PROTO_BASE )
#define LS_LINK_INHIBIT_DENIED_IND		(  -4L - LS_PROTO_BASE )
#define LS_LINK_INHIBIT_FAILED_IND		(  -5L - LS_PROTO_BASE )
#define LS_LINK_UNINHIBITED_IND			(  -6L - LS_PROTO_BASE )
#define LS_LINK_UNINHIBIT_FAILED_IND		(  -7L - LS_PROTO_BASE )
#define LS_FORCE_UNINHIBIT_FAILED_IND		(  -8L - LS_PROTO_BASE )
#define LS_AVAILABLE_IND			(  -9L - LS_PROTO_BASE )
#define LS_UNAVAILABLE_IND			( -10L - LS_PROTO_BASE )
#define LS_RETRIEVED_MESSAGE_IND		( -11L - LS_PROTO_BASE )
#define LS_RETRIEVAL_COMPLETE_IND		( -12L - LS_PROTO_BASE )
#define LS_CONGESTED_IND			( -13L - LS_PROTO_BASE )
#define LS_CONGESTION_CEASED_IND		( -14L - LS_PROTO_BASE )
#define LS_LINK_IN_SERVICE_AT_LEVEL_2_IND	( -15L - LS_PROTO_BASE )
#define LS_USTR_FIRST				( -15L - LS_PROTO_BASE )

#define LS_SIGNALS_MASK_H0	0x0010f00f
#define LS_SIGNALS_MASK_H1	0x000000f0

#define LS_SIGNAL_CHM		0x00000001
#define LS_SIGNAL_COO		0x00000011
#define LS_SIGNAL_COA		0x00000021
#define LS_SIGNAL_CBD		0x00000051
#define LS_SIGNAL_CBA		0x00000061

#define LS_SIGNAL_ECM		0x00000002
#define LS_SIGNAL_ECO		0x00000012
#define LS_SIGNAL_ECA		0x00000022

#define LS_SIGNAL_FCM		0x00000003
#define LS_SIGNAL_RCT		0x00000013
#define LS_SIGNAL_TFC		0x00000023

#define LS_SIGNAL_TFM		0x00000004
#define LS_SIGNAL_TFP		0x00000014
#define LS_SIGNAL_TCP		0x00000024
#define LS_SIGNAL_TFR		0x00000034
#define LS_SIGNAL_TCR		0x00000044
#define LS_SIGNAL_TFA		0x00000054
#define LS_SIGNAL_TCA		0x00000064

#define LS_SIGNAL_RSM		0x00000005
#define LS_SIGNAL_RSP		0x00000015
#define LS_SIGNAL_RSR		0x00000025
#define LS_SIGNAL_RCP		0x00000035
#define LS_SIGNAL_RCR		0x00000045

#define LS_SIGNAL_MIM		0x00000006
#define LS_SIGNAL_LIN		0x00000016
#define LS_SIGNAL_LUN		0x00000026
#define LS_SIGNAL_LIA		0x00000036
#define LS_SIGNAL_LUA		0x00000046
#define LS_SIGNAL_LID		0x00000056
#define LS_SIGNAL_LFU		0x00000066
#define LS_SIGNAL_LLI		0x00000076
#define LS_SIGNAL_LRI		0x00000086

#define LS_SIGNAL_TRM		0x00000007
#define LS_SIGNAL_TRA		0x00000017
#define LS_SIGNAL_TRW		0x00000027

#define LS_SIGNAL_DLM		0x00000008
#define LS_SIGNAL_DLC		0x00000018
#define LS_SIGNAL_CSS		0x00000028
#define LS_SIGNAL_CNS		0x00000038
#define LS_SIGNAL_CNP		0x00000048

#define LS_SIGNAL_UFC		0x0000000a
#define LS_SIGNAL_UPU		0x0000001a
#define LS_SIGNAL_UPA		0x0000002a
#define LS_SIGNAL_UPT		0x0000003a

#define LS_SIGNAL_SLTC		0x00001001
#define LS_SIGNAL_SLTM		0x00001011
#define LS_SIGNAL_SLTA		0x00001021

#define LS_SIGNAL_SSLTC		0x00002001
#define LS_SIGNAL_SSLTM		0x00002011
#define LS_SIGNAL_SSLTA		0x00002021

#define LS_SIGNAL_USER		0x00100000

typedef struct {
	ls_ulong dpc;			/* destination point code */
	ls_ulong opc;			/* originating point code */
	ls_ulong sls;			/* signalling link selection */
} ls_rl_t;

typedef struct {
	ls_uchar ni;			/* network indicator */
	ls_uchar mp;			/* message priority */
	ls_uchar si;			/* service indicator */
	ls_rl_t rl;			/* routing label */
} ls_mtph_t;

typedef struct {
	ls_uchar ni;			/* network indicator */
	ls_uchar mp;			/* message priority */
	ls_uchar si;			/* service indicator */
	ls_rl_t rl;			/* routing label */
	ls_uchar h0;			/* header 0 */
	ls_uchar h1;			/* header 1 */
} ls_mtpm_t;

/*
 */

/*
 *  Simple Primitives:
 *
 *  LS_ACTIVATE_REQ, M_PROTO
 *  LS_DEACTIVATE_REQ, M_PROTO
 *  LS_INHIBIT_REQ, M_PROTO
 *  LS_UNINHIBIT_REQ, M_PROTO
 *  LS_FORCE_UNINHIBIT_REQ, M_PCPROTO
 *  LS_ROUTING_OUTAGE_REQ, M_PCPROTO
 *  LS_ROUTING_RECOVERED_REQ, M_PCPROTO
 *  LS_CRITICAL_REQ, M_PCPROTO
 *  LS_NONCRITICAL_REQ, M_PCPROTO
 *  LS_EMERGENCY_REQ, M_PCPROTO
 *  LS_EMERGENCY_CEASES_REQ, M_PCPROTO
 *  LS_TRAFFIC_STARTS_REQ, M_PROTO
 *  LS_TRAFFIC_ENDS_REQ, M_PROTO
 *  LS_FORCE_UNINHIBIT_FAILED_IND, M_PCPROTO
 *  LS_AVAILABLE_IND, M_PCPROTO
 *  LS_UNAVAILABLE_IND, M_PCPROTO
 *  LS_RETRIEVAL_COMPLETE_IND, M_PROTO
 *  LS_LINK_IN_SERVICE_AT_LEVEL_2_IND, M_PCPROTO
 */

/*
 *  Link Primitives:
 *
 *  LS_LINK_ACTIVATE_REQ, M_PROTO
 *  LS_LINK_DEACTIVATE_REQ, M_PROTO
 *  LS_LINK_INHIBIT_REQ, M_PCPROTO
 *  LS_LINK_UNINHIBIT_REQ, M_PCPROTO
 *  LS_LINK_INHIBITED_IND, M_PROTO
 *  LS_LINK_INHIBIT_DENIED_IND, M_PROTO
 *  LS_LINK_INHIBIT_FAILED_IND, M_PROTO
 *  LS_LINK_UNINHIBITED_IND, M_PROTO
 *  LS_LINK_UNINHIBIT_FAILED_IND, M_PROTO
 */
typedef struct {
	ls_long primitive;		/* one of LS link primitives */
	ls_ulong lpc;			/* local point code */
	ls_ulong apc;			/* adjacent point code */
	ls_ulong slc;			/* signalling link code */
} ls_link_t;

/*
 *  Congestion Primitives:
 *
 *  LS_CONGESTED_IND, M_PCPROTO
 *  LS_CONGESTION_CEASED_IND, M_PCPROTO
 */
typedef struct {
	ls_long primitive;		/* one of LS congestion primitives */
	ls_ulong cong_status;		/* congestion status */
	ls_ulong disc_status;		/* discard status */
} ls_congestion_t;

/*
 *  Message Primitives:
 *
 *  LS_MESSAGE_FOR_ROUTING_REQ, M_PROTO
 *  LS_MESSAGE_FOR_DISTRIBUTION_REQ, M_PROTO
 *  LS_MESSAGE_FOR_ROUTING_IND, M_PROTO
 *  LS_MESSAGE_FOR_DISCRIMINATION_IND, M_PROTO
 *  LS_RETRIEVED_MESSAGE_IND, M_PROTO
 */

/*
 *  MTP-User Part Message (UPM), M_PROTO
 *
 *  LS_SIGNAL_UP    -   MTP-User (MTP-TRANSFER)
 */
typedef struct {
	ls_long primitive;		/* one of LS message primitives */
	ls_ulong signal;		/* Traffic Management Signal */
	ls_mtph_t mh;			/* mtp header */
} ls_signal_up_t;
/*
 *  Signalling Link Test Control Messages (SLTC), M_PROTO
 *
 *  LS_SIGNAL_SLTM  -   Signalling-link-test-message (regular)
 *  LS_SIGNAL_SLTA  -   Signalling-link-test-acknowledgement (regular)
 *  LS_SIGNAL_SSLTM -   Signalling-link-test-message (special)
 *  LS_SIGNAL_SSLTA -   Signalling-link-test-acknowledgement (special)
 */
typedef struct {
	ls_long primitive;		/* one of LS message primitives */
	ls_ulong signal;		/* Traffic Management Signal */
	ls_mtpm_t mh;			/* mtp management header */
	ls_ulong slc;			/* signalling link code (ANSI) */
	ls_uchar tli;			/* test length indicator */
	ls_uchar tmsx[15];		/* test pattern */
} ls_signal_sltc_t;
/*
 *  Changeback Messages (CBM), M_PROTO
 *
 *  LS_SIGNAL_CBD   -   Changeback-declaration
 *  LS_SIGNAL_CBA   -   Changeback-acknowledgement
 */
typedef struct {
	ls_long primitive;		/* one of LS message primitives */
	ls_ulong signal;		/* Changeback Signal */
	ls_mtpm_t mh;			/* mtp management header */
	ls_ulong slc;			/* signalling link code (ANSI) */
	ls_ulong cbc;			/* changeback code */
} ls_signal_cbm_t;
/*
 *  Changeover Messages (COM), M_PROTO
 *
 *  LS_SIGNAL_COO   -   Changeover-order
 *  LS_SIGNAL_COA   -   Changeover-acknowledgement
 */
typedef struct {
	ls_long primitive;		/* one of LS message primitives */
	ls_ulong signal;		/* Changeover Signal */
	ls_mtpm_t mh;			/* mtp management header */
	ls_ulong slc;			/* signalling link code (ANSI) */
	ls_ulong fsnc;			/* FSNC */
} ls_signal_com_t;
/*
 *  Signalling Link Messages (SLM), M_PROTO
 *
 *  LS_SIGNAL_CNP   -   Signalling-data-link-connection-not-possible
 *  LS_SIGNAL_CNS   -   Signalling-data-link-connection-not-successful
 *  LS_SIGNAL_CSS   -   Signalling-data-link-connection-successful
 *  LS_SIGNAL_ECO   -   Emergency-changeover-order
 *  LS_SIGNAL_ECA   -   Emergency-changeover-acknowledgement
 *  LS_SIGNAL_LFU   -   Link-force-unihibit
 *  LS_SIGNAL_LIA   -   Link-inhibit-accepted
 *  LS_SIGNAL_LID   -   Link-inhibit-denied
 *  LS_SIGNAL_LIN   -   Link-inhibit-not-possible
 *  LS_SIGNAL_LLI   -   Link-local-inhibit-test
 *  LS_SIGNAL_LRI   -   Link-remote-inhibit-test
 *  LS_SIGNAL_LUA   -   Link-uninhibit-accept
 *  LS_SIGNAL_LUN   -   Link-uninhibit-not-possible
 */
typedef struct {
	ls_long primitive;		/* one of LS message primitives */
	ls_ulong signal;		/* Emergency Changeover Signal */
	ls_mtpm_t mh;			/* mtp management header */
	ls_ulong slc;			/* signalling link code (ANSI) */
} ls_signal_slm_t;
/*
 *  Traffic Flow Control Messages (TFC), M_PROTO
 *
 *  LS_SIGNAL_TFC   -   Transfer-controlled
 */
typedef struct {
	ls_long primitive;		/* one of LS message primitives */
	ls_ulong signal;		/* Emergency Changeover Signal */
	ls_mtpm_t mh;			/* mtp management header */
	ls_ulong dest;			/* destination sp or cluster */
	ls_ulong stat;			/* congestion status */
} ls_signal_tfc_t;
/*
 *  Traffic Management Messages (TFM), M_PROTO
 *
 *  LS_SIGNAL_TFA   -   Transfer-allowed
 *  LS_SIGNAL_TFP   -   Transfer-prohibited
 *  LS_SIGNAL_TFR   -   Transfer-restricted
 *  LS_SIGNAL_TCA   -   Transfer-cluster-allowed
 *  LS_SIGNAL_TCP   -   Transfer-cluster-prohibited
 *  LS_SIGNAL_TCR   -   Transfer-cluster-restricted
 *  LS_SIGNAL_RSP   -   Signalling-route-set-test-prohibited
 *  LS_SIGNAL_RSR   -   Signalling-route-set-test-restricted
 *  LS_SIGNAL_RCP   -   Signalling-route-set-test-cluster-prohibited
 *  LS_SIGNAL_RCR   -   Signalling-route-set-test-cluster-restricted
 */
typedef struct {
	ls_long primitive;		/* one of LS message primitives */
	ls_ulong signal;		/* Traffic Management Signal */
	ls_mtpm_t mh;			/* mtp management header */
	ls_ulong dest;			/* concerned sp or cluster */
} ls_signal_tfm_t;
/*
 *  User-Part Flow Control Messages (UFC), M_PROTO
 *
 *  LS_SIGNAL_UPU   -   User-part-unavailable
 *  LS_SIGNAL_UPA   -   User-part-available
 *  LS_SIGNAL_UPT   -   User-part-test
 */
typedef struct {
	ls_long primitive;		/* one of LS message primitives */
	ls_ulong signal;		/* Traffic Management Signal */
	ls_mtpm_t mh;			/* mtp management header */
	ls_ulong dest;			/* destination sp */
	ls_ulong upi;			/* user part indicator */
} ls_signal_ufc_t;
/*
 *  Data Link Connection Messages (DLC), M_PROTO
 *
 *  LS_SIGNAL_DLC   -   Data-link-connection-order
 */
typedef struct {
	ls_long primitive;		/* one of LS message primitives */
	ls_ulong signal;		/* Traffic Management Signal */
	ls_mtpm_t mh;			/* mtp management header */
	ls_ulong slc;			/* signalling link code (ANSI) */
	ls_ulong sdli;			/* signalling data link indicator */
} ls_signal_dlc_t;
/*
 *  Management messages without parameters (SIG), M_PROTO
 *
 *  LS_SIGNAL_RCT   -   Route-set-congestion-test
 *  LS_SIGNAL_TRA   -   Traffic-restart-allowed
 *  LS_SIGNAL_TRW   -   Traffic-restart-waiting
 */
typedef struct {
	ls_long primitive;		/* one of LS message primitives */
	ls_ulong signal;		/* Traffic Management Signal */
	ls_mtpm_t mh;			/* mtp management header */
} ls_signal_sig_t;

typedef union {
	ls_long primitive;
	ls_signal_cbm_t cbm;
	ls_signal_com_t com;
	ls_signal_dlc_t dlc;
	ls_signal_sig_t sig;
	ls_signal_slm_t slm;
	ls_signal_sltc_t sltc;
	ls_signal_tfc_t tfc;
	ls_signal_tfm_t tfm;
	ls_signal_ufc_t ufc;
	ls_signal_up_t up;
} ls_signals_t;

union LS_primitives {
	ls_long primitive;

	ls_link_t link;
	ls_congestion_t congestion;
	ls_signals_t signals;

	ls_signal_cbm_t cbm;
	ls_signal_cbm_t cba;
	ls_signal_cbm_t cbd;
	ls_signal_com_t com;
	ls_signal_com_t coa;
	ls_signal_com_t coo;
	ls_signal_dlc_t dlc;
	ls_signal_sig_t sig;
	ls_signal_sig_t rct;
	ls_signal_sig_t tra;
	ls_signal_sig_t trw;
	ls_signal_slm_t slm;
	ls_signal_slm_t cnp;
	ls_signal_slm_t cns;
	ls_signal_slm_t css;
	ls_signal_slm_t eca;
	ls_signal_slm_t eco;
	ls_signal_slm_t lfu;
	ls_signal_slm_t lia;
	ls_signal_slm_t lid;
	ls_signal_slm_t lin;
	ls_signal_slm_t lli;
	ls_signal_slm_t lri;
	ls_signal_slm_t lua;
	ls_signal_slm_t lun;
	ls_signal_sltc_t sltc;
	ls_signal_sltc_t slta;
	ls_signal_sltc_t sltm;
	ls_signal_sltc_t sslta;
	ls_signal_sltc_t ssltm;
	ls_signal_tfc_t tfc;
	ls_signal_tfm_t tfm;
	ls_signal_tfm_t rcp;
	ls_signal_tfm_t rcr;
	ls_signal_tfm_t rsp;
	ls_signal_tfm_t rsr;
	ls_signal_tfm_t tca;
	ls_signal_tfm_t tcp;
	ls_signal_tfm_t tcr;
	ls_signal_tfm_t tfa;
	ls_signal_tfm_t tfp;
	ls_signal_tfm_t tfr;
	ls_signal_ufc_t ufc;
	ls_signal_ufc_t upa;
	ls_signal_ufc_t upt;
	ls_signal_ufc_t upu;
	ls_signal_up_t up;
	ls_signal_up_t user;
};

typedef union LS_primitives ls_prim_t;

#define LS_PRIMITIVE_MAX_SIZE	sizeof(ls_prim_t)

#define LS_LINK_SIZE		sizeof(ls_link_t)
#define LS_CONGESTION_SIZE	sizeof(ls_congestion_t)
#define LS_SIGNALS_SIZE		sizeof(ls_signals_t)

#define LS_SIGNAL_CBM_SIZE	sizeof(ls_signal_cbm_t)
#define LS_SIGNAL_CBA_SIZE	sizeof(ls_signal_cbm_t)
#define LS_SIGNAL_CBD_SIZE	sizeof(ls_signal_cbm_t)
#define LS_SIGNAL_COM_SIZE	sizeof(ls_signal_com_t)
#define LS_SIGNAL_COA_SIZE	sizeof(ls_signal_com_t)
#define LS_SIGNAL_COO_SIZE	sizeof(ls_signal_com_t)
#define LS_SIGNAL_DLC_SIZE	sizeof(ls_signal_dlc_t)
#define LS_SIGNAL_SLM_SIZE	sizeof(ls_signal_slm_t)
#define LS_SIGNAL_CNP_SIZE	sizeof(ls_signal_slm_t)
#define LS_SIGNAL_CNS_SIZE	sizeof(ls_signal_slm_t)
#define LS_SIGNAL_CSS_SIZE	sizeof(ls_signal_slm_t)
#define LS_SIGNAL_LFU_SIZE	sizeof(ls_signal_slm_t)
#define LS_SIGNAL_LIA_SIZE	sizeof(ls_signal_slm_t)
#define LS_SIGNAL_LID_SIZE	sizeof(ls_signal_slm_t)
#define LS_SIGNAL_LIN_SIZE	sizeof(ls_signal_slm_t)
#define LS_SIGNAL_LLI_SIZE	sizeof(ls_signal_slm_t)
#define LS_SIGNAL_LRI_SIZE	sizeof(ls_signal_slm_t)
#define LS_SIGNAL_LUA_SIZE	sizeof(ls_signal_slm_t)
#define LS_SIGNAL_LUN_SIZE	sizeof(ls_signal_slm_t)
#define LS_SIGNAL_SIG_SIZE	sizeof(ls_signal_sig_t)
#define LS_SIGNAL_RCT_SIZE	sizeof(ls_signal_sig_t)
#define LS_SIGNAL_TRA_SIZE	sizeof(ls_signal_sig_t)
#define LS_SIGNAL_TRW_SIZE	sizeof(ls_signal_sig_t)
#define LS_SIGNAL_ECA_SIZE	sizeof(ls_signal_slm_t)
#define LS_SIGNAL_ECO_SIZE	sizeof(ls_signal_slm_t)
#define LS_SIGNAL_TFC_SIZE	sizeof(ls_signal_tfc_t)
#define LS_SIGNAL_TFM_SIZE	sizeof(ls_signal_tfm_t)
#define LS_SIGNAL_RCP_SIZE	sizeof(ls_signal_tfm_t)
#define LS_SIGNAL_RCR_SIZE	sizeof(ls_signal_tfm_t)
#define LS_SIGNAL_RSP_SIZE	sizeof(ls_signal_tfm_t)
#define LS_SIGNAL_RSR_SIZE	sizeof(ls_signal_tfm_t)
#define LS_SIGNAL_TCA_SIZE	sizeof(ls_signal_tfm_t)
#define LS_SIGNAL_TCP_SIZE	sizeof(ls_signal_tfm_t)
#define LS_SIGNAL_TCR_SIZE	sizeof(ls_signal_tfm_t)
#define LS_SIGNAL_TFA_SIZE	sizeof(ls_signal_tfm_t)
#define LS_SIGNAL_TFP_SIZE	sizeof(ls_signal_tfm_t)
#define LS_SIGNAL_TFR_SIZE	sizeof(ls_signal_tfm_t)
#define LS_SIGNAL_UFC_SIZE	sizeof(ls_signal_ufc_t)
#define LS_SIGNAL_UPA_SIZE	sizeof(ls_signal_ufc_t)
#define LS_SIGNAL_UPT_SIZE	sizeof(ls_signal_ufc_t)
#define LS_SIGNAL_UPU_SIZE	sizeof(ls_signal_ufc_t)
#define LS_SIGNAL_SLTC_SIZE	sizeof(ls_signal_sltc_t)
#define LS_SIGNAL_SLTA_SIZE	sizeof(ls_signal_sltc_t)
#define LS_SIGNAL_SLTM_SIZE	sizeof(ls_signal_sltc_t)
#define LS_SIGNAL_SSLTA_SIZE	sizeof(ls_signal_sltc_t)
#define LS_SIGNAL_SSLTM_SIZE	sizeof(ls_signal_sltc_t)
#define LS_SIGNAL_UP_SIZE	sizeof(ls_signal_up_t)
#define LS_SIGNAL_USER_SIZE	sizeof(ls_signal_up_t)

#endif				/* __SS7_SLSI_H__ */
