/*
 *  tihdr.h  - header for Transport Provider Interface
 *
 *  Copyright (C) 1998 Ole Husgaard <sparre@login.dknet.dk>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 *  MA 02139, USA.
 *
 *
 *  I have written this header file to be as modern as possible.
 *  But the standard evolves:
 *  In a few places this differs from the old SVR4 TPI specs. If you have
 *  code written to the old specs you can define TPI_OLD_STYLE.
 *
 */

#ifndef _SYS_TIHDR_H
#define _SYS_TIHDR_H

#ident "@(#) LiS tihdr.h 2.1 4/4/99 11:34:28 "

#if 0
#define TPI_OLD_STYLE	/* Use old standard where it differs from new	*/
#endif

#include <sys/LiS/tpicommon.h>


/*
 *  downstream primitives  - sent by transport service user
 */
#define T_CONN_REQ	0	/* connection request			*/
#define T_CONN_RES	1	/* connection response			*/
#define T_DISCON_REQ	2	/* disconnect request			*/
#define T_DATA_REQ	3	/* data request				*/
#define T_EXDATA_REQ	4	/* expedited data request		*/
#define T_INFO_REQ	5	/* information request			*/
#define T_BIND_REQ	6	/* bind request				*/
#define T_UNBIND_REQ	7	/* unbind request			*/
#define T_UNITDATA_REQ	8	/* unitdata request			*/
#define T_OPTMGMT_REQ	9	/* options management request		*/
#define T_ORDREL_REQ	10	/* orderly release request		*/
#ifndef TPI_OLD_STYLE
#define T_OPTDATA_REQ	24	/* data with options request		*/
#define T_ADDR_REQ	25	/* address request			*/
#endif

/*
 *  upstream primitives  - sent by transport service provider
 */
#define T_CONN_IND	11	/* connection indication		*/
#define T_CONN_CON	12	/* connection confirmation		*/
#define T_DISCON_IND	13	/* disconnect indication		*/
#define T_DATA_IND	14	/* data indication			*/
#define T_EXDATA_IND	15	/* expedited data indication		*/
#define T_INFO_ACK	16	/* information acknowledgement		*/
#define T_BIND_ACK	17	/* bind acknowledgement			*/
#define T_ERROR_ACK	18	/* error acknowledgement		*/
#define T_OK_ACK	19	/* ok acknowledgement			*/
#define T_UNITDATA_IND	20	/* unitdata indication			*/
#define T_UDERROR_IND	21	/* unitdata error indication		*/
#define T_OPTMGMT_ACK	22	/* options management acknowledgement	*/
#define T_ORDREL_IND	23	/* orderly release indication		*/
#ifndef TPI_OLD_STYLE
#define T_OPTDATA_IND	26	/* data with options indication		*/
#define T_ADDR_ACK	27	/* address acknowledgement		*/
#endif

/*
 *  states of the transport provider interface
 */
#define TS_UNBND	0	/* unbound				*/
#define TS_WACK_BREQ	1	/* awaiting ack of T_BIND_REQ		*/
#define TS_WACK_UREQ	2	/* awaiting ack of T_UNBIND_REQ		*/
#define TS_IDLE		3	/* no connection			*/
#ifdef TPI_OLD_STYLE
#define TS_WACK_OPTREQ	4	/* awaiting ack of T_OPTMGMT_REQ?	*/
#endif
#define TS_WACK_CREQ	5	/* awaiting ack of T_CONN_REQ		*/
#define TS_WCON_CREQ	6	/* awaiting confirmation of T_CONN_REQ	*/
#define TS_WRES_CIND	7	/* awaiting response to T_CONN_IND	*/
#define TS_WACK_CRES	8	/* awaiting ack of T_CONN_RES		*/
#define TS_DATA_XFER	9	/* data transfer			*/
#define TS_WIND_ORDREL	10	/* awaiting T_ORDREL_IND		*/
#define TS_WREQ_ORDREL	11	/* awaiting T_ORDREL_REQ		*/
#define TS_WACK_DREQ6	12	/* awaiting ack of T_DISCON_REQ		*/
#define TS_WACK_DREQ7	13	/* awaiting ack of T_DISCON_REQ		*/
#define TS_WACK_DREQ9	14	/* awaiting ack of T_DISCON_REQ		*/
#define TS_WACK_DREQ10	15	/* awaiting ack of T_DISCON_REQ		*/
#define TS_WACK_DREQ11	16	/* awaiting ack of T_DISCON_REQ		*/

