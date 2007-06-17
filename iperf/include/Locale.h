/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

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
 * Locale.h
 * by Ajay Tirumala <tirumala@ncsa.uiuc.edu>
 * & Mark Gates <mgates@nlanr.net>
 * -------------------------------------------------------------------
 * Strings and other stuff that is locale specific.
 * ------------------------------------------------------------------- */

#ifndef LOCALE_H
#define LOCALE_H

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

#ifdef __cplusplus
extern "C" {
#endif
/* -------------------------------------------------------------------
 * usage
 * ------------------------------------------------------------------- */

extern const char usage_short[];

#ifdef WIN32
extern const char usage_long1[];
extern const char usage_long2[];
#else
extern const char usage_long[];
#endif

extern const char version[];
extern const char copying[];

/* -------------------------------------------------------------------
 * settings
 * ------------------------------------------------------------------- */

extern const char seperator_line[];

extern const char server_port[];

extern const char client_port[];

extern const char bind_address[];

extern const char multicast_ttl[];

extern const char join_multicast[];

extern const char client_datagram_size[];

extern const char server_datagram_size[];

extern const char sctp_window_size[];

extern const char tcp_window_size[];

extern const char udp_buffer_size[];

extern const char window_default[];

extern const char wait_server_threads[];

/* -------------------------------------------------------------------
 * reports
 * ------------------------------------------------------------------- */

extern const char report_read_lengths[];

extern const char report_read_length_times[];

extern const char report_bw_header[];

extern const char report_bw_format[];

extern const char report_sum_bw_format[];

extern const char report_bw_jitter_loss_header[];

extern const char report_bw_jitter_loss_format[];

extern const char report_sum_bw_jitter_loss_format[];

extern const char report_outoforder[];

extern const char report_sum_outoforder[];

extern const char report_peer[];

extern const char report_mss_unsupported[];

extern const char report_mss[];

extern const char report_datagrams[];

extern const char report_sum_datagrams[];

extern const char server_reporting[];

extern const char reportCSV_peer[];

extern const char reportCSV_bw_format[];

extern const char reportCSV_bw_jitter_loss_format[];

/* -------------------------------------------------------------------
 * warnings
 * ------------------------------------------------------------------- */

extern const char warn_window_requested[];

extern const char sctp_window_small[];

extern const char warn_window_small[];

extern const char warn_delay_large[];

extern const char warn_no_pathmtu[];

extern const char warn_no_ack[];

extern const char warn_ack_failed[];

extern const char warn_fileopen_failed[];

extern const char unable_to_change_win[];

extern const char opt_estimate[];

extern const char report_interval_small[];

extern const char warn_invalid_server_option[];

extern const char warn_invalid_client_option[];

extern const char warn_invalid_compatibility_option[];

extern const char warn_implied_udp[];

extern const char warn_implied_compatibility[];

extern const char warn_buffer_too_small[];

extern const char warn_invalid_single_threaded[];

extern const char warn_invalid_report_style[];

extern const char warn_invalid_report[];

#ifdef __cplusplus
} /* end extern "C" */
#endif
#endif // LOCALE_H










