/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2012  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

/* This is a little utility program that performs socket input-output controls. */

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

#include <stdint.h>
#include <stropts.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>

#include <sys/sockio.h>
#include <net/if.h>
#include <net/lif.h>

int my_argc = 0;
char **my_argv = NULL;

int output = 1;

char devname[256] = "/dev/streams/bpf/x400p-sl";
char interface[LIFNAMSIZ] = "";

static void
copying(int argc, char *argv[])
{
	if (!output)
		return;
	(void) fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2008-2012  Monavacon Limited <http://www.monavacon.com/>\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software: you can  redistribute it  and/or modify  it under\n\
the terms of the  GNU Affero  General  Public  License  as published by the Free\n\
Software Foundation, version 3 of the license.\n\
\n\
This program is distributed in the hope that it will  be useful, but WITHOUT ANY\n\
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.\n\
\n\
You should have received a copy of the  GNU Affero General Public License  along\n\
with this program.   If not, see <http://www.gnu.org/licenses/>, or write to the\n\
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
--------------------------------------------------------------------------------\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the U.S. Government (\"Government\"), the following provisions apply to you. If\n\
the Software is supplied by the Department of Defense (\"DoD\"), it is classified\n\
as \"Commercial  Computer  Software\"  under  paragraph  252.227-7014  of the  DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the  license rights granted\n\
herein (the license rights customarily provided to non-Government users). If the\n\
Software is supplied to any unit or agency of the Government  other than DoD, it\n\
is  classified as  \"Restricted Computer Software\" and the Government's rights in\n\
the Software  are defined  in  paragraph 52.227-19  of the  Federal  Acquisition\n\
Regulations (\"FAR\")  (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of  the  NASA  Supplement  to the FAR (or any  successor\n\
regulations).\n\
--------------------------------------------------------------------------------\n\
Commercial  licensing  and  support of this  software is  available from OpenSS7\n\
Corporation at a fee.  See http://www.openss7.com/\n\
--------------------------------------------------------------------------------\n\
", ident);
}

