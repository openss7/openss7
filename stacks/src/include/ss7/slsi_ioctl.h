/*****************************************************************************

 @(#) $Id: slsi_ioctl.h,v 0.9.2.4 2007/06/17 01:56:02 brian Exp $

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

 Last Modified $Date: 2007/06/17 01:56:02 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: slsi_ioctl.h,v $
 Revision 0.9.2.4  2007/06/17 01:56:02  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __SLSI_IOCTL_H__
#define __SLSI_IOCTL_H__

#ident "@(#) $RCSfile: slsi_ioctl.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>

/*
 *  FIXME: these definitions are completely bogus right now....
 */

#define LS_IOC_MAGIC 'S'

/*
 *  PROTOCOL VARIANT AND OPTIONS
 */

#define LS_IOCGOPTIONS	_IOR(  LS_IOC_MAGIC, 0, lmi_option_t )
#define LS_IOCSOPTIONS	_IOW(  LS_IOC_MAGIC, 1, lmi_option_t )

/*
 *  CONFIGURATION
 */
typedef struct ls_config {
	ls_uchar type;			/* link set type */
	ls_uchar ni;			/* network indicator for this linkset */
	ls_ulong apc;			/* adjacent point code */
	ls_ulong lpc;			/* local point code */
	ls_ulong links_specified;	/* number of links specified for linkset */
} ls_config_t;

#define LS_TYPE_A_LINK	0x01
#define LS_TYPE_B_LINK	0x02
#define LS_TYPE_C_LINK	0x84
#define LS_TYPE_D_LINK	0x08
#define LS_TYPE_E_LINK	0x10
#define LS_TYPE_F_LINK	0xa0

#define LS_TYPE_STP	0x40
#define LS_TYPE_DOUBLE	0x80

#define LS_IOCGCONFIG	_IOWR( LS_IOC_MAGIC, 2, ls_config_t )
#define LS_IOCSCONFIG	_IOWR( LS_IOC_MAGIC, 3, ls_config_t )
#define LS_IOCTCONFIG	_IOWR( LS_IOC_MAGIC, 4, ls_config_t )
#define LS_IOCCCONFIG	_IOWR( LS_IOC_MAGIC, 5, ls_config_t )

/*
 *  STATE
 */

typedef struct ls_statem {
	ls_ulong flags;
	ls_ulong sls;			/* sls for loadsharing SNMMs */
	ls_ulong links_loaded;		/* number of links delivering traffic */
	ls_ulong links_available;	/* number of links available */
	ls_ulong cong_status;		/* cong status for this link set */
	ls_ulong disc_status;		/* disc status for this link set */
} ls_statem_t;

#define LS_FLAGS_AVAILABLE	    0x00000001	/* link set is available */
#define LS_FLAGS_CRITICAL	    0x00000002	/* link set supports critical routes */
#define LS_FLAGS_EMERGENCY	    0x00000004	/* link set has an emergency */
#define LS_FLAGS_ROUTING_OUTAGE	    0x00000008	/* link set cannot talk to routing */
#define LS_FLAGS_TRAFFIC	    0x00000010	/* link set is loaded */
#define LS_FLAGS_ADJ_INACCESSIBLE   0x00000020	/* adjacent sp is inaccessible */

#define LS_IOCGSTATEM	_IOR(  LS_IOC_MAGIC, 6, ls_statem_t )
#define LS_IOCCMRESET	_IOR(  LS_IOC_MAGIC, 7, ls_statem_t )

/*
 *  -----------------------------------------------------------------------
 *
 *  STATISTICS
 *
 *  -----------------------------------------------------------------------
 */

typedef struct ls_stats {
	lmi_sta_t header;
} ls_stats_t;
					    /*
					       FIXME: read 3.11 and recheck congestion procedures 
					     */

