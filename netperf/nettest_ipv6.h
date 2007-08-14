/*****************************************************************************

 @(#) $Id: nettest_ipv6.h,v 1.1.1.5 2007/08/14 07:26:49 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

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

 Last Modified $Date: 2007/08/14 07:26:49 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: nettest_ipv6.h,v $
 Revision 1.1.1.5  2007/08/14 07:26:49  brian
 - GPLv3 header update

 *****************************************************************************/

#ifndef __LOCAL_NETTEST_IPV6_H__
#define __LOCAL_NETTEST_IPV6_H__

#ident "@(#) $RCSfile: nettest_ipv6.h,v $ $Name:  $($Revision: 1.1.1.5 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
        Copyright (C) 1993-2004 Hewlett-Packard Company
*/

 /* This file contains the test-specific definitions for netperf's BSD */
 /* sockets tests */

#ifdef DO_IPV6_SCTP
struct sctpipv6_stream_request_struct {
	int send_buf_size;
	int recv_buf_size;		/* how big does the client want it - the */
	/* receive socket buffer that is */
	int receive_size;		/* how many bytes do we want to receive at one */
	/* time? */
	int recv_alignment;		/* what is the alignment of the receive */
	/* buffer? */
	int recv_offset;		/* and at what offset from that alignment? */
	int no_delay;			/* do we disable the nagle algorithm for send */
	/* coalescing? */
	int measure_cpu;		/* does the client want server cpu utilization */
	/* measured? */
	float cpu_rate;			/* do we know how fast the cpu is already? */
	int test_length;		/* how long is the test? */
	int so_rcvavoid;		/* do we want the remote to avoid copies on */
	/* receives? */
	int so_sndavoid;		/* do we want the remote to avoid send copies? */
	int dirty_count;		/* how many integers in the receive buffer */
	/* should be made dirty before calling recv? */
	int clean_count;		/* how many integers should be read from the */
	/* recv buffer before calling recv? */
	int port;			/* the port to which the recv side should bind to allow
					   netperf to run through those evil firewall things */
};

struct sctpipv6_stream_response_struct {
	int recv_buf_size;		/* how big does the client want it */
	int receive_size;
	int no_delay;
	int measure_cpu;		/* does the client want server cpu */
	int test_length;		/* how long is the test? */
	int send_buf_size;
	int data_port_number;		/* connect to me here */
	float cpu_rate;			/* could we measure */
	int so_rcvavoid;		/* could the remote avoid receive copies? */
	int so_sndavoid;		/* could the remote avoid send copies? */
};

struct sctpipv6_stream_results_struct {
	double bytes_received;
	unsigned int recv_calls;
	float elapsed_time;		/* how long the test ran */
	float cpu_util;			/* -1 if not measured */
	float serv_dem;			/* -1 if not measured */
	int cpu_method;			/* how was cpu util measured? */
	int num_cpus;			/* how many CPUs had the remote? */
};

struct sctpipv6_rr_request_struct {
	int recv_buf_size;		/* how big does the client want it */
	int send_buf_size;
	int recv_alignment;
	int recv_offset;
	int send_alignment;
	int send_offset;
	int request_size;
	int response_size;
	int no_delay;
	int measure_cpu;		/* does the client want server cpu */
	float cpu_rate;			/* do we know how fast the cpu is? */
	int test_length;		/* how long is the test? */
	int so_rcvavoid;		/* do we want the remote to avoid receive */
	/* copies? */
	int so_sndavoid;		/* do we want the remote to avoid send copies? */
	int port;			/* the port to which the recv side should bind to allow
					   netperf to run through those evil firewall things */
};

struct sctpipv6_rr_response_struct {
	int recv_buf_size;		/* how big does the client want it */
	int no_delay;
	int measure_cpu;		/* does the client want server cpu */
	int test_length;		/* how long is the test? */
	int send_buf_size;
	int data_port_number;		/* connect to me here */
	float cpu_rate;			/* could we measure */
	int so_rcvavoid;		/* could the remote avoid receive copies? */
	int so_sndavoid;		/* could the remote avoid send copies? */
};

struct sctpipv6_rr_results_struct {
	unsigned int bytes_received;	/* ignored initially */
	unsigned int recv_calls;	/* ignored initially */
	unsigned int trans_received;	/* not ignored */
	float elapsed_time;		/* how long the test ran */
	float cpu_util;			/* -1 if not measured */
	float serv_dem;			/* -1 if not measured */
	int cpu_method;			/* how was cpu util measured? */
	int num_cpus;			/* how many CPUs had the remote? */
};

