/*****************************************************************************

 @(#) $Id: m2ua_lm.h,v 0.9.2.1 2007/01/28 01:09:03 brian Exp $

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

 Last Modified $Date: 2007/01/28 01:09:03 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m2ua_lm.h,v $
 Revision 0.9.2.1  2007/01/28 01:09:03  brian
 - new file

 *****************************************************************************/

#ifndef __SYS_M2UA_H__
#define __SYS_M2UA_H__

#ident "@(#) $RCSfile: m2ua_lm.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

#ifndef M2UA_AS_IOC_MAGIC
#define M2UA_AS_IOC_MAGIC			('2' + 30)
#endif

/*
 * LAYER MANAGEMENT PRIMITIVES
 */

#define M2_T_ESTABLISH_REQ	((M2UA_AS_IOC_MAGIC << 8) +  1)
#define M2_T_RELEASE_REQ	((M2UA_AS_IOC_MAGIC << 8) +  2)
#define M2_T_STATUS_REQ		((M2UA_AS_IOC_MAGIC << 8) +  3)
#define M2_ASP_STATUS_REQ	((M2UA_AS_IOC_MAGIC << 8) +  4)
#define M2_AS_STATUS_REQ	((M2UA_AS_IOC_MAGIC << 8) +  5)
#define M2_ASP_UP_REQ		((M2UA_AS_IOC_MAGIC << 8) +  6)
#define M2_ASP_DOWN_REQ		((M2UA_AS_IOC_MAGIC << 8) +  7)
#define M2_ASP_ACTIVE_REQ	((M2UA_AS_IOC_MAGIC << 8) +  8)
#define M2_ASP_INACTIVE_REQ	((M2UA_AS_IOC_MAGIC << 8) +  9)
#define M2_REG_REQ		((M2UA_AS_IOC_MAGIC << 8) + 10)
#define M2_DEREG_REQ		((M2UA_AS_IOC_MAGIC << 8) + 11)

#define M2_T_ESTABLISH_CON	((M2UA_AS_IOC_MAGIC << 8) + 12)
#define M2_T_ESTABLISH_IND	((M2UA_AS_IOC_MAGIC << 8) + 13)
#define M2_T_RELEASE_CON	((M2UA_AS_IOC_MAGIC << 8) + 14)
#define M2_T_RELEASE_IND	((M2UA_AS_IOC_MAGIC << 8) + 15)
#define M2_T_RESTART_IND	((M2UA_AS_IOC_MAGIC << 8) + 16)
#define M2_T_STATUS_IND		((M2UA_AS_IOC_MAGIC << 8) + 17)
#define M2_T_STATUS_CON		((M2UA_AS_IOC_MAGIC << 8) + 18)
#define M2_ASP_STATUS_IND	((M2UA_AS_IOC_MAGIC << 8) + 19)
#define M2_ASP_STATUS_CON	((M2UA_AS_IOC_MAGIC << 8) + 10)
#define M2_AS_STATUS_IND	((M2UA_AS_IOC_MAGIC << 8) + 21)
#define M2_AS_STATUS_CON	((M2UA_AS_IOC_MAGIC << 8) + 22)
#define M2_NOTIFY_IND		((M2UA_AS_IOC_MAGIC << 8) + 23)
#define M2_ERROR_IND		((M2UA_AS_IOC_MAGIC << 8) + 24)
#define M2_ASP_UP_IND		((M2UA_AS_IOC_MAGIC << 8) + 25)
#define M2_ASP_UP_CON		((M2UA_AS_IOC_MAGIC << 8) + 26)
#define M2_ASP_DOWN_IND		((M2UA_AS_IOC_MAGIC << 8) + 27)
#define M2_ASP_DOWN_CON		((M2UA_AS_IOC_MAGIC << 8) + 28)
#define M2_ASP_ACTIVE_IND	((M2UA_AS_IOC_MAGIC << 8) + 29)
#define M2_ASP_ACTIVE_CON	((M2UA_AS_IOC_MAGIC << 8) + 30)
#define M2_ASP_INACTIVE_IND	((M2UA_AS_IOC_MAGIC << 8) + 31)
#define M2_ASP_INACTIVE_CON	((M2UA_AS_IOC_MAGIC << 8) + 32)
#define M2_AS_ACTIVE_IND	((M2UA_AS_IOC_MAGIC << 8) + 33)
#define M2_AS_INACTIVE_IND	((M2UA_AS_IOC_MAGIC << 8) + 34)
#define M2_AS_DOWN_IND		((M2UA_AS_IOC_MAGIC << 8) + 35)
#define M2_REG_CON		((M2UA_AS_IOC_MAGIC << 8) + 36)
#define M2_REG_IND		((M2UA_AS_IOC_MAGIC << 8) + 37)
#define M2_DEREG_CON		((M2UA_AS_IOC_MAGIC << 8) + 38)
#define M2_DEREG_IND		((M2UA_AS_IOC_MAGIC << 8) + 39)
#define M2_ERROR_OCC_IND	((M2UA_AS_IOC_MAGIC << 8) + 40)

