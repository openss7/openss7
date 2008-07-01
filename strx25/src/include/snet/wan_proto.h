/*****************************************************************************

 @(#) $Id: wan_proto.h,v 0.9.2.2 2008-07-01 12:31:08 brian Exp $

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

 Last Modified $Date: 2008-07-01 12:31:08 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: wan_proto.h,v $
 Revision 0.9.2.2  2008-07-01 12:31:08  brian
 - updated man pages, drafts, specs, header files

 Revision 0.9.2.1  2008-06-18 16:43:14  brian
 - added new files for NLI and DLPI

 *****************************************************************************/

#ifndef __SYS_SNET_WAN_PROTO_H__
#define __SYS_SNET_WAN_PROTO_H__

#ident "@(#) $RCSfile: wan_proto.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

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
#define WAN_REG		2	/* register subnetwork ID */
#define WAN_SID		1	/* specify subnetwork ID */
#define WAN_CTL		3	/* control connection */
#define WAN_DAT		4	/* transfer data */

/*
 * WAN_CTL command types: These constant values are used in the wan_command
 * field of the WAN_CTL primitive.
 */
#define WC_CONNECT	1	/* connect */
#define WC_CONCNF	2	/* connect confirm */
#define WC_DISC		3	/* disconnect */
#define WC_DISCCNF	4	/* disconnect confirm */

/*
 * WAN_DAT command types: These constant values are used in the wan_command
 * field of the WAN_DAT primitive.
 */
#define WC_TX		1	/* data for transmission */
#define WC_RX		2	/* received data */

/*
 * Address types: These constant values are used in the wan_remtype field of the
 * WAN_CTL primitive.
 */
#define WAN_TYPE_ASC	0	/* ASCII digits */
#define WAN_TYPE_BCD	1	/* BCD encoded digits */

/*
 * Status values: These constant values are used in the wan_status field of the
 * WAN_CTL primitive.
 */
#define WAN_FAIL	0	/* operation failed */
#define WAN_SUCCESS	1	/* operation successful */

/*
 * WAN_REG primitive, consists of one M_(PC)PROTO message block.
 */
struct wan_reg {
	uint8_t wan_type;		/* always WAN_REG */
	uint8_t wan_spare[3];		/* spare for alignment */
	uint32_t wan_snid;		/* subnetwork ID */
};

/*
 * WAN_SID primitive, consists of one M_PROTO message block.
 */
struct wan_sid {
	uint8_t wan_type;		/* always WAN_SID */
	uint8_t wan_spare[3];		/* spare for alignment */
	uint32_t wan_snid;		/* subnetwork ID */
};

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
 * WAN_MSG primitive, consists of one M_PROTO message block followed by one or
 * more M_DATA message blocks.
 */
struct wan_msg {
	uint8_t wan_type;		/* always WAN_MSG */
	uint8_t wan_command;		/* WC_TX or WC_RX */
};

union WAN_primitives {
	uint8_t wan_type;		/* WAN_SID, WAN_REG, WAN_CTL or WAN_DAT */
	struct wan_reg wreg;		/* registration message class */
	struct wan_sid wsid;		/* subnetwork ID message class */
	struct wan_ctl wctl;		/* control message class */
	struct wan_msg wmsg;		/* data message class */
};

#endif				/* __SYS_SNET_WAN_PROTO_H__ */
