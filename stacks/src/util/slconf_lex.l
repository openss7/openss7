/*****************************************************************************

 @(#) $RCSfile: slconf_lex.l,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-05-04 16:16:37 $

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

 Last Modified $Date: 2008-05-04 16:16:37 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: slconf_lex.l,v $
 Revision 0.9.2.6  2008-05-04 16:16:37  brian
 - changes and corrections for release

 Revision 0.9.2.5  2008-04-29 07:11:33  brian
 - updating headers for release

 Revision 0.9.2.4  2008-04-28 07:41:09  brian
 - updates for release

 Revision 0.9.2.3  2008-04-10 09:18:20  brian
 - corrections for compile

 Revision 0.9.2.2  2007/08/19 11:24:03  brian
 - flex version nuances

 Revision 0.9.2.1  2007/08/18 03:53:15  brian
 - working up configuration files

 *****************************************************************************/

/* C DECLARATIONS */

%{

#ident "@(#) $RCSfile: slconf_lex.l,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-05-04 16:16:37 $"

static char const ident[] = "$RCSfile: slconf_lex.l,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-05-04 16:16:37 $";

#include <unistd.h> /* isatty(), official flex 2.5.4a skeleton misses this */

#include <stdlib.h>
#include <stropts.h>
#include <string.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

#include "slconf_yac.h"

#define ECHO fputs(yytext, yyout)

#ifndef YY_FLEX_SUBMINOR_VERSION
#define FIND_RULE goto find_rule
#else
#define FIND_RULE while (0) { }
#endif

#if 0
static int
stdgrp_val(char *text)
{
    int pvar;

    if (0) {
	pvar = -1;
    } else if (!strncasecmp(text, "itut", 4)) {
	pvar = SS7_PVAR_ITUT;
    } else if (!strncasecmp(text, "ansi", 4)) {
	pvar = SS7_PVAR_ANSI;
    } else if (!strncasecmp(text, "etsi", 4)) {
	pvar = SS7_PVAR_ETSI;
    } else if (!strncasecmp(text, "jttc", 4)) {
	pvar = SS7_PVAR_JTTC;
    } else if (!strncasecmp(text, "chin", 4)) {
	pvar = SS7_PVAR_CHIN;
    } else if (!strncasecmp(text, "sing", 4)) {
	pvar = SS7_PVAR_SING;
    } else if (!strncasecmp(text, "span", 4)) {
	pvar = SS7_PVAR_SPAN;
    } else {
	pvar = -1;
    }
    return (pvar);
}
#endif

#if 0
static int
stdyr_val(char *text)
{
    int val, year = -1;
    char *end;

    val = strtol(text, &end, 0);
    if (end != text && *end == '\0') {
	if (val >= 1988)
	    if ((year = (val - 1988) / 4) > 254)
		year = -1;
    }
    return (year);
}
#endif

#if 0
static int
stdvar_val(char *text)
{
    int pvar, year;

    pvar = stdgrp_val(text);
    year = stdyr_val(&text[5]);
    if (pvar != -1 && year != -1)
	pvar |= year;
    return (pvar);
}
#endif

#if 0
static int
dt_val(char *text, int len)
{
    int type = -1;

    if (len == 4 && !strncasecmp(text, "none", len))
	type = 0;
    if (len == 5 && !strncasecmp(text, "x400p", len))
	type = 1;
    if (len == 4 && !strncasecmp(text, "m2pa", len))
	type = 2;
    if (len == 4 && !strncasecmp(text, "m2ua", len))
	type = 3;
    if (len == 3 && !strncasecmp(text, "ppa", len))
	type = 4;
    if (len == 4 && !strncasecmp(text, "pipe", len))
	type = 5;
    return (type);
}
#endif

#if 0
static int
ppa_val(char *text)
{
    char *beg, *end;
    int val, ppa = 0;

    beg = end = text;
    val = strtol(beg, &end, 0);
    if (end == beg || *end != ':')
	goto bad_ppa;
    if (1 > val || val > 8)
	goto bad_ppa;
    ppa |= (val << 16);
    beg = ++end;
    val = strtol(beg, &end, 0);
    if (end == beg || *end != ':')
	goto bad_ppa;
    if (1 > val || val > 4)
	goto bad_ppa;
    ppa |= (val << 8);
    beg = ++end;
    val = strtol(beg, &end, 0);
    if (end == beg || *end != '\0')
	goto bad_ppa;
    if (0 > val || val > 31)
	goto bad_ppa;
    ppa |= (val << 0);
    return (ppa);
  bad_ppa:
    return (-1);
}
#endif

#if 0
static int
gppa_val(char *text)
{
    int gppa = -1, ppa;
    char *beg;

    if ((beg = strpbrk(text, ":")))
	if ((gppa = dt_val(text, beg - text)) != -1)
	    if ((ppa = ppa_val(beg + 1)) != -1)
		gppa = (gppa << 24) | ppa;
    return (gppa);
}
#endif

static int
pc_val(char *text)
{
    char *beg, *end;
    int val, pc = 0;

    beg = end = text;
    val = strtol(beg, &end, 0);
    if (end == beg || *end != '-')
	goto bad_pc;
    if (0 > val || val > 255)
	goto bad_pc;
    pc |= (val << 16);
    val = strtol(beg, &end, 0);
    if (end == beg || *end != '-')
	goto bad_pc;
    if (0 > val || val > 255)
	goto bad_pc;
    pc |= (val << 8);
    val = strtol(beg, &end, 0);
    if (end == beg || *end != '\0')
	goto bad_pc;
    if (0 > val || val > 255)
	goto bad_pc;
    pc |= (val << 0);
    return (pc);
  bad_pc:
    return (-1);
}

%}

