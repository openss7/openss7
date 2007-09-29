/*****************************************************************************

 @(#) $Id: xomi.h,v 0.9.2.1 2007/09/29 14:08:34 brian Exp $

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

 $Log: xomi.h,v $
 Revision 0.9.2.1  2007/09/29 14:08:34  brian
 - added new files

 *****************************************************************************/

#ifndef __XOMI_H__
#define __XOMI_H__

#ident "@(#) $RCSfile: xomi.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

typedef OM_return_code (*OMP_copy) (OM_private_object original, OM_workspace workspace,
				    OM_private_object *copy);

typedef OM_return_code (*OMP_copy_value) (OM_private_object source, OM_type source_type,
					  OM_value_position source_value_position,
					  OM_private_object destination, OM_type destination_type,
					  OM_value_position destination_value_position);

typedef OM_return_code (*OMP_create) (OM_object_identifier class, OM_boolean initialise,
				      OM_workspace workspace, OM_private_object *object);

typedef OM_return_code (*OMP_decode) (OM_private_object encoding, OM_private_object *original);

typedef OM_return_code (*OMP_delete) (OM_object subject);

typedef OM_return_code (*OMP_encode) (OM_private_object original, OM_object_identifier rules,
				      OM_private_object *encoding);

typedef OM_return_code (*OMP_get) (OM_private_object original, OM_exclusions exclusions,
				   OM_type_list included_types, OM_boolean local_strings,
				   OM_value_position initial_value,
				   OM_value_position limiting_value, OM_public_object *copy,
				   OM_value_position *total_number);

typedef OM_return_code (*OMP_instance) (OM_object subject, OM_object_identifier class,
					OM_boolean *instance);

typedef OM_return_code (*OMP_put) (OM_private_object destination, OM_modification modification,
				   OM_object source, OM_type_list included_types,
				   OM_value_position initial_value,
				   OM_value_position limiting_value);

typedef OM_return_code (*OMP_read) (OM_private_object subject, OM_type type,
				    OM_value_position value_position, OM_boolean local_string,
				    OM_string_length *string_offset, OM_string *elements);

typedef OM_return_code (*OMP_remove) (OM_private_object subject, OM_type type,
				      OM_value_position initial_value,
				      OM_value_position limiting_value);

typedef OM_return_code (*OMP_write) (OM_private_object subject, OM_type type,
				     OM_value_position value_position, OM_syntax syntax,
				     OM_string_length *string_offset, OM_string elements);

typedef struct OMP_functions_body {
	OM_uint32 function_number;
	OMP_copy omp_copy;
	OMP_copy_value omp_copy_value;
	OMP_create omp_create;
	OMP_decode omp_decode;
	OMP_delete omp_delete;
	OMP_encode omp_encode;
	OMP_get omp_get;
	OMP_instance omp_instance;
	OMP_put omp_put;
	OMP_read omp_read;
	OMP_remove omp_remove;
	OMP_write omp_write;
} OMP_functions;

typedef struct OMP_workspace_body {
	struct OMP_functions_body *functions;
} *OMP_workspace;

/* USEFUL MACROS */

/* The following macro converts (a pointer to) the internal representation of an
 * object to (a pointer to) the external representation. */
#define OMP_EXTERNAL(internal) \
((OM_object)((OM_descriptor *)(internal) + 1))

/* The following macro converts (a pointer to) the external representation of an
 * object to (a pointer to) the internal representation. */
#define OMP_INTERNAL(external) ((OM_descriptor *)(external) - 1)

/* The following macro extracts the type component of a descriptor, given the
 * pointer to it. */
#define OMP_TYPE(desc) (((OM_descriptor *)(desc))->type)

/* The following macro extracts the workspace of an object, given the external
 * pointer to it. The effect of applying it to a client-generated public object
 * is undefined. */
#define OMP_WORKSPACE(external) \
((OMP_workspace)(OMP_INTERNAL(external)->value.string.elements))

/* The following macro extracts the function jump-table associated with an
 * object, given the external pointer to it. The effect of applying it to a
 * client-generated public object is undefined. */
#define OMP_FUNCTIONS(external) (OMP_WORKSPACE(external)->functions)

#include <stddef.h>
#define om_copy(ORIGINAL,WORKSPACE,COPY) \
	( (WORKSPACE) == NULL ? \
	  OM_NO_SUCH_WORKSPACE : \
	  (((OMP_workspace)(WORKSPACE) )->functions->omp_copy( \
		(ORIGINAL),(WORKSPACE),(COPY) )) \
	)

#define om_create(CLASS,INITIALISE,WORKSPACE,OBJECT) \
	( (WORKSPACE) == NULL ? \
	  OM_NO_SUCH_WORKSPACE : \
	  (((OMP_workspace)(WORKSPACE) )->functions->omp_create( \
		(CLASS),(INITIALISE),(WORKSPACE),(OBJECT) )) \
	)

#define om_delete(SUBJECT) \
	( (SUBJECT) == NULL ? \
	  OM_NO_SUCH_OBJECT : \
	  (((SUBJECT)->syntax & OM_S_SERVICE_GENERATED) ? \
		(OMP_FUNCTIONS(SUBJECT)->omp_delete( (SUBJECT))) : \
		OM_NOT_THE_SERVICES \
	  ) \
	)

