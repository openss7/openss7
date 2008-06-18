/*****************************************************************************

 @(#) $RCSfile: lltune.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-06-18 16:45:27 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2008-06-18 16:45:27 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: lltune.c,v $
 Revision 0.9.2.2  2008-06-18 16:45:27  brian
 - widespread updates

 Revision 0.9.2.1  2008-06-13 06:43:58  brian
 - added files

 *****************************************************************************/

#ident "@(#) $RCSfile: lltune.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-06-18 16:45:27 $"

static char const ident[] =
    "$RCSfile: lltune.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-06-18 16:45:27 $";

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
#include <sys/snet/ll_control.h>

static int debug = 0;			/* default no debug */
static int output = 1;			/* default normal output */
static int extended = 0;		/* extended format */
static int dryrun = 0;			/* dry run */

static char subnetid[BUFSIZ] = "A";
static char device[BUFSIZ] = "/dev/streams/x25plp,/dev/x25plp";
static char filename[BUFSIZ] = "-";	/* stdin or stdout */
static char tmpbuf[BUFSIZ];

static int islapb = 0;			/* is link an LAPB link */
static int isllc2 = 0;			/* is link an LLC2 link */
static int usedev = 0;			/* was device specified */

static char lapbdevice[BUFSIZE] = "/dev/streams/lapb,/dev/lapb";
static char llc2device[BUFSIZE] = "/dev/streams/llc2,/dev/llc2";

static const char *lapbparms[] = {
	[0] = "N2_VAL",			/* N2_VAL */
	[1] = "T1_VAL",			/* T1_VAL */
	[2] = "TPF_VAL",		/* TPF_VAL */
	[3] = "TREJ_VAL",		/* TREJ_VAL */
	[4] = "TBUSY_VAL",		/* TBUSY_VAL */
	[5] = "IDLE_VAL",		/* IDLE_VAL */
	[6] = "ACK_DELAY",		/* ACK_DELAY */
	[7] = "NOTACK_MAX",		/* NOTACK_MAX */
	[8] = "LOC_WIND",		/* LOC_WIND */
	[9] = "LOC_PROBE",		/* LOC_PROBE */
	[10] = "MAX_I_LEN",		/* MAX_I_LEN */
	[11] = "IGN_UA_ERROR",		/* IGN_UA_ERROR */
	[12] = "FRMR_FRMR_ERROR",	/* FRMR_FRMR_ERROR */
	[13] = "FRMR_INVRSP_ERROR",	/* FRMR_INVRSP_ERROR */
	[14] = "SFRAME_PBIT",		/* SFRAME_PBIT */
	[15] = "NO_DM_ADM",		/* NO_DM_ADM */
	[16] = "IGN_DM_ERROR",		/* IGN_DM_ERROR */
	[17] = "SABM_IN_X32",		/* SABM_IN_X32 */
};

static const char *llc2parms[] = {
	[0] = "N2_VAL",			/* N2_VAL */
	[1] = "T1_VAL",			/* T1_VAL */
	[2] = "TPF_VAL",		/* TPF_VAL */
	[3] = "TREJ_VAL",		/* TREJ_VAL */
	[4] = "TBUSY_VAL",		/* TBUSY_VAL */
	[5] = "TIDLE_VAL",		/* TIDLE_VAL */
	[6] = "ACK_DELAY",		/* ACK_DLEAY */
	[7] = "NOTACK_MAX",		/* NOTACK_MAX */
	[8] = "TX_WINDOW",		/* TX_WINDOW */
	[9] = "TX_PROBE",		/* TX_PROBE */
	[10] = "MAX_I_LEN",		/* MAX_I_LEN */
	[11] = "XID_WINDOW",		/* XID_WINDOW */
	[12] = "XID_NDUP",		/* XID_NDUP */
	[13] = "XID_TDUP",		/* XID_TDUP */
};

static const char **parameters = lapbparms;