#define LS_IOCGSTATSP	_IOR(  LS_IOC_MAGIC,  8, lmi_sta_t )
#define LS_IOCSSTATSP	_IORW( LS_IOC_MAGIC,  9, lmi_sta_t )
#define LS_IOCGSTATS	_IOR(  LS_IOC_MAGIC, 10, ls_stats_t )
#define LS_IOCCSTATS	_IOW(  LS_IOC_MAGIC, 11, ls_stats_t )

/*
 *  NOTIFICATIONS
 */

typedef struct ls_notify {
	ls_ulong events;
} ls_notify_t;

#define LS_EVENT_HEADER_LEN         16	/* for now */

#define LS_EVT_FAIL_UNSPECIFIED   LS_FAIL_UNSPECIFIED
#define LS_EVT_FAIL_CONG_TIMEOUT  LS_FAIL_CONG_TIMEOUT	/* Q.752 Table 1.6 */
#define LS_EVT_FAIL_ACK_TIMEOUT   LS_FAIL_ACK_TIMEOUT	/* Q.752 Table 1.4 */
#define LS_EVT_FAIL_ABNORMAL_BSNR LS_FAIL_ABNORMAL_BSNR
#define LS_EVT_FAIL_ABNORMAL_FIBR LS_FAIL_ABNORMAL_FIBR
#define LS_EVT_FAIL_ABORMAL       LS_FAIL_ABNORMAL_BSNR | \
                                  LS_FAIL_ABNORMAL_FIBR	/* Q.752 Table 1.3 */
#define LS_EVT_FAIL_SUERM_EIM     LS_FAIL_SUERM_EIM	/* Q.752 Table 1.5 */
#define LS_EVT_FAIL_ALIGNMENT     LS_FAIL_ASLIGNMENT_NOT_POSSIBLE
#define LS_EVT_FAIL_RECEIVED_SIO  LS_FAIL_RECEIVED_SIO
#define LS_EVT_FAIL_RECEIVED_SIN  LS_FAIL_RECEIVED_SIN
#define LS_EVT_FAIL_RECEIVED_SIE  LS_FAIL_RECEIVED_SIE
#define LS_EVT_FAIL_RECEIVED_SIOS LS_FAIL_RECEIVED_SIOS
#define LS_EVT_FAIL_T1_TIMEOUT    LS_FAIL_T1_TIMEOUT
#define LS_EVT_RPO_BEGIN          LS_FAIL_T1_TIMEOUT      <<1	/* Q.752 Table 2.10 */
#define LS_EVT_RPO_END            LS_EVT_RPO_BEGIN        <<1	/* Q.752 Table 2.11 */
#define LS_EVT_CONGEST_ONSET_IND  LS_EVT_RPO_END          <<1	/* Q.752 Table 3.6 1st&D */
#define LS_EVT_CONGEST_DISCD_IND  LS_EVT_CONGEST_ONSET_IND<<1	/* Q.752 Table 3.11 1st&D */

#define LS_EVT_FAIL_ALL_REASONS   LS_FAIL_UNSPECIFIED \
                                 |LS_FAIL_CONG_TIMEOUT \
                                 |LS_FAIL_ACK_TIMEOUT  \
                                 |LS_FAIL_ABNORMAL_FIBR \
                                 |LS_FAIL_ABNORMAL_BSNR \
                                 |LS_FAIL_SUERM_EIM \
                                 |LS_FAIL_ALIGNMENT_NOT_POSSIBLE \
                                 |LS_FAIL_RECEIVED_SIO \
                                 |LS_FAIL_RECEIVED_SIN \
                                 |LS_FAIL_RECEIVED_SIE \
                                 |LS_FAIL_RECEIVED_SIOS \
                                 |LS_FAIL_T1_TIMEOUT	/* Q.752 Table 1.2 */

#define LS_IOCGNOTIFY	_IOR(  LS_IOC_MAGIC, 12, ls_notify_t )
#define LS_IOCSNOTIFY	_IOW(  LS_IOC_MAGIC, 13, ls_notify_t )
#define LS_IOCCNOTIFY	_IOW(  LS_IOC_MAGIC, 14, ls_notify_t )

