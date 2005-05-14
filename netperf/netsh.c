/*****************************************************************************

 @(#) $RCSfile: netsh.c,v $ $Name:  $($Revision: 1.1.1.11 $) $Date: 2005/05/14 08:30:03 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2005/05/14 08:30:03 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: netsh.c,v $
 Revision 1.1.1.11  2005/05/14 08:30:03  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: netsh.c,v $ $Name:  $($Revision: 1.1.1.11 $) $Date: 2005/05/14 08:30:03 $"

static char const ident[] = "$RCSfile: netsh.c,v $ $Name:  $($Revision: 1.1.1.11 $) $Date: 2005/05/14 08:30:03 $";

#ifdef NEED_MAKEFILE_EDIT
#error you must first edit and customize the makefile to your platform
#endif /* NEED_MAKEFILE_EDIT */
char	netsh_id[]="\
@(#)netsh.c (c) Copyright 1993-2004 Hewlett-Packard Company. Version 2.3pl1";


/****************************************************************/
/*								*/
/*	Global include files					*/
/*								*/
/****************************************************************/

#include <stdio.h>
#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#if HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif
#if STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# if HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif
#if HAVE_STRING_H
# if !STDC_HEADERS && HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#endif
#if HAVE_STRINGS_H
# include <strings.h>
#endif
#if HAVE_INTTYPES_H
# include <inttypes.h>
#else
# if HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif
#if HAVE_UNISTD_H
# include <unistd.h>
#endif
#if HAVE_MALLOC_H
# include <malloc.h>
#endif

#ifndef WIN32
# ifndef __VMS
#  include <sys/ipc.h>
# endif /* __VMS */
#endif /* WIN32 */

#if HAVE_FCNTL_H
# include <fcntl.h>
#endif

#ifndef WIN32
# include <errno.h>
# include <signal.h>
#endif  // !WIN32

#include <ctype.h>

 /* the following four includes should not be needed ?*/
#ifndef WIN32
# if TIME_WITH_SYS_TIME
#  include <sys/time.h>
#  include <time.h>
# else
#  if HAVE_SYS_TIME_H
#   include <sys/time.h>
#  else
#   include <time.h>
#  endif
# endif
# if HAVE_SYS_SOCKET_H
#  include <sys/socket.h>
# endif
# if HAVE_NETINET_IN_H
#  include <netinet/in.h>
# endif
# if HAVE_NETDB_H
#  include <netdb.h>
# endif
#else
# include <time.h>
# include <winsock2.h>
#endif

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif /* _GNU_SOURCE */

#ifdef WIN32
extern	int	getopt(int , char **, char *) ;
#else
double atof(const char *);
#endif /* WIN32 */

/**********************************************************************/
/*                                                                    */
/*          Local Include Files                                       */
/*                                                                    */
/**********************************************************************/

#define  NETSH
#include "netsh.h"
#include "netlib.h"
#include "nettest_bsd.h"
#ifdef DO_UNIX
#include "nettest_unix.h"
#ifndef WIN32
#if HAVE_SYS_SOCKET_H
# include "sys/socket.h"
#endif
#endif  // !WIN32
#endif /* DO_UNIX */
#ifdef DO_XTI
#include "nettest_xti.h"
#endif /* DO_XTI */
#ifdef DO_DLPI
#include "nettest_dlpi.h"
#endif /* DO_DLPI */
#ifdef DO_IPV6
#include "nettest_ipv6.h"
#endif /* DO_IPV6 */
#ifdef DO_DNS
#include "nettest_dns.h"
#endif /* DO_DNS */

/************************************************************************/
/*									*/
/*	Global constants  and macros					*/
/*									*/
/************************************************************************/

 /* Some of the args take optional parameters. Since we are using */
 /* getopt to parse the command line, we will tell getopt that they do */
 /* not take parms, and then look for them ourselves */
#ifdef _GNU_SOURCE
#define GLOBAL_CMD_LINE_ARGS "A:a:b:C::c::dDf:F:H:hi:I:l:n:O:o:P:p:t:T:v:VW:w:46"
#else /* _GNU_SOURCE */
#define GLOBAL_CMD_LINE_ARGS "A:a:b:CcdDf:F:H:hi:I:l:n:O:o:P:p:t:T:v:VW:w:46"
#endif /* _GNU_SOURCE */

/************************************************************************/
/*									*/
/*	Extern variables 						*/
/*									*/
/************************************************************************/

/*
extern int errno;
extern char *sys_errlist[ ];
extern int sys_nerr;
*/

/************************************************************************/
/*									*/
/*	Global variables 						*/
/*									*/
/************************************************************************/

/* some names and such                                                  */
char	*program;		/* program invocation name		*/
char	username[BUFSIZ];	/* login name of user			*/
char	cmd_file[BUFSIZ];	/* name of the commands file		*/

/* stuff to say where this test is going                                */
char	host_name[HOSTNAMESIZE];	/* remote host name or ip addr  */
char    test_name[BUFSIZ];		/* which test to run 		*/
short	test_port;			/* where is the test waiting    */

/* the source of data for filling the buffers */
char    fill_file[BUFSIZ];

/* output controlling variables                                         */
int
  debug,			/* debugging level */
  print_headers,		/* do/don't display headers */
  verbosity;		/* verbosity level */