struct sctpipv6_conn_rr_request_struct {
	int recv_buf_size;		/* how big does the client want it */
	int send_buf_size;
	int recv_alignment;
	int recv_offset;
	int send_alignment;
	int send_offset;
	int request_size;
	int response_size;
	int no_delay;
	int measure_cpu;		/* does the client want server cpu */
	float cpu_rate;			/* do we know how fast the cpu is? */
	int test_length;		/* how long is the test? */
	int so_rcvavoid;		/* do we want the remote to avoid receive */
	/* copies? */
	int so_sndavoid;		/* do we want the remote to avoid send copies? */
};

struct sctpipv6_conn_rr_response_struct {
	int recv_buf_size;		/* how big does the client want it */
	int no_delay;
	int measure_cpu;		/* does the client want server cpu */
	int test_length;		/* how long is the test? */
	int send_buf_size;
	int data_port_number;		/* connect to me here */
	float cpu_rate;			/* could we measure */
	int so_rcvavoid;		/* could the remote avoid receive copies? */
	int so_sndavoid;		/* could the remote avoid send copies? */
	int port;			/* the port to which the recv side should bind to allow
					   netperf to run through those evil firewall things */
};

struct sctpipv6_conn_rr_results_struct {
	unsigned int bytes_received;	/* ignored initially */
	unsigned int recv_calls;	/* ignored initially */
	unsigned int trans_received;	/* not ignored */
	float elapsed_time;		/* how long the test ran */
	float cpu_util;			/* -1 if not measured */
	float serv_dem;			/* -1 if not measured */
	int cpu_method;			/* how was cpu util measured? */
	int num_cpus;			/* how many CPUs had the remote? */
};

struct sctpipv6_tran_rr_request_struct {
	int recv_buf_size;		/* how big does the client want it */
	int send_buf_size;
	int recv_alignment;
	int recv_offset;
	int send_alignment;
	int send_offset;
	int request_size;
	int response_size;
	int no_delay;
	int measure_cpu;		/* does the client want server cpu */
	float cpu_rate;			/* do we know how fast the cpu is? */
	int test_length;		/* how long is the test? */
	int so_rcvavoid;		/* do we want the remote to avoid receive */
	/* copies? */
	int so_sndavoid;		/* do we want the remote to avoid send copies? */
	int port;			/* the port to which the recv side should bind to allow
					   netperf to run through those evil firewall things */
};

struct sctpipv6_tran_rr_response_struct {
	int recv_buf_size;		/* how big does the client want it */
	int no_delay;
	int measure_cpu;		/* does the client want server cpu */
	int test_length;		/* how long is the test? */
	int send_buf_size;
	int data_port_number;		/* connect to me here */
	float cpu_rate;			/* could we measure */
	int so_rcvavoid;		/* could the remote avoid receive copies? */
	int so_sndavoid;		/* could the remote avoid send copies? */
};

struct sctpipv6_tran_rr_results_struct {
	unsigned int bytes_received;	/* ignored initially */
	unsigned int recv_calls;	/* ignored initially */
	unsigned int trans_received;	/* not ignored */
	float elapsed_time;		/* how long the test ran */
	float cpu_util;			/* -1 if not measured */
	float serv_dem;			/* -1 if not measured */
	int cpu_method;			/* how was cpu util measured? */
	int num_cpus;			/* how many CPUs had the remote? */
};

#endif				/* DO_IPV6_SCTP */

struct tcpipv6_stream_request_struct {
	int send_buf_size;
	int recv_buf_size;		/* how big does the client want it - the */
	/* receive socket buffer that is */
	int receive_size;		/* how many bytes do we want to receive at one */
	/* time? */
	int recv_alignment;		/* what is the alignment of the receive */
	/* buffer? */
	int recv_offset;		/* and at what offset from that alignment? */
	int no_delay;			/* do we disable the nagle algorithm for send */
	/* coalescing? */
	int measure_cpu;		/* does the client want server cpu utilization */
	/* measured? */
	float cpu_rate;			/* do we know how fast the cpu is already? */
	int test_length;		/* how long is the test? */
	int so_rcvavoid;		/* do we want the remote to avoid copies on */
	/* receives? */
	int so_sndavoid;		/* do we want the remote to avoid send copies? */
	int dirty_count;		/* how many integers in the receive buffer */
	/* should be made dirty before calling recv? */
	int clean_count;		/* how many integers should be read from the */
	/* recv buffer before calling recv? */
	int port;			/* the port to which the recv side should bind to allow
					   netperf to run through those evil firewall things */
};

struct tcpipv6_stream_response_struct {
	int recv_buf_size;		/* how big does the client want it */
	int receive_size;
	int no_delay;
	int measure_cpu;		/* does the client want server cpu */
	int test_length;		/* how long is the test? */
	int send_buf_size;
	int data_port_number;		/* connect to me here */
	float cpu_rate;			/* could we measure */
	int so_rcvavoid;		/* could the remote avoid receive copies? */
	int so_sndavoid;		/* could the remote avoid send copies? */
};

