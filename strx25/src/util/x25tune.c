/*****************************************************************************

 @(#) $RCSfile: x25tune.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-06-18 16:45:27 $

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

 $Log: x25tune.c,v $
 Revision 0.9.2.2  2008-06-18 16:45:27  brian
 - widespread updates

 Revision 0.9.2.1  2008-06-13 06:43:58  brian
 - added files

 *****************************************************************************/

#ident "@(#) $RCSfile: x25tune.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-06-18 16:45:27 $"

static char const ident[] =
    "$RCSfile: x25tune.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-06-18 16:45:27 $";

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
#include <sys/snet/x25_control.h>

static int debug = 0;			/* default no debug */
static int output = 1;			/* default normal output */
static int extended = 0;		/* extended format */
static int dryrun = 0;			/* dry run */

static char subnetid[BUFSIZ] = "A";
static char address[BUFSIZ] = "";
static char device[BUFSIZ] = "/dev/streams/x25plp,/dev/x25plp";
static char filename[BUFSIZ] = "-";	/* stdin or stdout */
static char tmpbuf[BUFSIZ];

static int usedev = 0;			/* was device specified */

static const char *networks[] = {
	[1] = "X25_LLC",		/* ISO/IEC 8881 */
	[2] = "X25_88",			/* ISO/IEC 8208 (1988) */
	[3] = "X25_84",			/* ISO/IEC 8208 (1984) */
	[4] = "X25_80",			/* ISO/IEC 8208 (1980) */
	[5] = "PSS",			/* UK */
	[6] = "AUSTPAC",		/* Australia */
	[7] = "DATAPAC",		/* Canada */
	[8] = "DDN",			/* DoD */
	[9] = "TELENET",		/* USA */
	[10] = "TRANSPAC",		/* France */
	[11] = "TYMNET",		/* USA */
	[12] = "DATEX_P",		/* West Germany */
	[13] = "DDX_P",			/* Japan */
	[14] = "VENUS_P",		/* Japan */
	[15] = "ACCUNET",		/* USA */
	[16] = "ITAPAC",		/* Italy */
	[17] = "DATAPAK",		/* Sweden */
	[18] = "DATANET",		/* Holland */
	[19] = "DCS",			/* Belgium */
	[20] = "TELEPAC",		/* Switzerland */
	[21] = "F_DATAPAC",		/* Finland */
	[22] = "FINPAC",		/* Finland */
	[23] = "PACNET",		/* New Zealand */
	[24] = "LUXPAC",		/* Luxembourgh */
	[25] = "X25_CIRCUIT",		/* circuit switched */
};

