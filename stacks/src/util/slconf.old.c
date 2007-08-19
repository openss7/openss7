/*****************************************************************************

 @(#) $RCSfile: slconf.old.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $

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

 Last Modified $Date: 2007/08/18 03:53:15 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: slconf.old.c,v $
 Revision 0.9.2.1  2007/08/18 03:53:15  brian
 - working up configuration files

 *****************************************************************************/

#ident "@(#) $RCSfile: slconf.old.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $"

static char const ident[] = "$RCSfile: slconf.old.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $";

/*
 *  This is slconf(8).  It is responsible for configuring the signalling link
 *  multiplexer and managing signalling links from there.  It understands the
 *  particular needs of some drivers such as sl-pmod, ch, x400p, m2pa, m2ua,
 *  etc, to the extent that is necessary to create a signalling link of these
 *  types and link them under the SL-MUX multiplexing driver.  Once linked,
 *  all of these signalling link types are managed in the same way.
 */

int my_argc;
char **my_argv;

int output = 1;
int debug = 0;

#ifndef MAX_PATH_LENGTH
#define MAX_PATH_LENGTH 256
#endif

#ifndef X400P_CONF_FILENAME
#define X400P_CONF_FILENAME "/etc/sysconfig/x400p.conf"
#endif

char filename[MAX_PATH_LENGTH] = X400P_CONF_FILENAME;

#define MAX_CLEI_LENGTH 64

char clei[MAX_CLEI_LENGTH] = "";

#define MAX_DEVICE_LENGTH 64

char device[MAX_DEVICE_LENGTH] = "/dev/x400p-sl";

char modules[FMNAMESZ][8] = { "", };

struct gppa_spec {
	int type;
	int card;
	int span;
	int slot;
};

struct gppa_spec gobal = { 1, 1, 1, 1 };
struct gppa_spec ppa = { 0, 1, 1, 1 };

struct key_val {
	char *key;
	long val;
};

static struct key_val type_names[] = {
	{"none", SDL_TYPE_NONE},
	{"v35", SDL_TYPE_V35},
	{"ds0", SDL_TYPE_DS0},
	{"ds0a", SDL_TYPE_DS0A},
	{"e1", SDL_TYPE_E1},
	{"t1", SDL_TYPE_T1},
	{"j1", SDL_TYPE_J1},
	{"atm", SDL_TYPE_ATM},
	{"packet", SDL_TYPE_PACKET},
	{NULL, 0}
};

static struct key_val rate_names[] = {
	{"none", SDL_RATE_NONE},
	{"ds0a", SDL_RATE_DS0A},
	{"ds0", SDL_RATE_DS0},
	{"t1", SDL_RATE_T1},
	{"j1", SDL_RATE_J1},
	{"e1", SDL_RATE_E1},
	{"t2", SDL_RATE_T2},
	{"e2", SDL_RATE_E2},
	{"e3", SDL_RATE_E3},
	{"t3", SDL_RATE_T3},
	{NULL, -1}
};

static struct key_val gtype_names[] = {
	{"none", SDL_GTYPE_NONE},
	{"t1", SDL_GTYPE_T1},
	{"e1", SDL_GTYPE_E1},
	{"j1", SDL_GTYPE_J1},
	{"atm", SDL_GTYPE_ATM},
	{"eth", SDL_GTYPE_ETH},
	{"ip", SDL_GTYPE_IP},
	{"udp", SDL_GTYPE_UDP},
	{"tcp", SDL_GTYPE_TCP},
	{"rtp", SDL_GTYPE_RTP},
	{"sctp", SDL_GTYPE_SCTP},
	{"t2", SDL_GTYPE_T2},
	{"e2", SDL_GTYPE_E2},
	{"e3", SDL_GTYPE_E3},
	{"t3", SDL_GTYPE_T3},
	{"oc3", SDL_GTYPE_OC3},
	{"oc12", SDL_GTYPE_OC12},
	{"oc48", SDL_GTYPE_OC48},
	{"oc192", SDL_GTYPE_OC192},
	{NULL, 0}
};

