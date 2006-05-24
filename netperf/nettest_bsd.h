/*****************************************************************************

 @(#) $Id: nettest_bsd.h,v 1.1.1.6 2006/05/23 22:43:32 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2006/05/23 22:43:32 $ by $Author: brian $

 *****************************************************************************/

#ifndef __LOCAL_NETTEST_BSD_H__
#define __LOCAL_NETTEST_BSD_H__

/*
        Copyright (C) 1993-2004 Hewlett-Packard Company
*/

 /* This file contains the test-specific definitions for netperf's BSD */
 /* sockets tests */

#ifdef DO_SCTP
struct	sctp_stream_request_struct {
  int	send_buf_size;
  int	recv_buf_size;	/* how big does the client want it - the */
			/* receive socket buffer that is */ 
  int	receive_size;   /* how many bytes do we want to receive at one */
			/* time? */ 
  int	recv_alignment; /* what is the alignment of the receive */
			/* buffer? */ 
  int	recv_offset;    /* and at what offset from that alignment? */ 
  int	no_delay;       /* do we disable the nagle algorithm for send */
			/* coalescing? */ 
  int	measure_cpu;	/* does the client want server cpu utilization */
			/* measured? */ 
  float	cpu_rate;	/* do we know how fast the cpu is already? */ 
  int	test_length;	/* how long is the test?		*/
  int	so_rcvavoid;    /* do we want the remote to avoid copies on */
			/* receives? */ 
  int	so_sndavoid;    /* do we want the remote to avoid send copies? */
  int   dirty_count;    /* how many integers in the receive buffer */
			/* should be made dirty before calling recv? */  
  int   clean_count;    /* how many integers should be read from the */
			/* recv buffer before calling recv? */ 
  int   port;           /* the port to which the recv side should bind
			   to allow netperf to run through those evil
			   firewall things */
  int   ipaddress;      /* the IP address to which the recv side
			   should bind to allow netperf to run through
			   those evil firewall things */
};

struct	sctp_stream_response_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	receive_size;
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  int	test_length;	/* how long is the test?		*/
  int	send_buf_size;
  int	data_port_number;	/* connect to me here	*/
  float	cpu_rate;		/* could we measure	*/
  int	so_rcvavoid;	/* could the remote avoid receive copies? */ 
  int	so_sndavoid;	/* could the remote avoid send copies? */
};

struct sctp_stream_results_struct {
  double         bytes_received;
  unsigned int	 recv_calls;	
  float	         elapsed_time;	/* how long the test ran */
  float	         cpu_util;	/* -1 if not measured */
  float	         serv_dem;	/* -1 if not measured */
  int            cpu_method;    /* how was cpu util measured? */
  int            num_cpus;      /* how many CPUs had the remote? */
};

struct	sctp_maerts_request_struct {
  int	send_buf_size;
  int	recv_buf_size;	/* how big does the client want it - the */
			/* receive socket buffer that is */ 
  int	send_size;      /* how many bytes do we want netserver to send
			   at one time? */
  int	send_alignment; /* what is the alignment of the send */
			/* buffer? */ 
  int	send_offset;    /* and at what offset from that alignment? */ 
  int	no_delay;       /* do we disable the nagle algorithm for send */
			/* coalescing? */ 
  int	measure_cpu;	/* does the client want server cpu utilization */
			/* measured? */ 
  float	cpu_rate;	/* do we know how fast the cpu is already? */ 
  int	test_length;	/* how long is the test?		*/
  int	so_rcvavoid;    /* do we want the remote to avoid copies on */
			/* receives? */ 
  int	so_sndavoid;    /* do we want the remote to avoid send copies? */
  int   dirty_count;    /* how many integers in the send buffer */
			/* should be made dirty before calling recv? */  
  int   clean_count;    /* how many integers should be read from the */
			/* recv buffer before calling recv? */ 
  int   port;           /* the port to which the recv side should bind
			   to allow netperf to run through those evil
			   firewall things */
  int   ipaddress;      /* the IP address to which the recv side
			   should bind to allow netperf to run through
			   those evil firewall things */
};

