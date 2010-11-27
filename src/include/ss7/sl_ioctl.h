/*****************************************************************************

 @(#) $Id: sl_ioctl.h,v 1.1.2.1 2009-06-21 11:25:34 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2009-06-21 11:25:34 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sl_ioctl.h,v $
 Revision 1.1.2.1  2009-06-21 11:25:34  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SL_IOCTL_H__
#define __SL_IOCTL_H__

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

#define SL_OBJ_TYPE_DF	    0	    /* default */
#define SL_OBJ_TYPE_ST	    1	    /* signalling terminal */
#define SL_OBJ_TYPE_SD	    2	    /* signalling data link */
#define SL_OBJ_TYPE_SL	    3	    /* signalling link */

/*
 * INFORMATION
 *
 * Provides read-only information that is not available for the manager to
 * configure.  Also, this information does not change while the object exists:
 * it is purely static for the lifetime of the object.
 */
typedef struct sl_info_df {
} sl_info_df_t;
typedef struct sl_info_st {
} sl_info_st_t;
typedef struct sl_info_sd {
} sl_info_sd_t;
typedef struct sl_info_sl {
} sl_info_sl_t;

typedef union sl_info_obj {
	struct sl_info_df df;
	struct sl_info_st st;
	struct sl_info_sd sd;
	struct sl_info_sl sl;
} sl_info_obj_t;

typedef struct sl_info {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union sl_info_obj info[0];
} sl_info_t;

#define SL_IOCGINFO	_IOWR(SL_IOC_MAGIC,	 0, sl_info_t)	/* get */

/*
 * OPTIONS
 *
 * Provides options not necessary for configuration but that can be changed
 * while the object exists without reconfiguration of the object (that is, a
 * change to these items does not change the relationship between the object
 * changed and other objects).
 *
 * Note that one of these option structures can optionally be appended to a
 * configuration structure when an object is created or moved.
 */
typedef struct sl_opt_conf_df {
	lmi_option_t opt; /* protocol variant and options */
} sl_opt_conf_df_t;
typedef struct sl_opt_conf_st {
} sl_opt_conf_st_t;
typedef struct sl_opt_conf_sd {
} sl_opt_conf_sd_t;
typedef struct sl_opt_conf_sl {
	lmi_option_t opt; /* protocol variant and options */
} sl_opt_conf_sl_t;

typedef union sl_option_obj {
	struct sl_opt_conf_df df;
	struct sl_opt_conf_st st;
	struct sl_opt_conf_sd sd;
	struct sl_opt_conf_sl sl;
} sl_option_obj_t;

typedef struct sl_option {
	uint type;			/* object type */
	uint id;			/* object id */
	/* followed by specific proto structure */
	union sl_option_obj options[0];
} sl_option_t;

#define SL_IOCGOPTION	_IOWR(SL_IOC_MAGIC,	 1, sl_option_t)	/* get */
#define SL_IOCSOPTION	_IOWR(SL_IOC_MAGIC,	 2, sl_option_t)	/* set */

/*
 * CONFIGURATION
 *
 * Configures the object (creates, destroys, moves).  These actions require
 * reconfiguration of the object and a change in its relationship to other
 * objects.  Also, this structure contains information that must be specified
 * during creation of the object.  Each structure can optionally be followed by
 * an options structure to specify the options during creation.
 */
typedef struct sl_conf_df {
} sl_conf_df_t;
typedef struct sl_conf_st {
} sl_conf_st_t;
typedef struct sl_conf_sd {
} sl_conf_sd_t;
typedef struct sl_conf_sl {
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
} sl_conf_sl_t;

typedef union sl_conf_obj {
	struct sl_conf_df df;
	struct sl_conf_st st;
	struct sl_conf_sd sd;
	struct sl_conf_sl sl;
} sl_conf_obj_t;

#define SL_GET		0	/* get options or configuration */
#define SL_ADD		1	/* add configuration */
#define SL_CHA		2	/* set options or change configuration */
#define SL_DEL		3	/* delete configuraiton */
#define SL_TST		4	/* test options or configuration */
#define SL_COM		5	/* commit options or configuration */

typedef struct sl_config {
	uint type;			/* object type */
	uint id;			/* object id */
	uint cmd;			/* object command */
	/* followed by specific configuration structure */
	union sl_conf_obj config[0];
} sl_config_t;

#define SL_IOCLCONFIG	_IOWR(SL_IOC_MAGIC,	 3, sl_config_t)	/* list */
#define SL_IOCGCONFIG	_IOWR(SL_IOC_MAGIC,	 4, sl_config_t)	/* get */
#define SL_IOCSCONFIG	_IOWR(SL_IOC_MAGIC,	 5, sl_config_t)	/* set */
#define SL_IOCTCONFIG	_IOWR(SL_IOC_MAGIC,	 6, sl_config_t)	/* test */
#define SL_IOCCCONFIG	_IOWR(SL_IOC_MAGIC,	 7, sl_config_t)	/* commit */

/*
 * STATE
 *
 * Provides state machine state information.  The purpose of these items is
 * primarily for debugging: to view the internal state of the state machine.
 * Some of these state variables might also be reflected in status fields.
 */