static struct key_val grate_names[] = {
	{"none", SDL_GRATE_NONE},
	{"t1", SDL_GRATE_T1},
	{"j1", SDL_GRATE_J1},
	{"e1", SDL_GRATE_E1},
	{"t2", SDL_GRATE_T2},
	{"e2", SDL_GRATE_E2},
	{"e3", SDL_GRATE_E3},
	{"t3", SDL_GRATE_T3},
	{"oc3", SDL_GRATE_OC3},
	{"oc12", SDL_GRATE_OC12},
	{"oc48", SDL_GRATE_OC48},
	{"oc192", SDL_GRATE_OC192},
	{NULL, -1}
};

static struct key_val mode_names[] = {
	{"none", SDL_MODE_NONE},
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
	{"test", SDL_MODE_TEST},
	{NULL, 0}
};

static struct key_val gmode_names[] = {
	{"none", SDL_GMODE_NONE},
	{"loc_lb", SDL_GMODE_LOC_LB},
	{"rem_lb", SDL_GMODE_REM_LB},
	{NULL, 0}
};

static struct key_val gcrc_names[] = {
	{"none", SDL_GCRC_NONE},
	{"crc4", SDL_GCRC_CRC4},
	{"crc5", SDL_GCRC_CRC5},
	{"crc6", SDL_GCRC_CRC6},
	{"crc6j", SDL_GCRC_CRC6J},
	{NULL, 0}
};

static struct key_val clock_names[] {
	{"none", SDL_CLOCK_NONE},
	{"int", SDL_CLOCK_INT},
	{"ext", SDL_CLOCK_EXT},
	{"loop", SDL_CLOCK_LOOP},
	{"master", SDL_CLOCK_MASTER},
	{"slave", SDL_CLOCK_SLAVE},
	{"dpll", SDL_CLOCK_DPLL},
	{"abr", SDL_CLOCK_ABR},
	{"shaper", SDL_CLOCK_SHAPER},
	{"tick", SDL_CLOCK_TICK},
	{NULL, 0}
};

static struct key_val coding_names[] {
	{"none", SDL_CODING_NONE},
	{"nrz", SDL_CODING_NRZ},
	{"nrzi", SDL_CODING_NRZI},
	{"ami", SDL_CODING_AMI},
	{"b6zs", SDL_CODING_B6ZS},
	{"b8zs", SDL_CODING_B8ZS},
	{"hdb3", SDL_CODING_HDB3},
	{"aal1", SDL_CODING_AAL1},
	{"aal2", SDL_CODING_AAL2},
	{"aal5", SDL_CODING_AAL5},
	{NULL, 0}
};

static struct key_val framing_names[] {
	{"none", SDL_FRAMING_NONE},
	{"ccs", SDL_FRAMING_CCS},
	{"cas", SDL_FRAMING_CAS},
	{"sf", SDL_FRAMING_SF},
	{"esf", SDL_FRAMING_ESF},
	{"d4", SDL_FRAMING_D4},
	{NULL, 0}
};

static struct key_val rxlevel_names[] {
	{NULL, -1}
};

static struct key_val txlevel_names[] {
	{"none", SDL_TXLEVEL_NONE},
	{"dsx-133ft", SDL_TXLEVEL_DSX_133FT},
	{"dsx-266ft", SDL_TXLEVEL_DSX_266FT},
	{"dsx-399ft", SDL_TXLEVEL_DSX_399FT},
	{"dsx-533ft", SDL_TXLEVEL_DSX_533FT},
	{"dsx-666ft", SDL_TXLEVEL_DSX_666FT},
	{"csu-0db", SDL_TXLEVEL_CSU_0DB},
	{"csu-8db", SDL_TXLEVEL_CSU_8DB},
	{"csu-16db", SDL_TXLEVEL_CSU_16DB},
	{"csu-23db", SDL_TXLEVEL_CSU_23DB},
	{"75ohm-nm", SDL_TXLEVEL_75OHM_NM},
	{"120ohm-nm", SDL_TXLEVEL_120OHM_NM},
	{"75ohm-pr", SDL_TXLEVEL_75OHM_PR},
	{"120ohm-pr", SDL_TXLEVEL_120OHM_PR},
	{"75ohm-hrl", SDL_TXLEVEL_75OHM_HRL},
	{"120ohm-hrl", SDL_TXLEVEL_120OHM_HRL},
	{"mon-0db", SDL_TXLEVEL_MON_0DB},
	{"mon-12db", SDL_TXLEVEL_MON_12DB},
	{"mon-20db", SDL_TXLEVEL_MON_20DB},
	{"mon-26db", SDL_TXLEVEL_MON_26DB},
	{"mon-30db", SDL_TXLEVEL_MON_30DB},
	{"mon-32db", SDL_TXLEVEL_MON_32DB},
	{NULL, 0}
};


