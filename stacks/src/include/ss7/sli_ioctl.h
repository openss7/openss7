/*****************************************************************************

 @(#) $Id: sli_ioctl.h,v 0.9.2.5 2006/10/31 21:04:37 brian Exp $

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

 Last Modified $Date: 2006/10/31 21:04:37 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SLI_IOCTL_H__
#define __SLI_IOCTL_H__

#ident "@(#) $RCSfile: sli_ioctl.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2004  OpenSS7 Corporation"

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>

/*
 *  This file is rather Linux specific for now.  Linux encodes direction and
 *  size in upper 16 bits of 32 bit ioctl command.  This should make
 *  transparent ioctl in STREAMS easier for Linux (i.e., all transparent
 *  ioctls could be converted to I_STR types because size is known).
 *
 *  If using SVR4.2, perhaps these macros should be redefined to something
 *  which does not accept size and transpartent ioctls would have to be
 *  provided for in the STREAMS code anyways.  Then it would work either way.
 *
 *  Following defines would do the job...
 *
 *  #define _IO(magic,nr)        ((magic<<8)+nr)
 *  #define _IOR(magic,nr,size)  ((magic<<8)+nr)
 *  #define _IOW(magic,nr,size)  ((magic<<8)+nr)
 *  #define _IOWR(magic,nr,size) ((magic<<8)+nr)
 */

#define SL_IOC_MAGIC 'l'

/*
 *  PROTOCOL VARIANT AND OPTIONS
 */

#define SL_IOCGOPTIONS	_IOR(  SL_IOC_MAGIC, 0, lmi_option_t )
#define SL_IOCSOPTIONS	_IOW(  SL_IOC_MAGIC, 1, lmi_option_t )

/*
 *  CONFIGURATION
 */
#ifdef __KERNEL__
typedef struct sl_timers {
	toid_t t1;			/* T1 timer */
	toid_t t2;			/* T2 timer */
	toid_t t3;			/* T3 timer */
	toid_t t4;			/* T4 timer */
	toid_t t5;			/* T5 timer */
	toid_t t6;			/* T6 timer */
	toid_t t7;			/* T7 timer */
} sl_timers_t;
#endif

typedef struct sl_config {
	sl_ulong t1;			/* timer t1 duration (milliseconds) */
	sl_ulong t2;			/* timer t2 duration (milliseconds) */
	sl_ulong t2l;			/* timer t2l duration (milliseconds) */
	sl_ulong t2h;			/* timer t2h duration (milliseconds) */
	sl_ulong t3;			/* timer t3 duration (milliseconds) */
	sl_ulong t4n;			/* timer t4n duration (milliseconds) */
	sl_ulong t4e;			/* timer t4e duration (milliseconds) */
	sl_ulong t5;			/* timer t5 duration (milliseconds) */
	sl_ulong t6;			/* timer t6 duration (milliseconds) */
	sl_ulong t7;			/* timer t7 duration (milliseconds) */
	sl_ulong rb_abate;		/* RB cong abatement (#msgs) */
	sl_ulong rb_accept;		/* RB cong onset accept (#msgs) */
	sl_ulong rb_discard;		/* RB cong discard (#msgs) */
	sl_ulong tb_abate_1;		/* lev 1 cong abate (#bytes) */
	sl_ulong tb_onset_1;		/* lev 1 cong onset (#bytes) */
	sl_ulong tb_discd_1;		/* lev 1 cong discard (#bytes) */
	sl_ulong tb_abate_2;		/* lev 1 cong abate (#bytes) */
	sl_ulong tb_onset_2;		/* lev 1 cong onset (#bytes) */
	sl_ulong tb_discd_2;		/* lev 1 cong discard (#bytes) */
	sl_ulong tb_abate_3;		/* lev 1 cong abate (#bytes) */
	sl_ulong tb_onset_3;		/* lev 1 cong onset (#bytes) */
	sl_ulong tb_discd_3;		/* lev 1 cong discard (#bytes) */
	sl_ulong N1;			/* PCR/RTBmax messages (#msg) */
	sl_ulong N2;			/* PCR/RTBmax octets (#bytes) */
	sl_ulong M;			/* IAC normal proving periods */
} sl_config_t;