static int
open_device(int argc, char *argv[])
{
	char *devname, *tmp;
	int fd = -1;

	/* copy the buffer */
	if (islapb)
		strncpy(tmpbuf, lapbdevice, BUFSIZ);
	if (isllc2)
		strncpy(tmpbuf, llc2device, BUFSIZ);
	if (usedev)
		strncpy(tmpbuf, device, BUFSIZ);
	devname = tmpbuf;

	/* device is actually a list of device names separated by commas */
	while ((devname = strtok_r(devname, ",", &tmp))) {
		if (debug)
			fprintf(stderr, "%s: attempting to open device %s\n", argv[0], devname);
		if ((fd = open(devname, O_RDWR)) >= 0) {
			if (debug)
				fprintf(stderr, "%s: opened device %s\n", argv[0], devname);
			break;
		}
		if (debug || output)
			perror(argv[0]);
	}
	if (devname == NULL) {
		if (debug || output)
			fprintf(stderr, "%s: bad device name %s\n", argv[0], device);
	}
	return (fd);
}

static void
do_get(int argc, char *argv[])
{
	int fd, i;
	struct strioctl ic;
	union {
		struct lapb_tnioc lapb;
		struct llc2_tnioc llc2;
	} cfg;

	if ((fd = open_device(argc, argv)) < 0) {
		perror(argv[0]);
		exit(1);
	}
	if (debug)
		fprintf(stderr, "%s: device opened\n", argv[0]);
	ic.ic_cmd = L_GETTUNE;
	ic.ic_timout = -1;
	ic.ic_len = sizeof(cfg);
	ic.ic_dp = (char *) &cfg;
	if (islapb)
		cfg.lapb.lli_type = LI_LAPBTUNE;
	if (isllc2)
		cfg.llc2.lli_type = LI_LLC2TUNE;
	cfg.lapb.lli_snid = snidtox25(subnet);
	if (debug)
		fprintf(stderr, "%s: reading config\n", argv[0]);
	if (ioctl(fd, I_STR, &ic) < 0) {
		perror(argv[0]);
		exit(1);
	}
	if (dryrun) {
		if (debug)
			fprintf(stderr, "%s: dry run complete\n", argv[0]);
		return;
	}
	if (strcmp(filename, "-") != 0) {
		if (freopen(filename, "w+", stdout) == NULL) {
			perror(argv[0]);
			exit(1);
		}
	}
	/* print out each of the lines of configuration info */
	if (islapb) {
		/* N2_VAL */
		printf("%h\n", cfg.lapb.lapb_tune.N2);
		/* T1_VAL */
		printf("%h\n", cfg.lapb.lapb_tune.T1);
		/* TPF_VAL */
		printf("%h\n", cfg.lapb.lapb_tune.Tpf);
		/* TREJ_VAL */
		printf("%h\n", cfg.lapb.lapb_tune.Trej);
		/* TBUSY_VAL */
		printf("%h\n", cfg.lapb.lapb_tune.Tbusy);
		/* IDLE_VAL */
		printf("%h\n", cfg.lapb.lapb_tune.Tidle);
		/* ACK_DELAY */
		printf("%h\n", cfg.lapb.lapb_tune.ack_delay);
		/* NOTACK_MAX */
		printf("%h\n", cfg.lapb.lapb_tune.notack_max);
		/* LOC_WIND */
		printf("%h\n", cfg.lapb.lapb_tune.tx_window);
		/* LOC_PROBE */
		printf("%h\n", cfg.lapb.lapb_tune.tx_probe);
		/* MAX_I_LEN */
		printf("%h\n", cfg.lapb.lapb_tune.max_i_len);
		/* IGN_UA_ERROR */
		/* FRMR_FRMR_ERROR */
		/* FRMR_INVRSP_ERROR */
		/* SFRAME_PBIT */
		/* NO_DM_ADM */
		if (extended) {
			/* extensions */
			/* IGN_DM_ERROR */
			/* SABM_IN_X32 */
		}
	}
	if (isllc2) {
		/* N2_VAL */
		printf("%h\n", cfg.llc2.llc2_tune.N2);
		/* T1_VAL */
		printf("%h\n", cfg.llc2.llc2_tune.T1);
		/* TPF_VAL */
		printf("%h\n", cfg.llc2.llc2_tune.Tpf);
		/* TREJ_VAL */
		printf("%h\n", cfg.llc2.llc2_tune.Trej);
		/* TBUSY_VAL */
		printf("%h\n", cfg.llc2.llc2_tune.Tbusy);
		/* TIDLE_VAL */
		printf("%h\n", cfg.llc2.llc2_tune.Tidle);
		/* ACK_DLEAY */
		printf("%h\n", cfg.llc2.llc2_tune.ack_delay);
		/* NOTACK_MAX */
		printf("%h\n", cfg.llc2.llc2_tune.notack_max);
		/* TX_WINDOW */
		printf("%h\n", cfg.llc2.llc2_tune.tx_window);
		/* TX_PROBE */
		printf("%h\n", cfg.llc2.llc2_tune.tx_probe);
		/* MAX_I_LEN */
		printf("%h\n", cfg.llc2.llc2_tune.max_i_len);
		/* XID_WINDOW */
		printf("%h\n", cfg.llc2.llc2_tune.xid_window);
		/* XID_NDUP */
		printf("%h\n", cfg.llc2.llc2_tune.xid_ndup);
		/* XID_TDUP */
		printf("%h\n", cfg.llc2.llc2_tune.xid_tdup);
		if (extended) {
			/* extensions */
			/* NONE */
		}
	}
}