typedef struct sl_timers_df {
} sl_timers_df_t;
typedef struct sl_timers_st {
} sl_timers_st_t;
typedef struct sl_timers_sd {
} sl_timers_sd_t;
typedef struct sl_timers_sl {
	uint t1;			/* T1 timer */
	uint t2;			/* T2 timer */
	uint t3;			/* T3 timer */
	uint t4;			/* T4 timer */
	uint t5;			/* T5 timer */
	uint t6;			/* T6 timer */
	uint t7;			/* T7 timer */
} sl_timers_sl_t;

typedef struct sl_statem_df {
	struct sl_timers_df timers;
} sl_statem_df_t;
typedef struct sl_statem_st {
	struct sl_timers_st timers;
} sl_statem_st_t;
typedef struct sl_statem_sd {
	struct sl_timers_sd timers;
} sl_statem_sd_t;
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

typedef struct sl_statem_sl {
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
	struct sl_timers_sl timers;
} sl_statem_sl_t;
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

typedef union sl_statem_obj {
	struct sl_statem_df df;
	struct sl_statem_st st;
	struct sl_statem_sd sd;
	struct sl_statem_sl sl;
} sl_statem_obj_t;

typedef struct sl_status {
	uint type;			/* object type */
	uint id;			/* object identifier */
	/* followed by object-specific structure */
	union sl_status_obj status[0];
} sl_status_t;

#define SL_IOCGSTATUS	_IOWR(SL_IOC_MAGIC,	10, sl_status_t)	/* get */
#define SL_IOCSSTATUS	_IOWR(SL_IOC_MAGIC,	11, sl_status_t)	/* set */
#define SL_IOCCSTATUS	_IOWR(SL_IOC_MAGIC,	12, sl_status_t)	/* clear */

/*
 * STATISTICS
 */
typedef struct sl_stats_df {
} sl_stats_df_t;
typedef struct sl_stats_st {
} sl_stats_st_t;
typedef struct sl_stats_sd {
} sl_stats_sd_t;
typedef struct sl_stats_sl {
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
	   sl_ulong sl_msus_cong_discard; Q.752 Table 3.10 5,30 min - Not here in MTP or SLS */
	sl_ulong sl_cong_discd_ind[4];	/* Q.752 Table 3.11 5,30 min - rising only when < onset */
	/* FIXME: read 3.11 and recheck congestion procedures */
} sl_stats_sl_t;

typedef union sl_stats_obj {
	struct sl_stats_df df;
	struct sl_stats_st st;
	struct sl_stats_sd sd;
	struct sl_stats_sl sl;
} sl_stats_obj_t;

typedef struct sl_stats {
	uint type;			/* object type */
	uint id;			/* object id */
	uint header;			/* object stats header */
	/* followed by object-specific statistics structure */
	union sl_stats_obj stats[0];
} sl_stats_t;

#define SL_IOCGSTATSP	_IOWR(SL_IOC_MAGIC,	 8,  sl_stats_t)	/* get period */
#define SL_IOCSSTATSP	_IOWR(SL_IOC_MAGIC,	 9,  sl_stats_t)	/* set period */
#define SL_IOCGSTATS	_IOWR(SL_IOC_MAGIC,	10,  sl_stats_t)	/* get stats */
#define SL_IOCCSTATS	_IOWR(SL_IOC_MAGIC,	11,  sl_stats_t)	/* get and clear stats */

/*
 * EVENTS
 *
 * Provides a mechanism for requesting event notifications.  The flags field in
 * the header is used to requesting X.721 events (e.g. object creation
 * notification).  Events can be requested by object class, and a mechanism
 * exists for requesting events by specific object; however, the driver is not
 * required to support per-object notification and will return EOPNOTSUPP.  To
 * request notification for an object class, specify zero (0) for the
 * object identifier.
 *
 * Event requests are per requesting Stream.  Non-management streams should only
 * be notified of events for objects associated with the Stream and not X.721
 * events unless also specified as a specific event (e.g. communications alarm
 * events).  Management Streams can be notified of any object.  Notifications
 * are provided using the LMI_EVENT_IND (event indication) for X.721 events, and
 * the LMI_EVENT_IND (for other events).
 */
typedef struct sl_notify_df {
	uint events;
} sl_notify_df_t;
typedef struct sl_notify_st {
	uint events;
} sl_notify_st_t;
typedef struct sl_notify_sd {
	uint events;
} sl_notify_sd_t;
typedef struct sl_notify_sl {
	uint events;
} sl_notify_sl_t;
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


typedef union sl_notify_obj {
	struct sl_notify_df df;
	struct sl_notify_st st;
	struct sl_notify_sd sd;
	struct sl_notify_sl sl;
} sl_notify_obj_t;

typedef struct sl_notify {
	uint type;			/* object type */
	uint id;			/* object id */
	/* followed by object-specific notification structure */
	union sl_notify_obj notify[0];
} sl_notify_t;

