/*****************************************************************************

 @(#) $Id: sccpi_ioctl.h,v 0.9 2004/01/17 08:08:12 brian Exp $

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/01/17 08:08:12 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SCCPI_IOCTL_H__
#define __SCCPI_IOCTL_H__

#ident "@(#) $RCSfile: sccpi_ioctl.h,v $ $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2003 OpenSS7 Corporation."

#include <linux/ioctl.h>

#define SCCP_IOC_MAGIC 's'

#define SCCP_OBJ_TYPE_DF	    0	/* Default */
#define SCCP_OBJ_TYPE_SC	    1	/* SCCP User */
#define SCCP_OBJ_TYPE_NA	    2	/* Network Appearance */
#define SCCP_OBJ_TYPE_CP	    3	/* Coupling */
#define SCCP_OBJ_TYPE_SS	    4	/* Local Subsystem */
#define SCCP_OBJ_TYPE_RS	    5	/* Remote Subsystem */
#define SCCP_OBJ_TYPE_SR	    6	/* Signalling Relation */
#define SCCP_OBJ_TYPE_SP	    7	/* Signalling Point */
#define SCCP_OBJ_TYPE_MT	    8	/* Message Transfer Part */

/*
 *  Mesage transfer part options
 */
typedef struct sccp_opt_conf_mt {
} sccp_opt_conf_mt_t;
/*
 *  Signalling point options
 */
typedef struct sccp_opt_conf_sp {
	ulong tgtt;
} sccp_opt_conf_sp_t;
/*
 *  Signalling relation options
 */
typedef struct sccp_opt_conf_sr {
	ulong tattack;			/* T(attack) timer */
	ulong tdecay;			/* T(decay) timer */
	ulong tstatinfo;		/* T(stat_info) timer */
	ulong tsst;			/* T(sst) timer */
} sccp_opt_conf_sr_t;
/*
 *  Remote subsystem options
 */
typedef struct sccp_opt_conf_rs {
	ulong tsst;			/* T(sst) timer */
} sccp_opt_conf_rs_t;
/*
 *  Coupling options
 */
typedef struct sccp_opt_conf_cp {
} sccp_opt_conf_cp_t;
/*
 *  Local subsystem options
 */
typedef struct sccp_opt_conf_ss {
	ulong tisst;
	ulong twsog;
} sccp_opt_conf_ss_t;
/*
 *  Network appearance options
 */
typedef struct sccp_opt_conf_na {
} sccp_opt_conf_na_t;
/*
 *  SCCP user options
 */
typedef struct sccp_opt_conf_sc {
	ulong tcon;
	ulong tias;
	ulong tiar;
	ulong trel;
	ulong trel2;
	ulong tint;
	ulong tguard;
	ulong tres;
	ulong trea;
	ulong tack;
} sccp_opt_conf_sc_t;
/*
 *  Default options
 */
typedef struct sccp_opt_conf_df {
} sccp_opt_conf_df_t;

/*
 *  OPTIONS
 */
typedef struct sccp_option {
	ulong type;			/* object type */
	ulong id;			/* object id */
	/*
	   followed by object-specific protocol options structure 
	 */
} sccp_option_t;

#define SCCP_IOCGOPTION	_IOWR(	SCCP_IOC_MAGIC,	 0,	sccp_option_t	)
#define SCCP_IOCSOPTION	_IOWR(	SCCP_IOC_MAGIC,	 1,	sccp_option_t	)

/*
 *  Mesage transfer part configuration
 */
typedef struct sccp_conf_mt {
	ulong spid;			/* local signalling point identifier */
	ulong srid;			/* remote signalling point identifier */
	ulong muxid;			/* lower multipleding driver id */
	lmi_option_t proto;		/* protocol variant and options */
} sccp_conf_mt_t;
/*
 *  Signalling point configuration
 */
typedef struct sccp_conf_sp {
	ulong naid;			/* network appearance id */
	struct mtp_addr add;		/* local signalling point code */
	lmi_option_t proto;		/* protocol variant and options */
} sccp_conf_sp_t;
/*
 *  Signalling relation configuration
 */
typedef struct sccp_conf_sr {
	ulong spid;			/* local signalling point identifier */
	struct mtp_addr add;		/* remote signalling point code */
	lmi_option_t proto;		/* protocol variant and options */
} sccp_conf_sr_t;
/*
 *  Remote subsystem configuration
 */
typedef struct sccp_conf_rs {
	ulong srid;			/* remote signalling point identifier */
	ulong ssn;			/* subsystem number */
} sccp_conf_rs_t;
/*
 *  Coupling configuration
 */
typedef struct sccp_conf_cp {
} sccp_conf_cp_t;
/*
 *  Local subsystem configuration
 */
typedef struct sccp_conf_ss {
	ulong spid;			/* local signalling point identifier */
	ulong ssn;			/* subsystem number */
} sccp_conf_ss_t;
/*
 *  Network appearance configuration
 */