struct parm_tab {
	char *name;
	struct key_val *tab;
	sdl_ulong *valp;
};

static struct parm_tab parm_table[] = {
	{"type", &type_names, &config.sdl.iftype},
	{"rate", &rate_names, &config.sdl.ifrate},
	{"gtype", &gtype_names, &config.sdl.ifgtype},
	{"grate", &grate_names, &config.sdl.ifgrate},
	{"mode", &mode_names, &config.sdl.ifmode},
	{"gmode", &gmode_names, &config.sdl.ifgmode},
	{"gcrc", &gcrc_names, &config.sdl.ifgcrc},
	{"clock", &clock_names, &config.sdl.ifclock},
	{"coding", &coding_names, &config.sdl.ifcoding},
	{"framing", &framing_names, &config.sdl.ifframing},
	{"rxlevel", &rxlevel_names, &config.sdl.ifrxlevel},
	{"txlevel", &txlevel_names, &config.sdl.iftxlevel},
	{NULL, 0}
};

struct parm_val {
	char *name;
	uint32_t *valp;
};

static struct parm_tab pval_table[] = {
	{"t8", &config.sdt.t8},
	{"Tin", &config.sdt.Tin},
	{"Tie", &config.sdt.Tie},
	{"T", &config.sdt.T},
	{"D", &config.sdt.D},
	{"Te", &config.sdt.Te},
	{"De", &config.sdt.De},
	{"Ue", &config.sdt.Ue},
	{"t1", &config.sl.t1},
	{"t2", &config.sl.t2},
	{"t2l", &config.sl.t2l},
	{"t2h", &config.sl.t2h},
	{"t3", &config.sl.t3},
	{"t4n", &config.sl.t4n},
	{"t4e", &config.sl.t4e},
	{"t5", &config.sl.t5},
	{"t6", &config.sl.t6},
	{"t7", &config.sl.t7},
	{"rb_abate", &config.sl.rb_abate},
	{"rb_accept", &config.sl.rb_accept},
	{"rb_discard", &config.sl.rb_discard},
	{"tb_abate_1", &config.sl.tb_abate_1},
	{"tb_onset_1", &config.sl.tb_onset_1},
	{"tb_discd_1", &config.sl.tb_discd_1},
	{"tb_abate_2", &config.sl.tb_abate_2},
	{"tb_onset_2", &config.sl.tb_onset_2},
	{"tb_discd_2", &config.sl.tb_discd_2},
	{"tb_abate_3", &config.sl.tb_abate_3},
	{"tb_onset_3", &config.sl.tb_onset_3},
	{"tb_discd_3", &config.sl.tb_discd_3},
	{"N1", &config.sl.N1},
	{"N2", &config.sl.N2},
	{"M", &config.sl.M},
	{NULL, 0}
};

/**
 * x400pconf_get: - * get and display signalling link settings
 * @arg: number of CLEI or PPAs to get
 * @argv: the CLEIs or PPAs
 */
