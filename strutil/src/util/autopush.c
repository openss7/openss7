/*****************************************************************************

 @(#) $RCSfile: autopush.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/04/28 01:30:34 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/04/28 01:30:34 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: autopush.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/04/28 01:30:34 $"

static char const ident[] =
    "$RCSfile: autopush.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2004/04/28 01:30:34 $";

/* 
 *  autopush(8)
 *
 *  AIX Utility: autopush - Configures lists of automatically pushed STREAMS
 *  modules.
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>

#include <sys/stropts.h>
#include <sys/sad.h>

#define MAX_MAJOR_DEVICE    254
#define MAX_MINOR_DEVICE    255

#define SAD_USER_FILENAME   "/dev/sad/user"
#define SAD_ADMIN_FILENAME  "/dev/sad/admin"

static int debug = 0;
static int output = 1;

static void version(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
%1$s:\n\
    %2$s\n\
    Copyright (c) 2001-2004  OpenSS7 Corporation.  All Rights Reserved.\n\
    Distributed under GPL Version 2, included here by reference.\n\
", argv[0], ident);
}

static void usage(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] {-f|--file} filename\n\
    %1$s [options] {-g|--get} [{-M [major|name]|-N name} [-m minor]]\n\
    %1$s [options] {-r|--reset} [{-M [major|name]|-N name} [-m minor]]\n\
    %1$s [options] {-s|--set} {-M [major|name]|-N name} [-m minor [-l lastminor]] module ...\n\
    %1$s [options] {-c|--clone} {-M [major|name]|-N name} [-m minor]\n\
    %1$s [options] {-v|--verify} module ...\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
", argv[0]);
}

static void help(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options] {-f|--file} filename\n\
    %1$s [options] {-g|--get} [{-M [major|name]|-N name} [-m minor]]\n\
    %1$s [options] {-r|--reset} [{-M [major|name]|-N name} [-m minor]]\n\
    %1$s [options] {-s|--set} {-M [major|name]|-N name} [-m minor [-l lastminor]] module ...\n\
    %1$s [options] {-c|--clone} {-M [major|name]|-N name} [-m minor]\n\
    %1$s [options] {-v|--verify} module ...\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
Arguments:\n\
    filename\n\
        filename from which to load autopush configuration\n\
    module ...\n\
        list of module names to set or verify\n\
Options:\n\
    -f, --file filename\n\
        filename from which to read autopush configuration\n\
    -g, --get\n\
        get the autopush list for the specified device (if no device\n\
        is specified, get list for all devices)\n\
    -r, --reset\n\
        reset (clear) the autopush list for the specified device (if\n\
        no device specified, reset list for all devices)\n\
    -s, --set module ...\n\
        set the autopush list for the specified device\n\
    -c, --clone\n\
        set as clonable minro the specified device\n\
    -v, --verify module ...\n\
        verify the module list (that names are valid and loaded)\n\
    -N, --name name\n\
        specify the device name (cannot be used with -M)\n\
    -M, --major [major|name]\n\
        specify the major device number or name (cannot be used with -N)\n\
    -m, --minor minor\n\
        specify the minor device number (used with -M or -N)\n\
    -l, --lastminor lastminor\n\
        specify the last minor device number in a range (used with -s)\n\
    -d, --debug\n\
        sets debugging verbosity\n\
    -q, --quiet\n\
        suppress output\n\
    -h, --help\n\
        prints this usage information and exits\n\
    -V, --version\n\
        prints the version and exits\n\
", argv[0]);
}

static int sad_cmd(int fd, int cmd, struct strapush *sap)
{
	struct strioctl ioc = {
		ic_cmd:cmd,
		ic_timout:0,
		ic_len:sizeof(*sap),
		ic_dp:(char *) sap,
	};
	return ioctl(fd, I_STR, &ioc);
}
static int sad_vml(int fd, struct str_list *sml)
{
	struct strioctl ioc = {
		ic_cmd:I_STR,
		ic_timout:0,
		ic_len:sizeof(*sml) + MAXAPUSH * sizeof(struct str_mlist),
		ic_dp:(char *) sml,
	};
	return ioctl(fd, I_STR, &ioc);
}

static int autopush_set(char *devname, int major, int minor, int lastminor, int nmods,
			char **modlist)
{
	int i;
	if (debug) {
		fprintf(stderr,
			"%s: devname=%s, major=%d, minor=%d, lastminor=%d, nmods=%d, modlist=",
			__FUNCTION__, devname, major, minor, lastminor, nmods);
		for (i = 0; i < nmods; i++)
			fprintf(stderr, "\"%s\" ", modlist[i]);
		fprintf(stderr, "\n");
	}
	return (1);
}

static int autopush_cln(char *devname, int major, int minor)
{
	if (debug)
		fprintf(stderr, "%s: devname=%s, major=%d, minor=%d\n", __FUNCTION__, devname,
			major, minor);
	return (1);
}

static int autopush_ver(int nmods, char **modlist)
{
	int i, fd;
	union {
		struct str_list list;
		struct str_mlist mlist[MAXAPUSH];
	} sml;
	if (debug) {
		fprintf(stderr, "%s: nmods = %d, modlist=", __FUNCTION__, nmods);
		for (i = 0; i < nmods; i++)
			fprintf(stderr, "\"%s\" ", modlist[i]);
		fprintf(stderr, "\n");
	}
	if ((fd = open(SAD_USER_FILENAME, O_RDWR | O_NONBLOCK)) < 0) {
		if (output || debug)
			perror("autopush_ver: " SAD_USER_FILENAME);
		return (1);
	}
	memset(&sml, 0, sizeof(sml));
	sml.list.sl_nmods = nmods;
	sml.list.sl_modlist = &sml.mlist[0];
	for (i = 0; i < nmods; i++)
		snprintf(sml.mlist[i].l_name, FMNAMESZ + 1, modlist[i]);
	if (sad_vml(fd, &sml.list) < 0) {
		if (output || debug)
			perror(__FUNCTION__);
		return (1);
	}
	close(fd);
	return (0);
}

static int autopush_get(char *devname, int major, int minor)
{
	struct strapush sap;
	int header = 0;
	int fd;
	if (debug)
		fprintf(stderr, "%s: devname=%s, major=%d, minor=%d\n", __FUNCTION__, devname,
			major, minor);
	if ((fd = open(SAD_USER_FILENAME, O_RDWR | O_NONBLOCK)) < 0) {
		if (output || debug)
			perror("autopush_get: " SAD_USER_FILENAME);
		return (1);
	}
	if (minor == -1)
		minor = 0;
	if (major != -1 || *devname != '\0') {
		memset(&sap, 0, sizeof(sap));
		sap.sap_cmd = 0;
		sap.sap_major = major;
		sap.sap_minor = minor;
		sap.sap_lastminor = 0;
		sap.sap_npush = MAXAPUSH;
		strncpy(devname, sap.sap_module, FMNAMESZ);
		if (sad_cmd(fd, SAD_GAP, &sap) < 0) {
			if (output || debug)
				perror(__FUNCTION__);
			return (1);
		}
		if (output) {
			printf("DeviceName Major Minor Lastminor Modules\n");
			printf("%10s       %3d   %3d   %3s       ", sap.sap_module, sap.sap_major,
			       sap.sap_minor, "-");
		}
		close(fd);
		return (0);
	}
	for (major = 0; major <= MAX_MAJOR_DEVICE; major++) {
		memset(&sap, 0, sizeof(sap));
		sap.sap_cmd = 0;
		sap.sap_major = major;
		sap.sap_minor = minor;
		sap.sap_lastminor = 0;
		sap.sap_npush = MAXAPUSH;
		strncpy(devname, sap.sap_module, FMNAMESZ);
		if (sad_cmd(fd, SAD_GAP, &sap) < 0) {
			switch (errno) {
			case ENOSTR:	/* skip that device */
			case ENODEV:	/* skip that device */
				continue;
			case EINVAL:	/* last device */
				if (header)
					break;
			default:
				if (output || debug)
					perror(__FUNCTION__);
				return (1);
			}
		}
		if (!header) {
			if (output)
				printf("DeviceName Major Minor Lastminor Modules\n");
			header = 1;
		}
		if (output)
			printf("%10s       %3d   %3d   %3s       ", sap.sap_module,
			       sap.sap_major, sap.sap_minor, "-");
	}
	close(fd);
	return (0);
}

