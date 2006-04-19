/*****************************************************************************

 @(#) $Id: np.h,v 0.9.2.1 2006/04/18 18:00:13 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>

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

 Last Modified $Date: 2006/04/18 18:00:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: np.h,v $
 Revision 0.9.2.1  2006/04/18 18:00:13  brian
 - added some headers to move event definitions out

 *****************************************************************************/

#ifndef __LOCAL_NP_H__
#define __LOCAL_NP_H__

#ident "@(#) $RCSfile: np.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#include <sys/npi.h>

/* State flags */
#define NSF_UNBND	(1 << NS_UNBND		)
#define NSF_WACK_BREQ	(1 << NS_WACK_BREQ	)
#define NSF_WACK_UREQ	(1 << NS_WACK_UREQ	)
#define NSF_IDLE	(1 << NS_IDLE		)
#define NSF_WACK_OPTREQ	(1 << NS_WACK_OPTREQ	)
#define NSF_WACK_RRES	(1 << NS_WACK_RRES	)
#define NSF_WCON_CREQ	(1 << NS_WCON_CREQ	)
#define NSF_WRES_CIND	(1 << NS_WRES_CIND	)
#define NSF_WACK_CRES	(1 << NS_WACK_CRES	)
#define NSF_DATA_XFER	(1 << NS_DATA_XFER	)
#define NSF_WCON_RREQ	(1 << NS_WCON_RREQ	)
#define NSF_WRES_RIND	(1 << NS_WRES_RIND	)
#define NSF_WACK_DREQ6	(1 << NS_WACK_DREQ6	)
#define NSF_WACK_DREQ7	(1 << NS_WACK_DREQ7	)
#define NSF_WACK_DREQ9	(1 << NS_WACK_DREQ9	)
#define NSF_WACK_DREQ10	(1 << NS_WACK_DREQ10	)
#define NSF_WACK_DREQ11	(1 << NS_WACK_DREQ11	)
#define NSF_NOSTATES	(1 << NS_NOSTATES	)

/* State masks */
#define NSF_ALLSTATES	(NSF_NOSTATES - 1)
#define NSM_WACK_DREQ	(NSF_WACK_DREQ6 \
			|NSF_WACK_DREQ7 \
			|NSF_WACK_DREQ9 \
			|NSF_WACK_DREQ10 \
			|NSF_WACK_DREQ11)
#define NSM_LISTEN	(NSF_IDLE \
			|NSF_WRES_CIND)
#define NSM_CONNECTED	(NSF_WCON_CREQ\
			|NSF_WRES_CIND\
			|NSF_DATA_XFER\
			|NSF_WCON_RREQ\
			|NSF_WRES_RIND)
#define NSM_DISCONN	(NSF_IDLE\
			|NSF_UNBND)
#define NSM_INDATA	(NSF_DATA_XFER\
			|NSF_WCON_RREQ)
#define NSM_OUTDATA	(NSF_DATA_XFER\
			|NSF_WRES_RIND)

#ifndef N_PROVIDER
#define N_PROVIDER  0
#define N_USER	    1
#endif

struct ne_bind_req {
	np_ulong EVENT;
	uchar *ADDR_buffer;
	size_t ADDR_length;
	np_ulong CONIND_number;
	np_ulong BIND_flags;
	uchar *PROTOID_buffer;
	size_t PROTOID_length;
};

struct ne_bind_ack {
	np_ulong EVENT;
	uchar *ADDR_buffer;
	size_t ADDR_length;
	np_ulong CONIND_number;
	np_ulong TOKEN_value;
	uchar *PROTOID_buffer;
	size_t PROTOID_length;
};

struct ne_unbind_req {
	np_ulong EVENT;
};

struct ne_optmgmt_req {
	np_ulong EVENT;
	uchar *QOS_buffer;
	size_t QOS_length;
	np_ulong OPTMGMT_flags;
};

struct ne_conn_req {
	np_ulong EVENT;
	uchar *DEST_buffer;
	size_t DEST_length;
	uchar *QOS_buffer;
	size_t QOS_length;
	np_ulong CONN_flags;
	mblk_t *DATA_blocks;
	size_t DATA_length;
};

struct ne_conn_ind {
	np_ulong EVENT;
	uchar *DEST_buffer;
	size_t DEST_length;
	uchar *SRC_buffer;
	size_t SRC_length;
	mblk_t *SEQ_number;
	np_ulong CONN_flags;
	uchar *QOS_buffer;
	size_t QOS_length;
	mblk_t *DATA_blocks;
	size_t DATA_length;
};

