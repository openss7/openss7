/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/*--------------------------------------------------------------- 
 * Copyright (c) 1999,2000,2001,2002,2003                              
 * The Board of Trustees of the University of Illinois            
 * All Rights Reserved.                                           
 *--------------------------------------------------------------- 
 * Permission is hereby granted, free of charge, to any person    
 * obtaining a copy of this software (Iperf) and associated       
 * documentation files (the "Software"), to deal in the Software  
 * without restriction, including without limitation the          
 * rights to use, copy, modify, merge, publish, distribute,        
 * sublicense, and/or sell copies of the Software, and to permit     
 * persons to whom the Software is furnished to do
 * so, subject to the following conditions: 
 *
 *     
 * Redistributions of source code must retain the above 
 * copyright notice, this list of conditions and 
 * the following disclaimers. 
 *
 *     
 * Redistributions in binary form must reproduce the above 
 * copyright notice, this list of conditions and the following 
 * disclaimers in the documentation and/or other materials 
 * provided with the distribution. 
 * 
 *     
 * Neither the names of the University of Illinois, NCSA, 
 * nor the names of its contributors may be used to endorse 
 * or promote products derived from this Software without
 * specific prior written permission. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE CONTIBUTORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 * ________________________________________________________________
 * National Laboratory for Applied Network Research 
 * National Center for Supercomputing Applications 
 * University of Illinois at Urbana-Champaign 
 * http://www.ncsa.uiuc.edu
 * ________________________________________________________________ 
 *
 * Locale.c
 * by Ajay Tirumala <tirumala@ncsa.uiuc.edu>
 * & Mark Gates <mgates@nlanr.net>
 * -------------------------------------------------------------------
 * Strings and other stuff that is locale specific.
 * ------------------------------------------------------------------- */
#include "version.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#ifdef WIN32
#include "config.win32.h"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif
/* -------------------------------------------------------------------
 * usage
 * ------------------------------------------------------------------- */

const char usage_short[] = "\
Usage: %s [-s|-c host] [options]\n\
Try `%s --help' for more information.\n";

#ifdef WIN32
const char usage_long1[] = "\
Usage: iperf [-s|-c host] [options]\n\
       iperf [-h|--help] [-v|--version]\n\
\n\
Client/Server:\n\
  -f, --format    [kmKM]   format to report: Kbits, Mbits, KBytes, MBytes\n\
  -i, --interval  #        seconds between periodic bandwidth reports\n\
  -l, --len       #[KM]    length of buffer to read or write (default 8 KB)\n\
  -m, --print_mss          print SCTP/TCP maximum segment size (MTU - SCTP/TCP/IP header)\n\
  -o, --output    <filename> output the report or error message to this specified file\n\
  -p, --port      #        server port to listen on/connect to\n\
  -q, --seqpacket          for SCTP use SOCK_SEQPACKET instead of SOCK_STREAM\n\
  -u, --udp                use UDP rather than TCP\n\
  -w, --window    #[KM]    SCTP/TCP window size (socket buffer size)\n\
  -z, --sctp               use SCTP (rather than UDP or TCP)\n\
  -B, --bind      <host>   bind to <host>, an interface or multicast address\n\
  -C, --compatibility      for use with older versions does not sent extra msgs\n\
  -M, --mss       #        set SCTP/TCP maximum segment size (MTU - 40 bytes)\n\
  -N, --nodelay            set SCTP/TCP no delay, disabling Nagle's Algorithm\n\
  -V, --IPv6Version        Set the domain to IPv6\n\
\n\
Server specific:\n\
  -s, --server             run in server mode\n\
  -U, --single_udp         run in single threaded UDP mode\n\
  -D, --daemon             run the server as a daemon\n\
  -R, --remove             remove service in win32\n";

const char usage_long2[] = "\
\n\
Client specific:\n\
  -b, --bandwidth #[KM]    for UDP, bandwidth to send at in bits/sec\n\
                           (default 1 Mbit/sec, implies -u)\n\
  -c, --client    <host>   run in client mode, connecting to <host>\n\
  -d, --dualtest           Do a bidirectional test simultaneously\n\
  -n, --num       #[KM]    number of bytes to transmit (instead of -t)\n\
  -r, --tradeoff           Do a bidirectional test individually\n\
  -t, --time      #        time in seconds to transmit for (default 10 secs)\n\
  -F, --fileinput <name>   input the data to be transmitted from a file\n\
  -I, --stdin              input the data to be transmitted from stdin\n\
  -L, --listenport #       port to recieve bidirectional tests back on\n\
  -P, --parallel  #        number of parallel client threads to run\n\
  -T, --ttl       #        time-to-live, for multicast (default 1)\n\
\n\
Miscellaneous:\n\
  -h, --help               print this message and quit\n\
  -v, --version            print version information and quit\n\
  --copying                print copying information and quit\n\
\n\
[KM] Indicates options that support a K or M suffix for kilo- or mega-\n\
\n\
The SCTP/TCP window size option can be set by the environment variable\n\
TCP_WINDOW_SIZE. Most other options can be set by an environment variable\n\
IPERF_<long option name>, such as IPERF_BANDWIDTH.\n\
\n\
Report bugs to <dast@nlanr.net>\n";