/* cpu variables */
int
  local_cpu_usage,	/* you guessed it			*/
  remote_cpu_usage;	/* still right !			*/

float			       
  local_cpu_rate,
  remote_cpu_rate;

int
  shell_num_cpus=1;

/* the end-test conditions for the tests - either transactions, bytes,  */
/* or time. different vars used for clarity - space is cheap ;-)        */
int	
  test_time,		/* test ends by time			*/
  test_len_ticks,       /* how many times will the timer go off before */
			/* the test is over? */
  test_bytes,		/* test ends on byte count		*/
  test_trans;		/* test ends on tran count		*/

/* the alignment conditions for the tests				*/
int
  local_recv_align,	/* alignment for local receives		*/
  local_send_align,	/* alignment for local sends		*/
  local_send_offset = 0,
  local_recv_offset = 0,
  remote_recv_align,	/* alignment for remote receives	*/
  remote_send_align,	/* alignment for remote sends		*/
  remote_send_offset = 0,
  remote_recv_offset = 0;

#ifdef INTERVALS
int
  interval_usecs,
  interval_wate,
  interval_burst;

int demo_mode;
double units_this_tick;
#endif /* INTERVALS */

#ifdef DIRTY
int	loc_dirty_count;
int	loc_clean_count;
int	rem_dirty_count;
int	rem_clean_count;
#endif /* DIRTY */

 /* some of the vairables for confidence intervals... */

int  confidence_level;
int  iteration_min;
int  iteration_max;

double interval;

 /* stuff to control the "width" of the buffer rings for sending and */
 /* receiving data */
int	send_width;
int     recv_width;

/* address family */
int	af = AF_INET;

char netserver_usage[] = "\n\
Usage: netserver [options] \n\
\n\
Options:\n\
    -h                Display this text\n\
    -d                Increase debugging output\n\
    -p portnum        Listen for connect requests on portnum.\n\
    -4                Do IPv4\n\
    -6                Do IPv6\n\
    -v verbosity      Specify the verbosity level\n\
\n";

char netperf_usage[] = "\n\
Usage: netperf [global options] -- [test options] \n\
\n\
Global options:\n\
    -a send,recv      Set the local send,recv buffer alignment\n\
    -A send,recv      Set the remote send,recv buffer alignment\n\
    -c [cpu_rate]     Report local CPU usage\n\
    -C [cpu_rate]     Report remote CPU usage\n\
    -d                Increase debugging output\n\
    -f G|M|K|g|m|k    Set the output units\n\
    -F fill_file      Pre-fill buffers with data from fill_file\n\
    -h                Display this text\n\
    -H name|ip        Specify the target machine\n\
    -i max,min        Specify the max and min number of iterations (15,1)\n\
    -I lvl[,intvl]    Specify confidence level (95 or 99) (99) \n\
                      and confidence interval in percentage (10)\n\
    -l testlen        Specify test duration (>0 secs) (<0 bytes|trans)\n\
    -o send,recv      Set the local send,recv buffer offsets\n\
    -O send,recv      Set the remote send,recv buffer offset\n\
    -n numcpu         Set the number of processors for CPU util\n\
    -p port           Specify netserver port number\n\
    -P 0|1            Don't/Do display test headers\n\
    -t testname       Specify test to perform\n\
    -T cpu            Request remote netserver be bound to cpu\n\
    -v verbosity      Specify the verbosity level\n\
    -W send,recv      Set the number of send,recv buffers\n\
\n\
For those options taking two parms, at least one must be specified;\n\
specifying one value without a comma will set both parms to that\n\
value, specifying a value with a leading comma will set just the second\n\
parm, a value with a trailing comma will set just the first. To set\n\
each parm to unique values, specify both and separate them with a\n\
comma.\n"; 


/* This routine will return the two arguments to the calling routine. */
/* If the second argument is not specified, and there is no comma, */
/* then the value of the second argument will be the same as the */
/* value of the first. If there is a comma, then the value of the */
/* second argument will be the value of the second argument ;-) */
void
break_args(char *s, char *arg1, char *arg2)

{
  char *ns;
  ns = strchr(s,',');
  if (ns) {
    /* there was a comma arg2 should be the second arg*/
    *ns++ = '\0';
    while ((*arg2++ = *ns++) != '\0');
  }
  else {
    /* there was not a comma, we can use ns as a temp s */
    /* and arg2 should be the same value as arg1 */
    ns = s;
    while ((*arg2++ = *ns++) != '\0');
  };
  while ((*arg1++ = *s++) != '\0');
}