#define LS_IOC_FIRST     0
#define LS_IOC_LAST     14
#define LS_IOC_PRIVATE  32

#define LK_IOC_MAGIC 'S'

/*
 *  PROTOCOL VARIANT AND OPTIONS
 */

#define LK_IOCGOPTIONS	_IOR(  LK_IOC_MAGIC, 0, lmi_option_t )
#define LK_IOCSOPTIONS	_IOW(  LK_IOC_MAGIC, 1, lmi_option_t )

/*
 *  CONFIGURATION
 */
typedef struct lk_config {
	ls_uchar ni;			/* network indicator for this link */
	ls_ulong slc;			/* signalling link code (initial) */
	ls_ulong tt1;			/* signalling link test timer */
	ls_ulong tt2;			/* signalling link test timer */
	ls_ulong t1;
	ls_ulong t2;
	ls_ulong t3;
	ls_ulong t4;
	ls_ulong t5;
	ls_ulong t7;
	ls_ulong t12;
	ls_ulong t13;
	ls_ulong t14;
	ls_ulong t17;
	ls_ulong t22;
	ls_ulong t23;
	ls_ulong t24;
	ls_ulong terminal_predetermined;
	ls_ulong data_link_predetermined;
} lk_config_t;

#define LK_IOCGCONFIG	_IOWR( LK_IOC_MAGIC, 2, lk_config_t )
#define LK_IOCSCONFIG	_IOWR( LK_IOC_MAGIC, 3, lk_config_t )
#define LK_IOCTCONFIG	_IOWR( LK_IOC_MAGIC, 4, lk_config_t )
#define LK_IOCCCONFIG	_IOWR( LK_IOC_MAGIC, 5, lk_config_t )

/*
 *  STATE
 */

typedef struct lk_statem {
	ls_uchar tli;			/* test length indicator for test messages */
	ls_uchar tmsx[15];		/* test pattern for test messages */
	ls_ulong slc;			/* signalling link code (actual) */
	ls_ulong sli;			/* signalling link index (larray) */
	ls_ulong bsnt;			/* backwards seuqnece number for xmission */
	ls_ulong fsnc;			/* forward sequence number changeover */
	ls_ulong flags;			/* flags for this link */
	ls_ulong c;			/* no of logical channels (SLS values) */
	ls_ulong cong_status;		/* congestion status for this link */
	ls_ulong disc_status;		/* discard status for this link */
} lk_statem_t;

#define LK_FLAG_OUT_OF_SERVICE	0x0001	/* link is out of service */
#define LK_FLAG_INACTIVE	0x0002	/* link has not been activated */
#define LK_FLAG_LOCAL_INHIBIT	0x0004	/* link has been inhibited locally */
#define LK_FLAG_REMOTE_INHIBIT	0x0008	/* link has been inhibited remotely */
#define LK_FLAG_CONGESTED	0x0010	/* link is congested */
#define LK_FLAG_BLOCKED		0x0020	/* link is blocked (remote proc outage) */
#define LK_FLAG_CHANGEOVER	0x0040	/* performing changeover on this link */
#define LK_FLAG_CHANGEBACK	0x0080	/* performing changeback on this link */
#define LK_FLAG_TEST		0x0100	/* first SLT */
#define LK_FLAG_RETEST		0x0200	/* second SLT */

#define LK_MASK_DONTUSE		0x000f	/* this link is unusable */

#define LK_COB_IDLE		0	/* no changeover/back in progress */
#define LK_COB_BSNT_REQ_COO	1	/* no changeover/back in progress */
#define LK_COB_BSNT_REQ_COA	2	/* no changeover/back in progress */
#define LK_COB_TIME_CONTROLLED	3	/* no changeover/back in progress */
#define LK_COB_WAITING_ACK	4	/* no changeover/back in progress */
#define LK_COB_RETRIEVING	5	/* no changeover/back in progress */

