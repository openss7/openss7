/*****************************************************************************

 @(#) src/util/m2payac.y

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2019  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 *****************************************************************************/

/* C DECLARATIONS */

%{

static char const ident[] = "src/util/m2payac.y (" PACKAGE_ENVR " " PACKAGE_DATE ")";

#include <stropts.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
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

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>
#include <ss7/sli.h>
#include <ss7/sli_ioctl.h>

#include <sys/npi.h>
#include <sys/npi_sctp.h>

#include "m2payac.h"

#if !defined YYLTYPE && !defined YYLTYPE_IS_DECLARED
typedef long YYLTYPE;
#define yyltype YYLTYPE
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 1
#endif

#if !defined YYENABLE_NLS
#define YYENABLE_NLS 0
#endif

#define YYDEBUG 1

static char *prompt = "m2paconfig";
static int interactive = 1;
static int output = 1;

void version();
void copying();
void usage(char*[]);
void help(char*[]);
void newline(void);
void helpon(int what);

int yydebug = 0;

extern int yylex(void);
int yyerror(const char *);

static char *filename = "stdin";
static int lineno = 0;
extern char *yytext;

extern FILE *yyin, *yyout;

#define MAX_BUF_SIZE 64
#define MAX_HANDLES  32

struct select {
    char device[MAX_BUF_SIZE];
    char clli[MAX_BUF_SIZE];
    int type;
    int card;
    int span;
    int slot;
    int level;
    int state;
    int fd;
};

int handle = 0;

struct select cur, sel[MAX_HANDLES];


%}

/* BISON DECLARATIONS */

%debug
%defines
%locations
%token-table
%verbose

%union {
    int val;
    char *str;
}

/* %token-table */

