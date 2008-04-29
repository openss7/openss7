/*****************************************************************************

 @(#) $RCSfile: sdlconf.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2008-04-29 07:11:33 $

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

 Last Modified $Date: 2008-04-29 07:11:33 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sdlconf.c,v $
 Revision 0.9.2.11  2008-04-29 07:11:33  brian
 - updating headers for release

 Revision 0.9.2.10  2007/09/06 11:16:27  brian
 - testing updates

 Revision 0.9.2.9  2007/08/19 11:57:40  brian
 - move stdbool.h, obviate need for YFLAGS, general workup

 Revision 0.9.2.8  2007/08/18 03:53:15  brian
 - working up configuration files

 Revision 0.9.2.7  2007/08/12 16:49:58  brian
 - header updates

 Revision 0.9.2.6  2007/08/12 16:20:49  brian
 - new PPA handling

 *****************************************************************************/

#ident "@(#) $RCSfile: sdlconf.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2008-04-29 07:11:33 $"

static char const ident[] =
    "$RCSfile: sdlconf.c,v $ $Name:  $($Revision: 0.9.2.11 $) $Date: 2008-04-29 07:11:33 $";

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

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>

struct key_val {
	char *key;
	unsigned long val;
};
static struct key_val type_names[] = {
	{"default", SDL_TYPE_NONE},
	{"v35", SDL_TYPE_V35},
	{"ds0", SDL_TYPE_DS0},
	{"ds0a", SDL_TYPE_DS0A},
	{"e1", SDL_TYPE_E1},
	{"t1", SDL_TYPE_T1},
	{"j1", SDL_TYPE_J1},
	{"atm", SDL_TYPE_ATM},
	{"packet", SDL_TYPE_PACKET}
};
static struct key_val gtype_names[] = {
	{"default", SDL_GTYPE_NONE},
	{"t1", SDL_GTYPE_T1},
	{"e1", SDL_GTYPE_E1},
	{"j1", SDL_GTYPE_J1},
	{"atm", SDL_GTYPE_ATM},
	{"eth", SDL_GTYPE_ETH},
	{"ip", SDL_GTYPE_IP},
	{"udp", SDL_GTYPE_UDP},
	{"tcp", SDL_GTYPE_TCP},
	{"rtp", SDL_GTYPE_RTP},
	{"sctp", SDL_GTYPE_SCTP}
};
static struct key_val mode_names[] = {
	{"default", SDL_MODE_NONE},
	{"dsu", SDL_MODE_DSU},
	{"csu", SDL_MODE_CSU},
	{"dte", SDL_MODE_DTE},
	{"dce", SDL_MODE_DCE},
	{"client", SDL_MODE_CLIENT},
	{"server", SDL_MODE_SERVER},
	{"peer", SDL_MODE_PEER},
	{"echo", SDL_MODE_ECHO},
	{"rem_lb", SDL_MODE_REM_LB},
	{"loc_lb", SDL_MODE_LOC_LB},
	{"lb_echo", SDL_MODE_LB_ECHO},
	{"test", SDL_MODE_TEST}
};
static struct key_val gmode_names[] = {
	{"default", SDL_GMODE_NONE},
	{"loc_lb", SDL_GMODE_LOC_LB},
	{"rem_lb", SDL_GMODE_REM_LB}
};
static struct key_val gcrc_names[] = {
	{"default", SDL_GCRC_NONE},
	{"crc4", SDL_GCRC_CRC4},
	{"crc5", SDL_GCRC_CRC5},
	{"crc6", SDL_GCRC_CRC6},
	{"crc6j", SDL_GCRC_CRC6J}
};
static struct key_val clock_names[] = {
	{"default", SDL_CLOCK_NONE},
	{"int", SDL_CLOCK_INT},
	{"ext", SDL_CLOCK_EXT},
	{"loop", SDL_CLOCK_LOOP},
	{"master", SDL_CLOCK_MASTER},
	{"slave", SDL_CLOCK_SLAVE},
	{"dpll", SDL_CLOCK_DPLL},
	{"abr", SDL_CLOCK_ABR},
	{"shaper", SDL_CLOCK_SHAPER},
	{"tick", SDL_CLOCK_TICK}
};
static struct key_val coding_names[] = {
	{"default", SDL_CODING_NONE},
	{"nrz", SDL_CODING_NRZ},
	{"nrzi", SDL_CODING_NRZI},
	{"ami", SDL_CODING_AMI},
	{"b6zs", SDL_CODING_B6ZS},
	{"b8zs", SDL_CODING_B8ZS},
	{"hbd3", SDL_CODING_HDB3},
	{"aal1", SDL_CODING_AAL1},
	{"aal2", SDL_CODING_AAL2},
	{"aal5", SDL_CODING_AAL5}
};
static struct key_val framing_names[] = {
	{"default", SDL_FRAMING_NONE},
	{"ccs", SDL_FRAMING_CCS},
	{"cas", SDL_FRAMING_CAS},
	{"d4", SDL_FRAMING_D4},
	{"esf", SDL_FRAMING_ESF},
	{"sf", SDL_FRAMING_SF}
};
static struct key_val rate_names[] = {
	{"default", SDL_RATE_NONE},
	{"ds0a", SDL_RATE_DS0A},
	{"ds0", SDL_RATE_DS0},
	{"t1", SDL_RATE_T1},
	{"j1", SDL_RATE_J1},
	{"e1", SDL_RATE_E1}
};
static struct key_val grate_names[] = {
	{"default", SDL_GRATE_NONE},
	{"t1", SDL_GRATE_T1},
	{"j1", SDL_GRATE_J1},
	{"e1", SDL_GRATE_E1}
};
static struct key_val txlevel_names[] = {
	{"default", SDL_TXLEVEL_NONE},
	{"DSX-1 0-133ft", SDL_TXLEVEL_DSX_133FT},
	{"DSX-1 133-266ft", SDL_TXLEVEL_DSX_266FT},
	{"DSX-1 266-399ft", SDL_TXLEVEL_DSX_399FT},
	{"DSX-1 399-533ft", SDL_TXLEVEL_DSX_533FT},
	{"DSX-1 533-666ft", SDL_TXLEVEL_DSX_666FT},
	{"0dB CSU", SDL_TXLEVEL_CSU_0DB},
	{"-7.5dB CSU", SDL_TXLEVEL_CSU_8DB},
	{"-15dB CSU", SDL_TXLEVEL_CSU_15DB},
	{"-22.5dB CSU", SDL_TXLEVEL_CSU_23DB},
	{"75 Ohm, Normal", SDL_TXLEVEL_75OHM_NM},
	{"120 Ohm, Normal", SDL_TXLEVEL_120OHM_NM},
	{"75 Ohm, Protected", SDL_TXLEVEL_75OHM_PR},
	{"120 Ohm, Protected", SDL_TXLEVEL_120OHM_PR},
	{"75 Ohm, High Return Loss", SDL_TXLEVEL_75OHM_HRL},
	{"120 Ohm, High Return Loss", SDL_TXLEVEL_120OHM_HRL},
	{"0Db Monitor", SDL_TXLEVEL_MON_0DB},
	{"12dB Monitor", SDL_TXLEVEL_MON_12DB},
	{"20dB Monitor", SDL_TXLEVEL_MON_20DB},
	{"26dB Monitor", SDL_TXLEVEL_MON_26DB},
	{"30dB Monitor", SDL_TXLEVEL_MON_30DB},
	{"32dB Monitor", SDL_TXLEVEL_MON_32DB},
};

