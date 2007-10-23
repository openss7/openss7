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

/* jpegphoto.c - your comments here */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/quipu/photo/RCS/jpegphoto.c,v 9.0 1992/06/16 12:43:35 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/others/quipu/photo/RCS/jpegphoto.c,v 9.0 1992/06/16 12:43:35 isode Rel
 *
 *
 * Log: jpegphoto.c,v
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

/*
 *
 * jpegphoto- Read standard input and pipe output to correct displayer
 *
 * Russ Wright - Lawrence Berkeley Laboratory-  Oct 1991
 */

#include <stdio.h>
#include "tailor.h"
#include "general.h"
#include "jpeg.h"
#include <signal.h>

#define BAD_EXIT        -1

char command[512];

int
mygetchar()
{
	char c;

	if (!read(0, &c, 1))
		return (EOF);
	else
		return (c);
}

/*
 * SkipAsn1Len - skip the ASN-1 encoded length (variable # of octets)
 */
void
SkipAsn1Len()
{
	unsigned char c;

	c = mygetchar();

	if (c > 0x80) {
		c &= ~0x80;
		while (c--) {
			(void) mygetchar();
		}
	}
}

void
DoG3Fax(firstChar)
	unsigned char firstChar;
{
	char buffer[8192];
	FILE *newPipe;
	int len;

	(void) strncpy(command, isodefile("g3fax/Xphoto", 1), sizeof(command) - 1);
	(void) fprintf(stderr, "command IS '%s' ", command);
	newPipe = popen(command, "w");
	if (newPipe == NULL) {
		(void) fprintf(stderr, "command '%s' failed", command);
		perror(" ");
		exit(BAD_EXIT);
	}
	(void) fwrite((char *) &firstChar, 1, 1, newPipe);

	while (len = read(0, buffer, sizeof(buffer))) {
		if (!fwrite(buffer, 1, len, newPipe)) {
			(void) fprintf(stderr, "write to pipe failed for '%s'", command);
			perror(" ");
			exit(BAD_EXIT);
		}
	}

	(void) pclose(newPipe);
	exit(0);
}

void
DoNewJPEG()
{
	SkipAsn1Len();

	(void) strncpy(command, isodefile("g3fax/jpeg.sh", 1), sizeof(command) - 1);

	if (execl(command, "xphoto-jpeg", 0)) {
		(void) fprintf(stderr, "command '%s' failed", command);
		perror(" ");
		exit(BAD_EXIT);
	}
/*NOTREACHED*/}

void
DoJPEG()
{
	SkipAsn1Len();

	(void) strncpy(command, isodefile("g3fax/jpeg.sh", 1), sizeof(command) - 1);

	if (execl(command, "xphoto-jpeg", 0)) {
		(void) fprintf(stderr, "command '%s' failed", command);
		perror(" ");
		exit(BAD_EXIT);
	}
/*NOTREACHED*/}

/* ARGSUSED */
void
main(argc, argv, envp)
	int argc;
	char **argv, **envp;
{
	unsigned char firstChar;

	firstChar = mygetchar();

	/* test first octet */

	switch (firstChar) {
	case NEW_JPEG_TAG:
		DoNewJPEG();
		break;
	case JPEG_TAG:
		DoJPEG();
		break;
	case OLD_G3Fax_TAG:
	case G3Fax_TAG:
		DoG3Fax(firstChar);
		break;
	default:
		(void) fprintf(stderr, "Unknown Photo Format: %d\n", firstChar);
		exit(BAD_EXIT);
	}
 /*NOTREACHED*/}
