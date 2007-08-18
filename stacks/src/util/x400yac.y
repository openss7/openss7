/*****************************************************************************

 @(#) $RCSfile: x400yac.y,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $

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

 $Log: x400yac.y,v $
 Revision 0.9.2.1  2007/08/18 03:53:15  brian
 - working up configuration files

 *****************************************************************************/

/* C DECLARATIONS */

%{

#ident "@(#) $RCSfile: x400yac.y,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $"

static char const ident[] = "$RCSfile: x400yac.y,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $";

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

#include "x400yac.h"

#if !defined YYLTYPE && !defined YYLTYPE_IS_DECLARED
typedef int YYLTYPE;
#define yyltype YYLTYPE
#define YYLTYPE_IS_DECLARED 1
#define YYLTYPE_IS_TRIVIAL 1
#endif

#if !defined YYENABLE_NLS
#define YYENABLE_NLS 0
#endif

#define YYDEBUG 1

static char *prompt = "x400config";
static int interactive = 1;
static int output = 1;

void version(int, char*[]);
void copying(int, char*[]);
void usage(int, char*[]);
void help(int, char*[]);
void newline(void);
void helpon(int what);

int yydebug = 0;

extern int yylex(void);
int yyerror(const char *);

#define MAX_BUF_SIZE 256

static char filename[MAX_BUF_SIZE] = "stdin";
static int lineno = 0;
extern char *yytext;

extern FILE *yyin, *yyout;

char default_device[] = "/dev/x400p-sl";

struct select {
	char device[MAX_BUF_SIZE];
	char clli[MAX_BUF_SIZE];
	uint32_t type;
	uint32_t card;
	uint32_t span;
	uint32_t slot;
	int levl;
};

static struct select cur = { "/dev/x400p-sl", "", 0, 1, 1, 1, 0 };
static struct select sel = { "/dev/x400p-sl", "", 0, 1, 1, 1, 0 };

%}

/* BISON DECLARATIONS */

%union {
    int val;
    char *str;
}

/* %token_table */

%token <val> TOK_EOL
%token <val> TOK_EQUALS
%token <val> TOK_USAGE
%token <val> TOK_HELP
%token <val> TOK_VERSION
%token <val> TOK_COPYING
%token <val> TOK_QUIT
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
%token <val> TOK_OPTION
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
%token <val> TOK_PROFILE
%token <val> TOK_BASED_ON
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
    | get
    | set
    | show
    | select
    | deselect
    | profile
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

deselect:
    TOK_DESELECT deselection
    {
	sel.levl = $<val>2;
    }
    TOK_EOL
    ;

deselection:
    { yyerror("specify a level to deselect"); YYERROR; }
    | TOK_DEVICE { $<val>$ = 0; }
    | TOK_CARD { $<val>$ = 1; }
    | TOK_SPAN { $<val>$ = 2; }
    | TOK_SLOT { $<val>$ = 3; }

select:
    TOK_SELECT selection
    {
	sel = cur;
	sel.levl = $<val>2;
    }
    TOK_EOL
    {
	if (interactive && sel.levl>0) {
	    if (sel.levl>0)
		fprintf(stdout, "\nDevice...%.64s", sel.device);
	    if (sel.levl>1)
		fprintf(stdout, "\nCard.....%d", sel.card);
	    if (sel.levl>2)
		fprintf(stdout, "\nSpan.....%d", sel.span);
	    if (sel.levl>3)
		fprintf(stdout, "\nSlot.....%d", sel.slot);
	    fprintf(stdout, "\n");
	}
    }
    ;

selection:
    {
	cur.card = sel.card ;
	cur.span = sel.span ;
	cur.slot = sel.slot ;
	cur.levl = sel.levl ;
    }
    device_selection card_selection span_selection slot_selection
    {
	int l = 0;
	if ($<val>1) l = $<val>1; if ($<val>2) l = $<val>2; if ($<val>3) l = $<val>3; if ($<val>4) l = $<val>4;
	$<val>$ = l;
    }
    ;

