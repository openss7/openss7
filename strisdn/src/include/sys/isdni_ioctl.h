/*****************************************************************************

 @(#) $Id: isdni_ioctl.h,v 0.9.2.3 2007/08/14 07:41:28 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/14 07:41:28 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: isdni_ioctl.h,v $
 Revision 0.9.2.3  2007/08/14 07:41:28  brian
 - GPLv3 header update

 Revision 0.9.2.2  2007/06/17 01:57:01  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __SS7_ISDNI_IOCTL_H__
#define __SS7_ISDNI_IOCTL_H__

#ident "@(#) $RCSfile: isdni_ioctl.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

#define ISDN_IOC_MAGIC 'I'

/*
   protocol options 
 */
#define ISDN_POPT_ACK	0x00000001
#define ISDN_POPT_ENQ	0x00000002
#define ISDN_POPT_CAUSE	0x00000004
#define ISDN_POPT_REL	0x00000008
#define ISDN_POPT_USER	0x00000010
#define ISDN_POPT_REJ	0x00000020

#define ISDN_OBJ_TYPE_DF	0	/* default */
#define ISDN_OBJ_TYPE_CH	1	/* channel */
#define ISDN_OBJ_TYPE_TG	2	/* transmission group */
#define ISDN_OBJ_TYPE_FG	3	/* facility group */
#define ISDN_OBJ_TYPE_EG	4	/* equipment group */
#define ISDN_OBJ_TYPE_XG	5	/* exchange group */
#define ISDN_OBJ_TYPE_DC	6	/* d channel */
#define ISDN_OBJ_TYPE_DL	7	/* data link */

/*
 *  Call specific structures
 *  -----------------------------------
 *  Calls belong to a facility group and a call control interface and call
 *  management interface.  A call has a list of channels which are being used
 *  for the call.
 */
typedef struct isdn_conf_cr {
	ulong fgid;			/* facility group identifier */
	ulong ccid;			/* call control idenitifer */
	ulong cmid;			/* call management identifier */
	/* 
	   followed by a list channel configurations */
} isdn_conf_cr_t;

typedef struct isdn_opt_conf_cr {
} isdn_opt_conf_cr_t;

typedef struct isdn_timers_cr {
	ulong t301;			/* ISDN T301 timer */
	ulong t302;			/* ISDN T302 timer */
	ulong t303;			/* ISDN T303 timer */
	ulong t304;			/* ISDN T304 timer */
	ulong t305;			/* ISDN T305 timer */
	ulong t306;			/* ISDN T306 timer */
	ulong t307;			/* ISDN T307 timer */
	ulong t308;			/* ISDN T308 timer */
	ulong t309;			/* ISDN T309 timer */
	ulong t310;			/* ISDN T310 timer */
	ulong t312;			/* ISDN T312 timer */
	ulong t313;			/* ISDN T313 timer */
	ulong t314;			/* ISDN T314 timer */
	ulong t316;			/* ISDN T316 timer */
	ulong t317;			/* ISDN T317 timer */
	ulong t318;			/* ISDN T318 timer */
	ulong t319;			/* ISDN T319 timer */
	ulong t320;			/* ISDN T320 timer */
	ulong t321;			/* ISDN T321 timer */
	ulong t322;			/* ISDN T322 timer */
} isdn_timers_cr_t;

typedef struct isdn_statem_cr_t {
	isdn_timers_cr_t timers;
	ulong t303_count;		/* count of T303 timeouts */
	ulong t308_count;		/* count of T308 timeouts */
} isdn_statem_cr_t;

typedef struct isdn_stats_cr {
} isdn_stats_cr_t;

/*
 *  Channel specific structures
 *  -----------------------------------
 *  Channels belong to an tranmission group (all channels and data links
 *  sharing the same transmission facility: i.e. PRI span) and a facility
 *  group whose d-channels are responsible for call control for this
 *  channel.
 */
typedef struct isdn_conf_ch {
	ulong fgid;			/* facility group identifier */
	ulong tgid;			/* transmission group identifier */
	ulong ts;			/* timeslot in transmission group */
	/* 
	   followed by a null list */
} isdn_conf_ch_t;

