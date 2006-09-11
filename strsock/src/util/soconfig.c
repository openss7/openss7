/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <stropts.h>
#include <sys/sc.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/sockio.h>
#include <sys/sockmod.h>
#include <sys/socksys.h>

static int debug = 0;			/* default no debug */
static int output = 1;			/* default normal output */

static void
version(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
%2$s\n\
Copyright (c) 2001-2006  OpenSS7 Corporation.  All Rights Reserved.\n\
Distributed under GPL Version 2, included here by reference.\n\
See `%1$s --copying' for copying permissions.\n\
", argv[0], ident);
}

static void
usage(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] [{-l|--list}]\n\
    %1$s [options] [{-f|--file}] FILE\n\
    %1$s [options] [{-c|--clear}] FAMILY TYPE PROTOCOL\n\
    %1$s [options] [{-s|--set}] FAMILY TYPE PROTOCOL PATH\n\
    %1$s [options] {-r|--reset}\n\
    %1$s [options] {-f|--file} [FILENAME]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

static void
help(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options] [{-l|--list}]\n\
    %1$s [options] [{-f|--file}] FILE\n\
    %1$s [options] [{-c|--clear}] FAMILY TYPE PROTOCOL\n\
    %1$s [options] [{-s|--set}] FAMILY TYPE PROTOCOL PATH\n\
    %1$s [options] {-r|--reset}\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Options:\n\
    [-l, --list]\n\
	list the kernel socket to transport mapping\n\
        this option is assumed when there are zero non-option arguments\n\
    [-f, --file] FILE\n\
	set the kernel socket to transport mapping from FILE\n\
        this option is assumed when there is one non-option argument\n\
    [-c, --clear] FAMILY TYPE PROTOCOL\n\
        clear the specific kernel socket to transport mapping entry\n\
        this option is assumed when there are three non-option arguments\n\
    [-s, --set] FAMILY TYPE PROTOCOL PATH\n\
        set the specific socket to transport mapping entry\n\
        this option is assumed when there are four non-option arguments\n\
    -r, --reset\n\
        reset the entire kernel socket to transport mapping table\n\
    -q, --quiet\n\
        suppress output\n\
    -d, --debug [LEVEL]\n\
        increase or set debugging verbosity\n\
    -v, --verbose [LEVEL]\n\
        increase or set output verbosity\n\
    -h, --help\n\
        prints this usage information and exits\n\
    -V, --version\n\
        prints the version and exits\n\
    -C, --copying\n\
        prints copying permissions and exits\n\
", argv[0]);
}

static void
copying(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com>\n\
Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software; you can  redistribute  it and/or modify  it under\n\
the terms  of the GNU General Public License  as  published by the Free Software\n\
Foundation; version  2  of  the  License.\n\
\n\
This program is distributed in the hope that it will  be useful, but WITHOUT ANY\n\
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU General Public License for more details.\n\
\n\
You should  have received a copy of the GNU  General  Public License  along with\n\
this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave,\n\
Cambridge, MA 02139, USA.\n\
--------------------------------------------------------------------------------\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the U.S. Government (\"Government\"), the following provisions apply to you. If\n\
the Software is supplied by the  Department of Defense (\"DoD\"), it is classified\n\
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

/**
 * cmn_list - list the sock2path mapping in sock2path file format
 */
void
cmn_list(void)
{
}

/**
 * cmn_clear - clear the protocol entry
 * @family: protocol family
 * @type: socket type
 * @protocol: protocol within family
 */
void
cmn_clear(int family, int type, int protocol)
{
}

/**
 * cmn_set - set the protocol entry
 * @family: protocol family
 * @type: socket type
 * @protocol: protocol within family
 * @path: path to device
 */
void
cmn_set(int family, int type, int protocol, const char *path)
{
	struct stat st;
	struct socknewproto prot;
	int fd;
	struct strioctl ioc;

	if (stat(path, &st) == -1) {
		fprintf(stderr, "%s: %s", "soconfig", strerror(errno));
		exit(1);
	}
	prot.family = family;
	prot.type = type;
	prot.proto = protocol;
	prot.dev = st.st_rdev;
	prot.flags = 0;
	if ((fd = open("/dev/socksys", O_RDWR)) == -1) {
		fprintf(stderr, "%s: %s", "soconfig", strerror(errno));
		exit(1);
	}
	ioc.ic_cmd = SIOCPROTO;
	ioc.ic_len = sizeof(prot);
	ioc.ic_dp = (char *)&prot;
	ioc.ic_timout = -1;
	if (ioctl(fd, I_STR, &ioc) == -1) {
		fprintf(stderr, "%s: %s", "soconfig", strerror(errno));
		close(fd);
		exit(1);
	}
	close(fd);
	return;
}

/**
 * cmn_reset - reset the sock2path mapping
 */
void
cmn_reset(void)
{
	int fd;
	struct strioctl ioc;

	if ((fd = open("/dev/socksys", O_RDWR)) == -1) {
		fprintf(stderr, "%s: %s", "soconfig", strerror(errno));
		exit(1);
	}
	ioc.ic_cmd = SIOCXPROTO;
	ioc.ic_dp = NULL;
	ioc.ic_len = 0;
	ioc.ic_timout = -1;
	if (ioctl(fd, I_STR, &ioc) == -1) {
		fprintf(stderr, "%s: %s", "soconfig", strerror(errno));
		close(fd);
		exit(1);
	}
	close(fd);
	return;
}

/**
 * cmn_file - set the sock2path mapping from a sock2path formatted file
 * @filename: file to use
 */
void
cmn_file(const char *filename)
{
}

enum { CMN_NONE, CMN_LIST, CMN_CLEAR, CMN_SET, CMN_RESET, CMN_FILE, } command = CMN_NONE;
int option_all = 0;

int family = -1;
int socktype = -1;
int protocol = -1;
char path[PATH_MAX] = "";
char filename[PATH_MAX] = "/etc/sock2path";

struct strmap {
	const char *name;
	int value;
};

struct strmap families[] = {
	{"AF_UNSPEC", AF_UNSPEC},
	{"AF_LOCAL", AF_LOCAL},
	{"AF_UNIX", AF_UNIX},
	{"AF_FILE", AF_FILE},
	{"AF_INET", AF_INET},
	{"AF_AX25", AF_AX25},
	{"AF_IPX", AF_IPX},
	{"AF_APPLETALK", AF_APPLETALK},
	{"AF_NETROM", AF_NETROM},
	{"AF_BRIDGE", AF_BRIDGE},
	{"AF_ATMPVC", AF_ATMPVC},
	{"AF_X25", AF_X25},
	{"AF_INET6", AF_INET6},
	{"AF_ROSE", AF_ROSE},
	{"AF_DECnet", AF_DECnet},
	{"AF_NETBEUI", AF_NETBEUI},
	{"AF_SECURITY", AF_SECURITY},
	{"AF_KEY", AF_KEY},
	{"AF_NETLINK", AF_NETLINK},
	{"AF_ROUTE", AF_ROUTE},
	{"AF_PACKET", AF_PACKET},
	{"AF_ASH", AF_ASH},
	{"AF_ECONET", AF_ECONET},
	{"AF_ATMSVC", AF_ATMSVC},
	{"AF_SNA", AF_SNA},
	{"AF_IRDA", AF_IRDA},
	{"AF_PPPOX", AF_PPPOX},
	{"AF_WANPIPE", AF_WANPIPE},
	{"AF_BLUETOOTH", AF_BLUETOOTH},
	{NULL, 0}
};
int
strtofamily(const char *string)
{
	struct strmap *map = families;
	char *endptr = NULL;
	int value;

	value = strtoul(string, &endptr, 0);
	if (*endptr)
		return (value);

	while (map->name) {
		if (strcmp(string, map->name) == 0)
			return (map->value);
		map++;
	}
	return (-1);
}

struct strmap socktypes[] = {
	{"SOCK_STREAM", SOCK_STREAM},
	{"SOCK_DGRAM", SOCK_DGRAM},
	{"SOCK_RAW", SOCK_RAW},
	{"SOCK_RDM", SOCK_RDM},
	{"SOCK_SEQPACKET", SOCK_SEQPACKET},
	{"SOCK_PACKET", SOCK_PACKET},
	{NULL, 0}
};
int
strtosocktype(const char *string)
{
	struct strmap *map = socktypes;
	char *endptr = NULL;
	int value;

	value = strtoul(string, &endptr, 0);
	if (*endptr)
		return (value);

	while (map->name) {
		if (strcmp(string, map->name) == 0)
			return (map->value);
		map++;
	}
	return (-1);
}

struct strmap protocols[] = {
	{"IPPROTO_IP", IPPROTO_IP},
	{"IPPROTO_HOPOPTS", IPPROTO_HOPOPTS},
	{"IPPROTO_ICMP", IPPROTO_ICMP},
	{"IPPROTO_IGMP", IPPROTO_IGMP},
	{"IPPROTO_IPIP", IPPROTO_IPIP},
	{"IPPROTO_TCP", IPPROTO_TCP},
	{"IPPROTO_EGP", IPPROTO_EGP},
	{"IPPROTO_PUP", IPPROTO_PUP},
	{"IPPROTO_UDP", IPPROTO_UDP},
	{"IPPROTO_IDP", IPPROTO_IDP},
	{"IPPROTO_TP", IPPROTO_TP},
	{"IPPROTO_IPV6", IPPROTO_IPV6},
	{"IPPROTO_ROUTING", IPPROTO_ROUTING},
	{"IPPROTO_FRAGMENT", IPPROTO_FRAGMENT},
	{"IPPROTO_RSVP", IPPROTO_RSVP},
	{"IPPROTO_GRE", IPPROTO_GRE},
	{"IPPROTO_ESP", IPPROTO_ESP},
	{"IPPROTO_AH", IPPROTO_AH},
	{"IPPROTO_ICMPV6", IPPROTO_ICMPV6},
	{"IPPROTO_NONE", IPPROTO_NONE},
	{"IPPROTO_DSTOPTS", IPPROTO_DSTOPTS},
	{"IPPROTO_MTP", IPPROTO_MTP},
	{"IPPROTO_ENCAP", IPPROTO_ENCAP},
	{"IPPROTO_PIM", IPPROTO_PIM},
	{"IPPROTO_COMP", IPPROTO_COMP},
	{"IPPROTO_RAW", IPPROTO_RAW},
	{NULL, 0}
};
int
strtoprotocol(const char *string)
{
	struct strmap *map = protocols;
	char *endptr = NULL;
	int value;

	value = strtoul(string, &endptr, 0);
	if (*endptr)
		return (value);

	while (map->name) {
		if (strcmp(string, map->name) == 0)
			return (map->value);
		map++;
	}
	return (-1);
}

int
main(int argc, char *argv[])
{
	for (;;) {
		int c, val;

#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"list",	no_argument,		NULL, 'l'},
			{"clear",	no_argument,		NULL, 'c'},
			{"set",		no_argument,		NULL, 's'},
			{"reset",	no_argument,		NULL, 'r'},
			{"file",	no_argument,		NULL, 'f'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"debug",	optional_argument,	NULL, 'D'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'H'},
			{ 0, }
		};
		/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "lcsrfqD::v::hVC?W:", long_options, &option_index);
#else
		c = getopt(argc, argv, "lcsrfqDvhVC?");
#endif
		if (c == -1) {
			if (debug)
				fprintf(stderr, "%s: done options processing\n", argv[0]);
			option_all = 1;
			break;
		}
		switch (c) {
		case 0:
			goto bad_usage;
		case 'l':	/* -l, --list */
			if (command != CMN_NONE)
				goto bad_option;
			if (debug)
				fprintf(stderr, "%s: setting --list command\n", argv[0]);
			command = CMN_LIST;
			break;
		case 'c':	/* -c, --clear */
			if (command != CMN_NONE)
				goto bad_option;
			if (debug)
				fprintf(stderr, "%s: setting --clear command\n", argv[0]);
			command = CMN_CLEAR;
			break;
		case 's':	/* -s, --set */
			if (command != CMN_NONE)
				goto bad_option;
			if (debug)
				fprintf(stderr, "%s: setting --set command\n", argv[0]);
			command = CMN_CLEAR;
			break;
		case 'r':	/* -r, --reset */
			if (command != CMN_NONE)
				goto bad_option;
			if (debug)
				fprintf(stderr, "%s: setting --reset command\n", argv[0]);
			command = CMN_RESET;
			break;
		case 'f':	/* -f, --file */
			if (command != CMN_NONE)
				goto bad_option;
			if (debug)
				fprintf(stderr, "%s: setting --file command\n", argv[0]);
			command = CMN_FILE;
			break;
		case 'D':	/* -D, --debug */
			if (debug)
				fprintf(stderr, "%s: increasing debug verbosity\n", argv[0]);
			if (optarg == NULL) {
				debug++;
			} else {
				if ((val = strtol(optarg, NULL, 0)) < 0)
					goto bad_option;
				debug = val;
			}
			break;
		case 'q':	/* -q, --quiet */
			if (debug)
				fprintf(stderr, "%s: suppressing normal output\n", argv[0]);
			debug = 0;
			output = 0;
			break;
		case 'v':	/* -v, --verbose [level] */
			if (debug)
				fprintf(stderr, "%s: increasing output verbosity\n", argv[0]);
			if (optarg == NULL) {
				output++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			break;
		case 'h':	/* -h, --help */
		case 'H':	/* -H, --? */
			if (debug)
				fprintf(stderr, "%s: printing help message\n", argv[0]);
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			if (debug)
				fprintf(stderr, "%s: printing version message\n", argv[0]);
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copying */
			if (debug)
				fprintf(stderr, "%s: printing copying message\n", argv[0]);
			copying(argc, argv);
			exit(0);
		case '?':
		default:
		      bad_option:
			optind--;
			goto bad_nonopt;
		      bad_nonopt:
			if (output || debug) {
				if (optind < argc) {
					fprintf(stderr, "%s: syntax error near '", argv[0]);
					while (optind < argc)
						fprintf(stderr, "%s ", argv[optind++]);
					fprintf(stderr, "'\n");
				} else {
					fprintf(stderr, "%s: missing option or argument", argv[0]);
					fprintf(stderr, "\n");
				}
				fflush(stderr);
			      bad_usage:
				usage(argc, argv);
			}
			exit(2);
		}
	}
	if (debug)
		fprintf(stderr, "%s: parsing %d nonoption arguments\n", argv[0], argc - optind);
	switch (command) {
	case CMN_NONE:		/* 0, 3, or 4 arguments */
		switch (argc - optind) {
		case 0:
			command = CMN_LIST;
		      arguments_0:
			break;
		case 3:
			command = CMN_CLEAR;
		      arguments_3:
			/* family */
			if ((family = strtofamily(argv[optind++])) == -1)
				goto bad_nonopt;
			/* type */
			if ((socktype = strtosocktype(argv[optind++])) == -1)
				goto bad_nonopt;
			/* protocol */
			if ((protocol = strtoprotocol(argv[optind++])) == -1)
				goto bad_nonopt;
			cmn_clear(family, socktype, protocol);
			break;
		case 4:
			command = CMN_SET;
		      arguments_4:
			/* family */
			if ((family = strtofamily(argv[optind++])) == -1)
				goto bad_nonopt;
			/* type */
			if ((socktype = strtosocktype(argv[optind++])) == -1)
				goto bad_nonopt;
			/* protocol */
			if ((protocol = strtoprotocol(argv[optind++])) == -1)
				goto bad_nonopt;
			/* path */
			optind++;
			cmn_set(family, socktype, protocol, path);
			break;
		default:
		      bad_args:
			switch (command) {
			case CMN_NONE:
				fprintf(stderr, "%s: expecting 0, 3, or 4 arguments\n", argv[0]);
				break;
			case CMN_LIST:
				fprintf(stderr, "%s: --list expects 0 arguments\n", argv[0]);
				break;
			case CMN_CLEAR:
				fprintf(stderr, "%s: --clear expects 3 arguments\n", argv[0]);
				break;
			case CMN_SET:
				fprintf(stderr, "%s: --set expects 4 arguments\n", argv[0]);
				break;
			case CMN_RESET:
				fprintf(stderr, "%s: --reset expects 0 arguments\n", argv[0]);
				break;
			case CMN_FILE:
				fprintf(stderr, "%s: --file expects 0 arguments\n", argv[0]);
				break;
			}
			goto bad_usage;
		}
		break;
	case CMN_LIST:		/* 0 arguments */
		if (argc - optind != 0)
			goto bad_args;
		cmn_list();
		goto arguments_0;
	case CMN_CLEAR:	/* 3 arguments */
		if (argc - optind != 3)
			goto bad_args;
		goto arguments_3;
	case CMN_SET:		/* 4 arguments */
		if (argc - optind != 3)
			goto bad_args;
		goto arguments_4;
	case CMN_RESET:	/* 0 arguments */
		if (argc - optind != 0)
			goto bad_args;
		cmn_reset();
		goto arguments_0;
	case CMN_FILE:		/* 0 arguments */
		if (argc - optind != 0)
			goto bad_args;
		cmn_file(filename);
		goto arguments_0;
	}
	return (0);
}