device_selection: { $<val>$ = 0; }
    | TOK_DEVICE TOK_STRING
    { if ($<val>2)
	{ $<val>$ = 1; cur.levl = 1; strncpy(cur.device,$<str>2,MAX_BUF_SIZE); }
	else
	{ $<val>$ = 0; }
    }
    | TOK_DEVICE error { yyclearin; yyerrok; }
    ;

card_selection: { $<val>$ = 0; }
    | TOK_CARD TOK_NUMBER
    {
	if ( $<val>2 > 16 )
	{ yyerror("card number invalid: maximum 16"); YYERROR; }
	else if ( $<val>2 < 1 )
	{ yyerror("card number invalid; minimum 1"); YYERROR; }
	else
	{ $<val>$ = 2; cur.levl = 2; cur.card = $<val>2; }
    }
    | TOK_CARD error { yyclearin; yyerrok; }
    ;

span_selection: { $<val>$ = 0; }
    | TOK_SPAN TOK_NUMBER
    {
	if ( $<val>2 > 4 )
	{ yyerror("span number invalid: maximum 4"); YYERROR; }
	else if ( $<val>2 < 1 )
	{ yyerror("span number invalid; minimum 1"); YYERROR; }
	else
	{ $<val>$ = 3; cur.levl = 3; cur.span = $<val>2; }
    }
    | TOK_SPAN error { yyclearin; yyerrok; }
    ;

slot_selection: { $<val>$ = 0; }
    | TOK_SLOT TOK_NUMBER
    {
	if ( $<val>2 > 31 )
	{ yyerror("slot number invalid: maximum 31"); YYERROR; }
	else if ( $<val>2 < 1 )
	{ yyerror("slot number invalid; minimum 1"); YYERROR; }
	else
	{ $<val>$ = 4; cur.levl = 4; cur.slot = $<val>2; }
    }
    | TOK_SLOT error { yyclearin; yyerrok; }
    ;

get:
   TOK_GET TOK_EOL
   ;

set:
   TOK_SET parameter TOK_EOL
   ;

profile:
    TOK_PROFILE TOK_STRING based_on parmlist
    ;

based_on:
    { }
    | TOK_BASED_ON TOK_STRING
    ;

parmlist:
    parmlist parameter_and_value
    | parmlist option_and_value
    | parameter_and_value
    | option_and_value
    ;

parameter_and_value: parameter_name parameter_value ;

parameter_value: TOK_NUMBER { $<val>$ = $<val>1; } | TOK_STRING { $<str>$ = $<str>1; } ;

parameter_name:
    sl_parameter_name
    | sdt_parameter_name
    | sdl_parameter_name
    ;

sl_parameter_name:
    | TOK_RB_ABATE TOK_EQUALS TOK_NUMBER
    | TOK_RB_ACCEPT TOK_EQUALS TOK_NUMBER
    | TOK_RB_DISCARD TOK_EQUALS TOK_NUMBER
    | TOK_TB_ABATE1 TOK_EQUALS TOK_NUMBER
    | TOK_TB_ACCEPT1 TOK_EQUALS TOK_NUMBER
    | TOK_TB_DISCARD1 TOK_EQUALS TOK_NUMBER
    | TOK_TB_ABATE2 TOK_EQUALS TOK_NUMBER
    | TOK_TB_ACCEPT2 TOK_EQUALS TOK_NUMBER
    | TOK_TB_DISCARD2 TOK_EQUALS TOK_NUMBER
    | TOK_TB_ABATE3 TOK_EQUALS TOK_NUMBER
    | TOK_TB_ACCEPT3 TOK_EQUALS TOK_NUMBER
    | TOK_TB_DISCARD3 TOK_EQUALS TOK_NUMBER
    ;