struct	sctp_maerts_response_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	send_size;
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  int	test_length;	/* how long is the test?		*/
  int	send_buf_size;
  int	data_port_number;	/* connect to me here	*/
  float	cpu_rate;		/* could we measure	*/
  int	so_rcvavoid;	/* could the remote avoid receive copies? */ 
  int	so_sndavoid;	/* could the remote avoid send copies? */
};

struct sctp_maerts_results_struct {
  double         bytes_sent;
  unsigned int	 send_calls;	
  float	         elapsed_time;	/* how long the test ran */
  float	         cpu_util;	/* -1 if not measured */
  float	         serv_dem;	/* -1 if not measured */
  int            cpu_method;    /* how was cpu util measured? */
  int            num_cpus;      /* how many CPUs had the remote? */
};

struct	sctp_rr_request_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	send_buf_size;
  int	recv_alignment;
  int	recv_offset;
  int	send_alignment;
  int	send_offset;
  int	request_size;
  int	response_size;
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  float	cpu_rate;	/* do we know how fast the cpu is?	*/
  int	test_length;	/* how long is the test?		*/
  int	so_rcvavoid;    /* do we want the remote to avoid receive */
			/* copies? */ 
  int	so_sndavoid;    /* do we want the remote to avoid send copies? */
  int   port;           /* the port to which the recv side should bind
			   to allow netperf to run through those evil
			   firewall things */
  int   ipaddress;      /* the IP address to which the recv side
			   should bind to allow netperf to run through
			   those evil firewall things */
};

struct	sctp_rr_response_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  int	test_length;	/* how long is the test?		*/
  int	send_buf_size;
  int	data_port_number;	/* connect to me here	*/
  float	cpu_rate;		/* could we measure	*/
  int	so_rcvavoid;	/* could the remote avoid receive copies? */
  int	so_sndavoid;	/* could the remote avoid send copies? */
};

struct sctp_rr_results_struct {
  unsigned int  bytes_received;	/* ignored initially */
  unsigned int	recv_calls;	/* ignored initially */
  unsigned int	trans_received;	/* not ignored  */
  float	        elapsed_time;	/* how long the test ran */
  float	        cpu_util;	/* -1 if not measured */
  float	        serv_dem;	/* -1 if not measured */
  int           cpu_method;    /* how was cpu util measured? */
  int           num_cpus;      /* how many CPUs had the remote? */
};

struct	sctp_conn_rr_request_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	send_buf_size;
  int	recv_alignment;
  int	recv_offset;
  int	send_alignment;
  int	send_offset;
  int	request_size;
  int	response_size;
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  float	cpu_rate;	/* do we know how fast the cpu is?	*/
  int	test_length;	/* how long is the test?		*/
  int	so_rcvavoid;    /* do we want the remote to avoid receive */
			/* copies? */ 
  int	so_sndavoid;    /* do we want the remote to avoid send copies? */
  int   port;           /* the port to which the recv side should bind
			   to allow netperf to run through those evil
			   firewall things */
  int   ipaddress;      /* the IP address to which the recv side
			   should bind to allow netperf to run through
			   those evil firewall things */
};


struct	sctp_conn_rr_response_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  int	test_length;	/* how long is the test?		*/
  int	send_buf_size;
  int	data_port_number;	/* connect to me here	*/
  float	cpu_rate;		/* could we measure	*/
  int	so_rcvavoid;	/* could the remote avoid receive copies? */
  int	so_sndavoid;	/* could the remote avoid send copies? */
};

struct sctp_conn_rr_results_struct {
  unsigned int	bytes_received;	/* ignored initially */
  unsigned int	recv_calls;	/* ignored initially */
  unsigned int	trans_received;	/* not ignored  */
  float	        elapsed_time;	/* how long the test ran */
  float	        cpu_util;	/* -1 if not measured */
  float	        serv_dem;	/* -1 if not measured */
  int           cpu_method;    /* how was cpu util measured? */
  int           num_cpus;      /* how many CPUs had the remote? */
};