/* FLEX DECLARATIONS */

%option nolex-compat
%option warn
/* %option debug */
/* %option verbose */
/* %option perf-report */
%option default
%option caseless
%option 7bit
%option interactive
%option noalways-interactive
%option nonever-interactive
%option nostdinit
%option nostack
%option nomain
%option noyywrap
%option yylineno

POINTCODE	[0-9]+"-"[0-9]+"-"[0-9]+

NUMBER		[1-9][0-9]*
PPA		{NUMBER}":"{NUMBER}(":"{NUMBER})?
DEVICETYPE	("none"|"x400p"|"m2pa"|"m2ua"|"ppa")
GPPA		({DEVICETYPE})":"({PPA})
STDYEAR		("19"[0-9][0-9])|("20"[0-9][0-9])
STDGRP		"itut"|"etsi"|"ansi"|"jttc"|"chin"|"sing"|"span"
PVAR		({STDGRP})"-"({STDYEAR})


DF		"default"|"df"
MT		"mtp"|"mt"
NA		"network-appearance"|"na"
SP		"signal[l]?ing-point"|"sp"
RS		"route-set"|"rs"
RL		"route-list"|"rl"
RT		"route"|"rt"
LS		"combined-linkset"|"ls"
LK		"linkset"|"lk"
SL		"signal[l]?ing-link"|"link"|"sl"
CB		"changeover-buffer"|"changeback-buffer"|"cb"
CR		"controlled-rerouting-buffer"|"cr"
SDT		"signal[l]?ing-(data-)?terminal"|"sdt"
SDL		"signal[l]?ing-data-link"|"sdl"

MPLEV		"mplev"|"multiple-priority-levels"
PCR		"pcr"|"preventative-cyclic-retransmission"
HSL		"hsl"|"high-speed-link"
XSN		"xsn"|"extended-sequence-numbers"
NOPR		"nopr"|"no-proving"
TFR		"tfr"|"tfr-normal"
TFRB		"tfr"|"tfr-broadcast"
TFRR		"tfr"|"tfr-regulated"
MCSTA		"mcsta"|"multiple-congestion-states"
XFER		"xfer"|"transfer-function"
LOSCA		"osca"|"link-oscillation-procedure-a"
LOSCB		"osca"|"link-oscillation-procedure-b"
NI		"ni"|"network-indicator"
SLC		"slc"|"signal[l]?ing-link-code"

OPTION		(({MPLEV})|({PCR})|({HSL})|({XSN})|({NOPR}))
OPTIONS		({OPTION})(","{OPTION})*

%%

"\\\n"			/* absorb continuation markers */

"\n"			{ return TOK_EOL	; }

"usage"|"u"		{ return TOK_USAGE	; }
"help"|"h"|"?"		{ return TOK_HELP	; }
"version"|"v"		{ return TOK_VERSION	; }
"copying"|"c"		{ return TOK_COPYING	; }
"quit"|"q"		{ return TOK_QUIT	; }

