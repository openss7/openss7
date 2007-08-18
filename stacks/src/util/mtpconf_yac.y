/*****************************************************************************

 @(#) $RCSfile: mtpconf_yac.y,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $

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

 $Log: mtpconf_yac.y,v $
 Revision 0.9.2.1  2007/08/18 03:53:15  brian
 - working up configuration files

 *****************************************************************************/

/* C DECLARATIONS */

%{

#ident "@(#) $RCSfile: mtpconf_yac.y,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $"

static char const ident[] = "$RCSfile: mtpconf_yac.y,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $";

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
#include <fcntl.h>
#include <getopt.h>
#include <time.h>
#include <signal.h>
#include <syslog.h>
#include <sys/utsname.h>

#include "mtpconf_yac.h"

#if !defined YYLTYPE && !defined YYLTYPE_IS_DECLARED
typedef int YYLTYPE;
#define yyltype YYLTYPE
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 1
#endif

#if !defined YYENABLE_NLS
#define YYENABLE_NLS 0
#endif

static char *prompt = "mtpconf";
static int interactive = 1;
static int output = 1;

void version(int, char*[]);
void copying(int, char*[]);
void usage(int, char*[]);
void help(int, char*[]);
void newline(void);
void helpon(YYSTYPE what);

int yydebug = 0;

extern int yylex(void);
int yyerror(const char *);

static char *filename = "stdin";
static int lineno = 0;
extern char *yytext;

extern FILE *yyin, *yyout;

%}

/* BISON DECLARATIONS */

/* %token_table */