static void
version(int argc, char *argv[])
{
	if (!output)
		return;
	(void) fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
Written by Brian Bidulock.\n\
\n\
Copyright (c) 2008, 2009, 2010, 2011, 2012  Monavacon Limited.\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation.\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 under GNU Affero General Public License Version 3,\n\
with conditions, incorporated herein by reference.\n\
\n\
See `%1$s --copying' for copying permissions.\n\
", NAME, PACKAGE, VERSION, "$Revision: 1.1.2.2 $ $Date: 2010-11-28 14:22:38 $");
}

static void
usage(int argc, char *argv[])
{
	if (!output)
		return;
	(void) fprintf(stderr, "\
Usage:\n\
    %1$s [options] {-d,--device DEVNAME} {-i,--interface INTERFACE} {-a,--alternate}\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

static void
help(int argc, char *argv[])
{
	if (!output)
		return;
	(void) fprintf(stdout, "\
Usage:\n\
    %1$s [options] {-d,--device DEVNAME} {-i,--interface INTERFACE} {-a,--alternate}\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    (none)\n\
General Options:\n\
    -q, --quiet                         (default: 0)\n\
        suppress normal output (equivalent to --verbose=0)\n\
    -D, --debug [LEVEL]                 (default: 0)\n\
        increment or set debug LEVEL\n\
    -v, --verbose [LEVEL]               (default: %2$d)\n\
        increment or set verbosity LEVEL\n\
Command Options:\n\
    -h, --help, -?, --?\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
Get Options:\n\
    -d, --device DEVNAME                (default: \"%3$s\")\n\
        device name\n\
    -i, --interface INTERFACE           (default: \"%4$s\")\n\
        get only for INTERFACE\n\
    -a, --alternate\n\
        use alternate LIF input-output controls\n\
", argv[0], output, devname, interface);
}

void
soconf_get(int alt)
{
	int fd, ret;

	if ((fd = open(devname, O_RDWR)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	switch (alt) {
	case 1:
	{
		int i, num;

		struct lifnum lifn = {
			.lifn_family = AF_UNSPEC,
			.lifn_flags =
			    (LIFC_NOXMIT | LIFC_EXTERNAL_SOURCE | LIFC_TEMPORARY | LIFC_ALLZONES |
			     LIFC_UNDER_IPMP | LIFC_ENABLED),
			.lifn_count = 0,
		};
		struct lifconf lifc;
		struct lifreq *lifr;
		size_t size;

		if (interface[0] = '\0') {
			printf("SIOCGLIFCONF(%d): lifr_name = %s\n", i, lifr[i].lifr_name);
			printf("SIOCGLIFCONF(%d): lifr_addr.sa_family = ", i);
			switch (lifr[i].lifr_addr.ss_family) {
			case AF_UNSPEC:
				printf("AF_UNSPEC");
				break;
			case AF_UNIX:
				printf("AF_UNIX");
				break;
			case AF_INET:
				printf("AF_INET");
				break;
			case AF_INET6:
				printf("AF_INET6");
				break;
			case AF_IPX:
				printf("AF_IPX");
				break;
			case AF_NETLINK:
				printf("AF_NETLINK");
				break;
			case AF_X25:
				printf("AF_X25");
				break;
			case AF_AX25:
				printf("AF_AX25");
				break;
			case AF_ATMPVC:
				printf("AF_ATMPVC");
				break;
			case AF_APPLETALK:
				printf("AF_APPLETALK");
				break;
			case AF_PACKET:
				printf("AF_PACKET");
				break;
			default:
				printf("%d", lifr[i].lifr_addr.ss_family);
				break;
			}
			printf("\n");
			if ((ret = ioctl(fd, SIOCGLIFNUM, &lifn)) < 0) {
				perror(__FUNCTION__);
				exit(1);
			}
			printf("SIOCGLIFNUM: lifn_family = AF_UNSPEC\n");
			printf
			    ("SIOCGLIFNUM: lifn_flags = <NOXMIT,EXTERNAL_SOURCE,TEMPORARY,ALLZONES,UNDER_IPMP,ENABLED>\n");
			printf("SIOCGLIFNUM: lifn_count = %d\n", lifn.lifn_count);
			if (lifn.lifn_count <= 0)
				break;
			size = lifn.lifn_count * sizeof(struct lifreq);
			if ((lifr = malloc(size)) == NULL) {
				perror(__FUNCTION__);
				exit(1);
			}
			memset(lifr, 0, size);
			lifc.lifc_family = AF_UNSPEC;
			lifc.lifc_flags =
			    (LIFC_NOXMIT | LIFC_EXTERNAL_SOURCE | LIFC_TEMPORARY | LIFC_ALLZONES |
			     LIFC_UNDER_IPMP | LIFC_ENABLED);
			lifc.lifc_len = size;
			lifc.lifc_req = lifr;
			if ((ret = ioctl(fd, SIOCGLIFCONF, &lifc)) < 0) {
				perror(__FUNCTION__);
				exit(1);
			}
			printf("SIOCGLIFCONF: lifc_len = %d\n", lifc.lifc_len);
			if ((num = lifc.lifc_len / sizeof(struct lifreq)) < 0)
				break;
		} else {
			size = sizeof(struct lifreq);
			if ((lifr = malloc(size)) == NULL) {
				perror(__FUNCTION__);
				exit(1);
			}
			memset(lifr, 0, size);
			lifc.lifc_family = AF_UNSPEC;
			lifc.lifc_flags =
			    (LIFC_NOXMIT | LIFC_EXTERNAL_SOURCE | LIFC_TEMPORARY | LIFC_ALLZONES |
			     LIFC_UNDER_IPMP | LIFC_ENABLED);
			lifc.lifc_len = size;
			lifc.lifc_req = lifr;
			strncpy(lifr[0].lifr_name, interface, LIFNAMSIZ);
			num = 1;
		}
		for (i = 0; i < num; i++) {
			char sep[2] = { '\0', '\0' };

			if ((ret = ioctl(fd, SIOCGLIFFLAGS, &lifr[i])) < 0) {
				perror(__FUNCTION__);
				exit(1);
			}
			printf("SIOCGLIFFLAGS(%d): lifr_name = %s\n", i, lifr[i].lifr_name);
			printf("SIOCGLIFFLAGS(%d): lifr_ppa = 0x%04x\n", i, lifr[i].lifr_ppa);
			printf("SIOCGLIFFLAGS(%d): lifr_flags = <", i);
			if (lifr[i].lifr_flags & IFF_UP) {
				printf("%s%s", sep, "UP");
				sep[0] = ',';
			}
			if (lifr[i].lifr_flags & IFF_BROADCAST) {
				printf("%s%s", sep, "BROADCAST");
				sep[0] = ',';
			}
			if (lifr[i].lifr_flags & IFF_DEBUG) {
				printf("%s%s", sep, "DEBUG");
				sep[0] = ',';
			}
			if (lifr[i].lifr_flags & IFF_LOOPBACK) {
				printf("%s%s", sep, "LOOPBACK");
				sep[0] = ',';
			}
			if (lifr[i].lifr_flags & IFF_POINTTOPOINT) {
				printf("%s%s", sep, "POINTTOPOINT");
				sep[0] = ',';
			}
			if (lifr[i].lifr_flags & IFF_RUNNING) {
				printf("%s%s", sep, "RUNNING");
				sep[0] = ',';
			}
			if (lifr[i].lifr_flags & IFF_NOARP) {
				printf("%s%s", sep, "NOARP");
				sep[0] = ',';
			}
			if (lifr[i].lifr_flags & IFF_PROMISC) {
				printf("%s%s", sep, "PROMISC");
				sep[0] = ',';
			}
			if (lifr[i].lifr_flags & IFF_NOTRAILERS) {
				printf("%s%s", sep, "NOTRAILERS");
				sep[0] = ',';
			}
			if (lifr[i].lifr_flags & IFF_ALLMULTI) {
				printf("%s%s", sep, "ALLMULTI");
				sep[0] = ',';
			}
			printf(">\n");
		}
		break;
	}
	default:
	{
		int i, num, result = 0;
		struct ifconf ifc;
		struct ifreq *ifr;
		size_t size;

		if (interface[0] == '\0') {
			if ((ret = ioctl(fd, SIOCGIFNUM, &result)) < 0) {
				perror(__FUNCTION__);
				exit(1);
			}
			printf("SIOCGIFNUM: result = %d\n", result);
			if (result <= 0)
				break;
			size = result * sizeof(struct ifreq);
			if ((ifr = malloc(size)) == NULL) {
				perror(__FUNCTION__);
				exit(1);
			}
			memset(ifr, 0, size);
			ifc.ifc_len = size;
			ifc.ifc_req = ifr;
			if ((ret = ioctl(fd, SIOCGIFCONF, &ifc)) < 0) {
				perror(__FUNCTION__);
				exit(1);
			}
			printf("SIOCGIFCONF: ifc_len = %d\n", ifc.ifc_len);
			if ((num = ifc.ifc_len / sizeof(struct ifreq)) <= 0)
				break;
		} else {
			size = sizeof(struct ifreq);
			if ((ifr = malloc(size)) == NULL) {
				perror(__FUNCTION__);
				exit(1);
			}
			memset(ifr, 0, size);
			ifc.ifc_len = size;
			ifc.ifc_req = ifr;
			strncpy(ifr[0].ifr_name, interface, IFNAMSIZ);
			num = 1;
		}
		for (i = 0; i < num; i++) {
			char sep[2] = { '\0', '\0' };

			printf("SIOCGIFCONF(%d): ifr_name = %s\n", i, ifr[i].ifr_name);
			printf("SIOCGIFCONF(%d): ifr_addr.sa_family = ", i);
			switch (ifr[i].ifr_addr.sa_family) {
			case AF_UNSPEC:
				printf("AF_UNSPEC");
				break;
			case AF_UNIX:
				printf("AF_UNIX");
				break;
			case AF_INET:
				printf("AF_INET");
				break;
			case AF_INET6:
				printf("AF_INET6");
				break;
			case AF_IPX:
				printf("AF_IPX");
				break;
			case AF_NETLINK:
				printf("AF_NETLINK");
				break;
			case AF_X25:
				printf("AF_X25");
				break;
			case AF_AX25:
				printf("AF_AX25");
				break;
			case AF_ATMPVC:
				printf("AF_ATMPVC");
				break;
			case AF_APPLETALK:
				printf("AF_APPLETALK");
				break;
			case AF_PACKET:
				printf("AF_PACKET");
				break;
			default:
				printf("%d", ifr[i].ifr_addr.sa_family);
				break;
			}
			printf("\n");
			if ((ret = ioctl(fd, SIOCGIFFLAGS, &ifr[i])) < 0) {
				perror(__FUNCTION__);
				exit(1);
			}
			printf("SIOCGIFFLAGS(%d): ifr_name = %s\n", i, ifr[i].ifr_name);
			printf("SIOCGIFFLAGS(%d): ifr_flags = <", i);
			if (ifr[i].ifr_flags & IFF_UP) {
				printf("%s%s", sep, "UP");
				sep[0] = ',';
			}
			if (ifr[i].ifr_flags & IFF_BROADCAST) {
				printf("%s%s", sep, "BROADCAST");
				sep[0] = ',';
			}
			if (ifr[i].ifr_flags & IFF_DEBUG) {
				printf("%s%s", sep, "DEBUG");
				sep[0] = ',';
			}
			if (ifr[i].ifr_flags & IFF_LOOPBACK) {
				printf("%s%s", sep, "LOOPBACK");
				sep[0] = ',';
			}
			if (ifr[i].ifr_flags & IFF_POINTTOPOINT) {
				printf("%s%s", sep, "POINTTOPOINT");
				sep[0] = ',';
			}
			if (ifr[i].ifr_flags & IFF_RUNNING) {
				printf("%s%s", sep, "RUNNING");
				sep[0] = ',';
			}
			if (ifr[i].ifr_flags & IFF_NOARP) {
				printf("%s%s", sep, "NOARP");
				sep[0] = ',';
			}
			if (ifr[i].ifr_flags & IFF_PROMISC) {
				printf("%s%s", sep, "PROMISC");
				sep[0] = ',';
			}
			if (ifr[i].ifr_flags & IFF_NOTRAILERS) {
				printf("%s%s", sep, "NOTRAILERS");
				sep[0] = ',';
			}
			if (ifr[i].ifr_flags & IFF_ALLMULTI) {
				printf("%s%s", sep, "ALLMULTI");
				sep[0] = ',';
			}
			printf(">\n");
		}
		break;
	}
	}
	close(fd);
}

int
main(int argc, char **argv)
{
	int c;
	int val;
	int alt = 0;

	my_argc = argc;
	my_argv = argv;

	while (1) {
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"device",	required_argument,	NULL,	'd'},
			{"interface",	required_argument,	NULL,	'i'},
			{"alternate",	no_argument,		NULL,	'a'},
			{"quiet",	no_argument,		NULL,	'q'},
			{"verbose",	optional_argument,	NULL,	'v'},
			{"help",	no_argument,		NULL,	'h'},
			{"version",	no_argument,		NULL,	'V'},
			{"copying",	no_argument,		NULL,	'C'},
			{"?",		no_argument,		NULL,	'h'},
			{NULL,		0,			NULL,	 0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "d:i:aqvhVC?", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case 'd':	/* -d, --device DEVPATH */
			if (strnlen(optarg, sizeof(devname)) > sizeof(devname) - 1)
				goto bad_option;
			strncpy(devname, optarg, sizeof(devname));
			break;
		case 'i':	/* -i, --interface INTERFACE */
			if (strnlen(optarg, sizeof(interface)) > sizeof(interface) - 1)
				goto bad_option;
			strncpy(interface, optarg, sizeof(interface));
			break;
		case 'a':	/* -a, --alternate */
			alt = 1;
			break;
		case 'q':	/* -q, --quiet */
			output -= output > 0 ? 1 : output;
			break;
		case 'v':	/* -v, --verbose [LEVEL] */
			if (optarg == NULL) {
				output += 1;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			break;
		case 'h':	/* -h, --help, -?, --? */
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			version(argc, argv);
			exit(0);
		case 'C':
			copying(argc, argv);
			exit(0);
		case ':':
			fprintf(stderr, "%s: missing parm -- %s\n", argv[0], argv[optind - 1]);
			usage(argc, argv);
			exit(2);
		case '?':
		default:
		      bad_option:
			optind--;
			// syntax error
			if (optind < argc) {
				fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
				for (; optind < argc; optind++)
					fprintf(stderr, "%s ", argv[optind]);
				fprintf(stderr, "\n");
			}
			usage(argc, argv);
			exit(2);
		}
	}
	soconf_get(alt);
}
