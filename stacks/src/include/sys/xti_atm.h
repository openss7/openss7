/*****************************************************************************

 @(#) $Id: xti_atm.h,v 0.9.2.6 2007/08/14 12:17:13 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2007/08/14 12:17:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xti_atm.h,v $
 Revision 0.9.2.6  2007/08/14 12:17:13  brian
 - GPLv3 header updates

 Revision 0.9.2.5  2006/09/25 12:10:09  brian
 - updated and doxygenified headers

 Revision 0.9.2.4  2006/09/18 13:52:37  brian
 - added doxygen markers to sources

 Revision 0.9.2.3  2005/05/14 08:30:49  brian
 - copyright header correction

 Revision 0.9.2.2  2004/09/02 09:31:14  brian
 - Synchronization with other packages.

 Revision 0.9.2.1  2004/05/12 08:01:39  brian
 - Added in xti library and STREAMS modules.

 Revision 1.1.2.2  2004/04/13 13:05:20  brian
 - Minor adjustments to ATM exposed definitions.

 Revision 1.1.2.1  2004/04/13 12:12:52  brian
 - Rearranged header files.

 *****************************************************************************/

#ifndef _SYS_XTI_ATM_H
#define _SYS_XTI_ATM_H

#ident "@(#) $RCSfile: xti_atm.h,v $ $Name:  $($Revision: 0.9.2.6 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup xnet
  * @{ */

/** @file
  * XTI ATM Transport Specific header file.
  *
  * For the purposes of conformance testing, it may be assumed that any constant values defined in
  * these header files are mandatory, unless the constant:
  *
  * @li defines an option or options level,
  * @li is accompanied by a comment that specifies the value is not mandatory.  */

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

/* 
 * XTI ATM Transport Specific Header File.
 *
 * For the purposes of conformance testing, it may be assumed that any constant
 * values defined in these header files are mandatory, unless the constant:
 *
 *   1. defines an option or options level,
 *   2. is accompanied by a comment that specifies the value is not mandatory.
 */

#ifndef t_leaf_status
/**
  * Leaf status structure.
  */
struct t_leaf_status {
	int leafid;		/**< Leaf identifier. */
	int status;		/**< Current status. */
	int reason;		/**< Reason for leaf removal. */
};

#define t_leaf_status t_leaf_status

/** @name Leaf Status
  * @{ */
#define T_LEAF_NOCHANGE		0	/**< No change in leaf status. */
#define T_LEAF_CONNECTED	1	/**< Leaf has connected. */
#define T_LEAF_DISCONNECTED	2	/**< Leaf has disconnected. */
/** @} */
#endif				/* t_leaf_status */

/**
  * @name ATM Constants
  * ATM commonly used constants.
  * In the first 3 constants, #T_ATM_ABSENT, #T_ATM_PRESENT and #T_ATM_ANY, the
  * specific value is not mandatory, but any conforming value must be negative.
  * @{ */
#define T_ATM_ABSENT		(-1)
#define T_ATM_PRESENT		(-2)
#define T_ATM_ANY		(-3)

#define T_ATM_NULL		0
#define T_ATM_ENDSYS_ADDR	1	/* value is not mandatory */
#define T_ATM_NSAP_ADDR		2	/* value is not mandatory */
#define T_ATM_E164_ADDR		3	/* value is not mandatory */

#define T_ATM_ITU_CODING	0
#define T_ATM_NETWORK_CODING	3
/** @} */

/**
  * @name ATM-Specific Addresses
  * @{
  */

/**
  * ATM protocol address structure
  */
struct t_atm_sap {
	struct t_atm_sap_addr {
		int8_t SVE_tag_addr;
		int8_t SVE_tag_selector;
		uint8_t address_format;
		uint8_t address_length;
		uint8_t address[20];
	} t_atm_sap_addr;
	struct t_atm_sap_layer2 {
		int8_t SVE_tag;
		uint8_t ID_type;
		union {
			uint8_t simple_ID;
			uint8_t user_defined_ID;
		} ID;
	} t_atm_sap_layer2;
	struct t_atm_sap_layer3 {
		int8_t SVE_tag;
		uint8_t ID_type;
		union {
			uint8_t simple_ID;
			int32_t IPI_ID;
			struct {
				uint8_t OUI[3];
				uint8_t PID[2];
			} SNAP_ID;
			uint8_t user_defined_ID;
		} ID;
	} t_atm_sap_layer3;
	struct t_atm_sap_appl {
		int8_t SVE_tag;
		uint8_t ID_type;
		union {
			uint8_t ISO_ID[8];
			struct {
				uint8_t OUI[3];
				uint8_t app_ID[4];
			} vendor_ID;
			uint8_t user_defined_ID[8];
		} ID;
	} t_atm_sap_appl;
};

/** @} */

/*
   ATM-SPECIFIC OPTIONS 
 */

/**
  * @name ATM Signalling-Level Options
  * @{ */
#define T_ATM_AAL5		0x1	/* ATM adaptation layer 5. */
#define T_ATM_TRAFFIC		0x2	/* Data traffic descriptor. */
#define T_ATM_BEARER_CAP	0x3	/* ATM service capabilities. */
#define T_ATM_BHLI		0x4	/* Higher-layer protocol. */
#define T_ATM_BLLI		0x5	/* Lower-layer protocol. */
#define T_ATM_DEST_ADDR		0x6	/* Call responder's address. */
#define T_ATM_DEST_SUB		0x7	/* Call responder's subaddress. */
#define T_ATM_ORIG_ADDR		0x8	/* Call initiator's address. */
#define T_ATM_ORIG_SUB		0x9	/* Call initiator's subaddress. */
#define T_ATM_CALLER_ID		0xa	/* Caller's ID attributes. */
#define T_ATM_CAUSE		0xb	/* Cause of disconnection. */
#define T_ATM_QOS		0xc	/* Desired quality of service. */
#define T_ATM_TRANSIT		0xd	/* Choice of public carrier. */
/** @} */

/**
  * T_ATM_AAL5 structure.
  */
struct t_atm_aal5 {
	int32_t forward_max_SDU_size;
	int32_t backward_max_SDU_size;
	int32_t SSCS_type;
};

/**
  * @name T_ATM_AAL5 Values
  * @{ */
#define T_ATM_SSCS_SSCOP_REL	1
#define T_ATM_SSCS_SSCOP_UNREL	2
#define T_ATM_SSCS_FR		4
/** @} */

/**
  * T_ATM_TRAFFIC structure.
  */
struct t_atm_traffic_substruct {
	int32_t PCR_high_priority;
	int32_t PCR_all_traffic;
	int32_t SCR_high_priority;
	int32_t SCR_all_traffic;
	int32_t MBS_high_priority;
	int32_t MBS_all_traffic;
	int32_t tagging;
};
struct t_atm_traffic {
	struct t_atm_traffic_substruct forward;
	struct t_atm_traffic_substruct backward;
	uint8_t best_effort;
};

/**
  * T_ATM_BEARER_CAP structure.
  */
struct t_atm_bearer {
	uint8_t bearer_class;
	uint8_t traffic_type;
	uint8_t timing_requirements;
	uint8_t clipping_susceptibility;
	uint8_t connection_configuration;
};

/**
  * @name T_ATM_BEARER_CAP Values
  * @{
  */
#define T_ATM_CLASS_A		0x01	/**< Bearer class A. */
#define T_ATM_CLASS_C		0x03	/**< Bearer class C. */
#define T_ATM_CLASS_X		0x10	/**< Bearer class X. */

#define T_ATM_CBR		0x01	/**< Constant bit rate. */
#define T_ATM_VBR		0x02	/**< Variable bit rate. */

#define T_ATM_END_TO_END	0x01	/**< End-to-end timing required. */
#define T_ATM_NO_END_TO_END	0x02	/**< End-to-end timing not required. */

#define T_ATM_1_TO_1		0x00	/**< Point-to-point connection. */
#define T_ATM_1_TO_MANY		0x01	/**< Point-to-multipoint connection. */
/** @} */

/**
  * T_ATM_BHLI structure.
  */
struct t_atm_bhli {
	int32_t ID_type;
	union {
		uint8_t ISO_ID[8];
		struct {
			uint8_t OUI[3];
			uint8_t app_ID[4];
		} vendor_ID;
		uint8_t user_defined_ID[8];
	} ID;
};

/**
  * @name T_ATM_BHLI Values
  * @{ */
#define T_ATM_ISO_APP_ID	0	/**< ISO codepoint. */
#define T_ATM_VENDOR_APP_ID	3	/**< Vendor-specific codepoint. */
#define T_ATM_USER_APP_ID	1	/**< User-specific codepoint. */
/** @} */

/**
  * T_ATM_BLLI structure.
  */
struct t_atm_blli {
	struct {
		int8_t ID_type;
		union {
			uint8_t simple_ID;
			uint8_t user_defined_ID;
		} ID;
		int8_t mode;
		int8_t window_size;
	} layer_2_protocol;
	struct {
		int8_t ID_type;
		union {
			uint8_t simple_ID;
			int32_t IPI_ID;
			struct {
				uint8_t OUI[3];
				uint8_t PID[2];
			} SNAP_ID;
			uint8_t user_defined_ID;
		} ID;
		int8_t mode;
		int8_t packet_size;
		int8_t window_size;
	} layer_3_protocol;
};

/**
  * @name T_ATM_BLLI Values
  * Constant values in 4 definitions are not mandatory: #T_ATM_SIMPLE_ID,
  * #T_ATM_IPI_ID, #T_ATM_SNAP_ID and #T_ATM_USER_ID.
  * @{ */
#define T_ATM_SIMPLE_ID		1	/**< ID via ITU encoding. */
#define T_ATM_IPI_ID		2	/**< ID via ISO/IEC TR 9577. */
#define T_ATM_SNAP_ID		3	/**< ID via SNAP. */
#define T_ATM_USER_ID		4	/**< ID via user codepoints. */

#define T_ATM_BLLI_NORMAL_MODE		 1
#define T_ATM_BLLI_EXTENDED_MODE	 2

#define T_ATM_BLLI2_I1745		 1	/**< I.1745 */
#define T_ATM_BLLI2_Q921		 2	/**< Q.921 */
#define T_ATM_BLLI2_X25_LINK		 6	/**< X.25, link layer. */
#define T_ATM_BLLI2_X25_MLINK		 7	/**< X.25, multilink. */
#define T_ATM_BLLI2_LAPB		 8	/**< Extended LAPB */
#define T_ATM_BLLI2_HDLC_ARM		 9	/**< I.4335, ARM */
#define T_ATM_BLLI2_HDLC_NRM		10	/**< I.4335, NRM */
#define T_ATM_BLLI2_HDLC_ABM		11	/**< I.4335, ABM */
#define T_ATM_BLLI2_I8802		12	/**< I.8802 */
#define T_ATM_BLLI2_X75			13	/**< X.75 */
#define T_ATM_BLLI2_Q922		14	/**< Q.922 */
#define T_ATM_BLLI2_I7776		17	/**< I.7776 */

#define T_ATM_BLLI3_X25			 6	/**< X.25 */
#define T_ATM_BLLI3_I8208		 7	/**< I.8208 */
#define T_ATM_BLLI3_X223		 8	/**< X.223 */
#define T_ATM_BLLI3_I8473		 9	/**< I.8473 */
#define T_ATM_BLLI3_T70			10	/**< T.70 */
#define T_ATM_BLLI3_I9577		11	/**< I.9577 */

#define T_ATM_PACKET_SIZE_16		 4
#define T_ATM_PACKET_SIZE_32		 5
#define T_ATM_PACKET_SIZE_64		 6
#define T_ATM_PACKET_SIZE_128		 7
#define T_ATM_PACKET_SIZE_256		 8
#define T_ATM_PACKET_SIZE_512		 9
#define T_ATM_PACKET_SIZE_1024		10
#define T_ATM_PACKET_SIZE_2048		11
#define T_ATM_PACKET_SIZE_4096		12
/** @} */

/**
  * ATM network address structure.
  */
struct t_atm_addr {
	int8_t address_format;
	uint8_t address_length;
	uint8_t address[20];
};

/**
  * T_ATM_CALLER_ID structure.
  */
struct t_atm_caller_id {
	int8_t presentation;
	uint8_t screening;
};

/**
  * @name T_ATM_CALLER_ID Values
  * @{ */
#define T_ATM_PRES_ALLOWED		0
#define T_ATM_PRES_RESTRICTED		1
#define T_ATM_PRES_UNAVAILABLE		2

#define T_ATM_USER_ID_NOT_SCREENED	0
#define T_ATM_USER_ID_PASSED_SCREEN	1
#define T_ATM_USER_ID_FAILED_SCREEN	2

#define T_ATM_NETWORK_PROVIDED_ID	3
/** @} */

/**
  * T_ATM_CAUSE structure.
  */
struct t_atm_cause {
	int8_t coding_standard;
	uint8_t location;
	uint8_t cause_value;
	uint8_t diagnostics[4];
};

/**
  * @name T_ATM_CAUSE Location Values
  * @{ */
#define T_ATM_LOC_USER			 0
#define T_ATM_LOC_LOCAL_PRIVATE_NET	 1
#define T_ATM_LOC_LOCAL_PUBLIC_NET	 2
#define T_ATM_LOC_TRANSIT_NET		 3
#define T_ATM_LOC_REMOTE_PUBLIC_NET	 4
#define T_ATM_LOC_REMOTE_PRIVATE_NET	 5
#define T_ATM_LOC_INTERNATIONAL_NET	 7
#define T_ATM_LOC_BEYOND_INTERWORKING	10
/** @} */

/**
  * @name T_ATM_CAUSE Cause Values
  * @{ */
#define T_ATM_CAUSE_UNALLOCATED_NUMBER				  1
#define T_ATM_CAUSE_NO_ROUTE_TO_TRANSIT_NETWORK			  2
#define T_ATM_CAUSE_NO_ROUTE_TO_DESTINATION			  3
#define T_ATM_CAUSE_NORMAL_CALL_CLEARING			 16
#define T_ATM_CAUSE_USER_BUSY					 17
#define T_ATM_CAUSE_NO_USER_RESPONDING				 18
#define T_ATM_CAUSE_CALL_REJECTED				 21
#define T_ATM_CAUSE_NUMBER_CHANGED				 22
#define T_ATM_CAUSE_ALL_CALLS_WITHOUT_CALLER_ID_REJECTED	 23
#define T_ATM_CAUSE_DESTINATION_OUT_OF_ORDER			 27
#define T_ATM_CAUSE_INVALID_NUMBER_FORMAT			 28
#define T_ATM_CAUSE_RESPONSE_TO_STATUS_ENQUIRY			 30
#define T_ATM_CAUSE_UNSPECIFIED_NORMAL				 31
#define T_ATM_CAUSE_REQUESTED_VPCI_VCI_NOT_AVAILABLE		 35
#define T_ATM_CAUSE_VPCI_VCI_ASSIGNMENT_FAILURE			 36
#define T_ATM_CAUSE_USER_CELL_RATE_NOT_AVAILABLE		 37
#define T_ATM_CAUSE_NETWORK_OUT_OF_ORDER			 38
#define T_ATM_CAUSE_TEMPORARY_FAILURE				 41
#define T_ATM_CAUSE_ACCESS_INFO_DISCARDED			 43
#define T_ATM_CAUSE_NO_VPCI_VCI_AVAILABLE			 45
#define T_ATM_CAUSE_UNSPECIFIED_RESOURCE_UNAVAILABLE		 47
#define T_ATM_CAUSE_QUALITY_OF_SERVICE_UNAVAILABLE		 49
#define T_ATM_CAUSE_BEARER_CAPABILITY_NOT_AUTHORIZED		 57
#define T_ATM_CAUSE_BEARER_CAPABILITY_UNAVAILABLE		 58
#define T_ATM_CAUSE_SERVICE_OR_OPTION_UNAVAILABLE		 63
#define T_ATM_CAUSE_BEARER_CAPABILITY_NOT_IMPLEMENTED		 65
#define T_ATM_CAUSE_INVALID_TRAFFIC_PARAMETERS			 73
#define T_ATM_CAUSE_AAL_PARAMETERS_NOT_SUPPORTED		 78
#define T_ATM_CAUSE_INVALID_CALL_REFERENCE_VALUE		 81
#define T_ATM_CAUSE_IDENTIFIED_CHANNEL_DOES_NOT_EXIST		 82
#define T_ATM_CAUSE_INCOMPATIBLE_DESTINATION			 88
#define T_ATM_CAUSE_INVALID_ENDPOINT_REFERENCE			 89
#define T_ATM_CAUSE_INVALID_TRANSIT_NETWORK_SELECTION		 91
#define T_ATM_CAUSE_TOO_MANY_PENDING_ADD_PARTY_REQUESTS		 92
#define T_ATM_CAUSE_MANDITORY_INFO_ELEMENT_MISSING		 96
#define T_ATM_CAUSE_MESSAGE_TYPE_NOT_IMPLEMENTED		 97
#define T_ATM_CAUSE_INFO_ELEMENT_NOT_IMPLEMENTED		 99
#define T_ATM_CAUSE_INVALID_INFO_ELEMENT_CONTENTS		100
#define T_ATM_CAUSE_MESSAGE_INCOMPATIBLE_WITH_CALL_STATE	101
#define T_ATM_CAUSE_RECOVERY ON_TIMER_EXPIRY			102
#define T_ATM_CAUSE_INCORRECT_MESSAGE_LENGTH			104
#define T_ATM_CAUSE_UNSPECIFIED_PROTOCOL_ERROR			111
/** @} */

/**
  * T_ATM_QOS structure.
  */
struct t_atm_qos_substruct {
	int32_t coding_standard;
};
struct t_atm_qos {
	int8_t coding_standard;
	struct t_atm_qos_substruct forward;
	struct t_atm_qos_substruct backward;
};

/**
  * @name T_ATM_QOS Values
  * @{ */
#define T_ATM_QOS_CLASS_0	0
#define T_ATM_QOS_CLASS_1	1
#define T_ATM_QOS_CLASS_2	2
#define T_ATM_QOS_CLASS_3	3
#define T_ATM_QOS_CLASS_4	4
/** @} */

/**
  * T_ATM_TRANSIT structure.
  */
struct t_atm_transit {
	uint8_t length;
	uint8_t network_id[0];		/**< Variable-sized array. */
};

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* _SYS_XTI_ATM_H */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