#define LK_STATE_IDLE				 0
#define LK_STATE_INACTIVE			 0
#define LK_STATE_UNAVAILABLE			 0	/* tlac */
#define LK_STATE_ACTIVE				 1
#define LK_STATE_WAIT				 2
#define LK_STATE_ACTIVATING_RESTORING		 3
#define LK_STATE_FAILED				 4
#define LK_STATE_WAIT_FOR_DATA_LINK		 5
#define LK_STATE_WAIT_FOR_TERMINAL		 6
#define LK_STATE_WAITING			 7
#define LK_STATE_WAITING_FOR_CONNECTION		 8
#define LK_STATE_AVAILABLE			 9	/* tlac */
#define LK_STATE_SP_RESTARTING			10	/* tlac */
#define LK_STATE_1ST_ATTEMPT			11	/* sltc */
#define LK_STATE_2ND_ATTEMPT			12	/* sltc */
#define LK_STATE_TIME_CONTROLLED_DIVERSION	13	/* tcbc */
#define LK_STATE_FIRST_ATTEMPT			14	/* tcbc */
#define LK_STATE_SECOND_ATTEMPT			15	/* tcbc */
#define LK_STATE_WAIT_FOR_ROUTING_DATA		16	/* tcbc */
#define LK_STATE_WAIT01				17	/* tcoc */
#define LK_STATE_WAIT02				18	/* tcoc */
#define LK_STATE_WAIT05				19	/* tcoc */
#define LK_STATE_WAIT06				20	/* tcoc */
#define LK_STATE_WAIT07				21	/* tcoc */
#define LK_STATE_RETRIEVING			22	/* tcoc */
#define LK_STATE_WAIT_FOR_ACKNOWLEDGEMENT	23	/* tcoc */

/*
 *  New link flags
 */

#define LK_FLAGS_FAILED			0x00000001
#define LK_FLAGS_DEACTIVATED		0x00000002
#define LK_FLAGS_INACTIVE		0x00000003
#define LK_FLAGS_LINK_BLOCKED		0x00000004
#define LK_FLAGS_LINKSET_BLOCKED	0x00000008
#define LK_FLAGS_LOCAL_BLOCKED		0x0000000c
#define LK_FLAGS_REMOTE_BLOCKED		0x00000010
#define LK_FLAGS_BLOCKED		0x0000001c
#define LK_FLAGS_LOCAL_INHIBITED	0x00000020
#define LK_FLAGS_REMOTE_INHIBITED	0x00000040
#define LK_FLAGS_INHIBITED		0x00000060
#define LK_FLAGS_PO_LONG_TERM		0x00000080
#define LK_FLAGS_RESTORE_PENDING	0x00000100
#define LK_FLAGS_LIN_SENT		0x00000200
#define LK_FLAGS_LUN_SENT		0x00000400
#define LK_FLAGS_LFU_SENT		0x00000800
#define LK_FLAGS_T12_EXPIRED		0x00001000
#define LK_FLAGS_T13_EXPIRED		0x00002000
#define LK_FLAGS_T14_EXPIRED		0x00004000
#define LK_FLAGS_LIN_PENDING		0x00008000
#define LK_FLAGS_LUN_PENDING		0x00010000
#define LK_FLAGS_SLTM_SENT		0x00020000
#define LK_FLAGS_SSLTM_SENT		0x00040000
#define LK_FLAGS_BSNT_RETRIEVED		0x00080000
#define LK_FLAGS_FSNC_PROVIDED		0x00100000
#define LK_FLAGS_UPDATING		0x00200000
#define LK_FLAGS_CO_RCVD		0x00400000
#define LK_FLAGS_CO_SENT		0x00800000
#define LK_FLAGS_COO_PENDING		0x01000000
#define LK_FLAGS_COA_PENDING		0x02000000
#define LK_FLAGS_TIME_CTRL_CO		0x04000000	/* time-controlled changeover in progress */
#define LK_FLAGS_CHANGEOVER		0x08000000	/* changeover in progress */
#define LK_FLAGS_TRAFFIC		0x10000000
#define LK_FLAGS_1ST_SLT		0x20000000
#define LK_FLAGS_2ND_SLT		0x40000000

