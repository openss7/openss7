/*****************************************************************************

 @(#) $Id: npi_sccp.h,v 0.9.2.7 2008-04-29 07:10:46 brian Exp $

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

 Last Modified $Date: 2008-04-29 07:10:46 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: npi_sccp.h,v $
 Revision 0.9.2.7  2008-04-29 07:10:46  brian
 - updating headers for release

 Revision 0.9.2.6  2007/08/14 12:17:12  brian
 - GPLv3 header updates

 Revision 0.9.2.5  2007/02/13 14:05:30  brian
 - corrected ulong and long for 32-bit compat

 Revision 0.9.2.4  2007/02/13 07:55:42  brian
 - working up MTP and UAs

 *****************************************************************************/

#ifndef _SYS_NPI_SCCP_H
#define _SYS_NPI_SCCP_H

#ident "@(#) $RCSfile: npi_sccp.h,v $ $Name:  $($Revision: 0.9.2.7 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/*
 *  LiS npi.h is version 1
 */
#ifndef N_VERSION_2

#define N_CURRENT_VERSION   0x02	/* current version of NPI */
#define N_VERSION_2	    0x02	/* version of npi, December 16, 1991 */

typedef uchar np_uchar;

/*
 * Information acknowledgement
 */
typedef struct {
	np_ulong PRIM_type;		/* always N_INFO_ACK */
	np_ulong NSDU_size;		/* maximum NSDU size */
	np_ulong ENSDU_size;		/* maximum ENSDU size */
	np_ulong CDATA_size;		/* connect data size */
	np_ulong DDATA_size;		/* discon data size */
	np_ulong ADDR_size;		/* address size */
	np_ulong ADDR_length;		/* address length */
	np_ulong ADDR_offset;		/* address offset */
	np_ulong QOS_length;		/* QOS values length */
	np_ulong QOS_offset;		/* QOS values offset */
	np_ulong QOS_range_length;	/* length of QOS values' range */
	np_ulong QOS_range_offset;	/* offset of QOS values' range */
	np_ulong OPTIONS_flags;		/* bit masking for options supported */
	np_ulong NIDU_size;		/* network i/f data unit size */
	np_long SERV_type;		/* service type */
	np_ulong CURRENT_state;		/* current state */
	np_ulong PROVIDER_type;		/* type of NS provider */
	np_ulong NODU_size;		/* optimal NSDU size */
	np_ulong PROTOID_length;	/* length of bound protocol ids */
	np_ulong PROTOID_offset;	/* offset of bound protocol ids */
	np_ulong NPI_version;		/* version # of npi that is supported */
} __N_info_ack_t;

#define N_info_ack_t __N_info_ack_t

/*
 * Bind acknowledgement
 */
typedef struct {
	np_ulong PRIM_type;		/* always N_BIND_ACK */
	np_ulong ADDR_length;		/* address length */
	np_ulong ADDR_offset;		/* offset of address */
	np_ulong CONIND_number;		/* connection indications */
	np_ulong TOKEN_value;		/* value of "token" assigned to stream */
	np_ulong PROTOID_length;	/* length of bound protocol ids */
	np_ulong PROTOID_offset;	/* offset of bound protocol ids */
} __N_bind_ack_t;

#define N_bind_ack_t __N_bind_ack_t

#endif				/* N_VERSION_2 */

/*
 *  Extended NPI primitives for SCCP.
 */

#define N_NOTICE_IND	32
typedef struct {
	np_ulong PRIM_type;		/* always N_NOTICE_IND */
	np_ulong DEST_length;		/* address data was sent to */
	np_ulong DEST_offset;
	np_ulong SRC_length;		/* address data was sent from */
	np_ulong SRC_offset;
	np_ulong QOS_length;		/* QOS parameters data was sent with */
	np_ulong QOS_offset;
	np_ulong RETURN_cause;		/* reason for return of data */
} N_notice_ind_t;

#define N_INFORM_REQ	33
typedef struct {
	np_ulong PRIM_type;		/* always N_INFORM_REQ */
	np_ulong QOS_length;		/* qos parameters */
	np_ulong QOS_offset;
	np_ulong REASON;		/* inform reason */
} N_inform_req_t;

#define N_INFORM_IND	34
typedef struct {
	np_ulong PRIM_type;		/* always N_INFORM_IND */
	np_ulong QOS_length;		/* qos parameters */
	np_ulong QOS_offset;
	np_ulong REASON;		/* inform reason */
} N_inform_ind_t;

