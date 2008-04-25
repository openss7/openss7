/*****************************************************************************

 @(#) $Id: dl_x25.h,v 0.9.2.2 2008-04-25 11:39:32 brian Exp $

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

 Last Modified $Date: 2008-04-25 11:39:32 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dl_x25.h,v $
 Revision 0.9.2.2  2008-04-25 11:39:32  brian
 - updates to AGPLv3

 Revision 0.9.2.1  2007/12/15 20:16:50  brian
 - added new mib files

 *****************************************************************************/

#ifndef __SYS_DL_X25_H__
#define __SYS_DL_X25_H__

#ident "@(#) $RCSfile: dl_x25.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

#define LI_STATS	 1
#define LI_PLAIN	 2
#define LI_GSTATS	 3
#define LI_SPPA		 4
#define LI_LLC2TUNE	 5

#define tx_ign		 0
#define rx_badlen	 1
#define rx_unknown	 2
#define t1_exp		 3
#define t4_exp		 4
#define t4_n2_exp	 5

#define RR_rx_cmd	 6		/**< Number of received RR commands. */
#define RR_rx_rsp	 7		/**< Number of received RR responses. */
#define RR_tx_cmd	 8		/**< Number of sent RR commands. */
#define RR_tx_rsp	 9		/**< Number of sent RR responses. */
#define RR_tx_cmd_p	10		/**< Number of sent RR responses P-bit set. */

#define RNR_rx_cmd	11		/**< Number of received RNR commands. */
#define RNR_rx_rsp	12		/**< Number of received RNR responses. */
#define RNR_tx_cmd	13		/**< Number of sent RNR commands. */
#define RNR_tx_rsp	14		/**< Number of sent RNR responses. */
#define RNR_tx_rsp_p	15		/**< Number of sent RNR responses P-bit set. */

#define REJ_rx_cmd	16		/**< Number of received REJ commands. */
#define REJ_rx_rsp	17		/**< Number of received REJ responses. */
#define REJ_tx_cmd	18		/**< Number of sent REJ commands. */
#define REJ_tx_rsp	19		/**< Number of sent REJ responses. */
#define REJ_tx_rsp_p	20		/**< Number of sent REJ responses P-bit set. */

#define SABME_rx_cmd	21		/**< Number of received SABME commands. */
#define SABME_tx_cmd	22		/**< Number of sent SABME commands. */

#define DISC_rx_cmd	23		/**< Number of received DISC commands. */
#define DISC_tx_cmd	24		/**< Number of sent DISC commands. */

#define UA_rx_rsp	25		/**< Number of received UA responses. */
#define UA_tx_rsp	26		/**< Number of sent UA responses. */

#define DM_rx_rsp	27		/**< Number of received DM responses. */
#define DM_tx_rsp	28		/**< Number of sent DM responses. */

#define I_rx_cmd	29		/**< Number of received I-frame commands. */
#define I_tx_cmd	30		/**< Number of sent I-frame commands. */

#define FRMR_rx_rsp	31		/**< Number of received FRMR responses. */
#define FRMR_tx_rsp	32		/**< Number of sent FRMR responses. */

#define tx_rtr		33
#define rx_bad		34
#define rx_dud		35
#define rx_ign		36

#define I_rx_rsp	37		/**< Number of received I-frame responses. */
#define I_tx_rsp	38		/**< Number of sent I-frame responses. */

#define UI_rx_cmd	39		/**< Number of received UI frames. */
#define UI_tx_cmd	40		/**< Number of sent UI frames. */

#define XID_rx_cmd	41		/**< Number of received XID commands. */
#define XID_rx_rsp	42		/**< Number of received XID responses. */
#define XID_tx_cmd	43		/**< Number of sent XID commands. */
#define XID_tx_rsp	44		/**< Number of sent XID responses. */

#define TEST_rx_cmd	45		/**< Number of received TEST commands. */
#define TEST_rx_rsp	46		/**< Number of received TEST responses. */
#define TEST_tx_cmd	47		/**< Number of sent TEST commands. */
#define TEST_tx_rsp	48		/**< Number of sent TEST responses. */

#define llc2statmax	40

typedef struct lab2_stats {
	unsigned int lapbmonarray[llc2statmax];
} lapbstats_t;

struct lapb_strioc {
	unsigned char lli_type;		/**< The table type.  Always LI_STATS. */
	unsigned char lli_spare[3];	/**< Reserved field set to zero. */
	unsigned int lli_ppa;		/**< The Physical Point of Attachment (PPA). */
	lapbstats_t lli_stats;		/**< The statistics table. */
};