struct	sctp_tran_rr_request_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	send_buf_size;
  int	recv_alignment;
  int	recv_offset;
  int	send_alignment;
  int	send_offset;
  int	request_size;
  int	response_size;
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  float	cpu_rate;	/* do we know how fast the cpu is?	*/
  int	test_length;	/* how long is the test?		*/
  int	so_rcvavoid;    /* do we want the remote to avoid receive */
			/* copies? */ 
  int	so_sndavoid;    /* do we want the remote to avoid send copies? */
  int   port;           /* the port to which the recv side should bind
			   to allow netperf to run through those evil
			   firewall things */
  int   ipaddress;      /* the IP address to which the recv side
			   should bind to allow netperf to run through
			   those evil firewall things */
};


struct	sctp_tran_rr_response_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  int	test_length;	/* how long is the test?		*/
  int	send_buf_size;
  int	data_port_number;	/* connect to me here	*/
  float	cpu_rate;		/* could we measure	*/
  int	so_rcvavoid;	/* could the remote avoid receive copies? */
  int	so_sndavoid;	/* could the remote avoid send copies? */
};

struct sctp_tran_rr_results_struct {
  unsigned int	bytes_received;	/* ignored initially */
  unsigned int	recv_calls;	/* ignored initially */
  unsigned int	trans_received;	/* not ignored  */
  float	        elapsed_time;	/* how long the test ran */
  float	        cpu_util;	/* -1 if not measured */
  float	        serv_dem;	/* -1 if not measured */
  int           cpu_method;    /* how was cpu util measured? */
  int           num_cpus;      /* how many CPUs had the remote? */

};
#endif				/* DO_SCTP */

struct	tcp_stream_request_struct {
  int	send_buf_size;
  int	recv_buf_size;	/* how big does the client want it - the */
			/* receive socket buffer that is */ 
  int	receive_size;   /* how many bytes do we want to receive at one */
			/* time? */ 
  int	recv_alignment; /* what is the alignment of the receive */
			/* buffer? */ 
  int	recv_offset;    /* and at what offset from that alignment? */ 
  int	no_delay;       /* do we disable the nagle algorithm for send */
			/* coalescing? */ 
  int	measure_cpu;	/* does the client want server cpu utilization */
			/* measured? */ 
  float	cpu_rate;	/* do we know how fast the cpu is already? */ 
  int	test_length;	/* how long is the test?		*/
  int	so_rcvavoid;    /* do we want the remote to avoid copies on */
			/* receives? */ 
  int	so_sndavoid;    /* do we want the remote to avoid send copies? */
  int   dirty_count;    /* how many integers in the receive buffer */
			/* should be made dirty before calling recv? */  
  int   clean_count;    /* how many integers should be read from the */
			/* recv buffer before calling recv? */ 
  int   port;           /* the port to which the recv side should bind
			   to allow netperf to run through those evil
			   firewall things */
  int   ipaddress;      /* the IP address to which the recv side
			   should bind to allow netperf to run through
			   those evil firewall things */
};

struct	tcp_stream_response_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	receive_size;
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  int	test_length;	/* how long is the test?		*/
  int	send_buf_size;
  int	data_port_number;	/* connect to me here	*/
  float	cpu_rate;		/* could we measure	*/
  int	so_rcvavoid;	/* could the remote avoid receive copies? */ 
  int	so_sndavoid;	/* could the remote avoid send copies? */
};

struct tcp_stream_results_struct {
  double         bytes_received;
  unsigned int	 recv_calls;	
  float	         elapsed_time;	/* how long the test ran */
  float	         cpu_util;	/* -1 if not measured */
  float	         serv_dem;	/* -1 if not measured */
  int            cpu_method;    /* how was cpu util measured? */
  int            num_cpus;      /* how many CPUs had the remote? */
};