void
set_defaults(void)
{
  
  /* stuff to say where this test is going                              */
  strcpy(host_name,"localhost");	/* remote host name or ip addr  */
  strcpy(test_name,"TCP_STREAM");	/* which test to run 		*/
  test_port	= 12865;	        /* where is the test waiting    */
  
  /* output controlling variables                               */
  debug			= 0;/* debugging level			*/
  print_headers		= 1;/* do print test headers		*/
  verbosity		= 1;/* verbosity level			*/
  /* cpu variables */
  local_cpu_usage	= 0;/* measure local cpu		*/
  remote_cpu_usage	= 0;/* what do you think ;-)		*/
  
  local_cpu_rate	= (float)0.0;
  remote_cpu_rate	= (float)0.0;
  
  /* the end-test conditions for the tests - either transactions, bytes,  */
  /* or time. different vars used for clarity - space is cheap ;-)        */
  test_time	= 10;	/* test ends by time			*/
  test_bytes	= 0;	/* test ends on byte count		*/
  test_trans	= 0;	/* test ends on tran count		*/
  
  /* the alignment conditions for the tests				*/
  local_recv_align	= 8;	/* alignment for local receives	*/
  local_send_align	= 8;	/* alignment for local sends	*/
  remote_recv_align	= 8;	/* alignment for remote receives*/
  remote_send_align	= 8;	/* alignment for remote sends	*/
  
#ifdef INTERVALS
  /* rate controlling stuff */
  interval_usecs  = 0;
  interval_wate   = 1;
  interval_burst  = 0;
#endif /* INTERVALS */
  
#ifdef DIRTY
  /* dirty and clean cache stuff */
  loc_dirty_count = 0;
  loc_clean_count = 0;
  rem_dirty_count = 0;
  rem_clean_count = 0;
#endif /* DIRTY */

 /* some of the vairables for confidence intervals... */

  confidence_level = 99;
  iteration_min = 1;
  iteration_max = 1;
  interval = 0.05; /* five percent? */

  strcpy(fill_file,"");
}
     

void
print_netperf_usage(int argc, char *argv[])
{
	fprintf(stderr, "\
Usgae:\n\
    %1$s [-t, --testname=TESTNAME] [global opts] -- [test opts]\n\
    %1$s [-t, --testname=TESTNAME] -- {-h, --help}\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-L, --copying}\n\
", argv[0]);
}

void
print_netperf_help(int argc, char *argv[])
{
        fprintf(stdout, "\
Usgae:\n\
    %1$s [-t, --testname=TESTNAME] [options] -- [test options]\n\
    %1$s [-t, --testname=TESTNAME] -- {-h, --help}\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-L, --copying}\n\
Arguments:\n\
    (none)\n\
Options:\n\
    [-t, --testname=TESTNAME]\n\
        set the test to perform\n\
    -h, --help\n\
        display this message and exit\n\
    -V, --version\n\
        display version information and exit\n\
    -L, --copying\n\
        display copying permission and exit\n\
    -4, --ipv4\n\
        use ipv4 for the control socket (default)\n\
    -6, --ipv6\n\
        use ipv6 for the control socket\n\
    -a, --loc-align=[LOC-SND-ALIGN][,][LOC-RCV-ALIGN]\n\
        set local send and receive buffer alignements\n\
    -A, --rem-align=[REM-SND-ALIGN][,][REM-SND-ALIGN]\n\
        set remote send and receive buffer alignements\n\
    -b, --burst=BURST\n\
        set number of burst packets per interval\n\
    -c, --loc-cpu [LOC-RATE]\n\
        request (set) local cpu rate\n\
    -C, --rem-cpu [REM-RATE]\n\
        request (set) remote cpu rate\n\
    -d, --debug\n\
        debugging output\n\
    -D, --demo\n\
        demo mode: not support on Linux\n\
    -f, --format={G|H|K|g|m|k}\n\
        printed units of measure for bulk transfer tests\n\
        capital letters are powers of 2, lowercase are powers of 10\n\
    -F, --fillfile=FILL-FILE\n\
        prefill buffers with data from this file\n\
    -H, --host=REMOTE-HOST\n\
        hostname (or IP address) of netserver\n\
    -i, --interations=[MIN][,][MAX]\n\
        minimum and maximum interations for confidence level\n\
    -I, --interval=LEVEL[,INTVL]\n\
        set confidence level (95 or 99) and width of confidence interval\n\
        in percentage [Default: 99,10]\n\
    -k, --loc-count=[LOC-DIRTY][,][LOC-CLEAN]\n\
        set counts for local dirty and clean buffers\n\
    -K, --rem-count=[REM-DIRTY][,][REM-CLEAN]\n\
        set counts for remote dirty and clean buffers\n\
    -l, --length=TESTLEN\n\
        set test length (positive: seconds, negative: packets)\n\
    -n, --numcpus=NUMCPUS\n\
        set number of CPUs\n\
    -o, --loc-offset=[LOC-SND-OFFSET][,][LOC-RCV-OFFSET]\n\
        set local send and receive buffer offset\n\
    -O, --rem-offset=[REM-SND-OFFSET][,][REM-RCV-OFFSET]\n\
        set remote send and receive buffer offset\n\
    -p, --port=PORTNUM\n\
        set port number of netserver [Default: 12865]\n\
    -P, --print={0|1}\n\
        whether to print test headers [Default: 1:yes]\n\
    -v, --verbose=VERBOSITY\n\
        set output verbosity level\n\
    -w, --rate=RATE\n\
        send packet rate (packets per millisecond)\n\
    -W, --width=[SND-WIDTH][,][RCV-WIDTH]\n\
        width of user space send or receive buffer rings\n\
", argv[0]);
}