static int autopush_res(char *devname, int major, int minor)
{
	struct strapush sap;
	int header = 0;
	int fd;
	if (debug)
		fprintf(stderr, "%s: devname=%s, major=%d, minor=%d\n", __FUNCTION__, devname,
			major, minor);
	if ((fd = open(SAD_USER_FILENAME, O_RDWR | O_NONBLOCK)) < 0) {
		if (output || debug)
			perror("autopush_res: " SAD_USER_FILENAME);
		return (1);
	}
	if (major != -1 || *devname != '\0') {
		memset(&sap, 0, sizeof(sap));
		sap.sap_cmd = SAP_CLEAR;
		sap.sap_major = major;
		sap.sap_minor = minor;
		sap.sap_lastminor = 0;
		sap.sap_npush = MAXAPUSH;
		strncpy(devname, sap.sap_module, FMNAMESZ);
		if (sad_cmd(fd, SAD_SAP, &sap) < 0) {
			if (output || debug)
				perror(__FUNCTION__);
			return (1);
		}
		if (output) {
			printf("DeviceName Major Minor Lastminor Modules\n");
			printf("%10s       %3d   %3d   %3s       ", sap.sap_module, sap.sap_major,
			       sap.sap_minor, "-");
		}
		close(fd);
		return (0);
	}
	for (major = 0; major <= MAX_MAJOR_DEVICE; major++) {
		memset(&sap, 0, sizeof(sap));
		sap.sap_cmd = SAP_CLEAR;
		sap.sap_major = major;
		sap.sap_minor = 0;
		sap.sap_lastminor = 0;
		sap.sap_npush = MAXAPUSH;
		strncpy(devname, sap.sap_module, FMNAMESZ);
		if (sad_cmd(fd, SAD_SAP, &sap) < 0) {
			switch (errno) {
			case ENOSTR:	/* skip that device */
			case ENODEV:	/* skip that device */
				continue;
			case EINVAL:	/* last device */
				if (header)
					break;
			default:
				if (output || debug)
					perror(__FUNCTION__);
				return (1);
			}
		}
		if (!header) {
			if (output)
				printf("DeviceName Major Minor Lastminor Modules\n");
			header = 1;
		}
		if (output)
			printf("%10s       %3d   %3d   %3s       ", sap.sap_module,
			       sap.sap_major, sap.sap_minor, "-");
	}
	close(fd);
	return (0);
}