struct ll_hdioc {
	unsigned char lli_type;		/**< The table type.  Always LI_PLAIN. */
	unsigned char lli_spare[3];	/**< Reserved for alignment. */
	unsigned int lli_ppa;		/**< The Physical Point of Attachment (PPA).  Set this value
					     to 0xff to zero statistics for all links. */
};

#define frames_tx		0	/**< Frames transmitted. */
#define frames_rx		1	/**< Frames received. */
#define sabme_tx		2	/**< SABMEs transmitted. */
#define sabme_rx		3	/**< SABMEs received. */
#define bytes_tx		4	/**< Data bytes transmitted. */
#define bytes_rx		5	/**< Data bytes received. */
#define gobstatmax		6	/**< The size of the global statistics table. */

struct lapb_gstioc {
	unsigned char lli_type;		/**< The table type. Always LI_GSTATS. */
	unsigned char lli_spare[3];	/**< Reserved for alignment. */
	unsigned int lapbgstats[globstatmax];	/**< Global stats table. */
};

struct ll_snioc {
	unsigned char lli_type;		/**< The table type.  Always LI_SPPA. */
	unsigned char lli_class;	/**< DTE/DCE/extended.  This indicates the type of link.
					     LC_LLC2 must be used for LLC2. LC_LAPBDTE or LC_LAPBDCE
					     must be used for LAPB.  The file
					     /usr/include/netdlc/ll_proto.h contains a complete list
					     of values. */
	unsigned short lli_spl_pri;	/**< SLP priority hwne used in an MLP multiplexer.  This
					     determines the priority of SLP when MLP is used. */
	unsigned int lli_ppa;		/**< Physical Point of Attachment (PPA) and Subnetwork ID
					     character. */
	unsigned int lli_index;		/**< Link index under the MLP multiplexing driver.  This must 
					     be set with the muxid value returned by the I_LINK(7)
					     ioctl when LAPB is placed over a serial driver. */
};

typedef struct llc2_tune {
	unsigned short N2;		/**< Maximum number of retries.  Values between 1 and 255,
					     default 10. */
	unsigned short T1;		/**< Acknowledgement time (units of 0.1 seconds).  Values
					     between 10 and 3000 dS, default 90. */
	unsigned short Tpf;		/**< P/F cycle retry time (units of 0.1 seconds).  Values
					     between 10 and 2000 dS, default 7. */
	unsigned short Tbusy;		/**< Remote busy check time (units of 0.1 seconds).  Values
					     between 10 and 30000 dS, default 100. */
	unsigned short Tidle;		/**< Idle P/F cycle time (units of 0.1 seconds).  Values
					     betwen 0 and 32000 dS, default 250. */
	unsigned short ack_delay;	/**< RR delay time (units of 0.1 seconds).  Values between 0
					     and 3000 dS, default 4. */
	unsigned short notack_max;	/**< Maximum number of unacknowledged received I-frames.
					     Values between 0 and 127, default 3. */
	unsigned short tx_window;	/**< Transmit window (if no XID received). */
	unsigned short tx_probe;	/**< P-bit position before end of transmit window. Values
					     between 0 and 127, default 0. */
	unsigned short max_I_len;	/**< Maximum I-frame length. */
	unsigned short xid_window;	/**< XID window size (receive window). Values between 1 and
					     127, default 7. */
	unsigned short xid_Ndup;	/**< Duplicate MAC XID count (zero for no test).  Values
					     between 0 and 255, default 2. */
	unsigned short xid_Tdup;	/**< Duplication MAX XID time (units of 0.1 seconds).  Values 
					     between 0 and 3000 dS, default 50. */
} llc2tune_t;

struct llc2_tnioc {
	unsigned char lli_type;
	unsigned char lli_spare[3];
	unsigned int lli_ppa;
	llc2tune_t llc2_tune;
};

#define X25DL_IOC_MAGIC		'x'

#define L_GETSTATS	((X25DL_IOC_MAGIC << 8) + 240)
#define L_ZEROSTATS	((X25DL_IOC_MAGIC << 8) + 241)
#define L_GETGSTATS	((X25DL_IOC_MAGIC << 8) + 242)
#define L_SETPPA	((X25DL_IOC_MAGIC << 8) + 243)
#define L_GETPPA	((X25DL_IOC_MAGIC << 8) + 244)
#define L_SETTUNE	((X25DL_IOC_MAGIC << 8) + 245)
#define L_GETTUNE	((X25DL_IOC_MAGIC << 8) + 246)
#define W_SETTUNE	((X25DL_IOC_MAGIC << 8) + 247)

#endif				/* __SYS_DL_X25_H__ */
