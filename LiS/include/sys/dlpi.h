/* 
 * dlpi.h 	   : Data Link Provider Interface
 * Author          : David Grothe
 * Created On      : Tue May 31 21:40:37 1994
 * Last Modified By: $Author: dave $
 * Last Modified On: $Date: 1996/02/11 20:53:12 $
 * Update Count    : $Revision: 1.2 $
 * RCS Id          : $Id: dlpi.h,v 1.2 1996/02/11 20:53:12 dave Exp $
 * Status          : ($State: Exp $) 
 * Purpose         : STREAMS interface to link level and MAC layer drivers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 */

#ifndef _DLPI_H
#define _DLPI_H		1		/* define several markers */
#define	 DLPI_H		1
#define	 LIS_DLPI_H	1		/* unique to this version of dlpi.h */

#ident "@(#) LiS dlpi.h 1.11 9/18/03"

/************************************************************************
*                           Reference                                   *
*************************************************************************
*									*
* This file follows the DLPI specification published by UNIX		*
* International, version 2.0.0 dated August 20, 1991.			*
*									*
* Added changes from DLPI Corrigendum U019 thanks to Ole Husgaard	*
* <sparre@login.dknet.dk>.						*
*									*
************************************************************************/


/************************************************************************
*                           DLPI Types                                  *
*************************************************************************
*									*
* Unlike other dlpi.h files, this one defines its own types so that	*
* it can be used without depending upon system specific types.h files.	*
*									*
************************************************************************/
#if !defined(_LINUX_TYPES_H) && !defined(_INTTYPES_H)
# if defined(__sparcv9)			/* 64-bit SPARC compile */
    typedef unsigned int	dl_ulong ;
    typedef unsigned short	dl_ushort ;
# else
    typedef unsigned long	dl_ulong ;
    typedef unsigned short	dl_ushort ;
# endif
#else
    typedef uint32_t		dl_ulong ;
    typedef uint16_t		dl_ushort ;
#endif


/************************************************************************
*                          Message Structures                           *
*************************************************************************
*									*
* These structures are located in an M_PROTO or an M_PCPROTO message	*
* starting at b_rptr.  Some message types also have an attached		*
* M_DATA chained through the b_cont field.				*
*									*
* Each primitive is sent as a stream message.  It is possible that	*
* the messages may be viewed as a sequence of bytes that have the	*
* following form without any padding.  The structure definition		*
* of the following messages may have to change depending on the		*
* underlying hardware architecture and crossing of a hardware		*
* boundary with a different hardware architecture.			*
*									*
* Fields in the primitives having a name of the form "dl_reserved"	*
* cannot be used and have the value of binary zero, no bits turned on.	*
*									*
************************************************************************/

/*
 * Local Management Primitives
 */

typedef struct
{
    dl_ulong		dl_primitive ;

} dl_info_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_max_sdu ;
    dl_ulong		dl_min_sdu ;
    dl_ulong		dl_addr_length ;
    dl_ulong		dl_mac_type ;
    dl_ulong		dl_reserved ;
    dl_ulong		dl_current_state ;
    long		dl_sap_length ;			/* yes, signed long */
    dl_ulong		dl_service_mode ;
    dl_ulong		dl_qos_length ;
    dl_ulong		dl_qos_offset ;
    dl_ulong		dl_qos_range_length ;
    dl_ulong		dl_qos_range_offset ;
    long		dl_provider_style ;		/* yes, signed long */
    dl_ulong		dl_addr_offset ;
    dl_ulong		dl_version ;
    dl_ulong		dl_brdcst_addr_length ;
    dl_ulong		dl_brdcst_addr_offset ;
    dl_ulong		dl_growth ;

} dl_info_ack_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_ppa ;

} dl_attach_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;

} dl_detach_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_sap ;
    dl_ulong		dl_max_conind ;
    dl_ushort		dl_service_mode ;
    dl_ushort		dl_conn_mgmt ;
    dl_ulong		dl_xidtest_flg ;

} dl_bind_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_sap ;
    dl_ulong		dl_addr_length ;
    dl_ulong		dl_addr_offset ;
    dl_ulong		dl_max_conind ;
    dl_ulong		dl_xidtest_flg ;

} dl_bind_ack_t ;

typedef struct
{
    dl_ulong		dl_primitive ;

} dl_unbind_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_subs_sap_offset ;
    dl_ulong		dl_subs_sap_length ;
    dl_ulong		dl_subs_bind_class ;

} dl_subs_bind_req_t ;
#define	dl_subs_sap_len	dl_subs_sap_length	/* SCO compatibility */


typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_subs_sap_offset ;
    dl_ulong		dl_subs_sap_length ;

} dl_subs_bind_ack_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_subs_sap_offset ;
    dl_ulong		dl_subs_sap_length ;

} dl_subs_unbind_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_addr_length ;
    dl_ulong		dl_addr_offset ;

} dl_enabmulti_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_addr_length ;
    dl_ulong		dl_addr_offset ;

} dl_disabmulti_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_level ;

} dl_promiscon_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_level ;

} dl_promiscoff_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_correct_primitive ;

} dl_ok_ack_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_error_primitive ;
    dl_ulong		dl_errno ;
    dl_ulong		dl_unix_errno ;

} dl_error_ack_t ;


/*
 * Connection-mode Service Primitives
 */

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_dest_addr_length ;
    dl_ulong		dl_dest_addr_offset ;
    dl_ulong		dl_qos_length ;
    dl_ulong		dl_qos_offset ;
    dl_ulong		dl_growth ;

} dl_connect_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_correlation ;
    dl_ulong		dl_called_addr_length ;
    dl_ulong		dl_called_addr_offset ;
    dl_ulong		dl_calling_addr_length ;
    dl_ulong		dl_calling_addr_offset ;
    dl_ulong		dl_qos_length ;
    dl_ulong		dl_qos_offset ;
    dl_ulong		dl_growth ;

} dl_connect_ind_t ;


typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_correlation ;
    dl_ulong		dl_resp_token ;
    dl_ulong		dl_qos_length ;
    dl_ulong		dl_qos_offset ;
    dl_ulong		dl_growth ;

} dl_connect_res_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_resp_addr_length ;
    dl_ulong		dl_resp_addr_offset ;
    dl_ulong		dl_qos_length ;
    dl_ulong		dl_qos_offset ;
    dl_ulong		dl_growth ;

} dl_connect_con_t ;

typedef struct
{
    dl_ulong		dl_primitive ;

} dl_token_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_token ;

} dl_token_ack_t ;


typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_reason ;
    dl_ulong		dl_correlation ;

} dl_disconnect_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_originator ;
    dl_ulong		dl_reason ;
    dl_ulong		dl_correlation ;

} dl_disconnect_ind_t ;

typedef struct
{
    dl_ulong		dl_primitive ;

} dl_reset_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_originator ;
    dl_ulong		dl_reason ;

} dl_reset_ind_t ;

typedef struct
{
    dl_ulong		dl_primitive ;

} dl_reset_res_t ;

typedef struct
{
    dl_ulong		dl_primitive ;

} dl_reset_con_t ;



/*
 * Connectionless-mode Service Primitives
 */

typedef struct
{
    dl_ulong		dl_min ;
    dl_ulong		dl_max ;

} dl_priority_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_dest_addr_length ;
    dl_ulong		dl_dest_addr_offset ;
    dl_priority_t	dl_priority ;

} dl_unitdata_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_dest_addr_length ;
    dl_ulong		dl_dest_addr_offset ;
    dl_ulong		dl_src_addr_length ;
    dl_ulong		dl_src_addr_offset ;
    dl_ulong		dl_group_address ;

} dl_unitdata_ind_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_dest_addr_length ;
    dl_ulong		dl_dest_addr_offset ;
    dl_ulong		dl_unix_errno ;
    dl_ulong		dl_errno ;

} dl_uderror_ind_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_qos_length ;
    dl_ulong		dl_qos_offset ;

} dl_udqos_req_t ;



/*
 * XID and TEST Operations Primitives
 */

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_flag ;
    dl_ulong		dl_dest_addr_length ;
    dl_ulong		dl_dest_addr_offset ;

} dl_test_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_flag ;
    dl_ulong		dl_dest_addr_length ;
    dl_ulong		dl_dest_addr_offset ;
    dl_ulong		dl_src_addr_length ;
    dl_ulong		dl_src_addr_offset ;

} dl_test_ind_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_flag ;
    dl_ulong		dl_dest_addr_length ;
    dl_ulong		dl_dest_addr_offset ;

} dl_test_res_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_flag ;
    dl_ulong		dl_dest_addr_length ;
    dl_ulong		dl_dest_addr_offset ;
    dl_ulong		dl_src_addr_length ;
    dl_ulong		dl_src_addr_offset ;

} dl_test_con_t ;


typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_flag ;
    dl_ulong		dl_dest_addr_length ;
    dl_ulong		dl_dest_addr_offset ;

} dl_xid_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_flag ;
    dl_ulong		dl_dest_addr_length ;
    dl_ulong		dl_dest_addr_offset ;
    dl_ulong		dl_src_addr_length ;
    dl_ulong		dl_src_addr_offset ;

} dl_xid_ind_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_flag ;
    dl_ulong		dl_dest_addr_length ;
    dl_ulong		dl_dest_addr_offset ;

} dl_xid_res_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_flag ;
    dl_ulong		dl_dest_addr_length ;
    dl_ulong		dl_dest_addr_offset ;
    dl_ulong		dl_src_addr_length ;
    dl_ulong		dl_src_addr_offset ;

} dl_xid_con_t ;