sdt_parameter_name:
    TOK_TIN TOK_NUMBER
    | TOK_TIE TOK_NUMBER
    | TOK_T TOK_NUMBER
    | TOK_D TOK_NUMBER
    | TOK_TE TOK_NUMBER
    | TOK_DE TOK_NUMBER
    | TOK_UE TOK_NUMBER
    | TOK_N TOK_NUMBER
    ;

sdl_parameter_name:
    TOK_CLOCK clock_value
    | TOK_RATE rate_value
    | TOK_TYPE type_value
    | TOK_MODE mode_value
    | TOK_GRATE grate_value
    | TOK_GTYPE gtype_value
    | TOK_GMODE gmode_value
    | TOK_GCRC gcrc_value
    | TOK_CODING coding_value
    | TOK_FRAMING framing_value
    ;

clock_value:
    { }
    | TOK_INT
    | TOK_EXT
    | TOK_LOOP
    | TOK_MASTER
    | TOK_SLAVE
    | TOK_DPLL
    | TOK_ABR
    | TOK_SHAPER
    | TOK_TICK
    ;

rate_value:
    { }
    | TOK_NUMBER
    ;

type_value:
    { }
    | TOK_V35
    | TOK_DS0
    | TOK_DS0A
    | TOK_E1
    | TOK_T1
    | TOK_J1
    | TOK_ATM
    | TOK_PACKET
    ;

mode_value:
    { }
    | TOK_DSU
    | TOK_CSU
    | TOK_DTE
    | TOK_DCE
    | TOK_CLIENT
    | TOK_SERVER
    | TOK_PEER
    | TOK_ECHO
    | TOK_REM_LB
    | TOK_LOC_LB
    | TOK_LB_ECHO
    | TOK_TEST
    ;

grate_value:
    { }
    | TOK_NUMBER
    ;

gtype_value:
    { }
    | TOK_ETH
    | TOK_IP
    | TOK_UDP
    | TOK_TCP
    | TOK_RTP
    | TOK_SCTP
    ;

gmode_value:
    ;

gcrc_value:
    { }
    | TOK_CRC4
    | TOK_CRC5
    | TOK_CRC6
    | TOK_CRC6J
    ;

coding_value:
    { }
    | TOK_NRZ
    | TOK_NRZI
    | TOK_AMI
    | TOK_B6ZS
    | TOK_B8ZS
    | TOK_AAL1
    | TOK_AAL2
    | TOK_AAL5
    | TOK_HDB3
    ;

framing_value:
    { }
    | TOK_CCS
    | TOK_CAS
    | TOK_SF
    | TOK_ESF
    ;


option_and_value: TOK_OPTION option_value ;

option_value: TOK_NUMBER { $<val>$ = $<val>1; } | TOK_STRING { $<str>$ = $<str>1; } ;


parameter:
    {
	fprintf(stdout, "set what? - type \"help set\" for more info\n");
    }
    ;

help:
    TOK_HELP what TOK_EOL
    ;

