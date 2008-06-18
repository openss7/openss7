/*****************************************************************************

 @(#) $Id: dlpi_ext.h,v 0.9.2.1 2008-06-18 16:43:16 brian Exp $

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

 Last Modified $Date: 2008-06-18 16:43:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dlpi_ext.h,v $
 Revision 0.9.2.1  2008-06-18 16:43:16  brian
 - added new files for NLI and DLPI

 *****************************************************************************/

#ifndef __SYS_DLPI_EXT_H__
#define __SYS_DLPI_EXT_H__

#ident "@(#) $RCSfile: dlpi_ext.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/*
 * This is a HP-UX extensions file.  It include extension DLPI primitive
 * definitions and HP-UX specific IOCTL definitions.  Don't include it if you
 * don't want the HP extensions.  Source compatibility is attempted.  Binary
 * compatiblity is not attempted.  It is used to attempt source compatibiltiy
 * with HP extensions using the Linux Fast-STREAMS DLPI drivers.
 */

/* Note: the numbers here are definitely not binary compatible with HP-UX.  If
 * anyone knows what the correct constants should be, please donate them.
 * Solaris uses 0x1XX so we just use 0x2XX for now and number them
 * sequentially. */

/*
 * General Extensions
 */
#define DL_HP_PPA_REQ			0x200	/* request PPAs */
#define DL_HP_PPA_ACK			0x201	/* provide PPAs */
#define DL_HP_MULTICAST_LIST_REQ	0x202	/* request multicast address list */
#define DL_HP_MULTICAST_LIST_ACK	0x203	/* provide multicast address list */
#define DL_HP_RAWDATA_REQ		0x204	/* send raw data */
#define DL_HP_RAWDATA_IND		0x205	/* receive raw data */
#define DL_HP_HW_RESET_REQ		0x206	/* reset the hardware */

/*
 * Connection-Oriented Mode Extensions
 * 
 * These are primarly for LLC2 it seems, but they can be mapped into other
 * HDLC ABM providers such as LAPB, LAPD, LAPF, HDLC, SDLC, ISO DL.
 */
#define DL_HP_INFO_REQ			0x207	/* request CO info */
#define DL_HP_INFO_ACK			0x208	/* provide CO info */
#define DL_HP_SET_ACK_TO_REQ		0x209	/* set ack timeout */
#define DL_HP_SET_P_TO_REQ		0x20a	/* set P-bit timeout */
#define DL_HP_SET_REJ_TO_REQ		0x20b	/* set reject timeout */
#define DL_HP_SET_BUSY_TO_REQ		0x20c	/* set busy timeout */
#define DL_HP_SET_SEND_ACK_TO_REQ	0x20d	/* set send ack timeout */
#define DL_HP_SET_MAX_RETRIES_REQ	0x20e	/* set max retries */
#define DL_HP_SET_ACK_THRESHOLD_REQ	0x20f	/* set ack threshold */
#define DL_HP_SET_LOCAL_WIN_REQ		0x210	/* set local window */
#define DL_HP_SET_REMOTE_WIN_REQ	0x211	/* set remote window */
#define DL_HP_CLEAR_STATS_REQ		0x212	/* clear statistics */
#define DL_HP_SET_LOCAL_BUSY_REQ	0x213	/* set local busy condition */
#define DL_HP_CLEAR_LOCAL_BUSY_REQ	0x214	/* clear local busy condition */

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_PPA_REQ */
} dl_hp_ppa_req_t;