/*
 * Acknowledged Connectionless-mode Service Primitives
 */

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_correlation ;
    dl_ulong		dl_dest_addr_length ;
    dl_ulong		dl_dest_addr_offset ;
    dl_ulong		dl_src_addr_length ;
    dl_ulong		dl_src_addr_offset ;
    dl_ulong		dl_priority ;
    dl_ulong		dl_service_class ;

} dl_data_ack_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_dest_addr_length ;
    dl_ulong		dl_dest_addr_offset ;
    dl_ulong		dl_src_addr_length ;
    dl_ulong		dl_src_addr_offset ;
    dl_ulong		dl_priority ;
    dl_ulong		dl_service_class ;

} dl_data_ack_ind_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_correlation ;
    dl_ulong		dl_status ;

} dl_data_ack_status_ind_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_correlation ;
    dl_ulong		dl_dest_addr_length ;
    dl_ulong		dl_dest_addr_offset ;
    dl_ulong		dl_src_addr_length ;
    dl_ulong		dl_src_addr_offset ;
    dl_ulong		dl_priority ;
    dl_ulong		dl_service_class ;

} dl_reply_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_dest_addr_length ;
    dl_ulong		dl_dest_addr_offset ;
    dl_ulong		dl_src_addr_length ;
    dl_ulong		dl_src_addr_offset ;
    dl_ulong		dl_priority ;
    dl_ulong		dl_service_class ;

} dl_reply_ind_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_correlation ;
    dl_ulong		dl_status ;

} dl_reply_status_ind_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_correlation ;
    dl_ulong		dl_src_addr_length ;
    dl_ulong		dl_src_addr_offset ;

} dl_reply_update_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_correlation ;
    dl_ulong		dl_status ;

} dl_reply_update_status_ind_t ;


/*
 * QOS parameter structures
 */

typedef struct
{
    dl_ulong		dl_target_value ;
    dl_ulong		dl_accept_value ;

} dl_through_t ;

typedef struct
{
    dl_ulong		dl_target_value ;
    dl_ulong		dl_accept_value ;

} dl_transdelay_t ;

typedef struct
{
    dl_ulong		dl_min ;
    dl_ulong		dl_max ;

} dl_protect_t ;

typedef struct
{
    dl_ulong		dl_disc_prob ;
    dl_ulong		dl_reset_prob ;

} dl_resilience_t ;


/*
 * QOS Data Structures
 */

typedef struct
{
    dl_ulong		dl_qos_type ;
    dl_through_t	dl_rcv_throughput ;
    dl_transdelay_t	dl_rcv_trans_delay ;
    dl_through_t	dl_xmt_throughput ;
    dl_transdelay_t	dl_xmt_trans_delay ;
    dl_priority_t	dl_priority ;
    dl_protect_t	dl_protection ;
    dl_ulong		dl_residual_error ;
    dl_resilience_t	dl_resilience ;

} dl_qos_co_range1_t ;

typedef struct
{
    dl_ulong		dl_qos_type ;
    dl_through_t	dl_rcv_throughput ;
    dl_transdelay_t	dl_rcv_trans_delay ;
    dl_through_t	dl_xmt_throughput ;
    dl_transdelay_t	dl_xmt_trans_delay ;
    dl_priority_t	dl_priority ;
    dl_protect_t	dl_protection ;
    dl_ulong		dl_residual_error ;
    dl_resilience_t	dl_resilience ;

} dl_qos_co_sel1_t ;

typedef struct
{
    dl_ulong		dl_qos_type ;
    dl_transdelay_t	dl_trans_delay ;
    dl_priority_t	dl_priority ;
    dl_protect_t	dl_protection ;
    dl_ulong		dl_residual_error ;

} dl_qos_cl_range1_t ;

typedef struct
{
    dl_ulong		dl_qos_type ;
    long		dl_trans_delay ;
    long		dl_priority ;
    long		dl_protection ;
    long		dl_residual_error ;

} dl_qos_cl_sel1_t ;



/*
 * Physical Layer Management Primitives
 */

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_addr_type ;

} dl_phys_addr_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_addr_length ;
    dl_ulong		dl_addr_offset ;

} dl_phys_addr_ack_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_addr_length ;
    dl_ulong		dl_addr_offset ;

} dl_set_phys_addr_req_t ;


/*
 * Statistics Extensions
 */

typedef struct
{
    dl_ulong		dl_primitive ;

} dl_get_statistics_req_t ;

