/*****************************************************************************

 @(#) $RCSfile: slconf_yac.y,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $

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

 $Log: slconf_yac.y,v $
 Revision 0.9.2.1  2007/08/18 03:53:15  brian
 - working up configuration files

 *****************************************************************************/

/* C DECLARATIONS */

%{

#ident "@(#) $RCSfile: slconf_yac.y,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $"

static char const ident[] = "$RCSfile: slconf_yac.y,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/08/18 03:53:15 $";

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
#include <sys/sad.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

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

#include "slconf_yac.h"

#if !defined YYENABLE_NLS
#define YYENABLE_NLS 0
#endif

static const char *prompt = "slconfig";
extern int interactive;

void version(const char *);
void copying(const char *);
void usage(const char *);
void help(const char *);

void newline(void);

int yydebug = 0;

extern int yylex(void);
int yyerror(const char *);

static char *filename = "stdin";
extern int yylineno;
extern char *yytext;

extern FILE *yyin, *yyout;

#define MAX_BUF_SIZE 64
#define MAX_HANDLES  32

struct link_config {
    struct sdl_config sdl;
    struct sdt_config sdt;
    struct sl_config sl;
};

struct link_config itut_default = {
    .sdl = {
	.ifname = NULL,
	.ifflags = 0,
	.iftype = SDL_TYPE_DS0,
	.ifrate = 64000,
	.ifgtype = SDL_GTYPE_E1,
	.ifgrate = 2048000,
	.ifmode = SDL_MODE_PEER,
	.ifgmode = SDL_GMODE_NONE,
	.ifgcrc = SDL_GCRC_CRC5,
	.ifclock = SDL_CLOCK_SLAVE,
	.ifcoding = SDL_CODING_HDB3,
	.ifframing = SDL_FRAMING_CCS,
	.ifblksize = 8,
	.ifleads = 0,
	.ifbpv = 0,
	.ifalarms = 0,
	.ifrxlevel = 0,
	.iftxlevel = 1,
	.ifsync = 0,
	.ifsyncsrc = {0, 0, 0, 0}
    },
    .sdt = {
	.Tin = 4,
	.Tie = 1,
	.T = 64,
	.D = 256,
	.t8 = 100,
	.Te = 73544,
	.De = 11328000,
	.Ue = 198384000,
	.N = 16,
	.m = 272,
	.b = 8,
	.f = SDT_FLAGS_ONE,
    },
    .sl = {
	.t1 = 45 * 1000,
	.t2 = 5 * 1000,
	.t2l = 20 * 1000,
	.t2h = 100 * 1000,
	.t3 = 1 * 1000,
	.t4n = 8 * 1000,
	.t4e = 500,
	.t5 = 100,
	.t6 = 4 * 1000,
	.t7 = 1 * 1000,
	.rb_abate = 3,
	.rb_accept = 6,
	.rb_discard = 9,
	.tb_abate_1 = 128 * 272,
	.tb_onset_1 = 256 * 272,
	.tb_discd_1 = 384 * 272,
	.tb_abate_2 = 512 * 272,
	.tb_onset_2 = 640 * 272,
	.tb_discd_2 = 768 * 272,
	.tb_abate_3 = 896 * 272,
	.tb_onset_3 = 1024 * 272,
	.tb_discd_3 = 1152 * 272,
	.N1 = 127,
	.N2 = 8192,
	.M = 5,
    },
};

struct link_config ansi_default = {
    .sdl = {
	.ifname = NULL,
	.ifflags = 0,
	.iftype = SDL_TYPE_DS0,
	.ifrate = 64000,
	.ifgtype = SDL_GTYPE_T1,
	.ifgrate = 1544000,
	.ifmode = SDL_MODE_PEER,
	.ifgmode = SDL_GMODE_NONE,
	.ifgcrc = SDL_GCRC_CRC6,
	.ifclock = SDL_CLOCK_LOOP,
	.ifcoding = SDL_CODING_B8ZS,
	.ifframing = SDL_FRAMING_ESF,
	.ifblksize = 8,
	.ifleads = 0,
	.ifbpv = 0,
	.ifalarms = 0,
	.ifrxlevel = 0,
	.iftxlevel = 0,
	.ifsync = 0,
	.ifsyncsrc = {0, 0, 0, 0}
    },
    .sdt = {
	.Tin = 4,
	.Tie = 1,
	.T = 64,
	.D = 256,
	.t8 = 100,
	.Te = 577169,
	.De = 9308000,
	.Ue = 144292000,
	.N = 16,
	.m = 272,
	.b = 8,
	.f = SDT_FLAGS_ONE,
    },
    .sl = {
	.t1 = 45 * 1000,
	.t2 = 5 * 1000,
	.t2l = 20 * 1000,
	.t2h = 100 * 1000,
	.t3 = 1 * 1000,
	.t4n = 8 * 1000,
	.t4e = 500,
	.t5 = 100,
	.t6 = 4 * 1000,
	.t7 = 1 * 1000,
	.rb_abate = 3,
	.rb_accept = 6,
	.rb_discard = 9,
	.tb_abate_1 = 128 * 272,
	.tb_onset_1 = 256 * 272,
	.tb_discd_1 = 384 * 272,
	.tb_abate_2 = 512 * 272,
	.tb_onset_2 = 640 * 272,
	.tb_discd_2 = 768 * 272,
	.tb_abate_3 = 896 * 272,
	.tb_onset_3 = 1024 * 272,
	.tb_discd_3 = 1152 * 272,
	.N1 = 127,
	.N2 = 8192,
	.M = 5,
    },
};

struct link_config jttc_default = {
    .sdl = {
	.ifname = NULL,
	.ifflags = 0,
	.iftype = SDL_TYPE_DS0,
	.ifrate = 64000,
	.ifgtype = SDL_GTYPE_T1,
	.ifgrate = 1544000,
	.ifmode = SDL_MODE_PEER,
	.ifgmode = SDL_GMODE_NONE,
	.ifgcrc = SDL_GCRC_CRC6J,
	.ifclock = SDL_CLOCK_LOOP,
	.ifcoding = SDL_CODING_B8ZS,
	.ifframing = SDL_FRAMING_ESF,
	.ifblksize = 8,
	.ifleads = 0,
	.ifbpv = 0,
	.ifalarms = 0,
	.ifrxlevel = 0,
	.iftxlevel = 0,
	.ifsync = 0,
	.ifsyncsrc = {0, 0, 0, 0}
    },
    .sdt = {
	.Tin = 4,
	.Tie = 1,
	.T = 64,
	.D = 256,
	.t8 = 100,
	.Te = 577169,
	.De = 9308000,
	.Ue = 144292000,
	.N = 16,
	.m = 272,
	.b = 8,
	.f = SDT_FLAGS_ONE,
    },
    .sl = {
	.t1 = 45 * 1000,
	.t2 = 5 * 1000,
	.t2l = 20 * 1000,
	.t2h = 100 * 1000,
	.t3 = 1 * 1000,
	.t4n = 8 * 1000,
	.t4e = 500,
	.t5 = 100,
	.t6 = 4 * 1000,
	.t7 = 1 * 1000,
	.rb_abate = 3,
	.rb_accept = 6,
	.rb_discard = 9,
	.tb_abate_1 = 128 * 272,
	.tb_onset_1 = 256 * 272,
	.tb_discd_1 = 384 * 272,
	.tb_abate_2 = 512 * 272,
	.tb_onset_2 = 640 * 272,
	.tb_discd_2 = 768 * 272,
	.tb_abate_3 = 896 * 272,
	.tb_onset_3 = 1024 * 272,
	.tb_discd_3 = 1152 * 272,
	.N1 = 127,
	.N2 = 8192,
	.M = 5,
    },
};

struct select {
    char device[MAX_BUF_SIZE];
    char clei[MAX_BUF_SIZE];
    int type;
    int card;
    int span;
    int slot;
    int level;
    int state;
    int fd;
    uint32_t gppa;
    uint32_t ppa;
    uint32_t pvar;
    uint32_t popt;
    struct link_config config;
    struct str_list modules;
    struct str_mlist module_list[8];
    int num_addrs;
    int num_laddrs;
    int num_raddrs;
    struct sockaddr_in addrs[8];
};

int handle = 0;

struct select cur = { "/dev/x400p-sl", "", 1, 1, 1, 0, -1, -1 };
struct select sel[MAX_HANDLES] = {
    [0x00] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x01] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x02] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x03] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x04] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x05] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x06] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x07] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x08] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x09] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x0a] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x0b] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x0c] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x0d] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x0e] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x0f] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x10] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x11] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x12] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x13] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x14] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x15] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x16] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x17] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x18] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x19] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x1a] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x1b] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x1c] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x1d] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x1e] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
    [0x1f] = {"/dev/x400p-sl", "", 1, 1, 1, 1, 0, -1, -1},
};

#undef TOK_T1
#define TOK_T1 TOK_T1T
#undef TOK_T2
#define TOK_T2 TOK_T2T
#undef TOK_T3
#define TOK_T3 TOK_T3T

%}

/* BISON DECLARATIONS */

%debug
%defines
%locations
%token_table
%verbose

%union {
    int val;
    char *str;
}