struct tcpipv6_stream_results_struct {
	double bytes_received;
	unsigned int recv_calls;
	float elapsed_time;		/* how long the test ran */
	float cpu_util;			/* -1 if not measured */
	float serv_dem;			/* -1 if not measured */
	int cpu_method;			/* how was cpu util measured? */
	int num_cpus;			/* how many CPUs had the remote? */
};

struct tcpipv6_rr_request_struct {
	int recv_buf_size;		/* how big does the client want it */
	int send_buf_size;
	int recv_alignment;
	int recv_offset;
	int send_alignment;
	int send_offset;
	int request_size;
	int response_size;
	int no_delay;
	int measure_cpu;		/* does the client want server cpu */
	float cpu_rate;			/* do we know how fast the cpu is? */
	int test_length;		/* how long is the test? */
	int so_rcvavoid;		/* do we want the remote to avoid receive */
	/* copies? */
	int so_sndavoid;		/* do we want the remote to avoid send copies? */
	int port;			/* the port to which the recv side should bind to allow
					   netperf to run through those evil firewall things */
};

struct tcpipv6_rr_response_struct {
	int recv_buf_size;		/* how big does the client want it */
	int no_delay;
	int measure_cpu;		/* does the client want server cpu */
	int test_length;		/* how long is the test? */
	int send_buf_size;
	int data_port_number;		/* connect to me here */
	float cpu_rate;			/* could we measure */
	int so_rcvavoid;		/* could the remote avoid receive copies? */
	int so_sndavoid;		/* could the remote avoid send copies? */
};

struct tcpipv6_rr_results_struct {
	unsigned int bytes_received;	/* ignored initially */
	unsigned int recv_calls;	/* ignored initially */
	unsigned int trans_received;	/* not ignored */
	float elapsed_time;		/* how long the test ran */
	float cpu_util;			/* -1 if not measured */
	float serv_dem;			/* -1 if not measured */
	int cpu_method;			/* how was cpu util measured? */
	int num_cpus;			/* how many CPUs had the remote? */
};

struct tcpipv6_conn_rr_request_struct {
	int recv_buf_size;		/* how big does the client want it */
	int send_buf_size;
	int recv_alignment;
	int recv_offset;
	int send_alignment;
	int send_offset;
	int request_size;
	int response_size;
	int no_delay;
	int measure_cpu;		/* does the client want server cpu */
	float cpu_rate;			/* do we know how fast the cpu is? */
	int test_length;		/* how long is the test? */
	int so_rcvavoid;		/* do we want the remote to avoid receive */
	/* copies? */
	int so_sndavoid;		/* do we want the remote to avoid send copies? */
};

struct tcpipv6_conn_rr_response_struct {
	int recv_buf_size;		/* how big does the client want it */
	int no_delay;
	int measure_cpu;		/* does the client want server cpu */
	int test_length;		/* how long is the test? */
	int send_buf_size;
	int data_port_number;		/* connect to me here */
	float cpu_rate;			/* could we measure */
	int so_rcvavoid;		/* could the remote avoid receive copies? */
	int so_sndavoid;		/* could the remote avoid send copies? */
	int port;			/* the port to which the recv side should bind to allow
					   netperf to run through those evil firewall things */
};

struct tcpipv6_conn_rr_results_struct {
	unsigned int bytes_received;	/* ignored initially */
	unsigned int recv_calls;	/* ignored initially */
	unsigned int trans_received;	/* not ignored */
	float elapsed_time;		/* how long the test ran */
	float cpu_util;			/* -1 if not measured */
	float serv_dem;			/* -1 if not measured */
	int cpu_method;			/* how was cpu util measured? */
	int num_cpus;			/* how many CPUs had the remote? */
};

struct tcpipv6_tran_rr_request_struct {
	int recv_buf_size;		/* how big does the client want it */
	int send_buf_size;
	int recv_alignment;
	int recv_offset;
	int send_alignment;
	int send_offset;
	int request_size;
	int response_size;
	int no_delay;
	int measure_cpu;		/* does the client want server cpu */
	float cpu_rate;			/* do we know how fast the cpu is? */
	int test_length;		/* how long is the test? */
	int so_rcvavoid;		/* do we want the remote to avoid receive */
	/* copies? */
	int so_sndavoid;		/* do we want the remote to avoid send copies? */
	int port;			/* the port to which the recv side should bind to allow
					   netperf to run through those evil firewall things */
};

