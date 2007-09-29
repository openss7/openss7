/*****************************************************************************

 @(#) $Id: xom.h,v 0.9.2.1 2007/09/29 14:08:34 brian Exp $

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

 $Log: xom.h,v $
 Revision 0.9.2.1  2007/09/29 14:08:34  brian
 - added new files

 *****************************************************************************/

#ifndef __XOM_H__
#define __XOM_H__

#ident "@(#) $RCSfile: xom.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* BEGIN SERVICE INTERFACE */

/* INTERMEDIATE DATA TYPES */

typedef int16_t OM_sint;
typedef int16_t OM_sint16;
typedef int32_t OM_sint32;
typedef uint16_t OM_uint;
typedef uint16_t OM_uint16;
typedef uint32_t OM_uint32;
typedef double OM_double;

/* PRIMATE DATA TYPES */

/* Boolean */
typedef OM_uint32 OM_boolean;

/* String Length */
typedef OM_uint32 OM_string_length;

/* Enumeration */
typedef OM_sint32 OM_enumeration;

/* Exclusions */
typedef OM_uint OM_exclusions;

/* Integer */
typedef OM_sint32 OM_integer;

/* Real */
typedef OM_double OM_real;

/* Modification */
typedef OM_uint OM_modification;

/* Object */
struct OM_descriptor_struct;
typedef struct OM_descriptor_struct *OM_object;

/* String */
typedef struct {
	OM_string_length length;
	void *elements;
} OM_string;

#define OM_STRING(string)	{ (OM_string_length)(sizeof(string)-1), (string) }

/* Workspace */
typedef void *OM_workspace;

/* SECONDARY DATA TYPES */

/* Object Identifier */
typedef OM_string OM_object_identifier;

/* Private Object */
typedef OM_object OM_private_object;

/* Public Object */
typedef OM_object OM_public_object;

/* Return Code */
typedef OM_uint OM_return_code;

/* Syntax */
typedef OM_uint16 OM_syntax;

/* Type */
typedef OM_uint16 OM_type;

/*  List */
typedef OM_type *OM_type_list;

/* Value */
typedef struct {
	OM_uint32 padding;
	OM_object object;
} OM_padded_object;

typedef union OM_value_union {
	OM_string string;
	OM_boolean boolean;
	OM_enumeration enumeration;
	OM_integer integer;
	OM_padded_object object;
	OM_real real;
} OM_value;

/* macro to extract an object from a value */
#define OM_OBJ_VALUE(v)			((OM_object)((v).string.elements))

/* Macro to extract syntax from a descriptor */
#define OM_SYNTAX(d)		((d).syntax & OM_S_SYNTAX)

/* Macro to assign a syntax to a descriptor */
#define OM_SYNTAX_ASSIGN(d,s)	((d).syntax = (s) | ((d).syntax & ~OM_S_SYNTAX))

/* Macro to extract additional information contained in the syntax */
#define OM_HAS_VALUE(d)			(((d).syntax & OM_S_NO_VALUE) == 0)
#define OM_IS_PRIVATE(d)		(((d).syntax & OM_S_PRIVATE) != 0)
#define OM_IS_SERVICE_GENERATED(d)	(((d).syntax & OM_S_SERVICE_GENERATED) != 0)
#define OM_IS_LOCAL_STRING(d)		(((d).syntax & OM_S_LOCAL_STRING) != 0)
#define OM_IS_LONG_STRING(d)		(((d).syntax & OM_S_LONG_STRING) != 0)

/* Macro to set additional information contained in the syntax */
#define OM_SET_LOCAL_STRING(d)		((d).syntax |= OM_S_LOCAL_STRING)

/* Value Length */
typedef OM_uint32 OM_value_length;

/* Value Position */
typedef OM_uint32 OM_value_position;

/* TERTIARY DATA TYPES */

/* Descriptor */
typedef struct OM_descriptor_struct {
	OM_type type;
	OM_syntax syntax;
	union OM_value value;
} OM_descriptor;

/* SYMBOLIC CONSTANTS */

/* Boolean */
#define OM_FALSE	((OM_boolean)0)
#define OM_TRUE		((OM_boolean)1)