%token <val> TOK_EOL
%token <val> TOK_USAGE
%token <val> TOK_HELP
%token <val> TOK_VERSION
%token <val> TOK_COPYING
%token <val> TOK_QUIT
%token <val> TOK_OPEN
%token <val> TOK_CLOSE
%token <val> TOK_ATTACH
%token <val> TOK_DETACH
%token <val> TOK_ENABLE
%token <val> TOK_DISABLE
%token <val> TOK_START
%token <val> TOK_EMERGENCY
%token <val> TOK_NORMAL
%token <val> TOK_STOP
%token <val> TOK_MSU
%token <val> TOK_MONITOR
%token <val> TOK_OPTIONS
%token <val> TOK_CONFIG
%token <val> TOK_STATS
%token <val> TOK_STATSP
%token <val> TOK_NOTIFY
%token <val> TOK_GET
%token <val> TOK_SHOW
%token <val> TOK_SET
%token <val> TOK_CLEAR
%token <val> TOK_RESET
%token <val> TOK_ADD
%token <val> TOK_CHA
%token <val> TOK_DEL
%token <val> TOK_SELECT
%token <val> TOK_DESELECT
%token <val> TOK_STATUS
%token <val> TOK_DEBUG
%token <val> TOK_DF
%token <val> TOK_MT
%token <val> TOK_NA
%token <val> TOK_SP
%token <val> TOK_RS
%token <val> TOK_RL
%token <val> TOK_RT
%token <val> TOK_LS
%token <val> TOK_LK
%token <val> TOK_SL
%token <val> TOK_CB
%token <val> TOK_CR
%token <val> TOK_SDT
%token <val> TOK_SDL
%token <val> TOK_X400P
%token <val> TOK_M2PA
%token <val> TOK_M2UA
%token <val> TOK_NONE
%token <val> TOK_CLLI
%token <val> TOK_ITUT
%token <val> TOK_ETSI
%token <val> TOK_ANSI
%token <val> TOK_JTTC
%token <val> TOK_CHIN
%token <val> TOK_MPLEV
%token <val> TOK_PCR
%token <val> TOK_HSL
%token <val> TOK_XSN
%token <val> TOK_TFR
%token <val> TOK_TFRB
%token <val> TOK_TFRR
%token <val> TOK_MCSTA
%token <val> TOK_MEMBER
%token <val> TOK_CLUSTER
%token <val> TOK_NETWORK
%token <val> TOK_XFER
%token <val> TOK_SECURITY
%token <val> TOK_LOSCA
%token <val> TOK_LOSCB
%token <val> TOK_ADJACENT
%token <val> TOK_DEVICE
%token <val> TOK_CARD
%token <val> TOK_SLOT
%token <val> TOK_SPAN
%token <val> TOK_NI
%token <val> TOK_COST
%token <val> TOK_MASK
%token <val> TOK_DEST
%token <val> TOK_SLC
%token <val> TOK_ALLOW
%token <val> TOK_RESTRICT
%token <val> TOK_PROHIBIT
%token <val> TOK_ACTIVATE
%token <val> TOK_DEACTIVATE
%token <val> TOK_BLOCK
%token <val> TOK_UNBLOCK
%token <val> TOK_INHIBIT
%token <val> TOK_UNINHIBIT
%token <val> TOK_CONGEST
%token <val> TOK_UNCONGEST
%token <val> TOK_DANGER
%token <val> TOK_NODANGER
%token <val> TOK_RESTART
%token <val> TOK_RESTARTED
%token <val> TOK_T1T
%token <val> TOK_T1RT
%token <val> TOK_T1ST
%token <val> TOK_T1TT
%token <val> TOK_T2T
%token <val> TOK_T2HT
%token <val> TOK_T2LT
%token <val> TOK_T2TT
%token <val> TOK_T3T
%token <val> TOK_T4T
%token <val> TOK_T4NT
%token <val> TOK_T4ET
%token <val> TOK_T5T
%token <val> TOK_T6T
%token <val> TOK_T7T
%token <val> TOK_T8T
%token <val> TOK_T10T
%token <val> TOK_T11T
%token <val> TOK_T12T
%token <val> TOK_T13T
%token <val> TOK_T14T
%token <val> TOK_T15T
%token <val> TOK_T16T
%token <val> TOK_T17T
%token <val> TOK_T18AT
%token <val> TOK_T18T
%token <val> TOK_T19AT
%token <val> TOK_T19T
%token <val> TOK_T20AT
%token <val> TOK_T20T
%token <val> TOK_T21AT
%token <val> TOK_T21T
%token <val> TOK_T22AT
%token <val> TOK_T22T
%token <val> TOK_T23AT
%token <val> TOK_T23T
%token <val> TOK_T24AT
%token <val> TOK_T24T
%token <val> TOK_T25AT
%token <val> TOK_T26AT
%token <val> TOK_T27AT
%token <val> TOK_T28AT
%token <val> TOK_T29AT
%token <val> TOK_T30AT
%token <val> TOK_T31AT
%token <val> TOK_T32AT
%token <val> TOK_T33AT
%token <val> TOK_T34AT
%token <val> TOK_CLOCK
%token <val> TOK_INT
%token <val> TOK_EXT
%token <val> TOK_LOOP
%token <val> TOK_MASTER
%token <val> TOK_SLAVE
%token <val> TOK_DPLL
%token <val> TOK_ABR
%token <val> TOK_SHAPER
%token <val> TOK_TICK
%token <val> TOK_RATE
%token <val> TOK_TYPE
%token <val> TOK_V35
%token <val> TOK_DS0
%token <val> TOK_DS0A
%token <val> TOK_E1
%token <val> TOK_T1
%token <val> TOK_J1
%token <val> TOK_ATM
%token <val> TOK_MODE
%token <val> TOK_PACKET
%token <val> TOK_DSU
%token <val> TOK_CSU
%token <val> TOK_DTE
%token <val> TOK_DCE
%token <val> TOK_CLIENT
%token <val> TOK_SERVER
%token <val> TOK_PEER
%token <val> TOK_ECHO
%token <val> TOK_REM_LB
%token <val> TOK_LOC_LB
%token <val> TOK_LB_ECHO
%token <val> TOK_TEST
%token <val> TOK_GRATE
%token <val> TOK_GTYPE
%token <val> TOK_ETH
%token <val> TOK_IP
%token <val> TOK_UDP
%token <val> TOK_TCP
%token <val> TOK_RTP
%token <val> TOK_SCTP
%token <val> TOK_GMODE
%token <val> TOK_GCRC
%token <val> TOK_CRC4
%token <val> TOK_CRC5
%token <val> TOK_CRC6
%token <val> TOK_CRC6J
%token <val> TOK_CODING
%token <val> TOK_NRZ
%token <val> TOK_NRZI
%token <val> TOK_AMI
%token <val> TOK_B6ZS
%token <val> TOK_B8ZS
%token <val> TOK_AAL1
%token <val> TOK_AAL2
%token <val> TOK_AAL5
%token <val> TOK_HDB3
%token <val> TOK_FRAMING
%token <val> TOK_CCS
%token <val> TOK_CAS
%token <val> TOK_SF
%token <val> TOK_ESF
%token <val> TOK_SYNCSRC
%token <val> TOK_N1
%token <val> TOK_N2
%token <val> TOK_M
%token <val> TOK_RB_ABATE
%token <val> TOK_RB_ACCEPT
%token <val> TOK_RB_DISCARD
%token <val> TOK_TB_ABATE1
%token <val> TOK_TB_ACCEPT1
%token <val> TOK_TB_DISCARD1
%token <val> TOK_TB_ABATE2
%token <val> TOK_TB_ACCEPT2
%token <val> TOK_TB_DISCARD2
%token <val> TOK_TB_ABATE3
%token <val> TOK_TB_ACCEPT3
%token <val> TOK_TB_DISCARD3
%token <val> TOK_TIN
%token <val> TOK_TIE
%token <val> TOK_T
%token <val> TOK_D
%token <val> TOK_TE
%token <val> TOK_DE
%token <val> TOK_UE
%token <val> TOK_N
%token <val> TOK_LCM
%token <val> TOK_FAIL_UNSPECIFIED	    
%token <val> TOK_FAIL_CONG_TIMEOUT	    
%token <val> TOK_FAIL_ACK_TIMEOUT	    
%token <val> TOK_FAIL_ABNORMAL_BSNR	    
%token <val> TOK_FAIL_ABNORMAL_FIBR	    
%token <val> TOK_FAIL_ABNORMAL	    
%token <val> TOK_FAIL_SUERM_EIM	    
%token <val> TOK_FAIL_ALIGNMENT	    
%token <val> TOK_FAIL_RECEIVED_SIO	    
%token <val> TOK_FAIL_RECEIVED_SIN	    
%token <val> TOK_FAIL_RECEIVED_SIE	    
%token <val> TOK_FAIL_RECEIVED_SIOS	    
%token <val> TOK_FAIL_T1_TIMEOUT	    
%token <val> TOK_RPO_BEGIN		    
%token <val> TOK_RPO_END		    
%token <val> TOK_CONGEST_ONSET_IND	    
%token <val> TOK_CONGEST_DISCD_IND	    
%token <val> TOK_TX_FAIL		    
%token <val> TOK_RX_FAIL		    
%token <val> TOK_CARRIER		    
%token <val> TOK_SU_ERROR		    
%token <val> TOK_LOST_SYNC		    
%token <val> TOK_POINTCODE
%token <val> TOK_NUMBER
%token <str> TOK_STRING

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
    | error TOK_EOL
    ;

