/*****************************************************************************

 @(#) $Id: x25_proto.h,v 0.9.2.1 2008-06-18 16:43:14 brian Exp $

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

 Last Modified $Date: 2008-06-18 16:43:14 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: x25_proto.h,v $
 Revision 0.9.2.1  2008-06-18 16:43:14  brian
 - added new files for NLI and DLPI

 *****************************************************************************/

#ifndef __SYS_SNET_X25_PROTO_H__
#define __SYS_SNET_X25_PROTO_H__

#ident "@(#) $RCSfile: x25_proto.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

#include <stdint.h>

/* From IRIS SX.25 documentation:
 *
 * The LSAP is defined by the lsapformat structure.  The members of the
 * lsapformat structure are:
 *
 * lsap_len:	This gives the length of the DTE address, the MAC+SAP address,
 *		or the LCI in semi-octets.  For example, for Ethernet, the
 *		length is always 14 to indicate the MAC (12 semi-octets), plus
 *		SAP (2 semi-octets).  The SAP always follows the MAC address.
 *		The DTE can be up to 15 decimal digits unless X.25(88) and
 *		TO/NPI (Type Of Address/Numbering Plan Identification)
 *		addressing is being used, when it can be up to 17 decimal
 *		digits.  For an LCI the length is 3.
 *
 *		The length of the DTE address or LSAP as two BCD digits per
 *		byte, right justified.  An LSAP is always 14 digits long.  A
 *		DTE address can be up to 15 decimal digits unless X.25(88) and
 *		TOA/NPI addressing is used, in which case it can be up to 17
 *		decimal digits.  A PVC_LCI is 3 digits long (hexadecimal,
 *		0-4095).  For TOA/NPI the TOA is:
 *
 *		0000 0 Network-dependent number or unknown
 *		0001 1 International number
 *		0010 2 National number
 *		0011 3 Network specific number (for use in private networks)
 *		0100 4 Complementary address without main address.
 *		0101 5 Alernative address.
 *
 *		NPI for other than Alternative Address is:
 *
 *		0000 0 Network-dependent number or unknown
 *		0001 1 Rec. E.164 (digital)
 *		0010 2 Rec. E.164 (analog)
 *		0011 3 Rec. X.121
 *		0100 4 Rec. F.69 (telex numbering plan)
 *		0101 5 Private numbering plan (for private use only)
 *
 *		NPI when TOA is Alternative Address is:
 *
 *		0000 0 Character string coding to ISO/IEC 646.
 *		0001 1 OSI NSAP address coded per X.213/ISO 8348.
 *		0010 2 MAC address per IEEE 802.
 *		0011 3 Internet Address per RFC 1166. (i.e. an IPv4 address)
 *
 *
 * lsap_add:	The DTE address, LSAP or PVC_LCI as two BCD digtis per byte,
 *		right justified.
 */
#define LSAPMAXSIZE		9

struct lsapformat {
	uint8_t lsap_len;
	uint8_t lsap_add[LSAPMAXSIZE];
};

/* From IRIS SX.25 documentation:
 *
 * Addressing is defined by the xaddrf structure.  The members of the xaddrf
 * structure are:
 *
 * sn_id:	The subnetwork identifier, selected by the system
 *		administrator.  It identifies the subnetwork required for a
 *		Connect Request, or on which a Connect Indication arrived.
 *		The sn_id field holds a representation of the one byte string
 *		subnetwork identifier as an unsigned long.  The X.25 library
 *		routin snidtox25() can be used to convert the character
 *		subnetwork identifier to an unsigned long.
 *
 *		Note that Solstice X.25 uses link_id here instead of sn_id.
 *
 * aflags:	Specifies the options required or used by the subnetwork to
 *		encode and interpret addresses.  These take on of these values:
 *
 *		NSAP_ADDR   0x00    NSAP field contains OSI-encoded NSAP
 *				    address.
 *		EXT_ADDR    0x01    NSAP field contains non-OSI-encoded
 *				    extended address.
 *		PVC_LCI	    0x02    NSAP field contains a PVC number.
 *
 *		When the NSAP field is empty, aflags has the value 0.
 *
 * DTE_MAC:	The DTE address or LSAP as two BCD digits per byte, right
 *		justified, or the PVC_LCI as three BCD digits with two digits
 *		per byte, right justified.
 *
 * nsap_len:	The length in semi-octets of the NSAP as two BCD digits per
 *		byte, right justified.
 *
 * NSAP:	The NSAP or address extension (see aflags) as two BCD digits
 *		per byte, right justified.
 */