/* Inform reasons for use with N_INFORM_IND */
#define N_SCCP_INFR_NSP_FAILURE		1
#define N_SCCP_INFR_NSP_CONGESTION	2
#define N_SCCP_INFR_NSP_QOS_CHANGE	3
/* Inform reasons for use with N_INFORM_REQ */
#define N_SCCP_INFR_NSU_FAILURE		4
#define N_SCCP_INFR_NSU_CONGESTION	5
#define N_SCCP_INFR_NSU_QOS_CHANGE	6
/* Inform reasons for use with N_INFORM */
#define N_SCCP_INFR_UNSPECIFIED		7

#define N_COORD_REQ	35
typedef struct {
	np_ulong PRIM_type;		/* alwyas N_COORD_REQ */
	np_ulong ADDR_length;		/* affected subsystem */
	np_ulong ADDR_offset;
} N_coord_req_t;

#define N_COORD_RES	36
typedef struct {
	np_ulong PRIM_type;		/* always N_COORD_RES */
	np_ulong ADDR_length;		/* affected subsystem */
	np_ulong ADDR_offset;
} N_coord_res_t;

#define N_COORD_IND	37
typedef struct {
	np_ulong PRIM_type;		/* alwyas N_COORD_IND */
	np_ulong ADDR_length;		/* affected subsystem */
	np_ulong ADDR_offset;
	np_ulong SMI;			/* subsystem multiplicity indicator */
} N_coord_ind_t;

#define N_COORD_CON	38
typedef struct {
	np_ulong PRIM_type;		/* always N_COORD_CON */
	np_ulong ADDR_length;		/* affected subsystem */
	np_ulong ADDR_offset;
	np_ulong SMI;			/* subsystem multiplicity indicator */
} N_coord_con_t;

#define N_STATE_REQ	39
typedef struct {
	np_ulong PRIM_type;		/* always N_STATE_REQ */
	np_ulong ADDR_length;		/* affected subsystem */
	np_ulong ADDR_offset;
	np_ulong STATUS;		/* user status */
} N_state_req_t;

#define N_STATE_IND	40
typedef struct {
	np_ulong PRIM_type;		/* always N_STATE_IND */
	np_ulong ADDR_length;		/* affected subsystem */
	np_ulong ADDR_offset;
	np_ulong STATUS;		/* user status */
	np_ulong SMI;			/* subsystem multiplicity indicator */
} N_state_ind_t;

#define N_PCSTATE_IND	41
typedef struct {
	np_ulong PRIM_type;		/* always N_PCSTATE_IND */
	np_ulong ADDR_length;		/* affected point code */
	np_ulong ADDR_offset;
	np_ulong STATUS;		/* status */
} N_pcstate_ind_t;

#define N_TRAFFIC_IND	42
typedef struct {
	np_ulong PRIM_type;		/* always N_TRAFFIC_IND */
	np_ulong ADDR_length;		/* affected user */
	np_ulong ADDR_offset;
	np_ulong TRAFFIC_mix;		/* traffic mix */
} N_traffic_ind_t;

#define N_SCCP_STATUS_USER_IN_SERVICE			1
#define N_SCCP_STATUS_USER_OUT_OF_SERVICE		2
#define N_SCCP_STATUS_REMOTE_SCCP_AVAILABLE		3
#define N_SCCP_STATUS_REMOTE_SCCP_UNAVAILABLE		4
#define N_SCCP_STATUS_REMOTE_SCCP_UNEQUIPPED		5
#define N_SCCP_STATUS_REMOTE_SCCP_INACCESSIBLE		6
//#define N_SCCP_STATUS_REMOTE_SCCP_CONGESTED           
#define N_SCCP_STATUS_REMOTE_SCCP_CONGESTED(_x)		(7 + _x)
#define N_SCCP_STATUS_SIGNALLING_POINT_INACCESSIBLE	16
//#define N_SCCP_STATUS_SIGNALLING_POINT_CONGESTED      
#define N_SCCP_STATUS_SIGNALLING_POINT_CONGESTED(_x)	(17 + _x)
#define N_SCCP_STATUS_SIGNALLING_POINT_ACCESSIBLE	26