typedef struct isdn_opt_conf_ch {
} isdn_opt_conf_ch_t;

typedef struct isdn_timers_ch {
} isdn_timers_ch_t;

typedef struct isdn_statem_ch {
	isdn_timers_ch_t timers;
	ulong mgm_bind;
	ulong mnt_bind;
	ulong xry_bind;
	ulong icc_bind;
	ulong ogc_bind[0];
} isdn_statem_ch_t;

typedef struct isdn_stats_ch {
} isdn_stats_ch_t;

/*
 *  Transmission Group specific structures
 *  -----------------------------------
 *  Each transmission group belongs to an equipment group (all equipment
 *  connecting the same two endpoints).
 */
typedef struct isdn_conf_tg {
	ulong egid;			/* equipment group identifier */
	lmi_option_t proto;		/* protocol options */
	/* 
	   followed by list of data link configurations */
	/* 
	   followed by list of channel configurations */
} isdn_conf_tg_t;

typedef struct isdn_opt_conf_tg {
} isdn_opt_conf_tg_t;

typedef struct isdn_timers_tg {
} isdn_timers_tg_t;

typedef struct isdn_statem_tg {
	isdn_timers_tg_t timers;
	ulong mgm_actv;
	ulong mgm_bind;
	ulong mnt_bind;
	ulong xry_bind;
	ulong icc_bind;
	ulong ogc_bind[0];
} isdn_statem_tg_t;

typedef struct isdn_stats_tg {
} isdn_stats_tg_t;

/*
 *  Facility Group specific structures
 *  -----------------------------------
 *  Each facility group belongs to an equipment group (all equipment
 *  connecting the same two endpoints).
 */
typedef struct isdn_conf_fg {
	ulong egid;			/* equipment group identifier */
	lmi_option_t proto;		/* protocol options */
	/* 
	   followed by list of data link configurations */
	/* 
	   followed by list of channel configurations */
} isdn_conf_fg_t;

typedef struct isdn_opt_conf_fg {
	ulong t301;			/* ISDN T301 timer */
	ulong t302;			/* ISDN T302 timer */
	ulong t303;			/* ISDN T303 timer */
	ulong t304;			/* ISDN T304 timer */
	ulong t305;			/* ISDN T305 timer */
	ulong t306;			/* ISDN T306 timer */
	ulong t307;			/* ISDN T307 timer */
	ulong t308;			/* ISDN T308 timer */
	ulong t309;			/* ISDN T309 timer */
	ulong t310;			/* ISDN T310 timer */
	ulong t312;			/* ISDN T312 timer */
	ulong t313;			/* ISDN T313 timer */
	ulong t314;			/* ISDN T314 timer */
	ulong t316;			/* ISDN T316 timer */
	ulong t317;			/* ISDN T317 timer */
	ulong t318;			/* ISDN T318 timer */
	ulong t319;			/* ISDN T319 timer */
	ulong t320;			/* ISDN T320 timer */
	ulong t321;			/* ISDN T321 timer */
	ulong t322;			/* ISDN T322 timer */
} isdn_opt_conf_fg_t;

typedef struct isdn_timers_fg {
} isdn_timers_fg_t;

typedef struct isdn_statem_fg {
	isdn_timers_fg_t timers;
	ulong mgm_actv;
	ulong xry_actv;
	ulong mgm_bind;
	ulong mnt_bind;
	ulong xry_bind;
	ulong icc_bind;
	ulong ogc_bind[0];
} isdn_statem_fg_t;

typedef struct isdn_stats_fg {
} isdn_stats_fg_t;

/*
 *  Equipment Group specific structures
 *  -----------------------------------
 */
typedef struct isdn_conf_eg {
	ulong xgid;			/* exchange group identifier */
	lmi_option_t proto;		/* protocol options */
	/* 
	   followed by list of facility group configurations */
	/* 
	   followed by list of transmission group configurations */
} isdn_conf_eg_t;

typedef struct isdn_opt_conf_eg {
} isdn_opt_conf_eg_t;

