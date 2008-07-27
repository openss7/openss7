/*****************************************************************************

 @(#) $Id: x25_control.h,v 0.9.2.1 2008/06/18 16:43:14 brian Exp $

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

 Last Modified $Date: 2008/06/18 16:43:14 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: x25_control.h,v $
 Revision 0.9.2.1  2008/06/18 16:43:14  brian
 - added new files for NLI and DLPI

 *****************************************************************************/

#ifndef __SYS_SNET_X25_CONTROL_H__
#define __SYS_SNET_X25_CONTROL_H__

#ident "@(#) $RCSfile: x25_control.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

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
	short NPCchannels;
	short NICchannels;
	short NTCchannels;
	short NOCchannels;
	short Nochnls;

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

#endif				/* __SYS_SNET_X25_CONTROL_H__ */