#define N_SCCP_TMIX_ALL_PREFFERED_NO_BACKUP	1
#define N_SCCP_TMIX_ALL_PREFERRED_SOME_BACKUP	2
#define N_SCCP_TMIX_ALL_PREFERRED_ALL_BACKUP	3
#define N_SCCP_TMIX_SOME_PREFERRED_NO_BACKUP	4
#define N_SCCP_TMIX_SOME_PREFERRED_SOME_BACKUP	5
#define N_SCCP_TMIX_NO_PREFERRED_NO_BACKUP	6
#define N_SCCP_TMIX_ALL				7
#define N_SCCP_TMIX_SOME			8
#define N_SCCP_TMIX_NONE			9

#ifndef NSF_UNBND
#define NSF_UNBND	(1<<NS_UNBND	  )
#define NSF_WACK_BREQ	(1<<NS_WACK_BREQ  )
#define NSF_WACK_UREQ	(1<<NS_WACK_UREQ  )
#define NSF_IDLE	(1<<NS_IDLE	  )
#define NSF_WACK_OPTREQ	(1<<NS_WACK_OPTREQ)
#define NSF_WACK_RRES	(1<<NS_WACK_RRES  )
#define NSF_WCON_CREQ	(1<<NS_WCON_CREQ  )
#define NSF_WRES_CIND	(1<<NS_WRES_CIND  )
#define NSF_WACK_CRES	(1<<NS_WACK_CRES  )
#define NSF_DATA_XFER	(1<<NS_DATA_XFER  )
#define NSF_WCON_RREQ	(1<<NS_WCON_RREQ  )
#define NSF_WRES_RIND	(1<<NS_WRES_RIND  )
#define NSF_WACK_DREQ6	(1<<NS_WACK_DREQ6 )
#define NSF_WACK_DREQ7	(1<<NS_WACK_DREQ7 )
#define NSF_WACK_DREQ9	(1<<NS_WACK_DREQ9 )
#define NSF_WACK_DREQ10	(1<<NS_WACK_DREQ10)
#define NSF_WACK_DREQ11	(1<<NS_WACK_DREQ11)
#endif				/* NSF_UNBND */

typedef struct sccp_addr {
	np_ulong ni;			/* network indicator */
	np_ulong ri;			/* routing indicator */
	np_ulong pc;			/* point code (-1 not present) */
	np_ulong ssn;			/* subsystem number (0 not present) */
	np_ulong gtt;			/* type of global title */
	np_ulong tt;			/* translation type */
	np_ulong es;			/* encoding scheme */
	np_ulong nplan;			/* numbering plan */
	np_ulong nai;			/* nature of address indicator */
	np_ulong alen;			/* address length */
	uchar addr[0];			/* address digits */
	/* followed by address bytes */
} sccp_addr_t;

#define SCCP_MAX_ADDR_LENGTH 32

#define SCCP_RI_DPC_SSN		0	/* route on DPC/SSN */
#define SCCP_RI_GT		1	/* route on GT */

#define SCCP_GTTTYPE_NONE	0	/* no GT */
#define SCCP_GTTTYPE_NAI	1	/* NAI only */
#define SCCP_GTTTYPE_TT		2	/* TT only */
#define SCCP_GTTTYPE_NP		3	/* TT, ES, NPLAN */
#define SCCP_GTTTYPE_NP_NAI	4	/* TT, ES, NPLAN and NAI */

#define N_QOS_SEL_DATA_SCCP	0x0901
typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_SEL_DATA_SCCP */
	np_ulong protocol_class;	/* protocol class */
	np_ulong option_flags;		/* options flags (return option) */
	np_ulong sequence_selection;	/* selected SLS value */
	np_ulong message_priority;	/* MTP message priority */
	np_ulong importance;		/* importance */
} N_qos_sel_data_sccp_t;

#define N_QOS_SEL_CONN_SCCP	0x0902
typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_SEL_CONN_SCCP */
	np_ulong protocol_class;	/* protocol class 2 or 3 */
	np_ulong option_flags;		/* options flags (return option) */
	np_ulong sequence_selection;	/* selected SLS value */
	np_ulong message_priority;	/* MTP message priority */
	np_ulong importance;		/* importance */
} N_qos_sel_conn_sccp_t;

#define N_QOS_SEL_INFO_SCCP	0x0903
typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_SEL_INFO_SCCP */
	np_ulong protocol_class;	/* protocol class */
	np_ulong option_flags;		/* options flags (return option) */
	np_ulong sequence_selection;	/* selected SLS value */
	np_ulong message_priority;	/* MTP message priority */
	np_ulong importance;		/* importance */
} N_qos_sel_info_sccp_t;

