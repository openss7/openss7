/*****************************************************************************

 @(#) $RCSfile: netperf.c,v $ $Name:  $($Revision: 1.1.1.5 $) $Date: 2005/02/06 06:43:02 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/02/06 06:43:02 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: netperf.c,v $ $Name:  $($Revision: 1.1.1.5 $) $Date: 2005/02/06 06:43:02 $"

static char const ident[] = "$RCSfile: netperf.c,v $ $Name:  $($Revision: 1.1.1.5 $) $Date: 2005/02/06 06:43:02 $";

#ifdef NEED_MAKEFILE_EDIT
#error you must first edit and customize the makefile to your platform
#endif /* NEED_MAKEFILE_EDIT */
/*
 
	   Copyright (C) 1993-2001 Hewlett-Packard Company
                         ALL RIGHTS RESERVED.
 
  The enclosed software and documention includes copyrighted works of
  Hewlett-Packard Co. For as long as you comply with the following
  limitations, you are hereby authorized to (i) use, reproduce, and
  modify the software and documentation, and to (ii) distribute the
  software and documentation, including modifications, for
  non-commercial purposes only.
      
  1.  The enclosed software and documentation is made available at no
      charge in order to advance the general development of
      high-performance networking products.
 
  2.  You may not delete any copyright notices contained in the
      software or documentation. All hard copies, and copies in
      source code or object code form, of the software or
      documentation (including modifications) must contain at least
      one of the copyright notices.
 
  3.  The enclosed software and documentation has not been subjected
      to testing and quality control and is not a Hewlett-Packard Co.
      product. At a future time, Hewlett-Packard Co. may or may not
      offer a version of the software and documentation as a product.
  
  4.  THE SOFTWARE AND DOCUMENTATION IS PROVIDED "AS IS".
      HEWLETT-PACKARD COMPANY DOES NOT WARRANT THAT THE USE,
      REPRODUCTION, MODIFICATION OR DISTRIBUTION OF THE SOFTWARE OR
      DOCUMENTATION WILL NOT INFRINGE A THIRD PARTY'S INTELLECTUAL
      PROPERTY RIGHTS. HP DOES NOT WARRANT THAT THE SOFTWARE OR
      DOCUMENTATION IS ERROR FREE. HP DISCLAIMS ALL WARRANTIES,
      EXPRESS AND IMPLIED, WITH REGARD TO THE SOFTWARE AND THE
      DOCUMENTATION. HP SPECIFICALLY DISCLAIMS ALL WARRANTIES OF
      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
  
  5.  HEWLETT-PACKARD COMPANY WILL NOT IN ANY EVENT BE LIABLE FOR ANY
      DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES
      (INCLUDING LOST PROFITS) RELATED TO ANY USE, REPRODUCTION,
      MODIFICATION, OR DISTRIBUTION OF THE SOFTWARE OR DOCUMENTATION.
 
*/
char	netperf_id[]="@(#)netperf.c (c) Copyright 1993-2004 Hewlett-Packard Company. Version 2.3";

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

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif /* WIN32 */

#include "netsh.h"
#include "netlib.h"
#include "nettest_bsd.h"
#ifdef DO_UNIX
#include "nettest_unix.h"
#endif /* DO_UNIX */
#ifdef DO_FORE
#include "nettest_fore.h"
#endif /* DO_FORE */
#ifdef DO_HIPPI
#include "nettest_hippi.h"
#endif /* DO_HIPPI */
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

 /* this file contains the main for the netperf program. all the other */
 /* routines can be found in the file netsh.c */