/* %token_table */

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
%token <val> TOK_REMOVE
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
%token <val> TOK_PPATYPE
%token <val> TOK_PIPE
%token <val> TOK_ALL
%token <val> TOK_NONE
%token <val> TOK_CLEI
%token <val> TOK_CLLI
%token <val> TOK_ITUT
%token <val> TOK_ETSI
%token <val> TOK_ANSI
%token <val> TOK_JTTC
%token <val> TOK_CHIN
%token <val> TOK_SING
%token <val> TOK_SPAIN
%token <val> TOK_PVAR
%token <val> TOK_STDGRP
%token <val> TOK_MPLEV
%token <val> TOK_PCR
%token <val> TOK_HSL
%token <val> TOK_XSN
%token <val> TOK_NOPR
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
%token <val> TOK_T2
%token <val> TOK_T3
%token <val> TOK_E2
%token <val> TOK_E3
%token <val> TOK_OC3
%token <val> TOK_OC12
%token <val> TOK_OC48
%token <val> TOK_OC192
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
%token <val> TOK_D4
%token <val> TOK_RXLEVEL
%token <val> TOK_TXLEVEL
%token <val> TOK_SYNCSRC
%token <val> TOK_N1
%token <val> TOK_N2
%token <val> TOK_M
%token <val> TOK_RB_ABATE
%token <val> TOK_RB_ACCEPT
%token <val> TOK_RB_DISCARD
%token <val> TOK_TB_ABATE_1
%token <val> TOK_TB_ONSET_1
%token <val> TOK_TB_DISCD_1
%token <val> TOK_TB_ABATE_2
%token <val> TOK_TB_ONSET_2
%token <val> TOK_TB_DISCD_2
%token <val> TOK_TB_ABATE_3
%token <val> TOK_TB_ONSET_3
%token <val> TOK_TB_DISCD_3
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
%token <val> TOK_PPA
%token <val> TOK_YEAR
%token <val> TOK_DEVICETYPE
%token <val> TOK_SEMICOLON
%token <val> TOK_COLON
%token <val> TOK_EQUAL
%token <val> TOK_COMMA
%token <val> TOK_DASH
%token <val> TOK_PERIOD
%token <str> TOK_COMMENT
%token <str> TOK_WHITESPACE
%token <val> TOK_NUMBER
%token <str> TOK_STRING

%left TOK_SEMICOLON
%left TOK_WHITESPACE
%left TOK_COMMA
%left TOK_COLON
%left TOK_EQUAL
%left TOK_PERIOD
%left TOK_DASH

%type <val> file_handle card_selection span_selection slot_selection global_ppa profile options ppa
%type <str> device_selection clei device

/* GRAMMAR RULES */

%%

lines:
    line
    | lines line
    ;

line:
    line_ending
    | command line_ending
    | error line_ending { yyerrok; }
    ;

line_ending:
    TOK_EOL { newline(); }
    | TOK_SEMICOLON
    | TOK_COMMENT line_ending
    /* | TOK_WHITESPACE line_ending */
    ;

command:
    help_command
    | usage_command
    | copying_command
    | version_command
    | quit_command
    | clear_command
    | debug_command
    | link_command
    | manage_command
    /* | TOK_WHITESPACE command */
    ;

usage_command:
    TOK_USAGE { help(prompt); }
    ;

copying_command:
    TOK_COPYING { copying(prompt); }
    ;

version_command:
    TOK_VERSION { version(prompt); }
    ;

clear_command:
    TOK_CLEAR { fprintf(stdout, "\f"); }
    ;

help_command:
    TOK_HELP
    {
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
", prompt);
    }
    | TOK_HELP /* TOK_WHITESPACE */ what
    ;