/* Element Position */
#define OM_LENGTH_UNSPECIFIED	((OM_string_length)(0xFFFFFFFF))

/* Exclusions */
#define OM_NO_EXCLUSIONS		((OM_exclusions)(0))
#define OM_EXCLUDE_ALL_BUT_THESE_TYPES	((OM_exclusions)(1<<0))
#define OM_EXCLUDE_ALL_BUT_THESE_VALUES	((OM_exclusions)(1<<2))
#define OM_EXCLUDE_MULTIPLES		((OM_exclusions)(1<<1))
#define OM_EXCLUDE_SUBOBJECTS		((OM_exclusions)(1<<4))
#define OM_EXCLUDE_VALUES		((OM_exclusions)(1<<3))
#define OM_EXCLUDE_DESCRIPTORS		((OM_exclusions)(1<<5))

/* Modification */
#define OM_INSERT_AT_BEGINNING		((OM_modification)1)
#define OM_INSERT_AT_CERTAIN_POINT	((OM_modification)2)
#define OM_INSERT_AT_END		((OM_modification)3)
#define OM_REPLACE_ALL			((OM_modification)4)
#define OM_REPLACE_CERTAIN_VALUES	((OM_modification)5)

/* Object Identifiers */

/* Note:
 * These macros rely on the ## token-pasting operator of ANSI C.  On many
 * pre-ANSI compilers the same effect can be obtained by replacing ## with /*.
 */

/* Private macro to calculate length of an object identifier. */
#define OMP_LENGTH(oid_string) (sizeof(OMP_O_ ## oid_string)-1)

/* Macro to intialize the syntax and valud of an object identifier */
#define OM_OID_DESC(type, oid_name) \
	{ \
		(type), OM_S_OBJECT_IDENTIFIER_STRING, \
		{ \
			{ \
				OMP_LENGTH(oid_name), OMP_D_ ## oid_name \
			} \
		} \
	}

/* Macro to mark the end of a public object. */
#define OM_NULL_DESCRIPTOR \
	{ \
		OM_NO_MORE_TYPES, OM_S_NO_MORE_SYNTAXES, \
		{ \
			0, OM_ELEMENTS_UNSPECIFIED \
		} \
	}

/* Macro to make class constants available within a compilation unit */
#define OM_IMPORT(class_name) \
	extern char OMP_D_ ## class_name []; \
	extern OM_string class_name;