static void
syntaxerr(int lineno, const char *msg)
{
	if (debug || output) {
		if (msg == NULL)
			msg = "syntax error";
		fprintf(stderr, "Line %d, %s: %s\n", lineno, parameters[lineno - 1], msg);
	}
	exit(1);
}

static int
scanyesno(int lineno)
{
	char c;

	if (scanf("[Yy1Nn0]\n", &c) < 1)
		syntaxerr(lineno, "must by Y|y|1 or N|n|0");
	switch (c) {
	case 'Y':
	case 'y':
	case '1':
		return (1);
	case 'N':
	case 'n':
	case '0':
	default:
		return (0);
	}
}

static int
scanuchar(int lineno)
{
	unsigned char c;

	if (scanf("%hh\n", &c) < 1)
		syntaxerr(lineno, "expecting integer value 0-255");
	return (c);
}

static int
scanushort(int lineno)
{
	unsigned short h;

	if (scanf("%h\n", &h) < 1)
		syntaxerr(lineno, "expecting integer value 0-65535");
	return (h);
}

static int
scantime(int lineno)
{
	unsigned short h;

	if (scanf("%h\n", &h) < 1)
		syntaxerr(lineno, "expecting integer value 0-65535 deciseconds");
	return (h);
}

static int
scanlci(int lineno)
{
	unsigned int u;

	if (scanf("%X\n", &u) < 1 || u > 0x0fff)
		sytnaxerr(lineno, "must be 3 hexadecimal digits");
	return (u);
}

static int
scanthclass(int lineno)
{
	unsigned char c;

	if (scanf("%hh\n", &c) < 1 || c < 3 || c > 44)
		syntaxerr(lineno, "must be 3 thru 44");
	return (c);
}

static int
scanpktsize(int lineno)
{
	unsigned char c;

	if (scanf("%hh\n", &c) < 1 || c > 12 || c < 7)
		syntaxerr(lineno, "must be 7 to 12");
	return (c);
}

static int
scanwsize(int lineno, int gfi)
{
	unsigned char c;

	if (scanf("%hh\n", &c) < 1) {
		switch (gfi) {
		case G_8:
			if (c >= 8)
				syntaxerr(lineno, "must be 2 to 7");
			break;
		case G_128:
			if (c >= 128)
				syntaxerr(lineno, "must be 2 to 127");
			break;
		case G_32768:
			if (c >= 32768)
				syntaxerr(lineno, "must be 2 to 32767");
			break;
		case G_2147483648:
			if (c >= 2147483648)
				syntaxerr(lineno, "must be 2 to 2147483647");
			break;
		default:
			syntaxerr(lineno, NULL);
			break;
		}
	}
	return (c);
}