what:
    TOK_USAGE
    {
	help(prompt);
    }
    | TOK_VERSION
    {
	version(prompt);
    }
    | TOK_COPYING
    {
	copying(prompt);
    }
    | TOK_OPEN
    {
	fprintf(stdout, "\
\n\
  open [NUMBER] [device DEVNAME]\n\
\n\
    - Opens the specified or selected DEVNAME to obtain a signalling link\n\
      stream and associates it with stream NUMBER.  NUMBER must be a integer\n\
      between 0 and %2$d inclusive.  DEVNAME should be the full or relative\n\
      pathname to a character device special file corresponding to the STREAMS\n\
      driver to open an instance.\n\
\n\
    - If NUMBER is not specified, then the most recent handle will be used, or\n\
      number `0' if no other handle has been used.  The current default is\n\
      `%3$d'.\n\
\n\
    - If DEVNAME is not specified, then the most recent device for the handle\n\
      NUMBER will be used, or \"/dev/x400p-sl\" if no other device has been\n\
      used for the handle.  The current default is \"%4$.64s\".\n\
\n\
    - Example:\n\
\n\
      %1$s> open 3 device /dev/x400p-sl\n\
\n\
      The above example opens the device \"/dev/x400p-sl\" and, if successful,\n\
      associates it with stream number `3'.  If an error occurs, a diagnostic\n\
      will be printed.\n\
\n\
      %1$s> open\n\
\n\
      The above example opens device \"%4$.64s\" and, if successful associates\n\
      it with stream number `%3$d'.  If an error occurs, a diagnostic is\n\
      printed.\n\
\n\
", prompt, MAX_HANDLES - 1, handle, sel[handle].device);
    }
    | TOK_CLOSE
    {
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
    }
    | TOK_ATTACH
    {
	fprintf(stdout, "\
\n\
  attach [NUMBER] [type TYPE] [card CARD] [span SPAN] [slot SLOT]\n\
  attach [NUMBER] [clei CLEI]\n\
\n\
    - Attaches the currently open device stream, NUMBER, to the selected or\n\
      specified type, card, span and slot number.  TYPE is the type of card,\n\
      or driver type and is 1, 2, or 3; CARD is the number of the card, from 1\n\
      to 32; SPAN is the span number on the card, from 1 to 4; SLOT is the\n\
      timeslot number within the span; from 1 to 24 for T1 or J1, and from 1\n\
      to 31 for E1.\n\
\n\
    - Alternately, attaches the currently open device stream, NUMBER, to the\n\
      specified common language equipment identifer, CLEI.\n\
\n\
    - The defaults are TYPE=x400p, CARD=1, SPAN=1, SLOT=1.\n\
\n\
    - Example:\n\
\n\
      %1$s> attach type x400p card 1 span 1 slot 19\n\
\n\
", prompt);
    }
    | TOK_DETACH
    {
	fprintf(stdout, "\
\n\
  detach [NUMBER]\n\
\n\
    - Detaches the currently attached signalling link.  This has the effect of\n\
      issuing the local management detach request to the attached signalling\n\
      link.\n\
\n\
    - Example:\n\
\n\
      %1$s> detach\n\
\n\
", prompt);
    }
    | TOK_ENABLE
    {
	fprintf(stdout, "\
\n\
  enable [NUMBER]\n\
\n\
    - Enables the currently attached signalling link.  This  has the effect of\n\
      issuing the local management enable request to the attached signalling\n\
      link.\n\
\n\
    - Example:\n\
\n\
      %1$s> enable\n\
\n\
", prompt);
    }
    | TOK_DISABLE
    {
	fprintf(stdout, "\
\n\
  disable [NUMBER]\n\
\n\
    - Disable the currently enabled signallingn link.  This has the effect of\n\
      issuing the local management disable request to the enabled signalling\n\
      link.\n\
\n\
    - Example:\n\
\n\
      %1$s> disable\n\
\n\
", prompt);
    }
    | TOK_START
    {
	fprintf(stdout, "\
\n\
  start [NUMBER]\n\
\n\
    - Starts the currently enabled signalling link.  This has the effect of\n\
      issuing the MTP Level 2 start command to the enabled signalling link.\n\
\n\
    - Example:\n\
\n\
      %1$s> start\n\
\n\
", prompt);
    }
    | TOK_STOP
    {
	fprintf(stdout, "\
\n\
  stop [NUMBER]\n\
\n\
    - Stops the currently enabled signalling link.  This has the effect of\n\
      issuing the MTP Level 2 stop command to the enabled signalling link.\n\
\n\
    - Example:\n\
\n\
      %1$s> stop\n\
\n\
", prompt);
    }
    | TOK_EMERGENCY
    {
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
    }
    | TOK_NORMAL
    {
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
    }
    | TOK_MSU
    {
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
    }
    | TOK_ADD
    {
	fprintf(stdout, "\
\n\
  add CLEI GPPA OPTS DEVTYPE DEVNAME MODS PPA OVERRIDES\n\
\n\
    - Add a newly created signalling link to the signalling link multiplexer\n\
      with the specified common language equipment identifier (CLEI) and global\n\
      physical point of attachment (GPPA).\n\
\n\
    - CLEI is any unique user-assigned string.\n\
\n\
    - GPPA is a global physical point of attachment formatted as DEVTYPE:PPA.\n\
\n\
    - PROF identifies the base protocol settings, and is formatted as\n\
      STD-BODY:STD-YEAR.  STD-BODY can be one of:\n\
\n\
	itut	- International Telecomunications Union\n\
	ansi	- American National Standards Institute\n\
	etsi	- European Telecommunications Standards Institute\n\
	jttc	- Japan Telecommunications Technology Commitee\n\
	chin	- China Telecom\n\
	sing	- Singapore\n\
	span	- Spain\n\
\n\
    - OPTS is a comma separated list of protocol options, or the special value\n\
      \"none\".  Options in the comma separated list can be one of the\n\
      following:\n\
\n\
	mplev	- multiple protoocl levels\n\
	pcr	- preventative cyclic retransmission\n\
	hsl	- high speed links\n\
	xsn	- extended sequence number\n\
	nopr	- no proving\n\
\n\
    - DEVTYPE can be one of:\n\
\n\
	x400p	- the x400p-ss7 driver\n\
	m2pa	- the M2PA signalling link module\n\
	m2ua	- the M2UA AS driver\n\
	pipe	- a pipe with modules\n\
	ppa	- any driver accepting normal card:span:slot PPA\n\
\n\
    - DEVNAME is the name of a device file to open to obtain an instance of the\n\
      signalling link.\n\
\n\
    - MODS is a comma separated list of modules to push, or the special value\n\
      \"none\".\n\
\n\
    - PPA is the physical point of attachment to which to attach, formatted as\n\
      CARD:SPAN:SLOT, where CARD, SPAN and SLOT are card number, span number on\n\
      the card, and timeslot in the span.\n\
\n\
    - OVERRIDES is a comma or space separated list of parameter value pairs\n\
      concatenated with an equal sign, or the special value \"none\".  These\n\
      values with override the corresponding value from the PROF.\n\
\n\
    - Example:\n\
\n\
      %1$s> add EDTNAB01DS1SL0 x400p:1:1:1 itut:88 none x400p \\\n\
	    /dev/x400p-sl none 1:1:1 t6=125\n\
\n\
", prompt);
    }
    | TOK_X400P
    {
	fprintf(stdout, "\
\n\
  add CLEI GPPA PROF OPTS x400p DEVNAME MODS PPA OVERRIDES\n\
\n\
    - Adds a newly created X400P signalling link to the signalling link\n\
      multiplexer with the specified common language equipment identifier (CLEI)\n\
      and global physical point of attachment (GPPA).\n\
\n\
    - CLEI is any unique user-assigned string.\n\
\n\
    - GPPA is a global physical point of attachment formated as DEVTYPE:PPA.\n\
\n\
    - PROF identifies the base protocol setting, and is formatted as\n\
      STD-BODY:STD-YEAR.  STD-BODY and STD-YEAR can be one of:\n\
\n\
	itut	ansi	etsi	jttc	chin	sing	span\n\
	1988	1992	1993	1996	1997	2000	2004\n\
\n\
    - OPTS is a comma separated list of options, or the special value \"none\".\n\
      Options in the comma separated list can be one of the following:\n\
\n\
	mplev	pcr	hsl	xsn	nopr\n\
\n\
    - DEVTYPE for X400P signalling links is always \"x400p\".\n\
\n\
    - DEVNAME is the name of the device file to open to obtain an instance of\n\
      the signalling link.  For X400P, usually \"/dev/x400p-sl\".\n\
\n\
    - MODS is a comma separated list of modules to push, or \"none\".  For\n\
      X400P, usually \"none\".\n\
\n\
    - PPA is the phsyical point of attachment to which to attach, formatted as\n\
      CARD:SPAN:SLOT, where CARD, SPAN and SLOT are card number, span number on\n\
      the card, and timeslot in the span (or zero for whole-span).\n\
\n\
    - OVERRIDES is a comma separated list of parameter value pairs concatenated\n\
      with an equal sign, or the special value \"none\".  These values will\n\
      override the corresponding value from the profile, PROF.\n\
\n\
    - Example:\n\
\n\
      %1$s> add EDTNAB01DS1SL0 x400p:1:1:1 itut:92 none, x400p \\\n\
		/dev/x400p-sl none 1:1:1 none\n\
\n\
", prompt);
    }
    | TOK_M2PA
    {
	fprintf(stdout, "\
\n\
  add CLEI GPPA PROF OPTS m2pa DEVNAME MODS LOCAL REMOTE OVERRIDES\n\
\n\
    - Adds a newly created M2PA signalling link to the signalling link\n\
      multiplexing driver with the specified CLEI and GPPA.\n\
\n\
    - CLEI is any unique user-assigned common language equipment identifier\n\
      string.\n\
\n\
    - GPPA is a global physical point of attachement formatted as a device type\n\
      and a physical point of attachment: DEVTYPE:PPA.\n\
\n\
    - PROF identifies the base protocol setting, and is formatted as\n\
      STD-BODY:STD-YEAR.  STD-BODY and STD-YEAR can be one of:\n\
\n\
	itut	ansi	etsi	jttc	chin	sing	span\n\
	1988	1992	1993	1996	1997	2000	2004\n\
\n\
    - OPTS is a comma separated list of options, or the special value \"none\".\n\
      Options in the comma separated list can be one of the following:  (Note\n\
      that \"nopr\" is of particular importance to M2PA as it determines whether\n\
      proving is performed or not for the M2PA link.)\n\
\n\
	mplev	pcr	hsl	xsn	nopr\n\
\n\
    - DEVTYPE for M2PA signalling links is always \"m2pa\".\n\
\n\
    - DEVNAME is the name of the device file to open to obtain an instance of\n\
      the signalling link.  For M2PA, usually \"/dev/sctp_n\".\n\
\n\
    - MODS is a comma separated list of modules to push, or \"none\".  For\n\
      M2PA, usually \"m2pa-sl\".\n\
\n\
    - LOCAL is the local address(es) to which to attach the signalling link.\n\
      REMOTE is the remote address(es) to which to enable the signalling link.\n\
      These addresses are formatted as {HOSTNAME|D.D.D.D[,D.D.D.D]*}[:PORT].\n\
\n\
    - OVERRIDES is a comma separated list of parameter value pairs concatenated\n\
      with an equal sign, or the special value \"none\".  These values will\n\
      override the corresponding value from the profile, PROF.\n\
\n\
    - Example:\n\
\n\
      %1$s> add EDTNAB01DS1SL0 m2pa:1:1:1 itut:1992 none, m2pa /dev/sccp_n \\\n\
		m2pa-sl 127.0.0.1,127.0.0.2:0 ns.pigworks.openss7.net:200 none\n\
\n\
", prompt);
    }
    | TOK_M2UA
    {
	fprintf(stdout, "\
\n\
  add CLEI GPPA PROF OPTS m2ua DEVNAME MODS IID LOCAL REMOTE OVERRIDES\n\
\n\
    - Adds a newly created M2UA signalling link to the signalling link\n\
      multiplexing driver for the specified CLEI and GPPA.\n\
\n\
    - CLEI is a user-assigned common language equipment identifier string.\n\
\n\
    - GPPA is a global physical point of attachment, formatted as device type\n\
      and physical point of attachment concatentanted: DEVTYPE:PPA.\n\
\n\
    - PROF identifies the base protocol setting, and is formatted as\n\
      STD-BODY:STD-YEAR.  STD-BODY and STD-YEAR can be one of:\n\
\n\
	itut	ansi	etsi	jttc	chin	sing	span\n\
	1988	1992	1993	1996	1997	2000	2004\n\
\n\
    - OPTS is a comma separated list of options, or the special value \"none\".\n\
      Options in the comma separated list can be one of the following:\n\
\n\
	mplev	pcr	hsl	xsn	nopr\n\
\n\
    - DEVTYPE for M2UA is always \"m2ua\".\n\
\n\
    - DEVNAME is the name of the device file to open to obtain an instance of\n\
      the signalling link.  For M2UA, this normall \"/dev/sctp_n\".\n\
\n\
    - MODS is a comma separated list of modules to push, or \"none\".  For M2UA,\n\
      the \"m2ua-as\" module is pushed.\n\
\n\
    - IID is the interface identifier to use when talking to the SG, if\n\
      different from the PPA.  (Note that the CLEI will be used as a text\n\
      interface identifier.)\n\
\n\
    - LOCAL is the local address(es) to which to attach the signalling link.\n\
      REMOTE is the remote address(es) to which to enable the signalling link.\n\
      These addresses are formatted as {HOSTNAME|D.D.D.D[,D.D.D.D]*}[:PORT].\n\
\n\
    - OVERRIDES is a comma separated list of parameter value pairs concatenated\n\
      with an equal sign, or the special value \"none\".  These values will\n\
      override the corresponding value from the profile, PROF.\n\
\n\
    - Example:\n\
\n\
      %1$s> add EDTNAB01DS1SL0 m2ua:1:1:1 itut:92 nopr m2ua /dev/sccp_n \\\n\
		m2ua-as 7 127.0.0.1,127.0.0.2:0 ns.pigworks.openss7.net:200 none\n\
\n\
", prompt);
    }
    | TOK_PIPE
    {
	fprintf(stdout, "\
\n\
  add CLEI GPPA PROF OPTS pipe DEVNAME MODS PPA OVERRIDES\n\
\n\
    - Example:\n\
\n\
      %1$s> add EDTNAB01DS1SL0 pipe:1:1:1 itut:92 none, pipe \\\n\
                /dev/pipe-sl none 1:1:1 none\n\
\n\
", prompt);
    }
    | TOK_PPATYPE
    {
	fprintf(stdout, "\
\n\
  add CLEI GPPA PROF OPTS ppa DEVNAME MODS PPA OVERRIDES\n\
\n\
    - Example:\n\
\n\
      %1$s> add EDTNAB01DS1SL0 ppa:1:1:1 itut:92 none, ppa \\\n\
                /dev/ppa-sl none 1:1:1 none\n\
\n\
", prompt);
    }
    | TOK_CHA
    {
	fprintf(stdout, "\
\n\
  cha {CLEI|GPPA} [PROF] [OPTS] [OVERRIDES]\n\
\n\
    - Changes the protocol profile, options, or overrides associated with a\n\
      signalling link sepcified by CLEI or GPPA.\n\
\n\
    - CLEI is a user defined string that was associated with the signalling link\n\
      with the \"add\" command.\n\
\n\
    - GPPA is the colon separated concatenation of the DEVTYPE and PPA,\n\
      formatted as DEVTYPE:CARD:SPAN:SLOT.\n\
\n\
    - Example:\n\
\n\
      %1$s> cha\n\
\n\
", prompt);
    }
    | TOK_DEL
    {
	fprintf(stdout, "\
\n\
  del [{CLEI|GPPA}[ {CLEI|GPPA}]*]\n\
\n\
    - Deletes either the specified space separated list of CLIE or GPPA\n\
      specifications indicating the signalling links to be deleted, or, if no\n\
      CLEI or GPPA is specified, all signalling links known to the signalling\n\
      link multiplexer.\n\
\n\
    - CLEI is a user defined string that was associated with the signalling link\n\
      with the \"add\" command.\n\
\n\
    - GPPA is the colon separated concatenation of the DEVTYPE and PPA,\n\
      formatted as DEVTYPE:CARD:SPAN:SLOT.\n\
\n\
    - Example:\n\
\n\
      %1$s> del EDTNAB01DS1SL0 x400p:1:1:1\n\
\n\
", prompt);
    }
    | TOK_GET
    {
	fprintf(stdout, "\
\n\
  get [{CLEI|GPPA}[ {CLEI|GPPA}]*]\n\
\n\
    - Gets and displays either the specified space separated list of CLEI or\n\
      GPPA specifications indicating the signalling links to get and display,\n\
      or, if no CLEI or GPPA is specified, all signalling links known to the\n\
      signalling link multiplexer.\n\
\n\
    - CLEI is a user defined string that was associated with the signalling link\n\
      with the \"add\" command.\n\
\n\
    - GPPA is the colon separated concatenation of the DEVTYPE and PPA,\n\
      formatted as DEVTYPE:CARD:SPAN:SLOT.\n\
\n\
    - Example:\n\
\n\
      %1$s> get EDTNAB01DS1SL0 EDTNAB01DS1SL1\n\
\n\
", prompt);
    }
    | TOK_REMOVE
    {
	fprintf(stdout, "\
\n\
  remove [{CLEI|GPPA}[ {CLEI|GPPA}]*]\n\
\n\
    - Removes either the specified space separated list of CLIE or GPPA\n\
      specifications indicating the signalling links to be removed, or, if no\n\
      CLEI or GPPA is specified, all signalling links known to the signalling\n\
      link multiplexer.\n\
\n\
    - CLEI is a user defined string that was associated with the signalling link\n\
      with the \"add\" command.\n\
\n\
    - GPPA is the colon separated concatenation of the DEVTYPE and PPA,\n\
      formatted as DEVTYPE:CARD:SPAN:SLOT.\n\
\n\
    - Example:\n\
\n\
      %1$s> remove EDTNAB01DS1SL0 x400p:1:1:1\n\
\n\
", prompt);
    }
    | TOK_SET
    {
	fprintf(stdout, "\
\n\
  set {CLEI|GPPA} [PROF] [OPTS] [OVERRIDES]\n\
\n\
    - Sets the protocol profile, options, or overrides associated with a\n\
      signalling link sepcified by CLEI or GPPA.\n\
\n\
    - CLEI is a user defined string that was associated with the signalling link\n\
      with the \"add\" command.\n\
\n\
    - GPPA is the colon separated concatenation of the DEVTYPE and PPA,\n\
      formatted as DEVTYPE:CARD:SPAN:SLOT.\n\
\n\
    - Example:\n\
\n\
      %1$s> set\n\
\n\
", prompt);
    }
    | error
    {
	yyerror("no help for that topic");
	YYERROR;
    }
    ;