typedef struct sccp_conf_na {
	lmi_option_t proto;		/* protocol variant and options */
} sccp_conf_na_t;
/*
 *  SCCP user configuration
 */
typedef struct sccp_conf_sc {
} sccp_conf_sc_t;
/*
 *  Default configuration
 */
typedef struct sccp_conf_df {
	lmi_option_t proto;		/* protocol variant and options */
} sccp_conf_df_t;

/*
   additional SCCP protocol options 
 */

/*
 *  CONFIGURATION
 */
typedef struct sccp_config {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong cmd;			/* configuration command */
	/*
	   followed by object-specific configuration structure 
	 */
} sccp_config_t;

#define SCCP_GET	0	/* get configuration */
#define SCCP_ADD	1	/* add configuration */
#define SCCP_CHA	2	/* cha configuration */
#define SCCP_DEL	3	/* del configuration */

#define	SCCP_IOCGCONFIG	_IOWR(	SCCP_IOC_MAGIC,	 2,	sccp_config_t	)
#define	SCCP_IOCSCONFIG	_IOWR(	SCCP_IOC_MAGIC,	 3,	sccp_config_t	)
#define	SCCP_IOCTCONFIG	_IOWR(	SCCP_IOC_MAGIC,	 4,	sccp_config_t	)
#define	SCCP_IOCCCONFIG	_IOWR(	SCCP_IOC_MAGIC,	 5,	sccp_config_t	)

/*
 *  Mesage transfer part state
 */
typedef struct sccp_timers_mt {
} sccp_timers_mt_t;
typedef struct sccp_statem_mt {
	struct sccp_timers_mt timers;
} sccp_statem_mt_t;
/*
 *  Signalling point state
 */
typedef struct sccp_timers_sp {
	ulong tgtt;
} sccp_timers_sp_t;
typedef struct sccp_statem_sp {
	struct sccp_timers_sp timers;
} sccp_statem_sp_t;
/*
 *  Signalling relation state
 */
typedef struct sccp_timers_sr {
	ulong tattack;			/* T(attack) timer */
	ulong tdecay;			/* T(decay) timer */
	ulong tstatinfo;		/* T(stat_info) timer */
	ulong tsst;			/* T(sst) timer */
} sccp_timers_sr_t;
typedef struct sccp_statem_sr {
	struct sccp_timers_sr timers;
} sccp_statem_sr_t;
/*
 *  Remote subsystem state
 */
typedef struct sccp_timers_rs {
	ulong tsst;			/* T(sst) timer */
} sccp_timers_rs_t;
typedef struct sccp_statem_rs {
	struct sccp_timers_rs timers;
} sccp_statem_rs_t;
/*
 *  Coupling state
 */
typedef struct sccp_timers_cp {
} sccp_timers_cp_t;
typedef struct sccp_statem_cp {
	struct sccp_timers_cp timers;
} sccp_statem_cp_t;
/*
 *  Local subsystem state
 */
typedef struct sccp_timers_ss {
	ulong tisst;
	ulong twsog;
} sccp_timers_ss_t;
typedef struct sccp_statem_ss {
	struct sccp_timers_ss timers;
} sccp_statem_ss_t;
/*
 *  Network appearance state
 */
typedef struct sccp_timers_na {
} sccp_timers_na_t;
typedef struct sccp_statem_na {
	struct sccp_timers_na timers;
} sccp_statem_na_t;
/*
 *  Network appearance state
 */
typedef struct sccp_timers_sc {
	ulong tcon;
	ulong tias;
	ulong tiar;
	ulong trel;
	ulong trel2;
	ulong tint;
	ulong tguard;
	ulong tres;
	ulong trea;
	ulong tack;
} sccp_timers_sc_t;
typedef struct sccp_statem_sc {
	struct sccp_timers_sc timers;
} sccp_statem_sc_t;
/*
 *  Default state
 */
typedef struct sccp_timers_df {
} sccp_timers_df_t;
typedef struct sccp_statem_df {
	struct sccp_timers_df timers;
} sccp_statem_df_t;

/*
 *  STATE
 */
typedef struct sccp_statem {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong flags;			/* object flags */
	ulong state;			/* object state */
	/*
	   followed by object-specific state structure 
	 */
} sccp_statem_t;

#define	SCCP_IOCGSTATEM	_IOWR(	SCCP_IOC_MAGIC,	 6,	sccp_statem_t	)
#define	SCCP_IOCCMRESET	_IOWR(	SCCP_IOC_MAGIC,	 7,	sccp_statem_t	)

/*
 *  Mesage transfer part statistics
 */
typedef struct sccp_stats_mt {
} sccp_stats_mt_t;
/*
 *  Signalling point statistics
 */
typedef struct sccp_stats_sp {
} sccp_stats_sp_t;
/*
 *  Signalling relation statistics
 */
typedef struct sccp_stats_sr {
} sccp_stats_sr_t;
/*
 *  Remote subsystem statistics
 */
typedef struct sccp_stats_rs {
} sccp_stats_rs_t;
/*
 *  Coupling statistics
 */