void
print_netperf_version(int argc, char *argv[])
{
	int i = 0;
	fprintf(stdout, "\
%1$s:\n\
    %2$s\n\
    COPYRIGHT (C) 2001-2004  OPENSS7 CORPORATION.  ALL RIGHTS RESERVED.\n\
    Distributed by OpenSS7 Corporation under GPL Version 2,\n\
    incorporated here by reference.\n\
    See `%1$s --copying' for copying permissions.\n\
", argv[0], ident);
	fprintf(stdout, "\
Compiled-in Test Names: (try `%1$s -t TESTNAME -- -h' for help):\n\
", argv[0]);
	fprintf(stdout, "    "); i = 4;
#ifdef DO_SCTP
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "SCTP_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "SCTP_MAERTS, ");
#ifdef HAVE_SENDFILE
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "SCTP_SENDFILE, ");
#endif /* HAVE_SENDFILE */
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "SCTP_RR, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
#ifdef DO_NBRR
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "SCTP_NBRR, ");
#endif /* DO_NBRR */
	i += fprintf(stdout, "SCTP_CRR, ");
#ifdef DO_1644
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "SCTP_TRR, ");
#endif /* DO_1644 */
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "SCTP_CC, ");
#endif /* DO_SCTP */
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "TCP_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "TCP_MAERTS, ");
#ifdef HAVE_SENDFILE
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "TCP_SENDFILE, ");
#endif /* HAVE_SENDFILE */
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "TCP_RR, ");
#ifdef DO_NBRR
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "TCP_NBRR, ");
#endif /* DO_NBRR */
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "TCP_CRR, ");
#ifdef DO_1644
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "TCP_TRR, ");
#endif /* DO_1644 */
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "TCP_CC, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "UDP_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "UDP_RR, ");
#ifdef DO_XTI
#ifdef DO_XTI_SCTP
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "XTI_SCTP_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "XTI_SCTP_RR, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "XTI_SCTP_CRR, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "XTI_SCTP_TRR, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "XTI_SCTP_CC, ");
#endif /* DO_XTI_SCTP */
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "XTI_TCP_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "XTI_TCP_RR, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "XTI_TCP_CRR, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "XTI_TCP_CC, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "XTI_UDP_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "XTI_UDP_RR, ");
#endif /* DO_XTI */
#ifdef DO_IPV6
#ifdef DO_IPV6_SCTP
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "SCTPIPV6_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "SCTPIPV6_RR, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "SCTPIPV6_CRR, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "SCTPIPV6_TRR, ");
#endif /* DO_IPV6_SCTP */
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "TCPIPV6_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "TCPIPV6_RR, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "TCPIPV6_CRR, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "UDPIPV6_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "UDPIPV6_RR, ");
#endif /* DO_IPV6 */
#ifdef DO_UNIX
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "STREAM_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "STREAM_RR, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DG_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DG_RR, ");
#endif /* DO_UNIX */
#ifdef DO_DLPI
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DLCO_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DLCO_RR, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DLCL_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DLCL_RR, ");
#endif /* DO_DLPI */
#ifdef DO_FORE
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "FORE_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "FORE_RR, ");
#endif /* DO_FORE */
#ifdef DO_HIPPI
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "HIPPI_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "HIPPI_RR, ");
#endif /* DO_HIPPI */
#ifdef DO_LWP
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "LWPSTR_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "LWPSTR_RR, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "LWPDG_STREAM, ");
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "LWPDG_RR, ");
#endif /* DO_LWP */
#ifdef DO_DNS
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DNS_RR, ");
#endif /* DO_DNS */
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "LOC_CPU, ");
	fprintf(stdout, "REM_CPU\n");
	fprintf(stdout, "\
Compiled-in Options:\n\
");
	fprintf(stdout, "    "); i = 4;
#ifdef DIRTY
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DIRTY, ");
#endif /* DIRTY */
#ifdef DO_1644
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DO_1644, ");
#endif /* DO_1644 */
#ifdef DO_DLPI
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DO_DLPI, ");
#endif /* DO_DLPI */
#ifdef DO_DNS
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DO_DNS, ");
#endif /* DO_DNS */
#ifdef DO_FIRST_BURST
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DO_FIRST_BURST, ");
#endif /* DO_FIRST_BURST */
#ifdef DO_FORE
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DO_FORE, ");
#endif /* DO_FORE */
#ifdef DO_HIPPI
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DO_HIPPI, ");
#endif /* DO_HIPPI */
#ifdef DO_IPV6
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DO_IPV6, ");
#endif /* DO_IPV6 */
#ifdef DO_IPV6_SCTP
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DO_IPV6_SCTP, ");
#endif /* DO_IPV6_SCTP */
#ifdef DO_NBRR
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DO_NBRR, ");
#endif /* DO_NBRR */
#ifdef DO_LWP
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DO_LWP, ");
#endif /* DO_LWP */
#ifdef DO_SCTP
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DO_SCTP, ");
#endif /* DO_SCTP */
#ifdef DO_SELECT
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DO_SELECT, ");
#endif /* DO_SELECT */
#ifdef DO_UNIX
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DO_UNIX, ");
#endif /* DO_UNIX */
#ifdef DO_XTI
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DO_XTI, ");
#endif /* DO_XTI */
#ifdef DO_XTI_SCTP
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DO_XTI_SCTP, ");
#endif /* DO_XTI_SCTP */
#ifdef DONT_WAIT
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "DONT_WAIT, ");
#endif /* DONT_WAIT */
#ifdef HAVE_SENDFILE
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "HAVE_SENDFILE, ");
#endif /* HAVE_SENDFILE */
#ifdef HISTOGRAM
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "HISTOGRAM, ");
#endif /* HISTOGRAM */
#ifdef INTERVALS
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "INTERVALS, ");
#endif /* INTERVALS */
#ifdef OLD_HISTOGRAM
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "OLD_HISTOGRAM, ");
#endif /* HISTOGRAM */
#ifdef USE_KSTAT
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "USE_KSTAT, ");
#endif /* USE_KSTAT */
#ifdef USE_LOOPER
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "USE_LOOPER, ");
#endif /* USE_LOOPER */
#ifdef USE_PERFSTAT
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "USE_PERFSTAT, ");
#endif /* USE_PERFSTAT */
#ifdef USE_PROC_STAT
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "USE_PROC_STAT, ");
#endif /* USE_PROC_STAT */
#ifdef USE_SYSCTL
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "USE_SYSCTL, ");
#endif /* USE_SYSCTL */
#ifdef _GNU_SOURCE
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	i += fprintf(stdout, "_GNU_SOURCE, ");
#endif /* _GNU_SOURCE */
	if (i > 64) { fprintf(stdout, "\n    "); i = 4; }
	fprintf(stdout, "_XOPEN_SOURCE\n\n");
}

