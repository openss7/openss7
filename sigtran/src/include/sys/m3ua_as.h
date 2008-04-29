/*****************************************************************************

 @(#) $Id: m3ua_as.h,v 0.9.2.3 2008-04-29 01:52:23 brian Exp $

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

 Last Modified $Date: 2008-04-29 01:52:23 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m3ua_as.h,v $
 Revision 0.9.2.3  2008-04-29 01:52:23  brian
 - updated headers for release

 Revision 0.9.2.2  2007/08/14 08:34:06  brian
 - GPLv3 header update

 Revision 0.9.2.1  2007/01/26 21:53:51  brian
 - working up AS drivers and docs

 *****************************************************************************/

#ifndef __SYS_M3UA_AS_H__
#define __SYS_M3UA_AS_H__

#ident "@(#) $RCSfile: m3ua_as.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

#define M3UA_AS_IOC_MAGIC	('M' + 30)

#define M3UA_CLEI_MAX	32

struct m3ua_sgp {
	int m3_index;
	unsigned int m3_sgid;
	char m3_sg_clei[M3UA_CLEI_MAX];
	unsigned int m3_aspid;
	char m3_asp_clei[M3UA_CLEI_MAX];
	struct sockaddr m3_loc_add;
	unsigned int m3_loc_len;
	struct sockaddr m3_rem_add;
	unsigned int m3_rem_len;
	unsigned int m3_ppi;
	unsigned short m3_streams;
	unsigned short m3_options;
	unsigned int m3_version;
};

#define M3UA_SG_DYNAMIC		0x01

#define M3UA_STREAMS_DEFAULT	33

#define M3UA_VERSION_NONE	0
#define M3UA_VERSION_RFC3332	1
#define M3UA_VERSION_TS129202	2
#define M3UA_VERSION_TS102142	3
#define M3UA_VERSION_RFC4666	4
#define M3UA_VERSION_DEFAULT	M3UA_VERSION_RFC3331

struct m3ua_sgp_list {
	unsigned int m3_sgp_num;
	struct m3ua_sgp m3_sgp_list[0];
	/* followed by M3UA SGP configurations */
};

struct m3ua_key {
	unsigned int m3_sgid;
	char m3_sg_clei[M3UA_CLEI_MAX];
	unsigned int m3_tmode;
	unsigned short m3_rc;
	unsigned short m3_na;
	unsigned int m3_loc_addrs;
	unsigned int m3_rem_addrs;
	struct mtp_addr m3_addrs[0];
	/* followed by MTP addresses */
};

#define M3UA_AS_TMODE_NONE		0
#define M3UA_AS_TMODE_OVERRIDE		1
#define M3UA_AS_TMODE_LOADSHARE		2
#define M3UA_AS_TMODE_BROADCAST		3

struct m3ua_key_list {
	unsigned int m3_key_num;
	struct m3ua_key m3_key_list[0];
	/* follwed by M3UA keys */
};

#define M3UA_AS_IOCSCONFIG	_IOW(	M3UA_AS_IOC_MAGIC,	1,	struct m3ua_sgp		)
#define M3UA_AS_IOCGCONFIG	_IOWR(	M3UA_AS_IOC_MAGIC,	2,	struct m3ua_sgp		)
#define M3UA_AS_IOCLCONFIG	_IOWR(	M3UA_AS_IOC_MAGIC,	3,	struct m3ua_sgp_list	)

#define M3UA_AS_IOCSKEY		_IOW(	M3UA_AS_IOC_MAGIC,	4,	struct m3ua_key		)
#define M3UA_AS_IOCCKEY		_IOW(	M3UA_AS_IOC_MAGIC,	5,	struct m3ua_key		)
#define M3UA_AS_IOCGKEY		_IOWR(	M3UA_AS_IOC_MAGIC,	6,	struct m3ua_key_list	)

/*
 * LAYER MANAGEMENT PRIMITIVES
 */

#define M3_T_ESTABLISH_REQ	((M3UA_AS_IOC_MAGIC << 8) +  1)
#define M3_T_RELEASE_REQ	((M3UA_AS_IOC_MAGIC << 8) +  2)
#define M3_T_STATUS_REQ		((M3UA_AS_IOC_MAGIC << 8) +  3)
#define M3_ASP_STATUS_REQ	((M3UA_AS_IOC_MAGIC << 8) +  4)
#define M3_AS_STATUS_REQ	((M3UA_AS_IOC_MAGIC << 8) +  5)
#define M3_ASP_UP_REQ		((M3UA_AS_IOC_MAGIC << 8) +  6)
#define M3_ASP_DOWN_REQ		((M3UA_AS_IOC_MAGIC << 8) +  7)
#define M3_ASP_ACTIVE_REQ	((M3UA_AS_IOC_MAGIC << 8) +  8)
#define M3_ASP_INACTIVE_REQ	((M3UA_AS_IOC_MAGIC << 8) +  9)
#define M3_REG_REQ		((M3UA_AS_IOC_MAGIC << 8) + 10)
#define M3_DEREG_REQ		((M3UA_AS_IOC_MAGIC << 8) + 11)

