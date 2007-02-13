/*****************************************************************************

 @(#) $Id: tcap_ioctl.h,v 0.9.2.5 2007/02/13 14:05:29 brian Exp $

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

 Last Modified $Date: 2007/02/13 14:05:29 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tcap_ioctl.h,v $
 Revision 0.9.2.5  2007/02/13 14:05:29  brian
 - corrected ulong and long for 32-bit compat

 *****************************************************************************/

#ifndef __SSCP_IOCTL_H__
#define __SSCP_IOCTL_H__

#ident "@(#) $RCSfile: tcap_ioctl.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>

#define TCAP_IOC_MAGIC 'T'

#define TCAP_OBJ_TYPE_DF	0	/* default */
#define TCAP_OBJ_TYPE_TC	1	/* component */
#define TCAP_OBJ_TYPE_IV	2	/* invoke */
#define TCAP_OBJ_TYPE_DG	3	/* dialog */
#define TCAP_OBJ_TYPE_TR	4	/* transaction */
#define TCAP_OBJ_TYPE_SP	5	/* SCCP SAP */
#define TCAP_OBJ_TYPE_SC	6	/* SCCP */

/*
 *  Component configuration
 *  -----------------------------------
 */
typedef struct tcap_conf_tc {
} tcap_conf_tc_t;
typedef struct tcap_opt_conf_tc {
} tcap_opt_conf_tc_t;

/*
 *  Invoke configuration
 *  -----------------------------------
 */
typedef struct tcap_conf_iv {
} tcap_conf_iv_t;
typedef struct tcap_opt_conf_iv {
} tcap_opt_conf_iv_t;

/*
 *  Dialogue configuration
 *  -----------------------------------
 */
typedef struct tcap_conf_dg {
} tcap_conf_dg_t;
typedef struct tcap_opt_conf_dg {
} tcap_opt_conf_dg_t;

/*
 *  Transaction configuration
 *  -----------------------------------
 */
typedef struct tcap_conf_tr {
} tcap_conf_tr_t;
typedef struct tcap_opt_conf_tr {
} tcap_opt_conf_tr_t;

/*
 *  SCCP SAP configuration
 *  -----------------------------------
 */
typedef struct tcap_conf_sp {
} tcap_conf_sp_t;
typedef struct tcap_opt_conf_sp {
} tcap_opt_conf_sp_t;

/*
 *  SCCP configuration
 *  -----------------------------------
 */
typedef struct tcap_conf_sc {
} tcap_conf_sc_t;
typedef struct tcap_opt_conf_sc {
} tcap_opt_conf_sc_t;

/*
 *  Default configuration
 *  -----------------------------------
 */
typedef struct tcap_conf_df {
} tcap_conf_df_t;
typedef struct tcap_opt_conf_df {
} tcap_opt_conf_df_t;

/*
 *  OPTIONS
 */
typedef struct tcap_option {
	t_uscalar_t type;		/* object type */
	t_uscalar_t id;			/* object id */
	/* 
	   followed by specific proto structure */
} tcap_option_t;

#define TCAP_IOCGOPTIONS	_IOR(	TCAP_IOC_MAGIC,	 0, tcap_option_t   )
#define TCAP_IOCSOPTIONS	_IOR(	TCAP_IOC_MAGIC,	 1, tcap_option_t   )

/*
 *  CONFIGURATION
 */
typedef struct tcap_config {
	t_uscalar_t type;		/* object type */
	t_uscalar_t id;			/* object id */
	t_uscalar_t cmd;		/* object command */
	/* 
	   followed by specific configuration structure */
} tcap_config_t;

#define TCAP_GET	0
#define TCAP_ADD	1
#define TCAP_CHA	2
#define TCAP_DEL	3

#define TCAP_IOCGCONFIG		_IOWR(	TCAP_IOC_MAGIC,	 2, tcap_config_t   )
#define TCAP_IOCSCONFIG		_IOWR(	TCAP_IOC_MAGIC,	 3, tcap_config_t   )
#define TCAP_IOCTCONFIG		_IOWR(	TCAP_IOC_MAGIC,	 4, tcap_config_t   )
#define TCAP_IOCCCONFIG		_IOWR(	TCAP_IOC_MAGIC,	 5, tcap_config_t   )

/*
 *  STATE
 */
typedef struct tcap_timers_tc {
} tcap_timers_tc_t;
typedef struct tcap_statem_tc {
	tcap_timers_tc_t timers;
} tcap_statem_tc_t;

typedef struct tcap_timers_iv {
} tcap_timers_iv_t;
typedef struct tcap_statem_iv {
	tcap_timers_iv_t timers;
} tcap_statem_iv_t;