#define SL_IOCGNOTIFY	_IOWR(SL_IOC_MAGIC, 17,  sl_notify_t)	/* get */
#define SL_IOCSNOTIFY	_IOWR(SL_IOC_MAGIC, 18,  sl_notify_t)	/* set */
#define SL_IOCCNOTIFY	_IOWR(SL_IOC_MAGIC, 19,  sl_notify_t)	/* clear */

/*
 * ATTRIBUTES
 *
 * Provides a structure that contains all of the other information structures
 * pertaining to an object.  This is the total view of the object.  This 
 * structure can only be read and cannot be used to reconfigure or change
 * options associated with an object.  This structure is primarily for the
 * convenience of management agents so that it is possible to atomically read
 * all of the attributes associated with an object.
 */
typedef struct sl_attr_df {
	struct sl_conf_df config;
	struct sl_opt_conf_df option;
	struct sl_info_df inform;
	struct sl_statem_df statem;
	struct sl_status_df status;
	struct sl_stats_df stats;
	struct sl_notify_df events;
} sl_attr_df_t;
typedef struct sl_attr_st {
	struct sl_conf_st config;
	struct sl_opt_conf_st option;
	struct sl_info_st inform;
	struct sl_statem_st statem;
	struct sl_status_st status;
	struct sl_stats_st stats;
	struct sl_notify_st events;
} sl_attr_st_t;
typedef struct sl_attr_sd {
	struct sl_conf_sd config;
	struct sl_opt_conf_sd option;
	struct sl_info_sd inform;
	struct sl_statem_sd statem;
	struct sl_status_sd status;
	struct sl_stats_sd stats;
	struct sl_notify_sd events;
} sl_attr_sd_t;
typedef struct sl_attr_sl {
	struct sl_conf_sl config;
	struct sl_opt_conf_sl option;
	struct sl_info_sl inform;
	struct sl_statem_sl statem;
	struct sl_status_sl status;
	struct sl_stats_sl stats;
	struct sl_notify_sl events;
} sl_attr_sl_t;

typedef union sl_attr_obj {
	struct sl_attr_df df;
	struct sl_attr_st st;
	struct sl_attr_sd sd;
	struct sl_attr_sl sl;
} sl_attr_obj_t;

typedef struct sl_attr {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union sl_attr_obj attrs[0];
} sl_attr_t;

#define	SL_IOCGATTR	_IOWR(SL_IOC_MAGIC,	20, sl_attr_t)	/* get attributes */

/*
 *  MANAGEMENT
 */
typedef struct sl_action_df {
} sl_action_df_t;
typedef struct sl_action_st {
} sl_action_st_t;
typedef struct sl_action_sd {
} sl_action_sd_t;
typedef struct sl_action_sl {
} sl_action_sl_t;

typedef union sl_action_obj {
	struct sl_action_df df;
	struct sl_action_st st;
	struct sl_action_sd sd;
	struct sl_action_sl sl;
} sl_action_obj_t;

typedef struct sl_mgmt {
	uint type;			/* object type */
	uint id;			/* object id */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union sl_action_obj action[0];
} sl_mgmt_t;

#define SL_MGMT_BLOCK		1
#define SL_MGMT_UNBLOCK		2
#define SL_MGMT_RESET		3
#define SL_MGMT_QUERY		4

#define SL_IOCCMGMT	_IOWR(SL_IOC_MAGIC,	21, sl_mgmt_t)	/* command */

/*
 * PASS LOWER
 *
 * This structure is deprecated and as largely used for testing as a mechanism
 * to bypass an input-output control to a stream linked under a multiplexing
 * driver supporting this interface.
 */
typedef struct sl_pass {
	uint muxid;			/* mux index of lower stream to pass message to */
	uint8_t type;			/* type of message block */
	uint8_t band;			/* band of message block */
	uint ctl_length;		/* length of cntl part */
	uint dat_length;		/* length of data part */
	/* followed by cntl and data part of message to pass */
} sl_pass_t;

#define SL_IOCCPASS	_IOWR(SL_IOC_MAGIC,	22, sl_pass_t)	/* pass */

union sl_ioctls {
	struct {
		struct sl_info info;
		union sl_info_obj obj;
	} info;
	struct {
		struct sl_option option;
		union sl_option_obj obj;
	} opt_conf;
	struct {
		struct sl_config config;
		union sl_conf_obj obj;
	} conf;
	struct {
		struct sl_statem statem;
		union sl_statem_obj obj;
	} statem;
	struct {
		struct sl_status status;
		union sl_status_obj obj;
	} status;
	struct {
		struct sl_stats stats;
		union sl_stats_obj obj;
	} stats;
	struct {
		struct sl_notify notify;
		union sl_notify_obj obj;
	} notify;
	struct {
		struct sl_attr attr;
		union sl_attr_obj obj;
	} attr;
	struct {
		struct sl_mgmt mgmt;
		union sl_action_obj obj;
	} mgmt;
	struct sl_pass pass;
};

#define SL_IOC_FIRST     0
#define SL_IOC_LAST     22
#define SL_IOC_PRIVATE  32

#endif				/* __SL_IOCTL_H__ */
