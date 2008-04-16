/*****************************************************************************

 @(#) $Id: xap_rose.h,v 0.9.2.1 2007/09/29 14:08:34 brian Exp $

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

 Last Modified $Date: 2007/09/29 14:08:34 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xap_rose.h,v $
 Revision 0.9.2.1  2007/09/29 14:08:34  brian
 - added new files

 *****************************************************************************/

#ifndef __XAP_ROSE_H__
#define __XAP_ROSE_H__

#ident "@(#) $RCSfile: xap_rose.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * xap_rose.h
 *
 * Contains structures and constants needed to interface
 * with the ROSE protocol machine through XAP.
 *
 */
#ifndef AP_ROSE_ID
#define AP_ROSE_ID		(13)

#ifndef AP_ID
#include <xap.h>
#endif				/* AP_ID */
/*
 * XAP-ROSE flag for AP_MODE_SEL
 */
#define AP_ROSE_MODE		0x04
/*
 * Possible values for cdata->type argument
 */
#define AP_RO_LOCAL		1
#define AP_RO_GLOBAL		2
#define AP_RO_NO_RESULT		3
/* added by U012F */
#define AP_RO_DIRECT_REF	4
#define AP_RO_INDIRECT_REF	5
#define AP_RO_DIR_AND_INDIR	6
/*
 * Flag for returned parameter information
 */
#define AP_RO_RETURN_PARM	1
/*
 * Reject code type values
 */
#define AP_RO_GENERAL_TYPE	0
#define AP_RO_INVOKE_TYPE	1
#define AP_RO_RESULT_TYPE	2
#define AP_RO_ERROR_TYPE	3
/*
 * Possible General problem types
 */
#define AP_RO_GEN_UNREC_PDU		0
#define AP_RO_GEN_MISTYPED_PDU		1
#define AP_RO_GEN_BAD_STRUCT_PDU	2
/*
 * Possible Invoke Problem types
 */
#define AP_RO_IN_DUP_INVOCATION			0
#define AP_RO_IN_UNREC_OPERATION		1
#define AP_RO_IN_MISTYPED_ARG			2
#define AP_RO_IN_RESOURCE_LIMIT			3
#define AP_RO_IN_RELEASE_IN_PROG		4
#define AP_RO_IN_UNREC_LINKED_ID		5
#define AP_RO_IN_LINKED_RESP_UNEXPECTED		6
#define AP_RO_IN_UNEXPECTED_LINKED_OPER		7
/*
 * Possible Result problem types
 */
#define AP_RO_RES_UNREC_INVOCATION		0
#define AP_RO_RES_RESULT_RESP_UNEXPECTED	1
#define AP_RO_RES_MISTYPED_RESULT		2
/*
 * Possible Error problem types
 */
#define AP_RO_ER_UNREC_INVOCATION	0
#define AP_RO_ER_ERROR_RESP_UNEXPECTED	1
#define AP_RO_ER_UNREC_ERROR		2
#define AP_RO_ER_UNEXPECTED_ERROR	3
#define AP_RO_ER_MISTYPED_PARAM		4
/*
 * Primitive types
 */
#define AP_RO_INVOKE_IND	((AP_ROSE_ID<<16) | 0x01)
#define AP_RO_INVOKE_REQ	((AP_ROSE_ID<<16) | 0x02)
#define AP_RO_RESULT_IND	((AP_ROSE_ID<<16) | 0x03)
#define AP_RO_RESULT_REQ	((AP_ROSE_ID<<16) | 0x04)
#define AP_RO_ERROR_IND		((AP_ROSE_ID<<16) | 0x05)
#define AP_RO_ERROR_REQ		((AP_ROSE_ID<<16) | 0x06)
#define AP_RO_REJECTU_IND	((AP_ROSE_ID<<16) | 0x07)
#define AP_RO_REJECTU_REQ	((AP_ROSE_ID<<16) | 0x08)
#define AP_RO_REJECTP_IND	((AP_ROSE_ID<<16) | 0x09)
#define AP_RO_BIND_REQ		((AP_ROSE_ID<<16) | 0x0A)
#define AP_RO_BIND_IND		((AP_ROSE_ID<<16) | 0x0B)
#define AP_RO_BIND_RSP		((AP_ROSE_ID<<16) | 0x0C)
#define AP_RO_BIND_CNF		((AP_ROSE_ID<<16) | 0x0D)
#define AP_RO_UNBIND_REQ	((AP_ROSE_ID<<16) | 0x0E)
#define AP_RO_UNBIND_IND	((AP_ROSE_ID<<16) | 0x0F)
#define AP_RO_UNBIND_RSP	((AP_ROSE_ID<<16) | 0x10)
#define AP_RO_UNBIND_CNF	((AP_ROSE_ID<<16) | 0x11)
/*
 * The following are provider primitive types
 * included here for value allocation purposes
 */
