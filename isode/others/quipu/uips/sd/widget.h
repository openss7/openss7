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

#ifndef __QUIPU_UIPS_SD_SIDGET_H__
#define __QUIPU_UIPS_SD_SIDGET_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* widget.h - definition of the widget structure and assorted constants */

/*
 * Header: /xtel/isode/isode/others/quipu/uips/sd/RCS/widget.h,v 9.0 1992/06/16 12:45:08 isode Rel
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

/*****************************************************************************
 *This file has been modified;
 *Modifier:     Damanjit Mahl @ Brunel University, Uxbridge
 *****************************************************************************/

/*****************************************************************************
 * File:	widget.h
 * Author:	Paul Sharpe @ GEC Research, Hirst Research Centre.
 * Date:	August 12, 1988.
 * Function:	Definition of the widget structure and assorted constants.
 *
 * DISCLAIMER:
 * This source file is deemed to be public domain: any person may use the
 * code in any way, on two simple provisos -
 *   1 - That this header remains in this file,
 *   2 - It is accepted that neither the author, nor the authors employees
 *       accept any responsibility for the use, abuse or misuse of the
 *       contained code.
 * No guarantee is made by the author to provide maintainance or up-grading
 * of this source. However, any adaptations made to this file will be viewed
 * at least with interest.
 *****************************************************************************/

#ifndef WIDGETDEFS
#define WIDGETDEFS

#include <curses.h>

/* These define the type of the widget involved */
#define FINISH		0
#define LABEL		1
#define COMMAND		2
#define TOGGLE		3
#define DIALOG		4
#define SCROLLBAR       5
#define DUMMY		6

/* These define functions that aren't really needed */
#define NULLFN		nullfn
#define TOGGLEFN	NULLFN
#define QUITFN		quitfn

/* These define the LABEL wdgt text justification */
#define CENTER		CENTRE
#define CENTRE		1
#define LEFT		2
#define RIGHT		4

/* Thisdefines an expanded widget box */
#define EXPAND		-1

/* This defines a widget position starting on a new line */
#define CRNL		-1

/* This is the default shown length of the dialog string */
#define DIALOGLEN	4

/* This defines the maximum number of levels of active widgets */
#define MAXACTIVE	10
#ifndef BUFLEN
#define BUFLEN		1024
#endif

int lowy;

/* This is the height of a widget box, in number of lines */
#define WDGTHGHT	3

typedef struct widget {
	char type;			/* Type of widget: see the above definitions */
	char *label;			/* text string label for the widget */

	/* The former is used by LABEL and COMMAND widgets: the latter by COMMAND */
	char callch;			/* Character to activate the COMMAND widget */
	void (*callfn) ();		/* Function called by an activated COMMAND */

	/* ALL widgets need these fields to be set */
	int x, y;			/* Position of the top right of the window */
	int wdth, hght;			/* width and height of the widget window */

	/* These are only used by the DIALOG type widgets */
	int dstrlen;			/* Maximum length of the DIALOG widget str */
	char *dstr;			/* Pointer to the DIALOG string to fill in */

	/* These are only used by the TOGGLE type widgets */
	char tindx;			/* Index into the toggle values */
	char **tvalues;			/* NULL-terminated array of TOGGLE strings */

	WINDOW *wndw;			/* The curses-widget window structure */
} WIDGET;

WIDGET *currwidgets;
char typetoggled;

void initwidgets(), textfresh(), makewidgets(), setwdgtwdth(),
killwidgets(), activewidget(), deleteactive(), activeindex(),
redraw(), rfrshwidgets(), boxwdgt(), printwdgt(), printbar(),
printlabel(), printdialog(), printtoggle(), printcommand(),
interact(), docallback(), dialog(), setdialogstr(),
toggle(), settogglstrs(), setlabel(), getlabel(), wprint(), tprint(),
xprint(), xprintint(), cleartext(), jumpback(), nullfn(), quitfn(), endwidgets();

int linec(), gety(), posnwidgets(), getwidgetindex(), getdialogstr(),
settogglindx(), gettogglindx(), gettogglstr(), lowesty(), findactiveinput();

WIDGET *getwidget();

#endif

#endif				/* __QUIPU_UIPS_SD_SIDGET_H__ */