struct	tcp_maerts_request_struct {
  int	send_buf_size;
  int	recv_buf_size;	/* how big does the client want it - the */
			/* receive socket buffer that is */ 
  int	send_size;      /* how many bytes do we want netserver to send
			   at one time? */
  int	send_alignment; /* what is the alignment of the send */
			/* buffer? */ 
  int	send_offset;    /* and at what offset from that alignment? */ 
  int	no_delay;       /* do we disable the nagle algorithm for send */
			/* coalescing? */ 
  int	measure_cpu;	/* does the client want server cpu utilization */
			/* measured? */ 
  float	cpu_rate;	/* do we know how fast the cpu is already? */ 
  int	test_length;	/* how long is the test?		*/
  int	so_rcvavoid;    /* do we want the remote to avoid copies on */
			/* receives? */ 
  int	so_sndavoid;    /* do we want the remote to avoid send copies? */
  int   dirty_count;    /* how many integers in the send buffer */
			/* should be made dirty before calling recv? */  
  int   clean_count;    /* how many integers should be read from the */
			/* recv buffer before calling recv? */ 
  int   port;           /* the port to which the recv side should bind
			   to allow netperf to run through those evil
			   firewall things */
  int   ipaddress;      /* the IP address to which the recv side
			   should bind to allow netperf to run through
			   those evil firewall things */
};

struct	tcp_maerts_response_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	send_size;
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  int	test_length;	/* how long is the test?		*/
  int	send_buf_size;
  int	data_port_number;	/* connect to me here	*/
  float	cpu_rate;		/* could we measure	*/
  int	so_rcvavoid;	/* could the remote avoid receive copies? */ 
  int	so_sndavoid;	/* could the remote avoid send copies? */
};

struct tcp_maerts_results_struct {
  double         bytes_sent;
  unsigned int	 send_calls;	
  float	         elapsed_time;	/* how long the test ran */
  float	         cpu_util;	/* -1 if not measured */
  float	         serv_dem;	/* -1 if not measured */
  int            cpu_method;    /* how was cpu util measured? */
  int            num_cpus;      /* how many CPUs had the remote? */
};

struct	tcp_rr_request_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	send_buf_size;
  int	recv_alignment;
  int	recv_offset;
  int	send_alignment;
  int	send_offset;
  int	request_size;
  int	response_size;
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  float	cpu_rate;	/* do we know how fast the cpu is?	*/
  int	test_length;	/* how long is the test?		*/
  int	so_rcvavoid;    /* do we want the remote to avoid receive */
			/* copies? */ 
  int	so_sndavoid;    /* do we want the remote to avoid send copies? */
  int   port;           /* the port to which the recv side should bind
			   to allow netperf to run through those evil
			   firewall things */
  int   ipaddress;      /* the IP address to which the recv side
			   should bind to allow netperf to run through
			   those evil firewall things */
};

struct	tcp_rr_response_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  int	test_length;	/* how long is the test?		*/
  int	send_buf_size;
  int	data_port_number;	/* connect to me here	*/
  float	cpu_rate;		/* could we measure	*/
  int	so_rcvavoid;	/* could the remote avoid receive copies? */
  int	so_sndavoid;	/* could the remote avoid send copies? */
};

struct tcp_rr_results_struct {
  unsigned int  bytes_received;	/* ignored initially */
  unsigned int	recv_calls;	/* ignored initially */
  unsigned int	trans_received;	/* not ignored  */
  float	        elapsed_time;	/* how long the test ran */
  float	        cpu_util;	/* -1 if not measured */
  float	        serv_dem;	/* -1 if not measured */
  int           cpu_method;    /* how was cpu util measured? */
  int           num_cpus;      /* how many CPUs had the remote? */
};