typedef struct sccp_stats_cp {
} sccp_stats_cp_t;
/*
 *  Local subsystem statistics
 */
typedef struct sccp_stats_ss {
} sccp_stats_ss_t;
/*
 *  Network appearance statistics
 */
typedef struct sccp_stats_na {
} sccp_stats_na_t;
/*
 *  SCCP user statistics
 */
typedef struct sccp_stats_sc {
} sccp_stats_sc_t;
/*
 *  Default statistics
 */
typedef struct sccp_stats_df {
} sccp_stats_df_t;

/*
 *  STATISTICS
 */
typedef struct sccp_stats {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong header;			/* object stats header */
	/*
	   followed by object-specific statistics structure 
	 */
} sccp_stats_t;

#define	SCCP_IOCGSTATSP	_IOWR(	SCCP_IOC_MAGIC,	 8,	sccp_stats_t	)
#define	SCCP_IOCSSTATSP	_IOWR(	SCCP_IOC_MAGIC,	 9,	sccp_stats_t	)
#define	SCCP_IOCGSTATS	_IOWR(	SCCP_IOC_MAGIC,	10,	sccp_stats_t	)
#define	SCCP_IOCSSTATS	_IOWR(	SCCP_IOC_MAGIC,	11,	sccp_stats_t	)

/*
 *  Mesage transfer part notifications
 */
typedef struct sccp_notify_mt {
	ulong events;
} sccp_notify_mt_t;
/*
 *  Signalling point notifications
 */
typedef struct sccp_notify_sp {
	ulong events;
} sccp_notify_sp_t;
/*
 *  Signalling relation notifications
 */
typedef struct sccp_notify_sr {
	ulong events;
} sccp_notify_sr_t;
/*
 *  Remote subsystem notifications
 */
typedef struct sccp_notify_rs {
	ulong events;
} sccp_notify_rs_t;
/*
 *  Coupling notifications
 */
typedef struct sccp_notify_cp {
	ulong events;
} sccp_notify_cp_t;
/*
 *  Local subsystem notifications
 */
typedef struct sccp_notify_ss {
	ulong events;
} sccp_notify_ss_t;
/*
 *  Network appearance notifications
 */
typedef struct sccp_notify_na {
	ulong events;
} sccp_notify_na_t;
/*
 *  SCCP user notifications
 */
typedef struct sccp_notify_sc {
	ulong events;
} sccp_notify_sc_t;
/*
 *  Default notifications
 */
typedef struct sccp_notify_df {
	ulong events;
} sccp_notify_df_t;

/*
 *  EVENTS
 */
typedef struct sccp_notify {
	ulong type;			/* object type */
	ulong id;			/* object id */
	/*
	   followed by object-specific notification structure 
	 */
} sccp_notify_t;

#define	SCCP_IOCGNOTIFY	_IOWR(	SCCP_IOC_MAGIC,	12,	sccp_notify_t	)
#define	SCCP_IOCSNOTIFY	_IOWR(	SCCP_IOC_MAGIC,	13,	sccp_notify_t	)
#define	SCCP_IOCCNOTIFY	_IOWR(	SCCP_IOC_MAGIC,	14,	sccp_notify_t	)

/*
 *  MANAGEMENT
 */
typedef struct sccp_mgmt {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong cmd;			/* mgmt command */
} sccp_mgmt_t;

#define SCCP_MGMT_ALLOW			 0
#define SCCP_MGMT_RESTRICT		 1
#define SCCP_MGMT_PROHIBIT		 2
#define SCCP_MGMT_ACTIVATE		 3
#define SCCP_MGMT_DEACTIVATE		 4
#define SCCP_MGMT_BLOCK			 5
#define SCCP_MGMT_UNBLOCK		 6
#define SCCP_MGMT_INHIBIT		 7
#define SCCP_MGMT_UNINHIBIT		 8
#define SCCP_MGMT_CONGEST		 9
#define SCCP_MGMT_UNCONGEST		10
#define SCCP_MGMT_DANGER		11
#define SCCP_MGMT_NODANGER		12
#define SCCP_MGMT_RESTART		13
#define SCCP_MGMT_RESTARTED		14

#define	SCCP_IOCCMGMT	_IOW(	SCCP_IOC_MAGIC,	15,	sccp_mgmt_t	)

/*
 *  PASS LOWER
 */
typedef struct sccp_pass {
	ulong muxid;			/* mux index of lower SL structure to pass message to */
	ulong type;			/* type of message block */
	ulong band;			/* band of mesage block */
	ulong ctl_length;		/* length of cntl part */
	ulong dat_length;		/* length of data part */
	/*
	   followed by cntl and data part of message to pass to signalling link 
	 */
} sccp_pass_t;

#define SCCP_IOCCPASS	_IOW(	SCCP_IOC_MAGIC,	16,	sccp_pass_t	)

#define SCCP_IOC_FIRST    0
#define SCCP_IOC_LAST    16
#define SCCP_IOC_PRIVATE 32

#endif				/* __SCCPI_IOCTL_H__ */