%token TOK_EOL
%token TOK_USAGE
%token TOK_HELP
%token TOK_VERSION
%token TOK_COPYING
%token TOK_QUIT
%token TOK_ATTACH
%token TOK_DETACH
%token TOK_ENABLE
%token TOK_DISABLE
%token TOK_START
%token TOK_EMERGENCY
%token TOK_NORMAL
%token TOK_STOP
%token TOK_MSU
%token TOK_MONITOR
%token TOK_OPTIONS
%token TOK_CONFIG
%token TOK_STATS
%token TOK_STATSP
%token TOK_NOTIFY
%token TOK_GET
%token TOK_SHOW
%token TOK_SET
%token TOK_CLEAR
%token TOK_RESET
%token TOK_ADD
%token TOK_CHA
%token TOK_DEL
%token TOK_SELECT
%token TOK_DESELECT
%token TOK_STATUS
%token TOK_DEBUG
%token TOK_DF
%token TOK_MT
%token TOK_NA
%token TOK_SP
%token TOK_RS
%token TOK_RL
%token TOK_RT
%token TOK_LS
%token TOK_LK
%token TOK_SL
%token TOK_CB
%token TOK_CR
%token TOK_SDT
%token TOK_SDL
%token TOK_ITUT
%token TOK_ETSI
%token TOK_ANSI
%token TOK_JTTC
%token TOK_CHIN
%token TOK_MPLEV
%token TOK_PCR
%token TOK_HSL
%token TOK_XSN
%token TOK_TFR
%token TOK_TFRB
%token TOK_TFRR
%token TOK_MCSTA
%token TOK_MEMBER
%token TOK_CLUSTER
%token TOK_NETWORK
%token TOK_XFER
%token TOK_SECURITY
%token TOK_LOSCA
%token TOK_LOSCB
%token TOK_ADJACENT
%token TOK_DEVICE
%token TOK_CARD
%token TOK_SLOT
%token TOK_SPAN
%token TOK_NI
%token TOK_COST
%token TOK_MASK
%token TOK_DEST
%token TOK_SLC
%token TOK_ALLOW
%token TOK_RESTRICT
%token TOK_PROHIBIT
%token TOK_ACTIVATE
%token TOK_DEACTIVATE
%token TOK_BLOCK
%token TOK_UNBLOCK
%token TOK_INHIBIT
%token TOK_UNINHIBIT
%token TOK_CONGEST
%token TOK_UNCONGEST
%token TOK_DANGER
%token TOK_NODANGER
%token TOK_RESTART
%token TOK_RESTARTED
%token TOK_T1T
%token TOK_T1RT
%token TOK_T1ST
%token TOK_T1TT
%token TOK_T2T
%token TOK_T2HT
%token TOK_T2LT
%token TOK_T2TT
%token TOK_T3T
%token TOK_T4T
%token TOK_T4NT
%token TOK_T4ET
%token TOK_T5T
%token TOK_T6T
%token TOK_T7T
%token TOK_T8T
%token TOK_T10T
%token TOK_T11T
%token TOK_T12T
%token TOK_T13T
%token TOK_T14T
%token TOK_T15T
%token TOK_T16T
%token TOK_T17T
%token TOK_T18AT
%token TOK_T18T
%token TOK_T19AT
%token TOK_T19T
%token TOK_T20AT
%token TOK_T20T
%token TOK_T21AT
%token TOK_T21T
%token TOK_T22AT
%token TOK_T22T
%token TOK_T23AT
%token TOK_T23T
%token TOK_T24AT
%token TOK_T24T
%token TOK_T25AT
%token TOK_T26AT
%token TOK_T27AT
%token TOK_T28AT
%token TOK_T29AT
%token TOK_T30AT
%token TOK_T31AT
%token TOK_T32AT
%token TOK_T33AT
%token TOK_T34AT
%token TOK_CLOCK
%token TOK_INT
%token TOK_EXT
%token TOK_LOOP
%token TOK_MASTER
%token TOK_SLAVE
%token TOK_DPLL
%token TOK_ABR
%token TOK_SHAPER
%token TOK_TICK
%token TOK_RATE
%token TOK_TYPE
%token TOK_V35
%token TOK_DS0
%token TOK_DS0A
%token TOK_E1
%token TOK_T1
%token TOK_J1
%token TOK_ATM
%token TOK_MODE
%token TOK_PACKET
%token TOK_DSU
%token TOK_CSU
%token TOK_DTE
%token TOK_DCE
%token TOK_CLIENT
%token TOK_SERVER
%token TOK_PEER
%token TOK_ECHO
%token TOK_REM_LB
%token TOK_LOC_LB
%token TOK_LB_ECHO
%token TOK_TEST
%token TOK_GRATE
%token TOK_GTYPE
%token TOK_ETH
%token TOK_IP
%token TOK_UDP
%token TOK_TCP
%token TOK_RTP
%token TOK_SCTP
%token TOK_GMODE
%token TOK_GCRC
%token TOK_CRC4
%token TOK_CRC5
%token TOK_CRC6
%token TOK_CRC6J
%token TOK_CODING
%token TOK_NRZ
%token TOK_NRZI
%token TOK_AMI
%token TOK_B6ZS
%token TOK_B8ZS
%token TOK_AAL1
%token TOK_AAL2
%token TOK_AAL5
%token TOK_HDB3
%token TOK_FRAMING
%token TOK_CCS
%token TOK_CAS
%token TOK_SF
%token TOK_ESF
%token TOK_SYNCSRC
%token TOK_N1
%token TOK_N2
%token TOK_M
%token TOK_RB_ABATE
%token TOK_RB_ACCEPT
%token TOK_RB_DISCARD
%token TOK_TB_ABATE1
%token TOK_TB_ACCEPT1
%token TOK_TB_DISCARD1
%token TOK_TB_ABATE2
%token TOK_TB_ACCEPT2
%token TOK_TB_DISCARD2
%token TOK_TB_ABATE3
%token TOK_TB_ACCEPT3
%token TOK_TB_DISCARD3
%token TOK_TIN
%token TOK_TIE
%token TOK_T
%token TOK_D
%token TOK_TE
%token TOK_DE
%token TOK_UE
%token TOK_N
%token TOK_LCM
%token TOK_FAIL_UNSPECIFIED	    
%token TOK_FAIL_CONG_TIMEOUT	    
%token TOK_FAIL_ACK_TIMEOUT	    
%token TOK_FAIL_ABNORMAL_BSNR	    
%token TOK_FAIL_ABNORMAL_FIBR	    
%token TOK_FAIL_ABNORMAL	    
%token TOK_FAIL_SUERM_EIM	    
%token TOK_FAIL_ALIGNMENT	    
%token TOK_FAIL_RECEIVED_SIO	    
%token TOK_FAIL_RECEIVED_SIN	    
%token TOK_FAIL_RECEIVED_SIE	    
%token TOK_FAIL_RECEIVED_SIOS	    
%token TOK_FAIL_T1_TIMEOUT	    
%token TOK_RPO_BEGIN		    
%token TOK_RPO_END		    
%token TOK_CONGEST_ONSET_IND	    
%token TOK_CONGEST_DISCD_IND	    
%token TOK_TX_FAIL		    
%token TOK_RX_FAIL		    
%token TOK_CARRIER		    
%token TOK_SU_ERROR		    
%token TOK_LOST_SYNC		    
%token TOK_POINTCODE
%token TOK_NUMBER
%token TOK_STRING

/* GRAMMAR RULES */

%%

line:
    command { newline(); }
    | line command { newline(); }
    ;

command:
    TOK_EOL
    | help
    | usage
    | copying
    | version
    | quit
    | clear
    | debug
    ;

usage:
    TOK_USAGE { help(1, (char *[]){ prompt }); } TOK_EOL
    ;

copying:
    TOK_COPYING { copying(1, (char *[]){ prompt }); } TOK_EOL
    ;

