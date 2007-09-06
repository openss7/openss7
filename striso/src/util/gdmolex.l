/*****************************************************************************

 @(#) $RCSfile: gdmolex.l,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/09/06 10:59:13 $

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

 Last Modified $Date: 2007/09/06 10:59:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: gdmolex.l,v $
 Revision 0.9.2.1  2007/09/06 10:59:13  brian
 - added new gdmo files

 *****************************************************************************/

/* C DECLARATIONS */

%{

#ident "@(#) $RCSfile: gdmolex.l,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/09/06 10:59:13 $"

static char const ident[] = "$RCSfile: gdmolex.l,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/09/06 10:59:13 $";

#include <unistd.h> /* isatty(), official flex 2.5.4a skeleton misses this */

#include <stdlib.h>
#include <string.h>

#include "gdmoyac.h"

#define ECHO \
	(int)({ \
		gdmo_column += fwrite(yytext, 1, yyleng, yyout); \
	})

extern void gdmo_newline(FILE *);
extern int gdmo_level;
extern int gdmo_column;
extern int gdmo_incomment;


%}

/* FLEX DECLARATIONS */

%pointer

%option nolex-compat
%option warn
%option debug
%option verbose
/* %option perf-report */
%option default
%option case-sensitive
%option 8bit
%option interactive
%option noalways-interactive
%option nonever-interactive
%option nostdinit
%option stack
%option nomain
%option noyywrap
%option yylineno

EOL		\n

/*
 * A comment is introduced by the character pair `--' and is terminated by the
 * chracter pair `--' or by the end of a line, whichever comes first.  A comment
 * is equivalent to a space for the purposes of interpreting the templates
 * defined in X.722.  Comments have no normative significance.
 */

COMPAIR		"--"
COMEND		({COMPAIR})|({EOL})
COMSTRING	[^\-\n]+
COMMENT		{COMPAIR}{COMSTRING}{COMEND}

/*
 * Whenever it is necessary to include text in a template, the text is included
 * in the form of a string of characters, optionally starting and ending with a
 * text-delimiter character chosen from the following characters: !"#$%^&'`~?@\
 * If a text-delimiter character is used, the same character shall be used at
 * the start and end of the string, and wherever that text-delimiter character
 * appears in the body of the text string, it sha be replaced by two occurences
 * of that character.  If a text-delimiter character is not used, then the text
 * string shall not contain any punctuation character that is a valid successor
 * to the text string in the template definition in which the text string is
 * used.  The supported productions:
 *
 *   delimited-string -> text-delimiter <text-string> text-delimiter | <text-string>
 *
 *   text-delimiter -> ! | " | # | $ | % | ^ | & | * | ' | ` | ~ | ? | @ | \
 *
 * and therefore assumed for all templates that permit the user of a
 * delimited-string, where text-string is an arbitrary sequence of characters,
 * and if the text-delimiter has been used, all occurences of that delimiter
 * character in text-string have been replaced by a pair of text-delimiter
 * characters.
 *
 * With the exception of the rules pertaining to the use of delimiters, the
 * internal structure of text-string, and in particular, the use of the comment
 * structure defined in X.722, has no significance with respect to the
 * provisions of X.722.  The meaning of the entire text-string must therefore be
 * assumed to carry normative significance, unless explicitly stated otherwise
 * in the document in which the notation is used.
 *
 * To accomplish these we need to enter a delimited_string condition when any of
 * the following are encountered: TOK_PRESENT_IF, TOK_PRESENT_ONLY_IF,
 * TOK_DESCRIPTION, TOK_DEFINED_AS.  In the TOK_DESCRIPTION and TOK_DEFINED_AS
 * cases, the condition can be exited upon encountering a `;'.  In the
 * TOK_PRESENT_IF and TOK_PRESENT_ONLY_IF case, the condition can be exited upon
 * encountering a ',' or ';'.
 */