debug_command:
     TOK_DEBUG /* TOK_WHITESPACE */ TOK_NUMBER
     {
	 yydebug = $<val>2;
	 fprintf(stdout, "debug level is now set to %d\n", $<val>2);
     }
     ;

quit_command:
    TOK_QUIT
    {
	YYABORT;
    }
    ;

manage_command:
    TOK_ADD add_command
    {
	int i;
	fprintf(stdout, "\nAdd command:\n\n");
	fprintf(stdout, "CLEI............. %s\n", cur.clei);
	fprintf(stdout, "Global PPA....... %08x (card %d, span %d, slot %d)\n", cur.gppa,
	    (cur.gppa >> 16) & 0xff, (cur.gppa >> 8) & 0xff, cur.gppa & 0xff);
	fprintf(stdout, "Profile.......... %08x\n", cur.pvar);
	fprintf(stdout, "Protocol opts.... %08x\n", cur.popt);
	fprintf(stdout, "Device type...... %d\n", cur.type);
	fprintf(stdout, "Device........... %s\n", cur.device);
	for (i = 0; i < cur.modules.sl_nmods; i++)
	    fprintf(stdout, "Module[%d]........ %s\n", i, cur.module_list[i].l_name);
	fprintf(stdout, "PPA.............. %08x (card %d, span %d, slot %d)\n", cur.ppa,
	    (cur.ppa >> 16) & 0xff, (cur.ppa >> 8) & 0xff, cur.ppa & 0xff);
	fprintf(stdout, "\n");
	for (i = 0; i < cur.num_laddrs; i++)
	    fprintf(stdout, "Loc addr[%d]...... %d.%d.%d.%d:%d\n", i + 1,
		(int)((cur.addrs[i].sin_addr.s_addr >>  0) & 0xff),
		(int)((cur.addrs[i].sin_addr.s_addr >>  8) & 0xff),
		(int)((cur.addrs[i].sin_addr.s_addr >> 16) & 0xff),
		(int)((cur.addrs[i].sin_addr.s_addr >> 24) & 0xff),
		(int)((cur.addrs[i].sin_port)));
	for (i = cur.num_laddrs; i < cur.num_addrs; i++)
	    fprintf(stdout, "Rem addr[%d]...... %d.%d.%d.%d:%d\n", i - cur.num_laddrs + 1,
		(int)((cur.addrs[i].sin_addr.s_addr >>  0) & 0xff),
		(int)((cur.addrs[i].sin_addr.s_addr >>  8) & 0xff),
		(int)((cur.addrs[i].sin_addr.s_addr >> 16) & 0xff),
		(int)((cur.addrs[i].sin_addr.s_addr >> 24) & 0xff),
		(int)((cur.addrs[i].sin_port)));
	fprintf(stdout, "\n");
    fprintf(stdout, "iftype......... %'9d %-9s", (int) cur.config.sdl.iftype, "");
	fprintf(stdout, "Timer T1....... %'9d %-9s\n", (int) cur.config.sl.t1, "msec");
    fprintf(stdout, "ifrate......... %'9d %-9s", (int) cur.config.sdl.ifrate, "bps");
	fprintf(stdout, "Timer T2....... %'9d %-9s\n", (int) cur.config.sl.t2, "msec");
    fprintf(stdout, "ifgtype........ %'9d %-9s", (int) cur.config.sdl.ifgtype, "");
	fprintf(stdout, "Timer T2l...... %'9d %-9s\n", (int) cur.config.sl.t2l, "msec");
    fprintf(stdout, "ifgrate........ %'9d %-9s", (int) cur.config.sdl.ifgrate, "bps");
	fprintf(stdout, "Timer T2h...... %'9d %-9s\n", (int) cur.config.sl.t2h, "msec");
    fprintf(stdout, "ifmode......... %'9d %-9s", (int) cur.config.sdl.ifmode, "");
	fprintf(stdout, "Timer T3....... %'9d %-9s\n", (int) cur.config.sl.t3, "msec");
    fprintf(stdout, "ifgmode........ %'9d %-9s", (int) cur.config.sdl.ifgmode, "");
	fprintf(stdout, "Timer T4n...... %'9d %-9s\n", (int) cur.config.sl.t4n, "msec");
    fprintf(stdout, "ifgcrc......... %'9d %-9s", (int) cur.config.sdl.ifgcrc, "");
	fprintf(stdout, "Timer T4e...... %'9d %-9s\n", (int) cur.config.sl.t4e, "msec");
    fprintf(stdout, "ifclock........ %'9d %-9s", (int) cur.config.sdl.ifclock, "");
	fprintf(stdout, "Timer T5....... %'9d %-9s\n", (int) cur.config.sl.t5, "msec");
    fprintf(stdout, "ifcoding....... %'9d %-9s", (int) cur.config.sdl.ifcoding, "");
	fprintf(stdout, "Timer T6....... %'9d %-9s\n", (int) cur.config.sl.t6, "msec");
    fprintf(stdout, "ifframing...... %'9d %-9s", (int) cur.config.sdl.ifframing, "");
	fprintf(stdout, "Timer T7....... %'9d %-9s\n", (int) cur.config.sl.t7, "msec");
    fprintf(stdout, "ifblksize...... %'9d %-9s", (int) cur.config.sdl.ifblksize, "bytes");
	fprintf(stdout, "RB Abate....... %'9d %-9s\n", (int) cur.config.sl.rb_abate, "msgs");
    fprintf(stdout, "ifrxlevel...... %'9d %-9s", (int) cur.config.sdl.ifrxlevel, "");
	fprintf(stdout, "RB Accept...... %'9d %-9s\n", (int) cur.config.sl.rb_accept, "msgs");
    fprintf(stdout, "iftxlevel...... %'9d %-9s", (int) cur.config.sdl.iftxlevel, "");
	fprintf(stdout, "RB Discard..... %'9d %-9s\n", (int) cur.config.sl.rb_discard, "msgs");
    fprintf(stdout, "ifsync......... %'9d %-9s", (int) cur.config.sdl.ifsync, "");
	fprintf(stdout, "TB Abate 1..... %'9d %-9s\n", (int) cur.config.sl.tb_abate_1, "octs");
    fprintf(stdout, "Tin............ %'9d %-9s", (int) cur.config.sdt.Tin, "msgs");
	fprintf(stdout, "TB Onset 1..... %'9d %-9s\n", (int) cur.config.sl.tb_onset_1, "octs");
    fprintf(stdout, "Tie............ %'9d %-9s", (int) cur.config.sdt.Tie, "msgs");
	fprintf(stdout, "TB Discard 1... %'9d %-9s\n", (int) cur.config.sl.tb_discd_1, "octs");
    fprintf(stdout, "T.............. %'9d %-9s", (int) cur.config.sdt.T, "msgs");
	fprintf(stdout, "TB Abate 2..... %'9d %-9s\n", (int) cur.config.sl.tb_abate_2, "octs");
    fprintf(stdout, "D.............. %'9d %-9s", (int) cur.config.sdt.D, "msgs");
	fprintf(stdout, "TB Onset 2..... %'9d %-9s\n", (int) cur.config.sl.tb_onset_2, "octs");
    fprintf(stdout, "Timer T8....... %'9d %-9s", (int) cur.config.sdt.t8, "msec");
	fprintf(stdout, "TB Discard 2... %'9d %-9s\n", (int) cur.config.sl.tb_discd_2, "octs");
    fprintf(stdout, "Te............. %'9d %-9s", (int) cur.config.sdt.Te, "bits");
	fprintf(stdout, "TB Abate 3..... %'9d %-9s\n", (int) cur.config.sl.tb_abate_3, "octs");
    fprintf(stdout, "De............. %'9d %-9s", (int) cur.config.sdt.De, "bits");
	fprintf(stdout, "TB Onset 3..... %'9d %-9s\n", (int) cur.config.sl.tb_onset_3, "octs");
    fprintf(stdout, "Ue............. %'9d %-9s", (int) cur.config.sdt.Ue, "bits");
	fprintf(stdout, "TB Discard 3... %'9d %-9s\n", (int) cur.config.sl.tb_discd_3, "octs");
    fprintf(stdout, "N.............. %'9d %-9s", (int) cur.config.sdt.N, "octs");
	fprintf(stdout, "N1............. %'9d %-9s\n", (int) cur.config.sl.N1, "msgs");
    fprintf(stdout, "m.............. %'9d %-9s", (int) cur.config.sdt.m, "octs");
	fprintf(stdout, "N2............. %'9d %-9s\n", (int) cur.config.sl.N2, "octs");
    fprintf(stdout, "b.............. %'9d %-9s", (int) cur.config.sdt.b, "octs");
	fprintf(stdout, "M.............. %'9d %-9s\n", (int) cur.config.sl.M, "tries");
    fprintf(stdout, "f.............. %'9d %-9s", (int) cur.config.sdt.f, "flags");
	fprintf(stdout, "\n\n");
    }
    | TOK_CHA cha_command
    {
	fprintf(stdout, "\nChange command:\n");
    }
    | TOK_DEL del_command
    {
	fprintf(stdout, "\nDelete command:\n");
    }
    | TOK_GET get_command
    {
	fprintf(stdout, "\nGet command:\n");
    }
    | TOK_REMOVE del_command
    {
	fprintf(stdout, "\nRemove command:\n");
    }
    | TOK_SET cha_command
    {
	fprintf(stdout, "\nSet command:\n");
    }
    ;

