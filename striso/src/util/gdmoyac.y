/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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

/* C DECLARATIONS */

%{

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <getopt.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <sys/utsname.h>

#include "gdmoyac.h"

#if !defined YYENABLE_NLS
#define YYENABLE_NLS 0
#endif

int yydebug = 0;
int yyerror(const char *);

extern int yy_flex_debug;
extern int yylex(void);
extern int yylineno;
extern char *yytext;
extern void yyrestart(FILE *);

extern int yyleng;
extern FILE *yyin, *yyout;

char filename[256] = "";
char outpfile[256] = "";

int gdmo_level = 0;
int gdmo_column = 0;
int gdmo_incomment = 0;

void gdmo_checkcomment(FILE *);
void gdmo_newline(FILE *);
void gdmo_fprintwrap(FILE *, const char *, int, int);

char *currentfile = NULL;

%}

/* BISON DECLARATIONS */

%debug 
%defines
%locations
%token_table
%verbose

%union {
    int val;
    char *str;
}

/* TOKEN TABLE */


%token TOK_MANAGED_OBJECT_CLASS_LABEL
%token TOK_PACKAGE_LABEL
%token TOK_PARAMETER_LABEL
%token TOK_NAME_BINDING_LABEL
%token TOK_ATTRIBUTE_LABEL
%token TOK_ATTRIBUTE_GROUP_LABEL
%token TOK_BEHAVIOUR_LABEL
%token TOK_ACTION_LABEL
%token TOK_NOTIFICATION_LABEL

%token TOK_MANAGED_OBJECT_CLASS
%token TOK_PACKAGE
%token TOK_PARAMETER
%token TOK_NAME_BINDING
%token TOK_ATTRIBUTE
%token TOK_ATTRIBUTE_GROUP
%token TOK_BEHAVIOUR
%token TOK_ACTION
%token TOK_NOTIFICATION

%token TOK_ACTION_INFO
%token TOK_ACTION_REPLY
%token TOK_ACTIONS
%token TOK_ADD_REMOVE
%token TOK_ADD
%token TOK_AND_ATTRIBUTE_IDS
%token TOK_AND_SUBCLASSES
%token TOK_ATTRIBUTE_GROUPS
%token TOK_ATTRIBUTES
%token TOK_CHARACTERIZED_BY
%token TOK_CONDITIONAL_PACKAGES
%token TOK_CONTEXT
%token TOK_CREATE
%token TOK_DEFAULT_VALUE
%token TOK_DEFINED_AS
%token TOK_DELETES_CONTAINED_OBJECTS
%token TOK_DELETE
%token TOK_DERIVATION_RULE
%token TOK_DERIVED_FROM
%token TOK_DESCRIPTION
%token TOK_EQUALITY
%token TOK_EVENT_INFO
%token TOK_EVENT_REPLY
%token TOK_FIXED
%token TOK_GET_REPLACE
%token TOK_GET
%token TOK_GROUP_ELEMENTS
%token TOK_INITIAL_VALUE
%token TOK_MATCHES_FOR
%token TOK_MODE_CONFIRMED
%token TOK_NAMED_BY
%token TOK_SUPERIOR_OBJECT_CLASS
%token TOK_NO_MODIFY
%token TOK_NOTIFICATIONS
%token TOK_ONLY_IF_NO_CONTAINED_OBJECTS
%token TOK_ORDERING
%token TOK_PARAMETERS
%token TOK_PERMITTED_VALUES
%token TOK_PRESENT_IF
%token TOK_PRESENT_ONLY_IF  /* X.722 Corrigendum 1. */
%token TOK_REGISTERED_AS
%token TOK_REMOVE
%token TOK_REPLACE_WITH_DEFAULT
%token TOK_REPLACE
%token TOK_REQUIRED_VALUES
%token TOK_SET_BY_CREATE
%token TOK_SET_COMPARISON
%token TOK_SET_INTERSECTION
%token TOK_SPECIFIC_ERROR
%token TOK_SUBORDINATE
%token TOK_SUBSTRINGS
%token TOK_WITH_ATTRIBUTE_SYNTAX
%token TOK_WITH_ATTRIBUTE
%token TOK_WITH_AUTOMATIC_INSTANCE_NAMING
%token TOK_WITH_INFORMATION_SYNTAX
%token TOK_WITH_REFERENCE_OBJECT
%token TOK_WITH_REPLY_SYNTAX
%token TOK_WITH_SYNTAX
%token TOK_DELIMITED_STRING
%token TOK_LABEL
%token TOK_NAME_AND_NUMBER
%token TOK_ID
%token TOK_EXTERNREF
%token TOK_SEMICOLON
%token TOK_COMMA
%token TOK_COLON
%token TOK_ELIPSIS
%token TOK_PERIOD
%token TOK_DOUBLEQUOTE
%left TOK_LBRACE
%right TOK_RBRACE
%left TOK_LPAREN
%right TOK_RPAREN
%left TOK_LANGLE
%right TOK_RANGLE
%token TOK_XMLOPEN
%token TOK_XMLCLOSE
%token TOK_XMLCOPEN
%token TOK_LDBRACKET
%token TOK_RDBRACKET
%token TOK_LBRACKET
%token TOK_RBRACKET
%token TOK_HYPHEN
%token TOK_EQUALS
%token TOK_APOSTROPHE
%token TOK_AT
%token TOK_BAR
%token TOK_EXCLAMATION
%token TOK_CARET
%token TOK_ASTERISK
%token TOK_AMPERSAND
%token TOK_SLASH
%token TOK_NUMBER
%token TOK_REALNUMBER
%token TOK_DOCID
%token TOK_COMSTRING
%token TOK_TEXT_STRING

%token TOK_ABSTRACT_SYNTAX
%token TOK_ALL
%token TOK_AUTOMATIC
%token TOK_BMPString
%token TOK_CHARACTER
%token TOK_CLASS
%token TOK_CONSTRAINED
%token TOK_CONTAINING
%token TOK_EMBEDDED
%token TOK_ENCODED
%token TOK_EXCEPT
%token TOK_EXTENSIBILITY
%token TOK_GeneralizedTime
%token TOK_GeneralString
%token TOK_GraphicString
%token TOK_IA5String
%token TOK_INSTANCE
%token TOK_INTERSECTION
%token TOK_IMPLIED
%token TOK_ISO646String
%token TOK_NumericString
%token TOK_ObjectDescriptor
%token TOK_PrintableString
%token TOK_PATTERN
%token TOK_PDV
%token TOK_RELATIVE_OID
%token TOK_SYNTAX
%token TOK_T61String
%token TOK_TeletexString
%token TOK_TYPE_IDENTIFIER
%token TOK_UNION
%token TOK_UNIQUE
%token TOK_UniversalString
%token TOK_UTCTime
%token TOK_UTF8String
%token TOK_VideotexString
%token TOK_VisibleString

%token TOK_BOOLEAN
%token TOK_INTEGER
%token TOK_BIT
%token TOK_STRING
%token TOK_OCTET
%token TOK_NULL
%token TOK_SEQUENCE
%token TOK_OF
%token TOK_SET
%token TOK_IMPLICIT
%token TOK_CHOICE
%token TOK_ANY
%token TOK_EXTERNAL
%token TOK_OBJECT
%token TOK_IDENTIFIER
%token TOK_OPTIONAL
%token TOK_DEFAULT
%token TOK_COMPONENTS
%token TOK_UNIVERSAL
%token TOK_APPLICATION
%token TOK_PRIVATE
%token TOK_TRUE
%token TOK_FALSE
%token TOK_BEGIN
%token TOK_END
%token TOK_DEFINITIONS
%token TOK_EXPLICIT
%token TOK_ENUMERATED
%token TOK_EXPORTS
%token TOK_IMPORTS
%token TOK_REAL
%token TOK_INCLUDES
%token TOK_MIN
%token TOK_MAX
%token TOK_SIZE
%token TOK_FROM
%token TOK_WITH
%token TOK_COMPONENT
%token TOK_PRESENT
%token TOK_ABSENT
%token TOK_DEFINED
%token TOK_BY
%token TOK_PLUS_INFINITY
%token TOK_MINUS_INFINITY
%token TOK_TAGS
%token TOK_ASSIGNMENT

%token TOK_true
%token TOK_false


/* GRAMAR RULES */

%%

file:
	gdmo { gdmo_newline(yyout); } /* asn1 */
	;