"open"			{ return TOK_OPEN	; }
"close"			{ return TOK_CLOSE	; }
"attach"		{ return TOK_ATTACH	; }
"detach"		{ return TOK_DETACH	; }
"enable"		{ return TOK_ENABLE	; }
"disable"		{ return TOK_DISABLE	; }
"start"			{ return TOK_START	; }
"emergency"|"emerg"	{ return TOK_EMERGENCY	; }
"normal"|"norm"		{ return TOK_NORMAL	; }
"stop"			{ return TOK_STOP	; }
"msu"			{ return TOK_MSU	; }
"monitor"		{ return TOK_MONITOR	; }

"options"		{ return TOK_OPTIONS	; }
"config"|"configuration" { return TOK_CONFIG	; }
"stats"|"statistics"	{ return TOK_STATS	; }
"statsp"|"periods"	{ return TOK_STATSP	; }
"notify"		{ return TOK_NOTIFY	; }

"get"			{ return TOK_GET	; }
"show"			{ return TOK_SHOW	; }
"set"			{ return TOK_SET	; }
"clear"			{ return TOK_CLEAR	; }
"reset"			{ return TOK_RESET	; }
"add"			{ return TOK_ADD	; }
"cha"|"change"		{ return TOK_CHA	; }
"del"			{ return TOK_DEL	; }
"remove"		{ return TOK_REMOVE	; }
"select"		{ return TOK_SELECT	; }
"status"		{ return TOK_STATUS	; }
"debug"			{ return TOK_DEBUG	; }

{DF}			{ return TOK_DF		; }
{MT}			{ return TOK_MT		; }
{NA}			{ return TOK_NA		; }
{SP}			{ return TOK_SP		; }
{RS}			{ return TOK_RS		; }
{RL}			{ return TOK_RL		; }
{RT}			{ return TOK_RT		; }
{LS}			{ return TOK_LS		; }
{LK}			{ return TOK_LK		; }
{SL}			{ return TOK_SL		; }
{CB}			{ return TOK_CB		; }
{CR}			{ return TOK_CR		; }
{SDT}			{ return TOK_SDT	; }
{SDL}			{ return TOK_SDL	; }

"all"			{ return TOK_ALL	; }
"none"			{ return TOK_NONE	; }
"x400p"			{ return TOK_X400P	; }
"m2pa"			{ return TOK_M2PA	; }
"m2ua"			{ return TOK_M2UA	; }
"ppa"			{ return TOK_PPATYPE	; }
"pipe"			{ return TOK_PIPE	; }

"clli"			{ return TOK_CLLI	; }

"itut"|"itu-t"|"itu"	{ return TOK_ITUT	; }
"etsi"			{ return TOK_ETSI	; }
"ansi"			{ return TOK_ANSI	; }
"jttc"|"ttc"|"japan"	{ return TOK_JTTC	; }
"chin"|"china"		{ return TOK_CHIN	; }

{MPLEV}			{ yylval.val = SS7_POPT_MPLEV;	return TOK_MPLEV	; }
{PCR}			{ yylval.val = SS7_POPT_PCR;	return TOK_PCR		; }
{HSL}			{ yylval.val = SS7_POPT_HSL;	return TOK_HSL		; }
{XSN}			{ yylval.val = SS7_POPT_XSN;	return TOK_XSN		; }
{NOPR}			{ yylval.val = SS7_POPT_NOPR;	return TOK_NOPR		; }
{TFR}			{ return TOK_TFR	; }
{TFRB}			{ return TOK_TFRB	; }
{TFRR}			{ return TOK_TFRR	; }
{MCSTA}			{ return TOK_MCSTA	; }

"member"		{ return TOK_MEMBER	; }
"cluster"		{ return TOK_CLUSTER	; }
"network"		{ return TOK_NETWORK	; }
{XFER}			{ return TOK_XFER	; }
"security"		{ return TOK_SECURITY	; }
{LOSCA}			{ return TOK_LOSCA	; }
{LOSCB}			{ return TOK_LOSCB	; }
"adj"|"adjacent"	{ return TOK_ADJACENT	; }
"device"		{ return TOK_DEVICE	; }
"card"			{ return TOK_CARD	; }
"slot"			{ return TOK_SLOT	; }
"span"			{ return TOK_SPAN	; }
{NI}			{ return TOK_NI		; }
"cost"			{ return TOK_COST	; }
"mask"			{ return TOK_MASK	; }
"dest"|"destination"	{ return TOK_DEST	; }
{SLC}			{ return TOK_SLC	; }