add_command:
    /* TOK_WHITESPACE */ clei
    /* TOK_WHITESPACE */ global_ppa
    /* TOK_WHITESPACE */ profile
    /* TOK_WHITESPACE */ options
    /* TOK_WHITESPACE */ add_remainder
    {
	cur.popt = $<val>4;
	if (cur.popt & SS7_POPT_HSL) {
	    cur.config.sdl.iftype = cur.config.sdl.ifgtype;
	    cur.config.sdl.ifrate = cur.config.sdl.ifgrate;
	}
	if (cur.popt & SS7_POPT_XSN) {
	    cur.config.sl.N1 = 4095;
	    cur.config.sl.N2 = 65536;
	}
    }
    ;

add_remainder:
    TOK_X400P add_ppa_remainder { $<val>$ = 1; cur.type = 1; }
    | TOK_M2PA add_m2pa_remainder { $<val>$ = 2; cur.type = 2; }
    | TOK_M2UA add_m2ua_remainder { $<val>$ = 3; cur.type = 3; }
    | TOK_PIPE add_pipe_remainder { $<val>$ = 4; cur.type = 4; }
    | TOK_PPATYPE add_ppa_remainder { $<val>$ = 5; cur.type = 5; }
    ;

add_ppa_remainder:
    /* TOK_WHITESPACE */ device
    /* TOK_WHITESPACE */ modules
    /* TOK_WHITESPACE */ ppa
    /* TOK_WHITESPACE */ overrides
    {
	fprintf(stdout, "\nPPA Device Add:\n");
    }
    ;

add_m2pa_remainder:
    /* For m2pa, the device is normally /dev/sctp_n and the modules pushed are normall the m2pa-sl
       module. */
    /* TOK_WHITESPACE */ device
    /* TOK_WHITESPACE */ modules
    /* M2PA does not have a CARD:SPAN:SLOT based PPA, it uses a local set of IP addresses for SCTP
       passed to the bind from the attach.  This is called local here.  Also there is a set of iP
       addresses to be passed to connect form the enable.  This is called remote here. */
    /* TOK_WHITESPACE */ addresses
    /* TOK_WHITESPACE */ overrides
    {
	fprintf(stdout, "\nM2PA Device Add:\n");
    }
    ;

add_m2ua_remainder:
    /* TOK_WHITESPACE */ device
    /* TOK_WHITESPACE */ modules
    /* TOK_WHITESPACE */ ppa
    /* TOK_WHITESPACE */ addresses
    /* TOK_WHITESPACE */ overrides
    {
	fprintf(stdout, "\nM2UA Device Add:\n");
    }
    ;

add_pipe_remainder:
    /* For pipes, device must be /dev/spx or a device that behaves similarly and recognized the
       I_FDINSERT command. */
    /* TOK_WHITESPACE */ device
    /* For pipes, the first module in the module list is pushed on one side of the pipe, and the
       remainder are pushed on both sides of the pipe */
    /* TOK_WHITESPACE */ modules
    /* Pipes do not have PPAs and are STYLE1 devices, but can still have protocol overrides. */
    /* TOK_WHITESPACE */ overrides
    {
	fprintf(stdout, "\nPipe Device Add:\n");
    }
    ;

clei:
    TOK_STRING
    {
	strncpy(cur.clei, $<str>1, sizeof(cur.clei));
	$<str>$ = $<str>1;
    }
    ;

device:
    TOK_STRING
    {
	char *device = $<str>1;
	struct stat buf;
	if (stat(device,&buf) == -1) {
	    yyerror(strerror(errno));
	    YYERROR;
	}
	if (!S_ISCHR(buf.st_mode)) {
	    yyerror("device is not a character device");
	    YYERROR;
	}
	strncpy(cur.device, device, sizeof(cur.device));
	$<str>$ = $<str>1;
    }
    ;

ppa: TOK_NUMBER TOK_COLON TOK_NUMBER TOK_COLON TOK_NUMBER
    {
	if (1 > $<val>1 || $<val>1 > 8) {
	    yyerror("card must be between 1 and 8");
	    YYERROR;
	}
	if (1 > $<val>3 || $<val>3 > 4) {
	    yyerror("span must be between 1 and 4");
	    YYERROR;
	}
	if (0 > $<val>5 || $<val>5 > 31) {
	    yyerror("slot must be 0, or between 1 and 31");
	    YYERROR;
	}
	$<val>$ = cur.ppa = ($<val>1 << 16) | ($<val>3 << 8) | ($<val>5 << 0);
    }
    ;

addresses:
    loc_address /* TOK_WHITESPACE */ rem_address
    { cur.num_laddrs = $<val>1; cur.num_raddrs = $<val>2; cur.num_addrs = $<val>1 + $<val>2; }
    ;

loc_address:
    ip_address_spec ip_port_number
    {
	int i;
	cur.num_laddrs = $<val>1;
	for (i = 0; i < cur.num_laddrs; i++)
	    cur.addrs[i].sin_port = $<val>2;
	$<val>$ = $<val>1;
    }
    ;

rem_address:
    ip_address_spec ip_port_number
    {
	int i;
	cur.num_raddrs = $<val>1;
	for (i = 0; i < cur.num_raddrs; i++)
	    cur.addrs[i + cur.num_laddrs].sin_port = $<val>2;
	$<val>$ = $<val>1;
    }
    ;

ip_address_spec:
    hostname
    | ip_address_list
    ;

ip_address_list:
    ip_address
    | ip_address_list TOK_COMMA ip_address
    {
	$<val>$ = $<val>1 + $<val>3;
    }
    ;

ip_address:
    TOK_NUMBER TOK_PERIOD TOK_NUMBER TOK_PERIOD TOK_NUMBER TOK_PERIOD TOK_NUMBER
    {
	if ($<val>1 < 0 || $<val>1 > 255) {
	    yyerror("portion of IP number out of range");
	    YYERROR;
	}
	if ($<val>3 < 0 || $<val>3 > 255) {
	    yyerror("portion of IP number out of range");
	    YYERROR;
	}
	if ($<val>5 < 0 || $<val>5 > 255) {
	    yyerror("portion of IP number out of range");
	    YYERROR;
	}
	if ($<val>7 < 0 || $<val>7 > 255) {
	    yyerror("portion of IP number out of range");
	    YYERROR;
	}
	if (cur.num_addrs < 8) {
	    cur.addrs[cur.num_addrs].sin_family = AF_INET;
	    cur.addrs[cur.num_addrs].sin_addr.s_addr =
		($<val>1 << 0) | ($<val>3 <<  8) | ($<val>5 << 16) | ($<val>7 << 24);
	    cur.addrs[cur.num_addrs].sin_port = 0; /* for now */
	    cur.num_addrs++;
	    $<val>$ = 1;
	} else {
	    $<val>$ = 0;
	}
    }
    ;

ip_port_number:
    { $<val>$ = 0; }
    | TOK_COLON TOK_NUMBER
    {
	if ($<val>2 < 0 || $<val>2 > ((1<<16) - 1)) {
	    yyerror("invalid port number");
	    YYERROR;
	}
	$<val>$ = $<val>2;
    }
    ;

