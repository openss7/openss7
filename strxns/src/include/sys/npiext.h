/*****************************************************************************

 @(#) $Id: npiext.h,v 0.9.2.1 2008/06/28 08:46:48 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2008/06/28 08:46:48 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: npiext.h,v $
 Revision 0.9.2.1  2008/06/28 08:46:48  brian
 - added NPI extension header

 *****************************************************************************/

#ifndef __SYS_NPIEXT_H__
#define __SYS_NPIEXT_H__

#ident "@(#) $RCSfile: npiext.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* Following are GCOM extension primitives. */

#define N_EXT_BIND_REQ		129	/* extended bind request */
#define N_EXT2_BIND_REQ		130	/* second extended bind request */
#define N_EXT_CONN_REQ		131	/* extended connection request */
#define N_EXT_CONN_RES		132	/* extended connection response */
#define N_EXT_CONN_IND		133	/* extended connection indication */
#define N_EXT2_CONN_IND		134	/* second extended connection indication */
#define N_EXT_CONN_CON		135	/* extended connection confirmation */
#define N_EXT2_CONN_CON		136	/* second extended connection confirmation */
#define N_FLOW_REQ		137	/* flow control request */
#define N_DRAIN_REQ		138	/* data drain request */

typedef struct {
	np_ulong PRIM_type;		/* always N_EXT_BIND_REQ */
	np_ulong ADDR_length;		/* length of address */
	np_ulong ADDR_offset;		/* offset of address */
	np_ulong CONIND_number;		/* requested # of connect-indications to be queued */
	np_ulong BIND_flags;		/* bind flags */
#if 0
	np_ulong PROTOID_length;	/* length of bound protocol ids */
	np_ulong PROTOID_offset;	/* offset of bound protocol ids */
#endif
	np_ulong REM_length;
	np_ulong REM_offset;
	np_ulong LPA_number;
} N_ext_bind_req_t;

typedef struct {
	np_ulong PRIM_type;		/* always N_EXT2_BIND_REQ */
	np_ulong ADDR_length;		/* length of address */
	np_ulong ADDR_offset;		/* offset of address */
	np_ulong CONIND_number;		/* requested # of connect-indications to be queued */
	np_ulong BIND_flags;		/* bind flags */
#if 0
	np_ulong PROTOID_length;	/* length of bound protocol ids */
	np_ulong PROTOID_offset;	/* offset of bound protocol ids */
#endif
	np_ulong REM_length;
	np_ulong REM_offset;
	np_ulong LPA_number;
	uint8_t DATA_val[16];
	uint8_t DATA_mask[16];
} N_ext2_bind_req_t;

typedef struct {
	np_ulong PRIM_type;		/* always N_EXT_CONN_REQ */
	np_ulong DEST_length;		/* destination address length */
	np_ulong DEST_offset;		/* destination address offset */
	np_ulong CONN_flags;		/* bit masking for options flags */
	np_ulong QOS_length;		/* length of QOS parameter values */
	np_ulong QOS_offset;		/* offset of QOS parameter values */
	np_ulong FAC_length;
	np_ulong FAC_offset;
} N_ext_conn_req_t;

typedef struct {
	np_ulong PRIM_type;		/* always N_EXT_CONN_RES */
	np_ulong TOKEN_value;		/* NC response token value */
	np_ulong RES_length;		/* responding address length */
	np_ulong RES_offset;		/* responding address offset */
	np_ulong SEQ_number;		/* sequence number */
	np_ulong CONN_flags;		/* bit masking for options flags */
	np_ulong QOS_length;		/* length of QOS parameter values */
	np_ulong QOS_offset;		/* offset of QOS parameter values */
	np_ulong FAC_length;
	np_ulong FAC_offset;
} N_ext_conn_res_t;

typedef struct {
	np_ulong PRIM_type;		/* always N_EXT_CONN_IND */
	np_ulong DEST_length;		/* destination address length */
	np_ulong DEST_offset;		/* destination address offset */
	np_ulong SRC_length;		/* source address length */
	np_ulong SRC_offset;		/* source address offset */
	np_ulong SEQ_number;		/* sequence number */
	np_ulong CONN_flags;		/* bit masking for options flags */
	np_ulong QOS_length;		/* length of QOS parameter values */
	np_ulong QOS_offset;		/* offset of QOS parameter values */
	np_ulong FAC_length;
	np_ulong FAC_offset;
} N_ext_conn_ind_t;

typedef struct {
	np_ulong PRIM_type;		/* always N_EXT2_CONN_IND */
	np_ulong DEST_length;		/* destination address length */
	np_ulong DEST_offset;		/* destination address offset */
	np_ulong SRC_length;		/* source address length */
	np_ulong SRC_offset;		/* source address offset */
	np_ulong SEQ_number;		/* sequence number */
	np_ulong CONN_flags;		/* bit masking for options flags */
	np_ulong QOS_length;		/* length of QOS parameter values */
	np_ulong QOS_offset;		/* offset of QOS parameter values */
	np_ulong FAC_length;
	np_ulong FAC_offset;
	np_ulong LPA_number;
} N_ext2_conn_ind_t;

typedef struct {
	np_ulong PRIM_type;		/* always N_EXT_CONN_CON */
	np_ulong RES_length;		/* responding address length */
	np_ulong RES_offset;		/* responding address offset */
	np_ulong CONN_flags;		/* bit masking for options flags */
	np_ulong QOS_length;		/* length of QOS parameter values */
	np_ulong QOS_offset;		/* offset of QOS parameter values */
	np_ulong FAC_length;
	np_ulong FAC_offset;
} N_ext_conn_con_t;

typedef struct {
	np_ulong PRIM_type;		/* always N_EXT2_CONN_CON */
	np_ulong RES_length;		/* responding address length */
	np_ulong RES_offset;		/* responding address offset */
	np_ulong CONN_flags;		/* bit masking for options flags */
	np_ulong QOS_length;		/* length of QOS parameter values */
	np_ulong QOS_offset;		/* offset of QOS parameter values */
	np_ulong FAC_length;
	np_ulong FAC_offset;
	np_ulong LPA_number;
} N_ext2_conn_con_t;

#define NP_FC_ZERO	0x00001000	/* set level to zero */
#define NP_FC_INFINITE	0x0001ffff	/* set level to infinite */

typedef struct {
	np_ulong PRIM_type;		/* always N_FLOW_REQ */
	np_ulong flow_incr;		/* flow control increment */
} N_flow_req_t;

#define N_DRAIN_IMMED	1	/* acknowledge immediately */
#define N_DRAIN_SENT	2	/* acknowledge after sent */
#define N_DRAIN_ACKD	3	/* acknowledge after acknowledged */

typedef struct {
	np_ulong PRIM_type;		/* always N_DRAIN_REQ */
	np_ulong drain_option;		/* type of drain condition */
} N_drain_req_t;

#endif				/* __SYS_NPIEXT_H__ */