usage:
    TOK_USAGE { help(&prompt); } TOK_EOL
    ;

copying:
    TOK_COPYING { copying(); } TOK_EOL
    ;

version:
    TOK_VERSION { version(); } TOK_EOL
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
    | TOK_OPEN { helpon(TOK_OPEN); }
    | TOK_CLOSE { helpon(TOK_CLOSE); }
    | TOK_ATTACH { helpon(TOK_ATTACH); }
    | TOK_DETACH { helpon(TOK_DETACH); }
    | TOK_ENABLE { helpon(TOK_ENABLE); }
    | TOK_DISABLE { helpon(TOK_DISABLE); }
    | TOK_START { helpon(TOK_START); }
    | TOK_STOP { helpon(TOK_STOP); }
    | TOK_EMERGENCY { helpon(TOK_EMERGENCY); }
    | TOK_NORMAL { helpon(TOK_NORMAL); }
    | TOK_MSU { helpon(TOK_MSU); }
    ;

debug:
     TOK_DEBUG TOK_NUMBER TOK_EOL
     {
	 yydebug = $<val>2;
	 fprintf(stdout, "debug level is now set to %d\n", $<val>2);
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
	fprintf(stderr, "\n--ERROR: (%s:line %d) [%s] %s.\n", filename, lineno, yytext, s);
	if (interactive) {
		fprintf(stderr, "Type \"help\" for information.\n");
		fflush(stderr);
	} else {
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
helpon(int what)
{
	switch (what) {
	case TOK_OPEN:
		fprintf(stdout, "\
    \n\
    open [NUMBER] [device DEVICE]\n\
    \n\
      - Opens the specified or selected DEVICE to obtain a signalling\n\
	link stream and associates it with stream NUMBER.  NUMBER must\n\
	be a integer between 0 and %2$d inclusive.  DEVICE should be\n\
	the full or relative pathname to a character device special file\n\
	corresponding to the STREAMS driver to open an instance.\n\
    \n\
      - If NUMBER is not specified, then the most recent handle will be\n\
        used, or number `0' if no other handle has been used.  The current\n\
	default is `%3$d'.\n\
    \n\
      - If DEVICE is not specified, then the most recent device for the\n\
        handle NUMBER will be used, or \"/dev/x400p-sl\" if no other\n\
	device has been used for the handle.  The current default is\n\
	\"%4$.64s\".\n\
    \n\
      - Example:\n\
    \n\
        %1$s> open 3 device /dev/x400p-sl\n\
    \n\
        The above example opens the device \"/dev/x400p-sl\" and, if\n\
	successful, associates it with stream number `3'.  If an error\n\
	occurs, a diagnostic will be printed.\n\
    \n\
        %1$s> open\n\
    \n\
        The above example opens device \"%4$.64s\" and, if successful\n\
	associates it with stream number `%3$d'.  If an error occurs, a\n\
	diagnostic is printed.\n\
    \n\
		", prompt, MAX_HANDLES-1, handle, sel[handle].device);
		return;
	case TOK_CLOSE:
		fprintf(stdout, "\
    \n\
    close [NUMBER]\n\
    \n\
      - Closes the currently open signalling link stream, NUMBER.\n\
    \n\
      - Example:\n\
    \n\
        %1$s> close\n\
    \n\
		", prompt);
		return;
	case TOK_ATTACH:
		fprintf(stdout, "\
    \n\
    attach [NUMBER] [type TYPE] [card CARD] [span SPAN] [slot SLOT]\n\
    attach [NUMBER] [clli CLLI]\n\
    \n\
      - Attaches the currently open device stream, NUMBER, to the\n\
	selected or specified type, card, span and slot number.  TYPE is\n\
	the type of card, or driver type and is 1, 2, or 3; CARD is the\n\
	number of the card, from 1 to 32; SPAN is the span number on the\n\
	card, from 1 to 4; SLOT is the timeslot number within the span;\n\
	from 1 to 24 for T1 or J1, and from 1 to 31 for E1.\n\
    \n\
      - Alternately, attaches the currently open device stream, NUMBER,\n\
	to the specified common language location identifer, CLLI.\n\
    \n\
      - The defaults are TYPE=x400p, CARD=1, SPAN=1, SLOT=1.\n\
    \n\
      - Example:\n\
    \n\
          %1$s> attach type x400p card 1 span 1 slot 19\n\
    \n\
		", prompt);
		return;
	case TOK_DETACH:
		fprintf(stdout, "\
    \n\
    detach [NUMBER]\n\
    \n\
      - Detaches the currently attached signalling link.  This has the\n\
        effect of issuing the local management detach request to the\n\
        attached signalling link.\n\
    \n\
      - Example:\n\
    \n\
        %1$s> detach\n\
    \n\
		", prompt);
		return;
	case TOK_ENABLE:
		fprintf(stdout, "\
    \n\
    enable [NUMBER]\n\
    \n\
      - Enables the currently attached signalling link.  This  has the\n\
	effect of issuing the local management enable request to the\n\
	attached signalling link.\n\
    \n\
      - Example:\n\
    \n\
        %1$s> enable\n\
    \n\
		", prompt);
		return;
	case TOK_DISABLE:
		fprintf(stdout, "\
    \n\
    disable [NUMBER]\n\
    \n\
      - Disable the currently enabled signallingn link.  This has the\n\
	effect of issuing the local management disable request to the\n\
	enabled signalling link.\n\
    \n\
      - Example:\n\
    \n\
        %1$s> disable\n\
    \n\
		", prompt);
		return;
	case TOK_START:
		fprintf(stdout, "\
    \n\
    start [NUMBER]\n\
    \n\
      - Starts the currently enabled signalling link.  This has the\n\
	effect of issuing the MTP Level 2 start command to the enabled\n\
	signalling link.\n\
    \n\
      - Example:\n\
    \n\
        %1$s> start\n\
    \n\
		", prompt);
		return;
	case TOK_STOP:
		fprintf(stdout, "\
    \n\
    stop [NUMBER]\n\
    \n\
      - Stops the currently enabled signalling link.  This has the\n\
	effect of issuing the MTP Level 2 stop command to the enabled\n\
	signalling link.\n\
    \n\
      - Example:\n\
    \n\
        %1$s> stop\n\
    \n\
		", prompt);
		return;
	case TOK_EMERGENCY:
		fprintf(stdout, "\
    \n\
    emergency [NUMBER]\n\
    \n\
      - Sets emergency on the currently enabled signalling link.\n\
    \n\
      - Example:\n\
    \n\
        %1$s> emergency\n\
    \n\
		", prompt);
		return;
	case TOK_NORMAL:
		fprintf(stdout, "\
    \n\
    normal [NUMBER]\n\
    \n\
      - Clears emergency on the currently enabled signalling link.\n\
    \n\
      - Example:\n\
    \n\
        %1$s> normal\n\
    \n\
		", prompt);
		return;
	case TOK_MSU:
		fprintf(stdout, "\
    \n\
    msu [NUMBER] [count COUNT]\n\
    \n\
      - Sends a count of MSUs on an in-service signalling link.\n\
    \n\
      - Example:\n\
    \n\
        %1$s> msu 60\n\
    \n\
		", prompt);
		return;
	case TOK_USAGE:
		usage(&prompt);
		break;
	case TOK_COPYING:
		copying();
		break;
	case TOK_VERSION:
		version();
		break;
	default:
		help(&prompt);
		break;
	case TOK_HELP:
		fprintf(stdout, "\
    \n\
    help [COMMAND]\n\
    \n\
      - Provides help on a number of commands.  COMMAND may be one of\n\
	the following:\n\
    \n\
	open	    close	attach	    detach	enable\n\
	disable	    start	stop	    emergency	normal\n\
	usage	    copying	version	    help	msu\n\
    \n\
      - Example:\n\
    \n\
        %1$s> help attach\n\
    \n\
		", prompt);
		break;
	}
	return;
}

void
copying()
{
	if (!output)
		return;
	(void) fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2008-2019  Monavacon Limited <http://www.monavacon.com/>\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
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

void
version()
{
	if (!output)
		return;
	(void) fprintf(stdout, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
Written by Brian Bidulock.\n\
\n\
Copyright (c) 2008, 2009, 2010, 2012, 2015, 2017, 2018, 2019  Monavacon Limited.\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation.\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 under GNU Affero General Public License Version 3,\n\
with conditions, incorporated herein by reference.\n\
\n\
See `%1$s --copying' for copying permissions.\n\
", NAME, PACKAGE, VERSION, PACKAGE_ENVR " " PACKAGE_DATE);
}

void
usage(char *argv[])
{
	if (!output)
		return;
	(void) fprintf(stderr, "\
Usage:\n\
    %1$s [options] {{-p|--ppa} PPA | {-c|--clei} CLEI}\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

void
help(char *argv[])
{
	if (!output)
		return;
	(void) fprintf(stdout, "\
Usage:\n\
    %1$s [options] {{-p|--ppa} PPA | {-c|--clei} CLEI}\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    None.\n\
Options:\n\
    -q, --quiet                                 (default: off)\n\
        suppress normal output (equivalent to --verbose=0)\n\
    -d, --debug [LEVEL]                         (default: 0)\n\
        increment or set debug LEVEL [default: 0]\n\
    -v, --verbose [LEVEL]                       (default: 1)\n\
        increment or set output verbosity LEVEL [default: 1]\n\
        this option may be repeated.\n\
    -h, --help, -?, --?\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
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
			help(argv);
			exit(0);
		case 'V':	/* -V, --version */
			version();
			exit(0);
		case 'C':	/* -C, --copying */
			copying();
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
			usage(argv);
			exit(2);
		}
	}
	if (interactive) {
	    copying();
	    fprintf(stdout, "\
Type \"help\" for help...\n\
	    ");
	}
	newline();
	yyinit();
	while (yyparse()) ;
	exit(0);
}