#define AP_RO_INFO_REQ		((AP_ROSE_ID<<16) | 0x12)
#define AP_RO_INFO_ACK		((AP_ROSE_ID<<16) | 0x13)
#define AP_RO_INFO_ACK_XAP	((AP_ROSE_ID<<16) | 0x17)
/*
 * The following are the ROSE specific error that
 * the ROSE provider can return.
 */
#define AP_RO_ILLEGAL_SIZE	((AP_ROSE_ID<<16) | 0x14)
#define AP_RO_EMPTY_LIST	((AP_ROSE_ID<<16) | 0x15)
#define AP_RO_CNTX_NOT_PRES	((AP_ROSE_ID<<16) | 0x16)
#define AP_RO_BAD_PCI		((AP_ROSE_ID<<16) | 0X18)
#define AP_RO_T_SYTX_NSUP	((AP_ROSE_ID<<16) | 0X19)
#define AP_RO_BADCD_TYPE	((AP_ROSE_ID<<16) | 0x1A)
/*
 * Attribute identifiers
 */
#define AP_RO_FAC_AVAIL		((AP_ROSE_ID<<16) | 0x01)
#define AP_RO_PCI_LIST		((AP_ROSE_ID<<16) | 0x02)
/*
 * Identifiers for use with ap_free()
 */
#define AP_RO_PCI_LIST_T	AP_RO_PCI_LIST
#define AP_RO_CDATA_T		((AP_ROSE_ID<<16) | 0x03)
/*
 * Bit masks for AP_RO_FAC_AVAIL
 */
#define AP_RO_BIND		(1<<0)
/*
 * Environment Attribute Structure definitions special to ROSE
 */
typedef					/* Abstract syntaxes containing ROSE PDUs */
    struct {
	int size_pcil;			/* Number of PCIs in list */
	int *pci_list;			/* Pointer to an array of PCIs */
} ap_ro_pci_list_t;

typedef struct {
	long udata_length;		/* length of user-data field */
	long rsn;			/* reason for activity or abort/release primitives */
	long evt;			/* event that caused abort */
	long sync_p_sn;			/* synchronization point serial number */
	long sync_type;			/* synchronization type */
	long resync_type;		/* resynchronization type */
	long src;			/* source of abort */
	long res;			/* result of association or release request */
	long res_src;			/* source of result */
	long diag;			/* reason for association rejection */
	unsigned long tokens;		/* tokens identifier: 0 => "tokens absent" */
	unsigned long token_assignment;	/* tokens assignment */
	ap_a_assoc_env_t *env;		/* environment attribute values */
	ap_octet_string_t act_id;	/* activity identifier */
	ap_octet_string_t old_act_id;	/* old activity identifier */
	ap_old_conn_id_t *old_conn_id;	/* old session connection identifier */
	/* 
	 * XAP-ROSE cdata elements
	 */
	long pci;			/* P. context id for user data */
	long priority;			/* Informative to provider, (optional) */
	long invoke_id_present;		/* invoke id present flag */
	long invoke_id;			/* operation invocation identifier */
	long linked_id_present;		/* linked id identifier present */
	long linked_id;			/* invocation identifier of parent operation */
	long class;			/* class of operation */
	long type;			/* value/result/operation */
	union {
		unsigned long local;
		ap_objid_t global;
	} value;			/* value of operation argument */
} ap_ro_cdata_t;

#endif				/* end AP_ROSE_ID */

#endif				/* __XAP_ROSE_H__ */