what:
    {
	fprintf(stdout, "\n\
    Type `help' and one of the following:\n\
    \n\
        set    get    show   card    span    slot\n\
	");
    }
    | TOK_HELP
    {
	fprintf(stdout, "\n\
    Type `help' and one of the following:\n\
    \n\
        set    get    show   card    span    slot\n\
	");
    }
    | TOK_ADD
    {
	fprintf(stdout, "\
    add [clei CLEI] [[type TYPE] [card CARD] [span SPAN] [slot SLOT]] \\\n\
        [profile PROFILE] [device DEVICE] [modules MODULE[,MODULE]]\n\
    \n\
      - Add the signalling with identifier CLEI, signalling link for\n\
	type, TYPE, card number CARD, span number SPAN and timeslot\n\
	SLOT, with profile PROFILE, from device DEVICE with specified\n\
	modules pushed.  This results in opening a stream from device\n\
	DEVICE, pushing the specified modules list, binding it to card,\n\
	span and slot, linking it under the SL-MUX multiplexing driver,\n\
	and then assigning it the specified CLEI and global PPA\n\
	(TYPE:CARD:SPAN:SLOT).\n\
    \n\
      - TYPE must be \"x400p\".  DEVICE should be \"x400p-sl\".  MODULEs\n\
        pushed should be absent.\n\
    \n\
	");
    }
    | TOK_SET
    {
	fprintf(stdout, "\n\
    set [device DEVICE] [card CARD] [span SPAN] [slot SLOT] PARAMETER=VALUE\n\
    \n\
      - Sets a value on the specified or selected entity.  Parameters that\n\
        are available to be set vary by entity type: for more information\n\
        try \"help parameters\".  To view the current setting of parameters\n\
        on various entities, try the show or status commands.\n\
    \n\
      - Example:\n\
    \n\
        %s> set debug 3\n\
    \n\
      - Parameters: PARAMETER can be one of the following:\n\
    \n\
        clock, rate, type, mode, grate, gtype, gmode, gcrc, coding, framing, syncsrc\n\
	", prompt);
    }
    | TOK_CLOCK
    {
	fprintf(stdout, "\n\
    set [device DEVICE] [card CARD] [span SPAN] [slot SLOT] clock=VALUE\n\
    \n\
      - Sets the clock mode for the specified entity.  VALUE can have one\n\
        of the following mnemonic values:\n\
    \n\
          - default   the default clocking for the span.\n\
          - int       internal clocking.\n\
          - ext       external clocking.\n\
          - loop      loop clocking.\n\
          - master    master clock.\n\
          - slave     slave clock.\n\
          - dpll      digital phase locked loop.\n\
          - abr       average bit rate.\n\
          - shaper    traffic shaper.\n\
          - tick      system tick clock.\n\
	");
    }
    | TOK_RATE
    {
	fprintf(stdout, "\n\
    set [device DEVICE] [card CARD] [span SPAN] [slot SLOT] rate=VALUE\n\
    \n\
      - Sets the clock rate for the specified entity.  VALUE can be one\n\
        of the following mnemonic values (or a number):\n\
    \n\
          - default   the default rate for the span.\n\
	");
    }
    | TOK_TYPE
    {
	fprintf(stdout, "\n\
    set [device DEVICE] [card CARD] [span SPAN] [slot SLOT] type=VALUE\n\
    \n\
      - Sets the interface type for the specified entity.  VALUE can be one\n\
        of the following mnemonic values:\n\
    \n\
          - default   the channel interface is the default type.\n\
          - v35       the channel interface is a V.35 interface.\n\
          - ds0       the channel interface is a DS0 interface.\n\
          - ds0a      the channel interface is a DS0a interface.\n\
          - e1        the channel interface is an entire E1 span.\n\
          - t1        the channel interface is an entire T1 span.\n\
          - j1        the channel interface is an entire J1 span.\n\
          - atm       the channel interface is ATM.\n\
          - packet    the channel interface is packet based.\n\
	");
    }
    | TOK_MODE
    {
	fprintf(stdout, "\n\
    set [device DEVICE] [card CARD] [span SPAN] [slot SLOT] mode=VALUE\n\
    \n\
      - Sets the interface mode for the specified entity.  VALUE can be one\n\
        of the following mnemonic values:\n\
    \n\
          - default   the channel mode is the default for the device.\n\
          - dsu       the channel is DSU mode.\n\
          - csu       the channel is CSU mode.\n\
          - dte       the channel is DTE mode.\n\
          - dce       the channel is DCE mode.\n\
          - client    the channel is client mode.\n\
          - server    the channel is server mode.\n\
          - peer      the channel is peer mode.\n\
          - echo      the channel is in echo mode.\n\
          - rem_lb    the channel is in remote loopback mode.\n\
          - loc_lb    the channel is in local loopback mode.\n\
          - lb_echo   the channel is in loopback and echo mode.\n\
          - test      the channel is in test mode.\n\
	");
    }
    | TOK_SHOW
    {
	fprintf(stdout, "\n\
    show [device DEVICE] [card CARD] [span SPAN] [slot SLOT] {sdl|sdt|sl} config\n\
    \n\
      - Show the configuration for the specified or selected entity.\n\
        There are three levels of configuration sdl (signaling data link),\n\
        sdt (signalling data terminal) and sl (signalling link).\n\
    \n\
      - Example:\n\
    \n\
        %1$s> show device /dev/x400p-sl card 1 span 2 slot 3 sdt config\n\
	", prompt);
    }
    | TOK_GET
    | TOK_USAGE { usage(1, (char *[]){ prompt }); }
    | TOK_CLEAR
    {
	fprintf(stdout, "\n\
    clear\n\
    \n\
    - clear the screen.\n\
    \n\
    - Example:\n\
    \n\
      %1$s> clear\n\
    \n\
	", prompt);
    }
    | TOK_SELECT
    {
	fprintf(stdout, "\n\
    select [device DEVICE] [card CARD] [span SPAN] [slot SLOT]\n\
    \n\
      - Selects an entity which will be the target of toerh commands\n\
        which do not eplicitly specify an entity.  Once a level of\n\
        entity (card, span, slot) is selected, the shell prompt reflects\n\
        the current selection.\n\
    \n\
      - Example:\n\
    \n\
        %1$s> select device /dev/x400p-sl\n\
        %1$s> select card 1\n\
        %1$s> select span 2\n\
    \n\
	", prompt);
    }
    | TOK_STATUS
    {
	fprintf(stdout, "\n\
    status [device DEVICE] [card CARD] [span SPAN] [slot SLOT]\n\
    \n\
      - Provides the status of the selected entity.  The status is the\n\
        device driver's idea of the status of the selected entity (device,\n\
        card, span, and slot).\n\
    \n\
      - Example:\n\
    \n\
        %1$s> status device /dev/x400p-sl card 1 span 2 slot 23\n\
    \n\
	", prompt);
    }
    | TOK_PROFILE
    {
	fprintf(stdout, "\n\
    profile IDENTIFIER [based-on PROFILE] [PARAMETER=VALUE] ...\n\
    \n\
     - Defines a profile of parameter values and option values for use\n\
       with a signalling link.  IDENTIFIER must be unique.  Some built-in\n\
       IDENTIFIERs are pre-defined.  A new IDENTIFIER profile can be\n\
       based on the values of any other PROFILE.  This command can be\n\
       used repeatedly with the same IDENTIFIER but different sets of\n\
       PARAMETER=VALUE pairs to alter parameters.  When a signalling\n\
       link is created, the parameters in effect at the time of creation\n\
       are used.  Altering the profile after signalling link creation\n\
       does not change the values associated with already-created\n\
       signalling links.\n\
    \n\
     - Built-in profiles are as follows:\n\
    \n\
	itut-1988	itut-1992	itut-1996	itut-2000\n\
	itut-2004	ansi-1988	ansi-1992	ansi-1996\n\
	ansi-2000	ansi-2004	jttc-1994	jttc-1998\n\
	jttc-2002	chin-2004\n\
    \n\
     - Protocol profiles not based on a built-in or user-defined profile\n\
       will have values default to itut-2004.\n\
    \n\
     - Example:\n\
    \n\
       %1$s> profile MY_ANSI based-on ansi-1992 t4n=4000 option=hsl\n\
    \n\
	", prompt);
    }
    | TOK_DEVICE
    | TOK_CARD
    | TOK_SPAN
    | TOK_SLOT
    ;

