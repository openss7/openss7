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

/* susaptest.c - test out -lsusap for session unit data service  */

#ifndef	lint
static char *rcsid = "Header: /f/iso/tsap/RCS/tsaptest.c,v 5.0 88/07/21 15:00:04 mrose Rel";
#endif

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#include <stdio.h>
#include <ctype.h>
#include "spkt.h"
#include "ssap.h"
#include "susap.h"
#include "isoservent.h"
#include "isoaddrs.h"
#include "internet.h"

/*  */

char *gets(), *calloc();

printsrv();
printtaddr();

#define CLIENT_MODE  	0
#define SERVER_MODE  	1

/*  */

/* ARGSUSED */

int
main(argc, argv, envp)
	int argc;
	char **argv, **envp;
{
	struct spkt *t;			/* Working packet pointer */
	char sendbuf[1024];		/* send buffer */
	char recvbuf[1024];		/* recv buffer */
	char buffer[80],		/* Working input buffer */
	*bptr;				/* Pointer to our buffer */
	struct udvec uvs[2];
	register struct udvec *uv;
	int cc, result, data, mode;
	u_short pattern;
	register struct tsapblk *tb;
	struct SuSAPstart sunitdata;
	struct SuSAPstart *sud = &sunitdata;
	int sd;
	struct SSAPaddr *sa;
	struct isoservent *is;
	struct servent *sp;
	struct SSAPindication si;
	struct SSAPindication *sptr;

#define	NTADDRS		FD_SETSIZE

	struct TSAPaddr *ta;
	register int n = ta->ta_naddr - 1;
	register struct NSAPaddr *na = ta->ta_addrs;
	static struct TSAPaddr *tz;
	static struct TSAPaddr tas[NTADDRS];
	int port;
	struct NSAPaddr *tcp_na;

	printf("\n SuSAP test driver for SUNITDATA\n");
	printf("\n running on host \"%s\"\n", SLocalHostName());

	t = NULL;
	bptr = buffer;
	*bptr = 'h';

/*
 *  Init the send buffer to a continous pattern.
 */

	for (cc = pattern = 0; cc < sizeof(sendbuf); cc++, pattern++)
		sendbuf[cc] = pattern;

/*
 *  Setup the calling and called network addresses.
 */

	if ((is = getisoserventbyname("susaptest", "ssap")) == NULL) {
		printf("\nfailed to lookup iso service \n");
		exit(1);
	}

	if (is)
		printsrv(is);

	if ((sa = is2saddr("loopback", "udptest", is)) == NULLSA) {
		printf("\n ssap address translation failed \n");
		exit(1);
	}
	if (sa)
		printsaddr(sa);

	if ((ta = is2taddr("loopback", "udptest", is)) == NULLTA) {
		printf("\n tsap address translation failed \n");
		exit(1);
	}
	if (ta)
		printtaddr(ta);

	while (1) {
		switch (toupper(*bptr)) {
		case 'H':
			printf("\n\nCommands are:\n");
			printf("  T - Toggle mode (default is client)\n");
			printf("  B - Bind service\n");
			printf("  U - Unbind service\n");
			printf("  L - Listen on a socket \n");
			printf("  S - Send a datagram\n");
			printf("  R - Receive a datagram\n");
			break;
		case 'Q':
			printf("\nSuSAP tester terminated by user\n");
			exit(0);
		case 'T':

			printf("\n Toggle mode \n");

			printf("\n Current mode is ");
			if (mode == SERVER_MODE) {
				printf("SERVER\n");
				mode = CLIENT_MODE;
				printf("\n Changing to CLIENT\n");
			} else {
				printf("CLIENT\n");
				mode = SERVER_MODE;
				printf("\n Changing to SERVER\n");
			}
			break;

		case 'B':
			printf("\n Binding the service\n");
#ifdef UDP

			if (mode == CLIENT_MODE)
				sd = SUnitDataBind(NOTOK, NULL, sa, NULL, &si);
			else
				sd = SUnitDataBind(NOTOK, sa, NULL, NULL, &si);

			if (sd == NOTOK)
				printf("\n Bind unit data service failed\n");
			else
				printf("\n application bound to socket = %d \n", sd);

#endif
			break;

		case 'U':
			printf("\n Unbinding the service\n");

			SUnitDataUnbind(sd);

			break;

		case 'L':

			if ((sp = getservbyname("tsap", "tcp")) == NULL)
				printf("\ntcp/tsap: unknown service");

			tz = tas;
			tcp_na = tz->ta_addrs;
			tcp_na->na_type = NA_TCP;
			tcp_na->na_domain[0] = NULL;
			tcp_na->na_port = sp->s_port;
			tz->ta_naddr = 1;

			sd = TUnitDataListen(tcp_na, NULL);

			if (sd == NOTOK)
				printf("\n TNetListen failed\n");
			else {
				printf("\n listening on socket = %d \n", sd);
				printf("\n              port = %d \n", tcp_na->na_port);
			}
			break;

		case 'S':

			printf("\n Send a datagram \n");
			printf("\n sending %d bytes of user data\n", cc);
			printf("\n here are the first 100 bytes \n");

			for (data = 0; data < 100; data++)
				printf(" %x ", sendbuf[data]);

			uv = &uvs[0];
			uv->uv_base = &sendbuf[0];
			uv->uv_len = 1024;
			uv++;
			uv->uv_len = 0;
			uv->uv_base = NULL;

			result = SUnitDataWrite(sd, &sendbuf[0], sizeof sendbuf, &si);

			if (result != OK)
				printf("\n datagram write failed\n");
			else
				printf("\n datagram write successful\n");
			break;

		case 'R':
			printf("\n Read datagram on socket %d \n", sd);

			result = SUnitDataRead(sd, &sunitdata, 3, &si);

			if (result != OK)
				printf("\n Read failed.  \n");
			else {
				printf("\n Read successful.  \n");

				printf("\n calling addr: \n");
				printsaddr(&sunitdata.ss_calling);

				printf("\n called addr: \n");
				printsaddr(&sunitdata.ss_called);

				printf("\n user data len = %d  ...  \n", sunitdata.ss_cc);
				for (data = 0; data < min(sunitdata.ss_cc, 256); data++)
					printf(" %x ", *(sunitdata.ss_data + data));

				SUSFREE(&sunitdata);

				if (mode == CLIENT_MODE)
					break;

				/* rebind to the remote addr */

				sd = SUnitDataBind(sd, sa, &sunitdata.ss_calling, NULL, &si);

				if (sd == NOTOK)
					printf("\n REBIND unit data service failed\n");
				else
					printf("\n application REBINDED to socket = %d \n", sd);

			}
			break;

		case '\0':
			break;
		default:
			if (*bptr != '\0')
				printf("** Unrecognized command\n");
			break;
		}
		printf("\nCommand: ");
		(void) fflush(stdout);
		bptr = gets(buffer);
		if (feof(stdin))
			exit(1);
		while (isspace(*bptr) && (*bptr != '\0'))
			++bptr;
	}
}