static const char *parameters[] = {
	[0] = "NET_MODE",
	[1] = "X25_VERSION",
	[2] = "L3PLPMODE",
	[3] = "LPC",
	[4] = "HPC",
	[5] = "LIC",
	[6] = "HIC",
	[7] = "LTC",
	[8] = "HTC",
	[9] = "LOC",
	[10] = "HOC",
	[11] = "THISGFI",
	[12] = "LOCMAXPKTSIZE",
	[13] = "REMMAXPKTSIZE",
	[14] = "LOCDEFPKTSIZE",
	[15] = "REMDEFPKTSIZE",
	[16] = "LOCMAXWSIZE",
	[17] = "REMMAXWSIZE",
	[18] = "LOCDEFWSIZE",
	[19] = "REMDEFWSIZE",
	[20] = "MAXNSDULEN",
	[21] = "ACKDELAY",
	[22] = "T20VALUE",
	[23] = "T21VALUE",
	[24] = "T22VALUE",
	[25] = "T23VALUE",
	[26] = "TVALUE",
	[27] = "T25VALUE",
	[28] = "T26VALUE",
	[29] = "IDLEVALUE",
	[30] = "CONNECTVALUE",
	[31] = "R20VALUE",
	[32] = "R22VALUE",
	[33] = "R23VALUE",
	[34] = "LOCALDELAY",
	[35] = "ACCESSDELAY",
	[36] = "LOCMAXTHCLASS",
	[37] = "REMMAXTHCLASS",
	[38] = "LOCDEFTHCLASS",
	[39] = "REMDEFTHCLASS",
	[40] = "LOCMINTHCLASS",
	[41] = "REMMINTHCLASS",
	[42] = "SUB_CUG",
	[43] = "SUB_PREF",
	[44] = "SUB_CUGOA",
	[45] = "SUB_CUGIA",
	[46] = "CUG_FORMAT",
	[47] = "BAR_CUG_IN",
	[48] = "SUB_EXTENDED",
	[49] = "BAR_EXTENDED",
	[50] = "SUB_FSELECT",
	[51] = "SUB_FSRRESP",
	[52] = "SUB_REVCHARGE",
	[53] = "SUB_LOC_CHG_PREV",
	[54] = "BAR_INCALL",
	[55] = "BAR_OUTCALL",
	[56] = "SUB_TOA_NPI_FMT",
	[57] = "BAR_TOA_NPI_FMT",
	[58] = "SUB_NUI_OVERRIDE",
	[59] = "ACC_NODIAG",
	[60] = "USE_DIAG",
	[61] = "CCITT_CLEAR_LEN",
	[62] = "BAR_DIAG",
	[63] = "DISC_NS_DIAG",
	[64] = "ACC_HEX_ADD",
	[65] = "BAR_NONPRIV_LISTEN",
	[66] = "INTL_ADDR_RECOGN",
	[67] = "INTL_PRIORITISED",
	[68] = "DNIC",
	[69] = "PRTY_ENCODE_CONTROL",
	[70] = "PRTY_PKT_FORCED_VAL",
	[71] = "SRC_ADDR_CONTROL",
	[72] = "DBIT_ACCEPT_IN",
	[73] = "DBIT_ACCEPT_OUT",
	[74] = "DBIT_DATA_IN",
	[75] = "DBIT_DATA_OUT",
	[76] = "THCLASS_NEG_TO_DEF",
	[77] = "THCLASS_TYPE",
	[78] = "THCLASS_WMAP",
	[79] = "THCLASS_PMAP",
	[80] = "T24VALUE",
	[81] = "T27VALUE",
	[82] = "T28VALUE",
	[83] = "R25VALUE",
	[84] = "R27VALUE",
	[85] = "R28VALUE",
};