#define M3_T_ESTABLISH_CON	((M3UA_AS_IOC_MAGIC << 8) + 12)
#define M3_T_ESTABLISH_IND	((M3UA_AS_IOC_MAGIC << 8) + 13)
#define M3_T_RELEASE_CON	((M3UA_AS_IOC_MAGIC << 8) + 14)
#define M3_T_RELEASE_IND	((M3UA_AS_IOC_MAGIC << 8) + 15)
#define M3_T_RESTART_IND	((M3UA_AS_IOC_MAGIC << 8) + 16)
#define M3_T_STATUS_IND		((M3UA_AS_IOC_MAGIC << 8) + 17)
#define M3_T_STATUS_CON		((M3UA_AS_IOC_MAGIC << 8) + 18)
#define M3_ASP_STATUS_IND	((M3UA_AS_IOC_MAGIC << 8) + 19)
#define M3_ASP_STATUS_CON	((M3UA_AS_IOC_MAGIC << 8) + 10)
#define M3_AS_STATUS_IND	((M3UA_AS_IOC_MAGIC << 8) + 21)
#define M3_AS_STATUS_CON	((M3UA_AS_IOC_MAGIC << 8) + 22)
#define M3_NOTIFY_IND		((M3UA_AS_IOC_MAGIC << 8) + 23)
#define M3_ERROR_IND		((M3UA_AS_IOC_MAGIC << 8) + 24)
#define M3_ASP_UP_IND		((M3UA_AS_IOC_MAGIC << 8) + 25)
#define M3_ASP_UP_CON		((M3UA_AS_IOC_MAGIC << 8) + 26)
#define M3_ASP_DOWN_IND		((M3UA_AS_IOC_MAGIC << 8) + 27)
#define M3_ASP_DOWN_CON		((M3UA_AS_IOC_MAGIC << 8) + 28)
#define M3_ASP_ACTIVE_IND	((M3UA_AS_IOC_MAGIC << 8) + 29)
#define M3_ASP_ACTIVE_CON	((M3UA_AS_IOC_MAGIC << 8) + 30)
#define M3_ASP_INACTIVE_IND	((M3UA_AS_IOC_MAGIC << 8) + 31)
#define M3_ASP_INACTIVE_CON	((M3UA_AS_IOC_MAGIC << 8) + 32)
#define M3_AS_ACTIVE_IND	((M3UA_AS_IOC_MAGIC << 8) + 33)
#define M3_AS_INACTIVE_IND	((M3UA_AS_IOC_MAGIC << 8) + 34)
#define M3_AS_DOWN_IND		((M3UA_AS_IOC_MAGIC << 8) + 35)
#define M3_REG_CON		((M3UA_AS_IOC_MAGIC << 8) + 36)
#define M3_REG_IND		((M3UA_AS_IOC_MAGIC << 8) + 37)
#define M3_DEREG_CON		((M3UA_AS_IOC_MAGIC << 8) + 38)
#define M3_DEREG_IND		((M3UA_AS_IOC_MAGIC << 8) + 39)
#define M3_ERROR_OCC_IND	((M3UA_AS_IOC_MAGIC << 8) + 40)

/*
 * M3_T_ESTABLISH_REQ - request SCTP association establishment
 */
struct M3_t_establish_req {
	uint32_t PRIM_type;		/* always M3_T_ESTABLISH_REQ */
	uint32_t ASSOC_id;		/* multiplexer id */
	uint32_t DEST_length;		/* destination address length */
	uint32_t DEST_offset;		/* destination address offset */
	uint32_t OPT_length;		/* connect options length */
	uint32_t OPT_offset;		/* connect options offset */
};

/*
 * M3_T_ESTABLISH_CON -
 */
struct M3_t_establish_con {
	uint32_t PRIM_type;		/* always M3_T_ESTABLISH_CON */
	uint32_t ASSOC_id;		/* multiplexer id */
	uint32_t RES_length;		/* responding address length */
	uint32_t RES_offset;		/* responding address offset */
	uint32_t OPT_length;		/* connected options length */
	uint32_t OPT_offset;		/* connected options offset */
};

/*
 * M3_T_ESTABLISH_IND -
 */