void
print_netperf_copying(int argc, char *argv[])
{
	fprintf(stdout, "\
\n\
COPYRIGHT (C) 2001-2004 OPENSS7 CORPORATION <http://www.openss7.com/>\n\
COPYRIGHT (C) 1997-2001 BRIAN F. G. BIDULOCK <bidulock@openss7.org>\n\
\n\
ALL RIGHTS RESERVED.\n\
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
General Public License  (GPL)  Version 2  or later,  so long as  the software is\n\
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
\n\
COPYRIGHT (C) 1993-2001 HEWLETT-PACKARD COMPANY\n\
\n\
ALL RIGHTS RESERVED.\n\
\n\
The  enclosed software and documention  includes  copyrighted  works of Hewlett-\n\
Packard  Co. For as long as you  comply with the following  limitations, you are\n\
hereby authorized to (i) use, reproduce, and modify  the software  and document-\n\
ation, and to (ii) distribute the software and documentation,  including modifi-\n\
cations, for non-commercial purposes only.\n\
\n\
1.  The enclosed  software and documentation is made  available at no  charge in\n\
    order  to  advance  the general  development of  high-performance networking\n\
    products.\n\
\n\
2.  You may not delete any  copyright notices contained in the software or docu-\n\
    mentation.  All hard copies, and copies in source code or object  code form,\n\
    of the software or  documentation (including  modifications) must contain at\n\
    least one of the copyright notices.\n\
\n\
3.  The enclosed  software and documentation  has not been  subjected to testing\n\
    and quality control  and is not a Hewlett-Packard Co. product.   At a future\n\
    time, Hewlett-Packard Co. may or may not offer a version of the software and\n\
    documentation as a product.\n\
\n\
4.  THE SOFTWARE AND DOCUMENTATION IS PROVIDED \"AS IS\".  HEWLETT-PACKARD COMPANY\n\
    DOES NOT WARRANT THAT THE USE, REPRODUCTION, MODIFICATION OR DISTRIBUTION OF\n\
    THE SOFTWARE OR DOCUMENTATION WILL NOT INFRINGE A THIRD PARTY'S INTELLECTUAL\n\
    PROPERTY RIGHTS.  HP DOES NOT WARRANT THAT  THE SOFTWARE OR DOCUMENTATION IS\n\
    ERROR FREE. HP DISCLAIMS ALL WARRANTIES, EXPRESS AND IMPLIED, WITH REGARD TO\n\
    THE SOFTWARE AND THE DOCUMENTATION. HP SPECIFICALLY DISCLAIMS ALL WARRANTIES\n\
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
5.  HEWLETT-PACKARD COMPANY  WILL NOT IN  ANY EVENT BE  LIABLE  FOR  ANY  DIRECT,\n\
    INDIRECT,  SPECIAL,  INCIDENTAL  OR  CONSEQUENTIAL  DAMAGES  (INCLUDING  LOST\n\
    PROFITS) RELATED TO ANY USE,  REPRODUCTION,  MODIFICATION, OR DISTRIBUTION OF\n\
    THE SOFTWARE OR DOCUMENTATION.\n\
\n\
");
}

void
scan_cmd_line(int argc, char *argv[])
{
  extern int	optind;           /* index of first unused arg 	*/
  extern char	*optarg;	  /* pointer to option string	*/
  
  int		c;
  
  char	arg1[BUFSIZ],  /* argument holders		*/
  arg2[BUFSIZ];

#ifdef _GNU_SOURCE
  int option_index = 0;
  static struct option long_options[] = {
	{"rem-align",	required_argument,	NULL, 'A'},
	{"loc-align",	required_argument,	NULL, 'a'},
	{"burst",	required_argument,	NULL, 'b'},
	{"rem-cpu",	optional_argument,	NULL, 'C'},
	{"loc-cpu",	optional_argument,	NULL, 'c'},
	{"debug",	no_argument,		NULL, 'd'},
	{"demo",	no_argument,		NULL, 'D'},
	{"format",	required_argument,	NULL, 'f'},
	{"fillfile",	required_argument,	NULL, 'F'},
	{"host",	required_argument,	NULL, 'H'},
	{"help",	no_argument,		NULL, 'h'},
	{"iterations",	required_argument,	NULL, 'i'},
	{"interval",	required_argument,	NULL, 'I'},
	{"rem-count",	required_argument,	NULL, 'k'},
	{"loc-count",	required_argument,	NULL, 'K'},
	{"length",	required_argument,	NULL, 'l'},
	{"numcpus",	required_argument,	NULL, 'n'},
	{"rem-offset",	required_argument,	NULL, 'O'},
	{"loc-offset",	required_argument,	NULL, 'o'},
	{"print",	required_argument,	NULL, 'P'},
	{"port",	required_argument,	NULL, 'p'},
	{"testname",	required_argument,	NULL, 't'},
	{"verbose",	required_argument,	NULL, 'v'},
	{"version",	no_argument,		NULL, 'V'},
	{"width",	required_argument,	NULL, 'W'},
	{"rate",	required_argument,	NULL, 'w'},
	{"copying",	no_argument,		NULL, 'L'},
	{"ipv4",	no_argument,		NULL, '4'},
	{"ipv6",	no_argument,		NULL, '6'},
  };
#endif /* _GNU_SOURCE */
  
  program = (char *)malloc(strlen(argv[0]) + 1);
  if (program == NULL) {
    printf("malloc(%d) failed!\n", strlen(argv[0]) + 1);
    exit(1);
  }
  strcpy(program, argv[0]);
  
  /* Go through all the command line arguments and break them */
  /* out. For those options that take two parms, specifying only */
  /* the first will set both to that value. Specifying only the */
  /* second will leave the first untouched. To change only the */
  /* first, use the form first, (see the routine break_args.. */
  
#ifdef _GNU_SOURCE
  while ((c= getopt_long(argc, argv, GLOBAL_CMD_LINE_ARGS, long_options, &option_index)) != EOF) {
#else /* _GNU_SOURCE */
  while ((c= getopt(argc, argv, GLOBAL_CMD_LINE_ARGS)) != EOF) {
#endif /* _GNU_SOURCE */
    switch (c) {
    case '?':	
      print_netperf_usage(argc, argv);
      exit(1);
    case 'h':
      print_netperf_help(argc, argv);
      exit(0);
    case 'V':
      print_netperf_version(argc, argv);
      exit(0);
    case 'L':
      print_netperf_copying(argc, argv);
      exit(0);
    case 'a':
      /* set local alignments */
      break_args(optarg,arg1,arg2);
      if (arg1[0]) {
	local_send_align = convert(arg1);
      }
      if (arg2[0])
	local_recv_align = convert(arg2);
      break;
    case 'A':
      /* set remote alignments */
      break_args(optarg,arg1,arg2);
      if (arg1[0]) {
	remote_send_align = convert(arg1);
      }
      if (arg2[0])
	remote_recv_align = convert(arg2);
      break;
    case 'd':
      debug++;
      break;
    case 'D':
#if (defined INTERVALS) && (defined __hpux)
      demo_mode++;
#else /* INTERVALS __hpux */
      printf("Sorry, Demo Mode requires -DINTERVALS compilation \n");
      printf("as well as a mechanism to dynamically select syscall \n");
      printf("restart or interruption. I only know how to do this \n");
      printf("for HP-UX. Please examine the code in netlib.c:catcher() \n");
      printf("and let me know of more standard alternatives. \n");
      printf("                             Rick Jones <raj@cup.hp.com>\n");
      exit(1);
#endif /* INTERVALS __hpux */
      break;
    case 'f':
      /* set the thruput formatting */
      libfmt = *optarg;
      break;
    case 'F':
      /* set the fill_file variable for pre-filling buffers */
      strcpy(fill_file,optarg);
      break;
    case 'i':
      /* set the iterations min and max for confidence intervals */
      break_args(optarg,arg1,arg2);
      if (arg1[0]) {
	iteration_max = convert(arg1);
      }
      if (arg2[0] ) {
	iteration_min = convert(arg2);
      }
      /* limit maximum to 30 iterations */
      if(iteration_max>30) iteration_max=30;
      if(iteration_min>30) iteration_min=30;
      break;
    case 'I':
      /* set the confidence level (95 or 99) and width */
      break_args(optarg,arg1,arg2);
      if (arg1[0]) {
	confidence_level = convert(arg1);
      }
      if((confidence_level != 95) && (confidence_level != 99)){
	printf("Only 95%% and 99%% confidence level is supported\n");
	exit(1);
      }
      if (arg2[0] ) {
	interval = (double) convert(arg2)/100;
      }
      break;
    case 'k':
      /* local dirty and clean counts */
#ifdef DIRTY
      break_args(optarg,arg1,arg2);
      if (arg1[0]) {
	loc_dirty_count = convert(arg1);
      }
      if (arg2[0] ) {
	loc_clean_count = convert(arg2);
      }
#else
      printf("I don't know how to get dirty.\n");
#endif /* DIRTY */
      break;
    case 'K':
      /* remote dirty and clean counts */
#ifdef DIRTY
      break_args(optarg,arg1,arg2);
      if (arg1[0]) {
	rem_dirty_count = convert(arg1);
      }
      if (arg2[0] ) {
	rem_clean_count = convert(arg2);
      }
#else
      printf("I don't know how to get dirty.\n");
#endif /* DIRTY */
      break;
    case 'n':
      shell_num_cpus = atoi(optarg);
      break;
    case 'o':
      /* set the local offsets */
      break_args(optarg,arg1,arg2);
      if (arg1[0])
	local_send_offset = convert(arg1);
      if (arg2[0])
	local_recv_offset = convert(arg2);
      break;
    case 'O':
      /* set the remote offsets */
      break_args(optarg,arg1,arg2);
      if (arg1[0]) 
	remote_send_offset = convert(arg1);
      if (arg2[0])
	remote_recv_offset = convert(arg2);
      break;
    case 'P':
      /* to print or not to print, that is */
      /* the header question */
      print_headers = convert(optarg);
      break;
    case 't':
      /* set the test name */
      strcpy(test_name,optarg);
      break;
    case 'T':
      /* We want to set the processor on which netserver */
      /* will run on the remote system. */
      proc_affinity = convert(optarg);
      break;
    case 'W':
      /* set the "width" of the user space data buffer ring. This will */
      /* be the number of send_size buffers malloc'd in the tests */  
      break_args(optarg,arg1,arg2);
      if (arg1[0]) 
	send_width = convert(arg1);
      if (arg2[0])
	recv_width = convert(arg2);
      break;
    case 'l':
      /* determine test end conditions */
      /* assume a timed test */
      test_time = convert(optarg);
      test_bytes = test_trans = 0;
      if (test_time < 0) {
	test_bytes = -1 * test_time;
	test_trans = test_bytes;
	test_time = 0;
      }
      break;
    case 'v':
      /* say how much to say */
      verbosity = convert(optarg);
      break;
    case 'c':
      /* measure local cpu usage please. the user */
      /* may have specified the cpu rate as an */
#ifdef _GNU_SOURCE
      if (optarg)
	local_cpu_rate = (float)atof(optarg);
#else /* _GNU_SOURCE */
      /* optional parm */
      if (argv[optind] && isdigit((unsigned char)argv[optind][0])){
	/* there was an optional parm */
	local_cpu_rate = (float)atof(argv[optind]);
	optind++;
      }
#endif /* _GNU_SOURCE */
      local_cpu_usage++;
      break;
    case 'C':
      /* measure remote cpu usage please */
#ifdef _GNU_SOURCE
      if (optarg)
	remote_cpu_rate = (float)atof(optarg);
#else /* _GNU_SOURCE */
      if (argv[optind] && isdigit((unsigned char)argv[optind][0])){
	/* there was an optional parm */
	remote_cpu_rate = (float)atof(argv[optind]);
	optind++;
      }
#endif /* _GNU_SOURCE */
      remote_cpu_usage++;
      break;
    case 'p':
      /* specify an alternate port number */
      test_port = (short) convert(optarg);
      break;
    case 'H':
      /* save-off the host identifying information */
      strcpy(host_name,optarg);
      break;
    case 'w':
      /* We want to send requests at a certain wate. */
      /* Remember that there are 1000000 usecs in a */
      /* second, and that the packet rate is */
      /* expressed in packets per millisecond. */
#ifdef INTERVALS
      interval_wate  = convert(optarg);
      interval_usecs = interval_wate * 1000;
#else
      fprintf(where,
	      "Packet rate control is not compiled in.\n");
#endif
      break;
    case 'b':
      /* we want to have a burst so many packets per */
      /* interval. */
#ifdef INTERVALS
      interval_burst = convert(optarg);
#else
      fprintf(where,
	      "Packet burst size is not compiled in. \n");
#endif /* INTERVALS */
      break;
    };
  }
  /* we have encountered a -- in global command-line */
  /* processing and assume that this means we have gotten to the */
  /* test specific options. this is a bit kludgy and if anyone has */
  /* a better solution, i would love to see it */
  if (optind != argc) {
    if (
#ifdef DO_SCTP
	(strcasecmp(test_name,"SCTP_STREAM") == 0) || 
#ifdef HAVE_SENDFILE
	(strcasecmp(test_name,"SCTP_SENDFILE") == 0) ||
#endif /* HAVE_SENDFILE */
	(strcasecmp(test_name,"SCTP_MAERTS") == 0) ||
	(strcasecmp(test_name,"SCTP_RR") == 0) ||
	(strcasecmp(test_name,"SCTP_CRR") == 0) ||
#ifdef DO_1644
	(strcasecmp(test_name,"SCTP_TRR") == 0) ||
#endif /* DO_1644 */
#ifdef DO_NBRR
	(strcasecmp(test_name,"SCTP_NBRR") == 0) ||
#endif /* DO_NBRR */
#endif				/* DO_SCTP */
	(strcasecmp(test_name,"TCP_STREAM") == 0) || 
#ifdef HAVE_SENDFILE
	(strcasecmp(test_name,"TCP_SENDFILE") == 0) ||
#endif /* HAVE_SENDFILE */
	(strcasecmp(test_name,"TCP_MAERTS") == 0) ||
	(strcasecmp(test_name,"TCP_RR") == 0) ||
	(strcasecmp(test_name,"TCP_CRR") == 0) ||
#ifdef DO_1644
	(strcasecmp(test_name,"TCP_TRR") == 0) ||
#endif /* DO_1644 */
#ifdef DO_NBRR
	(strcasecmp(test_name,"TCP_NBRR") == 0) ||
#endif /* DO_NBRR */
	(strcasecmp(test_name,"UDP_STREAM") == 0) ||
	(strcasecmp(test_name,"UDP_RR") == 0))
      {
	scan_sockets_args(argc, argv);
      }
#ifdef DO_DLPI
    else if ((strcasecmp(test_name,"DLCO_RR") == 0) ||
	     (strcasecmp(test_name,"DLCL_RR") == 0) ||
	     (strcasecmp(test_name,"DLCO_STREAM") == 0) ||
	     (strcasecmp(test_name,"DLCL_STREAM") == 0))
      {
	scan_dlpi_args(argc, argv);
      }
#endif /* DO_DLPI */
#ifdef DO_UNIX
    else if ((strcasecmp(test_name,"STREAM_RR") == 0) ||
	     (strcasecmp(test_name,"DG_RR") == 0) ||
	     (strcasecmp(test_name,"STREAM_STREAM") == 0) ||
	     (strcasecmp(test_name,"DG_STREAM") == 0))
      {
	scan_unix_args(argc, argv);
      }
#endif /* DO_UNIX */
#ifdef DO_FORE
    else if ((strcasecmp(test_name,"FORE_RR") == 0) ||
	     (strcasecmp(test_name,"FORE_STREAM") == 0))
      {
	scan_fore_args(argc, argv);
      }
#endif /* DO_FORE */
#ifdef DO_HIPPI
    else if ((strcasecmp(test_name,"HIPPI_RR") == 0) ||
	     (strcasecmp(test_name,"HIPPI_STREAM") == 0))
      {
	scan_hippi_args(argc, argv);
      }
#endif /* DO_HIPPI */
#ifdef DO_XTI
    else if (
#ifdef DO_XTI_SCTP
	     (strcasecmp(test_name,"XTI_SCTP_RR") == 0) ||
	     (strcasecmp(test_name,"XTI_SCTP_STREAM") == 0) ||
#endif				/* DO_XTI_SCTP */
	     (strcasecmp(test_name,"XTI_TCP_RR") == 0) ||
	     (strcasecmp(test_name,"XTI_TCP_STREAM") == 0) ||
	     (strcasecmp(test_name,"XTI_UDP_RR") == 0) ||
	     (strcasecmp(test_name,"XTI_UDP_STREAM") == 0))
      {
	scan_xti_args(argc, argv);
      }
#endif /* DO_XTI */
#ifdef DO_LWP
    else if ((strcasecmp(test_name,"LWPSTR_RR") == 0) ||
	     (strcasecmp(test_name,"LWPSTR_STREAM") == 0) ||
	     (strcasecmp(test_name,"LWPDG_RR") == 0) ||
	     (strcasecmp(test_name,"LWPDG_STREAM") == 0))
      {
	scan_lwp_args(argc, argv);
      }
#endif /* DO_LWP */
#ifdef DO_IPV6
    else if (
#ifdef DO_IPV6_SCTP
	     (strcasecmp(test_name,"SCTPIPV6_RR") == 0) ||
	     (strcasecmp(test_name,"SCTPIPV6_CRR") == 0) ||
	     (strcasecmp(test_name,"SCTPIPV6_STREAM") == 0) ||
#endif				/* DO_IPV6_SCTP */
	     (strcasecmp(test_name,"TCPIPV6_RR") == 0) ||
	     (strcasecmp(test_name,"TCPIPV6_CRR") == 0) ||
	     (strcasecmp(test_name,"TCPIPV6_STREAM") == 0) ||
	     (strcasecmp(test_name,"UDPIPV6_RR") == 0) ||
	     (strcasecmp(test_name,"UDPIPV6_STREAM") == 0))
      {
	scan_ipv6_args(argc, argv);
      }
#endif /* DO_IPV6 */
#ifdef DO_DNS
    else if (strcasecmp(test_name,"DNS_RR") == 0)
      {
	scan_dns_args(argc, argv);
      }
#endif /* DO_DNS */
  }

#ifdef DO_IPV6
  /* address family check */   
  if (
#ifdef DO_IPV6_SCTP
      (strcasecmp(test_name,"SCTPIPV6_RR") == 0) ||
      (strcasecmp(test_name,"SCTPIPV6_CRR") == 0) ||
      (strcasecmp(test_name,"SCTPIPV6_STREAM") == 0) ||
#endif				/* DO_IPV6_SCTP */
      (strcasecmp(test_name,"TCPIPV6_RR") == 0) ||
      (strcasecmp(test_name,"TCPIPV6_CRR") == 0) ||
      (strcasecmp(test_name,"TCPIPV6_STREAM") == 0) ||
      (strcasecmp(test_name,"UDPIPV6_RR") == 0) ||
      (strcasecmp(test_name,"UDPIPV6_STREAM") == 0))
    {
      af = AF_INET6;
    }
#endif /* DO_IPV6 */
}


void
dump_globals(void)
{
  printf("Program name: %s\n", program);
  printf("Local send alignment: %d\n",local_send_align);
  printf("Local recv alignment: %d\n",local_recv_align);
  printf("Remote send alignment: %d\n",remote_send_align);
  printf("Remote recv alignment: %d\n",remote_recv_align);
  printf("Report local CPU %d\n",local_cpu_usage);
  printf("Report remote CPU %d\n",remote_cpu_usage);
  printf("Verbosity: %d\n",verbosity);
  printf("Debug: %d\n",debug);
  printf("Port: %d\n",test_port);
  printf("Test name: %s\n",test_name);
  printf("Test bytes: %d Test time: %d Test trans: %d\n",
	 test_bytes,
	 test_time,
	 test_trans);
  printf("Host name: %s\n",host_name);
  printf("\n");
}