typedef struct
{
    dl_ulong		dl_primitive ;
    dl_ulong		dl_stat_length ;
    dl_ulong		dl_stat_offset ;

} dl_get_statistics_ack_t ;

/*
 * Union of all primitives
 */
union DL_primitives
{
    dl_ulong			dl_primitive ;
    dl_info_req_t           	info_req;
    dl_info_ack_t           	info_ack;
    dl_attach_req_t         	attach_req;
    dl_detach_req_t		detach_req;
    dl_bind_req_t		bind_req;
    dl_bind_ack_t		bind_ack;
    dl_unbind_req_t		unbind_req;
    dl_subs_bind_req_t		subs_bind_req;
    dl_subs_bind_ack_t		subs_bind_ack;
    dl_subs_unbind_req_t	subs_unbind_req;
    dl_enabmulti_req_t		enabmulti_req;
    dl_disabmulti_req_t		disabmulti_req;
    dl_promiscon_req_t		promiscon_req;
    dl_promiscoff_req_t		promiscoff_req;
    dl_ok_ack_t			ok_ack;
    dl_error_ack_t		error_ack;
    dl_connect_req_t		connect_req;
    dl_connect_ind_t		connect_ind;
    dl_connect_res_t		connect_res;
    dl_connect_con_t		connect_con;
    dl_token_req_t		token_req;
    dl_token_ack_t		token_ack;
    dl_disconnect_req_t		disconnect_req;
    dl_disconnect_ind_t		disconnect_ind;
    dl_reset_req_t		reset_req;
    dl_reset_ind_t		reset_ind;
    dl_reset_res_t		reset_res;
    dl_reset_con_t		reset_con;
    dl_unitdata_req_t		unitdata_req;
    dl_unitdata_ind_t		unitdata_ind;
    dl_uderror_ind_t		uderror_ind;
    dl_udqos_req_t		udqos_req;
    dl_test_req_t		test_req;
    dl_test_ind_t		test_ind;
    dl_test_res_t		test_res;
    dl_test_con_t		test_con;
    dl_xid_req_t		xid_req;
    dl_xid_ind_t		xid_ind;
    dl_xid_res_t		xid_res;
    dl_xid_con_t		xid_con;
    dl_data_ack_req_t		data_ack_req;
    dl_data_ack_ind_t		data_ack_ind;
    dl_data_ack_status_ind_t	data_ack_status_ind;
    dl_reply_req_t		reply_req;
    dl_reply_ind_t		reply_ind;
    dl_reply_status_ind_t	reply_status_ind;
    dl_reply_update_req_t	reply_update_req;
    dl_reply_update_status_ind_t reply_update_status_ind;
    dl_phys_addr_req_t		physaddr_req;
    dl_phys_addr_ack_t		physaddr_ack;
    dl_set_phys_addr_req_t	set_physaddr_req;
    dl_get_statistics_req_t	get_statistics_req;
    dl_get_statistics_ack_t	get_statistics_ack;
} ;



/*
 * Sizes of the primitives
 */