/*
 * Outgoing events  - these are initiated by the transport service user
 */
#define TE_BIND_REQ	0	/* bind request				*/
#define TE_UNBIND_REQ	1	/* unbind request			*/
#define TE_OPTMGMT_REQ	2	/* options management request		*/
#define TE_CONN_REQ	10	/* connection request			*/
#define TE_CONN_RES	11	/* connection response			*/
#define TE_DISCON_REQ	12	/* disconnect request			*/
#define TE_DATA_REQ	13	/* data request				*/
#define TE_EXDATA_REQ	14	/* expedited data request		*/
#define TE_ORDREL_REQ	15	/* orderly release request		*/
#define TE_UNITDATA_REQ	25	/* unitdata request			*/
#ifndef TPI_OLD_STYLE
#define TE_OPTDATA_REQ	28	/* data with options request		*/
#endif

/*
 * Incoming events  - these are initiated by the transport service provider
 */
#define TE_BIND_ACK	3	/* bind	acknowledgement			*/
#define TE_OPTMGMT_ACK	4	/* options management acknowledgement	*/
#define TE_ERROR_ACK	5	/* error acknowledgement		*/
#define TE_OK_ACK1	6	/* ok ack, outcnt == 0			*/
#define TE_OK_ACK2	7	/* ok ack, outcnt == 1, q == rq		*/
#define TE_OK_ACK3	8	/* ok ack, outcnt == 1, q != rq		*/
#define TE_OK_ACK4	9	/* ok ack, outcnt > 1			*/
#define TE_CONN_IND	16	/* connect indication			*/
#define TE_CONN_CON	17	/* connect confirmation			*/
#define TE_DATA_IND	18	/* data indication			*/
#define TE_EXDATA_IND	19	/* expedited data indication		*/
#define TE_ORDREL_IND	20	/* orderly release indication		*/
#define TE_DISCON_IND1	21	/* disconnect indication, outcnt == 0	*/
#define TE_DISCON_IND2	22	/* disconnect indication, outcnt == 1	*/
#define TE_DISCON_IND3	23	/* disconnect indication, outcnt > 1	*/
#define TE_PASS_CON	24	/* pass connection			*/
#define TE_UNITDATA_IND	26	/* unitdata indication			*/
#define TE_UDERROR_IND	27	/* unitdata error indication		*/
#ifndef TPI_OLD_STYLE
#define TE_OPTDATA_IND	29	/* data with options indication		*/
#endif


/*
 *  primitive structures
 */
struct T_ok_ack {
	t_scalar_t PRIM_type;		/* T_OK_ACK			*/
	t_scalar_t CORRECT_prim;	/* correct primitive		*/
};

struct T_error_ack {
	t_scalar_t PRIM_type;		/* T_ERROR_ACK			*/
	t_scalar_t ERROR_prim;		/* primitive in error		*/
	t_scalar_t TLI_error;		/* TLI error code		*/
	t_scalar_t UNIX_error;		/* Linux error code		*/
};

struct T_addr_req {
	t_scalar_t PRIM_type;		/* T_ADDR_REQ			*/
};

struct T_addr_ack {
	t_scalar_t PRIM_type;		/* T_ADDR_ACK			*/
	t_scalar_t LOCADDR_length;	/* local address length		*/
	t_scalar_t LOCADDR_offset;	/* local address offset		*/
	t_scalar_t REMADDR_length;	/* remote address length	*/
	t_scalar_t REMADDR_offset;	/* remote address offset	*/
	/* followed by the addresses */
};

struct T_info_req {
	t_scalar_t PRIM_type;		/* T_INFO_REQ			*/
};