void
x400pconf_get(int argc, char *argv[])
{
	if (argc == 0) {
		/* get all signalling links */
	} else {
		int i;

		/* get specific signalling links */
		for (i = 0; i < argc; i++) {
			char *label = argv[i];

			/* label either has : or not */
			if (strpbrk(label, ":")) {
				char *token, *end;
				long val;

				/* label is a global PPA */
				/* Global PPAs are formatted as x400p:card:span:slot */
				if ((token = strsep(&label, ":")) == NULL)
					goto bad_ppa;
				if (strcasecmp(token, "x400p"))
					goto bad_ppa;
				if ((token = strsep(&label, ":")) == NULL)
					goto bad_ppa;
				val = strtol(token, &end, 0);
				if (end == token || *end != '\0')
					goto bad_ppa;
				if (1 > val || val > 8)
					goto bad_ppa;
				ppa.card = val;
				if ((token = strsep(&label, ":")) == NULL)
					goto bad_ppa;
				val = strtol(token, &end, 0);
				if (end == token || *end != '\0')
					goto bad_ppa;
				if (1 > val || val > 4)
					goto bad_ppa;
				ppa.span = val;
				if ((token = strsep(&label, ":")) == NULL || label != NULL)
					goto bad_ppa;
				val = strtol(token, &end, 0);
				if (end == token || *end != '\0')
					goto bad_ppa;
				if (1 > val || val > 31)
					goto bad_ppa;
				ppa.slot = val;
			} else {
				/* label is a CLEI */
			}
		}
	}
	return;
      bad_ppa:
	fprintf(stderr, "%s: bad global PPA format\n", my_argv[0]);
	exit(2);
}

/* change signalling link settings */
void
x400pconf_set(int argc, char *argv[])
{
	if (argc == 0) {
		/* set all signalling links */
	} else {
		int i;

		/* set specific signalling links */
		for (i = 0; i < argc; i++) {
			char *label = argv[i];

			/* label either has : or not */
			if (strpbrk(label, ":")) {
				char *token, *end;
				long val;

				/* label is a global PPA */
				/* Global PPAs are formatted as x400p:card:span:slot */
				if ((token = strsep(&label, ":")) == NULL)
					goto bad_ppa;
				if (strcasecmp(token, "x400p"))
					goto bad_ppa;
				if ((token = strsep(&label, ":")) == NULL)
					goto bad_ppa;
				val = strtol(token, &end, 0);
				if (end == token || *end != '\0')
					goto bad_ppa;
				if (1 > val || val > 8)
					goto bad_ppa;
				ppa.card = val;
				if ((token = strsep(&label, ":")) == NULL)
					goto bad_ppa;
				val = strtol(token, &end, 0);
				if (end == token || *end != '\0')
					goto bad_ppa;
				if (1 > val || val > 4)
					goto bad_ppa;
				ppa.span = val;
				if ((token = strsep(&label, ":")) == NULL || label != NULL)
					goto bad_ppa;
				val = strtol(token, &end, 0);
				if (end == token || *end != '\0')
					goto bad_ppa;
				if (1 > val || val > 31)
					goto bad_ppa;
				ppa.slot = val;
			} else {
				/* label is a CLEI */
			}
		}
	}
      bad_ppa:
	fprintf(stderr, "%s: bad global PPA format\n", my_argv[0]);
	exit(2);
}

/* add signalling link */
/*
 *  Add fields are as follows:
 *
 *  clei global-ppa profile options device-type device modules card span slot overrides
 */
