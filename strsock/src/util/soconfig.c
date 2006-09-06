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
	struct stat stat;
	struct socknewproto prot;
	int fd;
	struct strioctl ioc;

	if (stat(path, &stat) == -1) {
		fprintf(stderr, "%s: strerror(errno)", "soconfig", strerror(errno));
		exit(1);
	}
	prot.family = family;
	prot.type = type;
	prot.proto = protocol;
	prot.dev = stat.st_rdev;
	prot.flags = 0;
	if ((fd = open("/dev/socksys", O_RDWR)) == -1) {
		fprintf(stderr, "%s: strerror(errno)", "soconfig", strerror(errno));
		exit(1);
	}
	ioc.cmd = SIOCPROTO;
	ioc.len = sizeof(prot);
	ioc.dp = (char *)&prot;
	ioc.timout = -1;
	if (ioctl(fd, I_STR, &ioc) == -1) {
		fprintf(stderr, "%s: strerror(errno)", "soconfig", strerror(errno));
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
		fprintf(stderr, "%s: strerror(errno)", "soconfig", strerror(errno));
		exit(1);
	}
	ioc.cmd = SIOCXPROTO;
	ioc.dp = NULL;
	ioc.len = 0;
	ioc.timout = -1;
	if (ioctl(fd, I_STR, &ioc) == -1) {
		fprintf(stderr, "%s: strerror(errno)", "soconfig", strerror(errno));
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
	int i, fd, count;

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
}