typedef struct {
	dl_ulong dl_next_offset;
	dl_ulong dl_ppa;
	dl_ulong dl_hw_path[100];
	dl_ulong dl_mac_type;
	dl_ulong dl_phys_addr[20];
	dl_ulong dl_addr_length;
	dl_ulong dl_mjr_num;
	dl_ulong dl_name[64];
	dl_ulong dl_instance_num;
	dl_ulong dl_mtu;
	dl_ulong dl_hdw_state;
	dl_ulong dl_module_id_1[64];
	dl_ulong dl_module_id_2[64];
	dl_ulong dl_arpmod_name[64];
	dl_ulong dl_nmid;
	dl_ulong dl_reserved1;
	dl_ulong dl_reserved2;
} dl_hp_ppa_info_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_PPA_ACK */
	dl_ulong dl_length;		/* length of PPAs */
	dl_ulong dl_count;		/* count of dl_hp_ppa_info_t structures */
	dl_ulong dl_offset;		/* offset of PPAs from start of message block */
} dl_hp_ppa_ack_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_MULTICAST_LIST_REQ */
} dl_hp_multicast_list_req_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_MULTICAST_LIST_ACK */
	dl_ulong dl_length;		/* length of addresses */
	dl_ulong dl_count;		/* count of addresses */
	dl_ulong dl_offset;		/* offset of addresses from start of message block */
} dl_hp_multicast_list_ack_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_RAWDATA_REQ */
} dl_hp_rawdata_req_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_RAWDATA_IND */
} dl_hp_rawdata_ind_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_HW_RESET_REQ */
} dl_hp_hw_reset_req_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_INFO_REQ */
} dl_hp_info_req_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_INFO_ACK */
	dl_ulong dl_mem_fails;		/* buffer allocation failures */
	dl_ulong dl_queue_fails;	/* queue congestion failures */
	dl_ulong dl_ack_to;		/* ack timeout (0.1 sec) */
	dl_ulong dl_p_to;		/* p timeout (0.1 sec) */
	dl_ulong dl_rej_to;		/* reject timeout (0.1 sec) */
	dl_ulong dl_busy_to;		/* busy timeout (0.1 sec) */
	dl_ulong dl_send_ack_to;	/* send ack timeout (0.1 sec) */
	dl_ulong dl_ack_to_cnt;		/* count of ack timeouts */
	dl_ulong dl_p_to_cnt;		/* count of p timeouts */
	dl_ulong dl_rej_to_cnt;		/* count of reject timeouts */
	dl_ulong dl_busy_to_cnt;	/* count of busy timeouts */
	dl_ulong dl_local_win;		/* local (receive) window */
	dl_ulong dl_remote_win;		/* remote (transmit) window */
	dl_ulong dl_i_pkts_in;		/* I-PDUs received */
	dl_ulong dl_i_pkts_oos;		/* I-PDUs received out of sequence */
	dl_ulong dl_i_pkts_in_drop;	/* I-PDUs receive dropped */
	dl_ulong dl_i_pkts_out;		/* I-PDUs sent */
	dl_ulong dl_i_pkts_retrans;	/* I-PDUs retransmitted */
	dl_ulong dl_s_pkts_in;		/* S-PDUs received */
	dl_ulong dl_s_pkts_out;		/* S-PDUs sent */
	dl_ulong dl_u_pkts_in;		/* UI-PDUs received */
	dl_ulong dl_u_pkts_out;		/* UI-PDus sent */
	dl_ulong dl_bad_pkts;		/* bad packets received */
	dl_ulong dl_retry_cnt;		/* p-bit retries current */
	dl_ulong dl_max_retry_cnt;	/* p-bit retries high water */
	dl_ulong dl_max_retries;	/* retries high water */
	dl_ulong dl_ack_thres;		/* ack window */
	dl_ulong dl_remote_busy_cnt;	/* remote busy conditions */
	dl_ulong dl_hw_req_fails;	/* hardware request failures */
} dl_hp_info_ack_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_SET_ACK_TO_REQ */
	dl_ulong dl_ack_to;		/* ack timeout (0.1 sec) */
} dl_hp_set_ack_to_req_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_SET_P_TO_REQ */
	dl_ulong dl_p_to;		/* p timeout (0.1 sec) */
} dl_hp_set_p_to_req_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_SET_REJ_TO_REQ */
	dl_ulong dl_rej_to;		/* reject timeout (0.1 sec) */
} dl_hp_set_rej_to_req_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_SET_BUSY_TO_REQ */
	dl_ulong dl_busy_to;		/* busy timeout (0.1 sec) */
} dl_hp_set_busy_to_req_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_SET_SEND_ACK_TO_REQ */
	dl_ulong dl_send_ack_to;	/* send ack timeout (0.1 sec) */
} dl_hp_set_send_ack_to_req_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_SET_MAX_RETRIES_REQ */
	dl_ulong dl_max_retries;	/* maximum retries */
} dl_hp_set_max_retries_req_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_SET_ACK_THRESHOLD_REQ */
	dl_ulong dl_ack_thres;		/* unacknowledged ack threshold */
} dl_hp_set_ack_threshold_req_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_SET_LOCAL_WIN_REQ */
	dl_ulong dl_local_win;		/* local (receive) window */
} dl_hp_set_local_win_req_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_SET_REMOTE_WIN_REQ */
	dl_ulong dl_remote_win;		/* remote (transmit) window */
} dl_hp_set_remote_win_req_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_CLEAR_STATS_REQ */
} dl_hp_clear_stats_req_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_SET_LOCAL_BUSY_REQ */
} dl_hp_set_local_busy_req_t;

typedef struct {
	dl_ulong dl_primitive;		/* always DL_HP_CLEAR_LOCAL_BUSY_REQ */
} dl_hp_clear_local_busy_req_t;

#endif				/* __SYS_DLPI_EXT_H__ */