void
x400pconf_add(int argc, char *argv[])
{
	int optind = 0, index;
	char *token, *end, *optarg, *tuple;
	long val;

	if (argc != 11)
		goto bad_opt;
	/* CLEI */
	optarg = argv[optind++];
	if (strlen(optarg) < sizeof(clei) - 1)
		goto syntax_error;
	strncpy(clei, optarg, sizeof(clei));
	/* Global PPA */
	optarg = argv[optind++];
	if ((token = strsep(&optarg, ":")) == NULL)
		goto syntax_error;
	if (strcasecmp(token, "x400p"))
		goto syntax_error;
	if ((token = strsep(&optarg, ":")) == NULL)
		goto syntax_error;
	val = strtol(token, &end, 0);
	if (end == token || *end != '\0')
		goto syntax_error;
	if (1 > val || val > 8)
		goto syntax_error;
	ppa.card = val;
	if ((token = strsep(&optarg, ":")) == NULL)
		goto syntax_error;
	val = strtol(token, &end, 0);
	if (end == token || *end != '\0')
		goto syntax_error;
	if (1 > val || val > 4)
		goto syntax_error;
	ppa.span = val;
	if ((token = strsep(&optarg, ":")) == NULL || optarg != NULL)
		goto syntax_error;
	val = strtol(token, &end, 0);
	if (end == token || *end != '\0')
		goto syntax_error;
	if (1 > val || val > 31)
		goto syntax_error;
	ppa.slot = val;
	/* Profile */
	optarg = argv[optind++];
	/* variant component */
	if ((token = strsep(&optarg, "-")) == NULL)
		goto syntax_error;
	if (!strcasecmp(token, "itut")) {
		pvar = SS7_PVAR_ITUT;
	} else if (!strcasecmp(token, "etsi")) {
		pvar = SS7_PVAR_ETSI;
	} else if (!strcasecmp(token, "ansi")) {
		pvar = SS7_PVAR_ANSI;
	} else if (!strcasecmp(token, "jttc")) {
		pvar = SS7_PVAR_JTTC;
	} else if (!strcasecmp(token, "chin")) {
		pvar = SS7_PVAR_CHIN;
	} else if (!strcasecmp(token, "sing")) {
		pvar = SS7_PVAR_SING;
	} else if (!strcasecmp(token, "span")) {
		pvar = SS7_PVAR_SPAN;
	} else
		goto syntax_error;
	/* year component */
	if (optarg == NULL)
		pvar |= SS7_PVAR_04;
	else {
		if ((token = strsep(&optarg, "-")) == NULL || optarg != NULL)
			goto syntax_error;
		val = strtol(token, &end, 0);
		if (end == token || *end != '\0')
			goto syntax_error;
		if (1988 <= val && val < 1992)
			pvar |= SS7_PVAR_1988;
		else if (1992 <= val && val < 1996)
			pvar |= SS7_PVAR_1992;
		else if (1996 <= val && val < 2000)
			pvar |= SS7_PVAR_1996;
		else if (2000 <= val && val < 2004)
			pvar |= SS7_PVAR_2000;
		else if (2004 <= val && val < 2008)
			pvar |= SS7_PVAR_2004;
		else
			goto syntax_error;
	}
	/* Options */
	optarg = argv[optind++];
	popt = 0;
	while ((token = strsep(&optarg, ","))) {
		if (!strcasecmp(token, "none"))
			continue;
		if (!strcasecmp(token, "mplev")) {
			popt |= SS7_POPT_MPLEV;
			continue;
		}
		if (!strcasecmp(token, "pcr")) {
			popt |= SS7_POPT_PCR;
			continue;
		}
		if (!strcasecmp(token, "hsl")) {
			popt |= SS7_POPT_HSL;
			continue;
		}
		if (!strcasecmp(token, "xsn")) {
			popt |= SS7_POPT_XSN;
			continue;
		}
		if (!strcasecmp(token, "nopr")) {
			popt |= SS7_POPT_NOPR;
			continue;
		}
		if (!strcasecmp(token, "all")) {
			popt |= SS7_POPT_ALL;
			continue;
		}
	}
	/* Device type */
	optarg = argv[optind++];
	if (strcasecmp(optarg, "x400p"))
		goto syntax_error;
	/* device */
	optarg = argv[optind++];
	if (strlen(optarg) > sizeof(device) - 1)
		goto syntax_error;
	strncpy(device, optarg, sizeof(device));
	/* modules */
	optarg = argv[optind++];
	for (index = 0; index < 8 && (token = strsep(&optarg, ",")); index++) {
		if (strlen(token) > FMNAMESZ)
			goto syntax_error;
		strncpy(modules[index], token, sizeof(modules[index]));
	}
	/* card */
	optarg = argv[optind++];
	val = strtol(optarg, &end, 0);
	if (end == optarg || *end != '\0')
		goto syntax_error;
	if (1 > val || val > 8)
		goto syntax_error;
	ppa.card = val;
	/* span */
	optarg = argv[optind++];
	val = strtol(optarg, &end, 0);
	if (end == optarg || *end != '\0')
		goto syntax_error;
	if (1 > val || val > 4)
		goto syntax_error;
	ppa.span = val;
	/* slot (use slot 0 for all slots) */
	optarg = argv[optind++];
	val = strtol(optarg, &end, 0);
	if (end == optarg || *end != '\0')
		goto syntax_error;
	if (0 > val || val > 31)
		goto syntax_error;
	ppa.slot = val;
	/* overrides */
	optarg = argv[optind++];
	/* this is a comma-separated list of parameter=value overrides */
	while ((tuple = strsep(&optarg, ","))) {
		struct parm_tab *pt;
		struct key_val *kv;
		struct parm_val *pv;

		if ((token = strsep(&tuple, "=")) == NULL || tuple == NULL)
			goto syntax_error;

		for (pt = parm_table; pt->name; pt++) {
			if (!strcasecmp(token, pt->name)) {
				for (kv = pt->tab; kv->key; kv++) {
					if (!strcasecmp(tuple, kv->key)) {
						*pt->valp = kv->val;
						break;
					}
				}
				if (kv->key != NULL)
					break;
				if (kv->val == 0)
					goto syntax_error;
				if ((val = strtol(tuple, &end, 0)) < 0)
					goto syntax_error;
				if (end == tuple || *end != '\0')
					goto syntax_error;
				*pt->valp = val;
				break;
			}

		}
		if (pt->name != NULL)
			continue;
		for (pv = pval_table; pv->name; pv++) {
			if (!strcasecmp(token, pv->name)) {
				if ((val = strtol(tuple, &end, 0)) < 0)
					goto syntax_error;
				if (end == typle || *end != '\0')
					goto syntax_error;
				*pv->valp = val;
				break;
			}
		}
		if (pv->name != NULL)
			continue;
		if (!strcasecmp(token, "syncsrc")) {
			for (index = 0; index < SDL_SYNCS; index++) {
				if ((token = strsep(tuple, ":"))) {
					if ((val = strtol(token, &end)) < 0)
						goto syntax_error;
					if (0 > val || val > 4)
						goto syntax_error;
					config.sdl.ifsyncsrc[index] = val;
				} else {
					config.sdl.ifsyncsrc[index] = 0;
				}
			}
			continue;
		}
		goto syntax_error;
	}
	/* Done parsing */
	return;
      syntax_error:
	optind--;
      bad_opt:
	fprintf(stderr, "%s: bad options format:", my_argv[0]);
	for (; optind < argc; optind++)
		fprintf(stderr, " %s", argv[optind]);
	fprintf(stderr, "\n");
	fflush(stderr);
	exit(2);
}