static
printsrv(is)
	register struct isoservent *is;
{
	register int n = is->is_tail - is->is_vec - 1;
	register char **ap = is->is_vec;

	printf("ENT: \"%s\" PRV: \"%s\" SEL: %s\n",
	       is->is_entity, is->is_provider, sel2str(is->is_selector, is->is_selectlen, 1));

	for (; n >= 0; ap++, n--)
		printf("\t%d: \"%s\"\n", ap - is->is_vec, *ap);
	printf("\n");
}

/*  */

static
printsaddr(sa)

	register struct SSAPaddr *sa;

{

	struct TSAPaddr *ta = &(sa->sa_addr);
	struct NSAPaddr *na = ta->ta_addrs;
	int n = 1;

	printf("ADDR:    SSEL: %s\n", sel2str(sa->sa_selector, sa->sa_selectlen, 1));

	printf("ADDR:    TSEL: %s\n", sel2str(ta->ta_selector, ta->ta_selectlen, 1));

	for (; n >= 0; na++, n--) {
		printf("\t%d: ", ta->ta_naddr - n - 1);

		switch (na->na_type) {
		case NA_NSAP:
			printf("NS %s", na2str(na));
			break;

		case NA_TCP:
			printf("TCP %s", na2str(na));
			if (na->na_port)
				printf("  Port %d Tset 0x%x",
				       (int) ntohs(na->na_port), (int) na->na_tset);
			break;

		case NA_X25:
		case NA_BRG:
			printf("X.25 %s%s", na->na_type == NA_BRG ? "(BRIDGE) " : "", na2str(na));
			if (na->na_pidlen > 0)
				printf(" %s", sel2str(na->na_pid, (int) na->na_pidlen, 1));
			break;

		default:
			printf("0x%x", na->na_type);
			break;
		}
	}

}

static
printtaddr(ta)

	register struct TSAPaddr *ta;

{

	register int n;
	register struct NSAPaddr *na;

	n = ta->ta_naddr - 1;
	na = ta->ta_addrs;

	printf("ADDR:    TSEL: %s\n", sel2str(ta->ta_selector, ta->ta_selectlen, 1));

	for (; n >= 0; na++, n--) {
		printf("\t%d: ", ta->ta_naddr - n - 1);

		switch (na->na_type) {
		case NA_NSAP:
			printf("NS %s", na2str(na));
			break;

		case NA_TCP:
			printf("TCP %s", na2str(na));
			if (na->na_port)
				printf("  Port %d Tset 0x%x",
				       (int) ntohs(na->na_port), (int) na->na_tset);
			break;

		case NA_X25:
		case NA_BRG:
			printf("X.25 %s%s", na->na_type == NA_BRG ? "(BRIDGE) " : "", na2str(na));
			if (na->na_pidlen > 0)
				printf(" %s", sel2str(na->na_pid, (int) na->na_pidlen, 1));
			break;

		default:
			printf("0x%x", na->na_type);
			break;
		}
	}

}