static int autopush_fil(char *filename)
{
	char buffer[1024];
	char *line;
	int lineno, fd;
	FILE *file;
	if ((file = fopen(filename, "r")) == NULL) {
		if (output || debug)
			perror(__FUNCTION__);
		return (1);
	}
	/* read the file one line at a time */
	for (lineno = 1; (line = fgets(buffer, sizeof(buffer), file)) != NULL; lineno++) {
		struct strapush sap;
		char *str, *token, *tmp = NULL;
		int tokenind;
		int major = -1, minor = -1, lastminor = -1;
		unsigned char devname[FMNAMESZ + 1] = "";
		for (str = line, tokenind = 0, sap.sap_npush = 0; tokenind < MAXAPUSH + 3 &&
		     (token = strtok_r(str, " \t\f\n\r\v", &tmp)) != NULL; str = NULL, tokenind++) {
			switch (tokenind) {
				int i;
				char *end;
			case 0:	/* major | name */
				if (token[0] == '#')
					goto skip_line;
				if ('0' <= token[0] || token[0] <= '9') {
					major = strtol(token, &end, 0);
				} else {
					strncpy(devname, optarg, FMNAMESZ);
				}
				if (end[0] != '\0')
					break;
				if (major < 1 || major > MAX_MAJOR_DEVICE)
					break;
				continue;
			case 1:	/* minor */
				if (token[0] == '#')
					break;
				minor = strtol(token, &end, 0);
				if (end[0] != '\0')
					break;
				if (minor != -1 && (minor < 0 || minor > MAX_MINOR_DEVICE))
					break;
				continue;
			case 2:	/* lastminor */
				if (token[0] == '#')
					break;
				lastminor = strtol(token, &end, 0);
				if (end[0] != '\0')
					break;
				if (lastminor != -1 &&
				    (lastminor < 0 || lastminor > MAX_MINOR_DEVICE ||
				     lastminor < minor))
					break;
				continue;
			default:	/* module name (tokenind - 3) */
				if (token[0] == '#')
					break;
				strncpy(sap.sap_list[sap.sap_npush++], token, FMNAMESZ);
				continue;
			}
			break;
		}
		if (tokenind < 3)
			goto error;
		sap.sap_major = major;
		strncpy(devname, sap.sap_module, FMNAMESZ);
		if (minor == -1) {
			sap.sap_cmd = SAP_ALL;
			sap.sap_minor = 0;
			sap.sap_lastminor = 0;
		} else if (lastminor == -1) {
			sap.sap_cmd = SAP_CLONE;
			sap.sap_minor = minor;
			sap.sap_lastminor = 0;
		} else if (lastminor == 0) {
			sap.sap_cmd = SAP_ONE;
			sap.sap_minor = minor;
			sap.sap_lastminor = 0;
		} else if (lastminor >= minor) {
			sap.sap_cmd = SAP_RANGE;
			sap.sap_minor = minor;
			sap.sap_lastminor = lastminor;
		} else {
			sap.sap_cmd = SAP_ONE;
			sap.sap_minor = minor;
			sap.sap_lastminor = 0;
		}
		if (tokenind == 3 && sap.sap_cmd != SAP_CLONE)
			goto error;
		if ((fd = open(SAD_ADMIN_FILENAME, O_RDWR | O_NONBLOCK)) < 0) {
			if (output || debug)
				perror(__FUNCTION__);
			return (1);
		}
		if (sad_cmd(fd, SAD_SAP, &sap) < 0) {
			if (output || debug)
				perror(__FUNCTION__);
			return (1);
		}
		close(fd);
	      skip_line:
		continue;
	      error:
		/* print error */
		continue;
	}
	fclose(file);
}