typedef struct isdn_timers_eg {
} isdn_timers_eg_t;

typedef struct isdn_statem_eg {
	isdn_timers_eg_t timers;
	ulong mgm_actv;
	ulong mgm_bind;
	ulong mnt_bind;
	ulong xry_bind;
	ulong icc_bind;
	ulong ogc_bind[0];
} isdn_statem_eg_t;

typedef struct isdn_stats_eg {
} isdn_stats_eg_t;

/*
 *  Exchange Group specific structures
 *  -----------------------------------
 */
typedef struct isdn_conf_xg {
	lmi_option_t proto;		/* protocol options */
	/* 
	   followed by list of equipment group configurations */
} isdn_conf_xg_t;

typedef struct isdn_opt_conf_xg {
} isdn_opt_conf_xg_t;

typedef struct isdn_timers_xg {
} isdn_timers_xg_t;

typedef struct isdn_statem_xg {
	isdn_timers_xg_t timers;
	ulong mgm_actv;
	ulong mgm_bind;
	ulong mnt_bind;
	ulong xry_bind;
	ulong icc_bind;
	ulong ogc_bind[0];
} isdn_statem_xg_t;

typedef struct isdn_stats_xg {
} isdn_stats_xg_t;

/*
 *  D Channel specific structures
 *  -----------------------------------
 */
typedef struct isdn_conf_dc {
	ulong fgid;			/* facility group controlled by this d channel */
	ulong tgid;			/* transmission group for this d channel */
	ulong ts;			/* timeslot in transmission group */
	struct dl_addr sap;		/* data link address (PPA and SAPI only) */
	lmi_option_t proto;		/* protocol options */
} isdn_conf_dc_t;

typedef struct isdn_timers_dc {
} isdn_timers_dc_t;

typedef struct isdn_opt_conf_dc {
	isdn_timers_dc_t timers;
} isdn_opt_conf_dc_t;

typedef struct isdn_statem_dc {
	isdn_timers_dc_t timers;
	ulong mgm_bind;
	ulong xry_bind;
} isdn_statem_dc_t;

typedef struct isdn_stats_dc {
} isdn_stats_dc_t;

/*
 *  Data Link specific structures
 *  -----------------------------------
 */
typedef struct isdn_conf_dl {
	ulong muxid;			/* lower multiplexer id */
	ulong dcid;			/* d channel for this data link */
	struct dl_addr dlc;		/* data link address (PPA, SAPI and TEI) */
	lmi_option_t proto;		/* protocol options */
} isdn_conf_dl_t;

typedef struct isdn_opt_conf_dl {
} isdn_opt_conf_dl_t;

typedef struct isdn_timers_dl {
} isdn_timers_dl_t;

typedef struct isdn_statem_dl {
	isdn_timers_dl_t timers;
	ulong mgm_bind;
	ulong xry_bind;
} isdn_statem_dl_t;

typedef struct isdn_stats_dl {
} isdn_stats_dl_t;

/*
 *  Default specific structures
 *  -----------------------------------
 */
typedef struct isdn_conf_df {
	lmi_option_t proto;		/* protocol options */
	/* 
	   followed by list of exchange group configurations */
} isdn_conf_df_t;

typedef struct isdn_opt_conf_df {
} isdn_opt_conf_df_t;

typedef struct isdn_timers_df {
} isdn_timers_df_t;

typedef struct isdn_statem_df {
	isdn_timers_df_t timers;
	ulong mgm_bind;
	ulong mnt_bind;
	ulong xry_bind;
	ulong icc_bind;
	ulong ogc_bind[0];
} isdn_statem_df_t;

typedef struct isdn_stats_df {
} isdn_stats_df_t;

/*
 *  OPTIONS
 */
typedef struct isdn_option {
	ulong type;			/* object type */
	ulong id;			/* object id */
	/* 
	   followed by object-specific options structure */
} isdn_option_t;

#define ISDN_IOCGOPTIONS	_IOWR(	ISDN_IOC_MAGIC,	 0,	isdn_option_t	)
#define ISDN_IOCSOPTIONS	_IOWR(	ISDN_IOC_MAGIC,	 1,	isdn_option_t	)