struct	tcp_conn_rr_request_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	send_buf_size;
  int	recv_alignment;
  int	recv_offset;
  int	send_alignment;
  int	send_offset;
  int	request_size;
  int	response_size;
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  float	cpu_rate;	/* do we know how fast the cpu is?	*/
  int	test_length;	/* how long is the test?		*/
  int	so_rcvavoid;    /* do we want the remote to avoid receive */
			/* copies? */ 
  int	so_sndavoid;    /* do we want the remote to avoid send copies? */
  int   port;           /* the port to which the recv side should bind
			   to allow netperf to run through those evil
			   firewall things */
  int   ipaddress;      /* the IP address to which the recv side
			   should bind to allow netperf to run through
			   those evil firewall things */
};


struct	tcp_conn_rr_response_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  int	test_length;	/* how long is the test?		*/
  int	send_buf_size;
  int	data_port_number;	/* connect to me here	*/
  float	cpu_rate;		/* could we measure	*/
  int	so_rcvavoid;	/* could the remote avoid receive copies? */
  int	so_sndavoid;	/* could the remote avoid send copies? */
};

struct tcp_conn_rr_results_struct {
  unsigned int	bytes_received;	/* ignored initially */
  unsigned int	recv_calls;	/* ignored initially */
  unsigned int	trans_received;	/* not ignored  */
  float	        elapsed_time;	/* how long the test ran */
  float	        cpu_util;	/* -1 if not measured */
  float	        serv_dem;	/* -1 if not measured */
  int           cpu_method;    /* how was cpu util measured? */
  int           num_cpus;      /* how many CPUs had the remote? */
};

struct	tcp_tran_rr_request_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	send_buf_size;
  int	recv_alignment;
  int	recv_offset;
  int	send_alignment;
  int	send_offset;
  int	request_size;
  int	response_size;
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  float	cpu_rate;	/* do we know how fast the cpu is?	*/
  int	test_length;	/* how long is the test?		*/
  int	so_rcvavoid;    /* do we want the remote to avoid receive */
			/* copies? */ 
  int	so_sndavoid;    /* do we want the remote to avoid send copies? */
  int   port;           /* the port to which the recv side should bind
			   to allow netperf to run through those evil
			   firewall things */
  int   ipaddress;      /* the IP address to which the recv side
			   should bind to allow netperf to run through
			   those evil firewall things */
};


struct	tcp_tran_rr_response_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  int	test_length;	/* how long is the test?		*/
  int	send_buf_size;
  int	data_port_number;	/* connect to me here	*/
  float	cpu_rate;		/* could we measure	*/
  int	so_rcvavoid;	/* could the remote avoid receive copies? */
  int	so_sndavoid;	/* could the remote avoid send copies? */
};

struct tcp_tran_rr_results_struct {
  unsigned int	bytes_received;	/* ignored initially */
  unsigned int	recv_calls;	/* ignored initially */
  unsigned int	trans_received;	/* not ignored  */
  float	        elapsed_time;	/* how long the test ran */
  float	        cpu_util;	/* -1 if not measured */
  float	        serv_dem;	/* -1 if not measured */
  int           cpu_method;    /* how was cpu util measured? */
  int           num_cpus;      /* how many CPUs had the remote? */

};

struct	udp_stream_request_struct {
  int	recv_buf_size;
  int	message_size;
  int	recv_alignment;
  int	recv_offset;
  int	checksum_off;
  int	measure_cpu;
  float	cpu_rate;
  int	test_length;
  int	so_rcvavoid;    /* do we want the remote to avoid receive */
			/* copies? */ 
  int	so_sndavoid;    /* do we want the remote to avoid send copies? */
  int   port;           /* the port to which the recv side should bind
			   to allow netperf to run through those evil
			   firewall things */
  int   ipaddress;      /* the IP address to which the recv side
			   should bind to allow netperf to run through
			   those evil firewall things */
};

struct	udp_stream_response_struct {
  int	recv_buf_size;
  int	send_buf_size;
  int	measure_cpu;
  int	test_length;
  int	data_port_number;
  float	cpu_rate;
  int	so_rcvavoid;	/* could the remote avoid receive copies? */
  int	so_sndavoid;	/* could the remote avoid send copies? */
};