#define	DL_INFO_REQ_SIZE		sizeof(dl_info_req_t)
#define	DL_INFO_ACK_SIZE		sizeof(dl_info_ack_t)
#define DL_ATTACH_REQ_SIZE		sizeof(dl_attach_req_t)
#define DL_DETACH_REQ_SIZE		sizeof(dl_detach_req_t)
#define DL_BIND_REQ_SIZE		sizeof(dl_bind_req_t)
#define DL_BIND_ACK_SIZE		sizeof(dl_bind_ack_t)
#define DL_UNBIND_REQ_SIZE		sizeof(dl_unbind_req_t)
#define DL_SUBS_BIND_REQ_SIZE		sizeof(dl_subs_bind_req_t)
#define DL_SUBS_BIND_ACK_SIZE		sizeof(dl_subs_bind_ack_t)
#define DL_SUBS_UNBIND_REQ_SIZE		sizeof(dl_subs_unbind_req_t)
#define DL_ENABMULTI_REQ_SIZE		sizeof(dl_enabmulti_req_t)
#define DL_DISABMULTI_REQ_SIZE		sizeof(dl_disabmulti_req_t)
#define DL_PROMISCON_REQ_SIZE		sizeof(dl_promiscon_req_t)
#define DL_PROMISCOFF_REQ_SIZE		sizeof(dl_promiscoff_req_t)
#define DL_OK_ACK_SIZE			sizeof(dl_ok_ack_t)
#define DL_ERROR_ACK_SIZE		sizeof(dl_error_ack_t)
#define DL_CONNECT_REQ_SIZE		sizeof(dl_connect_req_t)
#define DL_CONNECT_IND_SIZE		sizeof(dl_connect_ind_t)
#define DL_CONNECT_RES_SIZE		sizeof(dl_connect_res_t)
#define DL_CONNECT_CON_SIZE		sizeof(dl_connect_con_t)
#define DL_TOKEN_REQ_SIZE		sizeof(dl_token_req_t)
#define DL_TOKEN_ACK_SIZE		sizeof(dl_token_ack_t)
#define DL_DISCONNECT_REQ_SIZE		sizeof(dl_disconnect_req_t)
#define DL_DISCONNECT_IND_SIZE		sizeof(dl_disconnect_ind_t)
#define DL_RESET_REQ_SIZE		sizeof(dl_reset_req_t)
#define DL_RESET_IND_SIZE		sizeof(dl_reset_ind_t)
#define DL_RESET_RES_SIZE		sizeof(dl_reset_res_t)
#define DL_RESET_CON_SIZE		sizeof(dl_reset_con_t)
#define DL_PRIORITY_SIZE		sizeof(dl_priority_t)
#define DL_UNITDATA_REQ_SIZE		sizeof(dl_unitdata_req_t)
#define DL_UNITDATA_IND_SIZE		sizeof(dl_unitdata_ind_t)
#define DL_UDERROR_IND_SIZE		sizeof(dl_uderror_ind_t)
#define DL_UDQOS_REQ_SIZE		sizeof(dl_udqos_req_t)
#define DL_TEST_REQ_SIZE		sizeof(dl_test_req_t)
#define DL_TEST_IND_SIZE		sizeof(dl_test_ind_t)
#define DL_TEST_RES_SIZE		sizeof(dl_test_res_t)
#define DL_TEST_CON_SIZE		sizeof(dl_test_con_t)
#define DL_XID_REQ_SIZE			sizeof(dl_xid_req_t)
#define DL_XID_IND_SIZE			sizeof(dl_xid_ind_t)
#define DL_XID_RES_SIZE			sizeof(dl_xid_res_t)
#define DL_XID_CON_SIZE			sizeof(dl_xid_con_t)
#define DL_DATA_ACK_REQ_SIZE		sizeof(dl_data_ack_req_t)
#define DL_DATA_ACK_IND_SIZE		sizeof(dl_data_ack_ind_t)
#define DL_DATA_ACK_STATUS_IND_SIZE	sizeof(dl_data_ack_status_ind_t)
#define DL_REPLY_REQ_SIZE		sizeof(dl_reply_req_t)
#define DL_REPLY_IND_SIZE		sizeof(dl_reply_ind_t)
#define DL_REPLY_STATUS_IND_SIZE	sizeof(dl_reply_status_ind_t)
#define DL_REPLY_UPDATE_REQ_SIZE	sizeof(dl_reply_update_req_t)
#define DL_REPLY_UPDATE_STATUS_IND_SIZE	sizeof(dl_reply_update_status_ind_t)
#define DL_PHYS_ADDR_REQ_SIZE		sizeof(dl_phys_addr_req_t)
#define DL_PHYS_ADDR_ACK_SIZE		sizeof(dl_phys_addr_ack_t)
#define DL_SET_PHYS_ADDR_REQ_SIZE	sizeof(dl_set_phys_addr_req_t)
#define DL_GET_STATISTICS_REQ_SIZE	sizeof(dl_get_statistics_req_t)
#define DL_GET_STATISTICS_ACK_SIZE	sizeof(dl_get_statistics_ack_t)



/************************************************************************
*                          Field Values                                 *
*************************************************************************
*									*
* The following #defines specifiy the values used in the fields of	*
* the structures declared above.					*
*									*
* These values are taken from the listing of dlpi.h given in the	*
* DLPI version 2 specification.  The listing in that specification	*
* contains no copyright, so the following has been entered pretty	*
* much literally from the listing.					*
*									*
************************************************************************/

/*
 * DLPI revision definition history
 */
#define	DL_CURRENT_VERSION	0x02		/* current version of DLPI */
#define	DL_VERSION_2		0x02		/* DLPI March 12, 1991 */


/************************************************************************
*                          Primitive Types                              *
************************************************************************/

/*
 * Primitives for Local Managemet Services
 */
#define	DL_INFO_REQ		0x00
#define	DL_INFO_ACK		0x03
#define	DL_ATTACH_REQ		0x0b
#define DL_DETACH_REQ		0x0c
#define DL_BIND_REQ		0x01
#define DL_BIND_ACK		0x04
#define DL_UNBIND_REQ		0x02
#define DL_SUBS_BIND_REQ	0x1b
#define DL_SUBS_BIND_ACK	0x1c
#define DL_SUBS_UNBIND_REQ	0x15
#define DL_ENABMULTI_REQ	0x1d
#define DL_DISABMULTI_REQ	0x1e
#define DL_PROMISCON_REQ	0x1f
#define DL_PROMISCOFF_REQ	0x20
#define DL_OK_ACK		0x06
#define DL_ERROR_ACK		0x05
/*
 * Primitives used for Connection-Oriented Service
 */
