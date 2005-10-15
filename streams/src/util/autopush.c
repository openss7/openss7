/*****************************************************************************

 @(#) $RCSfile: autopush.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2005/10/15 10:19:55 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/10/15 10:19:55 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: autopush.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2005/10/15 10:19:55 $"

static char const ident[] = "$RCSfile: autopush.c,v $ $Name:  $($Revision: 0.9.2.14 $) $Date: 2005/10/15 10:19:55 $";

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

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <stropts.h>
#include <sys/sysmacros.h>
#include <sys/ioctl.h>

#include <sys/sad.h>
#include <sys/sc.h>

#define MAX_MAJOR_DEVICE    254
#define MAX_MINOR_DEVICE    255

#define SAD_USER_FILENAME   "/dev/sad"
#define SAD_ADMIN_FILENAME  "/dev/sad"

static int output = 1;
static int debug = 0;

static void
version(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
%2$s\n\
Copyright (c) 2001-2005  OpenSS7 Corporation.  All Rights Reserved.\n\
Distributed under GPL Version 2, included here by reference.\n\
See `%1$s --copying' for copying permissions.\n\
", argv[0], ident);
}

static void
usage(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] {-f|--file} FILENAME\n\
    %1$s [options] {-g|--get} [{-M [MAJOR|NAME]|-N NAME} [-m MINOR]]\n\
    %1$s [options] {-r|--reset} [{-M [MAJOR|NAME]|-N NAME} [-m MINOR]]\n\
    %1$s [options] {-s|--set} {-M [MAJOR|NAME]|-N NAME} [-m MINOR [-l LASTMINOR]] MODULE ...\n\
    %1$s [options] {-c|--clone} {-M [MAJOR|NAME]|-N NAME} [-m MINOR]\n\
    %1$s [options] {-v|--verify} module ...\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

static void
help(int argc, char **argv)
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options] {-f|--file} FILENAME\n\
    %1$s [options] {-g|--get} [{-M [MAJOR|NAME]|-N NAME} [-m MINOR]]\n\
    %1$s [options] {-r|--reset} [{-M [MAJOR|NAME]|-N NAME} [-m MINOR]]\n\
    %1$s [options] {-s|--set} {-M [MAJOR|NAME]|-N NAME} [-m MINOR [-l LASTMINOR]] MODULE ...\n\
    %1$s [options] {-c|--clone} {-M [MAJOR|NAME]|-N NAME} [-m MINOR]\n\
    %1$s [options] {-v|--verify} MODULE ...\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    FILENAME\n\
        filename from which to load autopush configuration\n\
    MODULE ...\n\
        list of module names to set or verify\n\
Options:\n\
    -f, --file FILENAME\n\
        FILENAME from which to read autopush configuration\n\
    -g, --get\n\
        get the autopush list for the specified device (if no device\n\
        is specified, get list for all devices)\n\
    -r, --reset\n\
        reset (clear) the autopush list for the specified device (if\n\
        no device specified, reset list for all devices)\n\
    -s, --set MODULE ...\n\
        set the autopush MODULE list for the specified device\n\
    -c, --clone\n\
        set as clonable minro the specified device\n\
    -v, --verify MODULE ...\n\
        verify the MODULE list (that names are valid and loaded)\n\
    -N, --name NAME\n\
        specify the device NAME (cannot be used with -M)\n\
    -M, --major [MAJOR|NAME]\n\
        specify the MAJOR device number or NAME (cannot be used with -N)\n\
    -m, --minor MINOR\n\
        specify the MINOR device number (used with -M or -N)\n\
    -l, --lastminor LASTMINOR\n\
        specify the LASTMINOR device number in a range (used with -s)\n\
    -q, --quiet\n\
        suppress normal output (equivalent to --debug=0)\n\
    -d, --debug [LEVEL]\n\
        increase or set debugging verbosity\n\
    -x, --verbose [LEVEL]\n\
        increase or set output verbosity\n\
    -h, --help, -?, --?\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
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
Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>\n\
Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software; you can  redistribute  it and/or modify  it under\n\
the terms  of the GNU General Public License  as  published by the Free Software\n\
Foundation; either  version  2  of  the  License, or (at  your option) any later\n\
version.\n\
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

#if 0
static int
sad_cmd(int fd, int cmd, struct strapush *sap)
{
	struct strioctl ioc = {
		ic_cmd:cmd,
		ic_timout:0,
		ic_len:sizeof(*sap),
		ic_dp:(char *) sap,
	};

	return ioctl(fd, I_STR, &ioc);
}
static int
sad_vml(int fd, struct str_list *sml)
{
	struct strioctl ioc = {
		ic_cmd:SAD_VML,
		ic_timout:0,
		ic_len:sizeof(*sml) + MAXAPUSH * sizeof(struct str_mlist),
		ic_dp:(char *) sml,
	};

	return ioctl(fd, I_STR, &ioc);
}
#endif

static struct sc_list *
sc_list(char **argv, int fd)
{
	int count;
	struct sc_list *list = NULL;

	if (ioctl(fd, I_PUSH, "sc") < 0) {
		if (debug)
			fprintf(stderr, "%s: could not push sc module\n", __FUNCTION__);
		if (output || debug)
			perror(argv[0]);
		return (list);
	}
	if (debug)
		fprintf(stderr, "%s: getting number of drivers and modules\n", __FUNCTION__);
	if ((count = ioctl(fd, SC_IOC_LIST, NULL)) < 0) {
		if (debug)
			fprintf(stderr, "%s: could not perform SC_IOC_LIST command\n", __FUNCTION__);
		if (output || debug)
			perror(argv[0]);
		return (list);
	}
	if (debug)
		fprintf(stderr, "%s: size of list is %d\n", __FUNCTION__, count);
	if (count == 0)
		return (list);
	if ((list = malloc(sizeof(struct sc_list) + count * sizeof(struct sc_mlist))) == NULL) {
		if (debug)
			fprintf(stderr, "%s: could not allocate memory\n", __FUNCTION__);
		fprintf(stderr, "%s: %s\n", argv[0], strerror(ENOMEM));
		return (list);
	}
	list->sc_nmods = count;
	list->sc_mlist = (struct sc_mlist *) (list + 1);
	if (ioctl(fd, SC_IOC_LIST, list) < 0) {
		if (debug)
			fprintf(stderr, "%s: could not perform second SC_IOC_LIST command\n", __FUNCTION__);
		perror(argv[0]);
		free(list);
		list = NULL;
		return (list);
	}
	ioctl(fd, I_POP, 0);
	return (list);
}

static int
autopush_set(char **argv, char *devname, int major, int minor, int lastminor, int nmods, char **modlist)
{
	struct strapush sap;
	int i, fd, cmd;

	if (debug) {
		fprintf(stderr, "%s: devname=%s, major=%d, minor=%d, lastminor=%d, nmods=%d, modlist=", __FUNCTION__, devname, major, minor, lastminor, nmods);
		for (i = 0; i < nmods; i++)
			fprintf(stderr, "\"%s\" ", modlist[i]);
		fprintf(stderr, "\n");
	}
	if (debug)
		fprintf(stderr, "%s: opening %s\n", __FUNCTION__, SAD_ADMIN_FILENAME);
	if ((fd = open(SAD_ADMIN_FILENAME, O_RDWR | O_NONBLOCK)) < 0) {
		if (output || debug)
			perror("autopush_set: " SAD_ADMIN_FILENAME);
		return (1);
	}
	if (minor == -1) {
		cmd = SAP_ALL;
		if (debug)
			fprintf(stderr, "%s: command is SAP_ALL\n", __FUNCTION__);
	} else if (lastminor == -1 || lastminor == minor) {
		cmd = SAP_ONE;
		if (debug)
			fprintf(stderr, "%s: command is SAP_ONE\n", __FUNCTION__);
	} else {
		cmd = SAP_RANGE;
		if (debug)
			fprintf(stderr, "%s: command is SAP_RANGE\n", __FUNCTION__);
	}
	memset(&sap, 0, sizeof(sap));
	sap.sap_cmd = cmd;
	sap.sap_major = major;
	sap.sap_minor = minor;
	sap.sap_lastminor = lastminor;
	sap.sap_npush = nmods;
	for (i = 0; i < nmods; i++) {
		if (debug)
			fprintf(stderr, "%s: copying module name %s\n", __FUNCTION__, modlist[i]);
		strncpy(sap.sap_list[i], modlist[i], FMNAMESZ);
		if (debug)
			fprintf(stderr, "%s: copied module name %s\n", __FUNCTION__, sap.sap_list[i]);
	}
	strncpy(sap.sap_module, devname, FMNAMESZ);
	if (debug)
		fprintf(stderr, "%s: performing SAD_SAP\n", __FUNCTION__);
	if (ioctl(fd, SAD_SAP, &sap) < 0) {
		if (output || debug)
			perror(argv[0]);
		return (1);
	}
	close(fd);
	return (0);
}

static int
autopush_cln(char **argv, char *devname, int major, int minor)
{
	if (debug)
		fprintf(stderr, "%s: devname=%s, major=%d, minor=%d\n", __FUNCTION__, devname, major, minor);
	return (1);
}

static int
autopush_ver(char **argv, int nmods, char **modlist)
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
		strncpy(sml.mlist[i].l_name, modlist[i], FMNAMESZ);
	if (ioctl(fd, SAD_VML, &sml.list) < 0) {
		if (output || debug)
			perror(argv[0]);
		return (1);
	}
	close(fd);
	return (0);
}

static int
autopush_get(char **argv, char *devname, int major, int minor)
{
	struct strapush sap;
	int header = 0;
	int fd;

	if (debug)
		fprintf(stderr, "%s: devname=%s, major=%d, minor=%d\n", __FUNCTION__, devname, major, minor);
	if (debug)
		fprintf(stderr, "%s: opening %s\n", __FUNCTION__, SAD_USER_FILENAME);
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
		strncpy(sap.sap_module, devname, FMNAMESZ);
		if (debug)
			fprintf(stderr, "%s: performing SAD_GAP\n", __FUNCTION__);
		if (ioctl(fd, SAD_GAP, &sap) < 0) {
			if (output || debug)
				perror(argv[0]);
			return (1);
		}
		if (output || debug) {
			int j;

			printf("DeviceName Major Minor Lastminor Modules\n");
			printf("%10s %5ld %5ld     ", sap.sap_module, sap.sap_major, sap.sap_minor);
			if (sap.sap_minor == sap.sap_lastminor)
				printf("%5s ", "-");
			else
				printf("%5ld ", sap.sap_lastminor);
			for (j = 0; j < sap.sap_npush; j++)
				printf("%s ", sap.sap_list[j]);
			printf("\n");
		}
		close(fd);
		return (0);
	} else {
		struct sc_list *list;
		struct sc_mlist *mlist;
		int i;

		if ((list = sc_list(argv, fd)) == NULL) {
			close(fd);
			return (1);
		}
		mlist = (typeof(mlist)) (list + 1);
		for (i = 0; i < list->sc_nmods; i++, mlist++) {
			/* modules or end of list */
			if (mlist->major == -1 || mlist->major == 0)
				break;
			memset(&sap, 0, sizeof(sap));
			sap.sap_cmd = 0;
			sap.sap_major = mlist->major;
			sap.sap_minor = minor;
			sap.sap_lastminor = 0;
			sap.sap_npush = MAXAPUSH;
			strncpy(sap.sap_module, mlist->name, FMNAMESZ);
			if (debug)
				fprintf(stderr, "%s: performing SAD_GAP major = %d, minor = %d, name = %s\n", __FUNCTION__, (int) sap.sap_major, (int) sap.sap_minor, sap.sap_module);
			errno = 0;
			if (ioctl(fd, SAD_GAP, &sap) < 0) {
				if (debug)
					fprintf(stderr, "%s: got error\n", __FUNCTION__);
				switch (errno) {
				case ENOSTR:	/* skip that device */
					if (debug)
						perror(__FUNCTION__);
					continue;
				case ENODEV:	/* skip that device */
					if (debug)
						perror(__FUNCTION__);
					continue;
#if 0
				case EINVAL:	/* last device */
					if (header)
						break;
#endif
				default:
					if (output || debug)
						perror(argv[0]);
					return (1);
				}
			}
			if (!header) {
				if (output || debug)
					printf("DeviceName Major Minor Lastminor Modules\n");
				header = 1;
			}
			if (output || debug) {
				int j;

				printf("%10s %5ld %5ld     ", sap.sap_module, sap.sap_major, sap.sap_minor);
				if (sap.sap_minor == sap.sap_lastminor)
					printf("%5s ", "-");
				else
					printf("%5ld ", sap.sap_lastminor);
				for (j = 0; j < sap.sap_npush; j++)
					printf("%s ", sap.sap_list[j]);
				printf("\n");
			}
		}
	}
	close(fd);
	return (0);
}

