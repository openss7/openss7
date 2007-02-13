/*****************************************************************************

 @(#) $Id: devi.h,v 0.9.2.4 2007/02/13 14:05:28 brian Exp $

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

 Last Modified $Date: 2007/02/13 14:05:28 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: devi.h,v $
 Revision 0.9.2.4  2007/02/13 14:05:28  brian
 - corrected ulong and long for 32-bit compat

 *****************************************************************************/

#ifndef __DEVI_H__
#define __DEVI_H__

#ident "@(#) $RCSfile: devi.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

typedef int32_t dev_long;
typedef u_int32_t dev_ulong;
typedef u_int16_t dev_ushort;
typedef u_int8_t dev_uchar;

#define DEV_PROTO_BASE			 16L
#define DEV_DSTR_FIRST			( 1L + DEV_PROTO_BASE)
#define DEV_TX_FRAME_REQ		( 1L + DEV_PROTO_BASE)
#define DEV_RX_ENABLE_REQ		( 2L + DEV_PROTO_BASE)
#define DEV_TX_ENABLE_REQ		( 3L + DEV_PROTO_BASE)
#define DEV_DSTR_LAST			( 3L + DEV_PROTO_BASE)

#define DEV_USTR_LAST			(-1L - DEV_PROTO_BASE)
#define DEV_RX_FRAME_IND		(-2L - DEV_PROTO_BASE)
#define DEV_RX_COMPRESSED_FRAME_IND	(-3L - DEV_PROTO_BASE)
#define DEV_RX_ERROR_FRAME_IND		(-4L - DEV_PROTO_BASE)
#define DEV_RX_LOSS_OF_SYNC_IND		(-5L - DEV_PROTO_BASE)
#define DEV_RX_N_OCTETS_IND		(-6L - DEV_PROTO_BASE)
#define DEV_TX_REQUEST_IND		(-7L - DEV_PROTO_BASE)
#define DEV_USTR_FIRST			(-7L - DEV_PROTO_BASE)

/*
 *  DEVI PROTOCOL PRIMITIVES
 */

/*
 *  DEV_TX_FRAME_REQ, M_PROTO, M_PCPROTO (M_DATA)
 */
typedef struct {
	dev_long primitive;
} dev_tx_frame_req_t;

/*
 *  DEV_RX_ENABLE_REQ, M_PCPROTO
 */
typedef struct {
	dev_long primitive;
} dev_rx_enable_req_t;

/*
 *  DEV_TX_ENABLE_REQ, M_PCPROTO
 */
typedef struct {
	dev_long primitive;
} dev_tx_enable_req_t;

/*
 *  DEV_RX_FRAME_IND, M_PROTO, M_PCPROTO (M_DATA)
 */
typedef struct {
	dev_long primitive;
} dev_rx_frame_ind_t;

/*
 *  DEV_RX_COMPRESSED_FRAME_IND, M_PROTO, M_PCPROTO and M_DATA
 */
typedef struct {
	dev_long primitive;
	dev_ulong count;
} dev_rx_compressed_frame_ind_t;

/*
 *  DEV_RX_ERROR_FRAME_IND, M_PCPROTO
 */
typedef struct {
	dev_long primitive;
} dev_rx_error_frame_ind_t;

/*
 *  DEV_RX_LOSS_OF_SYNC_IND, M_PCPROTO
 */
typedef struct {
	dev_long primitive;
} dev_rx_loss_of_sync_ind_t;

/*
 *  DEV_RX_N_OCTETS_IND, M_PCPROTO
 */
typedef struct {
	dev_long primitive;
} dev_rx_n_octets_ind_t;

/*
 *  DEV_TX_REQUEST_IND, M_PCPROTO
 */
typedef struct {
	dev_long primitive;
} dev_tx_request_ind_t;

union DEV_primitives {
	dev_long primitive;
	dev_tx_frame_req_t tx_frame_req;
	dev_rx_enable_req_t rx_enable_req;
	dev_tx_enable_req_t tx_enable_req;
	dev_rx_frame_ind_t rx_frame_ind;
	dev_rx_compressed_frame_ind_t rx_compressed_frame_ind;
	dev_rx_error_frame_ind_t rx_error_frame_ind;
	dev_rx_loss_of_sync_ind_t rx_loss_of_sync_ind;
	dev_rx_n_octets_ind_t rx_n_octets_ind;
	dev_tx_request_ind_t tx_request_ind;
};

typedef union DEV_primitives dev_prim_t;

#define DEV_TX_FRAME_REQ_SIZE			sizeof(dev_tx_frame_req_t)
#define DEV_RX_ENABLE_REQ_SIZE			sizeof(dev_rx_enable_req_t)
#define DEV_TX_ENABLE_REQ_SIZE			sizeof(dev_tx_enable_req_t)
#define DEV_RX_FRAME_IND_SIZE			sizeof(dev_rx_frame_ind_t)
#define DEV_RX_COMPRESSED_FRAME_IND_SIZE	sizeof(dev_rx_compressed_frame_ind_t)
#define DEV_RX_ERROR_FRAME_IND_SIZE		sizeof(dev_rx_error_frame_ind_t)
#define DEV_RX_LOSS_OF_SYNC_IND_SIZE		sizeof(dev_rx_loss_of_sync_ind_t)
#define DEV_RX_N_OCTETS_IND_SIZE		sizeof(dev_rx_n_octets_ind_t)

#endif				/* __DEVI_H__ */