struct	udp_stream_results_struct {
  unsigned int	messages_recvd;
  unsigned int	bytes_received;
  float	        elapsed_time;
  float	        cpu_util;
  int           cpu_method;    /* how was cpu util measured? */
  int           num_cpus;      /* how many CPUs had the remote? */
};


struct	udp_rr_request_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	send_buf_size;
  int	recv_alignment;
  int	recv_offset;
  int	send_alignment;
  int	send_offset;
  int	request_size;
  int	response_size;
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  float	cpu_rate;	/* do we know how fast the cpu is?	*/
  int	test_length;	/* how long is the test?		*/
  int	so_rcvavoid;    /* do we want the remote to avoid receive */
			/* copies? */ 
  int	so_sndavoid;    /* do we want the remote to avoid send copies? */
  int   port;           /* the port to which the recv side should bind
			   to allow netperf to run through those evil
			   firewall things */
  int   ipaddress;      /* the IP address to which the recv side
			   should bind to allow netperf to run through
			   those evil firewall things */
};

struct	udp_rr_response_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  int	test_length;	/* how long is the test?		*/
  int	send_buf_size;
  int	data_port_number;	/* connect to me here	*/
  float	cpu_rate;		/* could we measure	*/
  int	so_rcvavoid;	/* could the remote avoid receive copies? */
  int	so_sndavoid;	/* could the remote avoid send copies? */
};

struct udp_rr_results_struct {
  unsigned int	bytes_received;	/* ignored initially */
  unsigned int	recv_calls;	/* ignored initially */
  unsigned int	trans_received;	/* not ignored  */
  float	        elapsed_time;	/* how long the test ran */
  float	        cpu_util;	/* -1 if not measured */
  float	        serv_dem;	/* -1 if not measured */
  int           cpu_method;    /* how was cpu util measured? */
  int           num_cpus;      /* how many CPUs had the remote? */
};

#ifdef DO_SCTP
struct	sctp_cc_request_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	send_buf_size;
  int	recv_alignment;
  int	recv_offset;
  int	send_alignment;
  int	send_offset;
  int	request_size;
  int	response_size;
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  float	cpu_rate;	/* do we know how fast the cpu is?	*/
  int	test_length;	/* how long is the test?		*/
  int	so_rcvavoid;    /* do we want the remote to avoid receive */
			/* copies? */ 
  int	so_sndavoid;    /* do we want the remote to avoid send copies? */
  int   port;           /* the port to which the recv side should bind
			   to allow netperf to run through those evil
			   firewall things */
  int   ipaddress;      /* the IP address to which the recv side
			   should bind to allow netperf to run through
			   those evil firewall things */
};


struct	sctp_cc_response_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  int	test_length;	/* how long is the test?		*/
  int	send_buf_size;
  int	data_port_number;	/* connect to me here	*/
  float	cpu_rate;		/* could we measure	*/
  int	so_rcvavoid;	/* could the remote avoid receive copies? */
  int	so_sndavoid;	/* could the remote avoid send copies? */
};

struct sctp_cc_results_struct {
  unsigned int	bytes_received;	/* ignored initially */
  unsigned int	recv_calls;	/* ignored initially */
  unsigned int	trans_received;	/* not ignored  */
  float	        elapsed_time;	/* how long the test ran */
  float	        cpu_util;	/* -1 if not measured */
  float	        serv_dem;	/* -1 if not measured */
  int           cpu_method;    /* how was cpu util measured? */
  int           num_cpus;      /* how many CPUs had the remote? */
};
#endif				/* DO_SCTP */

struct	tcp_cc_request_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	send_buf_size;
  int	recv_alignment;
  int	recv_offset;
  int	send_alignment;
  int	send_offset;
  int	request_size;
  int	response_size;
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  float	cpu_rate;	/* do we know how fast the cpu is?	*/
  int	test_length;	/* how long is the test?		*/
  int	so_rcvavoid;    /* do we want the remote to avoid receive */
			/* copies? */ 
  int	so_sndavoid;    /* do we want the remote to avoid send copies? */
  int   port;           /* the port to which the recv side should bind
			   to allow netperf to run through those evil
			   firewall things */
  int   ipaddress;      /* the IP address to which the recv side
			   should bind to allow netperf to run through
			   those evil firewall things */
};


