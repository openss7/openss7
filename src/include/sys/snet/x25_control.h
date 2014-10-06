/*****************************************************************************

 @(#) $Id: x25_control.h,v 1.1.2.3 2011-02-07 04:54:43 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2011-02-07 04:54:43 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: x25_control.h,v $
 Revision 1.1.2.3  2011-02-07 04:54:43  brian
 - code updates for new distro support

 Revision 1.1.2.2  2010-11-28 14:21:54  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:26:50  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SYS_SNET_X25_CONTROL_H__
#define __SYS_SNET_X25_CONTROL_H__

#include "x25_proto.h"

#define MAXPVCLCNSSIZE 36

struct wlcfg {
	uint32_t U_SN_ID;
	uchar NET_MODE;
#define X25_LLC		 1		/* ISO/IEC 8881 */
#define X25_88		 2		/* ISO/IEC 8208 (1988) */
#define X25_84		 3		/* ISO/IEC 8208 (1984) */
#define X25_80		 4		/* ISO/IEC 8208 (1980) */
#define PSS		 5		/* UK */
#define AUSTPAC		 6		/* Australia */
#define DATAPAC		 7		/* Canada */
#define DDN		 8		/* DoD */
#define TELENET		 9		/* USA */
#define TRANSPAC	10		/* France */
#define TYMNET		11		/* USA */
#define DATEX_P		12		/* West Germany */
#define DDX_P		13		/* Japan */
#define VENUS_P		14		/* Japan */
#define ACCUNET		15		/* USA */
#define ITAPAC		16		/* Italy */
#define DATAPAK		17		/* Sweden */
#define DATANET		18		/* Holland */
#define DCS		19		/* Belgium */
#define TELEPAC		20		/* Switzerland */
#define F_DATAPAC	21		/* Finland */
#define FINPAC		22		/* Finland */
#define PACNET		23		/* New Zealand */
#define LUXPAC		24		/* Luxembourgh */
#define X25_CIRCUIT	25		/* circuit switched */

	uchar X25_VSN;
	uchar L3PLPMODE;

	short LPC;			/* LCI - 12 bits significant */
	short HPC;			/* LCI - 12 bits significant */
	short LIC;			/* LCI - 12 bits significant */
	short HIC;			/* LCI - 12 bits significant */
	short LTC;			/* LCI - 12 bits significant */
	short HTC;			/* LCI - 12 bits significant */
	short LOC;			/* LCI - 12 bits significant */
	short HOC;			/* LCI - 12 bits significant */
	short NPCchannels;		/* number of PVC channels */
	short NICchannels;		/* number or incoming channels */
	short NTCchannels;		/* number of two-way channels */
	short NOCchannels;		/* number of outgoing channels */
	short Nochnls;			/* number of channels */

	uchar THISGFI;
#define G_8	0x10			/* GFI - normal sequencing */
#define G_128	0x20			/* GFI - extended sequencing */
#define G_32768	0x30			/* GFI - super extended sequencing */
#define G_2147483648 0x40		/* GFI - super duper extended sequencing */

	uchar LOCMAXPKTSIZE;		/* 7 to 12 (1<<7 == 128 to 1<<12 == 4096) */
	uchar REMMAXPKTSIZE;		/* 7 to 12 (1<<7 == 128 to 1<<12 == 4096) */
	uchar LOCDEFPKTSIZE;		/* 7 to 12 (1<<7 == 128 to 1<<12 == 4096) */
	uchar REMDEFPKTSIZE;		/* 7 to 12 (1<<7 == 128 to 1<<12 == 4096) */
	uchar LOCMAXWSIZE;		/* 1 to (Modulo-1) */
	uchar REMMAXWSIZE;		/* 1 to (Modulo-1) */
	uchar LOCDEFWSIZE;		/* 1 to (Modulo-1) */
	uchar REMDEFWSIZE;		/* 1 to (Modulo-1) */

	ushort MAXNSDULEN;

	short ACKDELAY;
	short T20value;
	short T21value;
	short T22value;
	short T23value;

	short Tvalue;
	short T25value;
	short T26value;
	short T28value;

	short idlevalue;
	short connectvalue;

	uchar R20value;
	uchar R22value;
	uchar R23value;
	uchar R28value;

	ushort localdelay;
	ushort accessdelay;

	uchar locmaxthclass;
	uchar remmaxthclass;
	uchar locdefthclass;
	uchar remdefthclass;
	uchar locminthclass;
	uchar remminthclass;

	uchar CUG_CONTROL;
	ushort SUB_MODES;

	struct {
		ushort SNMODES;
		uchar intl_addr_recogn;
		uchar intl_prioritised;
		uchar dnic1;
		uchar dnic2;
		uchar prty_encode_control;
		uchar prty_pkt_forced_value;
		uchar src_addr_control;
		uchar dbit_control;
		uchar thrclass_neg_to_def;
		uchar thclass_type;
		uchar thclass_wmap[16];
		uchar thclass_pmap[16];
	} psdn_local;

	struct lsapformat local_address;

	uchar pvclcns[MAXPVCLCNSSIZE];
};