struct M3_t_establish_ind {
	uint32_t PRIM_type;		/* always M3_T_ESTABLISH_IND */
	uint32_t ASSOC_id;		/* multiplexer id */
	uint32_t SRC_length;		/* source address length */
	uint32_t SRC_offset;		/* source address offset */
	uint32_t OPT_length;		/* connecting options length */
	uint32_t OPT_offset;		/* connecting options offset */
};

/*
 * M3_T_RELEASE_REQ -
 */
struct M3_t_release_req {
	uint32_t PRIM_type;		/* always M3_T_RELEASE_REQ */
	uint32_t ASSOC_id;		/* multiplexer id */
};

/*
 * M3_T_RELEASE_CON -
 */
struct M3_t_release_con {
	uint32_t PRIM_type;		/* always M3_T_RELEASE_CON */
	uint32_t ASSOC_id;		/* multiplexer id */
};

/*
 * M3_T_RELEASE_IND -
 */
struct M3_t_release_ind {
	uint32_t PRIM_type;		/* always M3_T_RELEASE_IND */
	uint32_t ASSOC_id;		/* multiplexer id */
	uint32_t DISCON_reason;		/* disconnect reason */
};

/*
 * M3_T_RESTART_IND -
 */
struct M3_t_restart_ind {
	uint32_t PRIM_type;		/* always M3_T_RESTART_IND */
	uint32_t ASSOC_id;		/* multiplexer id */
	uint32_t DISCON_reason;		/* disconnect reason */
};

/*
 * M3_T_STATUS_REQ -
 */
struct M3_t_status_req {
	uint32_t PRIM_type;		/* always M3_T_STATUS_REQ */
	uint32_t ASSOC_id;		/* multiplexer id */
};

/*
 * M3_T_STATUS_CON -
 */
struct M3_t_status_con {
	uint32_t PRIM_type;		/* always M3_T_STATUS_CON */
	uint32_t ASSOC_id;		/* multiplexer id */
	uint32_t CURRENT_state;		/* current TPI state */
	uint32_t LOCADDR_length;	/* local (bound) address length */
	uint32_t LOCADDR_offset;	/* local (bound) address offset */
	uint32_t REMADDR_length;	/* remote (connected) address length */
	uint32_t REMADDR_offset;	/* remote (connected) address offset */
};

/*
 * M3_T_STATUS_IND -
 */
struct M3_t_status_ind {
	uint32_t PRIM_type;		/* always M3_T_STATUS_IND */
	uint32_t ASSOC_id;		/* multiplexer id */
	uint32_t CURRENT_state;		/* current TPI state */
};

/*
 * M3_ASP_STATUS_REQ - Request ASP status
 *
 * Layer mangement request SGP to report status of remote ASP.
 */
struct M3_asp_status_req {
	uint32_t PRIM_type;		/* always M3_ASP_STATUS_REQ */
	uint32_t ASSOC_id;		/* SGP to interrogate */
	uint32_t ASP_id;		/* ASP for which to provide status */
	uint32_t ROUTING_context;	/* AS for which to report status */
};

/*
 * M3_ASP_STATUS_CON -
 */
struct M3_asp_status_con {
	uint32_t PRIM_type;		/* always M3_ASP_STATUS_CON */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
	uint32_t CURRENT_state;
};

/*
 * M3_ASP_STATUS_IND -
 */
struct M3_asp_status_ind {
	uint32_t PRIM_type;		/* always M3_ASP_STATUS_IND */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
	uint32_t CURRENT_state;
};

/*
 * M3_AS_STATUS_REQ -
 */
struct M3_as_status_req {
	uint32_t PRIM_type;		/* always M3_AS_STATUS_REQ */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
};

/*
 * M3_AS_STATUS_CON -
 */
struct M3_as_status_con {
	uint32_t PRIM_type;		/* always M3_AS_STATUS_CON */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
	uint32_t CURRENT_state;
};

/*
 * M3_AS_STATUS_IND -
 */
struct M3_as_status_ind {
	uint32_t PRIM_type;		/* always M3_AS_STATUS_IND */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
	uint32_t CURRENT_state;
};

/*
 * M3_NOTIFY_IND -
 */
struct M3_notify_ind {
	uint32_t PRIM_type;		/* always M3_NOTIFY_IND */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
	uint32_t NOTIFY_type;
};

/*
 * M3_ERROR_IND -
 */
struct M3_error_ind {
	uint32_t PRIM_type;		/* always M3_ERROR_IND */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
	uint32_t ERROR_type;
};

/*
 * M3_ASP_UP_REQ -
 */
struct M3_asp_up_req {
	uint32_t PRIM_type;		/* always M3_ASP_UP_REQ */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
};