struct	tcp_cc_response_struct {
  int	recv_buf_size;	/* how big does the client want it	*/
  int	no_delay;
  int	measure_cpu;	/* does the client want server cpu	*/
  int	test_length;	/* how long is the test?		*/
  int	send_buf_size;
  int	data_port_number;	/* connect to me here	*/
  float	cpu_rate;		/* could we measure	*/
  int	so_rcvavoid;	/* could the remote avoid receive copies? */
  int	so_sndavoid;	/* could the remote avoid send copies? */
};

struct tcp_cc_results_struct {
  unsigned int	bytes_received;	/* ignored initially */
  unsigned int	recv_calls;	/* ignored initially */
  unsigned int	trans_received;	/* not ignored  */
  float	        elapsed_time;	/* how long the test ran */
  float	        cpu_util;	/* -1 if not measured */
  float	        serv_dem;	/* -1 if not measured */
  int           cpu_method;    /* how was cpu util measured? */
  int           num_cpus;      /* how many CPUs had the remote? */
};

extern void scan_sockets_args(int argc, char *argv[]);

#ifdef DO_SCTP
extern void send_sctp_stream(char remote_host[]);
extern void send_sctp_maerts(char remote_host[]);
extern void send_sctp_rr(char remote_host[]);
extern void send_sctp_conn_rr(char remote_host[]);
extern void send_sctp_cc(char remote_host[]);
#ifdef DO_1644
extern void send_sctp_tran_rr(char remote_host[]);
#endif				/* DO_1644 */
#ifdef DO_NBRR
extern void send_sctp_nbrr(char remote_host[]);
#endif				/* DO_NBRR */
#endif				/* DO_SCTP */
extern void send_tcp_stream(char remote_host[]);
extern void send_tcp_maerts(char remote_host[]);
extern void send_tcp_rr(char remote_host[]);
extern void send_tcp_conn_rr(char remote_host[]);
extern void send_tcp_cc(char remote_host[]);
#ifdef DO_1644
extern void send_tcp_tran_rr(char remote_host[]);;
#endif
#ifdef DO_NBRR
extern void send_tcp_nbrr(char remote_host[]);
#endif				/* DO_NBRR */
extern void send_udp_stream(char remote_host[]);
extern void send_udp_rr(char remote_host[]);

#ifdef DO_SCTP
extern void recv_sctp_stream(void);
extern void recv_sctp_maerts(void);
extern void recv_sctp_rr(void);
extern void recv_sctp_conn_rr(void);
extern void recv_sctp_cc(void);
#ifdef DO_1644
extern void recv_sctp_tran_rr(void);
#endif				/* DO_1644 */
#ifdef DO_NBRR
extern void recv_sctp_nbrr(void);
#endif				/* DO_NBRR */
#endif				/* DO_SCTP */
extern void recv_tcp_stream(void);
extern void recv_tcp_maerts(void);
extern void recv_tcp_rr(void);
extern void recv_tcp_conn_rr(void);
extern void recv_tcp_cc(void);
#ifdef DO_1644
extern void recv_tcp_tran_rr(void);
#endif
#ifdef DO_NBRR
extern void recv_tcp_nbrr(void);
#endif				/* DO_NBRR */
extern void recv_udp_stream(void);
extern void recv_udp_rr(void);

extern void loc_cpu_rate(void);
extern void rem_cpu_rate(void);

#ifdef HAVE_SENDFILE
#ifdef DO_SCTP
extern void sendfile_sctp_stream(char remotehost[]);
#endif				/* DO_SCTP */
extern void sendfile_tcp_stream(char remotehost[]);
#endif /* HAVE_SENDFILE */

#endif				/* __LOCAL_NETTEST_BSD_H__ */