show:
    TOK_SHOW selection something TOK_EOL
    ;

something:
    {
	fprintf(stdout, "show what? - type \"help show\" for more information.\n");
    }
    | TOK_SDL TOK_CONFIG
    {
	fprintf(stdout, "Signalling Data Link Configuration:\n");
    }
    | TOK_SDT TOK_CONFIG
    {
	fprintf(stdout, "Signalling Terminal Configuration:\n");
    }
    | TOK_SL TOK_CONFIG
    {
	fprintf(stdout, "Signalling Link Configuration:\n");
    }
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
		if (sel.levl > 0)
			fprintf(stdout, ":%.64s", sel.device);
		if (sel.levl > 1)
			fprintf(stdout, ":%d", sel.card);
		if (sel.levl > 2)
			fprintf(stdout, ":%d", sel.span);
		if (sel.levl > 3)
			fprintf(stdout, ":%d", sel.slot);
		fprintf(stdout, "> ");
		fflush(stdout);
	}
	lineno++;
}

void
helpon(int what)
{
	switch (what) {
	case TOK_USAGE:
		return usage(1, (char *[]) { prompt});
	case TOK_COPYING:
		return copying(1, (char *[]) { prompt});
	case TOK_VERSION:
		return version(1, (char *[]) { prompt});
	default:
	case TOK_HELP:
		return help(1, (char *[]) { prompt});
	}
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
The purpose of the x400pconf program is to configure X400p driver\n\
signalling links for operation and then link them beneath the SL-MUX\n\
multiplexing driver for general purpose signalling link management.\n\
\n\
This can be performed on a per-command basis with the --add, --remove,\n\
--get options, or can be performed on a batch basis with the --file\n\
option.  The --interactive option provides the ability to interact with\n\
each signalling link before linking it under the SL-MUX multiplexing\n\
driver.\n\
\n\
Usage:\n\
    %1$s [options] {{-p, --ppa} PPA | {-c, --clei} CLEI}\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
\n\
Arguments:\n\
    CLEI\n\
	common language equipment identifier	[default: (null)]\n\
    PPA\n\
	global physical point of attachment	[default: %3$d:%4$d:%5$d:%6$d]\n\
\n\
Command Options:\n\
    -a, --add CLEI PPA arguments ...		[default: none]\n\
	add a signalling link\n\
    -g, --get [CLEI|PPA] ...			[default: all]\n\
	get one or more signalling links\n\
    -r, --remove [CLEI|PPA] ...			[default: none]\n\
	remove one or more signalling links\n\
    -i, --interactive				[default: off]\n\
	run interactive with terminal\n\
    -f, --file FILENAME\n\
	process commands from file FILENAME	[default: %2$.32s]\n\
\n\
Parameter Options:\n\
    -c, --clei CLEI				[default: (null)]\n\
	common language equipment identifier\n\
    -p, --ppa PPA\n\
	global physical point of appearance	[default: %3$d:%4$d:%5$d:%6$d]\n\
\n\
General Options:\n\
    -q, --quiet                                 [default: off]\n\
	suppress output\n\
    -v, --verbose                               [default: off]\n\
	increase verbosity of output\n\
    -d, --debug                                 [default: off]\n\
	increase debugging output\n\
    -h, --help\n\
	print usage information and exit\n\
    -V, --version\n\
	print version information an exit\n\
    -C, --copying\n\
	print copying permissions and exit\n\
\n\
", argv[0], filename, sel.type, sel.card, sel.span, sel.slot);
}

