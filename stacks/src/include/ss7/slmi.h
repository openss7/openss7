/*****************************************************************************

 @(#) $Id: slmi.h,v 0.9.2.5 2007/08/14 12:17:10 brian Exp $

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

 Last Modified $Date: 2007/08/14 12:17:10 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: slmi.h,v $
 Revision 0.9.2.5  2007/08/14 12:17:10  brian
 - GPLv3 header updates

 Revision 0.9.2.4  2007/02/13 14:05:29  brian
 - corrected ulong and long for 32-bit compat

 *****************************************************************************/

#ifndef __SS7_SLMI_H__
#define __SS7_SLMI_H__

#ident "@(#) $RCSfile: slmi.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

#define SLM_PROTO_BASE			 8192L

#define SLM_DSTR_FIRST			(   1L + SLM_PROTO_BASE)
#define SLM_LINK_REQ			(   1L + SLM_PROTO_BASE)
#define SLM_ATTACH_RES			(   2L + SLM_PROTO_BASE)
#define SLM_DETACH_RES			(   3L + SLM_PROTO_BASE)
#define SLM_ENABLE_RES			(   4L + SLM_PROTO_BASE)
#define SLM_DISABLE_RES			(   5L + SLM_PROTO_BASE)
#define SLM_DSTR_LAST			(   5L + SLM_PROTO_BASE)

#define SLM_USTR_LAST			(  -1L - SLM_PROTO_BASE)
#define SLM_OK_ACK			(  -1L - SLM_PROTO_BASE)
#define SLM_ERROR_ACK			(  -2L - SLM_PROTO_BASE)
#define SLM_ATTACH_IND			(  -3L - SLM_PROTO_BASE)
#define SLM_DETACH_IND			(  -4L - SLM_PROTO_BASE)
#define SLM_ENABLE_IND			(  -5L - SLM_PROTO_BASE)
#define SLM_DISABLE_IND			(  -6L - SLM_PROTO_BASE)
#define SLM_USTR_FIRST			(  -6L - SLM_PROTO_BASE)

/*
 *  SLMI PROTOCOL PRIMITIVES
 */

/*
 *  SLM_LINK_REQ
 */
typedef struct {
	lmi_long prim;
	lmi_ulong devnum;
	lmi_ulong linkid;
} slm_link_req_t;

/*
 *  SLM_OK_ACK
 */
typedef struct {
	lmi_long prim;
	lmi_long correct_prim;
} slm_ok_ack_t;

/*
 *  SLM_ERROR_ACK
 */
typedef struct {
	lmi_long prim;
	lmi_long error_prim;
	lmi_long errno;
} slm_error_ack_t;

/*
 *  SLM_ATTACH_IND
 */
typedef struct {
	lmi_long prim;
	lmi_ulong devnum;
	lmi_ulong linkid;
} slm_attach_ind_t;

/*
 *  SLM_ATTACH_RES
 */
typedef struct {
	lmi_long prim;
	lmi_ulong devnum;
	lmi_ulong index;
} slm_attach_ind_t;

/*
 *  SLM_DETACH_IND
 */
typedef struct {
	lmi_long prim;
	lmi_ulong devnum;
} slm_detach_ind_t;

/*
 *  SLM_ENABLE_IND
 */
typedef struct {
	lmi_long prim;
	lmi_ulong devnum;
} slm_enable_ind_t;

/*
 *  SLM_DISABLE_IND
 */
typedef struct {
	lmi_long prim;
	lmi_ulong devnum;
} slm_disable_ind_t;

union SLM_primitives {
	lmi_long prim;
	slm_link_req_t link_req;
	slm_attach_res_t attach_res;
	slm_detach_res_t detach_res;
	slm_enable_res_t enable_res;
	slm_disable_res_t disable_res;
	slm_ok_ack_t ok_ack;
	slm_error_ack_t error_ack;
	slm_attach_ind_t attach_ind;
	slm_detach_ind_t detach_ind;
	slm_enable_ind_t enable_ind;
	slm_disable_ind_t disable_ind;
};

typedef union SLM_primitives slm_prim_t;

#define SLM_LINK_REQ_SIZE	sizeof(slm_link_req_t)
#define SLM_ATTACH_RES_SIZE	sizeof(slm_attach_res_t)
#define SLM_DETACH_RES_SIZE	sizeof(slm_detach_res_t)
#define SLM_ENABLE_RES_SIZE	sizeof(slm_enable_res_t)
#define SLM_DISABLE_RES_SIZE	sizeof(slm_disable_res_t)
#define SLM_OK_ACK_SIZE		sizeof(slm_ok_ack_t)
#define SLM_ERROR_ACK_SIZE	sizeof(slm_error_ack_t)
#define SLM_ATTACH_IND_SIZE	sizeof(slm_attach_ind_t)
#define SLM_DETACH_IND_SIZE	sizeof(slm_detach_ind_t)
#define SLM_ENABLE_IND_SIZE	sizeof(slm_enable_ind_t)
#define SLM_DISABLE_IND_SIZE	sizeof(slm_disable_ind_t)

#endif				/* __SS7_SLMI_H__ */