#define NSAPMAXSIZE 20
struct xaddrf {
	uint32_t sn_id;
	unsigned char aflags;
#define EXT_ADDR	0x00		/* X.121 subaddress */
#define NSAP_ADDR	0x01		/* NSAP address */
#define PVC_LCI		0x02		/* PVC LCI number 0-4095 3 semi-octets */
	struct lsapformat DTE_MAC;	/* X.121 DTE address or IEEE 802 MAC */
	unsigned char nsap_len;
	unsigned char NSAP[NSAPMAXSIZE];
};

#define MAX_NUI_LEN	64
#define MAX_RPOA_LEN	 8
#define MAX_CUG_LEN	 2
#if 0
#define MAX_FAC_LEN	32
#else
#define MAX_FAC_LEN	109
#endif
#define MAX_TARRIFS	 4
#define MAX_CD_LEN	(MAX_TARRIFS * 4)
#define MAX_SC_LEN	(MAX_TARRIFS * 8)
#define MAX_MU_LEN	16

/*
 * Extra format (facilities) structure from Solstice X.25 and IRIS SX.25
 * documentation.
 */
struct extraformat {
	unsigned char fastselreq;
	unsigned char restrictresponse;
	unsigned char reversecharges;
	unsigned char pwoptions;
#define NEGOT_PKT	0x01	/* packet size negotiable */
#define NEGOT_WIN	0x02	/* window size negotiable */
#define ASSERT_HWM	0x04	/* concatenation limit assert */
	unsigned char locpacket;
	unsigned char rempacket;
#define DEF_X25_PKT	7	/* the standard default packet size */
	unsigned char locwsize;
	unsigned char remwsize;
#define DEF_X25_WIN	2	/* the standard default window size */
	int nsdulimit;
	unsigned char nui_len;
	unsigned char nui_field[MAX_NUI_LEN];
	unsigned char rpoa_len;
	unsigned char rpoa_field[MAX_RPOA_LEN];
	unsigned char cug_type;
#define CUG		1	/* closed user group, up to four semi-octets */
#define BCUG		2	/* bilateral CUG (two members only), for semi-octets */
	unsigned char cug_field[MAX_CUG_LEN];
	unsigned char reqcharging;
	unsigned char chg_cd_len;
	unsigned char chg_cd_field[MAX_CD_LEN];
	unsigned char chg_sc_len;
	unsigned char chg_sc_field[MAX_SC_LEN];
	unsigned char chg_mu_len;
	unsigned char chg_mu_field[MAX_MU_LEN];
	unsigned char called_add_mod;
	unsigned char call_redirect;
	struct lsapformat called;
	unsigned char call_deflect;
	unsigned char x_fac_len;
	unsigned char cq_fac_len;
	unsigned char cd_fac_len;
	unsigned char fac_field[MAX_FAC_LEN];
};

/*
 * QOS format structure: from Solstice X.25 and IRIS SX.25 documentation.
 */

#define MAX_PROT 32

struct qosformat {
	unsigned char reqtclass;
	unsigned char locthroughput;
	unsigned char remthroughput;
	unsigned char reqminthruput;
	unsigned char locminthru;
	unsigned char remminthru;
	unsigned char reqtransitdelay;
	unsigned short transitdelay;
	unsigned char reqmaxtransitdelay;
	unsigned short acceptable;
	unsigned char reqpriority;
	unsigned char reqprtygain;
	unsigned char reqprtykeep;
	unsigned char prtydata;
	unsigned char prtygain;
	unsigned char prtykeep;
	unsigned char reqlowprtydata;
	unsigned char reqlowprtygain;
	unsigned char reqlowprtykeep;
	unsigned char lowprtydata;
	unsigned char lowprtygain;
	unsigned char lowprtykeep;
	unsigned char protection_type;
#define PRT_SRC		1   /* source address specific */
#define PRT_DST		2   /* destination address specific */
#define PRT_GLB		3   /* globally unique */
	unsigned char prot_len;
	unsigned char lowprot_len;
	unsigned char protection[MAX_PROT];
	unsigned char lowprotection[MAX_PROT];
	unsigned char reqexpedited;
	unsigned char reqackservice;
#define RC_CONF_DTE	1
#define RC_CONF_APP	2
	struct extraformat xtras;
};