int main(int argc, char **argv)
{
	int c;
	unsigned char filename[256] = "";
	unsigned char devname[FMNAMESZ + 1] = "";
	int major = -1;
	int minor = -1;
	int lastminor = -1;
	enum options {
		OPTION_NONE = 0,
		OPTION_FILE,
		OPTION_SET,
		OPTION_GET,
		OPTION_RESET,
		OPTION_VERIFY,
		OPTION_CLONE,
	} option = OPTION_NONE;
	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"file", 1, 0, 'f'},
			{"set", 0, 0, 's'},
			{"get", 0, 0, 'g'},
			{"reset", 0, 0, 'r'},
			{"clone", 0, 0, 'c'},
			{"verify", 0, 0, 'v'},
			{"name", 1, 0, 'N'},
			{"major", 1, 0, 'M'},
			{"minor", 1, 0, 'm'},
			{"lastminor", 1, 0, 'l'},
			{"debug", 0, 0, 'd'},
			{"help", 0, 0, 'h'},
			{"version", 0, 0, 'V'},
			{"quiet", 0, 0, 'q'},
		};
		c = getopt_long_only(argc, argv, "hf:sgrcM:m:N:l:vVdq", long_options,
				     &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case 'h':	/* -h, --help */
			help(argc, argv);
			exit(0);
		case 'f':	/* -f, --file filename */
			if (option != OPTION_NONE)
				goto bad_option;
			strncpy(filename, optarg, sizeof(filename) - 1);
			option = OPTION_FILE;
			break;
		case 's':	/* -s, --set */
			if (option != OPTION_NONE)
				goto bad_option;
			option = OPTION_SET;
			break;
		case 'g':	/* -g, --get */
			if (option != OPTION_NONE)
				goto bad_option;
			option = OPTION_GET;
			break;
		case 'r':	/* -r, --reset */
			if (option != OPTION_NONE)
				goto bad_option;
			option = OPTION_RESET;
			break;
		case 'M':	/* -M, --major [ major | name ] */
			if (option == OPTION_NONE || option == OPTION_FILE ||
			    option == OPTION_VERIFY || *devname != 0)
				goto bad_option;
			if ('0' <= *optarg && *optarg <= '9') {
				/* -M, --major major */
				major = atoi(optarg);
				if (major < 1 || major > MAX_MAJOR_DEVICE)
					goto bad_option;
			} else {
				/* -M, --major name */
				strncpy(devname, optarg, FMNAMESZ);
			}
			break;
		case 'm':	/* -m, --minor minor */
			if (option == OPTION_NONE || option == OPTION_FILE ||
			    option == OPTION_VERIFY || minor != -1)
				goto bad_option;
			minor = atoi(optarg);
			if (minor < 0 || minor > MAX_MINOR_DEVICE)
				goto bad_option;
			break;
		case 'N':	/* -N, --name name */
			if (option == OPTION_NONE || option == OPTION_FILE ||
			    option == OPTION_VERIFY || major != -1)
				goto bad_option;
			strncpy(devname, optarg, FMNAMESZ);
			break;
		case 'l':	/* -l, --lastminor lastminor */
			if (option != OPTION_SET || minor == -1)
				goto bad_option;
			lastminor = atoi(optarg);
			if (lastminor < 0 || lastminor > MAX_MINOR_DEVICE || lastminor < minor)
				goto bad_option;
			break;
		case 'v':	/* -v, --verify */
			if (option != OPTION_NONE)
				goto bad_option;
			option = OPTION_VERIFY;
			break;
		case 'c':	/* -c, --clone */
			if (option != OPTION_NONE)
				goto bad_option;
			option = OPTION_CLONE;
			break;
		case 'V':	/* -V, --version */
			version(argc, argv);
			exit(0);
		case 'd':	/* -d, --debug */
			debug += 1;
			break;
		case 'q':	/* -q, --quiet */
			output = 0;
			debug = 0;
			break;
		case '?':
		default:
		      bad_option:
			optind--;
		      syntax_error:
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
	switch (option) {
	case OPTION_FILE:
		if (optind < argc)
			goto syntax_error;
		exit(autopush_fil(filename));
	case OPTION_SET:
		if (optind >= argc)
			goto option_reset;
		if (argc - optind > MAXAPUSH) {
			if (debug)
				fprintf(stderr, "%s: too many ( %d > %d ) module names\n", argv[0],
					argc - optind, MAXAPUSH);
			optind += MAXAPUSH;
			goto syntax_error;
		}
		exit(autopush_set(devname, major, minor, lastminor, argc - optind, &argv[optind]));
	case OPTION_GET:
		if (optind < argc)
			goto syntax_error;
		exit(autopush_get(devname, major, minor));
	case OPTION_RESET:
	      option_reset:
		if (optind < argc)
			goto syntax_error;
		if ((*devname == 0 && major == -1) || minor == -1) {
			if (debug)
				fprintf(stderr, "%s: one of -N or -M and -m must be set\n",
					argv[0]);
			goto syntax_error;
		}
		exit(autopush_res(devname, major, minor));
	case OPTION_VERIFY:
		if (optind >= argc)
			exit(0);	/* no modules to verify */
		if (argc - optind > MAXAPUSH) {
			if (debug)
				fprintf(stderr, "%s: too many ( %d > %d ) module names\n", argv[0],
					argc - optind, MAXAPUSH);
			optind += MAXAPUSH;
			goto syntax_error;
		}
		exit(autopush_ver(argc - optind, &argv[optind]));
	case OPTION_CLONE:
		if (optind < argc)
			goto syntax_error;
		if ((*devname == 0 && major == -1) || minor == -1) {
			if (debug)
				fprintf(stderr, "%s: one of -N or -M and -m must be set\n",
					argv[0]);
			goto syntax_error;
		}
		exit(autopush_cln(devname, major, minor));
	default:
	case OPTION_NONE:
		usage(argc, argv);
		exit(2);
	}
}