static int
open_device(int argc, char *argv[])
{
	char *devname, *tmp;
	int fd = -1;

	/* copy the buffer */
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
	struct wlcfg cfg;

	if ((fd = open_device(argc, argv)) < 0) {
		perror(argv[0]);
		exit(1);
	}
	if (debug)
		fprintf(stderr, "%s: device opened\n", argv[0]);
	ic.ic_cmd = N_snread;
	ic.ic_timout = -1;
	ic.ic_len = sizeof(cfg);
	ic.ic_dp = (char *) &cfg;
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
	/* NET_MODE */
	printf("%d\n", cfg.NET_MODE);
	/* X25_VERSION */
	switch (cfg.X25_VSN) {
	case 0:
		printf("80\n");
		break;
	case 1:
		printf("84\n");
		break;
	case 2:
		printf("88\n");
		break;
	case 3:
		printf("1992\n");
		break;
	case 4:
		printf("1996\n");
		break;
	case 5:
		printf("2002\n");
		break;
	default:
	case 4:
		printf("2006\n");
		break;
	}
	/* L3PLPMODE */
	printf("%d\n", cfg.L3PLPMODE);
	/* LPC */
	printf("%X\n", cfg.LPC);
	/* HPC */
	printf("%X\n", cfg.HPC);
	/* LIC */
	printf("%X\n", cfg.LIC);
	/* HIC */
	printf("%X\n", cfg.HIC);
	/* LTC */
	printf("%X\n", cfg.LTC);
	/* HTC */
	printf("%X\n", cfg.HTC);
	/* LOC */
	printf("%X\n", cfg.LOC);
	/* HOC */
	printf("%X\n", cfg.HOC);
	/* THISGFI */
	switch (cfg.THISGFI) {
	case G_8:
		printf("8\n");
	case G_128:
		printf("128\n");
	case G_32768:
		printf("32768\n");
	case G_2147483648:
	default:
		printf("2147483648\n");
	}
	/* LOCMAXPKTSIZE */
	printf("%d\n", cfg.LOCMAXPKTSIZE);
	/* REMMAXPKTSIZE */
	printf("%d\n", cfg.REMMAXPKTSIZE);
	/* LOCDEFPKTSIZE */
	printf("%d\n", cfg.LOCDEFPKTSIZE);
	/* REMDEFPKTSIZE */
	printf("%d\n", cfg.REMDEFPKTSIZE);
	/* LOCMAXWSIZE */
	printf("%d\n", cfg.LOCMAXWSIZE);
	/* REMMAXWSIZE */
	printf("%d\n", cfg.REMMAXWSIZE);
	/* LOCDEFWSIZE */
	printf("%d\n", cfg.LOCDEFWSIZE);
	/* REMDEFWSIZE */
	printf("%d\n", cfg.REMDEFWSIZE);
	/* MAXNSDULEN */
	printf("%d\n", cfg.MAXNSDULEN);
	/* ACKDELAY */
	printf("%d\n", cfg.ACKDELAY);
	/* T20VALUE */
	printf("%d\n", cfg.T20value);
	/* T21VALUE */
	printf("%d\n", cfg.T21value);
	/* T22VALUE */
	printf("%d\n", cfg.T22value);
	/* T23VALUE */
	printf("%d\n", cfg.T23value);
	/* TVALUE */
	printf("%d\n", cfg.Tvalue);
	/* T25VALUE */
	printf("%d\n", cfg.T25value);
	/* T26VALUE */
	printf("%d\n", cfg.T26value);
	/* IDLEVALUE */
	printf("%d\n", cfg.idlevalue);
	/* CONNECTVALUE */
	printf("%d\n", cfg.connectvalue);
	/* R20VALUE */
	printf("%d\n", cfg.R20value);
	/* R22VALUE */
	printf("%d\n", cfg.R22value);
	/* R23VALUE */
	printf("%d\n", cfg.R23value);
	/* LOCALDELAY */
	printf("%d\n", cfg.localdelay);
	/* ACCESSDELAY */
	printf("%d\n", cfg.accessdelay);
	/* LOCMAXTHCLASS */
	printf("%d\n", cfg.locmaxthclass);
	/* REMMAXTHCLASS */
	printf("%d\n", cfg.remmaxthclass);
	/* LOCDEFTHCLASS */
	printf("%d\n", cfg.locdefthclass);
	/* REMDEFTHCLASS */
	printf("%d\n", cfg.remdefthclass);
	/* LOCMINTHCLASS */
	printf("%d\n", cfg.locminthclass);
	/* REMMINTHCLASS */
	printf("%d\n", cfg.remminthclass);
	/* SUB_CUG */
	printf("%c\n", (cfg.CUG_CONTROL & 0x01) ? 'Y' : 'N');
	/* SUB_PREF */
	printf("%c\n", (cfg.CUG_CONTROL & 0x02) ? 'Y' : 'N');
	/* SUB_CUGOA */
	printf("%c\n", (cfg.CUG_CONTROL & 0x04) ? 'Y' : 'N');
	/* SUB_CUGIA */
	printf("%c\n", (cfg.CUG_CONTROL & 0x08) ? 'Y' : 'N');
	/* CUG_FORMAT */
	printf("%c\n", (cfg.CUG_CONTROL & 0x10) ? '0' : '1');
	/* BAR_CUG_IN */
	printf("%c\n", (cfg.CUG_CONTROL & 0x20) ? 'Y' : 'N');
	/* SUB_EXTENDED */
	printf("%c\n", (cfg.SUB_MODES & 0x0001) ? 'Y' : 'N');
	/* BAR_EXTENDED */
	printf("%c\n", (cfg.SUB_MODES & 0x0002) ? 'Y' : 'N');
	/* SUB_FSELECT */
	printf("%c\n", (cfg.SUB_MODES & 0x0004) ? 'Y' : 'N');
	/* SUB_FSRRESP */
	printf("%c\n", (cfg.SUB_MODES & 0x0008) ? 'Y' : 'N');
	/* SUB_REVCHARGE */
	printf("%c\n", (cfg.SUB_MODES & 0x0010) ? 'Y' : 'N');
	/* SUB_LOC_CHG_PREV */
	printf("%c\n", (cfg.SUB_MODES & 0x0020) ? 'Y' : 'N');
	/* BAR_INCALL */
	printf("%c\n", (cfg.SUB_MODES & 0x0040) ? 'Y' : 'N');
	/* BAR_OUTCALL */
	printf("%c\n", (cfg.SUB_MODES & 0x0080) ? 'Y' : 'N');
	/* SUB_TOA_NPI_FMT */
	printf("%c\n", (cfg.SUB_MODES & 0x0100) ? 'Y' : 'N');
	/* BAR_TOA_NPI_FMT */
	printf("%c\n", (cfg.SUB_MODES & 0x0200) ? 'Y' : 'N');
	/* SUB_NUI_OVERRIDE */
	printf("%c\n", (cfg.SUB_MODES & 0x0400) ? 'Y' : 'N');
	/* ACC_NODIAG */
	printf("%c\n", (cfg.psdn_local.SNMODES & 0x01) ? 'Y' : 'N');
	/* USE_DIAG */
	printf("%c\n", (cfg.psdn_local.SNMODES & 0x02) ? 'Y' : 'N');
	/* CCITT_CLEAR_LEN */
	printf("%c\n", (cfg.psdn_local.SNMODES & 0x04) ? 'Y' : 'N');
	/* BAR_DIAG */
	printf("%c\n", (cfg.psdn_local.SNMODES & 0x08) ? 'Y' : 'N');
	/* DISC_NS_DIAG */
	printf("%c\n", (cfg.psdn_local.SNMODES & 0x10) ? 'Y' : 'N');
	/* ACC_HEX_ADD */
	printf("%c\n", (cfg.psdn_local.SNMODES & 0x20) ? 'Y' : 'N');
	/* BAR_NONPRIV_LISTEN */
	printf("%c\n", (cfg.psdn_local.SNMODES & 0x40) ? 'Y' : 'N');
	/* INTL_ADDR_RECOGN */
	printf("%d\n", cfg.psdn_local.intl_addr_recogn);
	/* INTL_PRIORITISED */
	printf("%c\n", (cfg.psdn_local.intl_prioritised) ? 'Y' : 'N');
	/* DNIC */
	printf("%02X", cfg.psdn_local.dnic1);
	printf("%02X", cfg.psdn_local.dnic2);
	printf("\n");
	/* PRTY_ENCODE_CONTROL */
	printf("%d\n", cfg.psdn_local.prty_encode_control);
	/* PRTY_PKT_FORCED_VAL */
	printf("%d\n", cfg.psdn_local.prty_pkt_forced_value);
	/* SRC_ADDR_CONTROL */
	printf("%d\n", cfg.psdn_local.src_addr_control);
	/* DBIT_ACCEPT_IN */
	switch ((cfg.psdn_local.dbit_control >> 0) & 0x03) {
	case 0:
		printf("0\n");
		break;
	case 2:
		printf("1\n");
		break;
	case 1:
	case 3:
		printf("2\n");
		break;
	}
	/* DBIT_ACCEPT_OUT */
	switch ((cfg.psdn_local.dbit_control >> 2) & 0x03) {
	case 0:
		printf("0\n");
		break;
	case 2:
		printf("1\n");
		break;
	case 1:
	case 3:
		printf("2\n");
		break;
	}
	/* DBIT_DATA_IN */
	switch ((cfg.psdn_local.dbit_control >> 4) & 0x03) {
	case 0:
		printf("0\n");
		break;
	case 2:
		printf("1\n");
		break;
	case 1:
	case 3:
		printf("2\n");
		break;
	}
	/* DBIT_DATA_OUT */
	switch ((cfg.psdn_local.dbit_control >> 6) & 0x03) {
	case 0:
		printf("0\n");
		break;
	case 2:
		printf("1\n");
		break;
	case 1:
	case 3:
		printf("2\n");
		break;
	}
	/* THCLASS_NEG_TO_DEF */
	printf("%c\n", cfg.psdn_local.thclass_neg_to_def ? 'Y' : 'N');
	/* THCLASS_TYPE */
	printf("%d\n", cfg.psdn_local.thclass_type);
	/* THCLASS_WMAP */
	for (i = 0; i < 16; i++) {
		if (i)
			printf(".");
		printf("%d", cfc.psdn_local.thclass_wmap[i]);
	}
	/* THCLASS_PMAP */
	for (i = 0; i < 16; i++) {
		if (i)
			printf(".");
		printf("%d", cfc.psdn_local.thclass_pmap[i]);
	}
	/* extensions */
	if (extended) {
		/* T24VALUE */
		printf("%d\n", cfg.T24value);
		/* T27VALUE */
		printf("%d\n", cfg.T27value);
		/* T28VALUE */
		printf("%d\n", cfg.T28value);
		/* R25VALUE */
		printf("%d\n", cfg.R25value);
		/* R27VALUE */
		printf("%d\n", cfg.R27value);
		/* R28VALUE */
		printf("%d\n", cfg.R28value);
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
	struct wlcfg cfg;
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
	/* NET_MODE */
	if (scanf("%hh\n", &cfg.NET_MODE) < 1) {
		syntaxerr(lineno, NULL);
		exit(1);
	}
	if (cfg.NET_MODE < 1 || cfg.NET_MODE > 25)
		syntaxerr(lineno, "must be between 1 and 25 inclusive");
	lineno++;
	/* X25_VERSION */
	if (scanf("8[048]\n", &c) < 1)
		syntaxerr(lineno, "must be 80, 84 or 88");
	switch (c) {
	case '0':
		cfg.X25_VSN = 0;
		break;
	case '4':
		cfg.X25_VSN = 1;
		break;
	case '8':
		cfg.X25_VSN = 2;
		break;
	}
	lineno++;
	/* L3PLPMODE */
	if (scanf("%hh\n", &cfg.L3PLPMODE) < 1)
		syntaxerr(lineno, NULL);
	if (cfg.L3PLPMODE > 3)
		syntaxerr(lineno, "must be between 0 and 3");
	lineno++;
	/* LPC */
	cfg.LPC = scanlci(lineno++);
	/* HPC */
	cfg.HPC = scanlci(lineno++);
	/* LIC */
	cfg.LIC = scanlci(lineno++);
	/* HIC */
	cfg.HIC = scanlci(lineno++);
	/* LTC */
	cfg.LTC = scanlci(lineno++);
	/* HTC */
	cfg.HTC = scanlci(lineno++);
	/* LOC */
	cfg.LOC = scanlci(lineno++);
	/* HOC */
	cfg.HOC = scanlci(lineno++);
	/* THISGFI */
	if (scanf("%u\n", &u) < 1 || u > 0x7fff)
		sytnaxerr(lineno, "must be 3 hexadecimal digits");
	switch (u) {
	case 8:
		cfg.THISGFI = G_8;
		break;
	case 128:
		cfg.THISGFI = G_128;
		break;
	case 32768:
		cfg.THISGFI = G_32768;
		break;
	case 2147483648:
		cfg.THISGFI = G_2147483648;
		break;
	default:
		sytnaxerr(lineno, "must be 8, 128, 32768 or 2147483648");
	}
	lineno++;
	/* LOCMAXPKTSIZE */
	cfg.LOCMAXPKTSIZE = scanpktsize(lineno++);
	/* REMMAXPKTSIZE */
	cfg.REMMAXPKTSIZE = scanpktsize(lineno++);
	/* LOCDEFPKTSIZE */
	cfg.LOCDEFPKTSIZE = scanpktsize(lineno++);
	/* REMDEFPKTSIZE */
	cfg.REMDEFPKTSIZE = scanpktsize(lineno++);
	/* LOCMAXWSIZE */
	cfg.LOCMAXWSIZE = scanwsize(lineno++, cfg.THISGFI);
	/* REMMAXWSIZE */
	cfg.REMMAXWSIZE = scanwsize(lineno++, cfg.THISGFI);
	/* LOCDEFWSIZE */
	cfg.LOCDEFWSIZE = scanwsize(lineno++, cfg.THISGFI);
	/* REMDEFWSIZE */
	cfg.REMDEFWSIZE = scanwsize(lineno++, cfg.THISGFI);
	/* MAXNSDULEN */
	cfg.MAXNSDULEN = scanushort(lineno++);
	/* ACKDELAY */
	cfg.ACKDELAY = scanushort(lineno++);
	/* T20VALUE */
	cfg.T20VALUE = scanushort(lineno++);
	/* T21VALUE */
	cfg.T21VALUE = scanushort(lineno++);
	/* T22VALUE */
	cfg.T22VALUE = scanushort(lineno++);
	/* T23VALUE */
	cfg.T23VALUE = scanushort(lineno++);
	/* TVALUE */
	cfg.TVALUE = scanushort(lineno++);
	/* T25VALUE */
	cfg.T25VALUE = scanushort(lineno++);
	/* T26VALUE */
	cfg.T26VALUE = scanushort(lineno++);
	/* IDLEVALUE */
	cfg.IDLEVALUE = scanushort(lineno++);
	/* CONNECTVALUE */
	cfg.CONNECTVALUE = scanushort(lineno++);
	/* R20VALUE */
	cfg.R20VALUE = scanushort(lineno++);
	/* R22VALUE */
	cfg.R22VALUE = scanushort(lineno++);
	/* R23VALUE */
	cfg.R23VALUE = scanushort(lineno++);
	/* LOCALDELAY */
	cfg.LOCALDELAY = scanushort(lineno++);
	/* ACCESSDELAY */
	cfg.ACCESSDELAY = scanushort(lineno++);
	/* LOCMAXTHCLASS */
	cfg.locmaxthclass = scanthclass(lineno++);
	/* REMMAXTHCLASS */
	cfg.remmaxthclass = scanthclass(lineno++);
	/* LOCDEFTHCLASS */
	cfg.locdefthclass = scanthclass(lineno++);
	/* REMDEFTHCLASS */
	cfg.remdefthclass = scanthclass(lineno++);
	/* LOCMINTHCLASS */
	cfg.locminthclass = scanthclass(lineno++);
	/* REMMINTHCLASS */
	cfg.remminthclass = scanthclass(lineno++);
	/* SUB_CUG, SUB_PREF, SUB_CUGOA, SUB_CUGIA, CUG_FORMAT, BAR_CUG_IN */
	for (i = 0, u = 1; i < 6; i++, lineno++, u <<= 1)
		if (scanyesno(lineno))
			cfg.CUG_CONTROL |= u;
	/* SUB_EXTENDED, BAR_EXTENDED, SUB_FSELECT, SUB_FSRRESP, SUB_REVCHARGE, SUB_LOC_CHG_PREV,
	   BAR_INCALL, BAR_OUTCALL, SUB_TOA_NPI_FMT, BAR_TOA_NPI_FMT, SUB_NUI_OVERRIDE */
	for (i = 0, u = 1; i < 11; i++, lineno++, u <<= 1)
		if (scanyesno(lineno))
			cfg.SUB_MODES |= u;
	/* ACC_NODIAG, USE_DIAG, CCITT_CLEAR_LEN, BAR_DIAG, DISC_NS_DIAG, ACC_HEX_ADD,
	   BAR_NONPRIV_LISTEN */
	for (i = 0, u = 1; i < 7; i++, lineno++, u <<= 1)
		if (scanyesno(lineno))
			cfg.psdn_local.SNMODES |= u;
	/* INTL_ADDR_RECOGN */
	if (scanf("%hh\n", &cfg.psdn_local.intl_addr_recogn) < 1) {
		exit(1);
	}
	lineno++;
	/* INTL_PRIORITISED */
	if (scanyesno(lineno))
		cfg.psdn_local.intl_prioritised = 1;
	lineno++;
	/* DNIC - 4 BCD digits */
	if (scanf("[0-9]", &c) < 1) {
		exit(1);
	}
	cfg.psdn_local.dnic1 |= ((c - '0') << 4);
	if (scanf("[0-9]", &c) < 1) {
		exit(1);
	}
	cfg.psdn_local.dnic1 |= ((c - '0') << 0);
	if (scanf("[0-9]", &c) < 1) {
		exit(1);
	}
	cfg.psdn_local.dnic2 |= ((c - '0') << 4);
	if (scanf("[0-9]\n", &c) < 1) {
		exit(1);
	}
	cfg.psdn_local.dnic2 |= ((c - '0') << 0);
	lineno++;
	/* PRTY_ENCODE_CONTROL */
	cfg.psdn_local.prty_encode_control = scanuchar(lineno++);
	/* PRTY_PKT_FORCED_VAL */
	cfg.psdn_local.prty_pkt_forced_val = scanuchar(lineno++);
	/* SRC_ADDR_CONTROL */
	cfg.psdn_local.src_addr_control = scanuchar(lineno++);
	/* DBIT_ACCEPT_IN */
	if (scanf("[012]\n", &c) < 1) {
		exit(1);
	}
	switch (c) {
	case '0':
		break;
	case '1':
		cfg.psdn_local.dbit_control |= 0x02;
		break;
	case '2':
		cfg.psdn_local.dbit_control |= 0x01;
		break;
	}
	lineno++;
	/* DBIT_ACCEPT_OUT */
	if (scanf("[012]\n", &c) < 1) {
		exit(1);
	}
	switch (c) {
	case '0':
		break;
	case '1':
		cfg.psdn_local.dbit_control |= 0x08;
		break;
	case '2':
		cfg.psdn_local.dbit_control |= 0x04;
		break;
	}
	lineno++;
	/* DBIT_DATA_IN */
	if (scanf("[012]\n", &c) < 1) {
		exit(1);
	}
	switch (c) {
	case '0':
		break;
	case '1':
		cfg.psdn_local.dbit_control |= 0x20;
		break;
	case '2':
		cfg.psdn_local.dbit_control |= 0x10;
		break;
	}
	lineno++;
	/* DBIT_DATA_OUT */
	if (scanf("[012]\n", &c) < 1) {
		exit(1);
	}
	switch (c) {
	case '0':
		break;
	case '1':
		cfg.psdn_local.dbit_control |= 0x80;
		break;
	case '2':
		cfg.psdn_local.dbit_control |= 0x40;
		break;
	}
	lineno++;
	/* THCLASS_NEG_TO_DEF */
	if (scanf("[Yy1Nn0]\n", &c) < 1) {
		exit(1);
	}
	switch (c) {
	case 'Y':
	case 'y':
	case '1':
		cfg.psdn_local.thclass_neg_to_def = 1;
		break;
	default:
		cfg.psdn_local.thclass_neg_to_def = 0;
		break;
	}
	lineno++;
	/* THCLASS_TYPE */
	cfg.psdn_local.thclass_type = scanuchar(lineno++);
	/* THCLASS_WMAP */
	if (scanf("%hh", &cfg.psdn_local.thclass_wmap[0]) < 1) {
		exit(1);
	}
	for (i = 1; i < 16; i++) {
		if (scanf(".%hh", &cfg.psdn_local.thclass_wmap[i]) < 1) {
			exit(1);
		}
	}
	lineno++;
	/* THCLASS_PMAP */
	if (scanf("%hh", &cfg.psdn_local.thclass_pmap[0]) < 1) {
		exit(1);
	}
	for (i = 1; i < 16; i++) {
		if (scanf(".%hh", &cfg.psdn_local.thclass_pmap[i]) < 1) {
			exit(1);
		}
	}
	if (extended) {
		/* extensions */
		/* T24VALUE */
		cfg.T21value = scantime(lineno++);
		/* T27VALUE */
		cfg.T27value = scantime(lineno++);
		/* T28VALUE */
		cfg.T28value = scantime(lineno++);
		/* R25VALUE */
		cfg.R25value = scantime(lineno++);
		/* R27VALUE */
		cfg.R27value = scantime(lineno++);
		/* R28VALUE */
		cfg.R28value = scantime(lineno++);
	}
	lineno++;
	if (dryrun) {
		if (debug)
			fprintf(stderr, "%s: dry run complete\n", argv[0]);
		return;
	}
	ic.ic_cmd = N_snconfig;
	ic.ic_timout = -1;
	ic.ic_len = sizeof(cfg);
	ic.ic_dp = (char *) &cfg;
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
    -a, --address ADDRESS\n\
        sets the local X.25 address for the subnet\n\
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
			{"address",	required_argument,	NULL, 'a'},
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

		c = getopt_long_only(argc, argv, "PGes:a:d:nqD::v::hVC?W:", long_options,
				     &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "PGes:a:d:nqDvhVC?");
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
		case 'a':	/* -a, --address ADDRESS */
			if (strnlen(optarg, BUFSIZ + 1) > BUFSIZ) {
				if (debug)
					fprintf(stderr, "%s: option too long\n", argv[0]);
				goto bad_option;
			}
			strncpy(address, optarg, BUFSIZ);
			/* check later */
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
			if (debug)
				fprintf(stderr, "%s: argument too long\n", argv[0]);
			goto bad_nonopt;
		}
		strncpy(filename, argv[optind], BUFSIZ);
		/* check this later */
		optind++;
	}
	if (optind < argc) {
		if (debug)
			fprintf(stderr, "%s: excess non-option arguments\n", argv[0]);
		goto bad_nonopt;
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