enum {
    COMMAND_NONE = 0,
    COMMAND_GET,
    COMMAND_REMOVE,
    COMMAND_ADD,
    COMMAND_INTERACTIVE,
    COMMAND_FILE,
} cmd = COMMAND_NONE;

int
main(int argc, char **argv)
{
	int c;
	int val;
	char *beg, *end;

	while (1) {
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"get",		optional_argument,	NULL, 'g'},
			{"remove",	optional_argument,	NULL, 'r'},
			{"add",		required_argument,	NULL, 'a'},
			{"interactive",	no_argument,		NULL, 'i'},
			{"file",	optional_argument,	NULL, 'f'},
			{"ppa",		required_argument,	NULL, 'p'},
			{"clei",	required_argument,	NULL, 'c'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"debug",	optional_argument,	NULL, 'd'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{NULL,		0,			NULL,  0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "g::r::a:if::p:c:qvdhVC?", long_options, &option_index);
		if (c == -1)
			break;
		switch (c) {
		case 'g':	/* -g, --get [CLEI|PPA] ... */
			if (cmd != COMMAND_NONE)
			    goto bad_option;
			cmd = COMMAND_GET;
			break;
		case 'r':	/* -r, --remove [CLEI|PPA] ... */
			if (cmd != COMMAND_NONE)
			    goto bad_option;
			cmd = COMMAND_REMOVE;
			break;
		case 'a':	/* -a, --add CLEI PPA arguments */
			if (cmd != COMMAND_NONE)
			    goto bad_option;
			cmd = COMMAND_ADD;
			break;
		case 'i':	/* -i, --interactive */
			if (cmd != COMMAND_NONE)
			    goto bad_option;
			cmd = COMMAND_INTERACTIVE;
			interactive = 1;
			break;
		case 'f':	/* -f, --file [FILENAME] */
			if (cmd != COMMAND_NONE)
			    goto bad_option;
			if (optarg != NULL)
			    strncpy(filename,optarg,MAX_BUF_SIZE);
			cmd = COMMAND_FILE;
			break;
		case 'p':	/* -p, --ppa PPA */
			/* Global PPA is formatted as type:card:span:slot.
			 * Type for X400P is always the string x400p or the
			 * value 1. */
			if (optarg == NULL)
			    goto bad_option;
			beg = optarg;
			sel.type = 0;	/* set defaults */
			sel.card = 1;	/* set defaults */
			sel.span = 1;	/* set defaults */
			sel.slot = 1;	/* set defaults */
			if ((beg = strsep(&beg, ":"))) {
			    if ((sel.type = strtol(beg, &end, 0)) == 0) {
				if (beg == end) {
				    if (!strcmp(beg, "none")) {
					sel.type = 0;
				    } else if (!strcmp(beg, "x400p")) {
					sel.type = 1;
				    } else if (!strcmp(beg, "m2pa")) {
					sel.type = 2;
				    } else if (!strcmp(beg, "m2ua")) {
					sel.type = 3;
				    } else {
					goto bad_option;
				    }
				}
			    }
			    switch(sel.type) {
				case 0: case 1: case 2: case 3:
				break;
				default: goto bad_option;
			    }
			}
			if ((beg = strsep(&beg, ":"))) {
			    if ((sel.card = strtol(beg, &end, 0)) == 0) {
				if (beg == end)
				    goto bad_option;
			    }
			    if (sel.card < 1 || sel.card > 8)
				    goto bad_option;
			}
			if ((beg = strsep(&beg, ":"))) {
			    if ((sel.span = strtol(beg, &end, 0)) == 0) {
				if (beg == end)
				    goto bad_option;
			    }
			    if (sel.span < 1 || sel.span > 4)
				    goto bad_option;
			}
			if ((beg = strsep(&beg, ":"))) {
			    if ((sel.slot = strtol(beg, &end, 0)) == 0) {
				if (beg == end)
				    goto bad_option;
			    }
			    if (sel.slot < 1 || sel.slot > 31)
				    goto bad_option;
			}
			break;
		case 'c':	/* -c, --clei CLEI */
			/* CLEI is simply a string. */
			if (optarg == NULL || sel.clli[0] != '\0')
			    goto bad_option;
			strncpy(sel.clli,optarg,MAX_BUF_SIZE);
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
	/* Each of the remaining options arguments are either CLEIs or PPAs. */
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
