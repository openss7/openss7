/*****************************************************************************

 @(#) $Id: asnyac.y,v 0.9.2.3 2008-04-29 00:02:06 brian Exp $

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

 -----------------------------------------------------------------------------

 Last Modified $Date: 2008-04-29 00:02:06 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: asnyac.y,v $
 Revision 0.9.2.3  2008-04-29 00:02:06  brian
 - updated headers for release

 Revision 0.9.2.2  2008-04-11 16:58:44  brian
 - removed deprecated NEXUSWARE support

 Revision 0.9.2.1  2007/09/06 10:59:13  brian
 - added new gdmo files

 *****************************************************************************/

/* C DECLARATIONS */

%{

#ident "@(#) $RCSfile: asnyac.y,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-04-29 00:02:06 $"

static char const ident[] = "$RCSfile: asnyac.y,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2008-04-29 00:02:06 $";

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

#include "asnyac.h"

#if !defined YYENABLE_NLS
#define YYENABLE_NLS 0
#endif

int yydebug = 0;
int yyerror(const char *);

extern int yylex(void);
extern int yylineno;
extern char *yytext;

extern FILE *yyin, *yyout;

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

%token <str> TOK_BOOLEAN
%token <str> TOK_INTEGER
%token <str> TOK_BIT
%token <str> TOK_STRING
%token <str> TOK_OCTET
%token <str> TOK_NULL
%token <str> TOK_SEQUENCE
%token <str> TOK_OF
%token <str> TOK_SET
%token <str> TOK_IMPLICIT
%token <str> TOK_CHOICE
%token <str> TOK_ANY
%token <str> TOK_EXTERNAL
%token <str> TOK_OBJECT
%token <str> TOK_IDENTIFIER
%token <str> TOK_OPTIONAL
%token <str> TOK_DEFAULT
%token <str> TOK_COMPONENTS
%token <str> TOK_UNIVERSAL
%token <str> TOK_APPLICATION
%token <str> TOK_PRIVATE
%token <str> TOK_TRUE
%token <str> TOK_FALSE
%token <str> TOK_BEGIN
%token <str> TOK_END
%token <str> TOK_DEFINITIONS
%token <str> TOK_EXPLICIT
%token <str> TOK_ENUMERATED
%token <str> TOK_EXPORTS
%token <str> TOK_IMPORTS
%token <str> TOK_REAL
%token <str> TOK_INCLUDES
%token <str> TOK_MIN
%token <str> TOK_MAX
%token <str> TOK_SIZE
%token <str> TOK_FROM
%token <str> TOK_WITH
%token <str> TOK_COMPONENT
%token <str> TOK_PRESENT
%token <str> TOK_ABSENT
%token <str> TOK_DEFINED
%token <str> TOK_BY
%token <str> TOK_PLUS_INFINITY
%token <str> TOK_MINUS_INFINITY
%token <str> TOK_TAGS
%token <str> TOK_ASSIGNMENT
%token <str> TOK_OPEN_BRACE
%token <str> TOK_CLOSE_BRACE
%token <str> TOK_LEFT_ANGLE
%token <str> TOK_COMMA
%token <str> TOK_PERIOD
%token <str> TOK_OPEN_PAREN
%token <str> TOK_CLOSE_PAREN
%token <str> TOK_OPEN_BRACKET
%token <str> TOK_CLOSE_BRACKET
%token <str> TOK_HYPHEN
%token <str> TOK_SEMICOLON
%token <str> TOK_reference
%token <str> TOK_bstring
%token <str> TOK_hstring
%token <str> TOK_cstring
%token <str> TOK_identifier
%token <str> TOK_number

/* GRAMAR RULES */

%%

ModuleDefinition:
    ModuleIdentifier
    TOK_DEFINITIONS
    TagDefault
    TOK_ASSIGNMENT
    TOK_BEGIN
    ModuleBody
    TOK_END

TagDefault:
    | TOK_EXPLICIT TOK_TAGS
    | TOK_IMPLICIT TOK_TAGS
    ;

ModuleIdentifier:
    modulereference
    AssignedIdentifier
    ;

modulereference:
    reference
    ;

reference:
    TOK_reference
    ;

AssignedIdentifier:
    | ObjectIdentifierValue
    ;

ModuleBody:
    | Exports Imports AssignmentList
    ;

Exports:
    | TOK_EXPORTS SymbolsExported TOK_SEMICOLON
    ;

SymbolsExported:
    | SymbolList
    ;

Imports:
    | TOK_IMPORTS SymbolsImported TOK_SEMICOLON
    ;

SymbolsImported:
    | SymbolsFromModuleList
    ;

SymbolsFromModuleList:
    SymbolsFromModule SymbolsFromModuleList
    | SymbolsFromModule
    ;

SymbolsFromModule:
    SymbolList TOK_FROM ModuleIdentifier
    ;

SymbolList:
    Symbol TOK_COMMA SymbolList
    | Symbol
    ;

Symbol:
    reference
    | identifier
    ;

AssignmentList:
    Assignment AssignmentList
    | Assignment
    ;

Assignment:
    TypeAssignment
    | ValueAssignment
    ;

typereference:
    TOK_identifier
    ;

Externaltypereference:
    modulereference
    typereference
    ;

valuereference:
    reference
    ;

Externalvaluereference:
    modulereference
    valuereference
    ;

DefinedType:
    Externaltypereference
    | typereference
    ;

DefinedValue:
    Externalvaluereference
    | valuereference
    ;

TypeAssignment:
    typereference TOK_ASSIGNMENT Type
    ;

ValueAssignment:
    valuereference TOK_ASSIGNMENT Value
    ;

Type:
    BuiltinType
    | DefinedType
    | Subtype
    ;

BuiltinType:
    BooleanType
    | IntegerType
    | BitStringType
    | OctetStringType
    | NullType
    | SequenceType
    | SequenceOfType
    | SetType
    | SetOfType
    | ChoiceType
    | SelectionType
    | TaggedType
    | AnyType
    | ObjectIdentifierType
    | CharacterStringType
    | UsefulType
    | EnumeratedType
    | RealType
    ;

identifier:
    TOK_identifier
    ;

NamedType:
    identifier Type
    | Type
    | SelectionType
    ;

Value:
    BuiltinValue | DefinedValue
    ;

BuiltinValue:
    BooleanValue
    | IntegerValue
    | BitStringValue
    | OctetStringValue
    | NullValue
    | SequenceValue
    | SequenceOfValue
    | SetValue
    | SetOfValue
    | ChoiceValue
    | SelectionValue
    | TaggedValue
    | AnyValue
    | ObjectIdentifierValue
    | CharacterStringValue
    | EnumeratedValue
    | RealValue
    ;

NamedValue:
    identifier Value
    | Value
    ;

BooleanType:
    TOK_BOOLEAN
    ;

BooleanValue:
    TOK_TRUE | TOK_FALSE
    ;

IntegerType:
    TOK_INTEGER OptionalNamedNumberList
    ;

OptionalNamedNumberList:
    | TOK_OPEN_BRACE NamedNumberList TOK_CLOSE_BRACE
    ;

NamedNumberList:
    NamedNumber
    | NamedNumberList TOK_COMMA NamedNumber
    ;

NamedNumber:
    identifier TOK_OPEN_PAREN SignedNumberOrDefinedValue TOK_CLOSE_PAREN
    ;

SignedNumberOrDefinedValue:
    SignedNumber
    | DefinedValue
    ;

number:
    TOK_number
    ;

SignedNumber:
    number
    | TOK_HYPHEN number
    ;

IntegerValue:
    SignedNumber
    | identifier
    ;

EnumeratedType:
    TOK_ENUMERATED TOK_OPEN_BRACE Enumeration TOK_CLOSE_BRACE
    ;

Enumeration:
    NamedNumber
    | NamedNumber TOK_COMMA Enumeration
    ;

EnumeratedValue:
    identifier
    ;

RealType:
    TOK_REAL
    ;

RealValue:
    NumericRealValue
    | SpecialRealValue
    ;

NumericRealValue:
    TOK_OPEN_BRACE Mantissa TOK_COMMA Base TOK_COMMA Exponent TOK_CLOSE_BRACE
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
    TOK_PLUS_INFINITY
    | TOK_MINUS_INFINITY
    ;

BitStringType:
    TOK_BIT TOK_STRING OptionalNamedBitList
    ;

OptionalNamedBitList:
    | TOK_OPEN_BRACE NamedBitList TOK_CLOSE_BRACE
    ;

NamedBitList:
    NamedBit
    | NamedBitList TOK_COMMA NamedBit
    ;

NamedBit:
    identifier TOK_OPEN_PAREN NumberOrDefinedValue TOK_CLOSE_PAREN
    ;

NumberOrDefinedValue:
    number
    | DefinedValue
    ;

bstring:
    TOK_bstring
    ;

hstring:
    TOK_hstring
    ;

cstring:
    TOK_cstring
    ;

BitStringValue:
    bstring
    | hstring
    | TOK_OPEN_BRACE OptionalIdentifierList TOK_CLOSE_BRACE
    ;

OptionalIdentifierList:
    | IdentifierList
    ;

IdentifierList:
    identifier
    | IdentifierList TOK_COMMA identifier
    ;

OctetStringType:
    TOK_OCTET TOK_STRING
    ;

OctetStringValue:
    bstring
    | hstring
    ;

NullType:
    TOK_NULL
    ;

NullValue:
    TOK_NULL
    ;

SequenceType:
    TOK_SEQUENCE TOK_OPEN_BRACE OptionalElementTypeList TOK_CLOSE_BRACE
    ;

OptionalElementTypeList:
    | ElementTypeList
    ;

ElementTypeList:
    ElementType
    | ElementTypeList TOK_COMMA ElementType
    ;

ElementType:
    NamedType
    | NamedType TOK_OPTIONAL
    | NamedType TOK_DEFAULT Value
    | TOK_COMPONENTS TOK_OF Type
    ;

SequenceValue:
    TOK_OPEN_BRACE OptionalElementValueList TOK_CLOSE_BRACE
    ;

OptionalElementValueList:
    | ElementValueList
    ;

ElementValueList:
    NamedValue
    | ElementValueList TOK_COMMA NamedValue
    ;

SequenceOfType:
    TOK_SEQUENCE TOK_OF Type
    | TOK_SEQUENCE
    ;

SequenceOfValue:
    TOK_OPEN_BRACE OptionalValueList TOK_CLOSE_BRACE
    ;

OptionalValueList:
    | ValueList
    ;

ValueList:
    Value
    | ValueList TOK_COMMA Value
    ;

SetType:
    TOK_SET TOK_OPEN_BRACE OptionalElementTypeList TOK_CLOSE_BRACE
    ;

SetValue:
    TOK_OPEN_BRACE OptionalElementValueList TOK_CLOSE_BRACE
    ;

SetOfType:
    TOK_SET TOK_OF Type
    | TOK_SET
    ;

SetOfValue:
    TOK_OPEN_BRACE OptionalValueList TOK_CLOSE_BRACE
    ;

ChoiceType:
    TOK_CHOICE TOK_OPEN_BRACE AlternativeTypeList TOK_CLOSE_BRACE
    ;

AlternativeTypeList:
    NamedType
    | AlternativeTypeList TOK_COMMA NamedType
    ;

ChoiceValue:
    NamedValue
    ;

SelectionType:
    identifier TOK_LEFT_ANGLE Type
    ;

SelectionValue:
    NamedValue
    ;

TaggedType:
    Tag Type
    | Tag TOK_IMPLICIT Type
    | Tag TOK_EXPLICIT Type
    ;

Tag:
   TOK_OPEN_BRACKET Class ClassNumber TOK_CLOSE_BRACKET
   ;

ClassNumber:
    number
    | DefinedValue
    ;

Class:
    | TOK_UNIVERSAL
    | TOK_APPLICATION
    | TOK_PRIVATE
    ;

TaggedValue:
    Value
    ;

AnyType:
    TOK_ANY
    | TOK_ANY TOK_DEFINED TOK_BY identifier
    ;

AnyValue:
    Type Value
    ;

ObjectIdentifierType:
    TOK_OBJECT TOK_IDENTIFIER
    ;

ObjectIdentifierValue:
    TOK_OPEN_BRACE DefinedValueObjIdComponentList TOK_CLOSE_BRACE
    ;

DefinedValueObjIdComponentList:
    ObjIdComponentList
    | DefinedValue ObjIdComponentList
    ;

ObjIdComponentList:
    ObjIdComponent
    | ObjIdComponent ObjIdComponentList
    ;

ObjIdComponent:
    NameForm
    | NumberForm
    | NameAndNumberForm
    ;

NameForm:
    identifier
    ;

NumberForm:
    number
    | DefinedValue
    ;

NameAndNumberForm:
    identifier TOK_OPEN_PAREN NumberForm TOK_CLOSE_PAREN
    ;

CharacterStringType:
    typereference
    ;

CharacterStringValue:
    cstring
    ;

UsefulType:
    typereference
    ;

VisibleString:
    cstring
    ;

GeneralizedTime:
    TOK_OPEN_BRACKET TOK_UNIVERSAL "24" TOK_CLOSE_BRACKET TOK_IMPLICIT
    VisibleString
    ;

UTCTime:
    TOK_OPEN_BRACKET TOK_UNIVERSAL "23" TOK_CLOSE_BRACKET TOK_IMPLICIT
    VisibleString
    ;

ExternalType:
    TOK_EXTERNAL
    ;

GraphicString:
    cstring
    ;

ObjectDescriptor:
    TOK_OPEN_BRACKET TOK_UNIVERSAL "7" TOK_CLOSE_BRACKET TOK_IMPLICIT
    GraphicString
    ;

Subtype:
    ParentType SubtypeSpec
    | TOK_SET SizeConstraint TOK_OF Type
    | TOK_SEQUENCE SizeConstraint TOK_OF Type
    ;

ParentType:
    Type
    ;

SubtypeSpec:
    TOK_OPEN_PAREN SubtypeValueSet SubtypeValueSetList TOK_CLOSE_PAREN
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
    TOK_INCLUDES Type
    ;

ValueRange:
    LowerEndPoint TOK_PERIOD TOK_PERIOD UpperEndPoint
    ;

LowerEndPoint:
    LowerEndValue
    | LowerEndValue TOK_LEFT_ANGLE
    ;

UpperEndPoint:
    UpperEndValue
    | TOK_LEFT_ANGLE UpperEndValue
    ;

LowerEndValue:
    Value
    | TOK_MIN
    ;

UpperEndValue:
    Value
    | TOK_MAX
    ;

SizeConstraint:
    TOK_SIZE SubtypeSpec
    ;

PermittedAlphabet:
    TOK_FROM SubtypeSpec
    ;

InnerTypeConstraints:
    TOK_WITH TOK_COMPONENT SingleTypeConstraint
    | TOK_WITH TOK_COMPONENTS MultipleTypeConstraint
    ;

SingleTypeConstraint:
    SubtypeSpec
    ;

MultipleTypeConstraint:
    FullSpecification
    | PartialSpecification
    ;

FullSpecification:
    TOK_OPEN_BRACE TypeConstraints TOK_CLOSE_BRACE
    ;

PartialSpecification:
    TOK_OPEN_BRACE TOK_PERIOD TOK_PERIOD TOK_PERIOD TOK_COMMA TypeConstraints TOK_CLOSE_BRACE
    ;

TypeConstraints:
    NamedConstraint
    | NamedConstraint TOK_COMMA TypeConstraints
    ;

NamedConstraint:
    identifier Constraint
    | Constraint
    ;

Constraint:
    ValueConstraint PresenceConstraint
    ;

ValueConstraint:
    | SubtypeSpec
    ;

PresenceConstraint:
    | TOK_PRESENT
    | TOK_ABSENT
    | TOK_OPTIONAL
    ;


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
	fprintf(stderr, "error: %s:%d: [%s] %s.\n", filename, yylineno, text, s);
	return (0);
}

void
yyinit(void)
{
	yyin = stdin;
	yyout = stdout;
	yyrestart(yyin);
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
	yyinit();
	while (yyparse() == 0) ;
	exit(0);
}

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS formatoptions+=tcqlor