#define SL_IOCGCONFIG	_IOWR( SL_IOC_MAGIC, 2, sl_config_t )
#define SL_IOCSCONFIG	_IOWR( SL_IOC_MAGIC, 3, sl_config_t )
#define SL_IOCTCONFIG	_IOWR( SL_IOC_MAGIC, 4, sl_config_t )
#define SL_IOCCCONFIG	_IOWR( SL_IOC_MAGIC, 5, sl_config_t )

/*
 *  STATE
 */

struct sni {
	unsigned short bib;
	unsigned short bsn;
	unsigned short fib;
	unsigned short fsn;
};

typedef struct tx_sni {
	struct sni N;			/* normal */
	struct sni F;			/* first in RTB */
	struct sni L;			/* last in RTB */
	struct sni C;			/* RTB clear */
	struct sni X;			/* expected */
	struct sni R;			/* received */
	unsigned char sio;		/* SIO for LSSU */
	int len;			/* len for Tx */
	unsigned short ibmsk;		/* mask for IB */
	unsigned short snmsk;		/* mask for SN */
} tx_sni_t;

typedef struct rx_sni {
	struct sni X;			/* expected */
	struct sni F;			/* */
	struct sni R;			/* received */
	struct sni T;			/* transmitted */
	unsigned char sio;		/* SIO for LSSU */
	int len;			/* len for Rx */
	unsigned short ibmsk;		/* mask for IB */
	unsigned short snmsk;		/* mask for SN */
} rx_sni_t;

enum {
	LSSU_SIO = 0x0,			/* */
	LSSU_SIN = 0x1,			/* normal */
	LSSU_SIE = 0x2,			/* emergency */
	LSSU_SIOS = 0x3,		/* out of service */
	LSSU_SIPO = 0x4,		/* processor outage */
	LSSU_SIB = 0x5			/* busy */
};

typedef struct sl_statem {
	sl_ulong lsc_state;
	sl_ulong local_processor_outage;
	sl_ulong remote_processor_outage;
	sl_ulong processor_outage;
	sl_ulong emergency;
	sl_ulong l3_indication_received;
	sl_ulong failure_reason;	/* added for event notification */
	sl_ulong iac_state;
	sl_ulong further_proving;
	sl_ulong t4v;
	sl_ulong Cp;
	sl_ushort ib_mask;
	sl_ushort sn_mask;
	sl_ulong txc_state;
	tx_sni_t tx;			/* transmit SN's and IB's */
	sl_ulong msu_inhibited;
	sl_ulong lssu_available;
	sl_ulong rtb_full;
	sl_ulong clear_rtb;
	sl_ulong sib_received;
	sl_ulong retrans_cycle;
	sl_ulong forced_retransmission;
	sl_ulong Cm;			/* number of message in transmission buffer */
	sl_ulong Ct;			/* number of messages in retransmission buffer */
	sl_ulong cong_status;
	sl_ulong disc_status;
	sl_ulong Z;
	void *z_ptr;			/* actually mblk_t* */
	sl_ulong rc_state;
	rx_sni_t rx;			/* receive SN's and IB's */
	sl_ulong rtr;			/* retransmission indicator */
	sl_ulong unb;			/* unreasonable BSN received */
	sl_ulong unf;			/* unreasonable FIB received */
	sl_ulong msu_fisu_accepted;	/* proving complete */
	sl_ulong abnormal_bsnr;		/* received abnormal BSN */
	sl_ulong abnormal_fibr;		/* received abnormal FIB */
	sl_ulong congestion_discard;	/* discarding under Rx congest */
	sl_ulong congestion_accept;	/* accepting under Rx congest */
	sl_ulong l2_congestion_detect;	/* congestion detected by L2 */
	sl_ulong l3_congestion_detect;	/* congestion detected by L3 */
	sl_ulong Cr;			/* number of messages in receive buffer */
	sl_ulong cc_state;		/* CC state variable */
	sl_ulong poc_state;		/* POC state variable */
} sl_statem_t;