/*
 * The template definitions are based on the following sytactic convention:
 *
 * a) all terminal symbols and keywords that form part of a template definition are case-sensitive,
 *
 * b) whwere necessary [in order] to render the syntax of a template unambiguous, each element of a
 *    template shall be separated from adjacent elements by one or more delimiters.  Valid
 *    delimiters are the space character, the end of line, a blank line or a comment.  One ore more
 *    delimiteres shall appear between
 *
 *    1) a template-lable and a TEMPLATE-NAME
 *    2) a TEMPLATE-NAME and a CONSTRUCT-NAME
 *    3) a CONSTRUCT-NAME and a construct-argument
 *
 *    One or more delimiters may be introduced between any other pair of eleemnts within a template,
 *    and where a constructor-argument consists of a number of distinct elements, delimiters may be
 *    introduced between them.  Delimiters shall not otherwise be introduced within elements of the
 *    template unless the definition of the template explicitly permits thier introductions;
 *
 * c) spaces, blank lines, commetns and the end of line are significant only as delimiters;
 *
 * d) a comment is introduced by the character pair
 *
 *    --
 *
 *    and is terminated either by the character pair
 *
 *    --
 *
 *    or by the end of a line, whichever occurs first.  A comment is equivalent to a space for the
 *    purposes of interpreting the templates defined in this Recommendation | International Stanard.
 *    Comments have no normative significance.
 *
 * e) the character
 *
 *    ;
 *
 *    shall be used to mark the end of each construct within a template (except REGISTERED AS and
 *    DEFINED AS) and to mark the end of a template;
 *
 * f) the notation used for representing object identifiers shall be the value notation defined in
 *    CCITT Rec. X.208 ISO/IEC 8824 for representing object identifier values; i.e., the production
 *
 *	object-identifier -> <ObjectIdentifierValue>
 *
 *    is assumed to be supporting production for all template definitions in this document, where
 *    ObjectIdentifierValue refers to the corresponding notation define in CCITT Rec. X.208 |
 *    ISO/IEC 8824;
 *
 * g) strings surrounded by
 *
 *	[ ]
 *
 *    delimit parts of a template definition that may either be present or absent in each instance
 *    of use of the template.  If the closing brace is followed by an asterisk, i.e.,
 *
 *      [ ]*
 *
 *    the contents of the braces may appear zero or more times.  The circumstances under which these
 *    parts of the definition may be omitted or repeated are dependent upon the definition of the
 *    template type; 
 *
 * h) strings surrounded by
 *
 *	< >
 *
 *    delimit strings that shall be replaced in each instance of use of a template.  The structure
 *    and meaning of the replacement string is dependent upon the string type;
 *
 * i) upper case strings denote Keywords which are required to be present in each instance of use of
 *    a template unless they are enclosed in
 *
 *	[ ]
 *
 *    to indicate that there presence is optional;
 *
 *   ...
 *
 *  k)
 *
 *   ...
 *
 *    Where a template-label is declared in document A and referenced in document B, the reference
 *    in document B shall be prefixed by the globally unique name of document A.  In the case of
 *    documents named by an internationally recognised naming authority, such as CCITT or ISO/IEC,
 *    the registered aphanumeric designator of the document shall be used as the identifier, such as
 *    "CCITT Rec. X.722 (1992) | ISO/IEC 10165-4 : 1992".  The format of this string shall be as
 *    specified by the naming authority concerned for references to its documents.  Where the
 *    document references is jointly developed and published by CCITT and ISO/IEC, the designator of
 *    the document shall include both the CCITT and ISO/IEC document designators separated by a "|",
 *    as shown in this example.  Where a globally unique name is not already available, it is
 *    permissible to assign the value of an object identifier to the referenced document, and use
 *    this value as a globally unique document name.  The syntax of a template-label, defined using
 *    the above notation, is as follows:
 *
 *      [document-identifier :] <label-string>
 *      document-identiifer -> "<standard-name>" | object-identifier
 *
 *    A label-string may include any number of the following characters:
 *
 *    1) upper or lower case alphabetic characters
 *
 *    2) the digits 0-9;
 *
 *    3) the character
 *
 *       -
 *
 *    4) the character
 *
 *       /
 *
 *    in any order, commencing with a lower case alphabetic character, with the exception that the
 *    chracter pair
 *
 *      --
 *
 *    shall not appear in a label string.  For example, the following template-label
 *
 *	"CCITT Rec. X.722 (1992) | ISO/IEC 10165-4 : 1992":exampleObjectClass
 *
 *    consitutes a globally unique lable for the definition of exampleObjectClass contained in Annex
 *    A. 
 *
 *    Label references that are not prefixed by a document-identifier are assumed to be references
 *    to labels declared in the document in which the reference appears.
 *
 *  l) wherever a template-label is present in a template as a pointer to another template, it may
 *     be replaced by the entire tex of the references template itself (including the template's
 *     label).  This allows a template to include the templates it references (sub-templates)
 *     "in-line" while maintaining the ability for other templates to refer to the sub-tempates so
 *     defined.  In effect, the supporting production
 *
 *       tempate-label -> template-definition
 *
 *     is assumed for all instances of template-label and template-definition.
 *
 */ 

/*
 *  GDMO GRAMMAR RULES
 */

gdmo:
	| gdmo { gdmo_newline(yyout); } template { gdmo_newline(yyout); }
	;

template:
	class_template
	| package_template
	| parameter_template
	| name_binding_template
	| attribute_template
	| attribute_group_template
	| behaviour_template
	| action_template
	| notification_template

/*
comstring:
	TOK_COMSTRING { gdmo_column += fprintf(yyout, "--%.*s", yyleng, yytext); gdmo_newline(yyout); }
	;

comment:
	| comstring
	| comment comstring
	;
*/