/*
 * Diagnostic codes from Solstice X.25 and IRIS SX.25 documentation.   Note
 * that the values themselves are from ISO/IEC 8208 and are mapped from X.25
 * cause and diagnostic codes as described in ISO/IEC 8878.
 */
/*
 * To identify the originator in N_RI and N_DI messages
 */
#define NS_USER			0x01
#define NS_PROVIDER		0x02

/*
 * Reason when the originator is NS Provider
 */
#define NS_GENERIC		0xe0
#define NS_DTRANSIENT		0xe1
#define NS_DPERMENEN		0xe2
#define NS_TUNSPECIFIED		0xe3
#define NS_PUNSPECIFIED		0xe4
#define NS_QOSNATRANSIENT	0xe5
#define NS_QOSNAPERMENENT	0xe6
#define NS_NSAPTUNREACHABLE	0xe7
#define NS_NSAPPUNREACHABLE	0xe8
#define NS_NSAPPUNKNOWN		0xeb

/*
 * Reason when the originator is NS User
 */
#define NU_GENERIC		0xf0
#define NU_DNORMAL		0xf1
#define NU_DABNORMAL		0xf2
#define NU_DINCOMPUSERDATA	0xf3
#define NU_TRANSIENT		0xf4
#define NU_PERMANENT		0xf5
#define NU_QOSNATRANSIENT	0xf6
#define NU_QOSNAPERMENENT	0xf7
#define NU_INCOMPUSERDATA	0xf8
#define NU_BADPROTID		0xf9

/*
 * To specify the reason when the originator is NS Provider in N_RI messages
 */
#define NS_RUNSPECIFIED		0xe9
#define NS_RCONGESTION		0xea

/*
 * To specify the reason when the originator is NS User in N_RI messages
 */
#define NU_RESYNC		0xfa

/*
 * X.25 Primitive structures taken from IRIS SX.25 documentation.
 */

#define XL_CTL	    0
#define XL_DAT	    1

#define N_Abort	    10

struct xabortf {
	unsigned char xl_type;		/* always XL_CTL */
	unsigned char xl_command;	/* always N_Abort */
	/* No data part */
};

typedef struct xhdrf {
	unsigned char xl_type;		/* XL_CTL/XL_DAT */
	unsigned char xl_command;	/* Command */
} S_X25_HDR;

/*
 * X.25 primitives union from Solstice X.25 and IRIS SX.25 documentation.  Both
 * documetnation sources contain errors (maker with "[sic]" below).
 */
typedef union x25_primitives {
	struct xhdrf xhdr;		/* header */
	struct xcallf xcall;		/* connect request/indication */
	struct xccnff xccnf;		/* connect confirm/response */
	struct xdataf xdata;		/* normal, q-bit or d-bit data */
	struct xdatacf xdatac;		/* data ack */
	struct xedataf xedata;		/* expedited data */
	struct xedatacf xedatac;	/* expedited data ack */
	struct xrstf xrst;		/* reset request/indication */
	struct xrscf xrsc;		/* reset confirm/response */
	struct xrscf xrscf;		/* reset confirm/response [sic] */
	struct xdiscf xdisc;		/* disconnect request/indication */
	struct xdcnff xdcnf;		/* disconnect confirm */
	struct xabortf xabort;		/* abort indication */
	struct xabortf abort;		/* abort indication [sic] */
	struct xlistenf xlisten;	/* listen command/response */
	struct xcanlisf xcanlis;	/* cancel command/response */
	struct pvcattf pvcatt;		/* PVC attach */
	struct pvcdetf pvcdet;		/* PVC detach */
} x25_types;

#endif				/* __SYS_SNET_X25_PROTO_H__ */
