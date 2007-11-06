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

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/* sunview.c - sunview display process */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/photo/RCS/sunview.c,v 9.0 1992/06/16 12:43:35 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/others/quipu/photo/RCS/sunview.c,v 9.0 1992/06/16 12:43:35 isode Rel
 *
 *
 * Log: sunview.c,v
 * Revision 9.0  1992/06/16  12:43:35  isode
 * Release 8.0
 *
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

/* sunview display process */

#include <stdio.h>
#include "quipu/photo.h"
#include <suntool/tool_hs.h>
#include <suntool/panel.h>
#include <suntool/gfxsw.h>
#include  <sys/resource.h>

static struct gfxsubwindow *gfx;
static struct tool *tool;
static struct toolsw *gfx_sw;
static int sx = 20, sy = 20, x, y;
extern int PIC_LINESIZE;

sigwinched()
{
	tool_sigwinch(tool);
}

photo_start(name)
	char *name;
{
	char *getenv();

	/* Initialise a window to recieve a photo of 'name' */

	if (getenv("WINDOW_PARENT") == (char *) NULL) {
		(void) fprintf(stderr, "PHOTO: Must be running suntools on the console");
		return (-1);
	}

	if ((tool = tool_make(WIN_LABEL, name, WIN_TOP, sx, WIN_LEFT, sy, 0)) == NULL) {
		(void) fprintf(stderr, "PHOTO: can't create window");
		return (-1);
	}

	signal(SIGWINCH, sigwinched);

	gfx_sw =
	    gfxsw_createtoolsubwindow(tool, "", TOOL_SWEXTENDTOEDGE, TOOL_SWEXTENDTOEDGE, NULL);
	gfx = (struct gfxsubwindow *) gfx_sw->ts_data;
	gfxsw_getretained(gfx);

	tool_install(tool);

	/* return 0 if sucessful -1 if not */
	return (0);
}

photo_end(name)
	char *name;
{
	/* Decoding has finished - display the photo */

	(void) printf("(See sunview window)");
	(void) fflush(stdout);
	(void) close(1);

	/* return 0 if sucessful -1 if not */
	tool_set_attributes(tool, WIN_WIDTH, PIC_LINESIZE + 40, WIN_HEIGHT, sy + 40, 0);
	tool_select(tool, 0);

	return (0);
}

photo_black(length)
	int length;
{
	/* draw a black line of 'length' pixels */
}

photo_white(length)
	int length;
{
	/* draw a white line of 'length' pixels */
}

photo_line_end(line)
	bit_string *line;
{
	struct pixrect *pix;

	/* the end of a line has been reached */
	/* A bit string is stored in line->dbuf_top */

	pix = mem_point(PIC_LINESIZE, 1, 1, line->dbuf_top);
	pw_write(gfx->gfx_pixwin, sx, sy, PIC_LINESIZE - sx, 1, PIX_SRC, pix, 0, 0);
	sy++;

}