#define N_QOS_RANGE_INFO_SCCP	0x0904
typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_RANGE_INFO_SCCP */
	np_ulong protocol_classes;	/* protocol class */
	np_ulong sequence_selection;	/* SLS range */
} N_qos_range_info_sccp_t;

#define N_QOS_SEL_INFR_SCCP	0x0905
typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_SEL_INFR_SCCP */
	np_ulong protocol_class;	/* protocol class */
	np_ulong option_flags;		/* options flags (return option) */
	np_ulong sequence_selection;	/* selected SLS value */
	np_ulong message_priority;	/* MTP message priority */
	np_ulong importance;		/* importance */
} N_qos_sel_infr_sccp_t;

typedef union {
	np_ulong n_qos_type;
	N_qos_sel_data_sccp_t sel_data;
	N_qos_sel_conn_sccp_t sel_conn;
	N_qos_sel_info_sccp_t sel_info;
	N_qos_range_info_sccp_t range_info;
	N_qos_sel_infr_sccp_t sel_infr;
} N_qos_sccp_t;

/*
 *  Protocol Class bit masks with N_QOS_RANGE_SCCP protocol_classes
 */
#define N_QOS_PCLASS_0		0x01
#define N_QOS_PLCASS_1		0x02
#define N_QOS_PCLASS_2		0x04
#define N_QOS_PCLASS_3		0x08
#define N_QOS_PCLASS_ALL	0x0f

/*
 *  SCCP Causes
 */

/*
 *  SCCP Release Causes used in N_DISCON_IND
 */
#define SCCP_RELC_END_USER_ORIGINATED				0x1000
#define SCCP_RELC_END_USER_BUSY					0x1001
#define SCCP_RELC_END_USER_FAILURE				0x1002
#define SCCP_RELC_SCCP_USER_ORIGINATED				0x1003
#define SCCP_RELC_REMOTE_PROCEDURE_ERROR			0x1004
#define SCCP_RELC_INCONSISTENT_CONNECTION_DATA			0x1005
#define SCCP_RELC_ACCESS_FAILURE				0x1006
#define SCCP_RELC_ACCESS_CONGESTION				0x1007
#define SCCP_RELC_SUBSYSTEM_FAILURE				0x1008
#define SCCP_RELC_SUBSYSTEM_CONGESTION				0x1009
#define SCCP_RELC_MTP_FAILURE					0x100a
#define SCCP_RELC_NETWORK_CONGESTION				0x100b
#define SCCP_RELC_EXPIRATION_OF_RESET_TIMER			0x100c
#define SCCP_RELC_EXPIRATION_OF_RECEIVE_INACTIVITY_TIMER	0x100d
#define SCCP_RELC_RESERVED					0x100e
#define SCCP_RELC_UNQUALIFIED					0x100f
#define SCCP_RELC_SCCP_FAILURE					0x1010

/*
 *  SCCP Return Causes used in N_UDERROR_IND
 */
#define SCCP_RETC_NO_ADDRESS_TYPE_TRANSLATION			0x2000
#define	SCCP_RETC_NO_ADDRESS_TRANSLATION			0x2001
#define SCCP_RETC_SUBSYSTEM_CONGESTION				0x2002
#define SCCP_RETC_SUBSYSTEM_FAILURE				0x2003
#define SCCP_RETC_UNEQUIPPED_USER				0x2004
#define SCCP_RETC_MTP_FAILURE					0x2005
#define SCCP_RETC_NETWORK_CONGESTION				0x2006
#define SCCP_RETC_UNQUALIFIED					0x2007
#define SCCP_RETC_MESSAGE_TRANSPORT_ERROR			0x2008
#define SCCP_RETC_LOCAL_PROCESSING_ERROR			0x2009
#define SCCP_RETC_NO_REASSEMBLY_AT_DESTINATION			0x200a
#define SCCP_RETC_SCCP_FAILURE					0x200b
#define SCCP_RETC_SCCP_HOP_COUNTER_VIOLATION			0x200c
#define SCCP_RETC_SEGMENTATION_NOT_SUPPORTED			0x200d
#define SCCP_RETC_SEGMENTATION_FAILURE				0x200e