int _cdecl
main(int argc, char *argv[])
{

#ifdef WIN32
	WSADATA	wsa_data ;

	/* Initialize the winsock lib ( version 2.2 ) */
	if ( WSAStartup(MAKEWORD(2,2), &wsa_data) == SOCKET_ERROR ){
		printf("WSAStartup() failed : %d\n", GetLastError()) ;
		return 1 ;
	}
#endif /* WIN32 */

netlib_init();
set_defaults();
scan_cmd_line(argc,argv);

if (debug) {
	dump_globals();
	install_signal_catchers();
}

if (debug) {
	printf("remotehost is %s and port %d\n",host_name,test_port);
	fflush(stdout);
}


establish_control(host_name,test_port);

if (0) {
}
#ifdef DO_SCTP
else if (strcasecmp(test_name,"SCTP_STREAM") == 0) {
	send_sctp_stream(host_name);
}
else if (strcasecmp(test_name,"SCTP_MAERTS") == 0) {
	send_sctp_maerts(host_name);
}
#ifdef HAVE_SENDFILE
else if (strcasecmp(test_name,"SCTP_SENDFILE") == 0) {
	sendfile_sctp_stream(host_name);
}
#endif /* HAVE_SENDFILE */
else if (strcasecmp(test_name,"SCTP_RR") == 0) {
	send_sctp_rr(host_name);
}
else if (strcasecmp(test_name,"SCTP_CRR") == 0) {
	send_sctp_conn_rr(host_name);
}
else if (strcasecmp(test_name,"SCTP_CC") == 0) {
	send_sctp_cc(host_name);
}
#ifdef DO_1644
else if (strcasecmp(test_name,"SCTP_TRR") == 0) {
	send_sctp_tran_rr(host_name);
}
#endif /* DO_1644 */
#ifdef DO_NBRR
else if (strcasecmp(test_name,"SCTP_NBRR") == 0) {
	send_sctp_nbrr(host_name);
}
#endif /* DO_NBRR */
#endif	/* DO_SCTP */
else if (strcasecmp(test_name,"TCP_STREAM") == 0) {
	send_tcp_stream(host_name);
}
else if (strcasecmp(test_name,"TCP_MAERTS") == 0) {
	send_tcp_maerts(host_name);
}
#ifdef HAVE_SENDFILE
else if (strcasecmp(test_name,"TCP_SENDFILE") == 0) {
	sendfile_tcp_stream(host_name);
}
#endif /* HAVE_SENDFILE */
else if (strcasecmp(test_name,"TCP_RR") == 0) {
	send_tcp_rr(host_name);
}
else if (strcasecmp(test_name,"TCP_CRR") == 0) {
	send_tcp_conn_rr(host_name);
}
else if (strcasecmp(test_name,"TCP_CC") == 0) {
	send_tcp_cc(host_name);
}
#ifdef DO_1644
else if (strcasecmp(test_name,"TCP_TRR") == 0) {
	send_tcp_tran_rr(host_name);
}
#endif /* DO_1644 */
#ifdef DO_NBRR
else if (strcasecmp(test_name,"TCP_NBRR") == 0) {
	send_tcp_nbrr(host_name);
}
#endif /* DO_NBRR */
else if (strcasecmp(test_name,"UDP_STREAM") == 0) {
	send_udp_stream(host_name);
}
else if (strcasecmp(test_name,"UDP_RR") == 0) {
	send_udp_rr(host_name);
}
else if (strcasecmp(test_name,"LOC_CPU") == 0) {
	loc_cpu_rate();
}
else if (strcasecmp(test_name,"REM_CPU") == 0) {
	rem_cpu_rate();
}
#ifdef DO_DLPI
else if (strcasecmp(test_name,"DLCO_RR") == 0) {
	send_dlpi_co_rr(host_name);
}
else if (strcasecmp(test_name,"DLCL_RR") == 0) {
	send_dlpi_cl_rr(host_name);
}
else if (strcasecmp(test_name,"DLCO_STREAM") == 0) {
	send_dlpi_co_stream(host_name);
}
else if (strcasecmp(test_name,"DLCL_STREAM") == 0) {
	send_dlpi_cl_stream(host_name);
}
#endif /* DO_DLPI */
#ifdef DO_UNIX
else if (strcasecmp(test_name,"STREAM_RR") == 0) {
	send_stream_rr(host_name);
}
else if (strcasecmp(test_name,"DG_RR") == 0) {
	send_dg_rr(host_name);
}
else if (strcasecmp(test_name,"STREAM_STREAM") == 0) {
	send_stream_stream(host_name);
}
else if (strcasecmp(test_name,"DG_STREAM") == 0) {
	send_dg_stream(host_name);
}
#endif /* DO_UNIX */
#ifdef DO_FORE
else if (strcasecmp(test_name,"FORE_STREAM") == 0) {
	send_fore_stream(host_name);
}
else if (strcasecmp(test_name,"FORE_RR") == 0) {
	send_fore_rr(host_name);
}
#endif /* DO_FORE */
#ifdef DO_HIPPI
else if (strcasecmp(test_name,"HIPPI_STREAM") == 0) {
	send_hippi_stream(host_name);
}
else if (strcasecmp(test_name,"HIPPI_RR") == 0) {
	send_hippi_rr(host_name);
}
#endif /* DO_HIPPI */
#ifdef DO_XTI
#ifdef DO_XTI_SCTP
else if (strcasecmp(test_name,"XTI_SCTP_STREAM") == 0) {
	send_xti_sctp_stream(host_name);
}
else if (strcasecmp(test_name,"XTI_SCTP_RR") == 0) {
	send_xti_sctp_rr(host_name);
}
#endif	/* DO_XTI_SCTP */
else if (strcasecmp(test_name,"XTI_TCP_STREAM") == 0) {
	send_xti_tcp_stream(host_name);
}
else if (strcasecmp(test_name,"XTI_TCP_RR") == 0) {
	send_xti_tcp_rr(host_name);
}
else if (strcasecmp(test_name,"XTI_UDP_STREAM") == 0) {
	send_xti_udp_stream(host_name);
}
else if (strcasecmp(test_name,"XTI_UDP_RR") == 0) {
	send_xti_udp_rr(host_name);
}
#endif /* DO_XTI */
#ifdef DO_LWP
else if (strcasecmp(test_name,"LWPSTR_STREAM") == 0) {
	send_lwpstr_stream(host_name);
}
else if (strcasecmp(test_name,"LWPSTR_RR") == 0) {
	send_lwpstr_rr(host_name);
}
else if (strcasecmp(test_name,"LWPDG_STREAM") == 0) {
	send_lwpdg_stream(host_name);
}
else if (strcasecmp(test_name,"LWPDG_RR") == 0) {
	send_lwpdg_rr(host_name);
}
#endif /* DO_LWP */
#ifdef DO_IPV6
#ifdef DO_IPV6_SCTP
else if (strcasecmp(test_name,"SCTPIPV6_STREAM") == 0) {
	send_sctpipv6_stream(host_name);
}
else if (strcasecmp(test_name,"SCTPIPV6_RR") == 0) {
	send_sctpipv6_rr(host_name);
}
else if (strcasecmp(test_name,"SCTPIPV6_CRR") == 0) {
	send_sctpipv6_rr(host_name);
}
#endif	/* DO_IPV6_SCTP */
else if (strcasecmp(test_name,"TCPIPV6_STREAM") == 0) {
	send_tcpipv6_stream(host_name);
}
else if (strcasecmp(test_name,"TCPIPV6_RR") == 0) {
	send_tcpipv6_rr(host_name);
}
else if (strcasecmp(test_name,"TCPIPV6_CRR") == 0) {
	send_tcpipv6_rr(host_name);
}
else if (strcasecmp(test_name,"UDPIPV6_STREAM") == 0) {
	send_udpipv6_stream(host_name);
}
else if (strcasecmp(test_name,"UDPIPV6_RR") == 0) {
	send_udpipv6_rr(host_name);
}
#endif /* DO_IPV6 */
#ifdef DO_DNS
else if (strcasecmp(test_name,"DNS_RR") == 0) {
	send_dns_rr(host_name);
}
#endif /* DO_DNS */
else {
	printf("The test you requested is unknown to this netperf.\n");
	printf("Please verify that you have the correct test name, \n");
	printf("and that test family has been compiled into this netperf.\n");
	exit(1);
}

shutdown_control();

#ifdef WIN32
	/* Cleanup the winsock lib */
	WSACleanup();
#endif

return(0);
}