#define LK_FLAGS_UNAVAILABLE	(LK_FLAGS_INACTIVE \
					|LK_FLAGS_BLOCKED \
					|LK_FLAGS_INHIBITED)
#define LK_FLAGS_CO_FLAGS	(LK_FLAGS_BSNT_RETRIEVED \
					|LK_FLAGS_FSNC_PROVIDED \
					|LK_FLAGS_UPDATING \
					|LK_FLAGS_CO_RCVD \
					|LK_FLAGS_CO_SENT \
					|LK_FLAGS_COO_PENDING \
					|LK_FLAGS_COA_PENDING \
					|LK_FLAGS_TIME_CTRL_CO \
					|LK_FLAGS_CHANGEOVER)

#define LK_IOCGSTATEM	_IOR(  LK_IOC_MAGIC, 6, lk_statem_t )
#define LK_IOCCMRESET	_IOR(  LK_IOC_MAGIC, 7, lk_statem_t )

/*
 *  -----------------------------------------------------------------------
 *
 *  STATISTICS
 *
 *  -----------------------------------------------------------------------
 */

typedef struct lk_stats {
	lmi_sta_t header;
	 /**/ ls_ulong lk_dur_in_service;	/* Q.752 Table 1.1 30 min */
	ls_ulong lk_fail_align_or_proving;	/* Q.752 Table 1.7 5,30 min */
	ls_ulong lk_nacks_received;	/* Q.752 Table 1.9 5,30 min */
	ls_ulong lk_dur_unavail;	/* Q.752 Table 2.1 30 min */
	ls_ulong lk_dur_unavail_failed;	/* Q.752 Table 2.7 30 min */
	ls_ulong lk_dur_unavail_rpo;	/* Q.752 Table 2.9 30 min */
	ls_ulong lk_sibs_sent;		/* Q.752 Table 2.15 5,30 min */
	ls_ulong lk_tran_sio_sif_octets;	/* Q.752 Table 3.1 5,30 min *//* trasmitted first time */
	ls_ulong lk_retrans_octets;	/* Q.752 Table 3.2 5,30 min *//* includes opening flag and crc */
	ls_ulong lk_tran_msus;		/* Q.752 Table 3.3 5,30 min *//* transmitted first time */
	ls_ulong lk_recv_sio_sif_octets;	/* Q.752 Table 3.4 5,30 min */
	ls_ulong lk_recv_msus;		/* Q.752 Table 3.5 5,30 min *//* passed to L3 */
	ls_ulong lk_cong_onset_ind[4];	/* Q.752 Table 3.6 5,30 min *//* and 3.9 */
	ls_ulong lk_dur_cong_level[4];	/* Q.752 Table 3.7 30 min *//* rising only */
	/*
	   ls_ulong lk_msus_cong_discard; Q.752 Table 3.10 5,30 min 
 *//*
   Not here in MTP or SLS 
 */
	ls_ulong lk_cong_discd_ind[4];	/* Q.752 Table 3.11 5,30 min *//* rising only when < onset */
} lk_stats_t;
					    /*
					       FIXME: read 3.11 and recheck congestion procedures 
					     */

#define LK_IOCGSTATSP	_IOR(  LK_IOC_MAGIC,  8, lmi_sta_t )
#define LK_IOCSSTATSP	_IORW( LK_IOC_MAGIC,  9, lmi_sta_t )
#define LK_IOCGSTATS	_IOR(  LK_IOC_MAGIC, 10, lk_stats_t )
#define LK_IOCCSTATS	_IOW(  LK_IOC_MAGIC, 11, lk_stats_t )

/*
 *  NOTIFICATIONS
 */

