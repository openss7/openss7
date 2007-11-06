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

/* osilookup.c - convert entry in /etc/osi.hosts to isoentities format */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/osilookup/RCS/osilookup.c,v 9.0 1992/06/16 12:43:08 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/others/osilookup/RCS/osilookup.c,v 9.0 1992/06/16 12:43:08 isode Rel
 *
 * Contributed by John A. Scott, the MITRE Corporation
 *
 * N.B.:	I whipped up this code quickly to fill a need I had.  I
 *		do not, it any way, shape, or form, warrant its output.
 *
 *
 * Log: osilookup.c,v
 * Revision 9.0  1992/06/16  12:43:08  isode
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

#include "config.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#ifndef	SUNLINK_6_0
#include <sys/ieee802.h>
#else
#include <net/if_ieee802.h>
#endif
#include <netosi/osi.h>
#include <netosi/osi_addr.h>
#include <sunosi/mapds_user.h>

/*  */

/* ARGSUSED */

main(argc, argv, envp)
	int argc;
	char **argv, **envp;
{
	int len, paddr_type;
	char *prefix, *service, buf[BUFSIZ], buf2[BUFSIZ];
	OSI_ADDR p_addr;

	if (argc < 2) {
		fprintf(stderr, "usage: %s host [service]\n", argv[0]);
		exit(0);
	}
	service = (argc > 2) ? argv[2] : "FTAM";

	/* SUNLink OSI directory lookup */
	mds_lookup(argv[1], service, &p_addr);

	/* SUNLink function to slice out SAP bytes from full address */
	paddr_type = 0;
	if ((len = osi_get_sap(&p_addr, buf, sizeof buf, OSI_NSAP, &paddr_type))
	    <= 0) {
		fprintf(stderr, "no entry for %s %s\n", argv[1], service);
		exit(1);
	}

	buf2[explode(buf2, (u_char *) buf, len)] = NULL;
	switch (paddr_type) {
	case AF_NBS:
		prefix = "49";
		break;

	case AF_OSINET:
		prefix = "470004";
		break;

	default:
		prefix = "";
		break;
	}
	printf("\t\t\t\tNS+%s%s\n\n", prefix, buf2);

	exit(0);
}

/* so we don't have to load libisode.a */

static char nib2hex[0x10] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

static int
explode(a, b, n)
	register char *a;
	register u_char *b;
	register int n;
{
	register int i;
	register u_char c;

	for (i = 0; i < n; i++) {
		c = *b++;
		*a++ = nib2hex[(c & 0xf0) >> 4];
		*a++ = nib2hex[(c & 0x0f)];
	}
	*a = NULL;

	return (n * 2);
}