sdl_config_t config = {
	.ifname = NULL,
	.ifflags = 0,
	.iftype = SDL_TYPE_NONE,
	.ifrate = SDL_RATE_NONE,
	.ifgtype = SDL_GTYPE_NONE,
	.ifgrate = SDL_GRATE_NONE,
	.ifmode = SDL_MODE_NONE,
	.ifgmode = SDL_GMODE_NONE,
	.ifgcrc = SDL_GCRC_NONE,
	.ifclock = SDL_CLOCK_NONE,
	.ifcoding = SDL_CODING_NONE,
	.ifframing = SDL_FRAMING_NONE,
	.ifblksize = 0,
	.ifleads = 0,
	.ifbpv = 0,
	.ifalarms = 0,
	.ifrxlevel = 0,
	.iftxlevel = SDL_TXLEVEL_NONE,
	.ifsync = 0,
	.ifsyncsrc = {1, 2, 3, 4},
};

struct optname_struct {
	struct key_val *name;
	size_t size;
	volatile sdl_ulong *valp;
};

enum {
	CFG_TYPE,
	CFG_GTYPE,
	CFG_MODE,
	CFG_GMODE,
	CFG_GCRC,
	CFG_CLOCK,
	CFG_CODING,
	CFG_FRAMING,
	CFG_RATE,
	CFG_GRATE,
	CFG_TXLEVEL,
	CFG_SYNCSRC,
};