struct T_info_ack {
	t_scalar_t PRIM_type;		/* T_INFO_ACK			*/
	t_scalar_t TSDU_size;		/* max data size		*/
	t_scalar_t ETSDU_size;		/* max expedited data size	*/
	t_scalar_t CDATA_size;		/* max connect data size	*/
	t_scalar_t DDATA_size;		/* max disconnect data size	*/
	t_scalar_t ADDR_size;		/* max address size		*/
	t_scalar_t OPT_size;		/* max options size		*/
	t_scalar_t TIDU_size;		/* max primitive data size	*/
	t_scalar_t SERV_type;		/* service type			*/
	t_scalar_t CURRENT_state;	/* current state		*/
	t_scalar_t PROVIDER_flag;	/* provider flags		*/
};

struct T_optmgmt_req {
	t_scalar_t PRIM_type;		/* T_OPTMGMT_REQ		*/
	t_scalar_t OPT_length;		/* options length		*/
	t_scalar_t OPT_offset;		/* options offset		*/
	t_scalar_t MGMT_flags;		/* options data flags		*/
	/* followed by the options */
};

struct T_optmgmt_ack {
	t_scalar_t PRIM_type;		/* T_OPTMGMT_ACK		*/
	t_scalar_t OPT_length;		/* options length		*/
	t_scalar_t OPT_offset;		/* options offset		*/
	t_scalar_t MGMT_flags;		/* options data flags		*/
	/* followed by the options */
};

struct T_bind_req {
	t_scalar_t PRIM_type;		/* T_BIND_REQ			*/
	t_scalar_t ADDR_length;		/* address length		*/
	t_scalar_t ADDR_offset;		/* address offset		*/
	t_uscalar_t CONIND_number;	/* max outstanding conn. reqs.	*/
	/* followed by the requested address */
};

struct T_bind_ack {
	t_scalar_t PRIM_type;		/* T_BIND_ACK			*/
	t_scalar_t ADDR_length;		/* address length		*/
	t_scalar_t ADDR_offset;		/* address offset		*/
	t_uscalar_t CONIND_number;	/* max outstanding conn. reqs.	*/
	/* followed by the bound address */
};

struct T_unbind_req {
	t_scalar_t PRIM_type;		/* T_UNBIND_REQ			*/
};

struct T_conn_req {
	t_scalar_t PRIM_type;		/* T_CONN_REQ			*/
	t_scalar_t DEST_length;		/* destination address length	*/
	t_scalar_t DEST_offset;		/* destination address offset	*/
	t_scalar_t OPT_length;		/* options length		*/
	t_scalar_t OPT_offset;		/* options offset		*/
	/* followed by the peer's address and options */
};

struct T_conn_con {
	t_scalar_t PRIM_type;		/* T_CONN_CON			*/
	t_scalar_t RES_length;		/* peer's address length	*/
	t_scalar_t RES_offset;		/* peer's address offset	*/
	t_scalar_t OPT_length;		/* options length		*/
	t_scalar_t OPT_offset;		/* options offset		*/
	t_scalar_t SEQ_number;		/* connection indication id	*/
	/* followed by the peer's address and options */
};

struct T_conn_ind {
	t_scalar_t PRIM_type;		/* T_CONN_IND			*/
	t_scalar_t SRC_length;		/* peer's address length	*/
	t_scalar_t SRC_offset;		/* peer's address offset	*/
	t_scalar_t OPT_length;		/* options length		*/
	t_scalar_t OPT_offset;		/* options offset		*/
	t_scalar_t SEQ_number;		/* connection indication id	*/
	/* followed by the peer's address and options */
};

struct T_conn_res {
	t_scalar_t PRIM_type;		/* T_CONN_RES			*/
#ifdef TPI_OLD_STYLE
	queue_t *QUEUE_ptr;		/* accepting queue		*/
#else
	t_scalar_t ACCEPTOR_id;		/* accepting endpoint		*/
#endif
	t_scalar_t OPT_length;		/* options length		*/
	t_scalar_t OPT_offset;		/* options offset		*/
	t_scalar_t SEQ_number;		/* connection indication id	*/
	/* followed by our options */
};

struct T_data_req {
	t_scalar_t PRIM_type;		/* T_DATA_REQ			*/
	t_scalar_t MORE_flag;		/* more data follows		*/
};

struct T_data_ind {
	t_scalar_t PRIM_type;		/* T_DATA_IND			*/
	t_scalar_t MORE_flag;		/* more data follows		*/
};

