/*****************************************************************************

 @(#) $RCSfile: xom.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-04-25 08:33:40 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 Last Modified $Date: 2008-04-25 08:33:40 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xom.c,v $
 Revision 0.9.2.1  2008-04-25 08:33:40  brian
 - added man pages and skeletons

 *****************************************************************************/

#ident "@(#) $RCSfile: xom.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-04-25 08:33:40 $"

static char const ident[] = "$RCSfile: xom.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-04-25 08:33:40 $";

/* This file can be processed with doxygen(1). */

/** @weakgroup xom OpenSS7 XOM Library
  * @{ */

/** @file
  * OpenSS7 X/Open Object Model (XOM) implementation file. */

/**
  * This manual contains documentation of the OpenSS7 XOM Library functions that
  * are generated automatically from the source code with doxygen.  This
  * documentation is intended to be used for maintainers of the OpenSS7 XOM
  * Library and is not intended for users of the OpenSS7 XOM Library.  Users
  * should consult the documentation found in the user manual pages beginning
  * with xom(3).
  */

#include <xom.h>

#undef om_copy

/** @brief
  * @param original The original XOM object, which remains accessible.
  * @param workspace The workspace in which the copy is to be created.  The
  *	original's class shall be in a package associated with this workspace.
  * @param copy A pointer to the object to be returned.  The returned object
  *	will only be accessible if the call is successful.
  * @version XOM_1.0
  * @par Alias:
  * This symbol is an implementation of om_copy().
  */
OM_return_code
__xom_om_copy(const OM_private_object original, const OM_workspace workspace,
	      OM_private_object * copy)
{
	if (unlikely(workspace == NULL))
		return (OM_NO_SUCH_WORKSPACE);
	if (unlikely(workspace->functions->omp_copy == NULL))
		return (OM_FUNCTION_DECLINED);
	return (*workspace->functions->omp_copy)
	    (original, workspace, copy);
}

/** @fn OM_return_code om_copy(const OM_private_object original, const OM_workspace workspace, OM_private_object * copy)
  * @param original The original XOM object, which remains accessible.
  * @param workspace The workspace in which the copy is to be created.  The
  *	original's class shall be in a package associated with this workspace.
  * @param copy A pointer to the object to be returned.  The returned object
  *	will only be accessible if the call is successful.
  * @version XOM_1.0
  * @par Alias:
  * This symbol is a strong alias of __xom_om_copy().
  *
  * This function creates a new private object, the @copy, that is an exact, but
  * independent copy of an existing private object, the @original.  The function
  * is recursive in that copying the original also copies its subobjects.
  *
  * Upon successful completion, the zero value of [%OM_SUCCESS] is returned and
  * the new copy of the original is returned in the object pointed to by @copy.
  * Otherwise, a non-zero diagnostic error value is returned and @copy is
  * unaffected.
  *
  * When om_copy() fails, it returns a non-zero error code, as follows:
  *
  * [%OM_NO_SUCH_WORKSPACE]
  *	The @workspace argument is %NULL.
  * [%OM_FUNCTION_DECLINED]
  *	The @workspace does no support the om_copy() function.
  */
__asm__(".symver __xom_om_copy,om_copy@@XDS_1.0");

#undef om_copy_value

/** @brief
  * @param source
  * @param source_type
  * @param source_value_position
  * @param destination
  * @param destination_type
  * @param destination_value_position
  * @version XOM_1.0
  * @par Alias:
  * This symbol is an implementation of om_copy_value().
  */
OM_return_code
__xom_om_copy_value(const OM_private_object source, const OM_type source_type,
		    const OM_value_position source_value_position,
		    OM_private_object destination, OM_type destination_type,
		    OM_value_position destination_value_position)
{
	OMP_workspace workspace;

	if (unlikely(destination == NULL))
		return (OM_NO_SUCH_OBJECT);
	if (unlikely(!(destination->syntax & OM_S_PRIVATE)))
		return (OM_NOT_PRIVATE);
	workspace = (OM_workspace) ((destination - 1)->value.string.elements);
	if (unlikely(workspace->functions->omp_copy_value == NULL))
		return (OM_FUNCTION_DECLINED);
	return (*workspace->functions->omp_copy_value)
	    (source, source_type, source_value_position, destination, destination_type,
	     destination_value_position);
}

