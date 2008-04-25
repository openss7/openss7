/*****************************************************************************

 @(#) $Id: xti_apli.h,v 0.9.2.2 2008-04-25 08:38:31 brian Exp $

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

 Last Modified $Date: 2008-04-25 08:38:31 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xti_apli.h,v $
 Revision 0.9.2.2  2008-04-25 08:38:31  brian
 - working up libraries modules and drivers

 Revision 0.9.2.1  2007/12/15 20:16:48  brian
 - added new mib files

 *****************************************************************************/

#ifndef _SYS_XTI_APLI_H
#define _SYS_XTI_APLI_H

#ident "@(#) $RCSfile: xti_apli.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup xnet
  * @{ */

/** @file
  * XTI ACSE/Presentation header file. */

/*
 *  ACSE/Presentation Header File.
 */

#include <sys/xti_mosi.h>

#define T_OSI_APRI	0x0600
#define T_OSI_ROSE	0x0500
#define T_OSI_PRES	0x0400
#define T_OSI_SESS	0x0300

/* T_OSI_ARPI - ACSE/Presentation ROSE Options */
#define T_AP_OPT_AVAIL		 3 /**< A t_uscalar_t containing flags indicating which presentation
				        address components may be wildcarded on bind. */
#define T_AP_MODE_AVAIL		 4 /**< A t_uscalar_t containing flags indicating which provider modes
					are available. */
#define T_AP_MODE_SEL		 5 /**< A t_uscalar_t containing flags indicating which provider modes
					are selected. */
#   define T_AP_X410_MODE	    (1<<0) /**< X.410|1984 mode. */
#   define T_AP_NORMAL_MODE	    (1<<1) /**< Normal mode. */
#   define T_AP_ROSE_MODE	    (1<<2) /**< ROSE mode. */

#define T_AP_QLEN		 6 /**< A t_scalar_t containing the number of outstanding association
					requests that will be retained while no responder is
					available.  */
#define T_AP_COPYENV		 7 /**< A t_scalar_t containing T_YES or T_NO which indicates whether
					to provide option information with indication and confirmation
					primitives. */
#define T_AP_DIAGNOSTIC		 8 /**< A t_diag_t structure containing the reason, event and source
				        of an abort. */


/* T_ISO_ROSE - ROSE Options */
#define T_AP_RO_FAC_AVAIL	 1 /**< A t_uscalar_t containing bits that indicate the ROSE
					facilities available from the provider. */
#   define T_AP_RO_BIND		    (1<<0) /**< Bind and Unbind primitives supported. */

#define T_AP_RO_PCI_LIST	 2 /**< A t_scalar_t array containing the presentation context
					identifiers to examine for ROSE PDUs. */

/* T_ISO_APCO or T_ISO_APCL - ACSE Options */
/** T_AP_CNTX_NAME is an string of octets BER encoded without Tag or Length.
  * The default context name is per X.638
  * { iso(1) standard(0) curl(11188) mosi(3) default-application-context(3) }
  * */
#define T_AP_CNTX_NAME		 1  /* align with mOSI */
#	define T_AP_CNTX_NAME_DEFAULT	"\x28\xd7\x34\x03\x03"
#define T_AP_PCL		 2  /* align with (and detect) mOSI */
#define T_AP_ACSE_AVAIL		 3 /**< A t_uscalar_t containing flags indicating which ACSE protocol
					version are available. */
#define T_AP_ACSE_SEL		 4 /**< A t_uscalar_t containing flags indicating which ACSE protocol
					version is selected. */
#	define T_AP_ACSEVER1		(1<<0) /**< ACSE protocol version 1. */

#define T_AP_AFU_AVAIL		 5 /**< A t_uscalar_t containing flags indicating which ACSE
					Functional Units are available. */
#define T_AP_AFU_SEL		 6 /**< A t_uscalar_t containing flags indicating which ACSE
					Functional Units are selected. */
#define T_AP_CNTX_NAME		 7 /**< A octet string containing an OBJECT IDENTIFIER encoded in BER
					format without tag or length. */
#define T_AP_ROLE_ALLOWED	 8 /**< A t_uscalar_t containing flags indicating which roles are
					allowed. */
#define T_AP_ROLE_CURRENT	 9 /**< A t_uscalar_t containing flags indicating the current role. */

#define T_AP_CLG_APT		10 /**< An octet string encoding the calling AP Title. */

#define T_AP_CLG_AEQ		11 /**< An octet string encoding the calling AE Qualifier. */

#define T_AP_CLG_APID		12 /**< A t_scalar_t containing the calling AP invocation-identity. */

#define T_AP_CLG_AEID		13 /**< A t_scalar_t containing the calling AE invocation-identity. */

#define T_AP_CLD_APT		14 /**< An octet string encoding the called AP Title. */

#define T_AP_CLD_AEQ		15 /**< An octet string encoding the called AE Qualifier. */

#define T_AP_CLD_APID		16 /**< A t_scalar_t containing the called AP invocation-identifier.
					*/
#define T_AP_CLD_AEID		17 /**< A t_scalar_t containing the called AE invocation-identifier.
					*/
#define T_AP_RSP_APT		18 /**< An octet string encoding the responding AP Title. */

#define T_AP_RSP_AEQ		19 /**< An octet string encoding the responding AE Qualifier. */

#define T_AP_RSP_APID		20 /**< A t_scalar_t containing the responding AP
					invocation-identifier. */
#define T_AP_RSP_AEID		21 /**< A t_scalar_t containing the responding AE
					invocation-identifier. */
#define T_AP_STATE		22 /**< A t_scalar_t containing the current ACSE/Presentation state.
					*/


/* T_ISO_PRES - Presentation Options */
#define T_AP_PRES_AVAIL		 1 /**< A t_uscalar_t that contains the bits indicating which
					presentation protocol versions are available. */
#define T_AP_PRES_SEL		 2 /**< A t_uscalar_t that contains the bits selecting which
					presentation protocol versions to negotiate. */
#	define T_AP_PRESVER1		(1<<0)

#define T_AP_PFU_AVAIL		 3 /**< A t_uscalar_t that contains the bits indicating which
					Presentation Functional Unit are available. */
#define T_AP_PFU_SEL		 4 /**< A t_uscalar_t that contains the bits indicating which
					Presentation Functional Units are selected. */
#define T_AP_BIND_PADDR		 5 /**< A t_paddr_t structure that contains the presentation selector,
					transport selector and network service access points,
					representing the bound presentation address. */
#define T_AP_LCL_PADDR		 6 /**< A t_paddr_t structure that contains the presentation selector,
					transport selector and network service access point,
					representing the local entity. */
#define T_AP_REM_PADDR		 7 /**< A t_paddr_t structure that contains the presentation selector,
					trasnsport selector and network service access point,
					representing the remote entity. */
#define T_AP_DCS		 8 /**< A t_plc_t structure array that contains the defined
					presentation contexts. */
#define T_AP_DPCN		 9 /**< A t_plc_t structure array that contains the default
					presentation context. */
#	define T_AP_DPCN_ASYTX_DEFAULT	"\x28\xd7\x34\x03\x01\x01"
#	define T_AP_DPCN_TSYTX_DEFAULT	"\x28\xd7\x34\x03\x02\x01"
#define T_AP_DPCR		10
#define T_AP_PCDL		11 /**< A t_plc_t structure array that contains the default
					presentation context definition list. */
#define T_AP_PCDRL		12 /**< A t_plc_t structure array that contains the presentation
					context definition and result list. */


/* T_ISO_SESS - Session Options */
#define T_AP_SESS_AVAIL		 1 /**< A t_uscalar_t that contains bits indicating which session
					protocol versions are available. */
#define T_AP_SESS_SEL		 2 /**< A t_uscalar_t that selects from the available versions. */
#	define T_AP_SESSVER1		(1<<0)
#	define T_AP_SESSVER2		(1<<1)
#define T_AP_SESS_OPT_AVAIL	 3 /**< A t_uscalar_t containing flags indcating which session
					protocol options are available. */
#define T_AP_SESS_SFU_AVAIL	 4 /**< A t_uscalar_t containing flags indicating which Session
					Functional Units are available. */
#define T_AP_SESS_SFU_SEL	 5 /**< A t_uscalar_t containing flags indicating which Session
					Functional Units are selected. */
#define T_AP_QOS		 6 /**< A quality of service structure containing transport quality of
					service parameters. */
#define T_AP_CLG_CONN_ID	 7 /**< A structure containing three OBJECT IDENTIFIER octet strings,
					for the calling entity: one user reference, common reference
					and additional reference. */
#define T_AP_CLD_CONN_ID	 8 /**< A structure containing three OBJECT IDENTIFIER octet strings,
					for the called entity: one user reference, common reference
					and additional reference. */
#define T_AP_OLD_CONN_ID	 9 /**< A structure containing three OBJECT IDENTIFIER octet strings,
					for the old entity: one user reference, common reference and
					additional reference. */
#define T_AP_TOKENS_AVAIL	10 /**< A t_uscalar_t containing flags indicating which tokens are
					available to be acquired. */
#define T_AP_TOKENS_OWNED	11 /**< A t_uscalar_t containing flags indicating which tokena are
					owned by the user. */
#define T_AP_MSTATE		12 /**< A t_uscalar_t containing flags indicating which directions
					have more data to send or receive. */
#	define T_AP_SNDMORE		(1<<0)
#	define T_AP_RCVMORE		(1<<1)

/* T_AP_DIAGNOSTIC */
typedef struct {
	t_scalar_t rsn;
	t_scalar_t evt;
	t_scalar_t src;
	/* followed by error text string */
	char error[0];
} t_diag_t;

/* t_diag_t.src values */
#define T_AP_ACSE_SERV_PROV	(FIXME)
#define T_AP_PRES_SERV_PROV	(FIXME)
#define T_AP_SESS_SERV_PROV	(FIXME)
#define T_AP_TRAN_SERV_PROV	(FIXME)

/* t_diag_t.rsn values: */
/* t_diag_t.src == T_AP_ACSE_SERV_PROV */
#define T_AP_NSPEC		(FIXME)
#define T_AP_UNREC_APDU		(FIXME)
#define T_AP_UNEXPT_APDU	(FIXME)
#define T_AP_UNREC_APDU_PARM	(FIXME)
#define T_AP_UNEXPT_APDU_PARM	(FIXME)
#define T_AP_INVAL_APDU_PARM	(FIXME)
/* t_diag_t.src == T_AP_TRAN_SERV_PROV */
#define T_AP_TRAN_DISCONNECT	(FIXME)

/* t_diag_t.evt values: */
/* t_diag_t.src == T_AP_ACSE_SERV_PROV */
#define T_AP_AEI_AARQ		(FIXME)	/**< Associate request APDU. */
#define T_AP_AEI_AARE		(FIXME)	/**< Associate response APDU. */
#define T_AP_AEI_RLRQ		(FIXME)	/**< Associate release request APDU. */
#define T_AP_AEI_RLRE		(FIXME)	/**< Associate release response APDU. */
#define T_AP_AEI_ABRT		(FIXME)	/**< Associate abort APDU. */

/* If the src field is set to T_AP_TRAN_SERV_PROV and the rsn field is set to
 * T_AP_TRAN_DISCONNECT, the evt field will be set to an implementation-defined
 * diagnostic value such as that returned by the XTI t_rcvdis(3) call. */



/** @} */

#endif				/* _SYS_XTI_APLI_H */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