#define cll_in_g	  1
#define cll_out_g	  2
#define caa_in_g	  3
#define caa_out_g	  4
#define ed_in_g		  5
#define ed_out_g	  6
#define rnr_in_g	  7
#define rnr_out_g	  8
#define rr_in_g		  9
#define rr_out_g	 10
#define rst_in_g	 11
#define rst_out_g	 12
#define rsc_in_g	 13
#define rsc_out_g	 14
#define clr_in_g	 15
#define clr_out_g	 16
#define clc_in_g	 17
#define clc_out_g	 18
#define cll_coll_g	 19
#define cll_uabort_g	 20
#define rjc_buflow_g	 21
#define rjc_coll_g	 22
#define rjc_failNRS_g	 23
#define rjc_lstate_g	 24
#define rjc_nochnl_g	 25
#define rjc_nouser_g	 26
#define rjc_remote_g	 27
#define rjc_u_g		 28
#define dg_in_g		 29
#define dg_out_g	 30
#define p4_ferr_g	 31
#define rem_perr_g	 32
#define res_ferr_g	 33
#define res_in_g	 34
#define res_out_g	 35
#define vcs_labort_g	 36
#define r23exp_g	 37
#define l2conin_g	 38
#define l2conok_g	 39
#define l2conrej_g	 40
#define l2refusal_g	 41
#define l2lzap_g	 42
#define l2r20exp_g	 43
#define l2dxeexp_g	 44
#define l2dxebuf_g	 45
#define l2noconfig_g	 46
#define xiffnerror_g	 47
#define xintdisc_g	 48
#define xifaborts_g	 49
#define PVCusergone_g	 50
#define max_opens_g	 51
#define vcs_est_g	 52
#define bytes_in_g	 53
#define bytes_out_g	 54
#define dt_in_g		 55
#define dt_out_g	 56
#define res_conf_in_g	 57
#define res_conf_out_g	 58
#define reg_in_g	 59
#define reg_out_g	 60
#define reg_conf_in_g	 61
#define reg_conf_out_g	 62
#define l2r28exp_g	 63
#define Cantlzap_g	 64
#define L2badcc_g	 65
#define L2baddcnf_g	 66
#define L3T25timeouts_g	 67
#define L3badAE_g	 68
#define L3badT20_g	 69
#define L3badT24_g	 70
#define L3badT25_g	 71
#define L3badT28_g	 72
#define L3badevent_g	 73
#define L3badgfi_g	 74
#define L3badlstate_g	 75
#define L3badltock2_g	 76
#define L3badrandom_g	 77
#define L3badxtock0_g	 78
#define L3clrbadstate_g	 79
#define L3conlt0_g	 80
#define L3deqfailed_g	 81
#define L3indnodata_g	 82
#define L3matrixcall_g	 83
#define L3nodb_g	 84
#define L3qoscheck_g	 85
#define L3outbad_g	 86
#define L3shortframe_g	 87
#define L3tabfault_g	 88
#define L3usererror_g	 89
#define L3usergone_g	 90
#define LNeednotneeded_g 91
#define NSUbadref_g	 92
#define NSUdtnull_g	 93
#define NSUednull_g	 94
#define NSUrefrange_g	 95
#define NeednotNeeded_g	 96
#define NoNRSrequest_g	 97
#define UDRbad_g	 98
#define Ubadint_g	 99
#define Unoint_g	100
#define L3baddiag_g	101
#define glob_mon_size	102