#define SL_STATE_IDLE				0	/* txc, others */
#define SL_STATE_NORMAL				0	/* cc */
#define SL_STATE_POWER_OFF			0	/* lsc */
#define SL_STATE_BUSY				1	/* cc */
#define SL_STATE_IN_SERVICE			1	/* txc, others */
#define SL_STATE_LOCAL_PROCESSOR_OUTAGE		1	/* poc */
#define SL_STATE_PROVING			1	/* iac */
#define SL_STATE_INITIAL_ALIGNMENT		2	/* lsc */
#define SL_STATE_NOT_ALIGNED			2	/* iac */
#define SL_STATE_REMOTE_PROCESSOR_OUTAGE	2	/* poc */
#define SL_STATE_SLEEPING			2	/* txc */
#define SL_STATE_ALIGNED			3	/* iac */
#define SL_STATE_ALIGNED_NOT_READY		3	/* lsc */
#define SL_STATE_BOTH_PROCESSORS_OUT		3	/* poc */
#define SL_STATE_ALIGNED_READY			4	/* lsc */
#define SL_STATE_OUT_OF_SERVICE			5	/* lsc */
#define SL_STATE_PROCESSOR_OUTAGE		6	/* lsc */

#define SL_IOCGSTATEM	_IOR(  SL_IOC_MAGIC, 6, sl_statem_t )
#define SL_IOCCMRESET	_IOR(  SL_IOC_MAGIC, 7, sl_statem_t )

/*
 *  -----------------------------------------------------------------------
 *
 *  STATISTICS
 *
 *  -----------------------------------------------------------------------
 */

typedef struct sl_stats {
	lmi_sta_t header;
	 /**/ sl_ulong sl_dur_in_service;	/* Q.752 Table 1.1 30 min */
	sl_ulong sl_fail_align_or_proving;	/* Q.752 Table 1.7 5,30 min */
	sl_ulong sl_nacks_received;	/* Q.752 Table 1.9 5,30 min */
	sl_ulong sl_dur_unavail;	/* Q.752 Table 2.1 30 min */
	sl_ulong sl_dur_unavail_failed;	/* Q.752 Table 2.7 30 min */
	sl_ulong sl_dur_unavail_rpo;	/* Q.752 Table 2.9 30 min */
	sl_ulong sl_sibs_sent;		/* Q.752 Table 2.15 5,30 min */
	sl_ulong sl_tran_sio_sif_octets;	/* Q.752 Table 3.1 5,30 min - trasmitted first time 
						 */
	sl_ulong sl_retrans_octets;	/* Q.752 Table 3.2 5,30 min - includes opening flag and crc 
					 */
	sl_ulong sl_tran_msus;		/* Q.752 Table 3.3 5,30 min - transmitted first time */
	sl_ulong sl_recv_sio_sif_octets;	/* Q.752 Table 3.4 5,30 min */
	sl_ulong sl_recv_msus;		/* Q.752 Table 3.5 5,30 min - passed to L3 */
	sl_ulong sl_cong_onset_ind[4];	/* Q.752 Table 3.6 5,30 min - and 3.9 */
	sl_ulong sl_dur_cong_status[4];	/* Q.752 Table 3.7 30 min - rising only */
	/*
	   sl_ulong sl_msus_cong_discard; Q.752 Table 3.10 5,30 min - Not here in MTP or SLS 
	 */
	sl_ulong sl_cong_discd_ind[4];	/* Q.752 Table 3.11 5,30 min - rising only when < onset */
} sl_stats_t;				/* FIXME: read 3.11 and recheck congestion procedures */

#define SL_IOCGSTATSP	_IOR(  SL_IOC_MAGIC,  8, lmi_sta_t )
#define SL_IOCSSTATSP	_IOWR( SL_IOC_MAGIC,  9, lmi_sta_t )
#define SL_IOCGSTATS	_IOR(  SL_IOC_MAGIC, 10, sl_stats_t )
#define SL_IOCCSTATS	_IOW(  SL_IOC_MAGIC, 11, sl_stats_t )

