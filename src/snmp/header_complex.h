/*****************************************************************************

 @(#) src/snmp/header_complex.h

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

 *****************************************************************************/

#ifndef __LOCAL_HEADER_COMPLEX_H__
#define __LOCAL_HEADER_COMPLEX_H__

/*
 *  header_complex.h:  More complex storage and data sorting for mib modules
 */
struct header_complex_index {
	oid *name;
	size_t namelen;
	void *data;
	struct header_complex_index *next;
	struct header_complex_index *prev;
};

/*
 * Function pointer called by the header_comlpex functions when a client
 * pointer (void * to us) needs to be cleaned.
 */

typedef void (HeaderComplexCleaner) (void *);

void *header_complex(struct header_complex_index *datalist, struct variable *vp, oid * name,
		     size_t *length, int exact, size_t *var_len, WriteMethod ** write_method);

int header_complex_generate_varoid(struct variable_list *var);

int header_complex_parse_oid(oid * oidIndex, size_t oidLen, struct variable_list *data);

void header_complex_generate_oid(oid * name, size_t *length, oid * prefix, size_t prefix_len,
				 struct variable_list *data);

void header_complex_free_all(struct header_complex_index *thestuff, HeaderComplexCleaner * cleaner);

void header_complex_free_entry(struct header_complex_index *theentry,
			       HeaderComplexCleaner * cleaner);

void *header_complex_extract_entry(struct header_complex_index **thetop,
				   struct header_complex_index *thespot);

struct header_complex_index *header_complex_find_entry(struct header_complex_index *thestuff,
						       void *entry);

void *header_complex_get(struct header_complex_index *datalist, struct variable_list *index);

struct header_complex_index *header_complex_add_data(struct header_complex_index **thedata,
						     struct variable_list *var, void *data);

#endif				/* __LOCAL_HEADER_COMPLEX_H__ */
