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

#ifndef __QUIPU_UIPS_SD_WIDGETS_H__
#define __QUIPU_UIPS_SD_WIDGETS_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * Header: /xtel/isode/isode/others/quipu/uips/sd/RCS/wdgtdefs.h,v 9.0 1992/06/16 12:45:08 isode Rel
 */

#include "widget.h"

/* REMEMBER TO NULL-TERMINATE ALL OF THESE STRUCTURE ARRAYS     */
/*--------------------------------------------------------------*/

void main_help();
void list_start(), back_start();
void srch_start(), widen();
void rd_start();
void sd_quit(), cache_quit();
void get_listed_object(), goto_addr(), scrollbar();

WIDGET mainwdgts[] = {
	{LABEL, " QUIPU X.500 Screen Directory. ", CENTRE, NULLFN, 0, 0, EXPAND, 0},
	{COMMAND, "q Quit ", 'q', sd_quit, 0, CRNL, 0, 0},
	{COMMAND, "h Help ", 'h', main_help, 0, 0, 0, 0},
	{COMMAND, "l List ", 'l', list_start, 0, 0, 0, 0},
	{COMMAND, "w Widen Search Area", 'w', widen, 0, 0, 0, 0},
	{COMMAND, "b History ", 'b', back_start, 0, 0, 0, 0},
	{DIALOG, "Go To Number:", '*', get_listed_object, 0, 0, EXPAND, 0},
	{DIALOG, "Search Area: ", '\0', goto_addr, 0, CRNL, EXPAND, 0},
	{TOGGLE, "t Type: ", 't', TOGGLEFN, 0, 0, 0, 0},
	{DIALOG, "s Search for: ", 's', srch_start, 0, 0, EXPAND, 0},
	{DUMMY, "Search", '\n', srch_start, 0, 0, 0, 0},
/* Scrollbar always last */
	{SCROLLBAR, "", '%', scrollbar, 0, 0, 3, 0},
	{FINISH, "", 0, NULLFN}
};

/*--------------------------------------------------------------*/

void help_read(), help_list(), help_srch(), help_back(),
help_number(), help_addr(), help_up(), returnmain();

WIDGET dethelpwdgts[] = {
	{LABEL, "QUIPU X.500 Screen Directory: Help", CENTRE, NULLFN, 0, 0, EXPAND, 0},
	{COMMAND, "q QUIT Help", 'q', returnmain, 0, CRNL, 0, 0},
	{COMMAND, "s Search for", 's', help_srch, 0, 0, 0, 0},
	{COMMAND, "b History ", 'b', help_back, 0, 0, 0, 0},
	{COMMAND, "w Widen Search Area", 'w', help_up, 0, 0, 0, 0},
	{COMMAND, "l List ", 'l', help_list, 0, 0, 0, 0},
	{COMMAND, "n Number ", 'n', help_number, 0, 0, 0, 0},
/*scrollbar always last*/
	{SCROLLBAR, "", '%', scrollbar, 0, 0, 3, 0},
	{FINISH, "", 0, NULLFN}
};

#endif				/* __QUIPU_UIPS_SD_WIDGETS_H__ */