D1 [\!](([^\!])|([\!][\!]))*[\!]
D2 [\"](([^\"])|([\"][\"]))*[\"]
D3 [\#](([^\#])|([\#][\#]))*[\#]
D4 [\$](([^\$])|([\$][\$]))*[\$]
D5 [\%](([^\%])|([\%][\%]))*[\%]
D6 [\^](([^\^])|([\^][\^]))*[\^]
D7 [\&](([^\&])|([\&][\&]))*[\&]
D8 [\*](([^\*])|([\*][\*]))*[\*]
D9 [\'](([^\'])|([\'][\']))*[\']
DA [\`](([^\`])|([\`][\`]))*[\`]
DB [\~](([^\~])|([\~][\~]))*[\~]
DC [\?](([^\?])|([\?][\?]))*[\?]
DD [\@](([^\@])|([\@][\@]))*[\@]
DE [\\](([^\\])|([\\][\\]))*[\\]
DS ({D1})|({D2})|({D3})|({D4})|({D5})|({D6})|({D7})|({D8})|({D9})|({DA})|({DB})|({DC})|({DD})|({DE})
DELIMITED_STRING  {DS}
TEXT_STRING1 [^\!\"\#\$\%\^\&\*\'\`\~\?\@\\\;]([^\;]|[\n])+
TEXT_STRING2 [^\!\"\#\$\%\^\&\*\'\`\~\?\@\\\,\;]([^\,\;]|[\n])+

/*
 * A label-string may include any number of the following characters: 1) upper
 * or lower case alphabetic characters; 2) the digits 0-9; 3) the character `-';
 * 4) the character `/'; in any order, commencing with a lower case alphabetic
 * character, with the exception that the character pair "--" shall not appear
 * in a label-string.
 */

IDLABEL		[a-zA-Z][a-zA-Z0-9\/\-]*
LABEL		[a-z][a-zA-Z0-9\/\-]*
EXTERNREF	[a-zA-Z][a-zA-Z0-9/-]"."[a-zA-Z][a-zA-Z0-9/-]
WHITESPACE	[ \t\n]+
SEMICOLON	";"
COMMA		","
COLON		":"
PERIOD		"."
DOUBLEQUOTE	["]
DOCID		["][^"]*["][ \t\n]*":"[ \t\n]*[a-zA-Z][a-zA-Z0-9\/\-]*
LBRACE		"{"
RBRACE		"}"
LPAREN		"("
RPAREN		")"
LDBRACKET	"[["
RDBRACKET	"]]"
LBRACKET	"["
RBRACKET	"]"
NUMBER		[0-9]+
NAMEANDNUMBER	[a-zA-Z][a-zA-Z0-9\/\-]*"("(([0-9]+)|([a-z][a-zA-Z0-9\/\-]*))")"
HYPHEN		"-"
LANGLE		"<"
RANGLE		">"
EQUALS		"="
APOSTROPHE	"'"
AT		"@"
BAR		"|"
EXCLAMATION	"!"
CARET		"^"
ASTERISK	"*"
AMPERSAND	"&"
SLASH		"/"
ELIPSIS		"..."

XMLOPEN		"<&"
XMLCLOSE	"/>"
XMLCOPEN	"</&"

CLASS		MANAGED[- ]OBJECT[- ]CLASS
PACKAGE		PACKAGE
PARAMETER	PARAMETER
BINDING		NAME[- ]BINDING
ATTRIBUTE	ATTRIBUTE
GROUP		ATTRIBUTE[- ]GROUP
BEHAVIOUR	BEHAVIOUR
ACTION		ACTION
NOTIFICATION	NOTIFICATION

DEFAULT		.

%x ds1 ds2 com asn label id

%%

{LABEL}[ \t\n]+{CLASS}			{ yyless(strcspn(yytext, " \t\n")); return TOK_MANAGED_OBJECT_CLASS_LABEL	; }
{LABEL}[ \t\n]+{PACKAGE}		{ yyless(strcspn(yytext, " \t\n")); return TOK_PACKAGE_LABEL			; }
{LABEL}[ \t\n]+{PARAMETER}		{ yyless(strcspn(yytext, " \t\n")); return TOK_PARAMETER_LABEL			; }
{LABEL}[ \t\n]+{BINDING}		{ yyless(strcspn(yytext, " \t\n")); return TOK_NAME_BINDING_LABEL		; }
{LABEL}[ \t\n]+{ATTRIBUTE}		{ yyless(strcspn(yytext, " \t\n")); return TOK_ATTRIBUTE_LABEL			; }
{LABEL}[ \t\n]+{GROUP}			{ yyless(strcspn(yytext, " \t\n")); return TOK_ATTRIBUTE_GROUP_LABEL		; }
{LABEL}[ \t\n]+{BEHAVIOUR}		{ yyless(strcspn(yytext, " \t\n")); return TOK_BEHAVIOUR_LABEL			; }
{LABEL}[ \t\n]+{ACTION}			{ yyless(strcspn(yytext, " \t\n")); return TOK_ACTION_LABEL			; }
{LABEL}[ \t\n]+{NOTIFICATION}		{ yyless(strcspn(yytext, " \t\n")); return TOK_NOTIFICATION_LABEL		; }

ACTION[- ]INFO				{ return TOK_ACTION_INFO			; }
ACTION[- ]REPLY				{ return TOK_ACTION_REPLY			; }
ACTIONS					{ return TOK_ACTIONS				; }
ADD[- ]REMOVE				{ return TOK_ADD_REMOVE				; }
ADD					{ return TOK_ADD				; }
AND[- ]ATTRIBUTE[- ]IDS			{ return TOK_AND_ATTRIBUTE_IDS			; }
AND[- ]SUBCLASSES			{ return TOK_AND_SUBCLASSES			; }
ATTRIBUTE[- ]GROUPS			{ return TOK_ATTRIBUTE_GROUPS			; }
ATTRIBUTES				{ return TOK_ATTRIBUTES				; }
CHARACTERIZED[- ]BY			{ return TOK_CHARACTERIZED_BY			; }
CONDITIONAL[- ]PACKAGES			{ return TOK_CONDITIONAL_PACKAGES		; }
CONTEXT					{ return TOK_CONTEXT				; }
CREATE					{ return TOK_CREATE				; }
DEFAULT[- ]VALUE			{ return TOK_DEFAULT_VALUE			; }
DEFINED[- ]AS[ \t\n]*			{ yy_push_state(ds1); return TOK_DEFINED_AS	; }
DELETES[- ]CONTAINED[- ]OBJECTS		{ return TOK_DELETES_CONTAINED_OBJECTS		; }
DELETE					{ return TOK_DELETE				; }
DERIVATION[- ]RULE			{ return TOK_DERIVATION_RULE			; }
DERIVED[- ]FROM				{ return TOK_DERIVED_FROM			; }
DESCRIPTION[ \t\n]*			{ yy_push_state(ds1); return TOK_DESCRIPTION	; }
EQUALITY				{ return TOK_EQUALITY				; }
EVENT[- ]INFO				{ return TOK_EVENT_INFO				; }
EVENT[- ]REPLY				{ return TOK_EVENT_REPLY			; }
FIXED					{ return TOK_FIXED				; }
GET[- ]REPLACE				{ return TOK_GET_REPLACE			; }
GET					{ return TOK_GET				; }
GROUP[- ]ELEMENTS			{ return TOK_GROUP_ELEMENTS			; }
INITIAL[- ]VALUE			{ return TOK_INITIAL_VALUE			; }
MATCHES[- ]FOR				{ return TOK_MATCHES_FOR			; }
MODE([-]|[ \t\n]+)CONFIRMED		{ return TOK_MODE_CONFIRMED			; }
NAMED[- ]BY				{ return TOK_NAMED_BY				; }
SUPERIOR[- ]OBJECT[- ]CLASS		{ return TOK_SUPERIOR_OBJECT_CLASS		; }
NO[- ]MODIFY				{ return TOK_NO_MODIFY				; }
NOTIFICATIONS				{ return TOK_NOTIFICATIONS			; }
ONLY[- ]IF[- ]NO[- ]CONTAINED[- ]OBJECTS { return TOK_ONLY_IF_NO_CONTAINED_OBJECTS	; }
ORDERING				{ return TOK_ORDERING				; }
PARAMETERS				{ return TOK_PARAMETERS				; }
PERMITTED[- ]VALUES			{ return TOK_PERMITTED_VALUES			; }
PRESENT[- ]IF[ \t\n]*			{ yy_push_state(ds2); return TOK_PRESENT_IF	; }
PRESENT[- ]ONLY[- ]IF[ \t\n]*		{ yy_push_state(ds2); return TOK_PRESENT_ONLY_IF; }
REGISTERED[- ]AS			{ return TOK_REGISTERED_AS			; }
REMOVE					{ return TOK_REMOVE				; }
REPLACE[- ]WITH[- ]DEFAULT		{ return TOK_REPLACE_WITH_DEFAULT		; }
REPLACE					{ return TOK_REPLACE				; }
REQUIRED[- ]VALUES			{ return TOK_REQUIRED_VALUES			; }
SET[- ]BY[- ]CREATE			{ return TOK_SET_BY_CREATE			; }
SET[- ]COMPARISON			{ return TOK_SET_COMPARISON			; }
SET[- ]INTERSECTION			{ return TOK_SET_INTERSECTION			; }
SPECIFIC[- ]ERROR			{ return TOK_SPECIFIC_ERROR			; }
SUBORDINATE[- ]OBJECT[- ]CLASS		{ return TOK_SUBORDINATE			; }
SUBSTRINGS				{ return TOK_SUBSTRINGS				; }
WITH[- ]ATTRIBUTE[- ]SYNTAX		{ return TOK_WITH_ATTRIBUTE_SYNTAX		; }
WITH[- ]ATTRIBUTE			{ return TOK_WITH_ATTRIBUTE			; }
WITH[- ]AUTOMATIC[- ]INSTANCE[- ]NAMING	{ return TOK_WITH_AUTOMATIC_INSTANCE_NAMING	; }
WITH[- ]INFORMATION[- ]SYNTAX		{ return TOK_WITH_INFORMATION_SYNTAX		; }
WITH[- ]REFERENCE[- ]OBJECT		{ return TOK_WITH_REFERENCE_OBJECT		; }
WITH[- ]REPLY[- ]SYNTAX			{ return TOK_WITH_REPLY_SYNTAX			; }
WITH[- ]SYNTAX				{ return TOK_WITH_SYNTAX			; }

{CLASS}					{ return TOK_MANAGED_OBJECT_CLASS		; }
{PACKAGE}				{ return TOK_PACKAGE				; }
{PARAMETER}				{ return TOK_PARAMETER				; }
{BINDING}				{ return TOK_NAME_BINDING			; }
{ATTRIBUTE}				{ return TOK_ATTRIBUTE				; }
{GROUP}					{ return TOK_ATTRIBUTE_GROUP			; }
{BEHAVIOUR}				{ return TOK_BEHAVIOUR				; }
{ACTION}				{ return TOK_ACTION				; }
{NOTIFICATION}				{ return TOK_NOTIFICATION			; }

<asn>ABSTRACT[- ]SYNTAX			{ return TOK_ABSTRACT_SYNTAX			; }
<asn>ALL				{ return TOK_ALL				; }
<asn>AUTOMATIC				{ return TOK_AUTOMATIC				; }
<asn>BMPString				{ return TOK_BMPString				; }
<asn>CHARACTER				{ return TOK_CHARACTER				; }
<asn>CLASS				{ return TOK_CLASS				; }
<asn>CONSTRAINED			{ return TOK_CONSTRAINED			; }
<asn>CONTAINING				{ return TOK_CONTAINING				; }
<asn>EMBEDDED				{ return TOK_EMBEDDED				; }
<asn>ENCODED				{ return TOK_ENCODED				; }
<asn>EXCEPT				{ return TOK_EXCEPT				; }
<asn>EXTENSIBILITY			{ return TOK_EXTENSIBILITY			; }
<asn>GeneralizedTime			{ return TOK_GeneralizedTime			; }
<asn>GeneralString			{ return TOK_GeneralString			; }
<asn>GraphicString			{ return TOK_GraphicString			; }
<asn>IA5String				{ return TOK_IA5String				; }
<asn>IMPLIED				{ return TOK_IMPLIED				; }
<asn>INSTANCE				{ return TOK_INSTANCE				; }
<asn>INTERSECTION			{ return TOK_INTERSECTION			; }
<asn>ISO646String			{ return TOK_ISO646String			; }
<asn>NumericString			{ return TOK_NumericString			; }
<asn>ObjectDescriptor			{ return TOK_ObjectDescriptor			; }
<asn>PrintableString			{ return TOK_PrintableString			; }
<asn>PATTERN				{ return TOK_PATTERN				; }
<asn>PDV				{ return TOK_PDV				; }
<asn>RELATIVE[- ]OID			{ return TOK_RELATIVE_OID			; }
<asn>SYNTAX				{ return TOK_SYNTAX				; }
<asn>T61String				{ return TOK_T61String				; }
<asn>TeletexString			{ return TOK_TeletexString			; }
<asn>TYPE[- ]IDENTIFIER			{ return TOK_TYPE_IDENTIFIER			; }
<asn>UNION				{ return TOK_UNION				; }
<asn>UNIQUE				{ return TOK_UNIQUE				; }
<asn>UniversalString			{ return TOK_UniversalString			; }
<asn>UTCTime				{ return TOK_UTCTime				; }
<asn>UTF8String				{ return TOK_UTF8String				; }
<asn>VideotexString			{ return TOK_VideotexString			; }
<asn>VisibleString			{ return TOK_VisibleString			; }

<asn>ABSENT				{ return TOK_ABSENT				; }
<asn>ANY				{ return TOK_ANY				; }
<asn>APPLICATION			{ return TOK_APPLICATION			; }
<asn>BEGIN				{ return TOK_BEGIN				; }
<asn>BIT				{ return TOK_BIT				; }
<asn>BOOLEAN				{ return TOK_BOOLEAN				; }
<asn>BY					{ return TOK_BY					; }
<asn>CHOICE				{ return TOK_CHOICE				; }
<asn>COMPONENT				{ return TOK_COMPONENT				; }
<asn>COMPONENTS				{ return TOK_COMPONENTS				; }
<asn>DEFAULT				{ return TOK_DEFAULT				; }
<asn>DEFINED				{ return TOK_DEFINED				; }
<INITIAL>DEFINITIONS			{ yy_push_state(asn); return TOK_DEFINITIONS	; }
<asn>END				{ yy_pop_state(); return TOK_END		; }
<asn>ENUMERATED				{ return TOK_ENUMERATED				; }
<asn>EXPLICIT				{ return TOK_EXPLICIT				; }
<asn>EXPORTS				{ return TOK_EXPORTS				; }
<asn>EXTERNAL				{ return TOK_EXTERNAL				; }
<asn>FALSE				{ return TOK_FALSE				; }
<asn>FROM				{ return TOK_FROM				; }
<asn>IDENTIFIER				{ return TOK_IDENTIFIER				; }
<asn>IMPLICIT				{ return TOK_IMPLICIT				; }
<asn>IMPORTS				{ return TOK_IMPORTS				; }
<asn>INCLUDES				{ return TOK_INCLUDES				; }
<asn>INTEGER				{ return TOK_INTEGER				; }
<asn>MAX				{ return TOK_MAX				; }
<asn>MIN				{ return TOK_MIN				; }
<asn>MINUS[- ]INFINITY			{ return TOK_MINUS_INFINITY			; }
<asn>NULL				{ return TOK_NULL				; }
<asn>OBJECT				{ return TOK_OBJECT				; }
<asn>OCTET				{ return TOK_OCTET				; }
<asn>OF					{ return TOK_OF					; }
<asn>OPTIONAL				{ return TOK_OPTIONAL				; }
<asn>PLUS[- ]INFINITY			{ return TOK_PLUS_INFINITY			; }
<asn>PRESENT				{ return TOK_PRESENT				; }
<asn>PRIVATE				{ return TOK_PRIVATE				; }
<asn>REAL				{ return TOK_REAL				; }
<asn>"::="				{ return TOK_ASSIGNMENT				; }
<asn>SEQUENCE				{ return TOK_SEQUENCE				; }
<asn>SET				{ return TOK_SET				; }
<asn>SIZE				{ return TOK_SIZE				; }
<asn>STRING				{ return TOK_STRING				; }
<asn>TAGS				{ return TOK_TAGS				; }
<asn>TRUE				{ return TOK_TRUE				; }
<asn>UNIVERSAL				{ return TOK_UNIVERSAL				; }
<asn>WITH				{ return TOK_WITH				; }

<asn>true				{ return TOK_true				; }
<asn>false				{ return TOK_false				; }

<INITIAL,asn>{SEMICOLON}		{ return TOK_SEMICOLON				; }
<INITIAL,asn>{COMMA}			{ return TOK_COMMA				; }
<INITIAL,asn>{COLON}			{ return TOK_COLON				; }
<asn>{ELIPSIS}				{ return TOK_ELIPSIS				; }
<INITIAL,asn>{PERIOD}			{ return TOK_PERIOD				; }
<INITIAL,asn>{DOUBLEQUOTE}		{ return TOK_DOUBLEQUOTE			; }
<INITIAL,asn>{LBRACE}			{ return TOK_LBRACE				; }
<INITIAL,asn>{RBRACE}			{ return TOK_RBRACE				; }
<INITIAL,asn>{LPAREN}			{ return TOK_LPAREN				; }
<INITIAL,asn>{RPAREN}			{ return TOK_RPAREN				; }
<asn>{LDBRACKET}			{ return TOK_LDBRACKET				; }
<asn>{RDBRACKET}			{ return TOK_RDBRACKET				; }
<INITIAL,asn>{LBRACKET}			{ return TOK_LBRACKET				; }
<INITIAL,asn>{RBRACKET}			{ return TOK_RBRACKET				; }
<asn>{XMLOPEN}				{ return TOK_XMLOPEN				; }
<asn>{XMLCLOSE}				{ return TOK_XMLCLOSE				; }
<asn>{XMLCOPEN}				{ return TOK_XMLCOPEN				; }
<asn>{LANGLE}				{ return TOK_LANGLE				; }
<asn>{RANGLE}				{ return TOK_RANGLE				; }
<asn>{HYPHEN}				{ return TOK_HYPHEN				; }
<asn>{EQUALS}				{ return TOK_EQUALS				; }
<asn>{APOSTROPHE}			{ return TOK_APOSTROPHE				; }
<asn>{AT}				{ return TOK_AT					; }
<asn>{BAR}				{ return TOK_BAR				; }
<asn>{EXCLAMATION}			{ return TOK_EXCLAMATION			; }
<asn>{CARET}				{ return TOK_CARET				; }
<asn>{ASTERISK}				{ return TOK_ASTERISK				; }
<asn>{AMPERSAND}			{ return TOK_AMPERSAND				; }
<asn>{SLASH}				{ return TOK_SLASH				; }
<INITIAL,asn>{DOCID}			{ return TOK_DOCID				; }
<INTIIAL,asn>{EXTERNREF}		{ return TOK_EXTERNREF				; }
<INITIAL,asn>{NAMEANDNUMBER}		{ return TOK_NAME_AND_NUMBER			; }
<INTIIAL,asn>{IDLABEL}			{ return TOK_ID					; }
<INITIAL,asn>{LABEL}			{ return TOK_LABEL; goto find_rule		; }
<INITIAL,asn>{NUMBER}			{ return TOK_NUMBER				; }

<INITIAL,asn>[a-z]			{ yy_push_state(label); yymore();		; }
<label>([a-zA-Z0-9]|"/"|"-")*[ \t\n]*"."[ \t\n]*([a-zA-Z0-9]|"/"|"-")+		{ yy_pop_state(); return TOK_EXTERNREF;		; }
<label>([a-zA-Z0-9]|"/"|"-")*		{ yy_pop_state(); return TOK_LABEL;		; }

<INITIAL,asn>[A-Z]			{ yy_push_state(id); yymore();			; }
<id>([a-zA-Z0-9]|"/"|"-")*[ \t\n]*"."[ \t\n]*([a-zA-Z0-9]|"/"|"-")+		{ yy_pop_state(); return TOK_EXTERNREF;		; }
<id>([a-zA-Z0-9]|"/"|"-")*		{ yy_pop_state(); return TOK_ID;		; }

<INITIAL,asn>{COMPAIR}			{ yymore(); yy_push_state(com)			; }
<com>{COMPAIR}				{ ECHO; yy_pop_state(); gdmo_incomment = 1	; }
<com>[^\n]+				{ yymore()					; }
<com>{EOL}				{
						// if (gdmo_column && gdmo_column != 8 * gdmo_level) {
						// 	gdmo_newline(yyout);
						// 	gdmo_column += fprintf(yyout, "%.*s", yyleng - 1, yytext);
						// } else {
							gdmo_column += fprintf(yyout, "%.*s", yyleng - 1, yytext);
							gdmo_newline(yyout);
						// }
						yy_pop_state();
					}

<ds1>{SEMICOLON}			{ yy_pop_state(); return TOK_SEMICOLON		; }
<ds1>{DELIMITED_STRING}			{ yy_pop_state(); return TOK_DELIMITED_STRING	; }
<ds1>{TEXT_STRING1}			{ return TOK_TEXT_STRING			; }

<ds2>{COMMA}				{ yy_pop_state(); return TOK_COMMA		; }
<ds2>{SEMICOLON}			{ yy_pop_state(); return TOK_SEMICOLON		; }
<ds2>{DELIMITED_STRING}			{ yy_pop_state(); return TOK_DELIMITED_STRING	; }
<ds2>{TEXT_STRING2}			{ return TOK_TEXT_STRING			; }

<INITIAL,asn>{WHITESPACE}		/* eat whitespace */
<INITIAL,asn>{DEFAULT}			{ yymore()					; }


%%

/* ADDITIONAL CODE */

static void dummy(void) {
	(void) dummy;
	(void) yyunput;
	(void) yy_flex_realloc;
	(void) yy_push_state;
	(void) yy_top_state;
}

// vim: tw=100 com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS formatoptions+=tcqlor