static int
autopush_res(char **argv, char *devname, int major, int minor)
{
	struct strapush sap;
	int header = 0;
	int fd;

	if (debug)
		fprintf(stderr, "%s: devname=%s, major=%d, minor=%d\n", __FUNCTION__, devname, major, minor);
	if (debug)
		fprintf(stderr, "%s: opening %s\n", __FUNCTION__, SAD_ADMIN_FILENAME);
	if ((fd = open(SAD_ADMIN_FILENAME, O_RDWR | O_NONBLOCK)) < 0) {
		if (output || debug)
			perror("autopush_res: " SAD_ADMIN_FILENAME);
		return (1);
	}
	if (minor == -1)
		minor = 0;
	if (major != -1 || *devname != '\0') {
		memset(&sap, 0, sizeof(sap));
		sap.sap_cmd = SAP_CLEAR;
		sap.sap_major = major;
		sap.sap_minor = minor;
		sap.sap_lastminor = 0;
		sap.sap_npush = MAXAPUSH;
		strncpy(sap.sap_module, devname, FMNAMESZ);
		if (debug)
			fprintf(stderr, "%s: performing SAD_SAP\n", __FUNCTION__);
		if (ioctl(fd, SAD_SAP, &sap) < 0) {
			if (output || debug)
				perror(argv[0]);
			return (1);
		}
		if (output || debug) {
			int j;

			printf("DeviceName Major Minor Lastminor Modules\n");
			printf("%10s %5ld %5ld     ", sap.sap_module, sap.sap_major, sap.sap_minor);
			if (sap.sap_minor == sap.sap_lastminor)
				printf("%5s ", "-");
			else
				printf("%5ld ", sap.sap_lastminor);
			for (j = 0; j < sap.sap_npush; j++)
				printf("%s ", sap.sap_list[j]);
			printf("\n");
		}
		close(fd);
		return (0);
	} else {
		struct sc_list *list;
		struct sc_mlist *mlist;
		int i;

		if ((list = sc_list(argv, fd)) == NULL) {
			close(fd);
			return (1);
		}
		mlist = (typeof(mlist)) (list + 1);
		for (i = 0; i < list->sc_nmods; i++, mlist++) {
			/* modules or end of list */
			if (mlist->major == -1 || mlist->major == 0)
				break;
			memset(&sap, 0, sizeof(sap));
			sap.sap_cmd = SAP_CLEAR;
			sap.sap_major = mlist->major;
			sap.sap_minor = 0;
			sap.sap_lastminor = 0;
			sap.sap_npush = MAXAPUSH;
			strncpy(sap.sap_module, mlist->name, FMNAMESZ);
			if (debug)
				fprintf(stderr, "%s: performing SAD_SAP major = %d, name = %s\n", __FUNCTION__, (int) major, sap.sap_module);
			if (ioctl(fd, SAD_SAP, &sap) < 0) {
				switch (errno) {
				case ENOSTR:	/* skip that device */
				case ENODEV:	/* skip that device */
					continue;
#if 0
				case EINVAL:	/* last device */
					if (header)
						break;
#endif
				default:
					if (output || debug)
						perror(argv[0]);
					return (1);
				}
			}
			if (!header) {
				if (output || debug)
					printf("DeviceName Major Minor Lastminor Modules\n");
				header = 1;
			}
			if (output || debug) {
				int j;

				printf("%10s %5ld %5ld     ", sap.sap_module, sap.sap_major, sap.sap_minor);
				if (sap.sap_minor == sap.sap_lastminor)
					printf("%5s ", "-");
				else
					printf("%5ld ", sap.sap_lastminor);
				for (j = 0; j < sap.sap_npush; j++)
					printf("%s ", sap.sap_list[j]);
				printf("\n");
			}
		}
	}
	close(fd);
	return (0);
}