struct persnidstats {
	uint32_t snid;
	int network_state;
#define cll_in_s	 1
#define cll_out_s	 2
#define caa_in_s	 3
#define caa_out_s	 4
#define dt_in_s		 5
#define dt_out_s	 6
#define ed_in_s		 7
#define ed_out_s	 8
#define rnr_in_s	 9
#define rnr_out_s	10
#define rr_in_s		11
#define rr_out_s	12
#define prov_rst_in_s	13
#define rem_rst_in_s	14
#define rsc_in_s	15
#define rsc_out_s	16
#define prov_clr_in_s	17
#define clc_in_s	18
#define clc_out_s	19
#define perr_in_s	20
#define out_vcs_s	21
#define in_vcs_s	22
#define twoway_vcs_s	23
#define res_in_s	24
#define res_out_s	25
#define res_timeouts_s	26
#define cll_timeouts_s	27
#define rst_timeouts_s	28
#define clr_timeouts_s	29
#define ed_timeouts_s	30
#define retry_exceed_s	31
#define clear_exceed_s	32
#define octets_in_s	33
#define octets_out_s	34
#define rec_in_s	35
#define rec_out_s	36
#define rst_in_s	37
#define rst_out_s	38
#define dg_in_s		39
#define dg_out_s	40
#define res_in_conn_s	41
#define clr_in_s	42
#define clr_out_s	43
#define pkts_in_s	44
#define pkts_out_s	45
#define vcs_est_s	46
#define max_svcs_s	47
#define svcs_s		48
#define pvcs_s		49
#define max_pvcs_s	50
#define rjc_coll_s      51
#define rjc_failNRS_s   52
#define rjc_nouser_s    53
#define rjc_buflow_s    54
#define reg_in_s	55
#define reg_out_s	56
#define reg_conf_in_s	57
#define reg_conf_out_s	58
#define snid_mon_size   59
	uint32_t mon_array[snid_mon_size];
};

struct vcinfo {
	struct xaddrf rem_addr;
	struct xaddrf loc_addr;
	uint32_t xu_ident;
	uint32_t process_id;
	uint16_t lci;
	uint8_t xstate;
	uint8_t xtag;
	uint8_t ampvc;
	uint8_t call_direction;
	uint8_t vctype;
#define cll_in_v	 1
#define cll_out_v	 2
#define caa_in_v	 3
#define caa_out_v	 4
#define dt_in_v		 5
#define dt_out_v	 6
#define ed_in_v		 7
#define ed_out_v	 8
#define rnr_in_v	 9
#define rnr_out_v	10
#define rr_in_v		11
#define rr_out_v	12
#define rst_in_v	13
#define rst_out_v	14
#define rsc_in_v	15
#define rsc_out_v	16
#define clr_in_v	17
#define clr_out_v	18
#define clc_in_v	19
#define clc_out_v	20
#define octets_in_v	21
#define octets_out_v	22
#define rst_timeouts_v	23
#define ed_timeouts_v	24
#define prov_rst_in_v	25
#define rem_rst_in_v	26
#define VC_mon_size	27
	int perVC_stats[vc_mon_size];
};

#define VC_STATES_SIZE (MAXIOCBSZ - 8)
#define MAX_VC_ENTS (VC_STATE_SIZE / sizeof(struct vcinfo))

struct vcstatusf {
	int first_ent;
	uint8_t num_ent;
	struct vcinfo vc;
};

struct qosdatpri {
	int band;
	unsigned int tx_window;
};

#endif				/* __SYS_SNET_X25_CONTROL_H__ */
