/*****************************************************************************

 @(#) $Id: dl.h,v 0.9.2.4 2007/02/13 14:05:28 brian Exp $

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

 $Log: dl.h,v $
 Revision 0.9.2.4  2007/02/13 14:05:28  brian
 - corrected ulong and long for 32-bit compat

 *****************************************************************************/

#ifndef __DL_H__
#define __DL_H__

#ident "@(#) $RCSfile: dl.h,v $ $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

#define MDL_ASSIGN_REQ		0
#define MDL_ERROR_RES		1
#define MDL_REMOVE_REQ		2
#define MDL_UNITDATA_REQ	3

#define MDL_ERROR_IND		16
#define MDL_ASSIGN_IND		17
#define MDL_UNITDATA_IND	18

#define DL_ESTABLISH_REQ	4
#define DL_RELEASE_REQ		5
#define DL_DATA_REQ		6
#define DL_UNITDATA_REQ		7

#define DL_ESTABLISH_CON	19
#define DL_ESTABLISH_IND	20
#define DL_RELEASE_CON		21
#define DL_RELEASE_IND		22
#define DL_DATA_IND		23
#define DL_UNITDATA_IND		24

/*
 *  MDL_ASSIGN_REQ      0       M_PROTO block
 */
typedef struct {
	dl_long mdl_primitive;		/* always MDL_ASSIGN_REQ */
	dl_long mdl_tei;		/* TEI value */
	dl_long mdl_ces;		/* CES */
} mdl_assign_req_t;

/*
 *  MDL_ASSIGN_IND      17      M_PROTO block
 */
typedef struct {
	dl_long mdl_primitive;		/* always MDL_ASSIGN_IND */
	dl_long mdl_tei;		/* TEI value */
} mdl_assign_ind_t;

/*
 *  MDL_ERROR_RES       1       M_PROTO block
 */
typedef struct {
	dl_long mdl_primitive;		/* always MDL_ERROR_RES */
	dl_long mdl_reason;		/* reason for error message */
} mdl_error_res_t;

/*
 *  MDL_ERROR_IND       16      M_PROTO block
 */
typedef struct {
	dl_long mdl_primitive;		/* always MDL_ERROR_IND */
	dl_long mdl_reason;		/* reason for error message */
} mdl_error_ind_t;

/*
 *  MDL_REMOVE_REQ      2       M_PROTO block
 */
typedef struct {
	dl_long mdl_primitive;		/* always MDL_REMOVE_REQ */
	dl_long mdl_tei;		/* TEI value */
	dl_long mdl_ces;		/* CES */
} mdl_remove_req_t;

/*
 *  MDL_UNITDATA_REQ    3       M_PROTO followed by one or more M_DATA blocks
 */
typedef struct {
	dl_long mdl_primitive;		/* always MDL_UNITDATA_REQ */
} mdl_unitdata_req_t;

/*
 *  MDL_UNITDATA_IND    18      M_PROTO followed by one or more M_DATA blocks
 */
typedef struct {
	dl_long mdl_primitive;		/* always MDL_UNITDATA_IND */
} mdl_unitdata_ind_t;

/*
 *  DL_ESTABLISH_REQ    4       M_PROTO block
 */
typedef struct {
	dl_long dl_primitive;		/* always DL_ESTABLISH_REQ */
} dl_establish_req_t;

/*
 *  DL_ESTABLISH_CON    19      M_PROTO block
 */
typedef struct {
	dl_long dl_primitive;		/* always DL_ESTABLISH_CON */
} dl_establish_con_t;

/*
 *  DL_ESTABLISH_IND    20      M_PROTO block
 */
typedef struct {
	dl_long dl_primitive;		/* always DL_ESTABLISH_IND */
} dl_establish_ind_t;

/*
 *  DL_RELEASE_REQ      5       M_PROTO block
 */
typedef struct {
	dl_long dl_primitive;		/* always DL_RELEASE_REQ */
} dl_release_req_t;

/*
 *  DL_RELEASE_CON      21      M_PROTO block
 */
typedef struct {
	dl_long dl_primitive;		/* always DL_RELEASE_CON */
} dl_release_con_t;

/*
 *  DL_RELEASE_IND      22      M_PROTO block
 */
typedef struct {
	dl_long dl_primitive;		/* always DL_RELEASE_IND */
} dl_release_ind_t;

/*
 *  DL_DATA_REQ         6       M_PROTO followed by one or more M_DATA blocks
 */
typedef struct {
	dl_long dl_primitive;		/* always DL_DATA_REQ */
} dl_data_req_t;

/*
 *  DL_DATA_IND         23      M_PROTO followed by one or more M_DATA blocks
 */
typedef struct {
	dl_long dl_primitive;		/* always DL_DATA_IND */
} dl_data_ind_t;

/*
 *  DL_UNITDATA_REQ     7       M_PROTO followed by one ore more M_DATA blocks
 */
typedef struct {
	dl_long dl_primitive;		/* always DL_UNITDATA_REQ */
} dl_unitdata_req_t;

/*
 *  DL_UNITDATA_IND     24      M_PROTO followed by one ore more M_DATA blocks
 */
typedef struct {
	dl_long dl_primitive;		/* always DL_UNITDATA_IND */
} dl_unitdata_ind_t;

#endif				/* __DL_H__ */
