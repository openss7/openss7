//  ==========================================================================
//  
//  @(#) $Id: xom.hh,v 1.1.2.1 2009-06-21 11:23:46 brian Exp $
//  
//  --------------------------------------------------------------------------
//  
//  Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
//  Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
//  Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
//  
//  All Rights Reserved.
//  
//  This program is free software; you can redistribute it and/or modify it
//  under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation; version 3 of the License.
//  
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
//  License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>, or
//  write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
//  02139, USA.
//  
//  --------------------------------------------------------------------------
//  
//  U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
//  behalf of the U.S. Government ("Government"), the following provisions
//  apply to you.  If the Software is supplied by the Department of Defense
//  ("DoD"), it is classified as "Commercial Computer Software" under
//  paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
//  Regulations ("DFARS") (or any successor regulations) and the Government is
//  acquiring only the license rights granted herein (the license rights
//  customarily provided to non-Government users).  If the Software is
//  supplied to any unit or agency of the Government other than DoD, it is
//  classified as "Restricted Computer Software" and the Government's rights
//  in the Software are defined in paragraph 52.227-19 of the Federal
//  Acquisition Regulations ("FAR") (or any successor regulations) or, in the
//  cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
//  (or any successor regulations).
//  
//  --------------------------------------------------------------------------
//  
//  Commercial licensing and support of this software is available from
//  OpenSS7 Corporation at a fee.  See http://www.openss7.com/
//  
//  --------------------------------------------------------------------------
//  
//  Last Modified $Date: 2009-06-21 11:23:46 $ by $Author: brian $
//  
//  --------------------------------------------------------------------------
//  
//  $Log: xom.hh,v $
//  Revision 1.1.2.1  2009-06-21 11:23:46  brian
//  - added files to new distro
//
//  ==========================================================================

#ifndef __XOM_HH__
#define __XOM_HH__

#pragma interface

namespace xom {

typedef short OM_sint;
typedef short OM_sint16;
typedef int OM_sint32;
typedef unsigned short OM_uint;
typedef unsigned short OM_uint16;
typedef unsigned int OM_uint32;
typedef double OM_double;

typedef OM_uint32 OM_boolean;
typedef OM_uint32 OM_string_length;
typedef OM_uint OM_exclusions;
typedef OM_uint OM_modification;

typedef OM_descriptor_class *OM_object;

class OM_workspace_class;
typedef OM_workspace_class *OM_workspace;

typedef OM_object OM_private_object;
typedef OM_object OM_public_object;

typedef OM_uint OM_return_code;
typedef OM_uint16 OM_syntax;
typedef OM_uint16 OM_type;
typedef OM_type *OM_type_list;

typedef OM_uint32 OM_value_length;
typedef OM_uint32 OM_value_position;

static const bool OM_TRUE = true;
static const bool OM_FALSE = false;

class OM_descriptor_class {
	public:
		OM_type type;
		OM_syntax syntax;