/* Macro to allocate memory for class constants within a compilation unit */
#define OM_EXPORT(class_name) \
	char OMP_D_ ## class_name[] = OMP_O_ ## class_name; \
	OM_string class_name = { OMP_LENGTH(class_name), OMD_D_ ## class_name };

/* Constant for the OM package */
#define OMP_O_OM_OM		"\x2A\x86\x48\xCE\x21\x00"

/* Constant for the Encoding class */
#define OMP_O_OM_C_ENCODING	"\x2A\x86\x48\xCE\x21\x00\x01"

/* Constant for the External class */
#define OMP_O_OM_C_EXTERNAL	"\x2A\x86\x48\xCE\x21\x00\x02"

/* Constant for the Object class */
#define OMP_O_OM_C_EXTERNAL	"\x2A\x86\x48\xCE\x21\x00\x03"

/* Constant for the BER Object Identifier */
#define OMP_O_OM_BER		"\x51\01"

/* Constant for the Canonical-BER Object Identifier */
#define OMP_O_OM_CANONICAL_BER	"\x2A\x86\x48\xCE\x21\x00\x04"

/* Return Code */
typedef enum OMP_return_code_enum {
	OM_SUCCESS = 0,
#define OM_SUCCESS			((OM_return_code)OM_SUCCESS)
	OM_ENCODING_INVALID,
#define OM_ENCODING_INVALID		((OM_return_code)OM_ENCODING_INVALID)
	OM_FUNCTION_DECLINED,
#define OM_FUNCTION_DECLINED		((OM_return_code)OM_FUNCTION_DECLINED)
	OM_FUNCTION_INTERRUPTED,
#define OM_FUNCTION_INTERRUPTED		((OM_return_code)OM_FUNCTION_INTERRUPTED)
	OM_MEMORY_INSUFFICIENT,
#define OM_MEMORY_INSUFFICIENT		((OM_return_code)OM_MEMORY_INSUFFICIENT)
	OM_NETWORK_ERROR,
#define OM_NETWORK_ERROR		((OM_return_code)OM_NETWORK_ERROR)
	OM_NO_SUCH_CLASS,
#define OM_NO_SUCH_CLASS		((OM_return_code)OM_NO_SUCH_CLASS)
	OM_NO_SUCH_EXCLUSION,
#define OM_NO_SUCH_EXCLUSION		((OM_return_code)OM_NO_SUCH_EXCLUSION)
	OM_NO_SUCH_MODIFICATION,
#define OM_NO_SUCH_MODIFICATION		((OM_return_code)OM_NO_SUCH_MODIFICATION)
	OM_NO_SUCH_OBJECT,
#define OM_NO_SUCH_OBJECT		((OM_return_code)OM_NO_SUCH_OBJECT)
	OM_NO_SUCH_RULES,
#define OM_NO_SUCH_RULES		((OM_return_code)OM_NO_SUCH_RULES)
	OM_NO_SUCH_SYNTAX,
#define OM_NO_SUCH_SYNTAX		((OM_return_code)OM_NO_SUCH_SYNTAX)
	OM_NO_SUCH_TYPE,
#define OM_NO_SUCH_TYPE			((OM_return_code)OM_NO_SUCH_TYPE)
	OM_NO_SUCH_WORKSPACE,
#define OM_NO_SUCH_WORKSPACE		((OM_return_code)OM_NO_SUCH_WORKSPACE)
	OM_NOT_AN_ENCODING,
#define OM_NOT_AN_ENCODING		((OM_return_code)OM_NOT_AN_ENCODING)
	OM_NOT_CONCRETE,
#define OM_NOT_CONCRETE			((OM_return_code)OM_NOT_CONCRETE)
	OM_NOT_PRESENT,
#define OM_NOT_PRESENT			((OM_return_code)OM_NOT_PRESENT)
	OM_NOT_PRIVATE,
#define OM_NOT_PRIVATE			((OM_return_code)OM_NOT_PRIVATE)
	OM_NOT_THE_SERVICES,
#define OM_NOT_THE_SERVICES		((OM_return_code)OM_NOT_THE_SERVICES)
	OM_PERMANENT_ERROR,
#define OM_PERMANENT_ERROR		((OM_return_code)OM_PERMANENT_ERROR)
	OM_POINTER_INVALID,
#define OM_POINTER_INVALID		((OM_return_code)OM_POINTER_INVALID)
	RC_SYSTEM_ERROR,
#define OM_SYSTEM_ERROR			((OM_return_code)OM_SYSTEM_ERROR)
	OM_TEMPORARY_ERROR,
#define OM_TEMPORARY_ERROR		((OM_return_code)OM_TEMPORARY_ERROR)
	OM_TOO_MANY_VALUES,
#define OM_TOO_MANY_VALUES		((OM_return_code)OM_TOO_MANY_VALUES)
	OM_WRONG_VALUE_LENGTH,
#define OM_WRONG_VALUE_LENGTH		((OM_return_code)OM_WRONG_VALUE_LENGTH)
	OM_WRONG_VALUE_MAKEUP,
#define OM_WRONG_VALUE_MAKEUP		((OM_return_code)OM_WRONG_VALUE_MAKEUP)
	OM_WRONG_VALUE_NUMBER,
#define OM_WRONG_VALUE_NUMBER		((OM_return_code)OM_WRONG_VALUE_NUMBER)
	OM_WRONG_VALUE_POSITION,
#define OM_WRONG_VALUE_POSITION		((OM_return_code)OM_WRONG_VALUE_POSITION)
	OM_WRONG_VALUE_SYNTAX,
#define OM_WRONG_VALUE_SYNTAX		((OM_return_code)OM_WRONG_VALUE_SYNTAX)
	OM_WRONG_VALUE_TYPE,
#define OM_WRONG_VALUE_TYPE		((OM_return_code)OM_WRONG_VALUE_TYPE)
} OMP_return_code;

/* String (Elements component) */
#define OM_ELEMENTS_UNSPECIFIED		((void *)0)

/* Syntax */
#define OM_S_NO_MORE_SYNTAXES		((OM_syntax)  0)
#define OM_S_BIT_STRING			((OM_syntax)  3)
#define OM_S_BOOLEAN			((OM_syntax)  1)
#define OM_S_ENCODING_STRING		((OM_syntax)  8)
#define OM_S_ENUMERATION		((OM_syntax) 10)
#define OM_S_GENERAL_STRING		((OM_syntax) 27)
#define OM_S_GENERALISED_TIME_STRING	((OM_syntax) 24)
#define OM_S_GRAPHIC_STRING		((OM_syntax) 25)
#define OM_S_IA5_STRING			((OM_syntax) 22)
#define OM_S_INTEGER			((OM_syntax)  2)
#define OM_S_NULL			((OM_syntax)  5)
#define OM_S_NUMERIC_STRING		((OM_syntax) 18)
#define OM_S_OBJECT			((OM_syntax)127)
#define OM_S_OBJECT_DESCRIPTOR_STRING	((OM_syntax)  7)
#define OM_S_OBJECT_IDENTIFIER_STRING	((OM_syntax)  6)
#define OM_S_OCTET_STRING		((OM_syntax)  4)
#define OM_S_PRINTABLE_STRING		((OM_syntax) 19)
#define OM_S_REAL			((OM_syntax)  9)
#define OM_S_TELETEX_STRING		((OM_syntax) 20)
#define OM_S_UNIVERSAL_STRING		((OM_syntax) 28)
#define OM_S_UNRESTRICTED_STRING	((OM_syntax) 29)
#define OM_S_UTC_TIME_STRING		((OM_syntax) 23)
#define OM_S_VIDEOTEX_STRING		((OM_syntax) 21)
#define OM_S_VISIBLE_STRING		((OM_syntax) 26)

#define OM_S_LONG_STRING		((OM_syntax)0x8000)
#define OM_S_NO_VALUE			((OM_syntax)0x4000)
#define OM_S_LOCAL_STRING		((OM_syntax)0x2000)
#define OM_S_SERVICE_GENERATED		((OM_syntax)0x1000)
#define OM_S_PRIVATE			((OM_syntax)0x0800)
#define OM_S_SYNTAX			((OM_syntax)0x03FF)

/* Type */
#define OM_NO_MORE_TYPES		((OM_type) 0)
#define OM_ARBITRARY_ENCODING		((OM_type) 1)
#define OM_ASN1_ENCODING		((OM_type) 2)
#define OM_CLASS			((OM_type) 3)
#define OM_DATA_VALUE_DESCRIPTOR	((OM_type) 4)
#define OM_DIRECT_REFERENCE		((OM_type) 5)
#define OM_INDIRECT_REFERENCE		((OM_type) 6)
#define OM_OBJECT_CLASS			((OM_type) 7)
#define OM_OBJECT_ENCODING		((OM_type) 8)
#define OM_OCTET_ALIGNED_ENCODING	((OM_type) 9)
#define OM_PRIVATE_OBJECT		((OM_type)10)
#define OM_RULES			((OM_type)11)

/* Value Position */
#define OM_ALL_VALUES			((OM_value_position)0xFFFFFFFF)

/* WORKSPACE INTERFACE */
#include <xomi.h>

/* END SERVICE INTERFACE */

OM_return_code om_copy(const OM_private_object original, const OM_workspace workspace,
		       OM_private_object *copy);

#define om_copy(o,w,c) om_copy(o,w,c)

OM_return_code om_copy_value(const OM_private_object source, const OM_type source_type,
			     const OM_value_position source_value_position,
			     OM_private_object destination, OM_type destination_type,
			     OM_value_position destination_value_position);

#define om_copy_value(s,st,svp,d,dt,svp) om_copy_value(s,st,svp,d,dt,dvp)

OM_return_code om_create(const OM_object_identifier class, const OM_boolean initialize,
			 const OM_workspace workspace, OM_private_object *object);

#define om_create(c,i,w,o) om_create(c,i,w,o)

OM_return_code om_decode(const OM_private_object encoding, OM_private_object *original);

#define om_decode(e,o) om_decode(e,o)

OM_return_code om_delete(OM_object object);

#define om_delete(o) om_delete(o)

OM_return_code om_encode(const OM_private_object original, const OM_object_identifier rules,
			 OM_private_object *encoding);

#define om_encode(o,r,e) om_encode(o,r,e)

OM_return_code om_get(const OM_private_object original, const OM_exclusions exclusions,
		      const OM_type_list included_types, const OM_boolean local_strings,
		      const OM_value_position initial_value, const OM_value_position limiting_value,
		      OM_public_object *copy, OM_value_position *total_number);

#define om_get(o,e,i,l,iv,lv,c,t) om_get(o,e,i,l,iv,lv,c,t)

OM_return_code om_instance(const OM_object subject, const OM_object_identifier class,
			   OM_boolean *instance);

#define om_instance(s,c,i) om_instance(s,c,i)

OM_return_code om_put(OM_private_object destination, const OM_modification modification,
		      const OM_object source, const OM_type_list included_types,
		      const OM_value_position initial_value,
		      const OM_value_position limiting_value);

OM_return_code om_read(const OM_private_object subject, const OM_type type,
		       const OM_value_position value_position, const OM_boolean local_string,
		       const OM_string_length *string_offset, OM_string *elements);

OM_return_code om_remove(const OM_private_object subject, const OM_type type,
			 const OM_value_position initial_value,
			 const OM_value_position limiting_value);
/* NOTE I don't think that first argument should be const */

/**
 * This function writes a segment of an attribute value in a private object,
 * the subject.  The segment that is supplied is a segment of the string value
 * that would have been supplied if the complete value had been written in a
 * single call.
 *
 * The written segment is made the value's last; the function discards any
 * values whose offset equals or exceeds the @value_position argument. If the
 * value being written is in the local representation, it is converted to the
 * non-local representation (which may entail the loss of information and
 * which may yield a different number of elements than that provided).
 *
 * Note: This function enables the client to write an arbitrarily long value
 * without having to place a copy of the entire value in memory.
 */
OM_return_code om_write(const OM_private_object subject, const OM_type type,
			const OM_value_position value_position, OM_syntax syntax,
			const OM_string_length *string_offet, OM_string elements);

const char *omp_strerrors[] = {
	"The function completed successfully.",
	"The octets that constitute the value of an encoding's Object Encoding attribute are invalid.",
	"The function does not apply to the object to which it is addressed.",
	"The function was abored by an external force.",
	"The service cannot allocate main memory it needs to complete the function.",
	"The service could not successfully emply the network upon which its implementation depends.",
	"A purported class identifier is undefined.",
	"A purported exclusion identifier is undefined.",
	"A purported modification identifier is undefined.",
	"A purported object is nonexistent or the purported handle is invalid.",
	"A purported rules identifier is undefined.",
	"A purported syntax identifier is undefined.",
	"A purported type identifier is undefined.",
	"A purported workspace is nonexistent.",
	"An object is not an instance of the Encoding class.",
	"A class is abstract, not concrete.",
	"An attribute value is absent, not present.",
	"An object is public, not private.",
	"An object is client-generated, rather than service-generated or private.",
	"The service encountered a permanent difficulty other than those denoted by other return codes.",
	"In the C interface, an invalid point was supplied as a function argument or as the receptacle for a function result.",
	"The service could not successfully emply the operating system upon which its implementation depends.",
	"The service encountered a temporary difficulty other than those denoed by other return codes.",
	"An implementation limit prevents the addition to an object of another attribute value.  This limit is undefined.",
	"The descritpors for the values of a particular attribute are not adjacent.",
	"An attribute has, or would have, a value that violates the value length constraints in force.",
	"An attribute has, or would have, a value that violates a cosntraint of the value's syntax.",
	"An attribute has, or would have, a value the violates the value number constraints in force.",
	"The usage of value position(s) identified in the argument(s) of a function is invalid.",
	"An attribute has, or would have, a value whose syntax is not permitted.",
	"An object has, or would have, an attribute whose type is not permitted."
};

:w
#endif				/* __XOM_H__ */