/* remove signalling link */
void
x400pconf_del(int argc, char *argv[])
{
}

/* interactive */
void
x400pconf_int(int argc, char *argv[])
{
}

/* configure from file */
void
x400pconf_cfg(int argc, char *argv[])
{
}

void
copying(void)
{
	if (!output)
		return;
	fprintf(stdout, "\
\n\
%1$s\n\
\n\
Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>\n\
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
General Public License (GPL) Version 3,  so long as the  software is distributed\n\
with, and only used for management of, OpenSS7 modules, drivers, and libraries.\n\
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
	", ident);
};

void
version(void)
{
	if (!output)
		return;
	fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
Written by Brian Bidulock.\n\
\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007  OpenSS7 Corporation\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 Corporation under GNU General Public License Version 3,\n\
incorporated herein by reference.  See `%1$s --copying' for copying permission.\n\
", "slconf", PACKAGE, VERSION, "$Revision$ $Date$");
}

void
usage(void)
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
", my_argv[0]);
}

void
help(void)
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
", my_argv[0], cfgfile, devname, card, span, slot, clock_names[config.ifclock].key, config.ifrate, type_names[config.iftype].key, mode_names[config.ifmode].key, config.ifgrate, gtype_names[config.ifgtype].key, gmode_names[config.ifgmode].key, gcrc_names[config.ifgcrc].key, coding_names[config.ifcoding].key, framing_names[config.ifframing].key);
}

int
main(int argc, char *argv[])
{
	int c;
	int val;
	char *optstr, *opttok;
	enum {
		COMMAND_NONE = 0,
		COMMAND_GET,
		COMMAND_SET,
		COMMAND_ADD,
		COMMAND_DEL,
		COMMAND_INT,
		COMMAND_CFG,
	} cmd = COMMAND_NONE;

	my_argc = argc;
	my_argv = argv;

	while (1) {
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"get",		no_argument,		NULL, 'g'},
			{"set",		no_argument,		NULL, 's'},
			{"remove",	no_argument,		NULL, 'r'},
			{"add",		no_argument,		NULL, 'a'},
			{"cha",		no_argument,		NULL, 's'},
			{"del",		no_argument,		NULL, 'r'},
			{"interactive",	no_argument,		NULL, 'i'},
			{"file",	optional_argument,	NULL, 'f'},
			{"clei",	required_argument,	NULL, 'l'},
			{"ppa",		required_argument,	NULL, 'p'},
			{"device",	required_argument,	NULL, 'd'},
			{"card",	required_argument,	NULL, 'c'},
			{"span",	required_argument,	NULL, 'n'},
			{"slot",	required_argument,	NULL, 't'},

			{"debug",	optional_argument,	NULL, 'D'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'h'},
			{NULL,		0,			NULL,  0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "gsif::l:p:d:c:n:t:D::qv::hVC?", long_options,
				&option_index);
		if (c == -1)
			break;
		switch (c) {
		case 0:
		case 'g':	/* -g, --get */
			if (cmd != COMMAND_NONE)
				goto bad_option;
			cmd = COMMAND_GET;
			break;
		case 's':	/* -s, --set, --cha */
			if (cmd != COMMAND_NONE)
				goto bad_option;
			cmd = COMMAND_SET;
			break;
		case 'r':	/* -r, --remove, --del */
			if (cmd != COMMAND_NONE)
				goto bad_option;
			cmd = COMMAND_DEL;
			break;
		case 'a':	/* -a, --add */
			if (cmd != COMMAND_NONE)
				goto bad_option;
			cmd = COMMAND_ADD;
			break;
		case 'i':	/* -i, --interactive */
			if (cmd != COMMAND_NONE)
				goto bad_option;
			cmd = COMMAND_INT;
			break;
		case 'f':	/* -f, --file [FILENAME] */
			if (cmd != COMMAND_NONE)
				goto bad_option;
			cmd = COMMAND_CFG;
			if (optarg != NULL)
				strncpy(filename, optarg, sizeof(filename));
			break;
		case 'l':	/* -l, --clei CLEI */
			if (optarg == NULL)
				goto bad_option;
			strncpy(clei, optarg, sizeof(clei));
			break;
		case 'p':	/* -p, --ppa PPA */
			/* Global PPAs are formatted as x400p:card:span:slot */
			if (optarg == NULL)
				goto bad_option;
			if ((opttok = strsep(&optarg, ":")) == NULL)
				goto bad_option;
			if (strcasecmp(optarg, "x400p"))
				goto bad_option;
			gobal.type = 1;
			if ((opttok = strsep(&optarg, ":")) == NULL)
				goto bad_option;
			val = strtol(opttok, &optstr, 0);
			if ((optstr == opttok || *optstr != '\0'))
				goto bad_option;
			if (1 > val || val > 8)
				goto bad_option;
			gobal.card = val;
			if ((opttok = strsep(&optarg, ":")) = NULL)
				goto bad_option;
			val = strtol(opttok, &optstr, 0);
			if ((optstr = opttok || *opstr != '\0'))
				goto bad_option;
			if (1 > val || val > 4)
				goto bad_option;
			gobal.span = val;
			if ((opttok = strsep(&optarg, ":")) = NULL)
				goto bad_option;
			val = strtol(opttok, &optstr, 0);
			if ((optstr = opttok || *opstr != '\0'))
				goto bad_option;
			if (1 > val || val > 31)
				goto bad_option;
			gobal.slot = val;
			break;
		case 'd':	/* -d, --device DEVICE */
			if (optarg == NULL)
				goto bad_option;
			break;
		case 'c':	/* -c, --card CARD */
			if (optarg == NULL)
				goto bad_option;
			val = strtol(opttok, &optstr, 0);
			if ((optstr = opttok || *opstr != '\0'))
				goto bad_option;
			if (1 > val || val > 8)
				goto bad_option;
			ppal.card = val;
			break;
		case 'n':	/* -n, --span SPAN */
			if (optarg == NULL)
				goto bad_option;
			val = strtol(opttok, &optstr, 0);
			if ((optstr = opttok || *opstr != '\0'))
				goto bad_option;
			if (1 > val || val > 4)
				goto bad_option;
			ppal.span = val;
			break;
		case 't':	/* -t, --slot SLOT */
			if (optarg == NULL)
				goto bad_option;
			val = strtol(opttok, &optstr, 0);
			if ((optstr = opttok || *opstr != '\0'))
				goto bad_option;
			if (1 > val || val > 31)
				goto bad_option;
			ppal.slot = val;
			break;
		case 'D':	/* -D, --debug [LEVEL] */
			if ((optstr = optarg) == NULL) {
				debug += 1;
				break;
			}
			val = strtol(optarg, &optstr, 0);
			if (optstr == optarg || *optstr != '\0' || val < 0)
				goto bad_option;
			debug = val;
			break;
		case 'q':	/* -q, --quiet */
			output -= output > 1 ? 1 : output;
			break;
		case 'v':	/* -v, --verbose [LEVEL] */
			if ((optstr = optarg) == NULL) {
				output += 1;
				break;
			}
			val = strtol(optarg, &optstr, 0);
			if (optstr == optarg || *optstr != '\0' || val < 0)
				goto bad_option;
			output = val;
			break;
		case 'h':	/* -h, -?, --?, --help */
			if (cmd != COMMAND_NONE)
				goto bad_option;
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			if (cmd != COMMAND_NONE)
				goto bad_option;
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copying */
			if (cmd != COMMAND_NONE)
				goto bad_option;
			version(argc, argv);
			exit(0);
		case ':':
			optind--;
			fprintf(stderr, "%s: missing parm -- %s\n", argv[0], argv[optind]);
			usage(argc, argv);
			exit(2);
		case '?':
		default:
		      bad_option:
			optind--;
		      syntax_error:
			if (optind < argc) {
				fprintf(stderr, "%s: illegal syntax --", argv[0]);
				for (; optind < argc; optind++)
					fprintf(stderr, " %s", argv[optind]);
				fprintf(stderr, "\n");
			}
			usage(argc, argv);
			exit(2);

		}
	}
      reevaluate:
	switch (cmd) {
	case COMMAND_NONE:
		if (optind < argc) {
			optarg = argv[optind++];
			if (!strcasecmp(optarg, "add")) {
				cmd = COMMAND_ADD;
				goto reevaluate;
			} else if (!strcasecmp(optarg, "get")) {
				cmd = COMMAND_GET;
				goto reevaluate;
			} else if (!strcasecmp(optarg, "set") || !strcasecmp(optarg, "cha")) {
				cmd = COMMAND_SET;
				goto reevaluate;
			} else if (!strcasecmp(optarg, "del") || !strcasecmp(optarg, "remove")) {
				cmd = COMMAND_DEL;
				goto reevaluate;
			}
		}
		goto syntax_error;
	case COMMAND_GET:
		x400pconf_get(argc - optind, (optind < argc) ? &argv[optind] : NULL);
		break;
	case COMMAND_SET:
		x400pconf_set(argc - optind, (optind < argc) ? &argv[optind] : NULL);
		break;
	case COMMAND_ADD:
		x400pconf_add(argc - optind, (optind < argc) ? &argv[optind] : NULL);
		break;
	case COMMAND_DEL:
		x400pconf_del(argc - optind, (optind < argc) ? &argv[optind] : NULL);
		break;
	case COMMAND_INT:
		x400pconf_int(argc - optind, (optind < argc) ? &argv[optind] : NULL);
		break;
	case COMMAND_CFG:
		x400pconf_cfg(argc - optind, (optind < argc) ? &argv[optind] : NULL);
		break;
	default:
		break;
	}
	exit(4);
}