/** @fn OM_return_code om_copy_value(const OM_private_object source, const OM_type source_type, const OM_value_position source_value_position, OM_private_object destination, OM_type destination_type, OM_value_position destination_value_position)
  * @param source
  * @param source_type
  * @param source_value_position
  * @param destination
  * @param destination_type
  * @param destination_value_position
  * @version XOM_1.0
  * @par Alias:
  * This symbol is a strong alias of __xom_om_copy_value().
  */
__asm__(".symver __xom_om_copy_value,om_copy_value@@XDS_1.0");

#undef om_create

/** @brief
  * @param class
  * @param initialize
  * @param workspace
  * @param object
  * @version XOM_1.0
  * @par Alias:
  * This symbol is an implementation of om_create().
  */
OM_return_code
__xom_om_create(const OM_object_identifier class, const OM_boolean initialize,
		const OM_workspace workspace, OM_private_object * object)
{
	if (unlikely(workspace == NULL))
		return (OM_NO_SUCH_WORKSPACE);
	if (unlikely(workspace->functions->omp_create == NULL))
		return (OM_FUNCTION_DECLINED);
	return (*workspace->functions->omp_create)
	    (class, initialize, workspace, object);
}

/** @fn OM_return_code om_create(const OM_object_identifier class, const OM_boolean initialize, const OM_workspace workspace, OM_private_object * object)
  * @param class
  * @param initialize
  * @param workspace
  * @param object
  * @version XOM_1.0
  * @par Alias:
  * This symbol is a strong alias of __xom_om_create().
  */
__asm__(".symver __xom_om_create,om_create@@XDS_1.0");

#undef om_decode

/** @brief
  * @param encoding
  * @param original
  * @version XOM_1.0
  * @par Alias:
  * This symbol is an implementation of om_decode().
  */
OM_return_code
__xom_om_decode(const OM_private_object encoding, OM_private_object * original)
{
	OMP_workspace workspace;

	if (unlikely(original == NULL))
		return (OM_NO_SUCH_OBJECT);
	if (unlikely(!(original->syntax & OM_S_PRIVATE)))
		return (OM_NOT_PRIVATE);
	workspace = (OM_workspace) ((original - 1)->value.string.elements);
	if (unlikely(workspace->functions->omp_decode == NULL))
		return (OM_FUNCTION_DECLINED);
	return (*workspace->functions->omp_decode)
	    (encoding, original);
}

/** @fn OM_return_code om_decode(const OM_private_object encoding, OM_private_object * original)
  * @param encoding
  * @param original
  * @version XOM_1.0
  * @par Alias:
  * This symbol is a strong alias of __xom_om_decode().
  */
__asm__(".symver __xom_om_decode,om_decode@@XDS_1.0");

#undef om_delete

/** @brief
  * @param subject
  * @version XOM_1.0
  * @par Alias:
  * This symbol is an implementation of om_delete().
  */
OM_return_code
__xom_om_delete(OM_object subject)
{
	OMP_workspace workspace;

	if (unlikely(subject == NULL))
		return (OM_NO_SUCH_OBJECT);
	if (unlikely(!(subject->syntax & OM_S_SERVICE_GENERATED)))
		return (OM_NOT_THE_SERVICES);
	workspace = (OM_workspace) ((subject - 1)->value.string.elements);
	if (unlikely(workspace->functions->omp_delete == NULL))
		return (OM_FUNCTION_DECLINED);
	return (*workspace->functions->omp_delete)
	    (subject);
}

/** @fn OM_return_code om_delete(OM_object subject)
  * @param subject
  * @version XOM_1.0
  * @par Alias:
  * This symbol is a strong alias of __xom_om_delete().
  */
__asm__(".symver __xom_om_delete,om_delete@@XDS_1.0");

#undef om_encode

/** @brief
  * @param original
  * @param rules
  * @param encoding
  * @version XOM_1.0
  * @par Alias:
  * This symbol is an implementation of om_encode().
  */
OM_return_code
__xom_om_encode(const OM_private_object original, const OM_object_identifier rules,
		OM_private_object * encoding)
{
	OMP_workspace workspace;

	if (unlikely(original == NULL))
		return (OM_NO_SUCH_OBJECT);
	if (unlikely(!(original->syntax & OM_S_PRIVATE)))
		return (OM_NOT_PRIVATE);
	workspace = (OM_workspace) ((original - 1)->value.string.elements);
	if (unlikely(workspace->functions->omp_encode == NULL))
		return (OM_FUNCTION_DECLINED);
	return (*workspace->functions->omp_encode)
	    (original, rules, encoding);
}

/** @fn OM_return_code om_encode(const OM_private_object original, const OM_object_identifier rules, OM_private_object * encoding)
  * @param original
  * @param rules
  * @param encoding
  * @version XOM_1.0
  * @par Alias:
  * This symbol is a strong alias of __xom_om_encode().
  */