struct optname_struct optnames[] = {
	{type_names, sizeof(type_names) / sizeof(struct key_val), &config.iftype},
	{gtype_names, sizeof(gtype_names) / sizeof(struct key_val), &config.ifgtype},
	{mode_names, sizeof(mode_names) / sizeof(struct key_val), &config.ifmode},
	{gmode_names, sizeof(gmode_names) / sizeof(struct key_val), &config.ifgmode},
	{gcrc_names, sizeof(gcrc_names) / sizeof(struct key_val), &config.ifgcrc},
	{clock_names, sizeof(clock_names) / sizeof(struct key_val), &config.ifclock},
	{coding_names, sizeof(coding_names) / sizeof(struct key_val), &config.ifcoding},
	{framing_names, sizeof(framing_names) / sizeof(struct key_val), &config.ifframing},
	{rate_names, sizeof(rate_names) / sizeof(struct key_val), &config.ifrate},
	{grate_names, sizeof(grate_names) / sizeof(struct key_val), &config.ifgrate},
	{txlevel_names, sizeof(txlevel_names) / sizeof(struct key_val), &config.iftxlevel}
};

int card = 0;
int span = 0;
int slot = 1;

int my_argc = 0;
char **my_argv = NULL;

int output = 1;

char devname[256] = "/dev/x400p-sl";
char cfgfile[256] = "/etc/sysconfig/ss7/sdl.config";

void
copying(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
\n\
Unauthorized distribution or duplication is prohibited.\n\
\n\
This software and related documentation is protected by copyright and distribut-\n\
ed under licenses restricting its use,  copying, distribution and decompilation.\n\
No part of this software or related documentation may  be reproduced in any form\n\
by any means without the prior  written  authorization of the  copyright holder,\n\
and licensors, if any.\n\
\n\
The recipient of this document,  by its retention and use, warrants that the re-\n\
cipient  will protect this  information and  keep it confidential,  and will not\n\
disclose the information contained  in this document without the written permis-\n\
sion of its owner.\n\
\n\
The author reserves the right to revise  this software and documentation for any\n\
reason,  including but not limited to, conformity with standards  promulgated by\n\
various agencies, utilization of advances in the state of the technical arts, or\n\
the reflection of changes  in the design of any techniques, or procedures embod-\n\
ied, described, or  referred to herein.   The author  is under no  obligation to\n\
provide any feature listed herein.\n\
\n\
As an exception to the above,  this software may be  distributed  under the  GNU\n\
Affero  General  Public  License  (AGPL)  Version  3, so long as the software is\n\
distributed with,  and only used for the testing of,  OpenSS7 modules,  drivers,\n\
and libraries.\n\
\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the  U.S. Government  (\"Government\"),  the following provisions apply to you.\n\
If the Software is  supplied by the Department of Defense (\"DoD\"), it is classi-\n\
fied as  \"Commercial Computer Software\"  under paragraph 252.227-7014 of the DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the license rights  granted\n\
herein (the license  rights customarily  provided to non-Government  users).  If\n\
the Software is supplied to any unit or agency of the Government other than DoD,\n\
it is classified as  \"Restricted Computer Software\" and the  Government's rights\n\
in the  Software are defined in  paragraph 52.227-19 of the Federal  Acquisition\n\
Regulations  (\"FAR\") (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of the  NASA Supplement  to the  FAR (or  any  successor\n\
regulations).\n\
\n\
");
}