/*
 * M2_T_ESTABLISH_REQ - request SCTP association establishment
 *
 * If the association is already bound before, the ADDR_length can be specified as zero.  If the
 * association has already been connected before, the DEST_length can be specified as zero.
 */
struct M2_t_establish_req {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_T_ESTABLISH_REQ */
	uint32_t ADDR_length;		/* bind address length */
	uint32_t ADDR_offset;		/* bind address offset */
	uint32_t DEST_length;		/* destination address length */
	uint32_t DEST_offset;		/* destination address offset */
	uint32_t OPT_length;		/* connect options length */
	uint32_t OPT_offset;		/* connect options offset */
};

/*
 * M2_T_ESTABLISH_CON -
 */
struct M2_t_establish_con {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_T_ESTABLISH_CON */
	uint32_t ADDR_length;		/* bound address length */
	uint32_t ADDR_offset;		/* bound address offset */
	uint32_t RES_length;		/* responding address length */
	uint32_t RES_offset;		/* responding address offset */
	uint32_t OPT_length;		/* connected options length */
	uint32_t OPT_offset;		/* connected options offset */
};

/*
 * M2_T_ESTABLISH_IND -
 */
struct M2_t_establish_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_T_ESTABLISH_IND */
	uint32_t SRC_length;		/* source address length */
	uint32_t SRC_offset;		/* source address offset */
	uint32_t OPT_length;		/* connecting options length */
	uint32_t OPT_offset;		/* connecting options offset */
};

/*
 * M2_T_RELEASE_REQ -
 */
struct M2_t_release_req {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_T_RELEASE_REQ */
};

/*
 * M2_T_RELEASE_CON -
 */
struct M2_t_release_con {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_T_RELEASE_CON */
};

/*
 * M2_T_RELEASE_IND -
 */
struct M2_t_release_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_T_RELEASE_IND */
	uint32_t DISCON_reason;		/* disconnect reason */
};

/*
 * M2_T_RESTART_IND -
 */
struct M2_t_restart_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_T_RESTART_IND */
	uint32_t DISCON_reason;		/* disconnect reason */
};

/*
 * M2_T_STATUS_REQ -
 */
struct M2_t_status_req {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_T_STATUS_REQ */
};

/*
 * M2_T_STATUS_CON -
 */
struct M2_t_status_con {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_T_STATUS_CON */
	uint32_t CURRENT_state;		/* current TPI state */
	uint32_t LOCADDR_length;	/* local (bound) address length */
	uint32_t LOCADDR_offset;	/* local (bound) address offset */
	uint32_t REMADDR_length;	/* remote (connected) address length */
	uint32_t REMADDR_offset;	/* remote (connected) address offset */
};

/*
 * M2_T_STATUS_IND -
 */
struct M2_t_status_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_T_STATUS_IND */
	uint32_t CURRENT_state;		/* current TPI state */
};

/*
 * M2_ASP_STATUS_REQ - Request ASP status
 *
 * Layer mangement request SGP to report status of remote ASP.
 */
struct M2_asp_status_req {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_ASP_STATUS_REQ */
	uint32_t ASP_id;		/* ASP for which to provide status */
	uint32_t INTERFACE_id;		/* IID for which to report status */
	uint32_t IID_length;		/* Text IID length */
	uint32_t IID_offset;		/* Text IID offset */
};

/*
 * M2_ASP_STATUS_CON -
 */
struct M2_asp_status_con {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_ASP_STATUS_CON */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
	uint32_t CURRENT_state;
};

/*
 * M2_ASP_STATUS_IND -
 */
struct M2_asp_status_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_ASP_STATUS_IND */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
	uint32_t CURRENT_state;
};

/*
 * M2_AS_STATUS_REQ -
 */
struct M2_as_status_req {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_AS_STATUS_REQ */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
};

/*
 * M2_AS_STATUS_CON -
 */
