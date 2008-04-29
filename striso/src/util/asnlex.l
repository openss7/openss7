/*****************************************************************************

 @(#) $Id: asnlex.l,v 0.9.2.4 2008-04-29 00:02:06 brian Exp $

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

 $Log: asnlex.l,v $
 Revision 0.9.2.4  2008-04-29 00:02:06  brian
 - updated headers for release

 Revision 0.9.2.3  2008/04/26 11:05:37  brian
 - support for flex 2.5.4 and 2.5.33

 Revision 0.9.2.2  2008-04-11 16:58:44  brian
 - removed deprecated NEXUSWARE support

 Revision 0.9.2.1  2007/09/06 10:59:13  brian
 - added new gdmo files

 *****************************************************************************/

/* C DECLARATIONS */

%{

#ident "@(#) $RCSfile: asnlex.l,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 00:02:06 $"

static char const ident[] = "$RCSfile: asnlex.l,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 00:02:06 $";

#include <unistd.h> /* isatty(), official flex 2.5.4a skeleton misses this */

#include <stdlib.h>
#include <string.h>

#include "asnyac.h"

#define ECHO fputs(yytext, yyout)

#define reject_used_but_not_detected
#define FIND_RULE REJECT

%}


/* FLEX DECLARATIONS */

%option nolex-compat
%option warn
/* %option debug */
/* %option verbose */
/* %option perf-report */
%option default
%option nocaseless
%option 7bit
%option interactive
%option noalways-interactive
%option nonever-interactive
%option nostdinit
%option stack
%option nomain
%option noyywrap
%option noyylineno

%%

BOOLEAN			{ return TOK_BOOLEAN; FIND_RULE; }
INTEGER			{ return TOK_INTEGER; }
BIT			{ return TOK_BIT; }
STRING			{ return TOK_STRING; }
OCTET			{ return TOK_OCTET; }
NULL			{ return TOK_NULL; }
SEQUENCE		{ return TOK_SEQUENCE; }
OF			{ return TOK_OF; }
SET			{ return TOK_SET; }
IMPLICIT		{ return TOK_IMPLICIT; }
CHOICE			{ return TOK_CHOICE; }
ANY			{ return TOK_ANY; }
EXTERNAL		{ return TOK_EXTERNAL; }
OBJECT			{ return TOK_OBJECT; }
IDENTIFIER		{ return TOK_IDENTIFIER; }
OPTIONAL		{ return TOK_OPTIONAL; }
DEFAULT			{ return TOK_DEFAULT; }
COMPONENTS		{ return TOK_COMPONENTS; }
UNIVERSAL		{ return TOK_UNIVERSAL; }
APPLICATION		{ return TOK_APPLICATION; }
PRIVATE			{ return TOK_PRIVATE; }
TRUE			{ return TOK_TRUE; }
FALSE			{ return TOK_FALSE; }
BEGIN			{ return TOK_BEGIN; }
END			{ return TOK_END; }
DEFINITIONS		{ return TOK_DEFINITIONS; }
EXPLICIT		{ return TOK_EXPLICIT; }
ENUMERATED		{ return TOK_ENUMERATED; }
EXPORTS			{ return TOK_EXPORTS; }
IMPORTS			{ return TOK_IMPORTS; }
REAL			{ return TOK_REAL; }
INCLUDES		{ return TOK_INCLUDES; }
MIN			{ return TOK_MIN; }
MAX			{ return TOK_MAX; }
SIZE			{ return TOK_SIZE; }
FROM			{ return TOK_FROM; }
WITH			{ return TOK_WITH; }
COMPONENT		{ return TOK_COMPONENT; }
PRESENT			{ return TOK_PRESENT; }
ABSENT			{ return TOK_ABSENT; }
DEFINED			{ return TOK_DEFINED; }
BY			{ return TOK_BY; }
PLUS[- ]INFINITY	{ return TOK_PLUS_INFINITY; }
MINUS[- ]INFINITY	{ return TOK_MINUS_INFINITY; }
TAGS			{ return TOK_TAGS; }
"::="			{ return TOK_ASSIGNMENT; }
"{"			{ return TOK_OPEN_BRACE; }
"}"			{ return TOK_CLOSE_BRACE; }
"<"			{ return TOK_LEFT_ANGLE; }
","			{ return TOK_COMMA; }
"."			{ return TOK_PERIOD; }
"("			{ return TOK_OPEN_PAREN; }
")"			{ return TOK_CLOSE_PAREN; }
"["			{ return TOK_OPEN_BRACKET; }
"]"			{ return TOK_CLOSE_BRACKET; }
"-"			{ return TOK_HYPHEN; }
";"			{ return TOK_SEMICOLON; }
'[01]*'B		{ return TOK_bstring; }
'[0-9a-fA-F]*'H		{ return TOK_hstring; }
\"([^\"]*|\\\"|\"\")*\"	{ return TOK_cstring; }
[A-Z][-a-zA-Z0-9]*	{ return TOK_identifier; }
[a-z][-a-zA-Z0-9]*	{ return TOK_reference; }
[1-9][0-9]*		{ return TOK_number; }



%%

/* ADDITIONAL CODE */

static void dummy(void) {
	(void) dummy;
	(void) yyunput;
	(void) yy_flex_realloc;
}

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS formatoptions+=tcqlor