/*
 * M3_ASP_UP_CON -
 */
struct M3_asp_up_con {
	uint32_t PRIM_type;		/* always M3_ASP_UP_CON */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
};

/*
 * M3_ASP_UP_IND -
 */
struct M3_asp_up_ind {
	uint32_t PRIM_type;		/* always M3_ASP_UP_IND */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
};

/*
 * M3_ASP_DOWN_REQ -
 */
struct M3_asp_down_req {
	uint32_t PRIM_type;		/* always M3_ASP_DOWN_REQ */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
};

/*
 * M3_ASP_DOWN_CON -
 */
struct M3_asp_down_con {
	uint32_t PRIM_type;		/* always M3_ASP_DOWN_CON */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
};

/*
 * M3_ASP_DOWN_IND -
 */
struct M3_asp_down_ind {
	uint32_t PRIM_type;		/* always M3_ASP_DOWN_IND */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
};

/*
 * M3_ASP_ACTIVE_REQ -
 */
struct M3_asp_active_req {
	uint32_t PRIM_type;		/* always M3_ASP_ACTIVE_REQ */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
};

/*
 * M3_ASP_ACTIVE_CON -
 */
struct M3_asp_active_con {
	uint32_t PRIM_type;		/* always M3_ASP_ACTIVE_CON */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
};

/*
 * M3_ASP_ACTIVE_IND -
 */
struct M3_asp_active_ind {
	uint32_t PRIM_type;		/* always M3_ASP_ACTIVE_IND */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
};

/*
 * M3_ASP_INACTIVE_REQ -
 */
struct M3_asp_inactive_req {
	uint32_t PRIM_type;		/* always M3_ASP_INACTIVE_REQ */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
};

/*
 * M3_ASP_INACTIVE_CON -
 */
struct M3_asp_inactive_con {
	uint32_t PRIM_type;		/* always M3_ASP_INACTIVE_CON */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
};

/*
 * M3_ASP_INACTIVE_IND -
 */
struct M3_asp_inactive_ind {
	uint32_t PRIM_type;		/* always M3_ASP_INACTIVE_IND */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
};

/*
 * M3_AS_ACTIVE_IND -
 */
struct M3_as_active_ind {
	uint32_t PRIM_type;		/* always M3_AS_ACTIVE_IND */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
};

/*
 * M3_AS_INACTIVE_IND -
 */
struct M3_as_inactive_ind {
	uint32_t PRIM_type;		/* always M3_AS_INACTIVE_IND */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
};

/*
 * M3_AS_DOWN_IND -
 */
struct M3_as_down_ind {
	uint32_t PRIM_type;		/* always M3_AS_DOWN_IND */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
};

/*
 * M3_REG_REQ -
 */
struct M3_reg_req {
	uint32_t PRIM_type;		/* always M3_REG_REQ */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t NETWORK_appearance;
	uint32_t LOCADDR_length;
	uint32_t LOCADDR_offset;
	uint32_t REMADDR_length;
	uint32_t REMADDR_offset;
};

/*
 * M3_REG_CON -
 */
struct M3_reg_con {
	uint32_t PRIM_type;		/* always M3_REG_CON */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
};

/*
 * M3_REG_IND -
 */
struct M3_reg_ind {
	uint32_t PRIM_type;		/* always M3_REG_IND */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
	uint32_t LOCADDR_length;
	uint32_t LOCADDR_offset;
	uint32_t REMADDR_length;
	uint32_t REMADDR_offset;
};

/*
 * M3_DEREG_REQ -
 */
struct M3_dereg_req {
	uint32_t PRIM_type;		/* always M3_DEREG_REQ */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
};

/*
 * M3_DEREG_CON -
 */
struct M3_dereg_con {
	uint32_t PRIM_type;		/* always M3_DEREG_CON */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
};

/*
 * M3_DEREG_IND -
 */
struct M3_dereg_ind {
	uint32_t PRIM_type;		/* always M3_DEREG_IND */
	uint32_t ASSOC_id;
	uint32_t ASP_id;
	uint32_t ROUTING_context;	/* routing context */
};

/*
 * M3_ERROR_OCC_IND - Negative receipt acknowledgement
 *
 * M_PCPROTO.  This primitive negative acknowledges receipt of a request primitive.  Only one
 * request primitive can have acknowlegement outstanding per Stream.
 */
struct M3_error_occ_ind {
	uint32_t PRIM_type;		/* always M3_ERROR_OCC_IND */
	uint32_t ERROR_prim;		/* request primitive in error */
	uint32_t ERROR_type;		/* type of error */
};

#endif				/* __SYS_M3UA_AS_H__ */