"allow"			{ return TOK_ALLOW	; }
"restrict"		{ return TOK_RESTRICT	; }
"prohibit"		{ return TOK_PROHIBIT	; }
"activate"		{ return TOK_ACTIVATE	; }
"deactivate"		{ return TOK_DEACTIVATE	; }
"block"			{ return TOK_BLOCK	; }
"unblock"		{ return TOK_UNBLOCK	; }
"inhibit"		{ return TOK_INHIBIT	; }
"uninhibit"		{ return TOK_UNINHIBIT	; }
"congest"		{ return TOK_CONGEST	; }
"uncongest"		{ return TOK_UNCONGEST	; }
"danger"		{ return TOK_DANGER	; }
"nodanger"		{ return TOK_NODANGER	; }
"restart"		{ return TOK_RESTART	; }
"restarted"		{ return TOK_RESTARTED	; }

"t1"			{ return TOK_T1T	; }
"t1r"			{ return TOK_T1RT	; }
"t1s"			{ return TOK_T1ST	; }
"t1t"			{ return TOK_T1TT	; }
"t2"			{ return TOK_T2T	; }
"t2h"			{ return TOK_T2HT	; }
"t2l"			{ return TOK_T2LT	; }
"t2t"			{ return TOK_T2TT	; }
"t3"			{ return TOK_T3T	; }
"t4"			{ return TOK_T4T	; }
"t4n"			{ return TOK_T4NT	; }
"t4e"			{ return TOK_T4ET	; }
"t5"			{ return TOK_T5T	; }
"t6"			{ return TOK_T6T	; }
"t7"			{ return TOK_T7T	; }
"t8"			{ return TOK_T8T	; }
"t10"			{ return TOK_T10T	; }
"t11"			{ return TOK_T11T	; }
"t12"			{ return TOK_T12T	; }
"t13"			{ return TOK_T13T	; }
"t14"			{ return TOK_T14T	; }
"t15"			{ return TOK_T15T	; }
"t16"			{ return TOK_T16T	; }
"t17"			{ return TOK_T17T	; }
"t18a"			{ return TOK_T18AT	; }
"t18"			{ return TOK_T18T	; }
"t19a"			{ return TOK_T19AT	; }
"t19"			{ return TOK_T19T	; }
"t20a"			{ return TOK_T20AT	; }
"t20"			{ return TOK_T20T	; }
"t21a"			{ return TOK_T21AT	; }
"t21"			{ return TOK_T21T	; }
"t22a"			{ return TOK_T22AT	; }
"t22"			{ return TOK_T22T	; }
"t23a"			{ return TOK_T23AT	; }
"t23"			{ return TOK_T23T	; }
"t24a"			{ return TOK_T24AT	; }
"t24"			{ return TOK_T24T	; }
"t25a"			{ return TOK_T25AT	; }
"t26a"			{ return TOK_T26AT	; }
"t27a"			{ return TOK_T27AT	; }
"t28a"			{ return TOK_T28AT	; }
"t29a"			{ return TOK_T29AT	; }
"t30a"			{ return TOK_T30AT	; }
"t31a"			{ return TOK_T31AT	; }
"t32a"			{ return TOK_T32AT	; }
"t33a"			{ return TOK_T33AT	; }
"t34a"			{ return TOK_T34AT	; }

"clock"			{ return TOK_CLOCK	; }
"internal"|"int"	{ return TOK_INT	; }
"external"|"ext"	{ return TOK_EXT	; }
"loop"			{ return TOK_LOOP	; }
"master"		{ return TOK_MASTER	; }
"slave"			{ return TOK_SLAVE	; }
"dpll"			{ return TOK_DPLL	; }
"abr"			{ return TOK_ABR	; }
"shaper"		{ return TOK_SHAPER	; }
"tick"			{ return TOK_TICK	; }

"rate"			{ return TOK_RATE	; }

"type"			{ return TOK_TYPE	; }
"v35"			{ return TOK_V35	; }
"ds0"			{ return TOK_DS0	; }
"ds0a"			{ return TOK_DS0A	; }
"e1"			{ return TOK_E1		; }
"j1"			{ return TOK_J1		; }
"atm"			{ return TOK_ATM	; }
"packet"		{ return TOK_PACKET	; }