struct tcpipv6_tran_rr_response_struct {
	int recv_buf_size;		/* how big does the client want it */
	int no_delay;
	int measure_cpu;		/* does the client want server cpu */
	int test_length;		/* how long is the test? */
	int send_buf_size;
	int data_port_number;		/* connect to me here */
	float cpu_rate;			/* could we measure */
	int so_rcvavoid;		/* could the remote avoid receive copies? */
	int so_sndavoid;		/* could the remote avoid send copies? */
};

struct tcpipv6_tran_rr_results_struct {
	unsigned int bytes_received;	/* ignored initially */
	unsigned int recv_calls;	/* ignored initially */
	unsigned int trans_received;	/* not ignored */
	float elapsed_time;		/* how long the test ran */
	float cpu_util;			/* -1 if not measured */
	float serv_dem;			/* -1 if not measured */
	int cpu_method;			/* how was cpu util measured? */
	int num_cpus;			/* how many CPUs had the remote? */
};

struct udpipv6_stream_request_struct {
	int recv_buf_size;
	int message_size;
	int recv_alignment;
	int recv_offset;
	int checksum_off;
	int measure_cpu;
	float cpu_rate;
	int test_length;
	int so_rcvavoid;		/* do we want the remote to avoid receive */
	/* copies? */
	int so_sndavoid;		/* do we want the remote to avoid send copies? */
	int port;			/* the port to which the recv side should bind to allow
					   netperf to run through those evil firewall things */
};

struct udpipv6_stream_response_struct {
	int recv_buf_size;
	int send_buf_size;
	int measure_cpu;
	int test_length;
	int data_port_number;
	float cpu_rate;
	int so_rcvavoid;		/* could the remote avoid receive copies? */
	int so_sndavoid;		/* could the remote avoid send copies? */
};

struct udpipv6_stream_results_struct {
	unsigned int messages_recvd;
	unsigned int bytes_received;
	float elapsed_time;
	float cpu_util;
	int cpu_method;			/* how was cpu util measured? */
	int num_cpus;			/* how many CPUs had the remote? */
};

struct udpipv6_rr_request_struct {
	int recv_buf_size;		/* how big does the client want it */
	int send_buf_size;
	int recv_alignment;
	int recv_offset;
	int send_alignment;
	int send_offset;
	int request_size;
	int response_size;
	int no_delay;
	int measure_cpu;		/* does the client want server cpu */
	float cpu_rate;			/* do we know how fast the cpu is? */
	int test_length;		/* how long is the test? */
	int so_rcvavoid;		/* do we want the remote to avoid receive */
	/* copies? */
	int so_sndavoid;		/* do we want the remote to avoid send copies? */
	int port;			/* the port to which the recv side should bind to allow
					   netperf to run through those evil firewall things */
};

struct udpipv6_rr_response_struct {
	int recv_buf_size;		/* how big does the client want it */
	int no_delay;
	int measure_cpu;		/* does the client want server cpu */
	int test_length;		/* how long is the test? */
	int send_buf_size;
	int data_port_number;		/* connect to me here */
	float cpu_rate;			/* could we measure */
	int so_rcvavoid;		/* could the remote avoid receive copies? */
	int so_sndavoid;		/* could the remote avoid send copies? */
};

struct udpipv6_rr_results_struct {
	unsigned int bytes_received;	/* ignored initially */
	unsigned int recv_calls;	/* ignored initially */
	unsigned int trans_received;	/* not ignored */
	float elapsed_time;		/* how long the test ran */
	float cpu_util;			/* -1 if not measured */
	float serv_dem;			/* -1 if not measured */
	int cpu_method;			/* how was cpu util measured? */
	int num_cpus;			/* how many CPUs had the remote? */
};

extern void scan_ipv6_args(int argc, char *argv[]);

//+*+extern void scan_sockets_args(int argc, char *argv[]);  // included by nettest_bsd.h

#ifdef DO_IPV6_SCTP
extern void send_sctpipv6_stream(char remote_host[]);
extern void send_sctpipv6_rr(char remote_host[]);
extern void send_sctpipv6_conn_rr(char remote_host[]);
#endif				/* DO_IPV6_SCTP */
extern void send_tcpipv6_stream(char remote_host[]);
extern void send_tcpipv6_rr(char remote_host[]);
extern void send_tcpipv6_conn_rr(char remote_host[]);
extern void send_udpipv6_stream(char remote_host[]);
extern void send_udpipv6_rr(char remote_host[]);

#ifdef DO_IPV6_SCTP
extern void recv_sctpipv6_stream();
extern void recv_sctpipv6_rr();
extern void recv_sctpipv6_conn_rr();
#endif				/* DO_IPV6_SCTP */
extern void recv_tcpipv6_stream();
extern void recv_tcpipv6_rr();
extern void recv_tcpipv6_conn_rr();
extern void recv_udpipv6_stream();
extern void recv_udpipv6_rr();

#endif				/* __LOCAL_NETTEST_IPV6_H__ */
