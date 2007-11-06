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

#ifndef __FTAM_FTP_FTP_VAR_H__
#define __FTAM_FTP_FTP_VAR_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* ftp_var.h - FTP global variables */

/* 
 * Header: /xtel/isode/isode/ftam-ftp/RCS/ftp_var.h,v 9.0 1992/06/16 12:15:29 isode Rel
 *
 *
 * Log: ftp_var.h,v
 * Revision 9.0  1992/06/16  12:15:29  isode
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

/*
 *	Shamelessly taken from UCB
 */

#include "general.h"

/*
 * Options and other state info.
 */
int trace;				/* trace packets exchanged */
int hash;				/* print # for each buffer transferred */
int verbose;				/* print messages to/from server */
int connected;				/* connected to server */
int fromatty;				/* input is from a terminal */
int interactive;			/* interactively prompt on m* cmds */
int debug;				/* debugging level */
int bell;				/* ring bell on cmd completion */
int doglob;				/* glob local file names */
int autologin;				/* establish user account on connection */

char typename[32];			/* name of file transfer type */
int type;				/* file transfer type */
char structname[32];			/* name of file transfer structure */
int stru;				/* file transfer structure */
char formname[32];			/* name of file transfer format */
int form;				/* file transfer format */
char modename[32];			/* name of file transfer mode */
int mode;				/* file transfer mode */
char bytename[32];			/* local byte size in ascii */
int bytesize;				/* local byte size in binary */

char *hostname;				/* name of host connected to */

struct servent *sp;			/* service spec for tcp/ftp */

#include <setjmp.h>
jmp_buf toplevel;			/* non-local goto stuff for cmd scanner */

char line[200];				/* input line buffer */
char *stringbase;			/* current scan point in line buffer */
char argbuf[200];			/* argument storage buffer */
char *argbase;				/* current storage point in arg buffer */
int margc;				/* count of arguments on input line */
char *margv[20];			/* args parsed from input line */

int options;				/* used during socket creation */

/*
 * Format of command table.
 */
struct cmd {
	char *c_name;			/* name of command */
	char *c_help;			/* help string */
	char c_bell;			/* give bell when command completes */
	char c_conn;			/* must be connected to use command */
	int (*c_handler) ();		/* function to call */
};

extern char *tail();
extern char *remglob();
extern int errno;
extern int sys_nerr;
extern char *sys_errlist[];

/* global interface variables */
int ftp_directory;			/* TRUE if last ftp_exist was a multiple listing */
char ftp_error_buffer[BUFSIZ];
char *ftp_error;			/* points to FTP diagnostic string */

#endif				/* __FTAM_FTP_FTP_VAR_H__ */