document_identifier:
	TOK_DOCID { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

label_string:
	TOK_LABEL { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

label:
	document_identifier
	| label_string
	;

identifier:
	label_string
	| TOK_ID { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

reference:
	identifier
	;

field_name:
	label
	;

value_reference:
	reference
	;

semicolon:
	TOK_SEMICOLON { gdmo_level--; gdmo_checkcomment(yyout); gdmo_column += fprintf(yyout, "%c", ';');
	}
	;

comma:
	TOK_COMMA { gdmo_checkcomment(yyout); gdmo_column += fprintf(yyout, "%c", ',');  gdmo_newline(yyout); }
	;

/*
colon:
	TOK_COLON { gdmo_column += fprintf(yyout, "%c", ':'); }
	;

period:
	TOK_PERIOD { gdmo_column += fprintf(yyout, "%c", '.'); }
	;

doublequote:
	TOK_DOUBLEQUOTE { gdmo_column += fprintf(yyout, "%c", '"'); }
	;
*/

lbrace:
	TOK_LBRACE { gdmo_checkcomment(yyout); gdmo_column += fprintf(yyout, "{ "); }
	;

rbrace:
	TOK_RBRACE { gdmo_checkcomment(yyout); gdmo_column += fprintf(yyout, " }"); }
	;

/*
lbracket:
	TOK_LBRACKET { gdmo_column += fprintf(yyout, "["); }
	;

rbracket:
	TOK_RBRACKET { gdmo_column += fprintf(yyout, "]"); }
	;

ldbracket:
	TOK_LDBRACKET { gdmo_column += fprintf(yyout, "[["); }
	;

rdbracket:
	TOK_RDBRACKET { gdmo_column += fprintf(yyout, "]]"); }
	;

lparen:
	TOK_LPAREN { gdmo_column += fprintf(yyout, "("); }
	;

rparen:
	TOK_RPAREN { gdmo_column += fprintf(yyout, ")"); }
	;

langle:
	TOK_LANGLE { gdmo_column += fprintf(yyout, "<"); }
	;

rangle:
	TOK_RANGLE { gdmo_column += fprintf(yyout, ">"); }
	;

amp:
	TOK_AMPERSAND { gdmo_column += fprintf(yyout, "&"); }
	;

ampdotamp:
	amp period amp
	;

xmlopen:
	TOK_XMLOPEN { gdmo_column += fprintf(yyout, "<&"); }
	;

xmlclose:
	TOK_XMLCLOSE { gdmo_column += fprintf(yyout, "/>"); }
	;

xmlcopen:
	TOK_XMLCOPEN { gdmo_column += fprintf(yyout, "</&"); }
	;

xmlend:
	rangle
	;
*/

class_template:
	class optional_registration semicolon
	;

class:
	class_label_definition { gdmo_column += fprintf(yyout, " "); }
	TOK_MANAGED_OBJECT_CLASS { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; }
	optional_derivation
	optional_characterization
	optional_conditional_packages
	;

class_label_definition:
	TOK_MANAGED_OBJECT_CLASS_LABEL { fprintf(yyout, "%.*s", (int) strcspn(yytext, " \t\n"), yytext); }
	;

class_label:
	label | class_template
	;

optional_derivation:
	| derivation
	;

derivation:
	TOK_DERIVED_FROM { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; gdmo_newline(yyout); }
	class_label_sequence
	semicolon
	;

class_label_sequence:
	class_label | class_label_sequence comma class_label
	;

optional_characterization:
	| characterization
	;

characterization:
	TOK_CHARACTERIZED_BY { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; gdmo_newline(yyout); }
	package_label_sequence
	semicolon
	;

package_label_sequence:
	package_label | package_label_sequence comma package_label
	;

optional_conditional_packages:
	| conditional_packages
	;

conditional_packages:
	TOK_CONDITIONAL_PACKAGES { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; gdmo_newline(yyout); }
	package_condition_sequence
	semicolon
	;

package_condition_sequence:
	package_condition | package_condition_sequence comma package_condition
	;

package_condition:
	package_label
	package_condition_or_exclusion
	conditional_definition { gdmo_level -= 2; }
	;

package_condition_or_exclusion:
	package_conditional
	| package_exclusion
	;

package_conditional:
	TOK_PRESENT_IF { gdmo_level++; gdmo_newline(yyout); gdmo_column += fprintf(yyout, "PRESENT IF"); gdmo_level++; }
	;

package_exclusion:
	TOK_PRESENT_ONLY_IF { gdmo_level++; gdmo_newline(yyout); gdmo_column += fprintf(yyout, "PRESENT ONLY IF"); gdmo_level++; }
	;

conditional_definition:
	delimited_string
	;

delimited_string:
	TOK_DELIMITED_STRING { gdmo_fprintwrap(yyout, yytext, yyleng, 80 + 8*gdmo_level); }
	| TOK_TEXT_STRING { gdmo_fprintwrap(yyout, yytext, yyleng, 80 + 8*gdmo_level); }
	;

optional_registration:
	| registration

/*
 * [PARAMETER] The object-identifier value, if present, provides a globally unique identifier for
 * the parameter definition.  This value is used in management protocol when it is necessary to
 * identify the parameter.  This construct shall be present if and only if the WITH SYNTAX construct
 * is present.
 */
registration:
	TOK_REGISTERED_AS { gdmo_level--; gdmo_newline(yyout); gdmo_level++; gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	object_identifier
	;

object_identifier:
	ObjectIdentifierValue
	;

ObjectIdentifierValue:
	lbrace DefinedValueObjIdComponentList rbrace
	;

DefinedValueObjIdComponentList:
	ObjIdComponentList
	/* | DefinedValue { gdmo_column += fprintf(yyout, " "); } ObjIdComponentList */
	;

ObjIdComponentList:
	ObjIdComponent
	| ObjIdComponent { gdmo_column += fprintf(yyout, " "); } ObjIdComponentList
	;

ObjIdComponent:
	NameForm
	| NumberForm
	| NameAndNumberForm
	;

NameForm:
	identifier
	/* | DefinedValue */
	;

NumberForm:
	number
	;

NameAndNumberForm:
	TOK_NAME_AND_NUMBER { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	;

number:
	TOK_NUMBER { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

/*
realnumber:
	TOK_REALNUMBER
	| number
	;
*/

/*
DefinedValue:
	value_reference
	*//* | ExternalValueReference */
	/* | ParameterizedValue *//*
	;
*/

/*
ExternalValueReference:
	modulereference
	value_reference
	;
*/

/*
modulereference:
	reference
	;
*/

package_template:
	package optional_registration semicolon
	;

package:
	package_label_definition { gdmo_column += fprintf(yyout, " "); }
	TOK_PACKAGE { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; }
	optional_behaviour_reference
	optional_attributes
	optional_attribute_groups
	optional_actions
	optional_notifications
	;

package_label_definition:
	TOK_PACKAGE_LABEL { fprintf(yyout, "%.*s", (int) strcspn(yytext, " \t\n"), yytext); }
	;

package_label:
	label | package_template
	;

optional_behaviour_reference:
	| behaviour_reference
	;

/*
 * If present [in the PARAMETER template], this construct permits the specification of any behaviour
 * or semantics associated with the parameter.  If the ATTRIBUTE construct is used, this construct
 * does not modify the behaviour of the attribute.
 *
 * If present [in the NAME BINDING template], this construct permits any behavioural impact imposed
 * as a consequence of the name binding to be defined.  The behaviour-definition-label identifies
 * the behaviour definiton concerned.
 *
 * NOTE 1 - This construct is intended for use as a means of describing behaviour specific to a name
 * binding.  Any behaviour that is applicable to all possible instances of a managed object class
 * should be defined as part of the beahovur referenced by the Package template(s) that define the
 * managed object class.
 */
behaviour_reference:
	TOK_BEHAVIOUR { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; gdmo_newline(yyout); }
	behaviour_label_sequence
	semicolon
	;

behaviour_label_sequence:
	behaviour_label
	| behaviour_label_sequence comma behaviour_label
	;

optional_attributes:
	| attributes
	;

attributes:
	TOK_ATTRIBUTES { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; gdmo_newline(yyout); }
	attributes_sequence
	semicolon
	;

attributes_sequence:
	attribute_property
	| attributes_sequence comma attribute_property
	;

attribute_property:
	attribute_label { gdmo_column += fprintf(yyout, " "); }
	propertylist { gdmo_column += fprintf(yyout, " "); } optional_parameter_label_list
	;

propertylist:
	property | propertylist { gdmo_column += fprintf(yyout, " "); } property

property:
	replace_with_default
	| default_value
	| initial_value
	| permitted_values
	| required_values
	| get_property
	| replace_property
	| get_replace
	| add_property
	| remove_property
	| add_remove
	| set_by_create
	| no_modify
	;

replace_with_default:
	TOK_REPLACE_WITH_DEFAULT { gdmo_column += fprintf(yyout, "%.*s ", yyleng, yytext); }
	;

default_value:
	TOK_DEFAULT_VALUE { gdmo_column += fprintf(yyout, "%.*s ", yyleng, yytext); }
	value_specifier
	;

initial_value:
	TOK_INITIAL_VALUE { gdmo_column += fprintf(yyout, "%.*s ", yyleng, yytext); }
	value_specifier
	;

permitted_values:
	TOK_PERMITTED_VALUES { gdmo_column += fprintf(yyout, "%.*s ", yyleng, yytext); }
	context_keyword
	;

required_values:
	TOK_REQUIRED_VALUES { gdmo_column += fprintf(yyout, "%.*s ", yyleng, yytext); }
	context_keyword
	;

get_property:
	TOK_GET	{ gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

replace_property:
	TOK_REPLACE   { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

get_replace:
	TOK_GET_REPLACE	{ gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

add_property:
	TOK_ADD { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

remove_property:
	TOK_REMOVE { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

add_remove:
	TOK_ADD_REMOVE { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

/* Added by X.722 Ammendment 1. */
set_by_create:
	TOK_SET_BY_CREATE { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

/* Added by X.722 Ammendment 2. */
no_modify:
	TOK_NO_MODIFY { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

value_specifier:
	value_reference
	| TOK_EXTERNREF { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	| TOK_DERIVATION_RULE { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	behaviour_label
	;

optional_parameter_label_list:
	| parameter_label_list
	;

parameter_label_list:
	parameter_label { gdmo_newline(yyout); } | parameter_label_list { gdmo_column += fprintf(yyout, " "); } parameter_label { gdmo_newline(yyout); }
	;

optional_attribute_groups:
	| attribute_groups
	;

attribute_groups:
	TOK_ATTRIBUTE_GROUPS { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; gdmo_newline(yyout); }
	attribute_group_sequence
	semicolon
	;

attribute_group_sequence:
	attribute_group_reference
	| attribute_group_sequence comma attribute_group_reference
	;

attribute_group_reference:
	group_label optional_attribute_label_list
	;

optional_attribute_label_list:
	| attribute_label_list
	;

attribute_label_list:
	attribute_label { gdmo_newline(yyout); } | attribute_label_list { gdmo_column += fprintf(yyout, " "); } attribute_label { gdmo_newline(yyout); }
	;

optional_actions:
	| actions
	;

actions:
	TOK_ACTIONS { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); gdmo_level++; }
	action_parameter_sequence
	semicolon
	;

action_parameter_sequence:
	action_parameter
	| action_parameter_sequence comma action_parameter
	;

action_parameter:
	action_label { gdmo_column += fprintf(yyout, " "); }
	optional_action_parameter_label_list
	;

optional_action_parameter_label_list:
	| parameter_label_list
	;

optional_notifications:
	| notifications
	;

notifications:
	TOK_NOTIFICATIONS { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; gdmo_newline(yyout); }
	notification_parameter_sequence
	semicolon
	;

notification_parameter_sequence:
	notification_parameter
	| notification_parameter_sequence comma notification_parameter
	;

notification_parameter:
	notification_label optional_notification_parameter_label_list
	;

optional_notification_parameter_label_list:
	| parameter_label_list
	;

parameter_template:
	parameter optional_registration semicolon
	;

parameter:
	parameter_label_definition { gdmo_column += fprintf(yyout, " "); }
	TOK_PARAMETER { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; }
	context
	syntax_or_attribute_choice
	optional_behaviour_reference
	;

parameter_label_definition:
	TOK_PARAMETER_LABEL { fprintf(yyout, "%.*s", (int) strcspn(yytext, " \t\n"), yytext); }
	;

parameter_label:
	label | parameter_template
	;

context:
	TOK_CONTEXT { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); gdmo_level++; }
	context_type
	semicolon
	;

context_type:
	context_keyword
	| action_info
	| action_reply
	| event_info
	| event_reply
	| specific_error
	;

/*
 * context-keyword: this option is a reference to a context defined externally to the template.  The
 * structure of the reference consists of a type-reference followed by an identifier which is the
 * name of a field in the management PDU specified by the type-reference.  It may therefore be used
 * as a reference to a context defined in another document.  This may be used to indicate, for
 * example, that the parameter applies only to a specific field of the CMIS Event information
 * parameter (e.g. ITU-T Rec. X.733 | ISO/IEC 10164-4 Additional information field), or the CMIS
 * Action reply paraemter.  If the parameter does not map to a specific field-name (e.g., Event
 * information is defined to be  set of Parameter Identifier/Parameter Value pairs), one of the
 * following more general contexts can be specified.
 */
context_keyword:
	TOK_EXTERNREF { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	/* typereference period identifier */
	;

/*
 * ACTION-INFO: this option defines the parameter as being applicable to the representation of a
 * parameter that may be carried in the CMIS Action information parameter.
 */
action_info:
	TOK_ACTION_INFO { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

/*
 * ACTION-REPLY: this option defines the parameter as being applicable to the representation of a
 * parameter that may be carried in an CMIS Action reply parameter.
 */
action_reply:
	TOK_ACTION_REPLY { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

/*
 * EVENT-INFO: this option defines the parameter as being applicable to the representation of a
 * parameter that may be carried in the CMIS Event information parameter.
 */
event_info:
	TOK_EVENT_INFO { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

/*
 * EVENT-REPLY: this option defines the parameter as being applicable to the representation of a
 * parameter that may be carried in the CMIS Event reply parameter.
 */
event_reply:
	TOK_EVENT_REPLY { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

/*
 * SPECIFIC-ERROR: this option defines the parameter as being applicable  to the representation or
 * generation ofa CMIS processing failure error.  When this option is used with parameters that
 * apply to attributes, the managed object class definition shall specify whether, when this error
 * occurs for a Replace attribute value or Replace with default value operation on one attribute,
 * other attributes refrences in a single replace request are modified or not.
 */
specific_error:
	TOK_SPECIFIC_ERROR { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

syntax_or_attribute_choice:
	with_syntax | attribute_reference
	;

/*
 * This contruct, if present, identifies the ASN.1 type of the parameter, as carried in the
 * protocol.
 */
with_syntax:
	TOK_WITH_SYNTAX { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); gdmo_level++; }
	context_keyword
	semicolon
	;

/*
 * If present, this construct identifies an Atrribute template whose syntax and object identifier
 * are used as the syntax and object identifier of the parameter, respectively.
 */
attribute_reference:
	TOK_ATTRIBUTE { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); gdmo_level++; }
	attribute_label
	semicolon
	;


name_binding_template:
	name_binding optional_registration semicolon
	;

name_binding:
	name_binding_label_definition { gdmo_column += fprintf(yyout, " "); }
	TOK_NAME_BINDING { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; }
	subordinate semicolon
	named_by_superior semicolon
	with_attribute semicolon
	optional_behaviour_reference
	optional_create
	optional_delete
	;

name_binding_label_definition:
	TOK_NAME_BINDING_LABEL { fprintf(yyout, "%.*s", (int) strcspn(yytext, " \t\n"), yytext); }
	;

/*
name_binding_label:
	label | name_binding_template
	;
*/

/*
 * This defines a managed object class whose instances may be named by instances of the object class
 * defined by the NAMED BY SUPERIOR OBJECT CLASS construct.  The name of an instance of this
 * subordinate object class is constructed by concatenating the distinguished name of its superior
 * object with the relative distinguished name of the subordinate object.  If AND SUBCLASSES is
 * specified, the name binding also applies to all subclasses of the specified managed object class.
 */
subordinate:
	TOK_SUBORDINATE { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); gdmo_level++; }
	class_label optional_and_subclasses
	;

optional_and_subclasses:
	| and_subclasses
	;

and_subclasses:
	TOK_AND_SUBCLASSES { gdmo_column += fprintf(yyout, " %.*s", yyleng, yytext); }
	;

/*
 * This defines a managed object class or other object class, such as a Directory object class,
 * whose instances may name instances of the managed object class defined by the SUBORDINATE OBJECT
 * CLASS construct.  If AND SUBCLASSES is specified, the name binding also applies to all subclasses
 * of the specified object class.
 */
named_by_superior:
	TOK_NAMED_BY { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	TOK_SUPERIOR_OBJECT_CLASS { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); gdmo_level++; }
	class_label optional_and_subclasses
	;

/*
 * This defines the attribute that shall be used, in the context of this name binding, to construct
 * the relative distinguished name for instances of the managed object clas defined by the
 * SUBORDINATE OBJECT CLASS construct.  Values of this attribute shall be represented by
 * single-valued data types complying with the restriction specified in ITU-T Rec. X.720 | ISO/IEC
 * 10165-1; if no suitable attribute is available for use as a naming attribute, managed object
 * designers are encouranged to provide a namting attribute of type GraphicString.
 */
with_attribute:
	TOK_WITH_ATTRIBUTE { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); gdmo_level++; }
	attribute_label
	;

/*
 * Present if it is permitted to create new instances of the managed object class referenced by the
 * SUBORDINATE OBJECT CLASS construct in the context of this name binding, by means of system
 * management operation.  The create-modifier values specify the options available on creation.
 *
 * The behaviour defintions shall specify what course of action is taken when there is a choice of
 * Name Bindings that may be applied to the new managed object.
 *
 * The sources of intitial attribute values used at managed object creation time, and their
 * associated precedence rules, are defined in ITU-T Rec. X.720 | ISO/IEC 10165-1.
 *
 * If present, the parameter-labels identify name binding specific error parameters associated with
 * the Create operation.  These are reported as processing failures.  The syntax of the error
 * paramteers are defined in the referenced templates.
 */
optional_create:
	| create semicolon
	;

create:
	TOK_CREATE { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); gdmo_level++; }
	create_modifier_sequence optional_create_parameter_label_list
	;

create_modifier_sequence:
	| create_modifier
	| create_modifier_sequence comma create_modifier
	;

/*
 * The permitted create-modifier values are as follows:
 */
create_modifier:
	with_reference_object | with_automatic_instance_naming
	;

/*
 * WITH-REFERENCE-OBJECT.  If present, a reference managed object may be specified on creation as a
 * source of default values and to specify choice of conditional packages;
 */
with_reference_object:
	TOK_WITH_REFERENCE_OBJECT { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

/*
 * WITH-AUTOMATIC-INSTANCE-NAMING.  If present, the Create request may omit to specify the instance
 * name of the new managed object.
 */
with_automatic_instance_naming:
	TOK_WITH_AUTOMATIC_INSTANCE_NAMING { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

optional_create_parameter_label_list:
	| parameter_label_list
	;

optional_delete:
	| delete semicolon
	;

delete:
	TOK_DELETE { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); gdmo_level++; }
	optional_delete_modifier optional_delete_parameter_label_list
	;

/*
 * Present if it is permitted to delete instances of the manged object class referenced by the
 * SUBORDINATE OBJECT CLASS construct in the context of this name binding.  The delete-modifier, if
 * present, indicates the behaviour of a managed object of that class if the managed object is
 * deleted.
 *
 * Other rules which describe the behaviour with respect to deletion of contained managed objects
 * may be specified in the BEHAVIOR construct.
 *
 * NOTE 2 - Given that the DELETES-CONTAINED-OBJECTS modifier permits deletion of a managed object
 * regardless of whether it contains other managed objects, it is advisable to use the
 * ONLY-IF-NO-CONTAINED-OBJECTS modifier if there is any doubt as to which modifier is appropriate.
 *
 * If there are constraints on deletion relative to other relationships or conditions that are
 * generic to the managed object class, these shall be specified as part of the behaviour of the
 * managed object class.
 *
 * If present, the parameter-labels identify name binding specific error parameters associated with
 * the Delete operation.  These are reported as processing failures.  The syntax of the error
 * parameters are defined in the referenced templates.
 */
optional_delete_modifier:
	| delete_modifier
	;

/*
 * The permitted delete-modifiers are as follows:
 */
delete_modifier:
	only_if_no_contained_objects | deletes_contained_objects
	;

/*
 * ONLY-IF-NO-CONTAINTED-OBJECTS.  If specified, any contained managed objects shall be explicitly
 * deleted by management operations prior to deletion of the contained managed object, i.e., a
 * Delete request will cause an error if there are contained managed objects;
 */
only_if_no_contained_objects:
	TOK_ONLY_IF_NO_CONTAINED_OBJECTS { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

/*
 * DELETES-CONTAINED-OBJECTS.  If a Delete requeset is applied to a managed object for which the
 * DELETES-CONTAINED-OBJECTS modifier is specified, the Delete request will fail if any directly or
 * indirectly contained managed object has the ONLY-IF-NO-CONTAINED-OBJECTS modifier specified and
 * also has a contained managed object; otherwise, a successful Delete request will also have the
 * effect of deleting contained managed objects.
 */
deletes_contained_objects:
	TOK_DELETES_CONTAINED_OBJECTS { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

optional_delete_parameter_label_list:
	| parameter_label_list
	;

attribute_template:
	attribute optional_registration semicolon
	;

attribute:
	attribute_label_definition { gdmo_column += fprintf(yyout, " "); }
	TOK_ATTRIBUTE { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; }
	derived_or_with_syntax_choice
	optional_matches
	optional_behaviour_reference
	optional_parameters
	;

attribute_label_definition:
	TOK_ATTRIBUTE_LABEL { fprintf(yyout, "%.*s", (int) strcspn(yytext, " \t\n"), yytext); }
	;

attribute_label:
	label | attribute_template
	;

optional_matches:
	| matches semicolon
	;

matches:
	TOK_MATCHES_FOR { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; gdmo_newline(yyout); }
	qualifier_sequence
	;

qualifier_sequence:
	qualifier
	| qualifier_sequence comma qualifier
	;

optional_parameters:
	| parameters semicolon
	;

parameters:
	TOK_PARAMETERS { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; gdmo_newline(yyout); }
	parameter_label_sequence
	;

parameter_label_sequence:
	parameter_label | parameter_label_sequence comma parameter_label
	;

qualifier:
	equality | ordering | substrings | set_comparison | set_intersection
	;

equality:
	TOK_EQUALITY { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

ordering:
	TOK_ORDERING { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

substrings:
	TOK_SUBSTRINGS { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

set_comparison:
	TOK_SET_COMPARISON { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

set_intersection:
	TOK_SET_INTERSECTION { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

derived_or_with_syntax_choice:
	derived_from | with_attribute_syntax
	;

derived_from:
	TOK_DERIVED_FROM { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); gdmo_level++; }
	attribute_label
	semicolon
	;

with_attribute_syntax:
	TOK_WITH_ATTRIBUTE_SYNTAX { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); gdmo_level++; }
	context_keyword
	semicolon
	;

attribute_group_template:
	attribute_group optional_registration semicolon
	;

attribute_group:
	group_label_definition { gdmo_column += fprintf(yyout, " "); }
	TOK_ATTRIBUTE_GROUP { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; }
	optional_group_elements
	optional_fixed
	optional_description
	;

group_label_definition:
	TOK_ATTRIBUTE_GROUP_LABEL { fprintf(yyout, "%.*s", (int) strcspn(yytext, " \t\n"), yytext); }
	;

group_label:
	label | attribute_group_template
	;

optional_group_elements:
	| group_elements semicolon
	;

group_elements:
	TOK_GROUP_ELEMENTS { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; gdmo_newline(yyout); }
	attribute_label_sequence
	;

attribute_label_sequence:
	attribute_label | attribute_label_sequence comma attribute_label
	;

optional_fixed:
	| fixed semicolon
	;

fixed:
	TOK_FIXED { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); gdmo_level++; }
	;

optional_description:
	| description semicolon
	;

description:
	TOK_DESCRIPTION { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "DESCRIPTION"); gdmo_level++; }
	delimited_string
	;

behaviour_template:
	behaviour
	;

behaviour:
	behaviour_label_definition { gdmo_column += fprintf(yyout, " "); }
	TOK_BEHAVIOUR { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; }
	defined_as semicolon
	;

behaviour_label_definition:
	TOK_BEHAVIOUR_LABEL { fprintf(yyout, "%.*s", (int) strcspn(yytext, " \t\n"), yytext); }
	;

behaviour_label:
	label | behaviour_template
	;

defined_as:
	TOK_DEFINED_AS { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "DEFINED AS"); gdmo_level++; }
	delimited_string { gdmo_level--; }
	;

action_template:
	action optional_registration semicolon
	;

action:
	action_label_definition { gdmo_column += fprintf(yyout, " "); }
	TOK_ACTION { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; }
	optional_behaviour_reference
	optional_mode_confirmed
	optional_parameters
	optional_with_information_syntax
	optional_with_reply_syntax
	;

action_label_definition:
	TOK_ACTION_LABEL { fprintf(yyout, "%.*s", (int) strcspn(yytext, " \t\n"), yytext); }
	;

action_label:
	label | action_template
	;

optional_mode_confirmed:
	| mode_confirmed semicolon
	;

/*
 * If present, the action shall operate in confirmed mode.  If absent, the action may be confirmed
 * or unconfirmed at the discretion of the manager.
 */
mode_confirmed:
	TOK_MODE_CONFIRMED { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; }
	;

optional_with_information_syntax:
	| with_information_syntax
	;

/*
 * If present, the type-reference identifies the ASN.1 data type that described the structure of the
 * action information parameter that is carried in management protocol.  If absent, there is no
 * action-specific information associated with the action invocation.
 */
with_information_syntax:
	TOK_WITH_INFORMATION_SYNTAX { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); gdmo_level++; }
	context_keyword
	semicolon
	;

optional_with_reply_syntax:
	| with_reply_syntax semicolon
	;

/*
 * If present, the type-reference identifies the ASN.1 data type that described the structure of the
 * action reply parameter that is carried in management protocol.  If absent, there is no
 * action-specific information associated with the action reply.
 */
with_reply_syntax:
	TOK_WITH_REPLY_SYNTAX { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); gdmo_level++; }
	context_keyword
	;

notification_template:
	notification optional_registration semicolon
	;

notification:
	notification_label_definition { gdmo_column += fprintf(yyout, " "); }
	TOK_NOTIFICATION { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); gdmo_level++; }
	optional_behaviour_reference
	optional_parameters
	optional_with_information_syntax_and_attribute_ids
	optional_with_reply_syntax
	;

notification_label_definition:
	TOK_NOTIFICATION_LABEL { fprintf(yyout, "%.*s", (int) strcspn(yytext, " \t\n"), yytext); }
	;

notification_label:
	label | notification_template
	;

optional_with_information_syntax_and_attribute_ids:
	| with_information_syntax_and_attribute_ids
	;

with_information_syntax_and_attribute_ids:
	with_information_syntax2
	optional_and_attribute_ids
	;

with_information_syntax2:
	TOK_WITH_INFORMATION_SYNTAX { gdmo_newline(yyout); gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); gdmo_level++; }
	context_keyword
	;

optional_and_attribute_ids:
	semicolon
	| and_attribute_ids
	;

and_attribute_ids:
	TOK_AND_ATTRIBUTE_IDS { gdmo_newline(yyout); gdmo_column += fprintf(yyout, " %.*s", yyleng, yytext); gdmo_level++; gdmo_newline(yyout); }
	field_attribute_sequence { gdmo_level--; }
	semicolon
	;

field_attribute_sequence:
	field_attribute
	| field_attribute_sequence comma field_attribute
	;

field_attribute:
	field_name { gdmo_column += fprintf(yyout, " "); } attribute_label
	;

/*
 *  ASN.1 (X.208) GRAMAR RULES
 */

/*
asn1:
	| ModuleDefinition
	;

DEFINITIONS:
	TOK_DEFINITIONS { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

ASSIGNMENT:
	TOK_ASSIGNMENT { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

BEGIN:
	TOK_BEGIN { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

END:
	TOK_END { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

STRING:
	TOK_STRING { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

EXPLICIT:
	TOK_EXPLICIT { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

IMPLICIT:
	TOK_IMPLICIT { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

AUTOMATIC:
	TOK_AUTOMATIC { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

TAGS:
	TOK_TAGS { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

ANY:
	TOK_ANY { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

BY:
	TOK_BY { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

DEFINED:
	TOK_DEFINED { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

IDENTIFIER:
	TOK_IDENTIFIER { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

EXTENSIBILITY_IMPLIED:
	TOK_EXTENSIBILITY { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	TOK_IMPLIED { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

*/
/*
REFERENCE:
	TOK_REFERENCE { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;
*/
/*

EXPORTS:
	TOK_EXPORTS { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

ALL:
	TOK_ALL { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

IMPORTS:
	TOK_IMPORTS { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

FROM:
	TOK_FROM { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

AT:
	TOK_AT { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

ASTERISK:
	TOK_ASTERISK { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

BOOLEAN:
	TOK_BOOLEAN { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

TRUE:
	TOK_TRUE { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

FALSE:
	TOK_FALSE { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

true:
	TOK_true { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

false:
	TOK_false { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

INTEGER:
	TOK_INTEGER { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

HYPHEN:
	TOK_HYPHEN { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

ELIPSIS:
	TOK_ELIPSIS { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

OPTIONAL:
	TOK_OPTIONAL { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

DEFAULT:
	TOK_DEFAULT { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

UNIVERSAL:
	TOK_UNIVERSAL { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

PRIVATE:
	TOK_PRIVATE { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

APPLICATION:
	TOK_APPLICATION { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

PLUS_INFINITY:
	TOK_PLUS_INFINITY { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

MINUS_INFINITY:
	TOK_MINUS_INFINITY { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

OF:
	TOK_OF { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

SET:
	TOK_SET { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

SEQUENCE:
	TOK_SEQUENCE { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

NULL:
	TOK_NULL { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

ModuleDefinition:
	ModuleIdentifier
	DEFINITIONS
	TagDefault
	ExtensionDefault
	ASSIGNMENT
	BEGIN
	ModuleBody
	END
	;

VisibleString:
	STRING
	;

GraphicString:
	STRING
	;

xmlasn1typename:
	STRING
	;

hstring:
	STRING
	;

xmlhstring:
	STRING
	;

cstring:
	STRING
	;

xmlcstring:
	STRING
	;

bstring:
	STRING
	;

xmlbstring:
	STRING
	;

TagDefault:
	| EXPLICIT TAGS
	| IMPLICIT TAGS
	| AUTOMATIC TAGS
	;

ExtensionDefault:
	| EXTENSIBILITY_IMPLIED
	;

ModuleIdentifier:
	modulereference
	AssignedIdentifier
	;

modulereference:
	reference
	;

*/
/*
reference:
	REFERENCE
	;
*/
/*

AssignedIdentifier:
	| ObjectIdentifierValue
	| DefinedValue
	;

ModuleBody:
	| Exports Imports AssignmentList
	;

Exports:
	| EXPORTS SymbolsExported semicolon
	| EXPORTS ALL semicolon
	;

SymbolsExported:
	| SymbolList
	;

Imports:
	| IMPORTS SymbolsImported semicolon
	;

SymbolsImported:
	| SymbolsFromModuleList
	;

SymbolsFromModuleList:
	SymbolsFromModule SymbolsFromModuleList
	| SymbolsFromModule
	;

SymbolsFromModule:
	SymbolList FROM GlobalModuleReference
	;

GlobalModuleReference:
	modulereference AssignedIdentifier
	;

SymbolList:
	Symbol
	| SymbolList comma Symbol
	;

Symbol:
	Reference
	*//* | ParameterizedReference *//*
	;

Reference:
	typereference
	| valuereference
	*//* | objectclassreference */
	/* | objectreference */
	/* | objectsetreference *//*
	;

AssignmentList:
	Assignment
	| AssignmentList Assignment
	;

Assignment:
	TypeAssignment
	| ValueAssignment
	| XMLValueAssignment
	| ValueSetTypeAssignment
	*//* | ObjectClassAssignment */
	/* | ObjectAssignment */
	/* | ObjectSetAssignment */
	/* | ParameterizedAssignment *//*
	;

TypeAssignment:
	typereference ASSIGNMENT Type
	;

ValueAssignment:
	valuereference Type ASSIGNMENT Value
	;

XMLValueAssignment:
	valuereference ASSIGNMENT XMLTypedValue
	;

XMLTypedValue:
	xmlopen NonParameterizedTypeName xmlend
	XMLValue
	xmlcopen NonParameterizedTypeName xmlend
	| xmlopen NonParameterizedTypeName xmlclose
	;

ValueSetTypeAssignment:
	typereference Type ASSIGNMENT ValueSet
	;

ValueSet:
	lbrace ElementSetSpecs rbrace
	;

ExternalTypeReference:
	modulereference period typereference
	;

ExternalValueReference:
	modulereference period valuereference
	;

AbsoluteReference:
	AT ModuleIdentifier period ItemSpec
	;

ItemSpec:
	typereference
	| ItemId period ComponentId
	;

ItemId:
	ItemSpec
	;

ComponentId:
	identifier
	| number
	| ASTERISK
	;

DefinedType:
	ExternalTypeReference
	| typereference
	*//* | ParameterizedType */
	/* | ParameterizedValueSetType *//*
	;

DefinedValue:
	ExternalValueReference
	| valuereference
	;

NonParameterizedTypeName:
	ExternalTypeReference
	| typereference
	| xmlasn1typename
	;

Typeassignment:
	typereference ASSIGNMENT Type
	;

Valueassignment:
	valuereference ASSIGNMENT Value
	;

Type:
	BuiltinType
	| ReferencedType
	| ConstrainedType
	| Subtype
	;

BuiltinType:
	AnyType
	| BitStringType
	| BooleanType
	| CharacterStringType
	| ChoiceType
	| EmbeddedPDVType
	| EnumeratedType
	| ExternalType
	*//* | InstanceOfType *//*
	| IntegerType
	| NullType
	*//* | ObjectClassFieldType *//*
	| ObjectIdentifierType
	| OctetStringType
	| RealType
	| RelativeOIDType
	| SequenceOfType
	| SequenceType
	| SetOfType
	| SetType
	| TaggedType
	;

ReferencedType:
	DefinedType
	| UsefulType
	| SelectionType
	*//* | TypeFromObject */
	/* | ValueSetFromObjects *//*
	;

NamedType:
	identifier Type
	| Type
	| SelectionType
	;

Value:
	BuiltinValue
	| ReferencedValue
	*//* | ObjectClassFieldValue *//*
	;

XMLValue:
	XMLBuiltinValue
	*//* | XMLObjectClassFieldValue *//*
	;

BuiltinValue:
	AnyValue
	| BitStringValue
	| BooleanValue
	| CharacterStringValue
	| ChoiceValue
	| EmbeddedPDVValue
	| EnumeratedValue
	| ExternalValue
	*//* | InstanceOfValue *//*
	| IntegerValue
	| NullValue
	| ObjectIdentifierValue
	| OctetStringValue
	| RealValue
	| RelativeOIDValue
		| SelectionValue
	| SequenceOfValue
	| SequenceValue
	| SetOfValue
	| SetValue
	| TaggedValue
	;

XMLBuiltinValue:
	XMLBitStringValue
	| XMLBooleanValue
	| XMLCharacterStringValue
	| XMLChoiceValue
	| XMLEmbeddedPDVValue
	| XMLEnumeratedValue
	| XMLExternalValue
	*//* | XMLInstanceOfValue *//*
	| XMLIntegerValue
	| XMLNullValue
	| XMLObjectIdentifierValue
	| XMLOctetStringValue
	| XMLRealValue
	| XMLRelativeOIDValue
	| XMLSequenceOfValue
	| XMLSequenceValue
	| XMLSetOfValue
	| XMLSetValue
	| XMLTaggedValue
	;

ReferencedValue:
	DefinedValue
	*//* | ValueFromObject *//*
	;

NamedValue:
	identifier Value
	;

XMLNamedValue:
	xmlopen identifier xmlend
	XMLValue
	xmlcopen identifier xmlend
	;

BooleanType:
	BOOLEAN
	;

BooleanValue:
	TRUE | FALSE
	;

XMLBooleanValue:
	xmlopen true xmlclose
	| xmlopen false xmlclose
	;

IntegerType:
	INTEGER OptionalNamedNumberList
	;

OptionalNamedNumberList:
	| lbrace NamedNumberList rbrace
	;

NamedNumberList:
	NamedNumber
	| NamedNumberList comma NamedNumber
	;

NamedNumber:
	identifier lparen SignedNumberOrDefinedValue rparen
	;

SignedNumberOrDefinedValue:
	SignedNumber
	| DefinedValue
	;

SignedNumber:
	number
	| HYPHEN number
	;

IntegerValue:
	SignedNumber
	| identifier
	;

XMLIntegerValue:
	SignedNumber
	| xmlopen identifier xmlclose
	;

EnumeratedType:
	TOK_ENUMERATED { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	lbrace Enumerations rbrace
	;

Enumerations:
	RootEnumeration
	| RootEnumeration comma ELIPSIS ExceptionSpec
	| RootEnumeration comma ELIPSIS ExceptionSpec comma AdditionalEnumeration
	;

RootEnumeration:
	Enumeration
	;

AdditionalEnumeration:
	Enumeration
	;

Enumeration:
	EnumerationItem
	| EnumerationItem comma Enumeration
	;

EnumerationItem:
	identifier
	| NamedNumber
	;

EnumeratedValue:
	identifier
	;

XMLEnumeratedValue:
	xmlopen identifier xmlclose
	;

RealType:
	TOK_REAL { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	;

RealValue:
	NumericRealValue
	| SpecialRealValue
	;

NumericRealValue:
	realnumber
	| HYPHEN realnumber
	| SequenceValue
	| lbrace Mantissa comma Base comma Exponent rbrace
	| "0"
	;

Mantissa:
	SignedNumber
	;

Base:
	"2" | "10"
	;

Exponent:
	SignedNumber
	;

SpecialRealValue:
	PLUS_INFINITY
	| MINUS_INFINITY
	;

XMLRealValue:
	XMLNumericRealValue
	| XMLSpecialRealValue
	;

XMLNumericRealValue:
	realnumber
	| HYPHEN realnumber
	;

XMLSpecialRealValue:
	xmlopen PLUS_INFINITY xmlclose
	| xmlopen MINUS_INFINITY xmlclose
	;

BitStringType:
	TOK_BIT { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	STRING OptionalNamedBitList
	;

OptionalNamedBitList:
	| lbrace NamedBitList rbrace
	;

NamedBitList:
	NamedBit
	| NamedBitList comma NamedBit
	;

NamedBit:
	identifier lparen NumberOrDefinedValue rparen
	;

NumberOrDefinedValue:
	number
	| DefinedValue
	;

BitStringValue:
	bstring
	| hstring
	| lbrace OptionalIdentifierList rbrace
	| TOK_CONTAINING { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	Value
	;

OptionalIdentifierList:
	| IdentifierList
	;

IdentifierList:
	identifier
	| IdentifierList comma identifier
	;

XMLBitStringValue:
	| XMLTypedValue
	| xmlbstring
	| XMLIdentifierList
	;

XMLIdentifierList:
	xmlopen identifier xmlclose
	| XMLIdentifierList xmlopen identifier xmlclose
	;

OctetStringType:
	TOK_OCTET { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	STRING
	;

OctetStringValue:
	bstring
	| hstring
	| TOK_CONTAINING { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	Value
	;

XMLOctetStringValue:
	XMLTypedValue
	| xmlhstring
	;

NullType:
	NULL
	;

NullValue:
	NULL
	;

XMLNullValue:
	;

SequenceType:
	lbrace OptionalComponentTypeLists rbrace
	;

OptionalComponentTypeLists:
	| ComponentTypeLists
	| ExtensionAndException OptionalExtensionMarker
	;

ExtensionAndException:
	ELIPSIS OptionalExtensionSpec
	;

OptionalExtensionSpec:
	| ExtensionSpec
	;

ExtensionSpec:
	;

OptionalExtensionMarker:
	| comma
	| ELIPSIS
	;

ComponentTypeLists:
	RootComponentTypeList
	| RootComponentTypeList comma ExtensionAndException ExtensionAdditions OptionalExtensionMarker
	| RootComponentTypeList comma ExtensionAndException ExtensionAdditions ExtensionEndMarker comma RootComponentTypeList
	| ExtensionAndException ExtensionAdditions ExtensionEndMarker comma RootComponentTypeList
	| ExtensionAndException ExtensionAdditions OptionalExtensionMarker
	;

RootComponentTypeList:
	ComponentTypeList
	;

ExtensionEndMarker:
	comma ELIPSIS
	;

ExtensionAdditions:
	ExtensionAddition
	| ExtensionAdditions comma ExtensionAddition
	;

ExtensionAddition:
	ComponentType
	| ExtensionAdditionGroup
	;

ExtensionAdditionGroup:
	ldbracket VersionNumber ComponentTypeList rdbracket
	;

VersionNumber:
	| number colon
	;

ComponentTypeList:
	ComponentType
	| ComponentTypeList comma ComponentType
	;

ComponentType:
	NamedType
	| NamedType OPTIONAL
	| NamedType DEFAULT Value
	| TOK_COMPONENTS { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	OF Type
	;

SequenceValue:
	lbrace OptionalComponentValueList rbrace
	;

OptionalComponentValueList:
	| ComponentValueList
	;

ComponentValueList:
	NamedValue
	| ComponentValueList comma NamedValue
	;

XMLSequenceValue:
	| XMLComponentValueList
	;

XMLComponentValueList:
	XMLNamedValue
	| XMLComponentValueList XMLNamedValue
	;

SequenceOfType:
	SEQUENCE
	OF
	TypeOrNamedType
	| SEQUENCE
	;

TypeOrNamedType:
	Type
	| NamedType
	;

SequenceOfValue:
	lbrace OptionalValueList rbrace
	;

OptionalValueList:
	| ValueList
	| NamedValueList
	;

ValueList:
	Value
	| ValueList comma Value
	;

NamedValueList:
	NamedValue
	| NamedValueList comma NamedValue
	;

XMLSequenceOfValue:
	| XMLValueList
	| XMLDelimitedItemList
	| XMLSpaceSeparatedList
	;

XMLValueList:
	XMLValueOrEmpty
	| XMLValueOrEmpty XMLValueList
	;

XMLValueOrEmpty:
	XMLValue
	| xmlopen NonParameterizedTypeName xmlclose
	;

XMLSpaceSeparatedList:
	XMLValueOrEmpty
	| XMLValueOrEmpty *//* TOK_SPACE *//* XMLSpaceSeparatedList
	;

XMLDelimitedItemList:
	XMLDelimitedItem
	| XMLDelimitedItem XMLDelimitedItemList
	;

XMLDelimitedItem:
	xmlopen NonParameterizedTypeName xmlend
	XMLValue
	xmlcopen NonParameterizedTypeName xmlend
	|
	xmlopen identifier xmlend
	XMLValue
	xmlcopen identifier xmlend
	;

SetType:
	SET
	lbrace OptionalComponentTypeLists rbrace
	;

SetValue:
	lbrace OptionalComponentValueList rbrace
	;

XMLSetValue:
	| XMLComponentValueList
	;

SetOfType:
	SET
	OF
	TypeOrNamedType
	| SET
	;

SetOfValue:
	lbrace OptionalValueList rbrace
	;

XMLSetOfValue:
	| XMLValueList
	| XMLDelimitedItemList
	| XMLSpaceSeparatedList
	;

ChoiceType:
	TOK_CHOICE { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); }
	lbrace AlternativeTypeLists rbrace
	;

AlternativeTypeLists:
	RootAlternativeTypeList
	| RootAlternativeTypeList comma ExtensionAndException ExtensionAdditionAlternatives OptionalExtensionMarker
	;

RootAlternativeTypeList:
	AlternativeTypeList;

ExtensionAdditionAlternatives:
	| comma ExtensionAdditionAlternativesList
	;

ExtensionAdditionAlternativesList:
	ExtensionAdditionAlternative
	| ExtensionAdditionAlternativesList comma ExtensionAdditionAlternative
	;

ExtensionAdditionAlternative:
	ExtensionAdditionAlternativesGroup
	| NamedType
	;

ExtensionAdditionAlternativesGroup:
	ldbracket VersionNumber AlternativeTypeList rdbracket
	;

AlternativeTypeList:
	NamedType
	| AlternativeTypeList comma NamedType
	;

ChoiceValue:
	NamedValue
	| identifier colon Value
	;

XMLChoiceValue:
	xmlopen identifier xmlend
	XMLValue
	xmlcopen identifier xmlend
	;

SelectionType:
	identifier langle Type
	;

SelectionValue:
	NamedValue
	;

TaggedType:
	Tag Type
	| Tag IMPLICIT Type
	| Tag EXPLICIT Type
	;

Tag:
       lbracket Class ClassNumber rbracket
       ;

ClassNumber:
	number
	| DefinedValue
	;

Class:
	| UNIVERSAL
	| APPLICATION
	| PRIVATE
	;

TaggedValue:
	Value
	;

XMLTaggedValue:
	XMLValue
	;

AnyType:
	ANY
	| ANY DEFINED BY identifier
	;

AnyValue:
	Type Value
	;

ObjectIdentifierType:
	TOK_OBJECT { gdmo_column += fprintf(yyout, "%.*s", yyleng, yytext); } IDENTIFIER
	;

ObjectIdentifierValue:
	lbrace DefinedValueObjIdComponentsList rbrace
	;

DefinedValueObjIdComponentsList:
	ObjIdComponentsList
	| DefinedValue { gdmo_column += fprintf(yyout, " "); } ObjIdComponentsList
	;

ObjIdComponentsList:
	ObjIdComponents
	| ObjIdComponents { gdmo_column += fprintf(yyout, " "); } ObjIdComponentsList
	;

ObjIdComponents:
	NameForm
	| NumberForm
	| NameAndNumberForm
	| DefinedValue
	;

NameForm:
	identifier OptionalNumberForm
	;

OptionalNumberForm:
	| lparen NumberForm rparen
	;

NumberForm:
	number
	*//* | DefinedValue *//*
	;

NameAndNumberForm:
	TOK_NAME_AND_NUMBER { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	;

XMLObjectIdentifierValue:
	XMLObjIdComponentList
	;

XMLObjIdComponentList:
	XMLObjIdComponent
	| XMLObjIdComponent ampdotamp XMLObjIdComponentList
	;

XMLObjIdComponent:
	NameForm
	| XMLNumberForm
	| XMLNameAndNumberForm
	;

XMLNumberForm:
	number
	;

XMLNameAndNumberForm:
	identifier amp lparen amp XMLNumberForm amp rparen
	;

RelativeOIDType:
	TOK_RELATIVE_OID { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	;

RelativeOIDValue:
	lbrace RelativeOIDComponentsList rbrace
	;

RelativeOIDComponentsList:
	RelativeOIDComponents
	| RelativeOIDComponents RelativeOIDComponentsList
	;

RelativeOIDComponents:
	NumberForm
	NameAndNumberForm
	| DefinedValue
	;

XMLRelativeOIDValue:
	XMLRelativeOIDComponentList
	;

XMLRelativeOIDComponentList:
	XMLRelativeOIDComponent
	| XMLRelativeOIDComponent ampdotamp XMLRelativeOIDComponentList
	;

XMLRelativeOIDComponent:
	XMLNumberForm
	| XMLNameAndNumberForm
	;

EmbeddedPDVType:
	TOK_EMBEDDED { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); } TOK_PDV { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	;

EmbeddedPDVValue:
	SequenceValue
	;

XMLEmbeddedPDVValue:
	XMLSequenceValue
	;

ExternalType:
	TOK_EXTERNAL { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	;

ExternalValue:
	SequenceValue
	;

XMLExternalValue:
	XMLSequenceValue
	;

CharacterStringType:
	RestrictedCharacterStringType
	| UnrestrictedCharacterStringType
	;

CharacterStringValue:
	RestrictedCharacterStringValue
	| UnrestrictedCharacterStringValue
	;

XMLCharacterStringValue:
	XMLRestrictedCharacterStringValue
	| XMLUnrestrictedCharacterStringValue
	;

RestrictedCharacterStringType:
	TOK_BMPString { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	| TOK_GeneralString { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	| TOK_GraphicString { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	| TOK_IA5String { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	| TOK_ISO646String { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	| TOK_NumericString { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	| TOK_PrintableString { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	| TOK_TeletexString { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	| TOK_T61String { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	| TOK_UniversalString { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	| TOK_UTF8String { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	| TOK_VideotexString { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	| TOK_VisibleString { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	;

RestrictedCharacterStringValue:
	cstring
	| CharacterStringList
	| Quadruple
	| Tuple
	;

CharacterStringList:
	lbrace CharSyms rbrace
	;

CharSyms:
	CharsDefn
	| CharSyms comma CharsDefn
	;

CharsDefn:
	cstring
	| Quadruple
	| Tuple
	| DefinedValue
	;

Quadruple:
	lbrace Group comma Plane comma Row comma Cell rbrace
	;

Group:
	number
	;

Plane:
	number
	;

Row:
	number
	;

Cell:
	number
	;

Tuple:
	lbrace TableColumn comma TableRows rbrace
	;

TableColumn:
	number
	;

TableRows:
	number
	;

XMLRestrictedCharacterStringValue:
	xmlcstring
	;

UnrestrictedCharacterStringType:
	TOK_CHARACTER { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); } STRING
	;

UnrestrictedCharacterStringValue:
	SequenceValue
	;

XMLUnrestrictedCharacterStringValue:
	XMLSequenceValue
	;

UsefulType:
	typereference
	;

GeneralizedTime:
	lbracket UNIVERSAL "24" rbracket IMPLICIT
	VisibleString
	;

UTCTime:
	lbracket UNIVERSAL "23" rbracket IMPLICIT
	VisibleString
	;

ObjectDescriptor:
	lbracket UNIVERSAL "7" rbracket IMPLICIT
	GraphicString
	;

ConstrainedType:
	Type Constraint
	| TypeWithConstraint
	;

TypeWithConstraint:
	SET Constraint OF TypeOrNamedType
	| SET SizeConstraint OF TypeOrNamedType
	SEQUENCE Constraint OF TypeOrNamedType
	| SEQUENCE SizeConstraint OF TypeOrNamedType
	;

Constraint:
	lparen ConstraintSpec ExceptionSpec rparen
	;

ConstraintSpec:
	SubtypeConstraint
	*//* | GeneralConstraint *//*
	;

SubtypeConstraint:
	ElementSetSpecs
	;

ElementSetSpecs:
	RootElementSetSpec
	| RootElementSetSpec comma ELIPSIS
	| RootElementSetSpec comma ELIPSIS comma AdditionalElementSetSpec
	;

RootElementSetSpec:
	ElementSetSpec
	;

AdditionalElementSetSpec:
	ElementSetSpec
	;

ElementSetSpec:
	Unions
	| ALL Exclusions
	;

Unions:
	Intersections
	| UElems UnionMark Intersections
	;

UElems:
	Unions
	;

Intersections:
	IntersectionElements
	| IElems IntersectionMark IntersectionElements
	;

IElems:
	Intersections
	;

IntersectionElements:
	Elements
	| Elems Exclusions
	;

Elems:
	Elements
	;

Exclusions:
	TOK_EXCEPT { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	Elements
	;

UnionMark:
	TOK_BAR { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	| TOK_UNION { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	;

IntersectionMark:
	TOK_CARET { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	| TOK_INTERSECTION { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	;

Elements:
	SubtypeElements
	*//* | ObjectSetElements *//*
	| lparen ElementSetSpec rparen
	;

SubtypeElements:
	SingleValue
	| ContainedSubtype
	| ValueRange
	| PermittedAlphabet
	| SizeConstraint
	| TypeConstraint
	| InnerTypeConstraints
	| PatternConstraint
	;

TypeConstraint:
	Type
	;

PatternConstraint:
	TOK_PATTERN { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	Value
	;

Subtype:
	ParentType SubtypeSpec
	| SET SizeConstraint
	OF
	Type
	| SEQUENCE
	SizeConstraint OF
	Type
	;

ParentType:
	Type
	;

SubtypeSpec:
	lparen SubtypeValueSet SubtypeValueSetList rparen
	;

SubtypeValueSetList:
	| '|'
	| SubtypeValueSet SubtypeValueSetList
	;

SubtypeValueSet:
	SingleValue
	| ContainedSubtype
	| ValueRange
	| PermittedAlphabet
	| SizeConstraint
	| InnerTypeConstraints
	;

SingleValue:
	Value
	;

ContainedSubtype:
	Includes Type
	;

Includes:
	| TOK_INCLUDES { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	;

ValueRange:
	LowerEndPoint TOK_PERIOD { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	TOK_PERIOD { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	UpperEndPoint
	;

LowerEndPoint:
	LowerEndValue
	| LowerEndValue langle
	;

UpperEndPoint:
	UpperEndValue
	| langle UpperEndValue
	;

LowerEndValue:
	Value
	| TOK_MIN { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	;

UpperEndValue:
	Value
	| TOK_MAX { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	;

SizeConstraint:
	TOK_SIZE { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	SubtypeSpec
	| TOK_SIZE { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	Constraint
	;

PermittedAlphabet:
	FROM SubtypeSpec
	| FROM Constraint
	;

InnerTypeConstraints:
	TOK_WITH { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	TOK_COMPONENT { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	SingleTypeConstraint
	| TOK_WITH { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	TOK_COMPONENTS { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	MultipleTypeConstraint
	;

SingleTypeConstraint:
	SubtypeSpec
	| Constraint
	;

MultipleTypeConstraint:
	FullSpecification
	| PartialSpecification
	;

FullSpecification:
	lbrace TypeConstraints rbrace
	;

PartialSpecification:
	lbrace ELIPSIS comma TypeConstraints rbrace
	;

TypeConstraints:
	NamedConstraint
	| NamedConstraint comma TypeConstraints
	;

NamedConstraint:
	identifier ComponentConstraint
	| ComponentConstraint
	;

ComponentConstraint:
	ValueConstraint PresenceConstraint
	;

ValueConstraint:
	| SubtypeSpec
	| Constraint
	;

PresenceConstraint:
	| TOK_PRESENT { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	| TOK_ABSENT { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	| OPTIONAL
	;

ExceptionSpec:
	| TOK_EXCLAMATION { gdmo_column += fprintf(yyout, "%.*s\t", yyleng, yytext); }
	ExceptionIdentification
	;

ExceptionIdentification:
	SignedNumber
	| DefinedValue
	| Type colon Value
	;
*/
/* ADDITIONAL C CODE */

%%

int
yyerror(const char *s)
{
	char *text = yytext;

	switch (*text) {
	case '\n':
		text = "\n";
		break;
	case '\t':
		text = "\t";
		break;
	case '\f':
		text = "\f";
		break;
	}
	fprintf(stderr, "error: %s:%d: [%s] %s.\n", currentfile, yylineno, text, s);
	return (0);
}

void
gdmo_newline(FILE * output)
{
	int i;

	fprintf(output, "\n");
	gdmo_column = 0;
	gdmo_incomment = 0;
	for (i = 0; i < gdmo_level; i++) {
		fprintf(output, "\t");
		gdmo_column += 8;
	}
}

void
gdmo_checkcomment(FILE *output)
{
	if (gdmo_incomment)
		gdmo_newline(output);
}

void
gdmo_fprintwrap(FILE *output, const char *str, int len, int width)
{
	int col, n, ind;
	const char *tok = str, *end = str + len;

	/* First let's start at zero. */
	// gdmo_newline(output);
	ind = 8*gdmo_level;
	col = gdmo_column - ind;
	while ((tok < end)) {
		if ((n = strspn(tok, " \t\n")))
			tok += n;
		if ((n = strcspn(tok, " \t\n\0"))) {
			if (col + ind + n + 1 > width) {
				gdmo_newline(output);
				col = 0;
			}
			if (col != 0) {
				gdmo_column += fprintf(output, " ");
				col += 1;
			}
			gdmo_column += fwrite(tok, 1, n, output);
			col += n;
			tok += n;
		}
	}
}

/** @brief main
  * @param argc number of arguments.
  * @param argv the array of arguments.
  * @internal
  *
  * Simple main simply initializes yyin to stdin and yyout to stdout and runs the parser which
  * should pretty print the GDMO read on the input to the output.
  */
int
main(int argc, char *argv[])
{
	int c;

	for (;;) {
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"filename",	required_argument,	NULL,	'f'},
			{"output",	required_argument,	NULL,	'o'},
			{"debug",	no_argument,		NULL,	'D'},
			{NULL,		0,			NULL,	 0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, ":W:f:o:D", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 'f':
			if (optarg == NULL)
				goto missing_arg;
			strncpy(filename, optarg, sizeof(filename));
			break;
		case 'o':
			if (optarg == NULL)
				goto missing_arg;
			strncpy(outpfile, optarg, sizeof(outpfile));
			break;
		case 'D':
			yydebug = 1;
			break;
		case ':':
		      missing_arg:
			optind--;
			fprintf(stderr, "%s: missing argument -- %s\n", argv[0], argv[optind]);
			exit(2);
		case '?':
		default:
		      syntax_error:
			optind--;
			fprintf(stderr, "%s: illegal syntax -- %s\n", argv[0], argv[optind]);
			exit(2);
		}
	}
	yy_flex_debug = yydebug;

	if (outpfile[0] != '\0') {
		if ((yyout = fopen(outpfile, "w")) == NULL) {
			perror(basename(argv[0]));
			exit(1);
		}
	} else {
		yyout = stdout;
	}
	if (filename[0] != '\0') {
		if (optind < argc)
			goto syntax_error;
		currentfile = filename;
		if ((yyin = fopen(currentfile, "r")) == NULL) {
			perror(basename(argv[0]));
			exit(1);
		}
		yylineno = 1;
		yyrestart(yyin);
		if (yyparse() != 0)
			exit(1);
		fclose(yyin);
		exit(0);
	}
	if (optind < argc) {
		for (; optind < argc; optind++) {
			currentfile = argv[optind];
			if ((yyin = fopen(currentfile, "r")) == NULL) {
				perror(basename(argv[0]));
				continue;
			}
			yylineno = 1;
			yyrestart(yyin);
			if (yyparse() != 0)
				exit(1);
			fclose(yyin);
		}
		exit(0);
	}
	currentfile = "(stdin)";
	yyin = stdin;
	yylineno = 1;
	yyrestart(yyin);
	if (yyparse() != 0)
		exit(1);
	exit(0);
}

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS formatoptions+=tcqlor
