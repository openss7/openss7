/*****************************************************************************

 @(#) $Id: ixe_control.h,v 1.1.2.3 2011-02-07 04:54:42 brian Exp $

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

 Last Modified $Date: 2011-02-07 04:54:42 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ixe_control.h,v $
 Revision 1.1.2.3  2011-02-07 04:54:42  brian
 - code updates for new distro support

 Revision 1.1.2.2  2010-11-28 14:21:45  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:25:32  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SYS_SNET_IXE_CONTROL_H__
#define __SYS_SNET_IXE_CONTROL_H__

#define IXE_STID	208

#define IXEIOC		('E'<<8)

#define IXE_SNREG	(IXEIOC| 1)
#define IXE_ENT		(IXEIOC| 2)
#define IXE_DEL		(IXEIOC| 3)
#define IXE_GET		(IXEIOC| 4)
#define IXE_MGET	(IXEIOC| 5)
#define IXE_RESET	(IXEIOC| 6)
#define IXE_STATS	(IXEIOC| 7)
#define IXE_STATINIT	(IXEIOC| 8)
#define IXE_SNUNREG	(IXEIOC| 9)

#define EIXREG		6

#define DO_ACK		0
#define DO_DISC		1
#define DO_RESET	2

#define THIS_NSAP	0
#define IP_NSAP		1

#define IX_TICK		(HZ)
#define IX_RATE		(HZ/IX_TICK)
#define IX_PVC_TICKS	10

typedef struct ixe_sninfo {
	uint nsdu_hwm;
	uint idle_ticks;
	uint durn_ticks;
	uint hold_down;
	uint8_t conform84;
	uint8_t addr_ddn;
	uint8_t allow_no_addr;
	uint8_t rst_action;
	uint8_t exp_action;
	struct xaddrf my_xaddr;
	uint16_t lpc;
	uint16_t hpc;
} S_IXE_SNINFO;

typedef struct ixe_sninfo_32 {
	uint32_t nsdu_hwm;
	uint32_t idle_ticks;
	uint32_t durn_ticks;
	uint32_t hold_down;
	uint8_t conform84;
	uint8_t addr_ddn;
	uint8_t allow_no_addr;
	uint8_t rst_action;
	uint8_t exp_action;
	struct xaddrf my_xaddr;
	uint16_t lpc;
	uint16_t hpc;
} S_IXE_SNINFO_32;

typedef struct ixe_snreg {
	int8_t prim_class;
	int8_t op;
	uint32_t my_inaddr;
	struct ixe_sninfo sn_info;
} S_IXE_SNREG;

typedef struct ixe_snreg_32 {
	int8_t prim_class;
	int8_t op;
	uint32_t my_inaddr;
	struct ixe_sninfo_32 sn_info;
} S_IXE_SNREG;

typedef struct ixe_snunreg {
	int8_t prim_class;
	int8_t op;
	uint32_t my_inaddr;
} S_IXE_SNUNREG;

struct ixe_ent {
	int8_t prim_class;
	int8_t op;
	uint32_t in_addr;
	uint8_t nsap_flag;
	uint8_t pkg_siz;
	uint8_t win_siz;
	uint8_t num_vcs;
	uint8_t cug_type;
	uint8_t cug_field[MAX_CUG_LEN];
	uint8_t sub_rev_chrg;
	uint8_t bar_rev_chrg;
};

struct ixe_del {
	int8_t prim_class;
	int8_t op;
	uint32_t in_addr;
};

struct ixe_get {
	int8_t prim_class;
	int8_t op;
	uint32_t in_addr;
	struct xaddrf x_addr;
	uint8_t nsap_flag;
	uint8_t pkt_size;
	uint8_t win_siz;
	uint8_t num_vcs;
	uint8_t cug_type;
	uint8_t cug_field[MAX_CUG_LEN];
	uint8_t sub_rev_chrg;
	uint8_t bar_rev_chrg;
};

struct ixe_reset {
	int8_t prim_class;
	int8_t op;
};

typedef struct ix_addr {
	uint32_t in_addr;
	struct xaddrf x_addr;
	uint8_t nsap_flag;
	uint8_t pkt_siz;
	uint8_t win_siz;
	uint8_t num_vcs;
	uint8_t cug_type;
	uint8_t cug_field[MAX_CUG_LEN];
	uint8_t sub_rev_chrg;
	uint8_t bar_rev_chrg;
	uint8_t status;
	uint8_t delete_pending;
	time_t fail_call_time;
	int next_free;
	uint32_t PeerIndex;
} IX_ADDR;

typedef struct ix_addr_32 {
	uint32_t in_addr;
	struct xaddrf x_addr;
	uint8_t nsap_flag;
	uint8_t pkt_siz;
	uint8_t win_siz;
	uint8_t num_vcs;
	uint8_t cug_type;
	uint8_t cug_field[MAX_CUG_LEN];
	uint8_t sub_rev_chrg;
	uint8_t bar_rev_chrg;
	uint8_t status;
	uint8_t delete_pending;
	time_t fail_call_time;
	int32_t next_free;
	uint32_t PeerIndex;
} IX_ADDR_32;

#define IX_ADDR_SIZE	(sizeof(IX_ADDR) - sizeof(uint8_t) - sizeof(int))
#define IX_ADDR_SIZE_32	(sizeof(IX_ADDR_32) - sizeof(uint8_t) - sizeof(int32_t))

#define IX_TEMPORARY	1
#define IX_PERMANENT	2

#define MGET_BUFSIZE	(MAXIOCBSZ - 20)
#define MGET_MAXENT	(MGET_BUFSIZE / sizeof(IX_ADDR))
#define MGET_MAXENT_32	(MGET_BUFSIZE / sizeof(IX_ADDR_32))

struct ixe_mget {
	int8_t prim_class;
	int8_t op;
	uint first_ent;
	uint last_ent;
	uint num_ent;
	int8_t buf[MGET_BUFSIZE];
};

struct ixe_mget_32 {
	int8_t prim_class;
	int8_t op;
	uint32_t first_ent;
	uint32_t last_ent;
	uint32_t num_ent;
	int8_t buf[MGET_BUFSIZE];
};

typedef union ixe_adrop {
	int8_t prim_class;
	struct ixe_ent ixe_ent;
	struct ixe_del ixe_del;
	struct ixe_get ixe_get;
	struct ixe_mget ixe_mget;
	struct ixe_mget_32 ixe_mget_32;
	struct ixe_reset ixe_reset;
} S_IXE_ADROP;

struct ixe_stats {
	int8_t prim_class;
	int8_t op;
	uint ixcons_active;
	uint32_t dgs_out;
	uint32_t dgs_in;
	uint32_t nsdus_in;
	uint32_t nsdus_out;
};

struct ixe_stats_32 {
	int8_t prim_class;
	int8_t op;
	uint32_t ixcons_active;
	uint32_t dgs_out;
	uint32_t dgs_in;
	uint32_t nsdus_in;
	uint32_t nsdus_out;
};

struct ixe_statinit {
	int8_t prim_class;
	int8_t op;
};

typedef struct ix_peer {
	uint32_t *ix_peer;
	uint32_t callparm_index;
	uint32_t circ_index;
	uint32_t circ_channel;
	uint32_t connect_secs;
	uint8_t InUse;
} IX_PEER;

#endif				/* __SYS_SNET_IXE_CONTROL_H__ */