hostname:
    TOK_STRING
    {
	struct hostent *haddr;
	uint32_t *addr;
	int count = 0;
	haddr = gethostbyname($<str>1);
	if (haddr == NULL) {
	    yyerror("bad host name");
	    YYERROR;
	}
	addr = (uint32_t *)haddr->h_addr;
	while (*addr) {
	    if (cur.num_addrs < 8) {
		cur.addrs[cur.num_addrs].sin_family = AF_INET;
		cur.addrs[cur.num_addrs].sin_addr.s_addr = *addr;
		cur.addrs[cur.num_addrs].sin_port = 0; /* for now */
		cur.num_addrs++;
		count++;
		addr++;
	    }
	}
	$<val>$ = count;
    }
    ;

/* 
    modules:

    A comma separated list of modules to push or the special value "none".
    When the device is a pipe device, the first module on the list will be
    pushed on one side of the pipe only, and the remaining modules will be
    pushed on both sides of the pipe.  If you do not need the first module,
    specify "pipemod" or "nullmod" for the first module.
    
    Modules are verified using the SAD_VML ioctl of the sad(4) driver.
 */
modules:
    modules_spec
    {
	cur.modules.sl_nmods = $<val>1;
	cur.modules.sl_modlist = cur.module_list;
	if (cur.modules.sl_nmods > 0) {
	    int fd, rtn;
	    if ((fd = open("/dev/sad", O_RDWR)) == -1) {
		yyerror(strerror(errno));
		YYERROR;
	    }
	    if ((rtn = ioctl(fd, SAD_VML, &cur.modules)) == -1) {
		close(fd);
		yyerror(strerror(errno));
		YYERROR;
	    }
	    if (rtn != 0) {
		yyerror("invalid module name");
		YYERROR;
	    }
	    close(fd);
	}
    }
    ;

modules_spec:
    TOK_NONE { $<val>$ = 0; }
    | module_list
    ;

module_list:
    module
    | module_list TOK_COMMA module
    {
	$<val>$ = $<val>1 + $<val>3;
    }
    ;

module:
    TOK_STRING
    {
	int n = cur.modules.sl_nmods;
	char *name = cur.module_list[n].l_name;
	if (n < 8) {
	    strncpy(name, $<str>1, FMNAMESZ);
	    $<val>$ = 1;
	    cur.modules.sl_nmods++;
	} else {
	    $<val>$ = 0;
	}
    }
    ;

device_type: 
    TOK_NONE		{ $<val>$ = 0; }
    | TOK_X400P		{ $<val>$ = 1; }
    | TOK_M2PA		{ $<val>$ = 2; }
    | TOK_M2UA		{ $<val>$ = 3; }
    | TOK_PPATYPE	{ $<val>$ = 4; }
    | TOK_PIPE		{ $<val>$ = 5; }
    ;

global_ppa:
    device_type TOK_COLON ppa
    {
	cur.gppa =  ($<val>1 << 24) | $<val>3;
	$<val>$ = cur.gppa;
    }
    ;

profile:
    standard_group TOK_COLON standard_year
    {
	switch ($<val>1) {
	    case SS7_PVAR_JTTC:
		cur.config = jttc_default;
		cur.popt = SS7_POPT_MPLEV;
		break;
	    case SS7_PVAR_ANSI:
		cur.config = ansi_default;
		cur.popt = SS7_POPT_MPLEV;
		break;
	    case SS7_PVAR_ETSI:
	    case SS7_PVAR_ITUT:
	    default:
		cur.config = itut_default;
		cur.popt = 0;
		break;
	}
	if ((cur.gppa & 0x1f) == 0)
	    cur.popt |= SS7_POPT_HSL;
	cur.pvar = $<val>1 | $<val>3;
	$<val>$ = cur.pvar;
    }
    ;

standard_group:
    TOK_ITUT		{ $<val>$ = SS7_PVAR_ITUT; }
    | TOK_ANSI		{ $<val>$ = SS7_PVAR_ANSI; }
    | TOK_ETSI		{ $<val>$ = SS7_PVAR_ETSI; }
    | TOK_JTTC		{ $<val>$ = SS7_PVAR_JTTC; }
    | TOK_CHIN		{ $<val>$ = SS7_PVAR_CHIN; }
    | TOK_SING		{ $<val>$ = SS7_PVAR_SING; }
    | TOK_SPAIN		{ $<val>$ = SS7_PVAR_SPAN; }
    ;

/*
    The standard year can be of the form 1988 or 2004, or can be abbreviated
    as 88 or 04.
 */

standard_year:
    TOK_NUMBER
    {
	int val = $<val>1;
	if (val >= 88 && val <= 99)
	    val += 1900;
	if (val >= 0 && val <= 50)
	    val += 2000;
	if (val < 1988) {
	    yyerror("standard year must be greater than 1987");
	    YYERROR;
	}
	if (val >= 3004) {
	    yyerror("standard year must be less than 3004");
	    YYERROR;
	}
	$<val>$ = (val - 1988) / 4;
    }
    ;

options:
    TOK_NONE		{ $<val>$ = 0; }
    | option_list	{ $<val>$ = $<val>1; }
    ;

option_list:
    option		{ $<val>$ = $<val>1; }
    | option_list TOK_COMMA option
    {
	$<val>$ = $<val>1 | $<val>3;
    }
    ;

option:
    TOK_MPLEV		{ $<val>$ = $<val>1; }
    | TOK_PCR		{ $<val>$ = $<val>1; }
    | TOK_HSL		{ $<val>$ = $<val>1; }
    | TOK_XSN		{ $<val>$ = $<val>1; }
    | TOK_NOPR		{ $<val>$ = $<val>1; }
    ;

overrides:
    TOK_NONE
    | override_list
    ;

override_list:
    override
    | override_list TOK_COMMA override
    | override_list /* TOK_WHITESPACE */ override
    ;

override:
    TOK_TYPE TOK_EQUAL type_value		{ cur.config.sdl.iftype = $<val>3; }
    | TOK_RATE TOK_EQUAL rate_value		{ cur.config.sdl.ifrate = $<val>3; }
    | TOK_GTYPE TOK_EQUAL gtype_value		{ cur.config.sdl.ifgtype = $<val>3; }
    | TOK_GRATE TOK_EQUAL grate_value		{ cur.config.sdl.ifgrate = $<val>3; }
    | TOK_MODE TOK_EQUAL mode_value		{ cur.config.sdl.ifmode = $<val>3; }
    | TOK_GMODE TOK_EQUAL gmode_value		{ cur.config.sdl.ifgmode = $<val>3; }
    | TOK_GCRC TOK_EQUAL gcrc_value		{ cur.config.sdl.ifgcrc = $<val>3; }
    | TOK_CLOCK TOK_EQUAL clock_value		{ cur.config.sdl.ifclock = $<val>3; }
    | TOK_CODING TOK_EQUAL coding_value		{ cur.config.sdl.ifcoding = $<val>3; }
    | TOK_FRAMING TOK_EQUAL framing_value	{ cur.config.sdl.ifframing = $<val>3; }
    | TOK_RXLEVEL TOK_EQUAL rxlevel_value	{ cur.config.sdl.ifrxlevel = $<val>3; }
    | TOK_TXLEVEL TOK_EQUAL txlevel_value	{ cur.config.sdl.iftxlevel = $<val>3; }
    | TOK_T8T TOK_EQUAL TOK_NUMBER		{ cur.config.sdt.t8 = $<val>$; }
    | TOK_TIN TOK_EQUAL TOK_NUMBER		{ cur.config.sdt.Tin = $<val>$; }
    | TOK_TIE TOK_EQUAL TOK_NUMBER		{ cur.config.sdt.Tie = $<val>$; }
    | TOK_T TOK_EQUAL TOK_NUMBER		{ cur.config.sdt.T = $<val>$; }
    | TOK_D TOK_EQUAL TOK_NUMBER		{ cur.config.sdt.D = $<val>$; }
    | TOK_TE TOK_EQUAL TOK_NUMBER		{ cur.config.sdt.Te = $<val>$; }
    | TOK_DE TOK_EQUAL TOK_NUMBER		{ cur.config.sdt.De = $<val>$; }
    | TOK_UE TOK_EQUAL TOK_NUMBER		{ cur.config.sdt.Ue = $<val>$; }
    | TOK_T1T TOK_EQUAL TOK_NUMBER		{ cur.config.sl.t1 = $<val>3; }
    | TOK_T2T TOK_EQUAL TOK_NUMBER		{ cur.config.sl.t2 = $<val>3; }
    | TOK_T2LT TOK_EQUAL TOK_NUMBER		{ cur.config.sl.t2l = $<val>3; }
    | TOK_T2HT TOK_EQUAL TOK_NUMBER		{ cur.config.sl.t2h = $<val>3; }
    | TOK_T3T TOK_EQUAL TOK_NUMBER		{ cur.config.sl.t3 = $<val>3; }
    | TOK_T4NT TOK_EQUAL TOK_NUMBER		{ cur.config.sl.t4n = $<val>3; }
    | TOK_T4ET TOK_EQUAL TOK_NUMBER		{ cur.config.sl.t4e = $<val>3; }
    | TOK_T5T TOK_EQUAL TOK_NUMBER		{ cur.config.sl.t5 = $<val>3; }
    | TOK_T6T TOK_EQUAL TOK_NUMBER		{ cur.config.sl.t6 = $<val>3; }
    | TOK_T7T TOK_EQUAL TOK_NUMBER		{ cur.config.sl.t7 = $<val>3; }
    | TOK_RB_ABATE TOK_EQUAL TOK_NUMBER		{ cur.config.sl.rb_abate = $<val>3; }
    | TOK_RB_ACCEPT TOK_EQUAL TOK_NUMBER	{ cur.config.sl.rb_accept = $<val>3; }
    | TOK_RB_DISCARD TOK_EQUAL TOK_NUMBER	{ cur.config.sl.rb_discard = $<val>3; }
    | TOK_TB_ABATE_1 TOK_EQUAL TOK_NUMBER	{ cur.config.sl.tb_abate_1 = $<val>3; }
    | TOK_TB_ONSET_1 TOK_EQUAL TOK_NUMBER	{ cur.config.sl.tb_onset_1 = $<val>3; }
    | TOK_TB_DISCD_1 TOK_EQUAL TOK_NUMBER	{ cur.config.sl.tb_discd_1 = $<val>3; }
    | TOK_TB_ABATE_2 TOK_EQUAL TOK_NUMBER	{ cur.config.sl.tb_abate_2 = $<val>3; }
    | TOK_TB_ONSET_2 TOK_EQUAL TOK_NUMBER	{ cur.config.sl.tb_onset_2 = $<val>3; }
    | TOK_TB_DISCD_2 TOK_EQUAL TOK_NUMBER	{ cur.config.sl.tb_discd_2 = $<val>3; }
    | TOK_TB_ABATE_3 TOK_EQUAL TOK_NUMBER	{ cur.config.sl.tb_abate_3 = $<val>3; }
    | TOK_TB_ONSET_3 TOK_EQUAL TOK_NUMBER	{ cur.config.sl.tb_onset_3 = $<val>3; }
    | TOK_TB_DISCD_3 TOK_EQUAL TOK_NUMBER	{ cur.config.sl.tb_discd_3 = $<val>3; }
    | TOK_N1 TOK_EQUAL TOK_NUMBER		{ cur.config.sl.N1 = $<val>3; }
    | TOK_N2 TOK_EQUAL TOK_NUMBER		{ cur.config.sl.N2 = $<val>3; }
    | TOK_M TOK_EQUAL TOK_NUMBER		{ cur.config.sl.M = $<val>3; }
    ;