#define om_instance(SUBJECT,CLASS,INSTANCE) \
	( (SUBJECT) == NULL ? \
	  OM_NO_SUCH_OBJECT : \
	  (((SUBJECT)->syntax & OM_S_SERVICE_GENERATED) ? \
		(OMP_FUNCTIONS(SUBJECT)->omp_instance( \
		(SUBJECT),(CLASS),(INSTANCE) )) : \
		OM_NOT_THE_SERVICES \
	  ) \
	)

#define om_copy_value(SOURCE, SOURCE_TYPE, SOURCE_POSITION, DEST, DEST_TYPE, DEST_POSITION) \
	( (DEST) == NULL ? \
	  OM_NO_SUCH_OBJECT : \
	  (((DEST)->syntax & OM_S_PRIVATE) ? \
		(OMP_FUNCTIONS(DEST)->omp_copy_value( \
		(SOURCE), (SOURCE_TYPE), (SOURCE_POSITION), \
		(DEST), (DEST_TYPE), (DEST_POSITION) )): \
		OM_NOT_PRIVATE \
	  ) \
	)

#define om_decode(ENCODING,ORIGINAL) \
	( (ENCODING) == NULL ? \
	  OM_NO_SUCH_OBJECT : \
	  (((ENCODING)->syntax & OM_S_PRIVATE) ? \
		(OMP_FUNCTIONS(ENCODING)->omp_decode( (ENCODING),(ORIGINAL) )): \
		OM_NOT_PRIVATE \
	  ) \
	)

#define om_encode(ORIGINAL,RULES,ENCODING) \
	( (ORIGINAL) == NULL ? \
	  OM_NO_SUCH_OBJECT : \
	  (((ORIGINAL)->syntax & OM_S_PRIVATE) ? \
		(OMP_FUNCTIONS(ORIGINAL)->omp_encode( \
		(ORIGINAL),(RULES),(ENCODING) )) : \
		OM_NOT_PRIVATE \
	  ) \
	)

#define om_get(ORIGINAL,EXCLUSIONS,TYPES,LOCAL_STRINGS, INITIAL,LIMIT,COPY,TOTAL_NUMBER) \
	( (ORIGINAL) == NULL ? \
	  OM_NO_SUCH_OBJECT : \
	  (((ORIGINAL)->syntax & OM_S_PRIVATE) ? \
		(OMP_FUNCTIONS(ORIGINAL)->omp_get( \
		(ORIGINAL),(EXCLUSIONS),(TYPES),(LOCAL_STRINGS), \
		(INITIAL),(LIMIT),(COPY),(TOTAL_NUMBER) )): \
		OM_NOT_PRIVATE \
	  ) \
	)

#define om_put(DESTINATION,MODIFICATION,SOURCE,TYPES,INITIAL,LIMIT) \
	( (DESTINATION) == NULL ? \
	  OM_NO_SUCH_OBJECT : \
	  (((DESTINATION)->syntax & OM_S_PRIVATE) ? \
		(OMP_FUNCTIONS(DESTINATION)->omp_put( \
		(DESTINATION),(MODIFICATION),(SOURCE),(TYPES), \
		(INITIAL),(LIMIT) )) : \
		OM_NOT_PRIVATE \
	  ) \
	)

#define om_read(SUBJECT,TYPE,VALUE_POS,LOCAL_STRING, STRING_OFFSET,ELEMENTS) \
	( (SUBJECT) == NULL ? \
	  OM_NO_SUCH_OBJECT : \
	  (((SUBJECT)->syntax & OM_S_PRIVATE) ? \
		(OMP_FUNCTIONS(SUBJECT)->omp_read( \
		(SUBJECT),(TYPE),(VALUE_POS),(LOCAL_STRING), \
		(STRING_OFFSET),(ELEMENTS) )) : \
		OM_NOT_PRIVATE \
	  ) \
	)

#define om_remove(SUBJECT,TYPE,INITIAL,LIMIT) \
	( (SUBJECT) == NULL ? \
	  OM_NO_SUCH_OBJECT : \
	  (((SUBJECT)->syntax & OM_S_PRIVATE) ? \
		(OMP_FUNCTIONS(SUBJECT)->omp_remove( \
		(SUBJECT),(TYPE),(INITIAL),(LIMIT) )) : \
		OM_NOT_PRIVATE \
	  ) \
	)

#define om_write(SUBJECT,TYPE,VALUE_POS,SYNTAX,STRING_OFFSET,ELEMENTS) \
	( (SUBJECT) == NULL ? \
	  OM_NO_SUCH_OBJECT : \
	  (((SUBJECT)->syntax & OM_S_PRIVATE) ? \
		(OMP_FUNCTIONS(SUBJECT)->omp_write( \
		(SUBJECT),(TYPE),(VALUE_POS), \
		(SYNTAX),(STRING_OFFSET),(ELEMENTS) )) : \
		OM_NOT_PRIVATE \
	  ) \
	)

#endif				/* __XOMI_H__ */