typedef struct tcap_timers_dg {
} tcap_timers_dg_t;
typedef struct tcap_statem_dg {
	tcap_timers_dg_t timers;
} tcap_statem_dg_t;

typedef struct tcap_timers_tr {
} tcap_timers_tr_t;
typedef struct tcap_statem_tr {
	tcap_timers_tr_t timers;
} tcap_statem_tr_t;

typedef struct tcap_timers_sp {
} tcap_timers_sp_t;
typedef struct tcap_statem_sp {
	tcap_timers_sp_t timers;
} tcap_statem_sp_t;

typedef struct tcap_timers_sc {
} tcap_timers_sc_t;
typedef struct tcap_statem_sc {
	tcap_timers_sc_t timers;
} tcap_statem_sc_t;

typedef struct tcap_timers_df {
} tcap_timers_df_t;
typedef struct tcap_statem_df {
	tcap_timers_df_t timers;
} tcap_statem_df_t;

/*
 *  STATISTICS
 */
typedef struct tcap_stats {
	t_uscalar_t type;		/* object type */
	t_uscalar_t id;			/* object id */
	t_uscalar_t header;		/* object stats header */
	/* 
	   followed by object specific stats structure */
} tcap_stats_t;

typedef struct tcap_stats_tc {
} tcap_stats_tc_t;

typedef struct tcap_stats_iv {
} tcap_stats_iv_t;

typedef struct tcap_stats_dg {
} tcap_stats_dg_t;

typedef struct tcap_stats_tr {
} tcap_stats_tr_t;

typedef struct tcap_stats_sp {
} tcap_stats_sp_t;

typedef struct tcap_stats_sc {
} tcap_stats_sc_t;

typedef struct tcap_stats_df {
} tcap_stats_df_t;

#define TCAP_IOCGSTATSP		_IOWR(	TCAP_IOC_MAGIC,  8,  tcap_stats_t )
#define TCAP_IOCSSTATSP		_IOWR(	TCAP_IOC_MAGIC,  9,  tcap_stats_t )
#define TCAP_IOCGSTATS		_IOWR(	TCAP_IOC_MAGIC, 10,  tcap_stats_t )
#define TCAP_IOCCSTATS		_IOWR(	TCAP_IOC_MAGIC, 11,  tcap_stats_t )

/*
 *  EVENTS
 */
typedef struct tcap_notify {
	t_uscalar_t type;		/* object type */
	t_uscalar_t id;			/* object id */
	lmi_notify_t notify;		/* notifications */
} tcap_notify_t;

#define TCAP_IOCGNOTIFY		_IOR(	TCAP_IOC_MAGIC, 12,  tcap_notify_t )
#define TCAP_IOCSNOTIFY		_IOW(	TCAP_IOC_MAGIC, 13,  tcap_notify_t )
#define TCAP_IOCCNOTIFY		_IOW(	TCAP_IOC_MAGIC, 14,  tcap_notify_t )

typedef struct tcap_notify_tc {
} tcap_notify_tc_t;
typedef struct tcap_notify_iv {
} tcap_notify_iv_t;
typedef struct tcap_notify_dg {
} tcap_notify_dg_t;
typedef struct tcap_notify_tr {
} tcap_notify_tr_t;
typedef struct tcap_notify_sp {
} tcap_notify_sp_t;
typedef struct tcap_notify_sc {
} tcap_notify_sc_t;
typedef struct tcap_notify_df {
} tcap_notify_df_t;

/*
 *  MANAGEMENT
 */
typedef struct tcap_mgmt {
	t_uscalar_t type;		/* object type */
	t_uscalar_t id;			/* object id */
	t_uscalar_t cmd;		/* command */
} tcap_mgmt_t;

#define TCAP_MGMT_BLOCK			1
#define TCAP_MGMT_UNBLOCK		2
#define TCAP_MGMT_RESET			3
#define TCAP_MGMT_QUERY			4

#define TCAP_IOCCMGMT		_IOWR(	TCAP_IOC_MAGIC, 15,  tcap_mgmt_t )

/*
 *  CONTROL LOWER
 */
typedef struct tcap_pass {
	t_uscalar_t muxid;		/* mux index of lower MTP to pass message to */
	t_uscalar_t type;		/* type of message block */
	t_uscalar_t band;		/* band of message block */
	t_uscalar_t ctl_length;		/* length of cntl part */
	t_uscalar_t dat_length;		/* length of data part */
	/* 
	   followed by cntl and data part of message to pass to MTP */
} tcap_pass_t;

#define TCAP_IOCCPASS		_IOWR(	TCAP_IOC_MAGIC, 16,  tcap_pass_t )

#define TCAP_IOC_FIRST		 0
#define TCAP_IOC_LAST		16
#define TCAP_IOC_PRIVATE	32

#endif				/* __SSCP_IOCTL_H__ */