type_value:
    TOK_NONE		{ $<val>$ = SDL_TYPE_NONE; }
    | TOK_V35		{ $<val>$ = SDL_TYPE_V35; }
    | TOK_DS0		{ $<val>$ = SDL_TYPE_DS0; }
    | TOK_DS0A		{ $<val>$ = SDL_TYPE_DS0A; }
    | TOK_E1		{ $<val>$ = SDL_TYPE_E1; }
    | TOK_T1		{ $<val>$ = SDL_TYPE_T1; }
    | TOK_J1		{ $<val>$ = SDL_TYPE_J1; }
    | TOK_ATM		{ $<val>$ = SDL_TYPE_ATM; }
    | TOK_PACKET	{ $<val>$ = SDL_TYPE_PACKET; }
    ;

rate_value:
    TOK_NONE		{ $<val>$ = SDL_RATE_NONE; }
    | TOK_DS0A		{ $<val>$ = SDL_RATE_DS0A; }
    | TOK_DS0		{ $<val>$ = SDL_RATE_DS0; }
    | TOK_T1		{ $<val>$ = SDL_RATE_T1; }
    | TOK_J1		{ $<val>$ = SDL_RATE_J1; }
    | TOK_E1		{ $<val>$ = SDL_RATE_E1; }
    | TOK_T2		{ $<val>$ = SDL_RATE_T2; }
    | TOK_E2		{ $<val>$ = SDL_RATE_E2; }
    | TOK_E3		{ $<val>$ = SDL_RATE_E3; }
    | TOK_T3		{ $<val>$ = SDL_RATE_T3; }
    | TOK_NUMBER
    ;

gtype_value:
    TOK_NONE		{ $<val>$ = SDL_GTYPE_NONE; }
    | TOK_T1		{ $<val>$ = SDL_GTYPE_T1; }
    | TOK_E1		{ $<val>$ = SDL_GTYPE_E1; }
    | TOK_J1		{ $<val>$ = SDL_GTYPE_J1; }
    | TOK_ATM		{ $<val>$ = SDL_GTYPE_ATM; }
    | TOK_ETH		{ $<val>$ = SDL_GTYPE_ETH; }
    | TOK_IP		{ $<val>$ = SDL_GTYPE_IP; }
    | TOK_UDP		{ $<val>$ = SDL_GTYPE_UDP; }
    | TOK_TCP		{ $<val>$ = SDL_GTYPE_TCP; }
    | TOK_RTP		{ $<val>$ = SDL_GTYPE_RTP; }
    | TOK_SCTP		{ $<val>$ = SDL_GTYPE_SCTP; }
    | TOK_T2		{ $<val>$ = SDL_GTYPE_T2; }
    | TOK_E2		{ $<val>$ = SDL_GTYPE_E2; }
    | TOK_E3		{ $<val>$ = SDL_GTYPE_E3; }
    | TOK_T3		{ $<val>$ = SDL_GTYPE_T3; }
    | TOK_OC3		{ $<val>$ = SDL_GTYPE_OC3; }
    | TOK_OC12		{ $<val>$ = SDL_GTYPE_OC12; }
    | TOK_OC48		{ $<val>$ = SDL_GTYPE_OC48; }
    | TOK_OC192		{ $<val>$ = SDL_GTYPE_OC192; }
    ;

grate_value:
    TOK_NONE		{ $<val>$ = SDL_GRATE_NONE; }
    | TOK_T1		{ $<val>$ = SDL_GRATE_T1; }
    | TOK_J1		{ $<val>$ = SDL_GRATE_J1; }
    | TOK_E1		{ $<val>$ = SDL_GRATE_E1; }
    | TOK_T2		{ $<val>$ = SDL_GRATE_T2; }
    | TOK_E2		{ $<val>$ = SDL_GRATE_E2; }
    | TOK_E3		{ $<val>$ = SDL_GRATE_E3; }
    | TOK_T3		{ $<val>$ = SDL_GRATE_T3; }
    | TOK_OC3		{ $<val>$ = SDL_GRATE_OC3; }
    | TOK_OC12		{ $<val>$ = SDL_GRATE_OC12; }
    | TOK_OC48		{ $<val>$ = SDL_GRATE_OC48; }
    | TOK_OC192		{ $<val>$ = SDL_GRATE_OC192; }
    | TOK_NUMBER
    ;

mode_value:
    TOK_NONE		{ $<val>$ = SDL_MODE_NONE; }
    | TOK_DSU		{ $<val>$ = SDL_MODE_DSU; }
    | TOK_CSU		{ $<val>$ = SDL_MODE_CSU; }
    | TOK_DTE		{ $<val>$ = SDL_MODE_DTE; }
    | TOK_DCE		{ $<val>$ = SDL_MODE_DCE; }
    | TOK_CLIENT	{ $<val>$ = SDL_MODE_CLIENT; }
    | TOK_SERVER	{ $<val>$ = SDL_MODE_SERVER; }
    | TOK_PEER		{ $<val>$ = SDL_MODE_PEER; }
    | TOK_ECHO		{ $<val>$ = SDL_MODE_ECHO; }
    | TOK_REM_LB	{ $<val>$ = SDL_MODE_REM_LB; }
    | TOK_LOC_LB	{ $<val>$ = SDL_MODE_LOC_LB; }
    | TOK_LB_ECHO	{ $<val>$ = SDL_MODE_LB_ECHO; }
    | TOK_TEST		{ $<val>$ = SDL_MODE_TEST; }
    ;

gmode_value:
    TOK_NONE		{ $<val>$ = SDL_GMODE_NONE; }
    | TOK_LOC_LB	{ $<val>$ = SDL_GMODE_LOC_LB; }
    | TOK_REM_LB	{ $<val>$ = SDL_GMODE_REM_LB; }
    ;

gcrc_value:
    TOK_NONE		{ $<val>$ = SDL_GCRC_NONE; }
    | TOK_CRC4		{ $<val>$ = SDL_GCRC_CRC4; }
    | TOK_CRC5		{ $<val>$ = SDL_GCRC_CRC5; }
    | TOK_CRC6		{ $<val>$ = SDL_GCRC_CRC6; }
    | TOK_CRC6J		{ $<val>$ = SDL_GCRC_CRC6J; }
    ;

clock_value:
    TOK_NONE		{ $<val>$ = SDL_CLOCK_NONE; }
    | TOK_INT		{ $<val>$ = SDL_CLOCK_INT; }
    | TOK_EXT		{ $<val>$ = SDL_CLOCK_EXT; }
    | TOK_LOOP		{ $<val>$ = SDL_CLOCK_LOOP; }
    | TOK_MASTER	{ $<val>$ = SDL_CLOCK_MASTER; }
    | TOK_SLAVE		{ $<val>$ = SDL_CLOCK_SLAVE; }
    | TOK_DPLL		{ $<val>$ = SDL_CLOCK_DPLL; }
    | TOK_ABR		{ $<val>$ = SDL_CLOCK_ABR; }
    | TOK_SHAPER	{ $<val>$ = SDL_CLOCK_SHAPER; }
    | TOK_TICK		{ $<val>$ = SDL_CLOCK_TICK; }
    ;

coding_value:
    TOK_NONE		{ $<val>$ = SDL_CODING_NONE; }
    | TOK_NRZ		{ $<val>$ = SDL_CODING_NRZ; }
    | TOK_NRZI		{ $<val>$ = SDL_CODING_NRZI; }
    | TOK_AMI		{ $<val>$ = SDL_CODING_AMI; }
    | TOK_B6ZS		{ $<val>$ = SDL_CODING_B6ZS; }
    | TOK_B8ZS		{ $<val>$ = SDL_CODING_B8ZS; }
    | TOK_HDB3		{ $<val>$ = SDL_CODING_HDB3; }
    | TOK_AAL1		{ $<val>$ = SDL_CODING_AAL1; }
    | TOK_AAL2		{ $<val>$ = SDL_CODING_AAL2; }
    | TOK_AAL5		{ $<val>$ = SDL_CODING_AAL5; }
    ;