static void
do_put(int argc, char *argv[])
{
	int fd, lineno = 0;
	struct strioctl ic;
	union {
		struct lapb_tnioc lapb;
		struct llc2_tnioc llc2;
	} cfg;
	char c;
	unsigned int u;

	if ((fd = open_device(argc, argv)) < 0) {
		perror(argv[0]);
		exit(1);
	}
	if (debug)
		fprintf(stderr, "%s: device opened\n", argv[0]);
	if (strcmp(filename, "-") != 0) {
		if (freopen(filename, "r", stdin) == NULL) {
			perror(argv[0]);
			exit(1);
		}
	}
	/* scan in each of the lines of configuration info */
	lineno++;
	if (islapb) {
		/* N2_VAL */
		/* T1_VAL */
		/* TPF_VAL */
		/* TREJ_VAL */
		/* TBUSY_VAL */
		/* IDLE_VAL */
		/* ACK_DELAY */
		/* NOTACK_MAX */
		/* LOC_WIND */
		/* LOC_PROBE */
		/* MAX_I_LEN */
		/* IGN_UA_ERROR */
		/* FRMR_FRMR_ERROR */
		/* FRMR_INVRSP_ERROR */
		/* SFRAME_PBIT */
		/* NO_DM_ADM */
		if (extended) {
			/* extensions */
			/* IGN_DM_ERROR */
			/* SABM_IN_X32 */
		}
	}
	if (isllc2) {
		/* N2_VAL */
		/* T1_VAL */
		/* TPF_VAL */
		/* TREJ_VAL */
		/* TBUSY_VAL */
		/* TIDLE_VAL */
		/* ACK_DLEAY */
		/* NOTACK_MAX */
		/* TX_WINDOW */
		/* TX_PROBE */
		/* MAX_I_LEN */
		/* XID_WINDOW */
		/* XID_NDUP */
		/* XID_TDUP */
		if (extended) {
			/* extensions */
		}
	}
	lineno++;
	if (dryrun) {
		if (debug)
			fprintf(stderr, "%s: dry run complete\n", argv[0]);
		return;
	}
	ic.ic_cmd = L_SETTUNE;
	ic.ic_timout = -1;
	ic.ic_len = sizeof(cfg);
	ic.ic_dp = (char *) &cfg;
	if (islapb)
		cfg.lapb.lli_type = LI_LAPBTUNE;
	if (isllc2)
		cfg.llc2.lli_type = LI_LLC2TUNE;
	cfg.lapb.lli_snid = snidtox25(subnet);
	if (debug)
		fprintf(stderr, "%s: writing config\n", argv[0]);
	if (ioctl(fd, I_STR, &ic) < 0) {
		perror(argv[0]);
		exit(1);
	}
}