#define DL_CONNECT_REQ		0x0d
#define DL_CONNECT_IND		0x0e
#define DL_CONNECT_RES		0x0f
#define DL_CONNECT_CON		0x10
#define DL_TOKEN_REQ		0x11
#define DL_TOKEN_ACK		0x12
#define DL_DISCONNECT_REQ	0x13
#define DL_DISCONNECT_IND	0x14
#define DL_RESET_REQ		0x17
#define DL_RESET_IND		0x18
#define DL_RESET_RES		0x19
#define DL_RESET_CON		0x1a
/*
 * Primitives used for Connectionless Service
 */
#define DL_UNITDATA_REQ		0x07
#define DL_UNITDATA_IND		0x08
#define DL_UDERROR_IND		0x09
#define DL_UDQOS_REQ		0x0a
/*
 * Primitives for TEST and XID
 */
#define DL_TEST_REQ		0x2d
#define DL_TEST_IND		0x2e
#define DL_TEST_RES		0x2f
#define DL_TEST_CON		0x30
#define DL_XID_REQ		0x29
#define DL_XID_IND		0x2a
#define DL_XID_RES		0x2b
#define DL_XID_CON		0x2c
/*
 * Primitives used for Acknowledged Connectionless Service
 */
#define DL_DATA_ACK_REQ		0x21
#define DL_DATA_ACK_IND		0x22
#define DL_DATA_ACK_STATUS_IND	0x23
#define DL_REPLY_REQ		0x24
#define DL_REPLY_IND		0x25
#define DL_REPLY_STATUS_IND	0x26
#define DL_REPLY_UPDATE_REQ	0x27
#define DL_REPLY_UPDATE_STATUS_IND 0x28
/*
 * Primitives to get and set the physical address
 */
#define DL_PHYS_ADDR_REQ	0x31
#define DL_PHYS_ADDR_ACK	0x32
#define DL_SET_PHYS_ADDR_REQ	0x33
/*
 * Primitives to get statistics
 */
#define DL_GET_STATISTICS_REQ	0x34
#define DL_GET_STATISTICS_ACK	0x35


/************************************************************************
*                       DLPI Interface States                           *
*************************************************************************
*									*
* Refer to DLPI spec for the meaning of these states.			*
*									*
************************************************************************/

#define DL_UNATTACHED		0x04
#define DL_ATTACH_PENDING	0x05
#define DL_DETACH_PENDING	0x06
#define DL_UNBOUND		0x00
#define DL_BIND_PENDING		0x01
#define DL_UNBIND_PENDING	0x02
#define DL_IDLE			0x03
#define DL_UDQOS_PENDING	0x07
#define DL_OUTCON_PENDING	0x08
#define DL_INCON_PENDING	0x09
#define DL_CONN_RES_PENDING	0x0a
#define DL_DATAXFER		0x0b
#define DL_USER_RESET_PENDING	0x0c
#define DL_PROV_RESET_PENDING	0x0d
#define DL_RESET_RES_PENDING	0x0e
#define DL_DISCON8_PENDING	0x0f
#define DL_DISCON9_PENDING	0x10
#define DL_DISCON11_PENDING	0x11
#define DL_DISCON12_PENDING	0x12
#define DL_DISCON13_PENDING	0x13
#define DL_SUBS_BIND_PND	0x14
#define DL_SUBS_UNBIND_PND	0x15

/************************************************************************
*                        DL_ERROR_ACK Error Values                      *
*************************************************************************
*									*
* These are the values of the dl_errno field.				*
*									*
************************************************************************/

#define DL_ACCESS	0x02	/* Improper permissions */
#define DL_BADADDR	0x01	/* DLSAP addr in improper format or invalid */
#define DL_BADCORR	0x05	/* Seq nr not from outstanding DL_CONN_IND */
#define DL_BADDATA	0x06	/* User data exceeded provider limit */
#define DL_BADPPA	0x08	/* PPA was invalid */
#define DL_BADPRIM	0x09	/* Primitive received is not know by DLS prov */
#define DL_BADQOSPARAM	0x0a	/* QOS params contained invalid values */
#define DL_BADQOSTYPE	0x0b	/* QOS structure type unknown or unsupported */
#define DL_BADSAP	0x00	/* Bad LSAP selector, LLI compatibility */
#define DL_BADTOKEN	0x0c	/* Token not associated with active stream */
#define DL_BOUND	0x0d	/* Attempted second bind with dl_max_conind or
				 * dl_conn_mgmt > 0 on same DLSAP or PPA
				 */