#else
const char usage_long[] = "\
Usage: iperf [-s|-c host] [options]\n\
       iperf [-h|--help] [-v|--version]\n\
\n\
Client/Server:\n\
  -f, --format    [kmKM]   format to report: Kbits, Mbits, KBytes, MBytes\n\
  -i, --interval  #        seconds between periodic bandwidth reports\n\
  -l, --len       #[KM]    length of buffer to read or write (default 8 KB)\n\
  -m, --print_mss          print SCTP/TCP maximum segment size (MTU - SCTP/TCP/IP header)\n\
  -p, --port      #        server port to listen on/connect to\n\
  -q, --seqpacket          for SCTP use SOCK_SEQPACKET instead of SOCK_STREAM\n\
  -u, --udp                use UDP rather than TCP\n\
  -w, --window    #[KM]    SCTP/TCP window size (socket buffer size)\n\
  -z, --sctp               use SCTP (rather than UDP or TCP)\n\
  -B, --bind      <host>   bind to <host>, an interface or multicast address\n\
  -C, --compatibility      for use with older versions does not sent extra msgs\n\
  -M, --mss       #        set SCTP/TCP maximum segment size (MTU - 40 bytes)\n\
  -N, --nodelay            set SCTP/TCP no delay, disabling Nagle's Algorithm\n\
  -V, --IPv6Version        Set the domain to IPv6\n\
\n\
Server specific:\n\
  -s, --server             run in server mode\n\
  -U, --single_udp         run in single threaded UDP mode\n\
  -D, --daemon             run the server as a daemon\n\
\n\
Client specific:\n\
  -b, --bandwidth #[KM]    for UDP, bandwidth to send at in bits/sec\n\
                           (default 1 Mbit/sec, implies -u)\n\
  -c, --client    <host>   run in client mode, connecting to <host>\n\
  -d, --dualtest           Do a bidirectional test simultaneously\n\
  -n, --num       #[KM]    number of bytes to transmit (instead of -t)\n\
  -r, --tradeoff           Do a bidirectional test individually\n\
  -t, --time      #        time in seconds to transmit for (default 10 secs)\n\
  -F, --fileinput <name>   input the data to be transmitted from a file\n\
  -I, --stdin              input the data to be transmitted from stdin\n\
  -L, --listenport #       port to recieve bidirectional tests back on\n\
  -P, --parallel  #        number of parallel client threads to run\n\
  -T, --ttl       #        time-to-live, for multicast (default 1)\n\
\n\
Miscellaneous:\n\
  -h, --help               print this message and quit\n\
  -v, --version            print version information and quit\n\
  --copying                print copying information and quit\n\
\n\
[KM] Indicates options that support a K or M suffix for kilo- or mega-\n\
\n\
The SCTP/TCP window size option can be set by the environment variable\n\
TCP_WINDOW_SIZE. Most other options can be set by an environment variable\n\
IPERF_<long option name>, such as IPERF_BANDWIDTH.\n\
\n\
Report bugs to <dast@nlanr.net>\n";
#endif

// include a description of the threading in the version
#ifdef HAVE_POSIX_THREAD
    #define IPERF_THREADS "pthreads"
#else
#ifdef HAVE_WIN32_THREAD
    #define IPERF_THREADS "win32 threads"
#else
    #define IPERF_THREADS "single threaded"
#endif
#endif

const char version[] =
"iperf version " IPERF_VERSION " (" IPERF_VERSION_DATE ") " IPERF_THREADS "\n";

const char copying[] = "\n\
 ----------------------------------------------------------------------------\n\
\n\
 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>\n\
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
 All Rights Reserved.\n\
\n\
 This program is free software; you can redistribute it and/or modify it under\n\
 the terms of the GNU General Public License as published by the Free Software\n\
 Foundation; either version 2 of the License, or (at your option) any later\n\
 version.\n\
\n\
 This program is distributed in the hope that it will be useful, but WITHOUT\n\
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n\
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more\n\
 details.\n\
\n\
 You should have received a copy of the GNU General Public License along with\n\
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass\n\
 Ave, Cambridge, MA 02139, USA.\n\