void
version(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
Written by Brian Bidulock.\n\
\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 Corporation under GNU Affero General Public License Version 3,\n\
incorporated herein by reference.  See `%1$s --copying' for copying permission.\n\
", NAME, PACKAGE, VERSION, "$Revision: 0.9.2.11 $ $Date: 2008-04-29 07:11:33 $");
}

void
usage(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options] {-g, --get} {-d, --device} [{-c, --card} card] [{-p, --span} span] [{-t, --slot} slot]\n\
    %1$s [options] {-s, --set} {-d, --device} [{-c, --card} card] [{-p, --span} span] [{-t, --slot} slot] [set-options]\n\
    %1$s [options] {-f, --file} config_file\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
", argv[0]);
}

void
help(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
\n\
Usage:\n\
    %1$s [options] {-g, --get} {-d, --device} [{-c, --card} card] [{-p, --span} span] [{-t, --slot} slot]\n\
    %1$s [options] {-s, --set} {-d, --device} [{-c, --card} card] [{-p, --span} span] [{-t, --slot} slot] [set-options]\n\
    %1$s [options] {-f, --file} config_file\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
Arguments:\n\
    (none)\n\
General Options:\n\
    -q, --quiet				(default: off)\n\
	suppress output\n\
    -v, --verbose			(default: off)\n\
	increase verbosity of output\n\
Command Options:\n\
    -g, --get\n\
	get (display) device configuration\n\
    -s, --set\n\
	set (configure) device configuration\n\
    -f, --file file			(default: %2$s)\n\
	read device configuration from file\n\
    -h, --help\n\
	prints this usage information and exits\n\
    -V, --version\n\
	prints the version and exits\n\
    -C, --copying\n\
	prints the copying permissions and exits\n\
Set/Get Options:\n\
    -d, --device devname		(default: %3$s)\n\
	device name\n\
    -c, --card card			(default: %4$d)\n\
	card in host (numbered from 0)\n\
    -p, --span span			(default: %5$d)\n\
	span on card (numbered from 0)\n\
    -t, --slot slot			(default: %6$d)\n\
	timeslot (numbered from 1, 0 for entire span)\n\
Set Options:\n\
    --clock				(default: %7$s)\n\
	span clock source: default int ext loop master slave dpll abr shaper tick\n\
    --rate				(default: %8$u)\n\
	channel interface rate: 56000 64000 1544000 2048000\n\
    --type				(default: %9$s)\n\
	channel interface type: default v35 ds0 ds0a e1 t1 j1 atm packet\n\
    --mode				(default: %10$s)\n\
	channel interface mode: default dsu csu dte dce client server peer echo rem_lb loc_lb lb_echo test\n\
    --grate				(default: %11$u)\n\
	span interface rate: 1544000 2048000 10000000 100000000\n\
    --gtype				(default: %12$s)\n\
	span interface type: default t1 e1 j1 atm eth ip udp tcp rtp sctp\n\
    --gmode				(default: %13$s)\n\
	span interface mode: default loc_lb rem_lb\n\
    --gcrc				(default: %14$s)\n\
	span CRC calulation: default crc4 crc5 crc6 crc6j\n\
    --coding				(default: %15$s)\n\
	span coding: default nrz nrzi ami b6zs b8zs aal1 aal2 aal5 hbd3\n\
    --framing				(default: %16$s)\n\
	span framing: ccs cas sf esf\n\
    --syncsrc sync1,sync2,sync3,sync4\n\
	span synchronization: 0 - self, 1 - span1 ...\n\
", argv[0], cfgfile, devname, card, span, slot, clock_names[config.ifclock].key, config.ifrate, type_names[config.iftype].key, mode_names[config.ifmode].key, config.ifgrate, gtype_names[config.ifgtype].key, gmode_names[config.ifgmode].key, gcrc_names[config.ifgcrc].key, coding_names[config.ifcoding].key, framing_names[config.ifframing].key);
}

void
show_config_long(sdl_config_t * cfg, int get)
{
	char *unknown = "unknown";

	printf("Configuration:\n");
	printf("  %s card %d, span %d, slot %d\n", devname, card, span, slot);
	printf("  iftype: %s(%u)\n",
	       (cfg->iftype < optnames[CFG_TYPE].size) ?
	       optnames[CFG_TYPE].name[cfg->iftype].key : unknown, cfg->iftype);
	printf("  ifrate: %u\n", cfg->ifrate);
	printf("  ifgtype: %s(%u)\n",
	       (cfg->ifgtype < optnames[CFG_GTYPE].size) ?
	       optnames[CFG_GTYPE].name[cfg->ifgtype].key : unknown, cfg->ifgtype);
	printf("  ifgrate: %u\n", cfg->ifgrate);
	printf("  ifmode: %s(%u)\n",
	       (cfg->ifmode < optnames[CFG_MODE].size) ?
	       optnames[CFG_MODE].name[cfg->ifmode].key : unknown, cfg->ifmode);
	printf("  ifgmode: %s(%u)\n",
	       (cfg->ifgmode < optnames[CFG_GMODE].size) ?
	       optnames[CFG_GMODE].name[cfg->ifgmode].key : unknown, cfg->ifgmode);
	printf("  ifgcrc: %s(%u)\n",
	       (cfg->ifgcrc < optnames[CFG_GCRC].size) ?
	       optnames[CFG_GCRC].name[cfg->ifgcrc].key : unknown, cfg->ifgcrc);
	printf("  ifclock: %s(%u)\n",
	       (cfg->ifclock < optnames[CFG_CLOCK].size) ?
	       optnames[CFG_CLOCK].name[cfg->ifclock].key : unknown, cfg->ifclock);
	printf("  ifcoding: %s(%u)\n",
	       (cfg->ifcoding < optnames[CFG_CODING].size) ?
	       optnames[CFG_CODING].name[cfg->ifcoding].key : unknown, cfg->ifcoding);
	printf("  ifframing: %s(%u)\n",
	       (cfg->ifframing < optnames[CFG_FRAMING].size) ?
	       optnames[CFG_FRAMING].name[cfg->ifframing].key : unknown, cfg->ifframing);
	printf("  ifblksize: %u\n", cfg->ifblksize);
	printf("  iftxlevel: %u\n", cfg->iftxlevel);
	printf("  ifsyncsrc: %u %u %u %u\n", cfg->ifsyncsrc[0], cfg->ifsyncsrc[1],
	       cfg->ifsyncsrc[2], cfg->ifsyncsrc[3]);
	if (get) {
		printf("  ifleads:");
		if (cfg->ifleads & SDL_LEAD_DTR)
			printf(" dtr");
		if (cfg->ifleads & SDL_LEAD_RTS)
			printf(" rts");
		if (cfg->ifleads & SDL_LEAD_DCD)
			printf(" dcd");
		if (cfg->ifleads & SDL_LEAD_CTS)
			printf(" cts");
		if (cfg->ifleads & SDL_LEAD_DSR)
			printf(" dsr");
		printf("\n");
		printf("  ifalarms:");
		if (cfg->ifalarms & SDL_ALARM_RED)
			printf(" red");
		if (cfg->ifalarms & SDL_ALARM_BLU)
			printf(" blu");
		if (cfg->ifalarms & SDL_ALARM_YEL)
			printf(" yel");
		if (cfg->ifalarms & SDL_ALARM_REC)
			printf(" rec");
		printf("\n");
		printf("  ifrxlevel: %u\n", cfg->ifrxlevel);
		printf("  ifsync: %u\n", cfg->ifsync);
		printf("  ifbpv: %u\n", cfg->ifbpv);
	}
}

void
show_config(sdl_config_t * cfg, int get)
{
	char *unknown = "unknown";

	printf("Configuration:");
	printf("  %s card %d, span %d, slot %d\n", devname, card, span, slot);
	printf("  type: %s(%u)",
	       (cfg->iftype < optnames[CFG_TYPE].size) ?
	       optnames[CFG_TYPE].name[cfg->iftype].key : unknown, cfg->iftype);
	printf(", rate: %u", cfg->ifrate);
	printf(", mode: %s(%u)\n",
	       (cfg->ifmode < optnames[CFG_MODE].size) ?
	       optnames[CFG_MODE].name[cfg->ifmode].key : unknown, cfg->ifmode);
	printf("  gtype: %s(%u)",
	       (cfg->ifgtype < optnames[CFG_GTYPE].size) ?
	       optnames[CFG_GTYPE].name[cfg->ifgtype].key : unknown, cfg->ifgtype);
	printf(", grate: %u", cfg->ifgrate);
	printf(", gmode: %s(%u)",
	       (cfg->ifgmode < optnames[CFG_GMODE].size) ?
	       optnames[CFG_GMODE].name[cfg->ifgmode].key : unknown, cfg->ifgmode);
	printf(", clock: %s(%u)\n",
	       (cfg->ifclock < optnames[CFG_CLOCK].size) ?
	       optnames[CFG_CLOCK].name[cfg->ifclock].key : unknown, cfg->ifclock);
	printf("  gcrc: %s(%u)",
	       (cfg->ifgcrc < optnames[CFG_GCRC].size) ?
	       optnames[CFG_GCRC].name[cfg->ifgcrc].key : unknown, cfg->ifgcrc);
	printf(", coding: %s(%u)",
	       (cfg->ifcoding < optnames[CFG_CODING].size) ?
	       optnames[CFG_CODING].name[cfg->ifcoding].key : unknown, cfg->ifcoding);
	printf(", framing: %s(%u)\n",
	       (cfg->ifframing < optnames[CFG_FRAMING].size) ?
	       optnames[CFG_FRAMING].name[cfg->ifframing].key : unknown, cfg->ifframing);
	printf("  blksize: %u", cfg->ifblksize);
	printf(", txlevel: %u", cfg->iftxlevel);
	printf(", syncsrc: {%u,%u,%u,%u}\n", cfg->ifsyncsrc[0], cfg->ifsyncsrc[1],
	       cfg->ifsyncsrc[2], cfg->ifsyncsrc[3]);
	if (get) {
		printf("  leads:");
		if (cfg->ifleads & SDL_LEAD_DTR)
			printf(" dtr");
		if (cfg->ifleads & SDL_LEAD_RTS)
			printf(" rts");
		if (cfg->ifleads & SDL_LEAD_DCD)
			printf(" dcd");
		if (cfg->ifleads & SDL_LEAD_CTS)
			printf(" cts");
		if (cfg->ifleads & SDL_LEAD_DSR)
			printf(" dsr");
		printf(", alarms:");
		if (cfg->ifalarms & SDL_ALARM_RED)
			printf(" red");
		if (cfg->ifalarms & SDL_ALARM_BLU)
			printf(" blu");
		if (cfg->ifalarms & SDL_ALARM_YEL)
			printf(" yel");
		if (cfg->ifalarms & SDL_ALARM_REC)
			printf(" rec");
		printf(", rxlevel: %u", cfg->ifrxlevel);
		printf(", sync: %u", cfg->ifsync);
		printf(", bpv: %u\n", cfg->ifbpv);
	}
}

#if 0
void
show_stats(sdl_stats_t * sta)
{
	printf("Statistics:");
	printf("  %s card %d, span %d, slot %d\n", devname, card, span, slot);
	printf("  rx_octets:           %12u\n", sta->rx_octets);
	printf("  tx_octets:           %12u\n", sta->tx_octets);
	printf("  rx_overruns:         %12u\n", sta->rx_overruns);
	printf("  tx_underruns:        %12u\n", sta->tx_underruns);
	printf("  rx_buffer_overflows: %12u\n", sta->rx_buffer_overflows);
	printf("  rx_buffer_overflows: %12u\n", sta->rx_buffer_overflows);
	printf("  lead_cts_lost:       %12u\n", sta->lead_cts_lost);
	printf("  lead_dcd_lost:       %12u\n", sta->lead_dcd_lost);
	printf("  carrier_lost:        %12u\n", sta->carrier_lost);
}
#endif

#define BUFSIZE 128

void
sdlconf_get(void)
{
	sdl_config_t cfg;
#if 0
	sdl_stats_t sta;
#endif
	struct strioctl ctl;
	char cbuf[BUFSIZE];
	char dbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*dbuf), 0, dbuf };
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	unsigned short ppa = (card << 12) | (span << 8) | (slot << 0);
	int fd, ret, flags = 0;

	if ((fd = open(devname, O_RDWR)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	ctrl.maxlen = sizeof(cbuf);
	ctrl.len = sizeof(p->attach_req) + sizeof(ppa);
	ctrl.buf = cbuf;
	p->attach_req.lmi_primitive = LMI_ATTACH_REQ;
	p->attach_req.lmi_ppa_length = sizeof(ppa);
	p->attach_req.lmi_ppa_offset = sizeof(p->attach_req);
	bcopy(&ppa, p->attach_req.lmi_ppa, sizeof(ppa));
	if ((ret = putmsg(fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	flags = 0;
	ctrl.maxlen = BUFSIZE;
	ctrl.len = 0;
	ctrl.buf = cbuf;
	data.maxlen = BUFSIZE;
	data.len = 0;
	data.buf = dbuf;
	if ((ret = getmsg(fd, &ctrl, &data, &flags)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	if (p->lmi_primitive != LMI_OK_ACK) {
		if (p->lmi_primitive == LMI_ERROR_ACK) {
			fprintf(stderr, "%s: %s: could not attach (LMI_ERROR_ACK)\n", "sdlconf",
				__FUNCTION__);
			fprintf(stderr, "%s: %s: reason %08x, errno %08x\n", "sdlconf",
					__FUNCTION__, (uint) p->error_ack.lmi_reason,
					(uint) p->error_ack.lmi_errno);
		} else
			fprintf(stderr, "%s: %s: could not attach\n", "sdlconf", __FUNCTION__);
		exit(1);
	}
	ctl.ic_cmd = SDL_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(cfg);
	ctl.ic_dp = (char *) &cfg;
	if (ioctl(fd, I_STR, &ctl) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
#if 0
	ctl.ic_cmd = SDL_IOCGSTATS;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(sta);
	ctl.ic_dp = (char *) &sta;
	if (ioctl(fd, I_STR, &ctl) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
#endif
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->detach_req);
	ctrl.buf = cbuf;
	p->detach_req.lmi_primitive = LMI_DETACH_REQ;
	if ((ret = putmsg(fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	if (output) {
		show_config(&cfg, 1);
#if 0
		show_stats(&sta);
#endif
	}
}

void
sdlconf_set(void)
{
	sdl_config_t cfg;
	struct strioctl ctl;
	char cbuf[BUFSIZE];
	char dbuf[BUFSIZE];
	struct strbuf ctrl = { sizeof(*cbuf), 0, cbuf };
	struct strbuf data = { sizeof(*dbuf), 0, dbuf };
	union LMI_primitives *p = (union LMI_primitives *) cbuf;
	unsigned short ppa = (card << 12) | (span << 8) | (slot << 0);
	int fd, ret, flags = 0;

	cfg = config;
	if (output > 2)
		show_config(&cfg, 0);
	if ((fd = open(devname, O_RDWR)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	ctrl.maxlen = sizeof(cbuf);
	ctrl.len = sizeof(p->attach_req) + sizeof(ppa);
	ctrl.buf = cbuf;
	p->attach_req.lmi_primitive = LMI_ATTACH_REQ;
	p->attach_req.lmi_ppa_length = sizeof(ppa);
	p->attach_req.lmi_ppa_offset = sizeof(p->attach_req);
	bcopy(&ppa, p->attach_req.lmi_ppa, sizeof(ppa));
	if ((ret = putmsg(fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	flags = 0;
	ctrl.maxlen = BUFSIZE;
	ctrl.len = 0;
	ctrl.buf = cbuf;
	data.maxlen = BUFSIZE;
	data.len = 0;
	data.buf = dbuf;
	if ((ret = getmsg(fd, &ctrl, &data, &flags)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	if (p->lmi_primitive != LMI_OK_ACK) {
		if (p->lmi_primitive == LMI_ERROR_ACK) {
			fprintf(stderr, "%s: %s: could not attach (LMI_ERROR_ACK)\n", "sdlconf",
				__FUNCTION__);
		} else
			fprintf(stderr, "%s: %s: could not attach\n", "sdlconf", __FUNCTION__);
		exit(1);
	}
	if (output > 1)
		show_config(&cfg, 0);
	ctl.ic_cmd = SDL_IOCSCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(cfg);
	ctl.ic_dp = (char *) &cfg;
	if (ioctl(fd, I_STR, &ctl) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	ctl.ic_cmd = SDL_IOCGCONFIG;
	ctl.ic_timout = 0;
	ctl.ic_len = sizeof(cfg);
	ctl.ic_dp = (char *) &cfg;
	if (ioctl(fd, I_STR, &ctl) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	ctrl.maxlen = BUFSIZE;
	ctrl.len = sizeof(p->detach_req);
	ctrl.buf = cbuf;
	p->detach_req.lmi_primitive = LMI_DETACH_REQ;
	if ((ret = putmsg(fd, &ctrl, NULL, RS_HIPRI)) < 0) {
		perror(__FUNCTION__);
		exit(1);
	}
	if (output)
		show_config(&cfg, 1);
}

extern void sdlconf_int(void);

void
sdlconf_cfg(void)
{
	fprintf(stderr, "%s: %s: file option not currently available\n", "sdlconf", __FUNCTION__);
	exit(1);
}

int
main(int argc, char **argv)
{
	int c;
	int val;
	enum {
		CMD_NONE = 0,
		CMD_GET,		/* get device */
		CMD_SET,		/* set device */
		CMD_INT,		/* interactive */
		CMD_CFG,		/* configure from file */
	} cmd = CMD_NONE;

	my_argc = argc;
	my_argv = argv;

	while (1) {
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"get",		no_argument,		NULL, 'g'},
			{"set",		no_argument,		NULL, 's'},
			{"interactive",	no_argument,		NULL, 'i'},
			{"file",	required_argument,	NULL, 'f'},
			{"device",	required_argument,	NULL, 'd'},
			{"card",	required_argument,	NULL, 'c'},
			{"span",	required_argument,	NULL, 'p'},
			{"slot",	required_argument,	NULL, 't'},
			{"type",	required_argument,	NULL, CFG_TYPE + 1},
			{"gtype",	required_argument,	NULL, CFG_GTYPE + 1},
			{"mode",	required_argument,	NULL, CFG_MODE + 1},
			{"gmode",	required_argument,	NULL, CFG_GMODE + 1},
			{"gcrc",	required_argument,	NULL, CFG_GCRC + 1},
			{"clock",	required_argument,	NULL, CFG_CLOCK + 1},
			{"coding",	required_argument,	NULL, CFG_CODING + 1},
			{"framing",	required_argument,	NULL, CFG_FRAMING + 1},
			{"rate",	required_argument,	NULL, CFG_RATE + 1},
			{"grate",	required_argument,	NULL, CFG_GRATE + 1},
			{"txlevel",	required_argument,	NULL, CFG_TXLEVEL + 1},
			{"syncsrc",	required_argument,	NULL, CFG_SYNCSRC + 1},
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'h'},
			{NULL,		0,			NULL,  0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "gsif:c:p:t:qvhVC?", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case 'g':	/* -g, --get */
			if (cmd != CMD_NONE)
				goto bad_option;
			cmd = CMD_GET;
			break;
		case 's':	/* -s, --set */
			if (cmd != CMD_NONE)
				goto bad_option;
			cmd = CMD_SET;
			break;
		case 'i':
			if (cmd != CMD_NONE)
				goto bad_option;
			cmd = CMD_INT;
			break;
		case 'f':	/* -f, --file file */
			if (cmd != CMD_NONE)
				goto bad_option;
			cmd = CMD_CFG;
			strncpy(cfgfile, optarg, sizeof(cfgfile) - 1);
			break;
		case 'd':	/* -d, --device */
			if (cmd != CMD_SET && cmd != CMD_GET)
				goto bad_option;
			strncpy(devname, optarg, sizeof(devname) - 1);
			break;
		case 'c':	/* -c, --card */
			if (cmd != CMD_SET && cmd != CMD_GET)
				goto bad_option;
			card = atoi(optarg);
			break;
		case 'p':	/* -p, --span */
			if (cmd != CMD_SET && cmd != CMD_GET)
				goto bad_option;
			span = atoi(optarg);
			break;
		case 't':	/* -t, --slot */
			if (cmd != CMD_SET && cmd != CMD_GET)
				goto bad_option;
			slot = atoi(optarg);
			break;
		case CFG_TYPE + 1:	/* --type */
		case CFG_GTYPE + 1:	/* --gtype */
		case CFG_MODE + 1:	/* --mode */
		case CFG_GMODE + 1:	/* --gmode */
		case CFG_GCRC + 1:	/* --gcrc */
		case CFG_CLOCK + 1:	/* --clock */
		case CFG_CODING + 1:	/* --coding */
		case CFG_FRAMING + 1:	/* --framing */
		case CFG_RATE + 1:	/* --rate */
		case CFG_GRATE + 1:	/* --grate */
		case CFG_TXLEVEL + 1:	/* --txlevel */
		{
			int i;

			if (cmd != CMD_SET)
				goto bad_option;
			if ('0' <= optarg[0] && optarg[0] <= '9') {
				*(optnames[c - 1].valp) = atoi(optarg);
				break;
			}
			for (i = 0; i < optnames[c - 1].size; i++)
				if (!strcmp(optarg, optnames[c - 1].name[i].key))
					break;
			if (i < optnames[c - 1].size) {
				*(optnames[c - 1].valp) = optnames[c - 1].name[i].val;
				break;
			}
			fprintf(stderr, "%s: illegal parm -- %s\n", argv[0], argv[optind - 1]);
			fprintf(stderr, "%s: valid labels --", argv[0]);
			for (i = 0; i < optnames[c - 1].size; i++)
				fprintf(stderr, " %s", optnames[c - 1].name[i].key);
			fprintf(stderr, "\n");
			usage(argc, argv);
			exit(2);
		}
		case CFG_SYNCSRC + 1:	/* --syncsrc */
		{
			ulong syncs[4] = { 0, 0, 0, 0 };
			int i, j, p, len;

			if (cmd != CMD_SET)
				goto bad_option;
			len = strlen(optarg);
			for (p = 0, j = 0, i = 0; i < 4; i++) {
				for (; j <= len; j++)
					if (optarg[j] == ',' || optarg[j] == '\0')
						goto gotone;
				if (j > p) {
				      gotone:
					syncs[i] = atoi(&optarg[p]);
					if (output > 3)
						fprintf(stderr, "got one: %d : %lu\n", i, syncs[i]);
					j++;
					p = j;
					if (j >= len)
						break;
				}
			}
			if (i == 0)
				goto bad_option;
			for (j = 0; j <= i; j++)
				config.ifsyncsrc[j] = syncs[j];
			break;
		}
		case 'q':	/* -q, --quiet */
			output -= output > 0 ? 1 : output;
			break;
		case 'v':	/* -v, --verbose */
			if (optarg == NULL) {
				output += 1;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			break;
		case 'h':	/* -h, --help */
			if (cmd != CMD_NONE)
				goto bad_option;
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			if (cmd != CMD_NONE)
				goto bad_option;
			version(argc, argv);
			exit(0);
		case 'C':
			if (cmd != CMD_NONE)
				goto bad_option;
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
			// syntax_error:
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
	switch (cmd) {
	default:
	case CMD_NONE:
		help(argc, argv);
		exit(0);
	case CMD_GET:
		sdlconf_get();
		exit(0);
	case CMD_SET:
		sdlconf_set();
		exit(0);
	case CMD_INT:
		sdlconf_int();
		exit(0);
	case CMD_CFG:
		sdlconf_cfg();
		exit(0);
	}
	exit(4);
}