typedef struct lk_notify {
	ls_ulong events;
} lk_notify_t;

#define LK_EVENT_HEADER_LEN		16	/* for now */

#define LK_EVT_FAIL_UNSPECIFIED		LK_FAIL_UNSPECIFIED
#define LK_EVT_FAIL_CONG_TIMEOUT	LK_FAIL_CONG_TIMEOUT	/* Q.752 Table 1.6 */
#define LK_EVT_FAIL_ACK_TIMEOUT		LK_FAIL_ACK_TIMEOUT	/* Q.752 Table 1.4 */
#define LK_EVT_FAIL_ABNORMAL_BSNR	LK_FAIL_ABNORMAL_BSNR
#define LK_EVT_FAIL_ABNORMAL_FIBR	LK_FAIL_ABNORMAL_FIBR
#define LK_EVT_FAIL_ABORMAL		LK_FAIL_ABNORMAL_BSNR | \
					LK_FAIL_ABNORMAL_FIBR	/* Q.752 Table 1.3 */
#define LK_EVT_FAIL_SUERM_EIM		LK_FAIL_SUERM_EIM	/* Q.752 Table 1.5 */
#define LK_EVT_FAIL_ALIGNMENT		LK_FAIL_ALIGNMENT_NOT_POSSIBLE
#define LK_EVT_FAIL_RECEIVED_SIO	LK_FAIL_RECEIVED_SIO
#define LK_EVT_FAIL_RECEIVED_SIN	LK_FAIL_RECEIVED_SIN
#define LK_EVT_FAIL_RECEIVED_SIE	LK_FAIL_RECEIVED_SIE
#define LK_EVT_FAIL_RECEIVED_SIOS	LK_FAIL_RECEIVED_SIOS
#define LK_EVT_FAIL_T1_TIMEOUT		LK_FAIL_T1_TIMEOUT
#define LK_EVT_RPO_BEGIN		LK_FAIL_T1_TIMEOUT      <<1	/* Q.752 Table 2.10 */
#define LK_EVT_RPO_END			LK_EVT_RPO_BEGIN        <<1	/* Q.752 Table 2.11 */
#define LK_EVT_CONGEST_ONSET_IND	LK_EVT_RPO_END          <<1	/* Q.752 Table 3.6 1st&D */
#define LK_EVT_CONGEST_DISCD_IND	LK_EVT_CONGEST_ONSET_IND<<1	/* Q.752 Table 3.11 1st&D */

#define LK_EVT_FAIL_ALL_REASONS		 LK_FAIL_UNSPECIFIED \
						|LK_FAIL_CONG_TIMEOUT \
						|LK_FAIL_ACK_TIMEOUT  \
						|LK_FAIL_ABNORMAL_FIBR \
						|LK_FAIL_ABNORMAL_BSNR \
						|LK_FAIL_SUERM_EIM \
						|LK_FAIL_ALIGNMENT_NOT_POSSIBLE \
						|LK_FAIL_RECEIVED_SIO \
						|LK_FAIL_RECEIVED_SIN \
						|LK_FAIL_RECEIVED_SIE \
						|LK_FAIL_RECEIVED_SIOS \
						|LK_FAIL_T1_TIMEOUT	/* Q.752 Table 1.2 */

#define LK_IOCGNOTIFY	_IOR(  LK_IOC_MAGIC, 12, lk_notify_t )
#define LK_IOCSNOTIFY	_IOW(  LK_IOC_MAGIC, 13, lk_notify_t )
#define LK_IOCCNOTIFY	_IOW(  LK_IOC_MAGIC, 14, lk_notify_t )

#define LK_IOC_FIRST     0
#define LK_IOC_LAST     14
#define LK_IOC_PRIVATE  32

#define LL_FLAGS_STOPPED	0x00000001
#define LL_FLAGS_CHANGEBACK	0x00000002
#define LL_FLAGS_CHANGEOVER	0x00000004

#endif				/* __SLSI_IOCTL_H__ */
