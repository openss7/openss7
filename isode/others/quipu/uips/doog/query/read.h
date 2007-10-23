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

#ifndef __QUIPU_UIPS_DOOG_QUERY_READ_H__
#define __QUIPU_UIPS_DOOG_QUERY_READ_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/read.h,v 9.0 1992/06/16 12:45:27 isode Rel */

#ifndef _query_read_h_
#define _query_read_h_

#include "types.h"
#include "error.h"
#include "sequence.h"

typedef struct _photo_data {
	int width;
	int height;

	unsigned char *bytes;
} *photoData;

typedef struct _attr_val_list {
	stringCell attr_name;

	QBool is_photo;

	stringCell val_list;
	photoData photo;

	struct _attr_val_list *next;
} attr_val_list, *attrValList;

#define NULLAVList (attrValList) NULL
#define av_list_alloc() (attrValList) smalloc(sizeof(attr_val_list))

typedef struct _read_results {
	char *base_object;
	attrValList entry;
	errorList errors;
} read_results, *readResults;

#define NULLReadResults (readResults) NULL
#define read_res_alloc() (readResults) smalloc(sizeof(read_results))

typedef struct _read_rec {
	QCardinal request_id;

	char *base_object;

	int task_id;

	readResults results;
} read_rec, *readRec;

#define NULLReadRec (readRec) NULL
#define read_rec_alloc() (readRec) smalloc(sizeof(read_rec))

typedef struct _read_dn_attr_rec {
	QCardinal request_id;

	char *base_object;
	char *read_object;

	int task_id;

	stringCell read_attrs;
	stringCell dn_attr;

	readResults results;
} read_dn_attr_rec, *readDnAttrRec;

#define NULLReadDnAttrRec (readDnAttrRec) NULL
#define read_dn_attr_rec_alloc() \
        (readDnAttrRec) smalloc(sizeof(read_dn_attr_rec))

/*
 *	Procs
 */
QE_error_code do_read();
QE_error_code do_dn_attr_read();

request_state process_read_ds_result(), process_read_ds_error();

request_state process_read_dn_attr_result(), process_read_dn_attr_error();

readResults get_read_results(), get_read_dn_attr_results();

void read_rec_free(), read_dn_attr_rec_free(), read_result_free();

int photo2xbm();

#endif

#endif				/* __QUIPU_UIPS_DOOG_QUERY_READ_H__ */