#define SCCP_RETC_MESSAGE_CHANGE_FAILURE			0x20f7
#define SCCP_RETC_INVALID_INS_ROUTING_REQUEST			0x20f8
#define SCCP_RETC_INVALID_INSI_ROUTING_REQUEST			0x20f9
#define SCCP_RETC_UNAUTHORIZED_MESSAGE				0x20fa
#define SCCP_RETC_MESSAGE_INCOMPATIBILITY			0x20fb
#define SCCP_RETC_CANNOT_PERFORM_ISNI_CONSTRAINED_ROUTING	0x20fc
#define SCCP_RETC_REDUNDANT_ISNI_CONSTRAINED_ROUTING_INFO	0x20fd
#define SCCP_RETC_UNABLE_TO_PERFORM_ISNI_IDENTIFICATION		0x20fe

/*
 *  SCCP Reset Causes used in N_RESET_IND/REQ
 */
#define SCCP_RESC_END_USER_ORIGINATED				0x3000
#define SCCP_RESC_SCCP_USER_ORIGINATED				0x3001
#define SCCP_RESC_MESSAGE_OUT_OF_ORDER_INCORRECT_PS		0x3002
#define SCCP_RESC_MESSAGE_OUT_OF_ORDER_INCORRECT_PR		0x3003
#define SCCP_RESC_REMOTE_PROC_ERROR_MESSAGE_OUT_OF_WINDOW	0x3004
#define SCCP_RESC_REMOTE_PROC_ERROR_INCORRECT_PS_AFTER_INIT	0x3005
#define SCCP_RESC_REMOTE_PROC_ERROR_GENERAL			0x3006
#define SCCP_RESC_REMOTE_END_USER_OPERATIONAL			0x3007
#define SCCP_RESC_NETWORK_OPERATIONAL				0x3008
#define SCCP_RESC_ACCESS_OPERATIONAL				0x3009
#define SCCP_RESC_NETWORK_CONGESTION				0x300a
#define SCCP_RESC_NOT_OBTAINABLE				0x300b
#define SCCP_RESC_UNQUALIFIED					0x300c

/*
 *  SCCP Error Causes
 */
#define SCCP_ERRC_LRN_MISMATCH_UNASSIGNED_DEST_LRN		0x4000
#define SCCP_ERRC_LRN_MISMATCH_INCONSISTENT_SOURCE_LRN		0x4001
#define SCCP_ERRC_POINT_CODE_MISMATCH				0x4002
#define SCCP_ERRC_SERVICE_CLASS_MISMATCH			0x4003
#define SCCP_ERRC_UNQUALIFIED					0x4004

/*
 *  SCCP Refusal Causes used in N_DISCON_IND
 */
#define SCCP_REFC_END_USER_ORIGINATED				0x5000
#define SCCP_REFC_END_USER_CONGESTION				0x5001
#define SCCP_REFC_END_USER_FAILURE				0x5002
#define SCCP_REFC_SCCP_USER_ORIGINATED				0x5003
#define SCCP_REFC_DESTINATION_ADDRESS_UNKNOWN			0x5004
#define SCCP_REFC_DESTINATION_INACCESSIBLE			0x5005
#define SCCP_REFC_NETWORK_RESOURCE_QOS_NOT_AVAIL_PERM		0x5006
#define SCCP_REFC_NETWORK_RESOURCE_QOS_NOT_AVAIL_TRANS		0x5007
#define SCCP_REFC_ACCESS_FAILURE				0x5008
#define SCCP_REFC_ACCESS_CONGESTION				0x5009
#define SCCP_REFC_SUBSYSTEM_FAILURE				0x500a
#define SCCP_REFC_SUBSYSTEM_CONGESTION				0x500b
#define SCCP_REFC_EXPIRATION_OF_NC_ESTAB_TIMER			0x500c
#define SCCP_REFC_INCOMPATIBLE_USER_DATA			0x500d
#define SCCP_REFC_RESERVED					0x500e
#define SCCP_REFC_UNQUALIFIED					0x500f
#define SCCP_REFC_SCCP_HOP_COUNTER_VIOLATION			0x5010
#define SCCP_REFC_SCCP_FAILURE					0x5011
#define SCCP_REFC_NO_ADDRESS_TYPE_TRANSLATION			0x5012
#define SCCP_REFC_UNEQUIPPED_USER				0x5013

#endif				/* _SYS_NPI_SCCP_H */