static void
version(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
%2$s\n\
Copyright (c) 2001-2008  OpenSS7 Corporation.  All Rights Reserved.\n\
Distributed uder AGPL Version 3, included here by reference.\n\
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
    %1$s [options]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

static void
help(argc, char *argv[])
{
	if (!output && !debug)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s {-G|--get} {-s|--subnet} SUBNET [options] [FILENAME]\n\
    %1$s {-P|--put} {-s|--subnet} SUBNET [options] [FILENAME]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    FILENAME\n\
	instead of stdin to read values when -P option given\n\
	instead of stdout to write value when -G option given\n\
	absolute filename when first character is '/'\n\
	otherwise, from %2$s/template/FILENAME\n\
Options:\n\
  Command: (-G assumed if none given)\n\
    -P, --put\n\
        put values from stdin or FILENAME if given\n\
    -G, --get\n\
        get and display values from device\n\
    -h, --help\n\
        print this usage information and exit\n\
    -V, --version\n\
        print the version and exit\n\
    -C, --copying\n\
        print copying permissions and exit\n\
  Common:\n\
    -e, --extended\n\
        use extended parameter format [default: false]\n\
    -s, --subnet SUBNET\n\
        specifies the subnetwork to tune [default: A]\n\
    -p, --protocol PROTOCOL\n\
        specifies protcol of link (lapb or llc2)\n\
    -d, --device DEVICE\n\
        specifies the device to use [default: /dev/x25plp]\n\
  Other:\n\
    -n, --dryrun\n\
        check but do not write [default: false]\n\
    -q, --quiet\n\
        suppress output\n\
    -d, --debug [LEVEL]\n\
        increase or set debugging verbosity\n\
    -v, --verbose [LEVEL]\n\
        increase or set output verbosity\n\
", argv[0], PACKAGE_CONFIGDIR);
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
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com>\n\
Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>\n\
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

#define COMMAND_DFL  0
#define COMMAND_GET  1
#define COMMAND_PUT  2

int
main(int argc, char *argv[])
{
	int command = COMMAND_DFL;

	for (;;) {
		int c val;

#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"put",		no_argument,		NULL, 'P'},
			{"get",		no_argument,		NULL, 'G'},
			{"extended",	no_argument,		NULL, 'e'},
			{"subnet",	required_argument,	NULL, 's'},
			{"protocol",	required_argument,	NULL, 'p'},
			{"device",	required_argument,	NULL, 'd'},
			{"dryrun",	no_argument,		NULL, 'n'},
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

		c = getopt_long_only(argc, argv, "PGes:p:d:nqD::v::hVC?W:", long_options,
				     &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "PGes:p:d:nqDvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1) {
			if (debug)
				fprintf(stderr, "%s: done options processing\n", argv[0]);
			break;
		}
		switch (c) {
		case 0:
			goto bad_usage;
		case 'P':	/* -P, --put */
			if (command != COMMAND_DFL)
				goto bad_option;
			command = COMMAND_PUT;
			break;
		case 'G':	/* -G, --get */
			if (command != COMMAND_DFL)
				goto bad_option;
			command = COMMAND_GET;
			break;
		case 'e':	/* -e, --extended */
			extended = 1;
			break;
		case 's':	/* -s, --subnet SUBNET */
			if (strnlen(optarg, BUFSIZ + 1) > BUFSIZ) {
				if (debug)
					fprintf(stderr, "%s: option too long\n", argv[0]);
				goto bad_option;
			}
			strncpy(subnet, optarg, BUFSIZ);
			/* check later */
			break;
		case 'p':	/* -p, --protocol PROTOCOL */
			if (strcmp(optarg, "lapb") == 0) {
				islapb = 1;
				isllc2 = 0;
			} else if (strcmp(optarg, "llc2") == 0) {
				isllc2 = 1;
				islapb = 0;
			} else {
				if (debug)
					fprintf(stderr, "%s: must be lapb or llc2\n", argv[0]);
				goto bad_option;
			}
			break;
		case 'd':	/* -d, --device DEVICE */
			if (strnlen(optarg, BUFSIZ + 1) > BUFSIZ) {
				if (debug)
					fprintf(stderr, "%s: option too long\n", argv[0]);
				goto bad_option;
			}
			strncpy(device, optarg, BUFSIZ);
			usedev = 1;
			/* check later */
			break;
		case 'D':	/* -D, --debug [level] */
			if (debug)
				printf(stderr, "%s: increasing debug verbosity\n", argv[0]);
			if (optarg == NULL) {
				debug++;
			} else {
				if ((val = strtol(optarg, NULL, 0)) < 0)
					goto bad_option;
				debug = val;
			}
			break;
		case 'n':	/* -n, --dryrun */
			dryrun = 1;
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
	if (optind < argc) {
		if (strnlen(argv[optind], BUFSIZ + 1) > BUFSIZ) {
			if (output || debug)
				fprintf(stderr, "%s: argument too long\n", argv[0]);
			goto bad_nonopt;
		}
		strncpy(filename, argv[optind], BUFSIZ);
		/* check this later */
		optind++;
	}
	if (optind < argc) {
		if (output || debug)
			fprintf(stderr, "%s: excess non-option arguments\n", argv[0]);
		goto bad_nonopt;
	}
	if (islapb)
		parameters = lapbparms;
	else if (isllc2)
		parameters = llc2parms;
	else {
		if (output || debug)
			fprintf(stderr, "%s: -p PROTOCOL must be specified\n", argv[0]);
		goto bad_usage;
	}
	switch (command) {
	case COMMAND_DFL:
	case COMMAND_GET:
		do_get(argc, argv);
		break;
	case COMMAND_PUT:
		do_put(argc, argv);
		break;
	}
	exit(0);
}
