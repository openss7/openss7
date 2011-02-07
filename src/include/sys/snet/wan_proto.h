/*****************************************************************************

 @(#) $Id: wan_proto.h,v 1.1.2.3 2011-02-07 04:54:43 brian Exp $

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

 $Log: wan_proto.h,v $
 Revision 1.1.2.3  2011-02-07 04:54:43  brian
 - code updates for new distro support

 Revision 1.1.2.2  2010-11-28 14:21:53  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:26:50  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SYS_SNET_WAN_PROTO_H__
#define __SYS_SNET_WAN_PROTO_H__

/*
 * This file contains a basic SpiderWAN like interface.  Source compatibility is
 * attempted.  Binary compatibility is not attempted (but may result, YMMV).
 * Portable applications programs, STREAMS drivers and modules, should use the
 * CDI interface instead.
 */

/*
 * Primitive types: These constant values are used in the wan_type field of the
 * various primitives.
 */
#define WAN_SID		1	/* Set subnetwork identifier */
#define WAN_REG		2	/* Register subnetwork identifier */
#define WAN_CTL		3	/* control connection */
#define WAN_DAT		4	/* transfer data */
#define WAN_NTY		5	/* register and notify events */

/*
 * WAN_SID - one M_PROTO message block
 *
 * This primitive assigns a subnetwork identifier to the Stream upon which it
 * issued.  It is equivalent to an ATTACH.
 */
struct wan_sid {
	uint8_t wan_type;		/* always WAN_SID */
	uint8_t wan_spare[3];		/* spare for alignment */
	uint32_t wan_snid;		/* subnetwork ID */
};

/*
 * WAN_REG - one M_PROTO message block
 *
 * This primitive enables the subnetwork identifier for use.  The subnetwork
 * identifier does not need to match the Stream upon which the primitive is
 * issued.
 */
struct wan_reg {
	uint8_t wan_type;		/* always WAN_REG */
	uint8_t wan_spare[3];		/* spare for alignment */
	uint32_t wan_snid;		/* subnetwork ID */
};

/*
 * WAN_CTL Primitives - one M_PROTO message block
 *
 * These primitives come in eight forms: WC_CONNECT, WC_CONCNF, WC_DISC and
 * WC_DISCCNF, each issued in either direction, to or from the WAN driver.
 */
/*
 * WAN_CTL command types: These constant values are used in the wan_command
 * field of the WAN_CTL primitive.
 */
#define WC_CONNECT	1       /* connect */
#define WC_CONCNF	2       /* connect confirm */
#define WC_DISC		3       /* disconnect */
#define WC_DISCCNF	4       /* disconnect confirm */

/*
 * Address types: These constant values are used in the wan_remtype field of the
 * WAN_CTL primitive.
 */
#define WAN_TYPE_ASC	1	/* digits are ASCII digits, length is octets */
#define WAN_TYPE_BCD	2	/* digits are BCD digits, length is nibbles */

/*
 * Status values: These constant values are used in the wan_status field of the
 * WAN_CTL primitive.
 */
#define WAN_FAIL	0	/* operation failed */
#define WAN_SUCCESS	1	/* operation successful */

/*
 * WAN_CTL primitive, consists of one M_PROTO message block.
 */
struct wan_ctl {
	uint8_t wan_type;		/* always WAN_CTL */
	uint8_t wan_command;		/* command: WC_CONNECT, WC_CONCNF, WC_DISC, WC_DISCCNF */
	uint8_t wan_remtype;		/* remote address type: WAN_TYPE_ASC or WAN_TYPE_BCD */
	uint8_t wan_remsize;		/* size of remote address in octets or semi-octets */
	uint8_t wan_remaddr[20];	/* the remote address */
	uint8_t wan_status;		/* status: WAN_SUCCESS or WAN_FAIL */
	uint8_t wan_diag;		/* diagnostic when failed */
};

/*
 * WAN_DAT Primitives - one M_PROTO and one or more M_DATA message blocks
 *
 * These primitives come in two forms: WC_TX and WC_RX.  WC_RX are issued by
 * the WAN driver, and WC_TX are issued to the WAN driver.  Any addresses must
 * be implicit or included in the data.
 */
#define WC_TX		1	/* data for transmission */
#define WC_RX		2	/* received data */

/*
 * WAN_MSG primitive, consists of one M_PROTO message block followed by one or
 * more M_DATA message blocks.
 */
struct wan_msg {
	uint8_t wan_type;		/* always WAN_MSG */
	uint8_t wan_command;		/* WC_TX or WC_RX */
};

/*
 * WAN_NTY Primitive - one M_PROTO message block
 *
 * When the WAN_NTY primitive is sent to the WAN driver, the WAN driver marks
 * the bits that are masked and generates notifications for those events that
 * have a 1 in the corresponding bit location.  When the event occurs, the WAN
 * driver will issue a WAN_NTY primitive upstream.  By default, and for
 * compatibility, no events are registered for notification.
 */

/* definitions for wan_eventstat field */
#define W_RECEIVE_BUFFER_OVFL	(1<< 0)
#define W_FRAMING_ERROR		(1<< 1)
#define W_TIMEOUT		(1<< 2)
#define W_HD_OVERRUN		(1<< 3)
#define W_ATTACHED_DEV_INACT	(1<< 4)
#define W_ATTACHED_DEV_ACTIVE	(1<< 5)
#define W_FCS_ERR		(1<< 7)
#define W_CTS_ON		(1<< 8)
#define W_CTS_OFF		(1<< 9)
#define W_DCD_ON		(1<<10)
#define W_DCD_OFF		(1<<11)
#define W_DSR_ON		(1<<12)
#define W_DSR_OFF		(1<<13)
#define W_RI_ON			(1<<14)
#define W_RI_OFF		(1<<15)
#define W_PARITY_ERROR		(1<<16)
#define W_BREAK_DETECTED	(1<<17)
#define W_SHORT_FRAME		(1<<18)
#define W_TX_UNDERRUN		(1<<19)
#define W_ABORT			(1<<20)
#define W_RCL_NONZERO		(1<<21)
#define W_BSC_PAD_ERR		(1<<22)
#define W_CTS_UNDERRUN		(1<<23)

struct wan_nty {
	uint8_t wan_type;		/* always WAN_NTY */
	uint8_t wan_spare[3];		/* spare for alignment */
	uint32_t wan_snid;		/* subnetwork identifier */
	uint32_t wan_eventstat;		/* event status */
	uint32_t wan_reserved[2];	/* reserved for future use */
};

union WAN_primitives {
	uint8_t wan_type;
	struct wan_sid wsid;		/* WAN_SID primitives */
	struct wan_reg wreg;		/* WAN_REG primitives */
	struct wan_ctl wctl;		/* WAN_CTL primitives */
	struct wan_dat wdat;		/* WAN_DAT primitives */
	struct wan_nty wnty;		/* WAN_NTY primitives */
};

#endif				/* __SYS_SNET_WAN_PROTO_H__ */