#define DL_INITFAILED	0x0e	/* Physical link initialization failed */
#define DL_NOADDR	0x0f	/* Provider couldn't allocate alternate addr */
#define DL_NOTINIT	0x10	/* Physical link not initialized */
#define DL_OUTSTATE	0x03	/* Primitive issued in improper state */
#define DL_SYSERR	0x04	/* UNIX system error occurred */
#define DL_UNSUPPORTED	0x07	/* Requested service not supplied by provider */
#define DL_UNDELIVERABLE 0x11	/* Previous data unit could not be delivered */
#define DL_NOTSUPPORTED	0x12	/* Primitive is known but not supported */
#define DL_TOOMANY	0x13	/* Limit exceeded */
#define DL_NOTENAB	0x14	/* Promiscuous mode not enabled */
#define DL_BUSY		0x15	/* Other streams for a particular PPA in
				 * the post-attached state.
				 */
#define DL_NOAUTO	0x16	/* Automatic handling of XID and TEST response
				 * not supported.
				 */
#define DL_NOXIDAUTO	0x17	/* Automatic handling of XID not supported */
#define DL_NOTESTAUTO	0x18	/* Automatic handling of TEST not supported */
#define DL_XIDAUTO	0x19	/* Automatic handling of XID response */
#define DL_TESTAUTO	0x1a	/* Automatic handling of TEST response */
#define DL_PENDING	0x1b	/* pending outstanding connect indications */

/*
 * NOTE: The range of error codes from 0x80 - 0xff is reserved for
 *       implementation specific error codes.  This reserved range
 *       of error codes will be defined by the DLS Provider.
 */


/************************************************************************
*                         Types of Services                             *
************************************************************************/

/*
 * Media types supported
 */
#define DL_CSMACD	0x00	/* IEEE 802.3 CSMA/CD network */
#define DL_TPB		0x01	/* IEEE 802.4 Token Passing Bus */
#define DL_TPR		0x02	/* IEEE 802.5 Token Passing Ring */
#define DL_METRO	0x03	/* IEEE 802.6 Metro Net */
#define DL_ETHER	0x04	/* Ethernet Bus */
#define DL_HDLC		0x05	/* ISO HDLC protocol support, bit synchronous */
#define DL_CHAR		0x06	/* Character Synchronous (Bisync) */
#define DL_CTCA		0x07	/* IBM Channel-to-Channel Adapter */
#define DL_FDDI		0x08	/* Fiber Distributed Data Interface */
#define DL_OTHER	0x09	/* Any other medium not lested above */
#define DL_FC		0x10	/* Fibre Channel */
#define DL_ATM		0x11	/* ATM */
#define DL_IPATM	0x12	/* ATM Classical IP */
#define DL_X25		0x13	/* X.25 LAPB interface */
#define DL_ISDN		0x14	/* ISDN */
#define DL_HIPPI	0x15	/* HIPPI */
#define DL_100VG	0x16	/* 100 Base VG Ethernet */
#define DL_100VGTPR	0x17	/* 100 Base VG Token Ring */
#define DL_ETH_CSMA	0x18	/* ISO 8802/3 and Ethernet */
#define DL_100BT	0x19	/* 100 Base TX */
#define DL_FRAME	0x0a	/* Frame Relay LAPF */
#define DL_MPFRAME	0x0b	/* Multiprotocol on Frame Relay */
#define DL_ASYNC	0x0c	/* Character Asynchronous */
#define DL_IPX25	0x0d	/* X.25 Classical IP interface */
#define DL_LOOP		0x0e	/* software loopback */

/*
 * DLPI provider services supported
 *
 * These are bit-wise ORed for dl_service_mode in DL_INFO_ACK
 */
#define	DL_CODLS	0x01	/* connection oriented service supported */
#define	DL_CLDLS	0x02	/* connectionless service supported */
#define	DL_ACLDLS	0x04	/* acknowledged connectionless service */


/*
 * DLPI provider style
 *
 * The DLPI provider style which determines whether a provider
 * requires a DL_ATTACH_REQ to inform the provider which PPA
 * user messages should be set/receive on.
 */
#define	DL_STYLE1	0x0500	/* PPA is implicitly bound by open */
#define	DL_STYLE2	0x0501	/* PPA must be attached by DL_ATTACH_REQ */



/************************************************************************
*                     Disconnect and Reset Reasons                      *
************************************************************************/

/*
 * DLPI Originator for Disconnect and Resets
 */
#define	DL_PROVIDER	0x0700
#define	DL_USER		0x0701

/*
 * DLPI Disconnect Reasons
 */