static int
autopush_fil(char **argv, char *filename)
{
	char buffer[1024];
	char *line;
	int lineno, fd;
	FILE *file;

	if ((file = fopen(filename, "r")) == NULL) {
		if (output || debug)
			perror(argv[0]);
		return (1);
	}
	/* read the file one line at a time */
	for (lineno = 1; (line = fgets(buffer, sizeof(buffer), file)) != NULL; lineno++) {
		struct strapush sap;
		char *str, *token, *tmp = NULL;
		int tokenind;
		int major = -1, minor = -1, lastminor = -1;
		char devname[FMNAMESZ + 1] = "";

		for (str = line, tokenind = 0, sap.sap_npush = 0; tokenind < MAXAPUSH + 3 && (token = strtok_r(str, " \t\f\n\r\v", &tmp)) != NULL; str = NULL, tokenind++) {
			switch (tokenind) {
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
				if (lastminor != -1 && (lastminor < 0 || lastminor > MAX_MINOR_DEVICE || lastminor < minor))
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
		strncpy(sap.sap_module, devname, FMNAMESZ);
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
				perror(argv[0]);
			return (1);
		}
		if (ioctl(fd, SAD_SAP, &sap) < 0) {
			if (output || debug)
				perror(argv[0]);
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
	return (0);
}

int
main(int argc, char **argv)
{
	int c, val;
	char filename[256] = "";
	char devname[FMNAMESZ + 1] = "";
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
#ifdef _GNU_SOURCE
		int option_index = 0;
		static struct option long_options[] = {
			/* *INDENT-OFF* */
			{ "file",	required_argument,	NULL, 'f' },
			{ "set",	no_argument,		NULL, 's' },
			{ "get",	no_argument,		NULL, 'g' },
			{ "reset",	no_argument,		NULL, 'r' },
			{ "clone",	no_argument,		NULL, 'c' },
			{ "verify",	no_argument,		NULL, 'v' },
			{ "name",	required_argument,	NULL, 'N' },
			{ "major",	required_argument,	NULL, 'M' },
			{ "minor",	required_argument,	NULL, 'm' },
			{ "lastminor",	required_argument,	NULL, 'l' },
			{ "quiet",	no_argument,		NULL, 'q' },
			{ "debug",	optional_argument,	NULL, 'd' },
			{ "verbose",	optional_argument,	NULL, 'x' },
			{ "version",	no_argument,		NULL, 'V' },
			{ "copying",	no_argument,		NULL, 'C' },
			{ "help",	no_argument,		NULL, 'h' },
			{ "?",		no_argument,		NULL, 'h' },
			{ 0, }
			/* *INDENT-ON* */
		};

		c = getopt_long_only(argc, argv, "f:sgrcM:m:N:l:vqd::x::VCh?", long_options, &option_index);
#else				/* _GNU_SOURCE */
		c = getopt(argc, argv, "f:sgrcM:m:N:l:vqd::x::VCh?");
#endif				/* _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
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
			if (option == OPTION_NONE || option == OPTION_FILE || option == OPTION_VERIFY || *devname != 0)
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
			if (option == OPTION_NONE || option == OPTION_FILE || option == OPTION_VERIFY || minor != -1)
				goto bad_option;
			minor = atoi(optarg);
			if (minor < 0 || minor > MAX_MINOR_DEVICE)
				goto bad_option;
			break;
		case 'N':	/* -N, --name name */
			if (option == OPTION_NONE || option == OPTION_FILE || option == OPTION_VERIFY || major != -1)
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
		case 'q':	/* -q, --quiet */
			output = 0;
			debug = 0;
			break;
			break;
		case 'd':	/* -d, --debug [LEVEL] */
			if (debug)
				fprintf(stderr, "%s: increasing debug verbosity\n", argv[0]);
			if (optarg == NULL) {
				debug++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			debug = val;
			break;
		case 'x':	/* -x, --verbose [LEVEL] */
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
		case 'h':	/* -h, --help, -? --? */
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
		      bad_nonopt:
			if (output > 0 || debug > 0) {
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
				usage(argc, argv);
			}
			exit(2);
		}
	}
	switch (option) {
	case OPTION_FILE:
		if (optind < argc) {
			if (debug)
				fprintf(stderr, "%s: OPTION_FILE: too many non-option arguments\n", argv[0]);
			goto bad_nonopt;
		}
		exit(autopush_fil(argv, filename));
	case OPTION_SET:
		if (optind >= argc)
			goto option_reset;
		if (argc - optind > MAXAPUSH) {
			if (debug)
				fprintf(stderr, "%s: OPTION_SET: too many ( %d > %d ) module names\n", argv[0], argc - optind, MAXAPUSH);
			optind += MAXAPUSH;
			goto bad_nonopt;
		}
		exit(autopush_set(argv, devname, major, minor, lastminor, argc - optind, &argv[optind]));
	case OPTION_GET:
		if (optind < argc) {
			if (debug)
				fprintf(stderr, "%s: OPTION_GET: too many non-option arguments\n", argv[0]);
			goto bad_nonopt;
		}
		exit(autopush_get(argv, devname, major, minor));
	case OPTION_RESET:
	      option_reset:
		if (optind < argc) {
			if (debug)
				fprintf(stderr, "%s: OPTION_RESET: too many non-option arguments\n", argv[0]);
			goto bad_nonopt;
		}
#if 0
		if ((*devname == 0 && major == -1) || minor == -1) {
			if (debug)
				fprintf(stderr, "%s: OPTION_RESET: one of -N or -M and -m must be set\n", argv[0]);
			goto bad_nonopt;
		}
#endif
		exit(autopush_res(argv, devname, major, minor));
	case OPTION_VERIFY:
		if (optind >= argc)
			exit(0);	/* no modules to verify */
		if (argc - optind > MAXAPUSH) {
			if (debug)
				fprintf(stderr, "%s: OPTION_VERIFY: too many ( %d > %d ) module names\n", argv[0], argc - optind, MAXAPUSH);
			optind += MAXAPUSH;
			goto bad_nonopt;
		}
		exit(autopush_ver(argv, argc - optind, &argv[optind]));
	case OPTION_CLONE:
		if (optind < argc) {
			if (debug)
				fprintf(stderr, "%s: OPTION_CLONE: too many non-option arguments\n", argv[0]);
			goto bad_nonopt;
		}
		if ((*devname == 0 && major == -1) || minor == -1) {
			if (debug)
				fprintf(stderr, "%s: OPTION_CLONE: one of -N or -M and -m must be set\n", argv[0]);
			goto bad_nonopt;
		}
		exit(autopush_cln(argv, devname, major, minor));
	default:
	case OPTION_NONE:
		usage(argc, argv);
		exit(2);
	}
}