/*
 *  NOTIFICATIONS
 */

typedef struct sl_notify {
	sl_ulong events;
} sl_notify_t;

#define SL_EVENT_HEADER_LEN         16	/* for now */

#define SL_EVT_FAIL_UNSPECIFIED		SL_FAIL_UNSPECIFIED
#define SL_EVT_FAIL_CONG_TIMEOUT	SL_FAIL_CONG_TIMEOUT	/* Q.752 Table 1.6 */
#define SL_EVT_FAIL_ACK_TIMEOUT		SL_FAIL_ACK_TIMEOUT	/* Q.752 Table 1.4 */
#define SL_EVT_FAIL_ABNORMAL_BSNR	SL_FAIL_ABNORMAL_BSNR
#define SL_EVT_FAIL_ABNORMAL_FIBR	SL_FAIL_ABNORMAL_FIBR
#define SL_EVT_FAIL_ABORMAL		SL_FAIL_ABNORMAL_BSNR | \
					SL_FAIL_ABNORMAL_FIBR	/* Q.752 Table 1.3 */
#define SL_EVT_FAIL_SUERM_EIM		SL_FAIL_SUERM_EIM	/* Q.752 Table 1.5 */
#define SL_EVT_FAIL_ALIGNMENT		SL_FAIL_ALIGNMENT_NOT_POSSIBLE
#define SL_EVT_FAIL_RECEIVED_SIO	SL_FAIL_RECEIVED_SIO
#define SL_EVT_FAIL_RECEIVED_SIN	SL_FAIL_RECEIVED_SIN
#define SL_EVT_FAIL_RECEIVED_SIE	SL_FAIL_RECEIVED_SIE
#define SL_EVT_FAIL_RECEIVED_SIOS	SL_FAIL_RECEIVED_SIOS
#define SL_EVT_FAIL_T1_TIMEOUT		SL_FAIL_T1_TIMEOUT
#define SL_EVT_RPO_BEGIN		SL_FAIL_T1_TIMEOUT      <<1	/* Q.752 Table 2.10 */
#define SL_EVT_RPO_END			SL_EVT_RPO_BEGIN        <<1	/* Q.752 Table 2.11 */
#define SL_EVT_CONGEST_ONSET_IND	SL_EVT_RPO_END          <<1	/* Q.752 Table 3.6 1st&D */
#define SL_EVT_CONGEST_DISCD_IND	SL_EVT_CONGEST_ONSET_IND<<1	/* Q.752 Table 3.11 1st&D */

#define SL_EVT_FAIL_ALL_REASONS	 SL_FAIL_UNSPECIFIED \
				|SL_FAIL_CONG_TIMEOUT \
				|SL_FAIL_ACK_TIMEOUT  \
				|SL_FAIL_ABNORMAL_FIBR \
				|SL_FAIL_ABNORMAL_BSNR \
				|SL_FAIL_SUERM_EIM \
				|SL_FAIL_ALIGNMENT_NOT_POSSIBLE \
				|SL_FAIL_RECEIVED_SIO \
				|SL_FAIL_RECEIVED_SIN \
				|SL_FAIL_RECEIVED_SIE \
				|SL_FAIL_RECEIVED_SIOS \
				|SL_FAIL_T1_TIMEOUT	/* Q.752 Table 1.2 */

#define SL_IOCGNOTIFY	_IOR(  SL_IOC_MAGIC, 12, sl_notify_t )
#define SL_IOCSNOTIFY	_IOW(  SL_IOC_MAGIC, 13, sl_notify_t )
#define SL_IOCCNOTIFY	_IOW(  SL_IOC_MAGIC, 14, sl_notify_t )

#define SL_IOC_FIRST     0
#define SL_IOC_LAST     14
#define SL_IOC_PRIVATE  32

#endif				/* __SLI_IOCTL_H__ */