struct ne_conn_res {
	np_ulong EVENT;
	uchar *RES_buffer;
	size_t RES_length;
	uchar *QOS_buffer;
	size_t QOS_length;
	np_ulong CONN_flags;
	struct np *TOKEN_value;
	mblk_t *SEQ_number;
	mblk_t *DATA_blocks;
	size_t DATA_length;
};

struct ne_conn_con {
	np_ulong EVENT;
	uchar *RES_buffer;
	size_t RES_length;
	np_ulong CONN_flags;
	uchar *QOS_buffer;
	size_t QOS_length;
};

struct ne_discon_req {
	np_ulong EVENT;
	np_ulong DISCON_reason;
	uchar *RES_buffer;
	size_t RES_length;
	mblk_t *SEQ_number;
	mblk_t *DATA_blocks;
	size_t DATA_length;
};

struct ne_data_req {
	np_ulong EVENT;
	np_ulong DATA_xfer_flags;
	mblk_t *DATA_blocks;
	size_t DATA_length;
};

struct ne_data_ind {
	np_ulong EVENT;
	np_ulong DATA_xfer_flags;
	mblk_t *DATA_blocks;
	size_t DATA_length;
};

struct ne_exdata_req {
	np_ulong EVENT;
	mblk_t *DATA_blocks;
	size_t DATA_length;
};

struct ne_exdata_ind {
	np_ulong EVENT;
	mblk_t *DATA_blocks;
	size_t DATA_length;
};

struct ne_reset_req {
	np_ulong EVENT;
	np_ulong RESET_reason;
};

struct ne_reset_req {
	np_ulong EVENT;
	np_ulong RESET_orig;
	np_ulong RESET_reason;
};

struct ne_reset_res {
	np_ulong EVENT;
};

struct ne_reset_con {
	np_ulong EVENT;
};

struct ne_datack_req {
	np_ulong EVENT;
};

struct ne_datack_ind {
	np_ulong EVENT;
};

struct ne_unitdata_req {
	np_ulong EVENT;
	uchar *DEST_buffer;
	size_t DEST_length;
	mblk_t *DATA_blocks;
	size_t DATA_length;
};

struct ne_unitdata_ind {
	np_ulong EVENT;
	uchar *DEST_buffer;
	size_t DEST_length;
	uchar *SRC_buffer;
	size_t SRC_length;
	mblk_t *DATA_blocks;
	size_t DATA_length;
};

struct ne_uderror_ind {
	np_ulong EVENT;
	uchar *DEST_buffer;
	size_t DEST_length;
	np_ulong ERROR_type;
	mblk_t *DATA_blocks;
	size_t DATA_length;
};

struct ne_error_ack {
	np_ulong EVENT;
	np_ulong ERROR_prim;
	np_ulong NPI_error;
	np_ulong UNIX_error;
};

struct ne_ok_ack {
	np_ulong EVENT;
	np_ulong CORRECT_prim;
	struct np *TOKEN_value;
	mblk_t *SEQ_number;
};

union ne_event {
	np_ulong EVENT;
	struct ne_bind_req bind_req;
	struct ne_bind_ack bind_ack;
	struct ne_unbind_req unbind_req;
	struct ne_optmgmt_req optmgmt_req;
	struct ne_conn_req conn_req;
	struct ne_conn_ind conn_ind;
	struct ne_conn_res conn_res;
	struct ne_conn_con conn_con;
	struct ne_discon_req discon_req;
	struct ne_discon_ind discon_ind;
	struct ne_data_req data_req;
	struct ne_data_ind data_ind;
	struct ne_exdata_req exdata_req;
	struct ne_exdata_ind exdata_ind;
	struct ne_reset_req reset_req;
	struct ne_reset_ind reset_ind;
	struct ne_reset_res reset_res;
	struct ne_reset_con reset_con;
	struct ne_datack_req datack_req;
	struct ne_datack_ind datack_ind;
	struct ne_unitdata_req unitdata_req;
	struct ne_unitdata_ind unitdata_ind;
	struct ne_uderror_ind uderror_ind;
	struct ne_error_ack error_ack;
	struct ne_ok_ack ok_ack;
	struct ne_pass_con pass_con;
};

#endif				/* __LOCAL_NP_H__ */