/*
 *  CONFIGURATION
 */
typedef struct isdn_config {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong cmd;			/* object command */
	/* 
	   followed by object-specific configuration structure */
} isdn_config_t;

#define ISDN_GET	0
#define ISDN_ADD	1
#define ISDN_CHA	2
#define ISDN_DEL	3

#define ISDN_IOCGCONFIG		_IOWR(	ISDN_IOC_MAGIC,	 2,	isdn_config_t	)
#define ISDN_IOCSCONFIG		_IOWR(	ISDN_IOC_MAGIC,	 3,	isdn_config_t	)
#define ISDN_IOCTCONFIG		_IOWR(	ISDN_IOC_MAGIC,	 4,	isdn_config_t	)
#define ISDN_IOCCCONFIG		_IOWR(	ISDN_IOC_MAGIC,	 5,	isdn_config_t	)

/*
 *  STATE
 */
typedef struct isdn_statem {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong flags;			/* object primary flags */
	ulong state;			/* object primary state */
	/* 
	   followed by object-specific state structure */
} isdn_statem_t;

#define ISDN_IOCGSTATEM		_IOWR(	ISDN_IOC_MAGIC,	 6,	isdn_statem_t	)
#define ISDN_IOCCMRESET		_IOWR(	ISDN_IOC_MAGIC,	 7,	isdn_statem_t	)

/*
 *  STATISTICS
 */
typedef struct isdn_stats {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong header;			/* object stats header */
	/* 
	   followed by object-specific stats structure */
} isdn_stats_t;

#define ISDN_IOCGSTATSP		_IOWR(	ISDN_IOC_MAGIC,  8,  isdn_stats_t )
#define ISDN_IOCSSTATSP		_IOWR(	ISDN_IOC_MAGIC,  9,  isdn_stats_t )
#define ISDN_IOCGSTATS		_IOWR(	ISDN_IOC_MAGIC, 10,  isdn_stats_t )
#define ISDN_IOCCSTATS		_IOWR(	ISDN_IOC_MAGIC, 11,  isdn_stats_t )

/*
 *  EVENTS
 */
typedef struct isdn_notify {
	ulong type;			/* object type */
	ulong id;			/* object id */
	lmi_notify_t notify;		/* notifications */
} isdn_notify_t;

#define ISDN_IOCGNOTIFY		_IOR(	ISDN_IOC_MAGIC, 12,  isdn_notify_t )
#define ISDN_IOCSNOTIFY		_IOW(	ISDN_IOC_MAGIC, 13,  isdn_notify_t )
#define ISDN_IOCCNOTIFY		_IOW(	ISDN_IOC_MAGIC, 14,  isdn_notify_t )

/*
 *  ISDN MANAGEMENT
 */
typedef struct isdn_mgmt {
	ulong type;			/* object type */
	ulong id;			/* object id */
	ulong cmd;			/* command */
} isdn_mgmt_t;

#define ISDN_MGMT_BLOCK			1
#define ISDN_MGMT_UNBLOCK		2
#define ISDN_MGMT_RESET			3
#define ISDN_MGMT_QUERY			4

#define ISDN_IOCCMGMT		_IOWR(	ISDN_IOC_MAGIC, 15,  isdn_mgmt_t )

/*
 *  CONTROL LOWER
 */
typedef struct isdn_pass {
	ulong muxid;			/* mux index of lower DL to pass message to */
	ulong type;			/* type of message block */
	ulong band;			/* band of message block */
	ulong ctl_length;		/* length of cntl part */
	ulong dat_length;		/* length of data part */
	/* 
	   followed by cntl and data part of message to pass to DL */
} isdn_pass_t;

#define ISDN_IOCCPASS		_IOWR(	ISDN_IOC_MAGIC, 16,  isdn_pass_t )

#define ISDN_IOC_FIRST		 0
#define ISDN_IOC_LAST		16
#define ISDN_IOC_PRIVATE	32

#endif				/* __SS7_ISDNI_IOCTL_H__ */