\n\
 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on\n\
 behalf of the U.S. Government (\"Government\"), the following provisions apply\n\
 to you.  If the Software is supplied by the Department of Defense (\"DoD\"), it\n\
 is classified as \"Commercial Computer Software\" under paragraph 252.227-7014\n\
 of the DoD Supplement to the Federal Acquisition Regulations (\"DFARS\") (or any\n\
 successor regulations) and the Government is acquiring only the license rights\n\
 granted herein (the license rights customarily provided to non-Government\n\
 users).  If the Software is supplied to any unit or agency of the Government\n\
 other than DoD, it is classified as \"Restricted Computer Software\" and the\n\
 Government's rights in the Software are defined in paragraph 52.227-19 of the\n\
 Federal Acquisition Regulations (\"FAR\") (or any successor regulations) or, in\n\
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR\n\
 (or any successor regulations).\n\
\n\
 Commercial licensing and support of this software is available from OpenSS7\n\
 Corporation at a fee.  See http://www.openss7.com/\n\
\n\
 ----------------------------------------------------------------------------\n\
\n\
 Copyright (c) 1999,2000,2001,2002,2003\n\
 The Board of Trustees of the University of Illinois\n\
 All Rights Reserved.\n\
\n\
 Permission is hereby granted, free of charge, to any person obtaining a copy of\n\
 this software (Iperf) and associated documentation files (the \"Software\"), to\n\
 deal in the Software without restriction, including without limitation the\n\
 rights to use, copy, modify, merge, publish, distribute, sublicense, and/or\n\
 sell copies of the Software, and to permit persons to whom the Software is\n\
 furnished to do so, subject to the following conditions: \n\
\n\
 Redistributions of source code must retain the above copyright notice, this\n\
 list of conditions and the following disclaimers. \n\
\n\
 Redistributions in binary form must reproduce the above copyright notice, this\n\
 list of conditions and the following disclaimers in the documentation and/or\n\
 other materials provided with the distribution. \n\
\n\
 Neither the names of the University of Illinois, NCSA, nor the names of its\n\
 contributors may be used to endorse or promote products derived from this\n\
 Software without specific prior written permission. \n\
\n\
 THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n\
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n\
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n\
 CONTIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n\
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n\
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n\
 SOFTWARE. \n\
\n\
 National Laboratory for Applied Network Research \n\
 National Center for Supercomputing Applications \n\
 University of Illinois at Urbana-Champaign \n\
 http://www.ncsa.uiuc.edu\n\
\n\
 ----------------------------------------------------------------------------\n\
\n\
";
/* -------------------------------------------------------------------
 * settings
 * ------------------------------------------------------------------- */

const char seperator_line[] =
"------------------------------------------------------------\n";

const char server_port[] =
"Server listening on %s port %d\n";

const char client_port[] =
"Client connecting to %s, %s port %d\n";

const char bind_address[] =
"Binding to local address %s\n";

const char multicast_ttl[] =
"Setting multicast TTL to %d\n";

const char join_multicast[] =
"Joining multicast group  %s\n";

const char client_datagram_size[] =
"Sending %d byte datagrams\n";

const char server_datagram_size[] =
"Receiving %d byte datagrams\n";

const char sctp_window_size[] =
"SCTP window size";

const char tcp_window_size[] =
"TCP window size";

const char udp_buffer_size[] =
"UDP buffer size";

const char window_default[] =
"(default)";

const char wait_server_threads[] =
"Waiting for server threads to complete. Interrupt again to force quit.\n";

/* -------------------------------------------------------------------
 * reports
 * ------------------------------------------------------------------- */

const char report_read_lengths[] =
"[%3d] Read lengths occurring in more than 5%% of reads:\n";

const char report_read_length_times[] =
"[%3d] %5d bytes read %5d times (%.3g%%)\n";

const char report_bw_header[] =
"[ ID] Interval       Transfer     Bandwidth\n";

const char report_bw_format[] =
"[%3d] %4.1f-%4.1f sec  %ss  %ss/sec\n";

const char report_sum_bw_format[] =
"[SUM] %4.1f-%4.1f sec  %ss  %ss/sec\n";

const char report_bw_jitter_loss_header[] =
"[ ID] Interval       Transfer     Bandwidth       Jitter   Lost/Total \
Datagrams\n";

const char report_bw_jitter_loss_format[] =
"[%3d] %4.1f-%4.1f sec  %ss  %ss/sec  %5.3f ms %4d/%5d (%.2g%%)\n";

const char report_sum_bw_jitter_loss_format[] =
"[SUM] %4.1f-%4.1f sec  %ss  %ss/sec  %5.3f ms %4d/%5d (%.2g%%)\n";

const char report_outoforder[] =
"[%3d] %4.1f-%4.1f sec  %d datagrams received out-of-order\n";

const char report_sum_outoforder[] =
"[SUM] %4.1f-%4.1f sec  %d datagrams received out-of-order\n";

const char report_peer[] =
"[%3d] local %s port %u connected with %s port %u\n";

const char report_mss_unsupported[] =
"[%3d] MSS and MTU size unknown (TCP_MAXSEG not supported by OS?)\n";

const char report_mss[] =
"[%3d] MSS size %d bytes (MTU %d bytes, %s)\n";

const char report_datagrams[] =
"[%3d] Sent %d datagrams\n";

const char report_sum_datagrams[] =
"[SUM] Sent %d datagrams\n";

const char server_reporting[] =
"[%3d] Server Report:\n";

const char reportCSV_peer[] =
"%s,%u,%s,%u";

#ifdef HAVE_QUAD_SUPPORT
#ifdef HAVE_PRINTF_QD
const char reportCSV_bw_format[] =
"%s,%s,%d,%.1f-%.1f,%qd,%qd\n";

const char reportCSV_bw_jitter_loss_format[] =
"%s,%s,%d,%.1f-%.1f,%qd,%qd,%.3f,%d,%d,%.3f,%d\n";
#else // HAVE_PRINTF_QD
const char reportCSV_bw_format[] =
"%s,%s,%d,%.1f-%.1f,%lld,%lld\n";

const char reportCSV_bw_jitter_loss_format[] =
"%s,%s,%d,%.1f-%.1f,%lld,%lld,%.3f,%d,%d,%.3f,%d\n";
#endif // HAVE_PRINTF_QD
#else // HAVE_QUAD_SUPPORT
#ifdef WIN32
const char reportCSV_bw_format[] =
"%s,%s,%d,%.1f-%.1f,%I64d,%I64d\n";

const char reportCSV_bw_jitter_loss_format[] =
"%s,%s,%d,%.1f-%.1f,%I64d,%I64d,%.3f,%d,%d,%.3f,%d\n";
#else
const char reportCSV_bw_format[] =
"%s,%s,%d,%.1f-%.1f,%d,%d\n";

const char reportCSV_bw_jitter_loss_format[] =
"%s,%s,%d,%.1f-%.1f,%d,%d,%.3f,%d,%d,%.3f,%d\n";
#endif //WIN32
#endif //HAVE_QUAD_SUPPORT
/* -------------------------------------------------------------------
 * warnings
 * ------------------------------------------------------------------- */

const char warn_window_requested[] =
" (WARNING: requested %s)";

const char sctp_window_small[] = "\
WARNING: SCTP window size set to %d bytes. A small window size\n\
will give poor performance. See the Iperf documentation.\n";

const char warn_window_small[] = "\
WARNING: TCP window size set to %d bytes. A small window size\n\
will give poor performance. See the Iperf documentation.\n";

const char warn_delay_large[] =
"WARNING: delay too large, reducing from %.1f to 1.0 seconds.\n";

const char warn_no_pathmtu[] =
"WARNING: Path MTU Discovery may not be enabled.\n";

const char warn_no_ack[]=
"[%3d] WARNING: did not receive ack of last datagram after %d tries.\n";

const char warn_ack_failed[]=
"[%3d] WARNING: ack of last datagram failed after %d tries.\n";

const char warn_fileopen_failed[]=
"WARNING: Unable to open file stream for transfer\n\
Using default data stream. \n";

const char unable_to_change_win[]=
"WARNING: Unable to change the window size\n";

const char opt_estimate[]=
"Optimal Estimate\n";

const char report_interval_small[] =
"WARNING: interval too small, increasing from %3.2f to 0.5 seconds.\n";

const char warn_invalid_server_option[] =
"WARNING: option -%c is not valid for server mode\n";

const char warn_invalid_client_option[] =
"WARNING: option -%c is not valid for client mode\n";

const char warn_invalid_compatibility_option[] =
"WARNING: option -%c is not valid in compatibility mode\n";

const char warn_implied_udp[] =
"WARNING: option -%c implies udp testing\n";

const char warn_implied_compatibility[] =
"WARNING: option -%c has implied compatibility mode\n";

const char warn_buffer_too_small[] =
"WARNING: the UDP buffer was increased to %d for proper operation\n";

const char warn_invalid_single_threaded[] =
"WARNING: option -%c is not valid in single threaded versions\n";

const char warn_invalid_report_style[] =
"WARNING: unknown reporting style \"%s\", switching to default\n";

const char warn_invalid_report[] =
"WARNING: unknown reporting type \"%c\"\n";

#ifdef __cplusplus
} /* end extern "C" */
#endif