		virtual bool om_has_value() {
			return (syntax & OM_S_NO_VALUE);
		};
		virtual bool om_is_private() {
			return (syntax & OM_S_PRIVATE);
		};
		virtual bool om_is_service_generated() {
			return (syntax & OM_S_SERVICE_GENERATED);
		};
		virtual bool om_is_local_string() {
			return (syntax & OM_S_LOCAL_STRING);
		};
		virtual bool om_is_long_string() {
			return (syntax & OM_S_LONG_STRING);
		};
		virtual void om_set_local_string() {
			syntax |= OM_S_LOCAL_STRING;
		};
		virtual OM_descriptor_class& om_copy(const OM_decriptor_class& orig, const OM_workspace_class& workspace);
		virtual void om_copy_value(
				const OM_descriptor_class& source,
				const OM_type source_type,
				const OM_value_position source_value_position);
};

class OM_value_class : public OM_descriptor_class {
};

class OM_string : public OM_value_class {
	private:
		OM_string_length length;
		char *elements;
};

class OM_object_identifier : public OM_value_class {
	private:
		OM_string_length length;
		unsigned char *elements;
};

class OM_enumeration : public OM_value_class {
	private:
		OM_sint32 value;
};

class OM_integer : public OM_value_class {
	private:
		OM_sint32 value;
};

class OM_padded_object : public OM_value_class {
	public:
		OM_uint32 padding;
		OM_object object;
		OM_padded_object(OM_object obj) : padding(0), object(obj) {
		};
};

class OM_real : public OM_value_class {
	private:
		OM_double value;
};


#define OMP_OBJECT(oid_string) OMP_O_ ## oid_string
#define OMP_STRING(oid_string) OMP_D_ ## oid_string
#define OM_LENGTH(oid_string) (sizeof(OMP_OBJECT(oid_string))-1)

#define OM_IMPORT(class_name) \
	extern char OMP_STRING(class_name)[]; \
	extern OM_object_identifier class_name;

#define OM_EXPORT(class_name) \
	char OMP_STRING(class_name)[] = OMP_OBJECT(class_name); \
	OM_object_identifier class_name(OMP_LENGTH(class_name), OMP_D_ ## class_name);

// Constant for the OM package
#define OMP_O_OM_OM		"\x2A\x86\x48\xCE\x21\x00"
// Constant for the Encoding class
#define OMP_O_OM_C_ENCODING	"\x2A\x86\x48\xCE\x21\x00\x01"
// Constant for the External class
#define OMP_O_OM_C_EXTERNAL	"\x2A\x86\x48\xCE\x21\x00\x02"
// Constant for the Object class
#define OMP_O_OM_C_OBJECT	"\x2A\x86\x48\xCE\x21\x00\x03"
// Constant for the BER Object Identifier
#define OMP_O_OM_BER		"\x51\01"
// Constant for the Canonical-BER Object Identifier
#define OMP_O_OM_CANONICAL_BER	"\x2A\x86\x48\xCE\x21\x00\x04"

}; // namespace xom

enum OMP_return_code {
	OM_SUCCESS = 0,
	OM_ENCODING_INVALID,
	OM_FUNCTION_DECLINED,
	OM_FUNCTION_INTERRUPTED,
	OM_MEMORY_INSUFFICIENT,
	OM_NETWORK_ERROR,
	OM_NO_SUCH_CLASS,
	OM_NO_SUCH_EXCLUSION,
	OM_NO_SUCH_MODIFICATION,
	OM_NO_SUCH_OBJECT,
	OM_NO_SUCH_RULES,
	OM_NO_SUCH_SYNTAX,
	OM_NO_SUCH_TYPE,
	OM_NO_SUCH_WORKSPACE,
	OM_NOT_AN_ENCODING,
	OM_NOT_CONCRETE,
	OM_NOT_PRESENT,
	OM_NOT_PRIVATE,
	OM_NOT_THE_SERVICES,
	OM_PERMANENT_ERROR,
	OM_POINTER_INVALID,
	OM_SYSTEM_ERROR,
	OM_TEMPORARY_ERROR,
	OM_TOO_MANY_VALUES,
	OM_WRONG_VALUE_LENGTH,
	OM_WRONG_VALUE_MAKEUP,
	OM_WRONG_VALUE_NUMBER,
	OM_WRONG_VALUE_POSITION,
	OM_WRONG_VALUE_SYNTAX,
	OM_WRONG_VALUE_TYPE,
};

enum OM_type_enum {
	OM_NO_MORE_TYPES = 0,
	OM_ARBITRARY_ENCODING,
	OM_ASN1_ENCODING,
	OM_CLASS,
	OM_DATA_VALUE_DESCRIPTOR,
	OM_DIRECT_REFERENCE,
	OM_INDIRECT_REFERENCE,
	OM_OBJECT_CLASS,
	OM_OBJECT_ENCODING,
	OM_OCTET_ALIGNED_ENCODING,
	OM_PRIVATE_OBJECT,
	OM_RULES,
};

#endif				/* __XOM_HH__ */

