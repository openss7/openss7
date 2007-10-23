/*****************************************************************************

 @(#) $Id$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __QUIPU_UIPS_DOOG_QUERY_REQUEST_H__
#define __QUIPU_UIPS_DOOG_QUERY_REQUEST_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/request.h,v 9.0 1992/06/16 12:45:27 isode Rel */

#ifndef _query_request_h_
#define _query_request_h_

#include "types.h"
#include "error.h"
#include "ufname.h"
#include "ufsearch.h"
#include "modify.h"
#include "read.h"

#define MAXREQUESTS 64

/*
 *	Request type identifier
 */
typedef enum {
	DS_READ,
	READ_DN_ATTR,
	DS_SEARCH,
	UFNAME,
	UFSEARCH,
	ENTRY_TEMPLATE,
	MODIFY_ENTRY,
	MODIFY_RDN,
	REMOVE_ENTRY,
	ADD_ENTRY
} request_type;

/*
 *	Generic request record
 */
typedef struct _request_rec {
	request_state status;
	errorList errors;
	request_type type;
	QCardinal request_id;

	struct _request_rec *next;
	struct _request_rec *prev;

	union {
		ufnameRec ufname_request;
		ufsearchRec ufsearch_request;
		readRec read_request;
		readDnAttrRec read_dn_attr_request;
		modifyRec modify_request;
		makeTemplateRec template_request;
	} req_type_rec;

#define UFNAME_REC req_type_rec.ufname_request
#define UFSEARCH_REC req_type_rec.ufsearch_request
#define READ_REC req_type_rec.read_request
#define READ_DN_ATTR_REC req_type_rec.read_dn_attr_request
#define MODIFY_REC req_type_rec.modify_request
#define TEMPLATE_REC req_type_rec.template_request

} request_rec, *requestRec;

#define NULLReqRec (requestRec) NULL
#define request_rec_alloc() (requestRec) smalloc(sizeof(request_rec))

/*
 *	Procedures
 */

QE_error_code _request_invoked();
void _request_complete();

QCardinal directory_wait();
void abort_request();
requestRec _get_request_of_id();

#endif				/* _query_request_h_ */

#endif				/* __QUIPU_UIPS_DOOG_QUERY_REQUEST_H__ */
