/*****************************************************************************

 @(#) $Id: mlp_control.h,v 1.1.2.3 2011-02-07 04:54:42 brian Exp $

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

 $Log: mlp_control.h,v $
 Revision 1.1.2.3  2011-02-07 04:54:42  brian
 - code updates for new distro support

 Revision 1.1.2.2  2010-11-28 14:21:46  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:25:32  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SYS_SNET_MLP_CONTROL_H__
#define __SYS_SNET_MLP_CONTROL_H__

#include <sys/snet/ll_control.h>

#define M_SETSNID	('M'<<8| 1)
#define M_GETSNID	('M'<<8| 2)
#define M_SETTUNE	('M'<<8| 3)
#define M_GETTUNE	('M'<<8| 4)
#define M_GETSTATS	('M'<<8| 5)
#define M_ZEROSTATS	('M'<<8| 6)
#define M_TRACEON	('M'<<8| 7)
#define M_TRACEOFF	('M'<<8| 8)
#define M_GETGSTATS	('M'<<8|10)
#define M_ZEROGSTATS	('M'<<8|11)

/* MLP TUNING */

#define LI_MLPTUNE	0x33

typedef struct mlptune {
	uint16_t mw;			/* MLP window */
	uint16_t mx;			/* MLP window in guard region */
	uint16_t mt1;			/* lost frame timer */
	uint16_t mt2;			/* group busy timer */
	uint16_t mt3;			/* MLP reset confirmation timer */
	uint16_t mn1;			/* */
} mlptune_t;

struct mlp_tnioc {
	uint8_t lli_type;		/* always LI_MLPTUNE */
	uint8_t lli_spare[3];		/* for alignment only */
	uint8_t lli_snid;		/* subnetwork (link) ID */
	mlptune_t mlp_tune;		/* MLP tuning parameters */
};

/* MLP STATISTICS */

/* shared global and subnetwork ID stats */
#define MLP_frames_tx		0
#define MLP_frames_rx		1
#define MLP_reset_tx		2
#define MLP_reset_rx		3
#define MLP_confs_tx		4
#define MLP_confs_rx		5
#define MLP_slps		6
#define MLP_num_slps		7

/* MLP GLOBAL STATISTICS */

/* global stats */
#define MLP_bytes_tx		8
#define MLP_bytes_rx		9
#define MLP_gstatmax		10

struct mlp_gstioc {
	uint8_t lli_type;		/* always LI_STATS */
	uint8_t lli_spare[3];		/* for alignment only */
	uint32_t mlpgstats[MLP_gstatmax];	/* global MLP stats */
};

/* MLP SNID STATISTICS */

/* subnetwork ID specific stats */
#define MLP_mt1_exp		8
#define MLP_mt2_exp		9
#define MLP_mt3_exp		10
#define MLP_mn1_exp		11
#define MLP_statmax		12

typedef struct mlp_stats {
	uint32_t mlpmonarray[MLPSTATMAX];	/* MLP subnetwork statistics */
} mlpstats_t;

struct mlp_stioc {
	uint8_t lli_type;		/* always LI_GSTATS */
	uint8_t state;
	uint16_t lli_spare;		/* for alignment only */
	uint32_t lli_snid;		/* subnetwork (link) ID */
	mlpstats_t lli_stats;		/* subnetwork (link) MLP stats */
};

/* UNION OF MLP STRUCTURES */

typedef union mlp_union {
	struct ll_hdioc ll_hd;		/* link ID structure */
	struct ll_snioc ll_sn;		/* subnetwork ID structure */
	struct mlp_tnioc mlp_tn;	/* MLP tuning structure */
	struct mlp_stioc mlp_st;	/* MLP statistics structure */
	struct mlp_gstioc mlp_gst;	/* MLP global statistics structure */
} mlpun_t;

#endif				/* __SYS_SNET_MLP_CONTROL_H__ */