"mode"			{ return TOK_MODE	; }
"dsu"			{ return TOK_DSU	; }
"csu"			{ return TOK_CSU	; }
"dte"			{ return TOK_DTE	; }
"dce"			{ return TOK_DCE	; }
"client"		{ return TOK_CLIENT	; }
"server"		{ return TOK_SERVER	; }
"peer"			{ return TOK_PEER	; }
"echo"			{ return TOK_ECHO	; }
"rem_lb"		{ return TOK_REM_LB	; }
"loc_lb"		{ return TOK_LOC_LB	; }
"lb_echo"		{ return TOK_LB_ECHO	; }
"test"			{ return TOK_TEST	; }

"grate"|"grouprate"	{ return TOK_GRATE	; }
"e2"			{ return TOK_E2		; }
"e3"			{ return TOK_E3		; }
"oc3"			{ return TOK_OC3	; }
"oc12"			{ return TOK_OC12	; }
"oc48"			{ return TOK_OC48	; }
"oc192"			{ return TOK_OC192	; }

"gtype"|"grouptype"	{ return TOK_GTYPE	; }
"eth"			{ return TOK_ETH	; }
"ip"			{ return TOK_IP		; }
"udp"			{ return TOK_UDP	; }
"tcp"			{ return TOK_TCP	; }
"rtp"			{ return TOK_RTP	; }
"sctp"			{ return TOK_SCTP	; }

"gmode"|"groupmode"	{ return TOK_GMODE	; }

"gcrc"|"groupcrc"|"crc"	{ return TOK_GCRC	; }
"crc4"			{ return TOK_CRC4	; }
"crc5"			{ return TOK_CRC5	; }
"crc6"			{ return TOK_CRC6	; }
"crc6j"			{ return TOK_CRC6J	; }

"coding"		{ return TOK_CODING	; }
"nrz"			{ return TOK_NRZ	; }
"nrzi"			{ return TOK_NRZI	; }
"ami"			{ return TOK_AMI	; }
"b6zs"			{ return TOK_B6ZS	; }
"b8zs"			{ return TOK_B8ZS	; }
"aal1"			{ return TOK_AAL1	; }
"aal2"			{ return TOK_AAL2	; }
"aal5"			{ return TOK_AAL5	; }
"hdb3"			{ return TOK_HDB3	; }

"framing"		{ return TOK_FRAMING	; }
"ccs"			{ return TOK_CCS	; }
"cas"			{ return TOK_CAS	; }
"sf"			{ return TOK_SF		; }
"esf"			{ return TOK_ESF	; }
"d4"			{ return TOK_D4		; }

"syncsrc"		{ return TOK_SYNCSRC	; }


"N1"			{ return TOK_N1		; }
"N2"			{ return TOK_N2		; }
"M"			{ return TOK_M		; }

"rb-abate"		{ return TOK_RB_ABATE	; }
"rb-accept"		{ return TOK_RB_ACCEPT	; }
"rb-discard"		{ return TOK_RB_DISCARD	; }

"tb-abate-1"		{ return TOK_TB_ABATE_1	; }
"tb-onset-1"		{ return TOK_TB_ONSET_1	; }
"tb-discard-1"		{ return TOK_TB_DISCD_1	; }
"tb-abate-2"		{ return TOK_TB_ABATE_2	; }
"tb-onset-2"		{ return TOK_TB_ONSET_2	; }
"tb-discard-2"		{ return TOK_TB_DISCD_2	; }
"tb-abate-3"		{ return TOK_TB_ABATE_3	; }
"tb-onset-3"		{ return TOK_TB_ONSET_3	; }
"tb-discard-3"		{ return TOK_TB_DISCD_3	; }

"Tin"			{ return TOK_TIN	; }
"Tie"			{ return TOK_TIE	; }
"T"			{ return TOK_T		; }
"D"			{ return TOK_D		; }
"Te"			{ return TOK_TE		; }
"De"			{ return TOK_DE		; }
"Ue"			{ return TOK_UE		; }

"N"			{ return TOK_N		; }
	/* "m"			{ return TOK_LCM	; } */