__asm__(".symver __xom_om_encode,om_encode@@XDS_1.0");

#undef om_get

/** @brief
  * @param original
  * @param exclusions
  * @param included_types
  * @param local_strings
  * @param initial_value
  * @param limiting_values
  * @param copy
  * @param total_number
  * @version XOM_1.0
  * @par Alias:
  * This symbol is an implementation of om_get().
  */
OM_return_code
__xom_om_get(const OM_private_object original, const OM_exclusions exclusions,
	     const OM_type_list included_types, const OM_boolean local_strings,
	     const OM_value_position initial_value, const OM_value_position limiting_value,
	     OM_public_object * copy, OM_value_position * total_number)
{
	OMP_workspace workspace;

	if (unlikely(original == NULL))
		return (OM_NO_SUCH_OBJECT);
	if (unlikely(!(original->syntax & OM_S_PRIVATE)))
		return (OM_NOT_PRIVATE);
	workspace = (OM_workspace) ((original - 1)->value.string.elements);
	if (unlikely(workspace->functions->omp_get == NULL))
		return (OM_FUNCTION_DECLINED);
	return (*workspace->functions->omp_get)
	    (original, exclusions, included_types, local_strings, initial_value, limiting_value,
	     copy, total_number);
}

/** @fn OM_return_code om_get(const OM_private_object original, const OM_exclusions exclusions, const OM_type_list included_types, const OM_boolean local_strings, const OM_value_position initial_value, const OM_value_position limiting_value, OM_public_object * copy, OM_value_position * total_number)
  * @param original
  * @param exclusions
  * @param included_types
  * @param local_strings
  * @param initial_value
  * @param limiting_values
  * @param copy
  * @param total_number
  * @version XOM_1.0
  * @par Alias:
  * This symbol is a strong alias of __xom_om_get().
  */
__asm__(".symver __xom_om_get,om_get@@XDS_1.0");

#undef om_instance

/** @brief
  * @param subject
  * @param class
  * @param instance
  * @version XOM_1.0
  * @par Alias:
  * This symbol is an implementation of om_instance().
  */
OM_return_code
__xom_om_instance(const OM_object subject, const OM_object_identifier class, OM_boolean * instance)
{
	OMP_workspace workspace;

	if (unlikely(subject == NULL))
		return (OM_NO_SUCH_OBJECT);
	if (unlikely(!(subject->syntax & OM_S_SERVICE_GENERATED)))
		return (OM_NOT_THE_SERVICES);
	workspace = (OM_workspace) ((subject - 1)->value.string.elements);
	if (unlikely(workspace->functions->omp_instance == NULL))
		return (OM_FUNCTION_DECLINED);
	return (*workspace->functions->omp_instance)
	    (subject, class, instance);
}

/** @fn OM_return_code om_instance(const OM_object subject, const OM_object_identifier class, OM_boolean * instance)
  * @param subject
  * @param class
  * @param instance
  * @version XOM_1.0
  * @par Alias:
  * This symbol is a strong alias of __xom_om_instance().
  */
__asm__(".symver __xom_om_instance,om_instance@@XDS_1.0");

#undef om_put

/** @brief
  * @param destination
  * @param modification
  * @param source
  * @param included_types
  * @param initial_value
  * @param limiting_value
  * @version XOM_1.0
  * @par Alias:
  * This symbol is an implementation of om_put().
  */
OM_return_code
__xom_om_put(OM_private_object destination, const OM_modification modification,
	     const OM_object source, const OM_type_list included_types,
	     const OM_value_position initial_value, const OM_value_position limiting_value)
{
	OMP_workspace workspace;

	if (unlikely(destination == NULL))
		return (OM_NO_SUCH_OBJECT);
	if (unlikely(!(destination->syntax & OM_S_PRIVATE)))
		return (OM_NOT_PRIVATE);
	workspace = (OM_workspace) ((destination - 1)->value.string.elements);
	if (unlikely(workspace->functions->omp_put == NULL))
		return (OM_FUNCTION_DECLINED);
	return (*workspace->functions->omp_put)
	    (destination, modification, source, included_types, initial_value, limiting_value);
}

/** @fn OM_return_code om_put(OM_private_object destination, const OM_modification modification, const OM_object source, const OM_type_list included_types, const OM_value_position initial_value, const OM_value_position limiting_value)
  * @param destination
  * @param modification
  * @param source
  * @param included_types
  * @param initial_value
  * @param limiting_value
  * @version XOM_1.0
  * @par Alias:
  * This symbol is a strong alias of __xom_om_put().
  */