framing_value:
    TOK_NONE		{ $<val>$ = SDL_FRAMING_NONE; }
    | TOK_CCS		{ $<val>$ = SDL_FRAMING_CCS; }
    | TOK_CAS		{ $<val>$ = SDL_FRAMING_CAS; }
    | TOK_SF		{ $<val>$ = SDL_FRAMING_SF; }
    | TOK_ESF		{ $<val>$ = SDL_FRAMING_ESF; }
    | TOK_D4		{ $<val>$ = SDL_FRAMING_D4; }
    ;

rxlevel_value:
    TOK_NUMBER
    ;

txlevel_value:
    TOK_NUMBER
    ;

cha_command:
    /* TOK_WHITESPACE */ identifier
    profile_optional		{ cur.pvar = $<val>2; }
    options_optional		{ cur.popt = $<val>4; }
    overrides_optional
    ;

identifier:
    global_ppa		{ cur.gppa = $<val>1; $<val>$ = 2; }
    | clei		{ $<val>$ = 1; }
    ;

profile_optional:
    { $<val>$ = cur.pvar; }
    | /* TOK_WHITESPACE */ profile
    ;

options_optional:
    { $<val>$ = cur.popt; }
    | /* TOK_WHITESPACE */ option_list
    ;

overrides_optional:
    | /* TOK_WHITESPACE */ override_list
    ;

del_command:
    /* TOK_WHITESPACE */ identifiers
    ;

identifiers:
    TOK_ALL
    | identifier_list
    ;

identifier_list:
    identifier
    | identifier_list TOK_COMMA identifier
    | identifier_list /* TOK_WHITESPACE */ identifier
    ;

get_command:
    /* TOK_WHITESPACE */ identifiers
    ;

link_command:
    TOK_OPEN file_handle device_selection
    {
	handle = $<val>2;
	if (yydebug)
	    fprintf(stderr, "handle is %d\n", handle);
	if (sel[handle].fd == -1) {
	    if (yydebug)
		fprintf(stderr, "result is %.64s\n", $<str>3);
	    strncpy(&sel[handle].device[0],$<str>3,MAX_BUF_SIZE);
	    if (sel[handle].level < 1) sel[handle].level = 1;
	    if ((sel[handle].fd = open(sel[handle].device, O_RDWR)) != -1) {
		sel[handle].state = LMI_UNATTACHED;
		if (interactive)
		    fprintf(stdout, "\nSuccessful open of stream `%d' on \"%.64s\".\n", handle, $<str>3);
	    } else {
		yyerror(strerror(errno));
		YYERROR;
	    }
	} else {
	    yyerror("handle not in closed state");
	    YYERROR;
	}
    }
    | TOK_ATTACH file_handle link_selection
    {
	handle = $<val>2;
	if (sel[handle].state != LMI_UNATTACHED) {
	    yyerror("attach attempted in wrong state");
	    YYERROR;
	}
	if (interactive && sel[handle].clei[0] != '\0')
	    fprintf(stdout, "\nAttaching `%d' to CLEI \"%.64s\".\n", handle, sel[handle].clei);
	sel[handle].state = LMI_DISABLED;
    }
    | TOK_ENABLE file_handle
    {
	handle = $<val>2;
	if (sel[handle].state != LMI_DISABLED) {
	    yyerror("enable attempted in wrong state");
	    YYERROR;
	}
	if (interactive)
	    fprintf(stdout, "\nEnabling `%d'.\n", handle);
	sel[handle].state = LMI_ENABLED;
    }
    | TOK_START file_handle
    {
	handle = $<val>2;
	if (sel[handle].state != LMI_ENABLED) {
	    yyerror("start attempted in wrong state");
	    YYERROR;
	}
	if (interactive)
	    fprintf(stdout, "\nStarting `%d'.\n", handle);
    }
    | TOK_EMERGENCY file_handle
    {
	handle = $<val>2;
	if (sel[handle].state != LMI_ENABLED) {
	    yyerror("emergency attempted in wrong state");
	    YYERROR;
	}
	if (interactive)
	    fprintf(stdout, "\nEmergency `%d'.\n", handle);
    }
    | TOK_NORMAL file_handle
    {
	handle = $<val>2;
	if (sel[handle].state != LMI_ENABLED) {
	    yyerror("normal attempted in wrong state");
	    YYERROR;
	}
	if (interactive)
	    fprintf(stdout, "\nNormal `%d'.\n", handle);
    }
    | TOK_STOP file_handle
    {
	handle = $<val>2;
	if (sel[handle].state != LMI_ENABLED) {
	    yyerror("stop attempted in wrong state");
	    YYERROR;
	}
	if (interactive)
	    fprintf(stdout, "\nStoping `%d'.\n", handle);
    }
    | TOK_DISABLE file_handle
    {
	handle = $<val>2;
	if (sel[handle].state != LMI_ENABLED) {
	    yyerror("disable attempted in wrong state");
	    YYERROR;
	}
	if (interactive)
	    fprintf(stdout, "\nDisabling `%d'.\n", handle);
	sel[handle].state = LMI_DISABLED;
    }
    | TOK_DETACH file_handle
    {
	handle = $<val>2;
	if (sel[handle].state != LMI_DISABLED) {
	    yyerror("detach attempted in wrong state");
	    YYERROR;
	}
	if (interactive && sel[handle].clei[0] != '\0')
	    fprintf(stdout, "\nDetaching `%d' from CLEI \"%.64s\".\n", handle, sel[handle].clei);
	sel[handle].state = LMI_UNATTACHED;
    }
    | TOK_CLOSE file_handle
    {
	handle = $<val>2;
	if (sel[handle].fd == -1) {
	    yyerror("handle not in open state");
	    YYERROR;
	}
	close(sel[handle].fd);
	sel[handle].fd = -1;
	sel[handle].state = -1;
	if (interactive)
	    fprintf(stdout, "\nSuccessful close of stream `%d' on \"%.64s\".\n", handle, sel[handle].device);
    }
    ;

file_handle:
    { $<val>$ = handle; }
    | TOK_NUMBER
    {
	int h = $<val>1;
	if (h >= 0 && h < MAX_HANDLES) {
	    $<val>$ = h;
	} else {
	    yyerror("invalid file handle");
	    YYERROR;
	}
    }
    ;

device_selection:
    { $<str>$ = &sel[handle].device[0]; }
    | TOK_DEVICE TOK_STRING
    {
	char *device = $<str>2;
	$<str>$ = device;
    }
    ;

link_selection:
    type_selection card_selection span_selection slot_selection
    {
	sel[handle].type = $<val>1;
	sel[handle].card = $<val>2;
	sel[handle].span = $<val>3;
	sel[handle].slot = $<val>4;
    }
    | TOK_CLEI TOK_STRING
    {
	strncpy(sel[handle].clei,$<str>2,MAX_BUF_SIZE);
    }
    ;

type_selection:
    { $<val>$ = sel[handle].type; }
    | TOK_TYPE driver_type { sel[handle].type = $<val>2; }
    ;

driver_type:
    TOK_NONE  { $<val>$ = 0; }
    | TOK_X400P { $<val>$ = 1; }
    | TOK_M2PA  { $<val>$ = 2; }
    | TOK_M2UA  { $<val>$ = 3; }
    | error { yyerror("invalid driver type"); YYERROR; }
    ;

card_selection:
    { $<val>$ = sel[handle].card; }
    | TOK_CARD TOK_NUMBER
    {
	int val = $<val>2;
	if (val < 1) { yyerror("invalid card number, min 1"); YYERROR; }
	if (val > 8) { yyerror("invalid card number, max 8"); YYERROR; }
	$<val>$ = val;
    }
    ;

span_selection:
    { $<val>$ = sel[handle].span; }
    | TOK_SPAN TOK_NUMBER
    {
	int val = $<val>2;
	if (val < 1) { yyerror("invalid span number, min 1"); YYERROR; }
	if (val > 4) { yyerror("invalid span number, max 4"); YYERROR; }
	$<val>$ = val;
    }
    ;

slot_selection:
    { $<val>$ = sel[handle].slot; }
    | TOK_SLOT TOK_NUMBER
    {
	int val = $<val>2;
	if (val <  1) { yyerror("invalid slot number, min 1"); YYERROR; }
	if (val > 31) { yyerror("invalid slot number, max 31"); YYERROR; }
	$<val>$ = val;
    }
    ;

/* ADDITIONAL C CODE */

%%

int
yyerror(const char *s)
{
	char *text = yytext;

	if (*text == '\n')
	    text = "<EOL>";
	if (interactive)
		fprintf(stderr, "\n");
	fprintf(stderr, "--ERROR: (%s:line %d) [%s] %s.\n", filename, yylineno, text, s);
	if (interactive)
		fprintf(stderr, "         Type \"help\" for information.\n");
	if (interactive)
		fprintf(stderr, "\n");
	fflush(stderr);
	if (!interactive)
		exit(2);
	return (0);
}

void
newline(void)
{
	if (interactive) {
		fprintf(stdout, "%s", prompt);
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
}

extern void yyrestart(FILE *);

void
yyinit(const char *name, FILE *input, FILE *output)
{
	if ((prompt = strrchr(name, '/')) != NULL)
		prompt++;
	else
		prompt = name;
	yyin = input;
	yyout = output;
	yyrestart(yyin);
	if (interactive) {
		copying(prompt);
		fprintf(stdout, "Type \"help\" for help...\n");
	}
	newline();
}