struct T_optdata_req {
	t_scalar_t PRIM_type;		/* T_OPTDATA_REQ		*/
	t_scalar_t DATA_flag;		/* options data flags		*/
	t_scalar_t OPT_length;		/* options length		*/
	t_scalar_t OPT_offset;		/* options offset		*/
	/* followed by the options */
};

struct T_optdata_ack {
	t_scalar_t PRIM_type;		/* T_OPTDATA_ACK		*/
	t_scalar_t DATA_flag;		/* options data flags		*/
	t_scalar_t OPT_length;		/* options length		*/
	t_scalar_t OPT_offset;		/* options offset		*/
	/* followed by the options */
};

struct T_exdata_req {
	t_scalar_t PRIM_type;		/* T_EXDATA_REQ			*/
	t_scalar_t MORE_flag;		/* more data follows		*/
};

struct T_exdata_ind {
	t_scalar_t PRIM_type;		/* T_EXDATA_IND			*/
	t_scalar_t MORE_flag;		/* more data follows		*/
};

struct T_discon_req {
	t_scalar_t PRIM_type;		/* T_DISCON_REQ			*/
	t_scalar_t SEQ_number;		/* sequence number		*/
};

struct T_discon_ind {
	t_scalar_t PRIM_type;		/* T_DISCON_IND			*/
	t_scalar_t DISCON_reason;	/* disconnect reason		*/
	t_scalar_t SEQ_number;		/* sequence number		*/
};

struct T_ordrel_req {
	t_scalar_t PRIM_type;		/* T_ORDREL_REQ			*/
};

struct T_ordrel_ind {
	t_scalar_t PRIM_type;		/* T_ORDREL_IND			*/
};

struct T_unitdata_req {
	t_scalar_t PRIM_type;		/* T_UNITDATA_REQ		*/
	t_scalar_t DEST_length;		/* destination address length	*/
	t_scalar_t DEST_offset;		/* destination address offset	*/
	t_scalar_t OPT_length;		/* options length		*/
	t_scalar_t OPT_offset;		/* options offset		*/
	/* followed by the address and options */
};

struct T_uderror_ind {
	t_scalar_t PRIM_type;		/* T_UDERROR_IND		*/
	t_scalar_t DEST_length;		/* destination address length	*/
	t_scalar_t DEST_offset;		/* destination address offset	*/
	t_scalar_t OPT_length;		/* options length		*/
	t_scalar_t OPT_offset;		/* options offset		*/
	t_scalar_t ERROR_type;		/* error code			*/
	/* followed by the address and options */
};

struct T_unitdata_ind {
	t_scalar_t PRIM_type;		/* T_UNITDATA_IND		*/
	t_scalar_t SRC_length;		/* source address length	*/
	t_scalar_t SRC_offset;		/* source address offset	*/
	t_scalar_t OPT_length;		/* options length		*/
	t_scalar_t OPT_offset;		/* options offset		*/
	/* followed by the address and options */
};

/*
 *  union of all TPI primitives
 */
union T_primitives {
	t_scalar_t type;
	struct T_ok_ack		ok_ack;
	struct T_error_ack	error_ack;
	struct T_addr_req	addr_req;
	struct T_addr_ack	addr_ack;
	struct T_info_req	info_req;
	struct T_info_ack	info_ack;
	struct T_optmgmt_req	optmgmt_req;
	struct T_optmgmt_ack	optmgmt_ack;
	struct T_bind_req	bind_req;
	struct T_bind_ack	bind_ack;
	struct T_unbind_req	unbind_req;
	struct T_conn_req	conn_req;
	struct T_conn_con	conn_con;
	struct T_conn_ind	conn_ind;
	struct T_conn_res	conn_res;
	struct T_data_req	data_req;
	struct T_data_ind	data_ind;
	struct T_optdata_req	optdata_req;
	struct T_optdata_ack	optdata_ack;
	struct T_exdata_req	exdata_req;
	struct T_exdata_ind	exdata_ind;
	struct T_discon_req	discon_req;
	struct T_discon_ind	discon_ind;
	struct T_ordrel_req	ordrel_req;
	struct T_ordrel_ind	ordrel_ind;
	struct T_unitdata_req	unitdata_req;
	struct T_uderror_ind	uderror_ind;
	struct T_unitdata_ind	unitdata_ind;
};

#endif /* _SYS_TIHDR_H */