__asm__(".symver __xom_om_put,om_put@@XDS_1.0");

#undef om_read

/** @brief
  * @param subject
  * @param type
  * @param value_position
  * @param local_string
  * @param string_offset
  * @param elements
  * @version XOM_1.0
  * @par Alias:
  * This symbol is an implementation of om_read().
  */
OM_return_code
__xom_om_read(const OM_private_object subject, const OM_type type,
	      const OM_value_position value_position, const OM_boolean local_string,
	      const OM_string_length * string_offset, OM_string * elements)
{
	if (unlikely(subject == NULL))
		return (OM_NO_SUCH_OBJECT);
	if (unlikely(!(subject->syntax & OM_S_PRIVATE)))
		return (OM_NOT_PRIVATE);
	if (unlikely(workspace->functions->omp_read == NULL))
		return (OM_FUNCTION_DECLINED);
	return (*OM_WORKSPACE(subject)->functions->omp_read)
	    (subject, type, value_position, local_string, string_offset, elements);
}

/** @fn OM_return_code om_read(const OM_private_object subject, const OM_type type, const OM_value_position value_position, const OM_boolean local_string, const OM_string_length * string_offset, OM_string * elements)
  * @param subject
  * @param type
  * @param value_position
  * @param local_string
  * @param string_offset
  * @param elements
  * @version XOM_1.0
  * @par Alias:
  * This symbol is a strong alias of __xom_om_read().
  */
__asm__(".symver __xom_om_read,om_read@@XDS_1.0");

#undef om_remove

/** @brief
  * @param subject
  * @param type
  * @param initial_value
  * @param limiting_value
  * @version XOM_1.0
  * @par Alias:
  * This symbol is an implementation of om_remove().
  */
OM_return_code
__xom_om_remove(const OM_private_object subject, const OM_type type,
		const OM_value_position initial_value, const OM_value_position limiting_value)
{
	OMP_workspace workspace;

	if (unlikely(subject == NULL))
		return (OM_NO_SUCH_OBJECT);
	if (unlikely(!(subject->syntax & OM_S_PRIVATE)))
		return (OM_NOT_PRIVATE);
	workspace = (OM_workspace) ((subject - 1)->value.string.elements);
	if (unlikely(workspace->functions->omp_remove == NULL))
		return (OM_FUNCTION_DECLINED);
	return (*workspace->functions->omp_remove)
	    (subject, type, initial_value, limiting_value);
}

/** @fn OM_return_code om_remove(const OM_private_object subject, const OM_type type, const OM_value_position initial_value, const OM_value_position limiting_value)
  * @param subject
  * @param type
  * @param initial_value
  * @param limiting_value
  * @version XOM_1.0
  * @par Alias:
  * This symbol is a strong alias of __xom_om_remove().
  */
__asm__(".symver __xom_om_remove,om_remove@@XDS_1.0");

#undef om_write

/** @brief
  * @param subject
  * @param type
  * @param value_position
  * @param syntax
  * @param string_offset
  * @param elements
  * @version XOM_1.0
  * @par Alias:
  * This symbol is an implementation of om_write().
  */
OM_return_code
__xom_om_write(const OM_private_object subject, const OM_type type,
	       const OM_value_position value_position, OM_syntax syntax,
	       const OM_string_length * string_offset, OM_string elements)
{
	OMP_workspace workspace;

	if (unlikely(subject == NULL))
		return (OM_NO_SUCH_OBJECT);
	if (unlikely(!(subject->syntax & OM_S_PRIVATE)))
		return (OM_NOT_PRIVATE);
	workspace = (OM_workspace) ((subject - 1)->value.string.elements);
	if (unlikely(workspace->functions->omp_write == NULL))
		return (OM_FUNCTION_DECLINED);
	return (*workspace->functions->omp_write)
	    (subject, type, value_position, syntax, string_offset, elements);
}

/** @fn OM_return_code om_write(const OM_private_object subject, const OM_type type, const OM_value_position value_position, OM_syntax syntax, const OM_string_length * string_offset, OM_string elements)
  * @param subject
  * @param type
  * @param value_position
  * @param syntax
  * @param string_offset
  * @param elements
  * @version XOM_1.0
  * @par Alias:
  * This symbol is a strong alias of __xom_om_write().
  */
__asm__(".symver __xom_om_write,om_write@@XDS_1.0");

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