struct M2_as_status_con {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_AS_STATUS_CON */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
	uint32_t CURRENT_state;
};

/*
 * M2_AS_STATUS_IND -
 */
struct M2_as_status_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_AS_STATUS_IND */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
	uint32_t CURRENT_state;
};

/*
 * M2_NOTIFY_IND -
 */
struct M2_notify_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_NOTIFY_IND */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
	uint32_t NOTIFY_type;
};

/*
 * M2_ERROR_IND -
 */
struct M2_error_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_ERROR_IND */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
	uint32_t ERROR_type;
};

/*
 * M2_ASP_UP_REQ -
 */
struct M2_asp_up_req {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_ASP_UP_REQ */
	uint32_t ASP_id;
};

/*
 * M2_ASP_UP_CON -
 */
struct M2_asp_up_con {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_ASP_UP_CON */
	uint32_t ASP_id;
};

/*
 * M2_ASP_UP_IND -
 */
struct M2_asp_up_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_ASP_UP_IND */
	uint32_t ASP_id;
};

/*
 * M2_ASP_DOWN_REQ -
 */
struct M2_asp_down_req {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_ASP_DOWN_REQ */
	uint32_t ASP_id;
};

/*
 * M2_ASP_DOWN_CON -
 */
struct M2_asp_down_con {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_ASP_DOWN_CON */
	uint32_t ASP_id;
};

/*
 * M2_ASP_DOWN_IND -
 */
struct M2_asp_down_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_ASP_DOWN_IND */
	uint32_t ASP_id;
};

/*
 * M2_ASP_ACTIVE_REQ -
 */
struct M2_asp_active_req {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_ASP_ACTIVE_REQ */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
};

/*
 * M2_ASP_ACTIVE_CON -
 */
struct M2_asp_active_con {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_ASP_ACTIVE_CON */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
};

/*
 * M2_ASP_ACTIVE_IND -
 */
struct M2_asp_active_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_ASP_ACTIVE_IND */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
};

/*
 * M2_ASP_INACTIVE_REQ -
 */
struct M2_asp_inactive_req {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_ASP_INACTIVE_REQ */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
};

/*
 * M2_ASP_INACTIVE_CON -
 */
struct M2_asp_inactive_con {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_ASP_INACTIVE_CON */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
};

/*
 * M2_ASP_INACTIVE_IND -
 */
struct M2_asp_inactive_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_ASP_INACTIVE_IND */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
};

/*
 * M2_AS_ACTIVE_IND -
 */
struct M2_as_active_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_AS_ACTIVE_IND */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
};

/*
 * M2_AS_INACTIVE_IND -
 */
struct M2_as_inactive_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_AS_INACTIVE_IND */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
};

/*
 * M2_AS_DOWN_IND -
 */
struct M2_as_down_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_AS_DOWN_IND */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
};

/*
 * M2_REG_REQ -
 */
struct M2_reg_req {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_REG_REQ */
	uint32_t ASP_id;
	uint32_t SDT_id;
	uint32_t SDL_id;
};

/*
 * M2_REG_CON -
 */
struct M2_reg_con {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_REG_CON */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
	uint32_t SDT_id;
	uint32_t SDL_id;
};

/*
 * M2_REG_IND -
 */
struct M2_reg_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_REG_IND */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
	uint32_t SDTI;
	uint32_t SDLI;
};

/*
 * M2_DEREG_REQ -
 */
struct M2_dereg_req {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_DEREG_REQ */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
};

/*
 * M2_DEREG_CON -
 */
struct M2_dereg_con {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_DEREG_CON */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
};

/*
 * M2_DEREG_IND -
 */
struct M2_dereg_ind {
	uint32_t SG_id;			/* SG identifier */
	uint32_t PRIM_type;		/* always M2_DEREG_IND */
	uint32_t ASP_id;
	uint32_t INTERFACE_id;
	uint32_t IID_length;
	uint32_t IID_offset;
};

/*
 * M2_ERROR_OCC_IND - Negative receipt acknowledgement
 *
 * M_PCPROTO.  This primitive negative acknowledges receipt of a request primitive.  Only one
 * request primitive can have acknowlegement outstanding per Stream.
 */
struct M2_error_occ_ind {
	uint32_t PRIM_type;		/* always M2_ERROR_OCC_IND */
	uint32_t ERROR_prim;		/* request primitive in error */
	uint32_t ERROR_type;		/* type of error */
};

#endif				/* __SYS_M2UA_H__ */