version:
    TOK_VERSION { version(1, (char *[]){ prompt }); } TOK_EOL
    ;

clear: TOK_CLEAR TOK_EOL { fprintf(stdout, "\f"); } ;

help:
    TOK_HELP what TOK_EOL
    ;

what:
    { helpon(TOK_HELP); }
    | TOK_HELP { helpon(TOK_HELP); }
    | TOK_USAGE { helpon(TOK_USAGE); }
    | TOK_VERSION { helpon(TOK_VERSION); }
    | TOK_COPYING { helpon(TOK_COPYING); }
    ;

debug:
     TOK_DEBUG TOK_NUMBER TOK_EOL
     {
	 yydebug = $2;
	 fprintf(stdout, "debug level is now set to %d\n", $2);
     }
     ;

quit:
    TOK_QUIT TOK_EOL
    {
	YYACCEPT;
    }
    ;

/* ADDITIONAL C CODE */

%%

int
yyerror(const char *s)
{
	fprintf(stderr, "\n--ERROR: (%s:line %d) [%s] %s", filename, lineno, yytext, s);
	if (interactive) {
		fprintf(stderr, ", type \"help\" for information.\n");
		fflush(stderr);
	} else {
		fprintf(stderr, ".\n");
		fflush(stderr);
		exit(2);
	}
	return (0);
}

int
yyinit(void)
{
	yyin = stdin;
	yyout = stdout;
	return 0;
}

void
newline(void)
{
	if (interactive) {
		fprintf(stdout, "\n%s", prompt);
#if 0
		if (sel.levl > 0)
			fprintf(stdout, ":%s", sel.device);
		if (sel.levl > 1)
			fprintf(stdout, ":%d", sel.card);
		if (sel.levl > 2)
			fprintf(stdout, ":%d", sel.span);
		if (sel.levl > 3)
			fprintf(stdout, ":%d", sel.slot);
#endif
		fprintf(stdout, "> ");
		fflush(stdout);
	}
	lineno++;
}

void
helpon(YYSTYPE what)
{
	switch (what) {
	case TOK_USAGE:
		usage(1, (char *[]) {prompt});
		break;
	case TOK_COPYING:
		copying(1, (char *[]) {prompt});
		break;
	case TOK_VERSION:
		version(1, (char *[]) {prompt});
		break;
	default:
	case TOK_HELP:
		help(1, (char *[]) {prompt});
		break;
	}
	return;
}

void
copying(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
\n\
%1$s %2$s\n\
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
", prompt, ident);
}

void
version(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
    \n\
    %1$s:\n\
	%2$s\n\
	Copyright (c) 2003-2007  OpenSS7 Corporation.  All Rights Reserved.\n\
    \n\
	Distributed by OpenSS7 Corporation under GPL Version 3,\n\
	incorporated here by reference.\n\
    \n\
	See `%1$s --copying' for copying permission.\n\
    \n\
", argv[0], ident);
}

void
usage(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stderr, "\
    \n\
    Usage:\n\
	%1$s [options] {{-p, --ppa} PPA | {-c, --clei} CLEI}\n\
	%1$s {-h, --help}\n\
	%1$s {-V, --version}\n\
	%1$s {-C, --copying}\n\
    \n\
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
	%1$s [options] {{-p, --ppa} PPA | {-c, --clei} CLEI}\n\
	%1$s {-h, --help}\n\
	%1$s {-V, --version}\n\
	%1$s {-C, --copying}\n\
    Arguments:\n\
	(none)\n\
    General Options:\n\
	-q, --quiet                                 (default: off)\n\
	    suppress output\n\
	-v, --verbose                               (default: off)\n\
	    increase verbosity of output\n\
", argv[0]);
}

int
main(int argc, char **argv)
{
	int c;
	int val;

	while (1) {
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"interactive",	no_argument,		NULL, 'i'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"debug",	optional_argument,	NULL, 'd'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{NULL,		0,			NULL,  0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "iqvdhVC?", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 'i':	/* -i, --interactive */
			interactive = 1;
			break;
		case 'q':	/* -q, --quiet */
			output -= output > 0 ? 1 : output;
			interactive = 0;
			break;
		case 'd':	/* -d, --debug */
			yydebug = 1;
			break;
		case 'v':	/* -v, --versbose */
			if (optarg == NULL) {
				output += 1;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			break;
		case 'h':	/* -h, --help */
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copying */
			copying(argc, argv);
			exit(0);
		case '?':
		default:
		      bad_option:
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
	if (interactive) {
	    copying(argc, argv);
	    fprintf(stdout, "\
Type \"help\" for help...\n\
	    ");
	}
	newline();
	yyinit();
	while (yyparse()) ;
	exit(0);
}
