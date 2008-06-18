/*****************************************************************************

 @(#) $Id: scodlpi.h,v 0.9.2.1 2008-06-18 16:43:17 brian Exp $

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

 Last Modified $Date: 2008-06-18 16:43:17 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: scodlpi.h,v $
 Revision 0.9.2.1  2008-06-18 16:43:17  brian
 - added new files for NLI and DLPI

 *****************************************************************************/

#ifndef __SYS_SCODLPI_H__
#define __SYS_SCODLPI_H__

#ident "@(#) $RCSfile: scodlpi.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/*
 * This includes UnixWare specific DLPI extensions.  UnixWare documentation
 * shows this header being included without the sys/dlpi.h header, so we will
 * include it here.
 */
#ifdef _UW7_SOURCE
#include <sys/dlpi.h>
#else				/* _UW7_SOURCE */
#define _UW7_SOURCE
#include <sys/dlpi.h>
#undef _UW7_SOURCE
#endif				/* _UW7_SOURCE */

/*
 * Extension primitives defined by UnixWare.  These are not binary compatibile
 * as I have no idea what the primitives values are supposed to be.  I don't
 * even know in which order they are enumerated.
 */

#define DL_UW7	    0x200

#define DL_CLR_STATISTICS_REQ		(DL_UW7 + 0x00)
#define DL_SAP_STATISTICS_REQ		(DL_UW7 + 0x01)
#define DL_SAP_STATISTICS_ACK		(DL_UW7 + 0x02)
#define DL_SET_SRMODE_REQ		(DL_UW7 + 0x03)
#define DL_SRTABLE_REQ			(DL_UW7 + 0x04)
#define DL_SRTABLE_ACK			(DL_UW7 + 0x05)
#define DL_SR_REQ			(DL_UW7 + 0x06)
#define DL_SR_ACK			(DL_UW7 + 0x07)
#define DL_SET_SR_REQ			(DL_UW7 + 0x08)
#define DL_CLR_SR_REQ			(DL_UW7 + 0x09)
#define DL_SET_SRPARMS_REQ		(DL_UW7 + 0x0a)
#define DL_MCTABLE_REQ			(DL_UW7 + 0x0b)
#define DL_MCTABLE_ACK			(DL_UW7 + 0x0c)
#define DL_ENABALLMULTI_REQ		(DL_UW7 + 0x0d)
#define DL_DISABALLMULTI_REQ		(DL_UW7 + 0x0e)
#define DL_ISDN_MSG			(DL_UW7 + 0x0f)
#define DL_DLPI_BILOOPMODE		(DL_UW7 + 0x10)

/*
   DL_CLR_STATISTICS_REQ - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_CLR_STATISTICS_REQ */
} dl_clr_statistics_req_t;

/*
   DL_SAP_STATISTICS_REQ - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_SAP_STATISTICS_REQ */
} dl_sap_statistics_req_t;

/*
   DL_SAP_STATISTICS_ACK - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_SAP_STATISTICS_ACK */
} dl_sap_statistics_ack_t;

/*
   DL_SET_SRMODE_REQ - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_SET_SRMODE_REQ */
} dl_set_srmode_req_t;

/*
   DL_SRTABLE_REQ - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_SRTABLE_REQ */
} dl_srtable_req_t;

/*
   DL_SRTABLE_ACK - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_SRTABLE_ACK */
} dl_srtable_ack_t;

/*
   DL_SR_REQ - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_SR_REQ */
} dl_sr_req_t;

/*
   DL_SR_ACK - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_SR_ACK */
} dl_sr_ack_t;

/*
   DL_SET_SR_REQ - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_SET_SR_REQ */
} dl_set_sr_req_t;

/*
   DL_CLR_SR_REQ - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_CLR_SR_REQ */
} dl_clr_sr_req_t;

/*
   DL_SET_SRPARMS_REQ - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_SET_SRPARMS_REQ */
} dl_set_srparms_req_t;

/*
   DL_MCTABLE_REQ - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_MCTABLE_REQ */
} dl_mctable_req_t;

/*
   DL_MCTABLE_ACK - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_MCTABLE_ACK */
} dl_mctable_ack_t;

/*
   DL_ENABALLMULTI_REQ - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_ENABALLMULTI_REQ */
} dl_enaballmulti_req_t;

/*
   DL_DISABALLMULTI_REQ - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_DISABALLMULTI_REQ */
} dl_disaballmulti_req_t;

/*
   DL_ISDN_MSG - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_ISDN_MSG */
} dl_isdn_msg_t;

/*
   DL_DLPI_BILOOPMODE - one M_PROTO message block
 */
typedef struct {
	dl_ulong dl_primitive;		/* always DL_DLPI_BILOOPMODE */
} dl_dlpi_biloopmode_t;

#endif				/* __SYS_SCODLPI_H__ */