#define DL_CONREJ_DEST_UNKNOWN			0x0800
#define DL_CONREJ_DEST_UNREACH_PERMANENT	0x0801
#define DL_CONREJ_DEST_UNREACH_TRANSIENT	0x0802
#define DL_CONREJ_QOS_UNAVAIL_PERMANENT		0x0803
#define DL_CONREJ_QOS_UNAVAIL_TRANSIENT		0x0804
#define DL_CONREJ_PERMANENT_COND		0x0805
#define DL_CONREJ_TRANSIENT_COND		0x0806
#define DL_DISC_ABNORMAL_CONDITION		0x0807
#define DL_DISC_NORMAL_CONDITION		0x0808
#define DL_DISC_PERMANENT_CONDITION		0x0809
#define DL_DISC_TRANSIENT_CONDITION		0x080a
#define DL_DISC_UNSPECIFIED			0x080b

/*
 * DLPI Reset Reasons
 */
#define DL_RESET_FLOW_CONTROL			0x0900
#define DL_RESET_LINK_ERROR			0x0901
#define DL_RESET_RESYNCH			0x0902


/************************************************************************
*                 Acknowledged Connectionless Service                   *
************************************************************************/

/*
 * DLPI status values for acknowledged connectionless data transfer
 */
#define DL_CMD_MASK	0x0f
#define DL_CMD_OK	0x00	/* Command accepted */
#define DL_CMD_RS	0x01	/* Unimplemented or inactivated service */
#define DL_CMD_UE	0x05	/* Data Link User interface error */
#define DL_CMD_PE	0x06	/* protocol error */
#define DL_CMD_IP	0x07	/* Permanent implementation dependent error */
#define DL_CMD_UN	0x09	/* Resources temporarily unavailable */
#define DL_CMD_IT	0x0f	/* Temporary implementation dependent error */

#define DL_RSP_MASK	0xf0
#define DL_RSP_OK	0x00	/* Response DLSDU present */
#define DL_RSP_RS	0x10	/* Unimplemented or inactivated service */
#define DL_RSP_NE	0x30	/* Response DLSDU never submitted */
#define DL_RSP_NR	0x40	/* Response DLSDU not requested */
#define DL_RSP_UE	0x50	/* Data Link User interface error */
#define DL_RSP_IP	0x70	/* Permanent implementation dependent error */
#define DL_RSP_UN	0x90	/* Resources temporarily unavailable */
#define DL_RSP_IT	0xf0	/* Temporary implementation dependent error */

/*
 * Service class values for acknowledged connectionless data transfer
 */
#define DL_RQST_RSP	0x01	/* Use acknowldege capability in MAC sublayer */
#define DL_RQST_NORSP	0x02	/* No acknowldegement services requested */



/************************************************************************
*                       Quality of Service                              *
************************************************************************/

/*
 * QOS Protection specification
 */
#define	DL_NONE			0x0b01	/* no protection supplied */
#define	DL_MONITOR		0x0b02	/* protection against passive
					 * monitoring
					 */
#define	DL_MAXIMUM		0x0b03	/* protection against modification,
					 * replay, addition or deletion.
					 */

/*
 * QOS parameter values
 */
#define DL_UNKNOWN		-1
#define DL_QOS_DONT_CARE	-2

/*
 * QOS type definitions
 */
#define DL_QOS_CO_RANGE1	0x0101	/* connection oriented service */
#define DL_QOS_CO_SEL1		0x0102	/* connection oriented service */
#define DL_QOS_CL_RANGE1	0x0103	/* connectionless service */
#define DL_QOS_CL_SEL1		0x0104	/* connectionless service */


/************************************************************************
*                          Miscellaneous                                *
************************************************************************/

/*
 * DLPI address type definition
 */
#define	DL_FACT_PHYS_ADDR	0x01	/* factory physical address */
#define	DL_CURR_PHYS_ADDR	0x02	/* current physical address */

/*
 * DLPI flag definitions
 */
#define	DL_POLL_FINAL		0x01	/* poll/final bit for TEST/XID */

/*
 * XID and TEST responses supported by the provider
 */
#define DL_AUTO_XID		0x01	/* provider will respond to XID */
#define DL_AUTO_TEST		0x02	/* provider will respond to TEST */

/*
 * Subsequent bind types
 */
#define	DL_PEER_BIND		0x01	/* subsequent bind on a peer addr */
#define	DL_HIERARCHICAL_BIND	0x02	/* subs-bind on a hierarchical addr */

/*
 * DLPI promiscuous mode definitions
 */
#define	DL_PROMISC_PHYS		0x01	/* promiscuous mode at physical level */
#define	DL_PROMISC_SAP		0x02	/* promiscuous mode at SAP level */
#define	DL_PROMISC_MULTI	0x03	/* promiscuous mode for multicast */





#endif /*!_DLPI_H*/