"fail-unspecified"	{ return TOK_FAIL_UNSPECIFIED	    ; }
"fail-cong-timeout"	{ return TOK_FAIL_CONG_TIMEOUT	    ; }
"fail-ack-timeout"	{ return TOK_FAIL_ACK_TIMEOUT	    ; }
"fail-abnormal-bsnr"	{ return TOK_FAIL_ABNORMAL_BSNR	    ; }
"fail-abnormal-fibr"	{ return TOK_FAIL_ABNORMAL_FIBR	    ; }
"fail-abnormal"		{ return TOK_FAIL_ABNORMAL	    ; }
"fail-suerm-eim"	{ return TOK_FAIL_SUERM_EIM	    ; }
"fail-alignment"	{ return TOK_FAIL_ALIGNMENT	    ; }
"fail-received-sio"	{ return TOK_FAIL_RECEIVED_SIO	    ; }
"fail-received-sin"	{ return TOK_FAIL_RECEIVED_SIN	    ; }
"fail-received-sie"	{ return TOK_FAIL_RECEIVED_SIE	    ; }
"fail-received-sios"	{ return TOK_FAIL_RECEIVED_SIOS	    ; }
"fail-t1-timeout"	{ return TOK_FAIL_T1_TIMEOUT	    ; }
"rpo-begin"		{ return TOK_RPO_BEGIN		    ; }
"rpo-end"		{ return TOK_RPO_END		    ; }
"congest-onset-ind"	{ return TOK_CONGEST_ONSET_IND	    ; }
"congest-discard-ind"	{ return TOK_CONGEST_DISCD_IND	    ; }

"tx-fail"		{ return TOK_TX_FAIL		    ; }
"rx-fail"		{ return TOK_RX_FAIL		    ; }
"carrier"		{ return TOK_CARRIER		    ; }
"su-error"		{ return TOK_SU_ERROR		    ; }
"lost-sync"		{ return TOK_LOST_SYNC		    ; }

			/* Note: might want to break these out into complex return value */
{POINTCODE}		{ yylval.val = pc_val(yytext);		return TOK_POINTCODE	; }
      /* {GPPA}			{ yylval.val = gppa_val(yytext);	return TOK_GPPA		; } */
      /* {PPA}			{ yylval.val = ppa_val(yytext);		return TOK_PPA		; } */
      /* {STDYEAR}		{ yylval.val = stdyr_val(yytext);	return TOK_YEAR		; } */
      /* {STDGRP}		{ yylval.val = stdgrp_val(yytext);	return TOK_STDGRP	; } */
      /* {PVAR}			{ yylval.val = stdvar_val(yytext);	return TOK_PVAR		; } */
      /* {OPTIONS}		{ yylval.val = opts_val(yytext);	return TOK_OPTIONS	; } */
      /* {DEVICETYPE}		{ yylval.val = dt_val(yytext,yyleng);	return TOK_DEVICETYPE	; } */

[;]			{ return TOK_SEMICOLON	; FIND_RULE; }
[:]			{ return TOK_COLON	; }
[=]			{ return TOK_EQUAL	; }
[,]			{ return TOK_COMMA	; }
[-]			{ return TOK_DASH	; }
[.]			{ return TOK_PERIOD	; }

"#"[^\n]*		{ yylval.str = yytext;			return TOK_COMMENT	; }
[ \t]+			/* { yylval.str = yytext;			return TOK_WHITESPACE	; } */

[0-9]+			{ yylval.val = atoi(yytext);		return TOK_NUMBER	; }
      /* [^ \t\n,=:\-]+		{ yylval.str = yytext;			return TOK_STRING	; } */
[a-zA-Z_/][a-zA-Z0-9_/\-.]*	{ yylval.str = yytext;			return TOK_STRING	; }

%%

/* ADDITIONAL CODE */

static void dummy(void) {
	(void) dummy;
	(void) yyunput;
#ifndef YY_FLEX_SUBMINOR_VERSION
	(void) yy_flex_realloc;
#endif
}

static YY_BUFFER_STATE buf = NULL;
static char *mem = NULL;

void
optlex_init(int argc, char *argv[], int optind)
{
	int size = 4096, len = 0;

	if ((mem = malloc(size)) == NULL) {
		perror(argv[0]);
		exit(1);
	}
	len += snprintf(mem + len, size - len - 1, "%s", argv[optind++]);
	while (optind < argc)
		len += snprintf(mem + len, size - len - 1, " %s", argv[optind++]);
	len += snprintf(mem + len, size - len - 1, "\n");
	len += snprintf(mem + len, size - len - 1, "%1$c%1$c", '\0');
	if ((buf = yy_scan_buffer(mem, len)) == NULL) {
		fprintf(stderr, "%s: cannot allocate scan buffer\n", argv[0]);
		exit(1);
	}
	return;
}

void
optlex_term(void)
{
	yy_delete_buffer(buf);
	free(mem);
}


/*
 * vim: ft=lex
 */
